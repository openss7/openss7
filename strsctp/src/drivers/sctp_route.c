/*****************************************************************************

 @(#) $RCSfile: sctp_route.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/05/14 08:29:19 $

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

 Last Modified $Date: 2005/05/14 08:29:19 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_route.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/05/14 08:29:19 $"

static char const ident[] = "$RCSfile: sctp_route.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/05/14 08:29:19 $";

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
        sa_family_t     ss_family;
        char            __data[_SS_MAXSIZE - sizeof(sa_family_t)];
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
#include "os7/debug.h"
#endif
#include "os7/bufq.h"

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_cache.h"
#include "sctp_route.h"
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

/*
 *  =========================================================================
 *
 *  IP OUTPUT: ROUTING FUNCTIONS
 *
 *  =========================================================================
 *
 *  MULTI-HOMED IP ROUTING FUNCTIONS
 *
 *  -------------------------------------------------------------------------
 *
 *  CHOSE BEST
 *  -------------------------------------------------------------------------
 *  This function is called by sctp_update_routes to choose the best primary
 *  and secondary addresses.  We alway return a usable address if possible,
 *  even if the secondary is not different from the primary.  This is called
 *  only when a route fails or a destination goes inactive due to too many
 *  timeouts, so destinations that are timing out are not so large a factor.
 */
STATIC sctp_daddr_t *
sctp_choose_best(sp, not)
	sctp_t *sp;
	sctp_daddr_t *not;
{
	sctp_daddr_t *best, *sd;

	assert(sp);

	if ((best = sp->daddr))
		for (sd = best->next; sd; sd = sd->next) {
			/*
			   choose usable over unusable 
			 */
			if (!best->dst_cache && sd->dst_cache) {
				best = sd;
				continue;
			}
			/*
			   choose active routes 
			 */
			if (best->retransmits > best->max_retrans
			    && sd->retransmits <= sd->max_retrans) {
				best = sd;
				continue;
			}
			/*
			   choose routes without timeouts 
			 */
			if (best->retransmits && !(sd->retransmits)) {
				best = sd;
				continue;
			}
			/*
			   choose routes without dups 
			 */
			if (best->dups && !(sd->dups)) {
				best = sd;
				continue;
			}
			/*
			   choose usable alternate if possible 
			 */
			if (best == not && sd != not) {
				best = sd;
				continue;
			}
			/*
			   choose routes with least excessive timeouts 
			 */
			if (best->retransmits + sd->max_retrans >
			    sd->retransmits + best->max_retrans) {
				best = sd;
				continue;
			}
			/*
			   choose routes with the least duplicates 
			 */
			if (best->dups > sd->dups) {
				best = sd;
				continue;
			}
			/*
			   choose lowest rto 
			 */
			if (best->rto > sd->rto) {
				best = sd;
				continue;
			}
			/*
			   choose not to slow start 
			 */
			if (best->cwnd <= best->ssthresh && sd->cwnd > sd->ssthresh) {
				best = sd;
				continue;
			}
			/*
			   choose largest available window 
			 */
			if (best->cwnd + best->mtu - 1 - best->in_flight <
			    sd->cwnd + sd->mtu - 1 - sd->in_flight) {
				best = sd;
				continue;
			}
		}
	return (best);
}

/*
 *  UPDATE ROUTES
 *  -------------------------------------------------------------------------
 *  This function is called whenever there is a routing problem or whenever a
 *  timeout occurs on a destination.  It's purpose is to perform a complete
 *  re-analysis of the available destination addresses and IP routing and
 *  establish new routes as required and set the (primary) and (secondary)
 *  destination addresses.
 */
#ifndef sysctl_ip_dynaddr
#define sysctl_ip_dynaddr 0
#else
extern int sysctl_ip_dynaddr;
#endif
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP 132
#endif
#ifndef HAVE_STRUCT_DST_ENTRY_PATH
static inline u32
dst_pmtu(struct dst_entry *dst)
{
	return (dst->pmtu);
}
#endif
#if defined HAVE_NEW_STYLE_INET_PROTOCOL
#ifndef ip_route_output_flow
#ifdef HAVE_IP_ROUTE_OUTPUT_FLOW_ADDR
int
ip_route_output_flow(struct rtable **rp, struct flowi *flp, struct sock *sk, int flags) {
	int (*func) (struct rtable **, struct flowi *, struct sock *, int)
		= (typeof(func)) HAVE_IP_ROUTE_OUTPUT_FLOW_ADDR;
	return func(rp, flp, sk, flags);
}
#endif
#endif
#ifndef __ip_route_output_key
#ifdef HAVE___IP_ROUTE_OUTPUT_KEY_ADDR
int
__ip_route_output_key(struct rtable **rp, const struct flowi *flp) {
	int (*func) (struct rtable **, const struct flowi *)
		= (typeof(func)) HAVE___IP_ROUTE_OUTPUT_KEY_ADDR;
	return func(rp, flp);
}
#endif
#endif
#endif
/*
 *  sysctl_ip_dynaddr: this symbol is normally not exported, but we exported
 *  for the Linux Native version of SCTP, so we may have to treat it as extern
 *  here...
 */
int
sctp_update_routes(sp, force_reselect)
	sctp_t *sp;
	int force_reselect;
{
	int err = -EHOSTUNREACH;
	int mtu_changed = 0;
	int viable_route = 0;
	int route_changed = 0;

	sctp_daddr_t *sd;
	sctp_daddr_t *taddr, *raddr;

	int old_pmtu;

	assert(sp);
	ensure(sp->daddr, return (-EFAULT));

	old_pmtu = xchg(&sp->pmtu, INT_MAX);	/* big enough? */
	taddr = sp->taddr;
	raddr = sp->raddr;
	/*
	 *  First we check our cached routes..
	 */
	for (sd = sp->daddr; sd; sd = sd->next) {
		struct rtable *rt = (struct rtable *) sd->dst_cache;

		if (rt && (rt->u.dst.obsolete || rt->u.dst.ops->check(&rt->u.dst, 0))) {
			rare();
			sd->dst_cache = NULL;
			ip_rt_put(rt);
			sd->saddr = 0;

			route_changed = 1;
		}
		if (!rt) {
			if (sd->dif)
				sd->saddr = 0;
#if defined HAVE_OLD_STYLE_INET_PROTOCOL
			if ((err =
			     ip_route_connect(&rt, sd->daddr, sd->saddr,
					      RT_TOS(sp->ip_tos) | RTO_CONN | sp->ip_dontroute,
					      sd->dif)) < 0) {
				rare();
				continue;
			}
#elif defined HAVE_NEW_STYLE_INET_PROTOCOL
			if ((err =
			     ip_route_connect(&rt, sd->daddr, sd->saddr,
					      RT_TOS(sp->ip_tos) | RTO_CONN | sp->ip_dontroute,
					      sd->dif, IPPROTO_SCTP, sp->sport, sp->dport,
					      NULL)) < 0) {
				rare();
				continue;
			}
#else
#error One of HAVE_OLD_STYLE_INET_PROTOCOL or HAVE_NEW_STYLE_INET_PROTOCOL must be defined.
#endif
			if (rt->rt_flags & (RTCF_MULTICAST | RTCF_BROADCAST) && !sp->ip_broadcast) {
				rare();
				ip_rt_put(rt);
				err = -ENETUNREACH;
				continue;
			}
			sd->saddr = rt->rt_src;
			if (!sctp_find_saddr(sp, sd->saddr)) {
				rare();
				if (!sp->saddr) {
					rare();
					ip_rt_put(rt);
					assure(sp->saddr);
					err = -EADDRNOTAVAIL;
					continue;
				}
				sd->saddr = sp->saddr->saddr;
			}
			/*
			   always revert to initial settings when rerouting 
			 */
			sd->rto = sp->rto_ini;
			sd->rttvar = 0;
			sd->srtt = 0;
			sd->mtu = dst_pmtu(&rt->u.dst);
			sd->ssthresh = 2 * sd->mtu;
			sd->cwnd = sd->mtu;
			sd->dst_cache = &rt->u.dst;

			route_changed = 1;
		}
		/*
		   You're welcome diald! 
		 */
		if (sysctl_ip_dynaddr && sp->s_state == SCTP_COOKIE_WAIT && sd == sp->daddr) {
			/*
			   see if route changed on primary as result of INIT that was discarded 
			 */
			struct rtable *rt2 = NULL;
#if defined HAVE_OLD_STYLE_INET_PROTOCOL
			if (!ip_route_connect
			    (&rt2, rt->rt_dst, 0, RT_TOS(sp->ip_tos) | sp->ip_dontroute, sd->dif))
#elif defined HAVE_NEW_STYLE_INET_PROTOCOL
			if (!ip_route_connect
			    (&rt2, rt->rt_dst, 0, RT_TOS(sp->ip_tos) | sp->ip_dontroute, sd->dif,
			     IPPROTO_SCTP, sp->sport, sp->dport, NULL))
#else
#error One of HAVE_OLD_STYLE_INET_PROTOCOL or HAVE_NEW_STYLE_INET_PROTOCOL must be defined.
#endif
			{
				if (rt2->rt_src != rt->rt_src) {
					rare();
					rt2 = xchg(&rt, rt2);

					sd->rto = sp->rto_ini;
					sd->rttvar = 0;
					sd->srtt = 0;
					sd->mtu = dst_pmtu(&rt->u.dst);
					sd->ssthresh = 2 * sd->mtu;
					sd->cwnd = sd->mtu;
					sd->dst_cache = &rt->u.dst;

					route_changed = 1;
				}
				ip_rt_put(rt2);
			}
		}
		viable_route = 1;

		/*
		   always update MTU if we have a viable route 
		 */

		if (sd->mtu != dst_pmtu(&rt->u.dst)) {
			sd->mtu = dst_pmtu(&rt->u.dst);
			mtu_changed = 1;
			rare();
		}
		if (sp->pmtu > sd->mtu)
			sp->pmtu = sd->mtu;
	}
	if (!viable_route) {
		rare();

		/*
		   set defaults 
		 */
		sp->taddr = sp->daddr;
		sp->raddr = sp->daddr->next ? sp->daddr->next : sp->daddr;

		sp->pmtu = 576;

		return (err);
	}
	/*
	   if we have made or need changes then we want to reanalyze routes 
	 */
	if (force_reselect || route_changed || mtu_changed || sp->pmtu != old_pmtu || !sp->taddr
	    || !sp->raddr) {
#ifdef _DEBUG
#ifdef SCTP_CONFIG_ERROR_GENERATOR
		int bad_choice = 0;
#endif
#endif
		sp->taddr = sctp_choose_best(sp, NULL);
		usual(sp->taddr);

#ifdef _DEBUG
#ifdef SCTP_CONFIG_ERROR_GENERATOR
		if ((sp->options & SCTP_OPTION_BREAK)
		    && (sp->taddr == sp->daddr || sp->taddr == sp->daddr->next)
		    && sp->taddr->packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
			ptrace(("Primary sp->taddr %03d.%03d.%03d.%03d chosen poorly on %x\n",
				(sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff,
				(sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff,
				(uint) sp));
			bad_choice = 1;
		}
#endif
#endif

#if 0
		if (sp->taddr && sp->taddr != taddr)
			ptrace(("sp = %u, New primary route: %d.%d.%d.%d\n", (uint) sp,
				(sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff,
				(sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff));
#endif

		sp->raddr = sctp_choose_best(sp, sp->taddr);
		usual(sp->raddr);

#ifdef _DEBUG
#ifdef SCTP_CONFIG_ERROR_GENERATOR
		if ((sp->options & SCTP_OPTION_BREAK)
		    && (sp->raddr == sp->daddr || sp->raddr == sp->daddr->next)
		    && sp->raddr->packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
			ptrace(("Secondary sp->raddr %03d.%03d.%03d.%03d chosen poorly on %x\n",
				(sp->raddr->daddr >> 0) & 0xff, (sp->raddr->daddr >> 8) & 0xff,
				(sp->raddr->daddr >> 16) & 0xff, (sp->raddr->daddr >> 24) & 0xff,
				(uint) sp));
			bad_choice = 1;
		}
		if (bad_choice) {
			for (sd = sp->daddr; sd; sd = sd->next) {
				printk(KERN_INFO
				       "%03d.%03d.%03d.%03d: %x, %u:%u, %u, %lu, %u:%u, %u, %u\n",
				       (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
				       (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff,
				       (uint) sd->dst_cache, sd->retransmits, sd->max_retrans,
				       sd->dups, sd->rto, sd->cwnd, sd->ssthresh, sd->mtu,
				       sd->in_flight);
			}
		}
#endif
#endif

#if 0
		if (sp->raddr && sp->raddr != raddr)
			ptrace(("sp = %u, New secondary route: %d.%d.%d.%d\n", (uint) sp,
				(sp->raddr->daddr >> 0) & 0xff, (sp->raddr->daddr >> 8) & 0xff,
				(sp->raddr->daddr >> 16) & 0xff, (sp->raddr->daddr >> 24) & 0xff));
#endif

	}
	return (0);
}
