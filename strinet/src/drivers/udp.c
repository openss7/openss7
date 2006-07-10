/*****************************************************************************

 @(#) $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.40 $) $Date: 2006/07/08 09:37:52 $

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

 Last Modified $Date: 2006/07/08 09:37:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: udp.c,v $
 Revision 0.9.2.40  2006/07/08 09:37:52  brian
 - handle old SLES 9 2.6.5 kernel (untested)

 Revision 0.9.2.39  2006/07/07 21:14:59  brian
 - correct compile back to RH 7.2

 Revision 0.9.2.38  2006/07/02 12:19:52  brian
 - changes for 2.6.17 kernel

 Revision 0.9.2.37  2006/07/01 12:09:54  brian
 - gets and puts cannot be fastcalled

 Revision 0.9.2.36  2006/06/22 13:11:45  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.35  2006/06/22 04:47:52  brian
 - corrected bug in optimization

 Revision 0.9.2.34  2006/06/22 01:17:18  brian
 - syncing notebook, latest changes are not stable yet

 Revision 0.9.2.33  2006/06/19 20:51:29  brian
 - more optimizations

 Revision 0.9.2.32  2006/06/18 20:54:13  brian
 - minor optimizations from profiling

 Revision 0.9.2.31  2006/06/16 10:48:04  brian
 - recent updates

 Revision 0.9.2.30  2006/06/16 08:01:40  brian
 - found noxious buffer overflow

 Revision 0.9.2.29  2006/06/14 10:37:44  brian
 - defeat a lot of debug traces in debug mode for testing
 - changes to allow strinet to compile under LiS (why???)

 Revision 0.9.2.28  2006/06/06 06:26:47  brian
 - second gen UDP driver working well now

 Revision 0.9.2.27  2006/06/05 02:53:40  brian
 - working up udp zero-copy

 Revision 0.9.2.26  2006/05/31 10:27:40  brian
 - working up zero-copy

 Revision 0.9.2.25  2006/05/29 08:53:07  brian
 - started zero copy architecture

 Revision 0.9.2.24  2006/05/25 08:39:11  brian
 - added noinline in strategic places

 Revision 0.9.2.23  2006/05/24 10:50:39  brian
 - optimizations

 Revision 0.9.2.22  2006/05/23 10:40:22  brian
 - handle non-exported sysctl_ip_default_ttl on receive FC4 kernels

 Revision 0.9.2.21  2006/05/22 02:09:09  brian
 - changes from performance testing

 Revision 0.9.2.20  2006/05/19 23:15:35  brian
 - corrections from testing

 Revision 0.9.2.19  2006/05/19 12:29:07  brian
 - results of testing, almost full pass

 Revision 0.9.2.18  2006/05/19 08:49:44  brian
 - working up RAWIP and UDP drivers and testing

 Revision 0.9.2.17  2006/05/18 21:01:18  brian
 - synced UDP driver to RAWIP driver

 Revision 0.9.2.16  2006/05/14 08:34:30  brian
 - changes for compile and load

 Revision 0.9.2.15  2006/05/08 11:26:05  brian
 - post inc problem and working through test cases

 Revision 0.9.2.14  2006/05/07 22:12:49  brian
 - updated for NPI-IP driver

 Revision 0.9.2.13  2006/05/03 11:53:51  brian
 - changes for compile, working up NPI-IP driver

 Revision 0.9.2.12  2006/04/24 09:25:10  brian
 - working up RAWIP and UDP drivers

 Revision 0.9.2.11  2006/04/23 18:13:22  brian
 - changes for compile

 Revision 0.9.2.10  2006/04/22 10:51:04  brian
 - working up UDP and RAWIP drivers

 Revision 0.9.2.9  2006/04/22 01:08:52  brian
 - working up second generation UPD driver

 Revision 0.9.2.8  2006/04/18 17:55:42  brian
 - raiontalizing rawip and udp drivers

 Revision 0.9.2.7  2006/04/03 10:57:25  brian
 - need attributes on definition as well as declaration

 Revision 0.9.2.6  2006/03/27 01:25:36  brian
 - working up IP driver and SCTP testing

 Revision 0.9.2.5  2006/03/25 10:20:09  brian
 - corrected some compile problems

 Revision 0.9.2.4  2006/03/24 16:03:26  brian
 - changes for x86_64 2.6.15 compile, working up udp

 Revision 0.9.2.3  2006/03/24 04:58:00  brian
 - first compile pass

 Revision 0.9.2.2  2006/03/24 00:08:29  brian
 - added second generation udp driver

 Revision 0.9.2.1  2006/03/20 12:16:23  brian
 - new approach to UDP

 *****************************************************************************/

#ident "@(#) $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.40 $) $Date: 2006/07/08 09:37:52 $"

static char const ident[] =
    "$RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.40 $) $Date: 2006/07/08 09:37:52 $";

/*
 *  This driver provides a somewhat different approach to UDP that the inet
 *  driver.  It hooks directly to the inet_protos list in the Linux kernel and
 *  intercepts UDP packets ahead of Linux native UDP.  UDP is then processed
 *  per TPI specifications.  If no Stream is bound, the received sk_buff is
 *  passed on to Linux native UDP.
 *
 *  One of the reasons for taking this approach instead of the Streams over
 *  Sockets approach of the inet.c driver is that UDP is easy enough to
 *  implement, and this driver will provide a reasonable comparison between
 *  the efficiency and performance of the Streams and the Socket frameworks.
 *
 *  There are three ways that this module cn operate: 1) The module can be
 *  opened as a pseudo-device driver (DRVOPEN|CLONEOPEN) of its own right.  2)
 *  The module can be pushed over another Stream.  That Stream needs to be a
 *  freshly opened (unbound) NPI IP driver Stream.  An NPI IP Stream can be
 *  linked underneath the driver as a multiplexing driver.  The reason for all
 *  these approaches is to determine the relative merits (performance and
 *  otherwise) of each approach.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define tp_tst_bit(nr,addr)	    test_bit(nr,addr)
#define tp_set_bit(nr,addr)	    __set_bit(nr,addr)
#define tp_clr_bit(nr,addr)	    __clear_bit(nr,addr)

#include <linux/interrupt.h>

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

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

#include "udp_hooks.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_inet.h>

#define T_ALLLEVELS -1

#define UDP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define UDP_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define UDP_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define UDP_REVISION	"OpenSS7 $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.40 $) $Date: 2006/07/08 09:37:52 $"
#define UDP_DEVICE	"SVR 4.2 STREAMS UDP Driver"
#define UDP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define UDP_LICENSE	"GPL"
#define UDP_BANNER	UDP_DESCRIP	"\n" \
			UDP_EXTRA	"\n" \
			UDP_REVISION	"\n" \
			UDP_COPYRIGHT	"\n" \
			UDP_DEVICE	"\n" \
			UDP_CONTACT	"\n"
#define UDP_SPLASH	UDP_DESCRIP	" - " \
			UDP_REVISION

#ifdef LINUX
MODULE_AUTHOR(UDP_CONTACT);
MODULE_DESCRIPTION(UDP_DESCRIP);
MODULE_SUPPORTED_DEVICE(UDP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(UDP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-udp");
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
MODULE_ALIAS("streams-driver-udp2");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_UDP_MAJOR));
MODULE_ALIAS("/dev/streams/udp2");
MODULE_ALIAS("/dev/streams/udp2/*");
MODULE_ALIAS("/dev/streams/clone/udp2");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/udp2");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  ==========================================================================
 *
 *  STREAMS Definitions
 *
 *  ==========================================================================
 */

#define DRV_ID		UDP_DRV_ID
#define DRV_NAME	UDP_DRV_NAME
#define CMAJORS		UDP_CMAJORS
#define CMAJOR_0	UDP_CMAJOR_0
#define UNITS		UDP_UNITS
#ifdef MODULE
#define DRV_BANNER	UDP_BANNER
#else				/* MODULE */
#define DRV_BANNER	UDP_SPLASH
#endif				/* MODULE */

STATIC struct module_info udp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = (1 << 18),		/* Hi water mark */
	.mi_lowat = (1 << 17),		/* Lo water mark */
};

STATIC struct module_stat udp_mstat = {
};

/* Upper multiplex is a T provider following the TPI. */

STATIC streamscall int udp_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int udp_qclose(queue_t *, int, cred_t *);

STATIC struct qinit udp_rinit = {
	.qi_putp = ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = ss7_osrv,		/* Read service procedure */
	.qi_qopen = udp_qopen,		/* Each open */
	.qi_qclose = udp_qclose,	/* Last close */
	.qi_minfo = &udp_minfo,		/* Module information */
	.qi_mstat = &udp_mstat,		/* Module statistics */
};

STATIC struct qinit udp_winit = {
	.qi_putp = ss7_iput,		/* Write put procedure (message from above) */
	.qi_srvp = ss7_isrv,		/* Write service procedure */
	.qi_minfo = &udp_minfo,		/* Module information */
	.qi_mstat = &udp_mstat,		/* Module statistics */
};

MODULE_STATIC struct streamtab udp_info = {
	.st_rdinit = &udp_rinit,	/* Upper read queue */
	.st_wrinit = &udp_winit,	/* Upper write queue */
};

/*
 *  Primary data structures.
 */

struct tp_bhash_bucket;
struct tp_chash_bucket;

struct tp_daddr {
	uint32_t addr;			/* IP address this destination */
	unsigned char ttl;		/* time to live, this destination */
	unsigned char tos;		/* type of service, this destination */
	unsigned short mtu;		/* maximum transfer unit this destination */
	struct dst_entry *dst;		/* route for this destination */
};

struct tp_saddr {
	uint32_t addr;			/* IP address this source */
};

struct tp_baddr {
	uint32_t addr;			/* IP address this bind */
};

typedef struct tp_options {
#if 0
	unsigned char flags[12];	/* twelve bytes of flags for 96 flags */
#else
	unsigned long flags[3];		/* at least 96 flags */
#endif
	struct {
		t_uscalar_t debug[4];	/* XTI_DEBUG */
		struct t_linger linger;	/* XTI_LINGER */
		t_uscalar_t rcvbuf;	/* XTI_RCVBUF */
		t_uscalar_t rcvlowat;	/* XTI_RCVLOWAT */
		t_uscalar_t sndbuf;	/* XTI_SNDBUF */
		t_uscalar_t sndlowat;	/* XTI_SNDLOWAT */
		t_uscalar_t priority;	/* XTI_PRIORITY */
	} xti;
	struct {
		unsigned char protocol;	/* T_IP_PROTOCOL */
		unsigned char options[40];	/* T_IP_OPTIONS */
		unsigned char ttl;	/* T_IP_TTL */
		unsigned char tos;	/* T_IP_TOS */
		unsigned int reuseaddr;	/* T_IP_REUSEADDR */
		unsigned int dontroute;	/* T_IP_DONTROUTE */
		unsigned int broadcast;	/* T_IP_BROADCAST */
		uint32_t addr;		/* T_IP_ADDR */
		uint32_t saddr;		/* T_IP_SADDR */
		uint32_t daddr;		/* T_IP_DADDR */
		uint32_t mtu;		/* T_IP_MTU */
	} ip;
	struct {
		t_uscalar_t checksum;	/* T_UDP_CHECKSUM */
	} udp;
} tp_options_t;

/* Private structure */
typedef struct tp {
	STR_DECLARATION (struct tp);	/* Stream declaration */
	struct tp *bnext;		/* linkage for bind/list hash */
	struct tp **bprev;		/* linkage for bind/list hash */
	struct tp_bhash_bucket *bhash;	/* linkage for bind/list hash */
	struct tp *cnext;		/* linkage for conn hash */
	struct tp **cprev;		/* linkage for conn hash */
	struct tp_chash_bucket *chash;	/* linkage for conn hash */
	struct T_info_ack info;		/* service provider information */
	unsigned int sndblk;		/* sending blocked */
	unsigned int sndmem;		/* send buffer memory allocated */
	unsigned int rcvmem;		/* recv buffer memory allocated */
	unsigned int BIND_flags;	/* bind flags */
	unsigned int CONN_flags;	/* connect flags */
	unsigned int CONIND_number;	/* maximum number of outstanding connection indications */
	bufq_t conq;			/* queue outstanding connection indications */
	unsigned short pnum;		/* number of bound protocol ids */
	uint8_t protoids[16];		/* bound protocol ids */
	unsigned short bnum;		/* number of bound addresses */
	unsigned short bport;		/* bound port number (network order) */
	struct tp_baddr baddrs[8];	/* bound addresses */
	unsigned short snum;		/* number of source (connected) addresses */
	unsigned short sport;		/* source (connected) port number (network order) */
	struct tp_saddr saddrs[8];	/* source (connected) addresses */
	unsigned short dnum;		/* number of destination (connected) addresses */
	unsigned short dport;		/* destination (connected) port number (network order) */
	struct tp_daddr daddrs[8];	/* destination (connected) addresses */
	struct tp_options options;	/* protocol options */
	unsigned char _pad[40];		/* pad for ip options */
} tp_t;

#define PRIV(__q) (((__q)->q_ptr))
#define TP_PRIV(__q) ((struct tp *)((__q)->q_ptr))

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (tp, tp);		/* master list of tp (open) structures */
} df_t;

static struct df master = {.lock = RW_LOCK_UNLOCKED, };

#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		(SK_RMEM_MAX << 1)  /* needs to be sysctl_rmem_default */
#define xti_default_rcvlowat		1
#define xti_default_sndbuf		(SK_WMEM_MAX << 1)  /* needs to be sysctl_wmem_default */
#define xti_default_sndlowat		SK_WMEM_MAX /* needs to be sysctl_wmem_default >> 1 */
#define xti_default_priority		0

#define ip_default_protocol		17
#define ip_default_options		{ 0, }
#define ip_default_tos			0
#define ip_default_ttl			64
#define ip_default_reuseaddr		T_NO
#define ip_default_dontroute		T_NO
#define ip_default_broadcast		T_NO
#define ip_default_addr			INADDR_ANY
#define ip_default_saddr		INADDR_ANY
#define ip_default_daddr		INADDR_ANY
#define ip_default_mtu			536

#define udp_default_checksum		T_YES

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_XTI_PRIORITY,
	_T_BIT_IP_PROTOCOL,
	_T_BIT_IP_OPTIONS,
	_T_BIT_IP_TOS,
	_T_BIT_IP_TTL,
	_T_BIT_IP_REUSEADDR,
	_T_BIT_IP_DONTROUTE,
	_T_BIT_IP_BROADCAST,
	_T_BIT_IP_ADDR,
	_T_BIT_IP_RETOPTS,
	_T_BIT_IP_SADDR,
	_T_BIT_IP_DADDR,
	_T_BIT_IP_MTU,
	_T_BIT_UDP_CHECKSUM,
};

#define t_tst_bit(nr,addr) tp_tst_bit(nr,addr)
#define t_set_bit(nr,addr) tp_set_bit(nr,addr)
#define t_clr_bit(nr,addr) tp_clr_bit(nr,addr)

/*
 *  Bind buckets, caches and hashes.
 */
struct tp_bind_bucket {
	struct tp_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct tp_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short port;		/* port number (host order) */
	struct tp *owners;		/* list of owners of this protocol/port combination */
	struct tp *dflt;		/* default listeners/destinations for this protocol */
};
struct tp_conn_bucket {
	struct tp_conn_bucket *next;	/* linkage of conn buckets for hash slot */
	struct tp_conn_bucket **prev;	/* linkage of conn buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short sport;		/* source port number (network order) */
	unsigned short dport;		/* destination port number (network order) */
	struct tp *owners;		/* list of owners of this protocol/sport/dport combination */
};

struct tp_bhash_bucket {
	rwlock_t lock;
	struct tp *list;
};
struct tp_chash_bucket {
	rwlock_t lock;
	struct tp *list;
};

STATIC struct tp_bhash_bucket *udp_bhash;
STATIC size_t udp_bhash_size = 0;
STATIC size_t udp_bhash_order = 0;

STATIC struct tp_chash_bucket *udp_chash;
STATIC size_t udp_chash_size = 0;
STATIC size_t udp_chash_order = 0;

STATIC INLINE streams_fastcall __hot_in int
udp_bhashfn(unsigned char proto, unsigned short bport)
{
	return ((udp_bhash_size - 1) & (proto + bport));
}

STATIC INLINE streams_fastcall __unlikely int
udp_chashfn(unsigned char proto, unsigned short sport, unsigned short dport)
{
	return ((udp_chash_size - 1) & (proto + sport + dport));
}

#ifdef LINUX
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
struct inet_protocol {
	struct net_protocol proto;
	struct net_protocol *next;
	struct module *kmod;
};
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#endif				/* LINUX */

struct tp_prot_bucket {
	unsigned char proto;		/* protocol number */
	int refs;			/* reference count */
	int corefs;			/* T_COTS(_ORD) references */
	int clrefs;			/* T_CLTS references */
	struct inet_protocol prot;	/* Linux registration structure */
};
STATIC rwlock_t udp_prot_lock = RW_LOCK_UNLOCKED;
STATIC struct tp_prot_bucket *udp_prots[256];

STATIC kmem_cache_t *udp_prot_cachep;
STATIC kmem_cache_t *udp_priv_cachep;

static INLINE __unlikely struct tp *
tp_get(struct tp *tp)
{
	dassert(tp != NULL);
	atomic_inc(&tp->refcnt);
	return (tp);
}
static INLINE __hot void
tp_put(struct tp *tp)
{
	dassert(tp != NULL);
	if (atomic_dec_and_test(&tp->refcnt)) {
		kmem_cache_free(udp_priv_cachep, tp);
	}
}
static INLINE streams_fastcall __hot void
tp_release(struct tp **tpp)
{
	struct tp *tp;
	dassert(tpp != NULL);
	if (likely((tp = XCHG(tpp, NULL)) != NULL))
		tp_put(tp);
}
static INLINE __unlikely struct tp *
tp_alloc(void)
{
	struct tp *tp;

	if ((tp = kmem_cache_alloc(udp_priv_cachep, SLAB_ATOMIC))) {
		bzero(tp, sizeof(*tp));
		atomic_set(&tp->refcnt, 1);
		spin_lock_init(&tp->lock);	/* "tp-lock" */
		tp->priv_put = &tp_put;
		tp->priv_get = &tp_get;
		// tp->type = 0;
		// tp->id = 0;
		// tp->state = 0;
		// tp->flags = 0;
	}
	return (tp);
}

/*
 *  State changes.
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
udp_state_name(t_scalar_t state)
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

STATIC INLINE streams_fastcall __unlikely void
tp_set_state(struct tp *tp, const t_uscalar_t state)
{
	_printd(("%s: %p: %s <- %s\n", DRV_NAME, tp, udp_state_name(state),
		 udp_state_name(tp->info.CURRENT_state)));
	tp->info.CURRENT_state = state;
}

STATIC INLINE streams_fastcall __hot t_uscalar_t
tp_get_state(const struct tp *tp)
{
	return (tp->info.CURRENT_state);
}

STATIC INLINE streams_fastcall __unlikely t_uscalar_t
tp_chk_state(const struct tp *tp, const t_uscalar_t mask)
{
	return (((1 << tp->info.CURRENT_state) & (mask)) != 0);
}

STATIC INLINE streams_fastcall __unlikely t_uscalar_t
tp_not_state(const struct tp *tp, const t_uscalar_t mask)
{
	return (((1 << tp->info.CURRENT_state) & (mask)) == 0);
}

STATIC INLINE streams_fastcall __unlikely long
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
	.ip = {
	       .protocol = ip_default_protocol,
	       .options = ip_default_options,
	       .tos = ip_default_tos,
	       .ttl = ip_default_ttl,
	       .reuseaddr = ip_default_reuseaddr,
	       .dontroute = ip_default_dontroute,
	       .broadcast = ip_default_broadcast,
	       .addr = ip_default_addr,
	       .saddr = ip_default_saddr,
	       .daddr = ip_default_daddr,
	       .mtu = ip_default_mtu,
	       },
	.udp = {
		.checksum = udp_default_checksum,
		}
};

#define t_defaults tp_defaults

STATIC noinline streams_fastcall __unlikely int
t_opts_size_ud_slow(const struct tp *tp, const mblk_t *mp)
{
	int size = 0;
	struct iphdr *iph;

	/* only need to deliver up destination address info if the stream is multihomed (i.e.
	   wildcard bound) */
	iph = (struct iphdr *) mp->b_datap->db_base;
	size += _T_SPACE_SIZEOF(t_defaults.ip.addr);	/* T_IP_ADDR */
	return (size);
}

/**
 * t_opts_size_ud - size options from received message for unitdata
 * @t: private structure
 * @mp: message pointer for message
 */
STATIC INLINE streams_fastcall __hot_in int
t_opts_size_ud(const struct tp *t, const mblk_t *mp)
{
	if (likely(t->bnum == 1))
		if (likely(t->baddrs[0].addr != INADDR_ANY))
			return (0);
	return t_opts_size_ud_slow(t, mp);
}

/**
 * t_opts_build_ud - build options output from received message for unitdata
 * @t: private structure
 * @mp: message pointer for message
 * @op: output pointer
 * @olen: output length
 */
static INLINE streams_fastcall __hot_in int
t_opts_build_ud(const struct tp *t, mblk_t *mp, unsigned char *op, const size_t olen)
{
	struct iphdr *iph;
	struct t_opthdr *oh;

	if (op == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	iph = (struct iphdr *) mp->b_datap->db_base;
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(uint32_t);

		oh->level = T_INET_IP;
		oh->name = T_IP_ADDR;
		oh->status = T_SUCCESS;
		*((uint32_t *) T_OPT_DATA(oh)) = iph->daddr;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	assure(oh == NULL);
	return (olen);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_opts_size - size options from received message
 * @t: private structure
 * @mp: message pointer for message
 */
STATIC INLINE streams_fastcall __hot_in int
t_opts_size(const struct tp *t, const mblk_t *mp)
{
	int size = 0;
	struct iphdr *iph;
	int optlen = 0;

	iph = (struct iphdr *) mp->b_datap->db_base;
	optlen = (iph->ihl << 2) - sizeof(struct iphdr);
	if (optlen > 0)
		size += T_SPACE(optlen);	/* T_IP_OPTIONS */
	size += _T_SPACE_SIZEOF(t_defaults.ip.ttl);	/* T_IP_TTL */
	size += _T_SPACE_SIZEOF(t_defaults.ip.tos);	/* T_IP_TOS */
	size += _T_SPACE_SIZEOF(t_defaults.ip.addr);	/* T_IP_ADDR */
	size += _T_SPACE_SIZEOF(t_defaults.udp.checksum);	/* T_UDP_CHECKSUM */
	return (size);
}

/**
 * t_opts_build - build options output from received message
 * @t: private structure
 * @mp: message pointer for message
 * @op: output pointer
 * @olen: output length
 */
static INLINE streams_fastcall __hot_in int
t_opts_build(const struct tp *t, mblk_t *mp, unsigned char *op, const size_t olen)
{
	struct iphdr *iph;
	struct udphdr *uh;
	struct t_opthdr *oh;
	int optlen;

	if (op == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	iph = (struct iphdr *) mp->b_datap->db_base;
	optlen = (iph->ihl << 2) - sizeof(*iph);
	if (unlikely(optlen > 0)) {
		if (oh == NULL)
			goto efault;
		oh->len = T_LENGTH(optlen);
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		bcopy((iph + 1), T_OPT_DATA(oh), optlen);
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(unsigned char);

		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((unsigned char *) T_OPT_DATA(oh)) = iph->tos;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(unsigned char);

		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = T_SUCCESS;
		*((unsigned char *) T_OPT_DATA(oh)) = iph->ttl;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(uint32_t);

		oh->level = T_INET_IP;
		oh->name = T_IP_ADDR;
		oh->status = T_SUCCESS;
		*((uint32_t *) T_OPT_DATA(oh)) = iph->daddr;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if 1
	uh = (struct udphdr *) (mp->b_datap->db_base + (iph->ihl << 2));
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->level = T_INET_UDP;
		oh->name = T_UDP_CHECKSUM;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = (uh->check == 0) ? T_NO : T_YES;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif
	assure(oh == NULL);
	return (olen);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_errs_size - size options from received ICMP message
 * @t: private structure
 * @mp: message pointer for ICMP message
 *
 * Note that mp->b_rptr points to the encapsulated IP header.  mp->b_datap->db_base points to the IP
 * header of the ICMP message itself.
 */
STATIC noinline streams_fastcall __unlikely int
t_errs_size(const struct tp *t, const mblk_t *mp)
{
	int size = 0;

	if (mp != NULL) {
		struct iphdr *iph;
		int optlen = 0;

		iph = (struct iphdr *) mp->b_rptr;
		optlen = (iph->ihl << 2) - sizeof(struct iphdr);
		if (optlen > 0)
			size += T_SPACE(optlen);	/* T_IP_OPTIONS */
		size += _T_SPACE_SIZEOF(t_defaults.ip.ttl);	/* T_IP_TTL */
		size += _T_SPACE_SIZEOF(t_defaults.ip.tos);	/* T_IP_TOS */
		size += _T_SPACE_SIZEOF(t_defaults.ip.addr);	/* T_IP_ADDR */
		size += _T_SPACE_SIZEOF(t_defaults.udp.checksum);	/* T_UDP_CHECKSUM */
	}
	return (size);
}

/**
 * t_errs_build - build options output from receive ICMP message
 * @t: private structure
 * @mp: message pointer for ICMP message
 * @op: output pointer
 * @olen: output length
 *
 * Note that mp->b_rptr points to the encapsulated IP header.  mp->b_datap->db_base points to the IP
 * header of the ICMP message.
 */
STATIC noinline streams_fastcall __unlikely int
t_errs_build(const struct tp *t, mblk_t *mp, unsigned char *op, const size_t olen)
{
	struct iphdr *iph;
	struct udphdr *uh;
	struct t_opthdr *oh;
	int optlen;

	if (op == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	iph = (struct iphdr *) mp->b_rptr;
	optlen = (iph->ihl << 2) - sizeof(*iph);
	if (optlen > 0) {
		if (oh == NULL)
			goto efault;
		oh->len = T_LENGTH(optlen);
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		bcopy((iph + 1), T_OPT_DATA(oh), optlen);
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(unsigned char);

		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((unsigned char *) T_OPT_DATA(oh)) = iph->tos;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(unsigned char);

		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = T_SUCCESS;
		*((unsigned char *) T_OPT_DATA(oh)) = iph->ttl;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(uint32_t);

		oh->level = T_INET_IP;
		oh->name = T_IP_ADDR;
		oh->status = T_SUCCESS;
		*((uint32_t *) T_OPT_DATA(oh)) = iph->daddr;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if 1
	uh = (struct udphdr *) (mp->b_rptr + (iph->ihl << 2));
	{
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->level = T_INET_UDP;
		oh->name = T_UDP_CHECKSUM;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = (uh->check == 0) ? T_NO : T_YES;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif
	assure(oh == NULL);
	return (olen);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_opts_parse_ud - parse options from received primitive for T_UNITDATA_REQ
 * @opts: pointer to options structure
 * @ip: pointer to options
 * @ilen: length of options
 *
 * T_UNITDATA_REQ ignores unrecognized options or option levels.
 */
STATIC noinline streams_fastcall int
t_opts_parse_ud(const unsigned char *ip, const size_t ilen, struct tp_options *op)
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
			continue;
#if 0
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				continue;
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
#endif
		case T_INET_IP:
			switch (ih->name) {
			default:
				continue;
			case T_IP_OPTIONS:
			{
				if (unlikely(optlen > 40))
					goto error;
				/* FIXME: handle options */
				t_set_bit(_T_BIT_IP_OPTIONS, op->flags);
				continue;
			}
			case T_IP_TOS:
			{
				const unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->ip.tos = *valp;
				t_set_bit(_T_BIT_IP_TOS, op->flags);
				continue;
			}
			case T_IP_TTL:
			{
				const unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->ip.tos = *valp;
				t_set_bit(_T_BIT_IP_TTL, op->flags);
				continue;
			}
			case T_IP_DONTROUTE:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->ip.dontroute = *valp;
				t_set_bit(_T_BIT_IP_DONTROUTE, op->flags);
				continue;
			}
			case T_IP_BROADCAST:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->ip.broadcast = *valp;
				t_set_bit(_T_BIT_IP_BROADCAST, op->flags);
				continue;
			}
			case T_IP_ADDR:
			{
				const uint32_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				op->ip.addr = *valp;
				t_set_bit(_T_BIT_IP_ADDR, op->flags);
				continue;
			}
			case T_IP_REUSEADDR:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->ip.reuseaddr = *valp;
				t_set_bit(_T_BIT_IP_REUSEADDR, op->flags);
				continue;
			}
			}
#if 1
		case T_INET_UDP:
			switch (ih->name) {
			default:
				continue;
			case T_UDP_CHECKSUM:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->udp.checksum = *valp;
				t_set_bit(_T_BIT_UDP_CHECKSUM, op->flags);
				continue;
			}
			}
#endif
		}
	}
	return (0);
      error:
	return (err);
}

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
				op->xti.rcvbuf = *valp << 1;
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
				op->xti.sndbuf = *valp << 1;
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
		case T_INET_IP:
			switch (ih->name) {
			default:
				goto error;
			case T_IP_OPTIONS:
				if (unlikely(optlen > 40))
					goto error;
				/* FIXME: handle options */
				t_set_bit(_T_BIT_IP_OPTIONS, op->flags);
				continue;
			case T_IP_TOS:
			{
				const unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->ip.tos = *valp;
				t_set_bit(_T_BIT_IP_TOS, op->flags);
				continue;
			}
			case T_IP_TTL:
			{
				const unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				/* FIXME: validate value */
				op->ip.tos = *(unsigned char *) T_OPT_DATA(ih);
				t_set_bit(_T_BIT_IP_TTL, op->flags);
				continue;
			}
			case T_IP_DONTROUTE:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->ip.dontroute = *valp;
				t_set_bit(_T_BIT_IP_DONTROUTE, op->flags);
				continue;
			}
			case T_IP_BROADCAST:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->ip.broadcast = *valp;
				t_set_bit(_T_BIT_IP_BROADCAST, op->flags);
				continue;
			}
			case T_IP_ADDR:
			{
				const uint32_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				op->ip.addr = *valp;
				t_set_bit(_T_BIT_IP_ADDR, op->flags);
				continue;
			}
			case T_IP_REUSEADDR:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->ip.reuseaddr = *valp;
				t_set_bit(_T_BIT_IP_REUSEADDR, op->flags);
				continue;
			}
			}
#if 1
		case T_INET_UDP:
			switch (ih->name) {
			default:
				goto error;
			case T_UDP_CHECKSUM:
			{
				const t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (unlikely(optlen != sizeof(*valp)))
					goto error;
				if (unlikely(*valp != T_NO && *valp != T_YES))
					goto error;
				op->udp.checksum = *valp;
				t_set_bit(_T_BIT_UDP_CHECKSUM, op->flags);
				continue;
			}
			}
#endif
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
STATIC noinline streams_fastcall int
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
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
		case T_INET_UDP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				olen += _T_SPACE_SIZEOF(t_defaults.udp.checksum);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall int
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
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				olen += _T_SPACE_SIZEOF(t->options.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				olen += _T_SPACE_SIZEOF(t->options.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				olen += _T_SPACE_SIZEOF(t->options.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				olen += _T_SPACE_SIZEOF(t->options.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				olen += _T_SPACE_SIZEOF(t->options.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				olen += _T_SPACE_SIZEOF(t->options.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
		case T_INET_UDP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				olen += _T_SPACE_SIZEOF(t->options.udp.checksum);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall int
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
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				if (optlen && optlen != sizeof(t->options.ip.options))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				if (optlen && optlen != sizeof(t->options.ip.tos))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				if (optlen && optlen != sizeof(t->options.ip.ttl))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				if (optlen && optlen != sizeof(t->options.ip.reuseaddr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				if (optlen && optlen != sizeof(t->options.ip.dontroute))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				if (optlen && optlen != sizeof(t->options.ip.broadcast))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				if (optlen && optlen != sizeof(t->options.ip.addr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
		case T_INET_UDP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				if (optlen && optlen != sizeof(t->options.udp.checksum))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall int
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
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.ip.tos))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.ip.ttl))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.ip.reuseaddr))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.ip.dontroute))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.ip.broadcast))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.ip.addr))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
		case T_INET_UDP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.udp.checksum))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.udp.checksum);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall t_scalar_t
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
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_rmem_default;
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
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_wmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_wmem_default >> 1;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				oh->len = sizeof(*oh);
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				/* not supported yet */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.tos);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.tos;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ttl);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.ttl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.reuseaddr);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.reuseaddr;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.dontroute);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.dontroute;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.broadcast);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.broadcast;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.addr);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				*((uint32_t *) T_OPT_DATA(oh)) = t_defaults.ip.addr;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.udp.checksum);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.udp.checksum;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall t_scalar_t
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
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvbuf >> 1;
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
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndbuf >> 1;
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
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				oh->len = sizeof(*oh);
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				/* not supported yet */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.tos);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned char *) T_OPT_DATA(oh)) = t->options.ip.tos;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.ttl);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned char *) T_OPT_DATA(oh)) = t->options.ip.ttl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.reuseaddr);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = t->options.ip.reuseaddr;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.dontroute);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = t->options.ip.dontroute;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.broadcast);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = t->options.ip.broadcast;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.addr);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((uint32_t *) T_OPT_DATA(oh)) = t->options.ip.addr;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = _T_LENGTH_SIZEOF(t->options.udp.checksum);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.udp.checksum;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall t_scalar_t
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
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
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
			case T_IP_OPTIONS:
				/* not supported yet */
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TOS:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);
					unsigned char prec, type;

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					prec = (*valp >> 5) & 0x7;
					type = *valp & (T_LDELAY | T_HITHRPT | T_HIREL | T_LOCOST);
					if (*valp != SET_TOS(prec, type))
						goto einval;
					if (prec >= T_CRITIC_ECP && !capable(CAP_NET_ADMIN))
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp == 0) {
						*valp = sysctl_ip_default_ttl;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#if 0
					if (*valp > 255) {
						*valp = 255;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#endif
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_UDP:
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
			case T_UDP_CHECKSUM:
				oh->len = ih->len;
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall t_scalar_t
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
					*valp = sysctl_rmem_default;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
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
				t->options.xti.rcvbuf = *valp << 1;
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
					*valp = sysctl_wmem_default;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > sysctl_wmem_max) {
						*valp = sysctl_wmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.xti.sndbuf = *valp << 1;
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
					*valp = sysctl_wmem_default >> 1;
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
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				/* not supported yet */
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.tos;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					{
						unsigned char prec = (*valp >> 5) & 0x7;
						unsigned char type =
						    *valp & (T_LDELAY | T_HITHRPT |
							     T_HIREL | T_LOCOST);

						if (*valp != SET_TOS(prec, type))
							goto einval;
						if (prec >= T_CRITIC_ECP && !capable(CAP_NET_ADMIN)) {
							oh->status =
							    t_overall_result
							    (&overall, T_NOTSUPPORT);
							break;
						}
					}
				}
				t->options.ip.tos = *valp;
				// sk->sk_priority = rt_tos2priority(*valp);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TTL:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.ttl;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp == 0) {
						*valp = sysctl_ip_default_ttl;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				t->options.ip.ttl = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_REUSEADDR:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.reuseaddr;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				t->options.ip.reuseaddr = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_DONTROUTE:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.dontroute;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				t->options.ip.dontroute = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_BROADCAST:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.broadcast;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				t->options.ip.broadcast = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_ADDR:
			{
				uint32_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.addr;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
				}
				t->options.ip.addr = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#if 1
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.udp.checksum;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				t->options.udp.checksum = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
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
STATIC noinline streams_fastcall t_scalar_t
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

/*
 *  =========================================================================
 *
 *  IP Local Management
 *
 *  =========================================================================
 */

STATIC int tp_v4_rcv(struct sk_buff *skb);
STATIC void tp_v4_err(struct sk_buff *skb, u32 info);

/*
 *  IP subsystem management
 */
#ifdef LINUX
/**
 * tp_v4_steal - steal a socket buffer
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
STATIC INLINE streams_fastcall __hot_in void
tp_v4_steal(struct sk_buff *skb)
{
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
	skb->nh.iph->protocol = 255;
	skb->protocol = 255;
#endif				/* HAVE_KTYPE_STRUCT_INET_PROTOCOL */
}

/**
 * tp_v4_rcv_next - pass a socket buffer to the next handler
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
STATIC INLINE streams_fastcall __hot_in int
tp_v4_rcv_next(struct sk_buff *skb)
{
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
	struct tp_prot_bucket *pb;
	struct net_protocol *pp;
	unsigned char proto;

	proto = skb->nh.iph->protocol;
	if ((pb = udp_prots[proto]) && (pp = pb->prot.next)) {
		pp->handler(skb);
		return (1);
	}
	kfree_skb(skb);
	return (0);
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
	struct tp_prot_bucket *pb;
	unsigned char proto;

	proto = skb->nh.iph->protocol;
	kfree_skb(skb);
	pb = udp_prots[proto];
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
	if (pb)
		return (pb->prot.copy != 0);
#endif
	return (0);
#endif
}

/**
 * tp_v4_err_next - pass a socket buffer to the next error handler
 * @skb: socket buffer to pass
 *
 * In the 2.6 packet error handler, if the packet is not for us, pass it to the next error handler.
 * If there is no next error handler, simply return.
 *
 * In the 2.4 packet error handler, if the packet is not for us, pass it to the next error handler
 * by simply returning.  Error packets are not cloned, so don't free it.
 */
STATIC INLINE streams_fastcall __hot_in void
tp_v4_err_next(struct sk_buff *skb, __u32 info)
{
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
	struct tp_prot_bucket *pb;
	struct net_protocol *pp;
	unsigned char proto;

	proto = ((struct iphdr *) skb->data)->protocol;
	if ((pb = udp_prots[proto])
	    && (pp = pb->prot.next))
		pp->err_handler(skb, info);
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
	return;
}

#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;
STATIC struct net_protocol **inet_protosp = (typeof(inet_protosp)) HAVE_INET_PROTOS_ADDR;
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */

#ifdef HAVE_MODULE_TEXT_ADDRESS_ADDR
#define module_text_address(__arg) ((typeof(&module_text_address))HAVE_MODULE_TEXT_ADDRESS_ADDR)((__arg))
#endif

/**
 * tp_init_nproto - initialize network protocol override
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
STATIC INLINE streams_fastcall __unlikely struct tp_prot_bucket *
tp_init_nproto(unsigned char proto, unsigned int type)
{
	struct tp_prot_bucket *pb;
	struct inet_protocol *pp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	write_lock_bh(&udp_prot_lock);
	if ((pb = udp_prots[proto]) != NULL) {
		pb->refs++;
		switch (type) {
		case T_COTS:
		case T_COTS_ORD:
			++pb->corefs;
			break;
		case T_CLTS:
			++pb->clrefs;
			break;
		}
	} else if ((pb = kmem_cache_alloc(udp_prot_cachep, SLAB_ATOMIC))) {
		pb->refs = 1;
		switch (type) {
		case T_COTS:
		case T_COTS_ORD:
			pb->corefs = 1;
			pb->clrefs = 0;
			break;
		case T_CLTS:
			pb->corefs = 0;
			pb->clrefs = 1;
			break;
		}
		pp = &pb->prot;
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
		(void) hash;
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
		pp->protocol = proto;
		pp->name = "streams-ip";
#endif
		pp->handler = &tp_v4_rcv;
		pp->err_handler = &tp_v4_err;
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
		pp->copy = 0;
		pp->next = NULL;
#endif
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
		pp->no_policy = 1;
#endif
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
		inet_add_protocol(pp);
#else
		inet_add_protocol(pp, proto);
#endif
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO
		pp->proto.proto = proto;
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO */
		pp->proto.handler = &tp_v4_rcv;
		pp->proto.err_handler = &tp_v4_err;
		pp->proto.no_policy = 1;
		pp->next = NULL;
		pp->kmod = NULL;
		spin_lock_bh(inet_proto_lockp);
		if ((pp->next = inet_protosp[hash]) != NULL) {
			if ((pp->kmod = module_text_address((ulong) pp->next))
			    && pp->kmod != THIS_MODULE) {
				if (!try_module_get(pp->kmod)) {
					spin_unlock_bh(inet_proto_lockp);
					kmem_cache_free(udp_prot_cachep, pb);
					return (NULL);
				}
			}
		}
		inet_protosp[hash] = &pp->proto;
		spin_unlock_bh(inet_proto_lockp);
		// synchronize_net(); /* might sleep */
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
		/* link into hash slot */
		udp_prots[proto] = pb;
	}
	write_unlock_bh(&udp_prot_lock);
	return (pb);
}

/**
 * tp_term_nproto - terminate network protocol override
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
STATIC INLINE streams_fastcall __unlikely void
tp_term_nproto(unsigned char proto, unsigned int type)
{
	struct tp_prot_bucket *pb;

	write_lock_bh(&udp_prot_lock);
	if ((pb = udp_prots[proto]) != NULL) {
		switch (type) {
		case T_COTS:
		case T_COTS_ORD:
			--pb->corefs;
			break;
		case T_CLTS:
			--pb->clrefs;
			break;
		}
		if (--pb->refs == 0) {
			struct inet_protocol *pp = &pb->prot;

#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
			inet_del_protocol(pp);
#else
			inet_del_protocol(pp, proto);
#endif
			/* unlink from hash slot */
			udp_prots[proto] = NULL;
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
			spin_lock_bh(inet_proto_lockp);
			inet_protosp[proto] = pp->next;
			spin_unlock_bh(inet_proto_lockp);
			if (pp->next != NULL && pp->kmod != NULL && pp->kmod != THIS_MODULE)
				module_put(pp->kmod);
			/* unlink from hash slot */
			udp_prots[proto] = NULL;
			// synchronize_net(); /* might sleep */
#else
#error
#endif
			kmem_cache_free(udp_prot_cachep, pb);
		}
	}
	write_unlock_bh(&udp_prot_lock);
}
#endif				/* LINUX */

/**
 *  tp_bind_prot -  bind a protocol
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
STATIC INLINE streams_fastcall __unlikely int
tp_bind_prot(unsigned char proto, unsigned int type)
{
	struct tp_prot_bucket *pb;

	if ((pb = tp_init_nproto(proto, type)))
		return (0);
	return (-ENOMEM);
}

/**
 *  tp_unbind_prot - unbind a protocol
 *  @proto:	    protocol number to unbind
 */
STATIC INLINE streams_fastcall __unlikely void
tp_unbind_prot(unsigned char proto, unsigned int type)
{
	tp_term_nproto(proto, type);
}

/**
 * tp_bind - bind a Stream to a TSAP
 * @tp: private structure
 * @ADDR_buffer: addresses to bind
 * @ADDR_length: length of addresses
 * @CONIND_number: maximum number of connection indications
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
 * TODO: also check Linux UPD bind hashes.  If the port is specifically in the Linux UDP dynamic
 * port assignment range, we should just refuse it.  This is because Linux UDP believes that it owns
 * this range and will assign within this range without the user having any control.  If outside
 * this range, it might be an idea to check the Linux UDP hashes to make sure that noone else is
 * using this port at the moment.  It does not keep Linux UDP from assigning it later, resulting in
 * a conflict.
 *
 */
STATIC INLINE streams_fastcall __unlikely int
tp_bind(struct tp *tp, struct sockaddr_in *ADDR_buffer, const t_uscalar_t ADDR_length,
	const t_uscalar_t CONIND_number)
{
	static unsigned short tp_prev_port = 61000;
	static const unsigned short tp_frst_port = 61000;	/* XXX */
	static const unsigned short tp_last_port = 65000;	/* XXX */

	struct tp_bhash_bucket *hp;
	unsigned short bport = ADDR_buffer[0].sin_port;
	unsigned char proto = tp->options.ip.protocol;
	size_t anum = ADDR_length / sizeof(*ADDR_buffer);
	struct tp *tp2;
	int i, j, err;
	unsigned short num = 0;

	if (bport == 0) {
		num = tp_prev_port;	/* UNSAFE */
	      try_again:
		bport = htons(num);
	}
	hp = &udp_bhash[udp_bhashfn(proto, bport)];
	write_lock_bh(&hp->lock);
	for (tp2 = hp->list; tp2; tp2 = tp2->bnext) {
		if (proto != tp2->protoids[0])
			continue;
		if (bport != tp2->bport)
			continue;
#if 0
		/* Allowed to bind to each NSAP once as DEFAULT_DEST, once as DEFAULT_LISTENER and
		   once as neither. */
		if ((BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)) !=
		    (np2->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
			continue;
#endif
		for (i = 0; i < tp2->bnum; i++) {
#if 0
			if (tp2->baddrs[i].addr == 0)
				break;
#endif
			for (j = 0; j < anum; j++)
				if (tp2->baddrs[i].addr == ADDR_buffer[j].sin_addr.s_addr)
					break;
			if (j < anum)
				break;
		}
		if (i < tp2->bnum)
			break;
	}
	if (tp2 != NULL) {
		write_unlock_bh(&hp->lock);
		if (num == 0)
			/* specific port number requested */
			return (TADDRBUSY);
		if (++num > tp_last_port)
			num = tp_frst_port;
		if (num != tp_prev_port)
			goto try_again;
		return (TNOADDR);
	}
	if ((err = tp_bind_prot(proto, tp->info.SERV_type))) {
		write_unlock_bh(&hp->lock);
		return (err);
	}
	if (num != 0)
		tp_prev_port = num;
	ADDR_buffer[0].sin_port = bport;
	if ((tp->bnext = hp->list))
		tp->bnext->bprev = &tp->bnext;
	tp->bprev = &hp->list;
	hp->list = tp_get(tp);
	tp->bhash = hp;
	/* copy into private structure */
	tp->CONIND_number = CONIND_number;
	tp->pnum = 1;

	tp->protoids[0] = proto;
	tp->bnum = anum;
	tp->bport = bport;
	_ptrace(("%s: %s: bound proto = %d, bport = %d\n", DRV_NAME, __FUNCTION__,
		 (int) proto, (int) ntohs(bport)));
	for (i = 0; i < anum; i++)
		tp->baddrs[i].addr = ADDR_buffer[i].sin_addr.s_addr;
	write_unlock_bh(&hp->lock);
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
	synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
	return (0);
}

#if defined HAVE_KFUNC_DST_OUTPUT
STATIC INLINE __hot_out int
tp_ip_queue_xmit(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct iphdr *iph = skb->nh.iph;

#if defined NETIF_F_TSO
	ip_select_ident_more(iph, dst, NULL, 0);
#else				/* !defined NETIF_F_TSO */
	ip_select_ident(iph, dst, NULL);
#endif				/* defined NETIF_F_TSO */
	ip_send_check(iph);
#if defined HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dst->dev, ip_dst_output);
#else				/* !defined HAVE_KFUNC_IP_DST_OUTPUT */
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dst->dev, dst_output);
#endif				/* defined HAVE_KFUNC_IP_DST_OUTPUT */
}
#else				/* !defined HAVE_KFUNC_DST_OUTPUT */
STATIC INLINE __hot_out int
tp_ip_queue_xmit(struct sk_buff *skb)
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

STATIC noinline streams_fastcall void
tp_skb_destructor_slow(struct tp *tp, struct sk_buff *skb)
{
	spin_lock_bh(&tp->qlock);
	// ensure(tp->sndmem >= skb->truesize, tp->sndmem = skb->truesize);
	tp->sndmem -= skb->truesize;
	if (unlikely((tp->sndmem < tp->options.xti.sndlowat || tp->sndmem == 0))) {
		tp->sndblk = 0;	/* no longer blocked */
		spin_unlock_bh(&tp->qlock);
		if (tp->iq != NULL && tp->iq->q_first != NULL)
			qenable(tp->iq);
	} else {
		spin_unlock_bh(&tp->qlock);
	}
#if 0				/* destructor is nulled by skb_orphan */
	skb_shinfo(skb)->frags[0].page = NULL;
	skb->destructor = NULL;
#endif
	tp_put(tp);
	return;
}

/**
 * tp_skb_destructor - socket buffer destructor
 * @skb: socket buffer to destroy
 *
 * This provides the impedance matching between socket buffer flow control and STREAMS flow control.
 * When tp->sndmem is greater than tp->options.xti.sndbuf we place STREAMS buffers back on the send
 * queue and stall the queue.  When the send memory falls below tp->options.xti.sndlowat (or to
 * zero) and there are message on the send queue, we enable the queue.
 *
 * NOTE: There was not enough hysteresis in this function!  It was qenabling too fast.  We need a
 * flag in the private structure that indicates that the queue is stalled awaiting subsiding below
 * the send low water mark (or to zero) that is set when we stall the queue and reset when we fall
 * beneath the low water mark.
 */
STATIC __hot_out void
tp_skb_destructor(struct sk_buff *skb)
{
	struct tp *tp;

	tp = (typeof(tp)) skb_shinfo(skb)->frags[0].page;
	dassert(tp != NULL);
	if (likely(tp->sndblk == 0)) {
		/* technically we could have multiple processors freeing sk_buffs at the same time */
		spin_lock_bh(&tp->qlock);
		// ensure(tp->sndmem >= skb->truesize, tp->sndmem = skb->truesize);
		tp->sndmem -= skb->truesize;
		spin_unlock_bh(&tp->qlock);
#if 0				/* destructor is nulled by skb_orphan */
		skb_shinfo(skb)->frags[0].page = NULL;
		skb->destructor = NULL;
#endif
		tp_put(tp);
		return;
	}
	tp_skb_destructor_slow(tp, skb);
}

#undef skbuff_head_cache
#ifdef HAVE_SKBUFF_HEAD_CACHE_ADDR
#define skbuff_head_cache (*((kmem_cache_t **) HAVE_SKBUFF_HEAD_CACHE_ADDR))
#endif

/**
 * tp_alloc_skb_slow - allocate a socket buffer from a message block
 * @tp: private pointer
 * @mp: the message block
 * @headroom: header room for resulting sk_buff
 * @gfp: general fault protection
 *
 * This is the old slow way of allocating a socket buffer.  We simple allocate a socket buffer with
 * sufficient head room and copy the data from the message block(s) to the socket buffer.  This is
 * slow.  This is the only way that LiS can do things (because it has unworkable message block
 * allocation).
 */
STATIC noinline streams_fastcall __unlikely struct sk_buff *
tp_alloc_skb_slow(struct tp *tp, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;
	unsigned int dlen = msgsize(mp);

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
					__assert(data <= skb->tail);
				} else
					rare();
			}
		}
		freemsg(mp);	/* must absorb */
		/* we never have any page fragments, so we can steal a pointer from the page
		   fragement list. */
		assert(skb_shinfo(skb)->nr_frags == 0);
		skb_shinfo(skb)->frags[0].page = (struct page *) tp_get(tp);
		skb->destructor = tp_skb_destructor;
		spin_lock_bh(&tp->qlock);
		tp->sndmem += skb->truesize;
		spin_unlock_bh(&tp->qlock);
	}
	return (skb);
}

/**
 * tp_alloc_skb - allocate a socket buffer from a message block
 * @tp: private pointer
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
STATIC INLINE streams_fastcall __hot_out struct sk_buff *
tp_alloc_skb(struct tp *tp, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;
	unsigned char *beg, *end;
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
	skb->tail = mp->b_wptr;
	skb->end = end;
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

	/* we never have any page fragments, so we can steal a pointer from the page fragement
	   list. */
	assert(skb_shinfo(skb)->nr_frags == 0);
	skb_shinfo(skb)->frags[0].page = (struct page *) tp_get(tp);
	skb->destructor = tp_skb_destructor;
	spin_lock_bh(&tp->qlock);
	tp->sndmem += skb->truesize;
	spin_unlock_bh(&tp->qlock);

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
      go_frag: /* for now */
      go_slow:
	return tp_alloc_skb_slow(tp, mp, headroom, gfp);
}
#else
STATIC INLINE streams_fastcall __hot_out struct sk_buff *
tp_alloc_skb(struct tp *tp, mblk_t *mp, unsigned int headroom, int gfp)
{
	return tp_alloc_skb_slow(tp, mp, headroom, gfp);
}
#endif

STATIC noinline streams_fastcall int
tp_route_output_slow(struct tp *tp, const struct tp_options *opt, struct rtable **rtp)
{
	int err;

	if (XCHG(rtp, NULL) != NULL)
		dst_release(XCHG(&tp->daddrs[0].dst, NULL));
	if (likely((err = ip_route_output(rtp, opt->ip.daddr, opt->ip.addr, 0, 0)) == 0)) {
		dst_hold(&(*rtp)->u.dst);
		tp->daddrs[0].dst = &(*rtp)->u.dst;
	}
	return (err);
}

STATIC INLINE streams_fastcall __hot_out int
tp_route_output(struct tp *tp, const struct tp_options *opt, struct rtable **rtp)
{
	register struct rtable *rt;

	if (likely((rt = *rtp) != NULL)) {
		if (likely(rt->rt_dst == opt->ip.daddr)) {
			dst_hold(&rt->u.dst);
			return (0);
		}
	}
	return tp_route_output_slow(tp, opt, rtp);
}

/**
 * tp_senddata - process a unit data request
 * @tp: Stream private structure
 * @dport: destination port
 * @opt: options to use
 * @mp: message payload
 */
STATIC INLINE streams_fastcall __hot_out int
tp_senddata(struct tp *tp, const unsigned short dport, const struct tp_options *opt, mblk_t *mp)
{
	struct rtable *rt;
	int err;

	prefetch((void *)opt);
	rt = (struct rtable *) tp->daddrs[0].dst;
	prefetch(rt);

	/* Allows slop over by 1 buffer per processor. */
	if (unlikely(tp->sndmem > tp->options.xti.sndbuf))
		goto ebusy;

	assert(opt != NULL);
	if (likely((err = tp_route_output(tp, opt, &rt)) == 0)) {
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

		if (likely((skb = tp_alloc_skb(tp, mp, hlen, GFP_ATOMIC)) != NULL)) {
			struct iphdr *iph;
			struct udphdr *uh;
			uint32_t saddr = opt->ip.saddr ? opt->ip.saddr : rt->rt_src;
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
			skb->h.raw = __skb_push(skb, sizeof(struct udphdr));
			skb->nh.raw = __skb_push(skb, sizeof(struct iphdr));

			skb->dst = &rt->u.dst;
			skb->priority = 0;	// opt->xti.priority;

			iph = skb->nh.iph;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = opt->ip.tos;
			iph->frag_off = htons(IP_DF);	/* never frag */
			// iph->frag_off = 0; /* need qos bit */
			iph->ttl = opt->ip.ttl;
			iph->daddr = daddr;
			iph->saddr = saddr;
			iph->protocol = opt->ip.protocol;
			iph->tot_len = htons(tlen);

			uh = skb->h.uh;
			uh->dest = dport;
			uh->source = tp->sport ? tp->sport : tp->bport;
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
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, tp_ip_queue_xmit);
#else
			tp_ip_queue_xmit(skb);
#endif
			return (QR_ABSORBED);
		}
		_rare();
		return (-ENOBUFS);
	}
	_rare();
	return (err);
      ebusy:
	return (-EBUSY);
}

#if 0
STATIC INLINE streams_fastcall int
np_datack(queue_t *q)
{
	/* not supported */
	return (-EOPNOTSUPP);
}
#endif

/**
 * tp_conn_check - check and enter into connection hashes
 * @tp: private structure
 * @proto: protocol to which to connect
 */
STATIC streams_fastcall int
tp_conn_check(struct tp *tp, const unsigned char proto)
{
	unsigned short sport = tp->sport;
	unsigned short dport = tp->dport;
	struct tp *conflict = NULL;
	struct tp_chash_bucket *hp, *hp1, *hp2;

	hp1 = &udp_chash[udp_chashfn(proto, dport, sport)];
	hp2 = &udp_chash[udp_chashfn(proto, 0, 0)];

	write_lock_bh(&hp1->lock);
	if (hp1 != hp2)
		write_lock(&hp2->lock);

	hp = hp1;
	do {
		struct tp *tp2;
		t_uscalar_t state;

		for (tp2 = hp->list; tp2; tp2 = tp2->cnext) {
			int i, j;

			if ((state = tp_get_state(tp2)) != TS_DATA_XFER && state != TS_WIND_ORDREL)
				continue;
			if (tp2->sport != sport)
				continue;
			if (tp2->dport != dport)
				continue;
			for (i = 0; conflict == NULL && i < tp2->snum; i++)
				for (j = 0; conflict == NULL && j < tp->snum; j++)
					if (tp2->saddrs[i].addr == tp->saddrs[j].addr)
						conflict = tp2;
			if (conflict == NULL)
				continue;
			conflict = NULL;
			for (i = 0; conflict == NULL && i < tp2->dnum; i++)
				for (j = 0; conflict == NULL && j < tp->dnum; j++)
					if (tp2->daddrs[i].addr == tp->daddrs[j].addr)
						conflict = tp2;
			if (conflict == NULL)
				continue;
			break;
		}
	} while (conflict == NULL && hp != hp2 && (hp = hp2));
	if (conflict != NULL) {
		int i;

		if (hp1 != hp2)
			write_unlock(&hp2->lock);
		write_unlock_bh(&hp1->lock);
		/* free dst caches */
		for (i = 0; i < tp->dnum; i++)
			dst_release(XCHG(&tp->daddrs[i].dst, NULL));
		tp->dnum = 0;
		tp->dport = 0;
		/* blank source addresses */
		tp->snum = 0;
		tp->sport = 0;
		/* how do we say already connected? (-EISCONN) */
		return (TADDRBUSY);
	}
	/* link into connection hash */
	if ((tp->cnext = hp1->list))
		tp->cnext->cprev = &tp->cnext;
	tp->cprev = &hp1->list;
	hp1->list = tp_get(tp);
	tp->chash = hp1;
	if (hp1 != hp2)
		write_unlock(&hp2->lock);
	write_unlock_bh(&hp1->lock);
	return (0);
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
STATIC streams_fastcall int
tp_connect(struct tp *tp, const struct sockaddr_in *DEST_buffer, const socklen_t DEST_length,
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
		if (OPT_buffer->xti.rcvbuf < SOCK_MIN_RCVBUF >> 1)
			OPT_buffer->xti.rcvbuf = SOCK_MIN_RCVBUF >> 1;
		OPT_buffer->xti.rcvbuf <<= 1;
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
		if (OPT_buffer->xti.sndbuf < SOCK_MIN_SNDBUF >> 1)
			OPT_buffer->xti.sndbuf = SOCK_MIN_SNDBUF >> 1;
		OPT_buffer->xti.sndbuf <<= 1;
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
	if (t_tst_bit(_T_BIT_IP_PROTOCOL, OPT_buffer->flags)) {
		for (i = 0; i < tp->pnum; i++)
			if (tp->protoids[i] == OPT_buffer->ip.protocol)
				break;
		if (i >= tp->bnum)
			goto error;
	} else {
		OPT_buffer->ip.protocol = tp->options.ip.protocol;
	}
	if (t_tst_bit(_T_BIT_IP_TTL, OPT_buffer->flags)) {
		if ((t_scalar_t) (signed char) OPT_buffer->ip.ttl < 1)
			goto error;
		// if ((t_scalar_t) (signed char) OPT_buffer->ip.ttl > 255)
		// goto error;
	} else {
		OPT_buffer->ip.ttl = tp->options.ip.ttl;
	}
	if (t_tst_bit(_T_BIT_IP_TOS, OPT_buffer->flags)) {
		if ((t_scalar_t) (signed char) OPT_buffer->ip.tos < 0)
			goto error;
		if ((t_scalar_t) (signed char) OPT_buffer->ip.tos > 15)
			goto error;
	} else {
		OPT_buffer->ip.tos = tp->options.ip.tos;
	}
	if (t_tst_bit(_T_BIT_IP_MTU, OPT_buffer->flags)) {
		if ((t_scalar_t) OPT_buffer->ip.mtu < 536)
			goto error;
		if ((t_scalar_t) OPT_buffer->ip.mtu > 65535)
			goto error;
	} else {
		OPT_buffer->ip.mtu = tp->options.ip.mtu;
	}
	if (t_tst_bit(_T_BIT_UDP_CHECKSUM, OPT_buffer->flags)) {
		if ((t_scalar_t) OPT_buffer->udp.checksum != T_YES
		    && (t_scalar_t) OPT_buffer->udp.checksum != T_NO)
			goto error;
	} else {
		OPT_buffer->udp.checksum = tp->options.udp.checksum;
	}

	/* Need to determine source addressess from bound addresses before we can test the source
	   address.  If we are bound to specific addresses, then the source address list is simply
	   the destination address list. If bound to a wildcard address, then the source address
	   list could be determined from the scope of the destination addresses and the available
	   interfaces and their addresses.  However, for the moment it is probably easier to simply 
	   allow wildcard source addresses and let the user specify any address when there is a
	   wildcard source address. */

	tp->sport = tp->bport;
	tp->snum = tp->bnum;
	for (i = 0; i < tp->bnum; i++)
		tp->saddrs[i].addr = tp->baddrs[i].addr;

	if (t_tst_bit(_T_BIT_IP_SADDR, OPT_buffer->flags)) {
		if (OPT_buffer->ip.saddr != 0) {
			for (i = 0; i < tp->snum; i++) {
				if (tp->saddrs[i].addr == INADDR_ANY)
					break;
				if (tp->saddrs[i].addr == OPT_buffer->ip.saddr)
					break;
			}
			if (i >= tp->snum)
				goto error;
		}
	} else {
		OPT_buffer->ip.saddr = tp->options.ip.saddr;
	}
	if (t_tst_bit(_T_BIT_IP_DADDR, OPT_buffer->flags)) {
		/* Specified default destination address must be in the destination address list. */
		for (i = 0; i < dnum; i++)
			if (DEST_buffer[i].sin_addr.s_addr == OPT_buffer->ip.daddr)
				break;
		if (i >= dnum)
			goto error;
	} else {
		/* The default destination address is the first address in the list. */
		OPT_buffer->ip.daddr = DEST_buffer[0].sin_addr.s_addr;
	}

	/* Destination addresses have been checked as follows: they have been aligned. There is at
	   least 1 address and no more than 8 addresses.  The first address has an address family
	   type of AF_INET or zero (0).  No IP address in the list is INADDR_ANY.  Things that have
	   not been checked are: there might be duplicates in the list.  The user might not have the 
	   necessary privilege to use some of the addresses.  Some addresses might be zeronet,
	   broadcast or multicast addresses. The addresses might be of disjoint scope.  There might
	   not exist a route to some addresses.  The destination port number might be zero. */

	tp->dport = DEST_buffer[0].sin_port;

	err = TBADADDR;
	if (tp->dport == 0 && (tp->bport != 0 || tp->sport != 0))
		goto error;
	if (tp->dport != 0 && tp->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port
		   number. */
		goto error;

	for (i = 0; i < dnum; i++) {
		struct rtable *rt = NULL;

		if ((err = ip_route_output(&rt, DEST_buffer[i].sin_addr.s_addr, 0, 0, 0))) {
			while (--i >= 0)
				dst_release(XCHG(&tp->daddrs[i].dst, NULL));
			goto error;
		}
		tp->daddrs[i].dst = &rt->u.dst;

		/* Note that we do not have to use the destination reference cached above.  It is
		   enough that we hold a reference to it so that it remains in the routing caches
		   so lookups to this destination are fast.  They will be released upon
		   disconnection. */

		tp->daddrs[i].addr = DEST_buffer[i].sin_addr.s_addr;
		tp->daddrs[i].ttl = OPT_buffer->ip.ttl;
		tp->daddrs[i].tos = OPT_buffer->ip.tos;
		tp->daddrs[i].mtu = dst_pmtu(tp->daddrs[i].dst);
		if (tp->daddrs[i].mtu < OPT_buffer->ip.mtu)
			OPT_buffer->ip.mtu = tp->daddrs[i].mtu;
	}
	tp->dnum = dnum;

	/* try to place in connection hashes with conflict checks */
	if ((err = tp_conn_check(tp, OPT_buffer->ip.protocol)) != 0)
		goto error;

	/* store negotiated values */
	tp->options.xti.priority = OPT_buffer->xti.priority;
	tp->options.ip.protocol = OPT_buffer->ip.protocol;
	tp->options.ip.ttl = OPT_buffer->ip.ttl;
	tp->options.ip.tos = OPT_buffer->ip.tos;
	tp->options.ip.mtu = OPT_buffer->ip.mtu;
	tp->options.ip.saddr = OPT_buffer->ip.saddr;
	tp->options.ip.daddr = OPT_buffer->ip.daddr;
	tp->options.udp.checksum = OPT_buffer->udp.checksum;

	return (0);
      error:
	return (err);
}

#if 0
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
	mblk_t *resp, **respp;

	/* find last one on list */
	for (respp = &np->resq; (*respp) && (*respp)->b_next; respp = &(*respp)->b_next) ;
	if (*respp == NULL)
		return (-EFAULT);
	resp = *respp;
	*respp = resp->b_next;
	resp->b_next = NULL;
	freemsg(resp);
	np->resinds--;
	return (0);
}
#endif

/**
 * tp_unbind - unbind a Stream from an NSAP
 * @tp: private structure
 *
 * Simply remove the Stream from the bind hashes and release a reference to the Stream.  This
 * function can be called whether the stream is bound or not (and is always called before the
 * private structure is freed.
 */
STATIC int
tp_unbind(struct tp *tp)
{
	struct tp_bhash_bucket *hp;

	if ((hp = tp->bhash)) {
		write_lock_bh(&hp->lock);
		if ((*tp->bprev = tp->bnext))
			tp->bnext->bprev = tp->bprev;
		tp->bnext = NULL;
		tp->bprev = &tp->bnext;
		tp->bhash = NULL;
		tp_unbind_prot(tp->protoids[0], tp->info.SERV_type);
		tp->bport = tp->sport = 0;
		tp->bnum = tp->snum = tp->pnum = 0;
		tp_put(tp);
		write_unlock_bh(&hp->lock);
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
		synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
		return (0);
	}
	return (-EALREADY);
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
STATIC noinline streams_fastcall int
tp_passive(struct tp *tp, const struct sockaddr_in *RES_buffer, const socklen_t RES_length,
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
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
		/* Accepting Stream must be bound to the same protocol as connection indication. */
		for (j = 0; j < ACCEPTOR_id->pnum; j++)
			if (ACCEPTOR_id->protoids[j] == iph->protocol)
				break;
		if (j >= ACCEPTOR_id->pnum)
			/* Must be bound to the same protocol. */
			goto error;
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
		/* Problem for 2.4: we overwrote iph->protocol with 255 to steal the packet, so we
		   can't check it now.  Check that the accepting stream is bound to the same
		   protocol id as the listening stream. */
		/* Another approach for 2.4 would be to copy the sk_buff before stealing it. */
		for (j = 0, i = 0; j < ACCEPTOR_id->pnum; j++) {
			for (i = 0; i < tp->pnum; i++)
				if (ACCEPTOR_id->protoids[j] == tp->protoids[i])
					break;
			if (ACCEPTOR_id->protoids[j] == tp->protoids[i])
				break;
		}
		if (j >= ACCEPTOR_id->pnum || i >= tp->pnum)
			/* Must be bound to the same protocol. */
			goto error;
#endif				/* HAVE_KTYPE_STRUCT_INET_PROTOCOL */
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
		if (OPT_buffer->xti.rcvbuf < SOCK_MIN_RCVBUF >> 1)
			OPT_buffer->xti.rcvbuf = SOCK_MIN_RCVBUF >> 1;
		OPT_buffer->xti.rcvbuf <<= 1;
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
		if (OPT_buffer->xti.sndbuf < SOCK_MIN_SNDBUF >> 1)
			OPT_buffer->xti.sndbuf = SOCK_MIN_SNDBUF >> 1;
		OPT_buffer->xti.sndbuf <<= 1;
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
	if (t_tst_bit(_T_BIT_IP_PROTOCOL, OPT_buffer->flags)) {
		/* Specified protocol probably needs to be the same as the indication, but since we
		   only bind to one protocol id at the moment that is not a problem.  The connection 
		   indication protocol was checked against the accepting Stream above. */
		for (i = 0; i < ACCEPTOR_id->pnum; i++)
			if (ACCEPTOR_id->protoids[i] == OPT_buffer->ip.protocol)
				break;
		if (i >= ACCEPTOR_id->pnum)
			goto error;
	} else {
		OPT_buffer->ip.protocol = ACCEPTOR_id->options.ip.protocol;
	}
	if (t_tst_bit(_T_BIT_IP_TTL, OPT_buffer->flags)) {
		if ((t_scalar_t) (signed char) OPT_buffer->ip.ttl < 1)
			goto error;
		// if ((t_scalar_t) (signed char)OPT_buffer->ip.ttl > 127)
		// goto error;
	} else {
		OPT_buffer->ip.ttl = ACCEPTOR_id->options.ip.ttl;
	}
	if (t_tst_bit(_T_BIT_IP_TOS, OPT_buffer->flags)) {
		if ((t_scalar_t) (signed char) OPT_buffer->ip.tos < 0)
			goto error;
		if ((t_scalar_t) (signed char) OPT_buffer->ip.tos > 15)
			goto error;
	} else {
		/* FIXME: TOS should be negotiated.  The TOS should be upgraded to whatever TOS the 
		   caller wishes, but not downgraded. */
		OPT_buffer->ip.tos = ACCEPTOR_id->options.ip.tos;
	}
	if (t_tst_bit(_T_BIT_IP_MTU, OPT_buffer->flags)) {
		if ((t_scalar_t) OPT_buffer->ip.mtu < 536)
			goto error;
		if ((t_scalar_t) OPT_buffer->ip.mtu > 65535)
			goto error;
		/* FIXME: MTU should be negotiated.  The MTU should be downgraded to the lesser
		   value of what the connection requires or what was specified, but not upgraded. */
	} else {
		OPT_buffer->ip.mtu = ACCEPTOR_id->options.ip.mtu;
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
				goto error;
		}
	} else {
		OPT_buffer->ip.saddr = ACCEPTOR_id->options.ip.saddr;
	}

	/* Here's a problem: we don't realy have any destination addresses yet, so we can't check
	   at this point. */

	if (t_tst_bit(_T_BIT_IP_DADDR, OPT_buffer->flags)) {
		if (rnum > 0) {
			/* Specified destination addresses must be in the responding address list. */
			for (i = 0; i < rnum; i++)
				if (RES_buffer[i].sin_addr.s_addr == OPT_buffer->ip.daddr)
					break;
			if (i >= rnum)
				goto error;
		} else {
			/* If no responding address list is provided (rnum == 0), the destination
			   address must be the source address of the connection indication. */
			if (OPT_buffer->ip.daddr != iph->saddr)
				goto error;
		}
	} else {
		OPT_buffer->ip.daddr = rnum ? RES_buffer[0].sin_addr.s_addr : iph->saddr;
	}

	ACCEPTOR_id->dport = rnum ? RES_buffer[0].sin_port : uh->source;

	err = TBADADDR;
	if (ACCEPTOR_id->dport == 0 && (ACCEPTOR_id->bport != 0 || ACCEPTOR_id->sport != 0))
		goto error;
	if (ACCEPTOR_id->dport != 0 && ACCEPTOR_id->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port
		   number. */
		goto error;

	if (rnum > 0) {
		for (i = 0; i < rnum; i++) {
			struct rtable *rt = NULL;

			if ((err = ip_route_output(&rt, RES_buffer[i].sin_addr.s_addr, 0, 0, 0))) {
				while (--i >= 0)
					dst_release(XCHG(&ACCEPTOR_id->daddrs[i].dst, NULL));
				goto error;
			}
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
			goto error;
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

	/* try to place in connection hashes with conflict checks */
	if ((err = tp_conn_check(ACCEPTOR_id, iph->protocol)) != 0)
		goto error;

	if (dp != NULL)
		if (unlikely((err = tp_senddata(tp, tp->dport, OPT_buffer, dp)) != QR_ABSORBED))
			goto error;
	if (SEQ_number != NULL) {
		bufq_unlink(&tp->conq, SEQ_number);
		freeb(XCHG(&SEQ_number, SEQ_number->b_cont));
		/* queue any pending data */
		while (SEQ_number)
			put(ACCEPTOR_id->oq, XCHG(&SEQ_number, SEQ_number->b_cont));
	}

	/* store negotiated qos values */
	ACCEPTOR_id->options.xti.priority = OPT_buffer->xti.priority;
	ACCEPTOR_id->options.ip.protocol = OPT_buffer->ip.protocol;
	ACCEPTOR_id->options.ip.ttl = OPT_buffer->ip.ttl;
	ACCEPTOR_id->options.ip.tos = OPT_buffer->ip.tos;
	ACCEPTOR_id->options.ip.mtu = OPT_buffer->ip.mtu;
	ACCEPTOR_id->options.ip.saddr = OPT_buffer->ip.saddr;
	ACCEPTOR_id->options.ip.daddr = OPT_buffer->ip.daddr;
	return (QR_ABSORBED);

      error:
	return (err);
}

/**
 * tp_disconnect - disconnect from the connection hashes
 * @q: active queue (write queue)
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

	if (dp != NULL) {
		err = tp_senddata(tp, tp->dport, &tp->options, dp);
		if (unlikely(err != QR_ABSORBED))
			goto error;
	}
	if (SEQ_number != NULL) {
		bufq_unlink(&tp->conq, SEQ_number);
		freemsg(SEQ_number);
	}
	if ((hp = tp->chash) != NULL) {
		write_lock_bh(&hp->lock);
		if ((*tp->cprev = tp->cnext))
			tp->cnext->cprev = tp->cprev;
		tp->cnext = NULL;
		tp->cprev = &tp->cnext;
		tp->chash = NULL;
		tp->dport = tp->sport = 0;
		tp->dnum = tp->snum = 0;
		tp_put(tp);
		write_unlock_bh(&hp->lock);
	}
	return (QR_ABSORBED);
      error:
	return (err);
}

/*
 *  Addressing:
 *
 *  NSAPs (Protocol IDs) are IP protocol numbers.  NSAP addresses consist of a port number and a
 *  list of IP addreses.  If the port number is zero, any port number is used.  Initially it is only
 *  supporting IPv4.
 *
 *  There are two types of providers: connectionless and connection oriented.
 *
 *  - Connectionless providers will start delivering packets after the bind.
 *
 *  - When the NS provider is bound to multiple protocol ids, or bound or connected to multiple
 *    addresses, data will be delivered as N_DATA_IND primitives that contain the protocol id index,
 *    destination address index, and source addresses index in the DATA_xfer_flags as the highest
 *    order byte, next highest order byte and so on.  An index of 0 indicates the first bound
 *    protocol id, source address or destination address.  In this way, the high order 3 bytes of
 *    the DATA_xfer_flags are coded all zeros in the non-multiple case.
 *
 *    The NS user is also permitted to send N_DATA_REQ primitives that contain the protocol id,
 *    destination address, and source addresses, similarly encoded in the DATA_xfer_flags.  Invalid
 *    indexes will error the stream.
 *
 *  - Connection oriented provider bound as listening will start delivering packets as connection
 *    indications after the bind.  The user can either reject these with a disconnect request or can
 *    accept them with a connection response, with a slight difference from normal NPI: the
 *    responding address in the connection response is the list of peer addresses to which to
 *    establish a connection connect rather than the local responding address.
 *
 *    If the connection is accepted on the listening stream, no further connection indications will
 *    be delivered.  If accepted on another stream, further connection indications (belonging to
 *    annother association) will be delivered.
 *
 *  - Connection oriented providers bound as non-listening will deliver packets only after a
 *    successful connection establishment operation.  Connection establishment does not doe anything
 *    except to provide a port and list of addresses from which the provider is to deliver packets.
 *    This can be useful for RTP and for establishing endpoint communication with SCTP.
 */

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
STATIC noinline streams_fastcall int
m_flush(queue_t *q, const int how, const int band)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;

	if (unlikely((mp = ss7_allocb(q, 2, BPRI_HI)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = how;
	*mp->b_wptr++ = band;
	putnext(tp->oq, mp);
	return (QR_DONE);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error: deliver an M_ERROR message upstream
 * @q: a queue in the queue pair (write queue)
 * @error: the error to deliver
 */
STATIC noinline streams_fastcall __unlikely int
m_error(queue_t *q, const int error)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, 2, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_ERROR;
		mp->b_wptr[0] = mp->b_wptr[1] = error;
		mp->b_wptr += 2;
		/* make sure the stream is disconnected */
		if (tp->chash != NULL) {
			tp_disconnect(tp, NULL, NULL, N_REASON_UNDEFINED, NULL);
			tp_set_state(tp, TS_IDLE);
		}
		/* make sure the stream is unbound */
		if (tp->bhash != NULL) {
			tp_unbind(tp);
			tp_set_state(tp, TS_UNBND);
		}
		_printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, tp, error));
		qreply(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * m_hangup: deliver an M_HANGUP message upstream
 * @q: a queue in the queue pair (write queue)
 */
STATIC noinline streams_fastcall __unlikely int
m_hangup(queue_t *q)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, 0, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_HANGUP;
		/* make sure the stream is disconnected */
		if (tp->chash != NULL) {
			tp_disconnect(tp, NULL, NULL, N_REASON_UNDEFINED, NULL);
			tp_set_state(tp, TS_IDLE);
		}
		/* make sure the stream is unbound */
		if (tp->bhash != NULL) {
			tp_unbind(tp);
			tp_set_state(tp, TS_UNBND);
		}
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
STATIC noinline streams_fastcall __unlikely int
te_error_reply(queue_t *q, const long error)
{
	switch (error) {
	case 0:
	case 1:
	case 2:
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
STATIC noinline streams_fastcall int
te_info_ack(queue_t *q)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;
	size_t size = sizeof(*p);
	int err;

	err = -ENOBUFS;
	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto error;

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
	return (QR_DONE);

      error:
	return (err);
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
STATIC noinline streams_fastcall int
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
	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
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
STATIC noinline streams_fastcall __unlikely int
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
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL)
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
 * @SEQ_number: sequence number (i.e. connection/reset indication sequence number)
 * @ACCEPTOR_id: token (i.e. connection response token)
 * @flags: mangement flags, connection flags, disconnect reason, etc.
 * @dp: user data
 */
STATIC streams_fastcall __hot_put int
te_ok_ack(queue_t *q, const t_scalar_t CORRECT_prim, const struct sockaddr_in *ADDR_buffer,
	  const socklen_t ADDR_length, void *OPT_buffer, mblk_t *SEQ_number, struct tp *ACCEPTOR_id,
	  const t_uscalar_t flags, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_ok_ack *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);
	int err = QR_DONE;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	mp->b_wptr += sizeof(*p);
	switch (tp_get_state(tp)) {
#if 0
	case TS_WACK_OPTREQ:
		break;
#endif
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
#if 0
	case TS_WACK_CREQ:
		/* FIXME: don't do this, use te_conn_con() instead */
		if ((err = tp_connect(tp, ADDR_buffer, ADDR_length, OPT_buffer, 0)))
			goto error;
		tp_set_state(tp, TS_WCON_CREQ);
		if ((err = tp_senddata(tp, tp->dport, &tp->options, NULL))) {
			tp_disconnect(tp, ADDR_buffer, SEQ_number, flags, dp);
			goto error;
		}
		break;
#endif
	case TS_WACK_CRES:
		if (tp != ACCEPTOR_id)
			ACCEPTOR_id->i_oldstate = tp_get_state(ACCEPTOR_id);
		tp_set_state(ACCEPTOR_id, TS_DATA_XFER);
		err = tp_passive(tp, ADDR_buffer, ADDR_length, OPT_buffer, SEQ_number,
				 ACCEPTOR_id, flags, dp);
		if (unlikely(err != QR_ABSORBED)) {
			tp_set_state(ACCEPTOR_id, ACCEPTOR_id->i_oldstate);
			goto error;
		}
		if (tp != ACCEPTOR_id)
			tp_set_state(tp, bufq_length(&tp->conq) > 0 ? TS_WRES_CIND : TS_IDLE);
		err = QR_TRIMMED;
		break;
#if 0
	case NS_WACK_RRES:
		err = np_reset_rem(np, N_USER, N_REASON_UNDEFINED);
		if (unlikely(err != 0))
			goto free_error;
		np_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
		break;
#endif
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		err = tp_disconnect(tp, ADDR_buffer, SEQ_number, flags, dp);
		if (unlikely(err != QR_ABSORBED))
			goto error;
		tp_set_state(tp, bufq_length(&tp->conq) > 0 ? TS_WRES_CIND : TS_IDLE);
		break;
	default:
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
#if 0
		if (CORRECT_prim == N_OPTMGMT_REQ) {
			err = np_optmgmt(np, QOS_buffer, flags);
			if (unlikely(err != 0))
				goto free_error;
			break;
		}
#endif
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
STATIC noinline streams_fastcall int
te_conn_con(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	    struct tp_options *OPT_buffer, t_uscalar_t CONN_flags)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp = NULL;
	struct T_conn_con *p;
	int err;
	size_t OPT_length = 0;		// FIXME: t_size_conn_con_opts(OPT_buffer);
	size_t size = sizeof(*p) + RES_length + OPT_length;

	tp_set_state(tp, TS_WCON_CREQ);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
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
	return (QR_DONE);

      free_error:
	freeb(mp);
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);

}

#if 0
/**
 * ne_reset_con - generate a N_RESET_CON message
 * @q: active queue in queue pair (write queue)
 * @RESET_orig: origin of the reset
 * @RESET_reason: reason for the reset
 * @dp: message containing IP packet
 *
 * An N_RESET_CON message is sent only when the reset completes successfully.
 */
STATIC streams_fastcall int
ne_reset_con(queue_t *q, const np_ulong RESET_orig, const np_ulong RESET_reason, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_reset_con_t *p;
	size_t size = sizeof(*p);
	int err;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely((err = np_reset_loc(np, RESET_orig, RESET_reason, dp)) != 0))
		goto free_error;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	np_set_state(np, np->resinds > 0 ? NS_WRES_RIND : NS_DATA_XFER);
	_printd(("%s: <- N_RESET_CON\n", DRV_NAME));
	qreply(q, mp);
	return (QR_DONE);

      free_error:
	freeb(mp);
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
#endif

/**
 * te_conn_ind - generate a T_CONN_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * We generate connection indications to Streams that are bound as listening to an address including
 * the destination address of the IP packet, where no connection exists for the source address of
 * the IP packet.
 */
STATIC noinline streams_fastcall __hot_get int
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

	if (unlikely(tp_get_statef(tp) & ~(TSF_IDLE | TSF_WRES_CIND | TSF_WACK_CRES)))
		goto discard;

	spin_lock_bh(&tp->conq.q_lock);
	for (cp = bufq_head(&tp->conq); cp; cp = cp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) cp->b_rptr;
		struct udphdr *uh2 = (struct udphdr *) (cp->b_rptr + (iph->ihl << 2));

		if (iph->protocol == iph2->protocol
		    && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr
		    && uh->source == uh2->source && uh->dest == uh2->dest) {
			/* already have a connection indication, link the data */
			linkb(cp, SEQ_number);
			spin_unlock_bh(&tp->conq.q_lock);
			goto absorbed;
		}
	}
	spin_unlock_bh(&tp->conq.q_lock);

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

	if (unlikely((cp = ss7_dupmsg(q, SEQ_number)) == NULL))
		goto enobufs;

	SRC_length = sizeof(struct sockaddr_in);
	OPT_length = t_opts_size(tp, SEQ_number);
	size = sizeof(*p) + SRC_length + OPT_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
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

/**
 * te_discon_ind - TE_DISCON_IND TC disconnected event
 * @q: active queue (read queue)
 * @dp: message containing ICMP packet
 *
 * The N_DISCON_IND is sent when we encounter an error on a connection oriented Stream, i.e. as a
 * result of receiving an ICMP error.  For multihomed hosts, we only do this if all destination
 * addresses have errors, otherwise, we just perform a reset for the affected destination.
 */
STATIC noinline streams_fastcall __hot_get int
te_discon_ind(queue_t *q, const struct sockaddr_in *RES_buffer, const socklen_t RES_length,
	      const t_uscalar_t DISCON_reason, const mblk_t *SEQ_number, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_discon_ind *p;
	size_t size = sizeof(*p);

	if (unlikely
	    (tp_not_state(tp, (TSF_WRES_CIND | TSF_DATA_XFER | TSF_WREQ_ORDREL | TSF_WIND_ORDREL))))
		goto discard;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = DISCON_reason;
	p->SEQ_number = (t_uscalar_t) (long) SEQ_number;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	_printd(("%s: %p: <- T_DISCON_IND\n", DRV_NAME, tp));
	putnext(q, mp);
	return (QR_ABSORBED);

      enobufs:
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * te_discon_ind_icmp - TE_DISCON_IND event resulting from ICMP message
 * @q: active queue in queue pair
 * @mp: the ICMP message
 */
STATIC noinline streams_fastcall __unlikely int
te_discon_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in res_buf, *RES_buffer = &res_buf;
	t_uscalar_t DISCON_reason;
	mblk_t **conpp, *SEQ_number;
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

	DISCON_reason = ((t_uscalar_t) icmp->type << 8) | ((t_uscalar_t) icmp->code);

	/* check for outstanding connection indications for responding address */
	for (conpp = &tp->conq.q_head; (*conpp); conpp = &(*conpp)->b_next) {
		struct iphdr *iph2 = (struct iphdr *) (*conpp)->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}
	SEQ_number = (*conpp);

	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = te_discon_ind(q, RES_buffer, sizeof(*RES_buffer),
				 DISCON_reason, SEQ_number, mp)) < 0)
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

#if 0
/**
 * te_data_ind - generate a T_DATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Very fast.  In fact, we could just pass the raw M_DATA blocks upstream.  We leave the IP header
 * in the block.
 */
STATIC INLINE streams_fastcall __hot_get int
te_data_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;
	const size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DATA_IND;
	/* TODO: here we can set some info like ECN... */
	p->MORE_flag = 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- T_DATA_IND\n", DRV_NAME, TP_PRIV(q)));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * te_exdata_ind - generate a T_EXDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC INLINE streams_fastcall __hot_get int
te_exdata_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!bcanputnext(q, 1)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- T_EXDATA_IND\n", DRV_NAME, TP_PRIV(q)));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
#endif

/**
 * te_unitdata_ind - send a T_UNITDATA_IND upstream
 * @q: read queue
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block contains the IP, header and payload starting at
 * dp->b_datap->db_base.  The message payload starts at dp->b_rptr.  This function extracts IP
 * header information and uses it to create options.
 */
STATIC INLINE streams_fastcall __hot_get int
te_unitdata_ind(queue_t *q, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_unitdata_ind *p;
	struct sockaddr_in *SRC_buffer;
	const t_scalar_t SRC_length = sizeof(*SRC_buffer);
	t_scalar_t OPT_length = t_opts_size_ud(tp, dp);
	size_t size = sizeof(*p) + SRC_length + OPT_length;
	struct iphdr *iph;
	struct udphdr *uh;

	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto discard;
	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + SRC_length : 0;
	mp->b_wptr += sizeof(*p);

	iph = (struct iphdr *) dp->b_rptr;
	uh = (struct udphdr *) (dp->b_rptr + (iph->ihl << 2));

	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = uh->source;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += SRC_length;
	}
	if (unlikely(OPT_length != 0)) {
		t_opts_build_ud(tp, dp, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	/* pull IP header and UDP header */
	dp->b_rptr = (unsigned char *) (uh + 1);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, tp));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

#if 1
/**
 * te_optdata_ind - send a T_OPTDATA_IND upstream
 * @q: read queue
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block containst the IP header and payload, starting at
 * dp->b_datap->db_base.  The IP message payload starts at dp->b_rptr.  This function extracts IP
 * header information and uses it to create options.
 */
STATIC noinline streams_fastcall __hot_get int
te_optdata_ind(queue_t *q, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_optdata_ind *p;
	t_scalar_t OPT_length = t_opts_size_ud(tp, dp);

	if (unlikely(tp_get_statef(tp) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto discard;
	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + OPT_length, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) : 0;
	if (unlikely(OPT_length != 0)) {
		t_opts_build_ud(tp, dp, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	dp->b_datap->db_type = M_DATA;
	mp->b_cont = dp;
	_printd(("%s: %p: <= T_OPTDATA_IND\n", DRV_NAME, tp));
	putnext(q, mp);
	return (QR_ABSORBED);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}
#endif

/**
 * te_uderror_ind - generate a T_UDERROR_IND message
 * @q: active queue in queue pair (read or write queue)
 * @DEST_buffer: pointer to destination address
 * @OPT_buffer: pointer to options buffer
 * @OPT_length: length of options in buffer
 * @ERROR_type: error number
 * @dp: message containing user data of errored packet
 */
STATIC noinline __unlikely int
te_uderror_ind(queue_t *q, const struct sockaddr_in *DEST_buffer, const unsigned char *OPT_buffer,
	       const size_t OPT_length, const t_uscalar_t ERROR_type, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_uderror_ind *p;
	t_uscalar_t DEST_length = DEST_buffer ? sizeof(*DEST_buffer) : 0;
	size_t size = sizeof(*p) + DEST_length + OPT_length;

	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto discard;
	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(tp->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + DEST_length : 0;
	p->ERROR_type = ERROR_type;
	mp->b_wptr += sizeof(*p);
	if (DEST_length) {
		bcopy(DEST_buffer, mp->b_wptr, DEST_length);
		mp->b_wptr += DEST_length;
	}
	if (OPT_length) {
		bcopy(OPT_buffer, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	if (dp != NULL) {
		mp->b_cont = dp;
		dp->b_datap->db_type = M_DATA;	/* was M_ERROR in some cases */
	}
	_printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, tp));
	putnext(tp->oq, mp);
	return (QR_ABSORBED);
      ebusy:
	freeb(mp);
	ptrace(("%s: ERROR: Flow controlled\n", DRV_NAME));
	return (-EBUSY);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * te_uderror_ind_icmp - generate an T_UDERROR_IND message from an ICMP packet
 * @q: active queue in pair (read queue)
 * @mp: message containing ICMP packet
 *
 * There is another reason for issuing an T_UDERROR_IND and that is Explicit Congestion
 * Notification, but there is no ICMP message associated with that and it has not yet been coded:
 * probably need an ne_uderror_ind_ecn() function.
 */
STATIC noinline streams_fastcall __unlikely int
te_uderror_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in dst_buf, *DEST_buffer = &dst_buf;
	t_uscalar_t ERROR_type;
	unsigned char *hidden;
	unsigned char OPT_buffer[128];
	size_t OPT_length;
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
	ERROR_type = ((t_uscalar_t) icmp->type << 8) | ((t_uscalar_t) icmp->code);
	/* hide ICMP header */
	hidden = mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	OPT_length = t_errs_size(tp, mp);
	t_errs_build(tp, mp, OPT_buffer, OPT_length);
	if ((err = te_uderror_ind(q, DEST_buffer, OPT_buffer, OPT_length, ERROR_type, mp)) < 0)
		mp->b_rptr = hidden;
	return (err);
}

STATIC noinline streams_fastcall __unlikely int
te_uderror_reply(queue_t *q, const struct sockaddr_in *DEST_buffer, const unsigned char *OPT_buffer,
		 const size_t OPT_length, t_scalar_t ERROR_type, mblk_t *db)
{
	switch (ERROR_type) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (ERROR_type);
	case 0:
		return (ERROR_type);
	case -EADDRINUSE:
		ERROR_type = TADDRBUSY;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		ERROR_type = TNOADDR;
		break;
	case -EACCES:
		ERROR_type = TACCES;
		break;
	case -EOPNOTSUPP:
		ERROR_type = TNOTSUPPORT;
		break;
	case TBADOPT:
	case TBADADDR:
		break;
	default:
	case TOUTSTATE:
	case TBADDATA:
	case -EINVAL:
	case -EFAULT:
	case -EMSGSIZE:
		return te_error_reply(q, -EPROTO);
	}
	return te_uderror_ind(q, DEST_buffer, OPT_buffer, OPT_length, ERROR_type, db);
}

#if 0
/**
 * ne_reset_ind - generate a N_RESET_IND message
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
	struct np *np = TP_PRIV(q);
	mblk_t *mp, *bp;
	N_reset_ind_t *p;
	const size_t size = sizeof(*p);
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct icmphdr *icmp = (struct icmphdr *) (dp->b_rptr + (iph->ihl << 2));

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
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = N_PROVIDER;
	mp->b_wptr += sizeof(*p);
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
			/* If the reason was fragmentation needed, then we sent a packet that was
			   too large and so we might need to adjust down our NSDU_size as well as
			   the np->qos.mtu that is being reported for the Stream.  When the user
			   receives this error, it is their responsibility to check sizes again
			   with N_INFO_REQ. */
			if (np->qos.mtu > icmp->un.frag.mtu)
				np->qos.mtu = icmp->un.frag.mtu;
			if (np->info.NIDU_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.NIDU_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.NSDU_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.NSDU_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.ENSDU_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.ENSDU_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.CDATA_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.CDATA_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.DDATA_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.DDATA_size = np->qos.mtu - sizeof(struct iphdr);
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
	/* save original in reset indication list */
	dp->b_next = np->resq;
	np->resq = dp;
	np->resinds++;
	_printd(("%s: <- N_RESET_IND\n", DRV_NAME));
	putnext(q, mp);
      discard:
	return (QR_DONE);
      enobufs:
	if (bp)
		freemsg(bp);
	return (-ENOBUFS);
}
#endif

#if 1
/**
 * t_optmgmt_ack - send a T_OPTMGMT_ACK upstream
 * @q: a queue in the queue pair
 * @flags: result flags
 * @req: requested options pointer from T_OPTMGMT_REQ
 * @req_len: length of requested options
 * @opt_len: size of output options
 *
 * Note: opt_len is conservative but might not be actual size of the output options.  This will be
 * adjusted when the option buffer is built.
 */
STATIC noinline int
t_optmgmt_ack(queue_t *q, t_scalar_t flags, const unsigned char *req, const size_t req_len,
	      size_t opt_len)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	if ((flags = t_build_options(tp, req, req_len, mp->b_wptr, &opt_len, flags)) < 0) {
		freemsg(mp);
		return (flags);
	}
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		mp->b_wptr += opt_len;
	}
#ifdef TS_WACK_OPTREQ
	if (tp_get_state(tp) == TS_WACK_OPTREQ)
		tp_set_state(tp, TS_IDLE);
#endif
	_printd(("%s: %p: <- T_OPTMGMT_ACK\n", DRV_NAME, tp));
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif

#ifdef T_ADDR_ACK
/**
 * t_addr_ack - send a T_ADDR_ACK upstream
 * @q: a queue in the queue pair
 * @LOCADDR_buffer: local address
 * @LOCADDR_length: local address length
 * @REMADDR_buffer: remote address
 * @REMADDR_length: remote address length
 */
STATIC noinline int
t_addr_ack(queue_t *q, const struct sockaddr_in *LOCADDR_buffer, const t_uscalar_t LOCADDR_length,
	   const struct sockaddr_in *REMADDR_buffer, const t_uscalar_t REMADDR_length)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t size = sizeof(*p) + LOCADDR_length + REMADDR_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = LOCADDR_length;
	p->LOCADDR_offset = LOCADDR_length ? sizeof(*p) : 0;
	p->REMADDR_length = REMADDR_length;
	p->REMADDR_offset = REMADDR_length ? sizeof(*p) + LOCADDR_length : 0;
	mp->b_wptr += sizeof(*p);
	if (LOCADDR_length) {
		bcopy(LOCADDR_buffer, mp->b_wptr, LOCADDR_length);
		mp->b_wptr += LOCADDR_length;
	}
	if (REMADDR_length) {
		bcopy(REMADDR_buffer, mp->b_wptr, REMADDR_length);
		mp->b_wptr += REMADDR_length;
	}
	_printd(("%s: %p: <- T_ADDR_ACK\n", DRV_NAME, tp));
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif				/* T_ADDR_ACK */

#ifdef T_CAPABILITY_ACK
/**
 * t_capability_ack - send a T_CAPABILITY_ACK upstream
 * @q: a queue in the queue pair
 * @caps: capability bits
 * @type: STREAMS message type M_PROTO or M_PCPROTO
 */
STATIC noinline int
t_capability_ack(queue_t *q, const t_uscalar_t caps, const int type)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *mp;
	struct T_capability_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = type;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = caps & (TC1_INFO | TC1_ACCEPTOR_ID);
	p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (long) tp->oq : 0;
	mp->b_wptr += sizeof(*p);
	if (caps & TC1_INFO) {
		p->INFO_ack = tp->info;
	} else
		bzero(&p->INFO_ack, sizeof(p->INFO_ack));
	_printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, tp));
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif				/* T_CAPABILITY_ACK */

/*
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 */
/*
 *  TS User -> TS Provider (Request and Response) primitives
 *  ========================================================
 */
/**
 * te_info_req - TE_INFO_REQ information request event
 * @q: active queue in pair (write queue)
 * @mp: T_INFO_REQ message
 */
STATIC noinline int
te_info_req(queue_t *q, mblk_t *mp)
{
	struct T_info_req *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_INFO_REQ);
	return te_info_ack(q);
      error:
	return (err);
}

/*
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 */
/**
 * te_bind_req - T_BIND_REQ bind a TS user to transport address event
 * @q: active queue in pair (write queue)
 * @mp: T_BIND_REQ message
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
STATIC noinline int
te_bind_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	t_uscalar_t ADDR_length;
	int type;
	const struct T_bind_req *p;
	struct sockaddr_in ADDR_buffer[8] = { {AF_INET,}, };
	size_t anum = 0;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_BIND_REQ);
	err = TOUTSTATE;
	if (unlikely(tp_get_state(tp) != TS_UNBND))
		goto error;
	tp_set_state(tp, TS_WACK_BREQ);
	err = TBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto error;
	if ((ADDR_length = p->ADDR_length) == 0) {
		ADDR_length = sizeof(ADDR_buffer[0]);
		anum = 1;
	} else {
		err = TBADADDR;
		if (unlikely((anum = ADDR_length / sizeof(ADDR_buffer[0])) < 1 || anum > 8))
			goto error;
		if (unlikely(ADDR_length % sizeof(ADDR_buffer[0]) != 0))
			goto error;
		bcopy(mp->b_rptr + p->ADDR_offset, ADDR_buffer, ADDR_length);
		if (unlikely(ADDR_buffer[0].sin_family != AF_INET
			     && ADDR_buffer[0].sin_family != 0))
			goto error;
	}
	type = inet_addr_type(ADDR_buffer[0].sin_addr.s_addr);
	err = TNOADDR;
	if (sysctl_ip_nonlocal_bind == 0 && ADDR_buffer[0].sin_addr.s_addr != INADDR_ANY
	    && type != RTN_LOCAL && type != RTN_MULTICAST && type != RTN_BROADCAST)
		goto error;
	tp->bport = ntohs(ADDR_buffer[0].sin_port);
	/* check for bind to privileged port */
	err = TACCES;
	if (tp->bport && tp->bport < PROT_SOCK && !capable(CAP_NET_BIND_SERVICE))
		goto error;
	if (unlikely((err = te_bind_ack(q, ADDR_buffer, ADDR_length, p->CONIND_number)) != 0))
		goto error;
	return (QR_DONE);
      error:
	return te_error_ack(q, T_BIND_REQ, err);
}

/**
 * te_unbind_req - process a T_UNBIND_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC noinline int
te_unbind_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	int err;

	err = TOUTSTATE;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto error;
	tp_set_state(tp, TS_WACK_UREQ);
	if (unlikely((err = te_ok_ack(q, T_UNBIND_REQ, NULL, 0, NULL, NULL, NULL, 0, NULL)) != 0))
		goto error;
	return (QR_DONE);
      error:
	return te_error_ack(q, T_UNBIND_REQ, err);
}

/**
 * te_unitdata_req_slow - process a T_UNITDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 *
 * The destination address, DEST_length and DEST_offset, must be specified.  The port number is not
 * significant: the attached M_DATA blocks begin with the transport header which will include any
 * port numbers as required.  The IP header information for the transmitted packet can be set with
 * options to T_OPTMGMT_REQ and will be taken from the Stream private structure
 * for all packets sent.  The TPI-IP provider will request that the Stream head provide an
 * additional write offset of 20 bytes to accomodate the IP header.
 */
STATIC noinline streams_fastcall __unlikely int
te_unitdata_req_slow(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	size_t dlen;
	struct T_unitdata_req *p;
	struct sockaddr_in dst_buf, *DEST_buffer = NULL;
	unsigned char *OPT_buffer = NULL;
	size_t OPT_length = 0;
	int err;
	mblk_t *dp = mp->b_cont;
	uint32_t daddr;
	uint16_t dport;
	struct tp_options opts;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_UNITDATA_REQ);
	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_COTS || tp->info.SERV_type == T_COTS_ORD))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_CLTS))
		goto error;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto error;
	err = TNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = TBADADDR;
	if (unlikely((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)))
		goto error;
	if (unlikely(p->DEST_length != sizeof(struct sockaddr_in)))
		goto error;
	/* avoid alignment problems */
	DEST_buffer = &dst_buf;
	bcopy(mp->b_rptr + p->DEST_offset, DEST_buffer, p->DEST_length);
	if (unlikely(DEST_buffer->sin_family != AF_INET))
		goto error;
	if (unlikely((daddr = DEST_buffer->sin_addr.s_addr) == INADDR_ANY))
		goto error;
	if (unlikely((dport = DEST_buffer->sin_port) == 0))
		goto error;
	err = TBADDATA;
	if (unlikely(dp == NULL))
		goto error;
	if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > tp->info.TSDU_size))
		goto error;
	err = TBADOPT;
	if (unlikely(p->OPT_length > tp->info.OPT_size))
		goto error;
	err = TBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	OPT_buffer = mp->b_rptr + p->OPT_offset;
	OPT_length = p->OPT_length;
	opts = tp->options;
	opts.ip.daddr = daddr;
	opts.flags[0] = 0;
	if (unlikely(OPT_length != 0))
		if (unlikely((err = t_opts_parse_ud(OPT_buffer, OPT_length, &opts)) != 0))
			goto error;
	if (unlikely((err = tp_senddata(tp, dport, &opts, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      error:
	err = te_uderror_reply(q, DEST_buffer, OPT_buffer, OPT_length, err, dp);
	if (likely(err == QR_ABSORBED))
		return (QR_TRIMMED);
	return (err);
}

/**
 * te_unitdata_req - process a T_UNITDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 *
 * Optimize fast path for no options and correct behaviour.  This should run much faster than the
 * old plodding way of doing things.
 */
STATIC INLINE streams_fastcall __hot_put int
te_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_unitdata_req *p;
	mblk_t *dp;
	int err;
	struct sockaddr_in dst_buf;

	prefetch(tp);
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	p = (typeof(p)) mp->b_rptr;
	prefetch(p);
	dassert(p->PRIM_type == T_UNITDATA_REQ);
	if (unlikely(tp->info.SERV_type == T_COTS || tp->info.SERV_type == T_COTS_ORD))
		goto go_slow;
	if (unlikely(tp->info.SERV_type != T_CLTS))
		goto go_slow;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto go_slow;
	if (unlikely(p->DEST_length == 0))
		goto go_slow;
	if (unlikely((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)))
		goto go_slow;
	if (unlikely(p->DEST_length != sizeof(struct sockaddr_in)))
		goto go_slow;
	/* avoid alignment problems */
	bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, p->DEST_length);
	if (unlikely(dst_buf.sin_family != AF_INET))
		goto go_slow;
	if (unlikely(dst_buf.sin_addr.s_addr == INADDR_ANY))
		goto go_slow;
	if (unlikely(dst_buf.sin_port == 0))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	prefetch(dp);
	if (unlikely(p->OPT_length != 0))
		goto go_slow;
	{
		size_t dlen;
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > tp->info.TSDU_size))
			goto go_slow;
	}
	if (unlikely((err = tp_senddata(tp, dst_buf.sin_port, &tp->options, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      error:
	err = te_uderror_reply(q, &dst_buf, NULL, 0, err, dp);
	if (likely(err == QR_ABSORBED))
		return (QR_TRIMMED);
	return (err);
      go_slow:
	return te_unitdata_req_slow(q, mp);
}

/**
 * te_conn_req - TE_CONN_REQ connect TS user event
 * @q: active queue (write queue)
 * @mp: T_CONN_REQ message
 *
 * Request that a connection be made to (possibly supplemental) destination addresses.  The
 * addresses are formatted as an array of sockaddr_in structures.
 *
 * Fields are as follows:
 *
 * - PRIM_type: always N_CONN_REQ
 * - DEST_length: destination address length
 * - DEST_offset: destination address offset from start of message block
 * - OPT_length: options length
 * - OPT_offset: options offset from start of message block
 *
 * To support multihoming, this event permits an array of destination addresses.  The first address
 * in the array will be the primary address, unless another primary is specified in the options
 * parameters.  The primary address is used for subsequent TE_DATA_REQ and TE_EXDATA_REQ events
 * until changed with a TE_OPTMGMT_REQ event.
 */
STATIC noinline streams_fastcall __hot_put int
te_conn_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_conn_req *p;
	struct tp_options opt_buf = { }, *OPT_buffer = &opt_buf;
	struct sockaddr_in dst_buf[8] = { {AF_INET,}, }, *DEST_buffer = NULL;
	int err;
	mblk_t *dp = mp->b_cont;
	size_t dlen;
	int i;

	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_CONN_REQ);
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_COTS && tp->info.SERV_type != T_COTS_ORD))
		goto error;
	/* Connection requests are not allowed on a listening Stream.  Note that there is a
	   conflict in the NPI specifications here: under the description for N_BIND_REQ, NPI 2.0.0 
	   says: "If a Stream is bound as a listener Stream, it will not be able to initiate
	   connect requests. If the NS user attempts to send an N_CONN_REQ primitive down this
	   Stream, an N_ERROR_ACK message will be sent to the NS user by the NS provider with an
	   error value of NACCESS." Then, under N_BIND_ACK, NPI 2.0.0 says: "A Stream with a
	   negotiated CONIND_number greater than zero may generate connect requests or accept
	   connect indications." */
	err = TACCES;
	if (unlikely(tp->CONIND_number > 0))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto error;
#if 0
	err = NBADFLAG;
	if (unlikely(p->CONN_flags != 0))
		goto error;
#endif
	err = TNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = TBADADDR;
	if (unlikely(p->DEST_length > tp->info.ADDR_size))
		goto error;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
		goto error;
	if (unlikely(p->DEST_length < sizeof(*DEST_buffer)))
		goto error;
	if (unlikely(p->DEST_length > (sizeof(*DEST_buffer) << 3)))
		goto error;
	if (unlikely(p->DEST_length % sizeof(*DEST_buffer) != 0))
		goto error;
	DEST_buffer = dst_buf;
	bcopy(mp->b_rptr + p->DEST_offset, DEST_buffer, p->DEST_length);
	if (unlikely(DEST_buffer[0].sin_family != AF_INET && DEST_buffer[0].sin_family != 0))
		goto error;
	for (i = 0; i < p->DEST_length / sizeof(*DEST_buffer); i++)
		if (unlikely(DEST_buffer[i].sin_addr.s_addr == INADDR_ANY))
			goto error;
	/* set default values in OPT buffer */
	*OPT_buffer = tp->options;
	OPT_buffer->flags[0] = 0;
	if (p->OPT_length != 0) {
		err = TBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
			goto error;
		if (unlikely(p->OPT_length > tp->info.OPT_size))
			goto error;
		if ((err = t_opts_parse(mp->b_rptr + p->OPT_offset, p->OPT_length, &opt_buf)))
			goto error;
	}
	if (dp != NULL) {
		err = TBADDATA;
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > tp->info.CDATA_size))
			goto error;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	err = te_conn_con(q, DEST_buffer, p->DEST_length, OPT_buffer, 0);
	if (unlikely(err != 0))
		goto error;
	/* send data only after connection complete */
	if (dp == NULL)
		return (QR_DONE);
	if (tp_senddata(tp, tp->dport, &tp->options, dp) != QR_ABSORBED)
		goto error;
	return (QR_TRIMMED);	/* tp_senddata() consumed message blocks */
      error:
	return te_error_ack(q, T_CONN_REQ, err);
}

STATIC INLINE streams_fastcall mblk_t *
t_seq_check(struct tp *tp, const t_uscalar_t SEQ_number)
{
	mblk_t *cp;

	spin_lock_bh(&tp->conq.q_lock);
	for (cp = bufq_head(&tp->conq); cp && (t_uscalar_t) (long) cp != SEQ_number;
	     cp = cp->b_next) ;
	spin_unlock_bh(&tp->conq.q_lock);
	usual(cp);
	return (cp);
}
STATIC INLINE streams_fastcall struct tp *
t_tok_check(const t_uscalar_t ACCEPTOR_id)
{
	struct tp *ap;

	read_lock_bh(&master.lock);
	for (ap = master.tp.list; ap && (t_uscalar_t) (long) ap->oq != ACCEPTOR_id; ap = ap->next) ;
	read_unlock_bh(&master.lock);
	usual(ap);
	return (ap);
}

/**
 * te_conn_res - TE_CONN_RES accept previous connection indication event
 * @q: active queue (write queue)
 * @mp: T_CONN_RES message
 *
 * An TE_CONN_IND event is generated when an IP message arrives for an address bound to an T_COTS
 * Stream and there is not yet a connection for the source address in the message.  Outstanding
 * connection indications are queued against the Stream with a sequence number assigned (derived
 * from the address of the connecting message held in the connection indication list).  The
 * TE_CONN_RES event is generated by the user to accept a connection indication event from the
 * connection indication queue and to perform a passive connection (TE_PASS_CON event) on the
 * indicated Stream.  In addition, the user can set options for the Stream to which a passive
 * connection is made and for any reponse message (data attached to the T_CONN_RES message).
 *
 * There is a deviation here from the TPI specifications: the responding address(es) in the
 * T_CONN_RES primitive contains the list of destination address(es) to which to form the
 * connection.  If no responding addresses are provided, then the destination address is the source
 * address from the connection indication.
 */
STATIC noinline streams_fastcall int
te_conn_res(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q), *ACCEPTOR_id = tp;
	struct T_conn_res *p;
	struct tp_options opt_buf = { }, *OPT_buffer = NULL;
	mblk_t *dp, *SEQ_number;
	size_t dlen;
	int err;

	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_CONN_RES);
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_COTS && tp->info.SERV_type != T_COTS_ORD))
		goto error;
	if (unlikely(tp_get_state(tp) != TS_WRES_CIND))
		goto error;
	if (p->OPT_length != 0) {
		err = TBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
			goto error;
		if (unlikely(p->OPT_length > tp->info.OPT_size))
			goto error;
		opt_buf = ACCEPTOR_id->options;
		opt_buf.flags[0] = 0;
		OPT_buffer = &opt_buf;
		if ((err = t_opts_parse(mp->b_rptr + p->OPT_offset, p->OPT_length, OPT_buffer)))
			goto error;
	}
	err = TBADDATA;
	if ((dp = mp->b_cont))
		if (unlikely((dlen = msgsize(dp)) == 0 || dlen > tp->info.CDATA_size))
			goto error;
	err = TBADSEQ;
	if (unlikely(p->SEQ_number == 0))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp_not_state(tp, TSF_WRES_CIND) || bufq_length(&tp->conq) < 1))
		goto error;
	err = TBADSEQ;
	if (unlikely((SEQ_number = t_seq_check(tp, p->SEQ_number)) == NULL))
		goto error;
	if (p->ACCEPTOR_id != 0) {
		err = TBADF;
		if (unlikely((ACCEPTOR_id = t_tok_check(p->ACCEPTOR_id)) == NULL))
			goto error;
		err = TPROVMISMATCH;
		if (unlikely(ACCEPTOR_id->info.SERV_type == T_CLTS))
			goto error;
		err = TRESQLEN;
		if (unlikely(ACCEPTOR_id->CONIND_number > 0))
			goto error;
		err = TOUTSTATE;
		if (tp_get_statef(ACCEPTOR_id) != TS_IDLE)
			/* Later we could auto bind if in TS_UNBND state. Note that the Stream to
			   which we do a passive connection could be already connected: we just are 
			   just adding another address to a multihomed connection.  But this is not 
			   as useful as adding or removing an address with T_OPTMGMT_REQ. */
			goto error;
		err = TPROVMISMATCH;
		if (ACCEPTOR_id->info.SERV_type != T_COTS
		    && ACCEPTOR_id->info.SERV_type != T_COTS_ORD)
			/* Must be connection-oriented Stream. */
			goto error;
		if (ACCEPTOR_id != tp) {
			/* FIXME: do this check in tp_passive() */
			err = TRESADDR;
			if (tp->sport != ACCEPTOR_id->sport
			    || tp->saddrs[0].addr != ACCEPTOR_id->saddrs[0].addr)
				goto error;
		}
	} else {
		err = TBADF;
		if (bufq_length(&tp->conq) > 1)
			goto error;
	}
#if 0
	/* Do this in ok ack */
	{
		struct T_conn_ind *c = (typeof(c)) SEQ_number->b_rptr;

		bcopy(SEQ_number->b_rptr + c->SRC_offset, &ACCEPTOR_id->DEST_buffer, c->SRC_length);
	}
#endif
	/* Ok, all checking done.  Now we need to connect the new address. */
	tp_set_state(tp, TS_WACK_CRES);
	err = te_ok_ack(q, T_CONN_RES, NULL, 0, OPT_buffer, SEQ_number, ACCEPTOR_id, 0, dp);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);	/* user data is not absorbed */
      error:
	return te_error_ack(q, T_CONN_RES, err);
}

/**
 * te_discon_req - process T_DISCON_REQ message
 * @q: active queue (write queue)
 * @mp: the T_DISCON_REQ message
 */
STATIC noinline streams_fastcall int
te_discon_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_discon_req *p;
	struct sockaddr_in *RES_buffer = NULL;
	size_t RES_length = 0;
	mblk_t *dp, *SEQ_number = NULL;
	size_t dlen;
	int err;
	t_uscalar_t state;

	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_DISCON_REQ);
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_COTS && tp->info.SERV_type != T_COTS_ORD))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp_not_state(tp, TSM_CONNECTED)))
		goto error;
#if 0
	if ((RES_length = p->RES_length)) {
		err = -EINVAL;
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + RES_length)
			goto error;
		err = NBADADDR;
		if (RES_length % sizeof(*RES_buffer) != 0)
			goto error;
		RES_buffer = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
	}
#endif
	err = TBADDATA;
	if ((dp = mp->b_cont) != NULL)
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > tp->info.DDATA_size))
			goto error;
	state = tp_get_state(tp);
	err = TBADSEQ;
	if (p->SEQ_number != 0) {
		if (unlikely(state != TS_WRES_CIND))
			goto error;
		if (unlikely((SEQ_number = t_seq_check(tp, p->SEQ_number)) == NULL))
			goto error;
	} else {
		if (unlikely(state == TS_WRES_CIND))
			goto error;
	}
	err = TOUTSTATE;
	switch (state) {
	case TS_WCON_CREQ:
		tp_set_state(tp, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		tp_set_state(tp, TS_WACK_DREQ7);
		break;
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ11);
		break;
	default:
		goto error;
	}
	/* Ok, all checking done.  Now we need to disconnect the address. */
	err = te_ok_ack(q, T_DISCON_REQ, RES_buffer, RES_length, NULL, SEQ_number, NULL, 0, dp);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);	/* user data is not absorbed */
      error:
	return te_error_ack(q, T_DISCON_REQ, err);
}

/**
 * te_write_req - process M_DATA message
 * @q: write queue
 * @mp: the M_DATA message
 *
 * TODO: We should check the MSGDELIM flag and see whether this was a complete write or not.  If
 * not, we should accumulate the M_DATA block in a buffer waiting for a delimited message or final
 * N_DATA_REQ.
 */
STATIC noinline streams_fastcall __unlikely int
te_write_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	size_t dlen;
	int err;

	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_COTS && tp->info.SERV_type != T_COTS_ORD))
		goto error;
	/* Note: If the interface is in the TS_IDLE state when the provider receives the T_DATA_REQ 
	   primitive, then the TS provider should discard the request without generating a fatal
	   error. */
	if (unlikely(tp_chk_state(tp, (TSF_IDLE))))
		goto discard;
	/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
	   destination of the request is an address that does not have an outstanding reset
	   indication. */
	if (unlikely(tp_not_state(tp, TSM_OUTDATA)))
		goto error;
	/* If we are writing we must include the IP header, which is at least 20 bytes, and, if the 
	   Stream is bound to a port, at least the size of a UDP header.  The length of the entire
	   TSDU must not exceed 65535 bytes. */
	err = TBADDATA;
	if (unlikely((dlen = msgsize(mp)) == 0
		     || dlen > tp->info.TIDU_size || dlen > tp->info.TSDU_size))
		goto error;
	if (unlikely((err = tp_senddata(tp, tp->dport, &tp->options, mp)) != QR_ABSORBED))
		goto error;
	return (QR_ABSORBED);	/* tp_senddata() consumed message block */
      discard:
	return (QR_DONE);	/* tp_senddata() did not consume message blocks */
      error:
	return te_error_reply(q, -EPROTO);
}

/**
 * te_data_req - process a T_DATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 *
 * Unfortunately, there is no standard way of specifying destination and source addreses for
 * multihomed hosts.  We use T_OPTMGMT_REQ to change the primary destination address, source address
 * and options.
 */
STATIC noinline streams_fastcall int
te_data_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_data_req *p;
	size_t dlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_DATA_REQ);
	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_COTS && tp->info.SERV_type != T_COTS_ORD))
		goto error;
	/* Note: If the interface is in the TS_IDLE state when the provider receives the T_DATA_REQ 
	   primitive, then the TS provider should discard the request without generating a fatal
	   error. */
	if (tp_chk_state(tp, (TSF_IDLE)))
		/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
		   destination of the request is an address that does not have an outstanding reset
		   indication. */
		goto discard;
	err = TOUTSTATE;
	if (unlikely(tp_get_statef(tp) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	err = TBADFLAG;
	if (unlikely(p->MORE_flag != 0))
		/* TODO: We should check the MORE_flag and see whether this is a complete TSDU or
		   not.  If not, we should accumulate the M_DATA block in a buffer waiting for a
		   final T_DATA_REQ or delimited message.  */
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely
	    ((dlen = msgsize(dp)) == 0 || dlen > tp->info.TIDU_size || dlen > tp->info.TSDU_size))
		goto error;
	if (unlikely((err = tp_senddata(tp, tp->dport, &tp->options, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* tp_senddata() consumed message blocks */
      discard:
	return (QR_DONE);	/* tp_senddata() does not consume message blocks */
      error:
	return te_error_reply(q, -EPROTO);
}

/**
 * te_exdata_req - process a T_EXDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC noinline streams_fastcall int
te_exdata_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_exdata_req *p;
	size_t dlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_EXDATA_REQ);
	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tp->info.SERV_type != T_COTS && tp->info.SERV_type != T_COTS_ORD))
		goto error;
	/* Note: If the interface is in the TS_IDLE state when the provider receives the
	   T_EXDATA_REQ primitive, then the TS provider should discard the request without
	   generating a fatal error. */
	if (tp_chk_state(tp, (TSF_IDLE)))
		goto discard;
	if (unlikely(tp_not_state(tp, TSM_OUTDATA)))
		goto error;
	err = TBADFLAG;
	if (unlikely(p->MORE_flag != 0))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	dlen = msgsize(dp);
	if (unlikely(dlen == 0 || dlen > tp->info.TIDU_size || dlen > tp->info.ETSDU_size))
		goto error;
	err = tp_senddata(tp, tp->dport, &tp->options, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      discard:
	return (QR_DONE);
      error:
	return te_error_reply(q, -EPROTO);
}

/**
 * te_optdata_req - process a T_OPTDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC noinline streams_fastcall int
te_optdata_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_optdata_req *p;
	struct tp_options opts;
	size_t mlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_OPTDATA_REQ);
	err = TNOTSUPPORT;
	if (unlikely(tp->info.SERV_type == T_CLTS))
		goto error;
	if (unlikely(tp_get_state(tp) == TS_IDLE))
		goto discard;
	err = TOUTSTATE;
	if (unlikely(tp_get_statef(tp) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	err = TBADFLAG;
	if (unlikely(p->DATA_flag != 0))
		goto error;
	err = TBADOPT;
	if (unlikely(p->OPT_length > tp->info.OPT_size))
		goto error;
	err = TBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((mlen = msgsize(dp)) == 0 || mlen > tp->info.TSDU_size))
		goto error;
	opts = tp->options;
	opts.flags[0] = 0;
	if (unlikely(p->OPT_length != 0))
		if ((err = t_opts_parse(mp->b_wptr + p->OPT_offset, p->OPT_length, &opts)))
			goto error;
	if ((err = tp_senddata(tp, tp->dport, &opts, dp)) != QR_ABSORBED)
		goto error;
	return (QR_TRIMMED);
      discard:
	return (QR_DONE);
      error:
	return te_error_reply(q, err);
}

/**
 * te_optmgmt_req - handle T_OPTMGMT_REQ primitive
 * @q: active queue in the queue pair
 * @mp: the primitive
 *
 * The T_OPTMGMT_REQ is responsible for establishing options while the stream is in the T_IDLE
 * state.  When the stream is bound to a local address using the T_BIND_REQ, the settings of options
 * with end-to-end significance will have an affect on how the driver response to an INIT with
 * INIT-ACK for SCTP.  For example, the bound list of addresses is the list of addresses that will
 * be sent in the INIT-ACK.  The number of inbound streams and outbound streams are the numbers that
 * will be used in the INIT-ACK.
 *
 * Errors:
 * [TACCES] the user did not have proper permissions for the user of the requested options;
 * [TBADFLAG] the flags as specified were incorrect or invalid;
 * [TBADOPT] the options as specified were in an incorrect format, or they contained invalid information;
 * [TOUTSTATE] the primitive would place the transport interface out of state;
 * [TNOTSUPPORT] this prmitive is not supported;
 * [TSYSERR] a system error has occured and the UNIX system error is indicated in the primitive.
 */
STATIC noinline streams_fastcall int
te_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	int opt_len;
	const struct T_optmgmt_req *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_OPTMGMT_REQ);
#ifdef TS_WACK_OPTREQ
	if (tp_get_state(tp) == TS_IDLE)
		tp_set_state(tp, TS_WACK_OPTREQ);
#endif
	err = TBADOPT;
	if (unlikely(p->OPT_length > tp->info.OPT_size))
		goto error;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	err = TBADFLAG;
	switch (p->MGMT_flags) {
	case T_DEFAULT:
		opt_len = t_size_default_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CURRENT:
		opt_len = t_size_current_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CHECK:
		opt_len = t_size_check_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_NEGOTIATE:
		opt_len = t_size_negotiate_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	default:
		goto error;
	}
	if (unlikely(opt_len < 0)) {
		switch (-(err = opt_len)) {
		case EINVAL:
			err = TBADOPT;
			goto error;
		case EACCES:
			err = TACCES;
			goto error;
		default:
			goto error;
		}
	}
	err = t_optmgmt_ack(q, p->MGMT_flags, mp->b_rptr + p->OPT_offset, p->OPT_length, opt_len);
	if (unlikely(err < 0)) {
		switch (-err) {
		case EINVAL:
			err = TBADOPT;
			goto error;
		case EACCES:
			err = TACCES;
			goto error;
		case ENOBUFS:
		case ENOMEM:
			break;
		default:
			goto error;
		}
	}
	return (err);
      error:
	return te_error_ack(q, T_OPTMGMT_REQ, err);
}

#ifdef T_ADDR_REQ
/**
 * te_addr_req - process a T_ADDR_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC noinline streams_fastcall __unlikely int
te_addr_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_addr_req *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_ADDR_REQ);
	{
		struct sockaddr_in loc_buf[8], rem_buf[8];
		struct sockaddr_in *LOCADDR_buffer, *REMADDR_buffer;
		socklen_t LOCADDR_length, REMADDR_length;
		int i;

		err = TOUTSTATE;
		switch (tp_get_state(tp)) {
		case TS_UNBND:
			LOCADDR_buffer = NULL;
			LOCADDR_length = 0;
			REMADDR_buffer = NULL;
			REMADDR_length = 0;
			break;
		case TS_IDLE:
		case TS_WRES_CIND:
			for (i = 0; i < tp->bnum; i++) {
				loc_buf[i].sin_family = AF_INET;
				loc_buf[i].sin_port = tp->bport;
				loc_buf[i].sin_addr.s_addr = tp->baddrs[i].addr;
			}
			LOCADDR_buffer = loc_buf;
			LOCADDR_length = tp->bnum * sizeof(struct sockaddr_in);
			REMADDR_buffer = NULL;
			REMADDR_length = 0;
			break;
		case TS_WCON_CREQ:
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
			for (i = 0; i < tp->snum; i++) {
				loc_buf[i].sin_family = AF_INET;
				loc_buf[i].sin_port = tp->sport;
				loc_buf[i].sin_addr.s_addr = tp->saddrs[i].addr;
			}
			for (i = 0; i < tp->dnum; i++) {
				rem_buf[i].sin_family = AF_INET;
				rem_buf[i].sin_port = tp->dport;
				rem_buf[i].sin_addr.s_addr = tp->daddrs[i].addr;
			}
			LOCADDR_buffer = loc_buf;
			LOCADDR_length = tp->snum * sizeof(struct sockaddr_in);
			REMADDR_buffer = rem_buf;
			REMADDR_length = tp->dnum * sizeof(struct sockaddr_in);
			break;
		default:
			goto error;
		}
		return t_addr_ack(q, LOCADDR_buffer, LOCADDR_length, REMADDR_buffer,
				  REMADDR_length);
	}
      error:
	return te_error_ack(q, T_ADDR_REQ, err);
}
#endif				/* T_ADDR_REQ */

#ifdef T_CAPABILITY_REQ
/**
 * te_capability_req - process a T_CAPABILITY_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC noinline streams_fastcall __unlikely int
te_capability_req(queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == T_CAPABILITY_REQ);
	return t_capability_ack(q, p->CAP_bits1, mp->b_datap->db_type);
      error:
	return te_error_ack(q, T_CAPABILITY_REQ, err);
}
#endif				/* T_CAPABILITY_REQ */

/*
 *  STREAMS MESSAGE HANDLING
 */

/**
 * tp_w_proto - process an M_PROTO, M_PCPROTO message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_PROTO, M_PCPROTO message to process
 *
 * These are normal T-primitives written from the upper layer protocol.
 */
STATIC INLINE streams_fastcall __hot_put int
tp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	t_scalar_t prim = 0;
	struct tp *tp = TP_PRIV(q);

	tp->i_oldstate = tp_get_state(tp);	/* checkpoint */

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *((t_scalar_t *) mp->b_rptr))) {
		case T_UNITDATA_REQ:	/* Connection-less data send request */
			_printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, tp));
			rtn = te_unitdata_req(q, mp);
			break;
		case T_DATA_REQ:	/* Connection-Mode data transfer request */
			_printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, tp));
			rtn = te_data_req(q, mp);
			break;
		case T_CONN_REQ:	/* TC request */
			_printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, tp));
			rtn = te_conn_req(q, mp);
			break;
		case T_CONN_RES:	/* Accept previous connection indication */
			_printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, tp));
			rtn = te_conn_res(q, mp);
			break;
		case T_DISCON_REQ:	/* TC disconnection request */
			_printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, tp));
			rtn = te_discon_req(q, mp);
			break;
		case T_EXDATA_REQ:	/* Expedited data request */
			_printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, tp));
			rtn = te_exdata_req(q, mp);
			break;
		case T_INFO_REQ:	/* Information Request */
			_printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, tp));
			rtn = te_info_req(q, mp);
			break;
		case T_BIND_REQ:	/* Bind a TS user to transport address */
			_printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, tp));
			rtn = te_bind_req(q, mp);
			break;
		case T_UNBIND_REQ:	/* Unbind TS user from transport address */
			_printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, tp));
			rtn = te_unbind_req(q, mp);
			break;
		case T_OPTMGMT_REQ:	/* Options Management request */
			_printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, tp));
			rtn = te_optmgmt_req(q, mp);
			break;
		case T_ORDREL_REQ:	/* Orderly release request */
			_printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, tp));
			rtn = -EPROTO;
			break;
		case T_OPTDATA_REQ:	/* Data transfer with options request */
			_printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, tp));
			rtn = te_optdata_req(q, mp);
			break;
#if defined T_ADDR_REQ
		case T_ADDR_REQ:	/* Address request */
			_printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, tp));
			rtn = te_addr_req(q, mp);
			break;
#endif				/* defined T_ADDR_REQ */
#if defined T_CAPABILITY_REQ
		case T_CAPABILITY_REQ:	/* Capablities request */
			_printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, tp));
			rtn = te_capability_req(q, mp);
			break;
#endif				/* defined T_CAPABILITY_REQ */
		case T_CONN_IND:	/* Incoming connection indication */
		case T_CONN_CON:	/* Connection established */
		case T_DISCON_IND:	/* TC disconnected */
		case T_DATA_IND:	/* Incoming connection-mode data indication */
		case T_EXDATA_IND:	/* Incoming expedited data indication */
		case T_INFO_ACK:	/* Information Acknowledgement */
		case T_BIND_ACK:	/* TS User bound to transport address */
		case T_ERROR_ACK:	/* Error Acknowledgement */
		case T_OK_ACK:	/* Success Acknowledgement */
		case T_UNITDATA_IND:	/* Connection-less data receive indication */
		case T_UDERROR_IND:	/* UNITDATA Error Indication */
		case T_OPTMGMT_ACK:	/* Options management acknowledgement */
		case T_ORDREL_IND:	/* Orderly release indication */
		case T_OPTDATA_IND:	/* Data with options indication */
		case T_ADDR_ACK:	/* Address acknowledgement */
		case T_CAPABILITY_ACK:	/* Capability acknowledgement */
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
#ifndef _TEST
		/* not so seldom() during conformance suite testing */
		seldom();
#endif
		tp_set_state(tp, tp->i_oldstate);
		/* The put and service procedure do not recognize all errors.  Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EOPNOTSUPP:	/* primitive not supported */
			return te_error_ack(q, prim, rtn);
		case -EPROTO:
			return te_error_reply(q, -EPROTO);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/**
 * tp_wp_data - process M_DATA message
 * @q: active queue in pair (write queue)
 * @mp: the M_DATA message
 */
STATIC noinline streams_fastcall __unlikely int
tp_w_data(queue_t *q, mblk_t *mp)
{
	return te_write_req(q, mp);
}

/**
 * tp_w_other - proces other message
 * @q: active queue in pair (write queue)
 * @mp: the message
 */
STATIC noinline streams_fastcall __unlikely int
tp_w_other(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		tp->u.dev.cminor);
	return (-EOPNOTSUPP);
}

/**
 * tp_w_ioctl - M_IOCTL handling
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 *
 * This TPI-UDP provider does not support any input-output controls and, therefore, all input-output
 * controls are negatively acknowledged.
 */
STATIC noinline streams_fastcall __unlikely int
tp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;

	mp->b_datap->db_type = M_IOCNAK;
	iocp->ioc_count = 0;
	iocp->ioc_error = EINVAL;
	iocp->ioc_rval = -1;
	qreply(q, mp);
	return (QR_ABSORBED);
}

/**
 * tp_r_other - process other message
 * @q: active queue in pair (read queue)
 * @mp: the message
 */
STATIC noinline streams_fastcall __unlikely int
tp_r_other(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		tp->u.dev.cminor);
#if 0
	putnext(q, mp);
	return (QR_ABSORBED);
#else
	return (-EOPNOTSUPP);
#endif
}

/**
 * tp_r_data - process M_DATA message
 * @q: active queue in queue pair (read queue)
 * @mp: the M_DATA message
 *
 * M_DATA messages are placed to the read queue by the Linux IP tp_v4_rcv() callback.  The message
 * contains a complete IP datagram starting with the IP header.  What needs to be done is to convert
 * this to an upper layer indication and deliver it upstream.
 */
STATIC INLINE streams_fastcall __hot_in int
tp_r_data(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	int rtn;

	switch (tp->info.SERV_type) {
	case T_CLTS:
		rtn = te_unitdata_ind(q, mp);
		break;
	case T_COTS:
	case T_COTS_ORD:
		switch (tp_get_state(tp)) {
		case TS_DATA_XFER:
		case TS_WCON_CREQ:
		case TS_WACK_CREQ:
		case TS_WREQ_ORDREL:
		case TS_WIND_ORDREL:
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			rtn = te_optdata_ind(q, mp);
			break;
		case TS_IDLE:
			if (tp->CONIND_number == 0) {
				rtn = QR_DONE;
				break;
			}
			/* fall through */
		case TS_WRES_CIND:
		case TS_WACK_CRES:
			rtn = te_conn_ind(q, mp);
			break;
		default:
			/* ignore it now */
			rtn = QR_DONE;
			break;
		}
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

/**
 * tp_r_error - process M_ERROR message
 * @q: active queue in queue pair (read queue)
 * @mp: the M_ERROR message
 *
 * M_ERROR messages are placed to the read queue by the Linux IP tp_v4_err() callback.  The message
 * contains a complete ICMP datagram starting with the IP header.  What needs to be done is to
 * convert this to an upper layer indication and deliver it upstream.
 */
STATIC noinline streams_fastcall __unlikely int
tp_r_error(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	int rtn;

	switch (tp->info.SERV_type) {
	case T_CLTS:
		rtn = te_uderror_ind_icmp(q, mp);
		break;
	case T_COTS:
	case T_COTS_ORD:
		switch (tp_get_state(tp)) {
		case TS_DATA_XFER:
		case TS_WCON_CREQ:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			rtn = te_discon_ind_icmp(q, mp);
			break;
		case TS_WRES_CIND:
		case TS_WACK_CRES:
			rtn = te_discon_ind_icmp(q, mp);
			break;
		default:
			/* ignore it now */
			rtn = QR_DONE;
			break;
		}
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

STATIC noinline streams_fastcall __unlikely int
tp_r_prim_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tp_r_data(q, mp);
	case M_ERROR:
		return tp_r_error(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	default:
		return tp_r_other(q, mp);
	}
}

/**
 * tp_r_prim - process primitive on read queue
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC INLINE streamscall __hot_in int
tp_r_prim(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_DATA))
		/* fast path for data */
		if (likely(TP_PRIV(q)->info.SERV_type == T_CLTS))
			return te_unitdata_ind(q, mp);
	return tp_r_prim_slow(q, mp);
}

STATIC noinline streams_fastcall __unlikely int
tp_w_prim_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return tp_w_ioctl(q, mp);
	default:
		return tp_w_other(q, mp);
	}
}

STATIC noinline streams_fastcall __unlikely int
tp_w_prim_error(queue_t *q, const int rtn)
{
	switch (rtn) {
	case -EBUSY:		/* flow controlled */
	case -EAGAIN:		/* try again */
	case -ENOMEM:		/* could not allocate memory */
	case -ENOBUFS:		/* could not allocate an mblk */
	case -EOPNOTSUPP:	/* primitive not supported */
		return te_error_ack(q, T_UNITDATA_REQ, rtn);
	case -EPROTO:
		return te_error_reply(q, -EPROTO);
	default:
		return (0);
	}
}

/**
 * tp_w_prim - process primitive on write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC INLINE streamscall __hot_put int
tp_w_prim(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_PROTO)) {
		/* fast path for data */
		if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
			if (likely(*((t_scalar_t *) mp->b_rptr) == T_UNITDATA_REQ)) {
				int rtn;

				if (likely((rtn = te_unitdata_req(q, mp)) >= 0))
					return (rtn);
				return tp_w_prim_error(q, rtn);
			}
		}
	}
	return tp_w_prim_slow(q, mp);
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
/**
 * tp_lookup_conn - lookup Stream in the connection hashes
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
STATIC noinline streams_fastcall __unlikely struct tp *
tp_lookup_conn(unsigned char proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct tp *result = NULL;
	int hiscore = -1;
	struct tp_chash_bucket *hp, *hp1, *hp2;

	hp1 = &udp_chash[udp_chashfn(proto, sport, dport)];
	hp2 = &udp_chash[udp_chashfn(proto, 0, 0)];

	hp = hp1;
	do {
		read_lock_bh(&hp->lock);
		{
			struct tp *tp;
			t_uscalar_t state;
			int i;

			for (tp = hp->list; tp; tp = tp->cnext) {
				int score = 0;

				/* only Streams in close to the correct state */
				if ((state = tp_get_state(tp)) != TS_DATA_XFER
				    && state != TS_WIND_ORDREL)
					continue;
				/* must match a bound protocol id */
				for (i = 0; i < tp->pnum; i++) {
					if (tp->protoids[i] != proto)
						continue;
					break;
				}
				if (i >= tp->pnum)
					continue;
				if (tp->sport != 0) {
					if (tp->sport != sport)
						continue;
					score++;
				}
				if (tp->dport != 0) {
					if (tp->dport != dport)
						continue;
					score++;
				}
				for (i = 0; i < tp->snum; i++) {
					if (tp->saddrs[i].addr == 0)
						break;
					if (tp->saddrs[i].addr != daddr)
						continue;
					score++;
					break;
				}
				if (i >= tp->snum)
					continue;
				for (i = 0; i < tp->dnum; i++) {
					if (tp->daddrs[i].addr == 0)
						break;
					if (tp->daddrs[i].addr != saddr)
						continue;
					score++;
					break;
				}
				if (i >= tp->dnum)
					continue;
				if (score > hiscore) {
					hiscore = score;
					if (result != NULL)
						tp_put(result);
					result = tp_get(tp);
				}
				if (score == 4)
					/* perfect match */
					break;
			}
		}
		read_unlock_bh(&hp->lock);
	} while (hiscore < 4 && hp != hp2 && (hp = hp2));
	usual(result);
	return (result);
}

/**
 * tp_lookup_bind - lookup Stream in the bind/listening hashes
 * @proto:	protocol from IP header
 * @daddr:	destination address from IP header
 * @dport:	destination port from UDP header
 *
 * Note that an T_CLTS Stream can bind to either a port number or port number zero. An T_COTS Stream
 * can only bind listening to a non-zero port number, but can bind normal to a zero port number.
 *
 * T_CLTS Streams are always entered into the bind hashes against port number zero.  T_COTS Streams
 * are entered against their bound port number (which can be zero only if they are not listening).
 * T_COTS Streams that are not listening will not be matched.  Only the DEFAULT_LISTENER can be
 * bound listening against a zero port (and it requires a wildcard address).
 *
 * NOTICES: There are two types of Streams in the bind hashes, T_COTS and T_CLTS.  When lookup up
 * a Stream for a received packet, we are interested in any T_CLTS Stream that matches or any T_COTS
 * stream that is in the listening state that matches.
 */
STATIC INLINE streams_fastcall __hot_in struct tp *
tp_lookup_bind(unsigned char proto, uint32_t daddr, unsigned short dport)
{
	struct tp *result = NULL;
	int hiscore = -1;
	struct tp_bhash_bucket *hp, *hp1, *hp2;

	hp1 = &udp_bhash[udp_bhashfn(proto, dport)];
	hp2 = &udp_bhash[udp_bhashfn(proto, 0)];

	hp = hp1;
	_ptrace(("%s: %s: proto = %d, dport = %d\n", DRV_NAME, __FUNCTION__, (int) proto,
		 (int) ntohs(dport)));
	do {
		read_lock_bh(&hp->lock);
		{
			register struct tp *tp;

			for (tp = hp->list; tp; tp = tp->bnext) {
				register int score = 0;
				register int i;

				/* only listening T_COTS(_ORD) Streams and T_CLTS Streams */
				if (tp->CONIND_number == 0 && tp->info.SERV_type != T_CLTS) {
					__trace();
					continue;
				}
				/* only Streams in close to the correct state */
				if (tp_not_state(tp, (TSF_IDLE|TSF_WACK_CREQ))) {
					__trace();
					continue;
				}
				for (i = 0; i < tp->pnum; i++) {
					if (tp->protoids[i] != proto) {
						__trace();
						continue;
					}
					break;
				}
				if (i >= tp->pnum) {
					__trace();
					continue;
				}
				if (tp->bport != 0) {
					if (tp->bport != dport) {
						__trace();
						continue;
					}
					score++;
				}

				for (i = 0; i < tp->bnum; i++) {
					if (tp->baddrs[i].addr == 0)
						break;
					if (tp->baddrs[i].addr != daddr) {
						__trace();
						continue;
					}
					score++;
					break;
				}
				if (i >= tp->bnum) {
					__trace();
					continue;
				}
				if (score > hiscore) {
					hiscore = score;
					if (result != NULL)
						tp_put(result);
					result = tp_get(tp);
				}
				if (score == 2)
					/* perfect match */
					break;
			}
		}
		read_unlock_bh(&hp->lock);
	} while (hiscore < 2 && hp != hp2 && (hp = hp2));
	__usual(result);
	return (result);
}

STATIC noinline streams_fastcall __unlikely struct tp *
tp_lookup_common_slow(struct tp_prot_bucket *pp, uint8_t proto, uint32_t daddr, uint16_t dport,
		      uint32_t saddr, uint16_t sport)
{
	struct tp *result = NULL;

	if (likely(pp != NULL)) {
		if (likely(pp->corefs != 0))
			result = tp_lookup_conn(proto, daddr, dport, saddr, sport);
		if (result == NULL && (pp->corefs != 0 || pp->clrefs != 0))
			result = tp_lookup_bind(proto, daddr, dport);
	}
	return (result);
}

STATIC INLINE streams_fastcall __hot_in struct tp *
tp_lookup_common(uint8_t proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct tp_prot_bucket *pp, **ppp;
	register struct tp *result;

	ppp = &udp_prots[proto];

	read_lock_bh(&udp_prot_lock);
	if (likely((pp = *ppp) != NULL)) {
		if (likely(pp->corefs == 0)) {
			if (likely(pp->clrefs > 0)) {
				result = tp_lookup_bind(proto, daddr, dport);
			      done:
				read_unlock_bh(&udp_prot_lock);
				return (result);
			} else
				__rare();
		} else
			__rare();
	} else
		__rare();
	result = tp_lookup_common_slow(pp, proto, daddr, dport, saddr, sport);
	goto done;
}

/**
 * tp_lookup - lookup Stream by protocol, address and port.
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
STATIC INLINE streams_fastcall __hot_in struct tp *
tp_lookup(struct iphdr *iph, struct udphdr *uh)
{
	return tp_lookup_common(iph->protocol, iph->daddr, uh->dest, iph->saddr, uh->source);
}

/**
 * tp_lookup_icmp - a little different from tp_lookup(), for ICMP messages
 * @iph:	returned IP header
 * @len:	length of returned message
 *
 * This needs to do a reverse lookup (where destination address and port are compared to source
 * address and port, and visa versa).
 */
STATIC noinline streams_fastcall __unlikely struct tp *
tp_lookup_icmp(struct iphdr *iph, unsigned int len)
{
	struct udphdr *uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));

	if (len < (iph->ihl << 2) + 4)
		/* too short: don't have port numbers - ignore it */
		return (NULL);

	return tp_lookup_common(iph->protocol, iph->saddr, uh->source, iph->daddr, uh->dest);
}

/**
 * tp_free - message block free function for mblks esballoc'ed from sk_buffs
 * @data: client data (sk_buff pointer in this case)
 */
STATIC streamscall __hot_get void
tp_free(char *data)
{
	struct sk_buff *skb = (typeof(skb)) data;
	struct tp *tp;

	dassert(skb != NULL);
	if (likely((tp = *(struct tp **) skb->cb) != NULL)) {
		spin_lock_bh(&tp->qlock);
		// ensure(tp->rcvmem >= skb->truesize, tp->rcvmem = skb->truesize);
		tp->rcvmem -= skb->truesize;
		spin_unlock_bh(&tp->qlock);
#if 0
		/* put this back to null before freeing it */
		*(struct tp **) skb->cb = NULL;
#endif
		tp_put(tp);
	      done:
		kfree_skb(skb);
		return;
	}
	goto done;
}

/**
 * tp_v4_rcv - receive IPv4 protocol packets
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
tp_v4_rcv(struct sk_buff *skb)
{
	struct tp *tp;
	struct iphdr *iph = skb->nh.iph;
	struct udphdr *uh = (struct udphdr *) (skb->nh.raw + (iph->ihl << 2));
	struct rtable *rt;
	ushort ulen;

//      IP_INC_STATS_BH(IpInDelivers);  /* should wait... */

	if (unlikely(!pskb_may_pull(skb, sizeof(struct udphdr))))
		goto too_small;
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
	rt = (struct rtable *) skb->dst;
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;

	_printd(("%s: %s: packet received %p\n", DRV_NAME, __FUNCTION__, skb));
//      UDP_INC_STATS_BH(UdpInDatagrams);
	uh = skb->h.uh;
	ulen = ntohs(uh->len);
	/* sanity check UDP length */
	if (unlikely(ulen > skb->len || ulen < sizeof(struct udphdr)))
		goto too_small;
	if (unlikely(pskb_trim(skb, ulen)))
		goto too_small;
	/* we do the lookup before the checksum */
	if (unlikely((tp = tp_lookup(iph, uh)) == NULL))
		goto no_stream;

	/* TODO: for 2.4 we overwrite iph->protocol to steal the packet.  This means that we loose
	   the protocol information.  This is important if we ever allow binding to more than one
	   protocol id (which we don't currently do).  At that point we would have to copy the
	   sk_buff before stealing the original and free the original.  See the NBADTOKEN check in
	   tp_passive(). */
	/* FIXME: Another trick for 2.4 would be to copy the IP header and UDP header, but esballoc 
	   the payload.  That might be the best way to do things.  The problem is that we indicate
	   iph->protocol on connection indications, and it will be the wrong protocol on 2.4. */
	tp_v4_steal(skb);	/* its ours */
	/* checksum initialization */
	if (uh->check == 0)
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else if (skb->ip_summed == CHECKSUM_HW)
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else if (skb->ip_summed != CHECKSUM_UNNECESSARY)
		skb->csum = csum_tcpudp_nofold(iph->saddr, iph->daddr, ulen, IPPROTO_UDP, 0);
#if 0
	/* For now... We should actually place non-linear fragments into separate mblks and pass
	   them up as a chain, or deal with non-linear sk_buffs directly.  As it winds up, the
	   netfilter hooks linearize anyway. */
	if (unlikely(skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0))
		goto linear_fail;
#else
	if (unlikely(skb_is_nonlinear(skb))) {
		_ptrace(("Non-linear sk_buff encountered!\n"));
		goto linear_fail;
	}
#endif
	/* Before passing the message up, check that there is room in the receive buffer.  Allows
	   slop over by 1 buffer per processor. */
	if (unlikely(tp->rcvmem > tp->options.xti.rcvbuf))
		goto flow_controlled;
	{
		mblk_t *mp;
		frtn_t fr = { &tp_free, (char *) skb };
		size_t plen = skb->len + (skb->data - skb->nh.raw);

		/* now allocate an mblk */
		if (unlikely((mp = esballoc(skb->nh.raw, plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* We can do this because the buffer belongs to us. */
		*(struct tp **) skb->cb = tp_get(tp);
		/* already in bottom half */
		spin_lock(&tp->qlock);
		tp->rcvmem += skb->truesize;
		spin_unlock(&tp->qlock);
		// mp->b_datap->db_type = M_DATA;
		mp->b_wptr += plen;
		put(tp->oq, mp);
//              UDP_INC_STATS_BH(UdpInDatagrams);
		/* release reference from lookup */
		tp_put(tp);
		return (0);
	}
      bad_checksum:
//      UDP_INC_STATS_BH(UdpInErrors);
//      IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ??? */
	// goto linear_fail;
      no_buffers:
      flow_controlled:
      linear_fail:
	if (tp)
		tp_put(tp);
	kfree_skb(skb);
	return (0);
      no_stream:
	ptrace(("ERROR: No stream\n"));
	/* Note, in case there is nobody to pass it too, we have to complete the checksum check
	   before dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
//      UDP_INC_STATS_BH(UdpNoPorts);   /* should wait... */
	// goto pass_it;
      bad_pkt_type:
	// goto pass_it;
      too_small:
	// goto pass_it;
      // pass_it:
	if (tp_v4_rcv_next(skb)) {
		/* TODO: want to generate an ICMP error here */
	}
	return (0);
}

/**
 * tp_v4_err - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information (unused)
 *
 * This function is a network protocol callback that is invoked when transport specific ICMP errors
 * are received.  The function looks up the Stream and, if found, wraps the packet in an M_ERROR
 * message and passes it to the read queue of the Stream.
 *
 * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned packet, and IP payload
 * of returned packet (up to some number of bytes of total payload).  The passed in sk_buff has
 * skb->data pointing to the ICMP payload which is the beginning of the returned IP header.
 * However, we include the entire packet in the message.
 *
 * LOCKING: master.lock protects the master list and protects from open, close, link and unlink.
 * tp->qlock protects the state of private structure.  tp->refs protects the private structure
 * from being deallocated before locking.
 */
STATIC __unlikely void
tp_v4_err(struct sk_buff *skb, u32 info)
{
	struct tp *tp;
	struct iphdr *iph = (struct iphdr *) skb->data;

#if 0
/* icmp.c does this for us */
#define ICMP_MIN_LENGTH sizeof(struct udphdr)
	if (skb->len < (iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
#endif
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	if ((tp = tp_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (tp_get_state(tp) == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		size_t plen = skb->len + (skb->data - skb->nh.raw);

		/* Create a queue a specialized M_ERROR message to the Stream's read queue for
		   further processing.  The Stream will convert this message into a T_UDERROR_IND
		   or T_DISCON_IND message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* check flow control only after we have a buffer */
		if (tp->oq == NULL || !canput(tp->oq))
			goto flow_controlled;
		mp->b_datap->db_type = M_ERROR;
		bcopy(skb->nh.raw, mp->b_wptr, plen);
		mp->b_wptr += plen;
		put(tp->oq, mp);
		goto discard_put;
	      flow_controlled:
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	tp_put(tp);
	tp_v4_err_next(skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(skb, info);
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
 * tp_alloc_priv - allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @slp: pointer to place in list for insertion
 * @type: type of provider: 0, T_COTS, T_COTS_ORD, T_CLTS
 * @devp: pointer to device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initializes it to appropriate values, and then inserts it into
 * the private structure list.
 */
STATIC noinline struct tp *
tp_alloc_priv(queue_t *q, struct tp **tpp, int type, dev_t *devp, cred_t *crp)
{
	struct tp *tp;

	if ((tp = tp_alloc())) {
		/* tp generic members */
		tp->u.dev.cmajor = getmajor(*devp);
		tp->u.dev.cminor = getminor(*devp);
		tp->cred = *crp;
		(tp->oq = q)->q_ptr = tp_get(tp);
		(tp->iq = WR(q))->q_ptr = tp_get(tp);
		tp->i_prim = &tp_w_prim;
		tp->o_prim = &tp_r_prim;
		// tp->i_wakeup = NULL;
		// tp->o_wakeup = NULL;
		spin_lock_init(&tp->qlock);	/* "tp-queue-lock" */
		// tp->ibid = 0;
		// tp->obid = 0;
		// tp->iwait = NULL;
		// tp->owait = NULL;
		tp->i_state = (-1);	// LMI_UNUSABLE;
		tp->i_style = (1);	// LMI_STYLE1;
		tp->i_version = 1;
		tp->i_oldstate = (1);	// LMI_UNUSABLE;
		tp->type = type;
		/* tp specific members */
		tp->info.PRIM_type = T_INFO_ACK;
		tp->info.TSDU_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
		tp->info.ETSDU_size = T_INVALID;
		tp->info.CDATA_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
		tp->info.DDATA_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
		tp->info.ADDR_size = sizeof(struct sockaddr_storage);
		tp->info.OPT_size = 65535;
		tp->info.TIDU_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
		tp->info.SERV_type = type ? : T_CLTS;
		tp->info.CURRENT_state = TS_UNBND;
		tp->info.PROVIDER_flag = T_XPG4_1 & ~T_SNDZERO;
		bufq_init(&tp->conq);
		/* option defaults */
		tp->options.xti.linger = xti_default_linger;
		tp->options.xti.rcvbuf = sysctl_rmem_default;
		tp->options.xti.rcvlowat = xti_default_rcvlowat;
		tp->options.xti.sndbuf = sysctl_wmem_default;
		tp->options.xti.sndlowat = 16768;
		tp->options.xti.priority = xti_default_priority;
		tp->options.ip.protocol = ip_default_protocol;
		tp->options.ip.tos = ip_default_tos;
		tp->options.ip.ttl = ip_default_ttl;
		tp->options.ip.reuseaddr = ip_default_reuseaddr;
		tp->options.ip.dontroute = ip_default_dontroute;
		tp->options.ip.broadcast = ip_default_broadcast;
		tp->options.ip.addr = ip_default_addr;
		tp->options.ip.saddr = ip_default_saddr;
		tp->options.ip.daddr = ip_default_daddr;
		tp->options.ip.mtu = ip_default_mtu;
		tp->options.udp.checksum = udp_default_checksum;
		/* link into master list */
		tp_get(tp);
		if ((tp->next = *tpp))
			tp->next->prev = &tp->next;
		tp->prev = tpp;
		*tpp = tp;
	} else
#if 0
		strlog(DRV_ID, getminor(*devp), 0, SL_WARN | SL_CONSOLE,
		       "could not allocate driver private structure");
#else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
#endif
	return (tp);
}

/**
 * tp_free_priv - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
STATIC noinline void
tp_free_priv(queue_t *q)
{
	struct tp *tp;

	ensure(q, return);
	tp = TP_PRIV(q);
	ensure(tp, return);
#if 0
	strlog(DRV_ID, tp->u.dev.cminor, 0, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&tp->refcnt));
#else
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&tp->refcnt)));
#endif
	/* make sure the stream is disconnected */
	if (tp->chash != NULL) {
		tp_disconnect(tp, NULL, NULL, 0, NULL);
		tp_set_state(tp, TS_IDLE);
	}
	/* make sure the stream is unbound */
	if (tp->bhash != NULL) {
		tp_unbind(tp);
		tp_set_state(tp, TS_UNBND);
	}
	if (tp->daddrs[0].dst != NULL) {
		dst_release(tp->daddrs[0].dst);
		tp->daddrs[0].dst = NULL;
	}
#if 0
	{
		mblk_t *b, *b_prev, *b_next;

		/* purge connection indication queue, conq */
		b_next = XCHG(&tp->conq, NULL);
		while ((b = b_next)) {
			b_next = XCHG(&b->b_next, NULL);
			/* might be data hanging off of b_prev pointer */
			b_prev = b;
			while ((b = b_prev)) {
				b_prev = XCHG(&b->b_prev, NULL);
				freemsg(b);
			}
		}
		/* purge reset indication queue, resq */
		b_next = XCHG(&tp->resq, NULL);
		while ((b = b_next)) {
			b_next = XCHG(&b->b_next, NULL);
			freemsg(b);
		}
	}
#else
	bufq_purge(&tp->conq);
#endif
	ss7_unbufcall((str_t *) tp);
#if 0
	strlog(DRV_ID, tp->u.dev.cminor, 0, SL_TRACE, "removed bufcalls: reference count = %d",
	       atomic_read(&tp->refcnt));
#else
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME,
		atomic_read(&tp->refcnt)));
#endif
	/* remove from master list */
	if ((*tp->prev = tp->next))
		tp->next->prev = tp->prev;
	tp->next = NULL;
	tp->prev = &tp->next;
	tp_put(tp);
#if 0
	strlog(DRV_ID, tp->u.dev.cminor, 0, SL_TRACE, "unlinked: reference count = %d",
	       atomic_read(&tp->refcnt));
#else
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&tp->refcnt)));
#endif
	tp_release((struct tp **) &tp->oq->q_ptr);
	tp->oq = NULL;
	tp_release((struct tp **) &tp->iq->q_ptr);
	tp->iq = NULL;
	assure(atomic_read(&tp->refcnt) == 1);
	tp_release(&tp);	/* should normally be final put */
	return;
}

/*
 *  Open and Close
 */
#define FIRST_CMINOR	0
#define    IP_CMINOR	0
#define  UDPCO_CMINOR	T_COTS
#define  UDPCD_CMINOR	T_COTS_ORD
#define  UDPCL_CMINOR	T_CLTS
#define  LAST_CMINOR	0
#define  FREE_CMINOR	1
STATIC int tp_majors[CMAJORS] = { CMAJOR_0, };

/**
 * udp_qopen - UDP driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
udp_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct tp *tp, **tpp = &master.tp.list;

#if defined LFS
	mblk_t *mp;
	struct stroptions *so;
#endif

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
#if defined LFS
	if (!(mp = allocb(sizeof(*so), BPRI_MED)))
		return (ENOBUFS);
#endif
	write_lock_bh(&master.lock);
	for (; *tpp; tpp = &(*tpp)->next) {
		if (cmajor != (*tpp)->u.dev.cmajor)
			break;
		if (cmajor == (*tpp)->u.dev.cmajor) {
			if (cminor < (*tpp)->u.dev.cminor)
				break;
			if (cminor == (*tpp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = tp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_bh(&master.lock);
#if defined LFS
		freeb(mp);
#endif
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(tp = tp_alloc_priv(q, tpp, type, devp, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_bh(&master.lock);
#if defined LFS
		freeb(mp);
#endif
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
#if defined LFS
	/* want to set a write offet of MAX_HEADER bytes */
	so = (typeof(so)) mp->b_wptr;
	so->so_flags = SO_WROFF | SO_WRPAD;
	so->so_wroff = MAX_HEADER;	/* this is too big */
	so->so_wrpad = SMP_CACHE_BYTES + sizeof(struct skb_shared_info);	/* this is too big */
	mp->b_wptr += sizeof(*so);
	mp->b_datap->db_type = M_SETOPTS;
	putnext(q, mp);
#endif
	qprocson(q);
	return (0);
}

/**
 * udp_qclose - UDP driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
STATIC streamscall int
udp_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct tp *tp = TP_PRIV(q);

	(void) oflag;
	(void) crp;
	(void) tp;
	_printd(("%s: closing character device %d:%d\n", DRV_NAME, tp->u.dev.cmajor,
		 tp->u.dev.cminor));
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
	qprocsoff(q);
	tp_free_priv(q);	/* free and unlink the structure */
	goto quit;
      quit:
	return (0);
}

/*
 *  Private structure reference counting, allocation, deallocation and cache
 */
STATIC __unlikely int
tp_term_caches(void)
{
	if (udp_prot_cachep != NULL) {
		if (kmem_cache_destroy(udp_prot_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy udp_prot_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		_printd(("%s: destroyed udp_prot_cachep\n", DRV_NAME));
		udp_prot_cachep = NULL;
	}
	if (udp_priv_cachep != NULL) {
		if (kmem_cache_destroy(udp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy udp_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		_printd(("%s: destroyed udp_priv_cachep\n", DRV_NAME));
		udp_priv_cachep = NULL;
	}
	return (0);
}
STATIC __unlikely int
tp_init_caches(void)
{
	if (udp_priv_cachep == NULL) {
		udp_priv_cachep = kmem_cache_create("udp_priv_cachep", sizeof(struct tp), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (udp_priv_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate udp_priv_cachep", __FUNCTION__);
			tp_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (udp_prot_cachep == NULL) {
		udp_prot_cachep =
		    kmem_cache_create("udp_prot_cachep", sizeof(struct tp_prot_bucket), 0,
				      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (udp_prot_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate udp_prot_cachep", __FUNCTION__);
			tp_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver protocol structure cache\n", DRV_NAME));
	}
	return (0);
}

STATIC __unlikely void
tp_term_hashes(void)
{
	if (udp_bhash) {
		free_pages((unsigned long) udp_bhash, udp_bhash_order);
		udp_bhash = NULL;
		udp_bhash_size = 0;
		udp_bhash_order = 0;
	}
	if (udp_chash) {
		free_pages((unsigned long) udp_chash, udp_chash_order);
		udp_chash = NULL;
		udp_chash_size = 0;
		udp_chash_order = 0;
	}
}
STATIC __unlikely void
tp_init_hashes(void)
{
	int i;

	/* Start with just one page for each. */
	if (udp_bhash == NULL) {
		udp_bhash_order = 0;
		if ((udp_bhash =
		     (struct tp_bhash_bucket *) __get_free_pages(GFP_ATOMIC, udp_bhash_order))) {
			udp_bhash_size =
			    (1 << (udp_bhash_order + PAGE_SHIFT)) / sizeof(struct tp_bhash_bucket);
			_printd(("%s: INFO: bind hash table configured size = %ld\n", DRV_NAME,
				 (long) udp_bhash_size));
			bzero(udp_bhash, udp_bhash_size * sizeof(struct tp_bhash_bucket));
			for (i = 0; i < udp_bhash_size; i++)
				rwlock_init(&udp_bhash[i].lock);
		} else {
			tp_term_hashes();
			cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
			return;
		}
	}
	if (udp_chash == NULL) {
		udp_chash_order = 0;
		if ((udp_chash =
		     (struct tp_chash_bucket *) __get_free_pages(GFP_ATOMIC, udp_chash_order))) {
			udp_chash_size =
			    (1 << (udp_chash_order + PAGE_SHIFT)) / sizeof(struct tp_chash_bucket);
			_printd(("%s: INFO: conn hash table configured size = %ld\n", DRV_NAME,
				 (long) udp_chash_size));
			bzero(udp_chash, udp_chash_size * sizeof(struct tp_chash_bucket));
			for (i = 0; i < udp_chash_size; i++)
				rwlock_init(&udp_chash[i].lock);
		} else {
			tp_term_hashes();
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
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the UDP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the UDP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 */
#ifdef LFS

STATIC struct cdevsw udp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &udp_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC __unlikely int
tp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&udp_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
tp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&udp_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 */
#ifdef LIS

STATIC __unlikely int
tp_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &udp_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
tp_unregister_strdev(major_t major)
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
		if (tp_majors[mindex]) {
			if ((err = tp_unregister_strdev(tp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					tp_majors[mindex]);
			if (mindex)
				tp_majors[mindex] = 0;
		}
	}
	if ((err = tp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	tp_term_hashes();
	return;
}

MODULE_STATIC int __init
udpinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	tp_init_hashes();
	if ((err = tp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		udpterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = tp_register_strdev(tp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					tp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				udpterminate();
				return (err);
			}
		}
		if (tp_majors[mindex] == 0)
			tp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(tp_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = tp_majors[0];
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
