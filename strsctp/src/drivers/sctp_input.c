/*****************************************************************************

 @(#) $RCSfile: sctp_input.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/07/16 12:46:49 $

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

 Last Modified $Date: 2006/07/16 12:46:49 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_input.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/07/16 12:46:49 $"

static char const ident[] =
    "$RCSfile: sctp_input.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/07/16 12:46:49 $";

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
#include "sctp_input.h"
#include "sctp_output.h"

#include <linux/skbuff.h>

/*
 *  =========================================================================
 *
 *  IP Packet Handling
 *
 *  =========================================================================
 */

STATIC int
sctp_rcv_ootb(mblk_t *mp)
{
	struct iphdr *iph = (struct iphdr *) mp->b_datap->db_base;
	struct sctphdr *sh = (struct sctphdr *) (mp->b_datap->db_base + (iph->ihl << 2));
	struct sctpchdr *ch = (struct sctpchdr *) mp->b_rptr;
	int sat = inet_addr_type(iph->saddr);

	seldom();
	ensure(mp, return (-EFAULT));
	if (sat != RTN_UNICAST && sat != RTN_LOCAL) {
		/* 
		   RFC 2960 8.4(1). */
		freemsg(mp);
		return (0);
	}
	switch (ch->type) {
	case SCTP_CTYPE_COOKIE_ACK:	/* RFC 2960 8.4(7).  */
	case SCTP_CTYPE_ERROR:	/* RFC 2960 8.4(7).  */
	case SCTP_CTYPE_ABORT:	/* RFC 2960 8.4(2).  */
	case SCTP_CTYPE_SHUTDOWN_COMPLETE:	/* RFC 2960 8.4(6).  */
	case SCTP_CTYPE_INIT:	/* RFC 2960 8.4(3) and (8). */
	case SCTP_CTYPE_INIT_ACK:	/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_COOKIE_ECHO:	/* RFC 2960 8.4(4) and (8). */
	default:
		seldom();
		break;
	case SCTP_CTYPE_SHUTDOWN:	/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_SACK:	/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_HEARTBEAT:	/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_HEARTBEAT_ACK:	/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_DATA:	/* RFC 2960 8.4(8).  */
		seldom();
		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		break;
	case SCTP_CTYPE_SHUTDOWN_ACK:
		seldom();
		sctp_send_shutdown_complete_ootb(iph->saddr, iph->daddr, sh);
		break;
	}
	ptrace(("Freeing mblk %p\n", mp));
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP ERR
 *
 *  -------------------------------------------------------------------------
 *  We have received an ICMP error for the protocol number.  Because we don't
 *  want an races here we place a M_ERROR message on the read queue of the
 *  stream to which the message applies.  This distinguishes it from M_DATA
 *  messages.  It is processed within the stream with queues locked.  We have
 *  to copy the information because the skb will go away after this call.
 */
STATIC void
sctp_err(struct sk_buff *skb, u32 info)
{
	sctp_t *sp;
	struct sctphdr *sh;
	struct iphdr *iph = (struct iphdr *) skb->data;
	size_t ihl;

	ensure(skb, return);
#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH) {
		seldom();
		return;
	}
	sh = (struct sctphdr *) (skb->data + ihl);
	SCTPHASH_BH_RLOCK();
	sp = sh->v_tag ? sctp_lookup_ptag(sh->v_tag, sh->srce, sh->dest, iph->saddr,
					  iph->daddr) : sctp_lookup_tcb(sh->srce, sh->dest,
									iph->saddr, iph->daddr);
	SCTPHASH_BH_RUNLOCK();
	usual(sp);
	if (sp) {
		mblk_t *mp;
		size_t mlen;
		mlen = sizeof(uint32_t) + sizeof(struct icmphdr *);
		if ((mp = allocb(mlen, BPRI_MED))) {
			mp->b_datap->db_type = M_ERROR;
			*((uint32_t *) mp->b_wptr)++ = iph->daddr;
			*((struct icmphdr *) mp->b_wptr)++ = *(skb->h.icmph);
			if (canput(sp->rq)) {
				if (!putq(sp->rq, mp))
					__ctrace(freemsg(mp));	/* FIXME */
				return;
			}
			freemsg(mp);
			return;
		}
		rare();
		return;
	} else
		usual(sp);
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP RCV
 *
 *  -------------------------------------------------------------------------
 *  This is the received frame handler for SCTP IPv4.  All packets received by
 *  IPv4 with the protocol number IPPROTO_SCTP will arrive here first.  We
 *  should be performing the Adler-32 checksum on the packet.  If the Adler-32
 *  checksum fails, then we should silently discard per RFC 2960.
 */
STATIC streamscall void
sctp_free(caddr_t data)
{
	struct sk_buff *skb = (struct sk_buff *) data;

	trace();
	ensure(skb, return);
	kfree_skb(skb);
	return;
}
STATIC int
sctp_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	sctp_t *sp;
	uint32_t csum0, csum1, csum2;
	struct sctphdr *sh;
	unsigned short len;
	frtn_t fr = { &sctp_free, (char *) skb };

	if (skb->pkt_type == PACKET_HOST) {
		/* 
		   For now...  We should actually place non-linear fragments into seperate mblks
		   and pass them up as a chain. */
#ifdef HAVE_KFUNC_SKB_LINEARIZE_1_ARG
		if (!skb_is_nonlinear(skb) || skb_linearize(skb) == 0)
#else			/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
		if (!skb_is_nonlinear(skb) || skb_linearize(skb, GFP_ATOMIC) == 0)
#endif			/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
		{
			/* 
			   pull up the ip header */
			__skb_pull(skb, skb->h.raw - skb->data);
			sh = (struct sctphdr *) skb->h.raw;
			len = skb->len;
			/* 
			 *  perform the crc-32c checksum per RFC 2960 Appendix B.
			 */
			csum0 = sh->check;
			csum1 = ntohl(csum0);
			sh->check = 0;
			csum2 = crc32c(~0UL, sh, len);
			sh->check = csum0;
			if (csum1 == csum2) {
				/* 
				   pull to the ip header */
				__skb_push(skb, skb->data - skb->nh.raw);
				if ((mp = esballoc(skb->data, skb->len, BPRI_MED, &fr))) {
					ptrace(("Allocated mblk %p\n", mp));
					mp->b_datap->db_type = M_DATA;
					mp->b_wptr = mp->b_rptr + skb->len;
					/* 
					   trim the ip header */
					mp->b_rptr += skb->h.raw - skb->nh.raw;
					mp->b_rptr += sizeof(struct sctphdr);
					SCTPHASH_BH_RLOCK();
					sp = sctp_lookup(sh, skb->nh.iph->daddr,
							 skb->nh.iph->saddr);
					SCTPHASH_BH_RUNLOCK();
					if (sp) {
						if (canput(sp->rq)) {
							skb->dev = NULL;
							if (!putq(sp->rq, mp))
								__ctrace(freemsg(mp));	/* FIXME */
							return (0);
						}
						freemsg(mp);
						return (0);
					}
					return sctp_rcv_ootb(mp);
				} else
					ptrace(("Couldn't allocate mblk\n"));
			} else
				ptrace(("Bad checksum"));
		} else
			ptrace(("Couldn't linearize skb\n"));
	} else
		ptrace(("Receive non-HOST packet\n"));
	kfree_skb(skb);
	return (0);
}

/*
 *  =========================================================================
 *
 *  IP Protocol Registration
 *
 *  =========================================================================
 */
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP 132
#endif

#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
STATIC struct inet_protocol sctp_protocol = {
	sctp_rcv,			/* SCTP data handler */
	sctp_err,			/* SCTP error control */
	NULL,				/* next */
	IPPROTO_SCTP,			/* protocol ID */
	0,				/* copy */
	NULL,				/* data */
	"SCTP"				/* name */
};

void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol);
}

void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol);
}
#endif

#ifdef HAVE_NEW_STYLE_INET_PROTOCOL
STATIC struct inet_protocol sctp_protocol = {
	sctp_rcv,			/* SCTP data handler */
	sctp_err,			/* SCTP error control */
};

void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol, IPPROTO_SCTP);
}

void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol, IPPROTO_SCTP);
}
#endif
