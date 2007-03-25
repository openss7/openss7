/*****************************************************************************

 @(#) $RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.21 $) $Date: 2007/03/25 18:59:49 $

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

 Last Modified $Date: 2007/03/25 18:59:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp_npi.c,v $
 Revision 0.9.2.21  2007/03/25 18:59:49  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.20  2007/03/25 02:22:52  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.19  2007/02/26 07:25:40  brian
 - synchronizing changes

 Revision 0.9.2.18  2007/02/17 02:49:17  brian
 - first clean recompile of MTP modules on LFS

 Revision 0.9.2.17  2007/02/14 14:09:11  brian
 - broad changes updating support for SS7 MTP and M3UA

 Revision 0.9.2.16  2007/02/13 07:55:43  brian
 - working up MTP and UAs

 Revision 0.9.2.15  2006/05/08 11:01:00  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.14  2006/03/07 01:10:46  brian
 - binary compatible callouts

 Revision 0.9.2.13  2006/03/04 13:00:15  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.21 $) $Date: 2007/03/25 18:59:49 $"

static char const ident[] =
    "$RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.21 $) $Date: 2007/03/25 18:59:49 $";

/*
 *  This is a MTP NPI module which can be pushed over an MTPI (Message Transfer Part Interface)
 *  stream to effect an NPI interface to the MTP.  This module is intended to be used by application
 *  programs or by upper modules that expect an NPI connectionless service provider.
 */
#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#undef DB_TYPE
#define DB_TYPE(mp) mp->b_datap->db_type

#include <linux/socket.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_mtp.h>
#include <sys/xti.h>
#include <sys/xti_ss7.h>
#include <sys/xti_mtp.h>

#define MTP_NPI_DESCRIP		"SS7 Message Transfer Part (MTP) NPI STREAMS MODULE."
#define MTP_NPI_REVISION	"LfS $RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.21 $) $Date: 2007/03/25 18:59:49 $"
#define MTP_NPI_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_NPI_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MTP_NPI_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MTP_NPI_LICENSE		"GPL"
#define MTP_NPI_BANNER		MTP_NPI_DESCRIP		"\n" \
				MTP_NPI_REVISION	"\n" \
				MTP_NPI_COPYRIGHT	"\n" \
				MTP_NPI_DEVICE		"\n" \
				MTP_NPI_CONTACT		"\n"
#define MTP_NPI_SPLASH		MTP_NPI_DESCRIP		"\n" \
				MTP_NPI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(MTP_NPI_CONTACT);
MODULE_DESCRIPTION(MTP_NPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP_NPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP_NPI_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mtp_npi");
#endif
#endif				/* LINUX */

#ifdef LFS
#define MTP_NPI_MOD_ID		CONFIG_STREAMS_MTP_NPI_MODID
#define MTP_NPI_MOD_NAME	CONFIG_STREAMS_MTP_NPI_NAME
#endif

#define MOD_ID		MTP_NPI_MOD_ID
#define MOD_NAME	MTP_NPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MTP_NPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	MTP_NPI_SPLASH
#endif				/* MODULE */

/*
 *  =========================================================================
 *
 *  MTP-NPI Private Structure
 *
 *  =========================================================================
 */

typedef struct mtp {
	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	cred_t cred;
	struct mtp_addr src;		/* srce address */
	struct mtp_addr dst;		/* dest address */
	size_t src_len;
	size_t dst_len;
	uint coninds;
	struct {
		mtp_ulong pvar;
		mtp_ulong popt;
		mtp_ulong sls;		/* default options */
		mtp_ulong mp;		/* default options */
		mtp_ulong debug;	/* default options */
		mtp_ulong sls_mask;
	} options;
	N_info_ack_t prot;
} mtp_t;

#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

static caddr_t mtp_opens = NULL;

#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

/*
 *  =========================================================================
 *
 *  ADDRESS Handling
 *
 *  =========================================================================
 */
static inline fastcall size_t
n_addr_size(struct mtp_addr *add)
{
	if (add)
		return sizeof(*add);
	return (0);
}
static inline fastcall void
n_build_addr(struct mtp_addr *add, unsigned char *p)
{
	if (add)
		bcopy(add, p, sizeof(*add));
}

/*
 *  =========================================================================
 *
 *  QOS Parameters
 *
 *  =========================================================================
 */
typedef struct mtp_opts {
	uint flags;			/* success flags */
	mtp_ulong *sls;
	mtp_ulong *mp;
	mtp_ulong *debug;
} mtp_opts_t;

struct {
	mtp_ulong sls;
	mtp_ulong mp;
	mtp_ulong debug;
} opt_defaults = {
0, 0, 0};

static int
n_parse_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

static inline fastcall size_t
n_opts_size(union N_qos_mtp *qos)
{
	if (qos) {
		switch(qos->n_qos_type) {
		case N_QOS_SEL_DATA_MTP:
			return (sizeof(qos->n_qos_data));
		case N_QOS_SEL_CONN_MTP:
			return (sizeof(qos->n_qos_conn));
		case N_QOS_SEL_INFO_MTP:
			return (sizeof(qos->n_qos_info));
		case N_QOS_RANGE_INFO_MTP:
			return (sizeof(qos->n_qos_range));
		default:
			return (0);
		}
	}
	return (0);
}
static inline fastcall void
n_build_opts(union N_qos_mtp *qos, unsigned char *p)
{
	if (qos) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_DATA_MTP:
			return bcopy(qos, p, sizeof(qos->n_qos_data));
		case N_QOS_SEL_CONN_MTP:
			return bcopy(qos, p, sizeof(qos->n_qos_conn));
		case N_QOS_SEL_INFO_MTP:
			return bcopy(qos, p, sizeof(qos->n_qos_info));
		case N_QOS_RANGE_INFO_MTP:
			return bcopy(qos, p, sizeof(qos->n_qos_range));
		default:
			return;
		}
	}
}

/*
 *  =========================================================================
 *
 *  STATE Functions
 *
 *  =========================================================================
 */
static const char *
mtp_state(mtp_ulong state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("????");
	}
}

static mtp_ulong
mtp_set_state(struct mtp *mtp, mtp_ulong newstate)
{
	mtp_ulong oldstate = mtp->prot.CURRENT_state;

	if (newstate != oldstate) {
		mi_strlog(mtp->rq, STRLOGST, SL_TRACE, "%s <- %s", mtp_state(newstate),
			  mtp_state(oldstate));
		mtp->prot.CURRENT_state = newstate;
	}
	return (oldstate);
}

static mtp_ulong
mtp_get_state(struct mtp *mtp)
{
	return mtp->prot.CURRENT_state;
}

static mtp_ulong
mtp_get_statef(struct mtp *mtp)
{
	return (1 << mtp_get_state(mtp));
}

static mtp_ulong
mtp_chk_state(struct mtp *mtp, mtp_ulong mask)
{
	return (mtp_get_statef(mtp) & mask);
}

static mtp_ulong
mtp_not_state(struct mtp *mtp, mtp_ulong mask)
{
	return (mtp_chk_state(mtp, ~mask));
}

#ifndef NS_NOSTATES
#define NS_NOSTATES (NS_WACK_DREQ11 + 1)
#endif
/*
 *  TLI interface state flags
 */
#if 0
#define NSF_UNBND	( 1 << NS_UNBND		)
#define NSF_WACK_BREQ	( 1 << NS_WACK_BREQ	)
#define NSF_WACK_UREQ	( 1 << NS_WACK_UREQ	)
#define NSF_IDLE	( 1 << NS_IDLE		)
#define NSF_WACK_OPTREQ	( 1 << NS_WACK_OPTREQ	)
#define NSF_WACK_RRES	( 1 << NS_WACK_RRES	)
#define NSF_WCON_CREQ	( 1 << NS_WCON_CREQ	)
#define NSF_WRES_CIND	( 1 << NS_WRES_CIND	)
#define NSF_WACK_CRES	( 1 << NS_WACK_CRES	)
#define NSF_DATA_XFER	( 1 << NS_DATA_XFER	)
#define NSF_WCON_RREQ	( 1 << NS_WCON_RREQ	)
#define NSF_WRES_RIND	( 1 << NS_WRES_RIND	)
#define NSF_WACK_DREQ6	( 1 << NS_WACK_DREQ6	)
#define NSF_WACK_DREQ7	( 1 << NS_WACK_DREQ7	)
#define NSF_WACK_DREQ9	( 1 << NS_WACK_DREQ9	)
#define NSF_WACK_DREQ10	( 1 << NS_WACK_DREQ10	)
#define NSF_WACK_DREQ11	( 1 << NS_WACK_DREQ11	)
#endif
#define NSF_NOSTATES	( 1 << NS_NOSTATES	)

#define NSF_WACK_DREQ	(NSF_WACK_DREQ6 \
			|NSF_WACK_DREQ7 \
			|NSF_WACK_DREQ9 \
			|NSF_WACK_DREQ10 \
			|NSF_WACK_DREQ11)
#define NSF_WACK	(NSF_WACK_BREQ \
			|NSF_WACK_UREQ \
			|NSF_WACK_OPTREQ \
			|NSF_WACK_RRES \
			|NSF_WACK_CRES \
			|NSF_WACK_DREQ6 \
			|NSF_WACK_DREQ7 \
			|NSF_WACK_DREQ9 \
			|NSF_WACK_DREQ10 \
			|NSF_WACK_DREQ11)

static void
mtp_bind(struct mtp *mtp, struct mtp_addr *src)
{
	if (src) {
		mtp->src = *src;
		mtp->src_len = sizeof(*src);
	}
	return;
}
static inline void
mtp_connect(struct mtp *mtp, struct mtp_addr *dst)
{
	if (dst) {
		mtp->dst = *dst;
		mtp->dst_len = sizeof(*dst);
	}
	return;
}
static void
mtp_unbind(struct mtp *mtp)
{
	mtp->src_len = 0;
	return;
}
static void
mtp_disconnect(struct mtp *mtp)
{
	mtp->dst_len = 0;
	return;
}

/*
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  Primitives sent upstream
 *  -------------------------------------------------------------------------
 */
/**
 * m_error: - issue an M_ERROR message upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error value
 */
static int
m_error(struct mtp *mtp, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		mtp_set_state(mtp, NS_NOSTATES);
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_ERROR");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_ind: - issue a T_CONN_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @src: souce address
 * @opt: options
 * @dp: user data
 */
static inline fastcall __unlikely int
n_conn_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	   np_ulong seq, np_ulong flags, struct mtp_addr *src, struct mtp_addr *dst,
	   union N_qos_mtp *qos, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	const size_t src_len = n_addr_size(src);
	const size_t dst_len = n_addr_size(dst);
	const size_t qos_len = n_opts_size(qos);
	const size_t msg_len = sizeof(*p) + src_len + dst_len + qos_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p) + src_len;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + src_len + dst_len;
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			mp->b_wptr += sizeof(*p);
			n_build_addr(src, mp->b_wptr);
			mp->b_wptr += src_len;
			n_build_addr(dst, mp->b_wptr);
			mp->b_wptr += dst_len;
			n_build_opts(qos, mp->b_wptr);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_CONN_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_con: - issue a N_CONN_CON primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @res: responding address
 * @opt: options
 * @dp: user data
 */
static inline fastcall __unlikely int
n_conn_con(struct mtp *mtp, queue_t *q, mblk_t *bp,
	   np_ulong flags, struct mtp_addr *res, union N_qos_mtp *qos, mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;
	const size_t res_len = n_addr_size(res);
	const size_t qos_len = n_opts_size(qos);
	const size_t msg_len = sizeof(*p) + res_len + qos_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + res_len;
			mp->b_wptr += sizeof(*p);
			n_build_addr(res, mp->b_wptr);
			mp->b_wptr += res_len;
			n_build_opts(qos, mp->b_wptr);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_CONN_CON");
			if (mtp_get_state(mtp) != NS_WCON_CREQ)
				swerr();
			mtp_set_state(mtp, NS_DATA_XFER);
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_ind: - issue a N_DISCON_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @reason: reason for disconnection
 * @dp: user data
 */
static inline fastcall __unlikely int
n_discon_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	     np_ulong orig, np_ulong reason, np_ulong seq, struct mtp_addr *res, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	const size_t res_len = n_addr_size(res);
	const size_t msg_len = sizeof(*p) + res_len;

	if (mtp_chk_state
	    (mtp,
	     (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ | NSF_WRES_RIND))) {
		if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
			if (likely(canputnext(mtp->rq))) {
				DB_TYPE(mp) = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = N_DISCON_IND;
				p->DISCON_orig = orig;
				p->DISCON_reason = reason;
				p->RES_length = res_len;
				p->RES_offset = sizeof(*p);
				p->SEQ_number = seq;
				mp->b_wptr += sizeof(*p);
				n_build_addr(res, mp->b_wptr);
				mp->b_wptr += res_len;
				mp->b_cont = dp;
				if (bp)
					freeb(bp);
				mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_DISCON_IND");
				putnext(mtp->rq, mp);
				return (0);
			}
			freeb(mp);
			return (-EBUSY);
		}
		return (-ENOBUFS);
	}
	mi_strlog(q, 0, SL_ERROR, "unexpected indication for state %u", mtp_get_state(mtp));
	freeb(bp);
	freemsg(dp);
	return (0);
}

/**
 * n_data_ind: - issue a N_DATA_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @more: more data flag
 * @dp: user data
 */
static inline fastcall __unlikely int
n_data_ind(struct mtp *mtp, queue_t *q, mblk_t *bp, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	if (mtp_chk_state(mtp, (NSF_DATA_XFER | NSF_WCON_RREQ))) {
		if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
			if (likely(canputnext(mtp->rq))) {
				DB_TYPE(mp) = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = N_DATA_IND;
				p->DATA_xfer_flags = flags;
				mp->b_wptr += sizeof(*p);
				mp->b_cont = dp;
				if (bp)
					freeb(bp);
				mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_DATA_IND");
				putnext(mtp->rq, mp);
				return (0);
			}
			freeb(mp);
			return (-EBUSY);
		}
		return (-ENOBUFS);
	}
	mi_strlog(q, 0, SL_ERROR, "unexpected indication for state %u", mtp_get_state(mtp));
	freeb(bp);
	freemsg(dp);
	return (0);
}

/**
 * t_exdata_ind: - issue a T_EXDATA_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @more: more data flag
 * @dp: user data
 */
static inline fastcall __unlikely int
n_exdata_ind(struct mtp *mtp, queue_t *q, mblk_t *bp, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	if (mtp_chk_state(mtp, (NSF_DATA_XFER | NSF_WCON_RREQ))) {
		if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
			if (likely(canputnext(mtp->rq))) {
				DB_TYPE(mp) = M_PROTO;
				mp->b_band = 1;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = N_EXDATA_IND;
				mp->b_wptr += sizeof(*p);
				mp->b_cont = dp;
				if (bp)
					freeb(bp);
				mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_EXDATA_IND");
				putnext(mtp->rq, mp);
				return (0);
			}
			freeb(mp);
			return (-EBUSY);
		}
		return (-ENOBUFS);
	}
	mi_strlog(q, 0, SL_ERROR, "unexpected indication for state %u", mtp_get_state(mtp));
	freeb(bp);
	freemsg(dp);
	return (0);
}

/**
 * n_info_ack: - issue a N_INFO_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
n_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;
	size_t src_len, dst_len, msg_len;
	N_qos_sel_info_mtp_t *qos;
	N_qos_range_info_mtp_t *qor;
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);

	switch (mtp_get_state(mtp)) {
	default:
	case NS_UNBND:
		src_len = 0;
		dst_len = 0;
		break;
	case NS_IDLE:
		src_len = sizeof(mtp->src);
		dst_len = 0;
		break;
	case NS_DATA_XFER:
		src_len = sizeof(mtp->src);
		dst_len = sizeof(mtp->dst);
		break;
	}
	msg_len = sizeof(*p) + src_len + dst_len + qos_len + qor_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		*p = mtp->prot;
		p->ADDR_size = sizeof(struct mtp_addr);
		p->ADDR_length = src_len + dst_len;
		p->ADDR_offset = src_len + dst_len ? sizeof(*p) : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + src_len + dst_len : 0;
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = qor_len ? sizeof(*p) + src_len + dst_len + qos_len : 0;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		if (src_len) {
			bcopy(&mtp->src, mp->b_wptr, sizeof(mtp->src));
			mp->b_wptr += sizeof(mtp->src);
		}
		if (dst_len) {
			bcopy(&mtp->dst, mp->b_wptr, sizeof(mtp->dst));
			mp->b_wptr += sizeof(mtp->dst);
		}
		if (qos_len) {
			qos = (typeof(qos)) mp->b_wptr;
			mp->b_wptr += sizeof(*qos);
			qos->n_qos_type = N_QOS_SEL_INFO_MTP;
			qos->pvar = mtp->options.pvar;
			qos->popt = mtp->options.popt;
		}
		if (qor_len) {
			qor = (typeof(qor)) mp->b_wptr;
			mp->b_wptr += sizeof(*qor);
			qor->n_qos_type = N_QOS_RANGE_INFO_MTP;
			qor->sls_range = mtp->options.sls_mask;
			qor->mp_range = (mtp->options.popt & SS7_POPT_MPLEV) ? 3 : 0;
		}
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * n_bind_ack: - issue a N_BIND_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: bound address
 * @cons: negotiated number of oustanding connection indications
 */
static inline fastcall __unlikely int
n_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *add, np_ulong cons)
{
	N_bind_ack_t *p;
	mblk_t *mp;
	size_t add_len = n_addr_size(add);
	size_t msg_len = sizeof(*p) + add_len;

	if (mtp_get_state(mtp) == NS_WACK_BREQ) {
		if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
			DB_TYPE(mp) = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_BIND_ACK;
			p->ADDR_length = add_len;
			p->ADDR_offset = add_len ? sizeof(*p) : 0;
			p->CONIND_number = cons;
			p->TOKEN_value = (np_ulong) (long) mtp->rq;
			p->PROTOID_length = 0;
			p->PROTOID_offset = 0;
			mp->b_wptr += sizeof(*p);
			n_build_addr(add, mp->b_wptr);
			mp->b_wptr += add_len;
			mtp_bind(mtp, add);
			mtp_set_state(mtp, NS_IDLE);
			freemsg(msg);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_BIND_ACK");
			putnext(mtp->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	mi_strlog(q, 0, SL_ERROR, "unexpected indication for state %u", mtp_get_state(mtp));
	freemsg(msg);
	return (0);
}

/*
 * n_error_ack: - issue a N_ERROR_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @etypr: error type
 */
static inline fastcall __unlikely int
n_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong prim, np_long etype)
{
	N_error_ack_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = etype < 0 ? NSYSERR : etype;
		p->UNIX_error = etype < 0 ? -etype : 0;
		mp->b_wptr += sizeof(*p);
		switch (mtp_get_state(mtp)) {
#ifdef NS_WACK_OPTREQ
		case NS_WACK_OPTREQ:
#endif
		case NS_WACK_UREQ:

			mtp_set_state(mtp, NS_IDLE);
			break;
		case NS_WACK_BREQ:
			mtp_set_state(mtp, NS_UNBND);
			break;
		case NS_WACK_CRES:
			mtp_set_state(mtp, NS_WRES_CIND);
			break;
		case NS_WACK_DREQ6:
			mtp_set_state(mtp, NS_WCON_CREQ);
			break;
		case NS_WACK_DREQ7:
			mtp_set_state(mtp, NS_WRES_CIND);
			break;
		case NS_WACK_DREQ9:
			mtp_set_state(mtp, NS_DATA_XFER);
			break;
		case NS_WACK_DREQ10:
			mtp_set_state(mtp, NS_WCON_RREQ);
			break;
		case NS_WACK_DREQ11:
			mtp_set_state(mtp, NS_WRES_RIND);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we send NOUTSTATE or NNOTSUPPORT or are responding
			   to a N_OPTMGMT_REQ in other than NS_IDLE state. */
			break;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(mtp->rq, mp);
		/* Retruning -EPROTO here will make sure that the old state is restored correctly.
		   If we return 0, then the state will never be restored. */
		if (etype >= 0)
			return (-EPROTO);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_ok_ack: - issue a N_OK_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 * @seq: sequence number
 * @tok: stream token
 */
static inline fastcall __unlikely int
n_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (mtp_get_state(mtp)) {
		case NS_WACK_RRES:
			mtp_set_state(mtp, NS_DATA_XFER);
			break;
		case NS_WACK_UREQ:
			mtp_unbind(mtp);
			mtp_set_state(mtp, NS_UNBND);
			break;
		case NS_WACK_OPTREQ:
			mtp_set_state(mtp, NS_IDLE);
			break;
		case NS_WACK_CRES:
			/* FIXME: need to use sequence and token */
			mtp_set_state(mtp, NS_DATA_XFER);
			break;
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ7:
			/* FIXME: need to handle sequence numbers */
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			mtp_disconnect(mtp);
			mtp_set_state(mtp, NS_IDLE);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a N_OPTMGMT_REQ in other than
			   the NS_IDLE state. */
			break;
		}
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unitdata_ind: - issue a N_UNITDATA_IND primitive
 * @mtp: MTP private data
 * @q: active queue
 * @bp: message block to free upon success
 * @src: source address (or NULL)
 * @opt: options (or NULL)
 * @dp: user data
 */
static inline fastcall __unlikely int
n_unitdata_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	       struct mtp_addr *src, struct mtp_addr *dst, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	size_t src_len = n_addr_size(src);
	size_t dst_len = n_addr_size(dst);
	size_t msg_len = sizeof(*p) + src_len + dst_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->rq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = src_len ? sizeof(*p) : 0;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
			mp->b_wptr += sizeof(*p);
			n_build_addr(src, mp->b_wptr);
			mp->b_wptr += src_len;
			n_build_addr(dst, mp->b_wptr);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_UNITDATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: - issue a T_UDERROR_IND primitive
 * @mtp: MTP private data
 * @q: active queue
 * @bp: message block to free upon success
 * @dst: destination address (or NULL)
 * @opt: options (or NULL)
 * @dp: user data
 * @etype: error type
 */
static inline fastcall __unlikely int
n_uderror_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	      struct mtp_addr *dst, mblk_t *dp, mtp_ulong etype)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	size_t dst_len = n_addr_size(dst);
	size_t msg_len = sizeof(*p) + dst_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->rq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			n_build_addr(dst, mp->b_wptr);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_UDERROR_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_datack_ind: - issue a N_DATACK_IND primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
n_datack_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_DATACK_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_ind: - issue a N_RESET_IND primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 * @orig: origin of reset
 * @reason: reason for reset
 */
static inline fastcall __unlikely int
n_reset_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = orig;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_RESET_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_con: - issue a N_RESET_CON primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
n_reset_con(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- N_RESET_CON");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_bind_req: - issue a MTP_BIND_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 * @add: address to which to bind (or NULL)
 * @flags: bind flags
 */
static inline fastcall __unlikely int
mtp_bind_req(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *add, mtp_ulong flags)
{
	struct MTP_bind_req *p;
	mblk_t *mp;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_REQ;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_bind_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_BIND_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_unbind_req: - issue a MTP_UNBIND_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
mtp_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_unbind_req *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_UNBIND_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_conn_req: - issue a MTP_CONN_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @bp: message block to free upon success
 * @add: address to which to connect (or NULL)
 * @flags: connect flags
 * @dp: user data
 */
static inline fastcall __unlikely int
mtp_conn_req(struct mtp *mtp, queue_t *q, mblk_t *bp,
	     struct mtp_addr *add, mtp_ulong flags, mblk_t *dp)
{
	struct MTP_conn_req *p;
	mblk_t *mp;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_CONN_REQ;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
			p->mtp_conn_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_CONN_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_discon_req: - issue an MTP_DISCON_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @bp: message block to free upon success
 * @dp: user data
 */
static inline fastcall __unlikely int
mtp_discon_req(struct mtp *mtp, queue_t *q, mblk_t *bp, mblk_t *dp)
{
	struct MTP_discon_req *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_DISCON_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_DISCON_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_request: - issue an MTP_ADDR_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
mtp_addr_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_addr_req *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_ADDR_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_req: - issue an MTP_INFO_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
mtp_info_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_info_req *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_INFO_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_req: - issue an MTP_OPTMGMT_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 * @opt: options (or NULL)
 * @flags: management flags
 */
static inline fastcall __unlikely int
mtp_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *msg, union N_qos_mtp *opt, mtp_ulong flags)
{
	struct MTP_optmgmt_req *p;
	mblk_t *mp;
	struct t_opthdr *oh;
	size_t opt_len = opt ? 2 * sizeof(*oh) + 2 * sizeof(t_scalar_t) : 0;
	size_t msg_len = sizeof(*p) + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_OPTMGMT_REQ;
			p->mtp_opt_length = opt_len;
			p->mtp_opt_offset = sizeof(*p);
			p->mtp_mgmt_flags = flags;
			mp->b_wptr += sizeof(*p);
			if (opt_len) {
				oh = (typeof(oh)) mp->b_wptr;
				oh->level = T_SS7_MTP;
				oh->name = T_MTP_SLS;
				oh->len = sizeof(*oh) + sizeof(t_scalar_t);
				oh->status = T_SUCCESS;
				mp->b_wptr += sizeof(*oh);
				*(t_scalar_t *)mp->b_wptr = opt->n_qos_info.sls;
				mp->b_wptr += sizeof(t_scalar_t);
				oh = (typeof(oh)) mp->b_wptr;
				oh->level = T_SS7_MTP;
				oh->name = T_MTP_MP;
				oh->len = sizeof(*oh) + sizeof(t_scalar_t);
				oh->status = T_SUCCESS;
				mp->b_wptr += sizeof(*oh);
				*(t_scalar_t *)mp->b_wptr = opt->n_qos_info.mp;
				mp->b_wptr += sizeof(t_scalar_t);
			}
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_OPTMGMT_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_req: - issue an MTP_TRANSFER_REQ primitive
 * @mtp: MTP private data
 * @q: active queue
 * @bp: message block to free upon success
 * @dst: destination address (or NULL)
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static inline fastcall __unlikely int
mtp_transfer_req(struct mtp *mtp, queue_t *q, mblk_t *bp,
		 struct mtp_addr *dst, mtp_ulong pri, mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_req *p;
	mblk_t *mp;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t msg_len = sizeof(*p) + dst_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->wq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_REQ;
			p->mtp_dest_length = dst_len;
			p->mtp_dest_offset = dst_len ? sizeof(*p) : 0;
			p->mtp_mp = pri;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGDA, SL_TRACE, "MTP_TRANSFER_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from above.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * n_data: - process M_DATA message
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * To support pseudo-connectionless modes, when this message is sent for N_CLNS we should send the
 * data to the same adress and with the same options as the last N_UNITDATA_REQ primitive.
 */
static inline fastcall __hot_read int
n_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const size_t dlen = msgdsize(mp);

	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mtp_chk_state(mtp, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	if (mtp_get_state(mtp) != NS_DATA_XFER)
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot.NSDU_size || dlen > mtp->prot.NIDU_size)
		goto baddata;
	return mtp_transfer_req(mtp, q, mp, &mtp->dst, mtp->options.mp, mtp->options.sls, mp);
      baddata:
	mi_strlog(q, 0, SL_TRACE, "bad data size %lu", (ulong)dlen);
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      discard:
	mi_strlog(q, 0, SL_TRACE, "ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for N_CLNS");
	goto error;
      error:
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * n_conn_req: - process N_CONN_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * As MTP is really a connectionless protocol, when we form a connection we simply remember the
 * destination address.  Some interim MTPs had the abilitty to send a UPT (User Part Test) message.
 * If the protocol variant has this ability, we wait for the result of the User Part Test before
 * confirming the connection.
 */
static inline fastcall __unlikely int
n_conn_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	struct mtp_opts opts;

	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badprim;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	if (dst->family != AF_MTP)
		goto badaddr;
	if (dst->si == 0 && mtp->src.si == 0)
		goto noaddr;
	if (dst->si < 3 && mtp->src.si != 0)
		goto badaddr;
	if (dst->si < 3 && mtp->cred.cr_uid != 0)
		goto access;
	if (dst->si != mtp->src.si && mtp->src.si != 0)
		goto badaddr;
	if (n_parse_opts(mtp, &opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badqostype;
	/* TODO: set options first */
	if (mp->b_cont) {
		putbq(q, mp->b_cont);	/* hold back data */
		mp->b_cont = NULL;	/* abosrbed mp->b_cont */
	}
	mtp->dst = *dst;
	mtp_set_state(mtp, NS_WCON_CREQ);
	return n_conn_con(mtp, q, mp, 0, &mtp->dst, NULL, NULL);
      access:
	err = NACCESS;
	mi_strlog(q, 0, SL_TRACE, "no permission");
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	mi_strlog(q, 0, SL_TRACE, "bad qos type");
	goto error;
#if 0
      badqosparam:
	err = NBADQOSPARAM;
	mi_strlog(q, 0, SL_TRACE, "bad qos parameter");
	goto error;
#endif
      badaddr:
	err = NBADADDR;
	mi_strlog(q, 0, SL_TRACE, "bad destination address");
	goto error;
      noaddr:
	err = NNOADDR;
	mi_strlog(q, 0, SL_TRACE, "couldn't allocate destination address");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for N_CLNS");
	goto error;
      error:
	return n_error_ack(mtp, q, mp, N_CONN_REQ, err);
}

/**
 * n_conn_res: - process N_CONN_RES primtive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_conn_res(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const N_conn_res_t *p = (typeof(p)) mp->b_rptr;

	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mtp_get_state(mtp) != NS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* We never give an N_CONN_IND, so there is no reason for a N_CONN_RES.  We probably could
	   do this (issue an N_CONN_IND on a listening stream when there is no other MTP user for
	   the SI value and * send a UPU on an N_DISCON_REQ or just redirect all traffic for that
	   user on a N_CONN_RES) but that is for later. */
	goto eopnotsupp;
      eopnotsupp:
	err = -EOPNOTSUPP;
	mi_strlog(q, 0, SL_TRACE, "operation not supported");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported");
	goto error;
      error:
	return n_error_ack(mtp, q, mp, N_CONN_RES, err);
}

/**
 * n_discon_req: - process N_DISCON_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_discon_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const N_discon_req_t *p = (typeof(p)) mp->b_rptr;

	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* Currently there are only three states we can disconnect from.  The first does not
	   happen. Only the second one is normal.  The third should occur during simulteneous
	   diconnect only. */
	switch (mtp_get_state(mtp)) {
	case NS_WCON_CREQ:
		mtp_set_state(mtp, NS_WACK_DREQ6);
		break;
	case NS_DATA_XFER:
		mtp_set_state(mtp, NS_WACK_DREQ9);
		break;
	case NS_IDLE:
		rare();
		break;
	default:
		goto outstate;
	}
	/* change state and let mtp_ok_ack do all the work */
	return mtp_discon_req(mtp, q, mp, NULL);
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for N_CLNS");
	goto error;
      error:
	return n_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * n_data_req: - process N_DATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_data_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const N_data_req_t *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp) : 0;

	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mtp_get_state(mtp) == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (!mp->b_cont)
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(NSF_DATA_XFER | NSF_WRES_RIND | NSF_WCON_RREQ))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot.NSDU_size || dlen > mtp->prot.NIDU_size)
		goto baddata;
	if (p->DATA_xfer_flags)
		/* N_MORE_DATA_FLAG and N_RC_FLAG not supported yet.  We could do N_MORE_DATA_FLAG
		   pretty easily by accumulating the packet until the last data request is
		   received, but this would be rather pointless for small MTP packet sizes.
		   N_RC_FLAG cannot be supported until the DLPI link driver is done and zero-loss
		   operation is completed. */
		goto notsupport;
	return mtp_transfer_req(mtp, q, mp, &mtp->dst, mtp->options.mp, mtp->options.sls,
				mp->b_cont);
      baddata:
	mi_strlog(q, 0, SL_TRACE, "bad data size %lu", (ulong) dlen);
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      einval:
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      discard:
	mi_strlog(q, 0, SL_TRACE, "ignore in idle state");
	return (0);
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for N_CLNS");
	goto error;
      error:
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * n_exdata_req: - process N_EXDATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_exdata_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	(void) mp;
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * n_info_req: - process N_INFO_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	return n_info_ack(mtp, q, mp);
}

/**
 * n_bind_req:- process N_BIND_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_bind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr src;

	if (mtp_get_state(mtp) != NS_UNBND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->CONIND_number)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
		goto badprim;
	if (p->PROTOID_length)
		goto badaddr;
	if (!p->ADDR_length)
		goto noaddr;
	if (p->ADDR_length < sizeof(src))
		goto badaddr;
	bcopy(mp->b_rptr + p->ADDR_offset, &src, sizeof(src));
	/* we don't allow wildcards yet. */
	if (src.family != AF_MTP)
		goto badaddr;
	if (!src.si || !src.pc)
		goto noaddr;
	if (src.si < 3 && mtp->cred.cr_uid != 0)
		goto acces;
	mtp_set_state(mtp, NS_WACK_BREQ);
	return mtp_bind_req(mtp, q, mp, &src, 0);
      acces:
	err = NACCESS;
	mi_strlog(q, 0, SL_TRACE, "no priviledge for requested address");
	goto error;
      noaddr:
	err = NNOADDR;
	mi_strlog(q, 0, SL_TRACE, "could not allocate address");
	goto error;
      badaddr:
	err = NBADADDR;
	mi_strlog(q, 0, SL_TRACE, "requested address invalid");
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not support for N_CLNS");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      error:
	return n_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_unbind_req: - process T_UNBIND_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	mtp_set_state(mtp, NS_WACK_UREQ);
	return mtp_unbind_req(mtp, q, mp);
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      error:
	return n_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_unitdata_req: - process T_UNITDATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_unitdata_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	struct mtp_addr dst;
#if 0
	struct mtp_opts opts = { 0L, NULL, };
#endif

	if (mtp->prot.SERV_type != N_CLNS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (dlen == 0)
		goto baddata;
	if (dlen > mtp->prot.NSDU_size || dlen > mtp->prot.NIDU_size)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(dst))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_offset, &dst, sizeof(dst));
	if (dst.family != AF_MTP)
		goto badaddr;
	if (!dst.si || !dst.pc)
		goto badaddr;
	if (dst.si < 3 && mtp->cred.cr_uid != 0)
		goto acces;
	if (dst.si != mtp->src.si)
		goto badaddr;
#if 0
	if (n_parse_opts(&opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badopt;
#endif
	fixme(("Handle options correctly\n"));
	return mtp_transfer_req(mtp, q, mp, &dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
#if 0
      badopt:
	mi_strlog(q, 0, SL_TRACE, "bad options");
	goto error;
#endif
      acces:
	mi_strlog(q, 0, SL_TRACE, "no permission to address");
	goto error;
      badaddr:
	mi_strlog(q, 0, SL_TRACE, "requested address invalid");
	goto error;
      noaddr:
	mi_strlog(q, 0, SL_TRACE, "could not allocate address");
	goto error;
      baddata:
	mi_strlog(q, 0, SL_TRACE, "invalid amount of data");
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      badprim:
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "primitive type not supported");
	goto error;
      error:
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * t_optmgmt_req: - process T_OPTMGMT_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
n_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err = 0;
	const N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	union N_qos_mtp *qos = { 0L, };

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badprim;
#ifdef NS_WACK_OPTREQ
	if (mtp_get_state(mtp) == NS_IDLE)
		mtp_set_state(mtp, NS_WACK_OPTREQ);
#endif
	if (p->OPTMGMT_flags)
		/* Can't support DEFAULT_RC_SEL yet */
		goto badflags;
	qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
	if (p->QOS_length < sizeof(qos->n_qos_data))
		goto badqostype;
	if (qos->n_qos_type != N_QOS_SEL_DATA_MTP)
		goto badqostype;
	if (qos->n_qos_data.sls > 255 && qos->n_qos_data.sls != -1U)
		goto badqosparam;
	if (qos->n_qos_data.mp > 3 && qos->n_qos_data.mp != -1U)
		goto badqosparam;
	mtp->options.sls = qos->n_qos_data.sls;
	mtp->options.mp = qos->n_qos_data.mp;
	return n_ok_ack(mtp, q, mp, N_OPTMGMT_REQ);
      badqostype:
	err = NBADQOSTYPE;
	mi_strlog(q, 0, SL_TRACE, "invalid qos type");
	goto error;
      badqosparam:
	err = NBADQOSPARAM;
	mi_strlog(q, 0, SL_TRACE, "invalid qos parameter");
	goto error;
      badflags:
	err = NBADFLAG;
	mi_strlog(q, 0, SL_TRACE, "invalid flag");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      error:
	return n_error_ack(mtp, q, mp, N_OPTMGMT_REQ, err);
}

/*
 *  N_DATACK_REQ:
 *  -------------------------------------------------------------------
 */
static inline fastcall __unlikely int
n_datack_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	   We don't support DATACK yet.  With zero loss operation we will. */
	rare();
	return (0);
}

/*
 *  N_RESET_REQ:
 *  -------------------------------------------------------------------
 */
static inline fastcall __unlikely int
n_reset_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	todo(("Accept resets with reason from the user\n"));
	rare();
	return (0);
}

/*
 *  N_RESET_RES:
 *  -------------------------------------------------------------------
 */
static inline fastcall __unlikely int
n_reset_res(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	   ignore.  if the user wishes to respond to our reset indications that's fine. */
	rare();
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from below.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_data: - process M_DATA message
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __hot_in int
mtp_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	if (mtp->prot.SERV_type == N_CONS)
		return (QR_PASSALONG);
	swerr();
	return (-EFAULT);
}

/**
 * mtp_ok_ack: - process MTP_OK_ACK primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Simply translate the MTP_OK_ACK into a N_OK_ACK.
 */
static inline fastcall __unlikely int
mtp_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	mtp_ulong prim;
	struct MTP_ok_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (p->mtp_correct_prim) {
	case MTP_BIND_REQ:
		swerr();
		prim = N_BIND_REQ;
		break;
	case MTP_UNBIND_REQ:
		prim = N_UNBIND_REQ;
		break;
	case MTP_CONN_REQ:
		prim = N_CONN_REQ;
		if ((err = n_ok_ack(mtp, q, NULL, prim)) < 0)
			return (err);
		return n_conn_con(mtp, q, mp, 0, NULL, NULL, NULL);
	case MTP_DISCON_REQ:
		prim = N_DISCON_REQ;
		break;
	case MTP_ADDR_REQ:
		swerr();
		prim = N_INFO_REQ;
		break;
	case MTP_INFO_REQ:
		swerr();
		prim = N_INFO_REQ;
		break;
	case MTP_OPTMGMT_REQ:
		swerr();
		prim = N_OPTMGMT_REQ;
		break;
	case MTP_TRANSFER_REQ:
		swerr();
		prim = N_DATA_REQ;
		break;
	default:
		swerr();
		prim = 0;
		break;
	}
	return n_ok_ack(mtp, q, mp, prim);
      efault:
	mi_strlog(q, 0, SL_ERROR, "invalid primitive from below");
	return (-EFAULT);
}

/**
 * mtp_error_ack: - process MTP_ERROR_ACK primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Simply translate the MTP_ERROR_ACK into a N_ERROR_ACK.
 */
static inline fastcall __unlikely int
mtp_error_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	mtp_ulong prim;
	struct MTP_error_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (p->mtp_primitive) {
	case MTP_BIND_REQ:
		prim = N_BIND_REQ;
		break;
	case MTP_UNBIND_REQ:
		prim = N_UNBIND_REQ;
		break;
	case MTP_CONN_REQ:
		prim = N_CONN_REQ;
		break;
	case MTP_DISCON_REQ:
		prim = N_DISCON_REQ;
		break;
	case MTP_ADDR_REQ:
		swerr();
		prim = N_INFO_REQ;
		break;
	case MTP_INFO_REQ:
		prim = N_INFO_REQ;
		break;
	case MTP_OPTMGMT_REQ:
		prim = N_OPTMGMT_REQ;
		break;
	case MTP_TRANSFER_REQ:
		swerr();
		prim = N_DATA_REQ;
		break;
	default:
		swerr();
		prim = 0;
		break;
	}
	switch (p->mtp_mtpi_error) {
	case MSYSERR:
		err = -p->mtp_unix_error;
		break;
	case MACCESS:
		err = NACCESS;
		break;
	case MBADADDR:
		err = NBADADDR;
		break;
	case MNOADDR:
		err = NNOADDR;
		break;
	case MBADPRIM:
		err = -EINVAL;
		break;
	case MOUTSTATE:
		err = NOUTSTATE;
		break;
	case MNOTSUPP:
		err = NNOTSUPPORT;
		break;
	case MBADFLAG:
		err = NBADFLAG;
		break;
	case MBADOPT:
		err = NBADOPT;
		break;
	default:
		swerr();
		err = -EFAULT;
		break;
	}
	return n_error_ack(mtp, q, mp, prim, err);
      efault:
	mi_strlog(q, 0, SL_ERROR, "invalid primitive from below");
	return (-EFAULT);
}

/**
 * mtp_bind_ack: - process MTP_BIND_ACK primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Translate the MTP_BIND_ACK into a T_BIND_ACK.
 */
static inline fastcall __unlikely int
mtp_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_bind_ack *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *add = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	if (p->mtp_addr_length == sizeof(*add))
		add = (typeof(add)) (mp->b_rptr + p->mtp_addr_offset);
	return n_bind_ack(mtp, q, mp, add, 0);
      efault:
	mi_strlog(q, 0, SL_ERROR, "invalid primitive from below");
	return (-EFAULT);
}

/**
 * mtp_addr_ack: - process MTP_ADDR_ACK primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Simply translate MTP_ADDR_ACK to T_ADDR_ACK.
 */
static inline fastcall __unlikely int
mtp_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_addr_ack *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *loc = NULL, *rem = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_loc_offset + p->mtp_loc_length)
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_rem_offset + p->mtp_rem_length)
		goto efault;
	if (p->mtp_loc_length == sizeof(*loc))
		loc = (typeof(loc)) (mp->b_rptr + p->mtp_loc_offset);
	if (p->mtp_rem_length == sizeof(*rem))
		rem = (typeof(rem)) (mp->b_rptr + p->mtp_rem_offset);
	// return n_addr_ack(mtp, q, mp, loc, rem);
	freemsg(mp);
	return (0);
      efault:
	mi_strlog(q, 0, SL_ERROR, "invalid primitive from below");
	return (-EFAULT);
}

/**
 * mtp_info_ack: - process MTP_INFO_ACK primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Simply translate MTP_INFO_ACK to T_INFO_ACK, unless this is the very first info ack to our
 * MTP_INFO_REQ issues from the mtp_qopen() procedure.  This can be identified by the fact that the
 * current state is NS_NOSTATES.
 */
static inline fastcall __unlikely int
mtp_info_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	int first = (mtp->prot.CURRENT_state == -1U);

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto badaddr;
	mtp->prot.NSDU_size = p->mtp_msu_size;
	mtp->prot.NIDU_size = p->mtp_msu_size;
	mtp->prot.ADDR_size = p->mtp_addr_size;
	switch (p->mtp_current_state) {
	case MTPS_UNBND:	/* 0UL */
		mtp->prot.CURRENT_state = NS_UNBND;
		break;
	case MTPS_WACK_BREQ:	/* 1UL */
		mtp->prot.CURRENT_state = NS_WACK_BREQ;
		break;
	case MTPS_IDLE:	/* 2UL */
		mtp->prot.CURRENT_state = NS_IDLE;
		break;
	case MTPS_WACK_CREQ:	/* 3UL */
		mtp->prot.CURRENT_state = NS_WCON_CREQ;
		break;
	case MTPS_WCON_CREQ:	/* 4UL */
		mtp->prot.CURRENT_state = NS_WCON_CREQ;
		break;
	case MTPS_CONNECTED:	/* 5UL */
		mtp->prot.CURRENT_state = NS_DATA_XFER;
		break;
	case MTPS_WACK_UREQ:	/* 6UL */
		mtp->prot.CURRENT_state = NS_WACK_UREQ;
		break;
	case MTPS_WACK_DREQ6:	/* 7UL */
		mtp->prot.CURRENT_state = NS_WACK_DREQ6;
		break;
	case MTPS_WACK_DREQ9:	/* 8UL */
		mtp->prot.CURRENT_state = NS_WACK_DREQ9;
		break;
	case MTPS_WACK_OPTREQ:	/* 9UL */
		mtp->prot.CURRENT_state = NS_WACK_OPTREQ;
		break;
	case MTPS_WREQ_ORDREL:	/* 10UL */
		mtp->prot.CURRENT_state = NS_WRES_RIND;
		break;
	case MTPS_WIND_ORDREL:	/* 11UL */
		mtp->prot.CURRENT_state = NS_WCON_RREQ;
		break;
	case MTPS_WRES_CIND:	/* 12UL */
		mtp->prot.CURRENT_state = NS_WRES_CIND;
		break;
	case MTPS_UNUSABLE:	/* 0xffffffffUL */
		mtp->prot.CURRENT_state = NS_NOSTATES;
		break;
	default:
		mtp->prot.CURRENT_state = NS_NOSTATES;
		break;
	}
	switch (p->mtp_serv_type) {
	case M_COMS:
		mtp->prot.SERV_type = N_CONS;
		mtp->prot.CDATA_size = p->mtp_msu_size;
		mtp->prot.DDATA_size = p->mtp_msu_size;
		break;
	case M_CLMS:
		mtp->prot.SERV_type = N_CLNS;
		mtp->prot.CDATA_size = -2;
		mtp->prot.DDATA_size = -2;
		break;
	default:
		swerr();
		break;
	}
	if (!first)
		return n_info_ack(mtp, q, mp);
      error:
	freemsg(mp);
	return (0);
      badaddr:
	mi_strlog(q, 0, SL_ERROR, "bad address or incorrect format");
	goto error;
      protoshort:
	mi_strlog(q, 0, SL_ERROR, "primitive too short");
	goto error;
}

/**
 * mtp_optmgmt_ack: - process MTP_OPTMGMT_ACK primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
mtp_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/**
 * mtp_transfer_ind: - translate MTP_TRANSFER_IND into N_UNITDATA_IND or N_OPTDATA_IND
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
mtp_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *src = NULL;

	if (mp->b_cont == NULL)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mtp_chk_state(mtp, NSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		if (mtp_not_state(mtp, (NSF_WCON_RREQ | NSF_DATA_XFER)))
			goto outstate;
		return n_data_ind(mtp, q, mp, 0, mp->b_cont);
	case N_CLNS:
		if (mtp_not_state(mtp, (NSF_IDLE | NSF_WACK_UREQ)))
			goto outstate;
		if (mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length)
			goto badaddr;
		if (p->mtp_srce_length) {
			if (p->mtp_srce_length < sizeof(*src))
				goto badaddr2;
			src = (typeof(src)) (mp->b_rptr + p->mtp_srce_offset);
		}
		mp->b_cont->b_band = mp->b_band;
		return n_unitdata_ind(mtp, q, mp, src, &mtp->src, mp->b_cont);
	default:
		break;
	}
      error:
	freemsg(mp);
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "data received in incorrect state %u", mtp_get_state(mtp));
	goto error;
      badaddr2:
	mi_strlog(q, 0, SL_ERROR, "address of incorrect length");
	goto error;
      badaddr:
	mi_strlog(q, 0, SL_ERROR, "bad address or incorrect format");
	goto error;
      protoshort:
	mi_strlog(q, 0, SL_ERROR, "primitive too short");
	goto error;
      baddata:
	mi_strlog(q, 0, SL_ERROR, "invalid amount of user data");
	goto error;
      wait:
	mi_strlog(q, 0, SL_ERROR, "primitive received in wack state %u", mtp_get_state(mtp));
	qenable(q);
	return (-EAGAIN);
}

/**
 * mtp_pause_ind: - process an MTP_PAUSE_IND primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Translate MTP_PAUSE_IND into N_UDERROR_IND or N_DISCON_IND.
 */
static inline fastcall __unlikely int
mtp_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mtp_chk_state(mtp, NSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		if (mtp_not_state(mtp, (NSF_DATA_XFER | NSF_WCON_RREQ)))
			goto outstate;
		return n_discon_ind(mtp, q, mp, N_PROVIDER, N_MTP_DEST_PROHIBITED, 0, NULL, mp->b_cont);
	case N_CLNS:
		if (mtp_not_state(mtp, NSF_IDLE))
			goto outstate;
		if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
			goto badaddr;
		if (p->mtp_addr_length) {
			if (p->mtp_addr_length < sizeof(*dst))
				goto badaddr;
			dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_offset);
		}
		return n_uderror_ind(mtp, q, mp, dst, mp->b_cont, N_MTP_DEST_PROHIBITED);
	default:
		goto notsupp;
	}
	goto outstate;
      error:
	freemsg(mp);
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "primitive received in incorrect state %u", mtp_get_state(mtp));
	goto error;
      notsupp:
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", (int) mtp->prot.SERV_type);
	goto error;
      badaddr:
	mi_strlog(q, 0, SL_ERROR, "invalid or bad address format");
	goto error;
      protoshort:
	mi_strlog(q, 0, SL_ERROR, "protocol block too short");
	goto error;
      wait:
	mi_strlog(q, 0, SL_ERROR, "primitive received in wack state %u", mtp_get_state(mtp));
	qenable(q);
	return (-EAGAIN);
}

/**
 * mtp_resume_ind: - process an MTP_RESUME_IND primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * For N_CONS, translate the MTP_RESUME_IND into N_CONN_IND or N_CONN_CON (with no data) or discard
 * depending on the state of the connection.  For N_CLNS, translate the MTP_RESUME_IND into
 * N_UDERROR_IND (with no data) (but not really an error).  N_UDERROR_IND is really an out-of-band
 * indication of the status of a destination or a user or service at a destination anyway.
 */
static inline fastcall __unlikely int
mtp_resume_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_resume_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *add = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto badaddr;
	if (p->mtp_addr_length) {
		if (p->mtp_addr_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->mtp_addr_offset);
	}
	if (mtp_chk_state(mtp, NSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		switch (mtp_get_state(mtp)) {
		case NS_WCON_CREQ:
			return n_conn_con(mtp, q, mp, 0, add, NULL, mp->b_cont);
		case NS_IDLE:
		case NS_WRES_CIND:
			if (mtp->coninds > 0)
				/* FIXME: need a sequence number */
				return n_conn_ind(mtp, q, mp, 0, 0, add, NULL, NULL, mp->b_cont);
		default:
			goto outstate;
		}
		break;
	case N_CLNS:
		if (mtp_get_state(mtp) != NS_IDLE)
			goto outstate;
		return n_uderror_ind(mtp, q, mp, add, mp->b_cont, N_MTP_DEST_AVAILABLE);
	default:
		goto notsupp;
	}
	goto outstate;
      error:
	freemsg(mp);
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "primitive received in incorrect state %u", mtp_get_state(mtp));
	goto error;
      notsupp:
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", (int) mtp->prot.SERV_type);
	goto error;
      badaddr:
	mi_strlog(q, 0, SL_ERROR, "invalid or bad address format");
	goto error;
      protoshort:
	mi_strlog(q, 0, SL_ERROR, "protocol block too short");
	goto error;
      wait:
	mi_strlog(q, 0, SL_ERROR, "primitive received in wack state %u", mtp_get_state(mtp));
	qenable(q);
	return (-EAGAIN);
}

/**
 * mtp_status_ind: - process MTP_STATUS_IND primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Translate MTP_STATUS_IND into N_UDERROR_IND or N_RESET_IND or N_DISCON_IND.
 */
static inline fastcall __unlikely int
mtp_status_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst = NULL;
	mtp_ulong type;
	mtp_ulong status;
	mtp_ulong error;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	type = p->mtp_type;
	status = p->mtp_status;
	switch (type) {
	case MTP_STATUS_TYPE_UPU:
		switch (status) {
		case MTP_STATUS_UPU_UNKNOWN:
			error = N_MTP_USER_PART_UNKNOWN;
			break;
		case MTP_STATUS_UPU_UNEQUIPPED:
			error = N_MTP_USER_PART_UNEQUIPPED;
			break;
		case MTP_STATUS_UPU_INACCESSIBLE:
			error = N_MTP_USER_PART_UNAVAILABLE;
			break;
		default:
			goto badstatus;
		}
		break;
	case MTP_STATUS_TYPE_CONG:
		switch (status) {
		case MTP_STATUS_CONGESTION_LEVEL0:
			error = N_MTP_DEST_CONGESTED(0);
			break;
		case MTP_STATUS_CONGESTION_LEVEL1:
			error = N_MTP_DEST_CONGESTED(1);
			break;
		case MTP_STATUS_CONGESTION_LEVEL2:
			error = N_MTP_DEST_CONGESTED(2);
			break;
		case MTP_STATUS_CONGESTION_LEVEL3:
			error = N_MTP_DEST_CONGESTED(3);
			break;
		case MTP_STATUS_CONGESTION:
			error = N_MTP_DEST_CONGESTED(4);
			break;
		default:
			goto badstatus;
		}
		break;
	default:
		goto badtype;
	}
	if (mtp_chk_state(mtp, NSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		if (mtp_not_state(mtp, (NSF_DATA_XFER | NSF_WRES_RIND)))
			goto outstate;
		switch (type) {
		case MTP_STATUS_TYPE_UPU:
			return n_discon_ind(mtp, q, mp, N_USER, error, 0, NULL, mp->b_cont);
		case MTP_STATUS_TYPE_CONG:
			return n_reset_ind(mtp, q, mp, N_PROVIDER, error);
		}
		goto badstatus;
	case N_CLNS:
		if (mtp_get_state(mtp) != NS_IDLE)
			goto outstate;
		dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_length);
		return n_uderror_ind(mtp, q, mp, dst, mp->b_cont, error);
	default:
		goto notsupp;
	}
      error:
	freemsg(mp);
	return (0);
      badstatus:
	mi_strlog(q, 0, SL_ERROR, "bad status value from below");
	goto error;
      badtype:
	mi_strlog(q, 0, SL_ERROR, "bad status type from below");
	goto error;
      outstate:
	mi_strlog(q, 0, SL_ERROR, "primitive received in incorrect state %u", mtp_get_state(mtp));
	goto error;
      notsupp:
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", (int) mtp->prot.SERV_type);
	goto error;
      protoshort:
	mi_strlog(q, 0, SL_ERROR, "protocol block too short");
	goto error;
      wait:
	mi_strlog(q, 0, SL_ERROR, "primitive received in wack state %u", mtp_get_state(mtp));
	qenable(q);
	return (-EAGAIN);
}

/**
 * mtp_restart_begins_ind: - process MTP_RESTART_BEGINS_IND primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Translate MTP_RESTART_BEGINS_IND into N_UDERROR_IND or N_DISCON_IND.
 */
static inline fastcall __unlikely int
mtp_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	mtp_ulong error = N_MTP_RESTARTING;
	struct mtp_addr *dst = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mtp_chk_state(mtp, NSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		switch (mtp_get_state(mtp)) {
		case NS_WCON_RREQ:
		case NS_WRES_RIND:
		case NS_WCON_CREQ:
		case NS_DATA_XFER:
			return n_discon_ind(mtp, q, mp, N_PROVIDER, error, 0, NULL, mp->b_cont);
		default:
			goto outstate;
		}
		break;
	case N_CLNS:
		switch (mtp_get_state(mtp)) {
		case NS_UNBND:
			goto outstate;
		case NS_IDLE:
			return n_uderror_ind(mtp, q, mp, dst, mp->b_cont, error);
		default:
			goto outstate;
		}
		break;
	default:
		goto notsupp;
	}
	goto outstate;
      error:
	freemsg(mp);
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "primitive received in incorrect state %u", mtp_get_state(mtp));
	goto error;
      notsupp:
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", (int) mtp->prot.SERV_type);
	goto error;
      protoshort:
	mi_strlog(q, 0, SL_ERROR, "protocol block too short");
	goto error;
      wait:
	mi_strlog(q, 0, SL_ERROR, "primitive received in wack state %u", mtp_get_state(mtp));
	qenable(q);
	return (-EAGAIN);
}

/**
 * mtp_restart_complete_ind: - process MTP_RESTART_COMPLETE_IND
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * Translate MTP_RESTART_COMPLETE_IND.
 */
static inline fastcall __unlikely int
mtp_restart_complete_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static inline fastcall __unlikely int
mtp_other_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "unknown MTP primitive %d", *(uint *) mp->b_rptr);
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
#if 0
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;

	switch (type) {
	case __SID:
		switch (nr) {
		case _IOC_NR(I_STR):
		case _IOC_NR(I_LINK):
		case _IOC_NR(I_PLINK):
		case _IOC_NR(I_UNLINK):
		case _IOC_NR(I_PUNLINK):
			(void) lp;
			mi_strlog(q, 0, SL_ERROR, "Unsupported STREAMS ioctl %d", nr);
			ret = -EINVAL;
			break;
		default:
			mi_strlog(q, 0, SL_ERROR, "Unsupported STREAMS ioctl %d", nr);
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	case MTP_IOC_MAGIC:
		ret = (QR_PASSALONG);
		break;
	default:
		ret = (QR_PASSALONG);
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		DB_TYPE(mp) = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		DB_TYPE(mp) = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from NPI to MTP.
 *  -----------------------------------
 */
static noinline fastcall int
mtp_w_proto_slow(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	mtp_ulong oldstate;
	int err;

	if (!(mtp = (struct mtp *) mi_trylock(q)))
		return (-EAGAIN);
	oldstate = mtp_get_state(mtp);
	switch (*(mtp_ulong *) mp->b_rptr) {
	case N_CONN_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_CONN_REQ");
		err = n_conn_req(mtp, q, mp);
		break;
	case N_CONN_RES:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_CONN_RES");
		err = n_conn_res(mtp, q, mp);
		break;
	case N_DISCON_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_DISCON_REQ");
		err = n_discon_req(mtp, q, mp);
		break;
	case N_DATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_DATA_REQ");
		err = n_data_req(mtp, q, mp);
		break;
	case N_EXDATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_EXDATA_REQ");
		err = n_exdata_req(mtp, q, mp);
		break;
	case N_INFO_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_INFO_REQ");
		err = n_info_req(mtp, q, mp);
		break;
	case N_BIND_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_BIND_REQ");
		err = n_bind_req(mtp, q, mp);
		break;
	case N_UNBIND_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_UNBIND_REQ");
		err = n_unbind_req(mtp, q, mp);
		break;
	case N_UNITDATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_UNITDATA_REQ");
		err = n_unitdata_req(mtp, q, mp);
		break;
	case N_OPTMGMT_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_OPTMGMT_REQ");
		err = n_optmgmt_req(mtp, q, mp);
		break;
	case N_DATACK_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_DATACK_REQ");
		err = n_datack_req(mtp, q, mp);
		break;
	case N_RESET_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_RESET_REQ");
		err = n_reset_req(mtp, q, mp);
		break;
	case N_RESET_RES:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> N_RESET_RES");
		err = n_reset_res(mtp, q, mp);
		break;
	default:
		swerr();
		err = -EOPNOTSUPP;
		break;
	}
	if (err < 0)
		mtp_set_state(mtp, oldstate);
	mi_unlock((caddr_t) mtp);
	return (err);
}

static inline fastcall int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	register uint32_t prim;

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		prim = *(uint32_t *) mp->b_rptr;
		if (prim == N_UNITDATA_REQ) {
		}
		if (prim == N_DATA_REQ) {
		}
		if (prim == N_EXDATA_REQ) {
		}
	}
	return mtp_w_proto_slow(q, mp);
}

/*
 *  Primitives from MTP to NPI.
 *  -----------------------------------
 */
static noinline fastcall int
mtp_r_proto_slow(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	mtp_ulong oldstate;
	int err;

	if (!(mtp = (struct mtp *) mi_trylock(q)))
		return (-EAGAIN);
	oldstate = mtp_get_state(mtp);
	switch (*(mtp_ulong *) mp->b_rptr) {
	case MTP_OK_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_OK_ACK <-");
		err = mtp_ok_ack(mtp, q, mp);
		break;
	case MTP_ERROR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_ERROR_ACK <-");
		err = mtp_error_ack(mtp, q, mp);
		break;
	case MTP_BIND_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_BIND_ACK <-");
		err = mtp_bind_ack(mtp, q, mp);
		break;
	case MTP_ADDR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_ADDR_ACK <-");
		err = mtp_addr_ack(mtp, q, mp);
		break;
	case MTP_INFO_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_INFO_ACK <-");
		err = mtp_info_ack(mtp, q, mp);
		break;
	case MTP_OPTMGMT_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_OPTMGMT_ACK <-");
		err = mtp_optmgmt_ack(mtp, q, mp);
		break;
	case MTP_TRANSFER_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_TRANSFER_IND <-");
		err = mtp_transfer_ind(mtp, q, mp);
		break;
	case MTP_PAUSE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_PAUSE_IND <-");
		err = mtp_pause_ind(mtp, q, mp);
		break;
	case MTP_RESUME_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_RESUME_IND <-");
		err = mtp_resume_ind(mtp, q, mp);
		break;
	case MTP_STATUS_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_STATUS_IND <-");
		err = mtp_status_ind(mtp, q, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_RESTART_BEGINS_IND <-");
		err = mtp_restart_begins_ind(mtp, q, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_RESTART_COMPLETE_IND <-");
		err = mtp_restart_complete_ind(mtp, q, mp);
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "MTP_????_IND <-");
		err = mtp_other_ind(mtp, q, mp);
		break;
	}
	if (err < 0)
		mtp_set_state(mtp, oldstate);
	mi_unlock((caddr_t) mtp);
	return (err);
}

/**
 * mtp_r_proto: - process M_PROTO message
 * @q: active queue (read queue)
 * @mp: the message
 *
 * This is the fast path for well-formed MTP_TRANSFER_IND primitives.  If there is any anomally with
 * the message, the slow path is taken.  Note that when the fast path is taken, locking and state
 * checking is not performed.
 */
static inline fastcall int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *src = NULL;

	if (unlikely(mp->b_cont == NULL))
		goto go_slow;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	if (unlikely(p->mtp_primitive != MTP_TRANSFER_IND))
		goto go_slow;
	if (unlikely(mtp_chk_state(mtp, NSF_WACK)))
		goto go_slow;
	/* Two choices, either we are connectionless or connection oriented. */
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		if (unlikely(mtp_not_state(mtp, (NSF_WRES_RIND | NSF_WCON_RREQ | NSF_DATA_XFER))))
			goto go_slow;
		return n_data_ind(mtp, q, mp, 0, mp->b_cont);
	case N_CLNS:
		if (unlikely(mtp_not_state(mtp, (NSF_IDLE | NSF_WACK_UREQ))))
			goto go_slow;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length))
			goto go_slow;
		if (p->mtp_srce_length) {
			if (unlikely(p->mtp_srce_length < sizeof(*src)))
				goto go_slow;
			src = (typeof(src)) (mp->b_rptr + p->mtp_srce_offset);
		}
		mp->b_cont->b_band = mp->b_band;
		return n_unitdata_ind(mtp, q, mp, src, &mtp->src, mp->b_cont);
	default:
		break;
	}
      go_slow:
	return mtp_r_proto_slow(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_w_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	/* data from above */
	mi_strlog(q, STRLOGDA, SL_TRACE, "-> M_DATA [%lu]", (ulong) msgdsize(mp));
	return n_data(mtp, q, mp);
}
static int
mtp_r_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	/* data from below */
	mi_strlog(q, STRLOGDA, SL_TRACE, "M_DATA [%lu] <-", (ulong) msgdsize(mp));
	return mtp_data(mtp, q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
static noinline fastcall int
mtp_wmsg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		return (0);
#if 0
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	case M_IOCDATA:
		return mtp_w_iocdata(q, mp);
	case M_CTL:
	case M_PCCTL:
		return mtp_w_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mtp_w_sig(q, mp);
#endif
	default:
		break;
	}
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
mtp_rmsg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mtp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		break;
	default:
		break;
	}
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
mtp_wmsg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mtp_r_data(q, mp);
	case M_PROTO:
		return mtp_r_proto(q, mp);
	}
	return mtp_wmsg_slow(q, mp);
}

static inline fastcall int
mtp_rmsg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
		return mtp_w_proto(q, mp);
	}
	return mtp_rmsg_slow(q, mp);
}

static streamscall __hot_put int
mtp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mtp_wmsg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_out int
mtp_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mtp_wmsg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_get int
mtp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mtp_rmsg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_in int
mtp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mtp_rmsg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/**
 * mtp_qopen: - STREAMS open procedure
 * @q: newly formed queue pair read queue
 * @devp: pointer to device number
 * @oflags: flags to open(2s) call
 * @sflag: STREAMS flag
 * @crp: credentials pointer
 *
 * Note that module pushes can only return ENXIO.
 */
static streamscall int
mtp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct mtp *mtp;
	mblk_t *mp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((mp = allocb(sizeof(mtp_ulong), BPRI_WAITOK)) == NULL ||
	    (mi_open_comm(&mtp_opens, sizeof(*mtp), q, devp, oflags, sflag, crp) != 0)) {
		freemsg(mp);
		return (ENXIO);
	}
	mtp = MTP_PRIV(q);
	bzero(mtp, sizeof(*mtp));
	mtp->rq = RD(q);
	mtp->wq = WR(q);
	mtp->cred = *crp;
	mtp->prot.PRIM_type = N_INFO_ACK;
	mtp->prot.NSDU_size = -1;
	mtp->prot.ENSDU_size = -2;
	mtp->prot.CDATA_size = -2;
	mtp->prot.DDATA_size = -2;
	mtp->prot.ADDR_size = sizeof(struct mtp_addr);
	mtp->prot.OPTIONS_flags = 0;
	mtp->prot.NIDU_size = -1;
	mtp->prot.SERV_type = N_CLNS;
	mtp->prot.CURRENT_state = NS_UNBND;
	mtp->prot.PROVIDER_type = N_SNICFP;
	mtp->prot.NODU_size = 279;
	mtp->prot.NPI_version = N_VERSION_2;
	/* issue immediate information request */
	*(mtp_ulong *) mp->b_wptr = MTP_INFO_REQ;
	mp->b_wptr += sizeof(mtp_ulong);
	qprocson(q);
	putnext(q, mp);
	if ((mtp->prot.CURRENT_state == -1U && qwait_sig(q) == 0) ||
	    (mtp->prot.CURRENT_state != NS_UNBND)) {
		mi_close_comm(&mtp_opens, q);
		return (ENXIO);
	}
	return (0);
}

/**
 * mtp_qclose: - STREAMS close procedure
 * @q: closing queue pair read queue
 * @oflags: flags to open(2s) call
 * @crp: credentials pointer
 */
static streamscall int
mtp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct mtp *mtp = MTP_PRIV(q);

	qprocsoff(q);
	flushq(mtp->rq, FLUSHALL);
	flushq(mtp->wq, FLUSHALL);
	mi_close_comm(&mtp_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_info mtp_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};
static struct module_stat mtp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mtp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct qinit mtp_rinit = {
	.qi_putp = mtp_rput,		/* Read put (msg from below) */
	.qi_srvp = mtp_rsrv,
	.qi_qopen = mtp_qopen,		/* Each open */
	.qi_qclose = mtp_qclose,	/* Last close */
	.qi_minfo = &mtp_minfo,		/* Information */
	.qi_mstat = &mtp_rstat,		/* Statistics */
};
static struct qinit mtp_winit = {
	.qi_putp = mtp_wput,		/* Write put (msg from above) */
	.qi_srvp = mtp_wsrv,
	.qi_minfo = &mtp_minfo,		/* Information */
	.qi_mstat = &mtp_wstat,		/* Statistics */
};
static struct streamtab mtp_npiinfo = {
	.st_rdinit = &mtp_rinit,	/* Upper read queue */
	.st_wrinit = &mtp_winit,	/* Upper write queue */
};

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
MODULE_PARM_DESC(modid, "Module ID for the MTP-NPI module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static struct fmodsw mtp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mtp_npiinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
mtp_npiinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = register_strmod(&mtp_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
mtp_npiterminate(void)
{
	int err;

	if ((err = unregister_strmod(&mtp_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mtp_npiinit);
module_exit(mtp_npiterminate);

#endif				/* LINUX */
