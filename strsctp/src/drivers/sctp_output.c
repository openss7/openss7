/*****************************************************************************

 @(#) $RCSfile: sctp_output.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:10:56 $

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

 Last Modified $Date: 2006/10/27 23:10:56 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_output.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:10:56 $"

static char const ident[] =
    "$RCSfile: sctp_output.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:10:56 $";

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
#   include <linux/autoconf.h>
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
#include "sctp_hash.h"
#include "sctp_cache.h"
#include "sctp_route.h"

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
#include "sctp_output.h"

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

/*
 *  QUEUE XMIT (Queue for transmission)
 *  -------------------------------------------------------------------------
 *  We need this broken out so that we can use the netfilter hooks.
 */
#if defined HAVE_OLD_STYLE_INET_PROTOCOL
static inline int
sctp_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;

	if (skb->len > rt->u.dst.pmtu) {
		rare();
		return ip_fragment(skb, skb->dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return skb->dst->output(skb);
	}
}
#else
#define sctp_queue_xmit ip_queue_xmit
#endif

/*
 *  XMIT OOTB (Disconnect Send with no Listening STREAM).
 *  -------------------------------------------------------------------------
 *  This sends disconnected without a STREAM.  All that is needed is a
 *  destination address and a message block.  The only time that we use this
 *  is for responding to OOTB packets with ABORT or SHUTDOWN COMPLETE.
 */
void
sctp_xmit_ootb(daddr, saddr, mp)
	uint32_t daddr;
	uint32_t saddr;
	mblk_t *mp;
{
	struct rtable *rt = NULL;

	ensure(mp, return);
	if (!ip_route_output(&rt, daddr, 0, 0, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp);
		size_t tlen = plen + sizeof(struct iphdr);

		usual(hlen);
		usual(plen);

		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			struct sctphdr *sh;
			unsigned char *data;

			skb_reserve(skb, hlen);
			iph = (struct iphdr *) __skb_put(skb, tlen);
			sh = (struct sctphdr *) (iph + 1);
			data = (unsigned char *) (sh);

			skb->dst = &rt->u.dst;
			skb->priority = 0;

			iph->version = 4;
			iph->ihl = 5;
			iph->tos = 0;
			iph->frag_off = 0;
			iph->ttl = 0;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = 132;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);

			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;

				if (blen > 0) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
				} else
					rare();
			}
			sh->check = 0;
			sh->check = htonl(crc32c(~0UL, (unsigned char *) sh, plen));

			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
		} else
			rare();
	} else
		rare();
	/* sending OOTB reponses are one time events, if we can't send the message we just drop it, 
	   the peer will probably come back again later */
	freemsg(mp);
	return;
}

/*
 *
 *  XMIT MSG  (Disconnected Send)
 *  -------------------------------------------------------------------------
 *  This sends disconnected sends.  All that is needed is a destination
 *  address and a message block.  The only time that we use this is for
 *  sending INIT ACKs (because we have not built a complete stream yet,
 *  however we do at least have a Listening STREAM or possibly an established
 *  STREAM that is partially qualitfied and can provide some TOS and other
 *  information for the packet.
 */
void
sctp_xmit_msg(daddr, mp, sp)
	uint32_t daddr;
	mblk_t *mp;
	sctp_t *sp;
{
	struct rtable *rt = NULL;

	ensure(mp, return);
	if (!ip_route_output(&rt, daddr, 0, RT_TOS(sp->ip_tos) | sp->ip_dontroute, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp);
		size_t tlen = plen + sizeof(struct iphdr);
		size_t alen = 0;

		usual(hlen);
		usual(plen);

		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			struct sctphdr *sh;
			unsigned char *data;

			skb_reserve(skb, hlen);
			iph = (struct iphdr *) __skb_put(skb, tlen);
			sh = (struct sctphdr *) (iph + 1);
			data = (unsigned char *) (sh);

			skb->dst = &rt->u.dst;
			skb->priority = sp->ip_priority;

			iph->version = 4;
			iph->ihl = 5;
			iph->tos = sp->ip_tos;
			iph->frag_off = 0;
			iph->ttl = sp->ip_ttl;
			iph->daddr = rt->rt_dst;
			iph->saddr = rt->rt_src;
			iph->protocol = sp->ip_proto;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);

			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;

				if (blen > 0) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
					alen += blen;
				} else
					rare();
			}
			sh->check = 0;
			sh->check = htonl(crc32c(~0UL, (unsigned char *) sh, plen));

			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
		} else
			rare();
	} else
		rare();
	/* sending INIT ACKs are one time events, if we can't get the response off, we just drop
	   the INIT ACK: the peer should send us another INIT * in a short while... */
	freemsg(mp);
	return;
}

/*
 *  SEND MSG  (Connected Send)
 *  -------------------------------------------------------------------------
 *  This sends connected sends.  It requires a STREAM a desination address
 *  structure and a message block.  This function does not free the message
 *  block.  The caller is responsible for the message block.
 */
STATIC int break_packets = 0;
STATIC int seed = 152;

STATIC unsigned char
random(void)
{
	return (unsigned char) (seed = seed * 60691 + 1);
}

void
sctp_send_msg(sp, sd, mp)
	sctp_t *sp;
	sctp_daddr_t *sd;
	mblk_t *mp;
{
	int tried_update;

	ensure(sp, return);
	ensure(mp, return);

	for (tried_update = 0; !tried_update; sctp_update_routes(sp, 1), tried_update++) {
		if (sd && sd->dst_cache &&
		    (!sd->dst_cache->obsolete || sd->dst_cache->ops->check(sd->dst_cache, 0))) {

			struct sk_buff *skb;
			struct net_device *dev;
			size_t hlen, plen, tlen;

#ifdef SCTP_CONFIG_ERROR_GENERATOR
			if ((sp->options & SCTP_OPTION_DBREAK)
			    && sd->daddr == 0x010000ff
			    && ++break_packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
				if (break_packets > SCTP_CONFIG_BREAK_GENERATOR_LIMIT)
					break_packets = 0;
				return;
			}
			if ((sp->options & SCTP_OPTION_BREAK)
			    && (sd == sp->daddr || sd == sp->daddr->next)
			    && ++sd->packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
				return;
			}
			if ((sp->options & SCTP_OPTION_DROPPING)
			    && ++sd->packets > SCTP_CONFIG_ERROR_GENERATOR_LEVEL) {
				if (sd->packets > SCTP_CONFIG_ERROR_GENERATOR_LIMIT)
					sd->packets = 0;
				return;
			}
			if ((sp->options & SCTP_OPTION_RANDOM)
			    && ++sd->packets > 2 * SCTP_CONFIG_ERROR_GENERATOR_LEVEL) {
				if (!(random() & 0x03))
					return;
			}
#endif
			dev = sd->dst_cache->dev;
			plen = SCTP_TCB(mp)->dlen;
			hlen = (dev->hard_header_len + 15) & ~15;
			tlen = sizeof(struct iphdr) + plen;

			unusual(plen == 0 || plen > 1 << 14);

			if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
				mblk_t *bp;
				struct iphdr *iph;
				struct sctphdr *sh;
				unsigned char *head, *data;
				size_t alen = 0;

				skb_reserve(skb, hlen);
				iph = (struct iphdr *) __skb_put(skb, tlen);
				sh = (struct sctphdr *) (iph + 1);
				data = (unsigned char *) (sh);
				head = data;

				skb->dst = dst_clone(sd->dst_cache);
				skb->priority = sp->ip_priority;

				iph->version = 4;
				iph->ihl = 5;
				iph->tos = sp->ip_tos;
				iph->frag_off = 0;
				iph->ttl = sp->ip_ttl;
				iph->daddr = sd->daddr;
				iph->saddr = sd->saddr;
				iph->protocol = sp->ip_proto;
				iph->tot_len = htons(tlen);
				skb->nh.iph = iph;
				__ip_select_ident(iph, sd->dst_cache);

				for (bp = mp; bp; bp = bp->b_next) {
					mblk_t *db;
					size_t clen = 0;
					sctp_tcb_t *cb = SCTP_TCB(bp);

					cb->daddr = sd;
					cb->when = jiffies;
					cb->flags |= SCTPCB_FLAG_SENT;
					cb->trans++;

					for (db = bp; db; db = db->b_cont) {
						int blen = db->b_wptr - db->b_rptr;

						normal(db->b_datap->db_type == M_DATA);
						if (db->b_datap->db_type == M_DATA) {
							normal(blen > 0);
							if (blen > 0) {
								ensure(head + plen >= data + blen,
								       kfree_skb(skb); return);
								bcopy(db->b_rptr, data, blen);
								data += blen;
								clen += blen;
							}
						}
					}
					{
						/* pad each chunk if not padded already */
						size_t pad = PADC(clen) - clen;

						ensure(head + plen >= data + pad, kfree_skb(skb);
						       return);
						bzero(data, pad);
						data += pad;
						alen += clen + pad;
					}
				}
				if (alen != plen) {
					usual(alen == plen);
					ptrace(("alen = %u, plen = %u discarding\n", alen, plen));
					kfree_skb(skb);
					return;
				}

				sh->check = 0;
				sh->check = htonl(crc32c(~0UL, (unsigned char *) sh, plen));

				NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
				/*
				 *  Whenever we transmit something, we expect a reply to our v_tag, so
				 *  we put ourselves in the 1st level vtag caches expecting a quick
				 *  reply.
				 */
				if (!((1 << sp->s_state) & (SCTPF_CLOSED | SCTPF_LISTEN)))
					sctp_cache[sp->hashent] = sp;
				break;

			} else
				rare();
		} else
			usual(sd && sd->dst_cache &&
			      (!sd->dst_cache->obsolete
			       || sd->dst_cache->ops->check(sd->dst_cache, 0)));
	}
}
