/*****************************************************************************

 @(#) $Id: sctp_hash.h,v 0.9.2.5 2007/06/17 01:56:20 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/06/17 01:56:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp_hash.h,v $
 Revision 0.9.2.5  2007/06/17 01:56:20  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SCTP_HASH_H__
#define __SCTP_HASH_H__

#ident "@(#) $RCSfile: sctp_hash.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include <linux/interrupt.h>	/* for local_irq functions */
#ifdef HAVE_KINC_ASM_SOFTIRQ_H
#include <asm/softirq.h>	/* for start_bh_atomic, end_bh_atomic */
#endif

/*
 *  Hashing Functions
 *  -------------------------------------------------------------------------
 */
/*
   for now: adjust it and scale it with kernel pages later 
 */
#define sctp_bhash_size (1<<6)
#define sctp_lhash_size (1<<6)
#define sctp_phash_size (1<<6)
#define sctp_cache_size (1<<6)
#define sctp_vhash_size (1<<6)
#define sctp_thash_size (1<<6)

extern sctp_t *sctp_bhash[];		/* bind */
extern sctp_t *sctp_lhash[];		/* listen */
extern sctp_t *sctp_phash[];		/* p_tag */
extern sctp_t *sctp_cache[];		/* v_tag *//* level 1 */
extern sctp_t *sctp_vhash[];		/* v_tag *//* level 2 */
extern sctp_t *sctp_thash[];		/* tcb *//* level 4 */

static inline uint
sctp_bhashfn(uint16_t port)
{
	return ((sctp_bhash_size - 1) & port);
}
static inline uint
sctp_lhashfn(uint16_t port)
{
	return ((sctp_lhash_size - 1) & port);
}
static inline uint
sctp_phashfn(uint32_t ptag)
{
	return ((sctp_phash_size - 1) & ptag);
}
static inline uint
sctp_cachefn(uint32_t vtag)
{
	return ((sctp_cache_size - 1) & vtag);
}
static inline uint
sctp_vhashfn(uint32_t vtag)
{
	return ((sctp_vhash_size - 1) & vtag);
}
static inline uint
sctp_thashfn(uint16_t sport, uint16_t dport)
{
	return ((sctp_thash_size - 1) & (sport + (dport << 4)));
}

static inline uint
sctp_sp_bhashfn(sctp_t * sp)
{
	return sctp_bhashfn(sp->sport);
}
static inline uint
sctp_sp_lhashfn(sctp_t * sp)
{
	return sctp_lhashfn(sp->sport);
}
static inline uint
sctp_sp_phashfn(sctp_t * sp)
{
	return sctp_phashfn(sp->p_tag);
}
static inline uint
sctp_sp_cachefn(sctp_t * sp)
{
	return (sp->hashent = sctp_cachefn(sp->v_tag));
}
static inline uint
sctp_sp_vhashfn(sctp_t * sp)
{
	return sctp_vhashfn(sp->v_tag);
}
static inline uint
sctp_sp_thashfn(sctp_t * sp)
{
	return sctp_thashfn(sp->sport, sp->dport);
}

extern rwlock_t sctp_hash_lock;
extern void sctp_init_locks(void);
extern void sctp_term_locks(void);

#define SCTPHASH_RLOCK()    read_lock_bh(&sctp_hash_lock)
#define SCTPHASH_RUNLOCK()  read_unlock_bh(&sctp_hash_lock)
#define SCTPHASH_WLOCK()    write_lock_bh(&sctp_hash_lock)
#define SCTPHASH_WUNLOCK()  write_unlock_bh(&sctp_hash_lock)

#define SCTPHASH_BH_RLOCK()    read_lock(&sctp_hash_lock)
#define SCTPHASH_BH_RUNLOCK()  read_unlock(&sctp_hash_lock)
#define SCTPHASH_BH_WLOCK()    write_lock(&sctp_hash_lock)
#define SCTPHASH_BH_WUNLOCK()  write_unlock(&sctp_hash_lock)

extern void sctp_phash_rehash(sctp_t * sp);
extern int sctp_bind_hash(sctp_t * sp, uint cons);
extern int sctp_conn_hash(sctp_t * sp);
extern void __sctp_unhash(sctp_t * sp);	/* without locks */
extern void __sctp_bind_unhash(sctp_t * sp);	/* without locks */
extern void __sctp_conn_unhash(sctp_t * sp);	/* without locks */

extern uint16_t sctp_get_port(void);

extern sctp_t *sctp_lookup_tcb(uint16_t sport, uint16_t dport, uint32_t saddr, uint32_t daddr);
extern sctp_t *sctp_lookup_ptag(uint32_t p_tag, uint16_t sport, uint16_t dport, uint32_t saddr,
				uint16_t daddr);
extern sctp_t *sctp_lookup(struct sctphdr *sh, uint32_t daddr, uint32_t saddr);

#endif				/* __SCTP_HASH_H__ */
