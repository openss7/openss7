dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.12 2004/12/22 12:11:51 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (C) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
dnl
dnl All Rights Reserved.
dnl
dnl Permission is granted to make and distribute verbatim copies of this
dnl README file provided the copyright notice and this permission notice are
dnl preserved on all copies.
dnl 
dnl Permission is granted to copy and distribute modified versions of this
dnl manual under the conditions for verbatim copying, provided that the
dnl entire resulting derived work is distributed under the terms of a
dnl permission notice identical to this one
dnl 
dnl Since the Linux kernel and libraries are constantly changing, this README
dnl file may be incorrect or out-of-date.  The author(s) assume no
dnl responsibility for errors or omissions, or for damages resulting from the
dnl use of the information contained herein.  The author(s) may not have
dnl taken the same level of care in the production of this manual, which is
dnl licensed free of charge, as they might when working professionally.
dnl 
dnl Formatted or processed versions of this manual, if unaccompanied by the
dnl source, must acknowledge the copyright and authors of this work.
dnl 
dnl =========================================================================
dnl 
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions
dnl apply to you.  If the Software is supplied by the Department of Defense
dnl ("DoD"), it is classified as "Commercial Computer Software" under
dnl paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
dnl Regulations ("DFARS") (or any successor regulations) and the Government is
dnl acquiring only the license rights granted herein (the license rights
dnl customarily provided to non-Government users).  If the Software is
dnl supplied to any unit or agency of the Government other than DoD, it is
dnl classified as "Restricted Computer Software" and the Government's rights
dnl in the Software are defined in paragraph 52.227-19 of the Federal
dnl Acquisition Regulations ("FAR") (or any success regulations) or, in the
dnl cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl 
dnl =========================================================================
dnl 
dnl Commercial licensing and support of this software is available from
dnl OpenSS7 Corporation at a fee.  See http://www.openss7.com/
dnl 
dnl =========================================================================
dnl
dnl Last Modified $Date: 2004/12/22 12:11:51 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/openss7.m4])
m4_include([m4/kernel.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/autotest.m4])

# =========================================================================
# AC_SCTP
# -------------------------------------------------------------------------
AC_DEFUN([AC_SCTP], [dnl
    _OPENSS7_PACKAGE([SCTP], [OpenSS7 Linux Native SCTP])
    _SCTP_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    SCTP_INCLUDES="-I- -imacros ./config.h"
    AC_SUBST([SCTP_INCLUDES])dnl
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _LINUX_KERNEL
    _SCTP_SETUP
    AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user LDFLAGS   = $USER_LDFLAGS])
    AC_MSG_NOTICE([final user INCLUDES  = $SCTP_INCLUDES])
    AC_MSG_NOTICE([final kern MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kern NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kern CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kern CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final kern LDFLAGS   = $KERNEL_LDFLAGS])
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    CPPFLAGS=
    CFLAGS=
dnl _AUTOTEST
    _SCTP_OUTPUT dnl
])# AC_SCTP
# =========================================================================

# =========================================================================
# _SCTP_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPTIONS], [dnl
])# _SCTP_OPTIONS
# =========================================================================

# =========================================================================
# _SCTP_SETUP_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP_DEBUG], [dnl
    AC_DEFINE_UNQUOTED([CONFIG_SCTP_MODULE], [], [Always define this for the
                        autoconf configuration.])
    case "$linux_cv_debug" in
    _DEBUG)
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_DEBUG], [], [Define to perform
                            internal structure tracking within SCTP as well as
                            to provide additional /proc filesystem files for
                            examining internal structures.])
        ;;
    _TEST)
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_TEST], [], [Define to perform
                            performance testing with debugging.  This mode
                            does not dump massive amounts of information into
                            system logs, but peforms all assertion checks.])
        ;;
    _SAFE)
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_SAFE], [], [Define to perform
                            fundamental assertion checks.  This is a safer
                            mode of operation.])
        ;;
    _NONE | *)
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_NONE], [], [Define to perform no
                            assertion checks but report software errors.  This
                            is the smallest footprint, highest performance
                            mode of operation.])
        ;;
    esac
])# _SCTP_SETUP_DEBUG
# =========================================================================

# =========================================================================
# _SCTP_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP], [dnl
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _SCTP_SETUP_MODULE
    _SCTP_CHECK_KERNEL
    _SCTP_SETUP_DEBUG
])# _SCTP_SETUP
# =========================================================================

# =========================================================================
# _SCTP_SETUP_MODULE
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP_MODULE], [dnl
    if test :"${linux_cv_modules:-yes}" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MODULE], [], [When defined, STREAMS
            is being compiled as a loadable kernel module.])
    else
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS], [], [When defined, STREAMS is
            being compiled as a kernel linkable object.])
    fi
])# _SCTP_SETUP_MODULE
# =========================================================================

# =========================================================================
# _SCTP_CHECK_KERNEL
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_CHECK_KERNEL], [dnl
    sctp_cv_other_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp compiled in], [CONFIG_IP_SCTP], [
        sctp_cv_other_sctp='yes'
        AC_MSG_ERROR([
**** 
**** Configure has detected a kernel with the deprecated lksctp compiled in.
**** Ths is NOT a recommended situation.  Installing OpenSS7 SCTP on such a
**** bastadized kernel will most likely result in an unstable situation.  Try
**** a different kernel, or try recompiling your kernel with lksctp set to no.
**** ])
    ])
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp as module], [CONFIG_IP_SCTP_MODULE],
                               [sctp_cv_other_sctp='yes'])
    if test :$sctp_cv_other_sctp = :yes ; then
        AC_DEFINE_UNQUOTED([HAVE_LKSCTP_SCTP], [], [Some more recent 2.4.25
            and greater kernels have this poorman version of SCTP included in
            the kernel.  Define this symbol if you have such a bastardized
            kernel.  When we have such a kernel we need to define lksctp's
            header wrapper defines so that none of the lksctp header files are
            included (we use our own instead).])
    else
        _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp patched in], [CONFIG_SCTP],
                                   [sctp_cv_other_sctp='yes'])
        _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp as module], [CONFIG_SCTP_MODULE],
                                   [sctp_cv_other_sctp='yes'])
        if test :$sctp_cv_other_sctp = :yes ; then
            AC_DEFINE_UNQUOTED([HAVE_OPENSS7_SCTP], [], [Some kernels may
                already have the premium OpenSS7 implementation of SCTP
                included.  A number of kernel rpms were released this way and
                some distros have picked up OpenSS7 patches.  If this is the
                case you are done.])dnl
        fi
    fi
    _LINUX_KERNEL_ENV([dnl
        AC_CACHE_CHECK([for kernel ip_route_output], [linux_cv_have_ip_route_output], [dnl
            CFLAGS="$CFLAGS -Werror-implicit-function-declaration"
            AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[
#include <linux/config.h>
#include <linux/version.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>]],
                [[ip_route_output(NULL, 0, 0, 0, 0);]]) ],
                    [linux_cv_have_ip_route_output='yes'],
                    [linux_cv_have_ip_route_output='no'])
            ])
        if test :$linux_cv_have_ip_route_output = :yes ; then
            AC_DEFINE_UNQUOTED([HAVE_IP_ROUTE_OUTPUT_EXPLICIT], [], [Define if you have the explicit
                version of ip_route_output.])
        fi
    ])
    _LINUX_CHECK_HEADERS([net/xfrm.h net/dst.h], [], [], [
#include <linux/config.h>
#include <linux/sysctl.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/ipsec.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <net/protocol.h>
#include <net/inet_common.h>
    ])
    _LINUX_CHECK_TYPES([struct sockaddr_storage,
                        struct sctp_mib], [], [], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/snmp.h>
    ])
    _LINUX_CHECK_MEMBERS([struct inet_protocol.protocol,
                          struct inet_protocol.no_policy,
                          struct dst_entry.path,
                          struct sk_buff.h.sh,
                          struct sock.protinfo.af_inet.ttl,
                          struct sock.protoinfo.af_inet.uc_ttl,
                          struct sock.tp_pinfo.af_sctp], [], [], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/protocol.h>
#ifdef HAVE_NET_DST_H
#include <net/dst.h>
#endif
    ])
    _LINUX_KERNEL_SYMBOLS([icmp_err_convert,
                           icmp_statistics,
                           inet_bind,
                           inet_getname,
                           inet_ioctl,
                           inet_multi_getname,
                           ip_cmsg_send,
                           ip_getsockopt,
                           ip_route_output_flow,
                           __ip_route_output_key,
                           ip_rt_min_pmtu,
                           ip_rt_mtu_expires,
                           ip_rt_update_pmtu,
                           ipsec_sk_policy,
                           ip_setsockopt,
                           sysctl_ip_dynaddr,
                           sysctl_ip_nonlocal_bind,
                           __xfrm_policy_check,
                           xfrm_policy_delete,
                           __xfrm_sk_clone_policy])
    _LINUX_KERNEL_ENV([dnl
        AC_CACHE_CHECK([for kernel struct sk_buff.cb size], [linux_cv_sk_buff_cb_size], [dnl
            CPPFLAGS="$CPPFLAGS -I. -I$srcdir -I./src -I$srcdir/src $SCTP_INCLUDES $KERNEL_CPPFLAGS $KERNEL_MODFLAGS"
            AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include "include/net/net_sock.h"
#include "include/linux/linux_sctp.h"
#include "include/net/net_sctp.h"
                ]], [[
struct sk_buff system_sk_buff_structure;
struct __os7_sctp_skb_cb mycb;
if (sizeof(system_sk_buff_structure.cb) < sizeof(mycb)) {
    invoke_some_function_that_does_not_exist(NULL, 0, 0);
}]]) ],
                [if ( objdump -r conftest.$ac_objext | grep -q invoke_some_function_that_does_not_exist ) 2>/dev/null
                 then
                     linux_cv_sk_buff_cb_size='toosmall'
                 else
                     linux_cv_sk_buff_cb_size='ok'
                 fi],
                [linux_cv_sk_buff_cb_size='unknown'])
            ])
    ])
    case :${linux_cv_sk_buff_cb_size:-unknown} in
        :ok)
            ;;
        :toosmall)
            AC_MSG_ERROR([
**** 
**** 
**** ERROR: The size of the control block (cb) in struct sk_buff is plain too
**** small to accept the size of the sctp_skb_cb.  There is nothing that can
**** be done about this as the size of the cb is compiled into the kernel.
**** You have no choice but to increase the size and recompile your kernel
**** before attempting to rebuild SCTP.
**** ])
            ;;
        *)
            AC_MSG_WARN([
**** 
**** WARNING: Configure cannot determine whether the size of your sk_buff
**** control block is large enough to accomodate the sctp_skb_cb structure.
**** There is a check which prohibits the kernel module from loading if that
**** winds up being the case.  If your module does not load later or fails a
**** depmod -ae, due to an unresolved symbol with a peculiar name, then you
**** may need to recompile your kernel with a larger cb size and rebuild and
**** reinstall SCTP on the new kernel.
**** ])
            ;;
    esac
    _LINUX_KERNEL_ENV([dnl
        AC_CACHE_CHECK([for kernel struct sock.tp_pinfo size], [linux_cv_sock_tp_pinfo_size], [dnl
            CPPFLAGS="$CPPFLAGS -I. -I$srcdir -I./src -I$srcdir/src $SCTP_INCLUDES $KERNEL_CPPFLAGS $KERNEL_MODFLAGS"
            AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include "include/net/net_sock.h"
#include "include/linux/linux_sctp.h"
#include "include/net/net_sctp.h"
                ]], [[
extern size_t the_size_of_tcp_opt;
extern size_t the_size_of_os7_sctp_opt;
struct sock system_sock_structure;
struct __os7_sctp_opt myopt;
if (sizeof(system_sock_structure.tp_pinfo) < sizeof(myopt)) {
    the_size_of_tcp_opt = sizeof(struct tcp_opt);
    the_size_of_os7_sctp_opt = sizeof(struct __os7_sctp_opt);
    invoke_some_function_that_does_not_exist(NULL, 0, 0);
}
                ]])
                ], [dnl
    if ( objdump -r conftest.$ac_objext | grep -q invoke_some_function_that_does_not_exist ) 2>/dev/null
    then
        linux_cv_sock_tp_pinfo_size='toosmall'
        cp -f conftest.$ac_objext error_object.$ac_objext
    else
        linux_cv_sock_tp_pinfo_size='ok'
    fi], [dnl
        linux_cv_sock_tp_pinfo_size='unknown'])
        ])
    ])
    case :$linux_cv_sock_tp_pinfo_size in
        :ok)
            ;;
        :toosmall)
            # Another question, is lksctp bigger than tcp?
            AC_MSG_ERROR([
**** 
**** ERROR: The size of the socket private information union (tp_pinfo) in
**** struct sock is plain too small to accept the size of the sctp_opt private
**** structure.  There is nothing that can be done about this as the size of
**** the private information union is compiled into the kernel.  You have no
**** choice but to increase the size and recompile your kernel before
**** attempting to rebuild SCTP.
**** ])
            ;;
        *)
            AC_MSG_WARN([
**** 
**** WARNING: Configure cannot determine whether the size of your socket
**** private information union is large enough to accomodate the sctp_opt
**** structure.  There is a check which prohibits the kernel module from
**** loading if that winds up being the case.  If your module does not load
**** later or failes a depmod -ae, due to an unresolved symbol with a peculiar
**** name, then you may need to recompile your kernel with a larger tp_pinfo
**** size and rebuild and reinstall SCTP on the new kernel.
**** ])
            ;;
    esac
    if test :$linux_cv_type_struct_sctp_mib = :yes ; then
        _LINUX_CHECK_MEMBERS([struct sctp_mib.SctpRtoAlgorithm,
                              struct sctp_mib.SctpRtoMin,
                              struct sctp_mib.SctpRtoMax,
                              struct sctp_mib.SctpRtoInitial,
                              struct sctp_mib.SctpMaxAssoc,
                              struct sctp_mib.SctpValCookieLife,
                              struct sctp_mib.SctpMaxInitRetr,
                              struct sctp_mib.SctpCurrEstab,
                              struct sctp_mib.SctpActiveEstabs,
                              struct sctp_mib.SctpPassiveEstabs,
                              struct sctp_mib.SctpAborteds,
                              struct sctp_mib.SctpShutdowns,
                              struct sctp_mib.SctpOutOfBlues,
                              struct sctp_mib.SctpChecksumErrors,
                              struct sctp_mib.SctpOutCtrlChunks,
                              struct sctp_mib.SctpOutOrderChunks,
                              struct sctp_mib.SctpOutUnorderChunks,
                              struct sctp_mib.SctpInCtrlChunks,
                              struct sctp_mib.SctpInOrderChunks,
                              struct sctp_mib.SctpInUnorderChunks,
                              struct sctp_mib.SctpFragUsrMsgs,
                              struct sctp_mib.SctpReasmUsrMsgs,
                              struct sctp_mib.SctpOutSCTPPacks,
                              struct sctp_mib.SctpInSCTPPacks,
                              struct sctp_mib.SctpDiscontinuityTime], [], [], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/snmp.h>
                              ])
    fi
dnl _LINUX_KERNEL_SYMBOL_EXPORT([add_wait_queue])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([add_wait_queue_exclusive])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([alloc_skb])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([create_proc_entry])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([del_timer])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([dev_base])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([do_softirq])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([free_pages])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__generic_copy_to_user])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__get_free_pages])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__get_user_4])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([icmp_err_convert])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([icmp_statistics])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_accept])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_add_protocol])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_addr_type])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_bind])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_del_protocol])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_family_ops])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_getname])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_getsockopt])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_ioctl])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_recvmsg])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_register_protosw])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_release])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_sendmsg])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_setsockopt])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_shutdown])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_stream_connect])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([inet_unregister_protosw])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_cmsg_recv])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_fragment])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_getsockopt])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_route_output_key])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_update_pmtu])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__ip_select_ident])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_send_check])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_setsockopt])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([irq_stat])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([jiffies])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kfree])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__kfree_skb])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kill_pg])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kill_proc])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kmem_cache_alloc])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kmem_cache_create])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kmem_cache_free])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([kmem_find_general_cachep])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__lock_sock])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([mod_timer])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([net_statistics])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([nf_hooks])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([nf_hook_slow])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([num_physpages])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__out_of_line_bug])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([panic])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__pollwait])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([printk])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([proc_net])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([___pskb_trim])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([put_cmsg])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__release_sock])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([remove_proc_entry])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([remove_wait_queue])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([schedule_timeout])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([secure_tcp_sequence_number])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([send_sig])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sk_alloc])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([skb_clone])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([skb_copy_datagram_iovec])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([skb_linearize])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([skb_over_panic])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([skb_realloc_headroom])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([skb_under_panic])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sk_free])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sk_run_filter])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sock_no_mmap])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sock_no_sendpage])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sock_no_socketpair])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sock_wake_async])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sock_wfree])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sprintf])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([__wake_up])
])# _SCTP_CHECK_KERNEL
# =========================================================================

# =========================================================================
# _SCTP_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OUTPUT], [dnl
    _SCTP_CONFIG
])# _SCTP_OUTPUT
# =========================================================================

# =========================================================================
# _SCTP_CONFIG
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_CONFIG], [dnl
# CONFIG_SCTP_SLOW_VERIFICATION
    AC_MSG_CHECKING([for sctp slow verification])
    AC_ARG_ENABLE([sctp-slow-verification],
        AS_HELP_STRING([--enable-sctp-slow-verification],
            [enable slow verification of addresses and tags. @<:@default=no@:>@]),
        [sctp_cv_slow_verification="$enableval"],
        [sctp_cv_slow_verification='no'])
    if test :"${sctp_cv_slow_verification:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_SLOW_VERIFICATION], [], [
            When a message comes from an SCTP endpoint with the correct
            verification tag, it is not necessary to check ports or addresses
            to identify the SCTP association to which it belongs.  When
            undefined, port numbers and addresses are not checked on local
            tags and the addresses are not checked on peer tags.  When
            defined, the redundant port number and destination address checks
            are performed.  Both situations provide RFC 2960 compliant
            operation.  If in doubt, leave this undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_slow_verification])
# CONFIG_SCTP_THROTTLE_HEARTBEATS
    AC_MSG_CHECKING([for sctp throttle heartbeats])
    AC_ARG_ENABLE([sctp-throttle-heartbeats],
        AS_HELP_STRING([--enable-sctp-throttle-heartbeats],
            [enable heartbeat throttling. @<:@default=no@:>@]),
        [sctp_cv_throttle_heartbeats="$enableval"],
        [sctp_cv_throttle_heartbeats='no'])
    if test :"${sctp_cv_throttle_heartbeats:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_THROTTLE_HEARTBEATS], [], [
            Special feature of OpenSS7 SCTP which is not mentioned in RFC
            2960.  When defined, SCTP will throttle the rate at which it
            responds to heartbeats to the system control sctp_heartbeat_itvl.
            This makes SCTP more reslilient to implementations which flood
            heartbeat messages.  For RFC 2960 compliant operation, leave this
            undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_throttle_heartbeats])
    AC_MSG_CHECKING([for sctp dicard out-of-the-blue])
    AC_ARG_ENABLE([sctp-discard-ootb],
        AS_HELP_STRING([--enable-sctp-discard-ootb],
            [enable discard out-of-the-blue packets. @<:@default=no@:>@]),
        [sctp_cv_discard_ootb="$enableval"],
        [if test :"${sctp_cv_openss7_sctp:-no}" = :yes ; then
            sctp_cv_discard_ootb='yes'
         else
            sctp_cv_discard_ootb='no'
         fi])
    if test :"$sctp_cv_discard_ootb" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_DISCARD_OOTB], [], [
            RFC 2960 requires the implementation to send ABORT to some OOTB
            packets (packets for which no SCTP association exists).  Sending
            ABORT chunks to unverified source addreses with the T bit set
            opens SCTP to blind masquerade attacks.  Not sending them may lead
            to delays at the peer endpoint aborting associations where our
            ABORT ahs been lost and the stream is already closes or if we have
            restarted and the peer still has open associations to us.  If
            defined, SCTP will discard all OOTB packets.  This is necessary if
            another SCTP stack is being run on the same machine.  For RFC 2960
            compliant operation, leave undefined.  If in doubt, leave this
            undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_discard_ootb])
# CONFIG_SCTP_EXTENDED_IP_SUPPORT
    AC_MSG_CHECKING([for sctp extended ip support])
    AC_ARG_ENABLE([sctp-extended-ip-support],
        AS_HELP_STRING([--enable-sctp-extended-ip-support],
            [enable extended IP support for SCTP. @<:@default=disabled@:>@]),
        [sctp_cv_extended_ip_support="$enableval"],
        [sctp_cv_extended_ip_support='no'])
    if test :"${sctp_cv_extended_ip_support:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_EXTENDED_IP_SUPPORT], [], [
            This provides extended IP support for SCTP for things like IP
            Transparent Proxy and IP Masquerading.  This is experimental
            stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_extended_ip_support])
    AC_C_BIGENDIAN(
        [sctp_cv_be_machine='yes'; sctp_cv_le_machine='no'],
        [sctp_cv_be_machine='no'; sctp_cv_le_machine='yes'],
        [sctp_cv_be_machine='yes'; sctp_cv_le_machine='yes'])
# CONFIG_SCTP_HMAC_SHA1
    AC_MSG_CHECKING([for sctp hmac sha-1])
    AC_ARG_ENABLE([sctp-hmac-sha1],
        AS_HELP_STRING([--disable-sctp-hmac-sha1],
            [disable SHA-1 HMAC. @<:@default=enabled(BE),disabled(LE)@:>@]),
        [sctp_cv_hmac_sha1="$enableval"],
        [sctp_cv_hmac_sha1="$sctp_cv_be_machine"])
    if test :"$sctp_cv_hmac_sha1" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_HMAC_SHA1], [], [
            When defined, this provides the ability to sue the FIPS 180-1
            (SHA-1) message authentication code in SCTP cookies.  When
            defined and the appropriate sysctl and option is set, SCTP will
            use SHA-1 HMAC when signing cookies in the INIT-ACK chunk.  If
            undefined, the SHA-1 HMAC will be unavailable for use with SCTP.
            If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_SHA1], [test :"$sctp_cv_hmac_sha1" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_hmac_sha1])
# CONFIG_SCTP_HMAC_MD5
    AC_MSG_CHECKING([for sctp hmac md5])
    AC_ARG_ENABLE([sctp-hmac-md5],
        AS_HELP_STRING([--disable-sctp-hmac-md5],
            [disable MD5 HMAC. @<:@default=enabled(LE),disabled(BE)@:>@]),
        [sctp_cv_hmac_md5="$enableval"],
        [sctp_cv_hmac_md5="$sctp_cv_le_machine"])
    if test :"$sctp_cv_hmac_md5" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_HMAC_MD5], [], [
            When defined, this provides the ability to use the MD5 (RFC 1321)
            message authentication code in SCTP cookies.  If you define this
            macro, when the appropriate system control and stream option is
            set, SCTP will use the MD5 HMAC when signing cookies in the
            INIT-ACK chunk.  If this macro is undefined, the HD5 HMAC will be
            unavailable for use with SCTP.  If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_MD5], [test :"$sctp_cv_hmac_md5" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_hmac_md5])
# CONFIG_SCTP_ADLER_32
    AC_MSG_CHECKING([for sctp Adler32 checksum])
    AC_ARG_ENABLE([sctp-adler32],
        AS_HELP_STRING([--enable-sctp-adler32],
            [enable Adler32 checksum. @<:@default=disabled@:>@]),
        [sctp_cv_adler32="$enableval"],
        [sctp_cv_adler32='no'])
    if test :"$sctp_cv_adler32" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_ADLER_32], [], [
            This provides the ability to use the older RFC 2960 Adler32
            checksum.  If CONFIG_SCTP_CRC_32 below is not selected, the Adler32
            checksum is always provided.])dnl
    fi
    AM_CONDITIONAL([WITH_ADLER_32], [test :"$sctp_cv_adler32" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_adler32])
# CONFIG_SCTP_CRC_32C
    AC_MSG_CHECKING([for sctp CRC-32C checksum])
    AC_ARG_ENABLE([sctp-crc32c],
        AS_HELP_STRING([--disable-sctp-crc32c],
            [disable CRC-32C checksum. @<:@default=enabled@:>@]),
        [sctp_cv_crc32c="$enableval"],
        [sctp_cv_crc32c='yes'])
    if test :"${sctp_cv_adler32:-no}" != :yes ; then
        sctp_cv_crc32c='yes'
    fi
    if test :"$sctp_cv_crc32c" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_CRC_32C], [], [
            This provides the ability to use the newer CRC-32c checksum as
            described in RFC 3309.  When this is selected and
            CONFIG_SCTP_ADLER_32 is not selected above, then the only checksum
            that will be used is the CRC-32c checksum.])dnl
    fi
    AM_CONDITIONAL([WITH_CRC_32C], [test :"$sctp_cv_crc32c" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_crc32c])
# CONFIG_SCTP_THROTTLE_PASSIVEOPENS
    AC_MSG_CHECKING([for sctp throttle passive opens])
    AC_ARG_ENABLE([sctp-throttle-passiveopens],
        AS_HELP_STRING([--enable-sctp-throttle-passiveopens],
            [enable throttling of passive opens. @<:@default=disabled@:>@]),
        [sctp_cv_throttle_passiveopens="$enableval"],
        [sctp_cv_throttle_passiveopens='no'])
    if test :"$sctp_cv_throttle_passiveopens" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_THROTTLE_PASSIVEOPENS], [], [
            Special feature of Linux SCTP not mentioned in RFC 2960.  When
            secure algorithms are used for signing cookies, the implementation
            becomes vulnerable to INIT and COOKIE ECHO flooding.  If defined,
            SCTP will only allow one INIT and one COOKE ECHO to be processed in
            each interval corresponding to the sysctl sctp_throttle_itvl.
            Setting sctp_throttle_itvl to 0 defeats this function.  If
            undefined, each INIT and COOKIE ECHO will be processed.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_throttle_passiveopens])
# CONFIG_SCTP_ECN
    AC_MSG_CHECKING([for sctp ecn])
    AC_ARG_ENABLE([sctp-ecn],
        AS_HELP_STRING([--enable-sctp-ecn],
            [enable Explicit Congestion Notification. @<:@default=disabled@:>@]),
        [sctp_cv_ecn="$enableval"],
        [sctp_cv_ecn='no'])
    if test :"$sctp_cv_ecn" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_ECN], [], [
            This enables support for Explicit Congestion Notification (ECN)
            chunks in SCTP messages as defined in RFC 2960 and RFC 3168.  It
            also adds syctl (/proc/net/ipv4/sctp_ecn) which allows ECN for SCTP
            to be disabled at runtime.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_ecn])
# CONFIG_SCTP_LIFETIMES
    AC_MSG_CHECKING([for sctp lifetimes])
    AC_ARG_ENABLE([sctp-lifetimes],
        AS_HELP_STRING([--enable-sctp-lifetimes],
            [enable SCTP message lifetimes. @<:@default=disabled@:>@]),
        [sctp_cv_lifetimes="$enableval"],
        [sctp_cv_lifetimes='no'])
    if test :"$sctp_cv_lifetimes" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_LIFETIMES], [], [
            This enables support for message lifetimes as described in RFC 2960.
            When enabled, message lifetimes can be set on messages.  See
            sctp(7).  This feature is always enabled when Partial Reliability
            Support is set.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_lifetimes])
# CONFIG_SCTP_ADD_IP
    AC_MSG_CHECKING([for sctp add ip])
    AC_ARG_ENABLE([sctp-add-ip],
        AS_HELP_STRING([--enable-sctp-add-ip],
            [enable ADD-IP. @<:@default=disabled@:>@]),
        [sctp_cv_add_ip="$enableval"],
        [sctp_cv_add_ip='no'])
    if test :"$sctp_cv_add_ip" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_ADD_IP], [], [
            This enables support for ADD-IP as described in
            draft-ietf-tsvwg-addip-sctp-07.txt.  This allows the addition and
            removal of IP addresses from existing connections.  This is
            experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_add_ip])
# CONFIG_SCTP_ADAPTATION_LAYER_INFO
    AC_MSG_CHECKING([for sctp adaptation layer info])
    AC_ARG_ENABLE([sctp-adaptation-layer-info],
        AS_HELP_STRING([--enable-sctp-adaptation-layer-info],
            [enable ALI. @<:@default=disabled@:>@]),
        [sctp_cv_adaptation_layer_info="$enableval"],
        [sctp_cv_adaptation_layer_info='no'])
    if test :"$sctp_cv_adaptation_layer_info" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_ADAPTATION_LAYER_INFO], [], [
            This enables support for the Adaptation Layer Information parameter
            described in draft-ietf-tsvwg-addip-sctp-07.txt for communicating
            application layer information bits at initialization.  This is
            experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_adaptation_layer_info])
# CONFIG_SCTP_PARTIAL_RELIABILITY
    AC_MSG_CHECKING([for sctp partial reliability])
    AC_ARG_ENABLE([sctp-partial-reliability],
        AS_HELP_STRING([--enable-sctp-partial-reliability],
            [enable SCTP Partial Reliability (PR-SCTP). @<:@default=disabled@:>@]),
        [sctp_cv_partial_reliability="$enableval"],
        [sctp_cv_partial_reliability='no'])
    if test :"$sctp_cv_partial_reliability" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_PARTIAL_RELIABILITY], [], [
            This enables support for PR-SCTP as described in
            draft-stewart-tsvwg-prsctp-03.txt.  This allows for partial
            reliability of message delivery on a "timed reliability" basis.
            This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_partial_reliability])
# CONFIG_SCTP_ERROR_GENERATOR
    AC_MSG_CHECKING([for sctp error generator])
    AC_ARG_ENABLE([sctp-error-generator],
        AS_HELP_STRING([--disable-sctp-error-generator],
            [disable SCTP error generator. @<:@default=enabled@:>@]),
        [sctp_cv_error_generator="$enableval"],
        [sctp_cv_error_generator='yes'])
    if test :"$sctp_cv_error_generator" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_SCTP_ERROR_GENERATOR], [], [
            This provides an internal error generator that can be accessed with
            socket options for testing SCTP operation under packet loss.  You
            will need this option to run some of the test programs distributed
            with the SCTP module.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_error_generator])
])# _SCTP_CONFIG
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
