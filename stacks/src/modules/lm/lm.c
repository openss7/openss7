/*****************************************************************************

 @(#) $RCSfile: lm.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:44 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/21 10:14:44 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: lm.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:44 $"

static char const ident[] =
    "$RCSfile: lm.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:44 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/stropts.h>

#include "debug.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>

#include "lm/lm.h"

#define DEBUG_LEVEL 2

/* 
 *  -----------------------------------------------------------------------
 *
 *  LMI PRIMITIVES
 *
 *  -----------------------------------------------------------------------
 */

static inline mblk_t *lmi_reuseb(mblk_t * mp, size_t room)
{
	mblk_t *m2;
	if (mp && mp->b_datap->db_ref < 2 && mp->b_datap->db_lim - mp->b_datap->db_base >= room) {
		mp->b_rptr = mp->b_datap->db_base;
		mp->b_wptr = mp->b_rptr + room;
		if (mp->b_cont && (m2 = unlinkb(mp)))
			freemsg(m2);
		return mp;
	}
	if ((m2 = allocb(room, BPRI_HI))) {
		m2->b_wptr += room;
		if (!mp)
			return m2;
		m2->b_datap->db_type = mp->b_datap->db_type;
		freemsg(mp);
		return m2;
	}
	if (mp)
		freemsg(mp);
	return NULL;
}

/* 
 *  =======================================================================
 *
 *  LMI PRIMITIVES
 *
 *  =======================================================================
 */

/* Upstream Primitives */

static inline int lmi_error_ind(lmi_t * lmi, mblk_t ** mpp, int err)
{
	if ((*mpp = lmi_reuseb(*mpp, LMI_ERROR_IND_SIZE))) {
		lmi_error_ind_t *p = (lmi_error_ind_t *) (*mpp)->b_rptr;
		(*mpp)->b_datap->db_type = M_PCPROTO;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_state = lmi->state;
		p->lmi_errno = err & 0x0000ffff;
		p->lmi_reason = err & 0xffff0000;
		qreply(lmi->wq, *mpp);
		*mpp = NULL;
	}
	return (0);
}

static inline int lmi_error_ack(lmi_t * lmi, mblk_t ** mpp, int prim, int err)
{
	if ((*mpp = lmi_reuseb(*mpp, LMI_ERROR_ACK_SIZE))) {
		lmi_error_ack_t *p = (lmi_error_ack_t *) (*mpp)->b_rptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_state = lmi->state;
		p->lmi_error_primitive = prim;
		p->lmi_errno = err & 0x0000ffff;
		p->lmi_reason = err & 0xffff0000;
		qreply(lmi->wq, *mpp);
		*mpp = NULL;
	}
	return (0);
}

static inline int lmi_ok_ack(lmi_t * lmi, mblk_t ** mpp, int prim, int err)
{
	if (err)
		return lmi_error_ack(lmi, mpp, prim, err);
	if ((*mpp = lmi_reuseb(*mpp, LMI_OK_ACK_SIZE))) {
		lmi_ok_ack_t *p = (lmi_ok_ack_t *) (*mpp)->b_rptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_state = lmi->state;
		p->lmi_correct_primitive = prim;
		qreply(lmi->wq, *mpp);
		*mpp = NULL;
	}
	return (0);
}

static inline int lmi_info_ack(lmi_t * lmi, mblk_t ** mpp, int prim, void *ppa, int len, int err)
{
	if (err)
		return lmi_error_ack(lmi, mpp, prim, err);
	if ((*mpp = lmi_reuseb(*mpp, LMI_INFO_ACK_SIZE + len))) {
		lmi_info_ack_t *p = (lmi_info_ack_t *) (*mpp)->b_rptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 0x00070200;
		p->lmi_state = lmi->state;
		p->lmi_max_sdu = 0xffffffff;
		p->lmi_min_sdu = 0;
		p->lmi_header_len = 0;
		ptrace(("ppa len = %d, ppa addr = 0x%08x\n", len, (unsigned int) ppa));
		if (len) {
			p->lmi_ppa_style = LMI_STYLE2;
			bcopy(ppa, p->lmi_ppa_addr, len);
		} else {
			p->lmi_ppa_style = LMI_STYLE1;
		}
		qreply(lmi->wq, *mpp);
	}
	return (0);
}

static inline int lmi_enable_con(lmi_t * lmi, mblk_t ** mpp, int err)
{
	if (err)
		return lmi_error_ind(lmi, mpp, err);
	if ((*mpp = lmi_reuseb(*mpp, LMI_ENABLE_CON_SIZE))) {
		lmi_enable_con_t *p = (lmi_enable_con_t *) (*mpp)->b_rptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = lmi->state;
		qreply(lmi->wq, *mpp);
	}
	return (0);
}

static inline int lmi_enable_ack(lmi_t * lmi, mblk_t ** mpp, int prim, int err)
{
	int ret;
	if (err)
		return lmi_error_ack(lmi, mpp, prim, err);
	if (!(ret = lmi_ok_ack(lmi, mpp, prim, 0)))
		ret = lmi_enable_con(lmi, mpp, 0);
	return (ret);
}

static inline int lmi_disable_con(lmi_t * lmi, mblk_t ** mpp, int err)
{
	if (err)
		return lmi_error_ind(lmi, mpp, err);
	if ((*mpp = lmi_reuseb(*mpp, LMI_DISABLE_CON_SIZE))) {
		lmi_disable_con_t *p = (lmi_disable_con_t *) (*mpp)->b_rptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = lmi->state;
		qreply(lmi->wq, *mpp);
	}
	return (0);
}

static inline int lmi_disable_ack(lmi_t * lmi, mblk_t ** mpp, int prim, int err)
{
	int ret;
	if (err)
		return lmi_error_ack(lmi, mpp, prim, err);
	if (!(ret = lmi_ok_ack(lmi, mpp, prim, 0)))
		ret = lmi_disable_con(lmi, mpp, 0);
	return (ret);
}

/* Downstream Primitives */

int lmi_info(lmi_t * lmi, void **ppap, int *lenp)
{
	int err;
	lmi_driver_t *drv = lmi->driver;
	if (drv && (err = drv->ops.lmi.info(lmi->device, ppap, lenp)))
		return (err);
	return (0);
}

int lmi_attach(lmi_t * lmi, void *ppa, int len)
{
	int err;
	lmi_driver_t *drv = lmi->driver;
	if (lmi->state != LMI_UNATTACHED)
		return (EINVAL | LMI_OUTSTATE);
	if (drv && (err = drv->ops.lmi.attach(lmi->device, ppa, len)))
		return (err);
	lmi->state = LMI_DISABLED;
	return (0);
}

int lmi_detach(lmi_t * lmi)
{
	int err;
	lmi_driver_t *drv = lmi->driver;
	if (lmi->state != LMI_DISABLED)
		return (EINVAL | LMI_OUTSTATE);
	if (drv && (err = drv->ops.lmi.detach(lmi->device)))
		return (err);
	lmi->state = LMI_UNATTACHED;
	return (0);
}

int lmi_enable(lmi_t * lmi)
{
	int err;
	lmi_driver_t *drv = lmi->driver;
	if (lmi->state != LMI_DISABLED)
		return (EINVAL | LMI_OUTSTATE);
	if (drv && (err = drv->ops.lmi.enable(lmi->device)))
		return (err);
	lmi->state = LMI_ENABLED;
	return (0);
}

int lmi_disable(lmi_t * lmi)
{
	int err;
	lmi_driver_t *drv = lmi->driver;
	if (lmi->state != LMI_ENABLED)
		return (EINVAL | LMI_OUTSTATE);
	if (drv && (err = drv->ops.lmi.disable(lmi->device)))
		return (err);
	lmi->state = LMI_DISABLED;
	return (0);
}

static int lmi_info_req(lmi_t * lmi, mblk_t * mp)
{
	static void *ppa = NULL;
	static int len = 0;
	int err = lmi_info(lmi, &ppa, &len);
	return lmi_info_ack(lmi, &mp, *((int *) mp->b_rptr), ppa, len, err);
}

static int lmi_attach_req(lmi_t * lmi, mblk_t * mp)
{
	union LMI_primitives *p = (union LMI_primitives *) mp->b_rptr;
	return lmi_ok_ack(lmi, &mp, *((int *) mp->b_rptr),
			  lmi_attach(lmi, p->attach_req.lmi_ppa,
				     mp->b_wptr - mp->b_rptr - LMI_ATTACH_REQ_SIZE));
}

static int lmi_detach_req(lmi_t * lmi, mblk_t * mp)
{
	return lmi_ok_ack(lmi, &mp, *((int *) mp->b_rptr), lmi_detach(lmi));
}

static int lmi_enable_req(lmi_t * lmi, mblk_t * mp)
{
	return lmi_enable_ack(lmi, &mp, *((int *) mp->b_rptr), lmi_enable(lmi));
}

static int lmi_disable_req(lmi_t * lmi, mblk_t * mp)
{
	return lmi_disable_ack(lmi, &mp, *((int *) mp->b_rptr), lmi_disable(lmi));
}

int (*lmi_lmi_ops[5]) (lmi_t *, mblk_t *) = {
	lmi_info_req,		/* LMI_INFO_REQ */
	    lmi_attach_req,	/* LMI_ATTACH_REQ */
	    lmi_detach_req,	/* LMI_DETACH_REQ */
	    lmi_enable_req,	/* LMI_ENABLE_REQ */
	    lmi_disable_req	/* LMI_DISABLE_REQ */
};

/* 
 *  =======================================================================
 *
 *  DRIVER
 *
 *  =======================================================================
 */

lmi_t *lmi_drv_attach(dev_t dev, lmi_driver_t * list, size_t size)
{
	lmi_driver_t *drv;
	lmi_ops_t *ops;
	lmi_t **lmip, *lmi = NULL;
	int cminor = getminor(dev);
	int cmajor = getmajor(dev);

	for (drv = list; drv; drv = drv->next)
		if (cmajor == drv->cmajor)
			break;
	if (!drv)
		return NULL;

	ops = &drv->ops;

	if (cminor >= drv->nminor)
		return NULL;

	for (lmip = &drv->list; *lmip; lmip = &(*lmip)->next) {
		if (cminor < getminor((*lmip)->devnum)) {
			lmi = NULL;
			break;
		}
		if (cminor == getminor((*lmip)->devnum)) {
			lmi = *lmip;
			break;
		}
	}

	if (!lmi) {
		if (!(lmi = kmalloc(size, GFP_KERNEL)))
			return NULL;
		bzero(drv, size);
		if (!(lmi->device = ops->dev.attach(dev))) {
			kfree(lmi);
			return NULL;
		}
		lmi->next = *lmip;
		*lmip = lmi;
		lmi->devnum = dev;
		lmi->driver = drv;
	}

	lmi->state = LMI_UNATTACHED;

	return lmi;
}

int lmi_drv_close(lmi_t * lmi, lmi_ulong * tids, int ntids, lmi_ulong * bids, int nbids)
{
	int i;
	lmi_t **lmip;
	lmi_driver_t *drv = lmi->driver;
	lmi_ops_t *ops = &drv->ops;

	for (lmip = &drv->list; *lmip; lmip = &(*lmip)->next)
		if (*lmip == lmi)
			break;
	if (!*lmip)
		return ENXIO;

	if (lmi->state == LMI_ENABLED) {
		ops->lmi.disable(lmi);
		lmi->state = LMI_DISABLED;
	}
	if (lmi->state == LMI_DISABLED) {
		ops->lmi.detach(lmi);
		lmi->state = LMI_UNATTACHED;
	}

	for (i = 0; i < ntids; i++)
		if (tids[i]) {
			untimeout(tids[i]);
			tids[i] = 0;
		}
	for (i = 0; i < nbids; i++)
		if (bids[i]) {
			untimeout(bids[i]);
			bids[i] = 0;
		}

	ops->dev.close(lmi->device);

	*lmip = lmi->next;
	kfree(lmi);

	return (0);
}

int lmi_drv_open(lmi_t * lmi)
{
	int err;
	if (!lmi || !lmi->driver)
		return ENXIO;
	if ((err = lmi->driver->ops.dev.open(lmi->device))) {
		lmi->state = LMI_UNUSABLE;
		lmi_drv_close(lmi, NULL, 0, NULL, 0);
	}
	return (err);
}

/* 
 *  =======================================================================
 *
 *  MUX OPEN and CLOSE
 *
 *  =======================================================================
 */

int
lmi_mux_open(queue_t * q, dev_t * devp, int flag, int sflag, cred_t * crp, lmi_driver_t * list,
	     size_t size)
{
	lmi_t *lmi = NULL;

	if (q->q_ptr != NULL)
		return (0);	/* do this in caller to avoid false success */

	if (size < sizeof(lmi_t))
		return EFAULT;

	if (sflag == DRVOPEN || WR(q)->q_next == NULL) {
		int err;
		lmi_t **lmip, *dev = NULL;
		lmi_driver_t *drv;
		int cminor = getminor(*devp);
		int cmajor = getmajor(*devp);

		for (drv = list; drv; drv = drv->next)
			if (cmajor == drv->cmajor)
				break;

		if (drv) {
			ptrace(("Driver open for cmajor %d, cminor %d, size %d\n", cmajor, cminor,
				size));

			if (sflag == CLONEOPEN || cminor == 0) {
				for (cminor = 1, lmip = &drv->list;
				     cminor <= drv->nminor && *lmip; lmip = &(*lmip)->next) {
					if (cminor < getminor((*lmip)->devnum)) {
						ptrace(("Calling device attach\n"));
						while (!
						       (dev =
							drv->ops.dev.
							attach(makedevice(cmajor, cminor))))
							if (++cminor < getminor((*lmip)->devnum))
								break;
						if (dev)
							break;
					}
					if (cminor == getminor((*lmip)->devnum))
						cminor++;
				}
				if (!*lmip && cminor <= drv->nminor) {
					ptrace(("Calling device attach\n"));
					dev = drv->ops.dev.attach(makedevice(cmajor, cminor));
				}
				if (!dev || cminor > drv->nminor)
					return ENXIO;
				*devp = makedevice(getmajor(*devp), cminor);
			} else {
				if (cminor > drv->nminor)
					return ENXIO;
				if (!(dev = drv->ops.dev.attach(*devp)))
					return ENXIO;
				for (lmip = &drv->list; *lmip; lmip = &(*lmip)->next) {
					if (cminor < getminor((*lmip)->devnum))
						break;
					if (cminor == getminor((*lmip)->devnum))
						return EBUSY;
				}
			}

			if (!(lmi = kmalloc(size, GFP_KERNEL)))
				return ENOMEM;
			bzero(lmi, size);

			lmi->devnum = *devp;
			lmi->driver = drv;
			lmi->device = dev;
			ptrace(("Calling device open\n"));
			if ((err = drv->ops.dev.open(dev))) {
				kfree(lmi);
				return (err);
			}
			dev->module = lmi;
			dev->ucalls = drv->ucalls;
			lmi->next = *lmip;
			*lmip = lmi;
		} else {
			ptrace(("Multiplexor open for size %d\n", size));

			/* FIXME: check permissions if minor number = 0 */
			if (!(lmi = kmalloc(size, GFP_KERNEL)))
				return ENOMEM;
			bzero(lmi, size);
		}
	} else
		return EIO;	/* these muxes can't be pushed */

	lmi->rq = RD(q);
	lmi->wq = WR(q);
	lmi->state = LMI_UNATTACHED;
	lmi_init_lock(lmi);
	lmi->user = NULL;

	q->q_ptr = WR(q)->q_ptr = lmi;

	return (0);
}

int
lmi_mux_close(queue_t * q, int flag, cred_t * crp, lmi_ulong * tids, int ntids, lmi_ulong * bids,
	      int nbids)
{
	int i;
	lmi_t **lmip = NULL, *lmi = (lmi_t *) q->q_ptr;
	lmi_driver_t *drv = lmi->driver;

	if (drv) {
		for (lmip = &drv->list; *lmip; lmip = &(*lmip)->next)
			if (*lmip == lmi)
				break;
		if (!*lmip)
			return EFAULT;
	}

	lmi_disable(lmi);

	for (i = 0; i < ntids; i++)
		if (tids[i]) {
			untimeout(tids[i]);
			tids[i] = 0;
		}
	for (i = 0; i < nbids; i++)
		if (bids[i]) {
			untimeout(bids[i]);
			bids[i] = 0;
		}

	lmi_detach(lmi);

	lmi->rq->q_ptr = lmi->wq->q_ptr = NULL;

	if (drv) {
		drv->ops.dev.close(lmi->device);
		*lmip = lmi->next;
	}

	kfree(lmi);
	return (0);
}

/* 
 *  =======================================================================
 *
 *  OPEN and CLOSE
 *
 *  =======================================================================
 */

int lmi_open(queue_t * q, dev_t * devp, int flag, int sflag, cred_t * crp, lmi_driver_t * list,
	     size_t size)
{
	lmi_t *lmi = NULL;

	if (q->q_ptr != NULL)
		return (0);	/* do this in caller to avoid false success */

	if (size < sizeof(lmi_t))
		return EFAULT;

	if (sflag == DRVOPEN || WR(q)->q_next == NULL) {
		int err;
		lmi_t **lmip, *dev = NULL;
		lmi_driver_t *drv;
		int cminor = getminor(*devp);
		int cmajor = getmajor(*devp);

		ptrace(("Driver open for cmajor %d, cminor %d, size %d\n", cmajor, cminor, size));

		for (drv = list; drv; drv = drv->next)
			if (cmajor == drv->cmajor)
				break;
		if (!drv)
			return ENXIO;

		if (sflag == CLONEOPEN || cminor == 0) {
			for (cminor = 1, lmip = &drv->list;
			     cminor <= drv->nminor && *lmip; lmip = &(*lmip)->next) {
				if (cminor < getminor((*lmip)->devnum)) {
					ptrace(("Calling device attach\n"));
					while (!
					       (dev =
						drv->ops.dev.attach(makedevice(cmajor, cminor))))
						if (++cminor < getminor((*lmip)->devnum))
							break;
					if (dev)
						break;
				}
				if (cminor == getminor((*lmip)->devnum))
					cminor++;
			}
			if (!*lmip && cminor <= drv->nminor) {
				ptrace(("Calling device attach\n"));
				dev = drv->ops.dev.attach(makedevice(cmajor, cminor));
			}
			if (!dev || cminor > drv->nminor)
				return ENXIO;
			*devp = makedevice(getmajor(*devp), cminor);
		} else {
			if (cminor > drv->nminor)
				return ENXIO;
			if (!(dev = drv->ops.dev.attach(*devp)))
				return ENXIO;
			for (lmip = &drv->list; *lmip; lmip = &(*lmip)->next) {
				if (cminor < getminor((*lmip)->devnum))
					break;
				if (cminor == getminor((*lmip)->devnum))
					return EBUSY;
			}
		}
		if (!(lmi = kmalloc(size, GFP_KERNEL)))
			return ENOMEM;
		bzero(lmi, size);

		lmi->devnum = *devp;
		lmi->driver = drv;
		lmi->device = dev;
		ptrace(("Calling device open\n"));
		if ((err = drv->ops.dev.open(dev))) {
			kfree(lmi);
			return (err);
		}
		dev->module = lmi;
		dev->ucalls = drv->ucalls;
		lmi->next = *lmip;
		*lmip = lmi;
	} else if (sflag == MODOPEN || WR(q)->q_next != NULL) {

		ptrace(("Module open for size %d\n", size));

		if (!(lmi = kmalloc(size, GFP_KERNEL)))
			return ENOMEM;
		bzero(lmi, size);
	} else
		return EIO;

	lmi->rq = RD(q);
	lmi->wq = WR(q);
	lmi->state = LMI_UNATTACHED;

	q->q_ptr = WR(q)->q_ptr = lmi;

	return (0);
}

int lmi_close(queue_t * q, int flag, cred_t * crp, lmi_ulong * tids, int ntids, lmi_ulong * bids,
	      int nbids)
{
	int i;
	lmi_t **lmip = NULL, *lmi = (lmi_t *) q->q_ptr;
	lmi_driver_t *drv = lmi->driver;

	if (drv) {
		for (lmip = &drv->list; *lmip; lmip = &(*lmip)->next)
			if (*lmip == lmi)
				break;
		if (!*lmip)
			return EFAULT;
	}

	lmi_disable(lmi);

	for (i = 0; i < ntids; i++)
		if (tids[i]) {
			untimeout(tids[i]);
			tids[i] = 0;
		}
	for (i = 0; i < nbids; i++)
		if (bids[i]) {
			untimeout(bids[i]);
			bids[i] = 0;
		}

	lmi_detach(lmi);

	lmi->rq->q_ptr = lmi->wq->q_ptr = NULL;

	if (drv) {
		drv->ops.dev.close(lmi->device);
		*lmip = lmi->next;
	}

	kfree(lmi);
	return (0);
}

/* 
 *  =======================================================================
 *
 *  DRIVER Registration
 *
 *  =======================================================================
 */

int lmi_register_driver(list, cmajor, strtab, nminor, name, ops, dcalls, ucalls)
	lmi_driver_t **list;
	major_t cmajor;
	struct streamtab *strtab;
	int nminor;
	char *name;
	lmi_ops_t *ops;
	void *dcalls;
	void *ucalls;
{
	int ret;
	lmi_driver_t *drv;

	if (nminor < 1 || !list || !strtab || !dcalls || !ucalls || !ops ||
	    !ops->dev.attach || !ops->dev.open || !ops->dev.close ||
	    !ops->lmi.info || !ops->lmi.attach || !ops->lmi.detach ||
	    !ops->lmi.enable || !ops->lmi.disable || !ops->lmi.ioctl) {
		__ptrace(("required pointer is NULL!\n"));
		return -EINVAL;
	}

	if (!(drv = kmalloc(sizeof(*drv), GFP_KERNEL))) {
		__ptrace(("couldn't allocate memory!\n"));
		return -ENOMEM;
	}
	bzero(drv, sizeof(*drv));

	drv->next = *list;	/* should spin lock this global */
	drv->cmajor = cmajor;
	drv->nminor = nminor;
	drv->info = strtab;
	drv->ops = *ops;
	drv->ucalls = ucalls;
	drv->dcalls = dcalls;

	if ((ret = lis_register_strdev(cmajor, strtab, nminor, name)) >= 0) {
		if (ret && !cmajor)
			drv->cmajor = cmajor = ret;
		*list = drv;
	} else {
		kfree(drv);
		__ptrace(("couldn't register driver with LiS!\n"));
	}
	return (ret);
}

int lmi_unregister_driver(lmi_driver_t ** list, major_t cmajor)
{
	int ret;
	lmi_driver_t **drvp, *drv;

	for (drvp = list; *drvp; drvp = &(*drvp)->next)
		if (cmajor == (*drvp)->cmajor)
			break;
	if (!(drv = *drvp)) {
		__ptrace(("couldn't find driver!\n"));
		return -ENXIO;
	}
	if (drv->list) {
		__ptrace(("drivers still running!\n"));
		return -EBUSY;
	}
	if ((ret = lis_unregister_strdev(cmajor))) {
		__ptrace(("couldn't unregister driver from LiS!\n"));
		return (ret);
	}

	*drvp = drv->next;
	kfree(drv);
	return (0);
}
