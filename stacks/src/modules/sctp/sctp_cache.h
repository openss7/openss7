/*****************************************************************************

 @(#) $Id: sctp_cache.h,v 0.9 2004/01/17 08:21:59 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2004/01/17 08:21:59 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTP_CACHE_H__
#define __SCTP_CACHE_H__

#ident "@(#) $RCSfile: sctp_cache.h,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:21:59 $"

/*
 *  Cache pointers
 *  -------------------------------------------------------------------------
 */
extern kmem_cache_t *sctp_sctp_cachep;
extern kmem_cache_t *sctp_dest_cachep;
extern kmem_cache_t *sctp_srce_cachep;
extern kmem_cache_t *sctp_strm_cachep;

extern void sctp_init_caches(void);
extern void sctp_term_caches(void);

/*
 *  Find a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline sctp_daddr_t *
sctp_find_daddr(sp, daddr)
	sctp_t *sp;
	uint32_t daddr;
{
	sctp_daddr_t *sd;
	for (sd = sp->daddr; sd && sd->daddr != daddr; sd = sd->next) ;
	return (sd);
}
extern sctp_daddr_t *sctp_daddr_include(sctp_t * sp, uint32_t daddr, int *errp);
extern int sctp_alloc_daddrs(sctp_t * sp, uint16_t dport, uint32_t * daddrs, size_t dnum);

/*
 *  Find a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline sctp_saddr_t *
sctp_find_saddr(sp, saddr)
	sctp_t *sp;
	uint32_t saddr;
{
	sctp_saddr_t *ss;
	for (ss = sp->saddr; ss && ss->saddr != saddr; ss = ss->next) ;
	return (ss);
}
extern sctp_saddr_t *sctp_saddr_include(sctp_t * sp, uint32_t saddr, int *errp);
extern int sctp_alloc_saddrs(sctp_t * sp, uint16_t sport, uint32_t * saddrs, size_t snum);

extern sctp_strm_t *sctp_strm_alloc(sctp_strm_t ** stp, uint16_t sid, int *erp);
/*
 *  Find or Add an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline sctp_strm_t *
sctp_istrm_find(sp, sid, errp)
	sctp_t *sp;
	uint16_t sid;
	int *errp;
{
	sctp_strm_t *st;
	if (!(st = sp->istr) || st->sid != sid)
		for (st = sp->istrm; st && st->sid != sid; st = st->next) ;
	if (st)
		sp->istr = st;	/* cache */
	else if ((st = sctp_strm_alloc(&sp->istrm, sid, errp)))
		st->ssn = -1;
	return (st);
}
static inline sctp_strm_t *
sctp_ostrm_find(sp, sid, errp)
	sctp_t *sp;
	uint16_t sid;
	int *errp;
{
	sctp_strm_t *st;
	if (!(st = sp->ostr) || st->sid != sid)
		for (st = sp->ostrm; st && st->sid != sid; st = st->next) ;
	if (st)
		sp->ostr = st;	/* cache */
	else
		st = sctp_strm_alloc(&sp->ostrm, sid, errp);
	return (st);
}

#define sctp_init_lock(__sp) lis_spin_lock_init(&((__sp)->lock),"sctp-private")
#define sctp_locked(__sp) ((__sp)->userq)

static inline int
sctp_trylock(queue_t *q)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	psw_t flags;
	if (lis_spin_is_locked(&sp->lock))
		return (!0);
	lis_spin_lock_irqsave(&sp->lock, &flags);
	if (sp->userq && sp->userq != q) {
		lis_spin_unlock_irqrestore(&sp->lock, &flags);
		return (!0);
	}
	sp->userq = q;
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
	return (0);
}
static inline int
sctp_waitlock(queue_t *q)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	psw_t flags;
	lis_spin_lock_irqsave(&sp->lock, &flags);
	if (sp->userq && sp->userq != q) {
		if (sp->waitq && sp->waitq != q)
			ptrace(("SWERR: More than two queues in pair!\n"));
		sp->waitq = q;
		lis_spin_unlock_irqrestore(&sp->lock, &flags);
		return (!0);
	}
	sp->userq = q;
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
	return (0);
}

extern void sctp_cleanup_read(sctp_t * sp);
extern void sctp_transmit_wakeup(sctp_t * sp);

static inline void
sctp_unlock(queue_t *q)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	psw_t flags;
	sctp_cleanup_read(sp);	/* deliver to userq what is possible */
	sctp_transmit_wakeup(sp);	/* reply to peer what is necessary */
	lis_spin_lock_irqsave(&sp->lock, &flags);
	if (sp->userq && sp->userq == q) {
		sp->userq = NULL;
		if (sp->waitq && sp->waitq != q)	/* run the other queue in the queue * pair
							   if it was blocked */
			qenable(xchg(&sp->waitq, NULL));
		else
			ptrace(("SWERR: Bogus wait\n"));
	} else
		ptrace(("SWERR: Bogus unlock\n"));
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
}

/*
 *  These two are used by timeout functions to lock normal queue functions
 *  from entering put and srv routines.
 */
#define sctp_bh_lock(__sp) lis_spin_lock(&((__sp)->lock))
#define sctp_bh_unlock(__sp) lis_spin_unlock(&((__sp)->lock))

extern sctp_t *sctp_alloc_priv(queue_t *q, sctp_t ** spp, int cmajor, int cminor,
			       struct sctp_ifops *ops);
extern void sctp_free_priv(queue_t *q);
extern void sctp_disconnect(sctp_t * sp);
extern void __sctp_disconnect(sctp_t * sp);
extern void sctp_unbind(sctp_t * sp);
extern void sctp_reset(sctp_t * sp);

#endif				/* __SCTP_CACHE_H__ */
