/*****************************************************************************

 @(#) $RCSfile: nf.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/05/14 06:34:31 $

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

 Last Modified $Date: 2006/05/14 06:34:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: nf.c,v $
 Revision 0.9.2.2  2006/05/14 06:34:31  brian
 - corrected buffer leaks

 Revision 0.9.2.1  2006/03/25 10:26:32  brian
 - added beginnings of netfilter driver

 *****************************************************************************/

#ident "@(#) $RCSfile: nf.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/05/14 06:34:31 $"

static char const ident[] = "$RCSfile: nf.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/05/14 06:34:31 $";

/*
 *  This pseudo-device driver implements a range of netfilter hooks for
 *  performance testing of Internet Protocols.  It hooks into the local
 *  delivery netfilter hook point and can perform one of several actions on a
 *  specified range of packets:
 *
 *  1. Pass the packet unaltered.
 *  2. Simulate a checksum error (toggle a bit in the packet).
 *  3. Simulate a packet drop (discard the packet).
 *  4. Simulate delay (steal the packet and then resend it later).
 *  5. Simulate duplication (clone the packet and send it twice).
 *
 *  Combinations of the above.
 *
 *  The purpose of this driver is for performance testing of SCTP and other
 *  OpenSS7 protocols.  The driver is GPL2, however, the utility programs that
 *  come with this are under the OpenSS7 testing license (GPL2 for OpenSS7
 *  module testing only).
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>
#include <linux/interrupt.h>

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include "ip_hooks.h"

#define NF_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NF_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define NF_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define NF_REVISION	"OpenSS7 $RCSfile: nf.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/05/14 06:34:31 $"
#define NF_DEVICE	"SVR 4.2 STREAMS NETFILTER Driver"
#define NF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NF_LICENSE	"GPL"
#define NF_BANNER	NF_DESCRIP	"\n" \
			NF_EXTRA	"\n" \
			NF_REVISION	"\n" \
			NF_COPYRIGHT	"\n" \
			NF_DEVICE	"\n" \
			NF_CONTACT
#define NF_SPLASH	NF_DESCRIP	"\n" \
			NF_REVISION

#ifdef LINUX
MODULE_AUTHOR(NF_CONTACT);
MODULE_DESCRIPTION(NF_DESCRIP);
MODULE_SUPPORTED_DEVICE(NF_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NF_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-nf");
#endif
#endif				/* LINUX */

#ifdef LFS
#define NF_DRV_ID	CONFIG_STREAMS_NF_MODID
#define NF_DRV_NAME	CONFIG_STREAMS_NF_NAME
#define NF_CMAJORS	CONFIG_STREAMS_NF_NMAJORS
#define NF_CMAJOR_0	CONFIG_STREAMS_NF_MAJOR
#define NF_UNITS	CONFIG_STREAMS_NF_NMINORS
#endif

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NF_MODID));
MODULE_ALIAS("streams-driver-nf");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NF_MAJOR));
MODULE_ALIAS("/dev/streams/nf");
MODULE_ALIAS("/dev/streams/nf/*");
MODULE_ALIAS("/dev/streams/clone/nf");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(NF_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(NF_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(NF_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(NF_CMAJOR_0) "-" __stringify(NF_CMINOR));
MODULE_ALIAS("/dev/nf");
MODULE_ALIAS("/dev/inet/nf");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		NF_DRV_ID
#define DRV_NAME	NF_DRV_NAME
#define CMAJORS		NF_CMAJORS
#define CMAJOR_0	NF_CMAJOR_0
#define UNITS		NF_UNITS
#ifdef MODULE
#define DRV_BANNER	NF_BANNER
#else				/* MODULE */
#define DRV_BANNER	NF_SPLASH
#endif				/* MODULE */

STATIC struct module_info nf_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC streamscall int nf_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int nf_close(queue_t *, int, cred_t *);

STATIC streamscall int nf_rput(queue_t *, mblk_t *);
STATIC streamscall int nf_rsrv(queue_t *);

STATIC struct qinit nf_rinit = {
	.qi_putp = nf_rput,		/* Read put (msg from below) */
	.qi_srvp = nf_rsrv,		/* Read queue service */
	.qi_qopen = nf_open,		/* Each open */
	.qi_qclose = nf_close,		/* Last close */
	.qi_minfo = &nf_minfo,		/* Information */
};

STATIC streamscall int nf_wput(queue_t *, mblk_t *);
STATIC streamscall int nf_wsrv(queue_t *);

STATIC struct qinit nf_winit = {
	.qi_putp = nf_wput,		/* Write put (msg from above) */
	.qi_srvp = nf_wsrv,		/* Write queue service */
	.qi_minfo = &nf_minfo,		/* Information */
};

MODULE_STATIC struct streamtab nf_info = {
	.st_rdinit = &nf_rinit,		/* Upper read queue */
	.st_wrinit = &nf_winit,		/* Lower read queue */
};

/*
 *  Primary data structure.
 */
struct nf;
typedef struct nf nf_t;
struct nf {
	nf_t *next;			/* linkage for master list */
	nf_t **prev;			/* linkage for master list */
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
	nf_t *bnext;			/* linkage for bind hash */
	nf_t **bprev;			/* linkage for bind hash */
	struct nf_bind_bucket *bindb;	/* linkage for bind hash */
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
#define NF_PRIV(__q) ((struct nf *)((__q)->q_ptr))

STATIC struct nf *nf_opens = NULL;
STATIC spinlock_t nf_lock = SPIN_LOCK_UNLOCKED;

/* FIXME: much more to write here... */


