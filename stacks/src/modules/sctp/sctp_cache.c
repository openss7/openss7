/*****************************************************************************

 @(#) $RCSfile: sctp_cache.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/21 10:14:57 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_cache.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:57 $"

static char const ident[] = "$RCSfile: sctp_cache.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:57 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include "debug.h"
#include "bufq.h"

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_hash.h"
#include "sctp_cache.h"

#undef min			/* LiS should not have defined these */
#undef max			/* LiS should not have defined these */

#define sctp_daddr sctp_daddr__
#define sctp_saddr sctp_saddr__
#define sctp_strm  sctp_strm__
#define sctp_dup   sctp_dup__
#ifdef ASSERT
#undef ASSERT
#endif

#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>

#undef sctp_daddr
#undef sctp_saddr
#undef sctp_strm
#undef sctp_dup
#ifdef ASSERT
#undef ASSERT
#endif

#include "sctp_msg.h"

/*
   ip defaults 
 */
uint sctp_default_ip_tos = SCTP_DEFAULT_IP_TOS;
uint sctp_default_ip_ttl = SCTP_DEFAULT_IP_TTL;
uint sctp_default_ip_proto = SCTP_DEFAULT_IP_PROTO;
uint sctp_default_ip_dontroute = SCTP_DEFAULT_IP_DONTROUTE;
uint sctp_default_ip_broadcast = SCTP_DEFAULT_IP_BROADCAST;
uint sctp_default_ip_priority = SCTP_DEFAULT_IP_PRIORITY;

/*
   per association defaults 
 */
size_t sctp_default_max_init_retries = SCTP_DEFAULT_MAX_INIT_RETRIES;
size_t sctp_default_valid_cookie_life = SCTP_DEFAULT_VALID_COOKIE_LIFE;
size_t sctp_default_max_sack_delay = SCTP_DEFAULT_MAX_SACK_DELAY;
size_t sctp_default_assoc_max_retrans = SCTP_DEFAULT_ASSOC_MAX_RETRANS;
size_t sctp_default_mac_type = SCTP_DEFAULT_MAC_TYPE;
size_t sctp_default_cookie_inc = SCTP_DEFAULT_COOKIE_INC;
size_t sctp_default_throttle_itvl = SCTP_DEFAULT_THROTTLE_ITVL;
size_t sctp_default_req_ostreams = SCTP_DEFAULT_REQ_OSTREAMS;
size_t sctp_default_max_istreams = SCTP_DEFAULT_MAX_ISTREAMS;
size_t sctp_default_rmem = SCTP_DEFAULT_RMEM;
size_t sctp_default_ppi = SCTP_DEFAULT_PPI;
size_t sctp_default_sid = SCTP_DEFAULT_SID;

/*
   per destination defaults 
 */
size_t sctp_default_path_max_retrans = SCTP_DEFAULT_PATH_MAX_RETRANS;
size_t sctp_default_rto_initial = SCTP_DEFAULT_RTO_INITIAL;
size_t sctp_default_rto_min = SCTP_DEFAULT_RTO_MIN;
size_t sctp_default_rto_max = SCTP_DEFAULT_RTO_MAX;
size_t sctp_default_heartbeat_itvl = SCTP_DEFAULT_HEARTBEAT_ITVL;

/*
 *  Cache pointers
 *  -------------------------------------------------------------------------
 */
kmem_cache_t *sctp_sctp_cachep = NULL;
kmem_cache_t *sctp_dest_cachep = NULL;
kmem_cache_t *sctp_srce_cachep = NULL;
kmem_cache_t *sctp_strm_cachep = NULL;

void
sctp_init_caches(void)
{
	if (!sctp_sctp_cachep
	    && !(sctp_sctp_cachep =
		 kmem_cache_create("sctp_sctp_cachep", sizeof(sctp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL)))
		panic("%s:Cannot alloc sctp_sctp_cachep.\n", __FUNCTION__);
	if (!sctp_dest_cachep
	    && !(sctp_dest_cachep =
		 kmem_cache_create("sctp_dest_cachep", sizeof(sctp_daddr_t), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_dest_cachep.\n", __FUNCTION__);
	if (!sctp_srce_cachep
	    && !(sctp_srce_cachep =
		 kmem_cache_create("sctp_srce_cachep", sizeof(sctp_saddr_t), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_srce_cachep.\n", __FUNCTION__);
	if (!sctp_strm_cachep
	    && !(sctp_strm_cachep =
		 kmem_cache_create("sctp_strm_cachep", sizeof(sctp_strm_t), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_strm_cachep.\n", __FUNCTION__);
	return;
}

void
sctp_term_caches(void)
{
	if (sctp_sctp_cachep)
		if (kmem_cache_destroy(sctp_sctp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_sctp_cachep");
	if (sctp_dest_cachep)
		if (kmem_cache_destroy(sctp_dest_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_dest_cachep");
	if (sctp_srce_cachep)
		if (kmem_cache_destroy(sctp_srce_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_srce_cachep");
	if (sctp_strm_cachep)
		if (kmem_cache_destroy(sctp_strm_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_strm_cachep");
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DESTINATION ADDRESS HANDLING
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC sctp_daddr_t *
__sctp_daddr_alloc(sp, daddr, errp)
	sctp_t *sp;
	uint32_t daddr;
	int *errp;
{
	sctp_daddr_t *sd;
	assert(errp);
	ensure(sp, *errp = -EFAULT;
	       return (NULL));
	if (!daddr)
		return (NULL);
	if (!(daddr & 0xff000000)) {	/* zeronet is illegal */
		assure(daddr & 0xff000000);
		*errp = -EADDRNOTAVAIL;
		seldom();
		return (NULL);
	}
	/*
	 *  TODO: need to check permissions (TACCES) for broadcast or multicast addresses
	 *  and whether host addresses are valid (TBADADDR).
	 */
	if ((sd = kmem_cache_alloc(sctp_dest_cachep, SLAB_ATOMIC))) {
		bzero(sd, sizeof(*sd));
		if ((sd->next = sp->daddr))
			sd->next->prev = &sd->next;
		sd->prev = &sp->daddr;
		sp->daddr = sd;
		sp->danum++;
		sd->sp = sp;
		sd->daddr = daddr;
		sd->mtu = 576;	/* fix up after routing */
		sd->ssthresh = 2 * sd->mtu;	/* fix up after routing */
		sd->cwnd = sd->mtu;	/* fix up after routing */
		/*
		   per destination defaults 
		 */
		sd->hb_itvl = sp->hb_itvl;	/* heartbeat interval */
		sd->rto_max = sp->rto_max;	/* maximum RTO */
		sd->rto_min = sp->rto_min;	/* minimum RTO */
		sd->rto = sp->rto_ini;	/* initial RTO */
		sd->max_retrans = sp->rtx_path;	/* max path retrans */
		return (sd);
	}
	*errp = -ENOMEM;
	rare();
	return (NULL);
}

/*
 *  Find or Add a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
sctp_daddr_t *
sctp_daddr_include(sp, daddr, errp)
	sctp_t *sp;
	uint32_t daddr;
	int *errp;
{
	sctp_daddr_t *sd;
	assert(errp);
	ensure(sp, *errp = -EFAULT;
	       return (NULL));
	SCTPHASH_WLOCK();
	if (!(sd = sctp_find_daddr(sp, daddr)))
		sd = __sctp_daddr_alloc(sp, daddr, errp);
	SCTPHASH_WUNLOCK();
	usual(sd);
	return (sd);
}

/*
 *  Free a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_daddr_free(sd)
	sctp_daddr_t *sd;
{
	ensure(sd, return);
	/*
	 *  Need to free any cached IP routes.
	 */
	if (sd->dst_cache) {
		rare();
		dst_release(xchg(&sd->dst_cache, NULL));
	}
	if (sd->timer_idle) {
		rare();
		untimeout(xchg(&sd->timer_idle, 0));
	}
	if (sd->timer_heartbeat) {
		rare();
		untimeout(xchg(&sd->timer_heartbeat, 0));
	}
	if (sd->timer_retrans) {
		rare();
		untimeout(xchg(&sd->timer_retrans, 0));
	}
	if (sd->sp)
		sd->sp->danum--;
	if ((*sd->prev = sd->next))
		sd->next->prev = sd->prev;
	kmem_cache_free(sctp_dest_cachep, sd);
}

/*
 *  Free all Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_free_daddrs(sctp_t * sp)
{
	sctp_daddr_t *sd, *sd_next;
	ensure(sp, return);
	sd_next = sp->daddr;
	usual(sd_next);
	while ((sd = sd_next)) {
		sd_next = sd->next;
		__sctp_daddr_free(sd);
	}
	unless(sp->danum, sp->danum = 0);
	sp->dport = 0;
	sp->taddr = NULL;
	sp->raddr = NULL;
	sp->caddr = NULL;
}

/*
 *  Allocate a group of Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
int
sctp_alloc_daddrs(sctp_t * sp, uint16_t dport, uint32_t * daddrs, size_t dnum)
{
	int err = 0;
	ensure(sp, return (-EFAULT));
	ensure(daddrs || !dnum, return (-EFAULT));
	SCTPHASH_WLOCK();
	if (sp->daddr || sp->danum) {
		rare();
		__sctp_free_daddrs(sp);
	}			/* start clean */
	sp->dport = dport;
	if (dnum) {
		while (dnum--)
			if (!__sctp_daddr_alloc(sp, daddrs[dnum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(dnum);
	if (err) {
		rare();
		__sctp_free_daddrs(sp);
	}
	SCTPHASH_WUNLOCK();
	if (err) {
		abnormal(err);
		ptrace(("Returning error %d\n", err));
	}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SOURCE ADDRESS HANDLING
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC sctp_saddr_t *
__sctp_saddr_alloc(sp, saddr, errp)
	sctp_t *sp;
	uint32_t saddr;
	int *errp;
{
	sctp_saddr_t *ss;
	assert(errp);
	ensure(sp, *errp = -EFAULT;
	       return (NULL));
	if (!saddr) {
		rare();
		return (NULL);
	}
	if ((ss = kmem_cache_alloc(sctp_srce_cachep, SLAB_ATOMIC))) {
		bzero(ss, sizeof(*ss));
		if ((ss->next = sp->saddr))
			ss->next->prev = &ss->next;
		ss->prev = &sp->saddr;
		sp->saddr = ss;
		sp->sanum++;
		ss->sp = sp;
		ss->saddr = saddr;
		return (ss);
	}
	*errp = -ENOMEM;
	rare();
	return (ss);
}

/*
 *  Find or Add a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
sctp_saddr_t *
sctp_saddr_include(sp, saddr, errp)
	sctp_t *sp;
	uint32_t saddr;
	int *errp;
{
	sctp_saddr_t *ss;
	assert(errp);
	ensure(sp, *errp = -EFAULT;
	       return (NULL));
	SCTPHASH_WLOCK();
	if (!(ss = sctp_find_saddr(sp, saddr)))
		ss = __sctp_saddr_alloc(sp, saddr, errp);
	SCTPHASH_WUNLOCK();
	usual(ss);
	return (ss);
}

/*
 *  Free a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_saddr_free(ss)
	sctp_saddr_t *ss;
{
	assert(ss);
	if (ss->sp)
		ss->sp->sanum--;
	if ((*ss->prev = ss->next))
		ss->next->prev = ss->prev;
	kmem_cache_free(sctp_srce_cachep, ss);
}

/*
 *  Free all Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_free_saddrs(sctp_t * sp)
{
	sctp_saddr_t *ss, *ss_next;
	assert(sp);
	ss_next = sp->saddr;
	usual(ss_next);
	while ((ss = ss_next)) {
		ss_next = ss->next;
		__sctp_saddr_free(ss);
	}
	unless(sp->sanum, sp->sanum = 0);
	sp->sport = 0;
}

/*
 *  Allocate a group of Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
int
sctp_alloc_saddrs(sp, sport, saddrs, snum)
	sctp_t *sp;
	uint16_t sport;
	uint32_t *saddrs;
	size_t snum;
{
	int err = 0;
	ensure(sp, return (-EFAULT));
	ensure(saddrs || !snum, return (-EFAULT));
	SCTPHASH_WLOCK();
	if (sp->saddr || sp->sanum) {
		rare();
		__sctp_free_saddrs(sp);
	}			/* start clean */
	sp->sport = sport;
	if (snum) {
		while (snum--)
			if (!__sctp_saddr_alloc(sp, saddrs[snum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(snum);
	if (err) {
		rare();
		__sctp_free_saddrs(sp);
	}
	SCTPHASH_WUNLOCK();
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP Stream handling
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
sctp_strm_t *
sctp_strm_alloc(stp, sid, errp)
	sctp_strm_t **stp;
	uint16_t sid;
	int *errp;
{
	sctp_strm_t *st;
	if ((st = kmem_cache_alloc(sctp_strm_cachep, SLAB_ATOMIC))) {
		bzero(st, sizeof(*st));
		if ((st->next = (*stp)))
			st->next->prev = &st->next;
		st->prev = stp;
		(*stp) = st;
		st->sid = sid;
		return (st);
	}
	*errp = -ENOMEM;
	rare();
	return (NULL);
}

/*
 *  Free a Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_strm_free(st)
	sctp_strm_t *st;
{
	assert(st);
	if ((*st->prev = st->next))
		st->next->prev = st->prev;
	kmem_cache_free(sctp_strm_cachep, st);
}

/*
 *  Free all Streams
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_free_strms(sp)
	sctp_t *sp;
{
	sctp_strm_t *st, *st_next;
	assert(sp);
	st_next = sp->ostrm;
	usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->ostr = NULL;
	st_next = sp->istrm;
	usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->istr = NULL;
}

/*
 *  =========================================================================
 *
 *  SCTP Private Data Structure Functions
 *
 *  =========================================================================
 *
 *  We use Linux hardware aligned cache here for speedy access to information
 *  contained in the private data structure.
 */
sctp_t *
sctp_alloc_priv(q, spp, cmajor, cminor, ops)
	queue_t *q;
	sctp_t **spp;
	int cmajor;
	int cminor;
	struct sctp_ifops *ops;
{
	sctp_t *sp;

	assert(q);
	assert(spp);

	/*
	   must have these 4 
	 */
	ensure(ops->sctp_conn_ind, return (NULL));
	ensure(ops->sctp_conn_con, return (NULL));
	ensure(ops->sctp_data_ind, return (NULL));
	ensure(ops->sctp_discon_ind, return (NULL));

	assure(cmajor);
	assure(cminor);

	if ((sp = kmem_cache_alloc(sctp_sctp_cachep, SLAB_ATOMIC))) {
		MOD_INC_USE_COUNT;
		bzero(sp, sizeof(*sp));

		/*
		   link into master list 
		 */
		if ((sp->next = *spp))
			sp->next->prev = &sp->next;
		sp->prev = spp;
		*spp = sp;

		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);
		sp->cmajor = cmajor;
		sp->cminor = cminor;
		sp->ops = ops;
		sp->i_state = 0;
		sp->s_state = SCTP_CLOSED;

		/*
		   ip defaults 
		 */
		sp->ip_tos = sctp_default_ip_tos;
		sp->ip_ttl = sctp_default_ip_ttl;
		sp->ip_proto = sctp_default_ip_proto;
		sp->ip_dontroute = sctp_default_ip_dontroute;
		sp->ip_broadcast = sctp_default_ip_broadcast;
		sp->ip_priority = sctp_default_ip_priority;

		/*
		   per association defaults 
		 */
		sp->max_istr = sctp_default_max_istreams;
		sp->req_ostr = sctp_default_req_ostreams;
		sp->max_inits = sctp_default_max_init_retries;
		sp->max_retrans = sctp_default_assoc_max_retrans;
		sp->a_rwnd = sctp_default_rmem;
		sp->ck_life = sctp_default_valid_cookie_life;
		sp->ck_inc = sctp_default_cookie_inc;
		sp->hmac = sctp_default_mac_type;
		sp->throttle = sctp_default_throttle_itvl;
		sp->sid = sctp_default_sid;
		sp->ppi = sctp_default_ppi;
		sp->max_sack = sctp_default_max_sack_delay;

		/*
		   per destination association defaults 
		 */
		sp->rto_ini = sctp_default_rto_initial;
		sp->rto_min = sctp_default_rto_min;
		sp->rto_max = sctp_default_rto_max;
		sp->rtx_path = sctp_default_path_max_retrans;
		sp->hb_itvl = sctp_default_heartbeat_itvl;

		bufq_init(&sp->rcvq);
		bufq_init(&sp->sndq);
		bufq_init(&sp->urgq);
		bufq_init(&sp->errq);

		bufq_init(&sp->oooq);
		bufq_init(&sp->dupq);
		bufq_init(&sp->rtxq);
		bufq_init(&sp->ackq);
		bufq_init(&sp->conq);

		sctp_init_lock(sp);

		sp->pmtu = 576;

		sp->s_state = SCTP_CLOSED;
	}
	usual(sp);
	return (sp);
}

void
sctp_free_priv(q)
	queue_t *q;
{
	sctp_t *sp;

	ensure(q, return);

	sp = SCTP_PRIV(q);
	ensure(sp, return);

	SCTPHASH_WLOCK();
	{
		sp->s_state = SCTP_CLOSED;

		__sctp_unhash(sp);

		if (sp->timer_init) {
			rare();
			untimeout(xchg(&sp->timer_init, 0));
		}
		if (sp->timer_cookie) {
			rare();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		if (sp->timer_shutdown) {
			rare();
			untimeout(xchg(&sp->timer_shutdown, 0));
		}
		if (sp->timer_sack) {
			rare();
			untimeout(xchg(&sp->timer_sack, 0));
		}

		if (sp->saddr) {
			__sctp_free_saddrs(sp);
		}
		if (sp->daddr) {
			__sctp_free_daddrs(sp);
		}
	}
	SCTPHASH_WUNLOCK();

	unusual(sp->retry);
	freechunks(xchg(&sp->retry, NULL));

	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;

	unusual(sp->conq.q_msgs);
	bufq_purge(&sp->conq);

	unusual(sp->rq->q_count);
	flushq(sp->rq, FLUSHALL);
	unusual(sp->wq->q_count);
	flushq(sp->wq, FLUSHALL);

	unusual(sp->rcvq.q_msgs);
	bufq_purge(&sp->rcvq);
	unusual(sp->sndq.q_msgs);
	bufq_purge(&sp->sndq);
	unusual(sp->urgq.q_msgs);
	bufq_purge(&sp->urgq);
	unusual(sp->errq.q_msgs);
	bufq_purge(&sp->errq);

	unusual(sp->dupq.q_msgs);
	bufq_purge(&sp->dupq);
	unusual(sp->dups);
	sp->dups = NULL;
	unusual(sp->ndups);
	sp->ndups = 0;

#ifdef _DEBUG
	if (sp->oooq.q_msgs && sp->oooq.q_head) {
		mblk_t *mp;
		for (mp = sp->oooq.q_head; mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);
			printk("oooq tsn = %u\n", cb->tsn);
		}
	}
#endif
	unusual(sp->oooq.q_msgs);
	bufq_purge(&sp->oooq);
	unusual(sp->gaps);
	sp->gaps = NULL;
	unusual(sp->ngaps);
	sp->ngaps = 0;
	unusual(sp->nunds);
	sp->nunds = 0;

#ifdef _DEBUG
	if (sp->rtxq.q_msgs && sp->rtxq.q_head) {
		mblk_t *mp;
		for (mp = sp->rtxq.q_head; mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);
			printk("rtxq tsn = %u\n", cb->tsn);
		}
	}
#endif
	unusual(sp->rtxq.q_msgs);
	bufq_purge(&sp->rtxq);
	unusual(sp->nrtxs);
	sp->nrtxs = 0;
	unusual(sp->ackq.q_msgs);
	bufq_purge(&sp->ackq);

	/*
	   do we really need to keep this stuff hanging around for retrieval? 
	 */
	if (sp->ostrm || sp->istrm) {
		sctp_free_strms(sp);
	}

	unusual(RD(q)->q_count);
	if (RD(q)->q_count)
		flushq(RD(q), FLUSHALL);
	unusual(WR(q)->q_count);
	if (WR(q)->q_count)
		flushq(WR(q), FLUSHALL);

	RD(q)->q_ptr = WR(q)->q_ptr = NULL;

	if ((*sp->prev = sp->next))
		sp->next->prev = sp->prev;
	sp->next = NULL;
	sp->prev = NULL;

	kmem_cache_free(sctp_sctp_cachep, sp);
	MOD_DEC_USE_COUNT;
}

/*
 *  DISCONNECT
 *  -------------------------------------------------------------------------
 *  Disconnect the STREAM.  The caller must send a disconnect indication to
 *  the user interface if necessary and send an abort if necessary.  This just
 *  pulls the stream out of the hashes, stops timers, frees simple
 *  retransmission, and zeros connection info.  The stream is left bound and
 *  destination addressses are left allocated.  Any connection indications are
 *  left queued against the stream.
 */
void
sctp_disconnect(sp)
	sctp_t *sp;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		sctp_daddr_t *sd;
		sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;

		/*
		   remove from connected hashes 
		 */
		__sctp_conn_unhash(sp);

		/*
		   stop timers 
		 */
		if (sp->timer_init) {
			untimeout(xchg(&sp->timer_init, 0));
		}
		if (sp->timer_cookie) {
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		if (sp->timer_shutdown) {
			untimeout(xchg(&sp->timer_shutdown, 0));
		}
		if (sp->timer_sack) {
			untimeout(xchg(&sp->timer_sack, 0));
		}

		for (sd = sp->daddr; sd; sd = sd->next) {
			if (sd->dst_cache) {
				dst_release(xchg(&sd->dst_cache, 0));
			}
			if (sd->timer_idle) {
				untimeout(xchg(&sd->timer_idle, 0));
			}
			if (sd->timer_retrans) {
				seldom();
				untimeout(xchg(&sd->timer_retrans, 0));
			}
			if (sd->timer_heartbeat) {
				seldom();
				untimeout(xchg(&sd->timer_heartbeat, 0));
			}
		}
	}
	SCTPHASH_WUNLOCK();

	unusual(sp->retry);
	freechunks(xchg(&sp->retry, NULL));

	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
}

/*
 *  Non-locking version for use from within timeouts (runninga at
 *  bottom-half so don't do bottom-half locks).
 */
void
__sctp_disconnect(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;

	assert(sp);
	SCTPHASH_BH_WLOCK();
	{
		sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;

		/*
		   remove from connected hashes 
		 */
		__sctp_conn_unhash(sp);

		/*
		   stop timers 
		 */
		if (sp->timer_init) {
			seldom();
			untimeout(xchg(&sp->timer_init, 0));
		}
		if (sp->timer_cookie) {
			seldom();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		if (sp->timer_shutdown) {
			seldom();
			untimeout(xchg(&sp->timer_shutdown, 0));
		}
		if (sp->timer_sack) {
			seldom();
			untimeout(xchg(&sp->timer_sack, 0));
		}

		for (sd = sp->daddr; sd; sd = sd->next) {
			if (sd->dst_cache) {
				seldom();
				dst_release(xchg(&sd->dst_cache, 0));
			}
			if (sd->timer_idle) {
				seldom();
				untimeout(xchg(&sd->timer_idle, 0));
			}
			if (sd->timer_retrans) {
				rare();
				untimeout(xchg(&sd->timer_retrans, 0));
			}
			if (sd->timer_heartbeat) {
				rare();
				untimeout(xchg(&sd->timer_heartbeat, 0));
			}
		}
	}
	SCTPHASH_BH_WUNLOCK();

	unusual(sp->retry);
	freechunks(xchg(&sp->retry, NULL));

	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
}

/*
 *  UNBIND
 *  -------------------------------------------------------------------------
 *  We should already be in a disconnected state.  This pulls us from the bind
 *  hashes and deallocates source addresses, any connection indications that
 *  are queued against the stream are purged (these might occur if a
 *  connection indication comes in just before we unbind and we have made an
 *  error somewhere else: normally the response to an X_UNBIND_REQ in a
 *  connection indication state will be a X_ERROR_ACK).
 */
void
sctp_unbind(sp)
	sctp_t *sp;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		sp->s_state = SCTP_CLOSED;

		__sctp_bind_unhash(sp);
		__sctp_free_saddrs(sp);
	}
	SCTPHASH_WUNLOCK();

	unusual(sp->conq.q_msgs);
	bufq_purge(&sp->conq);
}

/*
 *  RESET
 *  -------------------------------------------------------------------------
 *  Clear the connection information hanging around on a stream.  This include
 *  any queued data blocks that are waitinga around for retrieval.  It is OK
 *  to call this function twice in a row on the same streeam.
 */
void
sctp_reset(sp)
	sctp_t *sp;
{
	unusual(sp->rq->q_count);
	flushq(sp->rq, FLUSHALL);
	unusual(sp->wq->q_count);
	flushq(sp->wq, FLUSHALL);

	sp->pmtu = 576;

	/*
	   purge queues 
	 */
	unusual(sp->rcvq.q_msgs);
	bufq_purge(&sp->rcvq);
	unusual(sp->sndq.q_msgs);
	bufq_purge(&sp->sndq);
	unusual(sp->urgq.q_msgs);
	bufq_purge(&sp->urgq);
	unusual(sp->errq.q_msgs);
	bufq_purge(&sp->errq);

	unusual(sp->dupq.q_msgs);
	bufq_purge(&sp->dupq);
	unusual(sp->dups);
	sp->dups = NULL;
	unusual(sp->ndups);
	sp->ndups = 0;

	unusual(sp->oooq.q_msgs);
	bufq_purge(&sp->oooq);
	unusual(sp->gaps);
	sp->gaps = NULL;
	unusual(sp->ngaps);
	sp->ngaps = 0;
	unusual(sp->nunds);
	sp->nunds = 0;

	unusual(sp->rtxq.q_msgs);
	bufq_purge(&sp->rtxq);
	unusual(sp->nrtxs);
	sp->nrtxs = 0;
	unusual(sp->ackq.q_msgs);
	bufq_purge(&sp->ackq);

	if (sp->ostrm || sp->istrm) {
		sctp_free_strms(sp);
	}
}
