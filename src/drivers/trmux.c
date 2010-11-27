/*****************************************************************************

 @(#) $RCSfile: trmux.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-06-29 07:35:44 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2009-06-29 07:35:44 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: trmux.c,v $
 Revision 1.1.2.2  2009-06-29 07:35:44  brian
 - SVR 4.2 => SVR 4.2 MP

 Revision 1.1.2.1  2009-06-21 11:20:56  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: trmux.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-06-29 07:35:44 $";

/*
 * This is TRMUX, a TCAP Translaction Sublayer (TR) driver implementing the TRI
 * interface as described in ITU-T Rec. Q.771 as a driver linked over an SCCP
 * stream implementing the NPI-SCCP interface.  This module implements all of
 * the transaction layer service interface described in ITU-T Q.771.
 */

#define _DEBUG 1

#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <sys/sad.h>

#define TRMUX_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TRMUX_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define TRMUX_COPYRIGHT	"Copyright (c) 2008-2010  Monavacon Limited.  All Rights Reserved."
#define TRMUX_REVISION	"Monavacon $RCSfile: trmux.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-06-29 07:35:44 $"
#define TRMUX_DEVICE	"SVR 4.2 MP STREAMS Q.771 Transaction Sublayer (TR) Driver"
#define TRMUX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TRMUX_LICENSE	"GPL"
#define TRMUX_BANNER	TRMUX_DESCRIP	"\n" \
			TRMUX_EXTRA	"\n" \
			TRMUX_REVISION	"\n" \
			TRMUX_COPYRIGHT	"\n" \
			TRMUX_DEVICE	"\n" \
			TRMUX_CONTACT
#define TRMUX_SPLASH	TRMUX_DESCRIPT	"\n" \
			TRMUX_REVISION

#ifdef LINUX
MODULE_AUTHOR(TRMUX_CONTACT);
MODULE_DESCRIPTION(TRMUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(TRMUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TRMUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-trmux");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define TRMUX_DRV_ID	CONFIG_STREAMS_TRMUX_MODID
#define TRMUX_DRV_NAME	CONFIG_STREAMS_TRMUX_NAME
#define TRMUX_CMAJORS	CONFIG_STREAMS_TRMUX_NMAJORS
#define TRMUX_CMAJOR_0	CONFIG_STREAMS_TRMUX_MAJOR
#define TRMUX_UNITS	CONFIG_STREAMS_TRMUX_NMINORS

#ifndef TRMUX_DRV_ID
#error "TRMUX_DRV_ID must be defined."
#endif
#ifndef TRMUX_DRV_NAME
#error "TRMUX_DRV_NAME must be defined."
#endif

#define FIRST_CMINOR	0
#define TR_CMINOR	0
#define MGR_CMINOR	1
#define TP_CMINOR	2
#define TC_CMINOR	3
#define MAP_CMINOR	4
#define LAST_CMINOR	4

#define FREE_CMINOR	10

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TRMUX_MODID));
MODULE_ALIAS("streams-driver-trmux");
MODULE_ALIAS("/dev/streams/trmux");
MODULE_ALIAS("/dev/streams/trmux/*");
MODULE_ALIAS("/dev/streams/clone/trmux");
MODULE_ALIAS("/dev/streams/trmux/tr");
MODULE_ALIAS("/dev/streams/trmux/mgr");
MODULE_ALIAS("/dev/streams/trmux/tp");
MODULE_ALIAS("/dev/streams/trmux/tc");
MODULE_ALIAS("/dev/streams/trmux/map");
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-" __stringify(TR_CMINOR));
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-" __stringify(MGR_CMINOR));
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-" __stringify(TP_CMINOR));
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-" __stringify(TC_CMINOR));
MODULE_ALIAS("char-major-" __stringify(TRMUX_CMAJOR_0) "-" __stringify(MAP_CMINOR));
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef _OPTIMIZE_SPEED
#define STRLOG(priv, level, flags, fmt, ...) \
	do { } while (0)
#else
#define STRLOG(priv, level, flags, fmt, ...) \
	mi_strlog(priv->rq, level, flags, fmt, ##__VA_ARGS__)
#endif

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

#if 1
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#endif

/*
 * STREAMS DEFINITIONS
 */
#define DRV_ID		TRMUX_DRV_ID
#define DRV_NAME	TRMUX_DRV_NAME
#define CMAJORS		TRMUX_CMAJORS
#define CMAJOR_0	TRMUX_CMAJOR_0
#define UNITS		TRMUX_UNITS
#ifdef MODULE
#define DRV_BANNER	TRMUX_BANNER
#else				/* MODULE */
#define DRV_BANNER	TRMUX_SPLASH
#endif				/* MODULE */

static struct module_info tr_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_info sc_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tr_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sc_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static rwlock_t tr_lock = RW_LOCK_UNLOCKED;
static caddr_t tr_opens = NULL;
static caddr_t sc_links = NULL;

struct tr;
struct sc;

/*
 * LOCKING
 */
static inline fastcall struct tr *
tr_trylock(queue_t *q)
{
	struct tr *tr;

	read_lock(&tr_lock);
	tr = (struct tr *) mi_trylock(q);
	read_unlock(&tr_lock);
	return (tr);
}

static inline fastcall void
tr_unlock(struct tr *tr)
{
	read_lock(&tr_lock);
	mi_unlock((caddr_t) tr);
	read_unlock(&tr_lock);
}

static inline fastcall struct tr *
sc_trylock(queue_t *q)
{
	struct sc *sc;

	read_lock(&tr_lock);
	sc = (struct sc *) mi_trylock(q);
	read_unlock(&tr_lock);
	return (sc);
}

static inline fastcall void
sc_unlock(struct sc *sc)
{
	read_lock(&tr_lock);
	mi_unlock((caddr_t) sc);
	read_unlock(&tr_lock);
}

/*
 * PRIVATE STRUCTURES
 */

struct tr;
struct tn;
struct sc;

struct tn {
	struct tr *tr;
	struct tn *tn_next;
	struct tn **tn_prev;
	t_scalar_t CURRENT_state;
	t_uscalar_t pvar;
	struct {
		t_scalar_t CURRENT_state;
	} save;
	struct {
		t_scalar_t TRANS_id;
		t_scalar_t ASSOC_flags;
		t_scalar_t ADDR_length;
		t_scalar_t ADDR_offset;
		union {
			struct sockaddr_sccp addr;
			char abuf[sizeof(struct sockaddr_sccp)];
		};
		union {
			N_qos_sel_data_sccp_t qos;
			char qbuf[sizeof(N_qos_sel_data_sccp_t)];
		};
	} info, peer;
};

struct common {
	struct tr *tr;
	struct sc *sc;
};

struct tr {
	struct common self;		/* must be first */
	queue_t *rq;
	queue_t *wq;
	cred_t cred;
	dev_t dev;
	struct sc *sc;
	uint pvar;			/* protocol variant */
	t_scalar_t base;
	t_scalar_t increment;
	t_scalar_t TRANS_id;
	t_uscalar_t XACT_number;
	struct tn *tn_hash[256];
	struct {
		t_scalar_t TRANS_id;
		t_scalar_t CURRENT_state;
	} save;
	struct {
		t_scalar_t TSDU_size;
		t_scalar_t ETSDU_size;
		t_scalar_t CDATA_size;
		t_scalar_t DDATA_size;
		t_scalar_t ADDR_size;
		t_scalar_t OPT_size;
		t_scalar_t TIDU_size;
		t_scalar_t SERV_type;
		t_scalar_t CURRENT_state;
		t_scalar_t PROVIDER_flag;
		t_scalar_t TRPI_version;
	} info;
	struct {
		t_scalar_t XACT_number;
		t_scalar_t BIND_flags;
		t_scalar_t ADDR_length;
		t_scalar_t ADDR_offset;
		union {
			struct sockaddr_sccp addr;
			char abuf[sizeof(struct sockaddr_sccp)];
		};
	} bind;
};

struct sc {
	struct common self;		/* must be first */
	queue_t *rq;
	queue_t *wq;
	cred_t cred;
	int muxid;
	struct tr *tr;
	uint pvar;			/* protocol variant */
	struct {
		np_ulong CURRENT_state;
	} save;
	struct {
		np_ulong NSDU_size;
		np_ulong ENSDU_size;
		np_ulong CDATA_size;
		np_ulong DDATA_size;
		np_ulong ADDR_size;
		np_ulong ADDR_length;
		np_ulong ADDR_offset;
		np_ulong QOS_length;
		np_ulong QOS_offset;
		np_ulong QOS_range_length;
		np_ulong QOS_range_offset;
		np_ulong OPTIONS_flags;
		np_ulong NIDU_size;
		np_ulong SERV_type;
		np_ulong CURRENT_state;
		np_ulong PROVIDER_type;
		np_ulong NODU_size;
		np_ulong PROTOID_length;
		np_ulong PROTOID_offset;
		np_ulong NPI_version;
		union {
			struct sockaddr_sccp addr;
			unsigned char abuf[sizeof(struct sockaddr_sccp)];
		};
		union {
			N_qos_sel_info_sccp_t qos;
			char qbuf[sizeof(N_qos_sel_info_sccp_t)];
		};
		union {
			N_qos_range_info_sccp_t qor;
			char rbuf[sizeof(N_qos_range_info_sccp_t)];
		};
		char pbuf[4];
	} info;
	struct {
		np_ulong CONIND_number;
		np_ulong BIND_flags;
		np_ulong ADDR_length;
		np_ulong ADDR_offset;
		union {
			struct sockaddr_sccp addr;
			unsigned char abuf[sizeof(struct sockaddr_sccp)];
		};
	} bind;
};

struct priv {
	struct tr tr;
	struct sc sc;
};

static char *
tr_statename(t_scalar_t state)
{
	switch (state) {
	case TRS_UNBND:
		return ("TRS_UNBND");
	case TRS_WACK_BREQ:
		return ("TRS_WACK_BREQ");
	case TRS_WACK_UREQ:
		return ("TRS_WACK_UREQ");
	case TRS_IDLE:
		return ("TRS_IDLE");
	case TRS_WACK_OPTREQ:
		return ("TRS_WACK_OPTREQ");
	case TRS_WACK_CREQ:
		return ("TRS_WACK_CREQ");
	case TRS_WCON_CREQ:
		return ("TRS_WCON_CREQ");
	case TRS_WRES_CIND:
		return ("TRS_WRES_CIND");
	case TRS_WACK_CRES:
		return ("TRS_WACK_CRES");
	case TRS_DATA_XFER:
		return ("TRS_DATA_XFER");
	case TRS_WACK_DREQ6:
		return ("TRS_WACK_DREQ6");
	case TRS_WACK_DREQ7:
		return ("TRS_WACK_DREQ7");
	case TRS_WACK_DREQ9:
		return ("TRS_WACK_DREQ9");
	case TRS_NOSTATES:
		return ("TRS_NOSTATES");
	default:
		return ("TRS_UNINIT");
	}
}

static t_scalar_t
tn_save_state(struct tn *tn)
{
	return (tn->save.CURRENT_state = tn->CURRENT_state);
}
static t_scalar_t
tn_restore_state(struct tn *tn)
{
	return (tn->CURRENT_state = tn->save.CURRENT_state);
}
static t_scalar_t
tn_get_state(struct tn *tn)
{
	return (tn->CURRENT_state);
}
static fastcall void tn_destroy(struct tn *tn);
static t_scalar_t
tn_set_state(struct tn *tn, t_scalar_t newstate)
{
	t_scalar_t oldstate = tn->CURRENT_state;

	if (newstate != oldstate) {
		LOGST(tn->tr, "Transaction %08X: %s <- %s", tn->info.TRANS_id,
		      tr_statename(newstate), tr_statename(oldstate));
		tn->CURRENT_state = newstate;
		if (newstate == TRS_IDLE)
			tn_destroy(tn);
	}
	return (oldstate);
}

static t_scalar_t
tr_save_state(struct tr *tr)
{
	tr->save.TRANS_id = tr->TRANS_id;
	return (tr->save.CURRENT_state = tr->info.CURRENT_state);
}
static t_scalar_t
tr_restore_state(struct tr *tr)
{
	tr->TRANS_id = tr->save.TRANS_id;
	return (tr->info.CURRENT_state = tr->save.CURRENT_state);
}
static inline fastcall t_scalar_t
tr_get_state(struct tr *tr)
{
	return (tr->info.CURRENT_state);
}
static inline fastcall t_scalar_t
tr_set_state(struct tr *tr, t_scalar_t newstate)
{
	t_scalar_t oldstate = tr->info.CURRENT_state;

	if (newstate != oldstate) {
		LOGST(tr, "%s <- %s", tr_statename(newstate), tr_statename(oldstate));
		tr->info.CURRENT_state = newstate;
	}
	return (oldstate);
}

static char *
sc_statename(np_ulong state)
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
	      defulat:
		return ("NS_UNINIT");
	}
}

static np_ulong
sc_save_state(struct sc *sc)
{
	return (sc->save.CURRENT_state = sc->info.CURRENT_state);
}
static np_ulong
sc_restore_state(struct sc *sc)
{
	return (sc->info.CURRENT_state = sc->save.CURRENT_state);
}

static inline fastcall np_ulong
sc_get_state(struct sc *sc)
{
	return (sc->info.CURRENT_state);
}
static fastcall np_ulong
sc_set_state(struct sc *sc, np_ulong newstate)
{
	np_ulong oldstate = sc_get_state(sc);

	if (newstate != oldstate) {
		LOGST(sc, "%s <- %s", sc_statename(newstate), sc_statename(oldstate));
		sc->info.CURRENT_state = newstate;
	}
	return (oldstate);
}

#define PRIV(q) = ((struct priv *)q->q_ptr)
#define TR_PRIV(q) = (PRIV(q)->tr.self.tr)
#define SC_PRIV(q) = (PRIV(q)->tr.self.sc)

/*
 * CACHES
 */
static kmem_cachep_t priv_cachep = NULL;
static kmem_cachep_t tr_cachep = NULL;
static kmem_cachep_t sc_cachep = NULL;
static kmem_cachep_t tn_cachep = NULL;

static int
tr_term_caches(void)
{
	if (tn_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tn_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy tn_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed tn_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(tn_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	if (sc_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sc_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed sc_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(sc_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	if (tr_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tr_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy tr_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed tr_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(tr_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	if (priv_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed priv_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(priv_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	return (0);
}
static int
tr_init_caches(void)
{
	if (!priv_cachep
	    && !(priv_cachep =
		 kmem_create_cache("priv_cachep", mi_open_size(sizeof(struct priv)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate priv_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", MOD_NAME);
	if (!tr_cachep
	    && !(tr_cachep =
		 kmem_create_cache("tr_cachep", mi_open_size(sizeof(struct tr)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate tr_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", MOD_NAME);
	if (!sc_cachep
	    && !(sc_cachep =
		 kmem_create_cache("sc_cachep", mi_open_size(sizeof(struct sc)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate sc_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", MOD_NAME);
	if (!tn_cachep
	    && !(tn_cachep =
		 kmem_create_cache("tn_cachep", mi_open_size(sizeof(struct tn)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate tn_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", MOD_NAME);
	return (0);
}

static fastcall int
tr_get_lower(struct tr *tr, queue_t *q, struct sc **scp)
{
	struct sc *sc;
	int err;

	if (!(sc = tr->sc))
		goto outstate;
	if (sc != tr->self.sc) {
		read_lock(&tr_lock);
		if (!mi_acquire((caddr_t) sc, q)) {
			read_unlock(&tr_lock);
			goto edeadlk;
		}
		read_unlock(&tr_lock);
	}
	*scp = sc;
	return (0);
      edeadlk:
	err = EDEADLK;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      error:
	return (err);
}
static fastcall void
tr_put_lower(struct tr *tr, struct sc *sc)
{
	if (sc != NULL && sc != tr->self.sc)
		mi_release((caddr_t) sc);
}

static fastcall int
sc_get_upper(struct sc *sc, queue_t *q, struct tr **trp)
{
	struct tr *tr;
	int err;

	if (!(tr = sc->tr))
		goto outstate;
	if (tr != sc->self.tr) {
		read_lock(&tr_lock);
		if (!mi_acquire((caddr_t) tr, q)) {
			read_unlock(&tr_lock);
			goto edeadlk;
		}
		read_unlock(&tr_lock);
	}
	*trp = tr;
	return (0);
      edeadlk:
	err = EDEADLK;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      error:
	return (err);
}
static fastcall void
sc_put_upper(struct sc *sc, struct tr *tr)
{
	if (tr != NULL && tr != sc->self.tr)
		mi_release((caddr_t) tr);
}

#define tn_hash_function(TRANS_id) ((TRANS_id ^ (TRANS_id >> 8) ^ (TRANS_id >> 16)) & (0x7f))

static inline struct tn *
tr_lookup(struct tr *tr, t_scalar_t TRANS_id)
{
	struct tn *tn;

	for (tn = tr->tn_hash[tn_hash_function(TRANS_id)]; tn; tn = tn->tn_next)
		if (tn->info.TRANS_id == TRANS_id)
			break;
	return (tn);
}

static inline t_scalar_t
tr_next_transid(struct tr *tr, t_scalar_t TRANS_id)
{
	if (!(TRANS_id += tr->increment))
		TRANS_id += tr->increment;
	if (TRANS_id < 0)
		TRANS_id = tr->base;
	if (!TRANS_id)
		TRANS_id += tr->increment;
	return (TRANS_id);
}

static inline int
tr_alloc_transid(struct tr *tr, t_scalar_t *tidp)
{
	t_scalar_t TRANS_id = *tidp;
	int err;

	if (TRANS_id == 0) {
		int i;

		TRANS_id = tr_next_transid(tr, tr->TRANS_id);
		for (i = 128; i > 0; i--) {
			if (!tr_lookup(tr, TRANS_id))
				break;
			TRANS_id = tr_next_transid(tr, TRANS_id);
		}
		if (i == 0)
			goto noprotoid;
		tr->TRANS_id = TRANS_id;
		*tidp = TRANS_id;
	} else if (tr_lookup(tr, TRANS_id))
		goto badseq;
	return (0);
      noprotoid:
	err = TRNOPROTOID;
	goto error;
      badseq:
	err = TRBADSEQ;
	goto error;
      error:
	return (err);
}

static fastcall int
tn_create(struct tr *tr, struct tn **tnp, t_scalar_t TRANS_id)
{
	struct tn *tn;
	int hash;
	int err;

	if ((err = tr_alloc_transid(tr, &TRANS_id)))
		goto error;
	if (!(tn = kmem_cache_alloc(tn_cachep, GFP_ATOMIC)))
		goto enomem;
	bzero(tn, sizeof(*tn));
	tr->XACT_number++;
	tn->tr = tr;
	hash = tn_hash_function(TRANS_id);
	if ((tn->tn_next = tr->tn_hash[hash]))
		tn->tn_next->tn_prev = &tn->tn_next;
	tn->tn_prev = &tr->tn_hash[hash];
	tn->CURRENT_state = TRS_IDLE;
	tn->pvar = tr->pvar;
	tn->info.TRANS_id = TRANS_id;
	tn->info.ASSOC_flags = 0;
	tn->info.ADDR_length = 0;
	tn->info.ADDR_offset = &tn->info.abuf - (caddr_t) &tn->info;
	tn->info.qos.n_qos_type = N_QOS_SEL_DATA_SCCP;
	tn->infinfo.qos.protocol_class = QOS_UNKNOWN;
	tn->info.qos.option_flags = QOS_UNKNOWN;
	tn->info.qos.sequence_control = QOS_UNKNOWN;
	tn->info.qos.message_priority = QOS_UNKNOWN;
	tn->info.qos.importance = QOS_UNKNOWN;
	tn->peer.TRANS_id = 0;
	tn->peer.ASSOC_flags = 0;
	tn->peer.ADDR_length = 0;
	tn->peer.ADDR_offset = &tn->peer.abuf - (caddr_t) &tn->peer;
	tn->peer.qos.n_qos_type = N_QOS_SEL_DATA_SCCP;
	tn->peer.qos.protocol_class = QOS_UNKNOWN;
	tn->peer.qos.option_flags = QOS_UNKNOWN;
	tn->peer.qos.sequence_control = QOS_UNKNOWN;
	tn->peer.qos.message_priority = QOS_UNKNOWN;
	tn->peer.qos.importance = QOS_UNKNOWN;
	*tnp = tn;
	return (0);
      enomem:
	err = -ENOMEM;
	goto error;
      error:
	return (err);
}

static fastcall void
tn_destroy(struct tn *tn)
{
	if (tn != NULL) {
		if ((*tn->tn_prev = tn->tn_next))
			tn->tn_next->tn_prev = tn->tn_prev;
		tn->tr->XACT_number--;
		bzero(tn, sizeof(*tn));
		kmem_cache_free(tn_cachep, tn);
	}
}

static inline int
tn_abort(struct tn *tn, queue_t *q)
{
	struct tr *tr = tn->tr;
	struct sc *sc = NULL;
	int err;

	if (tr_get_lower(tr, q, &sc) == 0 || sc_get_state(sc) != NS_IDLE) {
		switch (tn_get_state(tn)) {
		case TRS_WACK_CREQ:
		case TRS_WCON_CREQ:
		case TRS_WRES_CIND:
		case TRS_WACK_CRES:
		case TRS_DATA_XFER:
		case TRS_WACK_DREQ6:
		case TRS_WACK_DREQ7:
		case TRS_WACK_DREQ9:
			if ((err = tn_send_u_abort(sc, tn, q, NULL)))
				goto error;
			tn->CURRENT_state = TRS_IDLE;
			break;
		case TRS_IDLE:
			break;
		}
	}
	tr_put_lower(tr, sc);
	tn_destroy(tn);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

static inline int
tr_abort_transactions(struct tr *tr, queue_t *q)
{
	struct tn *tn;
	uint hash;
	int err;

	if (tr->XACT_number) {
		for (hash = 0; hash < 256; hash++) {
			while ((tn = tr->tn_hash[hash])) {
				if ((err = tn_abort(tn, q)))
					goto error;
			}
		}
		tr->XACT_number = 0;
	}
	return (0);
      error:
	return (err);
}

static void
sc_init(struct sc *sc, struct tr *tr)
{
	sc->self.sc = sc;
	if ((sc->self.tr = tr)) {
		sc->rq = tr->rq;
		sc->wq = tr->wq;
		sc->cred = tr->cred;
		sc->muxid = tr->dev;
	}
	sc->tr = tr;
	sc->save.CURRENT_state = -1U;
	sc->info.NSDU_size =;
	sc->info.ENSDU_size =;
	sc->info.CDATA_size =;
	sc->info.DDATA_size =;
	sc->info.ADDR_size = sizeof(struct sockaddr_sccp);
	sc->info.ADDR_length = 0;
	sc->info.ADDR_offset = sc->info.abuf - (caddr_t) &sc->info;
	bzero(sc->bind.abuf, sizeof(sc->bind.abuf));
	sc->info.QOS_length = 0;
	sc->info.QOS_offset = sc->info.qbuf - (caddr_t) &sc->info;
	bzero(sc->bind.qbuf, sizeof(sc->bind.qbuf));
	sc->info.QOS_range_length = 0;
	sc->info.QOS_range_offset = sc->info.rbuf - (caddr_t) &sc->info;
	bzero(sc->bind.rbuf, sizeof(sc->bind.rbuf));
	sc->info.OPTIONS_flags = 0;
	sc->info.NIDU_size =;
	sc->info.SERV_type = N_CLNS;
	sc->info.CURRENT_state = -1U;
	sc->info.PROVIDER_type = N_SUBNET;
	sc->info.NODU_size =;
	sc->info.PROTOID_length = 0;
	sc->info.PROTOID_offset = sc->info.pbuf - (caddr_t) &sc->info;
	bzero(sc->bind.pbuf, sizeof(sc->bind.pbuf));
	sc->info.NPI_version = N_CURRENT_VERSION;
	sc->bind.CONIND_number = 0;
	sc->bind.BIND_flags = 0;
	sc->bind.ADDR_length = 0;
	sc->bind.ADDR_offset = sc->bind.abuf - (caddr_t) &sc->bind;
	bzero(sc->bind.abuf, sizeof(sc->bind.abuf));
}

static void
tr_init(struct tr *tr, struct sc *sc)
{
	tr->self.tr = tr;
	tr->self.sc = sc;
	tr->sc = sc;
	tr->base = 0;
	tr->increment = 1;
	tr->TRANS_id = tr->base;
	tr->XACT_number = 0;
	bzero(tr->tn_hash, sizeof(tr->tn_hash));
	tr->save.TRANS_id = tr->base;
	tr->save.CURRENT_state = TRS_UNBND;
	tr->info.TSDU_size =;
	tr->info.ETSDU_size =;
	tr->info.CDATA_size =;
	tr->info.DDATA_size =;
	tr->info.ADDR_size = sizeof(struct sockaddr_sccp);
	tr->info.OPT_size =;
	tr->info.TIDU_size =;
	tr->info.SERV_type =;
	tr->info.CURRENT_state = TRS_UNBND;
	tr->info.PROVIDER_flag =;
	tr->info.TRPI_version = TRPI_CURRENT_VERSION;
	tr->bind.XACT_number = 0;
	tr->bind.BIND_flags = 0;
	tr->bind.ADDR_length = 0;
	tr->bind.ADDR_offset = tr->bind.abuf - (caddr_t) &tr->bind;
	bzero(tr->bind.abuf, sizeof(tr->bind.abuf));
	if (sc)
		sc_init(sc, tr);
	return;
}

static void
priv_init(struct priv *pp)
{
	struct tr *tr = &pp->tr;
	struct sc *sc = &pp->sc;

	tr_init(tr, sc);
}

#define tr_find_notice(tr, ...) (NULL)
#define tr_find_uderror(tr, ...) (NULL)

static fastcall int
tn_set_dest(struct tn *tn, mblk_t *mp, uint off, uint len)
{
	struct sockaddr_sccp addr;

	if (len == 0) {
		if (tn->peer.ADDR_length == 0)
			goto noaddr;
		goto done;
	}
	if (len != sizeof(addr))
		goto badaddr;
	if (!MBLKIN(mp, off, len))
		goto badaddr;
	bcopy(mp->b_rptr + off, &addr, len);
	/* FIXME: check the address */
	tn->peer.ADDR_length = sizeof(addr);
	tn->peer.addr = addr;
      done:
	return (0);
      badaddr:
	return (TRBADADDR);
      noaddr:
	return (TRNOADDR);
}
static fastcall int
tn_set_orig(struct tn *tn, mblk_t *mp, uint off, uint len)
{
	struct sockaddr_sccp addr;

	if (len == 0) {
		if (tn->info.ADDR_length == 0)
			goto noaddr;
		goto done;
	}
	if (len != sizeof(addr))
		goto badaddr;
	if (!MBLKIN(mp, off, len))
		goto badaddr;
	bcopy(mp->b_rptr + off, &addr, len);
	/* FIXME: check the address */
	tn->info.ADDR_length = sizeof(addr);
	tn->info.addr = addr;
      done:
	return (0);
      badaddr:
	return (TRBADADDR);
      noaddr:
	return (TRNOADDR);
}
static fastcall int
tn_set_opts(struct tn *tn, mblk_t *mp, uint off, uint len)
{
	if (!MBLKIN(mp, off, len))
		return TRBADOPT;
	if (tn_read_opts(tn, mp->b_rptr + off, len) != 0)
		return TRBADOPT;
	return (0);
}

static char *
tr_primname(t_scalar_t prim)
{
	switch (prim) {
	case TR_INFO_REQ:
		return ("TR_INFO_REQ");
	case TR_BIND_REQ:
		return ("TR_BIND_REQ");
	case TR_UNBIND_REQ:
		return ("TR_UNBIND_REQ");
	case TR_OPTMGMT_REQ:
		return ("TR_OPTMGMT_REQ");
	case TR_UNI_REQ:
		return ("TR_UNI_REQ");
	case TR_BEGIN_REQ:
		return ("TR_BEGIN_REQ");
	case TR_BEGIN_RES:
		return ("TR_BEGIN_RES");
	case TR_CONT_REQ:
		return ("TR_CONT_REQ");
	case TR_END_REQ:
		return ("TR_END_REQ");
	case TR_ABORT_REQ:
		return ("TR_ABORT_REQ");
	case TR_ADDR_REQ:
		return ("TR_ADDR_REQ");
	case TR_CAPABILITY_REQ:
		return ("TR_CAPABILITY_REQ");
	case TR_INFO_ACK:
		return ("TR_INFO_ACK");
	case TR_BIND_ACK:
		return ("TR_BIND_ACK");
	case TR_OK_ACK:
		return ("TR_OK_ACK");
	case TR_ERROR_ACK:
		return ("TR_ERROR_ACK");
	case TR_OPTMGMT_ACK:
		return ("TR_OPTMGMT_ACK");
	case TR_UNI_IND:
		return ("TR_UNI_IND");
	case TR_BEGIN_IND:
		return ("TR_BEGIN_IND");
	case TR_BEGIN_CON:
		return ("TR_BEGIN_CON");
	case TR_CONT_IND:
		return ("TR_CONT_IND");
	case TR_END_IND:
		return ("TR_END_IND");
	case TR_ABORT_IND:
		return ("TR_ABORT_IND");
	case TR_NOTICE_IND:
		return ("TR_NOTICE_IND");
	case TR_ADDR_ACK:
		return ("TR_ADDR_ACK");
	case TR_CAPABILITY_ACK:
		return ("TR_CAPABILITY_ACK");
	case TR_COORD_REQ:
		return ("TR_COORD_REQ");
	case TR_COORD_RES:
		return ("TR_COORD_RES");
	case TR_COORD_IND:
		return ("TR_COORD_IND");
	case TR_COORD_CON:
		return ("TR_COORD_CON");
	case TR_STATE_REQ:
		return ("TR_STATE_REQ");
	case TR_STATE_IND:
		return ("TR_STATE_IND");
	case TR_PCSTATE_IND:
		return ("TR_PCSTATE_IND");
	case TR_TRAFFIC_IND:
		return ("TR_TRAFFIC_IND");
	default:
		return ("TR_???");
	}
}

static char *
sc_primname(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return ("N_CONN_REQ");
	case N_CONN_RES:
		return ("N_CONN_RES");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ");
	case N_DATA_REQ:
		return ("N_DATA_REQ");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case N_INFO_REQ:
		return ("N_INFO_REQ");
	case N_BIND_REQ:
		return ("N_BIND_REQ");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case N_CONN_IND:
		return ("N_CONN_IND");
	case N_CONN_CON:
		return ("N_CONN_CON");
	case N_DISCON_IND:
		return ("N_DISCON_IND");
	case N_DATA_IND:
		return ("N_DATA_IND");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND");
	case N_INFO_ACK:
		return ("N_INFO_ACK");
	case N_BIND_ACK:
		return ("N_BIND_ACK");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK");
	case N_OK_ACK:
		return ("N_OK_ACK");
	case N_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case N_UDERROR_IND:
		return ("N_UDERROR_IND");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ");
	case N_DATACK_IND:
		return ("N_DATACK_IND");
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	case N_NOTICE_IND:
		return ("N_NOTICE_IND");
	case N_INFORM_REQ:
		return ("N_INFORM_REQ");
	case N_INFORM_IND:
		return ("N_INFORM_IND");
	case N_COORD_REQ:
		return ("N_COORD_REQ");
	case N_COORD_RES:
		return ("N_COORD_RES");
	case N_COORD_IND:
		return ("N_COORD_IND");
	case N_COORD_CON:
		return ("N_COORD_CON");
	case N_STATE_REQ:
		return ("N_STATE_REQ");
	case N_STATE_IND:
		return ("N_STATE_IND");
	case N_PCSTATE_IND:
		return ("N_PCSTATE_IND");
	case N_TRAFFIC_IND:
		return ("N_TRAFFIC_IND");
	case N_REQUEST_REQ:
		return ("N_REQUEST_REQ");
	case N_REPLY_ACK:
		return ("N_REPLY_ACK");
	default:
		return ("N_???");
	}
}

static fastcall char *
unix_errname(int error)
{
	switch (err) {
	case EAGAIN:
		return ("EAGAIN");
	case ENOMEM:
		return ("ENOMEM");
	case ENOBUFS:
		return ("ENOBUFS");
	case EBUSY:
		return ("EBUSY");
	case ESRCH:
		return ("ESRCH");
	case ENXIO:
		return ("ENXIO");
	case EMSGSIZE:
		return ("EMSGSIZE");
	case EPROTO:
		return ("EPROTO");
	case EDEADLK:
		return ("EDEADLK");
	default:
		return ("EBAD???");
	}
}
static fastcall char *
tr_errname(t_scalar_t error)
{
	if (err <= 0)
		return unix_errname(-error);
	switch (err) {
	case TRBADADDR:
		return ("TRBADADDR");
	case TRBADOPT:
		return ("TRBADOPT");
	case TRACCESS:
		return ("TRACCESS");
	case TRNOADDR:
		return ("TRNOADDR");
	case TROUTSTATE:
		return ("TROUTSTATE");
	case TRBADSEQ:
		return ("TRBADSEQ");
	case TRSYSERR:
		return ("TRSYSERR");
	case TRBADDATA:
		return ("TRBADDATA");
	case TRBADFLAG:
		return ("TRBADFLAG");
	case TRNOTSUPPORT:
		return ("TRNOTSUPPORT");
	case TRBOUND:
		return ("TRBOUND");
	case TRBADQOSPARAM:
		return ("TRBADQOSPARAM");
	case TRBADQOSTYPE:
		return ("TRBADQOSTYPE");
	case TRBADTOKEN:
		return ("TRBADTOKEN");
	case TRNOPROTOID:
		return ("TRNOPROTOID");
	default:
		return ("TRBAD???");
	}
}
static fastcall char *
sc_errname(np_long error)
{
	if (err <= 0)
		return unix_errname(-error);
	switch (err) {
	case NBADADDR:
		return ("NBADADDR");
	case NBADOPT:
		return ("NBADOPT");
	case NACCESS:
		return ("NACCESS");
	case NNOADDR:
		return ("NNOADDR");
	case NOUTSTATE:
		return ("NOUTSTATE");
	case NBADSEQ:
		return ("NBADSEQ");
	case NSYSERR:
		return ("NSYSERR");
	case NBADDATA:
		return ("NBADDATA");
	case NBADFLAG:
		return ("NBADFLAG");
	case NNOTSUPPORT:
		return ("NNOTSUPPORT");
	case NBOUND:
		return ("NBOUND");
	case NBADQOSPARAM:
		return ("NBADQOSPARAM");
	case NBADQOSTYPE:
		return ("NBADQOSTYPE");
	case NBADTOKEN:
		return ("NBADTOKEN");
	case NNOPROTOID:
		return ("NNOPROTOID");
	default:
		return ("NBAD???");
	}
}

static fastcall const char *
tr_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
	case __SID:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(I_LINK):
			return ("I_LINK");
		case _IOC_NR(I_PLINK):
			return ("I_PLINK");
		case _IOC_NR(I_UNLINK):
			return ("I_UNLINK");
		case _IOC_NR(I_PUNLINK):
			return ("I_PUNLINK");
		default:
			return ("I_????");
		}
	case TCAP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(TCAP_IOCGOPTION):
			return ("TCAP_IOCGOPTION");
		case _IOC_NR(TCAP_IOCSOPTION):
			return ("TCAP_IOCSOPTION");
		case _IOC_NR(TCAP_IOCSCONFIG):
			return ("TCAP_IOCSCONFIG");
		case _IOC_NR(TCAP_IOCGCONFIG):
			return ("TCAP_IOCGCONFIG");
		case _IOC_NR(TCAP_IOCTCONFIG):
			return ("TCAP_IOCTCONFIG");
		case _IOC_NR(TCAP_IOCCCONFIG):
			return ("TCAP_IOCCCONFIG");
		case _IOC_NR(TCAP_IOCLCONFIG):
			return ("TCAP_IOCLCONFIG");
		case _IOC_NR(TCAP_IOCGSTATEM):
			return ("TCAP_IOCGSTATEM");
		case _IOC_NR(TCAP_IOCCMRESET):
			return ("TCAP_IOCCMRESET");
		case _IOC_NR(TCAP_IOCGSTATSP):
			return ("TCAP_IOCGSTATSP");
		case _IOC_NR(TCAP_IOCSSTATSP):
			return ("TCAP_IOCSSTATSP");
		case _IOC_NR(TCAP_IOCGSTATS):
			return ("TCAP_IOCGSTATS");
		case _IOC_NR(TCAP_IOCCSTATS):
			return ("TCAP_IOCCSTATS");
		case _IOC_NR(TCAP_IOCGNOTIFY):
			return ("TCAP_IOCGNOTIFY");
		case _IOC_NR(TCAP_IOCSNOTIFY):
			return ("TCAP_IOCSNOTIFY");
		case _IOC_NR(TCAP_IOCCNOTIFY):
			return ("TCAP_IOCCNOTIFY");
		case _IOC_NR(TCAP_IOCCMGMT):
			return ("TCAP_IOCCMGMT");
		case _IOC_NR(TCAP_IOCCPASS):
			return ("TCAP_IOCCPASS");
		default:
			return ("TCAP_IOC????");
		}
	case SCCP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SCCP_IOCGOPTION):
			return ("SCCP_IOCGOPTION");
		case _IOC_NR(SCCP_IOCSOPTION):
			return ("SCCP_IOCSOPTION");
		case _IOC_NR(SCCP_IOCSCONFIG):
			return ("SCCP_IOCSCONFIG");
		case _IOC_NR(SCCP_IOCGCONFIG):
			return ("SCCP_IOCGCONFIG");
		case _IOC_NR(SCCP_IOCTCONFIG):
			return ("SCCP_IOCTCONFIG");
		case _IOC_NR(SCCP_IOCCCONFIG):
			return ("SCCP_IOCCCONFIG");
		case _IOC_NR(SCCP_IOCLCONFIG):
			return ("SCCP_IOCLCONFIG");
		case _IOC_NR(SCCP_IOCGSTATEM):
			return ("SCCP_IOCGSTATEM");
		case _IOC_NR(SCCP_IOCCMRESET):
			return ("SCCP_IOCCMRESET");
		case _IOC_NR(SCCP_IOCGSTATSP):
			return ("SCCP_IOCGSTATSP");
		case _IOC_NR(SCCP_IOCSSTATSP):
			return ("SCCP_IOCSSTATSP");
		case _IOC_NR(SCCP_IOCGSTATS):
			return ("SCCP_IOCGSTATS");
		case _IOC_NR(SCCP_IOCCSTATS):
			return ("SCCP_IOCCSTATS");
		case _IOC_NR(SCCP_IOCGNOTIFY):
			return ("SCCP_IOCGNOTIFY");
		case _IOC_NR(SCCP_IOCSNOTIFY):
			return ("SCCP_IOCSNOTIFY");
		case _IOC_NR(SCCP_IOCCNOTIFY):
			return ("SCCP_IOCCNOTIFY");
		case _IOC_NR(SCCP_IOCCMGMT):
			return ("SCCP_IOCCMGMT");
		case _IOC_NR(SCCP_IOCCPASS):
			return ("SCCP_IOCCPASS");
		default:
			return ("SCCP_IOC????");
		}
	default:
		return ("???_IOC???");
	}
}

/*
 * OPTION Handling
 */

struct t_tr_options {
	long flags[4];
	struct {
		t_scalar_t debug[4];
		struct t_linger linger;
		t_uscalar_t rcvbuf;
		t_uscalar_t rcvlowat;
		t_uscalar_t sndbuf;
		t_uscalar_t sndlowat;
	} xti;
	struct {
		t_uscalar_t pvar;
		t_uscalar_t mplev;
		t_uscalar_t cluster;
		t_uscalar_t prio;
	} mtp;
	struct {
		t_uscalar_t pvar;
		t_uscalar_t mplev;
		t_uscalar_t cluster;
		t_uscalar_t prio;
		t_uscalar_t pclass;
		t_uscalar_t seqctrl;
		t_uscalar_t imp;
		t_uscalar_t reterr;
		t_uscalar_t credit;
		struct sockaddr_sccp discon_add;
		t_uscalar_t discon_reason;
		t_uscalar_t reset_reason;
		N_qos_sccp_t qos;
	} sccp;
};

struct t_tr_options t_defaults = {
};

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_MTP_PVAR,
	_T_BIT_MTP_MPLEV,
	_T_BIT_MTP_SLS,
	_T_BIT_MTP_MP,
	_T_BIT_SCCP_PVAR,
	_T_BIT_SCCP_MPLEV,
	_T_BIT_SCCP_CLUSTER,
	_T_BIT_SCCP_SEQCTRL,
	_T_BIT_SCCP_PRIO,
	_T_BIT_SCCP_PCLASS,
	_T_BIT_SCCP_IMP,
	_T_BIT_SCCP_RETERR,
	_T_BIT_SCCP_CREDIT,
	_T_BIT_SCCP_DISCON_ADD,
	_T_BIT_SCCP_DISCON_REASON,
	_T_BIT_SCCP_RESET_REASON,
	_T_BIT_SCCP_LAST_FLAG,
};

#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

static fastcall void
t_qos_set_flags(N_qos_sccp_t *qos, ulong *flags)
{
	switch (qos->n_qos_type) {
	case N_QOS_SEL_CONN_SCCP:
		if (qos->sel_conn.protocol_class != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PCLASS, flags);
		if (qos->sel_conn.option_flags != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_RETERR, flags);
		if (qos->sel_conn.sequence_selection != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_SEQCTRL, flags);
		if (qos->sel_conn.message_priority != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PRIO, flags);
		if (qos->sel_conn.importance != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_IMP, flags);
#if 0
		if (qos->sel_conn.credit != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_CREDIT, flags);
#endif
		break;
	case N_QOS_SEL_DATA_SCCP:
		if (qos->sel_data.protocol_class != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PCLASS, flags);
		if (qos->sel_data.option_flags != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_RETERR, flags);
		if (qos->sel_data.sequence_selection != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_SEQCTRL, flags);
		if (qos->sel_data.message_priority != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PRIO, flags);
		if (qos->sel_data.importance != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_IMP, flags);
		break;
	case N_QOS_SEL_INFO_SCCP:
		if (qos->sel_info.protocol_class != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PCLASS, flags);
		if (qos->sel_info.option_flags != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_RETERR, flags);
		if (qos->sel_info.sequence_selection != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_SEQCTRL, flags);
		if (qos->sel_info.message_priority != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PRIO, flags);
		if (qos->sel_info.importance != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_IMP, flags);
#if 0
		if (qos->sel_info.credit != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_CREDIT, flags);
#endif
		break;
	case N_QOS_RANGE_INFO_SCCP:
		if (qos->range_info.protocol_classes != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PCLASS, flags);
		if (qos->range_info.sequence_selection != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_SEQCTRL, flags);
#if 0
		if (qos->range_info.credit != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_CREDIT, flags);
#endif
		break;
	case N_QOS_SEL_INFR_SCCP:
		if (qos->sel_infr.protocol_class != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PCLASS, flags);
		if (qos->sel_infr.option_flags != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_RETERR, flags);
		if (qos->sel_infr.sequence_selection != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_SEQCTRL, flags);
		if (qos->sel_infr.message_priority != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_PRIO, flags);
		if (qos->sel_infr.importance != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_IMP, flags);
#if 0
		if (qos->sel_infr.credit != QOS_UNKNOWN)
			t_set_bit(_T_BIT_SCCP_CREDIT, flags);
#endif
		break;
	default:
		break;
	}
}

/**
 * tn_size_opt: - size options for indication or confirmation primitive
 * @tr: TR private structure
 * @tn: transaction (if any)
 * @qos: quality  of service passed from SCCP
 * @prim: primitive for which to generate options
 */
static inline fastcall size_t
tr_size_opt(struct tr *tr, struct tn *tn, N_qos_sccp_t *qos, const t_scalar_t prim)
{
	size_t size = 0;
	ulong flags[4] = { 0, };

	switch (prim) {
	case TR_UNI_IND:
		t_qos_set_flags(qos, flags);
		break;
	case TR_BEGIN_IND:
	case TR_BEGIN_CON:
		if (tn_get_state(tn) == TRS_WCON_CREQ) {
			/* When the primitive is responsive, also include whatever options were
			   specified when the transaction was created by the user. */
			flags[0] = tn->opt.flags[0];
			flags[1] = tn->opt.flags[1];
			flags[2] = tn->opt.flags[2];
			flags[3] = tn->opt.flags[3];
		}
		t_qos_set_flags(qos, flags);
		break;
	case TR_CONT_IND:
		t_qos_set_flags(qos, flags);
		break;
	case TR_END_IND:
	case TR_ABORT_IND:
		if (tn_get_state(tn) == TRS_WCON_CREQ) {
			/* When the primitive is responsive, also include whatever options were
			   specified when the transaction was created by the user. */
			flags[0] = tn->opt.flags[0];
			flags[1] = tn->opt.flags[1];
			flags[2] = tn->opt.flags[2];
			flags[3] = tn->opt.flags[3];
		}
		t_qos_set_flags(qos, flags);
		break;
	default:
		break;
	}
      none:
	return (size);
}

static size_t
t_size_opts(N_qos_sccp_t *qos)
{
	size_t size = 0;

	if (qos) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_DATA_SCCP:
			if (qos->sel_data.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_data.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_data.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_data.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_data.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
			break;
		case N_QOS_SEL_CONN_SCCP:
			if (qos->sel_conn.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_conn.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_conn.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_conn.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_conn.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
#if 0
			if (qos->sel_conn.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
#endif
			break;
		case N_QOS_SEL_INFO_SCCP:
			if (qos->sel_info.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_info.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_info.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_info.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_info.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
#if 0
			if (qos->sel_info.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
#endif
			break;
		case N_QOS_RANGE_INFO_SCCP:
			if (qos->range_info.protocol_classes != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->range_info.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
#if 0
			if (qos->range_info.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
#endif
			break;
		case N_QOS_SEL_INFR_SCCP:
			if (qos->sel_infr.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_infr.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_infr.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_infr.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_infr.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
#if 0
			if (qos->sel_infr.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
#endif
			break;
		default:
			break;
		}
	}
	return (size);
}

static size_t
t_build_opts(N_qos_sccp_t *qos, unsigned char *op, size_t olen)
{
	struct t_opthdr *oh = (typeof(oh)) op;
	unsigned char *beg;

	if (op == NULL || olen == 0)
		return (0);
	if (qos) {
		oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
		switch (qos->n_qos_type) {
		case N_QOS_SEL_DATA_SCCP:
			if (qos->sel_data.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_data.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_data.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_data.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_data.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_data.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		case N_QOS_SEL_CONN_SCCP:
			if (qos->sel_conn.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_conn.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_conn.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_conn.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_conn.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_conn.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if 0
			if (qos->sel_conn.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->sel_conn.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif
			break;
		case N_QOS_SEL_INFO_SCCP:
			if (qos->sel_info.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_info.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_info.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_info.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_info.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_info.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if 0
			if (qos->sel_info.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->sel_info.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif
			break;
		case N_QOS_RANGE_INFO_SCCP:
			if (qos->range_info.protocol_classes != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->range_info.protocol_classes;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->range_info.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->range_info.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if 0
			if (qos->range_info.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->range_info.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif
			break;
		case N_QOS_SEL_INFR_SCCP:
			if (qos->sel_infr.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_infr.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_infr.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_infr.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_infr.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_infr.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if 0
			if (qos->sel_infr.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->sel_infr.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif
			break;
		default:
			break;
		}
	}
	if (oh == NULL)
		return (olen);
	return ((unsigned char *) oh - op);
      efault:
	return (-EFAULT);
}

/**
 * tn_size_write_opts: - size write options supplied with transcation primitive
 * @tr: TR private structure
 * @tn: transaction structure
 *
 * Returns the size of the options buffer that will be created by a call to tn_write_opts().
 */
static size_t
tn_size_write_opts(struct tr *tr, struct tn *tn)
{
	N_qos_sel_data_sccp_t *qos = &tn->peer.qos;
	int qos_too = 0;
	size_t size = 0;

	if (tn_get_state(tn) == TRS_IDLE && tn->pvar != tr->pvar) {
		size += _T_SPACE_SIZEOF(t_uscalar_t);
	}
	if (qos->protocol_class != QOS_UNKNOWN) {
		size += _T_SPACE_SIZEOF(t_scalar_t);

		qos_too = 1;
	}
	if (qos->option_flags != QOS_UNKNOWN) {
		size += _T_SPACE_SIZEOF(t_scalar_t);

		qos_too = 1;
	}
	if (qos->sequence_control != QOS_UNKNOWN) {
		size += _T_SPACE_SIZEOF(t_scalar_t);

		qos_too = 1;
	}
	if (qos->message_priority != QOS_UNKNOWN) {
		size += _T_SPACE_SIZEOF(t_scalar_t);

		qos_too = 1;
	}
	if (qos->importance != QOS_UNKNOWN) {
		size += _T_SPACE_SIZEOF(t_scalar_t);

		qos_too = 1;
	}
	if (qos_too) {
		size += _T_SPACE_SIZEOF(N_qos_sel_data_sccp_t);
	}

}

/**
 * tn_write_opts: - write options supplied with transaction primitive
 * @tr: TR private structure
 * @tn: transaction structure
 * @op: output option buffer pointer
 * @olen: output option buffer length
 *
 * This function is used to take options received for the transcation and build and option buffer in
 * the extent specified by @op and @olen.  The function returns zero when successful, or a negative
 * error number otherwise.  This is used to build the option buffer for a TR_BEGIN_IND,
 * TR_BEGIN_CON, TR_CONT_IND, TR_END_IND or TR_ABORT_IND primitive.  When the transcation is in the
 * TRS_IDLE state, the protocol variant is also supplied when different from the default protocol
 * variant.
 */
static ptrdiff_t
tn_write_opts(struct tr *tr, struct tn *tn, unsigned char *op, size_t olen)
{
	struct t_opthdr *oh = (typeof(oh)) op;
	N_qos_sel_data_sccp_t *qos = &tn->peer.qos;
	int qos_too = 0;

	if (op == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	if (tn_get_state(tn) == TRS_IDLE && tn->pvar != tr->pvar) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->level = T_SS7_TCAP;
		oh->name = T_TCAP_PVAR;
		oh->status = T_SUCCESS;
		*(t_uscalar_t *) T_OPT_DATA(oh) = tn->pvar;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (qos->protocol_class != QOS_UNKNOWN) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_scalar_t);

		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_PCLASS;
		oh->status = T_SUCCESS;
		*(t_scalar_t *) T_OPT_DATA(oh) = qos->protocol_class;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		qos_too = 1;
	}
	if (qos->option_flags != QOS_UNKNOWN) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_scalar_t);

		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_RETERR;
		oh->status = T_SUCCESS;
	      *(t_scalar_t *) T_OPT_DATA(oh) = (qos->option_flags & 0x01) T_YES:T_NO;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		qos_too = 1;
	}
	if (qos->sequence_selection != QOS_UNKNOWN) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_SEQCTRL;
		oh->status = T_SUCCESS;
		*(t_uscalar_t *) T_OPT_DATA(oh) = qos->sequence_selection;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		qos_too = 1;
	}
	if (qos->message_priority != QOS_UNKNOWN) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_scalar_t);

		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_PRIO;
		oh->status = T_SUCCESS;
		*(t_scalar_t *) T_OPT_DATA(oh) = qos->message_priority;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		qos_too = 1;
	}
	if (qos->importance != QOS_UNKNOWN) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_scalar_t);

		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_IMP;
		oh->status = T_SUCCESS;
		*(t_scalar_t *) T_OPT_DATA(oh) = qos->importance;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		qos_too = 1;
	}
	if (qos_too) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(N_qos_sel_data_sccp_t);

		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_QOS;
		oh->status = T_SUCCESS;
		*(N_qos_sel_data_sccp_t *) T_OPT_DATA(oh) = *qos;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	return (0);
      efault:
	return (-EFAULT);
}

/**
 * tn_read_opts: - read options supplied with transaction primitive
 * @tn: transaction structure
 * @ip: pointer to beginning of options
 * @ilen: length of options
 *
 * This function is used to parse options for TR_BEGIN_REQ, TR_BEGIN_RES, TR_CONT_REQ, TR_END_REQ
 * and TR_ABORT_REQ primitives.  Any value of any option is appropriately stored in the transaction
 * structure.  Note that this only affects local options (not remote options).
 */
static int
tn_read_opts(struct tn *tn, unsigned char *ip, size_t ilen)
{
	struct t_opthdr *ih;
	N_qos_sel_data_sccp_t *qos = &tn->info.qos;

	if (ip == NULL || ilen == 0)
		return (0);
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		switch (ih->level) {
		case T_SS7_TCAP:
			switch (ih->name) {
			case T_TCAP_PVAR:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				/* It is only possible to set the protocol variant for the
				   transction on a transaction by transaction basis when the
				   transaction is idle (has just been created and the initial
				   TR_BEGIN_REQ has not been issued for the transaction. */
				if (tn_get_state(tn) == TRS_IDLE)
					tn->pvar = *valp;
				continue;
			}
			}
			continue;
		case T_SS7_SCCP:
			switch (ih->name) {
			case T_SCCP_PCLASS:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				switch (*valp) {
				case T_UNSPEC:
					qos->protocol_class = QOS_UNKNOWN;
					break;
				case T_SCCP_PCLASS_0:
					qos->protocol_class = N_QOS_PLCASS_0;
					break;
				case T_SCCP_PCLASS_1:
					qos->protocol_class = N_QOS_PCLASS_1;
					break;
				default:
					goto badopt;
				}
				continue;
			}
			case T_SCCP_IMP:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				switch (*valp) {
				case T_UNSPEC:
					qos->importance = QOS_UNKNOWN;
					break;
				default:
					if (*valp > 8)
						goto badopt;
					qos->importance = *valp;
					break;
				}
				continue;
			}
			case T_SCCP_RETERR:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				switch (*valp) {
				case T_UNSPEC:
					qos->option_flags = QOS_UNKNOWN;
					break;
				case T_YES:
					qos->option_flags = 1;
					break;
				case T_NO:
					qos->option_flags = 0;
					break;
				default:
					goto badopt;
				}
				continue;
			}
			case T_SCCP_PRIO:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				switch (*valp) {
				case T_UNSPEC:
					qos->message_priority = QOS_UNKNOWN;
					break;
				case 0:
				case 1:
				case 2:
					qos->message_priority = *valp;
					break;
				default:
					goto badopt;
				}
				continue;
			}
			case T_SCCP_QOS:
			{
				N_qos_sel_data_sccp_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				switch (valp->protocol_class) {
				case QOS_UNKNOWN:
					break;
				case N_QOS_PCLASS_0:
				case N_QOS_PCLASS_1:
					qos->protocol_class = valp->protocol_class;
					break;
				default:
					goto badopt;
				}
				switch (valp->option_flags) {
				case QOS_UNKNOWN:
					break;
				case 0:
				case 1:
					qos->option_flags = valp->option_flags;
					break;
				default:
					goto badopt;
				}
				switch (valp->sequence_selection) {
				case QOS_UNKNOWN:
					break;
				default:
					qos->sequence_selection = valp->sequence_selection;
					break;
				}
				switch (valp->message_priority) {
				case QOS_UNKNOWN:
					break;
				case 0:
				case 1:
				case 2:
					qos->message_priority = valp->message_priority;
					break;
				default:
					goto badopt;
				}
				switch (valp->importance) {
				case QOS_UNKNOWN:
					break;
				default:
					if (valp->importance < 0 || valp->importance > 8)
						goto badopt;
					qos->importance = valp->importance;
					break;
				}
			}
			}
			continue;
		}

	}
	return (0);
      einval:
	return (-EINVAL);
}

struct tr_var {
	uchar *ptr;
	size_t len;
};

struct tr_opts {
	ulong flags;			/* success flags */
	t_scalar_t *pcl;		/* protocol class */
	t_scalar_t *ret;		/* return option */
	t_scalar_t *imp;		/* importance */
	t_scalar_t *seq;		/* sequence control */
	t_scalar_t *pri;		/* priority */
	t_scalar_t *sls;		/* signalling link selection */
	t_scalar_t *mp;			/* message priority */
};

#define TRF_OPT_PCL	(1<<0)	/* protocol class */
#define TRF_OPT_RET	(1<<3)	/* return option */
#define TRF_OPT_IMP	(1<<4)	/* importance */
#define TRF_OPT_SEQ	(1<<5)	/* sequence control */
#define TRF_OPT_PRI	(1<<6)	/* priority */
#define TRF_OPT_SLS	(1<<7)	/* signalling link selection */
#define TRF_OPT_MP	(1<<8)	/* message priority */

#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))

static size_t
tr_opts_size(struct tr *tr, struct tr_opts *ops)
{
	size_t len = 0;

	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */

		if (ops->pcl)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pcl));
		if (ops->ret)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->ret));
		if (ops->imp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->imp));
		if (ops->seq)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->seq));
		if (ops->pri)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pri));
		if (ops->sls)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
		if (ops->mp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
	}
	return (len);
}

static void
tr_build_opts(struct tr *tr, struct tr_opts *ops, uchar *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);

		if (ops->pcl) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->pcl));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = (ops->flags & TRF_OPT_PCL) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->pcl)) p) = *(ops->pcl);
			p += _T_ALIGN_SIZEOF(*ops->pcl);
		}
		if (ops->ret) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->ret));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = (ops->flags & TRF_OPT_RET) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->ret)) p) = *(ops->ret);
			p += _T_ALIGN_SIZEOF(*ops->ret);
		}
		if (ops->imp) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->imp));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = (ops->flags & TRF_OPT_IMP) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->imp)) p) = *(ops->imp);
			p += _T_ALIGN_SIZEOF(*ops->imp);
		}
		if (ops->seq) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->seq));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = (ops->flags & TRF_OPT_SEQ) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->seq)) p) = *(ops->seq);
			p += _T_ALIGN_SIZEOF(*ops->seq);
		}
		if (ops->pri) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->pri));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = (ops->flags & TRF_OPT_PRI) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->pri)) p) = *(ops->pri);
			p += _T_ALIGN_SIZEOF(*ops->pri);
		}
		if (ops->sls) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status = (ops->flags & TRF_OPT_SLS) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status = (ops->flags & TRF_OPT_MP) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
	}
}

static int
tr_parse_opts(struct tr *tr, struct tr_opts *ops, uchar *op, size_t len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_SCCP:
			switch (oh->name) {
			case T_SCCP_PCLASS:
				ops->pcl = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_PCL;
				continue;
			case T_SCCP_RET_ERROR:
				ops->ret = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_RET;
				continue;
			case T_SCCP_IMPORTANCE:
				ops->imp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_IMP;
				continue;
			case T_SCCP_SEQ_CTRL:
				ops->seq = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_SEQ;
				continue;
			case T_SCCP_PRIORITY:
				ops->pri = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_PRI;
				continue;
			}
			break;
		case T_SS7_MTP:
			switch (oh->name) {
			case T_MTP_SLS:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_SLS;
				continue;
			case T_MTP_MP:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TRF_OPT_MP;
				continue;
			}
			break;
			break;
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

static inline int
tr_opt_check(struct tr *tr, struct tr_opts *ops)
{
	if (ops && ops->flags) {
		ops->flags = 0;
		if (ops->pcl)
			ops->flags |= TRF_OPT_PCL;
		if (ops->ret)
			ops->flags |= TRF_OPT_RET;
		if (ops->imp)
			ops->flags |= TRF_OPT_IMP;
		if (ops->seq)
			ops->flags |= TRF_OPT_SEQ;
		if (ops->pri)
			ops->flags |= TRF_OPT_PRI;
		if (ops->sls)
			ops->flags |= TRF_OPT_SLS;
		if (ops->mp)
			ops->flags |= TRF_OPT_MP;
	}
	return (0);
}
static inline int
tr_opt_default(struct tr *tr, struct tr_opts *ops)
{
	if (ops) {
		int flags = ops->flags;

		ops->flags = 0;
		if (!flags || ops->pcl) {
			ops->pcl = &opt_defaults.pcl;
			ops->flags |= TRF_OPT_PCL;
		}
		if (!flags || ops->ret) {
			ops->ret = &opt_defaults.ret;
			ops->flags |= TRF_OPT_RET;
		}
		if (!flags || ops->imp) {
			ops->imp = &opt_defaults.imp;
			ops->flags |= TRF_OPT_IMP;
		}
		if (!flags || ops->seq) {
			ops->seq = &opt_defaults.seq;
			ops->flags |= TRF_OPT_SEQ;
		}
		if (!flags || ops->pri) {
			ops->pri = &opt_defaults.pri;
			ops->flags |= TRF_OPT_PRI;
		}
		if (!flags || ops->sls) {
			ops->sls = &opt_defaults.sls;
			ops->flags |= TRF_OPT_SLS;
		}
		if (!flags || ops->mp) {
			ops->mp = &opt_defaults.mp;
			ops->flags |= TRF_OPT_MP;
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
static inline int
tr_opt_current(struct tr *tr, struct tr_opts *ops)
{
	int flags = ops->flags;

	ops->flags = 0;
	if (!flags || ops->pcl) {
		ops->pcl = &tr->options.pcl;
		ops->flags |= TRF_OPT_PCL;
	}
	if (!flags || ops->ret) {
		ops->ret = &tr->options.ret;
		ops->flags |= TRF_OPT_RET;
	}
	if (!flags || ops->imp) {
		ops->imp = &tr->options.imp;
		ops->flags |= TRF_OPT_IMP;
	}
	if (!flags || ops->seq) {
		ops->seq = &tr->options.seq;
		ops->flags |= TRF_OPT_SEQ;
	}
	if (!flags || ops->pri) {
		ops->pri = &tr->options.pri;
		ops->flags |= TRF_OPT_PRI;
	}
	if (!flags || ops->sls) {
		ops->sls = &tr->options.sls;
		ops->flags |= TRF_OPT_SLS;
	}
	if (!flags || ops->mp) {
		ops->mp = &tr->options.mp;
		ops->flags |= TRF_OPT_MP;
	}
	return (0);
}
static inline int
tr_opt_negotiate(struct tr *tr, struct tr_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->pcl) {
			tr->options.pcl = *ops->pcl;
			ops->pcl = &tr->options.pcl;
			ops->flags |= TRF_OPT_PCL;
		}
		if (ops->ret) {
			tr->options.ret = *ops->ret;
			ops->ret = &tr->options.ret;
			ops->flags |= TRF_OPT_RET;
		}
		if (ops->imp) {
			tr->options.imp = *ops->imp;
			ops->imp = &tr->options.imp;
			ops->flags |= TRF_OPT_IMP;
		}
		if (ops->seq) {
			tr->options.seq = *ops->seq;
			ops->seq = &tr->options.seq;
			ops->flags |= TRF_OPT_SEQ;
		}
		if (ops->pri) {
			tr->options.pri = *ops->pri;
			ops->pri = &tr->options.pri;
			ops->flags |= TRF_OPT_PRI;
		}
		if (ops->sls) {
			tr->options.sls = *ops->sls;
			ops->sls = &tr->options.sls;
			ops->flags |= TRF_OPT_SLS;
		}
		if (ops->mp) {
			tr->options.mp = *ops->mp;
			ops->mp = &tr->options.mp;
			ops->flags |= TRF_OPT_MP;
		}
	}
	return (0);
}

/*
 * TR-Provider -> TR-User primitives.
 */

static fastcall int
m_error_reply(struct tr *tr, queue_t *q, mblk_t *msg, int rerror, int werror)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_HI)))) {
		DB_TYPE(mp) = M_ERROR;
		mp->b_wptr[0] = rerror;
		mp->b_wptr[1] = werror;
		mp->b_wptr += 2;
		tr_set_state(tr, -1);
		freemsg(msg);
		flushq(tr->wq, FLUSHALL);
		flushq(tr->rq, FLUSHALL);
		LOGTX(tr, "<- M_ERROR(%s,%s)", unix_errname(rerror), unix_errname(werror));
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_info_ack(struct tr *tr, queue_t *q, mblk_t *msg)
{
	struct TR_info_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_INFO_ACK;
		p->TSDU_size = tr->info.TSDU_size;
		p->ETSDU_size = tr->info.ETSDU_size;
		p->CDATA_size = tr->info.CDATA_size;
		p->DDATA_size = tr->info.DDATA_size;
		p->ADDR_size = tr->info.ADDR_size;
		p->OPT_size = tr->info.OPT_size;
		p->TIDU_size = tr->info.TIDU_size;
		p->SERV_type = tr->info.SERV_type;
		p->CURRENT_state = tr->info.CURRENT_state;
		p->PROVIDER_flag = tr->info.PROVIDER_flag;
		p->TRPI_version = tr->info.TRPI_version;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tr, "<- TR_INFO_ACK");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_bind_ack(struct tr *tr, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct TR_bind_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BIND_ACK;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->XACT_number = tr->bind.XACT_number;
		p->BIND_flags = tr->bind.BIND_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		tr_set_state(tr, TRS_IDLE);
		LOGTX(tr, "<- TR_BIND_ACK");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_ok_reply(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg, t_uscalar_t CORRECT_prim,
	    mblk_t *mp)
{
	struct TR_ok_ack *p = (typeof(p)) mp->b_rptr;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	p->TRANS_id = tn ? tn->info.TRANS_id : 0;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	if (tn) {
		switch (tn_get_state(tn)) {
		case TRS_WACK_CREQ:
			tn_set_state(tn, TRS_WCON_CREQ);
			break;
		case TRS_WACK_CRES:
			tn_set_state(tn, TRS_DATA_XFER);
			break;
		case TRS_WACK_DREQ6:
		case TRS_WACK_DREQ7:
		case TRS_WACK_DREQ9:
			tn_set_state(tn, TRS_IDLE);
			/* tn destroyed */
			break;
		}
	} else {
		switch (tr_get_state(tr)) {
		case TRS_WACK_UREQ:
			tr_set_state(tr, TRS_UNBND);
			break;
		case TRS_WACK_OPTREQ:
			tr_set_state(tr, TRS_IDLE);
			break;
		}
	}
	LOGTX(tr, "<- TR_OK_ACK");
	putnext(tr->rq, mp);
	return (0);
}
static fastcall int
tr_ok_ack(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg, t_uscalar_t CORRECT_prim)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(struct T_ok_ack), BPRI_MED))))
		return tr_ok_reply(tr, tn, q, msg, CORRECT_prim, mp);
	return (-ENOBUFS);
}
static fastcall int
tr_error_ack(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg, t_uscalar_t ERROR_prim,
	     t_scalar_t error, const char *func)
{
	struct TR_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_ERROR_ACK;
		p->ERROR_prim = ERROR_prim;
		p->TRPI_error = error < 0 ? TRSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		p->TRANS_id = tn ? tn->info.TRANS_id : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		if (tn) {
			switch (tn_get_state(tn)) {
			case TRS_WACK_CREQ:
				tn_set_state(tn, TRS_IDLE);
				/* destroys transaction */
				break;
			case TRS_WACK_CRES:
				tn_set_state(tn, TRS_WRES_CIND);
				break;
			case TRS_WACK_DREQ6:
				tn_set_state(tn, TRS_WCON_CREQ);
				break;
			case TRS_WACK_DREQ7:
				tn_set_state(tn, TRS_WRES_CIND);
				break;
			case TRS_WACK_DREQ9:
				tn_set_state(tn, TRS_DATA_XFER);
				break;
			case TRS_IDLE:
				tn_destroy(tn);
				break;
			}
		} else {
			switch (tr_get_state(tr)) {
			case TRS_WACK_BREQ:
				tr_set_state(tr, TRS_UNBND);
				break;
			case TRS_WACK_UREQ:
				tr_set_state(tr, TRS_IDLE);
				break;
			case TRS_WACK_OPTREQ:
				tr_set_state(tr, TRS_IDLE);
				break;
			}
		}
		LOGNO(tr, "%s: %s: %s: %s", func, tr_primname(ERROR_prim), tr_errname(error),
		      tr_errstring(error));
		LOGTX(tr, "<- TR_ERROR_ACK");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_error_reply(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg, t_uscalar_t ERROR_prim,
	       t_scalar_t error, const char *func)
{
	/* These errors feed back to the calling put or service procedure. */
	switch (error) {
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
	case 0:
		return (error);
	}
	/* For primitives for which there is no TR_ERROR_ACK primitive possible, then the TRI user
	   is send an M_ERROR message instead. */
	switch (ERROR_prim) {
	case TR_CONT_REQ:
		goto merror_data;
	}
	/* If the interface is an a waiting for acknowledgement state, then the TRI user has
	   violated the interface protocol and desrerves an M_ERROR message. */
	switch (tr_get_state(tr)) {
	case TRS_WACK_BREQ:
	case TRS_WACK_UREQ:
	case TRS_WACK_OPTREQ:
		goto merror;
	}
	if (tn) {
		switch (tn_get_state(tn)) {
		case TRS_WACK_CREQ:
		case TRS_WACK_CRES:
		case TRS_WACK_DREQ6:
		case TRS_WACK_DREQ7:
		case TRS_WACK_DREQ9:
			goto merror;
		}
	}
	/* Otherwise, return a regular TR_ERROR_ACK. */
	return tr_error_ack(tr, tn, q, msg, ERROR_prim, func);
      merror_data:
	LOGDA(tr, "ERROR: %s: %s: %s", func, tr_errname(error), tr_strerror(error));
	return m_error_reply(tr, q, msg, EPROTO, EPROTO);
      merror:
	LOGERR(tr, "ERROR: %s: %s: %s", func, tr_errname(error), tr_strerror(error));
	return m_error_reply(tr, q, msg, EPROTO, EPROTO);
}
static fastcall int
tr_optmgmt_ack(struct tr *tr, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, t_uscalar_t flags)
{
	struct TR_optmgmt_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_OPTMGMT_ACK;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		if (tr_get_state(tr) == TRS_WACK_OPTREQ)
			tr_set_state(tr, TRS_IDLE);
		LOGTX(tr, "<- TR_OPTMGMT_ACK");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_uni_ind(struct tr *tr, queue_t *q, mblk_t *msg, caddr_t dptr, size_t dlen, caddr_t sptr,
	   size_t slen, caddr_t optr, size_t olen, mblk_t *dp, uchar *beg, uchar *end)
{
	struct TR_uni_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen + olen;

	if (unlikely(!canputnext(tr->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_UNI_IND;
		p->DEST_length = dlen;
		p->DEST_offset = sizeof(*p);
		p->ORIG_length = slen;
		p->ORIG_offset = p->DEST_offset + dlen;
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + slen;
		mp->b_wptr += sizeof(*p);
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		dp->b_rptr = beg;
		dp->b_wptr = end;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* no state change */
		LOGDA(tr, "<- TR_UNI_IND");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_begin_ind(struct tr *tr, queue_t *q, mblk_t *msg, struct tn *tn, caddr_t dptr, size_t dlen,
	     caddr_t sptr, size_t slen, caddr_t optr, size_t olen, mblk_t *dp, uchar *beg,
	     uchar *end)
{
	struct TR_begin_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen + olen;

	if (likely(canputnext(tr->rq))) {
		if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_BEGIN_IND;
			p->TRANS_id = tn->info.TRANS_id;
			p->ASSOC_flags = tn->info.ASSOC_flags;
			p->DEST_length = dlen;
			p->DEST_offset = sizeof(*p);
			p->ORIG_length = slen;
			p->ORIG_offset = p->DEST_offset + p->DEST_length;
			p->OPT_length = olen;
			p->OPT_offset = p->ORIG_offset + p->ORIG_length;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			bcopy(sptr, mp->b_wptr, slen);
			mp->b_wptr += slen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			dp->b_rptr = beg;
			dp->b_wptr = end;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tn_set_state(tn, TRS_WRES_CIND);
			LOGDA(tr, "<- TR_BEGIN_IND");
			putnext(tr->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
static fastcall int
tr_begin_con(struct tr *tr, queue_t *q, mblk_t *msg, struct tn *tn, caddr_t sptr, size_t slen,
	     caddr_t optr, size_t olen, mblk_t *dp, uchar *beg, uchar *end)
{
	struct TR_begin_con *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + slen + olen;

	if (likely(canputnext(tr->rq))) {
		if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_BEGIN_CON;
			p->TRANS_id = tn->info.TRANS_id;
			p->ASSOC_flags = tn->info.ASSOC_flags;
			p->ORIG_length = slen;
			p->ORIG_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = p->ORIG_offset + p->ORIG_length;
			mp->b_wptr += sizeof(*p);
			bcopy(sptr, mp->b_wptr, slen);
			mp->b_wptr += slen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			dp->b_rptr = beg;
			dp->b_wptr = end;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tn_set_state(tn, TRS_DATA_XFER);
			LOGDA(tr, "<- TR_BEGIN_CON");
			putnext(tr->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
static fastcall int
tr_cont_ind(struct tr *tr, queue_t *q, mblk_t *msg, struct tn *tn, caddr_t optr, size_t olen,
	    mblk_t *dp, uchar *beg, uchar *end)
{
	struct TR_cont_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(canputnext(tr->rq))) {
		if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_CONT_IND;
			p->TRANS_id = tn->info.TRANS_id;
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p);
			p->ASSOC_flags = tn->info.ASSOC_flags;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			dp->b_rptr = beg;
			dp->b_wptr = end;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			/* no state change */
			LOGDA(tr, "<- TR_CONT_IND");
			putnext(tr->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
static fastcall int
tr_end_ind(struct tr *tr, queue_t *q, mblk_t *msg, struct tn *tn, caddr_t sptr, size_t slen,
	   caddr_t optr, size_t olen, mblk_t *dp, uchar *beg, uchar *end)
{
	struct TR_end_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen + slen;

	if (likely(!canputnext(tr->rq))) {
		if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_END_IND;
			p->TRANS_id = tn->info.TRANS_id;
			p->ORIG_length = slen;
			p->ORIG_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p) + slen;
			mp->b_wptr += sizeof(*p);
			bcopy(sptr, mp->b_wptr, slen);
			mp->b_wptr += slen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			dp->b_rptr = beg;
			dp->b_wptr = end;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tn_set_state(tn, TRS_IDLE);
			LOGDA(tr, "<- TR_END_IND");
			putnext(tr->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
static fastcall int
tr_abort_ind(struct tr *tr, queue_t *q, mblk_t *msg, struct tn *tn, t_uscalar_t ABORT_cause,
	     t_uscalar_t ORIGINATOR, caddr_t optr, size_t olen, mblk_t *dp, uchar *beg, uchar *end)
{
	struct TR_abort_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(canputnext(tr->rq))) {
		if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_ABORT_IND;
			p->TRANS_id = tn->info.TRANS_id;
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p);
			/* When the data field contains an [APPLICATION 10] tag it is an ITU-T
			   P-abort-cause.  When it contains an [APPLICATION 11] tag it is an ITU-T
			   DialoguePortion. When the data field contains [PRIVATE 23] it is an ANSI 
			   P-abort-cause.  When it contains [PRIVATE 25] it is an ANSI dialogue
			   portion. */
			p->ABORT_cause = ABORT_cause;
			p->ORIGINATOR = ORIGINATOR;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			dp->b_rptr = beg;
			dp->b_wptr = end;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tn_set_state(tn, TRS_IDLE);
			/* transaction destroyed */
			LOGDA(tr, "<- TR_ABORT_IND");
			putnext(tr->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
static fastcall int
tr_notice_ind(struct tr *tr, queue_t *q, mblk_t *msg, struct tn *tn, caddr_t dptr, size_t dlen,
	      caddr_t sptr, size_t slen, t_uscalar_t REPORT_cause, mblk_t *dp)
{
	struct TR_notice_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen;

	if (likely(bcanputnext(tr->rq, 1))) {
		if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_NOTICE_IND;
			p->TRANS_id = tn ? tn->info.TRANS_id : 0;
			p->DEST_length = dlen;
			p->DEST_offset = sizeof(*p);
			p->ORIG_length = olen;
			p->ORIG_offset = p->DEST_offset + p->DEST_length;
			p->REPORT_cause = REPORT_cause;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			bcopy(sptr, mp->b_wptr, slen);
			mp->b_wptr += slen;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			/* no state change */
			LOGDA(tr, "<- TR_NOTICE_IND");
			putnext(tr->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
static fastcall int
tr_addr_ack(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg)
{
	caddr_t lptr, rptr;
	size_t llen, rlen;
	struct TR_addr_ack *p;
	mblk_t *mp;
	size_t mlen;

	if (unlikely(!canputnext(tr->rq)))
		return (-EBUSY);
	if (tn != NULL) {
		lptr = (caddr_t) &tn->info + tn->info.ADDR_offset;
		llen = tn->info.ADDR_length;
		rptr = (caddr_t) &tn->peer + tn->peer.ADDR_offset;
		rlen = tn->peer.ADDR_length;
	} else {
		lptr = (caddr_t) &tr->bind + tr->bind.ADDR_offset;
		llen = tr->bind.ADDR_length;
		rptr = NULL;
		rlen = 0;
	}
	mlen = sizeof(*p) + llen + rlen;
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_ADDR_ACK;
		p->LOCADDR_length = llen;
		p->LOCADDR_offset = sizeof(*p);
		p->REMADDR_length = rlen;
		p->REMADDR_offset = sizeof(*p) + llen;
		mp->b_wptr += sizeof(*p);
		bcopy(lptr, mp->b_wptr, llen);
		mp->b_wptr += llen;
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		freemsg(msg);
		LOGDA(tr, "<- TR_ADDR_ACK");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_coord_ind(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t smi, caddr_t aptr, size_t alen)
{
	struct TR_coord_ind *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_COORD_IND;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->SMI = smi;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tr, "<- TR_COORD_IND");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_coord_con(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t smi, caddr_t aptr, size_t alen)
{
	struct TR_coord_con *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_COORD_CON;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->SMI = smi;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tr, "<- TR_COORD_CON");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_state_ind(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t status, t_uscalar_t smi,
	     caddr_t aptr, size_t alen)
{
	struct TR_state_ind *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_STATE_IND;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->STATUS = status;
		p->SMI = smi;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tr, "<- TR_STATE_IND");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_pcstate_ind(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t status, caddr_t aptr,
	       size_t alen)
{
	struct TR_pcstate_ind *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_PCSTATE_IND;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->STATUS = status;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tr, "<- TR_PCSTATE_IND");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_traffic_ind(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t tmix, caddr_t aptr, size_t alen)
{
	struct TR_traffic_ind *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_TRAFFIC_IND;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->TRAFFIC_mix = tmix;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tr, "<- TR_TRAFFIC_IND");
		putnext(tr->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * NS-User -> NS-Provider primitives (Connectionless service).
 */
static fastcall int
n_info_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(sc, "N_INFO_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_bind_req(struct sc *sc, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, caddr_t pptr,
	   size_t plen)
{
	N_bind_req_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen + plen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_BIND_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = 0;
		p->BIND_flags = 0;
		p->PROTOID_length = plen;
		p->PROTOID_offset = sizeof(*p) + alen;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		bcopy(pptr, mp->b_wptr, plen);
		mp->b_wptr += plen;
		freemsg(msg);
		n_set_state(sc, NS_WACK_BREQ);
		LOGTX(sc, "N_BIND_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_unbind_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		n_set_state(sc, NS_WACK_UREQ);
		LOGTX(sc, "N_UNBIND_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_optmgmt_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong flags, caddr_t qptr, size_t qlen)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + qlen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_OPTMGMT_REQ;
		p->QOS_length = qlen;
		p->QOS_offset = sizeof(*p);
		p->OPTMGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(qptr, mp->b_wptr, qlen);
		mp->b_wptr += qlen;
		freemsg(msg);
		if (n_get_state(sc) == NS_IDLE)
			n_set_state(sc, NS_WACK_OPTREQ);
		LOGDA(sc, "N_OPTMGMT_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_unitdata_req(struct sc *sc, struct tn *tn, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;
	caddr_t dptr = (caddr_t) &tn->peer + tn->peer.ADDR_offset;
	size_t dlen = tn->peer.ADDR_length;
	caddr_t sptr = (caddr_t) &tn->info + tn->info.ADDR_offset;
	size_t slen = tn->info.ADDR_length;
	caddr_t qptr = (caddr_t) &tn->info.qos;
	size_t qlen = sizeof(tn->info.qos);
	const size_t mlen = sizeof(*p) + dlen + slen;
	int err;

	if (unlikely(!canputnext(sc->wq)))
		return (-EBUSY);
	if (qlen && (err = n_optmgmt_req(sc, q, NULL, 0, qptr, qlen)))
		return (err);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_UNITDATA_REQ;
		p->DEST_length = dlen;
		p->DEST_offset = sizeof(*p);
		p->RESERVED_field[0] = slen;
		p->RESERVED_field[1] = sizeof(*p) + dlen;
		mp->b_wptr += sizeof(*p);
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		mp->b_cont = dp;
		if (msg)
			freeb(msg);
		LOGDA(sc, "N_UNITDATA_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_coord_req(struct sc *sc, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	N_coord_req_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_COORD_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(sc, "N_COORD_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_coord_res(struct sc *sc, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	N_coord_res_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_COORD_RES;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(sc, "N_COORD_RES ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
n_state_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong status, caddr_t aptr, size_t alen)
{
	N_state_req_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type N_STATE_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->STATUS = status;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(sc, "N_STATE_REQ ->");
		putnext(sc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * PROTOCOL STATE MACHINE ENCODING AND DECODING FUNCTIONS
 */

/* Message Types */
#define	TCAP_MT_UNI		 1	/* Unidirectional */
#define	TCAP_MT_QWP		 2	/* Query w/ permission */
#define	TCAP_MT_QWOP		 3	/* Query w/o permission */
#define	TCAP_MT_CWP		 4	/* Conversation w/ permission */
#define	TCAP_MT_CWOP		 5	/* Conversation w/o permission */
#define	TCAP_MT_RESP		 6	/* Response */
#define	TCAP_MT_ABORT		 7	/* Abort */

/* Component Types */
#define	TCAP_CT_INVOKE_L	 1	/* Invoke (Last) */
#define	TCAP_CT_INVOKE_NL	 2	/* Invoke (Not Last) */
#define	TCAP_CT_RESULT_L	 3	/* Return Result (Last) */
#define	TCAP_CT_RESULT_NL	 4	/* Return Result (Not Last) */
#define	TCAP_CT_REJECT		 5	/* Reject */
#define	TCAP_CT_ERROR		 6	/* Error */

#define	TCAP_TCAP_ACG		 1	/* TCAP ACG Indicators */
#define	TCAP_TCAP_STA		 2	/* TCAP Standard Announcement */
#define TCAP_TCAP_CUA		 3	/* TCAP Customized Announcment */
#define	TCAP_TCAP_TDIG		 4	/* TCAP Digits */
#define	TCAP_TCAP_SUEC		 5	/* TCAP Standard User Error Code */
#define	TCAP_TCAP_PDTA		 6	/* TCAP Problem Data */
#define	TCAP_TCAP_TCGPA		 7	/* TCAP SCCP Calling Party Address */
#define	TCAP_TCAP_TRSID		 8	/* TCAP Transaction ID */
#define	TCAP_TCAP_PCTY		 9	/* TCAP Package Type */
#define	TCAP_TCAP_SKEY		10	/* TCAP Service Key (Constructor) */
#define	TCAP_TCAP_BISTAT	11	/* TCAP Busy/Idle Status */
#define	TCAP_TCAP_CFSTAT	12	/* TCAP Call Forwarding Status */
#define	TCAP_TCAP_ORIGR		13	/* TCAP Origination Restrictions */
#define	TCAP_TCAP_TERMR		14	/* TCAP Terminating Restrictions */
#define	TCAP_TCAP_DNMAP		15	/* TCAP DN to Line Service TYpe Mapping */
#define	TCAP_TCAP_DURTN		16	/* TCAP Duration */
#define	TCAP_TCAP_RETD		17	/* TCAP Return Data (Constructor) */
#define	TCAP_TCAP_BCRQ		18	/* TCAP Bearer Capability Requested */
#define	TCAP_TCAP_BCSUP		19	/* TCAP Bearer Capability Supported */
#define	TCAP_TCAP_REFID		20	/* TCAP Reference Id */
#define	TCAP_TCAP_BGROUP	21	/* TCAP Business Group */
#define	TCAP_TCAP_SNI		22	/* TCAP Signalling Networks Identifier */
#define	TCAP_TCAP_MWIT		23	/* TCAP Message Waiting Indicator Type */

#define	TCAP_PRIV_UNI		 1	/* ANSI Unidirectional */
#define	TCAP_PRIV_QWP		 2	/* ANSI Query w/ permission */
#define	TCAP_PRIV_QWOP		 3	/* ANSI Query w/o permission */
#define	TCAP_PRIV_RESP		 4	/* ANSI Response */
#define	TCAP_PRIV_CWP		 5	/* ANSI Conversaion w/ permission */
#define	TCAP_PRIV_CWOP		 6	/* ANSI Conversaion w/o permission */
#define	TCAP_PRIV_TRSID		 7	/* ANSI Transaction Id */
#define	TCAP_PRIV_CSEQ		 8	/* ANSI Component Sequence */
#define	TCAP_PRIV_INKL		 9	/* ANSI Invoke (Last) */
#define	TCAP_PRIV_RRL		10	/* ANSI Return Result (Last) */
#define	TCAP_PRIV_RER		11	/* ANSI Return Error */
#define	TCAP_PRIV_REJ		12	/* ANSI Reject */
#define	TCAP_PRIV_INK		13	/* ANSI Invoke (Not Last) */
#define	TCAP_PRIV_RR		14	/* ANSI Result (Not Last) */
#define	TCAP_PRIV_CORID		15	/* ANSI Correlation Id(s) */
#define	TCAP_PRIV_NOPCO		16	/* ANSI National Operation Code */
#define	TCAP_PRIV_POPCO		17	/* ANSI Private Operation Code */
#define	TCAP_PRIV_PSET		18	/* ANSI Parameter Set */
#define TCAP_PRIV_NECODE	19	/* ANSI National Error Code */
#define TCAP_PRIV_PECODE	20	/* ANSI Private Error Code */
#define	TCAP_PRIV_PBCODE	21	/* ANSI Reject Problem Code */
#define	TCAP_PRIV_PSEQ		21	/* ANSI Parameter Sequence */
#define	TCAP_PRIV_ABORT		22	/* ANSI Abort */
#define	TCAP_PRIV_PCAUSE	23	/* ANSI P-Abort Cause */
#define	TCAP_PRIV_U_ABORT	24	/* ANSI User Abort Information */
#define	TCAP_PRIV_DLGP		25	/* ANSI Dialog Portion */
#define	TCAP_PRIV_VERSION	26	/* ANSI Protocol Version */
#define	TCAP_PRIV_CONTEXT	27	/* ANSI Integer Application Context */
#define	TCAP_PRIV_CTX_OID	28	/* ANSI OID Application Context */
#define	TCAP_PRIV_UINFO		29	/* ANSI User Information */

#define TCAP_APPL_UNI		 1	/* ITUT Unidirectional */
#define TCAP_APPL_BEGIN		 2	/* ITUT Begin Transaction */
#define TCAP_APPL_END		 4	/* ITUT End Transaction */
#define TCAP_APPL_CONT		 5	/* ITUT Continue Transaction */
#define TCAP_APPL_ABORT		 7	/* ITUT Abort Transaction */
#define TCAP_APPL_ORIGID	 8	/* ITUT Origination Transaction Id */
#define TCAP_APPL_DESTID	 9	/* ITUT Destination Transaction Id */
#define TCAP_APPL_PCAUSE	10	/* ITUT P-Abort Cause */
#define TCAP_APPL_DLGP		11	/* ITUT Dialog Portion */
#define TCAP_APPL_CSEQ		12	/* ITUT Component Portion */

/* Dialogue PDUs */
#define TCAP_APPL_AUDT		 0	/* ITUT AUDT PDU */
#define TCAP_APPL_AARQ		 0	/* ITUT AARQ PDU */
#define TCAP_APPL_AARE		 1	/* ITUT AARE PDU */
#define TCAP_APPL_RLRQ		 2	/* ITUT RLRQ PDU */
#define TCAP_APPL_RLRE		 3	/* ITUT RLRE PDU */
#define TCAP_APPL_ABRT		 4	/* ITUT ABRT PDU */

#define TCAP_CNTX_LID		 0	/* Linked Id */
#define TCAP_CNTX_INK		 1	/* Invoke */
#define TCAP_CNTX_RRL		 2	/* Return Result (Last) */
#define TCAP_CNTX_RER		 3	/* Return Error */
#define TCAP_CNTX_REJ		 4	/* Reject */
#define TCAP_CNTX_RR		 7	/* Return Result (Not Last) */

#define TCAP_CNTX_ASN1		 0	/* Single ASN.1 Type */

#define ASN_PRIM		0x00	/* form 0 0000 0000 */
#define ASN_CONS		0x20	/* form 1 0010 0000 */

#define ASN_UNIV		0x00	/* class 0 0000 0000 */
#define ASN_APPL		0x40	/* class 1 0100 0000 */
#define ASN_CNTX		0x80	/* class 2 1000 0000 */
#define ASN_PRIV		0xc0	/* class 3 1100 0000 */

#define ASN_UNIV_PRIM		(ASN_UNIV|ASN_PRIM)	/* univ prim - class 0 form 0 - 0000 0000 */
#define ASN_UNIV_CONS		(ASN_UNIV|ASN_CONS)	/* univ cons - class 0 form 1 - 0010 0000 */
#define ASN_APPL_PRIM		(ASN_APPL|ASN_PRIM)	/* appl prim - class 1 form 0 - 0100 0000 */
#define ASN_APPL_CONS		(ASN_APPL|ASN_CONS)	/* appl cons - class 1 form 1 - 0110 0000 */
#define ASN_CNTX_PRIM		(ASN_CNTX|ASN_PRIM)	/* cntx prim - class 2 form 0 - 1000 0000 */
#define ASN_CNTX_CONS		(ASN_CNTX|ASN_CONS)	/* cntx cons - class 2 form 1 - 1010 0000 */
#define ASN_PRIV_PRIM		(ASN_PRIV|ASN_PRIM)	/* priv prim - class 3 form 0 - 1100 0000 */
#define ASN_PRIV_CONS		(ASN_PRIV|ASN_CONS)	/* priv cons - class 3 form 1 - 1110 0000 */

#define ASN_UNIV_EOC		 0	/* UNIV End of Contents */
#define ASN_UNIV_INT		 2	/* UNIV Integer */
#define ASN_UNIV_OSTR		 4	/* UNIV Octet String */
#define ASN_UNIV_OID		 6	/* UNIV Object Id */
#define ASN_UNIV_EXT		 8	/* UNIV External */
#define	ASN_UNIV_PSEQ		16	/* UNIV Parameter Sequence */
#define ASN_UNIV_UTC		17	/* UNIV Universal Time */
#define ASN_UNIV_SEQ		48	/* UNIV Sequence */

struct asn_val {
	uint cls;			/* class */
	uint tag;			/* tag */
	uint len;			/* length of value */
	uint val;			/* unpacked value when appropriate */
	uchar *tagp;			/* pointer to the tag */
	uchar *lenp;			/* pointer to the len */
	uchar *valp;			/* pointer to the val */
	uchar *endp;			/* pointer to the end */
};

static noinline fastcall __unlikely int
asn_unpack_tag_extended(struct asn_val *v)
{
	uchar ptag;
	uint octets = 0;

	do {
		v->tag <<= 7;
		ptag = *v->lenp++;
		v->tag |= (ptag & 0x7f);
		octets++;
	}
	while ((ptag & 0x80) && v->lenp < v->endp && octets < 6);
	if (unlikely(v->lenp >= v->endp))
		return (-EMSGSIZE);
	if (unlikely(octets > 5))
		return (-EPROTO);
	return (0);
}

/**
 * asn_unpack_len_extended: - upack extended length
 * @v: ASN encoding
 *
 * Upon entry @v->tagp points to the tag, @v->tag and @v->cls are valid, @v->valp points to the
 * second byte of the length field, @v->len contains 0x8X, the first octet of the length field,
 * @v->endp points to the enclosing envelope.
 */
static noinline fastcall __unlikely int
asn_unpack_len_extended(struct asn_val *v)
{
	uint plen = (v->len & 0x7f);
	uint octets = 0;
	int err;

	if (unlikely(plen == 0))
		goto efault;	/* use other function for indefinite length */
	v->len = 0;
	while (plen-- && v->valp < v->endp && octets < 5) {
		v->len <<= 8;
		v->len |= *v->valp++;
		octets++;
	}
	if (unlikely(v->endp - v->valp < v->len))
		goto emsgsize;
	if (unlikely(octets > 4))
		goto eproto;
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      eproto:
	err = -EPROTO;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * asn_unpack_len_indefinite: - unpack indefinite length
 * @v: ASN encoding
 *
 * Upon entry @v->tagp points to the tag, @v->tag and @v->cls are valid, @v->valp points to the
 * value, and @v->endp points to the end of the enclosing envelope.  Upon successful result,
 * @v->endp points to the first EOC octet, describing the new envelope and @v->len is valid.
 *
 * For the indefinite length we already have the envelope, so the length must be the contents minus
 * the end of contents octet at the end of the envelope.  If there is no end of contents octets at
 * the end of the envelope, then it is an error.  Note that indefinite length is only valid for
 * constructed types and not primitive types.  This is not true for an indefinite length inside an
 * indefinite length sequence; however, the only indefinite length that we encounter is for the
 * transaction portion, the contents of the dialogue portion and component sequence are passed on
 * unanalyzed.
 */
static noinline fastcall __unlikely int
asn_unpack_len_indefinite(struct asn_val *v)
{
	int err;

	if ((v->cls & 0x20) != 0)
		goto eproto;	/* primitive */
	if (v->endp < v->valp + 2)
		goto einval;
	if (*v->endp-- != 0 || *v->endp-- != 0)
		goto einval;
	v->len = v->endp - v->valp;
	return (0);
      eproto:
	err = -EPROTO;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return (err);
}

/**
 * asn_unpack_taglen: - unpack class, tag and length and define value envelope
 * @v: ASN encoding
 * @b: beg of envelope
 * @e: end of envelope
 *
 * Upon entry @v->tagp must point to the first byte of the encoding and @v->endp must point to the
 * last byte of the encoding.  Upon successful result (0 returned), @v->tag, @v->cls, @v->len are
 * valid, and @v->tagp points to the first tag octet, @v->lenp points to the first length octet,
 * @v->valp points to the first value octet, and @v->endp points one past the last value octet.
 */
static inline fastcall int
asn_unpack_taglen(struct asn_val *v, uchar *b, uchar *e)
{
	uint tac;
	int err;

	v->tagp = v->lenp = b;
	v->endp = e;
	if (unlikely(v->lenp >= v->endp))
		goto enomsg;
	tac = *v->lenp++;
	v->tag = (tag & 0x1f);
	v->cls = (tag & 0xe0);
	if (unlikely((v->tag & 0x1f) == 0x1f) && (err = asn_unpack_tag_extended(v)))
		goto error;
	if (unlikely(val->lenp >= v->endp))
		goto emsgsize;
	v->valp = v->lenp;
	v->len = *v->valp++;
	if (unlikely(v->len == 0x80)) {
		if (unlikely((err = asn_unpack_len_indefinite(v))))
			goto error;
	} else if (unlikely((v->len & 0x80))) {
		if (unlikely((err = asn_unpack_len_extended(v))))
			goto error;
	} else {
		if (unlikely(v->endp - v->valp < v->len))
			goto emsgsize;
		v->endp = v->valp + v->len;
	}
	return (0);
      enomsg:
	err = -ENOMSG;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline fastcall int
asn_unpack_taglen_expected(struct asn_val *v, uchar *b, uchar *e, uint cls, uint tag, uint min,
			   uint max)
{
	struct asn_val t;
	int err;

	if ((err = asn_unpack_taglen(&t, b, e)))
		goto error;
	if (t.cls == cls && t.tag == tag) {
		if (min <= t.len && t.len <= max) {
			*v = t;
			return (0);
		}
		err = -EINVAL;
		goto error;
	}
	err = -ESRCH;
	goto error;
      error:
	return (err);
}
static inline fastcall void
asn_parse_transid(struct asn_val *v)
{
	uchar *p;

	/* transaction ids are octet strings of up to 4 octets */
	for (v->val = 0, p = v->valp; p < v->endp; v->val <<= 8, v->val |= *p++) ;
}

static inline fastcall int
asn_len_len(uint len)
{
	int octets = 1;

	if (len > 0x7f) {
		do {
			octets++;
			len >>= 8;
		} while (len);
	}
	return (octets);
}
static inline fastcall int
asn_tag_len(uint tag)
{
	int octets = 1;

	if (tag >= 0x1f) {

	}
	return (octets);
}

static inline fastcall int
asn_tid_len(uint tid)
{
	int octets = 0;

	do {
		tid >>= 8;
		octets++;
	} while (tid && octets < 4);
	return (octets);
}

static inlint fastcall int
asn_pack_tid(uchar **p, uint tid)
{
	uint n, octets = asn_tid_len(tid);

	for (n = octets; n >= 0; n--)
		*(*p)++ = (tid >> (octets << 3));
	return (octets);
}

static inline fastcall int
asn_taglen_len(uint tag, uint len)
{
	return asn_tag_len(tag) + asn_len_len(len);
}

static inline fastcall int
asn_len(uint tag, uint len)
{
	return asn_taglen_len(tag, len) + len;
}

static inline fastcall int
asn_pack_octet(uchar **p, uchar octet)
{
	*(*p)++ = octet;
	return (1);
}
static inline fastcall int
asn_pack_octets(uchar **p, uchar *w, uint len)
{
	uint size = len;

	while (len--) {
		*(*p)++ = *w++;
	}
	return size;
}
noinline fastcall int
asn_pack_tag_extended(uchar **p, uint cls, uint tag)
{
	uint bytes = 0;

	bytes += asn_pack_octet(p, (cls & 0xe0) | 0x1f);
	do {
		if (tag <= 0x7f) {
			bytes += asn_pack_octet(p, 0x00 + (tag & 0x7f));
		} else {
			bytes += asn_pack_octet(p, 0x80 + (tag & 0x7f));
		}
		tag >>= 7;
	} while (tag);
	return (bytes);
}
noinline fastcall int
asn_pack_len_extended(uchar **p, size_t len)
{
	uchar *l = (*p)++;
	uint bytes = 0;

	do {
		bytes += asn_pack_octet(p, len);
		len >>= 8;
	} while (len);
	bytes += asn_pack_octet(&l, 0x80 + bytes);
	return (bytes);
}

static inline fastcall int
asn_pack_tag(uchar **p, uint cls, uint tag)
{
	if (likely(tag < 0x1f))
		return asn_pack_octet(p, (cls & 0xe0) | (tag & 0x1f));
	return asn_pack_tag_extended(p, cls, tag);
}

static inline fastcall int
asn_pack_len(uchar **p, size_t len)
{
	if (likely(len <= 0x7f))
		return asn_pack_octet(p, (len & 0x7f));
	return asn_pack_len_extended(p, len);
}

static fastcall int
tn_send_msg(struct tr *tr, struct *tn, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err;
	struct sc *sc = NULL;
	size_t nplen =
	    sizeof(N_unitdata_req_t) + tn->info.ADDR_length + tn->peer.ADDR_length +
	    sizeof(tn->info.qos);

	if ((err = tr_get_lower(tr, q, &sc)))
		goto error;
	if ((err = n_unitdata_req(sc, tn, q, msg, dp)))
		goto error;
	tr_put_lower(tr, sc);
	return (0);
      error:
	freeb(tp);
	tr_put_lower(tr, sc);
	return (error);
}

static fastcall int
tn_send_pdu(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg, uint type, int state)
{
	mblk_t *mp, *dp = msg->b_cont;
	unsigned char **p;
	uint tag;
	size_t oilen = 0, dilen = 0;
	size_t mplen, mtlen = msgdsize(dp);

	switch ((tn->pvar & SS7_PVAR_MASK)) {
	default:
		switch (type) {
		case TR_BEGIN_REQ:
			tag = TCAP_APPL_BEGIN;
			oilen = asn_tid_len(tn->info.TRANS_id);
			break;
		case TR_BEGIN_RES:
		case TR_CONT_REQ:
			tag = TCAP_APPL_CONT;
			oilen = asn_tid_len(tn->info.TRANS_id);
			dilen = asn_tid_len(tn->peer.TRANS_id);
			break;
		case TR_END_REQ:
			tag = TCAP_APPL_END;
			dilen = asn_tid_len(tn->peer.TRANS_id);
			break;
		case TR_ABORT_REQ:
			tag = TCAP_APPL_ABORT;
			dilen = asn_tid_len(tn->peer.TRANS_id);
			break;
		}
		mtlen +=
		    (oilen ? asn_len(TCAP_APPL_ORIGID, oilen) :) +
		    (dilen ? asn_len(TCAP_APPL_DESTID, dilen) :);
		break;
	case SS7_PVAR_ANSI:
	{
		bool perm = ((tn->info.ASSOC_flags & TR_PERMISSION) != 0);

		switch (type) {
		case TR_BEGIN_REQ:
			tag = perm ? TCAP_PRIV_QWP : TCAP_PRIV_QWOP;
			oilen = 4;
			break;
		case TR_BEGIN_RES:
		case TR_CONT_REQ:
			tag = perm ? TCAP_PRIV_CWP : TCAP_PRIV_CWOP;
			oilen = 4;
			dilen = 4;
			break;
		case TR_END_REQ:
			tag = TCAP_PRIV_RESP;
			dilen = 4;
			break;
		case TR_ABORT_REQ:
			tag = TCAP_PRIV_ABORT;
			dilen = 4;
			break;
		}
		mtlen += asn_len(TCAP_PRIV_TRSID, oilen + dilen);
		break;
	}
	}
	mplen = ans_len(tag, mtlen);
	if (!(mp = mi_allocb(q, mplen, BPRI_MED)))
		return (-ENOBUFS);
	p = &mp->b_wptr;
	switch ((tn->pvar & SS7_PVAR_MASK)) {
	default:
		asn_pack_tag(p, ASN_APPL_CONS, tag);
		asn_pack_len(p, mtlen);
		if (oilen) {
			asn_pack_tag(p, ASN_APPL_PRIM, TCAP_APPL_ORIGID);
			asn_pack_len(p, oilen);
			asn_pack_tid(p, tn->info.TRANS_id);
		}
		if (dilen) {
			asn_pack_tag(p, ASN_APPL_PRIM, TCAP_APPL_DESTID);
			asn_pack_len(p, dilen);
			asn_pack_tid(p, tn->peer.TRANS_id);
		}
		break;
	case SS7_PVAR_ANSI:
		asn_pack_tag(p, ASN_PRIV_CONS, tag);
		asn_pack_len(p, mtlen);
		asn_pack_tag(p, ASN_PRIV_PRIM, TCAP_PRIV_TRSID);
		asn_pack_len(p, oilen + dilen);
		if (oilen) {
			asn_pack_octet(p, tn->info.TRANS_id >> 24);
			asn_pack_octet(p, tn->info.TRANS_id >> 16);
			asn_pack_octet(p, tn->info.TRANS_id >> 8);
			asn_pack_octet(p, tn->info.TRANS_id >> 0);
		}
		if (dilen) {
			asn_pack_octet(p, tn->peer.TRANS_id >> 24);
			asn_pack_octet(p, tn->peer.TRANS_id >> 16);
			asn_pack_octet(p, tn->peer.TRANS_id >> 8);
			asn_pack_octet(p, tn->peer.TRANS_id >> 0);
		}
		break;
	}
	mp->b_cont = dp;
	return tn_send_msg(tr, tn, q, msg, mp);
}

#if 0
static fastcall int
tn_send_pdu(struct tr *tr, struct tn *tn, queue_t *q, mblk_t *msg, uint type, int state)
{
	int err;
	mblk_t *mp = NULL, *dp;
	uchar **p;

	if ((dp = msg->b_cont) == NULL)
		return (TRBADDATA);
	/* The dialog portion and component portion must already be correctly formatted.  For ANSI
	   that is a concatentation of one or two private constructors for the dialog and
	   components portion.  For ITU-T that is a list of one or two application constructors for 
	   the dialog and components portion. */

	if ((tr->pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
		uint oilen = 0;		/* length of originating-id */
		uint dilen = 0;		/* length of destination-id */
		uint aclen = tn->info.aclen;	/* length of application-context-name */
		uint uilen = tn->info.uilen;	/* length of user-information */
		uint aulen = 0;		/* length of dialogue-pdu */
		uint a1len = 0;		/* length of single-asn-type */
		uint exlen = 0;		/* length of external */
		uint dglen = 0;		/* length of dialog-portion */
		uint cslen = msgdsize(dp);	/* length of component sequence */
		uint mtlen = 0;		/* length of transaction-portion */
		uint mplen;		/* length of message block */
		int tag, dia = -1;

		/* BIT STRING { version1(0) } */
		char *vrstr = "\x07\x80";
		uint vrlen = 2;

		char *asname;
		uint aslen = 7;

		if (type == TR_UNI_REQ)
			/* itu(0) recommendation(0) q(17) 773 as(1) unidialogue-as(2) version1(1) */
			asname = "\x00\x11\x86\x05\x01\x02\x01";
		else
			/* itu(0) recommendation(0) q(17) 773 as(1) dialogue-as(1) version1(1) */
			asname = "\x00\x11\x86\x05\x01\x01\x01";

		switch (type) {
		case TR_UNI_REQ:
			tag = TCAP_APPL_UNI;
			dia = TCAP_APPL_AUDT;
			break;
		case TR_BEGIN_REQ:
			tag = TCAP_APPL_BEGIN;
			dia = TCAP_APPL_AARQ;
			oilen = 4;
			break;
		case TR_BEGIN_RES:
			tag = TCAP_APPL_CONT;
			dia = TCAP_APPL_AARE;
			oilen = 4;
			dilen = 4;
		case TR_CONT_REQ:
			tag = TCAP_APPL_CONT;
			aclen = 0;	/* no dialogue portion */
			oilen = 4;
			dilen = 4;
			break;
		case TR_END_REQ:
			tag = TCAP_APPL_END;
			if (state == TRS_WRES_CIND)
				dia = TCAP_APPL_AARE;
			else
				aclen = 0;	/* no dialogue portion */
			dilen = 4;
			break;
		case TR_ABORT_REQ:
			tag = TCAP_APPL_ABORT;
			/* FIXME: this also depends upon reason */
			if (state == TRS_WRES_CIND)
				dia = TCAP_APPL_AARE;	/* reject */
			else
				dia = TCAP_APPL_ABRT;
			dilen = 4;
			cslen = 0;	/* no component-sequence */
			break;
		default:
			goto efault;
		}
		if (oilen)
			mtlen += asn_len(TCAP_APPL_ORIGID, oilen);
		if (dilen)
			mtlen += asn_len(TCAP_APPL_DESTID, dilen);
		if (cslen)
			mtlen += asn_len(TCAP_APPL_CSEQ, cslen);
		if (aclen) {
			aulen =
			    asn_len(0, vrlen) + asn_len(1, aclen) + uilen ? asn_len(30, uilen) : 0;
			a1len = asn_len(dia, aulen);
			exlen = asn_len(ASN_UNIV_OID, aslen) + asn_len(TCAP_CNTX_ASN1, a1len);
			dglen = asn_len(ASN_UNIV_EXT, exlen);
		}
		if (dglen)
			mtlen += asn_len(TCAP_APPL_DLGP, dglen);
		mplen = asn_len(tag, mtlen);
		if (!(mp = mi_allocb(q, mplen, BPRI_MED)))
			return (-ENOBUFS);
		p = &mp->b_wptr;
		/* PACKAGE */
		asn_pack_tag(p, ASN_APPL_CONS, tag);
		asn_pack_len(p, mtlen);
		/* originating-id */
		if (oilen) {
			asn_pack_tag(p, ASN_APPL_PRIM, TCAP_APPL_ORIGID);
			asn_pack_len(p, oilen);
			asn_pack_octet(p, tn->info.TRANS_id >> 24);
			asn_pack_octet(p, tn->info.TRANS_id >> 16);
			asn_pack_octet(p, tn->info.TRANS_id >> 8);
			asn_pack_octet(p, tn->info.TRANS_id >> 0);
		}
		/* terminating-id */
		if (dilen) {
			asn_pack_tag(p, ASN_APPL_PRIM, TCAP_APPL_DESTID);
			asn_pack_len(p, idlen);
			asn_pack_octet(p, tn->peer.TRANS_id >> 24);
			asn_pack_octet(p, tn->peer.TRANS_id >> 16);
			asn_pack_octet(p, tn->peer.TRANS_id >> 8);
			asn_pack_octet(p, tn->peer.TRANS_id >> 0);
		}
		/* dialogue portion */
		if (dglen) {
			asn_pack_tag(p, ASN_APPL_CONS, TCAP_APPL_DLGP);
			asn_pack_len(p, dglen);
			/* EXTERNAL */
			asn_pack_tag(p, ASN_UNIV_CONS, ASN_UNIV_EXT)
			    asn_pack_len(p, exlen);
			/* abstract-syntax ::= { itu(0) recommendation(0) q(17) 773 as(1)
			   dialogue-as(1) version1(1) } */
			asn_pack_tag(p, ASN_UNIV_PRIM, ASN_UNIV_OID);
			asn_pack_len(p, aslen);
			asn_pack_octets(p, asname, aslen);
			/* single-ASN-Type */
			asn_pack_tag(p, ASN_CNTX_CONS, TCAP_CNTX_ASN1);
			asn_pack_len(p, a1len);
			/* dialog-portion */
			asn_pack_tag(p, ASN_APPL_CONS, dia);
			asn_pack_len(p, aulen);
			/* protocol-version */
			asn_pack_tag(p, ASN_CNTX_PRIM, 0);
			asn_pack_len(p, vrlen);
			asn_pack_octets(p, vrstr, vrlen);
			/* application-context-name */
			asn_pack_tag(p, ASN_CNTX_CONS, 1);
			asn_pack_len(p, aclen);
			/* Note that the application context name supplied by the user must include 
			   the UNIVERSAL OBJECT IDENTIFIER tag or the UNIVERSAL INTEGER tag
			   depending upon which form of the identifier is used (whether direct or
			   indirect). */
			asn_pack_octets(p, tn->info.acnam, tn->info.aclen);
			if (uilen) {
				/* user-information */
				asn_pack_tag(p, ASN_CNTX_CONS, 30);
				asn_pack_len(p, uilen);
				/* Note that the user information supplied must be the contents of
				   a sequence of EXTERNAL.  */
				asn_pack_octets(p, tn->info.uinfo, uilen);
			}
		}
		/* component sequence portion */
		if (cslen) {
			asn_pack_tag(p, ASN_APPL_CONS, TCAP_APPL_CSEQ);
			asn_pack_len(p, cslen);
			/* Note that the component sequence supplied must be the contents of a
			   component sequence */
			linkb(mp, dp);
		}
	} else {
		uint oilen = 0;		/* length of originating-id */
		uint dilen = 0;		/* length of destination-id */
		uint aclen = tn->info.aclen;	/* length of application-context-name */
		uint uilen = tn->info.uilen;	/* length of user-information */
		uint sclen = tn->info.sclen;	/* length of security-context-name */
		uint cilen = tn->info.cilen;	/* length of confidentiality */
		uint dglen = 0;		/* length of dialogue-portion */
		uint cslen = msgdsize(dp);	/* length of components */
		uint ailen = 0;		/* length of abort information */
		uint mtlen = 0;		/* length of transaction portion */
		uint mplen;		/* length of message block */
		int tag, dia = -1;

		uchar *vrstr;
		uint vrlen;

		switch (state) {
		case TRS_WRES_CIND:
			vrstr = tn->info.vrstr;
			vrlen = tn->info.vrlen;
			break;
		case TRS_IDLE:
			vrstr = "\x07";	/* T1.114-1996 T1.114-2000 T1.114-2004 */
			vrlen = 1;
			break;
		default:
		case TRS_DATA_XFER:
			vrstr = NULL;
			vrlen = 0;	/* no version */
			aclen = 0;	/* no application context */
			sclen = 0;	/* no security context */
			break;
		}

		switch (type) {
		case TR_UNI_REQ:
			tag = TCAP_PRIV_UNI;
			break;
		case TR_BEGIN_REQ:
			tag =
			    (tn->info.ASSOC_flags & TR_PERMISSION) ? TCAP_PRIV_QWP : TCAP_PRIV_QWOP;
			oilen = 4;
			break;
		case TR_BEGIN_RES:
			tag =
			    (tn->info.ASSOC_flags & TR_PERMISSION) ? TCAP_PRIV_CWP : TCAP_PRIV_CWOP;
			oilen = 4;
			dilen = 4;
			break;
		case TR_CONT_REQ:
			tag =
			    (tn->info.ASSOC_flags & TR_PERMISSION) ? TCAP_PRIV_CWP : TCAP_PRIV_CWOP;
			oilen = 4;
			dilen = 4;
			break;
		case TR_END_REQ:
			tag = TCAP_PRIV_RESP;
			dilen = 4;
			break;
		case TR_ABORT_REQ:
			tag = TCAP_PRIV_ABORT;
			dilen = 4;
			ailen = uilen;
			uilen = 0;	/* no component-sequence */
			break;
		default:
			goto efault;
		}
		mtlen += asn_len(TCAP_PRIV_TRSID, oilen + dilen);
		if (vrlen)
			dglen += asn_len(TCAP_PRIV_VERSION, vrlen);
		if (aclen)
			dglen += aclen + (uilen ? asn_len(TCAP_PRIV_UINFO, uilen) :);
		if (sclen)
			dglen += sclen + (cilen ? asn_len(2, cilen) :);
		if (dglen)
			mtlen += asn_len(TCAP_PRIV_DLGP, dglen);
		if (cslen)
			mtlen += asn_len(TCAP_PRIV_CSEQ, cslen);
		if (ailen)
			mtlen += asn_len(TCAP_PRIV_U_ABORT, ailen);
		mplen = asn_len(tag, mtlen);
		if (!(mp = mi_allocb(q, mplen, BPRI_MED)))
			return (-ENOBUFS);
		p = &mp->b_wptr;
		/* PACKAGE */
		asn_pack_tag(p, ASN_PRIV_CONS, tag);
		asn_pack_len(p, mtlen);
		asn_pack_tag(p, ASN_PRIV_PRIM, TCAP_PRIV_TRSID);
		asn_pack_len(p, oilen + dilen);
		/* transaction-id */
		if (oilen) {
			asn_pack_octet(p, tn->info.TRANS_id >> 24);
			asn_pack_octet(p, tn->info.TRANS_id >> 16);
			asn_pack_octet(p, tn->info.TRANS_id >> 8);
			asn_pack_octet(p, tn->info.TRANS_id >> 0);
		}
		if (dilen) {
			asn_pack_octet(p, tn->peer.TRANS_id >> 24);
			asn_pack_octet(p, tn->peer.TRANS_id >> 16);
			asn_pack_octet(p, tn->peer.TRANS_id >> 8);
			asn_pack_octet(p, tn->peer.TRANS_id >> 0);
		}
		/* dialogue portion */
		if (dglen) {
			asn_pack_tag(p, ASN_PRIV_CONS, TCAP_PRIV_DLGP);
			asn_pack_len(p, dglen);
			if (vrlen) {
				asn_pack_tag(p, ASN_PRIV_PRIM, TCAP_PRIV_VERSION);
				asn_pack_len(p, vrlen);
				asn_pack_octets(p, vrstr, vrlen);
			}
			if (aclen) {
				uchar *here = *p;

				asn_pack_octets(p, tn->info.acnam, aclen);
				/* Note, the user is allowed to supply these as a UNIVERSAL INTEGER 
				   or a UNIVERSAL OBJECT IDENTIFIER, replendent with tag. */
				switch (*here) {
				case (ASN_UNIV_PRIM | ASN_UNIV_INT):
					*here = (ASN_PRIV_PRIM | TCAP_PRIV_CONTEXT);
					break;
				case (ASN_UNIV_PRIM | ASN_UNIV_OID):
					*here = (ASN_PRIV_PRIM | TCAP_PRIV_CTX_OID);
					break;
				}
				if (uilen) {
					asn_pack_tag(p, ASN_PRIV_CONS, TCAP_PRIV_UINFO);
					asn_pack_len(p, uilen);
					/* Note that the user information supplied must be the
					   contents of a sequence of EXTERNAL. */
					asn_pack_octets(p, tn->info.uinfo, uilen);
				}
			}
			if (sclen) {
				uchar *here = *p;

				asn_pack_octets(p, tn->info.scnam, sclen);
				/* Note, the user is allowed to supply these as a UNIVERSAL INTEGER 
				   or a UNIVERSAL OBJECT IDENTIFIER, replendent with tag. */
				switch (*here) {
				case (ASN_UNIV_PRIM | ASN_UNIV_INT):
					*here = (ASN_CNTX_PRIM | 0);
					break;
				case (ASN_UNIV_PRIM | ASN_UNIV_OID):
					*here = (ASN_CNTX_PRIM | 1);
					break;
				}
				if (cilen) {
					asn_pack_tag(p, ASN_CNTX_CONS, 2);
					asn_pack_len(p, cilen);
					/* Note that confidentiality information supplied must be
					   the contents of the sequence, appropriately coded. */
					asn_pack_octets(p, tn->info.cinfo, cilen);
				}
			}
		}
		if (cslen) {
			asn_pack_tag(p, ASN_PRIV_CONS, TCAP_PRIV_CSEQ);
			asn_pack_len(p, cslen);
			/* Note that the component sequence supplied must be the contents of a
			   component sequence. */
			linkb(mp, dp);
		}
		if (ailen) {
			asn_pack_tag(p, ASN_PRIV_CONS, TCAP_PRIV_U_ABORT);
			asn_pack_len(p, ailen);
			/* Note that the user abort information supplied must be of type EXTERNAL. */
			asn_pack_octets(p, tn->info.uinfo, ailen);
		}
	}
	if ((err = n_unitdata_req(tr, tn, q, msg, mp))) {
		freeb(mp);
		goto error;
	}
	return (0);
      baddata:
	err = TRBADDATA;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}
#endif

struct asn_transaction {
	struct asn_var trns;		/* message type */
	struct asn_var otid;		/* origination transaction id */
	struct asn_var dtid;		/* destination transaction id */
	struct asn_var abrt;		/* p-abort cause */
};

static fastcall int
n_recv_msg(struct sc *sc, queue_t *q, mblk_t *msg, caddr_t dptr, size_t dlen, caddr_t sptr,
	   size_t slen, mblk_t *dp)
{
	struct asn_transaction t;
	struct tr *tr = NULL;
	int err;

	if (!dp)
		return (NBADDATA);
	if (!pullupmsg(dp, -1))
		return (-ENOBUFS);
	if ((err = sc_get_upper(sc, q, &tr)))
		return (err);
	if (unlikely(tr_get_state(tr) != TRS_IDLE))
		goto eagain;	/* come back later */
	if ((tr->pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
		if (asn_unpack_taglen(&t.trns, msg->b_rptr, msg->b_wptr))
			goto badlyFormattedTransactionPortion;
		if (t.trns.cls != ASN_APPL_CONS)
			goto incorrectTransactionPortion;
		switch (t.trns.tag) {
		case TCAP_APPL_UNI:
			/* need to create new transaction */
			t.otid.len = 0;
			t.otid.valp = t.otid.endp = t.trns.valp;
			asn_parse_transid(&t.otid);
			t.dtid.len = 0;
			t.dtid.valp = t.dtid.endp = t.trns.valp;
			asn_parse_transid(&t.dtid);
			if ((err =
			     tr_uni_ind(tr, q, msg, dptr, dlen, sptr, slen, NULL, 0, dp,
					t.dtid.endp, t.trns.endp)))
				goto error;
			return (0);
		case TCAP_APPL_BEGIN:
			if (asn_unpack_taglen_expected
			    (&t.otid, t.trns.valp, t.trns.endp, ASN_APPL_PRIM,
			     TCAP_APPL_ORIGID, 1, 4))
				/* missing or invalid originating transaction id */
				goto badlyFormattedTransactionPortion;
			asn_parse_transid(&t.otid);
			t.dtid.len = 0;
			t.dtid.valp = t.dtid.endp = t.otid.endp;
			asn_parse_transid(&t.dtid);
			/* IMPLEMENTATION NOTE:- Need to always create new a transaction. ITU-T
			   Rec. Q.771 (2001) and ANSI T1.114.4/2000 states that when a BEGIN/QUERY
			   is received a new transaction is formed regardless of the originating
			   transaction id. */
			if ((err = tn_create(tr, &tn, 0)))
				return (err);
			tn->peer.TRANS_id = t.otid.val;
			tn->peer.ADDR_length = slen;
			bcopy(sptr, tn->peer.abuf, slen);
			if ((err =
			     tr_begin_ind(tr, q, msg, tn, dptr, dlen, sptr, slen, NULL, 0,
					  dp, t.dtid.endp, t.trns.endp))) {
				tn_destroy(tn);
				goto error;
			}
			return (0);
		case TCAP_APPL_CONT:
			if (asn_unpack_taglen_expected
			    (&t.otid, t.trns.valp, t.trns.endp, ASN_APPL_PRIM,
			     TCAP_APPL_ORIGID, 1, 4))
				goto badlyFormattedTransactionPortion;
			asn_parse_transid(&t.otid);
			if (asn_unpack_taglen_expected
			    (&t.dtid, t.otid.endp, t.trns.endp, ASN_APPL_PRIM,
			     TCAP_APPL_DESTID, 1, 4))
				goto badlyFormattedTransactionPortion;
			asn_parse_transid(&t.dtid);
			if (!(tn = tn_lookup(tr, t.dtid.val)))
				goto unrecognizedTransactionID;
			switch (__builtin_expect(tn_get_state(tn), TRS_DATA_XFER)) {
			case TRS_DATA_XFER:
				/* process now */
				if ((err =
				     tr_cont_ind(tr, q, msg, tn, NULL, 0, dp, t.dtid.endp,
						 t.trns.endp)))
					goto error;
				return (0);
			case TRS_WCON_CREQ:
				/* process now */
				if ((err =
				     tr_begin_con(tr, q, msg, tn, sptr, slen, NULL, 0, dp,
						  t.dtid.endp, t.trns.endp)))
					goto error;
				return (0);
			default:
			case TRS_IDLE:
			case TRS_WRES_CIND:
				/* wrong state for this message */
				goto incorrectTransactionPortion;
			case TRS_WACK_CREQ:
			case TRS_WACK_CRES:
			case TRS_WACK_DREQ6:
			case TRS_WACK_DREQ7:
			case TRS_WACK_DREQ9:
				goto eagain;	/* process later, but should never happen due to
						   locking */
			}
			break;
		case TCAP_APPL_END:
		case TCAP_APPL_ABORT:
			t.otid.len = 0;
			t.otid.valp = t.otid.endp = t.trns.valp;
			asn_parse_transid(&t.otid);
			if (asn_unpack_taglen_expected
			    (&t.dtid, t.otid.endp, t.trns.endp, ASN_APPL_PRIM,
			     TCAP_APPL_DESTID, 1, 4))
				goto badlyFormattedTransactionPortion;
			asn_parse_transid(&t.dtid);
			if (!(tn = tn_lookup(tr, t.dtid.val)))
				goto unrecognizedTransactionID;
			switch (__builtin_expect(t.trns.tag, TCAP_APPL_END)) {
			case TCAP_APPL_END:
				switch (__builtin_expect(tn_get_state(tn), TRS_DATA_XFER)) {
				case TRS_DATA_XFER:
					/* termination of established transaction */
					if ((err =
					     tr_end_ind(tr, q, msg, tn, NULL, 0, NULL, 0, dp,
							t.dtid.endp, t.trns.endp)))
						goto error;
					return (0);
				case TRS_WCON_CREQ:
					/* response to initial BEGIN */
					if ((err =
					     tr_end_ind(tr, q, msg, tn, sptr, slen, NULL, 0,
							dp, t.dtid.endp, t.trns.endp)))
						goto error;
					return (0);
				case TRS_WRES_CIND:
				case TRS_IDLE:
					/* wrong state for this message */
					goto incorrectTransactionPortion;
				case TRS_WACK_CREQ:
				case TRS_WACK_CRES:
				case TRS_WACK_DREQ6:
				case TRS_WACK_DREQ7:
				case TRS_WACK_DREQ9:
					/* Process later.  However, due to the locking arrangement
					   in place, this should never happen. */
					goto eagain;
				}
				break;
			case TCAP_APPL_ABORT:
				switch (__builtin_expect(tn_get_state(tn), TRS_DATA_XFER)) {
				case TRS_DATA_XFER:
				case TRS_WCON_CREQ:
				case TRS_WRES_CIND:
				{
					t_uscalar_t ORIGINATOR, ABORT_cause;

					switch (asn_unpack_taglen_expected
						(&t.abrt, t.dtid.endp, t.trns.endp,
						 ASN_APPL_CONS, TCAP_APPL_PCAUSE, 1, 1)) {
					case -ENOMSG:
						ORIGINATOR = TR_UNKNOWN;
						ABORT_cause = 0;
						break;
					case -ESRCH:
						if (t.abrt.cls == ASN_APPL_CONS
						    && t.abrt.tag == TCAP_APPL_DLGP) {
							ORIGINATOR = TR_USER;
							ABORT_cause = 0;
						} else {
							ORIGINATOR = TR_UNKNOWN;
							ABORT_cause = 0;
						}
						break;
					case 0:
						ABORT_cause = 0x0100 | (t_uscalar_t) *t.abrt.valp;
						ORIGINATOR = TR_PROVIDER;
						break;
					default:
						goto discard;	/* FIXME: or do we abort an abort? */
					}
					if ((err =
					     tr_abort_ind(tr, q, msg, tn, ABORT_cause,
							  ORIGINATOR, NULL, 0, dp, t.dtid.endp,
							  t.trns.endp)))
						goto error;
					return (0);
				}
				case TRS_IDLE:
					/* doesn't exist */
					goto discard;
				case TRS_WACK_CREQ:
				case TRS_WACK_CRES:
				case TRS_WACK_DREQ6:
				case TRS_WACK_DREQ7:
				case TRS_WACK_DREQ9:
					/* Process later.  However, due to the locking arrangement
					   in place, this should never happen. */
					goto eagain;
				}
				break;
			}
			break;
		default:
			goto unrecognizedMessageType;
		      unrecognizedMessageType:
			/* P-Abort-Cause unrecognizedMessageType(0) */
		      unrecognizedTransactionID:
			/* P-Abort-Cause unrecognizedTransactionID(1) */
		      badlyFormattedTransactionPortion:
			/* P-Abort-Cause badlyFormattedTransactionPortion(2) */
		      incorrectTransactionPortion:
			/* P-Abort-Cause incorrectTransactionPortion(3) */
		      resourceLimitation:
			/* P-Abort-Cause resourceLimitation(4) */
			return (NBADDATA);
		}
	} else {
		if (asn_unpack_taglen(&t.trns, msg->b_rptr, msg->b_wptr))
			goto badlyStructuredTransactionPortion;
		if (t.trns.cls != ASN_PRIV_CONS)
			goto badlyStructuredTransactionPortion;
		switch (t.trns.tag) {
		case TCAP_PRIV_UNI:
			if (asn_unpack_taglen_expected
			    (&t.otid, t.trns.valp, t.trns.endp, ASN_PRIV_PRIM, TCAP_PRIV_TRSID,
			     0, 0))
				goto badlyStructuredTransactionPortion;
			asn_parse_transid(&t.otid);
			t.dtid.len = 0;
			t.dtid.valp = t.dtid.endp = t.otid.endp;
			asn_parse_transid(&t.dtid);
			break;
		case TCAP_PRIV_QWP:
		case TCAP_PRIV_QWOP:
			if (asn_unpack_taglen_expected
			    (&t.otid, t.trns.valp, t.trns.endp, ASN_PRIV_PRIM, TCAP_PRIV_TRSID,
			     4, 4))
				goto badlyStructuredTransactionPortion;
			t.dtid.len = 0;
			t.dtid.valp = t.dtid.endp = t.otid.endp;
			asn_parse_transid(&t.otid);
			asn_parse_transid(&t.dtid);
			/* need to create new transaction */
			if (!(tn = tr_create_trasaction(tr)))
				goto resourceUnavailable;
			break;
		case TCAP_PRIV_CWP:
		case TCAP_PRIV_CWOP:
			if (asn_unpack_taglen_unexpected
			    (&t.otid, t.trns.valp, t.trns.endp, ASN_PRIV_PRIM, TCAP_PRIV_TRSID,
			     8, 8))
				goto badlyStructuredTransactionPortion;
			t.dtid.len = t.otid.len = 4;
			t.dtid.valp = t.otid.valp + 4;
			t.dtid.endp = t.otid.endp;
			t.otid.endp = t.dtid.valp;
			asn_parse_transid(&t.otid);
			asn_parse_transid(&t.dtid);
			/* need to lookup transaction by destination id */
			if (!(tn = tr_lookup_dtid(tr, t.dtid.valp, t.dtid.len)))
				goto unassignedRespondingTransactionId;
			break;
		case TCAP_PRIV_RESP:
		case TCAP_PRIV_ABORT:
			if (asn_unpack_taglen_unexpected
			    (&t.dtid, t.trns.valp, t.trns.endp, ASN_PRIV_PRIM, TCAP_PRIV_TRSID,
			     4, 4))
				goto badlyStructuredTransactionPortion;
			t.otid.len = 0;
			t.otid.val = 0;
			t.otid.valp = t.otid.endp = t.dtid.valp;
			asn_parse_transid(&t.otid);
			asn_parse_transid(&t.dtid);
			/* need to lookup transaction by destination id */
			if (!(tn = tr_lookup_dtid(tr, t.dtid.valp, t.dtid.len)))
				goto unassignedRespondingTransactionId;
			if (t.trns.tag == TCAP_PRIV_RESP && !(tn->info.ASSOC_flags & TR_PERMISSION))
				goto permissionToReleaseProblem;
			break;
		default:
			goto unrecognizedPackageType;
		      unrecognizedPackageType:
			/* P-abort unrecognizedPackageType(1) */
		      incorrectTransactionPortion:
			/* P-abort incorrectTransactionPortion(2) */
		      badlyStructuredTransactionPortion:
			/* P-abort badlyStructuredTransactionPortion(3) */
		      unassignedRespondingTransactionID:
			/* P-abort unassignedRespondingTransactionID(4) */
		      permissionToReleaseProblem:
			/* P-abort permissionToReleaseProblem(5) */
		      resourceUnavailable:
			/* P-abort resourceUnavailable(6) */
#if 0
			/* These need to be handled by the TC sub-layer */
		      unrecognizedDialoguePortionID:
			/* P-abort unrecognizedDialoguePortionID(7) */
		      badlyStructuredDialoguePortion:
			/* P-abort badlyStructuredDialoguePortion(8) */
		      missingDialoguePortion:
			/* P-abort missingDialoguePortion(9) */
		      inconsistentDialoguePortion:
			/* P-abort inconsistentDialoguePortion(10) */
#endif
			return (NBADDATA);
		}
	}
	return (FIXME);
      eagain:
	err = -EAGAIN;
	goto error;
      error:
	return (err);
}

/*
 * TR-User -> TR-Provider primitives.
 */
static fastcall int
tr_info_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_info_ack *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	return tr_info_ack(tr, q, mp);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_bind_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_bind_req)
{
	int err;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if ((alen = p->ADDR_length)) {
		if (!MBLKIN(mp, p->ADDR_offset, alen))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if (tr_get_state(tr) != TRS_UNBND)
		goto outstate;
	tr_set_state(tr, TRS_WACK_BREQ);
	tr->bind.ADDR_length = alen;
	bcopy(aptr, (caddr_t) &tr->bind + tr->bind.ADDR_offset, alen);
	tr->bind.XACT_number = p->XACT_number;
	tr->bind.BIND_flags = p->BIND_flags;
	if ((err = n_bind_req(tr, q, mp, aptr, alen, NULL, 0)))
		goto error;
	return (0);
      outstate:
	err = TROUTSTATE;
	goto error;
      badaddr:
	err = TRBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}

/**
 * tr_unbind_req: - process TR_UNBIND_REQ primitive
 * @tr: TR private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 * @p: pointer to primitive structure
 *
 * There are two approaches that can be taken here: 1, reject unbind requests when there are
 * transaction pending.  2, abort all pending transactions.  Because the user has little control
 * over incoming transaction requests, aborting pending transactions is probably the better
 * approach.
 */
static fastcall int
tr_unbind_req(struct tr *tr, queue_t *q, mblk_t *mp, TR_unbind_req * p)
{
	int err;
	struct sc *sc = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if ((err = tr_get_lower(tr, q, &sc)))
		goto error;
	tr_set_state(tr, TRS_WACK_UREQ);
	tr_abort_transactions(tr, q);
	if ((err = n_unbind_req(sc, q, mp)))
		goto error;
	tr_put_lower(tr, sc);
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	tr_put_lower(tr, sc);
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_convert_options(caddr_t optr, size_t olen, N_qos_sel_data_sccp_t *n_qos)
{
	int err;
	TR_qos_sel1_t tr_qos;

	if (olen != sizeof(tr_qos))
		goto badopt;
	bcopy(optr, &tr_qos, olen);
	if (tr_qos.type != TR_QOS_SEL1)
		goto badqostype;
	n_qos->type = N_QOS_SEL_DATA_SCCP;
	n_qos->protocol_class = tr_qos.seq_ctrl == -1U ? -1U : 1;
	n_qos->option_flags = tr_qos.flags;
	n_qos->sequence_selection = tr_qos.seq_ctrl;
	n_qos->message_priority = tr_qos.priority;
	n_qos->importance = -1U;
	return (0);
      badqostype:
	err = TRBADQOSTYPE;
	goto error;
      badopt:
	err = TRBADOPT;
	goto error;
      error:
	return (err);
}
static fastcall int
tr_set_options(struct tr *tr, queue_t *q, mblk_t *mp, caddr_t optr, size_t olen)
{
	int err;
	TR_qos_sel1_t tr_qos;
	N_qos_sel_data_sccp_t n_qos;

	if (olen != sizeof(tr_qos))
		goto badopt;
	bcopy(optr, &tr_qos, olen);
	if (tr_qos.type != TR_QOS_SEL1)
		goto badqostype;
	n_qos.type = N_QOS_SEL_DATA_SCCP;
	n_qos.protocol_class = tr_qos.seq_ctrl == -1U ? -1U : 1;
	n_qos.option_flags = tr_qos.flags;
	n_qos.sequence_selection = tr_qos.seq_ctrl;
	n_qos.message_priority = tr_qos.priority;
	n_qos.importance = -1U;
	if ((err = n_optmgmt_req(tr, q, mp, (caddr_t) &n_qos, sizeof(n_qos))))
		goto error;
      done:
	return (0);
      badqostype:
	err = TRBADQOSTYPE;
	goto error;
      badopt:
	err = TRBADOPT;
	goto error;
      error:
	return (err);
}
static fastcall int
tr_optmgmt_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_optmgmt_req *p)
{
	int err;
	caddr_t optr = NULL;
	size_t olen = 0;
	N_qos_sel_data_sccp_t n_qos;
	struct sc *sc = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	olen = p->OPT_length;
	if (!MBLKIN(mp, p->OPT_offset, olen))
		goto badopt;
	optr = (caddr_t) p + p->OPT_offset;
	if ((err = tr_convert_options(optr, olen, &n_qos)))
		goto error;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	tr_set_state(tr, TRS_WACK_OPTREQ);
	if ((err = n_optmgmt_req(tr, q, mp, p->MGMT_flags, (caddr_t) &n_qos, sizeof(n_qos))))
		goto error;
	tr_put_lower(tr, sc);
	return (0);
      badqostype:
	err = TRBADQOSTYPE;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      badopt:
	err = TRBADOPT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	tr_put_lower(tr, sc);
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_uni_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_uni_req *p)
{
	int err;
	caddr_t dptr = NULL, sptr = NULL, optr = NULL;
	size_t dlen = 0, slen = 0, olen = 0;
	struct tn *tn = NULL;
	mblk_t *dp;
	struct sc *sc = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if (!(dlen = p->DEST_length))
		goto noaddr;
	if (dlen != tr->info.ADDR_size)
		goto badaddr;
	if (!MBLKIN(mp, p->DEST_offset, dlen))
		goto badaddr;
	if ((slen = p->ORIG_length)) {
		if (!MBLKIN(mp, p->ORIG_offset, slen))
			goto badaddr;
		if (slen != tr->info.ADDR_size)
			goto badaddr;
		sptr = (caddr_t) p + p->ORIG_offset;
	}
	if ((olen = p->OPT_length)) {
		N_qos_sel_data_sccp_t n_qos;

		if (olen != sizeof(n_qos))
			goto badopt;
		if (!MBLKIN(mp, p->OPT_offset, olen))
			goto badopt;
		optr = (caddr_t) p + p->OPT_offset;
		bcopy(optr, &n_qos, olen);
		if (n_qos->type != N_QOS_SEL_DATA_SCCP)
			goto badqostype;
	}
	if ((err = tn_create(tr, &tn)))	/* FIXME: do not create transaction */
		goto error;
	tn->info.CURRENT_state = TRS_IDLE;
	tn->info.ASSOC_flags = 0;
	tn->info.DEST_length = dlen;
	bcopy(dptr, (caddr_t) &tn->info + tn->info.DEST_offset, dlen);
	if (slen) {
		tn->info.ORIG_length = slen;
		bcopy(sptr, (caddr_t) &tn->info + tn->info.ORIG_offset, slen);
	}
	if (olen) {
		tn->info.OPT_length = olen;
		bcopy(optr, &tn->opts, olen);
	}
	if ((err = tr_get_lower(tr, q, &sc)))
		goto error;
	if (!(dp = tn_encode(tr, tn, p->PRIM_type, mp->b_cont)))
		goto enobufs;
	if ((err = n_unitdata_req(sc, tn, q, mp, dp))) {
		freeb(dp);
		goto error;
	}
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      enomem:
	err = -ENOMEM;
	goto error;
      badqostype:
	err = TRBADQOSTYPE;
	goto error;
      badopt:
	err = TRBADOPT;
	goto error;
      badaddr:
	err = TRBADADDR;
	goto error;
      noaddr:
	err = TRNOADDR;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	tn_delete(tn);
	tr_put_lower(tr, sc);
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}

/**
 * tr_begin_req: - process TR_BEGIN_REQ primitive
 * @tr: TR private structure
 * @q: active queue
 * @mp: the message block
 * @p: pointer to primitive in message block
 *
 * Establishes a
 */
static fastcall int
tr_begin_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_begin_req *p)
{
	int err;
	struct tn *tn = NULL;
	mblk_t *rp = NULL;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if ((err = tn_create(tr, &tn, p->TRANS_id)))
		goto error;
	tn->info.CURRENT_state = TRS_WCON_CREQ;
	tn->info.ASSOC_flags = p->ASSOC_flags;
	if ((err = tn_set_dest(tn, mp, p->DEST_offset, p->DEST_length)))
		goto error;
	if ((err = tn_set_orig(tn, mp, p->ORIG_offset, p->ORIG_length)))
		goto error;
	if ((err = tn_set_opts(tn, mp, p->OPT_offset, p->OPT_length)))
		goto error;
	if (unlikely(!(rp = mi_allocb(q, sizeof(struct TR_ok_ack), BPRI_MED))))
		goto enobufs;
	if ((dlen = msgdsize(mp)) < 2 || dlen > tr->info.CDATA_size)
		goto baddata;
	if ((err = tn_send_pdu(tr, tn, q, mp, p->PRIM_type)))
		goto error;
	return tr_ok_reply(tr, tn, q, mp, p->PRIM_type, rp);
      baddata:
	err = TRBADDATA;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      enomem:
	err = -ENOMEM;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return tr_error_reply(tr, tn, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_begin_res(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_begin_res *p)
{
	int err;
	struct tn *tn = NULL;
	mblk_t *rp = NULL;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if (!(tn = tn_find(tr, p->TRANS_id)))
		goto badseq;
	if (tn_get_state(tn) != TRS_WRES_CIND)
		goto outstate;
	tn->info.ASSOC_flags = p->ASSOC_flags;
	if ((err = tn_set_orig(tn, mp, p->ORIG_offset, p->ORIG_length)))
		goto error;
	if ((err = tn_set_opts(tn, mp, p->OPT_offset, p->OPT_length)))
		goto error;
	if (unlikely(!(rp = mi_allocb(q, sizeof(struct TR_ok_ack), BPRI_MED))))
		goto enobufs;
	if ((dlen = msgdsize(mp)) < 2 || dlen > tr->info.TSDU_size)
		goto baddata;
	if ((err = tn_send_pdu(tr, tn, q, mp, p->PRIM_type)))
		goto error;
	return tr_ok_reply(tr, tn, q, mp, p->PRIM_type, rp);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      badseq:
	err = TRBADSEQ;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return tr_error_reply(tr, tn, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_cont_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_cont_req *p)
{
	int err;
	struct tn *tn;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if (!(tn = tn_find(tr, p->TRANS_id)))
		goto badseq;
	switch (tn_get_state(tn)) {
	case TRS_DATA_XFER:
		break;
	case TRS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	tn->info.ASSOC_flags = p->ASSOC_flags;
	if ((err = tn_set_opts(tn, mp, p->OPT_offset, p->OPT_length)))
		goto error;
	if ((dlen = msgdsize(mp)) < 2 || dlen > tr->info.TSDU_size)
		goto baddata;
	if ((err = tn_send_pdu(tr, tn, q, mp, p->PRIM_type)))
		goto error;
	return (0);
      outstate:
	err = TROUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      badseq:
	err = TRBADSEQ;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, tn, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_end_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_end_req *p)
{
	int err, state;
	struct tn *tn;
	mblk_t *rp = NULL;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if (!(tn = tn_find(tr, p->TRANS_id)))
		goto badseq;
	switch ((state = tn_get_state(tn))) {
	case TRS_DATA_XFER:
	case TRS_WRES_CIND:
		break;
	case TRS_IDLE:
		goto discard;
	default:
		goto oustate;
	}
	if (((tn->pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
	    && !(tn->peer.ASSOC_flags & TR_PERMISSION))
		goto oustate;	/* not allowed to end without permission */
	tn->info.ASSOC_flags = p->ASSOC_flags;
	if ((err = tn_set_opts(tn, mp, p->OPT_offset, p->OPT_length)))
		goto error;
	if ((dlen = msgdsize(mp)) < 2 || dlen > tr->info.DDATA_size)
		goto baddata;
	if (!(rp = mi_allocb(q, sizeof(struct TR_ok_ack), BPRI_MED)))
		goto enobufs;
	if ((err = tn_send_pdu(tr, tn, q, mp, p->PRIM_type)))
		goto error;
	return tr_ok_reply(tr, tn, q, mp, p->PRIM_type, rp);
      enobufs:
	err = -ENOBUFS;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      badseq:
	err = TRBADSEQ;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return tr_error_reply(tr, tn, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_abort_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_abort_req *p)
{
	int err, state;
	struct tn *tn;
	mblk_t *rp = NULL;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_IDLE)
		goto outstate;
	if (!(tn = tn_find(tr, p->TRANS_id)))
		goto badseq;
	switch ((state = tn_get_state(tn))) {
	case TRS_WRES_CIND:
	case TRS_WCON_CREQ:
	case TRS_DATA_XFER:
		break;
	case TRS_IDLE:
		goto discard;
	default:
		goto oustate;
	}
	tn->info.ABORT_cause = p->ABORT_cause;
	if ((err = tn_set_opts(tn, mp, p->OPT_offset, p->OPT_length)))
		goto error;
	if (!(rp = mi_allocb(q, sizeof(struct TR_ok_ack), BPRI_MED)))
		goto enobufs;
	if ((dlen = msgdsize(mp)) > tr->info.DDATA_size)
		goto baddata;
	if ((err = tn_send_pdu(tr, tn, q, mp, p->PRIM_type)))
		goto error;
	return tr_ok_reply(tr, tn, q, mp, p->PRIM_type, rp);
      enobufs:
	err = -ENOBUFS;
	goto error;
      badseq:
	err = TRBADSEQ;
	goto error;
      outstate:
	err = TROUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return tr_error_reply(tr, tn, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_addr_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_addr_req *p)
{
	int err;
	struct tn *tn = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (p->TRANS_id != 0 && !(tn = tn_find(tr, p->TRANS_id)))
		goto badseq;
	return tr_addr_ack(tr, tn, q, mp);
      badseq:
	err = TRBADSEQ;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, tn, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_coord_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_coord_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_coord_res(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_coord_res *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
tr_state_req(struct tr *tr, queue_t *q, mblk_t *mp, struct TR_state_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_reply(tr, NULL, q, mp, p->PRIM_type, err, __FUNCTION__);
}

/*
 * NS-Provider -> NS-User primitives (Connectionless service).
 */

static fastcall int
n_info_ack(struct sc *sc, queue_t *q, mblk_t *mp, N_info_ack_t *p)
{
	int err;
	caddr_t aptr = NULL, qptr = NULL, rptr = NULL, pptr = NULL;
	size_t alen = 0, qlen = 0, rlen = 0, plen = 0;
	struct tr *tr = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if ((alen = p->ADDR_length)) {
		if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
			goto badaddr;
		if (alen > sizeof(sc->info.abuf))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((qlen = p->QOS_length)) {
		if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
			goto badaddr;
		if (qlen > sizeof(sc->info.qbuf))
			goto badaddr;
		qptr = (caddr_t) p + p->QOS_offset;
	}
	if ((rlen = p->QOS_range_length)) {
		if (!MBLKIN(mp, p->QOS_range_offset, p->QOS_range_length))
			goto badaddr;
		if (rlen > sizeof(sc->info.rbuf))
			goto badaddr;
		rptr = (caddr_t) p + p->QOS_range_offset;
	}
	if ((plen = p->PROTOID_length)) {
		if (!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length))
			goto badaddr;
		if (plen > sizeof(sc->info.pbuf))
			goto badaddr;
		pptr = (caddr_t) p + p->PROTOID_offset;
	}
	sc->info.NSDU_size = p->NSDU_size;
	sc->info.ENSDU_size = p->ENSDU_size;
	sc->info.CDATA_size = p->CDATA_size;
	sc->info.DDATA_size = p->DDATA_size;
	sc->info.ADDR_size = p->ADDR_size;
	sc->info.ADDR_length = p->ADDR_length;
	sc->info.ADDR_offset = (caddr_t) sc->info.abuf - (caddr_t) &sc->info;
	bcopy(aptr, (caddr_t) sc->info.abuf + sc->info.ADDR_offset, alen);
	sc->info.QOS_length = p->QOS_length;
	sc->info.QOS_offset = (caddr_t) sc->info.qbuf - (caddr_t) &sc->info;
	bcopy(aptr, (caddr_t) sc->info.qbuf + sc->info.QOS_offset, alen);
	sc->info.QOS_range_length = p->QOS_range_length;
	sc->info.QOS_range_offset = (caddr_t) sc->info.rbuf - (caddr_t) &sc->info;
	bcopy(aptr, (caddr_t) sc->info.rbuf + sc->info.QOS_range_offset, alen);
	sc->info.OPTIONS_flags = p->OPTIONS_flags;
	sc->info.NIDU_size = p->NIDU_size;
	sc->info.SERV_type = p->SERV_type;
	sc->info.CURRENT_state = p->CURRENT_state;
	sc->info.PROVIDER_type = p->PROVIDER_type;
	sc->info.NODU_size = p->NODU_size;
	sc->info.PROTOID_length = p->PROTOID_length;
	sc->info.PROTOID_offset = (caddr_t) sc->info.pbuf - (caddr_t) &sc->info;
	bcopy(aptr, (caddr_t) sc->info.pbuf + sc->info.PROTOID_offset, alen);
	sc->info.NPI_version = p->NPI_version;
	if ((err = sc_get_upper(sc, q, &tr)) == 0) {
		if ((sc->info.SERV_type & N_CLNS) == 0)
			goto incompatible;
		if (sc->info.qos.n_qos_type != N_QOS_SEL_INFO_SCCP)
			goto incompatible;
		if (sc->info.qor.n_qos_type != N_QOS_RANGE_INFO_SCCP)
			goto incompatible;
		if ((tr->pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			tr->info.TSDU_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_PRIV_CWP, sc->info.NSDU_size)
			    - asn_len(TCAP_PRIV_TRSID, 8);
			tr->info.ETSDU_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_PRIV_UNI, sc->info.NSDU_size)
			    - asn_len(TCAP_PRIV_TRSID, 0);
			tr->info.CDATA_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_PRIV_QWP, sc->info.NSDU_size)
			    - asn_len(TCAP_PRIV_TRSID, 4);
			tr->info.DDATA_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_PRIV_RESP, sc->info.NSDU_size)
			    - asn_len(TCAP_PRIV_TRSID, 4);
			tr->info.TIDU_size = sc->info.NODU_size
			    - asn_taglen_len(TCAP_PRIV_CWP, sc->info.NODU_size)
			    - asn_len(TCAP_PRIV_TRSID, 8);
		} else {
			tr->info.TSDU_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_APPL_BEGIN, sc->info.NSDU_size)
			    - asn_len(TCAP_APPL_ORIGID, 4)
			    - asn_len(TCAP_APPL_DESTID, 4);
			tr->info.TSDU_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_APPL_UNI, sc->info.NSDU_size);
			tr->info.CDATA_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_PRIV_BEGIN, sc->info.NSDU_size)
			    - asn_len(TCAP_APPL_ORIGID, 4);
			tr->info.DDATA_size = sc->info.NSDU_size
			    - asn_taglen_len(TCAP_PRIV_BEGIN, sc->info.NSDU_size)
			    - asn_len(TCAP_APPL_DESTID, 4);
			tr->info.TIDU_size = sc->info.NODU_size
			    - asn_taglen_len(TCAP_APPL_BEGIN, sc->info.NODU_size)
			    - asn_len(TCAP_APPL_ORIGID, 4)
			    - asn_len(TCAP_APPL_DESTID, 4);
		}
		tr->info.OPT_size = 256;	/* XXX: for now */
		tr->info.ADDR_size = sc->info.ADDR_size;
		tr->info.SERV_type = (TR_STRUCTURED | TR_UNSTRUCTURED);
		switch (sc->info.CURRENT_state) {
		case NS_UNBND:
			tr->info.CURRENT_state = TRS_UNBND;
			break;
		case NS_IDLE:
			tr->info.CURRENT_state = TRS_IDLE;
			break;
		}
		tr->info.PROVIDER_flag = (1 << (8 + sc->info.PROVIDER_type));
		tr->info.TRPI_version = TRPI_CURRENT_VERSION;
	}
	freemsg(mp);
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
      incompatible:
	return m_error(tr, q, mp, ENXIO);
}
static fastcall int
n_bind_ack(struct sc *sc, queue_t *q, mblk_t *mp, N_bind_ack_t *p)
{
	int err;
	struct tr *tr = NULL;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (likely((alen = p->ADDR_length))) {
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((err = sc_get_upper(sc, q, &tr)))
		goto error;
	if (tr_get_state(tr) != TRS_WACK_BREQ)
		goto outstate;
	sc_set_state(sc, NS_IDLE);
	if ((err = tr_bind_ack(tr, q, mp, aptr, alen)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_error_ack(struct sc *sc, queue_t *q, mblk_t *mp, N_error_ack_t *p)
{
	int err;
	struct tr *tr = NULL;
	t_scalar_t error;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if ((err = sc_get_upper(sc, q, &tr)))
		goto outstate;
	error = sc_xlate_error(p->ERROR_prim, p->NPI_error, p->UNIX_error);
	switch (sc_get_state(sc)) {
	case NS_WACK_BREQ:
		sc_set_state(sc, NS_UNBND);
		if ((err = tr_error_ack(tr, NULL, q, mp, TR_BIND_REQ, error, __FUNCTION__)))
			goto error;
		break;
	case NS_WACK_UREQ:
		sc_set_state(sc, NS_IDLE);
		if ((err = tr_error_ack(tr, NULL, q, mp, TR_UNBIND_REQ, error, __FUNCTION__)))
			goto error;
		break;
	default:
		if (p->ERROR_prim != N_OPTMGMT_REQ)
			goto outstate;
		/* fall through */
	case NS_WACK_OPTREQ:
		sc_set_state(sc, NS_IDLE);
		if ((err = tr_error_ack(tr, NULL, q, mp, TR_OPTMGMT_REQ, error, __FUNCTION__)))
			goto error;
		break;
	}
	sc_put_upper(sc, tr);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_ok_ack(struct sc *sc, queue_t *q, mblk_t *mp, N_ok_ack_t *p)
{
	int err;
	struct tr *tr = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if ((err = sc_get_upper(sc, q, &tr)))
		goto error;
	switch (sc_get_state(sc)) {
	case NS_WACK_UREQ:
		sc_set_state(sc, NS_UNBND);
		if ((err = tr_ok_ack(tr, NULL, q, mp, TR_UNBIND_REQ)))
			goto error;
		break;
	default:
		if (p->CORRECT_prim != N_OPTMGMT_REQ)
			goto oustate;
		/* fall through */
	case NS_WACK_OPTREQ:
		sc_set_state(sc, NS_IDLE);
		if ((err = tr_optmgmt_ack(tr, NULL, q, mp, NULL, 0, 0)))
			goto error;
		break;
	}
	sc_put_upper(sc, tr);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);	/* must accept NULL tr */
	return (err);
}
static fastcall int
n_unitdata_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_unitdata_ind_t *p)
{
	int err;
	caddr_t dptr = NULL, sptr = NULL;
	size_t dlen = 0, slen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (likely((slen = p->SRC_length))) {
		if (slen != sc->info.ADDR_size)
			goto badaddr;
		if (unlikely(!MBLKIN(mp, p->SRC_offset, p->SRC_length)))
			goto badaddr;
		sptr = (caddr_t) p + p->SRC_offset;
	}
	if (likely((dlen = p->DEST_length))) {
		if (dlen != sc->info.ADDR_size)
			goto badaddr;
		if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
			goto badaddr;
		dptr = (caddr_t) p + p->DEST_offset;
	}
	return n_recv_msg(sc, q, mp, dptr, dlen, sptr, slen, mp->b_cont);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_uderror_ind: - process N_UDERROR_IND from SCCP
 * @sc: SCCP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 * @p: pointer to primitive
 *
 * Some versions of SCCP return all indications in the N_UDERROR_IND message, including those
 * that correspond to N_STATE_IND, N_PCSTATE_IND and N_NOTICE_IND.  This function determines the
 * correct upward primitive using the ERROR_type included in the primitive.
 */
static fastcall int
n_uderror_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_uderror_ind_t *p)
{
	int err;
	caddr_t dptr = NULL, sptr = NULL;
	size_t dlen = 0, slen = 0;
	struct tr *tr = NULL;
	struct tn *tn = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (likely((dlen = p->DEST_length))) {
		if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
			goto badaddr;
		dptr = (caddr_t) p + p->DEST_offset;
	}
	if ((err = sc_get_upper(sc, q, &tr)))
		goto error;
	switch (p->ERROR_type) {
	case TR_RC_NO_ADDRESS_TYPE_TRANSLATION:
	case TR_RC_NO_ADDRESS_TRANSLATION:
	case TR_RC_SUBSYSTEM_CONGESTION:
	case TR_RC_SUBSYSTEM_FAILURE:
	case TR_RC_UNEQUIPPED_USER:
	case TR_RC_MTP_FAILURE:
	case TR_RC_NETWORK_CONGESTION:
	case TR_RC_UNQUALIFIED:
	case TR_RC_MESSAGE_TRANSPORT_ERROR:
	case TR_RC_LOCAL_PROCESSING_ERROR:
	case TR_RC_NO_REASSEMBLY_AT_DESTINATION:
	case TR_RC_SCCP_FAILURE:
	case TR_RC_SCCP_HOP_COUNTER_VIOLATION:
	case TR_RC_SEGMENTATION_NOT_SUPPORTED:
	case TR_RC_SEGMENTATION_FAILURE:
	case TR_RC_MESSAGE_CHANGE_FAILURE:
	case TR_RC_INVALID_INS_ROUTING_REQUEST:
	case TR_RC_INVALID_INSI_ROUTING_REQUEST:
	case TR_RC_UNAUTHORIZED_MESSAGE:
	case TR_RC_MESSAGE_INCOMPATIBILITY:
	case TR_RC_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING:
	case TR_RC_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFO:
	case TR_RC_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION:
		/* XXX: try to dig transaction ID out of returned message and identify transaction */
		tn = tr_find_uderror(tr, dptr, dlen, sptr, slen, mp->b_cont);
		if ((err = tr_notice_ind(tr, q, mp, tn, dptr, dlen, sptr, slen,
					 p->ERROR_type, mp->b_cont)))
			goto error;
		break;
	case TR_STATUS_USER_IN_SERVICE:
	case TR_STATUS_USER_OUT_OF_SERVICE:
		if ((err = tr_state_ind(tr, q, mp, p->ERROR_type,
					TR_SMI_MULTIPLICITY_UNKNOWN, dptr, dlen)))
			goto error;
		break;
	case TR_STATUS_REMOTE_SCCP_AVAILABLE:
	case TR_STATUS_REMOTE_SCCP_UNAVAILABLE:
	case TR_STATUS_REMOTE_SCCP_UNEQUIPPED:
	case TR_STATUS_REMOTE_SCCP_INACCESSIBLE:
	case TR_STATUS_REMOTE_SCCP_CONGESTED(0):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(1):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(2):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(3):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(4):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(5):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(6):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(7):
	case TR_STATUS_REMOTE_SCCP_CONGESTED(8):
	case TR_STATUS_SIGNALLING_POINT_INACCESSIBLE:
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(0):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(1):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(2):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(3):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(4):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(5):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(6):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(7):
	case TR_STATUS_SIGNALLING_POINT_CONGESTED(8):
	case TR_STATUS_SIGNALLING_POINT_ACCESSIBLE:
		if ((err = tr_pcstate_ind(tr, q, mp, p->ERROR_type, dptr, dlen)))
			goto error;
		break;
	default:
		goto badflag;
	}
	sc_put_upper(sc, tr);
	return (0);
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}

/** 
 * n_notice_ind: - process N_NOTICE_IND from SCCP
 * @sc: SCCP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 * @p: pointer to the primitive
 *
 * Note that the definitions for REPORT_cause are the same values as the definitions for the SCCP
 * RETURN_cause.  The values of the SCCP RETURN_cause are precisely those from the protocol as
 * defined in ITU-T Rec. Q.713 and ANSI T1.112.
 */
static fastcall int
n_notice_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_notice_ind_t *p)
{
	int err;
	caddr_t dptr = NULL, sptr = NULL;
	size_t dlen = 0, slen = 0;
	t_uscalar_t REPORT_cause;
	struct tr *tr = NULL;
	struct tn *tn = NULL;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if ((dlen = p->DEST_length)) {
		if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
			goto badaddr;
		dptr = (caddr_t) p + p->DEST_offset;
	}
	if ((slen = p->SRC_length)) {
		if (unlikely(!MBLKIN(mp, p->SRC_offset, p->SRC_length)))
			goto badaddr;
		sptr = (caddr_t) p + p->SRC_offset;
	}
	if ((err = sc_get_upper(sc, q, &tr)))
		goto error;
	REPORT_cause = p->RETURN_cause;	/* same values */
	/* XXX: attempt to extract transaction ID from returned message */
	tn = tr_find_notice(tr, dptr, dlen, sptr, slen, mp->b_cont);
	if ((err = tr_notice_ind(tr, q, mp, tn, dptr, dlen, sptr, slen, REPORT_cause, mp->b_cont)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_coord_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_coord_ind_t *p)
{
	int err;
	struct tr *tr = NULL;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (likely((alen = p->ADDR_length))) {
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((err = sc_get_upper(sc, q, tr)))
		goto error;
	/* SMI values are the same as SCCP */
	if ((err = tr_coord_ind(tr, q, mp, p->SMI, aptr, alen)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_coord_con(struct sc *sc, queue_t *q, mblk_t *mp, N_coord_con_t *p)
{
	int err;
	struct tr *tr = NULL;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (likely((alen = p->ADDR_length))) {
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((err = sc_get_upper(sc, q, tr)))
		goto error;
	/* SMI values are the same as SCCP */
	if ((err = tr_coord_con(tr, q, mp, p->SMI, aptr, alen)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_state_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_state_ind_t *p)
{
	int err;
	struct tr *tr = NULL;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (likely((alen = p->ADDR_length))) {
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((err = sc_get_upper(sc, q, tr)))
		goto error;
	/* STATUS values and SMI values are the same as SCCP */
	if ((err = tr_state_ind(tr, q, mp, p->STATUS, p->SMI, aptr, alen)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_pcstate_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_pcstate_ind_t *p)
{
	int err;
	struct tr *tr = NULL;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (likely((alen = p->ADDR_length))) {
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((err = sc_get_upper(sc, q, tr)))
		goto error;
	/* STATUS values are the same as SCCP */
	if ((err = tr_pcstate_ind(tr, q, mp, p->STATUS, aptr, alen)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_traffic_ind(struct sc *sc, queue_t *q, mblk_t *mp, N_traffic_ind_t *p)
{
	int err;
	struct tr *tr = NULL;
	caddr_t aptr = NULL;
	size_t alen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (likely((alen = p->ADDR_length))) {
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto badaddr;
		aptr = (caddr_t) p + p->ADDR_offset;
	}
	if ((err = sc_get_upper(sc, q, tr)))
		goto error;
	/* TRAFFIC_mix values are the same as SCCP */
	if ((err = tr_traffic_ind(tr, q, mp, p->TRAFFIC_mix, aptr, alen)))
		goto error;
	sc_put_upper(sc, tr);
	return (0);
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	sc_put_upper(sc, tr);
	return (err);
}
static fastcall int
n_other_ind(struct sc *sc, queue_t *q, mblk_t *mp, np_ulong *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	goto badprim;
      badprim:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * INPUT-OUTPUT CONTROLS
 * --------------------------------------------------------------------------
 */

static void
tr_copy_done(struct tr *tr, queue_t *q, mblk_t *mp, int err)
{
	if (err == 0) {
		LOGIO(tr, "<- M_IOCACK(%s)", tr_iocname(((struct iocblk *)mp->b_rptr)->ioc_cmd));
	} else {
		LOGIO(tr, "<- M_IOCNAK(%s)", tr_iocname(((struct iocblk *)mp->b_rptr)->ioc_cmd));
	}
	mi_copy_done(q, mp, err);
}
static void
tr_copyin(struct tr *tr, queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len)
{
	LOGIO(tr, "<- M_COPYIN(%s)", tr_iocname(((struct iocblk *)mp->b_rptr)->ioc_cmd));

	mi_copyin(q, mp, uaddr, len);
}
static void
tr_copyin_n(struct tr *tr, queue_t *q, mblk_t *mp, caddr_t offset, size_t len)
{
	LOGIO(tr, "<- M_COPYIN(%s)", tr_iocname(((struct iocblk *)mp->b_rptr)->ioc_cmd));

	mi_copyin_n(q, mp, offset, len);
}
static void
tr_copyout(struct tr *tr, queue_t *q, mblk_t *mp)
{
	LOGIO(tr, "<- M_COPYOUT(%s)", tr_iocname(((struct iocblk *)mp->b_rptr)->ioc_cmd));

	mi_copyout(q, mp);
}

static int
tr_i_link(struct tr *mgr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct tr *tr;
	struct sc *sc = NULL;
	int err;
	N_info_req_t *p;
	mblk_t *rp = NULL;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!(sc = tr_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, 0)))
		goto enomem;

	sc_init(sc, tr);

	write_lock_irqsave(&tr_lock, flags);
	if ((sc->tr_next = tr->sc_list))
		sc->tr_next->tr_prev = &sc->tr_next;
	sc->tr_prev = &tr->sc_list;
	sc->mgr = tr;
	mi_attach(l->l_qtop, (caddr_t) sc);
	write_unlock_irqrestore(&tr_lock, flags);

	tr_copy_done(tr, q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->PRIM_type = N_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(sc->wq, rp);
	/* Immediate info request. */
	return (0);
      enxio:
	err = ENXIO;
	goto unlock_error;
      unlock_error:
	write_unlock_irqrestore(&tr_lock, flags);
	goto error;
      enomem:
	err = -ENOMEM;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(rp);
	return (err);
}
static int
tr_i_plink(struct tr *mgr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sc *sc = NULL;
	int err = EPERM;

	if (ioc->ioc_cr->cr_uid == 0) {
		err = ENOMEM;
		if (!(sc = tr_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, 0)))
			err = 0;
	}

	tr_copy_done(tr, q, mp, 0);
	return (0);
}
static int
tr_i_unlink(struct tr *mgr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sc *sc;

	sc = SC_PRIV(l->l_qtop);
	tr_free_link(sc);
	tr_copy_done(tr, q, mp, 0);
	return (0);
}
static int
tr_i_punlink(struct tr *mgr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sc *sc;
	int err = EPERM;

	if (ioc->ioc_cr->cr_uid == 0) {
		sc = SC_PRIV(l->l_qtop);
		tr_free_link(c);
		err = 0;
	}
	tr_copy_done(tr, q, mp, err);
	return (0);
}

static int
tr_i_ioctl(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		return tr_i_link(tr, q, mp);
	case _IOC_NR(I_PLINK):
		return tr_i_plink(tr, q, mp);
	case _IOC_NR(I_UNLINK):
		return tr_i_unlink(tr, q, mp);
	case _IOC_NR(I_PUNLINK):
		return tr_i_punlink(tr, q, mp);
	}
	return (-EINVAL);
}

union tcap_ioctls {
	struct {
		struct tcap_option option;
		struct tcap_option_obj obj;
	} opt_conf;
	struct {
		struct tcap_config config;
		struct tcap_conf_obj obj;
	} conf;
	struct {
		struct tcap_statem statem;
		struct tcap_statem_obj obj;
	} statem;
	struct {
		struct tcap_stats stats;
		struct tcap_stats_obj obj;
	} stats;
	struct {
		struct tcap_notify notify;
		struct tcap_notify_obj obj;
	} notify;
	struct tcap_mgmt mgmt;
	struct tcap_pass pass;
};

static int
tr_tcap_ioctl(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCSOPTION):
		size = sizeof(struct tcap_option);
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
	case _IOC_NR(TCAP_IOCLCONFIG):
		size = sizeof(struct tcap_config);
		break;
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCCMRESET):
		size = sizeof(struct tcap_statem);
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCSSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
		size = sizeof(struct tcap_stats);
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		size = sizeof(struct tcap_notify);
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		size = sizeof(struct tcap_mgmt);
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		size = sizeof(struct tcap_pass);
		break;
	default:
		tr_copy_done(tr, q, mp, EOPNOTSUPP);
		return (0);
	}
	if (size == -1)
		tr_copyout(tr, q, mp);
	else
		tr_copyin(tr, q, mp, NULL, size);
	return (0);
}
static int
tr_tcap_copyin(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
	case _IOC_NR(TCAP_IOCGNOTIFY):
		len = sizeof(union tcap_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
#ifdef TCAP_IOCLCONFIG
	case _IOC_NR(TCAP_IOCLCONFIG):
	{
		struct tcap_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
#endif				/* TCAP_IOCLCONFIG */
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_options(bp);
		break;
	case _IOC_NR(TCAP_IOCSOPTION):
		err = tcap_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
		err = tcap_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCTCONFIG):
		err = tcap_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCCCONFIG):
		err = tcap_size_conf(dp->b_rptr, &size);
		break;
#ifdef TCAP_IOCLCONFIG
	case _IOC_NR(TCAP_IOCLCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = tcap_lst_conf(bp);
		break;
#endif				/* TCAP_IOCLCONFIG */
	case _IOC_NR(TCAP_IOCGSTATEM):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_statem(bp);
		break;
	case _IOC_NR(TCAP_IOCCMRESET):
		err = EOPNOTSUPP; /* later */ ;
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
		if (bp == NULL)
			goto enobufs;
		err = tcap_do_statsp(bp, TCAP_GET);
		break;
	case _IOC_NR(TCAP_IOCSSTATSP):
		err = tcap_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCGSTATS):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_stats(bp, false);
		break;
	case _IOC_NR(TCAP_IOCCSTATS):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_stats(bp, true);
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
		if (bp == NULL)
			goto enobufs;
		err = tcap_do_notify(bp, 0, TCAP_GET);
		break;
	case _IOC_NR(TCAP_IOCSNOTIFY):
		err = tcap_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCCNOTIFY):
		err = tcap_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		err = tcap_action(q, dp);
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0) {
		tr_copy_done(tr, q, mp, err);
		return (0);
	}
	if (size == -1)
		tr_copyout(tr, q, mp);
	else
		tr_copyin_n(tr, q, mp, 0, size);
	return (0);
}
static int
tr_tcap_copyin2(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSOPTION):
		err = tcap_set_options(bp);
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
		err = tcap_set_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCTCONFIG):
		err = tcap_test_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCCCONFIG):
		err = tcap_commit_conf(bp);
		break;
#ifdef TCAP_IOCLCONFIG
	case _IOC_NR(TCAP_IOCLCONFIG):
		err = EPROTO;
		break;
#endif				/* TCAP_IOCLCONFIG */
	case _IOC_NR(TCAP_IOCGSTATEM):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCCMRESET):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSSTATSP):
		err = tcap_do_statsp(bp, TCAP_CHA);
		break;
	case _IOC_NR(TCAP_IOCGSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCCSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSNOTIFY):
		err = tcap_do_notify(bp, 0, TCAP_ADD);
		break;
	case _IOC_NR(TCAP_IOCCNOTIFY):
		err = tcap_do_notify(bp, 0, TCAP_DEL);
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCCPASS):
	{
		struct tcap_pass *p = (typeof(p)) dp->b_rptr;

		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		tr_copy_done(tr, q, mp, err);
	if (err == 0)
		tr_copyout(tr, q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}
static int
tr_tcap_copyout(struct tr *tr, queue_t *q, mblk_t *mp)
{
	tr_copyout(tr, q, mp);
	return (0);
}

static int
tr_sccp_ioctl(struct tr *tr, queue_t *q, mblk_t *mp)
{
}
static int
tr_sccp_copyin(struct tr *tr, queue_t *q, mblk_t *mp)
{
}
static int
tr_sccp_copyin2(struct tr *tr, queue_t *q, mblk_t *mp)
{
}
static int
tr_sccp_copyout(struct tr *tr, queue_t *q, mblk_t *mp)
{
}

/**
 * tr_other_ioctl: - process other input-output controls
 * @tr: TR private structure
 * @q: active queue (upper write queue)
 * @mp: the input-output control message
 *
 * This function simply passes input-output controls that are not for our level down to the lower
 * level drivers beneath the multiplexing driver.  The input-output control command and id are
 * stored against the upper private structure so that input-output control responses are passed to
 * the correct originating stream.
 */
static int
tr_other_ioctl(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sc *sc = NULL;
	int err;

	if ((err = tr_get_lower(tr, &sc)) == 0) {
		tr->ioc_cmd = ioc->ioc_cmd;
		tr->ioc_id = ioc->ioc_id;
		LOGTX(sc, "%s: %s", __FUNCTION__, tr_iocname(ioc->ioc_cmd));
		putnext(sc->wq, mp);
		tr_put_lower(tr, sc);
	}
	return (err);
}

/**
 * sc_other_ioctl: = process other input-output controls
 * @sc: SCCP private structure
 * @q: active queue (lower read queue)
 * @mp: the input-output control message
 *
 * This function is the other half of tr_other_ioctl(): it passes input-output control responses
 * that are not for use to the upper level uers above the multiplexing driver.  The input-output
 * control command and id are compared against those stored in the upper private structure so that
 * input-output control responess are passed to the correct originating stream.
 */
static int
sc_other_ioctl(struct sc *sc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tr *tr = NULL;
	int err = EINVAL;

	read_lock(&tr_lock);
	for (tr = sc->tr_list; tr; tr = tr->sc_next) {
		if (tr->ioc_cmd == ioc->ioc_cmd && tr->ioc_id == ioc->ioc_id) {
			putnext(tr->rq, mp);
			err = 0;
			break;
		}
	}
	read_unlock(&tr_lock);
	return (err);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

/*
 * M_IOCTL Handling
 * --------------------------------------------------------------------------
 */
static noinline __unlikely int
__tr_w_ioctl(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tr *tr;
	int err;

	LOGIO(tr, "-> M_IOCTL(%s)", tr_iocname(ioc->ioc_cmd));

	if (likely(!!mp->b_cont)) {
		switch (_IOC_TYPE(ioc->ioc_cmd)) {
		case __SID:
			err = tr_i_ioctl(tr, q, mp);
			break;
		case TCAP_IOC_MAGIC:
			err = tr_tcap_ioctl(tr, q, mp);
			break;
		case SCCP_IOC_MAGIC:
			err = tr_sccp_ioctl(tr, q, mp);
			break;
		default:
			/* pass all others down */
			err = tr_other_ioctl(tr, q, mp);
			break;
		}
	} else
		err = -EFAULT;
	if (err < 0) {
		switch (-err) {
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EBUSY:
		case -EDEADLK:
			break;
		default:
			tr_copy_done(tr, q, mp, -err);
			return (0);
		}
	}
	return (err);
}
static noinline __unlikely int
tr_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct tr *tr;
	int err;

	if (!!(tr = tr_trylock(q))) {
		err = __tr_w_ioctl(tr, q, mp);
		tr_unlock(tr);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_IOCDATA Handling
 * --------------------------------------------------------------------------
 */
static noinline __unlikely int
__tr_w_iocdata(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;
	int err;

	LOGIO(tr, "-> M_IOCDATA(%s)", tr_iocname(cp->cp_cmd));

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case TCAP_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			err = tr_tcap_copyin(tr, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 2):
			err = tr_tcap_copyin2(tr, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			err = tr_tcap_copyout(tr, q, mp, dp);
			break;
		case -1:
			return (0);
		default:
			err = -EPROTO;
			break;
		}
		break;
	case SCCP_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			err = tr_sccp_copyin(tr, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 2):
			err = tr_sccp_copyin2(tr, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			err = tr_sccp_copyout(tr, q, mp, dp);
			break;
		case -1:
			return (0);
		default:
			err = -EPROTO;
			break;
		}
		break;
	default:
		/* pass all others down */
		err = tr_other_ioctl(tr, q, mp);
		break;
	}
	if (err < 0) {
		switch (-err) {
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EBUSY:
		case -EDEADLK:
			break;
		default:
			tr_copy_done(tr, q, mp, -err);
			return (0);
		}
	}
	return (err);
}
static noinline __unlikely int
tr_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct tr *tr;
	int err;

	if (!!(tr = tr_trylock(q))) {
		err = __tr_w_iocdata(tr, q, mp);
		tr_unlock(tr);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_CTL and M_PCCTL Handling
 * --------------------------------------------------------------------------
 */

/*
 * M_PROTO and M_PCPROTO Handling
 * --------------------------------------------------------------------------
 */
static noinline __unlikely int
tr_w_proto_return(struct tr *tr, queue_t *q, mblk_t *mp, int err)
{
	t_scalar_t type = MBLKIN(mp, 0, sizeof(type)) ? *(typeof(type) *) mp->b_rptr : -1;

	if (err < 0)
		tr_restore_state(tr);
	switch (__builtin_expect(-err, EDEADLK)) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (err);
	default:
		freemsg(mp);
	case 0:
		LOGNO(tr, "Primitive <%s> on upper write queue generated error [%s]",
		      tr_primname(type), tr_errname(err));
		return (0);
	case EPROTO:
	case EFAULT:
		return m_error(tr, q, mp, -err);
	}
}
noinline fastcall __unlikely int
sc_r_proto_return(struct sc *sc, queue_t *q, mblk_t *mp, int err)
{
	np_long type = MBLKIN(mp, 0, sizeof(type)) ? *(typeof(type) *) mp->b_rptr : -1;

	if (err < 0)
		sc_restore_state(sc);
	switch (__builtin_expect(-err, EDEADLK)) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (err);
	default:
		freemsg(mp);
		/* fall through */
	case 0:
		LOGERR(sc, "Primitive <%s> on lower read queue generated error [%s]",
		       sc_primname(type), sc_errname(err));
		return (0);
	case EPROTO:
	case EFAULT:
		return m_error_ind(sc, q, mp, -err);
	}
}

noinline fastcall int
__tr_w_proto_slow(union TR_primitives *p, struct tr *tr, queue_t *q, mblk_t *mp, t_scalar_t type)
{
	LOGRX(tr, "-> %s", tr_primname(type));
	switch (type) {
	case TR_INFO_REQ:
		return tr_info_req(tr, q, mp, &p->info_req);
	case TR_BIND_REQ:
		return tr_bind_req(tr, q, mp, &p->bind_req);
	case TR_UNBIND_REQ:
		return tr_unbind_req(tr, q, mp, &p->unbind_req);
	case TR_OPTMGMT_REQ:
		return tr_optmgmt_req(tr, q, mp, &p->optmgmt_req);
	case TR_UNI_REQ:
		return tr_uni_req(tr, q, mp, &p->uni_req);
	case TR_BEGIN_REQ:
		return tr_begin_req(tr, q, mp, &p->begin_req);
	case TR_BEGIN_RES:
		return tr_begin_res(tr, q, mp, &p->begin_res);
	case TR_CONT_REQ:
		return tr_cont_req(tr, q, mp, &p->cont_req);
	case TR_END_REQ:
		return tr_end_req(tr, q, mp, &p->end_req);
	case TR_ABORT_REQ:
		return tr_abort_req(tr, q, mp, &p->abort_req);
	case TR_ADDR_REQ:
		return tr_addr_req(tr, q, mp, &p->addr_req);
	case TR_CAPABILITY_REQ:
		return tr_capability_req(tr, q, mp, &p->capability_req);
	case TR_COORD_REQ:
		return tr_coord_req(tr, q, mp, &p->coord_req);
	case TR_COORD_RES:
		return tr_coord_res(tr, q, mp, &p->coord_res);
	case TR_STATE_REQ:
		return tr_state_req(tr, q, mp, &p->state_req);
	default:
		return tr_other_req(tr, q, mp, &p->type);
	}
}
noinline fastcall int
__sc_r_proto_slow(struct sc *sc, queue_t *q, mblk_t *mp, union N_primitives *p, np_long type)
{
	LOGRX(sc, "%s <-", sc_primname(type));
	switch (type) {
	case N_INFO_ACK:
		return n_info_ack(sc, q, mp, &p->info_ack);
	case N_BIND_ACK:
		return n_bind_ack(sc, q, mp, &p->bind_ack);
	case N_ERROR_ACK:
		return n_error_ack(sc, q, mp, &p->error_ack);
	case N_OK_ACK:
		return n_ok_ack(sc, q, mp, &p->ok_ack);
	case N_UNITDATA_IND:
		return n_unitdata_ind(sc, q, mp, &p->unitdata_ind);
	case N_UDERROR_IND:
		return n_uderror_ind(sc, q, mp, &p->uderror_ind);

/* Note that this module can be pushed over any old NPI stream but there may be no support for
   the extra management SCCP N-primitives.   Primitives N_NOTICE_IND, N_STATE_IND, N_PCSTATE_IND
   and N_TRAFFIC_IND can all be supplied with appropriately coded N_UDERROR_IND primitives; but
   there is no equivalent in OSI for N_COORD_REQ and N_COORD_IND.  The user will have to use
   input-output controls instead. */

	case N_NOTICE_IND:
		return n_notice_ind(sc, q, mp, &p->notice_ind);
	case N_COORD_IND:
		return n_coord_ind(sc, q, mp, &p->coord_ind);
	case N_COORD_CON:
		return n_coord_con(sc, q, mp, &p->coord_con);
	case N_STATE_IND:
		return n_state_ind(sc, q, mp, &p->state_ind);
	case N_PCSTATE_IND:
		return n_pcstate_ind(sc, q, mp, &p->pcstate_ind);
	case N_TRAFFIC_IND:
		return n_traffic_ind(sc, q, mp, &p->traffic_ind);
	default:
		return n_other_ind(sc, q, mp, &p->type);
	}
}

noinline fastcall int
tr_w_proto_slow(queue_t *q, mblk_t *mp, t_scalar_t prim)
{
	struct tr *tr;
	int err;

	if (!!(tr = tr_trylock(q))) {
		err = __tr_w_proto_slow(tr, q, mp);
		tr_unlock(tr);
	} else
		err = -EDEADLK;
	return (err);
}
noinline fastcall int
sc_r_proto_slow(queue_t *q, mblk_t *mp, t_scalar_t prim)
{
	struct sc *sc;
	int err;

	if (!!(sc = sc_trylock(q))) {
		err = __sc_r_proto_slow(sc, q, mp);
		sc_unlock(sc);
	} else
		err = -EDEADLK;
	return (err);
}

static fastcall __hot_read int
__tr_w_proto(struct tr *tr, queue_t *q, mblk_t *mp)
{
	union TR_primitives *p = (typeof(p)) mp->b_rptr;
	register int err;

	if (likely(MBLKIN(mp, 0, sizeof(p->type)))) {
		register int type = p->type;

		tr_save_state(tr);

		if (likely
		    ((1 << type) &
		     ((1 << TR_BEGIN_REQ) | (1 << TR_END_REQ) | (1 << TR_CONT_REQ) |
		      (1 << TR_UNI_REQ)))) {
			/* fast paths for data */
			switch (__builtin_expect(type, TR_BEGIN_REQ)) {
			case TR_BEGIN_REQ:
				if (unlikely((err = tr_begin_req(p, tr, q, mp)) != 0))
					goto error;
				break;
			case TR_END_REQ:
				if (unlikely((err = tr_end_req(p, tr, q, mp)) != 0))
					goto error;
				break;
			case TR_CONT_REQ:
				if (unlikely((err = tr_cont_req(p, tr, q, mp)) != 0))
					goto error;
				break;
			case TR_UNI_REQ:
				if (unlikely((err = tr_uni_req(p, tr, q, mp)) != 0))
					goto error;
				break;
			}
		} else {
			if (unlikely((err = __tr_w_proto_slow(p, tr, q, mp, type)) != 0))
				goto error;
		}
		return (0);
	} else
		err = -EPROTO;
      error:
	return tr_w_proto_return(tr, q, mp, err);
}
static fastcall __hot_write int
__sc_r_proto(struct sc *sc, queue_t *q, mblk_t *mp)
{
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	int err;

	if (likely(MBLKIN(mp, 0, sizeof(p->type)))) {
		register int type = p->type;

		sc_save_state(sc);
		if ((1 << type) &
		    ((1 << N_UNITDATA_IND) | (1 << N_UDERROR_IND) | (1 << N_NOTICE_IND))) {
			/* fast paths for data */
			switch (__builtin_expect(type, N_UNITDATA_IND)) {
			case N_UNITDATA_IND:
				if (unlikely
				    ((err = n_unitdata_ind(sc, q, mp, &p->unitdata_ind)) != 0))
					goto error;
				break;
			case N_UDERROR_IND:
				if (unlikely
				    ((err = n_uderror_ind(sc, q, mp, &p->uderror_ind)) != 0))
					goto error;
				break;
			case N_NOTICE_IND:
				if (unlikely((err = n_notice_ind(sc, q, mp, &p->notice_ind)) != 0))
					goto error;
				break;
			}
		} else {
			if (unlikely((err = __sc_r_proto_slow(sc, q, mp, p, type)) != 0))
				goto error;
		}
		return (0);
	} else
		err = -EPROTO;
      error:
	return sc_r_proto_return(sc, q, mp, err);
}
static fastcall __hot_read int
__tr_w_data(struct tr *tr, queue_t *q, mblk_t *mp)
{
	int err;

	tr_save_state(tr);
	if (unlikely((err = tr_data_req(sc, q, mp)) != 0))
		goto error;
	return (0);
      error:
	return tr_w_proto_return(tr, q, mp, err);

}
static fastcall __hot_write int
__sc_r_data(struct sc *sc, queue_t *q, mblk_t *mp)
{
	int err;

	sc_save_state(sc);
	if (unlikely((err = n_data_ind(sc, q, mp)) != 0))
		goto error;
	return (0);
      error:
	return sc_r_proto_return(sc, q, mp, err);
}

static fastcall __hot_put int
tr_w_proto(queue_t *q, mblk_t *mp)
{
	union TR_primitives *p = (typeof(p)) mp->b_rptr;
	register int err;

	if (likely(MBLKIN(mp, 0, sizeof(p->type)))) {
		register int type = p->type;

		if (likely
		    ((1 << type) &
		     ((1 << TR_BEGIN_REQ) | (1 << TR_END_REQ) | (1 << TR_CONT_REQ) |
		      (1 << TR_UNI_REQ))))
			return (-EAGAIN);	/* always queue data from put procedure */
		err = tr_w_proto_slow(q, mp, type);
	} else
		err = -EPROTO;
      error:
	return tr_w_proto_return(tr, q, mp, err);
}
static fastcall __hot_get int
sc_r_proto(queue_t *q, mblk_t *mp)
{
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	register int err;

	if (likely(MBLKIN(mp, 0, sizeof(p->type)))) {
		if ((1 << type) &
		    ((1 << N_UNITDATA_IND) | (1 << N_UDERROR_IND) | (1 << N_NOTICE_IND)))
			return (-EAGAIN);	/* always queue data from put procedure */
		err = sc_r_proto_slow(q, mp, type);
	} else
		err = -EPROTO;
      error:
	return sc_r_proto_return(sc, q, mp, err);
}

/*
 * M_DATA and M_HPDATA Handling
 * --------------------------------------------------------------------------
 */
static fastcall __hot_put int
tr_w_data(queue_t *q, mblk_t *mp)
{
	return (-EAGAIN);	/* always queue data from put procedure */
}
static fastcall __hot_in int
sc_r_data(queue_t *q, mblk_t *mp)
{
	return (-EAGAIN);	/* always queue data from put procedure */
}

/*
 * M_FLUSH HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
tr_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
	} else
		freemsg(mp);
	return (0);
}
noinline fastcall __unlikely int
sc_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		if (!(mp->b_flag & MSGNOLOOP)) {
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
		} else
			freemsg(mp);
	} else
		freemsg(mp);
	return (0);
}

/*
 * UNKNOWN STREAMS MESSAGE HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
tr_w_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
noinline fastcall __unlikely int
sc_r_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * STREAMS MESSAGE DISCRIMINATION
 * --------------------------------------------------------------------------
 */
static fastcall int
tr_wsrv_msg(struct tr *tr, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __tr_w_proto(tr, q, mp);
	case M_DATA:
		return __tr_w_data(tr, q, mp);
	default:
		return tr_w_other(q, mp);
	}
}
static fastcall int
sc_rsrv_msg(struct tr *tr, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sc_r_proto(tr, q, mp);
	case M_DATA:
		return __sc_r_data(tr, q, mp);
	default:
		return sc_r_other(q, mp);
	}
}
static fastcall int
tr_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return tr_w_proto(q, mp);
	case M_DATA:
		return tr_w_data(q, mp);
	case M_FLUSH:
		return tr_w_flush(q, mp);
	default:
		return tr_w_other(q, mp);
	}
}
static fastcall int
sc_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sc_r_proto(q, mp);
	case M_DATA:
		return sc_r_data(q, mp);
	case M_FLUSH:
		return sc_r_flush(q, mp);
	case M_ERROR:
		return sc_r_error(q, mp);
	case M_HANGUP:
		return sc_r_hangup(q, mp);
	default:
		return sc_r_other(q, mp);
	}
}

/*
 * QUEUE PUT AND SERVICE PROCEDURES
 * --------------------------------------------------------------------------
 */
static streamscall int
tr_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(tr_wput_msg(q, mp))) {
		tr_mstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}
static streamscall int
tr_wsrv(queue_t *q)
{
	struct tr *tr;
	mblk_t *mp;

	if (likely(!!(tr = tr_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(tr_wsrv_msg(tr, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(tr, "write queue stalled");
				break;
			}
		}
		tr_unlock(tr);
	}
	return (0);
}
static streamscall int
tr_rsrv(queue_t *q)
{
	struct tr *tr;

	if (likely(!!(tr = tr_trylock(q)))) {
		struct sc *sc = NULL;

		if (tr_get_lower(tr, &sc) == 0) {
			/* back-enable across multiplexer */
			qenable(sc->rq);
			tr_put_lower(tr, sc);
		}
		tr_unlock(tr);
	}
	return (0);
}
static streamscall int
sc_wsrv(queue_t *q)
{
	struct sc *sc;

	if (likely(!!(sc = sc_trylock(q)))) {
		struct tr *tr = NULL;

		if (sc_get_upper(sc, &tr) == 0) {
			/* back-enable across multiplexer */
			qenable(tr->wq);
			sc_put_upper(sc, tr);
		}
		sc_unlock(sc);
	}
	return (0);
}
static streamscall int
sc_rsrv(queue_t *q)
{
	struct sc *sc;
	mblk_t *mp;

	if (likely(!!(sc = sc_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(sc_rsrv_msg(sc, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(sc, "read queue stalled");
				break;
			}
		}
		sc_unlock(sc);
	}
	return (0);
}
static streamscall int
sc_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(sc_rput_msg(q, mp))) {
		sc_mstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}

/*
 * QUEUE OPEN and CLOSE routines
 * --------------------------------------------------------------------------
 */
static streamscall int
trmux_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	struct tr *tr;
	caddr_t ptr;
	int err;
	minor_t cminor = getminor(*devp);

	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next != NULL)
		goto enxio;	/* can't be pushed as module */
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR)
		goto enxio;
	if (!mi_set_sth_lowat(q, 0))
		goto enobufs;
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		goto enobufs;
	if (!(tr = (struct tr *) (ptr = mi_open_alloc_cache(priv_cachep, GFP_KERNEL))))
		goto enomem;
	bzero(tr, sizeof(*tr));
	tr->rq = RD(q);
	tr->wq = WR(q);
	tr->cred = *crp;
	tr->dev = *devp;
	sflag = CLONEOPEN;
	cminor = FREE_CMINOR;
	*devp = makedevice(getmajor(*devp), cminor);
	write_lock_irqsave(&tr_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &tr_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err = mi_open_link(&tr_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&tr_lock, flags);
	tr_init(tr);
	qprocson(q);
	return (0);
      enxio:
	err = ENXIO;
	goto error;
      enobufs:
	err = ENOBUFS;
	goto error;
      enomem:
	err = ENOMEM;
	goto error;
      unlock_free:
	mi_close_free_cache(priv_cachep, ptr);
	write_unlock_irqrestore(&tr_lock, flags);
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static streamscall int
trmux_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;
	struct tr *tr = PRIV(q);
	caddr_t ptr = (caddr_t) tr;
	int err;

	write_lock_irqsave(&tr_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &tr_lock, &flags);
	qprocsoff(q);
	tr_term(tr);
	tr->rq = NULL;
	tr->wq = NULL;
	err = mi_close_comm(&tr_opens, q);
	write_unlock_irqrestore(&tr_lock, flags);
	if (err)
		cmn_err(CE_WARN, "%s: error closing stream, err = %d", __FUNCTION__, err);
	return (err);
}

/*
 * STREAMS DEFINITIONS
 * --------------------------------------------------------------------------
 */

static struct qinit tr_rinit = {
	/* never put to upper read queue */
	.qi_srvp = tr_rsrv,	/* for back-enable */
	.qi_qopen = trmux_qopen,
	.qi_qclose = trmux_qclose,
	.qi_minfo = &tr_minfo,
	.qi_mstat = &tr_mstat,
};
static struct qinit tr_winit = {
	.qi_putp = tr_wput,
	.qi_srvp = tr_wsrv,
	.qi_minfo = &tr_minfo,
	.qi_mstat = &tr_mstat,
};
static struct qinit sc_rinit = {
	.qi_putp = sc_rput,
	.qi_srvp = sc_rsrv,
	.qi_qopen = sc_link,
	.qi_qclose = sc_unlink,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_mstat,
};
static struct qinit sc_winit = {
	/* never put to lower write queue */
	.qi_srvp = sc_wsrv,	/* for back-enable */
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_mstat,
};

MODULE_STATIC struct streamtab trmuxinfo = {
	.st_rdinit = &tr_rinit,
	.st_wrinit = &tr_winit,
	.st_muxrinit = &sc_rinit,
	.st_muxwinit = &sc_winit,
};

STATIC struct cdevsw tr_cdev = {
	.d_name = DRV_NAME,
	.d_str = &trmuxinfo,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/* mgr minor for management access */
static struct devnode tr_node_mgr = {
	.n_name = "mgr",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/* tr minor for basic TRI access */
static struct devnode tr_node_tr = {
	.n_name = "tr",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/* tp minor for basic TPI access */
static struct devnode tr_node_tp = {
	.n_name = "tp",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
static struct strapush tr_push_tp = {
	.ap_cmd = SAD_ONE,
	.sap_major = major,
	.sap_minor = TC_CMINOR,
	.sap_lastminor = TC_CMINOR,
	.sap_npush = 1,
	.sap_list = {
		     [0] = "tptr",
		     };
};


/* tc minor to autopush tcmod */
static struct devnode tr_node_tc = {
	.n_name = "tc",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
static struct strapush tr_push_tc = {
	.ap_cmd = SAD_ONE,
	.sap_major = major,
	.sap_minor = TC_CMINOR,
	.sap_lastminor = TC_CMINOR,
	.sap_npush = 1,
	.sap_list = {
		     [0] = "tcmod",
		     };
};

/* map minor to autopush tcmod and mapmod */
static struct devnode tr_node_map = {
	.n_name = "map",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
static struct strapush tr_push_map = {
	.ap_cmd = SAD_ONE,
	.sap_major = major,
	.sap_minor = MAP_CMINOR,
	.sap_lastminor = MAP_CMINOR,
	.sap_npush = 2,
	.sap_list = {
		     [0] = "tcmod",
		     [1] = "mapmod",
		     };
};

modID_t modid = DRV_ID;
major_t major = CMAJOR_0;

/*
 * Kernel module initialization
 * --------------------------------------------------------------------------
 */

MODULE_STATIC int __init
trmuxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = tr_init_caches()) < 0) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", __FUNCTION__, err);
		return (err);
	}
	if ((err = register_strdev(&tr_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not regsiter driver, err = %d", __FUNCTION__, err);
		goto no_major;
	}
	if (major == 0)
		major = err;
	if ((err = register_strnod(&tr_cdev, &tr_node_mgr, MGR_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register MGR_CMINOR, err = %d", __FUNCTION__, err);
		goto no_mgr_cminor;
	}
	if ((err = register_strnod(&tr_cdev, &tr_node_tr, TR_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register TR_CMINOR, err = %d", __FUNCTION__, err);
		goto no_tr_cminor;
	}
	if ((err = register_strnod(&tr_cdev, &tr_node_tp, TP_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register TP_CMINOR, err = %d", __FUNCTION__, err);
		goto no_tp_cminor;
	}
	if ((err = register_strnod(&tr_cdev, &tr_node_tc, TC_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register TC_CMINOR, err = %d", __FUNCTION__, err);
		goto no_tc_cminor;
	}
	if ((err = register_strnod(&tr_cdev, &tr_node_map, MAP_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register MAP_CMINOR, err = %d", __FUNCTION__, err);
		goto no_map_cminor;
	}
	if ((err = autopush_add(&tr_push_tp)) < 0) {
		cmn_err(CE_WARN, "%s: could not add autopush for TP_CMINOR, err = %d",
			__FUNCTION__, err);
		goto no_apush_tp;
	}
	if ((err = autopush_add(&tr_push_tc)) < 0) {
		cmn_err(CE_WARN, "%s: could not add autopush for TC_CMINOR, err = %d",
			__FUNCTION__, err);
		goto no_apush_tc;
	}
	if ((err = autopush_add(&tr_push_map)) < 0) {
		cmn - err(CE_WARN, "%s: could not add autopush for MAP_CMINOR, err = %d",
			  __FUNCTION__, err);
		goto no_apush_map;
	}
	if (major == 0)
		goto lost_major;
	return (0);

      lost_major:
	autopush_del(&tr_push_map);
      no_apush_map:
	autopush_del(&tr_push_tc);
      no_apush_tc:
	autopush_del(&tr_push_tp);
      no_apush_tp:
	unregister_strnod(&tr_cdev, MAP_CMINOR);
      no_map_cminor:
	unregister_strnod(&tr_cdev, TC_CMINOR);
      no_tc_cminor:
	unregister_strnod(&tr_cdev, TP_CMINOR);
      no_tp_cminor:
	unregister_strnod(&tr_cdev, TR_CMINOR);
      no_tr_cminor:
	unregister_strnod(&tr_cdev, MGR_CMINOR);
      no_mgr_cminor:
	unregister_strdev(&tr_cdev, major);
      no_major:
	tr_term_caches();
	return (err);
}

MODULE_STATIC void __exit
trmuxexit(void)
{
	int err;

	if ((err = autopush_del(&tr_push_map)) < 0)
		cmn_err(CE_WARN, "%s: could not delete autopush for MAP_CMINOR, err = %d",
			__FUNCTION__, err);
	if ((err = autopush_del(&tr_push_tp)) < 0)
		cmn_err(CE_WARN, "%s: could not delete autopush for TP_CMINOR, err = %d",
			__FUNCTION__, err);
	if ((err = autopush_del(&tr_push_tc)) < 0)
		cmn_err(CE_WARN, "%s: could not delete autopush for TC_CMINOR, err = %d",
			__FUNCTION__, err);
	if ((err = unregister_strnod(&tr_cdev, MAP_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister MAP_CMINOR, err = %d", __FUNCTION__,
			err);
	if ((err = unregister_strnod(&tr_cdev, TC_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister TC_CMINOR, err = %d", __FUNCTION__, err);
	if ((err = unregister_strnod(&tr_cdev, TP_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister TP_CMINOR, err = %d", __FUNCTION__, err);
	if ((err = unregister_strnod(&tr_cdev, TR_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister TR_CMINOR, err = %d", __FUNCTION__, err);
	if ((err = unregister_strnod(&tr_cdev, MGR_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister MGR_CMINOR, err = %d", __FUNCTION__,
			err);
	if ((err = unregister_strdev(&tr_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", __FUNCTION__, err);
	if ((err = tr_term_caches()) < 0)
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", __FUNCTION__, err);
	return;
}

module_init(trmuxinit);
module_exit(trmuxexit);
