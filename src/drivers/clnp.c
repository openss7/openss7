/*****************************************************************************

 @(#) File: src/drivers/clns.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2020  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "src/drivers/clns.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This is an X.233 CLNS driver.
 *
 *  The driver implements X.233 (CLNP) as a multiplexing pseudo-device driver.  The driver presents
 *  an NPI CLNS service interface at the upper multiplex.
 *
 *  Data link streasm providing the CODLS or CLDLS data link services using the DLPI interface are
 *  linked beneath the multiplexing driver.  Data link Streams should be attached and bound (CLDLS)
 *  or attached, bound and connected (CODLS), before being linked.  Data link Streams can be Streams
 *  of any type.  In support of various methods for transporting CLNP, the can be one of the
 *  following types:
 *
 *  DL_ETHER:	    Ethertype 0x00FE, MTU 1500.
 *  DL_CSMACD:	    LLC1 hooked into LSAP 0x00FE, MTU 1497 or MTU 1492 (w/SNAP).
 *  DL_ETH_CSMA:    Capable of both DL_ETHER and DL_CSMACD.
 *  DL_X25:	    NPLID 0x81
 *  DL_MPFRAME:	    MP over FR,   NPLID 0x81 or SNAP 0x80.
 *
 *  Note that RFC 1070 ISO-IP is implemented using separate device drivers.
 *
 *  For handling CONS over (ISO-IP RFC 1070), NPI provider Streams are linked under the multiplexing
 *  driver.  These are CLNS Streams from the np-ip driver or np-udp driver from the strxns package.
 *
 *  NP-IP:	    IP packets with protocol number 80. (RFC 1070)
 *  NP-UDP:	    UDP packets for port number 147. (RFC 1070)
 *
 *  CLNP packets are self identifying as CLNS, ES-IS, or IS-IS (IDRP) packets.  When configured for
 *  ES operation, the driver initiates and responds to ES-IS packets on data links that have been
 *  associated with the end system.  When configured for IS operation, the driver initiates and
 *  responds to IS-IS packets on data links that have been associated with the intermediate system.
 *  ES, IS and their associated addresses, network titles and data links are configured using
 *  input-output controls.
 *
 *  Routing is performed in accordance with ES or IS procedures.  Additional addressing schemes,
 *  routing procedures exist for IP and UDP encapsulation of CLNP packets.  For these forms the
 *  SNPA address (IP address) is provided by the NPI service interface.  Routing is performed
 *  directly to the underlying NP-IP or NP-UDP provider Streams.  In fact, this will be implemented
 *  as a separate driver.
 *
 *  This is an X.233 CLNS driver.  This is an NPI driver that can be pushed over or link a DLPI
 *  Stream.  DLPI Streams can be DL_ETH, DL_CMSACD or DL_CMSA or DL_IP and this driver will use
 *  various methods for transporting CLNS.
 *
 *  Actually, this is a CLNL driver.  It does not have to link DL Streams underneath unless there is
 *  only a DL Stream for the link driver.  This hooks into the Linux device independent packet layer
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

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#include <linux/bitops.h>

#define n_tst_bit(nr,addr)	test_bit(nr,addr)
#define n_set_bit(nr,addr)	__set_bit(nr,addr)
#define n_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/net.h>
#include <linux/in.h>
#include <linux/un.h>
#include <linux/ip.h>

#undef ASSERT

#include <net/sock.h>
#include <net/ip.h>
#include <net/udp.h>

#include <linux/errqueue.h>

#include <sys/dlpi.h>
#include <sys/npi.h>
#include <sys/npi_osi.h>
#include <sys/nlm_ioctl.h>

#include <sys/strlog.h>

#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGIO	6	/* log Stream additional data */
#define STRLOGDA	7	/* log Stream data */

#define CLNS_DESCRIP	"OSI CLNS Network Provider STREAMS Driver"
#define CLNS_EXTRA	"Part of the OpenSS7 OSI Stack for Linux Fast-STREAMS"
#define CLNS_COPYRIGHT	"Copyright (c) 2008-2020  Monavacon Limited.  All Rights Reserved."
#define CLNS_REVISION	"OpenSS7 src/drivers/clns.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define CLNS_DEVICE	"SVR 4.2 MP STREAMS CLNS OSI Network Provider"
#define CLNS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CLNS_LICENSE	"GPL"
#define CLNS_BANNER	CLNS_DESCRIP	"\n" \
			CLNS_EXTRA	"\n" \
			CLNS_REVISION	"\n" \
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
MODULE_ALIAS("streams-clns");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif				/* LINUX */

#define CLNS_DRV_ID	CONFIG_STREAMS_CLNS_MODID
#define CLNS_DRV_NAME	CONFIG_STREAMS_CLNS_NAME
#define CLNS_CMAJORS	CONFIG_STREAMS_CLNS_NMAJORS
#define CLNS_CMAJOR_0	CONFIG_STREAMS_CLNS_MAJOR
#define CLNS_UNITS	CONFIG_STREAMS_CLNS_NMINORS

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CLNS_MODID));
MODULE_ALIAS("streams-driver-clns");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CLNS_MAJOR));
MODULE_ALIAS("/dev/streams/clnl");
MODULE_ALIAS("/dev/streams/clnl/*");
MODULE_ALIAS("/dev/streams/clnl/clnl");
MODULE_ALIAS("/dev/streams/clnl/clns");
MODULE_ALIAS("/dev/streams/clnl/esis");
MODULE_ALIAS("/dev/streams/clnl/isis");
MODULE_ALIAS("/dev/streams/clone/clnl");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(CLNS_CMAJOR_0));
MODULE_ALIAS("/dev/clnl");
//MODULE_ALIAS("devname:clnl");
MODULE_ALIAS("/dev/clns");
MODULE_ALIAS("/dev/esis");
MODULE_ALIAS("/dev/isis");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define DRV_ID		CLNS_DRV_ID
#define DRV_NAME	CLNS_DRV_NAME
#define CMAJORS		CLNS_CMAJORS
#define CMAJOR_0	CLNS_CMAJOR_0
#define UNITS		CLNS_UNITS
#ifdef MODULE
#define DRV_BANNER	CLNS_BANNER
#else				/* MODULE */
#define DRV_BANNER	CLNS_SPLASH
#endif				/* MODULE */

/*
 * forward declarations
 */
struct df;				/* Default structure */
struct np;				/* NLS User - upper multiplex stream */
struct ne;				/* Network Entity */
struct cn;				/* connection */
struct xp;				/* X25 PLE Profile (Initial Values) */
struct vc;				/* Virtual Circuit */
struct vp;				/* Virtual Circuit Profile (Initial Values) */
struct dl;				/* DLS User - lower multiplex stream */

struct aj;				/* ISIS Adjacency */
struct va;				/* ISIS Virtual Adjacency */
struct ds;				/* ISIS Destination System */
struct da;				/* ISIS Destination Area */
struct ra;				/* ISIS Reachable Address */

static struct df master = {
#if	defined __RW_LOCK_UNLOCKED
	.lock = __RW_LOCK_UNLOCKED(master.lock),
#elif	defined RW_LOCK_UNLOCKED
	.lock = RW_LOCK_UNLOCKED,
#else
#error cannot initialize read-write locks
#endif
};

static struct df *
df_lookup(uint id)
{
	if (id)
		return (NULL);
	return (&master);
}

/*
 *  NP - NLS User
 *  ----------------------------------------
 *  The NP structure corresponds to an NLS User Stream.  This Stream is bound to a source address,
 *  and possibly a destination address if the NLS user is connection-oriented.  The source address
 *  defines a local network entity and the destination address defines a remote network entity.
 */
struct np {
	struct np *next;
	queue_t *rq;			/* RD queue */
	union {
		dev_t dev;		/* device number */
		uint id;
	};
	cred_t cred;			/* credentials of creator */
	struct {
		uint flags;		/* interface flags */
		uint type;		/* interface type */
		uint state;		/* interface state */
		uint style;		/* interface style */
		uint version;		/* interface version */
	};
	struct nsap_addr src;		/* srce address */
	struct nsap_addr dst;		/* dest address */
	struct {
		struct ne *loc;		/* local network entity */
		struct ne *rem;		/* remote network entity */
		struct np *next;	/* next NLSU for local network entity */
		struct np **prev;	/* prev NLSU for local network entity */
	} ne;
	struct np *bnext;		/* linkage for bind/list hash */
	struct np **bprev;		/* linkage for bind/list hash */
#if 0
	struct np_bhash_bucket *bhash;	/* linkage for bind/list hash */
#endif
	struct np *cnext;		/* linkage for connection hash */
	struct np **cprev;		/* linkage for connection hash */
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
};

#define NP_PRIV(__q) ((struct np *)(__q)->q_ptr)

static struct np *np_alloc_priv(queue_t *, dev_t *, cred_t *, minor_t);
static void np_free_priv(struct np *);
static struct np *np_lookup(uint);
static uint np_get_id(uint);
static void np_attach(struct np *, struct ne *, uchar);
static void np_detach(struct np *);

/*
 *  NE - Network Entity
 *  ----------------------------------------
 *  The network entity structure represents a local network entity.
 */
struct ne {
	HEAD_DECLARATION (struct ne);
	int users;
	queue_t *waitq;
	struct nsap_addr net;		/* network entity title */
	struct {
		spinlock_t lock;
		int users;
	} sq;

	SLIST_HEAD (np, np);		/* NLS user list */
	SLIST_HEAD (dl, dl);		/* datalink list */
	struct nlm_timers_ne timers;	/* network entity timers */
	struct nlm_opt_conf_ne config;	/* network entity configuration */
	struct nlm_status_ne status;	/* network entity status */
	struct nlm_stats_ne statsp;	/* network entity statistics periods */
	struct nlm_stats_ne stats;	/* network entity statistics */
	struct nlm_notify_ne notify;	/* network entity notifications */
};

static struct ne *np_alloc_ne(uint, struct nlm_conf_ne *);
static void np_free_ne(struct ne *);
static struct ne *ne_lookup(uint);
static uint ne_get_id(uint);

/*
 *  CN - Connection
 *  ----------------------------------------
 */
struct cn {
	struct nlm_timers_cn timers;
	struct nlm_opt_conf_cn config;
	struct nlm_stats_cn statsp;
	struct nlm_stats_cn stats;
	struct nlm_notify_cn notify;
};					/* connection */

static struct cn *np_alloc_cn(uint, struct nlm_conf_cn *);
static void np_free_cn(struct cn *);
static struct cn *cn_lookup(uint);
static uint cn_get_id(uint);

/*
 *  XP - X25 Packet Layer Entity Initial Values
 *  ----------------------------------------
 */
struct xp {
	struct nlm_timers_xp timers;
	struct nlm_opt_conf_xp config;
	struct nlm_stats_xp statsp;
	struct nlm_stats_xp stats;
	struct nlm_notify_xp notify;
};					/* X25 PLE initial values */

static struct xp *np_alloc_xp(uint, struct nlm_conf_xp *);
static void np_free_xp(struct xp *);
static struct xp *xp_lookup(uint);
static uint xp_get_id(uint);

/*
 *  VC - Virtual Call
 *  ----------------------------------------
 */
struct vc {
	struct nlm_timers_vc timers;
	struct nlm_opt_conf_vc config;
	struct nlm_stats_vc statsp;
	struct nlm_stats_vc stats;
	struct nlm_notify_vc notify;
};					/* Virtual Circuit */

static struct vc *np_alloc_vc(uint, struct nlm_conf_vc *);
static void np_free_vc(struct vc *);
static struct vc *vc_lookup(uint);
static uint vc_get_id(uint);

/*
 *  VP - Virtual Call Profile (Intitial Values)
 *  ----------------------------------------
 */
struct vp {
	struct nlm_timers_vp timers;
	struct nlm_opt_conf_vp config;
	struct nlm_stats_vp statsp;
	struct nlm_stats_vp stats;
	struct nlm_notify_vp notify;
};					/* Virtual Circuit initial values */

static struct vp *np_alloc_vp(uint, struct nlm_conf_vp *);
static void np_free_vp(struct vp *);
static struct vp *vp_lookup(uint);
static uint vp_get_id(uint);

/*
 *  DL - DLS Provider
 *  ----------------------------------------
 */
struct dl {
	struct nlm_timers_dl timers;
	struct nlm_opt_conf_dl config;
	struct nlm_stats_dl statsp;
	struct nlm_stats_dl stats;
	struct nlm_notify_dl notify;
};					/* DLS User - lower multiplex stream */

static struct dl *np_alloc_dl(uint, struct nlm_conf_dl *);
static void np_free_dl(struct dl *);
static struct dl *dl_lookup(uint);
static uint dl_get_id(uint);

/*
 *  AJ - ISIS Adjacency
 *  ----------------------------------------
 */
struct aj {
	struct nlm_timers_aj timers;
	struct nlm_opt_conf_aj config;
	struct nlm_stats_aj statsp;
	struct nlm_stats_aj stats;
	struct nlm_notify_aj notify;
};					/* ISIS Adjacency */

static struct aj *np_alloc_aj(uint, struct nlm_conf_aj *);
static void np_free_aj(struct aj *);
static struct aj *aj_lookup(uint);
static uint aj_get_id(uint);

/*
 *  VA - ISIS Virtual Adjacency
 *  ----------------------------------------
 */
struct va {
	struct nlm_timers_va timers;
	struct nlm_opt_conf_va config;
	struct nlm_stats_va statsp;
	struct nlm_stats_va stats;
	struct nlm_notify_va notify;
};					/* ISIS Virtual Adjacency */

/*
 *  DS - ISIS Destination System
 *  ----------------------------------------
 */
struct ds {
	struct nlm_timers_ds timers;
	struct nlm_opt_conf_ds config;
	struct nlm_stats_ds statsp;
	struct nlm_stats_ds stats;
	struct nlm_notify_ds notify;
};					/* ISIS Destination System */

static struct ds *np_alloc_ds(uint, struct nlm_conf_ds *);
static void np_free_ds(struct ds *);
static struct ds *ds_lookup(uint);
static uint ds_get_id(uint);

/*
 *  DA - ISIS Destination Area
 *  ----------------------------------------
 */
struct da {
	struct nlm_timers_da timers;
	struct nlm_opt_conf_da config;
	struct nlm_stats_da statsp;
	struct nlm_stats_da stats;
	struct nlm_notify_da notify;
};					/* ISIS Destination Area */

static struct da *np_alloc_da(uint, struct nlm_conf_da *);
static void np_free_da(struct da *);
static struct da *da_lookup(uint);
static uint da_get_id(uint);

/*
 *  RA - ISIS Reachable Address
 *  ----------------------------------------
 */
struct ra {
	struct nlm_timers_ra timers;
	struct nlm_opt_conf_ra config;
	struct nlm_stats_ra statsp;
	struct nlm_stats_ra stats;
	struct nlm_notify_ra notify;
};					/* ISIS Reachable Address */

static struct ra *np_alloc_ra(uint, struct nlm_conf_ra *);
static void np_free_ra(struct ra *);
static struct ra *ra_lookup(uint);
static uint ra_get_id(uint);

#if 0
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
#endif

#define PRIV(__q) (((__q)->q_ptr))
#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

STATIC kmem_cachep_t np_priv_cachep;

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

	if ((np = kmem_cache_alloc(np_priv_cachep, GFP_ATOMIC))) {
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

#if 0
STATIC kmem_cachep_t dl_priv_cachep;

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

	if ((dl = kmem_cache_alloc(dl_priv_cachep, GFP_ATOMIC))) {
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
#endif

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (np, np);		/* master list of np (open) structures */
	SLIST_HEAD (dl, dl);		/* master list of dl (link) structures */
} df_t;

#if	defined __RW_LOCK_UNLOCKED
STATIC struct df master = {.lock = __RW_LOCK_UNLOCKED(master.lock), };
#elif	defined RW_LOCK_UNLOCKED
STATIC struct df master = {.lock = RW_LOCK_UNLOCKED, };
#else
#error cannot initialize read-write locks
#endif

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

#ifdef CONFIG_STREAMS_DEBUG
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
#endif				/* CONFIG_STREAMS_DEBUG */

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

#ifdef CONFIG_STREAMS_DEBUG
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
#endif				/* CONFIG_STREAMS_DEBUG */

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
		/* The put and service procedure do not recognize all errors. Sometimes we return an error to 
		   here just to restore the previous state. */
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
		/* The put and service procedure do not recognize all errors. Sometimes we return an error to 
		   here just to restore the previous state. */
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
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_priv_cachep)) {
			strlog(DRV_ID, 0, LOG_ERR, SL_ERROR | SL_CONSOLE, "could not destroy np_priv_cachep");
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(np_priv_cachep);
#endif
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "destroyed np_priv_cachep");
	}
	return (0);
}

STATIC int
np_init_caches(void)
{
	if (np_priv_cachep == NULL) {
		np_priv_cachep =
		    kmem_create_cache("np_priv_cachep", sizeof(struct np), 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_priv_cachep == NULL) {
			strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
			       "cannot allocate np_priv_cachep");
			np_term_caches();
			return (-ENOMEM);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "initialized driver private structure cache");
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

	/* FIXME: Here we need to handle the disposition of the state of the Stream.  If the state is not
	   unattached, we need to take it there. */

	qprocsoff(q);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&np->refcnt));
	ss7_unbufcall((str_t *) np);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE, "removed bufcalls: reference count = %d",
	       atomic_read(&np->refcnt));
	/* remove from master list */
	write_lock_bh(&master.lock);
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	write_unlock_bh(&master.lock);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE, "unlinked: reference count = %d",
	       atomic_read(&np->refcnt));
	np_release((struct np **) &np->oq->q_ptr);
	np->oq = NULL;
	np_release((struct np **) &np->iq->q_ptr);
	np->iq = NULL;
	assure(atomic_read(&np->refcnt) == 1);
	np_release(&np);
	return;
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
/**
 * np_i_link: - perform I_LINK operation
 * @np: NP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * The driver supports I_LINK operations; however, an DL Stream that is linked with an I_LINK
 * operation can only be managed by the control Stream linking the lower Stream and cannot be shared
 * across other upper Streams unless configured against a NET.
 *
 * Note that if this is not the first DL linked and there are running Users, this DL will not be
 * available to them until it is configured and brought to the active state.  If this is the first
 * DL, there cannot be running users.
 */
static int
np_i_link(queue_t *q, struct np *np, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	lmi_info_req_t *p;
	unsigned long flags;
	struct ne *ne;
	struct dl *dl = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	if (!(dl = np_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, 0))) {
		err = ENOMEM;
		goto error;
	}

	write_lock_irqsave(&np_mux_lock, flags);

	if (!(ne = np->ne.loc)) {
		write_unlock_irqrestore(&np_mux_lock, flags);
		err = ENXIO;
		goto error;
	}
	if (!ne_trylock(q, ne)) {
		err = -EDEADLK;
		write_unlock_irqrestore(&np_mux_lock, flags);
		goto error;
	}
	/* Note that there can only be one layer management Stream per SP.  For temporary links, that must be 
	   the same layer management Stream used to create the SP. */
	if (ne->np.lm != np) {
		err = EPERM;
		ne_unlock(ne);
		write_unlock_irqrestore(&np_mux_lock, flags);
		goto error;
	}

	if ((dl->ne.next = ne->dl.list))
		dl->ne.next->ne.prev = &dl->ne.next;
	dl->ne.prev = &ne->dl.list;
	dl->ne.ne = ne;
	ne->dl.list = dl;

	mi_attach(l->l_qtop, (caddr_t) dl);
	ne_unlock(ne);
	write_unlock_irqrestore(&np_mux_lock, flags);

	mi_copy_done(q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->lmi_primitive = LMI_INFO_REQ;
	rp->b_wptr = rp->b_rptr + sizeof(*p);
	putnext(dl->wq, rp);	/* immediate info request */

	return (0);
      error:
	if (rp)
		freeb(rp);
	if (dl) {
		np_free_link(dl);
	}
	if (err >= 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	return (err);
}

static int
np_i_plink(queue_t *q, struct np *np, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;
	int err = EPERM;

#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
	if (ioc->ioc_cr->cr_uid.val == 0)
#else
	if (ioc->ioc_cr->cr_uid == 0)
#endif
	{
		err = ENOMEM;
		if ((dl = np_alloc_link(l->l_qbot, l->l_index, ioc->ioc_cr, 0)))
			err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}

static int
np_i_unlink(queue_t *q, struct np *np, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;

	dl = SL_PRIV(l->l_qtop);
	np_free_link(dl);
	mi_copy_done(q, mp, 0);
	return (0);
}

static int
np_i_punlink(queue_t *q, struct np *np, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;
	int err = EPERM;

#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
	if (ioc->ioc_cr->cr_uid.val == 0)
#else
	if (ioc->ioc_cr->cr_uid == 0)
#endif
	{
		dl = SL_PRIV(l->l_qtop);
		np_free_link(dl);
		err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}

static int
np_i_ioctl(queue_t *q, struct np *np, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		return np_i_link(q, np, mp);
	case _IOC_NR(I_PLINK):
		return np_i_plink(q, np, mp);
	case _IOC_NR(I_UNLINK):
		return np_i_unlink(q, np, mp);
	case _IOC_NR(I_PUNLINK):
		return np_i_punlink(q, np, mp);
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/**
 * np_ioctl: - process NP M_IOCTL message
 * @np: NP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists of copying in the
 * necessary prefix structure that identifies the object type and id being managed.
 */
static int
np_ioctl(queue_t *q, struct np *np, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(NLM_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCGOPTION)");
		size = sizeof(struct nlm_option);
		break;
	case _IOC_NR(NLM_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCSOPTION)");
		size = sizeof(struct nlm_option);
		break;
	case _IOC_NR(NLM_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCSCONFIG)");
		size = sizeof(struct nlm_config);
		break;
	case _IOC_NR(NLM_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCGCONFIG)");
		size = sizeof(struct nlm_config);
		break;
	case _IOC_NR(NLM_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCTCONFIG)");
		size = sizeof(struct nlm_config);
		break;
	case _IOC_NR(NLM_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCCCONFIG)");
		size = sizeof(struct nlm_config);
		break;
#ifdef NLM_IOCLCONFIG
	case _IOC_NR(NLM_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCLCONFIG)");
		size = sizeof(struct nlm_config);
		break;
#endif				/* NLM_IOCLCONFIG */
	case _IOC_NR(NLM_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCGSTATEM)");
		size = sizeof(struct nlm_statem);
		break;
	case _IOC_NR(NLM_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCCMRESET)");
		size = sizeof(struct nlm_statem);
		break;
	case _IOC_NR(NLM_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCGSTATSP)");
		size = sizeof(struct nlm_stats);
		break;
	case _IOC_NR(NLM_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCSSTATSP)");
		size = sizeof(struct nlm_stats);
		break;
	case _IOC_NR(NLM_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCGSTATS)");
		size = sizeof(struct nlm_stats);
		break;
	case _IOC_NR(NLM_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCCSTATS)");
		size = sizeof(struct nlm_stats);
		break;
	case _IOC_NR(NLM_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCGNOTIFY)");
		size = sizeof(struct nlm_notify);
		break;
	case _IOC_NR(NLM_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCSNOTIFY)");
		size = sizeof(struct nlm_notify);
		break;
	case _IOC_NR(NLM_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCCNOTIFY)");
		size = sizeof(struct nlm_notify);
		break;
	case _IOC_NR(NLM_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCCMGMT)");
		size = sizeof(struct nlm_mgmt);
		break;
	case _IOC_NR(NLM_IOCCPASS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(NLM_IOCCPASS)");
		size = sizeof(struct nlm_pass);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/* this structure is just to determine the maximum size of an ioctl */
union nlm_ioctls {
	struct {
		struct nlm_option option;
		union nlm_option_obj obj;
	} opt_conf;
	struct {
		struct nlm_config config;
		union nlm_conf_obj obj;
	} conf;
	struct {
		struct nlm_statem statem;
		union nlm_statem_obj obj;
	} statem;
	struct {
		struct nlm_stats stats;
		union nlm_stats_obj obj;
	} stats;
	struct {
		struct nlm_notify notify;
		union nlm_notify_obj obj;
	} notify;
	struct nlm_mgmt mgmt;
	struct nlm_pass pass;
};

/**
 * np_copyin: - process NLM M_IOCDATA message
 * @np: NP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step 2 of the NLM input-output control operation.  Step 2 consists of copying out for GET
 * operations, and processing an additional copy in operation of object specific information for SET
 * operations.
 */
static int
np_copyin(queue_t *q, struct np *np, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(NLM_IOCGOPTION):
	case _IOC_NR(NLM_IOCGCONFIG):
	case _IOC_NR(NLM_IOCGSTATEM):
	case _IOC_NR(NLM_IOCGSTATSP):
	case _IOC_NR(NLM_IOCGSTATS):
	case _IOC_NR(NLM_IOCCSTATS):
	case _IOC_NR(NLM_IOCGNOTIFY):
		len = sizeof(union nlm_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
#ifdef NLM_IOCLCONFIG
	case _IOC_NR(NLM_IOCLCONFIG):
	{
		struct nlm_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
#endif				/* NLM_IOCLCONFIG */
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(NLM_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGOPTION)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_get_options(bp);
		break;
	case _IOC_NR(NLM_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSOPTION)");
		err = nlm_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(NLM_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGCONFIG)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_get_conf(bp);
		break;
	case _IOC_NR(NLM_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSCONFIG)");
		err = nlm_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(NLM_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCTCONFIG)");
		err = nlm_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(NLM_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCCONFIG)");
		err = nlm_size_conf(dp->b_rptr, &size);
		break;
#ifdef NLM_IOCLCONFIG
	case _IOC_NR(NLM_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCLCONFIG)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_lst_conf(bp);
		break;
#endif				/* NLM_IOCLCONFIG */
	case _IOC_NR(NLM_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGSTATEM)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_get_statem(bp);
		break;
	case _IOC_NR(NLM_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCMRESET)");
		err = EOPNOTSUPP; /* later */ ;
		break;
	case _IOC_NR(NLM_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGSTATSP)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_do_statsp(bp, NLM_GET);
		break;
	case _IOC_NR(NLM_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSSTATSP)");
		err = nlm_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(NLM_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGSTATS)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_get_stats(bp, false);
		break;
	case _IOC_NR(NLM_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCSTATS)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_get_stats(bp, true);
		break;
	case _IOC_NR(NLM_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCSTATS)");
		if (bp == NULL)
			goto enobufs;
		err = nlm_do_notify(bp, 0, NLM_GET);
		break;
	case _IOC_NR(NLM_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSNOTIFY)");
		err = nlm_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(NLM_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCNOTIFY)");
		err = nlm_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(NLM_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCMGMT)");
		err = nlm_action(q, dp);
		break;
	case _IOC_NR(NLM_IOCCPASS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCPASS)");
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin_n(q, mp, 0, size);
	}
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/**
 * np_copyin2: - process NLM M_IOCDATA message
 * @np: NP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * Step 3 of the input-output control operation is an optional step that is used for SET operations.
 * After the second copyin we now have the object specific structure that was passed by the user and
 * can complete the SET operation.  All SET operations also include a last copyout step that copies
 * out the information actually set (e.g. the assigned id on NLM_ADD operations).
 */
static int
np_copyin2(queue_t *q, struct np *np, mblk_t *mp, mblk_t *dp)
{
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(((struct copyresp *) mp->b_rptr)->cp_cmd)) {
	case _IOC_NR(NLM_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGOPTION)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSOPTION)");
		err = nlm_set_options(bp);
		break;
	case _IOC_NR(NLM_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGCONFIG)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSCONFIG)");
		err = nlm_set_conf(bp);
		break;
	case _IOC_NR(NLM_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCTCONFIG)");
		err = nlm_test_conf(bp);
		break;
	case _IOC_NR(NLM_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCCONFIG)");
		err = nlm_commit_conf(bp);
		break;
#ifdef NLM_IOCLCONFIG
	case _IOC_NR(NLM_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCLCONFIG)");
		err = EPROTO;
		break;
#endif				/* NLM_IOCLCONFIG */
	case _IOC_NR(NLM_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGSTATEM)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCMRESET)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGSTATSP)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSSTATSP)");
		err = nlm_do_statsp(bp, NLM_CHA);
		break;
	case _IOC_NR(NLM_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGSTATS)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCSTATS)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCGNOTIFY)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCSNOTIFY)");
		err = nlm_do_notify(bp, 0, NLM_ADD);
		break;
	case _IOC_NR(NLM_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCNOTIFY)");
		err = nlm_do_notify(bp, 0, NLM_DEL);
		break;
	case _IOC_NR(NLM_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCMGMT)");
		err = EPROTO;
		break;
	case _IOC_NR(NLM_IOCCPASS):
	{
		struct nlm_pass *p = (typeof(p)) dp->b_rptr;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(NLM_IOCCPASS)");
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
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/**
 * np_copyout: - process NLM M_IOCDATA message
 * @np: NP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 * 
 * Step 4 and the final step of the input-output control operation is a final copyout step.
 */
static int
np_copyout(queue_t *q, struct np *np, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

/**
 * np_w_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static int
np_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct np *np;
	int err = 0;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if (!(np = np_acquire(q)))
		return (-EAGAIN);

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		err = np_i_ioctl(q, np, mp);
		break;
	case NLM_IOC_MAGIC:
		err = np_ioctl(q, np, mp);
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	np_release(np);
	return (err);

}

/**
 * np_w_iocdata: - process M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 */
static int
np_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct np *np;
	int err = 0;
	mblk_t *dp;

	if (!(np = np_acquire(q)))
		return (-EAGAIN);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case NLM_IOC_MAGIC:
			err = np_copyin(q, np, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case NLM_IOC_MAGIC:
			err = np_copyin2(q, np, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case NLM_IOC_MAGIC:
			err = np_copyout(q, np, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	np_release(np);
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
static noinline fastcall int
npi_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(np_ulong *) mp->b_rptr) {
	case N_CONN_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_CONN_REQ");
		return n_conn_req(q, mp);
	case N_CONN_RES:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_CONN_RES");
		return n_conn_res(q, mp);
	case N_DISCON_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_DISCON_REQ");
		return n_discon_req(q, mp);
	case N_DATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_DATA_REQ");
		return n_data_req(q, mp);
	case N_EXDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_EXDATA_REQ");
		return n_exdata_req(q, mp);
	case N_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_INFO_REQ");
		return n_info_req(q, mp);
	case N_BIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_BIND_REQ");
		return n_bind_req(q, mp);
	case N_UNBIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_UNBIND_REQ");
		return n_unbind_req(q, mp);
	case N_UNITDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_UNITDATA_REQ");
		return n_unitdata_req(q, mp);
	case N_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_OPTMGMT_REQ");
		return n_optmgmt_req(q, mp);
	case N_DATACK_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_DATACK_REQ");
		return n_datack_req(q, mp);
	case N_RESET_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_RESET_REQ");
		return n_reset_req(q, mp);
	case N_RESET_RES:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_RESET_RES");
		return n_reset_res(q, mp);
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_????_???");
		return n_error_ack(q, NP_PRIV(q), mp, *(np_ulong *) mp->b_rptr, NNOTSUPPORT);
	}
}

static inline fastcall int
np_w_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if (likely((np = np_acquire(q)) != NULL)) {
		uint oldstate = np_get_state(np);

		if ((err = npi_w_proto(q, mp)))
			np_set_state(np, oldstate);
		np_release(np);
	}
	return (err < 0 ? err : 0);
}

static int
dlpi_r_proto(queue_t *q, mblk_t *mp)
{
	switch (*(dl_ulong *) mp->b_rptr) {
	case DL_UNITDATA_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "DL_UNITDATA_IND [%u] <-", (uint) msgdsize(mp->b_cont));
		return dl_unitdata_ind(q, mp);
	case DL_DATA_ACK_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "DL_DATA_ACK_IND <-");
		return dl_data_ack_ind(q, mp);
	case DL_DATA_ACK_STATUS_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "DL_DATA_ACK_STATUS_IND <-");
		return dl_data_ack_status_ind(q, mp);
	case DL_INFO_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_INFO_ACK <-");
		return dl_info_ack(q, mp);
	case DL_BIND_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_BIND_ACK <-");
		return dl_info_ack(q, mp);
	case DL_OK_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_OK_ACK <-");
		return dl_info_ack(q, mp);
	case DL_ERROR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_ERROR_ACK <-");
		return dl_info_ack(q, mp);
	case DL_SUBS_BIND_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_SUBS_BIND_ACK <-");
		return dl_info_ack(q, mp);
	case DL_UDERROR_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_UDERROR_IND <-");
		return dl_info_ack(q, mp);
	case DL_CONNECT_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_CONNECT_IND <-");
		return dl_info_ack(q, mp);
	case DL_CONNECT_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_CONNECT_CON <-");
		return dl_info_ack(q, mp);
	case DL_TOKEN_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_TOKEN_ACK <-");
		return dl_info_ack(q, mp);
	case DL_DISCONNECT_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_DISCONNECT_IND <-");
		return dl_info_ack(q, mp);
	case DL_RESET_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_RESET_IND <-");
		return dl_info_ack(q, mp);
	case DL_RESET_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_RESET_CON <-");
		return dl_info_ack(q, mp);
	case DL_REPLY_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_REPLY_IND <-");
		return dl_info_ack(q, mp);
	case DL_REPLY_STATUS_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_REPLY_STATUS_IND <-");
		return dl_info_ack(q, mp);
	case DL_REPLY_UPDATE_STATUS_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_REPLY_UPDATE_STATUS_IND <-");
		return dl_info_ack(q, mp);
	case DL_XID_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_XID_IND <-");
		return dl_info_ack(q, mp);
	case DL_XID_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_XID_CON <-");
		return dl_info_ack(q, mp);
	case DL_TEST_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_TEST_IND <-");
		return dl_info_ack(q, mp);
	case DL_TEST_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_TEST_CON <-");
		return dl_info_ack(q, mp);
	case DL_PHYS_ADDR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_PHYS_ADDR_ACK <-");
		return dl_info_ack(q, mp);
	case DL_GET_STATISTICS_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_GET_STATISTICS_ACK <-");
		return dl_info_ack(q, mp);
	default:
		/* reject what we don't recognize */
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_????_??? <-");
		return EOPNOTSUPP;
	}
}

static int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;

	if ((dl = dl_acquire(q))) {
		uint oldstate = dl_get_l_state(dl);

		if ((err = dlpi_r_proto(q, mp)))
			dl_set_state(q, dl, oldstate);

		dl_release(dl);
	}
	return (err < 0 ? err : 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline fastcall int
np_w_data(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if (likely((np = np_acquire(q)) != NULL)) {
		err = n_data(q, np, mp);
		np_release(np);
	}
	return (err < 0 ? err : 0);
}

static int
dl_r_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;

	if (likely(! !(dl = dl_acquire(q)))) {
		err = dl_data(q, mp);
		dl_release(dl);
	}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_SIG, M_PCSIG Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
do_timeout(queue_t *q, mblk_t *mp)
{
	struct np_timer *t = (typeof(t)) mp->b_rptr;
	int rtn;

	switch (t->timer) {
	case t1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t1 expiry at %lu", jiffies);
		return ne_t1_timeout(q, t->ne);
	default:
		return (0);
	}
	return (rtn);
}

static int
np_w_sig(queue_t *q, mblk_t *mp)
{
	struct np *np;
	uint oldstate;
	int err = 0;

	if (!(np = np_acquire(q)))
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);

	oldstate = np_get_state(np);

	if (likely(mi_timer_valid(mp))) {
		err = do_timeout(q, mp);

		if (err) {
			np_set_state(np, oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	np_release(np);
	return (err < 0 ? err : 0);
}

static int
dl_r_sig(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	uint oldstate;
	int err = 0;

	if (!(dl = dl_acquire(q)))
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);

	oldstate = dl_get_l_state(dl);

	if (likely(mi_timer_valid(mp))) {
		err = do_timeout(q, mp);

		if (err) {
			dl_set_state(q, dl, oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	dl_release(dl);
	return (err < 0 ? err : 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_RSE, M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
np_w_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

static int
dl_r_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Unknown STREAMS Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
np_w_unknown(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

static int
dl_r_unknown(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
np_w_flush(queue_t *q, mblk_t *mp)
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
		return (0);
	}
	freemsg(mp);
	return (0);
}

static int
dl_r_flush(queue_t *q, mblk_t *mp)
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
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.  Move link to the
 *  out-of-service state, notify management, and perform normal link failure actions.
 */
static int
dl_r_error(queue_t *q, mblk_t *mp)
{
	struct dl *dl = SL_PRIV(q);

	dl_set_i_state(dl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.  Move link to the
 *  out-of-service state, notify management, and perform normal link failure actions.
 */
static int
dl_r_hangup(queue_t *q, mblk_t *mp)
{
	struct dl *dl = SL_PRIV(q);

	dl_set_i_state(dl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

static noinline fastcall int
np_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return np_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return np_w_sig(q, mp);
	case M_FLUSH:
		return np_w_flush(q, mp);
	case M_IOCTL:
		return np_w_ioctl(q, mp);
	case M_IOCDATA:
		return np_w_iocdata(q, mp);
	case M_RSE:
	case M_PCRSE:
		return np_w_rse(q, mp);
	default:
		return np_w_unknown(q, mp);
	}
}

static noinline fastcall int
dl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_r_sig(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
#if 0
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_copy(q, mp);
#endif
	case M_RSE:
	case M_PCRSE:
		return dl_r_rse(q, mp);
	default:
		return dl_r_unknown(q, mp);
	}
}

static inline fastcall int
np_w_msg(queue_t *q, mblk_t *mp)
{
	/* TODO: write a fast path */
	return np_w_msg_slow(q, mp);
}

static inline fastcall int
dl_r_msg(queue_t *q, mblk_t *mp)
{
	/* TODO: write a fast path */
	return dl_r_msg_slow(q, mp);
}

/*
 *  QUEUE PUT and SERVICE procedures
 *  =========================================================================
 *  Canonical STREAMS multiplexing driver processing.
 */
static streamscall __hot_put int
np_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || np_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_out int
np_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (np_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_get int
np_rsrv(queue_t *q)
{
	/* FIXME: backenable across the mux */
	return (0);
}

static streamscall __unlikely int
np_rput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "np_rput() called");
	putnext(q, mp);
	return (0);
}

static streamscall __unlikely int
dl_wput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "dl_wput() called");
	putnext(q, mp);
	return (0);
}

static streamscall __hot_out int
dl_wsrv(queue_t *q)
{
	/* FIXME: backenable across the mux */
	return (0);
}

static streamscall __hot_get int
dl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_in int
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || dl_r_msg(q, mp))
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
// STATIC int np_majors[CMAJORS] = { CMAJOR_0, };

struct np_profile {
	struct {
		uint family;
		uint type;
		uint protocol;
	} prot;
	N_info_ack_t info;
};

static const struct np_profile np_profiles[] = {
	{{PF_OSI, SOCK_DGRAM, IPPROTO_CLNP},
	 {N_INFO_ACK, 512, 512, 0, 0, 20, sizeof(N_info_ack_t),
		 sizeof(N_qos_cl_sel_t), sizeof(N_info_ack_t)+20,
		 sizeof(N_qos_cl_range_t), sizeof(N_info_ack_t)+20+sizeof(N_qos_cl_sel_t),
		 REC_CONF_OPT|EX_DATA_OPT,
		 (1<<16), N_CLNS, NS_IDLE, N_SUBNET, 512,
		 1, sizeof(N_info_ack_t)+20+sizeof(N_qos_cl_sel_t) + sizeof(N_qos_cl_range_t),
		 N_VERSION_2}}

};


/**
 * np_qopen: - STREAMS driver open routine
 * @q: read queue of newly created queue pair
 * @devp: pointer to device number associated with Stream
 * @oflags: flags to the open(2s) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to an NE associated with the driver.  The NE
 * is determined from the minor device opened.  All minor devices correpsponding to NE are clone or
 * auto-clone devices.  There may be several DL lower Streams for each NE.  If an NE structure has
 * not been allocated for the corresponding minor device number, we allocate one.  When an NE Stream
 * is I_LINK'ed under the driver, it is associated with the NE structure.  An NP structure is
 * allocated and associated with each upper Stream.
 *
 * This driver cannot be pushed as a module.
 *
 * (cminor == 0) && (sflag == DRVOPEN)
 *	When minor device number 0 is opened with DRVOPEN (non-clone), a control Stream is opened.
 *	If a control Stream has already been opened, the open is refused.  The @sflag is changed
 *	from %DRVOPEN to %CLONEOPEN and a new minor device number above NUM_NE is assigned.  This
 *	uses the autocloning features of Linux Fast-STREAMS.  This corresponds to the
 *	/dev/streams/clns/nlm clone device node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A disassociated user Stream is
 *	opened.  A new unique minor device number above NUM_NE is assigned.  This corresponds to the 
 *	/dev/streams/clone/clns clone device node.
 *
 * (1 <= cminor && cminor <= NUM_NE)
 *	This is a normal non-clone open.  Where the minor device number is between 1 and NUM_NE, an
 *	associated user Stream is opened.  If there is no NE structure to associate, one is created
 *	with default values (and associated with the default NET).  A new minor device number above
 *	NUM_NE is assigned.  This uses the autocloning features of Linux Fast-STREAMS.  This
 *	corresponds to the /dev/streams/clns/NNNN minor device node where NNNN is the minor device
 *	number.
 */
#ifndef NUM_NE
#define NUM_NE 16
#endif
static caddr_t np_opens = NULL;
static struct np *nlm_ctrl = NULL;

DECLARE_WAIT_QUEUE_HEAD(np_waitq);
STATIC streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	psw_t flags;
	major_t cmajor = getmajor(*devp);
	major_t cminor = getminor(*devp);
	struct np *np;
	int err;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		mi_strlog(q, 0, SL_ERROR, "cannot push as module");
		return (ENXIO);
	}
	if (cminor > NUM_NE) {
		mi_strlog(q, 0, SL_ERROR, "cannot open dynamic minor device number");
		return (ENXIO);
	}
	if (cmajor != NP_CMAJOR_0 || cminor >= NP_CMINOR_FREE) {
		mi_strlog(q, 0, SL_ERROR, "cannot open cloned minors");
		return (ENXIO);
	}
	if (!(np = np_alloc_priv(q, devp, crp, cminor)))
		return (ENOMEM);
	*devp = makedevice(cmajor, NUM_NE + 1);
	/* start assigning minors at NUM_NE + 1 */

	write_lock_irqsave(&np_mux_lock, flags);

	if (cminor == 0) {
		/* When a zero minor device number was opened, the Stream is either a clone open or an
		   attempt to open the master control Stream.  The difference is whether the @sflag was
		   %DRVOPEN or %CLONEOPEN. */
		if (sflag == DRVOPEN) {
			/* When the master control Stream is opened, another master control Stream must not
			   yet exist.  If this is the only master control Stream then it is created. */
			if (nlm_ctrl != NULL) {
				write_unlock_irqrestore(&np_mux_lock, flags);
				np_free_priv(np);
				return (ENXIO);
			}
		}
		if ((err = mi_open_link(&np_opens, (caddr_t) np, devp, oflags, CLONEOPEN, crp))) {
			write_unlock_irqrestore(&np_mux_lock, flags);
			np_free_priv(np);
			return (ENXIO);
		}
		if (sflag == DRVOPEN)
			nlm_ctrl = np;
		/* Both master control Streams and clone user Streams are disassociated with any specific NE. 
		   Master control Streams are never associated with a specific NE. User Streams are
		   associated with an NE uasing the NET in the NSAP address to the bind primitive, or when a
		   DL Stream is temporarily linked under the driver using the I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);
		struct ne *ne;

		/* When a non-zero minor device number was opened, the Stream is automatically associated
		   with the NE to which the minor device number corresponds.  It cannot be disassociated
		   except when it is closed. */
		if (!(ne = ne_lookup(cminor))) {
			write_unlock_irqrestore(&np_mux_lock, flags);
			np_free_priv(np);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the NE structure can be acquired, or a signal is
		   received, or the NE structure is deallocated.  If the lock can be acquired, associate the
		   User Stream with the NE structure; in all other cases, return an error.  Note that it is a 
		   likely event that the lock can be acquired without waiting. */
		err = 0;
		add_wait_queue(&np_waitq, &wait);
		spin_lock(&ne->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&ne->sq.lock);
				break;
			}
			if (ne->sq.users != 0) {
				spin_unlock(&ne->sq.lock);
				write_unlock_irqrestore(&np_mux_lock, flags);
				schedule();
				write_lock_irqsave(&np_mux_lock, flags);
				if (!(ne = ne_lookup(cminor))) {
					err = ENXIO;
					break;
				}
				spin_lock(&ne->sq.lock);
				continue;
			}
			err = mi_open_link(&np_opens, (caddr_t) np, devp, oflags, CLONEOPEN, crp);
			if (err == 0)
				np_attach(np, ne, 0);
			spin_unlock(&ne->sq.lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&np_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&np_mux_lock, flags);
			np_free_priv(np);
			return (err);
		}
	}
	write_unlock_irqrestore(&np_mux_lock, flags);

	mi_attach(q, (caddr_t) np);
	qprocson(q);
	return (0);
}
static streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	minor_t cminor = getminor(*devp);
	psw_t flags;
	caddr_t ptr;
	struct np *np;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR)
		return (ENXIO);
	if (!mi_set_sth_lowat(q, 0))
		return (ENOBUFS);
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		return (ENOBUFS);
	if ((err = np_alloc_qbands(q, cminor)))
		return (err);
	if (!(np = (struct np *) (ptr = mi_open_alloc_cache(np_priv_cachep, GFP_KERNEL))))
		return (ENOMEM);
	{
		bzero(np, sizeof(*np));
		np->ss_parent = np;
		np->rq = RD(q);
		np->wq = WR(q);
		np->p = np_profiles[cminor - FIRST_CMINOR];
		np->cred = *crp;
		bufq_init(&np->conq);
	}
	sflag = CLONEOPEN;
	cminor = FREE_CMINOR;	/* start at the first free minor device number */
	/* The static device range for mi_open_link() is 5 or 10, and clns uses 50, so we need to adjust the
	   minor device number before calling mi_open_link(). */
	*devp = makedevice(getmajor(*devp), cminor);
	write_lock_irqsave(&np_mux_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &np_mux_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err = mi_open_link(&np_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&np_mux_lock, flags);
	np_priv_init(np);
	return (0);
      unlock_free:
	mi_close_free_cache(np_priv_cachep, ptr);
	write_unlock_irqrestore(&np_mux_lock, flags);
	return (err);
}

/**
 * np_qclose: - STREAMS driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2s) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
np_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct np *np = NP_PRIV(q);
	caddr_t ptr = (caddr_t) np;
	psw_t flags;
	int err;

	write_lock_irqsave(&np_mux_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &np_mux_lock, &flags);
	qprocsoff(q);
	while (bufq_head(&np->conq))
		freemsg(__bufq_dequeue(&np->conq));
	np->np_parent = NULL;
	np->rq = NULL;
	np->wq = NULL;
	err = mi_close_comm(&np_opens, q);
	write_unlock_irqrestore(&np_mux_lock, flags);
	return (err);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
static kmem_cachep_t np_np_cachep = NULL;	/* NLS User cache */
static kmem_cachep_t np_ne_cachep = NULL;	/* Network Entity cache */
static kmem_cachep_t np_cn_cachep = NULL;	/* Connection cache */
static kmem_cachep_t np_xp_cachep = NULL;	/* X25 PLE IVMO cache */
static kmem_cachep_t np_vc_cachep = NULL;	/* virtual call cache */
static kmem_cachep_t np_vp_cachep = NULL;	/* virtual call IVMO cache */
static kmem_cachep_t np_dl_cachep = NULL;	/* Datalink cache */
static kmem_cachep_t np_aj_cachep = NULL;	/* Adjacency cache */
static kmem_cachep_t np_va_cachep = NULL;	/* Vritual adjacency cache */
static kmem_cachep_t np_ds_cachep = NULL;	/* Destination system cache */
static kmem_cachep_t np_da_cachep = NULL;	/* Destination area cache */
static kmem_cachep_t np_ra_cachep = NULL;	/* Reachable address cache */
static int
np_init_caches(void)
{
	size_t sizeof_struct_np = mi_open_size(sizeof(struct np));
	size_t sizeof_struct_dl = mi_open_size(sizeof(struct dl));

	if (!np_np_cachep
	    && !(np_np_cachep =
		 kmem_create_cache("np_np_cachep", sizeof_struct_np, 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_np_cachep", DRV_NAME);
		goto failed_np;
	} else
		printd(("%s: initialized np structure cache", DRV_NAME));
	if (!np_ne_cachep
	    && !(np_ne_cachep =
		 kmem_create_cache("np_ne_cachep", sizeof(struct ne), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_ne_cachep", DRV_NAME);
		goto failed_ne;
	} else
		printd(("%s: initialized ne structure cache", DRV_NAME));
	if (!np_cn_cachep
	    && !(np_cn_cachep =
		 kmem_create_cache("np_cn_cachep", sizeof(struct cn), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_cn_cachep", DRV_NAME);
		goto failed_cn;
	} else
		printd(("%s: initialized cn structure cache", DRV_NAME));
	if (!np_xp_cachep
	    && !(np_xp_cachep =
		 kmem_create_cache("np_xp_cachep", sizeof(struct xp), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_xp_cachep", DRV_NAME);
		goto failed_xp;
	} else
		printd(("%s: initialized xp structure cache", DRV_NAME));
	if (!np_vc_cachep
	    && !(np_vc_cachep =
		 kmem_create_cache("np_vc_cachep", sizeof(struct vc), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_vc_cachep", DRV_NAME);
		goto failed_vc;
	} else
		printd(("%s: initialized vc structure cache", DRV_NAME));
	if (!np_vp_cachep
	    && !(np_vp_cachep =
		 kmem_create_cache("np_vp_cachep", sizeof(struct vp), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_vp_cachep", DRV_NAME);
		goto failed_vp;
	} else
		printd(("%s: initialized vp structure cache", DRV_NAME));
	if (!np_aj_cachep
	    && !(np_aj_cachep =
		 kmem_create_cache("np_aj_cachep", sizeof(struct aj), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_aj_cachep", DRV_NAME);
		goto failed_aj;
	} else
		printd(("%s: initialized va structure cache", DRV_NAME));
	if (!np_va_cachep
	    && !(np_va_cachep =
		 kmem_create_cache("np_va_cachep", sizeof(struct va), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_va_cachep", DRV_NAME);
		goto failed_va;
	} else
		printd(("%s: initialized va structure cache", DRV_NAME));
	if (!np_ds_cachep
	    && !(np_ds_cachep =
		 kmem_create_cache("np_ds_cachep", sizeof(struct ds), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_ds_cachep", DRV_NAME);
		goto failed_ds;
	} else
		printd(("%s: initialized ds structure cache", DRV_NAME));
	if (!np_da_cachep
	    && !(np_da_cachep =
		 kmem_create_cache("np_da_cachep", sizeof(struct da), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_da_cachep", DRV_NAME);
		goto failed_da;
	} else
		printd(("%s: initialized da structure cache", DRV_NAME));
	if (!np_ra_cachep
	    && !(np_ra_cachep =
		 kmem_create_cache("np_ra_cachep", sizeof(struct ra), 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_ra_cachep", DRV_NAME);
		goto failed_ra;
	} else
		printd(("%s: initialized ra structure cache", DRV_NAME));
	if (!np_dl_cachep
	    && !(np_dl_cachep =
		 kmem_create_cache("np_dl_cachep", sizeof_struct_dl, 0, SLAB_WHCACHE_ALIGN, NULL, NULL))) {
		cnm_nerr(CE_PANIC, "%s: did not allocate np_dl_cachep", DRV_NAME);
		goto failed_dl;
	} else
		printd(("%s: initialized dl structure cache", DRV_NAME));
	return (0);
	kmem_cache_destroy(np_dl_cachep);
      failed_dl:
	kmem_cache_destroy(np_ra_cachep);
      failed_ra:
	kmem_cache_destroy(np_da_cachep);
      failed_da:
	kmem_cache_destroy(np_ds_cachep);
      failed_ds:
	kmem_cache_destroy(np_va_cachep);
      failed_va:
	kmem_cache_destroy(np_aj_cachep);
      failed_aj:
	kmem_cache_destroy(np_vp_cachep);
      failed_vp:
	kmem_cache_destroy(np_vc_cachep);
      failed_vc:
	kmem_cache_destroy(np_xp_cachep);
      failed_xp:
	kmem_cache_destroy(np_cn_cachep);
      failed_cn:
	kmem_cache_destroy(np_ne_cachep);
      failed_ne:
	kmem_cache_destroy(np_np_cachep);
      failed_np:
	return (-ENOMEM);
}

static int
np_term_caches(void)
{
	int err = 0;

	if (np_np_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_np_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_np_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_np_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_np_cachep);
#endif
	}
	if (np_ne_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_ne_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_ne_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_ne_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_ne_cachep);
#endif
	}
	if (np_cn_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_cn_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_cn_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_cn_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_cn_cachep);
#endif
	}
	if (np_xp_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_xp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_xp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_xp_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_xp_cachep);
#endif
	}
	if (np_vc_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_vc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_vc_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_vc_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_vc_cachep);
#endif
	}
	if (np_vp_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_vp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_vp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_vp_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_vp_cachep);
#endif
	}
	if (np_aj_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_aj_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_aj_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_aj_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_aj_cachep);
#endif
	}
	if (np_va_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_va_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_va_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_va_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_va_cachep);
#endif
	}
	if (np_ds_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_ds_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_ds_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_ds_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_ds_cachep);
#endif
	}
	if (np_da_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_da_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_da_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_da_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_da_cachep);
#endif
	}
	if (np_ra_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_ra_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_ra_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_ra_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_ra_cachep);
#endif
	}
	if (np_dl_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_dl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_dl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed np_dl_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(np_dl_cachep);
#endif
	}
	return (err);
}

/*
 *  NP allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct np *
np_lookup(uint id)
{
	struct np *np = NULL;

	if (id)
		for (np = master.np.list; np && np->id != id; np = np->next) ;
	return (np);
}

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_stat np_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat np_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct module_info np_winfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME "-wr",	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_info np_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME "-rd",	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_info dl_winfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME "-muxw",	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_info dl_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME "-muxr",	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct qinit np_rinit = {
	.qi_putp = np_rput,	/* Write put (message from below) */
	.qi_srvp = np_rsrv,	/* Write queue service */
	.qi_qopen = np_qopen,	/* Each open */
	.qi_qclose = np_qclose,	/* Last close */
	.qi_minfo = &np_rinfo,	/* Information */
	.qi_mstat = &np_rstat,	/* Statistics */
};

static struct qinit np_winit = {
	.qi_putp = np_wput,	/* Write put (message from above) */
	.qi_srvp = np_wsrv,	/* Write queue service */
	.qi_minfo = &np_winfo,	/* Information */
	.qi_mstat = &np_wstat,	/* Statistics */
};

static struct qinit dl_rinit = {
	.qi_putp = dl_rput,	/* Write put (message from below) */
	.qi_srvp = dl_rsrv,	/* Write queue service */
	.qi_minfo = &dl_rinfo,	/* Information */
	.qi_mstat = &dl_rstat,	/* Statistics */
};

static struct qinit dl_winit = {
	.qi_putp = dl_wput,	/* Write put (message from above) */
	.qi_srvp = dl_wsrv,	/* Write queue service */
	.qi_minfo = &dl_winfo,	/* Information */
	.qi_mstat = &dl_wstat,	/* Statistics */
};

MODULE_STATIC struct streamtab np_info = {
	.st_rdinit = &np_rinit,	/* Upper read queue */
	.st_wrinit = &np_winit,	/* Upper write queue */
	.st_muxrinit = &dl_rinit,	/* Lower read queue */
	.st_muxwinit = &dl_winit,	/* Lower write queue */
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

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID number for CLNS driver (0-for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for CLNS driver (0 for allocation).");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static struct cdevsw np_cdev = {
	.d_name = DRV_NAME,
	.d_str = &np_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static struct devnode np_node_nlm = {
	.n_name = "nlm",
	.n_flag = 0,		/* non-clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode np_node_clnp = {
	.n_name = "clnp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode np_node_iso_ip = {
	.n_name = "iso-ip",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode np_node_iso_udp = {
	.n_name = "iso-udp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * np_register_strdev: - register STREAMS pseudo-device driver
 * @major: major device number to register (0 for allocation)
 *
 * This function registers the CLNS pseudo-device driver and a host of minor device nodes associated
 * with the driver.  Should this function fail it returns a negative error number; otherwise, it
 * returns zero.  Only failure to register the major device number is considered an error as minor
 * device notes in the specfs are optional.
 */
static __unlikely int
np_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&np_cdev, major)) < 0)
		return (err);
	if ((err = register_strnod(&ss_cdev, &np_node_nlm, NLM_CMINOR_NLM)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register NLM_CMINOR_NLM, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &np_node_clns, NLM_CMINOR_CLNS)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register NLM_CMINOR_CLNS, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &np_node_clnp, NLM_CMINOR_CLNP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register NLM_CMINOR_CLNP, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &np_node_iso_ip, NLM_CMINOR_ISO_IP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register NLM_CMINOR_ISO_IP, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &np_node_iso_udp, NLM_CMINOR_ISO_UDP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register NLM_CMINOR_ISO_UDP, err = %d", err);
	return (0);
}

/**
 * np_unregister_strdev: - unregister STREAMS pseudo-device driver
 * @major: major device number to unregister
 *
 * The function unregisters the host of minor device nodes and the major device node associated with
 * the driver in the reverse order in which they were allocated.  Only deregistration of the major
 * device node is considered fatal as minor device nodes were optional during initialization.
 */
static __unlikely int
np_register_strdev(major_t major)
{
	int err;

	if ((err = unregister_strnod(&ss_cdev, NLM_CMINOR_ISO_UDP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister NLM_CMINOR_ISO_UDP, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, NLM_CMINOR_ISO_IP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister NLM_CMINOR_ISO_IP, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, NLM_CMINOR_CLNP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister NLM_CMINOR_CLNP, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, NLM_CMINOR_CLNS)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister NLM_CMINOR_CLNS, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, NLM_CMINOR_NLM)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister NLM_CMINOR_NLM, err = %d", err);
	if ((err = unregister_strdev(&np_cdev, major)) < 0)
		return (err);
}

/**
 * np_init: - initialize the CLNS kernel module under Linux
 */
int __init
np_init(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = np_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		return (err);
	}
	if ((err = np_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver, err = %d", DRV_NAME, err);
		np_term_caches();
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

/**
 * np_exit: - remove the CLNS kernel module under Linux
 */
void __exit
np_exit(void)
{
	int err;

	if (major && (err = np_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", DRV_NAME, err);
	if (major && (err = np_unregister_strdev(major)) < 0)
		cmn_err(CE_PANIC, "%s: cannot unregister major %d, err = %d", DRV_NAME, major, err);
	return;
}

#ifdef MODULE
module_init(np_init);
module_exit(np_exit);
#endif				/* MODULE */

#endif				/* LINUX */
