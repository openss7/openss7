/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 *  This driver provides transport interface loopback streams TICLTS, TICOTS
 *  and TICOTSORD that are provided by devices /dev/ticlts /dev/ticots and
 *  /dev/ticots_ord.  These are similar to UNIX domain sockets but this
 *  implementation is not directly associated with them.  That is, an AF_UNIX
 *  socket cannot communicate with a ticots Stream, even when bound or
 *  connecting to the same address.  (Note that the inet(4) driver is
 *  different in this respect.)
 */

#define _SVR4_SOURCE
#define _LFS_SOURCE

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define tp_tst_bit(nr,addr)	    test_bit(nr,addr)
#define tp_set_bit(nr,addr)	    __set_bit(nr,addr)
#define tp_clr_bit(nr,addr)	    __clear_bit(nr,addr)

#include <linux/interrupt.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_inet.h>

#define T_ALLLEVELS -1

#define TI_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TI_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define TI_COPYRIGHT	"Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved."
#define TI_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define TI_DEVICE	"SVR 4.2 STREAMS TI Driver"
#define TI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TI_LICENSE	"GPL"
#define TI_BANNER	TI_DESCRIP	"\n" \
			TI_EXTRA	"\n" \
			TI_REVISION	"\n" \
			TI_COPYRIGHT	"\n" \
			TI_DEVICE	"\n" \
			TI_CONTACT
#define TI_SPLASH	TI_DESCRIP	" - " \
			TI_REVISION

#ifdef LINUX
MODULE_AUTHOR(TI_CONTACT);
MODULE_DESCRIPTION(TI_DESCRIP);
MODULE_SUPPORTED_DEVICE(TI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define UDP_DRV_ID	CONFIG_STREAMS_UDP_MODID
#define UDP_DRV_NAME	CONFIG_STREAMS_UDP_NAME
#define UDP_CMAJORS	CONFIG_STREAMS_UDP_NMAJORS
#define UDP_CMAJOR_0	CONFIG_STREAMS_UDP_MAJOR
#define UDP_UNITS	CONFIG_STREAMS_UDP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_UDP_MODID));
MODULE_ALIAS("streams-driver-tl");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_UDP_MAJOR));
MODULE_ALIAS("/dev/streams/tl");
MODULE_ALIAS("/dev/streams/tl/ticlts");
MODULE_ALIAS("/dev/streams/tl/ticots");
MODULE_ALIAS("/dev/streams/tl/ticotsord");
MODULE_ALIAS("/dev/streams/tl/*");
MODULE_ALIAS("/dev/streams/clone/tl");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/tl");
MODULE_ALIAS("/dev/ticlts");
MODULE_ALIAS("/dev/ticots");
MODULE_ALIAS("/dev/ticotsord");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  ==========================================================================
 *
 *  STREAMS Definitions
 *
 *  ==========================================================================
 */

#define DRV_ID		TI_DRV_ID
#define DRV_NAME	TI_DRV_NAME
#define CMAJORS		TI_CMAJORS
#define CMAJOR_0	TI_CMAJOR_0
#define UNITS		TI_UNITS
#ifdef MODULE
#define DRV_BANNER	TI_BANNER
#else				/* MODULE */
#define DRV_BANNER	TI_SPLASH
#endif				/* MODULE */

STATIC struct module_info tl_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT << 4,	/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_stat tl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat tl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* Upper multiplex is a T provider following the TPI. */

STATIC streamscall int tl_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int tl_qclose(queue_t *, int, cred_t *);

streamscall int tp_rput(queue_t *, mblk_t *);
streamscall int tp_rsrv(queue_t *);

STATIC struct qinit tl_rinit = {
	.qi_putp = tp_rput,		/* Read put procedure (message from below) */
	.qi_srvp = tp_rsrv,		/* Read service procedure */
	.qi_qopen = tl_qopen,		/* Each open */
	.qi_qclose = tl_qclose,		/* Last close */
	.qi_minfo = &tl_rinfo,		/* Module information */
	.qi_mstat = &tl_rstat,		/* Module statistics */
};

STATIC struct module_info tl_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

streamscall int tp_wput(queue_t *, mblk_t *);
streamscall int tp_wsrv(queue_t *);

STATIC struct qinit tl_winit = {
	.qi_putp = tp_wput,		/* Write put procedure (message from above) */
	.qi_srvp = tp_wsrv,		/* Write service procedure */
	.qi_minfo = &tl_winfo,		/* Module information */
	.qi_mstat = &tl_wstat,		/* Module statistics */
};

MODULE_STATIC struct streamtab tl_info = {
	.st_rdinit = &tl_rinit,		/* Upper read queue */
	.st_wrinit = &tl_winit,		/* Upper write queue */
};

/*
 *  Primary data structures.
 */

struct tp_options {
	unsigned long flags[3];		/* at least 96 flags */
	struct {
		t_uscalar_t debug[4];	/* XTI_DEBUG */
		struct t_linger linger;	/* XTI_LINGER */
		t_uscalar_t rcvbuf;	/* XTI_RCVBUF */
		t_uscalar_t rcvlowat;	/* XTI_RCVLOWAT */
		t_uscalar_t sndbuf;	/* XTI_SNDBUF */
		t_uscalar_t sndlowat;	/* XTI_SNDLOWAT */
		t_uscalar_t priority;	/* XTI_PRIORITY */
	} xti;
};

/* Private structure */
struct tp {
	struct tp *next;		/* list linkage */
	struct tp **prev;		/* list linkage */
	atomic_t refcnt;		/* reference count */
	spinlock_t lock;		/* structure lock */
	struct T_info_ack info;		/* service provider information */
	unsigned int BIND_flags;	/* bind flags */
	unsigned int CONIND_number;	/* maximum number of outstanding connection indications */
	mblk_t *conq;			/* connection indication queue */
	struct tp_options options;	/* protocol options */
};

#define PRIV(__q) (((__q)->q_ptr))
#define TP_PRIV(__q) ((struct tp *)((__q)->q_ptr))

#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		SHEADHIWAT
#define xti_default_rcvlowat		SHEADLOWAT
#define xti_default_sndbuf		SHEADHIWAT
#define xti_default_sndlowat		SHEADLOWAT
#define xti_default_priority		0

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_XTI_PRIORITY,
};

#define t_tst_bit(nr,addr) tp_tst_bit(nr,addr)
#define t_set_bit(nr,addr) tp_set_bit(nr,addr)
#define t_clr_bit(nr,addr) tp_clr_bit(nr,addr)

static kmem_cachep_t tl_prot_cachep;
static kmem_cachep_t tl_priv_cachep;

static inline struct tp *
tp_get(struct tp *tp)
{
	dassert(tp != NULL);
	atomic_inc(&tp->refcnt);
	return (tp);
}
static inline __hot void
tp_put(struct tp *tp)
{
	dassert(tp != NULL);
	if (atomic_dec_and_test(&tp->refcnt)) {
		kmem_cache_free(tl_priv_cachep, tp);
	}
}
static inline fastcall __hot void
tp_release(struct tp **tpp)
{
	struct tp *tp;
	dassert(tpp != NULL);
	if (likely((tp = xchg(tpp, NULL)) != NULL))
		tp_put(tp);
}
static inline struct tp *
tp_alloc(void)
{
	struct tp *tp;
	if ((tp = kmem_cache_alloc(tl_priv_cachep, GFP_ATOMIC))) {
		bzero(tp, sizeof(*tp));
		atomic_set(&tp->refcnt, 1);
		spin_lock_init(&tp->lock);
		tp->next = NULL;
		tp->prev = &tp->next;
	}
	return (tp);
}

/*
 *  =========================================================================
 *
 *  State Changes
 *
 *  =========================================================================
 */

/* State flags */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#ifdef TS_WACK_OPTREQ
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#endif
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

/* State masks */
#define TSM_ALLSTATES	(TSF_NOSTATES - 1)
#define TSM_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)
#define TSM_LISTEN	(TSF_IDLE \
			|TSF_WRES_CIND)
#define TSM_CONNECTED	(TSF_WCON_CREQ\
			|TSF_WRES_CIND\
			|TSF_DATA_XFER\
			|TSF_WIND_ORDREL\
			|TSF_WREQ_ORDREL)
#define TSM_DISCONN	(TSF_IDLE\
			|TSF_UNBND)
#define TSM_INDATA	(TSF_DATA_XFER\
			|TSF_WIND_ORDREL)
#define TSM_OUTDATA	(TSF_DATA_XFER\
			|TSF_WREQ_ORDREL)

#ifndef T_PROVIDER
#define T_PROVIDER  0
#define T_USER	    1
#endif

#ifdef _DEBUG
STATIC const char *
tp_state_name(t_scalar_t state)
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
		return ("(unknown)");
	}
}
#endif				/* _DEBUG */

/* State functions */

STATIC INLINE fastcall __unlikely void
tp_set_state(struct tp *tp, const t_uscalar_t state)
{
	_printd(("%s: %p: %s <- %s\n", DRV_NAME, tp, tp_state_name(state),
		 tp_state_name(tp->info.CURRENT_state)));
	tp->info.CURRENT_state = state;
}

STATIC INLINE fastcall __hot t_uscalar_t
tp_get_state(const struct tp *tp)
{
	return (tp->info.CURRENT_state);
}

STATIC INLINE fastcall __unlikely t_uscalar_t
tp_chk_state(const struct tp *tp, const t_uscalar_t mask)
{
	return (((1 << tp->info.CURRENT_state) & (mask)) != 0);
}

STATIC INLINE fastcall __unlikely t_uscalar_t
tp_not_state(const struct tp *tp, const t_uscalar_t mask)
{
	return (((1 << tp->info.CURRENT_state) & (mask)) == 0);
}

STATIC INLINE fastcall __unlikely long
tp_get_statef(const struct tp *tp)
{
	return (1 << tp_get_state(tp));
}

/*
 *  Option Handling
 */
#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

STATIC struct tp_options tp_defaults = {
	.flags = {0,},
	.xti = {
		.debug = xti_default_debug,
		.linger = xti_default_linger,
		.rcvbuf = xti_default_rcvbuf,
		.rcvlowat = xti_default_rcvlowat,
		.sndbuf = xti_default_sndbuf,
		.sndlowat = xti_default_sndlowat,
		.priority = xti_default_priority,
		},
};

#define t_defaults tp_defaults

/**
 * t_opts_parse - parse options from received primitive
 * @opts: pointer to options structure
 * @ip: pointer to options
 * @ilen: length of options
 */
STATIC int
t_opts_parse(const unsigned char *ip, const size_t ilen, struct tp_options *op)
{
	const struct t_opthdr *ih;
	int optlen;
	int err;

	err = TBADOPT;
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); unlikely(ih != NULL);
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (unlikely(ih->len < sizeof(*ih)))
			goto error;
		if (unlikely((unsigned char *) ih + ih->len > ip + ilen))
			goto error;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto error;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				goto error;
			case XTI_DEBUG:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen > sizeof(op->xti.debug)))
					goto error;
				bcopy(valp, op->xti.debug, optlen);
				t_set_bit(_T_BIT_XTI_DEBUG, op->flags);
				continue;
			}
			case XTI_LINGER:
			{
				const struct t_linger *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->xti.linger = *valp;
				t_set_bit(_T_BIT_XTI_LINGER, op->flags);
				continue;
			}
			case XTI_RCVBUF:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->xti.rcvbuf = *valp;
				t_set_bit(_T_BIT_XTI_RCVBUF, op->flags);
				continue;
			}
			case XTI_RCVLOWAT:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->xti.rcvlowat = *valp;
				t_set_bit(_T_BIT_XTI_RCVLOWAT, op->flags);
				continue;
			}
			case XTI_SNDBUF:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->xti.sndbuf = *valp;
				t_set_bit(_T_BIT_XTI_SNDBUF, op->flags);
				continue;
			}
			case XTI_SNDLOWAT:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->xti.sndlowat = *valp;
				t_set_bit(_T_BIT_XTI_SNDLOWAT, op->flags);
				continue;
			}
			}
		}
	}
	return (0);
      error:
	return (err);
}

/**
 * t_size_default_options - size default options
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 *
 * Check the validity of the option structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 */
noinline fastcall int
t_size_default_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	_ptrace(("%p: Calculated option output size = %u\n", t, olen));
	return (olen);
      einval:
	ptrace(("%p: ERROR: Invalid input options\n", t));
	return (-EINVAL);
}

/**
 * t_size_current_options - calculate size of current options
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 */
noinline fastcall int
t_size_current_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t->options.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t->options.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t->options.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t->options.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	_ptrace(("%p: Calculated option output size = %u\n", t, olen));
	return (olen);
      einval:
	ptrace(("%p: ERROR: Invalid input options\n", t));
	return (-EINVAL);
}

/**
 * t_size_check_option - determine size of options output for T_CHECK
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 */
noinline fastcall int
t_size_check_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				/* can be any non-zero array of t_uscalar_t */
				if (optlen
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (optlen && optlen != sizeof(t->options.xti.linger))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (optlen && optlen != sizeof(t->options.xti.rcvbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (optlen && optlen != sizeof(t->options.xti.rcvlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (optlen && optlen != sizeof(t->options.xti.sndbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (optlen && optlen != sizeof(t->options.xti.sndlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	_ptrace(("%p: Calculated option output size = %u\n", t, olen));
	return (olen);
      einval:
	ptrace(("%p: ERROR: Invalid input options\n", t));
	return (-EINVAL);
}

/**
 * t_size_negotiate_options - determine size of options output for T_NEGOTIATE
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 */
noinline fastcall int
t_size_negotiate_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				if (ih->name != T_ALLOPT
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.linger))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.rcvbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.xti.rcvlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.sndbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.xti.sndlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	_ptrace(("%p: Calculated option output size = %u\n", t, olen));
	return (olen);
      einval:
	ptrace(("%p: ERROR: Invalid input options\n", t));
	return (-EINVAL);
}

/**
 * t_overall_result - determine overall options results
 * @overall: current overall result
 * @result: result to include in the overall result
 *
 * Calculates the overall T_OPTMGMT_ACK flag result from individual results.
 */
STATIC uint
t_overall_result(uint * overall, const uint result)
{
	switch (result) {
	case T_NOTSUPPORT:
		if (!(*overall & (T_NOTSUPPORT)))
			*overall = T_NOTSUPPORT;
		break;
	case T_READONLY:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY)))
			*overall = T_READONLY;
		break;
	case T_FAILURE:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE)))
			*overall = T_FAILURE;
		break;
	case T_PARTSUCCESS:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS)))
			*overall = T_PARTSUCCESS;
		break;
	case T_SUCCESS:
		if (!
		    (*overall &
		     (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS | T_SUCCESS)))
			*overall = T_SUCCESS;
		break;
	}
	return (result);
}

/**
 * t_build_default_options - build options output for T_DEFAULT
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 * @op: output pointer
 * @olen: output length
 *
 * Perform the actions required of T_DEFAULT placing the output in the provided buffer.
 */
noinline fastcall t_scalar_t
t_build_default_options(const struct tp *t, const unsigned char *ip, size_t ilen,
			unsigned char *op, size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t_defaults.xti.debug, T_OPT_DATA(oh),
				      sizeof(t_defaults.xti.debug));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				*((struct t_linger *) T_OPT_DATA(oh)) = t_defaults.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_build_current_options - build options output for T_CURRENT
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 * @op: output pointer
 * @olen: output length
 *
 * Perform the actions required of T_CURRENT placing the output in the provided buffer.
 */
noinline fastcall t_scalar_t
t_build_current_options(const struct tp *t, const unsigned char *ip, size_t ilen,
			unsigned char *op, size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t->options.xti.debug, T_OPT_DATA(oh),
				      4 * sizeof(t_uscalar_t));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_linger *) T_OPT_DATA(oh)) = t->options.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

#ifdef __LP64__
#undef MAX_SCHEDULE_TIMEOUT
#define MAX_SCHEDULE_TIMEOUT INT_MAX
#endif

/**
 * t_build_check_options - built output options for T_CHECK
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 * @op: output pointer
 * @olen: output length
 *
 * Perform the actions required of T_CHECK placing the output in the provided buffer.
 */
noinline fastcall t_scalar_t
t_build_check_options(const struct tp *t, const unsigned char *ip, size_t ilen, unsigned char *op,
		      size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				if (optlen) {
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!capable(CAP_NET_ADMIN)) {
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
					} else if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(oh), valp, optlen);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if ((valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					    || (valp->l_linger == T_UNSPEC
						&& valp->l_onoff != T_NO))
						goto einval;
					if (valp->l_linger == T_UNSPEC) {
						valp->l_linger = t_defaults.xti.linger.l_linger;
					}
					if (valp->l_linger == T_INFINITE) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->l_linger < 0)
						goto einval;
					if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > INT_MAX) {
						*valp = INT_MAX;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_wmem_max) {
						*valp = sysctl_wmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_build_negotiate_options - build output options for T_NEGOTIATE
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 * @op: output pointer
 * @olen: output length
 *
 * Perform the actions required of T_NEGOTIATE placing the output in the provided buffer.
 */
noinline fastcall t_scalar_t
t_build_negotiate_options(struct tp *t, const unsigned char *ip, size_t ilen, unsigned char *op,
			  size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.debug[0];
				} else {
					bcopy(T_OPT_DATA(ih), valp, optlen);
				}
				if (!capable(CAP_NET_ADMIN)) {	/* XXX */
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				} else {
					bzero(t->options.xti.debug, sizeof(t->options.xti.debug));
					if (oh->len > sizeof(*oh))
						bcopy(valp, t->options.xti.debug,
						      oh->len - sizeof(*oh));
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.linger;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if ((valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					    || (valp->l_linger == T_UNSPEC
						&& valp->l_onoff != T_NO))
						goto einval;
					if (valp->l_linger == T_UNSPEC) {
						valp->l_linger = t_defaults.xti.linger.l_linger;
					}
					if (valp->l_linger == T_INFINITE) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->l_linger < 0)
						goto einval;
					if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.xti.linger = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.rcvbuf;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_RCVBUF) {
						*valp = SOCK_MIN_RCVBUF;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.xti.rcvbuf = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.rcvlowat;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > INT_MAX) {
						*valp = INT_MAX;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.xti.rcvlowat = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.sndbuf;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > sysctl_wmem_max) {
						*valp = sysctl_wmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF) {
						*valp = SOCK_MIN_SNDBUF;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.xti.sndbuf = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.sndlowat;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.xti.sndlowat = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_build_options - build the output options
 * @t: private structure
 * @ip: input pointer
 * @ilen: input length
 * @op: output pointer
 * @olen: output length
 * @flag: management flag
 *
 * Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and T_NEGOTIARE, placing the output
 * in the provided buffer.
 */
noinline fastcall t_scalar_t
t_build_options(struct tp *t, const unsigned char *ip, const size_t ilen, unsigned char *op,
		size_t *olen, const t_scalar_t flag)
{
	switch (flag) {
	case T_DEFAULT:
		return t_build_default_options(t, ip, ilen, op, olen);
	case T_CURRENT:
		return t_build_current_options(t, ip, ilen, op, olen);
	case T_CHECK:
		return t_build_check_options(t, ip, ilen, op, olen);
	case T_NEGOTIATE:
		return t_build_negotiate_options(t, ip, ilen, op, olen);
	}
	return (-EINVAL);
}

/**
 * tp_connect - form a connection
 * @tp: private structure
 * @DEST_buffer: pointer to destination addresses
 * @DEST_length: length of destination addresses
 * @OPT_buffer: pointer to connection option parameters
 * @CONN_flags: connection flags
 *
 * Connect to the specified port and address.  If the connection results in a conflict, TADDRBUSY is
 * returned, zero (0) on success.
 *
 * Destination addresses and port number as well as connection request quality of service parameters
 * should already be stored into the private structure.  Yes, this information will remain if there
 * is an error in the connection request.  When any primitive containing options fails and returns
 * and error, it is the caller's responsibility to set again the values of the options.
 */
STATIC fastcall int
tp_connect(struct tp *tp, const struct sockaddr_un *DEST_buffer, const socklen_t DEST_length,
	   struct tp_options *OPT_buffer, const t_uscalar_t CONN_flags)
{
	size_t dnum = DEST_length / sizeof(*DEST_buffer);
	int err;
	int i;

	err = NBADOPT;
	/* first validate parameters */
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, OPT_buffer->flags)) {
		if (OPT_buffer->xti.rcvbuf > sysctl_rmem_max)
			OPT_buffer->xti.rcvbuf = sysctl_rmem_max;
		if (OPT_buffer->xti.rcvbuf < SOCK_MIN_RCVBUF)
			OPT_buffer->xti.rcvbuf = SOCK_MIN_RCVBUF;
	} else {
		OPT_buffer->xti.rcvbuf = tp->options.xti.rcvbuf;
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, OPT_buffer->flags)) {
		if (OPT_buffer->xti.rcvlowat > sysctl_rmem_max)
			OPT_buffer->xti.rcvlowat = sysctl_rmem_max;
		if (OPT_buffer->xti.rcvlowat < 1)
			OPT_buffer->xti.rcvlowat = 1;
	} else {
		OPT_buffer->xti.rcvlowat = tp->options.xti.rcvlowat;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, OPT_buffer->flags)) {
		if (OPT_buffer->xti.sndbuf > sysctl_wmem_max)
			OPT_buffer->xti.sndbuf = sysctl_wmem_max;
		if (OPT_buffer->xti.sndbuf < SOCK_MIN_SNDBUF)
			OPT_buffer->xti.sndbuf = SOCK_MIN_SNDBUF;
	} else {
		OPT_buffer->xti.sndbuf = tp->options.xti.sndbuf;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, OPT_buffer->flags)) {
		if (OPT_buffer->xti.sndlowat > sysctl_wmem_max)
			OPT_buffer->xti.sndlowat = sysctl_wmem_max;
		if (OPT_buffer->xti.sndlowat < 1)
			OPT_buffer->xti.sndlowat = 1;
	} else {
		OPT_buffer->xti.sndlowat = tp->options.xti.sndlowat;
	}
	if (t_tst_bit(_T_BIT_XTI_PRIORITY, OPT_buffer->flags)) {
		if ((t_scalar_t) OPT_buffer->xti.priority < 0)
			goto error;
		if ((t_scalar_t) OPT_buffer->xti.priority > 255)
			goto error;
	} else {
		OPT_buffer->xti.priority = tp->options.xti.priority;
	}

	/* store negotiated values */
	tp->options.xti.priority = OPT_buffer->xti.priority;

	/* copy bound address to source address */
	bcopy(tp->baddr, tp->saddr, sizeof(tp->baddr));

	/* FIXME copy destination address to tp->daddr. */

	/* try to place in connection hashes with conflict checks */
	if ((err = tp_conn_check(tp)) != 0)
		goto recover;

	return (0);
      recover:
	/* clear out source addresses */
	tp->saddr[0] = '\0';
	/* clear out destination addresses */
	tp->daddr[0] = '\0';
      error:
	return (err);
}

/**
 * tp_passive - perform a passive connection
 * @tp: private structure
 * @RES_buffer: responding addresses
 * @RES_length: length of responding addresses
 * @OPT_buffer: options
 * @SEQ_number: connection indication being accepted
 * @ACCEPTOR_id: accepting Stream private structure
 * @CONN_flags: connection flags
 * @dp: user connect data
 */
noinline fastcall int
tp_passive(struct tp *tp, const struct sockaddr_un *RES_buffer, const socklen_t RES_length,
	   struct tp_options *OPT_buffer, mblk_t *SEQ_number, struct tp *ACCEPTOR_id,
	   const t_uscalar_t CONN_flags, mblk_t *dp)
{
	size_t rnum = RES_length / sizeof(*RES_buffer);
	int err;
	struct iphdr *iph;
	struct udphdr *uh;
	int i, j;

	/* Get at the connection indication.  The packet is contained in the SEQ_number message
	   block starting with the IP header. */
	iph = (typeof(iph)) SEQ_number->b_rptr;
	uh = (typeof(uh)) (SEQ_number->b_rptr + (iph->ihl << 2));

	if (ACCEPTOR_id != tp) {
		err = TBADF;
		/* Accepting Stream must be bound to the same protocol as connection indication. */
		for (j = 0; j < ACCEPTOR_id->pnum; j++)
			if (ACCEPTOR_id->protoids[j] == iph->protocol)
				break;
		if (j >= ACCEPTOR_id->pnum)
			/* Must be bound to the same protocol. */
			goto error;
		/* Accepting Stream must be bound to the same address (or wildcard) including
		   destination address in connection indication. */
		for (i = 0; i < ACCEPTOR_id->bnum; i++)
			if (ACCEPTOR_id->baddrs[i].addr == INADDR_ANY
			    || ACCEPTOR_id->baddrs[i].addr == iph->daddr)
				break;
		if (i >= ACCEPTOR_id->bnum)
			goto error;
	}

	/* validate parameters */
	err = TBADOPT;
	/* Cannot really validate parameters here.  One of the problems is that some of the
	   information against which we should be checking is contained in the connection
	   indication packet, and other information is associated with the destination addresses
	   themselves, that are contained in the responding address(es) for NPI-IP.  Therefore, QOS 
	   parameter checks must be performed in the np_passive() function instead. */
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, OPT_buffer->flags)) {
		if (OPT_buffer->xti.rcvbuf > sysctl_rmem_max)
			OPT_buffer->xti.rcvbuf = sysctl_rmem_max;
		if (OPT_buffer->xti.rcvbuf < SOCK_MIN_RCVBUF)
			OPT_buffer->xti.rcvbuf = SOCK_MIN_RCVBUF;
	} else {
		OPT_buffer->xti.rcvbuf = tp->options.xti.rcvbuf;
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, OPT_buffer->flags)) {
		if (OPT_buffer->xti.rcvlowat > sysctl_rmem_max)
			OPT_buffer->xti.rcvlowat = sysctl_rmem_max;
		if (OPT_buffer->xti.rcvlowat < 1)
			OPT_buffer->xti.rcvlowat = 1;
	} else {
		OPT_buffer->xti.rcvlowat = tp->options.xti.rcvlowat;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, OPT_buffer->flags)) {
		if (OPT_buffer->xti.sndbuf > sysctl_wmem_max)
			OPT_buffer->xti.sndbuf = sysctl_wmem_max;
		if (OPT_buffer->xti.sndbuf < SOCK_MIN_SNDBUF)
			OPT_buffer->xti.sndbuf = SOCK_MIN_SNDBUF;
	} else {
		OPT_buffer->xti.sndbuf = tp->options.xti.sndbuf;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, OPT_buffer->flags)) {
		if (OPT_buffer->xti.sndlowat > sysctl_wmem_max)
			OPT_buffer->xti.sndlowat = sysctl_wmem_max;
		if (OPT_buffer->xti.sndlowat < 1)
			OPT_buffer->xti.sndlowat = 1;
	} else {
		OPT_buffer->xti.sndlowat = tp->options.xti.sndlowat;
	}
	if (t_tst_bit(_T_BIT_XTI_PRIORITY, OPT_buffer->flags)) {
		if ((t_scalar_t) OPT_buffer->xti.priority < 0)
			goto error;
		if ((t_scalar_t) OPT_buffer->xti.priority > 255)
			goto error;
	} else {
		OPT_buffer->xti.priority = ACCEPTOR_id->options.xti.priority;
	}

	/* Need to determine source addressess from bound addresses before we can test the source
	   address.  If we are bound to specific addresses, then the source address list is simply
	   the destination address list. If bound to a wildcard address, then the source address
	   list could be determined from the scope of the destination addresses and the available
	   interfaces and their addresses.  However, for the moment it is probably easier to simply 
	   allow wildcard source addresses and let the user specify any address when there is a
	   wildcard source address.  Port number is a different situation: either the Stream is
	   bound to the port number in the received connection indication, or it was bound to a
	   wildcard port number.  In either case, the local port number for the connection is the
	   port number to which the connection indication was sent. */

	ACCEPTOR_id->sport = uh->dest;
	ACCEPTOR_id->snum = ACCEPTOR_id->bnum;
	for (i = 0; i < ACCEPTOR_id->bnum; i++)
		ACCEPTOR_id->saddrs[i].addr = ACCEPTOR_id->baddrs[i].addr;

	if (OPT_buffer->ip.saddr != QOS_UNKNOWN) {
		if (OPT_buffer->ip.saddr != 0) {
			for (i = 0; i < ACCEPTOR_id->snum; i++) {
				if (ACCEPTOR_id->saddrs[i].addr == INADDR_ANY)
					break;
				if (ACCEPTOR_id->saddrs[i].addr == OPT_buffer->ip.saddr)
					break;
			}
			if (i >= ACCEPTOR_id->snum)
				goto recover;
		}
	} else {
		OPT_buffer->ip.saddr = ACCEPTOR_id->options.ip.saddr;
	}

	ACCEPTOR_id->dport = rnum ? RES_buffer[0].sin_port : uh->source;

	err = TBADADDR;
	if (ACCEPTOR_id->dport == 0 && (ACCEPTOR_id->bport != 0 || ACCEPTOR_id->sport != 0))
		goto recover;
	if (ACCEPTOR_id->dport != 0 && ACCEPTOR_id->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port
		   number. */
		goto recover;

	if (rnum > 0) {
		for (i = 0; i < rnum; i++) {
			struct rtable *rt = NULL;

			if ((err = ip_route_output(&rt, RES_buffer[i].sin_addr.s_addr, 0, 0, 0)))
				goto recover;
			ACCEPTOR_id->daddrs[i].dst = &rt->u.dst;

			/* Note that we do not have to use the destination reference cached above.
			   It is enough that we hold a reference to it so that it remains in the
			   routing caches so lookups to this destination are fast.  They will be
			   released upon disconnection. */

			ACCEPTOR_id->daddrs[i].addr = RES_buffer[i].sin_addr.s_addr;
			ACCEPTOR_id->daddrs[i].ttl = OPT_buffer->ip.ttl;
			ACCEPTOR_id->daddrs[i].tos = OPT_buffer->ip.tos;
			ACCEPTOR_id->daddrs[i].mtu = dst_pmtu(ACCEPTOR_id->daddrs[i].dst);
			if (ACCEPTOR_id->daddrs[i].mtu < OPT_buffer->ip.mtu)
				OPT_buffer->ip.mtu = ACCEPTOR_id->daddrs[i].mtu;
		}
		ACCEPTOR_id->dnum = rnum;
	} else {
		struct rtable *rt = NULL;

		if ((err = ip_route_output(&rt, iph->saddr, 0, 0, 0)))
			goto recover;
		ACCEPTOR_id->daddrs[0].dst = &rt->u.dst;

		/* Note that we do not have to use the destination reference cached above.  It is
		   enough that we hold a reference to it so that it remains in the routing caches
		   so lookups to this destination are fast.  They will be released upon
		   disconnection. */

		ACCEPTOR_id->daddrs[0].addr = iph->saddr;
		ACCEPTOR_id->daddrs[0].ttl = OPT_buffer->ip.ttl;
		ACCEPTOR_id->daddrs[0].tos = OPT_buffer->ip.tos;
		ACCEPTOR_id->daddrs[0].mtu = dst_pmtu(ACCEPTOR_id->daddrs[0].dst);
		if (ACCEPTOR_id->daddrs[0].mtu < OPT_buffer->ip.mtu)
			OPT_buffer->ip.mtu = ACCEPTOR_id->daddrs[0].mtu;

		ACCEPTOR_id->dnum = 1;
	}

	/* store negotiated qos values */
	ACCEPTOR_id->options.xti.priority = OPT_buffer->xti.priority;
	ACCEPTOR_id->options.ip.protocol = OPT_buffer->ip.protocol;
	ACCEPTOR_id->options.ip.ttl = OPT_buffer->ip.ttl;
	ACCEPTOR_id->options.ip.tos = OPT_buffer->ip.tos;
	ACCEPTOR_id->options.ip.mtu = OPT_buffer->ip.mtu;
	ACCEPTOR_id->options.ip.saddr = OPT_buffer->ip.saddr;
	ACCEPTOR_id->options.ip.daddr = OPT_buffer->ip.daddr;
	/* note: on failure allowed to have partially negotiated options */

	/* try to place in connection hashes with conflict checks */
	if ((err = tp_conn_check(ACCEPTOR_id, iph->protocol)) != 0)
		goto recover;

	if (dp != NULL)
		if (unlikely((err = tp_senddata(tp, tp->dport, OPT_buffer, dp)) != QR_ABSORBED))
			goto recover;
	if (SEQ_number != NULL) {
		bufq_unlink(&tp->conq, SEQ_number);
		freeb(XCHG(&SEQ_number, SEQ_number->b_cont));
		/* queue any pending data */
		while (SEQ_number)
			put(ACCEPTOR_id->oq, XCHG(&SEQ_number, SEQ_number->b_cont));
	}

	return (QR_ABSORBED);

      recover:
	/* clear out source addresses */
	ACCEPTOR_id->sport = 0;
	for (i = 0; i < ACCEPTOR_id->snum; i++) {
		ACCEPTOR_id->saddrs[i].addr = INADDR_ANY;
	}
	ACCEPTOR_id->snum = 0;
	/* clear out destination addresses */
	ACCEPTOR_id->dport = 0;
	for (i = 0; i < ACCEPTOR_id->dnum; i++) {
		if (ACCEPTOR_id->daddrs[i].dst)
			dst_release(XCHG(&ACCEPTOR_id->daddrs[i].dst, NULL));
		ACCEPTOR_id->daddrs[i].addr = INADDR_ANY;
		ACCEPTOR_id->daddrs[i].ttl = 0;
		ACCEPTOR_id->daddrs[i].tos = 0;
		ACCEPTOR_id->daddrs[i].mtu = 0;
	}
	ACCEPTOR_id->dnum = 0;
      error:
	return (err);
}

/**
 * tp_disconnect - disconnect from the connection hashes
 * @tp: private structure
 * @RES_buffer: responding address (unused)
 * @SEQ_number: connection indication being refused
 * @DISCON_reason: disconnect reason (unused)
 * @dp: user disconnect data
 */
STATIC int
tp_disconnect(struct tp *tp, const struct sockaddr_in *RES_buffer, mblk_t *SEQ_number,
	      const t_uscalar_t DISCON_reason, mblk_t *dp)
{
	struct tp_chash_bucket *hp;
	int err;
	unsigned long flags;

	if (dp != NULL) {
		err = tp_senddata(tp, tp->dport, &tp->options, dp);
		if (unlikely(err != QR_ABSORBED))
			goto error;
	}
	if (SEQ_number != NULL) {
		unsigned long flags;
		mblk_t *b;

		spin_lock_irqsave(&tp->lock, flags);
		if ((b = rmvb(tp->conq, SEQ_number)) != (mblk_t *)(-1L))
			tp->conq = b;
		spin_unlock_irqrestore(&tp->lock, flags);
		freemsg(SEQ_number);
	}
	if ((hp = tp->chash) != NULL) {
		write_lock_irqsave(&hp->lock, flags);
		if ((*tp->cprev = tp->cnext))
			tp->cnext->cprev = tp->cprev;
		tp->cnext = NULL;
		tp->cprev = &tp->cnext;
		tp->chash = NULL;
		tp->daddr[0] = tp->saddr[0] = '\0';
		tp_set_state(tp, TS_IDLE);
		tp_put(tp);
		write_unlock_irqrestore(&hp->lock, flags);
	}
	return (QR_ABSORBED);
      error:
	return (err);
}

/*
 *  TS Provider -> TS User (Indication, Confirmation and Ack) Primitives
 *  ====================================================================
 */

/**
 * m_flush: deliver an M_FLUSH message upstream
 * @q: a queue in the queue pair
 * @how: FLUSHBAND or FLUSHALL
 * @band: band to flush if how is FLUSHBAND
 */
noinline fastcall int
m_flush(queue_t *q, const int how, const int band)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;

	if (unlikely((mp = mi_allocb(q, 2, BPRI_HI)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = how;
	*mp->b_wptr++ = band;
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error: deliver an M_ERROR message upstream
 * @q: a queue in the queue pair (write queue)
 * @error: the error to deliver
 */
noinline fastcall __unlikely int
m_error(queue_t *q, const int error)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, 2, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_ERROR;
		mp->b_wptr[0] = mp->b_wptr[1] = error;
		mp->b_wptr += 2;
		/* make sure the stream is disconnected */
		tp_disconnect(tp, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the stream is unbound */
		tp_unbind(tp);
		_printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, tp, error));
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_hangup: deliver an M_HANGUP message upstream
 * @q: a queue in the queue pair (write queue)
 */
noinline fastcall __unlikely int
m_hangup(queue_t *q)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, 0, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_HANGUP;
		/* make sure the stream is disconnected */
		tp_disconnect(tp, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the stream is unbound */
		tp_unbind(tp);
		_printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, tp));
		qreply(q, mp);
		return (QR_DONE);

	}
	return (-ENOBUFS);
}

/**
 * te_error_reply - reply to a message with an M_ERROR message
 * @q: active queue in queue pair (write queue)
 * @error: error number
 */
noinline fastcall __unlikely int
te_error_reply(queue_t *q, const long error)
{
	switch (error) {
	case 0:
	case 1:
		__seldom();
		return (error);
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (error);
	case -EPIPE:
	case -ENETDOWN:
	case -EHOSTUNREACH:
		return m_hangup(q);
	default:
		return m_error(q, EPROTO);
	}
}

/**
 * te_info_ack - generate a T_INFO_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 */
noinline fastcall int
te_info_ack(queue_t *q)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;
	size_t size = sizeof(*p);

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_INFO_ACK;
	p->TSDU_size = tp->info.TSDU_size;
	p->ETSDU_size = tp->info.ETSDU_size;
	p->CDATA_size = tp->info.CDATA_size;
	p->DDATA_size = tp->info.DDATA_size;
	p->ADDR_size = tp->info.ADDR_size;
	p->OPT_size = tp->info.OPT_size;
	p->TIDU_size = tp->info.TIDU_size;
	p->SERV_type = tp->info.SERV_type;
	p->CURRENT_state = tp->info.CURRENT_state;
	p->PROVIDER_flag = tp->info.PROVIDER_flag;
	mp->b_wptr += sizeof(*p);
	_printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, tp));
	qreply(q, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * te_bind_ack - TE_BIND_ACK event
 * @q: active queue in queue pair (write queue)
 * @ADDR_buffer: addresses to bind
 * @ADDR_length: length of addresses
 * @CONIND_number: maximum number of connection indications
 *
 * Generate an T_BIND_ACK and pass it upstream.
 */
noinline fastcall int
te_bind_ack(queue_t *q, struct sockaddr_in *ADDR_buffer, const socklen_t ADDR_length,
	    const t_uscalar_t CONIND_number)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp = NULL;
	struct T_bind_ack *p;
	size_t size = sizeof(*p) + ADDR_length;
	int err;

	err = -EFAULT;
	if (unlikely(tp_get_state(tp) != TS_WACK_BREQ))
		goto error;

	err = -ENOBUFS;
	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto error;

	err = tp_bind(tp, ADDR_buffer, ADDR_length, CONIND_number);
	if (unlikely(err != 0)) {
		freeb(mp);
		goto error;
	}

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = CONIND_number;
	mp->b_wptr += sizeof(*p);
	if (ADDR_length) {
		bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
		mp->b_wptr += ADDR_length;
	}
	/* all ready, complete the bind */
	tp_set_state(tp, TS_IDLE);
	_printd(("%s: %p: <- T_BIND_ACK\n", DRV_NAME, tp));
	qreply(q, mp);
	return (QR_DONE);

      error:
	return (err);
}

/**
 * te_error_ack: deliver a T_ERROR_ACK upstream with state changes
 * @q: active queue (write queue)
 * @prim: primitive in error
 * @err: TLI or UNIX error number
 *
 * Some errors are passed through because they are intercepted and used by the put or service
 * procedure on the write side queue.  Theses are 0 (no error), -EBUSY (canput failed), -EAGAIN
 * (lock failed), -ENOMEM (kmem_alloc failed), -ENOBUFS (allocb failed).  All of these pass-through
 * errors will cause the state to be restored to the state before the current write queue message
 * was processed and the current write queue message will be placed back on the write queue.
 *
 * Some BSD-specific UNIX errors are translated to equivalent NPI errors.
 *
 * This function will also place the TPI provider interface state in the correct state following
 * issuing the T_ERROR_ACK according to the Sequence of Primities of the Transport Provider Interface
 * specification, Revision 2.0.0.
 */
noinline fastcall __unlikely int
te_error_ack(queue_t *q, const t_scalar_t ERROR_prim, t_scalar_t error)
{
	struct tp *tp = TP_PRIV(q);
	struct T_error_ack *p;
	mblk_t *mp;
	int err;

	/* rollback state */
	tp_set_state(tp, tp->i_oldstate);
	tp->i_oldstate = tp_get_state(tp);

	err = error;
	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		goto error;
	case 0:
		goto error;
	case -EADDRINUSE:
		error = TADDRBUSY;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		error = TNOADDR;
		break;
	case -EMSGSIZE:
		error = TBADDATA;
		break;
	case -EACCES:
		error = TACCES;
		break;
	case -EOPNOTSUPP:
		error = TNOTSUPPORT;
		break;
	}
	err = -ENOBUFS;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) == NULL)
		goto error;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = ERROR_prim;
	p->TLI_error = (error < 0) ? TSYSERR : error;
	p->UNIX_error = (error < 0) ? -error : 0;
	mp->b_wptr += sizeof(*p);
	_printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, tp));
	qreply(q, mp);
	return (0);
      error:
	return (err);
}

/**
 * te_ok_ack: deliver a T_OK_ACK upstream with state changes
 * @q: a queue in the queue pair
 * @CORRECT_prim: correct primitive
 * @ADDR_buffer: destination or responding address
 * @ADDR_length: length of destination or responding addresses
 * @OPT_buffer: option parameters
 * @SEQ_number: sequence number (i.e. connection/reset indication sequence number)
 * @ACCEPTOR_id: token (i.e. connection response token)
 * @flags: mangement flags, connection flags, disconnect reason, etc.
 * @dp: user data
 */
STATIC fastcall __hot_put int
te_ok_ack(queue_t *q, const t_scalar_t CORRECT_prim, const struct sockaddr_in *ADDR_buffer,
	  const socklen_t ADDR_length, void *OPT_buffer, mblk_t *SEQ_number, struct tp *ACCEPTOR_id,
	  const t_uscalar_t flags, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_ok_ack *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);
	int err = 0;
	unsigned long flags;

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	mp->b_wptr += sizeof(*p);
	switch (tp_get_state(tp)) {
	case TS_WACK_UREQ:
		err = tp_unbind(tp);
		if (unlikely(err != 0))
			goto free_error;
		/* TPI spec says that if the provider must flush both queues before responding with 
		   a T_OK_ACK primitive when responding to a T_UNBIND_REQ. This is to flush queued
		   data for connectionless providers. */
		err = m_flush(q, FLUSHRW, 0);
		if (unlikely(err != 0))
			goto free_error;
		tp_set_state(tp, TS_UNBND);
		break;
	case TS_WACK_CRES:
		/* FIXME: needs to hold reference to and lock the accepting stream */
		if (tp != ACCEPTOR_id)
			ACCEPTOR_id->i_oldstate = tp_get_state(ACCEPTOR_id);
		tp_set_state(ACCEPTOR_id, TS_DATA_XFER);
		err = tp_passive(tp, ADDR_buffer, ADDR_length, OPT_buffer, SEQ_number,
				 ACCEPTOR_id, flags, dp);
		if (unlikely(err != QR_ABSORBED)) {
			tp_set_state(ACCEPTOR_id, ACCEPTOR_id->i_oldstate);
			goto error;
		}
		if (tp != ACCEPTOR_id) {
			spin_lock_irqsave(&tp->lock, flags);
			tp_set_state(tp, tp->conq != NULL ? TS_WRES_CIND : TS_IDLE);
			spin_unlock_irqrestore(&tp->lock, flags);
		}
		break;
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		err = tp_disconnect(tp, ADDR_buffer, SEQ_number, flags, dp);
		if (unlikely(err != QR_ABSORBED))
			goto error;
		spin_lock_irqsave(&tp->lock, flags);
		tp_set_state(tp, tp->conq != NULL ? TS_WRES_CIND : TS_IDLE);
		spin_unlock_irqrestore(&tp->lock, flags);
		break;
	default:
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
		break;
	}
	_printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, tp));
	qreply(q, mp);
	return (err);
      free_error:
	freemsg(mp);
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/**
 * te_conn_con: perform T_CONN_CON actions
 * @q: active queue in queue pair (write queue)
 * @RES_buffer: responding addresses
 * @RES_length: length of responding addresses
 * @QOS_buffer: connected quality of service
 * @CONN_flags: connected connection flags
 *
 * The TPI-IP driver only supports a pseudo-connection-oriented mode.  The destination address and
 * quality-of-service parameters returned in the T_CONN_CON do not represent a connection
 * establishment or negotiation with the remote NS provider, but are simply formed locally and
 * negotiated locally.  The T_CONN_CON is synthesized from the T_CONN_REQ and is returned
 * automatically and immediately in response to the T_CONN_REQ.
 *
 * Note that, unlike TPI, NPI does not return a N_OK_ACK in response to N_CONN_REQ and does not have
 * a TS_WACK_CREQ state, only a NS_WCON_CREQ state.  This makes NPI more ammenable to this kind of
 * pseudo-connection-oriented mode.
 *
 * Originally, this primitive returned one options structure for each destination.  This
 * is no longer the case.  Only one options structure is returned representing the values
 * for the entire association.  ttl is the maximum ttl of the destinations.  mtu is the smallest
 * value for the destinations.  These values are set in the private structure by the tp_connect()
 * function.
 */
noinline fastcall int
te_conn_con(queue_t *q, caddr_t RES_buffer, size_t RES_length, struct tp_options *OPT_buffer,
	    t_uscalar_t CONN_flags)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp = NULL;
	struct T_conn_con *p;
	int err;
	size_t OPT_length = 0;		// FIXME: t_size_conn_con_opts(OPT_buffer);
	size_t size = sizeof(*p) + RES_length + OPT_length;

	tp_set_state(tp, TS_WCON_CREQ);

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	err = tp_connect(tp, RES_buffer, RES_length, OPT_buffer, CONN_flags);
	if (unlikely(err != 0))
		goto free_error;

	tp_set_state(tp, TS_DATA_XFER);

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;

	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + RES_length : 0;
	mp->b_wptr += sizeof(*p);
	if (RES_length) {
		bcopy(RES_buffer, mp->b_wptr, RES_length);
		mp->b_wptr += RES_length;
	}
	if (OPT_length) {
		// FIXME: t_build_conn_con_options(mp->b_wptr, OPT_length, OPT_buffer);
		mp->b_wptr += OPT_length;
	}

	_printd(("%s: %p: <- T_CONN_CON\n", DRV_NAME, tp));
	qreply(q, mp);
	return (0);

      free_error:
	freeb(mp);
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);

}

/**
 * te_conn_ind - generate a T_CONN_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * We generate connection indications to Streams that are bound as listening to an address including
 * the destination address of the IP packet, where no connection exists for the source address of
 * the IP packet.
 */
noinline fastcall __hot_get int
te_conn_ind(queue_t *q, mblk_t *SEQ_number)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp, *cp;
	struct T_conn_ind *p;
	struct sockaddr_in *SRC_buffer;
	t_scalar_t SRC_length, OPT_length;
	size_t size;
	struct iphdr *iph = (struct iphdr *) SEQ_number->b_rptr;
	struct udphdr *uh = (struct udphdr *) (SEQ_number->b_rptr + (iph->ihl << 2));
	unsigned long flags;

	if (unlikely(tp_get_statef(tp) & ~(TSF_IDLE | TSF_WRES_CIND | TSF_WACK_CRES)))
		goto discard;

	/* Make sure we don't already have a connection indication */
	spin_lock_irqsave(&tp->conq.q_lock, flags);
	for (cp = bufq_head(&tp->conq); cp; cp = cp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) cp->b_rptr;
		struct udphdr *uh2 = (struct udphdr *) (cp->b_rptr + (iph->ihl << 2));

		if (iph->protocol == iph2->protocol
		    && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr
		    && uh->source == uh2->source && uh->dest == uh2->dest) {
			/* already have a connection indication, link the data */
			linkb(cp, SEQ_number);
			spin_unlock_irqrestore(&tp->conq.q_lock, flags);
			goto absorbed;
		}
	}
	spin_unlock_irqrestore(&tp->conq.q_lock, flags);

	if (unlikely(bufq_length(&tp->conq) >= tp->CONIND_number))
		/* If there are already too many connection indications outstanding, discard
		   further connection indications until some are accepted -- we might get fancy
		   later and queue it anyway.  Note that data for existing outstanding connection
		   indications is preserved above. */
		goto eagain;
	if (unlikely(tp_not_state(tp, (TSF_IDLE | TSF_WRES_CIND))))
		/* If there is already a connection accepted on the listening stream, discard
		   further connection indications until the current connection disconnects */
		goto eagain;

	tp_set_state(tp, TS_WRES_CIND);

	if (unlikely((cp = tp_dupmsg(q, SEQ_number)) == NULL))
		goto enobufs;

	SRC_length = sizeof(struct sockaddr_in);
	OPT_length = t_opts_size(tp, SEQ_number);
	size = sizeof(*p) + SRC_length + OPT_length;

	if (unlikely((mp = tp_allocb(q, size, BPRI_MED)) == NULL))
		goto free_enobufs;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + SRC_length : 0;
	p->SEQ_number = (t_uscalar_t) (long) SEQ_number;
	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = iph->protocol;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += sizeof(struct sockaddr_in);
	}
	if (OPT_length) {
		t_opts_build(tp, SEQ_number, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	/* should we pull the IP header? */
	mp->b_cont = cp;
	/* sure, all the info is in the options and addresses */
	cp->b_rptr += (iph->ihl << 2);
	/* save original in connection indication list */
	bufq_queue(&tp->conq, SEQ_number);
	_printd(("%s: %p: <- T_CONN_IND\n", DRV_NAME, tp));
	putnext(q, mp);
      absorbed:
	return (QR_ABSORBED);

      ebusy:
	freeb(cp);
	freeb(mp);
	return (-EBUSY);
      free_enobufs:
	freeb(mp);
      enobufs:
	return (-ENOBUFS);
      eagain:
	return (-EAGAIN);
      discard:
	return (QR_DONE);
}

