/*****************************************************************************

 @(#) $RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-09-01 09:09:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2009-09-01 09:09:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: wrapper.c,v $
 Revision 1.1.2.2  2009-09-01 09:09:51  brian
 - added text image files

 Revision 1.1.2.1  2009-07-23 16:39:29  brian
 - added wrapper module

 *****************************************************************************/

#ident "@(#) $RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-09-01 09:09:51 $"

static char const ident[] = "$RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-09-01 09:09:51 $";

#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/dst.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/file.h>

#if 0
#include <sys/os7/compat.h>

#define WRAPPER_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define WRAPPER_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define WRAPPER_REVISION	"LfS $RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-09-01 09:09:51 $"
#define WRAPPER_DEVICE		"SVR 4.2 Wrappers (WRAPPER)"
#define WRAPPER_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define WRAPPER_LICENSE		"GPL"

#ifdef CONFIG_STREAMS_MODULE
MODULE_AUTHOR(WRAPPER_CONTACT);
MODULE_DESCRIPTION(WRAPPER_DESCRIP);
MODULE_SUPPORTED_DEVICE(WRAPPER_DEVICE);
MODULE_LICENSE(WRAPPER_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("wrapper");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif
#endif
#endif

#ifdef HAVE_SESSION_OF_PGRP_ADDR
//pid_t session_of_pgrp(pid_t);
__asm__(".equ session_of_pgrp," __stringify(HAVE_SESSION_OF_PGRP_ADDR));
__asm__(".global session_of_pgrp");
EXPORT_SYMBOL_GPL(session_of_pgrp);
#endif				/* defined HAVE_SESSION_OF_PGRP_ADDR */

#ifdef HAVE_IS_IGNORED_ADDR
extern int is_ignored(int sig);
__asm__(".equ is_ignored," __stringify(HAVE_IS_IGNORED_ADDR));
__asm__(".global is_ignored");
EXPORT_SYMBOL_GPL(is_ignored);
#endif				/* HAVE_IS_IGNORED_ADDR */

#ifdef HAVE_IS_ORPHANED_PGRP_ADDR
extern int is_orphaned_pgrp(int pgrp);

__asm__(".equ is_orphaned_pgrp," __stringify(HAVE_IS_ORPHANED_PGRP_ADDR));
__asm__(".global is_orphaned_pgrp");
EXPORT_SYMBOL_GPL(is_orphaned_pgrp);
#endif				/* HAVE_IS_ORPHANED_PGRP_ADDR */

#ifdef HAVE_IS_CURRENT_PGRP_ORPHANED_ADDR
extern int is_current_pgrp_orphaned(void);
__asm__(".equ is_current_pgrp_orphaned," __stringify(HAVE_IS_CURRENT_PGRP_ORPHANED_ADDR));
__asm__(".global is_current_pgrp_orphaned");
EXPORT_SYMBOL_GPL(is_current_pgrp_orphaned);
#endif				/* HAVE_IS_CURRENT_PGRP_ORPHANED_ADDR */

#ifdef HAVE_TASKLIST_LOCK_ADDR
extern rwlock_t tasklist_lock;
__asm__(".equ tasklist_lock," __stringify(HAVE_TASKLIST_LOCK_ADDR));
__asm__(".global tasklist_lock");
EXPORT_SYMBOL_GPL(tasklist_lock);
#endif				/* HAVE_TASKLIST_LOCK_ADDR */

#ifdef HAVE_KILL_PROC_INFO_ADDR
int kill_proc_info(int sig, struct siginfo *sip, pid_t pid);
__asm__(".equ kill_proc_info," __stringify(HAVE_KILL_PROC_INFO_ADDR));
__asm__(".global kill_proc_info");
EXPORT_SYMBOL_GPL(kill_proc_info);
#endif				/* HAVE_KILL_PROC_INFO_ADDR */

#ifdef HAVE_KILL_SL_ADDR
int kill_sl_func(pid_t, int, int);
__asm__(".equ kill_sl_func," __stringify(HAVE_KILL_SL_FUNC_ADDR));
__asm__(".global kill_sl_func");
EXPORT_SYMBOL_GPL(kill_sl_func);
#endif				/* HAVE_KILL_SL_ADDR */

#ifdef HAVE_SEND_GROUP_SIG_INFO_ADDR
int send_group_sig_info(int, struct siginfo *, struct task_struct *);
__asm__(".equ send_group_sig_info," __stringify(HAVE_SEND_GROUP_SIG_INFO_ADDR));
__asm__(".global send_group_sig_info");
EXPORT_SYMBOL_GPL(send_group_sig_info);
#endif				/* HAVE_SEND_GROUP_SIG_INFO_ADDR */

#ifdef HAVE_GROUP_SEND_SIG_INFO_ADDR
int group_send_sig_info(int, struct siginfo *, struct task_struct *);
__asm__(".equ group_send_sig_info," __stringify(HAVE_GROUP_SEND_SIG_INFO_ADDR));
__asm__(".global group_send_sig_info");
EXPORT_SYMBOL_GPL(group_send_sig_info);
#endif				/* HAVE_GROUP_SEND_SIG_INFO_ADDR */

#ifdef HAVE___WAKE_UP_SYNC_ADDR
__asm__(".equ __wake_up_sync," __stringify(HAVE___WAKE_UP_SYNC_ADDR));
__asm__(".global __wake_up_sync");
EXPORT_SYMBOL_GPL(__wake_up_sync);
#endif				/* HAVE___WAKE_UP_SYNC_ADDR */

#ifdef HAVE_MODULE_TEXT_ADDRESS_ADDR
//extern struct module *module_text_address(ulong addr);
//
__asm__(".equ module_text_address," __stringify(HAVE_MODULE_TEXT_ADDRESS_ADDR));
__asm__(".global module_text_address");
EXPORT_SYMBOL_GPL(module_text_address);
#endif				/* HAVE_MODULE_TEXT_ADDRESS_ADDR */

#ifdef HAVE_FILE_MOVE_ADDR
__asm__(".equ file_move," __stringify(HAVE_FILE_MOVE_ADDR));
__asm__(".global file_move");
EXPORT_SYMBOL_GPL(file_move);
#endif				/* HAVE_FILE_MOVE_ADDR */

#ifdef HAVE_FILE_KILL_ADDR
//extern void file_kill(struct file *file);
__asm__(".equ file_kill," __stringify(HAVE_FILE_KILL_ADDR));
__asm__(".global file_kill");
EXPORT_SYMBOL_GPL(file_kill);
#endif				/* HAVE_FILE_KILL_ADDR */

#ifdef HAVE_PUT_FILP_ADDR
__asm__(".equ put_filp," __stringify(HAVE_PUT_FILP_ADDR));
__asm__(".global put_filp");
EXPORT_SYMBOL_GPL(put_filp);
#endif				/* HAVE_PUT_FILP_ADDR */

#ifdef HAVE_KTHREAD_SHOULD_STOP_ADDR
/* SLES 2.6.5 takes the prize for kernel developer stupidity! */
__asm__(".equ kthread_should_stop," __stringify(HAVE_KTHREAD_SHOULD_STOP_ADDR));
__asm__(".global kthread_should_stop");
EXPORT_SYMBOL_GPL(kthread_should_stop);
#endif				/* HAVE_KTHREAD_SHOULD_STOP_ADDR */

#ifdef HAVE_KTHREAD_CREATE_ADDR
/* SLES 2.6.5 takes the prize for kernel developer stupidity! */
__asm__(".equ kthread_create," __stringify(HAVE_KTHREAD_CREATE_ADDR));
__asm__(".global kthread_create");
EXPORT_SYMBOL_GPL(kthread_create);
#endif				/* HAVE_KTHREAD_CREATE_ADDR */

#ifdef HAVE_KTHREAD_BIND_ADDR
/* SLES 2.6.5 takes the prize for kernel developer stupidity! */
__asm__(".equ kthread_bind," __stringify(HAVE_KTHREAD_BIND_ADDR));
__asm__(".global kthread_bind");
EXPORT_SYMBOL_GPL(kthread_bind);
#endif				/* HAVE_KTHREAD_BIND_ADDR */

#ifdef HAVE_KTHREAD_STOP_ADDR
/* SLES 2.6.5 takes the prize for kernel developer stupidity! */
__asm__(".equ kthread_stop," __stringify(HAVE_KTHREAD_STOP_ADDR));
__asm__(".global kthread_stop");
EXPORT_SYMBOL_GPL(kthread_stop);
#endif				/* HAVE_KTHREAD_STOP_ADDR */

#ifdef HAVE_INET_PROTO_LOCK_ADDR
extern spinlock_t inet_proto_lock;
__asm__(".equ inet_proto_lock," __stringify(HAVE_INET_PROTO_LOCK_ADDR));
__asm__(".global inet_proto_lock");
EXPORT_SYMBOL_GPL(inet_proto_lock);
#endif				/* HAVE_INET_PROTO_LOCK_ADDR */

#ifdef HAVE_INET_PROTOS_ADDR
__asm__(".equ inet_protos," __stringify(HAVE_INET_PROTOS_ADDR));
__asm__(".global inet_protos");
EXPORT_SYMBOL_GPL(inet_protos);
#endif				/* HAVE_INET_PROTOS_ADDR */

#ifdef HAVE_SKBUFF_HEAD_CACHE_ADDR
extern kmem_cachep_t skbuff_head_cache;
__asm__(".equ skbuff_head_cache," __stringify(HAVE_SKBUFF_HEAD_CACHE_ADDR));
__asm__(".global skbuff_head_cache");
EXPORT_SYMBOL_GPL(skbuff_head_cache);
#endif				/* HAVE_SKBUFF_HEAD_CACHE_ADDR */

#ifdef HAVE_ICMP_ERR_CONVERT_ADDR
extern struct icmp_err icmp_err_convert[];
__asm__(".equ icmp_err_convert," __stringify(HAVE_ICMP_ERR_CONVERT_ADDR));
__asm__(".global icmp_err_convert");
EXPORT_SYMBOL_GPL(icmp_err_convert);
#endif				/* HAVE_ICMP_ERR_CONVERT_ADDR */

#ifdef HAVE_ICMP_STATISTICS_ADDR
extern struct icmp_mib icmp_statistic[];
__asm__(".equ icmp_statistics," __stringify(HAVE_ICMP_STATISTICS_ADDR));
__asm__(".global icmp_statistics");
EXPORT_SYMBOL_GPL(icmp_statistics);
#endif				/* HAVE_ICMP_STATISTICS_ADDR */

#ifdef HAVE_IP_ROUTE_OUTPUT_FLOW_ADDR
extern int ip_route_output_flow(struct rtable **rp, struct flowi *flp, struct sock *sk, int flags);
__asm__(".equ ip_route_output_flow," __stringify(HAVE_IP_ROUTE_OUTPUT_FLOW_ADDR));
__asm__(".global ip_route_output_flow");
EXPORT_SYMBOL_GPL(ip_route_output_flow);
#endif				/* HAVE_IP_ROUTE_OUTPUT_FLOW_ADDR */

#ifdef HAVE_IP_RT_MIN_PMTU_ADDR
extern int ip_rt_min_pmtu;
__asm__(".equ ip_rt_min_pmtu," __stringify(HAVE_IP_RT_MIN_PMTU_ADDR));
__asm__(".global ip_rt_min_pmtu");
EXPORT_SYMBOL_GPL(ip_rt_min_pmtu);
#endif				/* HAVE_IP_RT_MIN_PMTU_ADDR */

#ifdef HAVE_IP_RT_MTU_EXPIRES_ADDR
extern int ip_rt_mtu_expires;
__asm__(".equ ip_rt_mtu_expires," __stringify(HAVE_IP_RT_MTU_EXPIRES_ADDR));
__asm__(".global ip_rt_mtu_expires");
EXPORT_SYMBOL_GPL(ip_rt_mtu_expires);
#endif				/* HAVE_IP_RT_MTU_EXPIRES_ADDR */

#ifdef HAVE_IP_RT_UPDATE_PMTU_ADDR
void ip_rt_update_pmtu(struct dst_entry *dst, unsigned mtu);
__asm__(".equ ip_rt_update_pmtu," __stringify(HAVE_IP_RT_UPDATE_PMTU_ADDR));
__asm__(".global ip_rt_update_pmtu");
EXPORT_SYMBOL_GPL(ip_rt_update_pmtu);
#endif				/* HAVE_IP_RT_UPDATE_PMTU_ADDR */

#ifdef HAVE_SYSCTL_IP_DYNADDR_ADDR
extern int sysctl_ip_dynaddr;
__asm__(".equ sysctl_ip_dynaddr," __stringify(HAVE_SYSCTL_IP_DYNADDR_ADDR));
__asm__(".global sysctl_ip_dynaddr");
EXPORT_SYMBOL_GPL(sysctl_ip_dynaddr);
#endif				/* HAVE_SYSCTL_IP_DYNADDR_ADDR */

#ifdef HAVE_SYSCTL_IP_NONLOCAL_BIND_ADDR
extern int sysctl_ip_nonlocal_bind;
__asm__(".equ sysctl_ip_nonlocal_bind," __stringify(HAVE_SYSCTL_IP_NONLOCAL_BIND_ADDR));
__asm__(".global sysctl_ip_nonlocal_bind");
EXPORT_SYMBOL_GPL(sysctl_ip_nonlocal_bind);
#endif				/* HAVE_SYSCTL_IP_NONLOCAL_BIND_ADDR */

#ifdef HAVE_SYSCTL_IP_DEFAULT_TTL_ADDR
extern int sysctl_ip_default_ttl;
__asm__(".equ sysctl_ip_default_ttl," __stringify(HAVE_SYSCTL_IP_DEFAULT_TTL_ADDR));
__asm__(".global sysctl_ip_default_ttl");
EXPORT_SYMBOL_GPL(sysctl_ip_default_ttl);
#endif				/* HAVE_SYSCTL_IP_DEFAULT_TTL_ADDR */

#ifdef HAVE___IP_ROUTE_OUTPUT_KEY_ADDR
int __ip_route_output_key(struct rtable **rp, const struct flowi *flp);
__asm__(".equ __ip_route_output_key," __stringify(HAVE___IP_ROUTE_OUTPUT_KEY_ADDR));
__asm__(".global __ip_route_output_key");
EXPORT_SYMBOL_GPL(__ip_route_output_key);
#endif				/* HAVE___IP_ROUTE_OUTPUT_KEY_ADDR */

#ifdef HAVE___XFRM_POLICY_CHECK_ADDR
int __xfrm_policy_check(struct sock *sk, int dir, struct sk_buff *skb, unsigned short family);
__asm__(".equ __xfrm_policy_check," __stringify(HAVE___XFRM_POLICY_CHECK_ADDR));
__asm__(".global __xfrm_policy_check");
EXPORT_SYMBOL_GPL(__xfrm_policy_check);
#endif				/* HAVE___XFRM_POLICY_CHECK_ADDR */

#ifdef HAVE_XFRM_POLICY_DELETE_ADDR
#ifdef HAVE_XFRM_POLICY_DELETE_RETURNS_INT
int xfrm_policy_delete(struct xfrm_policy *pol, int dir);
#else
void xfrm_policy_delete(struct xfrm_policy *pol, int dir);
#endif
__asm__(".equ xfrm_policy_delete," __stringify(HAVE_XFRM_POLICY_DELETE_ADDR));
__asm__(".global xfrm_policy_delete");
EXPORT_SYMBOL_GPL(xfrm_policy_delete);
#endif				/* HAVE_XFRM_POLICY_DELETE_ADDR */

#ifdef HAVE_SNMP_GET_INFO_ADDR
int snmp_get_info(char *buffer, char **start, off_t offset, int length);
__asm__(".equ snmp_get_info," __stringify(HAVE_SNMP_GET_INFO_ADDR));
__asm__(".global snmp_get_info");
EXPORT_SYMBOL_GPL(snmp_get_info);
#endif				/* HAVE_SNMP_GET_INFO_ADDR */

#ifdef HAVE_IP_FRAG_NQUEUES_ADDR
__asm__(".equ ip_frag_nqueues," __stringify(HAVE_IP_FRAG_NQUEUES_ADDR));
__asm__(".global ip_frag_nqueues");
EXPORT_SYMBOL_GPL(ip_frag_nqueues);
#endif				/* HAVE_IP_FRAG_NQUEUES_ADDR */

#ifdef HAVE_IP_FRAG_MEM_ADDR
__asm__(".equ ip_frag_mem," __stringify(HAVE_IP_FRAG_MEM_ADDR));
__asm__(".global ip_frag_mem");
EXPORT_SYMBOL_GPL(ip_frag_mem);
#endif				/* HAVE_IP_FRAG_MEM_ADDR */

#ifdef HAVE_SYSCTL_RMEM_DEFAULT_ADDR
extern __u32 sysctl_rmem_default;
__asm__(".equ sysctl_rmem_default," __stringify(HAVE_SYSCTL_RMEM_DEFAULT_ADDR));
__asm__(".global sysctl_rmem_default");
EXPORT_SYMBOL_GPL(sysctl_rmem_default);
#endif				/* HAVE_SYSCTL_RMEM_DEFAULT_ADDR */

#ifdef HAVE_SYSCTL_WMEM_DEFAULT_ADDR
extern __u32 sysctl_wmem_default;
__asm__(".equ sysctl_wmem_default," __stringify(HAVE_SYSCTL_WMEM_DEFAULT_ADDR));
__asm__(".global sysctl_wmem_default");
EXPORT_SYMBOL_GPL(sysctl_wmem_default);
#endif				/* HAVE_SYSCTL_WMEM_DEFAULT_ADDR */

#ifdef HAVE_SYSCTL_RMEM_MAX_ADDR
__asm__(".equ sysctl_rmem_max," __stringify(HAVE_SYSCTL_RMEM_MAX_ADDR));
__asm__(".global sysctl_rmem_max");
EXPORT_SYMBOL_GPL(sysctl_rmem_max);
#endif				/* HAVE_SYSCTL_RMEM_MAX_ADDR */

#ifdef HAVE_SYSCTL_WMEM_MAX_ADDR
__asm__(".equ sysctl_wmem_max," __stringify(HAVE_SYSCTL_WMEM_MAX_ADDR));
__asm__(".global sysctl_wmem_max");
EXPORT_SYMBOL_GPL(sysctl_wmem_max);
#endif				/* HAVE_SYSCTL_WMEM_MAX_ADDR */

#ifdef HAVE_TCP_OPENREQ_CACHEP_ADDR
extern kmem_cachep_t tcp_openreq_cachep;
__asm__(".equ tcp_openreq_cachep," __stringify(HAVE_TCP_OPENREQ_CACHEP_ADDR));
__asm__(".global tcp_openreq_cachep");
EXPORT_SYMBOL_GPL(tcp_openreq_cachep);
#endif				/* HAVE_TCP_OPENREQ_CACHEP_ADDR */

#ifdef HAVE_TCP_SET_KEEPALIVE_ADDR
void tcp_set_keepalive(struct sock *sk, int val);
__asm__(".equ tcp_set_keepalive," __stringify(HAVE_TCP_SET_KEEPALIVE_ADDR));
__asm__(".global tcp_set_keepalive");
EXPORT_SYMBOL_GPL(tcp_set_keepalive);
#endif				/* HAVE_TCP_SET_KEEPALIVE_ADDR */

#ifdef HAVE_TCP_SYNC_MSS_ADDR
int tcp_sync_mss(struct sock *sk, u32 pmtu);
__asm__(".equ tcp_sync_mss," __stringify(HAVE_TCP_SYNC_MSS_ADDR));
__asm__(".global tcp_sync_mss");
EXPORT_SYMBOL_GPL(tcp_sync_mss);
#endif				/* HAVE_TCP_SYNC_MSS_ADDR */

#ifdef HAVE_TCP_WRITE_XMIT_ADDR
int tcp_write_xmit(struct sock *sk, int nonagle);
__asm__(".equ tcp_write_xmit," __stringify(HAVE_TCP_WRITE_XMIT_ADDR));
__asm__(".global tcp_write_xmit");
EXPORT_SYMBOL_GPL(tcp_write_xmit);
#endif				/* HAVE_TCP_WRITE_XMIT_ADDR */

#ifdef HAVE_TCP_CWND_APPLICATION_LIMITED_ADDR
void tcp_cwnd_application_limited(struct sock *sk);
__asm__(".equ tcp_cwnd_application_limited," __stringify(HAVE_TCP_CWND_APPLICATION_LIMITED_ADDR));
__asm__(".global tcp_cwnd_application_limited");
EXPORT_SYMBOL_GPL(tcp_cwnd_application_limited);
#endif				/* HAVE_TCP_CWND_APPLICATION_LIMITED_ADDR */

#ifdef HAVE_SYSCTL_TCP_FIN_TIMEOUT_ADDR
__asm__(".equ sysctl_tcp_fin_timeout," __stringify(HAVE_SYSCTL_TCP_FIN_TIMEOUT_ADDR));
__asm__(".global sysctl_tcp_fin_timeout");
EXPORT_SYMBOL_GPL(sysctl_tcp_fin_timeout);
#endif				/* HAVE_SYSCTL_TCP_FIN_TIMEOUT_ADDR */

#ifdef HAVE_TCP_CURRENT_MSS_ADDR
unsigned int tcp_current_mss(struct sock *sk, int large);
__asm__(".equ tcp_current_mss," __stringify(HAVE_TCP_CURRENT_MSS_ADDR));
__asm__(".global tcp_current_mss");
EXPORT_SYMBOL_GPL(tcp_current_mss);
#endif				/* HAVE_TCP_CURRENT_MSS_ADDR */

#ifdef HAVE_TCP_SET_SKB_TSO_SEGS_ADDR
#ifdef HAVE_KFUNC_TCP_SET_SKB_TSO_SEGS_SOCK
void tcp_set_skb_tso_segs(struct sock *sk, struct sk_buff *skb);
#else
void tcp_set_skb_tso_segs(struct sk_buff *skb, unsigned int mss_std);
#endif
__asm__(".equ tcp_set_skb_tso_segs," __stringify(HAVE_TCP_SET_SKB_TSO_SEGS_ADDR));
__asm__(".global tcp_set_skb_tso_segs");
EXPORT_SYMBOL_GPL(tcp_set_skb_tso_segs);
#endif				/* HAVE_TCP_SET_SKB_TSO_SEGS_ADDR */

#ifdef HAVE_TCP_SET_SKB_TSO_FACTOR_ADDR
/* older 2.6.8 name for the same function */
void tcp_set_skb_tso_factor(struct sk_buff *skb, unsigned int mss_std);
__asm__(".equ tcp_set_skb_tso_factor," __stringify(HAVE_TCP_SET_SKB_TSO_FACTOR_ADDR));
__asm__(".global tcp_set_skb_tso_factor");
EXPORT_SYMBOL_GPL(tcp_set_skb_tso_factor);
#endif				/* HAVE_TCP_SET_SKB_TSO_FACTOR_ADDR */

#ifdef HAVE_SOCK_ALLOC_ADDR
struct socket *sock_alloc(void);
__asm__(".equ sock_alloc," __stringify(HAVE_SOCK_ALLOC_ADDR));
__asm__(".global sock_alloc");
EXPORT_SYMBOL_GPL(sock_alloc);
#endif				/* HAVE_SOCK_ALLOC_ADDR */

#ifdef HAVE_SYSCTL_LOCAL_PORT_RANGE_ADDR
extern int sysctl_local_port_range[2];
__asm__(".equ sysctl_local_port_range," __stringify(HAVE_SYSCTL_LOCAL_PORT_RANGE_ADDR));
__asm__(".global sysctl_local_port_range");
EXPORT_SYMBOL_GPL(sysctl_local_port_range);
#endif				/* HAVE_SYSCTL_LOCAL_PORT_RANGE_ADDR */

#ifdef HAVE_TCP_MEMORY_ALLOCATED_ADDR
extern atomic_t tcp_memory_allocated;
__asm__(".equ tcp_memory_allocated," __stringify(HAVE_TCP_MEMORY_ALLOCATED_ADDR));
__asm__(".global tcp_memory_allocated");
EXPORT_SYMBOL_GPL(tcp_memory_allocated);
#endif				/* HAVE_TCP_MEMORY_ALLOCATED_ADDR */

#ifdef HAVE_TCP_SOCKETS_ALLOCATED_ADDR
extern atomic_t tcp_sockets_allocated;
__asm__(".equ tcp_sockets_allocated," __stringify(HAVE_TCP_SOCKETS_ALLOCATED_ADDR));
__asm__(".global tcp_sockets_allocated");
EXPORT_SYMBOL_GPL(tcp_sockets_allocated);
#endif				/* HAVE_TCP_SOCKETS_ALLOCATED_ADDR */

#ifdef HAVE_TCP_ORPHAN_COUNT_ADDR
extern atomic_t tcp_orphan_count;
__asm__(".equ tcp_orphan_count," __stringify(HAVE_TCP_ORPHAN_COUNT_ADDR));
__asm__(".global tcp_orphan_count");
EXPORT_SYMBOL_GPL(tcp_orphan_count);
#endif				/* HAVE_TCP_ORPHAN_COUNT_ADDR */

#ifdef HAVE_TCP_TW_COUNT_ADDR
extern int tcp_tw_count;
__asm__(".equ tcp_tw_count," __stringify(HAVE_TCP_TW_COUNT_ADDR));
__asm__(".global tcp_tw_count");
EXPORT_SYMBOL_GPL(tcp_tw_count);
#endif				/* HAVE_TCP_TW_COUNT_ADDR */

#ifdef HAVE_IP_CMSG_RECV_ADDR
void ip_cmsg_recv(struct msghdr *msg, struct sk_buff *skb);
__asm__(".equ ip_cmsg_recv," __stringify(HAVE_IP_CMSG_RECV_ADDR));
__asm__(".global ip_cmsg_recv");
EXPORT_SYMBOL_GPL(ip_cmsg_recv);
#endif				/* HAVE_IP_CMSG_RECV_ADDR */

#ifdef HAVE_IP_CMSG_SEND_ADDR
/* int ip_cmsg_send(struct msghdr *msg, struct ipcm_cookie *ipc); */
__asm__(".equ ip_cmsg_send," __stringify(HAVE_IP_CMSG_SEND_ADDR));
__asm__(".global ip_cmsg_send");
EXPORT_SYMBOL_GPL(ip_cmsg_send);
#endif				/* HAVE_IP_CMSG_SEND_ADDR */

#ifdef HAVE_INET_BIND_ADDR
int inet_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len);
__asm__(".equ inet_bind," __stringify(HAVE_INET_BIND_ADDR));
__asm__(".global inet_bind");
EXPORT_SYMBOL_GPL(inet_bind);
#endif				/* HAVE_INET_BIND_ADDR */

#ifdef HAVE_INET_IOCTL_ADDR
int inet_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg);
__asm__(".equ inet_ioctl," __stringify(HAVE_INET_IOCTL_ADDR));
__asm__(".global inet_ioctl");
EXPORT_SYMBOL_GPL(inet_ioctl);
#endif				/* HAVE_INET_IOCTL_ADDR */

#ifdef HAVE_INET_GETNAME_ADDR
int inet_getname(struct socket *sock, struct sockaddr *uaddr, int *uaddr_len, int peer);
__asm__(".equ inet_getname," __stringify(HAVE_INET_GETNAME_ADDR));
__asm__(".global inet_getname");
EXPORT_SYMBOL_GPL(inet_getname);
#endif				/* HAVE_INET_GETNAME_ADDR */

#ifdef HAVE_INET_MULTI_GETNAME_ADDR
int inet_multi_getname(struct socket *sock, struct sockaddr *uaddr, int *uaddr_len, int peer);
__asm__(".equ inet_multi_getname," __stringify(HAVE_INET_MULTI_GETNAME_ADDR));
__asm__(".global inet_multi_getname");
EXPORT_SYMBOL_GPL(inet_multi_getname);
#endif				/* HAVE_INET_MULTI_GETNAME_ADDR */

#ifdef HAVE_SOCKET_GET_INFO_ADDR
int socket_get_info(char *buffer, char **start, off_t offset, int length);
__asm__(".equ socket_get_info," __stringify(HAVE_SOCKET_GET_INFO_ADDR));
__asm__(".global socket_get_info");
EXPORT_SYMBOL_GPL(socket_get_info);
#endif				/* HAVE_SOCKET_GET_INFO_ADDR */

#ifdef HAVE_AFINET_GET_INFO_ADDR
int afinet_get_info(char *buffer, char **start, off_t offset, int length);
__asm__(".equ afinet_get_info," __stringify(HAVE_AFINET_GET_INFO_ADDR));
__asm__(".global afinet_get_info");
EXPORT_SYMBOL_GPL(afinet_get_info);
#endif				/* HAVE_AFINET_GET_INFO_ADDR */

#ifdef HAVE_TCP_PROT_ADDR
extern struct proto tcp_prot;
__asm__(".equ tcp_prot," __stringify(HAVE_TCP_PROT_ADDR));
__asm__(".global tcp_prot");
EXPORT_SYMBOL_GPL(tcp_prot);
#endif				/* HAVE_TCP_PROT_ADDR */

#ifdef HAVE_UDP_PROT_ADDR
extern struct proto udp_prot;
__asm__(".equ udp_prot," __stringify(HAVE_UDP_PROT_ADDR));
__asm__(".global udp_prot");
EXPORT_SYMBOL_GPL(udp_prot);
#endif				/* HAVE_UDP_PROT_ADDR */

#ifdef HAVE_RAW_PROT_ADDR
extern struct proto raw_prot;
__asm__(".equ raw_prot," __stringify(HAVE_RAW_PROT_ADDR));
__asm__(".global raw_prot");
EXPORT_SYMBOL_GPL(raw_prot);
#endif				/* HAVE_RAW_PROT_ADDR */

#ifdef HAVE_SECURE_TCP_SEQUENCE_NUMBER_ADDR
extern __u32 secure_tcp_sequence_number(__u32 saddr, __u32 daddr, __u16 sport, __u16 dport);
__asm__(".equ secure_tcp_sequence_number," __stringify(HAVE_SECURE_TCP_SEQUENCE_NUMBER_ADDR));
__asm__(".global secure_tcp_sequence_number");
EXPORT_SYMBOL_GPL(secure_tcp_sequence_number);
#endif				/* HAVE_SECURE_TCP_SEQUENCE_NUMBER_ADDR */

#ifdef HAVE_IOCTL32_HASH_TABLE_ADDR
extern struct ioctl_trans *ioctl32_hash_table[];
__asm__(".equ ioctl32_hash_table," __stringify(HAVE_IOCTL32_HASH_TABLE_ADDR));
__asm__(".global ioctl32_hash_table");
EXPORT_SYMBOL_GPL(ioctl32_hash_table);
#endif				/* HAVE_IOCTL32_HASH_TABLE_ADDR */

#ifdef HAVE_IOCTL32_SEM_ADDR
extern struct rw_semaphore ioctl32_sem;
__asm__(".equ ioctl32_sem," __stringify(HAVE_IOCTL32_SEM_ADDR));
__asm__(".global ioctl32_sem");
EXPORT_SYMBOL_GPL(ioctl32_sem);
#endif				/* HAVE_IOCTL32_SEM_ADDR */

#if defined HAVE_MOUNT_SEM_ADDR
__asm__(".equ mount_sem," __stringify(HAVE_MOUNT_SEM_ADDR));
__asm__(".global mount_sem");
EXPORT_SYMBOL_GPL(mount_sem);
#endif

#ifdef HAVE_NAMESPACE_SEM_ADDR
__asm__(".equ namespace_sem," __stringify(HAVE_NAMESPACE_SEM_ADDR));
__asm__(".global namespace_sem");
extern struct rw_semaphore namespace_sem;
EXPORT_SYMBOL_GPL(namespace_sem);
#endif				/* HAVE_NAMESPACE_SEM_ADDR */

#ifdef HAVE_CLONE_MNT_ADDR
extern struct vfsmount *clone_mnt(struct vfsmount *old, struct dentry *root);
__asm__(".equ clone_mnt," __stringify(HAVE_CLONE_MNT_ADDR));
__asm__(".global clone_mnt");
EXPORT_SYMBOL_GPL(clone_mnt);
#endif				/* defined HAVE_CLONE_MNT_ADDR */

#ifdef HAVE_CHECK_MNT_ADDR
int check_mnt(struct vfsmount *mnt);
__asm__(".equ check_mnt," __stringify(HAVE_CHECK_MNT_ADDR));
__asm__(".global check_mnt");
EXPORT_SYMBOL_GPL(check_mnt);
#endif				/* HAVE_CHECK_MNT_ADDR */

#ifdef HAVE_GRAFT_TREE_ADDR
int graft_tree(struct vfsmount *mnt, struct nameidata *nd);
__asm__(".equ graft_tree," __stringify(HAVE_GRAFT_TREE_ADDR));
__asm__(".global graft_tree");
EXPORT_SYMBOL_GPL(graft_tree);
#endif				/* HAVE_GRAFT_TREE_ADDR */

#ifdef HAVE_DO_UMOUNT_ADDR
extern int do_umount(struct vfsmount *mnt, int flags);

__asm__(".equ do_umount," __stringify(HAVE_DO_UMOUNT_ADDR));
__asm__(".global do_umount");
EXPORT_SYMBOL_GPL(do_umount);
#endif				/* HAVE_DO_UMOUNT_ADDR */

#ifdef HAVE__DEF_FIFO_OPS_ADDR
extern const struct file_operations *_def_fifo_ops;

__asm__(".equ _def_fifo_ops," __stringify(HAVE__DEF_FIFO_OPS_ADDR));
__asm__(".global _def_fifo_ops");
EXPORT_SYMBOL_GPL(_def_fifo_ops);
#endif				/* HAVE__DEF_FIFO_OPS_ADDR */

#ifdef  HAVE___TCP_PUSH_PENDING_FRAMES_ADDR
#ifdef HAVE_OLD_SOCK_STRUCTURE
void __tcp_push_pending_frames(struct sock *sk, struct tcp_opt *tp, unsigned int cur_mss, int nonagle);
#else				/* defined HAVE_OLD_SOCK_STRUCTURE */
#ifndef HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS
void __tcp_push_pending_frames(struct sock *sk, struct tcp_sock *tp, unsigned int cur_mss, int nonagle);
#else				/* !defined HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS */
void __tcp_push_pending_frames(struct sock *sk, unsigned int cur_mss, int nonagle);
#endif				/* !defined HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS */
#endif				/* defined HAVE_OLD_SOCK_STRUCTURE */
__asm__(".equ __tcp_push_pending_frames," __stringify(HAVE___TCP_PUSH_PENDING_FRAMES_ADDR));
__asm__(".global __tcp_push_pending_frames");
EXPORT_SYMBOL_GPL(__tcp_push_pending_frames);
#endif				/* defined HAVE___TCP_PUSH_PENDING_FRAMES_ADDR */

