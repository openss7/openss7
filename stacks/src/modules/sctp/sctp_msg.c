/*****************************************************************************

 @(#) $RCSfile: sctp_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:30:27 $

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

 Last Modified $Date: 2005/03/08 19:30:27 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:30:27 $"

static char const ident[] = "$RCSfile: sctp_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:30:27 $";

#define __NO_VERSION__

#include "os7/compat.h"

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_hash.h"
#include "sctp_cache.h"
#include "sctp_route.h"
#include "sctp_cookie.h"
#include "sctp_output.h"

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

#include <linux/random.h>

#undef min
#define min lis_min
#undef max
#define max lis_min

STATIC uint32_t
sctp_get_vtag(uint32_t daddr, uint32_t saddr, uint16_t dport, uint16_t sport)
{
	uint32_t ret;
	ret = secure_tcp_sequence_number(daddr, saddr, dport, sport);
	usual(ret);
	return (ret);
}

static inline void
set_timeout(sctp_t * sp, int *tidp, timo_fcn_t *fnc, void *data, long ticks)
{
	psw_t flags;
	assert(tidp);
	assert(data);
	lis_spin_lock_irqsave(&sp->lock, &flags);
	{
		if (*tidp) {
			abnormal(*tidp);
			untimeout(xchg(tidp, 0));
		}
		*tidp = timeout(fnc, data, ticks ? ticks : 1);
	}
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
}

static inline void
mod_timeout(sctp_t * sp, int *tidp, timo_fcn_t *fnc, void *data, long ticks)
{
	psw_t flags;
	assert(tidp);
	assert(data);
	lis_spin_lock_irqsave(&sp->lock, &flags);
	{
		if (*tidp) {
			untimeout(xchg(tidp, 0));
		}
		*tidp = timeout(fnc, data, ticks ? ticks : 1);
	}
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
}

/*
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */
/*
 *  BUNDLING FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  BUNDLE SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_bundle_sack(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	sctp_daddr_t *sd;		/* destination */
	size_t dmps;			/* destination maximum payload size */
	size_t amps;			/* association maximum payload size */
	mblk_t ***dpp;			/* place to link buffer */
	size_t *mrem;			/* remaining payload */
	size_t *mlen;			/* current message length */
{
	mblk_t *mp;
	sctp_tcb_t *cb;
	struct sctp_sack *m;
	size_t ngaps, ndups, glen, dlen, clen, plen;

	ngaps = sp->ngaps;
	ndups = sp->ndups;
	glen = ngaps * sizeof(uint32_t);
	dlen = ndups * sizeof(uint32_t);
	clen = sizeof(*m) + glen + dlen;
	plen = PADC(clen);

	if (clen > dmps) {
		size_t too_many_dups;
		rare();		/* trim down sack */
		too_many_dups = (clen - *mrem + 3) / sizeof(uint32_t);
		ndups = ndups > too_many_dups ? ndups - too_many_dups : 0;
		clen = sizeof(*m) + glen + ndups * sizeof(uint32_t);
		if (*mrem < clen) {
			size_t too_many_gaps;
			rare();	/* trim some more */
			too_many_gaps = (clen - *mrem + 3) / sizeof(uint32_t);
			ngaps = ngaps > too_many_gaps ? ngaps - too_many_gaps : 0;
			clen = sizeof(*m) + ngaps * sizeof(uint32_t);
		}
	}
	if (plen > *mrem && plen <= dmps) {
		rare();
		return;		/* wait for next packet */
	}
	if ((mp = allocb(sizeof(*cb) + plen, BPRI_HI))) {
		sctp_tcb_t *gap = sp->gaps;
		sctp_tcb_t *dup = sp->dups;
		size_t arwnd = sp->a_rwnd;
		size_t count = bufq_size(&sp->oooq) + bufq_size(&sp->dupq) + bufq_size(&sp->rcvq);

		arwnd = count < arwnd ? arwnd - count : 0;

		/*
		   __ptrace(("oooq = %u:%u, dupq = %u:%u, rcvq = %u:%u\n",
		   bufq_size(&sp->oooq),bufq_length(&sp->oooq),
		   bufq_size(&sp->dupq),bufq_length(&sp->dupq),
		   bufq_size(&sp->rcvq),bufq_length(&sp->rcvq))); 
		 */

		// if ( !canputnext(sp->rq) ) arwnd = 0;

		mp->b_datap->db_type = M_DATA;
		cb = (sctp_tcb_t *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = clen;	/* XXX */
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr = mp->b_rptr;

		bzero(mp->b_wptr + clen, plen - clen);

		m = (struct sctp_sack *) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_SACK;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->c_tsn = htonl(sp->r_ack);
		m->a_rwnd = htonl(arwnd);
		m->ngaps = htons(ngaps);
		m->ndups = htons(ndups);
		mp->b_wptr += sizeof(*m);

		for (; gap && ngaps; gap = gap->next, ngaps--) {
			*((uint16_t *) mp->b_wptr)++ = htons(gap->tsn - sp->r_ack);
			gap = gap->tail;
			*((uint16_t *) mp->b_wptr)++ = htons(gap->tsn - sp->r_ack);
		}

		for (; dup && ndups; dup = dup->next, ndups--)
			*((uint32_t *) mp->b_wptr)++ = htonl(dup->tsn);

		bufq_purge(&sp->dupq);
		sp->ndups = 0;
		sp->dups = NULL;

		sp->sackf = 0;
		if (sp->timer_sack)
			untimeout(xchg(&sp->timer_sack, 0));

		*mrem = *mrem > plen ? *mrem - plen : 0;
		*mlen += plen;

		**dpp = mp;
		*dpp = &(mp->b_next);
		mp->b_next = NULL;

		return;

	}
	rare();
}

/*
 *  BUNDLE ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_bundle_error(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	sctp_daddr_t *sd;		/* destination */
	size_t dmps;			/* destination maximum payload size */
	size_t amps;			/* association maximum payload size */
	mblk_t ***dpp;			/* place to link buffer */
	size_t *mrem;			/* remaining payload */
	size_t *mlen;			/* current message length */
{
	mblk_t *mp;

	while (*mrem && (mp = bufq_head(&sp->errq))) {
		size_t clen = mp->b_wptr - mp->b_rptr;
		size_t plen = PADC(clen);

		if (plen > *mrem && plen <= dmps) {
			rare();
			return;	/* wait for next packet */
		}

		*mrem = *mrem > plen ? *mrem - plen : 0;
		*mlen += plen;

		mp = bufq_dequeue(&sp->errq);

		**dpp = mp;
		*dpp = &(mp->b_next);
		mp->b_next = NULL;

		continue;
	}
}

/*
 *  BUNDLE DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline int
trimhead(mblk_t *mp, int len)
{
	for (; len && mp; mp = mp->b_cont) {
		if (mp->b_datap->db_type == M_DATA) {
			int size = mp->b_wptr - mp->b_rptr;
			if (size > len) {
				mp->b_rptr += len;
				len = 0;
			} else if (size >= 0) {
				mp->b_rptr = mp->b_wptr;
				len -= size;
			}
		}
	}
	assure(!len);
	return (len);
}
static inline int
trimtail(mblk_t *mp, int len)
{
	for (; len && mp; mp = mp->b_cont) {
		if (mp->b_datap->db_type == M_DATA) {
			int size = mp->b_wptr - mp->b_rptr;
			if (size > len) {
				mp->b_wptr = mp->b_rptr + len;
				len = 0;
			} else if (size >= 0) {
				mp->b_wptr = mp->b_rptr;
				len -= size;
			}
		}
	}
	for (; mp; mp = mp->b_cont)
		if (mp->b_datap->db_type == M_DATA)
			if (mp->b_wptr > mp->b_rptr)
				mp->b_wptr = mp->b_rptr;
	assure(!len);
	return (len);
}

/*
 *  FRAGMENT DATA CHUNKS
 *
 *  Try to fragment a DATA chunk which has not been transmitted yet into
 *  two chunks, the first small enough to fit into the pmtu and the second one
 *  containing the remainder of the data in a chunk.  This is called
 *  iteratively, so the reminaing data may also be further fragmented
 *  according to the pmtu experienced at the time that it is further
 *  fragmented.
 */
#if defined(_DEBUG)||defined(_SAFE)
STATIC void
sctp_frag_chunk(bq, mp, mps)
	bufq_t *bq;
	mblk_t *mp;
	size_t mps;
{
	mblk_t *dp;

	assert(bq);
	assert(mp);

	rare();
	/*
	 *  This should be extremely rare, now that we are fragmenting in
	 *  sctp_send_data.  This fragmentation only occurs if the path MTU
	 *  has dropped since we buffered data for transmission.  It is
	 *  probably not necessary any more.
	 */
	{
		mblk_t *bp;
		/*
		   copy the transmission control block and data header 
		 */
		if (!(dp = copyb(mp))) {
			rare();
			return;
		}
		/*
		   copyb does not copy the hiddle control block 
		 */
		bcopy(mp->b_datap->db_base, dp->b_datap->db_base,
		      mp->b_datap->db_lim - mp->b_datap->db_base);
		/*
		   duplicate the message blocks which form the data 
		 */
		if (!(bp = dupmsg(mp->b_cont))) {
			rare();
			freeb(dp);
			return;
		}
		dp->b_cont = bp;
	}
	{
		sctp_tcb_t *cb1 = SCTP_TCB(mp);
		sctp_tcb_t *cb2 = SCTP_TCB(dp);
		struct sctp_data *m1 = (struct sctp_data *) mp->b_rptr;
		struct sctp_data *m2 = (struct sctp_data *) dp->b_rptr;
		size_t dlen = (mps - sizeof(struct sctp_data));

		cb1->dlen = dlen;
		cb1->flags &= ~SCTPCB_FLAG_LAST_FRAG;
		m1->ch.len = htons(cb1->dlen + sizeof(*m1));
		m1->ch.flags = cb1->flags & 0x7;

		cb2->dlen -= dlen;
		cb2->flags &= ~SCTPCB_FLAG_FIRST_FRAG;
		m2->ch.len = htons(cb2->dlen + sizeof(*m2));
		m2->ch.flags = cb2->flags & 0x7;

#if 1
		{
			int ret;
			ret = trimhead(mp, dlen);	/* trim originai */
			ensure(ret, freemsg(dp);
			       return;
			    );
			ret = trimtail(dp, dlen);	/* trim fragment */
			ensure(ret, freemsg(dp);
			       return;
			    );
		}
#else
		fixme(("Should consider multiple mblks\n"));
		mp->b_cont->b_wptr = mp->b_cont->b_rptr + dlen;	/* trim original */
		dp->b_cont->b_rptr = dp->b_cont->b_rptr + dlen;	/* trim fragment */
#endif
	}
	/*
	   insert the fresh copy after the existing copy in the bufq 
	 */
	__ctrace(bufq_append(bq, mp, dp));
}
#endif

/*
 *  CONGESTION/RECEIVE WINDOW AVAILABILITY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Calculate of the remaining space in the current packet, how much is
 *  available for use by data according to the current peer receive window,
 *  the current destination congestion window, and the current outstanding
 *  data bytes in flight.
 *
 *  This is called iteratively as each data chunk is tested for bundling into
 *  the current message.  The usable length returned does not include the data
 *  chunk header.
 */
STATIC size_t
sctp_avail(sp, sd, dmps, mrem)
	sctp_t *sp;
	sctp_daddr_t *sd;
	size_t dmps;
	size_t mrem;
{
	size_t ulen;

	ulen = mrem > sizeof(struct sctp_data) ? mrem - sizeof(struct sctp_data) : 0;

	if (ulen) {
		size_t cwnd, rwnd, swnd, awnd;

		cwnd = sd->cwnd + dmps;
		cwnd = cwnd > sd->in_flight ? cwnd - sd->in_flight : 0;
		rwnd = sp->p_rwnd;
		rwnd = rwnd > sp->in_flight ? rwnd - sp->in_flight : 0;
		swnd = cwnd < rwnd ? cwnd : rwnd;
		awnd = sp->in_flight ? swnd : cwnd;
		ulen = awnd < ulen ? awnd : ulen;
	}
	return (ulen);
}

/*
 *  BUNDLE DATA for RETRANSMISSION
 */
STATIC void
sctp_bundle_data_retrans(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	sctp_daddr_t *sd;		/* destination */
	size_t dmps;			/* destination maximum payload size */
	size_t amps;			/* association maximum payload size */
	mblk_t ***dpp;			/* place to link buffer */
	size_t *mrem;			/* remaining payload */
	size_t *mlen;			/* current message length */
{
	mblk_t *mp = bufq_head(&sp->rtxq);
	size_t swnd = sctp_avail(sp, sd, dmps, *mrem);

	for (; mp && *mrem && swnd; mp = mp->b_next) {
		if ((SCTP_TCB(mp)->flags & SCTPCB_FLAG_RETRANS)) {
			mblk_t *db;
			sctp_tcb_t *cb = SCTP_TCB(mp);
			size_t dlen = cb->dlen;
			size_t plen = PADC(sizeof(struct sctp_data) + dlen);

			if (dlen > swnd) {
				rare();
				return;	/* congested */
			}
			if (plen > *mrem && plen <= dmps) {
				rare();
				return;	/* wait for next packet */
			}
			if ((db = dupmsg(mp))) {
				cb->flags &= ~SCTPCB_FLAG_RETRANS;
				sp->nrtxs--;
				// cb->trans += 1; done by sctp_send_msg */
				cb->sacks = 0;
				cb->when = jiffies;
				cb->daddr = sd;

				sd->in_flight += dlen;
				sp->in_flight += dlen;

				*mrem = *mrem > plen ? *mrem - plen : 0;
				*mlen += plen;

				**dpp = db;
				*dpp = &(db->b_next);
				db->b_next = NULL;

				swnd = sctp_avail(sp, sd, dmps, *mrem);

				continue;
			}
			rare();
			return;
		}
	}
	return;
}

/*
 *  BUNDLE NEW EXPEDITED (OUT OF ORDER) DATA
 */
STATIC void
sctp_bundle_data_urgent(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	sctp_daddr_t *sd;		/* destination */
	size_t dmps;			/* destination maximum payload size */
	size_t amps;			/* association maximum payload size */
	mblk_t ***dpp;			/* place to link buffer */
	size_t *mrem;			/* remaining payload */
	size_t *mlen;			/* current message length */
{
	mblk_t *mp;
	size_t swnd;

	while (*mrem && (swnd = sctp_avail(sp, sd, dmps, *mrem))
	       && (mp = bufq_head(&sp->urgq))) {
		mblk_t *db;
		sctp_tcb_t *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(struct sctp_data) + dlen);

		ensure(cb->st, continue);

#if defined(_DEBUG)||defined(_SAFE)
		/*
		   this should only occur if the pmtu is falling 
		 */
		if (amps <= *mrem && plen > amps) {
			rare();
			sctp_frag_chunk(&sp->urgq, mp, amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
		if (dlen > swnd) {
			rare();
			return;	/* congested */
		}
		if (plen > *mrem && plen <= dmps) {
			rare();
			return;	/* wait for next packet */
		}
		if ((mp == cb->st->x.head)) {
			rare();
			cb->st->x.head = NULL;	/* steal partial */
		}
		if ((db = dupmsg(mp))) {
			uint32_t tsn = sp->t_tsn++;
			struct sctp_data *m = (struct sctp_data *) db->b_rptr;

			cb->tsn = tsn;
			cb->flags |= SCTPCB_FLAG_SENT;
			cb->trans = 0;
			cb->sacks = 0;
			cb->when = jiffies;
			cb->daddr = sd;

			m->tsn = htonl(tsn);

			sd->in_flight += dlen;
			sp->in_flight += dlen;

			*mrem = *mrem > plen ? *mrem - plen : 0;
			*mlen += plen;

			**dpp = db;
			*dpp = &(db->b_next);
			db->b_next = NULL;

			bufq_queue(&sp->rtxq, bufq_dequeue(&sp->urgq));

			continue;
		}
		rare();
		return;
	}
	return;
}

/*
 *  BUNDLE NEW NORMAL (ORDERED) DATA
 */
STATIC void
sctp_bundle_data_normal(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	sctp_daddr_t *sd;		/* destination */
	size_t dmps;			/* destination maximum payload size */
	size_t amps;			/* association maximum payload size */
	mblk_t ***dpp;			/* place to link buffer */
	size_t *mrem;			/* remaining payload */
	size_t *mlen;			/* current message length */
{
	mblk_t *mp;
	size_t swnd;

	/*
	   don't bundle normal data without more to send (like a SACK) 
	 */
	if (sp->options & SCTP_OPTION_CORK
	    || (sp->options & SCTP_OPTION_NAGLE && sp->in_flight && *mlen == sizeof(struct sctphdr)
		&& bufq_size(&sp->sndq) < *mrem >> 1))
		return;

	while (*mrem && (swnd = sctp_avail(sp, sd, dmps, *mrem))
	       && (mp = bufq_head(&sp->sndq))) {
		mblk_t *db;
		sctp_tcb_t *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(struct sctp_data) + dlen);

		ensure(cb->st, continue);

#if defined(_DEBUG)||defined(_SAFE)
		/*
		   this should only occur if the pmtu is falling 
		 */
		if (amps <= *mrem && plen > amps) {
			rare();
			sctp_frag_chunk(&sp->sndq, mp, amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
		if (dlen > swnd) {
			seldom();
			return;	/* congested */
		}
		if (plen > *mrem && plen <= dmps) {
			seldom();
			return;	/* wait for next packet */
		}
		if ((mp == cb->st->n.head)) {
			seldom();
			cb->st->n.head = NULL;	/* steal partial */
		}
		if ((db = dupmsg(mp))) {
			uint32_t tsn = sp->t_tsn++;

			cb->tsn = tsn;
			cb->flags |= SCTPCB_FLAG_SENT;
			// cb->trans = 1; done by sctp_send_msg */
			cb->sacks = 0;
			cb->when = jiffies;
			cb->daddr = sd;

			((struct sctp_data *) db->b_rptr)->tsn = htonl(tsn);

			sd->in_flight += dlen;
			sp->in_flight += dlen;

			*mrem = *mrem > plen ? *mrem - plen : 0;
			*mlen += plen;

			**dpp = db;
			*dpp = &(db->b_next);
			db->b_next = NULL;

			bufq_queue(&sp->rtxq, bufq_dequeue(&sp->sndq));

			continue;
		}
		rare();
		return;
	}
	return;
}

/*
 *  BUNDLE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Tack on SACK, ERROR, and DATA chunks up to the destination MTU considering
 *  congestion windows and fragmentation sizes.
 */
STATIC void sctp_retrans_timeout(caddr_t data);

STATIC void
sctp_bundle_more(sp, sd, mp)
	sctp_t *sp;
	sctp_daddr_t *sd;
	mblk_t *mp;
{
	mblk_t **dpp;
	size_t mrem, dmps, amps, mlen;
	size_t htax = sizeof(struct iphdr) + sizeof(struct sctphdr);
	size_t in_flight;

	ensure(mp, return);
	assert(sp);
	assert(sd);

	in_flight = sd->in_flight;

	ensure(sd->mtu > htax, return);	/* FIXME: this is worse... */
	ensure(sp->pmtu > htax, return);	/* FIXME: this is worse... */

	dmps = sd->mtu - htax;	/* destintaion max payload size */
	amps = sp->pmtu - htax;	/* association max payload size */

	mlen = msgdsize(mp);
	mrem = sd->mtu - sizeof(struct iphdr);
	mrem = (mlen < mrem) ? mrem - mlen : 0;

	dpp = &(mp->b_next);
	mp->b_next = NULL;

	if (mrem && (sp->sackf & SCTP_SACKF_NOW))
		sctp_bundle_sack(sp, sd, dmps, amps, &dpp, &mrem, &mlen);
	if (mrem && bufq_head(&sp->errq))
		sctp_bundle_error(sp, sd, dmps, amps, &dpp, &mrem, &mlen);
	if (mrem && bufq_head(&sp->urgq))
		sctp_bundle_data_urgent(sp, sd, dmps, amps, &dpp, &mrem, &mlen);
	if (mrem && sp->nrtxs)
		sctp_bundle_data_retrans(sp, sd, dmps, amps, &dpp, &mrem, &mlen);
	if (mrem && bufq_head(&sp->sndq))
		sctp_bundle_data_normal(sp, sd, dmps, amps, &dpp, &mrem, &mlen);

	SCTP_TCB(mp)->dlen = mlen;

	if (sd->in_flight && !sd->timer_retrans) {
		set_timeout(sp, &sd->timer_retrans, &sctp_retrans_timeout, sd, sd->rto);
	}
	return;
}

/*
 *  ALLOC MSG
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocates a new message block with a hidden transmission control block, an
 *  SCTP message header, and the first chunk of a message.
 */
STATIC mblk_t *
sctp_alloc_msg(sp, clen)
	sctp_t *sp;
	size_t clen;
{
	mblk_t *mp;
	struct sctphdr *sh;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);

	assert(sp);
	if ((mp = allocb(sizeof(*cb) + sizeof(*sh) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		cb = (sctp_tcb_t *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = sizeof(*sh) + plen;
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);

		sh = (struct sctphdr *) mp->b_wptr;
		sh->srce = sp->sport;
		sh->dest = sp->dport;
		sh->v_tag = sp->p_tag;
		sh->check = 0;
		mp->b_wptr += sizeof(*sh);

		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/*
 *  ALLOC REPLY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for allocating a message block with a hidden transmission control
 *  block for a message sith an STCP header.  This is for out of the blue
 *  replies and all that is required is the SCTP header of the message to
 *  which this is a reply.
 */
STATIC mblk_t *
sctp_alloc_reply(rh, clen)
	struct sctphdr *rh;
	size_t clen;
{
	mblk_t *mp;
	struct sctphdr *sh;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);

	assert(rh);
	if ((mp = allocb(sizeof(*cb) + sizeof(*sh) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		cb = (sctp_tcb_t *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = sizeof(*sh) + plen;
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);

		sh = (struct sctphdr *) mp->b_wptr;
		sh->srce = rh->dest;
		sh->dest = rh->srce;
		sh->v_tag = rh->v_tag;
		sh->check = 0;
		mp->b_wptr += sizeof(*sh);

		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/*
 *  ROUTE SELECTION
 *  -------------------------------------------------------------------------
 *  
 *  ROUTE NORMAL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Send to the normal transmit (primary) address.  If that has timedout, we
 *  use the retransmit (secondary) address.  We always use the secondary
 *  address if we have retransmit chunks to bundle or if we have be receiving
 *  duplicates (our SACKs are not getting through on the primary address).
 *
 */
STATIC sctp_daddr_t *
sctp_route_normal(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;
	assert(sp);
	if (sctp_update_routes(sp, 1)) {
		rare();
		/*
		   we have no viable route 
		 */
		if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
			sp->ops->sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -EHOSTUNREACH, NULL);
		} else
			rare();
		__sctp_disconnect(sp);
		return (NULL);
	}
	sd = sp->taddr;
	normal(sd);
#if 0
	if ((!sd || ((sd->retransmits || sp->nrtxs) && sd->max_retrans)) && sp->raddr)
		sd = sp->raddr;
	// if ( !sd || ((sd->retransmits || sp->nrtxs) && sd->max_retrans) || (sp->sackf &
	// SCTP_SACKF_DUP) )
	if (!sd || (sp->nrtxs && sd->max_retrans) || (sp->sackf & SCTP_SACKF_DUP))
		sd = sp->raddr;	/* might be same address */
#endif
	return (sd);
}

/*
 *  ROUTE RESPONSE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  In response to control chunks we normally send back to the address that
 *  the control chunk came from.  If that address is unusable or wasn't
 *  provided we send as normal.
 */
STATIC sctp_daddr_t *
sctp_route_response(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;
	assert(sp);
	sd = sp->caddr;
	if (!sd || !sd->dst_cache || sd->retransmits)
		sd = sctp_route_normal(sp);
	normal(sd);
	return (sd);
}

/*
 *  WAKEUP (Send SACK, ERROR, DATA)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void
sctp_transmit_wakeup(sp)
	sctp_t *sp;
{
	int i;
	mblk_t *mp;
	sctp_daddr_t *sd;
	int loop_max = 3000;

	assert(sp);
	if ((1 << sp->s_state) & SCTPF_SENDING) {
		for (i = 0; i < loop_max; i++) {
			/*
			   placed in order of probability 
			 */
			if (bufq_head(&sp->sndq)
			    || (sp->sackf & SCTP_SACKF_NOW)
			    || sp->nrtxs || bufq_head(&sp->urgq)
			    || bufq_head(&sp->errq)
			    ) {
				if (!(sd = sctp_route_normal(sp))) {
					rare();
					break;
				}
				if (!(mp = sctp_alloc_msg(sp, 0))) {
					rare();
					break;
				}

				sctp_bundle_more(sp, sd, mp);

				if (mp->b_next) {
					sctp_send_msg(sp, sd, mp);
					freechunks(mp);
					continue;
				}
				unusual(bufq_head(&sp->sndq));
				unusual(bufq_head(&sp->urgq));
				unusual(bufq_head(&sp->errq));
				unusual(sp->sackf & SCTP_SACKF_NOD);
				unusual(sp->nrtxs);
				if (sp->nrtxs)
					ptrace(("sp->nrtxs = %u\n", sp->nrtxs));
				rare();
				freechunks(mp);
#ifdef _DEBUG
				for (mp = bufq_head(&sp->sndq); mp; mp = mp->b_next) {
					sctp_tcb_t *cb = SCTP_TCB(mp);
					printk("sndq: mp = %08x, dlen = %u\n", (uint) mp, cb->dlen);
				}
				for (mp = bufq_head(&sp->urgq); mp; mp = mp->b_next) {
					sctp_tcb_t *cb = SCTP_TCB(mp);
					printk("urgq: mp = %08x, dlen = %u\n", (uint) mp, cb->dlen);
				}
				if (sp->nrtxs)
					for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
						sctp_tcb_t *cb = SCTP_TCB(mp);
						printk("rtxq: mp = %08x, dlen = %u, tsn = %u\n",
						       (uint) mp, cb->dlen, cb->tsn);
					}
#endif
			}
			break;
		}
		assure(i < loop_max);
	}
	return;
}

/*
 *  =========================================================================
 *
 *  SCTP State Machine TIMEOUTS
 *
 *  =========================================================================
 */
STATIC void sctp_send_heartbeat(sctp_t * sp, sctp_daddr_t * sd);
STATIC void sctp_reset_idle(sctp_daddr_t * sd);
/*
 *  ASSOCIATION TIMEOUT FUNCTION
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_assoc_timedout(sp, sd, rmax)
	sctp_t *sp;
	sctp_daddr_t *sd;
	size_t rmax;
{
	assert(sp);
	assert(sd);

	/*
	   RFC 2960 6.3.3 E1 and 7.2.3, E2, E3 and 8.3 
	 */
	sd->ssthresh = sd->cwnd >> 1 > sd->mtu << 1 ? sd->cwnd >> 1 : sd->mtu << 1;
	sd->cwnd = sd->mtu;
	sd->rto = sd->rto ? sd->rto << 1 : 1;
	sd->rto = sd->rto_min > sd->rto ? sd->rto_min : sd->rto;
	sd->rto = sd->rto_max < sd->rto ? sd->rto_max : sd->rto;

	/*
	   See RFC 2960 Section 8.3 
	 */
	if (sd->retransmits++ >= sd->max_retrans) {
		if (sd->dst_cache)
			dst_negative_advice(&sd->dst_cache);
		if (sd->retransmits == sd->max_retrans + 1) {
			/*
			 *  IMPLEMENTATION NOTE:-  When max_retrans and
			 *  rto_max are set to zero, we are cruel on
			 *  destinations that drop a single packet due to
			 *  noise.  This forces an immediate heartbeat on the
			 *  destination so that it can be made available again
			 *  quickly it if passes the heartbeat.
			 */
			if ((1 << sp->s_state) & (SCTPF_CONNECTED | SCTPF_CLOSING)) {
				if (sd->timer_idle)
					untimeout(xchg(&sd->timer_idle, 0));
				sctp_send_heartbeat(sp, sd);
			}
		} else {
			if ((1 << sp->s_state) & (SCTPF_CONNECTED))
				return (0);
		}
	}
	/*
	   See RFC 2960 Section 8.2 
	 */
	if (rmax && sp->retransmits++ >= rmax) {
		seldom();
		if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
			sp->ops->sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT, NULL);
		} else
			rare();
		__sctp_disconnect(sp);
		return (-ETIMEDOUT);
	}

	return (0);
}

#define SCTP_TIMER_BACKOFF 1

/*
 *  INIT TIMEOUT (T1-init)
 *  -------------------------------------------------------------------------
 *  The init timer has expired indicating that we have not received an INIT
 *  ACK within timer T1-init.  This means that we should attempt to retransmit
 *  the INIT until we have attempted Max.Init.Retrans times.
 */
STATIC void
sctp_init_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	sctp_daddr_t *sd;

	sd = (sctp_daddr_t *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sp->timer_init) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sp->timer_init =
			    timeout(sctp_init_timeout, (caddr_t) sd, SCTP_TIMER_BACKOFF);
			break;
		}
		sp->timer_init = 0;
		if (sp->s_state != SCTP_COOKIE_WAIT) {
			rare();
			break;
		}
		if (sctp_assoc_timedout(sp, sd, sp->max_inits)) {
			seldom();
			break;
		}
		sd = sp->taddr;	/* might have new primary */
		ensure(sd, break);
		set_timeout(sp, &sp->timer_init, sctp_init_timeout, (caddr_t) sd, sd->rto);
		usual(sp->retry);
		sctp_send_msg(sp, sd, sp->retry);
	} while (0);
	sctp_bh_unlock(sp);
}

/*
 *  COOKIE TIMEOUT
 *  -------------------------------------------------------------------------
 *  The cookie timer has expired indicating that we have not yet received a
 *  COOKIE ACK within timer T1-cookie.  This means that we should attempt to
 *  retransmit the COOKIE ECHO until we have attempted Path.Max.Retrans times.
 */
STATIC void
sctp_cookie_timeout(data)
	caddr_t data;
{
	mblk_t *mp;
	sctp_t *sp;
	sctp_daddr_t *sd;

	sd = (sctp_daddr_t *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sp->timer_cookie) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sp->timer_cookie =
			    timeout(sctp_cookie_timeout, (caddr_t) sd, SCTP_TIMER_BACKOFF);
			break;
		}
		sp->timer_cookie = 0;
		if (sp->s_state != SCTP_COOKIE_ECHOED) {
			rare();
			break;
		}
		if (sctp_assoc_timedout(sp, sd, sp->max_retrans)) {
			seldom();
			break;
		}
		/*
		   See RFC 2960 6.3.3 E3 
		 */
		for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			seldom();
			if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
			    && !(cb->flags & SCTPCB_FLAG_RETRANS)) {
				cb->flags |= SCTPCB_FLAG_RETRANS;
				sp->nrtxs++;
				cb->sacks = 0;
			}
		}
		sd = sp->taddr;	/* might have new primary */
		ensure(sd, break);
		set_timeout(sp, &sp->timer_cookie, sctp_cookie_timeout, sd, sd->rto);
		usual(sp->retry);
		sctp_send_msg(sp, sd, sp->retry);
	}
	while (0);
	sctp_bh_unlock(sp);
}

/*
 *  RETRANS TIMEOUT (T3-rtx)
 *  -------------------------------------------------------------------------
 *  This means that we have not received an ack for a DATA chunk within timer
 *  T3-rtx.  This means that we should mark all outstanding DATA chunks for
 *  retransmission and start a retransmission cycle.
 */
STATIC void
sctp_retrans_timeout(data)
	caddr_t data;
{
	mblk_t *mp;
	sctp_t *sp;
	sctp_daddr_t *sd;
	int retransmits = 0;

	sd = (sctp_daddr_t *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sd->timer_retrans) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sd->timer_retrans =
			    timeout(sctp_retrans_timeout, (caddr_t) sd, SCTP_TIMER_BACKOFF);
			break;
		}
		sd->timer_retrans = 0;
		if (!((1 << sp->s_state) & (SCTPF_CONNECTED))) {
			rare();
			break;
		}
		if (sctp_assoc_timedout(sp, sd, sp->max_retrans)) {
			seldom();
			break;
		}
		/*
		   See RFC 2960 6.3.3 E3 
		 */
		for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);
			size_t dlen = cb->dlen;

			if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
			    && !(cb->flags & SCTPCB_FLAG_RETRANS)
			    && !(cb->flags & SCTPCB_FLAG_SACKED)) {
				cb->flags |= SCTPCB_FLAG_RETRANS;
				sp->nrtxs++;
				cb->sacks = 0;
				normal(sd->in_flight >= dlen);
				normal(sp->in_flight >= dlen);
				sd->in_flight = sd->in_flight > dlen ? sd->in_flight - dlen : 0;	/* credit
													   dest */
				sp->in_flight = sp->in_flight > dlen ? sp->in_flight - dlen : 0;	/* credit
													   assoc */
				retransmits++;
			}
		}
		normal(retransmits);
		sctp_transmit_wakeup(sp);
	} while (0);
	sctp_bh_unlock(sp);
}

/*
 *  SACK TIMEOUT
 *  -------------------------------------------------------------------------
 *  This timer is the 200ms timer which ensures that a SACK is sent within
 *  200ms of the receipt of an unacknoweldged DATA chunk.  When an
 *  unacknowledged DATA chunks i receive and the timer is not running, the
 *  timer is set.  Whenever a DATA chunks(s) are acknowledged, the timer is
 *  stopped.
 */
STATIC void
sctp_sack_timeout(data)
	caddr_t data;
{
	sctp_t *sp;

	sp = (sctp_t *) data;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sp->timer_sack) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sp->timer_sack =
			    timeout(sctp_sack_timeout, (caddr_t) sp, SCTP_TIMER_BACKOFF);
			break;
		}
		sp->timer_sack = 0;
		if (!((1 << sp->s_state) & (SCTPF_RECEIVING))) {
			rare();
			break;
		}
		sp->sackf |= SCTP_SACKF_TIM;	/* RFC 2960 6.2 */
		sctp_transmit_wakeup(sp);
	} while (0);
	sctp_bh_unlock(sp);
}

/*
 *  IDLE TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that a destination has been idle for longer than the hb.itvl or
 *  the interval for which we must send heartbeats.  This timer is reset every
 *  time we do an RTT calculation for a destination.  It is stopped while
 *  sending heartbeats and started again whenever an RTT calculation is done.
 *  While this timer is stopped, heartbeats will be sent until they are
 *  acknowledged.
 */
STATIC void
sctp_idle_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	sctp_daddr_t *sd;

	sd = (sctp_daddr_t *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sd->timer_idle) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sd->timer_idle =
			    timeout(&sctp_idle_timeout, (caddr_t) sd, SCTP_TIMER_BACKOFF);
			break;
		}
		sd->timer_idle = 0;
		if (!((1 << sp->s_state) & (SCTPF_CONNECTED | SCTPF_CLOSING))) {
			rare();
			break;
		}
		sctp_send_heartbeat(sp, sd);
	} while (0);
	sctp_bh_unlock(sp);
}

/*
 *  HEARTBEAT TIMEOUT
 *  -------------------------------------------------------------------------
 *  If we get a heartbeat timeout we adjust RTO the same as we do for
 *  retransmit (and the congestion window) and resend the heartbeat.  Once we
 *  have sent Path.Max.Retrans heartbeats unsuccessfully, we mark the
 *  destination as unusable, but continue heartbeating until they get
 *  acknowledged.  (Well!  That's not really true, is it?)
 */
STATIC void
sctp_heartbeat_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	sctp_daddr_t *sd;

	sd = (sctp_daddr_t *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sd->timer_heartbeat) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sd->timer_heartbeat =
			    timeout(sctp_heartbeat_timeout, (caddr_t) sd, SCTP_TIMER_BACKOFF);
			break;
		}
		sd->timer_heartbeat = 0;
		if (!((1 << sp->s_state) & (SCTPF_CONNECTED | SCTPF_CLOSING))) {
			rare();
			break;
		}
		if (sctp_assoc_timedout(sp, sd, 0)) {
			seldom();
			break;
		}
		sctp_send_heartbeat(sp, sd);
	} while (0);
	sctp_bh_unlock(sp);
}

/*
 *  SHUTDOWN TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timedout on sending a SHUTDOWN or a SHUTDOWN ACK
 *  message.  We simply resend the message.
 */
STATIC void
sctp_shutdown_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	sctp_daddr_t *sd;

	sd = (sctp_daddr_t *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);

	sctp_bh_lock(sp);
	do {
		if (!sp->timer_shutdown) {
			rare();
			break;
		}
		if (sctp_locked(sp)) {
			seldom();
			sp->timer_shutdown =
			    timeout(sctp_shutdown_timeout, (caddr_t) sd, SCTP_TIMER_BACKOFF);
			break;
		}
		sp->timer_shutdown = 0;
		if (!((1 << sp->s_state) & (SCTPF_CLOSING))) {
			rare();
			break;
		}
		if (sctp_assoc_timedout(sp, sd, sp->max_retrans)) {
			seldom();
			break;
		}
		sd = sp->taddr;
		ensure(sd, break);
		set_timeout(sp, &sp->timer_shutdown, sctp_shutdown_timeout, (caddr_t) sd, sd->rto);
		usual(sp->retry);
		sctp_send_msg(sp, sd, sp->retry);
	} while (0);
	sctp_bh_unlock(sp);
}

/*
 *  SEND DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This function slaps a chunk header onto the M_DATA blocks which form the
 *  data and places it onto the stream's write queue.  The message blocks
 *  input to this function already have a chunk control block prepended.
 */
STATIC int
sctp_send_data(sp, st, flags, dp)
	sctp_t *sp;
	sctp_strm_t *st;
	ulong flags;
	mblk_t *dp;
{
	uint *more;
	mblk_t **head;
	uint32_t ppi;
	size_t mlen, dlen;
	ulong dflags = flags;
	uint urg = (dflags & SCTPCB_FLAG_URG);

	assert(sp);
	assert(st);
	assert(dp);

	if (urg) {
		more = &st->x.more;
		head = &st->x.head;
		ppi = st->x.ppi;
	} else {
		more = &st->n.more;
		head = &st->n.head;
		ppi = st->n.ppi;
	}

	for (mlen = msgdsize(dp); mlen; mlen -= dlen, dflags &= ~SCTPCB_FLAG_FIRST_FRAG) {
		mblk_t *bp;
		struct sctp_daddr *sd;

		if (!(sd = sctp_route_normal(sp)))
			return (-EHOSTUNREACH);

		/*
		 *  If there is not enough room in the current send window to handle all or at
		 *  least 1/2 MTU of the data and the current send backlog then return (-EBUSY)
		 *  and put the message back on the queue so that backpressure will result.  We
		 *  only do this separately for normal data and urgent data (urgent data will be
		 *  expedited ahead of even retransmissions).
		 */
		{
			size_t cwnd, rwnd, swnd, awnd, plen, amps, dmps, used;

			plen = PADC(sizeof(struct sctp_data) + mlen);
			amps = sp->pmtu - sizeof(struct iphdr) - sizeof(struct sctphdr);
			dmps = sd->mtu - sizeof(struct iphdr) - sizeof(struct sctphdr);
			used = urg ? sp->urgq.q_count : sp->sndq.q_count;

			cwnd = sd->cwnd + dmps;
			cwnd = cwnd > sd->in_flight ? cwnd - sd->in_flight : 0;
			rwnd = sp->p_rwnd;
			rwnd = rwnd > sp->in_flight ? rwnd - sp->in_flight : 0;
			swnd = cwnd < rwnd ? cwnd : rwnd;
			awnd = sp->in_flight ? swnd : cwnd;
			awnd = awnd > used ? awnd - used : 0;

			if (plen > awnd || plen > amps) {
				if (plen > amps && awnd >= amps >> 1) {	/* SWS avoidance */
					if ((bp = dupmsg(dp))) {

						dlen = awnd < amps ? amps >> 1 : amps;
						ensure(dlen > sizeof(struct sctp_data), freemsg(bp);
						       return (-EFAULT));
						dlen -= sizeof(struct sctp_data);
						ensure(dlen < mlen, freemsg(bp);
						       return (-EFAULT));
#if 1
						{
							int ret;
							ret = trimhead(dp, dlen);	/* trim
											   original 
											 */
							unless(ret, freemsg(bp);
							       return (-EFAULT));
							ret = trimtail(bp, dlen);	/* trim
											   fragment 
											 */
							unless(ret, freemsg(bp);
							       return (-EFAULT));
						}
#else
						fixme(("Should consider multiple mblks\n"));
						dp->b_rptr = dp->b_rptr + dlen;	/* trim original */
						bp->b_wptr = bp->b_rptr + dlen;	/* trim fragment */
#endif
						dflags &= ~SCTPCB_FLAG_LAST_FRAG;

					} else {
						rare();
						return (-ENOBUFS);
					}
				} else {
					rare();
					return (-EBUSY);
				}
			} else {

				bp = dp;
				dlen = mlen;	/* use entire */
				dflags |= flags & SCTPCB_FLAG_LAST_FRAG;

				/*
				 *  If we have an existing SDU being built that hasn't
				 *  been transmitted yet, we just tack data onto it.  We
				 *  concatenate only to an MTU.
				 */
				if (*more && *head && plen + SCTP_TCB(*head)->dlen <= amps) {
					struct sctp_data *m;
					sctp_tcb_t *cb = SCTP_TCB(*head);

					cb->flags |= (dflags & 0x7);
					cb->dlen += dlen;

					m = (struct sctp_data *) (*head)->b_rptr;
					m->ch.flags = cb->flags & 0x7;
					m->ch.len = htons(sizeof(*m) + cb->dlen);

					linkb(*head, bp);

					normal(cb->dlen == msgdsize((*head)->b_cont));

					if (dflags & SCTPCB_FLAG_LAST_FRAG) {
						*head = NULL;
						*more = 0;
					}
					return (0);
				}
			}
		}
		{
			mblk_t *mp;
			sctp_tcb_t *cb;
			struct sctp_data *m;

			if ((mp = allocb(sizeof(*cb) + sizeof(*m), BPRI_MED))) {
				mp->b_datap->db_type = M_DATA;
				cb = (sctp_tcb_t *) mp->b_wptr;
				bzero(cb, sizeof(*cb));
				cb->mp = mp;
				cb->dlen = dlen;
				cb->flags = dflags;
				cb->st = st;
				cb->when = jiffies;
				cb->daddr = NULL;	/* set when transmitted */
				cb->ppi = ppi;
				cb->sid = st->sid;
				cb->ssn = urg ? 0 : st->ssn;
				mp->b_rptr += sizeof(*cb);	/* hide control block */
				mp->b_wptr += sizeof(*cb);

				m = (struct sctp_data *) mp->b_wptr;
				m->ch.type = SCTP_CTYPE_DATA;
				m->ch.flags = dflags;
				m->ch.len = htons(sizeof(*m) + dlen);
				m->tsn = 0;	/* assign before transmission */
				m->sid = htons(cb->sid);
				m->ssn = htons(cb->ssn);
				m->ppi = htonl(cb->ppi);
				mp->b_wptr += sizeof(*m);

				mp->b_cont = bp;

				normal(cb->dlen == msgdsize(mp->b_cont));
				/*
				 *  Remember where we can add more data in case data
				 *  completing a SDU comes before we are forced to bundle the
				 *  DATA.
				 */
				*more = (dflags & SCTPCB_FLAG_LAST_FRAG) ? 0 : 1;
				*head = (dflags & SCTPCB_FLAG_LAST_FRAG) ? NULL : mp;

				if (urg) {
					bufq_queue(&sp->urgq, mp);
				} else {
					if (dflags & SCTPCB_FLAG_LAST_FRAG)
						st->ssn = (st->ssn + 1) & 0xffff;
					bufq_queue(&sp->sndq, mp);
				}
			} else {
				rare();
				return (-ENOBUFS);
			}
		}
	}
	return (0);
}

/*
 *  SEND SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_sack(sp)
	sctp_t *sp;
{
	sp->sackf |= SCTP_SACKF_NOD;
}

/*
 *  SEND ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We just queue the error, we don't send it out...  It gets bundled with
 *  other things.
 */
STATIC int
sctp_send_error(sp, ecode, eptr, elen)
	sctp_t *sp;
	uint ecode;
	caddr_t eptr;
	size_t elen;
{
	mblk_t *mp;
	sctp_tcb_t *cb;
	struct sctp_error *m;
	struct sctpehdr *eh;
	size_t clen = sizeof(*m) + sizeof(*eh) + elen;
	size_t plen = PADC(clen);

	assert(sp);
	if ((mp = allocb(sizeof(*cb) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		cb = (sctp_tcb_t *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr = mp->b_rptr;

		bzero(mp->b_wptr + clen, plen - clen);

		m = (struct sctp_error *) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_ERROR;
		m->ch.flags = 0;
		m->ch.len = htons(clen);

		eh = (struct sctpehdr *) (m + 1);
		eh->code = htons(ecode);
		eh->len = htons(sizeof(*eh) + elen);

		bcopy(eptr, (eh + 1), elen);
		mp->b_wptr += plen;

		bufq_queue(&sp->errq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SEND INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the INIT and get timers started, we must return an
 *  error to the user interface calling this function.
 */
STATIC int
sctp_send_init(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sp->daddr)) {
		mblk_t *mp;
		struct sctp_init *m;
		struct sctp_addr_type *at;
		struct sctp_ipv4_addr *ap;
		struct sctp_cookie_psrv *cp;
		size_t sanum = sp->sanum;
		size_t clen = sizeof(*m)
		    + (sanum * PADC(sizeof(*ap)))
		    + (sp->ck_inc ? PADC(sizeof(*cp)) : 0)
		    + (sizeof(*at) + sizeof(at->type[0]));

		if ((mp = sctp_alloc_msg(sp, clen))) {
			sctp_saddr_t *ss;

			m = (struct sctp_init *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_INIT;
			m->ch.flags = 0;
			m->ch.len = htons(clen);
			m->i_tag = sp->v_tag;
			m->a_rwnd = htonl(sp->a_rwnd);
			m->n_istr = htons(sp->max_istr);
			m->n_ostr = htons(sp->req_ostr);
			m->i_tsn = htonl(sp->v_tag);
			mp->b_wptr += sizeof(*m);

			for (ss = sp->saddr; ss && sanum; ss = ss->next, sanum--) {
				ap = (struct sctp_ipv4_addr *) mp->b_wptr;
				ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
				ap->ph.len = htons(sizeof(*ap));
				ap->addr = ss->saddr;
				mp->b_wptr += PADC(sizeof(*ap));
			}

			unusual(ss);
			unusual(sanum);

			if (sp->ck_inc) {
				cp = (struct sctp_cookie_psrv *) mp->b_wptr;
				cp->ph.type = SCTP_PTYPE_COOKIE_PSRV;
				cp->ph.len = htons(sizeof(*cp));;
				cp->ck_inc = htonl(sp->ck_inc);
				mp->b_wptr += PADC(sizeof(*cp));
			}

			at = (struct sctp_addr_type *) mp->b_wptr;
			at->ph.type = SCTP_PTYPE_ADDR_TYPE;
			at->ph.len = htons(sizeof(*at) + sizeof(at->type[0]));
			at->type[0] = SCTP_PTYPE_IPV4_ADDR;
			mp->b_wptr += PADC(sizeof(*at) + sizeof(at->type[0]));

			sctp_send_msg(sp, sd, mp);
			mod_timeout(sp, &sp->timer_init, &sctp_init_timeout, sd, sd->rto);
			unusual(sp->retry);
			freechunks(xchg(&sp->retry, mp));
			sp->s_state = SCTP_COOKIE_WAIT;
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EFAULT);
}

/*
 *  SEND INIT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  No s_state change results from replying to an INIT.  INIT ACKs are sent
 *  without a TCB but a STREAM is referenced.  INIT ACK chunks cannot have any
 *  other chunks bundled with them. (RFC 2960 6.10).
 */
STATIC void
sctp_send_init_ack(sp, daddr, sh, ck)
	sctp_t *sp;
	uint32_t daddr;
	struct sctphdr *sh;
	struct sctp_cookie *ck;
{
	mblk_t *mp;
	struct sctp_init_ack *m;
	struct sctpphdr *ph;
	struct sctp_cookie *cp;
	struct sctp_ipv4_addr *ap;
	struct sctp_init *im = (struct sctp_init *) (sh + 1);
	unsigned char *init = (unsigned char *) im;

	int anum = ck->danum;
	int snum = ck->sanum;

	size_t klen = sizeof(*ph) + raw_cookie_size(ck) + HMAC_SIZE;
	size_t dlen = sp->sanum * PADC(sizeof(*ap));
	size_t clen = sizeof(*m) + dlen + klen;

	int arem, alen;

	assert(sp);
	if ((mp = sctp_alloc_reply(sh, clen))) {
		sctp_saddr_t *ss;
		struct sctphdr *rh;

		rh = ((struct sctphdr *) mp->b_wptr) - 1;
		rh->v_tag = im->i_tag;

		m = (struct sctp_init_ack *) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_INIT_ACK;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->i_tag = ck->v_tag;
		m->a_rwnd = htonl(sp->a_rwnd);
		m->n_istr = htons(ck->n_istr);
		m->n_ostr = htons(ck->n_ostr);
		m->i_tsn = htonl(ck->v_tag);
		mp->b_wptr += sizeof(*m);

		for (ss = sp->saddr; ss; ss = ss->next) {
			ap = (struct sctp_ipv4_addr *) mp->b_wptr;
			ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
			ap->ph.len = __constant_htons(sizeof(*ap));
			ap->addr = ss->saddr;
			mp->b_wptr += PADC(sizeof(*ap));
		}

		ph = (struct sctpphdr *) mp->b_wptr;
		ph->type = SCTP_PTYPE_STATE_COOKIE;
		ph->len = htons(klen);
		mp->b_wptr += sizeof(*ph);

		cp = (struct sctp_cookie *) mp->b_wptr;
		bcopy(ck, cp, sizeof(*cp));
		mp->b_wptr += sizeof(*cp);

#if 0
		/*
		   copy in IP reply options 
		 */
		if (ck->opt_len) {
			assure(opt);
			bcopy(opt, mp->b_wptr, optlength(opt));
			kfree_s(opt, optlength(opt));
			sp->opt = (struct ip_options *) mp->b_wptr;
			mp->b_wptr += ck->opt_len;
		}
#endif
		for (ap = (struct sctp_ipv4_addr *) (init + sizeof(struct sctp_init)), arem =
		     PADC(htons(((struct sctpchdr *) init)->len)) - sizeof(struct sctp_init);
		     anum && arem >= sizeof(struct sctpphdr);
		     arem -= PADC(alen), ap =
		     (struct sctp_ipv4_addr *) (((uint8_t *) ap) + PADC(alen))) {
			if ((alen = ntohs(ap->ph.len)) > arem) {
				assure(alen <= arem);
				freemsg(mp);
				rare();
				return;
			}
			if (ap->ph.type == SCTP_PTYPE_IPV4_ADDR) {
				/*
				   skip primary 
				 */
				if (ap->addr != ck->daddr) {
					*((uint32_t *) mp->b_wptr)++ = ap->addr;
					anum--;
				}
			}
		}

		for (ss = sp->saddr; ss; ss = ss->next) {
			if (ss->saddr != ck->saddr) {
				*((uint32_t *) mp->b_wptr)++ = ss->saddr;
				snum--;
			}
		}

		assure(!anum);
		assure(!snum);

		sctp_sign_cookie(sp, cp);
		mp->b_wptr += HMAC_SIZE;

		sctp_xmit_msg(daddr, mp, sp);
	}
}

/*
 *  SEND COOKIE ECHO
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the COOKIE ECHO and get timers started, we must
 *  return an error to the user interface calling this function.
 */
STATIC int
sctp_send_cookie_echo(sp, kptr, klen)
	sctp_t *sp;
	caddr_t kptr;
	size_t klen;
{
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sp->daddr)) {
		mblk_t *mp;
		struct sctp_cookie_echo *m;
		size_t clen = sizeof(*m) + klen;
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_cookie_echo *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_COOKIE_ECHO;
			m->ch.flags = 0;
			m->ch.len = htons(clen);
			bcopy(kptr, (m + 1), klen);
			mp->b_wptr += plen;

			sctp_bundle_more(sp, sd, mp);
			sctp_send_msg(sp, sd, mp);
			mod_timeout(sp, &sp->timer_cookie, &sctp_cookie_timeout, sd, sd->rto);
			unusual(sp->retry);
			freechunks(xchg(&sp->retry, mp));
			sp->s_state = SCTP_COOKIE_ECHOED;
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EFAULT);
}

/*
 *  SEND COOKIE ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SACK and DATA can be bundled with the COOKIE ACK.
 */
STATIC void
sctp_send_cookie_ack(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sctp_route_response(sp))) {
		mblk_t *mp;
		struct sctp_cookie_ack *m;
		size_t clen = sizeof(*m);
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_cookie_ack *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_COOKIE_ACK;
			m->ch.flags = 0;
			m->ch.len = __constant_htons(clen);
			mp->b_wptr += plen;

			sctp_bundle_more(sp, sd, mp);
			sctp_send_msg(sp, sd, mp);
			freechunks(mp);
		}
		sp->s_state = SCTP_ESTABLISHED;
		/*
		   start idle timers 
		 */
		for (sd = sp->daddr; sd; sd = sd->next)
			sctp_reset_idle(sd);
	}
}

/*
 *  SEND HEARTBEAT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We don't send heartbeats when idle timers expire if we are in the wrong
 *  state, we just reset the idle timer.
 */
STATIC void
sctp_send_heartbeat(sp, sd)
	sctp_t *sp;
	sctp_daddr_t *sd;
{
	mblk_t *mp;
	struct sctp_heartbeat *m;
	struct sctp_heartbeat_info *h;
	size_t fill, clen, hlen, plen;

	assert(sp);
	assert(sd);

	fill = sd->hb_fill;
	clen = sizeof(*m) + sizeof(*h) + fill;
	hlen = clen - sizeof(struct sctpchdr);
	plen = PADC(clen);

	sd->hb_time = jiffies;

	if ((mp = sctp_alloc_msg(sp, clen))) {
		m = (struct sctp_heartbeat *) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_HEARTBEAT;
		m->ch.flags = 0;
		m->ch.len = htons(clen);

		h = (struct sctp_heartbeat_info *) (m + 1);
		h->ph.type = SCTP_PTYPE_HEARTBEAT_INFO;
		h->ph.len = htons(hlen);
		h->hb_info.timestamp = sd->hb_time;
		h->hb_info.daddr = sd->daddr;
		h->hb_info.mtu = sd->mtu;

		bzero(h->hb_info.fill, fill);
		mp->b_wptr += plen;

		sctp_send_msg(sp, sd, mp);
		freechunks(mp);
	}
	mod_timeout(sp, &sd->timer_heartbeat, &sctp_heartbeat_timeout, sd, sd->rto);
}

/*
 *  SEND HEARTBEAT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Take the incoming HEARTBEAT message and turn it back around as a HEARTBEAT
 *  ACK message.  Note that if the incoming chunk parameters are invalid, so
 *  are the outgoing parameters, this is because the hb_info parameter is
 *  opaque to us.  This is consistent with draft-stewart-ong-sctpbakeoff-
 *  sigtran-01.
 */
STATIC void
sctp_send_heartbeat_ack(sp, hptr, hlen)
	sctp_t *sp;
	caddr_t hptr;
	size_t hlen;
{
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sctp_route_response(sp))) {
		mblk_t *mp;
		struct sctp_heartbeat_ack *m;
		size_t clen = sizeof(*m) + hlen;
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_heartbeat_ack *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_HEARTBEAT_ACK;
			m->ch.flags = 0;
			m->ch.len = htons(clen);
			bcopy(hptr, (m + 1), hlen);
			mp->b_wptr += plen;
			sctp_send_msg(sp, sd, mp);
			freechunks(mp);
		}
	}
}

/*
 *  SEND ABORT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  There is no point in bundling control chunks after an ABORT chunk.  Also,
 *  DATA chunks are not to be bundled with ABORT chunks.
 */
STATIC void
sctp_send_abort(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;

	assert(sp);
	if ((1 << sp->s_state) & SCTPF_CONNECTED)
		sd = sctp_route_normal(sp);
	else
		sd = sp->daddr;
	if (sd) {
		mblk_t *mp;
		struct sctp_abort *m;
		size_t clen = sizeof(*m);
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_abort *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_ABORT;
			m->ch.flags = 0;
			m->ch.len = __constant_htons(clen);
			mp->b_wptr += plen;
			sctp_send_msg(sp, sd, mp);
			freechunks(mp);
		}
		sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
	}
}

/*
 *  SEND ABORT (w/ERROR CAUSE)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Errors (beyond the error header) must be formatted by the called and
 *  indicated by are and len.  There is no point in bundling data or control
 *  chunks after and abort chunk.
 */
STATIC void
sctp_send_abort_error(sp, errn, aptr, alen)
	sctp_t *sp;
	int errn;
	void *aptr;			/* argument ptr */
	size_t alen;			/* argument len */
{
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sctp_route_normal(sp))) {
		if (errn) {
			mblk_t *mp;
			struct sctp_abort *m;
			struct sctpehdr *eh;
			size_t elen = sizeof(*eh) + alen;
			size_t clen = sizeof(*m) + elen;
			size_t plen = PADC(clen);

			if ((mp = sctp_alloc_msg(sp, clen))) {
				m = (struct sctp_abort *) mp->b_wptr;
				m->ch.type = SCTP_CTYPE_ABORT;
				m->ch.flags = 0;
				m->ch.len = htons(clen);
				eh = (struct sctpehdr *) (m + 1);
				eh->code = htons(errn);
				eh->len = htons(elen);
				bcopy(aptr, (eh + 1), alen);
				mp->b_wptr += plen;
				sctp_send_msg(sp, sd, mp);
				freechunks(mp);
			}
			sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
			return;
		}
		sctp_send_abort(sp);
	}
}

/*
 *  SEND SHUTDOWN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  RFC 2960 6.2 "... DATA chunks cannot be bundled with SHUTDOWN or SHUTDOWN
 *  ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN and get timers started, we must inform
 *  the user interface calling this function.
 */
STATIC int
sctp_send_shutdown(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sctp_route_normal(sp))) {
		struct sctp_shutdown *m;
		size_t clen = sizeof(*m);
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_shutdown *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_SHUTDOWN;
			m->ch.flags = 0;
			m->ch.len = __constant_htons(clen);
			m->c_tsn = htonl(sp->r_ack);
			mp->b_wptr += plen;

			/*
			   shutdown acks everything but dups and gaps 
			 */
			sp->sackf &= (SCTP_SACKF_DUP | SCTP_SACKF_GAP);

			sctp_bundle_more(sp, sd, mp);	/* not DATA */
			sctp_send_msg(sp, sd, mp);
			mod_timeout(sp, &sp->timer_shutdown, &sctp_shutdown_timeout, sd, sd->rto);
			// unusual( sp->retry ); /* not that unusual */
			freechunks(xchg(&sp->retry, mp));
			sp->s_state = SCTP_SHUTDOWN_SENT;
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SHUTDOWN ACK is sent in response to a SHUTDOWN message after all data has
 *  cleared or in reponse to a COOKIE ECHO during the SHUTDOWN_ACK_SENT s_state.
 *  If the error flag is set, we want to bundle and ERROR chunk with the
 *  SHUTDOWN ACK indicating "cookie received while shutting down."
 *
 *  RFC 2960 6.2. "...  DATA chunks cannot be bundled with SHUTDOWN or
 *  SHUTDOWN ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN ACK and get timers started, we must
 *  return an error to the user interface calling this function.
 */
STATIC int
sctp_send_shutdown_ack(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sctp_route_response(sp))) {
		struct sctp_shutdown_ack *m;
		size_t clen = sizeof(*m);
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_shutdown_ack *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_SHUTDOWN_ACK;
			m->ch.flags = 0;
			m->ch.len = __constant_htons(clen);
			mp->b_wptr += plen;
			sctp_bundle_more(sp, sd, mp);	/* not DATA */
			sctp_send_msg(sp, sd, mp);
			mod_timeout(sp, &sp->timer_shutdown, &sctp_shutdown_timeout, sd, sd->rto);
			unusual(sp->retry);
			freechunks(xchg(&sp->retry, mp));
			sp->s_state = SCTP_SHUTDOWN_ACK_SENT;
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN COMPLETE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_shutdown_complete(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	sctp_daddr_t *sd;

	assert(sp);
	if ((sd = sctp_route_response(sp))) {
		struct sctp_shutdown_comp *m;
		size_t clen = sizeof(*m);
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_msg(sp, clen))) {
			m = (struct sctp_shutdown_comp *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
			m->ch.flags = 0;
			m->ch.len = __constant_htons(clen);
			mp->b_wptr += plen;
			sctp_send_msg(sp, sd, mp);
			freechunks(mp);
		}
	}
	sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
}

/*
 *  SENDING WITHOUT TCB  (Responding to OOTB packets)
 *  -------------------------------------------------------------------------
 *  When sending without an SCTP TCB, we only have the IP header and the SCTP
 *  header from which to work.  We have no associated STREAM.  These are
 *  usually used for replying to OOTB messages.
 *
 *  SEND ABORT (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void
sctp_send_abort_ootb(daddr, saddr, sh)
	uint32_t daddr;
	uint32_t saddr;
	struct sctphdr *sh;
{
	mblk_t *mp;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);

	assert(sh);
	if ((mp = sctp_alloc_reply(sh, clen))) {
		m = (struct sctp_abort *) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_ABORT;
		m->ch.flags = 1;
		m->ch.len = __constant_htons(clen);
		mp->b_wptr += plen;

		sctp_xmit_ootb(daddr, saddr, mp);
	} else
		rare();
}

/*
 *  SEND ABORT (w/ERROR CAUSE) (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_abort_error_ootb(daddr, saddr, sh, errn, aptr, alen)
	uint32_t daddr;
	uint32_t saddr;
	struct sctphdr *sh;
	int errn;
	caddr_t aptr;			/* argument ptr */
	size_t alen;			/* argument len */
{
	assert(sh);
	if (errn) {
		mblk_t *mp;
		struct sctp_abort *m;
		struct sctpehdr *eh;
		size_t elen = sizeof(*eh) + alen;
		size_t clen = sizeof(*m) + elen;
		size_t plen = PADC(clen);

		if ((mp = sctp_alloc_reply(sh, clen))) {
			m = (struct sctp_abort *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_ABORT;
			m->ch.flags = 1;
			m->ch.len = htons(clen);
			eh = (struct sctpehdr *) (m + 1);
			eh->code = htons(errn);
			eh->len = htons(elen);
			bcopy(aptr, (eh + 1), alen);
			mp->b_wptr += plen;

			sctp_xmit_ootb(daddr, saddr, mp);
		} else
			rare();
		return;
	}
	sctp_send_abort_ootb(daddr, saddr, sh);
}

/*
 *  SEND SHUTDOWN COMPLETE (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void
sctp_send_shutdown_complete_ootb(daddr, saddr, sh)
	uint32_t daddr;
	uint32_t saddr;
	struct sctphdr *sh;
{
	mblk_t *mp;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);

	assert(sh);
	if ((mp = sctp_alloc_reply(sh, clen))) {
		m = (struct sctp_shutdown_comp *) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
		m->ch.flags = 1;
		m->ch.len = __constant_htons(clen);
		mp->b_wptr += plen;
		sctp_xmit_ootb(daddr, saddr, mp);
	} else
		rare();
}

/*
 *  BIND_REQ:
 *  -------------------------------------------------------------------------
 *  Bind the stream to the addresses provided in its bound address lists.
 *  There are some errors that may be returned here:
 *
 *  Any UNIX error.
 *
 *  NOADDR      - A wildcard address was specified and we don't support
 *                wildcards (maybe we will).
 *
 *  ADDRBUSY    - 
 *
 */
int
sctp_bind_req(sp, sport, sptr, snum, cons)
	sctp_t *sp;
	uint16_t sport;
	uint32_t *sptr;
	size_t snum;
	ulong cons;
{
	int err;

	assert(sp);

	if (!cons && !sport) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	if ((err = sctp_alloc_saddrs(sp, sport, sptr, snum))) {
		rare();
		return (err);
	}
	if (cons && (!sp->sanum || !sp->saddr)) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	if (cons)
		sp->s_state = SCTP_LISTEN;
	else
		sp->s_state = SCTP_CLOSED;
	if ((err = sctp_bind_hash(sp, cons))) {
		rare();
		return (err);
	}
	sp->conind = cons;
	return (0);
}

/*
 *  CONN_REQ:
 *  -------------------------------------------------------------------------
 *  Connect to the peer.  This launches the INIT process.
 */
int
sctp_conn_req(sp, dport, dptr, dnum, dp)
	sctp_t *sp;
	uint16_t dport;
	uint32_t *dptr;
	size_t dnum;
	mblk_t *dp;
{
	int err;

	assert(sp);

	if (!dport) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	if ((err = sctp_alloc_daddrs(sp, dport, dptr, dnum))) {
		rare();
		return (err);
	}
	if (!sp->daddr || !sp->danum) {
		rare();
		return (-EADDRNOTAVAIL);
	}

	sp->v_tag = sctp_get_vtag(sp->daddr->daddr, sp->saddr->saddr, sp->dport, sp->sport);
	sp->p_tag = 0;

	if ((err = sctp_conn_hash(sp))) {
		rare();
		return (err);
	}
	/*
	   XXX 
	 */
	if ((err = sctp_update_routes(sp, 1))) {
		rare();
		return (err);
	}

	sctp_reset(sp);		/* clear old information */
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->t_tsn = sp->v_tag;
	sp->t_ack = sp->v_tag - 1;
	sp->r_ack = 0;

	/*
	   fake a data request if data in conn req 
	 */
	if (dp) {
		seldom();
		if ((err = sctp_data_req(sp, sp->ppi, sp->sid, 0, 0, 0, dp))) {
			rare();
			return (err);
		}
	}
	if ((err = sctp_send_init(sp))) {
		rare();
		return (err);
	}
	return (0);

}

/*
 *  CONN_RES:
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_return_more(mblk_t *mp);

int
sctp_conn_res(sp, cp, ap, dp)
	sctp_t *sp;
	mblk_t *cp;
	sctp_t *ap;
	mblk_t *dp;
{
	int err;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;
	uint32_t *daddrs;
	uint32_t *saddrs;

	assert(sp);
	assert(cp);
	assert(ap);

	m = (struct sctp_cookie_echo *) cp->b_rptr;
	ck = (struct sctp_cookie *) m->cookie;
	daddrs = (uint32_t *) (ck + 1);
	saddrs = daddrs + ck->danum;

	sctp_unbind(ap);	/* we need to rebind the accepting stream */

	if ((err = sctp_alloc_saddrs(ap, ck->sport, saddrs, ck->sanum))) {
		rare();
		return (err);
	}
	if (!sctp_saddr_include(ap, ck->saddr, &err) && err) {
		rare();
		return (err);
	}

	if ((err = sctp_bind_hash(ap, ap->conind))) {
		rare();
		return (err);
	}

	if ((err = sctp_alloc_daddrs(ap, ck->dport, daddrs, ck->danum))) {
		rare();
		return (err);
	}
	if (!sctp_daddr_include(ap, ck->daddr, &err) && err) {
		rare();
		return (err);
	}

	ap->v_tag = ck->v_tag;
	ap->p_tag = ck->p_tag;

	if ((err = sctp_conn_hash(ap))) {
		rare();
		return (err);
	}
	/*
	   XXX 
	 */
	if ((err = sctp_update_routes(ap, 1))) {
		rare();
		return (err);
	}

	sctp_reset(ap);		/* clear old information */

	ap->n_istr = ck->n_istr;
	ap->n_ostr = ck->n_ostr;
	ap->t_tsn = ck->v_tag;
	ap->t_ack = ck->v_tag - 1;
	ap->r_ack = ck->p_tsn - 1;
	ap->p_rwnd = ck->p_rwnd;

	ap->s_state = SCTP_ESTABLISHED;
	/*
	   process any chunks bundled with cookie echo on accepting stream 
	 */
	if (sctp_return_more(cp) > 0)
		sctp_recv_msg(ap, cp);

	/*
	   fake a data request if data in conn res 
	 */
	if (dp) {
		if ((err = sctp_data_req(ap, ap->ppi, ap->sid, 0, 0, 0, dp))) {
			rare();
			return (err);
		}
	}
	sctp_send_cookie_ack(ap);

	/*
	   caller will unlink connect indication 
	 */
	return (0);
}

/*
 *  DATA_REQ:
 *  -------------------------------------------------------------------------
 */
int
sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)
	sctp_t *sp;
	uint32_t ppi;
	uint16_t sid;
	uint ord;			/* when non-zero, indicates ordered delivery */
	uint more;			/* when non-zero, indicates more data to follow */
	uint rcpt;			/* when non-zero, indicates receipt conf requested */
	mblk_t *mp;
{
	uint err = 0, flags = 0;
	sctp_strm_t *st;

	ensure(mp, return (-EFAULT));

	/*
	   don't allow zero-length data through 
	 */
	if (!msgdsize(mp)) {
		freemsg(mp);
		return (0);
	}

	if (!(st = sctp_ostrm_find(sp, sid, &err))) {
		rare();
		return (err);
	}

	/*
	   we probably want to data ack out of order as well 
	 */
#if 0
	if (rcpt || (ord && (sp->flags & SCTP_FLAG_DEFAULT_RC_SEL)))
#else
	if (rcpt)
#endif
		flags |= SCTPCB_FLAG_CONF;

	if (!ord) {
		flags |= SCTPCB_FLAG_URG;
		if (!st->x.more) {
			flags |= SCTPCB_FLAG_FIRST_FRAG;
			st->x.ppi = ppi;
		}
	} else {
		if (!st->n.more) {
			flags |= SCTPCB_FLAG_FIRST_FRAG;
			st->n.ppi = ppi;
		}
	}
	if (!more)
		flags |= SCTPCB_FLAG_LAST_FRAG;

	return sctp_send_data(sp, st, flags, mp);
}

/*
 *  RESET_REQ:
 *  -------------------------------------------------------------------------
 *  Don't know what to do here, probably nothing...
 *
 *  Gee we could keep a copy of the old cookie against the stream of we
 *  actively connected and send a COOKIE ECHO to generate a RESTART at the
 *  other end????
 */
int
sctp_reset_req(sp)
	sctp_t *sp;
{
	int err;
	/*
	   do nothing 
	 */
	if (sp->ops->sctp_reset_con && (err = sp->ops->sctp_reset_con(sp))) {
		rare();
		return (err);
	}
	return (0);
}

/*
 *  RESET_RES:
 *  -------------------------------------------------------------------------
 */
int
sctp_reset_res(sp)
	sctp_t *sp;
{
	mblk_t *cp;

	if (!(cp = bufq_dequeue(&sp->conq))) {
		rare();
		return (-EFAULT);
	}
	return sctp_conn_res(sp, cp, sp, NULL);
}

/*
 *  DISCON_REQ:
 *  -------------------------------------------------------------------------
 */
int
sctp_discon_req(sp, cp)
	sctp_t *sp;
	mblk_t *cp;
{
	/*
	 *  Caller must ensure that sp and cp (if any) are correct and
	 *  appropriate.
	 */
	if (cp) {
		struct iphdr *iph = (struct iphdr *) cp->b_datap->db_base;
		struct sctphdr *sh = (struct sctphdr *) (cp->b_datap->db_base + (iph->ihl << 2));

		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		/*
		   conn ind will be unlinked by caller 
		 */
		return (0);
	}
	if ((1 << sp->s_state) & (SCTPF_NEEDABORT)) {
		sctp_send_abort(sp);
	} else
		rare();
	sctp_disconnect(sp);
	return (0);
}

/*
 *  ORDREL_REQ:
 *  -------------------------------------------------------------------------
 */
int
sctp_ordrel_req(sp)
	sctp_t *sp;
{
	switch (sp->s_state) {
	case SCTP_ESTABLISHED:
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown(sp);
		else
			sp->s_state = SCTP_SHUTDOWN_PENDING;
		return (0);
	case SCTP_SHUTDOWN_RECEIVED:
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown_ack(sp);
		else
			sp->s_state = SCTP_SHUTDOWN_RECVWAIT;
		return (0);
	}
	rare();
	// ptrace(("sp->s_state = %d\n", sp->s_state));
	return (-EPROTO);
}

/*
 *  UNBIND_REQ:
 *  -------------------------------------------------------------------------
 */
int
sctp_unbind_req(sp)
	sctp_t *sp;
{
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		/*
		   can't wait for SHUTDOWN COMPLETE any longer 
		 */
		sctp_disconnect(sp);
	case SCTP_CLOSED:
	case SCTP_LISTEN:
		sctp_unbind(sp);
		return (0);
	}
	rare();
	return (-EPROTO);
}

/*
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */
/*
 *  RETURN VALUE FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  RETURN VALUE when expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_more(mp)
	mblk_t *mp;
{
	int ret;
	struct sctpchdr *ch;
	assert(mp);
	ch = (struct sctpchdr *) mp->b_rptr;
	mp->b_rptr += PADC(ntohs(ch->len));
	ret = mp->b_wptr - mp->b_rptr;
	ret = (ret < 0 || (0 < ret && ret < sizeof(struct sctpchdr))) ? -EMSGSIZE : ret;
	unusual(ret < 0);
	return (ret);
}

/*
 *  RETURN VALUE when not expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_stop(mp)
	mblk_t *mp;
{
	int ret = sctp_return_more(mp) ? -EPROTO : 0;
	unusual(ret < 0);
	return (ret);
}

#if 0
/*
 *  RETURN VALUE when expecting specific chunk or nothing
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_check(mp, ctype)
	mblk_t *mp;
	uint ctype;
{
	int ret = sctp_return_more(mp);
	ret = (ret > 0 && ((struct sctpchdr *) mp->b_rptr)->type != ctype) ? -EPROTO : ret;
	unusual(ret < 0);
	return (ret);
}
#endif

/*
   is s2<=s1<=s3 ? 
 */
#define between(__s1,__s2,__s3)((uint32_t)(__s3)-(uint32_t)(__s2)>=(uint32_t)(__s1)-(uint32_t)(__s2))
#define before(__s1,__s2) (((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))<0))
#define after(__s1,__s2) (((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))<0))

/*
 *  RESET IDLE
 *  -------------------------------------------------------------------------
 *  Reset the idle timer for generation of heartbeats.  Stop any heartbeating
 *  that we might be involved in at the moment.
 */
STATIC void
sctp_reset_idle(sd)
	sctp_daddr_t *sd;
{
	unsigned long rtt;
	assert(sd);
	if (sd->timer_heartbeat)
		untimeout(xchg(&sd->timer_heartbeat, 0));
	rtt = sd->rto + sd->hb_itvl + ((jiffies & 0x1) * (sd->rto >> 1));
	mod_timeout(sd->sp, &sd->timer_idle, &sctp_idle_timeout, sd, rtt);
}

/*
 *  RTT CALC
 *  -------------------------------------------------------------------------
 *  Round Trip Time calculations for messages acknowledged on the first
 *  transmission.  When a message is acknowledged, this function peforms and
 *  update of the RTT calculation if appropriate.  It is called by
 *  sctp_ack_calc for control chunks which expect acknowledgements, and by
 *  sctp_dest_calc when DATA chunks are acknolwedged on first transmission via
 *  SACK or SHUTDOWN chunks, and by sctp_recv_heartbeat_ack when calculating
 *  RTTs for HEARTBEAT chunks.
 */
STATIC void
sctp_rtt_calc(sd, time)
	sctp_daddr_t *sd;
	unsigned long time;
{
	unsigned long rtt;
	unsigned long rttvar;

	assert(sd);
	ensure(jiffies >= time, return);

	/*
	   RFC 2960 6.3.1 
	 */
	rtt = jiffies - time;
	if (sd->srtt) {
		/*
		   RFC 2960 6.3.1 (C3) 
		 */
		rttvar = sd->srtt > rtt ? sd->srtt - rtt : rtt - sd->srtt;
		sd->rttvar += (rttvar - sd->rttvar) >> 2;
		sd->srtt += (rtt - sd->srtt) >> 3;
		sd->rto = rtt + (sd->rttvar << 2);
	} else {
		/*
		   RFC 2960 6.3.1 (C2) 
		 */
		sd->rttvar = rtt >> 1;
		sd->srtt = rtt;
		sd->rto = rtt + (rtt << 1);
	}
	sd->rttvar = sd->rttvar ? sd->rttvar : 1;	/* RFC 2960 6.3.1 (G1) */
	sd->rto = sd->rto_min > sd->rto ? sd->rto_min : sd->rto;	/* RFC 2960 6.3.1 (C6) */
	sd->rto = sd->rto_max < sd->rto ? sd->rto_max : sd->rto;	/* RFC 2960 6.3.1 (C7) */

#ifdef _DEBUG
#ifdef ERROR_GENERATOR
	if (sd->retransmits && (sd->sp->options & SCTP_OPTION_BREAK)
	    && (sd->packets > BREAK_GENERATOR_LEVEL))
		ptrace(("Aaaarg! Reseting counts for address %d.%d.%d.%d\n",
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff,
			(sd->daddr >> 24) & 0xff));
#endif
#endif
	sd->dups = 0;
	/*
	   RFC 2960 8.2 
	 */
	sd->retransmits = 0;
	/*
	   RFC 2960 8.1 
	 */
	sd->sp->retransmits = 0;
	/*
	   reset idle timer 
	 */
	sctp_reset_idle(sd);
}

#ifndef SCTP_DESTF_DROP
#define SCTP_DESTF_DROP 0x10000000	/* destination is dropping packets */
#endif
/*
 *  DEST CALC
 *  -------------------------------------------------------------------------
 *  This function performs delayed processing of RTT and CWND calculations on
 *  destinations which need it and is called from sctp_recv_shutdown and
 *  sctp_recv_sack once all ack and gap ack processing is complete.  This
 *  performs the necessary calculations for each destination before closing
 *  processing of the received SHUTDOWN or SACK chunk.
 */
STATIC void
sctp_dest_calc(sp)
	sctp_t *sp;
{
	sctp_daddr_t *sd;

	assert(sp);
	usual(sp->daddr);

	for (sd = sp->daddr; sd; sd = sd->next) {
		size_t accum;

		if (sd->when) {
			/*
			   calculate RTT based on latest sent acked TSN 
			 */
			sctp_rtt_calc(sd, sd->when);
			sd->when = 0;
		}
		/*
		 *  NOTE:- first we grow the congestion window according to
		 *  whatever TSNs were cummulatively acked to the destination
		 *  and then we back off if the destination is dropping (as
		 *  indiciated by gap reports).
		 */
		if ((accum = sd->ack_accum)) {
			if (sd->cwnd <= sd->ssthresh) {
				/*
				   RFC 2960 7.2.1 
				 */
				if (sd->in_flight > sd->cwnd)
					sd->cwnd += accum < sd->mtu ? accum : sd->mtu;
			} else {
				/*
				   RFC 2960 7.2.2 
				 */
				if (sd->in_flight > sd->cwnd)
					sd->cwnd += sd->mtu;
			}
			/*
			   credit of destination (accum) 
			 */
			normal(sd->in_flight >= accum);
			sd->in_flight = sd->in_flight > accum ? sd->in_flight - accum : 0;

			/*
			   RFC 2960 6.3.2 (R3) 
			 */
			if (sd->timer_retrans)
				untimeout(xchg(&sd->timer_retrans, 0));

			sd->ack_accum = 0;
		}
		if (sd->flags & SCTP_DESTF_DROP) {
			/*
			   RFC 2960 7.2.4 (2), 7.2.3 
			 */
			sd->ssthresh = sd->cwnd >> 1 > sd->mtu << 1 ? sd->cwnd >> 1 : sd->mtu << 1;
			sd->cwnd = sd->ssthresh;
			sd->flags &= ~SCTP_DESTF_DROP;
		}
		/*
		   RFC 2960 6.3.2 (R2) 
		 */
		if (!sd->in_flight && sd->timer_retrans)
			untimeout(xchg(&sd->timer_retrans, 0));
		if (sd->in_flight && !sd->timer_retrans)
			set_timeout(sp, &sd->timer_retrans, &sctp_retrans_timeout, sd, sd->rto);
	}
}

/*
 *  CUMM ACK
 *  -------------------------------------------------------------------------
 *  This function is responsible for moving the cummulative ack point.  The
 *  sender must check that the ack is valid (monotonically increasing), but it
 *  may be the same TSN as was previously acknowledged.  When the ack point
 *  advances, this function stikes DATA chunks from the retransmit buffer.
 *  This also indirectly updates the amount of data outstanding for
 *  retransmission.  This function is called by both sctp_recv_sack and
 *  sctp_recv_shutdown.
 *
 *  We only perform calculations on TSNs that were not previously acknowledged
 *  by a GAP Ack.  If the TSN has not been retransmitted (Karn's algorithm RFC
 *  2960 6.3.1 (C5)), the destination that it was set to is marked for RTT
 *  calculation update.  If the TSN is currently marked for retransmission
 *  awaiting available cwnd, it is unmarked and retranmission of the TSN is
 *  aborted.  If the TSN is not marked for retransmission, the destination's
 *  acked bytes accumulator is increased and the need for a CWND calculation
 *  for the destination indicated.  The association number of bytes in flight
 *  is decreased to account for the acknowledged TSN.  If any calculations are
 *  pending as a result of the ack, the function returns non-zero to
 *  indication that calculations (RTT and CWND) must be processed before
 *  message processing is complete.
 */
STATIC void
sctp_cumm_ack(sp, ack)
	sctp_t *sp;
	uint32_t ack;
{
	assert(sp);

	/*
	   make sure we actually move the ack point 
	 */
	if (after(ack, sp->t_ack)) {
		mblk_t *mp;
		sp->t_ack = ack;

		while ((mp = bufq_head(&sp->rtxq)) && !after(SCTP_TCB(mp)->tsn, ack)) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
				sctp_daddr_t *sd = cb->daddr;
				/*
				   RFC 2960 6.3.1 (C5) 
				 */
				if (cb->trans < 2) {
					/*
					   remember latest transmitted packet acked for rtt calc 
					 */
					sd->when = sd->when > cb->when ? sd->when : cb->when;
				}
				if (cb->flags & SCTPCB_FLAG_RETRANS) {
					cb->flags &= ~SCTPCB_FLAG_RETRANS;
					sp->nrtxs--;
				} else {
					size_t dlen = cb->dlen;
					/*
					   credit destination (later) 
					 */
					normal(sd->in_flight >= sd->ack_accum + dlen);
					sd->ack_accum =
					    sd->in_flight >
					    sd->ack_accum + dlen ? sd->ack_accum +
					    dlen : sd->in_flight;
					/*
					   credit association (now) 
					 */
					normal(sp->in_flight >= dlen);
					sp->in_flight =
					    sp->in_flight > dlen ? sp->in_flight - dlen : 0;
				}
				cb->flags |= SCTPCB_FLAG_SACKED;	/* hack */
			}
			if ((cb->flags & SCTPCB_FLAG_CONF) && sp->ops->sctp_datack_ind)
				bufq_queue(&sp->ackq, bufq_dequeue(&sp->rtxq));
			else
				freemsg(bufq_dequeue(&sp->rtxq));
			/*
			 *  Need to back-enable the write queue if required.
			 */
			if (sp->wq->q_count)
				qenable(sp->wq);
		}
	}
	return;
}

/*
 *  ACK CALC
 *  -------------------------------------------------------------------------
 *  This one is for messages for which a timer is set an retransmission occurs
 *  until acknowledged.  We stop the timer, perform an RTT calculation if the
 *  message was not retransmitted, free the retry buffer, and clear the
 *  association and destination retransmission counts.
 */
STATIC void
sctp_ack_calc(sp, tp)
	sctp_t *sp;			/* private structure for stream */
	tid_t *tp;			/* timer to cancel */
{
	sctp_tcb_t *cb;
	sctp_daddr_t *sd;

	assert(sp);
	assert(sp->retry);

	cb = SCTP_TCB(sp->retry);
	sd = cb->daddr;

	untimeout(xchg(tp, 0));
	if (sd) {
		if (cb->trans < 2)
			sctp_rtt_calc(sd, cb->when);
	} else
		rare();
	freechunks(xchg(&sp->retry, NULL));
}

/*
 *  RECV DATA
 *  -------------------------------------------------------------------------
 *  We have received a DATA chunk in a T-Provider s_state where is it valid to
 *  receive DATA (TS_DATA_XFER and TS_WIND_ORDREL).  We can also receive data
 *  chunks in TS_IDLE or TS_WRES_CIND on a listening socket bundled with a
 *  COOKIE ECHO.  In any other states we discard the data.  Because no other
 *  chunks can be bundled after a DATA chunk (just yet), we process all the
 *  DATA chunks in the remainder of the message in a single loop here.
 */
STATIC int
sctp_recv_data(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	int blen;
	size_t plen;
	uint newd;
	assert(sp);
	assert(mp);
	if (!((1 << sp->s_state) & (SCTPF_RECEIVING)))
		goto outstate;
	newd = 0;		/* number of new data chunks */
	for (; (blen = mp->b_wptr - mp->b_rptr); mp->b_rptr += plen) {
		mblk_t *dp, *db;
		sctp_tcb_t *cb;
		sctp_strm_t *st;
		struct sctp_data *m = (struct sctp_data *) mp->b_rptr;
		size_t clen = ntohs(m->ch.len);
		size_t dlen = clen > sizeof(*m) ? clen - sizeof(*m) : 0;
		uint32_t tsn = ntohl(m->tsn);
		uint16_t sid = ntohs(m->sid);
		uint16_t ssn = ntohs(m->ssn);
		uint32_t ppi = ntohl(m->ppi);
		uint flags = (m->ch.flags);
		int ord = !(flags & SCTPCB_FLAG_URG);
		int more = !(flags & SCTPCB_FLAG_LAST_FRAG);
		sctp_tcb_t **gap;
		err = 0;
		plen = PADC(clen);
		if (blen <= 0 || blen < sizeof(*m) || clen < sizeof(*m) || blen < plen)
			goto emsgsize;
		if (m->ch.type != SCTP_CTYPE_DATA)
			goto eproto;
		if (dlen <= 0)
			goto baddata;
		if (sid >= sp->n_istr)
			goto badsid;
		if (sp->a_rwnd <=
		    bufq_size(&sp->oooq) + bufq_size(&sp->dupq) + bufq_size(&sp->rcvq))
			goto ebusy;
		if (!(st = sctp_istrm_find(sp, sid, &err)))
			goto enomem;
		if (!(dp = dupb(mp)))
			goto enobufs;
		/*
		   trim copy to data only 
		 */
		dp->b_wptr = dp->b_rptr + clen;
		dp->b_rptr += sizeof(*m);
		/*
		   fast path, nothing backed up 
		 */
		if (tsn == sp->r_ack + 1 && !bufq_head(&sp->rcvq) && !bufq_head(&sp->oooq)) {
			/*
			   we have next expected TSN, just process it 
			 */
			if ((err = sp->ops->sctp_data_ind(sp, ppi, sid, ssn, tsn, ord, more, dp)))
				goto free_error;
			if (ord) {
				if (!(st->n.more = more))
					st->ssn = ssn;
			} else {
				st->x.more = more;
			}
			sp->r_ack++;
			newd++;
			continue;
		}
		if (!(db = allocb(sizeof(*cb), BPRI_MED)))
			goto free_nobufs;
		gap = &sp->gaps;
		cb = (sctp_tcb_t *) db->b_rptr;
		db->b_datap->db_type = M_CTL;
		bzero(db->b_wptr, sizeof(*cb));
		db->b_cont = dp;
		cb->dlen = dlen;
		cb->when = jiffies;
		cb->tsn = tsn;
		cb->sid = sid;
		cb->ssn = ssn;
		cb->ppi = ppi;
		cb->flags = flags & 0x7;
		cb->daddr = sp->caddr;
		cb->mp = db;
		cb->st = st;
		cb->tail = cb;
		cb->head = cb;
		usual(sp->caddr);
		if (after(tsn, sp->r_ack)) {
			for (; (*gap); gap = &((*gap)->tail->next)) {
				if (between(tsn, (*gap)->tsn, (*gap)->tail->tsn))
					goto sctp_recv_data_duplicate;
				if (before(tsn, (*gap)->tsn)) {
					/*
					   insert in front of gap 
					 */
					bufq_insert(&sp->oooq, (*gap)->mp, db);
					cb->next = (*gap);
					(*gap) = cb;
					sp->ngaps++;
				} else if (tsn == (*gap)->tail->tsn + 1) {
					/*
					   expand at end of gap 
					 */
					bufq_queue(&sp->oooq, db);
					cb->next = (*gap)->tail->next;
					(*gap)->tail->next = cb;
					cb->head = (*gap);
					(*gap)->tail = cb;
				} else
					continue;
				if (cb->next && cb->next->tsn == tsn + 1) {
					/*
					   join two gaps 
					 */
					cb->next->tail->head = (*gap);
					(*gap)->tail = cb->next->tail;
					usual(sp->ngaps);
					sp->ngaps--;
				}
				break;
			}
			if (!(*gap)) {
				/*
				   append to list 
				 */
				bufq_queue(&sp->oooq, db);
				cb->next = (*gap);
				(*gap) = cb;
				sp->ngaps++;
			}
			sp->nunds++;	/* more undelivered data */
			newd++;
		} else {
		      sctp_recv_data_duplicate:
			/*
			   message is a duplicate tsn 
			 */
			bufq_queue(&sp->dupq, db);
			cb->next = sp->dups;
			sp->dups = cb;
			sp->ndups++;
		}
		continue;
	      free_error:
		freemsg(dp);
		rare();
		break;
	      free_nobufs:
		freemsg(dp);
		rare();
		err = -ENOBUFS;
		break;		/* couldn't allocate buffer */
	      enobufs:
		rare();
		err = -ENOBUFS;
		break;		/* couldn't allocate buffer */
	      enomem:
		rare();
		err = -ENOMEM;
		break;		/* couldn't allocate stream */
	      ebusy:
		rare();
		err = -EBUSY;
		break;		/* flow controlled (discard) */
	      badsid:
		rare();
		sctp_send_error(sp, SCTP_CAUSE_INVALID_STR, &m->sid, sizeof(m->sid));
		continue;	/* just skip that DATA chunk */
	      baddata:
		rare();
		/*
		   RFC 2960 6.2: ...no user data... 
		 */
		if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
			if ((err =
			     sp->ops->sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, SCTP_CAUSE_NO_DATA,
						      NULL)))
				break;
			if ((1 << sp->s_state) & (SCTPF_NEEDABORT)) {
				sctp_send_abort_error(sp, SCTP_CAUSE_NO_DATA, &m->tsn,
						      sizeof(m->tsn));
			} else
				rare();
			sctp_disconnect(sp);
			return (-EPROTO);
		} else
			rare();
	      eproto:
		rare();
		err = -EPROTO;
		break;		/* non-data chunk after data */
	      emsgsize:
		rare();
		err = -EMSGSIZE;
		break;		/* bad message or chunk sizes */
	}
	if (newd) {		/* we have underlivered data and new data */
		sctp_tcb_t *cb, *cb_next = sp->gaps;
		/*
		   try to deliver undelivered data now 
		 */
		while ((cb = cb_next)) {
			cb_next = cb->next;
			if (!(cb->flags & SCTPCB_FLAG_DELIV)) {
				mblk_t *db;
				sctp_strm_t *st = cb->st;
				uint flags = cb->flags;
				int ord = !(flags & SCTPCB_FLAG_URG);
				int more = !(flags & SCTPCB_FLAG_LAST_FRAG);
				int frag = !(flags & SCTPCB_FLAG_FIRST_FRAG);
				if (after(cb->tsn, sp->r_ack + 1)) {
					/*
					   after gap 
					 */
					if (frag)
						continue;
					if (ord) {
						if (st->n.more)
							continue;
						if (cb->ssn != ((st->ssn + 1) & 0xffff))
							continue;
					} else {
						if (st->x.more)
							continue;
						if (more)
							continue;
					}
				}
				if ((db = dupb(cb->mp))) {
					db->b_cont = cb->mp->b_cont;
					cb->mp->b_cont = NULL;
					bufq_queue(&sp->rcvq, db);
					cb->flags |= SCTPCB_FLAG_DELIV;
					if (ord) {
						if (!(st->n.more = more))
							st->ssn = cb->ssn;
					} else {
						st->x.more = more;
					}
					sp->nunds--;
				} else {
					rare();
					break;
				}	/* no buffers */
			}
			if (cb->tsn == sp->r_ack + 1) {
				sp->r_ack++;
				sp->sackf |= SCTP_SACKF_NOD;
			}
			if ((cb->flags & SCTPCB_FLAG_DELIV) && !after(cb->tsn, sp->r_ack)) {
				assure(sp->gaps == cb);
				if (!(sp->gaps = cb->next))
					sp->ngaps = 0;
				else {
					if (cb == cb->tail)
						sp->ngaps--;
					else {
						cb->next->tail = cb->tail;
						cb->tail->head = cb->next;
					}
				}
				freemsg(bufq_unlink(&sp->oooq, cb->mp));
			}
		}
	}
	/*
	   RFC 2960 6.2 
	 */
	if (sp->ndups) {
		/*
		 *  IMPLEMENTATION NOTE:- If we are receiving duplicates the
		 *  probability is high that our SACKs aren't getting through
		 *  (or have been delayed too long).  If we do not have a sack
		 *  pending (one being delayed) then we will peg the
		 *  duplicates against the destination.  This will change
		 *  where we are sending SACKs.
		 *
		 *  Because we are being cruel to this destination and we
		 *  don't really know that this is where the offending SACKs
		 *  were sent, we send an immediate heartbeat if there is no
		 *  data is in flight to the destination (i.e., no
		 *  retransmission timer running for the destination).  This
		 *  fixes some sticky problems when one-way data is being
		 *  sent.
		 */
		sctp_daddr_t *sd = sp->taddr;
		sp->sackf |= SCTP_SACKF_DUP;
		if (sd && !(sp->sackf & SCTP_SACKF_NEW)) {
			sd->dups += sp->ndups;
			if (!sd->in_flight) {
				if (sd->timer_idle)
					untimeout(xchg(&sd->timer_idle, 0));
				sctp_send_heartbeat(sp, sd);
			}
		}
	}
	/*
	   RFC 2960 7.2.4 
	 */
	if (sp->ngaps) {
		sp->sackf |= SCTP_SACKF_GAP;
	}
	/*
	   RFC 2960 6.2 
	 */
	if (newd) {
		sp->sackf += ((sp->sackf & 0x3) < 3) ? SCTP_SACKF_NEW : 0;
		/*
		 *  IMPLEMENTATION NOTE:-  The SACK timer is probably too
		 *  slow.  For unidirectional operation, the sender may have
		 *  timed out before we send a sack.  We should really not
		 *  wait any longer than some fraction of the RTO for the
		 *  destination from which we are receiving (or sending) data.
		 *  However, if we wait too long we will just get a
		 *  retransmission and a dup.
		 *
		 *  If the sack delay is set to zero, we do not set the timer,
		 *  but issue the sack immediately.
		 */
		if (!sp->timer_sack) {
			if (sp->max_sack > 0)
				set_timeout(sp, &sp->timer_sack, &sctp_sack_timeout, sp,
					    sp->max_sack);
			else
				sp->sackf |= SCTP_SACKF_NOD;
		}
	}
	if (sp->s_state == SCTP_SHUTDOWN_SENT)
		sctp_send_shutdown(sp);
      done:
	/*
	 *  We should not break with ENOBUFS or ENOMEM or EBUSY
	 *  here... I'm not sure that we have left the buffer in a
	 *  state where it can be put back on the queue and processed
	 *  later.
	 */
	switch (err) {
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOMEM:
	case -EBUSY:
		/*
		   SCTP Implementor's Guide Section 2.15.1 
		 */
		sp->sackf |= SCTP_SACKF_NOD;
		err = 0;
	}
	return (err);
      outstate:
	/*
	 *  We have received DATA in the wrong s_state.  If so, it is probably
	 *  an old packet that was stuck in the network and just got delivered
	 *  to us.  Nevertheless we should just ignore any message containing
	 *  DATA when we are not expecting it.  The only exception to this
	 *  might be receiving DATA in the COOKIE-WAIT s_state.  There should
	 *  not be data hanging around in the network that matches our tags.
	 *  If that is the case, we should abandon the connection attempt and
	 *  let the user try again with a different verification tag.
	 */
	rare();
	if (sp->s_state == SCTP_COOKIE_WAIT) {
		rare();
		ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
		if (!(err = sp->ops->sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -EPROTO, NULL)))
			sctp_disconnect(sp);
		err = -EPROTO;
	}
	goto done;
}

/*
 *  CLEANUP READ
 *  -------------------------------------------------------------------------
 *  This is called to clean up the read queue by the STREAMS read service
 *  routine.  This permits backenabling to work.
 */
void
sctp_cleanup_read(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	assert(sp);

	while ((mp = bufq_head(&sp->ackq))) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if (!sp->ops->sctp_datack_ind(sp, cb->ppi, cb->sid, cb->ssn, cb->tsn)) {
			mp = bufq_dequeue(&sp->ackq);
			freemsg(mp);
			continue;
		}
		seldom();
		break;		/* error on delivery (ENOBUFS, EBUSY) */
	}
	if (bufq_head(&sp->rcvq)) {
		int need_sack = (sp->a_rwnd <= bufq_size(&sp->oooq)
				 + bufq_size(&sp->dupq)
				 + bufq_size(&sp->rcvq));
		while ((mp = bufq_head(&sp->rcvq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);
			sctp_strm_t *st = cb->st;
			int ord = !(cb->flags & SCTPCB_FLAG_URG);
			int more = !(cb->flags & SCTPCB_FLAG_LAST_FRAG);

			ensure(st, return);

			if (!sp->ops->
			    sctp_data_ind(sp, cb->ppi, cb->sid, cb->ssn, cb->tsn, ord, more,
					  mp->b_cont)) {
				mp = bufq_dequeue(&sp->rcvq);
				mp->b_cont = NULL;
				freemsg(mp);
				if (ord) {
					if (!(st->n.more = more))
						st->ssn = cb->ssn;
				} else
					st->x.more = more;
				if (!need_sack)
					continue;
				/*
				 *  Should really do SWS here.
				 */
				sp->sackf |= SCTP_SACKF_NOD;
				need_sack = 0;
				continue;
			}
			seldom();
			break;	/* error on delivery (ENOBUFS, EBUSY) */
		}
	}
}

/*
 *  RECV SACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_sack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	assert(sp);
	assert(mp);

	if ((1 << sp->s_state) & (SCTPF_SENDING)) {

		mblk_t *dp;
		struct sctp_sack *m = (struct sctp_sack *) mp->b_rptr;
		uint32_t ack = htonl(m->c_tsn);
		uint32_t rwnd = htonl(m->a_rwnd);
		size_t ngaps = htons(m->ngaps);
		size_t ndups = htons(m->ndups);
		uint16_t *gap = m->gaps;

		/*
		   RFC 2960 6.2.1 (D) i) 
		 */
		if (before(ack, sp->t_ack)) {
			rare();
			return 0;
		}
		/*
		 *  If the receive window is increasing and we have data in the write
		 *  queue, we might need to backenable.
		 */
		if (rwnd > sp->p_rwnd && sp->wq->q_count)
			qenable(sp->wq);

		/*
		   RFC 2960 6.2.1 (D) ii) 
		 */
		sp->p_rwnd = rwnd;	/* we keep in_flight separate from a_rwnd */

		/*
		 *  advance the cummulative ack point and check need to perform
		 *  per-round-trip and cwnd calcs
		 */
		sctp_cumm_ack(sp, ack);

		if (ndups) {
			// sp->sackf |= SCTP_SACKF_DUP;
			/*
			 *  TODO: we could look through the list of duplicate TSNs.
			 *  Duplicate TSNs really means that the peer's SACKs aren't
			 *  getting back to us.  But there is nothing really that we
			 *  can do about that.  The peer has already detected the
			 *  problem and should sent SACKs via an alternative route if
			 *  possible.  But that's how this SACK go here...
			 */
		}
		/*
		   process gap acks 
		 */
		if (!ngaps) {
			/*
			   perform fast retransmission algorithm on missing TSNs 
			 */
			for (dp = bufq_head(&sp->rtxq); dp; dp = dp->b_next) {
				sctp_tcb_t *cb = SCTP_TCB(dp);
				/*
				   RFC 2960 7.2.4 
				 */
				if (!(cb->flags & SCTPCB_FLAG_RETRANS) && ++(cb->sacks) >= 4) {
					size_t dlen = cb->dlen;
					sctp_daddr_t *sd = cb->daddr;
					/*
					   RFC 2960 7.2.4 (1) 
					 */
					cb->flags |= SCTPCB_FLAG_RETRANS;
					sp->nrtxs++;
					cb->sacks = 0;
					/*
					   RFC 2960 7.2.4 (2) 
					 */
					sd->flags |= SCTP_DESTF_DROP;
					/*
					   credit destination (now) 
					 */
					normal(sd->in_flight >= dlen);
					sd->in_flight =
					    sd->in_flight > dlen ? sd->in_flight - dlen : 0;
					/*
					   credit association (now) 
					 */
					normal(sp->in_flight >= dlen);
					sp->in_flight =
					    sp->in_flight > dlen ? sp->in_flight - dlen : 0;
				}
				/*
				   RFC 2960 6.3.2 (R4) 
 *//*
   reneg 
 */
				if (cb->flags & SCTPCB_FLAG_SACKED) {
					sctp_daddr_t *sd = cb->daddr;
					cb->flags &= ~SCTPCB_FLAG_SACKED;
					if (!sd->timer_retrans) {
						set_timeout(sp, &sd->timer_retrans,
							    &sctp_retrans_timeout, sd, sd->rto);
					} else
						seldom();
				}
			}
		} else {
			/*
			   perform fast retransmission algorithm on gaps 
			 */
			while (ngaps--) {
				uint32_t beg = ack + ntohs(*gap++);
				uint32_t end = ack + ntohs(*gap++);
				if (before(end, beg)) {
					rare();
					continue;
				}
				/*
				   move to the acks 
				 */
				dp = bufq_head(&sp->rtxq);
				for (; dp && before(SCTP_TCB(dp)->tsn, beg); dp = dp->b_next)
					SCTP_TCB(dp)->flags |= SCTPCB_FLAG_NACK;
				/*
				   sack the acks 
				 */
				for (; dp && !after(SCTP_TCB(dp)->tsn, end); dp = dp->b_next)
					SCTP_TCB(dp)->flags |= SCTPCB_FLAG_ACK;
			}
			/*
			   walk the whole retrans buffer looking for holes and renegs 
			 */
			for (dp = bufq_head(&sp->rtxq); dp; dp = dp->b_next) {
				sctp_tcb_t *cb = SCTP_TCB(dp);
				/*
				   msg is inside gapack block 
				 */
				if (cb->flags & SCTPCB_FLAG_ACK) {
					cb->flags &= ~SCTPCB_FLAG_ACK;
					cb->flags &= ~SCTPCB_FLAG_NACK;
					if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
						sctp_daddr_t *sd = cb->daddr;
						cb->flags |= SCTPCB_FLAG_SACKED;
						/*
						   RFC 2960 6.3.1 (C5) 
						 */
						if (cb->trans < 2) {
							/*
							   remember latest transmitted packet acked 
							   for rtt calc 
							 */
							sd->when =
							    sd->when >
							    cb->when ? sd->when : cb->when;
						}
						if (cb->flags & SCTPCB_FLAG_RETRANS) {
							cb->flags &= ~SCTPCB_FLAG_RETRANS;
							sp->nrtxs--;
						} else {
							size_t dlen = cb->dlen;
							/*
							   credit destination 
							 */
							normal(sd->in_flight >= dlen);
							sd->in_flight =
							    sd->in_flight >
							    dlen ? sd->in_flight - dlen : 0;
							/*
							   credit association 
							 */
							normal(sp->in_flight >= dlen);
							sp->in_flight =
							    sp->in_flight >
							    dlen ? sp->in_flight - dlen : 0;
						}
					}
					continue;
				}
				/*
				   msg is between gapack blocks 
				 */
				if (cb->flags & SCTPCB_FLAG_NACK) {
					cb->flags &= ~SCTPCB_FLAG_NACK;
					/*
					   RFC 2960 7.2.4 
					 */
					if (!(cb->flags & SCTPCB_FLAG_RETRANS)
					    && ++(cb->sacks) >= 4) {
						size_t dlen = cb->dlen;
						sctp_daddr_t *sd = cb->daddr;
						/*
						   RFC 2960 7.2.4 (1) 
						 */
						cb->flags |= SCTPCB_FLAG_RETRANS;
						sp->nrtxs++;
						cb->sacks = 0;
						/*
						   RFC 2960 7.2.4 (2) 
						 */
						sd->flags |= SCTP_DESTF_DROP;
						/*
						   credit destination (now) 
						 */
						normal(sd->in_flight >= dlen);
						sd->in_flight =
						    sd->in_flight > dlen ? sd->in_flight - dlen : 0;
						/*
						   credit association (now) 
						 */
						normal(sp->in_flight >= dlen);
						sp->in_flight =
						    sp->in_flight > dlen ? sp->in_flight - dlen : 0;
					}
					/*
					   RFC 2960 6.3.2 (R4) 
 *//*
   reneg 
 */
					if (cb->flags & SCTPCB_FLAG_SACKED) {
						sctp_daddr_t *sd = cb->daddr;
						cb->flags &= ~SCTPCB_FLAG_SACKED;
						if (!sd->timer_retrans) {
							set_timeout(sp, &sd->timer_retrans,
								    &sctp_retrans_timeout, sd,
								    sd->rto);
						} else
							seldom();
					}
					continue;
				}
				/*
				   msg is after all gapack blocks 
				 */
				break;
			}
		}
		sctp_dest_calc(sp);

		if (((1 << sp->
		      s_state) & (SCTPF_SHUTDOWN_PENDING | SCTPF_SHUTDOWN_RECEIVED |
				  SCTPF_SHUTDOWN_RECVWAIT))
		    && !bufq_head(&sp->sndq)
		    && !bufq_head(&sp->rtxq)) {
			seldom();
			/*
			 *  After receiving a cummulative ack, I want to check if the
			 *  sndq and rtxq is empty and a SHUTDOWN or SHUTDOWN-ACK is
			 *  pending.  If so, I want to issue these primitives.
			 */
			switch (sp->s_state) {
			case SCTP_SHUTDOWN_PENDING:
				/*
				   Send the SHUTDOWN I didn't send before. 
				 */
				sctp_send_shutdown(sp);
				break;
			case SCTP_SHUTDOWN_RECEIVED:
				/*
				   Send the SHUTDOWN-ACK I didn't send before 
				 */
				if (!sp->ops->sctp_ordrel_ind)
					sctp_send_shutdown_ack(sp);
				break;
			case SCTP_SHUTDOWN_RECVWAIT:
				/*
				   Send the SHUTDOWN-ACK I didn't send before 
				 */
				sctp_send_shutdown_ack(sp);
				break;
			default:
				never();
				break;
			}
		}
	} else {
		rare();
		/*
		 *  We may have received a SACK in the wrong s_state.  Because
		 *  SACKs are completely advisory, there is no reason to get
		 *  too upset about this.  Simply ignore them.  No need to
		 *  process them.
		 */
	}
	{
		int ret;
		if ((ret = sctp_return_more(mp)) > 0) {
			struct sctpchdr *ch = (struct sctpchdr *) mp->b_rptr;
			switch (ch->type) {
				/*
				   RFC 2960 6 
				 */
			case SCTP_CTYPE_DATA:
				/*
				   RFC 2960 6.5 
				 */
			case SCTP_CTYPE_ERROR:
				/*
				   RFC 2960 3.3.7 
				 */
			case SCTP_CTYPE_ABORT:
				break;
			default:
				rare();
				return (-EPROTO);
			}
		}
		return (ret);
	}
}

/*
 *  RECV ERROR
 *  -------------------------------------------------------------------------
 *  We have received an ERROR chunk in opening, connected or closing states.
 */
STATIC int
sctp_recv_error(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct sctp_error *m;
	struct sctpehdr *eh;
	int ecode;

	assert(sp);
	assert(mp);

	seldom();

	m = (struct sctp_error *) mp->b_rptr;
	eh = (struct sctpehdr *) (m + 1);
	ecode = ntohs(eh->code);

	switch (ecode) {
	case SCTP_CAUSE_STALE_COOKIE:
		if (sp->s_state == SCTP_COOKIE_ECHOED) {
			sctp_tcb_t *cb;
			sctp_daddr_t *sd;

			assert(sp->retry);

			cb = SCTP_TCB(sp->retry);
			sd = cb->daddr;

			assert(sd);

			seldom();
			/*
			 *  We can try again with cookie preservative,
			 *  and then we can keep trying until we have
			 *  tried as many times as we can...
			 */
			if (!sp->ck_inc) {
				int err;
				rare();
				sp->ck_inc = sp->ck_inc + (sd->rto >> 1);
				sctp_ack_calc(sp, &sp->timer_init);
				if ((err = sctp_send_init(sp))) {
					rare();
					return (err);
				}
				return sctp_return_stop(mp);
			}
			/*
			   RFC 2960 5.2.6 (1) 
			 */
			if (cb->trans < sp->max_inits) {
				untimeout(xchg(&sp->timer_init, 0));
				/*
				   RFC 2960 5.2.6 (3) 
				 */
				if (cb->trans < 2)
					sctp_rtt_calc(sd, cb->when);
				usual(sp->retry);
				sctp_send_msg(sp, sd, sp->retry);
				return sctp_return_stop(mp);
			}
			/*
			   RFC 2960 5.2.6 (2) 
			 */
			goto recv_error_error;
		}
		break;

	case SCTP_CAUSE_INVALID_PARM:
	case SCTP_CAUSE_BAD_ADDRESS:
		/*
		 *  If the sender of the ERROR has already given us a valid
		 *  INIT-ACK then we can ignore these errors.
		 */
		if (sp->s_state == SCTP_COOKIE_ECHOED) {
			break;
		}
		seldom();

	case SCTP_CAUSE_MISSING_PARM:
	case SCTP_CAUSE_NO_RESOURCE:
	case SCTP_CAUSE_INVALID_STR:
		/*
		 *  These errors are bad.  If we don't get an abort with them
		 *  then we must abort the association.
		 */
	      recv_error_error:
		if (!((1 << sp->s_state) & (SCTPF_HAVEUSER))) {
			ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
			if ((err = sp->ops->sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, ecode, NULL)))
				return (err);
		} else
			rare();
		if ((1 << sp->s_state) & (SCTPF_NEEDABORT)) {
			sctp_send_abort(sp);
		} else
			rare();
		sctp_disconnect(sp);
		return sctp_return_stop(mp);

	default:
	case SCTP_CAUSE_BAD_CHUNK_TYPE:
	case SCTP_CAUSE_BAD_PARM:
	case SCTP_CAUSE_NO_DATA:
	case SCTP_CAUSE_SHUTDOWN:
		rare();
		break;

	}
	return sctp_return_more(mp);	/* ignore */
}

/*
 *  RECV ABORT
 *  -------------------------------------------------------------------------
 *  We have received an ABORT chunk in opening, connected or closing states.
 *  If there is a user around we want to send a disconnect indication,
 *  otherwise we want to just go away.
 */
STATIC int
sctp_recv_abort(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct sctp_abort *m;
	ulong orig;

	assert(sp);
	assert(mp);

	m = (struct sctp_abort *) mp->b_rptr;
	orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;

	if (sp->s_state == SCTP_LISTEN) {
		struct iphdr *iph;
		struct sctphdr *sh;

		mblk_t *cp;

		struct sctp_cookie *ck;
		struct sctp_cookie_echo *ce;

		seldom();

		iph = (struct iphdr *) mp->b_datap->db_base;
		sh = (struct sctphdr *) (mp->b_datap->db_base + (iph->ihl << 2));

		for (cp = bufq_head(&sp->conq); cp; cp = cp->b_next) {
			ce = (struct sctp_cookie_echo *) cp->b_rptr;
			ck = (struct sctp_cookie *) ce->cookie;
			if (ck->v_tag == sh->v_tag && ck->sport == sh->dest && ck->dport == sh->srce
			    && ck->saddr == iph->daddr && ck->daddr == iph->saddr) {
				ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
				if ((err = sp->ops->sctp_discon_ind(sp, orig, 0, cp))) {
					rare();
					return (err);
				}
				return sctp_return_stop(mp);
			}
		}
		usual(cp);
		return (-EPROTO);	/* discard it */
	}
	if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
		ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
		if ((err = sp->ops->sctp_discon_ind(sp, orig, 0, NULL)))	/* XXX reason */
			return (err);
	} else
		seldom();
	sctp_disconnect(sp);
	return sctp_return_stop(mp);
}

/*
 *  RECV INIT (Listener only)
 *  -------------------------------------------------------------------------
 *  We have receive an INIT in the LISTEN s_state.  This is the normal path for
 *  the establishment of an SCTP association.  There can be no other stream
 *  bound to this local port but we can have accepted streams which share the
 *  same local binding.
 *
 *  INIT chunks cannot have other chunks bundled with them (RFC 2960 6.10).
 */
STATIC int
sctp_recv_init(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct iphdr *iph;
	struct sctphdr *sh;
	struct sctp_init *m;

	sctp_t *oldsp;
	union sctp_parm *ph;
	unsigned char *pptr, *pend;
	size_t plen;

	size_t anum = 0;

	ulong ck_inc = 0;

	int err = 0;
	size_t errl = 0;
	unsigned char *errp = NULL;

	struct sctp_cookie ck;

	assert(sp);
	assert(mp);

	iph = (struct iphdr *) mp->b_datap->db_base;
	sh = (struct sctphdr *) (mp->b_datap->db_base + (iph->ihl << 2));
	m = (struct sctp_init *) mp->b_rptr;
	pptr = (unsigned char *) (m + 1);
	pend = pptr + ntohs(m->ch.len) - sizeof(*m);

	if (mp->b_wptr - mp->b_rptr < sizeof(*m)) {
		rare();
		return (-EFAULT);
	}
	/*
	   RFC 2960 p.26 initiate tag zero 
	 */
	if (!m->i_tag) {
		err = -SCTP_CAUSE_INVALID_PARM;
		seldom();
		goto init_error;
	}

	for (ph = (union sctp_parm *) pptr;
	     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
	     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
		uint type;
		switch ((type = ph->ph.type)) {
		case SCTP_PTYPE_IPV6_ADDR:
		case SCTP_PTYPE_HOST_NAME:
			errp = pptr;
			errl = plen;
			err = -SCTP_CAUSE_BAD_ADDRESS;
			seldom();
			goto init_error;
		case SCTP_PTYPE_COOKIE_PSRV:
			if (plen == sizeof(ph->cookie_prsv)) {
				ck_inc = ntohl(ph->cookie_prsv.ck_inc);
				break;
			}
			goto init_bad_parm;
		case SCTP_PTYPE_IPV4_ADDR:
			if (plen == sizeof(ph->ipv4_addr)) {
				/*
				   skip primary 
				 */
				if (ph->ipv4_addr.addr != iph->saddr)
					anum++;
				break;
			}
			goto init_bad_parm;
		case SCTP_PTYPE_ADDR_TYPE:
			/*
			 *  Ensure that address types supported includes IPv4.
			 *  Actually address types must include IPv4 so we
			 *  just ignore.
			 */
			break;
		      init_bad_parm:
		default:
			rare();
			if (type & SCTP_PTYPE_MASK_REPORT) {
				err = -SCTP_CAUSE_BAD_PARM;
				errp = pptr;
				errl = plen;
			}
			if (!(type & SCTP_PTYPE_MASK_CONTINUE))
				rare();
			goto init_error;
		}
	}
	/*
	   put together cookie 
	 */
	{
		/*
		   negotiate inbound and outbound streams 
		 */

		size_t istrs = sp->n_istr;
		size_t ostrs = sp->n_ostr;

		istrs = ((istrs && istrs <= ntohs(m->n_istr))) ? istrs : ntohs(m->n_istr);
		ostrs = ((ostrs && ostrs <= ntohs(m->n_ostr))) ? ostrs : ntohs(m->n_ostr);

		errl = 0;
		errp = NULL;
		err = -SCTP_CAUSE_INVALID_PARM;

		if (m->n_istr && !istrs) {
			seldom();
			goto init_error;
		}
		if (m->n_ostr && !ostrs) {
			seldom();
			goto init_error;
		}

		/*
		   RFC 2969 5.2.6 
		 */
		if (ck_inc) {
			seldom();
			ck_inc = (ck_inc * HZ + 999) / 1000;
			usual(ck_inc);
			if (ck_inc > sp->rto_ini + sp->ck_inc) {
				rare();
				ck_inc = sp->rto_ini + sp->ck_inc;
			}
		}

		ck.timestamp = jiffies;
		ck.lifespan = sp->ck_life + ck_inc;
		ck.daddr = iph->saddr;
		ck.saddr = iph->daddr;
		ck.dport = sh->srce;
		ck.sport = sh->dest;
		ck.v_tag = sctp_get_vtag(ck.daddr, ck.saddr, ck.dport, ck.sport);
		ck.p_tag = m->i_tag;
		ck.p_tsn = ntohl(m->i_tsn);
		ck.p_rwnd = ntohl(m->a_rwnd);
		ck.n_istr = istrs;
		ck.n_ostr = ostrs;
		ck.danum = anum;
	}

	/*
	   RFC 2960 5.2.2 Note 
	 */
	SCTPHASH_RLOCK();
	oldsp = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr);
	SCTPHASH_RUNLOCK();
	if (oldsp && oldsp->s_state != SCTP_COOKIE_WAIT) {
		rare();
		ck.l_ttag = oldsp->v_tag;
		ck.p_ttag = oldsp->p_tag;
	} else {
		ck.l_ttag = 0;
		ck.p_ttag = 0;
	}
	ck.sanum = sp->sanum - 1;	/* don't include primary */
	ck.opt_len = 0;
	sctp_send_init_ack(sp, iph->saddr, sh, &ck);
	return (0);

      init_error:
	seldom();
	if (err) {
		rare();
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, -err, errp, errl);
	}
	return (0);
}

/*
 *  RECV INIT ACK
 *  -------------------------------------------------------------------------
 *  We have recevied an INIT ACK in the SCTP_COOKIE_WAIT (TS_WCON_CREQ) s_state.
 *  (RFC 2960 5.2.3)
 */
STATIC int
sctp_recv_init_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	int reason;

	assert(sp);
	assert(mp);

	if (sp->s_state == SCTP_COOKIE_WAIT) {
		struct iphdr *iph = (struct iphdr *) mp->b_datap->db_base;
		struct sctp_init_ack *m = (struct sctp_init_ack *) mp->b_rptr;

		if (sctp_daddr_include(sp, iph->saddr, &err)) {
			unsigned char *kptr = NULL;
			size_t klen = 0;

			unsigned char *pptr = (unsigned char *) (m + 1);
			unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
			size_t plen;

			struct sctpphdr *ph = (struct sctpphdr *) pptr;

			ensure((pend <= mp->b_wptr), return (-EMSGSIZE));
			{
				size_t ostr = htons(m->n_ostr);
				size_t istr = htons(m->n_istr);

				if (!ostr || !istr || istr > sp->max_istr) {
					err = -EPROTO;
					goto sctp_recv_init_ack_error;
				}
				sp->p_tag = m->i_tag;
				sp->p_rwnd = ntohl(m->a_rwnd);
				sp->n_ostr = ostr;
				sp->n_istr = istr;
				sp->r_ack = ntohl(m->i_tsn) - 1;
			}
			for (ph = (struct sctpphdr *) pptr;
			     pptr + sizeof(*ph) <= pend && pptr + (plen = ntohs(ph->len)) <= pend;
			     pptr += PADC(plen), ph = (struct sctpphdr *) pptr) {
				uint type;
				switch ((type = ph->type)) {
				case SCTP_PTYPE_IPV4_ADDR:
					if (!sctp_daddr_include(sp, *((uint32_t *) (ph + 1)), &err)) {
						rare();
						sctp_send_error(sp, SCTP_CAUSE_BAD_ADDRESS, ph,
								plen);
					}
					continue;

				case SCTP_PTYPE_STATE_COOKIE:
					kptr = pptr + sizeof(*ph);
					klen = plen - sizeof(*ph);
					continue;

				case SCTP_PTYPE_IPV6_ADDR:
				case SCTP_PTYPE_HOST_NAME:
					rare();
					sctp_send_error(sp, SCTP_CAUSE_BAD_ADDRESS, ph, plen);
					continue;

				case SCTP_PTYPE_UNREC_PARMS:
				default:
					rare();
					reason = SCTP_CAUSE_BAD_PARM;
					if (type & SCTP_PTYPE_MASK_REPORT)
						sctp_send_error(sp, reason, ph, plen);
					if (type & SCTP_PTYPE_MASK_CONTINUE)
						continue;
					break;
				}
			}
			if (!kptr) {	/* no cookie? */
				struct {
					uint32_t num;
					uint16_t mparm;
				} es = {
				1, SCTP_PTYPE_STATE_COOKIE};
				rare();
				reason = SCTP_CAUSE_MISSING_PARM;
				sctp_send_error(sp, reason, &es, sizeof(es));
				return (-EPROTO);
			}

			sctp_ack_calc(sp, &sp->timer_init);
			sctp_phash_rehash(sp);
			if ((err = sctp_send_cookie_echo(sp, kptr, klen))) {
				rare();
				return (err);
			}
			return sctp_return_stop(mp);

		}
		rare();
		return (err);	/* fall back on timer init */
	}
	rare();
	return (-EPROTO);

      sctp_recv_init_ack_error:
	rare();
	if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
		ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
		if ((err = sp->ops->sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, err, NULL))) {
			rare();
			return (err);
		}
	} else
		rare();
	sctp_disconnect(sp);	/* we won't be back */
	return (err);
}

/*
 *  RECV COOKIE ECHO
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ECHO for a STREAM.  We have already determined
 *  the STREAM to which the COOKIE ECHO applies.  We must still verify the
 *  cookie and apply the appropriate action per RFC 2960 5.2.4.
 */
STATIC int
sctp_recv_cookie_echo(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct iphdr *iph;
	struct sctphdr *sh;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;

	assert(sp);
	assert(mp);

	iph = (struct iphdr *) mp->b_datap->db_base;
	sh = (struct sctphdr *) (mp->b_datap->db_base + (iph->ihl << 2));
	m = (struct sctp_cookie_echo *) mp->b_rptr;
	ck = (struct sctp_cookie *) m->cookie;

	/*
	   RFC 2960 5.2.4 (1) & (2) 
	 */
	if ((err = sctp_verify_cookie(sp, ck))) {
		rare();
		return (err);
	}
	if (sp->v_tag) {
		if (ck->v_tag != sp->v_tag) {
			if (ck->p_tag != sp->p_tag) {
				if (ck->l_ttag == sp->v_tag && ck->p_ttag == sp->p_tag)
					/*
					   RFC 2960 5.2.4. Action (A) 
					 */
					goto recv_cookie_echo_action_a;
			} else if (ck->l_ttag == 0 && ck->p_ttag == 0)
				/*
				   RFC 2960 5.2.4. Action (C). 
				 */
				goto recv_cookie_echo_action_c;
		} else {
			if (!sp->p_tag
			    || ((1 << sp->s_state) & (SCTPF_COOKIE_WAIT | SCTPF_COOKIE_ECHOED)))
				/*
				   RFC 2960 5.2.4 Action (B). 
				 */
				goto recv_cookie_echo_action_b;
			else if (ck->p_tag != sp->p_tag)
				/*
				   RFC 2960 5.2.4 Action (B) 
				 */
				goto recv_cookie_echo_action_b;
			else
				/*
				   RFC 2960 5.2.4 Action (D). 
				 */
				goto recv_cookie_echo_action_d;
		}
	} else
		/*
		   RFC 2960 5.2.4 Action (D). 
		 */
		goto recv_cookie_echo_action_d;

	rare();
	/*
	   RFC 2960 5.2.4 ...silently discarded 
	 */
	return (0);

      recv_cookie_echo_action_a:
	rare();
	/*
	 *  RFC 2960 5.2.4 Action (A)
	 *
	 *  In this case, the peer may have restarted.  When the endpoint
	 *  recognizes this potential 'restart', the existing session is
	 *  treated the same as if it received an ABORT followed by a new
	 *  COOKIED ECHO with the following exceptions:  - Any SCTP DATA
	 *  Chunks MAY be retained (this is an implementation specific
	 *  option).  - A notification of RESTART SHOULD be sent to the ULP
	 *  instead of "COMMUNICATION LOST" notification.
	 *
	 *  All the Congestion control parameters (e.g., cwnd, ssthresh)
	 *  related to this peer MUST be reset to their initial values (see
	 *  Section 6.2.1).  After this the endpoint shall enter the
	 *  ESTABLISHED s_state.
	 */
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		rare();
		/*
		   RFC 2960 5.2.4 (A) 
		 */
		sctp_send_abort_error(sp, SCTP_CAUSE_SHUTDOWN, NULL, 0);
		sctp_send_shutdown_ack(sp);
		return sctp_return_stop(mp);

	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
		rare();
		/*
		 *  We trash all existing data in queue.
		 */
		sctp_disconnect(sp);
		/*
		 *  Notify user of reset or disconnect
		 */
		if (sp->ops->sctp_reset_ind) {
			fixme(("Need reason argument to reset ind\n"));
			if ((err = sp->ops->sctp_reset_ind(sp, SCTP_ORIG_USER, 0, mp))) {	/* XXX 
												   reason 
												 */
				rare();
				return (err);
			}
			return (0);
		}
		if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
			fixme(("Need reason argument to discon ind\n"));
			if ((err = sp->ops->sctp_discon_ind(sp, SCTP_ORIG_USER, 0, NULL))) {	/* XXX 
												   reason 
												 */
				rare();
				return (err);
			}
		} else
			rare();
		goto recv_cookie_echo_conn_ind;
	default:
		rare();
		return (-EFAULT);
	}
	never();

      recv_cookie_echo_action_b:
	rare();
	/*
	 *  RFC 2960 5.2.4 Action (B)
	 *
	 *  In this case, both sides may be attempting to start and
	 *  association at about the same time but the peer endpoint started
	 *  its INIT after responding to the local endpoint's INIT.  Thus it
	 *  may have picked a new Verification Tag not being aware of the
	 *  previous Tag it had sent this endpoint.  The endpoint should stay
	 *  in or enter the ESTABLISHED s_state but it MUST update its peer's
	 *  Verification Tag from the State Cookie, stop any init or cookie
	 *  timers that may be running and send a COOKIE ACK.
	 */
	switch (sp->s_state) {
	case SCTP_COOKIE_WAIT:

		if (sp->timer_init) {
			rare();
			untimeout(xchg(&sp->timer_init, 0));
		}
		/*
		 *  We haven't got an INIT ACK yet so we need some
		 *  stuff from the cookie
		 */
		sp->p_rwnd = ck->p_rwnd;
		sp->r_ack = ck->p_tsn - 1;
		sp->n_istr = ck->n_istr;
		sp->n_ostr = ck->n_ostr;

		ensure(sp->ops->sctp_conn_con, return (-EFAULT));
		if ((err = sp->ops->sctp_conn_con(sp))) {
			rare();
			return (err);
		}
		break;

	case SCTP_COOKIE_ECHOED:
		if (sp->timer_cookie) {
			rare();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		break;

	case SCTP_ESTABLISHED:
		break;

	default:
		rare();
		return (-EFAULT);
	}
	sp->p_tag = ck->p_tag;
	sctp_phash_rehash(sp);
	sctp_send_sack(sp);
	sctp_send_cookie_ack(sp);
	return sctp_return_more(mp);

      recv_cookie_echo_action_c:
	rare();
	/*
	 *  RFC 2960 5.2.4 Action (C)
	 *
	 *  In this case, the local endpoint's cookie has arrived late.
	 *  Before it arrived, the local sendpoint sent an INIT and receive an
	 *  INIT-ACK and finally sent a COOKIE ECHO with the peer's same tag
	 *  but a new tag of its own.  The cookie should be silently
	 *  discarded.  The endpoint SHOULD NOT change states and should leave
	 *  any timers running.
	 */
	/*
	   RFC 2960 5.2.4 (3) 
	 */
	if (jiffies - ck->timestamp > ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	return (0);

      recv_cookie_echo_action_d:
	/*
	 *  RFC 2960 5.2.4 Action (D)
	 *
	 *  When both local and remote tags match the endpoint should always
	 *  enter the ESTABLISHED s_state, it if has not already done so.  It
	 *  should stop any init or cookie timers that may be running and send
	 *  a COOKIE ACK.
	 */
	/*
	   RFC 2960 5.2.4 (3) 
	 */
	if (jiffies - ck->timestamp > ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
      recv_cookie_echo_conn_ind:
	if (sp->conind) {
		mblk_t *cp;		/* check for existing conn ind */
		for (cp = bufq_head(&sp->conq); cp; cp = cp->b_next) {
			struct sctp_cookie_echo *ce = (struct sctp_cookie_echo *) cp->b_rptr;
			struct sctp_cookie *co = (struct sctp_cookie *) ce->cookie;

			if (co->v_tag == ck->v_tag) {
				seldom();
				return (0);
			}
			if (co->dport != ck->dport || co->sport != ck->sport)
				continue;
			if (co->daddr == ck->daddr && co->saddr == ck->saddr) {
				seldom();
				return (0);
			}
		}
		/*
		   RFC 2960 5.2.4 (4) 
		 */
		if (bufq_length(&sp->conq) >= sp->conind) {
			seldom();
			sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
						   SCTP_CAUSE_NO_RESOURCE, NULL, 0);
			return (0);
		}
		ensure(sp->ops->sctp_conn_ind, return (-EFAULT));
		if ((err = sp->ops->sctp_conn_ind(sp, mp))) {
			rare();
			return (err);
		}
		return (1);	/* leave cookie and data in the message for conn_res */
	}
	normal(sp->s_state == SCTP_ESTABLISHED);
	sctp_send_cookie_ack(sp);
	return (0);
}

/*
 *  RECV COOKIE ACK
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ACK.  If we are in COOKIE ECHOED s_state then we
 *  inform the user interface that the previous connection request is
 *  confirmed, cancel the cookie timer while performing an RTO calculation on
 *  the message and enter the ESTABLISHED s_state.  Any DATA that is bundled
 *  with the COOKIE ACK will be separately indicated to the user with data
 *  indications.  In s_state other than COOKIE ECHOED the entire message is
 *  silently discarded.
 */
STATIC int
sctp_recv_cookie_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	sctp_daddr_t *sd;

	assert(sp);
	assert(mp);

	switch (sp->s_state) {
	case SCTP_COOKIE_ECHOED:
		/*
		   RFC 2960 5.1 (E) 
		 */
		ensure(sp->ops->sctp_conn_con, return (-EFAULT));
		if ((err = sp->ops->sctp_conn_con(sp))) {
			rare();
			return (err);
		}
		sctp_ack_calc(sp, &sp->timer_cookie);
		sp->s_state = SCTP_ESTABLISHED;
		/*
		   start idle timers 
		 */
		usual(sp->daddr);
		for (sd = sp->daddr; sd; sd = sd->next)
			sctp_reset_idle(sd);
		return sctp_return_more(mp);
	default:
		/*
		   RFC 2960 5.2.5 
		 */
		rare();
		break;
	}
	return (0);		/* silently discard */

}

/*
 *  RECV HEARTBEAT
 *  -------------------------------------------------------------------------
 *  We have received a HEARTBEAT.  Quite frankly we don't care what s_state we
 *  are in, we take the heartbeat info and turn it back around as a HEARTBEAT
 *  ACK msg.  We do a little bit of error checking here to make sure that we
 *  are not wasting our time on the packet.  We should only receive HEARTBEAT
 *  messages if we are in the vtag hashes: meaning that we will accept and
 *  reply to a HEARTBEAT in any s_state other than CLOSED and LISTEN.
 *
 */
STATIC int
sctp_recv_heartbeat(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_heartbeat *m;
	struct sctpphdr *ph;
	size_t plen;

	assert(sp);
	assert(mp);

	m = (struct sctp_heartbeat *) mp->b_rptr;
	ph = (struct sctpphdr *) (m + 1);
	plen = PADC(htons(m->ch.len)) - sizeof(*m);

	if (plen >= sizeof(struct sctpphdr)) {
		if (ph->type == SCTP_PTYPE_HEARTBEAT_INFO) {
			{
				caddr_t hptr = (caddr_t) ph;
				size_t hlen = min(plen, PADC(ntohs(ph->len)));
				sctp_send_heartbeat_ack(sp, hptr, hlen);
				return sctp_return_stop(mp);
			}
		}
		rare();
		return (-EPROTO);	/* bad message */
	}
	rare();
	return (-EMSGSIZE);	/* bad message */
}

/*
 *  RECV HEARTBEAT ACK
 *  -------------------------------------------------------------------------
 *  This is our HEARTBEAT coming back.  We check that the HEARTBEAT information matches
 *  the information of the last sent HEARTBEAT message to ensure that no fiddling with the
 *  HEARTBEAT info has occured.  The only information we trust initially is the
 *  destination address which is contained in the HEARTBEAT INFO.  This just helps us
 *  index the remainder of the hearbeat information.
 *
 *  We have received a HEARTBEAT ACK message on an ESTABLISHED STREAM.  Perform the
 *  procedures from RFC 2960 8.3.
 */
STATIC int
sctp_recv_heartbeat_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	sctp_daddr_t *sd;
	struct sctp_heartbeat_ack *m;
	struct sctp_heartbeat_info *hb;

	assert(sp);
	assert(mp);

	m = (struct sctp_heartbeat_ack *) mp->b_rptr;
	hb = (struct sctp_heartbeat_info *) (m + 1);

	if ((sd = sctp_find_daddr(sp, hb->hb_info.daddr))) {
		if (ntohs(hb->ph.len) == sizeof(*hb) + sd->hb_fill) {
			if (hb->ph.type == SCTP_PTYPE_HEARTBEAT_INFO) {
				if (sd->hb_time && sd->hb_time == hb->hb_info.timestamp) {
					{
						if (sd->timer_heartbeat)
							untimeout(xchg(&sd->timer_heartbeat, 0));
						sctp_rtt_calc(sd, sd->hb_time);
					}
				} else
					rare();	/* old or fiddled timestamp */
			} else
				rare();	/* bad hb parm type */
		} else
			rare();	/* bad hb parm size */
	} else
		rare();		/* bad dest parameter */
	return sctp_return_stop(mp);
}

/*
 *  RECV SHUTDOWN
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_shutdown(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct sctp_shutdown *m;
	uint32_t ack;

	assert(sp);
	assert(mp);

	m = (struct sctp_shutdown *) mp->b_rptr;
	ack = ntohl(m->c_tsn);

	if (sp->ngaps) {
		rare();
		/*
		 *  Check sanity of sender: if we have gaps in our acks to the
		 *  peer and the peer sends a SHUTDOWN, then it is in error.
		 *  The peer cannot send SHUTDOWN when it has unacknowledged
		 *  data.  If this is the case, we zap the connection.
		 */
		if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
			ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
			if ((err = sp->ops->sctp_discon_ind(sp, SCTP_ORIG_USER, -EPIPE, NULL)))
				return (err);
		} else
			rare();
		if ((1 << sp->s_state) & (SCTPF_NEEDABORT)) {
			sctp_send_abort(sp);
		} else
			rare();
		sctp_disconnect(sp);
		return sctp_return_stop(mp);
	}
	if (before(ack, sp->t_ack)) {
		rare();
		/*
		 *  If the SHUTDOWN acknowledges our sent data chunks that
		 *  have already been acknowledged, then it is an old (or
		 *  erroneous) message and we will ignore it.
		 */
		return sctp_return_stop(mp);
	}

	switch (sp->s_state) {
	case SCTP_ESTABLISHED:
		if (sp->ops->sctp_ordrel_ind) {
			if ((err = sp->ops->sctp_ordrel_ind(sp)))
				return (err);
		} else {
			if ((err = sp->ops->sctp_discon_ind(sp, SCTP_ORIG_USER, 0, NULL)))
				return (err);
		}
		sp->s_state = SCTP_SHUTDOWN_RECEIVED;
		/*
		   fall thru 
		 */
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_cumm_ack(sp, ack);
		sctp_dest_calc(sp);
		if (!sp->ops->sctp_ordrel_ind)
			if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
				sctp_send_shutdown_ack(sp);
		break;

	case SCTP_SHUTDOWN_PENDING:	/* only when we have ordrel */
		if ((err = sp->ops->sctp_ordrel_ind(sp)))
			return (err);
		sp->s_state = SCTP_SHUTDOWN_RECVWAIT;
		/*
		   fall thru 
		 */
	case SCTP_SHUTDOWN_RECVWAIT:
		sctp_cumm_ack(sp, ack);
		sctp_dest_calc(sp);
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown_ack(sp);
		break;

	case SCTP_SHUTDOWN_SENT:	/* only when we have ordrel */
		if ((err = sp->ops->sctp_ordrel_ind(sp)))
			return (err);
		sctp_cumm_ack(sp, ack);
		sctp_dest_calc(sp);
		/*
		   faill thru 
		 */
	case SCTP_SHUTDOWN_ACK_SENT:
		sctp_send_shutdown_ack(sp);	/* We do this */
		break;

	default:
		/*
		   ignore the SHUTDOWN chunk 
		 */
		rare();
		break;
	}
	return sctp_return_more(mp);
}

/*
 *  RECV SHUTDOWN ACK
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN ACK chunk through the vtag hashes.  We are expecting the
 *  SHUTDOWN ACK because we have previously sent a SHUTDOWN or SHUTDOWN ACK or we ignore the
 *  message.
 */
STATIC int
sctp_recv_shutdown_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_SENT:
		/*
		   send up orderly release indication to ULP 
		 */
		if (sp->ops->sctp_ordrel_ind)
			if ((err = sp->ops->sctp_ordrel_ind(sp)))
				return (err);
		/*
		   fall thru 
		 */
	case SCTP_SHUTDOWN_ACK_SENT:
		// sctp_ack_calc(sp, &sp->timer_shutdown); /* WHY? */
		sctp_send_shutdown_complete(sp);
		sctp_disconnect(sp);
		break;
	default:
		/*
		   ignore unexpected SHUTDOWN ACK 
		 */
		rare();
		break;
	}
	return sctp_return_stop(mp);
}

/*
 *  RECV SHUTDOWN COMPLETE
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN COMPLETE in the SHUTDOWN ACK SENT s_state.  This is the
 *  normal path for shutting down an SCTP association.  Outstanding data has already been
 *  processed.  Remove ourselves from the hashes and process any backlog.  RFC 2960 6.10:
 *  SHUTDOWN COMPLETE cannot have any other chunks bundled with them.
 *
 *  We receive SHUTDOWN COMPLETE chunks through the vtag hashes.  We are expecting
 *  SHUTDOWN COMPLETE because we sent a SHUTDOWN ACK.  We are in the TS_IDLE, NS_IDLE
 *  states but are still in the vtag hashes.
 */
STATIC int
sctp_recv_shutdown_complete(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	assert(sp);
	assert(mp);
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		// sctp_ack_calc(sp, &sp->timer_shutdown); /* WHY? */
		sctp_disconnect(sp);
		break;
	default:
		/*
		   ignore unexpected SHUTDOWN COMPLETE 
		 */
		rare();
		break;
	}
	return sctp_return_stop(mp);
}

/*
 *  RECV UNRECOGNIZED CHUNK TYPE
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_unrec_ctype(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int ctype;

	rare();
	assert(sp);
	assert(mp);
	ctype = ((struct sctpchdr *) mp->b_rptr)->type;
	if (ctype & SCTP_CTYPE_MASK_REPORT)
		sctp_send_abort_error(sp, SCTP_CAUSE_BAD_CHUNK_TYPE, mp->b_rptr,
				      mp->b_wptr - mp->b_rptr);

	if (ctype & SCTP_CTYPE_MASK_CONTINUE)
		return sctp_return_more(mp);

	return -EPROTO;
}

/*
 *  RECV SCTP MESSAGE
 *  -------------------------------------------------------------------------
 */
int
sctp_recv_msg(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err = -EMSGSIZE;

	if (mp) {
		/*
		   set the address for reply chunks 
		 */
		if (sp->daddr) {
			sp->caddr =
			    sctp_find_daddr(sp, ((struct iphdr *) mp->b_datap->db_base)->saddr);
			normal(sp->caddr);
		}
		do {
			int dlen = mp->b_wptr - mp->b_rptr;
			struct sctpchdr *ch = (struct sctpchdr *) mp->b_rptr;

			unusual(dlen < 0);
			if (dlen > 0 && dlen >= sizeof(*ch) && dlen >= PADC(ntohs(ch->len))) {
				switch ((ch->type & SCTP_CTYPE_MASK)) {
				case SCTP_CTYPE_DATA:
					err = sctp_recv_data(sp, mp);
					break;
				case SCTP_CTYPE_INIT:
					err = sctp_recv_init(sp, mp);
					break;
				case SCTP_CTYPE_INIT_ACK:
					err = sctp_recv_init_ack(sp, mp);
					break;
				case SCTP_CTYPE_SACK:
					err = sctp_recv_sack(sp, mp);
					break;
				case SCTP_CTYPE_HEARTBEAT:
					err = sctp_recv_heartbeat(sp, mp);
					break;
				case SCTP_CTYPE_HEARTBEAT_ACK:
					err = sctp_recv_heartbeat_ack(sp, mp);
					break;
				case SCTP_CTYPE_ABORT:
					err = sctp_recv_abort(sp, mp);
					break;
				case SCTP_CTYPE_SHUTDOWN:
					err = sctp_recv_shutdown(sp, mp);
					break;
				case SCTP_CTYPE_SHUTDOWN_ACK:
					err = sctp_recv_shutdown_ack(sp, mp);
					break;
				case SCTP_CTYPE_ERROR:
					err = sctp_recv_error(sp, mp);
					break;
				case SCTP_CTYPE_COOKIE_ECHO:
					err = sctp_recv_cookie_echo(sp, mp);
					break;
				case SCTP_CTYPE_COOKIE_ACK:
					err = sctp_recv_cookie_ack(sp, mp);
					break;
				case SCTP_CTYPE_ECNE:
					err = sctp_recv_unrec_ctype(sp, mp);
					break;
				case SCTP_CTYPE_CWR:
					err = sctp_recv_unrec_ctype(sp, mp);
					break;
				case SCTP_CTYPE_SHUTDOWN_COMPLETE:
					err = sctp_recv_shutdown_complete(sp, mp);
					break;
				default:
					err = sctp_recv_unrec_ctype(sp, mp);
					break;
				}
				normal(err >= 0);
			} else {
				rare();
				err = -EMSGSIZE;
			}
		}
		while (err > 0 && err >= sizeof(struct sctpchdr));
	} else
		rare();
	if (err < 0) {
		rare();
		ptrace(("sp = %x, Error %d returned.\n", (uint) sp, err));
	}
	return (err);
}

/*
 *  RECV SCTP ICMP ERROR
 *  -------------------------------------------------------------------------
 */
#ifndef ip_rt_update_pmtu
#ifdef HAVE_IP_RT_UPDATE_PMTU_ADDR
void
ip_rt_update_pmtu(struct dst_entry *dst, u32 mtu)
{
	void (*func) (struct dst_entry *, u32) = (typeof(func)) HAVE_IP_RT_UPDATE_PMTU_ADDR;
	return func(dst, mtu);
}
#endif
#endif

int
sctp_recv_err(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_daddr *sd;
	uint32_t daddr = *((uint32_t *) mp->b_rptr)++;
	struct icmphdr *icmph = (struct icmphdr *) mp->b_rptr;

	if ((sd = sctp_find_daddr(sp, daddr))) {
		int type = icmph->type;
		int code = icmph->code;

		switch (type) {
		case ICMP_SOURCE_QUENCH:
			rare();
			sd->ssthresh = sd->cwnd >> 1 > sd->mtu << 1 ? sd->cwnd >> 1 : sd->mtu << 1;
			sd->cwnd = sd->mtu;
			sd->rto = sd->rto << 1 ? sd->rto << 1 : 1;
			sd->rto = sd->rto_min > sd->rto ? sd->rto_min : sd->rto;
			sd->rto = sd->rto_max < sd->rto ? sd->rto_max : sd->rto;
			break;

		case ICMP_DEST_UNREACH:
		{
			if (code > NR_ICMP_UNREACH) {
				rare();
				return (0);
			}
			if (sd->dst_cache) {
				if (code == ICMP_FRAG_NEEDED) {
					size_t mtu = ntohs(icmph->un.frag.mtu);
					rare();
					ip_rt_update_pmtu(sd->dst_cache, mtu);
				} else
					dst_release(xchg(&sd->dst_cache, NULL));
			} else
				rare();
			break;
		}
		default:
			rare();
			return (0);
		}
	} else
		rare();
	return (0);
}
