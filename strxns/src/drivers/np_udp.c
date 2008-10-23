/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This driver provides the functionality of a UDP (User Datagram Protocol) hook similary to udp
 * sockets, with the exception that the hook acts as a port bound intercept for UDP packets for the
 * bound addresses.  This driver is used primarily by OpenSS7 protocol test module (e.g. for CLNS
 * over UDP) and for applications where a single port for an existing addres must be intercepted
 * (e.g. for RTP/RTCP).  This driver uses a hook into the Linux IP protocol tables and passes
 * packets transparently on to the underlying protocol in which it is not interested (bound).  The
 * driver uses the NPI (Network Provider Interface) API.
 */

#define _DEBUG 1
#undef	_DEBUG

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#if defined LINUX
#undef ASSERT

#include <linux/bitopts.h>

#define np_tst_bit(nr,addr) test_bit(nr,addr)
#define np_set_bit(nr,addr) __set_bit(nr,addr)
#define np_clr_bit(nr,addr) __clear_bit(nr,addr)

#include <linux/interrupt.h>

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <linux/udp.h>

#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/snmp.h>

#include <net/udp.h>
#include <net/tcp.h>		/* for checksumming */

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <net/protocol.h>

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#endif				/* LINUX */

#include "ip_hooks.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>
#include <sys/npi_udp.h>

#define NP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NP_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define NP_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define NP_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define NP_DEVICE	"SVR 4.2 STREAMS NPI NP_UDP Network Provider"
#define NP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NP_LICENSE	"GPL"
#define NP_BANNER	NP_DESCRIP	"\n" \
			NP_EXTRA	"\n" \
			NP_REVISION	"\n" \
			NP_COPYRIGHT	"\n" \
			NP_DEVICE	"\n" \
			NP_CONTACT
#define NP_SPLASH	NP_DESCRIP	"\n" \
			NP_REVISION

#ifdef LINUX
MODULE_AUTHOR(NP_CONTACT);
MODULE_DESCRIPTION(NP_DESCRIP);
MODULE_SUPPORTED_DEVICE(NP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-np_udp");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif
#endif				/* LINUX */

#ifdef LFS
#define NP_UDP_DRV_ID	CONFIG_STREAMS_NP_UDP_MODID
#define NP_UDP_DRV_NAME	CONFIG_STREAMS_NP_UDP_NAME
#define NP_UDP_CMAJORS	CONFIG_STREAMS_NP_UDP_NMAJORS
#define NP_UDP_CMAJOR_0	CONFIG_STREAMS_NP_UDP_MAJOR
#define NP_UDP_UNITS	CONFIG_STREAMS_NP_UDP_NMINORS
#endif				/* defined LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NP_UDP_MODID));
MODULE_ALIAS("streams-driver-np_udp");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NP_UDP_MAJOR));
MODULE_ALIAS("/dev/streams/np_udp");
MODULE_ALIAS("/dev/streams/np_udp/*");
MODULE_ALIAS("/dev/streams/clone/np_udp");
#endif				/* defined LFS */
MODULE_ALIAS("char-major-" __stringify(NP_UDP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(NP_UDP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(NP_UDP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/np_udp");
#endif				/* defined MODULE_ALIAS */
#endif				/* defined LINUX */

/*
 *  ==========================================================================
 *
 *  STREAMS Definitions
 *
 *  ==========================================================================
 */

#define DRV_ID		NP_UDP_DRV_ID
#define DRV_NAME	NP_UDP_DRV_NAME
#define CMAJORS		NP_UDP_CMAJORS
#define CMAJOR_0	NP_UDP_CMAJOR_0
#define UNITS		NP_UDP_UNITS
#ifdef MODULE
#define DRV_BANNER	NP_BANNER
#else				/* MODULE */
#define DRV_BANNER	NP_SPLASH
#endif				/* MODULE */

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),		/* Max packet size accepted */
	.mi_hiwat = (1 << 18),		/* Hi water mark */
	.mi_lowat = (1 << 16),		/* Lo water mark */
};

STATIC struct module_stat np_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat np_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* Upper multiplex is a N provider following the NPI. */

STATIC streamscall int np_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int np_qclose(queue_t *, int, cred_t *);

streamscall int np_rput(queue_t *, mblk_t *);
streamscall int np_rsrv(queue_t *);

STATIC struct qinit np_rinit = {
	.qi_putp = np_rput,		/* Read put procedure (message from below) */
	.qi_srvp = np_rsrv,		/* Read service procedure */
	.qi_qopen = np_qopen,		/* Each open */
	.qi_qclose = np_qclose,		/* Last close */
	.qi_minfo = &np_minfo,		/* Module information */
	.qi_mstat = &np_rstat,		/* Module statistics */
};

streamscall int np_wput(queue_t *, mblk_t *);
streamscall int np_wsrv(queue_t *);

STATIC struct qinit np_winit = {
	.qi_putp = np_wput,		/* Write put procedure (message from above) */
	.qi_srvp = np_wsrv,		/* Write service procedure */
	.qi_minfo = &np_minfo,		/* Module information */
	.qi_mstat = &np_wstat,		/* Module statistics */
};

MODULE_STATIC struct streamtab np_info = {
	.st_rdinit = &np_rinit,		/* Upper read queue */
	.st_wrinit = &np_winit,		/* Upper write queue */
};

#if !defined HAVE_KMEMB_STRUCT_SK_BUFF_TRANSPORT_HEADER
#if !defined HAVE_KFUNC_SKB_TRANSPORT_HEADER
static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}
static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}
static inline unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	return skb->h.raw;
}
static inline unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return skb->nh.raw;
}
static inline unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	return skb->mac.raw;
}
static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}
static inline void skb_reset_end_pointer(struct sk_buff *skb)
{
	skb->end = skb->data;
}
static inline void skb_reset_transport_header(struct sk_buff *skb)
{
	skb->h.raw = skb->data;
}
static inline void skb_reset_network_header(struct sk_buff *skb)
{
	skb->nh.raw = skb->data;
}
static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}
static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb_reset_tail_pointer(skb);
	skb->tail += offset;
}
static inline void skb_set_transport_header(struct sk_buff *skb, const int offset)
{
	skb_reset_transport_header(skb);
	skb->h.raw += offset;
}
static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb_reset_network_header(skb);
	skb->nh.raw += offset;
}
static inline void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb_reset_mac_header(skb);
	skb->mac.raw += offset;
}
#endif				/* !defined HAVE_KFUNC_SKB_TRANSPORT_HEADER */
#endif				/* !defined HAVE_KMEMB_STRUCT_SK_BUFF_TRANSPORT_HEADER */

/*
 *  Primary data structures.
 */

struct np_bhash_bucket;
struct np_chash_bucket;

struct np_daddr {
	uint32_t addr;			/* IP address this destination */
	unsigned char ttl;		/* time to live, this destination */
	unsigned char tos;		/* type of service, this destination */
	unsigned short mtu;		/* maximum transfer unit this destination */
	struct dst_entry *dst;		/* route for this destination */
};

struct np_saddr {
	uint32_t addr;			/* IP address this source */
};

struct np_baddr {
	uint32_t addr;			/* IP address this bind */
};

/* Private structure */
typedef struct np {
	STR_DECLARATION (struct np);	/* Stream declaration */
	struct np *bnext;		/* linkage for bind/list hash */
	struct np **bprev;		/* linkage for bind/list hash */
	struct np_bhash_bucket *bhash;	/* linkage for bind/list hash */
	struct np *cnext;		/* linkage for conn hash */
	struct np **cprev;		/* linkage for conn hash */
	struct np_chash_bucket *chash;	/* linkage for conn hash */
	N_info_ack_t info;		/* service provider information */
	unsigned int sndblk;		/* sending blocked */
	unsigned int sndmem;		/* send buffer memory allocated */
	unsigned int rcvmem;		/* recv buffer memory allocated */
	unsigned int BIND_flags;	/* bind flags */
	unsigned int CONN_flags;	/* connect flags */
	unsigned int CONIND_number;	/* maximum number of outstanding connection indications */
	bufq_t conq;			/* connection indication queue */
	bufq_t datq;			/* data indication queue */
	bufq_t resq;			/* reset indication queue */
	unsigned short pnum;		/* number of bound protocol ids */
	uint8_t protoids[16];		/* bound protocol ids */
	unsigned short bnum;		/* number of bound addresses */
	unsigned short bport;		/* bound port number (network order) */
	struct np_baddr baddrs[8];	/* bound addresses */
	unsigned short snum;		/* number of source (connected) addresses */
	unsigned short sport;		/* source (connected) port number (network order) */
	struct np_saddr saddrs[8];	/* source (connected) addresses */
	unsigned short dnum;		/* number of destination (connected) addresses */
	unsigned short dport;		/* destination (connected) port number (network order) */
	struct np_daddr daddrs[8];	/* destination (connected) addresses */
	struct N_qos_sel_info_udp qos;	/* network service provider quality of service */
	struct N_qos_range_info_udp qor;	/* network service provider quality of service
						   range */
} np_t;

#define PRIV(__q) (((__q)->q_ptr))
#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (np, np);		/* master list of np (open) structures */
} df_t;

static struct df master = {.lock = RW_LOCK_UNLOCKED, };

/*
 *  Bind buckets, caches and hashes.
 */
struct np_bind_bucket {
	struct np_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct np_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short port;		/* port number (host order) */
	struct np *owners;		/* list of owners of this protocol/port combination */
	struct np *dflt;		/* default listeners/destinations for this protocol */
};
struct np_conn_bucket {
	struct np_conn_bucket *next;	/* linkage of conn buckets for hash slot */
	struct np_conn_bucket **prev;	/* linkage of conn buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short sport;		/* source port number (network order) */
	unsigned short dport;		/* destination port number (network order) */
	struct np *owners;		/* list of owners of this protocol/sport/dport combination */
};

struct np_bhash_bucket {
	rwlock_t lock;
	struct np *list;
};
struct np_chash_bucket {
	rwlock_t lock;
	struct np *list;
};

STATIC struct np_bhash_bucket *np_bhash;
STATIC size_t np_bhash_size = 0;
STATIC size_t np_bhash_order = 0;

STATIC struct np_chash_bucket *np_chash;
STATIC size_t np_chash_size = 0;
STATIC size_t np_chash_order = 0;

STATIC inline fastcall __hot_in int
np_bhashfn(unsigned char proto, unsigned short bport)
{
	return ((np_bhash_size - 1) & (proto + bport));
}

STATIC INLINE fastcall __unlikely int
np_chashfn(unsigned char proto, unsigned short sport, unsigned short dport)
{
	return ((np_chash_size - 1) & (proto + sport + dport));
}

#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
#define mynet_protocol net_protocol
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
#define mynet_protocol inet_protocol
#endif				/* defined HAVE_KTYPE_STRUCT_INET_PROTOCOL */

struct ipnet_protocol {
	struct mynet_protocol proto;
	struct mynet_protocol *next;
	struct module *kmod;
};

struct np_prot_bucket {
	unsigned char proto;		/* protocol number */
	int refs;			/* reference count */
	int corefs;			/* N_CONS references */
	int clrefs;			/* N_CLNS references */
	struct ipnet_protocol prot;	/* Linux registration structure */
};
STATIC rwlock_t np_prot_lock = RW_LOCK_UNLOCKED;
STATIC struct np_prot_bucket *np_prots[256];

STATIC kmem_cachep_t np_udp_prot_cachep;
STATIC kmem_cachep_t np_udp_priv_cachep;

static INLINE struct np *
np_get(struct np *np)
{
	dassert(np != NULL);
	atomic_inc(&np->refcnt);
	return (np);
}
static INLINE __hot void
np_put(struct np *np)
{
	dassert(np != NULL);
	if (atomic_dec_and_test(&np->refcnt)) {
		kmem_cache_free(udp_priv_cachep, np);
	}
}
static INLINE fastcall __hot void
np_release(struct np **npp)
{
	struct np *np;

	dassert(npp != NULL);
	if (likely((np = XCHG(npp, NULL)) != NULL))
		np_put(np);
}
static INLINE struct np *
np_alloc(void)
{
	struct np *np;

	if ((np = kmem_cache_alloc(udp_priv_cachep, GFP_ATOMIC))) {
		bzero(np, sizeof(*np));
		atomic_set(&np->refcnt, 1);
		spin_lock_init(&np->lock);	/* "np-lock" */
		np->priv_put = &np_put;
		np->priv_get = &np_get;
		// np->type = 0;
		// np->id = 0;
		// np->state = 0;
		np->flags = 0;
	}
	return (np);
}

/*
 *  Locking
 */

/* Must always be bottom-half versions to avoid lock badness.  But give these
 * different names to avoid conflict with generic definitions.  */

//#if defined CONFIG_STREAMS_NOIRQ || defined _TEST
#if 0

#define spin_lock_str2(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str2(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)
#define write_lock_str2(__lkp, __flags) \
	do { (void)__flags; write_lock_bh(__lkp); } while (0)
#define write_unlock_str2(__lkp, __flags) \
	do { (void)__flags; write_unlock_bh(__lkp); } while (0)
#define read_lock_str2(__lkp, __flags) \
	do { (void)__flags; read_lock_bh(__lkp); } while (0)
#define read_unlock_str2(__lkp, __flags) \
	do { (void)__flags; read_unlock_bh(__lkp); } while (0)
#define local_save_str2(__flags) \
	do { (void)__flags; local_bh_disable(); } while (0)
#define local_restore_str2(__flags) \
	do { (void)__flags; local_bh_enable(); } while (0)

#else

#define spin_lock_str2(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str2(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)
#define write_lock_str2(__lkp, __flags) \
	write_lock_irqsave(__lkp, __flags)
#define write_unlock_str2(__lkp, __flags) \
	write_unlock_irqrestore(__lkp, __flags)
#define read_lock_str2(__lkp, __flags) \
	read_lock_irqsave(__lkp, __flags)
#define read_unlock_str2(__lkp, __flags) \
	read_unlock_irqrestore(__lkp, __flags)
#define local_save_str2(__flags) \
	local_irq_save(__flags)
#define local_restore_str2(__flags) \
	local_irq_restore(__flags)

#endif

#ifdef LIS
#ifndef noinline
#define noinline
#endif
#ifndef fastcall
#define fastcall
#endif
#ifndef __unlikely
#define __unlikely
#endif
#endif

/*
 *  Buffer allocation
 */

STATIC streamscall __unlikely void
np_bufsrv(long data)
{
	str_t *s;
	queue_t *q;

	q = (queue_t *) data;
	ensure(q, return);
	s = STR_PRIV(q);
	ensure(s, return);

	if (q == s->iq) {
		if (xchg(&s->ibid, 0) != 0)
			atomic_dec(&s->refcnt);
		qenable(q);
		return;
	}
	if (q == s->oq) {
		if (xchg(&s->obid, 0) != 0)
			atomic_dec(&s->refcnt);
		qenable(q);
		return;
	}
	return;
}

noinline fastcall __unlikely void
np_unbufcall(str_t * s)
{
	bufcall_id_t bid;

	if ((bid = xchg(&s->ibid, 0))) {
		unbufcall(bid);
		atomic_dec(&s->refcnt);
	}
	if ((bid = xchg(&s->obid, 0))) {
		unbufcall(bid);
		atomic_dec(&s->refcnt);
	}
}

noinline fastcall __unlikely void
np_bufcall(queue_t *q, size_t size, int prior)
{
	if (q) {
		str_t *s = STR_PRIV(q);
		bufcall_id_t bid, *bidp = NULL;

		if (q == s->iq)
			bidp = &s->ibid;
		if (q == s->oq)
			bidp = &s->obid;

		if (bidp) {
			atomic_inc(&s->refcnt);
			if ((bid = xchg(bidp, bufcall(size, prior, &np_bufsrv, (long) q)))) {
				unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
				atomic_dec(&s->refcnt);
			}
			return;
		}
	}
	swerr();
	return;
}

STATIC INLINE fastcall __unlikely mblk_t *
np_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;

	if (likely((mp = allocb(size, prior)) != NULL))
		return (mp);
	rare();
	np_bufcall(q, size, prior);
	return (mp);
}

STATIC INLINE fastcall __unlikely mblk_t *
np_dupmsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (likely((mp = dupmsg(bp)) != NULL))
		return (mp);
	rare();
	np_bufcall(q, msgsize(bp), BPRI_MED);
	return (mp);
}

/*
 *  =========================================================================
 *
 *  State Changes
 *
 *  =========================================================================
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

/* State functions */

STATIC INLINE fastcall __unlikely void
np_set_state(struct np *np, const np_ulong state)
{
	_printd(("%s: %p: %s <- %s\n", DRV_NAME, np, np_state_name(state),
		 np_state_name(np->info.CURRENT_state)));
	np->info.CURRENT_state = state;
}

STATIC INLINE fastcall __unlikely np_ulong
np_get_state(const struct np *np)
{
	return (np->info.CURRENT_state);
}

STATIC INLINE fastcall __unlikely np_ulong
np_chk_state(const struct np *np, const np_ulong mask)
{
	return (((1 << np->info.CURRENT_state) & (mask)) != 0);
}

STATIC INLINE fastcall __unlikely np_ulong
np_not_state(const struct np *np, const np_ulong mask)
{
	return (((1 << np->info.CURRENT_state) & (mask)) == 0);
}

STATIC INLINE fastcall __unlikely long
np_get_statef(const struct np *np)
{
	return (1 << np_get_state(np));
}

/*
 *  =========================================================================
 *
 *  IP Local Management
 *
 *  =========================================================================
 */

STATIC int np_v4_rcv(struct sk_buff *skb);
STATIC void np_v4_err(struct sk_buff *skb, u32 info);

/*
 *  IP subsystem management
 */
#ifdef LINUX
/**
 * np_v4_rcv_next - pass a socket buffer to the next handler
 * @skb: socket buffer to pass
 *
 * In the Linux packet handler, if the packet is not for us, pass it to the next handler.  If there
 * is no next handler, free the packet and return.  Note that we do not have to lock the hash
 * because we own it and are also holding a reference to any module owning the next handler.  This
 * function returns zero (0) if the packet has not or will not be seen by another packet handler,
 * and one (1) if the packet has or will be seen by another packet handler.  This return value is
 * used to determine whether to generate ICMP errors or not.
 */
STATIC INLINE fastcall __hot_in int
np_v4_rcv_next(struct sk_buff *skb)
{
	struct np_prot_bucket *pb;
	struct mynet_protocol *pp;
	struct iphdr *iph;
	unsigned char proto;

	iph = (typeof(iph)) skb_network_header(skb);
	proto = iph->protocol;
	if ((pb = np_prots[proto]) && (pp = pb->prot.next)) {
		pp->handler(skb);
		return (1);
	}
	kfree_skb(skb);
	return (0);
}

/**
 * np_v4_err_next - pass a socket buffer to the next error handler
 * @skb: socket buffer to pass
 *
 * In the Linux packet error handler, if the packet is not for us, pass it to the next error
 * handler.  If there is no next error handler, simply return.
 */
STATIC INLINE fastcall __hot_in void
np_v4_err_next(struct sk_buff *skb, __u32 info)
{
	struct np_prot_bucket *pb;
	struct mynet_protocol *pp;
	unsigned char proto;

	proto = ((struct iphdr *) skb->data)->protocol;
	if ((pb = np_prots[proto]) && (pp = pb->prot.next))
		pp->err_handler(skb, info);
	return;
}

#ifndef CONFIG_SMP
#define net_protocol_lock() local_bh_disable()
#define net_protocol_unlock() local_bh_enable()
#else				/* CONFIG_SMP */
#ifdef HAVE_INET_PROTO_LOCK_ADDR
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;

#define net_protocol_lock() spin_lock_bh(inet_proto_lockp)
#define net_protocol_unlock() spin_unlock_bh(inet_proto_lockp)
#else
#define net_protocol_lock() br_write_lock_bh(BR_NETPROTO_LOCK)
#define net_protocol_unlock() br_write_unlock_bh(BR_NETPROTO_LOCK)
#endif
#endif				/* CONFIG_SMP */
#ifdef HAVE_INET_PROTOS_ADDR
STATIC struct mynet_protocol **inet_protosp = (typeof(inet_protosp)) HAVE_INET_PROTOS_ADDR;
#endif

#ifdef HAVE_MODULE_TEXT_ADDRESS_ADDR
#define module_text_address(__arg) ((typeof(&module_text_address))HAVE_MODULE_TEXT_ADDRESS_ADDR)((__arg))
#endif

/**
 * np_init_nproto - initialize network protocol override
 * @proto: the protocol to register or override
 *
 * This is the network protocol override function.
 *
 * This is complicated because we hack the inet protocol tables.  If no other protocol was
 * previously registered, this reduces to inet_add_protocol().  If there is a protocol previously
 * registered, we take a reference on the kernel module owning the entry, if possible, and replace
 * the entry with our own, saving a pointer to the previous entry for passing sk_bufs along that we
 * are not interested in.  Taking a module reference is particularly for things like SCTP, where
 * unloading the module after protocol override would break things horribly.  Taking the reference
 * keeps the module from unloading (this works for OpenSS7 SCTP as well as lksctp).
 */
STATIC INLINE fastcall __unlikely struct np_prot_bucket *
np_init_nproto(unsigned char proto, unsigned int type)
{
	struct np_prot_bucket *pb;
	struct ipnet_protocol *pp;
	struct mynet_protocol **ppp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	write_lock_bh(&np_prot_lock);
	if ((pb = np_prots[proto]) != NULL) {
		pb->refs++;
		switch (type) {
		case N_CONS:
			++pb->corefs;
			break;
		case N_CLNS:
			++pb->clrefs;
			break;
		default:
			swerr();
			break;
		}
	} else if ((pb = kmem_cache_alloc(np_udp_prot_cachep, GFP_ATOMIC))) {
		bzero(pb, sizeof(*pb));
		pb->refs = 1;
		switch (type) {
		case N_CONS:
			pb->corefs = 1;
			break;
		case N_CLNS:
			pb->clrefs = 1;
			break;
		default:
			swerr();
			break;
		}
		pp = &pb->prot;
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
		pp->proto.protocol = proto;
		pp->proto.name = "streams-np_udp";
#endif
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO
		pp->proto.proto = proto;
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO */
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
		pp->proto.no_policy = 1;
#endif
		pp->proto.handler = &np_v4_rcv;
		pp->proto.err_handler = &np_v4_err;
		ppp = &inet_protosp[hash];

		{
			net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
			while (*ppp && (*ppp)->protocol != proto)
				ppp = &(*ppp)->next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
			if (*ppp != NULL) {
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
				/* can only override last entry */
				if ((*ppp)->copy != 0) {
					__ptrace(("Cannot override copy entry\n"));
					net_protocol_unlock();
					write_unlock_bh(&np_prot_lock);
					kmem_cache_free(np_udp_prot_cachep, pb);
					return (NULL);
				}
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY */
#ifdef HAVE_MODULE_TEXT_ADDRESS_ADDR
				if ((pp->kmod = module_text_address((ulong) *ppp))
				    && pp->kmod != THIS_MODULE) {
					if (!try_module_get(pp->kmod)) {
						__ptrace(("Cannot acquire module\n"));
						net_protocol_unlock();
						write_unlock_bh(&np_prot_lock);
						kmem_cache_free(np_udp_prot_cachep, pb);
						return (NULL);
					}
				}
#endif				/* HAVE_MODULE_TEXT_ADDRESS_ADDR */
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NEXT || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NEXT
				pp->proto.next = (*ppp)->next;
#endif
			}
			pp->next = (*ppp);
			*ppp = &pp->proto;
			net_protocol_unlock();
		}
		/* link into hash slot */
		np_prots[proto] = pb;
	}
	write_unlock_bh(&np_prot_lock);
	return (pb);
}

/**
 * np_term_nproto - terminate network protocol override
 * @proto: network protocol to terminate
 *
 * This is the network protocol restoration function.
 *
 * This is complicated and brittle.  The module stuff here is just for ourselves (other kernel
 * modules pulling the same trick) as Linux IP protocols are normally kernel resident.  If a
 * protocol was previously registered, restore the protocol's entry and drop the reference to its
 * owning kernel module.  If there was no protocol previously registered, this reduces to
 * inet_del_protocol().
 */
STATIC INLINE fastcall __unlikely void
np_term_nproto(unsigned char proto, unsigned int type)
{
	struct np_prot_bucket *pb;

	write_lock_bh(&np_prot_lock);
	if ((pb = np_prots[proto]) != NULL) {
		switch (type) {
		case N_CONS:
			assure(pb->corefs > 0);
			--pb->corefs;
			break;
		case N_CLNS:
			assure(pb->clrefs > 0);
			--pb->clrefs;
			break;
		default:
			swerr();
			break;
		}
		if (--pb->refs == 0) {
			struct ipnet_protocol *pp = &pb->prot;
			struct mynet_protocol **ppp;
			int hash = proto & (MAX_INET_PROTOS - 1);

			ppp = &inet_protosp[hash];
			{
				net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
				while (*ppp && *ppp != &pp->proto)
					ppp = &(*ppp)->next;
				if (pp->next)
					pp->next->next = pp->proto.next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
				__assert(*ppp == &pp->proto);
				*ppp = pp->next;
				net_protocol_unlock();
			}
#ifdef HAVE_MODULE_TEXT_ADDRESS_ADDR
			if (pp->next != NULL && pp->kmod != NULL && pp->kmod != THIS_MODULE)
				module_put(pp->kmod);
#endif				/* HAVE_MODULE_TEXT_ADDRESS_ADDR */
			/* unlink from hash slot */
			np_prots[proto] = NULL;

			kmem_cache_free(np_udp_prot_cachep, pb);
		}
	}
	write_unlock_bh(&np_prot_lock);
}
#endif				/* LINUX */

/**
 *  np_bind_prot -  bind a protocol
 *  @proto:	    protocol number to bind
 *
 *  NOTICES: Notes about registration.  Older 2.4 kernels will allow you to register whatever inet
 *  protocols you want on top of any existing protocol.  This is good.  2.6 kernels, on the other
 *  hand, do not allow registration of inet protocols over existing inet protocols.  We rip symbols
 *  on 2.6 and put special code in the handler to give us effectively the old 2.4 approach.
 *  This is also detectable by the fact that inet_add_protocol() returns void on 2.4 and int on 2.6.
 *
 *  Issues with the 2.4 approach to registration is that the ip_input function passes a cloned skb
 *  to each protocol registered.  We don't want to do that.  If the message is for us, we want to
 *  process it without passing it to others.
 *
 *  Issues with the 2.6 approach to registration is that the ip_input function passes the skb to
 *  only one function.  We don't want that either.  If the message is not for us, we want to pass it
 *  to the next protocol module.
 */
STATIC INLINE fastcall __unlikely int
np_bind_prot(unsigned char proto, unsigned int type)
{
	struct np_prot_bucket *pb;

	if ((pb = np_init_nproto(proto, type)))
		return (0);
	return (-ENOMEM);
}

/**
 *  np_unbind_prot - unbind a protocol
 *  @proto:	    protocol number to unbind
 */
STATIC INLINE fastcall __unlikely void
np_unbind_prot(unsigned char proto, unsigned int type)
{
	np_term_nproto(proto, type);
}

/**
 * np_bind - bind a Stream to an NSAP
 * @np: private structure
 * @PROTOID_buffer: protocol ids to bind
 * @PROTOID_length; length of protocol ids
 * @ADDR_buffer: addresses to bind
 * @ADDR_length: length of addresses
 * @CONIND_number: maximum number of connection indications
 * @BIND_flags: bind flags
 *
 * Bind to protocol ids and port and addresses.  We currently only bind the first protocol id
 * regardless of the number specified.  Binding supports wildcard addresses, both in port number and
 * IP address.  The bind hash contains bind buckets that list Streams that are bound to the same
 * protocol id and port number.
 *
 * Assign a port number and bind to it, or bind to the selected port.
 *
 * Linux UDP uses a specific port range for dynamic assignment of port numbers.  We take the same
 * approach here, however, we need to use a range that does not overlap with the Linux UDP range.
 *
 * TODO: also check Linux UDP bind hashes.  If the port is specifically in the Linux UDP dynamic
 * port assignment range, we should just refuse it.  This is because Linux UDP believes that it owns
 * this range and will assign within this range without the user having any control.  If outside
 * this range, it might be an idea to check the Linux UDP hashes to make sure that noone else is
 * using this port at the moment.  It does not keep Linux UDP from assigning it later, resulting in
 * a conflict.
 *
 */
STATIC INLINE fastcall __unlikely int
np_bind(struct np *np, unsigned char *PROTOID_buffer, size_t PROTOID_length,
	struct sockaddr_in *ADDR_buffer, const socklen_t ADDR_length,
	const np_ulong CONIND_number, const np_ulong BIND_flags)
{
	static unsigned short np_prev_port = 61000;
	static const unsigned short np_frst_port = 61000;
	static const unsigned short tp_last_port = 65000;

	struct np_bhash_bucket *hp;
	unsigned short bport = ADDR_buffer[0].sin_port;
	unsigned char proto = PROTOID_buffer[0];
	size_t anum = ADDR_length / sizeof(*ADDR_buffer);
	struct np *np2;
	int i, j, err;
	unsigned short num = 0;
	unsigned long flags;

	PROTOID_length = 1;
	if (bport == 0) {
		num = tp_prev_port;	/* UNSAFE */
	      try_again:
		bport = htons(num);
	}
	hp = &np_bhash[np_bhashfn(proto, bport)];
	write_lock_str2(&hp->lock, flags);
	for (np2 = hp->list; np2; np2 = np2->bnext) {
		if (proto != np2->protoids[0])
			continue;
		if (bport != np2->bport)
			continue;
#if 1
		/* Allowed to bind to each NSAP once as DEFAULT_DEST, once as DEFAULT_LISTENER and
		   once as neither. */
		if ((BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)) !=
		    (np2->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
			continue;
#endif
		for (i = 0; i < np2->bnum; i++) {
#if 0
			if (np2->baddrs[i].addr == 0)
				break;
#endif
			for (j = 0; j < anum; j++)
				if (np2->baddrs[i].addr == ADDR_buffer[j].sin_addr.s_addr)
					break;
			if (j < anum)
				break;
		}
		if (i < np2->bnum)
			break;
	}
	if (np2 != NULL) {
		if (num == 0) {
			/* specific port number requested */
			write_unlock_str2(&hp->lock, flags);
			/* There is a question as to which error should be returned when a protocol
			   address is already bound.

			   NPI 2.0.0 says that "[i]f the NS provider cannot bind the specified
			   address, it may assign another network address to the user.  It is the
			   network user's responsibility to check the network address returned in
			   the N_BIND_ACK primitive to see if it is the same as the one requested."

			   NPI 2.0.0 says "[o]nly one default listener Stream is allowed per
			   occurrence of NPI.  An attempt to bind a default listener Stream when one 
			   is already bound should result in an error (of type NBOUND)" and "[o]nly
			   one default destination stream per NSAP is alloed per occurence of NPI.
			   An attempt to bind a default destination stream to an NSAP when one is
			   already bound should result in an error of type NBOUND." But aslo,
			   "NBOUND: The NS user attempted to bind a second Stream to a network
			   address with the CONIND_number set to a non-zero value, or attempted to
			   bind a second Stream with the DEFAULT_LISTENER flag value set to
			   non-zero."

			   However, we return NBOUND for a connectionless bind when an attempt is
			   made to bind a second address to the same NSAP where either both have the 
			   DEFAULT_DEST flag set or both have the DEFAULT_DEST flag clear. */
			return (NBOUND);
		} else {
			write_unlock_str2(&hp->lock, flags);
			if (++num > np_last_port)
				num = np_frst_port;
			if (num != np_prev_port)
				goto try_again;
			return (NNOADDR);
		}
	}
	if ((err = np_bind_prot(proto, np->info.SERV_type))) {
		write_unlock_str2(&hp->lock, flags);
		return (err);
	}
	if (num != 0)
		np_prev_port = num;
	ADDR_buffer[0].sin_port = bport;
	if ((np_long) np->qos.protocol == QOS_UNKNOWN)
		np->qos.protocol = proto;
	if ((np->bnext = hp->list))
		np->bnext->bprev = &np->bnext;
	np->bprev = &hp->list;
	hp->list = np_get(np);
	np->bhash = hp;
	/* copy into private structure */
	np->CONIND_number = CONIND_number;
	np->BIND_flags = BIND_flags;
	np->pnum = PROTOID_length;
	for (i = 0; i < PROTOID_length; i++)
		np->protoids[i] = PROTOID_buffer[i];
	np->bnum = anum;
	np->bport = bport;
	ptrace(("%s: %s: bound to proto = %d, bport = %d\n", DRV_NAME, __FUNCTION__,
		(int) proto, (int) ntohs(bport)));
	for (i = 0; i < anum; i++)
		np->baddrs[i].addr = ADDR_buffer[i].sin_addr.s_addr;
	write_unlock_str2(&hp->lock, flags);
#if defined HAVE_KFUNC_SYNCHRONIZE_NET
	synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KFUNC_SYNCHRONIZE_NET */
	return (0);
}

#if defined HAVE_KFUNC_DST_OUTPUT
STATIC INLINE __hot_out int
np_udp_queue_xmit(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct iphdr *iph = (typeof(iph)) skb_network_header(skb);

#if defined NETIF_F_TSO
	ip_select_ident_more(iph, dst, NULL, 0);
#else				/* !defined NETIF_F_TSO */
	ip_select_ident(iph, dst, NULL);
#endif				/* defined NETIF_F_TSO */
	ip_send_check(iph);
#ifndef NF_IP_LOCAL_OUT
#define NF_IP_LOCAL_OUT NF_INET_LOCAL_OUT
#endif
#if defined HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dst->dev, ip_dst_output);
#else				/* !defined HAVE_KFUNC_IP_DST_OUTPUT */
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dst->dev, dst_output);
#endif				/* defined HAVE_KFUNC_IP_DST_OUTPUT */
}
#else				/* !defined HAVE_KFUNC_DST_OUTPUT */
STATIC INLINE __hot_out int
np_udp_queue_xmit(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct iphdr *iph = skb->nh.iph;

	if (skb->len > dst_pmtu(dst)) {
		rare();
		return ip_fragment(skb, dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return dst->output(skb);
	}
}
#endif				/* defined HAVE_KFUNC_DST_OUTPUT */

#if 0
noinline fastcall void
np_skb_destructor_slow(struct np *np, struct sk_buff *skb)
{
	unsigned long flags;

	spin_lock_str2(&np->qlock, flags);
	// ensure(np->sndmem >= skb->truesize, np->sndmem = skb->truesize);
	np->sndmem -= skb->truesize;
	if (unlikely((np->sndmem < np->options.xti.sndlowat || np->sndmem == 0))) {
		np->sndblk = 0;	/* no longer blocked */
		spin_unlock_str2(&np->qlock, flags);
		if (np->iq != NULL && np->iq->q_first != NULL)
			qenable(np->iq);
	} else {
		spin_unlock_str2(&np->qlock, flags);
	}
#if 0				/* destructor is nulled by skb_orphan */
	skb_shinfo(skb)->frags[0].page = NULL;
	skb->destructor = NULL;
#endif
	np_put(np);
	return;
}

/**
 * np_skb_destructor - socket buffer destructor
 * @skb: socket buffer to destroy
 *
 * This provides the impedance matching between socket buffer flow control and STREAMS flow control.
 * When np->sndmem is greater than np->options.xti.sndbuf we place STREAMS buffers back on the send
 * queue and stall the queue.  When the send memory falls below np->options.xti.sndlowat (or to
 * zero) and there are message on the send queue, we enable the queue.
 *
 * NOTE: There was not enough hysteresis in this function!  It was qenabling too fast.  We need a
 * flag in the private structure that indicates that the queue is stalled awaiting subsiding below
 * the send low water mark (or to zero) that is set when we stall the queue and reset when we fall
 * beneath the low water mark.
 */
STATIC __hot_out void
np_skb_destructor(struct sk_buff *skb)
{
	struct np *np;
	unsigned long flags;

	np = (typeof(np)) skb_shinfo(skb)->frags[0].page;
	dassert(np != NULL);
	if (likely(np->sndblk == 0)) {
		/* technically we could have multiple processors freeing sk_buffs at the same time */
		spin_lock_str2(&np->qlock, flags);
		// ensure(np->sndmem >= skb->truesize, np->sndmem = skb->truesize);
		np->sndmem -= skb->truesize;
		spin_unlock_str2(&np->qlock, flags);
#if 0				/* destructor is nulled by skb_orphan */
		skb_shinfo(skb)->frags[0].page = NULL;
		skb->destructor = NULL;
#endif
		np_put(np);
		return;
	}
	np_skb_destructor_slow(np, skb);
}
#endif

#undef skbuff_head_cache
#ifdef HAVE_SKBUFF_HEAD_CACHE_ADDR
#define skbuff_head_cache (*((kmem_cachep_t *) HAVE_SKBUFF_HEAD_CACHE_ADDR))
#endif

/**
 * np_alloc_skb_slow - allocate a socket buffer from a message block
 * @np: private pointer
 * @mp: the message block
 * @headroom: header room for resulting sk_buff
 * @gfp: general fault protection
 *
 * This is the old slow way of allocating a socket buffer.  We simple allocate a socket buffer with
 * sufficient head room and copy the data from the message block(s) to the socket buffer.  This is
 * slow.  This is the only way that LiS can do things (because it has unworkable message block
 * allocation).
 */
noinline fastcall __unlikely struct sk_buff *
np_alloc_skb_slow(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;
	unsigned int dlen = msgsize(mp);

#if 0
	unsigned long flags;
#endif

	if (likely((skb = alloc_skb(headroom + dlen, GFP_ATOMIC)) != NULL)) {
		skb_reserve(skb, headroom);
		{
			unsigned char *data;
			mblk_t *b;
			int blen;

			data = skb_put(skb, dlen);
			for (b = mp; b; b = b->b_cont) {
				if ((blen = b->b_wptr - b->b_rptr) > 0) {
					bcopy(b->b_rptr, data, blen);
					data += blen;
					__assert(data <= skb_tail_pointer(skb));
				} else
					rare();
			}
		}
		freemsg(mp);	/* must absorb */
#if 0
		/* we never have any page fragments, so we can steal a pointer from the page
		   fragement list. */
		assert(skb_shinfo(skb)->nr_frags == 0);
		skb_shinfo(skb)->frags[0].page = (struct page *) np_get(np);
		skb->destructor = np_skb_destructor;
		spin_lock_str2(&np->qlock, flags);
		np->sndmem += skb->truesize;
		spin_unlock_str2(&np->qlock, flags);
#if 0
		/* keep track of high water mark */
		if (tp_wstat.ms_acnt < tp->sndmem)
			tp_wstat.ms_acnt = tp->sndmem;
#endif
#endif
	}
	return (skb);
}

/**
 * np_alloc_skb_old - allocate a socket buffer from a message block
 * @np: private pointer
 * @mp: the message block
 * @headroom: header room for resulting sk_buff
 * @gfp: general fault protection
 *
 * Description: this function is used for zero-copy allocation of a socket buffer from a message
 * block.  The socket buffer contains all of the data in the message block including any head or
 * tail room (db_base to db_lim).  The data portion of the socket buffer contains the data
 * referenced by the message block (b_rptr to b_wptr).  Because there is no socket buffer destructor
 * capable of freeing the message block, we steal the kmem_alloc'ed buffer from the message and
 * attach it tot he socket buffer header.  The reference to the message block is consumed unless the
 * function returns NULL.
 *
 * A problem exists in converting mblks to sk_buffs (although visa versa is easy):  sk_buffs put a
 * hidden shared buffer structure at the end of the buffer (where it is easily overwritten on buffer
 * overflows).  There is not necessarily enough room at the end of the mblk to add this structure.
 * There are several things that I added to the Stream head to help with this:
 *
 * 1. A SO_WRPAD option to M_SETOPTS that will specify how much room to leave after the last SMP
 *    cache line in the buffer.
 *
 * 2. Three flags, SO_NOCSUM, SO_CSUM, SO_CRC32C were added to the Stream head so that the stream
 *    can support partial checksum while copying from the user.
 *
 * 3. db_lim is now always set to the end of the actual allocation rather than the end of the
 *    requested allocation.  Linux kmalloc() allocates from 2^n size memory caches that are
 *    always SMP cache line aligned.
 *
 * With these options in play, the size of the buffer should have sufficient room for the shared
 * buffer structure.  If, however, the data block was not delivered by the Stream head (but an
 * intermediate module) or has been modified (by an intermediate module) the tail room might not be
 * available.  Instead of copying the entire buffer which would be quite memory intensive, in this
 * case we allocate a new buffer and copy only the portion of the original buffer necessary to make
 * room for the shared buffer structure.
 *
 * The same is true for the IP header portion.  Using SO_WROFF it is possible to reserve sufficient
 * room for the hardware header, IP header and UDP header.  Message blocks should normally already
 * contain this headroom.  However, again, it might be possible that the message block originated at
 * an intermediate module or was modified by an intermediate module unaware of this policy.  If
 * there is insufficient headroom, again we allocate a new message block large enough to contain the
 * header and make two sk_buffs, one for the header and one for the payload.
 *
 * As a result, we might wind up with three socket buffers: one containing the headroom for the hard
 * header, IP header and UDP header; one containing most of the data payload; and one containing the
 * last fragment of the payload smaller than or equal to sizeof(struct skb_shared_info).  All but
 * the initial socket buffer are placed in the frag_list of the first socket buffer.  Note that only
 * the header need be completed.  If checksum has not yet been performed, it is necessary to walk
 * through the data to generate the checksum.
 */
#if defined LFS
noinline fastcall __unlikely struct sk_buff *
np_alloc_skb_old(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;
	unsigned char *beg, *end;

#if 0
	unsigned long flags;
#endif

#if 0
	struct sk_buff *skb_head = NULL, *skb_tail = NULL;
#endif

	/* must not be a fastbuf */
	if (unlikely(mp->b_datap->db_size <= FASTBUF))
		goto go_slow;
	/* must not be esballoc'ed */
	if (unlikely(mp->b_datap->db_frtnp != NULL))
		goto go_slow;
	/* must be only reference (for now) */
	if (unlikely(mp->b_datap->db_ref > 1))
		goto go_slow;

	beg = mp->b_rptr - headroom;
	/* First, check if there is enough head room in the data block. */
	if (unlikely(beg < mp->b_datap->db_base)) {
#if 0
		/* No, there is not enough headroom, allocate an sk_buff for the header. */
		skb_head = alloc_skb(headroom, gfp);
		if (unlikely(skb_head == NULL))
			goto no_head;
		skb_reserve(skb_head, headroom);
		beg = mp->b_rptr;
#else
		goto go_frag;
#endif
	}
	/* Next, check if there is enough tail room in the data block. */
	end = (unsigned char *) (((unsigned long) mp->b_wptr + (SMP_CACHE_BYTES - 1)) &
				 ~(SMP_CACHE_BYTES - 1));
	if (unlikely(end + sizeof(struct skb_shared_info) > mp->b_datap->db_lim)) {
#if 0
		/* No, there is not enough tailroom, allocate an sk_buff for the tail. */
		skb_tail = alloc_skb(SMP_CACHE_BYTES + sizeof(struct skb_shared_info), gfp);
		if (unlikely(skb_tail == NULL))
			goto no_tail;
		{
			unsigned int len;

			end = (unsigned char *) (((unsigned long) mp->b_datap->db_lim -
						  sizeof(struct skb_shared_info)) &
						 ~(SMP_CACHE_BYTES - 1));
			len = mp->b_wptr - end;
			bcopy(end, skb_put(skb_tail, len), len);
			mp->b_wptr = end;
		}
#else
		goto go_frag;
#endif
	}

	/* Last, allocate a socket buffer header and point it to the payload data. */
	skb = kmem_cache_alloc(skbuff_head_cache, gfp);
	if (unlikely(skb == NULL))
		goto no_skb;

	memset(skb, 0, offsetof(struct sk_buff, truesize));
	skb->truesize = end - beg + sizeof(struct sk_buff);
	atomic_set(&skb->users, 1);
	skb->head = mp->b_datap->db_base;
	skb->data = mp->b_rptr;
	skb_set_tail_pointer(skb, mp->b_wptr - mp->b_rptr);
#if defined NET_SKBUFF_DATA_USES_OFFSET
	skb->end = end - skb->head;
#else				/* defined NET_SKBUFF_DATA_USES_OFFSET */
	skb->end = end;
#endif				/* defined NET_SKBUFF_DATA_USES_OFFSET */
	skb->len = mp->b_wptr - mp->b_rptr;
	skb->cloned = 0;
	skb->data_len = 0;
	/* initialize shared data structure */
	memset(skb_shinfo(skb), 0, sizeof(struct skb_shared_info));
	atomic_set(&(skb_shinfo(skb)->dataref), 1);

	/* need to release message block and data block without releasing buffer */

	/* point into internal buffer */
	mp->b_datap->db_frtnp = (struct free_rtn *)
	    ((struct mdbblock *) ((struct mbinfo *) mp->b_datap - 1))->databuf;
	/* override with dummy free routine */
	mp->b_datap->db_frtnp->free_func = NULL;	/* tells freeb not to call */
	mp->b_datap->db_frtnp->free_arg = NULL;
	freemsg(mp);

#if 0
	/* we never have any page fragments, so we can steal a pointer from the page fragement
	   list. */
	assert(skb_shinfo(skb)->nr_frags == 0);
	skb_shinfo(skb)->frags[0].page = (struct page *) np_get(np);
	skb->destructor = np_skb_destructor;
	spin_lock_str2(&np->qlock, flags);
	np->sndmem += skb->truesize;
	spin_unlock_str2(&np->qlock, flags);
#endif

#if 0
	if (likely(skb_head == NULL)) {
		if (unlikely(skb_tail != NULL)) {
			/* Chain skb_tail onto skb. */
			skb_shinfo(skb)->frag_list = skb_tail;
			skb->data_len = skb_tail->len;
			skb->len += skb->data_len;
		}
		return (skb);
	}
	if (likely(skb_tail == NULL)) {
		/* Chain skb onto skb_head. */
		skb_shinfo(skb_head)->frag_list = skb;
		skb_head->data_len = skb->len;
		skb_head->len += skb_head->data_len;
	} else {
		/* Chain skb and skb_tail onto skb_head. */
		skb_shinfo(skb_head)->frag_list = skb;
		skb->next = skb_tail;
		skb_head->data_len = skb->len + skb_tail->len;
		skb_head->len += skb_head->data_len;
	}
	return (skb_head);
#else
      no_skb:
	return (skb);
#endif
#if 0
      no_skb:
	if (skb_tail != NULL)
		kfree_skb(skb_tail);
      no_tail:
	if (skb_head != NULL)
		kfree_skb(skb_head);
      no_head:
#endif
	return (NULL);
      go_frag:			/* for now */
      go_slow:
	return np_alloc_skb_slow(np, mp, headroom, gfp);
}

STATIC INLINE fastcall __hot_out struct sk_buff *
np_alloc_skb(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;

#if 0
	unsigned long flags;
#endif

	if (unlikely((mp->b_datap->db_flag & (DB_SKBUFF)) == 0))
		goto old_way;
	if (unlikely((mp->b_rptr < mp->b_datap->db_base + headroom)))
		goto go_slow;
	if (unlikely((skb = (typeof(skb)) mp->b_datap->db_frtnp->free_arg) == NULL))
		goto go_slow;
	skb_get(skb);
	skb_reserve(skb, mp->b_rptr - skb->data);
	skb_put(skb, mp->b_wptr - mp->b_rptr);
#if 0
	/* we never have any page fragments, so we can steal a pointer from the page fragement
	   list. */
	assert(skb_shinfo(skb)->nr_frags == 0);
	skb_shinfo(skb)->frags[0].page = (struct page *) np_get(np);
	skb->destructor = np_skb_destructor;
	spin_lock_str2(&np->qlock, flags);
	np->sndmem += skb->truesize;
	spin_unlock_str2(&np->qlock, flags);
#endif
	freemsg(mp);
	return (skb);
      old_way:
	return np_alloc_skb_old(np, mp, headroom, gfp);
      go_slow:
	return np_alloc_skb_slow(np, mp, headroom, gfp);
}
#else				/* !defined LFS */
STATIC INLINE fastcall __hot_out struct sk_buff *
np_alloc_skb(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	return np_alloc_skb_slow(np, mp, headroom, gfp);
}
#endif				/* !defined LFS */

noinline fastcall int
np_route_output_slow(struct np *np, const uint32_t daddr, struct rtable **rtp)
{
	int err;

	if (XCHG(rtp, NULL) != NULL)
		dst_release(XCHG(&np->daddrs[0].dst, NULL));
	if (likely((err = ip_route_output(rtp, daddr, np->qos.saddr, 0, 0)) == 0)) {
		dst_hold(&(*rtp)->u.dst);
		np->daddrs[0].dst = &(*rtp)->u.dst;
	}
	return (err);
}

STATIC INLINE fastcall __hot_out int
np_route_output(struct np *np, const uint32_t daddr, struct rtable **rtp)
{
	register struct rtable *rt;

	if (likely((rt = *rtp) != NULL)) {
		if (likely(rt->rt_dst == daddr)) {
			dst_hold(&rt->u.dst);
			return (0);
		}
	}
	return np_route_output_slow(np, daddr, rtp);
}

/**
 * np_senddata - process a unit data request
 * @np: Stream private structure
 * @protocol: IP protocol number for packet
 * @dport: destination port
 * @daddr: destination address
 * @mp: message payload
 */
STATIC INLINE fastcall __hot_out int
np_senddata(struct np *np, uint8_t protocol, const unsigned short dport, uint32_t daddr, mblk_t *mp)
{
	struct rtable *rt;
	int err;

	rt = (struct rtable *) np->daddrs[0].dst;
	prefetch(rt);

	if (likely((err = np_route_output(np, daddr, &rt)) == 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = ((dev->hard_header_len + 15) & ~15)
		    + sizeof(struct iphdr) + sizeof(struct udphdr);
		size_t dlen = msgsize(mp);
		size_t plen = dlen + sizeof(struct udphdr);
		size_t tlen = plen + sizeof(struct iphdr);

		_ptrace(("%s: %s: sending data message block %p\n", DRV_NAME, __FUNCTION__, mp));
		usual(hlen > sizeof(struct iphdr) + sizeof(struct udphdr));
		usual(dlen);

		if (likely((skb = np_alloc_skb(np, mp, hlen, GFP_ATOMIC)) != NULL)) {
			struct iphdr *iph;
			struct udphdr *uh;
			uint32_t saddr = np->qos.saddr ? np->qos.saddr : rt->rt_src;
			uint32_t daddr = rt->rt_dst;

			skb->ip_summed = CHECKSUM_UNNECESSARY;
			skb->csum = 0;
			if (unlikely(opt->udp.checksum != T_YES))
				goto no_csum;
			if (likely(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				goto no_csum;
			skb->ip_summed = 0;
			skb->csum = csum_tcpudp_nofold(saddr, daddr, skb->len, IPPROTO_UDP, 0);
			skb->csum = csum_fold(skb_checksum(skb, 0, skb->len, skb->csum));

		      no_csum:
			/* find headers */
			__skb_push(skb, sizeof(struct udphdr));
			skb_reset_transport_header(skb);
			__skb_push(skb, sizeof(struct iphdr));
			skb_reset_network_header(skb);

			skb->dst = &rt->u.dst;
			skb->priority = 0;	// np->qos.priority;

			iph = (typeof(iph)) skb_network_header(skb);
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = np->qos.tos;
			iph->frag_off = htons(IP_DF);	/* never frag */
			// iph->frag_off = 0; /* need qos bit */
			iph->ttl = np->qos.ttl;
			iph->daddr = daddr;
			iph->saddr = saddr;
			iph->protocol = protocol;
			iph->tot_len = htons(tlen);

			uh = (typeof(uh)) skb_transport_header(skb);
			uh->dest = dport;
			uh->source = np->sport ? np->sport : np->bport;
			uh->len = htons(plen);
			uh->check = 0;

#ifndef HAVE_KFUNC_DST_OUTPUT
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
			__ip_select_ident(iph, &rt->u.dst);
#else
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
			__ip_select_ident(iph, &rt->u.dst, 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
#endif
#endif
			_printd(("sending message %p\n", skb));
#ifdef HAVE_KFUNC_DST_OUTPUT
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, np_udp_queue_xmit);
#else
			np_udp_queue_xmit(skb);
#endif
			return (QR_ABSORBED);
		}
		_rare();
		return (-ENOBUFS);
	}
	_rare();
	return (err);
}

#if 1
STATIC INLINE fastcall int
np_datack(queue_t *q)
{
	/* not supported */
	return (-EOPNOTSUPP);
}
#endif

/**
 * np_conn_check - check and enter into connection hashes
 * @np: private structure
 * @proto: protocol to which to connect
 */
STATIC fastcall int
np_conn_check(struct np *np, unsigned char proto)
{
	unsigned short sport = np->sport;
	unsigned short dport = np->dport;
	struct np *conflict = NULL;
	struct np_chash_bucket *hp, *hp1, *hp2;
	unsigned long flags;

	hp1 = &np_chash[np_chashfn(proto, dport, sport)];
	hp2 = &np_chash[np_chashfn(proto, 0, 0)];

	write_lock_str2(&hp1->lock, flags);
	if (hp1 != hp2)
		write_lock(&hp2->lock);

	hp = hp1;
	do {
		register struct np *np2;

		for (np2 = hp->list; np2; np2 = np2->cnext) {
			int i, j;

			if (np_not_state(np, (NSF_DATA_XFER | NSF_WRES_RIND)))
				continue;
			if (np2->sport != sport)
				continue;
			if (np2->dport != dport)
				continue;
			for (i = 0; conflict == NULL && i < np2->snum; i++)
				for (j = 0; conflict == NULL && j < np->snum; j++)
					if (np2->saddrs[i].addr == np->saddrs[j].addr)
						conflict = np2;
			if (conflict == NULL)
				continue;
			conflict = NULL;
			for (i = 0; conflict == NULL && i < np2->dnum; i++)
				for (j = 0; conflict == NULL && j < np->dnum; j++)
					if (np2->daddrs[i].addr == np->daddrs[j].addr)
						conflict = np2;
			if (conflict == NULL)
				continue;
			break;
		}
	} while (conflict == NULL && hp != hp2 && (hp = hp2));
	if (conflict != NULL) {
		if (hp1 != hp2)
			write_unlock(&hp2->lock);
		write_unlock_str2(&hp1->lock, flags);
		/* how do we say already connected? (-EISCONN) */
		return (NBADADDR);
	}
	/* link into connection hash */
	if ((np->cnext = hp1->list))
		np->cnext->cprev = &np->cnext;
	np->cprev = &hp1->list;
	hp1->list = np_get(np);
	np->chash = hp1;
	if (hp1 != hp2)
		write_unlock(&hp2->lock);
	write_unlock_str2(&hp1->lock, flags);
	return (0);
}

/**
 * np_connect - form a connection
 * @np: private structure
 * @DEST_buffer: pointer to destination addresses
 * @DEST_length: length of destination addresses
 * @QOS_buffer: pointer to connection quality-of-service parameters
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
np_connect(struct np *np, const struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	   struct N_qos_sel_conn_udp *QOS_buffer, const np_ulong CONN_flags)
{
	size_t dnum = DEST_length / sizeof(*DEST_buffer);
	int err;
	int i;

	err = NBADQOSPARAM;
	/* first validate parameters */
	if (QOS_buffer->priority != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->priority < np->qor.priority.priority_min_value)
			goto error;
		if ((np_long) QOS_buffer->priority > np->qor.priority.priority_max_value)
			goto error;
	} else {
		QOS_buffer->priority = np->qos.priority;
	}
	if (QOS_buffer->protocol != QOS_UNKNOWN) {
		for (i = 0; i < np->pnum; i++)
			if (np->protoids[i] == QOS_buffer->protocol)
				break;
		if (i >= np->pnum)
			goto error;
	} else {
		QOS_buffer->protocol = np->qos.protocol;
	}
	if (QOS_buffer->ttl != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->ttl < np->qor.ttl.ttl_min_value)
			goto error;
		if ((np_long) QOS_buffer->ttl > np->qor.ttl.ttl_max_value)
			goto error;
	} else {
		QOS_buffer->ttl = np->qos.ttl;
	}
	if (QOS_buffer->tos != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->tos < np->qor.tos.tos_min_value)
			goto error;
		if ((np_long) QOS_buffer->tos > np->qor.tos.tos_max_value)
			goto error;
	} else {
		QOS_buffer->tos = np->qos.tos;
	}
	if (QOS_buffer->mtu != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->mtu < np->qor.mtu.mtu_min_value)
			goto error;
		if ((np_long) QOS_buffer->mtu > np->qor.mtu.mtu_max_value)
			goto error;
	} else {
		QOS_buffer->mtu = np->qos.mtu;
	}

	/* Need to determine source addressess from bound addresses before we can test the source
	   address.  If we are bound to specific addresses, then the source address list is simply
	   the destination address list. If bound to a wildcard address, then the source address
	   list could be determined from the scope of the destination addresses and the available
	   interfaces and their addresses.  However, for the moment it is probably easier to simply 
	   allow wildcard source addresses and let the user specify any address when there is a
	   wildcard source address. */

	np->sport = np->bport;
	np->snum = np->bnum;
	for (i = 0; i < np->bnum; i++)
		np->saddrs[i].addr = np->baddrs[i].addr;

	if (QOS_buffer->saddr != QOS_UNKNOWN) {
		if (QOS_buffer->saddr != 0) {
			for (i = 0; i < np->snum; i++) {
				if (np->saddrs[i].addr == INADDR_ANY)
					break;
				if (np->saddrs[i].addr == QOS_buffer->saddr)
					break;
			}
			if (i >= np->snum)
				goto recover;
		}
	} else {
		QOS_buffer->saddr = np->qos.saddr;
	}
	if (QOS_buffer->daddr != QOS_UNKNOWN) {
		/* Specified default destination address must be in the destination address list. */
		for (i = 0; i < dnum; i++)
			if (DEST_buffer[i].sin_addr.s_addr == QOS_buffer->daddr)
				break;
		if (i >= dnum)
			goto recover;
	} else {
		/* The default destination address is the first address in the list. */
		QOS_buffer->daddr = DEST_buffer[0].sin_addr.s_addr;
	}

	/* Destination addresses have been checked as follows: they have been aligned. There is at
	   least 1 address and no more than 8 addresses.  The first address has an address family
	   type of AF_INET or zero (0).  No IP address in the list is INADDR_ANY.  Things that have
	   not been checked are: there might be duplicates in the list.  The user might not have the 
	   necessary privilege to use some of the addresses.  Some addresses might be zeronet,
	   broadcast or multicast addresses. The addresses might be of disjoint scope.  There might
	   not exist a route to some addresses.  The destination port number might be zero. */

	np->dport = DEST_buffer[0].sin_port;

	err = NBADADDR;
	if (np->dport == 0 && (np->bport != 0 || np->sport != 0))
		goto recover;
	if (np->dport != 0 && np->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port
		   number. */
		goto recover;

	for (i = 0; i < dnum; i++) {
		struct rtable *rt = NULL;

		if ((err = ip_route_output(&rt, DEST_buffer[i].sin_addr.s_addr, 0, 0, 0)))
			goto recover;
		np->daddrs[i].dst = &rt->u.dst;

		/* Note that we do not have to use the destination reference cached above.  It is
		   enough that we hold a reference to it so that it remains in the routing caches
		   so lookups to this destination are fast.  They will be released upon
		   disconnection. */

		np->daddrs[i].addr = DEST_buffer[i].sin_addr.s_addr;
		np->daddrs[i].ttl = QOS_buffer->ttl;
		np->daddrs[i].tos = QOS_buffer->tos;
		np->daddrs[i].mtu = dst_pmtu(np->daddrs[i].dst);
		if (np->daddrs[i].mtu < QOS_buffer->mtu)
			QOS_buffer->mtu = np->daddrs[i].mtu;
	}
	np->dnum = dnum;

	/* store negotiated values */
	np->qos.protocol = QOS_buffer->protocol;
	np->qos.priority = QOS_buffer->priority;
	np->qos.ttl = QOS_buffer->ttl;
	np->qos.tos = QOS_buffer->tos;
	np->qos.mtu = QOS_buffer->mtu;
	np->qos.saddr = QOS_buffer->saddr;
	np->qos.daddr = QOS_buffer->daddr;
	np->qos.checksum = QOS_buffer->checksum;
	/* note that on failure we are allowed to have partially negotiated some values */

	/* note that all these state changes are not seen by the read side until we are placed into
	   the hashes under hash lock. */

	/* try to place in connection hashes with conflict checks */
	if ((err = np_conn_check(np, QOS_buffer->protocol)) != 0)
		goto recover;

	return (0);
      recover:
	/* clear out source addresses */
	np->sport = 0;
	for (i = 0; i < np->snum; i++) {
		np->saddrs[i].addr = INADDR_ANY;
	}
	np->snum = 0;
	/* clear out destination addresses */
	np->dport = 0;
	for (i = 0; i < np->dnum; i++) {
		if (np->daddrs[i].dst)
			dst_release(XCHG(&np->daddrs[i].dst, NULL));
		np->daddrs[i].addr = INADDR_ANY;
		np->daddrs[i].ttl = 0;
		np->daddrs[i].tos = 0;
		np->daddrs[i].mtu = 0;
	}
	np->dnum = 0;
      error:
	return (err);
}

#if 1
/**
 * np_reset_loc - perform a local reset
 * @np: Stream private structure
 * @RESET_orig: origin of reset
 * @RESET_reason: reason for reset
 * @dp: ICMP message payload
 *
 * When completing a local reset, it is necessary to send an ICMP message to the peer.  The attached
 * M_DATA message blocks contain the ICMP message payload.  The @RESET_reason parameter contains the
 * reset reason that translates to an ICMP error code.  The destination for the reset on a
 * multi-homed connection is the current default destination.
 */
STATIC int
np_reset_loc(struct np *np, np_ulong RESET_orig, np_ulong RESET_reason, mblk_t *dp)
{
	fixme(("Write this function.\n"));
	/* should send ICMP, but don't discard it because send function will not abosorb it. */
	return (QR_DONE);
}

/**
 * np_reset_rem - perform a remote reset
 * @np: Stream private structure
 * @RESET_orig: origin of reset
 * @RESET_reason: reason for reset
 *
 * When completing a remote reset, it is necessary to dequeue and free the earliest outstanding
 * reset indication.
 */
STATIC int
np_reset_rem(struct np *np, np_ulong RESET_orig, np_ulong RESET_reason)
{
	mblk_t *rp;

	/* free last one on list */
	if ((rp = bufq_tail(&np->resq)) != NULL) {
		bufq_unlink(&np->resq, rp);
		freemsg(rp);
	}
	return (0);
}
#endif

STATIC int
np_optmgmt(struct np *np, union N_qos_udp_types *QOS_buffer, np_ulong OPTMGMT_flags)
{
	int i;

	switch (QOS_buffer->n_qos_type) {
	case N_QOS_SEL_INFO_UDP:
		/* protocol must be one of the bound protocol ids */
		if ((np_long) QOS_buffer->n_qos_sel_info.protocol != QOS_UNKNOWN && np->pnum > 0) {
			if ((np_long) QOS_buffer->n_qos_sel_info.protocol < 0) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.protocol > 255) {
				return (NBADQOSPARAM);
			}
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->n_qos_sel_info.protocol)
					break;
			if (i >= np->pnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.priority <
			    np->qor.priority.priority_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.priority >
			    np->qor.priority.priority_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.ttl < np->qor.ttl.ttl_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.ttl > np->qor.ttl.ttl_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.tos < np->qor.tos.tos_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.tos > np->qor.tos.tos_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.mtu != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.mtu < np->qor.mtu.mtu_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.mtu > np->qor.mtu.mtu_max_value) {
				return (NBADQOSPARAM);
			}
		}
		/* source address should be one of the specified source addresses */
		if ((np_long) QOS_buffer->n_qos_sel_info.saddr != QOS_UNKNOWN && np->snum > 0) {
			if (QOS_buffer->n_qos_sel_info.saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->n_qos_sel_info.saddr)
						break;
				}
				if (i >= np->snum) {
					return (NBADQOSPARAM);
				}
			}
		}
		/* destination address must be one of the specified destination addresses */
		if ((np_long) QOS_buffer->n_qos_sel_info.daddr != QOS_UNKNOWN && np->dnum > 0) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == QOS_buffer->n_qos_sel_info.daddr)
					break;
			if (i >= np->dnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.protocol != QOS_UNKNOWN)
			np->qos.protocol = QOS_buffer->n_qos_sel_info.protocol;
		if ((np_long) QOS_buffer->n_qos_sel_info.priority != QOS_UNKNOWN)
			np->qos.priority = QOS_buffer->n_qos_sel_info.priority;
		if ((np_long) QOS_buffer->n_qos_sel_info.ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->n_qos_sel_info.ttl;
		if ((np_long) QOS_buffer->n_qos_sel_info.tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->n_qos_sel_info.tos;
		if ((np_long) QOS_buffer->n_qos_sel_info.mtu != QOS_UNKNOWN)
			np->qos.mtu = QOS_buffer->n_qos_sel_info.mtu;
		if ((np_long) QOS_buffer->n_qos_sel_info.saddr != QOS_UNKNOWN)
			np->qos.saddr = QOS_buffer->n_qos_sel_info.saddr;
		if ((np_long) QOS_buffer->n_qos_sel_info.daddr != QOS_UNKNOWN)
			np->qos.daddr = QOS_buffer->n_qos_sel_info.daddr;
		break;
	case N_QOS_RANGE_INFO_UDP:
		return (NBADQOSTYPE);
	case N_QOS_SEL_CONN_UDP:
		if (!(np->info.SERV_type & N_CONS))
			return (NBADQOSTYPE);
		/* protocol must be one of the bound protocol ids */
		if ((np_long) QOS_buffer->n_qos_sel_conn.protocol != QOS_UNKNOWN && np->pnum > 0) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.protocol < 0) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.protocol > 255) {
				return (NBADQOSPARAM);
			}
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->n_qos_sel_conn.protocol)
					break;
			if (i >= np->pnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.priority <
			    np->qor.priority.priority_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.priority >
			    np->qor.priority.priority_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.ttl < np->qor.ttl.ttl_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.ttl > np->qor.ttl.ttl_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.tos < np->qor.tos.tos_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.tos > np->qor.tos.tos_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.mtu != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.mtu < np->qor.mtu.mtu_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.mtu > np->qor.mtu.mtu_max_value) {
				return (NBADQOSPARAM);
			}
		}
		/* source address should be one of the specified source addresses */
		if ((np_long) QOS_buffer->n_qos_sel_conn.saddr != QOS_UNKNOWN && np->snum > 0) {
			if (QOS_buffer->n_qos_sel_conn.saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->n_qos_sel_conn.saddr)
						break;
				}
				if (i >= np->snum) {
					return (NBADQOSPARAM);
				}
			}
		}
		/* destination address must be one of the specified destination addresses */
		if ((np_long) QOS_buffer->n_qos_sel_conn.daddr != QOS_UNKNOWN) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == QOS_buffer->n_qos_sel_conn.daddr)
					break;
			if (i >= np->dnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.protocol != QOS_UNKNOWN)
			np->qos.protocol = QOS_buffer->n_qos_sel_conn.protocol;
		if ((np_long) QOS_buffer->n_qos_sel_conn.priority != QOS_UNKNOWN)
			np->qos.priority = QOS_buffer->n_qos_sel_conn.priority;
		if ((np_long) QOS_buffer->n_qos_sel_conn.ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->n_qos_sel_conn.ttl;
		if ((np_long) QOS_buffer->n_qos_sel_conn.tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->n_qos_sel_conn.tos;
		if ((np_long) QOS_buffer->n_qos_sel_conn.mtu != QOS_UNKNOWN)
			np->qos.mtu = QOS_buffer->n_qos_sel_conn.mtu;
		if ((np_long) QOS_buffer->n_qos_sel_conn.saddr != QOS_UNKNOWN)
			np->qos.saddr = QOS_buffer->n_qos_sel_conn.saddr;
		if ((np_long) QOS_buffer->n_qos_sel_conn.daddr != QOS_UNKNOWN)
			np->qos.daddr = QOS_buffer->n_qos_sel_conn.daddr;
		np->info.SERV_type = N_CONS;
		break;
	case N_QOS_SEL_UD_UDP:
		if (!(np->info.SERV_type & N_CLNS))
			return (NBADQOSTYPE);
		/* protocol must be one of the bound protocol ids */
		if ((np_long) QOS_buffer->n_qos_sel_ud.protocol != QOS_UNKNOWN && np->pnum > 0) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.protocol < 0) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.protocol > 255) {
				return (NBADQOSPARAM);
			}
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->n_qos_sel_ud.protocol)
					break;
			if (i >= np->pnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.priority <
			    np->qor.priority.priority_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.priority >
			    np->qor.priority.priority_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.ttl < np->qor.ttl.ttl_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.ttl > np->qor.ttl.ttl_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.tos < np->qor.tos.tos_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.tos > np->qor.tos.tos_max_value) {
				return (NBADQOSPARAM);
			}
		}
		/* source address should be one of the specified source addresses */
		if ((np_long) QOS_buffer->n_qos_sel_ud.saddr != QOS_UNKNOWN && np->snum > 0) {
			if (QOS_buffer->n_qos_sel_ud.saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->n_qos_sel_ud.saddr)
						break;
				}
				if (i >= np->snum) {
					return (NBADQOSPARAM);
				}
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.protocol != QOS_UNKNOWN)
			np->qos.protocol = QOS_buffer->n_qos_sel_ud.protocol;
		if ((np_long) QOS_buffer->n_qos_sel_ud.priority != QOS_UNKNOWN)
			np->qos.priority = QOS_buffer->n_qos_sel_ud.priority;
		if ((np_long) QOS_buffer->n_qos_sel_ud.ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->n_qos_sel_ud.ttl;
		if ((np_long) QOS_buffer->n_qos_sel_ud.tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->n_qos_sel_ud.tos;
		if ((np_long) QOS_buffer->n_qos_sel_ud.saddr != QOS_UNKNOWN)
			np->qos.saddr = QOS_buffer->n_qos_sel_ud.saddr;
		np->info.SERV_type = N_CLNS;
		break;
	default:
		return (NBADQOSTYPE);
	}
	return (0);
}

/**
 * np_unbind - unbind a Stream from an NSAP
 * @np: private structure
 *
 * Simply remove the Stream from the bind hashes and release a reference to the Stream.  This
 * function can be called whether the stream is bound or not (and is always called before the
 * private structure is freed.
 */
STATIC int
np_unbind(struct np *np)
{
	struct np_bhash_bucket *hp;
	unsigned long flags;

	if ((hp = np->bhash)) {
		write_lock_str2(&hp->lock, flags);
		if ((*np->bprev = np->bnext))
			np->bnext->bprev = np->bprev;
		np->bnext = NULL;
		np->bprev = &np->bnext;
		np->bhash = NULL;
		np_unbind_prot(np->protoids[0], np->info.SERV_type);
		np->bport = np->sport = 0;
		np->bnum = np->snum = np->pnum = 0;
		np_set_state(np, NS_UNBND);
		np_put(np);
		write_unlock_str2(&hp->lock, flags);
#if defined HAVE_KFUNC_SYNCHRONIZE_NET
		synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KFUNC_SYNCHRONIZE_NET */
		return (0);
	}
	return (-EALREADY);
}

/**
 * np_passive - perform a passive connection
 * @np: private structure
 * @RES_buffer: responding addresses
 * @RES_length: length of responding addresses
 * @QOS_buffer: quality of service parameters
 * @SEQ_number: connection indication being accepted
 * @TOKEN_value: accepting Stream private structure
 * @CONN_flags: connection flags
 * @dp: user connect data
 */
noinline fastcall int
np_passive(struct np *np, struct sockaddr_in *RES_buffer, const socklen_t RES_length,
	   struct N_qos_sel_conn_udp *QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value,
	   const np_ulong CONN_flags, mblk_t *dp)
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

	if (TOKEN_value != np) {
		err = NBADTOKEN;
		/* Accepting Stream must be bound to the same protocol as connection indication. */
		for (j = 0; j < TOKEN_value->pnum; j++)
			if (TOKEN_value->protoids[j] == iph->protocol)
				break;
		if (j >= TOKEN_value->pnum)
			/* Must be bound to the same protocol. */
			goto error;
		/* Accepting Stream must be bound to the same address (or wildcard) including
		   destination address in connection indication. */
		for (i = 0; i < TOKEN_value->bnum; i++)
			if (TOKEN_value->baddrs[i].addr == INADDR_ANY
			    || TOKEN_value->baddrs[i].addr == iph->daddr)
				break;
		if (i >= TOKEN_value->bnum)
			goto error;
	}

	/* validate parameters */
	err = NBADQOSPARAM;
	/* Cannot really validate parameters here.  One of the problems is that some of the
	   information against which we should be checking is contained in the connection
	   indication packet, and other information is associated with the destination addresses
	   themselves, that are contained in the responding address(es) for NPI-IP.  Therefore, QOS 
	   parameter checks must be performed in the np_passive() function instead. */
	if (QOS_buffer->protocol != QOS_UNKNOWN) {
		/* Specified protocol probably needs to be the same as the indication, but since we
		   only bind to one protocol id at the moment that is not a problem.  The connection 
		   indication protocol was checked against the accepting Stream above. */
		for (i = 0; i < TOKEN_value->pnum; i++)
			if (TOKEN_value->protoids[i] == QOS_buffer->protocol)
				break;
		if (i >= TOKEN_value->pnum)
			goto error;
	} else {
		QOS_buffer->protocol = TOKEN_value->qos.protocol;
	}
	if (QOS_buffer->priority != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->priority < TOKEN_value->qor.priority.priority_min_value)
			goto error;
		if ((np_long) QOS_buffer->priority > TOKEN_value->qor.priority.priority_max_value)
			goto error;
	} else {
		QOS_buffer->priority = TOKEN_value->qos.priority;
	}
	if (QOS_buffer->ttl != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->ttl < TOKEN_value->qor.ttl.ttl_min_value)
			goto error;
		if ((np_long) QOS_buffer->ttl > TOKEN_value->qor.ttl.ttl_max_value)
			goto error;
	} else {
		QOS_buffer->ttl = TOKEN_value->qos.ttl;
	}
	if (QOS_buffer->tos != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->tos < TOKEN_value->qor.tos.tos_min_value)
			goto error;
		if ((np_long) QOS_buffer->tos > TOKEN_value->qor.tos.tos_max_value)
			goto error;
	} else {
		/* FIXME: TOS should be negotiated.  The TOS should be upgraded to whatever TOS the 
		   caller wishes, but not downgraded. */
		QOS_buffer->tos = TOKEN_value->qos.tos;
	}
	if (QOS_buffer->mtu != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->mtu < TOKEN_value->qor.mtu.mtu_min_value)
			goto error;
		if ((np_long) QOS_buffer->mtu > TOKEN_value->qor.mtu.mtu_max_value)
			goto error;
		/* FIXME: MTU should be negotiated.  The MTU should be downgraded to the lesser
		   value of what the connection requires or what was specified, but not upgraded. */
	} else {
		QOS_buffer->mtu = TOKEN_value->qos.mtu;
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

	TOKEN_value->sport = uh->dest;
	TOKEN_value->snum = TOKEN_value->bnum;
	for (i = 0; i < TOKEN_value->bnum; i++)
		TOKEN_value->saddrs[i].addr = TOKEN_value->baddrs[i].addr;

	if (QOS_buffer->saddr != QOS_UNKNOWN) {
		if (QOS_buffer->saddr != 0) {
			for (i = 0; i < TOKEN_value->snum; i++) {
				if (TOKEN_value->saddrs[i].addr == INADDR_ANY)
					break;
				if (TOKEN_value->saddrs[i].addr == QOS_buffer->saddr)
					break;
			}
			if (i >= TOKEN_value->snum)
				goto recover;
		}
	} else {
		QOS_buffer->saddr = TOKEN_value->qos.saddr;
	}

	/* Here's a problem: we don't realy have any destination addresses yet, so we can't check
	   at this point. */

	if (QOS_buffer->daddr != QOS_UNKNOWN) {
		if (rnum > 0) {
			/* Specified destination addresses must be in the responding address list. */
			for (i = 0; i < rnum; i++)
				if (RES_buffer[i].sin_addr.s_addr == QOS_buffer->daddr)
					break;
			if (i >= rnum)
				goto recover;
		} else {
			/* If no responding address list is provided (rnum == 0), the destination
			   address must be the source address of the connection indication. */
			if (QOS_buffer->daddr != iph->saddr)
				goto recover;
		}
	} else {
		QOS_buffer->daddr = rnum ? RES_buffer[0].sin_addr.s_addr : iph->saddr;
	}

	TOKEN_value->dport = rnum ? RES_buffer[0].sin_port : uh->source;

	err = NBADADDR;
	if (TOKEN_value->dport == 0 && (TOKEN_value->bport != 0 || TOKEN_value->sport != 0))
		goto recover;
	if (TOKEN_value->dport != 0 && TOKEN_value->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port
		   number. */
		goto recover;

	if (rnum > 0) {
		for (i = 0; i < rnum; i++) {
			struct rtable *rt = NULL;

			if ((err = ip_route_output(&rt, RES_buffer[i].sin_addr.s_addr, 0, 0, 0)))
				goto recover;
			TOKEN_value->daddrs[i].dst = &rt->u.dst;

			/* Note that we do not have to use the destination reference cached above.
			   It is enough that we hold a reference to it so that it remains in the
			   routing caches so lookups to this destination are fast.  They will be
			   released upon disconnection. */

			TOKEN_value->daddrs[i].addr = RES_buffer[i].sin_addr.s_addr;
			TOKEN_value->daddrs[i].ttl = QOS_buffer->ttl;
			TOKEN_value->daddrs[i].tos = QOS_buffer->tos;
			TOKEN_value->daddrs[i].mtu = dst_pmtu(TOKEN_value->daddrs[i].dst);
			if (TOKEN_value->daddrs[i].mtu < QOS_buffer->mtu)
				QOS_buffer->mtu = TOKEN_value->daddrs[i].mtu;
		}
		TOKEN_value->dnum = rnum;
	} else {
		struct rtable *rt = NULL;

		if ((err = ip_route_output(&rt, iph->saddr, 0, 0, 0)))
			goto recover;
		TOKEN_value->daddrs[0].dst = &rt->u.dst;

		/* Note that we do not have to use the destination reference cached above.  It is
		   enough that we hold a reference to it so that it remains in the routing caches
		   so lookups to this destination are fast.  They will be released upon
		   disconnection. */

		TOKEN_value->daddrs[0].addr = iph->saddr;
		TOKEN_value->daddrs[0].ttl = QOS_buffer->ttl;
		TOKEN_value->daddrs[0].tos = QOS_buffer->tos;
		TOKEN_value->daddrs[0].mtu = dst_pmtu(TOKEN_value->daddrs[0].dst);
		if (TOKEN_value->daddrs[0].mtu < QOS_buffer->mtu)
			QOS_buffer->mtu = TOKEN_value->daddrs[0].mtu;

		TOKEN_value->dnum = 1;
	}

	/* store negotiated qos values */
	TOKEN_value->qos.protocol = QOS_buffer->protocol;
	TOKEN_value->qos.priority = QOS_buffer->priority;
	TOKEN_value->qos.ttl = QOS_buffer->ttl;
	TOKEN_value->qos.tos = QOS_buffer->tos;
	TOKEN_value->qos.mtu = QOS_buffer->mtu;
	TOKEN_value->qos.saddr = QOS_buffer->saddr;
	TOKEN_value->qos.daddr = QOS_buffer->daddr;
	/* note: on failure allowed to have partially negotiated options */

	/* try to place in connection hashes with conflict checks */
	if ((err = np_conn_check(TOKEN_value, QOS_buffer->protocol)) != 0)
		goto recover;

	if (dp != NULL)
		if (unlikely
		    ((err = np_senddata(np, np->qos.protocol, np->qos.daddr, dp)) != QR_ABSORBED))
			goto recover;
	if (SEQ_number != NULL) {
		bufq_unlink(&np->conq, SEQ_number);
		freeb(XCHG(&SEQ_number, SEQ_number->b_cont));
		/* queue any pending data */
		while (SEQ_number)
			put(TOKEN_value->oq, XCHG(&SEQ_number, SEQ_number->b_cont));
	}

	return (QR_ABSORBED);

      recover:
	/* clear out source addresses */
	TOKEN_value->sport = 0;
	for (i = 0; i < TOKEN_value->snum; i++) {
		TOKEN_value->saddrs[i].addr = INADDR_ANY;
	}
	TOKEN_value->snum = 0;
	/* clear out destination addresses */
	TOKEN_value->dport = 0;
	for (i = 0; i < TOKEN_value->dnum; i++) {
		if (TOKEN_value->daddrs[i].dst)
			dst_release(XCHG(&TOKEN_value->daddrs[i].dst, NULL));
		TOKEN_value->daddrs[i].addr = INADDR_ANY;
		TOKEN_value->daddrs[i].ttl = 0;
		TOKEN_value->daddrs[i].tos = 0;
		TOKEN_value->daddrs[i].mtu = 0;
	}
	TOKEN_value->dnum = 0;
      error:
	return (err);
}

noinline fastcall __unlikely int
np_w_prim_error(queue_t *q, const int rtn)
{
	switch (rtn) {
	case -EBUSY:		/* flow controlled */
	case -EAGAIN:		/* try again */
	case -ENOMEM:		/* could not allocate memory */
	case -ENOBUFS:		/* could not allocate an mblk */
	case -EOPNOTSUPP:	/* primitive not supported */
		return ne_error_ack(q, N_UNITDATA_REQ, rtn);
	case -EPROTO:
		return ne_error_reply(q, -EPROTO);
	default:
		return (0);
	}
}

/**
 * np_w_prim - process primitive on write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC INLINE streamscall __hot_put int
np_w_prim(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_PROTO)) {
		/* fast path for data */
		if (likely(mp->b_wptr >= mp->b_rptr + sizeof(np_ulong))) {
			if (likely(*((np_ulong *) mp->b_rptr) == N_UNITDATA_REQ)) {
				int rtn;

				if (likely((rtn = ne_unitdata_req(q, mp)) >= 0))
					return (rtn);
				return np_w_prim_error(q, rtn);
			}
		}
	}
	return np_w_prim_slow(q, mp);
}

noinline fastcall void
np_putq_slow(queue_t *q, mblk_t *mp, int rtn)
{
	switch (rtn) {
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		break;
	case QR_STRIP:
		if (mp->b_cont)
			if (unlikely(putq(q, mp->b_cont) == 0))
				freemsg(mp->b_cont);
	case QR_TRIMMED:
		freeb(mp);
		break;
	case QR_LOOP:
		if (!q->q_next) {
			qreply(q, mp);
			break;
		}
	case QR_PASSALONG:
		if (q->q_next) {
			putnext(q, mp);
			break;
		}
		rtn = -EOPNOTSUPP;
	default:
		printd(("%s: %p: ERROR: (q dropping) %d\n", q->q_qinfo->qi_minfo->mi_idname,
			q->q_ptr, rtn));
		freemsg(mp);
		break;
	case QR_DISABLE:
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		break;
	case QR_PASSFLOW:
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			break;
		}
	case -ENOBUFS:
	case -EBUSY:
	case -ENOMEM:
	case -EAGAIN:
	case QR_RETRY:
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		break;
	}
	return;
}

static noinline fastcall int
np_srvq_slow(queue_t *q, mblk_t *mp, int rtn)
{
	switch (rtn) {
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		return (1);
	case QR_STRIP:
		if (mp->b_cont)
			if (!putbq(q, mp->b_cont))
				freemsg(mp->b_cont);
	case QR_TRIMMED:
		freeb(mp);
		return (1);
	case QR_LOOP:
		if (!q->q_next) {
			qreply(q, mp);
			return (1);
		}
	case QR_PASSALONG:
		if (q->q_next) {
			putnext(q, mp);
			return (1);
		}
		rtn = -EOPNOTSUPP;
	default:
		printd(("%s: %p: ERROR: (q dropping) %d\n", q->q_qinfo->qi_minfo->mi_idname,
			q->q_ptr, rtn));
		freemsg(mp);
		return (1);
	case QR_DISABLE:
		printd(("%s: %p: ERROR: (q disabling) %d\n", q->q_qinfo->qi_minfo->mi_idname,
			q->q_ptr, rtn));
		noenable(q);
		if (!putbq(q, mp))
			freemsg(mp);
		rtn = 0;
		return (0);
	case QR_PASSFLOW:
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (1);
		}
	case -ENOBUFS:		/* proc must have scheduled bufcall */
	case -EBUSY:		/* proc must have failed canput */
	case -ENOMEM:		/* proc must have scheduled bufcall */
	case -EAGAIN:		/* proc must re-enable on some event */
		if (mp->b_datap->db_type < QPCTL) {
			printd(("%s: %p: ERROR: (q stalled) %d\n", q->q_qinfo->qi_minfo->mi_idname,
				q->q_ptr, rtn));
			if (!putbq(q, mp))
				freemsg(mp);
			return (0);
		}
		/* 
		 *  Be careful not to put a priority message back on the queue.
		 */
		switch (mp->b_datap->db_type) {
		case M_PCPROTO:
			mp->b_datap->db_type = M_PROTO;
			break;
		case M_PCRSE:
			mp->b_datap->db_type = M_RSE;
			break;
		case M_PCCTL:
			mp->b_datap->db_type = M_CTL;
			break;
		default:
			printd(("%s: %p: ERROR: (q dropping) %d\n", q->q_qinfo->qi_minfo->mi_idname,
				q->q_ptr, rtn));
			freemsg(mp);
			return (1);
		}
		mp->b_band = 255;
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		return (0);
	case QR_RETRY:
		if (!putbq(q, mp))
			freemsg(mp);
		return (1);
	}
}

streamscall __hot_in int
np_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))) {
		np_rstat.ms_acnt++;
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
	} else {
		int rtn;

		rtn = np_r_prim(q, mp);
		/* Fast Paths */
		if (likely(rtn == QR_TRIMMED))
			freeb(mp);
		else if (likely(rtn == QR_DONE))
			freemsg(mp);
		else
			np_putq_slow(q, mp, rtn);
	}
	return (0);
}

streamscall __hot_in int
np_rsrv(queue_t *q)
{
	mblk_t *mp;

#if 1
	/* try bottom half locking across loop to allow softirqd to burst. */
	local_bh_disable();
#endif
	while (likely((mp = getq(q)) != NULL)) {
		int rtn;

		rtn = np_r_prim(q, mp);
		/* Fast Path */
		if (likely(rtn == QR_ABSORBED)) {
			continue;
		} else if (unlikely(np_srvq_slow(q, mp, rtn) == 0))
			break;
	}
#if 1
	/* this should run the burst from softirqd. */
	local_bh_enable();
#endif
	return (0);
}

streamscall __hot_out int
np_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))) {
		np_wstat.ms_acnt++;
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
	} else {
		int rtn;

		rtn = np_w_prim(q, mp);
		/* Fast Paths */
		if (likely(rtn == QR_TRIMMED)) {
			freeb(mp);
			return (0);
		} else if (likely(rtn == QR_DONE))
			freemsg(mp);
		else
			np_putq_slow(q, mp, rtn);
	}
	return (0);
}

streamscall __hot_out int
np_wsrv(queue_t *q)
{
	mblk_t *mp;

#if 0
	/* try bottom half locking across loop to bundle burst for softirqd. */
	local_bh_disable();
#endif
	while (likely((mp = getq(q)) != NULL)) {
		register int rtn;

		rtn = np_w_prim(q, mp);
		/* Fast Path */
		if (likely(rtn == QR_TRIMMED)) {
			freeb(mp);
			continue;
		} else if (unlikely(np_srvq_slow(q, mp, rtn) == 0))
			break;
	}
#if 0
	/* this should run the burst to softirqd. */
	local_bh_enable();
#endif
	return (0);
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
/**
 * np_lookup_conn - lookup Stream in the connection hashes
 * @proto: protocol (of received packet)
 * @daddr: destination address (of received packet)
 * @dport: destination port (of received packet)
 * @saddr: source address (of received packet)
 * @sport: source port (of received packet)
 *
 * The start of the right kind of code shaping up here.  We provide connection buckets that have
 * IP protocol number, source port and destination port as hash parameters.  Attached to the
 * connection bucket are all Streams with the same protocol number, source port and destination port
 * combination (but possibly different IP adresses).  These Streams that are "owners" of the
 * connection bucket must be traversed and checked for address matches.
 */
noinline fastcall __unlikely struct np *
np_lookup_conn(unsigned char proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct np *result = NULL;
	int hiscore = -1;
	struct np_chash_bucket *hp, *hp1, *hp2;
	unsigned long flags;

	hp1 = &np_chash[np_chashfn(proto, sport, dport)];
	hp2 = &np_chash[np_chashfn(proto, 0, 0)];

	hp = hp1;
	do {
		read_lock_str2(&hp->lock, flags);
		{
			register struct np *np;

			for (np = hp->list; np; np = np->cnext) {
				register int score = 0;
				register int i;

				/* only Streams in close to the correct state */
				if (np_not_state(np, (NSF_DATA_XFER | NSF_WRES_RIND)))
					continue;
				/* must match a bound protocol id */
				for (i = 0; i < np->pnum; i++) {
					if (np->protoids[i] != proto)
						continue;
					break;
				}
				if (i >= np->pnum)
					continue;
				if (np->sport != 0) {
					if (np->sport != sport)
						continue;
					score++;
				}
				if (np->dport != 0) {
					if (np->dport != dport)
						continue;
					score++;
				}
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == 0)
						break;
					if (np->saddrs[i].addr != daddr)
						continue;
					score++;
					break;
				}
				if (i >= np->snum)
					continue;
				for (i = 0; i < np->dnum; i++) {
					if (np->daddrs[i].addr == 0)
						break;
					if (np->daddrs[i].addr != saddr)
						continue;
					score++;
					break;
				}
				if (i >= np->dnum)
					continue;
				if (score > hiscore) {
					hiscore = score;
					if (result != NULL)
						np_put(result);
					result = np_get(np);
				}
				if (score == 4)
					/* perfect match */
					break;
			}
		}
		read_unlock_str2(&hp->lock, flags);
	} while (hiscore < 4 && hp != hp2 && (hp = hp2));
	usual(result);
	return (result);
}

/**
 * np_lookup_bind - lookup Stream in the bind/listening hashes
 * @proto:	protocol from IP header
 * @daddr:	destination address from IP header
 * @dport:	destination port from UDP header
 *
 * Note that an N_CLNS Stream can bind to either a port number or port number zero. An N_CONS Stream
 * can only bind listening to a non-zero port number, but can bind normal to a zero port number.
 *
 * N_CLNS Streams are always entered into the bind hashes against port number zero.  N_CONS Streams
 * are entered against their bound port number (which can be zero only if they are not listening).
 * N_CONS Streams that are not listening will not be matched.  Only the DEFAULT_LISTENER can be
 * bound listening against a zero port (and it requires a wildcard address).
 *
 * NOTICES: There are two types of Streams in the bind hashes, N_CONS and N_CLNS.  When lookup up
 * a Stream for a received packet, we are interested in any N_CLNS Stream that matches or any N_CONS
 * stream that is in the listening state that matches.
 */
STATIC INLINE fastcall __hot_in struct np *
np_lookup_bind(unsigned char proto, uint32_t daddr, unsigned short dport)
{
	struct np *result = NULL;
	int hiscore = -1;
	struct np_bhash_bucket *hp, *hp1, *hp2;
	unsigned long flags;

	hp1 = &np_bhash[np_bhashfn(proto, dport)];
	hp2 = &np_bhash[np_bhashfn(proto, 0)];

	hp = hp1;
	_ptrace(("%s: %s: proto = %d, dport = %d\n", DRV_NAME, __FUNCTION__, (int) proto,
		 (int) ntohs(dport)));
	do {
		read_lock_str2(&hp->lock, flags);
		{
			register struct np *np;

			for (np = hp->list; np; np = np->bnext) {
				register int score = 0;
				register int i;

				/* only listening N_CONS Streams and N_CLNS Streams */
				if (np->CONIND_number == 0 && np->info.SERV_type != N_CLNS)
					continue;
				/* only Streams in close to the correct state */
				if (np_not_state(np, (NSF_IDLE | NSF_WCON_CREQ)))
					continue;
				for (i = 0; i < np->pnum; i++) {
					if (np->protoids[i] != proto)
						continue;
					break;
				}
				if (i >= np->pnum)
					continue;
				if (np->bport != 0) {
					if (np->bport != dport)
						continue;
					score++;
				}
				if (!(np->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
					score++;
				for (i = 0; i < np->bnum; i++) {
					if (np->baddrs[i].addr == 0)
						break;
					if (np->baddrs[i].addr != daddr)
						continue;
					score++;
					break;
				}
				if (i >= np->bnum)
					continue;
				if (score > hiscore) {
					hiscore = score;
					if (result != NULL)
						np_put(result);
					result = np_get(np);
				}
				if (score == 3)
					/* perfect match */
					break;
			}
		}
		read_unlock_str2(&hp->lock, flags);
	} while (hiscore < 2 && hp != hp2 && (hp = hp2));
	usual(result);
	return (result);
}

noinline fastcall __unlikely struct np *
np_lookup_common_slow(struct np_prot_bucket *pp, uint8_t proto, uint32_t daddr, uint16_t dport,
		      uint32_t saddr, uint16_t sport)
{
	struct np *result = NULL;

	if (likely(pp != NULL)) {
		if (likely(pp->corefs != 0))
			result = np_lookup_conn(proto, daddr, dport, saddr, sport);
		if (result == NULL && (pp->corefs != 0 || pp->clrefs != 0))
			result = np_lookup_bind(proto, daddr, dport);
	}
	return (result);
}

STATIC INLINE fastcall __hot_in struct np *
np_lookup_common(uint8_t proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct np_prot_bucket *pp, **ppp;
	register struct np *result;
	unsigned long flags;

	ppp = &np_prots[proto];

	read_lock_str2(&np_prot_lock, flags);
	if (likely((pp = *ppp) != NULL)) {
		if (likely(pp->corefs == 0)) {
			if (likely(pp->clrefs > 0)) {
				result = np_lookup_bind(proto, daddr, dport);
			      done:
				read_unlock_str2(&np_prot_lock, flags);
				return (result);
			}
		}
	}
	result = np_lookup_common_slow(pp, proto, daddr, dport, saddr, sport);
	goto done;
}

/**
 * np_lookup - lookup Stream by protocol, address and port.
 * @iph:	IP header
 * @uh:		UDP header
 *
 * The UDP header here is just to get port numbers in the right place.  All connection-oriented IP
 * protocols use port numbers in the same layout as UDP.  This header is ignored for connectionless
 * lookups if the Stream is not bound to a non-zero port number.
 *
 * There are two places to look: connection hash and bind hash.  Connection hashes and bind hashes
 * are only searched for connection-oriented Streams if there is a connection oriented Stream bound
 * to the protocol id.  Bind hashes are only searched for connectionless Streams if there is a
 * connectionless Stream bound to the protocol id.
 */
STATIC INLINE fastcall __hot_in struct np *
np_lookup(struct iphdr *iph, struct udphdr *uh)
{
	return np_lookup_common(iph->protocol, iph->daddr, uh->dest, iph->saddr, uh->source);
}

/**
 * np_lookup_icmp - a little different from np_lookup(), for ICMP messages
 * @iph:	returned IP header
 * @len:	length of returned message
 *
 * This needs to do a reverse lookup (where destination address and port are compared to source
 * address and port, and visa versa).
 */
noinline fastcall __unlikely struct np *
np_lookup_icmp(struct iphdr *iph, unsigned int len)
{
	struct udphdr *uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));

	if (len < (iph->ihl << 2) + 4)
		/* too short: don't have port numbers - ignore it */
		return (NULL);

	return np_lookup_common(iph->protocol, iph->saddr, uh->source, iph->daddr, uh->dest);
}

/**
 * np_free - message block free function for mblks esballoc'ed from sk_buffs
 * @data: client data (sk_buff pointer in this case)
 */
STATIC streamscall __hot_get void
np_free(caddr_t data)
{
	struct sk_buff *skb = (typeof(skb)) data;

	dassert(skb != NULL);
	kfree_skb(skb);
	return;
}

/**
 * np_v4_rcv - process a received IP packet
 * @skb: socket buffer containing IP packet
 *
 * This function is a callback function called by the Linux IP code when a packet is delivered to an
 * IP protocol number to which a Stream is bound.  If the destination address is a broadcast or
 * multicast address, pass it for distribution to multiple Streams.  If the destination address is a
 * unicast address, look up the receiving IP Stream based on the protocol number and IP addresses.
 * If no receiving IP Stream exists for a unicast packet, or if the packet is a broadcast or
 * multicast packet, pass the packet along to the next handler if any. If there is no next handler
 * and the packet was not sent to any Stream, generate an appropriate ICMP error.  If the receiving
 * Stream is flow controlled, simply discard its copy of the IP packet.  Otherwise, generate an
 * (internal) M_DATA message and pass it to the Stream.
 */
STATIC __hot_in int
np_v4_rcv(struct sk_buff *skb)
{
	struct np *np;
	struct iphdr *iph = (typeof(iph)) skb_network_header(skb);
	struct udphdr *uh = (struct udphdr *) (skb_network_header(skb) + (iph->ihl << 2));
	struct rtable *rt;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif

//      IP_INC_STATS_BH(IpInDelivers);  /* should wait... */

	if (unlikely(!pskb_may_pull(skb, 4)))
		goto too_small;
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
	rt = (struct rtable *) skb->dst;
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;

	_printd(("%s: %s: packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* we do the lookup before the checksum */
	if (unlikely((np = np_lookup(iph, uh)) == NULL))
		goto no_stream;
#if 0
	/* For now... We should actually place non-linear fragments into separate mblks and pass
	   them up as a chain, or deal with non-linear sk_buffs directly.  As it winds up, the
	   netfilter hooks linearize anyway. */
#ifdef HAVE_KFUNC_SKB_LINEARIZE_1_ARG
	if (unlikely(skb_is_nonlinear(skb) && skb_linearize(skb) != 0))
		goto linear_fail;
#else				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	if (unlikely(skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0))
		goto linear_fail;
#endif				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
#else
	if (unlikely(skb_is_nonlinear(skb))) {
		_ptrace(("Non-linear sk_buff encountered!\n"));
		goto linear_fail;
	}
#endif
	{
		mblk_t *mp;
		frtn_t fr = { &np_free, (caddr_t) skb };
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* now allocate an mblk */
		if (unlikely((mp = esballoc(skb_network_header(skb), plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
#ifndef LIS
		/* tell others it is a socket buffer */
		mp->b_datap->db_flag |= DB_SKBUFF;
#endif
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* check flow control only after we have a buffer */
		if (np->oq == NULL || !canput(np->oq))
			goto flow_controlled;
		// mp->b_datap->db_type = M_DATA;
		mp->b_wptr += plen;
		put(np->oq, mp);
		/* release reference from lookup */
		np_put(np);
		return (0);
	      flow_controlled:
		np_rstat.ms_ccnt++;
		freeb(mp);	/* will take sk_buff with it */
		np_put(np);
		return (0);
	}
      no_buffers:
      linear_fail:
	if (np)
		np_put(np);
	kfree_skb(skb);
	return (0);
      no_stream:
	ptrace(("ERROR: No stream\n"));
      bad_pkt_type:
      too_small:
	if (np_v4_rcv_next(skb)) {
		/* TODO: want to generate an ICMP error here */
	}
	return (0);
}

/**
 * np_v4_err - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information (unused)
 *
 * This function is a network protocol callback that is invoked when transport specific ICMP errors
 * are received.  The function looks up the Stream and, if found, wraps the packet in an M_CTL
 * message and passes it to the read queue of the Stream.
 *
 * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned packet, and IP payload
 * of returned packet (up to some number of bytes of total payload).  The passed in sk_buff has
 * skb->data pointing to the ICMP payload which is the beginning of the returned IP header.
 * However, we include the entire packet in the message.
 *
 * LOCKING: master.lock protects the master list and protects from open, close, link and unlink.
 * np->qlock protects the state of private structure.  np->refs protects the private structure
 * from being deallocated before locking.
 */
STATIC __unlikely void
np_v4_err(struct sk_buff *skb, u32 info)
{
	struct np *np;
	struct iphdr *iph = (struct iphdr *) skb->data;

#if 0
/* icmp.c does this for us */
#define ICMP_MIN_LENGTH 4
	if (skb->len < (iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
#endif
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	if ((np = np_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (np_get_state(np) == NS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		queue_t *q;
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* Create a queue a specialized M_CTL message to the Stream's read queue for
		   further processing.  The Stream will convert this message into a N_UDERROR_IND,
		   N_RESET_IND or N_DISCON_IND message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* check flow control only after we have a buffer */
		if ((q = np->oq) == NULL || !bcanput(q, 1))
			goto flow_controlled;
		mp->b_datap->db_type = M_CTL;
		mp->b_band = 1;
		bcopy(skb_network_header(skb), mp->b_wptr, plen);
		mp->b_wptr += plen;
		put(q, mp);
		goto discard_put;
	      flow_controlled:
		np_rstat.ms_ccnt++;
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	np_put(np);
	np_v4_err_next(skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	np_v4_err_next(skb, info);
#if 0
	goto drop;
      drop:
#endif
#ifdef HAVE_KINC_LINUX_SNMP_H
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

/*
 *  Private structure allocation and deallocation.
 */

/**
 * np_alloc_priv - allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @slp: pointer to place in list for insertion
 * @type: type of provider: 0, N_CONS, N_CLNS
 * @devp: pointer to device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initializes it to appropriate values, and then inserts it into
 * the private structure list.
 */
noinline struct np *
np_alloc_priv(queue_t *q, struct np **npp, int type, dev_t *devp, cred_t *crp)
{
	struct np *np;

	if ((np = np_alloc())) {
		/* np generic members */
		np->u.dev.cmajor = getmajor(*devp);
		np->u.dev.cminor = getminor(*devp);
		np->cred = *crp;
		(np->oq = q)->q_ptr = np_get(np);
		(np->iq = WR(q))->q_ptr = np_get(np);
		np->i_prim = &np_w_prim;
		np->o_prim = &np_r_prim;
		// np->i_wakeup = NULL;
		// np->o_wakeup = NULL;
		spin_lock_init(&np->qlock);	/* "np-queue-lock" */
		// np->ibid = 0;
		// np->obid = 0;
		// np->iwait = NULL;
		// np->owait = NULL;
		np->i_state = (-1);	// LMI_UNUSABLE;
		np->i_style = (1);	// LMI_STYLE1;
		np->i_version = 1;
		np->i_oldstate = (1);	// LMI_UNUSABLE;
		np->type = type;
		/* np specific members */
		np->info.PRIM_type = N_INFO_ACK;
		np->info.NSDU_size = 65535 - sizeof(struct iphdr);
		np->info.ENSDU_size = 0;
		np->info.CDATA_size = 65535 - sizeof(struct iphdr);
		np->info.DDATA_size = 65535 - sizeof(struct iphdr);
		np->info.ADDR_size = sizeof(struct sockaddr_storage);
		np->info.ADDR_length = 0;
		np->info.ADDR_offset = 0;
		np->info.QOS_length = 0;
		np->info.QOS_offset = 0;
		np->info.QOS_range_length = 0;
		np->info.QOS_range_offset = 0;
		np->info.OPTIONS_flags = 0;
		np->info.NIDU_size = 65535 - sizeof(struct iphdr);
		np->info.SERV_type = type ? : (N_CLNS | N_CONS);
		np->info.CURRENT_state = NS_UNBND;
		np->info.PROVIDER_type = N_SUBNET;
		np->info.NODU_size = 536;
		np->info.PROTOID_length = 0;
		np->info.PROTOID_offset = 0;
		np->info.NPI_version = N_CURRENT_VERSION;
		bufq_init(&np->conq);
		bufq_init(&np->datq);
		bufq_init(&np->resq);
		/* qos defaults */
		np->qos.n_qos_type = N_QOS_SEL_INFO_IP;
		np->qos.protocol = QOS_UNKNOWN;
		np->qos.priority = 0;
		np->qos.ttl = 64;
		np->qos.tos = 0;
		np->qos.mtu = 65535;
		np->qos.saddr = 0;
		np->qos.daddr = QOS_UNKNOWN;
		/* qos range info */
		np->qor.n_qos_type = N_QOS_RANGE_INFO_IP;
		np->qor.priority.priority_min_value = 0;
		np->qor.priority.priority_max_value = 255;
		np->qor.ttl.ttl_min_value = 1;
		np->qor.ttl.ttl_max_value = 255;
		np->qor.tos.tos_min_value = 0;
		np->qor.tos.tos_max_value = 255;
		np->qor.mtu.mtu_min_value = 536;
		np->qor.mtu.mtu_max_value = 65535;
		/* link into master list */
		np_get(np);
		if ((np->next = *npp))
			np->next->prev = &np->next;
		np->prev = npp;
		*npp = np;
	} else
#if 0
		strlog(DRV_ID, getminor(*devp), 0, SL_WARN | SL_CONSOLE,
		       "could not allocate driver private structure");
#else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
#endif
	return (np);
}

/**
 * np_free_priv - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
noinline void
np_free_priv(queue_t *q)
{
	struct np *np;

	ensure(q, return);
	np = NP_PRIV(q);
	ensure(np, return);
#if 0
	strlog(DRV_ID, np->u.dev.cminor, 0, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&np->refcnt));
#else
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&np->refcnt)));
#endif
	/* make sure the stream is disconnected */
	np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
	/* make sure the stream is unbound */
	np_unbind(np);
	if (np->daddrs[0].dst != NULL) {
		dst_release(np->daddrs[0].dst);
		np->daddrs[0].dst = NULL;
	}
	bufq_purge(&np->conq);
	bufq_purge(&np->datq);
	bufq_purge(&np->resq);
	np_unbufcall((str_t *) np);
#if 0
	strlog(DRV_ID, np->u.dev.cminor, 0, SL_TRACE, "removed bufcalls: reference count = %d",
	       atomic_read(&np->refcnt));
#else
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME,
		atomic_read(&np->refcnt)));
#endif
	/* remove from master list */
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	np_put(np);
#if 0
	strlog(DRV_ID, np->u.dev.cminor, 0, SL_TRACE, "unlinked: reference count = %d",
	       atomic_read(&np->refcnt));
#else
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&np->refcnt)));
#endif
	np_release((struct np **) &np->oq->q_ptr);
	np->oq = NULL;
	np_release((struct np **) &np->iq->q_ptr);
	np->iq = NULL;
	assure(atomic_read(&np->refcnt) == 1);
	np_release(&np);	/* should normally be final put */
	return;
}

/*
 *  Open and Close
 */
#define FIRST_CMINOR	0
#define    IP_CMINOR	0
#define UDPCO_CMINOR	N_CONS
#define UDPCL_CMINOR	N_CLNS
#define  LAST_CMINOR	2
#define  FREE_CMINOR	3
STATIC int np_majors[CMAJORS] = { CMAJOR_0, };

/**
 * np_qopen - NPI IP driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct np *np, **npp = &master.np.list;
	unsigned long flags;
	mblk_t *mp;
	struct stroptions *so;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	/* Linux Fast-STREAMS always passes internal major device number (module id).  Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
#if defined LIS
	if (cmajor != CMAJOR_0)
		return (ENXIO);
#endif
#if defined LFS
	/* Linux Fast-STREAMS always passes internal major device numbers (modules ids) */
	if (cmajor != DRV_ID)
		return (ENXIO);
#endif
	/* sorry, you can't open by minor device */
	if (cminor > LAST_CMINOR) {
		return (ENXIO);
	}
	type = cminor;
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = FREE_CMINOR;
	if (!(mp = allocb(sizeof(*so), BPRI_MED)))
		return (ENOBUFS);
	write_lock_str2(&master.lock, flags);
	for (; *npp; npp = &(*npp)->next) {
		if (cmajor != (*npp)->u.dev.cmajor)
			break;
		if (cmajor == (*npp)->u.dev.cmajor) {
			if (cminor < (*npp)->u.dev.cminor)
				break;
			if (cminor == (*npp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = np_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_str2(&master.lock, flags);
		freeb(mp);
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(np = np_alloc_priv(q, npp, type, devp, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_str2(&master.lock, flags);
		freeb(mp);
		return (ENOMEM);
	}
	write_unlock_str2(&master.lock, flags);
	so = (typeof(so)) mp->b_wptr;
	bzero(so, sizeof(*so));
#if defined LFS
	so->so_flags |= SO_SKBUFF;
#endif
	/* want to set a write offet of MAX_HEADER bytes */
	so->so_flags |= SO_WROFF;
	so->so_wroff = MAX_HEADER;	/* this is too big */
	so->so_flags |= SO_MINPSZ;
	so->so_minpsz = np_minfo.mi_minpsz;
	so->so_flags |= SO_MAXPSZ;
	so->so_maxpsz = np_minfo.mi_maxpsz;
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = np_minfo.mi_hiwat;
	so->so_flags |= SO_LOWAT;
	so->so_lowat = np_minfo.mi_lowat;
	mp->b_wptr += sizeof(*so);
	mp->b_datap->db_type = M_SETOPTS;
	qprocson(q);
	putnext(q, mp);
	return (0);
}

/**
 * np_qclose - NPI UDP driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
STATIC streamscall int
np_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct np *np = NP_PRIV(q);

	(void) oflag;
	(void) crp;
	(void) np;
	_printd(("%s: closing character device %d:%d\n", DRV_NAME, np->u.dev.cmajor,
		 np->u.dev.cminor));
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	goto skip_pop;
      skip_pop:
	/* make sure procedures are off */
	flushq(WR(q), FLUSHALL);
	flushq(RD(q), FLUSHALL);
	qprocsoff(q);
	np_free_priv(q);	/* free and unlink the structure */
	goto quit;
      quit:
	return (0);
}

/*
 *  Private structure reference counting, allocation, deallocation and cache
 */
STATIC __unlikely int
np_term_caches(void)
{
	if (np_udp_prot_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_udp_prot_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_udp_prot_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(np_udp_prot_cachep);
#endif
		_printd(("%s: destroyed np_udp_prot_cachep\n", DRV_NAME));
		np_udp_prot_cachep = NULL;
	}
	if (np_udp_priv_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_udp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_udp_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(np_udp_priv_cachep);
#endif
		_printd(("%s: destroyed np_udp_priv_cachep\n", DRV_NAME));
		np_udp_priv_cachep = NULL;
	}
	return (0);
}
STATIC __unlikely int
np_init_caches(void)
{
	if (np_udp_priv_cachep == NULL) {
		np_udp_priv_cachep =
		    kmem_create_cache("np_udp_priv_cachep", sizeof(struct np), 0,
				      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_udp_priv_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_udp_priv_cachep", __FUNCTION__);
			np_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (np_udp_prot_cachep == NULL) {
		np_udp_prot_cachep =
		    kmem_create_cache("np_udp_prot_cachep", sizeof(struct np_prot_bucket), 0,
				      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_udp_prot_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_udp_prot_cachep", __FUNCTION__);
			np_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver protocol structure cache\n", DRV_NAME));
	}
	return (0);
}

STATIC __unlikely void
np_term_hashes(void)
{
	if (np_bhash) {
		free_pages((unsigned long) np_bhash, np_bhash_order);
		np_bhash = NULL;
		np_bhash_size = 0;
		np_bhash_order = 0;
	}
	if (np_chash) {
		free_pages((unsigned long) np_chash, np_chash_order);
		np_chash = NULL;
		np_chash_size = 0;
		np_chash_order = 0;
	}
}
STATIC __unlikely void
np_init_hashes(void)
{
	int i;

	/* Start with just one page for each. */
	if (np_bhash == NULL) {
		np_bhash_order = 0;
		if ((np_bhash =
		     (struct np_bhash_bucket *) __get_free_pages(GFP_ATOMIC, np_bhash_order))) {
			np_bhash_size =
			    (1 << (np_bhash_order + PAGE_SHIFT)) / sizeof(struct np_bhash_bucket);
			_printd(("%s: INFO: bind hash table configured size = %ld\n", DRV_NAME,
				 (long) np_bhash_size));
			bzero(np_bhash, np_bhash_size * sizeof(struct np_bhash_bucket));
			for (i = 0; i < np_bhash_size; i++)
				rwlock_init(&np_bhash[i].lock);
		} else {
			np_term_hashes();
			cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
			return;
		}
	}
	if (np_chash == NULL) {
		np_chash_order = 0;
		if ((np_chash =
		     (struct np_chash_bucket *) __get_free_pages(GFP_ATOMIC, np_chash_order))) {
			np_chash_size =
			    (1 << (np_chash_order + PAGE_SHIFT)) / sizeof(struct np_chash_bucket);
			_printd(("%s: INFO: conn hash table configured size = %ld\n", DRV_NAME,
				 (long) np_chash_size));
			bzero(np_chash, np_chash_size * sizeof(struct np_chash_bucket));
			for (i = 0; i < np_chash_size; i++)
				rwlock_init(&np_chash[i].lock);
		} else {
			np_term_hashes();
			cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
			return;
		}
	}
}

/*
 *  Registration and initialization
 */
#ifdef LINUX
/*
 *  Linux registration
 */

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the UDP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the UDP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 */
#ifdef LFS

STATIC struct cdevsw np_cdev = {
	.d_name = DRV_NAME,
	.d_str = &np_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC struct devnode np_node_udp = {
	.n_name = "udp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode np_node_udpco = {
	.n_name = "udpco",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode np_node_udpcl = {
	.n_name = "udpcl",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC __unlikely int
np_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&np_cdev, major)) < 0)
		return (err);
	register_strnod(&np_cdev, &np_node_udp, UDP_CMINOR);
	register_strnod(&np_cdev, &np_node_udpco, UDPCO_CMINOR);
	register_strnod(&np_cdev, &np_node_udpcl, UDPCL_CMINOR);
	return (0);
}

STATIC __unlikely int
np_unregister_strdev(major_t major)
{
	int err;

	unregister_strnod(&np_cdev, UDP_CMINOR);
	unregister_strnod(&np_cdev, UDPCO_CMINOR);
	unregister_strnod(&np_cdev, UDPCL_CMINOR);
	if ((err = unregister_strdev(&np_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 */
#ifdef LIS

STATIC __unlikely int
np_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &np_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

STATIC __unlikely int
np_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
udpterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (np_majors[mindex]) {
			if ((err = np_unregister_strdev(np_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					np_majors[mindex]);
			if (mindex)
				np_majors[mindex] = 0;
		}
	}
	if ((err = np_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	np_term_hashes();
	return;
}

MODULE_STATIC int __init
udpinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	np_init_hashes();
	if ((err = np_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		udpterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = np_register_strdev(np_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					np_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				udpterminate();
				return (err);
			}
		}
		if (np_majors[mindex] == 0)
			np_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(np_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = np_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(udpinit);
module_exit(udpterminate);

#endif				/* LINUX */
