/*****************************************************************************

 @(#) $RCSfile: inet.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2011-03-26 04:28:46 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2011-03-26 04:28:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: inet.c,v $
 Revision 1.1.2.6  2011-03-26 04:28:46  brian
 - updates to build process

 Revision 1.1.2.5  2011-02-07 04:54:41  brian
 - code updates for new distro support

 Revision 1.1.2.4  2010-11-28 14:32:24  brian
 - updates to support debian squeeze 2.6.32 kernel

 Revision 1.1.2.3  2009-07-23 16:37:52  brian
 - updates for release

 Revision 1.1.2.2  2009-06-29 07:35:43  brian
 - SVR 4.2 => SVR 4.2 MP

 Revision 1.1.2.1  2009-06-21 11:20:46  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: inet.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2011-03-26 04:28:46 $";

/*
   This driver provides the functionality of IP (Internet Protocol) over a connectionless network
   provider (NPI).  It provides a STREAMS-based encapsulation of the Linux IP stack. 
 */

#define _DEBUG 1
//#undef _DEBUG

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

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
#include <net/ip.h>
#include <net/udp.h>
#include <net/tcp.h>
#if defined HAVE_OPENSS7_SCTP
#undef STATIC
#undef INLINE
#include <net/sctp.h>
#endif

#include <linux/errqueue.h>

#if 0
/* Turn on some tracing and debugging. */
#undef ensure
#undef assure

#define ensure __ensure
#define assure __assure
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
#define sk_error_queue		error_queue
#define sk_rmem_alloc		rmem_alloc
#define sk_err			err
#define sk_shutdown		shutdown

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

#if !defined HAVE_OPENSS7_SCTP
#undef sctp_addr
#endif

#ifndef tcp_openreq_cachep
#ifdef HAVE_TCP_OPENREQ_CACHEP_USABLE
#include <linux/slab.h>
extern kmem_cachep_t *const _tcp_openreq_cachep_location;
#endif
#endif

#ifndef tcp_set_keepalive
#ifdef HAVE_TCP_SET_KEEPALIVE_USABLE
void tcp_set_keepalive(struct sock *sk, int val);
#endif
#endif

#ifndef tcp_sync_mss
#ifdef HAVE_TCP_SYNC_MSS_USABLE
unsigned int tcp_sync_mss(struct sock *sk, u32 pmtu);
#endif
#endif

#ifndef tcp_write_xmit
#ifdef HAVE_TCP_WRITE_XMIT_USABLE
int tcp_write_xmit(struct sock *sk, int nonagle);
#endif
#endif

#ifndef tcp_cwnd_application_limited
#ifdef HAVE_TCP_CWND_APPLICATION_LIMITED_USABLE
void tcp_cwnd_application_limited(struct sock *sk);
#endif
#endif

/*
   recreate this structure because it is used by an inline 
 */
typedef
typeof(ip_tos2prio)
    ip_tos2prio_t;
	ip_tos2prio_t ip_tos2prio = { 0, 1, 0, 0, 2, 2, 2, 2, 6, 6, 6, 6, 4, 4, 4, 4 };

#ifndef SK_WMEM_MAX
#define SK_WMEM_MAX 65535
#endif
#ifndef SK_RMEM_MAX
#define SK_RMEM_MAX 65535
#endif

#ifndef sysctl_rmem_default
#ifdef HAVE_SYSCTL_RMEM_DEFAULT_SUPPORT
extern __u32 sysctl_rmem_default;
#else
#define sysctl_rmem_default SK_RMEM_MAX
#endif
#endif

#ifndef sysctl_wmem_default
#ifdef HAVE_SYSCTL_WMEM_DEFAULT_SUPPORT
extern __u32 sysctl_wmem_default;
#else
#define sysctl_wmem_default SK_WMEM_MAX
#endif
#endif

#ifndef sysctl_rmem_max
#ifdef HAVE_SYSCTL_RMEM_MAX_SUPPORT
extern __u32 sysctl_rmem_max;
#else
#define sysctl_rmem_max SK_RMEM_MAX
#endif
#endif

#ifndef sysctl_wmem_max
#ifdef HAVE_SYSCTL_WMEM_MAX_SUPPORT
extern __u32 sysctl_wmem_max;
#else
#define sysctl_wmem_max SK_WMEM_MAX
#endif
#endif

#ifndef sysctl_tcp_fin_timeout
#ifndef HAVE_SYSCTL_TCP_FIN_TIMEOUT_USABLE
#define sysctl_tcp_fin_timeout TCP_FIN_TIMEOUT
#endif
#endif

/* Used by tcp_push_pending_frames inline in some kernels. */
#ifndef tcp_current_mss
#ifdef HAVE_TCP_CURRENT_MSS_USABLE
#ifdef HAVE_KFUNC_TCP_CURRENT_MSS_1_ARG
unsigned int tcp_current_mss(struct sock *sk);
#else
unsigned int tcp_current_mss(struct sock *sk, int large);
#endif
#endif
#endif

#ifndef sysctl_ip_dynaddr
#ifndef HAVE_SYSCTL_IP_DYNADDR_USABLE
#define sysctl_ip_dynaddr 0
#else
extern int sysctl_ip_dynaddr;
#endif
#endif

#ifndef sysctl_ip_nonlocal_bind
#ifndef HAVE_SYSCTL_IP_NONLOCAL_BIND_USABLE
#define sysctl_ip_nonlocal_bind 0
#else
extern int sysctl_ip_nonlocal_bind;
#endif
#endif

#ifndef IPDEFTTL
#define IPDEFTTL 64
#endif

#ifndef sysctl_ip_default_ttl
#ifndef HAVE_SYSCTL_IP_DEFAULT_TTL_USABLE
#define sysctl_ip_default_ttl IPDEFTTL
#else
extern int sysctl_ip_default_ttl;
#endif
#endif

#ifndef tcp_set_skb_tso_segs
#ifdef HAVE_TCP_SET_SKB_TSO_SEGS_USABLE
#ifdef HAVE_KFUNC_TCP_SET_SKB_TSO_SEGS_SOCK
void tcp_set_skb_tso_segs(struct sock *sk, struct sk_buff *skb);
#else
void tcp_set_skb_tso_segs(struct sk_buff *skb, unsigned int mss_std);
#endif
#endif
#endif

/* older 2.6.8 name for the same function */
#ifndef tcp_set_skb_tso_factor
#ifdef HAVE_TCP_SET_SKB_TSO_FACTOR_USABLE
void tcp_set_skb_tso_factor(struct sk_buff *skb, unsigned int mss_std);
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
#define SS__COPYRIGHT	"Copyright (c) 2008-2010  Monavacon Limited.  All Rights Reserved."
#define SS__REVISION	"OpenSS7 $RCSfile: inet.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2011-03-26 04:28:46 $"
#define SS__DEVICE	"SVR 4.2 MP STREAMS INET Drivers (NET4)"
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
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif
#endif				/* LINUX */

#define SS__DRV_ID	CONFIG_STREAMS_SS__MAJOR
#define SS__DRV_NAME	CONFIG_STREAMS_SS__NAME
#define SS__CMAJORS	CONFIG_STREAMS_SS__NMAJORS
#define SS__CMAJOR_0	CONFIG_STREAMS_SS__MAJOR
#define SS__UNITS	CONFIG_STREAMS_SS__NMINORS

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
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SS__MAJOR));
MODULE_ALIAS("streams-driver-inet");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_SS__MAJOR));
MODULE_ALIAS("/dev/streams/inet");
MODULE_ALIAS("/dev/streams/inet/*");
MODULE_ALIAS("/dev/streams/clone/inet");
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

#ifdef _OPTIMIZE_SPEED
#define INETLOG(ss, level, flags, fmt, ...) \
	do { } while (0)
#else
#define INETLOG(ss, level, flags, fmt, ...) \
	mi_strlog(ss->rq, level, flags, fmt, ##__VA_ARGS__)
#endif

#define STRLOGERR	0	/* log INET error information */
#define STRLOGNO	0	/* log INET notice information */
#define STRLOGST	1	/* log INET state transitions */
#define STRLOGTO	2	/* log INET timeouts */
#define STRLOGRX	3	/* log INET primitives received */
#define STRLOGTX	4	/* log INET primitives issued */
#define STRLOGTE	5	/* log INET timer events */
#define STRLOGIO	6	/* log INET additional data */
#define STRLOGDA	7	/* log INET data */

#if 1
#define LOGERR(ss, fmt, ...) INETLOG(ss, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(ss, fmt, ...) INETLOG(ss, STRLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGST(ss, fmt, ...) INETLOG(ss, STRLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTO(ss, fmt, ...) INETLOG(ss, STRLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGRX(ss, fmt, ...) INETLOG(ss, STRLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTX(ss, fmt, ...) INETLOG(ss, STRLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTE(ss, fmt, ...) INETLOG(ss, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(ss, fmt, ...) INETLOG(ss, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(ss, fmt, ...) INETLOG(ss, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOGERR(ss, fmt, ...) INETLOG(ss, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(ss, fmt, ...) INETLOG(ss, STRLOGNO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGST(ss, fmt, ...) INETLOG(ss, STRLOGST, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTO(ss, fmt, ...) INETLOG(ss, STRLOGTO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGRX(ss, fmt, ...) INETLOG(ss, STRLOGRX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTX(ss, fmt, ...) INETLOG(ss, STRLOGTX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTE(ss, fmt, ...) INETLOG(ss, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(ss, fmt, ...) INETLOG(ss, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(ss, fmt, ...) INETLOG(ss, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#endif

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

static struct module_info ss_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT << 5,	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

static struct module_stat ss_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ss_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int ss_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int ss_qclose(queue_t *, int, cred_t *);

static streamscall int ss_rput(queue_t *, mblk_t *);
static streamscall int ss_rsrv(queue_t *);

static struct qinit ss_rinit = {
	.qi_putp = ss_rput,	/* Read put (msg from below) */
	.qi_srvp = ss_rsrv,	/* Read queue service */
	.qi_qopen = ss_qopen,	/* Each open */
	.qi_qclose = ss_qclose,	/* Last close */
	.qi_minfo = &ss_rinfo,	/* Information */
	.qi_mstat = &ss_rstat,	/* Statistics */
};

static struct module_info ss_winfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = (SHEADHIWAT >> 1),	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

static streamscall int ss_wput(queue_t *, mblk_t *);
static streamscall int ss_wsrv(queue_t *);

static struct qinit ss_winit = {
	.qi_putp = ss_wput,	/* Write put (msg from above) */
	.qi_srvp = ss_wsrv,	/* Write queue service */
	.qi_minfo = &ss_winfo,	/* Information */
	.qi_mstat = &ss_wstat,	/* Statistics */
};

MODULE_STATIC struct streamtab ss_info = {
	.st_rdinit = &ss_rinit,	/* Upper read queue */
	.st_wrinit = &ss_winit,	/* Lower read queue */
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
	unsigned long flags[3];		/* at least 96 flags */
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

typedef struct inet {
	struct inet *ss_parent;		/* pointer to parent or self */
	volatile long ss_rflags;	/* rd queue flags */
	volatile long ss_wflags;	/* wr queue flags */
	queue_t *rq;			/* associated read queue */
	queue_t *wq;			/* associated write queue */
	t_scalar_t oldstate;		/* tpi state history */
	int tcp_state;			/* tcp state history */
	ss_profile_t p;			/* protocol profile */
	cred_t cred;			/* credientials */
	struct {
		void (*sk_state_change) (struct sock *);
		void (*sk_data_ready) (struct sock *, int);
		void (*sk_write_space) (struct sock *);
		void (*sk_error_report) (struct sock *);
	} cb_save;			/* call back holder */
	ushort port;			/* port/protocol number */
	struct sockaddr src;		/* bound address */
	struct sockaddr dst;		/* connected address */
	ss_options_t options;		/* protocol options */
	unsigned char _pad[40];		/* pad for options */
	bufq_t conq;			/* connection queue */
	uint conind;			/* number of connection indications */
	t_uscalar_t seq;		/* previous sequence number */
	int lasterror;			/* last socket error */
	struct socket *sock;		/* socket pointer */
} ss_t;

typedef struct ss_conind {
	/* first portion must be identical to struct inet */
	struct inet *ci_parent;		/* pointer to parent */
	volatile long ci_rflags;	/* rd queue flags */
	volatile long ci_wflags;	/* wr queue flags */
	struct sock *ci_sk;		/* underlying child socket */
	t_uscalar_t ci_seq;		/* sequence number */
} ss_conind_t;

#define PRIV(__q) ((ss_t *)((__q)->q_ptr))
#define SOCK_PRIV(__sk) ((__sk)->sk_user_data)

#define SS_BIT_STATE_CHANGE		0
#define SS_BIT_ERROR_REPORT		1
#define SS_BIT_DATA_READY		2
#define SS_BIT_WRITE_SPACE		3

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
#endif				/* defined HAVE_OPENSS7_SCTP */
};

static rwlock_t ss_lock = RW_LOCK_UNLOCKED;	/* protects ss_opens lists */
static caddr_t ss_opens = NULL;

#if 0

/*
   for later when we support default destinations and default listeners 
 */
static ss_t *ss_dflt_dest = NULL;
static ss_t *ss_dflt_lstn = NULL;
#endif

/*
 *  =========================================================================
 *
 *  Private Structure cache
 *
 *  =========================================================================
 */
static kmem_cachep_t ss_priv_cachep = NULL;

/**
 * ss_init_caches: - initialize caches
 *
 * Returns zero (0) on success or a negative error code on failure.
 */
static int
ss_init_caches(void)
{
	if (!ss_priv_cachep &&
	    !(ss_priv_cachep =
	      kmem_create_cache("ss_priv_cachep", mi_open_size(sizeof(ss_t)), 0, SLAB_HWCACHE_ALIGN,
				NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: Cannot allocate ss_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", DRV_NAME);
	return (0);
}

/**
 * ss_term_caches: - terminate caches
 *
 * Returns zero (0) on success or a negative error code on failure.
 */
static int
ss_term_caches(void)
{
	if (ss_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(ss_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ss_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed ss_priv_cachep", DRV_NAME);
#else
		kmem_cache_destroy(ss_priv_cachep);
#endif
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  Socket Callbacks
 *
 *  =========================================================================
 */

/**
 * ss_trylock: - try to lock a Stream queue pair
 * @q: the queue pair to lock
 *
 * Because we lock connecting, listening and accepting Streams, and listening streams can be waiting
 * on accepting streams, we must ensure that the other stream cannot close and detach from its queue
 * pair at the same time that we are locking or unlocking.  Therefore, hold the master list
 * reader-writer lock while locking or unlocking.
 */
static inline fastcall ss_t *
ss_trylock(queue_t *q)
{
	ss_t *ss;

	read_lock(&ss_lock);
	ss = (ss_t *) mi_trylock(q);
	read_unlock(&ss_lock);
	return (ss);
}

/**
 * ss_unlock: - unlock a Stream queue pair
 * @q: the queue pair to unlock
 *
 * Because we lock connecting, listening and accepting Streams, and listening streams can be waiting
 * on accepting streams, we must ensure that the other stream cannot close and detach from its queue
 * pair at the same time that we are locking or unlocking.  Therefore, hold the master list
 * reader-writer lock while locking or unlocking.
 */
static inline fastcall void
ss_unlock(ss_t *ss)
{
	read_lock(&ss_lock);
	mi_unlock((caddr_t) ss);
	read_unlock(&ss_lock);
}

/*
 *  =========================================================================
 *
 *  Socket Callbacks
 *
 *  =========================================================================
 */
/*
 * Forward declarations of our callback functions.
 */
static void ss_state_change(struct sock *sk);
static void ss_write_space(struct sock *sk);
static void ss_error_report(struct sock *sk);
static void ss_data_ready(struct sock *sk, int len);

/**
 * ss_socket_put: - restore socket from strinet use.
 * @sockp: pointer to socket pointer of socket to release
 */
static void
ss_socket_put(struct socket **sockp)
{
	unsigned long flags;
	struct socket *sock;
	struct sock *sk;

	if (likely((sock = xchg(sockp, NULL)) != NULL)) {
		if (likely((sk = sock->sk) != NULL)) {
			/* We don't really need to lock out interrupts here, just bottom halves
			   'cause a read lock is taken in the callback function itself. */
			lock_sock(sk);
			write_lock_irqsave(&sk->sk_callback_lock, flags);
			{
				ss_t *ss;

				if ((ss = SOCK_PRIV(sk))) {
					SOCK_PRIV(sk) = NULL;
					sk->sk_state_change = ss->cb_save.sk_state_change;
					sk->sk_data_ready = ss->cb_save.sk_data_ready;
					sk->sk_write_space = ss->cb_save.sk_write_space;
					sk->sk_error_report = ss->cb_save.sk_error_report;
					// mi_close_put((caddr_t) ss);
				} else
					assure(ss);
				/* The following will cause the socket to be aborted, particularly
				   for Linux TCP or other orderly release sockets. XXX: Perhaps
				   should check the state of the socket and call sock_disconnect()
				   first as well. SCTP will probably behave better that way. */
				sock_set_keepopen(sk);
				sk->sk_lingertime = 0;
			}
			write_unlock_irqrestore(&sk->sk_callback_lock, flags);
			release_sock(sk);
		} else
			assure(sk);
		sock_release(sock);
	}
}
static void
ss_socket_get(struct socket *sock, ss_t *ss)
{
	unsigned long flags;
	struct sock *sk;

	ensure(sock, return);
	if ((sk = sock->sk)) {
		/* We don't really need to lock out interrupts here, just bottom halves 'cause a
		   read lock is taken in the callback function itself. */
		lock_sock(sk);
		write_lock_irqsave(&sk->sk_callback_lock, flags);
		{
			SOCK_PRIV(sk) = ss;
			// mi_open_grab((caddr_t) ss);
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
		write_unlock_irqrestore(&sk->sk_callback_lock, flags);
		release_sock(sk);
	} else
		assure(sk);
}

/**
 * ss_socket_grab: - hold a socket for use by strinet
 * @sock: the accepted socket
 * @ss: the accepting Stream
 */
static void
ss_socket_grab(struct socket *sock, ss_t *ss)
{
	unsigned long flags;
	struct sock *sk;

	ensure(sock, return);
	if ((sk = sock->sk)) {
		/* We don't really need to lock out interrupts here, just bottom halves 'cause a
		   read lock is taken in the callback function itself. */
		lock_sock(sk);
		write_lock_irqsave(&sk->sk_callback_lock, flags);
		{
			SOCK_PRIV(sk) = ss;
			// mi_open_grab((caddr_t) ss);
			/* We only want to overwrite the ones that were not inherited from the
			   parent listening socket. */
			if (sk->sk_state_change != ss_state_change) {
				ss->cb_save.sk_state_change = sk->sk_state_change;
				sk->sk_state_change = ss_state_change;
			}
			if (sk->sk_data_ready != ss_data_ready) {
				ss->cb_save.sk_data_ready = sk->sk_data_ready;
				sk->sk_data_ready = ss_data_ready;
			}
			if (sk->sk_write_space != ss_write_space) {
				ss->cb_save.sk_write_space = sk->sk_write_space;
				sk->sk_write_space = ss_write_space;
			}
			if (sk->sk_error_report != ss_error_report) {
				ss->cb_save.sk_error_report = sk->sk_error_report;
				sk->sk_error_report = ss_error_report;
			}
#ifdef LINUX_2_4
			inet_sk(sk)->recverr = 1;
#else
			sk->ip_recverr = 1;
#endif
			ss->tcp_state = sk->sk_state;	/* initialized tcp state */
		}
		write_unlock_irqrestore(&sk->sk_callback_lock, flags);
		release_sock(sk);
	} else
		assure(sk);
}

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

static ss_options_t ss_defaults = {
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

/**
 * ss_size_conn_opts: - size connection indication or confirmation options
 * @ss: private structure (locked)
 *
 * Return an option buffer size for a connection indication or confirmation.  Options without
 * end-to-end significance are not indicated and are only confirmed if requested.  Options with
 * end-to-end significance are always both indicated and confirmed.  For this to work for connection
 * indications, all request options flags must be cleared to zero.
 */
static int
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
			size += _T_SPACE_SIZEOF(ss->options.ip.options);
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
				size += _T_SPACE_SIZEOF(ss->options.sctp.ostreams);
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

/**
 * t_build_conn_opts: - build connection confirmation options
 * @ss: private structure (locked)
 * @op: options pointer
 * @olen: options length
 *
 * These are options with end-to-end significance plus any options without end-to-end significance
 * that were requested for negotiation in the connection request.  For a connection indication, this
 * is only options with end-to-end significance.  For this to work with connection indications, all
 * options request flags must be set to zero.  The retrn value is the resulting size of the options
 * buffer, or a negative error number on software fault.
 *
 * The t_connect() or t_rcvconnect() functions return the values of all options with end-to-end
 * significance that were received with the connection response and the negotiated values of those
 * options without end-to-end significance that had been specified on input.  However, options
 * specified on input with t_connect() call that are not supported or refer to an unknown option
 * level are discarded and not returned on output.
 *
 * The status field of each option returned with t_connect() or t_rcvconnect() indicates if the
 * proposed value (T_SUCCESS) or a degraded value (T_PARTSUCCESS) has been negotiated.  The status
 * field of received ancillary information (for example, T_IP options) that is not subject to
 * negotiation is always set to T_SUCCESS.
 */
static int
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

		{
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(ss->options.ip.options);
			oh->level = T_INET_IP;
			oh->name = T_IP_OPTIONS;
			oh->status = T_SUCCESS;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
      efault:
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

#ifdef __LP64__
#undef MAX_SCHEDULE_TIMEOUT
#define MAX_SCHEDULE_TIMEOUT INT_MAX
#endif

/**
 * t_set_options: - set options on new socket
 * @ss: private structure (locked)
 *
 * This is used after accepting a new socket.  It is used to negotiate the options applied to the
 * responding stream in the connection response to the newly accepted socket.  All options of
 * interest have their flags set and the appropriate option values set.
 */
static int
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
#ifdef HAVE_TCP_SET_KEEPALIVE_USABLE
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
}

/**
 * t_parse_conn_opts: - parse connection request or response options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @request: non-zero, request; zero, response
 *
 * Only legal options can be negotiated; illegal options cause failure.  An option is illegal if the
 * following applies: 1) the length specified in t_opthdr.len exceeds the remaining size of the
 * option buffer (counted from the beginning of the option); 2) the option value is illegal: the
 * legal values are defined for each option.  If an illegal option is passed to XTI, the following
 * will happen: ... if an illegal option is passed to t_accept() or t_connect() then either the
 * function failes with t_errno set to [TBADOPT] or the connection establishment fails at a later
 * stage, depending on when the implementation detects the illegal option. ...
 *
 * If the tansport user passes multiple optiohs in one call and one of them is illegal, the call
 * fails as described above.  It is, however, possible that some or even all of the smbmitted legal
 * options were successfully negotiated.  The transport user can check the current status by a call
 * to t_optmgmt() with the T_CURRENT flag set.
 *
 * Specifying an option level unknown to the transport provider does not fail in calls to
 * t_connect(), t_accept() or t_sndudata(); the option is discarded in these cases.  The function
 * t_optmgmt() fails with [TBADOPT].
 *
 * Specifying an option name that is unknown to or not supported by the protocol selected by the
 * option level does not cause failure.  The option is discarded in calles to t_connect(),
 * t_accept() or t_sndudata().  The function t_optmgmt() returns T_NOTSUPPORT in the status field of
 * the option.
 *
 * If a transport user requests negotiation of a read-only option, or a non-privileged user requests
 * illegal access to a privileged option, the following outcomes are possible: ... 2) if negotiation
 * of a read-only option is required, t_accept() or t_connect() either fail with [TACCES], or the
 * connection establishment aborts and a T_DISCONNECT event occurs.  If the connection aborts, a
 * synchronous call to t_connect() failes with [TLOOK].  It depdends on timing an implementation
 * conditions whether a t_accept() call still succeeds or failes with [TLOOK].  If a privileged
 * option is illegally requested, the option is quietly ignored.  (A non-privileged user shall not
 * be able to select an option which is privileged or unsupported.)
 *
 * If multiple options are submitted to t_connect(), t_accept() or t_sndudata() and a read-only
 * option is rejected, the connection or the datagram transmission fails as described.  Options that
 * could be successfully negotiated before the erroneous option was processed retain their
 * negotiated values.  There is no roll-back mechanmism.
 */
static int
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
#ifdef HAVE_TCP_SET_KEEPALIVE_USABLE
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
					continue;
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
}

/**
 * ss_cmsg_size: - size send cmsg
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Calculate how large a cmsg buffer needs to be to contain all of the options provided to
 * T_OPTDATA_REQ or T_UNITDATA_REQ.
 */
static fastcall __hot_out int
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
      einval:
	return (-EINVAL);
}

/**
 * ss_cmsg_build: - build send cmsg
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @msg: message header
 *
 * Build a cmsg buffer from options provided to T_OPTDATA_REQ or T_UNITDATA_REQ.
 */
static fastcall __hot_out int
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
}

#define ss_opts_maxsize \
( \
	_T_SPACE_SIZEOF(uint32_t) + \
	_T_SPACE_SIZEOF(unsigned char) + \
	_T_SPACE_SIZEOF(unsigned char) + \
	T_SPACE(40) + \
	_T_SPACE_SIZEOF(t_uscalar_t) + \
	_T_SPACE_SIZEOF(t_uscalar_t) \
)

/**
 * ss_opts_size: size cmsg options
 * @ss: private structure (locked)
 * @msg: the message header for which to size options
 *
 * Calculate the size of the options buffer necessary to hold elements in a cmsg.  Only options of
 * end to end significance are important.
 */
static fastcall __hot_in int
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
			   cannot. T_UDP_CHECKSUM cannot be properly indicated.  Sorry. */
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

/**
 * ss_opts_build: - build options from cmsg
 * @ss: private structure (locked)
 * @msg: message header from which to build options
 * @op: output options pointer
 * @olen: output options length
 * @etype: pointer to store error type (or NULL)
 *
 * Data indications only indicate options with end-to-end significance.  This function fills as many
 * options into olen as possible.  This function was reworked from previous versions to fit as many
 * options as possible into the available option space.
 */
static fastcall __hot_in int
ss_opts_build(const ss_t *ss, struct msghdr *msg, unsigned char *op, size_t olen, t_scalar_t *etype)
{
	struct cmsghdr *cmsg;
	struct t_opthdr *oh;
	unsigned char *nh, *eh;

	if (op == NULL || olen == 0)
		return (0);
	for (cmsg = CMSG_FIRSTHDR(msg), oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0), eh = op;
	     cmsg && oh; cmsg = CMSG_NXTHDR(msg, cmsg)) {
		switch (cmsg->cmsg_level) {
		case SOL_SOCKET:
			continue;
		case SOL_IP:
			if (ss->p.prot.protocol == T_INET_UDP || ss->p.prot.protocol == T_INET_IP) {
				switch (cmsg->cmsg_type) {
				case IP_RECVERR:
				{
					if (etype) {
						struct sock_extended_err *ee;

						/* dig out error number for uderror */
						ee = (typeof(ee)) CMSG_DATA(msg);
						*etype = (t_scalar_t) ee->ee_errno;
					}
					continue;
				}
				case IP_RECVOPTS:
				{
					size_t len;

					len = cmsg->cmsg_len < 40 ? cmsg->cmsg_len : 40;

					LOGIO(ss, "processing option IP_RECVOPTS");
					LOGIO(ss, "building option T_IP_OPTIONS");
					oh->len = T_LENGTH(len);
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = T_SUCCESS;
					if ((nh = _T_ALIGN_OFS_OFS(oh, oh->len, 0)) > op + olen)
						break;
					bcopy(CMSG_DATA(cmsg), T_OPT_DATA(oh), len);
					eh = nh;	/* remember ending position */
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				}
				case IP_TOS:
					LOGIO(ss, "processing option IP_TOS");
					LOGIO(ss, "building option T_IP_TOS");
					oh->len = _T_LENGTH_SIZEOF(unsigned char);

					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					if ((nh = _T_ALIGN_OFS_OFS(oh, oh->len, 0)) > op + olen)
						break;
					*((unsigned char *) T_OPT_DATA(oh)) =
					    *((unsigned char *) CMSG_DATA(cmsg));
					eh = nh;	/* remember ending position */
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				case IP_TTL:
					LOGIO(ss, "processing option IP_TTL");
					LOGIO(ss, "building option T_IP_TTL");
					oh->len = _T_LENGTH_SIZEOF(unsigned char);

					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					if ((nh = _T_ALIGN_OFS_OFS(oh, oh->len, 0)) > op + olen)
						break;
					*((unsigned char *) T_OPT_DATA(oh)) =
					    *((unsigned char *) CMSG_DATA(cmsg));
					eh = nh;	/* remember ending position */
					oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
					continue;
				case IP_PKTINFO:
					LOGIO(ss, "processing option IP_PKTINFO");
					LOGIO(ss, "building option T_IP_ADDR");
					oh->len = _T_LENGTH_SIZEOF(uint32_t);

					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					if ((nh = _T_ALIGN_OFS_OFS(oh, oh->len, 0)) > op + olen)
						break;
					*((uint32_t *) T_OPT_DATA(oh)) =
					    ((struct in_pktinfo *) CMSG_DATA(cmsg))->ipi_addr.
					    s_addr;
					eh = nh;	/* remember ending position */
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
			   cannot. T_UDP_CHECKSUM cannot be properly indicated.  Sorry. */
			continue;
#if defined HAVE_OPENSS7_SCTP
		case SOL_SCTP:
			switch (cmsg->cmsg_type) {
			case SCTP_PPI:
				LOGIO(ss, "processing option SCTP_PPI");
				LOGIO(ss, "building option T_SCTP_PPI");
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				if ((nh = _T_ALIGN_OFS_OFS(oh, oh->len, 0)) > op + olen)
					break;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    *((unsigned int *) CMSG_DATA(cmsg));
				eh = nh;	/* remember ending position */
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
				continue;
			case SCTP_DISPOSITION:
				LOGIO(ss, "processing option SCTP_DISPOSITION");
				LOGIO(ss, "building option T_SCTP_DISPOSITION");
				oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->status = T_SUCCESS;
				if ((nh = _T_ALIGN_OFS_OFS(oh, oh->len, 0)) > op + olen)
					break;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    *((unsigned int *) CMSG_DATA(cmsg));
				eh = nh;	/* remember ending position */
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
				continue;
			}
			continue;
#endif				/* defined HAVE_OPENSS7_SCTP */
		default:
			continue;
		}
		break;
	}
	assure(oh == NULL);
	return ((unsigned char *) eh - op);
}

/**
 * t_size_default_options: - size options
 * @t: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_DEFAULT options management
 * request, or an error (negative).
 */
static int
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
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

/**
 * t_size_current_options: - size options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_CURRENT options management
 * request, or an error (negative).
 */
static int
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
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

/**
 * t_size_check_options: - size options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_CHECK options management
 * request, or an error (negative).
 */
static int
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
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

/**
 * t_size_negotiate_options: - size options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_NEGOTIATE options
 * management request, or an error (negative).
 */
static int
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
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

/**
 * t_overall_result: - overall option result
 * @overall: current overall options result
 * @result: specific result to include in overall result
 *
 * Calculates the overall T_OPTMGMT_ACK flag result from individual results.
 */
static uint
t_overall_result(t_scalar_t *overall, uint result)
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

/**
 * t_build_default_options: - build default options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 *
 * Perform the actions required of T_DEFAULT placing the output in the provided buffer.
 */
static t_scalar_t
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

/**
 * t_build_current_options: - build current options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 *
 * Perform the actions required of T_CURRENT placing the output in the provided buffer.
 */
static t_scalar_t
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
					/* read only */
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ssn);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_SSN;
					oh->status = t_overall_result(&overall, T_READONLY);
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ssn;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_SCTP_TSN:
					/* read only */
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.tsn);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_TSN;
					oh->status = t_overall_result(&overall, T_READONLY);
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
					/* read-only */
					oh->len = _T_LENGTH_SIZEOF(t->options.sctp.status);
					oh->level = T_INET_SCTP;
					oh->name = T_SCTP_STATUS;
					oh->status = t_overall_result(&overall, T_READONLY);
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
	LOGERR(t, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

/**
 * t_build_check_options: - build check options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 *
 * Perform the actions required of T_CHECK placing the output in the provided buffer.
 */
static t_scalar_t
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
	LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

#ifdef  HAVE___TCP_PUSH_PENDING_FRAMES_ADDR
#ifdef HAVE_OLD_SOCK_STRUCTURE
void __tcp_push_pending_frames(struct sock *sk, struct tcp_opt *tp, unsigned int cur_mss, int nonagle);
#else				/* defined HAVE_OLD_SOCK_STRUCTURE */
#ifndef  HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS
void __tcp_push_pending_frames(struct sock *sk, struct tcp_sock *tp, unsigned int cur_mss, int nonagle);
#else				/* !defined HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS */
void __tcp_push_pending_frames(struct sock *sk, unsigned int cur_mss, int nonagle);
#endif				/* !defined HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS */
#endif				/* defined HAVE_OLD_SOCK_STRUCTURE */
#endif				/* defined HAVE___TCP_PUSH_PENDING_FRAMES_ADDR */

/**
 * t_build_negotiate_options: - build negotiate options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 *
 * Perform the actions required of T_NEGOTIATE placing the output in the provided buffer.
 */
static t_scalar_t
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
#if !defined CONFIG_KERNEL_WEAK_MODULES || defined HAVE_TCP_PUSH_PENDING_FRAMES_EXPORT || \
    ( defined HAVE_TCP_CURRENT_MSS_EXPORT && defined HAVE_TCP___PUSH_PENDING_FRAMES_EXPORT)
#if !defined HAVE_KFUNC_TCP_PUSH_PENDING_FRAMES_1_ARG
						if ((tp->nonagle = (*valp == T_YES) ? 1 : 0))
							tcp_push_pending_frames(sk, tp);
#else				/* !defined HAVE_KFUNC_TCP_PUSH_PENDING_FRAMES_1_ARG */
						if ((tp->nonagle = (*valp == T_YES) ? 1 : 0))
							tcp_push_pending_frames(sk);
#endif				/* !defined HAVE_KFUNC_TCP_PUSH_PENDING_FRAMES_1_ARG */
#endif
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
#ifdef HAVE_TCP_SET_KEEPALIVE_USABLE
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
	LOGERR(t, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

/**
 * t_build_options: - build options
 * @ss: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 * @flags: options management flag
 *
 * Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK or T_NEGOTIARE, placing the output
 * in the provided buffer.
 */
static t_scalar_t
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

/**
 * ss_addr_size: - calculate addresss size
 * @add: socket address
 *
 * Calculates and returns the size of the socket address based on the address family contained in
 * the address.
 */
static inline fastcall __hot int
ss_addr_size(struct sockaddr *add)
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
#if !defined _OPTIMIZE_SPEED

/**
 * tpi_statename: - name TPI state
 * @state: state to name
 * Returns the name of the state or "(unknown)".
 */
static const char *
tpi_statename(t_scalar_t state)
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

/**
 * tpi_primname: - name TPI primitive
 * @prim: the primitive to name
 * Returns the name of the primitive or "(unknown)".
 */
static const char *
tpi_primname(t_scalar_t prim)
{
	switch (prim) {
	case T_CONN_REQ:
		return ("T_CONN_REQ");
	case T_CONN_RES:
		return ("T_CONN_RES");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ");
	case T_DATA_REQ:
		return ("T_DATA_REQ");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case T_INFO_REQ:
		return ("T_INFO_REQ");
	case T_BIND_REQ:
		return ("T_BIND_REQ");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ");
	case T_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND");
	case T_CONN_CON:
		return ("T_CONN_CON");
	case T_DISCON_IND:
		return ("T_DISCON_IND");
	case T_DATA_IND:
		return ("T_DATA_IND");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND");
	case T_INFO_ACK:
		return ("T_INFO_ACK");
	case T_BIND_ACK:
		return ("T_BIND_ACK");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK");
	case T_OK_ACK:
		return ("T_OK_ACK");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK");
	case T_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("????");
	}
}

/**
 * tcp_statename: - name TCP state
 * @state: the state to name
 * Returns the name of @state or "(unknown)".
 */
static const char *
tcp_statename(int state)
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

#if 0
/**
 * sock_statename: - name SOCKET state
 * @state: state to name
 * Returns the name of @state or "(uknown)".
 */
static const char *
sock_statename(int state)
{
	switch (state) {
	case SS_FREE:
		return ("SS_FREE");
	case SS_UNCONNECTED:
		return ("SS_UNCONNECTED");
	case SS_CONNECTING:
		return ("SS_CONNECTING");
	case SS_CONNECTED:
		return ("SS_CONNECTED");
	case SS_DISCONNECTING:
		return ("SS_DISCONNECTING");
	default:
		return ("(unknown)");
	}
}
#endif
#endif

/**
 * ss_set_state: - set TPI state of private structure
 * @ss: private structure (locked) for which to set state
 * @state: the state to set
 */
static inline fastcall __hot void
ss_set_state(ss_t *ss, t_scalar_t state)
{
	LOGST(ss, "%s <- %s", tpi_statename(state), tpi_statename(ss->p.info.CURRENT_state));
	ss->p.info.CURRENT_state = state;
}

/**
 * ss_get_state: - get TPI state of private strcutrue
 * @ss: private structure (locked) for which to get state
 * @state: the state to get
 */
static inline fastcall __hot t_scalar_t
ss_get_state(ss_t *ss)
{
	return (ss->p.info.CURRENT_state);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Socket Calls
 *
 *  ------------------------------------------------------------------------
 *  These are wrappered versions of socket calls.
 */

/*
 * Following are some socket calls that are sunken and not provided by the kernel directly.  They
 * are wrappered here for convenience.
 */

static int
sock_bind(struct socket *sock, struct sockaddr *add, int len)
{
	ensure(sock, return (-EFAULT));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->bind, return (-EFAULT));
	ensure(sock->sk, return (-EFAULT));
	return sock->ops->bind(sock, add, len);
}

static int
sock_listen(struct socket *sock, uint cons)
{
	ensure(sock, return (-EPROTO));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->listen, return (-EFAULT));
	ensure(sock->sk, return (-EFAULT));
	return sock->ops->listen(sock, cons);
}

struct socket *sock_alloc(void);

/**
 * sock_accept: - accept a socket connection
 * @sock: the listening socket
 * @newsockp: return value of new accepted socket
 * @cp: connection indication
 *
 * Unfortunately, sock->ops->accept will only accept the next sequential connection indication.  In
 * TLI's case, we want to be able to accept or release connection indications other than the next
 * sequential indication.  To do this we must muck with TCP's accept queue when the SEQ_number is
 * not the next in the queue.  To do this we mimic some of the tcp_accept behavior.
 *
 * This function accepts an open request child and grafts it onto a new socket.  The search for a
 * matching open request child is performed using the @cp connection indication information.  Note
 * that if the open request child disconnects before this function is called, the open request child
 * will simply not be found in the list.
 */
static int
sock_accept(struct socket *sock, struct socket **newsockp, mblk_t *cp)
{
	struct socket *newsock;
	struct ss_conind *ci = (typeof(ci)) cp->b_rptr;

	ensure(sock, return (-EPROTO));
	ensure(sock->sk, return (-EFAULT));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->accept, return (-EFAULT));
	if ((newsock = sock_alloc())) {
		struct sock *sk = sock->sk, *ask = ci->ci_sk;
		struct open_request *req, *req_prev, **reqp;

		lock_sock(sk);
		if (sock_accept_queue_head(sk)) {
			sock_accept_queue_lock(sk);
			/* find connection in queue */
			for (reqp = &sock_accept_queue_head(sk), req_prev = NULL;
			     *reqp && ((*reqp)->sk != ask || SOCK_PRIV((*reqp)->sk) != (void *) ci);
			     req_prev = (*reqp), reqp = &(*reqp)->dl_next) ;
			if ((req = *reqp)) {
				if (!((*reqp) = (*reqp)->dl_next))
					sock_accept_queue_tail(sk) = req_prev;
				sk->sk_ack_backlog--;
				tcp_openreq_fastfree(req);
			}
			sock_accept_queue_unlock(sk);
			if (req) {
				release_sock(sk);
				lock_sock(ask);
				newsock->type = sock->type;
				newsock->ops = sock->ops;
				sock_graft(ask, newsock);
				release_sock(ask);
				newsock->state = SS_CONNECTED;
				*newsockp = newsock;
				return (0);
			}
		}
		release_sock(sk);
		sock_release(newsock);
		return (-ESRCH);
	}
	return (-ENOMEM);
}

static int
sock_connect(struct socket *sock, struct sockaddr *add, int flags)
{
	int err;

	ensure(sock, return (-EPROTO));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->connect, return (-EFAULT));
	ensure(sock->sk, return (-EFAULT));
	if ((err = sock->ops->connect(sock, add, sizeof(*add), O_NONBLOCK)))
		if (err == -EINPROGRESS)
			err = 0;
	return (err);
}

static int
sock_disconnect(struct socket *sock)
{
	int err;
	struct sock *sk;

	ensure(sock, return (-EPROTO));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->connect, return (-EFAULT));
	ensure(sock->sk, return (-EFAULT));
	lock_sock((sk = sock->sk));
	ensure(sk->sk_prot, release_sock(sk); return (-EFAULT));
	ensure(sk->sk_prot->disconnect, release_sock(sk); return (-EFAULT));
	if (!(err = sk->sk_prot->disconnect(sk, O_NONBLOCK))) {
		sock->state = SS_UNCONNECTED;
	} else {
		sock->state = SS_DISCONNECTING;
	}
	release_sock(sk);
	return (err);
}

static int
sock_getsockname(struct socket *sock, struct sockaddr *add, int *len)
{
	ensure(sock, return (-EPROTO));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->getname, return (-EFAULT));
	return sock->ops->getname(sock, add, len, 0);
}

static int
sock_getpeername(struct socket *sock, struct sockaddr *add, int *len)
{
	ensure(sock, return (-EPROTO));
	ensure(sock->ops, return (-EFAULT));
	ensure(sock->ops->getname, return (-EFAULT));
	ensure(sock->sk, return (-EFAULT));
	return sock->ops->getname(sock, add, len, 1);
}

#if 0
static int
sock_getnames(struct socket *sock, struct sockaddr *sadd, int *slen, struct sockaddr *dadd,
	      int *dlen)
{
	int err_src, err_dst;

	ensure(sock, return (-EPROTO));
	err_src = sock_getsockname(sock, sadd, slen);
	err_dst = sock_getpeername(sock, dadd, dlen);
	return (err_dst < 0 ? err_dst : (err_src < 0 ? err_src : 0));
}
#endif

/**
 * ss_socket: - socket create
 * @ss: private structure (locked)
 */
static int
ss_socket(ss_t *ss)
{
	int err;
	int family, type, protocol;

	ensure(ss, return (-EFAULT));
	ensure(!ss->sock, return (-EPROTO));
	family = ss->p.prot.family;
	type = ss->p.prot.type;
	protocol = (ss->p.prot.protocol == IPPROTO_RAW) ? ss->port : ss->p.prot.protocol;
	LOGTX(ss, "SS_CREATE %d:%d:%d", family, type, protocol);
	if (!(err = sock_create(family, type, protocol, &ss->sock))) {
		ensure(ss->sock, return (-EFAULT));
		ensure(ss->sock->sk, return (-EFAULT));
		ss->sock->sk->sk_allocation = GFP_ATOMIC;
		ss_socket_get(ss->sock, ss);
		if (ss->p.prot.family == PF_INET)
			inet_sk(ss->sock->sk)->cmsg_flags |= 0x0f;
		return (0);
	}
	LOGNO(ss, "ERROR: from sock_create %d", err);
	return (err);
}

/**
 * ss_sock_init: - socket init
 * @ss: private structure (locked)
 *
 * For all but sockets of type IPPROTO_RAW, we can create the socket at initialization time.  For
 * IPPROTO_RAW sockets, we cannot create the socket until they have been bound to a protocol.  Those
 * are deferred until bind.
 */
static int
ss_sock_init(ss_t *ss)
{
	if (ss->p.prot.protocol == IPPROTO_RAW)
		return (0);
	return ss_socket(ss);
}

/**
 * ss_bind: - socket bind
 * @ss: private structure (locked)
 *
 * We create IPPROTO_RAW sockets at bind time.  All other sockets are normally created at
 * initialization (open/unbind) time, but we will create them here now if an earlier
 * reinitialization failed.
 */
static int
ss_bind(ss_t *ss, struct sockaddr *add, size_t add_len)
{
	int err;

	if (ss->sock || !(err = ss_socket(ss))) {
		LOGTX(ss, "SS_BIND");
		if (!(err = sock_bind(ss->sock, add, add_len))) {
			ss->src = *add;
			return (0);
		} else
			LOGNO(ss, "ERROR: from sock_bind() %d", err);
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

/**
 * ss_listen: - socket listen
 * @ss: private structure (locked)
 * @cons: maximum number of connect indications
 */
static int
ss_listen(ss_t *ss, uint cons)
{
	int err;
	int type = ss->p.prot.type;

	ensure(type == SOCK_STREAM || type == SOCK_SEQPACKET, return (-EFAULT));
	LOGTX(ss, "SS_LISTEN %d", cons);
	if (!(err = sock_listen(ss->sock, cons))) {
		ss->conind = cons;
		ss->tcp_state = ss->sock->sk->sk_state;
		return (0);
	}
	LOGNO(ss, "ERROR: from sock_listen %d", err);
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

static int t_discon_ind(ss_t *ss, queue_t *q, struct sock *sk, t_scalar_t reason, mblk_t *cp);

/**
 * ss_accept: - socket accept
 * @ss: private structure (locked)
 * @q: active queue (read or write queue)
 * @newsock: pointer to socket point into which to return the resulting socket
 * @cp: a pointer to an mblk_t containing the connection indication
 *
 * Accept the open reqeust child associated with the connection indication @cp.  When the
 * corresponding open request child does not exist, the underlying socket for the child has already
 * disconnected and we will generate a t_discon_ind(), delete the connection indication and
 * reenter by returning %EAGAIN and enabling the active queue to reenter immediately.  The reentry
 * will be with the connection indication removed and the accepting procedure will return
 */
static int
ss_accept(ss_t *ss, queue_t *q, struct socket **newsock, mblk_t *cp)
{
	int err;

	LOGTX(ss, "SS_ACCEPT");
	if (likely(!(err = sock_accept(ss->sock, newsock, cp)))) {
		__bufq_unlink(&ss->conq, cp);
		freemsg(cp);
		return (0);
	}
	/* Only four errors are returned from sock_accept(): */
	switch (-err) {
	case ENOMEM:		/* A socket structure could not be allocated.  Generate a buffer
				   callback to reinvoke when memory is available. */
		mi_bufcall(q, sizeof(struct socket), BPRI_MED);
		goto error;
	case ESRCH:		/* The open request child associated with the connection indication 
				   could not be found.  It needs to be disconnected and another
				   attempt made. */
		if (unlikely((err = t_discon_ind(ss, q, ss->sock->sk, ECONNRESET, cp)) < 0)) {
			switch (err) {
			case EBUSY:
			case EAGAIN:
			case ENOBUFS:
			case ENOMEM:
			case EDEADLK:
				goto error;
			}
			LOGERR(ss, "SWERR: unexpected error %d: %s %s:%d", -err,
				  __FUNCTION__, __FILE__, __LINE__);
			goto error;
		}
		/* Re-enter with state change, connection indication deleted by t_discon_ind(). */
		qenable(q);
		err = -EAGAIN;
		goto error;
	case EPROTO:		/* Unexpectedly there is no socket associated with the private
				   structure. */
	case EFAULT:		/* Unexpectedly there is no underlying socket associated with the
				   socket. */
		break;
	}
	LOGERR(ss, "SWERR: unexpected error %d: %s %s:%d", -err, __FUNCTION__, __FILE__,
		  __LINE__);
	goto error;
      error:
	return (err);
}

/**
 * ss_refuse: - refuse a connection indication
 * @ss: private structure (of listener, locked)
 * @q: active queue
 * @cp: connection indication to refuse
 *
 * This function is used in to refuse an open request child (connection indication) associated with
 * a T_DISCON_REQ issued by the transport user or a T_DISCON_IND being issued by the transport
 * provider.
 *
 * This function attempts to accept the open request child associated with connection indication @cp
 * and disconnect and release the open request child.  This is the equivalent of accepting and
 * immediately refusing the open request.  When the function is successful, the connection
 * indication is deleted.
 *
 * Should the function fail to allocate a socket, %ENOMEM is returned and a buffer callback
 * scheduled to reenter the function once there is sufficient memory to make another attempt.
 *
 * Should the open request child no longer exist (it is already disconnected, but the
 * connection indication has not yet been deleted), then the connection indication is simply removed
 * and no disconnect indication is given and the function succeeds.
 *
 * Should a fatal error occur, the connection indication is still removed and the function succeeds.
 */
static int
ss_refuse(ss_t *ss, queue_t *q, mblk_t *cp)
{
	struct socket *newsock = NULL;
	int err;

	LOGTX(ss, "SS_REFUSE");
	if (likely(!(err = sock_accept(ss->sock, &newsock, cp)))) {
		sock_disconnect(newsock);
		ss_socket_put(&newsock);
	      done:
		__bufq_unlink(&ss->conq, cp);
		freemsg(cp);
		return (0);
	}
	switch (-err) {
	case ENOMEM:		/* A socket structure could not be allocated.  Generate a buffer
				   callback and reinvoke when memory is available. */
		mi_bufcall(q, sizeof(struct socket), BPRI_MED);
		goto error;
	case ESRCH:		/* The open request child associated with the connection indication
				   could not be found, so it is already disconnected.  The
				   connection indication can simply be removed. */
		goto done;
	case EPROTO:		/* Unexpectedly there is no socket associated with the private
				   structure. */
	case EFAULT:		/* Unexpectedly there is no underlying socket associated with the
				   socket. */
		break;
	}
	LOGERR(ss, "SWERR: unexpected error %d: %s %s:%d", -err, __FUNCTION__, __FILE__,
		  __LINE__);
	/* feign success anyways */
	goto done;
      error:
	return (err);
}

/**
 * ss_unbind: - socket unbind
 * @ss: private structure (locked)
 *
 * There is no good way to unbind and rebind a socket in Linux, so we just close the entire socket.
 * The next time we go to bind, we will create a fresh IPPROTO_RAW socket to bind.  All other
 * sockets are recreated now.
 *
 * Actually, socket code is supposed to allow a bind to AF_UNSPEC to unbind a socket.  This is
 * X/Open POSIX behavior.
 */
static int
ss_unbind(ss_t *ss)
{
	LOGTX(ss, "SS_UNBIND");
	ss_socket_put(&ss->sock);
	/* Note: if the reinitialization of the socket fails, we will attempt again to reinitialize 
	   it when an attempt is ever made to rebind the socket.  We just want to have an
	   initialized socket around so that we can effect options management on the socket in the
	   unbound state, if possible. */
	ss_sock_init(ss);
	return (0);
}

/**
 * ss_connect: - socket connect
 * @ss: private structure (locked)
 * @dst: destination addres to which to connect
 */
static int
ss_connect(ss_t *ss, struct sockaddr *dst)
{
	int err;

	ensure(ss->sock, return (-EPROTO));
	LOGTX(ss, "SS_CONNECT");
	if ((err = sock_connect(ss->sock, dst, O_NONBLOCK)))
		LOGNO(ss, "ERROR: from sock_connect() %d", err);
	else {
		ss->tcp_state = ss->sock->sk->sk_state;
		ss->lasterror = 0;
	}
	return (err);
}

/**
 * ss_sendmsg: - socket sendmsg
 * @ss: private structure (locked)
 * @msg: a pre-prepared message header
 * @len: the length to send
 */
static inline fastcall __hot_out int
ss_sendmsg(ss_t *ss, struct msghdr *msg, int len)
{
	int err;

	ensure(ss->sock, return (-EPROTO));
	LOGDA(ss, "SS_SENDMSG");
	{
		mm_segment_t fs = get_fs();

		set_fs(KERNEL_DS);
		err = sock_sendmsg(ss->sock, msg, len);
		set_fs(fs);
	}
	if (err <= 0)
		LOGNO(ss, "ERROR: from sock_sendmsg() %d", err);
	return (err);
}

/**
 * ss_recvmsg: - socket recvmsg
 * @ss: private structure (locked)
 * @msg: a pre-prepared message header
 * @size: the size to receive
 *
 * Modified to include proper adaptation of the control buffer.
 */
static inline fastcall __hot_in int
ss_recvmsg(ss_t *ss, struct msghdr *msg, int size)
{
	int err;
	uint32_t *cbuf = (uint32_t *) msg->msg_control;
	int clen = msg->msg_controllen;

	/* place canary in control buffer */
	if (cbuf && clen > sizeof(*cbuf))
		*cbuf = 0xdeadbeef;

	ensure(ss->sock, return (-EPROTO));
	LOGDA(ss, "SS_RECVMSG");
	{
		mm_segment_t fs = get_fs();

		set_fs(KERNEL_DS);
		err = sock_recvmsg(ss->sock, msg, size, MSG_DONTWAIT | MSG_NOSIGNAL);
		set_fs(fs);
	}
	if (err < 0) {
		LOGNO(ss, "ERROR: from sock_recvmsg() %d", err);
		return (err);
	}
	LOGIO(ss, "recvmsg with len = %d", err);
	if (msg->msg_flags & MSG_CTRUNC) {
		LOGIO(ss, "control message truncated");
		msg->msg_control = NULL;
		msg->msg_controllen = 0;
	} else if (msg->msg_control != (void *) cbuf) {
		LOGIO(ss, "control message pointer moved!");
		LOGIO(ss, "initial control buffer %p", msg->msg_control);
		LOGIO(ss, "initial control length %ld", (long) msg->msg_controllen);
		msg->msg_control = (void *) cbuf;
		msg->msg_controllen = clen - msg->msg_controllen;
		LOGIO(ss, "final control buffer %p", msg->msg_control);
		LOGIO(ss, "final control length %ld", (long) msg->msg_controllen);
	} else if (clen > sizeof(*cbuf) && msg->msg_controllen == clen && cbuf[0] == 0xdeadbeef) {
		LOGIO(ss, "control message unchanged!");
		msg->msg_control = NULL;
		msg->msg_controllen = 0;
	}
	return (err);
}

/**
 * ss_disconnect: - socket disconnect
 *
 * Performing a sock_release (ss_socket_put) from the established state does not affect an abortive
 * release for TCP, but rather, initiates an orderly shutdown rather than an abortive release.  We
 * can try performing a protocol disconnect and see if that works better.  All sockets are supposed
 * to support connection to AF_UNSPEC as a disconnect; however, not all socket types do this
 * correctly, so call the protocol disconnect function directly (which is what sock_connect with an
 * address of AF_UNSPEC does for inet).
 */
static int
ss_disconnect(ss_t *ss)
{
	int err;

	ensure(ss->sock, return (-EPROTO));
	LOGTX(ss, "SS_DISCONNECT");
	if ((err = sock_disconnect(ss->sock)))
		LOGNO(ss, "ERROR: from sock_disconnect() %d", err);
	return (err);
}

/*
 *  SOCKET GETNAMES
 *  ------------------------------------------------------------------------
 *  Get the names from a socket.
 */
static int
ss_getsockname(ss_t *ss)
{
	int err, len = sizeof(ss->src);

	LOGTX(ss, "SS_GETSOCKNAME");
	if (!(err = sock_getsockname(ss->sock, &ss->src, &len)))
		return (len);
	return (err);
}
static int
ss_getpeername(ss_t *ss)
{
	int err, len = sizeof(ss->dst);

	LOGTX(ss, "SS_GETPEERNAME");
	if (!(err = sock_getpeername(ss->sock, &ss->dst, &len)))
		return (len);
	return (err);
}
static int
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

/**
 * m_flush: - issue an M_FLUSH message upstream
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @how: how to flush (FLUSHR, FLUSHW, FLUSHBAND)
 * @band: band to flush when FLUSHBAND set
 *
 * This procedure should be flushing the read queue when FLUSHR is set before passing the message
 * upstream; however, there seemed to be some problems in doing so.  Turn this back on once the
 * problems are resolved. 
 */
static int
m_flush(ss_t *ss, queue_t *q, mblk_t *msg, int how, int band)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 2, BPRI_HI))))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	mp->b_wptr[0] = how;
	mp->b_wptr[1] = band;
	mp->b_wptr += 2;
#if 0
	/* get things started off right */
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(ss->rq, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(ss->rq, FLUSHDATA);
	}
#endif
	freemsg(msg);
	LOGTX(ss, "<- M_FLUSH");
	putnext(ss->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error: - issue an M_ERROR message upstream
 * @ss: private structure (locked or unlocked)
 * @q: active qeuue
 * @msg: message to free upon success
 * @error: the error number to pass in the message
 *
 * The caller should determine what to do visa-vi the TPI state and the disposition of any attached
 * socket.
 */
static int
m_error(ss_t *ss, queue_t *q, mblk_t *msg, int error)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 2, BPRI_HI))))
		goto enobufs;
	mp->b_datap->db_type = M_ERROR;
	mp->b_wptr[0] = error < 0 ? -error : error;
	mp->b_wptr[1] = error < 0 ? -error : error;
	mp->b_wptr += 2;
	ss_set_state(ss, ss->oldstate);
	freemsg(msg);
	LOGTX(ss, "<- M_ERROR %d", error);
	putnext(ss->rq, mp);
#if 0
	if (ss->sock) {
		ss_disconnect(ss);
		ss_set_state(ss, TS_IDLE);
		ss_socket_put(&ss->sock);
		ss_set_state(ss, TS_UNBND);
	}
#endif
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_hangup: - issue an M_HANGUP message upstream
 * @ss: private structure (locked or unlocked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 *
 * The caller should determine what to do visa-vi the TPI state and the disposition of any attached
 * socket.
 */
static inline fastcall __unlikely int
m_hangup(ss_t *ss, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 0, BPRI_HI))))
		goto enobufs;
	mp->b_datap->db_type = M_HANGUP;
	ss_set_state(ss, ss->oldstate);
	freemsg(msg);
	LOGTX(ss, "<- M_HANGUP");
	putnext(ss->rq, mp);
#if 0
	if (ss->sock) {
		ss_disconnect(ss);
		ss_set_state(ss, TS_IDLE);
		ss_socket_put(&ss->sock);
		ss_set_state(ss, TS_UNBND);
	}
#endif
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error_reply: - reply with an M_ERROR or M_HANGUP message
 * @ss: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @err: error to place in M_ERROR message
 *
 * Generates an M_ERROR or M_HANGUP message upstream to generate a fatal error when required by the
 * TPI specification.  The four errors that require the message to be placed on the message queue
 * (%EBUSY, %ENOBUFS, %EAGAIN, %ENOMEM) are simply returned.  Other errors are either translated to
 * an M_ERROR or M_HANGUP message.  If a message block cannot be allocated -%ENOBUFS is returned,
 * otherwise zero (0) is returned.
 */
noinline fastcall __unlikely int
m_error_reply(ss_t *ss, queue_t *q, mblk_t *msg, int err)
{
	int hangup = 0;
	int error = err;

	if (error > 0)
		error = -error;
	switch (-error) {
	case 0:
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
		return (error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
	case ECONNRESET:
	case ECONNREFUSED:
		hangup = 1;
		error = EPIPE;
		break;
	case EFAULT:
		LOGERR(ss, "%s() fault", __FUNCTION__);
	default:
	case EPROTO:
		err = (err < 0) ? -err : err;
		break;
	}
#if 0
	/* always flush before M_ERROR or M_HANGUP */
	if (m_flush(ss, q, NULL, FLUSHRW, 0) == -ENOBUFS)
		return (-ENOBUFS);
#endif
	if (hangup)
		return m_hangup(ss, q, msg);
	return m_error(ss, q, msg, err);
}

/**
 * t_conn_ind: - issue a T_CONN_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying open request child socket
 *
 * This function creates a new connection indication and sequence number from the open request child
 * and generates a connection indication to the TLI user.
 */
static fastcall int
t_conn_ind(ss_t *ss, queue_t *q, struct sock *sk)
{
	mblk_t *mp, *cp = NULL;
	struct T_conn_ind *p;
	struct ss_conind *ci;
	struct sockaddr_in *src;
	size_t opt_len = ss_size_conn_opts(ss);
	int err;

	if (unlikely(bufq_length(&ss->conq) > ss->conind))
		goto eagain;
	if (unlikely(!canputnext(ss->rq)))
		goto ebusy;
	if (unlikely(!(cp = mi_allocb(q, sizeof(*ci), BPRI_MED))))
		goto enobufs;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + sizeof(*src) + opt_len, BPRI_MED))))
		goto enobufs;

	ci = (typeof(ci)) cp->b_wptr;
	ci->ci_rflags = 0;
	ci->ci_wflags = 0;
	ci->ci_sk = sk;
	ci->ci_seq = (ss->seq + 1) ? : (ss->seq + 2);
	ci->ci_parent = ss;
	cp->b_wptr += sizeof(*ci);
	cp->b_next = cp->b_prev = cp->b_cont = NULL;


	SOCK_PRIV(sk) = ci;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = sizeof(*src);
	p->SRC_offset = sizeof(*p);
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + sizeof(*src) : 0;
	p->SEQ_number = ci->ci_seq;
	mp->b_wptr += sizeof(*p);

	src = (typeof(src)) mp->b_wptr;
	src->sin_family = AF_INET;
	src->sin_port = sock_dport(sk);
	src->sin_addr.s_addr = sock_daddr(sk);
	mp->b_wptr += sizeof(*src);

	if (opt_len) {
		if ((err = t_build_conn_opts(ss, mp->b_wptr, opt_len)) >= 0)
			mp->b_wptr += opt_len;
		else {
			freemsg(mp);
			LOGNO(ss, "ERROR: option build fault");
			freemsg(cp);
			return (0);
		}
	}
	ss->seq = ++ss->seq ? : ++ss->seq;
	SOCK_PRIV(sk) = (void *) ci;
	__bufq_queue(&ss->conq, cp);
	ss_set_state(ss, TS_WRES_CIND);
	LOGTX(ss, "<- T_CONN_IND");
	putnext(ss->rq, mp);
	return (0);		/* absorbed cp */

      enobufs:
	freemsg(cp);
	return (-ENOBUFS);
      ebusy:
	LOGNO(ss, "ERROR: flow controlled");
	return (-EBUSY);
      eagain:
	LOGNO(ss, "ERROR: too many conn inds");
	return (-EAGAIN);
}

/**
 * t_conn_con: - issue a T_CONN_CON primitive
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 *
 * Issues a connection confirmation primitive to the TPI user as a result of a state change on the
 * underlying socket to the TCP_ESTABLISHED state from the TCP_SYN_SENT state.  Note that the
 * responding address is always the address to which the connection was attempted.  All options with
 * end-to-end significant and options that were specified on connection request are returned on
 * connection confirmation.
 *
 * Note that because only one connect confirmation message is sent on any given Stream, this
 * function does not check flow control.  This means that it is highly unlikely that this message
 * will fail.
 */
static fastcall int
t_conn_con(ss_t *ss, queue_t *q)
{
	mblk_t *mp;
	struct T_conn_con *p;
	struct sockaddr *res = &ss->dst;
	size_t res_len = ss_addr_size(res);
	size_t opt_len = ss_size_conn_opts(ss);
	int err;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
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
			LOGNO(ss, "ERROR: option build fault");
			return (0);
		}
	}
	ss_set_state(ss, TS_DATA_XFER);
	LOGTX(ss, "<- T_CONN_CON");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue a T_DISCON_IND primitive
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: the underlying socket (or NULL)
 * @reason: disconnect reason
 * @cp: connection indication
 *
 * Disconnect indications are issued on normal Streams as follows: (In this case the sequence number
 * is always zero.)
 *
 * TS_WCON_CREQ state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state (posthumously
 *	from the TCP_SYN_SENT state).
 * TS_DATA_XFER state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state (posthumously
 *	from the TCP_ESTABLISHED state).
 * TS_WIND_ORDREL state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state from the
 *	TCP_CLOSE_WAIT state.
 * TS_WREQ_ORDREL state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state from the
 *	TCP_ESTABLISHED or TCP_FIN_WAIT1 state.
 *
 * Disconnect indications are issued on listening Streams as follows: (In this case the sequence
 * number is never zero.)
 *
 * TS_WRES_CIND state:
 *	when the child socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state (posthumously from
 *	the TCP_SYN_RECV state).
 *
 */
static int
t_discon_ind(ss_t *ss, queue_t *q, struct sock *sk, t_scalar_t reason, mblk_t *cp)
{
	mblk_t *mp = NULL;
	struct T_discon_ind *p;
	t_uscalar_t seq;
	int err;

	if (!reason && !(reason = ss->lasterror) && sk)
		reason = ss->lasterror = sock_error(sk);
	reason = reason < 0 ? -reason : reason;

	seq = cp ? ((struct ss_conind *) (cp->b_rptr))->ci_seq : 0;

	switch (ss_get_state(ss)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
		   TS_WACK_ORDREL [sic] state, the stream must be flushed before sending up the
		   T_DISCON_IND primitive. */
		if (unlikely(m_flush(ss, ss->rq, NULL, FLUSHRW, 0)))
			goto enobufs;
		/* fall through */
	case TS_WCON_CREQ:
		assure(cp == NULL);
		break;
	case TS_WRES_CIND:
		/* If we are a listening Stream then the flow of connection indications and
		   disconnect indications must be controlled and flow control must be checked.  For
		   other Streams, only one disconnect indication can be issued for a given Stream
		   and flow control need not be checked. */
		assure(cp != NULL);
		if (unlikely(!canputnext(ss->rq)))
			goto ebusy;
		break;
	default:
		LOGERR(ss, "SWERR: out of state: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
		/* remove it anyway and feign success */
		if (cp && (err = ss_refuse(ss, q, cp)))
			goto error;
		return (0);
	}
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	if (unlikely(cp && (err = ss_refuse(ss, q, cp)))) {
		freemsg(mp);
		goto error;
	}

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	ss->lasterror = 0;
	if (!bufq_length(&ss->conq))
		ss_set_state(ss, TS_IDLE);
	else
		ss_set_state(ss, TS_WRES_CIND);
	LOGTX(ss, "<- T_DISCON_IND");
	putnext(ss->rq, mp);
	return (0);

      ebusy:
	LOGNO(ss, "ERROR: Flow controlled");
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/**
 * t_data_ind: - issue a T_DATA_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: prepared message header
 * @dp: associated data
 */
static inline fastcall __hot_in int
t_data_ind(ss_t *ss, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = (msg->msg_flags & MSG_EOR) ? 0 : T_MORE;
	mp->b_wptr += sizeof(*p);
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(ss, "<- T_DATA_IND");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_exdata_ind: - issue a T_EXDATA_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: prepared message header
 * @dp: associated data
 */
static inline fastcall __hot_in int
t_exdata_ind(ss_t *ss, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = (msg->msg_flags & MSG_EOR) ? 0 : T_MORE;
	mp->b_wptr += sizeof(*p);
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(ss, "<- T_EXDATA_IND");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_info_ack: - issue a T_INFO_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall int
t_info_ack(ss_t *ss, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct T_info_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	*p = ss->p.info;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(ss, "<- T_INFO_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_bind_ack: - issue a T_BIND_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @add: bound address
 * @conind: bound maximum number of outstanding connection indications
 */
static fastcall int
t_bind_ack(ss_t *ss, queue_t *q, mblk_t *msg, struct sockaddr *add, t_uscalar_t conind)
{
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = ss_addr_size(add);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))))
		goto enobufs;

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
	freemsg(msg);
	LOGTX(ss, "<- T_BIND_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issue a T_ERROR_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: positive TPI error or negative UNIX error
 */
static int
t_error_ack(ss_t *ss, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t error)
{
	mblk_t *mp;
	struct T_error_ack *p;

	switch (-error) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (error);
	case 0:
		freemsg(msg);
		return (0);
	}
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	ss_set_state(ss, ss->oldstate);
	freemsg(msg);
	LOGTX(ss, "<- T_ERROR_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

static fastcall __hot_in void ss_r_recvmsg(ss_t *ss, queue_t *q);

/**
 * t_ok_ack: - issue a T_OK_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct primitive
 * @cp: connection indication event to accept or disconnect
 * @as: accepting Stream private structure (also locked)
 *
 * The accepting Stream must be locked before we do all of these perambulations on it.  The
 * accepting Stream must be locked and unlocked by the caller.
 */
static int
t_ok_ack(ss_t *ss, queue_t *q, mblk_t *msg, t_scalar_t prim, mblk_t *cp, ss_t *as)
{
	int err = 0;
	mblk_t *mp;
	struct T_ok_ack *p;
	struct socket *sock = NULL;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (ss_get_state(ss)) {
	case TS_WACK_CREQ:
		ss->lasterror = 0;
		if ((err = ss_connect(ss, &ss->dst)))
			goto free_error;
		ss_getnames(ss);
		ss_set_state(ss, TS_WCON_CREQ);
		break;
	case TS_WACK_UREQ:
		if ((err = ss_unbind(ss)))
			goto free_error;
		/* TPI spec says that if the provider must flush both queues before responding with 
		   a T_OK_ACK primitive when responding to a T_UNBIND_REQ. This is to flush queued
		   data for connectionless providers. */
		switch (ss->p.prot.type) {
		case SOCK_DGRAM:
		case SOCK_RAW:
		case SOCK_RDM:
			if (unlikely(m_flush(ss, q, NULL, FLUSHRW, 0)))
				goto enobufs;
			break;
		case SOCK_SEQPACKET:
		case SOCK_STREAM:
			break;
		default:
			LOGERR(ss, "SWERR: unknown socket type %d: %s %s:%d", ss->p.prot.type,
				  __FUNCTION__, __FILE__, __LINE__);
			break;
		}
		ss_set_state(ss, TS_UNBND);
		break;
	case TS_WACK_CRES:
		ensure(as && cp, goto free_error);
		if ((err = ss_accept(ss, q, &sock, cp)))
			goto free_error;
		ss_socket_put(&as->sock);	/* get rid of old socket */
		as->sock = sock;
		ss_socket_grab(as->sock, as);
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
		ss_r_recvmsg(as, as->rq);
		break;
	case TS_WACK_DREQ7:
		ensure(cp, goto free_error);
		if ((err = ss_refuse(ss, q, cp)))
			goto free_error;
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
		/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
		   TS_WACK_ORDREL [sic] state, the stream must be flushed before responding with
		   the T_OK_ACK primitive. */
		if (unlikely(m_flush(ss, q, NULL, FLUSHRW, 0)))
			goto enobufs;
		ss_set_state(ss, TS_IDLE);
		break;
	default:
		break;
		/* Note: if we are not in a WACK state we simply do not change state. This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
	}
	freemsg(msg);
	LOGTX(ss, "<- T_OK_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(ss, q, msg, prim, err);
}

/**
 * t_unitdata_ind: - issue a T_UNITDATA_IND primitive
 * @ss: private structure (locked)
 * @q; active queue
 * @msg: pre-prepared message header
 * @dp: the data
 */
static inline fastcall __hot_in int
t_unitdata_ind(ss_t *ss, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t opt_len = ss_opts_size(ss, msg);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + msg->msg_namelen + opt_len, BPRI_MED))))
		goto enobufs;

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
		ss_opts_build(ss, msg, mp->b_wptr, opt_len, NULL);
		mp->b_wptr += opt_len;
	}
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(ss, "<- T_UNITDATA_IND");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: - issue a T_UDERROR_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: pre-prepared message header
 * @dp: the data
 *
 * This primitive indicates to the transport user that a datagram with the specified destination
 * address and options produced an error.
 *
 * This is not called, but should be when T_UNITDATA_REQ fails due to options errors or permission.
 */
static inline fastcall __hot_get int
t_uderror_ind(ss_t *ss, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t opt_len = ss_opts_size(ss, msg);

	if (unlikely(!bcanputnext(ss->rq, 1)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + msg->msg_namelen + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = msg->msg_namelen;
	p->DEST_offset = msg->msg_namelen ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + msg->msg_namelen : 0;
	p->ERROR_type = 0;
	mp->b_wptr += sizeof(*p);
	if (msg->msg_namelen) {
		bcopy(msg->msg_name, mp->b_wptr, msg->msg_namelen);
		mp->b_wptr += msg->msg_namelen;
	}
	if (opt_len) {
		ss_opts_build(ss, msg, mp->b_wptr, opt_len, &p->ERROR_type);
		mp->b_wptr += opt_len;
	}
	if (dp) {
		dp->b_band = 0;	/* sometimes non-zero */
		dp->b_flag &= ~MSGMARK;	/* sometimes marked */
		mp->b_cont = dp;
	}
	LOGTX(ss, "<- T_UDERROR_IND");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
      ebusy:
	LOGNO(ss, "ERROR: Flow controlled");
	return (-EBUSY);
}

/**
 * t_optmgmt_ack: - issue a T_OPTMGMT_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: management flags
 * @req: pointer to the requested options
 * @req_len: length of the requested options
 * @opt_len: estimate of the length of the reply options
 *
 * Note: opt_len is conservative but might not be actual size of the output options.  This will be
 * adjusted when the option buffer is built.
 */
static fastcall int
t_optmgmt_ack(ss_t *ss, queue_t *q, mblk_t *msg, t_scalar_t flags, unsigned char *req,
	      size_t req_len, size_t opt_len)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

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
	freemsg(msg);
	LOGTX(ss, "<- T_OPTMGMT_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue a T_ORDREL_IND primitive
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 *
 * This primitive is an orderly release indication.  An orderly release indication is set as a
 * result of a state change of the underlying socket from the TCP_ESTABLISHED state to the
 * TCP_CLOSE_WAIT state (half close), or from the TCP_FIN_WAIT2 state to the TCP_TIME_WAIT or
 * TCP_CLOSE state (full close after half close).
 *
 * Note that because only one of these primitives can be sent on a given Stream, flow control is not
 * checked.  This means that it is very unlikely that this message will fail.
 */
static int
t_ordrel_ind(ss_t *ss, queue_t *q)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ORDREL_IND;
	mp->b_wptr += sizeof(*p);
	switch (ss_get_state(ss)) {
	case TS_DATA_XFER:
		/* In this case the underlying socket has transitioned form the TCP_ESTABLISHED
		   state to the TCP_CLOSE_WAIT state. */
		ss_set_state(ss, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		/* In this case the underlying socket has transitioned from the TCP_FIN_WAIT2 state
		   to the TCP_TIME_WAIT or TCP_CLOSE state. */
		ss_set_state(ss, TS_IDLE);
		break;
	}
	LOGTX(ss, "<- T_ORDREL_IND");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_optdata_ind: - issue a T_OPTDATA_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: pre-prepared message header
 * @dp: the data
 */
static inline fastcall __hot_in int
t_optdata_ind(ss_t *ss, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t opt_len = ss_opts_size(ss, msg);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = (msg->msg_flags & MSG_OOB) ? 1 : 0;	/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag =
	    ((msg->msg_flags & MSG_EOR) ? 0 : T_ODF_MORE) |
	    ((msg->msg_flags & MSG_OOB) ? T_ODF_EX : 0);
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (opt_len) {
		ss_opts_build(ss, msg, mp->b_wptr, opt_len, NULL);
		mp->b_wptr += opt_len;
	}
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(ss, "<- T_OPTDATA_IND");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/**
 * t_addr_ack: - issue a T_ADDR_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @loc: local address (or NULL)
 * @rem: remote address (or NULL)
 */
static fastcall int
t_addr_ack(ss_t *ss, queue_t *q, mblk_t *msg, struct sockaddr *loc, struct sockaddr *rem)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = ss_addr_size(loc);
	size_t rem_len = ss_addr_size(rem);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))))
		goto enobufs;

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
	freemsg(msg);
	LOGTX(ss, "<- T_ADDR_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}
#endif

#ifdef T_CAPABILITY_ACK
/**
 * t_capability_ack: - issue a T_CAPABILITY_ACK primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @caps: capability bits
 * @type: message type (M_PROTO or M_PCPROTO)
 *
 * Note that TPI Revision 2 Draft 2 says that if the T_CAPABILITY_REQ is sent as a M_PCPROTO then
 * the the T_CAPABILITY_ACK must be sent as an M_PCPROTO and that if the T_CAPABILITY_REQ was sent
 * as a M_PROTO, then the T_CAPABILITY_ACK must also be sent as a M_PROTO.
 */
static fastcall int
t_capability_ack(ss_t *ss, queue_t *q, mblk_t *msg, t_uscalar_t caps, int type)
{
	mblk_t *mp;
	struct T_capability_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

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
	freemsg(msg);
	LOGTX(ss, "<- T_CAPABILITY_ACK");
	putnext(ss->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}
#endif

/*
 *  =========================================================================
 *
 *  SENDMSG and RECVMSG
 *
 *  =========================================================================
 */

/**
 * ss_sock_sendmsg: - sendmsg
 * @ss: private structure (locked)
 * @mp: the data message to send
 * @msg: a pre-prepared message header
 *
 * Convert the mblk to send into an iovec and the options into a control message and then call
 * sendmsg on the socket with the kernel data segment.  The socket will handle moving data from the
 * mblks.
 */
static fastcall __hot_out int
ss_sock_sendmsg(ss_t *ss, mblk_t *mp, struct msghdr *msg)
{
	int err;
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
#if 0
	err = -EBUSY;
	if (len > sock_wspace(ss->sock->sk))
		goto out;
#endif
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
		LOGIO(ss, "sendmsg with len = %d", len);
		err = ss_sendmsg(ss, msg, len);
	}
	if (err < 0)
		goto out;
	if (!err) {
		err = -EBUSY;
		goto out;
	}
	if (err >= len) {
		freemsg(mp);
		return (0);	/* full write */
	}
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
			dassert(dp != NULL);
		}
		if (!adjmsg(dp, err))
			LOGERR(ss, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
		err = -EAGAIN;
		goto out;
	}
	err = -EFAULT;
      out:
	switch (-err) {
	case ENOMEM:
	case ENOBUFS:
		mi_bufcall(ss->wq, len, BPRI_LO);
		/* This buffer call is just to kick us.  Because allocb uses kmalloc for mblks, if
		   we can allocate an mblk, then another kernel routine can allocate that much
		   memory too. */
		return (err);
	case EAGAIN:
	case EDEADLK:
	case EBUSY:
		qenable(ss->wq);
		return (err);
	case EPIPE:
		err = -ECONNRESET;
	case ENETDOWN:
	case EHOSTUNREACH:
	case ECONNRESET:
	case ECONNREFUSED:
		if (ss->lasterror == 0)
			ss->lasterror = err;
		return (err);
	default:
		return (err);
		// return m_error_reply(ss, ss->rq, mp, err);
	}
}

/**
 * ss_setup_size: - return the size of data available
 * @ss: private structure (locked)
 * @sk: the underlying socket
 * @type: the socket type
 * @band: normal or expedited/error band
 *
 * Calculates the size of message available to read depending on the type of socket and band of
 * message.  Returns the size of message available or zero if no message is available or an error
 * occurred.
 */
static inline fastcall __hot_in int
ss_setup_size(ss_t *ss, struct sock *sk, const int type, const char band)
{
	int size;
	struct sk_buff *skb;

	switch (__builtin_expect(type, SOCK_STREAM)) {
	case SOCK_STREAM:
		if (likely(!band)) {
			skb = skb_peek(&sk->sk_receive_queue);
			size = skb ? (skb->len ? : 1) : 0;
		} else {
			size = (tcp_sk(sk)->urg_data && (tcp_sk(sk)->urg_data != TCP_URG_READ)
				&& (tcp_sk(sk)->urg_data & TCP_URG_VALID)) ? 1 : 0;
		}
		break;
	case SOCK_DGRAM:
	case SOCK_RAW:
		if (likely(!band)) {
			skb = skb_peek(&sk->sk_receive_queue);
			size = skb ? (skb->len ? : 1) : 0;
		} else {
			skb = skb_peek(&sk->sk_error_queue);
			size = skb ? (skb->len ? : 1) : 0;
		}
		break;
	case SOCK_SEQPACKET:
	case SOCK_RDM:
		size = atomic_read(&sk->sk_rmem_alloc);
		break;
	default:
		LOGERR(ss, "SWERR: unknown socket type %d: %s %s:%d",
			  type, __FUNCTION__, __FILE__, __LINE__);
		size = 0;
		break;
	}
	return (size);
}

#define SS_CMAX (sizeof(uint32_t) << 5)

/**
 * ss_setup_message: - set up message header from message block
 * @mp: message block
 * @type: type of socket
 *
 * Sets up a message header and all of its ancilliary strucutres within a message block.  The
 * message block must have been allocated of sufficient size to hold all structures.  The function
 * returns a pointer to the message header suitable for use with recvmsg().
 */
static inline fastcall __hot_in struct msghdr *
ss_setup_message(mblk_t *mp, int size, int type)
{
	struct msghdr *msg;
	struct sockaddr *add;
	struct iovec *iov;
	void *cbuf;
	static const size_t clen = SS_CMAX;

	msg = (typeof(msg)) mp->b_wptr;
	mp->b_wptr += sizeof(*msg);
	cbuf = (typeof(cbuf)) mp->b_wptr;
	msg->msg_control = cbuf;
	msg->msg_controllen = clen;
	mp->b_wptr += clen;
	add = (typeof(add)) mp->b_wptr;
	add->sa_family = AF_INET;
	msg->msg_name = add;
	msg->msg_namelen = ss_addr_size(add);
	mp->b_wptr += sizeof(*add);
	iov = (typeof(iov)) mp->b_wptr;
	mp->b_wptr += sizeof(*iov);
	mp->b_rptr = mp->b_wptr;
	iov->iov_base = mp->b_rptr;
	iov->iov_len = size;
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
	msg->msg_flags = MSG_DONTWAIT | MSG_NOSIGNAL;

	return (msg);
}

/**
 * ss_data_ind: - generate T_DATA_IND or T_OPTDATA_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying socket
 * @type: socket type
 *
 * Reads data, when available, from the underlying socket and formulates T_DATA_IND or optionally
 * T_OPDATA_IND messages and passes them upstream.  Upon success, the amount of data read from the
 * socket is returned.  Upon failure, a negative error code is returned.
 */
noinline fastcall __hot_in int
ss_data_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int size, err;
	mblk_t *mp = NULL, *dp;
	struct msghdr *msg;
	static const size_t plen = sizeof(struct T_optdata_ind) + ss_opts_maxsize;
	static const size_t dlen =
	    sizeof(struct msghdr) + sizeof(struct sockaddr) + sizeof(struct iovec) + SS_CMAX;

	if (unlikely(!bcanputnext(ss->rq, 0)))
		goto ebusy;

	if ((size = ss_setup_size(ss, sk, type, 0)) == 0)
		goto eagain;

	if (unlikely(!(mp = mi_allocb(q, plen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!(dp = mp->b_cont = mi_allocb(q, dlen + size, BPRI_MED))))
		goto enobufs;

	msg = ss_setup_message(dp, size, type);

	if (likely(type == SOCK_STREAM)) {
		msg->msg_name = NULL;
		msg->msg_namelen = 0;
	}

	if (unlikely((err = ss_recvmsg(ss, msg, size)) <= 0)) {
		switch (__builtin_expect(-err, EAGAIN)) {
		case EAGAIN:
		case 0:
			LOGIO(ss, "no data to read!");
			goto eagain;
		case ENOBUFS:
		case ENOMEM:
			mi_bufcall(q, size, BPRI_MED);
			goto error;
		case EDEADLK:
		case EBUSY:
			qenable(q);
			goto error;
		}
		LOGERR(ss, "Error %d: %s : %s:%d", -err, __FUNCTION__, __FILE__, __LINE__);
		goto error;
	}

	if (unlikely(msg->msg_flags & MSG_TRUNC))
		goto efault;

	dp->b_wptr = dp->b_rptr + err;

	mp->b_datap->db_type = M_PROTO;
	if (msg->msg_controllen) {
		struct T_optdata_ind *p;

		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_IND;
		p->DATA_flag =
		    (type == SOCK_STREAM) ? 0 : (msg->msg_flags & MSG_EOR) ? 0 : T_ODF_MORE;
		p->OPT_length = ss_opts_size(ss, msg);
		p->OPT_offset = p->OPT_length ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		if (p->OPT_length) {
			ss_opts_build(ss, msg, mp->b_wptr, p->OPT_length, NULL);
			mp->b_wptr += p->OPT_length;
		}
		LOGTX(ss, "<- T_OPTDATA_IND");
	} else {
		struct T_data_ind *p;

		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_DATA_IND;
		p->MORE_flag = (type == SOCK_STREAM) ? 0 : (msg->msg_flags & MSG_EOR) ? 0 : T_MORE;
		mp->b_wptr += sizeof(*p);
		LOGTX(ss, "<- T_DATA_IND");
	}
	putnext(ss->rq, mp);
	return (err);

      efault:
	err = -EFAULT;
	goto error;
      eagain:
	err = -EAGAIN;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * ss_exdata_ind: - generate T_EXDATA_IND or T_OPTDATA_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying socket
 * @type: socket type
 *
 * Reads expedited data, when available, from the underlying socket and formulates T_EXDATA_IND or
 * optionally T_OPDATA_IND messages and passes them upstream.  Upon success, the amount of data read
 * from the socket is returned.  Upon failure, a negative error code is returned.
 */
noinline fastcall __hot_in int
ss_exdata_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int size, err;
	mblk_t *mp = NULL, *dp;
	struct msghdr *msg;
	static const size_t plen = sizeof(struct T_optdata_ind) + ss_opts_maxsize;
	static const size_t dlen =
	    sizeof(struct msghdr) + sizeof(struct sockaddr) + sizeof(struct iovec) + SS_CMAX;

	if (unlikely(!bcanputnext(ss->rq, 1)))
		goto ebusy;

	if ((size = ss_setup_size(ss, sk, type, 1)) == 0)
		goto eagain;

	if (unlikely(!(mp = mi_allocb(q, plen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!(dp = mp->b_cont = mi_allocb(q, dlen + size, BPRI_MED))))
		goto enobufs;

	msg = ss_setup_message(dp, size, type);

	if (likely(type == SOCK_STREAM)) {
		msg->msg_name = NULL;
		msg->msg_namelen = 0;
	}
	msg->msg_flags |= MSG_OOB;

	if (unlikely((err = ss_recvmsg(ss, msg, size)) <= 0)) {
		switch (__builtin_expect(-err, EAGAIN)) {
		case EINVAL:	/* No OOB data. */
		case EAGAIN:
		case 0:
			LOGIO(ss, "no data to read!");
			goto eagain;
		case ENOBUFS:
		case ENOMEM:
			mi_bufcall(q, size, BPRI_MED);
			goto error;
		case EDEADLK:
		case EBUSY:
			qenable(q);
			goto error;
		}
		LOGERR(ss, "Error %d: %s : %s:%d", -err, __FUNCTION__, __FILE__, __LINE__);
		goto error;
	}

	if (unlikely(msg->msg_flags & MSG_TRUNC))
		goto efault;

	dp->b_wptr = dp->b_rptr + err;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	if (msg->msg_controllen) {
		struct T_optdata_ind *p;

		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_IND;
		p->DATA_flag = T_ODF_EX |
		    ((type == SOCK_STREAM) ? 0 : (msg->msg_flags & MSG_EOR) ? 0 : T_ODF_MORE);
		p->OPT_length = ss_opts_size(ss, msg);
		p->OPT_offset = p->OPT_length ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		if (p->OPT_length) {
			ss_opts_build(ss, msg, mp->b_wptr, p->OPT_length, NULL);
			mp->b_wptr += p->OPT_length;
		}
		LOGTX(ss, "<- T_OPTDATA_IND");
	} else {
		struct T_exdata_ind *p;

		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_EXDATA_IND;
		p->MORE_flag = (type == SOCK_STREAM) ? 0 : (msg->msg_flags & MSG_EOR) ? 0 : T_MORE;
		mp->b_wptr += sizeof(*p);
		LOGTX(ss, "<- T_EXDATA_IND");
	}
	putnext(ss->rq, mp);
	return (err);

      efault:
	err = -EFAULT;
	goto error;
      eagain:
	err = -EAGAIN;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * ss_unitdata_ind: - generate T_UNITDATA_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying socket
 * @type: socket type
 *
 * Reads unit data, when available, from the underlying socket and formulates T_UNITDATA_IND
 * messages and passes them upstream.  Upon success, the amount of data read from the socket is
 * returned.  Upon failure, a negative error code is returned.
 */
noinline fastcall __hot_in int
ss_unitdata_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int size, err;
	mblk_t *mp = NULL, *dp;
	struct msghdr *msg;
	struct T_unitdata_ind *p;
	static const size_t plen =
	    sizeof(struct T_unitdata_ind) + sizeof(struct sockaddr) + ss_opts_maxsize;
	static const size_t dlen =
	    sizeof(struct msghdr) + sizeof(struct sockaddr) + sizeof(struct iovec) + SS_CMAX;

	if (unlikely(!bcanputnext(ss->rq, 0)))
		goto ebusy;

	/* Are zero-length UDP or RAW messages possible? */
	if ((size = ss_setup_size(ss, sk, type, 0)) == 0)
		goto eagain;

	if (unlikely(!(mp = mi_allocb(q, plen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!(dp = mp->b_cont = mi_allocb(q, dlen + size, BPRI_MED))))
		goto enobufs;

	msg = ss_setup_message(dp, size, type);

	/* Make this < 0 if it seems possible that zero-length data can be delivered. */
	if (unlikely((err = ss_recvmsg(ss, msg, size)) <= 0)) {
		switch (__builtin_expect(-err, EAGAIN)) {
		case EAGAIN:
			LOGIO(ss, "no data to read!");
			goto error;
		case ENOBUFS:
		case ENOMEM:
			mi_bufcall(q, size, BPRI_MED);
			goto error;
		case EDEADLK:
		case EBUSY:
			qenable(q);
			goto error;
		case 0:
			LOGIO(ss, "zero data!");
			goto eagain;
		}
		LOGERR(ss, "Error %d: %s : %s:%d", -err, __FUNCTION__, __FILE__, __LINE__);
		goto error;
	}

	/* leave zero length data blocks for now */
	dp->b_wptr = dp->b_rptr + err;

	if (unlikely(msg->msg_flags & MSG_TRUNC))
		goto efault;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = msg->msg_namelen;
	p->SRC_offset = msg->msg_namelen ? sizeof(*p) : 0;
	p->OPT_length = ss_opts_size(ss, msg);
	p->OPT_offset = p->OPT_length ? sizeof(*p) + msg->msg_namelen : 0;
	mp->b_wptr += sizeof(*p);
	if (msg->msg_namelen) {
		bcopy(msg->msg_name, mp->b_wptr, msg->msg_namelen);
		mp->b_wptr += msg->msg_namelen;
	}
	if (p->OPT_length) {
		ss_opts_build(ss, msg, mp->b_wptr, p->OPT_length, NULL);
		mp->b_wptr += p->OPT_length;
	}
	LOGTX(ss, "<- T_UNITDATA_IND");
	putnext(ss->rq, mp);
	return (err);

      efault:
	LOGIO(ss, "message was truncated");
	err = 0;
	goto error;
      eagain:
	err = -EAGAIN;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * ss_uderror_ind: - generate T_UDERROR_IND primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying socket
 * @type: socket type
 *
 * Reads unit data errors, when available, from the underlying socket and formulates T_UDERROR_IND
 * messages and passes them upstream.  Upon success, zero is returned.  Upon failure, a negative
 * error code is returned.
 */
noinline fastcall __hot_in int
ss_uderror_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int size, err;
	mblk_t *mp = NULL, *dp;
	struct msghdr *msg;
	struct T_uderror_ind *p;
	static const size_t plen =
	    sizeof(struct T_uderror_ind) + sizeof(struct sockaddr) + ss_opts_maxsize;
	static const size_t dlen =
	    sizeof(struct msghdr) + sizeof(struct sockaddr) + sizeof(struct iovec) + SS_CMAX;

	if (unlikely(!bcanputnext(ss->rq, 1)))
		goto ebusy;

	if ((size = ss_setup_size(ss, sk, type, 1)) < 0)
		goto eagain;
	if (size == 0 && sk->sk_err == 0)
		goto eagain;
	if (size == 0)
		size = 1;	/* iovlen cannot be zero */

	if (unlikely(!(mp = mi_allocb(q, plen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!(dp = mp->b_cont = mi_allocb(q, dlen + size, BPRI_MED))))
		goto enobufs;

	msg = ss_setup_message(dp, size, type);
	msg->msg_flags |= MSG_ERRQUEUE;

	if (unlikely((err = ss_recvmsg(ss, msg, size)) < 0)) {
		switch (__builtin_expect(-err, EAGAIN)) {
		case EAGAIN:
			LOGIO(ss, "no data to read!");
			goto error;
		case ENOBUFS:
		case ENOMEM:
			mi_bufcall(q, size, BPRI_MED);
			goto error;
		case EDEADLK:
		case EBUSY:
			qenable(q);
			goto error;
		}
		LOGERR(ss, "Error %d: %s : %s:%d", -err, __FUNCTION__, __FILE__, __LINE__);
		goto error;
	}

	/* leave zero length data blocks for now */
	dp->b_wptr = dp->b_rptr + err;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = msg->msg_namelen;
	p->DEST_offset = msg->msg_namelen ? sizeof(*p) : 0;
	p->OPT_length = ss_opts_size(ss, msg);
	p->OPT_offset = p->OPT_length ? sizeof(*p) + msg->msg_namelen : 0;
	p->ERROR_type = 0;	/* filled in later */
	mp->b_wptr += sizeof(*p);
	if (msg->msg_namelen) {
		bcopy(msg->msg_name, mp->b_wptr, msg->msg_namelen);
		mp->b_wptr += msg->msg_namelen;
	}
	if (p->OPT_length) {
		ss_opts_build(ss, msg, mp->b_wptr, p->OPT_length, &p->ERROR_type);
		mp->b_wptr += p->OPT_length;
	}
	LOGTX(ss, "<- T_UDERROR_IND");
	putnext(ss->rq, mp);
	return (0);

      eagain:
	err = -EAGAIN;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * ss_all_error_ind: - deliver all error indications
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: underlying socket
 * @type: socket type
 *
 * Read and deliver T_EXDATA_IND and T_DATA_IND while the receive qeuue can be read and primitives
 * can be delivered upstream.
 */
noinline fastcall __hot void
ss_all_error_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
#if 0
	int err = 0;

	if (unlikely((err = ss->lasterror) || (err = ss->lasterror = sock_error(sk)))) {
		/* Process disconnect-related errors as disconnects. */
		/* Note that the reason will be picked from ss->lasterror. */
		if (unlikely((err = t_discon_ind(ss, q, sk, 0, NULL)) < 0)) {
			switch (-err) {
			case EBUSY:
			case ENOBUFS:
			case ENOMEM:
			case EAGAIN:
			case EDEADLK:
				/* regenerate error report */
				set_bit(SS_BIT_ERROR_REPORT, &ss->ss_rflags);
				break;
			}
		}
		/* clear error */
		ss->lasterror = 0;
	}
#else
	if (ss->lasterror == 0)
		ss->lasterror = sock_error(sk);
#endif
}
/**
 * ss_all_data_ind: - deliver all data indications
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: underlying socket
 * @type: socket type
 *
 * Read and deliver T_EXDATA_IND and T_DATA_IND while the receive qeuue can be read and primitives
 * can be delivered upstream.
 */
noinline fastcall __hot void
ss_all_data_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int err = 0;

	do {
		if (unlikely((err = ss_exdata_ind(ss, q, sk, type)) <= 0)) {
			switch (-err) {
			case EBUSY:
			case ENOBUFS:
			case ENOMEM:
			case EDEADLK:
				/* deferred, mark data read service */
				set_bit(SS_BIT_DATA_READY, &ss->ss_rflags);
			case EAGAIN:
			case EINVAL:
				/* note that EAGAIN or EINVAL means that there was nothing to read */
				break;
			case EPIPE:
			case ENETDOWN:
			case EHOSTUNREACH:
			case ECONNRESET:
			case ECONNREFUSED:
			case ETIMEDOUT:
			case ENOTCONN:
				/* These errors indicate a disconnection. */
				ss->lasterror = -err;
				break;
			}
		}
	} while (err > 0);
	do {
		if (unlikely((err = ss_data_ind(ss, q, sk, type)) <= 0)) {
			switch (-err) {
			case EBUSY:
			case ENOBUFS:
			case ENOMEM:
			case EDEADLK:
				/* deferred, mark data read service */
				set_bit(SS_BIT_DATA_READY, &ss->ss_rflags);
			case EAGAIN:
				/* note that EAGAIN or zero(0) means that there was nothing to read 
				 */
				break;
			case EPIPE:
			case ENETDOWN:
			case EHOSTUNREACH:
			case ECONNRESET:
			case ECONNREFUSED:
			case ETIMEDOUT:
			case ENOTCONN:
				/* These errors indicate a disconnection. */
				ss->lasterror = -err;
				break;
			}
		}
	} while (err > 0);
}

/**
 * ss_all_uderror_ind: - deliver all error indications
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: underlying socket
 * @type: socket type
 *
 * Read and deliver T_UDERROR_IND while the error queue can be read and primitives can be delivered
 * upstream.
 */
noinline fastcall __hot void
ss_all_uderror_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int err = 0;

	do {
		if (unlikely((err = ss_uderror_ind(ss, q, sk, type)) < 0)) {
			switch (-err) {
			case EBUSY:
			case ENOBUFS:
			case ENOMEM:
			case EDEADLK:
				/* deferred, mark error report service */
				set_bit(SS_BIT_ERROR_REPORT, &ss->ss_rflags);
			case EAGAIN:
				/* note that here EAGAIN means that there was nothing to read */
				break;
			}
		}
	} while (err >= 0);
}

/**
 * ss_all_unitdata_ind: - deliver all data indications
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: underlying socket
 * @type: socket type
 *
 * Read and deliver T_UNITDATA_IND while the receive queue can be read and primitives can be
 * delivered upstream.
 */
noinline fastcall __hot void
ss_all_unitdata_ind(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int err = 0;

	do {
		if (unlikely((err = ss_unitdata_ind(ss, q, sk, type)) < 0)) {
			switch (-err) {
			case EBUSY:
			case ENOBUFS:
			case ENOMEM:
			case EDEADLK:
				/* deferred, mark data ready service */
				set_bit(SS_BIT_DATA_READY, &ss->ss_rflags);
			case EAGAIN:
				/* note that EAGAIN means that there was nothing to read */
				break;
			}
			break;
		}
	} while (err >= 0);
}

/*
 *  =========================================================================
 *
 *  Socket Callbacks
 *
 *  =========================================================================
 */

/**
 * ss_state_change: - state change
 * @sk: socket with state change
 *
 * Simply enable the read queue and chain the callback.  Note that if a state change is reported on
 * an open request child, then the read queue of the parent (listening Stream) will be enabled.
 */
static __hot void
ss_state_change(struct sock *sk)
{
	ss_conind_t *ci;

	if (likely(!!(ci = SOCK_PRIV(sk)))) {
		ss_t *ss;

		if (likely(!!(ss = ci->ci_parent))) {
			/* protect dereference of ss->rq */
			read_lock(&ss_lock);
			{
				if (likely(!!ss->rq)) {
					if (!test_and_set_bit(SS_BIT_STATE_CHANGE, &ss->ss_rflags))
						qenable(ss->rq);
					set_bit(SS_BIT_STATE_CHANGE, &ci->ci_rflags);
					if ((void *)ci != (void *)ss)
						LOGST(ss, "state change child %p -> %s", (void *)sk, tcp_statename(sk->sk_state));
					else
						LOGST(ss, "state change sock  %p -> %s", (void *)sk, tcp_statename(sk->sk_state));
				} else
					assure(ss->rq);
			}
			read_unlock(&ss_lock);
			if (ss->cb_save.sk_state_change) {
				assert(ss->cb_save.sk_state_change != ss_state_change);
				ss->cb_save.sk_state_change(sk);
			}
		} else
			assure(ss);
	} else
		assure(ci);
}

/**
 * ss_write_space: - write available
 * @sk: socket with write space
 *
 * Simply enable the write queue and chain the callback.  Note that if write space is reported on an
 * open request child, then the write queue of the parent (listening Stream) will be enabled.
 */
static __hot_out void
ss_write_space(struct sock *sk)
{
	ss_conind_t *ci;

	if (likely(!!(ci = SOCK_PRIV(sk)))) {
		ss_t *ss;

		if (likely(!!(ss = ci->ci_parent))) {
			/* protect dereference of ss->wq */
			read_lock(&ss_lock);
			{
				if (likely(!!ss->wq)) {
					if (!test_and_set_bit(SS_BIT_WRITE_SPACE, &ss->ss_wflags))
						qenable(ss->wq);
					set_bit(SS_BIT_WRITE_SPACE, &ci->ci_wflags);
					if ((void *)ci != (void *)ss)
						LOGDA(ss, "write space child %p", (void *)sk);
					else
						LOGDA(ss, "write space sock  %p", (void *)sk);
				} else
					assure(ss->wq);
			}
			read_unlock(&ss_lock);
			if (ss->cb_save.sk_write_space) {
				assert(ss->cb_save.sk_write_space != ss_write_space);
				ss->cb_save.sk_write_space(sk);
			}
		} else
			assure(ss);
	} else
		assure(ci);
}

/**
 * ss_error_report: - error available
 * @sk: socket with error available
 *
 * Simply enable the read queue and chain the callback.  Note that if error report is reported on an
 * open request qchild, then the read queue of the parent (listening Stream) will be enabled.
 */
static __hot_get void
ss_error_report(struct sock *sk)
{
	ss_conind_t *ci;

	if (likely(!!(ci = SOCK_PRIV(sk)))) {
		ss_t *ss;

		if (likely(!!(ss = ci->ci_parent))) {
			/* protect dereference of ss->rq */
			read_lock(&ss_lock);
			{
				if (likely(!!ss->rq)) {
					if (!test_and_set_bit(SS_BIT_ERROR_REPORT, &ss->ss_rflags))
						qenable(ss->rq);
					set_bit(SS_BIT_ERROR_REPORT, &ci->ci_rflags);
					if ((void *)ci != (void *)ss)
						LOGNO(ss, "error report child %p", (void *)sk);
					else
						LOGNO(ss, "error report sock  %p", (void *)sk);
				} else
					assure(ss->rq);
			}
			read_unlock(&ss_lock);
			if (ss->cb_save.sk_error_report) {
				assert(ss->cb_save.sk_error_report != ss_error_report);
				ss->cb_save.sk_error_report(sk);
			}
		} else
			assure(ss);
	} else
		assure(ci);
}

/**
 * ss_data_ready: - data available
 * @sk: socket with data available
 * @len: ignored
 *
 * Simply enable the read queue and chain the callback.  Note that if data ready is reported on an
 * open request child, then the read queue of the parent (listening stream) will be enabled.
 */
static __hot_in void
ss_data_ready(struct sock *sk, int len)
{
	ss_conind_t *ci;

	if (likely(!!(ci = SOCK_PRIV(sk)))) {
		ss_t *ss;

		if (likely(!!(ss = ci->ci_parent))) {
			/* protect dereference of ss->rq */
			read_lock(&ss_lock);
			{
				if (likely(!!ss->rq)) {
					if (!test_and_set_bit(SS_BIT_DATA_READY, &ss->ss_rflags))
						qenable(ss->rq);
					set_bit(SS_BIT_DATA_READY, &ci->ci_rflags);
					if ((void *)ci != (void *)ss)
						LOGDA(ss, "data ready child %p", (void *)sk);
					else
						LOGDA(ss, "data ready sock  %p", (void *)sk);
				} else
					assure(ss->rq);
			}
			read_unlock(&ss_lock);
			if (ss->cb_save.sk_data_ready) {
				assert(ss->cb_save.sk_data_ready != ss_data_ready);
				ss->cb_save.sk_data_ready(sk, len);
			}
		} else
			assure(ss);
	} else
		assure(ci);
}

/*
 *  =========================================================================
 *
 *  IP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 */

/**
 * t_conn_req: - process T_CONN_REQ primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @mp: the T_CONN_REQ primtive
 */
static fastcall int
t_conn_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	int err;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr *dst;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely(ss_get_state(ss) != TS_IDLE))
		goto outstate;
	if ((p->DEST_length && mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto einval;

	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);

	if (unlikely(p->DEST_length < ss_addr_size(dst)))
		goto badaddr;
	if (unlikely(dst->sa_family != ss->p.prot.family))
		goto badaddr;
	switch (__builtin_expect(ss->p.prot.family, AF_INET)) {
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
	if (unlikely
	    ((err = t_parse_conn_opts(ss, mp->b_rptr + p->OPT_offset, p->OPT_length, 1)) < 0)) {
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
	if (unlikely(!!mp->b_cont)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.CDATA_size) > 0 && mlen > mmax)
		    || mmax == T_INVALID || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax)
		    || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate connect with data */
	}
	ss_set_state(ss, TS_WACK_CREQ);
	return t_ok_ack(ss, q, mp, T_CONN_REQ, NULL, NULL);

      baddata:
	err = TBADDATA;
	LOGNO(ss, "ERROR: bad connection data");
	goto error;
      badopt:
	err = TBADOPT;
	LOGNO(ss, "ERROR: bad options");
	goto error;
      acces:
	err = TACCES;
	LOGNO(ss, "ERROR: no permission for address or option");
	goto error;
      badaddr:
	err = TBADADDR;
	LOGNO(ss, "ERROR: address is unusable");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(ss, "ERROR: invalid message format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(ss, q, mp, T_CONN_REQ, err);
}

/**
 * t_seq_check: - find a connection indication by sequence number
 * @ss: private structure (locked)
 * @seq: the sequence number to find
 *
 * Takes a sequence number and matches it to a connetion indication.  Note that it is not necessary
 * to separately lock the connection indication queue because it is only accessed with the private
 * structure locked.
 */
static mblk_t *
t_seq_check(ss_t *ss, t_uscalar_t seq)
{
	mblk_t *cp;

	for (cp = bufq_head(&ss->conq); cp && ((ss_conind_t *) cp->b_rptr)->ci_seq != seq;
	     cp = cp->b_next) ;
	usual(cp);
	return (cp);
}

/**
 * t_tok_check: - find an accepting stream by acceptor id
 * @ss: private structure (locked)
 * @q: active queue
 * @tok: the acceptor id
 * @errp: error return pointer
 *
 * Finds the accepting stream that matches the acceptor id and returns NULL if no match is found.
 * If a match is found and it is different than the stream on which the connection response was
 * received, the accepting stream is locked.  If the accepting stream cannot be locked, NULL is
 * returned and -EDEADLK is returned in the error pointer.
 */
static fastcall ss_t *
t_tok_check(ss_t *ss, queue_t *q, t_uscalar_t tok, int *errp)
{
	ss_t *as;

	read_lock(&ss_lock);
	for (as = (ss_t *) mi_first_ptr(&ss_opens); as && (t_uscalar_t) (long) as->rq != tok;
	     as = (ss_t *) mi_next_ptr((caddr_t) as)) ;
	if (as && as != ss)
		if (!mi_acquire((caddr_t) as, q))
			*errp = -EDEADLK;
	read_unlock(&ss_lock);
	usual(as);
	return (as);
}

/**
 * t_conn_res: - process a T_CONN_RES primitive
 * @ss: private structure (locked) of listening Stream
 * @q: active queue
 * @mp: the T_CONN_RES primitive
 *
 * Note that when the accepting Stream is different from the listening Stream, it is necessary to
 * acquire a lock also on the accepting Stream.  It is necessary to release this lock after
 * t_ok_ack() but may be released before t_error_ack().
 */
static fastcall int
t_conn_res(ss_t *ss, queue_t *q, mblk_t *mp)
{
	int err = 0;
	mblk_t *cp;
	ss_t *as;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	unsigned char *opt;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto inval;
	if (unlikely(ss_get_state(ss) != TS_WRES_CIND))
		goto outstate;
	ss_set_state(ss, TS_WACK_CRES);
	if (unlikely(!(cp = t_seq_check(ss, p->SEQ_number))))
		goto badseq;
	if (unlikely(!(as = t_tok_check(ss, q, p->ACCEPTOR_id, &err)) && err))
		goto edeadlk;
	if (unlikely(!as || !(as == ss || ((1 << ss_get_state(as)) & TSM_DISCONN))))
		goto badf;
	if (unlikely(ss->p.prot.protocol != as->p.prot.protocol))
		goto provmismatch;
	if (unlikely(ss_get_state(as) == TS_IDLE && as->conind))
		goto resqlen;
	if (unlikely(ss->cred.cr_uid != 0 && as->cred.cr_uid == 0))
		goto acces;
	opt = mp->b_rptr + p->OPT_offset;
	if (unlikely(p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
		goto badopt;
	if (unlikely(mp->b_cont != NULL)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.CDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate connect with data */
	}
	if (unlikely((err = t_parse_conn_opts(as, opt, p->OPT_length, 0)) < 0)) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto unlock_error;
		}
	}
	/* FIXME: options will be processed on wrong socket!!! When we accept, we will delete this
	   socket and create another one that was derived from the listening socket.  We need to
	   process the options against that socket, not the placeholder.  One way to fix this is to 
	   set flags when we process options against the stream structure and then reprocess those
	   options once the sockets have been swapped.  See t_ok_ack for details. */
	/* FIXME: The accepting socket does not have to be in the bound state. The socket will be
	   autobound to the correct address already. */
	err = t_ok_ack(ss, q, mp, T_CONN_RES, cp, as);
	if (as && as != ss) {
		read_lock(&ss_lock);
		mi_release((caddr_t) as);
		read_unlock(&ss_lock);
	}
	return (err);
      baddata:
	err = TBADDATA;
	LOGNO(ss, "ERROR: bad connection data");
	goto unlock_error;
      badopt:
	err = TBADOPT;
	LOGNO(ss, "ERROR: bad options");
	goto unlock_error;
      acces:
	err = TACCES;
	LOGNO(ss, "ERROR: no access to accepting queue");
	goto unlock_error;
      resqlen:
	err = TRESQLEN;
	LOGNO(ss, "ERROR: accepting queue is listening");
	goto unlock_error;
      provmismatch:
	err = TPROVMISMATCH;
	LOGNO(ss, "ERROR: not same transport provider");
	goto unlock_error;
      badf:
	err = TBADF;
	LOGNO(ss, "ERROR: accepting queue id is invalid");
	goto unlock_error;
      edeadlk:
	err = -EDEADLK;
	LOGNO(ss, "ERROR: cannot lock accepting stream");
	goto error;
      badseq:
	err = TBADSEQ;
	LOGNO(ss, "ERROR: sequence number is invalid");
	goto error;
      inval:
	err = -EINVAL;
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      unlock_error:
	if (as && as != ss) {
		read_lock(&ss_lock);
		mi_release((caddr_t) as);
		read_unlock(&ss_lock);
	}
	goto error;
      error:
	return t_error_ack(ss, q, mp, T_CONN_RES, err);
}

/*
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_discon_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely((1 << ss_get_state(ss)) & ~TSM_CONNECTED))
		goto outstate;
	if (unlikely(!!mp->b_cont)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate disconnect with data */
	}
	switch (__builtin_expect(ss_get_state(ss), TS_DATA_XFER)) {
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
	return t_ok_ack(ss, q, mp, T_DISCON_REQ, cp, NULL);

      badseq:
	err = TBADSEQ;
	LOGNO(ss, "ERROR: sequence number is invalid");
	goto error;
      baddata:
	err = TBADDATA;
	LOGNO(ss, "ERROR: bad disconnection data");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(ss, q, mp, T_DISCON_REQ, err);
}

/*
 *  M_DATA
 *  -------------------------------------------------------------------
 */
static inline fastcall __hot_write int
t_write(ss_t *ss, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	struct msghdr msg;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(ss_get_state(ss) == TS_IDLE))
		goto discard;
	if (unlikely((1 << ss_get_state(ss)) & ~TSM_OUTDATA))
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
	LOGNO(ss, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      outstate:
	LOGNO(ss, "ERROR: would place i/f out of staten");
	goto error;
      discard:
	freemsg(mp);
	LOGNO(ss, "ERROR: ignore in idle state");
	return (0);
      notsupport:
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(ss, q, mp, -EPROTO);
}

/*
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 */
static inline fastcall __hot_out int
t_data_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	struct msghdr msg;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely(ss_get_state(ss) == TS_IDLE))
		goto discard;
	if (unlikely((1 << ss_get_state(ss)) & ~TSM_OUTDATA))
		goto outstate;
	if (!(mlen = msgdsize(mp)) &&!(ss->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (((mmax = ss->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = (ss->p.prot.type == SOCK_SEQPACKET
			 && !(p->MORE_flag & T_MORE)) ? MSG_EOR : 0;
	return ss_sock_sendmsg(ss, mp, &msg);

      emsgsize:
	LOGNO(ss, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      baddata:
	LOGNO(ss, "ERROR: zero length data");
	goto error;
      outstate:
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      einval:
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      discard:
	LOGNO(ss, "ERROR: ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(ss, q, mp, -EPROTO);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 */
static inline fastcall __hot_put int
t_exdata_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	const struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	struct msghdr msg;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely(ss_get_state(ss) == TS_IDLE))
		goto discard;
	if (unlikely((1 << ss_get_state(ss)) & ~TSM_OUTDATA))
		goto outstate;
	if (!(mlen = msgdsize(mp)) &&!(ss->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (((mmax = ss->p.info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = MSG_OOB |
	    ((ss->p.prot.type == SOCK_SEQPACKET && !(p->MORE_flag & T_MORE)) ? MSG_EOR : 0);
	return ss_sock_sendmsg(ss, mp, &msg);

      emsgsize:
	LOGNO(ss, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      baddata:
	LOGNO(ss, "ERROR: zero length data");
	goto error;
      outstate:
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      einval:
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      discard:
	LOGNO(ss, "ERROR: ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(ss, q, mp, -EPROTO);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_info_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	return t_info_ack(ss, q, mp);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
static fastcall int
t_bind_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	int err, add_len;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr *add = &ss->src;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely(ss_get_state(ss) != TS_UNBND))
		goto outstate;
	ss_set_state(ss, TS_WACK_BREQ);
#if 0
	/* we are supposed to ignore CONIND_number for T_CLTS according to XTI */
	if (ss->p.info.SERV_type == T_CLTS && p->CONIND_number)
		goto notsupport;
#endif
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)) {
		LOGNO(ss, "ADDR_offset(%u) or ADDR_length(%u) are incorrect", p->ADDR_offset,
			 p->ADDR_length);
		goto badaddr;
	}
	switch (__builtin_expect(ss->p.prot.family, AF_INET)) {
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
				LOGNO(ss, "add_len(%d) < sizeof(struct sockaddr_in)(%lu)",
					 add_len, (ulong) sizeof(struct sockaddr_in));
				goto badaddr;
			}
			if (ss->p.prot.protocol != T_INET_SCTP) {
				if (add_len > sizeof(struct sockaddr_in)) {
					LOGNO(ss,
						 "add_len(%d) > sizeof(struct sockaddr_in)(%lu)",
						 add_len, (ulong) sizeof(struct sockaddr_in));
					goto badaddr;
				}
			} else {
				if ((add_len % sizeof(struct sockaddr_in)) != 0) {
					LOGNO(ss,
						 "add_len(%d) %% sizeof(struct sockaddr_in)(%lu)",
						 add_len, (ulong) sizeof(struct sockaddr_in));
					goto badaddr;
				}
			}
			if (add->sa_family != AF_INET && add->sa_family != 0) {
				LOGNO(ss, "sa_family incorrect (%u)", add->sa_family);
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
				LOGNO(ss, "add_len(%d) < sizeof(struct sockaddr_un)(%lu)",
					 add_len, (ulong) sizeof(struct sockaddr_un));
				goto badaddr;
			}
			if (add->sa_family != AF_UNIX && add->sa_family != 0) {
				LOGNO(ss, "sa_family incorrect (%u)", add->sa_family);
				goto badaddr;
			}
			add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		}
		add_un = (typeof(add_un)) add;
		ss->port = 0;
		break;
	}
	default:
		LOGNO(ss, "protocol family incorrect %u", ss->p.prot.family);
		goto badaddr;
	}
	if (unlikely((err = ss_bind(ss, add, add_len))))
		goto error_close;
	ss->conind = 0;
	if (p->CONIND_number
	    && (ss->p.prot.type == SOCK_STREAM || ss->p.prot.type == SOCK_SEQPACKET))
		if ((err = ss_listen(ss, p->CONIND_number)))
			goto error_close;
	return t_bind_ack(ss, q, mp, (ss_getsockname(ss) <= 0) ? NULL : &ss->src, p->CONIND_number);

      acces:
	err = TACCES;
	LOGNO(ss, "ERROR: no permission for address");
	goto error;
      noaddr:
	err = TNOADDR;
	LOGNO(ss, "ERROR: address could not be assigned");
	goto error;
      badaddr:
	err = TBADADDR;
	LOGNO(ss, "ERROR: address is invalid");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
#if 0
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
#endif
      error_close:
	ss_socket_put(&ss->sock);
      error:
	return t_error_ack(ss, q, mp, T_BIND_REQ, err);
}

/**
 * t_unbind_req: - process a T_UNBIND_REQ primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @mp: the T_UNBIND_REQ primitive
 *
 * This procedure unbinds a TS user from a transport address.
 */
static fastcall int
t_unbind_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	if (unlikely(ss_get_state(ss) != TS_IDLE))
		goto outstate;

	ss_set_state(ss, TS_WACK_UREQ);
	return t_ok_ack(ss, q, mp, T_UNBIND_REQ, NULL, NULL);

      outstate:
	LOGNO(ss, "ERROR: would place i/f out of state");
	return t_error_ack(ss, q, mp, T_UNBIND_REQ, TOUTSTATE);
}

/*
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 */
static inline fastcall __hot_out int
t_unitdata_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	long mmax;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(ss->p.info.SERV_type != T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely(dlen == 0 && !(ss->p.info.PROVIDER_flag & T_SNDZERO)))
		goto baddata;
	if (((mmax = ss->p.info.TSDU_size) > 0 && dlen > mmax) || mmax == T_INVALID
	    || ((mmax = ss->p.info.TIDU_size) > 0 && dlen > mmax) || mmax == T_INVALID)
		goto baddata;
	if (unlikely(ss_get_state(ss) != TS_IDLE))
		goto outstate;
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->DEST_length < sizeof(ss->dst.sa_family))
	    || (p->DEST_length < ss_addr_size((struct sockaddr *) (mp->b_rptr + p->DEST_length))))
		goto badadd;
	if (unlikely(ss->p.prot.type == SOCK_RAW && ss->cred.cr_uid != 0))
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
			msg.msg_namelen = ss_addr_size(dst);
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
	LOGNO(ss, "ERROR: bad options");
	goto error;
      acces:
	LOGNO(ss, "ERROR: no permission to address or options");
	goto error;
      badadd:
	LOGNO(ss, "ERROR: bad destination address");
	goto error;
      einval:
	LOGNO(ss, "ERROR: invalid primitive");
	goto error;
      outstate:
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      baddata:
	LOGNO(ss, "ERROR: bad data size");
	goto error;
      notsupport:
	LOGNO(ss, "ERROR: primitive not supported for T_COTS");
	goto error;
      error:
	return m_error(ss, q, mp, -EPROTO);
}

/**
 * t_optmgmt_req: - process T_OPTMGMT_REQ primitive
 * @ss: private structure (locked)
 * @q: active queue
 * @mp: the T_OPTMGMT_REQ message
 *
 * The T_OPTMGMT_REQ is responsible for establishing options while the stream is in the T_IDLE
 * state.  When the stream is bound to a local address using the T_BIND_REQ, the settings of options
 * with end-to-end significance will have an affect on how the driver response to an INIT with
 * INIT-ACK for SCTP.  For example, the bound list of addresses is the list of addresses that will
 * be sent in the INIT-ACK.  The number of inbound streams and outbound streams are the numbers that
 * will be used in the INIT-ACK.
 *
 * Returned Errors:
 *
 * TACCES:
 *	the user did not have proper permissions for the user of the requested options.
 * TBADFLAG:
 *	the flags as specified were incorrect or invalid.
 * TBADOPT:
 *	the options as specified were in an incorrect format, or they contained invalid information.
 * TOUTSTATE:
 *	the primitive would place the transport interface out of state.
 * TNOTSUPPORT:
 *	this prmitive is not supported.
 * TSYSERR:
 *	a system error has occured and the UNIX system error is indicated in the primitive.
 */
static inline fastcall __hot_put int
t_optmgmt_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	int err, opt_len;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
#ifdef TS_WACK_OPTREQ
	if (unlikely(ss_get_state(ss) == TS_IDLE))
		ss_set_state(ss, TS_WACK_OPTREQ);
#endif
	if (unlikely(p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length))
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
	if (unlikely(opt_len < 0)) {
		switch (-(err = opt_len)) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto provspec;
		}
	}
	err = t_optmgmt_ack(ss, q, mp, p->MGMT_flags, mp->b_rptr + p->OPT_offset, p->OPT_length,
			    opt_len);
	if (unlikely(err < 0)) {
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
	LOGNO(ss, "ERROR: provider specific");
	goto error;
      badflag:
	err = TBADFLAG;
	LOGNO(ss, "ERROR: bad options flags");
	goto error;
      acces:
	err = TACCES;
	LOGNO(ss, "ERROR: no permission for option");
	goto error;
      badopt:
	err = TBADOPT;
	LOGNO(ss, "ERROR: bad options");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      error:
	return t_error_ack(ss, q, mp, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
static fastcall int
t_ordrel_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(ss->p.info.SERV_type != T_COTS_ORD))
		goto notsupport;
	if (unlikely((1 << ss_get_state(ss)) & ~TSM_OUTDATA))
		goto outstate;
	if (unlikely(!!mp->b_cont)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = ss->p.info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate orderly release with data */
	}
	ss->sock->ops->shutdown(ss->sock, SEND_SHUTDOWN - 1);	/* XXX */
	switch (__builtin_expect(ss_get_state(ss), TS_DATA_XFER)) {
	case TS_DATA_XFER:
		ss_set_state(ss, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		ss_set_state(ss, TS_IDLE);
		break;
	}
	freemsg(mp);
	return (0);

      baddata:
	LOGNO(ss, "ERROR: bad orderly release data");
	goto error;
      outstate:
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS or T_COTS");
	goto error;
      error:
	ss_set_state(ss, ss->oldstate);
	return m_error(ss, q, mp, -EPROTO);
}

/*
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 */
static inline fastcall __hot_out int
t_optdata_req(ss_t *ss, queue_t *q, mblk_t *mp)

{
	long mlen, mmax;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(ss->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	if (unlikely(ss_get_state(ss) == TS_IDLE))
		goto discard;
	if (unlikely((1 << ss_get_state(ss)) & ~TSM_OUTDATA))
		goto outstate;
	if (!(mlen = msgdsize(mp)) &&!(ss->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (likely(!(p->DATA_flag & T_ODF_EX))) {
		if (((mmax = ss->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto emsgsize;
	} else {
		if (((mmax = ss->p.info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = ss->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto emsgsize;
	}
	{
		int cmsg_len;
		size_t opt_len = p->OPT_length;
		size_t opt_off = p->OPT_offset;
		unsigned char *opt = mp->b_rptr + opt_off;

		if (likely((cmsg_len = ss_cmsg_size(ss, opt, opt_len)) >= 0)) {
			struct msghdr msg;
			unsigned char cmsg[cmsg_len];

			msg.msg_name = NULL;
			msg.msg_namelen = 0;
			msg.msg_control = &cmsg;
			msg.msg_controllen = cmsg_len;
			msg.msg_flags = MSG_NOSIGNAL | MSG_DONTWAIT;
			if (unlikely(ss_cmsg_build(ss, opt, opt_len, &msg) < 0))
				goto badopt;
			if (unlikely((p->DATA_flag & T_ODF_EX)))
				msg.msg_flags |= MSG_OOB;
			if (unlikely(!(p->DATA_flag & T_ODF_MORE)
				     && ss->p.prot.type == SOCK_SEQPACKET))
				msg.msg_flags |= MSG_EOR;
			return ss_sock_sendmsg(ss, mp, &msg);
		}
		switch (-cmsg_len) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		}
		goto badopt;
	}
      badopt:
	LOGNO(ss, "ERROR: bad options");
	goto error;
      acces:
	LOGNO(ss, "ERROR: no permission to options");
	goto error;
      emsgsize:
	LOGNO(ss, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      baddata:
	LOGNO(ss, "ERROR: zero length data");
	goto error;
      outstate:
	LOGNO(ss, "ERROR: would place i/f out of state");
	goto error;
      einval:
	LOGNO(ss, "ERROR: invalid primitive format");
	goto error;
      discard:
	LOGNO(ss, "ERROR: ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	LOGNO(ss, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(ss, q, mp, -EPROTO);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_addr_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
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
	return t_addr_ack(ss, q, mp, loc, rem);
      outstate:
	return t_error_ack(ss, q, mp, T_ADDR_REQ, TOUTSTATE);
}
#endif
#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_capability_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto einval;
	(void) ss;
	return t_capability_ack(ss, q, mp, p->CAP_bits1, mp->b_datap->db_type);

      einval:
	err = -EINVAL;
	LOGNO(ss, "ERROR: invalid message format");
	goto error;
      error:
	return t_error_ack(ss, q, mp, T_CAPABILITY_REQ, err);
}
#endif
/*
 *  Other primitives    XX - other invalid primitives
 *  -------------------------------------------------------------------------
 */
static int
t_other_req(ss_t *ss, queue_t *q, mblk_t *mp)
{
	t_scalar_t prim = *((t_scalar_t *) mp->b_rptr);

	return t_error_ack(ss, q, mp, prim, TNOTSUPPORT);
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
/**
 * ss_w_proto_return: - handle M_PROTO and M_PCPROTO processing return value
 * @mp: the M_PROTO or M_PCPROTO message
 * @rtn: the return value
 *
 * Some M_PROTO and M_PCPROTO processing functions return an error just to cause the primary state
 * of the endpoint to be restored.  These return values are processed here.
 */
noinline fastcall int
ss_w_proto_return(mblk_t *mp, int rtn)
{
	switch (-rtn) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (rtn);
	default:
		freemsg(mp);
	case 0:
		return (0);
	}
}

/**
 * __ss_w_proto_slow: process M_PROTO or M_PCPROTO primitive
 * @ss: private structure (locked)
 * @q: the active queue
 * @mp: the M_PROTO or M_PCPROTO primitive
 * @prim: primitive
 *
 * This function either returns zero (0) when the message has been consumed, or a negative error
 * number when the message is to be (re)queued.  This function must be called with the private
 * structure locked.
 */
noinline fastcall int
__ss_w_proto_slow(ss_t *ss, queue_t *q, mblk_t *mp, t_scalar_t prim)
{
	int rtn;

	ss->oldstate = ss_get_state(ss);

	switch (prim) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_UNITDATA_REQ:
	case T_OPTDATA_REQ:
		LOGDA(ss, "-> %s", tpi_primname(prim));
		break;
	default:
		LOGRX(ss, "-> %s", tpi_primname(prim));
		break;
	}
	switch (prim) {
	case T_CONN_REQ:
		rtn = t_conn_req(ss, q, mp);
		break;
	case T_CONN_RES:
		rtn = t_conn_res(ss, q, mp);
		break;
	case T_DISCON_REQ:
		rtn = t_discon_req(ss, q, mp);
		break;
	case T_DATA_REQ:
		rtn = t_data_req(ss, q, mp);
		break;
	case T_EXDATA_REQ:
		rtn = t_exdata_req(ss, q, mp);
		break;
	case T_INFO_REQ:
		rtn = t_info_req(ss, q, mp);
		break;
	case T_BIND_REQ:
		rtn = t_bind_req(ss, q, mp);
		break;
	case T_UNBIND_REQ:
		rtn = t_unbind_req(ss, q, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = t_optmgmt_req(ss, q, mp);
		break;
	case T_UNITDATA_REQ:
		rtn = t_unitdata_req(ss, q, mp);
		break;
	case T_ORDREL_REQ:
		rtn = t_ordrel_req(ss, q, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = t_optdata_req(ss, q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		rtn = t_addr_req(ss, q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		rtn = t_capability_req(ss, q, mp);
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
		LOGRX(ss, "%s() replying with error %d", __FUNCTION__, -EPROTO);
		rtn = m_error(ss, q, mp, -EPROTO);
		break;
	default:
		rtn = t_other_req(ss, q, mp);
		break;
	}
	if (rtn < 0)
		ss_set_state(ss, ss->oldstate);
	/* The put and srv procedures do not recognize all errors. Sometimes we return an error to
	   here just to restore the previous state. */
	return ss_w_proto_return(mp, rtn);
}

noinline fastcall int
ss_w_proto_slow(queue_t *q, mblk_t *mp, t_scalar_t prim)
{
	ss_t *ss;
	int err;

	if (likely(!!(ss = ss_trylock(q)))) {
		err = __ss_w_proto_slow(ss, q, mp, prim);
		ss_unlock(ss);
	} else
		err = -EDEADLK;
	return (err);

}

/**
 * __ss_w_proto: - process M_PROTO or M_PCPROTO message locked
 * @ss: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locked version is for use by the service procedure that takes private structure locks once
 * for the entire service procedure run.
 */
static inline fastcall __hot_write int
__ss_w_proto(ss_t *ss, queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
		t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
		int rtn;

		ss->oldstate = ss_get_state(ss);
		if (likely(prim == T_DATA_REQ)) {
			if (likely((rtn = t_data_req(ss, q, mp)) == 0))
				return (0);
		} else if (likely(prim == T_UNITDATA_REQ)) {
			if (likely((rtn = t_unitdata_req(ss, q, mp)) == 0))
				return (0);
		} else if (likely(prim == T_OPTDATA_REQ)) {
			if (likely((rtn = t_optdata_req(ss, q, mp)) == 0))
				return (0);
		} else if (likely(prim == T_EXDATA_REQ)) {
			if (likely((rtn = t_exdata_req(ss, q, mp)) == 0))
				return (0);
		} else
			return __ss_w_proto_slow(ss, q, mp, prim);
		if (rtn < 0)
			ss_set_state(ss, ss->oldstate);
		return ss_w_proto_return(mp, rtn);
	}
	LOGRX(ss, "%s() replying with error %d", __FUNCTION__, -EPROTO);
	return m_error(ss, q, mp, -EPROTO);
}

/**
 * ss_w_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locking version is for use by the put procedure which does not take locks.  For data
 * messages we simply return (-EAGAIN) for performance.
 */
static inline fastcall __hot_write int
ss_w_proto(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
		t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;

		if (likely(prim == T_DATA_REQ)) {
		} else if (likely(prim == T_UNITDATA_REQ)) {
		} else if (likely(prim == T_OPTDATA_REQ)) {
		} else if (likely(prim == T_EXDATA_REQ)) {
		} else
			return ss_w_proto_slow(q, mp, prim);
	}
	return (-EAGAIN);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
/**
 * __ss_w_data: - process M_DATA message
 * @ss: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 *
 * This function either returns zero (0) when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This non-locking version is used by the service procedure.
 */
static inline fastcall __hot_write int
__ss_w_data(ss_t *ss, queue_t *q, mblk_t *mp)
{
	return t_write(ss, q, mp);
}

/**
 * ss_w_data: - process M_DATA  messages
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 *
 * This function either returns zero (0) when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This locking version is used by the put procedure.
 */
static inline fastcall __hot_write int
ss_w_data(queue_t *q, mblk_t *mp)
{
	/* Always queue from put procedure for performance. */
	return (-EAGAIN);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * ss_w_flush: - canonical driver write flush procedure
 * @q: active queue (write queue)
 * @mp: the flush message
 *
 * This canonical driver write flush procedure flushes the write side queue if requested and cancels
 * write side flush.  If read side flush is also requested, the read side is flushed and the message
 * is passed upwards.  If read side flush is not requested, the message is freed.
 */
noinline fastcall int
ss_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(OTHERQ(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(OTHERQ(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * ss_w_ioctl: - process M_IOCTL or M_IOCDATA messages
 * @q: active queue (write queue only)
 * @mp: the M_IOCTL or M_IOCDATA message
 *
 * This is canonical driver write processing for M_IOCTL or M_IOCDATA messages for a driver that has
 * no input-output controls of its own.  Messages are simply negatively acknowledged.
 */
noinline fastcall int
ss_w_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other Message Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * ss_w_other: - process other messages on the driver write queue
 * @q: active queue (write queue only)
 * @mp: the message
 *
 * The only other messages that we expect on the driver write queue is M_FLUSH and M_IOCTL or
 * M_IOCDATA messages.  We process these appropriately.  Any other message is an error and is
 * discarded.
 */
noinline fastcall int
ss_w_other(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_FLUSH)) {
	case M_FLUSH:
		return ss_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return ss_w_ioctl(q, mp);
	}
	LOGERR(PRIV(q), "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	freemsg(mp);
	return (0);
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
/**
 * ss_w_prim_put: - put a TPI primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the fast path for the TPI write put procedure.  Data is simply queued by returning
 * -%EAGAIN.  This provides better flow control and scheduling for maximum throughput.
 */
static inline streamscall __hot_put int
ss_w_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return ss_w_proto(q, mp);
	case M_DATA:
		return ss_w_data(q, mp);
	default:
		return ss_w_other(q, mp);
	}
}

/**
 * ss_w_prim_srv: = service a TPI primitive
 * @ss: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the fast path for the TPI write service procedure.  Data is processed.  Processing data
 * from the write service procedure provies better flow control and scheduling for maximum
 * throughput and minimum latency.
 */
static inline streamscall __hot_put int
ss_w_prim_srv(ss_t *ss, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __ss_w_proto(ss, q, mp);
	case M_DATA:
		return __ss_w_data(ss, q, mp);
	default:
		return ss_w_other(q, mp);
	}
}

/*
 *  READ SIDE wakeup procedure (called on each service procedure exit)
 *  -------------------------------------------------------------------------
 */

/**
 * __ss_r_child_state_change: handle state changes on an open request child
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying socket for state change (parent)
 * @type: type of socket
 *
 * For each of the open request children, if SOCK_PRIV(sk) points to the listening Stream, then a
 * connection indication has not yet been created for the open-request child.  A new mblk containing
 * the connection indication information is to be created, a connection indication formulated and
 * the connection indication appended to the listening stream and the SOCK_PRIV(sk) of the open
 * request child set to point to the connection indication structure.  This latter is done by
 * t_conn_ind().
 *
 * For each of the connection indications that is marked with a state change flag, a disconnect
 * indication is formulated and the connection indication is deleted from the listening stream.
 * This is done by t_discon_ind() with a non-NULL connection indication pointer.
 *
 * When a connection response or disconnect request is issued, the above synchronization is
 * performed on the connection indication only when it is marked with a state change flag.  If there
 * exists a connection indication with the appropriate sequence number from the connection response
 * or disconnect request, with no state change flag set, the appropriate action is performed and the
 * connection indication deleted.
 */
noinline fastcall int
__ss_r_child_state_change(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int err = 0;

	if (err >= 0) {
		lock_sock(sk);
		if (sock_accept_queue_head(sk)) {
			struct open_request *req;

			sock_accept_queue_lock(sk);
			for (req = sock_accept_queue_head(sk); req; req = req->dl_next) {
				if (SOCK_PRIV(req->sk) == ss && req->sk->sk_state == TCP_ESTABLISHED)
					if ((err = t_conn_ind(ss, q, req->sk)) < 0)
						break;
			}
			sock_accept_queue_unlock(sk);
		}
		release_sock(sk);
	}
	if (err >= 0) {
		mblk_t *cp, *cp_next;

		cp_next = bufq_head(&ss->conq);
		while ((cp = cp_next)) {
			cp_next = cp->b_next;
			if (((ss_conind_t *) cp->b_rptr)->ci_rflags & (1 << SS_BIT_STATE_CHANGE))
				if ((err = t_discon_ind(ss, q, sk, ECONNRESET, cp)) < 0)
					break;
		}
	}
	return (err);
}

/**
 * __ss_r_state_change: - handle state changes since last invocation
 * @ss: private structure (locked)
 * @q: active queue
 * @sk: underlying socket with state change
 * @type: type of socket
 *
 * State changes must only be permitted in an synchronous fashion.  State changes are blocked for
 * other processes while the lock is held by the private structure.  This permits state changes to
 * be performed in a syncrhonous manner.  Note that an event will be effected, but it might not be
 * possible to report the event immediately.  The reporting of the events are one of the following:
 *
 * T_CONN_IND:
 *	-> TCP_ESTABLISHED in TS_IDLE or TS_WRES_CIND state on listening Stream. (XXX)
 *
 * T_CONN_CON:
 *	new state is TS_DATA_XFER
 *	-> TCP_ESTABISHED in TS_WCON_CREQ state.
 *
 * T_ORDREL_IND:
 *	new state is TS_IDLE
 *	-> TCP_TIME_WAIT | TCP_CLOSE in TS_WIND_ORDREL state from  TCP_FIN_WAIT2.
 *
 *	new state is TS_WREQ_ORDREL
 *	-> TCP_CLOSE_WAIT in TS_DATA_XFER state.
 *
 * T_DISCON_IND:
 *	new state is TS_IDLE
 *	-> TCP_TIME_WAIT | TCP_CLOSE in TS_WCON_CREQ state.
 *	-> TCP_TIME_WAIT | TCP_CLOSE in TS_WIND_ORDREL state from TCP_ESTABLISHED or TCP_FIN_WAIT1.
 *	-> TCP_TIME_WAIT | TCP_CLOSE in TS_DATA_XFER state.
 *	-> TCP_TIME_WAIT | TCP_CLOSE in TS_WREQ_ORDREL state from TCP_CLOSE_WAIT.
 *
 *	new state is TS_IDLE or TS_WRES_CIND depending on remaining connection indications.
 *	-> TCP_TIME_WAIT | TCP_CLOSE in TS_IDLE or TS_WRES_CIND state on listening Stream. (XXX)
 *
 *  There are two kinds of indications generated based on state change callbacks: indications that
 *  pertain to an open-request children of a listening socket, and those that pertain to the socket
 *  itself.  Listening sockets do not experience state changes of their own while they are
 *  listening.  Other sockets do.  A state change indication on a listening socket indicates a state
 *  change on one of its open-request children.
 *
 *  When processing
 */
noinline fastcall __hot void
__ss_r_state_change(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	int tcp_oldstate, tcp_newstate, tpi_oldstate, err;

//      LOGRX(ss, "%s", __FUNCTION__);
	tcp_newstate = sk->sk_state;
	tcp_oldstate = XCHG(&ss->tcp_state, tcp_newstate);
	tpi_oldstate = ss_get_state(ss);

#if 0
	if (tcp_newstate == tcp_oldstate && tcp_oldstate != TCP_LISTEN)
		return;
#endif

	LOGST(ss, "%s [%s <- %s] (%s) %p", __FUNCTION__, tcp_statename(tcp_newstate),
		 tcp_statename(tcp_oldstate), tpi_statename(tpi_oldstate), sk);

	switch (__builtin_expect(type, SOCK_STREAM)) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (tpi_oldstate) {
		case TS_UNBND:
			break;
		case TS_IDLE:
		case TS_WRES_CIND:
			switch (tcp_oldstate) {
			case TCP_LISTEN:
				/* state change was on child */
				if ((err = __ss_r_child_state_change(ss, q, sk, type)))
					goto error;
				goto done;
#if 0
				ss->tcp_state = tcp_oldstate;
				switch (tcp_newstate) {
				case TCP_TIME_WAIT:
				case TCP_CLOSE:
					if ((err = t_discon_ind(ss, q, sk, reason, cp)))
						goto error;
					goto done;
				case TCP_ESTABLISHED:
					if ((err = t_conn_ind(ss, q, sk, reason, cp)))
						goto error;
					goto done;
				}
#endif
				break;
			case TCP_CLOSE_WAIT:
				switch (tcp_newstate) {
				case TCP_LAST_ACK:
				case TCP_CLOSE:
					goto done;
				}
				break;
			case TCP_FIN_WAIT2:
			case TCP_LAST_ACK:
				switch (tcp_newstate) {
				case TCP_TIME_WAIT:
				case TCP_CLOSE:
					goto done;
				}
				break;
			}
			break;
		case TS_WCON_CREQ:
			switch (tcp_newstate) {
			case TCP_ESTABLISHED:
				if ((err = t_conn_con(ss, q)))
					goto error;
				goto done;
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				/* The new TPI state will become TS_IDLE. */
				if ((err = t_discon_ind(ss, q, sk, 0, NULL)))
					goto error;
				goto done;
			}
			break;
		case TS_DATA_XFER:
			switch (tcp_newstate) {
			case TCP_CLOSE_WAIT:
				/* The new TPI state will become TS_WREQ_ORDREL. */
				if ((err = t_ordrel_ind(ss, q)))
					goto error;
				goto done;
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				/* The new TPI state will become TS_IDLE. */
				if ((err = t_discon_ind(ss, q, sk, 0, NULL)))
					goto error;
				goto done;
			}
			break;
		case TS_WREQ_ORDREL:
			switch (tcp_newstate) {
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (tcp_oldstate) {
				case TCP_CLOSE_WAIT:
					/* The new TPI state will become TS_IDLE. */
					if ((err = t_discon_ind(ss, q, sk, 0, NULL)))
						goto error;
					goto done;
				}
				break;
			}
			break;
		case TS_WIND_ORDREL:
			switch (tcp_newstate) {
			case TCP_TIME_WAIT:
				/* The new TPI state will become TS_IDLE. */
				if ((err = t_discon_ind(ss, q, sk, 0, NULL)))
					goto error;
				goto done;
			case TCP_CLOSE:
				switch (tcp_oldstate) {
				case TCP_FIN_WAIT2:
					/* The new TPI state will become TS_IDLE. */
					if ((err = t_ordrel_ind(ss, q)))
						goto error;
					goto done;
				case TCP_FIN_WAIT1:
				case TCP_ESTABLISHED:
					/* Went right through TCP_FIN_WAIT1 or TCP_FIN_WAIT2: the
					   was to detect which one is to check for errors. */
					if (sk->sk_shutdown == 0 || ss->lasterror != 0
					    || sk->sk_err != 0) {
						/* The new TPI state will become TS_IDLE. */
						if ((err = t_discon_ind(ss, q, sk, 0, NULL)))
							goto error;
					} else {
						/* The new TPI state will become TS_IDLE. */
						if ((err = t_ordrel_ind(ss, q)))
							goto error;
					}
					goto done;
				}
				break;
			case TCP_FIN_WAIT1:
			case TCP_FIN_WAIT2:
				goto done;
			}
			break;
		default:
			break;
		}
		if (tcp_oldstate != tcp_newstate)
			LOGERR(ss, "SWERR: socket state %s -> %s in TPI state %s: %s %s:%d",
				  tcp_statename(tcp_oldstate), tcp_statename(tcp_newstate),
				  tpi_statename(tpi_oldstate), __FUNCTION__, __FILE__, __LINE__);
		break;
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		/* connectionless sockets are stateless */
		LOGERR(ss, "SWERR: socket state %s -> %s in TPI state %s: %s %s:%d",
			  tcp_statename(tcp_oldstate), tcp_statename(tcp_newstate),
			  tpi_statename(tpi_oldstate), __FUNCTION__, __FILE__, __LINE__);
		break;
	default:
		LOGERR(ss, "SWERR: unsupported socket type %d: %s %s:%d",
			  ss->p.prot.type, __FUNCTION__, __FILE__, __LINE__);
		break;
	}
      done:
	return;
      error:
	/* Based on the error we can decide what to do.  If there is a buffer problem (%ENOBUFS or
	   %ENOMEM), then schedule a bufcall with the sk pointer as the argument.  If there is a
	   flow control problem (%EBUSY), then set the appropriate bit and await back-enabling of
	   the queue. */
	switch (-err) {
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EDEADLK:
	case EAGAIN:
		/* regenerate state change service */
		set_bit(SS_BIT_STATE_CHANGE, &ss->ss_rflags);
		break;
	}
	return;
}

/**
 * __ss_r_error_report: - service an error report callback
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: underlying socket
 * @type: socket type
 *
 * Error reports can be used to disconnect connection-oriented sockets in a connecting or data
 * transfer state.
 */
noinline fastcall __hot void
__ss_r_error_report(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	t_scalar_t tpi_oldstate = ss_get_state(ss);
	int tcp_oldstate = ss->tcp_state;

	LOGRX(ss, "%s", __FUNCTION__);
	if (unlikely(sk->sk_err == 0))
		/* error report was already absorbed */
		return;

	switch (__builtin_expect(type, SOCK_STREAM)) {
	case SOCK_STREAM:	/* TCP */
	case SOCK_SEQPACKET:	/* SCTP */
		switch (__builtin_expect(tpi_oldstate, TS_DATA_XFER)) {
		case TS_UNBND:
			break;
		case TS_WRES_CIND:
			if (tcp_oldstate == TCP_LISTEN)
				return;
			break;
		case TS_IDLE:
			if (tcp_oldstate == TCP_LISTEN)
				return;
			/* fall through */
		case TS_WCON_CREQ:
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
			ss_all_error_ind(ss, q, sk, type);
			return;
		}
		break;
	case SOCK_DGRAM:	/* UDP */
	case SOCK_RAW:		/* IP */
	case SOCK_RDM:
		switch (__builtin_expect(tpi_oldstate, TS_IDLE)) {
		case TS_IDLE:
			/* Read and deliver T_UDERROR_IND while the error queue can be read and
			   primitives can be delivered upstream. */
			ss_all_uderror_ind(ss, q, sk, type);
			return;
		}
		break;
	}
	LOGERR(ss, "SWERR: socket type %d state %s -> %s in TPI state %s: %s %s:%d", type,
		  tcp_statename(tcp_oldstate), tcp_statename(sk->sk_state),
		  tpi_statename(tpi_oldstate), __FUNCTION__, __FILE__, __LINE__);
	return;
}

/**
 * __ss_r_data_ready: - service a data ready callback
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 * @sk: underlying socket
 * @type: socket type
 *
 * Processed receive data until no more receive data an be processed.  This relies on calling
 * __ss_r_recvmsg().  __ss_r_recvmsg() is a function that must read data until no more receive
 * data can be processed.  When the function returns due to upstream flow control or insufficient
 * message blocks, the function must set SS_BIT_DATA_READY in ss->ss_rflags as well as rescheduling the
 * queue for later processing when flow control subsides or message block becomes available.
 */
noinline fastcall __hot void
__ss_r_data_ready(ss_t *ss, queue_t *q, struct sock *sk, int type)
{
	t_scalar_t tpi_oldstate = ss_get_state(ss);
	int tcp_oldstate = ss->tcp_state;

	LOGRX(ss, "%s", __FUNCTION__);
	switch (__builtin_expect(type, SOCK_STREAM)) {
	case SOCK_STREAM:	/* TCP */
	case SOCK_SEQPACKET:	/* SCTP */
		switch (__builtin_expect(tpi_oldstate, TS_DATA_XFER)) {
		case TS_UNBND:
		case TS_WCON_CREQ:
			break;
		case TS_WRES_CIND:
			if (tcp_oldstate == TCP_LISTEN)
				return;
			break;
		case TS_IDLE:
			if (tcp_oldstate == TCP_LISTEN)
				return;
			/* fall through */
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
			/* Read and deliver T_EXDATA_IND and T_DATA_IND while the receive queue can
			   be read and primitives can be delivered upstream. */
			ss_all_data_ind(ss, q, sk, type);
			return;
		}
		break;
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (__builtin_expect(tpi_oldstate, TS_IDLE)) {
		case TS_IDLE:
			/* Read and deliver T_UNITDATA_IND while the receive queue can be read and
			   primitives can be delivered upstream. */
			ss_all_unitdata_ind(ss, q, sk, type);
			return;
		}
		break;
	}
	LOGERR(ss, "SWERR: socket %d state %s -> %s in TPI state %s: %s %s:%d", type,
		  tcp_statename(tcp_oldstate), tcp_statename(sk->sk_state),
		  tpi_statename(tpi_oldstate), __FUNCTION__, __FILE__, __LINE__);
	return;
}

/**
 * __ss_r_events: - process pending events on a socket
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 */
noinline fastcall __hot void
__ss_r_events(ss_t *ss, queue_t *q)
{
	struct sock *sk;
	int type = ss->p.prot.type;
	long ss_rflags = xchg(&ss->ss_rflags, 0);

	if (!(ss->sock))
		return;
	if (!(sk = ss->sock->sk))
		return;

	if (test_bit(SS_BIT_DATA_READY, &ss_rflags))
		__ss_r_data_ready(ss, q, sk, type);
	if (test_bit(SS_BIT_ERROR_REPORT, &ss_rflags))
		__ss_r_error_report(ss, q, sk, type);
	if (test_bit(SS_BIT_STATE_CHANGE, &ss_rflags))
		__ss_r_state_change(ss, q, sk, type);
	return;
}

/**
 * ss_r_recvmsg: - receive error and normal message on a stream
 * @ss: private structure (locked)
 * @q: active queue (read queue only)
 */
noinline fastcall __hot_read void
ss_r_recvmsg(ss_t *ss, queue_t *q)
{
	struct sock *sk;
	int type;

	if (unlikely(!ss->sock))
		return;
	if (unlikely(!(sk = ss->sock->sk)))
		return;

	switch (__builtin_expect((type = ss->p.prot.type), SOCK_STREAM)) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		/* Read and deliver T_EXDATA_IND and T_DATA_IND while the receive queue can
		   be read and primitives can be delivered upstream. */
		ss_all_data_ind(ss, q, sk, type);
		break;
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		/* Read and deliver T_UNITDATA_IND while the receive queue can be read and
		   primitives can be delivered upstream. */
		ss_all_unitdata_ind(ss, q, sk, type);
		break;
	default:
		LOGERR(ss, "SWERR: unknown socket type %d: %s %s:%d", type, __FUNCTION__,
			  __FILE__, __LINE__);
		break;
	}
	return;
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 */

/**
 * ss_rput: - read put procedure
 * @q: active queue (read queue)
 *
 * The read put procedure is no longer used by the strinet driver.  Events result in generation of
 * messages upstream or state changes within the private structure.
 */
static streamscall __unlikely int
ss_rput(queue_t *q, mblk_t *mp)
{
	LOGERR(PRIV(q), "SWERR: Read put routine called!");
	freemsg(mp);
	return (0);
}

/**
 * ss_rsrv: - read service procedure
 * @q: active queue (read queue)
 *
 * The read service procedure is responsible for servicing the read side of the underlying socket
 * when data is available.  It is scheduled from socket callbacks on the read side: sk_data_ready
 * and sk_error_report and sk_state_change.
 */
static streamscall __hot_in int
ss_rsrv(queue_t *q)
{
	ss_t *ss;

	if (likely(!!(ss = ss_trylock(q)))) {
		__ss_r_events(ss, q);
		ss_unlock(ss);
	}
	return (0);
}

#if 0
#define PRELOAD (FASTBUF<<2)
#else
#define PRELOAD (0)
#endif

/**
 * ss_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 *
 * This is a canoncial put procedure for write.  Locking is performed by the individual message
 * handling procedures.
 */
static streamscall __hot_in int
ss_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(ss_w_prim_put(q, mp))) {
		ss_wstat.ms_acnt++;
		/* apply backpressure */
		mp->b_wptr += PRELOAD;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* must succeed */
		}
	}
	return (0);
}

/**
 * ss_wsrv: - write service procedure
 * @q: active queue (write queue)
 *
 * This is a canonical service procedure for write.  Locking is performed outside the loop so that
 * locks do not need to be released and acquired with each loop.  Note that the wakeup function must
 * also be executed with the private structure locked.
 */
static streamscall __hot_in int
ss_wsrv(queue_t *q)
{
	ss_t *ss;
	mblk_t *mp;

	if (likely(!!(ss = ss_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			/* remove backpressure */
			mp->b_wptr -= PRELOAD;
			if (unlikely(ss_w_prim_srv(ss, q, mp))) {
				/* reapply backpressure */
				mp->b_wptr += PRELOAD;
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(ss, "write queue stalled");
				break;
			}
		}
#if 0
		ss_w_wakeup(ss, q);
#endif
		ss_unlock(ss);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */

unsigned short modid = DRV_ID;

major_t major = CMAJOR_0;

static const ss_profile_t ss_profiles[] = {
	/* For all SOCK_RAW protocols, we seem able to send zero-length messages, but are unable to
	   receive them. */
	{{PF_INET, SOCK_RAW, IPPROTO_ICMP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_IGMP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_IPIP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	{{PF_INET, SOCK_STREAM, IPPROTO_TCP},
	 {T_INFO_ACK, 0, 1, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65535, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_EGP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_PUP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	/* T_SNDZERO is specified in UNIX '98 XNS 5.2 for UDP.  We seem to be able to send zero
	   length UDP datagrams but cannot receive them. */
	{{PF_INET, SOCK_DGRAM, IPPROTO_UDP},
	 {T_INFO_ACK, 65507, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65507, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_IDP},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
	,
	{{PF_INET, SOCK_RAW, IPPROTO_RAW},
	 {T_INFO_ACK, 65515, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 65515, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
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
	  T_INFINITE, 65507, T_CLTS, TS_UNBND, XPG4_1 | T_SNDZERO}}
#if defined HAVE_OPENSS7_SCTP
	,
	{{PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP},
	 {T_INFO_ACK, T_INFINITE, T_INFINITE, 536, T_INVALID,
	  8 * sizeof(struct sockaddr_in),
	  T_INFINITE, 65535, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}}
#endif				/* defined HAVE_OPENSS7_SCTP */
};
/**
 * ss_alloc_qbands: - pre-allocate queue bands for queue pair
 * @q: queue for which to allocate queue bands
 */
static fastcall int
ss_alloc_qbands(queue_t *q, int cminor)
{
	int err;
	unsigned long pl;

	pl = freezestr(q);
	{
		/* Pre-allocate queue band structures on the read side. */
		if ((err = strqset(q, QHIWAT, 1, STRHIGH))) {
			strlog(major, cminor, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
			       "ERROR: could not allocate queue band 1 structure, err = %d", err);
		} else if ((err = strqset(q, QHIWAT, 2, STRHIGH))) {
			strlog(major, cminor, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
			       "ERROR: could not allocate queue band 2 structure, err = %d", err);
		}
	}
	unfreezestr(q, pl);
	return (err);
}

/**
 * ss_qopen: - STREAMS open procedure
 * @q: newly created queue pair
 * @devp: pointer to device number of driver
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
static streamscall int
ss_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	minor_t cminor = getminor(*devp);
	unsigned long flags;
	caddr_t ptr;
	ss_t *ss;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR)
		return (ENXIO);
	if (!mi_set_sth_lowat(q, 0))
		return (ENOBUFS);
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		return (ENOBUFS);
	if ((err = ss_alloc_qbands(q, cminor)))
		return (err);
	if (!(ss = (ss_t *) (ptr = mi_open_alloc_cache(ss_priv_cachep, GFP_KERNEL))))
		return (ENOMEM);
	{
		bzero(ss, sizeof(*ss));
		ss->ss_parent = ss;
		ss->rq = RD(q);
		ss->wq = WR(q);
		ss->p = ss_profiles[cminor - FIRST_CMINOR];
		ss->cred = *crp;
		bufq_init(&ss->conq);
	}
	sflag = CLONEOPEN;
	cminor = FREE_CMINOR;	/* start at the first free minor device number */
	/* The static device range for mi_open_link() is 5 or 10, and inet uses 50, so we need to
	   adjust the minor device number before calling mi_open_link(). */
	*devp = makedevice(getmajor(*devp), cminor);
	write_lock_irqsave(&ss_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &ss_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err = mi_open_link(&ss_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&ss_lock, flags);
	ss_sock_init(ss);
	return (0);
      unlock_free:
	mi_close_free_cache(ss_priv_cachep, ptr);
	write_unlock_irqrestore(&ss_lock, flags);
	return (err);
}

/**
 * ss_qclose: - STREAMS close procedure
 * @q: queue pair
 * @oflags: file open flags
 * @crp: credentials of closing process
 */
static streamscall int
ss_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;
	ss_t *ss = PRIV(q);
	caddr_t ptr = (caddr_t) ss;
	int err;

	ss_socket_put(&ss->sock);
	write_lock_irqsave(&ss_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &ss_lock, &flags);
	qprocsoff(q);
	while (bufq_head(&ss->conq))
		freemsg(__bufq_dequeue(&ss->conq));
	ss->ss_parent = NULL;
	ss->rq = NULL;
	ss->wq = NULL;
	err = mi_close_comm(&ss_opens, q);
	write_unlock_irqrestore(&ss_lock, flags);
	return (err);
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

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, int, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static struct cdevsw ss_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ss_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static struct devnode inet_node_ip = {
	.n_name = "ip",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_icmp = {
	.n_name = "icmp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_ggp = {
	.n_name = "ggp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_ipip = {
	.n_name = "ipip",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_tcp = {
	.n_name = "tcp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_egp = {
	.n_name = "egp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_pup = {
	.n_name = "pup",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_udp = {
	.n_name = "udp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_idp = {
	.n_name = "idp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_rawip = {
	.n_name = "rawip",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_ticots_ord = {
	.n_name = "ticots_ord",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_ticots = {
	.n_name = "ticots",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode inet_node_ticlts = {
	.n_name = "ticlts",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

#if defined HAVE_OPENSS7_SCTP
static struct devnode inet_node_sctp = {
	.n_name = "sctp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
#endif

/**
 * ss_register_strdev: - register STREAMS pseudo-device driver
 * @major: major device number to register (0 for allocation)
 *
 * This function registers the INET pseudo-device driver and a host of minor device nodes associated
 * with the driver.  Should this function fail it returns a negative error number; otherwise, it
 * returns zero.  Only failure to register the major device number is considered an error as minor
 * device nodes in the specfs are optional.
 */
static int
ss_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&ss_cdev, major)) < 0)
		return (err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ip, IP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register IP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_icmp, ICMP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register ICMP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ggp, GGP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register GGP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ipip, IPIP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register IPIP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_tcp, TCP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TCP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_egp, EGP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register EGP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_pup, PUP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register PUP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_udp, UDP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register UDP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_idp, IDP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register IDP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_rawip, RAWIP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register RAWIP_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ticots_ord, TICOTS_ORD_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TICOTS_ORD_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ticots, TICOTS_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TICOTS_CMINOR, err = %d", err);
	if ((err = register_strnod(&ss_cdev, &inet_node_ticlts, TICLTS_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TICLTS_CMINOR, err = %d", err);
#if defined HAVE_OPENSS7_SCTP
	if ((err = register_strnod(&ss_cdev, &inet_node_sctp, SCTP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register SCTP_CMINOR, err = %d", err);
#endif
	return (0);
}

/**
 * ss_unregister_strdev: - unregister STREAMS pseudo-device driver
 * @major: major device number to unregister
 *
 * The function unregisters the host of minor device nodes and the major deivce node associated with
 * the driver in the reverse order in which they were allocated.  Only deregistration of the major
 * device node is considered fatal as minor device nodes were optional during initialization.
 */
static int
ss_unregister_strdev(major_t major)
{
	int err;

#if defined HAVE_OPENSS7_SCTP
	if ((err = unregister_strnod(&ss_cdev, SCTP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister SCTP_CMINOR, err = %d", err);
#endif
	if ((err = unregister_strnod(&ss_cdev, TICLTS_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TICLTS_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, TICOTS_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TICOTS_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, TICOTS_ORD_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TICOTS_ORD_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, RAWIP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister RAWIP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, IDP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister IDP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, UDP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister UDP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, PUP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister PUP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, EGP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister EGP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, TCP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TCP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, IPIP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister IPIP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, GGP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister GGP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, ICMP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister ICMP_CMINOR, err = %d", err);
	if ((err = unregister_strnod(&ss_cdev, IP_CMINOR)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister IP_CMINOR, err = %d", err);
	if ((err = unregister_strdev(&ss_cdev, major)) < 0)
		return (err);
	return (0);
}

/**
 * ss_init: - initialize the INET kernel module under Linux
 */
static __init int
ss_init(void)
{
	int err;

	if ((err = ss_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s could not register STREAMS device, err = %d", DRV_NAME, err);
		return (err);
	}
	if (major == 0)
		major = err;
	if ((err = ss_init_caches())) {
		cmn_err(CE_WARN, "%s could not initialize caches, err = %d", DRV_NAME, err);
		ss_unregister_strdev(major);
		return (err);
	}
	return (0);
}

/**
 * ss_exit: - remove the INET kernel module under Linux
 */
static __exit void
ss_exit(void)
{
	int err;

	if ((err = ss_term_caches()))
		cmn_err(CE_WARN, "%s could not terminate caches, err = %d", DRV_NAME, err);
	if ((err = ss_unregister_strdev(major)) < 0)
		cmn_err(CE_WARN, "%s could not unregister STREAMS device, err = %d", DRV_NAME, err);
	return;
}

module_init(ss_init);
module_exit(ss_exit);

#endif				/* LINUX */
