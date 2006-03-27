/*****************************************************************************

 @(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/27 01:25:54 $

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

 Last Modified $Date: 2006/03/27 01:25:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip.c,v $
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

#ident "@(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/27 01:25:54 $"

static char const ident[] =
    "$RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/27 01:25:54 $";

/*
   This driver provides the functionality of an IP (Internet Protocol) hook
   similar to raw sockets, with the exception that the hook acts as a port
   bound intercept for IP packets for the bound protocol ids.  This dirver is
   used primarily by OpenSS7 protocol test module (e.g. for SCTP) and for
   applications where entire ranges of port numbers for an existing protocol
   id must be intercepted (e.g. for RTP/RTCP).  This driver uses  hook into
   the Linux IP protocol tables and passes packets tranparently on to the
   underlying protocol in which it is not interested (bound).  The driver
   uses the NPI (Network Provider Interface) API.
*/

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>
#include <linux/interrupt.h>

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <net/ip.h>
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
#define IP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define IP_REVISION	"OpenSS7 $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/27 01:25:54 $"
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

STATIC streamscall int ip_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ip_close(queue_t *, int, cred_t *);

STATIC streamscall int ip_rput(queue_t *, mblk_t *);
STATIC streamscall int ip_rsrv(queue_t *);

STATIC struct qinit ip_rinit = {
	.qi_putp = ip_rput,		/* Read put (msg from below) */
	.qi_srvp = ip_rsrv,		/* Read queue service */
	.qi_qopen = ip_open,		/* Each open */
	.qi_qclose = ip_close,		/* Last close */
	.qi_minfo = &ip_minfo,		/* Information */
};

STATIC streamscall int ip_wput(queue_t *, mblk_t *);
STATIC streamscall int ip_wsrv(queue_t *);

STATIC struct qinit ip_winit = {
	.qi_putp = ip_wput,		/* Write put (msg from above) */
	.qi_srvp = ip_wsrv,		/* Write queue service */
	.qi_minfo = &ip_minfo,		/* Information */
};

MODULE_STATIC struct streamtab ip_info = {
	.st_rdinit = &ip_rinit,		/* Upper read queue */
	.st_wrinit = &ip_winit,		/* Lower read queue */
};

/*
   Queue put and service return values 
 */
#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7

/*
 *  Primary data structure.
 */
struct ip;
typedef struct ip ip_t;
struct ip {
	ip_t *next;			/* linkage for master list */
	ip_t **prev;			/* linkage for master list */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	cred_t cred;			/* credentials of opener */
	atomic_t refcnt;		/* structure refe(rence count */
	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* RD queue waiting on lock */
	queue_t *wwait;			/* WR queue waiting on lock */
	int users;			/* lock holders */
	int rbid;			/* RD bufcall id */
	int wbid;			/* WR bufcall id */
	np_ulong i_flags;		/* interface flags */
	np_ulong i_state;		/* interface state */
	np_ulong state;			/* IP state */
	np_ulong provider;		/* provider type */
	ip_t *bnext;			/* linkage for bind hash */
	ip_t **bprev;			/* linkage for bind hash */
	struct ip_bind_bucket *bindb;	/* linkage for bind hash */
	unsigned char tos;		/* IP type of service */
	unsigned char ttl;		/* IP time to live */
	struct sockaddr_storage addr;	/* bound addresses */
	size_t anum;			/* number of sockaddr_in addresses in addr */
	uint8_t protoids[16];		/* bound protocol ids */
	size_t pnum;			/* number of protocol ids in protoids */
	uint pmtu;			/* path MTU for association */
	ulong userlocks;
	unsigned short sport;
};

#define PRIV(__q) (((__q)->q_ptr))
#define IP_PRIV(__q) ((struct ip *)((__q)->q_ptr))

STATIC struct ip *ip_opens = NULL;
STATIC spinlock_t ip_lock = SPIN_LOCK_UNLOCKED;

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
	struct ip *dflt;		/* default destinations for this protocol */
};
struct ip_hash_bucket {
	rwlock_t lock;
	struct ip_bind_bucket *list;
};

struct ip_prot_bucket {
	unsigned char proto;		/* protocol number */
	char name[15];			/* protocol name */
	int refs;			/* reference count */
	int override;			/* was entry overriden */
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
	struct inet_protocol prot;	/* Linux registration structure */
	struct inet_protocol *next;	/* Linkage for protocol override */
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
	struct net_protocol prot;	/* Linux registration structure */
	struct net_protocol *next;	/* Linkage for protocol override */
#else
#error HAVE_KTYPE_STRUCT_INET_PROTOCOL or HAVE_KTYPE_STRUCT_NET_PROTOCOL must be defined.
#endif
};
STATIC rwlock_t ip_prot_lock = RW_LOCK_UNLOCKED;
STATIC struct ip_prot_bucket *ip_prots[MAX_INET_PROTOS];

STATIC kmem_cache_t *ip_bind_cachep;
STATIC kmem_cache_t *ip_prot_cachep;
STATIC kmem_cache_t *ip_priv_cachep;

STATIC INLINE struct ip *
ip_get(void)
{
	struct ip *ip;

	if ((ip = kmem_cache_alloc(ip_priv_cachep, SLAB_ATOMIC))) {
		bzero(ip, sizeof(*ip));
		atomic_set(&ip->refcnt, 1);
	}
	return (ip);
}
STATIC INLINE void
ip_hold(struct ip *ip)
{
	if (ip)
		atomic_inc(&ip->refcnt);
}
STATIC INLINE void
ip_put(struct ip *ip)
{
	if (ip)
		if (atomic_dec_and_test(&ip->refcnt)) {
			kmem_cache_free(ip_priv_cachep, ip);
		}
}

/*
 *  State changes.
 */
#ifdef DEBUG
STATIC const char *
state_name(long state)
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
STATIC void
ip_set_state(struct ip *ip, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, ip, state_name(state), state_name(ip->i_state)));
	ip->i_state = state;
}
STATIC long
ip_get_state(struct ip *ip)
{
	return (ip->i_state);
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
 *  Some notes are in order here: These two functions, inet_override_protocol()  and
 *  inet_restore_protocol() are intended to provide a drop and insert packet tap on received IP
 *  packets.
 *
 *  There are two approaches: an older approach for 2.4 kernels and a newer approach for
 *  2.6 kernels (and some patched back 2.4 kernels like RHEL3).
 *
 *  Under the older approach, when multiple protocol users are registered with inet_add_protocol()
 *  the kernel will duplicate the sk_buffs and pass a copy to each registered protocol user.
 *
 *  Under the newer approach, inet_add_protocol() will refuse to register multiple protocol users
 *  and will simply return -1 if it is attempted.
 *
 *  What we want is a mechanism whereby a single sk_buff is passed.  Our protocol user is registered
 *  ahead of existing protocol users.  When our protocol user receives an sk_buff that is not for
 *  it, it has the option of passing the sk_buff on to the next registered protocol user and is
 *  given notice if the sk_buff was indeed passed.  This permits us to overlay a protocol stack over
 *  an existing protocol stack.  This is useful for rawip, UDP (for use by RTP) and SCTP.  For rawip
 *  it permits us to intercept specific packets.
 *
 *  In fact this is very similar to a netfilter hook on local ip delivery.  The problem is that icmp
 *  errors are delivered to net protocols but are not delivered to netfilter hooks: a (rather
 *  extensive) netfilter hook for icmp messages would also be necessary.
 *
 *  Note also that Linux only delivers ICMP messages to one net protocol: the first on the list.  We
 *  want to be able to also send ICMP messages onwards that we are not interested in.
 */

#if defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
/**
 *  inet_override_protocol: - register or override a net protocol
 *  @pb: freshly allocated protocol bucket
 *  @proto: the protocol to register or override
 *
 */
STATIC void
inet_override_protocol(struct ip_prot_bucket *pb, int proto)
{
	bzero(pb, sizeof(*pb));
	pb->refs = 1;
	pb->override = 1;
	pb->proto = proto;
	pb->prot.handler = &ip_v4_rcv;
	pb->prot.err_handler = &ip_v4_err;
	pb->prot.protocol = proto;
	pb->prot.name = pb->name;
	pb->prot.next = NULL;
	pb->prot.copy = 0;
	{
		unsigned char hash = proto & (MAX_INET_PROTOS - 1);
		struct inet_protocol **p = &inet_protos[hash];

		br_write_lock_bh(BR_NETPROTO_LOCK);
		{
			/* FIXME: need to find the module that owns pb->prot.next and take a
			   reference */
			pb->next = (*p);
			(*p) = &pb->prot;
		}
		br_write_unlock_bh(BR_NETPROTO_LOCK);
	}
}

/**
 *  inet_restore_protocol: - restore or deregister a new protocol
 *  @pb: existing registered protocol bucket
 *
 */
STATIC void
inet_restore_protocol(struct ip_prot_bucket *pb)
{
	unsigned char proto = pb->proto;
	unsigned char hash = proto & (MAX_INET_PROTOS - 1);
	struct inet_protocol **p = &inet_protos[hash];

	if (pb == NULL)
		return;

	br_write_lock_bh(BR_NETPROTO_LOCK);
	{
		while ((*p) != NULL && (*p) != &pb->prot)
			p = &(*p)->next;
		if ((*p) == &pb->prot)
			(*p) = pb->next;
		/* FIXME: restore copy pointers on the list */
	}
	br_write_unlock_bh(BR_NETPROTO_LOCK);
}

/**
 *  inet_pass_next_handler: - pass sk_buff to next handler
 *  @skbp: pointer to sk_buff pointer
 *
 *  Pass the sk_buff to the next handler, if any.  If the sk_buff is passed, NULL the pointer for
 *  the caller to indicate that the caller is no longer responsible for the sk_buff and return the
 *  return value that ip_v4_rcv() should return.
 */
STATIC int
inet_pass_next_handler(struct sk_buff **skbp)
{
	struct iphdr *iph = skb->nh.iph;
	unsigned char proto = iph->protocol;
	unsigned char hash = proto & (MAX_INET_PROTOS - 1);
	struct inet_protocol **p = &inet_protos[hash];
	struct ip_prot_bucket *pb;
	int rtn = 0;

	br_read_lock_bh(BR_NETPROTO_LOCK);
	{
		if ((pb = ip_protos[hash])) {
			for (; (*p); p = &(*p)->next)
				if ((*p) == &pb->prot)
					break;
			for (; (*p); p = &(*p)->next)
				if ((*p) != &pb->prot)
					break;
			if (*p) {
				rtn = (*(*p)->handler) (*skbp);
				*skbp = NULL;
			}
		}
	}
	br_read_unlock_bh(BR_NETPROTO_LOCK);
	return (rtn);
}

/**
 *  inet_pass_next_err_handler: - pass sk_buff to next error handler
 *  @skb: pointer to sk_buff
 *  @info: information
 *
 *  Pass the sk_buff to the next err_handler, if any.
 */
STATIC void
inet_pass_next_err_handler(struct sk_buff *skb, __u32 info)
{
	struct iphdr *iph = skb->nh.iph;
	unsigned char proto = iph->protocol;
	unsigned char hash = proto & (MAX_INET_PROTOS - 1);
	struct inet_protocol **p = &inet_protos[hash];
	struct ip_prot_bucket *pb;

	br_read_lock_bh(BR_NETPROTO_LOCK);
	{
		if ((pb = ip_protos[hash])) {
			for (; (*p); p = &(*p)->next)
				if ((*p) == &pb->prot)
					break;
			for (; (*p); p = &(*p)->next)
				if ((*p) != &pb->prot)
					break;
			if (*p)
				(*(*p)->err_handler) (skb, info);
		}
	}
	br_read_unlock_bh(BR_NETPROTO_LOCK);
}

#elif defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;

STATIC struct net_protocol **inet_protosp = (typeof(inet_protosp)) HAVE_INET_PROTOS_ADDR;

/**
 *  inet_override_protocol: - register or override a net protocol
 *  @pb: freshly allocated protocol bucket
 *  @proto: the protocol to register or override
 *
 *  This is the newer 2.6 (and patched back 2.4) version of the function.  We always register
 *  ourselves and detach the list.
 */
STATIC void
inet_override_protocol(struct ip_prot_bucket *pb, int proto)
{
	bzero(pb, sizeof(*pb));
	pb->refs = 1;
	pb->proto = proto;
	{
		int hash = proto & (MAX_INET_PROTOS - 1);
		struct net_protocol **p = &inet_protosp[hash];

		spin_lock_bh(inet_proto_lockp);
		{
			/* FIXME: need to find the module that owns inet_protos[hash] and take a
			   reference */
			if (*p) {
				/* override existing entry */
				pb->override = 1;
				pb->prot.handler = (*p)->handler;
				pb->prot.err_handler = (*p)->err_handler;
				pb->prot.no_policy = (*p)->no_policy;
				(*p)->handler = &ip_v4_rcv;
				(*p)->err_handler = &ip_v4_err;
				(*p)->no_policy = 1;
			} else {
				/* add our own entry */
				pb->override = 0;
				pb->prot.handler = &ip_v4_rcv;
				pb->prot.err_handler = &ip_v4_err;
				pb->prot.no_policy = 1;
				inet_protosp[hash] = &pb->prot;
			}
		}
		spin_unlock_bh(inet_proto_lockp);
	}
}

/**
 *  inet_restore_protocol: - restore or deregister a new protocol
 *  @pb: existing registered protocol bucket
 *
 *  This is the newer 2.6 (and patched back 2.4) version of the function.  We always deregister
 *  ourselves and reattach the list.
 */
STATIC void
inet_restore_protocol(struct ip_prot_bucket *pb)
{
	unsigned char proto = pb->proto;
	int hash = proto & (MAX_INET_PROTOS - 1);
	struct net_protocol **p = &inet_protosp[hash];

	spin_lock_bh(inet_proto_lockp);
	{
		if (pb->override) {

			/* could have already been removed */
			if (*p) {
				/* restore original values */
				(*p)->handler = pb->prot.handler;
				(*p)->err_handler = pb->prot.err_handler;
				(*p)->no_policy = pb->prot.no_policy;
			}
		} else {
			inet_protos[hash] = NULL;
		}
	}
	spin_unlock_bh(inet_proto_lockp);
	synchronize_net();
}

/**
 *  inet_pass_next_handler: - pass sk_buff to next handler
 *  @skbp: pointer to sk_buff pointer
 *  
 *  If we are receiving a packet at the same time that an underlying protocol gets removed then
 *  there is a big problem.
 */
STATIC int
inet_pass_next_handler(struct sk_buff **skbp)
{
	struct iphdr *iph = (*skbp)->nh.iph;
	unsigned char proto = iph->protocol;
	struct ip_prot_bucket *pb;
	int rtn = 0;

	read_lock(&ip_lock);
	if ((pb = ip_protos[proto])) {
		if (pb->override) {
			rtn = (*pb->proto.handler) (*skbp);
			*skbp = NULL;
		}
	}
	read_unlock(&ip_lock);
	return (rtn);
}

/**
 *  inet_pass_next_err_handler: - pass sk_buff to next error handler
 *  @skb: pointer to sk_buff
 *  @info: information
 *
 *  If we are receiving a packet at the same time that an underlying protocol gets removed then
 *  there is a big problem.
 */
STATIC void
inet_pass_next_err_handler(struct sk_buff *skb, __u32 info)
{
	struct iphdr *iph = (*skbp)->nh.iph;
	unsigned char proto = iph->protocol;
	struct ip_prot_bucket *pb;

	read_lock(&ip_lock);
	if ((pb = ip_protos[proto])) {
		if (pb->override)
			(*pb->proto.err_handler) (skb, info);
	}
	read_unlock(&ip_lock);
}

#else
#error HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY or HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY must be defined.
#endif

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
ip_bind_prot(unsigned char proto)
{
	struct ip_prot_bucket *pb;

	write_lock_bh(&ip_prot_lock);
	{
		if ((pb = ip_prots[proto]) != NULL)
			++pb->refs;
		else
			if ((pb = kmem_cache_alloc(ip_prot_cachep, SLAB_ATOMIC)) != NULL) {
				/* register protocol */
				inet_override_protocol(pb, proto);
				ip_prots[proto] = pb;
			}
	}
	write_unlock_bh(&ip_prot_lock);
	return ((pb != NULL) ? 0 : -ENOMEM);
}

/**
 *  ip_unbind_prot - unbind a protocol
 *  @proto:	    protocol number to unbind
 */
STATIC void
ip_unbind_prot(unsigned char proto)
{
	struct ip_prot_bucket *pb;

	write_lock_bh(&ip_prot_lock);
	{
		if ((pb = ip_prots[proto]) != NULL && --pb->refs <= 0) {
			ip_prots[proto] = NULL;
			/* unregister protocol */
			inet_restore_protocol(pb);
			kmem_cache_free(ip_prot_cachep, pb);
		}
	}
	write_unlock_bh(&ip_prot_lock);
	return;
}

/**
 *  ip_bind_addr -  bind an address and protocol id to a Stream
 *  @q:		    Stream to which to bind
 *  @add:	    sockaddr_in structure with address to bind
 *  @proto:	    protocol id to bind
 *  @flag:	    default bind
 */
STATIC int
ip_bind_addr(queue_t *q, struct sockaddr_in *addr, unsigned char proto, int flag)
{
	int err;

	if ((err = ip_bind_prot(proto)) != 0)
		goto error;
      error:
	return (err);
}

/**
 *  ip_unbind_addr - unbind an address and protocol id from a Stream
 *  @q:		    Stream to which to unbind
 *  @add:	    sockaddr_in structure with address to unbind
 *  @proto:	    protocl id to unbind
 *  @flag:	    default unbind
 */
STATIC void
ip_unbind_addr(queue_t *q, struct sockaddr_in *addr, unsigned char proto, int flag)
{
}

/**
 *  ip_bind_req -   bind to addresses and protocol ids
 *  @q:		    Stream to which to bind
 *  @add_in:	    array of sockaddr_in structures to bind
 *  @anum:	    number of elements in sockaddr_in array
 *  @protoids:	    array of protocol ids to which to bind
 *  @pnum:	    number of elements in protocol id array
 *  @flags:	    bind flags
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
ip_bind_req(queue_t *q, struct sockaddr_in *add_in, size_t anum, unsigned char *protoids,
	    size_t pnum, ulong flags)
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

				if ((err = ip_bind_addr(q, add, proto, 0)))	/* FIXME: what
										   flag? */
					break;
			}
			if (err == 0)
				continue;
			for (; j >= 0; j--) {
				struct sockaddr_in *add = &add_in[j];

				ip_unbind_addr(q, add, proto, 0);	/* FIXME: what flag? */
			}
			break;
		} else {
			/* no addresses means wildcard bind */
			struct sockaddr_in add = {
				.sin_family = AF_INET,
				.sin_port = 0,
				.sin_addr = {.s_addr = 0,},
			};

			err = ip_bind_addr(q, &add, proto, 0);	/* FIXME: what flag? */
			if (err == 0)
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

				ip_unbind_addr(q, add, proto, 0);	/* FIXME: what flag? */
			}
		} else {
			/* no addresses means wildcard bind */
			struct sockaddr_in add = {
				.sin_family = AF_INET,
				.sin_port = 0,
				.sin_addr = {.s_addr = 0,},
			};

			ip_unbind_addr(q, &add, proto, 0);	/* FIXME: what flag? */
		}
	}
	return (err);
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

STATIC void
ip_xmit_msg(uint32_t daddr, uint32_t saddr, uint8_t protoid, mblk_t *mp, struct ip *ip)
{
	struct rtable *rt = NULL;

	ensure(mp, return);
	if (!ip_route_output(&rt, daddr, 0, 0, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp);
		size_t tlen = plen + sizeof(struct iphdr);

		usual(hlen);
		usual(plen);
		/* IMPLEMENTATION NOTE:- Sockets ensures that there is sufficient headroom in the
		   passed in sk_buff, fills out the IP header, calculates the checksum and sends
		   the buffer.  For STREAMS, no socket buffer is passed in, just a message block,
		   so a new socket buffer is allocated with the necessary headroom and the message
		   blocks are copied into the socket buffer. */
		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			unsigned char *data;

			skb_reserve(skb, hlen);
			/* find headers */
			iph = (typeof(iph)) __skb_put(skb, tlen);
			data = (unsigned char *) (iph + 1);
			skb->dst = &rt->u.dst;
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = ip->tos;
			iph->ttl = ip->ttl;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = protoid;
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
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possible a message buffer chain 
			   and we must iterate along the b_cont pointer. */
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
	freemsg(mp);
	return;
}

STATIC int
ip_unitdata_req(queue_t *q, struct sockaddr_in *dest, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	struct sockaddr_in *srce = (struct sockaddr_in *) &ip->addr;
	uint32_t saddr = srce->sin_addr.s_addr;
	uint32_t daddr = dest->sin_addr.s_addr;
	uint8_t protoid = ntohs(dest->sin_port);

	ip_xmit_msg(daddr, saddr, protoid, mp, ip);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Locking
 *
 *  =========================================================================
 */
STATIC int
ip_trylockq(queue_t *q)
{
	int res;
	struct ip *ip = PRIV(q);

	spin_lock_bh(&ip->qlock);
	if (!(res = !ip->users++)) {
		if (q == ip->rq)
			ip->rwait = q;
		if (q == ip->wq)
			ip->wwait = q;
	}
	spin_unlock_bh(&ip->qlock);
	return (res);
}
STATIC void
ip_unlockq(queue_t *q)
{
	struct ip *ip = PRIV(q);

	spin_lock_bh(&ip->qlock);
	if (ip->rwait)
		qenable(xchg(&ip->rwait, NULL));
	if (ip->wwait)
		qenable(xchg(&ip->wwait, NULL));
	ip->users = 0;
	spin_unlock_bh(&ip->qlock);
}

/*
 *  =========================================================================
 *
 *  Buffer Allocation
 *
 *  =========================================================================
 */
/**
 *  
 *  ip_bufsrv: - safe bufcalls
 *  @data: opaque client data
 *
 *  Whereas sockets allocate most buffers in user mode, where it is possible
 *  to simply return an error to a system call, the STREAMS driver runs as a
 *  coroutine and there is no (valid) user present.  This is done with
 *  bufcalls.  However, for LiS and a number of other STREAMS implementations,
 *  bufcalls are unsafe.  Here we do reference counting against the STREAMS
 *  private structure when buffer calls are issued and when they complete.
 *  STREAMS private structures will not be deallocated until the buffer calls
 *  are complete.  There are some mi_bufcall() compatibility functions that
 *  accomplish the same ends.  All of this can go away when this driver only
 *  needs to be used with Linux Fast-STREAMS that has safe buffer callbacks.
 */
STATIC void streamscall
ip_bufsrv(long data)
{
	struct ip *ip;
	queue_t *q;

	q = (queue_t *) data;
	ensure(q, return);
	ip = PRIV(q);
	ensure(ip, return);

	if (q == ip->rq && xchg(&ip->rbid, 0) != 0)
		ip_put(ip);
	if (q == ip->wq && xchg(&ip->wbid, 0) != 0)
		ip_put(ip);
	qenable(q);
	return;
}

/**
 *  ip_unbufcall: - reliably cancel a buffer callback
 *  @q: the queue whose callback to cancel
 *
 *  Cancel a qenable() buffer callback associated with one queue in the queue pair.  ip_unbufcall()
 *  effectively undoes the actions performed by ip_bufcall().
 *
 *  NOTICES: Cancellation of buffer callbacks on LiS using unbufcall() is unreliable.  The callback
 *  function could execute some time shortly after the call to unbufcall() has returned.   LiS
 *  abbrogates the SVR 4 STREAMS principles for unbufcall().  This is why you will find atomic
 *  exchanges here and in the callback function and why reference counting is performed on the
 *  structure and queue pointers are checked for NULL.
 */
STATIC void
ip_unbufcall(queue_t *q)
{
	struct ip *ip;
	bufcall_id_t bid;

	ensure(q, return);
	ip = PRIV(q);
	ensure(ip, return);

	if (q == ip->rq && (bid = xchg(&ip->rbid, 0))) {
		unbufcall(bid);
		ip_put(ip);
	}
	if (q == ip->wq && (bid = xchg(&ip->wbid, 0))) {
		unbufcall(bid);
		ip_put(ip);
	}
	return;
}

/**
 *  ip_bufcall: - generate a buffer callback to enable a queue
 *  @q: the queue to enable on callback
 *  @size: size of the allocation
 *  @prior: priority of the allocation
 *
 *  Maintain one buffer call for each queue in the queue pair.  The callback function will simply
 *  perform a qenable(9).
 *
 *  NOTICES: One of the reasons for going to such extents is that LiS has completely unsafe buffer
 *  callbacks.  The buffer callback function can be invoked (shortly) after unbufcall() returns
 *  under LiS in abrogation of SVR 4 STREAMS principles.  This is why you will find atomic exchanges
 *  here and in the callback function and why reference counting is performed on the structure and
 *  queue pointers are checked for NULL.
 */
STATIC void
ip_bufcall(queue_t *q, size_t size, int prior)
{
	struct ip *ip = IP_PRIV(q);
	bufcall_id_t bid;

	ensure(q, return);
	ip = IP_PRIV(q);
	ensure(ip, return);
	ip_hold(ip);
	if (q == ip->wq) {
		if ((bid = xchg(&ip->wbid, bufcall(size, prior, &ip_bufsrv, (long) ip)))) {
			unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
			ip_put(ip);
		}
		return;
	}
	if (q == ip->rq) {
		if ((bid = xchg(&ip->rbid, bufcall(size, prior, &ip_bufsrv, (long) ip)))) {
			unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
			ip_put(ip);
		}
		return;
	}
	swerr();
	return;
}

/**
 *  ip_allocb: - reliable allocb()
 *  @q: the queue to enable when allocation can succeed
 *  @size: the size to allocate
 *  @prior: the priority of the allocation
 *
 *  This helper function can be used by most STREAMS message handling procedures that allocate a
 *  buffer in response to an incoming message.  If the allocation fails, this routine will return
 *  NULL and issue a buffer callback to reenable the queue @q when the allocation could succeed.
 *  When NULL is returned, the caller should simply place the incoming message on the queue (i.e.
 *  with putq() from a put procedure or putbq() from a service procedure) and return.  The queue
 *  will be rescheduled with qenable() when the allocation could succeed.
 */
STATIC mblk_t *
ip_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;

	if (!(mp = allocb(size, prior)))
		ip_bufcall(q, size, prior);
	return (mp);
}

/**
 *  ip_allocb: - reliable allocb()
 *  @q: the queue to enable when allocation can succeed
 *  @base: the base of the external data buffer
 *  @size: the size of the external data buffer
 *  @prior: the priority of the allocation
 *  @frtn: a pointer to a free routine structure (containing callback and client data)
 *
 *  This helper function can be used by most STREAMS message handling procedures that allocate a
 *  message block with an external buffer in response to an incoming message.  If the allocation
 *  fails, this routine will return NULL and issue a buffer callback to reenable the queue @q when
 *  the allocation could succeed.  When NULL is returned, the caller should simply place the
 *  incoming message on the queue (i.e.  with putq() from a put procedure or putbq() from a service
 *  procedure) and return.  The queue will be rescheduled with qenable() when the allocation could
 *  succeed.
 */
STATIC mblk_t *
ip_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;

	if (!(mp = esballoc(base, size, prior, frtn)))
		ip_bufcall(q, FASTBUF, prior);
	return (mp);
}

/**
 *  ip_dupmsg: - reliable dupmsg()
 *  @q: queue to enable when duplication can succeed
 *
 *  This helper function can be used by most STREAMS message handling procedures that duplicate a an
 *  incoming message.  If the allocation fails, this routine will return NULL and issue a buffer
 *  callback to reenable the queue @q when the duplication could succeed.  When NULL is returned,
 *  the caller should simply place the incoming message on the queue (i.e.  with putq() from a put
 *  procedure or putbq() from a service procedure) and return.  The queue will be rescheduled with
 *  qenable() when the duplication could succeed.
 */
STATIC mblk_t *
ip_dupmsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (!(mp = dupmsg(bp)) && (q != NULL))
		ip_bufcall(q, FASTBUF, BPRI_MED);
	return (mp);
}

/**
 *  ip_dupb: - reliable dupb()
 *  @q: queue to enable when duplication can succeed
 *
 *  This helper function can be used by most STREAMS message handling procedures that duplicate a an
 *  incoming message block.  If the allocation fails, this routine will return NULL and issue a
 *  buffer callback to reenable the queue @q when the duplication could succeed.  When NULL is
 *  returned, the caller should simply place the incoming message on the queue (i.e.  with putq()
 *  from a put procedure or putbq() from a service procedure) and return.  The queue will be
 *  rescheduled with qenable() when the duplication could succeed.
 */
STATIC mblk_t *
ip_dupb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (!(mp = dupb(bp)) && (q != NULL))
		ip_bufcall(q, FASTBUF, BPRI_MED);
	return (mp);
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

/*
 *  M_ERROR		    - error out the stream
 *  ---------------------------------------------------------------
 */
STATIC int
n_error_reply(queue_t *q, long err)
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
	if ((mp = ip_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = (err < 0) ? -err : err;
		*(mp->b_wptr)++ = (err < 0) ? -err : err;
		putnext(ip->rq, mp);
		ip_unbind_req(ip);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  N_INFO_ACK		16 - Information Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_info_ack(queue_t *q)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	mblk_t *mp;
	N_info_ack_t *p;
	N_qos_sel_info_ip_t *qos;
	N_qos_range_info_ip_t *qor;
	size_t add_len = ip->anum * sizeof(struct sockaddr_in);
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);
	size_t pro_len = ip->pnum * sizeof(uint8_t);

	if (!(mp = ip_allocb(q, sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = 65535;
	p->ENSDU_size = 0;
	p->CDATA_size = 0;
	p->DDATA_size = 0;
	p->ADDR_size = sizeof(struct sockaddr_storage);
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
	p->QOS_range_length = qor_len;
	p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
	p->OPTIONS_flags = 0;
	p->NIDU_size = 65535;
	p->SERV_type = ip->provider ? ip->provider : (N_CONS|N_CLNS);
	p->CURRENT_state = ip_get_state(ip);
	p->PROVIDER_type = N_SNICFP;
	p->NODU_size = ip->pmtu ? : 536;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
	p->NPI_version = N_VERSION_2;
	mp->b_wptr += sizeof(*p);
	if (add_len) {
		bcopy(&ip->addr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	qos = (typeof(qos)) mp->b_wptr;
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
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/*
 *  N_BIND_ACK		17 - NS User bound to network address
 *  ---------------------------------------------------------------
 */
STATIC int
n_bind_ack(queue_t *q, ulong coninds)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	mblk_t *mp;
	N_bind_ack_t *p;
	size_t add_len = ip->anum * sizeof(struct sockaddr_in);
	size_t pro_len = ip->pnum * sizeof(uint8_t);

	if (ip_get_state(ip) != NS_WACK_BREQ)
		goto outstate;
	if (!(mp = ip_allocb(q, sizeof(*p) + add_len + pro_len, BPRI_MED)))
		goto enobufs;
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
		bcopy(&ip->addr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	if (pro_len) {
		bcopy(&ip->protoids, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	ip_set_state(ip, NS_IDLE);
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = -EFAULT;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/*
 *  N_ERROR_ACK		18 - Error Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_error_ack(queue_t *q, int prim, int err)
{
	struct ip *ip = IP_PRIV(q);
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
	if (!(mp = ip_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (N_error_ack_t *) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = (err < 0) ? NSYSERR : err;
	p->UNIX_error = (err < 0) ? -err : 0;
	mp->b_wptr += sizeof(*p);
	switch (ip_get_state(ip)) {
	case NS_WACK_OPTREQ:
	case NS_WACK_UREQ:
	case NS_WCON_CREQ:
		ip_set_state(ip, NS_IDLE);
		break;
	case NS_WCON_RREQ:
		ip_set_state(ip, NS_DATA_XFER);
		break;
	case NS_WACK_BREQ:
		ip_set_state(ip, NS_UNBND);
		break;
	case NS_WACK_CRES:
		ip_set_state(ip, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ6:
		ip_set_state(ip, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		ip_set_state(ip, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		ip_set_state(ip, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		ip_set_state(ip, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		ip_set_state(ip, NS_WRES_RIND);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send NOUTSTATE or NNOTSUPPORT or are responding to an
		   N_OPTMGMT_REQ in other than the NS_IDLE state. */
	}
	putnext(ip->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  N_OK_ACK		19 - Success Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_ok_ack(queue_t *q, np_ulong prim, np_ulong seq, np_ulong tok)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	mblk_t *mp;
	N_ok_ack_t *p;

	if (!(mp = ip_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (ip_get_state(ip)) {
	case NS_WACK_OPTREQ:
		ip_set_state(ip, NS_IDLE);
		break;
	case NS_WACK_RRES:
		ip_set_state(ip, NS_DATA_XFER);
		break;
	case NS_WACK_UREQ:
		ip_set_state(ip, NS_UNBND);
		break;
	case NS_WACK_CRES:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		swerr();
		ip_set_state(ip, NS_IDLE);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a N_OPTMGMT_REQ in other than the NS_IDLE
		   state. */
	}
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/**
 * n_conn_con: perform N_CONN_CON actions
 * @q: active queue in queue pair (write queue)
 * @dst: destination address(es)
 * @dnum: number of destination addresses
 * @qos: quality of service parameters
 *
 */
STATIC int
n_conn_con(queue_t *q, struct sockaddr_in *dst, size_t dnum, N_qos_conn_ip_t *qos)
{
	mblk_t *mp;
	N_conn_con_t *p;
	size_t qnum = dnum ? dnum : 1;

	if ((mp = ip_allocb(q, sizeof(*p) + dnum * sizeof(*dst) + qnum * sizeof(*qos)))) {
		/* FIXME: actually perform the connection */
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (N_conn_con_t *)mp->b_wptr;
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
	return (-ENOBUFS);
}

/**
 * n_reset_con: - generate a N_RESET_CON message
 * @q: active queue in queue pair (write queue)
 * @dp: message containing IP packet
 */
STATIC int
n_reset_con(queue_t *q, struct sockaddr_in *dst, np_ulong RESET_orig, np_ulong RESET_reason)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_reset_con_t *p;
	N_qos_sel_reset_t *qos;
	struct ip_daddr *da;
	size_t dnum, dlen, qlen;

	dnum = (dst == NULL || dst->sin_addr.s_addr == INADDR_ANY) ? ip->dnum : 1;
	dlen = dnum * sizeof(*dst);
	qlen = dnum * sizeof(*qos);

	if ((mp = ip_allocb(q, sizeof(*p) + dlen + qlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 0;
		p = (N_reset_con_t *) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) : 0;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + dlen : 0;
		mp->b_wptr += sizeof(*p);
		for (da = ip->daddrs; da; da = da->da_next) {
			if (dst == NULL || dst->sin_addr.s_addr == INADDR_ANY
			    || dst->sin_addr.s_addr == da->daddr) {
				struct sockaddr_in *sin;

				sin = (struct sockaddr_in *) mp->b_wptr;
				sin->sin_family = AF_INET;
				sin->sin_port = ip->proto;
				sin->sin_addr.s_addr = da->daddr;
				mp->b_wptr += sizeof(*sin);
			}
		}
		for (da = ip->daddrs; da; da = da->da_next) {
			if (dst == NULL || dst->sin_addr.s_addr == INADDR_ANY
			    || dst->sin_addr.s_addr == da->daddr) {
				N_qos_sel_reset_ip_t *qos;

				qos = (N_qos_sel_reset_ip_t *) mp->b_wptr;
				qos->ttl = da->ttl;
				qos->tos = da->tos;
				qos->mtu = da->mtu;
				mp->b_wptr += sizeof(*qos);
			}
		}
		qreply(q, mp);
		return (QR_DONE);

	}
	return (-ENOBUFS);
}

/**
 * n_conn_ind: - generate a N_CONN_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * We generate connection indications to Streams that are bound as listening to an address including
 * the destination address of the IP packet, where no connection exists for the source address of
 * the IP packet.
 */
STATIC int
n_conn_ind(queue_t *q, mblk_t *dp)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *bp;
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

	if ((bp = ip_dupmsg(q, dp))) {
		if ((mp = ip_allocb(q, size, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (N_conn_ind_t *) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->SRC_length = sizeof(*sin);
			p->SRC_offset = sizeof(*p) + sizeof(*sin);
			p->SEQ_number = (np_ulong) dp;
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
			qos->ttl = iph->ttl;
			qos->tos = iph->tos;
			qos->mtu =;
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
 * n_discon_ind: - generate a N_DISCON_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * The N_DISCON_IND is sent when we encounter an error on a connection oriented Stream, i.e. as a
 * result of receiving an ICMP error.  For multihomed hosts, we only do this if all destination
 * addresses have errors.
 */
STATIC int
n_discon_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * n_data_ind: - generate a N_DATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC int
n_data_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * n_exdata_ind: - generate a N_EXDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC int
n_exdata_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * n_unitdata_ind: - generate a N_UNITDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC int
n_unitdata_ind(queue_t *q, mblk_t *dp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	mblk_t *mp;
	N_unitdata_ind_t *p;
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct sockaddr_in *sin;

	if (canputnext(q)) {
		if ((mp = ip_allocb(q, sizeof(*p) + sizeof(*sin) + sizeof(*sin), BPRI_MED))) {
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
			dp->b_datap->db_type = M_DATA; /* just in case */
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}


/**
 * n_uderror_ind: - generate a N_UDERROR_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing ICMP packet
 *
 * There is another reason for issuing an N_UDERROR_IND and that is Explicit Congestion
 * Notification, but there is no ICMP message associated with that and it has not yet been coded:
 * probably need an n_uderror_ind_ecn() function.
 */
STATIC int
n_uderror_ind(queue_t *q, mblk_t *dp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_uderror_ind_t *p;
	mblk_t *mp;
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct icmphdr *icmp = (struct icmphdr *) (dp->b_rptr + (iph->ihl << 2));
	struct sockaddr_in *sin;

	if (canputnext(q)) {
		if ((mp = ip_allocb(q, sizeof(*p) + dest_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2; /* expedite */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field = 0;
			p->ERROR_type = error;
			switch (icmp->type) {
			case ICMP_DEST_UNREACH:
				switch (icmp->code) {
				case ICMP_NET_UNREACH:
				case ICMP_HOST_UNREACH:
				case ICMP_PROT_UNREACH:
				case ICMP_PORT_UNREACH:
					p->ERROR_type = N_UD_ROUTE_UNAVAIL;
					break;
				case ICMP_FRAG_NEEDED:
					mtu = icmp->un.frag.mtu;
					p->ERROR_type = N_UD_SEG_REQUIRED;
					break;
				case ICMP_NET_UNKNOWN:
				case ICMP_HOST_UNKNOWN:
				case ICMP_HOST_ISOLATED:
				case ICMP_NET_ANO:
				case ICMP_HOST_ANO:
				case ICMP_PKT_FILTERED:
				case ICMP_PREC_VIOLATION:
				case ICMP_PREC_CUTOFF:
					p->ERROR_type = N_UD_ROUTE_UNAVAIL;
					break;
				case ICMP_SR_FAILED:
				case ICMP_NET_UNR_TOS:
				case ICMP_HOST_UNR_TOS:
					p->ERROR_type = N_UD_QOS_UNAVAIL;
					break;
				default:
					p->ERROR_type = N_UD_UNDEFINED;
					break;
				}
				break;
			case ICMP_SOURCE_QUENCH:
				p->ERROR_type = N_UD_CONGESTION;
				break;
			case ICMP_TIME_EXCEEDED:
				switch (icmp->code) {
				case ICMP_EXC_TTL:
					p->ERROR_type = N_UD_LIFE_EXCEEDED;
					break;
				case ICMP_EXC_FRAGTIME:
					p->ERROR_type = N_UD_TD_EXCEEDED;
					break;
				default:
					p->ERROR_type = N_UD_UNDEFINED;
					break;
				}
				break;
			case ICMP_PARAMETERPROB:
				p->ERROR_type = N_UD_UNDEFINED;
				break;
			default:
				p->ERROR_type = N_UD_UNDEFINED;
				break;
			}
			mp->b_wptr += sizeof(*p);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = iph->protocol;
			sin->sin_addr.s_addr = iph->saddr;
			mp->b_wptr += sizeof(*sin);
			mp->b_cont = dp;
			dp->b_datap->db_type = M_DATA; /* just in case */
			putnext(ip->rq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

STATIC int
n_uderror_reply(queue_t *q, void *dest_ptr, size_t dest_len, long error, mblk_t *db)
{
	int err;

	switch (error) {
	case -EINVAL:
		return n_error_reply(q, error);
	case NBADADDR:
	case NBADDATA:
		break;
	case NOUTSTATE:
		return n_error_reply(q, -EPROTO);
	}
	if ((err = n_uderror_ind(q, dest_ptr, dest_len, error, db)) == QR_DONE)
		return (QR_STRIP);
	return (err);
}

/**
 * n_reset_ind: - generate a N_RESET_IND message
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
n_reset_ind(queue_t *q, mblk_t *dp)
{
	struct ip *ip = IP_PRIV(q);
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

	if ((bp = ip_dupmsg(q, dp))) {
		if ((mp = ip_allocb(q, size, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;
			p = (N_conn_ind_t *) mp->b_wptr;
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


/*
 *  ===================================================================
 *
 *  NPI User --> NPI Provider (IP) Primitives
 *
 *  ===================================================================
 */
/*
 *  N_INFO_REQ		 5 - Information request
 *  -------------------------------------------------------------------
 */
STATIC int
n_info_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);

	(void) ip;
	return n_info_ack(q);
}

/*
 *  N_BIND_REQ		 6 - Bind a NS user to network address
 *  -------------------------------------------------------------------
 *  It is possible to bind more than one address to a stream.  Each address is formatted as a
 *  sockaddr_in up to the size of sockaddr_storage (which is 8 addresses on Linux).  If the
 *  DEFAULT_DEST flag is set, then the port number is insignificant (and should be coded zero) or an
 *  address is not provided.  When DEFAULT_DEST is not specified, the port number is significant.
 *  Once bound, all packets that arrive for the specified address(es) and port number
 *  combindation(s) are delivered to the bound stream.  For DEFAULT_DEST streams, all packets that
 *  arrive for the specified address(es) that have port numbers that are bound to no other address
 *  will be delivered to the DEFAULT_DEST stream.  DEFAULT_DEST streams that are bound with no
 *  address (or to a wildcard address) will have all packets delivered that are bound to no other
 *  address or port number combination.  Protocol ids must always be specified.
 */
STATIC int
n_bind_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) != NS_UNBND)
		goto outstate;
	ip_set_state(ip, NS_WACK_BREQ);
	if (p->BIND_flags && p->BIND_flags != DEFAULT_DEST)
		goto badflag;
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto badaddr;
	if (p->PROTOID_length && (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length))
		goto badprotoid;
	if (p->PROTOID_length == 0)
		goto noprotoid;
	if (p->PROTOID_length > 1)
		goto badprotoid2;
	{
		struct sockaddr_storage sa;
		struct sockaddr_in *add_in = (typeof(add_in)) & sa;
		size_t add_len = p->ADDR_length;
		size_t anum;
		unsigned char *protoids = (typeof(protoids)) (mp->b_rptr + p->PROTOID_offset);
		size_t pnum = p->PROTOID_length;
		ulong flags = p->BIND_flags;

		if (add_len) {
			if (add_len < sizeof(struct sockaddr_in))
				goto badaddr;
			if (add_len > sizeof(struct sockaddr_storage))
				goto badaddr;
			anum = add_len / sizeof(struct sockaddr_in);
			if (add_len != anum * sizeof(struct sockaddr_in))
				goto badaddr;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, add_in, add_len);
			if (add_in->sin_family != AF_INET)
				goto badaddr;
		} else {
			anum = 1;
			add_in->sin_family = AF_INET;
			add_in->sin_port = 0;
			add_in->sin_addr.s_addr = INADDR_ANY;
		}
		if ((err = ip_bind_req(q, add_in, anum, protoids, pnum, flags)) < 0)
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
		if ((err = n_bind_ack(q, p->CONIND_number)) == 0)
			return (QR_DONE);
		ip_unbind_req(ip);
		return (err);
	}
      provspec:
	err = err;
	ptrace(("%s: %p: provider specific UNIX error %d\n", DRV_NAME, ip, -err));
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
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      error:
	return n_error_ack(q, N_BIND_REQ, err);
}

/*
 *  N_UNBIND_REQ	     7 - Unbind NS user from network address
 *  -------------------------------------------------------------------
 */
STATIC int
n_unbind_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) != NS_IDLE)
		goto outstate;
	ip_set_state(ip, NS_WACK_UREQ);
	if ((err = ip_unbind_req(ip)))
		goto error;
	return n_ok_ack(q, N_UNBIND_REQ, 0, 0);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return n_error_ack(q, N_UNBIND_REQ, err);
}

/*
 *  N_OPTMGMT_REQ	     9 - Options management request
 *  -------------------------------------------------------------------
 */
STATIC int
n_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_info_ip_t *qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) == NS_IDLE)
		ip_set_state(ip, NS_WACK_OPTREQ);
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt;
		if (qos->n_qos_type != N_QOS_SEL_INFO_IP)
			goto badqostype;
		if (p->QOS_length != sizeof(*qos))
			goto badopt2;
	}
	if (p->QOS_length) {
		if (qos->tos != -1)
			ip->tos = qos->tos;
		if (qos->ttl != -1)
			ip->ttl = qos->ttl;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		ip->i_flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		ip->i_flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	return n_ok_ack(q, N_OPTMGMT_REQ, 0, 0);
      badopt2:
	err = NBADOPT;
	ptrace(("%s: %p: QOS options were invalid\n", DRV_NAME, ip));
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: QOS structure type not supported\n", DRV_NAME, ip));
	goto error;
      badopt:
	err = NBADOPT;
	ptrace(("%s: %p: QOS options were invalid\n", DRV_NAME, ip));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return n_error_ack(q, N_OPTMGMT_REQ, err);
}

/*
 *  N_UNITDATA_REQ	    xx - Unitdata request
 *  -------------------------------------------------------------------
 *  N_UNITDATA_REQ does not have a source address field.  We use the source address to which the
 *  stream is bound.  In the event that the stream is bound as a DEFAULT_DEST with no address or
 *  bound to with DEFAULT_DEST to an address with no port number, or if the stream is bound to
 *  multiple protocol identifiers, then the protocol identifier, source address and source port
 *  number used will be the last received protocol identifier, source address or port number.  If
 *  there is no received protocol identifier, source address or port number to use, then the request
 *  will fail and a N_UDERROR_IND will be sent indicating the failure.
 */
STATIC int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	size_t mlen;
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in dest;
	size_t dlen = 0;

	if (ip->provider == N_CONS)
		goto notsupport;
	if (ip->provider != N_CLNS)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) != NS_IDLE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badaddr;
	if (!mp->b_cont)
		goto baddata;
	if ((mlen = msgdsize(mp->b_cont)) == 0 || mlen > 65535)
		goto baddata2;
	{
		size_t dlen = p->DEST_length;

		if (dlen == 0)
			goto noaddr;
		if (dlen != sizeof(struct sockaddr_in))
			goto badaddr2;
		/* avoid alignemnt problems */
		bcopy(mp->b_rptr + p->DEST_offset, &dest, dlen);
		if (dest.sin_family != AF_INET)
			goto badaddr;
		if (dest.sin_port == 0)
			goto badaddr;
		if (dest.sin_addr.s_addr == INADDR_ANY)
			goto badaddr;
		if ((err = ip_unitdata_req(q, &dest, mp->b_cont)) < 0)
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
	}
      provspec:
	err = err;
	ptrace(("%s: %p: provider specific UNIX error %d\n", DRV_NAME, ip, -err));
      access:
	err = NACCESS;
	ptrace(("%s: %p: no permission for requested address\n", DRV_NAME, ip));
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
	return n_uderror_reply(q, &dest, dlen, err, mp->b_cont);
}

/**
 * n_conn_req:- process N_CONN_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_CONN_REQ message
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
 */
STATIC int
n_conn_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_conn_req_t *p;
	N_qos_sel_conn_ip_t *qos = NULL;
	struct sockaddr_in *dst = NULL;
	size_t dnum = 0;
	np_long err = -EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (!(npi_get_statef(ip) & (NS_IDLE | NS_WRES_CIND | NS_DATA_XFER)))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;

	p = (N_conn_req_t *) mp->b_rptr;

	if (p->DEST_length != 0) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto einval;
		if (p->DEST_length % sizeof(*dst) != 0)
			goto badaddr;
		dst = (struct sockaddr_in *) (mp->b_rptr + p->DEST_offset);
		dnum = p->DEST_length / sizeof(*sin);
	}
	if (p->QOS_length != 0) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto einval;
		if (p->QOS_length != sizeof(*qos))
			goto badqos;
		qos = (N_qos_sel_conn_ip_t *) (mp->b_rptr + p->QOS_offset);
		if (qos->n_qos_type != N_QOS_SEL_CONN_IP)
			goto badqos;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	return n_conn_con(q, dst, dnum, qos);
      badqos:
	err = NBADQOSTYPE;
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
	return n_error_ack(q, N_CONN_REQ, err);
}

/**
 * n_conn_res:- process N_CONN_RES message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_CONN_RES message
 */
STATIC int
n_conn_res(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_conn_res_t *p;
	N_qos_sel_conn_ip_t *qos;
	struct sockaddr_in *res = NULL;
	size_t rnum = 0;
	np_long err = -EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (npi_get_state(ip) != NS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_conn_res_t *) mp->b_rptr;
	if (p->RES_length != 0) {
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto einval;
		if (p->RES_length % sizeof(*res) != 0)
			goto badaddr;
		res = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
		rnum = p->RES_length / sizeof(*res);
	}
	if (p->QOS_length != 0) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto einval;
		if (p->QOS_length != sizeof(*qos))
			goto badqos;
		qos = (N_qos_sel_conn_ip_t *) (mp->b_rptr + p->QOS_offset);
		if (qos->n_qos_type != N_QOS_SEL_CONN_IP)
			goto badqos;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	npi_set_state(ip, NS_WACK_CRES);
	return n_ok_ack(q, res, rnum, qos);
      badqos:
	err = NBADQOSTYPE;
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
	return n_error_ack(q, N_CONN_RES, err);
}

/**
 * n_discon_req:- process N_DISCON_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DISCON_REQ message
 */
STATIC int
n_discon_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_discon_req_t *p;
	struct sockaddr_in *res = NULL;
	size_t rnum = 0;
	np_long err = -EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (!(npi_get_statef(ip) & (NS_WRES_CIND | NS_DATA_XFER)))
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
	/* Ok, all checking done.  Now we need to disconnect the address. */
	return n_ok_ack(q, res, rnum, p->SEQ_number, p->DISCON_reason);
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
	return n_error_ack(q, N_DISCON_REQ, err);
}

/**
 * n_data_req:- process N_DATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATA_REQ message
 */
STATIC int
n_data_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_data_req_t *p;
	np_long err = EFAULT;
	mblk_t *dp;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (!(npi_get_statef(ip) & (NS_WRES_CIND | NS_DATA_XFER)))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_data_req_t *)mp->b_rptr;
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
      outstate:
	err = EPROTO;
	goto error;
      notsupport:
	err = EPROTO;
	goto error;
      error:
	return m_error(q, err);
}

/**
 * n_exdata_req:- process N_EXDATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_EXDATA_REQ message
 */
STATIC int
n_exdata_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_exdata_req_t *p;
	np_long err = EFAULT;
	mblk_t *dp;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (!(npi_get_statef(ip) & (NSF_WRES_CIND | NSF_DATA_XFER)))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (N_exdata_req_t *) mp->b_rptr;
	if (p->DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG))
		goto badflag;
	if (!(dp = mp->b_cont) || dp->b_wptr < dp->b_rptr + 1)
		goto baddata;
	/* do not currently support this primitive, even when properly formatted */
	goto notsupport;
      baddata:
	err = EPROTO;
	goto error;
      badflag:
	err = EPROTO;
	goto error;
      einval:
	err = EPROTO;
	goto error;
      outstate:
	err = EPROTO;
	goto error;
      notsupport:
	err = EPROTO;
	goto error;
      error:
	return m_error(q, err);
}

/**
 * n_datack_req:- process N_DATACK_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATACK_REQ message
 */
STATIC int
n_datack_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	np_long err = EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (!(np_get_statef(ip) & (NSF_WRES_CIND| NSF_DATA_XFER)))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(N_datack_req_t))
		goto einval;
	/* do not currently support this primitive, even when properly formatted */
	goto notsupport;
      einval:
	err = EPROTO;
	goto error;
      outstate:
	err = EPROTO;
	goto error;
      notsupport:
	err = EPROTO;
	goto error;
      error:
	return m_error(q, err);
}

/**
 * n_reset_req:- process N_RESET_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_REQ message
 */
STATIC int
n_reset_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_reset_req_t *p;
	struct sockaddr_in *dst = NULL;
	np_long err = -EFAULT;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (!(npi_get_statef(ip) & NS_DATA_XFER))
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
	return n_reset_con(q, dst, NS_USER, p->RESET_reason);
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
	return n_error_ack(q, N_RESET_REQ, err);

}

/**
 * n_reset_res:- process N_RESET_RES message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_RES message
 *
 * When a reset indication is sent to the user, we expect a reset response which is acknowledged.
 * The reason is, only one oustanding reset indication per destination per RESET_reason is issued.
 * No additional reset indication will be issued until a response is received.  Because of this,
 * reset indications are expedited (band 2).
 */
STATIC int
n_reset_res(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	N_reset_res_t *p;
	struct sockaddr_in dst;
	struct ip_daddr *da;
	mblk_t **bpp, **dpp = NULL;

	if (ip->provider == N_CLNS)
		goto notsupport;
	if (ip->provider != N_CONS)
		goto outstate;
	if (npi_get_state(ip) != NS_WRES_RIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->DEST_length == 0)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto einval;
	if (p->DEST_length != sizeof(*dst))
		goto badaddr;
	/* address might not be aligned */
	bcopy(mp->b_rptr + p->DEST_offset, &dst, p->DEST_length);
	if (dst.sin_family != AF_INET)
		goto badaddr;
	if (dst.sin_addr.s_addr == INADDR_ANY)
		goto noaddr;
	/* find pointer to pointer to oldest matching reset indication */
	for (bpp = &ip->resq; *(bpp); bpp = &(*bpp)->b_next)
		if (ip->proto == ihp->protocol && dst.sin_addr.s_addr ==
		    ((struct iphdr *) (*bpp)->b_rptr)->saddr)
			dpp = bpp;
	if (!dpp)
		goto badaddr;
	/* find pointer to corresponding destination routing information */
	for (da = ip->daddrs; da; da = da->da_next)
		if (da->daddr == dst.sin_addr.s_addr)
			break;
	if (!da)
		goto badaddr;
	/* Ok, parameters check out. */
	npi_set_state(ip, NS_WACK_RRES);
	return n_ok_ack(q, blah, blah, blah);
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
	return n_error_ack(q, N_RESET_RES, err);
}


/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/**
 * ip_w_ctl: process an M_CTL, M_PCCTL message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_CTL, M_PCCTL message to process
 *
 * M_CTL and M_PCCTL are special messages for the NPI IP driver.  These are M_PROTO, M_PCPROTO
 * messages (N_UNITDATA_IND) messages that have been delivered to the upper layer, but, after
 * address processing are found to not be destined for the upper layer.  The upper layer returns
 * unwanteded N_UNITDATA_IND messages in M_CTL messages back downstream for further processing by
 * the IP layer.  We take these messages, extract the original socket buffer (with esballoc magic)
 * and pass them along to any other registered users of the IP protocol.
 */
STATIC int
ip_w_ctl(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	np_long prim;
	mblk_t *dp;
	dblk_t *db;
	struct sk_buff *skb;
	int rtn;

	/* sanity checks */
	if (ip->provider != N_CLNS)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(np_long))
		goto discard;
	if ((prim = *((np_long *)) mp->b_rptr) != N_UNITDATA_IND)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(N_unitdata_ind_t))
		goto discard;
	if ((dp = mp->b_cont) == NULL)
		goto discard;
	if (dp->b_wptr < dp->b_rptr + sizeof(struct iphdr))
		goto discard;
	if ((db = dp->b_datap)->db_ref > 1)
		goto discard;
	/* esballoc magic: steal the socket buffer from the message if it can be passed to another
	   handler.  Note that this is the same for Linux Fast-STREAMS, LiS and SVR4 compatibile
	   dblk_t structures. */
	skb = db->db_frtnp->free_arg;
	rtn = inet_pass_next_handler(&skb);
	if (rtn == 0 && skb == NULL)
		db->db_frtnp->free_arg = NULL;
	return (QR_DONE);
      discard:
	return (-EPROTO);
}

/**
 * ip_w_proto: process an M_PROTO, M_PCPROTO message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_PROTO, M_PCPROTO message to process
 *
 * These are normal N-primitives written from the upper layer protocol.
 */
STATIC int
ip_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct ip *ip = IP_PRIV(q);
	np_long oldstate = ip_get_state(ip);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *((np_long *) mp->b_rptr))) {
		case N_UNITDATA_REQ:
			rtn = n_unitdata_req(q, mp);
			break;
		case N_DATA_REQ:
			rtn = n_data_req(q, mp);
			break;
		case N_CONN_REQ:
			rtn = n_conn_req(q, mp);
			break;
		case N_CONN_RES:
			rtn = n_conn_res(q, mp);
			break;
		case N_DISCON_REQ:
			rtn = n_discon_req(q, mp);
			break;
		case N_EXDATA_REQ:
			rtn = n_exdata_req(q, mp);
			break;
		case N_DATACK_REQ:
			rtn = n_datack_req(q, mp);
			break;
		case N_RESET_REQ:
			rtn = n_reset_req(q, mp);
			break;
		case N_RESET_RES:
			rtn = n_reset_res(q, mp);
			break;
		case N_INFO_REQ:
			rtn = n_info_req(q, mp);
			break;
		case N_BIND_REQ:
			rtn = n_bind_req(q, mp);
			break;
		case N_UNBIND_REQ:
			rtn = n_unbind_req(q, mp);
			break;
		case N_OPTMGMT_REQ:
			rtn = n_optmgmt_req(q, mp);
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
		ip_set_state(ip, oldstate);
		/* The put and srv procedures do not recognize all errors.  Sometimes we return an
		   error to here just to restore the previous state.  */
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EOPNOTSUPP:
			return n_error_ack(q, prim, rtn);
		case -EPROTO:
			return n_error_reply(q, -EPROTO);
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
		ip->cminor);
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
		ip->cminor);
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
STATIC int
ip_r_data(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int rtn;

	switch (ip->provider) {
	case N_CLNS:
		rtn = n_unitdata_ind(q, mp);
		break;
	case N_CONS:
		rtn = n_data_ind(q, mp);
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
STATIC int
ip_r_error(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int rtn;

	switch (ip->provider) {
	case N_CLNS:
		rtn = n_uderror_ind(q, mp);
		break;
	case N_CONS:
		rtn = n_reset_ind(q, mp);
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

/**
 * ip_w_flush: - M_FLUSH handling
 * @q: active queue in queue pair (write queue)
 * @mp: the M_FLUSH message
 *
 * Canonical M_FLUSH handling for driver write queue.  Flush write queue if requested, cancel
 * flushing in write direction.  Flush read queue if requested and reply.  If read side flush not
 * requested, discard message.
 */
STATIC int
ip_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		queue_t *rq = RD(q);

		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(rq, mp->b_rptr[1], FLUSHALL);
		else
			flushq(rq, FLUSHALL);
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
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
STATIC INLINE int
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
STATIC INLINE int
ip_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ip_w_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		return ip_w_ctl(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ip_w_proto(q, mp);
	case M_FLUSH:
		return ip_w_flush(q, mp);
	default:
		return ip_w_other(q, mp);
	}
}

/*
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
ip_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0, locked;

	ensure(mp, return (-EFAULT));
	ensure(q, freemsg(mp);
	       return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL && (q->q_first || q->q_flag & QSVCBUSY)) {
		if (!putq(q, mp))
			freemsg(mp);
		return (0);
	}
	if ((locked = ip_trylockq(q)) || mp->b_datap->db_type == M_FLUSH) {
		do {
			/* Fast Path */
			if ((rtn = (*proc) (q, mp)) == QR_DONE) {
				freemsg(mp);
				break;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				break;
			case QR_STRIP:
				if (mp->b_cont)
					if (!putq(q, mp->b_cont))
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
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				break;
			case QR_DISABLE:
				if (!putq(q, mp))
					freemsg(mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					break;
				}
			case -ENOBUFS:
			case -EBUSY:
			case -ENOMEM:
			case -EAGAIN:
			case QR_RETRY:
				if (!putq(q, mp))
					freemsg(mp);
				break;
			}
		} while (0);
		if (locked)
			ip_unlockq(q);
	} else {
		seldom();
		if (!putq(q, mp))
			freemsg(mp);
	}
	return (rtn);
}

/*
 *  SRVQ Service Routine
 *  -----------------------------------
 */
STATIC INLINE int
ip_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *), void (*procwake) (queue_t *))
{
	int rtn = 0;

	ensure(q, return (-EFAULT));
	if (ip_trylockq(q)) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			/* Fast Path */
			if ((rtn = (*proc) (q, mp)) == QR_DONE) {
				freemsg(mp);
				continue;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_STRIP:
				if (mp->b_cont)
					if (!putbq(q, mp->b_cont))
						freemsg(mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				pswerr(("%s: ERROR: (q dropping) %d\n", DRV_NAME, rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				pswerr(("%s: ERROR: (q disabling)\n", DRV_NAME));
				noenable(q);
				if (!putbq(q, mp))
					freemsg(mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must schedule bufcall */
			case -EBUSY:	/* proc must fail canput */
			case -ENOMEM:	/* proc must schedule re-enable */
			case -EAGAIN:	/* proc must schedule re-enable */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("%s: ERROR: (q stalled) %d\n", DRV_NAME, rtn));
					if (!putbq(q, mp))
						freemsg(mp);
					break;
				}
				/* Be careful not to put a priority message back on the queue. */
				switch (mp->b_datap->db_type) {
				case M_PCPROTO:
					mp->b_datap->db_type = M_PROTO;
					break;
				case M_PCRSE:
					mp->b_datap->db_type = M_RSE;
					break;
				case M_READ:
					mp->b_datap->db_type = M_CTL;
					break;
				default:
					ptrace(("%s: ERROR: (q dropping) %d\n", DRV_NAME, rtn));
					freemsg(mp);
					continue;
				}
				mp->b_band = 255;
				if (!putq(q, mp))
					freemsg(mp);
				break;
			}
			break;
		}
		if (procwake != NULL)
			procwake(q);
		ip_unlockq(q);
	}
	return (rtn);
}

STATIC int
ip_rput(queue_t *q, mblk_t *mp)
{
	return (int) ip_putq(q, mp, &ip_r_prim);
}

STATIC int
ip_rsrv(queue_t *q)
{
	return (int) ip_srvq(q, &ip_r_prim, NULL);
}

STATIC int
ip_wput(queue_t *q, mblk_t *mp)
{
	return (int) ip_putq(q, mp, &ip_w_prim);
}

STATIC int
ip_wsrv(queue_t *q)
{
	return (int) ip_srvq(q, &ip_w_prim, NULL);
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
STATIC struct ip_hash_bucket ip_bind_hash[1] = { };
STATIC int
ip_bind_hashfn(short snum)
{
	swerr();
	fixme(("write this function"));
	return (0);
}
STATIC struct sockaddr_in *
ip_find_saddr(struct ip *ip, uint32_t daddr)
{
	swerr();
	fixme(("write this function"));
	return (NULL);
}

/**
 *  ip_lookup -	lookup Stream by protocol, address and port.
 *  @proto:	IP protocol number
 *  @daddr:	IP address
 *  @dport:	IP port
 */
STATIC struct ip *
ip_lookup(uint8_t proto, uint32_t daddr, uint16_t dport)
{
	struct ip *result = NULL;
	int snum = ntohs(dport);
	struct ip_bind_bucket *ib;
	struct ip_hash_bucket *hp = &ip_bind_hash[ip_bind_hashfn(snum)];

	read_lock(&hp->lock);
	for (ib = hp->list; ib && (ib->proto != proto || ib->port != snum); ib = ib->next) ;
	if (ib) {
		struct ip *ip;
		int hiscore = 0;

		for (ip = ib->owners; ip; ip = ip->bnext) {
			int score = 0;

			if (ip_get_state(ip) == NS_UNBND)
				continue;
			if (!(ip->userlocks & IP_BINDPORT_LOCK))
				continue;
			if (ip->sport) {
				if (ip->sport != dport)
					continue;
				score++;
				if (ip->anum) {
					struct sockaddr_in *sa;

					if (!(sa = ip_find_saddr(ip, daddr)))
						continue;
					score++;
				}
			}
			if (score > 0) {
				result = ip;
				break;
			}
			if (score > hiscore) {
				hiscore = score;
				result = ip;
			}
		}
	}
	read_unlock(&hp->lock);
	usual(result);
	if (result) {
		ip_hold(result);
		return (result);
	}
	return NULL;
}

STATIC void
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

	read_lock(&ip_lock);	/* lock hashes and stream lists */
	{
		struct ip *ip;

		if ((ip = ip_lookup(skb->nh.iph))) {
			if (ip->rq && canput(ip->rq)) {
				mblk_t *mp;
				frtn_t fr = { &ip_free, (char *) skb };
				size_t plen = skb->len + (skb->data - skb->nh.raw);

				/* FIXME: handle non-linear sk_buffs */
				if ((mp = esballoc(skb->nh.raw, plen, BPRI_MED, &fr))) {
					mp->b_datap->db_type = M_DATA;
					mp->b_wptr += plen;
					put(ip->rq, mp);
					skb = NULL;
				}
			}
		} else {
			rtn = inet_pass_next_handler(&skb);
			/* Need to generate ICMP error? */
		}
	}
	read_unlock(&ip_lock);
	if (skb)
		kfree_skb(skb);
	return (rtn);
}

/**
 * ip_v4_err: - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information
 */
STATIC void
ip_v4_err(struct sk_buff *skb, u32 info)
{
	read_lock(&ip_lock);
	{
		struct ip *ip;

		if ((ip = ip_lookup_icmp(skb->nh.iph))) {
			if (ip->rq && canput(ip->rq)) {
				mblk_t *mp;
				size_t plen = skb->len + (skb->data - skb->nh.raw);

				if ((mp = allocb(plen, BPRI_MED))) {
					mp->b_datap->db_type == M_ERROR;
					bcopy(skb->nh.raw, mp->b_wptr, plen);
					mp->b_wptr += plen;
					put(ip->rq, mp);
				}
			}
		} else {
			inet_pass_next_err_handler(skb, info);
		}
	}
	read_unlock(&ip_lock);
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
	}
	if (ip_bind_cachep != NULL) {
		if (kmem_cache_destroy(ip_bind_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ip_bind_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed ip_bind_cachep\n", DRV_NAME));
	}
	if (ip_priv_cachep != NULL) {
		if (kmem_cache_destroy(ip_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ip_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		printd(("%s: destroyed ip_priv_cachep\n", DRV_NAME));
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
/**
 * ip_alloc_priv: - allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @slp: pointer to place in list for insertion
 * @type: type of provider: 0, N_CONS, N_CLTS
 * @cmajor: major character device number
 * @cminor: minor character device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initializes it to appropriate values, and then inserts it into
 * the private structure list.
 */
STATIC ip_t *
ip_alloc_priv(queue_t *q, ip_t ** slp, int type, major_t cmajor, minor_t cminor, cred_t *crp)
{
	ip_t *ip;

	if ((ip = ip_get())) {
		ip->provider = type;
		ip->cmajor = cmajor;
		ip->cminor = cminor;
		ip_hold(ip);
		ip->rq = RD(q);
		ip->rq->q_ptr = ip;
		ip_hold(ip);
		ip->wq = WR(q);
		ip->wq->q_ptr = ip;
		ip->cred = *crp;
		spin_lock_init(&ip->qlock);
		ip->rwait = NULL;
		ip->wwait = NULL;
		ip->users = 0;
		ip_set_state(ip, NS_UNBND);
		ip_hold(ip);
		if ((ip->next = *slp))
			ip->next->prev = &ip->next;
		ip->prev = slp;
		*slp = ip;
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
	ip_unbufcall(q);
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME,
		atomic_read(&ip->refcnt)));
	spin_lock_bh(&ip_lock);
	if ((*ip->prev = ip->next))
		ip->next->prev = ip->prev;
	ip->next = NULL;
	ip->prev = NULL;
	spin_unlock_bh(&ip_lock);
	ip_put(ip);
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&ip->refcnt)));
	ip->rq->q_ptr = NULL;
	ip->rq = NULL;
	ip_put(ip);
	ip->wq->q_ptr = NULL;
	ip->wq = NULL;
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
 * ip_open: - NPI IP driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC int
ip_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	ip_t *ip, **ipp = &ip_opens;

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
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR) {
		return (ENXIO);
	}
	type = cminor;
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = FREE_CMINOR;
	spin_lock_bh(&ip_lock);
	for (; *ipp; ipp = &(*ipp)->next) {
		if (cmajor != (*ipp)->cmajor)
			break;
		if (cmajor == (*ipp)->cmajor) {
			if (cminor < (*ipp)->cminor)
				break;
			if (cminor == (*ipp)->cminor) {
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
		spin_unlock_bh(&ip_lock);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(ip = ip_alloc_priv(q, ipp, type, cmajor, cminor, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		spin_unlock_bh(&ip_lock);
		return (ENOMEM);
	}
	spin_unlock_bh(&ip_lock);
	qprocson(q);
	return (0);
}

/**
 * ip_close: - NPI IP driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
STATIC int
ip_close(queue_t *q, int oflag, cred_t *crp)
{
	ip_t *ip = PRIV(q);

	(void) oflag;
	(void) crp;
	(void) ip;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, ip->cmajor, ip->cminor));
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
	.n_flag = D_CLONE, /* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode ip_node_ipco = {
	.n_name = "ipco",
	.n_flag = D_CLONE, /* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode ip_node_ipcl = {
	.n_name = "ipcl",
	.n_flag = D_CLONE, /* clone minor */
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
	return;
}

MODULE_STATIC int __init
ipinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
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
	(void) ip_unbind_prot;
	(void) ip_esballoc;
	(void) ip_dupmsg;
	(void) ip_dupb;
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ipinit);
module_exit(ipterminate);

#endif				/* LINUX */
