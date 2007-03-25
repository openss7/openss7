/*****************************************************************************

 @(#) $RCSfile: rtp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:56 $

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

 Last Modified $Date: 2007/03/25 00:51:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: rtp.c,v $
 Revision 0.9.2.2  2007/03/25 00:51:56  brian
 - synchronization updates

 Revision 0.9.2.1  2006/03/30 10:44:22  brian
 - added first cut of RTP driver

 *****************************************************************************/

#ident "@(#) $RCSfile: rtp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:56 $"

static char const ident[] = "$RCSfile: rtp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:56 $";

/*
 *  This driver provides the functionality of an RTP (Realtime Transport
 *  Protocol) hooked over UDP within the Linux kernel.  The driver is used
 *  primarily by the OpenSS7 protocol stack for Voice over IP (VoIP).  The
 *  driver can also operation with the NPI IP driver linked underneath it as a
 *  multiplexing driver.
 */

#include <sys/os8/compat.h>

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

#include "rtp_hooks.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>

#define RTP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define RTP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define RTP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define RTP_REVISION	"OpenSS7 $RCSfile: rtp.c,v $ $Name:  $ ($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:56 $"
#define RTP_DEVICE	"SVR 4.2 STREAMS RTP Driver"
#define RTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define RTP_LICENSE	"GPL"
#define RTP_BANNER	RTP_DESCRIP	"\n" \
			RTP_EXTRA	"\n" \
			RTP_REVSISION	"\n" \
			RTP_COPYRIGHT	"\n" \
			RTP_DEVICE	"\n" \
			RTP_CONTACT
#define RTP_SPLASH	RTP_DESCRIP	" - " \
			RTP_REVISION

#ifdef LINUX
MODULE_AUTHOR(RTP_CONTACT);
MODULE_DESCRIPTION(RTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(RTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(RTP_LICENSE);
#endif
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-rtp");
#endif
#endif				/* LINUX */

#ifdef LFS
#define RTP_DRV_ID	CONFIG_STREAMS_RTP_MODID
#define RTP_DRV_NAME	CONFIG_STREAMS_RTP_NAME
#define RTP_CMAJORS	CONFIG_STREAMS_RTP_NMAJORS
#define RTP_CMAJOR_0	CONFIG_STREAMS_RTP_MAJOR
#define RTP_UNITS	CONFIG_STREAMS_RTP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_RTP_MODID));
MODULE_ALIAS("streams-driver-rtp");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_RTP_MAJOR));
MODULE_ALIAS("/dev/streams/rtp");
MODULE_ALIAS("/dev/streams/rtp/*");
MODULE_ALIAS("/dev/streams/clone/rtp");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(RTP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(RTP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(RTP_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(RTP_CMAJOR_0) "-" __stringify(RTP_CMINOR));
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID	    RTP_DRV_ID
#define DRV_NAME    RTP_DRV_NAME
#define CMAJORS	    RTP_CMAJORS
#define CMAJOR_0    RTP_CMAJOR_0
#define UNITS	    RTP_UNITS
#ifdef MODULE
#define DRV_BANNER  RTP_BANNER
#else				/* MODULE */
#define DRV_NAMMER  RTP_SPLASH
#endif				/* MODULE */

STATIC struct module_info rtp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC streamscall int rtp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int rtp_close(queue_t *, int, cred_t *);

STATIC struct qinit rtp_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = rtp_open,		/* Each open */
	.qi_qclose = rtp_close,		/* Last close */
	.qi_minfo = &rtp_minfo,		/* Information */
};

STATIC struct qinit rtp_winit = {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_minfo = &rtp_minfo,		/* Information */
};

STATIC struct module_info ip_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC struct qinit ip_rinit = {
	.qi_putp = ss7_iput,		/* Read put (msg from below) */
	.qi_srvp = ss7_iput,		/* Read queue service */
	.qi_minfo = &ip_minfo,		/* Information */
};

STATIC struct qinit ip_winit = {
	.qi_putp = ss7_oput,		/* Write put (msg from above) */
	.qi_srvp = ss7_oput,		/* Write queue service */
	.qi_minfo = &ip_minfo,		/* Information */
};

MODULE_STATIC struct streamtab rtpinfo = {
	.st_rdinit = &rtp_rinit,	/* Upper read queue */
	.st_wrinit = &rtp_winit,	/* Upper write queue */
	.st_muxrinit = &ip_rinit,	/* Lower read queue */
	.st_muxwinit = &ip_winit,	/* Lower write queue */
};

/*
 *  Primary data structure
 */
typedef struct rtp {
	STR_DECLARATION (struct rtp);	/* stream declaration */
} rtp_t;

#define RTP_PRIV(__q) = ((struct rtp *)((__q)->q_ptr))

/*
 *  NPI IP user
 */
typedef struct nip {
	STR_DECLARATION (struct npi);	/* stream declaration */
} nip_t;

#define NIP_PRIV(__q) = ((struct nip *)((__q)->q_ptr))

/*
 *  Default
 */
typdef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (rtp, rtp);		/* master list of rtp (open) structures */
	SLIST_HEAD (nip, nip);		/* master list of nip (link) structures */
} df_t;

STATIC struct df master;

/*
 *  =========================================================================
 *
 *  Private structure allocation, deallocation and cache
 *
 *  =========================================================================
 *
 *  It would be a good idea to implement rmalloc() in the compatibility library
 *  and use that function for memory caches.  That way we could retain
 *  independence from Linux in this regard.  Note also, however, that
 *  kmem_cache_create(), kmem_cache_alloc(), kmem_cache_free(), and
 *  kmem_cache_destroy() where first Solaris functions (in fact the entire
 *  approach comes from a paper writen by someone working for Sun.)  Therefore,
 *  at least there aren't many GPL issues here...
 */
STATIC kmem_cache_t *rtp_open_cachep = NULL;
STATIC kmem_cache_t *rtp_link_cachep = NULL;

/**
 * rtp_term_caches: - terminate memory caches for private structures
 */
STATIC int
rtp_term_caches(void)
{
	int err = 0;

	if (rtp_link_cachep) {
		if (kmem_cache_destroy(rtp_link_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy rtp_link_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed rtp_link_cachep\n", DRV_NAME));
	}
	if (rtp_open_cachep) {
		if (kmem_cache_destroy(rtp_open_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy rtp_open_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed rtp_open_cachep\n", DRV_NAME));
	}
	return (err);
}

/**
 * rtp_term_caches: - initialize memory caches for private structures
 */
STATIC int
rtp_init_caches(void)
{
	if (!rtp_open_cachep
	    && !(rtp_open_cachep =
		 kmem_cache_create("rtp_open_cachep", sizeof(struct rtp), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate rtp_open_cachep", DRV_NAME);
		rtp_term_caches();
		return (-ENOMEM);
	} else
		printd(("%s: initialized rtp open structure cache\n", DRV_NAME));
	if (!rtp_link_cachep
	    && @(rtp_link_cachep =
		 kmem_cache_create("rtp_link_cachep", sizeof(struct nip), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate rtp_link_cachep", DRV_NAME);
		rtp_term_caches();
		return (-ENOMEM);
	} else
		printd(("%s: initialized rtp link structure cache\n", DRV_NAME));
	return (0);
}

/**
 * rtp_get: - grab a reference to a private structure
 * @rtp: private structure to grab
 */
STATIC struct rtp *
rtp_get(struct rtp *rtp)
{
	assure(rtp);
	if (rtp)
		atomic_inc(&rtp->refcnt);
	return (rtp);
}

/**
 * rtp_put: - relinquish a reference to a private structure
 * @rtp: private struture to release
 */
STATIC void
rtp_put(struct rtp *rtp)
{
	if (rtp && atomic_dec_and_test(&rtp->refcnt)) {
		kmem_cache_free(rtp_open_cachep, rtp);
		printd(("%s: %s: %p: deallocated rtp structure\n", DRV_NAME, __FUNCTION__, rtp));
	}
}

/**
 * rtp_alloc_open: - allocate a private structure for open
 * @q: active queue in queue pair (read queue)
 * @rtpp: place to insert structure in opens list
 * @devp: pointer to used device number
 * @crp: pointer to opening process credentials
 */
STATIC struct rtp *
rtp_alloc_open(queue_t *q, struct rtp **rtpp, dev_t *devp, cred_t *crp)
{
	struct rtp *rtp;
	pl_t pl;

	printd(("%s: %s: create rtp dev = %d:%d\n", DRV_NAME, __FUNCTION__, getmajor(*devp),
		getminor(*devp)));
	if ((rtp = kmem_cache_alloc(rtp_open_cachep, SLAB_ATOMIC))) {
		bzero(rtp, sizeof(*rtp));
		rtp_get(rtp);	/* first get */
		rtp->u.dev.cmajor = getmajor(*devp);
		rtp->u.dev.cminor = getminor(*devp);
		rtp->cred = *crp;
		spin_lock_init(&rtp->qlock);	/* "rtp-queue-lock" */
		(rtp->oq = RD(q))->q_ptr = rtp_get(rtp);
		(rtp->iq = WR(q))->q_ptr = rtp_get(rtp);
		rtp->o_prim = &rtp_r_prim;
		rtp->i_prim = &rtp_w_prim;
		rtp->o_wakeup = NULL;
		rtp->i_wakeup = NULL;
		rtp->i_state = LMI_UNUSABLE;
		rtp->i_style = LMI_STYLE1;
		rtp->i_version = 1;
		spin_lock_init(&rtp->lock);	/* "rtp-lock" */
		pl = RW_WRLOCK(&master.lock, plstr);
		{
			/* place in master list */
			if ((rtp->next = *rtpp))
				rtp->next->prev = &rtp->next;
			rtp->prev = rtpp;
			*rtpp = rtp_get(rtp);
			master.rtp.numb++;
		}
		RW_UNLOCK(&master.lock, pl);
	} else
		ptrace(("%s: %s: ERROR: failed to allocat rtp structure\n", DRV_NAME,
			__FUNCTION__));
	return (rtp);
}

/**
 * rtp_free_open: - deallocate a private structure for close
 * @q: active queue in queue pair (read queue)
 */
STATIC struct rtp *
rtp_free_open(queue_t *q)
{
	struct rtp *rtp = RTP_PRIV(q);
	pl_t pl;

	pl = LOCK(&rtp->lock, plstr);
	{
		/* stopping bufcalls */
		ss7_unbufcall((str_t *) rtp);
		/* flushing buffers */
		flushq(rtp->oq, FLUSHALL);
		flushq(rtp->iq, FLUSHALL);
		/* remove from master list */
		RW_WRLOCK(&master.lock, pl);
		{
			if ((*rtp->prev = rtp->next))
				rtp->next->prev = rtp->prev;
			rtp->next = NULL;
			rtp->prev = &rtp->next;
			ensure(atomic_read(&rtp->refcnt) > 1, rtp_get(rtp));
			rtp_put(rtp);
			assure(master.rtp.numb > 0);
			master.rtp.numb--;
		}
		RW_UNLOCK(&master.lock, pl);
		/* remove from queues */
		ensure(atomic_read(&rtp->refcnt) > 1, rtp_get(rtp));
		rtp_put(XCHG(&rtp->oq->q_ptr, NULL));
		ensure(atomic_read(&rtp->refcnt) > 1, rtp_get(rtp));
		rtp_put(XCHG(&rtp->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&rtp->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: rtp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, rtp, atomic_read(&rtp->refcnt)));
			atomic_set(&rtp->refcnt, 1);
		}
	}
	UNLOCK(&rtp->lock, pl);
	rtp_put(rtp);		/* final put */
}

/**
 * nip_get: - grab a reference to a private structure
 * @nip: private structure to grab
 */
STATIC struct nip *
nip_get(struct nip *nip)
{
	assure(nip);
	if (nip)
		atomic_inc(&nip->refcnt);
	return (nip);
}

/**
 * nip_put: - relinquish a reference to a private structure
 * @nip: private struture to release
 */
STATIC void
nip_put(struct nip *nip)
{
	if (nip && atomic_dec_and_test(&nip->refcnt)) {
		kmem_cache_free(nip_open_cachep, nip);
		printd(("%s: %s: %p: deallocated nip structure\n", DRV_NAME, __FUNCTION__, rtp));
	}
}

/**
 * rtp_alloc_link: - allocate a private structure for link
 * @q: active queue in queue pair (read queue)
 * @nipp: place to insert structure in links list
 * @index: link index number
 * @crp: point to linking process credentials
 */
STATIC struct nip *
rtp_alloc_link(queue_t *q, struct nip **nipp, ulong index, cred_t *crp)
{
	struct nip *nip;
	pl_t pl;

	printd(("%s: %s: create nip index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((nip = kmem_cache_alloc(rtp_link_cachep, SLAB_ATOMIC))) {
		bzero(nip, sizeof(*nip));
		nip_get(nip);	/* first get */
		nip->u.mux.index = index;
		nip->cred = *crp;
		spin_lock_init(&nip->qlock);	/* "nip-queue-lock" */
		(nip->iq = RD(q))->q_ptr = nip_get(nip);
		(nip->oq = WR(q))->q_ptr = nip_get(nip);
		nip->o_prim = npi_w_prim;
		nip->i_prim = npi_r_prim;
		nip->o_wakeup = NULL;
		nip->i_wakeup = NULL;
		nip->i_state = LMI_UNUSABLE;
		nip->i_style = LMI_STYLE1;
		nip->i_version = 1;
		spin_lock_init(&nip->lock);	/* "nip-lock" */
		pl = RW_WRLOCK(&master.lock, plstr);
		{
			/* place in master list */
			if ((nip->prev = *nipp))
				nip->next->prev = &nip->next;
			nip->prev = nipp;
			*nipp = nip_get(nip);
			master.nip.numb++;
		}
		RW_UNLOCK(&master.lock, pl);
	} else
		printd(("%s: %s: ERROR: failed to allocate nip structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (nip);
}

/**
 * rtp_free_link: - deallocate a private structure for unlink
 * @q: active queue in queue pair (read queue)
 */
STATIC void
rtp_free_link(queue_t *q)
{
	struct nip *nip = NIP_PRIV(q);
	pl_t pl;

	pl = LOCK(&nip->lock, plstr);
	{
		/* flushing buffers */
		ss7_unbufcall((str_t *) nip);
		flushq(nip->iq, FLUSHALL);
		flushq(nip->oq, FLUSHALL);
		/* remove from master list */
		RW_WRLOCK(&master.lock, pl);
		{
			if ((*nip->prev = nip->next))
				nip->next->prev = nip->prev;
			nip->next = NULL;
			nip->prev = &nip->next;
			ensure(atomic_read(&nip->refcnt) > 1, nip_get(nip));
			nip_put(nip);
			assure(master.nip.numb > 0);
			master.nip.numb--;
		}
		RW_UNLOCK(&master.lock, pl);
		/* remove from queues */
		ensure(atomic_read(&nip->refcnt) > 1, nip_get(nip));
		nip_put(XCHG(&nip->iq->q_ptr, NULL));
		ensure(atomic_read(&nip->refcnt) > 1, nip_get(nip));
		nip_put(XCHG(&nip->oq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&nip->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: nip lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, nip, atomic_read(&nip->refcnt)));
			atomic_set(&nip->refcnt, 1);
		}
	}
	UNLOCK(&nip->lock, pl);
	nip_put(nip);		/* final put */
}

/*
 *  MX-primitives sent upstream (upper mux)
 */

/*
 *  N-primitives sent downstream (lower mux)
 */

/**
 * n_info_req: - send an N_INFO_REQ donwstream
 * @q: active queue in queue pair (write queue)
 */
STATIC INLINE fastcall int
n_info_req(queue_t *q)
{
	mblk_t *mp;
	N_info_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_req_t *) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		putnext(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * n_optmgmt_req: - send an N_OPTMGMT_REQ donwstream
 * @q: active queue in queue pair (write queue)
 */
STATIC INLINE fastcall int
n_optmgmt_req(queue_t *q)
{
	mblk_t *mp;
	N_optmgmt_req_t *p;
}

/**
 * n_bind_req: - send an N_BIND_REQ donwstream
 * @q: active queue in queue pair (write queue)
 */
STATIC INLINE fastcall int
n_bind_req(queue_t *q)
{
	mblk_t *mp;
	N_bind_req_t *p;
}

/**
 * n_unitdata_req: - send an N_UNITDATA_REQ donwstream
 * @q: active queue in queue pair (upper write queue)
 * @dst: destination address (and port)
 * @src: source address (and port)
 * @dp: data payload (beyond UDP header)
 *
 * There are three approaches here that might seem to work.  The first approach
 * is to set DEST_length to zero (0) to indicate to the NPI IP layer that the
 * entire IP header is included in the payload.  Another is to set DEST_length
 * and use RESERVED_field[0] and RESERVED_field[1] to indicate QOS_length and
 * QOS_offset and pass a N_QOS_SEL_UD_IP (N_qos_sel_ud_ip_t) QOS structure using
 * those fields.  In the QOS structure is the source address, tos, ttl, etc.
 * Then the IP header is not included in the payload.  Yet another is to set
 * DEST_length but set RESERVED_field's to zero.  The NPI IP layer would use the
 * last N_OPTMGMT_REQ setting of TOS and TTL, and would simply use the source
 * address associated with the route.  This last approach is best (fastest and
 * most portable).  Therefore, we only include the UDP header.
 * 
 */
STATIC INLINE fastcall int
n_unitdata_req(queue_t *q, struct sockaddr_in *dst, struct sockaddr_in *src, mblk_t *dp)
{
	queue_t *bq = npi_bottom;
	struct udphdr *uh;

	if (bq == NULL)
		goto enetdown;
	if (canput(bq)) {
		mblk_t *mp, *bp = NULL;
		N_unitdata_req_t *p;
		struct sockaddr_in *sin;

		if ((mp = ss7_allocb(q, sizeof(*p) + sizeof(*sin), BPRI_MED))) {
			struct rtp *rtp = RTP_PRIV(q);

			if (dp->b_rptr < dp->b_datap->db_base + sizeof(*uh)) {
				/* shoot, someone didn't give use enough headroom */
				if (!(bp = ss7_allocb(q, sizeof(*uh), BPRI_MED))) {
					freemsg(mp);
					return (-ENOBUFS);
				}
				bp->b_datap->db_type = M_DATA;
				bp->b_wptr += sizeof(*uh);
				bp->b_cont = dp;
			} else {
				/* enough room, use it */
				dp->b_rptr -= sizeof(*uh);
				bp = dp;
			}

			mp->b_datap->db_type = M_PROTO;
			p = (N_unitdata_req_t *) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_REQ;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field[0] = 0;
			p->RESERVED_field[1] = 0;
			mp->b_wptr += sizeof(*p);
			sin = (struct sockaddr_in *) mp->b_wptr;
			sin->sin_family = dst->sin_family;
			sin->sin_port = IPPROTO_UDP; /* NPI IP port number is protocol */
			sin->sin_addr.s_addr = dst->sin_addr.s_addr;
			mp->b_wptr += sizeof(*sin);

			bp->b_datap->db_type = M_DATA;
			uh = (struct udphdr *) bp->b_rptr;
			uh->dest = dst->sin_port;
			uh->source = src->sin_port;
			uh->checksum = 0;	/* no checksumming yet */

			mp->b_cont = bp;
			put(bq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	/* Note: the bottom mux, when flow control subsides, must manually enable all feeding
	   Streams across the multiplexer. */
	return (-EBUSY);
      enetdown:
	return (QR_DONE);	/* just discard it for now: FIXME */
}

/**
 * n_unbind_req: - send an N_UNBIND_REQ donwstream
 * @q: active queue in queue pair (write queue)
 */
STATIC INLINE fastcall int
n_unbind_req(queue_t *q)
{
	mblk_t *mp;
	N_unbind_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_unbind_req_t *)mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		npi_set_state(NIP_PRIV(q), NS_WACK_UREQ);
		putnext(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * n_return_msg: - return an M_CTL/N_UNITDATA_IND donwstream
 * @q: active queue in queue pair (read queue)
 * @mp: the message to return
 */
STATIC INLINE fastcall int
n_return_msg(queue_t *q, mblk_t *mp)
{
	if (bcanputnext(OTHERQ(q), mp->b_band)) {
		mp->b_datap->db_type = M_CTL;
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE); /* discard if flow controlled */
}
/*
 *  MX-primitives from above
 */

/**
 * mx_info_req: - process an MX_INFO_REQ message
 * @q: write queue
 * @mp: the MX_INFO_REQ message
 */
STATIC INLINE fastcall int
mx_info_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_optmgmt_req: - process an MX_OPTMGMT_REQ message
 * @q: write queue
 * @mp: the MX_OPTMGMT_REQ message
 */
STATIC INLINE fastcall int
mx_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_attach_req: - process an MX_ATTACH_REQ message
 * @q: write queue
 * @mp: the MX_ATTACH_REQ message
 */
STATIC INLINE fastcall int
mx_attach_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_enable_req: - process an MX_ENABLE_REQ message
 * @q: write queue
 * @mp: the MX_ENABLE_REQ message
 */
STATIC INLINE fastcall int
mx_enable_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_connect_req: - process an MX_CONNECT_REQ message
 * @q: write queue
 * @mp: the MX_CONNECT_REQ message
 */
STATIC INLINE fastcall int
mx_connect_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_data_req: - process an MX_DATA_REQ message
 * @q: write queue
 * @mp: the MX_DATA_REQ message
 */
STATIC INLINE fastcall int
mx_data_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_disconnect_req: - process an MX_DISCONNECT_REQ message
 * @q: write queue
 * @mp: the MX_DISCONNECT_REQ message
 */
STATIC INLINE fastcall int
mx_disconnect_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_disable_req: - process an MX_DISABLE_REQ message
 * @q: write queue
 * @mp: the MX_DISABLE_REQ message
 */
STATIC INLINE fastcall int
mx_disable_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_detach_req: - process an MX_DETACH_REQ message
 * @q: write queue
 * @mp: the MX_DETACH_REQ message
 */
STATIC INLINE fastcall int
mx_detach_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_other_req: - process an unrecognized message
 * @q: write queue
 * @mp: the unrecognized message
 */
STATIC INLINE fastcall int
mx_other_req(queue_t *q, mblk_t *mp)
{
}
/**
 * rtp_w_data: - process M_DATA on write queue
 * @q: active queue in pari (write queue)
 * @mp: the M_DATA message
 */
STATIC INLINE fastcall __unlikely int
rtp_w_data(queue_t *q, mblk_t *mp)
{
	return (-EPROTO);
}

/**
 * rtp_w_proto: - process M_PROTO, M_PCPROTO on write queue
 * @q: active queue in pari (write queue)
 * @mp: the M_PROTO, M_PCPROTO message
 */
STATIC INLINE fastcall __hot_put int
rtp_w_proto(queue_t *q, mblk_t *mp)
{
	struct rtp *rtp = RTP_PRIV(q);
	uint32_t prim;
	int rtn = -EPROTO;
	int oldstate = rtp_get_state(rtp);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(uint32_t *) mp->b_rptr)) {
		case MX_INFO_REQ:
			printd(("%s: %p: -> MX_INFO_REQ\n", DRV_NAME, rtp));
			rtn = mx_info_req(q, mp);
			break;
		case MX_OPTMGMT_REQ:
			printd(("%s: %p: -> MX_OPTMGMT_REQ\n", DRV_NAME, rtp));
			rtn = mx_optmgmt_req(q, mp);
			break;
		case MX_ATTACH_REQ:
			printd(("%s: %p: -> MX_ATTACH_REQ\n", DRV_NAME, rtp));
			rtn = mx_attach_req(q, mp);
			break;
		case MX_ENABLE_REQ:
			printd(("%s: %p: -> MX_ENABLE_REQ\n", DRV_NAME, rtp));
			rtn = mx_enable_req(q, mp);
			break;
		case MX_CONNECT_REQ:
			printd(("%s: %p: -> MX_CONNECT_REQ\n", DRV_NAME, rtp));
			rtn = mx_connect_req(q, mp);
			break;
		case MX_DATA_REQ:
			printd(("%s: %p: -> MX_DATA_REQ\n", DRV_NAME, rtp));
			rtn = mx_data_req(q, mp);
			break;
		case MX_DISCONNECT_REQ:
			printd(("%s: %p: -> MX_DISCONNECT_REQ\n", DRV_NAME, rtp));
			rtn = mx_disconnect_req(q, mp);
			break;
		case MX_DISABLE_REQ:
			printd(("%s: %p: -> MX_DISABLE_REQ\n", DRV_NAME, rtp));
			rtn = mx_disable_req(q, mp);
			break;
		case MX_DETACH_REQ:
			printd(("%s: %p: -> MX_DETACH_REQ\n", DRV_NAME, rtp));
			rtn = mx_detach_req(q, mp);
			break;
		default:
			printd(("%s: %p: -> MX_????\n", DRV_NAME, rtp));
			rtn = mx_other_req(q, mp);
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		rtp_set_state(rtp, oldstate);
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EOPNOTSUPP:
			return mx_error_ack(q, prim, rtn);
		case -EPROTO:
			return mx_error_reply(q, rtn);
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/**
 * rtp_w_ioctl: - process M_IOCTL on write queue
 * @q: active queue in pair (write queue)
 * @mp: the M_IOCTL message
 */
STATIC INLINE fastcall int
rtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct rtp *rtp = RTP_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;

	switch (type) {
	case _IOC_TYPE(STR):
	{
		pl_t pl;
		struct nip *nip, **nipp;
		struct linkblk *lb;

		if (!(lb = arg)) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, rtp));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					rtp));
				ret = -EPERM;
				break;
			}
			goto linkit;
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, rtp));
		      linkit:
			pl = RW_WRLOCK(&master.lock, plstr);
			/* place in list in ascending index order */
			for (nipp = &master.nip.list; *nipp && (*nipp)->u.mux.index < lb->l_index;
			     nipp = &(*nipp)->net) ;
			if (!(nip = rtp_alloc_link(lb->l_qbot, nipp, lb->l_index, iocp->ioc_cr)))
				ret = -ENOMEM;
			RW_UNLOCK(&master.lock, pl);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, rtp));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					rtp));
				ret = -EPERM;
				break;
			}
			goto unlinkit;
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, rtp));
		      unlinkit:
			pl = RW_WRLOCK(&master.lock, plstr);
			for (nip = master.nip.list; nip; nip = nip->next)
				if (nip->u.mux.index == lb->l_index)
					break;
			if (nip)
				rtp_free_link(nip->iq);
			else {
				ret = -EINVAL;
				ptrace(("%s: %p: ERROR: Could not find I_UNLINK muxid\n", DRV_NAME,
					rtp));
			}
			RW_UNLOCK(&master.lock, pl);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s; %p: ERROR: Unsupported STREAMS ioctl %c, %d\n", DRV_NAME, rtp,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case MX_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		default:
			ptrace(("%s: %p: ERROR: Unsupported MX ioctl %c, %d\n", DRV_NAME, rtp,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, rtp, (char) type,
			nr));
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_count = 0;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_count = 0;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/**
 * rtp_w_iocdata: - process M_IOCDATA on write queue
 * @q: active queue in pari (write queue)
 * @mp: the M_IOCDATA message
 */
STATIC INLINE fastcall __unlikely int
rtp_w_iocdata(queue_t *q, mblk_t *mp)
{
	return (-EPROTO);
}

/*
 *  N-primitives from below
 */

/**
 * n_unitdata_ind: - process an N_UNITDATA_IND message
 * @q: lower read queue
 * @mp: the N_UNITDATA_IND message
 */
STATIC INLINE fastcall int
n_unitdata_ind(queue_t *q, mblk_t *mp)
{
}

/**
 * n_uderror_ind: - process an N_UDERROR_IND message
 * @q: lower read queue
 * @mp: the N_UDERROR_IND message
 */
STATIC INLINE fastcall int
n_uderror_ind(queue_t *q, mblk_t *mp)
{
}

/**
 * n_info_ack: - process an N_INFO_ACK message
 * @q: lower read queue
 * @mp: the N_INFO_ACK message
 */
STATIC INLINE fastcall int
n_info_ack(queue_t *q, mblk_t *mp)
{
}

/**
 * n_ok_ack: - process an N_OK_ACK message
 * @q: lower read queue
 * @mp: the N_OK_ACK message
 */
STATIC INLINE fastcall int
n_ok_ack(queue_t *q, mblk_t *mp)
{
}

/**
 * n_error_ack: - process an N_ERROR_ACK message
 * @q: lower read queue
 * @mp: the N_ERROR_ACK message
 */
STATIC INLINE fastcall int
n_error_ack(queue_t *q, mblk_t *mp)
{
}

/**
 * n_bind_ack: - process an N_BIND_ACK message
 * @q: lower read queue
 * @mp: the N_BIND_ACK message
 */
STATIC INLINE fastcall int
n_bind_ack(queue_t *q, mblk_t *mp)
{
}

/**
 * n_other_ind: - process an unrecognized message
 * @q: lower read queue
 * @mp: the unrecognized message
 */
STATIC INLINE fastcall int
n_other_ind(queue_t *q, mblk_t *mp)
{
}
/**
 * nip_r_data: - process M_DATA on read queue
 * @q: active queue in pari (read queue)
 * @mp: the M_DATA message
 */
STATIC INLINE fastcall __unlikely int
nip_r_data(queue_t *q, mblk_t *mp)
{
	return (-EPROTO);
}

/**
 * nip_r_proto: - process M_PROTO, M_PCPROTO on read queue
 * @q: active queue in pari (read queue)
 * @mp: the M_PROTO, M_PCPROTO message
 */
STATIC INLINE fastcall __hot_get int
nip_r_proto(queue_t *q, mblk_t *mp)
{
	struct nip *nip = NIP_PRIV(q);
	np_ulong prim;
	int rtn = -EPROTO;
	int oldstate = npi_get_state(nip);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(np_ulong *) mp->b_rptr)) {
		case N_UNITDATA_IND:
			printd(("%s: %p: <- N_UNITDATA_IND\n", DRV_NAME, nip));
			rtn = n_unitdata_ind(q, mp);
			break;
		case N_UDERROR_IND:
			printd(("%s: %p: <- N_UDERROR_IND\n", DRV_NAME, nip));
			rtn = n_uderror_ind(q, mp);
			break;
		case N_INFO_ACK:
			printd(("%s: %p: <- N_INFO_ACK\n", DRV_NAME, nip));
			rtn = n_info_ack(q, mp);
			break;
		case N_OK_ACK:
			printd(("%s: %p: <- N_OK_ACK\n", DRV_NAME, nip));
			rtn = n_ok_ack(q, mp);
			break;
		case N_ERROR_ACK:
			printd(("%s: %p: <- N_ERROR_ACK\n", DRV_NAME, nip));
			rtn = n_error_ack(q, mp);
			break;
		case N_BIND_ACK:
			printd(("%s: %p: <- N_BIND_ACK\n", DRV_NAME, nip));
			rtn = n_bind_ack(q, mp);
			break;
		default:
			printd(("%s: %p: <- N_????\n", DRV_NAME, nip));
			rtn = n_other_ind(q, mp);
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		npi_set_state(nip, oldstate);
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EOPNOTSUPP:
			break;
		case -EPROTO:
			return n_error(q, rtn);
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/**
 * nip_r_error: - process M_ERROR on read queue
 * @q: active queue in pari (read queue)
 * @mp: the M_ERROR message
 */
STATIC INLINE fastcall __unlikely int
nip_r_error(queue_t *q, mblk_t *mp)
{
}

/**
 * nip_r_hangup: - process M_HANGUP on read queue
 * @q: active queue in pari (read queue)
 * @mp: the M_HANGUP message
 */
STATIC INLINE fastcall __unlikely int
nip_r_hangup(queue_t *q, mblk_t *mp)
{
}

#ifdef M_UNHANGUP
/**
 * nip_r_unhangup: - process M_UNHANGUP on read queue
 * @q: active queue in pari (read queue)
 * @mp: the M_UNHANGUP message
 */
STATIC INLINE fastcall __unlikely int
nip_r_unhangup(queue_t *q, mblk_t *mp)
{
}
#endif				/* M_UNHANGUP */

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

/**
 * rtp_r_prim: - read side message processing
 * @q: active queue in queue pair (read queue)
 * @mp: message to process
 */
STATIC int
rtp_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/**
 * rtp_w_prim: - write side message processing
 * @q: active queue in queue pair (write queue)
 * @mp: message to process
 */
STATIC int
rtp_w_prim(queue_t *q, mblk_t *mp)
{
	int type;

	/* Fast Path */
	if ((type = mp->b_datap->db_type) == M_DATA)
		return rtp_w_data(q, mp);
	switch (type) {
	case M_DATA:
		return rtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return rtp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return rtp_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}

/**
 * rtp_r_prim: - read side message processing
 * @q: active queue in queue pair (read queue)
 * @mp: message to process
 */
STATIC int
nip_r_prim(queue_t *q, mblk_t *mp)
{
	int type;

	/* Fast Path */
	if ((type = mp->b_datap->db_type) == M_DATA)
		return nip_r_data(q, mp);
	switch (type) {
	case M_DATA:
		return nip_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return nip_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return nip_r_error(q, mp);
	case M_HANGUP:
		return nip_r_hangup(q, mp);
#ifdef M_UNHANGUP
	case M_UNHANGUP:
		return nip_r_unhangup(q, mp);
#endif				/* M_UNHANGUP */
	}
	seldom();
	return (QR_PASSFLOW);
}

/**
 * rtp_w_prim: - write side message processing
 * @q: active queue in queue pair (write queue)
 * @mp: message to process
 */
STATIC int
nip_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t rtp_majors[RTP_CMAJORS] = { RTP_CMAJOR_0, };

/**
 * rtp_open: - STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open system call
 * @sflag: STREAMS flags (DRVOPEN, MODOPEN, CLONEOPEN)
 * @crp: credentials of process opening Stream
 */
STATIC streamscall int
rtp_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	pl_t pl;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct rtp *rtp, **rtpp = &master.rtp.list;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (EIO);	/* cannot push as module */
	if (cmajor != RTP_CMAJOR_0 || cminor > 0)
		return (ENXIO);
	/* allocate a new device */
	cminor = 1;
	pl = LOCK(&rtp_lock, plstr);
	for (; *rtpp; rtpp = &(*rtpp)->next) {
		major_t dmajor = (*rtpp)->u.dev.cmajor;

		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*rtpp)->u.dev.cminor;

			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= RTP_CMAJORS ||
					    !(cmajor = rtp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= RTP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		UNLOCK(&rtp_lock, pl);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(rtp = rtp_alloc_open(q, rtpp, devp, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		UNLOCK(&rtp_lock, pl);
		return (ENOMEM);
	}
	UNLOCK(&rtp_lock, pl);
	return (0);
}

/**
 * rtp_close: - STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to the open system call
 * @crp: credentials of process closing Stream
 */
STATIC streamscall int
rtp_close(queue_t *q, int oflag, cred_t *crp)
{
	struct rtp *rtp = RTP_PRIV(q);
	pl_t pl;

	(void) rtp;
	printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, rtp->u.dev.cmajor,
		rtp->u.dev.cminor));
	pl = LOCK(&rtp_lock, plstr);
	rtp_free_open(q);
	UNLOCK(&rtp_lock, pl);
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
 */

modID_t modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, modID_t, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the RTP driver. (0 for allocation.)");

major_t major = DRV_ID;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, major_t, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Device number for the RTP driver. (0 for allocation.)");

#ifdef LFS
/*
 *  Linux Fast-STREAMS Registration
 */
STATIC struct cdevsw rtp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &rtpinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

/**
 * rtp_register_strdev: - register an RTP driver major device number
 * @major: the major device number to register
 */
STATIC int
rtp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&rtp_cdev, major)) < 0)
		return (err);
	return (0);
}

/**
 * rtp_unregister_strdev: - unregister an RTP driver major device number
 * @major: the major device number to unregister
 */
STATIC int
rtp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&rtp_cdev, major)) < 0)
		return (err);
	return (0);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  Linux STREAMS Registration
 */
/**
 * rtp_register_strdev: - register an RTP driver major device number
 * @major: the major device number to register
 */
STATIC int
rtp_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &rtpinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

/**
 * rtp_unregister_strdev: - unregister an RTP driver major device number
 * @major: the major device number to unregister
 */
STATIC int
rtp_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}
#endif				/* LIS */

/**
 * rtpterminate: - terminate the RTP kernel module
 */
MODULE_STATIC void __exit
rtpterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (rtp_majors[mindex]) {
			if ((err = rtp_unregister_strdev(rtp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					rtp_majors[mindex]);
			if (mindex)
				rtp_majors[mindex] = 0;
		}
	}
	if ((err = rtp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

/**
 * rtp_init: - initialized the RTP kernel module
 */
MODULE_STATIC int __init
rtpinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = rtp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		rtpterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = rtp_register_strdev(rtp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					rtp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				rtpterminate();
				return (err);
			}
		}
		if (rtp_majors[mindex] == 0)
			rtp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(rtp_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = rtp_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 */
module_init(rtpinit);
module_exit(rtpterminate);
#endif				/* LINUX */
