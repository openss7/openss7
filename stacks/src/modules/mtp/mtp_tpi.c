/*****************************************************************************

 @(#) $RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/03 13:35:26 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/08/03 13:35:26 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp_tpi.c,v $
 Revision 0.9.2.26  2007/08/03 13:35:26  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.25  2007/07/14 01:34:42  brian
 - make license explicit, add documentation

 Revision 0.9.2.24  2007/03/25 18:59:49  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.23  2007/03/25 02:22:52  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.22  2007/02/26 07:25:40  brian
 - synchronizing changes

 Revision 0.9.2.21  2007/02/17 02:49:17  brian
 - first clean recompile of MTP modules on LFS

 Revision 0.9.2.20  2007/02/14 14:09:12  brian
 - broad changes updating support for SS7 MTP and M3UA

 Revision 0.9.2.19  2007/02/13 07:55:43  brian
 - working up MTP and UAs

 Revision 0.9.2.18  2006/05/08 11:01:01  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.17  2006/03/07 01:10:50  brian
 - binary compatible callouts

 Revision 0.9.2.16  2006/03/04 13:00:16  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/03 13:35:26 $"

static char const ident[] =
    "$RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/03 13:35:26 $";

/*
 *  This is a MTP TPI module which can be pushed over an MTPI (Message Transfer Part Interface)
 *  stream to effect a TPI interface to the MTP.  This module is intended to be used by application
 *  programs or by upper modules that expect a TPI connectionless service provider.
 */
#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <linux/socket.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>

#define MTP_TPI_DESCRIP		"SS7 Message Transfer Part (MTP) TPI STREAMS MODULE."
#define MTP_TPI_REVISION	"LfS $RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/03 13:35:26 $"
#define MTP_TPI_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_TPI_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MTP_TPI_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MTP_TPI_LICENSE		"GPL v2"
#define MTP_TPI_BANNER		MTP_TPI_DESCRIP		"\n" \
				MTP_TPI_REVISION	"\n" \
				MTP_TPI_COPYRIGHT	"\n" \
				MTP_TPI_DEVICE		"\n" \
				MTP_TPI_CONTACT		"\n"
#define MTP_TPI_SPLASH		MTP_TPI_DESCRIP		"\n" \
				MTP_TPI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(MTP_TPI_CONTACT);
MODULE_DESCRIPTION(MTP_TPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP_TPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP_TPI_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mtp_tpi");
#endif
#endif				/* LINUX */

#ifdef LFS
#define MTP_TPI_MOD_ID		CONFIG_STREAMS_MTP_TPI_MODID
#define MTP_TPI_MOD_NAME	CONFIG_STREAMS_MTP_TPI_NAME
#endif

#define MOD_ID		MTP_TPI_MOD_ID
#define MOD_NAME	MTP_TPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MTP_TPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	MTP_TPI_SPLASH
#endif				/* MODULE */

/*
 *  =========================================================================
 *
 *  MTP-TPI Private Structure
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
	struct T_info_ack prot;
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
t_addr_size(struct mtp_addr *add)
{
	size_t len = 0;

	if (add) {
		len = sizeof(struct sockaddr_mtp);
	}
	return (len);
}
static inline fastcall void
t_build_addr(struct mtp_addr *add, unsigned char *p)
{
	if (add) {
		struct sockaddr_mtp *sa = (typeof(sa)) p;

		sa->mtp_family = 0;
		sa->mtp_ni = add->ni;
		sa->mtp_si = add->si;
		sa->mtp_pc = add->pc;
	}
}

/*
 *  =========================================================================
 *
 *  OPTION Handling
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

#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
static inline fastcall size_t
t_opts_size(struct mtp_opts *ops)
{
	size_t len = 0;

	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */

		if (ops->sls)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
		if (ops->mp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
		if (ops->debug)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->debug));
	}
	return (len);
}
static inline fastcall void
t_build_opts(struct mtp_opts *ops, unsigned char *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);

		if (ops->sls) {
			oh = (typeof(oh)) p++;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status = (ops->flags & (1 << T_MTP_SLS)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = (typeof(oh)) p++;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status = (ops->flags & (1 << T_MTP_MP)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
		if (ops->debug) {
			oh = (typeof(oh)) p++;
			oh->len = hlen + sizeof(*(ops->debug));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_DEBUG;
			oh->status = (ops->flags & (1 << T_MTP_DEBUG)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->debug)) p) = *(ops->debug);
			p += _T_ALIGN_SIZEOF(*ops->debug);
		}
	}
}
static int
t_parse_opts(struct mtp_opts *ops, unsigned char *op, size_t len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_MTP:
			switch (oh->name) {
			case T_MTP_SLS:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_MTP_SLS);
				continue;
			case T_MTP_MP:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_MTP_MP);
				continue;
			case T_MTP_DEBUG:
				ops->debug = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_MTP_DEBUG);
				continue;
			}
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

#if 0
static int
t_parse_qos(struct mtp_opts *ops, unsigned char *op, size_t len)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
#endif

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
static int
mtp_opt_check(struct mtp *mtp, struct mtp_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->sls)
			ops->flags |= (1 << T_MTP_SLS);
		if (ops->mp)
			ops->flags |= (1 << T_MTP_MP);
		if (ops->debug)
			ops->flags |= (1 << T_MTP_DEBUG);
	}
	return (0);
}
static int
mtp_opt_default(struct mtp *mtp, struct mtp_opts *ops)
{
	if (ops) {
		int flags = ops->flags;

		ops->flags = 0;
		if (!flags || ops->sls) {
			ops->sls = &opt_defaults.sls;
			ops->flags |= (1 << T_MTP_SLS);
		}
		if (!flags || ops->mp) {
			ops->mp = &opt_defaults.mp;
			ops->flags |= (1 << T_MTP_MP);
		}
		if (!flags || ops->debug) {
			ops->debug = &opt_defaults.debug;
			ops->flags |= (1 << T_MTP_DEBUG);
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
static int
mtp_opt_current(struct mtp *mtp, struct mtp_opts *ops)
{
	int flags = ops->flags;

	ops->flags = 0;
	if (!flags || ops->sls) {
		ops->sls = &mtp->options.sls;
		ops->flags |= (1 << T_MTP_SLS);
	}
	if (!flags || ops->mp) {
		ops->mp = &mtp->options.mp;
		ops->flags |= (1 << T_MTP_MP);
	}
	if (!flags || ops->debug) {
		ops->debug = &mtp->options.debug;
		ops->flags |= (1 << T_MTP_DEBUG);
	}
	return (0);
}
static int
mtp_opt_negotiate(struct mtp *mtp, struct mtp_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->sls) {
			mtp->options.sls = *ops->sls;
			ops->sls = &mtp->options.sls;
			ops->flags |= (1 << T_MTP_SLS);
		}
		if (ops->mp) {
			mtp->options.mp = *ops->mp;
			ops->mp = &mtp->options.mp;
			ops->flags |= (1 << T_MTP_MP);
		}
		if (ops->debug) {
			mtp->options.debug = *ops->debug;
			ops->debug = &mtp->options.debug;
			ops->flags |= (1 << T_MTP_DEBUG);
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */
static const char *
mtp_state(mtp_ulong state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	case TS_NOSTATES:
		return ("TS_UNINIT");
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

#ifndef TS_NOSTATES
#define TS_NOSTATES (TS_WACK_DREQ11 + 1)
#endif
/*
 *  TLI interface state flags
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)

#define TSF_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)
#define TSF_WACK	(TSF_WACK_BREQ \
			|TSF_WACK_UREQ \
			|TSF_WACK_OPTREQ \
			|TSF_WACK_CREQ \
			|TSF_WACK_CRES \
			|TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)

static void
mtp_bind(struct mtp *mtp, struct mtp_addr *src)
{
	if (src) {
		mtp->src = *src;
		mtp->src_len = sizeof(*src);
	}
	return;
}
static void
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
		mtp_set_state(mtp, TS_NOSTATES);
		freemsg(msg);
		mi_strlog(mtp->rq, STRLOGRX, SL_TRACE, "<- M_ERROR");
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
t_conn_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	   t_scalar_t seq, struct mtp_addr *src, struct mtp_opts *opt, mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;
	size_t src_len = t_addr_size(src);
	size_t opt_len = t_opts_size(opt);
	size_t msg_len = sizeof(*p) + src_len + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + src_len;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			t_build_addr(src, mp->b_wptr);
			mp->b_wptr += src_len;
			t_build_opts(opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_CONN_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_con: - issue a T_CONN_CON primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @res: responding address
 * @opt: options
 * @dp: user data
 */
static inline fastcall __unlikely int
t_conn_con(struct mtp *mtp, queue_t *q, mblk_t *bp,
	   struct mtp_addr *res, struct mtp_opts *opt, mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;
	size_t res_len = t_addr_size(res);
	size_t opt_len = t_opts_size(opt);
	size_t msg_len = sizeof(*p) + res_len + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->rq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + opt_len;
			mp->b_wptr += sizeof(*p);
			t_build_addr(res, mp->b_wptr);
			mp->b_wptr += res_len;
			t_build_opts(opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_CONN_CON");
			if (mtp_get_state(mtp) != TS_WCON_CREQ)
				swerr();
			mtp_set_state(mtp, TS_DATA_XFER);
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue a T_DISCON_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @reason: reason for disconnection
 * @dp: user data
 */
static inline fastcall __unlikely int
t_discon_ind(struct mtp *mtp, queue_t *q, mblk_t *bp, t_scalar_t reason, mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (mtp_chk_state(mtp, (TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL))) {
		if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
			if (likely(canputnext(mtp->rq))) {
				DB_TYPE(mp) = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = T_DISCON_IND;
				p->DISCON_reason = reason;
				p->SEQ_number = 0;
				mp->b_wptr += sizeof(*p);
				mtp_set_state(mtp, TS_IDLE);
				mp->b_cont = dp;
				if (bp)
					freeb(bp);
				mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_DISCON_IND");
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
 * t_data_ind: - issue a T_DATA_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @more: more data flag
 * @dp: user data
 */
static inline fastcall __unlikely int
t_data_ind(struct mtp *mtp, queue_t *q, mblk_t *bp, t_scalar_t more, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	if (mtp_chk_state(mtp, (TSF_DATA_XFER | TSF_WIND_ORDREL))) {
		if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
			if (likely(canputnext(mtp->rq))) {
				DB_TYPE(mp) = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = T_DATA_IND;
				p->MORE_flag = more;
				mp->b_wptr += sizeof(*p);
				mp->b_cont = dp;
				if (bp)
					freeb(bp);
				mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_DATA_IND");
				putnext(mtp->rq, mp);
				return (0);
			}
			freeb(mp);
			return (-EBUSY);
		}
		return (-ENOBUFS);
	}
	mi_strlog(q, 0, SL_ERROR, "unexpected indication for state %u", mtp_get_state(mtp));
	freemsg(mp);
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
t_exdata_ind(struct mtp *mtp, queue_t *q, mblk_t *bp, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;

	if (mtp_chk_state(mtp, (TSF_DATA_XFER | TSF_WIND_ORDREL))) {
		if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
			if (likely(canputnext(mtp->rq))) {
				DB_TYPE(mp) = M_PROTO;
				mp->b_band = 1;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = T_EXDATA_IND;
				p->MORE_flag = more;
				mp->b_wptr += sizeof(*p);
				mp->b_cont = dp;
				if (bp)
					freeb(bp);
				mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_EXDATA_IND");
				putnext(mtp->rq, mp);
				return (0);
			}
			freeb(mp);
			return (-EBUSY);
		}
		return (-ENOBUFS);
	}
	mi_strlog(q, 0, SL_ERROR, "unexpected indication for state %u", mtp_get_state(mtp));
	freemsg(mp);
	return (0);
}

/**
 * t_info_ack: - issue a T_INFO_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
t_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;


	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		*p = mtp->prot;
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * t_bind_ack: - issue a T_BIND_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: bound address
 * @cons: negotiated number of oustanding connection indications
 */
static inline fastcall __unlikely int
t_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *add, t_scalar_t cons)
{
	struct T_bind_ack *p;
	mblk_t *mp;
	size_t add_len = t_addr_size(add);
	size_t msg_len = sizeof(*p) + add_len;

	if (mtp_get_state(mtp) == TS_WACK_BREQ) {
		if (unlikely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
			DB_TYPE(mp) = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_BIND_ACK;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->CONIND_number = cons;
			mp->b_wptr += sizeof(*p);
			t_build_addr(add, mp->b_wptr);
			mp->b_wptr += add_len;
			mtp_bind(mtp, add);
			mtp_set_state(mtp, TS_IDLE);
			freemsg(msg);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_BIND_ACK");
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
 * t_error_ack: - issue a T_ERROR_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @etypr: error type
 */
static inline fastcall __unlikely int
t_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, const t_scalar_t prim, t_scalar_t etype)
{
	struct T_error_ack *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = etype < 0 ? TSYSERR : etype;
		p->UNIX_error = etype < 0 ? -etype : 0;
		mp->b_wptr += sizeof(*p);
		switch (mtp_get_state(mtp)) {
#ifdef TS_WACK_OPTREQ
		case TS_WACK_OPTREQ:
#endif
		case TS_WACK_UREQ:
		case TS_WACK_CREQ:
			mtp_set_state(mtp, TS_IDLE);
			break;
		case TS_WACK_BREQ:
			mtp_set_state(mtp, TS_UNBND);
			break;
		case TS_WACK_CRES:
			mtp_set_state(mtp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ6:
			mtp_set_state(mtp, TS_WCON_CREQ);
			break;
		case TS_WACK_DREQ7:
			mtp_set_state(mtp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			mtp_set_state(mtp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			mtp_set_state(mtp, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			mtp_set_state(mtp, TS_WREQ_ORDREL);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we send TOUTSTATE or TNOTSUPPORT or are responding
			   to a T_OPTMGMT_REQ in other than TS_IDLE state. */
			break;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_ERROR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_ok_ack: - issue a T_OK_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 * @seq: sequence number
 * @tok: stream token
 */
static inline fastcall __unlikely int
t_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t seq, t_scalar_t tok)
{
	struct T_ok_ack *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (mtp_get_state(mtp)) {
		case TS_WACK_CREQ:
			mtp_connect(mtp, &mtp->dst);
			mtp_set_state(mtp, TS_WCON_CREQ);
			break;
		case TS_WACK_UREQ:
			mtp_unbind(mtp);
			mtp_set_state(mtp, TS_UNBND);
			break;
		case TS_WACK_OPTREQ:
			mtp_set_state(mtp, TS_IDLE);
			break;
		case TS_WACK_CRES:
			/* FIXME: need to use sequence and token */
			mtp_set_state(mtp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
			/* FIXME: need to handle sequence numbers */
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			mtp_disconnect(mtp);
			mtp_set_state(mtp, TS_IDLE);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   the TS_IDLE state. */
			break;
		}
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_unitdata_ind: - issue a T_UNITDATA_IND primitive
 * @mtp: MTP private data
 * @q: active queue
 * @bp: message block to free upon success
 * @src: source address (or NULL)
 * @opt: options (or NULL)
 * @dp: user data
 */
static inline fastcall __unlikely int
t_unitdata_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	       struct mtp_addr *src, struct mtp_opts *opt, mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;
	size_t src_len = t_addr_size(src);
	size_t opt_len = t_opts_size(opt);
	size_t msg_len = sizeof(*p) + src_len + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->rq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + src_len;
			mp->b_wptr += sizeof(*p);
			t_build_addr(src, mp->b_wptr);
			mp->b_wptr += src_len;
			t_build_opts(opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_UNITDATA_IND");
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
t_uderror_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	      struct mtp_addr *dst, struct mtp_opts *opt, mblk_t *dp, t_scalar_t etype)
{
	struct T_uderror_ind *p;
	mblk_t *mp;
	size_t dst_len = t_addr_size(dst);
	size_t opt_len = t_opts_size(opt);
	size_t msg_len = sizeof(*p) + dst_len + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->rq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + dst_len;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			t_build_addr(dst, mp->b_wptr);
			mp->b_wptr += dst_len;
			t_build_opts(opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_UDERROR_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optmgmt_ack: - issue a T_OPTMGMT_ACK primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 * @flags: management flags
 * @opt: options
 */
static inline fastcall __unlikely int
t_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, t_scalar_t flags, struct mtp_opts *opt)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;
	size_t opt_len = t_opts_size(opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = sizeof(*p);
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		t_build_opts(opt, mp->b_wptr);
		mp->b_wptr += opt_len;
#ifdef TS_WACK_OPTREQ
		if (mtp_get_state(mtp) == TS_WACK_OPTREQ)
			mtp_set_state(mtp, TS_IDLE);
#endif
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_OPTMGMT_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue a T_ORDREL_IND primitive
 * @mtp: MTP private data
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall __unlikely int
t_ordrel_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	mi_strlog(q, 0, SL_ERROR, "unsupported primitive");
	freemsg(msg);
	return (0);
}

/**
 * t_optdata_ind: - issue a T_OPTDATA_IND primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @bp: message block to free upon success
 * @flags: data flags
 * @opt: options (or NULL)
 * @dp: user data
 */
static inline fastcall __unlikely int
t_optdata_ind(struct mtp *mtp, queue_t *q, mblk_t *bp,
	      t_scalar_t flags, struct mtp_opts *opt, mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	size_t opt_len = t_opts_size(opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->rq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			t_build_opts(opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (bp)
				freeb(bp);
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_OPTDATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/**
 * t_addr_ack: - issue a T_ADDR_ACK primtive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @loc: local address (or NULL)
 * @rem: remote address (or NULL)
 */
static inline fastcall __unlikely int
t_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *loc, struct mtp_addr *rem)
{
	struct T_addr_ack *p;
	mblk_t *mp;
	size_t loc_len = t_addr_size(loc);
	size_t rem_len = t_addr_size(rem);
	size_t msg_len = sizeof(*p) + loc_len + rem_len;

	if (likely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = sizeof(*p);
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = sizeof(*p) + loc_len;
		mp->b_wptr += sizeof(*p);
		t_build_addr(loc, mp->b_wptr);
		mp->b_wptr += loc_len;
		t_build_addr(rem, mp->b_wptr);
		mp->b_wptr += rem_len;
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_ADDR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#ifdef T_CAPABILITY_ACK
/**
 * t_capability_ack: - issue a T_CAPABILITY_ACK upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @caps: capability bits
 */
static inline fastcall __unlikely int
t_capability_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, t_scalar_t caps)
{
	struct T_capability_ack *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p);

	if (unlikely((mp = mi_allocb(q, msg_len, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = TC1_INFO;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_scalar_t) (ulong) mtp->rq : 0;
		mp->b_wptr += sizeof(*p);
		if (caps & TC1_INFO)
			p->INFO_ack = mtp->prot;
		else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- T_CAPABILITY_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * t_reset_ind: issue a T_RESET_IND upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reason for reset
 */
static inline fastcall __unlikely int
t_reset_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, t_scalar_t reason)
{
	mi_strlog(q, 0, SL_ERROR, "fix congestion resets");
	freemsg(msg);
	return (0);
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
mtp_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_opts *opt, mtp_ulong flags)
{
	struct MTP_optmgmt_req *p;
	mblk_t *mp;
	size_t opt_len = t_opts_size(opt);
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
			t_build_opts(opt, mp->b_wptr);
			mp->b_wptr += opt_len;
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
 * t_data: - process M_DATA message
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 *
 * To support pseudo-connectionless modes, when this message is sent for T_CLTS we should send the
 * data to the same adress and with the same options as the last T_UNITDATA_REQ primitive.
 */
static inline fastcall __hot_read int
t_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const size_t dlen = msgdsize(mp);

	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mtp_not_state(mtp, (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot.TSDU_size || dlen > mtp->prot.TIDU_size)
		goto baddata;
	return mtp_transfer_req(mtp, q, NULL, &mtp->dst, mtp->options.mp, mtp->options.sls, mp);
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
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * t_conn_req: - process T_CONN_REQ primitive
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
t_conn_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err = -EFAULT;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	struct mtp_opts opts = { 0L, NULL, };

	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
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
		goto acces;
	if (dst->si != mtp->src.si && mtp->src.si != 0)
		goto badaddr;
	if (t_parse_opts(&opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
		goto badopt;
	/* TODO: set options first */
	if (mp->b_cont) {
		putbq(q, mp->b_cont);	/* hold back data */
		mp->b_cont = NULL;	/* abosrbed mp->b_cont */
	}
	mtp->dst = *dst;
	mtp_set_state(mtp, TS_WACK_CREQ);
	return mtp_conn_req(mtp, q, mp, dst, 0, NULL);
      badopt:
	err = TBADOPT;
	mi_strlog(q, 0, SL_TRACE, "bad options");
	goto error;
      acces:
	err = TACCES;
	mi_strlog(q, 0, SL_TRACE, "no permission for address");
	goto error;
      badaddr:
	err = TBADADDR;
	mi_strlog(q, 0, SL_TRACE, "address is unusable");
	goto error;
      noaddr:
	err = TNOADDR;
	mi_strlog(q, 0, SL_TRACE, "couldn't allocate address");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_conn_res: - process T_CONN_RES primtive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_conn_res(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;

	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) != TS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* We never give an T_CONN_IND, so there is no reason for a T_CONN_RES.  We probably could
	   do this (issue an T_CONN_IND on a listening stream when there is no other MTP user for
	   the SI value and * send a UPU on an N_DISCON_REQ or just redirect all traffic for that
	   user on a T_CONN_RES) but that is for later. */
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
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_discon_req: - process T_DISCON_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_discon_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const struct T_discon_req *p = (typeof(p)) mp->b_rptr;

	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* Currently there are only three states we can disconnect from.  The first does not
	   happen. Only the second one is normal.  The third should occur during simulteneous
	   diconnect only. */
	switch (mtp_get_state(mtp)) {
	case TS_WCON_CREQ:
		mtp_set_state(mtp, TS_WACK_DREQ6);
		break;
	case TS_DATA_XFER:
		mtp_set_state(mtp, TS_WACK_DREQ9);
		break;
	case TS_IDLE:
		rare();
		break;
	default:
		goto outstate;
	}
	/* change state and let mtp_ok_ack do all the work */
	return mtp_discon_req(mtp, q, mp, mp->b_cont);
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_data_req: - process T_DATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_data_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;

	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (!mp->b_cont)
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot.TSDU_size || dlen > mtp->prot.TIDU_size)
		goto baddata;
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
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * t_exdata_req: - process T_EXDATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_exdata_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	(void) mp;
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * t_info_req: - process T_INFO_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	return mtp_info_req(mtp, q, mp);
}

/**
 * t_bind_req:- process T_BIND_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_bind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr src;

	if (mtp_get_state(mtp) != TS_UNBND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->CONIND_number)
		goto notsupport;
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
	mtp_set_state(mtp, TS_WACK_BREQ);
	return mtp_bind_req(mtp, q, mp, &src, 0);
      acces:
	err = TACCES;
	mi_strlog(q, 0, SL_TRACE, "no permission for address");
	goto error;
      noaddr:
	err = TNOADDR;
	mi_strlog(q, 0, SL_TRACE, "couldn't allocate address");
	goto error;
      badaddr:
	err = TBADADDR;
	mi_strlog(q, 0, SL_TRACE, "address is invalid");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not support for T_CLTS");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_unbind_req: - process T_UNBIND_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	mtp_set_state(mtp, TS_WACK_UREQ);
	return mtp_unbind_req(mtp, q, mp);
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_unitdata_req: - process T_UNITDATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_unitdata_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	struct mtp_addr dst;
	struct mtp_opts opts = { 0L, NULL, };

	if (mtp->prot.SERV_type != T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (dlen == 0 && !(mtp->prot.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (dlen > mtp->prot.TSDU_size || dlen > mtp->prot.TIDU_size)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
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
	if (t_parse_opts(&opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
		goto badopt;
	fixme(("Handle options correctly\n"));
	return mtp_transfer_req(mtp, q, mp, &dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
      badopt:
	mi_strlog(q, 0, SL_TRACE, "bad options");
	goto error;
      acces:
	mi_strlog(q, 0, SL_TRACE, "no permission to address");
	goto error;
      badaddr:
	mi_strlog(q, 0, SL_TRACE, "bad destination address");
	goto error;
      noaddr:
	mi_strlog(q, 0, SL_TRACE, "could not allocate address");
	goto error;
      baddata:
	mi_strlog(q, 0, SL_TRACE, "bad data size %lu", (ulong) dlen);
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      badprim:
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_COTS or T_COTS_ORD");
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
t_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err = 0;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_opts opts = { 0L, NULL, };

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badprim;
#ifdef TS_WACK_OPTREQ
	if (mtp_get_state(mtp) == TS_IDLE)
		mtp_set_state(mtp, TS_WACK_OPTREQ);
#endif
	if (t_parse_opts(&opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
		goto badopt;
	switch (p->MGMT_flags) {
	case T_CHECK:
		err = mtp_opt_check(mtp, &opts);
		break;
	case T_NEGOTIATE:
		if (!opts.flags)
			mtp_opt_default(mtp, &opts);
		else if ((err = mtp_opt_check(mtp, &opts)))
			break;
		err = mtp_opt_negotiate(mtp, &opts);
		break;
	case T_DEFAULT:
		err = mtp_opt_default(mtp, &opts);
		break;
	case T_CURRENT:
		err = mtp_opt_current(mtp, &opts);
		break;
	default:
		goto badflag;
	}
	if (err)
		goto provspec;
	return mtp_optmgmt_req(mtp, q, mp, &opts, p->MGMT_flags);
      provspec:
	err = err;
	mi_strlog(q, 0, SL_TRACE, "provider specific");
	goto error;
      badflag:
	err = TBADFLAG;
	mi_strlog(q, 0, SL_TRACE, "bad options flags");
	goto error;
      badopt:
	err = TBADOPT;
	mi_strlog(q, 0, SL_TRACE, "bad options");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

/**
 * t_ordrel_req: - process T_ORDREL_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_ordrel_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if (mtp->prot.SERV_type != T_COTS_ORD)
		goto notsupport;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	switch (mtp_get_state(mtp)) {
	case TS_DATA_XFER:
		mtp_set_state(mtp, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		goto error;
	}
	return t_error_ack(mtp, q, mp, T_ORDREL_REQ, TNOTSUPPORT);
      outstate:
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS or T_COTS");
	goto error;
      error:
	return m_error(mtp, q, mp, EPROTO);
}

/**
 * t_optdata_req: - process T_OPTDATA_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_optdata_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_opts opts = { 0L, NULL, };

	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (p->DATA_flag & T_ODF_EX || p->DATA_flag & T_ODF_MORE)
		goto notsupport;
	if (t_parse_opts(&opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
		goto badopt;
	fixme(("Handle options correctly\n"));
	return mtp_transfer_req(mtp, q, mp, &mtp->dst, mtp->options.mp, mtp->options.sls,
				mp->b_cont);
      badopt:
	err = TBADOPT;
	mi_strlog(q, 0, SL_TRACE, "bad options");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "would place i/f out of state");
	goto error;
      einval:
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	return m_error(mtp, q, mp, EPROTO);
      discard:
	mi_strlog(q, 0, SL_TRACE, "ignore in idle state");
	return (0);
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(mtp, q, mp, p->PRIM_type, err);
}

#ifdef T_ADDR_REQ
/**
 * t_addr_req: - process T_ADDR_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_addr_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const struct T_addr_req *p = (typeof(p)) mp->b_rptr;

	(void) mp;
	return mtp_addr_req(mtp, q, mp);
}
#endif

#ifdef T_CAPABILITY_REQ
/**
 * t_capability_req: - process T_CAPABILITY_REQ primitive
 * @mtp: private structure
 * @q: active queue
 * @mp: the message
 */
static inline fastcall __unlikely int
t_capability_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	const struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(mtp, q, mp, p->CAP_bits1);
      einval:
	mi_strlog(q, 0, SL_TRACE, "invalid primitive format");
	return t_error_ack(mtp, q, mp, p->PRIM_type, -EINVAL);
}
#endif

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
	if (mtp->prot.SERV_type == T_COTS)
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
 * Simply translate the MTP_OK_ACK into a T_OK_ACK.
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
		prim = T_BIND_REQ;
		break;
	case MTP_UNBIND_REQ:
		prim = T_UNBIND_REQ;
		break;
	case MTP_CONN_REQ:
		prim = T_CONN_REQ;
		if ((err = t_ok_ack(mtp, q, NULL, prim, 0, 0)) < 0)
			return (err);
		return t_conn_con(mtp, q, mp, NULL, NULL, NULL);
	case MTP_DISCON_REQ:
		prim = T_DISCON_REQ;
		break;
	case MTP_ADDR_REQ:
		swerr();
		prim = T_ADDR_REQ;
		break;
	case MTP_INFO_REQ:
		swerr();
		prim = T_INFO_REQ;
		break;
	case MTP_OPTMGMT_REQ:
		swerr();
		prim = T_OPTMGMT_REQ;
		break;
	case MTP_TRANSFER_REQ:
		swerr();
		prim = T_OPTDATA_REQ;
		break;
	default:
		swerr();
		prim = 0;
		break;
	}
	return t_ok_ack(mtp, q, mp, prim, 0, 0);
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
 * Simply translate the MTP_ERROR_ACK into a T_ERROR_ACK.
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
		prim = T_BIND_REQ;
		break;
	case MTP_UNBIND_REQ:
		prim = T_UNBIND_REQ;
		break;
	case MTP_CONN_REQ:
		prim = T_CONN_REQ;
		break;
	case MTP_DISCON_REQ:
		prim = T_DISCON_REQ;
		break;
	case MTP_ADDR_REQ:
		swerr();
		prim = T_ADDR_REQ;
		break;
	case MTP_INFO_REQ:
		prim = T_INFO_REQ;
		break;
	case MTP_OPTMGMT_REQ:
		prim = T_OPTMGMT_REQ;
		break;
	case MTP_TRANSFER_REQ:
		swerr();
		prim = T_OPTDATA_REQ;
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
		err = TACCES;
		break;
	case MBADADDR:
		err = TBADADDR;
		break;
	case MNOADDR:
		err = TNOADDR;
		break;
	case MBADPRIM:
		err = -EINVAL;
		break;
	case MOUTSTATE:
		err = TOUTSTATE;
		break;
	case MNOTSUPP:
		err = TNOTSUPPORT;
		break;
	case MBADFLAG:
		err = TBADF;
		break;
	case MBADOPT:
		err = TBADOPT;
		break;
	default:
		swerr();
		err = -EFAULT;
		break;
	}
	return t_error_ack(mtp, q, mp, prim, err);
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
	return t_bind_ack(mtp, q, mp, add, 0);
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
	return t_addr_ack(mtp, q, mp, loc, rem);
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
 * current state is TS_NOSTATES.
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
	mtp->prot.TSDU_size = p->mtp_msu_size;
	mtp->prot.TIDU_size = p->mtp_msu_size;
	mtp->prot.ADDR_size = p->mtp_addr_size;
	switch (p->mtp_current_state) {
	case MTPS_UNBND:	/* 0UL */
		mtp->prot.CURRENT_state = TS_UNBND;
		break;
	case MTPS_WACK_BREQ:	/* 1UL */
		mtp->prot.CURRENT_state = TS_WACK_BREQ;
		break;
	case MTPS_IDLE:	/* 2UL */
		mtp->prot.CURRENT_state = TS_IDLE;
		break;
	case MTPS_WACK_CREQ:	/* 3UL */
		mtp->prot.CURRENT_state = TS_WACK_CREQ;
		break;
	case MTPS_WCON_CREQ:	/* 4UL */
		mtp->prot.CURRENT_state = TS_WCON_CREQ;
		break;
	case MTPS_CONNECTED:	/* 5UL */
		mtp->prot.CURRENT_state = TS_DATA_XFER;
		break;
	case MTPS_WACK_UREQ:	/* 6UL */
		mtp->prot.CURRENT_state = TS_WACK_UREQ;
		break;
	case MTPS_WACK_DREQ6:	/* 7UL */
		mtp->prot.CURRENT_state = TS_WACK_DREQ6;
		break;
	case MTPS_WACK_DREQ9:	/* 8UL */
		mtp->prot.CURRENT_state = TS_WACK_DREQ9;
		break;
	case MTPS_WACK_OPTREQ:	/* 9UL */
		mtp->prot.CURRENT_state = TS_WACK_OPTREQ;
		break;
	case MTPS_WREQ_ORDREL:	/* 10UL */
		mtp->prot.CURRENT_state = TS_WREQ_ORDREL;
		break;
	case MTPS_WIND_ORDREL:	/* 11UL */
		mtp->prot.CURRENT_state = TS_WIND_ORDREL;
		break;
	case MTPS_WRES_CIND:	/* 12UL */
		mtp->prot.CURRENT_state = TS_WRES_CIND;
		break;
	case MTPS_UNUSABLE:	/* 0xffffffffUL */
		mtp->prot.CURRENT_state = TS_NOSTATES;
		break;
	default:
		mtp->prot.CURRENT_state = TS_NOSTATES;
		break;
	}
	switch (p->mtp_serv_type) {
	case M_COMS:
		mtp->prot.SERV_type = T_COTS;
		mtp->prot.CDATA_size = p->mtp_msu_size;
		mtp->prot.DDATA_size = p->mtp_msu_size;
		break;
	case M_CLMS:
		mtp->prot.SERV_type = T_CLTS;
		mtp->prot.CDATA_size = T_INVALID;
		mtp->prot.DDATA_size = T_INVALID;
		break;
	default:
		swerr();
		break;
	}
	if (!first)
		return t_info_ack(mtp, q, mp);
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
 * mtp_transfer_ind: - translate MTP_TRANSFER_IND into T_UNITDATA_IND or T_OPTDATA_IND
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
mtp_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *src = NULL;
	struct mtp_opts opts;

	if (mp->b_cont == NULL)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	opts.flags = (1 << T_MTP_SLS) | (1 << T_MTP_MP);
	opts.sls = &p->mtp_sls;
	opts.mp = &p->mtp_mp;
	opts.debug = NULL;
	if (mtp_chk_state(mtp, TSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		if (mtp_not_state(mtp, (TSF_WREQ_ORDREL | TSF_DATA_XFER)))
			goto outstate;
		return t_optdata_ind(mtp, q, mp, 0, &opts, mp->b_cont);
	case T_CLTS:
		if (mtp_not_state(mtp, (TSF_IDLE | TSF_WACK_UREQ)))
			goto outstate;
		if (mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length)
			goto badaddr;
		if (p->mtp_srce_length) {
			if (p->mtp_srce_length < sizeof(*src))
				goto badaddr2;
			src = (typeof(src)) (mp->b_rptr + p->mtp_srce_offset);
		}
		mp->b_cont->b_band = mp->b_band;
		return t_unitdata_ind(mtp, q, mp, src, &opts, mp->b_cont);
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
 * Translate MTP_PAUSE_IND into T_UDERROR_IND or T_DISCON_IND.
 */
static inline fastcall __unlikely int
mtp_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mtp_chk_state(mtp, TSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		if (mtp_not_state(mtp, (TSF_DATA_XFER | TSF_WIND_ORDREL)))
			goto outstate;
		return t_discon_ind(mtp, q, mp, T_MTP_DEST_PROHIBITED, mp->b_cont);
	case T_CLTS:
		if (mtp_not_state(mtp, TSF_IDLE))
			goto outstate;
		if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
			goto badaddr;
		if (p->mtp_addr_length) {
			if (p->mtp_addr_length < sizeof(*dst))
				goto badaddr;
			dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_offset);
		}
		return t_uderror_ind(mtp, q, mp, dst, NULL, mp->b_cont, T_MTP_DEST_PROHIBITED);
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
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", mtp->prot.SERV_type);
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
 * For T_COTS, translate the MTP_RESUME_IND into T_CONN_IND or T_CONN_CON (with no data) or discard
 * depending on the state of the connection.  For T_CLTS, translate the MTP_RESUME_IND into
 * T_UDERROR_IND (with no data) (but not really an error).  T_UDERROR_IND is really an out-of-band
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
	if (mtp_chk_state(mtp, TSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		switch (mtp_get_state(mtp)) {
		case TS_WCON_CREQ:
			return t_conn_con(mtp, q, mp, add, NULL, mp->b_cont);
		case TS_IDLE:
		case TS_WRES_CIND:
			if (mtp->coninds > 0)
				/* FIXME: need a sequence number */
				return t_conn_ind(mtp, q, mp, 0, add, NULL, mp->b_cont);
		default:
			goto outstate;
		}
		break;
	case T_CLTS:
		if (mtp_get_state(mtp) != TS_IDLE)
			goto outstate;
		return t_uderror_ind(mtp, q, mp, add, NULL, mp->b_cont, T_MTP_DEST_AVAILABLE);
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
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", mtp->prot.SERV_type);
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
 * Translate MTP_STATUS_IND into T_UDERROR_IND or T_RESET_IND or T_DISCON_IND.
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
			error = T_MTP_USER_PART_UNKNOWN;
			break;
		case MTP_STATUS_UPU_UNEQUIPPED:
			error = T_MTP_USER_PART_UNEQUIPPED;
			break;
		case MTP_STATUS_UPU_INACCESSIBLE:
			error = T_MTP_USER_PART_UNAVAILABLE;
			break;
		default:
			goto badstatus;
		}
		break;
	case MTP_STATUS_TYPE_CONG:
		switch (status) {
		case MTP_STATUS_CONGESTION_LEVEL0:
			error = T_MTP_DEST_CONGESTED(0);
			break;
		case MTP_STATUS_CONGESTION_LEVEL1:
			error = T_MTP_DEST_CONGESTED(1);
			break;
		case MTP_STATUS_CONGESTION_LEVEL2:
			error = T_MTP_DEST_CONGESTED(2);
			break;
		case MTP_STATUS_CONGESTION_LEVEL3:
			error = T_MTP_DEST_CONGESTED(3);
			break;
		case MTP_STATUS_CONGESTION:
			error = T_MTP_DEST_CONGESTED(4);
			break;
		default:
			goto badstatus;
		}
		break;
	default:
		goto badtype;
	}
	if (mtp_chk_state(mtp, TSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		if (mtp_not_state(mtp, (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
			goto outstate;
		switch (type) {
		case MTP_STATUS_TYPE_UPU:
			return t_discon_ind(mtp, q, mp, error, mp->b_cont);
		case MTP_STATUS_TYPE_CONG:
			return t_reset_ind(mtp, q, mp, error);
		}
		goto badstatus;
	case T_CLTS:
		if (mtp_get_state(mtp) != TS_IDLE)
			goto outstate;
		dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_length);
		return t_uderror_ind(mtp, q, mp, dst, NULL, mp->b_cont, error);
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
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", mtp->prot.SERV_type);
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
 * Translate MTP_RESTART_BEGINS_IND into T_UDERROR_IND or T_DISCON_IND.
 */
static inline fastcall __unlikely int
mtp_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	mtp_ulong error = T_MTP_RESTARTING;
	struct mtp_addr *dst = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (mtp_chk_state(mtp, TSF_WACK))
		goto wait;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		switch (mtp_get_state(mtp)) {
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
		case TS_WCON_CREQ:
		case TS_DATA_XFER:
			return t_discon_ind(mtp, q, mp, error, mp->b_cont);
		default:
			goto outstate;
		}
		break;
	case T_CLTS:
		switch (mtp_get_state(mtp)) {
		case TS_UNBND:
			goto outstate;
		case TS_IDLE:
			return t_uderror_ind(mtp, q, mp, dst, NULL, mp->b_cont, error);
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
	mi_strlog(q, 0, SL_ERROR, "invalid service type %d", mtp->prot.SERV_type);
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
	return (0);
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
 *  Primitives from TPI to MTP.
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
	case T_CONN_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_CONN_REQ");
		err = t_conn_req(mtp, q, mp);
		break;
	case T_CONN_RES:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_CONN_RES");
		err = t_conn_res(mtp, q, mp);
		break;
	case T_DISCON_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_DISCON_REQ");
		err = t_discon_req(mtp, q, mp);
		break;
	case T_DATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_DATA_REQ");
		err = t_data_req(mtp, q, mp);
		break;
	case T_EXDATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_EXDATA_REQ");
		err = t_exdata_req(mtp, q, mp);
		break;
	case T_INFO_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_INFO_REQ");
		err = t_info_req(mtp, q, mp);
		break;
	case T_BIND_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_BIND_REQ");
		err = t_bind_req(mtp, q, mp);
		break;
	case T_UNBIND_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_UNBIND_REQ");
		err = t_unbind_req(mtp, q, mp);
		break;
	case T_UNITDATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_UNITDATA_REQ");
		err = t_unitdata_req(mtp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_OPTMGMT_REQ");
		err = t_optmgmt_req(mtp, q, mp);
		break;
	case T_ORDREL_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_ORDREL_REQ");
		err = t_ordrel_req(mtp, q, mp);
		break;
	case T_OPTDATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_OPTDATA_REQ");
		err = t_optdata_req(mtp, q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_ADDR_REQ");
		err = t_addr_req(mtp, q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_CAPABILITY_REQ");
		err = t_capability_req(mtp, q, mp);
		break;
#endif
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
		if (prim == T_UNITDATA_REQ) {
		}
		if (prim == T_DATA_REQ) {
		}
		if (prim == T_EXDATA_REQ) {
		}
		if (prim == T_OPTDATA_REQ) {
		}
	}
	return mtp_w_proto_slow(q, mp);
}

/*
 *  Primitives from MTP to TPI.
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
	struct mtp_opts opts;

	if (unlikely(mp->b_cont == NULL))
		goto go_slow;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	if (unlikely(p->mtp_primitive != MTP_TRANSFER_IND))
		goto go_slow;
	opts.flags = (1 << T_MTP_SLS) | (1 << T_MTP_MP);
	opts.sls = &p->mtp_sls;
	opts.mp = &p->mtp_mp;
	opts.debug = NULL;
	if (unlikely(mtp_chk_state(mtp, TSF_WACK)))
		goto go_slow;
	/* Two choices, either we are connectionless or connection oriented. */
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		if (unlikely(mtp_not_state(mtp, (TSF_WREQ_ORDREL | TSF_DATA_XFER))))
			goto go_slow;
		return t_optdata_ind(mtp, q, mp, 0, &opts, mp->b_cont);
	case T_CLTS:
		if (unlikely(mtp_not_state(mtp, (TSF_IDLE | TSF_WACK_UREQ))))
			goto go_slow;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length))
			goto go_slow;
		if (p->mtp_srce_length) {
			if (unlikely(p->mtp_srce_length < sizeof(*src)))
				goto go_slow;
			src = (typeof(src)) (mp->b_rptr + p->mtp_srce_offset);
		}
		mp->b_cont->b_band = mp->b_band;
		return t_unitdata_ind(mtp, q, mp, src, &opts, mp->b_cont);
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
	return t_data(mtp, q, mp);
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
	mtp->prot.PRIM_type = T_INFO_ACK;
	mtp->prot.TSDU_size = T_INFINITE;
	mtp->prot.ETSDU_size = T_INVALID;
	mtp->prot.CDATA_size = T_INVALID;
	mtp->prot.DDATA_size = T_INVALID;
	mtp->prot.ADDR_size = sizeof(struct mtp_addr);
	mtp->prot.OPT_size = T_INFINITE;
	mtp->prot.TIDU_size = T_INFINITE;
	mtp->prot.SERV_type = T_CLTS;
	mtp->prot.CURRENT_state = -1U;
	mtp->prot.PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
	/* issue immediate information request */
	*(mtp_ulong *) mp->b_wptr = MTP_INFO_REQ;
	mp->b_wptr += sizeof(mtp_ulong);
	qprocson(q);
	putnext(q, mp);
	if ((mtp->prot.CURRENT_state == -1U && qwait_sig(q) == 0) ||
	    (mtp->prot.CURRENT_state != TS_UNBND)) {
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
static struct streamtab mtp_tpiinfo = {
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
MODULE_PARM_DESC(modid, "Module ID for the MTP-TPI module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static struct fmodsw mtp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mtp_tpiinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
mtp_tpiinit(void)
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
mtp_tpiterminate(void)
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
module_init(mtp_tpiinit);
module_exit(mtp_tpiterminate);

#endif				/* LINUX */
