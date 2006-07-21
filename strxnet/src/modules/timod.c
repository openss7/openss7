/*****************************************************************************

 @(#) $RCSfile: timod.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/07/07 21:14:53 $

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

 Last Modified $Date: 2006/07/07 21:14:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: timod.c,v $
 Revision 0.9.2.24  2006/07/07 21:14:53  brian
 - correct compile back to RH 7.2

 Revision 0.9.2.23  2006/05/25 08:39:14  brian
 - added noinline in strategic places

 Revision 0.9.2.22  2006/05/22 02:09:12  brian
 - changes from performance testing

 Revision 0.9.2.21  2006/03/03 11:35:17  brian
 - 32/64-bit compatibility

 *****************************************************************************/

#ident "@(#) $RCSfile: timod.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/07/07 21:14:53 $"

static char const ident[] =
    "$RCSfile: timod.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/07/07 21:14:53 $";

/*
 *  This is TIMOD an XTI library interface module for TPI Version 2 transport
 *  service providers supporting the T_ADDR_REQ primitive.
 *
 *  This module is suitable for supporting both Ole Husgaard's XTI library as
 *  (that does not use T_GETPROTADDR) or the more complete OpenSS7 libxnet XTI
 *  library that comes with the Linux Fast-STREAMS release.  The libxnet
 *  library has superior syncrhonization across a fork() and provides a
 *  conforming t_sync() library call.
 */
#include <sys/os7/compat.h>

/*
   These are for TPI definitions 
 */
#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif
#if defined HAVE_TIMOD_H
#   include <timod.h>
#else
#   include <sys/timod.h>
#endif

#define TIMOD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TIMOD_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TIMOD_REVISION	"OpenSS7 $RCSfile: timod.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/07/07 21:14:53 $"
#define TIMOD_DEVICE	"SVR 4.2 STREAMS XTI Library Module for TLI Devices (TIMOD)"
#define TIMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TIMOD_LICENSE	"GPL"
#define TIMOD_BANNER	TIMOD_DESCRIP	"\n" \
			TIMOD_COPYRIGHT	"\n" \
			TIMOD_REVISION	"\n" \
			TIMOD_DEVICE	"\n" \
			TIMOD_CONTACT
#define TIMOD_SPLASH	TIMOD_DEVICE	" - " \
			TIMOD_REVISION

#ifdef LINUX
MODULE_AUTHOR(TIMOD_CONTACT);
MODULE_DESCRIPTION(TIMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(TIMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TIMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-timod");
#endif
#endif				/* LINUX */

#ifndef TIMOD_MOD_NAME
#define TIMOD_MOD_NAME		"timod"
#endif

#ifndef TIMOD_MOD_ID
#define TIMOD_MOD_ID		0
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		TIMOD_MOD_ID
#define MOD_NAME	TIMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TIMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	TIMOD_SPLASH
#endif				/* MODULE */

static struct module_info timod_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

static struct module_stat timod_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat timod_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

static streamscall int timod_open(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int timod_close(queue_t *, int, cred_t *);

static streamscall int timod_rput(queue_t *, mblk_t *);
static streamscall int timod_wput(queue_t *, mblk_t *);

static struct qinit timod_rinit = {
	.qi_putp = timod_rput,		/* Read put (message from below) */
	.qi_qopen = timod_open,		/* Each open */
	.qi_qclose = timod_close,	/* Last close */
	.qi_minfo = &timod_minfo,	/* Information */
	.qi_mstat = &timod_rstat,	/* Statistics */
};

static struct qinit timod_winit = {
	.qi_putp = timod_wput,		/* Write put (message from above) */
	.qi_minfo = &timod_minfo,	/* Information */
	.qi_mstat = &timod_wstat,	/* Statistics */
};

MODULE_STATIC struct streamtab timodinfo = {
	.st_rdinit = &timod_rinit,	/* Upper read queue */
	.st_wrinit = &timod_winit,	/* Upper write queue */
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Private Datastructure ctors and dtors
 *  
 *  -------------------------------------------------------------------------
 */
struct timod {
	queue_t *rq;			/* module read queue */
	queue_t *wq;			/* module write queue */
	queue_t *hq;			/* stream head read queue */
	ulong state;			/* module state */
	ulong oldstate;			/* module state */
	ulong flags;			/* module flags */
	mblk_t *iocblk;			/* ioctl being processed */
	ulong qlen;			/* bind ack queue length */
	ulong cons;			/* outstanding connection indications */
};

static kmem_cache_t *timod_priv_cachep = NULL;

static __unlikely int
timod_init_caches(void)
{
	if (!timod_priv_cachep
	    && !(timod_priv_cachep =
		 kmem_cache_create(MOD_NAME, sizeof(struct timod), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_WARN, "%s: %s: Cannot allocate timod_priv_cachep", MOD_NAME,
			__FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}

static __unlikely int
timod_term_caches(void)
{
	if (timod_priv_cachep) {
		if (kmem_cache_destroy(timod_priv_cachep)) {
			cmn_err(CE_WARN, "%s: %s: did not destroy timod_priv_cachep", MOD_NAME,
				__FUNCTION__);
			return (-EBUSY);
		}
	}
	return (0);
}

static struct timod *
timod_alloc_priv(queue_t *q)
{
	struct timod *priv;

	if ((priv = kmem_cache_alloc(timod_priv_cachep, SLAB_ATOMIC))) {
		bzero(priv, sizeof(*priv));
		priv->rq = q;
		priv->wq = WR(q);
		priv->hq = q->q_next;
		priv->state = TS_UNBND;	/* assume we are in the correct initial state */
		priv->oldstate = -1UL;
		priv->flags = 0;
		priv->qlen = 0;
		priv->cons = 0;
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
timod_free_priv(queue_t *q)
{
	struct timod *priv;

	if ((priv = (typeof(priv)) q->q_ptr)) {
		q->q_ptr = WR(q)->q_ptr = NULL;
		priv->rq = NULL;
		priv->wq = NULL;
		priv->hq = NULL;
		priv->state = -1UL;
		priv->oldstate = -1UL;
		priv->flags = 0;
		priv->qlen = 0;
		priv->cons = 0;
		if (priv->iocblk)
			freemsg(xchg(&priv->iocblk, NULL));
		kmem_cache_free(timod_priv_cachep, priv);
	}
	return;
}

static __hot_put int
split_buffer(mblk_t *mp, int offset)
{
	unsigned char *ptr = mp->b_rptr + offset;

	if (ptr > mp->b_wptr)
		return (-EINVAL);
	if (ptr < mp->b_wptr) {
		mblk_t *dp;

		if ((dp = copyb(mp))) {
			dp->b_datap->db_type = M_DATA;
			dp->b_rptr += offset;
			mp->b_datap->db_type = M_PROTO;
			mp->b_wptr = ptr;
			linkb(mp, dp);
			return (0);
		}
		return (-ENOSR);
	}
	mp->b_datap->db_type = M_PROTO;
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  PUT routine
 *  
 *  -------------------------------------------------------------------------
 */
static noinline streams_fastcall __unlikely int
timod_rput_slow(queue_t *q, mblk_t *mp)
{
	struct timod *priv = q->q_ptr;

#if defined LIS
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	switch (mp->b_datap->db_type) {
		union T_primitives *p;
		struct iocblk *ioc;
		mblk_t *dp;

	case M_PCPROTO:
	case M_PROTO:
		p = (typeof(p)) mp->b_rptr;
		if (!(dp = xchg(&priv->iocblk, NULL)))
			goto pass_along;
		/* we are expecting a response, but only intercept the expected response, pass
		   along others */
		ioc = (typeof(ioc)) dp->b_rptr;
		switch (p->type) {
		case T_OK_ACK:
			switch (ioc->ioc_cmd) {
			case O_TI_UNBIND:
			case TI_UNBIND:
				priv->state = TS_UNBND;
				priv->qlen = 0;
				goto ack_it;
#ifdef TI_ACCEPT
			case TI_ACCEPT:
#endif
			case TI_SETMYNAME:
				switch (priv->state) {
				case TS_WACK_CRES:
					/* but could be TS_DATA_XFER for same stream connect */
					priv->state = (priv->cons && --priv->cons > 0)
					    ? TS_WRES_CIND : TS_IDLE;
					break;
				case TS_WACK_DREQ7:
					priv->state = (priv->cons && --priv->cons > 0)
					    ? TS_WRES_CIND : TS_IDLE;
					break;
				}
				goto ack_it;
#ifdef TI_CONNECT
			case TI_CONNECT:
#endif
			case TI_SETPEERNAME:
				switch (priv->state) {
				case TS_WACK_CREQ:
					priv->state = TS_WCON_CREQ;
					break;
				case TS_WACK_DREQ6:
				case TS_WACK_DREQ9:
				case TS_WACK_DREQ10:
				case TS_WACK_DREQ11:
					priv->state = TS_IDLE;
					break;
				}
				priv->state = TS_WCON_CREQ;
				goto ack_it;
#ifdef TI_DISCONNECT
			case TI_DISCONNECT:
				priv->state = TS_IDLE;
				goto ack_it;
#endif
			}
			break;
		case T_INFO_ACK:
			switch (ioc->ioc_cmd) {
			case O_TI_GETINFO:
			case TI_GETINFO:
				priv->state = priv->oldstate;
				goto ack_it;
			}
			break;
		case T_BIND_ACK:
			switch (ioc->ioc_cmd) {
			case O_TI_BIND:
			case TI_BIND:
				priv->state = TS_IDLE;
				priv->qlen = p->bind_ack.CONIND_number;
				goto ack_it;
			}
			break;
		case T_OPTMGMT_ACK:
			switch (ioc->ioc_cmd) {
			case O_TI_OPTMGMT:
			case TI_OPTMGMT:
				priv->state = priv->oldstate;
				goto ack_it;
			}
			break;
		case T_ADDR_ACK:
			switch (ioc->ioc_cmd) {
#ifdef TI_GETPROTADDR
			case TI_GETPROTADDR:
#endif
			case TI_GETADDRS:
			case TI_GETMYNAME:
			case TI_GETPEERNAME:
				priv->state = priv->oldstate;
				goto ack_it;
			}
			break;
#ifdef T_CAPABILITY_ACK
		case T_CAPABILITY_ACK:
			switch (ioc->ioc_cmd) {
			case TI_CAPABILITY:
				priv->state = priv->oldstate;
				goto ack_it;
			}
			break;
#endif
		case T_ERROR_ACK:
			priv->state = priv->oldstate;
			dp->b_datap->db_type = M_IOCACK;
			linkb(dp, mp);
			ioc->ioc_count = mp->b_wptr - mp->b_rptr;
			ioc->ioc_error = 0;
			ioc->ioc_rval = (p->error_ack.UNIX_error << 8) | p->error_ack.TLI_error;
			if (ioc->ioc_rval == 0 || ioc->ioc_rval == TSYSERR)
				ioc->ioc_rval = (EINVAL << 8) | TSYSERR;
			putnext(q, dp);
			return (0);
		      ack_it:
			dp->b_datap->db_type = M_IOCACK;
			linkb(dp, mp);
			ioc->ioc_count = mp->b_wptr - mp->b_rptr;
			ioc->ioc_error = 0;
			ioc->ioc_rval = 0;
			putnext(q, dp);
			return (0);
		}
		/* not expecting anything, but do state tracking */
	      pass_along:
		switch (p->type) {
		case T_CONN_IND:
			priv->oldstate = priv->state;
			priv->cons++;
			priv->state = TS_WRES_CIND;
			break;
		case T_CONN_CON:
			priv->oldstate = priv->state;
			priv->state = TS_DATA_XFER;
			break;
		case T_DISCON_IND:
			priv->oldstate = priv->state;
			if (priv->cons && --priv->cons) {
				priv->state = TS_WRES_CIND;
				break;
			}
			priv->state = TS_IDLE;
			break;
		case T_DATA_IND:
			priv->oldstate = priv->state;
			priv->state = TS_DATA_XFER;
			break;
		case T_EXDATA_IND:
			priv->oldstate = priv->state;
			priv->state = TS_DATA_XFER;
			break;
		case T_INFO_ACK:
			priv->state = priv->oldstate;
			break;
		case T_BIND_ACK:
			priv->oldstate = priv->state;
			priv->qlen = p->bind_ack.CONIND_number;
			priv->state = TS_IDLE;
			break;
		case T_ERROR_ACK:
			priv->state = priv->oldstate;
			break;
		case T_OK_ACK:
			switch ((priv->oldstate = priv->state)) {
			case TS_WACK_UREQ:
				priv->state = TS_UNBND;
				break;
			case TS_WACK_CREQ:
				priv->state = TS_WCON_CREQ;
				break;
			case TS_WACK_CRES:
				/* FIXME: depends */
				priv->state = TS_DATA_XFER;
				break;
			case TS_WACK_DREQ6:
			case TS_WACK_DREQ9:
			case TS_WACK_DREQ10:
			case TS_WACK_DREQ11:
				priv->state = TS_IDLE;
				break;
			case TS_WACK_DREQ7:
				if (priv->cons && --priv->cons) {
					priv->state = TS_WRES_CIND;
					break;
				}
				priv->state = TS_IDLE;
				break;
			}
			break;
		case T_UNITDATA_IND:
			priv->oldstate = priv->state;
			priv->state = TS_IDLE;
			break;
		case T_UDERROR_IND:
			priv->oldstate = priv->state;
			priv->state = TS_IDLE;
			break;
		case T_OPTMGMT_ACK:
			priv->state = priv->oldstate;
			break;
		case T_ORDREL_IND:
			switch ((priv->oldstate = priv->state)) {
			case TS_DATA_XFER:
				priv->state = TS_WIND_ORDREL;
				break;
			case TS_WIND_ORDREL:
				priv->state = TS_IDLE;
				break;
			}
			break;
		case T_ADDR_ACK:
			priv->state = priv->oldstate;
			break;
#ifdef T_CAPABILITY_ACK
		case T_CAPABILITY_ACK:
			priv->state = priv->oldstate;
			break;
#endif
		}
		break;
	case M_ERROR:
	case M_HANGUP:
		priv->oldstate = -1UL;
		priv->state = -1UL;
		break;
	}
	putnext(q, mp);
	return (0);
}
static streamscall __hot_in int
timod_rput(queue_t *q, mblk_t *mp)
{
#if 0
	struct timod *priv = q->q_ptr;
#endif
	union T_primitives *p;

#if 0
	prefetchw(priv);
#endif
#if defined LIS
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	/* fast path for data */
	if (likely(mp->b_datap->db_type == M_PROTO)) {
		if (likely(mp->b_wptr >= mp->b_rptr + sizeof(p->type))) {
			p = (typeof(p)) mp->b_rptr;
#if 0
			switch (p->type) {
			case T_UNITDATA_IND:
			case T_UDERROR_IND:
#if 0
				priv->oldstate = priv->state;
				priv->state = TS_IDLE;
#endif
				putnext(q, mp);
				return (0);
			case T_EXDATA_IND:
			case T_DATA_IND:
#if 0
				priv->oldstate = priv->state;
				priv->state = TS_DATA_XFER;
#endif
				putnext(q, mp);
				return (0);
			}
#else
			/* conveniently none of these are > 31 */
			if (likely((1 << p->type)
				   & ((1 << T_UNITDATA_IND) | (1 << T_UDERROR_IND) |
				      (1 << T_EXDATA_IND) | (1 << T_DATA_IND)))) {
				putnext(q, mp);
				return (0);
			}
#endif
		}
	}
	return timod_rput_slow(q, mp);
}

static noinline streams_fastcall __unlikely int
timod_wput_slow(queue_t *q, mblk_t *mp)
{
	struct timod *priv = q->q_ptr;

#if defined LIS
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	switch (mp->b_datap->db_type) {
		union T_primitives *p;
		struct iocblk *ioc;
		mblk_t *dp;
		int err;

	case M_IOCTL:
		/* Most of the ioctls provided here are to acheive atomic and thread-safe
		   operations on the stream for use by the XTI/TLI library.  Each ioctl takes a TPI 
		   message in the buffer and results in sending the TPI message downstream.  We
		   strip off and keep the io control block for latter response correlation.  We
		   also track the state of the stream and the number of outstanding connection
		   indications. */
		ioc = (typeof(ioc)) mp->b_rptr;
		err = -EFAULT;
		if (!(dp = unlinkb(mp)))
			goto error;
		dp->b_datap->db_type = (ioc->ioc_cmd == TI_GETINFO) ? M_PCPROTO : M_PROTO;
		p = (typeof(p)) dp->b_rptr;
		err = -EINVAL;
		if (ioc->ioc_count == TRANSPARENT)
			goto error;
		switch (ioc->ioc_cmd) {
		case O_TI_OPTMGMT:
			err = TNOTSUPPORT;
			goto error;
		case TI_OPTMGMT:
			if (p->type == T_OPTMGMT_REQ && ioc->ioc_count >= sizeof(p->optmgmt_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
#ifdef TS_WACK_OPTREQ
				priv->state = TS_WACK_OPTREQ;
#endif
				break;
			}
			goto error;
		case O_TI_BIND:
			err = TNOTSUPPORT;
			goto error;
		case TI_BIND:
			if (p->type == T_BIND_REQ && ioc->ioc_count >= sizeof(p->bind_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_BREQ;
				break;
			}
			goto error;
		case O_TI_GETINFO:
			err = TNOTSUPPORT;
			goto error;
		case TI_GETINFO:
			if (p->type == T_INFO_REQ && ioc->ioc_count >= sizeof(p->info_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				break;
			}
			goto error;
		case O_TI_UNBIND:
			err = TNOTSUPPORT;
			goto error;
		case TI_UNBIND:
			if (p->type == T_UNBIND_REQ && ioc->ioc_count >= sizeof(p->ok_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_UREQ;
				break;
			}
			goto error;
#if defined TI_ACCEPT
		case TI_ACCEPT:
#endif
		case TI_SETMYNAME:
			if (p->type == T_CONN_RES && ioc->ioc_count >= sizeof(p->conn_res)) {
				int doff = sizeof(p->conn_res);

				if (p->conn_res.OPT_length
				    && doff < p->conn_res.OPT_offset + p->conn_res.OPT_length)
					doff = p->conn_res.OPT_offset + p->conn_res.OPT_length;
				if ((err = split_buffer(dp, doff)) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_CRES;
				break;
			}
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_DREQ7;
				break;
			}
			goto error;
#if defined TI_CONNECT
		case TI_CONNECT:
#endif
		case TI_SETPEERNAME:
			if (p->type == T_CONN_REQ && ioc->ioc_count >= sizeof(p->conn_req)) {
				int doff = sizeof(p->conn_req);

				if (p->conn_req.OPT_length
				    && doff < p->conn_req.OPT_offset + p->conn_req.OPT_length)
					doff = p->conn_req.OPT_offset + p->conn_req.OPT_length;
				if (p->conn_req.DEST_length
				    && doff < p->conn_req.DEST_offset + p->conn_req.DEST_length)
					doff = p->conn_req.DEST_offset + p->conn_req.DEST_length;
				if ((err = split_buffer(dp, doff)) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_CREQ;
				break;
			}
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				switch ((priv->oldstate = priv->state)) {
				case TS_WCON_CREQ:
					priv->state = TS_WACK_DREQ6;
					break;
				case TS_DATA_XFER:
					priv->state = TS_WACK_DREQ9;
					break;
				case TS_WIND_ORDREL:
					priv->state = TS_WACK_DREQ10;
					break;
				case TS_WREQ_ORDREL:
					priv->state = TS_WACK_DREQ11;
					break;
				}
				break;
			}
			goto error;
		case TI_GETMYNAME:
			if (ioc->ioc_count >= sizeof(p->addr_ack)) {
				dp->b_datap->db_type = M_PCPROTO;
				p->type = T_ADDR_REQ;
				dp->b_wptr = dp->b_rptr + sizeof(p->addr_req);
				break;
			}
			goto error;
		case TI_GETPEERNAME:
			if (ioc->ioc_count >= sizeof(p->addr_ack)) {
				dp->b_datap->db_type = M_PCPROTO;
				p->type = T_ADDR_REQ;
				dp->b_wptr = dp->b_rptr + sizeof(p->addr_req);
				break;
			}
			goto error;
		case TI_GETADDRS:
			if (ioc->ioc_count >= sizeof(p->addr_ack)
			    && p->type == T_ADDR_REQ) {
				dp->b_datap->db_type = M_PCPROTO;
				priv->oldstate = priv->state;
				break;
			}
			goto error;
		case TI_SYNC:
			if (ioc->ioc_count >= sizeof(struct ti_sync_ack)) {
				int flags = ((struct ti_sync_req *) p)->tsr_flags;

				if (flags & TSRF_INFO_REQ) {
					dp->b_datap->db_type = M_PCPROTO;
					p->type = T_INFO_REQ;
					dp->b_wptr = dp->b_rptr + sizeof(p->info_req);
					break;
				} else {
				}
				if (flags & TSRF_IS_EXP_IN_RCVBUF) {
				}
				if (flags & TSRF_QLEN_REQ) {
				}
				break;
			}
			goto error;
#if 0
		case TI_GETPROTADDR:
			err = -EOPNOTSUPP;
			goto error;
		case TI_DISCONNECT:
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				switch (priv->state) {
				case TS_WCON_CREQ:
					priv->state = TS_WACK_DREQ6;
					break;
				case TS_WRES_CIND:
					priv->state = TS_WACK_DREQ7;
					break;
				case TS_DATA_XFER:
					priv->state = TS_WACK_DREQ9;
					break;
				case TS_WIND_ORDREL:
					priv->state = TS_WACK_DREQ10;
					break;
				case TS_WREQ_ORDREL:
					priv->state = TS_WACK_DREQ11;
					break;
				default:
					priv->state = -1;
					goto error;
				}
				break;
			}
			goto error;
#endif
		case TI_CAPABILITY:
#ifdef T_CAPABILITY_REQ
			if (ioc->ioc_count >= sizeof(p->capability_req)
			    && p->type == T_CAPABILITY_REQ) {
				dp->b_datap->db_type = M_PCPROTO;
				priv->oldstate = priv->state;
				break;
			}
			goto error;
#endif
		default:
			putnext(q, mp);
			return (0);
		}
		if ((mp = xchg(&priv->iocblk, mp)))
			freemsg(mp);
		putnext(q, dp);
		return (0);
	      error:
		mp->b_datap->db_type = M_IOCNAK;
		linkb(mp, dp);
		ioc->ioc_error = -err;
		ioc->ioc_rval = -1;
		qreply(q, mp);
		return (0);
	case M_PROTO:
	case M_PCPROTO:
		/* this is just to do state tracking */
		if (mp->b_wptr < mp->b_rptr + sizeof(p->type))
			break;
		p = (typeof(p)) mp->b_rptr;
		switch (p->type) {
		case T_CONN_REQ:
			priv->oldstate = priv->state;
			priv->state = TS_WACK_CREQ;
			break;
		case T_CONN_RES:
			priv->oldstate = priv->state;
			priv->state = TS_WACK_CRES;
			break;
		case T_DISCON_REQ:
			switch ((priv->oldstate = priv->state)) {
			case TS_WCON_CREQ:
				priv->state = TS_WACK_DREQ6;
				break;
			case TS_WRES_CIND:
				priv->state = TS_WACK_DREQ7;
				break;
			case TS_DATA_XFER:
				priv->state = TS_WACK_DREQ9;
				break;
			case TS_WIND_ORDREL:
				priv->state = TS_WACK_DREQ10;
				break;
			case TS_WREQ_ORDREL:
				priv->state = TS_WACK_DREQ11;
				break;
			}
			break;
		case T_DATA_REQ:
			priv->oldstate = priv->state;
			priv->state = TS_DATA_XFER;
			break;
		case T_EXDATA_REQ:
			priv->oldstate = priv->state;
			priv->state = TS_DATA_XFER;
			break;
		case T_INFO_REQ:
			priv->oldstate = priv->state;
			priv->state = priv->oldstate;
			break;
		case T_BIND_REQ:
			priv->oldstate = priv->state;
			priv->state = TS_WACK_BREQ;
			break;
		case T_UNBIND_REQ:
			priv->oldstate = priv->state;
			priv->state = TS_WACK_UREQ;
			break;
		case T_UNITDATA_REQ:
			priv->oldstate = priv->state;
			priv->state = TS_IDLE;
			break;
		case T_OPTMGMT_REQ:
			priv->oldstate = priv->state;
#ifdef TS_WACK_OPTREQ
			priv->state = TS_WACK_OPTREQ;
#endif
			break;
		case T_ORDREL_REQ:
			switch ((priv->oldstate = priv->state)) {
			case TS_WREQ_ORDREL:
				priv->state = TS_IDLE;
				break;
			case TS_DATA_XFER:
				priv->state = TS_WIND_ORDREL;
				break;
			}
			break;
		case T_ADDR_REQ:
			priv->oldstate = priv->state;
			priv->state = priv->oldstate;
			break;
#ifdef T_CAPABILITY_REQ
		case T_CAPABILITY_REQ:
			priv->oldstate = priv->state;
			priv->state = priv->oldstate;
			break;
#endif
		default:
			break;
		}
	}
	putnext(q, mp);
	return (0);
}
static streamscall __hot_out int
timod_wput(queue_t *q, mblk_t *mp)
{
#if 0
	struct timod *priv = q->q_ptr;
#endif
	union T_primitives *p;

#if 0
	prefetchw(priv);
#endif
#if defined LIS
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	/* fast path for data */
	if (likely(mp->b_datap->db_type == M_PROTO)) {
		if (likely(mp->b_wptr >= mp->b_rptr + sizeof(p->type))) {
			p = (typeof(p)) mp->b_rptr;
#if 0
			switch (p->type) {
			case T_UNITDATA_REQ:
#if 0
				priv->oldstate = priv->state;
				priv->state = TS_IDLE;
#endif
				putnext(q, mp);
				return (0);
			case T_EXDATA_REQ:
			case T_DATA_REQ:
#if 0
				priv->oldstate = priv->state;
				priv->state = TS_DATA_XFER;
#endif
				putnext(q, mp);
				return (0);
			}
#else
			/* conveniently none of these are > 31 */
			if (likely((1 << p->type)
				   & ((1 << T_UNITDATA_REQ) |
				      (1 << T_EXDATA_REQ) | (1 << T_DATA_REQ)))) {
				putnext(q, mp);
				return (0);
			}
#endif
		}
	}
	return timod_wput_slow(q, mp);
}

#define TIMOD_HANGUP	01
#define TIMOD_EPROTO	02

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */

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
timod_pop(queue_t *q)
{
	struct timod *priv = (typeof(priv)) q->q_ptr;
	mblk_t *mp;

	switch (priv->state) {
	case TS_WREQ_ORDREL:
		if (!(priv->flags & TIMOD_EPROTO)) {
			if ((mp = allocb(sizeof(struct T_discon_req), BPRI_WAITOK))) {
				struct T_discon_req *prim = (typeof(prim)) mp->b_wptr;

				mp->b_wptr = (unsigned char *) (prim + 1);
				mp->b_datap->db_type = M_PROTO;
				prim->PRIM_type = T_ORDREL_REQ;
				qreply(q, mp);
			}
		}
		/* fall through */
	case TS_DATA_XFER:
		if ((mp = allocb(sizeof(struct T_discon_req), BPRI_WAITOK))) {
			struct T_discon_req *prim = (typeof(prim)) mp->b_wptr;

			mp->b_wptr = (unsigned char *) (prim + 1);
			mp->b_datap->db_type = M_PROTO;
			prim->PRIM_type = T_DISCON_REQ;
			prim->SEQ_number = 0;
			qreply(q, mp);
		}
		break;
		break;
	case TS_IDLE:
	default:
		break;
	}
	if ((priv->flags & TIMOD_EPROTO)) {
		if ((mp = allocb(2, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = 0;
			*(mp->b_wptr)++ = 0;
			qreply(q, mp);
		}
#	    if defined M_ERROR_UNDOES_M_HANGUP
		priv->flags &= ~(TIMOD_EPROTO | TIMOD_HANGUP);
#	    else		/* defined M_ERROR_UNDOES_M_HANGUP */
		priv->flags &= ~TIMOD_EPROTO;
#	    endif		/* defined M_ERROR_UNDOES_M_HANGUP */
	}
#   if defined M_UNHANGUP
	if ((priv->flags & TIMOD_HANGUP)) {
		if ((mp = allocb(0, BRPI_WAITOK))) {
			mp->b_datap->db_type = M_UNHANGUP;
			qreply(q, mp);
		}
		priv->flags &= ~TIMOD_HANGUP;
	}
#   endif			/* defined M_UNHANGUP */
}

static streamscall int
timod_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err = 0;

	if (q->q_ptr != NULL)
		goto quit;	/* already open */
	err = ENXIO;
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		goto quit;
	err = ENOMEM;
	if (!(timod_alloc_priv(q)))
		goto quit;
	qprocson(q);
	return (0);
      quit:
	return (err);
}

static streamscall int
timod_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", MOD_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	timod_pop(q);
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	timod_free_priv(q);
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
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the TIMOD module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw timod_fmod = {
	.f_name = MOD_NAME,
	.f_str = &timodinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC __unlikely int
timod_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&timod_fmod)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
timod_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&timod_fmod)) < 0)
		return (err);
	return (0);
}

#if defined WITH_32BIT_CONVERSION
struct timod_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC timod_trans timod_trans_map[] = {
	{.cmd = _O_TI_GETINFO,}
	, {.cmd = _O_TI_OPTMGMT,}
	, {.cmd = _O_TI_BIND,}
	, {.cmd = _O_TI_UNBIND,}
	, {.cmd = _O_TI_GETMYNAME,}
	, {.cmd = _O_TI_GETPEERNAME,}
	, {.cmd = _O_TI_XTI_HELLO,}
	, {.cmd = _O_TI_XTI_GET_STATE,}
	, {.cmd = _O_TI_XTI_CLEAR_EVENT,}
	, {.cmd = _O_TI_XTI_MODE,}
	, {.cmd = _O_TI_TLI_MODE,}
	, {.cmd = O_TI_GETINFO,}
	, {.cmd = O_TI_OPTMGMT,}
	, {.cmd = O_TI_BIND,}
	, {.cmd = O_TI_UNBIND,}
	, {.cmd = TI_GETINFO,}
	, {.cmd = TI_OPTMGMT,}
	, {.cmd = TI_BIND,}
	, {.cmd = TI_UNBIND,}
	, {.cmd = TI_GETMYNAME,}
	, {.cmd = TI_GETPEERNAME,}
	, {.cmd = TI_SETMYNAME,}
	, {.cmd = TI_SETPEERNAME,}
	, {.cmd = TI_SYNC,}
	, {.cmd = TI_GETADDRS,}
	, {.cmd = TI_CAPABILITY,}
	, {.cmd = 0,}
};

STATIC __unlikely void
timod_ioctl32_unregister(void)
{
	struct timod_trans *t;

	for (t = timod_trans_map; t->cmd != 0; t++) {
		streams_unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}

STATIC __unlikely int
timod_ioctl32_register(void)
{
	struct timod_trans *t;

	for (t = timod_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = streams_register_ioctl32(t->cmd)) == NULL) {
			timod_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}

#endif				/* defined WITH_32BIT_CONVERSION */

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC __unlikely int
timod_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&timodinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC __unlikely int
timod_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&timodinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
timodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = timod_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
#if defined WITH_32BIT_CONVERSION
	if ((err = timod_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32bit ioctls, err = %d", MOD_NAME, err);
		timod_term_caches();
		return (err);
	}
#endif				/* defined WITH_32BIT_CONVERSION */
	if ((err = timod_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
#if defined WITH_32BIT_CONVERSION
		timod_ioctl32_unregister();
#endif				/* defined WITH_32BIT_CONVERSION */
		timod_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
timodterminate(void)
{
	int err;

	if ((err = timod_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
#if defined WITH_32BIT_CONVERSION
	timod_ioctl32_unregister();
#endif				/* defined WITH_32BIT_CONVERSION */
	if ((err = timod_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(timodinit);
module_exit(timodterminate);

#endif				/* LINUX */
