/*****************************************************************************

 @(#) $RCSfile: inet.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2006/07/07 21:14:54 $

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

 Last Modified $Date: 2006/07/07 21:14:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: inet.c,v $
 Revision 0.9.2.74  2006/07/07 21:14:54  brian
 - correct compile back to RH 7.2

 Revision 0.9.2.73  2006/06/14 10:37:34  brian
 - defeat a lot of debug traces in debug mode for testing
 - changes to allow strinet to compile under LiS (why???)

 Revision 0.9.2.72  2006/06/07 09:54:36  brian
 - minor non-service affecting bug found by inspection

 Revision 0.9.2.71  2006/05/23 10:40:11  brian
 - handle non-exported sysctl_ip_default_ttl on receive FC4 kernels

 Revision 0.9.2.70  2006/05/19 08:49:38  brian
 - working up RAWIP and UDP drivers and testing

 Revision 0.9.2.69  2006/05/14 06:58:17  brian
 - removed redundant or unused QR_ definitions

 Revision 0.9.2.68  2006/04/22 01:09:26  brian
 - locking correction

 Revision 0.9.2.67  2006/04/04 04:16:17  brian
 - handling of __tcp_push_pending_frames on 2.6.13+ kernels

 Revision 0.9.2.66  2006/04/03 22:32:59  brian
 - must freeze before strqset

 Revision 0.9.2.65  2006/04/03 10:58:09  brian
 - 2.6.13 kernel has request_sock but no inet_connection_sock

 Revision 0.9.2.64  2006/03/31 12:43:31  brian
 - stab at ss_sock_sendmsg corrections

 Revision 0.9.2.63  2006/03/31 00:36:43  brian
 - fixed buffer leak reported by John Wenker

 Revision 0.9.2.62  2006/03/18 00:15:13  brian
 - syncing notebook

 Revision 0.9.2.61  2006/03/10 19:47:43  brian
 - reformatting

 Revision 0.9.2.60  2006/02/23 11:08:51  brian
 - a couple of bug corrections reported by sponsors
 - 64bit changes
 - validated on x86_64 SMP kernel

 *****************************************************************************/

#ident "@(#) $RCSfile: inet.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2006/07/07 21:14:54 $"

static char const ident[] =
    "$RCSfile: inet.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2006/07/07 21:14:54 $";

/*
   This driver provides the functionality of IP (Internet Protocol) over a connectionless network
   provider (NPI).  It provides a STREAMS-based encapsulation of the Linux IP stack. 
 */

#include <sys/os7/compat.h>

#if defined HAVE_OPENSS7_SCTP
#if !defined CONFIG_SCTP && !defined CONFIG_SCTP_MODULE
#undef HAVE_OPENSS7_SCTP
#endif
#endif

#if !defined HAVE_OPENSS7_SCTP
#undef sctp_addr
#define sctp_addr stupid_sctp_addr_in_the_wrong_place
#endif

#include <linux/bitops.h>

#define t_tst_bit(nr,addr)	test_bit(nr,addr)
#define t_set_bit(nr,addr)	__set_bit(nr,addr)
#define t_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/net.h>
#include <linux/in.h>
#include <linux/un.h>
#include <linux/ip.h>

#undef ASSERT

#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#if defined HAVE_OPENSS7_SCTP
#undef STATIC
#undef INLINE
#include <net/sctp.h>
#endif

#if 0
/* Turn on some tracing and debugging. */
#undef ensure
#undef assure
#undef assert
#undef printd
#undef swerr
#undef rare
#undef seldom
#undef likely

#define ensure __ensure
#define assure __assure
#define assert __assert
#define printd __printd
#define swerr __swerr
#define rare __rare
#define seldom __seldom
#define _DEBUG
#endif

/* Compatibility functions between 2.4 and 2.6. */

#ifdef HAVE_OLD_SOCK_STRUCTURE

#define sk_callback_lock	callback_lock
#define sk_user_data		user_data
#define sk_state_change		state_change
#define sk_data_ready		data_ready
#define sk_write_space		write_space
#define sk_error_report		error_report
#define sk_lingertime		lingertime
#define sk_state		state
#define sk_rcvbuf		rcvbuf
#define sk_rcvlowat		rcvlowat
#define sk_sndbuf		sndbuf
#define sk_reuse		reuse
#define sk_localroute		localroute
#define sk_no_check		no_check
#define sk_debug		debug
#define sk_allocation		allocation
#define sk_priority		priority
#define sk_ack_backlog		ack_backlog
#define sk_prot			prot
#define sk_receive_queue	receive_queue
#define sk_rmem_alloc		rmem_alloc

#define sock_tst_dead(_sk)		(((struct sock *)_sk)->dead ? 1 : 0)
#define sock_tst_done(_sk)		(((struct sock *)_sk)->done ? 1 : 0)
#define sock_tst_urginline(_sk)		(((struct sock *)_sk)->urginline ? 1 : 0)
#define sock_tst_keepopen(_sk)		(((struct sock *)_sk)->keepopen ? 1 : 0)
#define sock_tst_linger(_sk)		(((struct sock *)_sk)->linger ? 1 : 0)
#define sock_tst_destroy(_sk)		(((struct sock *)_sk)->destroy ? 1 : 0)
#define sock_tst_broadcast(_sk)		(((struct sock *)_sk)->broadcast ? 1 : 0)
#define sock_tst_localroute(_sk)	(((struct sock *)_sk)->localroute ? 1 : 0)
#define sock_tst_debug(_sk)		(((struct sock *)_sk)->debug ? 1 : 0)

#define sock_set_dead(_sk)		(((struct sock *)_sk)->dead = 1)
#define sock_set_done(_sk)		(((struct sock *)_sk)->done = 1)
#define sock_set_urginline(_sk)		(((struct sock *)_sk)->urginline = 1)
#define sock_set_keepopen(_sk)		(((struct sock *)_sk)->keepopen = 1)
#define sock_set_linger(_sk)		(((struct sock *)_sk)->linger = 1)
#define sock_set_destroy(_sk)		(((struct sock *)_sk)->destroy = 1)
#define sock_set_broadcast(_sk)		(((struct sock *)_sk)->broadcast = 1)
#define sock_set_localroute(_sk)	(((struct sock *)_sk)->localroute = 1)
#define sock_set_debug(_sk)		(((struct sock *)_sk)->debug = 1)

#define sock_clr_dead(_sk)		(((struct sock *)_sk)->dead = 0)
#define sock_clr_done(_sk)		(((struct sock *)_sk)->done = 0)
#define sock_clr_urginline(_sk)		(((struct sock *)_sk)->urginline = 0)
#define sock_clr_keepopen(_sk)		(((struct sock *)_sk)->keepopen = 0)
#define sock_clr_linger(_sk)		(((struct sock *)_sk)->linger = 0)
#define sock_clr_destroy(_sk)		(((struct sock *)_sk)->destroy = 0)
#define sock_clr_broadcast(_sk)		(((struct sock *)_sk)->broadcast = 0)
#define sock_clr_localroute(_sk)	(((struct sock *)_sk)->localroute = 0)
#define sock_clr_debug(_sk)		(((struct sock *)_sk)->debug = 0)

#undef inet_sk
#define inet_sk(_sk)			(&(((struct sock *)_sk)->protinfo.af_inet))
#undef inet_csk
#define inet_csk(_sk)			(&(((struct sock *)_sk)->protinfo.af_inet))
#undef tcp_sk
#define tcp_sk(_sk)			(&(((struct sock *)_sk)->tp_pinfo.af_tcp))
#undef sctp_sk
#define sctp_sk(_sk)			(&(((struct sock *)_sk)->tp_pinfo.af_sctp))

#define sock_saddr(_sk)			(((struct sock *)_sk)->saddr)
#define sock_sport(_sk)			(((struct sock *)_sk)->sport)
#define sock_daddr(_sk)			(((struct sock *)_sk)->daddr)
#define sock_dport(_sk)			(((struct sock *)_sk)->dport)

#define tcp_user_mss(_tp)		((_tp)->user_mss)

#define sock_syn_retries(_sk)		(tcp_sk(_sk)->syn_retries)
#define sock_defer_accept(_sk)		(tcp_sk(_sk)->defer_accept)
#define sock_accept_queue_head(_sk)	(tcp_sk(_sk)->accept_queue)
#define sock_accept_queue_tail(_sk)	(tcp_sk(_sk)->accept_queue_tail)
#define sock_accept_queue_lock(_sk)
#define sock_accept_queue_unlock(_sk)

#else
#ifdef HAVE_TRN_SOCK_STRUCTURE

#define sock_tst_dead(_sk)		(sock_flag(_sk, SOCK_DEAD) ? 1 : 0)
#define sock_tst_done(_sk)		(sock_flag(_sk, SOCK_DONE) ? 1 : 0)
#define sock_tst_urginline(_sk)		(sock_flag(_sk, SOCK_URGINLINE) ? 1 : 0)
#define sock_tst_keepopen(_sk)		(sock_flag(_sk, SOCK_KEEPOPEN) ? 1 : 0)
#define sock_tst_linger(_sk)		(sock_flag(_sk, SOCK_LINGER) ? 1 : 0)
#define sock_tst_destroy(_sk)		(sock_flag(_sk, SOCK_DESTROY) ? 1 : 0)
#define sock_tst_broadcast(_sk)		(sock_flag(_sk, SOCK_BROADCAST) ? 1 : 0)
#define sock_tst_localroute(_sk)	(((struct sock *)_sk)->sk_localroute ? 1 : 0)
#define sock_tst_debug(_sk)		(((struct sock *)_sk)->sk_debug ? 1 : 0)

#define sock_set_dead(_sk)		(sock_set_flag(_sk, SOCK_DEAD))
#define sock_set_done(_sk)		(sock_set_flag(_sk, SOCK_DONE))
#define sock_set_urginline(_sk)		(sock_set_flag(_sk, SOCK_URGINLINE))
#define sock_set_keepopen(_sk)		(sock_set_flag(_sk, SOCK_KEEPOPEN))
#define sock_set_linger(_sk)		(sock_set_flag(_sk, SOCK_LINGER))
#define sock_set_destroy(_sk)		(sock_set_flag(_sk, SOCK_DESTROY))
#define sock_set_broadcast(_sk)		(sock_set_flag(_sk, SOCK_BROADCAST))
#define sock_set_localroute(_sk)	(((struct sock *)_sk)->sk_localroute = 1)
#define sock_set_debug(_sk)		(((struct sock *)_sk)->sk_debug = 1)

#define sock_clr_dead(_sk)		(sock_reset_flag(_sk, SOCK_DEAD))
#define sock_clr_done(_sk)		(sock_reset_flag(_sk, SOCK_DONE))
#define sock_clr_urginline(_sk)		(sock_reset_flag(_sk, SOCK_URGINLINE))
#define sock_clr_keepopen(_sk)		(sock_reset_flag(_sk, SOCK_KEEPOPEN))
#define sock_clr_linger(_sk)		(sock_reset_flag(_sk, SOCK_LINGER))
#define sock_clr_destroy(_sk)		(sock_reset_flag(_sk, SOCK_DESTROY))
#define sock_clr_broadcast(_sk)		(sock_reset_flag(_sk, SOCK_BROADCAST))
#define sock_clr_localroute(_sk)	(((struct sock *)_sk)->sk_localroute = 0)
#define sock_clr_debug(_sk)		(((struct sock *)_sk)->sk_debug = 0)

#define sock_saddr(_sk)			(inet_sk(_sk)->saddr)
#define sock_sport(_sk)			(inet_sk(_sk)->sport)
#define sock_daddr(_sk)			(inet_sk(_sk)->daddr)
#define sock_dport(_sk)			(inet_sk(_sk)->dport)

#define tcp_user_mss(_tp)		((_tp)->user_mss)

#define sock_syn_retries(_sk)		(tcp_sk(_sk)->syn_retries)
#define sock_defer_accept(_sk)		(tcp_sk(_sk)->defer_accept)
#define sock_accept_queue_head(_sk)	(tcp_sk(_sk)->accept_queue)
#define sock_accept_queue_tail(_sk)	(tcp_sk(_sk)->accept_queue_tail)
#define sock_accept_queue_lock(_sk)
#define sock_accept_queue_unlock(_sk)

#else
#ifdef HAVE_NEW_SOCK_STRUCTURE

#define sock_tst_dead(_sk)		(sock_flag(_sk, SOCK_DEAD) ? 1 : 0)
#define sock_tst_done(_sk)		(sock_flag(_sk, SOCK_DONE) ? 1 : 0)
#define sock_tst_urginline(_sk)		(sock_flag(_sk, SOCK_URGINLINE) ? 1 : 0)
#define sock_tst_keepopen(_sk)		(sock_flag(_sk, SOCK_KEEPOPEN) ? 1 : 0)
#define sock_tst_linger(_sk)		(sock_flag(_sk, SOCK_LINGER) ? 1 : 0)
#define sock_tst_destroy(_sk)		(sock_flag(_sk, SOCK_DESTROY) ? 1 : 0)
#define sock_tst_broadcast(_sk)		(sock_flag(_sk, SOCK_BROADCAST) ? 1 : 0)
#ifdef HAVE_KMEMB_STRUCT_SOCK_SK_LOCALROUTE
#define sock_tst_localroute(_sk)	(((struct sock *)_sk)->sk_localroute ? 1 : 0)
#else
#define sock_tst_localroute(_sk)	(sock_flag(_sk, SOCK_LOCALROUTE) ? 1 : 0)
#endif
#ifdef HAVE_KMEMB_STRUCT_SOCK_SK_DEBUG
#define sock_tst_debug(_sk)		(((struct sock *)_sk)->sk_debug ? 1 : 0)
#else
#define sock_tst_debug(_sk)		(sock_flag(_sk, SOCK_DBG) ? 1 : 0)
#endif

#define sock_set_dead(_sk)		(sock_set_flag(_sk, SOCK_DEAD))
#define sock_set_done(_sk)		(sock_set_flag(_sk, SOCK_DONE))
#define sock_set_urginline(_sk)		(sock_set_flag(_sk, SOCK_URGINLINE))
#define sock_set_keepopen(_sk)		(sock_set_flag(_sk, SOCK_KEEPOPEN))
#define sock_set_linger(_sk)		(sock_set_flag(_sk, SOCK_LINGER))
#define sock_set_destroy(_sk)		(sock_set_flag(_sk, SOCK_DESTROY))
#define sock_set_broadcast(_sk)		(sock_set_flag(_sk, SOCK_BROADCAST))
#ifdef HAVE_KMEMB_STRUCT_SOCK_SK_LOCALROUTE
#define sock_set_localroute(_sk)	(((struct sock *)_sk)->sk_localroute = 1)
#else
#define sock_set_localroute(_sk)	(sock_set_flag(_sk, SOCK_LOCALROUTE))
#endif
#ifdef HAVE_KMEMB_STRUCT_SOCK_SK_DEBUG
#define sock_set_debug(_sk)		(((struct sock *)_sk)->sk_debug = 1)
#else
#define sock_set_debug(_sk)		(sock_set_flag(_sk, SOCK_DBG))
#endif

#define sock_clr_dead(_sk)		(sock_reset_flag(_sk, SOCK_DEAD))
#define sock_clr_done(_sk)		(sock_reset_flag(_sk, SOCK_DONE))
#define sock_clr_urginline(_sk)		(sock_reset_flag(_sk, SOCK_URGINLINE))
#define sock_clr_keepopen(_sk)		(sock_reset_flag(_sk, SOCK_KEEPOPEN))
#define sock_clr_linger(_sk)		(sock_reset_flag(_sk, SOCK_LINGER))
#define sock_clr_destroy(_sk)		(sock_reset_flag(_sk, SOCK_DESTROY))
#define sock_clr_broadcast(_sk)		(sock_reset_flag(_sk, SOCK_BROADCAST))
#ifdef HAVE_KMEMB_STRUCT_SOCK_SK_LOCALROUTE
#define sock_clr_localroute(_sk)	(((struct sock *)_sk)->sk_localroute = 0)
#else
#define sock_clr_localroute(_sk)	(sock_reset_flag(_sk, SOCK_LOCALROUTE))
#endif
#ifdef HAVE_KMEMB_STRUCT_SOCK_SK_DEBUG
#define sock_clr_debug(_sk)		(((struct sock *)_sk)->sk_debug = 0)
#else
#define sock_clr_debug(_sk)		(sock_reset_flag(_sk, SOCK_DBG))
#endif

#define sock_saddr(_sk)			(inet_sk(_sk)->saddr)
#define sock_sport(_sk)			(inet_sk(_sk)->sport)
#define sock_daddr(_sk)			(inet_sk(_sk)->daddr)
#define sock_dport(_sk)			(inet_sk(_sk)->dport)

#define inet_opt	inet_sock
#define tcp_opt		tcp_sock

#define tcp_user_mss(_tp)		((_tp)->rx_opt.user_mss)

#ifndef HAVE_KFUNC_INET_CSK
#define sock_syn_retries(_sk)		(tcp_sk(_sk)->syn_retries)
#ifndef HAVE_KINC_NET_REQUEST_SOCK_H
#define sock_defer_accept(_sk)		(tcp_sk(_sk)->defer_accept)
#define sock_accept_queue_head(_sk)	(tcp_sk(_sk)->accept_queue)
#define sock_accept_queue_tail(_sk)	(tcp_sk(_sk)->accept_queue_tail)
#define sock_accept_queue_lock(_sk)
#define sock_accept_queue_unlock(_sk)
#else
#define sock_defer_accept(_sk)		(tcp_sk(_sk)->defer_accept)
#define sock_accept_queue_head(_sk)	(tcp_sk(_sk)->accept_queue.rskq_accept_head)
#define sock_accept_queue_tail(_sk)	(tcp_sk(_sk)->accept_queue.rskq_accept_tail)
#define sock_accept_queue_lock(_sk)	write_lock_bh(&tcp_sk(_sk)->accept_queue.syn_wait_lock)
#define sock_accept_queue_unlock(_sk)	write_unlock_bh(&tcp_sk(_sk)->accept_queue.syn_wait_lock)
#define open_request request_sock
#define tcp_openreq_fastfree(__req)	__reqsk_free(__req)
#endif
#else
#define sock_syn_retries(_sk)		(inet_csk(_sk)->icsk_syn_retries)
#define sock_defer_accept(_sk)		(inet_csk(_sk)->icsk_accept_queue.rskq_defer_accept)
#define sock_accept_queue_head(_sk)	(inet_csk(_sk)->icsk_accept_queue.rskq_accept_head)
#define sock_accept_queue_tail(_sk)	(inet_csk(_sk)->icsk_accept_queue.rskq_accept_tail)
#define sock_accept_queue_lock(_sk)	write_lock_bh(&inet_csk(_sk)->icsk_accept_queue.syn_wait_lock)
#define sock_accept_queue_unlock(_sk)	write_unlock_bh(&inet_csk(_sk)->icsk_accept_queue.syn_wait_lock)
#define open_request request_sock
#define tcp_openreq_fastfree(__req)	__reqsk_free(__req)
#endif

#else
#error One of HAVE_OLD_SOCK_STRUCTURE, HAVE_TRN_SOCK_STRUCTURE or HAVE_NEW_SOCK_STRUCTURE must be defined.
#endif
#endif
#endif

#ifndef SK_WMEM_MAX
#define SK_WMEM_MAX 65535
#endif
#ifndef SK_RMEM_MAX
#define SK_RMEM_MAX 65535
#endif

#if !defined HAVE_OPENSS7_SCTP
#undef sctp_addr
#endif

#ifndef tcp_openreq_cachep
#ifdef HAVE_TCP_OPENREQ_CACHEP_ADDR
#include <linux/slab.h>
static kmem_cache_t **const _tcp_openreq_cachep_location =
    (typeof(_tcp_openreq_cachep_location)) (HAVE_TCP_OPENREQ_CACHEP_ADDR);

#define tcp_openreq_cachep (*_tcp_openreq_cachep_location)
#endif
#endif

#ifndef tcp_set_keepalive
#ifdef HAVE_TCP_SET_KEEPALIVE_ADDR
void
tcp_set_keepalive(struct sock *sk, int val)
{
	static void (*func) (struct sock *, int) = (typeof(func)) HAVE_TCP_SET_KEEPALIVE_ADDR;

	return func(sk, val);
}
#endif
#endif

#ifndef tcp_sync_mss
#ifdef HAVE_TCP_SYNC_MSS_ADDR
int
tcp_sync_mss(struct sock *sk, u32 pmtu)
{
	static int (*func) (struct sock *, u32) = (typeof(func)) HAVE_TCP_SYNC_MSS_ADDR;

	return func(sk, pmtu);
}
#endif
#endif

#ifndef tcp_write_xmit
#ifdef HAVE_TCP_WRITE_XMIT_ADDR
int
tcp_write_xmit(struct sock *sk, int nonagle)
{
	static int (*func) (struct sock *, int) = (typeof(func)) HAVE_TCP_WRITE_XMIT_ADDR;

	return func(sk, nonagle);
}
#endif
#endif

#ifndef tcp_cwnd_application_limited
#ifdef HAVE_TCP_CWND_APPLICATION_LIMITED_ADDR
void
tcp_cwnd_application_limited(struct sock *sk)
{
	static void (*func) (struct sock *) = (typeof(func)) HAVE_TCP_CWND_APPLICATION_LIMITED_ADDR;

	return func(sk);
}
#endif
#endif

/*
   recreate this structure because it is used by an inline 
 */
__u8 ip_tos2prio[16] = { 0, 1, 0, 0, 2, 2, 2, 2, 6, 6, 6, 6, 4, 4, 4, 4 };

#ifndef sysctl_rmem_default
#ifdef HAVE_SYSCTL_RMEM_DEFAULT_ADDR
static __u32 *const _sysctl_rmem_default_location =
    (typeof(_sysctl_rmem_default_location)) (HAVE_SYSCTL_RMEM_DEFAULT_ADDR);

#define sysctl_rmem_default (*_sysctl_rmem_default_location)
#else
#define sysctl_rmem_default SK_RMEM_MAX
#endif
#endif

#ifndef sysctl_wmem_default
#ifdef HAVE_SYSCTL_WMEM_DEFAULT_ADDR
static __u32 *const _sysctl_wmem_default_location =
    (typeof(_sysctl_wmem_default_location)) (HAVE_SYSCTL_WMEM_DEFAULT_ADDR);

#define sysctl_wmem_default (*_sysctl_wmem_default_location)
#else
#define sysctl_wmem_default SK_WMEM_MAX
#endif
#endif

#ifndef sysctl_tcp_fin_timeout
#ifdef HAVE_SYSCTL_TCP_FIN_TIMEOUT_ADDR
static __u32 *const _sysctl_tcp_fin_timeout_location =
    (typeof(_sysctl_tcp_fin_timeout_location)) (HAVE_SYSCTL_TCP_FIN_TIMEOUT_ADDR);

#define sysctl_tcp_fin_timeout (*_sysctl_tcp_fin_timeout_location)
#else
#define sysctl_tcp_fin_timeout TCP_FIN_TIMEOUT
#endif
#endif

#ifndef tcp_current_mss
#ifdef HAVE_TCP_CURRENT_MSS_ADDR
unsigned int
tcp_current_mss(struct sock *sk, int large)
{
	static unsigned int (*func) (struct sock *, int) = (typeof(func)) HAVE_TCP_CURRENT_MSS_ADDR;

	return func(sk, large);
}
#endif
#endif

#ifndef sysctl_ip_dynaddr
#ifdef HAVE_SYSCTL_IP_DYNADDR_ADDR
extern int sysctl_ip_dynaddr;
#define sysctl_ip_dynaddr (*((typeof(sysctl_ip_dynaddr) *)HAVE_SYSCTL_IP_DYNADDR_ADDR))
#endif
#endif

#ifndef sysctl_ip_nonlocal_bind
#ifdef HAVE_SYSCTL_IP_NONLOCAL_BIND_ADDR
extern int sysctl_ip_nonlocal_bind;
#define sysctl_ip_nonlocal_bind (*((typeof(sysctl_ip_nonlocal_bind) *)HAVE_SYSCTL_IP_NONLOCAL_BIND_ADDR))
#endif
#endif

#ifndef sysctl_ip_default_ttl
#ifdef HAVE_SYSCTL_IP_DEFAULT_TTL_ADDR
extern int sysctl_ip_default_ttl;
#define sysctl_ip_default_ttl (*((typeof(sysctl_ip_default_ttl) *)HAVE_SYSCTL_IP_DEFAULT_TTL_ADDR))
#endif
#endif


#ifndef tcp_set_skb_tso_segs
#ifdef HAVE_TCP_SET_SKB_TSO_SEGS_ADDR
#ifdef HAVE_KFUNC_TCP_SET_SKB_TSO_SEGS_SOCK
void
tcp_set_skb_tso_segs(struct sock *sk, struct sk_buff *skb)
{
	static void (*func) (struct sock *, struct sk_buff *) =
	    (typeof(func)) HAVE_TCP_SET_SKB_TSO_SEGS_ADDR;

	return func(sk, skb);
}
#else
void
tcp_set_skb_tso_segs(struct sk_buff *skb, unsigned int mss_std)
{
	static void (*func) (struct sk_buff *, unsigned int) =
	    (typeof(func)) HAVE_TCP_SET_SKB_TSO_SEGS_ADDR;

	return func(skb, mss_std);
}
#endif
#endif
#endif

/* older 2.6.8 name for the same function */
#ifndef tcp_set_skb_tso_factor
#ifdef HAVE_TCP_SET_SKB_TSO_FACTOR_ADDR
void
tcp_set_skb_tso_factor(struct sk_buff *skb, unsigned int mss_std)
{
	static void (*func) (struct sk_buff *, unsigned int) =
	    (typeof(func)) HAVE_TCP_SET_SKB_TSO_FACTOR_ADDR;

	return func(skb, mss_std);
}
#endif
#endif

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_inet.h>
#include <sys/xti_sctp.h>

#define T_ALLLEVELS -1

#define LINUX_2_4 1

#define SS__DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SS__EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SS__COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SS__REVISION	"OpenSS7 $RCSfile: inet.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2006/07/07 21:14:54 $"
#define SS__DEVICE	"SVR 4.2 STREAMS INET Drivers (NET4)"
#define SS__CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SS__LICENSE	"GPL"
#define SS__BANNER	SS__DESCRIP	"\n" \
			SS__EXTRA	"\n" \
			SS__REVISION	"\n" \
			SS__COPYRIGHT	"\n" \
			SS__DEVICE	"\n" \
			SS__CONTACT
#define SS__SPLASH	SS__DESCRIP	"\n" \
			SS__REVISION

#ifdef LINUX
MODULE_AUTHOR(SS__CONTACT);
MODULE_DESCRIPTION(SS__DESCRIP);
MODULE_SUPPORTED_DEVICE(SS__DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SS__LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-inet");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SS__DRV_ID	CONFIG_STREAMS_SS__MAJOR
#define SS__DRV_NAME	CONFIG_STREAMS_SS__NAME
#define SS__CMAJORS	CONFIG_STREAMS_SS__NMAJORS
#define SS__CMAJOR_0	CONFIG_STREAMS_SS__MAJOR
#define SS__UNITS	CONFIG_STREAMS_SS__NMINORS
#endif				/* LFS */

#define IP_CMINOR	32

#define FIRST_CMINOR	33

#define ICMP_CMINOR	33
#define GGP_CMINOR	34
#define IPIP_CMINOR	35
#define TCP_CMINOR	36
#define EGP_CMINOR	37
#define PUP_CMINOR	38
#define UDP_CMINOR	39
#define IDP_CMINOR	40
#define RAWIP_CMINOR	41

#define TICOTS_ORD_CMINOR	42
#define TICOTS_CMINOR	43
#define TICLTS_CMINOR	44

#if defined HAVE_OPENSS7_SCTP
#define SCTP_CMINOR	45
#define LAST_CMINOR	45
#else				/* defined HAVE_OPENSS7_SCTP */
#define LAST_CMINOR	44
#endif				/* defined HAVE_OPENSS7_SCTP */

#define FREE_CMINOR	50

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SS__MAJOR));
MODULE_ALIAS("streams-driver-inet");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_SS__MAJOR));
MODULE_ALIAS("/dev/streams/inet");
MODULE_ALIAS("/dev/streams/inet/*");
MODULE_ALIAS("/dev/streams/clone/inet");
#endif
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(IP_CMINOR));
MODULE_ALIAS("/dev/ip");
MODULE_ALIAS("/dev/inet/ip");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(ICMP_CMINOR));
MODULE_ALIAS("/dev/icmp");
MODULE_ALIAS("/dev/inet/icmp");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(GGP_CMINOR));
MODULE_ALIAS("/dev/ggp");
MODULE_ALIAS("/dev/inet/ggp");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(IPIP_CMINOR));
MODULE_ALIAS("/dev/ipip");
MODULE_ALIAS("/dev/inet/ipip");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(TCP_CMINOR));
MODULE_ALIAS("/dev/tcp");
MODULE_ALIAS("/dev/inet/tcp");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(EGP_CMINOR));
MODULE_ALIAS("/dev/egp");
MODULE_ALIAS("/dev/inet/egp");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(PUP_CMINOR));
MODULE_ALIAS("/dev/pup");
MODULE_ALIAS("/dev/inet/pup");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(UDP_CMINOR));
MODULE_ALIAS("/dev/udp");
MODULE_ALIAS("/dev/inet/udp");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(IDP_CMINOR));
MODULE_ALIAS("/dev/idp");
MODULE_ALIAS("/dev/inet/idp");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(RAWIP_CMINOR));
MODULE_ALIAS("/dev/rawip");
MODULE_ALIAS("/dev/inet/rawip");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(TICOTS_ORD_CMINOR));
MODULE_ALIAS("/dev/ticots_ord");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(TICOTS_CMINOR));
MODULE_ALIAS("/dev/ticots");
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(TICLTS_CMINOR));
MODULE_ALIAS("/dev/ticlts");
#if defined HAVE_OPENSS7_SCTP
MODULE_ALIAS("char-major-" __stringify(SS__CMAJOR_0) "-" __stringify(SCTP_CMINOR));
MODULE_ALIAS("/dev/sctp");
MODULE_ALIAS("/dev/inet/sctp");
#endif				/* HAVE_OPENSS7_SCTP */
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SS__DRV_ID
#define DRV_NAME	SS__DRV_NAME
#define CMAJORS		SS__CMAJORS
#define CMAJOR_0	SS__CMAJOR_0
#define UNITS		SS__UNITS
#ifdef MODULE
#define DRV_BANNER	SS__BANNER
#else				/* MODULE */
#define DRV_BANNER	SS__SPLASH
#endif				/* MODULE */

STATIC struct module_info ss_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = 1 << 16,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat ss_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat ss_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

STATIC streamscall int ss_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ss_close(queue_t *, int, cred_t *);

STATIC streamscall int ss_rput(queue_t *, mblk_t *);
STATIC streamscall int ss_rsrv(queue_t *);

STATIC struct qinit ss_rinit = {
	.qi_putp = ss_rput,		/* Read put (msg from below) */
	.qi_srvp = ss_rsrv,		/* Read queue service */
	.qi_qopen = ss_open,		/* Each open */
	.qi_qclose = ss_close,		/* Last close */
	.qi_minfo = &ss_minfo,		/* Information */
	.qi_mstat = &ss_rstat,		/* Statistics */
};

STATIC streamscall int ss_wput(queue_t *, mblk_t *);
STATIC streamscall int ss_wsrv(queue_t *);

STATIC struct qinit ss_winit = {
	.qi_putp = ss_wput,		/* Write put (msg from above) */
	.qi_srvp = ss_wsrv,		/* Write queue service */
	.qi_minfo = &ss_minfo,		/* Information */
	.qi_mstat = &ss_wstat,		/* Statistics */
};

MODULE_STATIC struct streamtab ss_info = {
	.st_rdinit = &ss_rinit,		/* Upper read queue */
	.st_wrinit = &ss_winit,		/* Lower read queue */
};

/*
   TLI interface state flags 
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#ifdef TS_WACK_OPTREQ
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#endif
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)
#define TSM_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)
#define TSM_LISTEN	(TSF_IDLE \
			|TSF_WRES_CIND)
#define TSM_CONNECTED	(TSF_WCON_CREQ\
			|TSF_WRES_CIND\
			|TSF_DATA_XFER\
			|TSF_WIND_ORDREL\
			|TSF_WREQ_ORDREL)
#define TSM_DISCONN	(TSF_IDLE\
			|TSF_UNBND)
#define TSM_INDATA	(TSF_DATA_XFER\
			|TSF_WIND_ORDREL)
#define TSM_OUTDATA	(TSF_DATA_XFER\
			|TSF_WREQ_ORDREL)
#ifndef T_PROVIDER
#define T_PROVIDER  0
#define T_USER	    1
#endif

/*
   Socket state masks 
 */

/*
   TCP state masks 
 */
#define TCPM_CLOSING	(TCPF_CLOSE\
			|TCPF_TIME_WAIT\
			|TCPF_CLOSE_WAIT)
#define TCPM_CONNIND	(TCPF_SYN_RECV\
			|TCPF_ESTABLISHED\
			|TCPF_LISTEN)

/*
 *  =========================================================================
 *
 *  IP Private Datastructures
 *
 *  =========================================================================
 */

typedef struct ss_options {
#if 0
	unsigned char flags[12];	/* twelve bytes of flags for 96 flags */
#else
	unsigned long flags[3];		/* at least 96 flags */
#endif
	struct {
		t_uscalar_t debug[4];	/* XTI_DEBUG */
		struct t_linger linger;	/* XTI_LINGER */
		t_uscalar_t rcvbuf;	/* XTI_RCVBUF */
		t_uscalar_t rcvlowat;	/* XTI_RCVLOWAT */
		t_uscalar_t sndbuf;	/* XTI_SNDBUF */
		t_uscalar_t sndlowat;	/* XTI_SNDLOWAT */
	} xti;
	struct {
		unsigned char options[40];	/* T_IP_OPTIONS */
		unsigned char tos;	/* T_IP_TOS */
		unsigned char ttl;	/* T_IP_TTL */
		unsigned int reuseaddr;	/* T_IP_REUSEADDR */
		unsigned int dontroute;	/* T_IP_DONTROUTE */
		unsigned int broadcast;	/* T_IP_BROADCAST */
		uint32_t addr;		/* T_IP_ADDR */
	} ip;
	struct {
		t_uscalar_t checksum;	/* T_UDP_CHECKSUM */
	} udp;
	struct {
		t_uscalar_t nodelay;	/* T_TCP_NODELAY */
		t_uscalar_t maxseg;	/* T_TCP_MAXSEG */
		struct t_kpalive keepalive;	/* T_TCP_KEEPALIVE */
		t_uscalar_t cork;	/* T_TCP_CORK */
		t_uscalar_t keepidle;	/* T_TCP_KEEPIDLE */
		t_uscalar_t keepitvl;	/* T_TCP_KEEPINTVL */
		t_uscalar_t keepcnt;	/* T_TCP_KEEPCNT */
		t_uscalar_t syncnt;	/* T_TCP_SYNCNT */
		t_uscalar_t linger2;	/* T_TCP_LINGER2 */
		t_uscalar_t defer_accept;	/* T_TCP_DEFER_ACCEPT */
		t_uscalar_t window_clamp;	/* T_TCP_WINDOW_CLAMP */
		struct t_tcp_info info;	/* T_TCP_INFO */
		t_uscalar_t quickack;	/* T_TCP_QUICKACK */
	} tcp;
#if defined HAVE_OPENSS7_SCTP
	struct {
		t_uscalar_t nodelay;	/* T_SCTP_NODELAY */
		t_uscalar_t maxseg;	/* T_SCTP_MAXSEG */
		t_uscalar_t cork;	/* T_SCTP_CORK */
		t_uscalar_t ppi;	/* T_SCTP_PPI */
		t_uscalar_t sid;	/* T_SCTP_SID */
		t_uscalar_t ssn;	/* T_SCTP_SSN */
		t_uscalar_t tsn;	/* T_SCTP_TSN */
		t_uscalar_t recvopt;	/* T_SCTP_RECVOPT */
		t_uscalar_t cookie_life;	/* T_SCTP_COOKIE_LIFE */
		t_uscalar_t sack_delay;	/* T_SCTP_SACK_DELAY */
		t_uscalar_t path_max_retrans;	/* T_SCTP_PATH_MAX_RETRANS */
		t_uscalar_t assoc_max_retrans;	/* T_SCTP_ASSOC_MAX_RETRANS */
		t_uscalar_t max_init_retries;	/* T_SCTP_MAX_INIT_RETRIES */
		t_uscalar_t heartbeat_itvl;	/* T_SCTP_HEARTBEAT_ITVL */
		t_uscalar_t rto_initial;	/* T_SCTP_RTO_INITIAL */
		t_uscalar_t rto_min;	/* T_SCTP_RTO_MIN */
		t_uscalar_t rto_max;	/* T_SCTP_RTO_MAX */
		t_uscalar_t ostreams;	/* T_SCTP_OSTREAMS */
		t_uscalar_t istreams;	/* T_SCTP_ISTREAMS */
		t_uscalar_t cookie_inc;	/* T_SCTP_COOKIE_INC */
		t_uscalar_t throttle_itvl;	/* T_SCTP_THROTTLE_ITVL */
		t_uscalar_t mac_type;	/* T_SCTP_MAC_TYPE */
		t_uscalar_t cksum_type;	/* T_SCTP_CKSUM_TYPE */
		struct t_sctp_hb hb;	/* T_SCTP_HB */
		struct t_sctp_rto rto;	/* T_SCTP_RTO */
		struct {
			struct t_sctp_status status;
			struct t_sctp_dest_status dest_status;
		} status;		/* T_SCTP_STATUS */
		t_uscalar_t debug;	/* T_SCTP_DEBUG */
#if defined CONFIG_SCTP_ECN
		t_uscalar_t ecn;	/* T_SCTP_ECN */
#endif
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
		t_uscalar_t ali;	/* T_SCTP_ALI */
#endif
#if defined CONFIG_SCTP_ADD_IP
		t_uscalar_t add;	/* T_SCTP_ADD */
		t_uscalar_t set;	/* T_SCTP_SET */
		struct sockaddr_in add_ip;	/* T_SCTP_ADD_IP */
		struct sockaddr_in del_ip;	/* T_SCTP_DEL_IP */
		struct sockaddr_in set_ip;	/* T_SCTP_SET_IP */
#endif
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
		t_uscalar_t pr;		/* T_SCTP_PR */
#endif
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
		t_uscalar_t lifetime;	/* T_SCTP_LIFETIME */
#endif
		t_uscalar_t disposition;	/* T_SCTP_DISPOSITION */
		t_uscalar_t max_burst;	/* T_SCTP_MAX_BURST */
	} sctp;
#endif					/* defined HAVE_OPENSS7_SCTP */
} ss_options_t;

typedef struct ss_profile {
	struct {
		uint family;
		uint type;
		uint protocol;
	} prot;
	struct T_info_ack info;
} ss_profile_t;

typedef struct ss_protocol {
	struct ss_protocol *next;	/* list linkage */
	struct ss_protocol **prev;	/* list linkage */
	atomic_t refcnt;		/* reference count */
	spinlock_t lock;		/* structure lock */
	void (*put) (struct ss_protocol *);	/* release function */
	struct ss_protocol *(*get) (struct ss_protocol *);	/* acquire function */
	uint type;			/* structure type */
	uint id;			/* structure id */
	uint state;			/* state */
	uint flags;			/* flags */
	int ps_family;
	int ps_type;
	int ps_protocol;
	dev_t ps_dev;
	int ps_flags;
} ss_protocol_t;

struct ss_protocol *ss_protosw = NULL;

typedef struct inet {
	struct inet *next;		/* list of all IP-Users */
	struct inet **prev;		/* list of all IP-Users */
	size_t refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* associated read queue */
	queue_t *wq;			/* associated write queue */
	cred_t cred;			/* credientials */
	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* RD queue waiting on lock */
	queue_t *wwait;			/* WR queue waiting on lock */
	int users;			/* lock holders */
	uint rbid;			/* RD buffer call id */
	uint wbid;			/* WR buffer call id */
	ushort port;			/* port/protocol number */
	int tcp_state;			/* tcp state history */
	ss_profile_t p;			/* protocol profile */
	struct {
		void (*sk_state_change) (struct sock *);
		void (*sk_data_ready) (struct sock *, int);
		void (*sk_write_space) (struct sock *);
		void (*sk_error_report) (struct sock *);
	} cb_save;			/* call back holder */
	struct sockaddr src;		/* bound address */
	struct sockaddr dst;		/* connected address */
	ss_options_t options;		/* protocol options */
	unsigned char _pad[40];		/* pad for options */
	bufq_t conq;			/* connection queue */
	uint conind;			/* number of connection indications */
	struct socket *sock;		/* socket pointer */
} ss_t;

#define PRIV(__q) (((__q)->q_ptr))
#define SOCK_PRIV(__sk) ((__sk)->sk_user_data)

#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		SK_RMEM_MAX
#define xti_default_rcvlowat		1
#define xti_default_sndbuf		SK_WMEM_MAX
#define xti_default_sndlowat		1

#define ip_default_options		{ 0, }
#define ip_default_tos			0
#define ip_default_ttl			64
#define ip_default_reuseaddr		T_NO
#define ip_default_dontroute		T_NO
#define ip_default_broadcast		T_NO

#define udp_default_checksum		T_YES

#define tcp_default_nodelay		T_NO
#define tcp_default_maxseg		536
#define tcp_default_keepalive		(struct t_kpalive){ T_YES, 1 }

#define sctp_default_nodelay		T_NO
#define sctp_default_maxseg		536
#define sctp_default_cork		T_NO
#define sctp_default_ppi		0
#define sctp_default_sid		0
#define sctp_default_ssn		T_UNSPEC
#define sctp_default_tsn		T_UNSPEC
#define sctp_default_recvopt		T_YES
#define sctp_default_cookie_life	60	/* seconds */
#define sctp_default_sack_delay		200	/* milliseconds */
#define sctp_default_path_max_retrans	5
#define sctp_default_assoc_max_retrans	10
#define sctp_default_max_init_retries	8
#define sctp_default_heartbeat_itvl	30	/* seconds */
#define sctp_default_rto_initial	3000	/* milliseconds */
#define sctp_default_rto_min		1000	/* milliseconds */
#define sctp_default_rto_max		60000	/* milliseconds */
#define sctp_default_ostreams		1
#define sctp_default_istreams		33
#define sctp_default_cookie_inc		1000	/* milliseconds */
#define sctp_default_throttle_itvl	50	/* milliseconds */
#ifdef CONFIG_SCTP_HMAC_MD5
#define sctp_default_mac_type		T_SCTP_HMAC_MD5
#else
#ifdef CONFIG_SCTP_HMAC_SHA1
#define sctp_default_mac_type		T_SCTP_HMAC_SHA1
#else
#define sctp_default_mac_type		T_SCTP_HMAC_NONE
#endif
#endif
#if defined CONFIG_SCTP_CRC32C
#define sctp_default_cksum_type		T_SCTP_CSUM_CRC32C
#else
#define sctp_default_cksum_type		T_SCTP_CSUM_ADLER32
#endif
#define sctp_default_hb			(struct t_sctp_hb){ INADDR_ANY, T_YES, sctp_default_heartbeat_itvl }
#define sctp_default_rto		(struct t_sctp_rto){ INADDR_ANY, sctp_default_rto_initial, sctp_default_rto_min, sctp_default_rto_max, sctp_default_path_max_retrans }
#define sctp_default_sctp_status	(struct t_sctp_status){ 0, }
#define sctp_default_dest_status	(struct t_sctp_dest_status){ INADDR_ANY, }
#define sctp_default_status		{ { 0, }, { INADDR_ANY, } }
#define sctp_default_debug		0
#if defined CONFIG_SCTP_ECN
#define sctp_default_ecn		T_YES
#else
#define sctp_default_ecn		T_NO
#endif
#define sctp_default_ali		0
#define sctp_default_add		T_NO
#define sctp_default_set		T_NO
#define sctp_default_add_ip		INADDR_ANY
#define sctp_default_del_ip		INADDR_ANY
#define sctp_default_set_ip		INADDR_ANY
#define sctp_default_pr			T_NO
#define sctp_default_lifetime		T_UNSPEC
#define sctp_default_disposition	T_UNSPEC
#define sctp_default_max_burst		4

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_IP_OPTIONS,
	_T_BIT_IP_TOS,
	_T_BIT_IP_TTL,
	_T_BIT_IP_REUSEADDR,
	_T_BIT_IP_DONTROUTE,
	_T_BIT_IP_BROADCAST,
	_T_BIT_IP_ADDR,
	_T_BIT_IP_RETOPTS,
	_T_BIT_TCP_NODELAY,
	_T_BIT_TCP_MAXSEG,
	_T_BIT_TCP_KEEPALIVE,
	_T_BIT_TCP_CORK,
	_T_BIT_TCP_KEEPIDLE,
	_T_BIT_TCP_KEEPINTVL,
	_T_BIT_TCP_KEEPCNT,
	_T_BIT_TCP_SYNCNT,
	_T_BIT_TCP_LINGER2,
	_T_BIT_TCP_DEFER_ACCEPT,
	_T_BIT_TCP_WINDOW_CLAMP,
	_T_BIT_TCP_INFO,
	_T_BIT_TCP_QUICKACK,
	_T_BIT_UDP_CHECKSUM,
#if defined HAVE_OPENSS7_SCTP
	_T_BIT_SCTP_NODELAY,
	_T_BIT_SCTP_MAXSEG,
	_T_BIT_SCTP_CORK,
	_T_BIT_SCTP_PPI,
	_T_BIT_SCTP_SID,
	_T_BIT_SCTP_SSN,
	_T_BIT_SCTP_TSN,
	_T_BIT_SCTP_RECVOPT,
	_T_BIT_SCTP_COOKIE_LIFE,
	_T_BIT_SCTP_SACK_DELAY,
	_T_BIT_SCTP_PATH_MAX_RETRANS,
	_T_BIT_SCTP_ASSOC_MAX_RETRANS,
	_T_BIT_SCTP_MAX_INIT_RETRIES,
	_T_BIT_SCTP_HEARTBEAT_ITVL,
	_T_BIT_SCTP_RTO_INITIAL,
	_T_BIT_SCTP_RTO_MIN,
	_T_BIT_SCTP_RTO_MAX,
	_T_BIT_SCTP_OSTREAMS,
	_T_BIT_SCTP_ISTREAMS,
	_T_BIT_SCTP_COOKIE_INC,
	_T_BIT_SCTP_THROTTLE_ITVL,
	_T_BIT_SCTP_MAC_TYPE,
	_T_BIT_SCTP_CKSUM_TYPE,
	_T_BIT_SCTP_HB,
	_T_BIT_SCTP_RTO,
	_T_BIT_SCTP_STATUS,
	_T_BIT_SCTP_DEBUG,
	_T_BIT_SCTP_ECN,
	_T_BIT_SCTP_ALI,
	_T_BIT_SCTP_ADD,
	_T_BIT_SCTP_SET,
	_T_BIT_SCTP_ADD_IP,
	_T_BIT_SCTP_DEL_IP,
	_T_BIT_SCTP_SET_IP,
	_T_BIT_SCTP_PR,
	_T_BIT_SCTP_LIFETIME,
	_T_BIT_SCTP_DISPOSITION,
	_T_BIT_SCTP_MAX_BURST,
#endif					/* defined HAVE_OPENSS7_SCTP */
};

typedef struct ss_event {
	struct sock *sk;		/* sock (child) for event */
	int state;			/* state at time of event */
} ss_event_t;

STATIC spinlock_t ss_lock = SPIN_LOCK_UNLOCKED;	/* protects ss_opens lists */
STATIC ss_t *ss_opens = NULL;

#if 0

/*
   for later when we support default destinations and default listeners 
 */
STATIC ss_t *ss_dflt_dest = NULL;
STATIC ss_t *ss_dflt_lstn = NULL;
#endif

/*
 *  =========================================================================
 *
 *  Socket Callbacks
 *
 *  =========================================================================
 */
STATIC void ss_state_change(struct sock *sk);
STATIC void ss_write_space(struct sock *sk);
STATIC void ss_error_report(struct sock *sk);
STATIC void ss_data_ready(struct sock *sk, int len);
STATIC void
ss_socket_put(struct socket *sock)
{
	struct sock *sk;

	ensure(sock, return);
	if ((sk = sock->sk)) {
		/* We don't really need to lock out interrupts here, just bottom halves 'cause a
		   read lock is taken in the callback function itself. */
		write_lock_bh(&sk->sk_callback_lock);
		{
			ss_t *ss;

			if ((ss = SOCK_PRIV(sk))) {
				SOCK_PRIV(sk) = NULL;
				ss->refcnt--;
				sk->sk_state_change = ss->cb_save.sk_state_change;
				sk->sk_data_ready = ss->cb_save.sk_data_ready;
				sk->sk_write_space = ss->cb_save.sk_write_space;
				sk->sk_error_report = ss->cb_save.sk_error_report;
			} else
				assure(ss);
		}
		write_unlock_bh(&sock->sk->sk_callback_lock);
		/* The following will cause the socket to be aborted, particularly for Linux TCP or 
		   other orderly release sockets.  XXX: Perhaps should check the state of the
		   socket and call sk->prot->disconnect() first as well.  SCTP will probably behave 
		   better that way. */
		sock_set_keepopen(sk);
		sk->sk_lingertime = 0;
	} else
		assure(sk);
	sock_release(sock);
}
STATIC void
ss_socket_get(struct socket *sock, ss_t *ss)
{
	struct sock *sk;

	ensure(sock, return);
	if ((sk = sock->sk)) {
		/* We don't really need to lock out interrupts here, just bottom halves 'cause a
		   read lock is taken in the callback function itself. */
		write_lock_bh(&sock->sk->sk_callback_lock);
		{
			SOCK_PRIV(sk) = ss;
			ss->refcnt++;
			ss->cb_save.sk_state_change = sk->sk_state_change;
			ss->cb_save.sk_data_ready = sk->sk_data_ready;
			ss->cb_save.sk_write_space = sk->sk_write_space;
			ss->cb_save.sk_error_report = sk->sk_error_report;
			sk->sk_state_change = ss_state_change;
			sk->sk_data_ready = ss_data_ready;
			sk->sk_write_space = ss_write_space;
			sk->sk_error_report = ss_error_report;
#ifdef LINUX_2_4
			inet_sk(sk)->recverr = 1;
#else
			sk->ip_recverr = 1;
#endif
			ss->tcp_state = sk->sk_state;	/* initialized tcp state */
		}
		write_unlock_bh(&sock->sk->sk_callback_lock);
	} else
		assure(sk);
}

/*
 *  =========================================================================
 *
 *  Locking
 *
 *  =========================================================================
 */
STATIC int
ss_trylockq(queue_t *q)
{
	int res;
	ss_t *ss = PRIV(q);

	spin_lock_bh(&ss->qlock);
	if (!(res = !ss->users++)) {
		if (q == ss->rq)
			ss->rwait = q;
		if (q == ss->wq)
			ss->wwait = q;
		--ss->users;
	}
	spin_unlock_bh(&ss->qlock);
	return (res);
}
STATIC void
ss_unlockq(queue_t *q)
{
	ss_t *ss = PRIV(q);

	spin_lock_bh(&ss->qlock);
	if (ss->rwait)
		qenable(xchg(&ss->rwait, NULL));
	if (ss->wwait)
		qenable(xchg(&ss->wwait, NULL));
	ss->users = 0;
	spin_unlock_bh(&ss->qlock);
}

/*
 *  =========================================================================
 *
 *  Buffer Allocation
 *
 *  =========================================================================
 */
/*
 *  BUFSRV calls service routine
 *  -------------------------------------------------------------------------
 */
STATIC void streamscall
ss_bufsrv(long data)
{
	queue_t *q = (queue_t *) data;

	if (q) {
		ss_t *ss = PRIV(q);

		spin_lock_bh(&ss->lock);
		if (q == ss->rq) {
			if (ss->rbid) {
				ss->rbid = 0;
				ss->refcnt--;
			}
		} else if (q == ss->wq) {
			if (ss->wbid) {
				ss->wbid = 0;
				ss->refcnt--;
			}
		} else
			swerr();
		spin_unlock_bh(&ss->lock);
		qenable(q);
	}
}

/*
 *  UNBUFCALL
 *  -------------------------------------------------------------------------
 */
STATIC void
__ss_unbufcall(queue_t *q)
{
	ss_t *ss = PRIV(q);

	if (ss->rbid) {
		unbufcall(xchg(&ss->rbid, 0));
		ss->refcnt--;
	}
	if (ss->wbid) {
		unbufcall(xchg(&ss->wbid, 0));
		ss->refcnt--;
	}
}

/*
 *  ALLOCB
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
ss_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;

	if ((mp = allocb(size, prior)))
		return (mp);
	else {
		ss_t *ss = PRIV(q);

		spin_lock_bh(&ss->lock);
		if (q == ss->rq) {
			if (!ss->rbid) {
				ss->rbid = bufcall(size, prior, &ss_bufsrv, (long) q);
				ss->refcnt++;
			}
		} else if (q == ss->wq) {
			if (!ss->wbid) {
				ss->wbid = bufcall(size, prior, &ss_bufsrv, (long) q);
				ss->refcnt++;
			}
		} else
			swerr();
		spin_unlock_bh(&ss->lock);
		return (NULL);
	}
}

#if 0
/*
 *  ESBALLOC
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
ss_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;

	if ((mp = esballoc(base, size, prior, frtn)))
		return (mp);
	else {
		ss_t *ss = PRIV(q);

		if (q == ss->rq) {
			if (!ss->rbid) {
				ss->rbid = esbbcall(prior, &ss_bufsrv, (long) q);
				ss->refcnt++;
			}
			return (NULL);
		}
		if (q == ss->wq) {
			if (!ss->wbid) {
				ss->wbid = esbbcall(prior, &ss_bufsrv, (long) q);
				ss->refcnt++;
			}
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}
#endif

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

STATIC ss_options_t ss_defaults = {
	{0,}
	,
	{
	 xti_default_debug,
	 xti_default_linger,
	 xti_default_rcvbuf,
	 xti_default_rcvlowat,
	 xti_default_sndbuf,
	 xti_default_sndlowat,
	 }
	,
	{
	 ip_default_options,
	 ip_default_tos,
	 ip_default_ttl,
	 ip_default_reuseaddr,
	 ip_default_dontroute,
	 ip_default_broadcast,
	 }
	,
	{
	 udp_default_checksum,
	 }
	,
	{
	 tcp_default_nodelay,
	 tcp_default_maxseg,
	 tcp_default_keepalive,
	 }
#if defined HAVE_OPENSS7_SCTP
	,
	{
	 sctp_default_nodelay,
	 sctp_default_maxseg,
	 sctp_default_cork,
	 sctp_default_ppi,
	 sctp_default_sid,
	 sctp_default_ssn,
	 sctp_default_tsn,
	 sctp_default_recvopt,
	 sctp_default_cookie_life,
	 sctp_default_sack_delay,
	 sctp_default_path_max_retrans,
	 sctp_default_assoc_max_retrans,
	 sctp_default_max_init_retries,
	 sctp_default_heartbeat_itvl,
	 sctp_default_rto_initial,
	 sctp_default_rto_min,
	 sctp_default_rto_max,
	 sctp_default_ostreams,
	 sctp_default_istreams,
	 sctp_default_cookie_inc,
	 sctp_default_throttle_itvl,
	 sctp_default_mac_type,
	 sctp_default_cksum_type,
	 sctp_default_hb,
	 sctp_default_rto,
	 sctp_default_status,
	 sctp_default_debug,
#if defined CONFIG_SCTP_ECN
	 sctp_default_ecn,
#endif
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
	 sctp_default_ali,
#endif
#if defined CONFIG_SCTP_ADD_IP
	 sctp_default_add,
	 sctp_default_set,
	 sctp_default_add_ip,
	 sctp_default_del_ip,
	 sctp_default_set_ip,
#endif
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
	 sctp_default_pr,
#endif
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
	 sctp_default_lifetime,
#endif
	 sctp_default_disposition,
	 sctp_default_max_burst,
	 }
#endif				/* defined HAVE_OPENSS7_SCTP */
};

#define t_defaults ss_defaults

/*
 *  Size connection indication or confirmation options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Return an option buffer size for a connection indication or confirmation.
 *  Options without end-to-end significance are not indicated and are only
 *  confirmed if requested.  Options with end-to-end significance are always
 *  both indicated and confirmed.  For this to work for connection
 *  indications, all request options flags must be cleared to zero.
 */
STATIC int
ss_size_conn_opts(ss_t *ss)
{
	int size = 0;

	if (t_tst_bit(_T_BIT_XTI_DEBUG, ss->options.flags))
		size += _T_SPACE_SIZEOF(ss->options.xti.debug);
	if (t_tst_bit(_T_BIT_XTI_LINGER, ss->options.flags))
		size += _T_SPACE_SIZEOF(ss->options.xti.linger);
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, ss->options.flags))
		size += _T_SPACE_SIZEOF(ss->options.xti.rcvbuf);
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, ss->options.flags))
		size += _T_SPACE_SIZEOF(ss->options.xti.rcvlowat);
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, ss->options.flags))
		size += _T_SPACE_SIZEOF(ss->options.xti.sndbuf);
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, ss->options.flags))
		size += _T_SPACE_SIZEOF(ss->options.xti.sndlowat);
	if (ss->p.prot.family == PF_INET) {
		{
			/* These two have end-to-end significance for connection indications and
			   responses. */
#if 0
			if (t_tst_bit(_T_BIT_IP_OPTIONS, ss->options.flags))
#endif
				size += _T_SPACE_SIZEOF(ss->options.ip.options);
#if 0
			if (t_tst_bit(_T_BIT_IP_TOS, ss->options.flags))
#endif
				size += _T_SPACE_SIZEOF(ss->options.ip.tos);
		}
		if (t_tst_bit(_T_BIT_IP_TTL, ss->options.flags))
			size += _T_SPACE_SIZEOF(ss->options.ip.ttl);
		if (t_tst_bit(_T_BIT_IP_REUSEADDR, ss->options.flags))
			size += _T_SPACE_SIZEOF(ss->options.ip.reuseaddr);
		if (t_tst_bit(_T_BIT_IP_DONTROUTE, ss->options.flags))
			size += _T_SPACE_SIZEOF(ss->options.ip.dontroute);
		if (t_tst_bit(_T_BIT_IP_BROADCAST, ss->options.flags))
			size += _T_SPACE_SIZEOF(ss->options.ip.broadcast);
		if (t_tst_bit(_T_BIT_IP_ADDR, ss->options.flags))
			size += _T_SPACE_SIZEOF(ss->options.ip.addr);
		switch (ss->p.prot.protocol) {
		case T_INET_TCP:
			if (t_tst_bit(_T_BIT_TCP_NODELAY, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.nodelay);
			if (t_tst_bit(_T_BIT_TCP_MAXSEG, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.maxseg);
			if (t_tst_bit(_T_BIT_TCP_KEEPALIVE, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.keepalive);
			if (t_tst_bit(_T_BIT_TCP_CORK, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.cork);
			if (t_tst_bit(_T_BIT_TCP_KEEPIDLE, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.keepidle);
			if (t_tst_bit(_T_BIT_TCP_KEEPINTVL, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.keepitvl);
			if (t_tst_bit(_T_BIT_TCP_KEEPCNT, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.keepcnt);
			if (t_tst_bit(_T_BIT_TCP_SYNCNT, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.syncnt);
			if (t_tst_bit(_T_BIT_TCP_LINGER2, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.linger2);
			if (t_tst_bit(_T_BIT_TCP_DEFER_ACCEPT, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.defer_accept);
			if (t_tst_bit(_T_BIT_TCP_WINDOW_CLAMP, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.window_clamp);
#if 0
			/* read only */
			if (t_tst_bit(_T_BIT_TCP_INFO, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.info);
#endif
			if (t_tst_bit(_T_BIT_TCP_QUICKACK, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.tcp.quickack);
			break;
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t_tst_bit(_T_BIT_SCTP_NODELAY, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.nodelay);
			if (t_tst_bit(_T_BIT_SCTP_MAXSEG, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.maxseg);
			if (t_tst_bit(_T_BIT_SCTP_CORK, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.cork);
			if (t_tst_bit(_T_BIT_SCTP_PPI, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.ppi);
			if (t_tst_bit(_T_BIT_SCTP_SID, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.sid);
			if (t_tst_bit(_T_BIT_SCTP_SSN, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.ssn);
			if (t_tst_bit(_T_BIT_SCTP_TSN, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.tsn);
			if (t_tst_bit(_T_BIT_SCTP_RECVOPT, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.recvopt);
			if (t_tst_bit(_T_BIT_SCTP_COOKIE_LIFE, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.cookie_life);
			if (t_tst_bit(_T_BIT_SCTP_SACK_DELAY, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.sack_delay);
			if (t_tst_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.path_max_retrans);
			if (t_tst_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.assoc_max_retrans);
			if (t_tst_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.max_init_retries);
			if (t_tst_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.heartbeat_itvl);
			if (t_tst_bit(_T_BIT_SCTP_RTO_INITIAL, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.rto_initial);
			if (t_tst_bit(_T_BIT_SCTP_RTO_MIN, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.rto_min);
			if (t_tst_bit(_T_BIT_SCTP_RTO_MAX, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.rto_max);
			{
				/* These two have end-to-end significance for connection
				   indications and connection responses */
#if 0
				if (t_tst_bit(_T_BIT_SCTP_OSTREAMS, ss->options.flags))
#endif
					size += _T_SPACE_SIZEOF(ss->options.sctp.ostreams);
#if 0
				if (t_tst_bit(_T_BIT_SCTP_ISTREAMS, ss->options.flags))
#endif
					size += _T_SPACE_SIZEOF(ss->options.sctp.istreams);
			}
			if (t_tst_bit(_T_BIT_SCTP_COOKIE_INC, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.cookie_inc);
			if (t_tst_bit(_T_BIT_SCTP_THROTTLE_ITVL, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.throttle_itvl);
			if (t_tst_bit(_T_BIT_SCTP_MAC_TYPE, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.mac_type);
			if (t_tst_bit(_T_BIT_SCTP_HB, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.hb);
			if (t_tst_bit(_T_BIT_SCTP_RTO, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.rto);
			if (t_tst_bit(_T_BIT_SCTP_STATUS, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.status);
			if (t_tst_bit(_T_BIT_SCTP_DEBUG, ss->options.flags))
				size += _T_SPACE_SIZEOF(ss->options.sctp.debug);
			break;
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	return (size);
}

/*
 *  Build connection confirmation options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  These are options with end-to-end significance plus any options without end-to-end significance
 *  that were requested for negotiation in the connection request.  For a connection indication,
 *  this is only options with end-to-end significance.  For this to work with connection
 *  indications, all options request flags must be set to zero.  The retrn value is the resulting
 *  size of the options buffer, or a negative error number on software fault.
 *
 *  The t_connect() or t_rcvconnect() functions return the values of all options with end-to-end
 *  significance that were received with the connection response and the negotiated values of those
 *  options without end-to-end significance that had been specified on input.  However, options
 *  specified on input with t_connect() call that are not supported or refer to an unknown option
 *  level are discarded and not returned on output.
 *
 *  The status field of each option returned with t_connect() or t_rcvconnect() indicates if the
 *  proposed value (T_SUCCESS) or a degraded value (T_PARTSUCCESS) has been negotiated.  The status
 *  field of received ancillary information (for example, T_IP options) that is not subject to
 *  negotiation is always set to T_SUCCESS.
 */
STATIC int
t_build_conn_opts(ss_t *ss, unsigned char *op, size_t olen)
{
	struct t_opthdr *oh;
	struct sock *sk;

	if (op == NULL || olen == 0)
		return (0);
	if (!ss || !ss->sock || !(sk = ss->sock->sk))
		goto eproto;
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	if (t_tst_bit(_T_BIT_XTI_DEBUG, ss->options.flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(ss->options.xti.debug);
		oh->level = XTI_GENERIC;
		oh->name = XTI_DEBUG;
		oh->status = T_SUCCESS;
		bcopy(ss->options.xti.debug, T_OPT_DATA(oh), sizeof(ss->options.xti.debug));
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, ss->options.flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(ss->options.xti.linger);
		oh->level = XTI_GENERIC;
		oh->name = XTI_LINGER;
		oh->status = T_SUCCESS;
		if ((sock_tst_linger(sk) == 1) != (ss->options.xti.linger.l_onoff == T_YES)) {
			ss->options.xti.linger.l_onoff = sock_tst_linger(sk) ? T_YES : T_NO;
		}
		if (ss->options.xti.linger.l_onoff == T_YES) {
			if (ss->options.xti.linger.l_linger != sk->sk_lingertime / HZ) {
				if (ss->options.xti.linger.l_linger != T_UNSPEC
				    && ss->options.xti.linger.l_linger < sk->sk_lingertime / HZ)
					oh->status = T_PARTSUCCESS;
				ss->options.xti.linger.l_linger = sk->sk_lingertime / HZ;
			}
		} else
			ss->options.xti.linger.l_linger = T_UNSPEC;
		*((struct t_linger *) T_OPT_DATA(oh)) = ss->options.xti.linger;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, ss->options.flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(ss->options.xti.rcvbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVBUF;
		oh->status = T_SUCCESS;
		if (ss->options.xti.rcvbuf != sk->sk_rcvbuf / 2) {
			if (ss->options.xti.rcvbuf != T_UNSPEC
			    && ss->options.xti.rcvbuf < sk->sk_rcvbuf / 2)
				oh->status = T_PARTSUCCESS;
			ss->options.xti.rcvbuf = sk->sk_rcvbuf / 2;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.xti.rcvbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, ss->options.flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(ss->options.xti.rcvlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVLOWAT;
		oh->status = T_SUCCESS;
		if (ss->options.xti.rcvlowat != sk->sk_rcvlowat) {
			if (ss->options.xti.rcvlowat != T_UNSPEC
			    && ss->options.xti.rcvlowat < sk->sk_rcvlowat)
				oh->status = T_PARTSUCCESS;
			ss->options.xti.rcvlowat = sk->sk_rcvlowat;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.xti.rcvlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, ss->options.flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(ss->options.xti.sndbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDBUF;
		oh->status = T_SUCCESS;
		if (ss->options.xti.sndbuf != sk->sk_sndbuf / 2) {
			if (ss->options.xti.sndbuf != T_UNSPEC
			    && ss->options.xti.sndbuf < sk->sk_sndbuf / 2)
				oh->status = T_PARTSUCCESS;
			ss->options.xti.sndbuf = sk->sk_sndbuf / 2;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.xti.sndbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, ss->options.flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(ss->options.xti.sndlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDLOWAT;
		oh->status = T_SUCCESS;
		if (ss->options.xti.sndlowat != 1) {
			if (ss->options.xti.sndlowat != T_UNSPEC && ss->options.xti.sndlowat < 1)
				oh->status = T_PARTSUCCESS;
			ss->options.xti.sndlowat = 1;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (ss->p.prot.family == PF_INET) {
		struct inet_opt *np = inet_sk(sk);

#if 0
		if (t_tst_bit(_T_BIT_IP_OPTIONS, ss->options.flags))
#endif
		{
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(ss->options.ip.options);
			oh->level = T_INET_IP;
			oh->name = T_IP_OPTIONS;
			oh->status = T_SUCCESS;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
#if 0
		if (t_tst_bit(_T_BIT_IP_TOS, ss->options.flags))
#endif
		{
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(ss->options.ip.tos);
			oh->level = T_INET_IP;
			oh->name = T_IP_TOS;
			oh->status = T_SUCCESS;
			if (ss->options.ip.tos != np->tos) {
				if (ss->options.ip.tos > np->tos)
					oh->status = T_PARTSUCCESS;
				ss->options.ip.tos = np->tos;
			}
			*((unsigned char *) T_OPT_DATA(oh)) = ss->options.ip.tos;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_TTL, ss->options.flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(ss->options.ip.ttl);
			oh->level = T_INET_IP;
			oh->name = T_IP_TTL;
			oh->status = T_SUCCESS;
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_TTL
			if (ss->options.ip.ttl != np->ttl) {
				if (ss->options.ip.ttl > np->ttl)
					oh->status = T_PARTSUCCESS;
				ss->options.ip.ttl = np->ttl;
			}
#else
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
			if (ss->options.ip.ttl != np->uc_ttl) {
				if (ss->options.ip.ttl > np->uc_ttl)
					oh->status = T_PARTSUCCESS;
				ss->options.ip.ttl = np->uc_ttl;
			}
#endif
#endif				/* defined HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
			*((unsigned char *) T_OPT_DATA(oh)) = ss->options.ip.ttl;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_REUSEADDR, ss->options.flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(unsigned int);

			oh->level = T_INET_IP;
			oh->name = T_IP_REUSEADDR;
			oh->status = T_SUCCESS;
			if ((ss->options.ip.reuseaddr == T_NO) != (sk->sk_reuse == 0)) {
				oh->status = T_PARTSUCCESS;
				ss->options.ip.reuseaddr = sk->sk_reuse ? T_YES : T_NO;
			}
			*((unsigned int *) T_OPT_DATA(oh)) = ss->options.ip.reuseaddr;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_DONTROUTE, ss->options.flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(unsigned int);

			oh->level = T_INET_IP;
			oh->name = T_IP_DONTROUTE;
			oh->status = T_SUCCESS;
			if ((ss->options.ip.dontroute == T_NO) != (sock_tst_localroute(sk) == 0)) {
				oh->status = T_PARTSUCCESS;
				ss->options.ip.dontroute = sock_tst_localroute(sk) ? T_YES : T_NO;
			}
			*((unsigned int *) T_OPT_DATA(oh)) = ss->options.ip.dontroute;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_BROADCAST, ss->options.flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(unsigned int);

			oh->level = T_INET_IP;
			oh->name = T_IP_BROADCAST;
			oh->status = T_SUCCESS;
			if ((ss->options.ip.broadcast == T_NO) != (sock_tst_broadcast(sk) == 0)) {
				oh->status = T_PARTSUCCESS;
				ss->options.ip.broadcast = sock_tst_broadcast(sk) ? T_YES : T_NO;
			}
			*((unsigned int *) T_OPT_DATA(oh)) = ss->options.ip.broadcast;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_ADDR, ss->options.flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(uint32_t);

			oh->level = T_INET_IP;
			oh->name = T_IP_ADDR;
			oh->status = T_SUCCESS;
			*((uint32_t *) T_OPT_DATA(oh)) = ss->options.ip.addr;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		switch (ss->p.prot.protocol) {
		case T_INET_TCP:
		{
			struct tcp_opt *tp = tcp_sk(sk);

			if (t_tst_bit(_T_BIT_TCP_NODELAY, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				if ((ss->options.tcp.nodelay == T_NO) != (tp->nonagle == 0)) {
					oh->status = T_PARTSUCCESS;
					ss->options.tcp.nodelay = tp->nonagle ? T_YES : T_NO;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.nodelay;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_MAXSEG, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.maxseg != tcp_user_mss(tp)) {
					if (ss->options.tcp.maxseg > tcp_user_mss(tp))
						oh->status = T_PARTSUCCESS;
					ss->options.tcp.maxseg = tcp_user_mss(tp);
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.maxseg;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPALIVE, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(struct t_kpalive);

				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				if ((ss->options.tcp.keepalive.kp_onoff != T_NO) !=
				    (sock_tst_keepopen(sk) != 0)) {
					ss->options.tcp.keepalive.kp_onoff =
					    sock_tst_keepopen(sk) ? T_YES : T_NO;
				}
				if (ss->options.tcp.keepalive.kp_onoff == T_YES) {
					if (ss->options.tcp.keepalive.kp_timeout !=
					    tp->keepalive_time / 60 / HZ) {
						if (ss->options.tcp.keepalive.kp_timeout != T_UNSPEC
						    && ss->options.tcp.keepalive.kp_timeout >
						    tp->keepalive_time / 60 / HZ)
							oh->status = T_PARTSUCCESS;
						ss->options.tcp.keepalive.kp_timeout =
						    tp->keepalive_time / 60 / HZ;
					}
				} else
					ss->options.tcp.keepalive.kp_timeout = T_UNSPEC;
				*((struct t_kpalive *) T_OPT_DATA(oh)) = ss->options.tcp.keepalive;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_CORK, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				if ((ss->options.tcp.cork == T_YES) != (tp->nonagle == 2)) {
					oh->status = T_FAILURE;
					ss->options.tcp.cork = (tp->nonagle == 2) ? T_YES : T_NO;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.cork;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPIDLE, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.keepidle == T_UNSPEC)
					ss->options.tcp.keepidle = tp->keepalive_time / HZ;
				else if (ss->options.tcp.keepidle != tp->keepalive_time / HZ) {
					oh->status = T_PARTSUCCESS;
					ss->options.tcp.keepidle = tp->keepalive_time / HZ;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.keepidle;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPINTVL, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.keepitvl == T_UNSPEC)
					ss->options.tcp.keepitvl = tp->keepalive_intvl / HZ;
				else if (ss->options.tcp.keepitvl != tp->keepalive_intvl / HZ) {
					oh->status = T_PARTSUCCESS;
					ss->options.tcp.keepcnt = tp->keepalive_intvl / HZ;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.keepcnt;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPCNT, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.keepcnt == T_UNSPEC)
					ss->options.tcp.keepcnt = tp->keepalive_probes;
				else if (ss->options.tcp.keepcnt != tp->keepalive_probes) {
					oh->status = T_FAILURE;
					ss->options.tcp.keepcnt = tp->keepalive_probes;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.keepcnt;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_SYNCNT, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.syncnt == T_UNSPEC)
					ss->options.tcp.syncnt = sock_syn_retries(sk);
				else if (ss->options.tcp.syncnt != sock_syn_retries(sk)) {
					oh->status = T_FAILURE;
					ss->options.tcp.syncnt = sock_syn_retries(sk);
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.syncnt;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_LINGER2, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.linger2 == T_UNSPEC)
					ss->options.tcp.linger2 = tp->linger2 / HZ;
				else if (ss->options.tcp.linger2 != tp->linger2 / HZ) {
					oh->status = T_PARTSUCCESS;
					ss->options.tcp.linger2 = tp->linger2 / HZ;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.linger2;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_DEFER_ACCEPT, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.defer_accept == T_UNSPEC)
					ss->options.tcp.defer_accept =
					    (TCP_TIMEOUT_INIT / HZ) << sock_defer_accept(sk);
				else if (ss->options.tcp.defer_accept !=
					 ((TCP_TIMEOUT_INIT / HZ) << sock_defer_accept(sk))) {
					oh->status = T_PARTSUCCESS;
					ss->options.tcp.defer_accept =
					    ((TCP_TIMEOUT_INIT / HZ) << sock_defer_accept(sk));
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.defer_accept;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_WINDOW_CLAMP, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				if (ss->options.tcp.window_clamp == T_UNSPEC)
					ss->options.tcp.window_clamp = tp->window_clamp;
				else if (ss->options.tcp.window_clamp != tp->window_clamp) {
					oh->status = T_PARTSUCCESS;
					ss->options.tcp.window_clamp = tp->window_clamp;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.window_clamp;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_TCP_INFO, ss->options.flags)) {
				/* read-only */
			}
			if (t_tst_bit(_T_BIT_TCP_QUICKACK, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.tcp.quickack;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
		{
			struct sctp_opt *sp = sctp_sk(sk);

			if (t_tst_bit(_T_BIT_SCTP_NODELAY, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_NODELAY;
				oh->status = T_SUCCESS;
				if ((ss->options.sctp.nodelay != T_NO) != (sp->nonagle != 0)) {
					oh->status = T_PARTSUCCESS;
					ss->options.sctp.nodelay = sp->nonagle ? T_YES : T_NO;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.nodelay;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_MAXSEG, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAXSEG;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.maxseg != sp->user_amps) {
					if (ss->options.sctp.maxseg > sp->user_amps)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.maxseg = sp->user_amps;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.maxseg;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_CORK, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CORK;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.cork;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_PPI, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				ss->options.sctp.ppi = sp->ppi;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.ppi;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_SID, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				ss->options.sctp.sid = sp->sid;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.sid;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_SSN, ss->options.flags)) {
				/* only per-packet */
			}
			if (t_tst_bit(_T_BIT_SCTP_TSN, ss->options.flags)) {
				/* only per-packet */
			}
			if (t_tst_bit(_T_BIT_SCTP_RECVOPT, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RECVOPT;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.recvopt;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_COOKIE_LIFE, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_LIFE;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.cookie_life != sp->ck_life / HZ * 1000) {
					if (ss->options.sctp.cookie_life < sp->ck_life / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.cookie_life = sp->ck_life / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.cookie_life;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_SACK_DELAY, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_DELAY;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.sack_delay != sp->max_sack / HZ * 1000) {
					if (ss->options.sctp.sack_delay < sp->max_sack / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.sack_delay = sp->max_sack / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.sack_delay;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PATH_MAX_RETRANS;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    ss->options.sctp.path_max_retrans;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ASSOC_MAX_RETRANS;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    ss->options.sctp.assoc_max_retrans;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_INIT_RETRIES;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    ss->options.sctp.max_init_retries;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HEARTBEAT_ITVL;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.heartbeat_itvl != sp->hb_itvl / HZ * 1000) {
					if (ss->options.sctp.heartbeat_itvl <
					    sp->hb_itvl / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.heartbeat_itvl = sp->hb_itvl / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.heartbeat_itvl;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO_INITIAL, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_INITIAL;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.rto_initial != sp->rto_ini / HZ * 1000) {
					if (ss->options.sctp.rto_initial < sp->rto_ini / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.rto_initial = sp->rto_ini / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.rto_initial;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO_MIN, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MIN;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.rto_min != sp->rto_min / HZ * 1000) {
					if (ss->options.sctp.rto_min < sp->rto_min / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.rto_min = sp->rto_min / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.rto_min;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO_MAX, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MAX;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.rto_max != sp->rto_max / HZ * 1000) {
					if (ss->options.sctp.rto_max < sp->rto_max / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.rto_max = sp->rto_max / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.rto_max;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if 0
			if (t_tst_bit(_T_BIT_SCTP_OSTREAMS, ss->options.flags))
#endif
			{
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.ostreams != sp->req_ostr) {
					if (ss->options.sctp.ostreams > sp->req_ostr)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.ostreams = sp->req_ostr;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.ostreams;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if 0
			if (t_tst_bit(_T_BIT_SCTP_ISTREAMS, ss->options.flags))
#endif
			{
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.istreams != sp->max_istr) {
					if (ss->options.sctp.istreams > sp->max_istr)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.istreams = sp->max_istr;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.istreams;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_COOKIE_INC, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_INC;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.cookie_inc != sp->ck_inc / HZ * 1000) {
					if (ss->options.sctp.cookie_inc < sp->ck_inc / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.cookie_inc = sp->ck_inc / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.cookie_inc;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_THROTTLE_ITVL, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_THROTTLE_ITVL;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.throttle_itvl != sp->throttle / HZ * 1000) {
					if (ss->options.sctp.throttle_itvl <
					    sp->throttle / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.throttle_itvl = sp->throttle / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.throttle_itvl;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_MAC_TYPE, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAC_TYPE;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.mac_type;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_CKSUM_TYPE, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CKSUM_TYPE;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.cksum_type;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_HB, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(struct t_sctp_hb);

				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HB;
				oh->status = T_SUCCESS;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(struct t_sctp_rto);

				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO;
				oh->status = T_SUCCESS;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_STATUS, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len =
				    T_SPACE(sizeof(struct t_sctp_status) +
					    sizeof(struct t_sctp_dest_status));
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_STATUS;
				oh->status = T_SUCCESS;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_DEBUG, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEBUG;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.debug;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#if defined CONFIG_SCTP_ECN
			if (t_tst_bit(_T_BIT_SCTP_ECN, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ECN;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.ecn;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
			if (t_tst_bit(_T_BIT_SCTP_ALI, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ALI;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.ali;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
			if (t_tst_bit(_T_BIT_SCTP_ADD, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.add;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_SET, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.set;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_ADD_IP, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD_IP;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = ss->options.sctp.add_ip;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_DEL_IP, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEL_IP;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = ss->options.sctp.del_ip;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_SET_IP, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET_IP;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = ss->options.sctp.set_ip;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
			if (t_tst_bit(_T_BIT_SCTP_PR, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PR;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.pr;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
			if (t_tst_bit(_T_BIT_SCTP_LIFETIME, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_LIFETIME;
				oh->status = T_SUCCESS;
				if (ss->options.sctp.lifetime != sp->life / HZ * 1000) {
					if (ss->options.sctp.lifetime < sp->life / HZ * 1000)
						oh->status = T_PARTSUCCESS;
					ss->options.sctp.lifetime = sp->life / HZ * 1000;
				}
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.lifetime;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
			if (t_tst_bit(_T_BIT_SCTP_DISPOSITION, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				/* FIXME: this should probably be read-only */
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.disposition;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (t_tst_bit(_T_BIT_SCTP_MAX_BURST, ss->options.flags)) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_BURST;
				oh->status = T_SUCCESS;
				/* absolute requirement */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = ss->options.sctp.max_burst;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	/* expect oh to be NULL (filled buffer) */
	assure(oh == NULL);
	return (olen);
	// return ((unsigned char *) oh - op); /* return actual length */
      eproto:
	swerr();
	return (-EPROTO);
      efault:
	swerr();
	return (-EFAULT);
}

#ifdef __LP64__
#undef MAX_SCHEDULE_TIMEOUT
#define MAX_SCHEDULE_TIMEOUT INT_MAX
#endif

/*
 *  Set options on new socket.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is used after accepting a new socket.  It is used to negotiate the
 *  options applied to the responding stream in the connection response to the
 *  newly accepted socket.  All options of interest have their flags set and
 *  the appropriate option values set.
 */
STATIC int
t_set_options(ss_t *ss)
{
	struct sock *sk;

	if (!ss || !ss->sock || !(sk = ss->sock->sk))
		goto eproto;
	if (t_tst_bit(_T_BIT_XTI_DEBUG, ss->options.flags)) {
		/* absolute */
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, ss->options.flags)) {
		struct t_linger *valp = &ss->options.xti.linger;

		if (valp->l_onoff == T_NO)
			valp->l_linger = T_UNSPEC;
		else {
			if (valp->l_linger == T_UNSPEC)
				valp->l_linger = t_defaults.xti.linger.l_linger;
			if (valp->l_linger == T_INFINITE)
				valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
			if (valp->l_linger >= MAX_SCHEDULE_TIMEOUT / HZ)
				valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
		}
		if (valp->l_onoff == T_YES) {
			sock_set_linger(sk);
			sk->sk_lingertime = valp->l_linger * HZ;
		} else {
			sock_clr_linger(sk);
			sk->sk_lingertime = 0;
		}
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, ss->options.flags)) {
		t_uscalar_t *valp = &ss->options.xti.rcvbuf;

		if (*valp > sysctl_rmem_max)
			*valp = sysctl_rmem_max;
		if (*valp < SOCK_MIN_RCVBUF / 2)
			*valp = SOCK_MIN_RCVBUF / 2;
		sk->sk_rcvbuf = *valp * 2;
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, ss->options.flags)) {
		t_uscalar_t *valp = &ss->options.xti.rcvlowat;

		if (*valp < 1)
			*valp = 1;
		if (*valp > INT_MAX)
			*valp = INT_MAX;
		sk->sk_rcvlowat = *valp;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, ss->options.flags)) {
		t_uscalar_t *valp = &ss->options.xti.sndbuf;

		if (*valp > sysctl_wmem_max)
			*valp = sysctl_wmem_max;
		if (*valp < SOCK_MIN_SNDBUF / 2)
			*valp = SOCK_MIN_SNDBUF / 2;
		sk->sk_sndbuf = *valp * 2;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, ss->options.flags)) {
		t_uscalar_t *valp = &ss->options.xti.sndlowat;

		if (*valp < 1)
			*valp = 1;
		if (*valp > 1)
			*valp = 1;
	}
	if (ss->p.prot.family == PF_INET) {
		struct inet_opt *np = inet_sk(sk);

		if (t_tst_bit(_T_BIT_IP_OPTIONS, ss->options.flags)) {
			unsigned char *valp = ss->options.ip.options;

			(void) valp;	/* FIXME */
		}
		if (t_tst_bit(_T_BIT_IP_TOS, ss->options.flags)) {
			unsigned char *valp = &ss->options.ip.tos;

			np->tos = *valp;
		}
		if (t_tst_bit(_T_BIT_IP_TTL, ss->options.flags)) {
			unsigned char *valp = &ss->options.ip.ttl;

			if (*valp < 1)
				*valp = 1;
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_TTL
			np->ttl = *valp;
#else
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
			np->uc_ttl = *valp;
#endif
#endif				/* defined HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
		}
		if (t_tst_bit(_T_BIT_IP_REUSEADDR, ss->options.flags)) {
			unsigned int *valp = &ss->options.ip.reuseaddr;

			sk->sk_reuse = (*valp == T_YES) ? 1 : 0;
		}
		if (t_tst_bit(_T_BIT_IP_DONTROUTE, ss->options.flags)) {
			unsigned int *valp = &ss->options.ip.dontroute;

			if (*valp == T_YES)
				sock_set_localroute(sk);
			else
				sock_clr_localroute(sk);
		}
		if (t_tst_bit(_T_BIT_IP_BROADCAST, ss->options.flags)) {
			unsigned int *valp = &ss->options.ip.broadcast;

			if (*valp == T_YES)
				sock_set_broadcast(sk);
			else
				sock_clr_broadcast(sk);
		}
		if (t_tst_bit(_T_BIT_IP_ADDR, ss->options.flags)) {
			uint32_t *valp = &ss->options.ip.addr;

			sock_saddr(sk) = *valp;
		}
		switch (ss->p.prot.protocol) {
		case T_INET_UDP:
			if (t_tst_bit(_T_BIT_UDP_CHECKSUM, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.udp.checksum;

				sk->sk_no_check =
				    (*valp == T_YES) ? UDP_CSUM_DEFAULT : UDP_CSUM_NOXMIT;
			}
			break;
		case T_INET_TCP:
		{
			struct tcp_opt *tp = tcp_sk(sk);

			if (t_tst_bit(_T_BIT_TCP_NODELAY, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.nodelay;

				tp->nonagle = (*valp == T_YES) ? 1 : 0;
			}
			if (t_tst_bit(_T_BIT_TCP_MAXSEG, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.maxseg;

				if (*valp < 8)
					*valp = 8;
				if (*valp > MAX_TCP_WINDOW)
					*valp = MAX_TCP_WINDOW;
				tcp_user_mss(tp) = *valp;
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPALIVE, ss->options.flags)) {
				struct t_kpalive *valp = &ss->options.tcp.keepalive;

				if (valp->kp_onoff == T_NO)
					valp->kp_timeout = T_UNSPEC;
				else {
					if (valp->kp_timeout == T_UNSPEC)
						valp->kp_timeout =
						    t_defaults.tcp.keepalive.kp_timeout;
					if (valp->kp_timeout < 1)
						valp->kp_timeout = 1;
					if (valp->kp_timeout > MAX_SCHEDULE_TIMEOUT / 60 / HZ)
						valp->kp_timeout = MAX_SCHEDULE_TIMEOUT / 60 / HZ;
				}
				if (valp->kp_onoff == T_YES)
					tp->keepalive_time = valp->kp_timeout * 60 * HZ;
#if defined HAVE_TCP_SET_KEEPALIVE_ADDR
				tcp_set_keepalive(sk, valp->kp_onoff == T_YES ? 1 : 0);
#endif				/* defined HAVE_TCP_SET_KEEPALIVE_ADDR */
				if (valp->kp_onoff == T_YES)
					sock_set_keepopen(sk);
				else
					sock_clr_keepopen(sk);
			}
			if (t_tst_bit(_T_BIT_TCP_CORK, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.cork;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPIDLE, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.keepidle;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPINTVL, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.keepitvl;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_KEEPCNT, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.keepcnt;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_SYNCNT, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.syncnt;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_LINGER2, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.linger2;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_DEFER_ACCEPT, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.defer_accept;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_WINDOW_CLAMP, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.window_clamp;

				(void) valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_TCP_INFO, ss->options.flags)) {
				/* read only */
			}
			if (t_tst_bit(_T_BIT_TCP_QUICKACK, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.tcp.quickack;

				(void) valp;	/* TODO: complete this action */
			}
			break;
		}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
		{
			struct sctp_opt *sp = sctp_sk(sk);

			if (t_tst_bit(_T_BIT_SCTP_NODELAY, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.nodelay;

				(void) *valp;	/* TODO: complete this action */
			}
			if (t_tst_bit(_T_BIT_SCTP_MAXSEG, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.maxseg;

				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_WINDOW)
					*valp = MAX_TCP_WINDOW;
				sp->user_amps = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_CORK, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.cork;

				if (sp->nonagle != 1)
					sp->nonagle = (*valp == T_YES) ? 2 : 0;
			}
			if (t_tst_bit(_T_BIT_SCTP_PPI, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.ppi;

				sp->ppi = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_SID, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.sid;

				sp->sid = *valp;
			}
#if 0
			/* These are per-packet, read-only, options only. */
			if (t_tst_bit(_T_BIT_SCTP_SSN, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.ssn;
			}
			if (t_tst_bit(_T_BIT_SCTP_TSN, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.tsn;
			}
#endif
			if (t_tst_bit(_T_BIT_SCTP_RECVOPT, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.recvopt;

				if (*valp == T_YES)
					sp->cmsg_flags |=
					    (SCTP_CMSGF_RECVSID | SCTP_CMSGF_RECVPPI |
					     SCTP_CMSGF_RECVSSN | SCTP_CMSGF_RECVTSN);
				else
					sp->cmsg_flags &=
					    ~(SCTP_CMSGF_RECVSID | SCTP_CMSGF_RECVPPI |
					      SCTP_CMSGF_RECVSSN | SCTP_CMSGF_RECVTSN);
			}
			if (t_tst_bit(_T_BIT_SCTP_COOKIE_LIFE, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.cookie_life;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->ck_life = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_SACK_DELAY, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.sack_delay;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->max_sack = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.path_max_retrans;

				sp->rtx_path = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.assoc_max_retrans;

				sp->max_retrans = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.max_init_retries;

				sp->max_inits = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.heartbeat_itvl;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->hb_itvl = *valp / 1000 * HZ;
#if defined CONFIG_SCTP_THROTTLE_HEARTBEATS
				sp->hb_tint = (*valp >> 1) + 1;
#endif
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO_INITIAL, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.rto_initial;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp > sp->rto_max / HZ * 1000)
					*valp = sp->rto_max / HZ * 1000;
				if (*valp < sp->rto_min / HZ * 1000)
					*valp = sp->rto_min / HZ * 1000;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->rto_ini = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO_MIN, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.rto_min;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp > sp->rto_max / HZ * 1000)
					*valp = sp->rto_max / HZ * 1000;
				if (*valp > sp->rto_ini / HZ * 1000)
					*valp = sp->rto_ini / HZ * 1000;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->rto_min = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO_MAX, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.rto_max;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp < sp->rto_min / HZ * 1000)
					*valp = sp->rto_min / HZ * 1000;
				if (*valp < sp->rto_ini / HZ * 1000)
					*valp = sp->rto_ini / HZ * 1000;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->rto_max = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_OSTREAMS, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.ostreams;

				sp->req_ostr = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_ISTREAMS, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.istreams;

				sp->max_istr = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_COOKIE_INC, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.cookie_inc;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->ck_inc = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_THROTTLE_ITVL, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.throttle_itvl;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->throttle = *valp / 1000 * HZ;
			}
			if (t_tst_bit(_T_BIT_SCTP_MAC_TYPE, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.mac_type;

				sp->hmac = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_CKSUM_TYPE, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.cksum_type;

				sp->cksum = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_HB, ss->options.flags)) {
				struct t_sctp_hb *valp = &ss->options.sctp.hb;

				if (valp->hb_itvl == T_UNSPEC)
					valp->hb_itvl = t_defaults.sctp.hb.hb_itvl;
				if (valp->hb_itvl == T_INFINITE)
					valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->hb_itvl / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->hb_itvl < 1000 / HZ)
					valp->hb_itvl = 1000 / HZ;
				/* FIXME: set values for destination address */
			}
			if (t_tst_bit(_T_BIT_SCTP_RTO, ss->options.flags)) {
				struct t_sctp_rto *valp = &ss->options.sctp.rto;

				if (valp->rto_initial == T_INFINITE)
					valp->rto_initial = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_initial / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->rto_initial = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_min == T_INFINITE)
					valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_min / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_max == T_INFINITE)
					valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_max / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				/* FIXME: set values for destination address */
			}
			if (t_tst_bit(_T_BIT_SCTP_STATUS, ss->options.flags)) {
				/* this is a read-only option */
			}
			if (t_tst_bit(_T_BIT_SCTP_DEBUG, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.debug;

				sp->options = *valp;
			}
#if defined CONFIG_SCTP_ECN
			if (t_tst_bit(_T_BIT_SCTP_ECN, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.ecn;

				if (*valp == T_YES)
					sp->l_caps |= SCTP_CAPS_ECN;
				else
					sp->l_caps &= ~SCTP_CAPS_ECN;
			}
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
			if (t_tst_bit(_T_BIT_SCTP_ALI, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.ali;

				if (*valp)
					sp->l_caps |= SCTP_CAPS_ALI;
				else
					sp->l_caps &= ~SCTP_CAPS_ALI;
			}
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
			if (t_tst_bit(_T_BIT_SCTP_ADD, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.add;

				if (*valp == T_YES)
					sp->l_caps |= SCTP_CAPS_ADD_IP;
				else
					sp->l_caps &= ~SCTP_CAPS_ADD_IP;
			}
			if (t_tst_bit(_T_BIT_SCTP_SET, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.set;

				if (*valp == T_YES)
					sp->l_caps |= SCTP_CAPS_SET_IP;
				else
					sp->l_caps &= ~SCTP_CAPS_SET_IP;
			}
#if 0
			if (t_tst_bit(_T_BIT_SCTP_ADD_IP, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.add_ip;

				sctp_add_ip(sk, valp);
			}
			if (t_tst_bit(_T_BIT_SCTP_DEL_IP, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.del_ip;

				sctp_del_ip(sk, valp);
			}
			if (t_tst_bit(_T_BIT_SCTP_SET_IP, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.set_ip;

				sctp_set_ip(sk, valp);
			}
#endif
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
			if (t_tst_bit(_T_BIT_SCTP_PR, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.pr;

				sp->prel = *valp;
			}
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
			if (t_tst_bit(_T_BIT_SCTP_LIFETIME, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.lifetime;

				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->life = *valp / 1000 * HZ;
			}
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
			if (t_tst_bit(_T_BIT_SCTP_DISPOSITION, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.disposition;

				sp->disp = *valp;
			}
			if (t_tst_bit(_T_BIT_SCTP_MAX_BURST, ss->options.flags)) {
				t_uscalar_t *valp = &ss->options.sctp.max_burst;

				sp->max_burst = *valp;
			}
			break;
		}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	return (0);
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  Parse connection request or response options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Only legal options can be negotiated; illegal options cause failure.  An option is illegal if
 *  the following applies: 1) the length specified in t_opthdr.len exceeds the remaining size of the
 *  option buffer (counted from the beginning of the option); 2) the option value is illegal: the
 *  legal values are defined for each option.  If an illegal option is passed to XTI, the following
 *  will happen: ... if an illegal option is passed to t_accept() or t_connect() then either the
 *  function failes with t_errno set to [TBADOPT] or the connection establishment fails at a later
 *  stage, depending on when the implementation detects the illegal option. ...
 *
 *  If the tansport user passes multiple optiohs in one call and one of them is illegal, the call
 *  fails as described above.  It is, however, possible that some or even all of the smbmitted legal
 *  options were successfully negotiated.  The transport user can check the current status by a call
 *  to t_optmgmt() with the T_CURRENT flag set.
 *
 *  Specifying an option level unknown to the transport provider does not fail in calls to
 *  t_connect(), t_accept() or t_sndudata(); the option is discarded in these cases.  The function
 *  t_optmgmt() fails with [TBADOPT].
 *
 *  Specifying an option name that is unknown to or not supported by the protocol selected by the
 *  option level does not cause failure.  The option is discarded in calles to t_connect(),
 *  t_accept() or t_sndudata().  The function t_optmgmt() returns T_NOTSUPPORT in the status field
 *  of the option.
 *
 *  If a transport user requests negotiation of a read-only option, or a non-privileged user
 *  requests illegal access to a privileged option, the following outcomes are possible: ... 2) if
 *  negotiation of a read-only option is required, t_accept() or t_connect() either fail with
 *  [TACCES], or the connection establishment aborts and a T_DISCONNECT event occurs.  If the
 *  connection aborts, a synchronous call to t_connect() failes with [TLOOK].  It depdends on timing
 *  an implementation conditions whether a t_accept() call still succeeds or failes with [TLOOK].
 *  If a privileged option is illegally requested, the option is quietly ignored.  (A non-privileged
 *  user shall not be able to select an option which is privileged or unsupported.)
 *
 *  If multiple options are submitted to t_connect(), t_accept() or t_sndudata() and a read-only
 *  option is rejected, the connection or the datagram transmission fails as described.  Options
 *  that could be successfully negotiated before the erroneous option was processed retain their
 *  negotiated values.  There is no roll-back mechanmism.
 */
STATIC int
t_parse_conn_opts(ss_t *ss, const unsigned char *ip, size_t ilen, int request)
{
	const struct t_opthdr *ih;
	struct sock *sk;

	/* clear flags, these flags will be used when sending a connection confirmation to
	   determine which options to include in the confirmstion. */
	bzero(ss->options.flags, sizeof(ss->options.flags));
	if (ip == NULL || ilen == 0)
		return (0);
	if (!ss || !ss->sock || !(sk = ss->sock->sk))
		goto eproto;
	/* For each option recognized, we test the requested value for legallity, and then set the
	   requested value in the stream's option buffer and mark the option requested in the
	   options flags.  If it is a request (and not a response), we negotiate the value to the
	   underlying.  socket.  Once the protocol has completed remote negotiation, we will
	   determine whether the negotiation was successful or partially successful.  See
	   t_build_conn_opts(). */
	/* For connection responses, test the legality of each option and mark the option in the
	   options flags.  We do not negotiate to the socket because the final socket is not
	   present.  t_set_options() will read the flags and negotiate to the final socket after
	   the connection has been accepted. */
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		switch (ih->level) {
		case XTI_GENERIC:
			switch (ih->name) {
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (((ih->len - sizeof(*ih)) % sizeof(t_uscalar_t)) != 0)
					goto einval;
				if (ih->len >= sizeof(*ih) + 4 * sizeof(t_uscalar_t))
					ss->options.xti.debug[3] = valp[3];
				else
					ss->options.xti.debug[3] = 0;
				if (ih->len >= sizeof(*ih) + 3 * sizeof(t_uscalar_t))
					ss->options.xti.debug[2] = valp[2];
				else
					ss->options.xti.debug[2] = 0;
				if (ih->len >= sizeof(*ih) + 2 * sizeof(t_uscalar_t))
					ss->options.xti.debug[1] = valp[1];
				else
					ss->options.xti.debug[1] = 0;
				if (ih->len >= sizeof(*ih) + 1 * sizeof(t_uscalar_t))
					ss->options.xti.debug[0] = valp[0];
				else
					ss->options.xti.debug[1] = 0;
				t_set_bit(_T_BIT_XTI_DEBUG, ss->options.flags);
				continue;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (struct t_linger *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					goto einval;
				if (valp->l_linger != T_INFINITE && valp->l_linger != T_UNSPEC
				    && valp->l_linger < 0)
					goto einval;
				ss->options.xti.linger = *valp;
				t_set_bit(_T_BIT_XTI_LINGER, ss->options.flags);
				if (!request)
					continue;
				if (valp->l_onoff == T_NO)
					valp->l_linger = T_UNSPEC;
				else {
					if (valp->l_linger == T_UNSPEC)
						valp->l_linger = t_defaults.xti.linger.l_linger;
					if (valp->l_linger == T_INFINITE)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
					if (valp->l_linger >= MAX_SCHEDULE_TIMEOUT / HZ)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
				}
				if (valp->l_onoff == T_YES) {
					sock_set_linger(sk);
					sk->sk_lingertime = valp->l_linger * HZ;
				} else {
					sock_clr_linger(sk);
					sk->sk_lingertime = 0;
				}
				continue;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				ss->options.xti.rcvbuf = *valp;
				t_set_bit(_T_BIT_XTI_RCVBUF, ss->options.flags);
				if (!request)
					continue;
				if (*valp > sysctl_rmem_max)
					*valp = sysctl_rmem_max;
				if (*valp < SOCK_MIN_RCVBUF / 2)
					*valp = SOCK_MIN_RCVBUF / 2;
				sk->sk_rcvbuf = *valp * 2;
				continue;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				ss->options.xti.rcvlowat = *valp;
				t_set_bit(_T_BIT_XTI_RCVLOWAT, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > INT_MAX)
					*valp = INT_MAX;
				sk->sk_rcvlowat = *valp;
				continue;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				ss->options.xti.sndbuf = *valp;
				t_set_bit(_T_BIT_XTI_SNDBUF, ss->options.flags);
				if (!request)
					continue;
				if (*valp > sysctl_wmem_max)
					*valp = sysctl_wmem_max;
				if (*valp < SOCK_MIN_SNDBUF / 2)
					*valp = SOCK_MIN_SNDBUF / 2;
				sk->sk_sndbuf = *valp * 2;
				continue;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				ss->options.xti.sndlowat = *valp;
				t_set_bit(_T_BIT_XTI_SNDLOWAT, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > 1)
					*valp = 1;
				continue;
			}
			}
			continue;
		case T_INET_IP:
			if (ss->p.prot.family == PF_INET) {
				struct inet_opt *np = inet_sk(sk);

				switch (ih->name) {
				case T_IP_OPTIONS:
				{
					unsigned char *valp = (unsigned char *) T_OPT_DATA(ih);

					(void) valp;	/* FIXME */
					t_set_bit(_T_BIT_IP_OPTIONS, ss->options.flags);
					continue;
				}
				case T_IP_TOS:
				{
					unsigned char *valp = (unsigned char *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.ip.tos = *valp;
					t_set_bit(_T_BIT_IP_TOS, ss->options.flags);
					if (!request)
						continue;
					np->tos = *valp;
					continue;
				}
				case T_IP_TTL:
				{
					unsigned char *valp = (unsigned char *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.ip.ttl = *valp;
					t_set_bit(_T_BIT_IP_TTL, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 1)
						*valp = 1;
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_TTL
					np->ttl = *valp;
#else
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
					np->uc_ttl = *valp;
#endif
#endif				/* defined HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
					continue;
				}
				case T_IP_REUSEADDR:
				{
					unsigned int *valp = (unsigned int *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.ip.reuseaddr = *valp;
					t_set_bit(_T_BIT_IP_REUSEADDR, ss->options.flags);
					if (!request)
						continue;
					sk->sk_reuse = (*valp == T_YES) ? 1 : 0;
					continue;
				}
				case T_IP_DONTROUTE:
				{
					unsigned int *valp = (unsigned int *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.ip.dontroute = *valp;
					t_set_bit(_T_BIT_IP_DONTROUTE, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES)
						sock_set_localroute(sk);
					else
						sock_clr_localroute(sk);
					continue;
				}
				case T_IP_BROADCAST:
				{
					unsigned int *valp = (unsigned int *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.ip.broadcast = *valp;
					t_set_bit(_T_BIT_IP_BROADCAST, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES)
						sock_set_broadcast(sk);
					else
						sock_clr_broadcast(sk);
					continue;
				}
				case T_IP_ADDR:
				{
					uint32_t *valp = (unsigned int *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.ip.addr = *valp;
					t_set_bit(_T_BIT_IP_ADDR, ss->options.flags);
					if (!request)
						continue;
					sock_saddr(sk) = *valp;
					continue;
				}
				}
			}
			continue;
		case T_INET_UDP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				case T_UDP_CHECKSUM:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.udp.checksum = *valp;
					t_set_bit(_T_BIT_UDP_CHECKSUM, ss->options.flags);
					if (!request)
						continue;
					sk->sk_no_check =
					    (*valp == T_YES) ? UDP_CSUM_DEFAULT : UDP_CSUM_NOXMIT;
					continue;
				}
				}
			}
			continue;
		case T_INET_TCP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_TCP) {
				struct tcp_opt *tp = tcp_sk(sk);

				switch (ih->name) {
				case T_TCP_NODELAY:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.tcp.nodelay = *valp;
					t_set_bit(_T_BIT_TCP_NODELAY, ss->options.flags);
					if (!request)
						continue;
					tp->nonagle = (*valp == T_YES) ? 1 : 0;
					continue;
				}
				case T_TCP_MAXSEG:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.tcp.maxseg = *valp;
					t_set_bit(_T_BIT_TCP_MAXSEG, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 8)
						*valp = 8;
					if (*valp > MAX_TCP_WINDOW)
						*valp = MAX_TCP_WINDOW;
					tcp_user_mss(tp) = *valp;
					continue;
				}
				case T_TCP_KEEPALIVE:
				{
					struct t_kpalive *valp =
					    (struct t_kpalive *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (valp->kp_onoff != T_YES && valp->kp_onoff != T_NO)
						goto einval;
					if (valp->kp_timeout != T_INFINITE
					    && valp->kp_timeout != T_UNSPEC && valp->kp_timeout < 0)
						goto einval;
					ss->options.tcp.keepalive = *valp;
					t_set_bit(_T_BIT_TCP_KEEPALIVE, ss->options.flags);
					if (!request)
						continue;
					if (valp->kp_onoff == T_NO)
						valp->kp_timeout = T_UNSPEC;
					else {
						if (valp->kp_timeout == T_UNSPEC)
							valp->kp_timeout =
							    t_defaults.tcp.keepalive.kp_timeout;
						if (valp->kp_timeout < 1)
							valp->kp_timeout = 1;
						if (valp->kp_timeout >
						    MAX_SCHEDULE_TIMEOUT / 60 / HZ)
							valp->kp_timeout =
							    MAX_SCHEDULE_TIMEOUT / 60 / HZ;
					}
					if (valp->kp_onoff == T_YES)
						tp->keepalive_time = valp->kp_timeout * 60 * HZ;
#if defined HAVE_TCP_SET_KEEPALIVE_ADDR
					tcp_set_keepalive(sk, valp->kp_onoff == T_YES ? 1 : 0);
#endif				/* defined HAVE_TCP_SET_KEEPALIVE_ADDR */
					if (valp->kp_onoff == T_YES)
						sock_set_keepopen(sk);
					else
						sock_clr_keepopen(sk);
					continue;
				}
				case T_TCP_CORK:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
					ss->options.tcp.cork = *valp;
					t_set_bit(_T_BIT_TCP_CORK, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES && tp->nonagle == 0)
						tp->nonagle = 2;
					if (*valp == T_NO && tp->nonagle == 2)
						tp->nonagle = 0;
					continue;
				}
				case T_TCP_KEEPIDLE:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.keepidle = *valp;
					t_set_bit(_T_BIT_TCP_KEEPIDLE, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 1)
						*valp = 1;
					if (*valp > MAX_TCP_KEEPIDLE)
						*valp = MAX_TCP_KEEPIDLE;
					tp->keepalive_time = *valp * HZ;
					/* TODO: need to reset the keepalive timer */
					continue;
				}
				case T_TCP_KEEPINTVL:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.keepitvl = *valp;
					t_set_bit(_T_BIT_TCP_KEEPINTVL, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 1)
						*valp = 1;
					if (*valp > MAX_TCP_KEEPINTVL)
						*valp = MAX_TCP_KEEPINTVL;
					tp->keepalive_intvl = *valp * HZ;
					continue;
				}
				case T_TCP_KEEPCNT:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.keepcnt = *valp;
					t_set_bit(_T_BIT_TCP_KEEPCNT, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 1)
						*valp = 1;
					if (*valp > MAX_TCP_KEEPCNT)
						*valp = MAX_TCP_KEEPCNT;
					tp->keepalive_probes = *valp;
					continue;
				}
				case T_TCP_SYNCNT:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.syncnt = *valp;
					t_set_bit(_T_BIT_TCP_SYNCNT, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 1)
						*valp = 1;
					if (*valp > MAX_TCP_SYNCNT)
						*valp = MAX_TCP_SYNCNT;
					sock_syn_retries(sk) = *valp;
					continue;
				}
				case T_TCP_LINGER2:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.linger2 = *valp;
					t_set_bit(_T_BIT_TCP_LINGER2, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 0)
						tp->linger2 = -1;
					else if (*valp > sysctl_tcp_fin_timeout / HZ)
						tp->linger2 = 0;
					else
						tp->linger2 = *valp * HZ;
					continue;
				}
				case T_TCP_DEFER_ACCEPT:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.defer_accept = *valp;
					t_set_bit(_T_BIT_TCP_DEFER_ACCEPT, ss->options.flags);
					if (!request)
						continue;
					if (*valp == 0)
						sock_defer_accept(sk) = 0;
					else {
						for (sock_defer_accept(sk) = 0;
						     sock_defer_accept(sk) < 32
						     && *valp >
						     ((TCP_TIMEOUT_INIT /
						       HZ) << sock_defer_accept(sk));
						     sock_defer_accept(sk)++) ;
						sock_defer_accept(sk)++;
					}
					continue;
				}
				case T_TCP_WINDOW_CLAMP:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					ss->options.tcp.window_clamp = *valp;
					t_set_bit(_T_BIT_TCP_WINDOW_CLAMP, ss->options.flags);
					if (!request)
						continue;
					if (*valp < SOCK_MIN_RCVBUF / 2)
						*valp = SOCK_MIN_RCVBUF / 2;
					tp->window_clamp = *valp;
					continue;
				}
#if 0
				case T_TCP_INFO:
				{
					/* read-only */
				}
				case T_TCP_QUICKACK:
				{
					/* not valid in this state */
				}
#endif
				}
			}
			continue;
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_SCTP) {
				struct sctp_opt *sp = sctp_sk(sk);

				switch (ih->name) {
				case T_SCTP_NODELAY:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.sctp.nodelay = *valp;
					t_set_bit(_T_BIT_SCTP_NODELAY, ss->options.flags);
					if (!request)
						continue;
					continue;
				}
				case T_SCTP_MAXSEG:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.maxseg = *valp;
					t_set_bit(_T_BIT_SCTP_MAXSEG, ss->options.flags);
					if (!request)
						continue;
					if (*valp < 1)
						*valp = 1;
					if (*valp > MAX_TCP_WINDOW)
						*valp = MAX_TCP_WINDOW;
					sp->user_amps = *valp;
					continue;
				}
				case T_SCTP_CORK:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.sctp.cork = *valp;
					t_set_bit(_T_BIT_SCTP_CORK, ss->options.flags);
					if (!request)
						continue;
					if (sp->nonagle != 1)
						sp->nonagle = (*valp == T_YES) ? 2 : 0;
					continue;
				}
				case T_SCTP_PPI:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.ppi = *valp;
					t_set_bit(_T_BIT_SCTP_PPI, ss->options.flags);
					if (!request)
						continue;
					sp->ppi = *valp;
					continue;
				}
				case T_SCTP_SID:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp > sp->n_ostr)
						goto einval;
					ss->options.sctp.sid = *valp;
					t_set_bit(_T_BIT_SCTP_SID, ss->options.flags);
					if (!request)
						continue;
					sp->sid = *valp;
					continue;
				}
				case T_SCTP_SSN:
				{
					/* read-only, and only per-packet */
					goto einval;
				}
				case T_SCTP_TSN:
				{
					/* read-only, and only per-packet */
					goto einval;
				}
				case T_SCTP_RECVOPT:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
					ss->options.sctp.recvopt = *valp;
					t_set_bit(_T_BIT_SCTP_RECVOPT, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES)
						sp->cmsg_flags |=
						    (SCTP_CMSGF_RECVSID | SCTP_CMSGF_RECVPPI |
						     SCTP_CMSGF_RECVSSN | SCTP_CMSGF_RECVTSN);
					else
						sp->cmsg_flags &=
						    ~(SCTP_CMSGF_RECVSID | SCTP_CMSGF_RECVPPI |
						      SCTP_CMSGF_RECVSSN | SCTP_CMSGF_RECVTSN);
					continue;
				}
				case T_SCTP_COOKIE_LIFE:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.cookie_life = *valp;
					t_set_bit(_T_BIT_SCTP_COOKIE_LIFE, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->ck_life = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_SACK_DELAY:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.sack_delay = *valp;
					t_set_bit(_T_BIT_SCTP_SACK_DELAY, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->max_sack = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_PATH_MAX_RETRANS:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.path_max_retrans = *valp;
					t_set_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, ss->options.flags);
					if (!request)
						continue;
					sp->rtx_path = *valp;
					continue;
				}
				case T_SCTP_ASSOC_MAX_RETRANS:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.assoc_max_retrans = *valp;
					t_set_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, ss->options.flags);
					if (!request)
						continue;
					sp->max_retrans = *valp;
					continue;
				}
				case T_SCTP_MAX_INIT_RETRIES:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.max_init_retries = *valp;
					t_set_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, ss->options.flags);
					if (!request)
						continue;
					sp->max_inits = *valp;
					continue;
				}
				case T_SCTP_HEARTBEAT_ITVL:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.heartbeat_itvl = *valp;
					t_set_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->hb_itvl = *valp / 1000 * HZ;
#if defined CONFIG_SCTP_THROTTLE_HEARTBEATS
					sp->hb_tint = (*valp >> 1) + 1;
#endif
					continue;
				}
				case T_SCTP_RTO_INITIAL:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.rto_initial = *valp;
					t_set_bit(_T_BIT_SCTP_RTO_INITIAL, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp > sp->rto_max / HZ * 1000)
						*valp = sp->rto_max / HZ * 1000;
					if (*valp < sp->rto_min / HZ * 1000)
						*valp = sp->rto_min / HZ * 1000;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->rto_ini = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_RTO_MIN:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.rto_min = *valp;
					t_set_bit(_T_BIT_SCTP_RTO_MIN, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp > sp->rto_max / HZ * 1000)
						*valp = sp->rto_max / HZ * 1000;
					if (*valp > sp->rto_ini / HZ * 1000)
						*valp = sp->rto_ini / HZ * 1000;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->rto_min = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_RTO_MAX:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.rto_max = *valp;
					t_set_bit(_T_BIT_SCTP_RTO_MAX, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp < sp->rto_min / HZ * 1000)
						*valp = sp->rto_min / HZ * 1000;
					if (*valp < sp->rto_ini / HZ * 1000)
						*valp = sp->rto_ini / HZ * 1000;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->rto_max = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_OSTREAMS:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp < 1 || *valp > 0x0ffff)
						goto einval;
					ss->options.sctp.ostreams = *valp;
					t_set_bit(_T_BIT_SCTP_OSTREAMS, ss->options.flags);
					if (!request)
						continue;
					sp->req_ostr = *valp;
					continue;
				}
				case T_SCTP_ISTREAMS:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp < 1 || *valp > 0x0ffff)
						goto einval;
					ss->options.sctp.istreams = *valp;
					t_set_bit(_T_BIT_SCTP_ISTREAMS, ss->options.flags);
					if (!request)
						continue;
					sp->max_istr = *valp;
					continue;
				}
				case T_SCTP_COOKIE_INC:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.cookie_inc = *valp;
					t_set_bit(_T_BIT_SCTP_COOKIE_INC, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->ck_inc = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_THROTTLE_ITVL:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.throttle_itvl = *valp;
					t_set_bit(_T_BIT_SCTP_THROTTLE_ITVL, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->throttle = *valp / 1000 * HZ;
					continue;
				}
				case T_SCTP_MAC_TYPE:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_SCTP_HMAC_NONE
#if defined CONFIG_SCTP_HMAC_SHA1
					    && *valp != T_SCTP_HMAC_SHA1
#endif
#if defined CONFIG_SCTP_HMAC_MD5
					    && *valp != T_SCTP_HMAC_MD5
#endif
					    )
						goto einval;
					ss->options.sctp.mac_type = *valp;
					t_set_bit(_T_BIT_SCTP_MAC_TYPE, ss->options.flags);
					if (!request)
						continue;
					sp->hmac = *valp;
					continue;
				}
				case T_SCTP_CKSUM_TYPE:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (1
#if defined CONFIG_SCTP_ADLER_32 || !defined CONFIG_SCTP_CRC_32C
					    && *valp != T_SCTP_CSUM_ADLER32
#endif
#if defined CONFIG_SCTP_CRC_32C
					    && *valp != T_SCTP_CSUM_CRC32C
#endif
					    )
						goto einval;
					ss->options.sctp.cksum_type = *valp;
					t_set_bit(_T_BIT_SCTP_CKSUM_TYPE, ss->options.flags);
					if (!request)
						continue;
					sp->cksum = *valp;
				}
				case T_SCTP_HB:
				{
					struct t_sctp_hb *valp =
					    (struct t_sctp_hb *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (valp->hb_onoff != T_YES && valp->hb_onoff != T_NO)
						goto einval;
					if (valp->hb_itvl != T_INFINITE && valp->hb_itvl != T_UNSPEC
					    && valp->hb_itvl <= 0)
						goto einval;
					ss->options.sctp.hb = *valp;
					t_set_bit(_T_BIT_SCTP_HB, ss->options.flags);
					if (!request)
						continue;
					if (valp->hb_itvl == T_UNSPEC)
						valp->hb_itvl = t_defaults.sctp.hb.hb_itvl;
					if (valp->hb_itvl == T_INFINITE)
						valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->hb_itvl / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->hb_itvl < 1000 / HZ)
						valp->hb_itvl = 1000 / HZ;
					/* FIXME: set values for destination address */
					continue;
				}
				case T_SCTP_RTO:
				{
					struct t_sctp_rto *valp =
					    (struct t_sctp_rto *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (valp->rto_initial != T_INFINITE
					    && valp->rto_initial != T_UNSPEC
					    && valp->rto_initial < 0)
						goto einval;
					if (valp->rto_min != T_INFINITE && valp->rto_min != T_UNSPEC
					    && valp->rto_min < 0)
						goto einval;
					if (valp->rto_max != T_INFINITE && valp->rto_max != T_UNSPEC
					    && valp->rto_max < 0)
						goto einval;
					if (valp->max_retrans != T_INFINITE
					    && valp->max_retrans != T_UNSPEC
					    && valp->max_retrans < 0)
						goto einval;
					if (valp->rto_initial < valp->rto_min
					    || valp->rto_max < valp->rto_min
					    || valp->rto_max < valp->rto_initial)
						goto einval;
					ss->options.sctp.rto = *valp;
					t_set_bit(_T_BIT_SCTP_RTO, ss->options.flags);
					if (!request)
						continue;
					if (valp->rto_initial == T_INFINITE)
						valp->rto_initial =
						    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->rto_initial / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						valp->rto_initial =
						    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->rto_min == T_INFINITE)
						valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->rto_min / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->rto_max == T_INFINITE)
						valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (valp->rto_max / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					/* FIXME: set values for destination address */
					continue;
				}
				case T_SCTP_STATUS:
				{
					/* this is a read-only option */
					goto eacces;
				}
				case T_SCTP_DEBUG:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.debug = *valp;
					t_set_bit(_T_BIT_SCTP_DEBUG, ss->options.flags);
					if (!request)
						continue;
					sp->options = *valp;
					continue;
				}
#if defined CONFIG_SCTP_ECN
				case T_SCTP_ECN:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
					if (*valp == T_YES && !sysctl_sctp_ecn)
						goto eacces;
					ss->options.sctp.ecn = *valp;
					t_set_bit(_T_BIT_SCTP_ECN, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES)
						sp->l_caps |= SCTP_CAPS_ECN;
					else
						sp->l_caps &= ~SCTP_CAPS_ECN;
					continue;
				}
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
				case T_SCTP_ALI:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.ali = *valp;
					t_set_bit(_T_BIT_SCTP_ALI, ss->options.flags);
					if (!request)
						continue;
					sp->l_ali = *valp;
					if (*valp)
						sp->l_caps |= SCTP_CAPS_ALI;
					else
						sp->l_caps &= ~SCTP_CAPS_ALI;
					continue;
				}
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
				case T_SCTP_ADD:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
					ss->options.sctp.add = *valp;
					t_set_bit(_T_BIT_SCTP_ADD, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES)
						sp->l_caps |= SCTP_CAPS_ADD_IP;
					else
						sp->l_caps &= ~SCTP_CAPS_ADD_IP;
					continue;
				}
				case T_SCTP_SET:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
					ss->options.sctp.set = *valp;
					t_set_bit(_T_BIT_SCTP_SET, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_YES)
						sp->l_caps |= SCTP_CAPS_SET_IP;
					else
						sp->l_caps &= ~SCTP_CAPS_SET_IP;
					continue;
				}
#if 0
					/* We do not add, delete or set ip addresses on connection
					   request or on connection response. */
				case T_SCTP_ADD_IP:
				{
					struct sockaddr_in *valp =
					    (struct sockaddr_in *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (valp->sin_family != AF_INET)
						goto einval;
					if (!sk->sport || valp->sin_port != sk->sport)
						goto einval;
					ss->options.sctp.add_ip = *valp;
					t_set_bit(_T_BIT_SCTP_ADD_IP, ss->options.flags);
					if (!request)
						continue;
					sctp_add_ip(sk, valp);
					continue;
				}
				case T_SCTP_DEL_IP:
				{
					struct sockaddr_in *valp =
					    (struct sockaddr_in *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (valp->sin_family != AF_INET)
						goto einval;
					if (!sk->sport || valp->sin_port != sk->sport)
						goto einval;
					ss->options.sctp.del_ip = *valp;
					t_set_bit(_T_BIT_SCTP_DEL_IP, ss->options.flags);
					if (!request)
						continue;
					sctp_del_ip(sk, valp);
					continue;
				}
				case T_SCTP_SET_IP:
				{
					struct sockaddr_in *valp =
					    (struct sockaddr_in *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (valp->sin_family != AF_INET)
						goto einval;
					if (!sk->sport || valp->sin_port != sk->sport)
						goto einval;
					ss->options.sctp.set_ip = *valp;
					t_set_bit(_T_BIT_SCTP_SET_IP, ss->options.flags);
					if (!request)
						continue;
					sctp_set_ip(sk, valp);
					continue;
				}
#endif
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_PR:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != SCTP_PR_NONE && *valp != SCTP_PR_PREFERRED
					    && *valp != SCTP_PR_REQUIRED)
						goto einval;
					ss->options.sctp.pr = *valp;
					t_set_bit(_T_BIT_SCTP_PR, ss->options.flags);
					if (!request)
						continue;
					sp->prel = *valp;
					continue;
				}
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					ss->options.sctp.lifetime = *valp;
					t_set_bit(_T_BIT_SCTP_LIFETIME, ss->options.flags);
					if (!request)
						continue;
					if (*valp == T_INFINITE)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
					sp->life = *valp / 1000 * HZ;
					continue;
				}
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
				case T_SCTP_DISPOSITION:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_SCTP_DISPOSITION_NONE
					    && *valp != T_SCTP_DISPOSITION_UNSENT
					    && *valp != T_SCTP_DISPOSITION_SENT
					    && *valp != T_SCTP_DISPOSITION_GAP_ACKED
					    && *valp != T_SCTP_DISPOSITION_ACKED)
						goto einval;
					ss->options.sctp.disposition = *valp;
					t_set_bit(_T_BIT_SCTP_DISPOSITION, ss->options.flags);
					if (!request)
						continue;
					sp->disp = *valp;
					continue;
				}
				case T_SCTP_MAX_BURST:
				{
					t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp <= 0)
						goto einval;
					ss->options.sctp.max_burst = *valp;
					t_set_bit(_T_BIT_SCTP_MAX_BURST, ss->options.flags);
					if (!request)
						continue;
					sp->max_burst = *valp;
					continue;
				}
				}
			}
			continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	return (0);
      einval:
	return (-EINVAL);
#if defined HAVE_OPENSS7_SCTP
      eacces:
	return (-EACCES);
#endif				/* defined HAVE_OPENSS7_SCTP */
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  Size send cmsg
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Calculate how large a cmsg buffer needs to be to contain all of the
 *  options provided to T_OPTDATA_REQ or T_UNITDATA_REQ.
 */
STATIC streams_fastcall __hot_out int
ss_cmsg_size(const ss_t *ss, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;

	if (ip == NULL || ilen == 0)
		return (olen);
	if (!ss || !ss->sock || !ss->sock->sk)
		goto eproto;
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		case T_INET_IP:
			if (ss->p.prot.family == PF_INET) {
				switch (ih->name) {
				case T_IP_OPTIONS:
					olen += CMSG_SPACE(40);
					continue;
				case T_IP_TOS:
					olen += CMSG_SPACE(sizeof(unsigned char));
					continue;
				case T_IP_TTL:
					olen += CMSG_SPACE(sizeof(unsigned char));
					continue;
				case T_IP_DONTROUTE:
					continue;
				case T_IP_ADDR:
					olen += CMSG_SPACE(sizeof(struct in_pktinfo));
					continue;
				}
			}
			continue;
		case T_INET_UDP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				case T_UDP_CHECKSUM:
					continue;
				}
			}
			continue;
		case T_INET_TCP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_TCP) {
				switch (ih->name) {
				}
			}
			continue;
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_SCTP) {
				switch (ih->name) {
				case T_SCTP_SID:
				case T_SCTP_PPI:
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
					olen += CMSG_SPACE(sizeof(unsigned int));
					continue;
				}
			}
			continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	return (olen);
      eproto:
	swerr();
	return (-EPROTO);
      einval:
	return (-EINVAL);
}

/*
 *  Build send cmsg
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Build a cmsg buffer from options provided to T_OPTDATA_REQ or
 *  T_UNITDATA_REQ.
 */
STATIC streams_fastcall __hot_out int
ss_cmsg_build(const ss_t *ss, const unsigned char *ip, size_t ilen, struct msghdr *msg)
{
	struct cmsghdr *ch = CMSG_FIRSTHDR(msg);
	const struct t_opthdr *ih;
	struct sock *sk;

	if (ip == NULL || ilen == 0)
		return (0);
	if (!ss || !ss->sock || !(sk = ss->sock->sk))
		goto eproto;
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		switch (ih->level) {
		case T_INET_IP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_UDP) {
				struct inet_opt *np = inet_sk(sk);

				switch (ih->name) {
				case T_IP_OPTIONS:
				{
					size_t len =
					    ih->len - sizeof(*ih) < 40 ? ih->len - sizeof(*ih) : 40;

					ch->cmsg_len = CMSG_LEN(len);
					ch->cmsg_level = SOL_IP;
					ch->cmsg_type = IP_OPTIONS;
					bcopy(T_OPT_DATA(ih), CMSG_DATA(ch), len);
					ch = CMSG_NXTHDR(msg, ch);
					continue;
				}
				case T_IP_TOS:
					switch (ss->p.prot.protocol) {
					case T_INET_TCP:
						continue;
					case T_INET_IP:
					case T_INET_UDP:
						if (ih->len == sizeof(*ih) + sizeof(unsigned char)) {
							np->tos = *((unsigned char *)
								    T_OPT_DATA(ih));
						}
						continue;
#ifdef HAVE_OPENSS7_SCTP
					case T_INET_SCTP:
						if (ih->len == sizeof(*ih) + sizeof(unsigned char)) {
							ch->cmsg_len =
							    CMSG_LEN(ih->len - sizeof(*ih));
							ch->cmsg_level = SOL_IP;
							ch->cmsg_type = IP_TOS;
							*((unsigned char *) CMSG_DATA(ch)) =
							    *((unsigned char *)
							      T_OPT_DATA(ih));
							ch = CMSG_NXTHDR(msg, ch);
						}
						continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
					}
					continue;
				case T_IP_TTL:
					switch (ss->p.prot.protocol) {
					case T_INET_TCP:
						continue;
					case T_INET_IP:
					case T_INET_UDP:
						if (ih->len == sizeof(*ih) + sizeof(unsigned char)) {
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_TTL
							np->ttl = *((unsigned char *)
								    T_OPT_DATA(ih));
#else
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
							np->uc_ttl = *((unsigned char *)
								       T_OPT_DATA(ih));
#endif
#endif				/* defined HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
						}
						continue;
#if defined HAVE_OPENSS7_SCTP
					case T_INET_SCTP:
						if (ih->len == sizeof(*ih) + sizeof(unsigned char)) {
							ch->cmsg_len =
							    CMSG_LEN(ih->len - sizeof(*ih));
							ch->cmsg_level = SOL_IP;
							ch->cmsg_type = IP_TTL;
							*((unsigned char *) CMSG_DATA(ch)) =
							    *((unsigned char *)
							      T_OPT_DATA(ih));
							ch = CMSG_NXTHDR(msg, ch);
						}
						continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
					}
					continue;
				case T_IP_DONTROUTE:
					if (ih->len == sizeof(*ih) + sizeof(unsigned int)) {
						if (ss->p.prot.protocol == T_INET_UDP)
							msg->msg_flags |=
							    *((unsigned int *) T_OPT_DATA(ih))
							    == T_NO ? 0 : MSG_DONTROUTE;
					}
					continue;
				case T_IP_ADDR:
					if (ih->len == sizeof(*ih) + sizeof(uint32_t)) {
						ch->cmsg_len = CMSG_LEN(sizeof(struct in_pktinfo));
						ch->cmsg_level = SOL_IP;
						ch->cmsg_type = IP_PKTINFO;
						*((struct in_pktinfo *) CMSG_DATA(ch)) =
						    (struct in_pktinfo) {
							0, {
							*((uint32_t *) T_OPT_DATA(ih))}, {
						0}};
						ch = CMSG_NXTHDR(msg, ch);
					}
					continue;
				}
			}
			continue;
		case T_INET_UDP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				case T_UDP_CHECKSUM:
					if (ih->len == sizeof(*ih) + sizeof(t_uscalar_t)) {
						sk->sk_no_check =
						    *((t_uscalar_t *) T_OPT_DATA(ih)) ==
						    T_NO ? UDP_CSUM_NOXMIT : UDP_CSUM_DEFAULT;
					}
					continue;
				}
			}
			continue;
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_SCTP) {
				switch (ih->name) {
				case T_SCTP_SID:
					if (ih->len == sizeof(*ih) + sizeof(t_uscalar_t)) {
						ch->cmsg_len = CMSG_LEN(sizeof(unsigned int));
						ch->cmsg_level = SOL_SCTP;
						ch->cmsg_type = SCTP_SID;
						*((unsigned int *) CMSG_DATA(ch)) =
						    *((t_uscalar_t *) T_OPT_DATA(ih));
						ch = CMSG_NXTHDR(msg, ch);
					}
					continue;
				case T_SCTP_PPI:
					if (ih->len == sizeof(*ih) + sizeof(t_uscalar_t)) {
						ch->cmsg_len = CMSG_LEN(sizeof(unsigned int));
						ch->cmsg_level = SOL_SCTP;
						ch->cmsg_type = SCTP_PPI;
						*((unsigned int *) CMSG_DATA(ch)) =
						    *((t_uscalar_t *) T_OPT_DATA(ih));
						ch = CMSG_NXTHDR(msg, ch);
					}
					continue;
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
					if (ih->len == sizeof(*ih) + sizeof(t_uscalar_t)) {
						ch->cmsg_len = CMSG_LEN(sizeof(unsigned int));
						ch->cmsg_level = SOL_SCTP;
						ch->cmsg_type = SCTP_LIFETIME;
						*((unsigned int *) CMSG_DATA(ch)) =
						    *((t_uscalar_t *) T_OPT_DATA(ih));
						ch = CMSG_NXTHDR(msg, ch);
					}
					continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
				}
			}
			continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	/* trim length */
	msg->msg_controllen = (unsigned char *) ch - (unsigned char *) msg->msg_control;
	return (0);
      eproto:
	swerr();
	return (-EPROTO);
}

STATIC INLINE streams_fastcall __hot_get int
ss_errs_size(ss_t *ss, struct msghdr *msg)
{
	return (0);
}
STATIC INLINE streams_fastcall __hot_get int
ss_errs_build(ss_t *ss, struct msghdr *msg, unsigned char *op, size_t olen, int *etype)
{
	return (-EFAULT);
}

/*
 *  Size cmsg Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Calculate the size of the options buffer necessary to hold elements in a
 *  cmsg.  Only options with end to end significance are important.
 */
STATIC streams_fastcall __hot_in int
ss_opts_size(const ss_t *ss, struct msghdr *msg)
{
	int size = 0;
	struct cmsghdr *cmsg;

	for (cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
		switch (cmsg->cmsg_level) {
		case SOL_SOCKET:
			/* Noting on recvmsg. */
			continue;
		case SOL_IP:
			if (ss->p.prot.family == PF_INET
			    && (ss->p.prot.protocol == T_INET_UDP
				|| ss->p.prot.protocol == T_INET_IP)) {
				switch (cmsg->cmsg_type) {
				case IP_PKTINFO:
					size += _T_SPACE_SIZEOF(uint32_t);

					continue;
				case IP_TTL:
					size += _T_SPACE_SIZEOF(unsigned char);

					continue;
				case IP_TOS:
					size += _T_SPACE_SIZEOF(unsigned char);

					continue;
				case IP_RECVOPTS:
					size +=
					    T_SPACE((cmsg->cmsg_len < 40 ? cmsg->cmsg_len : 40));
					continue;
				}
			}
			continue;
		case SOL_TCP:
			/* TCP does not have any options of end to end significance */
			continue;
		case SOL_UDP:
			/* Nothing on recvmsg.  Linux is a little deficient here: it should be able 
			   to indicate whether the incoming datagram was checksummed or not. It
			   cannot.  T_UDP_CHECKSUM cannot be properly indicated.  Sorry. */
			continue;
#if defined HAVE_OPENSS7_SCTP
		case SOL_SCTP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_SCTP) {
				switch (cmsg->cmsg_type) {
				case SCTP_PPI:
				case SCTP_DISPOSITION:
					size += _T_SPACE_SIZEOF(t_uscalar_t);
					continue;
				}
			}
			continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	return (size);
}

/*
 *  Build options from cmsg.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Data indications only indicate options with end-to-end significance.
 */
STATIC streams_fastcall __hot_in int
ss_opts_build(const ss_t *ss, struct msghdr *msg, unsigned char *op, size_t olen)
{
	struct cmsghdr *cmsg;
	struct t_opthdr *oh;

	if (op == NULL || olen == 0)
		return (0);
	for (cmsg = CMSG_FIRSTHDR(msg), oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0); cmsg && oh;
	     cmsg = CMSG_NXTHDR(msg, cmsg)) {
		switch (cmsg->cmsg_level) {
		case SOL_SOCKET:
			continue;
		case SOL_IP:
			if (ss->p.prot.protocol == T_INET_UDP || ss->p.prot.protocol == T_INET_IP) {
				switch (cmsg->cmsg_type) {
				case IP_RECVOPTS:
				{
					size_t len;

					if (oh == NULL)
						goto efault;
					len = cmsg->cmsg_len < 40 ? cmsg->cmsg_len : 40;

					printd(("%s: %p: processing option IP_RECVOPTS\n", DRV_NAME,
						ss));
					printd(("%s: %p: building option T_IP_OPTIONS\n", DRV_NAME,
						ss));
					oh->len = T_LENGTH(len);
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = T_SUCCESS;
					bcopy(CMSG_DATA(cmsg), T_OPT_DATA(oh), len);
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				}
				case IP_TOS:
					if (oh == NULL)
						goto efault;
					printd(("%s: %p: processing option IP_TOS\n", DRV_NAME,
						ss));
					printd(("%s: %p: building option T_IP_TOS\n", DRV_NAME,
						ss));
					oh->len = _T_LENGTH_SIZEOF(unsigned char);

					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					*((unsigned char *) T_OPT_DATA(oh)) =
					    *((unsigned char *) CMSG_DATA(cmsg));
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				case IP_TTL:
					if (oh == NULL)
						goto efault;
					printd(("%s: %p: processing option IP_TTL\n", DRV_NAME,
						ss));
					printd(("%s: %p: building option T_IP_TTL\n", DRV_NAME,
						ss));
					oh->len = _T_LENGTH_SIZEOF(unsigned char);

					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					*((unsigned char *) T_OPT_DATA(oh)) =
					    *((unsigned char *) CMSG_DATA(cmsg));
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				case IP_PKTINFO:
					if (oh == NULL)
						goto efault;
					printd(("%s: %p: processing option IP_PKTINFO\n", DRV_NAME,
						ss));
					printd(("%s: %p: building option T_IP_ADDR\n", DRV_NAME,
						ss));
					oh->len = _T_LENGTH_SIZEOF(uint32_t);

					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					*((uint32_t *) T_OPT_DATA(oh)) =
					    ((struct in_pktinfo *) CMSG_DATA(cmsg))->ipi_addr.
					    s_addr;
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				}
			}
			continue;
		case SOL_TCP:
			/* TCP does not have any options of end to end significance */
			continue;
		case SOL_UDP:
			/* Nothing on recvmsg.  Linux is a little deficient here: it should be able 
			   to indicate whether the incoming datagram was checksummed or not. It
			   cannot.  T_UDP_CHECKSUM cannot be properly indicated.  Sorry. */
			continue;
#if defined HAVE_OPENSS7_SCTP
		case SOL_SCTP:
			switch (cmsg->cmsg_type) {
			case SCTP_PPI:
				if (oh == NULL)
					goto efault;
				printd(("%s: %p: processing option SCTP_PPI\n", DRV_NAME, ss));
				printd(("%s: %p: building option T_SCTP_PPI\n", DRV_NAME, ss));
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    *((unsigned int *) CMSG_DATA(cmsg));
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
				continue;
			case SCTP_DISPOSITION:
				if (oh == NULL)
					goto efault;
				printd(("%s: %p: processing option SCTP_DISPOSITION\n", DRV_NAME,
					ss));
				printd(("%s: %p: building option T_SCTP_DISPOSITION\n", DRV_NAME,
					ss));
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    *((unsigned int *) CMSG_DATA(cmsg));
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
				continue;
			}
			continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	assure(oh == NULL);
	return (olen);
	// return ((unsigned char *) oh - op);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  Size Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Check the validity of the options structure, check for correct size of each supplied option
 *  given the option management flag, and return the size required of the acknowledgement options
 *  field.
 */
STATIC int
t_size_default_options(const ss_t *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			if (t->p.prot.family == PF_INET) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					/* not supported yet */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TOS:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.tos);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TTL:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ttl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_REUSEADDR:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.reuseaddr);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_DONTROUTE:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.dontroute);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_BROADCAST:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.broadcast);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_ADDR:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.addr);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					olen += _T_SPACE_SIZEOF(t_defaults.udp.checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.nodelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_MAXSEG:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.maxseg);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPALIVE:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepalive);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_CORK:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.cork);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPIDLE:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepidle);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPINTVL:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepitvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPCNT:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepcnt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_SYNCNT:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.syncnt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_LINGER2:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.linger2);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_DEFER_ACCEPT:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.defer_accept);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_WINDOW_CLAMP:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.window_clamp);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_INFO:
					/* read only, can't get default */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_QUICKACK:
					olen += _T_SPACE_SIZEOF(t_defaults.tcp.quickack);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_SCTP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.nodelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CORK:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.cork);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PPI:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.ppi);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SID:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.sid);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SSN:
					/* read only, can't get default */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_TSN:
					/* read only, can't get default */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RECVOPT:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.recvopt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_LIFE:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.cookie_life);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SACK_DELAY:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.sack_delay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PATH_MAX_RETRANS:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.path_max_retrans);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ASSOC_MAX_RETRANS:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.assoc_max_retrans);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_INIT_RETRIES:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.max_init_retries);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HEARTBEAT_ITVL:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.heartbeat_itvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_INITIAL:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto_initial);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MIN:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto_min);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MAX:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto_max);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_OSTREAMS:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.ostreams);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ISTREAMS:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.istreams);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_INC:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.cookie_inc);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_THROTTLE_ITVL:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.throttle_itvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAC_TYPE:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.mac_type);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CKSUM_TYPE:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.cksum_type);
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
				case T_SCTP_ECN:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.ecn);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
				case T_SCTP_ALI:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.ali);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
				case T_SCTP_ADD:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.add);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.set);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ADD_IP:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.add_ip);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEL_IP:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.del_ip);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET_IP:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.set_ip);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_PR:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.pr);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.lifetime);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
				case T_SCTP_DISPOSITION:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.disposition);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_BURST:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.max_burst);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HB:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.hb);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAXSEG:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.maxseg);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_STATUS:
					/* read-only, no default */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEBUG:
					olen += _T_SPACE_SIZEOF(t_defaults.sctp.debug);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	ptrace(("%s: %p: Calculated option output size = %u\n", DRV_NAME, t, olen));
	return (olen);
      einval:
	ptrace(("%s: %p: ERROR: Invalid input options\n", DRV_NAME, t));
	return (-EINVAL);
}

STATIC int
t_size_current_options(const ss_t *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t->options.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t->options.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t->options.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t->options.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			if (t->p.prot.family == PF_INET) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					/* not supported yet */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TOS:
					olen += _T_SPACE_SIZEOF(t->options.ip.tos);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TTL:
					olen += _T_SPACE_SIZEOF(t->options.ip.ttl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_REUSEADDR:
					olen += _T_SPACE_SIZEOF(t->options.ip.reuseaddr);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_DONTROUTE:
					olen += _T_SPACE_SIZEOF(t->options.ip.dontroute);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_BROADCAST:
					olen += _T_SPACE_SIZEOF(t->options.ip.broadcast);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_ADDR:
					olen += _T_SPACE_SIZEOF(t->options.ip.addr);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					olen += _T_SPACE_SIZEOF(t->options.udp.checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					olen += _T_SPACE_SIZEOF(t->options.tcp.nodelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_MAXSEG:
					olen += _T_SPACE_SIZEOF(t->options.tcp.maxseg);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPALIVE:
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepalive);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_CORK:
					olen += _T_SPACE_SIZEOF(t->options.tcp.cork);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPIDLE:
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepidle);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPINTVL:
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepitvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPCNT:
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepcnt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_SYNCNT:
					olen += _T_SPACE_SIZEOF(t->options.tcp.syncnt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_LINGER2:
					olen += _T_SPACE_SIZEOF(t->options.tcp.linger2);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_DEFER_ACCEPT:
					olen += _T_SPACE_SIZEOF(t->options.tcp.defer_accept);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_WINDOW_CLAMP:
					olen += _T_SPACE_SIZEOF(t->options.tcp.window_clamp);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_INFO:
					olen += _T_SPACE_SIZEOF(t->options.tcp.info);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_QUICKACK:
					olen += _T_SPACE_SIZEOF(t->options.tcp.quickack);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_SCTP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					olen += _T_SPACE_SIZEOF(t->options.sctp.nodelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CORK:
					olen += _T_SPACE_SIZEOF(t->options.sctp.cork);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PPI:
					olen += _T_SPACE_SIZEOF(t->options.sctp.ppi);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SID:
					olen += _T_SPACE_SIZEOF(t->options.sctp.sid);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SSN:
					olen += _T_SPACE_SIZEOF(t->options.sctp.ssn);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_TSN:
					olen += _T_SPACE_SIZEOF(t->options.sctp.tsn);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RECVOPT:
					olen += _T_SPACE_SIZEOF(t->options.sctp.recvopt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_LIFE:
					olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_life);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SACK_DELAY:
					olen += _T_SPACE_SIZEOF(t->options.sctp.sack_delay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PATH_MAX_RETRANS:
					olen += _T_SPACE_SIZEOF(t->options.sctp.path_max_retrans);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ASSOC_MAX_RETRANS:
					olen += _T_SPACE_SIZEOF(t->options.sctp.assoc_max_retrans);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_INIT_RETRIES:
					olen += _T_SPACE_SIZEOF(t->options.sctp.max_init_retries);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HEARTBEAT_ITVL:
					olen += _T_SPACE_SIZEOF(t->options.sctp.heartbeat_itvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_INITIAL:
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto_initial);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MIN:
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto_min);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MAX:
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto_max);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_OSTREAMS:
					olen += _T_SPACE_SIZEOF(t->options.sctp.ostreams);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ISTREAMS:
					olen += _T_SPACE_SIZEOF(t->options.sctp.istreams);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_INC:
					olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_inc);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_THROTTLE_ITVL:
					olen += _T_SPACE_SIZEOF(t->options.sctp.throttle_itvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAC_TYPE:
					olen += _T_SPACE_SIZEOF(t->options.sctp.mac_type);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CKSUM_TYPE:
					olen += _T_SPACE_SIZEOF(t->options.sctp.cksum_type);
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
				case T_SCTP_ECN:
					olen += _T_SPACE_SIZEOF(t->options.sctp.ecn);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
				case T_SCTP_ALI:
					olen += _T_SPACE_SIZEOF(t->options.sctp.ali);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
				case T_SCTP_ADD:
					olen += _T_SPACE_SIZEOF(t->options.sctp.add);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET:
					olen += _T_SPACE_SIZEOF(t->options.sctp.set);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ADD_IP:
					olen += _T_SPACE_SIZEOF(t->options.sctp.add_ip);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEL_IP:
					olen += _T_SPACE_SIZEOF(t->options.sctp.del_ip);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET_IP:
					olen += _T_SPACE_SIZEOF(t->options.sctp.set_ip);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_PR:
					olen += _T_SPACE_SIZEOF(t->options.sctp.pr);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
					olen += _T_SPACE_SIZEOF(t->options.sctp.lifetime);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
				case T_SCTP_DISPOSITION:
					olen += _T_SPACE_SIZEOF(t->options.sctp.disposition);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_BURST:
					olen += _T_SPACE_SIZEOF(t->options.sctp.max_burst);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HB:
					olen += _T_SPACE_SIZEOF(t->options.sctp.hb);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO:
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAXSEG:
					olen += _T_SPACE_SIZEOF(t->options.sctp.maxseg);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_STATUS:
					olen += _T_SPACE_SIZEOF(t->options.sctp.status);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEBUG:
					olen += _T_SPACE_SIZEOF(t->options.sctp.debug);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	ptrace(("%s: %p: Calculated option output size = %u\n", DRV_NAME, t, olen));
	return (olen);
      einval:
	ptrace(("%s: %p: ERROR: Invalid input options\n", DRV_NAME, t));
	return (-EINVAL);
}

STATIC int
t_size_check_options(const ss_t *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				/* can be any non-zero array of t_uscalar_t */
				if (optlen
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (optlen && optlen != sizeof(t->options.xti.linger))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (optlen && optlen != sizeof(t->options.xti.rcvbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (optlen && optlen != sizeof(t->options.xti.rcvlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (optlen && optlen != sizeof(t->options.xti.sndbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (optlen && optlen != sizeof(t->options.xti.sndlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			if (t->p.prot.family == PF_INET) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					if (optlen && optlen != sizeof(t->options.ip.options))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TOS:
					if (optlen && optlen != sizeof(t->options.ip.tos))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TTL:
					if (optlen && optlen != sizeof(t->options.ip.ttl))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_REUSEADDR:
					if (optlen && optlen != sizeof(t->options.ip.reuseaddr))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_DONTROUTE:
					if (optlen && optlen != sizeof(t->options.ip.dontroute))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_BROADCAST:
					if (optlen && optlen != sizeof(t->options.ip.broadcast))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_ADDR:
					if (optlen && optlen != sizeof(t->options.ip.addr))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					if (optlen && optlen != sizeof(t->options.udp.checksum))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					if (optlen && optlen != sizeof(t->options.tcp.nodelay))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_MAXSEG:
					if (optlen && optlen != sizeof(t->options.tcp.maxseg))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPALIVE:
					if (optlen && optlen != sizeof(t->options.tcp.keepalive))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_CORK:
					if (optlen && optlen != sizeof(t->options.tcp.cork))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPIDLE:
					if (optlen && optlen != sizeof(t->options.tcp.keepidle))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPINTVL:
					if (optlen && optlen != sizeof(t->options.tcp.keepitvl))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPCNT:
					if (optlen && optlen != sizeof(t->options.tcp.keepcnt))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_SYNCNT:
					if (optlen && optlen != sizeof(t->options.tcp.syncnt))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_LINGER2:
					if (optlen && optlen != sizeof(t->options.tcp.linger2))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_DEFER_ACCEPT:
					if (optlen && optlen != sizeof(t->options.tcp.defer_accept))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_WINDOW_CLAMP:
					if (optlen && optlen != sizeof(t->options.tcp.window_clamp))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_INFO:
					if (optlen && optlen != sizeof(t->options.tcp.info))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_QUICKACK:
					if (optlen && optlen != sizeof(t->options.tcp.quickack))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_SCTP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					if (optlen && optlen != sizeof(t->options.sctp.nodelay))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CORK:
					if (optlen && optlen != sizeof(t->options.sctp.cork))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PPI:
					if (optlen && optlen != sizeof(t->options.sctp.ppi))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SID:
					if (optlen && optlen != sizeof(t->options.sctp.sid))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SSN:
					if (optlen && optlen != sizeof(t->options.sctp.ssn))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_TSN:
					if (optlen && optlen != sizeof(t->options.sctp.tsn))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RECVOPT:
					if (optlen && optlen != sizeof(t->options.sctp.recvopt))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_LIFE:
					if (optlen && optlen != sizeof(t->options.sctp.cookie_life))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SACK_DELAY:
					if (optlen && optlen != sizeof(t->options.sctp.sack_delay))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PATH_MAX_RETRANS:
					if (optlen
					    && optlen != sizeof(t->options.sctp.path_max_retrans))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ASSOC_MAX_RETRANS:
					if (optlen
					    && optlen != sizeof(t->options.sctp.assoc_max_retrans))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_INIT_RETRIES:
					if (optlen
					    && optlen != sizeof(t->options.sctp.max_init_retries))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HEARTBEAT_ITVL:
					if (optlen
					    && optlen != sizeof(t->options.sctp.heartbeat_itvl))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_INITIAL:
					if (optlen && optlen != sizeof(t->options.sctp.rto_initial))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MIN:
					if (optlen && optlen != sizeof(t->options.sctp.rto_min))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MAX:
					if (optlen && optlen != sizeof(t->options.sctp.rto_max))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_OSTREAMS:
					if (optlen && optlen != sizeof(t->options.sctp.ostreams))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ISTREAMS:
					if (optlen && optlen != sizeof(t->options.sctp.istreams))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_INC:
					if (optlen && optlen != sizeof(t->options.sctp.cookie_inc))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_THROTTLE_ITVL:
					if (optlen
					    && optlen != sizeof(t->options.sctp.throttle_itvl))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAC_TYPE:
					if (optlen && optlen != sizeof(t->options.sctp.mac_type))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CKSUM_TYPE:
					if (optlen && optlen != sizeof(t->options.sctp.cksum_type))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
				case T_SCTP_ECN:
					if (optlen && optlen != sizeof(t->options.sctp.ecn))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
				case T_SCTP_ALI:
					if (optlen && optlen != sizeof(t->options.sctp.ali))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
				case T_SCTP_ADD:
					if (optlen && optlen != sizeof(t->options.sctp.add))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET:
					if (optlen && optlen != sizeof(t->options.sctp.set))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ADD_IP:
					if (optlen && optlen != sizeof(t->options.sctp.add_ip))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEL_IP:
					if (optlen && optlen != sizeof(t->options.sctp.del_ip))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET_IP:
					if (optlen && optlen != sizeof(t->options.sctp.set_ip))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_PR:
					if (optlen && optlen != sizeof(t->options.sctp.pr))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
					if (optlen && optlen != sizeof(t->options.sctp.lifetime))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
				case T_SCTP_DISPOSITION:
					if (optlen && optlen != sizeof(t->options.sctp.disposition))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_BURST:
					if (optlen && optlen != sizeof(t->options.sctp.max_burst))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HB:
					if (optlen && optlen != sizeof(t->options.sctp.hb))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO:
					if (optlen && optlen != sizeof(t->options.sctp.rto))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAXSEG:
					if (optlen && optlen != sizeof(t->options.sctp.maxseg))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_STATUS:
					/* read-only */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEBUG:
					if (optlen && optlen != sizeof(t->options.sctp.debug))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	ptrace(("%s: %p: Calculated option output size = %u\n", DRV_NAME, t, olen));
	return (olen);
      einval:
	ptrace(("%s: %p: ERROR: Invalid input options\n", DRV_NAME, t));
	return (-EINVAL);
}

STATIC int
t_size_negotiate_options(const ss_t *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				if (ih->name != T_ALLOPT
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.linger))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.rcvbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.xti.rcvlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.sndbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.xti.sndlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			if (t->p.prot.family == PF_INET) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
					   T_READONLY, the returned option value is the same as the 
					   one requested on input. */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TOS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.ip.tos))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.ip.tos);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_TTL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.ip.ttl))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.ip.ttl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_REUSEADDR:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.ip.reuseaddr))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.ip.reuseaddr);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_DONTROUTE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.ip.dontroute))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.ip.dontroute);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_BROADCAST:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.ip.broadcast))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.ip.broadcast);
					if (ih->name != T_ALLOPT)
						continue;
				case T_IP_ADDR:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.ip.addr))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.ip.addr);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.udp.checksum))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.udp.checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.nodelay))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.nodelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_MAXSEG:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.maxseg))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.maxseg);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPALIVE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.keepalive))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepalive);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_CORK:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.cork))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.cork);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPIDLE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.keepidle))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepidle);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPINTVL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.keepitvl))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepitvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_KEEPCNT:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.keepcnt))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.keepcnt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_SYNCNT:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.syncnt))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.syncnt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_LINGER2:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.linger2))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.linger2);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_DEFER_ACCEPT:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.defer_accept))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.defer_accept);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_WINDOW_CLAMP:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.window_clamp))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.window_clamp);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_INFO:
					/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
					   T_READONLY, the returned option value is the same as the 
					   one requested on input. */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCP_QUICKACK:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.tcp.quickack))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.tcp.quickack);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_SCTP) {
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.nodelay))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.nodelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CORK:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.cork))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.cork);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PPI:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.ppi))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.ppi);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SID:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.sid))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.sid);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SSN:
					/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
					   T_READONLY, the returned option value is the same as the 
					   one requested on input. */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_TSN:
					/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
					   T_READONLY, the returned option value is the same as the 
					   one requested on input. */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RECVOPT:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.recvopt))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.recvopt);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_LIFE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.cookie_life))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_life);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SACK_DELAY:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.sack_delay))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.sack_delay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_PATH_MAX_RETRANS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.path_max_retrans))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.path_max_retrans);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ASSOC_MAX_RETRANS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.assoc_max_retrans))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.assoc_max_retrans);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_INIT_RETRIES:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.max_init_retries))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.max_init_retries);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HEARTBEAT_ITVL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.heartbeat_itvl))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.heartbeat_itvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_INITIAL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.rto_initial))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto_initial);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MIN:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.rto_min))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto_min);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO_MAX:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.rto_max))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto_max);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_OSTREAMS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.ostreams))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.ostreams);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ISTREAMS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.istreams))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.istreams);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_COOKIE_INC:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.cookie_inc))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_inc);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_THROTTLE_ITVL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.throttle_itvl))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.throttle_itvl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAC_TYPE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.mac_type))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.mac_type);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_CKSUM_TYPE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.cksum_type))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.cksum_type);
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
				case T_SCTP_ECN:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.ecn))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.ecn);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
				case T_SCTP_ALI:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.ali))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.ali);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
				case T_SCTP_ADD:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.add))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.add);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.set))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.set);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_ADD_IP:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.add_ip))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.add_ip);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEL_IP:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.del_ip))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.del_ip);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_SET_IP:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.set_ip))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.set_ip);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_PR:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.pr))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.pr);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
				case T_SCTP_LIFETIME:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.lifetime))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.lifetime);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
				case T_SCTP_DISPOSITION:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.disposition))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.disposition);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAX_BURST:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.max_burst))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.max_burst);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_HB:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.hb))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.hb);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_RTO:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.rto))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.rto);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_MAXSEG:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.maxseg))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.maxseg);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_STATUS:
					/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
					   T_READONLY, the returned option value is the same as the 
					   one requested on input. */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_SCTP_DEBUG:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.sctp.debug))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.sctp.debug);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	ptrace(("%s: %p: Calculated option output size = %u\n", DRV_NAME, t, olen));
	return (olen);
      einval:
	ptrace(("%s: %p: ERROR: Invalid input options\n", DRV_NAME, t));
	return (-EINVAL);
}

/*
 *  Overall Option Result
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Calculates the overall T_OPTMGMT_ACK flag result from individual results.
 */
STATIC uint
t_overall_result(uint * overall, uint result)
{
	switch (result) {
	case T_NOTSUPPORT:
		if (!(*overall & (T_NOTSUPPORT)))
			*overall = T_NOTSUPPORT;
		break;
	case T_READONLY:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY)))
			*overall = T_READONLY;
		break;
	case T_FAILURE:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE)))
			*overall = T_FAILURE;
		break;
	case T_PARTSUCCESS:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS)))
			*overall = T_PARTSUCCESS;
		break;
	case T_SUCCESS:
		if (!
		    (*overall &
		     (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS | T_SUCCESS)))
			*overall = T_SUCCESS;
		break;
	}
	return (result);
}

/*
 *  Default Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT placing the output in the provided buffer.
 */
STATIC t_scalar_t
t_build_default_options(const ss_t *ss, const unsigned char *ip, size_t ilen, unsigned char *op,
			size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	struct sock *sk = (ss && ss->sock) ? ss->sock->sk : NULL;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t_defaults.xti.debug, T_OPT_DATA(oh),
				      sizeof(t_defaults.xti.debug));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				*((struct t_linger *) T_OPT_DATA(oh)) = t_defaults.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_rmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_wmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (ss->p.prot.family == PF_INET)
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
		case T_INET_IP:
			if (ss->p.prot.family == PF_INET) {
				struct inet_opt *np = inet_sk(sk);

				(void) np;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
				{
					oh->len = sizeof(*oh);
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					/* not supported yet */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_TOS:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.tos);
					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.tos;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_TTL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ttl);
					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.ttl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_REUSEADDR:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.reuseaddr);
					oh->level = T_INET_IP;
					oh->name = T_IP_REUSEADDR;
					oh->status = T_SUCCESS;
					*((unsigned int *) T_OPT_DATA(oh)) =
					    t_defaults.ip.reuseaddr;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_DONTROUTE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.dontroute);
					oh->level = T_INET_IP;
					oh->name = T_IP_DONTROUTE;
					oh->status = T_SUCCESS;
					*((unsigned int *) T_OPT_DATA(oh)) =
					    t_defaults.ip.dontroute;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_BROADCAST:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.broadcast);
					oh->level = T_INET_IP;
					oh->name = T_IP_BROADCAST;
					oh->status = T_SUCCESS;
					*((unsigned int *) T_OPT_DATA(oh)) =
					    t_defaults.ip.broadcast;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_ADDR:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.addr);
					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					oh->status = T_SUCCESS;
					*((uint32_t *) T_OPT_DATA(oh)) = t_defaults.ip.addr;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				if (ss->p.prot.protocol == T_INET_UDP
				    || ss->p.prot.protocol == T_INET_TCP
				    || ss->p.prot.protocol == T_INET_SCTP)
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.udp.checksum);
					oh->level = T_INET_UDP;
					oh->name = T_UDP_CHECKSUM;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.udp.checksum;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_TCP) {
				struct tcp_opt *tp = tcp_sk(sk);

				(void) tp;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.nodelay);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_NODELAY;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.nodelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_MAXSEG:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.maxseg);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_MAXSEG;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.maxseg;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPALIVE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepalive);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPALIVE;
					oh->status = T_SUCCESS;
					*((struct t_kpalive *) T_OPT_DATA(oh)) =
					    t_defaults.tcp.keepalive;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_CORK:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.cork);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_CORK;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.cork;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPIDLE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepidle);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPIDLE;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.keepidle;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPINTVL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepitvl);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPINTVL;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.keepitvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPCNT:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepcnt);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPCNT;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.keepcnt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_SYNCNT:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.syncnt);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_SYNCNT;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.syncnt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_LINGER2:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.linger2);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_LINGER2;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.linger2;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_DEFER_ACCEPT:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.defer_accept);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_DEFER_ACCEPT;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tcp.defer_accept;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_WINDOW_CLAMP:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.window_clamp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_WINDOW_CLAMP;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tcp.window_clamp;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_INFO:
					/* read only, can't get default */
					oh->len = sizeof(*oh);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_INFO;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_QUICKACK:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.quickack);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_QUICKACK;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.quickack;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_SCTP) {
				struct sctp_opt *sp = sctp_sk(sk);

				(void) sp;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.nodelay);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_NODELAY;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.nodelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAXSEG:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.maxseg);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAXSEG;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.maxseg;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_CORK:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cork);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CORK;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.cork;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PPI:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ppi);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PPI;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ppi;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SID:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.sid);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SID;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.sid;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SSN:
					oh->len = sizeof(*oh);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_TSN:
					oh->len = sizeof(*oh);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_TSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RECVOPT:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.recvopt);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RECVOPT;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.recvopt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_COOKIE_LIFE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cookie_life);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_LIFE;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.cookie_life;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SACK_DELAY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.sack_delay);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SACK_DELAY;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.sack_delay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PATH_MAX_RETRANS:
					oh->len =
					    _T_LENGTH_SIZEOF(t_defaults.sctp.path_max_retrans);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PATH_MAX_RETRANS;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.path_max_retrans;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ASSOC_MAX_RETRANS:
					oh->len =
					    _T_LENGTH_SIZEOF(t_defaults.sctp.assoc_max_retrans);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ASSOC_MAX_RETRANS;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.assoc_max_retrans;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAX_INIT_RETRIES:
					oh->len =
					    _T_LENGTH_SIZEOF(t_defaults.sctp.max_init_retries);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_INIT_RETRIES;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.max_init_retries;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_HEARTBEAT_ITVL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.heartbeat_itvl);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HEARTBEAT_ITVL;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.heartbeat_itvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_INITIAL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto_initial);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_INITIAL;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.rto_initial;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_MIN:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto_min);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MIN;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.rto_min;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_MAX:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto_max);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MAX;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.rto_max;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_OSTREAMS:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ostreams);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_OSTREAMS;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.ostreams;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ISTREAMS:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.istreams);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ISTREAMS;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.istreams;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_COOKIE_INC:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cookie_inc);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_INC;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.cookie_inc;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_THROTTLE_ITVL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.throttle_itvl);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_THROTTLE_ITVL;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.throttle_itvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAC_TYPE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.mac_type);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAC_TYPE;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.mac_type;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_CKSUM_TYPE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cksum_type);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CKSUM_TYPE;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.cksum_type;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_HB:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.hb);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HB;
					oh->status = T_SUCCESS;
					*((struct t_sctp_hb *) T_OPT_DATA(oh)) = t_defaults.sctp.hb;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO;
					oh->status = T_SUCCESS;
					*((struct t_sctp_rto *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.rto;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_STATUS:
					/* read-only */
					oh->len = sizeof(*oh);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_STATUS;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DEBUG:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.debug);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEBUG;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.debug;
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ECN:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ECN;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ecn);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ecn;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ALI:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ALI;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ali);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ali;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ADD:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.add);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.add;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SET:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.set);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.set;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ADD_IP:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD_IP;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.add_ip);
					oh->status = T_SUCCESS;
					*((struct sockaddr_in *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.add_ip;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DEL_IP:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEL_IP;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.del_ip);
					oh->status = T_SUCCESS;
					*((struct sockaddr_in *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.del_ip;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SET_IP:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET_IP;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.set_ip);
					oh->status = T_SUCCESS;
					*((struct sockaddr_in *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.set_ip;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PR:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PR;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.pr);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.pr;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_LIFETIME:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_LIFETIME;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.lifetime);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.lifetime;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DISPOSITION:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DISPOSITION;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.disposition);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.disposition;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAX_BURST:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_BURST;
					oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.max_burst);
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.sctp.max_burst;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  Current Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_CURRENT placing the output in the
 *  provided buffer.
 */
STATIC t_scalar_t
t_build_current_options(const ss_t *t, const unsigned char *ip, size_t ilen, unsigned char *op,
			size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	struct sock *sk = (t && t->sock) ? t->sock->sk : NULL;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t->options.xti.debug, T_OPT_DATA(oh),
				      4 * sizeof(t_uscalar_t));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_linger *) T_OPT_DATA(oh)) = t->options.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (t->p.prot.family == PF_INET)
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
		case T_INET_IP:
			if (t->p.prot.family == PF_INET) {
				struct inet_opt *np = inet_sk(sk);

				(void) np;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
				{
					oh->len = sizeof(*oh);
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					/* not supported yet */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_TOS:
					oh->len = _T_LENGTH_SIZEOF(t->options.ip.tos);
					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned char *) T_OPT_DATA(oh)) = t->options.ip.tos;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_TTL:
					oh->len = _T_LENGTH_SIZEOF(t->options.ip.ttl);
					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned char *) T_OPT_DATA(oh)) = t->options.ip.ttl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_REUSEADDR:
					oh->len = _T_LENGTH_SIZEOF(t->options.ip.reuseaddr);
					oh->level = T_INET_IP;
					oh->name = T_IP_REUSEADDR;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned int *) T_OPT_DATA(oh)) =
					    t->options.ip.reuseaddr;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_DONTROUTE:
					oh->len = _T_LENGTH_SIZEOF(t->options.ip.dontroute);
					oh->level = T_INET_IP;
					oh->name = T_IP_DONTROUTE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned int *) T_OPT_DATA(oh)) =
					    t->options.ip.dontroute;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_BROADCAST:
					oh->len = _T_LENGTH_SIZEOF(t->options.ip.broadcast);
					oh->level = T_INET_IP;
					oh->name = T_IP_BROADCAST;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned int *) T_OPT_DATA(oh)) =
					    t->options.ip.broadcast;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_ADDR:
					oh->len = _T_LENGTH_SIZEOF(t->options.ip.addr);
					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((uint32_t *) T_OPT_DATA(oh)) = t->options.ip.addr;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				if (t->p.prot.protocol == T_INET_UDP
				    || t->p.prot.protocol == T_INET_TCP
				    || t->p.prot.protocol == T_INET_SCTP)
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(t->options.udp.checksum);
					oh->level = T_INET_UDP;
					oh->name = T_UDP_CHECKSUM;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.udp.checksum;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				struct tcp_opt *tp = tcp_sk(sk);

				(void) tp;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.nodelay);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_NODELAY;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.nodelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_MAXSEG:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.maxseg);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_MAXSEG;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.maxseg;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPALIVE:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepalive);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPALIVE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct t_kpalive *) T_OPT_DATA(oh)) =
					    t->options.tcp.keepalive;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_CORK:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.cork);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_CORK;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.cork;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPIDLE:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepidle);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPIDLE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.keepidle;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPINTVL:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepitvl);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPINTVL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.keepitvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPCNT:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepcnt);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPCNT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.keepcnt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_SYNCNT:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.syncnt);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_SYNCNT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.syncnt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_LINGER2:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.linger2);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_LINGER2;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.linger2;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_DEFER_ACCEPT:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.defer_accept);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_DEFER_ACCEPT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.tcp.defer_accept;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_WINDOW_CLAMP:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.window_clamp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_WINDOW_CLAMP;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.tcp.window_clamp;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_INFO:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.info);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_INFO;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct t_tcp_info *) T_OPT_DATA(oh)) =
					    t->options.tcp.info;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_QUICKACK:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.quickack);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_QUICKACK;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.quickack;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_SCTP) {
				struct sctp_opt *sp = sctp_sk(sk);

				(void) sp;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.nodelay);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_NODELAY;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.nodelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAXSEG:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.maxseg);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAXSEG;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.maxseg;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_CORK:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cork);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CORK;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.cork;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PPI:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ppi);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PPI;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ppi;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SID:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.sid);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SID;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.sid;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SSN:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ssn);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SSN;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ssn;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_TSN:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.tsn);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_TSN;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.tsn;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RECVOPT:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.recvopt);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RECVOPT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.recvopt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_COOKIE_LIFE:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cookie_life);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_LIFE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.cookie_life;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SACK_DELAY:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.sack_delay);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SACK_DELAY;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.sack_delay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PATH_MAX_RETRANS:
					oh->len =
					    _T_LENGTH_SIZEOF(t->options.sctp.path_max_retrans);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PATH_MAX_RETRANS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.path_max_retrans;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ASSOC_MAX_RETRANS:
					oh->len =
					    _T_LENGTH_SIZEOF(t->options.sctp.assoc_max_retrans);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ASSOC_MAX_RETRANS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.assoc_max_retrans;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAX_INIT_RETRIES:
					oh->len =
					    _T_LENGTH_SIZEOF(t->options.sctp.max_init_retries);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_INIT_RETRIES;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.max_init_retries;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_HEARTBEAT_ITVL:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.heartbeat_itvl);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HEARTBEAT_ITVL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.heartbeat_itvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_INITIAL:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto_initial);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_INITIAL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.rto_initial;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_MIN:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto_min);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MIN;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.rto_min;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_MAX:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto_max);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MAX;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.rto_max;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_OSTREAMS:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ostreams);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_OSTREAMS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.ostreams;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ISTREAMS:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.istreams);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ISTREAMS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.istreams;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_COOKIE_INC:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cookie_inc);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_INC;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.cookie_inc;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_THROTTLE_ITVL:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.throttle_itvl);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_THROTTLE_ITVL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.throttle_itvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAC_TYPE:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.mac_type);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAC_TYPE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.mac_type;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_CKSUM_TYPE:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cksum_type);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CKSUM_TYPE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.cksum_type;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_HB:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.hb);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HB;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct t_sctp_hb *) T_OPT_DATA(oh)) = t->options.sctp.hb;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct t_sctp_rto *) T_OPT_DATA(oh)) =
					    t->options.sctp.rto;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_STATUS:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.status);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_STATUS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					bcopy(&t->options.sctp.status, T_OPT_DATA(oh),
					      sizeof(struct t_sctp_status) +
					      sizeof(struct t_sctp_dest_status));
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DEBUG:
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.debug);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEBUG;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.debug;
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ECN:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ECN;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ecn);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ecn;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ALI:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ALI;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ali);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ali;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ADD:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.add);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.add;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SET:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.set);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.set;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ADD_IP:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD_IP;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.add_ip);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct sockaddr_in *) T_OPT_DATA(oh)) =
					    t->options.sctp.add_ip;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DEL_IP:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEL_IP;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.del_ip);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct sockaddr_in *) T_OPT_DATA(oh)) =
					    t->options.sctp.del_ip;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SET_IP:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET_IP;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.set_ip);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct sockaddr_in *) T_OPT_DATA(oh)) =
					    t->options.sctp.set_ip;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PR:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PR;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.pr);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.pr;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_LIFETIME:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_LIFETIME;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.lifetime);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.lifetime;
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DISPOSITION:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DISPOSITION;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.disposition);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.disposition;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAX_BURST:
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_BURST;
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.max_burst);
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.sctp.max_burst;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  Check Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_CHECK placing the output in the provided
 *  buffer.
 */
STATIC t_scalar_t
t_build_check_options(const ss_t *ss, const unsigned char *ip, size_t ilen, unsigned char *op,
		      size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	struct sock *sk = (ss && ss->sock) ? ss->sock->sk : NULL;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				if (optlen) {
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!capable(CAP_NET_ADMIN)) {
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
					} else if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(oh), valp, optlen);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if ((valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					    || (valp->l_linger == T_UNSPEC
						&& valp->l_onoff != T_NO))
						goto einval;
					if (valp->l_linger == T_UNSPEC) {
						valp->l_linger = t_defaults.xti.linger.l_linger;
					}
					if (valp->l_linger == T_INFINITE) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->l_linger < 0)
						goto einval;
					if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > INT_MAX) {
						*valp = INT_MAX;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (ss->p.prot.family == PF_INET)
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
		case T_INET_IP:
			if (ss->p.prot.family == PF_INET) {
				struct inet_opt *np = inet_sk(sk);

				(void) np;
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					/* not supported yet */
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_TOS:
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					if (optlen) {
						unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);
						unsigned char prec, type;

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						prec = (*valp >> 5) & 0x7;
						type =
						    *valp & (T_LDELAY | T_HITHRPT | T_HIREL |
							     T_LOCOST);
						if (*valp != SET_TOS(prec, type))
							goto einval;
						if (prec >= T_CRITIC_ECP && !capable(CAP_NET_ADMIN))
							oh->status =
							    t_overall_result(&overall,
									     T_NOTSUPPORT);
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_TTL:
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					if (optlen) {
						unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp == 0) {
							*valp = sysctl_ip_default_ttl;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
#if 0
						if (*valp > 255) {
							*valp = 255;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
#endif
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_REUSEADDR:
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_REUSEADDR;
					oh->status = T_SUCCESS;
					if (optlen) {
						unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_DONTROUTE:
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_DONTROUTE;
					oh->status = T_SUCCESS;
					if (optlen) {
						unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_BROADCAST:
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_BROADCAST;
					oh->status = T_SUCCESS;
					if (optlen) {
						unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_IP_ADDR:
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					oh->status = T_SUCCESS;
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				if (ss->p.prot.protocol == T_INET_UDP
				    || ss->p.prot.protocol == T_INET_TCP
				    || ss->p.prot.protocol == T_INET_SCTP)
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					oh->len = ih->len;
					oh->level = T_INET_UDP;
					oh->name = T_UDP_CHECKSUM;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_TCP) {
				struct tcp_opt *tp = tcp_sk(sk);

				(void) tp;
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_NODELAY;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_MAXSEG:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_MAXSEG;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 8) {
							*valp = 8;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_WINDOW) {
							*valp = MAX_TCP_WINDOW;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPALIVE:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPALIVE;
					oh->status = T_SUCCESS;
					if (optlen) {
						struct t_kpalive *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (valp->kp_onoff != T_YES
						    && valp->kp_onoff != T_NO)
							goto einval;
						if (valp->kp_timeout == T_UNSPEC)
							valp->kp_timeout =
							    t_defaults.tcp.keepalive.kp_timeout;
						if (valp->kp_timeout < 0)
							goto einval;
						if (valp->kp_timeout >
						    MAX_SCHEDULE_TIMEOUT / 60 / HZ) {
							valp->kp_timeout =
							    MAX_SCHEDULE_TIMEOUT / 60 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->kp_onoff == T_YES && valp->kp_timeout < 1) {
							valp->kp_timeout = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_CORK:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_CORK;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPIDLE:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPIDLE;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_KEEPIDLE) {
							*valp = MAX_TCP_KEEPIDLE;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPINTVL:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPINTVL;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_KEEPINTVL) {
							*valp = MAX_TCP_KEEPINTVL;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPCNT:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPCNT;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_KEEPCNT) {
							*valp = MAX_TCP_KEEPCNT;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_SYNCNT:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_SYNCNT;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_SYNCNT) {
							*valp = MAX_TCP_SYNCNT;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_LINGER2:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_LINGER2;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_INFINITE) {
							if (*valp > sysctl_tcp_fin_timeout / HZ)
								*valp = 0;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_DEFER_ACCEPT:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_DEFER_ACCEPT;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp == T_INFINITE
						    || *valp > ((TCP_TIMEOUT_INIT / HZ) << 31)) {
							*valp = (TCP_TIMEOUT_INIT / HZ) << 31;
						} else if (*valp != 0) {
							int count;

							for (count = 0;
							     count < 32
							     && *valp >
							     ((TCP_TIMEOUT_INIT / HZ) << count);
							     count++) ;
							count++;
							if (*valp !=
							    ((TCP_TIMEOUT_INIT / HZ) << count)) {
								*valp =
								    (TCP_TIMEOUT_INIT /
								     HZ) << count;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_WINDOW_CLAMP:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_WINDOW_CLAMP;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < SOCK_MIN_RCVBUF / 2) {
							*valp = SOCK_MIN_RCVBUF / 2;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_INFO:
					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_INFO;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_QUICKACK:
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_QUICKACK;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (ss->p.prot.family == PF_INET && ss->p.prot.protocol == T_INET_SCTP) {
				struct sctp_opt *sp = sctp_sk(sk);

				(void) sp;
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_NODELAY;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAXSEG:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAXSEG;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_WINDOW) {
							*valp = MAX_TCP_WINDOW;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_CORK:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CORK;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_NO && *valp != T_YES)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PPI:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PPI;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SID:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SID;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp > 0x0000ffff)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SSN:
					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_TSN:
					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_TSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RECVOPT:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RECVOPT;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_COOKIE_LIFE:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_LIFE;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
							*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SACK_DELAY:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SACK_DELAY;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > 500) {
							*valp = 500;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PATH_MAX_RETRANS:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PATH_MAX_RETRANS;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ASSOC_MAX_RETRANS:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ASSOC_MAX_RETRANS;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAX_INIT_RETRIES:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_INIT_RETRIES;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_HEARTBEAT_ITVL:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HEARTBEAT_ITVL;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_INITIAL:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_INITIAL;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
							*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_MIN:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MIN;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
							*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO_MAX:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MAX;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
							*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_OSTREAMS:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_OSTREAMS;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > 0x00010000) {
							*valp = 0x00010000;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ISTREAMS:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ISTREAMS;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > 0x00010000) {
							*valp = 0x00010000;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_COOKIE_INC:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_INC;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
							*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_THROTTLE_ITVL:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_THROTTLE_ITVL;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp < 1000 / HZ) {
							*valp = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
							*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAC_TYPE:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAC_TYPE;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_CKSUM_TYPE:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CKSUM_TYPE;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_HB:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HB;
					oh->status = T_SUCCESS;
					if (optlen) {
						struct t_sctp_hb *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (valp->hb_onoff != T_YES
						    && valp->hb_onoff != T_NO)
							goto einval;
						if (valp->hb_itvl == T_UNSPEC)
							valp->hb_itvl = t_defaults.sctp.hb.hb_itvl;
						if (valp->hb_itvl < 0)
							goto einval;
						if (valp->hb_itvl / HZ >
						    MAX_SCHEDULE_TIMEOUT / 1000) {
							valp->hb_itvl =
							    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->hb_onoff == T_YES && valp->hb_itvl < 1) {
							valp->hb_itvl = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_RTO:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO;
					oh->status = T_SUCCESS;
					if (optlen) {
						struct t_sctp_rto *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (valp->rto_initial < valp->rto_min
						    || valp->rto_initial > valp->rto_max)
							goto einval;
						if (valp->rto_initial < 1000 / HZ) {
							valp->rto_initial = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->rto_min < 1000 / HZ) {
							valp->rto_min = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->rto_max < 1000 / HZ) {
							valp->rto_max = 1000 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->rto_initial / HZ >
						    MAX_SCHEDULE_TIMEOUT / 1000) {
							valp->rto_initial =
							    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->rto_min / HZ >
						    MAX_SCHEDULE_TIMEOUT / 1000) {
							valp->rto_min =
							    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->rto_max / HZ >
						    MAX_SCHEDULE_TIMEOUT / 1000) {
							valp->rto_max =
							    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_STATUS:
					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_STATUS;
					oh->status = t_overall_result(&overall, T_READONLY);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DEBUG:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEBUG;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ECN:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ECN;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ALI:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ALI;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ADD:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SET:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_ADD_IP:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD_IP;
					oh->status = T_SUCCESS;
					if (optlen) {
						struct sockaddr_in *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DEL_IP:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEL_IP;
					oh->status = T_SUCCESS;
					if (optlen) {
						struct sockaddr_in *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_SET_IP:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET_IP;
					oh->status = T_SUCCESS;
					if (optlen) {
						struct sockaddr_in *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_PR:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PR;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_LIFETIME:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_LIFETIME;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (!*valp || *valp == T_UNSPEC)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_DISPOSITION:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DISPOSITION;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						switch (*valp) {
						case T_SCTP_DISPOSITION_NONE:
						case T_SCTP_DISPOSITION_UNSENT:
						case T_SCTP_DISPOSITION_SENT:
						case T_SCTP_DISPOSITION_GAP_ACKED:
						case T_SCTP_DISPOSITION_ACKED:
							break;
						default:
							goto einval;
						}
						(void) valp;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_MAX_BURST:
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_BURST;
					oh->status = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

#ifndef HAVE___TCP_PUSH_PENDING_FRAMES_EXPORT
#ifdef  HAVE___TCP_PUSH_PENDING_FRAMES_ADDR
#ifdef HAVE_OLD_SOCK_STRUCTURE
void
__tcp_push_pending_frames(struct sock *sk, struct tcp_opt *tp, unsigned int cur_mss, int nonagle)
{
	void (*func) (struct sock * sk, struct tcp_opt * tp, unsigned int cur_mss, int nonagle) =
	    (typeof(func)) HAVE___TCP_PUSH_PENDING_FRAMES_ADDR;
	return (*func) (sk, tp, cur_mss, nonagle);
}
#else
void
__tcp_push_pending_frames(struct sock *sk, struct tcp_sock *tp, unsigned int cur_mss, int nonagle)
{
	void (*func) (struct sock * sk, struct tcp_opt * tp, unsigned int cur_mss, int nonagle) =
	    (typeof(func)) HAVE___TCP_PUSH_PENDING_FRAMES_ADDR;
	return (*func) (sk, tp, cur_mss, nonagle);
}
#endif
#endif
#endif

/*
 *  Process Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and
 *  T_NEGOTIARE, placing the output in the provided buffer.
 */
STATIC t_scalar_t
t_build_negotiate_options(ss_t *t, const unsigned char *ip, size_t ilen, unsigned char *op,
			  size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	struct sock *sk = (t && t->sock) ? t->sock->sk : NULL;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.debug[0];
					} else {
						bcopy(T_OPT_DATA(ih), valp, optlen);
					}
					if (!capable(CAP_NET_ADMIN)) {	/* XXX */
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
					} else {
						bzero(t->options.xti.debug,
						      sizeof(t->options.xti.debug));
						if (oh->len > sizeof(*oh))
							bcopy(valp, t->options.xti.debug,
							      oh->len - sizeof(*oh));
						if (t->options.xti.debug[0] & 0x01)
							sock_set_debug(sk);
						else
							sock_clr_debug(sk);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.linger;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if ((valp->l_onoff != T_NO
						     && valp->l_onoff != T_YES)
						    || (valp->l_linger == T_UNSPEC
							&& valp->l_onoff != T_NO))
							goto einval;
						if (valp->l_linger == T_UNSPEC) {
							valp->l_linger =
							    t_defaults.xti.linger.l_linger;
						}
						if (valp->l_linger == T_INFINITE) {
							valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->l_linger < 0)
							goto einval;
						if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
							valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.linger = *valp;
					if (valp->l_onoff) {
						sock_set_linger(sk);
						sk->sk_lingertime = valp->l_linger * HZ;
					} else {
						sock_clr_linger(sk);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = sysctl_rmem_default;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > sysctl_rmem_max) {
							*valp = sysctl_rmem_max;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp < SOCK_MIN_RCVBUF / 2) {
							*valp = SOCK_MIN_RCVBUF / 2;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.rcvbuf = *valp;
					sk->sk_rcvbuf = *valp << 1;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.rcvlowat;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > INT_MAX) {
							*valp = INT_MAX;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp <= 0) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.rcvlowat = *valp;
					sk->sk_rcvlowat = *valp;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = sysctl_wmem_default;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > sysctl_rmem_max) {
							*valp = sysctl_rmem_max;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp < SOCK_MIN_SNDBUF / 2) {
							*valp = SOCK_MIN_SNDBUF / 2;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.sndbuf = *valp;
					sk->sk_sndbuf = *valp << 1;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.sndlowat;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp <= 0) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.sndlowat = *valp;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (t->p.prot.family == PF_INET)
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
		case T_INET_IP:
			if (t->p.prot.family == PF_INET) {
				struct inet_opt *np = inet_sk(sk);

				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
				{
					/* not supported yet */
					oh->len = ih->len;
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_TOS:
				{
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.ip.tos;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							{
								unsigned char prec =
								    (*valp >> 5) & 0x7;
								unsigned char type =
								    *valp & (T_LDELAY | T_HITHRPT |
									     T_HIREL | T_LOCOST);

								if (*valp != SET_TOS(prec, type))
									goto einval;
								if (prec >= T_CRITIC_ECP
								    && !capable(CAP_NET_ADMIN)) {
									oh->status =
									    t_overall_result
									    (&overall,
									     T_NOTSUPPORT);
									break;
								}
							}
						}
						t->options.ip.tos = *valp;
						if (np->tos != t->options.ip.tos) {
							np->tos = *valp;
							sk->sk_priority = rt_tos2priority(*valp);
							sk_dst_reset(sk);
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_TTL:
				{
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.ip.ttl;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp == 0) {
								*valp = sysctl_ip_default_ttl;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
							if (*valp < 1) {
								*valp = 1;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
#if 0
							if (*valp > 255) {
								*valp = 255;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
#endif
						}
						t->options.ip.ttl = *valp;
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_TTL
						np->ttl = *valp;
#else
#ifdef HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
						np->uc_ttl = *valp;
#endif
#endif				/* defined HAVE_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_REUSEADDR:
				{
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_IP;
					oh->name = T_IP_REUSEADDR;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.ip.reuseaddr;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp != T_YES && *valp != T_NO)
								goto einval;
						}
						t->options.ip.reuseaddr = *valp;
						sk->sk_reuse = (*valp == T_YES) ? 1 : 0;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_DONTROUTE:
				{
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_IP;
					oh->name = T_IP_DONTROUTE;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.ip.dontroute;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp != T_YES && *valp != T_NO)
								goto einval;
						}
						t->options.ip.dontroute = *valp;
						if (*valp == T_YES)
							sock_set_localroute(sk);
						else
							sock_clr_localroute(sk);
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_BROADCAST:
				{
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_IP;
					oh->name = T_IP_BROADCAST;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.ip.broadcast;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp != T_YES && *valp != T_NO)
								goto einval;
						}
						t->options.ip.broadcast = *valp;
						if (*valp == T_YES)
							sock_set_broadcast(sk);
						else
							sock_clr_broadcast(sk);
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_IP_ADDR:
				{
					uint32_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.ip.addr;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
						}
						t->options.ip.addr = *valp;
						sock_saddr(sk) = *valp;
					}
					if (ih->name != T_ALLOPT)
						continue;
				}
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				if (t->p.prot.protocol == T_INET_UDP
				    || t->p.prot.protocol == T_INET_TCP
				    || t->p.prot.protocol == T_INET_SCTP)
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_UDP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_UDP) {
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_UDP;
					oh->name = T_UDP_CHECKSUM;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.udp.checksum;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp != T_YES && *valp != T_NO)
								goto einval;
						}
						t->options.udp.checksum = *valp;
						sk->sk_no_check =
						    (*valp ==
						     T_YES) ? UDP_CSUM_DEFAULT : UDP_CSUM_NOXMIT;
					}
					if (ih->name != T_ALLOPT)
						continue;
				}
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				struct tcp_opt *tp = tcp_sk(sk);

				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_NODELAY;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.nodelay;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp != T_YES && *valp != T_NO)
								goto einval;
						}
						t->options.tcp.nodelay = *valp;
						if ((tp->nonagle = (*valp == T_YES) ? 1 : 0))
							tcp_push_pending_frames(sk, tp);
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_MAXSEG:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_MAXSEG;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.maxseg;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp < 8) {
								*valp = 8;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
							if (*valp > MAX_TCP_WINDOW) {
								*valp = MAX_TCP_WINDOW;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
						}
						t->options.tcp.maxseg = *valp;
						tcp_user_mss(tp) = *valp;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_KEEPALIVE:
				{
					struct t_kpalive *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPALIVE;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.keepalive;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (valp->kp_onoff != T_YES
							    && valp->kp_onoff != T_NO)
								goto einval;
							if (valp->kp_timeout == T_UNSPEC) {
								valp->kp_timeout =
								    t_defaults.tcp.keepalive.
								    kp_timeout;
							}
							if (valp->kp_timeout < 0)
								goto einval;
							if (valp->kp_timeout >
							    MAX_SCHEDULE_TIMEOUT / 60 / HZ) {
								valp->kp_timeout =
								    MAX_SCHEDULE_TIMEOUT / 60 / HZ;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
							if (valp->kp_onoff == T_YES
							    && valp->kp_timeout < 1) {
								valp->kp_timeout = 1;
								oh->status =
								    t_overall_result(&overall,
										     T_PARTSUCCESS);
							}
						}
						t->options.tcp.keepalive = *valp;
						if (valp->kp_onoff)
							tp->keepalive_time =
							    valp->kp_timeout * 60 * HZ;
#if defined HAVE_TCP_SET_KEEPALIVE_ADDR
						tcp_set_keepalive(sk,
								  (valp->kp_onoff ==
								   T_YES) ? 1 : 0);
#endif				/* defined HAVE_TCP_SET_KEEPALIVE_ADDR */
						if (valp->kp_onoff == T_YES)
							sock_set_keepopen(sk);
						else
							sock_clr_keepopen(sk);
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_CORK:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_CORK;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.cork;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.cork = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_KEEPIDLE:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPIDLE;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.keepidle;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.keepidle = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_KEEPINTVL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPINTVL;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.keepitvl;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.keepitvl = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_KEEPCNT:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPCNT;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.keepcnt;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.keepcnt = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_SYNCNT:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_SYNCNT;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.syncnt;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.syncnt = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_LINGER2:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_LINGER2;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.linger2;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.linger2 = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_DEFER_ACCEPT:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_DEFER_ACCEPT;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.defer_accept;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.defer_accept = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_WINDOW_CLAMP:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_WINDOW_CLAMP;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.window_clamp;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.window_clamp = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_INFO:
				{
					struct t_tcp_info *valp = (typeof(valp)) T_OPT_DATA(oh);

					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_TCP;
					oh->name = T_TCP_INFO;
					oh->status = t_overall_result(&overall, T_READONLY);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_TCP_QUICKACK:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_QUICKACK;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.tcp.quickack;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.tcp.quickack = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
				}
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#if defined HAVE_OPENSS7_SCTP
		case T_INET_SCTP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_SCTP) {
				struct sctp_opt *sp = sctp_sk(sk);

				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_SCTP_NODELAY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_NODELAY;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.nodelay;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							if (*valp != T_YES && *valp != T_NO)
								goto einval;
						}
						t->options.sctp.nodelay = *valp;
						sp->nonagle = (*valp == T_YES) ? 1 : 0;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_MAXSEG:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAXSEG;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.maxseg;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.maxseg = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_CORK:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CORK;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.cork;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.cork = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_PPI:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PPI;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.ppi;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.ppi = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_SID:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SID;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.sid;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.sid = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_SSN:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_TSN:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_TSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_RECVOPT:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RECVOPT;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.recvopt;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.recvopt = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_COOKIE_LIFE:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_LIFE;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.cookie_life;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.cookie_life = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_SACK_DELAY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SACK_DELAY;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.sack_delay;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.sack_delay = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_PATH_MAX_RETRANS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PATH_MAX_RETRANS;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.path_max_retrans;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.path_max_retrans = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_ASSOC_MAX_RETRANS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ASSOC_MAX_RETRANS;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.assoc_max_retrans;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.assoc_max_retrans = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_MAX_INIT_RETRIES:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_INIT_RETRIES;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.max_init_retries;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.max_init_retries = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_HEARTBEAT_ITVL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HEARTBEAT_ITVL;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.heartbeat_itvl;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.heartbeat_itvl = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_RTO_INITIAL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_INITIAL;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.rto_initial;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.rto_initial = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_RTO_MIN:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MIN;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.rto_min;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.rto_min = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_RTO_MAX:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO_MAX;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.rto_max;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.rto_max = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_OSTREAMS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_OSTREAMS;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.ostreams;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.ostreams = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_ISTREAMS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ISTREAMS;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.istreams;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.istreams = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_COOKIE_INC:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_COOKIE_INC;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.cookie_inc;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.cookie_inc = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_THROTTLE_ITVL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_THROTTLE_ITVL;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.throttle_itvl;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.throttle_itvl = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_MAC_TYPE:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAC_TYPE;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.mac_type;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.mac_type = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_CKSUM_TYPE:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_CKSUM_TYPE;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.cksum_type;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.cksum_type = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_HB:
				{
					struct t_sctp_hb *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_HB;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.hb;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.hb = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_RTO:
				{
					struct t_sctp_rto *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_RTO;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.rto;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.rto = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_STATUS:
				{
					struct {
						struct t_sctp_status status;
						struct t_sctp_dest_status dest_status;
					} *valp = (typeof(valp)) T_OPT_DATA(oh);

					/* read-only */
					oh->len = ih->len;
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_STATUS;
					oh->status = t_overall_result(&overall, T_READONLY);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_DEBUG:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEBUG;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!sk) {
						oh->status = t_overall_result(&overall, T_FAILURE);
					} else {
						if (ih->name == T_ALLOPT) {
							*valp = t_defaults.sctp.debug;
						} else {
							*valp = *((typeof(valp)) T_OPT_DATA(ih));
							/* negotiate value */
						}
						t->options.sctp.debug = *valp;
						/* set value on socket or stream */
					}
					if (ih->name != T_ALLOPT)
						continue;
#if defined CONFIG_SCTP_ECN
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_ECN:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ECN;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.ecn;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.ecn = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ECN */
#if defined CONFIG_SCTP_ADD_IP || defined CONFIG_SCTP_ADAPTATION_LAYER_INFO
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_ALI:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ALI;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.ali;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.ali = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP || defined
				   CONFIG_SCTP_ADAPTATION_LAYER_INFO */
#if defined CONFIG_SCTP_ADD_IP
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_ADD:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.add;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.add = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_SET:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.set;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.set = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_ADD_IP:
				{
					struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_ADD_IP;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.add_ip;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.add_ip = *valp;
					/* set value on socket or stream */
					sctp_add_ip(t, valp);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_DEL_IP:
				{
					struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DEL_IP;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.del_ip;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.del_ip = *valp;
					/* set value on socket or stream */
					sctp_del_ip(t, valp);
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_SET_IP:
				{
					struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SET_IP;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.set_ip;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.set_ip = *valp;
					/* set value on socket or stream */
					sctp_set_ip(t, valp);
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_ADD_IP */
#if defined CONFIG_SCTP_PARTIAL_RELIABILITY
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_PR:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_PR;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.pr;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.pr = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_PARTIAL_RELIABILITY */
#if defined CONFIG_SCTP_LIFETIMES || defined CONFIG_SCTP_PARTIAL_RELIABILITY
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_LIFETIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_LIFETIME;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.lifetime;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.lifetime = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
#endif				/* defined CONFIG_SCTP_LIFETIMES || defined
				   CONFIG_SCTP_PARTIAL_RELIABILITY */
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_DISPOSITION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_DISPOSITION;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.disposition;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.disposition = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
				case T_SCTP_MAX_BURST:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_MAX_BURST;
					oh->status = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.max_burst;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.max_burst = *valp;
					/* set value on socket or stream */
					if (ih->name != T_ALLOPT)
						continue;
				}
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif				/* defined HAVE_OPENSS7_SCTP */
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  Process Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and
 *  T_NEGOTIARE, placing the output in the provided buffer.
 */
STATIC t_scalar_t
t_build_options(ss_t *t, unsigned char *ip, size_t ilen, unsigned char *op, size_t *olen,
		t_scalar_t flag)
{
	switch (flag) {
	case T_DEFAULT:
		return t_build_default_options(t, ip, ilen, op, olen);
	case T_CURRENT:
		return t_build_current_options(t, ip, ilen, op, olen);
	case T_CHECK:
		return t_build_check_options(t, ip, ilen, op, olen);
	case T_NEGOTIATE:
		return t_build_negotiate_options(t, ip, ilen, op, olen);
	}
	return (-EINVAL);
}

/*
 *  =========================================================================
 *
 *  Address Handling
 *
 *  =========================================================================
 */
STATIC INLINE streams_fastcall __hot int
ss_addr_size(ss_t *ss, struct sockaddr *add)
{
	if (add) {
		switch (add->sa_family) {
		case AF_INET:
			return sizeof(struct sockaddr_in);
		case AF_UNIX:
			return sizeof(struct sockaddr_un);
		case AF_UNSPEC:
			return sizeof(add->sa_family);
		default:
			return sizeof(struct sockaddr);
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */
#ifdef _DEBUG
STATIC const char *
state_name(t_scalar_t state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif
STATIC INLINE streams_fastcall __hot void
ss_set_state(ss_t *ss, t_scalar_t state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, ss, state_name(state),
		state_name(ss->p.info.CURRENT_state)));
	ss->p.info.CURRENT_state = state;
}

STATIC INLINE streams_fastcall __hot t_scalar_t
ss_get_state(ss_t *ss)
{
	return (ss->p.info.CURRENT_state);
}

#ifdef _DEBUG
STATIC const char *
tcp_state_name(int state)
{
	switch (state) {
	case TCP_ESTABLISHED:
		return ("TCP_ESTABLISHED");
	case TCP_SYN_SENT:
		return ("TCP_SYN_SENT");
	case TCP_SYN_RECV:
		return ("TCP_SYN_RECV");
	case TCP_FIN_WAIT1:
		return ("TCP_FIN_WAIT1");
	case TCP_FIN_WAIT2:
		return ("TCP_FIN_WAIT2");
	case TCP_TIME_WAIT:
		return ("TCP_TIME_WAIT");
	case TCP_CLOSE:
		return ("TCP_CLOSE");
	case TCP_CLOSE_WAIT:
		return ("TCP_CLOSE_WAIT");
	case TCP_LAST_ACK:
		return ("TCP_LAST_ACK");
	case TCP_LISTEN:
		return ("TCP_LISTEN");
	case TCP_CLOSING:
		return ("TCP_CLOSING");
	case TCP_MAX_STATES:
		return ("TCP_MAX_STATES");
	default:
		return ("(unknown)");
	}
}
#endif

/*
 *  ------------------------------------------------------------------------
 *
 *  Socket Calls
 *
 *  ------------------------------------------------------------------------
 *  These are wrappered versions of socket calls.
 */
/*
 *  SOCKET CREATE
 *  ------------------------------------------------------------------------
 */
STATIC int
ss_socket(ss_t *ss)
{
	int err;
	int family, type, protocol;

	ensure(ss, return (-EFAULT));
	ensure(!ss->sock, return (-EPROTO));
	family = ss->p.prot.family;
	type = ss->p.prot.type;
	protocol = (ss->p.prot.protocol == IPPROTO_RAW) ? ss->port : ss->p.prot.protocol;
	printd(("%s: %p: SS_CREATE %d:%d:%d\n", DRV_NAME, ss, family, type, protocol));
	if (!(err = sock_create(family, type, protocol, &ss->sock))) {
		ensure(ss->sock, return (-EFAULT));
		ensure(ss->sock->sk, return (-EFAULT));
		ss->sock->sk->sk_allocation = GFP_ATOMIC;
		ss_socket_get(ss->sock, ss);
		if (ss->p.prot.family == PF_INET)
			inet_sk(ss->sock->sk)->cmsg_flags |= 0x0f;
		return (0);
	}
	printd(("%s: %p: ERROR: from sock_create %d\n", DRV_NAME, ss, err));
	return (err);
}

/*
 *  SOCKET INIT
 *  ------------------------------------------------------------------------
 *  For all but sockets of type IPPROTO_RAW, we can create the socket at
 *  initialization time.  For IPPROTO_RAW sockets, we cannot create the socket
 *  until they have been bound to a protocol.  Those are deferred until bind.
 */
STATIC int
ss_sock_init(ss_t *ss)
{
	if (ss->p.prot.protocol == IPPROTO_RAW)
		return (0);
	return ss_socket(ss);
}

/*
 *  SOCKET BIND
 *  ------------------------------------------------------------------------
 *  We create IPPROTO_RAW sockets at bind time.  All other sockets are
 *  normally created at initialization (open/unbind) time, but we will create
 *  them here now if an earlier reinitialization failed.
 */
STATIC int
ss_bind(ss_t *ss, struct sockaddr *add, size_t add_len)
{
	int err;

	ensure(ss, return (-EFAULT));
	if (ss->sock || !(err = ss_socket(ss))) {
		ensure(ss->sock, return (-EFAULT));
		ensure(ss->sock->sk, return (-EFAULT));
		ensure(ss->sock->ops, return (-EFAULT));
		ensure(ss->sock->ops->bind, return (-EFAULT));
		printd(("%s: %p: SS_BIND\n", DRV_NAME, ss));
		if (!(err = ss->sock->ops->bind(ss->sock, add, add_len))) {
			ss->src = *add;
			return (0);
		} else
			printd(("%s: %p: ERROR: from sock->ops->bind %d\n", DRV_NAME, ss, err));
		switch (-err) {
		case EADDRINUSE:
			err = TADDRBUSY;
			break;
		case EADDRNOTAVAIL:
		case EDESTADDRREQ:
		case ESOCKTNOSUPPORT:
			err = TNOADDR;
			break;
		case EAFNOSUPPORT:
		case ENAMETOOLONG:
		case ELOOP:
		case ENOENT:
		case ENOTDIR:
		case EROFS:
			err = TBADADDR;
			break;
		case EINVAL:
		case EISCONN:
			err = TOUTSTATE;
			break;
		case EOPNOTSUPP:
			err = TNOTSUPPORT;
			break;
		case EACCES:
			err = TACCES;
			break;
		}
	}
	return (err);
}

/*
 *  SOCKET LISTEN
 *  ------------------------------------------------------------------------
 */
STATIC int
ss_listen(ss_t *ss, uint cons)
{
	int err;
	int type;

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->sk, return (-EFAULT));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->listen, return (-EFAULT));
	type = ss->p.prot.type;
	ensure(type == SOCK_STREAM || type == SOCK_SEQPACKET, return (-EFAULT));
	printd(("%s: %p: SS_LISTEN %d\n", DRV_NAME, ss, cons));
	if (!(err = ss->sock->ops->listen(ss->sock, cons))) {
		ss->conind = cons;
		ss->tcp_state = ss->sock->sk->sk_state;
		return (0);
	}
	printd(("%s: %p: ERROR: from sock->ops->listen %d\n", DRV_NAME, ss, err));
	switch (-err) {
	case EDESTADDRREQ:
	case ESOCKTNOSUPPORT:
		err = TNOADDR;
		break;
	case EINVAL:
		err = TOUTSTATE;
		break;
	case EOPNOTSUPP:
		err = TNOTSUPPORT;
		break;
	case EACCES:
		err = TACCES;
		break;
	}
	return (err);
}

/*
 *  SOCKET ACCEPT
 *  ------------------------------------------------------------------------
 *  Unfortunately, sock->ops->accept will only accept the next sequential
 *  connection indication.  In TLI's case, we want to be able to accept or
 *  release connection indications other than the next sequential indication.
 *  To do this we must muck with TCP's accept queue when the SEQ_number is not
 *  the next in the queue.  To do this we mimic some of the tcp_accept
 *  behavior.
 */
STATIC int
ss_accept(ss_t *ss, struct socket **newsock, mblk_t *cp)
{
	struct socket *sock;

#ifdef HAVE_SOCK_ALLOC_ADDR
	struct socket *(*sock_alloc) (void) = (typeof(sock_alloc)) HAVE_SOCK_ALLOC_ADDR;
#endif
	ensure(newsock, return (-EFAULT));
	ensure(cp, return (-EFAULT));
	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->sk, return (-EFAULT));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->accept, return (-EFAULT));
	printd(("%s: %p: SS_ACCEPT\n", DRV_NAME, ss));
	if ((sock = sock_alloc())) {
		struct sock *sk = ss->sock->sk;
		struct open_request *req, *req_prev, **reqp;
		struct sock *ask = ((ss_event_t *) cp->b_rptr)->sk;

		sock->type = ss->sock->type;
		sock->ops = ss->sock->ops;
		lock_sock(sk);
		if (sock_accept_queue_head(sk)) {
			sock_accept_queue_lock(sk);
			/* find connection in queue */
			for (reqp = &sock_accept_queue_head(sk), req_prev = NULL;
			     *reqp && (*reqp)->sk != ask;
			     req_prev = (*reqp), reqp = &(*reqp)->dl_next) ;
			if ((req = *reqp)) {
				if (!((*reqp) = (*reqp)->dl_next))
					sock_accept_queue_tail(sk) = req_prev;
				sk->sk_ack_backlog--;
				tcp_openreq_fastfree(req);
			}
			sock_accept_queue_unlock(sk);
			release_sock(sk);
			lock_sock(ask);
			sock_graft(ask, sock);
			release_sock(ask);
			sock->state = SS_CONNECTED;
			*newsock = sock;
			bufq_unlink(&ss->conq, cp);
			freemsg(cp);
			return (0);
		}
		release_sock(sk);
		ss_socket_put(sock);
		printd(("%s: %p: invalid accept\n", DRV_NAME, ss));
		return (-EAGAIN);
	}
	printd(("%s: %p: ERROR: couldn't allocate accepting socket\n", DRV_NAME, ss));
	return (-EFAULT);
}

/*
 *  SOCKET UNBIND
 *  ------------------------------------------------------------------------
 *  There is no good way to unbind and rebind a socket in Linux, so we just
 *  close the entire socket.  The next time we go to bind, we will create a
 *  fresh IPPROTO_RAW socket to bind.  All other sockets are recreated now.
 *
 *  Actually, socket code is supposed to allow a bind to AF_UNSPEC to unbind a
 *  socket.  This is X/Open POSIX behavior.
 */
STATIC int
ss_unbind(ss_t *ss)
{
	ensure(ss, return (-EFAULT));
	if (ss->sock) {
		ensure(ss->sock->sk, return (-EFAULT));
		printd(("%s: %p: SS_UNBIND\n", DRV_NAME, ss));
		ss_socket_put(xchg(&ss->sock, NULL));
	}
	/* Note: if the reinitialization of the socket fails, we will attempt again to reinitialize 
	   it when an attempt is ever made to rebind the socket.  We just want to have an
	   initialized socket around so that we can effect options management on the socket in the
	   unbound state, if possible. */
	ss_sock_init(ss);
	return (0);
}

/*
 *  SOCKET CONNECT
 *  ------------------------------------------------------------------------
 */
STATIC int
ss_connect(ss_t *ss, struct sockaddr *dst)
{
	int err;

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->sk, return (-EFAULT));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->connect, return (-EFAULT));
	if ((err = ss->sock->ops->connect(ss->sock, dst, sizeof(*dst), O_NONBLOCK)) == 0
	    || err == -EINPROGRESS) {
		return (0);
	}
	printd(("%s: %p: ERROR: from sock->ops->connect %d\n", DRV_NAME, ss, err));
	return (err);
}

/*
 *  SOCKET SENDMSG
 *  ------------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_out int
ss_sendmsg(ss_t *ss, struct msghdr *msg, int len)
{
	int res;

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->sendmsg, return (-EFAULT));
	ensure(ss->sock->sk, return (-EFAULT));
	ensure(ss->sock->sk->sk_prot, return (-EFAULT));
	ensure(ss->sock->sk->sk_prot->sendmsg, return (-EFAULT));
	{
		mm_segment_t fs = get_fs();

		set_fs(KERNEL_DS);
		res = sock_sendmsg(ss->sock, msg, len);
		set_fs(fs);
	}
	if (res <= 0)
		printd(("%s: %p: ERROR: from sock->sk->sk_prot->sendmsg %d\n", DRV_NAME, ss, res));
	return (res);
}

/*
 *  SOCKET RECVMSG
 *  ------------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_in int
ss_recvmsg(ss_t *ss, struct msghdr *msg, int size)
{
	int res;
	int sflags = MSG_DONTWAIT | MSG_NOSIGNAL;

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->recvmsg, return (-EFAULT));
	ensure(ss->sock->sk, return (-EFAULT));
	ensure(ss->sock->sk->sk_prot, return (-EFAULT));
	ensure(ss->sock->sk->sk_prot->recvmsg, return (-EFAULT));
	{
		mm_segment_t fs = get_fs();

		set_fs(KERNEL_DS);
		res = sock_recvmsg(ss->sock, msg, size, sflags);
		set_fs(fs);
	}
	if (res < 0)
		printd(("%s: %p: ERROR: from sock->ops->recvmsg %d\n", DRV_NAME, ss, res));
	return (res);
}

/*
 *  SOCKET DISCONNECT
 *  ------------------------------------------------------------------------
 *  Performing a sock_release (ss_socket_put) from the established state does
 *  not affect an abortive release for TCP, but rather, initiates an orderly
 *  shutdown rather than an abortive release.  We can try performing a
 *  protocol disconnect and see if that works better.
 */
STATIC int
ss_disconnect(ss_t *ss)
{
	int err;

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->connect, return (-EFAULT));
	ensure(ss->sock->sk, return (-EFAULT));
	ensure(ss->sock->sk->sk_prot, return (-EFAULT));
	ensure(ss->sock->sk->sk_prot->disconnect, return (-EFAULT));
	if (!(err = ss->sock->sk->sk_prot->disconnect(ss->sock->sk, O_NONBLOCK))) {
		ss->sock->state = SS_UNCONNECTED;
		return (0);
	}
	ss->sock->state = SS_DISCONNECTING;
	printd(("%s: %p: ERROR: from sock->sk->sk_prot->disconnect %d\n", DRV_NAME, ss, err));
	return (err);
}

/*
 *  SOCKET GETNAMES
 *  ------------------------------------------------------------------------
 *  Get the names from a socket.
 */
STATIC int
ss_getsockname(ss_t *ss)
{
	int err, len = sizeof(ss->src);

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->getname, return (-EFAULT));
	if ((err = ss->sock->ops->getname(ss->sock, &ss->src, &len, 0)))
		return (err);
	return (len);
}
STATIC int
ss_getpeername(ss_t *ss)
{
	int err, len = sizeof(ss->dst);

	ensure(ss, return (-EFAULT));
	ensure(ss->sock, return (-EPROTO));
	ensure(ss->sock->ops, return (-EFAULT));
	ensure(ss->sock->ops->getname, return (-EFAULT));
	if ((err = ss->sock->ops->getname(ss->sock, &ss->dst, &len, 1)))
		return (err);
	return (len);
}
STATIC int
ss_getnames(ss_t *ss)
{
	int err_src, err_dst;

	err_src = ss_getsockname(ss);
	err_dst = ss_getpeername(ss);
	return (err_dst < 0 ? err_dst : (err_src < 0 ? err_src : 0));
}

/*
 *  =========================================================================
 *
 *  IP T-Provider --> T-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 */
/*
 *  M_FLUSH
 *  ---------------------------------------------------------------
 */
STATIC int
m_flush(queue_t *q, int how, int band)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;

	if ((mp = ss_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_FLUSH;
		*mp->b_wptr++ = how;
		*mp->b_wptr++ = band;
		putnext(ss->rq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  M_ERROR
 *  ---------------------------------------------------------------
 */
STATIC int
m_error(queue_t *q, int error)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	int hangup = 0;

	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if ((mp = ss_allocb(q, 2, BPRI_HI))) {
		if (ss->sock)
			ss_socket_put(xchg(&ss->sock, NULL));
		if (hangup) {
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, ss));
			mp->b_datap->db_type = M_HANGUP;
			putnext(ss->rq, mp);
			error = EPIPE;
		} else {
			printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, ss, error));
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error;
			*(mp->b_wptr)++ = error;
			putnext(ss->rq, mp);
		}
		return (-error);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_CONN_IND          11 - Connection Indication
 *  ---------------------------------------------------------------
 */
STATIC streams_fastcall int
t_conn_ind(queue_t *q, struct sockaddr *src, mblk_t *cp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_conn_ind *p;
	size_t src_len = ss_addr_size(ss, src);
	size_t opt_len = ss_size_conn_opts(ss);
	int err;

	if (bufq_length(&ss->conq) <= ss->conind) {
		if (canputnext(ss->rq)) {
			if ((mp = ss_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = T_CONN_IND;
				p->SRC_length = src_len;
				p->SRC_offset = src_len ? sizeof(*p) : 0;
				p->OPT_length = opt_len;
				p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
				p->SEQ_number = (t_uscalar_t) (long) cp;
				mp->b_wptr += sizeof(*p);
				if (src_len) {
					bcopy(src, mp->b_wptr, src_len);
					mp->b_wptr += src_len;
				}
				if (opt_len) {
					if ((err = t_build_conn_opts(ss, mp->b_wptr, opt_len)) >= 0)
						mp->b_wptr += opt_len;
					else {
						freemsg(mp);
						ptrace(("%s: ERROR: option build fault\n",
							DRV_NAME));
						return (-EFAULT);
					}
				}
				bufq_queue(&ss->conq, cp);
				ss_set_state(ss, TS_WRES_CIND);
				printd(("%s: %p: <- T_CONN_IND\n", DRV_NAME, ss));
				putnext(ss->rq, mp);
				return (QR_ABSORBED);	/* absorbed cp */
			}
			ptrace(("%s: ERROR: no buffers\n", DRV_NAME));
			return (-ENOBUFS);
		}
		ptrace(("%s: ERROR: flow controlled\n", DRV_NAME));
		return (-EBUSY);
	}
	ptrace(("%s: ERROR: too many conn inds\n", DRV_NAME));
	return (-EAGAIN);
}

/*
 *  T_CONN_CON          12 - Connection Confirmation
 *  ---------------------------------------------------------------
 */
STATIC streams_fastcall int
t_conn_con(queue_t *q, struct sockaddr *res, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_conn_con *p;
	size_t res_len = ss_addr_size(ss, res);
	size_t opt_len = ss_size_conn_opts(ss);
	int err;

	/* this shouldn't happen, we probably shouldn't even check */
	if (canputnext(ss->rq)) {
		if ((mp = ss_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
			mp->b_wptr += sizeof(*p);
			if (res_len) {
				bcopy(res, mp->b_wptr, res_len);
				mp->b_wptr += res_len;
			}
			if (opt_len) {
				if ((err = t_build_conn_opts(ss, mp->b_wptr, opt_len)) >= 0)
					mp->b_wptr += opt_len;
				else {
					freemsg(mp);
					ptrace(("%s: ERROR: option build fault\n", DRV_NAME));
					return (-EFAULT);
				}
			}
			ss_set_state(ss, TS_DATA_XFER);
			printd(("%s: %p: <- T_CONN_CON\n", DRV_NAME, ss));
			putnext(ss->rq, mp);
			return (0);
		}
		ptrace(("%s: ERROR: no buffers\n", DRV_NAME));
		return (-ENOBUFS);
	}
	ptrace(("%s: ERROR: flow controlled\n", DRV_NAME));
	return (-EBUSY);
}

/*
 *  T_DISCON_IND        13 - Disconnect Indication
 *  ---------------------------------------------------------------
 */
STATIC mblk_t *
t_seq_find(ss_t *ss, mblk_t *rp)
{
	mblk_t *mp;

	if ((mp = rp)) {
		struct sock *sk = ((ss_event_t *) rp->b_rptr)->sk;

		spin_lock_bh(&ss->conq.q_lock);
		{
			for (mp = bufq_head(&ss->conq); mp && ((ss_event_t *) mp->b_rptr)->sk != sk;
			     mp = mp->b_next) ;
		}
		spin_unlock_bh(&ss->conq.q_lock);
	}
	return (mp);
}

STATIC t_uscalar_t
t_seq_delete(ss_t *ss, mblk_t *rp)
{
	mblk_t *mp;

	if ((mp = t_seq_find(ss, rp))) {
		struct socket *sock = NULL;

		if (!ss_accept(ss, &sock, rp) && sock)
			sock_release(sock);
		return ((t_uscalar_t) (long) mp);
	}
	return (0);
}
STATIC int
t_discon_ind(queue_t *q, struct sockaddr *res, uint orig, uint reason, mblk_t *cp, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_discon_ind *p;
	t_uscalar_t seq = 0;

	(void) res;
	/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
	   TS_WACK_ORDREL [sic] state, the stream must be flushed before sending up the
	   T_DISCON_IND primitive. */
	switch (ss_get_state(ss)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		if (m_flush(q, FLUSHRW, 0) == -ENOBUFS)
			goto enobufs;
	}
	if (canputnext(ss->rq)) {
		if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
			if (!cp || (seq = t_seq_delete(ss, cp))) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = T_DISCON_IND;
				p->DISCON_reason = reason;
				p->SEQ_number = seq;
				mp->b_wptr += sizeof(*p);
				if (!bufq_length(&ss->conq))
					ss_set_state(ss, TS_IDLE);
				else
					ss_set_state(ss, TS_WRES_CIND);
				mp->b_cont = dp;
				printd(("%s: %p: <- T_DISCON_IND\n", DRV_NAME, ss));
				putnext(ss->rq, mp);
				return (0);
			}
			freemsg(mp);
			ptrace(("%s: ERROR: bad sequence number\n", DRV_NAME));
			return (-EFAULT);
		}
	      enobufs:
		ptrace(("%s: ERROR: no buffers\n", DRV_NAME));
		return (-ENOBUFS);
	}
	ptrace(("%s: ERROR: flow controlled\n", DRV_NAME));
	return (-EBUSY);
}

/*
 *  T_DATA_IND          14 - Data Indication
 *  ---------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_in int
t_data_ind(queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_data_ind *p;

	if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_DATA_IND;
		p->MORE_flag = (msg->msg_flags & MSG_EOR) ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		dp->b_band = 0;	/* sometimes non-zero */
		dp->b_flag &= ~MSGMARK;	/* sometimes marked */
		mp->b_cont = dp;
		printd(("%s: %p: <- T_DATA_IND\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: ERROR: no buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/*
 *  T_EXDATA_IND        15 - Expedited Data Indication
 *  ---------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_in int
t_exdata_ind(queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_exdata_ind *p;

	if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 1;	/* expedite */
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_EXDATA_IND;
		p->MORE_flag = (msg->msg_flags & MSG_EOR) ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		dp->b_band = 0;	/* sometimes non-zero */
		dp->b_flag &= ~MSGMARK;	/* sometimes marked */
		mp->b_cont = dp;
		printd(("%s: %p: <- T_EXDATA_IND\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: ERROR: no buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/*
 *  T_INFO_ACK          16 - Information acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC streams_fastcall int
t_info_ack(queue_t *q)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;

	if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = ss->p.info;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/*
 *  T_BIND_ACK          17 - Bind Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC streams_fastcall int
t_bind_ack(queue_t *q, struct sockaddr *add, t_uscalar_t conind)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = ss_addr_size(ss, add);

	if ((mp = ss_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		mp->b_wptr += sizeof(*p);
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		ss_set_state(ss, TS_IDLE);
		printd(("%s: %p: <- T_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/*
 *  T_ERROR_ACK         18 - Error Acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
t_error_ack(queue_t *q, t_scalar_t prim, t_scalar_t error)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_error_ack *p;

	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (error);
	case 0:
		never();
		return (error);
	}
	if (!(mp = ss_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	/* This is to only try and get the state correct for putnext. */
	if (error != TOUTSTATE) {
		switch (ss_get_state(ss)) {
#ifdef TS_WACK_OPTREQ
		case TS_WACK_OPTREQ:
			ss_set_state(ss, TS_IDLE);
			break;
#endif
		case TS_WACK_UREQ:
			ss_set_state(ss, TS_IDLE);
			break;
		case TS_WACK_CREQ:
			ss_set_state(ss, TS_IDLE);
			break;
		case TS_WACK_BREQ:
			ss_set_state(ss, TS_UNBND);
			break;
		case TS_WACK_CRES:
			ss_set_state(ss, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ6:
			ss_set_state(ss, TS_WCON_CREQ);
			break;
		case TS_WACK_DREQ7:
			ss_set_state(ss, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			ss_set_state(ss, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			ss_set_state(ss, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			ss_set_state(ss, TS_WREQ_ORDREL);
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   TS_IDLE state. */
		}
	}
	printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, ss));
	putnext(ss->rq, mp);
	/* Returning -EPROTO here will make sure that the old state is restored correctly.  If we
	   return QR_DONE, then the state will never be restored. */
	if (error < 0)
		return (error);
	return (-EPROTO);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

STATIC streams_fastcall __hot_in int ss_sock_recvmsg(queue_t *q);

/*
 *  T_OK_ACK            19 - Success Acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, t_scalar_t prim, mblk_t *cp, ss_t *as)
{
	int err = -EFAULT;
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_ok_ack *p;
	struct socket *sock = NULL;

	if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (ss_get_state(ss)) {
		case TS_WACK_CREQ:
			if ((err = ss_connect(ss, &ss->dst)))
				goto free_error;
			ss_getnames(ss);
			ss_set_state(ss, TS_WCON_CREQ);
			break;
		case TS_WACK_UREQ:
			if ((err = ss_unbind(ss)))
				goto free_error;
			/* TPI spec says that if the provider must flush both queues before
			   responding with a T_OK_ACK primitive when responding to a T_UNBIND_REQ.
			   This is to flush queued data for connectionless providers. */
			if (m_flush(q, FLUSHRW, 0) == -ENOBUFS)
				goto enobufs;
			ss_set_state(ss, TS_UNBND);
			break;
		case TS_WACK_CRES:
			ensure(as && cp, goto free_error);
			if ((err = ss_accept(ss, &sock, cp)))
				goto free_error;
			if (as->sock)
				ss_socket_put(as->sock);	/* get rid of old socket */
			as->sock = sock;
			ss_socket_get(as->sock, as);
			ss_set_state(as, TS_DATA_XFER);
			t_set_options(as);	/* reset options against new socket */
			if (as != ss) {
				/* only change state if not accepting on listening socket */
				if (bufq_length(&ss->conq))
					ss_set_state(ss, TS_WRES_CIND);
				else {
					ss_set_state(ss, TS_IDLE);
					/* make sure any backed up indications are processed */
					qenable(ss->rq);
				}
			}
			/* make sure any data on the socket is delivered */
			ss_sock_recvmsg(as->rq);
			break;
		case TS_WACK_DREQ7:
			ensure(cp, goto free_error);
			if (!(err = ss_accept(ss, &sock, cp))) {
				if (!sock->sk->sk_prot->disconnect(sock->sk, O_NONBLOCK))
					sock->state = SS_UNCONNECTED;
				sock_release(sock);
			}
			if (bufq_length(&ss->conq))
				ss_set_state(ss, TS_WRES_CIND);
			else {
				ss_set_state(ss, TS_IDLE);
				/* make sure any backed up indications are processed */
				qenable(ss->rq);
			}
			break;
		case TS_WACK_DREQ6:
			if ((err = ss_disconnect(ss)))
				goto free_error;
			ss_set_state(ss, TS_IDLE);
			break;
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			if ((err = ss_disconnect(ss)))
				goto free_error;
			/* TPI spec says that if the interface is in the TS_DATA_XFER,
			   TS_WIND_ORDREL or TS_WACK_ORDREL [sic] state, the stream must be flushed
			   before responding with the T_OK_ACK primitive. */
			if (m_flush(q, FLUSHRW, 0) == -ENOBUFS)
				goto enobufs;
			ss_set_state(ss, TS_IDLE);
			break;
		default:
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   the TS_IDLE state. */
		}
		printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (QR_DONE);
	}
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, prim, err);
}

/*
 *  T_UNIDATA_IND       20 - Unitdata indication
 *  -------------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_in int
t_unitdata_ind(queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t opt_len = ss_opts_size(ss, msg);

#if 0
	fixme(("Option processing strapped out for debugging: size would be %u\n", opt_len));
	opt_len = 0;
#endif
	if ((mp = ss_allocb(q, sizeof(*p) + msg->msg_namelen + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_UNITDATA_IND;
		p->SRC_length = msg->msg_namelen;
		p->SRC_offset = msg->msg_namelen ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + msg->msg_namelen : 0;
		mp->b_wptr += sizeof(*p);
		if (msg->msg_namelen) {
			bcopy(msg->msg_name, mp->b_wptr, msg->msg_namelen);
			mp->b_wptr += msg->msg_namelen;
		}
		if (opt_len) {
			ss_opts_build(ss, msg, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		dp->b_band = 0;	/* sometimes non-zero */
		dp->b_flag &= ~MSGMARK;	/* sometimes marked */
		mp->b_cont = dp;
		printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/*
 *  T_UDERROR_IND       21 - Unitdata Error Indication
 *  -------------------------------------------------------------------------
 *  This primitive indicates to the transport user that a datagram with the
 *  specified destination address and options produced an error.
 *
 *  This is not called, but should be when T_UNITDATA_REQ fails due to options
 *  errors or permission.
 */
STATIC INLINE streams_fastcall __hot_get int
t_uderror_ind(queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t opt_len = ss_errs_size(ss, msg);
	int etype = 0;

	if (canputnext(ss->rq)) {
		if ((mp = ss_allocb(q, sizeof(*p) + msg->msg_namelen + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = msg->msg_namelen;
			p->DEST_offset = msg->msg_namelen ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + msg->msg_namelen : 0;
			mp->b_wptr += sizeof(*p);
			if (msg->msg_namelen) {
				bcopy(msg->msg_name, mp->b_wptr, msg->msg_namelen);
				mp->b_wptr += msg->msg_namelen;
			}
			if (opt_len) {
				ss_errs_build(ss, msg, mp->b_wptr, opt_len, &etype);
				mp->b_wptr += opt_len;
			}
			p->ERROR_type = etype;
			dp->b_band = 0;	/* sometimes non-zero */
			dp->b_flag &= ~MSGMARK;	/* sometimes marked */
			mp->b_cont = dp;
			printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, ss));
			putnext(ss->rq, mp);
			return (0);
		}
		ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
		return (-ENOBUFS);
	}
	ptrace(("%s: ERROR: Flow controlled\n", DRV_NAME));
	return (-EBUSY);
}

/*
 *  T_OPTMGMT_ACK       22 - Options Management Acknowledge
 *  -------------------------------------------------------------------------
 *  Note: opt_len is conservative but might not be actual size of the output
 *  options.  This will be adjusted when the option buffer is built.
 */
STATIC streams_fastcall int
t_optmgmt_ack(queue_t *q, t_scalar_t flags, unsigned char *req, size_t req_len, size_t opt_len)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if ((mp = ss_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		if ((flags = t_build_options(ss, req, req_len, mp->b_wptr, &opt_len, flags)) < 0) {
			freemsg(mp);
			return (flags);
		}
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		if (opt_len) {
			mp->b_wptr += opt_len;
		}
#ifdef TS_WACK_OPTREQ
		if (ss_get_state(ss) == TS_WACK_OPTREQ)
			ss_set_state(ss, TS_IDLE);
#endif
		printd(("%s: %p: <- T_OPTMGMT_ACK\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

/*
 *  T_ORDREL_IND        23 - Orderly Release Indication
 *  -------------------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(queue_t *q)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_ordrel_ind *p;

	if (canputnext(ss->rq)) {
		if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_IND;
			mp->b_wptr += sizeof(*p);
			switch (ss_get_state(ss)) {
			case TS_DATA_XFER:
				ss_set_state(ss, TS_WREQ_ORDREL);
				break;
			case TS_WIND_ORDREL:
				ss_set_state(ss, TS_IDLE);
				break;
			}
			printd(("%s: %p: <- T_ORDREL_IND\n", DRV_NAME, ss));
			putnext(ss->rq, mp);
			return (0);
		}
		ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
		return (-ENOBUFS);
	}
	ptrace(("%s: ERROR: Flow controlled\n", DRV_NAME));
	return (-EBUSY);
}

/*
 *  T_OPTDATA_IND       26 - Data with Options Indication
 *  -------------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_in int
t_optdata_ind(queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t opt_len = ss_opts_size(ss, msg);

#if 0
	fixme(("Option processing strapped out for debugging: size would be %u\n", opt_len));
	opt_len = 0;
#endif
	if ((mp = ss_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = msg->msg_flags & T_ODF_EX ? 1 : 0;	/* expedite */
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_IND;
		p->DATA_flag =
		    ((msg->msg_flags & MSG_EOR) ? 0 : T_ODF_MORE) | ((msg->
								      msg_flags & MSG_OOB) ?
								     T_ODF_EX : 0);
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		if (opt_len) {
			ss_opts_build(ss, msg, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		dp->b_band = 0;	/* sometimes non-zero */
		dp->b_flag &= ~MSGMARK;	/* sometimes marked */
		mp->b_cont = dp;
		printd(("%s: %p: <- T_OPTDATA_IND\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/*
 *  T_ADDR_ACK          27 - Address Acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_addr_ack(queue_t *q, struct sockaddr *loc, struct sockaddr *rem)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = ss_addr_size(ss, loc);
	size_t rem_len = ss_addr_size(ss, rem);

	if ((mp = ss_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
		mp->b_wptr += sizeof(*p);
		if (loc_len) {
			bcopy(loc, mp->b_wptr, loc_len);
			mp->b_wptr += loc_len;
		}
		if (rem_len) {
			bcopy(rem, mp->b_wptr, rem_len);
			mp->b_wptr += rem_len;
		}
		printd(("%s: %p: <- T_ADDR_ACK\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif

#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK    ?? - Protocol Capability Ack
 *  -------------------------------------------------------------------------
 *  Note that TPI Version 2 Draft 2 says that if the T_CAPABILITY_REQ is sent as
 *  a M_PCPROTO then the the T_CAPABILITY_ACK must be sent as an M_PCPROTO and
 *  that if the T_CAPABILITY_REQ was sent as a M_PROTO, then the
 *  T_CAPABILITY_ACK must also be sent as a M_PROTO.
 */
STATIC streams_fastcall int
t_capability_ack(queue_t *q, t_uscalar_t caps, int type)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct T_capability_ack *p;

	if ((mp = ss_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = type;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = caps & (TC1_INFO | TC1_ACCEPTOR_ID);
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (long) ss->rq : 0;
		mp->b_wptr += sizeof(*p);
		if (caps & TC1_INFO)
			p->INFO_ack = ss->p.info;
		else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, ss));
		putnext(ss->rq, mp);
		return (0);
	}
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
}
#endif

/*
 *  =========================================================================
 *
 *  CONNECTION INNDICATIONS
 *
 *  =========================================================================
 */
/*
 *  CONIND
 *  -------------------------------------------------------------------------
 *  An unfortunate mis-match between the socket model and the TLI model is
 *  that in TLI it is possible to determine the remote address from a
 *  T_CONN_IND *before* accepting the connection.  Under sockets, the
 *  connection must be accepted before it is possible to determine the peer's
 *  address.  This means that connection must be accepted and then released,
 *  rather than refused.
 */
STATIC streams_fastcall int
ss_conn_ind(queue_t *q, mblk_t *cp)
{
	ss_t *ss = PRIV(q);
	struct sock *sk;
	struct ss_event *p = (typeof(p)) cp->b_rptr;
	struct sockaddr dst = { AF_UNSPEC, };

	if ((1 << ss_get_state(ss)) & ~TSM_LISTEN)
		goto outstate;
	if (cp->b_wptr < cp->b_rptr + sizeof(*p))
		goto einval;
	if (ss->sock->sk == p->sk)
		goto duplicate;
	sk = p->sk;
	switch (ss->p.prot.family) {
	case AF_INET:
	{
		struct sockaddr_in *dst_in = (typeof(dst_in)) & dst;

		if (!ss->conind || ss->sock->sk->sk_state != TCP_LISTEN)
			goto nolisten;
		dst_in->sin_family = AF_INET;
		dst_in->sin_port = sock_dport(sk);
		dst_in->sin_addr.s_addr = sock_daddr(sk);
		break;
	}
	case AF_UNIX:
	{
		struct sockaddr_un *dst_un = (typeof(dst_un)) & dst;

		/* FIXME: use AF_UNIX states */
		if (!ss->conind || ss->sock->sk->sk_state != TCP_LISTEN)
			goto nolisten;
		break;
	}
	default:
		goto einval;
	}
	return t_conn_ind(q, &dst, cp);
      einval:
	pswerr(("%s: SWERR: invalid primitive format\n", DRV_NAME));
	return (-EFAULT);
      outstate:
	ptrace(("%s: ERROR: connect indication in wrong state %ld\n", DRV_NAME,
		(long) ss_get_state(ss)));
	return (-EPROTO);
      nolisten:
	ptrace(("%s: ERROR: connect indication received while not listening\n", DRV_NAME));
	return (-EPROTO);
      duplicate:
	ptrace(("%s: %p: INFO: discarding duplicate connection indication\n", DRV_NAME, ss));
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  SENDMSG and RECVMSG
 *
 *  =========================================================================
 */
/*
 *  SENDMSG
 *  -------------------------------------------------------------------------
 *  Convert the mblk to send into an iovec and the options into a control
 *  message and then call sendmsg on the socket with the kernel data segment.
 *  The socket will handle moving data from the mblks.
 */
STATIC streams_fastcall __hot_out int
ss_sock_sendmsg(ss_t *ss, mblk_t *mp, struct msghdr *msg)
{
	int err = 0;
	int len, sdu, n;
	mblk_t *dp;

	for (len = 0, n = 0, dp = mp; dp; dp = dp->b_cont) {
		if (dp->b_datap->db_type == M_DATA && dp->b_wptr > dp->b_rptr) {
			len += dp->b_wptr - dp->b_rptr;
			n++;
		}
	}
	err = -EPROTO;
	if (!(ss->p.info.PROVIDER_flag & T_SNDZERO) && (!n || !len))
		goto out;
	if ((sdu = (int) ss->p.info.TSDU_size) > 0 && (!n || len > sdu))
		goto out;
	err = -EFAULT;
	ensure(ss->sock, goto out);
	err = -EBUSY;
	if (len > sock_wspace(ss->sock->sk))
		goto out;
	/* This has the ramification that we can never do zero length writes, but we have
	   ~T_SNDZERO set anyway. */
	{
		int i;
		struct iovec iov[n];

		msg->msg_flags |= (MSG_DONTWAIT | MSG_NOSIGNAL);
		msg->msg_iov = iov;
		msg->msg_iovlen = n;
		/* convert message blocks to an iovec */
		for (i = 0, dp = mp; dp; dp = dp->b_cont) {
			if (dp->b_datap->db_type == M_DATA && dp->b_wptr > dp->b_rptr) {
				iov[i].iov_base = dp->b_rptr;
				iov[i].iov_len = dp->b_wptr - dp->b_rptr;
				i++;
			}
		}
		ptrace(("%s: %p: sendmsg with len = %d\n", DRV_NAME, ss, len));
		err = ss_sendmsg(ss, msg, len);
	}
	if (err < 0)
		goto out;
	if (!err) {
		err = -EBUSY;
		goto out;
	}
	if (err >= len)
		return (QR_DONE);	/* full write */
	switch (ss->p.prot.type) {	/* partial write */
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		err = -EMSGSIZE;
		goto out;
	case SOCK_SEQPACKET:
	case SOCK_STREAM:
		dp = mp;
		if (dp->b_datap->db_type != M_DATA) {
			dp = dp->b_cont;
			assert(dp != NULL);
		}
		if (!adjmsg(dp, err))
			swerr();
		err = -EAGAIN;
		goto out;
	}
	err = -EFAULT;
      out:
	switch (-err) {
	case ENOMEM:
		/* This buffer call is just to kick us.  Because LiS uses kmalloc for mblks, if we
		   can allocate an mblk, then another kernel routine can allocate that much memory
		   too. */
		if (ss->wbid) {
			unbufcall(xchg(&ss->wbid, 0));
			ss->refcnt--;
		}
		ss->wbid = bufcall(len, BPRI_LO, &ss_bufsrv, (long) ss->wq);
		ss->refcnt++;
	default:
		return m_error(ss->rq, err);
	}
}

STATIC streams_fastcall int __hot_in ss_r_data(queue_t *q, mblk_t *mp);

/*
 *  RECVMSG
 *  -------------------------------------------------------------------------
 */
STATIC streams_fastcall __hot_in int
ss_sock_recvmsg(queue_t *q)
{
	ss_t *ss = PRIV(q);
	mblk_t *mp;
	struct msghdr *msg;
	struct sockaddr *add;
	u_int32_t *cbuf;
	const size_t clen = (sizeof(u_int32_t) << 5);
	struct iovec *iov;
	int size;

	while (canput(q)) {
		switch (ss->p.prot.type) {
		case SOCK_DGRAM:
		case SOCK_RAW:
		case SOCK_RDM:
		case SOCK_STREAM:
		{
			struct sk_buff *skb;

			ensure(ss->sock, return (QR_DONE));	/* spurious, ignore it */
			size = (skb = skb_peek(&ss->sock->sk->sk_receive_queue)) ? skb->len : 0;
			break;
		}
		case SOCK_SEQPACKET:
			ensure(ss->sock, return (QR_DONE));	/* spurious, ignore it */
			size = atomic_read(&ss->sock->sk->sk_rmem_alloc);
			break;
		default:
			swerr();
			size = 0;
			break;
		}
		if (!size)
			return (QR_DONE);

		if (!(mp = ss_allocb(ss->rq,
				     size + sizeof(*msg) + sizeof(*add) +
				     clen + sizeof(*iov), BPRI_MED)))
			return (-ENOBUFS);
		/* message header */
		msg = (struct msghdr *) mp->b_wptr;
		mp->b_wptr += sizeof(*msg);
		/* address */
		add = (struct sockaddr *) mp->b_wptr;
		add->sa_family = AF_INET;
		mp->b_wptr += sizeof(*add);
		/* control buffer */
		cbuf = (u_int32_t *) mp->b_wptr;
		cbuf[0] = 0xdeadbeef;
		mp->b_wptr += clen;
		/* io vector */
		iov = (struct iovec *) mp->b_wptr;
		mp->b_wptr += sizeof(*iov);
		mp->b_rptr = mp->b_wptr;	/* hide msg headers */
		iov->iov_base = mp->b_rptr;
		iov->iov_len = size;
		msg->msg_iov = iov;
		msg->msg_iovlen = 1;
		if (ss->p.prot.type != SOCK_STREAM) {
			msg->msg_name = add;
			msg->msg_namelen = ss_addr_size(ss, add);
		} else {
			msg->msg_name = NULL;
			msg->msg_namelen = 0;
		}
		msg->msg_control = cbuf;
		msg->msg_controllen = clen;
		msg->msg_flags = MSG_DONTWAIT;
		{
			int err;

			if ((err = ss_recvmsg(ss, msg, size)) <= 0) {
				freemsg(mp);
				return (err);
			}
			mp->b_wptr = mp->b_rptr + err;
			ptrace(("%s: %p: recvmsg with len = %d\n", DRV_NAME, ss, err));
		}
		if (msg->msg_flags & MSG_CTRUNC) {
			printd(("%s: %p: control message truncated\n", DRV_NAME, ss));
			msg->msg_control = NULL;
			msg->msg_controllen = 0;
		}
		if (msg->msg_control != (void *) cbuf) {
			printd(("%s: %p: control message pointer moved!\n", DRV_NAME, ss));
			printd(("%s: %p: initial control buffer %p\n", DRV_NAME, ss,
				msg->msg_control));
			printd(("%s: %p: initial control length %ld\n", DRV_NAME, ss,
				(long) msg->msg_controllen));
			msg->msg_control = cbuf;
			msg->msg_controllen = clen - msg->msg_controllen;
			printd(("%s: %p: final control buffer %p\n", DRV_NAME, ss,
				msg->msg_control));
			printd(("%s: %p: final control length %ld\n", DRV_NAME, ss,
				(long) msg->msg_controllen));
		} else if (msg->msg_controllen == clen && cbuf[0] == 0xdeadbeef) {
			printd(("%s: %p: control message unchanged!\n", DRV_NAME, ss));
			msg->msg_control = NULL;
			msg->msg_controllen = 0;
		}
		if (msg->msg_flags & MSG_TRUNC) {
			swerr();
			/* This indicates a problem with datagram size logic. A buffer large enough 
			   to hold the available data was not calculated correctly, resulting in a
			   truncated buffer.  This only happens for connectionless: Discard the
			   message rather than delivering a partial message. */
			freemsg(mp);
			continue;
		}
		{
			int err;

			/* Attempt to deliver directly as was done before. */
			if ((err = ss_r_data(q, mp)) != QR_ABSORBED) {
				/* If unsuccessful, place the M_DATA message on the queue for later
				   processing.  Note that if an M_READ message is placed back on the 
				   queue as a result of flow control (-EBUSY) of lack of bufers
				   (-ENOBUFS) it will be transformed into an M_CTL message of band 0 
				   and will, therefore, queue behind the M_DATA mesages. */
				if (msg->msg_flags & (MSG_OOB | MSG_ERRQUEUE)) {
					mp->b_band = 2;	/* expedite */
					mp->b_flag |= MSGMARK;
				} else {
					mp->b_band = 1;
				}
				/* Note that this could enable the queue.  When the service
				   procedure runs (possibly again), ss_r_data() will do a putbq()
				   on a subsequent failure and suspend the service procedure from
				   there. */
				if (!putq(q, mp)) {
					swerr();
					/* Will only happen if there is not enough memory to
					   allocate a band 1 or 2 qband structure, which is quite
					   possible if we got here as a result of a buffer
					   allocation failure.  Therefore, strqset() is used to
					   allocate one of each when the Stream is opened so (see
					   ss_open()) so that this call cannot fail. */
					freemsg(mp);
				}
				return (err);
			}
		}
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  STATE MACHINE
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Socket CALLBACKS
 *
 *  -------------------------------------------------------------------------
 */
STATIC streams_fastcall __hot void
ss_putctl(ss_t *ss, queue_t *q, int type, void streamscall (*func) (long), struct sock *sk)
{
	mblk_t *mp;
	ss_event_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_HI))) {
		mp->b_datap->db_type = type;
		p = (typeof(p)) mp->b_wptr;
		p->sk = sk;
		p->state = sk->sk_state;	/* capture current state */
		mp->b_wptr += sizeof(*p);
		if (!putq(q, mp))
			freemsg(mp);	/* FIXME */
		return (void) (0);
	}
	/* set up bufcall so we don't lose events */
	spin_lock_bh(&ss->lock);
	/* make sure bufcalls don't happen now */
	if (q == ss->rq) {
		if (ss->rbid)
			unbufcall(xchg(&ss->rbid, 0));
		ss->rbid = bufcall(FASTBUF, BPRI_HI, func, (long) sk);
	} else if (q == ss->wq) {
		if (ss->wbid)
			unbufcall(xchg(&ss->wbid, 0));
		ss->wbid = bufcall(FASTBUF, BPRI_HI, func, (long) sk);
	} else
		swerr();
	spin_unlock_bh(&ss->lock);
}

/*
 *  STATE change
 *  -------------------------------------------------------------------------
 *  We get state changes on sockets that we hold.  We also get state changes
 *  on accepting sockets.
 */
STATIC streamscall __hot void
_ss_sock_state_change(long data)
{
	struct sock *sk;
	ss_t *ss;

	if ((sk = (struct sock *) data)) {
		read_lock(&sk->sk_callback_lock);
		if (sk->sk_state_change == &ss_state_change) {
			if ((ss = SOCK_PRIV(sk))) {
				ss_putctl(ss, ss->rq, M_RSE, &_ss_sock_state_change, sk);
			} else
				assure(ss);
		} else
			assure(sk->sk_state_change == &ss_state_change);
		read_unlock(&sk->sk_callback_lock);
	} else
		assure(sk);
}
STATIC __hot void
ss_state_change(struct sock *sk)
{
	_ss_sock_state_change((long) sk);
}

/*
 *  WRITE Available
 *  -------------------------------------------------------------------------
 */
STATIC streamscall __hot_out void
_ss_sock_write_space(long data)
{
	struct sock *sk;
	ss_t *ss;

	if ((sk = (struct sock *) data)) {
		read_lock(&sk->sk_callback_lock);
		if (sk->sk_write_space == &ss_write_space) {
			if ((ss = SOCK_PRIV(sk))) {
				ss_putctl(ss, ss->wq, M_READ, &_ss_sock_write_space, sk);
			} else
				assure(ss);
		} else
			assure(sk->sk_write_space == &ss_write_space);
		read_unlock(&sk->sk_callback_lock);
	} else
		assure(sk);
}
STATIC __hot_out void
ss_write_space(struct sock *sk)
{
	_ss_sock_write_space((long) sk);
}

/*
 *  ERROR Available
 *  -------------------------------------------------------------------------
 */
STATIC streamscall __hot_get void
_ss_sock_error_report(long data)
{
	struct sock *sk;
	ss_t *ss;

	if ((sk = (struct sock *) data)) {
		read_lock(&sk->sk_callback_lock);
		if (sk->sk_error_report == &ss_error_report) {
			if ((ss = SOCK_PRIV(sk))) {
				ss_putctl(ss, ss->rq, M_ERROR, &_ss_sock_error_report, sk);
			}
#if 0
			else
				assure(ss);
#endif
		} else
			assure(sk->sk_error_report == &ss_error_report);
		read_unlock(&sk->sk_callback_lock);
	} else
		assure(sk);
}
STATIC __hot_get void
ss_error_report(struct sock *sk)
{
	_ss_sock_error_report((long) sk);
}

/*
 *  READ Available
 *  -------------------------------------------------------------------------
 */
STATIC streamscall __hot_in void
_ss_sock_data_ready(long data)
{
	struct sock *sk;
	ss_t *ss;

	if ((sk = (struct sock *) data)) {
		read_lock(&sk->sk_callback_lock);
		if (sk->sk_data_ready == &ss_data_ready) {
			if ((ss = SOCK_PRIV(sk))) {
				ss_putctl(ss, ss->rq, M_READ, &_ss_sock_data_ready, sk);
			} else
				assure(ss);
		} else
			assure(sk->sk_data_ready == &ss_data_ready);
		read_unlock(&sk->sk_callback_lock);
	} else
		assure(sk);
}
STATIC __hot_in void
ss_data_ready(struct sock *sk, int len)
{
	(void) len;
	_ss_sock_data_ready((long) sk);
}

/*
 *  =========================================================================
 *
 *  IP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 *
 *  T_CONN_REQ           0 - TC Request
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_conn_req(queue_t *q, mblk_t *mp)
{
	int err = -EFAULT;
	ss_t *ss = PRIV(q);
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ss_get_state(ss) != TS_IDLE)
		goto outstate;
	if ((p->DEST_length && mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto einval;
	else {
		struct sockaddr *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);

		if (p->DEST_length < ss_addr_size(ss, dst))
			goto badaddr;
		if (dst->sa_family != ss->p.prot.family)
			goto badaddr;
		switch (ss->p.prot.family) {
		case AF_INET:
		{
			struct sockaddr_in *dst_in = (typeof(dst_in)) dst;

			if (dst_in->sin_port == 0)
				goto badaddr;
			if (ss->cred.cr_uid != 0 && ntohs(dst_in->sin_port) == IPPROTO_RAW)
				goto acces;
			break;
		}
		case AF_UNIX:
			break;
		default:
			goto badaddr;
		}
		if ((err = t_parse_conn_opts(ss, mp->b_rptr + p->OPT_offset, p->OPT_length, 1)) < 0) {
			switch (-err) {
			case EINVAL:
				goto badopt;
			case EACCES:
				goto acces;
			default:
				goto error;
			}
		}
		ss->dst = *dst;
		if (mp->b_cont) {
			long mlen, mmax;

			mlen = msgdsize(mp->b_cont);
			if (((mmax = ss->p.info.CDATA_size) > 0 && mlen > mmax)
			    || mmax == T_INVALID || ((mmax = ss->p.info.TIDU_size) > 0
						     && mlen > mmax)
			    || mmax == T_INVALID)
				goto baddata;
			/* FIXME need to generate connect with data */
		}
		ss_set_state(ss, TS_WACK_CREQ);
		return t_ok_ack(q, T_CONN_REQ, NULL, NULL);
	}
      baddata:
	err = TBADDATA;
	ptrace(("%s: ERROR: bad connection data\n", DRV_NAME));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: ERROR: no permission for address or option\n", DRV_NAME));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: ERROR: address is unusable\n", DRV_NAME));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid message format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_CONN_REQ, err);
}

/*
 *  T_CONN_RES           1 - Accept previous connection indication
 *  -------------------------------------------------------------------
 */
STATIC mblk_t *
t_seq_check(ss_t *ss, t_uscalar_t seq)
{
	mblk_t *mp;

	spin_lock_bh(&ss->conq.q_lock);
	for (mp = bufq_head(&ss->conq); mp && (t_uscalar_t) (long) mp != seq; mp = mp->b_next) ;
	spin_unlock_bh(&ss->conq.q_lock);
	usual(mp);
	return (mp);
}
STATIC streams_fastcall ss_t *
t_tok_check(t_uscalar_t tok)
{
	ss_t *as;

	spin_lock_bh(&ss_lock);
	for (as = ss_opens; as && (t_uscalar_t) (long) as->rq != tok; as = as->next) ;
	spin_unlock_bh(&ss_lock);
	usual(as);
	return (as);
}
STATIC streams_fastcall int
t_conn_res(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	int err = 0;
	mblk_t *cp;
	ss_t *as;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	unsigned char *opt;

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto inval;
	if (ss_get_state(ss) != TS_WRES_CIND)
		goto outstate;
	ss_set_state(ss, TS_WACK_CRES);
	if ((cp = t_seq_check(ss, p->SEQ_number)) == NULL)
		goto badseq;
	if (((as = t_tok_check(p->ACCEPTOR_id)) == NULL)
	    || !(as == ss || ((1 << ss_get_state(as)) & TSM_DISCONN)))
		goto badf;
	if (ss->p.prot.protocol != as->p.prot.protocol)
		goto provmismatch;
	if (ss_get_state(as) == TS_IDLE && as->conind)
		goto resqlen;
	if (ss->cred.cr_uid != 0 && as->cred.cr_uid == 0)
		goto acces;
	opt = mp->b_rptr + p->OPT_offset;
	if (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
	if (mp->b_cont) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.CDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate connect with data */
	}
	if ((err = t_parse_conn_opts(as, opt, p->OPT_length, 0)) < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto error;
		}
	}
	/* FIXME: options will be processed on wrong socket!!! When we accept, we will delete this
	   socket and create another one that was derived from the listening socket.  We need to
	   process the options against that socket, not the placeholder.  One way to fix this is to
	   set flags when we process options against the stream structure and then reprocess those
	   options once the sockets have been swapped.  See t_ok_ack for details. */
	/* FIXME: The accepting socket does not have to be in the bound state.  The socket will be
	   autobound to the correct address already. */
	return t_ok_ack(q, T_CONN_RES, cp, as);
      baddata:
	err = TBADDATA;
	ptrace(("%s: ERROR: bad connection data\n", DRV_NAME));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: ERROR: no access to accepting queue\n", DRV_NAME));
	goto error;
      resqlen:
	err = TRESQLEN;
	ptrace(("%s: ERROR: accepting queue is listening\n", DRV_NAME));
	goto error;
      provmismatch:
	err = TPROVMISMATCH;
	ptrace(("%s: ERROR: not same transport provider\n", DRV_NAME));
	goto error;
      badf:
	err = TBADF;
	ptrace(("%s: ERROR: accepting queue id is invalid\n", DRV_NAME));
	goto error;
      badseq:
	err = TBADSEQ;
	ptrace(("%s: ERROR: sequence number is invalid\n", DRV_NAME));
	goto error;
      inval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_CONN_RES, err);
}

/*
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_discon_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	int err;
	mblk_t *cp = NULL;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << ss_get_state(ss)) & ~TSM_CONNECTED)
		goto outstate;
	if (mp->b_cont) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate disconnect with data */
	}
	switch (ss_get_state(ss)) {
	case TS_WCON_CREQ:
		ss_set_state(ss, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		if ((cp = t_seq_check(ss, p->SEQ_number))) {
			ss_set_state(ss, TS_WACK_DREQ7);
			break;
		}
		goto badseq;
	case TS_DATA_XFER:
		ss_set_state(ss, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		ss_set_state(ss, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		ss_set_state(ss, TS_WACK_DREQ11);
		break;
	}
	return t_ok_ack(q, T_DISCON_REQ, cp, NULL);
      badseq:
	err = TBADSEQ;
	ptrace(("%s: ERROR: sequence number is invalid\n", DRV_NAME));
	goto error;
      baddata:
	err = TBADDATA;
	ptrace(("%s: ERROR: bad disconnection data\n", DRV_NAME));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_DISCON_REQ, err);
}

/*
 *  M_DATA
 *  -------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_write int
ss_w_data(queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	struct msghdr msg;
	ss_t *ss = PRIV(q);

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (ss_get_state(ss) == TS_IDLE)
		goto discard;
	if ((1 << ss_get_state(ss)) & ~TSM_OUTDATA)
		goto outstate;
	mlen = msgdsize(mp);
	if (((mmax = ss->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = (ss->p.prot.type == SOCK_SEQPACKET) ? MSG_EOR : 0;
	return ss_sock_sendmsg(ss, mp, &msg);
      emsgsize:
	ptrace(("%s: ERROR: message too large %ld > %ld\n", DRV_NAME, mlen, mmax));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

/*
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_out int
t_data_req(queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	ss_t *ss = PRIV(q);
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	struct msghdr msg;

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ss_get_state(ss) == TS_IDLE)
		goto discard;
	if ((1 << ss_get_state(ss)) & ~TSM_OUTDATA)
		goto outstate;
	mlen = msgdsize(mp);
	if (((mmax = ss->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = (ss->p.prot.type == SOCK_SEQPACKET && !p->MORE_flag) ? MSG_EOR : 0;
	return ss_sock_sendmsg(ss, mp, &msg);
      emsgsize:
	ptrace(("%s: ERROR: message too large %ld > %ld\n", DRV_NAME, mlen, mmax));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_put int
t_exdata_req(queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	ss_t *ss = PRIV(q);
	const struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	struct msghdr msg;

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ss_get_state(ss) == TS_IDLE)
		goto discard;
	if ((1 << ss_get_state(ss)) & ~TSM_OUTDATA)
		goto outstate;
	mlen = msgdsize(mp);
	if (((mmax = ss->p.info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags =
	    MSG_OOB | ((ss->p.prot.type == SOCK_SEQPACKET && !p->MORE_flag) ? MSG_EOR : 0);
	return ss_sock_sendmsg(ss, mp, &msg);
      emsgsize:
	ptrace(("%s: ERROR: message too large %ld > %ld\n", DRV_NAME, mlen, mmax));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_info_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);

	(void) mp;
	(void) ss;
	return t_info_ack(q);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_bind_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	int err, add_len;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr *add = &ss->src;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ss_get_state(ss) != TS_UNBND)
		goto outstate;
	ss_set_state(ss, TS_WACK_BREQ);
#if 0
	/* we are supposed to ignore CONIND_number for T_CLTS according to XTI */
	if (ss->p.info.SERV_type == T_CLTS && p->CONIND_number)
		goto notsupport;
#endif
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)) {
		ptrace(("%s: %p: ADDR_offset(%u) or ADDR_length(%u) are incorrect\n", DRV_NAME, ss,
			p->ADDR_offset, p->ADDR_length));
		goto badaddr;
	}
	switch (ss->p.prot.family) {
	case AF_INET:
	{
		struct sockaddr_in *add_in;

		if ((add_len = p->ADDR_length) == 0) {
			if (ss->p.prot.type == SOCK_RAW)
				goto noaddr;
			bzero(add, sizeof(add));
			add_len = sizeof(struct sockaddr_in);
			add->sa_family = AF_INET;
		} else {
			if (add_len < sizeof(struct sockaddr_in)) {
				ptrace(("%s: %p: add_len(%d) < sizeof(struct sockaddr_in)(%lu)\n", DRV_NAME, ss, add_len, (ulong) sizeof(struct sockaddr_in)));
				goto badaddr;
			}
			if (ss->p.prot.protocol != T_INET_SCTP) {
				if (add_len > sizeof(struct sockaddr_in)) {
					ptrace(("%s: %p: add_len(%d) > sizeof(struct sockaddr_in)(%lu)\n", DRV_NAME, ss, add_len, (ulong) sizeof(struct sockaddr_in)));
					goto badaddr;
				}
			} else {
				if ((add_len % sizeof(struct sockaddr_in)) != 0) {
					ptrace(("%s: %p: add_len(%d) %% sizeof(struct sockaddr_in)(%lu)\n", DRV_NAME, ss, add_len, (ulong) sizeof(struct sockaddr_in)));
					goto badaddr;
				}
			}
			if (add->sa_family != AF_INET && add->sa_family != 0) {
				ptrace(("%s: %p: sa_family incorrect (%u)\n", DRV_NAME, ss,
					add->sa_family));
				goto badaddr;
			}
			add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		}
		add_in = (typeof(add_in)) add;
		if (ss->p.prot.type == SOCK_RAW && ss->cred.cr_uid != 0)
			goto acces;
		ss->port = ntohs(add_in->sin_port);
		/* raw sockets really do not like being fed zero protocol numbers */
		if (ss->p.prot.type == SOCK_RAW && ss->port == 0)
			goto noaddr;
		break;
	}
	case AF_UNIX:
	{
		struct sockaddr_un *add_un;

		if ((add_len = p->ADDR_length) == 0) {
			bzero(add, sizeof(add));
			add_len = sizeof(struct sockaddr_un);
			add->sa_family = AF_UNIX;
		} else {
			if (add_len < sizeof(struct sockaddr_un)) {
				ptrace(("%s: %p: add_len(%d) < sizeof(struct sockaddr_un)(%lu)\n", DRV_NAME, ss, add_len, (ulong) sizeof(struct sockaddr_un)));
				goto badaddr;
			}
			if (add->sa_family != AF_UNIX && add->sa_family != 0) {
				ptrace(("%s: %p: sa_family incorrect (%u)\n", DRV_NAME, ss,
					add->sa_family));
				goto badaddr;
			}
			add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		}
		add_un = (typeof(add_un)) add;
		ss->port = 0;
		break;
	}
	default:
		ptrace(("%s: %p: protocol family incorrect %u\n", DRV_NAME, ss, ss->p.prot.family));
		goto badaddr;
	}
	if ((err = ss_bind(ss, add, add_len)))
		goto error_close;
	ss->conind = 0;
	if (p->CONIND_number
	    && (ss->p.prot.type == SOCK_STREAM || ss->p.prot.type == SOCK_SEQPACKET))
		if ((err = ss_listen(ss, p->CONIND_number)))
			goto error_close;
	return t_bind_ack(q, (ss_getsockname(ss) <= 0) ? NULL : &ss->src, p->CONIND_number);
      acces:
	err = TACCES;
	ptrace(("%s: ERROR: no permission for address\n", DRV_NAME));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: ERROR: address could not be assigned\n", DRV_NAME));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: ERROR: address is invalid\n", DRV_NAME));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
#if 0
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
#endif
      error_close:
	ss_socket_put(xchg(&ss->sock, NULL));
      error:
	return t_error_ack(q, T_BIND_REQ, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);

	if (ss_get_state(ss) != TS_IDLE)
		goto outstate;
	ss_set_state(ss, TS_WACK_UREQ);
	return t_ok_ack(q, T_UNBIND_REQ, NULL, NULL);
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	return t_error_ack(q, T_UNBIND_REQ, TOUTSTATE);
}

/*
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_out int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	long mmax;
	ss_t *ss = PRIV(q);
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;

	if (ss->p.info.SERV_type != T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (dlen == 0 && !(ss->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (((mmax = ss->p.info.TSDU_size) > 0 && dlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && dlen > mmax) || mmax == T_INVALID)
		goto baddata;
	if (ss_get_state(ss) != TS_IDLE)
		goto outstate;
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->DEST_length < sizeof(ss->dst.sa_family))
	    || (p->DEST_length <
		ss_addr_size(ss, (struct sockaddr *) (mp->b_rptr + p->DEST_length))))
		goto badadd;
	if (ss->p.prot.type == SOCK_RAW && ss->cred.cr_uid != 0)
		goto acces;
	else {
		int cmsg_len;
		size_t opt_len = p->OPT_length;
		size_t opt_off = p->OPT_offset;
		unsigned char *opt = mp->b_rptr + opt_off;

		if ((cmsg_len = ss_cmsg_size(ss, opt, opt_len)) >= 0) {
			struct msghdr msg;
			unsigned char cmsg[cmsg_len];
			struct sockaddr *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);

			msg.msg_name = dst;
			msg.msg_namelen = ss_addr_size(ss, dst);
			msg.msg_control = cmsg_len ? cmsg : NULL;
			msg.msg_controllen = cmsg_len;
			msg.msg_flags = MSG_NOSIGNAL | MSG_DONTWAIT;
			if (ss_cmsg_build(ss, opt, opt_len, &msg) < 0)
				goto badopt;
			return ss_sock_sendmsg(ss, mp, &msg);
		}
		switch (-cmsg_len) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto error;
		}
	}
	/* FIXME: we can send uderr for some of these instead of erroring out the entire stream. */
      badopt:
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
      acces:
	ptrace(("%s: ERROR: no permission to address or options\n", DRV_NAME));
	goto error;
      badadd:
	ptrace(("%s: ERROR: bad destination address\n", DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive\n", DRV_NAME));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      baddata:
	ptrace(("%s: ERROR: bad data size\n", DRV_NAME));
	goto error;
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_COTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------
 *  The T_OPTMGMT_REQ is responsible for establishing options while the stream is in the T_IDLE
 *  state.  When the stream is bound to a local address using the T_BIND_REQ, the settings of
 *  options with end-to-end significance will have an affect on how the driver response to an INIT
 *  with INIT-ACK for SCTP.  For example, the bound list of addresses is the list of addresses that
 *  will be sent in the INIT-ACK.  The number of inbound streams and outbound streams are the
 *  numbers that will be used in the INIT-ACK.
 */
/*
 *  Errors:
 *
 *  TACCES:     the user did not have proper permissions for the user of the requested options.
 *
 *  TBADFLAG:   the flags as specified were incorrect or invalid.
 *
 *  TBADOPT:    the options as specified were in an incorrect format, or they contained invalid
 *		information.
 *
 *  TOUTSTATE:  the primitive would place the transport interface out of state.
 *
 *  TNOTSUPPORT: this prmitive is not supported.
 *
 *  TSYSERR:    a system error has occured and the UNIX system error is indicated in the primitive.
 */
STATIC INLINE streams_fastcall __hot_put int
t_optmgmt_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	int err, opt_len;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
#ifdef TS_WACK_OPTREQ
	if (ss_get_state(ss) == TS_IDLE)
		ss_set_state(ss, TS_WACK_OPTREQ);
#endif
	if (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
	switch (p->MGMT_flags) {
	case T_DEFAULT:
		opt_len = t_size_default_options(ss, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CURRENT:
		opt_len = t_size_current_options(ss, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CHECK:
		opt_len = t_size_check_options(ss, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_NEGOTIATE:
		opt_len = t_size_negotiate_options(ss, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	default:
		goto badflag;
	}
	if (opt_len < 0) {
		switch (-(err = opt_len)) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto provspec;
		}
	}
	if ((err = t_optmgmt_ack(q, p->MGMT_flags, mp->b_rptr + p->OPT_offset,
				 p->OPT_length, opt_len)) < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		case ENOBUFS:
		case ENOMEM:
			break;
		default:
			goto provspec;
		}
	}
	return (err);
      provspec:
	err = err;
	ptrace(("%s: ERROR: provider specific\n", DRV_NAME));
	goto error;
      badflag:
	err = TBADFLAG;
	ptrace(("%s: ERROR: bad options flags\n", DRV_NAME));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: ERROR: no permission for option\n", DRV_NAME));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_ordrel_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if (ss->p.info.SERV_type != T_COTS_ORD)
		goto notsupport;
	if ((1 << ss_get_state(ss)) & ~TSM_OUTDATA)
		goto outstate;
	if (mp->b_cont) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate orderly release with data */
	}
	ss->sock->ops->shutdown(ss->sock, SEND_SHUTDOWN - 1);	/* XXX */
	switch (ss_get_state(ss)) {
	case TS_DATA_XFER:
		ss_set_state(ss, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		ss_set_state(ss, TS_IDLE);
		break;
	}
	return (QR_DONE);
      baddata:
	ptrace(("%s: ERROR: bad orderly release data\n", DRV_NAME));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS or T_COTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

/*
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_out int
t_optdata_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;

	if (ss->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ss_get_state(ss) == TS_IDLE)
		goto discard;
	if ((1 << ss_get_state(ss)) & ~TSM_OUTDATA)
		goto outstate;
	else {
		int cmsg_len;
		size_t opt_len = p->OPT_length;
		size_t opt_off = p->OPT_offset;
		unsigned char *opt = mp->b_rptr + opt_off;

		if ((cmsg_len = ss_cmsg_size(ss, opt, opt_len)) >= 0) {
			struct msghdr msg;
			unsigned char cmsg[cmsg_len];

			msg.msg_name = NULL;
			msg.msg_namelen = 0;
			msg.msg_control = &cmsg;
			msg.msg_controllen = cmsg_len;
			msg.msg_flags = MSG_NOSIGNAL | MSG_DONTWAIT;
			if (ss_cmsg_build(ss, opt, opt_len, &msg) < 0)
				goto badopt;
			if ((p->DATA_flag & T_ODF_EX))
				msg.msg_flags |= MSG_OOB;
			if (!(p->DATA_flag & T_ODF_MORE) && ss->p.prot.type == SOCK_SEQPACKET)
				msg.msg_flags |= MSG_EOR;
			return ss_sock_sendmsg(ss, mp, &msg);
		}
		switch (cmsg_len) {
		case -EINVAL:
			goto badopt;
		case -EACCES:
			goto acces;
		}
		goto badopt;
	}
      badopt:
	ptrace(("%s: ERROR: bad options\n", DRV_NAME));
	goto error;
      acces:
	ptrace(("%s: ERROR: no permission to options\n", DRV_NAME));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, EPROTO);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_addr_req(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	struct sockaddr *loc = NULL;
	struct sockaddr *rem = NULL;

	(void) mp;
	switch (ss_get_state(ss)) {
	case TS_UNBND:
		break;
	case TS_IDLE:
		ss_getsockname(ss);
		loc = &ss->src;
		break;
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		ss_getnames(ss);
		loc = &ss->src;
		rem = &ss->dst;
		break;
	case TS_WRES_CIND:
		ss_getpeername(ss);
		rem = &ss->dst;
		break;
	default:
		goto outstate;
	}
	return t_addr_ack(q, loc, rem);
      outstate:
	return t_error_ack(q, T_ADDR_REQ, TOUTSTATE);
}
#endif
#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
STATIC streams_fastcall int
t_capability_req(queue_t *q, mblk_t *mp)
{
	int err = -EFAULT;
	ss_t *ss = PRIV(q);
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	(void) ss;
	return t_capability_ack(q, p->CAP_bits1, mp->b_datap->db_type);
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid message format\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_CAPABILITY_REQ, err);
}
#endif
/*
 *  Other primitives    XX - other invalid primitives
 *  -------------------------------------------------------------------------
 */
STATIC int
t_other_req(queue_t *q, mblk_t *mp)
{
	t_scalar_t prim = *((t_scalar_t *) mp->b_rptr);

	return t_error_ack(q, prim, TNOTSUPPORT);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall __hot_out int
ss_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	t_scalar_t prim;
	ss_t *ss = PRIV(q);
	t_uscalar_t oldstate = ss_get_state(ss);

	if (mp->b_wptr < mp->b_rptr + sizeof(t_scalar_t))
		goto eproto;
	switch ((prim = *((t_scalar_t *) mp->b_rptr))) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, ss));
		rtn = t_conn_req(q, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, ss));
		rtn = t_conn_res(q, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, ss));
		rtn = t_discon_req(q, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, ss));
		rtn = t_data_req(q, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, ss));
		rtn = t_exdata_req(q, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, ss));
		rtn = t_info_req(q, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, ss));
		rtn = t_bind_req(q, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, ss));
		rtn = t_unbind_req(q, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, ss));
		rtn = t_optmgmt_req(q, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, ss));
		rtn = t_unitdata_req(q, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, ss));
		rtn = t_ordrel_req(q, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, ss));
		rtn = t_optdata_req(q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, ss));
		rtn = t_addr_req(q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, ss));
		rtn = t_capability_req(q, mp);
		break;
#endif
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
#ifdef T_CAPABILITY_ACK
	case T_CAPABILITY_ACK:
#endif
	      eproto:
		rtn = m_error(q, EPROTO);
		break;
	default:
		rtn = t_other_req(q, mp);
		break;
	}
	if (rtn < 0) {
#ifndef _TEST
		/* not so rare() during conformance suite testing */
		rare();
#endif
		ss_set_state(ss, oldstate);
		/* The put and srv procedures do not recognize all errors.  Sometimes we return an
		   error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
			break;
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE streams_fastcall int
ss_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(OTHERQ(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(OTHERQ(q), FLUSHALL);
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 *  On the read queue, an M_PCRSE is used to indicate a sk_state_change callback
 *  on the socket.  This can mean that the socket has completed connecting or
 *  has disconnected, or it may signal a connection indication on a listening
 *  socket.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  ss->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 */
STATIC INLINE streams_fastcall __hot int
ss_r_pcrse(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	ss_event_t *p = (typeof(p)) mp->b_rptr;
	int oldstate = xchg(&ss->tcp_state, p->state);

	if (!ss->sock)
		goto discard;
#if 0
	assure(p->state != oldstate);
#endif
	printd(("%s: %p: sk_state_change [%s <- %s] %p\n", DRV_NAME, ss, tcp_state_name(p->state),
		tcp_state_name(oldstate), p->sk));
	switch (ss->p.prot.type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (ss_get_state(ss)) {
		case TS_WCON_CREQ:
			switch (p->state) {
			case TCP_ESTABLISHED:
				ss_getpeername(ss);
				return (t_conn_con(q, &ss->dst, NULL));
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				ss_getpeername(ss);
				return (t_discon_ind(q, &ss->dst, T_PROVIDER, 0, NULL, NULL));
			}
			break;
		case TS_WIND_ORDREL:
			switch (p->state) {
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (oldstate) {
				case TCP_FIN_WAIT2:
					return (t_ordrel_ind(q));
				case TCP_ESTABLISHED:
				case TCP_FIN_WAIT1:
					return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
				}
				break;
			case TCP_FIN_WAIT1:
			case TCP_FIN_WAIT2:
				return (QR_DONE);
			}
			break;
		case TS_DATA_XFER:
			switch (p->state) {
			case TCP_CLOSE_WAIT:
				return (t_ordrel_ind(q));
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
			}
			break;
		case TS_WREQ_ORDREL:
			switch (p->state) {
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (oldstate) {
				case TCP_CLOSE_WAIT:
					return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
				}
				break;
			}
			break;
		case TS_IDLE:
		case TS_WRES_CIND:
			switch (p->state) {
			case TCP_LAST_ACK:
				switch (oldstate) {
				case TCP_CLOSE_WAIT:
					return (QR_DONE);
				}
				break;
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (oldstate) {
				case TCP_FIN_WAIT2:
				case TCP_LAST_ACK:
					return (QR_DONE);
				case TCP_LISTEN:
				{
					mblk_t *cp;

					/* state change was on child */
					ss->tcp_state = TCP_LISTEN;
					/* look for the child */
					if ((cp = t_seq_find(ss, mp)))
						return (t_discon_ind
							(q, NULL, T_PROVIDER, 0, cp, NULL));
					return (QR_DONE);
				}
				}
				break;
			case TCP_ESTABLISHED:
				switch (oldstate) {
				case TCP_LISTEN:
					/* state change was on child */
					ss->tcp_state = TCP_LISTEN;
					return (ss_conn_ind(q, mp));
				}
				break;
			}
			break;
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	default:
		printd(("%s: SWERR: unsupported socket type\n", DRV_NAME));
		return (-EFAULT);
	}
      discard:
	printd(("%s: ERROR: lingering event, ignoring\n", DRV_NAME));
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
/**
 * ss_r_data - process an M_DATA message on the read queue
 * @q: active queue in pair (read queue)
 * @mp: the M_DATA message
 *
 * On the read queue, an M_DATA is used to save queued data against the queue.  This is data that
 * ss_sock_recvmsg() has retrieved from the underlying socket, but which has not yet been formatted
 * into T_DATA_IND, T_EXDATA_IND, T_UINITDATA_IND or T_UDERROR_IND messages.  These M_DATA blocks
 * are specially formatted: they contain a complete returned cmsg header ahead of the data.  The
 * cmsg header starts at mp->b_databp->db_base and ends at mp->b_rptr-1.  The message data itself
 * starts at mp->b_rptr and ends at mp->b_wptr-1.
 *
 */
STATIC streams_fastcall int __hot_in
ss_r_data(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	struct msghdr *msg = (struct msghdr *) mp->b_datap->db_base;
	int err;

	if (ss->p.info.SERV_type == T_CLTS) {
		err = (msg->msg_flags & MSG_ERRQUEUE) ?
		    t_uderror_ind(q, msg, mp) : t_unitdata_ind(q, msg, mp);
	} else if (!msg->msg_controllen) {
		err = (msg->msg_flags & MSG_OOB) ?
		    t_exdata_ind(q, msg, mp) : t_data_ind(q, msg, mp);
	} else {
		err = t_optdata_ind(q, msg, mp);
	}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_READ Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  M_READ on the RD(q)
 *  -------------------------------------------------------------------------
 *
 *  On the read queue, an M_READ is used to indicate a sk_data_ready callback on
 *  the socket.  This can mean that the socket is now readable (when it was
 *  blocked before), or it may mean that a socket has completed connecting, or
 *  disconnected, or that a connection indication may be availabel on a
 *  listening socket.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  ss->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 */
STATIC INLINE streams_fastcall __hot_in int
ss_r_read(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	ss_event_t *p = (typeof(p)) mp->b_rptr;

	if (!ss->sock)
		goto discard;
	assure(ss->tcp_state == p->state || ss->tcp_state == TCP_LISTEN || p->state == TCP_CLOSE
	       || p->state == TCP_CLOSE_WAIT || p->state == TCP_FIN_WAIT1
	       || p->state == TCP_FIN_WAIT2);
	printd(("%s: %p: sk_data_ready [%s <- %s] %p\n", DRV_NAME, ss, tcp_state_name(p->state),
		tcp_state_name(ss->tcp_state), p->sk));
	switch (ss->p.prot.type) {
	case SOCK_STREAM:	/* TCP */
	case SOCK_SEQPACKET:	/* SCTP */
		switch (ss_get_state(ss)) {
		case TS_IDLE:
		case TS_WRES_CIND:
			if (p->state == TCP_LISTEN)
				return (QR_DONE);
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:	/* TCP bug I believe */
			return ss_sock_recvmsg(q);
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (ss_get_state(ss)) {
		case TS_IDLE:
			return ss_sock_recvmsg(q);
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	}
	printd(("%s: SWERR: unsupported socket type %d\n", DRV_NAME, ss->p.prot.type));
	return (-EFAULT);
      discard:
	printd(("%s: ERROR: lingering event, ignoring\n", DRV_NAME));
	return (QR_DONE);
}

/*
 *  M_READ on the WR(q)
 *  -------------------------------------------------------------------------
 *
 *  On the write queue, an M_READ is used to indicate a sk_write_space callback
 *  on the socket.  This can mean that the socket is now writeable (when it
 *  was blocked before).  This can also mean that a connecting socket has
 *  completed connecting (or has disconnected), or that a listening socket has
 *  a connection indication on the socket.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  ss->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 *
 */
STATIC INLINE streams_fastcall __hot_out int
ss_w_read(queue_t *q, mblk_t *mp)
{
	ss_t *ss = PRIV(q);
	ss_event_t *p = (typeof(p)) mp->b_rptr;

	if (!ss->sock)
		goto discard;
	assure(ss->tcp_state == p->state || ss->tcp_state == TCP_LISTEN);
	printd(("%s: %p: sk_write_space [%s <- %s] %p\n", DRV_NAME, ss, tcp_state_name(p->state),
		tcp_state_name(ss->tcp_state), p->sk));
	switch (ss->p.prot.type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (ss_get_state(ss)) {
		case TS_DATA_XFER:
		case TS_WREQ_ORDREL:
			qenable(ss->wq);
			return (QR_DONE);
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (ss_get_state(ss)) {
		case TS_IDLE:
			qenable(ss->wq);
			return (QR_DONE);
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	}
	printd(("%s: SWERR: unsupported socket type %d\n", DRV_NAME, ss->p.prot.type));
	return (-EFAULT);
      discard:
	printd(("%s: ERROR: lingering event, ignoring\n", DRV_NAME));
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  On the read queue, an M_ERROR is used to indicate an sk_error_report callback
 *  on the socket.  This can mean that the socket has disconected.  We might
 *  not received any other indication of the error.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  ss->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 */
STATIC INLINE streams_fastcall __hot_get int
ss_r_error(queue_t *q, mblk_t *mp)
{
	int err;
	ss_t *ss = PRIV(q);
	ss_event_t *p = (typeof(p)) mp->b_rptr;

	err = sock_error(p->sk);
	if (!ss->sock)
		goto discard;
#if 0
	assure(ss->tcp_state == p->state || ss->tcp_state == TCP_LISTEN);
#endif
	printd(("%s: %p: sk_error_report [%s <- %s] %p\n", DRV_NAME, ss, tcp_state_name(p->state),
		tcp_state_name(ss->tcp_state), p->sk));
	switch (ss->p.prot.type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (ss_get_state(ss)) {
		case TS_WCON_CREQ:
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
			switch (-err) {
			case ECONNREFUSED:
			case EPIPE:
			case ECONNRESET:
				return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
			default:
				return (QR_DONE);
			}
#if 0
			fixme(("%s: %p: FIXME: save errors for later\n", DRV_NAME, ss));
#endif
		default:
		case TS_IDLE:
		case TS_WRES_CIND:
			printd(("%s: %p: INFO: ignoring error event %d\n", DRV_NAME, ss, err));
			return (QR_DONE);
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (ss_get_state(ss)) {
		case TS_IDLE:
			fixme(("%s: %p: FIXME: generate uderror\n", DRV_NAME, ss));
			printd(("%s: %p: INFO: ignoring error event %d\n", DRV_NAME, ss, err));
			return (QR_DONE);
		}
		printd(("%s: %p: SWERR: socket state %s in TPI state %s\n", DRV_NAME, ss,
			tcp_state_name(p->state), state_name(ss_get_state(ss))));
		return (-EFAULT);
	}
	printd(("%s: SWERR: unsupported socket type %d\n", DRV_NAME, ss->p.prot.type));
	return (-EFAULT);
      discard:
	printd(("%s: ERROR: lingering event, ignoring\n", DRV_NAME));
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 *
 *  WRITE PUT ad SERVICE (Message from above IP-User --> IP-Provider
 *  -------------------------------------------------------------------------
 */
STATIC streams_fastcall __hot_out int
ss_w_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ss_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ss_w_proto(q, mp);
	case M_CTL:
	case M_READ:
		return ss_w_read(q, mp);
	case M_FLUSH:
		return ss_w_flush(q, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  READ PUT ad SERVICE (Message from below IP-Provider --> IP-User
 *  -------------------------------------------------------------------------
 */
STATIC streams_fastcall __hot_in int
ss_r_prim(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(mp);
	assert(mp->b_datap);

	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return ss_r_pcrse(q, mp);
	case M_CTL:
	case M_READ:
		return ss_r_read(q, mp);
	case M_ERROR:
		return ss_r_error(q, mp);
	case M_DATA:
		/* actually, rather unlikely */
		return ss_r_data(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  READ SIDE wakeup procedure (called on each service procedure exit)
 *  -------------------------------------------------------------------------
 */
STATIC streams_fastcall __hot_in void
ss_r_wakeup(queue_t *q)
{
	if (((ss_t *) PRIV(q))->sock != NULL)
		ss_sock_recvmsg(q);
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 *
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE streams_fastcall __hot int
ss_putq(queue_t *q, mblk_t *mp, streams_fastcall int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0, locked;

	if (mp->b_datap->db_type < QPCTL && (q->q_first || q->q_flag & QSVCBUSY)) {
		if (!putq(q, mp))
			freemsg(mp);	/* FIXME */
		return (0);
	}
	if ((locked = ss_trylockq(q)) || mp->b_datap->db_type == M_FLUSH) {
		do {
			/* Fast Path */
			if ((rtn = (*proc) (q, mp)) == QR_DONE) {
				freemsg(mp);
				break;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				break;
			case QR_STRIP:
				if (mp->b_cont)
					if (!putq(q, mp->b_cont))
						freemsg(mp->b_cont);	/* FIXME */
			case QR_TRIMMED:
				freeb(mp);
				break;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					break;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					break;
				}
				rtn = -EOPNOTSUPP;
			default:
				freemsg(mp);
				break;
			case QR_DISABLE:
				if (!putq(q, mp))
					freemsg(mp);	/* FIXME */
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					break;
				}
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
			case -ENOMEM:
				if (!putq(q, mp))
					freemsg(mp);	/* FIXME */
				break;
			}
		}
		while (0);
		if (locked)
			ss_unlockq(q);
	} else {
		if (!putq(q, mp))
			freemsg(mp);	/* FIXME */
	}
	return (rtn);
}

/*
 *  SRVQ Service Routine
 *  -----------------------------------
 */
STATIC INLINE streams_fastcall __hot int
ss_srvq(queue_t *q, streams_fastcall int (*proc) (queue_t *, mblk_t *),
	streams_fastcall void (*procwake) (queue_t *))
{
	int rtn = 0;

	if (ss_trylockq(q)) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			/* Fast Path */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				freemsg(mp);
				continue;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_STRIP:
				if (mp->b_cont)
					if (!putbq(q, mp->b_cont))
						freemsg(mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				ptrace(("%s: ERROR: (q dropping) %d\n", DRV_NAME, rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				ptrace(("%s: ERROR: (q disabling)\n", DRV_NAME));
				noenable(q);
				if (!putbq(q, mp))
					freemsg(mp);	/* FIXME */
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* caller must schedule bufcall */
			case -EBUSY:	/* caller must have failed canput */
			case -EAGAIN:	/* caller must re-enable queue */
			case -ENOMEM:	/* caller must re-enable queue */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("%s: ERROR: (q stalled) %d\n", DRV_NAME, rtn));
					if (!putbq(q, mp))
						freemsg(mp);	/* FIXME */
					break;
				}
				/* Be careful not to put a priority message back on the queue.  */
				switch (mp->b_datap->db_type) {
				case M_PCPROTO:
					mp->b_datap->db_type = M_PROTO;
					break;
				case M_PCRSE:
					mp->b_datap->db_type = M_RSE;
					break;
				case M_READ:
					mp->b_datap->db_type = M_CTL;
					break;
				default:
					ptrace(("%s: ERROR: (q dropping) %d\n", DRV_NAME, rtn));
					freemsg(mp);
					continue;
				}
				mp->b_band = 255;
				if (!putq(q, mp))
					freemsg(mp);	/* FIXME */
				break;
			}
			break;
		}
		if (procwake != NULL)
			procwake(q);
		ss_unlockq(q);
	}
	return (rtn);
}

STATIC streamscall __hot_in int
ss_rput(queue_t *q, mblk_t *mp)
{
	return ss_putq(q, mp, &ss_r_prim);
}

STATIC streamscall __hot_in int
ss_rsrv(queue_t *q)
{
	return ss_srvq(q, &ss_r_prim, &ss_r_wakeup);
}

STATIC streamscall __hot_out int
ss_wput(queue_t *q, mblk_t *mp)
{
	return ss_putq(q, mp, &ss_w_prim);
}

STATIC streamscall __hot_out int
ss_wsrv(queue_t *q)
{
	return ss_srvq(q, &ss_w_prim, NULL);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *ss_priv_cachep = NULL;
STATIC int
ss_init_caches(void)
{
	if (!ss_priv_cachep
	    && !(ss_priv_cachep =
		 kmem_cache_create("ss_priv_cachep", sizeof(ss_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate ss_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	return (0);
}
STATIC int
ss_term_caches(void)
{
	if (ss_priv_cachep) {
		if (kmem_cache_destroy(ss_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ss_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed ss_priv_cachep\n", DRV_NAME));
	}
	return (0);
}
STATIC ss_t *
ss_alloc_priv(queue_t *q, ss_t **slp, major_t cmajor, minor_t cminor, cred_t *crp,
	      const ss_profile_t * prof)
{
	ss_t *ss;

	if ((ss = kmem_cache_alloc(ss_priv_cachep, SLAB_ATOMIC))) {
#if 0
		printd(("%s: allocated module private structure\n", DRV_NAME));
#endif
		bzero(ss, sizeof(*ss));
		ss->cmajor = cmajor;
		ss->cminor = cminor;
		ss->rq = RD(q);
		ss->rq->q_ptr = ss;
		ss->refcnt++;
		ss->wq = WR(q);
		ss->wq->q_ptr = ss;
		ss->refcnt++;
		ss->cred = *crp;
		ss->p = *prof;
		spin_lock_init(&ss->qlock);
		ss->rwait = NULL;
		ss->wwait = NULL;
		ss->users = 0;
		ss_set_state(ss, TS_UNBND);
		spin_lock_init(&ss->lock);
		bufq_init(&ss->conq);
		if ((ss->next = *slp))
			ss->next->prev = &ss->next;
		ss->prev = slp;
		*slp = ss;
		ss->refcnt++;
#if 0
		printd(("%s: linked private structure, reference count %d\n", DRV_NAME,
			ss->refcnt));
#endif
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
	return (ss);
}
STATIC void
ss_free_priv(queue_t *q)
{
	ss_t *ss = PRIV(q);

	printd(("%s: unlinking private structure, reference count = %lu\n", DRV_NAME, (ulong) ss->refcnt));
	/* Unfortunately, ss_socket_put calls sock_release which can cause TCP to do a
	   tcp_send_fin, and in tcp_send_fin an skbuff is allocated with GFP_KERNEL. */
	if (ss->sock)
		ss_socket_put(xchg(&ss->sock, NULL));
	printd(("%s: removed socket, reference count = %lu\n", DRV_NAME, (ulong) ss->refcnt));
	spin_lock_bh(&ss->lock);
	{
		bufq_purge(&ss->conq);
		__ss_unbufcall(q);
		printd(("%s: removed bufcalls, reference count = %lu\n", DRV_NAME, (ulong) ss->refcnt));
		spin_lock(&ss_lock);
		if ((*ss->prev = ss->next))
			ss->next->prev = ss->prev;
		ss->next = NULL;
		ss->prev = NULL;
		spin_unlock(&ss_lock);
		ss->refcnt--;
		printd(("%s: unlinked, reference count = %lu\n", DRV_NAME, (ulong) ss->refcnt));
		ss->rq->q_ptr = NULL;
		ss->refcnt--;
		ss->wq->q_ptr = NULL;
		ss->refcnt--;
	}
	spin_unlock_bh(&ss->lock);
	if (ss->refcnt) {
		assure(ss->refcnt);
		printd(("%s: WARNING: ss->refcnt = %lu\n", DRV_NAME, (ulong) ss->refcnt));
	}
	kmem_cache_free(ss_priv_cachep, ss);
#if 0
	printd(("%s: freed module private structure\n", DRV_NAME));
#endif
	return;
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC const ss_profile_t ss_profiles[] = {
	{{PF_INET, SOCK_RAW, IPPROTO_ICMP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_IGMP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_IPIP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_STREAM, IPPROTO_TCP},
	 {T_INFO_ACK, 0, 1, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65535, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_EGP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_PUP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_DGRAM, IPPROTO_UDP},
	 {T_INFO_ACK, 65507, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65507, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_IDP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_RAW},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_UNIX, SOCK_STREAM, 0},
	 {T_INFO_ACK, 0, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_un),
	  T_INFINITE, 65535, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_UNIX, SOCK_STREAM, 0},
	 {T_INFO_ACK, 0, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_un),
	  T_INFINITE, 65535, T_COTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_UNIX, SOCK_DGRAM, 0},
	 {T_INFO_ACK, 65507, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_un),
	  T_INFINITE, 65507, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
#if defined HAVE_OPENSS7_SCTP
	,
	{{PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP},
	 {T_INFO_ACK, T_INFINITE, T_INFINITE, 536, T_INVALID,
	  8 * sizeof(struct sockaddr_in),
	  T_INFINITE, 65535, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
#endif				/* defined HAVE_OPENSS7_SCTP */
};
STATIC int ss_majors[SS__CMAJORS] = { SS__CMAJOR_0, };
STATIC streamscall int
ss_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int mindex = 0, err;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	ss_t *ss, **ipp = &ss_opens;
	const ss_profile_t *prof;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: can't push as module\n", DRV_NAME));
		return (EIO);
	}
#if defined LIS
	if (cmajor != SS__CMAJOR_0)
		return (ENXIO);
#endif
#if defined LFS
	/* Linux Fast-STREAMS always passes internal major device numbers (module ids) */
	if (cmajor != SS__DRV_ID)
		return (ENXIO);
#endif
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR)
		return (ENXIO);
	prof = &ss_profiles[cminor - FIRST_CMINOR];
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = FREE_CMINOR;
	spin_lock_bh(&ss_lock);
	for (; *ipp; ipp = &(*ipp)->next) {
		if (cmajor != (*ipp)->cmajor)
			break;
		if (cmajor == (*ipp)->cmajor) {
			if (cminor < (*ipp)->cminor)
				break;
			if (cminor == (*ipp)->cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= SS__CMAJORS
					    || !(cmajor = ss_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= SS__CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_bh(&ss_lock);
		return (ENXIO);
	}
#if defined LFS
	{
		unsigned long flags = freezestr(q);
		/* Pre-allocate queue band structures on the read side. */
		if ((err = strqset(q, QHIWAT, 1, STRHIGH))) {
			ptrace(("%s: ERROR: could not allocate queue band 1 structure, err = %d\n",
				DRV_NAME, err));
			unfreezestr(q, flags);
			spin_unlock_bh(&ss_lock);
			return (err);
		}
		if ((err = strqset(q, QHIWAT, 2, STRHIGH))) {
			ptrace(("%s: ERROR: could not allocate queue band 2 structure, err = %d\n",
				DRV_NAME, err));
			unfreezestr(q, flags);
			spin_unlock_bh(&ss_lock);
			return (err);
		}
		unfreezestr(q, flags);
	}
#endif
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(ss = ss_alloc_priv(q, ipp, cmajor, cminor, crp, prof))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		spin_unlock_bh(&ss_lock);
		return (ENOMEM);
	}
	/* Create all but raw sockets at open time.  For raw sockets, we do not know the protocol
	   to create until the socket is bound to a protocol.  For all others, the protocol is
	   known and the socket is created so that we can accept options management on unbound
	   sockets. */
	spin_unlock_bh(&ss_lock);
	if ((err = ss_sock_init(ss)) < 0) {
		ptrace(("%s: ERROR: from ss_sock_init %d\n", DRV_NAME, -err));
		ss_free_priv(q);
		return (-err);
	}
	qprocson(q);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
ss_close(queue_t *q, int flag, cred_t *crp)
{
	ss_t *ss = PRIV(q);

	(void) flag;
	(void) crp;
	(void) ss;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, ss->cmajor, ss->cminor));
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	ss_free_priv(q);
	goto quit;
      quit:
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
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw ss_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ss_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

STATIC struct devnode inet_node_ip = {
	.n_name = "ip",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_icmp = {
	.n_name = "icmp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_ggp = {
	.n_name = "ggp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_ipip = {
	.n_name = "ipip",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_tcp = {
	.n_name = "tcp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_egp = {
	.n_name = "egp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_pup = {
	.n_name = "pup",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_udp = {
	.n_name = "udp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_idp = {
	.n_name = "idp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_rawip = {
	.n_name = "rawip",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_ticots_ord = {
	.n_name = "ticots_ord",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_ticots = {
	.n_name = "ticots",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode inet_node_ticlts = {
	.n_name = "ticlts",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

#if defined HAVE_OPENSS7_SCTP
STATIC struct devnode inet_node_sctp = {
	.n_name = "sctp",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
#endif

STATIC int
ss_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&ss_cdev, major)) < 0)
		return (err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ip, IP_CMINOR)) < 0)
		pswerr(("Could not register IP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_icmp, ICMP_CMINOR)) < 0)
		pswerr(("Could not register ICMP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_ggp, GGP_CMINOR)) < 0)
		pswerr(("Could not register GGP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_ipip, IPIP_CMINOR)) < 0)
		pswerr(("Could not register IPIP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_tcp, TCP_CMINOR)) < 0)
		pswerr(("Could not register TCP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_egp, EGP_CMINOR)) < 0)
		pswerr(("Could not register EGP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_pup, PUP_CMINOR)) < 0)
		pswerr(("Could not register PUP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_udp, UDP_CMINOR)) < 0)
		pswerr(("Could not register UDP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_idp, IDP_CMINOR)) < 0)
		pswerr(("Could not register IDP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_rawip, RAWIP_CMINOR)) < 0)
		pswerr(("Could not register RAWIP_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_ticots_ord, TICOTS_ORD_CMINOR)) < 0)
		pswerr(("Could not register TICOTS_ORD_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_ticots, TICOTS_CMINOR)) < 0)
		pswerr(("Could not register TICOTS_CMINOR, err = %d\n", err));
	if ((err = register_strnod(&ss_cdev, &inet_node_ticlts, TICLTS_CMINOR)) < 0)
		pswerr(("Could not register TICLTS_CMINOR, err = %d\n", err));
#if defined HAVE_OPENSS7_SCTP
	if ((err = register_strnod(&ss_cdev, &inet_node_sctp, SCTP_CMINOR)) < 0)
		pswerr(("Could not register SCTP_CMINOR, err = %d\n", err));
#endif
	return (0);
}

STATIC int
ss_unregister_strdev(major_t major)
{
	int err;

#if defined HAVE_OPENSS7_SCTP
	if ((err = unregister_strnod(&ss_cdev, SCTP_CMINOR)) < 0)
		pswerr(("Could not unregister SCTP_CMINOR, err = %d\n", err));
#endif
	if ((err = unregister_strnod(&ss_cdev, TICLTS_CMINOR)) < 0)
		pswerr(("Could not unregister TICLTS_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, TICOTS_CMINOR)) < 0)
		pswerr(("Could not unregister TICOTS_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, TICOTS_ORD_CMINOR)) < 0)
		pswerr(("Could not unregister TICOTS_ORD_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, RAWIP_CMINOR)) < 0)
		pswerr(("Could not unregister RAWIP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, IDP_CMINOR)) < 0)
		pswerr(("Could not unregister IDP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, UDP_CMINOR)) < 0)
		pswerr(("Could not unregister UDP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, PUP_CMINOR)) < 0)
		pswerr(("Could not unregister PUP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, EGP_CMINOR)) < 0)
		pswerr(("Could not unregister EGP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, TCP_CMINOR)) < 0)
		pswerr(("Could not unregister TCP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, IPIP_CMINOR)) < 0)
		pswerr(("Could not unregister IPIP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, GGP_CMINOR)) < 0)
		pswerr(("Could not unregister GGP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, ICMP_CMINOR)) < 0)
		pswerr(("Could not unregister ICMP_CMINOR, err = %d\n", err));
	if ((err = unregister_strnod(&ss_cdev, IP_CMINOR)) < 0)
		pswerr(("Could not unregister IP_CMINOR, err = %d\n", err));
	if ((err = unregister_strdev(&ss_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
ss_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &ss_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
ss_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
ss_terminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (ss_majors[mindex]) {
			if ((err = ss_unregister_strdev(ss_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					ss_majors[mindex]);
			if (mindex)
				ss_majors[mindex] = 0;
		}
	}
	if ((err = ss_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
ss_init(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = ss_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ss_terminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ss_register_strdev(ss_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					ss_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				ss_terminate();
				return (err);
			}
		}
		if (ss_majors[mindex] == 0)
			ss_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(ss_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = ss_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ss_init);
module_exit(ss_terminate);

#endif				/* LINUX */
