/*****************************************************************************

 @(#) $RCSfile: sctp_hash.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2006/09/26 00:54:51 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2006/09/26 00:54:51 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_hash.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2006/09/26 00:54:51 $"

static char const ident[] =
    "$RCSfile: sctp_hash.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2006/09/26 00:54:51 $";

#define __NO_VERSION__

#if defined LIS && !defined _LIS_SOURCE
#define _LIS_SOURCE
#endif

#if defined LFS && !defined _LFS_SOURCE
#define _LFS_SOURCE
#endif

#if !defined _LIS_SOURCE && !defined _LFS_SOURCE
#   error ****
#   error **** One of _LFS_SOURCE or _LIS_SOURCE must be defined
#   error **** to compile the sctp driver.
#   error ****
#endif

#ifdef LINUX
#   include <linux/config.h>
#   include <linux/version.h>
#   ifndef HAVE_SYS_LIS_MODULE_H
#	include <linux/module.h>
#	include <linux/init.h>
#   else
#	include <sys/LiS/module.h>
#   endif
#endif

#if defined HAVE_OPENSS7_SCTP
#   if !defined CONFIG_SCTP && !defined CONFIG_SCTP_MODULE
#	undef HAVE_OPENSS7_SCTP
#   endif
#endif

#if defined HAVE_OPENSS7_SCTP
#   define sctp_bind_bucket __sctp_bind_bucket
#   define sctp_dup __sctp_dup
#   define sctp_strm __sctp_strm
#   define sctp_saddr __sctp_saddr
#   define sctp_daddr __sctp_daddr
#   define sctp_protolist __sctp_protolist
#endif

#if defined HAVE_LKSCTP_SCTP
#   if !defined CONFIG_IP_SCTP && !defined CONFIG_IP_SCTP_MODULE
#	undef HAVE_LKSCTP_SCTP
#   endif
#endif

#if defined HAVE_LKSCTP_SCTP
#   define sctp_bind_bucket __sctp_bind_bucket
#   define sctp_mib	    __sctp_mib
#   define sctphdr	    __sctphdr
#   define sctp_cookie	    __sctp_cookie
#   define sctp_chunk	    __sctp_chunk
#endif

#if !defined HAVE_OPENSS7_SCTP
#   undef sctp_addr
#   define sctp_addr stupid_sctp_addr_in_the_wrong_place
#endif

#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>

#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>

#if defined HAVE_OPENSS7_SCTP
#   undef STATIC
#   undef INLINE
#   include <net/sctp.h>
#endif

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
#define _SS_MAXSIZE     128
#define _SS_ALIGNSIZE   (__alignof__ (struct sockaddr *))
struct sockaddr_storage {
	sa_family_t ss_family;
	char __data[_SS_MAXSIZE - sizeof(sa_family_t)];
} __attribute__ ((aligned(_SS_ALIGNSIZE)));
#endif

#include <sys/kmem.h>
#include <sys/cmn_err.h>
#include <sys/stream.h>
#include <sys/dki.h>

#ifdef LFS_SOURCE
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/strdebug.h>
#include <sys/debug.h>
#endif
#include <sys/ddi.h>

#ifndef LFS
#include <os7/debug.h>
#endif
#include <os7/bufq.h>

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_cache.h"
#include "sctp_hash.h"

sctp_t *sctp_bhash[sctp_bhash_size] = { NULL, };	/* bind */
sctp_t *sctp_lhash[sctp_lhash_size] = { NULL, };	/* listen */
sctp_t *sctp_phash[sctp_phash_size] = { NULL, };	/* p_tag */
sctp_t *sctp_cache[sctp_cache_size] = { NULL, };	/* v_tag *//* level 1 */
sctp_t *sctp_vhash[sctp_vhash_size] = { NULL, };	/* v_tag *//* level 2 */
sctp_t *sctp_thash[sctp_thash_size] = { NULL, };	/* tcb *//* level 4 */

STATIC void
sctp_bhash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_bhash[sctp_sp_bhashfn(sp)];

	if ((sp->bnext = *spp))
		sp->bnext->bprev = &sp->bnext;
	sp->bprev = spp;
	*spp = sp;
}
STATIC void
sctp_lhash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_lhash[sctp_sp_lhashfn(sp)];

	if ((sp->lnext = *spp))
		sp->lnext->lprev = &sp->lnext;
	sp->lprev = spp;
	*spp = sp;
}
STATIC void
sctp_phash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_phash[sctp_sp_phashfn(sp)];

	if ((sp->pnext = *spp))
		sp->pnext->pprev = &sp->pnext;
	sp->pprev = spp;
	*spp = sp;
}
STATIC void
sctp_vhash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	sctp_t **scp = &sctp_cache[sctp_sp_cachefn(sp)];

	if ((sp->vnext = *spp))
		sp->vnext->vprev = &sp->vnext;
	sp->vprev = spp;
	*spp = sp;
	*scp = sp;
}
STATIC void
sctp_thash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_thash[sctp_sp_thashfn(sp)];

	if ((sp->tnext = *spp))
		sp->tnext->tprev = &sp->tnext;
	sp->tprev = spp;
	*spp = sp;
}

STATIC void
sctp_bhash_unhash(sctp_t * sp)
{
	if (sp->bprev) {
		if ((*(sp->bprev) = sp->bnext))
			sp->bnext->bprev = sp->bprev;
		sp->bnext = NULL;
		sp->bprev = NULL;
	}
}
STATIC void
sctp_lhash_unhash(sctp_t * sp)
{
	if (sp->lprev) {
		if ((*(sp->lprev) = sp->lnext))
			sp->lnext->lprev = sp->lprev;
		sp->lnext = NULL;
		sp->lprev = NULL;
	}
}
STATIC void
sctp_phash_unhash(sctp_t * sp)
{
	if (sp->pprev) {
		if ((*(sp->pprev) = sp->pnext))
			sp->pnext->pprev = sp->pprev;
		sp->pnext = NULL;
		sp->pprev = NULL;
	}
}
STATIC void
sctp_vhash_unhash(sctp_t * sp)
{
	if (sp->vprev) {
		if ((*(sp->vprev) = sp->vnext))
			sp->vnext->vprev = sp->vprev;
		sp->vnext = NULL;
		sp->vprev = NULL;
		if (sctp_cache[sp->hashent] == sp)
			sctp_cache[sp->hashent] = NULL;
		sp->hashent = 0;
	}
}
STATIC void
sctp_thash_unhash(sctp_t * sp)
{
	if (sp->tprev) {
		if ((*(sp->tprev) = sp->tnext))
			sp->tnext->tprev = sp->tprev;
		sp->tnext = NULL;
		sp->tprev = NULL;
	}
}
void
__sctp_unhash(sctp_t * sp)
{
	assert(sp);
	sctp_phash_unhash(sp);
	sctp_thash_unhash(sp);
	sctp_vhash_unhash(sp);
	sctp_lhash_unhash(sp);
	sctp_bhash_unhash(sp);
}
STATIC void
__sctp_phash_rehash(sctp_t * sp)
{
	sctp_phash_unhash(sp);
	if (sp->p_tag)
		sctp_phash_insert(sp);
}

void
sctp_phash_rehash(sctp_t * sp)
{
	SCTPHASH_WLOCK(); {
		__sctp_phash_rehash(sp);
	} SCTPHASH_WUNLOCK();
}

/*
 *  Add to Bind Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
int
sctp_bind_hash(sp, cons)
	sctp_t *sp;
	uint cons;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		unusual(sp->bprev);
		sctp_bhash_unhash(sp);	/* start clean */
		unusual(sp->lprev);
		sctp_lhash_unhash(sp);	/* start clean */
		if (cons) {
			sctp_t *sp2, **spp = &sctp_lhash[sctp_sp_lhashfn(sp)];

			/* check for conflicts */
			for (sp2 = *spp; sp2; sp2 = sp2->lnext) {
				if (!sp2->sport || !sp->sport || sp2->sport == sp->sport) {
					if (sp2->sanum && sp->sanum) {
						sctp_saddr_t *ss, *ss2;

						for (ss = sp->saddr; ss; ss = ss->next)
							for (ss2 = sp2->saddr; ss2; ss2 = ss2->next) {
								if (ss->saddr != ss2->saddr)
									continue;
								SCTPHASH_WUNLOCK();
								return (-EADDRINUSE);
							}
					} else {
						SCTPHASH_WUNLOCK();
						return (-EADDRINUSE);
					}
				}
			}
			sctp_lhash_insert(sp);
		}
		sctp_bhash_insert(sp);
	}
	SCTPHASH_WUNLOCK();
	return (0);
}

/*
 *  Remove from Bind Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void
__sctp_bind_unhash(sp)
	sctp_t *sp;
{
	assert(sp);
	sctp_lhash_unhash(sp);
	sctp_bhash_unhash(sp);
}

/*
 *  Add to Connection Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
int
sctp_conn_hash(sp)
	sctp_t *sp;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		unusual(sp->tprev);
		sctp_thash_unhash(sp);	/* start clean */
		unusual(sp->vprev);
		sctp_vhash_unhash(sp);	/* start clean */
		unusual(sp->pprev);
		sctp_phash_unhash(sp);	/* start clean */
		{
			sctp_t *sp2, **spp = &sctp_thash[sctp_sp_thashfn(sp)];

			/* check for conflicts */
			for (sp2 = *spp; sp2; sp2 = sp2->tnext) {
				if (sp2->sport == sp->sport && sp2->dport == sp->dport) {
					sctp_saddr_t *ss, *ss2;

					for (ss = sp->saddr; ss; ss = ss->next)
						for (ss2 = sp2->saddr; ss2; ss2 = ss2->next) {
							sctp_daddr_t *sd, *sd2;

							if (ss->saddr != ss2->saddr)
								continue;
							for (sd = sp->daddr; sd; sd = sd->next)
								for (sd2 = sp2->daddr; sd2;
								     sd2 = sd2->next) {
									if (sd->daddr != sd2->daddr)
										continue;
									SCTPHASH_WUNLOCK();
									return (-EADDRINUSE);	/* conflict 
												 */
								}
						}
				}
			}
			sctp_thash_insert(sp);
		}
		sctp_vhash_insert(sp);
		sctp_phash_insert(sp);
	}
	SCTPHASH_WUNLOCK();
	return (0);
}

/*
 *  Remove from Connection Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void
__sctp_conn_unhash(sp)
	sctp_t *sp;
{
	assert(sp);
	sctp_thash_unhash(sp);
	sctp_vhash_unhash(sp);
	sctp_phash_unhash(sp);
}

/*
 *  Get a fresh unused local port number
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_port_rover = 0;

#ifdef sysctl_local_port_range
extern int sysctl_local_port_range[2];
#else
STATIC int sysctl_local_port_range[2] = { 1024, 4999 };
#endif

uint16_t
sctp_get_port(void)
{
	uint16_t snum;
	uint16_t sport = 0;
	sctp_t *sb = NULL;

	int low = sysctl_local_port_range[0];
	int high = sysctl_local_port_range[1];
	int rem = (high - low) + 1;

	seldom();

	snum = sctp_port_rover;
	if (snum > high || snum < low) {
		rare();
		snum = low;
	}

	/* find a fresh, completely unused port number */
	for (; rem > 0; snum++, rem--) {
		if (snum > high || snum < low) {
			rare();
			snum = low;
		}
		sport = htons(snum);
		if (!(sb = sctp_bhash[sctp_bhashfn(sport)]))
			break;
		for (; sb; sb = sb->bnext)
			if (sb->sport == sport) {
				seldom();
				break;
			}
		if (sb)
			break;
	}
	if (rem <= 0 || sb) {
		rare();
		return (0);
	}

	sctp_port_rover = snum;
	usual(sport);
	return (sport);
}

/*
 *  =========================================================================
 *
 *  LOOKUP Functions
 *
 *  =========================================================================
 *
 *  A fast caching hashing lookup function for SCTP.
 *
 *  IMPLEMENTATION NOTES:- All but a few SCTP messages carry our Verification
 *  Tag.  If the message requires our Verification Tag and we cannot lookup
 *  the stream on the Verification Tag we treat the packet similar to an OOTB
 *  packet.  The only restriction that this approach imposes is in the
 *  selection of our Verification Tag, which cannot be identical to any other
 *  Verification Tag which we have chosen so far.  We, therefore, check the
 *  Verification Tag selected at initialization against the cache for
 *  uniqueness.  This also allows us to acquire the Verification Tag to
 *  minimize collisions on the hash table.  This allows us to simplify the
 *  hashing function because we are guaranteeing equal hash coverage using
 *  selection.
 */
/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP LISTEN - LISTEN hash (sport)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Listener stream lookup with wildcards.  This will find any bound listener
 *  stream from the destination address, destination port and device index.
 *  This is only for INIT and COOKIE ECHO.
 */
STATIC sctp_t *
sctp_lookup_listen(uint16_t dport, uint32_t daddr)
{
	sctp_t *sp, *result = NULL;
	int hiscore = 0;

	for (sp = sctp_lhash[sctp_lhashfn(dport)]; sp; sp = sp->next) {
		int score = 0;

		if (sp->sport) {
			if (sp->sport != dport)
				continue;
			score++;
		}
		if (sp->saddr) {
			if (!sctp_find_saddr(sp, daddr))
				continue;
			score++;
		}
		if (score == 2) {
			result = sp;
			break;
		}
		if (score > hiscore) {
			hiscore = score;
			result = sp;
		}
	}
	usual(result);
	return result;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - TCB hash (port pair)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We do the hard match here, because we don't have valid v_tags or p_tags.
 *  We don't have the information anyways.  We are just looking for an
 *  established stream which can accept the packet based on port numbers and
 *  source and destination addresses.  This is used for INIT and exceptional
 *  COOKIE ECHO cases as well as ICMP lookups for failed sent INIT messages.
 *
 */
#define sctp_match_tcb(__sp, __saddr, __daddr, __sport, __dport) \
	( ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__saddr))) && \
	  (sctp_find_daddr((__sp),(__daddr))) )

sctp_t *
sctp_lookup_tcb(uint16_t sport, uint16_t dport, uint32_t saddr, uint32_t daddr)
{
	sctp_t *sp;

	for (sp = sctp_thash[sctp_thashfn(sport, dport)]; sp; sp = sp->tnext)
		if (sctp_match_tcb(sp, saddr, daddr, sport, dport))
			break;
	return sp;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP PTAG - Peer hash (peer tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We can either match fast and loose or slow and sure.  We have a peer tag
 *  which is validation enough without walking the address lists most of the
 *  time.  The INIT and COOKIE ECHO stuff needs it for checking peer tags and
 *  peer tie tags.  ICMP needs this for looking up all packets which were
 *  returned that we sent out with the peer's tag (excepts INIT which has no
 *  tag, we use TCB ICMP lookup for that).  ICMP lookups match with reversed
 *  source and destination addresses.
 */
#ifndef SCTP_SLOW_VERIFICATION
#define	sctp_match_ptag(__sp, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == (__sp)->p_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) )
#else
#define	sctp_match_ptag(__sp, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == (__sp)->p_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__saddr))) && \
	  (sctp_find_daddr((__sp),(__daddr))) )
#endif

sctp_t *
sctp_lookup_ptag(uint32_t p_tag, uint16_t sport, uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	sctp_t *sp;

	(void) saddr;
	(void) daddr;

	for (sp = sctp_phash[sctp_phashfn(p_tag)]; sp; sp = sp->pnext)
		if (sctp_match_ptag(sp, saddr, daddr, p_tag, sport, dport))
			break;
	return sp;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP VTAG - Established hash (local tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  This is the main lookup for data transfer on established streams.  This
 *  should run as fast and furious as possible.  We run fast and loose and
 *  rely on the verification tag and port numbers only.  We cache and hash
 *  so a stream of back-to-back packets to the same destination (bursty
 *  traffic) will whirl.  Because we MD4 hash verification tags when we
 *  generate them, the hash should get good random distribution with minimum
 *  collisions.
 *
 */
#ifndef	SCTP_SLOW_VERIFICATION
#define sctp_match_vtag(__sp, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == (__sp)->v_tag) && \
	  ((__sport) == (__sp)->sport) )
#else
#define sctp_match_vtag(__sp, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == (__sp)->v_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__daddr))) && \
	  (sctp_find_daddr((__sp),(__saddr))) )
#endif

STATIC sctp_t *
sctp_lookup_vtag(uint32_t v_tag, uint16_t sport, uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	sctp_t *sp;
	unsigned int hash = sctp_cachefn(v_tag);

	(void) saddr;
	(void) daddr;

	if (!(sp = sctp_cache[hash]) || !sctp_match_vtag(sp, saddr, daddr, v_tag, sport, dport))
		for (sp = sctp_vhash[sctp_vhashfn(v_tag)]; sp; sp = sp->next)
			if (sctp_match_vtag(sp, saddr, daddr, v_tag, sport, dport)) {
				sctp_cache[hash] = sp;
				break;
			}
	return sp;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP COOKIE ECHO - Special lookup rules for cookie echo chunks
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC sctp_t *
sctp_lookup_cookie_echo(ck, v_tag, sport, dport, saddr, daddr)
	struct sctp_cookie *ck;
	uint32_t v_tag;
	uint16_t dport;
	uint16_t sport;
	uint32_t daddr;
	uint32_t saddr;
{
	sctp_t *sp = NULL;

	/* quick sanity checks on cookie */
	if (ck->v_tag == v_tag && ck->sport == sport && ck->dport == dport) {
		if (		/* RFC 2960 5.2.4 (A) */
			   (ck->l_ttag && ck->p_ttag
			    && (sp = sctp_lookup_vtag(ck->l_ttag, sport, dport, saddr, daddr)))
			   /* RFC 2960 5.2.4 (B) */
			   || ((sp = sctp_lookup_vtag(v_tag, sport, dport, saddr, daddr)))
			   /* RFC 2960 5.2.4 (C) */
			   || (!ck->l_ttag && !ck->p_ttag
			       && (sp = sctp_lookup_ptag(ck->p_tag, sport, dport, saddr, daddr)))
			   /* RFC 2960 5.2.4 (D) */
			   || ((sp = sctp_lookup_listen(sport, saddr))))
			return (sp);
	} else
		rare();
	seldom();
	return (NULL);
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - Established hash (local verification tag with fallbacks)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Stream hash lookup with fast path for data.  This uses verification tag
 *  when it is available.  Source address and port are checked after the
 *  verification tag matches.  When called for INIT and COOKIE ECHO messages,
 *  the function returns a listening (bind) hash lookup.  For SHUTDOWN
 *  COMPLETE and ABORT messages with the T-bit set, an icmp (peer tag) lookup
 *  is performed instead.
 */
sctp_t *
sctp_lookup(struct sctphdr * sh, uint32_t daddr, uint32_t saddr)
{
	sctp_t *sp = NULL;
	struct sctpchdr *ch = (struct sctpchdr *) (sh + 1);
	int ctype = ch->type & SCTP_CTYPE_MASK;
	uint32_t v_tag = sh->v_tag;
	uint16_t dport = sh->dest;
	uint16_t sport = sh->srce;

	if (v_tag) {
		/* fast path */
		if (ctype == SCTP_CTYPE_SACK || ctype == SCTP_CTYPE_DATA)
			return sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr);

		switch (ctype) {
			/* See RFC 2960 Section 8.5.1 */
		case SCTP_CTYPE_ABORT:
		case SCTP_CTYPE_SHUTDOWN_COMPLETE:
			if (ch->flags & 0x1)	/* T bit set */
				return sctp_lookup_ptag(v_tag, dport, sport, daddr, saddr);
		default:
			if ((sp = sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr)))
				return (sp);
			if (ctype == SCTP_CTYPE_ABORT)
				/* check abort for conn ind */
				if ((sp = sctp_lookup_listen(dport, daddr)))
					return (sp);
		case SCTP_CTYPE_INIT:
			break;
		case SCTP_CTYPE_COOKIE_ECHO:{
			struct sctp_cookie *ck =
			    (struct sctp_cookie *) ((struct sctp_cookie_echo *) ch)->cookie;
			return sctp_lookup_cookie_echo(ck, v_tag, dport, sport, daddr, saddr);
		}
		}
	} else if (ctype == SCTP_CTYPE_INIT)
		if ((sp = sctp_lookup_listen(dport, daddr))
		    || (sp = sctp_lookup_tcb(dport, sport, daddr, saddr)))
			return (sp);

	seldom();
	return (NULL);
}

rwlock_t sctp_hash_lock;

void
sctp_init_locks(void)
{
	rwlock_init(&sctp_hash_lock);
}

void
sctp_term_locks(void)
{
}
