/*****************************************************************************

 @(#) $RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2007/08/15 05:35:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/08/15 05:35:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tirdwr.c,v $
 Revision 0.9.2.34  2007/08/15 05:35:34  brian
 - GPLv3 updates

 Revision 0.9.2.33  2007/08/14 04:00:55  brian
 - GPLv3 header update

 Revision 0.9.2.32  2007/08/03 13:36:47  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.31  2007/07/14 01:37:10  brian
 - make license explicit, add documentation

 Revision 0.9.2.30  2007/04/12 20:07:06  brian
 - changes from performance testing and misc bug fixes

 Revision 0.9.2.29  2007/03/25 19:02:39  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.28  2007/03/25 02:23:40  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.27  2007/03/25 00:53:44  brian
 - synchronization updates

 Revision 0.9.2.26  2006/09/26 00:51:30  brian
 - rationalized to stacks package

 *****************************************************************************/

#ident "@(#) $RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2007/08/15 05:35:34 $"

static char const ident[] =
    "$RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2007/08/15 05:35:34 $";

#include <sys/os7/compat.h>

/*
   These are for TPI definitions 
 */
#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

// #define TIRDWR_PEDANTIC
// #define TIRDWR_TPI_PEDANTIC

#define TIRDWR_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TIRDWR_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TIRDWR_REVISION		"OpenSS7 $RCSfile: tirdwr.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2007/08/15 05:35:34 $"
#define TIRDWR_DEVICE		"SVR 4.2 STREAMS Read Write Module for XTI/TLI Devices (TIRDWR)"
#define TIRDWR_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TIRDWR_LICENSE		"GPL"
#define TIRDWR_BANNER		TIRDWR_DESCRIP		"\n" \
				TIRDWR_COPYRIGHT	"\n" \
				TIRDWR_REVISION		"\n" \
				TIRDWR_DEVICE		"\n" \
				TIRDWR_CONTACT
#define TIRDWR_SPLASH		TIRDWR_DEVICE		" - " \
				TIRDWR_REVISION

#ifdef LINUX
MODULE_AUTHOR(TIRDWR_CONTACT);
MODULE_DESCRIPTION(TIRDWR_DESCRIP);
MODULE_SUPPORTED_DEVICE(TIRDWR_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TIRDWR_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tirdwr");
#endif
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_TIRDWR_NAME
//#define CONFIG_STREAMS_TIRDWR_NAME "timod"
#error "CONFIG_STREAMS_TIRDWR_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_TIRDWR_MODID
//#define CONFIG_STREAMS_TIRDWR_MODID "5060"
#error "CONFIG_STREAMS_TIRDWR_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_TIRDWR_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for TIRDWR. (0 for allocation.)");

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_NAME	CONFIG_STREAMS_TIRDWR_NAME

static struct module_info tirdwr_minfo = {
	.mi_idnum = CONFIG_STREAMS_TIRDWR_MODID,	/* Module ID number */
	.mi_idname = CONFIG_STREAMS_TIRDWR_NAME,	/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 0,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

static struct module_stat tirdwr_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tirdwr_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int tirdwr_open(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int tirdwr_close(queue_t *, int, cred_t *);

static streamscall int tirdwr_rput(queue_t *q, mblk_t *mp);
static streamscall int tirdwr_wput(queue_t *q, mblk_t *mp);

static struct qinit tirdwr_rinit = {
	.qi_putp = tirdwr_rput,		/* Read put (message from below) */
	.qi_qopen = tirdwr_open,	/* Each open */
	.qi_qclose = tirdwr_close,	/* Last close */
	.qi_minfo = &tirdwr_minfo,	/* Information */
	.qi_mstat = &tirdwr_rstat,	/* Statistics */
};

static struct qinit tirdwr_winit = {
	.qi_putp = tirdwr_wput,		/* Write put (message from above) */
	.qi_minfo = &tirdwr_minfo,	/* Information */
	.qi_mstat = &tirdwr_wstat,	/* Statistics */
};

static struct streamtab tirdwrinfo = {
	.st_rdinit = &tirdwr_rinit,	/* Upper read queue */
	.st_wrinit = &tirdwr_winit,	/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  Private Datastructure ctors and dtors
 *
 *  =========================================================================
 */
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

static kmem_cachep_t tirdwr_priv_cachep = NULL;

static int
tirdwr_init_caches(void)
{
	if (!tirdwr_priv_cachep
	    && !(tirdwr_priv_cachep =
		 kmem_cache_create(MOD_NAME, sizeof(tirdwr_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_WARN, "%s: %s: Cannot allocate tirdwr_priv_cachep", MOD_NAME,
			__FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}

static int
tirdwr_term_caches(void)
{
	if (tirdwr_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tirdwr_priv_cachep)) {
			cmn_err(CE_WARN, "%s: %s: did not destroy tirdwr_priv_cachep", MOD_NAME,
				__FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(tirdwr_priv_cachep);
#endif
	}
	return (0);
}

static tirdwr_t *
tirdwr_alloc_priv(queue_t *q)
{
	tirdwr_t *priv;

	if ((priv = kmem_cache_alloc(tirdwr_priv_cachep, GFP_ATOMIC))) {
		priv->rq = q;
		priv->wq = WR(q);
		priv->hq = q->q_next;
		priv->state = TS_DATA_XFER;	/* assume we are in the correct initial state */
		priv->flags = 0;
		priv->rdzero_bcid = 0;
		priv->hangup_bcid = 0;
		priv->eproto_bcid = 0;
		/* we are a module with no service routine so our hiwat, lowat shouldn't matter;
		   however, our minpsz, maxpsz do because we are the first queue under the stream
		   head.  We do not want to alter the characteristics of the transport packet sizes
		   so we copy them here. This will allow the stream head to exhibit the same
		   behaviour as before we were pushed. */
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
static void streamscall tirdwr_rdzero_bc(long arg);
static void streamscall tirdwr_hangup_bc(long arg);
static void streamscall tirdwr_eproto_bc(long arg);

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
			cmn_err(CE_WARN, "%s: could not allocate rdzero buffer call", MOD_NAME);
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
		/* LiS does not have a reliable putnextctl() */
		if ((mp = allocb(0, BPRI_HI))) {
			mp->b_datap->db_type = M_HANGUP;
			/* LiS doesn't have a reliable putnext() either :/ */
			putnext(priv->rq, mp);
			priv->flags |= TIRDWR_HANGUP;
			return;
		}
		if (!(priv->hangup_bcid = bufcall(0, BPRI_HI, &tirdwr_hangup_bc, (long) priv)))
			cmn_err(CE_WARN, "%s: could not allocate hangup buffer call", MOD_NAME);
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
			cmn_err(CE_WARN, "%s: could not allocate eproto buffer call", MOD_NAME);
	}
}

static void streamscall
tirdwr_rdzero_bc(long arg)
{
	tirdwr_t *priv = (typeof(priv)) arg;

	if (xchg(&priv->rdzero_bcid, 0) != 0)
		tirdwr_hangup((tirdwr_t *) arg, NULL, NULL);
}
static void streamscall
tirdwr_hangup_bc(long arg)
{
	tirdwr_t *priv = (typeof(priv)) arg;

	if (xchg(&priv->hangup_bcid, 0) != 0)
		tirdwr_hangup((tirdwr_t *) arg, NULL, NULL);
}
static void streamscall
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
	/* Although SVR 4 documentation says that we ignore message delimitors, we restore message
	   delimitors in case the RMSGN or RMSGD read options are set at the stream head. Otherwise
	   they are ignored anyway. */
	if (flag)
		mp->b_flag &= ~MSGDELIM;
	else
		mp->b_flag |= MSGDELIM;
#endif
	return;
}

static streamscall int
tirdwr_rput(queue_t *q, mblk_t *mp)
{
	tirdwr_t *priv = (typeof(priv)) q->q_ptr;
	mblk_t *bp = NULL;

#if defined LIS
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with null q->q_next pointer.",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	switch (mp->b_datap->db_type) {
	case M_DATA:
		/* There is a problem here right off the bat.  Although a TPI compliant provider
		   must accept M_DATA written (instead of T_DATA_REQ), it should not generate
		   M_DATA on read.  These should only be T_DATA_IND.  tirdwr can be pushed over
		   other modules, however, and an underlying module (e.g. timod) might be stripping 
		   T_DATA_IND blocks, so we process them anyway. */
#	    if !defined TIRDWR_TPI_PEDANTIC
		if (mp->b_band == 0) {
			if (msgdsize(mp) > 0) {
				putnext(q, mp);
			} else {
				/* Silently discard zero length normal data. */
				freemsg(mp);
			}
		} else
#	    endif		/* defined TIRDWR_TPI_PEDANTIC */
			tirdwr_eproto(priv, mp, bp);
		break;
	case M_PCPROTO:
#	    if defined TIRDWR_TPI_PEDANTIC
		/* We could be forgiving and accept M_PCPROTO mesages that should actually be sent
		   as M_PROTO messages; however, a transport service provider that sends M_PCPROTO
		   for data or disconnect messages is in violation of the TPI specifications. */
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
					/* Silently discard zero length normal data. */
					freeb(mp);
					freemsg(bp);
				}
			} else
				tirdwr_eproto(priv, mp, bp);
			break;
#	    if defined T_OPTDATA_IND
		case T_OPTDATA_IND:
			/* T_OPTDATA is not always provided by the tihdr and is a more recent TPI
			   (Version 5 Release 2) addition.  The purpose of T_OPTDATA is to support
			   sockmod rather than timod or bare TI streams.  If we are pedantic about
			   the TPI spec, we should not accept them even if we have them. */
#		    if !defined TIRDWR_TPI_PEDANTIC
			if ((bp = unlinkb(mp)) && bp->b_datap->db_type == M_DATA && bp->b_band == 0
			    && !(p->optdata_ind.DATA_flag & T_ODF_EX)) {
				if (msgdsize(bp) > 0) {
					tirdwr_restore_delim(p->optdata_ind.DATA_flag & T_ODF_MORE,
							     bp);
					freeb(mp);
					putnext(q, bp);
				} else {
					/* Silently discard zero length normal data. */
					freeb(mp);
					freemsg(bp);
				}
			} else
#		    endif	/* !defined TIRDWR_TPI_PEDANTIC */
				tirdwr_eproto(priv, mp, bp);
			break;
#	    endif		/* defined T_OPTDATA_IND */
		case T_DISCON_IND:
			/* need to pass up any data belonging to the disconnect indication first. */
			if ((bp = unlinkb(mp)) && bp->b_datap->db_type == M_DATA && bp->b_band == 0) {
				if (msgdsize(bp) > 0) {
					putnext(q, bp);
					bp = NULL;
				}
			}
			tirdwr_hangup(priv, mp, bp);
			priv->state = TS_IDLE;
			break;
		case T_ORDREL_IND:
			/* need to pass up any data belonging to the orderly release first. */
			switch (priv->state) {
			case TS_DATA_XFER:
				if ((bp = unlinkb(mp)) && bp->b_datap->db_type == M_DATA
				    && bp->b_band == 0) {
					if (msgdsize(bp) > 0) {
						putnext(q, bp);
						bp = NULL;
					}
				}
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
		/* If we are pedantic about the TPI spec and TIRDWR operation, we should not permit 
		   any other STREAMS messages from being transported upstream.  To be a little more 
		   forgiving we could just pass what we don't recognize. */
#	    if defined TIRDWR_TPI_PEDANTIC || defined TIRDWR_PEDANTIC
		tirdwr_eproto(priv, mp, bp);
#	    else		/* defined TIRDWR_TPI_PEDANTIC || defined TIRDWR_PEDANTIC */
		putnext(q, mp);
#	    endif		/* defined TIRDWR_TPI_PEDANTIC || defined TIRDWR_PEDANTIC */
		break;
	}
	return (0);
}

static streamscall int
tirdwr_wput(queue_t *q, mblk_t *mp)
{
	tirdwr_t *priv = (typeof(priv)) q->q_ptr;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;

#if defined LIS
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with null q->q_next pointer.",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if (!(priv->flags & (TIRDWR_EPROTO | TIRDWR_HANGUP))) {
			if (mp->b_band == 0) {
				if (msgdsize(mp) > 0) {
					putnext(q, mp);
				} else {
					/* Silently discard zero-length data */
					freemsg(mp);
				}
			} else {
				/* error on banded (expedited) data */
				tirdwr_eproto(priv, mp, NULL);
			}
		} else
			freemsg(mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		/* The major reason for denying these messages is that the normal use of tirdwr is
		   to dup2 standard in or standard out or both with a connected XTI device with
		   tirdwr pushed. Because the parent can still access the stream with XTI commands, 
		   these need to generate EPROTO to signal both parent and child that something is
		   wrong. */
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
		/* Documentation states that XTI/TLI library commands should fail with EPROTO.
		   Also, there is never any reason why the user should be accessing lower layer
		   IOCTLs with tirdwr installed.  This acheives that effect. */
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = EPROTO;
		iocp->ioc_rval = -1;
		qreply(q, mp);
		tirdwr_eproto(priv, NULL, NULL);	/* XXX */
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

	/* Need to check for bad messages on stream head read queue. LiS does not have a freezestr
	   or much of a qprocsoff, but the stream should be effectively frozen, so we can
	   dereference the q->q_next pointer.  If anything, the q->q_next pointer should remain
	   valid until we return from the open procedure. */
	if ((hq = q->q_next) == NULL)
		err = EFAULT;
	else if (qsize(hq) > 0) {
		mblk_t *mp;

#ifdef LIS
		lis_flags_t psw;

		/* Under LiS we can't freeze the stream but we can lock the queue.  This is not a
		   completely satisfactory solution for SMP because another processor could be in
		   the middle of performing putq to the queue from before the push while we lock
		   the queue.  Unless LiS provides assurances that the qopen procedure of the
		   module will not be called while lower level modules are putting to the stream
		   head, this will never work. Missing an M_PROTO or M_PCPROTO message on the
		   stream head read queue will result in read(2) returning EBADMSG for default
		   stream head read ops. tirdwr should return EPROTO and refuse to push the module
		   instead. */
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
#ifdef LIS
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
				struct T_ordrel_req *prim = (typeof(prim)) mp->b_wptr;

				mp->b_wptr = (unsigned char *) (prim + 1);
				mp->b_datap->db_type = M_PROTO;
				prim->PRIM_type = T_ORDREL_REQ;
				putnext(priv->wq, mp);
			}
			break;
		}
		/* fall through */
	case TS_DATA_XFER:
		if ((mp = allocb(sizeof(struct T_discon_req), BPRI_WAITOK))) {
			struct T_discon_req *prim = (typeof(prim)) mp->b_wptr;

			mp->b_wptr = (unsigned char *) (prim + 1);
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
		if ((mp = allocb(0, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_UNHANGUP;
			putnext(priv->rq, mp);
		}
		priv->flags &= ~TIRDWR_HANGUP;
	}
#   endif			/* defined M_UNHANGUP */
}

static streamscall int
tirdwr_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err = 0;

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
	return (err);
}

static streamscall int
tirdwr_close(queue_t *q, int oflag, cred_t *crp)
{
	queue_t *rq, *wq;

	(void) oflag;
	(void) crp;
	(void) rq;
	(void) wq;
#if defined LIS
	/* protect against some LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", MOD_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with null q->q_next pointer.",
			MOD_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	rq = RD(q);
	wq = WR(q);
	__ensure(rq->q_next != NULL, goto skip_pop);
	__ensure(wq->q_next != NULL, goto skip_pop);
	tirdwr_pop(q);
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	tirdwr_free_priv(q);
	goto quit;
      quit:
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw tirdwr_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tirdwrinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC int
tirdwr_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&tirdwr_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
tirdwr_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&tirdwr_fmod)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
tirdwr_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&tirdwrinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&tirdwrinfo);
		return (err);
	}
	return (0);
}

STATIC int
tirdwr_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&tirdwrinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
tirdwrinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_TIRDWR_MODULE
	cmn_err(CE_NOTE, TIRDWR_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, TIRDWR_SPLASH);	/* banner message */
#endif
	if ((err = tirdwr_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = tirdwr_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		tirdwr_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
tirdwrterminate(void)
{
	int err;

	if ((err = tirdwr_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = tirdwr_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(tirdwrinit);
module_exit(tirdwrterminate);

#endif				/* LINUX */
