/*****************************************************************************

 @(#) $RCSfile: momod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $

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

 Last Modified $Date: 2008-04-25 08:33:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: momod.c,v $
 Revision 0.9.2.1  2008-04-25 08:33:40  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: momod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $"

static char const ident[] = "$RCSfile: momod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $";

/*
 * Minimal OSI (mOSI) Module
 *
 * The mosi(4) module provides a cooperating module for the XTI-mOSI interface
 * for the XTI Library (XTI).  The lower boundary service interface is the
 * Transport Provider Interface (TPI) and the upper boundary service interface
 * is the XTI-mOSI interface.  This module contains Session, Presentation and
 * ACSE protocol layers.
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

#define MOSI_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MOSI_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define MOSI_REVISION	"OpenSS7 $RCSfile: momod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $"
#define MOSI_DEVICE	"SVR 4.2 STREAMS MOSI Module for RFC 1698 (mOSI)"
#define MOSI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MOSI_LICENSE	"GPL"
#define MOSI_BANNER	MOSI_DESCRIP	"\n" \
			MOSI_COPYRIGHT	"\n" \
			MOSI_REVISION	"\n" \
			MOSI_DEVICE	"\n" \
			MOSI_CONTACT
#define MOSI_SPLASH	MOSI_DEVICE	" - " \
			MOSI_REVISION

#ifdef LINUX
MODULE_AUTHOR(MOSI_CONTACT);
MODULE_DESCRIPTION(MOSI_DESCRIP);
MODULE_SUPPORTED_DEVICE(MOSI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MOSI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mosi");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef MOSI_MOD_NAME
#define MOSI_MOD_NAME		"mosi"
#endif				/* MOSI_MOD_NAME */
#ifndef MOSI_MOD_ID
#define MOSI_MOD_ID		0
#endif				/* MOSI_MOD_ID */

/*
 * STREAMS Definitions
 */

#define MOD_ID		MOSI_MOD_ID
#define MOD_NAME	MOSI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MOSI_BANNER
#else				/* MODULE */
#define MOD_BANNER	MOSI_SPLASH
#endif				/* MODULE */

static struct module_info mosi_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mosi_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
static struct module_stat mosi_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static streamscall int mosi_open(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int mosi_close(queue_t *, int, cred_t *);

static streamscall int mosi_rput(queue_t *, mblk_t *);
static streamscall int mosi_wput(queue_t *, mblk_t *);

static struct qinit mosi_rinit = {
	.qi_putp = mosi_rput,		/* Read put (message from below) */
	.qi_qopen = &mosi_open,		/* Each open */
	.qi_qclose = &mosi_close,	/* Last close */
	.qi_minfo = &mosi_minfo,	/* Module information */
	.qi_mstat = &mosi_rstat,	/* Module statistics */
};

static struct qinit mosi_winit = {
	.qi_putp = mosi_wput,		/* Write put (message form above) */
	.qi_minfo = &mosi_minfo,	/* Module information */
	.qi_mstat = &mosi_wstat,	/* Module statistics */
};

struct streamtab mosi_info = {
	.st_rdinit = &mosi_rinit,
	.st_wrinit = &mosi_winit,
};

/*
 * PUTP and SRVP, put and service procedures
 */

/*
 * Read-Side Processing
 */

/**
 * mosi_r_proto - process M_(PC)PROTO message block on read side
 * @q: read queue
 * @mp: the message
 *
 * Note that much of this processing is stateless and pretty automatic.  When
 * a request is generated on the write side, we transform to a transport TPI
 * message, pass it down and wait for the response.  Here the response is most
 * often simply translated into the corresponing mOSI response.
 */
static streams_fastcall streams_inline __hot_get int
mosi_r_proto(queue_t *q, mblk_t *mp)
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
mosi_r_flush(queue_t *q, mblk_t *mp)
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
mosi_r_data(queue_t *q, mblk_t *mp)
{
	__swerr();
	return (EFAULT);
}

static streams_fastcall streams_noinline int
mosi_r_msg(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return mosi_r_proto(q, mp);
	case M_DATA:
		return mosi_r_data(q, mp);
	case M_FLUSH:
		return mosi_r_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}

static streamscall __hot_get int
mosi_rput(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_PROTO)
		return mosi_r_proto(q, mp);
	return mosi_r_msg(q, mp);
}

/*
 * Write-Side Processing
 */

static streams_fastcall streams_inline __hot_put int
mosi_w_proto(queue_t *q, mblk_t *mp)
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
mosi_w_flush(queue_t *q, mblk_t *mp)
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
mosi_w_data(queue_t *q, mblk_t *mp)
{
	/* FIXME: this should be ok. */
	__swerr();
	return (EFAULT);
}

static streams_fastcall streams_noinline int
mosi_w_msg(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return mosi_w_proto(q, mp);
	case M_DATA:
		return mosi_w_data(q, mp);
	case M_FLUSH:
		return mosi_w_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}

/**
 * mosi_wput - write-side put procedure
 * @q: write queue
 * @mp: message to put
 *
 * Messages arriving at the write-side put procedure are messages from above
 * that are TPI formatted messages.  For all intents and purposes, the module
 * above us should believe that we are any mOSI based XTI provider.
 */
statis streamscall __hot_put int
mosi_wput(queue_t *q, mblk_t *mp)
{
	int type = mp->b_datap->db_type;

	/* Fast path. */
	if (type == M_PROTO)
		return mosi_w_proto(q, mp);
	return mosi_w_msg(q, mp);
}

static streamscall int
mosi_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	return (ENXIO);
}

static streamscall int
mosi_close(queue_t *q, int oflags, cred_t *crp)
{
	return (ENXIO);
}

/*
 * --------------------------------------------------------------------------
 *
 *  QOPEN and QCLOSE routines
 *
 * --------------------------------------------------------------------------
 */

static caddr_t mosi_opens = NULL;

static streamscall int
mosi_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct T_info_req), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&mosi_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);

	/* intiialize private structure */
	p->r_priv.priv = p;
	p->r_priv.mo = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state = TS_UNBND;
	p->r_priv.oldstate = TS_UNBND;

	p->w_priv.priv = p;
	p->w_priv.tp = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state = TS_UNBND;
	p->w_priv.oldstate = TS_UNBND;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((struct T_info_req *) mp->b_wptr)->PRIM_type = T_INFO_REQ;
	mp->b_wptr += sizeof(struct T_info_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall int
mosi_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);
	(void)p;
	qprocsoff(q);
	/* FIXME: free timers and other things */
	mi_close_comm(&mosi_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 *  STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit mosi_rinit = {
	.qi_putp = tp_rput,		/* Read put (message from below) */
	.qi_srvp = tp_rsrv,		/* Read service */
	.qi_qopen = &mosi_open,		/* Each open */
	.qi_qclose = &mosi_close,	/* Last close */
	.qi_minfo = &mosi_minfo,	/* Module information */
	.qi_mstat = &mosi_rstat,	/* Module statistics */
};

static struct qinit mosi_winit = {
	.qi_putp = mo_wput,		/* Write put (message form above) */
	.qi_srvp = mo_wsrv,		/* Write service */
	.qi_minfo = &mosi_minfo,	/* Module information */
	.qi_mstat = &mosi_wstat,	/* Module statistics */
};

struct streamtab mosi_info = {
	.st_rdinit = &mosi_rinit,   /* Upper RD queue */
	.st_wrinit = &mosi_winit,   /* Upper WR queue */
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 *  STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for mOSI. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw mosi_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mosi_modinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/*
 * --------------------------------------------------------------------------
 *
 *  LINUX KERNEL MODULE INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static __init int
mosi_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&mosi_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
mosi_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&mosi_fmod)) < 0) {
		cmd_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(mosi_modinit);
module_exit(mosi_modexit);
