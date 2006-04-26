/*****************************************************************************

 @(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/04/26 10:47:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/04/26 10:47:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip.c,v $
 Revision 0.9.2.22  2006/04/26 10:47:53  brian
 - sync

 Revision 0.9.2.21  2006/04/26 04:04:13  brian
 - sync

 Revision 0.9.2.20  2006/04/26 00:52:36  brian
 - sync

 Revision 0.9.2.19  2006/04/25 22:28:59  brian
 - working up NPI-IP driver

 Revision 0.9.2.18  2006/04/24 05:00:33  brian
 - call interface corrections

 Revision 0.9.2.17  2006/04/03 10:57:25  brian
 - need attributes on definition as well as declaration

 Revision 0.9.2.16  2006/03/31 12:36:46  brian
 - working up ip driver

 Revision 0.9.2.15  2006/03/30 12:51:56  brian
 - corrections for x64_64 compile

 Revision 0.9.2.14  2006/03/30 10:47:33  brian
 - working up NPI IP test and mux driver

 Revision 0.9.2.13  2006/03/27 01:25:54  brian
 - working up IP driver and SCTP testing

 Revision 0.9.2.12  2006/03/24 05:10:08  brian
 - 64-bit corrections

 Revision 0.9.2.11  2006/03/23 12:16:16  brian
 - changes for old 2.4 kernel (RH7) build

 Revision 0.9.2.10  2006/03/20 23:10:31  brian
 - corrected errors

 Revision 0.9.2.9  2006/03/20 12:16:45  brian
 - working up IP driver

 Revision 0.9.2.8  2006/03/18 09:42:11  brian
 - added in ip driver and manual pages

 Revision 0.9.2.7  2006/03/18 00:15:21  brian
 - syncing notebook

 Revision 0.9.2.6  2006/01/02 12:00:44  brian
 - working up IP driver

 Revision 0.9.2.5  2005/12/28 10:01:50  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.4  2005/07/18 12:40:29  brian
 - standard indentation

 Revision 0.9.2.3  2005/07/05 22:46:14  brian
 - change for strcompat package

 Revision 0.9.2.2  2005/06/19 09:27:33  brian
 - working up IP NPI driver

 Revision 0.9.2.1  2005/06/16 20:41:45  brian
 - Start of the IP NPI driver.

 *****************************************************************************/

#ident "@(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/04/26 10:47:53 $"

static char const ident[] =
    "$RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/04/26 10:47:53 $";

/*
   This driver provides the functionality of an IP (Internet Protocol) hook similar to raw sockets,
   with the exception that the hook acts as a port bound intercept for IP packets for the bound
   protocol ids.  This dirver is used primarily by OpenSS7 protocol test module (e.g. for SCTP) and
   for applications where entire ranges of port numbers for an existing protocol id must be
   intercepted (e.g. for RTP/RTCP).  This driver uses  hook into the Linux IP protocol tables and
   passes packets tranparently on to the underlying protocol in which it is not interested (bound).
   The driver uses the NPI (Network Provider Interface) API.
*/

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>
#include <linux/interrupt.h>

#undef socklen_t
typedef unsigned int socklen_t;

#define socklen_t socklen_t

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <net/ip.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/snmp.h>

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#endif				/* LINUX */

#include "ip_hooks.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>

#define IP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define IP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define IP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define IP_REVISION	"OpenSS7 $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/04/26 10:47:53 $"
#define IP_DEVICE	"SVR 4.2 STREAMS NPI IP Driver"
#define IP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define IP_LICENSE	"GPL"
#define IP_BANNER	IP_DESCRIP	"\n" \
			IP_EXTRA	"\n" \
			IP_REVISION	"\n" \
			IP_COPYRIGHT	"\n" \
			IP_DEVICE	"\n" \
			IP_CONTACT
#define IP_SPLASH	IP_DESCRIP	"\n" \
			IP_REVISION

#ifdef LINUX
MODULE_AUTHOR(IP_CONTACT);
MODULE_DESCRIPTION(IP_DESCRIP);
MODULE_SUPPORTED_DEVICE(IP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(IP_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ip");
#endif
#endif				/* LINUX */

#ifdef LFS
#define IP_DRV_ID	CONFIG_STREAMS_IP_MODID
#define IP_DRV_NAME	CONFIG_STREAMS_IP_NAME
#define IP_CMAJORS	CONFIG_STREAMS_IP_NMAJORS
#define IP_CMAJOR_0	CONFIG_STREAMS_IP_MAJOR
#define IP_UNITS	CONFIG_STREAMS_IP_NMINORS
#endif

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_IP_MODID));
MODULE_ALIAS("streams-driver-ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_IP_MAJOR));
MODULE_ALIAS("/dev/streams/ip");
MODULE_ALIAS("/dev/streams/ip/*");
MODULE_ALIAS("/dev/streams/clone/ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0) "-" __stringify(IP_CMINOR));
MODULE_ALIAS("/dev/ip");
MODULE_ALIAS("/dev/inet/ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		IP_DRV_ID
#define DRV_NAME	IP_DRV_NAME
#define CMAJORS		IP_CMAJORS
#define CMAJOR_0	IP_CMAJOR_0
#define UNITS		IP_UNITS
#ifdef MODULE
#define DRV_BANNER	IP_BANNER
#else				/* MODULE */
#define DRV_BANNER	IP_SPLASH
#endif				/* MODULE */

STATIC struct module_info ip_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC streamscall int ip_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ip_qclose(queue_t *, int, cred_t *);

STATIC struct qinit ip_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = ip_qopen,		/* Each open */
	.qi_qclose = ip_qclose,		/* Last close */
	.qi_minfo = &ip_minfo,		/* Information */
};

STATIC struct qinit ip_winit = {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_minfo = &ip_minfo,		/* Information */
};

MODULE_STATIC struct streamtab ip_info = {
	.st_rdinit = &ip_rinit,		/* Upper read queue */
	.st_wrinit = &ip_winit,		/* Upper write queue */
};

/*
 *  Primary data structures.
 */

struct ip_bhash_bucket;
struct ip_chash_bucket;

typedef struct ip_daddr {
	uint32_t addr;			/* IP address this destination */
	unsigned char ttl;		/* time to live, this destination */
	unsigned char tos;		/* type of service, this destination */
	unsigned short mtu;		/* maximum transfer unit this destination */
} ip_daddr_t;

typedef struct ip_saddr {
	uint32_t addr;			/* IP address this source */
} ip_saddr_t;

typedef struct ip_baddr {
	uint32_t addr;			/* IP address this bind */
} ip_baddr_t;

typedef struct ip {
	STR_DECLARATION (struct ip);	/* Stream declaration */
	unsigned char priority;		/* message priority */
	unsigned char protocol;		/* primary protocol */
	np_ulong provider;		/* provider type */
	struct ip *bnext;		/* linkage for bind/list hash */
	struct ip **bprev;		/* linkage for bind/list hash */
	struct ip_bhash_bucket *bindb;	/* linkage for bind/list hash */
	struct ip *cnext;		/* linkage for conn hash */
	struct ip **cprev;		/* linkage for conn hash */
	struct ip_chash_bucket *connb;	/* linkage for conn hash */
	N_info_ack_t info;		/* network service provider information */
	N_qos_sel_info_ip_t qos;	/* network service provider quality of service */
	unsigned int maxinds;		/* maximum number of oustanding connection indications */
	unsigned int coninds;		/* number of outstanding connection indications */
	mblk_t *conq;			/* connection indication queue */
	unsigned int datinds;		/* number of outstanding data indications */
	mblk_t *resq;			/* reset indication queue */
	unsigned int resinds;		/* number of outstanding reset indications */
	mblk_t *datq;			/* data indication queue */
	unsigned short pnum;		/* number of bound protocol ids */
	uint8_t protoids[16];		/* bound protocol ids */
	unsigned short bnum;		/* number of bound addresses */
	unsigned short bport;		/* bound port number - network order */
	struct ip_baddr baddrs[8];	/* bound addresses */
	unsigned short snum;		/* number of source (connected) addresses */
	unsigned short sport;		/* source (connected) port number - network order */
	struct ip_saddr saddrs[8];	/* source (connected) addresses */
	unsigned short dnum;		/* number of destination (connected) addresses */
	unsigned short dport;		/* destination (connected) port number - network order */
	struct ip_daddr daddrs[8];	/* destination (connected) addresses */
} ip_t;

#define PRIV(__q) (((__q)->q_ptr))
#define IP_PRIV(__q) ((struct ip *)((__q)->q_ptr))

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (ip, ip);		/* master list of ip (open) structures */
} df_t;

STATIC struct df master = {.lock = RW_LOCK_UNLOCKED, };

typedef int (*ip_rcv_fnc_t) (struct sk_buff *);

/*
 *  Bind buckets, caches and hashes.
 */
struct ip_bind_bucket {
	struct ip_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct ip_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short port;		/* port number (host order) */
	struct ip *owners;		/* list of owners of this protocol/port combination */
	struct ip *dflt;		/* default listeners/destinations for this protocol */
};
struct ip_conn_bucket {
	struct ip_conn_bucket *next;	/* linkage of conn buckets for hash slot */
	struct ip_conn_bucket **prev;	/* linkage of conn buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short sport;		/* source port number (network order) */
	unsigned short dport;		/* destination port number (network order) */
	struct ip *owners;		/* list of owners of this protocol/sport/dport combination */
};

struct ip_bhash_bucket {
	rwlock_t lock;
	struct ip *list;
};
struct ip_chash_bucket {
	rwlock_t lock;
	struct ip *list;
};

rwlock_t ip_hash_lock = RW_LOCK_UNLOCKED;

#ifdef LINUX
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
struct inet_protocol {
	struct net_protocol proto;
	struct net_protocol *next;
	struct module *kmod;
};
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTCCOL */
#endif				/* LINUX */

struct ip_prot_bucket {
	unsigned char proto;		/* protocol number */
	int refs;			/* reference count */
	int corefs;			/* N_CONS references */
	int clrefs;			/* N_CLNS references */
	struct inet_protocol prot;	/* Linux registration structure */
};
STATIC rwlock_t ip_prot_lock = RW_LOCK_UNLOCKED;
STATIC struct ip_prot_bucket *ip_prots[256];

STATIC kmem_cache_t *ip_bind_cachep;
STATIC kmem_cache_t *ip_prot_cachep;
STATIC kmem_cache_t *ip_priv_cachep;

STATIC INLINE struct ip *
ip_get(struct ip *ip)
{
	if (ip)
		atomic_inc(&ip->refcnt);
	return (ip);
}
STATIC INLINE void
ip_put(struct ip *ip)
{
	if (ip)
		if (atomic_dec_and_test(&ip->refcnt)) {
			kmem_cache_free(ip_priv_cachep, ip);
		}
}
STATIC INLINE struct ip *
ip_alloc(void)
{
	struct ip *ip;

	if ((ip = kmem_cache_alloc(ip_priv_cachep, SLAB_ATOMIC))) {
		bzero(ip, sizeof(*ip));
		atomic_set(&ip->refcnt, 1);
		spin_lock_init(&ip->lock);	/* "ip-lock" */
		ip->priv_put = &ip_put;
		ip->priv_get = &ip_get;
		// ip->type = 0;
		// ip->id = 0;
		// ip->state = 0;
		// ip->flags = 0;
	}
	return (ip);
}

/*
 *  State changes.
 */

/* State flags */
#define NSF_UNBND	(1 << NS_UNBND		)
#define NSF_WACK_BREQ	(1 << NS_WACK_BREQ	)
#define NSF_WACK_UREQ	(1 << NS_WACK_UREQ	)
#define NSF_IDLE	(1 << NS_IDLE		)
#define NSF_WACK_OPTREQ	(1 << NS_WACK_OPTREQ	)
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
#define NSF_ALLSTATES	(NSF_NOSTATES - 1)
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

#ifdef DEBUG
STATIC const char *
state_name(np_ulong state)
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
#endif

STATIC INLINE fastcall void
npi_set_state(struct ip *ip, np_ulong state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, ip, state_name(state), state_name(ip->i_state)));
	ip->i_state = state;
}

STATIC INLINE fastcall np_ulong
npi_get_state(struct ip *ip)
{
	return (ip->i_state);
}

STATIC INLINE fastcall np_ulong
npi_chk_state(struct ip * ip, np_ulong mask)
{
	return (((1 << ip->i_state) & (mask)) != 0);
}

STATIC INLINE fastcall np_ulong
npi_not_state(struct ip * ip, np_ulong mask)
{
	return (((1 << ip->i_state) & (mask)) == 0);
}

/*
 *  =========================================================================
 *
 *  IP Local Management
 *
 *  =========================================================================
 */

STATIC int ip_v4_rcv(struct sk_buff *skb);
STATIC void ip_v4_err(struct sk_buff *skb, u32 info);

/*
 *  IP subsystem management
 */
#ifdef LINUX
/**
 * npi_v4_steal - steal a socket buffer
 * @skb: socket buffer to steal
 *
 * In the 2.4 packet handler, if the packet is for us, steal the packet by overwritting the protocol
 * and returning.  This is only done for normal packets and not error packets (that do not need to
 * be stolen).  In the 2.4 handler loop, iph->protocol is examined on each iteration, permitting us
 * to steal the packet by overwritting the protocol number.
 *
 * In the 2.6 packet handler, if the packet is not for us, steal the packet by simply not passing it
 * to the next handler.
 */
STATIC INLINE fastcall __hot_in void
npi_v4_steal(struct sk_buff *skb)
{
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
	skb->nh.iph->protocol = 255;
	skb->protocol = 255;
#endif				/* HAVE_KTYPE_STRUCT_INET_PROTOCOL */
}

/**
 * npi_v4_rcv_next - pass a socket buffer to the next handler
 * @skb: socket buffer to pass
 *
 * In the 2.6 packet handler, if the packet is not for us, pass it to the next handler.  If there is
 * no next handler, free the packet and return.  Note that we do not have to lock the hash because
 * we own it and are also holding a reference to any module owning the next handler.
 *
 * In the 2.4 packet handler, if the packet is not for us, pass it to the next handler by simply
 * freeing the cloned copy and returning.
 */
STATIC INLINE fastcall __hot_in int
npi_v4_rcv_next(struct sk_buff *skb)
{
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
	struct ip_prot_bucket *pb;
	struct net_protocol *np;
	unsigned char proto;

	proto = skb->nh.iph->protocol;
	if ((pb = ip_prots[proto])
	    && (np = pb->prot.next))
		return (np->handler(skb));
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
	kfree_skb(skb);
	return (0);
}

/**
 * npi_v4_err_next - pass a socket buffer to the next error handler
 * @skb: socket buffer to pass
 *
 * In the 2.6 packet error handler, if the packet is not for us, pass it to the next error handler.
 * If there is no next error handler, simply return.
 *
 * In the 2.4 packet error handler, if the packet is not for us, pass it to the next error handler
 * by simply returning.  Error packets are not cloned, so don't free it.
 */
STATIC INLINE fastcall __hot_in void
npi_v4_err_next(struct sk_buff *skb, __u32 info)
{
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
	struct ip_prot_bucket *pb;
	struct net_protocol *np;
	unsigned char proto;

	proto = ((struct iphdr *) skb->data)->protocol;
	if ((pb = ip_prots[proto])
	    && (np = pb->prot.next))
		np->err_handler(skb, info);
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
	return;
}

#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;
STATIC struct net_protocol **inet_protosp = (typeof(inet_protosp)) HAVE_INET_PROTOS_ADDR;
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */

/**
 * npi_init_nproto - initialize network protocol override
 * @proto: the protocol to register or override
 *
 * This is the network protocol override function.
 *
 * Under 2.4, simply add the protocol to the network using an inet_protocol structure and the
 * inet_add_protocol() function.  Each added function will be delivered a clone of the packet in an
 * sk_buff, which is fine.
 *
 * Under 2.6, things are more complicated.  2.6 will refuse to register a network protocol if one
 * already exists, so we hack the 2.6 tables.  If no other protocol was previously registered, this
 * reduces to the 2.6 version of inet_add_protocol().  If there is a protocol previously registered,
 * we take a reference on the kernel module owning the entry, if possible, and replace the entry
 * with our own, saving a pointer to the previous entry for passing sk_bufs along that we are not
 * interested in.  Taking a module reference is particularly for things like SCTP, where unloading
 * the module after protocol override would break things horribly.  Taking the reference keeps the
 * module from unloading (this works for OpenSS7 SCTP as well as lksctp).
 */
STATIC struct ip_prot_bucket *
npi_init_nproto(unsigned char proto, unsigned int type)
{
	struct ip_prot_bucket *pb;
	struct inet_protocol *ip;
	int hash = proto & (MAX_INET_PROTOS - 1);

	write_lock_bh(&ip_prot_lock);
	if ((pb = ip_prots[proto]) != NULL) {
		pb->refs++;
		if (type & N_CONS)
			++pb->corefs;
		if (type & N_CLNS)
			++pb->clrefs;
	} else if ((pb = kmem_cache_alloc(ip_prot_cachep, SLAB_ATOMIC))) {
		pb->refs = 1;
		pb->corefs = (type & N_CONS) ? 1 : 0;
		pb->clrefs = (type & N_CLNS) ? 1 : 0;
		ip = &pb->prot;
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
		ip->protocol = proto;
		ip->name = "streams-ip";
		ip->handler = &ip_v4_rcv;
		ip->err_handler = &ip_v4_err;
		ip->copy = 0;
		ip->next = NULL;
		inet_add_protocol(ip, proto);
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO
		ip->proto.proto = proto;
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO */
		ip->proto.handler = &ip_v4_rcv;
		ip->proto.err_handler = &ip_v4_err;
		ip->proto.no_policy = 1;
		ip->next = NULL;
		ip->kmod = NULL;
		spin_lock_bh(&inet_proto_lockp);
		if ((ip->next = inet_protosp[hash]) != NULL) {
			if ((ip->kmod = module_text_address((ulong) ip->next))
			    && ip->kmod != THIS_MODULE) {
				if (!try_module_get(ip->kmod)) {
					spin_unlock_bh(inet_proto_lockp);
					kmem_cache_free(ip_prot_cachep, pb);
					return (NULL);
				}
			}
		}
		inet_protosp[hash] = &ip->proto;
		spin_unlock_bh(&inet_proto_lockp);
		synchronize_net();
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
		/* link into hash slot */
		ip_prots[proto] = pb;
	}
	write_unlock_bh(&ip_prot_lock);
	return (pb);
}

/**
 * npi_term_nproto - terminate network protocol override
 * @proto: network protocol to terminate
 *
 * This is the network protocol restoration function.
 *
 * Under 2.4, simply remove the protocol from the network using the inet_protocol structure and the
 * inet_del_protocol() function, and we stop receiving packets.
 * 
 * Under 2.6, things are more complicated.
 * The module stuff here is just for ourselves (other kernel modules pulling the same trick) as
 * Linux IP protocols are normally kernel resident.  If a protocol was previously registered,
 * restore the protocol's entry and drop the reference to its owning kernel module.  If there was no
 * protocol previously registered, this reduces to the 2.6 version of inet_del_protocol().
 */
STATIC void
npi_term_nproto(unsigned char proto, unsigned int type)
{
	struct ip_prot_bucket *pb;

	write_lock_bh(&ip_prot_lock);
	if ((pb = ip_prots[proto]) != NULL) {
		if (type & N_CONS)
			--pb->corefs;
		if (type & N_CLNS)
			--pb->clrefs;
		if (--pb->refs == 0) {
			struct inet_protocol *ip = &pb->prot;

#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
			inet_del_protocol(ip, proto);
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
			spin_lock_bh(inet_proto_lockp);
			inet_protosp[proto] = ip->next;
			spin_unlock_bh(inet_proto_lockp);
			synchronize_net();
			if (ip->next != NULL && ip->kmod != NULL && ip->kmod != THIS_MODULE)
				module_put(ip->kmod);
#else
#error
#endif
			/* unlink from hash slot */
			ip_prots[proto] = NULL;
			kmem_cache_free(ip_prot_cachep, pb);
		}
	}
	write_unlock_bh(&ip_prot_lock);
}
#endif				/* LINUX */

/**
 *  ip_bind_prot -  bind a protocol
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
STATIC int
ip_bind_prot(unsigned char proto, unsigned int type)
{
	struct ip_prot_bucket *pb;

	if ((pb = npi_init_nproto(proto, type)))
		return (0);
	return (-ENOMEM);
}

/**
 *  ip_unbind_prot - unbind a protocol
 *  @proto:	    protocol number to unbind
 */
STATIC void
ip_unbind_prot(unsigned char proto, unsigned int type)
{
	npi_term_nproto(proto, type);
}

/**
 *  ip_bind_addr -  bind an address and protocol id to a Stream
 *  @ip:	    Stream to which to bind
 *  @add:	    sockaddr_in structure with address to bind
 *  @proto:	    protocol id to bind
 *  @type:	    type of bind (N_CLNS|N_CONS)
 *  @flag:	    default bind
 */
STATIC int
ip_bind_addr(struct ip *ip, struct sockaddr_in *addr, unsigned char proto, unsigned int type,
	     int flag)
{
	int err;

	if ((err = ip_bind_prot(proto, type)) != 0)
		goto error;
	/* FIXME: now bind to address... */
      error:
	return (err);
}

/**
 *  ip_unbind_addr - unbind an address and protocol id from a Stream
 *  @ip:		    Stream to which to unbind
 *  @add:	    sockaddr_in structure with address to unbind
 *  @proto:	    protocl id to unbind
 *  @type:	    type of bind (N_CLNS|N_CONS)
 *  @flag:	    default unbind
 */
STATIC void
ip_unbind_addr(struct ip *ip, struct sockaddr_in *addr, unsigned char proto, unsigned int type,
	       int flag)
{
	/* FIXME: unbind from address... */
	ip_unbind_prot(proto, type);
}

/**
 * ip_bind - bind to addresses and protocol ids
 * @q:		    Stream to which to bind
 * @add_in:	    array of sockaddr_in structures to bind
 * @anum:	    number of elements in sockaddr_in array
 * @protoids:	    array of protocol ids to which to bind
 * @pnum:	    number of elements in protocol id array
 * @flags:	    bind flags
 *
 * If the number of connection indications is zero, the bind is performed on the bind hashes.  If
 * non-zero, the bind is performed on the listen hashes and the Stream is marked for N_CONS.  If the
 * port number in the bound address(es) is non-zero, whether the Stream is listening or not, the
 * Stream is marked for N_CONS.
 *
 * When the port number in the bound address(es) is zero and the number of connection indications
 * are zero, the Stream can still be considered connectionless or connection-oriented.  If
 * TOKEN_REQUEST is specified, the Stream is always considered connection-oriented.  If
 * TOKEN_REQUEST is not specified in this situation (zero port number) the Stream is always
 * considered connectionless.
 *
 * If DEFAULT_LISTENER is specified, the Stream is always considered connection-oriented.  Each
 * protocol id an port number combination (whether the port number is zero or not) is permitted
 * multiple DEFAULT_LISTENER Streams (one for each IP address, and one for the wildcard IP address).
 * DEFAULT_LISTENER Streams are only considered when there is no match for any other listening
 * Stream.
 *
 * If DEFAULT_DEST is specified, the Stream is always considered connectionless.  Each protocol id
 * is permitted multiple DEFAULT_DEST Streams (one for each IP address, and one for the wildcard IP
 * address).  DEFAULT_DEST Streams are only considered when there is no match for any other
 * connectionless bound Stream that matches the destination address.
 *
 * Binding supports wildcard addresses, both in port number and IP address.
 * 
 *
 * The bind hash contains bind buckets that list Streams that are bound to the
 * same protocol id and port number.
 *  
 *  Binding is performed as a two-level hash.  The first hash level is on protocol id.  The second
 *  level hash is on socket address.
 *
 *  For the first level protocol hash, whenever a protocol id has not yet been bound by any Stream,
 *  we either register the protocol id with Linux inet, or, if the protocol id has already been
 *  registered, we hook on top of it by stealing the receive function of the installed protocol.
 *  One issue that must be surmounted is that if an existing registered protocol is a loadable
 *  kernel module, we need to take a reference to the kernel module to keep it from unloading while
 *  we are hooked on top of it.  Hopefully we will be able to find the module structure for the
 *  registered module to accomodate this.  We actually only allow one and only one protcol id for
 *  now.
 */
STATIC int
ip_bind(struct ip *ip, struct sockaddr_in *add_in, size_t anum, unsigned char *protoids,
	size_t pnum, ulong coninds, ulong flags)
{
	int i, j, err = 0;

	for (i = 0; i < pnum; i++) {
		unsigned char proto = protoids[i];
		unsigned short port = 0;

		if (anum > 0) {
			for (j = 0; j < anum; j++) {
				struct sockaddr_in *add = &add_in[j];

				if (j == 0)
					port = add->sin_port;
				else
					add->sin_port = port;

				/* FIXME: what flag? what type? */
				if ((err = ip_bind_addr(ip, add, proto, N_CONS, 0)))
					break;
			}
			if (err == 0)
				continue;
			for (; j >= 0; j--) {
				struct sockaddr_in *add = &add_in[j];

				/* FIXME: what flag? what type? */
				ip_unbind_addr(ip, add, proto, N_CONS, 0);
			}
			break;
		} else {
			/* no addresses means wildcard bind */
			struct sockaddr_in add = {
				.sin_family = AF_INET,
				.sin_port = 0,
				.sin_addr = {.s_addr = 0,},
			};

			/* FIXME: what flag? what type? */
			if ((err = ip_bind_addr(ip, &add, proto, N_CONS, 0)))
				continue;
			break;
		}
	}
	if (err == 0)
		return (0);
	/* unregister fully registered protocol ids */
	for (i--; i >= 0; i--) {
		unsigned char proto = protoids[i];

		if (anum > 0) {
			for (j = 0; j < anum; j++) {
				struct sockaddr_in *add = &add_in[j];

				/* FIXME: what flag? what type? */
				ip_unbind_addr(ip, add, proto, N_CONS, 0);
			}
		} else {
			/* no addresses means wildcard bind */
			struct sockaddr_in add = {
				.sin_family = AF_INET,
				.sin_port = 0,
				.sin_addr = {.s_addr = 0,},
			};

			/* FIXME: what flag? what type? */
			ip_unbind_addr(ip, &add, proto, N_CONS, 0);
		}
	}
	return (err);
}

STATIC int
ip_connect(struct ip *ip, struct sockaddr_in *dst, size_t dnum, N_qos_sel_conn_ip_t * qos)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
ip_reset(struct ip *ip, struct sockaddr_in *dst, np_ulong orig, np_ulong reason)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
ip_optmgmt(struct ip *ip, void *qos, np_ulong flags)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
ip_unbind(struct ip *ip)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
ip_passive(struct ip *ip, struct sockaddr_in *dst, socklen_t anum, void *qos, mblk_t *seq,
	   struct ip *tok, np_ulong flags)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
ip_disconnect(struct ip *ip, struct sockaddr_in *res, mblk_t *seq, np_ulong reason)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  IP Message Transmission.
 *
 *  =========================================================================
 */
#ifdef HAVE_KFUNC_DST_OUTPUT
STATIC INLINE int
npi_ip_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;

#ifdef NETIF_F_TSO
	ip_select_ident_more(iph, &rt->u.dst, NULL, 0);
#else
	ip_select_ident(iph, &rt->u.dst, NULL);
#endif
	ip_send_check(iph);
#ifdef HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, ip_dst_output);
#else
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, dst_output);
#endif
}
#else
#ifdef HAVE_KFUNC_DST_MTU
/* Why do stupid people rename things like this? */
#undef dst_pmtu
#define dst_pmtu dst_mtu
#endif
#ifndef HAVE_STRUCT_DST_ENTRY_PATH
static inline u32
dst_pmtu(struct dst_entry *dst)
{
	return (dst->pmtu);
}
#endif
STATIC INLINE int
npi_ip_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;

	if (skb->len > dst_pmtu(&rt->u.dst)) {
		rare();
		return ip_fragment(skb, skb->dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return skb->dst->output(skb);
	}
}
#endif

STATIC INLINE fastcall __hot_put int
ip_unitdata_req(queue_t *q, struct sockaddr_in *dst, struct N_qos_sel_ud_ip *qos, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	struct rtable *rt = NULL;
	struct iphdr *iph;

	int protocol;
	int priority;
	int tos;
	int ttl;
	uint32_t saddr;
	uint32_t daddr;

	size_t ilen;

	if (!dst) {
		/* no destination, packet includes IP header */
		iph = (struct iphdr *) mp->b_rptr;
		protocol = iph->protocol;
		priority = mp->b_band;
		tos = iph->tos;
		ttl = iph->ttl;
		saddr = iph->saddr;
		daddr = iph->daddr;
		ilen = (iph->ihl << 2);
	} else if (qos) {
		/* if QOS provided, use values from QOS structure */
		protocol = (qos->protocol != QOS_UNKNOWN) ? qos->protocol : ip->qos.protocol;
		priority = (qos->priority != QOS_UNKNOWN) ? qos->priority : ip->qos.priority;
		tos = (qos->tos != QOS_UNKNOWN) ? qos->tos : ip->qos.tos;
		ttl = (qos->ttl != QOS_UNKNOWN) ? qos->ttl : ip->qos.ttl;
		saddr = (qos->saddr != QOS_UNKNOWN) ? qos->saddr : ip->qos.saddr;
		daddr = dst->sin_addr.s_addr;
		ilen = 0;
	} else {
		protocol = ip->qos.protocol;
		priority = ip->qos.priority;
		tos = ip->qos.tos;
		ttl = ip->qos.ttl;
		saddr = 0;
		daddr = dst->sin_addr.s_addr;
		ilen = 0;
	}

	if (!ip_route_output(&rt, daddr, saddr, 0, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp) - ilen;
		size_t xlen = ilen ? ilen : sizeof(struct iphdr);
		size_t tlen = plen + xlen;

		usual(hlen);
		usual(plen);

		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			unsigned char *data;

			skb_reserve(skb, hlen);
			/* find headers */
			iph = (typeof(iph)) __skb_put(skb, tlen);
			data = (unsigned char *) iph + xlen;
			skb->dst = &rt->u.dst;
			skb->priority = priority;
			if (ilen) {
				/* copy and consume ip header (with options) if it was provided */
				bcopy(mp->b_rptr, iph, ilen);
				mp->b_rptr += ilen;
			}
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = tos;
			iph->ttl = ttl;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr ? saddr : rt->rt_src;
			iph->protocol = protocol;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
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
			/* IMPLEMENTATION NOTE:- The passed in mblk_t pointer is possibly a message
			   buffer chain and we must iterate along the b_cont pointer.  Rather than
			   copying at this point, it is probably a better idea to create a
			   fragmented sk_buff and just point to the elements.  Of course, we also
			   need an sk_buff destructor.  This is not done yet. */
			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;

				if (blen > 0) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
				} else
					rare();
			}
#ifdef HAVE_KFUNC_DST_OUTPUT
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, npi_ip_queue_xmit);
#else
			npi_ip_queue_xmit(skb);
#endif
		} else
			rare();
	} else
		rare();
	return (QR_DONE);
}

/*
 *  ===================================================================
 *
 *  NPI Provider --> NPI User (Transport) Primitives
 *
 *  ===================================================================
 */
STATIC int
ip_unbind_req(struct ip *ip)
{
	swerr();
	fixme(("write this function"));
	return (0);
}

/**
 * ne_error_reply: - reply to a message with an M_ERROR message
 * @q: active queue in queue pair (read queue)
 * @err: error number
 */
STATIC int
ne_error_reply(queue_t *q, long err)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;

	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	case 0:
	case 1:
	case 2:
		return (err);
	}
	if ((mp = ss7_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = (err < 0) ? -err : err;
		*(mp->b_wptr)++ = (err < 0) ? -err : err;
		putnext(ip->oq, mp);
		ip_unbind_req(ip);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * ne_info_ack: - generate an N_INFO_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 */
STATIC INLINE fastcall int
ne_info_ack(queue_t *q)
{
	struct ip *ip = IP_PRIV(q);
	int err, i;
	mblk_t *mp;
	N_info_ack_t *p;
	struct sockaddr_in *sin;
	N_qos_sel_info_ip_t *qos;
	N_qos_range_info_ip_t *qor;
	size_t add_len = ip->snum * sizeof(struct sockaddr_in);
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);
	size_t pro_len = ip->pnum * sizeof(uint8_t);

	if ((mp = ss7_allocb(q, sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = 65535;
		p->ENSDU_size = 0;
		p->CDATA_size = 65535;
		p->DDATA_size = 65535;
		p->ADDR_size = sizeof(struct sockaddr_storage);
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		/* QOS_length: In the connection-mode environment, when this primitive is invoked
		   before the NC is established on the stream, the values returned specify the
		   default values supported by the NS provider.  When this primitive is invoked
		   after a NC has been established on the stream, the values returned indicated
		   negotiated values for the QOS parameters.  In the connection-less environment,
		   these values represent the default or the selected QOS parameter values.  In
		   case a QOS parameter is not supported by the NS Provider, a value of QOS_UNKNOWN 
		   will be returned.  In the case where no QOS parameters are supported by the NS
		   provider, this field will be zero. */
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
		/* QOS_range_length: These ranges are used by the NS user to select QOS parameter
		   values that are valid with the NS provider.  QOS parameter values are selected,
		   or the default values altered via the N_OPTMGMT_REQ primitive.  In the
		   connection-mode environment, the values for end-to-end QOS parameter may be
		   specified with the N_CONN primitives for negotiation. If the NS provider does
		   not support a certain QOS parameter, its value will be set to QOS_UNKNOWN.  In
		   the case where no QOS parameters are supported by the NS provider, the length of 
		   this field will be zero. */
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
		p->OPTIONS_flags = 0;
		p->NIDU_size = 65535;
		p->SERV_type = ip->provider ? ip->provider : (N_CONS | N_CLNS);
		p->CURRENT_state = npi_get_state(ip);
		p->PROVIDER_type = N_SNICFP;
		p->NODU_size = ip->pmtu ? : 536;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
		p->NPI_version = N_VERSION_2;
		mp->b_wptr += sizeof(*p);
		if (add_len) {
			for (i = 0; i < ip->snum; i++) {
				sin = (struct sockaddr_in *) mp->b_wptr;
				sin->sin_family = AF_INET;
				sin->sin_port = ip->sport;
				sin->sin_addr.s_addr = ip->saddrs[i].addr;
				mp->b_wptr += sizeof(*sin);
			}
		}
		qos = (typeof(qos)) mp->b_wptr;
		*qos = ip->qos;
		qos->n_qos_type = N_QOS_SEL_INFO_IP;
		qos->ttl = ip->ttl;
		qos->tos = ip->tos;
		mp->b_wptr += qos_len;
		qor = (typeof(qor)) mp->b_wptr;
		qor->n_qos_type = N_QOS_RANGE_INFO_IP;
		mp->b_wptr += qor_len;
		if (pro_len) {
			bcopy(&ip->protoids, mp->b_wptr, pro_len);
			mp->b_wptr += pro_len;
		}
		putnext(ip->oq, mp);
		return (QR_DONE);
	} else {
		err = -ENOBUFS;
		ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	}
	return (err);
}

/**
 * ne_bind_ack: - NE_BIND_ACK event
 * @q: active queue in queue pair (write queue)
 * @coninds: number of connection indications
 *
 * Generate an N_BIND_ACK and pass it upstream.
 */
STATIC int
ne_bind_ack(queue_t *q, ulong coninds)
{
	struct ip *ip = IP_PRIV(q);
	int err, i;
	mblk_t *mp;
	N_bind_ack_t *p;
	struct sockaddr_in *sin;
	size_t add_len = ip->snum * sizeof(struct sockaddr_in);
	size_t pro_len = ip->pnum * sizeof(uint8_t);

	if (npi_get_state(ip) == NS_WACK_BREQ) {
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len + pro_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_BIND_ACK;
			p->ADDR_length = add_len;
			p->ADDR_offset = add_len ? sizeof(*p) : 0;
			p->CONIND_number = 0;
			p->TOKEN_value = 0;
			p->PROTOID_length = pro_len;
			p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
			mp->b_wptr += sizeof(*p);
			if (add_len) {
				sin = (struct sockaddr_in *) mp->b_wptr;
				for (i = 0; i < ip->snum; i++) {
					sin->sin_family = AF_INET;
					sin->sin_port = ip->sport;
					sin->sin_addr.s_addr = ip->saddrs[i].addr;
					mp->b_wptr = (unsigned char *) (++sin);
				}
			}
			if (pro_len) {
				bcopy(&ip->protoids, mp->b_wptr, pro_len);
				mp->b_wptr += pro_len;
			}
			npi_set_state(ip, NS_IDLE);
			putnext(ip->oq, mp);
			return (QR_DONE);
		} else {
			err = -ENOBUFS;
			ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
		}
	} else {
		err = -EFAULT;
		ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	}
	return (err);
}

/**
 * ne_error_ack: - generate an N_ERROR_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @prim: primitive in error
 * @err: error (positive NPI_error, negative UNIX_error)
 *
 * Some errors are passed through because they are intercepted and used by the put or service
 * procedure on the write side queue.  Theses are 0 (no error), -EBUSY (canput failed), -EAGAIN
 * (lock failed), -ENOMEM (kmem_alloc failed), -ENOBUFS (allocb failed).  All of these pass-through
 * errors will cause the state to be restored to the state before the current write queue message
 * was processed and the current write queue message will be placed back on the write queue.
 *
 * Some BSD-specific UNIX errors are translated to equivalent NPI errors.
 *
 * This function will also place the NPI provider interface state in the correct state following
 * issuing the N_ERROR_ACK according to the Sequence of Primities of the Network Provider Interface
 * specification, Revision 2.0.0.
 */
STATIC int
ne_error_ack(queue_t *q, int prim, int err)
{
	mblk_t *mp;
	N_error_ack_t *p;

	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	case 0:
		return (err);
	case -EADDRINUSE:
		err = NBOUND;
		break;
	case -EADDRNOTAVAIL:
		err = NNOADDR;
		break;
	case -EACCES:
		err = NACCESS;
		break;
	case -EOPNOTSUPP:
		err = NNOTSUPPORT;
		break;
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		struct ip *ip = IP_PRIV(q);

		mp->b_datap->db_type = M_PCPROTO;
		p = (N_error_ack_t *) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = (err < 0) ? NSYSERR : err;
		p->UNIX_error = (err < 0) ? -err : 0;
		mp->b_wptr += sizeof(*p);
		switch (npi_get_state(ip)) {
		case NS_WACK_OPTREQ:
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_UREQ:
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WCON_CREQ:
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WCON_RREQ:
			npi_set_state(ip, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		case NS_WACK_BREQ:
			npi_set_state(ip, NS_UNBND);
			break;
		case NS_WACK_CRES:
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ6:
			npi_set_state(ip, NS_WCON_CREQ);
			break;
		case NS_WACK_DREQ7:
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ9:
			npi_set_state(ip, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		case NS_WACK_DREQ10:
			npi_set_state(ip, NS_WCON_RREQ);
			break;
		case NS_WACK_DREQ11:
			npi_set_state(ip, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we send NOUTSTATE or NNOTSUPPORT or are responding
			   to an N_OPTMGMT_REQ in other than the NS_IDLE state. */
			break;
		}
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ne_ok_ack: - generate an N_OK_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @prim: correct primitive
 * @add: destination or responding address
 * @anum: number of destination or responding addresses
 * @qos: quality of service parameters
 * @seq: sequence number (i.e. connection/reset indication sequence number)
 * @tok: token (i.e. connection response token)
 * @flags: mangement flags, error number, etc.
 */
STATIC int
ne_ok_ack(queue_t *q, np_ulong prim, struct sockaddr_in *add, socklen_t anum, void *qos, mblk_t
	  *seq, struct ip *tok, np_ulong flags)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	mblk_t *mp;
	N_ok_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (npi_get_state(ip)) {
		case NS_WACK_OPTREQ:
			if ((err = ip_optmgmt(ip, qos, flags)) < 0)
				goto free_out;
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_UREQ:
			if ((err = ip_unbind(ip)) < 0)
				goto free_out;
			npi_set_state(ip, NS_UNBND);
			break;
		case NS_WACK_CRES:
			if ((err = ip_passive(ip, add, anum, qos, seq, tok, flags)) < 0)
				goto free_out;
			if (ip != tok) {
				npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
				npi_set_state(tok, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			} else {
				npi_set_state(ip, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			}
			break;
		case NS_WACK_RRES:
			if ((err = ip_reset(ip, add, N_USER, N_REASON_UNDEFINED)) < 0)
				goto free_out;
			npi_set_state(ip, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		case NS_WACK_DREQ6:
			if ((err = ip_disconnect(ip, add, seq, flags)) < 0)
				goto free_out;
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ7:
			if ((err = ip_disconnect(ip, add, seq, flags)) < 0)
				goto free_out;
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ9:
			if ((err = ip_disconnect(ip, add, seq, flags)) < 0)
				goto free_out;
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ10:
			if ((err = ip_disconnect(ip, add, seq, flags)) < 0)
				goto free_out;
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ11:
			if ((err = ip_disconnect(ip, add, seq, flags)) < 0)
				goto free_out;
			npi_set_state(ip, ip->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a N_OPTMGMT_REQ in other than
			   the NS_IDLE state. */
			break;
		}
		putnext(ip->oq, mp);
		return (QR_DONE);
	} else {
		err = -ENOBUFS;
		ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
		return (err);
	}
      free_out:
	freemsg(mp);
	return (err);
}

/**
 * ne_conn_con: perform N_CONN_CON actions
 * @q: active queue in queue pair (write queue)
 * @dst: destination address(es)
 * @dnum: number of destination addresses
 * @qos: quality of service parameters
 *
 */
STATIC INLINE fastcall int
ne_conn_con(queue_t *q, struct sockaddr_in *dst, size_t dnum, N_qos_sel_conn_ip_t * qos)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_conn_con_t *p;
	size_t qnum = dnum ? dnum : 1;
	int err;

	npi_set_state(ip, NS_WCON_CREQ);

	if ((mp = ss7_allocb(q, sizeof(*p) + dnum * sizeof(*dst) + qnum * sizeof(*qos), BPRI_MED))) {
		if ((err = ip_connect(ip, dst, dnum, qos)) == 0) {
			mp->b_datap->db_type = M_PCPROTO;
			mp->b_band = 0;
			p = (N_conn_con_t *) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = dnum * sizeof(*dst);
			p->RES_offset = p->RES_length ? sizeof(*dst) : 0;
			p->CONN_flags = 0;
			p->QOS_length = qnum * sizeof(*qos);
			p->QOS_offset = p->QOS_length ? sizeof(*qos) + p->RES_length : 0;
			mp->b_wptr += sizeof(*p);
			if (p->RES_length != 0) {
				bcopy(dst, mp->b_wptr, p->RES_length);
				mp->b_wptr += p->RES_length;
			}
			if (p->QOS_length != 0) {
				bcopy(qos, mp->b_wptr, p->QOS_length);
				mp->b_wptr += p->QOS_length;
			}
			qreply(q, mp);
			return (QR_DONE);
		}
		freemsg(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * ne_reset_con: - generate a N_RESET_CON message
 * @q: active queue in queue pair (write queue)
 * @dp: message containing IP packet
 */
STATIC INLINE fastcall int
ne_reset_con(queue_t *q, struct sockaddr_in *dst, np_ulong RESET_orig, np_ulong RESET_reason)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_reset_con_t *p;
	N_qos_sel_reset_ip_t *qos;
	size_t dnum, dlen, qlen;
	int err, i;

	dnum = (dst == NULL || dst->sin_addr.s_addr == INADDR_ANY) ? ip->dnum : 1;
	dlen = dnum * sizeof(*dst);
	qlen = dnum * sizeof(*qos);

	npi_set_state(ip, NS_WCON_RREQ);

	if ((mp = ss7_allocb(q, sizeof(*p) + dlen + qlen, BPRI_MED))) {
		if ((err = ip_reset(ip, dst, RESET_orig, RESET_reason)) == 0) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (N_reset_con_t *) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			p->DEST_length = dlen;
			p->DEST_offset = dlen ? sizeof(*p) : 0;
			p->QOS_length = qlen;
			p->QOS_offset = qlen ? sizeof(*p) + dlen : 0;
			mp->b_wptr += sizeof(*p);
			for (i = 0; i < ip->dnum; i++) {
				if (dst == NULL || dst->sin_addr.s_addr == INADDR_ANY
				    || dst->sin_addr.s_addr == ip->daddrs[i].addr) {
					struct sockaddr_in *sin;

					sin = (struct sockaddr_in *) mp->b_wptr;
					sin->sin_family = AF_INET;
					sin->sin_port = ip->dport;
					sin->sin_addr.s_addr = ip->daddrs[i].addr;
					mp->b_wptr += sizeof(*sin);
				}
			}
			for (i = 0; i < ip->dnum; i++) {
				if (dst == NULL || dst->sin_addr.s_addr == INADDR_ANY
				    || dst->sin_addr.s_addr == ip->daddrs[i].addr) {
					qos = (N_qos_sel_reset_ip_t *) mp->b_wptr;
					qos->ttl = ip->daddrs[i].ttl;
					qos->tos = ip->daddrs[i].tos;
					qos->mtu = ip->daddrs[i].mtu;
					mp->b_wptr += sizeof(*qos);
				}
			}
			npi_set_state(ip, ip->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			qreply(q, mp);
			return (QR_DONE);
		}
		freemsg(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * ne_conn_ind: - generate a N_CONN_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * We generate connection indications to Streams that are bound as listening to an address including
 * the destination address of the IP packet, where no connection exists for the source address of
 * the IP packet.
 */
STATIC INLINE fastcall int
ne_conn_ind(queue_t *q, mblk_t *dp)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp, *bp;
	N_conn_ind_t *p;
	struct sockaddr_in *sin;
	N_qos_sel_conn_ip_t *qos;
	const size_t size = sizeof(*p) + sizeof(*sin) + sizeof(*sin) + sizeof(*qos);
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;

	assure(dp->b_wptr >= dp->b_rptr + sizeof(*iph));
	assure(dp->b_wptr >= dp->b_rptr + (iph->ihl << 2));

	/* Make sure we don't already have a connection indication */
	for (bp = ip->conq; bp; bp = bp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) bp->b_rptr;

		if (iph->protocol == iph2->protocol
		    && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr) {
			/* duplicate, just discard it */
			return (QR_DONE);
		}
	}

	npi_set_state(ip, NS_WRES_CIND);

	if ((bp = ss7_dupmsg(q, dp))) {
		if ((mp = ss7_allocb(q, size, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (N_conn_ind_t *) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->SRC_length = sizeof(*sin);
			p->SRC_offset = sizeof(*p) + sizeof(*sin);
			p->SEQ_number = (np_ulong) (long) dp;
			p->CONN_flags = 0;
			p->QOS_length = sizeof(*qos);
			p->QOS_offset = sizeof(*p) + sizeof(*sin) + sizeof(*sin);
			mp->b_wptr += sizeof(*p);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = iph->protocol;
			sin->sin_addr.s_addr = iph->daddr;
			mp->b_wptr += sizeof(*sin);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = iph->protocol;
			sin->sin_addr.s_addr = iph->saddr;
			mp->b_wptr += sizeof(*sin);
			qos = (N_qos_sel_conn_ip_t *) mp->b_wptr;
			qos->n_qos_type = N_QOS_SEL_CONN_IP;
			qos->protocol = iph->protocol;
			qos->priority = bp->b_band;
			qos->ttl = iph->ttl;
			qos->tos = iph->tos;
			qos->mtu = 0;	/* FIXME: determine route and get mtu from it */
			mp->b_wptr += sizeof(*qos);
			/* should we pull the IP header? */
			mp->b_cont = bp;
			/* save original in connection indication list */
			dp->b_next = ip->conq;
			ip->conq = dp;
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		freemsg(bp);
	}
	return (-ENOBUFS);
}

/**
 * ne_discon_ind: - NE_DISCON_IND NC disconnected event
 * @q: active queue (read queue)
 * @dp: message containing ICMP packet
 *
 * The N_DISCON_IND is sent when we encounter an error on a connection oriented Stream, i.e. as a
 * result of receiving an ICMP error.  For multihomed hosts, we only do this if all destination
 * addresses have errors, otherwise, we just perform a reset for the affected destination.
 */
STATIC INLINE fastcall int
ne_discon_ind(queue_t *q, struct sockaddr_in *res, np_ulong mtu, np_ulong orig, np_ulong reason,
	      mblk_t *conp, mblk_t *dp)
{
	mblk_t *mp;
	N_discon_ind_t *p;
	struct sockaddr_in *sin;

	if ((mp = ss7_allocb(q, sizeof(*p) + sizeof(*sin), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 2;	/* expedite */
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_IND;
		p->DISCON_orig = orig;
		p->DISCON_reason = reason;
		p->RES_length = sizeof(*sin);
		p->RES_offset = sizeof(*p);
		p->SEQ_number = (np_ulong) (long) conp;
		mp->b_wptr += sizeof(*p);
		sin = (struct sockaddr_in *) mp->b_wptr;
		*sin = *res;
		mp->b_wptr += sizeof(*sin);
		mp->b_cont = dp;
		putnext(q, mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/**
 * ne_discon_ind_icmp: - NE_DISCON_IND event resulting from ICMP message
 * @q: active queue in queue pair
 * @mp: the ICMP message
 */
STATIC INLINE fastcall int
ne_discon_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in resp;
	np_ulong mtu, orig, error;
	mblk_t **respp, **conpp;
	ptrdiff_t hidden;
	int err;

	iph = (struct iphdr *) mp->b_rptr;	/* this is the ICMP message IP header */
	icmp = (struct icmphdr *) (mp->b_rptr + (iph->ihl << 2));
	iph = (struct iphdr *) (icmp + 1);	/* this is the encapsulated IP header */
	uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));
	if (mp->b_wptr < (unsigned char *) (uh + 1))
		uh = NULL;	/* don't have a full transport header */

	resp.sin_family = AF_INET;
	resp.sin_port = uh ? uh->source : 0;
	resp.sin_addr.s_addr = iph->saddr;

	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			orig = N_PROVIDER;
			error = N_REJ_NSAP_UNREACH_P;	// N_UD_ROUTE_UNAVAIL;
			mtu = 0;
			break;
		case ICMP_FRAG_NEEDED:
			orig = N_PROVIDER;
			error = N_REJ_QOS_UNAVAIL_P;	// N_UD_SEG_REQUIRED;
			mtu = icmp->un.frag.mtu;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			orig = N_PROVIDER;
			error = N_REJ_NSAP_UNKNOWN;	// N_UD_ROUTE_UNAVAIL;
			mtu = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			orig = N_PROVIDER;
			error = N_REJ_QOS_UNAVAIL_P;	// N_UD_QOS_UNAVAIL;
			mtu = 0;
			break;
		default:
			orig = N_UNDEFINED;
			error = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			mtu = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		/* Should not cause disconnect. */
		orig = N_PROVIDER;
		error = N_CONGESTION;	// N_UD_CONGESTION;
		mtu = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			orig = N_PROVIDER;
			error = N_REJ_QOS_UNAVAIL_P;	// N_UD_LIFE_EXCEEDED;
			mtu = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			error = N_REJ_QOS_UNAVAIL_P;	// N_UD_TD_EXCEEDED;
			mtu = 0;
			break;
		default:
			orig = N_UNDEFINED;
			error = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			mtu = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		orig = N_UNDEFINED;
		error = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		mtu = 0;
		break;
	default:
		orig = N_UNDEFINED;
		error = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		mtu = 0;
		break;
	}

	/* check for outstanding reset indications for responding address */
	for (respp = &ip->resq; (*respp); respp = &(*respp)->b_next) {
		struct iphdr *iph2 = (struct iphdr *) (*respp)->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}

	/* check for outstanding connection indications for responding address */
	for (conpp = &ip->conq; (*conpp); conpp = &(*conpp)->b_next) {
		struct iphdr *iph2 = (struct iphdr *) (*conpp)->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}

	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = ne_discon_ind(q, &resp, mtu, orig, error, (*conpp), mp)) < 0)
		mp->b_rptr -= hidden;
	else if ((*conpp) != NULL) {
		mblk_t *b, *b_prev;

		/* Remove connection indication from queue */
		b = (*conpp);
		(*conpp) = b->b_next;
		b->b_next = NULL;

		/* Free any attached pending data */
		b_prev = b;
		while ((b = b_prev)) {
			b_prev = b->b_prev;
			b->b_prev = NULL;
			b->b_next = NULL;
			freemsg(b);
		}
	}
	return (err);
}

/**
 * ne_data_ind: - generate a N_DATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Very fast.  In fact, we could just pass the raw M_DATA blocks upstream.  We leave the IP header
 * in the block.  Should we pull it?
 */
STATIC INLINE fastcall __hot_in int
ne_data_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;

	if (canputnext(q)) {
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			/* TODO: here we can set some info like ECN... */
			p->DATA_xfer_flags = 0;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			dp->b_datap->db_type = M_DATA;	/* just in case */
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ne_exdata_ind: - generate a N_EXDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC INLINE fastcall int
ne_exdata_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;

	if (bcanputnext(q, 1)) {
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			dp->b_datap->db_type = M_DATA;	/* just in case */
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ne_unitdata_ind: - generate a N_UNITDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC INLINE fastcall __hot_in int
ne_unitdata_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;
	struct sockaddr_in *sin;

	if (canputnext(q)) {
		if ((mp = ss7_allocb(q, sizeof(*p) + sizeof(*sin) + sizeof(*sin), BPRI_MED))) {
			struct iphdr *iph = (struct iphdr *) dp->b_rptr;

			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->SRC_length = sizeof(*sin);
			p->SRC_offset = sizeof(*p);
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p) + sizeof(*sin);
			p->ERROR_type = 0;
			mp->b_wptr += sizeof(*p);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = iph->protocol;
			sin->sin_addr.s_addr = iph->saddr;
			mp->b_wptr += sizeof(*sin);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = iph->protocol;
			sin->sin_addr.s_addr = iph->daddr;
			mp->b_wptr += sizeof(*sin);
			mp->b_cont = dp;
			dp->b_datap->db_type = M_DATA;	/* just in case */
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ne_uderror_ind: - generate a N_UDERROR_IND message
 * @q: active queue in queue pair (read queue)
 * @dst: pointer to destination address
 * @mtu: MTU if error is N_UD_SEG_REQUIRED
 * @error: error number
 * @dp: message containing (part of) errored packet
 *
 * Note that in the special case of N_UD_SEG_REQUIRED, we use the RESERVED_field to indicate that
 * the value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky
 * trick, because the field must be coded zero according to spec, so the presence of a non-zero
 * value indicates that the MTU value is present for supporting providers.
 */
STATIC INLINE fastcall int
ne_uderror_ind(queue_t *q, struct sockaddr_in *dst, np_ulong mtu, np_ulong error, mblk_t *dp)
{
	mblk_t *mp;
	N_uderror_ind_t *p;
	struct sockaddr_in *sin;

	if (canputnext(q)) {
		if ((mp = ss7_allocb(q, sizeof(*p) + sizeof(*sin), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;	/* expedite */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field = mtu;
			p->ERROR_type = error;
			mp->b_wptr += sizeof(*p);
			sin = (struct sockaddr_in *) mp->b_wptr;
			*sin = *dst;
			mp->b_wptr += sizeof(*sin);
			mp->b_cont = dp;
			dp->b_datap->db_type = M_DATA;	/* was M_ERROR in some cases */
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ne_uderror_ind_icmp: - generate an N_UDERROR_IND message from an ICMP packet
 * @q: active queue in pair (read queue)
 * @mp: message containing ICMP packet
 *
 * There is another reason for issuing an N_UDERROR_IND and that is Explicit Congestion
 * Notification, but there is no ICMP message associated with that and it has not yet been coded:
 * probably need an ne_uderror_ind_ecn() function.
 *
 * Note that the special case of N_UD_SEG_REQUIRED, se use the RESERVED_field to indicate that the
 * value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky trick,
 * because the field must be coded zero according to NPI spec, so the presence of a non-zero value
 * indicates the MTU value from a supporting NPI provider.
 */
STATIC INLINE fastcall int
ne_uderror_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in dest;
	np_ulong mtu, error;
	ptrdiff_t hidden;
	int err;

	iph = (struct iphdr *) mp->b_rptr;	/* this is the ICMP message IP header */
	icmp = (struct icmphdr *) (mp->b_rptr + (iph->ihl << 2));
	iph = (struct iphdr *) (icmp + 1);	/* this is the encapsulated IP header */
	uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));
	if (mp->b_wptr < (unsigned char *) (uh + 1))
		uh = NULL;	/* don't have a full transport header */
	dest.sin_family = AF_INET;
	dest.sin_port = uh ? uh->source : 0;
	dest.sin_addr.s_addr = iph->saddr;
	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			error = N_UD_ROUTE_UNAVAIL;
			mtu = 0;
			break;
		case ICMP_FRAG_NEEDED:
			error = N_UD_SEG_REQUIRED;
			mtu = icmp->un.frag.mtu;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			error = N_UD_ROUTE_UNAVAIL;
			mtu = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			error = N_UD_QOS_UNAVAIL;
			mtu = 0;
			break;
		default:
			error = N_UD_UNDEFINED;
			mtu = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		error = N_UD_CONGESTION;
		mtu = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			error = N_UD_LIFE_EXCEEDED;
			mtu = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			error = N_UD_TD_EXCEEDED;
			mtu = 0;
			break;
		default:
			error = N_UD_UNDEFINED;
			mtu = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		error = N_UD_UNDEFINED;
		mtu = 0;
		break;
	default:
		error = N_UD_UNDEFINED;
		mtu = 0;
		break;
	}
	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = ne_uderror_ind(q, &dest, mtu, error, mp)) < 0)
		mp->b_rptr -= hidden;
	return (err);
}

STATIC INLINE fastcall int
ne_uderror_reply(queue_t *q, struct sockaddr_in *dst, np_ulong mtu, np_ulong error, mblk_t *db)
{
	int err;

	switch (error) {
	case -EINVAL:
		return ne_error_reply(q, error);
	case NBADADDR:
	case NBADDATA:
		break;
	case NOUTSTATE:
		return ne_error_reply(q, -EPROTO);
	}
	if ((err = ne_uderror_ind(q, dst, mtu, error, db)) == QR_DONE)
		return (QR_STRIP);
	return (err);
}

/**
 * ne_reset_ind: - generate a N_RESET_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing ICMP packet
 *
 * We generate an extended reset indication that includes the resetting address, and the qos
 * parameters associated with the reset.  Reset reasons are ICMP errors.  The reset originator is
 * always the NS provider.
 *
 * Another reason for sending a N_RESET_IND is Explicity Congestion Notification (N_UD_CONGESTION)
 * but not as a result of an ICMP message.  This has yet to be coded.
 */
STATIC int
ne_reset_ind(queue_t *q, mblk_t *dp)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp, *bp;
	N_reset_ind_t *p;
	struct sockaddr_in *sin;
	N_qos_sel_reset_ip_t *qos;
	const size_t size = sizeof(*p) + sizeof(*sin) + sizeof(*qos);
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct icmphdr *icmp = (struct icmphdr *) (dp->b_rptr + (iph->ihl << 2));
	ulong mtu = 0;

	assure(dp->b_wptr >= dp->b_rptr + sizeof(*iph));
	assure(dp->b_wptr >= dp->b_rptr + (iph->ihl << 2));

	/* Make sure we don't already have a reset indication */
	for (bp = ip->resq; bp; bp = bp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) bp->b_rptr;
		struct icmphdr *icmp2 = (struct icmphdr *) (bp->b_rptr + (iph2->ihl << 2));

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && icmp->type == icmp2->type && icmp->code == icmp2->code)
			/* duplicate, just discard it */
			return (QR_DONE);
	}

	if ((bp = ss7_dupmsg(q, dp))) {
		if ((mp = ss7_allocb(q, size, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;
			p = (N_reset_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = N_PROVIDER;
			switch (icmp->type) {
			case ICMP_DEST_UNREACH:
				switch (icmp->code) {
				case ICMP_NET_UNREACH:
				case ICMP_HOST_UNREACH:
				case ICMP_PROT_UNREACH:
				case ICMP_PORT_UNREACH:
					p->RESET_reason = N_UD_ROUTE_UNAVAIL;
					break;
				case ICMP_FRAG_NEEDED:
					mtu = icmp->un.frag.mtu;
					p->RESET_reason = N_UD_SEG_REQUIRED;
					break;
				case ICMP_NET_UNKNOWN:
				case ICMP_HOST_UNKNOWN:
				case ICMP_HOST_ISOLATED:
				case ICMP_NET_ANO:
				case ICMP_HOST_ANO:
				case ICMP_PKT_FILTERED:
				case ICMP_PREC_VIOLATION:
				case ICMP_PREC_CUTOFF:
					p->RESET_reason = N_UD_ROUTE_UNAVAIL;
					break;
				case ICMP_SR_FAILED:
				case ICMP_NET_UNR_TOS:
				case ICMP_HOST_UNR_TOS:
					p->RESET_reason = N_UD_QOS_UNAVAIL;
					break;
				default:
					p->RESET_reason = N_UD_UNDEFINED;
					break;
				}
				break;
			case ICMP_SOURCE_QUENCH:
				p->RESET_reason = N_UD_CONGESTION;
				break;
			case ICMP_TIME_EXCEEDED:
				switch (icmp->code) {
				case ICMP_EXC_TTL:
					p->RESET_reason = N_UD_LIFE_EXCEEDED;
					break;
				case ICMP_EXC_FRAGTIME:
					p->RESET_reason = N_UD_TD_EXCEEDED;
					break;
				default:
					p->RESET_reason = N_UD_UNDEFINED;
					break;
				}
				break;
			case ICMP_PARAMETERPROB:
				p->RESET_reason = N_UD_UNDEFINED;
				break;
			default:
				p->RESET_reason = N_UD_UNDEFINED;
				break;
			}
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->QOS_length = sizeof(*qos);
			p->QOS_offset = sizeof(*p) + sizeof(*sin);
			mp->b_wptr += sizeof(*p);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = iph->protocol;
			sin->sin_addr.s_addr = iph->saddr;
			mp->b_wptr += sizeof(*sin);
			qos = (N_qos_sel_reset_ip_t *) mp->b_wptr;
			qos->n_qos_type = N_QOS_SEL_RESET_IP;
			qos->ttl = iph->ttl;
			qos->tos = iph->tos;
			if (icmp->type == ICMP_DEST_UNREACH && icmp->code == ICMP_FRAG_NEEDED)
				qos->mtu = icmp->un.frag.mtu;
			else
				qos->mtu = 0;
			mp->b_wptr += sizeof(*qos);
			/* save original in reset indication list */
			dp->b_next = ip->resq;
			ip->resq = dp;
			putnext(q, mp);
			return (QR_DONE);
		}
		freemsg(bp);
	}
	return (-ENOBUFS);
}

/**
 * ne_datack_ind: - NE_DATACK_IND event
 * @q: active queue in pair (read queue)
 */
STATIC INLINE fastcall int
ne_datack_ind(queue_t *q)
{
	mblk_t *mp;
	N_datack_ind_t *p;

	if (canputnext(q)) {
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_datack_ind_t *) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			putnext(q, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  ===================================================================
 *
 *  NPI User --> NPI Provider (IP) Primitives
 *
 *  ===================================================================
 */
/**
 * ne_info_req: - NE_INFO_REQ information request event
 * @q: active queue in pair (write queue)
 * @mp: N_INFO_REQ message
 */
STATIC int
ne_info_req(queue_t *q, mblk_t *mp)
{
	return ne_info_ack(q);
}

/**
 * ne_bind_req: - NE_BIND_REQ bind a NS user to network address event
 * @q: active queue in pair (write queue)
 * @mp: N_BIND_REQ message
 *
 * It is possible to bind more than one address to a stream.  Each address is formatted as a
 * sockaddr_in up to the size of sockaddr_storage (which is 8 addresses on Linux).  If the
 * DEFAULT_DEST flag is set, then the port number is insignificant (and should be coded zero) or an
 * address is not provided.  When DEFAULT_DEST is not specified, the port number is significant.
 * Once bound, all packets that arrive for the specified address(es) and port number combindation(s)
 * are delivered to the bound stream.  For DEFAULT_DEST streams, all packets that arrive for the
 * specified address(es) that have port numbers that are bound to no other address will be delivered
 * to the DEFAULT_DEST stream.  DEFAULT_DEST streams that are bound with no address (or to a
 * wildcard address) will have all packets delivered that are bound to no other address or port
 * number combination.  Protocol ids must always be specified.
 */
STATIC INLINE fastcall int
ne_bind_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	p = (typeof(p)) mp->b_wptr;
	if (unlikely(p->PRIM_type != N_BIND_REQ))
		goto efault;
	if (unlikely(npi_not_state(ip, NSF_UNBND)))
		goto outstate;
	npi_set_state(ip, NS_WACK_BREQ);
	if (unlikely((p->BIND_flags & ~TOKEN_REQUEST)
		     && (p->BIND_flags & ~TOKEN_REQUEST) != DEFAULT_DEST
		     && (p->BIND_flags & ~TOKEN_REQUEST) != DEFAULT_LISTENER))
		goto badflag;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto badaddr;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length))
		goto badprotoid;
	if (unlikely(p->PROTOID_length == 0))
		goto noprotoid;
	if (unlikely(p->PROTOID_length > 1))
		goto badprotoid2;
	{
		struct sockaddr_storage sa;
		struct sockaddr_in *add_in = (typeof(add_in)) & sa;
		size_t ADDR_length = p->ADDR_length;
		size_t anum;
		unsigned char *protoids = (typeof(protoids)) (mp->b_rptr + p->PROTOID_offset);
		size_t pnum = p->PROTOID_length;

		if (ADDR_length) {
			if (unlikely(ADDR_length < sizeof(struct sockaddr_in)))
				goto badaddr;
			if (unlikely(ADDR_length > sizeof(struct sockaddr_storage)))
				goto badaddr;
			anum = ADDR_length / sizeof(struct sockaddr_in);
			if (unlikely(ADDR_length != anum * sizeof(struct sockaddr_in)))
				goto badaddr;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, add_in, ADDR_length);
			if (unlikely(add_in->sin_family != AF_INET))
				goto badaddr;
		} else {
			anum = 1;
			add_in->sin_family = AF_INET;
			add_in->sin_port = 0;
			add_in->sin_addr.s_addr = INADDR_ANY;
		}
		if ((err = ip_bind(ip, add_in, anum, protoids, pnum, p->CONIND_number,
				   p->BIND_flags)) < 0)
			switch (err) {
			case -EADDRNOTAVAIL:
				goto bound;
			case -EDESTADDRREQ:
				goto noaddr;
			case -EACCES:
				goto access;
			default:
				goto provspec;
			}
		if ((err = ne_bind_ack(q, p->CONIND_number)) == 0)
			return (QR_DONE);
		ip_unbind_req(ip);
		return (err);
	}
      provspec:
	err = err;
	ptrace(("%s: %p: provider specific UNIX error %d\n", DRV_NAME, ip, -err));
	goto error;
      access:
	err = NACCESS;
	ptrace(("%s: %p: no permission for requested address\n", DRV_NAME, ip));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: could not allocate address\n", DRV_NAME, ip));
	goto error;
      bound:
	err = NBOUND;
	ptrace(("%s; %p: default address is already bound\n", DRV_NAME, ip));
	goto error;
      badprotoid:
      noprotoid:
      badprotoid2:
	err = NNOPROTOID;
	ptrace(("%s: %p: could not allocate protocol id\n", DRV_NAME, ip));
	goto error;
      badflag:
	err = NBADFLAG;
	ptrace(("%s: %p: invalid flags\n", DRV_NAME, ip));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: address is invalid\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return ne_error_ack(q, N_BIND_REQ, err);
}

/**
 * ne_unbind_req:- NE_UNBIND_REQ unbind NS user from network address event
 * @q: active queue in pair (write queue)
 * @mp: N_UNBIND_REQ message
 */
STATIC INLINE fastcall int
ne_unbind_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_unbind_req_t *p;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	p = (typeof(p)) mp->b_rptr;
	if (unlikely(p->PRIM_type != N_UNBIND_REQ))
		goto efault;
	if (unlikely(npi_not_state(ip, NSF_IDLE)))
		goto outstate;
	npi_set_state(ip, NS_WACK_UREQ);
	if (unlikely((err = ip_unbind_req(ip))))
		goto error;
	return ne_ok_ack(q, N_UNBIND_REQ, NULL, 0, NULL, NULL, NULL, 0);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return ne_error_ack(q, N_UNBIND_REQ, err);
}

/**
 * ne_optmgmt_req: - NE_OPTMGMT_REQ options management request
 * @q: active queue in pair (write queue)
 * @mp: N_OPTMGMT_REQ message
 */
STATIC int
ne_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_optmgmt_req_t *p;
	N_qos_sel_info_ip_t *qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	p = (typeof(p)) mp->b_rptr;
	if (unlikely(p->PRIM_type != N_OPTMGMT_REQ))
		goto efault;
	if (unlikely(npi_chk_state(ip, NSF_IDLE)))
		npi_set_state(ip, NS_WACK_OPTREQ);
	if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
		goto badopt;
	if (p->QOS_length) {
		if (p->QOS_length < sizeof(qos->n_qos_type))
			goto badopt;
		if (qos->n_qos_type != N_QOS_SEL_INFO_IP)
			goto badqostype;
		if (p->QOS_length != sizeof(*qos))
			goto badopt;
		if (qos->tos != QOS_UNKNOWN)
			ip->tos = qos->tos;
		if (qos->ttl != QOS_UNKNOWN)
			ip->ttl = qos->ttl;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		ip->i_flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		ip->i_flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	return ne_ok_ack(q, N_OPTMGMT_REQ, NULL, 0, qos, NULL, NULL, p->OPTMGMT_flags);
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: QOS structure type not supported\n", DRV_NAME, ip));
	goto error;
      badopt:
	err = NBADOPT;
	ptrace(("%s: %p: QOS options were invalid\n", DRV_NAME, ip));
	goto error;
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return ne_error_ack(q, N_OPTMGMT_REQ, err);
}

/**
 * n_unitdata_req: - NE_UNITDATA_REQ unit data request event
 * @q: active queue in pair (write queue)
 * @mp: N_UNITDATA_REQ message
 *
 * N_UNITDATA_REQ does not have a qos parameter, nor source address field, only a destination
 * address field.  We use three different approaches to specifying pertinent information for the
 * packet:
 *
 * 1. If DEST_length is non-zero, it specifies a destination address.  If RESERVED_field[0] is zero,
 *    no QOS parameters are specified.  In this case, the tos, ttl, etc., come from the NPI IP
 *    stream, and the source address comes from the outgoing interface.
 *
 * 2. If DEST_length is non-zero, it specifies a destination address.  If RESERVED_field[0] is also
 *    non-zero, QOS parameters are specified, in which case tos, ttl, souce address, etc., come from
 *    the QOS structure.
 *
 * 3. If DEST_length is zero, it indicates that the IP header has been included in the data payload.
 *    Destination address, source address and QOS parameters come from the included IP header.
 *
 * Approach number (1) is recommended because it follows the NPI interface strictly.
 */
STATIC INLINE fastcall __hot_put int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	size_t mlen, hlen = sizeof(struct iphdr);
	N_unitdata_req_t *p;
	struct sockaddr_in dst_buf, *dst = NULL;
	N_qos_sel_ud_ip_t qos_buf, *qos = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (typeof(p)) mp->b_rptr;
	if (p->PRIM_type != N_UNITDATA_REQ)
		goto efault;
	if (ip->provider == N_CONS)
		goto notsupport;
	if (ip->provider != N_CLNS)
		goto outstate;
	if (npi_not_state(ip, NSF_IDLE))
		goto outstate;
	if (!mp->b_cont)
		goto baddata;
	/* Note: no destination address is our clue that the upper layer had already included the
	   IP header in the IP packet. */
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badaddr;
		if (p->DEST_length < sizeof(struct sockaddr_in))
			goto badaddr2;
		/* avoid alignemnt problems */
		bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, sizeof(dst_buf));
		if (dst_buf.sin_family != AF_INET)
			goto badaddr;
		if (dst_buf.sin_port == 0)	/* this is really the IP protocol */
			goto badaddr;
		if (dst_buf.sin_addr.s_addr == INADDR_ANY)
			goto badaddr;
		dst = &dst_buf;
		hlen = 0;
	}
#define QOS_length RESERVED_field[0]
#define QOS_offset RESERVED_field[1]
	/* Note: the reserved field can be used to send QOS data for the datagram. */
	/* Actually, the proper way to do this is to set the appropriate values using
	   N_QOS_SEL_UD_IP to N_OPTMGMT_REQ(7) and then sending the data. */
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt;
		if (p->QOS_length < sizeof(qos_buf.n_qos_type))
			goto badopt;
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf.n_qos_type));
		if (qos_buf.n_qos_type != N_QOS_SEL_UD_IP)
			goto badqostype;
		if (p->QOS_length != sizeof(qos_buf))
			goto badqostype;
		/* avoid alignment problems */
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf));
		qos = &qos_buf;
	}
#undef QOS_length
#undef QOS_offset
	if ((mlen = msgdsize(mp->b_cont)) + hlen <= sizeof(struct iphdr) || mlen + hlen > 65535)
		goto baddata2;
	if ((err = ip_unitdata_req(q, dst, qos, mp->b_cont)) < 0)
		switch (err) {
		case -EDESTADDRREQ:
			goto noaddr;
		case -EMSGSIZE:
			goto baddata;
		case -EACCES:
			goto access;
		default:
			goto provspec;
		}
	return (QR_DONE);
      provspec:
	err = err;
	ptrace(("%s: %p: provider specific UNIX error %d\n", DRV_NAME, ip, -err));
	goto error;
      access:
	err = NACCESS;
	ptrace(("%s: %p: no permission for requested address\n", DRV_NAME, ip));
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      baddata2:
	err = NBADDATA;
	ptrace(("%s: %p: invalid amount of user data %u\n", DRV_NAME, ip, mlen));
	goto error;
      baddata:
	err = NBADDATA;
	ptrace(("%s: %p: invalid amount of user data\n", DRV_NAME, ip));
	goto error;
      badaddr2:
	err = NBADADDR;
	ptrace(("%s: %p: invalid address format\n", DRV_NAME, ip));
	goto error;
      noaddr:
	err = NBADADDR;
	ptrace(("%s: %p: could not assign address\n", DRV_NAME, ip));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: invalid address format\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      error:
	return ne_uderror_reply(q, &dest, 0, err, mp->b_cont);
}

/**
 * ne_conn_req:- NE_CONN_REQ connect NS user event
 * @q: active queue (write queue)
 * @mp: N_CONN_REQ message
 *
 * Request that a connection be made to (possibly supplemental) destination addresses.  The
 * addresses are formatted as an array of sockaddr_in structures.
 *
 * Fields are as follows:
 *
 * - PRIM_type: always N_CONN_REQ
 * - DEST_length: destination address length
 * - DEST_offset: destination address offset from start of message block
 * - CONN_flags: REC_CONF_OPT or EX_DATA_OPT
 * - QOS_length: quality of service parameters length
 * - QOS_offset: quality of service parameters offset from start of message block
 *
 * To support multihoming, this event permits an array of destination addresses.  The first address
 * in the array will be the primary address, unless another primary is specified in the QOS
 * parameters.  The primary address is used for subsequent NE_DATA_REQ and NE_EXDATA_REQ events
 * until changed with a NE_OPTMGMT_REQ event.
 *
 */
STATIC INLINE fastcall int
ne_conn_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_conn_req_t *p;
	N_qos_sel_conn_ip_t qos_buf = { N_QOS_SEL_CONN_IP, }, *qos = NULL;
	struct sockaddr_in dst_buf[8] = { {AF_INET,}, }, *dst = NULL;
	size_t dnum = 0;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_conn_req_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_CONN_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(ip->provider == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(ip->provider != N_CONS))
		goto error;
	/* connection requests are not allowed on listening Streams */
	NPI_error = NACCESS;
	if (unlikely(ip->maxinds > 0))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_get_state(ip) != NS_IDLE))
		goto error;
	NPI_error = NBADFLAG;
	if (unlikely(p->CONN_flags != 0))
		goto error;
	if (p->DEST_length != 0) {
		NPI_error = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
			goto error;
		dnum = p->DEST_length / sizeof(*dst);
		if (unlikely(dnum < 1))
			goto error;
		if (unlikely(dnum > 8))
			goto error;
		if (unlikely(p->DEST_length % sizeof(*dst) != 0))
			goto error;
		bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, p->DEST_length);
		if (unlikely(dst_buf[0].sin_family != AF_INET && dst_buf[0].sin_family != 0))
			goto error;
		if (unlikely(dst_buf[0].sin_port == 0))
			goto error;
		dst = &dst_buf;
	}
	if (p->QOS_length != 0) {
		NPI_error = NBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
			goto error;
		if (unlikely(p->QOS_length < sizeof(qos_buf.n_qos_type)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf.n_qos_type));
		NPI_error = NBADQOSTYPE;
		if (unlikely(qos_buf.n_qos_type != N_QOS_SEL_CONN_IP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(qos_buf)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf));
		qos = &qos_buf;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	if (unlikely((err = ne_conn_con(q, dst, dnum, qos)) < 0))
		switch (err) {
		case -EADDRNOTAVAIL:
			NPI_error = NBOUND;
			goto error;
		case -EDESTADDRREQ:
			NPI_error = NNOADDR;
			goto error;
		case -EACCES:
			NPI_error = NACCESS;
			goto error;
		default:
			NPI_error = err;
			goto error;
		}
	return (err);
      error:
	return ne_error_ack(q, N_CONN_REQ, NPI_error);
}

/**
 * ne_conn_res:- NE_CONN_RES accept previous connection indication event
 * @q: active queue (write queue)
 * @mp: N_CONN_RES message
 *
 * An NE_CONN_IND event is generated when an IP message arrives for an address bound to an N_CONS
 * Stream and there is not yet a connection for the source address in the message.  Outstanding
 * connection indications are queued against the Stream with a sequence number assigned (derived
 * from the address of the connecting message held in the connection indication list).  The
 * NE_CONN_RES event is generated by the user to accept a connection indication event from the
 * connection indication queue and to perform a passive connection (NE_PASS_CON even) on the
 * indicated Stream.  In addition, the user can set QOS parameters for the Stream to which a passive
 * connection is made and for any reponse message (data attached to the N_CONN_RES message).
 */
STATIC INLINE fastcall int
ne_conn_res(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q), *tok = ip;
	N_conn_res_t *p;
	N_qos_sel_conn_ip_t qos_buf = { N_QOS_SEL_CONN_IP, }, *qos = NULL;
	struct sockaddr_in res_buf[8] = { {AF_INET,}, }, *res = NULL;
	size_t rnum = 0;
	mblk_t *dp, *seq;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_conn_res_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_CONN_RES))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(ip->provider == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(ip->provider != N_CONS))
		goto error;
	if (unlikely(npi_not_state(ip, NSM_LISTEN)))
		goto error;
	if (p->RES_length != 0) {
		NPI_error = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length))
			goto einval;
		if (unlikely((rnum = p->RES_length / sizeof(*res)) < 1 || rnum > 8))
			goto error;
		if (unlikely(p->RES_length % sizeof(*res) != 0))
			goto error;
		/* Cannot be sure that the address is properly aligned, and to keep unscrupulous
		   users from DoS attacks, copy the informatoin into an aligned buffer. */
		bcopy(mp->b_rptr = p->RES_offset, &res_buf, p->RES_length);
		if (unlikely(res_buf[0].sin_family != AF_INET && res_buf[0].sin_family != 0))
			goto error;
		if (unlikely(res_buf[0].sin_port == 0))
			goto error;
		res = &res_buf;
	}
	if (p->QOS_length != 0) {
		NPI_error = NBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
			goto error;
		if (unlikely(p->QOS_length < sizeof(qos_buf.n_qos_type)))
			goto error;
		if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(qos_buf.n_qos_type)))
			goto error;
		/* Cannot be sure that the quality of service parameters are properly aligned, and
		   to keep unscrupulous users from DoS attacks, copy the informatoin into an
		   aligned buffer. */
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf.n_qos_type));
		NPI_error = NBADQOSTYPE;
		if (unlikely(qos_buf.n_qos_type != N_QOS_SEL_CONN_IP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(qos_buf)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf));
		qos = &qos_buf;
	}
	NPI_error = NBADDATA;
	if (unlikely((dp = mp->b_cont) != NULL && dp->b_wptr - dp->b_rptr <= 0))
		goto error;
	NPI_error = NBADSEQ;
	if (unlikely(p->SEQ_number == 0))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_not_state(ip, NSF_WRES_CIND) || ip->coninds < 1))
		goto error;
	for (seq = ip->conq; seq && (np_ulong) (long) seq != p->SEQ_number; seq = seq->b_next) ;
	NPI_error = NBADSEQ;
	if (unlikely(seq == NULL))
		goto error;
	if (p->TOKEN_value != 0) {
		for (tok = master.ip.list; tok && (np_ulong) (long) tok != p->TOKEN_value;
		     tok = tok->next) ;
		NPI_error = NBADTOKEN;
		if (unlikely(tok == NULL))
			goto error;
		if (unlikely(npi_get_state(tok) == NS_UNBND))
			goto error;	/* we could bind it */
		if (unlikely(tok->maxinds > 0))
			goto error;
		/* Note that the Stream to which we do a passive connection could be already
		   connected: we just are just adding another address to a multihomed connection.
		   But this is not as useful as adding or removing an address with N_OPTMGMT_REQ. */
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	npi_set_state(ip, NS_WACK_CRES);
	return ne_ok_ack(q, N_CONN_RES, res, rnum, qos, seq, tok, p->CONN_flags);
      error:
	return ne_error_ack(q, N_CONN_RES, err);
}

/**
 * ne_discon_req:- process N_DISCON_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DISCON_REQ message
 */
STATIC INLINE fastcall int
ne_discon_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_discon_req_t *p;
	struct sockaddr_in *res = NULL;
	size_t rnum = 0;
	mblk_t *seq = NULL;
	np_long err = -EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (npi_not_state(ip, NSM_CONNECTED))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_discon_req_t *) mp->b_rptr;
	if (p->RES_length) {
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto einval;
		if (p->RES_length % sizeof(*res) != 0)
			goto badaddr;
		res = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
		rnum = p->RES_length / sizeof(*res);
	}
	if (p->SEQ_number) {
		if (npi_get_state(ip) != NS_WRES_CIND)
			goto badseq;
		for (seq = ip->conq; seq && (np_ulong) (long) seq != p->SEQ_number;
		     seq = seq->b_next) ;
		if (seq == NULL)
			goto badseq;

	} else {
		if (npi_get_state(ip) == NS_WRES_CIND)
			goto badseq;
	}
	/* Ok, all checking done.  Now we need to disconnect the address. */
	return ne_ok_ack(q, N_DISCON_REQ, res, rnum, NULL, seq, NULL, p->DISCON_reason);
      badseq:
	err = NBADSEQ;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      error:
	return ne_error_ack(q, N_DISCON_REQ, err);
}

/**
 * ne_data_req:- process N_DATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATA_REQ message
 *
 * Unfortunately, there is no standard way of specifying destination and source addreses for
 * multihomed hosts.  We could use N_OPTMGMT_REQ to change the primary destination address, source
 * address and QOS parameters.  We could defined a non-standard DATA_xfer_flags to indicate that the
 * IP header is included in the attached M_DATA blocks.  Or, we could accept N_UNITDATA_REQ
 * primitives even though we are in N_CONS mode.
 */
STATIC INLINE fastcall int
ne_data_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_data_req_t *p;
	np_long err = EFAULT;
	mblk_t *dp;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_DATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (npi_chk_state(ip, (NSF_IDLE | NSF_WRES_RIND)))
		goto ignore;
	/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
	   destination of the request is an address that does not have an outstanding reset
	   indication. */
	if (npi_not_state(ip, NSM_OUTDATA))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_data_req_t *) mp->b_rptr;
	if (p->DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG))
		goto badflag;
	if (!(dp = mp->b_cont) || dp->b_wptr < dp->b_rptr + 1)
		goto baddata;
	/* Ok, really tranfser the data. */
	return (QR_STRIP);
      baddata:
	err = EPROTO;
	goto error;
      badflag:
	err = EPROTO;
	goto error;
      einval:
	err = EPROTO;
	goto error;
      ignore:
	return (QR_DONE);	/* discard */
      outstate:
	err = EPROTO;
	goto error;
      notsupport:
	err = EPROTO;
	goto error;
      error:
	return ne_error_reply(q, err);
}

/**
 * ne_exdata_req:- process N_EXDATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_EXDATA_REQ message
 */
STATIC INLINE fastcall int
ne_exdata_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_exdata_req_t *p;
	np_long err = EFAULT;
	mblk_t *dp;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_EXDATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (npi_chk_state(ip, (NSF_IDLE | NSF_WRES_RIND)))
		goto ignore;
	if (npi_not_state(ip, NSM_OUTDATA))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_exdata_req_t *) mp->b_rptr;
	if (!(dp = mp->b_cont) || dp->b_wptr < dp->b_rptr + 1)
		goto baddata;
	/* do not currently support this primitive, even when properly formatted */
	goto notsupport;
      baddata:
	err = EPROTO;
	goto error;
      einval:
	err = EPROTO;
	goto error;
      ignore:
	return (QR_DONE);	/* discard */
      outstate:
	err = EPROTO;
	goto error;
      notsupport:
	err = EPROTO;
	goto error;
      error:
	return ne_error_reply(q, err);
}

/**
 * ne_datack_req:- process N_DATACK_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATACK_REQ message
 */
STATIC int
ne_datack_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	np_long err = EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_DATACK_REQ primitive, then the NS provider should discard the request without
	   generating a fatal error. */
	if (npi_get_state(ip) == NS_IDLE)
		goto ignore;
	if (npi_not_state(ip, NSM_CONNECTED))
		goto outstate;
	/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the receipt
	   confirmation flag set, then the NS provider should just ignore the request without
	   generating a fatal error. */
	if (ip->datinds <= 0)
		goto ignore;
	if (mp->b_wptr < mp->b_rptr + sizeof(N_datack_req_t))
		goto einval;
	/* do not currently support this primitive, even when properly formatted */
	goto notsupport;
      einval:
	err = EPROTO;
	goto error;
      ignore:
	return (QR_DONE);	/* discard */
      outstate:
	err = EPROTO;
	goto error;
      notsupport:
	err = EPROTO;
	goto error;
      error:
	return ne_error_reply(q, err);
}

/**
 * ne_reset_req:- process N_RESET_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_REQ message
 */
STATIC INLINE fastcall int
ne_reset_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_reset_req_t *p;
	struct sockaddr_in *dst = NULL;
	np_long err = -EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_RESET_REQ primitive, then the NS provider should discard the message without
	   generating an error. */
	if (npi_get_state(ip) == NS_IDLE)
		goto ignore;
	if (npi_get_state(ip) != NS_DATA_XFER)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_reset_req_t *) mp->b_rptr;
	if (p->DEST_length != 0) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto einval;
		if (p->DEST_length != sizeof(*dst))
			goto badaddr;
		dst = (struct sockaddr_in *) (mp->b_rptr + p->DEST_offset);
		if (dst->sin_family != AF_INET)
			goto badaddr;
	}
	/* Ok, message checks out. */
	npi_set_state(ip, NS_WCON_RREQ);
	return ne_reset_con(q, dst, N_USER, p->RESET_reason);
      badaddr:
	err = NBADADDR;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      ignore:
	return (QR_DONE);	/* discard */
      outstate:
	err = NOUTSTATE;
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      error:
	return ne_error_ack(q, N_RESET_REQ, err);

}

/**
 * ne_reset_res:- process N_RESET_RES message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_RES message
 *
 * When a reset indication is sent to the user, we expect a reset response which is acknowledged.
 * The reason is, only one oustanding reset indication per destination per RESET_reason is issued.
 * No additional reset indication will be issued until a response is received.  Because of this,
 * reset indications are expedited (band 2).
 */
STATIC INLINE fastcall int
ne_reset_res(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_reset_res_t *p;
	struct sockaddr_in dest;
	struct ip_daddr *da;
	mblk_t **bpp, **dpp = NULL;
	np_long err, i;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (npi_not_state(ip, NSF_WRES_RIND))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_reset_res_t *) mp->b_rptr;
	if (p->DEST_length == 0)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto einval;
	if (p->DEST_length != sizeof(dest))
		goto badaddr;
	/* address might not be aligned */
	bcopy(mp->b_rptr + p->DEST_offset, &dest, p->DEST_length);
	if (dest.sin_family != AF_INET)
		goto badaddr;
	if (dest.sin_addr.s_addr == INADDR_ANY)
		goto noaddr;
	/* find pointer to pointer to oldest matching reset indication */
	for (bpp = &ip->resq; *(bpp); bpp = &(*bpp)->b_next) {
		struct iphdr *iph = (struct iphdr *) (*bpp)->b_rptr;

		if (ip->protocol == iph->protocol && dest.sin_addr.s_addr == iph->saddr)
			dpp = bpp;
	}
	if (!dpp)
		goto badaddr;
	/* find pointer to corresponding destination routing information */
	for (i = 0, da = ip->daddrs; i < ip->dnum; i++, da++)
		if (da->addr == dest.sin_addr.s_addr)
			break;
	if (i >= ip->dnum)
		goto badaddr;
	/* Ok, parameters check out. */
	npi_set_state(ip, NS_WACK_RRES);
	return ne_ok_ack(q, N_RESET_RES, &dest, 1, NULL, NULL, NULL, 0);
      noaddr:
	err = NNOADDR;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      error:
	return ne_error_ack(q, N_RESET_RES, err);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/**
 * ip_w_proto: process an M_PROTO, M_PCPROTO message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_PROTO, M_PCPROTO message to process
 *
 * These are normal N-primitives written from the upper layer protocol.
 */
STATIC INLINE fastcall __hot_put int
ip_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct ip *ip = IP_PRIV(q);
	np_long oldstate = npi_get_state(ip);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *((np_long *) mp->b_rptr))) {
		case N_UNITDATA_REQ:
			rtn = n_unitdata_req(q, mp);
			break;
		case N_DATA_REQ:
			rtn = ne_data_req(q, mp);
			break;
		case N_CONN_REQ:
			rtn = ne_conn_req(q, mp);
			break;
		case N_CONN_RES:
			rtn = ne_conn_res(q, mp);
			break;
		case N_DISCON_REQ:
			rtn = ne_discon_req(q, mp);
			break;
		case N_EXDATA_REQ:
			rtn = ne_exdata_req(q, mp);
			break;
		case N_DATACK_REQ:
			rtn = ne_datack_req(q, mp);
			break;
		case N_RESET_REQ:
			rtn = ne_reset_req(q, mp);
			break;
		case N_RESET_RES:
			rtn = ne_reset_res(q, mp);
			break;
		case N_INFO_REQ:
			rtn = ne_info_req(q, mp);
			break;
		case N_BIND_REQ:
			rtn = ne_bind_req(q, mp);
			break;
		case N_UNBIND_REQ:
			rtn = ne_unbind_req(q, mp);
			break;
		case N_OPTMGMT_REQ:
			rtn = ne_optmgmt_req(q, mp);
			break;
		case N_CONN_IND:
		case N_CONN_CON:
		case N_DISCON_IND:
		case N_DATA_IND:
		case N_EXDATA_IND:
		case N_INFO_ACK:
		case N_BIND_ACK:
		case N_ERROR_ACK:
		case N_OK_ACK:
		case N_UNITDATA_IND:
		case N_UDERROR_IND:
		case N_DATACK_IND:
		case N_RESET_IND:
		case N_RESET_CON:
			/* wrong direction */
			rtn = -EPROTO;
			break;
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		npi_set_state(ip, oldstate);
		/* The put and srv procedures do not recognize all errors.  Sometimes we return an
		   error to here just to restore the previous state.  */
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EOPNOTSUPP:
			return ne_error_ack(q, prim, rtn);
		case -EPROTO:
			return ne_error_reply(q, -EPROTO);
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

STATIC int
n_write(struct ip *ip, mblk_t *mp)
{
	swerr();
	fixme(("write this function"));
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ip_w_data(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);

	return n_write(ip, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ip_w_other(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		ip->u.dev.cminor);
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ip_r_other(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		ip->u.dev.cminor);
#if 0
	putnext(q, mp);
	return (QR_ABSORBED);
#else
	return (-EOPNOTSUPP);
#endif
}

/**
 *  ip_r_data: process M_DATA message
 *  @q: active queue in queue pair (read queue)
 *  @mp: the M_DATA message
 *
 *  M_DATA messages are placed to the read queue by the Linux IP ip_v4_rcv() callback.  The message
 *  contains a complete IP datagram starting with the IP header.  What needs to be done is to
 *  convert this to an upper layer indication and deliver it upstream.
 */
STATIC INLINE fastcall __hot_in int
ip_r_data(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int rtn;

	switch (ip->provider) {
	case N_CLNS:
		rtn = ne_unitdata_ind(q, mp);
		break;
	case N_CONS:
		rtn = ne_data_ind(q, mp);
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

/**
 *  ip_r_error: process M_ERROR message
 *  @q: active queue in queue pair (read queue)
 *  @mp: the M_ERROR message
 *
 *  M_ERROR messages are placed to the read queue by the Linux IP ip_v4_err() callback.  The message
 *  contains a complete ICMP datagram starting with the IP header.  What needs to be done is to
 *  convert this to an upper layer indication and deliver it upstream.
 */
STATIC fastcall int
ip_r_error(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int rtn;

	switch (ip->provider) {
	case N_CLNS:
		rtn = ne_uderror_ind_icmp(q, mp);
		break;
	case N_CONS:
		rtn = ne_reset_ind(q, mp);
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */

/*
 *  NPI Write Message
 */
/*
 *  IP Read Message
 */
STATIC INLINE streamscall __hot_in int
ip_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ip_r_data(q, mp);
	case M_ERROR:
		return ip_r_error(q, mp);
	default:
		return ip_r_other(q, mp);
	}
}
STATIC INLINE streamscall __hot_put int
ip_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ip_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ip_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	default:
		return ip_w_other(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
STATIC struct ip_bhash_bucket *npi_bhash;
STATIC size_t npi_bhash_size = 0;
STATIC size_t npi_bhash_order = 0;

STATIC struct ip_chash_bucket *npi_chash;
STATIC size_t npi_chash_size = 0;
STATIC size_t npi_chash_order = 0;

STATIC INLINE fastcall int
npi_bhashfn(unsigned char proto, unsigned short bport)
{
	return ((npi_bhash_size - 1) & (proto + bport));
}

STATIC INLINE fastcall int
npi_chashfn(unsigned char proto, unsigned short sport, unsigned short dport)
{
	return ((npi_chash_size - 1) & (proto + sport + dport));
}

/**
 * ip_lookup_conn - lookup Stream in the connection hashes
 * @iph:	IP header
 * @uh:		UDP header
 *
 * The start of the right kind of code shaping up here.  We provide connection buckets that have
 * IP protocol number, source port and destination port as hash parameters.  Attached to the
 * connection bucket are all Streams with the same protocol number, source port and destination port
 * combination (but possibly different IP adresses).  These Streams that are "owners" of the
 * connection bucket must be traversed and checked for address matches.
 */
STATIC INLINE fastcall struct ip *
ip_lookup_conn(struct iphdr *iph, struct udphdr *uh)
{
	unsigned char proto = iph->protocol;
	unsigned short sport = uh->source;
	unsigned short dport = uh->dest;
	uint32_t saddr = iph->saddr;
	uint32_t daddr = iph->daddr;
	struct ip *result = NULL;
	int hiscore = 0;
	struct ip_chash_bucket *hp, *hp1, *hp2;

	hp1 = &npi_chash[npi_chashfn(proto, sport, dport)];
	hp2 = &npi_chash[npi_chashfn(proto, 0, 0)];

	hp = hp1;
	do {
		read_lock_bh(&hp->lock);
		{
			struct ip *ip;
			np_ulong state;
			int i;

			for (ip = hp->list; ip; ip = ip->cnext) {
				int score = 0;

				/* only Streams in close to the correct state */
				if ((state = npi_get_state(ip)) != NS_DATA_XFER
				    && state != NS_WRES_RIND)
					continue;
				if (ip->sport != 0) {
					if (ip->sport != sport)
						continue;
					score++;
				}
				if (ip->dport != 0) {
					if (ip->dport != dport)
						continue;
					score++;
				}
				{
					int found = 0;

					for (i = 0; i < ip->snum; i++) {
						if (ip->saddrs[i].addr != 0) {
							if (ip->saddrs[i].addr != saddr)
								continue;
							found = 1;
							score++;
							break;
						} else
							found = 1;
					}
					if (found == 0)
						continue;
				}
				{
					int found = 0;

					for (i = 0; i < ip->snum; i++) {
						if (ip->daddrs[i].addr != 0) {
							if (ip->daddrs[i].addr != daddr)
								continue;
							found = 1;
							score++;
							break;
						} else
							found = 1;
					}
					if (found == 0)
						continue;
				}
				if (score > hiscore) {
					hiscore = score;
					result = ip;
				}
				if (score == 4)
					/* perfect match */
					break;
			}
		}
		read_unlock_bh(&hp->lock);
	} while (hiscore < 4 && hp != hp2 && (hp = hp2));
	ip_get(result);
	usual(result);
	return (result);
}

/**
 * ip_lookup_bind - lookup Stream in the bind/listening hashes
 * @iph:	IP header
 * @uh:		UDP header
 *
 * Note that an N_CLNS Stream cannot bind to a port number and must bind to port number zero.  An
 * N_CONS Stream can only bind listening to a non-zero port number, but can bind normal to a zero
 * port number.
 *
 * N_CLNS Streams are always entered into the bind hashes against port 0, even if they have an
 * assigned port.  N_CONS Streams are entered against their bound port number (which can be zero
 * only if they are not listening).  N_CONS Streams that are not listening will not be matched.
 * Only the DEFAULT_LISTENER can be bound listening against a zero port (and it requires a wildcard
 * address).
 *
 * NOTICES: There are two types of Streams in the bind hashes, N_CONS and N_CLNS.  When lookup up
 * a Stream for a received packet, we are interested in any N_CLNS Stream that matches or any N_CONS
 * stream that is in the listening state that matches.
 */
STATIC INLINE fastcall __hot_in struct ip *
ip_lookup_bind(struct iphdr *iph, struct udphdr *uh)
{
	unsigned char proto = iph->protocol;
	unsigned short bport = uh->dest;
	uint32_t daddr = iph->daddr;
	struct ip *result = NULL;
	int hiscore = 0;
	struct ip_bhash_bucket *hp, *hp1, *hp2;

	hp1 = &npi_bhash[npi_bhashfn(proto, bport)];
	hp2 = &npi_bhash[npi_bhashfn(proto, 0)];

	hp = hp1;
	do {
		read_lock_bh(&hp->lock);
		{
			struct ip *ip;
			np_ulong state;
			int i;

			for (ip = hp->list; ip; ip = ip->bnext) {
				int score = 0;

				/* only listening N_COTS Streams and N_CLTS Streams */
				if (ip->maxinds == 0 && ip->info.SERV_type != N_CLNS)
					continue;
				/* only Streams in close to the correct state */
				if ((state = npi_get_state(ip)) != NS_IDLE && state != NS_WACK_UREQ)
					continue;
				if (ip->bport != 0) {
					if (ip->bport != bport)
						continue;
					score++;
				}
				{
					int found = 0;

					for (i = 0; i < ip->bnum; i++) {
						if (ip->baddrs[i].addr != 0) {
							if (ip->baddrs[i].addr != daddr)
								continue;
							found = 1;
							score++;
							break;
						} else
							found = 1;
					}
					if (found == 0)
						continue;
				}
				if (score > hiscore) {
					hiscore = score;
					result = ip;
				}
				if (score == 2)
					/* perfect match */
					break;
			}
		}
		read_unlock_bh(&hp->lock);
	} while (hiscore < 2 && hp != hp2 && (hp = hp2));
	ip_get(result);
	usual(result);
	return (result);
}

/**
 * ip_lookup_next - lookup next Stream by protocol, address and port.
 * @ip_prev:	result of previous lookup, NULL for new
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
STATIC INLINE fastcall struct ip *
ip_lookup_next(struct ip *ip_prev, struct iphdr *iph, struct udphdr *uh)
{
	struct ip *result = NULL;
	struct ip_prot_bucket *pp, **ipp;

	ipp = &ip_prots[iph->protocol];

	read_lock_bh(&ip_prot_lock);
	if ((pp = *ipp)) {
		if (pp->corefs > 0) {
			if (result == NULL)
				result = ip_lookup_conn(iph, uh);
			if (result == NULL)
				result = ip_lookup_bind(iph, uh);
		} else if (pp->clrefs > 0) {
			if (result == NULL)
				result = ip_lookup_bind(iph, uh);
		}
	}
	read_unlock_bh(&ip_prot_lock);
	return (result);
}

STATIC INLINE fastcall struct ip *
ip_lookup(struct iphdr *iph, struct udphdr *uh)
{
	return ip_lookup_next(NULL, iph, uh);
}

/**
 * ip_lookup_icmp - a little different from ip_lookup(), for ICMP messages
 * @iph:	returned IP header
 * @len:	length of returned message
 *
 * This needs to do a reverse lookup (where destination address and port are compared to source
 * address and port, and visa versa).
 */
STATIC INLINE fastcall struct ip *
ip_lookup_icmp(struct iphdr *iph, unsigned int len)
{
	fixme(("Write this function.\n"));
	return (NULL);
}

STATIC streamscall void
ip_free(char *data)
{
	struct sk_buff *skb = (typeof(skb)) data;

	_trace();
	/* sometimes skb is NULL if it has been stolen */
	if (skb != NULL)
		kfree_skb(skb);
	return;
}

/**
 * ip_v4_rcv: - process a received IP packet
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
STATIC int
ip_v4_rcv(struct sk_buff *skb)
{
	int rtn = 0;

	read_lock_bh(&ip_prot_lock);	/* lock stream lists */
	{
		struct ip *ip;
		struct iphdr *iph = skb->nh.iph;
		struct udphdr *uh = (struct udphdr *) (skb->nh.raw + (iph->ihl << 2));

		if ((ip = ip_lookup(iph, uh))) {
			npi_v4_steal(skb);
			if (ip->oq && canput(ip->oq)) {
				mblk_t *mp;
				frtn_t fr = { &ip_free, (char *) skb };
				size_t plen = skb->len + (skb->data - skb->nh.raw);

				/* FIXME: handle non-linear sk_buffs */
				if ((mp = esballoc(skb->nh.raw, plen, BPRI_MED, &fr))) {
					mp->b_datap->db_type = M_DATA;
					mp->b_wptr += plen;
					put(ip->oq, mp);
				} else
					kfree_skb(skb);
			} else
				kfree_skb(skb);
		} else
			rtn = npi_v4_rcv_next(skb);
	}
	read_unlock_bh(&ip_prot_lock);
	return (rtn);
}

/**
 * ip_v4_err: - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information
 *
 * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned packet, and IP payload
 * of returned packet (up to some number of bytes of total payload).  The passed in sk_buff has
 * skb->data pointing to the ICMP payload which is the beginning of the returned IP header.
 * However, we include the entire packet in the message.
 */
STATIC void
ip_v4_err(struct sk_buff *skb, u32 info)
{
	read_lock_bh(&ip_prot_lock);
	{
		struct ip *ip;

		/* Note: use returned IP header and possibly payload for lookup */
		if ((ip = ip_lookup_icmp((struct iphdr *) skb->data, skb->len))) {
			if (ip->oq && canput(ip->oq)) {
				mblk_t *mp;
				size_t plen = skb->len + (skb->data - skb->nh.raw);

				if ((mp = allocb(plen, BPRI_MED))) {
					mp->b_datap->db_type = M_ERROR;
					bcopy(skb->nh.raw, mp->b_wptr, plen);
					mp->b_wptr += plen;
					put(ip->oq, mp);
				}
			}
		}
		npi_v4_err_next(skb, info);
	}
	read_unlock_bh(&ip_prot_lock);
	return;
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC int
ip_term_caches(void)
{
	if (ip_prot_cachep != NULL) {
		if (kmem_cache_destroy(ip_prot_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ip_prot_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed ip_prot_cachep\n", DRV_NAME));
		ip_prot_cachep = NULL;
	}
	if (ip_bind_cachep != NULL) {
		if (kmem_cache_destroy(ip_bind_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ip_bind_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed ip_bind_cachep\n", DRV_NAME));
		ip_bind_cachep = NULL;
	}
	if (ip_priv_cachep != NULL) {
		if (kmem_cache_destroy(ip_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ip_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed ip_priv_cachep\n", DRV_NAME));
		ip_priv_cachep = NULL;
	}
	return (0);
}
STATIC int
ip_init_caches(void)
{
	if (ip_priv_cachep == NULL) {
		ip_priv_cachep = kmem_cache_create("ip_priv_cachep", sizeof(ip_t), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (ip_priv_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate ip_priv_cachep", __FUNCTION__);
			ip_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (ip_bind_cachep == NULL) {
		ip_bind_cachep = kmem_cache_create("ip_bind_cachep", sizeof(ip_t), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (ip_bind_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate ip_bind_cachep", __FUNCTION__);
			ip_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver bind structure cache\n", DRV_NAME));
	}
	if (ip_prot_cachep == NULL) {
		ip_prot_cachep = kmem_cache_create("ip_prot_cachep", sizeof(ip_t), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (ip_prot_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate ip_prot_cachep", __FUNCTION__);
			ip_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver protocol structure cache\n", DRV_NAME));
	}
	return (0);
}

STATIC void
npi_term_hashes(void)
{
	if (npi_bhash) {
		free_pages((unsigned long) npi_bhash, npi_bhash_order);
		npi_bhash = NULL;
		npi_bhash_size = 0;
		npi_bhash_order = 0;
	}
	if (npi_chash) {
		free_pages((unsigned long) npi_chash, npi_chash_order);
		npi_chash = NULL;
		npi_chash_size = 0;
		npi_chash_order = 0;
	}
}
STATIC void
npi_init_hashes(void)
{
	int order, i;
	unsigned long goal;

	/* size and allocate bind hash table */
	goal = num_physpages >> (20 - PAGE_SHIFT);
	for (order = 0; (1 << order) < goal; order++) ;
	do {
		npi_bhash_order = order;
		npi_bhash_size = (1 << order) * PAGE_SIZE / sizeof(struct ip_bhash_bucket);
		npi_bhash = (struct ip_bhash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (npi_bhash == NULL && --order >= 0);
	if (!npi_bhash)
		cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
	npi_bhash_size = npi_chash_size = npi_bhash_size >> 1;
	npi_bhash_order = npi_chash_order = npi_bhash_order - 1;
	bzero(npi_bhash, npi_bhash_size * sizeof(struct ip_bhash_bucket));
	bzero(npi_chash, npi_chash_size * sizeof(struct ip_chash_bucket));
	for (i = 0; i < npi_bhash_size; i++)
		rwlock_init(&npi_bhash[i].lock);
	for (i = 0; i < npi_chash_size; i++)
		rwlock_init(&npi_chash[i].lock);
	printd(("%s: INFO: bind hash table configured size = %d\n", DRV_NAME, npi_bhash_size));
	printd(("%s: INFO: conn hash table configured size = %d\n", DRV_NAME, npi_chash_size));
}

/**
 * ip_alloc_priv: - allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @slp: pointer to place in list for insertion
 * @type: type of provider: 0, N_CONS, N_CLTS
 * @devp: pointer to device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initializes it to appropriate values, and then inserts it into
 * the private structure list.
 */
STATIC ip_t *
ip_alloc_priv(queue_t *q, ip_t ** slp, int type, dev_t *devp, cred_t *crp)
{
	ip_t *ip;

	if ((ip = ip_alloc())) {
		ip->u.dev.cmajor = getmajor(*devp);
		ip->u.dev.cminor = getminor(*devp);
		ip->cred = *crp;
		(ip->oq = q)->q_ptr = ip_get(ip);
		(ip->iq = WR(q))->q_ptr = ip_get(ip);
		ip->i_prim = &ip_w_prim;
		ip->o_prim = &ip_r_prim;
		// ip->i_wakeup = NULL;
		// ip->o_wakeup = NULL;
		spin_lock_init(&ip->qlock);	/* "ip-queue-lock" */
		// ip->ibid = 0;
		// ip->obid = 0;
		// ip->iwait = NULL;
		// ip->owait = NULL;
		ip->i_state = (-1);	// LMI_UNUSABLE;
		ip->i_style = (1);	// LMI_STYLE1;
		ip->i_version = 1;
		ip->i_oldstate = (1);	// LMI_UNUSABLE;
		/* ip specific members */
		ip->provider = type;
		/* link into master list */
		write_lock_bh(&master.lock);
		{
			ip_get(ip);
			if ((ip->next = *slp))
				ip->next->prev = &ip->next;
			ip->prev = slp;
			*slp = ip;
		}
		write_unlock_bh(&master.lock);
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
	return (ip);
}

/**
 * ip_free_priv: - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
STATIC void
ip_free_priv(queue_t *q)
{
	struct ip *ip;

	ensure(q, return);
	ip = IP_PRIV(q);
	ensure(ip, return);
	qprocsoff(q);
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&ip->refcnt)));
	ss7_unbufcall((str_t *) ip);
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME,
		atomic_read(&ip->refcnt)));
	write_lock_bh(&master.lock);
	{
		if ((*ip->prev = ip->next))
			ip->next->prev = ip->prev;
		ip->next = NULL;
		ip->prev = NULL;
	}
	write_unlock_bh(&master.lock);
	ip_put(ip);
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&ip->refcnt)));
	ip->oq->q_ptr = NULL;
	ip->oq = NULL;
	ip_put(ip);
	ip->iq->q_ptr = NULL;
	ip->iq = NULL;
	ip_put(ip);
	assure(atomic_read(&ip->refcnt) == 1);
	ip_put(ip);		/* should normally be final put */
	return;
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
#define FIRST_CMINOR	0
#define    IP_CMINOR	0
#define  IPCO_CMINOR	N_CONS
#define  IPCL_CMINOR	N_CLNS
#define  LAST_CMINOR	2
#define  FREE_CMINOR	3
STATIC int ip_majors[IP_CMAJORS] = { IP_CMAJOR_0, };

/**
 * ip_qopen: - NPI IP driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
ip_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	ip_t *ip, **ipp = &master.ip.list;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: can't push as module\n", DRV_NAME));
		return (EIO);
	}
#if defined LIS
	if (cmajor != IP_CMAJOR_0)
		return (ENXIO);
#endif
#if defined LFS
	/* Linux Fast-STREAMS always passes internal major device numbers (modules ids) */
	if (cmajor != IP_DRV_ID)
		return (ENXIO);
#endif
	if (cminor > LAST_CMINOR) {
		return (ENXIO);
	}
	type = cminor;
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = FREE_CMINOR;
	write_lock_bh(&master.lock);
	for (; *ipp; ipp = &(*ipp)->next) {
		if (cmajor != (*ipp)->u.dev.cmajor)
			break;
		if (cmajor == (*ipp)->u.dev.cmajor) {
			if (cminor < (*ipp)->u.dev.cminor)
				break;
			if (cminor == (*ipp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= IP_CMAJORS || !(cmajor = ip_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= IP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_bh(&master.lock);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(ip = ip_alloc_priv(q, ipp, type, devp, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_bh(&master.lock);
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
	qprocson(q);
	return (0);
}

/**
 * ip_qclose: - NPI IP driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
STATIC streamscall int
ip_qclose(queue_t *q, int oflag, cred_t *crp)
{
	ip_t *ip = PRIV(q);

	(void) oflag;
	(void) crp;
	(void) ip;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, ip->u.dev.cmajor,
		ip->u.dev.cminor));
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
	qprocsoff(q);
	ip_free_priv(q);
	goto quit;
      quit:
	return (0);
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

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the IP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the IP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw ip_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ip_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC struct devnode ip_node_ip = {
	.n_name = "ip",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode ip_node_ipco = {
	.n_name = "ipco",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode ip_node_ipcl = {
	.n_name = "ipcl",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC int
ip_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&ip_cdev, major)) < 0)
		return (err);
	register_strnod(&ip_cdev, &ip_node_ip, IP_CMINOR);
	register_strnod(&ip_cdev, &ip_node_ipco, IPCO_CMINOR);
	register_strnod(&ip_cdev, &ip_node_ipcl, IPCL_CMINOR);
	return (0);
}

STATIC int
ip_unregister_strdev(major_t major)
{
	int err;

	unregister_strnod(&ip_cdev, IP_CMINOR);
	unregister_strnod(&ip_cdev, IPCO_CMINOR);
	unregister_strnod(&ip_cdev, IPCL_CMINOR);
	if ((err = unregister_strdev(&ip_cdev, major)) < 0)
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
ip_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &ip_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
ip_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
ipterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (ip_majors[mindex]) {
			if ((err = ip_unregister_strdev(ip_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					ip_majors[mindex]);
			if (mindex)
				ip_majors[mindex] = 0;
		}
	}
	if ((err = ip_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	npi_term_hashes();
	return;
}

MODULE_STATIC int __init
ipinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	npi_init_hashes();
	if ((err = ip_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ipterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ip_register_strdev(ip_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					ip_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				ipterminate();
				return (err);
			}
		}
		if (ip_majors[mindex] == 0)
			ip_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(ip_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = ip_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ipinit);
module_exit(ipterminate);

#endif				/* LINUX */
