/*****************************************************************************

 @(#) $RCSfile: timod.c,v $ $Name:  $($Revision: 1.1.1.3.4.4 $) $Date: 2005/07/13 12:01:14 $

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

 Last Modified $Date: 2005/07/13 12:01:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: timod.c,v $ $Name:  $($Revision: 1.1.1.3.4.4 $) $Date: 2005/07/13 12:01:14 $"

/*
 *  timod module.
 *
 *  Version: 0.2
 *
 *  Copyright (C) 1998 Ole Husgaard (sparre@login.dknet.dk)
 *
 */

#include <sys/LiS/module.h>	/* must be VERY first include */

#include <stdarg.h>

#include <sys/stream.h>
#include <sys/tihdr.h>
#include <sys/timod.h>
#include <sys/osif.h>

#define TIMOD_TRACE 0		/* Verbose level */

/*
 *  The private state of a timod stream
 */
typedef struct {
	mblk_t *ioc_mp;			/* IOCTL command we are processing, NULL if none */
	int verbose;
} timod_priv_t;

STATIC int timod_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int timod_close(queue_t *, int, cred_t *);
STATIC int timod_rput(queue_t *, mblk_t *);
STATIC int timod_wput(queue_t *, mblk_t *);

STATIC struct module_info timod_minfo = {
	0,				/* Module ID number */
	"timod",			/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	0,				/* Hi water mark ignored, no queue service */
	0				/* Low water mark ignored, no queue service */
};

STATIC struct qinit timod_rinit = {
	timod_rput,			/* Read put */
	NULL,				/* No read queue service */
	timod_open,			/* Each open */
	timod_close,			/* Last close */
	NULL,				/* Reserved */
	&timod_minfo,			/* Information */
	NULL				/* No statistics */
};

STATIC struct qinit timod_winit = {
	timod_wput,			/* Write put */
	NULL,				/* No write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Reserved */
	&timod_minfo,			/* Information */
	NULL				/* No statistics */
};

#ifdef MODULE
STATIC
#endif
struct streamtab timod_info = {
	&timod_rinit,			/* Read queue */
	&timod_winit,			/* Write queue */
	NULL,				/* Not a multiplexer */
	NULL				/* Not a multiplexer */
};

/*
 *  Debugging
 */

#if TIMOD_TRACE

STATIC void
vtrace(char *func, queue_t *rq, int verbose, char *msg, va_list args)
{
	timod_priv_t *priv = (timod_priv_t *) rq->q_ptr;

	if (verbose <= priv->verbose) {
		char s1[128], s2[1024];

		if (!(rq->q_flag & QREADR))
			rq = RD(rq);

		sprintf(s1, "rq=%p", rq);
		vsprintf(s2, msg, args);
		printk("timod: %s(): (%s): %s\n", func, s1, s2);
	}
}

STATIC void trace(char *func, queue_t *rq, int verbose, char *msg, ...)
    __attribute__ ((format(printf, 4, 5)));

STATIC void
trace(char *func, queue_t *rq, int verbose, char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vtrace(func, rq, verbose, msg, args);
	va_end(args);
}

#define TRACE(rq, verbose, msg, args...) \
	trace(__FUNCTION__, rq, verbose, msg, ## args)

#define FUNC_ENTER(rq)	TRACE(rq, 10, "Entered")
#define FUNC_EXIT(rq)	TRACE(rq, 10, "Exiting")

#else				/* No trace logging */

#define TRACE(rq, verbose, msg, args...)

#define FUNC_ENTER(rq)
#define FUNC_EXIT(rq)

#endif

/*
 *  Local routines
 */

STATIC INLINE void
do_ioctl_getinfo(queue_t *q, mblk_t *mp, struct iocblk *iocp, timod_priv_t * priv)
{
	mblk_t *bp = unlinkb(mp);
	struct T_info_req *req;

	FUNC_ENTER(q);
	LISASSERT(bp != NULL);
	req = (struct T_info_req *) bp->b_rptr;
	if (bp == NULL || bp->b_wptr - bp->b_rptr < sizeof(struct T_info_req)
	    || req->PRIM_type != T_INFO_REQ) {
		if (bp != NULL)
			freemsg(bp);
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_count = 0;
		iocp->ioc_error = EINVAL;
		priv->ioc_mp = NULL;
		qreply(q, mp);
		FUNC_EXIT(q);
		return;
	}
	bp->b_datap->db_type = M_PCPROTO;
	putnext(q, bp);
	FUNC_EXIT(q);
}

STATIC INLINE void
do_ioctl_optmgmt(queue_t *q, mblk_t *mp, struct iocblk *iocp, timod_priv_t * priv)
{
	mblk_t *bp = unlinkb(mp);
	struct T_optmgmt_req *req;

	FUNC_ENTER(q);
	LISASSERT(bp != NULL);
	req = (struct T_optmgmt_req *) bp->b_rptr;
	if (bp == NULL || bp->b_wptr - bp->b_rptr < sizeof(struct T_optmgmt_req)
	    || req->PRIM_type != T_OPTMGMT_REQ) {
		if (bp != NULL)
			freemsg(bp);
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_count = 0;
		iocp->ioc_error = EINVAL;
		priv->ioc_mp = NULL;
		qreply(q, mp);
		FUNC_EXIT(q);
		return;
	}
	bp->b_datap->db_type = M_PROTO;
	putnext(q, bp);
	FUNC_EXIT(q);
}

STATIC INLINE void
do_ioctl_bind(queue_t *q, mblk_t *mp, struct iocblk *iocp, timod_priv_t * priv)
{
	mblk_t *bp = unlinkb(mp);
	struct T_bind_req *req;

	FUNC_ENTER(q);
	LISASSERT(bp != NULL);
	req = (struct T_bind_req *) bp->b_rptr;
	if (bp == NULL || bp->b_wptr - bp->b_rptr < sizeof(struct T_bind_req)
	    || req->PRIM_type != T_BIND_REQ) {
		if (bp != NULL)
			freemsg(bp);
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_count = 0;
		iocp->ioc_error = EINVAL;
		priv->ioc_mp = NULL;
		qreply(q, mp);
		FUNC_EXIT(q);
		return;
	}
	bp->b_datap->db_type = M_PROTO;
	putnext(q, bp);
	FUNC_EXIT(q);
}

STATIC INLINE void
do_ioctl_unbind(queue_t *q, mblk_t *mp, struct iocblk *iocp, timod_priv_t * priv)
{
	mblk_t *bp = unlinkb(mp);
	struct T_unbind_req *req;

	FUNC_ENTER(q);
	LISASSERT(bp != NULL);
	req = (struct T_unbind_req *) bp->b_rptr;
	if (bp == NULL || bp->b_wptr - bp->b_rptr < sizeof(struct T_unbind_req)
	    || req->PRIM_type != T_UNBIND_REQ) {
		if (bp != NULL)
			freemsg(bp);
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_count = 0;
		iocp->ioc_error = EINVAL;
		priv->ioc_mp = NULL;
		qreply(q, mp);
		FUNC_EXIT(q);
		return;
	}
	bp->b_datap->db_type = M_PROTO;
	putnext(q, bp);
	FUNC_EXIT(q);
}

STATIC void
do_ioctl(queue_t *q, mblk_t *mp)
{
	timod_priv_t *priv;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;

	FUNC_ENTER(q);
	/* Is this for me ? */
	if (iocp->ioc_cmd != TI_GETINFO && iocp->ioc_cmd != TI_OPTMGMT && iocp->ioc_cmd != TI_BIND
	    && iocp->ioc_cmd != TI_UNBIND) {
		/* Nope, pass through */
		TRACE(q, 1, "M_IOCTL not for me, passing down.");
		putnext(q, mp);
		return;
	}

	priv = (timod_priv_t *) q->q_ptr;
	LISASSERT(priv != NULL);

	if (priv->ioc_mp != NULL) {
		/* We are already processing an IOCTL, nak it */
		TRACE(q, 1, "Already processing ioctl, NAKing second one.");
		mp->b_datap->db_type = M_IOCNAK;
		qreply(q, mp);
		FUNC_EXIT(q);
		return;
	}
	priv->ioc_mp = mp;

	switch (iocp->ioc_cmd) {
	case TI_GETINFO:
		TRACE(q, 1, "Processing TI_GETINFO ioctl...");
		do_ioctl_getinfo(q, mp, iocp, priv);
		FUNC_EXIT(q);
		return;
	case TI_OPTMGMT:
		TRACE(q, 1, "Processing TI_OPTMGMT ioctl...");
		do_ioctl_optmgmt(q, mp, iocp, priv);
		FUNC_EXIT(q);
		return;
	case TI_BIND:
		TRACE(q, 1, "Processing TI_BIND ioctl...");
		do_ioctl_bind(q, mp, iocp, priv);
		FUNC_EXIT(q);
		return;
	case TI_UNBIND:
		TRACE(q, 1, "Processing TI_UNBIND ioctl...");
		do_ioctl_unbind(q, mp, iocp, priv);
		FUNC_EXIT(q);
		return;
	default:
		LISASSERT(0);
		FUNC_EXIT(q);
		return;
	}
 /*NOTREACHED*/}

/*
 *  Read put function.
 *
 *  This is called from the read put function when we are processing
 *  an ioctl and we got an upstream M_PROTO or M_PCPROTO message.
 *
 *  This message could be a reply to the message that we sent downstream
 *  earlier when we started processing of the ioctl.
 *
 */
STATIC void
do_ioctl_rput(queue_t *q, mblk_t *mp)
{
	timod_priv_t *priv;
	struct iocblk *iocp;
	struct T_error_ack *error_ack;

	FUNC_ENTER(q);
	LISASSERT(mp->b_datap->db_type == M_PROTO || mp->b_datap->db_type == M_PCPROTO);

	if (mp->b_wptr - mp->b_rptr < sizeof(t_scalar_t)) {
		/* Short message */
		TRACE(q, 1, "Short message");
		putnext(q, mp);
		FUNC_EXIT(q);
		return;
	}

	priv = (timod_priv_t *) q->q_ptr;

	/* 
	 *  Recheck: This could change at interrupt time.
	 */
	if (priv->ioc_mp == NULL) {
	      not_for_us:
		putnext(q, mp);
		FUNC_EXIT(q);
		return;
	}

	iocp = (struct iocblk *) priv->ioc_mp->b_rptr;

	switch (*(t_scalar_t *) mp->b_rptr) {
	case T_INFO_ACK:
		if (iocp->ioc_cmd != TI_GETINFO)
			goto not_for_us;
		TRACE(q, 1, "Finishing TI_GETINFO ioctl...");
		goto send_iocack;

	case T_OPTMGMT_ACK:
		if (iocp->ioc_cmd != TI_OPTMGMT)
			goto not_for_us;
		TRACE(q, 1, "Finishing TI_OPTMGMT ioctl...");
		goto send_iocack;

	case T_BIND_ACK:
		if (iocp->ioc_cmd != TI_BIND)
			goto not_for_us;
		TRACE(q, 1, "Finishing TI_BIND ioctl...");
		goto send_iocack;

	case T_ERROR_ACK:
		error_ack = (struct T_error_ack *) mp->b_rptr;
		TRACE(q, 1, "Got T_ERROR_ACK: ERROR_prim=%ld, " "TLI_error=%ld, UNIX_error=%ld",
		      error_ack->ERROR_prim, error_ack->TLI_error, error_ack->UNIX_error);
		switch (iocp->ioc_cmd) {
		case TI_GETINFO:
			if (error_ack->ERROR_prim != T_INFO_REQ)
				goto not_for_us;
			TRACE(q, 1, "Got TI_GETINFO error ack...");
			goto send_error_ack;
		case TI_OPTMGMT:
			if (error_ack->ERROR_prim != T_OPTMGMT_REQ)
				goto not_for_us;
			TRACE(q, 1, "Got TI_OPTMGMT error ack...");
			goto send_error_ack;
		case TI_BIND:
			if (error_ack->ERROR_prim != T_BIND_REQ)
				goto not_for_us;
			TRACE(q, 1, "Got TI_BIND error ack...");
		      send_error_ack:priv->ioc_mp->b_datap->db_type =
			    M_IOCACK;
			iocp->ioc_count = 0;
			iocp->ioc_error = 0;
			iocp->ioc_rval = (error_ack->UNIX_error << 8)
			    + error_ack->TLI_error;
			if (iocp->ioc_rval == 0) {
				TRACE(q, 1, "No error code, changing " "to TSYSERR:EINVAL");
				iocp->ioc_rval = (EINVAL << 8) + TSYSERR;
			}
			freemsg(mp);
			mp = priv->ioc_mp;
			priv->ioc_mp = NULL;
			putnext(q, mp);
			TRACE(q, 1, "Sent error M_IOCACK.");
			FUNC_EXIT(q);
			return;
		case TI_UNBIND:
			error_ack = (struct T_error_ack *) mp->b_rptr;
			if (error_ack->ERROR_prim != T_UNBIND_REQ)
				goto not_for_us;
			TRACE(q, 1, "Got TI_UNBIND error ack...");
			goto send_error_ack;
		default:
			goto not_for_us;
		}
		if (iocp->ioc_cmd != TI_UNBIND)
			goto not_for_us;
		if (((struct T_ok_ack *) mp->b_rptr)->CORRECT_prim != T_UNBIND_REQ)
			goto not_for_us;

	case T_OK_ACK:
		if (iocp->ioc_cmd != TI_UNBIND)
			goto not_for_us;
		if (((struct T_ok_ack *) mp->b_rptr)->CORRECT_prim != T_UNBIND_REQ)
			goto not_for_us;
		TRACE(q, 1, "Finishing TI_UNBIND ioctl...");
	      send_iocack:mp->b_datap->db_type = M_DATA;
		freemsg(unlinkb(priv->ioc_mp));
		priv->ioc_mp->b_cont = mp;
		priv->ioc_mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_count = msgdsize(mp);
		mp = priv->ioc_mp;
		priv->ioc_mp = NULL;
		putnext(q, mp);
		TRACE(q, 1, "Sent M_IOCACK.");
		FUNC_EXIT(q);
		return;

	default:
		goto not_for_us;
	}
 /*NOTREACHED*/}

STATIC int
timod_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	timod_priv_t *priv;

	if (sflag != MODOPEN)
		return ENXIO;
	if (q->q_ptr != NULL)
		return ENXIO;
	if ((priv = ALLOC(sizeof(timod_priv_t))) == NULL)
		return ENOSR;
	priv->ioc_mp = NULL;
	priv->verbose = TIMOD_TRACE;
	q->q_ptr = WR(q)->q_ptr = priv;
	MODGET();
	return 0;
}

STATIC int
timod_close(queue_t *q, int flag, cred_t *crp)
{
	timod_priv_t *priv = (timod_priv_t *) q->q_ptr;

	LISASSERT(priv != NULL);
	LISASSERT(q->q_ptr == WR(q)->q_ptr);

	FREE(priv);
	q->q_ptr = WR(q)->q_ptr = NULL;
	MODPUT();
	return 0;
}

STATIC int
timod_rput(queue_t *q, mblk_t *mp)
{
	FUNC_ENTER(q);
	if (((timod_priv_t *) q->q_ptr)->ioc_mp != NULL
	    && (mp->b_datap->db_type == M_PROTO || mp->b_datap->db_type == M_PCPROTO)) {
		/* 
		 *  We are processing an IOCTL
		 *  and got a M_PROTO or M_PCPROTO message.
		 */
		TRACE(q, 5, "Possibly a reply, calling do_ioctl_rput()");
		do_ioctl_rput(q, mp);
		TRACE(q, 5, "Returned from do_ioctl_rput()");
	} else {
		TRACE(q, 9, "Not for me, doing putnext()");
		putnext(q, mp);
		TRACE(q, 9, "Returned from putnext() of msg not for me.");
	}
	FUNC_EXIT(q);
	return (0);
}

STATIC int
timod_wput(queue_t *q, mblk_t *mp)
{
	FUNC_ENTER(q);
	if (mp->b_datap->db_type != M_IOCTL) {
		TRACE(q, 9, "Not an ioctl, doing putnext()");
		putnext(q, mp);
		TRACE(q, 9, "Returned from putnext() of msg not an ioctl.");
	} else {
		TRACE(q, 5, "Possibly an ioctl for me, calling do_ioctl()");
		do_ioctl(q, mp);
		TRACE(q, 5, "Returned from do_ioctl()");
	}
	FUNC_EXIT(q);
	return (0);
}

#ifdef MODULE

#ifdef KERNEL_2_5
int
timod_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strmod(&timod_info, "timod");

	if (ret < 0) {
		printk("timod.init_module: Unable to register module.\n");
		return ret;
	}
	return 0;
}

#ifdef KERNEL_2_5
void
timod_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	if (lis_unregister_strmod(&timod_info) < 0)
		printk("timod.cleanup_module: Unable to unregister module.\n");
	else
		printk("timod.cleanup_module: Unregistered, ready to be unloaded.\n");
	return;
}

#ifdef KERNEL_2_5
module_init(timod_init_module);
module_exit(timod_cleanup_module);
#endif

#if defined(LINUX)		/* linux kernel */
#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("Ole Husgaard (sparre@login.dknet.dk");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS timod module, converts ioctls to TLI");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif
#endif				/* LINUX */

#endif				/* MODULE */
