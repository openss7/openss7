/*****************************************************************************

 @(#) $RCSfile: tali.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:23:30 $

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

 Last Modified $Date: 2004/01/17 08:23:30 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tali.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:23:30 $"

static char const ident[] =
    "$RCSfile: tali.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:23:30 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include "../debug.h"
#include "../bufq.h"

#include "tali.h"
#include "tali_data.h"

/*
 *  TALI MULTIPLEXING DRIVER
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver to TALI.  When TALI is opened by its control
 *  stream (typically a tali configuration daemon), it provides a control
 *  channel for configuration and routing.  When TALI is opened by one of its
 *  user streams, it provides either a normal OpenSS7 MTP, ISUP, or SCCP stream.
 */

#define TALI_DESCRIP	"TALI STREAMS MULTIPLEXING DRIVER."
#define TALI_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define TALI_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define TALI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TALI_LICENSE	"GPL"
#define TALI_BANNER	TALI_DESCRIP	"\n" \
			TALI_COPYRIGHT	"\n" \
			TALI_DEVICE	"\n" \
			TALI_CONTACT	"\n"

MODULE_AUTHOR(TALI_CONTACT);
MODULE_DESCRIPTION(TALI_DESCRIP);
MODULE_SUPPORTED_DEVICE(TALI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TALI_LICENSE);
#endif

#ifndef INT
#define INT void
#endif

/*
 *  =========================================================================
 *
 *  STREAM Definitions
 *
 *  =========================================================================
 */
static int tali_open(queue_t *, dev_t *, int, int, cred_t *);
static int tali_close(queue_t *, int, cred_t *);

static INT tali_rput(queue_t *, mblk_t *);
static INT tali_rsrv(queue_t *);

static INT tali_wput(queue_t *, mblk_t *);
static INT tali_wsrv(queue_t *);

static struct module_info tali_minfo = {
	0,				/* Module ID number */
	"tali",				/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	1,				/* Hi water mark */
	0				/* Lo water mark */
};

static struct qinit tali_rinit {
	tali_rput,			/* Read put (msg from below) */
	tali_rsrv,			/* Read queue service */
	tali_open,			/* Each open */
	tali_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&tali_minfo,			/* Information */
	NULL				/* Statistics */
};

static struct qinit tali_winit {
	tali_wput,			/* Write put (msg from above) */
	tali_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tali_minfo,			/* Information */
	NULL				/* Statistics */
};

static struct streamtab tali_info = {
	&tali_rinit,			/* Upper read queue */
	&tali_winit,			/* Upper write queue */
	&tali_rinit,			/* Lower read queue */
	&tali_winit			/* Lower write queue */
};

/*
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Buffer Allocation
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  BUFSRV calls service routine
 *  ------------------------------------
 */
static void tali_bufsrv(long data)
{
	tali_t *tali = (tali_t *) data;
	if (tali->bid) {
		tali->bid = 0;
		qenable(tali->rq);
		qenable(tali->wq);
	}
}

/*
 *  BUFCALL for enobufs
 *  ------------------------------------
 */
static int tali_bufcall(tali_t * tali, size_t size)
{
	if (!tali->bid)
		tali->bid = bufcall(size, BPRI_MED, *tali_bufsrv, (long) tali);
	return (-ENOBUFS);
}

/*
 *  REUSEB
 *  ------------------------------------
 *  Attempt to reuse a mblk before allocating one.
 */
extern int tali_reuseb(tali_t * tali, size_t size, int prior, mblk_t ** mp)
{
	int rtn;
	if (!(*mp) || (*mp)->b_datap->db_lim = (*mp)->b_datap->db_base < size) {
		rtn = 0;
		if (!((*mp) = allocb(size, prior)))
			return tali_bufcall(tali, size);
	} else {
		rtn = 1;
		if ((*mp)->b_cont) {
			freemsg((*mp)->b_cont);
			(*mp)->b_cont = NULL;
		}
		(*mp)->b_wptr = (*mp)->b_rptr = (*mp)->b_datap->db_base;
	}
	return (rtn);
}

/*
 *  =========================================================================
 *
 *  Cache initialization and termination.
 *
 *  =========================================================================
 */
extern kmem_cache_t *tali_pp_cachep = NULL;
extern kmem_cache_t *tali_xp_cachep = NULL;
extern kmem_cache_t *tali_gp_cachep = NULL;
extern kmem_cache_t *tali_as_cachep = NULL;
extern kmem_cache_t *tali_ap_cachep = NULL;
extern kmem_cache_t *tali_sp_cachep = NULL;
extern kmem_cache_t *tali_np_cachep = NULL;

static tali_pp_cache_allocated = 0;
static tali_xp_cache_allocated = 0;
static tali_gp_cache_allocated = 0;
static tali_as_cache_allocated = 0;
static tali_ap_cache_allocated = 0;
static tali_sp_cache_allocated = 0;
static tali_np_cache_allocated = 0;

static void tali_term_caches(void)
{
	if (tali_pp_cachep) {
		if (tali_pp_cache_allocated)
			kmem_cache_destroy(tali_pp_cachep);
		tali_pp_cachep = NULL;
	}
	if (tali_xp_cachep) {
		if (tali_xp_cache_allocated)
			kmem_cache_destroy(tali_xp_cachep);
		tali_xp_cachep = NULL;
	}
	if (tali_gp_cachep) {
		if (tali_gp_cache_allocated)
			kmem_cache_destroy(tali_gp_cachep);
		tali_gp_cachep = NULL;
	}
	if (tali_sp_cachep) {
		if (tali_sp_cache_allocated)
			kmem_cache_destroy(tali_sp_cachep);
		tali_sp_cachep = NULL;
	}
	if (tali_as_cachep) {
		if (tali_as_cache_allocated)
			kmem_cache_destroy(tali_as_cachep);
		tali_as_cachep = NULL;
	}
	if (tali_ap_cachep) {
		if (tali_ap_cache_allocated)
			kmem_cache_destroy(tali_ap_cachep);
		tali_ap_cachep = NULL;
	}
	if (tali_np_cachep) {
		if (tali_np_cache_allocated)
			kmem_cache_destroy(tali_np_cachep);
		tali_np_cachep = NULL;
	}
	return;
}

static int tali_init_caches(void)
{
	if (!(tali_pp_cachep))
		if (!(tali_pp_cachep = kmem_cache_create("tali_pp_cachep", sizeof(pp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_pp_cache_allocated = 1;
	if (!(tali_xp_cachep))
		if (!(tali_xp_cachep = kmem_cache_create("tali_xp_cachep", sizeof(xp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_xp_cache_allocated = 1;
	if (!(tali_gp_cachep))
		if (!(tali_gp_cachep = kmem_cache_create("tali_gp_cachep", sizeof(gp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_gp_cache_allocated = 1;
	if (!(tali_sp_cachep))
		if (!(tali_sp_cachep = kmem_cache_create("tali_sp_cachep", sizeof(sp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_sp_cache_allocated = 1;
	if (!(tali_as_cachep))
		if (!(tali_as_cachep = kmem_cache_create("tali_as_cachep", sizeof(as_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_as_cache_allocated = 1;
	if (!(tali_ap_cachep))
		if (!(tali_ap_cachep = kmem_cache_create("tali_ap_cachep", sizeof(ap_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_ap_cache_allocated = 1;
	if (!(tali_np_cachep))
		if (!(tali_np_cachep = kmem_cache_create("tali_np_cachep", sizeof(np_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_np_cache_allocated = 1;
	return (0);

      tali_init_caches_failed:
	tali_term_caches();
	return (ENOMEM);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
static int tali_m_flush(queue_t * q, mblk_t * mp, const uint8_t flag, const uint8_t oflag)
{
	if (mp->b_rptr[0] & flag) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return (1);
		}
		mp->b_rptr[0] &= ~flag;
	}
	if (mp->b_rptr[0] & oflag && !(mp->b_flag & MSGNOLOOP)) {
		queue_t *oq = OTHERQ(q);
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(oq, mp->b_rptr[1], FLUSHALL);
		else
			flushq(oq, FLUSHALL);
		mp->b_flag |= MSGNOLOOP;
		qreply(q, mp);
		return (1);
	}
	return (0);
}

extern int tali_w_flush(queue_t * q, mblk_t * mp)
{
	return tali_m_flush(q, mp, FLUSHW, FLUSHR);
}
extern int tali_r_flush(queue_t * q, mblk_t * mp)
{
	return tali_m_flush(q, mp, FLUSHR, FLUSHW);
}

/*
 *  =========================================================================
 *
 *  PUTQ and SRVQ
 *
 *  =========================================================================
 */
/*
 *  TALI PUTQ
 *  -----------------------------------
 */
static int tali_putq(queue_t * q, mblk_t * mp, int (*proc) (queue_t *, mblk_t *))
{
	int rtn;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		seldom();
		putq(q, mp);
		return (0);
	}
	switch ((rtn = (*proc) (q, mp))) {
	case 0:
		freemsg(mp);
	case 1:
		break;
	case 2:
		freeb(mp);
		break;
	case 3:
		if (!q->q_next) {
			qreply(q, mp);
			break;
		}
	case 4:
		if (q->q_next) {
			putnext(q, mp);
			break;
		}
		rtn = -EOPNOTSUPP;
	default:
		ptrace(("Error (dropping) %d\n", rtn));
		freemsg(mp);
		return (rtn);
	case 6:
		noenable(q);
		putq(q, mp);
		return (0);
	case 5:
		if (mp->b_datap->db_type >= QPTCL || canputnext(q)) {
			putnext(q, mp);
			break;
		}
	case -ENOBUFS:		/* caller must schedule bufcall */
	case -EBUSY:		/* caller must have failed canput */
	case -EAGAIN:		/* caller must re-enable queue */
	case -ENOMEM:		/* caller must re-enable queue */
		putq(q, mp);
		break;
	}
	return (0);
}

/*
 *  TALI SRVQ
 *  -----------------------------------
 */
static int tali_srvq(queue_t * q, int (*proc) (queue_t *, mblk_t *))
{
	int rtn;
	mblk_t *mp;
	ensure(q, return (-EFAULT));
	while ((mp = getq(q))) {
		switch ((rtn = (*proc) (q, mp))) {
		case 0:
			freemsg(mp);
		case 1:
			continue;
		case 2:
			freeb(mp);
			continue;
		case 3:
			if (!q->q_next) {
				qreply(q, mp);
				continue;
			}
		case 4:
			if (q->q_next) {
				putnext(q, mp);
				continue;
			}
		default:
			ptrace(("Error (dropping) %d\n", rtn));
			freemsg(mp);
			continue;
		case 6:
			noenable(q);
			putbq(q, mp);
			return (0);
		case 5:
			if (mp->b_datap->db_type >= QPTCL || canputnext(q)) {
				putnext(q, mp);
				break;
			}
		case -ENOBUFS:	/* caller must schedule bufcall */
		case -EBUSY:	/* caller must have failed canput */
		case -EAGAIN:	/* caller must re-enable queue */
		case -ENOMEM:	/* caller must re-enable queue */
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return (rtn);
			}
			if (mp->b_datap->db_type == M_PCPROTO) {
				mp->b_datap->db_type = M_PROTO;
				mp->b_band = 255;
				putq(q, mp);
				break;
			}
			ptrace(("Error (dropping) %d\n", rtn));
			freemsg(mp);
			continue;
		}
	}
	return (0);
}
static INT tali_rput(queue_t * q, mblk_t * mp)
{
	pp_t *pp = (pp_t *) q->q_ptr;
	ensure(pp, return ((INT) (-EFAULT)));
	ensure(pp->ops.r_prim, return ((INT) (-EFAULT)));
	return (INT) tali_putq(q, mp, pp->ops.r_prim);
}
static INT tali_rsrv(queue_t * q)
{
	pp_t *pp = (pp_t *) q->q_ptr;
	ensure(pp, return ((INT) (-EFAULT)));
	ensure(pp->ops.r_prim, return ((INT) (-EFAULT)));
	return (INT) tali_srvq(q, mp, pp->ops.r_prim);
}
static INT tali_wput(queue_t * q, mblk_t * mp)
{
	pp_t *pp = (pp_t *) q->q_ptr;
	ensure(pp, return ((INT) (-EFAULT)));
	ensure(pp->ops.w_prim, return ((INT) (-EFAULT)));
	return (INT) tali_putq(q, mp, pp->ops.w_prim);
}
static INT tali_wsrv(queue_t * q)
{
	pp_t *pp = (pp_t *) q->q_ptr;
	ensure(pp, return ((INT) (-EFAULT)));
	ensure(pp->ops.w_prim, return ((INT) (-EFAULT)));
	return (INT) tali_srvq(q, mp, pp->ops.w_prim);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
drv_t *tali_drivers[] = {
	&tali_lm_driver,
	&tali_sccp_driver,
	&tali_isup_driver,
	&tali_mtp_driver,
	NULL
};
static int tali_open(queue_t * q, dev_t * devp, int flag, int sflag, cred_t * crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	dp_t *dp;
	ll_t *lpp;
	drv_t **d;
	for (d = tali_drivers; *d d++)
		if (cmajor >= (*d)->cmajor && cmajor < (*d)->cmajor + (*d)->nmajor)
			break;
	if (!(*d))
		return (ENXIO);
	if (slfag == MODOPEN || WR(q)->q_next) {
		ptrace(("Can't open as module\n"));
		return (EIO);
	}
	if (cmajor == TALI_LM_CMAJOR && crp->cr_uid != 0) {
		ptrace(("Can't open LM without r00t permission\n"));
		return (EPERM);
	}
	if (q->q_ptr != NULL) {
		ptrace(("Device already open\n"));
		return (0);
	}
	if (cmajor == drv->cmajor && cminor == 0) {
		ptrace(("Clone minor opened\n"));
		sflag = CLONEOPEN;
	}
	if (sflag == CLONEOPEN) {
		ptrace(("Clone open in effect\n"));
		cminor = 1;
	}
	for (lpp = &tali_opens_list; *lpp; lpp = &(*lpp)->next) {
		ushort dmajor = getmajor((*lpp)->id.dev);
		if (cmajor < dmajor)
			break;
		if (cmajor == dmajor) {
			ushort dminor = getminor((*lpp)->id.dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor) {
				if (sflag == CLONEOPEN) {
					if (++cminor > TALI_NMINOR) {
						if (++cmajor >= (*d)->cmajor + (*d)->nmajor)
							break;
						cminor = 0;
					}
					continue;
				}
				ptrace(("Requested device in use\n"));
				return (EIO);
			}
		}
	}
	if (cmajor >= (*d)->cmajor + (*d)->nmajor) {
		ptrace(("No devices available\n"));
		return (ENXIO);
	}
	*devp = makedevice(cmajor, cminor);
	if (!(dp = kmem_cache_alloc(tali_pp_cachep, SLAB_ATOMIC))) {
		ptrace(("Cannot allocate cache entry for device\n"));
		return (ENOMEM);
	}
	bzero(dp, sizeof(*dp));
	if ((dp->l.next->prev = *lpp))
		dp->l.next->prev = &dp->l.next;
	dp->l.prev = lpp;
	*lpp = (ll_t *) dp;
	dp->id..dev = *devp;
	dp->rq = RD(q);
	dp->wq = WR(q);
	dp->rq->q_ptr = dp;
	dp->wq->q_ptr = dp;
	dp->drv = (*d);
	dp->ops = (*d)->u_ops;
	return (0);
}
static int tali_close(queue_t * q, int flag, cred_t * crp)
{
	dp_t *dp = (dp_t *) q->q_ptr;
	if ((*(dp->l.prev) = dp->l.next))
		dp->l.next->prev = dp->l.prev;
	dp->l.prev = NULL;
	dp->l.next = NULL;
	if (dp->bid)
		unbufcall(dp->bid);
	dp->rq->q_ptr = NULL;
	dp->wq->q_ptr = NULL;
	if (dp->u.as)
		tali_free_as(dp->u.as);
	if (dp->lk.links) {
		ptrace(("Non-collapsed multiplexor on close!\n"));
		tali_deref_links(dp->lk.links);
	}
	if (dp->l.use_count) {
		ptrace(("Arrgh! Deleting DP with references!\n"));
		dp->l.use_count = 0;
	}
	kmem_cache_free(tali_pp_cachep, dp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
static int tali_initialized = 0;
int tali_init(void)
{
	int rtn;
	int err = 0;
	if (!tali_initialized) {
		drv_t **d;
		for (d = tali_drivers; *d; d++) {
			int cmajor, nminor;
			for (nminor = (*d)->nminor,
			     cmajor = (*d)->cmajor; cmajor < (*d)->cmajor + (*d)->nmajor;
			     cmajor++) {
				if ((rtn =
				     lis_register_strdev(cmajor, tali_info, nminor,
							 (*d)->name)) < 0) {
					cmn_err(CE_WARN,
						"tali: couldn't register %s driver cmajor=%d\n",
						(*d)->name, cmajor);
					err = -rtn;
				}
			}
		}
		if (!err)
			tali_initialized = 1;
	}
	return (err);
}
void tali_terminate(void)
{
	if (tali_initialized) {
		drv_t **d;
		for (d = tali_drivers; *d; d++) {
			int cmajor;
			for (cmajor = (*d)->cmajor; cmajor < (*d)->cmajor + (*d)->nmajor; cmajor++) {
				if (lis_unregister_strdev(cmajor)) {
					cmn_err(CE_WARN,
						"tali: couldn't unregister %s driver cmajor=%d\n",
						(*d)->name, cmajor);
				}
			}
		}
		tali_initialized = 0;
	}
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int init_module(void)
{
	cmn_err(CE_NOTE, TALI_BANNER);
	return tali_init();
}
void cleanup_module(void)
{
	return tali_terimate();
}
