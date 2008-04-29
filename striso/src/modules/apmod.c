/*****************************************************************************

 @(#) $RCSfile: apmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 00:02:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: apmod.c,v $
 Revision 0.9.2.2  2008-04-29 00:02:05  brian
 - updated headers for release

 Revision 0.9.2.1  2008-04-25 08:33:40  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: apmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:05 $"

static char const ident[] = "$RCSfile: apmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:05 $";

/*
 * ASCE/Presentation (A/P) Module
 *
 * The apli(4) module provides a cooperating module for the ACSE/Presentation
 * Library (APLI).  The lower boundary service interface is the Transport
 * Provider Interface (TPI) and the upper boundary service interface is the
 * ACSE/Presenation Library interface (A/P).  This module contains Session,
 * Presentation, ACSE and optionally ROSE protocol layers.
 */

#include <sys/os7/compat.h>

/*
 * These are for TPI definitions.
 */
#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#define APLI_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define APLI_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define APLI_REVISION	"OpenSS7 $RCSfile: apmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:05 $"
#define APLI_DEVICE	"SVR 4.2 STREAMS APLI Module for APLI"
#define APLI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define APLI_LICENSE	"GPL"
#define APLI_BANNER	APLI_DESCRIP	"\n" \
			APLI_COPYRIGHT	"\n" \
			APLI_REVISION	"\n" \
			APLI_DEVICE	"\n" \
			APLI_CONTACT
#define APLI_SPLASH	APLI_DEVICE	" - " \
			APLI_REVISION

#ifdef LINUX
MODULE_AUTHOR(APLI_CONTACT);
MODULE_DESCRIPTION(APLI_DESCRIP);
MODULE_SUPPORTED_DEVICE(APLI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(APLI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-apli");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_APLI_NAME
//#define CONFIG_STREAMS_APLI_NAME "apli"
#error "CONFIG_STREAMS_APLI_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_APLI_MODID
//#define CONFIG_STREAMS_APLI_MODID "5080"
#error "CONFIG_STREAMS_APLI_MODID must be defined."
#endif

#ifndef APLI_MOD_NAME
#define APLI_MOD_NAME		CONFIG_STREAMS_APLI_NAME
#endif				/* APLI_MOD_NAME */
#ifndef APLI_MOD_ID
#define APLI_MOD_ID		CONFIG_STREAMS_APLI_MODID
#endif				/* APLI_MOD_ID */

/*
 * STREAMS Definitions
 */

#define MOD_ID		APLI_MOD_ID
#define MOD_NAME	APLI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	APLI_BANNER
#else				/* MODULE */
#define MOD_BANNER	APLI_SPLASH
#endif				/* MODULE */

static struct module_info ap_info = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_state ap_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_state ap_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * Private Datastructure ctors and dtors
 */
struct apli {
	queue_t *rq;			/* module read queue */
	queue_t *wq;			/* module write queue */
	queue_t *hq;			/* stream head read queue */
	ulong state;			/* module state */
	ulong oldstate;			/* module old state */
	ulong flags;			/* module flags */
	mblk_t *iocblk;			/* ioctl being processed */
	ulong qlen;			/* bind ack queue length */
	ulong cons;			/* outstanding connectoin indications */
};

/*
 * PUTP and SRVP, put and service procedures
 */

/*
 * Read-Side Processing
 */

static streams_fastcall streams_inline __hot_get int
ap_r_proto(queue_t *q, mblk_t *mp)
{
	switch (*(t_scalar_t) mp->b_rptr) {
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
	case T_CAPABILITY_ACK:
/*
 * None of the following messages should appear at the read-side put
 * procedure.  Only upward defined TPI messages should appear at the read-side
 * put procedure.  Discard these.
 */
	case T_CONN_REQ:
	case T_CONN_RES:
	case T_DISCON_REQ:
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_INFO_REQ:
	case T_BIND_REQ:
	case T_UNBIND_REQ:
	case T_UNITDATA_REQ:
	case T_OPTMGMT_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
	case T_ADDR_REQ:
	case T_CAPABILITY_REQ:
		__swerr();
	default:
		freemsg(mp);
		return (0);
	}
}

static streamscall int
ap_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

static streamscall int
ap_r_data(queue_t *q, mblk_t *mp)
{
	__swerr();
	return (EFAULT);
}

static streams_fastcall streams_noinline int
ap_r_msg(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return ap_r_proto(q, mp);
	case M_DATA:
		return ap_r_data(q, mp);
	case M_FLUSH:
		return ap_r_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}

static streamscall __hot_get int
ap_rput(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_PROTO)
		return ap_r_proto(q, mp);
	return ap_r_msg(q, mp);
}

/*
 * Write-Side Processing
 */

static streams_fastcall streams_inline __hot_put int
ap_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(t_scalar_t *) mp->b_rptr) {
	case T_CONN_REQ:
	case T_CONN_RES:
	case T_DISCON_REQ:
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_INFO_REQ:
	case T_BIND_REQ:
	case T_UNBIND_REQ:
	case T_UNITDATA_REQ:
	case T_OPTMGMT_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
	case T_ADDR_REQ:
	case T_CAPABILITY_REQ:
/*
 * None of the following messages should appear at the write-side put
 * procedure.  Only downward deinfed TPI messages should appear at the
 * write-side put procedure.  Discard these.
 */
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
	case T_CAPABILITY_ACK:
		__swerr();
	default:
		freemsg(mp);
		return (0);
	}
}

static streamscall int
ap_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

static streamscall int
ap_w_data(queue_t *q, mblk_t *mp)
{
	/* FIXME: this should be ok. */
	__swerr();
	return (EFAULT);
}

static streams_fastcall streams_noinline int
ap_w_msg(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return ap_w_proto(q, mp);
	case M_DATA:
		return ap_w_data(q, mp);
	case M_FLUSH:
		return ap_w_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}

/*
 * STREAMS Message Handling
 */

/*
 * Write queue processing: (APLI primitive from above.)
 */

static fastcall int
ap_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall int
ap_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct ap *ap;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	ap = AP_PRIV(q);
	ap_save_state(ap);
	tp_save_state(ap->tp);
	switch (prim) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- %s", ap_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", ap_primname(prim));
		break;
	}
	switch (prim) {
	case T_CONN_REQ:
		rtn = ap_conn_req(ap, q, mp);
		break;
	case T_CONN_RES:
		rtn = ap_conn_res(ap, q, mp);
		break;
	case T_DISCON_REQ:
		rtn = ap_discon_req(ap, q, mp);
		break;
	case T_DATA_REQ:
		rtn = ap_data_req(ap, q, mp);
		break;
	case T_EXDATA_REQ:
		rtn = ap_exdata_req(ap, q, mp);
		break;
	case T_INFO_REQ:
		rtn = ap_info_req(ap, q, mp);
		break;
	case T_BIND_REQ:
		rtn = ap_bind_req(ap, q, mp);
		break;
	case T_UNBIND_REQ:
		rtn = ap_unbind_req(ap, q, mp);
		break;
	case T_UNITDATA_REQ:
		rtn = ap_unitdata_req(ap, q, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = ap_optmgmt_req(ap, q, mp);
		break;
	case T_ORDREL_REQ:
		rtn = ap_ordrel_req(ap, q, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = ap_optdata_req(ap, q, mp);
		break;
	case T_ADDR_REQ:
		rtn = ap_addr_req(ap, q, mp);
		break;
	case T_CAPABILITY_REQ:
		rtn = ap_capability_req(ap, q, mp);
		break;
/*
 * None of the following messages should appear at the write-side put
 * procedure.  Only downward deinfed TPI messages should appear at the
 * write-side put procedure.  Discard these.
 */
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
	case T_CAPABILITY_ACK:
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding message in wrong direction",
			  __FUNCTION__);
		freemsg(mp);
		rtn = 0;
		break;
	default:
		rtn = ap_other_req(ap, q, mp);
		break;
	}
	if (rtn) {
		tp_restore_state(ap->tp);
		ap_restore_state(ap);
	}
	mi_unlock(priv);
	return (rtn);
}


static fastcall int
ap_m_sig(queue_t *q, mblk_t *mp)
{
	struct ap *ap;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(int))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	ap = AP_PRIV(q);
	ap_save_state(ap);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = ap_t1_timeout(ap, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d",
			  __FUNCTION__ * (int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		ap_restore_state(ap);
		rtn = mi_timer_requeue(mp) ? rnt : 0;
	}
	mi_unlock(priv);
	return (rtn);
      passalong:
	/* These are either timer messages (in which case they have a length
	   greater than or equal to 4) or true signal messages (in which case
	   they have a length of 1). Therefore, if the length of the message is 
	   less than sizeof(int) we pass them along. */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
ap_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct ap *ap;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != APLI_IOC_MAGIC) {
		/* Let driver deal with these. */
		putnext(q, mp);
		return (0);
	}
	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	ap = AP_PRIV(q);
	err = ap_ioctl(ap, q, mp);
	mi_unlock(priv);
	return (err);
}

static fastcall int
ap_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct ap *ap;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != APLI_IOC_MAGIC) {
		/* Let driver deal with these. */
		putnext(q, mp);
		return (0);
	}
	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	ap = AP_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = ap_copyin1(ap, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = ap_copyin2(ap, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = ap_copyout(tr, q, mp);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		err = 0;
		break;
	}
	mi_unlock(priv);
	return (err);
}

static fastcall int
ap_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

static fastcall int
ap_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_cont)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
ap_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return ap_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ap_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return ap_m_sig(q, mp);
	case M_IOCTL:
		return ap_m_ioctl(q, mp);
	case M_IOCDATA:
		return ap_m_iocdata(q, mp);
	case M_FLUSH:
		return ap_m_flush(q, mp);
	default:
		return ap_m_other(q, mp);

	}
}

/*
 * Read queue processing: (TPI primitive from below.)
 */

static fastcall int
tp_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall int
tp_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tp = TP_PRIV(q);
	tp_save_state(tp);
	ap_save_state(tp->ap);
	switch (prim) {
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_OPTDATA_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s <-", tp_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", tp_primname(prim));
		break;
	}
	switch (prim) {
	case T_CONN_IND:
		rtn = t_conn_ind(tp, q, mp);
		break;
	case T_CONN_CON:
		rtn = t_conn_con(tp, q, mp);
		break;
	case T_DISCON_IND:
		rtn = t_discon_ind(tp, q, mp);
		break;
	case T_DATA_IND:
		rtn = t_data_ind(tp, q, mp);
		break;
	case T_EXDATA_IND:
		rtn = t_exdata_ind(tp, q, mp);
		break;
	case T_INFO_ACK:
		rtn = t_info_ack(tp, q, mp);
		break;
	case T_BIND_ACK:
		rtn = t_bind_ack(tp, q, mp);
		break;
	case T_ERROR_ACK:
		rtn = t_error_ack(tp, q, mp);
		break;
	case T_OK_ACK:
		rtn = t_ok_ack(tp, q, mp);
		break;
	case T_UNITDATA_IND:
		rtn = t_unitdata_ind(tp, q, mp);
		break;
	case T_UDERROR_IND:
		rtn = t_uderror_ind(tp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		rtn = t_optmgmt_ack(tp, q, mp);
		break;
	case T_ORDREL_IND:
		rtn = t_ordrel_ind(tp, q, mp);
		break;
	case T_OPTDATA_IND:
		rtn = t_optdata_ind(tp, q, mp);
		break;
	case T_ADDR_ACK:
		rtn = t_addr_ack(tp, q, mp);
		break;
	case T_CAPABILITY_ACK:
		rtn = t_capability_ack(tp, q, mp);
		break;
/*
 * None of the following messages should appear at the read-side put
 * procedure.  Only upward defined TPI messages should appear at the read-side
 * put procedure.  Discard these.
 */
	case T_CONN_REQ:
	case T_CONN_RES:
	case T_DISCON_REQ:
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_INFO_REQ:
	case T_BIND_REQ:
	case T_UNBIND_REQ:
	case T_UNITDATA_REQ:
	case T_OPTMGMT_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
	case T_ADDR_REQ:
	case T_CAPABILITY_REQ:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding primitive in wrong direction",
			  __FUNCTION__);
		freemsg(mp);
		rtn = 0;
		break;
	default:
		rtn = tp_other_ind(tp, q, mp);
		break;
	}
	if (rtn) {
		ap_restore_state(tp->ap);
		tp_restore_state(tp);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * tp_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 *
 * These are either timer messages (in which case they have a length greater
 * than or equal to 4) or true signal messages (in which case they have a
 * length of 1).  Therefore, if the length of the message is less than
 * sizeof(int) we pass them along.
 */
static fastcall int
tp_m_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + 4)
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	tp = TP_PRIV(q);
	tp_save_state(tp);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tp_t1_tieout(tp, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tp_restore_state(tp);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
      passalong:
	/* These are either timer messages (in which case they have a length
	   greater than or equal to 4) or true signal messages (in which case
	   they have a length of 1). Therefore, if the length of the message is 
	   less than sizeof(int) we pass them along. */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
tp_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
tp_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
tp_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

static fastcall int
tp_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanpuntext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastacll int
tp_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tp_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return tp_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tp_m_iocack(q, mp);
	case M_FLUSH:
		return tp_m_flush(q, mp);
	default:
		return tp_m_other(q, mp);
	}
}

/*
 * Queue put and service procedures
 */
static streamscall __hot_write int
ap_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ap_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
ap_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (ap_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_read int
tp_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tp_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
ap_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tp_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * Queue open and close routines
 */
static caddr_t ap_opens = NULL;

static streamscall int
ap_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct T_info_req), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&ap_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);

	/* initialize private structure */
	p->r_priv.priv = p;
	p->r_priv.ap = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state = TS_UNBND;
	p->r_priv.oldstate = TS_UNBND;

	p->w_priv.priv = p;
	p->w_priv.tp = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state = AP_IDLE;
	p->w_priv.oldstate = AP_IDLE;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((struct T_info_req *) mp->b_wptr)->PRIM_type = T_INFO_REQ;
	mp->b_wptr += sizeof(struct T_info_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall int
ap_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&ap_opens, q);
	return (0);
}

/*
 * STREAMS initialization
 */

static struct qinit ap_rinit = {
	.qi_putp = ap_rput,		/* Read put (message from below) */
	.qi_qopen = &ap_open,		/* Each open */
	.qi_qclose = &ap_close,		/* Last close */
	.qi_minfo = &ap_minfo,		/* Module information */
	.qi_mstat = &ap_rstat,		/* Module statistics */
};

static struct qinit ap_winit = {
	.qi_putp = ap_wput,		/* Write put (message form above) */
	.qi_minfo = &ap_minfo,		/* Module information */
	.qi_mstat = &ap_wstat,		/* Module statistics */
};

struct streamtab ap_info = {
	.st_rdinit = &ap_rinit,
	.st_wrinit = &ap_winit,
};

static modID_t modid = MOD_ID;

/*
 * Kernel module initialization
 */

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for APLI. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw ap_fmod = {
	.f_name = MOD_NAME,
	.f_str = &ap_modinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
ap_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&ap_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
ap_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&ap_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(ap_modinit);
module_exit(ap_modexit);
