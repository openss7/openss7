/*****************************************************************************

 @(#) $RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2004/01/17 08:22:13 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $"

static char const ident[] = "$RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $";

/*
 *  A Signalling Data Link Multiplexor for the OpenSS7 SS7 Stack.
 *
 *  This mux links SDL streams on the bottom of the multiplexor and presents
 *  SDL streams on the top of the multiplexor.  This permits independence
 *  between the streams on the top side of the multiplexor and streams on the
 *  bottom side of the multiplexor.  A binding operation is performed to bind
 *  an upper stream to a lower stream.
 */

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#include "debug.h"
#include "bufq.h"
#include "priv.h"
#include "lock.h"
#include "queue.h"
#include "allocb.h"
#include "timer.h"

#define DL_DESCRIP	"SS7/SDL: (Signalling Data Link) MULTIPLEXING STREAMS DRIVER."
#define DL_REVISION	"OpenSS7 $RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $"
#define DL_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corp.  All Rights Reserved."
#define DL_DEVICE	"Supportes OpenSS7 SDL Drivers."
#define DL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_LICENSE	"GPL"
#define DL_BANNER	DL_DESCRIP	"\n" \
			DL_REVISION	"\n" \
			DL_COPYRIGHT	"\n" \
			DL_DEVICE	"\n" \
			DL_CONTACT	"\n"

MODULE_AUTHOR(DL_CONTACT);
MODULE_DESCRIPTION(DL_DESCRIP);
MODULE_SUPPORTED_DEVICE(DL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_LICENSE);
#endif

#define DL_NMAJOR (SDLM_CMAJORS - 1)
#define LM_NMAJOR 1

#define SDLM_CMINORS 256

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

STATIC struct module_info dl_minfo = {
	SDLM_DRV_ID,			/* Module ID number */
	SDLM_DRV_NAME,			/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	1 << 15,			/* Hi water mark */
	1 << 10				/* Lo water mark */
};

STATIC int dl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int dl_close(queue_t *, int, cred_t *);

STATIC int dl_m_rput(queue_t *, mblk_t *);
STATIC int dl_m_rsrv(queue_t *);

STATIC struct qinit dl_m_rinit = {
	dl_m_rput,			/* Read put (msg from below) */
	dl_m_rsrv,			/* Read queue service */
	dl_open,			/* Each open */
	dl_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&dl_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int dl_m_wput(queue_t *, mblk_t *);
STATIC int dl_m_wsrv(queue_t *);

STATIC struct qinit dl_m_winit = {
	dl_m_wput,			/* Write put (msg from above) */
	dl_m_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&dl_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC struct streamtab dl_m_info = {
	&dl_m_rinit,			/* Upper read queue */
	&dl_m_winit,			/* Upper write queue */
	NULL,				/* Lower read queue */
	NULL				/* Lower write queue */
};

STATIC int dl_u_rput(queue_t *, mblk_t *);

STATIC struct qinit dl_u_rinit = {
	dl_u_rput,			/* Read put (msg from below) */
	NULL,				/* Read queue service */
	dl_open,			/* Each open */
	dl_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&dl_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int dl_u_wput(queue_t *, mblk_t *);

STATIC struct qinit dl_u_winit = {
	dl_u_wput,			/* Write put (msg from above) */
	NULL,				/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&dl_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int dl_l_rput(queue_t *, mblk_t *);

STATIC struct qinit dl_l_rinit = {
	dl_l_rput,			/* Read put (msg from below) */
	NULL,				/* Read queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&dl_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int dl_l_wput(queue_t *, mblk_t *);

STATIC struct qinit dl_l_winit = {
	dl_l_wput,			/* Write put (msg from above) */
	NULL,				/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&dl_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC struct streamtab dl_u_info = {
	&dl_u_rinit,			/* Upper read queue */
	&dl_u_winit,			/* Upper write queue */
	&dl_l_rinit,			/* Lower read queue */
	&dl_l_winit			/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structure and Caches
 *
 *  =========================================================================
 */
typedef struct dl {
	struct dl *next;
	struct dl **prev;
	union {
		dev_t devid;
		int muxid;
	} u;
	queue_t *rq;
	queue_t *wq;
	ulong ppa;
} dl_t;

#define DL_PRIV(__q) ((dl_t *)((__q)->q_ptr))

dl_t *dl_opens = NULL;
dl_t *dl_links = NULL;
dl_t *dl_ctrls = NULL;

/*
 *  -------------------------------------------------------------------------
 *
 *  Caches
 *
 *  -------------------------------------------------------------------------
 */

STATIC kmem_cache_t *dl_cachep = NULL;
STATIC int dl_cachep_allocated = 0;

STATIC void
dl_term_caches(void)
{
	if (dl_cachep) {
		if (dl_cachep_allocated)
			kmem_cache_destroy(dl_cachep);
		dl_cachep = NULL;
	}
	return;
}

STATIC int
dl_init_caches(void)
{
	if (!dl_cachep) {
		if (!(dl_cachep =
		      kmem_cache_create("dl_cachep", sizeof(dl_t), 0, SLAB_HWCACHE_ALIGN, NULL,
					NULL))) {
			dl_term_caches();
			return (ENOMEM);
		}
		dl_cachep_allocated = 1;
	}
	return (0);
}

STATIC dl_t *
dl_alloc_priv(queue_t *q)
{
	dl_t *dl;
	if ((dl = kmem_cache_alloc(dl_cachep, SLAB_ATOMIC))) {
		MOD_INC_USE_COUNT;
		bzero(dl, sizeof(*dl));
		dl->rq = RD(q);
		dl->wq = WR(q);
		DL_PRIV(dl->rq) = DL_PRIV(dl->wq) = dl;
	}
	return (dl);
}

STATIC void
dl_free_priv(queue_t *q)
{
	dl_t *dl = DL_PRIV(q);
	DL_PRIV(dl->rq) = DL_PRIV(dl->wq) = NULL;
	dl->rq = NULL;
	dl->wq = NULL;
	kmem_cache_free(dl_cachep, dl);
	MOD_DEC_USE_COUNT;
}

/*
 *  =========================================================================
 *
 *  Primitives
 *
 *  =========================================================================
 */
STATIC INLINE int
lmi_ok_ack_reply(queue_t *q, mblk_t *mp, int prim, int state)
{
	lmi_ok_ack_t *p;
	mp->b_wptr = mp->b_rptr;
	p = ((typeof(p)) mp->b_wptr)++;
	p->lmi_primitive = LMI_OK_ACK;
	p->lmi_correct_primitive = prim;
	p->lmi_state = state;
	qreply(q, mp);
	return (0);
}
STATIC INLINE int
lmi_error_ack_reply(queue_t *q, mblk_t *mp, int prim, int state, int err)
{
	lmi_error_ack_t *p;
	/*
	   return a negative acknowledgement 
	 */
	mp->b_wptr = mp->b_rptr;
	p = ((typeof(p)) mp->b_wptr)++;
	p->lmi_primitive = LMI_ERROR_ACK;
	p->lmi_error_primitive = prim;
	p->lmi_state = state;
	p->lmi_errno = err > 0 ? 0 : -err;
	p->lmi_reason = err > 0 ? err : LMI_SYSERR;
	qreply(q, mp);
	return (err);
}
STATIC INLINE int
m_error_reply(queue_t *q, mblk_t *mp, int err)
{
	mp->b_wptr = mp->b_rptr;
	mp->b_datap->db_type = M_ERROR;
	*(mp->b_wptr)++ = err < 0 ? -err : err;
	*(mp->b_wptr)++ = err < 0 ? -err : err;
	qreply(q, mp);
	return (err);
}

/*
 *  LMI_ATTACH_REQ:
 *  -----------------------------------
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_attach_req_t *m = ((typeof(m)) mp->b_rptr);
	if (mlen < sizeof(*m) || mlen < 2 * sizeof(ulong))
		goto lprotoshort;
	if (q->q_next)
		goto loutstate;
	{
		dl_t *du = DL_PRIV(q), *dl;
		ulong ppa = *((ulong *) &m->lmi_ppa);
		for (dl = dl_links; dl && dl->ppa != ppa; dl = dl->next) ;
		if (!dl || dl->rq->q_next)
			goto lbadppa;
		/*
		   link queues together 
		 */
		du->wq->q_next = dl->wq;
		dl->rq->q_next = du->rq;
		return lmi_ok_ack_reply(q, mp, LMI_ATTACH_REQ, LMI_DISABLED);
	}
      lbadppa:
	err = LMI_BADPPA;
	ptrace(("ERROR: PPA in use\n"));
	goto error;
      loutstate:
	err = LMI_OUTSTATE;
	ptrace(("ERROR: would place i/f out of state\n"));
	goto error;
      lprotoshort:
	err = LMI_PROTOSHORT;
	ptrace(("ERROR: proto block to short\n"));
	goto error;
      error:
	return lmi_error_ack_reply(q, mp, LMI_ATTACH_REQ, LMI_UNATTACHED, err);
}

/*
 *  LMI_DETACH_REQ:
 *  -----------------------------------
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_detach_req_t *m = ((typeof(m)) mp->b_rptr);
	if (mlen < sizeof(*m))
		goto lprotoshort;
	if (!q->q_next)
		goto loutstate;
	{
		dl_t *du = DL_PRIV(q);
		dl_t *dl = DL_PRIV(q->q_next);
		/*
		   disconnect them 
		 */
		du->wq->q_next = NULL;
		dl->rq->q_next = NULL;
		{
			lmi_ok_ack_t *p;
			mp->b_wptr = mp->b_rptr;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_OK_ACK;
			p->lmi_correct_primitive = LMI_DETACH_REQ;
			p->lmi_state = LMI_UNATTACHED;
			qreply(q, mp);
			return (0);
		}
	}
      loutstate:
	err = LMI_OUTSTATE;
	ptrace(("ERROR: would place i/f out of state\n"));
	goto error;
      lprotoshort:
	err = LMI_PROTOSHORT;
	ptrace(("ERROR: proto block too short\n"));
	goto error;
      error:
	return lmi_error_ack_reply(q, mp, LMI_DETACH_REQ, LMI_UNATTACHED, err);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  CTRL Queues
 *  -----------------------------------
 */
STATIC int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_FLUSH:
	default:
		break;
	}
	return (QR_PASSALONG);
}
STATIC int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_FLUSH:
	case M_IOCTL:
	default:
		break;
	}
	return (QR_PASSALONG);
}
STATIC int
dl_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	if (mp->b_datap->db_type >= QPCTL && !q->q_count) {
		int rtn;
		switch ((rtn = (*proc) (q, mp))) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			break;
		case QR_TRIMMED:
			freeb(mp);
			break;
		case QR_LOOP:
			if (!q->q_next) {
				qreply(q, mp);
				break;
			}
		case QR_PASSALONG:
			if (q->q_next) {
				putnext(q, mp);
				break;
			}
			rtn = -EOPNOTSUPP;
		default:
			ptrace(("ERROR: (q dropping) %d\n", rtn));
			freemsg(mp);
			break;
		case QR_DISABLE:
			putq(q, mp);
			rtn = 0;
			break;
		case QR_PASSFLOW:
			if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
				putnext(q, mp);
				break;
			}
		case -ENOBUFS:
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
			putq(q, mp);
			break;
		}
		return (rtn);
	} else {
		putq(q, mp);
		return (0);
	}
}
STATIC int
dl_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	int rtn;
	mblk_t *mp;
	while ((mp = getq(q))) {
		switch ((rtn = (*proc) (q, mp))) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			continue;
		case QR_TRIMMED:
			freeb(mp);
			continue;
		case QR_LOOP:
			if (!q->q_next) {
				qreply(q, mp);
				continue;
			}
		case QR_PASSALONG:
			if (q->q_next) {
				putnext(q, mp);
				continue;
			}
			rtn = -EOPNOTSUPP;
		default:
			ptrace(("ERROR: (q dropping) %d\n", rtn));
			freemsg(mp);
			continue;
		case QR_DISABLE:
			ptrace(("ERROR: (q disabling) %d\n", rtn));
			noenable(q);
			putq(q, mp);
			rtn = 0;
			break;
		case QR_PASSFLOW:
			if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
				putnext(q, mp);
				continue;
			}
		case -ENOBUFS:
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
			ptrace(("ERROR: (q stalled) %d\n", rtn));
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
			ptrace(("ERROR: (q dropping) %d\n", rtn));
			freemsg(mp);
			continue;
		}
	}
	return (0);
}
STATIC int
dl_m_rput(queue_t *q, mblk_t *mp)
{
	return ((int) dl_putq(q, mp, &dl_r_prim));
}
STATIC int
dl_m_rsrv(queue_t *q)
{
	return ((int) dl_srvq(q, &dl_r_prim));
}
STATIC int
dl_m_wput(queue_t *q, mblk_t *mp)
{
	return ((int) dl_putq(q, mp, &dl_w_prim));
}
STATIC int
dl_m_wsrv(queue_t *q)
{
	return ((int) dl_srvq(q, &dl_w_prim));
}

/*
 *  UPPER MUX Queues
 *  -----------------------------------
 */
STATIC int
dl_u_rput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return ((int) (0));
}
STATIC int
dl_u_wput(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
	{
		long prim = *((long *) mp->b_rptr);
		switch (prim) {
		case LMI_ATTACH_REQ:
			return ((int) lmi_attach_req(q, mp));
		case LMI_DETACH_REQ:
			return ((int) lmi_detach_req(q, mp));
		case LMI_INFO_REQ:
		case LMI_ENABLE_REQ:
		case LMI_DISABLE_REQ:
		case LMI_OPTMGMT_REQ:
			if (q->q_next) {
				putnext(q, mp);
				return ((int) (0));
			}
			ptrace(("ERROR: would place i/f out of state\n"));
			return lmi_error_ack_reply(q, mp, prim, LMI_UNATTACHED, LMI_OUTSTATE);
		case SDL_BITS_FOR_TRANSMISSION_REQ:
		case SDL_CONNECT_REQ:
		case SDL_DISCONNECT_REQ:
			if (q->q_next) {
				putnext(q, mp);
				return ((int) (0));
			}
			return ((int) m_error_reply(q, mp, EPIPE));
		default:
			freemsg(mp);
			return ((int) m_error_reply(q, mp, EPROTO));
		}
	}
		break;
	case M_FLUSH:
		if (q->q_next)
			putnext(q, mp);
		else
			qreply(q, mp);
		break;
	default:
		if (q->q_next)
			putnext(q, mp);
		else {
			freemsg(mp);
			return ((int) (-EOPNOTSUPP));
		}
		break;
	}
	return ((int) (0));
}

/*
 *  LOWER MUX Queues
 *  -----------------------------------
 */
STATIC int
dl_l_rput(queue_t *q, mblk_t *mp)
{
	if (q->q_next) {
		putnext(q, mp);
		return ((int) (0));
	}
	if (mp->b_datap->db_type == M_FLUSH) {
		qreply(q, mp);
		return ((int) (0));
	}
	ptrace(("ERROR: receive message for detached stream\n"));
	freemsg(mp);
	return ((int) (-EOPNOTSUPP));
}
STATIC int
dl_l_wput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return ((int) (0));
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	int lmajor = 0;
	int lminor = 0;
	dl_t *dl = NULL, **dlp = &dl;
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("ERROR: Cannot open as module\n"));
		return (EIO);
	}
	if (SDLM_CMAJOR_1 <= cmajor && cmajor < SDLM_CMAJOR_1 + LM_NMAJOR && crp->cr_uid != 0) {
		ptrace(("ERROR: r00t permission needed for control stream\n"));
		return (EPERM);
	}
	if (DL_PRIV(q)) {
		ptrace(("INFO: Already open\n"));
		return (0);
	}
	if ((cmajor == SDLM_CMAJOR_0 || cmajor == SDLM_CMAJOR_1) && cminor == 0) {
		ptrace(("INFO: Clone open in effect\n"));
		sflag = CLONEOPEN;
		cminor = 1;
	}
	if (SDLM_CMAJOR_0 <= cmajor && cmajor < SDLM_CMAJOR_0 + DL_NMAJOR) {
		dlp = &dl_opens;
		lminor = SDLM_CMINORS;
		lmajor = SDLM_CMAJOR_0 + DL_NMAJOR;
	}
	if (SDLM_CMAJOR_1 <= cmajor && cmajor < SDLM_CMAJOR_1 + LM_NMAJOR) {
		dlp = &dl_ctrls;
		lminor = SDLM_CMINORS;
		lmajor = SDLM_CMAJOR_1 + LM_NMAJOR;
	}
	for (dlp = &dl_opens; *dlp; dlp = &((*dlp)->next)) {
		ushort dmajor = getmajor((*dlp)->u.devid);
		if (cmajor < dmajor)
			break;
		if (cmajor == dmajor) {
			ushort dminor = getminor((*dlp)->u.devid);
			if (cminor < dminor)
				break;
			if (cminor == dminor) {
				if (sflag != CLONEOPEN) {
					ptrace(("ERROR: Requested device in use\n"));
					return (EIO);
				}
				if (++cminor > lminor) {
					if (++cmajor >= lmajor)
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (cmajor >= lmajor) {
		ptrace(("ERROR: No device available\n"));
		return (ENXIO);
	}
	if (!(dl = dl_alloc_priv(q))) {
		ptrace(("ERROR: Could not allocate private structure\n"));
		return (ENOMEM);
	}
	*devp = makedevice(cmajor, cminor);
	dl->u.devid = *devp;
	if ((dl->next = *dlp))
		dl->next->prev = &dl->next;
	dl->prev = dlp;
	*dlp = dl;
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_close(queue_t *q, int flag, cred_t *crp)
{
	dl_t *dl = DL_PRIV(q);
	if ((*(dl->prev) = dl->next))
		dl->next->prev = dl->prev;
	dl->prev = NULL;
	dl->next = NULL;
	dl_free_priv(q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
STATIC int dl_initialized = 0;
STATIC int
dl_init(void)
{
	if (!dl_initialized) {
		int i;
		int err;
		for (i = 0; i < DL_NMAJOR; i++) {
			if ((err =
			     lis_register_strdev(SDLM_CMAJOR_0 + i, &dl_u_info, SDLM_CMINORS,
						 dl_minfo.mi_idname)) < 0) {
				cmn_err(CE_WARN, "%s: couldn't register driver cmajor %d\n",
					SDLM_DRV_NAME, SDLM_CMAJOR_0);
			}
		}
		for (i = 0; i < LM_NMAJOR; i++) {
			if ((err =
			     lis_register_strdev(SDLM_CMAJOR_1 + i, &dl_m_info, SDLM_CMINORS,
						 dl_minfo.mi_idname)) < 0) {
				cmn_err(CE_WARN, "%s: couldn't register driver cmajor %d\n",
					SDLM_DRV_NAME, SDLM_CMAJOR_1);
			}
		}
		dl_init_caches();
		dl_initialized = 1;
	}
	return (0);
}
STATIC void
dl_terminate(void)
{
	if (dl_initialized) {
		int i, err;
		for (i = 0; i < DL_NMAJOR; i++) {
			if ((err = lis_unregister_strdev(SDLM_CMAJOR_0 + i)) < 0) {
				cmn_err(CE_WARN, "%s: couldn't unregister driver cmajor %d\n",
					SDLM_DRV_NAME, SDLM_CMAJOR_0);
			}
		}
		for (i = 0; i < LM_NMAJOR; i++) {
			if ((err = lis_unregister_strdev(SDLM_CMAJOR_1 + i)) < 0) {
				cmn_err(CE_WARN, "%s: couldn't unregister driver cmajor %d\n",
					SDLM_DRV_NAME, SDLM_CMAJOR_1);
			}
		}
		dl_term_caches();
		dl_initialized = 0;
	}
	return;
}

/*
 *  =======================================================================
 *
 *  Kernel Module Initialization
 *
 *  =======================================================================
 */

int
init_module(void)
{
	cmn_err(CE_NOTE, DL_BANNER);
	return dl_init();
}

void
cleanup_module(void)
{
	(void) ss7_oput;
	(void) ss7_iput;
	(void) ss7_unbufcall;
	return dl_terminate();
}
