/*****************************************************************************

 @(#) $RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/14 08:00:06 $

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

 Last Modified $Date: 2004/05/14 08:00:06 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/14 08:00:06 $"

static char const ident[] =
    "$RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/14 08:00:06 $";

#if defined(_LIS_SOURCE) && !defined(MODULE)
#   error ****
#   error ****  tirdwr can only compile as a module under LiS.
#   error ****  This is normally because LiS has been grossly misconfigured.
#   error ****  Report bugs to <bugs@openss7.org>.
#   error ****
#endif

#ifdef LINUX
#   include <linux/config.h>
#   include <linux/version.h>
#   ifdef MODVERSIONS
#	include <linux/modversions.h>
#   endif
#   include <linux/module.h>
#   include <linux/modversions.h>
#   ifndef __GENKSYMS__
#	ifdef LIS
#	    include <sys/LiS/modversions.h>
#	else
#	    include <sys/streams/modversions.h>
#	endif
#   endif
#endif

#include <sys/kmem.h>
#include <sys/cmn_err.h>

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

/*
   These are for TPI definitions 
 */
#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#ifdef _LFS_SOURCE
#include "sys/config.h"
#include "strdebug.h"
#endif

// #define TIRDWR_PEDANTIC
// #define TIRDWR_TPI_PEDANTIC

#define TIRDWR_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TIRDWR_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define TIRDWR_REVISION		"LfS $RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/14 08:00:06 $"
#define TIRDWR_DEVICE		"SVR 4.2 STREAMS Read Write Module for XTI/TLI Devices (TIRDWR)"
#define TIRDWR_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TIRDWR_LICENSE		"GPL"
#define TIRDWR_BANNER		TIRDWR_DESCRIP		"\n" \
				TIRDWR_COPYRIGHT	"\n" \
				TIRDWR_REVISION		"\n" \
				TIRDWR_DEVICE		"\n" \
				TIRDWR_CONTACT		"\n"
#define TIRDWR_SPLASH		TIRDWR_DEVICE		" - " \
				TIRDWR_REVSISION

MODULE_AUTHOR(TIRDWR_CONTACT);
MODULE_DESCRIPTION(TIRDWR_DESCRIP);
MODULE_SUPPORTED_DEVICE(TIRDWR_DEVICE);
MODULE_LICENSE(TIRDWR_LICENSE);

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#ifndef TIRDWR_MOD_NAME
#   ifdef CONFIG_STREAMS_TIRDWR_NAME
#	define TIRDWR_MOD_NAME CONFIG_STREAMS_TIRDWR_NAME
#   else
#	define TIRDWR_MOD_NAME "tirdwr"
#   endif
#endif

#ifndef TIRDWR_MOD_ID
#   ifdef CONFIG_STREAMS_TIRDWR_MODID
#	define TIRDWR_MOD_ID CONFIG_STREAMS_TIRDWR_MODID
#   else
#	define TIRDWR_MOD_ID 0
#   endif
#endif

static modID_t modid = TIRDWR_MOD_ID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for TIRDWR.");

static struct module_info tirdwr_minfo = {
	mi_idnum:TIRDWR_MOD_ID,		/* Module ID number */
	mi_idname:TIRDWR_MOD_NAME,	/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

static int tirdwr_open(queue_t *, dev_t *, int, int, cred_t *);
static int tirdwr_close(queue_t *, int, cred_t *);

static int tirdwr_rput(queue_t *q, mblk_t *mp);
static int tirdwr_wput(queue_t *q, mblk_t *mp);

static struct qinit tirdwr_rinit = {
	qi_putp:tirdwr_rput,		/* Read put (message from below) */
	qi_qopen:tirdwr_open,		/* Each open */
	qi_qclose:tirdwr_close,		/* Last close */
	qi_minfo:&tirdwr_minfo,		/* Information */
};

static struct qinit tirdwr_winit = {
	qi_putp:tirdwr_wput,		/* Write put (message from above) */
	qi_minfo:&tirdwr_minfo,		/* Information */
};

static struct streamtab tirdwr_info = {
	st_rdinit:&tirdwr_rinit,	/* Upper read queue */
	st_wrinit:&tirdwr_winit,	/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  Private Datastructure ctors and dtors
 *
 *  =========================================================================
 */
#ifdef LIS
/*
   LiS does not define these... 
 */
#if !defined(HAVE_BCID_T)
typedef int bcid_t;
#endif
#if !defined(HAVE_BUFCALL_ID_T)
typedef int bufcall_id_t;
#endif
#endif

typedef struct tirdwr {
	queue_t *rq;			/* module read queue */
	queue_t *wq;			/* module write queue */
	queue_t *hq;			/* stream head read queue */
	ulong state;			/* module state */
	ulong flags;			/* module flags */
	bcid_t rdzero_bcid;		/* bufcall id for sending zero length M_DATA */
	bcid_t hangup_bcid;		/* bufcall id for sending M_HANGUP */
	bcid_t eproto_bcid;		/* bufcall id for sending M_ERROR */
} tirdwr_t;

static kmem_cache_t *tirdwr_priv_cachep = NULL;

static int
tirdwr_init_caches(void)
{
	if (!tirdwr_priv_cachep &&
	    !(tirdwr_priv_cachep =
	      kmem_cache_create(TIRDWR_MOD_NAME, sizeof(tirdwr_t), 0, SLAB_HWCACHE_ALIGN,
				NULL, NULL))) {
		cmn_err(CE_WARN, "%s: %s: Cannot allocate tirdwr_priv_cachep",
			TIRDWR_MOD_NAME, __FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}

static void
tirdwr_term_caches(void)
{
	if (tirdwr_priv_cachep) {
		if (kmem_cache_destroy(tirdwr_priv_cachep))
			cmn_err(CE_WARN, "%s: %s: did not destroy tirdwr_priv_cachep",
				TIRDWR_MOD_NAME, __FUNCTION__);
	}
	return;
}

static tirdwr_t *
tirdwr_alloc_priv(queue_t *q)
{
	tirdwr_t *priv;
	if ((priv = kmem_cache_alloc(tirdwr_priv_cachep, SLAB_ATOMIC))) {
		priv->rq = q;
		priv->wq = WR(q);
		priv->hq = q->q_next;
		priv->state = TS_DATA_XFER;	/* assume we are in the correct initial state */
		priv->flags = 0;
		priv->rdzero_bcid = 0;
		priv->hangup_bcid = 0;
		priv->eproto_bcid = 0;
		/*
		   we are a module with no service routine so our hiwat, lowat shouldn't matter;
		   however, our minpsz, maxpsz do because we are the first queue under the stream
		   head.  We do not want to alter the characteristics of the transport packet sizes
		   so we copy them here. This will allow the stream head to exhibit the same
		   behaviour as before we were pushed. 
		 */
		priv->wq->q_minpsz = priv->wq->q_next->q_minpsz;
		priv->wq->q_maxpsz = priv->wq->q_next->q_maxpsz;
		q->q_ptr = WR(q)->q_ptr = priv;
	}
	return (priv);
}

static void
tirdwr_free_priv(queue_t *q)
{
	tirdwr_t *priv;
	if ((priv = (typeof(priv)) q->q_ptr)) {
		if (priv->rdzero_bcid)
			unbufcall(xchg(&priv->rdzero_bcid, 0));
		if (priv->hangup_bcid)
			unbufcall(xchg(&priv->hangup_bcid, 0));
		if (priv->eproto_bcid)
			unbufcall(xchg(&priv->eproto_bcid, 0));
		q->q_ptr = WR(q)->q_ptr = NULL;
		priv->rq = NULL;
		priv->wq = NULL;
		priv->hq = NULL;
		priv->state = TS_IDLE;
		priv->flags = 0;
		kmem_cache_free(tirdwr_priv_cachep, priv);
	}
	return;
}

/*
 *  =========================================================================
 *
 *  Messages and Buffer Calls
 *
 *  =========================================================================
 */
static void tirdwr_rdzero_bc(long arg);
static void tirdwr_hangup_bc(long arg);
static void tirdwr_eproto_bc(long arg);

#define TIRDWR_HANGUP	01
#define TIRDWR_EPROTO	02

static void
tirdwr_rdzero(tirdwr_t * priv, mblk_t *mp, mblk_t *bp)
{
	if (bp)
		freemsg(bp);
	if (mp)
		freemsg(mp);
	if (!(priv->flags & (TIRDWR_HANGUP | TIRDWR_EPROTO))) {
		if (priv->rdzero_bcid != 0)
			unbufcall(xchg(&priv->rdzero_bcid, 0));
		if ((mp = allocb(0, BPRI_HI))) {
			mp->b_datap->db_type = M_DATA;
			putnext(priv->rq, mp);
			return;
		}
		if (!(priv->rdzero_bcid = bufcall(0, BPRI_HI, &tirdwr_rdzero_bc, (long) priv)))
			cmn_err(CE_WARN, "%s: could not allocate rdzero buffer call", TIRDWR_MOD_NAME);
	}
}
static void
tirdwr_hangup(tirdwr_t * priv, mblk_t *mp, mblk_t *bp)
{
	if (bp)
		freemsg(bp);
	if (mp)
		freemsg(mp);
	if (!(priv->flags & (TIRDWR_HANGUP | TIRDWR_EPROTO))) {
		if (priv->hangup_bcid != 0)
			unbufcall(xchg(&priv->hangup_bcid, 0));
		/*
		   LiS does not have a reliable putnextctl() 
		 */
		if ((mp = allocb(0, BPRI_HI))) {
			mp->b_datap->db_type = M_HANGUP;
			/*
			   LiS doesn't have a reliable putnext() either :/ 
			 */
			putnext(priv->rq, mp);
			priv->flags |= TIRDWR_HANGUP;
			return;
		}
		if (!(priv->hangup_bcid = bufcall(0, BPRI_HI, &tirdwr_hangup_bc, (long) priv)))
			cmn_err(CE_WARN, "%s: could not allocate hangup buffer call", TIRDWR_MOD_NAME);
	}
}
static void
tirdwr_eproto(tirdwr_t * priv, mblk_t *mp, mblk_t *bp)
{
	if (bp)
		freemsg(bp);
	if (mp)
		freemsg(mp);
	if (!(priv->flags & TIRDWR_EPROTO)) {
		if (priv->eproto_bcid != 0)
			unbufcall(xchg(&priv->eproto_bcid, 0));
		if ((mp = allocb(2, BPRI_HI))) {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = EPROTO;
			*(mp->b_wptr)++ = EPROTO;
			putnext(priv->rq, mp);
			priv->flags |= TIRDWR_EPROTO;
			return;
		}
		if (!(priv->eproto_bcid = bufcall(2, BPRI_HI, &tirdwr_eproto_bc, (long) priv)))
			cmn_err(CE_WARN, "%s: could not allocate eproto buffer call", TIRDWR_MOD_NAME);
	}
}

static void
tirdwr_rdzero_bc(long arg)
{
	tirdwr_t *priv = (typeof(priv)) arg;
	if (xchg(&priv->rdzero_bcid, 0) != 0)
		tirdwr_hangup((tirdwr_t *) arg, NULL, NULL);
}
static void
tirdwr_hangup_bc(long arg)
{
	tirdwr_t *priv = (typeof(priv)) arg;
	if (xchg(&priv->hangup_bcid, 0) != 0)
		tirdwr_hangup((tirdwr_t *) arg, NULL, NULL);
}
static void
tirdwr_eproto_bc(long arg)
{
	tirdwr_t *priv = (typeof(priv)) arg;
	if (xchg(&priv->eproto_bcid, 0) != 0)
		tirdwr_eproto((tirdwr_t *) arg, NULL, NULL);
}

/*
 *  =========================================================================
 *
 *  PUT routines
 *
 *  =========================================================================
 */

static inline void
tirdwr_restore_delim(t_uscalar_t flag, mblk_t *mp)
{
#if !defined TIRDWR_PEDANTIC
	/*
	   Although SVR 4 documentation says that we ignore message delimitors, we restore message
	   delimitors in case the RMSGN or RMSGD read options are set at the stream head. Otherwise
	   they are ignored anyway. 
	 */
	if (flag)
		mp->b_flag &= ~MSGDELIM;
	else
		mp->b_flag |= MSGDELIM;
#endif
	return;
}

static int
tirdwr_rput(queue_t *q, mblk_t *mp)
{
	tirdwr_t *priv = (typeof(priv)) q->q_ptr;
	mblk_t *bp = NULL;
#if defined _LIS_SOURCE
	if (q->q_next == NULL || OTHER(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with null q->q_next pointer.",
			TIRDWR_MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined _LIS_SOURCE */
	switch (mp->b_datap->db_type) {
	case M_DATA:
		/*
		   There is a problem here right off the bat.  Although a TPI compliant provider
		   must accept M_DATA written (instead of T_DATA_REQ), it should not generate
		   M_DATA on read.  These should only be T_DATA_IND.  tirdwr can be pushed over
		   other modules, however, and an underlying module (e.g. timod) might be stripping 
		   T_DATA_IND blocks, so we process them anyway. 
		 */
#	    if !defined TIRDWR_TPI_PEDANTIC
		if (mp->b_band == 0) {
			if (msgdsize(mp) > 0) {
				putnext(q, mp);
			} else {
				/*
				   Silently discard zero length normal data. 
				 */
				freemsg(mp);
			}
		} else
#	    endif		/* defined TIRDWR_TPI_PEDANTIC */
			tirdwr_eproto(priv, mp, bp);
		break;
	case M_PCPROTO:
#	    if defined TIRDWR_TPI_PEDANTIC
		/*
		   We could be forgiving and accept M_PCPROTO mesages that should actually be sent
		   as M_PROTO messages; however, a transport service provider that sends M_PCPROTO
		   for data or disconnect messages is in violation of the TPI specifications. 
		 */
		tirdwr_eproto(priv, mp, bp);
		break;
#	    endif		/* defined TIRDWR_TPI_PEDANTIC */
	case M_PROTO:
	{
		union T_primitives *p = (typeof(p)) mp->b_rptr;
		if (mp->b_wptr < mp->b_rptr + sizeof(p->type)) {
			tirdwr_eproto(priv, mp, bp);
			break;
		}
		switch (p->type) {
		case T_DATA_IND:
			if ((bp = unlinkb(mp)) && bp->b_datap->db_type == M_DATA && bp->b_band == 0) {
				if (msgdsize(bp) > 0) {
					tirdwr_restore_delim(p->data_ind.MORE_flag, bp);
					freeb(mp);
					putnext(q, bp);
				} else {
					/*
					   Silently discard zero length normal data. 
					 */
					freeb(mp);
					freemsg(bp);
				}
			} else
				tirdwr_eproto(priv, mp, bp);
			break;
#	    if defined T_OPTDATA_IND
		case T_OPTDATA_IND:
			/*
			   T_OPTDATA is not always provided by the tihdr and is a more recent TPI
			   (Version 5 Release 2) addition.  The purpose of T_OPTDATA is to support
			   sockmod rather than timod or bare TI streams.  If we are pedantic about
			   the TPI spec, we should not accept them even if we have them. 
			 */
#		    if !defined TIRDWR_TPI_PEDANTIC
			if ((bp = unlinkb(mp)) && bp->b_datap->db_type == M_DATA && bp->b_band == 0
			    && !(p->optdata_ind.DATA_flag & T_ODF_EX)) {
				if (msgdsize(bp) > 0) {
					tirdwr_restore_delim(p->optdata_ind.DATA_flag & T_ODF_MORE,
							     bp);
					freeb(mp);
					putnext(q, bp);
				} else {
					/*
					   Silently discard zero length normal data. 
					 */
					freeb(mp);
					freemsg(bp);
				}
			} else
#		    endif	/* !defined TIRDWR_TPI_PEDANTIC */
				tirdwr_eproto(priv, mp, bp);
			break;
#	    endif		/* defined T_OPTDATA_IND */
		case T_DISCON_IND:
			tirdwr_hangup(priv, mp, bp);
			priv->state = TS_IDLE;
			break;
		case T_ORDREL_IND:
			switch (priv->state) {
			case TS_DATA_XFER:
				tirdwr_rdzero(priv, mp, bp);
				priv->state = TS_WREQ_ORDREL;
				break;
			default:
				tirdwr_eproto(priv, mp, bp);
				break;
			}
			break;
		case T_EXDATA_IND:
		default:
			tirdwr_eproto(priv, mp, bp);
			break;
		}
		break;
	}
	case M_FLUSH:
	case M_SIG:
		putnext(q, mp);
		break;
	case M_HANGUP:
		priv->flags |= TIRDWR_HANGUP;
		priv->state = TS_IDLE;
		putnext(q, mp);
		break;
	case M_ERROR:
		priv->flags |= TIRDWR_EPROTO;
		priv->state = TS_IDLE;
		putnext(q, mp);
		break;
	default:
		/*
		   If we are pedantic about the TPI spec and TIRDWR operation, we should not permit 
		   any other STREAMS messages from being transported upstream.  To be a little more 
		   forgiving we could just pass what we don't recognize. 
		 */
#	    if defined TIRDWR_TPI_PEDANTIC || defined TIRDWR_PEDANTIC
		tirdwr_eproto(priv, mp, bp);
#	    else		/* defined TIRDWR_TPI_PEDANTIC || defined TIRDWR_PEDANTIC */
		putnext(q, mp);
#	    endif		/* defined TIRDWR_TPI_PEDANTIC || defined TIRDWR_PEDANTIC */
		break;
	}
	return (0);
}

static int
tirdwr_wput(queue_t *q, mblk_t *mp)
{
	tirdwr_t *priv = (typeof(priv)) q->q_ptr;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
#if defined _LIS_SOURCE
	if (q->q_next == NULL || OTHER(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with null q->q_next pointer.",
			TIRDWR_MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined _LIS_SOURCE */
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if (!(priv->flags & (TIRDWR_EPROTO | TIRDWR_HANGUP))) {
			if (mp->b_band == 0) {
				if (msgdsize(mp) > 0) {
					putnext(q, mp);
				} else {
					/*
					   Silently discard zero-length data 
					 */
					freemsg(mp);
				}
			} else {
				/*
				   error on banded (expedited) data 
				 */
				tirdwr_eproto(priv, mp, NULL);
			}
		} else
			freemsg(mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		/*
		   The major reason for denying these messages is that the normal use of tirdwr is
		   to dup2 standard in or standard out or both with a connected XTI device with
		   tirdwr pushed. Because the parent can still access the stream with XTI commands, 
		   these need to generate EPROTO to signal both parent and child that something is
		   wrong. 
		 */
		tirdwr_eproto(priv, mp, NULL);
		break;
#   if !defined TIRDWR_PEDANTIC
	case M_READ:
		if (!(priv->flags & (TIRDWR_EPROTO | TIRDWR_HANGUP))) {
			switch (priv->state) {
			case TS_IDLE:
				freemsg(mp);
				break;
			case TS_DATA_XFER:
				putnext(q, mp);
				break;
			case TS_WREQ_ORDREL:
				tirdwr_rdzero(priv, mp, NULL);
				break;
			default:
				freemsg(mp);
				break;
			}
		} else
			freemsg(mp);
		break;
	case M_FLUSH:
		putnext(q, mp);
		break;
#   endif			/* !defined TIRDWR_PEDANTIC */
	case M_IOCTL:
#	    if !defined TIRDWR_PEDANTIC
		if (_IOC_TYPE(iocp->ioc_cmd) != 'T') {
			putnext(q, mp);
			break;
		}
#	    endif		/* !defined TIRDWR_PEDANTIC */
		/*
		   Documentation states that XTI/TLI library commands should fail with EPROTO.
		   Also, there is never any reason why the user should be accessing lower layer
		   IOCTLs with tirdwr installed.  This acheives that effect. 
		 */
		tirdwr_eproto(priv, NULL, NULL);
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = EPROTO;
		iocp->ioc_rval = -1;
		qreply(q, mp);
		break;
	default:
		tirdwr_eproto(priv, mp, NULL);
		break;
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *  Note that LiS does not do module counting, therefore we have to do it.
 */

static int
tirdwr_push(queue_t *q)
{
	queue_t *hq;
	int err = 0;
	/*
	   Need to check for bad messages on stream head read queue. LiS does not have a freezestr
	   or much of a qprocsoff, but the stream should be effectively frozen, so we can
	   dereference the q->q_next pointer.  If anything, the q->q_next pointer should remain
	   valid until we return from the open procedure. 
	 */
	if ((hq = q->q_next) == NULL)
		err = EFAULT;
	else if (qsize(hq) > 0) {
		mblk_t *mp;
#ifdef _LIS_SOURCE
		lis_flags_t psw;
		/*
		   Under LiS we can't freeze the stream but we can lock the queue.  This is not a
		   completely satisfactory solution for SMP because another processor could be in
		   the middle of performing putq to the queue from before the push while we lock
		   the queue.  Unless LiS provides assurances that the qopen procedure of the
		   module will not be called while lower level modules are putting to the stream
		   head, this will never work. Missing an M_PROTO or M_PCPROTO message on the
		   stream head read queue will result in read(2) returning EBADMSG for default
		   stream head read ops. tirdwr should return EPROTO and refuse to push the module
		   instead. 
		 */
		LIS_QISRLOCK(hq, &psw);
#else
		unsigned long psw = freezestr(q);
#endif
		for (mp = hq->q_first; mp && !err; mp = mp->b_next)
			switch (mp->b_datap->db_type) {
			case M_PROTO:
			case M_PCPROTO:
				err = EPROTO;
			}
#ifdef _LIS_SOURCE
		LIS_QISRUNLOCK(hq, &psw);
#else
		unfreezestr(q, psw);
#endif
	}
	return (err);
}

/*
   LiS does not offer us a way to wait for an allocation.  Solaris, OSF and Linux Fast-STREAMS do. 
 */
#if !defined BPRI_WAITOK
#   if defined BPRI_FT
#	define BPRI_WAITOK BPRI_FT
#   else
#	define BPRI_WAITOK BPRI_HI
#   endif
#endif

static void
tirdwr_pop(queue_t *q)
{
	tirdwr_t *priv = (typeof(priv)) q->q_ptr;
	mblk_t *mp;
	switch (priv->state) {
	case TS_WREQ_ORDREL:
		if (!(priv->flags & TIRDWR_EPROTO)) {
			if ((mp = allocb(sizeof(struct T_ordrel_req), BPRI_WAITOK))) {
				struct T_ordrel_req *prim = ((typeof(prim)) mp->b_wptr)++;
				mp->b_datap->db_type = M_PROTO;
				prim->PRIM_type = T_ORDREL_REQ;
				putnext(priv->wq, mp);
			}
			break;
		}
		/*
		   fall through 
		 */
	case TS_DATA_XFER:
		if ((mp = allocb(sizeof(struct T_discon_req), BPRI_WAITOK))) {
			struct T_discon_req *prim = ((typeof(prim)) mp->b_wptr)++;
			mp->b_datap->db_type = M_PROTO;
			prim->PRIM_type = T_DISCON_REQ;
			prim->SEQ_number = 0;
			putnext(priv->wq, mp);
		}
		break;
	case TS_IDLE:
	default:
		break;
	}
	if ((priv->flags & TIRDWR_EPROTO)) {
		if ((mp = allocb(2, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = 0;
			*(mp->b_wptr)++ = 0;
			putnext(priv->rq, mp);
		}
#	    if defined M_ERROR_UNDOES_M_HANGUP
		priv->flags &= ~(TIRDWR_EPROTO | TIRDWR_HANGUP);
#	    else		/* defined M_ERROR_UNDOES_M_HANGUP */
		priv->flags &= ~TIRDWR_EPROTO;
#	    endif		/* defined M_ERROR_UNDOES_M_HANGUP */
	}
#   if defined M_UNHANGUP
	if ((priv->flags & TIRDWR_HANGUP)) {
		if ((mp = allocb(0, BRPI_WAITOK))) {
			mp->b_datap->db_type = M_UNHANGUP;
			putnext(priv->rq, mp);
		}
		priv->flags &= ~TIRDWR_HANGUP;
	}
#   endif			/* defined M_UNHANGUP */
}

static int
tirdwr_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	err = 0;
	if (q->q_ptr != NULL)
		goto quit;	/* already open */
	err = ENXIO;
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		goto quit;
	if ((err = tirdwr_push(q)))
		goto quit;
	err = ENOMEM;
	if (!(tirdwr_alloc_priv(q)))
		goto quit;
	qprocson(q);
	return (0);
      quit:
	MOD_DEC_USE_COUNT;
	return (ENXIO);
}

static int
tirdwr_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
#if defined _LIS_SOURCE
	/*
	   protect against some LiS bugs 
	 */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", TIRDWR_MOD_NAME,
			__FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL || OTHER(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with null q->q_next pointer.",
			TIRDWR_MOD_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined _LIS_SOURCE */
	tirdwr_pop(q);
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	tirdwr_free_priv(q);
	MOD_DEC_USE_COUNT;
	goto quit;
      quit:
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and intialization
 *
 *  =========================================================================
 */

#if defined _LFS_SOURCE

static struct fmodsw tirdwr_fmod = {
	f_name:TIRDWR_MOD_NAME,
	f_str:&tirdwr_info,
	f_flag:0,
	f_kmod:THIS_MODULE,
};

static int
tirdwr_register_module(void)
{
	int err;
	if ((err = register_strmod(&tirdwr_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}
static void
tirdwr_unregister_module(void)
{
	return (void) unregister_strmod(&tirdwr_fmod);
}

#elif defined _LIS_SOURCE

static int
tirdwr_register_module(void)
{
	int ret;
	if ((ret = lis_register_strmod(&tirdwr_info, TIRDWR_MOD_NAME)) != LIS_NULL_MID) {
		if (modid == 0)
			modid = ret;
		return (0);
	}
	/*
	   LiS is not too good on giving informative errors here 
	 */
	return (EIO);
}
static void
tirdwr_unregister_module(void)
{
	/*
	   LiS provides detailed errors here when they are discarded. 
	 */
	return (void) lis_unregister_strmod(&tirdwr_info);
}

#else
#   error ****
#   error ****  One of _LFS_SOURCE or _LIS_SOURCE must be defined
#   error ****  to compile the tirdwr module.
#   error ****
#endif

static int __init
tirdwr_init(void)
{
	int err;
#ifdef MODULE
	cmn_err(CE_NOTE, TIRDWR_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, TIRDWR_SLPASH);	/* console splash */
#endif
	if ((err = tirdwr_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", TIRDWR_MOD_NAME, -err);
		return (err);
	}
	if ((err = tirdwr_register_module())) {
		tirdwr_term_caches();
		cmn_err(CE_WARN, "%s: could not register module, err = %d", TIRDWR_MOD_NAME, -err);
		return (err);
	}
	return (0);
}

static void __exit
tirdwr_exit(void)
{
	tirdwr_unregister_module();
	tirdwr_term_caches();
	return;
}

module_init(tirdwr_init);
module_exit(tirdwr_exit);
