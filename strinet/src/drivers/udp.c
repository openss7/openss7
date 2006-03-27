/*****************************************************************************

 @(#) $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/27 01:25:36 $

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

 Last Modified $Date: 2006/03/27 01:25:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: udp.c,v $
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

#ident "@(#) $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/27 01:25:36 $"

static char const ident[] = "$RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/27 01:25:36 $";

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
#include <net/tcp.h> /* for checksumming */

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
#define UDP_REVISION	"OpenSS7 $RCSfile: udp.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/27 01:25:36 $"
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

STATIC streamscall int udp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int udp_close(queue_t *, int, cred_t *);

STATIC streamscall int udp_rput(queue_t *, mblk_t *);
STATIC streamscall int udp_rsrv(queue_t *);

STATIC struct qinit udp_rinit = {
	.qi_putp = udp_rput,		/* Read put (msg from below) */
	.qi_srvp = udp_rsrv,		/* Read queue service */
	.qi_qopen = udp_open,		/* Each open */
	.qi_qclose = udp_close,		/* Last close */
	.qi_minfo = &udp_minfo,		/* Information */
};

STATIC streamscall int udp_wput(queue_t *, mblk_t *);
STATIC streamscall int udp_wsrv(queue_t *);

STATIC struct qinit udp_winit = {
	.qi_putp = udp_wput,		/* Write put (msg from above) */
	.qi_srvp = udp_wsrv,		/* Write queue service */
	.qi_minfo = &udp_minfo,		/* Information */
};

STATIC streamscall int mux_rput(queue_t *, mblk_t *);
STATIC streamscall int mux_rsrv(queue_t *);

STATIC struct qinit mux_rinit = {
	.qi_putp = mux_rput,		/* Read put (msg from below) */
	.qi_srvp = mux_rsrv,		/* Read queue service */
	.qi_minfo = &udp_minfo,		/* Information */
};

STATIC streamscall int mux_wput(queue_t *, mblk_t *);
STATIC streamscall int mux_wsrv(queue_t *);

STATIC struct qinit mux_winit = {
	.qi_putp = mux_wput,		/* Write put (msg from above) */
	.qi_srvp = mux_wsrv,		/* Write queue service */
	.qi_minfo = &udp_minfo,		/* Information */
};

MODULE_STATIC struct streamtab udp_info = {
	.st_rdinit = &udp_rinit,	/* Upper read queue */
	.st_wrinit = &udp_winit,	/* Upper write queue */
	.st_muxrinit = &mux_rinit,	/* Lower read queue */
	.st_muxwinit = &mux_winit,	/* Lower write queue */
};

/*
 *  Queue put and service return values.
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
 *  NPI Interface state flags
 */
#define NSF_UNBND	( 1 << NS_UNBND		)
#define NSF_WACK_BREQ	( 1 << NS_WACK_BREQ	)
#define NSF_WACK_UREQ	( 1 << NS_WACK_UREQ	)
#define NSF_IDLE	( 1 << NS_IDLE		)
#define NSF_WACK_OPTREQ	( 1 << NS_WACK_OPTREQ	)
#define NSF_WACK_RRES	( 1 << NS_WACK_RRES	)
#define NSF_WCON_CREQ	( 1 << NS_WCON_CREQ	)
#define NSF_WRES_CIND	( 1 << NS_WRES_CIND	)
#define NSF_WACK_CRES	( 1 << NS_WACK_CRES	)
#define NSF_DATA_XFER	( 1 << NS_DATA_XFER	)
#define NSF_WCON_RREQ	( 1 << NS_WCON_RREQ	)
#define NSF_WRES_RIND	( 1 << NS_WRES_RIND	)
#define NSF_WACK_DREQ6	( 1 << NS_WACK_DREQ6	)
#define NSF_WACK_DREQ7	( 1 << NS_WACK_DREQ7	)
#define NSF_WACK_DREQ9	( 1 << NS_WACK_DREQ9	)
#define NSF_WACK_DREQ10	( 1 << NS_WACK_DREQ10	)
#define NSF_WACK_DREQ11	( 1 << NS_WACK_DREQ11	)
#define NSF_NOSTATES	( 1 << NS_NOSTATES	)
#define NSM_WACK_DREQ	(NSF_WACK_DREQ6\
			|NSF_WACK_DREQ7\
			|NSF_WACK_DREQ9\
			|NSF_WACK_DREQ10\
			|NSF_WACK_DREQ11)
#define NSM_LISTEN	(NSF_IDLE\
			|NSF_WRES_CIND\
			|NSF_WACK_CRES)
#define NSM_CONNECTED	(NSF_WCON_CREQ\
			|NSF_WRES_CIND\
			|NSF_WACK_CRES\
			|NSF_DATA_XFER\
			|NSF_WCON_RREQ\
			|NSF_WRES_RIND\
			|NSF_WACK_RRES)
#define NSM_DISCONN	(NSF_IDLE\
			|NSF_UNBND)
#define NSM_INDATA	(NSF_DATA_XFER\
			|NSF_WCON_RREQ)
#define NSM_OUTDATA	(NSF_DATA_XFER\
			|NSF_WRES_RIND\
			|NSF_WACK_RRES)
#ifndef N_PROVIDER
#define N_PROVIDER  0
#define N_USER	    1
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

typedef struct udp {
	struct udp *next;		/* list of all UDP-Users */
	struct udp **prev;		/* list of all UDP-Users */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* associated rd queue */
	queue_t *wq;			/* associated wr queue */
	cred_t cred;			/* credentials */
	atomic_t refcnt;		/* structure reference count */
	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* RD queue waiting on lock */
	queue_t *wwait;			/* WR queue waiting on lock */
	int users;			/* lock holders */
	bcid_t rbid;			/* RD buffer call id */
	bcid_t wbid;			/* WR buffer call id */
	t_uscalar_t i_flags;		/* TPI interface flags */
	t_uscalar_t i_state;		/* TPI interface state */
	np_ulong n_flags;		/* NPI interface flags */
	np_ulong n_state;		/* NPI interface state */
	struct udp *bnext;		/* linkage for bind hash */
	struct udp **bprev;		/* linkage for bind hash */
	struct udp_bind_bucket *bindb;	/* linkage for bind hash */
	short port;			/* bound port number - host order */
	struct sockaddr_storage src;	/* bound address */
	struct sockaddr_storage dst;	/* connected address */
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

STATIC kmem_cache_t *udp_bind_cachep;
STATIC kmem_cache_t *udp_priv_cachep;

STATIC INLINE struct udp *
udp_get(void)
{
	struct udp *udp;

	if ((udp = kmem_cache_alloc(udp_priv_cachep, SLAB_ATOMIC))) {
		bzero(udp, sizeof(*udp));
		atomic_set(&udp->refcnt, 1);
	}
	return (udp);
}
STATIC INLINE void
udp_hold(struct udp *udp)
{
	if (udp)
		atomic_inc(&udp->refcnt);
}
STATIC INLINE void
udp_put(struct udp *udp)
{
	if (udp && atomic_dec_and_test(&udp->refcnt))
		kmem_cache_free(udp_priv_cachep, udp);
}

/*
 *  LOCKING
 */
STATIC int
udp_trylockq(queue_t *q)
{
	int res;
	struct udp *udp = PRIV(q);

	spin_lock_bh(&udp->qlock);
	if (!(res = !udp->users++)) {
		if (q == udp->rq)
			udp->rwait = q;
		if (q == udp->wq)
			udp->wwait = q;
	}
	spin_unlock_bh(&udp->qlock);
	return (res);
}
STATIC void
udp_unlockq(queue_t *q)
{
	struct udp *udp = PRIV(q);

	spin_lock_bh(&udp->qlock);
	if (udp->rwait)
		qenable(xchg(&udp->rwait, NULL));
	if (udp->wwait)
		qenable(xchg(&udp->wwait, NULL));
	udp->users = 0;
	spin_unlock_bh(&udp->qlock);
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
STATIC void
tpi_set_state(struct udp *udp, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, udp, tpi_state_name(state),
		tpi_state_name(udp->i_state)));
	udp->i_state = state;
}
STATIC long
tpi_get_state(struct udp *udp)
{
	return (udp->i_state);
}

#ifdef _DEBUG
STATIC const char *
npi_state_name(np_long state)
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

STATIC void
npi_set_state(struct udp *udp, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, udp, npi_state_name(state),
		npi_state_name(udp->n_state)));
	udp->n_state = state;
}
STATIC long
npi_get_state(struct udp *udp)
{
	return (udp->n_state);
}

/*
 *  BUFFER ALLOCATION
 */

/**
 *  udp_bufsrv: - safe bufcalls
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
udp_bufsrv(long data)
{
	struct udp *udp;
	queue_t *q;

	q = (queue_t *) data;
	ensure(q, return);
	udp = PRIV(q);
	ensure(udp, return);

	if (q == udp->rq && xchg(&udp->rbid, 0) != 0)
		udp_put(udp);
	if (q == udp->wq && xchg(&udp->wbid, 0) != 0)
		udp_put(udp);
	qenable(q);
	return;
}

/**
 *  udp_unbufcall: - reliably cancel a buffer callback
 *  @q: the queue whose callback to cancel
 *
 *  Cancel a qenable() buffer callback associated with one queue in the queue pair.  udp_unbufcall()
 *  effectively undoes the actions performed by udp_bufcall().
 *
 *  NOTICES: Cancellation of buffer callbacks on LiS using unbufcall() is unreliable.  The callback
 *  function could execute some time shortly after the call to unbufcall() has returned.   LiS
 *  abbrogates the SVR 4 STREAMS principles for unbufcall().  This is why you will find atomic
 *  exchanges here and in the callback function and why reference counting is performed on the
 *  structure and queue pointers are checked for NULL.
 */
STATIC void
udp_unbufcall(queue_t *q)
{
	struct udp *udp;
	bufcall_id_t bid;

	ensure(q, return);
	udp = PRIV(q);
	ensure(udp, return);

	if (q == udp->rq && (bid = xchg(&udp->rbid, 0))) {
		unbufcall(bid);
		udp_put(udp);
	}
	if (q == udp->wq && (bid = xchg(&udp->wbid, 0))) {
		unbufcall(bid);
		udp_put(udp);
	}
	return;
}

/**
 *  udp_bufcall: - generate a buffer callback to enable a queue
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
udp_bufcall(queue_t *q, size_t size, int prior)
{
	struct udp *udp;
	bufcall_id_t bid;

	ensure(q, return);
	udp = UDP_PRIV(q);
	ensure(udp, return);
	udp_hold(udp);
	if (q == udp->wq) {
		if ((bid = xchg(&udp->wbid, bufcall(size, prior, &udp_bufsrv, (long) udp)))) {
			unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
			udp_put(udp);
		}
		return;
	}
	if (q == udp->rq) {
		if ((bid = xchg(&udp->rbid, bufcall(size, prior, &udp_bufsrv, (long) udp)))) {
			unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
			udp_put(udp);
		}
		return;
	}
	swerr();
	return;
}

/**
 *  udp_allocb: - reliable allocb()
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
udp_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;

	if (!(mp = allocb(size, prior)))
		udp_bufcall(q, size, prior);
	return (mp);
}

/**
 *  udp_allocb: - reliable allocb()
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
udp_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;

	if (!(mp = esballoc(base, size, prior, frtn)))
		udp_bufcall(q, FASTBUF, prior);
	return (mp);
}

/**
 *  udp_dupmsg: - reliable dupmsg()
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
udp_dupmsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (!(mp = dupmsg(bp)) && (q != NULL))
		udp_bufcall(q, FASTBUF, BPRI_MED);
	return (mp);
}

/**
 *  udp_dupb: - reliable dupb()
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
udp_dupb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (!(mp = dupb(bp)) && (q != NULL))
		udp_bufcall(q, FASTBUF, BPRI_MED);
	return (mp);
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
 * t_opts_size: - size options from received UDP message
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
 * t_opts_build: - build options output from received UDP message
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
 * t_errs_size: - size options from received ICMP message
 * @t: private structure
 * @mp: message pointer for ICMP message
 */
STATIC int
t_errs_size(const struct udp *t, mblk_t *mp)
{
	fixme(("Write this function."));
	return (0);
}

/**
 * t_errs_build: - build options output from receive ICMP message
 * @t: private structure
 * @mp: message pointer for ICMP message
 * @op: output pointer
 * @olen: output length
 */
STATIC int
t_errs_build(const struct udp *t, mblk_t *mp, unsigned char *op, size_t olen, int *etypep)
{
	fixme(("Write this function."));
	return (-EFAULT);
}

/**
 * t_size_default_options: - size default options
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
 * t_size_current_options: - calculate size of current options
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
 * t_size_check_option: - determine size of options output for T_CHECK
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
 * t_size_negotiate_options: - determine size of options output for T_NEGOTIATE
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
 * t_overall_result: - determine overall options results
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
 * t_build_default_options: - build options output for T_DEFAULT
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
 * t_build_current_options: - build options output for T_CURRENT
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
 * t_build_check_options: - built output options for T_CHECK
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
 * t_build_negotiate_options: - build output options for T_NEGOTIATE
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
 * t_build_options: - build the output options
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
 *  NS User -> NS Provider  (Request, Response) Primitives
 */
/**
 * n_bind_req: - send an N_BIND_REQ downstream
 * @q: active queue in queue pair
 * @add: address to bind
 * @add_len: length of the address
 * @conind: number of connection indications
 * @flags: bind flags
 */
STATIC int
n_bind_req(queue_t *q, struct sockaddr *add, socklen_t add_len, np_ulong conind, np_ulong flags)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	N_bind_req_t *p;
	queue_t *wq = udp->wq->q_next ? udp->wq : npi_bottom;

	if ((mp = udp_allocb(q, sizeof(*p) + add_len + 1, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		p->BIND_flags = flags;
		p->PROTOID_length = 1;
		p->PROTOID_offset = sizeof(*p) + add_len;
		mp->b_wptr += sizeof(*p);
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		*(unsigned char *) mp->b_wptr++ = IPPROTO_UDP;
		npi_set_state(udp, NS_WACK_BREQ);
		printd(("%s: %p: N_BIND_REQ ->\n", DRV_NAME, udp));
		putnext(wq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - send an N_UNBIND_REQ downstream
 * @q: active queue in queue pair
 */
STATIC int
n_unbind_req(queue_t *q)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	N_unbind_req_t *p;
	queue_t *wq = udp->wq->q_next ? udp->wq : npi_bottom;

	if ((mp = udp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		npi_set_state(udp, NS_WACK_UREQ);
		printd(("%s: %p: N_UNBIND_REQ ->\n", DRV_NAME, udp));
		putnext(wq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * n_unitdata_req: - send an N_UNITDATA_REQ downstream
 * @q: active queue in queue pair
 * @dst: destination address
 * @dst_len: length of destination address
 * @src: source address
 * @src_len: length of source address
 * @dp: data
 */
STATIC int
n_unitdata_req(queue_t *q, struct sockaddr *dst, socklen_t dst_len, struct sockaddr *src,
	       socklen_t src_len, mblk_t *dp)
{
	struct udp *udp = UDP_PRIV(q);
	mblk_t *mp;
	N_unitdata_req_t *p;
	queue_t *wq = udp->wq->q_next ? udp->wq : npi_bottom;

	if (wq != NULL) {
		if (bcanputnext(wq, dp->b_band)) {
			if ((mp = udp_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = N_UNITDATA_REQ;
				p->DEST_length = dst_len;
				p->DEST_offset = dst_len ? sizeof(*p) : 0;
				p->RESERVED_field[0] = src_len;
				p->RESERVED_field[1] = src_len ? sizeof(*p) + dst_len : 0;
				mp->b_wptr += sizeof(*p);
				if (dst_len) {
					bcopy(dst, mp->b_wptr, dst_len);
					mp->b_wptr += dst_len;
				}
				if (src_len) {
					bcopy(src, mp->b_wptr, src_len);
					mp->b_wptr += src_len;
				}
				mp->b_cont = dp;
				mp->b_band = dp->b_band;
				printd(("%s: %p: N_UNITDATA_REQ ->\n", DRV_NAME, udp));
				putnext(wq, mp);
				return (0);
			}
			ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
			return (-ENOBUFS);
		}
		ptrace(("%s: ERROR: Flow controlled\n", DRV_NAME));
		return (-EBUSY);
	}
	ptrace(("%s: ERROR: No queue\n", DRV_NAME));
	return (-EAGAIN);
}

/*
 * UDP actions
 */

STATIC struct udp *
udp_lookup(uint16_t dport, uint16_t sport, uint32_t daddr, uint32_t saddr)
{
	fixme(("Write this function."));
	return (NULL);
}

STATIC struct udp *
udp_lookup_icmp(uint16_t sport, uint16_t dport, uint32_t saddr, uint32_t daddr)
{
	fixme(("Write this function."));
	return (NULL);
}

/**
 * udp_bind: - bind a Stream to an NSAP
 * @q: active queue in queue pair (write queue)
 * @add: address to bind
 * @add_len: length of address
 *
 * There are two ways to bind a Stream:  1) When the Stream is a pseudo-device driver or when the
 * Stream is a multiplex driver, the bind is performed internally using hash tables.  2) When the
 * Stream is a pushable module, the bind is transformed into an N_BIND_REQ message and passed
 * downstream.
 */
STATIC int
udp_bind(queue_t *q, struct sockaddr *add, socklen_t add_len)
{
	struct udp *udp = UDP_PRIV(q);

	if (q->q_next != NULL) {
		return n_bind_req(q, add, add_len, 0, 0);
	} else {
		/* check if address is in use or return TADDRBUSY */
		/* really bind */
		bcopy(add, &udp->src, add_len);
		return (0);
	}
}

/**
 * udp_unbind: - unbind a Stream from an NSAP
 * @q: active queue in queue pair (write queue)
 *
 * There are two ways to unbind a Stream:  1) when the Stream is a pseudo-device driver or when the
 * Stream is a multiplex driver, the unbind is performed internally using hash tables.  2) When the
 * Stream is a pushable module, the bind is transformed into an N_UNBIND_REQ message and passed
 * downstream.
 */
STATIC int
udp_unbind(queue_t *q)
{
	struct udp *udp = UDP_PRIV(q);

	if (q->q_next != NULL) {
		return n_unbind_req(q);
	} else {
		/* really unbind */
		bzero(&udp->src, sizeof(udp->src));
		return (0);
	}
}

/**
 * udp_xmitmsg: - send a message from a Stream
 * @q: active queue in queue pair (write queue)
 * @mp: T_UNITDATA_REQ message
 * @opts: UDP options for send
 *
 * There are two ways to send a message: 1) When the Stream is a multiplex driver or when the Stream
 * is a pushable module, the message is transformed into an N_UNITDATA_REQ and passed to the lower
 * multiplex or downstream.  2) When the Stream is a pseudo-device driver, the message is sent to
 * the IP layer directly.
 */
STATIC int
udp_xmitmsg(queue_t *q, mblk_t *mp, struct udp_options *opts)
{
	struct udp *udp = UDP_PRIV(q);
	queue_t *wq = udp->wq->q_next ? udp->wq : npi_bottom;

	if (wq != NULL) {
		int rtn;
		struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
		struct sockaddr *src = NULL, *dst;
		socklen_t src_len = 0, dst_len;
		struct sockaddr_in sin;

		if (opts != NULL && t_tst_bit(_T_BIT_IP_ADDR, opts->flags)) {
			sin.sin_family = AF_INET;
			sin.sin_port = htons(udp->port);
			sin.sin_addr.s_addr = opts->ip.addr;
			src = (struct sockaddr *) &sin;
			src_len = sizeof(sin);
		}
		dst = (struct sockaddr *) ((unsigned char *) p + p->DEST_offset);
		dst_len = p->DEST_length;
		rtn = n_unitdata_req(q, dst, dst_len, src, src_len, mp->b_cont);
		if (rtn == QR_DONE)
			return (QR_TRIMMED);
		return (rtn);

	} else {
		/* FIXME: need to send message directly */
		return (-EFAULT);
	}
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
	struct udp *udp = PRIV(q);
	mblk_t *mp;

	if ((mp = udp_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_FLUSH;
		*mp->b_wptr++ = how;
		*mp->b_wptr++ = band;
		putnext(udp->rq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * m_error: deliver an M_ERROR message upstream
 * @q: a queue in the queue pair
 * @error: the error to deliver
 */
STATIC int
m_error(queue_t *q, int error)
{
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	int hangup = 0;

	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if ((mp = udp_allocb(q, 2, BPRI_HI))) {
		if (hangup) {
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, udp));
			mp->b_datap->db_type = M_HANGUP;
			putnext(udp->rq, mp);
			error = EPIPE;
		} else {
			printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, udp, error));
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error;
			*(mp->b_wptr)++ = error;
			putnext(udp->rq, mp);
		}
		return (-error);
	}
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
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;

	if ((mp = udp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = (unsigned short) 0xffff;
		p->ETSDU_size = T_INVALID;
		p->CDATA_size = T_INVALID;
		p->DDATA_size = T_INVALID;
		p->ADDR_size = sizeof(struct sockaddr_in);
		p->OPT_size = (unsigned short) 0xffff;
		p->TIDU_size = T_INVALID;
		p->SERV_type = T_CLTS;
		p->CURRENT_state = tpi_get_state(udp);
		p->PROVIDER_flag = T_XPG4_1 & ~T_SNDZERO;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, udp));
		putnext(udp->rq, mp);
		return (0);
	}
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
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_bind_ack *p;

	if ((mp = udp_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
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
		putnext(udp->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * t_error_ack: send a T_ERROR_ACK upstream with state changes
 * @q: a queue in the queue pair
 * @prim: primitive in error
 * @error: TPI/UNIX error
 */
STATIC int
t_error_ack(queue_t *q, t_scalar_t prim, t_scalar_t error)
{
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_error_ack *p;

	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (error);
	case 0:
		never();
		return (error);
	}
	if (!(mp = udp_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	/* This is to only try and get the state correct for putnext. */
	if (error != TOUTSTATE) {
		switch (tpi_get_state(udp)) {
#ifdef TS_WACK_OPTREQ
		case TS_WACK_OPTREQ:
			tpi_set_state(udp, TS_IDLE);
			break;
#endif
		case TS_WACK_BREQ:
			tpi_set_state(udp, TS_UNBND);
			break;
		case TS_WACK_UREQ:
			tpi_set_state(udp, TS_IDLE);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   TS_IDLE state. */
			break;
		}
	}
	printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, udp));
	putnext(udp->rq, mp);
	/* Returning -EPROTO here will make sure that the old state is restored correctly.  If we
	   return QR_DONE, then the state will never be restored. */
	if (error < 0)
		return (error);
	return (-EPROTO);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * t_ok_ack: deliver a T_OK_ACK upstream with state changes
 * @q: a queue in the queue pair
 * @prim: correct primitive
 */
STATIC int
t_ok_ack(queue_t *q, t_scalar_t prim)
{
	int err = -EFAULT;
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_ok_ack *p;

	if ((mp = udp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (tpi_get_state(udp)) {
		case TS_WACK_UREQ:
			if ((err = udp_unbind(q)))
				goto free_error;
			/* TPI spec says that if the provider must flush both queues before
			   responding with a T_OK_ACK primitive when responding to a T_UNBIND_REQ.
			   This is to flush queued data for connectionless providers. */
			if (m_flush(q, FLUSHRW, 0) == -ENOBUFS)
				goto enobufs;
			tpi_set_state(udp, TS_UNBND);
			break;
		default:
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   the TS_IDLE state. */
		}
		printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, udp));
		putnext(udp->rq, mp);
		return (QR_DONE);
	}
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, prim, err);
}

/**
 * t_unitdata_ind: - send a T_UNITDATA_IND upstream
 * @q: a queue in the queue pair
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block contains the IP, UDP header and UDP payload starting at
 * dp->b_datap->db_base.  The UDP message payload starts at dp->b_rptr.  This function extracts IP
 * header information and uses it to create options.
 */
STATIC int
t_unitdata_ind(queue_t *q, mblk_t *dp)
{
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t opt_len = t_opts_size(udp, dp);
	const size_t src_len = sizeof(struct sockaddr_in);

	if ((mp = udp_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_UNITDATA_IND;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
		mp->b_wptr += sizeof(*p);
		if (src_len) {
			struct sockaddr_in *sin = (typeof(sin)) mp->b_wptr;
			struct iphdr *iph = (typeof(iph)) dp->b_datap->db_base;
			struct udphdr *uh = (typeof(uh)) (dp->b_datap->db_base + (iph->ihl << 2));

			sin->sin_family = AF_INET;
			sin->sin_port = uh->source;
			sin->sin_addr.s_addr = iph->saddr;
			mp->b_wptr += src_len;
		}
		if (opt_len) {
			t_opts_build(udp, dp, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, udp));
		putnext(udp->rq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: - send a T_UDERROR_IND upstream
 * @q: a queue in the queue pair
 * @dp: data block containing IP packet
 *
 * IMPLEMENTATION: The data block contains the IP, ICMP header and ICMP payload starting at
 * dp->b_datap->db_base.  The ICMP message payload starts at dp->b_rptr. This function extracts IP
 * header information and uses it to create options.
 */
STATIC INLINE int
t_uderror_ind(queue_t *q, mblk_t *dp)
{
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t opt_len = t_errs_size(udp, dp);
	int etype = 0;
	const size_t src_len = sizeof(struct sockaddr_in);

	if (canputnext(udp->rq)) {
		if ((mp = udp_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = src_len;
			p->DEST_offset = src_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
			mp->b_wptr += sizeof(*p);
			if (src_len) {
				struct sockaddr_in *sin = (typeof(sin)) mp->b_wptr;
				struct iphdr *iph = (typeof(iph)) dp->b_datap->db_base;
				struct udphdr *uh = (typeof(uh)) (mp->b_datap->db_base + (iph->ihl << 2));

				sin->sin_family = AF_INET;
				sin->sin_port = uh->source;
				sin->sin_addr.s_addr = iph->saddr;
				mp->b_wptr += src_len;
			}
			if (opt_len) {
				t_errs_build(udp, dp, mp->b_wptr, opt_len, &etype);
				mp->b_wptr += opt_len;
			}
			p->ERROR_type = etype;
			mp->b_cont = dp;
			printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, udp));
			putnext(udp->rq, mp);
			return (0);
		}
		ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
		return (-ENOBUFS);
	}
	ptrace(("%s: ERROR: Flow controlled\n", DRV_NAME));
	return (-EBUSY);
}

/**
 * t_optmgmt_ack: - send a T_OPTMGMT_ACK upstream
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
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if ((mp = udp_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
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
		putnext(udp->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/**
 * t_addr_ack: - send a T_ADDR_ACK upstream
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
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_addr_ack *p;

	if ((mp = udp_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
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
		putnext(udp->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif				/* T_ADDR_ACK */

#ifdef T_CAPABILITY_ACK
/**
 * t_capability_ack: - send a T_CAPABILITY_ACK upstream
 * @q: a queue in the queue pair
 * @caps: capability bits
 * @type: STREAMS message type M_PROTO or M_PCPROTO
 */
STATIC int
t_capability_ack(queue_t *q, t_uscalar_t caps, int type)
{
	struct udp *udp = PRIV(q);
	mblk_t *mp;
	struct T_capability_ack *p;

	if ((mp = udp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = type;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = caps & (TC1_INFO | TC1_ACCEPTOR_ID);
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (long) udp->rq : 0;
		mp->b_wptr += sizeof(*p);
		if (caps & TC1_INFO) {
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = (unsigned short) 0xffff;
			p->INFO_ack.ETSDU_size = T_INVALID;
			p->INFO_ack.CDATA_size = T_INVALID;
			p->INFO_ack.DDATA_size = T_INVALID;
			p->INFO_ack.ADDR_size = sizeof(struct sockaddr_in);
			p->INFO_ack.OPT_size = T_INFINITE;
			p->INFO_ack.TIDU_size = (unsigned short) 0xffff;
			p->INFO_ack.SERV_type = T_CLTS;
			p->INFO_ack.CURRENT_state = tpi_get_state(udp);
			p->INFO_ack.PROVIDER_flag = T_XPG4_1 & ~T_SNDZERO;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, udp));
		putnext(udp->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif				/* T_CAPABILITY_ACK */

/*
 *  TS User -> TS Provider (Request and Response) primitives
 */

/**
 * t_info_req: - process a T_INFO_REQ primitive
 * @q: active queue in queue pair
 * @mp: the message
 */
STATIC int
t_info_req(queue_t *q, mblk_t *mp)
{
	return t_info_ack(q);
}

/**
 * t_bind_req: - process a T_BIND_REQ primitive
 * @q: active queue in queue pair
 * @mp: the message
 */
STATIC int
t_bind_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = PRIV(q);
	int err, add_len, type;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr *add = (struct sockaddr *)&udp->src;
	struct sockaddr_in *add_in;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (tpi_get_state(udp) != TS_UNBND)
		goto outstate;
	tpi_set_state(udp, TS_WACK_BREQ);
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)) {
		ptrace(("%s: %p: ADDR_offset(%u) or ADDR_length(%u) are incorrect\n", DRV_NAME, udp,
			p->ADDR_offset, p->ADDR_length));
		goto badaddr;
	}
	if ((add_len = p->ADDR_length) == 0) {
		bzero(add, sizeof(add));
		add_len = sizeof(struct sockaddr_in);
		add->sa_family = AF_INET;
	} else {
		if (add_len < sizeof(struct sockaddr_in)) {
			ptrace(("%s: %p: add_len(%u) < sizeof(struct sockaddr_in)(%u)\n",
				DRV_NAME, udp, add_len, sizeof(struct sockaddr_in)));
			goto badaddr;
		}
		if (add_len > sizeof(struct sockaddr_in)) {
			ptrace(("%s: %p: add_len(%u) > sizeof(struct sockaddr_in)(%u)\n",
				DRV_NAME, udp, add_len, sizeof(struct sockaddr_in)));
			goto badaddr;
		}
		if (add->sa_family != AF_INET && add->sa_family != 0) {
			ptrace(("%s: %p: sa_family incorrect (%u)\n", DRV_NAME, udp,
				add->sa_family));
			goto badaddr;
		}
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	}
	add_in = (typeof(add_in)) add;
	type = inet_addr_type(add_in->sin_addr.s_addr);
	if (sysctl_ip_nonlocal_bind == 0 && add_in->sin_addr.s_addr != INADDR_ANY && type != RTN_LOCAL
	    && type != RTN_MULTICAST && type != RTN_BROADCAST)
		goto noaddr;
	udp->port = ntohs(add_in->sin_port);
	/* check for bind to privileged port */
	if (udp->port && udp->port < PROT_SOCK && !capable(CAP_NET_BIND_SERVICE))
		goto acces;
	if ((err = udp_bind(q, add, add_len)))
		goto error;
	return t_bind_ack(q, (struct sockaddr *)&udp->src, sizeof(struct sockaddr_in), p->CONIND_number);
      acces:
	err = TACCES;
	ptrace(("%s: ERROR: no permission for address\n", DRV_NAME));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: ERROR: address could not be assigned\n", DRV_NAME));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: ERROR: address is invalid\n", DRV_NAME));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_BIND_REQ, err);
}

/**
 * t_unbind_req: - process a T_UNBIND_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = PRIV(q);

	if (tpi_get_state(udp) != TS_IDLE)
		goto outstate;
	tpi_set_state(udp, TS_WACK_UREQ);
	return t_ok_ack(q, T_UNBIND_REQ);
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	return t_error_ack(q, T_UNBIND_REQ, TOUTSTATE);
}

/**
 * t_unitdata_req: - process a T_UNITDATA_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = PRIV(q);
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (dlen == 0)
		goto baddata;
	if (dlen > (unsigned short) 0xffff)
		goto baddata;
	if (tpi_get_state(udp) != TS_IDLE)
		goto outstate;
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->DEST_length < sizeof(struct sockaddr_in)))
		goto badadd;
	if (p->OPT_length == 0)
		return udp_xmitmsg(q, mp, NULL);
	{
		struct udp_options opts;
		unsigned char *ip;
		size_t ilen;
		struct t_opthdr *ih;
		int optlen;

		opts = udp->options;
		ip = mp->b_rptr + p->OPT_offset;
		ilen = p->OPT_length;

		for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih;
		     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
			if (ih->len < sizeof(*ih))
				goto badopt;
			if ((unsigned char *) ih + ih->len > ip + ilen)
				goto badopt;
			optlen = ih->len - sizeof(*ih);
			switch (ih->level) {
			default:
				goto badopt;
			case T_INET_IP:
				switch (ih->name) {
				default:
					goto badopt;
				case T_IP_OPTIONS:
					t_set_bit(_T_BIT_IP_OPTIONS, opts.flags);
					continue;
				case T_IP_TOS:
					t_set_bit(_T_BIT_IP_TOS, opts.flags);
					opts.ip.tos = *(unsigned char *) T_OPT_DATA(ih);
					continue;
				case T_IP_TTL:
					t_set_bit(_T_BIT_IP_TTL, opts.flags);
					opts.ip.ttl = *(unsigned char *) T_OPT_DATA(ih);
					continue;
				case T_IP_DONTROUTE:
					t_set_bit(_T_BIT_IP_DONTROUTE, opts.flags);
					opts.ip.dontroute = *(t_uscalar_t *) T_OPT_DATA(ih);
					continue;
				case T_IP_BROADCAST:
					t_set_bit(_T_BIT_IP_BROADCAST, opts.flags);
					opts.ip.broadcast = *(t_uscalar_t *) T_OPT_DATA(ih);
					continue;
				case T_IP_ADDR:
					t_set_bit(_T_BIT_IP_ADDR, opts.flags);
					opts.ip.addr = *(uint32_t *) T_OPT_DATA(ih);
					continue;
				case T_IP_REUSEADDR:
					t_set_bit(_T_BIT_IP_REUSEADDR, opts.flags);
					/* or do we generate a T_UDERROR_IND */
					continue;
				}
			case T_INET_UDP:
				switch (ih->name) {
				default:
					goto badopt;
				case T_UDP_CHECKSUM:
					t_set_bit(_T_BIT_UDP_CHECKSUM, opts.flags);
					opts.udp.checksum = *(t_uscalar_t *) T_OPT_DATA(ih);
					continue;
				}
			}
		}
		return udp_xmitmsg(q, mp, &opts);
	}
	/* FIXME: we can send uderr for some of these instead of erroring out the entire stream. */
      badopt:
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
#if 0
      acces:
	ptrace(("%s: ERROR: no permission to address or options\n", DRV_NAME));
	goto error;
#endif
      badadd:
	ptrace(("%s: ERROR: bad destination address\n", DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive\n", DRV_NAME));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      baddata:
	ptrace(("%s: ERROR: bad data size\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

/**
 * t_optmgmt_req: - handle T_OPTMGMT_REQ primitive
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
	struct udp *udp = PRIV(q);
	int err, opt_len;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
#ifdef TS_WACK_OPTREQ
	if (tpi_get_state(udp) == TS_IDLE)
		tpi_set_state(udp, TS_WACK_OPTREQ);
#endif
	if (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
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
		goto badflag;
	}
	if (opt_len < 0) {
		switch (-(err = opt_len)) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto provspec;
		}
	}
	if ((err = t_optmgmt_ack(q, p->MGMT_flags, mp->b_rptr + p->OPT_offset,
				 p->OPT_length, opt_len)) < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		case ENOBUFS:
		case ENOMEM:
			break;
		default:
			goto provspec;
		}
	}
	return (err);
      provspec:
	err = err;
	ptrace(("%s: ERROR: provider specific\n", DRV_NAME));
	goto error;
      badflag:
	err = TBADFLAG;
	ptrace(("%s: ERROR: bad options flags\n", DRV_NAME));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: ERROR: no permission for option\n", DRV_NAME));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_OPTMGMT_REQ, err);
}

#ifdef T_ADDR_REQ
/**
 * t_addr_req: - process a T_ADDR_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_addr_req(queue_t *q, mblk_t *mp)
{
	struct udp *udp = PRIV(q);

	(void) mp;
	switch (tpi_get_state(udp)) {
	case TS_UNBND:
		return t_addr_ack(q, NULL, 0, NULL, 0);
	case TS_IDLE:
		return t_addr_ack(q, (struct sockaddr *) &udp->src, sizeof(struct sockaddr_in),
				  NULL, 0);
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		return t_addr_ack(q, (struct sockaddr *) &udp->src, sizeof(struct sockaddr_in),
				  (struct sockaddr *) &udp->dst, sizeof(struct sockaddr_in));
	case TS_WRES_CIND:
		return t_addr_ack(q, NULL, 0, (struct sockaddr *) &udp->dst,
				  sizeof(struct sockaddr_in));
	}
	return t_error_ack(q, T_ADDR_REQ, TOUTSTATE);
}
#endif				/* T_ADDR_REQ */

#ifdef T_CAPABILITY_REQ
/**
 * t_capability_req: - process a T_CAPABILITY_REQ primitive
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_capability_req(queue_t *q, mblk_t *mp)
{
	int err = -EFAULT;
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(q, p->CAP_bits1, mp->b_datap->db_type);
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid message format\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_CAPABILITY_REQ, err);
}
#endif				/* T_CAPABILITY_REQ */

/**
 * t_other_req: - process unsupported recognized primitive type
 * @q: active queue in queue pair
 * @mp: the primitive
 */
STATIC int
t_other_req(queue_t *q, mblk_t *mp)
{
	t_scalar_t prim = *((t_scalar_t *) mp->b_rptr);

	return t_error_ack(q, prim, TNOTSUPPORT);
}

/*
 * NS Provider -> NS User primitives
 */

/**
 * n_bind_ack: - process an N_BIND_ACK primitive
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
n_bind_ack(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	N_bind_ack_t *p;
	struct sockaddr *add = NULL;
	socklen_t add_len = 0;
	int rtn = 0;

	switch (npi_get_state(udp)) {
	case NS_WACK_BREQ:
		p = (typeof(p)) mp->b_rptr;
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto efault;
		if (p->ADDR_length) {
			if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
				goto efault;
			add = (struct sockaddr *) ((unsigned char *) p + p->ADDR_offset);
			add_len = p->ADDR_length;
		}
		rtn = t_bind_ack(q, add, add_len, p->CONIND_number);
		if (rtn >= 0)
			npi_set_state(udp, NS_IDLE);
		break;
	default:
		break;
	}
	return (rtn);
      efault:
	return (-EFAULT);
}

/**
 * n_ok_ack: - process an N_OK_ACK primitive
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
n_ok_ack(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	int rtn = 0;

	switch (npi_get_state(udp)) {
	case NS_WACK_UREQ:
		rtn = t_ok_ack(q, T_UNBIND_REQ);
		if (rtn >= 0)
			npi_set_state(udp, NS_UNBND);
		break;
	case NS_WACK_OPTREQ:
		if (rtn >= 0)
			npi_set_state(udp, NS_IDLE);
		break;
	default:
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when this is a response to an N_OPTMGMT_REQ that was issued in other
		   than the NS_IDLE state. */
		break;
	}
	return (rtn);
}

/**
 * n_tpi_error: - map NPI errors to TPI errors
 * @p: pointer to N_ERROR_ACK primitive
 */
STATIC t_scalar_t
n_tpi_error(N_error_ack_t * p)
{
	switch (p->NPI_error) {
	case NBADADDR:
		return (TBADADDR);
	case NBADOPT:
		return (TBADOPT);
	case NACCESS:
		return (TACCES);
	case NNOADDR:
		return (TNOADDR);
	case NOUTSTATE:
		return (TOUTSTATE);
	case NBADSEQ:
		return (TBADSEQ);
	case NSYSERR:
		return (-(t_scalar_t) p->UNIX_error);
	case NBADDATA:
		return (TBADDATA);
	case NBADFLAG:
		return (TBADOPT);
	case NNOTSUPPORT:
		return (TNOTSUPPORT);
	case NBOUND:
		return (TADDRBUSY);
	case NBADQOSPARAM:
		return (TBADOPT);
	case NBADQOSTYPE:
		return (TBADOPT);
	case NBADTOKEN:
		return (TBADF);
	case NNOPROTOID:
		return (TNOADDR);
	default:
		return (-EINVAL);
	}
}

/**
 * n_error_ack: - process an N_ERROR_ACK primitive
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
n_error_ack(queue_t *q, mblk_t *mp)
{
	struct udp *udp = UDP_PRIV(q);
	int rtn = 0;

	switch (npi_get_state(udp)) {
	case NS_WACK_OPTREQ:
		if (rtn >= 0)
			npi_set_state(udp, NS_IDLE);
		break;
	case NS_WACK_BREQ:
		rtn = t_error_ack(q, T_BIND_REQ, n_tpi_error((N_error_ack_t *) mp->b_rptr));
		if (rtn >= 0)
			npi_set_state(udp, NS_UNBND);
		break;
	case NS_WACK_UREQ:
		rtn = t_error_ack(q, T_UNBIND_REQ, n_tpi_error((N_error_ack_t *) mp->b_rptr));
		if (rtn >= 0)
			npi_set_state(udp, NS_IDLE);
		break;
	default:
		/* Note: if not in a WACK state simply do not change state.  This occurs normally
		   when responding to an N_OPTMGMT_REQ in other than NS_IDLE state. */
		break;
	}
	return (rtn);
}

/**
 * n_unitdata_ind: - process an N_UNITDATA_IND primitive
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
n_unitdata_ind(queue_t *q, mblk_t *mp)
{
	int rtn;

	rtn = t_unitdata_ind(q, mp->b_cont);
	if (rtn == QR_DONE) {
		freeb(mp);
		return (QR_ABSORBED);
	}
	return (rtn);
}

/**
 * n_uderror_ind: - process an N_UDERROR_IND primitive
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
n_uderror_ind(queue_t *q, mblk_t *mp)
{
	int rtn;

	rtn = t_uderror_ind(q, mp->b_cont);
	if (rtn == QR_DONE) {
		freeb(mp);
		return (QR_ABSORBED);
	}
	return (rtn);
}

/**
 * n_other_ind: - process an unsupported primitvie type
 * @q: active queue in queue pair (read queue)
 * @mp: the primitive
 */
STATIC int
n_other_ind(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  NPI IP Provider -> UDP Provider messages.
 */

/**
 * l_unidata_ind: - process N_UNITDATA_IND from linked NPI IP provider
 * @q: active queue in queue pair (read queue)
 * @mp: the N_UNITDATA_IND message
 *
 * N_UNITDATA_IND messages are passed from a linked NPI IP provider.  They are processed similar to
 * IP messages incoming in udp_v4_rcv().  The data block contains the IP header, UDP header and UDP
 * payload starting at mp->b_cont->b_datap->db_base.  The UDP message payload starts at
 * mp->b_cont->b_rptr.  This function extracts IP header information and uses it to create options.
 *
 * LOCKING: Holding udp_lock protects against the upper stream closing.  udp->qlock protects the
 * state of the private structure.  udp->refs protects the structure from being deallocated.
 */
STATIC int
l_unitdata_ind(queue_t *q, mblk_t *mp)
{
	struct udp *udp;
	mblk_t *dp = mp->b_cont;
	struct iphdr *iph = (typeof(iph)) dp->b_datap->db_base;
	struct udphdr *uh = (typeof(uh)) (dp->b_datap->db_base + (iph->ihl << 2));

	read_lock_bh(&udp_lock);
	udp = udp_lookup(uh->dest, uh->source, iph->daddr, iph->saddr);
	if (udp == NULL)
		goto no_stream;
	spin_lock_bh(&udp->qlock);
	if (tpi_get_state(udp) != TS_IDLE)
		goto outstate;
	if (bcanput(udp->rq, mp->b_band)) {
		put(udp->rq, mp);
		return (QR_ABSORBED);
	}
	/* Note: when flow control subsides, the upper read queue must backenable the lower read
	   queue for this to work.  Another possibility is to simply discard the message. */
	return (-EBUSY);
      no_stream:
	read_unlock_bh(&udp_lock);
	/* When there is no Stream associated with the message, we want the NPI IP driver to still
	   be able to pass the message along to another protocol, so we return it as an M_CTL
	   message.  Note that we do not check for flow control. */
	mp->b_datap->db_type = M_CTL;
	qreply(q, mp);
	return (QR_ABSORBED);
      outstate:
	spin_unlock_bh(&udp->qlock);
	udp_put(udp);
	read_unlock_bh(&udp_lock);
	/* discard */
	return (QR_DONE);
}

/**
 * l_uderror_ind: - process N_UDERROR_IND from linked NPI IP provider
 * @q: active queue in queue pair (read queue)
 * @mp: the N_UDERROR_IND message
 *
 * N_UDERROR_IND messages are passed from a linked NPI IP provider.  They are processed similar to
 * ICMP messages incoming in udp_v4_err().
 */
STATIC int
l_uderror_ind(queue_t *q, mblk_t *mp)
{
	struct udp *udp;
	mblk_t *dp = mp->b_cont;
	struct iphdr *iph = (typeof(iph)) dp->b_datap->db_base;
	struct udphdr *uh = (typeof(uh)) (dp->b_datap->db_base + (iph->ihl << 2));

	udp = udp_lookup_icmp(uh->source, uh->dest, iph->saddr, iph->daddr);
	if (udp == NULL)
		goto no_stream;
	if (bcanput(udp->rq, mp->b_band)) {
		put(udp->rq, mp);
		return (QR_ABSORBED);
	}
	/* Note: when flow control subsides, the upper read queue must backenable the lower read
	   queue for this to work.  Another possibility is to simply discard the message. */
	return (-EBUSY);
      no_stream:
	/* When there is no Stream associated with the message, we want the NPI IP driver to still
	   be able to pass the message along to another protocol, so we return it as an M_CTL
	   message.  Note that we do not check for flow control. */
	mp->b_datap->db_type = M_CTL;
	qreply(q, mp);
	return (QR_ABSORBED);
}

/**
 * l_other_ind: - process unsupported indication from linked NPI IP provider
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 *
 * Unexpected message.
 */
STATIC int
l_other_ind(queue_t *q, mblk_t *mp)
{
	swerr();
	/* discard it */
	return (QR_DONE);
}
/*
 *  STREAMS MESSAGE HANDLING
 */

/**
 * upd_w_proto: - M_PROTO, M_PCPROTO handling
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC int
udp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	t_scalar_t prim;
	udp_t *udp = PRIV(q);
	t_uscalar_t oldstate = tpi_get_state(udp);

	if (mp->b_wptr < mp->b_rptr + sizeof(t_scalar_t))
		goto eproto;
	switch ((prim = *((t_scalar_t *) mp->b_rptr))) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, udp));
		rtn = t_other_req(q, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, udp));
		rtn = t_other_req(q, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, udp));
		rtn = t_other_req(q, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, udp));
		rtn = t_other_req(q, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, udp));
		rtn = t_other_req(q, mp);
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
		rtn = t_other_req(q, mp);
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
		rtn = m_error(q, EPROTO);
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
		tpi_set_state(udp, oldstate);
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
 * udp_w_flush: - M_FLUSH handling (write queue)
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 *
 * Canonical flushing, taking into account that this might be a module pushed over a driver on the
 * stream or this might be the Stream end in either direction (i.e. if the Stream is the lower
 * multiplex or driver).
 */
STATIC int
udp_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
	}
	if (*mp->b_rptr & FLUSHR) {
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		if (*mp->b_rptr & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(RD(q), FLUSHALL);
		if (RD(q)->q_next) {
			qreply(q, mp);
			return (QR_ABSORBED);
		}
	}
	return (QR_DONE);
}

/**
 * udp_r_flush: - M_FLUSH handling (read queue)
 * @q: active queue in the queue pair (read queue)
 * @mp: the message
 *
 * Canonical flushing, taking into account that this might be a module pushed over a driver on the
 * stream or this might be the Stream end in either direction (i.e. if the Stream is the lower
 * multiplex or driver).
 */
STATIC int
udp_r_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
	}
	if (*mp->b_rptr & FLUSHW) {
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		if (*mp->b_rptr & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(WR(q), FLUSHALL);
		if (WR(q)->q_next) {
			qreply(q, mp);
			return (QR_ABSORBED);
		}
	}
	return (QR_DONE);
}

/**
 * udp_r_data: - M_DATA handling
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 *
 * M_DATA messages are placed on the read queue from below by udp_v4_rcv().  These messages are
 * transformed into T_UNITDATA_IND M_PROTO messages and passed along upstream.
 */
STATIC int
udp_r_data(queue_t *q, mblk_t *mp)
{
	return t_unitdata_ind(q, mp);
}

/**
 * udp_r_error: - M_ERROR handling
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 *
 * M_ERROR messages are placed on the read queue from below by udp_v4_err().  These messages are
 * transformed into T_UDERROR_IND M_PROTO messages and passed along upstream.
 */
STATIC int
udp_r_error(queue_t *q, mblk_t *mp)
{
	return t_uderror_ind(q, mp);
}

/**
 * udp_r_proto: - M_PROTO, M_PCPROTO handling
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 *
 * M_PROTO, M_PCPROTO messages are placed on the read queue by an NPI IP Stream below the UDP
 * driver.  The purpose of this function is for using a general purpose NPI Stream, in which case,
 * UDP can be a multiplexing driver and have a UDP protocol bound NPI-IP Stream linked beneath it at
 * initialization time, or can be a pushable module that is pushed over an NPI-IP Stream.  It is
 * that NPI-IP lower multiplex or driver Stream that places these messages on the read queue.
 */
STATIC int
udp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	np_long prim;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim))
		goto eproto;
	switch ((prim = *((np_long *) mp->b_rptr))) {
	case N_UNITDATA_IND:
		rtn = n_unitdata_ind(q, mp);
		break;
	case N_UDERROR_IND:
		rtn = n_uderror_ind(q, mp);
		break;
	case N_BIND_ACK:
		rtn = n_bind_ack(q, mp);
		break;
	case N_OK_ACK:
		rtn = n_ok_ack(q, mp);
		break;
	case N_ERROR_ACK:
		rtn = n_error_ack(q, mp);
		break;
	default:
		rtn = n_other_ind(q, mp);
		break;
	}
	return (rtn);
      eproto:
	return (-EPROTO);
}

/**
 * udp_w_prim: - process primitive on write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC int
udp_w_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return udp_w_flush(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return udp_w_proto(q, mp);
	}
	return (-EOPNOTSUPP);
}

/**
 * mux_r_proto: - process an M_PROTO message on the lower read queue
 * @q: active queue in queue pair (read queue)
 * @mp: the M_PROTO, M_PCPROTO message
 *
 * M_PROTO, M_PCPROTO messages are placed on the lower read queue by an NPI IP Stream linked beneath
 * the UDP multiplexing driver.  These are NPI message that have not yet been associated with a
 * Stream.  This function performs a similar function to that of the udp_v4_rcv() function.
 */
STATIC int
mux_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	np_ulong prim;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim))
		goto eproto;
	switch ((prim = *((np_ulong *) mp->b_rptr))) {
	case N_UNITDATA_IND:
		rtn = l_unitdata_ind(q, mp);
		break;
	case N_UDERROR_IND:
		rtn = l_uderror_ind(q, mp);
		break;
	case N_BIND_ACK:
	case N_OK_ACK:
	case N_ERROR_ACK:
	default:
		rtn = l_other_ind(q, mp);
		break;
	}
	return (rtn);
      eproto:
	return (-EPROTO);
}

/**
 * udp_r_prim: - process primitive on read queue
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
udp_r_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return udp_r_flush(q, mp);
	case M_DATA:
		return udp_r_data(q, mp);
	case M_ERROR:
		return udp_r_error(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return udp_r_proto(q, mp);
	}
	return (-EOPNOTSUPP);
}

/**
 * mux_w_prim: - proccess primitive on lower mux write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 */
STATIC int
mux_w_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return udp_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/**
 * mux_r_prim: - proccess primitive on lower mux read queue
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC int
mux_r_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return udp_r_flush(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mux_r_proto(q, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  QUEUE PUT and SERVICE Routines
 */

/**
 * udp_putq: - common put procedure
 * @q: active queue in queue pair
 * @mp: mesage to put
 * @proc: poceedure for processing message
 */
STATIC int
udp_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0, locked;

	if (mp->b_datap->db_type < QPCTL && (q->q_first || q->q_flag & QSVCBUSY)) {
		if (!putq(q, mp))
			freemsg(mp);	/* FIXME */
		return (0);
	}
	if ((locked = udp_trylockq(q)) || mp->b_datap->db_type == M_FLUSH) {
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
						freemsg(mp->b_cont);	/* FIXME */
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
				freemsg(mp);
				break;
			case QR_DISABLE:
				if (!putq(q, mp))
					freemsg(mp);	/* FIXME */
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					break;
				}
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
			case -ENOMEM:
				if (!putq(q, mp))
					freemsg(mp);	/* FIXME */
				break;
			}
		}
		while (0);
		if (locked)
			udp_unlockq(q);
	} else {
		if (!putq(q, mp))
			freemsg(mp);	/* FIXME */
	}
	return (rtn);
}

/**
 * udp_putq: - common service procedure
 * @q: active queue in queue pair
 * @proc: poceedure for processing messages
 * @procwake: wakeup method invoked after processing
 */
STATIC int
udp_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *), void (*procwake) (queue_t *))
{
	int rtn = 0;

	if (udp_trylockq(q)) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			/* Fast Path */
			if ((rtn = proc(q, mp)) == QR_DONE) {
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
				ptrace(("%s: ERROR: (q dropping) %d\n", DRV_NAME, rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				ptrace(("%s: ERROR: (q disabling)\n", DRV_NAME));
				noenable(q);
				if (!putbq(q, mp))
					freemsg(mp);	/* FIXME */
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* caller must schedule bufcall */
			case -EBUSY:	/* caller must have failed canput */
			case -EAGAIN:	/* caller must re-enable queue */
			case -ENOMEM:	/* caller must re-enable queue */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("%s: ERROR: (q stalled) %d\n", DRV_NAME, rtn));
					if (!putbq(q, mp))
						freemsg(mp);	/* FIXME */
					break;
				}
				/* Be careful not to put a priority message back on the queue.  */
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
					freemsg(mp);	/* FIXME */
				break;
			}
			break;
		}
		if (procwake != NULL)
			procwake(q);
		udp_unlockq(q);
	}
	return (rtn);
}

/**
 * udp_rwake: - an upper read queue has awoken
 * @q: active queue in the queue pair (read queue)
 *
 * The upper read queue service procudure is invoked only by back enabling (because we do not ever
 * place any message on the upper read queue but always put them to the next read queue).  When
 * invoked, we need to manually enable any lower multiplex (NPI IP) read queue.  This permits flow
 * control to work across the multiplexing driver.
 */
STATIC void
udp_rwake(queue_t *q)
{
	read_lock_bh(&udp_lock);
	if (npi_bottom != NULL)
		/* There is an NPI IP Stream linked under the driver. */
		qenable(RD(npi_bottom));
	read_unlock_bh(&udp_lock);
}

/**
 * mux_wwake: - the lower write queue has awoken
 * @q: active queue in queue pair (write queue)
 *
 * The lower write service procedure is used merely for backenabling across the multiplexing driver.
 * We never put messages to the lower write queue, but put them to the next queue below the lower
 * write queue.  When a bcanput() fails on the next queue to thelower write queue, a back enable
 * will invoke the lower write queue service procedure which can then be used to explicitly enable
 * the upper write queue(s) feeding the lower write queue.  This permits flow control to work across
 * the multiplexing driver.
 */
STATIC void
mux_wwake(queue_t *q)
{
	struct udp *top;

	read_lock_bh(&udp_lock);
	for (top = udp_opens; top; top = top->next)
		qenable(top->wq);
	read_unlock_bh(&udp_lock);
}

/**
 * udp_rput: - read side put procedure
 * @q: queue to put message to
 * @mp: message to put
 */
STATIC streamscall int
udp_rput(queue_t *q, mblk_t *mp)
{
	return udp_putq(q, mp, &udp_r_prim);
}

/**
 * udp_rsrv: - read side service procedure
 * @q: queue to service
 */
STATIC streamscall int
udp_rsrv(queue_t *q)
{
	return udp_srvq(q, &udp_r_prim, &udp_rwake);
}

/**
 * udp_wput: - write side put procedure
 * @q: queue to put message to
 * @mp: message to put
 */
STATIC streamscall int
udp_wput(queue_t *q, mblk_t *mp)
{
	return udp_putq(q, mp, &udp_w_prim);
}

/**
 * udp_wsrv: - write side service procedure
 * @q: queue to service
 */
STATIC streamscall int
udp_wsrv(queue_t *q)
{
	return udp_srvq(q, &udp_w_prim, NULL);
}

/**
 * mux_rput: - read side put procedure
 * @q: queue to put message to
 * @mp: message to put
 */
STATIC streamscall int
mux_rput(queue_t *q, mblk_t *mp)
{
	return udp_putq(q, mp, &mux_r_prim);
}

/**
 * mux_rsrv: - read side service procedure
 * @q: queue to service
 *
 * If the lower read put procedure encounters flow control on the queue beyond the accepting upper
 * read queue, it places the message back on its qeue and waits for the upper read queue service
 * procedure to enable it when congestion is cleared, or when a connection is formed.
 */
STATIC streamscall int
mux_rsrv(queue_t *q)
{
	return udp_srvq(q, &mux_r_prim, NULL);
}

/**
 * mux_wput: - write side put procedure
 * @q: queue to put message to
 * @mp: message to put
 */
STATIC streamscall int
mux_wput(queue_t *q, mblk_t *mp)
{
	return udp_putq(q, mp, &mux_w_prim);
}

/**
 * mux_wsrv: - write side service procedure
 * @q: queue to service
 */
STATIC streamscall int
mux_wsrv(queue_t *q)
{
	return udp_srvq(q, &mux_w_prim, &mux_wwake);
}

/*
 *  =========================================================================
 *
 *  IP Management
 *
 *  =========================================================================
 */

#ifdef LINUX
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
STATIC struct inet_protocol *udp_next_proto = NULL;
#endif				/* HAVE_KTYPE_STRUCT_INET_PROTOCOL */
#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
STATIC struct net_protocol *udp_next_proto = NULL;
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */
/**
 * udp_next_pkt_handler: - pass socket buffer to next packet handler
 * @skbp: pointer to the socket buffer to pass
 *
 * Return Value: Returns true when the socket buffer has been passed to the next packet handler and
 * the caller is no longer responsible for the buffer; false, when the socket buffer has not been
 * passed, and the caller is still responsible for the buffer.
 */
STATIC int
udp_next_pkt_handler(struct sk_buff **skbp)
{
	if (udp_next_proto != NULL) {
		struct sk_buff *skb;

		skb = *skbp;
		*skbp = NULL;
		return (*udp_next_proto->handler) (skb);
	}
	return (0);
}

/**
 * udp_next_err_handler: - pass socket buffer to next error handler
 * @skbp: pointer to the socket buffer to pass
 *
 * Return Value: Returns true when the socket buffer has been passed to the next error handler and
 * the caller is no longer responsible for the buffer; false, when the socket buffer has not been
 * passed, and the caller is still responsible for the buffer.
 */
STATIC void
udp_next_err_handler(struct sk_buff **skbp, u32 info)
{
	if (udp_next_proto != NULL) {
		struct sk_buff *skb = *skbp;

		skb = *skbp;
		*skbp = NULL;
		(*udp_next_proto->err_handler) (skb, info);
	}
}

/**
 * udp_free: - message block free function for mblks esballoc'ed from sk_buffs
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
 * udp_v4_rcv: - receive IPv4 UDP protocol packets
 */
STATIC int
udp_v4_rcv(struct sk_buff *skb)
{
	mblk_t *mp = NULL;
	ushort ulen;
	struct udp *udp;
	struct iphdr *iph;
	struct rtable *rt;
	struct udphdr *uh, *uh2;
	frtn_t fr = { &udp_free, (char *) skb };
	int rtn;

//	IP_INC_STATS_BH(IpInDelivers);	/* should wait... */

	if (!pskb_may_pull(skb, sizeof(struct udphdr)))
		goto too_small;

	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	rt = (struct rtable *) skb->dst;
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;

//	UDP_INC_STATS_BH(UdpInDatagrams);
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
	/* now allocate an mblk */
	if (!(mp = esballoc(skb->nh.raw, skb->len + (skb->data - skb->nh.raw), BPRI_MED, &fr)))
		goto no_buffers;
	mp->b_datap->db_type = M_DATA;
	mp->b_wptr = mp->b_rptr + skb->len + (skb->data - skb->nh.raw);
	/* trim the ip header */
	mp->b_rptr += skb->h.raw - skb->nh.raw;
	uh2 = (typeof(uh2)) mp->b_rptr;
	if (uh->check != uh2->check)
		goto sanity;
	mp->b_rptr += sizeof(struct udphdr);
	skb->dev = NULL;
	if (!udp->rq || !canput(udp->rq) || !putq(udp->rq, mp))
		goto flow_controlled;
//	UDP_INC_STATS_BH(UdpInDatagrams);
	udp_put(udp);
	read_unlock(&udp_lock);
	return (0);
      no_stream:
	read_unlock(&udp_lock);
	ptrace(("ERROR: No stream\n"));
	/* Note, if there is nobody to pass it too, we have to complete the checksum check before
	   dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
//	UDP_INC_STATS_BH(UdpNoPorts);	/* should wait... */
	goto pass_it;
      pass_it:
	rtn = udp_next_pkt_handler(&skb);
	if (skb == NULL)
		return (rtn);
	goto discard_it;
      bad_checksum:
//	UDP_INC_STATS_BH(UdpInErrors);
//	IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ??? */
	udp_put(udp);
	goto free_it;
      free_it:
	freemsg(mp);
	return (0);
      too_small:
	goto discard_it;
      linear_fail:
	udp_put(udp);
	read_unlock(&udp_lock);
	goto discard_it;
      bad_pkt_type:
	goto discard_it;
      discard_it:
	kfree_skb(skb);
	return (0);
      flow_controlled:
	udp_put(udp);
	read_unlock(&udp_lock);
	goto free_it;
      sanity:
	udp_put(udp);
	read_unlock(&udp_lock);
	goto free_it;
      no_buffers:
	ptrace(("ERROR: Could not allocate mblk\n"));
	udp_put(udp);
	read_unlock(&udp_lock);
	goto discard_it;
}

/**
 * udp_v4_err: - receive IPv4 UDP protocol ICMP packets
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
STATIC void
udp_v4_err(struct sk_buff *skb, u32 info)
{
	struct udp *udp;
	struct udphdr *uh;
	struct iphdr *iph = (struct iphdr *) skb->data;
	size_t ihl;

#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
	uh = (struct udphdr *) (skb->data + ihl);
	read_lock(&udp_lock);
	udp = udp_lookup_icmp(uh->source, uh->dest, iph->saddr, iph->daddr);
	if (udp == NULL)
		goto no_stream;
	spin_lock(&udp->qlock);
	if (udp->i_state == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		size_t mlen = sizeof(uint32_t) + sizeof(struct icmphdr *);

		/* Create a queue a specialized M_ERROR message to the Stream's read queue for
		   further processing.  The Stream will convert this message into a T_UDERR_IND
		   message and pass it along. */
		if (!udp->rq || !canput(udp->rq))
			goto flow_controlled;
		if (!(mp = allocb(mlen, BPRI_MED)))
			goto no_buffers;
		mp->b_datap->db_type = M_ERROR;
		*(uint32_t *) mp->b_wptr++ = iph->daddr;
		*(struct icmphdr *) mp->b_wptr++ = *skb->h.icmph;
		if (!putq(udp->rq, mp))
			freemsg(mp);
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
udp_alloc_priv(queue_t *q, struct udp **udpp, major_t major, minor_t minor, cred_t *crp)
{
	fixme(("Write this function."));
	return (NULL);
}
STATIC void
udp_free_priv(queue_t *q)
{
	fixme(("Write this function."));
	return;
}

/*
 *  Open and Close
 */
STATIC int udp_majors[UDP_CMAJORS] = { UDP_CMAJOR_0, };

/**
 * udp_open: - open a Stream
 * @q: read queue in queue pair
 * @devp: pointer to the device number opened
 * @flag: open flags
 * @sflag: STREAMS flags (DRVOPEN, MODOPEN, CLONEOPEN)
 * @crp: credentials pointer
 */
STATIC int
udp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
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
		if (cmajor != (*udpp)->cmajor)
			break;
		if (cmajor == (*udpp)->cmajor) {
			if (cminor < (*udpp)->cminor)
				break;
			if (cminor == (*udpp)->cminor) {
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
 * udp_close: - close a Stream
 * @q: read queue in queue pair
 * @flag: open flags
 * @crp: credentials pointer
 */
STATIC int
udp_close(queue_t *q, int flag, cred_t *crp)
{
	printd(("%s: closing character device %d:%d\n", DRV_NAME, (int) UDP_PRIV(q)->cmajor,
		(int) UDP_PRIV(q)->cminor));
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
	qprocsoff(q);
	udp_free_priv(q);
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

STATIC struct udp_bhash_bucket *udp_bhash;
STATIC size_t udp_bhash_size = 0;
STATIC size_t udp_bhash_order = 0;
STATIC void
udp_term_hashes(void)
{
	if (udp_bhash) {
		free_pages((unsigned long) udp_bhash, udp_bhash_order);
		udp_bhash = NULL;
		udp_bhash_order = 0;
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
	bzero(udp_bhash, udp_bhash_size * sizeof(struct udp_bhash_bucket));
	for (i = 0; i < udp_bhash_size; i++)
		rwlock_init(&udp_bhash[i].lock);
	printd(("%s: INFO: bind hash table configured size = %d\n", DRV_NAME, udp_bhash_size));
}

/*
 *  IP subsystem management
 */
#ifdef LINUX
STATIC struct net_protocol **inet_protosp = (typeof(inet_protosp)) HAVE_INET_PROTOS_ADDR;

#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
struct inet_protocol udp_net_protocol = {
	.handler = &udp_v4_rcv,
	.err_handler = &udp_v4_err,
	.protocol = IPPROTO_UDP,
	.name = "streams-udp",
	.next = NULL,
	.copy = 0,
};

/**
 * udp_term_nproto: - terminate UDP network protocol override
 *
 * This is largely a 2.4 version of the network protocol override function.
 */
STATIC void
udp_term_nproto(void)
{
	unsigned char hash = IPPROTO_UDP & (MAX_INET_PROTOS - 1);
	struct inet_protocol **p = (struct inet_protocol **)&inet_protosp[hash];

	br_write_lock_bh(BR_NETPROTO_LOCK);
	while ((*p) != NULL && (*p) != &udp_net_protocol)
		p = &(*p)->next;
	if ((*p) == &udp_net_protocol)
		(*p) = udp_next_proto;
	br_write_unlock_bh(BR_NETPROTO_LOCK);
}

/**
 * udp_init_nproto: - initialize UDP network protocol override
 *
 * This is largely a 2.4 version of the network protocol override function.
 */
STATIC void
udp_init_nproto(void)
{
	unsigned char hash = IPPROTO_UDP & (MAX_INET_PROTOS - 1);
	struct inet_protocol **p = (struct inet_protocol **)&inet_protosp[hash];

	br_write_lock_bh(BR_NETPROTO_LOCK);
	udp_next_proto = (*p);
	(*p) = &udp_net_protocol;
	br_write_unlock_bh(BR_NETPROTO_LOCK);
}
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL

struct net_protocol udp_net_protocol = {
#ifdef HAVE_KMEMB_STRUCT_NET_PROTOCOL_PROTO
	/* 2.6.15 is different */
	.proto = IPPROTO_UDP,
#endif
	.handler = &udp_v4_rcv,
	.err_handler = &udp_v4_err,
	.no_policy = 1,
};
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;

/**
 * udp_term_nproto: - terminate UDP network protocol override
 *
 * This is largely a 2.6 version of the network protocol override function.
 */
STATIC void
udp_term_nproto(void)
{
	int hash = IPPROTO_UDP & (MAX_INET_PROTOS - 1);

	spin_lock_bh(inet_proto_lockp);
	if (inet_protosp[hash] == &udp_net_protocol)
		inet_protosp[hash] = udp_next_proto;
	spin_unlock_bh(inet_proto_lockp);
	synchronize_net();
}

/**
 * udp_init_nproto: - initialize UDP network protocol override
 *
 * This is largely a 2.6 version of the network protocol override function.
 */
STATIC void
udp_init_nproto(void)
{
	int hash = IPPROTO_UDP & (MAX_INET_PROTOS - 1);

	spin_lock_bh(inet_proto_lockp);
	udp_next_proto = inet_protosp[hash];
	inet_protosp[hash] = &udp_net_protocol;
	spin_unlock_bh(inet_proto_lockp);
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

	(void)udp_unbufcall;
	(void)udp_esballoc;
	(void)udp_dupmsg;
	(void)udp_dupb;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	udp_init_hashes();
	udp_init_caches();
	udp_init_nproto();
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
