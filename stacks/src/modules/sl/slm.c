/*****************************************************************************

 @(#) $RCSfile: slm.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/07/05 22:45:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/07/05 22:45:53 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: slm.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/07/05 22:45:53 $"

static char const ident[] =
    "$RCSfile: slm.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/07/05 22:45:53 $";

/*
 *  This is an SLM (Signalling Link Management) multiplexing driver which also
 *  supports M2UA.  It has two purposes: 1) to act as an MTP Level 2
 *  signalling link management layer for OpenSS7 MTP2 drivers; and, 2) to
 *  implement the M2UA protocol for the OpenSS7 stack.
 *
 *  The SLM multiplexing driver is opened by the SS7 Configuration Daemon and
 *  OpenSS7 Signalling Link (SL) drivers are opened and I_PLINKed under the
 *  multiplexor.  In addition, the ss7d opens transport streams and forms the
 *  necessary associations and I_PLINKs them under the multiplexor.  Transport
 *  streams can either be client streams (going to an M2UA ASP), server
 *  streams (going to an M2UA SG), or peer streams (going to a peer AS or SG).
 *  Transport streams are normally SCTP streams but can also be TCP, SSCOP-MCE
 *  streams, or any stream recognizing TPI connection oriented data primtives
 *  (e.g, a pipe with a suitable module).  Users of the M2UA multiplexing
 *  driver may open user streams on the driver and use them in an identical
 *  fashion to an OpenSS7 MTP2 Signalling Link.
 *
 *  All SL (Signalling Link) user streams opened on the multiplexing driver
 *  are Style 2 drivers.  That is, they must be attached to a specific
 *  SDTI/SDLI (Signalling Data Terminal Identifier/Signalling Data Link
 *  Identifier) before being enabled.
 */
#include <os7/compat.h>

#ifdef LINUX
#include <linux/errno.h>
#include <linux/types.h>
#endif				/* LINUX */

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/ua_lm.h>
#include <ss7/ua_lm_ioctl.h>

#define SLM_DESCRIP	"SLM: SS7/SL (Signalling Link) STREAMS MULTIPLEXING DRIVER."
#define SLM_REVISION	"OpenSS7 $RCSfile: slm.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/07/05 22:45:53 $"
#define SLM_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SLM_DEVICE	"Supports the OpenSS7 MTP2 and INET transport drivers."
#define SLM_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SLM_LICENSE	"GPL"
#define SLM_BANNER	SLM_DESCRIP	"\n" \
			SLM_REVISION	"\n" \
			SLM_COPYRIGHT	"\n" \
			SLM_DEVICE	"\n" \
			SLM_CONTACT
#define SLM_SPLASH	SLM_DEVICE	" - " \
			SLM_REVISION	"\n" \

#ifdef LINUX
MODULE_AUTHOR(SLM_CONTACT);
MODULE_DESCRIPTION(SLM_DESCRIP);
MODULE_SUPPORTED_DEVICE(SLM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SLM_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-slm");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SLM_DRV_ID		CONFIG_STREAMS_SLM_MODID
#define SLM_DRV_NAME		CONFIG_STREAMS_SLM_NAME
#define SLM_CMAJORS		CONFIG_STREAMS_SLM_NMAJORS
#define SLM_CMAJOR_0		CONFIG_STREAMS_SLM_MAJOR
#define SLM_UNITS		CONFIG_STREAMS_SLM_NMINORS
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SLM_DRV_ID
#define DRV_NAME	SLM_DRV_NAME
#define CMAJORS		SLM_CMAJORS
#define CMAJOR_0	SLM_CMAJOR_0
#define UNITS		SLM_UNITS
#ifdef MODULE
#define DRV_BANNER	SLM_BANNER
#else				/* MODULE */
#define DRV_BANNER	SLM_SPLASH
#endif				/* MODULE */

STATIC struct module_info slm_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1024,		/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC int slm_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int slm_close(queue_t *, mblk_t *);

STATIC struct qinit sl_rinit = {
	.qi_putp = ss7_oput,		/* Read put (message from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = slm_open,		/* Each open */
	.qi_qclose = slm_close,		/* Last close */
	.qi_minfo = &slm_minfo,		/* Information */
};

STATIC struct qinit sl_winit = {
	.qi_putp = ss7_iput,		/* Write put (message from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_minfo = &slm_minfo,		/* Information */
};

STATIC struct qinit ls_rinit = {
	.qi_putp = ss7_iput,		/* Read put (message from below) */
	.qi_srvp = ss7_isrv,		/* Read queue service */
	.qi_minfo = &slm_minfo,		/* Information */
};

STATIC struct qinit ls_winit = {
	.qi_putp = ss7_oput,		/* Write put (message from above) */
	.qi_putp = ss7_osrv,		/* Write queue service */
	.qi_minfo = &slm_minfo,		/* Information */
};

STATIC struct streamtab slm_info = {
	.st_rdinit = &sl_rinit,		/* Upper read queue */
	.st_wrinit = &sl_winit,		/* Upper write queue */
	.st_muxrinit = &ls_rinit,	/* Lower read queue */
	.st_muxwinit = &ls_winit,	/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structure
 *
 *  =========================================================================
 */

typedef struct pp {
	struct pp *next;		/* list linkage */
	struct pp **prev;		/* list linkage */
	struct pp *links;		/* structures we have I_LINKed */
	size_t refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	union {
		struct {
			major_t cmajor;	/* major device number */
			minor_t cminor;	/* minor device number */
		} dev;
		uint mux;		/* multiplexor index for this structure */
	} id;
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	queue_t *mq;			/* mgmt queue */
	spinlock_t qlock;		/* queue lock */
	queue_t *userq;			/* queue using the queue lock */
	queue_t *waitq;			/* queue waiting on the queue lock */
	uint rbid;			/* rd bufcall id */
	uint wbid;			/* wr bufcall id */
	uint state;			/* interface state */
	uint version;			/* interface version */
	uint type;			/* type of structure */
	union {
		struct sp *sp;		/* AS/SG for user/provider */
		struct xp *xp;		/* default signalling process proxy */
	} u;
	struct {
		uint t1;		/* timer t1 */
		uint t2;		/* timer t2 */
		uint t3;		/* timer t3 */
		uint t4;		/* timer t4 */
		uint t5;		/* timer t5 */
	} timers;			/* M2UA timers */
	lmi_option_t option;		/* LMI protocol variant and options */
	struct {
		bufq_t rb;		/* receive buffer */
		bufq_t tb;		/* transmission buffer */
		bufq_t rtb;		/* retransmission buffer */
		sl_config_t config;	/* SL configuration */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		sdt_config_t config;	/* SDT configuration */
		sdt_notify_t notify;	/* SDT notification options */
		sdt_stats_t stats;	/* SDT statistics */
		sdt_stats_t stamp;	/* SDT statistics timestamps */
		sdt_stats_t statsp;	/* SDT statistics periods */
	} sdt;
	struct {
		sdl_config_t config;	/* SDT configuration */
		sdl_notify_t notify;	/* SDT notification options */
		sdl_stats_t stats;	/* SDT statistics */
		sdl_stats_t stamp;	/* SDT statistics timestamps */
		sdl_stats_t statsp;	/* SDT statistics periods */
	} sdl;
} pp_t;

typedef pp_t dp_t;
typedef pp_t lp_t;

STATIC dp_t *slmua_open_list = NULL;	/* list of all opened streams */
STATIC lp_t *slmua_link_list = NULL;	/* list of all linked streams */

#define PRIV(__q) ((pp_t *)(__q)->q_ptr)

#define SLM_TYPE_NONE	0	/* no type assigned yet */
#define SLM_TYPE_LM	1	/* Layer Management stream */
#define SLM_TYPE_SS7U	2	/* SS7 User stream */
#define SLM_TYPE_SS7P	3	/* SS7 Provider stream */
#define SLM_TYPE_ASP	4	/* SIGTRAN ASP transport stream */
#define SLM_TYPE_SGP	5	/* SIGTRAN SGP transport stream */
#define SLM_TYPE_XPP	6	/* SIGTRAN peer transport stream */

typedef struct xp {
	struct xp *next;		/* list linkage */
	struct xp **prev;		/* list linkage */
	uint refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	struct pp *pp;			/* stream structure for this XP */
	struct gp *gp;			/* graph of SP for this XP */
	struct sp *sp;			/* SP for this XP */
	uint id;			/* ASP Id */
	uint state;			/* state */
	uint level;			/* protocol level */
	uint version;			/* protocol version */
	bufq_t buf;			/* buffer for failovers */
} xp_t;

typedef xp_t asp_t;
typedef xp_t sgp_t;
typedef xp_t spp_t;

STATIC asp_t *slm_asp_list = NULL;	/* list of ASP proxies */
STATIC sgp_t *slm_sgp_list = NULL;	/* list of SGP proxies */
STATIC spp_t *slm_spp_list = NULL;	/* list of SPP proxies */

#define ASP_DOWN	0
#define ASP_INACTIVE	1
#define ASP_ACTIVE	2
#define ASP_WACK_BEAT	3	/* T(beat) timer running */
#define ASP_WACK_ASPUP	4	/* T(ack) timer running */
#define ASP_WACK_ASPDN	5	/* T(ack) timer running */
#define ASP_WACK_ASPAC	6	/* T(ack) timer running */
#define ASP_WACK_ASPIA	7	/* T(ack) timer running */

typedef struct gp {
	struct {
		struct xp *xp;		/* this XP associated with this SP */
		struct gp *next;	/* next XP associated with this SP */
		struct gp **prev;	/* prev XP associated with this SP */
	} xp;
	struct {
		struct sp *sp;		/* this SP associated with this SP */
		struct gp *next;	/* next SP associated with this SP */
		struct gp **prev;	/* prev SP associated with this SP */
	} sp;
	uint refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	uint state;			/* state of this XP in this SP */
	uint loadshare;			/* loadshare selector (stripe) */
	uint broadcast;			/* broadcast selector (mirror) */
	uint priority;			/* priority of this XP for this SP */
	uint cost;			/* cost of using this XP for this SP */
	uint t_beat;			/* timer for heartbeats */
	uint t_ack;			/* timer for ack of messages */
} gp_t;

typedef struct np {
	struct {
		struct sp *as;		/* this AS associated with this SG */
		struct np *next;	/* next AS associated with this SG */
		struct np **prev;	/* prev AS associated with this SG */
	} as;
	struct {
		struct sp *sg;		/* this SG associated with this AS */
		struct np *next;	/* next SG associated with this AS */
		struct np **prev;	/* prev SG associated with this AS */
	} sg;
	uint refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	uint id;			/* id for this AS in this SG (RC/IID) */
	uint state;			/* state of this SG serving this AS */
	uint loadshare;			/* loadshare selector (stripe) */
	uint broadcast;			/* broadcast selector (mirror) */
	uint priority;			/* priority of this SG for this AS */
	uint cost;			/* cost of this SG for this AS */
} np_t;

typedef struct sp {
	struct sp *next;		/* list linkage */
	struct sp **prev;		/* list linkage */
	uint refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	struct np *np;			/* graph of peer nodes for this node */
	struct gp *gp;			/* graph of AS/SG for this node */
	struct pp *pp;			/* list of SS7U/SS7P for this node */
	uint id;			/* ASP Id used or expected */
	uint state;			/* state of htis node */
	uint tmode;			/* traffic mode */
	uint min_count;			/* minimum number of active stripes */
	uint max_count;			/* maximum number of active stripes */
	uint xpdn_count;		/* XPs in down state */
	uint xpia_count;		/* XPs in inactive state */
	uint xpac_count;		/* XPs in active state */
} sp_t;

typedef sp_t as_t;
typedef sp_t sg_t;

#define AS_DOWN		0
#define AS_INACTIVE	1
#define AS_ACTIVE	2
#define AS_PENDING	3
#define AS_BLOCKED	4

/*
 *  -------------------------------------------------------------------------
 *
 *  Private structure allocation and deallocation
 *
 *  -------------------------------------------------------------------------
 */
STATIC kmem_cache_t *slm_pp_cachep = NULL;
STATIC kmem_cache_t *slm_gp_cachep = NULL;
STATIC kmem_cache_t *slm_np_cachep = NULL;
STATIC kmem_cache_t *slm_sp_cachep = NULL;
STATIC kmem_cache_t *slm_xp_cachep = NULL;

/*
 *  Cache allocation
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_term_caches(void)
{
	if (slm_pp_cachep) {
		if (kmem_cache_destroy(slm_pp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy slm_pp_cachep", __FUNCTION__);
		else
			printd(("SLM: destroyed slm_pp_cache\n"));
	}
	if (slm_gp_cachep) {
		if (kmem_cache_destroy(slm_gp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy slm_gp_cachep", __FUNCTION__);
		else
			printd(("SLM: destroyed slm_gp_cache\n"));
	}
	if (slm_np_cachep) {
		if (kmem_cache_destroy(slm_np_cachep))
			cmn_err(CE_WARN, "%s: did not destroy slm_np_cachep", __FUNCTION__);
		else
			printd(("SLM: destroyed slm_np_cache\n"));
	}
	if (slm_sp_cachep) {
		if (kmem_cache_destroy(slm_sp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy slm_sp_cachep", __FUNCTION__);
		else
			printd(("SLM: destroyed slm_sp_cache\n"));
	}
	if (slm_xp_cachep) {
		if (kmem_cache_destroy(slm_xp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy slm_xp_cachep", __FUNCTION__);
		else
			printd(("SLM: destroyed slm_xp_cache\n"));
	}
	return;
}
STATIC int
slm_init_caches(void)
{
	if (!slm_pp_cachep
	    && !(slm_pp_cachep =
		 kmem_cache_create("slm_pp_cachep", sizeof(pp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate slm_pp_cachep", __FUNCTION__);
		slm_term_caches();
		return (-ENOMEM);
	} else
		printd(("SLM: initialized pp structure cache\n"));
	if (!slm_gp_cachep
	    && !(slm_gp_cachep =
		 kmem_cache_create("slm_gp_cachep", sizeof(pp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate slm_gp_cachep", __FUNCTION__);
		slm_term_caches();
		return (-ENOMEM);
	} else
		printd(("SLM: initialized gp structure cache\n"));
	if (!slm_np_cachep
	    && !(slm_np_cachep =
		 kmem_cache_create("slm_np_cachep", sizeof(pp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate slm_np_cachep", __FUNCTION__);
		slm_term_caches();
		return (-ENOMEM);
	} else
		printd(("SLM: initialized np structure cache\n"));
	if (!slm_sp_cachep
	    && !(slm_sp_cachep =
		 kmem_cache_create("slm_sp_cachep", sizeof(pp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate slm_sp_cachep", __FUNCTION__);
		slm_term_caches();
		return (-ENOMEM);
	} else
		printd(("SLM: initialized sp structure cache\n"));
	if (!slm_xp_cachep
	    && !(slm_xp_cachep =
		 kmem_cache_create("slm_xp_cachep", sizeof(pp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate slm_xp_cachep", __FUNCTION__);
		slm_term_caches();
		return (-ENOMEM);
	} else
		printd(("SLM: initialized xp structure cache\n"));
	return (0);
}

/*
 *  Private structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC pp_t *
slm_alloc_priv(queue_t *q, queue_t *mq, pp_t ** ppp, uint index, cred_t *crp, uint type)
{
	pp_t *pp;
	if ((pp = kmem_cache_alloc(slm_pp_cachep, SLAB_ATOMIC))) {
		printd(("SLM: allocated device private structure\n"));
		bzero(pp, sizeof(*pp));
		pp->id.mux = index;
		RD(q)->q_ptr = pp;
		pp->refcnt++;
		WR(q)->q_ptr = pp;
		pp->refcnt++;
		pp->rq = RD(q);
		pp->wq = WR(q);
		pp->mq = mq;
		spin_lock_init(&pp->qlock); /* "pp-queue-lock" */
		pp->userq = NULL;
		pp->waitq = NULL;
		pp->rbid = 0;
		pp->wbid = 0;
		pp->state = LMI_UNATTACHED;
		pp->version = 1;
		pp->type = type;
		spin_lock_init(&pp->lock); /* "pp-priv-lock" */
		if ((pp->next = *ppp)) {
			pp->next->prev = &pp->next;
			pp->refcnt++;
		}
		*ppp = pp;
		pp->refcnt++;
		printd(("SLM: setting private structure defaults\n"));
	} else
		ptrace(("SLM: ERROR: Could not allocate device private structure\n"));
	return (pp);
}
STATIC void
slm_free_priv(queue_t *q)
{
	pp_t *pp = PRIV(q);
	psw_t flags;
	ensure(pp, return);
	spin_lock_irqsave(&pp->lock, flags);
	{
		if (pp->rbid) {
			unbufcall(xchg(&pp->rbid, 0));
			pp->refcnt--;
		}
		if (pp->wbid) {
			unbufcall(xchg(&pp->wbid, 0));
			pp->refcnt--;
		}
		if ((*pp->prev = pp->next)) {
			pp->next->prev = pp->prev;
			pp->refcnt--;
		}
		pp->refcnt--;
		pp->next = NULL;
		pp->prev = NULL;
		RD(q)->q_ptr = NULL;
		pp->refcnt--;
		WR(q)->q_ptr = NULL;
		pp->refcnt--;
	}
	spin_unlock_irqrestore(&pp->lock, flags);
	printd(("SLM: unlinked device private structure\n"));
	if (pp->refcnt) {
		assure(pp->refcnt == 0);
		printd(("SLM: WARNING: pp->refcnt = %d\n", pp->refcnt));
	}
	kmem_cache_free(slm_pp_cachep, pp);
	printd(("SLM: freed device private structure\n"));
	return;
}

/*
 *  AS/SG Node map structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC np_t *
slm_alloc_np(sp_t * as, sp_t * sg)
{
	np_t *mp;
	if ((np = kmem_cache_alloc(slm_np_cachep, SLAB_ATOMIC))) {
		printd(("M2UA: allocated np structure\n"));
		bzero(np, sizeof(*np));
		if ((np->as.next = as->np)) {
			np->as.next->as.prev = &np->as.next;
			np->refcnt++;
		}
		as->np = np;
		np->refcnt++;
		np->as.as = as;
		as->refcnt++;
		np->as.prev = &as->np;
		if ((np->sg.next = sg->np)) {
			np->sg.next->sg.prev = &np->sg.next;
			np->refcnt++;
		}
		sg->np = np;
		np->refcnt++;
		np->sg.sg = sg;
		sg->refcnt++;
		np->sg.prev = &sg->np;
		spin_lock_init(&np->lock);
		np->state = AS_DOWN;
		printd(("SLM: setting np structure defaults\n"));
	} else
		ptrace(("SLM: ERROR: Could not allocate np structure\n"));
	return (np);
}
STATIC void
slm_free_np(np_t * np)
{
	psw_t flags;
	ensure(np, return);
	spin_lock_irqsave(&np->lock, flags);
	{
		if ((*np->as.prev = np->as.next)) {
			np->as.next->as.prev = np->as.prev;
			np->refcnt--;
		}
		np->as.as->refcnt--;
		np->as.as = NULL;
		np->as.next = NULL;
		np->as.prev = NULL;
		if ((*np->sg.prev = np->sg.next)) {
			np->sg.next->sg.prev = np->sg.prev;
			np->refcnt--;
		}
		np->sg.sg->refcnt--;
		np->sg.sg = NULL;
		np->sg.next = NULL;
		np->sg.prev = NULL;
	}
	spin_unlock_irqrestore(&np->lock, flags);
	printd(("SLM unlinked np structure\n"));
	if (np->refcnt) {
		assure(np->refcnt == 0);
		printd(("SLM: WARNING: np->refcnt = %d\n", np->refcnt));
	}
	kmem_cache_free(slm_np_cachep, np);
	printd(("SLM: freed np struture\n"));
	return;
}

/*
 *  XP/SP Graph map structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC gp_t *
slm_alloc_gp(sp_t * sp, xp_t * xp)
{
	gp_t *mp;
	if ((gp = kmem_cache_alloc(slm_gp_cachep, SLAB_ATOMIC))) {
		printd(("SLM: allocated gp structure\n"));
		bzero(gp, sizeof(*gp));
		if ((gp->xp.next = xp->gp)) {
			gp->xp.next->xp.prev = &gp->xp.next;
			gp->refcnt++;
		}
		xp->gp = gp;
		gp->refcnt++;
		gp->xp.xp = xp;
		xp->refcnt++;
		gp->xp.prev = &xp->gp;
		if ((gp->sp.next = sp->gp)) {
			gp->sp.next->sp.prev = &gp->sp.next;
			gp->refcnt++;
		}
		sp->gp = gp;
		gp->refcnt++;
		gp->sp.sp = sp;
		sp->refcnt++;
		gp->sp.prev = &sp->gp;
		spin_lock_init(&gp->lock);
		printd(("SLM: setting gp structure defaults\n"));
		gp->state = AS_DOWN;
	} else
		ptrace(("SLM: ERROR: Could not allocate gp structure\n"));
	return (gp);
}
STATIC void
slm_free_gp(gp_t * gp)
{
	psw_t flags;
	ensure(gp, return);
	spin_lock_irqsave(&gp->lock, flags);
	{
		if ((*gp->xp.prev = gp->xp.next)) {
			gp->xp.next->xp.prev = gp->xp.prev;
			gp->refcnt--;
		}
		gp->xp.xp->refcnt--;
		gp->xp.xp = NULL;
		gp->xp.next = NULL;
		gp->xp.prev = NULL;
		if ((*gp->sp.prev = gp->sp.next)) {
			gp->sp.next->sp.prev = gp->sp.prev;
			gp->refcnt--;
		}
		gp->sp.sp->refcnt--;
		gp->sp.sp = NULL;
		gp->sp.next = NULL;
		gp->sp.prev = NULL;
	}
	spin_unlock_irqrestore(&gp->lock, flags);
	printd(("SLM unlinked gp structure\n"));
	if (gp->refcnt) {
		assure(gp->refcnt == 0);
		printd(("SLM: WARNING: gp->refcnt = %d\n", gp->refcnt));
	}
	kmem_cache_free(slm_gp_cachep, gp);
	printd(("SLM: freed gp struture\n"));
	return;
}

/*
 *  AS/SG structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC sp_t *
slm_alloc_sp(sp_t ** spp)
{
	sp_t *sp;
	if ((sp = kmem_cache_alloc(slm_sp_cachep, SLAB_ATOMIC))) {
		printd(("SLM: allocated sp structure\n"));
		bzero(sp, sizeof(*sp));
		if ((sp->next = *spp)) {
			sp->next->prev = &sp->next;
			sp->refcnt++;
		}
		*spp = sp;
		sp->refcnt++;
		sp->prev = spp;
		spin_lock_init(&sp->lock);
		printd(("SLM: setting sp structure defaults\n"));
		sp->state = AS_DOWN;
		// sp->tmode = UA_TMODE_OVERRIDE;
		sp->min_count = 1;
		sp->max_count = -1UL;
	} else
		ptrace(("SLM: ERROR: Could not allocate sp structure\n"));
	return (sp);
}
STATIC void
slm_free_sp(sp_t * sp)
{
	psw_t flags;
	ensure(sp, return);
	spin_lock_irqsave(&sp->lock, flags);
	{
		if ((*sp->prev = sp->next)) {
			sp->next->prev = sp->prev;
			sp->refcnt--;
		}
		sp->next = NULL;
		sp->prev = NULL;
		while (sp->np)
			slm_free_np(sp->np);
		while (sp->gp)
			slm_free_gp(sp->gp);
		while (sp->pp)
			slm_free_pp(sp->pp);
	}
	spin_unlock_irqrestore(&sp->lock, flags);
	printd(("SLM: unlinked sp structure\n"));
	if (sp->refcnt) {
		assure(sp->refcnt == 0);
		printd(("SLM: WARNING: sp->refcnt = %d\n", sp->refcnt));
	}
	kmem_cache_free(slm_sp_cachep, sp);
	printd(("SLM: freed sp structure\n"));
	return;
}

/*
 *  ASP/SGP/SPP structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC xp_t *
slm_alloc_xp(sp_t * sp)
{
	xp_t *xp;
	if ((xp = kmem_cache_alloc(slm_xp_cachep, SLAB_ATOMIC))) {
		printd(("SLM: allocated xp structure\n"));
		bzero(xp, sizeof(*xp));
		if ((xp->next = *xpp)) {
			xp->next->prev = &xp->next;
			xp->refcnt++;
		}
		*xpp = xp;
		xp->refcnt++;
		xp->prev = xpp;
		xp->sp = sp;
		sp->refcnt++;
		spin_lock_init(&xp->lock);
		bufq_init(&xp->buf);
		printd(("SLM: setting xp structure defaults\n"));
		xp->state = ASP_DOWN;
	} else
		ptrace(("SLM: ERROR: Could not allocate xp structure\n"));
	return (xp);
}
STATIC void
slm_free_xp(xp_t * xp)
{
	psw_t flags;
	ensure(xp, return);
	spin_lock_irqsave(&xp->lock, flags);
	{
		if ((*xp->prev = xp->next)) {
			xp->next->prev = xp->prev;
			xp->refcnt--;
		}
		xp->next = NULL;
		xp->prev = NULL;
		while (xp->gp)
			slm_free_gp(xp->gp);
		if (xp->pp) {
			pp_t *pp = xp->pp;
			pp->u.xp = NULL;
			xp->refcnt--;
			xp->pp = NULL;
			pp->refcnt--;
		}
		if (xp->sp) {
			sp_t *sp = xp->sp;
			xp->sp = NULL;
			sp->refcnt--;
		}
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	printd(("SLM: unlinked xp structure\n"));
	if (xp->refcnt) {
		assure(xp->refcnt == 0);
		printd(("SLM: WARNING: xp->refcnt = %d\n", xp->refcnt));
	}
	kmem_cache_free(slm_xp_cachep, xp);
	printd(("SLM: freed xp structure\n"));
	return;
}

/*
 *  ========================================================================
 *
 *  Locking
 *
 *  ========================================================================
 */
STATIC INLINE int
slm_trylockq(queue_t *q)
{
	int res;
	pp_t *pp = PRIV(q);
	if (!(res = spin_trylock(&pp->qlock))) {
		if (q == pp->rq)
			pp->rwait = q;
		if (q == pp->wq)
			pp->wwait = q;
	}
	return (res);
}
STATIC INLINE void
slm_unlockq(queue_t *q)
{
	pp_t *pp = PRIV(q);
	spin_unlock(&pp->qlock);
	if (pp->rwait)
		qenable(xchg(&pp->rwait, NULL));
	if (pp->wwait)
		qenable(xchg(&pp->wwait, NULL));
}

/*
 *  =========================================================================
 *
 *  BUFFER Allocation
 *
 *  =========================================================================
 */
/*
 *  BUFSRV
 *  -----------------------------------
 */
STATIC void
slm_bufsrv(long data)
{
	queue_t *q = (queue_t *) data;
	if (q) {
		pp_t *pp = PRIV(q);
		if (q == pp->rq)
			if (pp->rbid) {
				pp->rbid = 0;
				pp->refcnt--;
			}
		if (q == pp->wq)
			if (pp->wbid) {
				pp->wbid = 0;
				pp->refcnt--;
			}
		qenable(q);
	}
}

/*
 *  ALLOCB
 *  -----------------------------------
 */
STATIC mblk_t *
slm_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;
	if ((mp = allocb(size, prior)))
		return (mp);
	else {
		pp_t *pp = PRIV(q);
		if (q == pp->rq) {
			if (!pp->rbid) {
				pp->rbid = bufcall(size, prior, &slm_bufsrv, (long) q);
				pp->refcnt++;
			}
			return (NULL);
		}
		if (q == pp->wq) {
			if (!pp->wbid) {
				pp->wbid = bufcall(size, prior, &slm_bufsrv, (long) q);
				pp->refcnt++;
			}
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}

/*
 *  ESBALLOC
 *  -----------------------------------
 */
STATIC mblk_t *
slm_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;
	if ((mp = esballoc(base, size, prior, frtn)))
		return (mp);
	else {
		pp_t *pp = PRIV(q);
		if (q == pp->rq) {
			if (!pp->rbid) {
				pp->rbid = esbbcall(prior, &slm_bufsrv, (long) q);
				pp->refcnt++;
			}
			return (NULL);
		}
		if (q == pp->wq) {
			if (!pp->wbid) {
				pp->wbid = esbbcall(prior, &slm_bufsrv, (long) q);
				pp->refcnt++;
			}
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}

/*
 *  =========================================================================
 *
 *  PRIMITIVES Sent Upstream (Upper) or Downstream (Lower)
 *
 *  =========================================================================
 *
 *  PRIMITIVES Sent Upstream (Upper) to SS7 User
 *  -------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, int err)
{
	mblk_t *mp;
	if ((mp = slm_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, int err)
{
	mblk_t *mp;
	if ((mp = slm_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_PDU_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_pdu_ind(queue_t *q, mblk_t *dp, uint prior)
{
	if (canputnext(q)) {
		mblk_t *mp;
		sl_pdu_ind_t *p;
		if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->sl_primitive = SL_PDU_IND;
			p->sl_mp = prior;
			mp->b_cont = dp;
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congested_ind(queue_t *q, ulong cong, ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congestion_ceased_ind(queue_t *q, ulong cong, ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ceased_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieved_message_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	sl_retrieved_msg_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_cont = dp;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_complete_ind(queue_t *q)
{
	mblk_t *mp;
	sl_retrieval_comp_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_rb_cleared_ind(queue_t *q)
{
	mblk_t *mp;
	sl_rb_cleared_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RB_CLEARED_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_bsnt_ind(queue_t *q, ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_in_service_ind(queue_t *q)
{
	mblk_t *mp;
	sl_in_service_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_IN_SERVICE_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_out_of_service_ind(queue_t *q, ulong reason)
{
	mblk_t *mp;
	sl_out_of_service_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_outage_ind(queue_t *q)
{
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_recovered_ind(queue_t *q)
{
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_rtb_cleared_ind(queue_t *q)
{
	mblk_t *mp;
	sl_rtb_cleared_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_not_possible_ind(queue_t *q)
{
	mblk_t *mp;
	sl_retrieval_not_poss_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_bsnt_not_retrievable_ind(queue_t *q, ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_optmgmt_ack(queue_t *q, caddr_t opt_ptr, size_t opt_len, ulong flags)
{
	mblk_t *mp;
	sl_optmgmt_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_notify_ind(queue_t *q)
{
	mblk_t *mp;
	sl_notify_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_NOTIFY_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, caddr_t ppa_ptr, size_t ppa_len)
{
	pp_t *pp = PRIV(q);
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = pp->state;
		p->lmi_max_sdu = INFPSZ;
		p->lmi_min_sdu = 3;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, ulong state, long prim)
{
	pp_t *pp = PRIV(q);
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		p->lmi_state = pp->state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, ulong state, long prim, ulong errno, ulong reason)
{
	pp_t *pp = PRIV(q);
	mblk_t *mp;
	lmi_error_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		p->lmi_state = pp->state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		pp_t *pp = PRIV(q);
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = pp->state = LMI_ENABLED;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_con(queue_t *q)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		pp_t *pp = PRIV(q);
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = pp->state = LMI_DISABLED;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, ulong errno, ulong reason)
{
	pp_t *pp = PRIV(q);
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = pp->state;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q, ulong interval)
{
	if (canputnext(q)) {
		mblk_t *mp;
		lmi_stats_ind_t *p;
		if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, ulong oid, ulong level)
{
	if (canputnext(q)) {
		mblk_t *mp;
		lmi_event_ind_t *p;
		if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = level;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  PRIMITIVES Sent Upstream (Upper) to Layer Management
 *  -------------------------------------------------------------------------
 */
/*
 *  LM_LINK_IND
 *  -----------------------------------
 *  Indicates to management that an indication has been received on a linked
 *  stream which it is incapable of handling.  We normally do not use this.
 *  It is more typical to place the indication back on the stream and issue an
 *  LM_ERROR_IND to management to let management know that the stream should
 *  be unlinked and dealt with directly.  We should consider tunneling IOCTLs
 *  instead of these mechanisms.
 */
STATIC INLINE int
lm_link_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	lm_link_ind_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->prim = LM_LINK_IND;
		mp->b_cont = dp;
		putnext(q, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LM_OK_ACK
 *  -----------------------------------
 *  Used by the driver to indicate success of a primitive issued by local or
 *  layer management.
 */
STATIC INLINE int
lm_ok_ack(queue_t *q, ulong prim)
{
	mblk_t *mp;
	lm_ok_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->prim = LM_OK_ACK;
		p->correct_prim = prim;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LM_ERROR_ACK
 *  -----------------------------------
 *  Used by the driver to indicate failure of a primitive issues by local or
 *  layer management.
 */
STATIC INLINE int
lm_error_ack(queue_t *q, ulong prim, ulong type, ulong error)
{
	mblk_t *mp;
	lm_error_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->prim = LM_ERROR_ACK;
		p->error_prim = prim;
		p->lm_error = type;
		p->unix_error = error;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LM_REG_IND
 *  -----------------------------------
 *  Indicate to management that a registration requires has been received from
 *  an ASP with the specified protocol id and ASP id and the indicated routing
 *  keys.  If the registration request is acceptable, management responds with
 *  an LM_REG_RES indicating the configured application server by interface
 *  id.  If the registration request fails, management responds with an
 *  LM_REG_REF refusing the request.
 */
STATIC INLINE int
lm_reg_ind(queue_t *q, ulong index, ulong id, ulong load, caddr_t key_ptr, size_t key_len)
{
	if (canputnext(q, mp)) {
		mblk_t *mp;
		lm_reg_ind_t *p;
		if ((mp = slm_allocb(q, sizeof(*p) + key_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->prim = LM_REG_IND;
			p->muxid = index;
			p->aspid = id;
			p->load = load;
			p->prio = prio;
			p->KEY_number = keyno;
			p->KEY_offset = key_len ? sizeof(*p) : 0;
			p->KEY_length = key_len;
			bcopy(key_ptr, mp->b_wptr, key_len);
			mp->b_wptr += key_len;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LM_DEREG_IND
 *  -----------------------------------
 *  Indicate to management that a deregistration  has been received from an
 *  ASP which was previously registered.
 */
STATIC INLINE int
lm_dereg_ind(queue_t *q, ulong index)
{
	if (canputnext(q, mp)) {
		mblk_t *mp;
		lm_dereg_ind_t *p;
		if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->prim = LM_DEREG_IND;
			p->muxid = index;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LM_ERROR_IND
 *  -----------------------------------
 *  Indicate to mangement that an error has occured associated with a link.
 */
STATIC INLINE int
lm_error_ind(queue_t *q, ulong type, ulong error)
{
	mblk_t *mp;
	lm_error_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->prim = LM_ERROR_IND;
		p->lm_error = type;
		p->unix_error = error;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LM_INFO_ACK
 *  -----------------------------------
 *  Inidcate requested information to management.
 */
STATIC INLINE int
lm_info_ack(queue_t *q, ulong id)
{
	mblk_t *mp;
	lm_info_ack_t *p;
	if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->prim = LM_INFO_ACK;
		p->spid = id;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  PRIMITIVES Sent Downstream (Lower) to Transport
 *  -------------------------------------------------------------------------
 */
/*
 *  T_DATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_data_req(queue_t *q, mblk_t *dp, uint more)
{
	if (canputnext(q)) {
		mblk_t *mp;
		struct T_data_req *p;
		if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_cont = dp;
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_exdata_req(queue_t *q, mblk_t *dp, uint more)
{
	if (bcanputnext(q, 1)) {
		mblk_t *mp;
		struct T_exdata_req *p;
		if ((mp = slm_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_cont = dp;
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  T_OPTDATA_REQ
 *  -------------------------------------------
 */
STATIC INLINE int
t_optdata_req(queue_t *q, mblk_t *dp, uint flags, caddr_t opt_ptr, size_t opt_len)
{
	if (((flags & T_ODF_EX) && bcanputnext(q, 1)) || (!(flags & T_ODF_EX) && canputnext(q))) {
		mblk_t *mp;
		struct T_optdata_req *p;
		if ((mp = slm_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = (flags & T_ODF_EX) ? 1 : 0;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M2UA MESSAGE Encoder
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_send_data(queue_t *q, mblk_t *mp)
{
	return slm_maup_data(q, -1, mp->b_cont);
}

/*
 *  SL_PDU_REQ:
 *  -----------------------------------
 */
STATIC int
slm_pdu_req(queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	return slm_maup_data(q, p->sl_message_priority, mp->b_cont);
}

/*
 *  SL_EMERGENCY_REQ:
 *  -----------------------------------
 */
STATIC int
slm_emergency_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_EMER_SET);
}

/*
 *  SL_EMERGENCY_CEASES_REQ:
 *  -----------------------------------
 */
STATIC int
slm_emergency_ceases_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_stats_req(q, M2UA_STATUS_EMERG_CLEAR);
}

/*
 *  SL_START_REQ:
 *  -----------------------------------
 */
STATIC int
slm_start_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_estab_req(q);
}

/*
 *  SL_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
slm_stop_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_rel_req(q);
}

/*
 *  SL_RETRIEVE_BSNT_REQ:
 *  -----------------------------------
 */
STATIC int
slm_retrieve_bsnt_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_retr_req(q, M2UA_ACTION_RTRV_BSN, 0);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
 *  -----------------------------------
 */
STATIC int
slm_retrieval_request_and_fsnc_req(queue_t *q, mblk_t *mp)
{
	sl_retrieval_request_and_fsnc_req_t *p;
	return slm_maup_retr_req(q, M2UA_ACTION_RTRV_MSGS, p->sl_fsnc);
}

/*
 *  SL_RESUME_REQ:
 *  -----------------------------------
 */
STATIC int
slm_resume_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_LPO_CLEAR);
}

/*
 *  SL_CLEAR_BUFFERS_REQ:
 *  -----------------------------------
 */
STATIC int
slm_clear_buffers_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_FLUSH_BUFFERS);
}

/*
 *  SL_CLEAR_RTB_REQ:
 *  -----------------------------------
 */
STATIC int
slm_clear_rtb_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_CLEAR_RTB);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ:
 *  -----------------------------------
 */
STATIC int
slm_local_processor_outage_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_LPO_SET);
}

/*
 *  SL_CONGESTION_DISCARD_REQ:
 *  -----------------------------------
 */
STATIC int
slm_congestion_discard_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_CONG_DISCARD);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ:
 *  -----------------------------------
 */
STATIC int
slm_congestion_accept_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_CONG_ACCEPT);
}

/*
 *  SL_NO_CONGESTION_REQ:
 *  -----------------------------------
 */
STATIC int
slm_no_congestion_req(queue_t *q, mblk_t *mp)
{
	return slm_maup_state_req(q, M2UA_STATUS_CONG_CLEAR);
}

/*
 *  SL_POWER_ON_REQ:
 *  -----------------------------------
 */
STATIC int
slm_power_on_req(queue_t *q, mblk_t *mp)
{
	return (QR_DONE);	/* not necessary? */
}

/*
 *  SL_OPTMGMT_REQ:
 *  -----------------------------------
 */
STATIC int
slm_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NOTIFY_REQ:
 *  -----------------------------------
 */
STATIC int
slm_notify_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_INFO_REQ:
 *  -----------------------------------
 */
STATIC int
slm_info_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_ATTACH_REQ:
 *  -----------------------------------
 */
STATIC int
slm_attach_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_DETACH_REQ:
 *  -----------------------------------
 */
STATIC int
slm_detach_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_ENABLE_REQ:
 *  -----------------------------------
 */
STATIC int
slm_enable_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_DISABLE_REQ:
 *  -----------------------------------
 */
STATIC int
slm_disable_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_OPTMGMT_REQ:
 *  -----------------------------------
 */
STATIC int
slm_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_PDU_IND:
 *  -----------------------------------
 */
STATIC int
slm_pdu_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_LINK_CONGESTED_IND:
 *  -----------------------------------
 */
STATIC int
slm_link_congested_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND:
 *  -----------------------------------
 */
STATIC int
slm_link_congestion_ceased_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RETRIEVED_MESSAGE_IND:
 *  -----------------------------------
 */
STATIC int
slm_retrieved_message_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND:
 *  -----------------------------------
 */
STATIC int
slm_retrieval_complete_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RB_CLEARED_IND:
 *  -----------------------------------
 */
STATIC int
slm_rb_cleared_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_BSNT_IND:
 *  -----------------------------------
 */
STATIC int
slm_bsnt_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_IN_SERVICE_IND:
 *  -----------------------------------
 */
STATIC int
slm_in_service_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_OUT_OF_SERVICE_IND:
 *  -----------------------------------
 */
STATIC int
slm_out_of_service_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND:
 *  -----------------------------------
 */
STATIC int
slm_remote_processor_outage_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND:
 *  -----------------------------------
 */
STATIC int
slm_remote_processor_recovered_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RTB_CLEARED_IND:
 *  -----------------------------------
 */
STATIC int
slm_rtb_cleared_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND:
 *  -----------------------------------
 */
STATIC int
slm_retrieval_not_possible_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND:
 *  -----------------------------------
 */
STATIC int
slm_bsnt_not_retrievable_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_OPTMGMT_ACK:
 *  -----------------------------------
 */
STATIC int
slm_optmgmt_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NOTIFY_IND:
 *  -----------------------------------
 */
STATIC int
slm_notify_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_INFO_ACK:
 *  -----------------------------------
 */
STATIC int
slm_info_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_OK_ACK:
 *  -----------------------------------
 */
STATIC int
slm_ok_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_ERROR_ACK:
 *  -----------------------------------
 */
STATIC int
slm_error_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_ENABLE_CON:
 *  -----------------------------------
 */
STATIC int
slm_enable_con(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_DISABLE_CON:
 *  -----------------------------------
 */
STATIC int
slm_disable_con(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_OPTMGMT_ACK:
 *  -----------------------------------
 */
STATIC int
slm_optmgmt_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_ERROR_IND:
 *  -----------------------------------
 */
STATIC int
slm_error_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_STATS_IND:
 *  -----------------------------------
 */
STATIC int
slm_stats_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_EVENT_IND:
 *  -----------------------------------
 */
STATIC int
slm_event_ind(queue_t *q, mblk_t *mp)
{
}

STATIC int
slm_t_w_data(queue_t *q, mblk_t *mp)
{
	return slm_send_data(q, mp);
}
STATIC int
slm_t_w_proto(queue_t *q, mblk_t *mp)
{
	ulong prim = (*(ulong *) mp->b_rptr);
	if (pp->type == SLM_TYPE_XPP || pp->type = SLM_TYPE_SGP) {
		switch (prim) {
		case SL_PDU_REQ:
			return slm_pdu_req(q, mp);
		case SL_EMERGENCY_REQ:
			return slm_emergency_req(q, mp);
		case SL_EMERGENCY_CEASES_REQ:
			return slm_emergency_ceases_req(q, mp);
		case SL_START_REQ:
			return slm_start_req(q, mp);
		case SL_STOP_REQ:
			return slm_stop_req(q, mp);
		case SL_RETRIEVE_BSNT_REQ:
			return slm_retrieve_bsnt_req(q, mp);
		case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
			return slm_retrieval_request_and_fsnc_req(q, mp);
		case SL_RESUME_REQ:
			return slm_resume_req(q, mp);
		case SL_CLEAR_BUFFERS_REQ:
			return slm_clear_buffers_req(q, mp);
		case SL_CLEAR_RTB_REQ:
			return slm_clear_rtb_req(q, mp);
		case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
			return slm_local_processor_outage_req(q, mp);
		case SL_CONGESTION_DISCARD_REQ:
			return slm_congestion_discard_req(q, mp);
		case SL_CONGESTION_ACCEPT_REQ:
			return slm_congestion_accept_req(q, mp);
		case SL_NO_CONGESTION_REQ:
			return slm_no_congestion_req(q, mp);
		case SL_POWER_ON_REQ:
			return slm_power_on_req(q, mp);
		case SL_OPTMGMT_REQ:
			return slm_optmgmt_req(q, mp);
		case SL_NOTIFY_REQ:
			return slm_notify_req(q, mp);
		case LMI_INFO_REQ:
			return slm_info_req(q, mp);
		case LMI_ATTACH_REQ:
			return slm_attach_req(q, mp);
		case LMI_DETACH_REQ:
			return slm_detach_req(q, mp);
		case LMI_ENABLE_REQ:
			return slm_enable_req(q, mp);
		case LMI_DISABLE_REQ:
			return slm_disable_req(q, mp);
		case LMI_OPTMGMT_REQ:
			return slm_optmgmt_req(q, mp);
		}
	}
	if (pp->type == SLM_TYPE_XPP || pp->type == SLM_TYPE_ASP) {
		switch (prim) {
		case SL_PDU_IND:
			return slm_pdu_ind(q, mp);
		case SL_LINK_CONGESTED_IND:
			return slm_link_congested_ind(q, mp);
		case SL_LINK_CONGESTION_CEASED_IND:
			return slm_link_congestion_ceased_ind(q, mp);
		case SL_RETRIEVED_MESSAGE_IND:
			return slm_retrieved_message_ind(q, mp);
		case SL_RETRIEVAL_COMPLETE_IND:
			return slm_retrieval_complete_ind(q, mp);
		case SL_RB_CLEARED_IND:
			return slm_rb_cleared_ind(q, mp);
		case SL_BSNT_IND:
			return slm_bsnt_ind(q, mp);
		case SL_IN_SERVICE_IND:
			return slm_in_service_ind(q, mp);
		case SL_OUT_OF_SERVICE_IND:
			return slm_out_of_service_ind(q, mp);
		case SL_REMOTE_PROCESSOR_OUTAGE_IND:
			return slm_remote_processor_outage_ind(q, mp);
		case SL_REMOTE_PROCESSOR_RECOVERED_IND:
			return slm_remote_processor_recovered_ind(q, mp);
		case SL_RTB_CLEARED_IND:
			return slm_rtb_cleared_ind(q, mp);
		case SL_RETRIEVAL_NOT_POSSIBLE_IND:
			return slm_retrieval_not_possible_ind(q, mp);
		case SL_BSNT_NOT_RETRIEVABLE_IND:
			return slm_bsnt_not_retrievable_ind(q, mp);
		case SL_OPTMGMT_ACK:
			return slm_optmgmt_ack(q, mp);
		case SL_NOTIFY_IND:
			return slm_notify_ind(q, mp);
		case LMI_INFO_ACK:
			return slm_info_ack(q, mp);
		case LMI_OK_ACK:
			return slm_ok_ack(q, mp);
		case LMI_ERROR_ACK:
			return slm_error_ack(q, mp);
		case LMI_ENABLE_CON:
			return slm_enable_con(q, mp);
		case LMI_DISABLE_CON:
			return slm_disable_con(q, mp);
		case LMI_OPTMGMT_ACK:
			return slm_optmgmt_ack(q, mp);
		case LMI_ERROR_IND:
			return slm_error_ind(q, mp);
		case LMI_STATS_IND:
			return slm_stats_ind(q, mp);
		case LMI_EVENT_IND:
			return slm_event_ind(q, mp);
		}
	}
	swerr();
	return (-EOPNOTSUPP);
}

/*
 *  LM_SG_ADD_REQ:
 *  -----------------------------------
 *  Requests that the driver add or augment the signalling gateway for the
 *  protocol on whose user or control queue the request was issued.  The SG
 *  identifier is for ruther reference (i.e, for subsequent ADD and DEL
 *  requests).  The layer or local management queue normally uses this
 *  primitive to add a signalling gateway which does not yet exist.  This is
 *  normally only done on a node acting as an ASP.  The traffic mode describes
 *  the traffic mode to be used across the SGP making up the SG.  For ASP
 *  operation, the aspid is used to indicate the identifier that the ASP will
 *  use when it goes ASPUP for the given SG.  Only one ASP is ever assocated
 *  with an SG instance in this way.
 */
STATIC int
lm_sg_add_req(queue_t *q, mblk_t *mp)
{
	int err;
	sg_t *sg;
	as_t *as;
	np_t *np;
	pp_t *pp = PRIV(q);
	lm_sg_add_req_t *p = ((typeof(p)) mp->b_rptr);
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto emsgsize;
	if (pp->cred.cr_uid != 0)
		goto eperm;
	if (!(as = pp->u.sp))
		goto efault;
	/*
	   see if we already have the sg in question 
	 */
	for (as = pp->u.sp; as; as = as->next) {
		for (np = as->np; np; np = np->sg.next) {
			if (pp->sg.sg->id == p->sgid) {
				/*
				   we already have one 
				 */
				if ((err = lmi_ok_ack(q, LM_SG_ADD_REQ)))
					return (err);
				sg->id = p->sgid;
				sg->tmode = p->tmode;
				sg->min_count = 1;
				sg->max_count = -1UL;
				for (np = sg->np; np; np = np->as.next) {
					np->loadshare = p->load;
					np->prior = p->prior;
					np->cost = p->cost;
				}
				return (QR_DONE);
			}
		}
	}
	/*
	   create an np to link new sg 
	 */
	if (!(sg = slm_alloc_sp(&slm_sg_list)))
		goto enomem;
	sg->id = p->sgid;
	sg->state = AS_DOWN;
	sg->tmode = p->tmode;
	sg->min_count = 1;
	sg->max_count = -1UL;
	/*
	   automatically link the SG to each AS 
	 */
	for (as = pp->u.sp; as; as = as->next) {
		if (!(np = slm_alloc_np(as, sg)))
			goto free_enomem;
		np->id = 0;
		np->state = AS_DOWN;
		np->loadshare = p->load;
		np->priority = p->prior;
		np->cost = p->cost;
	}
	if ((err = lm_ok_ack(q, LM_SG_ADD_REQ)))
		goto free_err;
	return (QR_DONE);
      free_err:
	slm_free_sp(sg);
	return (err);
      free_enomem:
	slm_free_sp(sg);
      enomem:
	rare();
	return (-ENOMEM);
      emsgsize:
	return lm_error_ack(q, LM_SG_ADD_REQ, LMI_SYSERR, EMSGSIZE);
      eperm:
	return lm_error_ack(q, LM_SG_ADD_REQ, LMI_SYSERR, EPERM);
      efault:
	swerr();
	return lm_error_ack(q, LM_SG_ADD_REQ, LMI_SYSERR, EFAULT);
}

/*
 *  LM_SG_DEL_REQ:
 *  -----------------------------------
 *  Requests that the driver delete the signalling gateway for the protocol on
 *  whose control queue the request was issued.  The SG identifier specifies
 *  the existing SG to be deleted.  The layer or local management queue
 *  normally uses this rpimitive to delte a signalling gateway which was
 *  previously added with LM_SG_ADD_REQ.  If the specified SG does not exist,
 *  the request is silently ignored.
 */
STATIC int
lm_sg_del_req(queue_t *q, mblk_t *mp)
{
	int err;
	as_t *as;
	sg_t *sg;
	np_t *np;
	pp_t *pp = PRIV(q);
	lm_sg_del_req_t *p = ((typeof(p)) mp->b_wptr);
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto emsgsize;
	if (pp->cred.cr_uid != 0)
		goto eperm;
	for (as = pp->u.sp; as; as = as->next) {
		for (np = as->np; np; np = np->sg.next) {
			if ((sg = np->sg.sg) && sg->id == p->sgid) {
				if ((err = lm_ok_ack(q, LM_SG_DEL_REQ)))
					return (err);
				slm_free_sp(sg);
				return (QR_DONE);
			}
		}
	}
	return lm_ok_ack(q, LM_SG_DEL_REQ);
      emsgsize:
	return lm_error_ack(q, LM_SG_DEL_REQ, LMI_SYSERR, EMSGSIZE);
}

/*
 *  LM_AS_ADD_REQ:
 *  -----------------------------------
 *  Requests that the driver add or augment the application server for the
 *  protocol on whose control queue the request was issued.  The AS identifier
 *  is for further reference (i.e, a subsequent ADD or DEL request).  The RC
 *  or IID identifier will be added to the AS if the AS already exists with
 *  the specified AS Id.
 *
 *  The layer or local management queue normally uses this primitive to add an
 *  application server which does not yet exist at boot time or in response to
 *  the LM_REG_REQ before issuing an LM_REG_RES.
 *
 *  The optional routing (link) key is only required for statically allocated
 *  Application Servers.  This key is used to bind/connect SS7 provider
 *  streams which support the AS or to generate REG REQ to SGP/SPP 
 *  proxies.  I don't really intend to use this function, but it is there to
 *  support all SIGTRAN UA allocation modes.
 *
 *  The ASID is provided to uniquely identify the AS within the ASP.
 *
 *  The RC/IID is only required for statically allocated AS.  That is, if
 *  dynamic registration is desired, the RC/IID value should be zero.
 *
 *  The SGID is provided to provision an AS on an ASP which is serviced by a
 *  remote SG.  If the SGID is zero, all SGs which are available to the ASP
 *  will be used for this AS.
 *
 *  The MUXID is provided to provision an AS on an SGP which is serviced by a
 *  local SS7 provider.  The MUXID is the identifier of the local SS7 provider
 *  stream.  If there is no local SS7 provider stream, MUXID should be zero.
 */
STATIC int
lm_as_add_req(queue_t *q, mblk_t *mp)
{
	lm_as_add_req_t *p = ((typeof(p)) mp->b_rptr);
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr - mp->b_rptr < sizeof(*p) + p->KEY_length)
		goto emsgsize;
	if (pp->cred.cr_uid != 0)
		goto eperm;
	{
		int err;
		pp_t *pp = PRIV(q);
		as_t *as;
		np_t *np;
		/*
		   look for an AS with the same id 
		 */
		for (as = pp->u.sp; as; as = as->next) {
			if (as->id == p->asid) {
				if ((err = lm_ok_ack(q, LM_AS_ADD_REQ)))
					return (err);
			}
		}
	}
      emsgsize:
	return lm_error_ack(q, LM_AS_ADD_REQ, LMI_SYSERR, EMSGSIZE);
      eperm:
	return lm_error_ack(q, LM_AS_ADD_REQ, LMI_SYSERR, EPERM);
}

/*
 *  LM_AS_DEL_REQ:
 *  -----------------------------------
 */
STATIC int
lm_as_del_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_PROC_ADD_REQ:
 *  -----------------------------------
 */
STATIC int
lm_proc_add_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_PROC_DEL_REQ:
 *  -----------------------------------
 */
STATIC int
lm_proc_del_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_LINK_ADD_REQ:
 *  -----------------------------------
 */
STATIC int
lm_link_add_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_LINK_DEL_REQ:
 *  -----------------------------------
 */
STATIC int
lm_link_del_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_ROUTE_ADD_REQ:
 *  -----------------------------------
 */
STATIC int
lm_route_add_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_ROUTE_DEL_REQ:
 *  -----------------------------------
 */
STATIC int
lm_route_del_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_REG_RES:
 *  -----------------------------------
 */
STATIC int
lm_reg_res(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_REG_REF:
 *  -----------------------------------
 */
STATIC int
lm_reg_ref(queue_t *q, mblk_t *mp)
{
}

/*
 *  LM_INFO_REQ:
 *  -----------------------------------
 */
STATIC int
lm_info_req(queue_t *q, mblk_t *mp)
{
}

STATIC int
lm_m_w_proto(queue_t *q, mblk_t *mp)
{
	ulong prim = (*(ulong *) mp->b_rptr);
	switch (prim) {
	case LM_SG_ADD_REQ:
		return lm_sg_add_req(q, mp);
	case LM_SG_DEL_REQ:
		return lm_sg_del_req(q, mp);
	case LM_AS_ADD_REQ:
		return lm_as_add_req(q, mp);
	case LM_AS_DEL_REQ:
		return lm_as_del_req(q, mp);
	case LM_PROC_ADD_REQ:
		return lm_proc_add_req(q, mp);
	case LM_PROC_DEL_REQ:
		return lm_proc_del_req(q, mp);
	case LM_LINK_ADD_REQ:
		return lm_link_add_req(q, mp);
	case LM_LINK_DEL_REQ:
		return lm_link_del_req(q, mp);
	case LM_ROUTE_ADD_REQ:
		return lm_route_add_req(q, mp);
	case LM_ROUTE_DEL_REQ:
		return lm_route_del_req(q, mp);
	case LM_REG_RES:
		return lm_reg_res(q, mp);
	case LM_REG_REF:
		return lm_reg_ref(q, mp);
	case LM_INFO_REQ:
		return lm_info_req(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */
enum { t1, t2, t3, t4, t5 };
STATIC INLINE mblk_t *
slm_alloc_timeout(ulong timer)
{
	mblk_t *mp;
	if ((mp = allocb(sizeof(ulong), BPRI_HI))) {
		mp->b_datap->db_type = M_PCRSE;
		*((ulong) mp->b_wptr)++ = timer;
	}
	return (mp);
}
STATIC void
slm_t1_timeout(caddr_t data)
{
	pp_t *pp = (pp_t *) data;
	if (xchg(&pp->timers.t1, 0)) {
		mblk_t *mp;
		if ((mp = slm_alloc_timeout(t1))) {
			printd(("SLM: t1 timeout at %lu\n", jiffies));
			pp->rput(pp->rq, mp);
		} else {
			__printd(("SLM: t1 timeout collision\n"));
			pp->timers.t1 = timeout(&slm_t1_timeout, (cadd_t) pp, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
slm_t2_timeout(caddr_t data)
{
	pp_t *pp = (pp_t *) data;
	if (xchg(&pp->timers.t2, 0)) {
		mblk_t *mp;
		if ((mp = slm_alloc_timeout(t2))) {
			printd(("SLM: t2 timeout at %lu\n", jiffies));
			pp->rput(pp->rq, mp);
		} else {
			__printd(("SLM: t2 timeout collision\n"));
			pp->timers.t2 = timeout(&slm_t2_timeout, (cadd_t) pp, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
slm_t4_timeout(caddr_t data)
{
	pp_t *pp = (pp_t *) data;
	if (xchg(&pp->timers.t4, 0)) {
		mblk_t *mp;
		if ((mp = slm_alloc_timeout(t4))) {
			printd(("SLM: t4 timeout at %lu\n", jiffies));
			pp->rput(pp->rq, mp);
		} else {
			__printd(("SLM: t4 timeout collision\n"));
			pp->timers.t4 = timeout(&slm_t4_timeout, (cadd_t) pp, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
slm_t4_timeout(caddr_t data)
{
	pp_t *pp = (pp_t *) data;
	if (xchg(&pp->timers.t4, 0)) {
		mblk_t *mp;
		if ((mp = slm_alloc_timeout(t4))) {
			printd(("SLM: t4 timeout at %lu\n", jiffies));
			pp->rput(pp->rq, mp);
		} else {
			__printd(("SLM: t4 timeout collision\n"));
			pp->timers.t4 = timeout(&slm_t4_timeout, (cadd_t) pp, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
slm_t5_timeout(caddr_t data)
{
	pp_t *pp = (pp_t *) data;
	if (xchg(&pp->timers.t5, 0)) {
		mblk_t *mp;
		if ((mp = slm_alloc_timeout(t5))) {
			printd(("SLM: t5 timeout at %lu\n", jiffies));
			pp->rput(pp->rq, mp);
		} else {
			__printd(("SLM: t5 timeout collision\n"));
			pp->timers.t5 = timeout(&slm_t5_timeout, (cadd_t) pp, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC INLINE void
slm_timer_stop(queue_t *q, const uint t)
{
	pp_t *pp = PRIV(q);
	psw_t flags;
	spin_lock_irqsave(&pp->lock, flags);
	{
		switch (t) {
		case t1:
			if (pp->timers.t1) {
				printd(("SLM: stopping t1 at %lu\n", jiffies));
				untimeout(xchg(&pp->timers.t1, 0));
			}
			break;
		case t2:
			if (pp->timers.t2) {
				printd(("SLM: stopping t2 at %lu\n", jiffies));
				untimeout(xchg(&pp->timers.t2, 0));
			}
			break;
		case t3:
			if (pp->timers.t3) {
				printd(("SLM: stopping t3 at %lu\n", jiffies));
				untimeout(xchg(&pp->timers.t3, 0));
			}
			break;
		case t4:
			if (pp->timers.t4) {
				printd(("SLM: stopping t4 at %lu\n", jiffies));
				untimeout(xchg(&pp->timers.t4, 0));
			}
			break;
		case t5:
			if (pp->timers.t5) {
				printd(("SLM: stopping t5 at %lu\n", jiffies));
				untimeout(xchg(&pp->timers.t5, 0));
			}
			break;
		}
	}
	spin_unlock_irqrestore(&pp->lock, flags);
}
STATIC INLINE void
slm_timer_start(queue_t *q, const uint t)
{
	pp_t *pp = PRIV(q);
	psw_t flags;
	spin_lock_irqsave(&pp->lock, flags);
	{
		slm_timer_stop(q, t);
		switch (t) {
		case t1:
			printd(("SLM: starting t1 %lu ms at %lu\n", pp->config.t1 * 10, jiffies));
			pp->timers.t1 = timeout(&slm_t1_timeout, (caddr_t) pp, pp->config.t1);
			break;
		case t2:
			printd(("SLM: starting t2 %lu ms at %lu\n", pp->config.t2 * 10, jiffies));
			pp->timers.t2 = timeout(&slm_t2_timeout, (caddr_t) pp, pp->config.t2);
			break;
		case t3:
			printd(("SLM: starting t3 %lu ms at %lu\n", pp->config.t3 * 10, jiffies));
			pp->timers.t3 = timeout(&slm_t3_timeout, (caddr_t) pp, pp->config.t3);
			break;
		case t4:
			printd(("SLM: starting t4 %lu ms at %lu\n", pp->config.t4 * 10, jiffies));
			pp->timers.t4 = timeout(&slm_t4_timeout, (caddr_t) pp, pp->config.t4);
			break;
		case t5:
			printd(("SLM: starting t5 %lu ms at %lu\n", pp->config.t5 * 10, jiffies));
			pp->timers.t5 = timeout(&slm_t5_timeout, (caddr_t) pp, pp->config.t5);
			break;
		}
	}
	spin_unlock_irqrestore(&pp->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SLM State Machines
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  Events from below
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  SLM events from below (timeouts)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T1 EXPIRY
 *  -----------------------------------
 */
STATIC int
slm_t1_expiry(queue_t *q)
{
	pp_t *pp = PRIV(q);
	(void) pp;
	fixme(("SLM: FIXME: Implement this function\n"));
	return (-EFAULT);
}

/*
 *  T2 EXPIRY
 *  -----------------------------------
 */
STATIC int
slm_t2_expiry(queue_t *q)
{
	pp_t *pp = PRIV(q);
	(void) pp;
	fixme(("SLM: FIXME: Implement this function\n"));
	return (-EFAULT);
}

/*
 *  T3 EXPIRY
 *  -----------------------------------
 */
STATIC int
slm_t3_expiry(queue_t *q)
{
	pp_t *pp = PRIV(q);
	(void) pp;
	fixme(("SLM: FIXME: Implement this function\n"));
	return (-EFAULT);
}

/*
 *  T4 EXPIRY
 *  -----------------------------------
 */
STATIC int
slm_t4_expiry(queue_t *q)
{
	pp_t *pp = PRIV(q);
	(void) pp;
	fixme(("SLM: FIXME: Implement this function\n"));
	return (-EFAULT);
}

/*
 *  T5 EXPIRY
 *  -----------------------------------
 */
STATIC int
slm_t5_expiry(queue_t *q)
{
	pp_t *pp = PRIV(q);
	(void) pp;
	fixme(("SLM: FIXME: Implement this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M2UA Message Decoder
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_t_r_data(q, mp)
{
}
STATIC int
slm_t_r_proto(q, mp)
{
	switch (*(ulong *) mp->b_rptr) {
	case T_DISCON_IND:
	case T_ORDREL_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_OPTDATA_IND:
	default:
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Transport Events from Below
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  T_DATA_IND
 *  -----------------------------------
 */
STATIC int
t_data_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_EXDATA_IND
 *  -----------------------------------
 */
STATIC int
t_exdata_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_OPTDATA_IND
 *  -----------------------------------
 */
STATIC int
t_optdata_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  =========================================================================
 *
 *  EVENTS From Above
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  EVENTS From SS7 User
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
slm_send_data(queue_t *q, mblk_t *mp)
{
}

/*
 *  Any SL Primitive
 *  -----------------------------------
 */
STATIC int
sl_prim(q, mp, int (*prim) (queue_t *, mblk_t *))
{
	pp_t *pp = PRIV(q);
	if (pp->state != LMI_ENABLED) {
		pp->state = LMI_UNUSABLE;
		return m_error(q, EPROTO);
	}
	if (pp->link && pp->link->state == LMI_ENABLED) {
		queue_t *lq = (q == pp->wq) ? pp->link->wq : pp->link->rq;
		if (mp->b_datap->db_type == M_PCPROTO || canput(lq)) {
			slm_wput(lq, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-EBUSY);
	}
	return prim(q, mp);
}

/*
 *  SL SEND DATA
 *  -----------------------------------
 */
STATIC int
sl_send_data(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
sl_pdu_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC int
sl_emergency_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC int
sl_emergency_ceases_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC int
sl_start_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC int
sl_stop_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieve_bsnt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RETREIVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC int
sl_retreival_request_and_fsnc_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC int
sl_resume_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_buffers_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_rtb_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC int
sl_local_processor_outage_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC int
sl_congestion_discard_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC int
sl_congestion_accept_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC int
sl_no_congestion_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC int
sl_power_on_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
sl_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
STATIC int
sl_notify_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  Any LMI Request
 *  -----------------------------------
 */
STATIC int
lmi_request(q, mp, int (*request) (queue_t *, mblk_t *))
{
	pp_t *pp = PRIV(q);
	if (pp->link && pp->link->state == LMI_ENABLED) {
		queue_t *wq = pp->link->wq;
		if (mp->b_datap->db_type == M_PCPROTO || canput(wq)) {
			slm_wput(wq, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-EBUSY);
	}
	return request(q, mp);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  LMI_OPMGMT_REQ
 *  -----------------------------------
 */
STATIC int
lmi_opmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  -------------------------------------------------------------------------
 *
 *  EVENTS From Layer Management
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  MUX Linking and Unlinking
 *
 *  =========================================================================
 */
STATIC pp_t *slm_links = NULL;
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_i_link(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		pp_t *pp = PRIV(q), *lp;
		struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
		struct linkblk *lb = (typeof(lb)) mp->b_cont->b_rptr;
		queue_t *lq = lb->l_qbot;
		if (lq->q_ptr != NULL)
			return (QR_DONE);	/* already linked */
		if ((lp =
		     slm_alloc_priv(lq, q, &slmlinks, lb->l_index, iocp->ioc_cr, SLM_TYPE_NONE))) {
			noenable(RD(lq));
			noenable(WR(lq));
			return (QR_DONE);
		}
		return (-ENOMEM);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  I_PLINK
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_i_plink(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		pp_t *pp = PRIV(q);
		struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
		struct linkblk *lb = (typeof(lb)) mp->b_cont->b_rptr;
		if (iocp->ioc_cr->cr_uid != 0)
			return (-EPERM);
		return slm_i_link(q, mp);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  I_UNLINK
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_i_unlink(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		pp_t *pp = PRIV(q), *lp;
		struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
		struct linkblk *lb = (typeof(lb)) mp->b_cont->b_rptr;
		queue_t *lq = lb->l_qbot;
		if (!(lp = lq->q_ptr))
			return (QR_DONE);	/* already unlinked */
		fixme(("SLM: FIXME: check credentials before unlinking\n"));
		enableok(RD(lq));
		enableok(WR(lq));
		slm_free_priv(lq);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  I_PUNLINK
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_i_punlink(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		pp_t *pp = PRIV(q);
		struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
		struct linkblk *lb = (typeof(lb)) mp->b_cont->b_rptr;
		if (iocp->ioc_cr->cr_uid != 0)
			return (-EPERM);
		return slm_i_unlink(q, mp);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  M_IOCTL Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_w_ioctl(queue_t *q, mblk_t *mp)
{
	pp_t *pp = PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	switch (type) {
	case __SID:
		switch (pp->type) {
		case SLM_TYPE_LM:
		case SLM_TYPE_SS7U:
			switch (nr) {
			case _IOC_NR(I_LINK):
				ret = slm_i_link(q, mp);
				break;
			case _IOC_NR(I_PLINK):
				ret = slm_i_plink(q, mp);
				break;
			case _IOC_NR(I_UNLINK):
				ret = slm_i_unlink(q, mp);
				break;
			case _IOC_NR(I_PUNLINK):
				ret = slm_i_punlink(q, mp);
				break;
			default:
				ptrace(("SLM: ERROR: Unknwon IOCTL %d\n", nr));
			case _IOC_NR(I_STR):
				rare();
				(void) lp;
				ret = -EINVAL;
				break;
			}
			break;
		default:
			swerr();
			ret = (-EFAULT);
			break;
		}
		break;
	default:
		return (QR_PASSALONG);
	}
	if (ret >= 0) {
		mp->b_datap->db_type = M_IOCACK;
		ioc->ioc_error = 0;
		ioc->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		ioc->ioc_error = -ret;
		ioc->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  M_PROTO, M_PCPROTO Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	pp_t *pp = PRIV(q);
	ulong oldstate = pp->state;
	switch (pp->type) {
	case SLM_TYPE_SS7U:
		if ((rtn = slm_u_w_proto(q, mp)) != -EOPNOTSUPP)
			break;
	case SLM_TYPE_LM:
		rtn = slm_m_w_proto(q, mp);
		break;
	case SLM_TYPE_SS7P:
		rtn = slm_p_w_proto(q, mp);
		break;
	case SLM_TYPE_ASP:
	case SLM_TYPE_SGP:
	case SLM_TYPE_XPP:
		rtn = slm_t_w_proto(q, mp);
		break;
	case SLM_TYPE_NONE:
		rtn = (QR_DISABLE);
		break;
	default:
		swerr();
		rtn = (-EFAULT);
		break;
	}
	if (rtn < 0)
		pp->state = oldstate;
	return (rtn);
}
STATIC int
slm_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	pp_t *pp = PRIV(q);
	ulong oldstate = pp->state;
	switch (pp->type) {
	case SLM_TYPE_LM:
		rtn = slm_m_r_proto(q, mp);
		break;
	case SLM_TYPE_SS7U:
		rtn = slm_u_r_proto(q, mp);
		break;
	case SLM_TYPE_SS7P:
		rtn = slm_p_r_proto(q, mp);
		break;
	case SLM_TYPE_ASP:
	case SLM_TYPE_SGP:
	case SLM_TYPE_XPP:
		rtn = slm_t_r_proto(q, mp);
		break;
	case SLM_TYPE_NONE:
		rtn = (QR_DISABLE);
		break;
	default:
		swerr();
		rtn = (-EFAULT);
		break;
	}
	if (rtn < 0)
		pp->state = oldstate;
	return (rtn);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_w_data(queue_t *q, mblk_t *mp)
{
	pp_t *pp = PRIV(q);
	switch (pp->type) {
	case SLM_TYPE_LM:
		return slm_m_w_data(q, mp);
	case SLM_TYPE_SS7U:
		return slm_u_w_data(q, mp);
	case SLM_TYPE_SS7P:
		return slm_p_w_data(q, mp);
	case SLM_TYPE_ASP:
	case SLM_TYPE_SGP:
	case SLM_TYPE_XPP:
		return slm_t_w_data(q, mp);
	case SLM_TYPE_NONE:
		return (QR_DISABLE);
	default:
		swerr();
		return (-EFAULT);
	}
}
STATIC int
slm_r_data(queue_t *q, mblk_t *mp)
{
	pp_t *pp = PRIV(q);
	switch (pp->type) {
	case SLM_TYPE_LM:
		return slm_m_r_data(q, mp);
	case SLM_TYPE_SS7U:
		return slm_u_r_data(q, mp);
	case SLM_TYPE_SS7P:
		return slm_p_r_data(q, mp);
	case SLM_TYPE_ASP:
	case SLM_TYPE_SGP:
	case SLM_TYPE_XPP:
		return slm_t_r_data(q, mp);
	case SLM_TYPE_NONE:
		return (QR_DISABLE);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  M_RSE, M_PCRSE Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
slm_r_pcrse(queue_t *q, mblk_t *mp)
{
	pp_t *pp = PRIV(q);
	int rtn;
	psw_t flags;
	spin_lock_irqsave(&pp->lock, flags);
	switch (*(ulong *) mp->b_rptr) {
	case t1:
		printd(("SLM: t1 expiry at %lu\n", jiffies));
		rtn = sl_t1_expiry(q);
		break;
	case t2:
		printd(("SLM: t2 expiry at %lu\n", jiffies));
		rtn = sl_t2_expiry(q);
		break;
	case t3:
		printd(("SLM: t3 expiry at %lu\n", jiffies));
		rtn = sl_t3_expiry(q);
		break;
	case t4:
		printd(("SLM: t4 expiry at %lu\n", jiffies));
		rtn = sl_t4_expiry(q);
		break;
	case t5:
		printd(("SLM: t5 expiry at %lu\n", jiffies));
		rtn = sl_t5_expiry(q);
		break;
	default:
		rtn = -EFAULT;
		break;
	}
	spin_unlock_irqrestore(&pp->lock, flags);
	return (rtn);
}

/*
 *  M_FLUSH Handling
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
slm_w_flush(queue_t *q, mblk_t *mp)
{
	pp_t *pp = PRIV(q);
	fixme(("SLM: FIXME: Make this work for SLM\n"));
	if (*mp->b_rptr & FLUSHW) {
		if (pp) {
			psw_t flags;
			spin_lock_irqsave(&pp->lock, flags);
			if (pp->tx.cmp) {
				pp->tx.cmp = NULL;
				pp->tx.repeat = 0;
			}
			spin_unlock_irqrestore(&pp->lock, flags);
		}
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (pp) {
			psw_t flags;
			spin_lock_irqsave(&pp->lock, flags);
			if (pp->rx.cmp) {
				freeb(pp->rx.cmp);
				pp->rx.cmp = NULL;
				pp->rx.repeat = 0;
			}
			spin_unlock_irqrestore(&pp->lock, flags);
		}
		if (*mp->b_rptr & FLUSHBAND)
			flushband(OTHERQ(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(OTHERQ(q), FLUSHALL);
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
slm_r_prim(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_DATA)
		return slm_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return slm_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return slm_w_proto(q, mp);
	case M_FLUSH:
		return slm_w_flush(q, mp);
	case M_IOCTL:
		return slm_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
slm_w_prim(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_DATA)
		return slm_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return slm_r_data(q, mp);
	case M_RSE:
	case M_PCRSE:
		return slm_r_pcrse(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return slm_r_proto(q, mp);
	case M_FLUSH:
		return slm_r_flush(q, mp);
	}
}

STATIC int
slm_rput(queue_t *q, mblk_t *mp)
{
	return (int) slm_putq(q, mp, &slm_r_prim);
}

STATIC int
slm_rsrv(queue_t *q)
{
	return (int) slm_srvq(q, &slm_r_prim);
}

STATIC int
slm_wput(queue_t *q, mblk_t *mp)
{
	return (int) slm_putq(q, mp, &slm_w_prim);
}

STATIC int
slm_wsrv(queue_t *q)
{
	return (int) slm_srvq(q, &slm_w_prim);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *  Open is called on the first open of a character special device stream
 *  head; close is called on the last close of the same device.
 */
pp_t *x400p_list = NULL;

STATIC int
slm_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	pp_t *pp, **ppp = &x400p_list;
	(void) crp;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("SLM: ERROR: Can't open as module\n"));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN) {
		printd(("SLM: Clone open in effect on major %d\n", cmajor));
		cminor = 1;
	}
	for (; *ppp && (*ppp)->id.dev.cmajor < cmajor; ppp = &(*ppp)->next) ;
	for (; *ppp && cminor < NMINORS; ppp = &(*ppp)->next) {
		minor_t dminor = (*ppp)->id.dev.cminor;
		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN) {
				ptrace(("SLM: ERROR: Requested device in use\n"));
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
			cminor++;
		}
	}
	if (cminor >= NMINORS) {
		ptrace(("SLM: ERROR: No device minors left\n"));
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("SLM: Opened character device %d:%d\n", cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(pp = slm_alloc_priv(q, q, ppp, (uint) (*devp), crp, SLM_TYPE_NONE))) {
		ptrace(("SLM: ERROR: No memory\n"));
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	return (0);
}
STATIC int
slm_close(queue_t *q, int flag, cred_t *crp)
{
	pp_t *pp = PRIV(q);
	(void) flag;
	(void) crp;
	(void) pp;
	printd(("SLM: Closed character device %d:%d\n", pp->id.dev.cmajor, pp->id.dev.cminor));
	slm_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization (For unregistered driver.)
 *
 *  =========================================================================
 */
STATIC int slm_initialized = 0;
STATIC int slm_majors[SLM_CMAJORS] = { 0, };
STATIC void
slm_init(void)
{
	int err, major;
	unless(slm_initialized, return);
	cmn_err(CE_NOTE, SLM_BANNER);	/* console splash */
	if ((err = slm_init_caches())) {
		cmn_err(CE_PANIC, "SLM: ERROR: Could not allocate caches");
		slm_initialized = err;
		return;
	}
	for (major = 0; major < SLM_CMAJORS; major++) {
		if ((err =
		     lis_register_strdev(SLM_CMAJOR_0 + major, &slm_info, UNITS,
					 DRV_NAME)) <= 0) {
			cmn_err(CE_WARN, "SLM: ERROR: couldn't register driver for major %d",
				major + SLM_CMAJOR_0);
			slm_initialized = err;
			for (major -= 1; major >= 0; major--)
				lis_unregister_strdev(slm_majors[major]);
			slm_term_caches();
			return;
		} else
			slm_majors[major] = err;
	}
	slm_initialized = SLM_CMAJOR_0;
	return;
}
STATIC void
slm_terminate(void)
{
	int err, major;
	ensure(slm_initialized, return);
	for (major = 0; major < SLM_CMAJORS; major++) {
		if (slm_majors[major]) {
			if ((err = lis_unregister_strdev(slm_majors[major])))
				cmn_err(CE_PANIC, "SLM: couldn't unregister driver for major %d\n",
					slm_majors[major]);
			else
				slm_majors[major] = err;
		}
	}
	slm_initialized = 0;
	slm_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int
init_module(void)
{
	slm_init();
	if (slm_initialized < 0)
		return slm_initialized;
	return (0);
}

void
cleanup_module(void)
{
	slm_terminate();
}
