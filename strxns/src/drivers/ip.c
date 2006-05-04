/*****************************************************************************

 @(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/05/03 22:53:39 $

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

 Last Modified $Date: 2006/05/03 22:53:39 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip.c,v $
 Revision 0.9.2.26  2006/05/03 22:53:39  brian
 - working up NPI-IP driver

 Revision 0.9.2.25  2006/05/03 11:53:53  brian
 - changes for compile, working up NPI-IP driver

 Revision 0.9.2.24  2006/05/03 01:04:33  brian
 - corrections for compile

 Revision 0.9.2.23  2006/04/27 09:34:58  brian
 - working up NPI-IP driver some more

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

#ident "@(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/05/03 22:53:39 $"

static char const ident[] =
    "$RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/05/03 22:53:39 $";

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
#define IP_REVISION	"OpenSS7 $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/05/03 22:53:39 $"
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

STATIC streamscall int npi_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int npi_qclose(queue_t *, int, cred_t *);

STATIC struct qinit ip_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = npi_qopen,		/* Each open */
	.qi_qclose = npi_qclose,	/* Last close */
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

struct npi_bhash_bucket;
struct npi_chash_bucket;

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

typedef struct np {
	STR_DECLARATION (struct np);	/* Stream declaration */
	struct np *bnext;		/* linkage for bind/list hash */
	struct np **bprev;		/* linkage for bind/list hash */
	struct npi_bhash_bucket *bhash;	/* linkage for bind/list hash */
	struct np *cnext;		/* linkage for conn hash */
	struct np **cprev;		/* linkage for conn hash */
	struct npi_chash_bucket *chash;	/* linkage for conn hash */
	N_info_ack_t info;		/* network service provider information */
	unsigned int BIND_flags;	/* bind flags */
	unsigned int CONN_flags;	/* connect flags */
	unsigned int CONIND_number;	/* maximum number of oustanding connection indications */
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
	struct np_baddr baddrs[8];	/* bound addresses */
	unsigned short snum;		/* number of source (connected) addresses */
	unsigned short sport;		/* source (connected) port number - network order */
	struct np_saddr saddrs[8];	/* source (connected) addresses */
	unsigned short dnum;		/* number of destination (connected) addresses */
	unsigned short dport;		/* destination (connected) port number - network order */
	struct np_daddr daddrs[8];	/* destination (connected) addresses */
	struct N_qos_sel_info_ip qos;	/* network service provider quality of service */
	struct N_qos_range_info_ip qor;	/* network service provider quality of service range */
} np_t;

#define PRIV(__q) (((__q)->q_ptr))
#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (np, np);		/* master list of np (open) structures */
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
	struct np *owners;		/* list of owners of this protocol/port combination */
	struct np *dflt;		/* default listeners/destinations for this protocol */
};
struct ip_conn_bucket {
	struct ip_conn_bucket *next;	/* linkage of conn buckets for hash slot */
	struct ip_conn_bucket **prev;	/* linkage of conn buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short sport;		/* source port number (network order) */
	unsigned short dport;		/* destination port number (network order) */
	struct np *owners;		/* list of owners of this protocol/sport/dport combination */
};

struct npi_bhash_bucket {
	rwlock_t lock;
	struct np *list;
};
struct npi_chash_bucket {
	rwlock_t lock;
	struct np *list;
};

STATIC struct npi_bhash_bucket *npi_bhash;
STATIC size_t npi_bhash_size = 0;
STATIC size_t npi_bhash_order = 0;

STATIC struct npi_chash_bucket *npi_chash;
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

STATIC kmem_cache_t *npi_bind_cachep;
STATIC kmem_cache_t *npi_prot_cachep;
STATIC kmem_cache_t *npi_priv_cachep;

STATIC INLINE struct np *
np_get(struct np *np)
{
	if (np)
		atomic_inc(&np->refcnt);
	return (np);
}
STATIC INLINE void
np_put(struct np *np)
{
	if (np)
		if (atomic_dec_and_test(&np->refcnt)) {
			kmem_cache_free(npi_priv_cachep, np);
		}
}
STATIC INLINE void
np_release(struct np **npp)
{
	if (npp != NULL)
		np_put(XCHG(npp, NULL));
}
STATIC INLINE struct np *
np_alloc(void)
{
	struct np *np;

	if ((np = kmem_cache_alloc(npi_priv_cachep, SLAB_ATOMIC))) {
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
npi_set_state(struct np *np, np_ulong state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, np, state_name(state), state_name(np->i_state)));
	np->i_state = state;
}

STATIC INLINE fastcall np_ulong
npi_get_state(struct np *np)
{
	return (np->i_state);
}

STATIC INLINE fastcall np_ulong
npi_chk_state(struct np * np, np_ulong mask)
{
	return (((1 << np->i_state) & (mask)) != 0);
}

STATIC INLINE fastcall np_ulong
npi_not_state(struct np * np, np_ulong mask)
{
	return (((1 << np->i_state) & (mask)) == 0);
}

/*
 *  =========================================================================
 *
 *  IP Local Management
 *
 *  =========================================================================
 */

STATIC int npi_v4_rcv(struct sk_buff *skb);
STATIC void npi_v4_err(struct sk_buff *skb, u32 info);

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
 *
 * This function returns zero (0) if the packet has not or will not be seen by another packet
 * handler, and one (1) if the packet has or will be seen by another packet handler.  This return
 * value is used to determine whether to generate ICMP errors or not.
 */
STATIC INLINE fastcall __hot_in int
npi_v4_rcv_next(struct sk_buff *skb)
{
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
	struct ip_prot_bucket *pb;
	struct net_protocol *pp;
	unsigned char proto;

	proto = skb->nh.iph->protocol;
	if ((pb = ip_prots[proto]) && (pp = pb->prot.next)) {
		pp->handler(skb);
		return (1);
	}
	kfree_skb(skb);
	return (0);
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
	struct ip_prot_bucket *pb;
	unsigned char proto;

	proto = skb->nh.iph->protocol;
	kfree_skb(skb);
	if ((pb = ip_prots[proto]))
		return (pb->prot.copy != 0);
	return (0);
#endif
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
	struct net_protocol *pp;
	unsigned char proto;

	proto = ((struct iphdr *) skb->data)->protocol;
	if ((pb = ip_prots[proto])
	    && (pp = pb->prot.next))
		pp->err_handler(skb, info);
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
	struct inet_protocol *pp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	write_lock_bh(&ip_prot_lock);
	if ((pb = ip_prots[proto]) != NULL) {
		pb->refs++;
		if (type & N_CONS)
			++pb->corefs;
		if (type & N_CLNS)
			++pb->clrefs;
	} else if ((pb = kmem_cache_alloc(npi_prot_cachep, SLAB_ATOMIC))) {
		pb->refs = 1;
		pb->corefs = (type & N_CONS) ? 1 : 0;
		pb->clrefs = (type & N_CLNS) ? 1 : 0;
		pp = &pb->prot;
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
		(void) hash;
		pp->protocol = proto;
		pp->name = "streams-ip";
		pp->handler = &npi_v4_rcv;
		pp->err_handler = &npi_v4_err;
		pp->copy = 0;
		pp->next = NULL;
		inet_add_protocol(pp);
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO
		pp->proto.proto = proto;
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO */
		pp->proto.handler = &npi_v4_rcv;
		pp->proto.err_handler = &npi_v4_err;
		pp->proto.no_policy = 1;
		pp->next = NULL;
		pp->kmod = NULL;
		spin_lock_bh(inet_proto_lockp);
		if ((pp->next = inet_protosp[hash]) != NULL) {
			if ((pp->kmod = module_text_address((ulong) pp->next))
			    && pp->kmod != THIS_MODULE) {
				if (!try_module_get(pp->kmod)) {
					spin_unlock_bh(inet_proto_lockp);
					kmem_cache_free(npi_prot_cachep, pb);
					return (NULL);
				}
			}
		}
		inet_protosp[hash] = &pp->proto;
		spin_unlock_bh(inet_proto_lockp);
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
			struct inet_protocol *pp = &pb->prot;

#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
			inet_del_protocol(pp);
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
			spin_lock_bh(inet_proto_lockp);
			inet_protosp[proto] = pp->next;
			spin_unlock_bh(inet_proto_lockp);
			synchronize_net();
			if (pp->next != NULL && pp->kmod != NULL && pp->kmod != THIS_MODULE)
				module_put(pp->kmod);
#else
#error
#endif
			/* unlink from hash slot */
			ip_prots[proto] = NULL;
			kmem_cache_free(npi_prot_cachep, pb);
		}
	}
	write_unlock_bh(&ip_prot_lock);
}
#endif				/* LINUX */

/**
 *  npi_bind_prot -  bind a protocol
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
npi_bind_prot(unsigned char proto, unsigned int type)
{
	struct ip_prot_bucket *pb;

	if ((pb = npi_init_nproto(proto, type)))
		return (0);
	return (-ENOMEM);
}

/**
 *  npi_unbind_prot - unbind a protocol
 *  @proto:	    protocol number to unbind
 */
STATIC void
npi_unbind_prot(unsigned char proto, unsigned int type)
{
	npi_term_nproto(proto, type);
}

#if 0
/**
 *  npi_bind_addr -  bind an address and protocol id to a Stream
 *  @np:	    Stream to which to bind
 *  @add:	    sockaddr_in structure with address to bind
 *  @proto:	    protocol id to bind
 *  @type:	    type of bind (N_CLNS|N_CONS)
 *  @flag:	    default bind
 */
STATIC int
npi_bind_addr(struct np *np, struct sockaddr_in *addr, unsigned char proto, unsigned int type,
	      int flag)
{
	int err;

	if ((err = npi_bind_prot(proto, type)) != 0)
		goto error;
	/* FIXME: now bind to address... */
      error:
	return (err);
}

/**
 *  npi_unbind_addr - unbind an address and protocol id from a Stream
 *  @np:		    Stream to which to unbind
 *  @add:	    sockaddr_in structure with address to unbind
 *  @proto:	    protocl id to unbind
 *  @type:	    type of bind (N_CLNS|N_CONS)
 *  @flag:	    default unbind
 */
STATIC void
npi_unbind_addr(struct np *np, struct sockaddr_in *addr, unsigned char proto, unsigned int type,
		int flag)
{
	/* FIXME: unbind from address... */
	npi_unbind_prot(proto, type);
}
#endif

/**
 * npi_bind - bind a Stream to an NSAP
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
 */
STATIC INLINE fastcall int
npi_bind(struct np *np, unsigned char *PROTOID_buffer, size_t PROTOID_length,
	 struct sockaddr_in *ADDR_buffer, socklen_t ADDR_length, np_ulong CONIND_number,
	 np_ulong BIND_flags)
{
	struct npi_bhash_bucket *hp;
	unsigned short bport = ADDR_buffer[0].sin_port;
	unsigned char proto = PROTOID_buffer[0];
	size_t anum = ADDR_length / sizeof(*ADDR_buffer);
	struct np *np2;
	int i, j, err;

	(void) npi_term_nproto;
	(void) npi_unbind_prot;
	PROTOID_length = 1;
	hp = &npi_bhash[npi_bhashfn(proto, bport)];
	write_lock_bh(&hp->lock);
	for (np2 = hp->list; np2; np2 = np2->bnext) {
		if (bport == np2->bport) {
			/* Allowed to bind to each NSAP once as DEFAULT_DEST, once as
			   DEFAULT_LISTENER and once as neither. */
			if ((BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)) !=
			    (np2->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
				continue;
			for (i = 0; i < np2->bnum; i++) {
				if (np2->baddrs[i].addr == 0)
					break;
				for (j = 0; j < anum; j++)
					if (np2->baddrs[i].addr == ADDR_buffer[j].sin_addr.s_addr)
						break;
				if (j < anum)
					break;
			}
			if (i < np2->bnum)
				break;
		}
	}
	if (np2 != NULL) {
		write_unlock_bh(&hp->lock);
		/* There is a question as to which error should be returned when a protocol address 
		   is already bound.

		   NPI 2.0.0 says that "[i]f the NS provider cannot bind the specified address, it
		   may assign another network address to the user.  It is the network user's
		   responsibility to check the network address returned int he N_BIND_ACK primitive
		   to see if it is the same as the one requested."

		   NPI 2.0.0 says "[o]nly one default listener Stream is allowed per occurrence of
		   NPI.  An attempt to bind a default listener Stream when one is already bound
		   should result in an error (of type NBOUND)" and "[o]nly one default destination
		   stream per NSAP is alloed per occurence of NPI.  An attempt to bind a default
		   destination stream to an NSAP when one is already bound should result in an error
		   of type NBOUND." But aslo, "NBOUND: The NS user attempted to bind a second Stream
		   to a network address with the CONIND_number set to a non-zero value, or attempted
		   to bind a second Stream with the DEFAULT_LISTENER flag value set to non-zero."

		   However, we return NBOUND for a connectionless bind when an attempt is made to
		   bind a second address to the same NSAP where either both have the DEFAULT_DEST
		   flag set or both have the DEFAULT_DEST flag clear. */
		return (NBOUND);
	}
	if ((err = npi_bind_prot(proto, np->info.SERV_type))) {
		write_unlock_bh(&hp->lock);
		return (err);
	}
	if ((np->bnext = hp->list))
		np->bnext->bprev = &np->bnext;
	np->bprev = &hp->list;
	hp->list = np_get(np);
	np->bhash = hp;
	/* copy into private structure */
	np->CONIND_number = CONIND_number;
	np->BIND_flags = BIND_flags;
	for (i = 0; i < PROTOID_length; i++)
		np->protoids[i] = PROTOID_buffer[i];
	np->bnum = anum;
	np->bport = ADDR_buffer[0].sin_port;
	for (i = 0; i < anum; i++)
		np->baddrs[i].addr = ADDR_buffer[i].sin_addr.s_addr;
	write_unlock_bh(&hp->lock);
	return (0);
}

#ifdef HAVE_KFUNC_DST_MTU
/* Why do stupid people rename things like this? */
#undef dst_pmtu
#define dst_pmtu dst_mtu
#else
#ifndef HAVE_STRUCT_DST_ENTRY_PATH
static inline u32
dst_pmtu(struct dst_entry *dst)
{
	return (dst->pmtu);
}
#endif
#endif

/**
 * npi_connect - form a connection
 * @np: private structure
 *
 * Destination addresses and port number as well as connection request quality of service parameters
 * should already be stored into the private structure.  Yes, this information will remain if there
 * is an error in the connection request.  When any primitive containing options fails and returns
 * and error, it is the caller's responsibility to set again the values of the options.
 */
STATIC INLINE fastcall int
npi_connect(struct np *np, struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	    struct N_qos_sel_conn_ip * QOS_buffer, np_ulong CONN_flags)
{
	unsigned char proto = np->protoids[0];
	unsigned short pmtu = 65535;
	uint32_t saddr, daddr;
	size_t dnum = DEST_length / sizeof(*DEST_buffer);
	int i, j;

	if (QOS_buffer->mtu > 0 && QOS_buffer->mtu != QOS_UNKNOWN && QOS_buffer->mtu < pmtu)
		pmtu = QOS_buffer->mtu;

	if (QOS_buffer->protocol != QOS_UNKNOWN) {
		int found = 0;

		for (i = 0; i < np->pnum; i++) {
			if (QOS_buffer->protocol == np->protoids[i]) {
				found = 1;
				break;
			}
		}
		if (found == 0)
			return (NBADQOSPARAM);
		proto = QOS_buffer->protocol;
	}

	/* The default destination address is the first address in the list. */

	daddr = DEST_buffer[0].sin_addr.s_addr;

	/* The default source address is the source address of the outgoing interface. */

	saddr = 0;

	/* Destination addresses have been checked as follows: they have been aligned. There is at
	   least 1 address and no more than 8 addresses.  The first address has an address family
	   type of AF_INET or zero (0).  No IP address in the list is INADDR_ANY.  Things that have
	   not been checked are: there might be duplicates in the list.  The user might not have the 
	   necessary privilege to use some of the addresses.  Some addresses might be zeronet,
	   broadcast or multicast addresses. The addresses might be of disjoint scope.  There might
	   not exist a route to some addresses.  The destination port number might be zero. */

	np->dport = DEST_buffer[0].sin_port;

	if (np->dport == 0 && (np->bport != 0 || np->sport != 0)) {
		return (NBADADDR);
	}
	if (np->dport != 0 && np->sport == 0) {
		/* TODO: really need to autobind the stream to a dynamically allocated source port
		   number. */
		return (NBADADDR);
	}

	for (i = 0; i < dnum; i++) {
		struct rtable *rt = NULL;
		int err;

		if ((err = ip_route_output(&rt, DEST_buffer[i].sin_addr.s_addr, 0, 0, 0))) {
			while (--i >= 0)
				dst_release(xchg(&np->daddrs[i].dst, NULL));
			return (err);
		}
		np->daddrs[i].dst = &rt->u.dst;

		/* Note that we do not have to use the destination reference cached above.  It is
		   enough that we hold a reference to it so that it remains in the routing caches
		   so lookups to this destination are fast.  They will be released upon
		   disconnection. */

		np->daddrs[i].addr = DEST_buffer[i].sin_addr.s_addr;
		np->daddrs[i].ttl = QOS_buffer->ttl;
		np->daddrs[i].tos = QOS_buffer->tos;
		np->daddrs[i].mtu = dst_pmtu(np->daddrs[i].dst);
		if (np->daddrs[i].mtu < pmtu)
			pmtu = np->daddrs[i].mtu;
	}
	np->dnum = dnum;

	/* try to place in connection hashes with conflict checks */
	{
		unsigned short sport = np->sport;
		unsigned short dport = np->dport;
		struct np *conflict = NULL;
		struct npi_chash_bucket *hp, *hp1, *hp2;

		hp1 = &npi_chash[npi_chashfn(proto, dport, sport)];
		hp2 = &npi_chash[npi_chashfn(proto, 0, 0)];

		write_lock_bh(&hp1->lock);
		if (hp1 != hp2)
			read_lock(&hp2->lock);

		hp = hp1;
		do {
			struct np *np2;
			np_ulong state;

			for (np2 = hp->list; np2; np2 = np2->cnext) {
				if ((state = npi_get_state(np2)) != NS_DATA_XFER
				    && state != NS_WRES_RIND)
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
					for (j = 0; conflict == NULL && j < dnum; j++)
						if (np2->daddrs[i].addr == np->daddrs[j].addr)
							conflict = np2;
				if (conflict == NULL)
					continue;
				break;
			}
		} while (conflict == NULL && hp != hp2 && (hp = hp2));
		if (conflict != NULL) {
			if (hp1 != hp2)
				read_unlock(&hp2->lock);
			write_unlock_bh(&hp1->lock);
			/* free dst caches */
			for (i = 0; i < dnum; i++)
				dst_release(XCHG(&np->daddrs[i].dst, NULL));
			np->dnum = 0;
			np->dport = 0;
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
			read_unlock(&hp2->lock);
		write_unlock_bh(&hp1->lock);
	}

	/* XXX should this be inside locks? */
	/* store returned qos values */
	np->qos.protocol = proto;
	if (QOS_buffer->priority != QOS_UNKNOWN)
		np->qos.priority = QOS_buffer->priority;
	if (QOS_buffer->ttl != QOS_UNKNOWN)
		np->qos.ttl = QOS_buffer->ttl;
	if (QOS_buffer->tos != QOS_UNKNOWN)
		np->qos.tos = QOS_buffer->tos;
	np->qos.mtu = pmtu;
	np->qos.saddr = saddr;
	np->qos.daddr = daddr;
	return (0);
}

STATIC int
npi_reset(struct np *np, struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	  np_ulong RESET_orig, np_ulong RESET_reason)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
npi_optmgmt(struct np *np, void *QOS_buffer, np_ulong OPTMGMT_flags)
{
	np_ulong type;
	int i;

	switch ((type = *(typeof(type) *) QOS_buffer)) {
	case N_QOS_SEL_INFO_IP:
	{
		struct N_qos_sel_info_ip *qos = (typeof(qos)) QOS_buffer;

		/* protocol must be one of the bound protocol ids */
		if (qos->protocol != QOS_UNKNOWN) {
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == qos->protocol)
					break;
			if (i >= np->pnum)
				return (NBADQOSPARAM);
		}
		if (qos->priority != QOS_UNKNOWN) {
			if ((np_long) qos->priority < np->qor.priority.priority_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->priority > np->qor.priority.priority_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->ttl != QOS_UNKNOWN) {
			if ((np_long) qos->ttl < np->qor.ttl.ttl_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->ttl > np->qor.ttl.ttl_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->tos != QOS_UNKNOWN) {
			if ((np_long) qos->tos < np->qor.tos.tos_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->tos > np->qor.tos.tos_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->mtu != QOS_UNKNOWN) {
			if ((np_long) qos->mtu < np->qor.mtu.mtu_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->mtu > np->qor.mtu.mtu_max_value)
				return (NBADQOSPARAM);
		}
		/* source address should be one of the specified source addresses */
		if (qos->saddr != QOS_UNKNOWN) {
			if (qos->saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == qos->saddr)
						break;
				}
				if (i >= np->snum)
					return (NBADQOSPARAM);
			}
		}
		/* destination address must be one of the specified destination addresses */
		if (qos->daddr != QOS_UNKNOWN) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == qos->daddr)
					break;
			if (i >= np->dnum)
				return (NBADQOSPARAM);
		}
		if (qos->protocol != QOS_UNKNOWN)
			np->qos.protocol = qos->protocol;
		if (qos->priority != QOS_UNKNOWN)
			np->qos.priority = qos->priority;
		if (qos->ttl != QOS_UNKNOWN)
			np->qos.ttl = qos->ttl;
		if (qos->tos != QOS_UNKNOWN)
			np->qos.tos = qos->tos;
		if (qos->mtu != QOS_UNKNOWN)
			np->qos.mtu = qos->mtu;
		if (qos->saddr != QOS_UNKNOWN)
			np->qos.saddr = qos->saddr;
		if (qos->daddr != QOS_UNKNOWN)
			np->qos.daddr = qos->daddr;
		break;
	}
	case N_QOS_RANGE_INFO_IP:
	{
		struct N_qos_range_info_ip *qos = (typeof(qos)) QOS_buffer;

		(void) qos;
		return (NBADQOSTYPE);
	}
	case N_QOS_SEL_CONN_IP:
	{
		struct N_qos_sel_conn_ip *qos = (typeof(qos)) QOS_buffer;
		int i;

		if (!(np->info.SERV_type & N_CONS))
			return (NBADQOSTYPE);
		/* protocol must be one of the bound protocol ids */
		if (qos->protocol != QOS_UNKNOWN) {
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == qos->protocol)
					break;
			if (i >= np->pnum)
				return (NBADQOSPARAM);
		}
		if (qos->priority != QOS_UNKNOWN) {
			if ((np_long) qos->priority < np->qor.priority.priority_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->priority > np->qor.priority.priority_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->ttl != QOS_UNKNOWN) {
			if ((np_long) qos->ttl < np->qor.ttl.ttl_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->ttl > np->qor.ttl.ttl_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->tos != QOS_UNKNOWN) {
			if ((np_long) qos->tos < np->qor.tos.tos_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->tos > np->qor.tos.tos_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->mtu != QOS_UNKNOWN) {
			if ((np_long) qos->mtu < np->qor.mtu.mtu_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->mtu > np->qor.mtu.mtu_max_value)
				return (NBADQOSPARAM);
		}
		/* source address should be one of the specified source addresses */
		if (qos->saddr != QOS_UNKNOWN) {
			if (qos->saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == qos->saddr)
						break;
				}
				if (i >= np->snum)
					return (NBADQOSPARAM);
			}
		}
		/* destination address must be one of the specified destination addresses */
		if (qos->daddr != QOS_UNKNOWN) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == qos->daddr)
					break;
			if (i >= np->dnum)
				return (NBADQOSPARAM);
		}
		if (qos->protocol != QOS_UNKNOWN)
			np->qos.protocol = qos->protocol;
		if (qos->priority != QOS_UNKNOWN)
			np->qos.priority = qos->priority;
		if (qos->ttl != QOS_UNKNOWN)
			np->qos.ttl = qos->ttl;
		if (qos->tos != QOS_UNKNOWN)
			np->qos.tos = qos->tos;
		if (qos->mtu != QOS_UNKNOWN)
			np->qos.mtu = qos->mtu;
		if (qos->saddr != QOS_UNKNOWN)
			np->qos.saddr = qos->saddr;
		if (qos->daddr != QOS_UNKNOWN)
			np->qos.daddr = qos->daddr;
		break;
	}
	case N_QOS_SEL_RESET_IP:
	{
		struct N_qos_sel_reset_ip *qos = (typeof(qos)) QOS_buffer;

		if (!(np->info.SERV_type & N_CONS))
			return (NBADQOSTYPE);
		if (qos->ttl != QOS_UNKNOWN) {
			if ((np_long) qos->ttl < np->qor.ttl.ttl_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->ttl > np->qor.ttl.ttl_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->tos != QOS_UNKNOWN) {
			if ((np_long) qos->tos < np->qor.tos.tos_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->tos > np->qor.tos.tos_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->mtu != QOS_UNKNOWN) {
			if ((np_long) qos->mtu < np->qor.mtu.mtu_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->mtu > np->qor.mtu.mtu_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->ttl != QOS_UNKNOWN)
			np->qos.ttl = qos->ttl;
		if (qos->tos != QOS_UNKNOWN)
			np->qos.tos = qos->tos;
		if (qos->mtu != QOS_UNKNOWN)
			np->qos.mtu = qos->mtu;
		break;
	}
	case N_QOS_SEL_UD_IP:
	{
		struct N_qos_sel_ud_ip *qos = (typeof(qos)) QOS_buffer;

		if (!(np->info.SERV_type & N_CLNS))
			return (NBADQOSTYPE);
		/* protocol must be one of the bound protocol ids */
		if (qos->protocol != QOS_UNKNOWN) {
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == qos->protocol)
					break;
			if (i >= np->pnum)
				return (NBADQOSPARAM);
		}
		if (qos->priority != QOS_UNKNOWN) {
			if ((np_long) qos->priority < np->qor.priority.priority_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->priority > np->qor.priority.priority_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->ttl != QOS_UNKNOWN) {
			if ((np_long) qos->ttl < np->qor.ttl.ttl_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->ttl > np->qor.ttl.ttl_max_value)
				return (NBADQOSPARAM);
		}
		if (qos->tos != QOS_UNKNOWN) {
			if ((np_long) qos->tos < np->qor.tos.tos_min_value)
				return (NBADQOSPARAM);
			if ((np_long) qos->tos > np->qor.tos.tos_max_value)
				return (NBADQOSPARAM);
		}
		/* source address should be one of the specified source addresses */
		if (qos->saddr != QOS_UNKNOWN) {
			if (qos->saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == qos->saddr)
						break;
				}
				if (i >= np->snum)
					return (NBADQOSPARAM);
			}
		}
		if (qos->protocol != QOS_UNKNOWN)
			np->qos.protocol = qos->protocol;
		if (qos->priority != QOS_UNKNOWN)
			np->qos.priority = qos->priority;
		if (qos->ttl != QOS_UNKNOWN)
			np->qos.ttl = qos->ttl;
		if (qos->tos != QOS_UNKNOWN)
			np->qos.tos = qos->tos;
		if (qos->saddr != QOS_UNKNOWN)
			np->qos.saddr = qos->saddr;
		break;
	}
	default:
		return (NBADQOSTYPE);
	}
	return (0);
}

/**
 * npi_unbind - unbind a Stream from an NSAP
 * @np: private structure
 *
 * Simply remove the Stream from the bind hashes and release a reference to the Stream.
 */
STATIC int
npi_unbind(struct np *np)
{
	struct npi_bhash_bucket *hp;

	if ((hp = np->bhash) != NULL) {
		write_lock_bh(&hp->lock);
		if ((*np->bprev = np->bnext))
			np->bnext->bprev = np->bprev;
		np->bnext = NULL;
		np->bprev = &np->bnext;
		np->bhash = NULL;
		np_release(&np);
		write_unlock_bh(&hp->lock);
		return (0);
	}
	return (-EALREADY);
}

STATIC int
npi_passive(struct np *np, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	    struct N_qos_sel_conn_ip * QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value,
	    np_ulong CONN_flags)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}

STATIC int
npi_disconnect(struct np *np, struct sockaddr_in *res, mblk_t *seq, np_ulong reason)
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

/**
 * npi_unitdata_req - process a unit data request
 * @q: write queue
 * @DEST_buffer: destintation address (or NULL)
 * @QOS_buffer: quality of service parameters (or NULL)
 * @mp: message
 *
 * Note that if @DEST_buffer is %NULL, then the message @mp starts with the IP header.  If
 * @DEST_buffer is non-NULL, then the message @mp starts with the transport header.
 */
STATIC INLINE fastcall __hot_put int
npi_unitdata_req(queue_t *q, struct sockaddr_in *DEST_buffer, struct N_qos_sel_ud_ip *QOS_buffer,
		 mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	struct rtable *rt = NULL;
	struct iphdr *iph;

	int protocol = np->qos.protocol;
	int priority = np->qos.priority;
	int tos = np->qos.tos;
	int ttl = np->qos.ttl;
	uint32_t saddr = np->qos.saddr;
	uint32_t daddr = np->qos.daddr;

	size_t ilen;

	if (!DEST_buffer) {
		/* no destination, packet includes IP header */
		iph = (struct iphdr *) mp->b_rptr;
		protocol = iph->protocol;
		priority = mp->b_band;
		tos = iph->tos;
		ttl = iph->ttl;
		saddr = iph->saddr;
		if (saddr == 0)
			saddr = np->qos.saddr;
		daddr = iph->daddr;
		if (daddr == 0)
			daddr = np->qos.daddr;
		if (iph->ihl < 5)
			iph->ihl = 5;
		ilen = (iph->ihl << 2);
	} else if (QOS_buffer) {
		/* if QOS provided, use values from QOS structure */
		if (QOS_buffer->protocol != QOS_UNKNOWN)
			protocol = QOS_buffer->protocol;
		if (QOS_buffer->priority != QOS_UNKNOWN)
			priority = QOS_buffer->priority;
		if (QOS_buffer->tos != QOS_UNKNOWN)
			tos = QOS_buffer->tos;
		if (QOS_buffer->ttl != QOS_UNKNOWN)
			ttl = QOS_buffer->ttl;
		if (QOS_buffer->saddr != QOS_UNKNOWN)
			saddr = QOS_buffer->saddr;
		if (saddr == 0)
			saddr = np->qos.saddr;
		daddr = DEST_buffer->sin_addr.s_addr;
		if (daddr == 0)
			daddr = np->qos.daddr;
		ilen = 0;
	} else {
		protocol = np->qos.protocol;
		priority = np->qos.priority;
		tos = np->qos.tos;
		ttl = np->qos.ttl;
		saddr = np->qos.saddr;
		daddr = DEST_buffer->sin_addr.s_addr;
		if (daddr == 0)
			daddr = np->qos.daddr;
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
				/* copy and consume np header (with options) if it was provided */
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

STATIC INLINE fastcall __hot_put int
npi_data_req(queue_t *q, mblk_t *mp)
{
	/* FIXME: this is really a connection-oriented send.  It is not necessary to reroute each
	   time that we send to a destination: we have cached a route to each destination.  We only
	   need to check if the route is obsolete and send on it.  This is somewhat different from
	   the connectionless send which has no cached route.  Nevetheless, for the moment we
	   reroute each packet.  Because we have cached the route, these per-packet lookups should
	   be fast anyway.  */

	return npi_unitdata_req(q, NULL, NULL, mp);
}

STATIC INLINE fastcall int
npi_exdata_req(queue_t *q, mblk_t *mp)
{
	/* FIXME: this is really a connection-oriented send.  It is not necessary to reroute each
	   time that we send to a destination: we have cached a route to each destination.  We only
	   need to check if the route is obsolete and send on it.  This is somewhat different from
	   the connectionless send which has no cached route.  Nevetheless, for the moment we
	   reroute each packet.  Because we have cached the route, these per-packet lookups should
	   be fast anyway.  */

	/* not supported */
	return (-EOPNOTSUPP);
}

STATIC INLINE fastcall int
npi_datack_req(queue_t *q)
{
	/* not supported */
	return (-EOPNOTSUPP);
}

/*
 *  ===================================================================
 *
 *  NPI Provider --> NPI User (Transport) Primitives
 *
 *  ===================================================================
 */
/**
 * ne_error_reply: - reply to a message with an M_ERROR message
 * @q: active queue in queue pair (read queue)
 * @error: error number
 *
 * FIXME: This must process other errors as well.
 */
STATIC int
ne_error_reply(queue_t *q, long error)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (error);
	case 0:
	case 1:
	case 2:
		return (error);
	}
	if ((mp = ss7_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = (error < 0) ? -error : error;
		*(mp->b_wptr)++ = (error < 0) ? -error : error;
		/* make sure the stream is disconnected */
		if (np->chash != NULL) {
			npi_disconnect(np, NULL, NULL, 0);
			npi_set_state(np, NS_IDLE);
		}
		/* make sure the stream is unbound */
		if (np->bhash != NULL) {
			npi_unbind(np);
			npi_set_state(np, NS_UNBND);
		}
		putnext(np->oq, mp);
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
	struct np *np = NP_PRIV(q);
	int i;
	mblk_t *mp;
	N_info_ack_t *p;
	struct sockaddr_in *ADDR_buffer;
	struct N_qos_sel_info_ip *QOS_buffer = &np->qos;
	struct N_qos_range_info_ip *QOS_range_buffer = &np->qor;
	unsigned char *PROTOID_buffer = np->protoids;
	size_t ADDR_length = np->snum * sizeof(*ADDR_buffer);
	size_t QOS_length = sizeof(*QOS_buffer);
	size_t QOS_range_length = sizeof(*QOS_range_buffer);
	size_t PROTOID_length = np->pnum;
	size_t size = sizeof(*p) + ADDR_length + QOS_length + QOS_range_length + PROTOID_length;

	if ((mp = ss7_allocb(q, size, BPRI_MED)) == NULL)
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = 65535;
	p->ENSDU_size = 0;
	p->CDATA_size = 65535;
	p->DDATA_size = 65535;
	p->ADDR_size = sizeof(struct sockaddr_storage);
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	/* QOS_length: In the connection-mode environment, when this primitive is invoked before
	   the NC is established on the stream, the values returned specify the default values
	   supported by the NS provider.  When this primitive is invoked after a NC has been
	   established on the stream, the values returned indicated negotiated values for the QOS
	   parameters.  In the connection-less environment, these values represent the default or
	   the selected QOS parameter values.  In case a QOS parameter is not supported by the NS
	   Provider, a value of QOS_UNKNOWN will be returned.  In the case where no QOS parameters
	   are supported by the NS provider, this field will be zero. */
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + ADDR_length : 0;
	/* QOS_range_length: These ranges are used by the NS user to select QOS parameter values
	   that are valid with the NS provider.  QOS parameter values are selected, or the default
	   values altered via the N_OPTMGMT_REQ primitive.  In the connection-mode environment, the 
	   values for end-to-end QOS parameter may be specified with the N_CONN primitives for
	   negotiation. If the NS provider does not support a certain QOS parameter, its value will 
	   be set to QOS_UNKNOWN.  In the case where no QOS parameters are supported by the NS
	   provider, the length of this field will be zero. */
	p->QOS_range_length = QOS_range_length;
	p->QOS_range_offset = QOS_range_length ? sizeof(*p) + ADDR_length + QOS_length : 0;
	p->OPTIONS_flags = 0;
	p->NIDU_size = 65535;
	p->SERV_type = np->info.SERV_type ? np->info.SERV_type : (N_CONS | N_CLNS);
	p->CURRENT_state = npi_get_state(np);
	p->PROVIDER_type = N_SNICFP;
	p->NODU_size = np->qos.mtu ? : 536;
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset = PROTOID_length ? sizeof(*p) + ADDR_length + QOS_length + QOS_range_length : 0;
	p->NPI_version = N_VERSION_2;
	if (ADDR_length) {
		for (i = 0; i < np->snum; i++) {
			ADDR_buffer = (struct sockaddr_in *) mp->b_wptr++;
			ADDR_buffer->sin_family = AF_INET;
			ADDR_buffer->sin_port = np->sport;
			ADDR_buffer->sin_addr.s_addr = np->saddrs[i].addr;
		}
	}
	if (QOS_length) {
		bcopy(QOS_buffer, mp->b_wptr, QOS_length);
		mp->b_wptr += QOS_length;
	}
	if (QOS_range_length) {
		bcopy(QOS_range_buffer, mp->b_wptr, QOS_range_length);
		mp->b_wptr += QOS_range_length;
	}
	if (PROTOID_length) {
		bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_length);
		mp->b_wptr += PROTOID_length;
	}
	putnext(np->oq, mp);
	return (QR_DONE);

      enobufs:
	return (-ENOBUFS);
}

/**
 * ne_bind_ack: - NE_BIND_ACK event
 * @q: active queue in queue pair (write queue)
 * @PROTOID_buffer: protocol ids to bind
 * @PROTOID_length: length of protocol ids
 * @ADDR_buffer: addresses to bind
 * @ADDR_length: length of addresses
 * @CONIND_number: maximum number of connection indications
 * @BIND_flags: bind flags
 *
 * Generate an N_BIND_ACK and pass it upstream.
 */
STATIC INLINE fastcall int
ne_bind_ack(queue_t *q, unsigned char *PROTOID_buffer, size_t PROTOID_length,
	    struct sockaddr_in *ADDR_buffer, socklen_t ADDR_length, np_ulong CONIND_number,
	    np_ulong BIND_flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_bind_ack_t *p;
	size_t size = sizeof(*p) + ADDR_length + PROTOID_length;
	np_long NPI_error;

	if (unlikely(npi_get_state(np) != NS_WACK_BREQ))
		goto efault;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	if (unlikely((NPI_error = npi_bind(np, PROTOID_buffer, PROTOID_length,
					   ADDR_buffer, ADDR_length, CONIND_number,
					   BIND_flags)) != 0))
		goto free_error;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = CONIND_number;
	p->TOKEN_value = (long) RD(q);
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset = PROTOID_length ? sizeof(*p) + ADDR_length : 0;
	if (ADDR_length) {
		bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
		mp->b_wptr += ADDR_length;
	}
	if (PROTOID_length) {
		bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_length);
		mp->b_wptr += PROTOID_length;
	}
	npi_set_state(np, NS_IDLE);
	putnext(np->oq, mp);
	return (QR_DONE);

      free_error:
	freeb(mp);
	goto error;
      enobufs:
	NPI_error = -ENOBUFS;
	goto error;
      efault:
	NPI_error = -EFAULT;
	goto error;
      error:
	return (NPI_error);
}

/**
 * ne_error_ack: - generate an N_ERROR_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @ERROR_prim: primitive in error
 * @NPI_error: error (positive NPI_error, negative UNIX_error)
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
ne_error_ack(queue_t *q, int ERROR_prim, np_long NPI_error)
{
	mblk_t *mp;
	N_error_ack_t *p;

	switch (NPI_error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (NPI_error);
	case 0:
		return (NPI_error);
	case -EADDRINUSE:
		NPI_error = NBOUND;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		NPI_error = NNOADDR;
		break;
	case -EMSGSIZE:
		NPI_error = NBADDATA;
		break;
	case -EACCES:
		NPI_error = NACCESS;
		break;
	case -EOPNOTSUPP:
		NPI_error = NNOTSUPPORT;
		break;
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		struct np *np = NP_PRIV(q);

		mp->b_datap->db_type = M_PCPROTO;
		p = (N_error_ack_t *) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = ERROR_prim;
		p->NPI_error = (NPI_error < 0) ? NSYSERR : NPI_error;
		p->UNIX_error = (NPI_error < 0) ? -NPI_error : 0;
		mp->b_wptr += sizeof(*p);
		switch (npi_get_state(np)) {
		case NS_WACK_OPTREQ:
			npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_UREQ:
			npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WCON_CREQ:
			npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WCON_RREQ:
			npi_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		case NS_WACK_BREQ:
			npi_set_state(np, NS_UNBND);
			break;
		case NS_WACK_CRES:
			npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ6:
			npi_set_state(np, NS_WCON_CREQ);
			break;
		case NS_WACK_DREQ7:
			npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ9:
			npi_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		case NS_WACK_DREQ10:
			npi_set_state(np, NS_WCON_RREQ);
			break;
		case NS_WACK_DREQ11:
			npi_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
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
 * @CORRECT_prim: correct primitive
 * @ADDR_buffer: destination or responding address
 * @ADDR_length: length of destination or responding addresses
 * @QOS_buffer: quality of service parameters
 * @SEQ_number: sequence number (i.e. connection/reset indication sequence number)
 * @TOKEN_value: token (i.e. connection response token)
 * @flags: mangement flags, connection flags, disconnect reason, etc.
 */
STATIC int
ne_ok_ack(queue_t *q, np_ulong CORRECT_prim, struct sockaddr_in *ADDR_buffer, socklen_t ADDR_length,
	  void *QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value, np_ulong flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_ok_ack_t *p;
	const size_t size = sizeof(*p);
	np_long NPI_error;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	mp->b_wptr += sizeof(*p);
	switch (npi_get_state(np)) {
	case NS_WACK_OPTREQ:
		if ((NPI_error = npi_optmgmt(np, QOS_buffer, flags)) < 0)
			goto error;
		npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
		break;
	case NS_WACK_UREQ:
		if ((NPI_error = npi_unbind(np)) < 0)
			goto error;
		npi_set_state(np, NS_UNBND);
		break;
	case NS_WACK_CRES:
		if ((NPI_error = npi_passive(np, ADDR_buffer, ADDR_length, QOS_buffer, SEQ_number,
						TOKEN_value, flags)) < 0)
			goto error;
		if (np != TOKEN_value) {
			npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
			npi_set_state(TOKEN_value, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
		} else {
			npi_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
		}
		break;
	case NS_WACK_RRES:
		if ((NPI_error = npi_reset(np, ADDR_buffer, ADDR_length, N_USER, N_REASON_UNDEFINED)) < 0)
			goto error;
		npi_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		if ((NPI_error = npi_disconnect(np, ADDR_buffer, SEQ_number, flags)) < 0)
			goto error;
		npi_set_state(np, np->coninds > 0 ? NS_WRES_CIND : NS_IDLE);
		break;
	default:
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a N_OPTMGMT_REQ in other than the NS_IDLE
		   state. */
		break;
	}
	putnext(np->oq, mp);
	return (QR_DONE);
      error:
	freemsg(mp);
	return (NPI_error);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ne_conn_con: perform N_CONN_CON actions
 * @q: active queue in queue pair (write queue)
 * @RES_buffer: responding addresses
 * @RES_length: length of responding addresses
 * @QOS_buffer: connected quality of service
 * @CONN_flags: connected connection flags
 *
 * The NPI-IP driver only supports a pseudo-connection-oriented mode.  The destination address and
 * quality-of-service parameters returned in the N_CONN_CON do not represent a connection
 * establishment or negotiation with the remote NS provider, but are simply formed locally and
 * negotiated locally.  The N_CONN_CON is synthesized from the N_CONN_REQ and is returned
 * automatically and immediately in response to the N_CONN_REQ.
 *
 * Note that, unlike TPI, NPI does not return a N_OK_ACK in response to N_CONN_REQ and does not have
 * a NS_WACK_CREQ state, only a NS_WCON_CREQ state.  This makes NPI more ammenable to this kind of
 * pseudo-connection-oriented mode.
 *
 * Originally, this primitive returned one N_QOS_SEL_CONN_IP structure for each destination.  This
 * is no longer the case.  Only one N_QOS_SEL_CONN_IP structure is returned representing the values
 * for the entire association.  ttl is the maximum ttl of the destinations.  mtu is the smallest
 * value for the destinations.  These values are set in the private structure by the npi_connect()
 * function.
 *
 */
STATIC INLINE fastcall int
ne_conn_con(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	    struct N_qos_sel_conn_ip * QOS_buffer, np_ulong CONN_flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_conn_con_t *p;
	np_long NPI_error;
	size_t QOS_length = sizeof(*QOS_buffer);
	size_t size = sizeof(*p) + RES_length + QOS_length;

	npi_set_state(np, NS_WCON_CREQ);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	if (unlikely
	    ((NPI_error = npi_connect(np, RES_buffer, RES_length, QOS_buffer, CONN_flags)) != 0))
		goto free_error;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->CONN_flags = CONN_flags;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + RES_length : 0;
	if (RES_length) {
		bcopy(RES_buffer, mp->b_wptr, RES_length);
		mp->b_wptr += RES_length;
	}
	if (QOS_length) {
		bcopy(QOS_buffer, mp->b_wptr, QOS_length);
		mp->b_wptr += QOS_length;
	}

	qreply(q, mp);
	return (QR_DONE);

      free_error:
	freeb(mp);
	goto error;
      enobufs:
	NPI_error = -ENOBUFS;
	goto error;
      error:
	return (NPI_error);

}

/**
 * ne_reset_con: - generate a N_RESET_CON message
 * @q: active queue in queue pair (write queue)
 * @dp: message containing IP packet
 */
STATIC INLINE fastcall int
ne_reset_con(queue_t *q, struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	     np_ulong RESET_orig, np_ulong RESET_reason)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_reset_con_t *p;
	struct N_qos_sel_reset_ip *QOS_buffer;
	const size_t QOS_length = sizeof(*QOS_buffer);
	size_t size = sizeof(*p) + DEST_length + QOS_length;
	np_long NPI_error;

	npi_set_state(np, NS_WCON_RREQ);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto nobufs;
	if (unlikely((NPI_error = npi_reset(np, DEST_buffer, DEST_length, RESET_orig,
					    RESET_reason)) != 0))
		goto free_error;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_RESET_CON;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + DEST_length : 0;
	if (DEST_length) {
		bcopy(DEST_buffer, mp->b_wptr, DEST_length);
		mp->b_wptr += DEST_length;
	}
	if (QOS_length) {
		QOS_buffer = (typeof(QOS_buffer)) mp->b_wptr++;
		QOS_buffer->n_qos_type = N_QOS_SEL_RESET_IP;
		QOS_buffer->ttl = np->qos.ttl;
		QOS_buffer->tos = np->qos.tos;
		QOS_buffer->mtu = np->qos.mtu;
	}
	npi_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
	qreply(q, mp);
	return (QR_DONE);

      free_error:
	freeb(mp);
	goto error;
      nobufs:
	NPI_error = -ENOBUFS;
	goto error;
      error:
	return (NPI_error);
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
ne_conn_ind(queue_t *q, mblk_t *SEQ_number)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp, *bp;
	N_conn_ind_t *p;
	struct sockaddr_in *DEST_buffer, *SRC_buffer;
	struct N_qos_sel_conn_ip *QOS_buffer;
	const np_ulong DEST_length = sizeof(*DEST_buffer);
	const np_ulong SRC_length = sizeof(*SRC_buffer);
	const np_ulong QOS_length = sizeof(*QOS_buffer);
	const size_t size = sizeof(*p) + DEST_length + SRC_length + QOS_length;
	struct iphdr *iph = (struct iphdr *) SEQ_number->b_rptr;

	assure(SEQ_number->b_wptr >= SEQ_number->b_rptr + sizeof(*iph));
	assure(SEQ_number->b_wptr >= SEQ_number->b_rptr + (iph->ihl << 2));

	/* Make sure we don't already have a connection indication */
	for (bp = np->conq; bp; bp = bp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) bp->b_rptr;

		if (iph->protocol == iph2->protocol
		    && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr) {
			/* duplicate, just discard it */
			return (QR_DONE);
		}
	}

	npi_set_state(np, NS_WRES_CIND);

	if (unlikely((bp = ss7_dupmsg(q, SEQ_number)) == NULL))
		goto nobufs;
	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto free_nobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 0;
	p = (N_conn_ind_t *) mp->b_wptr++;
	p->PRIM_type = N_CONN_IND;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) + DEST_length : 0;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	p->CONN_flags = 0;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + DEST_length + SRC_length : 0;
	if (DEST_length) {
		DEST_buffer = (struct sockaddr_in *) mp->b_wptr++;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = iph->protocol;
		DEST_buffer->sin_addr.s_addr = iph->daddr;
	}
	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr++;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = iph->protocol;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
	}
	if (QOS_length) {
		QOS_buffer = (struct N_qos_sel_conn_ip *) mp->b_wptr++;
		QOS_buffer->n_qos_type = N_QOS_SEL_CONN_IP;
		QOS_buffer->protocol = iph->protocol;
		QOS_buffer->priority = bp->b_band;
		QOS_buffer->ttl = iph->ttl;
		QOS_buffer->tos = iph->tos;
		QOS_buffer->mtu = QOS_UNKNOWN;	/* FIXME: determine route and get mtu from it */
		QOS_buffer->daddr = iph->daddr;
		QOS_buffer->saddr = iph->saddr;
	}
	/* should we pull the IP header? */
	mp->b_cont = bp;
	/* save original in connection indication list */
	SEQ_number->b_next = np->conq;
	np->conq = SEQ_number;
	putnext(q, mp);
	return (QR_ABSORBED);

      free_nobufs:
	freemsg(bp);
	goto nobufs;
      nobufs:
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
ne_discon_ind(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	      np_ulong RESERVED_field, np_ulong DISCON_orig, np_ulong DISCON_reason,
	      mblk_t *SEQ_number, mblk_t *dp)
{
	mblk_t *mp;
	N_discon_ind_t *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + RES_length, BPRI_MED)) == NULL))
		goto nobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* expedite */
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = DISCON_orig;
	p->DISCON_reason = DISCON_reason;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	if (RES_length) {
		bcopy(RES_buffer, mp->b_wptr, RES_length);
		mp->b_wptr += RES_length;
	}
	mp->b_cont = dp;
	putnext(q, mp);
	return (QR_ABSORBED);

      nobufs:
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
	struct np *np = NP_PRIV(q);
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in res_buf, *RES_buffer = &res_buf;
	np_ulong RESERVED_field, RESET_orig, RESET_reason;
	mblk_t **respp, **conpp, *SEQ_number;
	ptrdiff_t hidden;
	int err;

	iph = (struct iphdr *) mp->b_rptr;	/* this is the ICMP message IP header */
	icmp = (struct icmphdr *) (mp->b_rptr + (iph->ihl << 2));
	iph = (struct iphdr *) (icmp + 1);	/* this is the encapsulated IP header */
	uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));
	if (mp->b_wptr < (unsigned char *) (uh + 1))
		uh = NULL;	/* don't have a full transport header */

	RES_buffer->sin_family = AF_INET;
	RES_buffer->sin_port = uh ? uh->source : 0;
	RES_buffer->sin_addr.s_addr = iph->saddr;

	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			RESET_orig = N_PROVIDER;
			RESET_reason = N_REJ_NSAP_UNREACH_P;	// N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_FRAG_NEEDED:
			RESET_orig = N_PROVIDER;
			RESET_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_SEG_REQUIRED;
			RESERVED_field = icmp->un.frag.mtu;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			RESET_orig = N_PROVIDER;
			RESET_reason = N_REJ_NSAP_UNKNOWN;	// N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			RESET_orig = N_PROVIDER;
			RESET_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_QOS_UNAVAIL;
			RESERVED_field = 0;
			break;
		default:
			RESET_orig = N_UNDEFINED;
			RESET_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		/* Should not cause disconnect. */
		RESET_orig = N_PROVIDER;
		RESET_reason = N_CONGESTION;	// N_UD_CONGESTION;
		RESERVED_field = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			RESET_orig = N_PROVIDER;
			RESET_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_LIFE_EXCEEDED;
			RESERVED_field = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			RESET_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_TD_EXCEEDED;
			RESERVED_field = 0;
			break;
		default:
			RESET_orig = N_UNDEFINED;
			RESET_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		RESET_orig = N_UNDEFINED;
		RESET_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	default:
		RESET_orig = N_UNDEFINED;
		RESET_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	}

	/* check for outstanding reset indications for responding address */
	for (respp = &np->resq; (*respp); respp = &(*respp)->b_next) {
		struct iphdr *iph2 = (struct iphdr *) (*respp)->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}

	/* check for outstanding connection indications for responding address */
	for (conpp = &np->conq; (*conpp); conpp = &(*conpp)->b_next) {
		struct iphdr *iph2 = (struct iphdr *) (*conpp)->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}
	SEQ_number = (*conpp);

	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = ne_discon_ind(q, RES_buffer, sizeof(*RES_buffer), RESERVED_field, RESET_orig,
				 RESET_reason, SEQ_number, mp)) < 0)
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
 * in the block.
 */
STATIC INLINE fastcall __hot_in int
ne_data_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	const size_t size = sizeof(*p);

	if (unlikely(!canputnext(q)))
		goto ebusy;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DATA_IND;
	/* TODO: here we can set some info like ECN... */
	p->DATA_xfer_flags = 0;
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	putnext(q, mp);
	return (QR_ABSORBED);

      enobufs:
	return (-ENOBUFS);

      ebusy:
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

	if (unlikely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto nobufs;
	if (unlikely(!bcanputnext(q, 1)))
		goto busy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	putnext(q, mp);
	return (QR_ABSORBED);

      busy:
	freeb(mp);
	return (-EBUSY);
      nobufs:
	return (-ENOBUFS);
}

/**
 * ne_unitdata_ind: - generate a N_UNITDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Note that on unit data indications the entire IP packet is delivered in the data part of
 * the message.  If the NS user is willing to extract information from the IP header, it is
 * welcome to discard the control part.
 */
STATIC INLINE fastcall __hot_in int
ne_unitdata_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;
	struct sockaddr_in *SRC_buffer, *DEST_buffer;
	const np_ulong SRC_length = sizeof(*SRC_buffer);
	const np_ulong DEST_length = sizeof(*DEST_buffer);
	const size_t size = sizeof(*p) + SRC_length + DEST_length;
	struct iphdr *iph;
	struct udphdr *uh;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) + SRC_length : 0;
	p->ERROR_type = 0;

	iph = (struct iphdr *) dp->b_rptr;
	uh = (struct udphdr *) (dp->b_rptr + (iph->ihl << 2));

	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr++;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = uh->source;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
	}
	if (DEST_length) {
		DEST_buffer = (struct sockaddr_in *) mp->b_wptr++;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = uh->dest;
		DEST_buffer->sin_addr.s_addr = iph->daddr;
	}
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ne_uderror_ind: - generate a N_UDERROR_IND message
 * @q: active queue in queue pair (read queue)
 * @DEST_buffer: pointer to destination address
 * @RESERVED_field: MTU if error is N_UD_SEG_REQUIRED
 * @ERROR_type: error number
 * @dp: message containing (part of) errored packet
 *
 * Note that in the special case of N_UD_SEG_REQUIRED, we use the RESERVED_field to indicate that
 * the value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky
 * trick, because the field must be coded zero according to spec, so the presence of a non-zero
 * value indicates that the MTU value is present for supporting providers.
 */
STATIC INLINE fastcall int
ne_uderror_ind(queue_t *q, struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	       np_ulong RESERVED_field, np_ulong ERROR_type, mblk_t *dp)
{
	mblk_t *mp;
	N_uderror_ind_t *p;
	const size_t size = sizeof(*p) + DEST_length;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* expedite */

	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->RESERVED_field = RESERVED_field;
	p->ERROR_type = ERROR_type;
	if (DEST_length) {
		bcopy(DEST_buffer, mp->b_wptr, DEST_length);
		mp->b_wptr += DEST_length;
	}
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* was M_ERROR in some cases */
	putnext(q, mp);
	return (QR_ABSORBED);

      enobufs:
	return (-ENOBUFS);

      ebusy:
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
	struct sockaddr_in dst_buf, *DEST_buffer = &dst_buf;
	np_ulong RESERVED_field, ERROR_type;
	ptrdiff_t hidden;
	int err;

	iph = (struct iphdr *) mp->b_rptr;	/* this is the ICMP message IP header */
	icmp = (struct icmphdr *) (mp->b_rptr + (iph->ihl << 2));
	iph = (struct iphdr *) (icmp + 1);	/* this is the encapsulated IP header */
	uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));
	if (mp->b_wptr < (unsigned char *) (uh + 1))
		uh = NULL;	/* don't have a full transport header */
	DEST_buffer->sin_family = AF_INET;
	DEST_buffer->sin_port = uh ? uh->source : 0;
	DEST_buffer->sin_addr.s_addr = iph->saddr;
	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			ERROR_type = N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_FRAG_NEEDED:
			ERROR_type = N_UD_SEG_REQUIRED;
			RESERVED_field = icmp->un.frag.mtu;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			ERROR_type = N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			ERROR_type = N_UD_QOS_UNAVAIL;
			RESERVED_field = 0;
			break;
		default:
			ERROR_type = N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		ERROR_type = N_UD_CONGESTION;
		RESERVED_field = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			ERROR_type = N_UD_LIFE_EXCEEDED;
			RESERVED_field = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			ERROR_type = N_UD_TD_EXCEEDED;
			RESERVED_field = 0;
			break;
		default:
			ERROR_type = N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		ERROR_type = N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	default:
		ERROR_type = N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	}
	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err =
	     ne_uderror_ind(q, DEST_buffer, sizeof(*DEST_buffer), RESERVED_field, ERROR_type,
			    mp)) < 0)
		mp->b_rptr -= hidden;
	return (err);
}

STATIC INLINE fastcall int
ne_uderror_reply(queue_t *q, struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
		 np_ulong RESERVED_field, np_long ERROR_type, mblk_t *db)
{
	int err;

	switch (ERROR_type) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (ERROR_type);
	case 0:
		return (ERROR_type);
	case -EADDRINUSE:
		ERROR_type = NBOUND;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		ERROR_type = NNOADDR;
		break;
	case -EMSGSIZE:
		ERROR_type = NBADDATA;
		break;
	case -EACCES:
		ERROR_type = NACCESS;
		break;
	case -EOPNOTSUPP:
		ERROR_type = NNOTSUPPORT;
		break;
	case NBADADDR:
	case NBADDATA:
		break;
	case NOUTSTATE:
	case -EINVAL:
	case -EFAULT:
		return ne_error_reply(q, -EPROTO);
	}
	if ((err =
	     ne_uderror_ind(q, DEST_buffer, DEST_length, RESERVED_field, ERROR_type,
			    db)) == QR_DONE)
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
	struct np *np = NP_PRIV(q);
	mblk_t *mp, *bp;
	N_reset_ind_t *p;
	struct sockaddr_in *DEST_buffer;
	struct N_qos_sel_reset_ip *QOS_buffer;
	const size_t DEST_length = sizeof(*DEST_buffer);
	const size_t QOS_length = sizeof(*QOS_buffer);
	const size_t size = sizeof(*p) + DEST_length + QOS_length;
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct icmphdr *icmp = (struct icmphdr *) (dp->b_rptr + (iph->ihl << 2));
	ulong mtu = 0;

	assure(dp->b_wptr >= dp->b_rptr + sizeof(*iph));
	assure(dp->b_wptr >= dp->b_rptr + (iph->ihl << 2));

	/* Make sure we don't already have a reset indication */
	for (bp = np->resq; bp; bp = bp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) bp->b_rptr;
		struct icmphdr *icmp2 = (struct icmphdr *) (bp->b_rptr + (iph2->ihl << 2));

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && icmp->type == icmp2->type && icmp->code == icmp2->code)
			/* duplicate, just discard it */
			goto discard;
	}

	if (unlikely((bp = ss7_dupmsg(q, dp)) == NULL))
		goto enobufs;
	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;
	p = (typeof(p)) mp->b_wptr++;
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
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + DEST_length : 0;
	if (DEST_length) {
		DEST_buffer = (typeof(DEST_buffer)) mp->b_wptr++;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = iph->protocol;
		DEST_buffer->sin_addr.s_addr = iph->saddr;
	}
	if (QOS_length) {
		QOS_buffer = (typeof(QOS_buffer)) mp->b_wptr++;
		QOS_buffer->n_qos_type = N_QOS_SEL_RESET_IP;
		QOS_buffer->ttl = iph->ttl;
		QOS_buffer->tos = iph->tos;
		if (icmp->type == ICMP_DEST_UNREACH && icmp->code == ICMP_FRAG_NEEDED)
			QOS_buffer->mtu = icmp->un.frag.mtu;
		else
			QOS_buffer->mtu = 0;
	}
	/* save original in reset indication list */
	dp->b_next = np->resq;
	np->resq = dp;
	np->resinds++;
	putnext(q, mp);
      discard:
	return (QR_DONE);
      enobufs:
	if (bp)
		freemsg(bp);
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

	if (unlikely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DATACK_IND;

	putnext(q, mp);
	return (QR_DONE);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
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
 * number combination.
 *
 * Protocol ids must always be specified.  Currently we don't allow binding to more than one
 * protocol id, but, instead of generating an error, we simply bind to the first protocol id
 * specified and ignore the reset.  We will only return the first protocol id in the bind ack.
 */
STATIC INLINE fastcall int
ne_bind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_bind_req_t *p;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_wptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_BIND_REQ))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_not_state(np, NSF_UNBND)))
		goto error;
	npi_set_state(np, NS_WACK_BREQ);
	NPI_error = NBADFLAG;
	if (unlikely((p->BIND_flags & ~(TOKEN_REQUEST | DEFAULT_DEST | DEFAULT_LISTENER)) != 0))
		goto error;
	switch (p->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)) {
	case DEFAULT_LISTENER:
		/* If DEFAULT_LISTENER is specified, the Stream is always considered
		   connection-oriented. Each protocol id and port number combination (whether the
		   port number is zero or not) is permitted multiple DEFAULT_LISTENER Streams (one
		   for each IP address, and one for the wildcard IP address).  DEFAULT_LISTENER
		   Streams are only considered when there is no match for any other listening
		   Stream.  It is ok under NPI-IP to set the DEFAULT_LISTENER flag, even though
		   CONIND_number is set to zero, in which case, it merely specifies that the Stream 
		   is to be a pseudo-connection-oriented stream. */
		np->info.SERV_type = N_CONS;
		np->CONIND_number = p->CONIND_number;
		break;
	case DEFAULT_DEST:
		/* If DEFAULT_DEST is specified, the Stream is always considered connectionless.
		   Each protocol id is permitted multiple DEFAULT_DEST Streams (one for each IP
		   address, and one for the wildcard IP address).  DEFAULT_DEST Streams are only
		   considered when there is no match for any other connectionless bound Stream that 
		   matches the destination address. */
		np->info.SERV_type = N_CLNS;
		np->CONIND_number = 0;
		break;
	case 0:
		/* If the number of connection indications is zero, the bind is performed and the
		   service type set to connectionless.  If the number of connection indications is
		   non-zero, the bind is performed and the service type set to connection-oriented.
		   If, however, the TOKEN_REQUEST flag is set, then the bind is always considered
		   connection-oriented, even if the number of connection indications is set to zero,
		   as this is the typical case for an accepting Stream. */
		if (p->CONIND_number > 0 || (p->BIND_flags & TOKEN_REQUEST)) {
			np->info.SERV_type = N_CONS;
			np->CONIND_number = p->CONIND_number;
		} else {
			np->info.SERV_type = N_CLNS;
			np->CONIND_number = 0;
		}
		break;
	case (DEFAULT_LISTENER | DEFAULT_DEST):
		/* can't specify both */
		goto error;
	}
	NPI_error = NBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto error;
	NPI_error = NNOPROTOID;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length))
		goto error;
	if (unlikely(p->PROTOID_length == 0))
		goto error;
	{
		struct sockaddr_storage sa = { 0, };
		struct sockaddr_in *ADDR_buffer = (typeof(ADDR_buffer)) & sa;
		size_t ADDR_length = p->ADDR_length;
		unsigned char *PROTOID_buffer =
		    (typeof(PROTOID_buffer)) (mp->b_rptr + p->PROTOID_offset);

		if (ADDR_length) {
			NPI_error = NBADADDR;
			if (unlikely(ADDR_length < sizeof(*ADDR_buffer)))
				goto error;
			if (unlikely(ADDR_length > (sizeof(*ADDR_buffer) << 3)))
				goto error;
			if (unlikely(ADDR_length % sizeof(*ADDR_buffer) != 0))
				goto error;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, ADDR_buffer, ADDR_length);
			if (unlikely(ADDR_buffer->sin_family != AF_INET))
				goto error;
			/* cannot specify address if default listener */
			if (unlikely((p->BIND_flags & DEFAULT_LISTENER) &&
				     (ADDR_buffer->sin_port != 0 ||
				      ADDR_buffer->sin_addr.s_addr != INADDR_ANY)))
				goto error;
			/* cannot listen on wildcard port number */
			if (unlikely(p->CONIND_number > 0 && ADDR_buffer->sin_port == 0))
				goto error;
			/* cannot specify port if connectionless */
			if (unlikely(np->info.SERV_type == N_CLNS && ADDR_buffer->sin_port != 0))
				goto error;
		} else {
			ADDR_length = sizeof(*ADDR_buffer);
			ADDR_buffer->sin_family = AF_INET;
			ADDR_buffer->sin_port = 0;
			ADDR_buffer->sin_addr.s_addr = INADDR_ANY;
		}
		NPI_error = ne_bind_ack(q, PROTOID_buffer, p->PROTOID_length,
					ADDR_buffer, ADDR_length, p->CONIND_number, p->BIND_flags);
		if (unlikely(NPI_error != 0))
			goto error;
		return (QR_DONE);
	}
      error:
	return ne_error_ack(q, N_BIND_REQ, NPI_error);
}

/**
 * ne_unbind_req:- NE_UNBIND_REQ unbind NS user from network address event
 * @q: active queue in pair (write queue)
 * @mp: N_UNBIND_REQ message
 */
STATIC INLINE fastcall int
ne_unbind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_unbind_req_t *p;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_UNBIND_REQ))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_not_state(np, NSF_IDLE)))
		goto error;
	npi_set_state(np, NS_WACK_UREQ);
	return ne_ok_ack(q, N_UNBIND_REQ, NULL, 0, NULL, NULL, NULL, 0);
      error:
	return ne_error_ack(q, N_UNBIND_REQ, NPI_error);
}

/**
 * ne_optmgmt_req: - NE_OPTMGMT_REQ options management request
 * @q: active queue in pair (write queue)
 * @mp: N_OPTMGMT_REQ message
 */
STATIC int
ne_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_optmgmt_req_t *p;
	struct N_qos_sel_info_ip *QOS_buffer = NULL;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_OPTMGMT_REQ))
		goto error;
	if (unlikely(npi_chk_state(np, NSF_IDLE)))
		npi_set_state(np, NS_WACK_OPTREQ);
	NPI_error = NBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
		goto error;
	if (p->QOS_length) {
		QOS_buffer = (typeof(QOS_buffer)) (mp->b_rptr + p->QOS_offset);
		NPI_error = NBADOPT;
		if (p->QOS_length < sizeof(QOS_buffer->n_qos_type))
			goto error;
		NPI_error = NBADQOSTYPE;
		/* FIXME: we can handle more option structures than this */
		if (QOS_buffer->n_qos_type != N_QOS_SEL_INFO_IP)
			goto error;
		NPI_error = NBADOPT;
		if (p->QOS_length != sizeof(*QOS_buffer))
			goto error;
		if (QOS_buffer->tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->tos;
		if (QOS_buffer->ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->ttl;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		np->i_flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		np->i_flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	return ne_ok_ack(q, N_OPTMGMT_REQ, NULL, 0, QOS_buffer, NULL, NULL, p->OPTMGMT_flags);
      error:
	return ne_error_ack(q, N_OPTMGMT_REQ, NPI_error);
}

/**
 * ne_unitdata_req: - NE_UNITDATA_REQ unit data request event
 * @q: active queue in pair (write queue)
 * @mp: N_UNITDATA_REQ message
 *
 * N_UNITDATA_REQ does not have a qos parameter, nor source address field, only a destination
 * address field.  We use three different approaches to specifying pertinent information for the
 * packet:
 *
 * 1. If DEST_length is non-zero, it specifies a destination address.  If RESERVED_field[0] is zero,
 *    no QOS parameters are specified.  In this case, the tos, ttl, saddr, etc., come from the NPI
 *    IP stream.  The transport header starts with the attached M_DATA blocks.
 *
 * 2. If DEST_length is non-zero, it specifies a destination address.  If RESERVED_field[0] is also
 *    non-zero, QOS parameters are specified, in which case tos, ttl, saddr, etc., come from the QOS
 *    structure.  The transport header starts with the attached M_DATA_blocks.
 *
 * 3. If DEST_length is zero, it indicates that the IP header has been included in the data payload.
 *    Destination address, source address and QOS parameters come from the included IP header.  If
 *    the included IP header is only partially complete, the NPI-IP provider will complete the
 *    uncompleted fields.  The network header starts with the attached M_DATA blocks.
 *
 * The NPI-IP provider will request that the Stream head provide an additional write offset of 20
 * bytes to accomodate the IP header.
 *
 * Approach number (1) is recommended because it follows the NPI interface strictly.
 */
STATIC INLINE fastcall __hot_put int
ne_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	size_t mlen, hlen = sizeof(struct iphdr);
	N_unitdata_req_t *p;
	struct sockaddr_in dst_buf, *DEST_buffer = NULL;
	struct N_qos_sel_ud_ip qos_buf, *QOS_buffer = NULL;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_UNITDATA_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CONS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CLNS))
		goto error;
	if (unlikely(npi_not_state(np, NSF_IDLE)))
		goto error;
	NPI_error = NBADDATA;
	if (unlikely(mp->b_cont == NULL))
		goto error;
	/* Note: no destination address is our clue that the upper layer had already included the
	   IP header in the IP packet. */
	if (p->DEST_length) {
		NPI_error = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
			goto error;
		if (unlikely(p->DEST_length < sizeof(struct sockaddr_in)))
			goto error;
		/* avoid alignemnt problems */
		bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, sizeof(dst_buf));
		if (unlikely(dst_buf.sin_family != AF_INET))
			goto error;
		if (unlikely(dst_buf.sin_port == 0))	/* this is really the IP protocol */
			goto error;
		if (unlikely(dst_buf.sin_addr.s_addr == INADDR_ANY))
			goto error;
		DEST_buffer = &dst_buf;
		hlen = 0;
	}
#define QOS_length RESERVED_field[0]
#define QOS_offset RESERVED_field[1]
	/* Note: the reserved field can be used to send QOS data for the datagram. */
	/* Actually, the proper way to do this is to set the appropriate values using
	   N_QOS_SEL_UD_IP to N_OPTMGMT_REQ(7) and then sending the data. */
	if (p->QOS_length) {
		int i;

		NPI_error = NBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
			goto error;
		if (unlikely(p->QOS_length < sizeof(qos_buf.n_qos_type)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf.n_qos_type));
		NPI_error = NBADQOSTYPE;
		if (unlikely(qos_buf.n_qos_type != N_QOS_SEL_UD_IP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(qos_buf)))
			goto error;
		/* avoid alignment problems */
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf));
		QOS_buffer = &qos_buf;
		NPI_error = NBADQOSPARAM;
		/* check qos parameter values */
		/* protocol must be one of the bound protocol ids */
		if (QOS_buffer->protocol != QOS_UNKNOWN) {
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->protocol)
					break;
			if (i >= np->pnum)
				goto error;
		}
		if (QOS_buffer->priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->priority < np->qor.priority.priority_min_value)
				goto error;
			if ((np_long) QOS_buffer->priority > np->qor.priority.priority_max_value)
				goto error;
		}
		if (QOS_buffer->ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->ttl < np->qor.ttl.ttl_min_value)
				goto error;
			if ((np_long) QOS_buffer->ttl > np->qor.ttl.ttl_max_value)
				goto error;
		}
		if (QOS_buffer->tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->tos < np->qor.tos.tos_min_value)
				goto error;
			if ((np_long) QOS_buffer->tos > np->qor.tos.tos_max_value)
				goto error;
		}
		/* source address should be one of the specified source addresses */
		if (QOS_buffer->saddr != QOS_UNKNOWN) {
			if (QOS_buffer->saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->saddr)
						break;
				}
				if (i >= np->snum)
					goto error;
			}
		}
	}
#undef QOS_length
#undef QOS_offset
	NPI_error = NBADDATA;
	if (unlikely((mlen = msgdsize(mp->b_cont)) + hlen <= sizeof(struct iphdr)
		     || mlen + hlen > 65535))
		goto error;
	if (unlikely((NPI_error = npi_unitdata_req(q, DEST_buffer, QOS_buffer, mp->b_cont)) < 0))
		goto error;
	return (QR_DONE);
      error:
	return ne_uderror_reply(q, DEST_buffer, sizeof(*DEST_buffer), 0, NPI_error, mp->b_cont);
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
	struct np *np = NP_PRIV(q);
	N_conn_req_t *p;
	struct N_qos_sel_conn_ip qos_buf = { N_QOS_SEL_CONN_IP, }, *QOS_buffer = NULL;
	struct sockaddr_in dst_buf[8] = { {AF_INET,}, }, *DEST_buffer = NULL;
	np_long NPI_error;
	int i;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_conn_req_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_CONN_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Connection requests are not allowed on a listening Stream.  Note that there is a
	   conflict in the NPI specifications here: under the description for N_BIND_REQ, NPI 2.0.0 
	   says: "If a Stream is bound as a listener Stream, it will not be able to initiate
	   connect requests. If the NS user attempts to send an N_CONN_REQ primitive down this
	   Stream, an N_ERROR_ACK message will be sent to the NS user by the NS provider with an
	   error value of NACCESS." Then, under N_BIND_ACK, NPI 2.0.0 says: "A Stream with a
	   negotiated CONIND_number greater than zero may generate connect requests or accept
	   connect indications." */
	NPI_error = NACCESS;
	if (unlikely(np->CONIND_number > 0))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_get_state(np) != NS_IDLE))
		goto error;
	NPI_error = NBADFLAG;
	if (unlikely(p->CONN_flags != 0))
		goto error;
	if (p->DEST_length != 0) {
		int i;

		NPI_error = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
			goto error;
		if (unlikely(p->DEST_length < sizeof(*DEST_buffer)))
			goto error;
		if (unlikely(p->DEST_length > (sizeof(*DEST_buffer) << 3)))
			goto error;
		if (unlikely(p->DEST_length % sizeof(*DEST_buffer) != 0))
			goto error;
		bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, p->DEST_length);
#if 0
		/* well, no... the destination can be a wildcard port as long as the stream is
		   bound to a wildcard port, then port numbers are effectively ignored */
		if (unlikely(dst_buf[0].sin_port == 0))
			goto error;
#endif
		if (unlikely(dst_buf[0].sin_family != AF_INET && dst_buf[0].sin_family != 0))
			goto error;
		for (i = 0; i < p->DEST_length / sizeof(*DEST_buffer); i++)
			if (unlikely(dst_buf[i].sin_addr.s_addr == INADDR_ANY))
				goto error;
		DEST_buffer = dst_buf;
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
		QOS_buffer = &qos_buf;
		NPI_error = NBADQOSPARAM;
		/* validate parameters */
		if (QOS_buffer->protocol != QOS_UNKNOWN) {
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->protocol)
					break;
			if (i >= np->pnum)
				goto error;
		}
		if (QOS_buffer->priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->priority < np->qor.priority.priority_min_value)
				goto error;
			if ((np_long) QOS_buffer->priority > np->qor.priority.priority_max_value)
				goto error;
		}
		if (QOS_buffer->ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->ttl < np->qor.ttl.ttl_min_value)
				goto error;
			if ((np_long) QOS_buffer->ttl > np->qor.ttl.ttl_max_value)
				goto error;
		}
		if (QOS_buffer->tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->tos < np->qor.tos.tos_min_value)
				goto error;
			if ((np_long) QOS_buffer->tos > np->qor.tos.tos_max_value)
				goto error;
		}
		if (QOS_buffer->mtu != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->mtu < np->qor.mtu.mtu_min_value)
				goto error;
			if ((np_long) QOS_buffer->mtu > np->qor.mtu.mtu_max_value)
				goto error;
		}
		if (QOS_buffer->saddr != QOS_UNKNOWN) {
			if (QOS_buffer->saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->saddr)
						break;
				}
				if (i >= np->snum)
					goto error;
			}
		}
		if (QOS_buffer->daddr != QOS_UNKNOWN) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == QOS_buffer->daddr)
					break;
			if (i >= np->dnum)
				goto error;
		}
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	if (unlikely((NPI_error = ne_conn_con(q, DEST_buffer, p->DEST_length,
					      QOS_buffer, p->CONN_flags)) < 0))
		goto error;
	return (NPI_error);
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
	struct np *np = NP_PRIV(q), *TOKEN_value = np;
	N_conn_res_t *p;
	struct N_qos_sel_conn_ip qos_buf = { N_QOS_SEL_CONN_IP, }, *QOS_buffer = NULL;
	struct sockaddr_in res_buf[8] = { {AF_INET,}, }, *RES_buffer = NULL;
	mblk_t *dp, *SEQ_number;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_conn_res_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_CONN_RES))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	if (unlikely(npi_not_state(np, NSM_LISTEN)))
		goto error;
	if (p->RES_length != 0) {
		NPI_error = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length))
			goto error;
		if (unlikely(p->RES_length < sizeof(*RES_buffer)))
			goto error;
		if (unlikely(p->RES_length > (sizeof(*RES_buffer) << 3)))
			goto error;
		if (unlikely(p->RES_length % sizeof(*RES_buffer) != 0))
			goto error;
		/* Cannot be sure that the address is properly aligned, and to keep unscrupulous
		   users from DoS attacks, copy the informatoin into an aligned buffer. */
		bcopy(mp->b_rptr + p->RES_offset, &res_buf, p->RES_length);
		if (unlikely(res_buf[0].sin_family != AF_INET && res_buf[0].sin_family != 0))
			goto error;
		if (unlikely(res_buf[0].sin_port == 0))
			goto error;
		RES_buffer = res_buf;
	}
	if (p->QOS_length != 0) {
		int i;

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
		QOS_buffer = &qos_buf;
		NPI_error = NBADQOSPARAM;
		/* validate parameters */
		if (QOS_buffer->protocol != QOS_UNKNOWN) {
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->protocol)
					break;
			if (i >= np->pnum)
				goto error;
		}
		if (QOS_buffer->priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->priority < np->qor.priority.priority_min_value)
				goto error;
			if ((np_long) QOS_buffer->priority > np->qor.priority.priority_max_value)
				goto error;
		}
		if (QOS_buffer->ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->ttl < np->qor.ttl.ttl_min_value)
				goto error;
			if ((np_long) QOS_buffer->ttl > np->qor.ttl.ttl_max_value)
				goto error;
		}
		if (QOS_buffer->tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->tos < np->qor.tos.tos_min_value)
				goto error;
			if ((np_long) QOS_buffer->tos > np->qor.tos.tos_max_value)
				goto error;
		}
		if (QOS_buffer->mtu != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->mtu < np->qor.mtu.mtu_min_value)
				goto error;
			if ((np_long) QOS_buffer->mtu > np->qor.mtu.mtu_max_value)
				goto error;
		}
		if (QOS_buffer->saddr != QOS_UNKNOWN) {
			if (QOS_buffer->saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->saddr)
						break;
				}
				if (i >= np->snum)
					goto error;
			}
		}
		if (QOS_buffer->daddr != QOS_UNKNOWN) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == QOS_buffer->daddr)
					break;
			if (i >= np->dnum)
				goto error;
		}
	}
	NPI_error = NBADDATA;
	if (unlikely((dp = mp->b_cont) != NULL && dp->b_wptr - dp->b_rptr <= 0))
		goto error;
	NPI_error = NBADSEQ;
	if (unlikely(p->SEQ_number == 0))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_not_state(np, NSF_WRES_CIND) || np->coninds < 1))
		goto error;
	for (SEQ_number = np->conq; SEQ_number && (np_ulong) (long) SEQ_number != p->SEQ_number;
	     SEQ_number = SEQ_number->b_next) ;
	NPI_error = NBADSEQ;
	if (unlikely(SEQ_number == NULL))
		goto error;
	if (p->TOKEN_value != 0) {
		for (TOKEN_value = master.np.list;
		     TOKEN_value && (np_ulong) (long) TOKEN_value != p->TOKEN_value;
		     TOKEN_value = TOKEN_value->next) ;
		NPI_error = NBADTOKEN;
		if (unlikely(TOKEN_value == NULL))
			goto error;
		if (unlikely(npi_get_state(TOKEN_value) == NS_UNBND))
			goto error;	/* we could bind it */
		if (unlikely(TOKEN_value->CONIND_number > 0))
			goto error;
		/* Note that the Stream to which we do a passive connection could be already
		   connected: we just are just adding another address to a multihomed connection.
		   But this is not as useful as adding or removing an address with N_OPTMGMT_REQ. */
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	npi_set_state(np, NS_WACK_CRES);
	return ne_ok_ack(q, N_CONN_RES, RES_buffer, p->RES_length, QOS_buffer, SEQ_number,
			 TOKEN_value, p->CONN_flags);
      error:
	return ne_error_ack(q, N_CONN_RES, NPI_error);
}

/**
 * ne_discon_req:- process N_DISCON_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DISCON_REQ message
 */
STATIC INLINE fastcall int
ne_discon_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_discon_req_t *p;
	struct sockaddr_in *RES_buffer = NULL;
	mblk_t *SEQ_number = NULL;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto error;
	p = (N_discon_req_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (p->PRIM_type != N_DISCON_REQ)
		goto error;
	NPI_error = NNOTSUPPORT;
	if (np->info.SERV_type == N_CLNS)
		goto error;
	NPI_error = NOUTSTATE;
	if (np->info.SERV_type != N_CONS)
		goto error;
	NPI_error = NOUTSTATE;
	if (npi_not_state(np, NSM_CONNECTED))
		goto error;
	if (p->RES_length) {
		NPI_error = -EINVAL;
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto error;
		NPI_error = NBADADDR;
		if (p->RES_length % sizeof(*RES_buffer) != 0)
			goto error;
		RES_buffer = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
	}
	if (p->SEQ_number) {
		NPI_error = NBADSEQ;
		if (npi_get_state(np) != NS_WRES_CIND)
			goto error;
		for (SEQ_number = np->conq;
		     SEQ_number && (np_ulong) (long) SEQ_number != p->SEQ_number;
		     SEQ_number = SEQ_number->b_next) ;
		NPI_error = NBADSEQ;
		if (SEQ_number == NULL)
			goto error;

	} else {
		NPI_error = NBADSEQ;
		if (npi_get_state(np) == NS_WRES_CIND)
			goto error;
	}
	/* Ok, all checking done.  Now we need to disconnect the address. */
	return ne_ok_ack(q, N_DISCON_REQ, RES_buffer, p->RES_length, NULL, SEQ_number, NULL,
			 p->DISCON_reason);
      error:
	return ne_error_ack(q, N_DISCON_REQ, NPI_error);
}

/**
 * ne_write_req - process M_DATA message
 * @q: write queue
 * @mp: the M_DATA message
 *
 * TODO: We should check the MSGDELIM flag and see whether this was a complete write or not.  If
 * not, we should accumulate the M_DATA block in a buffer waiting for a delimited message or final
 * N_DATA_REQ.
 */
STATIC INLINE fastcall __hot_write int
ne_write_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	size_t dlen;
	np_long NPI_error;

	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_DATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (npi_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
	   destination of the request is an address that does not have an outstanding reset
	   indication. */
	if (unlikely(npi_not_state(np, NSM_OUTDATA)))
		goto error;
	/* If we are writing we must include the IP header, which is at least 20 bytes, and, if the 
	   Stream is bound to a port, at least the size of a UDP header.  The length of the entire
	   NSDU must not exceed 65535 bytes. */
	NPI_error = NBADDATA;
	if (unlikely((dlen = msgsize(mp)) < 20 || dlen > 65535))
		goto error;
	if (unlikely((NPI_error = npi_data_req(q, mp)) < 0))
		goto error;
      discard:
	return (QR_DONE);
      error:
	return ne_error_reply(q, NPI_error);
}

/**
 * ne_data_req:- process N_DATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATA_REQ message
 *
 * Unfortunately, there is no standard way of specifying destination and source addreses for
 * multihomed hosts.  We could use N_OPTMGMT_REQ to change the primary destination address, source
 * address and QOS parameters.  We could define a non-standard DATA_xfer_flags to indicate that the
 * IP header is included in the attached M_DATA blocks.  Or, we could accept N_UNITDATA_REQ
 * primitives even though we are in N_CONS mode.
 *
 * TODO: We should check the MORE_DATA flag and see whether this is a complete NSDU or not.  If not,
 * we should accumulate the M_DATA block in a buffer waiting for a final N_DATA_REQ or delimited
 * message.
 */
STATIC INLINE fastcall __hot_put int
ne_data_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_data_req_t *p;
	size_t dlen;
	mblk_t *dp;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_data_req_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_DATA_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_DATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (npi_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
	   destination of the request is an address that does not have an outstanding reset
	   indication. */
	NPI_error = NOUTSTATE;
	if (unlikely(npi_not_state(np, NSM_OUTDATA)))
		goto error;
	NPI_error = NBADFLAG;
	if (unlikely(p->DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG)))
		goto error;
	/* If there is no dst we must include the IP header, which is at least 20 bytes, and, if
	   the Stream is bound to a port, at least the size of a UDP header.  The length of the
	   entire NSDU must not exceed 65535 bytes. */
	NPI_error = NBADDATA;
	if (unlikely(!(dp = mp->b_cont) || (dlen = msgsize(dp)) < 20 || dlen > 65535))
		goto error;
	if (unlikely((NPI_error = npi_data_req(q, mp->b_cont)) < 0))
		goto error;
	return (QR_STRIP);
      discard:
	return (QR_DONE);
      error:
	return ne_error_reply(q, NPI_error);
}

/**
 * ne_exdata_req:- process N_EXDATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_EXDATA_REQ message
 */
STATIC __unlikely int
ne_exdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_exdata_req_t *p;
	mblk_t *dp;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_EXDATA_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_EXDATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (npi_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	if (unlikely(npi_not_state(np, NSM_OUTDATA)))
		goto error;
	NPI_error = NBADDATA;
	if (unlikely(!(dp = mp->b_cont) || dp->b_wptr < dp->b_rptr + 1))
		goto error;
	if (unlikely((NPI_error = npi_exdata_req(q, mp->b_cont)) < 0))
		goto error;
	return (QR_STRIP);
      discard:
	return (QR_DONE);
      error:
	return ne_error_reply(q, NPI_error);
}

/**
 * ne_datack_req:- process N_DATACK_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATACK_REQ message
 */
STATIC __unlikely int
ne_datack_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_datack_req_t *p;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_DATACK_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_DATACK_REQ primitive, then the NS provider should discard the request without
	   generating a fatal error. */
	if (npi_get_state(np) == NS_IDLE)
		goto discard;
	if (unlikely(npi_not_state(np, NSM_CONNECTED)))
		goto error;
	/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the receipt
	   confirmation flag set, then the NS provider should just ignore the request without
	   generating a fatal error. */
	if (unlikely(np->datinds <= 0))
		goto error;
	if (unlikely((NPI_error = npi_datack_req(q)) < 0))
		goto error;
      discard:
	return (QR_DONE);
      error:
	return ne_error_reply(q, NPI_error);
}

/**
 * ne_reset_req:- process N_RESET_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_REQ message
 */
STATIC INLINE fastcall int
ne_reset_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_reset_req_t *p;
	struct sockaddr_in *DEST_buffer = NULL;
	np_ulong DEST_length;
	np_long NPI_error;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_reset_req_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_RESET_REQ))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_RESET_REQ primitive, then the NS provider should discard the message without
	   generating an error. */
	NPI_error = 0;
	if (npi_get_state(np) == NS_IDLE)
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(npi_get_state(np) != NS_DATA_XFER))
		goto error;
	if ((DEST_length = p->DEST_length) != 0) {
		NPI_error = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + DEST_length))
			goto error;
		if (unlikely(DEST_length != sizeof(*DEST_buffer)))
			goto error;
		DEST_buffer = (struct sockaddr_in *) (mp->b_rptr + p->DEST_offset);
		if (unlikely(DEST_buffer->sin_family != AF_INET))
			goto error;
	}
	/* Ok, message checks out. */
	npi_set_state(np, NS_WCON_RREQ);
	if ((NPI_error = ne_reset_con(q, DEST_buffer, DEST_length, N_USER, p->RESET_reason)) < 0)
		goto error;
	return (NPI_error);
      error:
	return ne_error_ack(q, N_RESET_REQ, NPI_error);

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
	struct np *np = NP_PRIV(q);
	N_reset_res_t *p;
	struct sockaddr_in dst_buf, *DEST_buffer = &dst_buf;
	struct np_daddr *da;
	mblk_t **bpp, **dpp = NULL;
	np_long NPI_error;
	int i;

	NPI_error = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_reset_res_t *) mp->b_rptr;
	NPI_error = -EFAULT;
	if (unlikely(p->PRIM_type != N_RESET_RES))
		goto error;
	NPI_error = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	NPI_error = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	if (unlikely(npi_not_state(np, NSF_WRES_RIND)))
		goto error;
	NPI_error = NNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	NPI_error = NBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
		goto error;
	if (unlikely(p->DEST_length != sizeof(*DEST_buffer)))
		goto error;
	/* address might not be aligned */
	bcopy(mp->b_rptr + p->DEST_offset, DEST_buffer, p->DEST_length);
	if (unlikely(DEST_buffer->sin_family != AF_INET))
		goto error;
	NPI_error = NNOADDR;
	if (unlikely(DEST_buffer->sin_addr.s_addr == INADDR_ANY))
		goto error;
	/* find pointer to pointer to oldest matching reset indication */
	for (bpp = &np->resq; *(bpp); bpp = &(*bpp)->b_next) {
		struct iphdr *iph = (struct iphdr *) (*bpp)->b_rptr;

		if (np->qos.protocol == iph->protocol && DEST_buffer->sin_addr.s_addr == iph->saddr)
			dpp = bpp;
	}
	NPI_error = NBADADDR;
	if (!dpp)
		goto error;
	/* find pointer to corresponding destination routing information */
	for (i = 0, da = np->daddrs; i < np->dnum; i++, da++)
		if (da->addr == DEST_buffer->sin_addr.s_addr)
			break;
	if (i >= np->dnum)
		goto error;
	/* Ok, parameters check out. */
	npi_set_state(np, NS_WACK_RRES);
	return ne_ok_ack(q, N_RESET_RES, DEST_buffer, sizeof(*DEST_buffer), NULL, NULL, NULL, 0);
      error:
	return ne_error_ack(q, N_RESET_RES, NPI_error);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/**
 * npi_w_proto: process an M_PROTO, M_PCPROTO message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_PROTO, M_PCPROTO message to process
 *
 * These are normal N-primitives written from the upper layer protocol.
 */
STATIC INLINE fastcall __hot_put int
npi_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct np *np = NP_PRIV(q);
	np_long oldstate = npi_get_state(np);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *((np_long *) mp->b_rptr))) {
		case N_UNITDATA_REQ:
			rtn = ne_unitdata_req(q, mp);
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
		npi_set_state(np, oldstate);
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

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE fastcall __hot_write int
npi_w_data(queue_t *q, mblk_t *mp)
{
	return ne_write_req(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
npi_w_other(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		np->u.dev.cminor);
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
npi_r_other(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		np->u.dev.cminor);
#if 0
	putnext(q, mp);
	return (QR_ABSORBED);
#else
	return (-EOPNOTSUPP);
#endif
}

/**
 *  npi_r_data: process M_DATA message
 *  @q: active queue in queue pair (read queue)
 *  @mp: the M_DATA message
 *
 *  M_DATA messages are placed to the read queue by the Linux IP npi_v4_rcv() callback.  The message
 *  contains a complete IP datagram starting with the IP header.  What needs to be done is to
 *  convert this to an upper layer indication and deliver it upstream.
 */
STATIC INLINE fastcall __hot_in int
npi_r_data(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	int rtn;

	switch (np->info.SERV_type) {
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
 *  npi_r_error: process M_ERROR message
 *  @q: active queue in queue pair (read queue)
 *  @mp: the M_ERROR message
 *
 *  M_ERROR messages are placed to the read queue by the Linux IP npi_v4_err() callback.  The message
 *  contains a complete ICMP datagram starting with the IP header.  What needs to be done is to
 *  convert this to an upper layer indication and deliver it upstream.
 */
STATIC fastcall int
npi_r_error(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	int rtn;

	switch (np->info.SERV_type) {
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
npi_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return npi_r_data(q, mp);
	case M_ERROR:
		return npi_r_error(q, mp);
	default:
		return npi_r_other(q, mp);
	}
}
STATIC INLINE streamscall __hot_put int
npi_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return npi_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return npi_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	default:
		return npi_w_other(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
/**
 * npi_lookup_conn - lookup Stream in the connection hashes
 * @iph:	IP header
 * @uh:		UDP header
 *
 * The start of the right kind of code shaping up here.  We provide connection buckets that have
 * IP protocol number, source port and destination port as hash parameters.  Attached to the
 * connection bucket are all Streams with the same protocol number, source port and destination port
 * combination (but possibly different IP adresses).  These Streams that are "owners" of the
 * connection bucket must be traversed and checked for address matches.
 */
STATIC INLINE fastcall struct np *
npi_lookup_conn(struct iphdr *iph, struct udphdr *uh)
{
	unsigned char proto = iph->protocol;
	unsigned short sport = uh->source;
	unsigned short dport = uh->dest;
	uint32_t saddr = iph->saddr;
	uint32_t daddr = iph->daddr;
	struct np *result = NULL;
	int hiscore = 0;
	struct npi_chash_bucket *hp, *hp1, *hp2;

	hp1 = &npi_chash[npi_chashfn(proto, sport, dport)];
	hp2 = &npi_chash[npi_chashfn(proto, 0, 0)];

	hp = hp1;
	do {
		read_lock_bh(&hp->lock);
		{
			struct np *np;
			np_ulong state;
			int i;

			for (np = hp->list; np; np = np->cnext) {
				int score = 0;

				/* only Streams in close to the correct state */
				if ((state = npi_get_state(np)) != NS_DATA_XFER
				    && state != NS_WRES_RIND)
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
				{
					int found = 0;

					for (i = 0; i < np->snum; i++) {
						if (np->saddrs[i].addr != 0) {
							if (np->saddrs[i].addr != daddr)
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

					for (i = 0; i < np->snum; i++) {
						if (np->daddrs[i].addr != 0) {
							if (np->daddrs[i].addr != saddr)
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
					result = np;
				}
				if (score == 4)
					/* perfect match */
					break;
			}
		}
		read_unlock_bh(&hp->lock);
	} while (hiscore < 4 && hp != hp2 && (hp = hp2));
	np_get(result);
	usual(result);
	return (result);
}

/**
 * npi_lookup_bind - lookup Stream in the bind/listening hashes
 * @iph:	IP header
 * @uh:		UDP header
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
npi_lookup_bind(struct iphdr *iph, struct udphdr *uh)
{
	unsigned char proto = iph->protocol;
	unsigned short bport = uh->dest;
	uint32_t daddr = iph->daddr;
	struct np *result = NULL;
	int hiscore = 0;
	struct npi_bhash_bucket *hp, *hp1, *hp2;

	hp1 = &npi_bhash[npi_bhashfn(proto, bport)];
	hp2 = &npi_bhash[npi_bhashfn(proto, 0)];

	hp = hp1;
	do {
		read_lock_bh(&hp->lock);
		{
			struct np *np;
			np_ulong state;
			int i;

			for (np = hp->list; np; np = np->bnext) {
				int score = 0;

				/* only listening N_COTS Streams and N_CLTS Streams */
				if (np->CONIND_number == 0 && np->info.SERV_type != N_CLNS)
					continue;
				/* only Streams in close to the correct state */
				if ((state = npi_get_state(np)) != NS_IDLE && state != NS_WACK_UREQ)
					continue;
				if (np->bport != 0) {
					if (np->bport != bport)
						continue;
					score++;
				}
				if (!(np->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
					score++;
				{
					int found = 0;

					for (i = 0; i < np->bnum; i++) {
						if (np->baddrs[i].addr != 0) {
							if (np->baddrs[i].addr != daddr)
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
					result = np;
				}
				if (score == 3)
					/* perfect match */
					break;
			}
		}
		read_unlock_bh(&hp->lock);
	} while (hiscore < 2 && hp != hp2 && (hp = hp2));
	np_get(result);
	usual(result);
	return (result);
}

/**
 * npi_lookup_next - lookup next Stream by protocol, address and port.
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
STATIC INLINE fastcall struct np *
npi_lookup_next(struct np *ip_prev, struct iphdr *iph, struct udphdr *uh)
{
	struct np *result = NULL;
	struct ip_prot_bucket *pp, **ppp;

	ppp = &ip_prots[iph->protocol];

	read_lock_bh(&ip_prot_lock);
	if ((pp = *ppp)) {
		if (pp->corefs > 0) {
			if (result == NULL)
				result = npi_lookup_conn(iph, uh);
			if (result == NULL)
				result = npi_lookup_bind(iph, uh);
		} else if (pp->clrefs > 0) {
			if (result == NULL)
				result = npi_lookup_bind(iph, uh);
		}
	}
	read_unlock_bh(&ip_prot_lock);
	return (result);
}

STATIC INLINE fastcall struct np *
npi_lookup(struct iphdr *iph, struct udphdr *uh)
{
	return npi_lookup_next(NULL, iph, uh);
}

/**
 * npi_lookup_icmp - a little different from npi_lookup(), for ICMP messages
 * @iph:	returned IP header
 * @len:	length of returned message
 *
 * This needs to do a reverse lookup (where destination address and port are compared to source
 * address and port, and visa versa).
 */
STATIC INLINE fastcall struct np *
npi_lookup_icmp(struct iphdr *iph, unsigned int len)
{
	fixme(("Write this function.\n"));
	return (NULL);
}

STATIC streamscall void
npi_free(char *data)
{
	struct sk_buff *skb = (typeof(skb)) data;

	_trace();
	/* sometimes skb is NULL if it has been stolen */
	if (skb != NULL)
		kfree_skb(skb);
	return;
}

/**
 * npi_v4_rcv: - process a received IP packet
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
npi_v4_rcv(struct sk_buff *skb)
{
	read_lock_bh(&ip_prot_lock);	/* lock stream lists */
	{
		struct np *np;
		struct iphdr *iph = skb->nh.iph;
		struct udphdr *uh = (struct udphdr *) (skb->nh.raw + (iph->ihl << 2));

		if ((np = npi_lookup(iph, uh))) {
			npi_v4_steal(skb);
			if (np->oq && canput(np->oq)) {
				mblk_t *mp;
				frtn_t fr = { &npi_free, (char *) skb };
				size_t plen = skb->len + (skb->data - skb->nh.raw);

				/* FIXME: handle non-linear sk_buffs */
				if ((mp = esballoc(skb->nh.raw, plen, BPRI_MED, &fr))) {
					mp->b_datap->db_type = M_DATA;
					mp->b_wptr += plen;
					put(np->oq, mp);
				} else
					kfree_skb(skb);
			} else
				kfree_skb(skb);
		} else if (npi_v4_rcv_next(skb)) {
			/* TODO: want to generate an ICMP error here */
		}
	}
	read_unlock_bh(&ip_prot_lock);
	return (0);
}

/**
 * npi_v4_err: - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information
 *
 * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned packet, and IP payload
 * of returned packet (up to some number of bytes of total payload).  The passed in sk_buff has
 * skb->data pointing to the ICMP payload which is the beginning of the returned IP header.
 * However, we include the entire packet in the message.
 */
STATIC void
npi_v4_err(struct sk_buff *skb, u32 info)
{
	read_lock_bh(&ip_prot_lock);
	{
		struct np *np;

		/* Note: use returned IP header and possibly payload for lookup */
		if ((np = npi_lookup_icmp((struct iphdr *) skb->data, skb->len))) {
			if (np->oq && canput(np->oq)) {
				mblk_t *mp;
				size_t plen = skb->len + (skb->data - skb->nh.raw);

				if ((mp = allocb(plen, BPRI_MED))) {
					mp->b_datap->db_type = M_ERROR;
					bcopy(skb->nh.raw, mp->b_wptr, plen);
					mp->b_wptr += plen;
					put(np->oq, mp);
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
npi_term_caches(void)
{
	if (npi_prot_cachep != NULL) {
		if (kmem_cache_destroy(npi_prot_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy npi_prot_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed npi_prot_cachep\n", DRV_NAME));
		npi_prot_cachep = NULL;
	}
	if (npi_bind_cachep != NULL) {
		if (kmem_cache_destroy(npi_bind_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy npi_bind_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed npi_bind_cachep\n", DRV_NAME));
		npi_bind_cachep = NULL;
	}
	if (npi_priv_cachep != NULL) {
		if (kmem_cache_destroy(npi_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy npi_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed npi_priv_cachep\n", DRV_NAME));
		npi_priv_cachep = NULL;
	}
	return (0);
}
STATIC int
npi_init_caches(void)
{
	if (npi_priv_cachep == NULL) {
		npi_priv_cachep = kmem_cache_create("npi_priv_cachep", sizeof(struct np), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (npi_priv_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate npi_priv_cachep", __FUNCTION__);
			npi_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (npi_bind_cachep == NULL) {
		npi_bind_cachep = kmem_cache_create("npi_bind_cachep", sizeof(struct np), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (npi_bind_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate npi_bind_cachep", __FUNCTION__);
			npi_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver bind structure cache\n", DRV_NAME));
	}
	if (npi_prot_cachep == NULL) {
		npi_prot_cachep = kmem_cache_create("npi_prot_cachep", sizeof(struct np), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (npi_prot_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate npi_prot_cachep", __FUNCTION__);
			npi_term_caches();
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
		npi_bhash_size = (1 << order) * PAGE_SIZE / sizeof(struct npi_bhash_bucket);
		npi_bhash = (struct npi_bhash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (npi_bhash == NULL && --order >= 0);
	if (!npi_bhash)
		cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
	npi_bhash_size = npi_chash_size = npi_bhash_size >> 1;
	npi_bhash_order = npi_chash_order = npi_bhash_order - 1;
	bzero(npi_bhash, npi_bhash_size * sizeof(struct npi_bhash_bucket));
	bzero(npi_chash, npi_chash_size * sizeof(struct npi_chash_bucket));
	for (i = 0; i < npi_bhash_size; i++)
		rwlock_init(&npi_bhash[i].lock);
	for (i = 0; i < npi_chash_size; i++)
		rwlock_init(&npi_chash[i].lock);
	printd(("%s: INFO: bind hash table configured size = %d\n", DRV_NAME, npi_bhash_size));
	printd(("%s: INFO: conn hash table configured size = %d\n", DRV_NAME, npi_chash_size));
}

/**
 * npi_alloc_priv: - allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @slp: pointer to place in list for insertion
 * @type: type of provider: 0, N_CONS, N_CLTS
 * @devp: pointer to device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initializes it to appropriate values, and then inserts it into
 * the private structure list.
 */
STATIC struct np *
npi_alloc_priv(queue_t *q, struct np **slp, int type, dev_t *devp, cred_t *crp)
{
	struct np *np;

	if ((np = np_alloc())) {
		np->u.dev.cmajor = getmajor(*devp);
		np->u.dev.cminor = getminor(*devp);
		np->cred = *crp;
		(np->oq = q)->q_ptr = np_get(np);
		(np->iq = WR(q))->q_ptr = np_get(np);
		np->i_prim = &npi_w_prim;
		np->o_prim = &npi_r_prim;
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
		/* np specific members */
		np->info.SERV_type = type;
		/* link into master list */
		write_lock_bh(&master.lock);
		{
			np_get(np);
			if ((np->next = *slp))
				np->next->prev = &np->next;
			np->prev = slp;
			*slp = np;
		}
		write_unlock_bh(&master.lock);
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
	return (np);
}

/**
 * npi_free_priv: - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
STATIC void
npi_free_priv(queue_t *q)
{
	struct np *np;

	ensure(q, return);
	np = NP_PRIV(q);
	ensure(np, return);
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&np->refcnt)));
	/* make sure the stream is disconnected */
	if (np->chash != NULL) {
		npi_disconnect(np, NULL, NULL, 0);
		npi_set_state(np, NS_IDLE);
	}
	/* make sure the stream is unbound */
	if (np->bhash != NULL) {
		npi_unbind(np);
		npi_set_state(np, NS_UNBND);
	}
	/* FIXME: purge conq and resq */
	ss7_unbufcall((str_t *) np);
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME,
		atomic_read(&np->refcnt)));
	write_lock_bh(&master.lock);
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	write_unlock_bh(&master.lock);
	np_put(np);
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&np->refcnt)));
	np_release((struct np **) &np->oq->q_ptr);
	np->oq = NULL;
	np_release((struct np **) &np->iq->q_ptr);
	np->iq = NULL;
	assure(atomic_read(&np->refcnt) == 1);
	np_release(&np);	/* should normally be final put */
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
STATIC int npi_majors[IP_CMAJORS] = { IP_CMAJOR_0, };

/**
 * npi_qopen: - NPI IP driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
npi_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct np *np, **npp = &master.np.list;
	mblk_t *mp;
	struct stroptions *so;

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
	if (!(mp = allocb(sizeof(*so), BPRI_MED)))
		return (ENOBUFS);
	write_lock_bh(&master.lock);
	for (; *npp; npp = &(*npp)->next) {
		if (cmajor != (*npp)->u.dev.cmajor)
			break;
		if (cmajor == (*npp)->u.dev.cmajor) {
			if (cminor < (*npp)->u.dev.cminor)
				break;
			if (cminor == (*npp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= IP_CMAJORS
					    || !(cmajor = npi_majors[mindex]))
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
		freeb(mp);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(np = npi_alloc_priv(q, npp, type, devp, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_bh(&master.lock);
		freeb(mp);
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
	/* want to set a write offet of 20 bytes */
	so = (typeof(so)) mp->b_wptr++;
	so->so_flags = SO_WROFF | SO_DELIM;
	so->so_wroff = 20;
	putnext(q, mp);
	qprocson(q);
	return (0);
}

/**
 * npi_qclose: - NPI IP driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
STATIC streamscall int
npi_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct np *np = PRIV(q);

	(void) oflag;
	(void) crp;
	(void) np;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, np->u.dev.cmajor,
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
	qprocsoff(q);
	npi_free_priv(q);
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
		if (npi_majors[mindex]) {
			if ((err = ip_unregister_strdev(npi_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					npi_majors[mindex]);
			if (mindex)
				npi_majors[mindex] = 0;
		}
	}
	if ((err = npi_term_caches()))
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
	if ((err = npi_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ipterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ip_register_strdev(npi_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					npi_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				ipterminate();
				return (err);
			}
		}
		if (npi_majors[mindex] == 0)
			npi_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(npi_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = npi_majors[0];
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
