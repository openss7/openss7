/*****************************************************************************

 @(#) $RCSfile: np_ip.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/03/25 06:01:05 $

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

 Last Modified $Date: 2007/03/25 06:01:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: np_ip.c,v $
 Revision 0.9.2.38  2007/03/25 06:01:05  brian
 - flush corrections

 Revision 0.9.2.37  2007/03/25 00:53:48  brian
 - synchronization updates

 Revision 0.9.2.36  2007/02/10 15:53:18  brian
 - PR: openss7/4734 fixed missing spinlock symbols on ubuntu i386 UP kernels

 Revision 0.9.2.35  2006/12/08 05:18:48  brian
 - minor updates and corrections

 Revision 0.9.2.34  2006/10/31 21:00:01  brian
 - optimization correction

 Revision 0.9.2.33  2006/10/12 10:28:38  brian
 - removed redundant debug flags

 Revision 0.9.2.32  2006/10/02 11:32:32  brian
 - changes to get master builds working for RPM and DEB
 - added outside licenses to package documentation
 - added LICENSE automated release file
 - copy MANUAL to source directory
 - add and remove devices in -dev debian subpackages
 - get debian rules working better
 - release library version files
 - added notes to debian changelog
 - corrections for cooked manual pages in spec files
 - added release documentation to spec and rules files
 - copyright header updates
 - moved controlling tty checks in stream head
 - missing some defines for LiS build in various source files
 - added OSI headers to striso package
 - added includes and manual page paths to acincludes for various packages
 - added sunrpc, uidlpi, uinpi and uitpi licenses to documentation and release
   files
 - moved pragma weak statements ahead of declarations
 - changes for master build of RPMS and DEBS with LiS

 Revision 0.9.2.31  2006/08/16 07:47:40  brian
 - removed locking macro pollution

 Revision 0.9.2.30  2006/07/25 06:39:24  brian
 - expanded minor device numbers and optimization and locking corrections

 Revision 0.9.2.29  2006/07/24 09:01:57  brian
 - results of udp2 optimizations

 Revision 0.9.2.28  2006/07/16 12:46:52  brian
 - handle skb_linearize with 1 arg on recent kernels

 Revision 0.9.2.27  2006/07/16 08:19:30  brian
 - noxious 2.4 bug fixed, test suite corrections

 Revision 0.9.2.26  2006/07/15 13:06:29  brian
 - rationalized np_ip.c and rawip.c to upd.c drivers

 Revision 0.9.2.25  2006/07/15 05:40:39  brian
 - converted 2.4 inet protocol hooks to 2.6 style

 Revision 0.9.2.24  2006/07/08 09:37:56  brian
 - handle old SLES 9 2.6.5 kernel (untested)

 Revision 0.9.2.23  2006/07/07 21:01:08  brian
 - change to correct LIS-only compile on FC5

 Revision 0.9.2.22  2006/07/02 12:22:16  brian
 - changes for 2.6.17 kernel

 Revision 0.9.2.21  2006/06/16 08:02:01  brian
 - added constness, rationalized to udp

 Revision 0.9.2.20  2006/06/05 02:53:42  brian
 - working up udp zero-copy

 Revision 0.9.2.19  2006/05/18 11:22:48  brian
 - rationalized to RAWIP driver

 Revision 0.9.2.18  2006/05/14 06:34:31  brian
 - corrected buffer leaks

 Revision 0.9.2.17  2006/05/13 14:00:38  brian
 - rationalized to rawip driver

 Revision 0.9.2.16  2006/05/13 02:43:34  brian
 - corrections for 2.4 testing

 Revision 0.9.2.15  2006/05/12 23:54:40  brian
 - close to last changes from testing

 Revision 0.9.2.14  2006/05/12 09:58:00  brian
 - more testing results and corrections for NPI-IP driver

 Revision 0.9.2.13  2006/05/12 01:16:20  brian
 - more results from testing NPI-IP driver

 Revision 0.9.2.12  2006/05/11 22:16:46  brian
 - more testing of NPI-IP driver

 Revision 0.9.2.11  2006/05/11 10:59:36  brian
 - more testing of NPI-IP driver

 Revision 0.9.2.10  2006/05/10 20:56:30  brian
 - more testing

 Revision 0.9.2.9  2006/05/10 09:42:42  brian
 - more testing on NPI-IP driver

 Revision 0.9.2.8  2006/05/09 22:13:04  brian
 - changes from testing

 Revision 0.9.2.7  2006/05/09 09:47:56  brian
 - changes from initial testing

 Revision 0.9.2.6  2006/05/09 06:48:08  brian
 - changes from testing

 Revision 0.9.2.5  2006/05/08 11:26:13  brian
 - post inc problem and working through test cases

 Revision 0.9.2.4  2006/05/08 08:16:43  brian
 - module_text_address, hash alloc changes

 Revision 0.9.2.3  2006/05/07 22:12:57  brian
 - updated for NPI-IP driver

 Revision 0.9.2.2  2006/04/18 18:00:45  brian
 - working up DL and NP drivers

 Revision 0.9.2.1  2006/04/13 18:32:50  brian
 - working up DL and NP drivers.

 *****************************************************************************/

#ident "@(#) $RCSfile: np_ip.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/03/25 06:01:05 $"

static char const ident[] =
    "$RCSfile: np_ip.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/03/25 06:01:05 $";

/*
   This driver provides the functionality of an IP (Internet Protocol) hook similar to raw sockets,
   with the exception that the hook acts as a port bound intercept for IP packets for the bound
   protocol ids.  This dirver is used primarily by OpenSS7 protocol test module (e.g. for SCTP) and
   for applications where entire ranges of port numbers for an existing protocol id must be
   intercepted (e.g. for RTP/RTCP).  This driver uses  hook into the Linux IP protocol tables and
   passes packets tranparently on to the underlying protocol in which it is not interested (bound).
   The driver uses the NPI (Network Provider Interface) API.
*/

// #define _DEBUG 1

#define _SVR4_SOURCE
#define _LFS_SOURCE

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define np_tst_bit(nr,addr)	    test_bit(nr,addr)
#define np_set_bit(nr,addr)	    __set_bit(nr,addr)
#define np_clr_bit(nr,addr)	    __clear_bit(nr,addr)

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

#define NP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define NP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define NP_REVISION	"OpenSS7 $RCSfile: np_ip.c,v $ $Name:  $ ($Revision: 0.9.2.38 $) $Date: 2007/03/25 06:01:05 $"
#define NP_DEVICE	"SVR 4.2 STREAMS NPI NP_IP Data Link Provider"
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
MODULE_ALIAS("streams-np_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define NP_IP_DRV_ID	CONFIG_STREAMS_NP_IP_MODID
#define NP_IP_DRV_NAME	CONFIG_STREAMS_NP_IP_NAME
#define NP_IP_CMAJORS	CONFIG_STREAMS_NP_IP_NMAJORS
#define NP_IP_CMAJOR_0	CONFIG_STREAMS_NP_IP_MAJOR
#define NP_IP_UNITS	CONFIG_STREAMS_NP_IP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NP_IP_MODID));
MODULE_ALIAS("streams-driver-np_ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NP_IP_MAJOR));
MODULE_ALIAS("/dev/streams/np_ip");
MODULE_ALIAS("/dev/streams/np_ip/*");
MODULE_ALIAS("/dev/streams/clone/np_ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/np_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  ==========================================================================
 *
 *  STREAMS Definitions
 *
 *  ==========================================================================
 */

#define DRV_ID		NP_IP_DRV_ID
#define DRV_NAME	NP_IP_DRV_NAME
#define CMAJORS		NP_IP_CMAJORS
#define CMAJOR_0	NP_IP_CMAJOR_0
#define UNITS		NP_IP_UNITS
#ifdef MODULE
#define DRV_BANNER	NP_BANNER
#else				/* MODULE */
#define DRV_BANNER	NP_SPLASH
#endif				/* MODULE */

#ifndef noinline
#define noinline
#endif
#ifndef fastcall
#define fastcall
#endif
#ifndef __unlikely
#define __unlikely
#endif

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = (1 << 19),		/* Hi water mark */
	.mi_lowat = (1 << 17),		/* Lo water mark */
};

STATIC struct module_stat np_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat np_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

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

STATIC INLINE streams_fastcall __hot_in int
np_bhashfn(unsigned char proto, unsigned short bport)
{
	return ((np_bhash_size - 1) & (proto + bport));
}

STATIC INLINE streams_fastcall __unlikely int
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

STATIC kmem_cache_t *np_ip_prot_cachep;
STATIC kmem_cache_t *np_ip_priv_cachep;

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
		kmem_cache_free(np_ip_priv_cachep, np);
	}
}
static INLINE streams_fastcall __hot void
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

	if ((np = kmem_cache_alloc(np_ip_priv_cachep, SLAB_ATOMIC))) {
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
 *  Locking
 */

/* Must always be bottom-half versions to avoid lock badness.  But give these
 * different names to avoid conflict with generic definitions.  */

//#if defined CONFIG_STREAMS_NOIRQ || defined _TEST
#if 1

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

STATIC noinline fastcall __unlikely void
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

STATIC noinline fastcall __unlikely void
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
#endif				/* _DEBUG */

/* State functions */

STATIC INLINE streams_fastcall __unlikely void
np_set_state(struct np *np, const np_ulong state)
{
	_printd(("%s: %p: %s <- %s\n", DRV_NAME, np, state_name(state),
		 state_name(np->info.CURRENT_state)));
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
STATIC INLINE streams_fastcall __hot_in int
np_v4_rcv_next(struct sk_buff *skb)
{
	struct np_prot_bucket *pb;
	struct mynet_protocol *pp;
	unsigned char proto;

	proto = skb->nh.iph->protocol;
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
STATIC INLINE streams_fastcall __hot_in void
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
STATIC INLINE streams_fastcall __unlikely struct np_prot_bucket *
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
	} else if ((pb = kmem_cache_alloc(np_ip_prot_cachep, SLAB_ATOMIC))) {
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
		pp->proto.name = "streams-np_ip";
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
					kmem_cache_free(np_ip_prot_cachep, pb);
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
						kmem_cache_free(np_ip_prot_cachep, pb);
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
STATIC INLINE streams_fastcall __unlikely void
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

			kmem_cache_free(np_ip_prot_cachep, pb);
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
STATIC INLINE streams_fastcall __unlikely int
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
STATIC INLINE streams_fastcall __unlikely void
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
 */
STATIC INLINE streams_fastcall __unlikely int
np_bind(struct np *np, unsigned char *PROTOID_buffer, size_t PROTOID_length,
	struct sockaddr_in *ADDR_buffer, const socklen_t ADDR_length,
	const np_ulong CONIND_number, const np_ulong BIND_flags)
{
	struct np_bhash_bucket *hp;
	unsigned short bport = ADDR_buffer[0].sin_port;
	unsigned char proto = PROTOID_buffer[0];
	size_t anum = ADDR_length / sizeof(*ADDR_buffer);
	struct np *np2;
	int i, j, err;
	unsigned long flags;

	PROTOID_length = 1;
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
		write_unlock_str2(&hp->lock, flags);
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
	if ((err = np_bind_prot(proto, np->info.SERV_type))) {
		write_unlock_str2(&hp->lock, flags);
		return (err);
	}
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
np_ip_queue_xmit(struct sk_buff *skb)
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
np_ip_queue_xmit(struct sk_buff *skb)
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
STATIC noinline streams_fastcall void
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
#define skbuff_head_cache (*((kmem_cache_t **) HAVE_SKBUFF_HEAD_CACHE_ADDR))
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
STATIC noinline streams_fastcall __unlikely struct sk_buff *
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
					__assert(data <= skb->tail);
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
STATIC noinline streams_fastcall __unlikely struct sk_buff *
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

STATIC INLINE streams_fastcall __hot_out struct sk_buff *
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
STATIC INLINE streams_fastcall __hot_out struct sk_buff *
np_alloc_skb(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	return np_alloc_skb_slow(np, mp, headroom, gfp);
}
#endif				/* !defined LFS */

STATIC noinline streams_fastcall int
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

STATIC INLINE streams_fastcall __hot_out int
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
 * @daddr: destination address
 * @mp: message payload
 */
STATIC INLINE streams_fastcall __hot_out int
np_senddata(struct np *np, uint8_t protocol, uint32_t daddr, mblk_t *mp)
{
	struct rtable *rt;
	int err;

	rt = (struct rtable *) np->daddrs[0].dst;
	prefetch(rt);

	if (likely((err = np_route_output(np, daddr, &rt)) == 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = ((dev->hard_header_len + 15) & ~15)
		    + sizeof(struct iphdr) + 0;
		size_t dlen = msgsize(mp);
		size_t plen = dlen + 0;
		size_t tlen = plen + sizeof(struct iphdr);

		_ptrace(("%s: %s: sending data message block %p\n", DRV_NAME, __FUNCTION__, mp));
		usual(hlen > sizeof(struct iphdr) + 0);
		usual(dlen);

		if (likely((skb = np_alloc_skb(np, mp, hlen, GFP_ATOMIC)) != NULL)) {
			struct iphdr *iph;
			uint32_t saddr = np->qos.saddr ? np->qos.saddr : rt->rt_src;
			uint32_t daddr = rt->rt_dst;

			/* find headers */

			skb->nh.raw = __skb_push(skb, sizeof(struct iphdr));

			skb->dst = &rt->u.dst;
			skb->priority = 0;	// np->qos.priority;

			iph = skb->nh.iph;
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
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, np_ip_queue_xmit);
#else
			np_ip_queue_xmit(skb);
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
STATIC INLINE streams_fastcall int
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
STATIC streams_fastcall int
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
 * Destination addresses and port number as well as connection request quality of service parameters
 * should already be stored into the private structure.  Yes, this information will remain if there
 * is an error in the connection request.  When any primitive containing options fails and returns
 * and error, it is the caller's responsibility to set again the values of the options.
 */
STATIC streams_fastcall int
np_connect(struct np *np, const struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	   struct N_qos_sel_conn_ip *QOS_buffer, const np_ulong CONN_flags)
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
np_optmgmt(struct np *np, union N_qos_ip_types *QOS_buffer, np_ulong OPTMGMT_flags)
{
	int i;

	switch (QOS_buffer->n_qos_type) {
	case N_QOS_SEL_INFO_IP:
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
	case N_QOS_RANGE_INFO_IP:
		return (NBADQOSTYPE);
	case N_QOS_SEL_CONN_IP:
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
	case N_QOS_SEL_UD_IP:
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
STATIC noinline streams_fastcall int
np_passive(struct np *np, struct sockaddr_in *RES_buffer, const socklen_t RES_length,
	   struct N_qos_sel_conn_ip *QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value,
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
		if (unlikely((err = np_senddata(np, np->qos.protocol, np->qos.daddr, dp)) != QR_ABSORBED))
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

/**
 * np_disconnect - disconnect a network connection
 * @np: private structure
 * @RES_buffer: responding address (unused)
 * @SEQ_number: connection indication being refused
 * @DISCON_reason: disconnect reason (unused)
 * @dp: user disconnect data
 */
STATIC int
np_disconnect(struct np *np, struct sockaddr_in *RES_buffer, mblk_t *SEQ_number,
	      const np_ulong DISCON_reason, mblk_t *dp)
{
	struct np_chash_bucket *hp;
	int err;
	unsigned long flags;

	if (dp != NULL) {
		err = np_senddata(np, np->qos.protocol, np->qos.daddr, dp);
		if (unlikely(err != QR_ABSORBED))
			goto error;
	}
	if (SEQ_number != NULL) {
		bufq_unlink(&np->conq, SEQ_number);
		freemsg(SEQ_number);
	}
	if ((hp = np->chash) != NULL) {
		write_lock_str2(&hp->lock, flags);
		if ((*np->cprev = np->cnext))
			np->cnext->cprev = np->cprev;
		np->cnext = NULL;
		np->cprev = &np->cnext;
		np->chash = NULL;
		np->dport = np->sport = 0;
		np->dnum = np->snum = 0;
		np_set_state(np, NS_IDLE);
		np_put(np);
		write_unlock_str2(&hp->lock, flags);
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
 *
 *  - Before bind, the provider will report both N_CLNS and N_CONS.  When bound with bind flags
 *    equal to N_CLNS in the second least significant BIND_flags byte, the provider will be
 *    connectionless.  When bound with bind flags equal to N_CONS in the second least significant
 *    BIND_flags byte, the provider will be connection-oriented.
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
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	if (unlikely((mp = np_allocb(q, 2, BPRI_HI)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = how;
	*mp->b_wptr++ = band;
	putnext(np->oq, mp);
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
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = np_allocb(q, 2, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_ERROR;
		mp->b_wptr[0] = mp->b_wptr[1] = error;
		mp->b_wptr += 2;
		/* make sure the stream is disconnected */
		np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the stream is unbound */
		np_unbind(np);
		_printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, np, error));
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
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = np_allocb(q, 0, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_HANGUP;
		/* make sure the stream is disconnected */
		np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the stream is unbound */
		np_unbind(np);
		_printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, np));
		qreply(q, mp);
		return (QR_DONE);

	}
	return (-ENOBUFS);
}

/**
 * ne_error_reply - reply to a message with an M_ERROR message
 * @q: active queue in queue pair (write queue)
 * @error: error number
 */
STATIC noinline streams_fastcall __unlikely int
ne_error_reply(queue_t *q, const long error)
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
 * ne_info_ack - generate an N_INFO_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 */
STATIC noinline streams_fastcall int
ne_info_ack(queue_t *q)
{
	struct np *np = NP_PRIV(q);
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

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = np->info.NSDU_size;
	p->ENSDU_size = np->info.ENSDU_size;
	p->CDATA_size = np->info.CDATA_size;
	p->DDATA_size = np->info.DDATA_size;
	p->ADDR_size = np->info.ADDR_size;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + ADDR_length : 0;
	p->QOS_range_length = QOS_range_length;
	p->QOS_range_offset = QOS_range_length ? sizeof(*p) + ADDR_length + QOS_length : 0;
	p->OPTIONS_flags = np->info.OPTIONS_flags;
	p->NIDU_size = np->info.NIDU_size;
	p->SERV_type = np->info.SERV_type ? np->info.SERV_type : (N_CONS | N_CLNS);
	p->CURRENT_state = np_get_state(np);
	p->PROVIDER_type = np->info.PROVIDER_type;
	p->NODU_size = 536;
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset =
	    PROTOID_length ? sizeof(*p) + ADDR_length + QOS_length + QOS_range_length : 0;
	p->NPI_version = np->info.NPI_version;
	mp->b_wptr += sizeof(*p);
	if (ADDR_length) {
		int i;

		for (i = 0; i < np->snum; i++) {
			ADDR_buffer = (struct sockaddr_in *) mp->b_wptr;
			ADDR_buffer->sin_family = AF_INET;
			ADDR_buffer->sin_port = np->sport;
			ADDR_buffer->sin_addr.s_addr = np->saddrs[i].addr;
			mp->b_wptr += sizeof(struct sockaddr_in);
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
	_printd(("%s: %p: <- N_INFO_ACK\n", DRV_NAME, np));
	qreply(q, mp);
	return (QR_DONE);

      enobufs:
	return (-ENOBUFS);
}

/**
 * ne_bind_ack - NE_BIND_ACK event
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
STATIC noinline streams_fastcall int
ne_bind_ack(queue_t *q, unsigned char *PROTOID_buffer, size_t PROTOID_length,
	    struct sockaddr_in *ADDR_buffer, socklen_t ADDR_length, np_ulong CONIND_number,
	    np_ulong BIND_flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_bind_ack_t *p;
	size_t size = sizeof(*p) + ADDR_length + PROTOID_length;
	int err;

	err = -EFAULT;
	if (unlikely(np_get_state(np) != NS_WACK_BREQ))
		goto error;

	err = -ENOBUFS;
	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto error;

	err = np_bind(np, PROTOID_buffer, PROTOID_length, ADDR_buffer, ADDR_length,
		      CONIND_number, BIND_flags);
	if (unlikely(err != 0)) {
		freeb(mp);
		goto error;
	}

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = CONIND_number;
	p->TOKEN_value = (BIND_flags & TOKEN_REQUEST) ? (long) RD(q) : 0;
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset = PROTOID_length ? sizeof(*p) + ADDR_length : 0;
	mp->b_wptr += sizeof(*p);
	if (ADDR_length) {
		bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
		mp->b_wptr += ADDR_length;
	}
	if (PROTOID_length) {
		bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_length);
		mp->b_wptr += PROTOID_length;
	}
	/* all ready, complete the bind */
	np_set_state(np, NS_IDLE);
	_printd(("%s: %p: <- N_BIND_ACK\n", DRV_NAME, np));
	qreply(q, mp);
	return (QR_DONE);

      error:
	return (err);
}

/**
 * ne_error_ack - generate an N_ERROR_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @ERROR_prim: primitive in error
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
STATIC noinline streams_fastcall __unlikely int
ne_error_ack(queue_t *q, np_ulong ERROR_prim, np_long error)
{
	struct np *np = NP_PRIV(q);
	N_error_ack_t *p;
	mblk_t *mp;
	int err;

	/* rollback state */
	np_set_state(np, np->i_oldstate);
	np->i_oldstate = np_get_state(np);

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
		error = NBOUND;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		error = NNOADDR;
		break;
	case -EMSGSIZE:
		error = NBADDATA;
		break;
	case -EACCES:
		error = NACCESS;
		break;
	case -EOPNOTSUPP:
		error = NNOTSUPPORT;
		break;
	}
	err = -ENOBUFS;
	if ((mp = np_allocb(q, sizeof(*p), BPRI_MED)) == NULL)
		goto error;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = ERROR_prim;
	p->NPI_error = (error < 0) ? NSYSERR : error;
	p->UNIX_error = (error < 0) ? -error : 0;
	mp->b_wptr += sizeof(*p);
	_printd(("%s: %p: <- N_ERROR_ACK\n", DRV_NAME, np));
	qreply(q, mp);
	return (0);
      error:
	return (err);
}

/**
 * ne_ok_ack - generate an N_OK_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @CORRECT_prim: correct primitive
 * @ADDR_buffer: destination or responding address
 * @ADDR_length: length of destination or responding addresses
 * @QOS_buffer: quality of service parameters
 * @SEQ_number: sequence number (i.e. connection/reset indication sequence number)
 * @TOKEN_value: token (i.e. connection response token)
 * @flags: mangement flags, connection flags, disconnect reason, etc.
 * @dp: user data
 */
STATIC int
ne_ok_ack(queue_t *q, np_ulong CORRECT_prim, struct sockaddr_in *ADDR_buffer, socklen_t ADDR_length,
	  void *QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value, np_ulong flags, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	N_ok_ack_t *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);
	int err = QR_DONE;
	pl_t pl;

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	mp->b_wptr += sizeof(*p);
	switch (np_get_state(np)) {
#if 1
	case NS_WACK_OPTREQ:
		err = np_optmgmt(np, QOS_buffer, flags);
		if (unlikely(err != 0))
			goto free_error;
		pl = bufq_lock(&np->conq);
		np_set_state(np, bufq_length(&np->conq) > 0 ? NS_WRES_CIND : NS_IDLE);
		bufq_unlock(&np->conq, pl);
		break;
#endif
	case NS_WACK_UREQ:
		err = np_unbind(np);
		if (unlikely(err != 0))
			goto free_error;
		/* NPI spec says that if the provider must flush both queues before responding with 
		   a N_OK_ACK primitive when responding to a N_UNBIND_REQ. This is to flush queued
		   data for connectionless providers. */
		err = m_flush(q, FLUSHRW, 0);
		if (unlikely(err != 0))
			goto free_error;
		np_set_state(np, NS_UNBND);
		break;
#if 0
	case NS_WCON_CREQ:
		break;
#endif
	case NS_WACK_CRES:
		/* FIXME: needs to hold reference to and lock the accepting stream */
		if (np != TOKEN_value)
			TOKEN_value->i_oldstate = np_get_state(TOKEN_value);
		np_set_state(TOKEN_value, NS_DATA_XFER);
		err = np_passive(np, ADDR_buffer, ADDR_length, QOS_buffer, SEQ_number,
				 TOKEN_value, flags, dp);
		if (unlikely(err != QR_ABSORBED)) {
			np_set_state(TOKEN_value, TOKEN_value->i_oldstate);
			goto free_error;
		}
		if (np != TOKEN_value) {
			pl = bufq_lock(&np->conq);
			np_set_state(np, bufq_length(&np->conq) > 0 ? NS_WRES_CIND : NS_IDLE);
			bufq_unlock(&np->conq, pl);
		}
		break;
#if 1
	case NS_WACK_RRES:
		err = np_reset_rem(np, N_USER, N_REASON_UNDEFINED);
		if (unlikely(err != 0))
			goto free_error;
		pl = bufq_lock(&np->resq);
		np_set_state(np, bufq_length(&np->resq) > 0 ? NS_WRES_RIND : NS_DATA_XFER);
		bufq_unlock(&np->resq, pl);
		break;
#endif
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		err = np_disconnect(np, ADDR_buffer, SEQ_number, flags, dp);
		if (unlikely(err != QR_ABSORBED))
			goto free_error;
		pl = bufq_lock(&np->conq);
		np_set_state(np, bufq_length(&np->conq) > 0 ? NS_WRES_CIND : NS_IDLE);
		bufq_unlock(&np->conq, pl);
		break;
	default:
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a N_OPTMGMT_REQ in other than the NS_IDLE
		   state. */
#if 1
		if (CORRECT_prim == N_OPTMGMT_REQ) {
			err = np_optmgmt(np, QOS_buffer, flags);
			if (unlikely(err != 0))
				goto free_error;
			break;
		}
#endif
		break;
	}
	_printd(("%s: %p: <- N_OK_ACK\n", DRV_NAME, np));
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
 * a TS_WACK_CREQ state, only a NS_WCON_CREQ state.  This makes NPI more ammenable to this kind of
 * pseudo-connection-oriented mode.
 *
 * Originally, this primitive returned one N_QOS_SEL_CONN_IP structure for each destination.  This
 * is no longer the case.  Only one N_QOS_SEL_CONN_IP structure is returned representing the values
 * for the entire association.  ttl is the maximum ttl of the destinations.  mtu is the smallest
 * value for the destinations.  These values are set in the private structure by the np_connect()
 * function.
 */
STATIC INLINE streams_fastcall int
ne_conn_con(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	    struct N_qos_sel_conn_ip *QOS_buffer, np_ulong CONN_flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_conn_con_t *p;
	int err;
	size_t QOS_length = sizeof(*QOS_buffer);
	size_t size = sizeof(*p) + RES_length + QOS_length;

	np_set_state(np, NS_WCON_CREQ);

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	err = np_connect(np, RES_buffer, RES_length, QOS_buffer, CONN_flags);
	if (unlikely(err != 0))
		goto free_error;

	np_set_state(np, NS_DATA_XFER);

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->CONN_flags = CONN_flags;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + RES_length : 0;
	mp->b_wptr += sizeof(*p);
	if (RES_length) {
		bcopy(RES_buffer, mp->b_wptr, RES_length);
		mp->b_wptr += RES_length;
	}
	if (QOS_length) {
		bcopy(QOS_buffer, mp->b_wptr, QOS_length);
		mp->b_wptr += QOS_length;
	}

	_printd(("%s: %p: <- N_CONN_CON\n", DRV_NAME, np));
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

#if 1
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
ne_reset_con(queue_t *q, np_ulong RESET_orig, np_ulong RESET_reason, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_reset_con_t *p;
	size_t size = sizeof(*p);
	int err;
	pl_t pl;

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely((err = np_reset_loc(np, RESET_orig, RESET_reason, dp)) != 0))
		goto free_error;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	pl = bufq_lock(&np->resq);
	np_set_state(np, bufq_length(&np->resq) > 0 ? NS_WRES_RIND : NS_DATA_XFER);
	bufq_unlock(&np->resq, pl);
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
 * ne_conn_ind - generate a N_CONN_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * We generate connection indications to Streams that are bound as listening to an address including
 * the destination address of the IP packet, where no connection exists for the source address of
 * the IP packet.
 */
STATIC INLINE streams_fastcall __hot_get int
ne_conn_ind(queue_t *q, mblk_t *SEQ_number)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp, *cp;
	N_conn_ind_t *p;
	struct sockaddr_in *DEST_buffer, *SRC_buffer;
	struct N_qos_sel_conn_ip *QOS_buffer;
	np_ulong DEST_length, SRC_length, QOS_length;
	size_t size;
	struct iphdr *iph = (struct iphdr *) SEQ_number->b_rptr;
	struct udphdr *uh = (struct udphdr *) (SEQ_number->b_rptr + (iph->ihl << 2));
	unsigned long flags;

	assure(SEQ_number->b_wptr >= SEQ_number->b_rptr + sizeof(*iph));
	assure(SEQ_number->b_wptr >= SEQ_number->b_rptr + (iph->ihl << 2));

	if (unlikely(np_not_state(np, (NSF_IDLE | NSF_WRES_CIND | NSF_WACK_CRES))))
		goto discard;

	/* Make sure we don't already have a connection indication */
	spin_lock_str2(&np->conq.q_lock, flags);
	for (cp = bufq_head(&np->conq); cp; cp = cp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) cp->b_rptr;

		if (iph->protocol == iph2->protocol
		    && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr) {
			/* already have a connection indication, link the data */
			linkb(cp, SEQ_number);
			spin_unlock_str2(&np->conq.q_lock, flags);
			goto absorbed;
		}
	}
	spin_unlock_str2(&np->conq.q_lock, flags);

	if (unlikely(bufq_length(&np->conq) >= np->CONIND_number))
		/* If there are already too many connection indications outstanding, discard
		   further connection indications until some are accepted -- we might get fancy
		   later and queue it anyway.  Note that data for existing outstanding connection
		   indications is preserved above. */
		goto eagain;
	if (unlikely(np_not_state(np, (NSF_IDLE | NSF_WRES_CIND))))
		/* If there is already a connection accepted on the listening stream, discard
		   further connection indications until the current connection disconnects */
		goto eagain;

	np_set_state(np, NS_WRES_CIND);

	if (unlikely((cp = np_dupmsg(q, SEQ_number)) == NULL))
		goto enobufs;

	DEST_length = sizeof(*DEST_buffer);
	SRC_length = sizeof(*SRC_buffer);
	QOS_length = sizeof(*QOS_buffer);
	size = sizeof(*p) + DEST_length + SRC_length + QOS_length;

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto free_enobufs;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
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
		DEST_buffer = (struct sockaddr_in *) mp->b_wptr;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = (np->bport != 0) ? uh->dest : 0;
		DEST_buffer->sin_addr.s_addr = iph->daddr;
		mp->b_wptr += sizeof(struct sockaddr_in);
	}
	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = (np->bport != 0) ? uh->source : 0;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += sizeof(struct sockaddr_in);
	}
	if (QOS_length) {
		QOS_buffer = (struct N_qos_sel_conn_ip *) mp->b_wptr;
		QOS_buffer->n_qos_type = N_QOS_SEL_CONN_IP;
		/* FIXME: might be a problem here on 2.4 where we steal the packet by overwritting
		   the protocol id. */
		QOS_buffer->protocol = iph->protocol;
		QOS_buffer->priority = cp->b_band;
		QOS_buffer->ttl = iph->ttl;
		QOS_buffer->tos = iph->tos;
		QOS_buffer->mtu = QOS_UNKNOWN;	/* FIXME: determine route and get mtu from it */
		QOS_buffer->daddr = iph->daddr;
		QOS_buffer->saddr = iph->saddr;
		mp->b_wptr += QOS_length;
	}
	/* should we pull the IP header? */
	mp->b_cont = cp;
	/* sure, all the info is in the qos structure and addresses */
	cp->b_rptr += (iph->ihl << 2);
	/* save original in connection indication list */
	bufq_queue(&np->conq, SEQ_number);
	_printd(("%s: %p: <- N_CONN_IND\n", DRV_NAME, np));
	putnext(q, mp);
      absorbed:
	return (QR_ABSORBED);

      ebusy:
	freeb(cp);
	freeb(mp);
	return (-EBUSY);
      free_enobufs:
	freemsg(cp);
      enobufs:
	return (-ENOBUFS);
      eagain:
	return (-EAGAIN);
      discard:
	return (QR_DONE);
}

/**
 * ne_discon_ind - NE_DISCON_IND NC disconnected event
 * @q: active queue (read queue)
 * @dp: message containing ICMP packet
 *
 * The N_DISCON_IND is sent when we encounter an error on a connection oriented Stream, i.e. as a
 * result of receiving an ICMP error.  For multihomed hosts, we only do this if all destination
 * addresses have errors, otherwise, we just perform a reset for the affected destination.
 */
STATIC INLINE streams_fastcall int
ne_discon_ind(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	      np_ulong RESERVED_field, np_ulong DISCON_orig, np_ulong DISCON_reason,
	      mblk_t *SEQ_number, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_discon_ind_t *p;
	size_t size = sizeof(*p) + RES_length;

	if (unlikely
	    (np_not_state(np, (NSF_WRES_CIND | NSF_DATA_XFER | NSF_WRES_RIND | NSF_WCON_RREQ))))
		goto discard;

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = DISCON_orig;
	p->DISCON_reason = DISCON_reason;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	mp->b_wptr += sizeof(*p);
	if (RES_length) {
		bcopy(RES_buffer, mp->b_wptr, RES_length);
		mp->b_wptr += RES_length;
	}
	mp->b_cont = dp;
	_printd(("%s: %p: <- N_DISCON_IND\n", DRV_NAME, np));
	putnext(q, mp);
	return (QR_ABSORBED);

      enobufs:
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * ne_discon_ind_icmp - NE_DISCON_IND event resulting from ICMP message
 * @q: active queue in queue pair
 * @mp: the ICMP message
 */
STATIC INLINE streams_fastcall int
ne_discon_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in res_buf, *RES_buffer = &res_buf;
	np_ulong DISCON_reason;
	np_ulong RESERVED_field, DISCON_orig;
	mblk_t *rp;
	mblk_t *cp, *SEQ_number;
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
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_NSAP_UNREACH_P;	// N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_FRAG_NEEDED:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_SEG_REQUIRED;
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
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_NSAP_UNKNOWN;	// N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_QOS_UNAVAIL;
			RESERVED_field = 0;
			break;
		default:
			DISCON_orig = N_UNDEFINED;
			DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		/* Should not cause disconnect. */
		DISCON_orig = N_PROVIDER;
		DISCON_reason = N_CONGESTION;	// N_UD_CONGESTION;
		RESERVED_field = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_LIFE_EXCEEDED;
			RESERVED_field = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_TD_EXCEEDED;
			RESERVED_field = 0;
			break;
		default:
			DISCON_orig = N_UNDEFINED;
			DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		DISCON_orig = N_UNDEFINED;
		DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	default:
		DISCON_orig = N_UNDEFINED;
		DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	}

	/* check for outstanding reset indications for responding address */
	for (rp = bufq_head(&np->resq); rp; rp = rp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) rp->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}

	/* check for outstanding connection indications for responding address */
	for (cp = bufq_head(&np->conq); cp; cp = cp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) cp->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && iph->daddr == iph2->daddr)
			break;
	}
	SEQ_number = cp;

	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = ne_discon_ind(q, RES_buffer, sizeof(*RES_buffer), RESERVED_field, DISCON_orig,
				 DISCON_reason, SEQ_number, mp)) < 0)
		mp->b_rptr -= hidden;
	else {
		if (cp != NULL) {
			bufq_unlink(&np->conq, cp);
			freemsg(cp);
		}
		if (rp != NULL) {
			bufq_unlink(&np->conq, rp);
			freemsg(rp);
		}
	}
	return (err);
}

#if 1
/**
 * ne_data_ind - generate a N_DATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Very fast.  In fact, we could just pass the raw M_DATA blocks upstream.  We leave the IP header
 * in the block.
 */
STATIC INLINE streams_fastcall __hot_get int
ne_data_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	const size_t size = sizeof(*p);

	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	/* TODO: here we can set some info like ECN... */
	p->DATA_xfer_flags = 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- N_DATA_IND\n", DRV_NAME, NP_PRIV(q)));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
#endif

#if 1
/**
 * ne_exdata_ind - generate a N_EXDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC INLINE streams_fastcall __hot_get int
ne_exdata_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;

	if (unlikely((mp = np_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!bcanputnext(q, 1)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- N_EXDATA_IND\n", DRV_NAME, NP_PRIV(q)));
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
 * ne_unitdata_ind - generate a N_UNITDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Note that on unit data indications the entire IP packet is delivered in the data part of
 * the message.  If the NS user is willing to extract information from the IP header, it is
 * welcome to discard the control part.
 */
STATIC INLINE streams_fastcall __hot_get int
ne_unitdata_ind(queue_t *q, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_unitdata_ind_t *p;
	struct sockaddr_in *SRC_buffer, *DEST_buffer;
	const np_ulong SRC_length = sizeof(*SRC_buffer);
	const np_ulong DEST_length = sizeof(*DEST_buffer);
	size_t size = sizeof(*p) + SRC_length + DEST_length;
	struct iphdr *iph;
	struct udphdr *uh;

	if (unlikely(np_get_state(np) != NS_IDLE))
		goto discard;
	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) + SRC_length : 0;
	p->ERROR_type = 0;
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
	if (DEST_length) {
		DEST_buffer = (struct sockaddr_in *) mp->b_wptr;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = uh->dest;
		DEST_buffer->sin_addr.s_addr = iph->daddr;
		mp->b_wptr += DEST_length;
	}
	/* pull IP header */
	dp->b_rptr = (unsigned char *) uh;
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- N_UNITDATA_IND\n", DRV_NAME, np));
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

#if 0
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
	if (unlikely((mp = tp_allocb(q, sizeof(*p) + OPT_length, BPRI_MED)) == NULL))
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
 * ne_uderror_ind - generate a N_UDERROR_IND message
 * @q: active queue in queue pair (read or write queue)
 * @DEST_buffer: pointer to destination address
 * @RESERVED_field: MTU if error is N_UD_SEG_REQUIRED
 * @ERROR_type: error number
 * @dp: message containing (part of) errored packet
 *
 * Note that in the special case of N_UD_SEG_REQUIRED, we use the RESERVED_field to indicate that
 * the value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky
 * trick, because the field must be coded zero according to spec, so the presence of a non-zero
 * value indicates that the MTU value is present for supporting providers.  Users that wish to
 * ignore this field must use N_INFO_REQ and examine the NSDU_size parameter to determine the
 * current value of the MTU.
 */
STATIC noinline __unlikely int
ne_uderror_ind(queue_t *q, struct sockaddr_in *DEST_buffer, np_ulong RESERVED_field,
	       np_ulong ERROR_type, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_uderror_ind_t *p;
	np_ulong DEST_length = sizeof(*DEST_buffer);
	size_t size = sizeof(*p) + (DEST_buffer ? DEST_length : 0);

	if (unlikely(np_get_state(np) != NS_IDLE))
		goto discard;
	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(np->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = DEST_buffer ? DEST_length : 0;
	p->DEST_offset = DEST_buffer ? sizeof(*p) : 0;
	p->RESERVED_field = RESERVED_field;
	p->ERROR_type = ERROR_type;
	mp->b_wptr += sizeof(*p);
	if (DEST_buffer) {
		bcopy(DEST_buffer, mp->b_wptr, DEST_length);
		mp->b_wptr += DEST_length;
	}
	if (dp != NULL) {
		mp->b_cont = dp;
		dp->b_datap->db_type = M_DATA;	/* was M_ERROR in some cases */
	}
	_printd(("%s: %p: <- N_UDERROR_IND\n", DRV_NAME, np));
	putnext(np->oq, mp);
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
 * ne_uderror_ind_icmp - generate an N_UDERROR_IND message from an ICMP packet
 * @q: active queue in pair (read queue)
 * @mp: message containing ICMP packet
 *
 * There is another reason for issuing an N_UDERROR_IND and that is Explicit Congestion
 * Notification, but there is no ICMP message associated with that and it has not yet been coded:
 * probably need an ne_uderror_ind_ecn() function.
 *
 * Note that the special case of N_UD_SEG_REQUIRED, we use the RESERVED_field to indicate that the
 * value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky trick,
 * because the field must be coded zero according to NPI spec, so the presence of a non-zero value
 * indicates the MTU value from a supporting NPI provider.
 */
STATIC noinline streams_fastcall __unlikely int
ne_uderror_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in dst_buf, *DEST_buffer = &dst_buf;
	np_ulong ERROR_type;
	np_ulong RESERVED_field;
	unsigned char *hidden;
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
	hidden = mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = ne_uderror_ind(q, DEST_buffer, RESERVED_field, ERROR_type, mp)) < 0)
		mp->b_rptr = hidden;
	return (err);
}

STATIC noinline streams_fastcall __unlikely int
ne_uderror_reply(queue_t *q, struct sockaddr_in *DEST_buffer, np_ulong RESERVED_field,
		 np_long ERROR_type, mblk_t *db)
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
		ERROR_type = NBOUND;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		ERROR_type = NNOADDR;
		break;
	case -EACCES:
		ERROR_type = NACCESS;
		break;
	case -EOPNOTSUPP:
		ERROR_type = NNOTSUPPORT;
		break;
	case NBADADDR:
	case NBADDATA:
	case NBADOPT:
	case NBADQOSTYPE:
		break;
	default:
	case NOUTSTATE:
	case -EINVAL:
	case -EFAULT:
	case -EMSGSIZE:
		return ne_error_reply(q, -EPROTO);
	}
	return ne_uderror_ind(q, DEST_buffer, RESERVED_field, ERROR_type, db);
}

#if 1
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
	struct np *np = NP_PRIV(q);
	mblk_t *mp, *bp;
	N_reset_ind_t *p;
	const size_t size = sizeof(*p);
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct icmphdr *icmp = (struct icmphdr *) (dp->b_rptr + (iph->ihl << 2));

	assure(dp->b_wptr >= dp->b_rptr + sizeof(*iph));
	assure(dp->b_wptr >= dp->b_rptr + (iph->ihl << 2));

	/* Make sure we don't already have a reset indication */
	for (bp = bufq_head(&np->resq); bp; bp = bp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) bp->b_rptr;
		struct icmphdr *icmp2 = (struct icmphdr *) (bp->b_rptr + (iph2->ihl << 2));

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr
		    && icmp->type == icmp2->type && icmp->code == icmp2->code)
			/* duplicate, just discard it */
			goto discard;
	}

	if (unlikely((bp = np_dupmsg(q, dp)) == NULL))
		goto enobufs;
	if (unlikely((mp = np_allocb(q, size, BPRI_MED)) == NULL))
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
	bufq_queue(&np->resq, dp);
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

#if 0
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

	if (unlikely((mp = tp_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) == NULL))
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

/**
 * ne_datack_ind - NE_DATACK_IND event
 * @q: active queue in pair (read queue)
 */
STATIC INLINE streams_fastcall int
ne_datack_ind(queue_t *q)
{
	mblk_t *mp;
	N_datack_ind_t *p;

	if (unlikely((mp = np_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);

	_printd(("%s: <- N_DATACK_IND\n", DRV_NAME));
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
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 */
/*
 *  NS User -> NS Provider (Request and Response) primitives
 *  ========================================================
 */
/**
 * ne_info_req - NE_INFO_REQ information request event
 * @q: active queue in pair (write queue)
 * @mp: N_INFO_REQ message
 */
STATIC noinline int
ne_info_req(queue_t *q, mblk_t *mp)
{
	N_info_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_INFO_REQ);
	return ne_info_ack(q);
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
 * ne_bind_req - NE_BIND_REQ bind a NS user to network address event
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
STATIC noinline int
ne_bind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_bind_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_BIND_REQ);
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSF_UNBND)))
		goto error;
	np_set_state(np, NS_WACK_BREQ);
	err = NBADFLAG;
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
	err = NBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto error;
	err = NNOPROTOID;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length))
		goto error;
	if (unlikely(p->PROTOID_length == 0))
		goto error;
	{
		struct sockaddr_in ADDR_buffer[8];
		size_t ADDR_length = p->ADDR_length;
		unsigned char *PROTOID_buffer =
		    (typeof(PROTOID_buffer)) (mp->b_rptr + p->PROTOID_offset);

		if (ADDR_length) {
			err = NBADADDR;
			if (unlikely(ADDR_length < sizeof(struct sockaddr_in)))
				goto error;
			if (unlikely(ADDR_length > sizeof(ADDR_buffer)))
				goto error;
			if (unlikely(ADDR_length % sizeof(struct sockaddr_in) != 0))
				goto error;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, ADDR_buffer, ADDR_length);
			if (unlikely(ADDR_buffer[0].sin_family != AF_INET))
				goto error;
		} else {
			ADDR_length = sizeof(struct sockaddr_in);
			ADDR_buffer[0].sin_family = AF_INET;
			ADDR_buffer[0].sin_port = 0;
			ADDR_buffer[0].sin_addr.s_addr = INADDR_ANY;
		}
		/* cannot specify address if default listener */
		if (unlikely((p->BIND_flags & DEFAULT_LISTENER) &&
			     (ADDR_buffer[0].sin_port != 0 ||
			      ADDR_buffer[0].sin_addr.s_addr != INADDR_ANY)))
			goto error;
		/* cannot listen on wildcard port number */
		if (unlikely(p->CONIND_number > 0 && ADDR_buffer[0].sin_port == 0))
			goto error;
		/* cannot specify port if default destination */
		if (unlikely((p->BIND_flags & DEFAULT_DEST) && ADDR_buffer[0].sin_port != 0))
			goto error;
		ptrace(("%s: %s: proto = %d, bport = %d\n", DRV_NAME, __FUNCTION__,
			(int) PROTOID_buffer[0], (int) ntohs(ADDR_buffer[0].sin_port)));
		err = ne_bind_ack(q, PROTOID_buffer, p->PROTOID_length,
				  ADDR_buffer, ADDR_length, p->CONIND_number, p->BIND_flags);
		if (unlikely(err != 0))
			goto error;
		return (QR_DONE);
	}
      error:
	return ne_error_ack(q, N_BIND_REQ, err);
}

/**
 * ne_unbind_req - NE_UNBIND_REQ unbind NS user from network address event
 * @q: active queue in pair (write queue)
 * @mp: N_UNBIND_REQ message
 */
STATIC INLINE streams_fastcall int
ne_unbind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_unbind_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_UNBIND_REQ);
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSF_IDLE)))
		goto error;
	np_set_state(np, NS_WACK_UREQ);
	err = ne_ok_ack(q, N_UNBIND_REQ, NULL, 0, NULL, NULL, NULL, 0, NULL);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);
      error:
	return ne_error_ack(q, N_UNBIND_REQ, err);
}

/**
 * ne_optmgmt_req - NE_OPTMGMT_REQ options management request
 * @q: active queue in pair (write queue)
 * @mp: N_OPTMGMT_REQ message
 */
STATIC noinline int
ne_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_optmgmt_req_t *p;
	union N_qos_ip_types qos_buf, *QOS_buffer = NULL;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_OPTMGMT_REQ);
	if (unlikely(np_chk_state(np, NSF_IDLE)))
		np_set_state(np, NS_WACK_OPTREQ);
	err = NBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
		goto error;
	if (p->QOS_length) {
		err = NBADOPT;
		if (p->QOS_length < sizeof(QOS_buffer->n_qos_type))
			goto error;
		if (p->QOS_length > sizeof(*QOS_buffer))
			goto error;
		err = NBADQOSTYPE;
		QOS_buffer = &qos_buf;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, p->QOS_length);
		switch (QOS_buffer->n_qos_type) {
		case N_QOS_SEL_INFO_IP:
			if (p->QOS_length != sizeof(QOS_buffer->n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_IP:
			if (p->QOS_length != sizeof(QOS_buffer->n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_IP:
			if (p->QOS_length != sizeof(QOS_buffer->n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_IP:
			if (p->QOS_length != sizeof(QOS_buffer->n_qos_range_info))
				goto error;
			break;
		default:
			goto error;
		}
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		np->i_flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		np->i_flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	err = ne_ok_ack(q, N_OPTMGMT_REQ, NULL, 0, QOS_buffer, NULL, NULL, p->OPTMGMT_flags, NULL);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);
      error:
	return ne_error_ack(q, N_OPTMGMT_REQ, err);
}

/**
 * ne_unitdata_req_slow - NE_UNITDATA_REQ unit data request event
 * @q: active queue in pair (write queue)
 * @mp: N_UNITDATA_REQ message
 *
 * The destination address, DEST_length and DEST_offset, must be specified.  The port number is not
 * significant: the attached M_DATA blocks begin with the transport header which will include any
 * port numbers as required.  The IP header information for the transmitted packet can be set with a
 * N_QOS_SEL_UD_IP structure to T_OPTMGMT_REQ and will be taken from the Stream private structure
 * for all packets sent.  The NPI-IP provider will request that the Stream head provide an
 * additional write offset of 20 bytes to accomodate the IP header.
 */
STATIC noinline streams_fastcall __unlikely int
ne_unitdata_req_slow(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	size_t dlen;
	N_unitdata_req_t *p;
	struct sockaddr_in dst_buf, *DEST_buffer = NULL;
	int err;
	mblk_t *dp = mp->b_cont;
	uint32_t daddr;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_UNITDATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CONS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CLNS))
		goto error;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto error;
	err = NNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = NBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
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
	err = NBADDATA;
	if (unlikely(dp == NULL))
		goto error;
	if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.NSDU_size))
		goto error;
	if (unlikely((err = np_senddata(np, np->qos.protocol, daddr, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      error:
	if ((err = ne_uderror_reply(q, DEST_buffer, 0, err, dp)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
}

/**
 * ne_unitdata_req - NE_UNITDATA_REQ unit data request event
 * @q: write queue
 * @mp: the primitive
 *
 * Optimize fast path for no options and correct behaviour.  This should run much faster than the
 * old plodding way of doing things.
 */
STATIC INLINE streams_fastcall __hot_put int
ne_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_unitdata_req_t *p;
	mblk_t *dp;
	int err;
	struct sockaddr_in dst_buf;
	uint32_t daddr;

	prefetch(np);
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	p = (typeof(p)) mp->b_rptr;
	prefetch(p);
	dassert(p->PRIM_type == N_UNITDATA_REQ);
	if (unlikely(np->info.SERV_type == N_CONS))
		goto go_slow;
	if (unlikely(np->info.SERV_type != N_CLNS))
		goto go_slow;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto go_slow;
	if (unlikely(p->DEST_length == 0))
		goto go_slow;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
		goto go_slow;
	if (unlikely(p->DEST_length != sizeof(struct sockaddr_in)))
		goto go_slow;
	/* avoid alignment problems */
	bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, p->DEST_length);
	if (unlikely(dst_buf.sin_family != AF_INET))
		goto go_slow;
	if (unlikely(dst_buf.sin_addr.s_addr == INADDR_ANY))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	prefetch(dp);
	{
		size_t dlen;

		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.NSDU_size))
			goto go_slow;
	}
	daddr = dst_buf.sin_addr.s_addr;
	if (unlikely((err = np_senddata(np, np->qos.protocol, daddr, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      error:
	err = ne_uderror_reply(q, &dst_buf, 0, err, dp);
	if (likely(err == QR_ABSORBED))
		return (QR_TRIMMED);
	return (err);
      go_slow:
	return ne_unitdata_req_slow(q, mp);
}

/**
 * ne_conn_req - NE_CONN_REQ connect NS user event
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
STATIC noinline streams_fastcall __hot_put int
ne_conn_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_conn_req_t *p;
	struct N_qos_sel_conn_ip qos_buf = { N_QOS_SEL_CONN_IP, }, *QOS_buffer = &qos_buf;
	struct sockaddr_in dst_buf[8] = { {AF_INET,}, }, *DEST_buffer = NULL;
	int err;
	mblk_t *dp = mp->b_cont;
	size_t dlen;
	int i;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_CONN_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
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
	err = NACCESS;
	if (unlikely(np->CONIND_number > 0))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto error;
#if 1
	err = NBADFLAG;
	if (unlikely(p->CONN_flags != 0))
		goto error;
#endif
	err = NNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = NBADADDR;
	if (unlikely(p->DEST_length > np->info.ADDR_size))
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
	if (p->QOS_length != 0) {
		err = NBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
			goto error;
		if (unlikely(p->QOS_length < sizeof(QOS_buffer->n_qos_type)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, sizeof(QOS_buffer->n_qos_type));
		err = NBADQOSTYPE;
		if (unlikely(QOS_buffer->n_qos_type != N_QOS_SEL_CONN_IP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(*QOS_buffer)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, p->QOS_length);
	} else {
		/* set default values in QOS buffer */
		QOS_buffer->n_qos_type = N_QOS_SEL_CONN_IP;
		QOS_buffer->protocol = QOS_UNKNOWN;
		QOS_buffer->priority = QOS_UNKNOWN;
		QOS_buffer->ttl = QOS_UNKNOWN;
		QOS_buffer->tos = QOS_UNKNOWN;
		QOS_buffer->mtu = QOS_UNKNOWN;
		QOS_buffer->saddr = QOS_UNKNOWN;
		QOS_buffer->daddr = QOS_UNKNOWN;
	}
	if (dp != NULL) {
		err = NBADDATA;
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.CDATA_size))
			goto error;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	err = ne_conn_con(q, DEST_buffer, p->DEST_length, QOS_buffer, p->CONN_flags);
	if (unlikely(err != 0))
		goto error;
	/* send data only after connection complete */
	if (dp == NULL)
		return (QR_DONE);
	if (np_senddata(np, np->qos.protocol, np->qos.daddr, dp) != QR_ABSORBED) {
		pswerr(("Discarding data on N_CONN_REQ\n"));
		return (QR_DONE);	/* discard the data */
	}
	return (QR_TRIMMED);	/* np_senddata() consumed message blocks */
      error:
	return ne_error_ack(q, N_CONN_REQ, err);
}

STATIC INLINE streams_fastcall mblk_t *
n_seq_check(struct np *np, np_ulong SEQ_number)
{
	mblk_t *cp;

	for (cp = bufq_head(&np->conq); cp && (np_ulong) (long) cp != SEQ_number; cp = cp->b_next) ;
	usual(cp);
	return (cp);
}
STATIC INLINE streams_fastcall struct np *
n_tok_check(np_ulong TOKEN_value)
{
	struct np *ap;
	unsigned long flags;

	read_lock_str2(&master.lock, flags);
	for (ap = master.np.list; ap && (np_ulong) (long) ap->oq != TOKEN_value; ap = ap->next) ;
	read_unlock_str2(&master.lock, flags);
	usual(ap);
	return (ap);
}

/**
 * ne_conn_res - NE_CONN_RES accept previous connection indication event
 * @q: active queue (write queue)
 * @mp: N_CONN_RES message
 *
 * An NE_CONN_IND event is generated when an IP message arrives for an address bound to an N_CONS
 * Stream and there is not yet a connection for the source address in the message.  Outstanding
 * connection indications are queued against the Stream with a sequence number assigned (derived
 * from the address of the connecting message held in the connection indication list).  The
 * NE_CONN_RES event is generated by the user to accept a connection indication event from the
 * connection indication queue and to perform a passive connection (NE_PASS_CON event) on the
 * indicated Stream.  In addition, the user can set QOS parameters for the Stream to which a passive
 * connection is made and for any reponse message (data attached to the N_CONN_RES message).
 *
 * There is a deviation here from the NPI specifications: the responding address(es) in the
 * N_CONN_RES primitive contains the list of destination address(es) to which to form the
 * connection.  If no responding addresses are provided, then the destination address is the source
 * address from the connection indication.
 */
STATIC noinline streams_fastcall int
ne_conn_res(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q), *TOKEN_value = np;
	N_conn_res_t *p;
	struct N_qos_sel_conn_ip qos_buf = { N_QOS_SEL_CONN_IP, }, *QOS_buffer = NULL;
	struct sockaddr_in res_buf[8] = { {AF_INET,}, }, *RES_buffer = NULL;
	mblk_t *dp, *SEQ_number;
	size_t dlen;
	int err;

	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_CONN_RES);
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	if (unlikely(np_not_state(np, NSF_WRES_CIND)))
		goto error;
	if (p->RES_length != 0) {
		err = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length))
			goto error;
		if (unlikely(p->RES_length < sizeof(*RES_buffer)))
			goto error;
		if (unlikely(p->RES_length > (sizeof(*RES_buffer) << 3)))
			goto error;
		if (unlikely(p->RES_length % sizeof(*RES_buffer) != 0))
			goto error;
		/* Cannot be sure that the address is properly aligned, and to keep unscrupulous
		   users from DoS attacks, copy the information into an aligned buffer. */
		bcopy(mp->b_rptr + p->RES_offset, &res_buf, p->RES_length);
		if (unlikely(res_buf[0].sin_family != AF_INET && res_buf[0].sin_family != 0))
			goto error;
		if (unlikely(res_buf[0].sin_port == 0))
			goto error;
		RES_buffer = res_buf;
	}
	if (p->QOS_length != 0) {
		err = NBADOPT;
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
		err = NBADQOSTYPE;
		if (unlikely(qos_buf.n_qos_type != N_QOS_SEL_CONN_IP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(qos_buf)))
			goto error;
		QOS_buffer = &qos_buf;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, p->QOS_length);
	}
	err = NBADDATA;
	if ((dp = mp->b_cont) != NULL)
		if (unlikely((dlen = msgsize(dp)) == 0 || dlen > np->info.CDATA_size))
			goto error;
	err = NBADSEQ;
	if (unlikely(p->SEQ_number == 0))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSF_WRES_CIND) || bufq_length(&np->conq) < 1))
		goto error;
	err = NBADSEQ;
	if (unlikely((SEQ_number = n_seq_check(np, p->SEQ_number)) == NULL))
		goto error;
	if (p->TOKEN_value != 0) {
		err = NBADTOKEN;
		if (unlikely((TOKEN_value = n_tok_check(p->TOKEN_value)) == NULL))
			goto error;
		err = NBADTOKEN;
		if (unlikely(TOKEN_value->info.SERV_type == N_CLNS))
			goto error;
		err = NBADTOKEN;
		if (unlikely(TOKEN_value->CONIND_number > 0))
			goto error;
		err = NOUTSTATE;
		if (np_get_state(TOKEN_value) != NS_IDLE)
			/* Later we could auto bind if in NS_UNBND state. Note that the Stream to
			   which we do a passive connection could be already connected: we just are 
			   just adding another address to a multihomed connection.  But this is not 
			   as useful as adding or removing an address with N_OPTMGMT_REQ. */
			goto error;
		err = NBADTOKEN;
		if (TOKEN_value->info.SERV_type != N_CONS)
			/* Must be connection-oriented Stream. */
			goto error;
	} else {
		err = NOUTSTATE;
		if (bufq_length(&np->conq) > 1)
			goto error;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	np_set_state(np, NS_WACK_CRES);
	err = ne_ok_ack(q, N_CONN_RES, RES_buffer, p->RES_length,
			QOS_buffer, SEQ_number, TOKEN_value, p->CONN_flags, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* user data is absorbed */
      error:
	return ne_error_ack(q, N_CONN_RES, err);
}

/**
 * ne_discon_req - process N_DISCON_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DISCON_REQ message
 */
STATIC noinline streams_fastcall int
ne_discon_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_discon_req_t *p;
	struct sockaddr_in *RES_buffer = NULL;
	size_t RES_length = 0;
	mblk_t *dp, *SEQ_number = NULL;
	size_t dlen;
	int err;
	np_long state;

	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DISCON_REQ);
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSM_CONNECTED)))
		goto error;
#if 1
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
	err = NBADDATA;
	if ((dp = mp->b_cont) != NULL)
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.DDATA_size))
			goto error;
	/* FIXME: hold conq bufq_lock to keep connection indication state from changing */
	state = np_get_state(np);
	err = NBADSEQ;
	if (p->SEQ_number != 0) {
		if (unlikely(state != NS_WRES_CIND))
			goto error;
		if (unlikely((SEQ_number = n_seq_check(np, p->SEQ_number)) == NULL))
			goto error;
	} else {
		if (unlikely(state == NS_WRES_CIND))
			goto error;
	}
	err = NOUTSTATE;
	switch (state) {
	case NS_WCON_CREQ:
		np_set_state(np, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		np_set_state(np, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		np_set_state(np, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		np_set_state(np, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		np_set_state(np, NS_WACK_DREQ11);
		break;
	default:
		goto error;
	}
	/* Ok, all checking done.  Now we need to disconnect the address. */
	err = ne_ok_ack(q, N_DISCON_REQ, RES_buffer, RES_length, NULL, SEQ_number, NULL,
			p->DISCON_reason, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* user data is absorbed */
      error:
	return ne_error_ack(q, N_DISCON_REQ, err);
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
STATIC noinline streams_fastcall __unlikely int
ne_write_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	size_t dlen;
	int err;

	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_DATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (np_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
	   destination of the request is an address that does not have an outstanding reset
	   indication. */
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	/* If we are writing we must include the IP header, which is at least 20 bytes, and, if the 
	   Stream is bound to a port, at least the size of a UDP header.  The length of the entire
	   NSDU must not exceed 65535 bytes. */
	err = NBADDATA;
	if (unlikely((dlen = msgsize(mp)) == 0
		     || dlen > np->info.NIDU_size || dlen > np->info.NSDU_size))
		goto error;
	if (unlikely((err = np_senddata(np, np->qos.protocol, np->qos.daddr, mp)) != QR_ABSORBED))
		goto error;
	return (QR_ABSORBED);	/* np_senddata() consumed message block */
      discard:
	return (QR_DONE);	/* np_senddata() did not consume message blocks */
      error:
	return ne_error_reply(q, -EPROTO);
}

/**
 * ne_data_req - process N_DATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATA_REQ message
 *
 * Unfortunately, there is no standard way of specifying destination and source addreses for
 * multihomed hosts.  We use N_OPTMGMT_REQ to change the primary destination address, source address
 * and QOS parameters.
 */
STATIC noinline streams_fastcall __hot_put int
ne_data_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_data_req_t *p;
	size_t dlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_DATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (np_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		/* For multihomed operation, we should not actually discard the N_DATA_REQ if the
		   destination of the request is an address that does not have an outstanding reset
		   indication. */
		goto discard;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	err = NBADFLAG;
	if (unlikely(p->DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG)))
		/* TODO: We should check the N_MORE_DATA_FLAG and see whether this is a complete
		   NSDU or not.  If not, we should accumulate the M_DATA block in a buffer waiting
		   for a final N_DATA_REQ or delimited message.  */
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely
	    ((dlen = msgsize(dp)) == 0 || dlen > np->info.NIDU_size || dlen > np->info.NSDU_size))
		goto error;
	if (unlikely((err = np_senddata(np, np->qos.protocol, np->qos.daddr, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* np_senddata() consumed message blocks */
      discard:
	return (QR_DONE);	/* np_senddata() does not consume message blocks */
      error:
	return ne_error_reply(q, -EPROTO);
}

/**
 * ne_exdata_req - process N_EXDATA_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_EXDATA_REQ message
 */
STATIC noinline streams_fastcall int
ne_exdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_exdata_req_t *p;
	size_t dlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_EXDATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider
	   receives the N_EXDATA_REQ primitive, then the NS provider should discard the request
	   without generating a fatal error. */
	if (np_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	dlen = msgsize(dp);
	if (unlikely(dlen == 0 || dlen > np->info.NIDU_size || dlen > np->info.ENSDU_size))
		goto error;
	err = np_senddata(np, np->qos.protocol, np->qos.daddr, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      discard:
	return (QR_DONE);	/* np_senddata() does not consume message blocks */
      error:
	return ne_error_reply(q, -EPROTO);
}

/**
 * ne_datack_req - process N_DATACK_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_DATACK_REQ message
 */
STATIC noinline streams_fastcall int
ne_datack_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_datack_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DATACK_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_DATACK_REQ primitive, then the NS provider should discard the request without
	   generating a fatal error. */
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (unlikely(np_not_state(np, NSM_CONNECTED)))
		goto error;
	/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the receipt
	   confirmation flag set, then the NS provider should just ignore the request without
	   generating a fatal error. */
	if (unlikely(bufq_length(&np->datq) <= 0))
		goto error;
	if (unlikely((err = np_datack(q)) < 0))
		goto error;
      discard:
	return (QR_DONE);
      error:
	return ne_error_reply(q, -EPROTO);
}

/**
 * ne_reset_req - process N_RESET_REQ message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_REQ message
 */
STATIC noinline streams_fastcall int
ne_reset_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_reset_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_reset_req_t *) mp->b_rptr;
	dassert(p->PRIM_type == N_RESET_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_RESET_REQ primitive, then the NS provider should discard the message without
	   generating an error. */
	err = 0;
	if (np_get_state(np) == NS_IDLE)
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_get_state(np) != NS_DATA_XFER))
		goto error;
	/* Ok, message checks out. */
	np_set_state(np, NS_WCON_RREQ);
	if ((err = ne_reset_con(q, N_USER, p->RESET_reason, mp->b_cont)) != 0)
		goto error;
	return (QR_DONE);	/* user data message blocks are not absorbed */
      error:
	return ne_error_ack(q, N_RESET_REQ, err);

}

/**
 * ne_reset_res - process N_RESET_RES message
 * @q: active queue in queue pair (write queue)
 * @mp: the N_RESET_RES message
 *
 * When a reset indication is sent to the user, we expect a reset response which is acknowledged.
 * The reason is, only one oustanding reset indication per destination per RESET_reason is issued.
 * No additional reset indication will be issued until a response is received.  Because of this,
 * reset indications are expedited (band 2).
 */
STATIC noinline streams_fastcall int
ne_reset_res(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_reset_res_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_reset_res_t *) mp->b_rptr;
	dassert(p->PRIM_type == N_RESET_RES);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	if (unlikely(np_not_state(np, NSF_WRES_RIND)))
		goto error;
	/* Ok, parameters check out. */
	np_set_state(np, NS_WACK_RRES);
	err = ne_ok_ack(q, N_RESET_RES, NULL, 0, NULL, NULL, NULL, 0, NULL);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);
      error:
	return ne_error_ack(q, N_RESET_RES, err);
}

/*
 *  STREAMS MESSAGE HANDLING
 */

/**
 * np_w_proto - process an M_PROTO, M_PCPROTO message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_PROTO, M_PCPROTO message to process
 *
 * These are normal N-primitives written from the upper layer protocol.
 */
STATIC INLINE streams_fastcall __hot_put int
np_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct np *np = NP_PRIV(q);

	np->i_oldstate = np_get_state(np);	/* checkpoint */

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(np_long *) mp->b_rptr)) {
		case N_UNITDATA_REQ:	/* Connection-less data send request */
			_printd(("%s: %p: -> N_UNITDATA_REQ ->\n", DRV_NAME, np));
			rtn = ne_unitdata_req(q, mp);
			break;
		case N_DATA_REQ:	/* Connection-Mode data transfer request */
			_printd(("%s: %p: -> N_DATA_REQ ->\n", DRV_NAME, np));
			rtn = ne_data_req(q, mp);
			break;
		case N_CONN_REQ:	/* NC request */
			_printd(("%s: %p: -> N_CONN_REQ ->\n", DRV_NAME, np));
			rtn = ne_conn_req(q, mp);
			break;
		case N_CONN_RES:	/* Accept previous connection indication */
			_printd(("%s: %p: -> N_CONN_RES ->\n", DRV_NAME, np));
			rtn = ne_conn_res(q, mp);
			break;
		case N_DISCON_REQ:	/* NC disconnection request */
			_printd(("%s: %p: -> N_DISCON_REQ ->\n", DRV_NAME, np));
			rtn = ne_discon_req(q, mp);
			break;
		case N_EXDATA_REQ:	/* Expedited data request */
			_printd(("%s: %p: -> N_EXDATA_REQ ->\n", DRV_NAME, np));
			rtn = ne_exdata_req(q, mp);
			break;
		case N_INFO_REQ:	/* Information Request */
			_printd(("%s: %p: -> N_INFO_REQ ->\n", DRV_NAME, np));
			rtn = ne_info_req(q, mp);
			break;
		case N_BIND_REQ:	/* Bind a NS user to network address */
			_printd(("%s: %p: -> N_BIND_REQ ->\n", DRV_NAME, np));
			rtn = ne_bind_req(q, mp);
			break;
		case N_UNBIND_REQ:	/* Unbind NS user from network address */
			_printd(("%s: %p: -> N_UNBIND_REQ ->\n", DRV_NAME, np));
			rtn = ne_unbind_req(q, mp);
			break;
		case N_OPTMGMT_REQ:	/* Options Management request */
			_printd(("%s: %p: -> N_OPTMGMT_REQ ->\n", DRV_NAME, np));
			rtn = ne_optmgmt_req(q, mp);
			break;
		case N_DATACK_REQ:	/* Data acknowledgement request */
			_printd(("%s: %p: -> N_DATACK_REQ ->\n", DRV_NAME, np));
			rtn = ne_datack_req(q, mp);
			break;
		case N_RESET_REQ:	/* NC reset request */
			_printd(("%s: %p: -> N_RESET_REQ ->\n", DRV_NAME, np));
			rtn = ne_reset_req(q, mp);
			break;
		case N_RESET_RES:	/* Reset processing accepted */
			_printd(("%s: %p: -> N_RESET_RES ->\n", DRV_NAME, np));
			rtn = ne_reset_res(q, mp);
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
#ifndef _TEST
		/* not so seldom() during conformance suite testing */
		seldom();
#endif
		np_set_state(np, np->i_oldstate);
		/* The put and service procedure do not recognize all errors.  Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EOPNOTSUPP:	/* primitive not supported */
			return ne_error_ack(q, prim, rtn);
		case -EPROTO:
			return ne_error_reply(q, -EPROTO);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/**
 * np_wp_data - process M_DATA message
 * @q: active queue in pair (write queue)
 * @mp: the M_DATA message
 */
STATIC noinline streams_fastcall __unlikely int
np_w_data(queue_t *q, mblk_t *mp)
{
	return ne_write_req(q, mp);
}

/**
 * np_w_other - proces other message
 * @q: active queue in pair (write queue)
 * @mp: the message
 */
STATIC noinline streams_fastcall __unlikely int
np_w_other(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		np->u.dev.cminor);
	return (-EOPNOTSUPP);
}

/**
 * np_w_ioctl - M_IOCTL handling
 * @q: active queue in queue pair (write queue)
 * @mp: the message
 *
 * This NPI-IP provider does not support any input-output controls and, therefore, all input-output
 * controls are negatively acknowledged.
 */
STATIC noinline streams_fastcall __unlikely int
np_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;

	mp->b_datap->db_type = M_IOCNAK;
	iocp->ioc_count = 0;
	iocp->ioc_error = EINVAL;
	iocp->ioc_rval = -1;
	qreply(q, mp);
	return (QR_ABSORBED);
}

STATIC noinline streamscall int
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
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

/**
 * np_r_other - process other message
 * @q: active queue in pair (read queue)
 * @mp: the message
 */
STATIC noinline streams_fastcall __unlikely int
np_r_other(queue_t *q, mblk_t *mp)
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
 * np_r_data - process M_DATA message
 * @q: active queue in queue pair (read queue)
 * @mp: the M_DATA message
 *
 * M_DATA messages are placed to the read queue by the Linux IP np_v4_rcv() callback.  The message
 * contains a complete IP datagram starting with the IP header.  What needs to be done is to convert
 * this to an upper layer indication and deliver it upstream.
 */
STATIC INLINE streams_fastcall __hot_in int
np_r_data(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	int rtn;

	switch (np->info.SERV_type) {
	case N_CLNS:
		switch (np_get_state(np)) {
		case NS_IDLE:
			rtn = ne_unitdata_ind(q, mp);
			break;
		default:
			rtn = QR_DONE;
			break;
		}
		break;
	case N_CONS:
		switch (np_get_state(np)) {
		case NS_DATA_XFER:
		case NS_WCON_CREQ:
		case NS_WACK_RRES:
		case NS_WRES_RIND:
		case NS_WCON_RREQ:
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ7:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			rtn = ne_data_ind(q, mp);
			break;
		case NS_IDLE:
			if (np->CONIND_number == 0) {
				rtn = QR_DONE;
				break;
			}
			/* fall through */
		case NS_WRES_CIND:
		case NS_WACK_CRES:
			rtn = ne_conn_ind(q, mp);
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
 * np_r_ctl - process M_CTL message
 * @q: active queue in queue pair (read queue)
 * @mp: the M_CTL message
 *
 * M_CTL messages are placed to the read queue by the Linux IP np_v4_err() callback.  The message
 * contains a complete ICMP datagram starting with the IP header.  What needs to be done is to
 * convert this to an upper layer indication and deliver it upstream.
 */
STATIC noinline streams_fastcall __unlikely int
np_r_ctl(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	int rtn;

	switch (np->info.SERV_type) {
	case N_CLNS:
		switch (np_get_state(np)) {
		case NS_IDLE:
			rtn = ne_uderror_ind_icmp(q, mp);
			break;
		default:
			rtn = QR_DONE;
			break;
		}
		break;
	case N_CONS:
		switch (np_get_state(np)) {
		case NS_DATA_XFER:
		case NS_WCON_CREQ:
		case NS_WACK_RRES:
		case NS_WCON_RREQ:
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ7:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			rtn = ne_reset_ind(q, mp);
			break;
		case NS_WRES_CIND:
		case NS_WACK_CRES:
			rtn = ne_discon_ind_icmp(q, mp);
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

STATIC noinline streamscall int
np_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		putnext(q, mp);
		return (QR_ABSORBED);
	}
	if (mp->b_rptr[0] & FLUSHW) {
		putnext(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

STATIC noinline streams_fastcall __unlikely int
np_r_prim_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return np_r_data(q, mp);
	case M_CTL:
		return np_r_ctl(q, mp);
	case M_FLUSH:
		return np_r_flush(q, mp);
	default:
		return np_r_other(q, mp);
	}
}

/**
 * np_r_prim - process primitive on read queue
 * @q: active queue in queue pair (read queue)
 * @mp: the message
 */
STATIC INLINE streamscall __hot_in int
np_r_prim(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_DATA))
		/* fast path for data */
		if (likely(NP_PRIV(q)->info.SERV_type == N_CLNS))
			return ne_unitdata_ind(q, mp);
	return np_r_prim_slow(q, mp);
}

STATIC noinline streams_fastcall __unlikely int
np_w_prim_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return np_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(q, mp);
	case M_FLUSH:
		return np_w_flush(q, mp);
	case M_IOCTL:
		return np_w_ioctl(q, mp);
	default:
		return np_w_other(q, mp);
	}
}

STATIC noinline streams_fastcall __unlikely int
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

STATIC noinline streams_fastcall void
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

static noinline streams_fastcall int
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
STATIC noinline streams_fastcall __unlikely struct np *
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
STATIC INLINE streams_fastcall __hot_in struct np *
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

STATIC noinline streams_fastcall __unlikely struct np *
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

STATIC INLINE streams_fastcall __hot_in struct np *
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
STATIC INLINE streams_fastcall __hot_in struct np *
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
STATIC noinline streams_fastcall __unlikely struct np *
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
	struct iphdr *iph = skb->nh.iph;
	struct udphdr *uh = (struct udphdr *) (skb->nh.raw + (iph->ihl << 2));
	struct rtable *rt;

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
		size_t plen = skb->len + (skb->data - skb->nh.raw);

		/* now allocate an mblk */
		if (unlikely((mp = esballoc(skb->nh.raw, plen, BPRI_MED, &fr)) == NULL))
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
		size_t plen = skb->len + (skb->data - skb->nh.raw);

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
		bcopy(skb->nh.raw, mp->b_wptr, plen);
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
STATIC noinline struct np *
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
		np->qor.tos.tos_max_value = 15;
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
STATIC noinline void
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
#define  IPCO_CMINOR	N_CONS
#define  IPCL_CMINOR	N_CLNS
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
	putnext(q, mp);
	qprocson(q);
	return (0);
}

/**
 * np_qclose - NPI IP driver STREAMS close routine
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
	if (np_ip_prot_cachep != NULL) {
		if (kmem_cache_destroy(np_ip_prot_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_ip_prot_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		_printd(("%s: destroyed np_ip_prot_cachep\n", DRV_NAME));
		np_ip_prot_cachep = NULL;
	}
	if (np_ip_priv_cachep != NULL) {
		if (kmem_cache_destroy(np_ip_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_ip_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		_printd(("%s: destroyed np_ip_priv_cachep\n", DRV_NAME));
		np_ip_priv_cachep = NULL;
	}
	return (0);
}
STATIC __unlikely int
np_init_caches(void)
{
	if (np_ip_priv_cachep == NULL) {
		np_ip_priv_cachep = kmem_cache_create("np_ip_priv_cachep", sizeof(struct np), 0,
						      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_ip_priv_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_ip_priv_cachep", __FUNCTION__);
			np_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (np_ip_prot_cachep == NULL) {
		np_ip_prot_cachep =
		    kmem_cache_create("np_ip_prot_cachep", sizeof(struct np_prot_bucket), 0,
				      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_ip_prot_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_ip_prot_cachep", __FUNCTION__);
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

STATIC struct devnode np_node_ip = {
	.n_name = "ip",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode np_node_ipco = {
	.n_name = "ipco",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode np_node_ipcl = {
	.n_name = "ipcl",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC __unlikely int
np_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&np_cdev, major)) < 0)
		return (err);
	register_strnod(&np_cdev, &np_node_ip, IP_CMINOR);
	register_strnod(&np_cdev, &np_node_ipco, IPCO_CMINOR);
	register_strnod(&np_cdev, &np_node_ipcl, IPCL_CMINOR);
	return (0);
}

STATIC __unlikely int
np_unregister_strdev(major_t major)
{
	int err;

	unregister_strnod(&np_cdev, IP_CMINOR);
	unregister_strnod(&np_cdev, IPCO_CMINOR);
	unregister_strnod(&np_cdev, IPCL_CMINOR);
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
ipterminate(void)
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
ipinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	np_init_hashes();
	if ((err = np_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ipterminate();
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
				ipterminate();
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
module_init(ipinit);
module_exit(ipterminate);

#endif				/* LINUX */
