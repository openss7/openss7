/*****************************************************************************

 @(#) $RCSfile: clns.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/08/26 09:19:05 $

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

 Last Modified $Date: 2006/08/26 09:19:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: clns.c,v $
 Revision 0.9.2.7  2006/08/26 09:19:05  brian
 - better release file generation

 Revision 0.9.2.6  2006/08/23 11:08:11  brian
 - changes for compile

 Revision 0.9.2.5  2006/07/24 09:01:38  brian
 - results of udp2 optimizations

 Revision 0.9.2.4  2006/07/11 12:32:04  brian
 - added ISO and other implementations to distribution

 Revision 0.9.2.3  2006/04/13 18:32:48  brian
 - working up DL and NP drivers.

 Revision 0.9.2.2  2006/04/13 01:51:43  brian
 - starting CLNL driver

 Revision 0.9.2.1  2006/04/12 20:36:01  brian
 - added some experimental drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: clns.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/08/26 09:19:05 $"

static char const ident[] =
    "$RCSfile: clns.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/08/26 09:19:05 $";

/*
 *  This is an X.233 CLNS driver.  This is an NPI driver that can be pushed over or link a DLPI
 *  stream.  DLPI streams can be DL_ETH, DL_CMSACD or DL_CMSA or DL_IP and this driver will use
 *  various methods for transporting CLNS.
 *
 *  Actually, this is a CLNL driver.  It does not have to link DL streams underneath unless there is
 *  only a DL stream for the link driver.  This hooks into the Linux device independent packet layer
 *  on a number of levels:
 *
 *  Ethernet: - hook into Ethertype 0x00FE, max packet size 1500.
 *  802.2 LLC - hook into LSAP 0x00FE, max packet size 1497.
 *  802.3 SNAP - hook into Ethertype 0x00FE, max packet size 1492.
 *  ETH_P_IP - filter IP packets with protocol number 80
 *  ETH_P_IP - filter UDP packets with port number 147
 *  ETH_P_HDLC - filter packets from X.25, FR or ATM devices with with NPLID 0x81 or SNAP headers
 *  (0x80 for these).  Unfortunately wanrouter does not do this for us.
 *  We need a some sort of X.121 proxy ARP entry for many of these.
 *
 *  These are all various levels of packet delivery mechanisms.  Hopefully we can munge all of these
 *  by overwritting skb->packet_host in cases of conflicts.
 *
 *  Note that ip at least checks for PACKET_OTHERHOST and drops otherwise, therefore, to override
 *  IP, just set skb->pkt_type = PACKET_OTHERHOST and ip will think that the interface is in
 *  promiscuous mode.  tcp discards packets that are not PACKET_HOST.  IP forward will also drop
 *  anything that is not PACKET_HOST.
 *
 *  We also could filter ICMP packets for our protocol or ports.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define n_tst_bit(nr,addr)	test_bit(nr,addr)
#define n_set_bit(nr,addr)	__set_bit(nr,addr)
#define n_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#endif				/* LINUX */

#include <sys/dlpi.h>
#include <sys/npi.h>
#include <sys/strlog.h>

#define CLNS_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CLNS_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define CLNS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define CLNS_REVISION	"OpenSS7 $RCSfile: clns.c,v $ $Name:  $ ($Revision: 0.9.2.7 $) $Date: 2006/08/26 09:19:05 $"
#define CLNS_DEVICE	"SVR 4.2 STREAMS CLNS OSI Network Provider"
#define CLNS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CLNS_LICENSE	"GPL"
#define CLNS_BANNER	CLNS_DESCRIP	"\n" \
			CLNS_EXTRA	"\n" \
			CLNS_COPYRIGHT	"\n" \
			CLNS_DEVICE	"\n" \
			CLNS_CONTACT
#define CLNS_SPLASH	CLNS_DESCRIP	"\n" \
			CLNS_REVISION

#ifdef LINUX
MODULE_AUTHOR(CLNS_CONTACT);
MODULE_DESCRIPTION(CLNS_DESCRIP);
MODULE_SUPPORTED_DEVICE(CLNS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CLNS_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-dl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define CLNS__DRV_ID	CONFIG_STREAMS_CLNS__MODID
#define CLNS__DRV_NAME	CONFIG_STREAMS_CLNS__NAME
#define CLNS__CMAJORS	CONFIG_STREAMS_CLNS__NMAJORS
#define CLNS__CMAJOR_0	CONFIG_STREAMS_CLNS__MAJOR
#define CLNS__UNITS	CONFIG_STREAMS_CLNS__NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CLNS__MODID));
MODULE_ALIAS("streams-driver-dl");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CLNS__MAJOR));
MODULE_ALIAS("/dev/streams/clnl");
MODULE_ALIAS("/dev/streams/clnl/*");
MODULE_ALIAS("/dev/streams/clnl/clnl");
MODULE_ALIAS("/dev/streams/clnl/clns");
MODULE_ALIAS("/dev/streams/clnl/esis");
MODULE_ALIAS("/dev/streams/clnl/isis");
MODULE_ALIAS("/dev/streams/clone/clnl");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(CLNS__CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(CLNS__CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(CLNS__CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(CLNS__CMAJOR_0) "-" __stringify(CLNS__CMINOR));
MODULE_ALIAS("/dev/clnl");
MODULE_ALIAS("/dev/clns");
MODULE_ALIAS("/dev/esis");
MODULE_ALIAS("/dev/isis");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		CLNS__DRV_ID
#define DRV_NAME	CLNS__DRV_NAME
#define CMAJORS		CLNS__CMAJORS
#define CMAJOR_0	CLNS__CMAJOR_0
#define UNITS		CLNS__UNITS
#ifdef MODULE
#define DRV_BANNER	CLNS_BANNER
#else				/* MODULE */
#define DRV_BANNER	CLNS_SPLASH
#endif				/* MODULE */

/* Upper multiplex is a N provider following the NPI. */

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,	/* Hi water mark */
	.mi_lowat = 1 << 10,	/* Lo water mark */
};

STATIC struct module_stat np_mstat = {
};

STATIC streamscall int np_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int np_qclose(queue_t *, int, cred_t *);

STATIC struct qinit np_rinit = {
	.qi_putp = &ss7_oput,	/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,	/* Read service procedure */
	.qi_qopen = &np_qopen,	/* Each open */
	.qi_qclose = &np_qclose,	/* Last close */
	.qi_minfo = &np_minfo,	/* Module information */
	.qi_mstat = &np_mstat,	/* Module statistics */
};

STATIC struct qinit np_winit = {
	.qi_putp = &ss7_iput,	/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,	/* Read service procedure */
	.qi_minfo = &np_minfo,	/* Module information */
	.qi_mstat = &np_mstat,	/* Module statistics */
};

/* Lower multiplex is a DL user following the DLPI. */

STATIC struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,	/* Hi water mark */
	.mi_lowat = 1 << 10,	/* Lo water mark */
};

STATIC struct module_stat dl_mstat = {
};

STATIC struct qinit dl_rinit = {
	.qi_putp = &ss7_iput,	/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,	/* Read service procedure */
	.qi_minfo = &dl_minfo,	/* Module information */
	.qi_mstat = &dl_mstat,	/* Module statistics */
};

STATIC struct qinit dl_winit = {
	.qi_putp = &ss7_oput,	/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,	/* Read service procedure */
	.qi_minfo = &dl_minfo,	/* Module information */
	.qi_mstat = &dl_mstat,	/* Module statistics */
};

STATIC struct streamtab np_info = {
	.st_rdinit = &np_rinit,	/* Upper read queue */
	.st_wrinit = &np_winit,	/* Upper write queue */
	.st_muxrinit = &dl_rinit,	/* Lower read queue */
	.st_muxwinit = &dl_winit,	/* Lower write queue */
};

/* Private structures */
typedef struct np {
	STR_DECLARATION (struct np);	/* Stream declaration */
	struct np *bnext;		/* linkage for bind/list hash */
	struct np **bprev;		/* linkage for bind/list hash */
#if 0
	struct np_bhash_bucket *bhash;	/* linkage for bind/list hash */
#endif
	struct np *cnext;		/* linkage for conn hash */
	struct np **cprev;		/* linkage for conn hash */
#if 0
	struct np_chash_bucket *chash;	/* linkage for conn hash */
#endif
	N_info_ack_t info;		/* service provider information */
	unsigned int BIND_flags;	/* bind flags */
	unsigned int CONN_flags;	/* connect flags */
	unsigned int CONIND_number;	/* maximum number of outstanding connection indications */
	unsigned int coninds;		/* number of outstanding connection indications */
	mblk_t *conq;			/* connection indication queue */
	unsigned int datinds;		/* number of outstanding data indications */
	mblk_t *datq;			/* data indication queue */
	unsigned int resinds;		/* number of outstanding reset indications */
	mblk_t *resq;			/* reset indication queue */
	unsigned short pnum;		/* number of bound protocol ids */
	uint8_t protoids[16];		/* bound protocol ids */
	unsigned short bnum;		/* number of bound addresses */
	unsigned short bport;		/* bound port number (network order) */
#if 0
	struct np_baddr baddrs[8];	/* bound addresses */
#endif
	unsigned short snum;		/* number of source (connected) addresses */
	unsigned short sport;		/* source (connected) port number (network order) */
#if 0
	struct np_saddr saddrs[8];	/* source (connected) addresses */
#endif
	unsigned short dnum;		/* number of destination (connected) addresses */
	unsigned short dport;		/* destination (connected) port number (network order) */
#if 0
	struct np_daddr daddrs[8];	/* destination (connected) addresses */
#endif
#if 0
	struct N_qos_sel_info_ip qos;	/* network service provider quality of service */
	struct N_qos_range_info_ip qor;	/* network service provider quality of service range */
#endif
} np_t;

#define PRIV(__q) (((__q)->q_ptr))
#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

STATIC kmem_cache_t *np_priv_cachep;

static INLINE struct np *
np_get(struct np *np)
{
	if (np)
		atomic_inc(&np->refcnt);
	return (np);
}
static INLINE void
np_put(struct np *np)
{
	if (np)
		if (atomic_dec_and_test(&np->refcnt)) {
			kmem_cache_free(np_priv_cachep, np);
		}
}
static INLINE void
np_release(struct np **npp)
{
	if (npp != NULL)
		np_put(XCHG(npp, NULL));
}
static INLINE struct np *
np_alloc(void)
{
	struct np *np;

	if ((np = kmem_cache_alloc(np_priv_cachep, SLAB_ATOMIC))) {
		bzero(np, sizeof(*np));
		atomic_set(&np->refcnt, 1);
		spin_lock_init(&np->lock);	/* "np-lock" */
		np->priv_put = &np_put;
		np->priv_get = &np_get;
		// np->type = 0;
		// np->id = 0;
		// np->state = 0;
		// np->flags = 0;
	}
	return (np);
}

typedef struct dl {
	STR_DECLARATION (struct dl);
	dl_info_ack_t info;
} dl_t;

#define PRIV(__q) (((__q)->q_ptr))
#define DL_PRIV(__q) ((struct dl *)((__q)->q_ptr))

STATIC kmem_cache_t *dl_priv_cachep;

static INLINE struct dl *
dl_get(struct dl *dl)
{
	if (dl)
		atomic_inc(&dl->refcnt);
	return (dl);
}
static INLINE void
dl_put(struct dl *dl)
{
	if (dl)
		if (atomic_dec_and_test(&dl->refcnt)) {
			kmem_cache_free(dl_priv_cachep, dl);
		}
}
static INLINE void
dl_release(struct dl **dlp)
{
	if (dlp != NULL)
		dl_put(XCHG(dlp, NULL));
}
static INLINE struct dl *
dl_alloc(void)
{
	struct dl *dl;

	if ((dl = kmem_cache_alloc(dl_priv_cachep, SLAB_ATOMIC))) {
		bzero(dl, sizeof(*dl));
		atomic_set(&dl->refcnt, 1);
		spin_lock_init(&dl->lock);	/* "dl-lock" */
		dl->priv_put = &dl_put;
		dl->priv_get = &dl_get;
		// dl->type = 0;
		// dl->id = 0;
		// dl->state = 0;
		// dl->flags = 0;
	}
	return (dl);
}

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (np, np);		/* master list of np (open) structures */
	SLIST_HEAD (dl, dl);		/* master list of dl (link) structures */
} df_t;

STATIC struct df master = {.lock = RW_LOCK_UNLOCKED, };

/*
 *  Logging.
 */
#ifndef LOG_EMERG

#define LOG_EMERG	0
#define LOG_ALERT	1
#define LOG_CRIT	2
#define LOG_ERR		3
#define LOG_WARNING	4
#define LOG_NOTICE	5
#define LOG_INFO	6
#define LOG_DEBUG	7

#define LOG_KERN    (0<<3)
#define LOG_USER    (1<<3)

#endif				/* LOG_EMERG */

/*
 *  State changes.
 */

/* State flags */
#define NSF_UNBND	(1 << NS_UNBND		)
#define NSF_WACK_BREQ	(1 << NS_WACK_BREQ	)
#define NSF_WACK_UREQ	(1 << NS_WACK_UREQ	)
#define NSF_IDLE	(1 << NS_IDLE		)
#ifdef NS_WACK_OPTREQ
#define NSF_WACK_OPTREQ	(1 << NS_WACK_OPTREQ	)
#endif
#define NSF_WACK_RRES	(1 << NS_WACK_RRES	)
#define NSF_WCON_CREQ	(1 << NS_WCON_CREQ	)
#define NSF_WRES_CIND	(1 << NS_WRES_CIND	)
#define NSF_WACK_CRES	(1 << NS_WACK_CRES	)
#define NSF_DATA_XFER	(1 << NS_DATA_XFER	)
#define NSF_WCON_RREQ	(1 << NS_WCON_RREQ	)
#define NSF_WRES_RIND	(1 << NS_WRES_RIND	)
#define NSF_WACK_DREQ6	(1 << NS_WACK_DREQ6	)
#define NSF_WACK_DREQ7	(1 << NS_WACK_DREQ7	)
#define NSF_WACK_DREQ9	(1 << NS_WACK_DREQ9	)
#define NSF_WACK_DREQ10	(1 << NS_WACK_DREQ10	)
#define NSF_WACK_DREQ11	(1 << NS_WACK_DREQ11	)
#define NSF_NOSTATES	(1 << NS_NOSTATES	)

/* State masks */
#define NSM_ALLSTATES	(NSF_NOSTATES - 1)
#define NSM_WACK_DREQ	(NSF_WACK_DREQ6 \
			|NSF_WACK_DREQ7 \
			|NSF_WACK_DREQ9 \
			|NSF_WACK_DREQ10 \
			|NSF_WACK_DREQ11)
#define NSM_LISTEN	(NSF_IDLE \
			|NSF_WRES_CIND)
#define NSM_CONNECTED	(NSF_WCON_CREQ\
			|NSF_WRES_CIND\
			|NSF_DATA_XFER\
			|NSF_WCON_RREQ\
			|NSF_WRES_RIND)
#define NSM_DISCONN	(NSF_IDLE\
			|NSF_UNBND)
#define NSM_INDATA	(NSF_DATA_XFER\
			|NSF_WCON_RREQ)
#define NSM_OUTDATA	(NSF_DATA_XFER\
			|NSF_WRES_RIND)

#ifndef N_PROVIDER
#define N_PROVIDER  0
#define N_USER	    1
#endif

#ifdef _DEBUG
STATIC const char *
np_state_name(np_ulong state)
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
		return ("(unknown)");
	}
}
#endif				/* _DEBUG */

STATIC INLINE streams_fastcall __unlikely void
np_set_state(struct np *np, const np_ulong state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, np, np_state_name(state),
		np_state_name(np->info.CURRENT_state)));
	np->info.CURRENT_state = state;
}

STATIC INLINE streams_fastcall __unlikely np_ulong
np_get_state(const struct np *np)
{
	return (np->info.CURRENT_state);
}

STATIC INLINE streams_fastcall __unlikely np_ulong
np_chk_state(const struct np *np, const np_ulong mask)
{
	return (((1 << np->info.CURRENT_state) & (mask)) != 0);
}

STATIC INLINE streams_fastcall __unlikely np_ulong
np_not_state(const struct np *np, const np_ulong mask)
{
	return (((1 << np->info.CURRENT_state) & (mask)) == 0);
}

STATIC INLINE streams_fastcall __unlikely np_ulong
np_get_statef(const struct np *np)
{
	return (1 << np_get_state(np));
}

#ifdef _DEBUG
STATIC const char *
dl_state_name(dl_ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("(unknown)");
	}
}
#endif				/* _DEBUG */

STATIC INLINE streams_fastcall __unlikely void
dl_set_state(struct dl *dl, const dl_ulong state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, dl, dl_state_name(state),
		dl_state_name(dl->info.dl_current_state)));
	dl->info.dl_current_state = state;
}

STATIC INLINE streams_fastcall __unlikely dl_ulong
dl_get_state(const struct dl *dl)
{
	return (dl->info.dl_current_state);
}

STATIC INLINE streams_fastcall __unlikely dl_ulong
dl_chk_state(const struct dl *dl, const dl_ulong mask)
{
	return (((1 << dl->info.dl_current_state) & (mask)) != 0);
}

STATIC INLINE streams_fastcall __unlikely dl_ulong
dl_not_state(const struct dl *dl, const dl_ulong mask)
{
	return (((1 << dl->info.dl_current_state) & (mask)) == 0);
}

STATIC INLINE streams_fastcall __unlikely dl_ulong
dl_get_statef(const struct dl *dl)
{
	return (1 << dl_get_state(dl));
}

/*
 *  Service Primitives passed downwards -- lower multiplex or module
 *  ----------------------------------------------------------------
 */

#if 0
STATIC int
dl_info_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_attach_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_detach_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_bind_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_unbind_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_subs_bind_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_subs_unbind_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_enabmulti_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_disabmulti_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_promiscon_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_promiscoff_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_unitdata_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_udqos_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_connect_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_connect_res(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_token_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_disconnect_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_reset_req(queue_t *q, queue_t *oq)
{
}
STATIC int
dl_reset_res(queue_t *q, queue_t *oq)
{
}
#endif				/* 0 */

/*
 *  Service Primitives passed upwards -- upper multiplex or module
 *  --------------------------------------------------------------
 */

#if 0
STATIC int
n_conn_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_conn_con(queue_t *q, queue_t *oq)
{
}
STATIC int
n_discon_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_data_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_exdata_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_info_ack(queue_t *q, queue_t *oq)
{
}
STATIC int
n_bind_ack(queue_t *q, queue_t *oq)
{
}
#endif				/* 0 */
STATIC int
n_error_ack(queue_t *q, np_long prim, np_long rtn)
{
	return (-EFAULT);
}
STATIC int
n_error_reply(queue_t *q, np_long rtn)
{
	return (-EFAULT);
}

#if 0
STATIC int
n_ok_ack(queue_t *q, queue_t *oq)
{
}
STATIC int
n_unitdata_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_uderror_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_datack_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_reset_ind(queue_t *q, queue_t *oq)
{
}
STATIC int
n_reset_con(queue_t *q, queue_t *oq)
{
}
#endif				/* 0 */

/*
 *  Service Primitives from above -- lower multiplex
 *  ------------------------------------------------
 */

#if 0
STATIC int
dl_w_rse(queue_t *q, mblk_t *mp)
{
}
#endif				/* 0 */

/*
 *  Service Primitives from above -- upper multiplex
 *  ------------------------------------------------
 */
STATIC int
n_conn_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_conn_res(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_discon_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_data_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_exdata_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_info_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_bind_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_unbind_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_optmgmt_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_datack_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_reset_req(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
n_reset_res(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}

STATIC INLINE fastcall __hot_put int
np_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct np *np = NP_PRIV(q);
	np_long oldstate = np_get_state(np);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(np_long *) mp->b_rptr)) {
		case N_CONN_REQ:	/* NC request */
			rtn = n_conn_req(q, mp);
			break;
		case N_CONN_RES:	/* Accept previous connection indication */
			rtn = n_conn_res(q, mp);
			break;
		case N_DISCON_REQ:	/* NC disconnection request */
			rtn = n_discon_req(q, mp);
			break;
		case N_DATA_REQ:	/* Connection-Mode data transfer request */
			rtn = n_data_req(q, mp);
			break;
		case N_EXDATA_REQ:	/* Expedited data request */
			rtn = n_exdata_req(q, mp);
			break;
		case N_INFO_REQ:	/* Information Request */
			rtn = n_info_req(q, mp);
			break;
		case N_BIND_REQ:	/* Bind a NS user to network address */
			rtn = n_bind_req(q, mp);
			break;
		case N_UNBIND_REQ:	/* Unbind NS user from network address */
			rtn = n_unbind_req(q, mp);
			break;
		case N_UNITDATA_REQ:	/* Connection-less data send request */
			rtn = n_unitdata_req(q, mp);
			break;
		case N_OPTMGMT_REQ:	/* Options Management request */
			rtn = n_optmgmt_req(q, mp);
			break;
		case N_DATACK_REQ:	/* Data acknowledgement request */
			rtn = n_datack_req(q, mp);
			break;
		case N_RESET_REQ:	/* NC reset request */
			rtn = n_reset_req(q, mp);
			break;
		case N_RESET_RES:	/* Reset processing accepted */
			rtn = n_reset_res(q, mp);
			break;

		case N_CONN_IND:	/* Incoming connection indication */
		case N_CONN_CON:	/* Connection established */
		case N_DISCON_IND:	/* NC disconnected */
		case N_DATA_IND:	/* Incoming connection-mode data indication */
		case N_EXDATA_IND:	/* Incoming expedited data indication */
		case N_INFO_ACK:	/* Information Acknowledgement */
		case N_BIND_ACK:	/* NS User bound to network address */
		case N_ERROR_ACK:	/* Error Acknowledgement */
		case N_OK_ACK:	/* Success Acknowledgement */
		case N_UNITDATA_IND:	/* Connection-less data receive indication */
		case N_UDERROR_IND:	/* UNITDATA Error Indication */
		case N_DATACK_IND:	/* Data acknowledgement indication */
		case N_RESET_IND:	/* Incoming NC reset request indication */
		case N_RESET_CON:	/* Reset processing complete */
			/* wrong direction */
			rtn = -EPROTO;
			break;
		default:
			/* unrecognized primitive */
			rtn = -EOPNOTSUPP;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		np_set_state(np, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EOPNOTSUPP:	/* primitive not supported */
			return n_error_ack(q, prim, rtn);
		case -EPROTO:
			return n_error_reply(q, rtn);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC int
np_w_data(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
np_w_ctl(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
np_w_ioctl(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}

/*
 *  Service Primitives from below -- upper multiplex
 *  ------------------------------------------------
 *  Messages arriving at the upper multiplex read queue are either passed directly by a DL stream
 *  upon which NP is pushed as a module, or are passed across the multiplex from a lower DL stream.
 *  These are DLPI primitives and other lower stream related messages.
 */

STATIC int
np_r_rse(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}

/*
 *  Service primitives from below -- lower multiplex
 *  ------------------------------------------------
 */
STATIC int
dl_info_ack(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_ok_ack(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_error_ack(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_subs_bind_ack(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_unitdata_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_uderror_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_connect_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_connect_con(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_token_ack(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_disconnect_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_reset_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_reset_con(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_data_ack_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_data_ack_status_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_reply_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_reply_status_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_reply_update_status_ind(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}

STATIC INLINE fastcall __hot_read int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	dl_long prim = 0;
	struct dl *dl = DL_PRIV(q);
	dl_long oldstate = dl_get_state(dl);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(dl_long *) mp->b_rptr)) {
		case DL_INFO_ACK:	/* Information Ack */
			rtn = dl_info_ack(q, mp);
			break;
		case DL_OK_ACK:	/* Success acknowledgment */
			rtn = dl_ok_ack(q, mp);
			break;
		case DL_ERROR_ACK:	/* Error acknowledgment */
			rtn = dl_error_ack(q, mp);
			break;
		case DL_SUBS_BIND_ACK:	/* Subsequent DLSAP address bound */
			rtn = dl_subs_bind_ack(q, mp);
			break;
		case DL_UNITDATA_IND:	/* datagram receive indication */
			rtn = dl_unitdata_ind(q, mp);
			break;
		case DL_UDERROR_IND:	/* datagram error indication */
			rtn = dl_uderror_ind(q, mp);
			break;
		case DL_CONNECT_IND:	/* Incoming connect indication */
			rtn = dl_connect_ind(q, mp);
			break;
		case DL_CONNECT_CON:	/* Connection established */
			rtn = dl_connect_con(q, mp);
			break;
		case DL_TOKEN_ACK:	/* Passoff token ack */
			rtn = dl_token_ack(q, mp);
			break;
		case DL_DISCONNECT_IND:	/* Disconnect indication */
			rtn = dl_disconnect_ind(q, mp);
			break;
		case DL_RESET_IND:	/* Incoming reset indication */
			rtn = dl_reset_ind(q, mp);
			break;
		case DL_RESET_CON:	/* Reset processing complete */
			rtn = dl_reset_con(q, mp);
			break;
		case DL_DATA_ACK_IND:	/* Arrival of a command PDU */
			rtn = dl_data_ack_ind(q, mp);
			break;
		case DL_DATA_ACK_STATUS_IND:	/* Status indication of DATA_ACK_REQ */
			rtn = dl_data_ack_status_ind(q, mp);
			break;
		case DL_REPLY_IND:	/* Arrival of a command PDU */
			rtn = dl_reply_ind(q, mp);
			break;
		case DL_REPLY_STATUS_IND:	/* Status indication of REPLY_REQ */
			rtn = dl_reply_status_ind(q, mp);
			break;
		case DL_REPLY_UPDATE_STATUS_IND:	/* Status of REPLY_UPDATE req */
			rtn = dl_reply_update_status_ind(q, mp);
			break;

		case DL_INFO_REQ:	/* Information Req */
		case DL_ATTACH_REQ:	/* Attach a PPA */
		case DL_DETACH_REQ:	/* Detach a PPA */
		case DL_BIND_REQ:	/* Bind dlsap address */
		case DL_BIND_ACK:	/* Dlsap address bound */
		case DL_UNBIND_REQ:	/* Unbind dlsap address */
		case DL_SUBS_BIND_REQ:	/* Bind Subsequent DLSAP address */
		case DL_SUBS_UNBIND_REQ:	/* Subsequent unbind */
		case DL_ENABMULTI_REQ:	/* Enable multicast addresses */
		case DL_DISABMULTI_REQ:	/* Disable multicast addresses */
		case DL_PROMISCON_REQ:	/* Turn on promiscuous mode */
		case DL_PROMISCOFF_REQ:	/* Turn off promiscuous mode */
		case DL_UNITDATA_REQ:	/* datagram send request */
		case DL_UDQOS_REQ:	/* set QOS for subsequent datagrams */
		case DL_CONNECT_REQ:	/* Connect request */
		case DL_CONNECT_RES:	/* Accept previous connect indication */
		case DL_TOKEN_REQ:	/* Passoff token request */
		case DL_DISCONNECT_REQ:	/* Disconnect request */
		case DL_RESET_REQ:	/* Reset service request */
		case DL_RESET_RES:	/* Complete reset processing */
		case DL_DATA_ACK_REQ:	/* data unit transmission request */
		case DL_REPLY_REQ:	/* Request a DLSDU from the remote */
		case DL_REPLY_UPDATE_REQ:	/* Hold a DLSDU for transmission */
			/* wrong direction */
			rtn = -EPROTO;
			break;
		default:
			/* unrecognized primitive */
			rtn = -EOPNOTSUPP;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		dl_set_state(dl, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
			return (rtn);
		case -EOPNOTSUPP:	/* primitive not supported */
			/* discard it */
			rtn = QR_DONE;
			break;
		case -EPROTO:
			/* FIXME: mark stream bad */
			rtn = QR_DONE;
			break;
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

STATIC INLINE fastcall __hot_read int
dl_r_data(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_r_ctl(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_r_ioctl(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_r_hangup(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}
STATIC int
dl_r_error(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}

/*
 *  Put and Service procedures
 *  ==========================
 */
/**
 * np_w_prim - process messages on the upper multiplex write queue
 * @q: upper multiplex write queue
 * @mp: message to process
 */
STATIC streamscall int
np_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(q, mp);
	case M_DATA:
		return np_w_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		/* intermodule IOCTLs */
		return np_w_ctl(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return np_w_ioctl(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	swerr();
	return (QR_PASSFLOW);
}

/**
 * np_r_prim - process messages on the upper multiplex read queue
 * @q: upper multiplex read queue
 * @mp: message to process
 *
 * Message only arrive at the upper multiplex when passed from the other queue in the pair or when
 * passed across the multiplexing driver from the lower multiplex.  Normally, message that are
 * destined upstream are passed to the queue above the upper multiplex.  There is only one situation
 * in which messages are placed on the upper read queue: lock contention on the upper multiplex
 * private structure (from the lower).  Therefore, all messages can do a simple passflow except any
 * special messages that we want to interpret internally across the multiplex.  These are always
 * M_RSE or M_PCRSE messages.
 *
 * Note that if we are pushed as a module, dl_r_prim() is used for the read queue instead of this
 * function.  This function is only used for the read queue of an upper multiplex.
 *
 */
STATIC streamscall int
np_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		/* intra multiplexer messages */
		return np_r_rse(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

STATIC streamscall void
np_r_wakeup(queue_t *q)
{
	qenable(q);
}

#if 0
/**
 * dl_w_prim - process messages for the lower multiplex write queue
 * @q: lower multiplex write queue
 * @mp: message to process
 *
 * Messages only arrive at the lower multiplex when passed from the other queue in the pair or when
 * passed across the multiplexing driver from the upper multiplex.  Normally, messages that are
 * destined downstream are passed the queue beneath the lower multiplex.  There is only one
 * situation in which messages are placed on the lower write queue: lock contention on the lower
 * multiplex private structure (from the upper).  Therefore, all messages can do a simple passflow
 * except any special messages that we want to interpret internally across the multiplex.  These are
 * always M_RSE or M_PCRSE messages.
 *
 * Note that if we are pushed as a module, np_w_prim() is used for the write queue instead of this
 * function.  This function is only used for the write queue of a lower multiplex.
 *
 */
STATIC int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		/* intra multiplexer messages */
		return dl_w_rse(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
#endif				/* 0 */

/**
 * dl_r_prim - process messages on the lower multiplex read queue or the module read queue
 * @q: upper multiplex or module read queue
 * @mp: message to process
 *
 * In general the procedures can detect whether this is a lower multiplex read queue or a module
 * read queue by checking q->q_next.  If it is NULL, this is a lower multiplex read queue; if
 * non-NULL, a module.  Typically, the procedures can check this when considering which upper
 * multiplex stream to which to deliver the message.  When a module, the result is trivial
 * (q->q_next); when a multiplexing driver, some algorithm that considers all upper multiplex
 * streams.
 */
STATIC streamscall int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_DATA:
		return dl_r_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		/* intermodule IOCTLs */
		return dl_r_ctl(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_ioctl(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	swerr();
	return (QR_PASSFLOW);
}

/*
 *  Private structure allocation, deallocate, cache
 *  ===============================================
 */
STATIC int
np_term_caches(void)
{
	if (np_priv_cachep != NULL) {
		if (kmem_cache_destroy(np_priv_cachep)) {
			strlog(DRV_ID, 0, LOG_ERR, SL_ERROR | SL_CONSOLE,
			       "could not destroy np_priv_cachep");
			return (-EBUSY);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "destroyed np_priv_cachep");
	}
	return (0);
}
STATIC int
np_init_caches(void)
{
	if (np_priv_cachep == NULL) {
		np_priv_cachep = kmem_cache_create("np_priv_cachep", sizeof(struct np), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_priv_cachep == NULL) {
			strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
			       "cannot allocate np_priv_cachep");
			np_term_caches();
			return (-ENOMEM);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE,
		       "initialized driver private structure cache");
	}
	return (0);
}

STATIC struct np *
np_alloc_priv(queue_t *q, struct np **npp, int sflag, dev_t *devp, cred_t *crp)
{
	struct np *np;

	if ((np = np_alloc())) {
		/* np generic members */
		np->u.dev.cmajor = getmajor(*devp);
		np->u.dev.cminor = getminor(*devp);
		np->cred = *crp;
		(np->oq = q)->q_ptr = np_get(np);
		(np->iq = WR(q))->q_ptr = np_get(np);
		if (sflag != MODOPEN) {
			np->i_prim = &np_w_prim;
			np->o_prim = &np_r_prim;
			np->i_wakeup = NULL;
			np->o_wakeup = &np_r_wakeup;
		} else {
			np->i_prim = &np_w_prim;
			np->o_prim = &dl_r_prim;
			np->i_wakeup = NULL;
			np->o_wakeup = NULL;
		}
		spin_lock_init(&np->qlock);
		np->i_state = NS_UNBND;
		np->i_style = 0;
		np->i_version = N_CURRENT_VERSION;
		np->i_oldstate = NS_UNBND;
		/* link into master list */
		if ((np->next = *npp))
			np->next->prev = &np->next;
		np->prev = npp;
		*npp = np_get(np);
	} else
		strlog(DRV_ID, getminor(*devp), LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not allocate driver private structure");
	return (np);
}
STATIC void
np_free_priv(queue_t *q)
{
	struct np *np = NP_PRIV(q);

	/* FIXME: Here we need to handle the disposition of the state of the Stream.  If the state
	   is not unattached, we need to take it there. */

	qprocsoff(q);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&np->refcnt));
	ss7_unbufcall((str_t *) np);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "removed bufcalls: reference count = %d", atomic_read(&np->refcnt));
	/* remove from master list */
	write_lock_bh(&master.lock);
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	write_unlock_bh(&master.lock);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinked: reference count = %d", atomic_read(&np->refcnt));
	np_release((struct np **) &np->oq->q_ptr);
	np->oq = NULL;
	np_release((struct np **) &np->iq->q_ptr);
	np->iq = NULL;
	assure(atomic_read(&np->refcnt) == 1);
	np_release(&np);
	return;
}

/*
 *  STREAMS Open and Close
 *  ======================
 */
STATIC int np_majors[CMAJORS] = { CMAJOR_0, };

STATIC streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	major_t cminor = getminor(*devp);
	struct np *np, **npp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE, "cannot push as module");
		return (ENXIO);
	}
#ifdef LIS
	if (cmajor != CMAJOR_0) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "major device number mismatch %d != %d", cmajor, CMAJOR_0);
		return (ENXIO);
	}
#endif				/* LIS */
#ifdef LFS
	/* Linux Fast-STREAMS always passes internal major dvice numbers (module ids) */
	if (cmajor != DRV_ID) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "major device number mismatch %d != %d", cmajor, DRV_ID);
		return (ENXIO);
	}
#endif				/* LFS */
	if (sflag == CLONEOPEN || cminor < 1) {
		strlog(DRV_ID, cminor, LOG_DEBUG, SL_TRACE, "clone open in effect");
		sflag = CLONEOPEN;
		cminor = 1;
	}
	write_lock_bh(&master.lock);
	for (npp = &master.np.list; *npp; npp = &(*npp)->next) {
		if (cmajor != (*npp)->u.dev.cmajor)
			break;
		if (cmajor == (*npp)->u.dev.cmajor) {
			if (cminor < (*npp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = np_majors[mindex]))
						break;
					cminor = 0;	/* start next major */
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE, "no device numbers available");
		write_unlock_bh(&master.lock);
		return (ENXIO);
	}
	cmn_err(CE_NOTE, "%s: opened character device %d:%d", DRV_NAME, cmajor, cminor);
	*devp = makedevice(cmajor, cminor);
	if (!(np = np_alloc_priv(q, npp, sflag, devp, crp))) {
		write_unlock_bh(&master.lock);
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
	qprocson(q);
	return (0);
}

STATIC streamscall int
np_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct np *np = NP_PRIV(q);

	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE, "closing character device");
#ifdef LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		strlog(DRV_ID, np->u.dev.cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS double-close bug detected");
		goto quit;
	}
#if 0
	/* only for modules pushed on pipe ends */
	if (q->q_next == NULL) {
		strlog(DRV_ID, np->u.dev.cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS pipe bug: called with NULL q->q_next pointer");
		goto skip_pop;
	}
#endif
#endif				/* LIS */
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	np_free_priv(q);
	goto quit;
      quit:
	return (0);
}

/*
 *  Registration and Deregistration
 *  ===============================
 */

#ifdef LINUX
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID number for CLNS driver (0-for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for CLNS driver (0 for allocation).");

#endif				/* LINUX */

#ifdef LFS
STATIC struct cdevsw np_cdev = {
	.d_name = DRV_NAME,
	.d_str = &np_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

int __init
np_init(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = np_init_caches()))
		return (err);
	if ((err = register_strdev(&np_cdev, major)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not register major %d", major);
		np_term_caches();
		return (err);
	}
	if (err > 0)
		major = err;
	return (0);
}

void __exit
np_exit(void)
{
	unregister_strdev(&np_cdev, major);
	np_term_caches();
}

#ifdef MODULE
module_init(np_init);
module_exit(np_exit);
#endif				/* MODULE */

#endif				/* LFS */

#ifdef LIS
STATIC int np_initalized = 0;
STATIC void
np_init(void)
{
	int err;

	if (np_initialized != 0)
		return;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = np_init_caches())) {
		np_initialized = err;
		return;
	}
	if ((err = lis_register_strdev(major, &np_info, UNITS, DRV_NAME)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		np_initialized = err;
		np_term_caches();
		return;
	}
	np_initialized = 1;
	if (major = 0 && err > 0) {
		major = err;
		np_initialized = 2;
	}
	return;
}
STATIC void
np_terminate(void)
{
	int err;

	if (np_initialized <= 0)
		return;
	if (major) {
		if ((err = lis_unregister_strdev(major)) < 0)
			strlog(DRV_ID, 0, LOG_CRIT, SL_FATAL | SL_CONSOLE,
			       "cannot unregister major %d", major);
		major = 0;
	}
	np_term_caches();
	np_initialized = 0;
	return;
}

#ifdef MODULE
int
init_module(void)
{
	np_init();
	if (np_initialized < 0)
		return np_initialized;
	return (0);
}

void
cleanup_module(void)
{
	return np_terminate();
}
#endif				/* MODULE */

#endif				/* LIS */
