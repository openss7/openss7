/*****************************************************************************

 @(#) $RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/08/29 20:25:25 $

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

 Last Modified $Date: 2004/08/29 20:25:25 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/08/29 20:25:25 $"

static char const ident[] =
    "$RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/08/29 20:25:25 $";

/*
 *  This is a MTP TPI module which can be pushed over an MTPI (Message
 *  Transfer Part Interface) stream to effect a TPI interface to the MTP.
 *  This module is intended to be used by application programs or by upper
 *  modules that expect a TPI connectionless service provider.
 */
#include "compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <sys/tihdr.h>
// #include <sys/tpi_ss7.h>
// #include <sys/tpi_mtp.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>

#undef INLINE
#define INLINE			/* let compiler do its job */

#define MTP_TPI_DESCRIP		"SS7 Message Transfer Part (MTP) TPI STREAMS MODULE."
#define MTP_TPI_REVISION	"LfS $RCSfile: mtp_tpi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/08/29 20:25:25 $"
#define MTP_TPI_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_TPI_DEVICE		"Part of the OpenSS7 Stack for LiS STREAMS."
#define MTP_TPI_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MTP_TPI_LICENSE		"GPL"
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
#endif				/* LINUX */

#ifdef LFS
#define MTP_TPI_MOD_ID		CONFIG_STREAMS_MTP_TPI_MODID
#define MTP_TPI_MOD_NAME	CONFIG_STREAMS_MTP_TPI_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		MTP_TPI_MOD_ID
#define MOD_NAME	MTP_TPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MTP_TPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	MTP_TPI_SPLASH
#endif				/* MODULE */

STATIC struct module_info mtp_minfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int mtp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int mtp_close(queue_t *, int, cred_t *);

STATIC struct qinit mtp_rinit = {
	qi_putp:ss7_oput,		/* Read put (msg from below) */
	qi_qopen:mtp_open,		/* Each open */
	qi_qclose:mtp_close,		/* Last close */
	qi_minfo:&mtp_minfo,		/* Information */
};

STATIC struct qinit mtp_winit = {
	qi_putp:ss7_iput,		/* Write put (msg from above) */
	qi_minfo:&mtp_minfo,		/* Information */
};

STATIC struct streamtab mtp_tpiinfo = {
	st_rdinit:&mtp_rinit,		/* Upper read queue */
	st_wrinit:&mtp_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  MTP-TPI Private Structure
 *
 *  =========================================================================
 */

typedef struct mtp {
	STR_DECLARATION (struct mtp);	/* stream declaration */
	struct mtp_addr src;		/* srce address */
	struct mtp_addr dst;		/* dest address */
	struct T_info_ack prot;
	struct {
		ulong sls;		/* default options */
		ulong mp;		/* default options */
		ulong debug;		/* default options */
	} options;
} mtp_t;

#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

struct mtp *mtp_opens = NULL;

STATIC struct mtp *mtp_alloc_priv(queue_t *q, struct mtp **, dev_t *, cred_t *);
STATIC struct mtp *mtp_get(struct mtp *);
STATIC void mtp_put(struct mtp *);
STATIC void mtp_free_priv(queue_t *q);

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
typedef struct mtp_opts {
	uint flags;			/* success flags */
	ulong *sls;
	ulong *mp;
	ulong *debug;
} mtp_opts_t;

struct {
	ulong sls;
	ulong mp;
	ulong debug;
} opt_defaults = {
0, 0, 0};

#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
STATIC size_t
mtp_opts_size(struct mtp *mtp, struct mtp_opts *ops)
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
STATIC void
mtp_build_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);
		if (ops->sls) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status = (ops->flags & (1 << T_MTP_SLS)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status = (ops->flags & (1 << T_MTP_MP)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
		if (ops->debug) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->debug));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_DEBUG;
			oh->status = (ops->flags & (1 << T_MTP_DEBUG)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->debug)) p) = *(ops->debug);
			p += _T_ALIGN_SIZEOF(*ops->debug);
		}
	}
}
STATIC int
mtp_parse_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
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
STATIC int
mtp_parse_qos(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
STATIC int
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
STATIC int
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
STATIC int
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
STATIC int
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
#ifdef _DEBUG
STATIC INLINE const char *
mtp_state(ulong state)
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
		return ("TS_NOSTATES");
	default:
		return ("????");
	}
}
#endif

STATIC INLINE ulong
mtp_get_state(struct mtp *mtp)
{
	return mtp->i_state;
}
STATIC INLINE void
mtp_set_state(struct mtp *mtp, ulong newstate)
{
	ulong oldstate = mtp->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", MOD_NAME, mtp, mtp_state(newstate), mtp_state(oldstate)));
	mtp->i_state = mtp->prot.CURRENT_state = newstate;
}

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

STATIC INLINE int
mtp_bind(struct mtp *mtp, struct mtp_addr *src)
{
	if (src)
		mtp->src = *src;
	return (0);
}
STATIC INLINE int
mtp_connect(struct mtp *mtp, struct mtp_addr *dst)
{
	return (0);
}
STATIC INLINE int
mtp_unbind(struct mtp *mtp)
{
	return (0);
}
STATIC INLINE int
mtp_disconnect(struct mtp *mtp)
{
	return (0);
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
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, struct mtp *mtp, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		mtp_set_state(mtp, TS_NOSTATES);
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, mtp));
		putnext(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

STATIC INLINE int t_error_ack(queue_t *q, struct mtp *mtp, const ulong prim, long error);

#if 0
STATIC int
m_error(q, mtp, error)
	queue_t *q;			/* service queue */
	struct mtp *mtp;		/* mtp private structure */
	int error;			/* error number */
{
	mblk_t *mp;
	int hangup = 0;
	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EAGAIN:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if (!(mp = ss7_allocb(q, 2, BPRI_MED)))
		goto enobufs;
	if (hangup) {
		mp->b_datap->db_type = M_HANGUP;
		printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, mtp));
		putnext(mtp->oq, mp);
		return (-error);
	} else {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		mtp_set_state(mtp, TS_NO_STATES);
		printd(("%s; %p: <- M_ERROR\n", MOD_NAME, mtp));
		putnext(mtp->oq, mp);
		return (QR_DONE);
	}
      enobufs:
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  T_CONN_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_CONN_CON
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_con(queue_t *q, struct mtp *mtp, struct mtp_addr *res, struct mtp_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_conn_con *p;
	size_t res_len = res ? sizeof(*res) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + opt_len : 0;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	if (opt_len) {
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- T_CONN_CON\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  T_DISCON_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_discon_ind(queue_t *q, struct mtp *mtp, ulong reason, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_discon_ind *p;
	if ((1 << mtp_get_state(mtp)) &
	    ~(TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL))
		goto efault;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = 0;
	mtp_set_state(mtp, TS_IDLE);
	mp->b_cont = dp;
	printd(("%s: %p: <- T_DISCON_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      efault:
	err = -EFAULT;
	ptrace(("%s: %p: SWERR: unexpected indication for state %ld\n", MOD_NAME, mtp,
		mtp_get_state(mtp)));
	goto error;
      error:
	return (err);
}

/*
 *  T_DATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_data_ind(queue_t *q, struct mtp *mtp, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_data_ind *p;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	printd(("%s: %p: <- T_DATA_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      efault:
	err = -EFAULT;
	ptrace(("%s: %p: SWERR: unexpected indication for state %ld\n", MOD_NAME, mtp,
		mtp_get_state(mtp)));
	goto error;
      error:
	return (err);
}

/*
 *  T_EXDATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_exdata_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_info_ack(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct T_info_ack *p;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	*p = mtp->prot;
	printd(("%s: %p: <- T_INFO_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  T_BIND_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_bind_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *add, ulong cons)
{
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (mtp_get_state(mtp) != TS_WACK_BREQ)
		goto efault;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = cons;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	if ((err = mtp_bind(mtp, add)))
		goto free_error;
	mtp_set_state(mtp, TS_IDLE);
	printd(("%s: %p: <- T_BIND_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", MOD_NAME, mtp,
		mtp_get_state(mtp)));
	goto error;
      error:
	return (err);
      free_error:
	freemsg(mp);
	return t_error_ack(q, mtp, T_BIND_REQ, err);
}

/*
 *  T_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_error_ack(queue_t *q, struct mtp *mtp, const ulong prim, long etype)
{
	int err = etype;
	mblk_t *mp;
	struct T_error_ack *p;
	size_t msg_len = sizeof(*p);
	switch (etype) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		goto error;
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
		never();
		goto error;
	}
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = etype < 0 ? TSYSERR : etype;
	p->UNIX_error = etype < 0 ? -etype : 0;
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
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send TOUTSTATE or TNOTSUPPORT or are responding to a
		   T_OPTMGMT_REQ in other than TS_IDLE state. */
		break;
	}
	printd(("%s: %p: <- T_ERROR_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  T_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_ok_ack(queue_t *q, struct mtp *mtp, ulong prim, ulong seq, ulong tok)
{
	int err;
	mblk_t *mp;
	struct T_ok_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	switch (mtp_get_state(mtp)) {
	case TS_WACK_CREQ:
		if ((err = mtp_connect(mtp, &mtp->dst)))
			goto free_error;
		mtp_set_state(mtp, TS_WCON_CREQ);
		break;
	case TS_WACK_UREQ:
		if ((err = mtp_unbind(mtp)))
			goto free_error;
		mtp_set_state(mtp, TS_UNBND);
		break;
	case TS_WACK_OPTREQ:
		mtp_set_state(mtp, TS_IDLE);
		break;
	case TS_WACK_CRES:
		mtp_set_state(mtp, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		if ((err = mtp_disconnect(mtp)))
			goto free_error;
		mtp_set_state(mtp, TS_IDLE);
		break;
	default:
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
		break;
	}
	printd(("%s: %p: <- T_OK_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
      free_error:
	freemsg(mp);
	return t_error_ack(q, mtp, prim, err);
}

/*
 *  T_UNITDATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_unitdata_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src, struct mtp_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	size_t msg_len = sizeof(*p) + src_len + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (opt_len) {
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- T_UNITDATA_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  T_UDERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_uderror_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, struct mtp_opts *opt,
	      mblk_t *dp, ulong etype)
{
	int err;
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	size_t msg_len = sizeof(*p) + dst_len + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
	p->ERROR_type = etype;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- T_UDERROR_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  T_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_optmgmt_ack(queue_t *q, struct mtp *mtp, ulong flags, struct mtp_opts *opt)
{
	int err;
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
#ifdef TS_WACK_OPTREQ
	if (mtp_get_state(mtp) == TS_WACK_OPTREQ)
		mtp_set_state(mtp, TS_IDLE);
#endif
	printd(("%s: %p: <- T_OPTMGMT_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  T_ORDREL_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_ordrel_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_OPTDATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_optdata_ind(queue_t *q, struct mtp *mtp, ulong flags, struct mtp_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_OPTDATA_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DATA_flag = flags;
	if (opt_len) {
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- T_OPTDATA_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

#ifdef T_ADDR_ACK
/*
 *  T_ADDR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_addr_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *loc, struct mtp_addr *rem)
{
	int err;
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;
	if (!(mp = ss7_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = loc_len;
	p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
	p->REMADDR_length = rem_len;
	p->REMADDR_offset = rem_len ? sizeof(*p) : 0;
	if (loc_len) {
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
	}
	if (rem_len) {
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
	}
	printd(("%s: %p: <- T_ADDR_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}
#endif

#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK
 *  -----------------------------------
 */
STATIC INLINE int
t_capability_ack(queue_t *q, struct mtp *mtp, ulong caps)
{
	int err;
	mblk_t *mp;
	struct T_capability_ack *p;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = TC1_INFO;
	p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) mtp->oq : 0;
	if (caps & TC1_INFO)
		p->INFO_ack = mtp->prot;
	else
		bzero(&p->INFO_ack, sizeof(p->INFO_ack));
	printd(("%s: %p: <- T_CAPABILITY_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}
#endif

/*
 *  T_RESET_IND
 *  -----------------------------------
 */
STATIC INLINE int
t_reset_ind(queue_t *q, struct mtp *mtp, ulong reason)
{
	fixme(("Do something for congestion resets in TPI\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream.
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_BIND_REQ        1 - Bind to an MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_bind_req(queue_t *q, struct mtp *mtp, struct mtp_addr *add, ulong flags)
{
	int err;
	mblk_t *mp;
	struct MTP_bind_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_BIND_REQ;
	p->mtp_addr_length = add_len;
	p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
	p->mtp_bind_flags = flags;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_UNBIND_REQ      2 - Unbind from an MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_unbind_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_unbind_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_UNBIND_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_CONN_REQ        3 - Connect to a remote MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_conn_req(queue_t *q, struct mtp *mtp, struct mtp_addr *add, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct MTP_conn_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_CONN_REQ;
	p->mtp_addr_length = add_len;
	p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
	p->mtp_conn_flags = flags;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	mp->b_cont = dp;
	putnext(mtp->iq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_DISCON_REQ      4 - Disconnect from a remote MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_discon_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_discon_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_DISCON_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_ADDR_REQ        5 - Address service
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_addr_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_addr_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_ADDR_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_INFO_REQ        6 - Information service
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_info_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_info_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_INFO_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_OPTMGMT_REQ     7 - Options management service
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_optmgmt_req(queue_t *q, struct mtp *mtp, struct mtp_opts *opt, ulong flags)
{
	int err;
	mblk_t *mp;
	struct MTP_optmgmt_req *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_OPTMGMT_REQ;
	p->mtp_opt_length = opt_len;
	p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
	p->mtp_mgmt_flags = flags;
	if (opt_len) {
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_TRANSFER_REQ    8 - MTP data transfer request
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_transfer_req(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, ulong pri, ulong sls,
		 mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct MTP_transfer_req *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t msg_len = sizeof(*p) + dst_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->mtp_primitive = MTP_TRANSFER_REQ;
	p->mtp_dest_length = dst_len;
	p->mtp_dest_offset = dst_len ? sizeof(*p) : 0;
	p->mtp_mp = pri;
	p->mtp_sls = sls;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	mp->b_cont = dp;
	putnext(mtp->iq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from above.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
t_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int dlen = msgdsize(mp);
	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot.TSDU_size || dlen > mtp->prot.TIDU_size)
		goto baddata;
	return mtp_transfer_req(q, mtp, &mtp->dst, mtp->options.mp, mtp->options.sls, mp);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", MTP_DRV_NAME, mtp, dlen));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", MTP_DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", MTP_DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", MTP_DRV_NAME));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_CONN_REQ
 *  -----------------------------------
 *  As MTP is really a connectionless protocol, when we form a connection we
 *  simply remember the destination address.  Some interim MTPs had the
 *  abilitty to send a UPT (User Part Test) message.  If the protocol variant
 *  has this ability, we wait for the result of the User Part Test before
 *  confirming the connection.
 */
STATIC int
t_conn_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err = -EFAULT;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	uchar *opt;
	struct mtp_opts opts = { 0L, NULL, };
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto einval;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	if (dst->family != AF_MTP)
		goto badaddr;
	if (dst->si == 0 && mtp->src.si == 0)
		goto noaddr;
	if (dst->si < 3 && mtp->cred.cr_uid != 0)
		goto acces;
	if (dst->si != mtp->src.si)
		goto badaddr;
	opt = mp->b_rptr + p->OPT_offset;
	if (mtp_parse_opts(mtp, &opts, opt, p->OPT_length))
		goto badopt;
	/* 
	   TODO: set options first */
	if (mp->b_cont) {
		putbq(q, mp->b_cont);	/* hold back data */
		mp->b_cont = NULL;	/* abosrbed mp->b_cont */
	}
	mtp->dst = *dst;
	mtp_set_state(mtp, TS_WACK_CREQ);
	return mtp_conn_req(q, mtp, dst, 0, NULL);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", MTP_DRV_NAME, mtp));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", MTP_DRV_NAME, mtp));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is unusable\n", MTP_DRV_NAME, mtp));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate address\n", MTP_DRV_NAME, mtp));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid message format\n", MTP_DRV_NAME, mtp));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MTP_DRV_NAME, mtp));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS\n", MTP_DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_CONN_RES
 *  -------------------------------------------------------------------
 */
STATIC int
t_conn_res(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) != TS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	goto notsupport;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", MTP_DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", MTP_DRV_NAME));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", MTP_DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 */
STATIC int
t_discon_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	switch (mtp_get_state(mtp)) {
	case TS_WCON_CREQ:
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto einval;
		mtp_set_state(mtp, TS_WACK_DREQ6);
		break;
	case TS_DATA_XFER:
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto einval;
		mtp_set_state(mtp, TS_WACK_DREQ9);
		break;
	default:
		goto outstate;
	}
	/* 
	   change state and let t_ok_ack do all the work */
	return mtp_discon_req(q, mtp);
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", MTP_DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", MTP_DRV_NAME));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", MTP_DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 */
STATIC int
t_data_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	if (mtp->prot.SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot.TSDU_size || dlen > mtp->prot.TIDU_size)
		goto baddata;
	return mtp_transfer_req(q, mtp, &mtp->dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", MTP_DRV_NAME, mtp, dlen));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", MTP_DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive format\n", MTP_DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", MTP_DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", MTP_DRV_NAME));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 */
STATIC int
t_exdata_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) mp;
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_info_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) mp;
	return mtp_info_req(q, mtp);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
STATIC int
t_bind_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *src;
	if (mtp_get_state != TS_UNBND)
		goto outstate;
	mtp_set_state(mtp, TS_WACK_BREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto einval;
	if (p->CONIND_number)
		goto notsupport;
	if (!p->ADDR_length)
		goto noaddr;
	if (p->ADDR_length < sizeof(struct mtp_addr))
		goto badaddr;
	src = (typeof(src)) (mp->b_rptr + p->ADDR_offset);
	/* 
	   we don't allow wildcards yet. */
	if (src->family != AF_MTP)
		goto badaddr;
	if (!src->si || !src->pc)
		goto noaddr;
	if (src->si < 3 && mtp->cred.cr_uid != 0)
		goto acces;
	mtp_set_state(mtp, TS_WACK_BREQ);
	return mtp_bind_req(q, mtp, src, 0);
      acces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", MTP_DRV_NAME, mtp));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate address\n", MTP_DRV_NAME, mtp));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", MTP_DRV_NAME, mtp));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MTP_DRV_NAME, mtp));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MTP_DRV_NAME, mtp));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not support for T_CLTS\n", MTP_DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -------------------------------------------------------------------
 */
STATIC int
t_unbind_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	const struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	mtp_set_state(mtp, TS_WACK_UREQ);
	return mtp_unbind_req(q, mtp);
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MTP_DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 */
STATIC int
t_unitdata_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	struct mtp_addr *dst;
	struct mtp_opts opts = { 0L, NULL, };
	if (mtp->prot.SERV_type != T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (dlen == 0 && !(mtp->prot.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (dlen > mtp->prot.TSDU_size || dlen > mtp->prot.TIDU_size)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	if (dst->si < 3 && mtp->cred.cr_uid != 0)
		goto acces;
	if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
		goto badopt;
	fixme(("Handle options correctly\n"));
	return mtp_transfer_req(q, mtp, dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
      badopt:
	ptrace(("%s: %p: ERROR: bad options\n", MTP_DRV_NAME, mtp));
	goto error;
      acces:
	ptrace(("%s: %p: ERROR: no permission to address\n", MTP_DRV_NAME, mtp));
	goto error;
      badaddr:
	ptrace(("%s: %p: ERROR: bad destination address\n", MTP_DRV_NAME, mtp));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid parameter\n", MTP_DRV_NAME, mtp));
	goto error;
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MTP_DRV_NAME, mtp));
	goto error;
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", MTP_DRV_NAME, mtp, dlen));
	goto error;
      notsupport:
	ptrace(("%s: %p: ERROR: primitive not supported for T_COTS or T_COTS_ORD\n", MTP_DRV_NAME,
		mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------
 */
STATIC int
t_optmgmt_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err = 0;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_opts opts = { 0L, NULL, };
#ifdef TS_WACK_OPTREQ
	if (mtp_get_state(mtp) == TS_IDLE)
		mtp_set_state(mtp, TS_WACK_OPTREQ);
#endif
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
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
	return mtp_optmgmt_req(q, mtp, &opts, p->MGMT_flags);
      provspec:
	err = err;
	ptrace(("%s: %p: ERROR: provider specific\n", MTP_DRV_NAME, mtp));
	goto error;
      badflag:
	err = TBADFLAG;
	ptrace(("%s: %p: ERROR: bad options flags\n", MTP_DRV_NAME, mtp));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", MTP_DRV_NAME, mtp));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MTP_DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
STATIC int
t_ordrel_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
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
	return t_error_ack(q, mtp, T_ORDREL_REQ, TNOTSUPPORT);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MTP_DRV_NAME, mtp));
	goto error;
      notsupport:
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS or T_COTS\n", MTP_DRV_NAME,
		mtp));
	goto error;
      error:
	return m_error(q, mtp, EPROTO);
}

/*
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 */
STATIC int
t_optdata_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
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
	if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
		goto badopt;
	fixme(("Handle options correctly\n"));
	return mtp_transfer_req(q, mtp, &mtp->dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", MTP_DRV_NAME, mtp));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MTP_DRV_NAME, mtp));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MTP_DRV_NAME, mtp));
	return m_error(q, mtp, EPROTO);
      discard:
	ptrace(("%s: %p: ERROR: ignore in idle state\n", MTP_DRV_NAME, mtp));
	return (QR_DONE);
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS\n", MTP_DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_addr_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	const struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	(void) mp;
	return mtp_addr_req(q, mtp);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_capability_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	const struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(q, mtp, p->CAP_bits1);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MTP_DRV_NAME, mtp));
	return t_error_ack(q, mtp, p->PRIM_type, -EINVAL);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from below.
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC INLINE int
mtp_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	if (mtp->prot.SERV_type == T_COTS)
		return (QR_PASSALONG);
	swerr();
	return (-EFAULT);
}

/*
 *  MTP_OK_ACK:
 *  -----------------------------------
 *  Simply translate the MTP_OK_ACK into a T_OK_ACK.
 */
STATIC INLINE int
mtp_ok_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	ulong prim;
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
		if ((err = t_ok_ack(q, mtp, prim, 0, 0)) < 0)
			return (err);
		return t_conn_con(q, mtp, NULL, NULL, NULL);
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
	return t_ok_ack(q, mtp, prim, 0, 0);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_ERROR_ACK:
 *  -----------------------------------
 *  Simply translate the MTP_ERROR_ACK into a T_ERROR_ACK.
 */
STATIC INLINE int
mtp_error_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	ulong prim;
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
	return t_error_ack(q, mtp, prim, err);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_BIND_ACK:
 *  -----------------------------------
 *  Translate the MTP_BIND_ACK into a T_BIND_ACK.
 */
STATIC INLINE int
mtp_bind_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_bind_ack *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *add = NULL;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	if (p->mtp_addr_length == sizeof(*add))
		add = (typeof(add)) (mp->b_rptr + p->mtp_addr_offset);
	return t_bind_ack(q, mtp, add, 0);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_ADDR_ACK:
 *  -----------------------------------
 *  Simply translate MTP_ADDR_ACK to T_ADDR_ACK.
 */
STATIC INLINE int
mtp_addr_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
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
	return t_addr_ack(q, mtp, loc, rem);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_INFO_ACK:
 *  -----------------------------------
 *  Simply translate MTP_INFO_ACK to T_INFO_ACK.
 */
STATIC INLINE int
mtp_info_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	mtp->prot.TSDU_size = p->mtp_msu_size;
	mtp->prot.TIDU_size = p->mtp_msu_size;
	mtp->prot.ADDR_size = p->mtp_addr_size;
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
	return t_info_ack(q, mtp);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_OPTMGMT_ACK:
 *  -----------------------------------
 */
STATIC INLINE int
mtp_optmgmt_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  MTP_TRANSFER_IND:
 *  -----------------------------------
 *  Translate MTP_TRANSFER_IND into T_OPTDATA_IND or T_UNITDATA_IND.
 */
STATIC INLINE int
mtp_transfer_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *src;
	struct mtp_opts opts = { 0L, NULL, };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length)
		goto efault;
	if (p->mtp_srce_length != sizeof(*src))
		goto efault;
	src = (typeof(src)) (mp->b_rptr + p->mtp_srce_offset);
	opts.flags |= (1 << T_MTP_SLS) | (1 << T_MTP_MP);
	opts.sls = &p->mtp_sls;
	opts.mp = &p->mtp_mp;
	opts.debug = NULL;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		/* 
		   T_OPTDATA_IND */
		if ((err = t_optdata_ind(q, mtp, 0, &opts, mp->b_cont)) < 0)
			goto error;
		return (QR_TRIMMED);
	case T_CLTS:
		/* 
		   T_UNITDATA_IND */
		if ((err = t_unitdata_ind(q, mtp, src, &opts, mp->b_cont)) < 0)
			goto error;
		return (QR_TRIMMED);
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  MTP_PAUSE_IND:
 *  -----------------------------------
 *  Translate MTP_PAUSE_IND into T_UDERROR_IND or T_DISCON_IND.
 */
STATIC INLINE int
mtp_pause_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	if (p->mtp_addr_length != sizeof(*dst))
		goto efault;
	dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_offset);
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		/* 
		   T_DISCON_IND */
		return t_discon_ind(q, mtp, T_MTP_DEST_PROHIBITED, mp->b_cont);
	case T_CLTS:
	{
		struct mtp_addr *dst;
		dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_length);
		/* 
		   T_UDERROR_IND */
		return t_uderror_ind(q, mtp, dst, NULL, mp->b_cont, T_MTP_DEST_PROHIBITED);
	}
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_RESUME_IND:
 *  -----------------------------------
 *  Translate MTP_RESUME_IND.
 */
STATIC INLINE int
mtp_resume_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	/* 
	   discard */
	return (QR_DONE);
}

/*
 *  MTP_STATUS_IND:
 *  -----------------------------------
 *  Translate MTP_STATUS_IND into T_UDERROR_IND or T_RESET_IND or
 *  T_DISCON_IND.
 */
STATIC INLINE int
mtp_status_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	ulong type;
	ulong status;
	ulong error;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
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
			swerr();
			return (-EFAULT);
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
			swerr();
			return (-EFAULT);
		}
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		switch (type) {
		case MTP_STATUS_TYPE_UPU:
			/* 
			   T_DISCON_IND */
			return t_discon_ind(q, mtp, error, mp->b_cont);
		case MTP_STATUS_TYPE_CONG:
			/* 
			   T_RESET_IND */
			return t_reset_ind(q, mtp, error);
		}
		break;
	case T_CLTS:
	{
		/* 
		   T_UDERROR_IND */
		struct mtp_addr *dst;
		dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_length);
		return t_uderror_ind(q, mtp, dst, NULL, mp->b_cont, error);
	}
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_RESTART_BEGINS_IND:
 *  -----------------------------------
 *  Translate MTP_RESTART_BEGINS_IND into T_UDERROR_IND or T_DISCON_IND.
 */
STATIC INLINE int
mtp_restart_begins_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	ulong error = T_MTP_RESTARTING;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (mtp->prot.SERV_type) {
	case T_COTS:
		/* 
		   T_DISCON_IND */
		return t_discon_ind(q, mtp, error, mp->b_cont);
	case T_CLTS:
	{
		/* 
		   T_UDERROR_IND */
		struct mtp_addr *dst = NULL;
		return t_uderror_ind(q, mtp, dst, NULL, mp->b_cont, error);
	}
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_RESTART_COMPLETE_IND:
 *  -----------------------------------
 *  Translate MTP_RESTART_COMPLETE_IND.
 */
STATIC INLINE int
mtp_restart_complete_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	/* 
	   discard */
	return (QR_DONE);
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
STATIC int
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
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EINVAL;
			break;
		default:
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
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
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
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
 *  Primitives from TPI to MTP.
 *  -----------------------------------
 */
STATIC int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	/* 
	   Fast Path */
	if ((prim = *((ulong *) mp->b_rptr)) == T_DATA_REQ) {
		printd(("%s: %p: -> T_DATA_REQ [%d]\n", MOD_NAME, mtp, msgdsize(mp->b_cont)));
		if ((rtn = t_data_req(q, mtp, mp)))
			mtp_set_state(mtp, oldstate);
		return (rtn);
	}
	switch (prim) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", MOD_NAME, mtp));
		rtn = t_conn_req(q, mtp, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", MOD_NAME, mtp));
		rtn = t_conn_res(q, mtp, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", MOD_NAME, mtp));
		rtn = t_discon_req(q, mtp, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", MOD_NAME, mtp));
		rtn = t_data_req(q, mtp, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", MOD_NAME, mtp));
		rtn = t_exdata_req(q, mtp, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", MOD_NAME, mtp));
		rtn = t_info_req(q, mtp, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", MOD_NAME, mtp));
		rtn = t_bind_req(q, mtp, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", MOD_NAME, mtp));
		rtn = t_unbind_req(q, mtp, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", MOD_NAME, mtp));
		rtn = t_unitdata_req(q, mtp, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", MOD_NAME, mtp));
		rtn = t_optmgmt_req(q, mtp, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", MOD_NAME, mtp));
		rtn = t_ordrel_req(q, mtp, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", MOD_NAME, mtp));
		rtn = t_optdata_req(q, mtp, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", MOD_NAME, mtp));
		rtn = t_addr_req(q, mtp, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		printd(("%s: %p: -> T_CAPABILITY_REQ\n", MOD_NAME, mtp));
		rtn = t_capability_req(q, mtp, mp);
		break;
#endif
	default:
		swerr();
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
	return (rtn);
}

/*
 *  Primitives from MTP to TPI.
 *  -----------------------------------
 */
STATIC int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	/* 
	   Fast Path */
	if ((prim = *((ulong *) mp->b_rptr)) == MTP_TRANSFER_IND) {
		printd(("%s: %p: MTP_TRANSFER_IND [%d] <-\n", MOD_NAME, mtp, msgdsize(mp->b_cont)));
		if ((rtn = mtp_transfer_ind(q, mtp, mp)) < 0)
			mtp_set_state(mtp, oldstate);
		return (rtn);
	}
	switch (prim) {
	case MTP_OK_ACK:
		printd(("%s: %p: MTP_OK_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_ok_ack(q, mtp, mp);
		break;
	case MTP_ERROR_ACK:
		printd(("%s: %p: MTP_ERROR_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_error_ack(q, mtp, mp);
		break;
	case MTP_BIND_ACK:
		printd(("%s: %p: MTP_BIND_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_bind_ack(q, mtp, mp);
		break;
	case MTP_ADDR_ACK:
		printd(("%s: %p: MTP_ADDR_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_addr_ack(q, mtp, mp);
		break;
	case MTP_INFO_ACK:
		printd(("%s: %p: MTP_INFO_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_info_ack(q, mtp, mp);
		break;
	case MTP_OPTMGMT_ACK:
		printd(("%s: %p: MTP_OPTMGMT_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_optmgmt_ack(q, mtp, mp);
		break;
	case MTP_TRANSFER_IND:
		printd(("%s: %p: MTP_TRANSFER_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_transfer_ind(q, mtp, mp);
		break;
	case MTP_PAUSE_IND:
		printd(("%s: %p: MTP_PAUSE_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_pause_ind(q, mtp, mp);
		break;
	case MTP_RESUME_IND:
		printd(("%s: %p: MTP_RESUME_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_resume_ind(q, mtp, mp);
		break;
	case MTP_STATUS_IND:
		printd(("%s: %p: MTP_STATUS_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_status_ind(q, mtp, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		printd(("%s: %p: MTP_RESTART_BEGINS_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_restart_begins_ind(q, mtp, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		printd(("%s: %p: MTP_RESTART_COMPLETE_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_restart_complete_ind(q, mtp, mp);
		break;
	default:
		swerr();
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_w_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	/* 
	   data from above */
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, mtp, msgdsize(mp)));
	return t_data(q, mtp, mp);
}
STATIC int
mtp_r_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	/* 
	   data from below */
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, mtp, msgdsize(mp)));
	return mtp_data(q, mtp, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
mtp_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
#if 0
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
#endif
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
mtp_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
#if 0
	case M_IOCACK:
		return mtp_r_iocack(q, mp);
	case M_IOCNAK:
		return mtp_r_iocnak(q, mp);
#endif
	}
	return (QR_PASSALONG);
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
mtp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct mtp *mtp;
		/* test for multiple push */
		for (mtp = mtp_opens; mtp; mtp = mtp->next) {
			if (mtp->u.dev.cmajor == cmajor && mtp->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(mtp_alloc_priv(q, &mtp_opens, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		fixme(("Get information from below.\n"));
		qprocson(q);
#if 0
		/* 
		   generate immediate information request */
		if ((err = sdt_info_req(q, mtp)) < 0) {
			mtp_free_priv(q);
			MOD_DEC_USE_COUNT;
			return (-err);
		}
#endif
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	mtp_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *mtp_priv_cachep = NULL;
STATIC int
mtp_init_caches(void)
{
	if (!mtp_priv_cachep &&
	    !(mtp_priv_cachep = kmem_cache_create
	      ("mtp_priv_cachep", sizeof(struct mtp), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate mtp_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cace\n", MOD_NAME));
	return (0);
}
STATIC int
mtp_term_caches(void)
{
	if (mtp_priv_cachep) {
		if (kmem_cache_destroy(mtp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed mtp_priv_cachep\n", MOD_NAME));
	}
	return (0);
}
STATIC struct mtp *
mtp_alloc_priv(queue_t *q, struct mtp **mtpp, dev_t *devp, cred_t *crp)
{
	struct mtp *mtp;
	if ((mtp = kmem_cache_alloc(mtp_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated module private structure\n", MOD_NAME));
		bzero(mtp, sizeof(*mtp));
		mtp_get(mtp);	/* first get */
		mtp->u.dev.cmajor = getmajor(*devp);
		mtp->u.dev.cminor = getminor(*devp);
		mtp->cred = *crp;
		(mtp->oq = RD(q))->q_ptr = mtp_get(mtp);
		(mtp->iq = WR(q))->q_ptr = mtp_get(mtp);
		spin_lock_init(&mtp->qlock);	/* "mtp-queue-lock" */
		mtp->o_prim = &mtp_r_prim;
		mtp->i_prim = &mtp_w_prim;
		mtp->o_wakeup = NULL;
		mtp->i_wakeup = NULL;
		mtp->i_state = TS_UNBND;
		mtp->i_style = LMI_STYLE1;
		mtp->i_version = 1;
		spin_lock_init(&mtp->lock);	/* "mtp-priv-lock" */
		if ((mtp->next = *mtpp))
			mtp->next->prev = &mtp->next;
		mtp->prev = mtpp;
		*mtpp = mtp_get(mtp);
		printd(("%s: linked module private structure\n", MOD_NAME));
		/* 
		   configuration defaults */
		mtp->prot.PRIM_type = T_INFO_ACK;
		mtp->prot.TSDU_size = T_INFINITE;
		mtp->prot.ETSDU_size = T_INVALID;
		mtp->prot.CDATA_size = T_INVALID;
		mtp->prot.DDATA_size = T_INVALID;
		mtp->prot.ADDR_size = sizeof(struct mtp_addr);
		mtp->prot.OPT_size = T_INFINITE;
		mtp->prot.TIDU_size = T_INFINITE;
		mtp->prot.SERV_type = T_CLTS;
		mtp->prot.CURRENT_state = TS_UNBND;
		mtp->prot.PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		printd(("%s: setting module private structure defaults\n", MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (mtp);
}
STATIC void
mtp_free_priv(queue_t *q)
{
	struct mtp *mtp = MTP_PRIV(q);
	psw_t flags;
	ensure(mtp, return);
	spin_lock_irqsave(&mtp->lock, flags);
	{
		ss7_unbufcall((str_t *) mtp);
		if ((*mtp->prev = mtp->next))
			mtp->next->prev = mtp->prev;
		mtp->next = NULL;
		mtp->prev = &mtp->next;
		mtp_put(mtp);
		mtp->oq->q_ptr = NULL;
		flushq(mtp->oq, FLUSHALL);
		mtp->oq = NULL;
		mtp_put(mtp);
		mtp->iq->q_ptr = NULL;
		flushq(mtp->iq, FLUSHALL);
		mtp->iq = NULL;
		mtp_put(mtp);
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
	mtp_put(mtp);		/* final put */
}
STATIC struct mtp *
mtp_get(struct mtp *mtp)
{
	atomic_inc(&mtp->refcnt);
	return (mtp);
}
STATIC void
mtp_put(struct mtp *mtp)
{
	if (atomic_dec_and_test(&mtp->refcnt)) {
		kmem_cache_free(mtp_priv_cachep, mtp);
		printd(("%s: %p: freed mtp private structure\n", MOD_NAME, mtp));
	}
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
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for the MTP-TPI module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw mtp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mtp_tpiinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
mtp_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&mtp_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
mtp_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&mtp_fmod)) < 0)
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
mtp_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&mtp_tpiinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
mtp_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&mtp_tpiinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
mtp_tpiinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = mtp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = mtp_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		mtp_term_caches();
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
	if ((err = mtp_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = mtp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mtp_tpiinit);
module_exit(mtp_tpiterminate);

#endif				/* LINUX */
