/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.13 2007/06/17 01:55:26 brian Exp $

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

 Last Modified $Date: 2007/06/17 01:55:26 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp.h,v $
 Revision 0.9.2.13  2007/06/17 01:55:26  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __OS7_NET_SCTP_H__
#define __OS7_NET_SCTP_H__

#ident "@(#) $RCSfile: sctp.h,v $ $Name:  $($Revision: 0.9.2.13 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include <linux/autoconf.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <net/checksum.h>
#include <net/tcp.h>		/* for sequence number functions, bind bucket */
#include "linux/sctp.h"

#include <linux/compiler.h>

/*
 *  Kernel 2.4, 2.6 module compatibility.
 */
#ifndef EXPORT_SYMBOL_NOVERS
#define EXPORT_SYMBOL_NOVERS(__sym) EXPORT_SYMBOL(__sym)
#endif

/*
 *  First optimizations, then assertions.
 */

#if __GNUC__ < 3
#define __builtin_prefetch(addr, rw, level) ((void)(addr))
#endif

#if defined FASTCALL && !defined fastcall
#define fastcall FASTCALL()
#endif

#if !defined FASTCALL
#if defined __i386__ || defined __x86_64__ || defined __k8__
#define FASTCALL(__x) __x __attribute__((__regparm__(3)))
#else
#define FASTCALL(__x) __x
#endif
#define fastcall FASTCALL()
#endif

#if !defined fastcall
#define fastcall FASTCALL()
#endif

#if !defined noinline
#define noinline
#endif

#if !defined __always_inline
#define __always_inline inline
#endif

#undef sctp_fastcall
#if defined __i386__ || defined __x86_64__ || defined __k8__
#define sctp_fastcall __attribute__((__regparm__(3)))
#else
#define sctp_fastcall
#endif

#undef SCTP_FASTCALL
#define SCTP_FASTCALL(__x) __x sctp_fastcall

#ifndef likely
#define likely(__exp) (__exp)
#endif

#ifndef unlikely
#define unlikely(__exp) (__exp)
#endif

#undef sctp_noinline
#define sctp_noinline noinline

#if defined(_OPTIMIZE_NONE) || defined(_DEBUG)

#undef prefetchw
#define prefetchw(__a) ((void)(__a))

#undef prefetch
#define prefetch(__a) ((void)(__a))

#undef likely
#define likely(__exp) (__exp)

#undef unlikely
#define unlikely(__exp) (__exp)

#define __hot_put
#define __hot_write
#define __hot_out
#define __hot
#define __hot_in
#define __hot_read
#define __hot_get
#define __unlikely

#undef STATIC
#define STATIC

#undef INLINE
#define INLINE

#undef sctp_inline
#define sctp_inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern
#endif

#else
#ifdef _OPTIMIZE_SIZE

#undef prefetchw
#define prefetchw(__a) __builtin_prefetch((__a),1,3)

#undef prefetch
#define prefetch(__a) __builtin_prefetch((__a),0,3)

/* these don't affect size, just position */
#define __hot_put   __attribute__((section(".text.sctp.hot.put")))
#define __hot_write __attribute__((section(".text.sctp.hot.write")))
#define __hot_out   __attribute__((section(".text.sctp.hot.out")))
#define __hot       __attribute__((section(".text.sctp.hot")))
#define __hot_in    __attribute__((section(".text.sctp.hot.in")))
#define __hot_read  __attribute__((section(".text.sctp.hot.read")))
#define __hot_get   __attribute__((section(".text.sctp.hot.get")))
#define __unlikely  __attribute__((section(".text.sctp.unlikely")))

#undef STATIC
#define STATIC static

#undef INLINE
#define INLINE

#undef sctp_inline
#define sctp_inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern
#endif

#else
#ifdef _OPTIMIZE_SPEED

#undef prefetchw
#define prefetchw(__a) __builtin_prefetch((__a),1,3)

#undef prefetch
#define prefetch(__a) __builtin_prefetch((__a),0,3)

/* these don't affect size, just position */
#define __hot_put   __attribute__((section(".text.sctp.hot.put")))
#define __hot_write __attribute__((section(".text.sctp.hot.write")))
#define __hot_out   __attribute__((section(".text.sctp.hot.out")))
#define __hot       __attribute__((section(".text.sctp.hot")))
#define __hot_in    __attribute__((section(".text.sctp.hot.in")))
#define __hot_read  __attribute__((section(".text.sctp.hot.read")))
#define __hot_get   __attribute__((section(".text.sctp.hot.get")))
#define __unlikely  __attribute__((section(".text.sctp.unlikely")))

#undef STATIC
#define STATIC static

#undef INLINE
#define INLINE inline

#undef sctp_inline
#define sctp_inline inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE static inline
#endif

#else /* defined(_OPTIMIZE_NORMAL) */

#undef prefetchw
#define prefetchw(__a) __builtin_prefetch((__a),1,3)

#undef prefetch
#define prefetch(__a) __builtin_prefetch((__a),0,3)

/* these don't affect size, just position */
#define __hot_put   __attribute__((section(".text.sctp.hot.put")))
#define __hot_write __attribute__((section(".text.sctp.hot.write")))
#define __hot_out   __attribute__((section(".text.sctp.hot.out")))
#define __hot       __attribute__((section(".text.sctp.hot")))
#define __hot_in    __attribute__((section(".text.sctp.hot.in")))
#define __hot_read  __attribute__((section(".text.sctp.hot.read")))
#define __hot_get   __attribute__((section(".text.sctp.hot.get")))
#define __unlikely  __attribute__((section(".text.sctp.unlikely")))

#undef STATIC
#define STATIC static

#undef INLINE
#define INLINE inline

#undef sctp_inline
#define sctp_inline inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern inline
#endif

#endif
#endif
#endif

#undef  __never
#define __never() \
	do { panic("%s: never() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __rare
#define __rare() \
	do { printk(KERN_NOTICE "%s: rare() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __seldom
#define __seldom() \
	do { printk(KERN_NOTICE "%s: seldom() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __usual
#define __usual(__exp) \
	do { if (unlikely(!(__exp))) printk(KERN_WARNING "%s: usual(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); } while(0)

#undef  __normal
#define __normal(__exp) \
	do { if (unlikely(!(__exp))) printk(KERN_WARNING "%s: normal(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); } while(0)

#undef  __assert
#define __assert(__exp) \
	do { if (unlikely(!(__exp))) { printk(KERN_EMERG "%s: assert(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); *(int *)0 = 0; } } while(0)

#undef  __assure
#define __assure(__exp) \
	do { if (unlikely(!(__exp))) printk(KERN_WARNING "%s: assure(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); } while(0)

#undef  __ensure
#define __ensure(__exp,__sta) \
	do { if (unlikely(!(__exp))) { printk(KERN_WARNING "%s: ensure(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); __sta; } } while(0)

#undef  __unless
#define __unless(__exp,__sta) \
__ensure(!(__exp),__sta)

#undef  __trace
#define __trace() \
	do { printk(KERN_INFO "%s: trace() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __ptrace
#define __ptrace(__pkspec) \
	do { printk(KERN_INFO "%s: ptrace() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#undef  __fixme
#define __fixme(__pkspec) \
	do { printk(KERN_INFO "%s: fixme() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#undef  __todo
#define __todo(__pkspec) \
	do { printk(KERN_INFO "%s: todo() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#undef  __ctrace
#define __ctrace(__fnc) \
	({ printk(KERN_INFO "%s: calling " #__fnc " at " __FILE__ "+%d\n", __FUNCTION__, __LINE__); __fnc; })

#undef  __printd
#define __printd(__pkspec) \
	do { printk __pkspec; } while(0)

#undef  __swerr
#define __swerr() \
	do { printk(KERN_WARNING "%s: swerr() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __pswerr
#define __pswerr(__pkspec) \
do { printk(KERN_WARNING "%s: pswerr() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#define    _never()		do { } while(0)
#define     _rare()		do { } while(0)
#define   _seldom()		do { } while(0)
#define    _usual(__exp)	do { } while(0)
#define  _unusual(__exp)	do { } while(0)
#define   _normal(__exp)	do { } while(0)
#define _abnormal(__exp)	do { } while(0)
#define   _assert(__exp)	do { } while(0)
#define   _assure(__exp)	do { } while(0)
#define   _ensure(__exp,__sta)	do { if (unlikely(!(__exp))) { __sta; } } while(0)
#define   _unless(__exp,__sta)	do { if (unlikely( (__exp))) { __sta; } } while(0)
#define    _trace()		do { } while(0)
#define   _ptrace(__pks)	do { } while(0)
#define   _ctrace(__fnc)	(__fnc)
#define    _fixme(__pks)	do { } while(0)
#define     _todo(__pks)	do { } while(0)
#define   _printd(__pks)	do { } while(0)
#define    _swerr()		do { } while(0)
#define   _pswerr(__pks)	do { } while(0)

#ifdef _DEBUG

#define    never()		__never()
#define     rare()		__rare()
#define   seldom()		__seldom()
#define    usual(__exp)		__usual(__exp)
#define  unusual(__exp)		__usual(!(__exp))
#define   normal(__exp)		__normal(__exp)
#define abnormal(__exp)		__normal(!(__exp))
#define  dassert(__exp)		__assert(__exp)
#define   assert(__exp)		__assert(__exp)
#define   assure(__exp)		__assure(__exp)
#define   ensure(__exp,__sta)	__ensure(__exp,__sta)
#define   unless(__exp,__sta)	__ensure(!(__exp),__sta)
#define    trace()		__trace()
#define   ptrace(__pks)		__ptrace(__pks)
#define   ctrace(__fnc)		__ctrace(__fnc)
#define    fixme(__pks)		__fixme(__pks)
#define     todo(__pks)		__todo(__pks)
#define   printd(__pks)		__printd(__pks)
#define    swerr()		__swerr()
#define   pswerr(__pks)		__pswerr(__pks)

#else
#ifdef _TEST

#define    never()		__never()
#define     rare()		__rare()
#define   seldom()		__seldom()
#define    usual(__exp)		__usual(__exp)
#define  unusual(__exp)		__usual(!(__exp))
#define   normal(__exp)		__normal(__exp)
#define abnormal(__exp)		__normal(!(__exp))
#define  dassert(__exp)		__assert(__exp)
#define   assert(__exp)		__assert(__exp)
#define   assure(__exp)		__assure(__exp)
#define   ensure(__exp,__sta)	__ensure(__exp,__sta)
#define   unless(__exp,__sta)	__ensure(!(__exp),__sta)
#define    trace()		_trace()
#define   ptrace(__pks)		_ptrace(__pks)
#define   ctrace(__fnc)		_ctrace(__fnc)
#define    fixme(__pks)		__fixme(__pks)
#define     todo(__pks)		__todo(__pks)
#define   printd(__pks)		_printd(__pks)
#define    swerr()		__swerr()
#define   pswerr(__pks)		__pswerr(__pks)

#else
#ifdef _SAFE

#define    never()		do { *(int *)0 = 0; } while(0)
#define     rare()		_rare()
#define   seldom()		_seldom()
#define    usual(__exp)		_usual(__exp)
#define  unusual(__exp)		_unusual(__exp)
#define   normal(__exp)		_normal(__exp)
#define abnormal(__exp)		_abnormal(__exp)
#define  dassert(__exp)		_assert(__exp)
#define   assert(__exp)		{ if (unlikely(!(__exp))) *(int *)0 = 0; } while(0)
#define   assure(__exp)		__assure(__exp)
#define   ensure(__exp,__sta)	__ensure(__exp,__sta)
#define   unless(__exp,__sta)	__ensure(!(__exp),__sta)
#define    trace()		_trace()
#define   ptrace(__pks)		_ptrace(__pks)
#define   ctrace(__fnc)		_ctrace(__fnc)
#define    fixme(__pks)		__fixme(__pks)
#define     todo(__pks)		__todo(__pks)
#define   printd(__pks)		_printd(__pks)
#define    swerr()		__swerr()
#define   pswerr(__pks)		__pswerr(__pks)

#else

#define    never()		_never()
#define     rare()		_rare()
#define   seldom()		_seldom()
#define    usual(__exp)		_usual(__exp)
#define  unusual(__exp)		_unusual(__exp)
#define   normal(__exp)		_normal(__exp)
#define abnormal(__exp)		_abnormal(__exp)
#define  dassert(__exp)		_assert(__exp)
#define   assert(__exp)		_assert(__exp)
#define   assure(__exp)		_assure(__exp)
#define   ensure(__exp,__sta)	_ensure(__exp,__sta)
#define   unless(__exp,__sta)	_unless(__exp,__sta)
#define    trace()		_trace()
#define   ptrace(__pks)		_ptrace(__pks)
#define   ctrace(__fnc)		_ctrace(__fnc)
#define    fixme(__pks)		_fixme(__pks)
#define     todo(__pks)		_todo(__pks)
#define   printd(__pks)		_printd(__pks)
#define    swerr()		_swerr()
#define   pswerr(__pks)		_pswerr(__pks)

#endif
#endif
#endif

#ifdef CONFIG_SCTP_MODULE
#define __SCTP_STATIC STATIC
#define __SCTP_INIT
#else
#define __SCTP_STATIC
#define __SCTP_INIT __init
#endif

extern __inline__ void
__strace(struct sock *sk)
{
	char *sname = "(???)";

	if (sk)
		switch (sk->state) {
		case __OS7_SCTP_ESTABLISHED:
			sname = "ESTABLISHED";
			break;
		case __OS7_SCTP_COOKIE_WAIT:
			sname = "COOKIE-WAIT";
			break;
		case __OS7_SCTP_COOKIE_ECHOED:
			sname = "COOKIE-ECHOED";
			break;
		case __OS7_SCTP_SHUTDOWN_PENDING:
			sname = "SHUTDOWN-PENDING";
			break;
		case __OS7_SCTP_SHUTDOWN_SENT:
			sname = "SHUTDOWN-SENT";
			break;
		case __OS7_SCTP_UNREACHABLE:
			sname = "UNREACHABLE";
			break;
		case __OS7_SCTP_CLOSED:
			sname = "CLOSED";
			break;
		case __OS7_SCTP_SHUTDOWN_RECEIVED:
			sname = "SHUTDOWN-RECEIVED";
			break;
		case __OS7_SCTP_SHUTDOWN_RECVWAIT:
			sname = "SHUTDOWN-RECVWAIT";
			break;
		case __OS7_SCTP_LISTEN:
			sname = "LISTEN";
			break;
		case __OS7_SCTP_SHUTDOWN_ACK_SENT:
			sname = "SHUTDOWN-ACK-SENT";
			break;
		}
	printk(KERN_INFO "sk = %p, state = %s\n", sk, sname);
}

/*
 *  Kernel memory caches and hashes
 */
/* 
 *  Cache pointers
 *  -------------------------------------------------------------------------
 */
extern kmem_cachep_t __os7_sctp_dest_cachep;
extern kmem_cachep_t __os7_sctp_srce_cachep;
extern kmem_cachep_t __os7_sctp_bind_cachep;
extern kmem_cachep_t __os7_sctp_strm_cachep;

extern struct sock *__os7_sctp_protolist;
extern atomic_t __os7_sctp_orphan_count;
extern atomic_t __os7_sctp_socket_count;

/*
 *  Private struct macros
 */
#define __sp_offset ((unsigned int)&(((struct sock *)(0))->tp_pinfo))
#define SCTP_PROT(__sk) ((struct __os7_sctp_opt *)&(__sk)->tp_pinfo)
#define SCTP_SOCK(__sp) ((struct sock *)(((uint8_t *)(__sp)) - __sp_offset))

/*
 *  Capabilities
 */
#define __OS7_SCTP_CAPS_ECN		0x01	/* ECN capable */
#define __OS7_SCTP_CAPS_ADD_IP		0x02	/* ADD-IP capable */
#define __OS7_SCTP_CAPS_SET_IP		0x04	/* ADD-IP capable */
#define __OS7_SCTP_CAPS_ALI		0x08	/* Adaptation Layer Info capable */
#define __OS7_SCTP_CAPS_PR		0x10	/* PR-SCTP capable */

/*
 *  Destination flags.
 */
#define __OS7_SCTP_DESTF_INACTIVE	0x0001	/* DEST is inactive */
#define __OS7_SCTP_DESTF_HBACTIVE	0x0002	/* DEST heartbeats */
#define __OS7_SCTP_DESTF_TIMEDOUT	0x0004	/* DEST timeout occured */
#define __OS7_SCTP_DESTF_PMTUDISC	0x0008	/* DEST pmtu discovered */
#define __OS7_SCTP_DESTF_ROUTFAIL	0x0010	/* DEST routing failure */
#define __OS7_SCTP_DESTF_UNUSABLE	0x0020	/* DEST unusable */
#define __OS7_SCTP_DESTF_CONGESTD	0x0040	/* DEST congested */
#define __OS7_SCTP_DESTF_DROPPING	0x0080	/* DEST is dropping packets */
#define __OS7_SCTP_DESTF_FORWDTSN	0x0100	/* DEST has forward tsn outstanding */
#define __OS7_SCTP_DESTM_DONT_USE	(__OS7_SCTP_DESTF_INACTIVE| \
					 __OS7_SCTP_DESTF_UNUSABLE| \
					 __OS7_SCTP_DESTF_ROUTFAIL| \
					 __OS7_SCTP_DESTF_CONGESTD| \
					 __OS7_SCTP_DESTF_TIMEDOUT)
#define __OS7_SCTP_DESTM_CANT_USE	(__OS7_SCTP_DESTF_UNUSABLE| \
					 __OS7_SCTP_DESTF_ROUTFAIL)

/*
 * Source flags.
 */
#define __OS7_SCTP_SRCEF_ADD_REQUEST	0x0001	/* SRCE add requested */
#define __OS7_SCTP_SRCEF_DEL_REQUEST	0x0002	/* SRCE del requested */
#define __OS7_SCTP_SRCEF_SET_REQUEST	0x0004	/* SRCE set requested */
#define __OS7_SCTP_SRCEF_ADD_PENDING	0x0008	/* SRCE add pending */
#define __OS7_SCTP_SRCEF_DEL_PENDING	0x0010	/* SRCE del pending */
#define __OS7_SCTP_SRCEF_SET_PENDING	0x0020	/* SRCE set pending */
#define __OS7_SCTP_SRCEM_ASCONF		(__OS7_SCTP_SRCEF_ADD_REQUEST| \
					 __OS7_SCTP_SRCEF_DEL_REQUEST| \
					 __OS7_SCTP_SRCEF_ADD_PENDING| \
					 __OS7_SCTP_SRCEF_DEL_PENDING)

/*
 *  Stream flags.
 */
#define __OS7_SCTP_STRMF_MORE		0x00000001	/* STRM more data in (E)SDU */
#define __OS7_SCTP_STRMF_DROP		0x00008000	/* STRM dropping */

/*
 *  Sack flags
 */
#define __OS7_SCTP_SACKF_NEW		0x0001	/* SACK for new data RFC 2960 6.2 */
#define __OS7_SCTP_SACKF_WUP		0x0002	/* SACK for wakeup RFC 2960 6.2 */
#define __OS7_SCTP_SACKF_GAP		0x0004	/* SACK for lost data RFC 2960 7.2.4 */
#define __OS7_SCTP_SACKF_DUP		0x0008	/* SACK for duplic data RFC 2960 6.2 */
#define __OS7_SCTP_SACKF_URG		0x0010	/* SACK for urgent data RFC 2960 ??? */
#define __OS7_SCTP_SACKF_WND		0x0020	/* SACK for new a_rwnd RFC 2960 6.2 */
#define __OS7_SCTP_SACKF_TIM		0x0040	/* SACK for timeout RFC 2960 6.2 */
#define __OS7_SCTP_SACKF_NOD		0x0080	/* SACK no delay RFC 2960 9.2 */
#define __OS7_SCTP_SACKF_ECN		0x0100	/* SACK for ECN RFC 2960 Adx A */
#define __OS7_SCTP_SACKF_FSN		0x0200	/* SACK for FORWARD TSN PR-SCTP 3.5 F2 */
#define __OS7_SCTP_SACKF_CWR		0x1000	/* SACK for CWR RFC 2960 Adx A */
#define __OS7_SCTP_SACKF_ASC		0x2000	/* SACK for ASCONF */
#define __OS7_SCTP_SACKF_NOW		0x01fe	/* SACK forced mask */
#define __OS7_SCTP_SACKF_ANY		0x01ff	/* SACK for anyting */
/*
 *  SCTP private flags
 */
#define __OS7_SCTP_FLAG_REC_CONF_OPT	0x00000001
#define __OS7_SCTP_FLAG_EX_DATA_OPT	0x00000002
#define __OS7_SCTP_FLAG_DEFAULT_RC_SEL	0x00000004
#define __OS7_SCTP_FLAG_NEED_CLEANUP	0x00000008
/*
 *  SCTP cmsg flags
 */
#define __OS7_SCTP_CMSGF_RECVSID  0x01
#define __OS7_SCTP_CMSGF_RECVPPI  0x02
#define __OS7_SCTP_CMSGF_RECVSSN  0x04
#define __OS7_SCTP_CMSGF_RECVTSN  0x08

/*
 *  This is what the send packet queueing engine uses to pass SCTP per-packet
 *  control information to the transmission code.  The receive packet queueing
 *  engine also uses this.  If this grows, please adjust
 *  skbuff.h:skbuff->cb[xxx] size appropriately.  (We currently have 48 bytes
 *  to work with.)
 *
 *  We are bang on the 48 byte limit.
 *
 */
struct __os7_sctp_skb_cb {
	struct __os7_sctp_skb_cb *next;	/* message linkage */
	struct __os7_sctp_skb_cb *tail;	/* message linkage */

	struct sk_buff *skb;		/* message linkage */

	__u32 tsn;			/* why do I need these?, they are in the chunk header */
	__u16 sid;
	__u16 ssn;
	__u32 ppi;

	__u16 dlen;			/* data length */
	__u16 flags;			/* general flags inc. data chunk flags */
	__u16 sacks;			/* number of times gap acked */
	__u16 trans;			/* number of times retransmitted */

	unsigned long expires;		/* when this packet expires */
	unsigned long when;		/* tim of tx/rx/ack */

	struct __os7_sctp_daddr *daddr;	/* daddr tx to or rx from */
	struct __os7_sctp_strm *st;	/* strm tx to or rx from */
};

/*
 *  Message control block flags
 */
#define __OS7_SCTPCB_FLAG_LAST_FRAG	0x0001	/* aligned with SCTP DATA chunk flags */
#define __OS7_SCTPCB_FLAG_FIRST_FRAG	0x0002	/* aligned with SCTP DATA chunk flags */
#define __OS7_SCTPCB_FLAG_URG		0x0004	/* aligned with SCTP DATA chunk flags */
#define __OS7_SCTPCB_FLAG_CKSUMMED	0x0100
#define __OS7_SCTPCB_FLAG_DELIV		0x0200	/* delivered on receive */
#define __OS7_SCTPCB_FLAG_DROPPED	0x0200	/* also dropped on transmit */
#define __OS7_SCTPCB_FLAG_ACK		0x0400
#define __OS7_SCTPCB_FLAG_NACK		0x0800
#define __OS7_SCTPCB_FLAG_CONF		0x1000
#define __OS7_SCTPCB_FLAG_SENT		0x2000
#define __OS7_SCTPCB_FLAG_RETRANS	0x4000
#define __OS7_SCTPCB_FLAG_SACKED	0x8000

#define __cb_offset ((unsigned int)&(((struct sk_buff *)(NULL))->cb[0]))
#define SCTP_SKB_CB(__skb) ((struct __os7_sctp_skb_cb *)(&(__skb)->cb[0]))
#define SCTP_CB_SKB(__cb) ((struct sk_buff *)(((uint8_t *)(__cb)) - __cb_offset))

#define PADC(__len) (((__len)+3)&~0x3)
#define SCTP_PADC(__len) (((__len)+3)&~0x3)

/*
 *  These are external declarations for major SCTP protocol operations.
 */
extern unsigned int __os7_sctp_poll(struct file *file, struct socket *sock, poll_table * wait);
extern int __os7_sctp_listen(struct socket *sock, int backlog);

extern struct proto *__os7_sctp_prot_hook;
extern struct __os7_sctp_mib __os7_sctp_statistics[NR_CPUS * 2];

#define SCTP_INC_STATS(field)		SNMP_INC_STATS(__os7_sctp_statistics, field)
#define SCTP_INC_STATS_BH(field)	SNMP_INC_STATS_BH(__os7_sctp_statistics, field)
#define SCTP_INC_STATS_USER(field)	SNMP_INC_STATS_USER(__os7_sctp_statistics, field)

/*
 *  Some sysctls
 */
extern int sysctl_sctp_max_istreams;
extern int sysctl_sctp_req_ostreams;
extern int sysctl_sctp_rto_initial;
extern int sysctl_sctp_rto_min;
extern int sysctl_sctp_rto_max;
extern int sysctl_sctp_heartbeat_itvl;
extern int sysctl_sctp_max_init_retries;
extern int sysctl_sctp_valid_cookie_life;
extern int sysctl_sctp_max_sack_delay;
extern int sysctl_sctp_path_max_retrans;
extern int sysctl_sctp_assoc_max_retrans;
extern int sysctl_sctp_mac_type;
extern int sysctl_sctp_csum_type;
extern int sysctl_sctp_cookie_inc;
extern int sysctl_sctp_throttle_itvl;
extern int sysctl_sctp_mem[3];
extern int sysctl_sctp_rmem[3];
extern int sysctl_sctp_wmem[3];
extern int sysctl_sctp_ecn;
extern int sysctl_sctp_adaptation_layer_info;
extern int sysctl_sctp_partial_reliability;
extern int sysctl_sctp_max_burst;

extern int min_sctp_max_istreams;
extern int min_sctp_req_ostreams;
extern int min_sctp_max_burst;

/*
 *  Socket options
 */
#define __OS7_SCTP_NODELAY		TCP_NODELAY
#define __OS7_SCTP_MAXSEG		TCP_MAXSEG
#define __OS7_SCTP_CORK			TCP_CORK
#define __OS7_SCTP_RECVSID		4
#define __OS7_SCTP_RECVPPI		5
#define __OS7_SCTP_RECVSSN		6
#define __OS7_SCTP_RECVTSN		7
#define __OS7_SCTP_SID			8
#define __OS7_SCTP_PPI			9
#define __OS7_SCTP_SSN			10
#define __OS7_SCTP_TSN			11
#define __OS7_SCTP_HB			12
#define __OS7_SCTP_RTO			13
#define __OS7_SCTP_COOKIE_LIFE		14
#define __OS7_SCTP_SACK_DELAY		15
#define __OS7_SCTP_PATH_MAX_RETRANS	16
#define __OS7_SCTP_ASSOC_MAX_RETRANS	17
#define __OS7_SCTP_MAX_INIT_RETRIES	18
#define __OS7_SCTP_HEARTBEAT_ITVL	19
#define __OS7_SCTP_RTO_INITIAL		20
#define __OS7_SCTP_RTO_MIN		21
#define __OS7_SCTP_RTO_MAX		22
#define __OS7_SCTP_OSTREAMS		23
#define __OS7_SCTP_ISTREAMS		24
#define __OS7_SCTP_COOKIE_INC		25
#define __OS7_SCTP_THROTTLE_ITVL	26
#define __OS7_SCTP_MAC_TYPE		27
#define __OS7_SCTP_CKSUM_TYPE		28
#define __OS7_SCTP_DEBUG_OPTIONS	29
#define __OS7_SCTP_STATUS		30
#define __OS7_SCTP_ALI			31
#define __OS7_SCTP_PR			32
#define __OS7_SCTP_DISPOSITION		33
#define __OS7_SCTP_LIFETIME		34
#define __OS7_SCTP_ADD			35
#define __OS7_SCTP_ADD_IP		36
#define __OS7_SCTP_DEL_IP		37
#define __OS7_SCTP_SET			38
#define __OS7_SCTP_SET_IP		39
#define __OS7_SCTP_ECN			40
#define __OS7_SCTP_MAX_BURST		41

/*
 *  User __OS7_SCTP_HB socket option structure
 */
struct __os7_sctp_hbitvl {
	struct sockaddr_in
	 dest;				/* destination IP address */
	uint hb_act;			/* activation flag */
	uint hb_itvl;			/* interval in milliseconds */
};

/*
 *  User __OS7_SCTP_RTO socket option structure
 */
struct __os7_sctp_rtoval {
	struct sockaddr_in
	 dest;				/* destination IP address */
	uint rto_initial;		/* RTO.Initial (milliseconds) */
	uint rto_min;			/* RTO.Min (milliseconds) */
	uint rto_max;			/* RTO.Max (milliseconds) */
	uint max_retrans;		/* Path.Max.Retrans (retires) */
};

/*
 *  User __OS7_SCTP_STATUS socket option structure
 */
struct __os7_sctp_dstat {
	struct sockaddr_in
	 dest;				/* destination IP address */
	uint dst_cwnd;			/* congestion window */
	uint dst_unack;			/* unacknowledged chunks */
	uint dst_srtt;			/* smoothed round trip time */
	uint dst_rvar;			/* rtt variance */
	uint dst_rto;			/* current rto */
	uint dst_sst;			/* slow start threshold */
};
struct __os7_sctp_astat {
	uint assoc_rwnd;		/* receive window */
	uint assoc_rbuf;		/* receive buffer */
	uint assoc_nrep;		/* destinations reported */
};

#define __OS7_SCTP_HMAC_NONE		0	/* no hmac (all one's) */
#define __OS7_SCTP_HMAC_SHA_1		1	/* SHA-1 coded hmac */
#define __OS7_SCTP_HMAC_MD5		2	/* MD5 coded hmac */

#define __OS7_SCTP_CSUM_ADLER32		0	/* Adler32 checksum output */
#define __OS7_SCTP_CSUM_CRC32C		1	/* CRC-32c checksum output */

#define __OS7_SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define __OS7_SCTP_OPTION_BREAK		0x02	/* stream will break dest #1 and 2 one way */
#define __OS7_SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define __OS7_SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */

#define __OS7_SCTP_PR_NONE		0x0	/* no partial reliability */
#define __OS7_SCTP_PR_PREFERRED		0x1	/* partial reliability preferred */
#define __OS7_SCTP_PR_REQUIRED		0x2	/* partial reliability required */

#define __OS7_SCTP_DISPOSITION_NONE		0x0
#define __OS7_SCTP_DISPOSITION_UNSENT		0x1
#define __OS7_SCTP_DISPOSITION_SENT		0x2
#define __OS7_SCTP_DISPOSITION_GAP_ACKED	0x3
#define __OS7_SCTP_DISPOSITION_ACKED		0x4

#if defined CONFIG_SCTP && !defined CONFIG_SCTP_MODULE
int __os7_sctp_v4_rcv(struct sk_buff *skb);
void __os7_sctp_v4_err(struct sk_buff *skb, uint32_t info);
extern struct proto __os7_sctp_prot;
#endif

#endif				/* __OS7_NET_SCTP_H__ */
