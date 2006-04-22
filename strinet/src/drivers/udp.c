/*****************************************************************************

 @(#) $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/04/22 10:51:04 $

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

 Last Modified $Date: 2006/04/22 10:51:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: udp.c,v $
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

#ident "@(#) $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/04/22 10:51:04 $"

static char const ident[] =
    "$RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/04/22 10:51:04 $";

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

#define udp_tst_bit(nr,addr)	    test_bit(nr,addr)
#define udp_set_bit(nr,addr)	    __set_bit(nr,addr)
#define udp_clr_bit(nr,addr)	    __clear_bit(nr,addr)

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
#define UDP_REVISION	"OpenSS7 $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/04/22 10:51:04 $"
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
MODULE_ALIAS("streams-driver-udp");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_UDP_MAJOR));
MODULE_ALIAS("/dev/streams/udp");
MODULE_ALIAS("/dev/streams/udp/*");
MODULE_ALIAS("/dev/streams/clone/udp");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(UDP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/udp");
MODULE_ALIAS("/dev/inet/udp");
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
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC streamscall int udp_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int udp_qclose(queue_t *, int, cred_t *);

STATIC struct qinit udp_rinit = {
	.qi_putp = &ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = &ss7_osrv,		/* Read queue service */
	.qi_qopen = udp_qopen,		/* Each open */
	.qi_qclose = udp_qclose,	/* Last close */
	.qi_minfo = &udp_minfo,		/* Information */
};

STATIC struct qinit udp_winit = {
	.qi_putp = &ss7_iput,		/* Write put (msg from above) */
	.qi_srvp = &ss7_isrv,		/* Write queue service */
	.qi_minfo = &udp_minfo,		/* Information */
};

MODULE_STATIC struct streamtab udp_info = {
	.st_rdinit = &udp_rinit,	/* Upper read queue */
	.st_wrinit = &udp_winit,	/* Upper write queue */
};

/*
 *  TLI Interface state flags
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#ifdef TS_WACK_OPTREQ
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#endif
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WREQ_CIND	)
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

/*
 *  =========================================================================
 *
 *  UDP Private Datastructures
 *
 *  =========================================================================
 */

typedef struct udp_options {
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
	} xti;
	struct {
		unsigned char options[40];	/* T_IP_OPTIONS */
		unsigned char tos;	/* T_IP_TOS */
		unsigned char ttl;	/* T_IP_TTL */
		unsigned int reuseaddr;	/* T_IP_REUSEADDR */
		unsigned int dontroute;	/* T_IP_DONTROUTE */
		unsigned int broadcast;	/* T_IP_BROADCAST */
		uint32_t addr;		/* T_IP_ADDR */
	} ip;
	struct {
		t_uscalar_t checksum;	/* T_UDP_CHECKSUM */
	} udp;
} udp_options_t;

struct udp_bind_bucket;

typedef struct udp {
	STR_DECLARATION (struct udp);	/* Streams declaration */
	struct udp *bnext;		/* linkage for bind hash */
	struct udp **bprev;		/* linkage for bind hash */
	struct udp_bind_bucket *bindb;	/* linkage for bind hash */
	struct udp *cnext;		/* linkage for connection hash */
	struct udp **cprev;		/* linkage for connection hash */
	struct udp_chash_bucket *hashb;	/* linkage for connection hash */
	unsigned short port;		/* bound port number - host order */
	struct sockaddr_storage bind;	/* bound address */
	struct sockaddr_storage srce;	/* source address */
	struct sockaddr_storage dest;	/* connected address */
	t_uscalar_t CONIND_number;	/* maximum number of connection indications */
	bufq_t conq;			/* queue outstanding connection indications */
	struct T_info_ack info;		/* information */
	struct udp_options options;	/* protocol options */
	unsigned char _pad[40];		/* pad for ip options */
} udp_t;

#define PRIV(__q) (((__q)->q_ptr))
#define UDP_PRIV(__q) ((struct udp *)((__q)->q_ptr))

STATIC struct udp *udp_opens = NULL;
STATIC rwlock_t udp_lock = RW_LOCK_UNLOCKED;

STATIC queue_t *npi_bottom = NULL;

typedef int (*udp_rcv_fnc_t) (struct sk_buff *);

#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		SK_RMEM_MAX
#define xti_default_rcvlowat		1
#define xti_default_sndbuf		SK_WMEM_MAX
#define xti_default_sndlowat		1

#define ip_default_options		{ 0, }
#define ip_default_tos			0
#define ip_default_ttl			64
#define ip_default_reuseaddr		T_NO
#define ip_default_dontroute		T_NO
#define ip_default_broadcast		T_NO

#define udp_default_checksum		T_YES

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_IP_OPTIONS,
	_T_BIT_IP_TOS,
	_T_BIT_IP_TTL,
	_T_BIT_IP_REUSEADDR,
	_T_BIT_IP_DONTROUTE,
	_T_BIT_IP_BROADCAST,
	_T_BIT_IP_ADDR,
	_T_BIT_IP_RETOPTS,
	_T_BIT_UDP_CHECKSUM,
};

#define t_tst_bit(nr,addr) udp_tst_bit(nr,addr)
#define t_set_bit(nr,addr) udp_set_bit(nr,addr)
#define t_clr_bit(nr,addr) udp_clr_bit(nr,addr)

/*
 *  Bind buckets, caches and hashes.
 */
struct udp_bind_bucket {
	struct udp_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct udp_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned short port;		/* port number (host order) */
	struct udp *owners;		/* list of owners of this port */
	struct udp *dflt;		/* default destinations for this protocol */
};
struct udp_bhash_bucket {
	rwlock_t lock;
	struct udp_bind_bucket *list;
};
struct udp_chash_bucket {
	rwlock_t lock;
	struct udp *list;
};

STATIC struct udp_bhash_bucket *udp_bhash;
STATIC size_t udp_bhash_size = 0;
STATIC size_t udp_bhash_order = 0;

STATIC struct udp_chash_bucket *udp_chash;
STATIC size_t udp_chash_size = 0;
STATIC size_t udp_chash_order = 0;

STATIC INLINE fastcall uint
udp_bhashfn(uint16_t num)
{
	return ((udp_bhash_size - 1) & num);
}
STATIC INLINE fastcall uint
udp_chashfn(uint16_t dport, uint16_t sport)
{
	return ((udp_chash_size - 1) & (dport + (sport << 4)));
}

STATIC kmem_cache_t *udp_bind_cachep;
STATIC kmem_cache_t *udp_priv_cachep;

STATIC INLINE struct udp *
udp_get(struct udp *udp)
{
	if (udp)
		atomic_inc(&udp->refcnt);
	return (udp);
}
STATIC INLINE void
udp_put(struct udp *udp)
{
	if (udp && atomic_dec_and_test(&udp->refcnt))
		kmem_cache_free(udp_priv_cachep, udp);
}
STATIC INLINE void
udp_release(struct udp **udpp)
{
	udp_put(xchg(udpp, NULL));
}
STATIC INLINE struct udp *
udp_alloc(void)
{
	struct udp *udp;

	if ((udp = kmem_cache_alloc(udp_priv_cachep, SLAB_ATOMIC))) {
		bzero(udp, sizeof(*udp));
		atomic_set(&udp->refcnt, 1);
		spin_lock_init(&udp->lock);	/* "udp-lock" */
		udp->priv_put = &udp_put;
		udp->priv_get = &udp_get;
	}
	return (udp);
}

/*
 *  State changes.
 */
#ifdef _DEBUG
STATIC const char *
tpi_state_name(t_scalar_t state)
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
STATIC INLINE fastcall void
tpi_set_state(struct udp *udp, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, udp, tpi_state_name(state),
		tpi_state_name(udp->i_state)));
	udp->i_state = udp->info.CURRENT_state = state;
}
STATIC INLINE fastcall long
tpi_get_state(struct udp *udp)
{
	return (udp->i_state);
}
STATIC INLINE fastcall long
tpi_get_statef(struct udp *udp)
{
	return (1 << tpi_get_state(udp));
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

STATIC struct udp_options udp_defaults = {
	{0,}
	,
	{
	 xti_default_debug,
	 xti_default_linger,
	 xti_default_rcvbuf,
	 xti_default_rcvlowat,
	 xti_default_sndbuf,
	 xti_default_sndlowat,
	 }
	,
	{
	 ip_default_options,
	 ip_default_tos,
	 ip_default_ttl,
	 ip_default_reuseaddr,
	 ip_default_dontroute,
	 ip_default_broadcast,
	 }
	,
	{
	 udp_default_checksum,
	 }
};

#define t_defaults udp_defaults

/**
 * t_opts_size - size options from received UDP message
 * @t: private structure
 * @mp: message pointer for UDP message
 */
STATIC int
t_opts_size(const struct udp *t, mblk_t *mp)
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
 * t_opts_build - build options output from received UDP message
 * @t: private structure
 * @mp: message pointer for UDP message
 * @op: output pointer
 * @olen: output length
 */
STATIC int
t_opts_build(const struct udp *t, mblk_t *mp, unsigned char *op, size_t olen)
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
 */
STATIC int
t_errs_size(const struct udp *t, mblk_t *mp)
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
 * t_errs_build - build options output from receive ICMP message
 * @t: private structure
 * @mp: message pointer for ICMP message
 * @op: output pointer
 * @olen: output length
 */
STATIC int
t_errs_build(const struct udp *t, mblk_t *mp, unsigned char *op, size_t olen, int *etypep)
{
	struct iphdr *iph;
	struct udphdr *uh;
	struct t_optlhdr *oh;
	int optlen;

	if (op == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	iph = (struct iphdr *) mp->b_datap->db_base;
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
	assure(oh == NULL);
	return (olen);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * t_opts_parse - parse options from received primitive
 * @opts: pointer to options structure
 * @ip: pointer to options
 * @ilen: length of options
 */
STATIC int
t_opts_parse(unsigned char *ip, size_t ilen, struct udp_options *op)
{
	struct t_opthdr *ih;
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
		      goto error:
		case T_INET_IP:
			switch (ih->name) {
			default:
				goto error;
			case T_IP_OPTIONS:
				t_set_bit(_T_BIT_IP_OPTIONS, op->flags);
				continue;
			case T_IP_TOS:
				t_set_bit(_T_BIT_IP_TOS, op->flags);
				op->ip.tos = *(unsigned char *) T_OPT_DATA(ih);
				continue;
			case T_IP_TTL:
				t_set_bit(_T_BIT_IP_TTL, op->flags);
				op->ip.tos = *(unsigned char *) T_OPT_DATA(ih);
				continue;
			case T_IP_DONTROUTE:
				t_set_bit(_T_BIT_IP_DONTROUTE, op->flags);
				op->ip.dontroute = *(t_uscalar_t *) T_OPT_DATA(ih);
				continue;
			case T_IP_BROADCAST:
				t_set_bit(_T_BIT_IP_BROADCAST, op->flags);
				op->ip.broadcast = *(t_uscalar_t *) T_OPT_DATA(ih);
				continue;
			case T_IP_ADDR:
				t_set_bit(_T_BIT_IP_ADDR, op->flags);
				op->ip.addr = *(uint32_t *) T_OPT_DATA(ih);
				continue;
			case T_IP_REUSEADDR:
				t_set_bit(_T_BIT_IP_REUSEADDR, op->flags);
				op->ip.reuseaddr = *(t_uscalar_t *) T_OPT_DATA(ih);
				continue;
			}
		case T_INET_UDP:
			switch (ih->name) {
			default:
				goto error;
			case T_UDP_CHECKSUM:
				t_set_bit(_T_BIT_UDP_CHECKSUM, op->flags);
				op->udp.checksum = *(t_uscalar_t *) T_OPT_DATA(ih);
				continue;
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
STATIC int
t_size_default_options(struct udp *t, const unsigned char *ip, size_t ilen)
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
		}
	}
	ptrace(("%p: Calculated option output size = %u\n", t, olen));
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
STATIC int
t_size_current_options(struct udp *t, const unsigned char *ip, size_t ilen)
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
		}
	}
	ptrace(("%p: Calculated option output size = %u\n", t, olen));
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
STATIC int
t_size_check_options(const struct udp *t, const unsigned char *ip, size_t ilen)
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
		}
	}
	ptrace(("%p: Calculated option output size = %u\n", t, olen));
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
STATIC int
t_size_negotiate_options(const struct udp *t, const unsigned char *ip, size_t ilen)
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
		}
	}
	ptrace(("%p: Calculated option output size = %u\n", t, olen));
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
t_overall_result(uint * overall, uint result)
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
STATIC t_scalar_t
t_build_default_options(const struct udp * t, const unsigned char *ip, size_t ilen,
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
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
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
STATIC t_scalar_t
t_build_current_options(const struct udp * t, const unsigned char *ip, size_t ilen,
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
STATIC t_scalar_t
t_build_check_options(const struct udp * t, const unsigned char *ip, size_t ilen, unsigned char *op,
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
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
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
STATIC t_scalar_t
t_build_negotiate_options(struct udp * t, const unsigned char *ip, size_t ilen, unsigned char *op,
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
					*valp = sysctl_wmem_default;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
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
STATIC t_scalar_t
t_build_options(struct udp * t, unsigned char *ip, size_t ilen, unsigned char *op, size_t *olen,
		t_scalar_t flag)
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
 * UDP actions
 */

/**
 * udp_lookup_conn - look up a stream in the connection hashes
 * @dport: destination port (of recieved packet)
 * @sport: soruce port (of recieved packet)
 * @daddr: destination address (of recieved packet)
 * @saddr: source address (of recieved packet)
 */
STATIC INLINE fastcall __hot_in struct udp *
udp_lookup_conn(uint16_t dport, uint16_t sport, uint32_t daddr, uint32_t saddr)
{
	struct udp *udp;
	struct udp_chash_bucket *hp = &udp_chash[udp_chashfn(dport, sport)];

	read_lock(&hp->lock);
	for (udp = hp->list; udp; udp = udp->cnext) {
		struct sockaddr_in *sin = (struct sockaddr_in *) &udp->srce;
		struct sockaddr_in *din = (struct sockaddr_in *) &udp->dest;

		if (sin->sin_port == dport && din->sin_port == sport
		    && sin->sin_addr.s_addr == daddr && din->sin_addr.s_addr == saddr)
			break;
	}
	udp_get(udp);
	read_unlock(&hp->lock);
	return (udp);
}

/**
 * udp_lookup_bind - look up a stream in the bind hashes
 * @dport: destination port (of received packet)
 * @daddr: destination address (of received packet)
 *
 * There are two types of Streams that exist in the bind hashes: CLTS Streams that need to be sent
 * received messages, and COTS Streams that are bound listening or not.  When performing a lookup
 * for receive packets, we are only interested in CLTS Streams or pseudo-COTS Streams that are bound
 * in a listening state.
 */
STATIC INLINE fastcall __hot_in struct udp *
udp_lookup_bind(uint16_t dport, uint32_t daddr)
{
	struct udp *result = NULL;
	int snum = htohs(dport);
	struct udp_bind_bucket *ub;
	struct udp_bhash_bucket *hp = &udp_bhash[udp_bhashfn(snum)];

	read_lock(&hp->lock);
	for (ub = hp->list; ub && ub->port != snum; ub = ub->next) ;
	if (ub) {
		struct udp *udp;
		int hiscore = 0;

		for (udp = ub->owners; udp; udp = udp->bnext) {
			int score = 0;
			struct sockaddr_in *sin = (struct sockaddr_in *) &udp->srce;
			t_uscalar_t state;
			uint16_t sport;
			uint32_t saddr;

			/* only listening T_COTS(_ORD) Streams and T_CLTS Streams */
			if (udp->CONIND_number == 0 && udp->info.SERV_type != T_CLTS)
				continue;
			/* only Streams in the (close to) correct state */
			if ((state = tpi_get_state(udp)) != TS_IDLE && state != TS_WACK_UREQ)
				continue;
			sport = sin->sin_port;
			if (sport == 0)
				continue;
			if (boprt != dport)
				continue;
			score++;
			saddr = sin->sin_addr.s_addr;
			if (saddr != 0) {
				if (saddr != daddr)
					continue;
				score++;
			}
			if (score == 2) {
				/* perfect match */
				result = udp;
				break;
			}
			if (score > hiscore) {
				hiscore = score;
				result = udp;
			}
		}
	}
	udp_get(result);
	read_unlock(&hp->lock);
	usual(result);
	return (result);
}

STATIC INLINE fastcall __hot_in struct udp *
udp_lookup(uint16_t dport, uint16_t sport, uint32_t daddr, uint32_t saddr)
{
	struct udp *udp;

	if ((udp = udp_lookup_conn(dport, sport, daddr, saddr)))
		return (udp);
	if ((udp = udp_lookup_bind(dport, daddr)))
		return (udp);
	return (NULL);
}

/**
 * udp_bind - bind a Stream to a TSAP
 * @q: active queue in queue pair (write queue)
 * @add: address to bind
 * @add_len: length of address
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
STATIC int
udp_bind(struct udp *udp, struct sockaddr *add, socklen_t add_len)
{
	static unsigned short udp_prev_port = 10000;
	static const unsigned short udp_frst_port = 10000;	/* XXX */
	static const unsigned short udp_last_port = 16000;	/* XXX */

	struct udp_bind_bucket *bp;
	struct sockaddr_in *sin = (struct sockaddr_in *) add;
	unsigned short bport = sin->sin_port;
	unsigned short num = 0;
	int err = 0;

	if (bport == 0) {
		num = udp_prev_port;	/* UNSAFE */
		bport = htons(num);
	}
	for (;;) {
		struct udp *test;

		bp = &udp_bhash[udp_bhashfn(bport)];
		write_lock(&bp->lock);
		for (test = bp->list; test; test = test->bnext) {
			struct sockaddr_in *sit = (struct sockaddr_in *) &test->srce;

			if (bport == sit->sin_port &&
			    (sit->sin_addr.s_addr == 0
			     || sit->sin_addr.s_addr = sin->sin_addr.s_addr))
				break;
		}
		if (test != NULL) {
			write_unlock(&bp->lock);
			if (num == 0)
				/* specific port number requested */
				return (TADDRBUSY);
			if (++num > udp_last_port)
				num = udp_frst_port;
			if (num != udp_prev_port)
				continue;
			return (TNOADDR);
		}
		if (num != 0)
			udp_prev_port = num;
		break;
	}
	sin->sin_port = bport;
	bcopy(sin, &udp->srce, sizeof(*sin));
	if ((udp->bnext = bp->list))
		udp->bnext->bprev = &udp->bnext;
	udp->bprev = &bp->list;
	bp->list = udp_get(udp);
	udp->bindb = bp;
	write_unlock(&bp->lock);
	return (0);
}

/**
 * udp_unbind - unbind a Stream from an NSAP
 * @udp: UDP private structure
 *
 * Simply remove it from the hashes.  This function can be called whether the stream is bound or
 * not (and is always called before the private structure is freed.
 */
STATIC int
udp_unbind(struct udp *udp)
{
	struct udp_bind_bucket *bp;

	if ((bp = udp->bindb)) {
		write_lock(&bp->lock);
		if ((*udp->bprev = udp->bnext))
			udp->bnext->bprev = udp->bprev;
		udp->bnext = NULL;
		udp->bprev = &udp->bnext;
		udp->bindb = NULL;
		write_unlock(&bp->lock);
		bzero(&udp->srce, sizeof(udp->srce));
		udp_release(&udp);
	}
	return (0);
}

/**
 * udp_connect - connect a Stream to an TSAP
 * @q: active queue (write queue)
 * @add: address to connect to
 * @add_len: length of address
 *
 * Connect to the specified port and address.  If the connection results in a conflict, TADDRBUSY is
 * returned, zero (0) on success.
 */
STATIC int
udp_connect(struct udp *udp, struct sockaddr *add, socklen_t add_len)
{
	struct udp *up;
	struct sockaddr_in *sin, *din;
	uint16_t sport, dport;
	uint32_t saddr, daddr;
	struct udp_chash_bucket *hp;

	sin = (struct sockaddr_in *) &udp->srce;
	sport = sin->sin_port;
	saddr = sin->sin_addr.s_addr;
	din = (struct sockaddr_in *) add;
	dport = din->sin_port;
	daddr = din->sin_addr.s_addr;
	hp = &udp_chash[udp_chashfn(dport, sport)];

	write_lock(&hp->lock);
	for (up = hp->list; up; up = up->cnext) {
		sin = (typeof(sin)) & up->srce;
		din = (typeof(din)) & up->dest;
		if (sin->sin_port == sport && din->sin_port == dport)
			break;
	}
	if (up != NULL) {
		write_unlock(&hp->lock);
		return (TADDRBUSY);
	}
	if ((udp->cnext = hp->list))
		udp->cnext->cprev = &udp->cnext;
	udp->cprev = &hp->list;
	hp->list = udp_get(udp);
	udp->hashb = hp;
	din = (typeof(din)) & udp->dest;
	din->sin_family = AF_INET;
	din->sin_port = dport;
	din->sin_addr.s_addr = daddr;
	write_unlock(&hp->lock);
	return (0);
}

/**
 * udp_disconnect - disconnect from the connection hashes
 * @q: active queue (write queue)
 */
STATIC int
udp_disconnect(struct udp *udp)
{
	struct udp_chash_bucket *hp;

	if ((hp = udp->hashb)) {
		write_lock(&hp->lock);
		if ((*udp->cprev = udp->cnext))
			udp->cnext->cprev = udp->cprev;
		udp->cnext = NULL;
		udp->cprev = &udp->cnext;
		udp->hashb = NULL;
		write_unlock(&hp->lock);
		bzero(&udp->dest, sizeof(udp->dest));
		udp_release(&udp);
	}
	return (0);
}

#if defined HAVE_KFUNC_DST_OUTPUT
STATIC INLINE int
udp_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;

#if defined NETIF_F_TSO
	ip_select_ident_more(iph, &rt->u.dst, NULL, 0);
#else				/* !defined NETIF_F_TSO */
	ip_select_ident(iph, &rt->u.dst, NULL);
#endif				/* defined NETIF_F_TSO */
	ip_send_check(iph);
#if defined HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, ip_dst_output);
#else				/* !defined HAVE_KFUNC_IP_DST_OUTPUT */
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, dst_output);
#endif				/* defined HAVE_KFUNC_IP_DST_OUTPUT */
}
#else				/* !defined HAVE_KFUNC_DST_OUTPUT */
STATIC INLINE int
udp_queue_xmit(struct sk_buff *skb)
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
#endif				/* defined HAVE_KFUNC_DST_OUTPUT */

/**
 * udp_xmitmsg - send a message from a Stream
 * @q: active queue in queue pair (write queue)
 * @mp: T_UNITDATA_REQ message
 * @opts: UDP options for send
 *
 * TODO: We need to handle IP options specified using the T_IP_OPTIONS option at the T_INET_IP
 * level.  Currently these are being ignored and all packets are being sent without IP options of
 * any kind. (We are, however, processing them on receive.)
 *
 * Yes, it is possible with STREAMS to use the Van Jacobson approach (simultaneous copy from user
 * and checksum); however, we need to place this function in the Stream head.  The approach would
 * specify to the Stream head the form of the checksum to use and then each mblk_t containing data
 * copied from the user would also contain a partial checksum of the appropriate type.  That is for
 * later.
 */
STATIC INLINE fastcall __hot_out int
udp_xmitmsg(queue_t *q, mblk_t *dp, struct sockaddr_in *sin, struct udp_options *opts)
{
	struct udp *udp = UDP_PRIV(q);
	struct rtable *rt = NULL;
	int err;

	if (likely((err = ip_route_output(&rt, sin->sin_addr.s_addr, 0, 0, 0)) == 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t ulen = msgdsize(dp);
		size_t plen = sizeof(struct udphdr) + ulen;
		size_t tlen = sizeof(struct iphdr) + plen;

		if (likely((skb = alloc_skb(hlen + tlen, GFP_ATOMIC)) != NULL)) {
			mblk_t *bp;
			struct iphdr *iph;
			struct udphdr *uh;
			unsigned char *data;

			skb_reserve(skb, hlen);
			/* find headers */
			iph = (typeof(iph)) __skb_put(skb, tlen);
			/* We need to process IP options, but are not doing so yet. */
			uh = (typeof(uh)) (iph + 1);
			data = (unsigned char *) (uh + 1);
			uh->dest = sin->sin_port;
			uh->source = htons(udp->port);
			uh->len = htons(ulen);
			uh->check = 0;
			skb->dst = &rt->u.dst;
			/* Should probably add an XTI_PRIORITY option at the XTI_GENERIC level. */
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			/* Can be set using the T_IP_TOS option at the T_INET_IP level. */
			iph->tos = opts->ip.tos;
			iph->frag_off = 0;
			/* Can be set using the T_IP_TTL option at the T_INET_IP level. */
			iph->ttl = opts->ip.ttl;
			iph->daddr = rt->rt_dst;
			/* Yes the TS user can override the source address using the T_IP_ADDR
			   option at the T_INET_IP level. */
			if (t_tst_bit(_T_BIT_IP_ADDR, opts.flags))
				iph->saddr = opts.ip.addr;
			else
				iph->saddr = rt->rt_src;
			iph->protocol = IPPROTO_UDP;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
#if !defined HAVE_KFUNC_DST_OUTPUT
#if defined HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
			__ip_select_ident(iph, &rt->u.dst);
#elif defined HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
			__ip_select_ident(iph, &rt->u.dst, 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
#endif				/* !defined HAVE_KFUNC_DST_OUTPUT */
			for (bp = dp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;

				if (likely(blen > 0)) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
				} else
					rare();
			}
			uh->check = 0;
			fixme(("Need a checksum function."));
			if (opts->udp.checksum == T_YES)
				if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
					uh->check = htonl(cksum_generate(uh, plen));
			// UDP_INC_STATS(UdpOutPackets);
#if defined HAVE_KFUNC_DST_OUTPUT
			udp_queue_xmit(skb);
#else				/* !defined HAVE_KFUNC_DST_OUTPUT */
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, udp_queue_xmit);
#endif				/* defined HAVE_KFUNC_DST_OUTPUT */
			return (QR_DONE);
		}
		/* Note that this is kind of a sneaky trick.  When we are able to allocate a
		   message block of the appropriate size, the chances of allocating an sk_buff of
		   the same size are also likely. */
		ss7_bufcall(q, hlen + tlen, BPRI_MED);
		return (-ENOBUFS);
	}
	if (err == -ENOMEM) {
		/* Note that this is kind of a sneaky trick.  When we are able to allocate a
		   message block of the appropriate size, the chances of allocating an rtable entry
		   are also likely. */
		ss7_bufcall(q, sizeof(*rt), BPRI_MED);
		return (-ENOMEM);
	}
	return (err);
}

/*
 *  TS Provider -> TS User (Indication, Confirmation and Ack) Primitives
 */

/**
 * m_flush: deliver an M_FLUSH message upstream
 * @q: a queue in the queue pair
 * @how: FLUSHBAND or FLUSHALL
 * @band: band to flush if how is FLUSHBAND
 */
STATIC int
m_flush(queue_t *q, int how, int band)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;

	if (unlikely((mp = ss7_allocb(q, 2, BPRI_HI)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = how;
	*mp->b_wptr++ = band;
	putnext(udp->oq, mp);
	return (QR_DONE);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error: deliver an M_ERROR message upstream
 * @q: a queue in the queue pair
 * @error: the error to deliver
 * @mp: message to reuse
 */
STATIC int
m_error(queue_t *q, int error, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *pp = mp;
	int hangup = 0;

	switch (error) {
	case 0:
		seldom();
		return (0);
	case -EBUSY:
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOMEM:
		return (error);
	case -EPIPE:
	case -ENETDOWN:
	case -EHOSTUNREACH:
		hangup = 1;
		error = EPIPE;
		break;
	default:
		error = EPROTO;
		break;
	}
	if (unlikely((mp == NULL || mp->b_datap->db_refs > 1) && (mp = ss7_allocb(q, 2, BPRI_HI)) == NULL))
		goto enobufs;
	mp->b_wptr = mp->b_rptr = mp->b_datap->db_base;
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	if (hangup) {
		printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, udp));
		mp->b_datap->db_type = M_HANGUP;
		mp->b_band = 0;
	} else {
		printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, udp, error));
		mp->b_datap->db_type = M_ERROR;
		mp->b_band = 0;
		*(mp->b_wptr)++ = error;
		*(mp->b_wptr)++ = error;
	}
	qreply(q, mp);
	return (pp == mp) ? (QR_ABSORBED) : (QR_DONE);
      enobufs:
	rare();
	return (-ENOBUFS);
}

/**
 * t_info_ack: send a T_INFO_ACK upstream
 * @q: a queue in the queue pair
 */
STATIC int
t_info_ack(queue_t *q)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	*p = udp->info;
	mp->b_wptr += sizeof(*p);
	printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, udp));
	putnext(udp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * t_bind_ack: send a T_BIND_ACK upstream with state changes
 * @q: a queue in the queue pair
 * @add: bound address
 * @add_len: length of the address
 * @conind: number of connection indications
 */
STATIC int
t_bind_ack(queue_t *q, struct sockaddr *add, socklen_t add_len, t_uscalar_t conind)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_bind_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = conind;
	mp->b_wptr += sizeof(*p);
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	tpi_set_state(udp, TS_IDLE);
	printd(("%s: %p: <- T_BIND_ACK\n", DRV_NAME, udp));
	putnext(udp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * t_error_ack: deliver a T_ERROR_ACK upstream with state changes
 * @q: active queue (write queue)
 * @prim: primitive in error
 * @err: TLI or UNIX error number
 * @mp: message block to reuse
 */
STATIC int
t_error_ack(queue_t *q, t_scalar_t prim, mblk_t *mp, t_scalar_t error)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_error_ack *p;
	mblk_t *pp = mp;
	int err;

	/* rollback state */
	tpi_set_state(udp, udp->i_oldstate);
	udp->i_oldstate = tpi_get_state(udp);

	err = error;
	switch (error) {
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		goto error;
	case 0:
		err = -EFAULT;
		goto error;
	}
	err = -ENOBUFS;
	if ((mp == NULL || mp->b_datap->db_refs > 1)
	    && (mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL)
		goto error;
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error > 0 ? error : TSYSERR;
	p->UNIX_error = error > 0 ? 0 : -error;
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, udp));
	qreply(q, mp);
	return (mp == pp) ? (QR_ABSORBED) : (QR_DONE);
      error:
	return (err);
}

/**
 * t_ok_ack: deliver a T_OK_ACK upstream with state changes
 * @q: a queue in the queue pair
 * @prim: correct primitive
 * @mp: message block to reuse
 * @cp: connection indication
 * @dp: user data
 * @ap; accepting stream
 */
STATIC INLINE fastcall __hot_put int
t_ok_ack(queue_t *q, t_scalar_t prim, mblk_t *mp, mblk_t *cp, mblk_t *dp, struct udp *ap)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_ok_ack *p;
	mblk_t *pp = mp;
	int err;

	err = -ENOBUFS;
	if ((mp == NULL || mp->b_datap->db_refs > 1)
	    && (mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL)
		goto error;
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	switch (tpi_get_state(udp)) {
	case TS_WACK_UREQ:
		if ((err = udp_unbind(udp)))
			goto error;
		/* TPI spec says that if the provider must flush both queues before responding with 
		   a T_OK_ACK primitive when responding to a T_UNBIND_REQ. This is to flush queued
		   data for connectionless providers. */
		if ((err = m_flush(q, FLUSHRW, 0)))
			goto error;
		tpi_set_state(udp, TS_UNBND);
		break;
	case TS_WACK_CREQ:
		if ((err = udp_connect(udp, &udp->dest, sizeof(udp->dest))))
			goto error;
		tpi_set_state(udp, TS_WCON_CREQ);
		if ((err = udp_xmitmsg(q, dp, &udp->options))) {
			udp_disconnect(udp);
			goto error;
		}
		break;
	case TS_WACK_CRES:
		if ((err = udp_connect(ap, &ap->dest, sizeof(ap->dest))))
			goto error;
		ap->i_oldstate = tpi_get_state(ap);
		tpi_set_state(ap, TS_DATA_XFER);
		if ((err = udp_xmitmsg(q, dp, &ap->options))) {
			udp_disconnect(ap);
			tpi_set_state(ap, ap->i_oldstate);
			goto error;
		}
		bufq_dequeue(&udp->conq, cp);
		freeb(XCHG(&cp, cp->b_cont));
		/* queue any pending data */
		while (cp)
			put(ap->oq, XCHG(&cp, cp->b_cont));
		if (ap != udp) {
			if (bufq_length(&udp->conq) > 0)
				tpi_set_state(udp, TS_WRES_CIND);
			else
				tpi_set_state(udp, TS_IDLE);
		}
		break;
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		if (cp != NULL) {
			bufq_dequeue(&udp->conq, cp);
			freemsg(cp);
		} else
			udp_disconnect(udp)
			    tpi_set_state(udp,
					  (bufq_length(&udp->conq) > 0) ? TS_WRES_CIND : TS_IDLE);
		break;
	default:
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
	}
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, udp));
	qreply(q, mp);
	return (mp == pp) ? (QR_ABSORBED) : (QR_DONE);
      error:
	if (mp != pp)
		freemsg(mp);
	return t_error_ack(q, prim, pp, err);
}

/**
 * t_reply_ack: send a T_OK_ACK or T_ERROR_ACK upstream with state changes
 * @q: a queue in the queue pair
 * @prim: primitive in error
 * @error: TPI/UNIX error
 * @mp: message to reuse
 * @cp: connection indication pointer
 */
STATIC int
t_reply_ack(queue_t *q, t_scalar_t prim, mblk_t *mp, t_scalar_t error, mblk_t *cp, mblk_t *dp,
	    struct udp *ap)
{
	if (error == 0)
		return t_ok_ack(q, prim, mp, cp, dp, ap);
	return t_error_ack(q, prim, mp, error);
}

/**
 * t_unitdata_ind - send a T_UNITDATA_IND upstream
 * @q: read queue
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block contains the IP, UDP header and UDP payload starting at
 * dp->b_datap->db_base.  The UDP message payload starts at dp->b_rptr.  This function extracts IP
 * header information and uses it to create options.
 */
STATIC INLINE fastcall __hot_get int
t_unitdata_ind(queue_t *q, mblk_t *dp)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_unitdata_ind *p;
	t_scalar_t OPT_length, SRC_length;

	if (unlikely(tpi_get_state(udp) != TS_IDLE))
		goto discard;
	OPT_length = t_opts_size(udp, dp);
	SRC_length = sizeof(struct sockaddr_in);
	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + SRC_length + OPT_length, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + SRC_length : 0;
	if (SRC_length) {
		struct sockaddr_in *sin = (typeof(sin)) mp->b_wptr;
		struct iphdr *iph = (typeof(iph)) dp->b_datap->db_base;
		struct udphdr *uh = (typeof(uh)) (dp->b_datap->db_base + (iph->ihl << 2));

		sin->sin_family = AF_INET;
		sin->sin_port = uh->source;
		sin->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += SRC_length;
	}
	if (OPT_length) {
		t_opts_build(udp, dp, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	dp->b_datap->db_type = M_DATA;
	mp->b_cont = dp;
	printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, udp));
	putnext(q, mp);
	return (QR_ABSORBED);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * t_conn_ind - send a T_CONN_IND upstream
 * @q: read queue
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block contains the IP, UDP header and UDP payload starting at
 * dp->b_datap->db_base.  The UDP message payload starts at dp->b_rptr.  This function extracts IP
 * header information and uses it to create options.
 */
STATIC INLINE fastcall __hot_get int
t_conn_ind(queue_t *q, mblk_t *dp)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp, *cp;
	struct T_conn_ind *p;
	t_scalar_t OPT_length, SRC_length;
	struct sockaddr_in *sin;
	struct iphdr *iph;
	struct udphdr *uh;

	if (unlikely(tpi_get_statef(udp) & ~(TSF_IDLE | TSF_WRES_CIND | TSF_WACK_CRES)))
		goto discard;

	iph = (typeof(iph)) dp->b_datap->db_base;
	uh = (typeof(uh)) (dp->b_datap->db_base + (iph->ihl << 2));

	spin_lock_bh(&udp->conq.q_lock);
	for (cp = bufq_head(&udp->conq); cp; cp = cp->b_next) {
		p = (typeof(p)) cp->b_rptr;
		sin = (typeof(sin)) (cp->b_rptr + p->SRC_offset);
		if (sin->sin_port != uh->source || sin->sin_addr.s_addr != iph->saddr)
			continue;
		break;
	}
	if (cp != NULL) {
		/* already have a connection indication, link the data */
		linkb(cp, dp);
		spin_unlock_bh(&udp->conq.q_lock);
		return (QR_ABSORBED);
	}
	spin_unlock_bh(&udp->conq.q_lock);

	/* If there are already too many connection indications outstanding, discard further
	   connection indications until some are accepted.  Note that data for existing outstanding
	   connection indications is preserved above. */
	if (unlikely(bufq_length(&udp->conq) >= udp->CONIND_number))
		goto eagain;
	/* If there is already a connection accepted on the listening stream, discard further
	   connection indications until the current connection disconnects */
	if (unlikely(tpi_get_statef(udp) & ~(TSF_IDLE | TSF_WRES_CIND)))
		goto eagain;

	OPT_length = t_opts_size(udp, dp);
	SRC_length = sizeof(struct sockaddr_in);

	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + SRC_length + OPT_length, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + SRC_length : 0;
	if (SRC_length) {
		struct sockaddr_in *sin = (typeof(sin)) mp->b_wptr;

		sin->sin_family = AF_INET;
		sin->sin_port = uh->source;
		sin->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += SRC_length;
	}
	if (OPT_length) {
		t_opts_build(udp, dp, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	if (unlikely((cp = ss7_copyb(q, mp)) == NULL))
		goto free_enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	bufq_queue(&udp->conq, cp);
	dp->b_datap->db_type = M_DATA;
	mp->b_cont = dp;
	tpi_set_state(udp, TS_WRES_CIND);
	printd(("%s: %p: <- T_CONN_IND\n", DRV_NAME, udp));
	putnext(q, mp);
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
 * t_optdata_ind - send a T_OPTDATA_IND upstream
 * @q: read queue
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block containst the IP, UDP header and UDP payload, starting at
 * dp->b_datap->db_base.  The UDP message payload starts at dp->b_rptr.  This function extracts IP
 * header information and uses it to create options.
 */
STATIC INLINE fastcall __hot_get int
t_optdata_ind(queue_t *q, mblk_t *dp)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_optdata_ind *p;
	t_scalar_t OPT_length = t_opts_size(udp, dp);

	if (unlikely(tpi_get_statef(udp) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto discard;
	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + OPT_length, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = 0;
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) : 0;
	if (OPT_length) {
		t_opts_build(udp, dp, mp->b_wptr, OPT_length);
		mp->b_wptr += OPT_length;
	}
	dp->b_datap->db_type = M_DATA;
	mp->b_cont = dp;
	printd(("%s: %p: <= T_OPTDATA_IND\n", DRV_NAME, udp));
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

/**
 * t_uderror_ind - send a T_UDERROR_IND upstream
 * @q: read queue
 * @dp: data block containing ICMP packet
 *
 * IMPLEMENTATION: The data block contains the ICMP header, IP header and UDP header starting at
 * dp->b_datap->db_base.  The UDP header starts at dp->b_rptr. This function extracts IP header
 * information and uses it to create options.
 */
STATIC INLINE int
t_uderror_ind(queue_t *q, mblk_t *dp)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_uderror_ind *p;
	t_uscalar_t OPT_length = t_errs_size(udp, dp);
	t_uscalar_t ERROR_type = 0;
	const t_uscalar_t SRC_length = sizeof(struct sockaddr_in);

	if (unlikely(tpi_get_statef(udp) & ~(TSF_IDLE)))
		goto discard;
	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + SRC_length + OPT_length, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(udp->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = SRC_length;
	p->DEST_offset = sizeof(*p);
	p->OPT_length = OPT_length;
	p->OPT_offset = OPT_length ? sizeof(*p) + SRC_length : 0;
	mp->b_wptr += sizeof(*p);
	{
		struct sockaddr_in *sin = (typeof(sin)) mp->b_wptr;
		struct icmphdr *icmph = (typeof(icmph)) dp->b_datap->db_base;
		struct iphdr *iph = (typeof(iph)) (icmph + 1);
		struct udphdr *uh = (typeof(uh)) dp->b_rptr;

		sin->sin_family = AF_INET;
		sin->sin_port = uh->dest;
		sin->sin_addr.s_addr = iph->daddr;
		mp->b_wptr += SRC_length;

		ERROR_type = ((t_uscalar_t) icmph->type << 8) | ((t_uscalar_t) icmph->code);
	}
	if (OPT_length) {
		t_errs_build(udp, dp, mp->b_wptr, OPT_length, &ERROR_type);
		mp->b_wptr += OPT_length;
	}
	p->ERROR_type = ERROR_type;
	dp->b_datap->db_type = M_DATA;
	mp->b_cont = dp;
	printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, udp));
	putnext(udp->oq, mp);
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
 * t_discon_ind - send a T_DISCON_IND upstream
 * @q: read queue
 * @dp: data block containing ICMP packet
 */
STATIC INLINE int
t_discon_ind(queue_t *q, mblk_t *dp)
{
	struct upd *udp = UDP_PRIV(q);
	mblk_t *mp, *cp;
	union T_primitives *p;
	struct sockaddr_in *sin;
	struct iphdr *iph;
	struct udphdr *uh;
	struct icmphdr *icmph;
	t_uscalar_t DISCON_reason = 0;

	if (unlikely(tpi_get_statef(udp) & ~(TSF_WRES_CIND | TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto discard;

	icmph = (typeof(icmph)) dp->b_datap->db_base;
	iph = (typeof(iph)) (icmph + 1);
	uh = (typeof(uh)) dp->b_rptr;

	spin_lock_bh(&udp->conq.q_lock);
	for (cp = bufq_head(&udp->conq); cp; cp = cp->b_next) {
		p = (typeof(p)) cp->b_rptr;
		sin = (typeof(sin)) (cp->b_rptr + p->conn_ind.SRC_offset);
		if (sin->sin_port != uh->dest || sin->sin_addr.s_addr != iph->daddr)
			continue;
		break;
	}
	if (cp != NULL)
		/* have a connection indication, unlink it */
		__bufq_dequeue(&udp->conq, cp);
	spin_unlock_bh(&udp->conq.q_lock);

	if (cp == NULL
	    && (tpi_get_statef(udp) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL)))
		goto discard;

	if (unlikely((mp = ss7_allocb(q, sizeof(p->discon_ind), BPRI_MED)) == NULL))
		goto enobufs;

	DISCON_reason = ((t_uscalar_t) icmph->type << 8) | ((t_uscalar_t) icmph->code);

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(p->discon_ind);
	p->discon_ind.PRIM_type = T_DISCON_IND;
	p->discon_ind.DISCON_reason = DISCON_reason;
	p->discon_ind.SEQ_number = (t_uscalar_t) (long) cp;
	if (cp)
		freemsg(cp);
	tpi_set_state(udp, bufq_length(&udp->conq) > 0 ? TS_WRES_CIND : TS_IDLE);
	printd(("%s: %p: <- T_DISCON_IND\n", DRV_NAME, udp));
	putnext(q, mp);
      discard:
	return (QR_DONE);
      enobufs:
	if (cp != NULL)
		bufq_queue(&udp->conq, cp);
	return (-ENOBUFS);
}

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
STATIC int
t_optmgmt_ack(queue_t *q, t_scalar_t flags, unsigned char *req, size_t req_len, size_t opt_len)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	if ((flags = t_build_options(udp, req, req_len, mp->b_wptr, &opt_len, flags)) < 0) {
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
	if (tpi_get_state(udp) == TS_WACK_OPTREQ)
		tpi_set_state(udp, TS_IDLE);
#endif
	printd(("%s: %p: <- T_OPTMGMT_ACK\n", DRV_NAME, udp));
	putnext(udp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/**
 * t_addr_ack - send a T_ADDR_ACK upstream
 * @q: a queue in the queue pair
 * @loc: local address
 * @loc_len: local address length
 * @rem: remote address
 * @rem_len: remote address length
 */
STATIC int
t_addr_ack(queue_t *q, struct sockaddr *loc, socklen_t loc_len, struct sockaddr *rem,
	   socklen_t rem_len)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_addr_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = loc_len;
	p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
	p->REMADDR_length = rem_len;
	p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
	mp->b_wptr += sizeof(*p);
	if (loc_len) {
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
	}
	if (rem_len) {
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
	}
	printd(("%s: %p: <- T_ADDR_ACK\n", DRV_NAME, udp));
	putnext(udp->oq, mp);
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
STATIC int
t_capability_ack(queue_t *q, t_uscalar_t caps, int type)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	struct T_capability_ack *p;

	if (unlikely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = type;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = caps & (TC1_INFO | TC1_ACCEPTOR_ID);
	p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (long) udp->oq : 0;
	mp->b_wptr += sizeof(*p);
	if (caps & TC1_INFO) {
		p->INFO_ack = udp->info;
	} else
		bzero(&p->INFO_ack, sizeof(p->INFO_ack));
	printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, udp));
	putnext(udp->oq, mp);
	return (0);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif				/* T_CAPABILITY_ACK */

/*
 *  TS User -> TS Provider (Request and Response) primitives
 *  ========================================================
 */

/**
 * t_info_req - process a T_INFO_REQ primitive
 * @q: active queue in queue pair
 * @mp: the message
 */
STATIC int
t_info_req(queue_t *q, mblk_t *mp)
{
	return t_info_ack(q);
}

/**
 * t_bind_req - process a T_BIND_REQ primitive
 * @q: active queue in queue pair
 * @mp: the message
 */
STATIC int
t_bind_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	int err, add_len, type;
	const struct T_bind_req *p;
	struct sockaddr *add = (struct sockaddr *) &udp->srce;
	struct sockaddr_in *add_in;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_BIND_REQ))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tpi_get_state(udp) != TS_UNBND))
		goto error;
	tpi_set_state(udp, TS_WACK_BREQ);
	err = TBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto error;
	if ((add_len = p->ADDR_length) == 0) {
		bzero(add, sizeof(add));
		add_len = sizeof(struct sockaddr_in);
		add->sa_family = AF_INET;
	} else {
		err = TBADADDR;
		if (unlikely(add_len < sizeof(struct sockaddr_in)))
			goto error;
		if (unlikely(add_len > sizeof(struct sockaddr_in)))
			goto error;
		if (unlikely(add->sa_family != AF_INET && add->sa_family != 0))
			goto error;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	}
	add_in = (typeof(add_in)) add;
	type = inet_addr_type(add_in->sin_addr.s_addr);
	err = TNOADDR;
	if (sysctl_ip_nonlocal_bind == 0 && add_in->sin_addr.s_addr != INADDR_ANY
	    && type != RTN_LOCAL && type != RTN_MULTICAST && type != RTN_BROADCAST)
		goto error;
	udp->port = ntohs(add_in->sin_port);
	/* check for bind to privileged port */
	err = TACCES;
	if (udp->port && udp->port < PROT_SOCK && !capable(CAP_NET_BIND_SERVICE))
		goto error;
	if ((err = udp_bind(udp, add, add_len)))
		goto error;
	return t_bind_ack(q, (struct sockaddr *) &udp->srce, sizeof(struct sockaddr_in),
			  p->CONIND_number);
      error:
	return t_error_ack(q, T_BIND_REQ, mp, err);
}

/**
 * t_unbind_req - process a T_UNBIND_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	int err;

	err = TOUTSTATE;
	if (unlikely(tpi_get_state(udp) != TS_IDLE))
		goto error;
	tpi_set_state(udp, TS_WACK_UREQ);
	err = 0;
      error:
	return t_reply_ack(q, T_UNBIND_REQ, mp, err, NULL);
}

/**
 * t_conn_req - process a T_CONN_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC int
t_conn_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	union T_primitives *p;
	struct T_conn_req *p;
	struct T_ok_ack *r;
	struct T_error_ack *e;
	struct T_conn_con *c;
	int err;
	size_t mlen;
	mblk_t *dp, *rp = NULL;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->conn_req)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->conn_req.PRIM_type != T_CONN_REQ))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type == T_CLTS && udp->info.CURRENT_state != TS_UNBND))
		goto error;
	err = TOUTSTATE;
	if (unlikely(!(tpi_get_statef(udp) & (TSF_IDLE | TSF_UNBND))))
		goto error;
	err = TNOADDR;
	if (unlikely(p->conn_req.DEST_length == 0))
		goto error;
	err = TBADADDR;
	if (unlikely(p->conn_req.DEST_length < udp->info.ADDR_size))
		goto error;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->conn_req.DEST_offset + p->conn_req.DEST_length))
		goto error;
	err = TBADOPT;
	if (unlikely(p->conn_req.OPT_length > udp->info.OPT_size))
		goto error;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->conn_req.OPT_offset + p->conn_req.OPT_length))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) != NULL))
		if (unlikely((mlen = msgsize(dp)) == 0 || mlen > udp->info.CDATA_size))
			goto error;
	if (likely(p->conn_req.DEST_length != 0)) {
		struct sockaddr_in *sin = (struct sockaddr_in *) &udp->dest;

		bcopy(mp->b_rptr + p->conn_req.DEST_offset, sin, sizeof(*sin));
		err = TBADADDR;
		if (unlikely(sin->sin_family != AF_INET))
			goto error;
		if (unlikely(sin->sin_port == 0))
			goto error;
		if (unlikely(sin->sin_addr.s_addr == 0))
			goto error;
	}
	if (likely(p->conn_req.OPT_length != 0)) {
		struct udp_options opts = udp->options;
		unsigned char *ip = mp->b_wptr + p->conn_res.OPT_offset;
		size_t ilen = p->conn_res.OPT_length;

		if ((err = t_opts_parse(ip, ilen, &opts)))
			goto error;
		udp->options = opts;
	}
	err = -ENOBUFS;
	if (unlikely((rp = ss7_allocb(q, sizeof(p->ok_ack), BPRI_MED)) == NULL))
		goto error;
	udp->info.SERV_type = T_COTS;
	if (udp->info.CURRENT_state == TS_UNBND) {
		struct sockaddr_in *sin = (struct sockaddr_in *) &udp->bind;

		/* need to autobind the Stream */
		sin->sin_family = 0;
		sin->sin_port = 0;
		sin->sin_addr.s_addr = 0;
		if ((err = udp_bind(udp, &udp->bind, sizeof(udp->bind))))
			goto error;
		tpi_set_state(udp, TS_IDLE);
	}
	tpi_set_state(udp, TS_WACK_CREQ);
	if ((err = t_ok_ack(q, T_CONN_REQ, rp, NULL, dp, NULL)) != QR_ABSORBED)
		goto error;
	p->PRIM_type = T_CONN_CON;
	/* all of the other fields and contents are the same */
	tpi_set_state(udp, TS_DATA_XFER);
	qreply(q, mp);
	return (QR_ABSORBED);
      error:
	if (rp != NULL)
		freeb(rp);
	return t_error_ack(q, T_CONN_REQ, mp, err);
}

STATIC INLINE fastcall mblk_t *
t_seq_check(struct udp *udp, t_uscalar_t seq)
{
	mblk_t *mp;

	spin_lock_bh(&udp->conq.q_lock);
	for (mp = bufq_head(&udp->conq); mp && (t_uscalar_t) (long) mp != seq; mp = mp->b_next) ;
	spin_unlock_bh(&udp->conq.q_lock);
	usual(mp);
	return (mp);
}
STATIC INLINE fastcall struct udp *
t_tok_check(t_uscalar_t tok)
{
	struct udp *ap;

	spin_lock_bh(&udp_lock);
	for (ap = udp_opens; ap && (t_uscalar_t) (long) ap->oq != tok; ap = ap->next) ;
	spin_unlock_bh(&udp_lock);
	usual(ap);
	return (ap);
}

/**
 * t_conn_res = process a T_CONN_RES primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC int
t_conn_res(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q), *ap;
	struct T_conn_res *p;
	mblk_t *dp, *cp;
	size_t mlen;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_CONN_RES))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type == T_CLTS))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tpi_get_state(udp) != TS_WRES_CIND))
		goto error;
	err = TBADOPT;
	if (unlikely(p->OPT_length > udp->info.OPT_size))
		goto error;
	err = TBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	err = TBADDATA;
	if ((dp = mp->b_cont))
		if (unlikely((mlen = msgsize(dp)) > udp->info.CDATA_size))
			goto error;
	err = TBADSEQ;
	if (unlikely((cp = t_seq_check(udp, p->SEQ_number)) == NULL))
		goto error;
	if (p->ACCEPTOR_id == 0) {
		ap = udp;
		err = TBADF;
		if (bufq_length(&udp->conq) > 1)
			goto error;
	} else {
		struct sockaddr_in *sin, *ain;

		err = TBADF;
		if (unlikely((ap = t_tok_check(udp, p->ACCEPTOR_id)) == NULL))
			goto error;
		err = TPROVMISMATCH;
		if (unlikely(ap->info.SERV_type == T_CLTS))
			goto error;
		err = TRESQLEN;
		if (unlikely(ap->CONIND_number > 0))
			goto error;
		err = TOUTSTATE;
		if (tpi_get_statef(ap) != TS_IDLE)
			goto error;
		sin = (struct sockaddr_in *) &udp->srce;
		ain = (struct sockaddr_in *) &ap->srce;
		err = TRESADDR;
		if (sin->sin_port != ain->sin_port || sin->sin_addr.s_addr != ain->sin_addr.s_addr)
			goto error;
	}
	if (likely(p->OPT_length != 0)) {
		struct upd_options opts = ap->options;

		if ((err = t_opts_parse(mp->b_rptr + p->OPT_offset, p->OPT_length, &opts)))
			goto error;
		/* TPI options processing rules allows us to set some (or all) of the options on
		   the Stream even if the primitive is about to fail. */
		ap->options = opts;
	}
	{
		struct T_conn_ind *c = (typeof(c)) cp->b_rptr;

		bcopy(cp->b_rptr + c->SRC_offset, &ap->dest, c->SRC_length);
	}
	tpi_set_state(udp, TS_WACK_CRES);
	return t_ok_ack(q, T_CONN_RES, mp, cp, dp, ap);
      error:
	return t_error_ack(q, T_CONN_RES, mp, err);
}

/**
 * t_discon_req - process a T_DISCON_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC int
t_discon_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	union T_primitives *p;
	t_uscalar_t state;
	mblk_t *dp, *cp = NULL;
	size_t mlen;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->discon_req)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->discon_req.PRIM_type != T_DISCON_REQ))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type == T_CLTS))
		goto error;
	state = tpi_get_state(udp);
	err = TOUTSTATE;
	if (unlikely((1 << state) & ~(TSM_CONNECTED)))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) != NULL))
		if (unlikely((mlen = msgsize(dp)) == 0 || mlen > udp->info.DDATA_size))
			goto error;
	switch (state) {
	case TS_WCON_CREQ:
		tpi_set_state(udp, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		err = TBADSEQ;
		if (unlikely((cp = t_seq_check(udp, p->discon_req.SEQ_number)) == NULL))
			goto error;
		tpi_set_state(udp, TS_WACK_DREQ7);
		break;
	case TS_DATA_XFER:
		tpi_set_state(udp, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		tpi_set_state(udp, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		tpi_set_state(udp, TS_WACK_DREQ11);
		break;
	}
	return t_ok_ack(q, T_DISCON_REQ, mp, cp, dp, NULL);
      error:
	return t_error_ack(q, T_DISCON_REQ, mp, err);
}

/**
 * t_unitdata_req - process a T_UNITDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC INLINE fastcall __hot_put int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_unitdata_req *p;
	struct sockaddr_in sin;
	struct udp_options opts;
	size_t mlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_UNITDATA_REQ))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type != T_CLTS))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tpi_get_state(udp) != TS_IDLE))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((mlen = msgsize(dp)) == 0 || mlen > udp->info.TSDU_size))
		goto error;
	err = TNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = TBADADDR;
	if (unlikely((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)))
		goto error;
	if (unlikely(p->DEST_length < sizeof(struct sockaddr_in)))
		goto error;
	bcopy(mp->b_rptr + p->DEST_length, &sin, sizeof(sin));
	err = TBADOPT;
	if (unlikely(p->OPT_length > udp->info.OPT_size))
		goto error;
	err = TBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	opts = udp->options;
	if (unlikely(p->OPT_length != 0))
		if ((err = t_opts_parse(mp->b_wptr + p->OPT_offset, p->OPT_length, &opts)))
			goto error;
	if ((err = udp_xmitmsg(q, dp, &sin, &opts)))
		goto err;
	return (QR_DONE);
      error:
	/* FIXME: we can send uderr for some of these instead of erroring out the entire stream. */
	return m_error(q, err, mp);
}

/**
 * t_optdata_req - process a T_OPTDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC INLINE fastcall __hot_put int
t_optdata_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_optdata_req *p;
	struct sockaddr_in *sin;
	struct udp_options opts;
	size_t mlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_OPTDATA_REQ))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type == T_CLTS))
		goto error;
	if (unlikely(tpi_get_state(udp) == TS_IDLE))
		goto discard;
	err = TOUTSTATE;
	if (unlikely(tpi_get_statef(udp) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	err = TBADFLAG;
	if (unlikely(p->DATA_flag != 0))
		goto error;
	err = TBADOPT;
	if (unlikely(p->OPT_length > udp->info.OPT_size))
		goto error;
	err = TBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((mlen = msgsize(dp)) == 0 || mlen > udp->info.TSDU_size))
		goto error;
	opts = udp->options;
	if (unlikely(p->OPT_length != 0))
		if ((err = t_opts_parse(mp->b_wptr + p->OPT_offset, p->OPT_length, &opts)))
			goto error;
	sin = (typeof(sin)) & udp->dest;
	if ((err = udp_xmitmsg(q, dp, sin, &opts)))
		goto error;
	return (QR_DONE);
      error:
	return m_error(q, err, mp);
}

/**
 * t_data_req - process a T_DATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC INLINE fastcall __hot_put int
t_data_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_data_req *p;
	struct sockaddr_in *sin;
	struct udp_options *opts;
	size_t mlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_DATA_REQ))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type == T_CLTS))
		goto error;
	if (unlikely(udp->info.TSDU_size == T_INVALID))
		goto error;
	err = QR_DONE;
	if (unlikely(tpi_get_state(udp) == TS_IDLE))
		goto error;
	err = TOUTSTATE;
	if (unlikely(tpi_get_statef(udp) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	err = TBADFLAG;
	if (unlikely(p->MORE_flag != 0))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((mlen = msgsize(dp)) == 0 || mlen > udp->info.TSDU_size))
		goto error;
	opts = &udp->options;
	sin = (typeof(sin)) & udp->dest;
	if ((err = udp_xmitmsg(q, dp, sin, opts)))
		goto error;
	return (QR_DONE);
      error:
	return m_error(q, err, mp);
}

/**
 * t_exdata_req - process a T_EXDATA_REQ primitive
 * @q: write queue
 * @mp: the primitive
 */
STATIC int
t_exdata_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_exdata_req *p;
	struct sockaddr_in *sin;
	struct udp_options *opts;
	size_t mlen;
	mblk_t *dp;
	int statef;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_EXDATA_REQ))
		goto error;
	err = TNOTSUPPORT;
	if (unlikely(udp->info.SERV_type == T_CLTS))
		goto error;
	if (unlikely(udp->info.ETSDU_size == T_INVALID))
		goto error;
	err = QR_DONE;
	if (unlikely((statef = tpi_get_statef(udp)) & TSF_IDLE))
		goto error;
	err = TOUTSTATE;
	if (unlikely(statef & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	err = TBADFLAG;
	if (unlikely(p->MORE_flag != 0))
		goto error;
	err = TBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((mlen = msgsize(dp)) == 0 || mlen > udp->info.ETSDU_size))
		goto error;
	opts = &udp->options;
	sin = (typeof(sin)) & udp->dest;
	if ((err = udp_xmitmsg(q, dp, sin, opts)))
		goto error;
	return (QR_DONE);
      error:
	return m_error(q, err, mp);

}

/**
 * t_optmgmt_req - handle T_OPTMGMT_REQ primitive
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
STATIC int
t_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	int err, opt_len;
	const struct T_optmgmt_req *p;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_OPTMGMT_REQ))
		goto error;
#ifdef TS_WACK_OPTREQ
	if (tpi_get_state(udp) == TS_IDLE)
		tpi_set_state(udp, TS_WACK_OPTREQ);
#endif
	err = TBADOPT;
	if (unlikely(p->OPT_length > udp->info.OPT_size))
		goto error;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto error;
	err = TBADFLAG;
	switch (p->MGMT_flags) {
	case T_DEFAULT:
		opt_len = t_size_default_options(udp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CURRENT:
		opt_len = t_size_current_options(udp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CHECK:
		opt_len = t_size_check_options(udp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_NEGOTIATE:
		opt_len = t_size_negotiate_options(udp, mp->b_rptr + p->OPT_offset, p->OPT_length);
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
			goto badopt;
		case EACCES:
			goto acces;
		case ENOBUFS:
		case ENOMEM:
			break;
		default:
			goto error;
		}
	}
	return (err);
      error:
	return t_error_ack(q, T_OPTMGMT_REQ, mp, err);
}

#ifdef T_ADDR_REQ
/**
 * t_addr_req - process a T_ADDR_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_addr_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	struct T_addr_req *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_ADDR_REQ))
		goto error;
	{
		struct sockaddr *LOCADDR_buffer, *REMADDR_buffer;
		socklen_t LOCADDR_length, REMADDR_length;

		err = TOUTSTATE;
		switch (tpi_get_state(udp)) {
		case TS_UNBND:
			LOCADDR_buffer = NULL;
			LOCADDR_length = 0;
			REMADDR_buffer = NULL;
			REMADDR_length = 0;
			break;
		case TS_IDLE:
		case TS_WRES_CIND:
			LOCADDR_buffer = (struct sockaddr *) &udp->srce;
			LOCADDR_length = sizeof(struct sockaddr_in);
			REMADDR_buffer = NULL;
			REMADDR_length = 0;
			break;
		case TS_WCON_CREQ:
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
			LOCADDR_buffer = (struct sockaddr *) &udp->srce;
			LOCADDR_length = sizeof(struct sockaddr_in);
			REMADDR_buffer = (struct sockaddr *) &udp->dest;
			REMADDR_length = sizeof(struct sockaddr_in);
			break;
		default:
			goto error;
		}
		return t_addr_ack(q, LOCADDR_buffer, LOCADDR_length, REMADDR_buffer,
				  REMADDR_length);
	}
      error:
	return t_error_ack(q, T_ADDR_REQ, mp, err);
}
#endif				/* T_ADDR_REQ */

#ifdef T_CAPABILITY_REQ
/**
 * t_capability_req - process a T_CAPABILITY_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_capability_req(queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	err = -EFAULT;
	if (unlikely(p->PRIM_type != T_CAPABILITY_REQ))
		goto error;
	return t_capability_ack(q, p->CAP_bits1, mp->b_datap->db_type);
      error:
	return t_error_ack(q, T_CAPABILITY_REQ, mp, err);
}
#endif				/* T_CAPABILITY_REQ */

/**
 * t_other_req - process unsupported recognized primitive type
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_other_req(queue_t *q, mblk_t *mp)
{
	t_scalar_t prim;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(prim)))
		goto error;
	prim = *((t_scalar_t *) mp->b_rptr);
	err = TNOTSUPPORT;
      error:
	return t_error_ack(q, prim, mp, err);
}

/*
 *  STREAMS MESSAGE HANDLING
 */

/**
 * upd_w_proto - M_PROTO, M_PCPROTO handling
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC INLINE fastcall __hot_put int
udp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	t_scalar_t prim;
	struct udp *udp = UDP_PRIV(q);

	udp->i_oldstate = tpi_get_state(udp);	/* checkpoint */

	if (mp->b_wptr < mp->b_rptr + sizeof(t_scalar_t))
		goto eproto;
	switch ((prim = *((t_scalar_t *) mp->b_rptr))) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, udp));
		rtn = t_conn_req(q, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, udp));
		rtn = t_conn_res(q, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, udp));
		rtn = t_discon_req(q, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, udp));
		rtn = t_data_req(q, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, udp));
		rtn = t_exdata_req(q, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, udp));
		rtn = t_info_req(q, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, udp));
		rtn = t_bind_req(q, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, udp));
		rtn = t_unbind_req(q, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, udp));
		rtn = t_optmgmt_req(q, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, udp));
		rtn = t_unitdata_req(q, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, udp));
		rtn = t_other_req(q, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, udp));
		rtn = t_optdata_req(q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, udp));
		rtn = t_addr_req(q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, udp));
		rtn = t_capability_req(q, mp);
		break;
#endif
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
#ifdef T_CAPABILITY_ACK
	case T_CAPABILITY_ACK:
#endif
	      eproto:
		rtn = m_error(q, EPROTO, mp);
		break;
	default:
		rtn = t_other_req(q, mp);
		break;
	}
	if (rtn < 0) {
#ifndef _TEST
		/* not so rare() during conformance suite testing */
		rare();
#endif
		tpi_set_state(udp, udp->i_oldstate);
		/* The put and srv procedures do not recognize all errors.  Sometimes we return an
		   error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
			break;
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/**
 * udp_w_ioctl - M_IOCTL handling
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 *
 * This TPI-UDP provider does not support any input-output controls and, therefore, all input-output
 * controls are negatively acknowledged.
 */
STATIC int
udp_w_ioctl(queue_t *q, mblk_t *mp)
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
 * udp_r_data - M_DATA handling
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 *
 * M_DATA messages are placed on the read queue from below by udp_v4_rcv().  These messages are
 * transformed into T_UNITDATA_IND, T_CONN_IND or T_OPTDATA_IND M_PROTO messages and passed along
 * upstream.
 */
STATIC INLINE fastcall __hot_get int
udp_r_data(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);

	if (udp->info.SERV_type == T_CLTS)
		return t_unitdata_ind(q, mp);
	if (udp->CONIND_number > 0)
		return t_conn_ind(q, mp);
	return t_optdata_ind(q, mp);
}

/**
 * udp_r_error - M_ERROR handling
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 *
 * M_ERROR messages are placed on the read queue from below by udp_v4_err().  These messages are
 * transformed into T_UDERROR_IND M_PROTO messages and passed along upstream.
 */
STATIC int
udp_r_error(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);

	if (udp->info.SERV_type == T_CLTS)
		return t_uderror_ind(q, mp);
	return t_discon_ind(q, mp);
}

/**
 * udp_w_prim - process primitive on write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC streamscall __hot_put int
udp_w_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return udp_w_proto(q, mp);
	case M_IOCTL:
		return udp_w_ioctl(q, mp);
	}
	return (-EOPNOTSUPP);
}

/**
 * udp_r_prim - process primitive on read queue
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC streamscall __hot_get int
udp_r_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_DATA:
		return udp_r_data(q, mp);
	case M_ERROR:
		return udp_r_error(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  IP Management
 *
 *  =========================================================================
 */

#ifdef LINUX
/**
 * udp_free - message block free function for mblks esballoc'ed from sk_buffs
 * @data: client data (sk_buff pointer in this case)
 */
STATIC void streamscall
udp_free(char *data)
{
	struct sk_buff *skb = (struct sk_buff *) data;

	kfree_skb(skb);
	return;
}

/**
 * udp_v4_rcv - receive IPv4 UDP protocol packets
 */
STATIC __hot_in int
udp_v4_rcv(struct sk_buff *skb)
{
	ushort ulen;
	struct udp *udp;
	struct iphdr *iph;
	struct rtable *rt;
	struct udphdr *uh, *uh2;
	frtn_t fr = { &udp_free, (char *) skb };
	int rtn;

//      IP_INC_STATS_BH(IpInDelivers);  /* should wait... */

	if (!pskb_may_pull(skb, sizeof(struct udphdr)))
		goto too_small;

	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	rt = (struct rtable *) skb->dst;
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;

//      UDP_INC_STATS_BH(UdpInDatagrams);
	/* pull up the udp header */
	uh = skb->h.uh;
	ulen = ntohs(uh->len);
	/* sanity check UDP length */
	if (ulen > skb->len || ulen < sizeof(struct udphdr))
		goto too_small;
	if (pskb_trim(skb, ulen))
		goto too_small;
	/* we do the lookup before the checksum */
	iph = skb->nh.iph;
	read_lock(&udp_lock);
	if (!(udp = udp_lookup(uh->dest, uh->source, iph->daddr, iph->saddr)))
		goto no_stream;
	udp_v4_steal(skb);	/* its ours */
	/* checksum initialization */
	if (uh->check == 0)
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else if (skb->ip_summed == CHECKSUM_HW)
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else if (skb->ip_summed != CHECKSUM_UNNECESSARY)
		skb->csum = csum_tcpudp_nofold(iph->saddr, iph->daddr, ulen, IPPROTO_UDP, 0);
	/* For now... We should actually place non-linear fragments into separate mblks and pass
	   them up as a chain, or deal with non-linear sk_buffs directly.  As it winds up, the
	   netfilter hooks linearize anyway. */
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
	{
		mblk_t *mp;
		size_t mlen = skb->len + (skb->data - skb->nh.raw);

		/* now allocate an mblk */
		if (!(mp = esballoc(skb->nh.raw, mlen, BPRI_MED, &fr)))
			goto no_buffers;
		mp->b_wptr = mp->b_rptr + mlen;
		/* trim the ip header */
		mp->b_rptr = skb->h.raw;
		uh2 = (typeof(uh2)) mp->b_rptr;
		if (uh->check != uh2->check)
			goto sanity;
		mp->b_rptr += sizeof(struct udphdr);
		skb->dev = NULL;

		if (!udp->oq || !canput(udp->oq))
			goto flow_controlled;
		put(udp->oq, mp);
//              UDP_INC_STATS_BH(UdpInDatagrams);
		udp_put(udp);
		read_unlock(&udp_lock);
		return (0);
	      sanity:
	      flow_controlled:
		udp_put(udp);
		read_unlock(&udp_lock);
		freeb(mp);	/* will take sk_buff with it */
		return (0);
	      no_buffers:
		udp_put(udp);
		read_unlock(&udp_lock);
		goto discard_it;
	}
      no_stream:
	read_unlock(&udp_lock);
	ptrace(("ERROR: No stream\n"));
	/* Note, in case there is nobody to pass it too, we have to complete the checksum check
	   before dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
//      UDP_INC_STATS_BH(UdpNoPorts);   /* should wait... */
	goto pass_it;
      bad_checksum:
//      UDP_INC_STATS_BH(UdpInErrors);
//      IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ??? */
	udp_put(udp);
	goto discard_it;
      linear_fail:
	udp_put(udp);
	read_unlock(&udp_lock);
	goto discard_it;
      discard_it:
	kfree_skb(skb);
	return (0);
      bad_pkt_type:
	goto pass_it;
      too_small:
	goto pass_it;
      pass_it:
	return udp_v4_rcv_next(skb);
}

/**
 * udp_v4_err - receive IPv4 UDP protocol ICMP packets
 * @skb: socket buffer containing message with headers pulled
 * @u32: icmp error (unused)
 *
 * This function is a network protocol callback that is invoked when transport specific ICMP errors
 * are received.  The function looks up the Stream and, if found, wraps the packet in an M_ERROR
 * message and passes it to the read queue of the Stream.
 *
 * LOCKING: udp_lock protects the master list and protects from open, close, link and unlink.
 * udp->qlock protects the state of private structure.  udp->refs protects the private structure
 * from being deallocated before locking.
 */
STATIC __hot_in void
udp_v4_err(struct sk_buff *skb, u32 info)
{
	struct udp *udp;
	struct udphdr *uh;
	struct iphdr *iph;
	size_t ihl;

	iph = (struct iphdr *) skb->data;
#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
	uh = (struct udphdr *) (skb->data + ihl);
	read_lock(&udp_lock);
	/* reverse port numbers and addresses in lookup */
	udp = udp_lookup(uh->source, uh->dest, iph->saddr, iph->daddr);
	if (udp == NULL)
		goto no_stream;
	spin_lock(&udp->qlock);
	if (udp->i_state == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		size_t mlen = sizeof(struct icmphdr) + skb->len;

		/* Create a queue a specialized M_ERROR message to the Stream's read queue for
		   further processing.  The Stream will convert this message into a T_UDERR_IND
		   or T_DISCON_IND message and pass it along. */
		if (!udp->oq || !canput(udp->oq))
			goto flow_controlled;
		if (!(mp = allocb(mlen, BPRI_MED)))
			goto no_buffers;
		mp->b_datap->db_type = M_ERROR;
		bcopy(skb->h.icmph, mp->b_wptr, sizeof(struct icmphdr));
		mp->b_wptr += sizeof(struct icmphdr);
		bcopy(skb->data, mp->b_wptr, skb->len);
		mp->b_rptr = mp->b_wptr + ihl;
		mp->b_wptr += skb->len;
		put(udp->oq, mp);
		goto unlock_discard_put;
	}
	goto discard_and_put;
      unlock_discard_put:
	spin_unlock(&udp->qlock);
	goto discard_and_put;
      discard_and_put:
	udp_put(udp);
	read_unlock(&udp_lock);
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto unlock_discard_put;
      flow_controlled:
	ptrace(("ERROR: stream is flow controlled\n"));
	goto unlock_discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto unlock_discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	read_unlock(&udp_lock);
	udp_next_err_handler(&skb, info);
	if (skb == NULL)
		return;
	goto drop;
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

#endif				/* LINUX */

/*
 *  Private structure allocation and deallocation.
 */
STATIC struct udp *
udp_alloc_priv(queue_t *q, struct udp **udpp, dev_t *devp, cred_t *crp)
{
	struct udp *udp;

	if ((udp = udp_alloc())) {
		major_t cmajor = getmajor(*devp);
		minor_t cminor = getminor(*devp);

		/* udp generic members */
		udp->u.dev.cmajor = cmajor;
		udp->u.dev.cminor = cminor;
		udp->cred = *crp;
		(udp->oq = RD(q))->q_ptr = udp_get(udp);
		(udp->iq = WR(q))->q_ptr = udp_get(udp);
		udp->i_prim = &udp_w_prim;
		udp->o_prim = &udp_r_prim;
		udp->i_wakeup = NULL;
		udp->o_wakeup = NULL;
		udp->type = IPPROTO_UDP;
		spinlock_init(&udp->qlock);
		udp->i_version = T_CURRENT_VERSION;
		udp->i_style = 2;
		udp->i_state = udp->i_oldstate = udp->info.CURRENT_state = TS_UNBND;
		/* initialized information */
		udp->info.PRIM_type = T_INFO_ACK;
		udp->info.TSDU_size = 65535;
		udp->info.ETSDU_size = T_INVALID;
		udp->info.CDATA_size = 65535;
		udp->info.DDATA_size = 65535;
		udp->info.ADDR_size = sizeof(struct sockaddr_in);
		udp->info.OPT_size = 65535;
		udp->info.TIDU_size = 4096;
		udp->info.SERV_type = T_CLTS;	/* by default */
		udp->info.CURRENT_state = TS_UNBND;
		udp->info.PROVIDER_flag = T_XPG4_1 & ~T_SNDZERO;
		bufq_init(&udp->conq);
		/* link into master list */
		if ((udp->next = *udpp))
			udp->next->prev = &udp->next;
		udp->prev = udpp;
		*udpp = udp_get(udp);
	} else
		strlog(DRV_ID, getminor(*devp), LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not allocate driver private structure");
	return (udp);
}
STATIC void
udp_free_priv(queue_t *q)
{
	struct udp *udp = UDP_PRIV(q);

	strlog(DRV_ID, udp->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&udp->refcnt));
	/* make sure the stream is disconnected */
	if (udp->hashb != NULL) {
		udp_disconnect(udp);
		bufq_purge(&udp->conq);
		tpi_set_state(udp, TS_IDLE);
	}
	/* make sure the stream is unbound */
	if (udp->bindb != NULL) {
		udp_unbind(udp);
		tpi_set_state(udp, TS_UNBND);
	}
	bufq_purge(&udp->conq);
	ss7_unbufcall((str_t *) udp);
	strlog(DRV_ID, udp->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "removed bufcalls: reference count = %d", atomic_read(&udp->refcnt));
	/* remove from master list */
	write_lock_bh(&udp_lock);
	if ((*udp->prev = udp->next))
		udp->next->prev = udp->prev;
	udp->next = NULL;
	udp->prev = &udp->next;
	write_unlock_bh(&udp_lock);
	strlog(DRV_ID, udp->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinked: reference count = %d", atomic_read(&udp->refcnt));
	udp_release(&udp->oq->q_ptr);
	udp->oq = NULL;
	udp_release(&udp->iq->q_ptr);
	udp->iq = NULL;
	assure(atomic_read(&udp->refcnt) == 1);
	udp_release(&udp);
	return;
}

/*
 *  Open and Close
 */
STATIC int udp_majors[UDP_CMAJORS] = { UDP_CMAJOR_0, };

/**
 * udp_qopen - open a Stream
 * @q: read queue in queue pair
 * @devp: pointer to the device number opened
 * @flag: open flags
 * @sflag: STREAMS flags (DRVOPEN, MODOPEN, CLONEOPEN)
 * @crp: credentials pointer
 */
STATIC streamscall int
udp_qopen(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	struct udp *udp, **udpp = &udp_opens;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	{
		/* Linux Fast-STREAMS always passes internal major device number (module id).  Note 
		   also, however, that strconf-sh attempts to allocate module ids that are
		   identical to the base major device number anyway. */
#ifdef LIS
		if (cmajor != UDP_CMAJOR_0)
			return (ENXIO);
#endif
#ifdef LFS
		if (cmajor != UDP_DRV_ID)
			return (ENXIO);
#endif
	}
	/* sorry, you can't open by minor device */
	if (cminor < 0 || cminor > 0)
		return (ENXIO);
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = 1;
	write_lock_bh(&udp_lock);
	for (; *udpp; udpp = &(*udpp)->next) {
		if (cmajor != (*udpp)->u.dev.cmajor)
			break;
		if (cmajor == (*udpp)->u.dev.cmajor) {
			if (cminor < (*udpp)->u.dev.cminor)
				break;
			if (cminor == (*udpp)->u.dev.cminor) {
				if (++cminor >= UDP_UNITS) {
					if (+mindex >= UDP_CMAJORS
					    || !(cmajor = udp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= UDP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_bh(&udp_lock);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, camjor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(udp = udp_alloc_priv(q, udpp, cmajor, cminor, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_bh(&udp_lock);
		return (ENOMEM);
	}
	write_unlock_bh(&udp_lock);
	qprocson(q);
	return (0);
}

/**
 * udp_qclose - close a Stream
 * @q: read queue in queue pair
 * @flag: open flags
 * @crp: credentials pointer
 */
STATIC streamscall int
udp_qclose(queue_t *q, int flag, cred_t *crp)
{
	printd(("%s: closing character device %d:%d\n", DRV_NAME, (int) UDP_PRIV(q)->u.dev.cmajor,
		(int) UDP_PRIV(q)->u.dev.cminor));
#ifdef LIS
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
#endif				/* LIS */
	goto skip_pop;
      skip_pop:
	/* make sure procedures are off */
	qprocsoff(q);
	udp_free_priv(q);	/* free and unlink the structure */
	goto quit;
      quit:
	return (0);
}

/*
 *  Private structure reference counting, allocation, deallocation and cache
 */
STATIC void
udp_term_caches(void)
{
	if (udp_priv_cachep && kmem_cache_destroy(udp_priv_cachep))
		cmn_err(CE_WARN, "%s: did not destroy udp_priv_cachep", __FUNCTION__);
	if (udp_bind_cachep && kmem_cache_destroy(udp_bind_cachep))
		cmn_err(CE_WARN, "%s: did not destroy udp_bind_cachep", __FUNCTION__);
	return;
}
STATIC void
udp_init_caches(void)
{
	if (!udp_priv_cachep &&
	    !(udp_priv_cachep =
	      kmem_cache_create("udp_priv_cachep", sizeof(struct udp),
				0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
		cmn_err(CE_PANIC, "%s: Cannot allocate udp_priv_cachep", __FUNCTION__);
	if (!udp_bind_cachep &&
	    !(udp_bind_cachep =
	      kmem_cache_create("udp_bind_cachep", sizeof(struct udp_bind_bucket),
				0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
		cmn_err(CE_PANIC, "%s: Cannot allocate udp_bind_cachep", __FUNCTION__);
	return;
}

STATIC void
udp_term_hashes(void)
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
STATIC void
udp_init_hashes(void)
{
	int order, i;
	unsigned long goal;

	/* size and allocate bind hash table */
	goal = num_physpages >> (20 - PAGE_SHIFT);
	for (order = 0; (1 << order) < goal; order++) ;
	do {
		udp_bhash_order = order;
		udp_bhash_size = (1 << order) * PAGE_SIZE / sizeof(struct udp_bhash_bucket);
		udp_bhash = (struct udp_bhash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (udp_bhash == NULL && --order >= 0);
	if (!udp_bhash)
		cmn_err(CE_PANIC, "%s: Failed to allocate UDP bind hash table\n", __FUNCTION__);
	udp_bhash_size = udp_chash_size = udp_bhash_size >> 1;
	udp_bhash_order = udp_chash_order = udp_bhash_order - 1;
	bzero(udp_bhash, udp_bhash_size * sizeof(struct udp_bhash_bucket));
	bzero(udp_chash, udp_chash_size * sizeof(struct udp_chash_bucket));
	for (i = 0; i < udp_bhash_size; i++)
		rwlock_init(&udp_bhash[i].lock);
	for (i = 0; i < udp_chash_size; i++)
		rwlock_init(&udp_chash[i].lock);
	printd(("%s: INFO: bind hash table configured size = %d\n", DRV_NAME, udp_bhash_size));
	printd(("%s: INFO: conn hash table configured size = %d\n", DRV_NAME, udp_chash_size));
}

/*
 *  IP subsystem management
 */
#ifdef LINUX
STATIC struct net_protocol **inet_protosp = (typeof(inet_protosp)) HAVE_INET_PROTOS_ADDR;

#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
struct inet_protocol udp_inet_protocol = {
	.protocol = IPPROTO_UDP,
	.name = "streams-udp",
	.handler = &udp_v4_rcv,
	.err_handler = &udp_v4_err,
	.copy = 0,
	.next = NULL,
};

struct inet_protocol *udp_proto = NULL;

/**
 * udp_v4_steal = steal a socket buffer
 * @skb: socket buffer to steal
 *
 * In the pakcet handler, if the packet is for us, steal the packet by overwritting the protocol and
 * returning.  This is only done for normal packets and not error packets (that do not need to be
 * stolen).  In the 2.4 handler loop, iph->protocol is examined on each iteration, permitting us to
 * stead the packet by overwritting the protocol number.
 */
STATIC INLINE fastcall __hot_in void
udp_v4_steal(struct sk_buff *skb)
{
	skb->nh.iph->protocol = 255;
}

/**
 * udp_v4_rcv_next - pass a socket buffer to the next handler
 * @skb: socket buffer to pass
 *
 * In the packet handler, if the packet is for us, pass it to the next handler by simply freeing the
 * cloned copy and returning.
 */
STATIC INLINE fastcall __hot_in int
udp_v4_rcv_next(struct sk_buff *skb)
{
	kfree_skb(skb);
	return (0);
}

/**
 * udp_v4_err_next - pass a socket buffer to the next error handler
 * @skb: socket buffer to pass
 *
 * In the error packet handler, if the packet is not for us, pass it to the next handler by simply
 * returning.  Error packets are not cloned, so don't free it.
 */
STATIC INLINE fastcall __hot_in int
udp_v4_err_next(struct sk_buff *skb, __u32 info)
{
	return (0);
}

/**
 * udp_init_nproto - initialize UDP network protocol override
 *
 * This is largely a 2.4 version of the network protocol override function.  Under 2.4, simply add
 * the protocol to the network using an inet_protocol structure and the inet_add_protocol()
 * function.
 */
STATIC int
udp_init_nproto(void)
{
	struct inet_protocol *ip;

	if ((ip = udp_proto) != NULL)
		return (-EALREADY);
	ip = udp_proto = &udp_inet_protocol;
	inet_add_protocol(ip, IPPROTO_UDP);
	return (0);
}

/**
 * udp_term_nproto - terminate UDP network protocol override
 *
 * This is largely a 2.4 version of the network protocol override function.  Under 2.4 simply remove
 * the protocol from the network using the inet_protocol structure and the inet_del_protocol()
 * function.
 */
STATIC int
udp_term_nproto(void)
{
	struct inet_protocol *ip;

	if ((ip = udp_proto) == NULL)
		return (-EALREADY);	/* already terminated */
	inet_del_protocol(ip, IPPROTO_UDP);
	udp_proto = NULL;
	return (0);
}

#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL

/*
 *  Under 2.6, attempt to do the equivalent of inet_add_protocol().  If it fails (as would be
 *  expected as UDP is permanent protocol), check whether a module owns the net_protocol structure
 *  and, if so, try to increment the module count (to keep it from unloading).  Take a reference to
 *  the old net_protocol structure.  Then replace the net_protocol pointer with our own in the
 *  hashes.  If it succeeds (which we would not expect) it reduces to the same as
 *  inet_add_protocol().  In the packet handler, if the packet is not for us, simply pass it to the
 *  next handler.  If the packet is for us, clone it, free the original and work with the clone.
 */

struct inet_protocol = {
	struct net_protocol proto;
	struct net_protocol *next;
	struct module *kmod;
};

struct inet_protocol udp_inet_protocol = {
	.proto = {
#ifdef HAVE_KMEMB_STRUCT_NET_PROTOCOL_PROTO
		  .proto = IPPROTO_UDP,
#endif				/* HAVE_KMEMB_STRUCT_NET_PROTOCOL_PROTO */
		  .handler = &udp_v4_rcv,
		  .err_handler = &udp_v4_err,
		  .no_policy = 1,
		  },
	.next = NULL,
	.kmod = NULL,
};

struct inet_protocol *udp_proto = NULL;

/**
 * udp_v4_steal - steal a socket buffer
 * @skb: socket buffer to steal
 *
 * In the packet handler, if the packet is for us, steal the packet by simply not passing it to the
 * next handler.
 */
STATIC INLINE fastcall __hot_in void
udp_v4_steal(struct sk_buff *skb)
{
}

/**
 * upd_v4_rcv_next - pass a socket buffer to the next handler
 * @skb - socket buffer to pass
 *
 * If the packet is not for us, pass it to the next handler.  If there is no next handler, free the
 * packet and return.  Note that we do not have to lock the hash because we own it and are also
 * holding a reference to any module owning the next handler.
 */
STATIC INLINE fastcall __hot_in int
udp_v4_rcv_next(struct sk_buff *skb)
{
	struct inet_protocol *ip;

	if ((ip = udp_proto) != NULL && ip->next != NULL)
		return ip->next->proto.handler(skb);
	kfree_skb(skb);
	return (0);
}

/**
 * udp_v4_err_next - pass a socket buffer to the next error handler
 * @skb - socket buffer to pass
 *
 * Error packets are not cloned, so pass it to the next handler.  If there is not next handler,
 * simply return.
 */
STATIC INLINE fastcall __hot_in int
udp_v4_err_next(struct sk_buff *skb, __u32 info)
{
	struct inet_protocol *ip;

	if ((ip = udp_proto) != NULL && ip->next != NULL)
		ip->next->proto.err_handler(skb, info);
	return (0);
}
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;

/**
 * udp_init_nproto - initialize UDP network protocol override
 *
 * This is largely a 2.6 version of the network protocol override function.  The module stuff here
 * is just for ourselves (other kernel modules pulling the same trick) as Linux UDP is kernel
 * resident.
 */
STATIC int
udp_init_nproto(void)
{
	struct inet_protocol *ip;
	int hash = IPPROTO_UDP & (MAX_INET_PROTOS - 1);

	if ((ip = udp_proto) != NULL)
		return (-EALREADY);	/* already initialized */
	ip = udp_proto = &udp_inet_protocol;
	/* reduces to inet_add_protocol() if no protocol registered */
	spin_lock_bh(inet_proto_lockp);
	if ((ip->next = inet_protosp[hash]) != NULL) {
		if ((ip->kmod = module_text_address(ip->next))
		    && ip->kmod != THIS_MODULE) {
			if (!try_to_get_module(ip->kmod)) {
				spin_unlock_bh(inet_proto_lockp);
				return (-EAGAIN);
			}
		}
	}
	inet_protosp[hash] = &ip->proto;
	spin_unlock_bh(inet_proto_lockp);
	synchronize_net();
	return (0);
}

/**
 * udp_term_nproto - terminate UDP network protocol override
 *
 * This is largely a 2.6 version of the network protocol override function.  The module stuff here
 * is just for ourselves (other kernel modules pulling the same trick) as Linux UDP is kernel
 * resident.
 */
STATIC int
udp_term_nproto(void)
{
	struct inet_protocol *ip;
	int hash = IPPROTO_UDP & (MAX_INET_PROTOS - 1);

	if ((ip = udp_proto) == NULL)
		return (-EALREADY);	/* already terminated */
	/* reduces to inet_del_protocol() if no protocol was registered */
	spin_lock_bh(inet_proto_lockp);
	inet_protos[hash] = ip->next;
	spin_unlock_bh(inet_proto_lockp);
	synchronize_net();
	udp_proto = NULL;
	if (ip->next != NULL && ip->kmod != NULL && ip->kmod != THIS_MODULE)
		module_put(ip->kmod);
	return (0);
}

#else
#error HAVE_KTYPE_STRUCT_INET_PROTOCOL or HAVE_KTYPE_STRUCT_NET_PROTOCOL must be defined.
#endif
#endif				/* LINUX */

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
module_param(modid, short, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the UDP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the UDP driver. (0 for allocation.)");

#ifdef LFS
/*
 *  Linux Fast-STREAMS Registration
 */
static struct cdevsw udp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &udp_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};
STATIC int
udp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&udp_cdev, major)) < 0)
		return (err);
	return (0);
}
STATIC int
udp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&udp_cdev, major)) < 0)
		return (err);
	return (0);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  Linux STREAMS Registration
 */
STATIC int
udp_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &udp_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}
STATIC int
udp_unregister_strdev(major_t major)
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
		if (udp_majors[mindex]) {
			if ((err = udp_unregister_strdev(udp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					udp_majors[mindex]);
			if (mindex)
				udp_majors[mindex] = 0;
		}
	}
	udp_term_nproto();
	udp_term_caches();
	udp_term_hashes();
	return;
}
MODULE_STATIC int __init
udpinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	udp_init_hashes();
	udp_init_caches();
	if ((err = udp_init_nproto())) {
		udp_term_caches();
		udp_term_hashes();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = udp_register_strdev(udp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d",
					DRV_NAME, udp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				udpterminate();
				return (err);
			}
		}
		if (udp_majors[mindex] == 0)
			udp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(udp_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = udp_majors[0];
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
