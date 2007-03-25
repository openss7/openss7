/*****************************************************************************

 @(#) $RCSfile: tpiperf.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/25 19:02:16 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/03/25 19:02:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tpiperf.c,v $
 Revision 0.9.2.6  2007/03/25 19:02:16  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.5  2007/03/25 06:00:54  brian
 - flush corrections

 Revision 0.9.2.4  2007/03/25 02:23:35  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.3  2007/03/25 00:53:34  brian
 - synchronization updates

 Revision 0.9.2.2  2006/10/10 10:46:20  brian
 - updates for release

 Revision 0.9.2.1  2006/08/07 22:17:14  brian
 - changes from SCTP Interop

 *****************************************************************************/

#ident "@(#) $RCSfile: tpiperf.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/25 19:02:16 $"

static char const ident[] = "$RCSfile: tpiperf.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/25 19:02:16 $";

/*
 *  This is a TPI performance testing  module for SCTP that provides some specialized intput-output
 *  controls meant largely for performance testing purpose only.
 */

#include <sys/os7/compat.h>

/*
 *  These are for TPI definitions.
 */
#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#define TPIPERF_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TPIPERF_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TPIPERF_REVISION	"OpenSS7 $RCSfile: tpiperf.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/25 19:02:16 $"
#define TPIPERF_DEVICE		"SVR 4.2 STREAMS TPI Performance Module (TPIPERF)"
#define TPIPERF_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TPIPERF_LICENSE		"GPL"
#define TPIPERF_BANNER		TPIPERF_DESCRIP		"\n" \
				TPIPERF_COPYRIGHT	"\n" \
				TPIPERF_REVISION	"\n" \
				TPIPERF_DEVICE		"\n" \
				TPIPERF_CONTACT
#define TPIPERF_SPLASH		TPIPERF_DEVICE		" - " \
				TPIPERF_REVISION

#ifdef LINUX
MODULE_AUTHOR(TPIPERF_CONTACT);
MODULE_DESCRIPTION(TPIPERF_DESCRIP);
MODULE_SUPPORTED_DEVICE(TPIPERF_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TPIPERF_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tpiperf");
#endif
#endif				/* LINUX */

#ifndef TPIPERF_MOD_NAME
#define TPIPERF_MOD_NAME	"tpiperf"
#endif				/* TPIPERF_MOD_NAME */

#ifndef TPIPERF_MOD_ID
#define TPIPERF_MOD_ID		0
#endif				/* TPIPERF_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		TPIPERF_MOD_ID
#define MOD_NAME	TPIPERF_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TPIPERF_BANNER
#else				/* MODULE */
#define MOD_BANNER	TPIPERF_SPLASH
#endif				/* MODULE */

static struct module_info tpiperf_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

static struct module_stat tpiperf_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tpiperf_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

static streamscall int tpiperf_open(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int tpiperf_close(queue_t *, int, cred_t *);

static streamscall int tpiperf_rput(queue_t *, mblk_t *);
static streamscall int tpiperf_rsrv(queue_t *);

static struct qinit tpiperf_rinit = {
	.qi_putp = tpiperf_rput,		/* Read put (message from below) */
	.qi_srvp = tpiperf_rsrv,
	.qi_qopen = tpiperf_open,	/* Each open */
	.qi_qclose = tpiperf_close,	/* Last close */
	.qi_minfo = &tpiperf_minfo,	/* Information */
	.qi_mstat = &tpiperf_rstat,	/* Statistics */
};

static streamscall int tpiperf_wput(queue_t *, mblk_t *);
static streamscall int tpiperf_wsrv(queue_t *);

static struct qinit tpiperf_winit = {
	.qi_putp = tpiperf_wput,		/* Write put (message from above) */
	.qi_srvp = tpiperf_wsrv,
	.qi_minfo = &tpiperf_minfo,	/* Information */
	.qi_mstat = &tpiperf_wstat,	/* Statistics */
};

static struct streamtab tpiperfinfo = {
	.st_rdinit = &tpiperf_rinit,	/* Upper read queue */
	.st_wrinit = &tpiperf_winit,	/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  Private Datastructure ctors and dtors
 *
 *  =========================================================================
 */
struct tpiperf {
	queue_t *rq;			/* module read queue */
	queue_t *wq;			/* module write queue */
	mblk_t *mp;			/* stored message */
	uint mode;			/* operations mode */
	uint number;			/* number of messages to send */
	uint burst;			/* number of messages to burst */
	uint streams;			/* number of streams on which to send */
	uint stream;			/* current stream number */
	uint count;			/* current message count */
	uint bytes;			/* current byte count */
	bcid_t bcid;		/* bufcall */
};

static kmem_cachep_t tpiperf_priv_cachep = NULL;

static int
tpiperf_init_caches(void)
{
	if (!tpiperf_priv_cachep
	    && !(tpiperf_priv_cachep =
		 kmem_cache_create(MOD_NAME, sizeof(struct tpiperf), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_WARN, "%s: %s: Cannot allocate tpiperf_priv_cachep", MOD_NAME,
			__FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}

static int
tpiperf_term_caches(void)
{
	if (tpiperf_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tpiperf_priv_cachep)) {
			cmn_err(CE_WARN, "%s: %s: did not destroy tpiperf_priv_cachep", MOD_NAME,
				__FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(tpiperf_priv_cachep);
#endif
	}
	return (0);
}

static struct tpiperf *
tpiperf_alloc_priv(queue_t *q)
{
	struct tpiperf *priv;

	if ((priv = kmem_cache_alloc(tpiperf_priv_cachep, GFP_ATOMIC))) {
		priv->rq = q;
		priv->wq = WR(q);
		priv->mode = 0;
		priv->streams = 1;
		priv->number = -1;
		priv->stream = 0;
		priv->count = 0;
		priv->bytes = 0;
		priv->mp = NULL;
		priv->bcid = 0;
		priv->burst = 0;
		/* Our minpsz, maxpsz matter because we are the first queue under the stream head.
		   We do not want to alter the characteristics of the transport packet sizes so we
		   copy them here. This will allow the stream head to exhibit the same behaviour as
		   before we were pushed. */
		priv->wq->q_minpsz = priv->wq->q_next->q_minpsz;
		priv->wq->q_maxpsz = priv->wq->q_next->q_maxpsz;
		q->q_ptr = WR(q)->q_ptr = priv;
	}
	return (priv);
}

static void
tpiperf_free_priv(queue_t *q)
{
	struct tpiperf *priv;

	if ((priv = (typeof(priv)) q->q_ptr)) {
		if (priv->bcid)
			unbufcall(xchg(&priv->bcid, 0));
		if (priv->mp)
			freemsg(xchg(&priv->mp, NULL));
		q->q_ptr = WR(q)->q_ptr = NULL;
		kmem_cache_free(tpiperf_priv_cachep, priv);
	}
	return;
}

static streamscall void
tpiperf_bufcall(long arg) {
	queue_t *q = (queue_t *) arg;
	qenable(q);
	return;
}

static streamscall int
tpiperf_wput(queue_t *q, mblk_t *mp)
{
	struct tpiperf *priv = (typeof(priv)) q->q_ptr;
	int db_type = mp->b_datap->db_type;

	if (unlikely((q->q_first || (q->q_flag & QSVCBUSY)) && db_type < QPCTL)) {
		if (unlikely(!putq(q, mp))) {
			swerr();
			freemsg(mp);
		}
		return (0);
	}
	if (unlikely(db_type >= QPCTL)) {
		if (likely(db_type != M_FLUSH)) {
			putnext(q, mp);
			return (0);
		} else {
			if (mp->b_rptr[0] & FLUSHR) {
				if (mp->b_rptr[0] & FLUSHBAND)
					flushband(q, mp->b_rptr[1], FLUSHDATA);
				else
					flushq(q, FLUSHDATA);
				mp->b_rptr[0] &= ~FLUSHR;
			}
			putnext(q, mp);
			return (0);
		}
	} else if (unlikely(db_type == M_IOCTL)) {
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
		int err = EINVAL;
		int rval = 0;
		int count = 0;
		int cmd;
		uint32_t *args = (typeof(args)) (mp->b_cont ? mp->b_cont->b_rptr : NULL);

		count = ioc->iocblk.ioc_count;

		switch ((cmd = ioc->iocblk.ioc_cmd)) {
		case 0:	/* none */
			priv->mode = cmd;
			__printd(("%s: set mode to NONE\n", MOD_NAME));
			goto ack;
		case 1:	/* discard */
			priv->mode = cmd;
			__printd(("%s: set mode to DISCARD\n", MOD_NAME));
			goto ack;
		case 2:	/* echo */
			priv->mode = cmd;
			__printd(("%s: set mode to ECHO\n", MOD_NAME));
			goto ack;
		case 3:	/* generator 1 stream */
			if (count == TRANSPARENT)
				goto nak;
			if (args == NULL || count < 2 * sizeof(uint32_t))
				goto nak;
			priv->mode = cmd;
			priv->number = args[0];
			priv->burst = args[1];
			priv->streams = 1;
			__printd(("%s: set mode to GENERATE, number = %u, burst = %u, streams = %u\n", MOD_NAME, priv->number, priv->burst, priv->streams));
			count = 0;
			goto ack;
		case 4:	/* generator N streams */
			if (count == TRANSPARENT)
				goto nak;
			if (args == NULL || count < 3 * sizeof(uint32_t))
				goto nak;
			priv->mode = cmd;
			priv->number = args[0];
			priv->burst = args[1];
			priv->streams = args[2];
			__printd(("%s: set mode to GENERATE, number = %u, burst = %u, streams = %u\n", MOD_NAME, priv->number, priv->burst, priv->streams));
			count = 0;
			goto ack;
		case 5:	/* collect stats with reset */
			if (count == TRANSPARENT)
				goto nak;
			if (args == NULL) {
				if ((mp->b_cont = allocb(2 * sizeof(uint32_t), BPRI_MED)) == NULL)
					goto nak;
				args = (typeof(args)) (mp->b_cont ? mp->b_cont->b_rptr : NULL);
			}
			args[0] = xchg(&priv->count, 0);
			args[1] = xchg(&priv->bytes, 0);
			count = 2 * sizeof(uint32_t);
			mp->b_cont->b_wptr = mp->b_cont->b_rptr + count;
			goto ack;
		}
		goto nak;
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_count = count;
		ioc->iocblk.ioc_rval = rval;
		ioc->iocblk.ioc_error = 0;
		qreply(q, mp);
		return (0);
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = count;
		ioc->iocblk.ioc_rval = -1;
		ioc->iocblk.ioc_error = err;
		qreply(q, mp);
		return (0);
	} else if (db_type == M_PROTO) {
		switch (priv->mode) {
		case 0:
		case 1:
		case 2:
			break;
		case 3:	/* generator 1 stream */
		case 4:	/* generator multiple streams */
			if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
				switch (*((t_scalar_t *) mp->b_rptr)) {
				case T_OPTDATA_REQ:
				case T_DATA_REQ:
				case T_EXDATA_REQ:
				{
					/* save message for later */
					if (priv->mp != NULL)
						freemsg(xchg(&priv->mp, mp));
					priv->mp = mp;
					qenable(q);
					return (0);
				}
				}
			}
			break;
		}
	}
	if (likely(bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
	} else if (unlikely(!putq(q, mp))) {
		swerr();
		freemsg(mp);
	}
	return (0);
}

static streamscall int
tpiperf_wsrv(queue_t *q)
{
	mblk_t *mp;

	while (likely((mp = getq(q)) != NULL)) {
		/* the only messages that are placed on the queue are messages that are to be
		   passed downstream without modification. */
		if (likely(bcanputnext(q, mp->b_band) || mp->b_datap->db_type >= QPCTL)) {
			putnext(q, mp);
			continue;
		}
		if (unlikely(!putbq(q, mp))) {
			swerr();
			freemsg(mp);
		}
		break;
	}
	{
		struct tpiperf *priv = (typeof(priv)) q->q_ptr;

		switch (priv->mode) {
		case 0:	/* none */
		case 1:	/* discard */
		case 2:	/* echo */
			break;
		case 3:	/* generator */
			if ((mp = priv->mp)) {
				mblk_t *dp;
				uint burst = 0;

				/* start generating immediately */
				for (;;) {
					if (likely((dp = dupmsg(mp)) != NULL)) {
						if (likely(mp->b_cont != NULL))
							priv->bytes += msgsize(mp->b_cont);
						priv->count++;
						burst++;
						if (likely(bcanputnext(q, dp->b_band))) {
							putnext(q, dp);
							if (priv->count > priv->number) {
								priv->mode = 0;
								if ((dp = allocb(2 * sizeof(t_scalar_t), BPRI_MED))) {
									dp->b_datap->db_type = M_PROTO;
									*((t_scalar_t *)dp->b_wptr) = T_DISCON_REQ;
									dp->b_wptr += sizeof(t_scalar_t);
									*((t_scalar_t *)dp->b_wptr) = 0;
									dp->b_wptr += sizeof(t_scalar_t);
									putnext(q, dp);
								}
								if ((dp = allocb(3 * sizeof(t_scalar_t), BPRI_MED))) {
									dp->b_datap->db_type = M_PROTO;
									*((t_scalar_t *)dp->b_wptr) = T_DISCON_IND;
									dp->b_wptr += sizeof(t_scalar_t);
									*((t_scalar_t *)dp->b_wptr) = 0;
									dp->b_wptr += sizeof(t_scalar_t);
									*((t_scalar_t *)dp->b_wptr) = 0;
									dp->b_wptr += sizeof(t_scalar_t);
									put(priv->rq, dp);
								}
								break;
							} else if (burst > priv->burst) {
								qenable(q);
								break;
							}
							continue;
						}
						if (unlikely(!putbq(q, dp))) {
							swerr();
							freemsg(dp);
						}
						break;
					}
				}
				break;
			}
			break;
		case 4:	/* generator multiple streams */
			/* FIXME: this is broken: mp is NULL! */
			if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
				switch (*((t_scalar_t *) mp->b_rptr)) {
				case T_OPTDATA_IND:
				case T_DATA_IND:
				case T_EXDATA_IND:
				{
					mblk_t *bp, *dp;

					/* save message for later */
					if (priv->mp != NULL)
						freemsg(xchg(&priv->mp, mp));
					/* start generating immediately */
					for (;;) {
						if (likely((bp = copyb(mp)) != NULL)) {
							if (likely((dp = mp->b_cont) == NULL || (dp = dupmsg(dp)) != NULL)) {
								if (likely((bp->b_cont = dp) != NULL))
									priv->bytes += msgsize(dp);
								priv->count++;
								if (likely(bcanput(q, bp->b_band))) {
									put(q, bp);
									continue;
								}
							} else {
								freeb(bp);
								if (mp->b_cont) {
									/* need bufcall */
									if (priv->bcid)
										unbufcall(xchg(&priv->bcid, NULL));
									priv->bcid = bufcall(msgsize(mp->b_cont), BPRI_MED, &tpiperf_bufcall, (long) q);
								}
							}
						} else {
							/* need bufcall */
							if (priv->bcid)
								unbufcall(xchg(&priv->bcid, NULL));
							priv->bcid = bufcall(mp->b_wptr - mp->b_rptr, BPRI_MED, &tpiperf_bufcall, (long) q);
						}
						if (unlikely(!putbq(q, bp))) {
							swerr();
							freemsg(bp);
						}
						break;
					}
					return (0);
				}
				}
			}
			break;
		}
	}
	return (0);
}

static streamscall int
tpiperf_rput(queue_t *q, mblk_t *mp)
{
	struct tpiperf *priv = (typeof(priv)) q->q_ptr;
	int db_type = mp->b_datap->db_type;

	if (unlikely((q->q_first || (q->q_flag & QSVCBUSY)) && db_type < QPCTL)) {
		if (unlikely(!putq(q, mp))) {
			swerr();
			freemsg(mp);
		}
		return (0);
	}
	if (likely(db_type == M_PROTO)) {
		switch (priv->mode) {
		case 0:	/* nothing */
			break;
		case 1:	/* discard */
		case 3:	/* generator */
			if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
				switch (*((t_scalar_t *) mp->b_rptr)) {
				case T_OPTDATA_IND:
				case T_DATA_IND:
				case T_EXDATA_IND:
					freemsg(mp);
					return (0);
				}
			}
			break;
		case 2:	/* echo */
			if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
				switch (*((t_scalar_t *) mp->b_rptr)) {
				case T_OPTDATA_IND:
					*((t_scalar_t *) mp->b_rptr) = T_OPTDATA_REQ;
					goto echo_it;
				case T_DATA_IND:
					*((t_scalar_t *) mp->b_rptr) = T_DATA_REQ;
					goto echo_it;
				case T_EXDATA_IND:
					*((t_scalar_t *) mp->b_rptr) = T_EXDATA_REQ;
					goto echo_it;
				}
			}
			break;
		}
	} else if (unlikely(db_type >= QPCTL)) {
		if (likely(db_type != M_FLUSH)) {
			putnext(q, mp);
			return (0);
		} else {
			if (mp->b_rptr[0] & FLUSHR) {
				if (mp->b_rptr[0] & FLUSHBAND)
					flushband(q, mp->b_rptr[1], FLUSHDATA);
				else
					flushq(q, FLUSHDATA);
				mp->b_rptr[0] &= ~FLUSHR;
			}
			putnext(q, mp);
			return (0);
		}
	}
	goto pass_it;
      echo_it:
	if (likely(bcanputnext(priv->wq, mp->b_band))) {
		putnext(priv->wq, mp);
	} else if (unlikely(!putq(priv->wq, mp))) {
		swerr();
		freemsg(mp);
	}
	return (0);
      pass_it:
	if (likely(bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
	} else if (unlikely(!putq(q, mp))) {
		swerr();
		freemsg(mp);
	}
	return (0);
}

static streamscall int
tpiperf_rsrv(queue_t *q)
{
	mblk_t *mp;

	while (likely((mp = getq(q)) != NULL)) {
		/* the only messages that are placed on the queue are messages that are to be
		   passed upstream without modification. */
		if (likely(bcanputnext(q, mp->b_band) || mp->b_datap->db_type >= QPCTL)) {
			putnext(q, mp);
			continue;
		} else if (unlikely(!putbq(q, mp))) {
			swerr();
			freemsg(mp);
			break;
		}
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

static streamscall int
tpiperf_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err = 0;

	if (q->q_ptr != NULL)
		goto quit;	/* already open */
	err = ENXIO;
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		goto quit;
	err = ENOMEM;
	if (!tpiperf_alloc_priv(q))
		goto quit;
	err = 0;
	qprocson(q);
      quit:
	return (err);
}

static streamscall int
tpiperf_close(queue_t *q, int oflag, cred_t *crp)
{
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
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	tpiperf_free_priv(q);
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

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the TIMOD module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw tpiperf_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tpiperfinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC int
tpiperf_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&tpiperf_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
tpiperf_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&tpiperf_fmod)) < 0)
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
tpiperf_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&tpiperfinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&tpiperfinfo);
		return (err);
	}
	return (0);
}

STATIC int
tpiperf_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&tpiperfinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
tpiperfinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = tpiperf_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = tpiperf_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		tpiperf_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
tpiperfterminate(void)
{
	int err;

	if ((err = tpiperf_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = tpiperf_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(tpiperfinit);
module_exit(tpiperfterminate);

#endif				/* LINUX */
