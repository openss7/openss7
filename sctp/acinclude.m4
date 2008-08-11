# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.62 $) $Date: 2008-04-29 08:49:49 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2008-04-29 08:49:49 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])

# =============================================================================
# AC_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([AC_SCTP], [dnl
    _OPENSS7_PACKAGE([SCTP], [OpenSS7 Linux Native SCTP])
    _SCTP_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/sctp-core.postinst
		     debian/sctp-core.postrm
		     debian/sctp-core.preinst
		     debian/sctp-core.prerm
		     debian/sctp-devel.preinst
		     debian/sctp-dev.postinst
		     debian/sctp-dev.preinst
		     debian/sctp-dev.prerm
		     debian/sctp-doc.postinst
		     debian/sctp-doc.preinst
		     debian/sctp-doc.prerm
		     debian/sctp-init.postinst
		     debian/sctp-init.postrm
		     debian/sctp-init.preinst
		     debian/sctp-init.prerm
		     debian/sctp-lib.preinst
		     debian/sctp-source.preinst
		     debian/sctp-util.preinst
		     Module.mkvars])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _SCTP_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
dnl PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/include -I${top_srcdir}/include'
dnl if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
dnl fi
dnl PKG_MODFLAGS='$(STREAMS_MODFLAGS)'
dnl Just check config.log if you want to see these...
dnl AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
dnl AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
dnl AC_MSG_NOTICE([final user    LDFLAGS   = $USER_LDFLAGS])
dnl AC_MSG_NOTICE([final package INCLUDES  = $PKG_INCLUDES])
dnl AC_MSG_NOTICE([final package MODFLAGS  = $PKG_MODFLAGS])
dnl AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
dnl AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
dnl AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
dnl AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
dnl AC_MSG_NOTICE([final kernel  LDFLAGS   = $KERNEL_LDFLAGS])
dnl AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
dnl AC_MSG_NOTICE([final streams MODFLAGS  = $STREAMS_MODFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([PKG_INCLUDES])dnl
    AC_SUBST([PKG_MODFLAGS])dnl
    PKG_MANPATH='$(mandir)'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _SCTP_OUTPUT dnl
    _AUTOTEST
])# AC_SCTP
# =============================================================================

# =============================================================================
# _SCTP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPTIONS], [dnl
])# _SCTP_OPTIONS
# =============================================================================

# =============================================================================
# _SCTP_OTHER_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OTHER_SCTP], [dnl
    sctp_cv_other_sctp='no'
    sctp_cv_lksctp_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp compiled in], [CONFIG_IP_SCTP])
    if test :"$linux_cv_CONFIG_IP_SCTP" = :"yes" ; then
	sctp_cv_other_sctp='lksctp'
	sctp_cv_lksctp_sctp='yes'
	AC_MSG_ERROR([
**** 
**** Configure has detected a kernel with the deprecated lksctp compiled in.
**** This is NOT a recommended situation.  Installing OpenSS7 STREAMS SCTP on
**** such a bastardized kernel will most likely result in an unstable
**** situation.  Try a different kernel, or try recompiling your kernel with
**** lksctp removed (or at least compiled as a module).
**** ])
    fi
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp as module], [CONFIG_IP_SCTP_MODULE])
    if test :"$linux_cv_CONFIG_IP_SCTP_MODULE" = :"yes" ; then
	sctp_cv_other_sctp='lksctp'
	sctp_cv_lksctp_sctp='yes'
	AC_DEFINE([HAVE_LKSCTP_SCTP], [1], [Some more recent 2.4.25 and
	    greater kernels have this poorman version of SCTP included in the
	    kernel.  Define this symbol if you have such a bastardized kernel.
	    When we have such a kernel we need to define lksctp's header
	    wrapper defines so that none of the lksctp header files are
	    included (we use our own instead).])
    fi
    sctp_cv_openss7_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp compiled in], [CONFIG_SCTP])
    if test :"$linux_cv_CONFIG_SCTP" = :"yes" ; then
	sctp_cv_other_sctp='openss7'
	sctp_cv_openss7_sctp='yes'
	AC_MSG_WARN([
**** 
**** Configure has detected a kernel with OpenSS7 SCTP compiled in.  This is
**** NOT a recommended situation.  Installing OpenSS7 STREAMS SCTP on such a
**** kernel can lead to difficulties.  Try a different kernel, or try
**** recompiling with OpenSS7 SCTP compiled as a module, and perhaps removed.
**** ])
    fi
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp module], [CONFIG_SCTP_MODULE])
    if test :"$linux_cv_CONFIG_SCTP_MODULE" = :"yes" ; then
	sctp_cv_other_sctp='openss7'
	sctp_cv_openss7_sctp='yes'
	AC_DEFINE([HAVE_OPENSS7_SCTP], [1], [Define if your kernel supports
	    the OpenSS7 Linux Kernel Sockets SCTP patches.  This enables
	    support in the SCTP driver for STREAMS on top of the OpenSS7 Linux
	    Kernel Sockets SCTP implementation.])
    fi
    AM_CONDITIONAL([WITH_LKSCTP_SCTP], [ test :"${sctp_cv_lksctp_sctp:-no}"  = :yes])dnl
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], [test :"${sctp_cv_openss7_sctp:-no}" = :yes])dnl
])# _SCTP_OTHER_SCTP
# =============================================================================

# =============================================================================
# _SCTP_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP], [dnl
    _LINUX_KERNEL
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _SCTP_OTHER_SCTP
    _SCTP_SETUP_MODULE
    _SCTP_CHECK_KERNEL
])# _SCTP_SETUP
# =============================================================================

# =============================================================================
# _SCTP_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP_MODULE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE([SCTP_CONFIG_MODULE], [1], [When defined, SCTP is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE([SCTP_CONFIG], [1], [When defined, SCTP is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([SCTP_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([SCTP_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _SCTP_SETUP_MODULE
# =============================================================================

# =============================================================================
# _SCTP_CHECK_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_CHECK_KERNEL], [dnl
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel ip_route_output], [linux_cv_have_ip_route_output], [dnl
	    CFLAGS="$CFLAGS -Werror-implicit-function-declaration"
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>]],
		[[ip_route_output(NULL, 0, 0, 0, 0);]]) ],
		    [linux_cv_have_ip_route_output='yes'],
		    [linux_cv_have_ip_route_output='no'])
	    ])
	if test :$linux_cv_have_ip_route_output = :yes ; then
	    AC_DEFINE([HAVE_IP_ROUTE_OUTPUT_EXPLICIT], [1], [Define if you have the explicit
		version of ip_route_output.])
	fi
    ])
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/brlock.h \
			  linux/slab.h linux/security.h linux/snmp.h net/xfrm.h net/dst.h \
			  net/request_sock.h], [:], [:], [
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
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/sched.h>
    ])
    _LINUX_CHECK_TYPES([struct sockaddr_storage], [], [], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
    ])
    _LINUX_CHECK_FUNCS([rcu_read_lock dst_output dst_mtu nf_reset ip_dst_output \
			ip_route_output_key __in_dev_get_rcu synchronize_net], [], [], [
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
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/inetdevice.h>
    ])
    _LINUX_CHECK_TYPES([irqreturn_t, irq_handler_t, bool, kmem_cache_t *,
			struct inet_protocol,
			struct net_protocol], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/time.h>		/* for struct timespec */
#include <net/sock.h>
#include <net/protocol.h>
    ])
    AH_TEMPLATE([kmem_cachep_t], [This kmem_cache_t is deprecated in recent
	2.6.20 kernels.  When it is deprecated, define this to struct
	kmem_cache *.])
    if test :"${linux_cv_type_kmem_cache_t_p:-no}" = :no ; then
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [struct kmem_cache *])
    else
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [kmem_cache_t *])
    fi
    _LINUX_CHECK_MACROS([rcu_read_lock], [], [], [
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
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
    ])
    _LINUX_CHECK_MEMBERS([struct inet_protocol.protocol,
			  struct inet_protocol.next,
			  struct inet_protocol.copy,
			  struct inet_protocol.no_policy,
			  struct net_protocol.proto,
			  struct net_protocol.next,
			  struct net_protocol.no_policy,
			  struct dst_entry.path,
			  struct dst_entry.path], [], [], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/protocol.h>
#ifdef HAVE_NET_DST_H
#include <net/dst.h>
#endif
    ])
    _LINUX_KERNEL_SYMBOLS([module_text_address,
			   skbuff_head_cache,
			   icmp_err_convert,
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
			   snmp_get_info,
			   socket_get_info,
			   sysctl_ip_dynaddr,
			   sysctl_ip_nonlocal_bind,
			   sysctl_local_port_range,
			   ip_frag_nqueues,
			   ip_frag_mem,
			   __xfrm_policy_check,
			   xfrm_policy_delete,
			   __xfrm_sk_clone_policy])
    if test :"${linux_cv_have_ip_route_output:-no}" = :yes ; then
	AC_DEFINE([HAVE_IP_ROUTE_OUTPUT], [1], [Most 2.4 kernels have
	the function ip_route_output() defined.  Newer RH kernels (EL3) use
	the 2.6 functions and do not provide ip_route_output().  Define this
	macro if your kernel provides ip_route_output().])
dnl     else
dnl 	_LINUX_KERNEL_SYMBOL_EXPORT([ip_route_output_flow], [dnl
dnl 	    AC_MSG_ERROR([
dnl **** 
dnl **** To use the package on newer kernels requires the availability of the
dnl **** function ip_route_output_flow() to permit calling ip_route_connect()
dnl **** which is an inline in net/route.h.  I cannot find this symbol on your
dnl **** system and the resulting kernel module will therefore not load.
dnl ****
dnl 		    ])
dnl 	])
dnl 	_LINUX_KERNEL_SYMBOL_EXPORT([__ip_route_output_key], [dnl
dnl 	    AC_MSG_ERROR([
dnl **** 
dnl **** To use the package on newer kernels requires the availability of the
dnl **** function __ip_route_output_key() to permit calling ip_route_connect()
dnl **** which is an inline in net/route.h.  I cannot find this symbol on your
dnl **** system and the resulting kernel module will therefore not load.
dnl ****
dnl 		    ])
dnl 	])
    fi
    _LINUX_KERNEL_ENV([dnl
	if test :$linux_cv_xfrm_policy_delete_symbol = :yes ; then
	    AC_CACHE_CHECK([for kernel xfrm_policy_delete_symbol returns int],
			   [linux_cv_xfrm_policy_delete_returns_int], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
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
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif]],
		    [[int retval = xfrm_policy_delete(NULL, 0);]]) ],
		    [linux_cv_xfrm_policy_delete_returns_int='yes'],
		    [linux_cv_xfrm_policy_delete_returns_int='no'])
	    ])
	    if test :$linux_cv_xfrm_policy_delete_returns_int = :yes ; then
		AC_DEFINE([HAVE_XFRM_POLICY_DELETE_RETURNS_INT], [1], [Define if function
			   xfrm_policy_delete returns int.])
	    fi
	fi
	AC_CACHE_CHECK([for kernel __ip_select_ident with 2 arguments], [linux_cv_have___ip_select_ident_2_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>]],
		[[void (*my_autoconf_function_pointer)(struct iphdr *, struct dst_entry *) = &__ip_select_ident;]]) ],
		[linux_cv_have___ip_select_ident_2_args='yes'],
		[linux_cv_have___ip_select_ident_2_args='no'])
	])
	if test :$linux_cv_have___ip_select_ident_2_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS], [1], [Define if function __ip_select_ident
		       takes 2 arguments.])
	fi
	AC_CACHE_CHECK([for kernel __ip_select_ident with 3 arguments], [linux_cv_have___ip_select_ident_3_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>]],
		[[void (*my_autoconf_function_pointer)(struct iphdr *, struct dst_entry *, int) = &__ip_select_ident;]]) ],
		[linux_cv_have___ip_select_ident_3_args='yes'],
		[linux_cv_have___ip_select_ident_3_args='no'])
	])
	if test :$linux_cv_have___ip_select_ident_3_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS], [1], [Define if function __ip_select_ident
		       takes 3 arguments.])
	fi
dnl 	if test :"${linux_cv_have___ip_select_ident_2_args:-no}" = :no \
dnl 	     -a :"${linux_cv_have___ip_select_ident_3_args:-no}" = :no
dnl 	then
dnl 	    AC_MSG_ERROR([
dnl ***
dnl *** Configure cannot determine whether your __ip_select_ident function takes
dnl *** 2 arguments or whether it takes 3 arguments.
dnl *** ])
dnl 	fi
	AC_CACHE_CHECK([for kernel sk_filter with 2 arguments], [linux_cv_have_sk_filter_2_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>]],
		[[sk_filter((struct sk_buff *)NULL, 0);]]) ],
		[linux_cv_have_sk_filter_2_args='yes'],
		[linux_cv_have_sk_filter_2_args='no'])
	])
	if test :$linux_cv_have_sk_filter_2_args = :yes ; then
	    AC_DEFINE([HAVE_SK_FILTER_2_ARGS], [1], [Define if function sk_filter takes 2
	    arguments.])
	fi
	AC_CACHE_CHECK([for kernel sk_filter with 3 arguments], [linux_cv_have_sk_filter_3_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>]],
		[[sk_filter((struct sock *)NULL, (struct sk_buff *)NULL, 0);]]) ],
		[linux_cv_have_sk_filter_3_args='yes'],
		[linux_cv_have_sk_filter_3_args='no'])
	])
	if test :$linux_cv_have_sk_filter_3_args = :yes ; then
	    AC_DEFINE([HAVE_SK_FILTER_3_ARGS], [1], [Define if function sk_filter takes 3
	    arguments.])
	fi
	AC_CACHE_CHECK([for kernel skb_linearize with 1 argument], [linux_cv_have_skb_linearize_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/skbuff.h>]],
		[[int (*my_autoconf_function_pointer)(struct sk_buff *) = &skb_linearize;]]) ],
		[linux_cv_have_skb_linearize_1_arg='yes'],
		[linux_cv_have_skb_linearize_1_arg='no'])
	])
	if test :$linux_cv_have_skb_linearize_1_arg = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_SKB_LINEARIZE_1_ARG], [1], [Define if
		       function skb_linearize takes 1 argument.])
	fi
    ])
    _LINUX_KERNEL_SYMBOLS([inet_proto_lock, inet_protos])
    _LINUX_CHECK_HEADERS([net/xfrm.h net/dst.h], [], [], [
#include <linux/autoconf.h>
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
    _LINUX_CHECK_TYPES([struct sockaddr_storage], [], [], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
    ])
    _LINUX_CHECK_MEMBERS([struct inet_protocol.protocol,
			  struct inet_protocol.no_policy,
			  struct dst_entry.path,
			  struct sk_buff.h.sh,
			  struct sock.protinfo.af_inet.ttl,
			  struct sock.protinfo.af_inet.uc_ttl,
			  struct sock.tp_pinfo.af_sctp], [], [], [
#include <linux/autoconf.h>
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
    _LINUX_CHECK_FUNCS([dst_pmtu], [], [], [
#include <linux/autoconf.h>
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
    _LINUX_KERNEL_SYMBOL_EXPORT([icmp_statistics])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_nonlocal_bind])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_default_ttl])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_dynaddr])
    _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_min_pmtu])
    _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_mtu_expires])
    _LINUX_KERNEL_SYMBOLS([afinet_get_info,
			   icmp_err_convert,
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
			   snmp_get_info,
			   socket_get_info,
			   sysctl_ip_dynaddr,
			   sysctl_ip_nonlocal_bind,
			   tcp_prot,
			   udp_prot,
			   raw_prot,
			   tcp_memory_allocated,
			   tcp_orphan_count,
			   tcp_sockets_allocated,
			   tcp_tw_count,
			   ip_frag_nqueues,
			   ip_frag_mem,
			   __xfrm_policy_check,
			   xfrm_policy_delete,
			   __xfrm_sk_clone_policy])
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel struct sk_buff.cb size], [linux_cv_sk_buff_cb_size], [dnl
	    CPPFLAGS="-I. -I$srcdir -I./src -I$srcdir/src -I./include -I$srcdir/include -I- $CPPFLAGS"
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include "net/sock.h"
#include "linux/sctp.h"
#include "net/sctp.h"
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
	AC_CACHE_CHECK([for kernel struct sock union size], [linux_cv_sock_size], [dnl
	    CPPFLAGS="-I. -I$srcdir -I./src -I$srcdir/src -I./include -I$srcdir/include -I- $CPPFLAGS"
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include "net/sock.h"
#include "linux/sctp.h"
#include "net/sctp.h"
		]], [[
static const size_t s1 = sizeof(((struct sock *)(0))->tp_pinfo);
static const size_t s2 = sizeof(struct __os7_sctp_opt_dummy);
static const size_t s3 = sizeof(((struct sock *)(0))->protinfo);
static const size_t s4 = sizeof(((struct sock *)(0))->protinfo.af_inet);
static const size_t s5 = sizeof(struct __os7_sctp_opt);
static const size_t s6 = sizeof(((struct __os7_sctp_opt *)(0))->__jump);
if (s1 < s2) {
    first_overlap_section_of_sctp_opt_is_too_large(NULL, 0, 0);
}
if (s3 < s4 + s5 - s2 - s6) {
    second_overlap_section_of_sctp_opt_is_too_large(NULL, 0, 0);
}
		]])
		], [dnl
    if ( objdump -r conftest.$ac_objext | grep -q overlap_section_of_sctp_opt_is_too_large ) 2>/dev/null
    then
	linux_cv_sock_size='toosmall'
	if ( objdump -r conftest.$ac_objext | grep -q first_overlap_section_of_sctp_opt_is_too_large ) 2>/dev/null
	then
	    linux_cv_sock_tp_pinfo_size='(tp_pinfo)'
	    linux_cv_sock_size="$linux_cv_sock_size (tp_pinfo)"
	else
	    linux_cv_sock_tp_pinfo_size=''
	fi
	if ( objdump -r conftest.$ac_objext | grep -q second_overlap_section_of_sctp_opt_is_too_large ) 2>/dev/null
	then
	    linux_cv_sock_protinfo_size='(protinfo)'
	    linux_cv_sock_size="$linux_cv_sock_size (protinfo)"
	else
	    linux_cv_sock_protinfo_size=''
	fi
	cp -f conftest.$ac_objext error_object.$ac_objext
    else
	linux_cv_sock_tp_pinfo_size='ok'
	linux_cv_sock_protinfo_size='ok'
	linux_cv_sock_size='ok'
    fi
		], [dnl
	linux_cv_sock_tp_pinfo_size='unknown'
	linux_cv_sock_protinfo_size='unknown'
	linux_cv_sock_size='unknown'
	    ])
	])
    ])
    case :$linux_cv_sock_size in
	:ok)
	    ;;
	:toosmall*)
	    # Another question, is lksctp bigger than tcp?
	    AC_MSG_ERROR([
**** 
**** ERROR: The size of a socket information union $linux_cv_sock_tp_pinfo_size$linux_cv_sock_protinfo_size in
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
dnl
dnl Use to complain about these but they were meaningless complaints.
dnl
    _LINUX_KERNEL_EXPORTS([
	    alloc_skb,
	    __generic_copy_to_user,
	    inetdev_lock,
	    inet_family_ops,
	    inet_getsockopt,
	    inet_recvmsg,
	    inet_setsockopt,
	    irq_stat,
	    __lock_sock,
	    __out_of_line_bug,
	    __pollwait,
	    __release_sock,
	    skb_linearize,
	    sysctl_ip_default_ttl
    ])
    _LINUX_KERNEL_EXPORTS([
	    add_wait_queue,
	    add_wait_queue_exclusive,
	    create_proc_entry,
	    del_timer,
	    dev_base_lock,
	    dev_base,
	    do_softirq,
	    free_pages,
	    __get_free_pages,
	    __get_user_4,
	    inet_accept,
	    inet_add_protocol,
	    inet_addr_type,
	    inet_del_protocol,
	    inet_register_protosw,
	    inet_release,
	    inet_sendmsg,
	    inet_shutdown,
	    inet_stream_connect,
	    inet_unregister_protosw,
	    ip_cmsg_recv,
	    ip_fragment,
	    ip_route_output_key,
	    __ip_select_ident,
	    ip_send_check,
	    jiffies,
	    kfree,
	    __kfree_skb,
	    kill_pg,
	    kill_proc,
	    kmem_cache_alloc,
	    kmem_cache_create,
	    kmem_cache_destroy,
	    kmem_cache_free,
	    kmem_find_general_cachep,
	    mod_timer,
	    net_statistics,
	    nf_hooks,
	    nf_hook_slow,
	    num_physpages,
	    panic,
	    printk,
	    proc_dointvec_jiffies,
	    proc_dointvec_minmax,
	    proc_dointvec,
	    proc_doulongvec_ms_jiffies_minmax,
	    proc_net,
	    ___pskb_trim,
	    put_cmsg,
	    register_sysctl_table,
	    remove_proc_entry,
	    remove_wait_queue,
	    schedule_timeout,
	    secure_tcp_sequence_number,
	    send_sig,
	    sk_alloc,
	    skb_clone,
	    skb_copy_datagram_iovec,
	    skb_over_panic,
	    skb_realloc_headroom,
	    skb_under_panic,
	    sk_free,
	    sk_run_filter,
	    sock_no_mmap,
	    sock_no_sendpage,
	    sock_no_socketpair,
	    sock_wake_async,
	    sock_wfree,
	    sprintf,
	    sysctl_intvec,
	    sysctl_jiffies,
	    sysctl_local_port_range,
	    unregister_sysctl_table,
	    __wake_up], [], [dnl
	    AC_MSG_WARN([
**** 
**** Linux kernel symbol ']LK_Export[' should be exported but it
**** isn't.  This could cause problems later.
**** ])])
])# _SCTP_CHECK_KERNEL
# =============================================================================

# =============================================================================
# _SCTP_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OUTPUT], [dnl
    _SCTP_CONFIG
    AM_CONDITIONAL(WITH_LFS, false)dnl
    AM_CONDITIONAL(WITH_LIS, false)dnl
])# _SCTP_OUTPUT
# =============================================================================

# =============================================================================
# _SCTP_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_CONFIG], [dnl
# SCTP_CONFIG_SLOW_VERIFICATION
    AC_MSG_CHECKING([for sctp slow verification])
    AC_ARG_ENABLE([sctp-slow-verification],
	AS_HELP_STRING([--enable-sctp-slow-verification],
	    [enable slow verification of addresses and tags. @<:@default=no@:>@]),
	[sctp_cv_slow_verification="$enableval"],
	[sctp_cv_slow_verification='no'])
    if test :"${sctp_cv_slow_verification:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_SLOW_VERIFICATION], [1], [
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
# SCTP_CONFIG_THROTTLE_HEARTBEATS
    AC_MSG_CHECKING([for sctp throttle heartbeats])
    AC_ARG_ENABLE([sctp-throttle-heartbeats],
	AS_HELP_STRING([--enable-sctp-throttle-heartbeats],
	    [enable heartbeat throttling. @<:@default=no@:>@]),
	[sctp_cv_throttle_heartbeats="$enableval"],
	[sctp_cv_throttle_heartbeats='no'])
    if test :"${sctp_cv_throttle_heartbeats:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_THROTTLE_HEARTBEATS], [1], [
	    Special feature of OpenSS7 SCTP which is not mentioned in RFC
	    2960.  When defined, SCTP will throttle the rate at which it
	    responds to heartbeats to the system control sctp_heartbeat_itvl.
	    This makes SCTP more reslilient to implementations which flood
	    heartbeat messages.  For RFC 2960 compliant operation, leave this
	    undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_throttle_heartbeats])
# SCTP_CONFIG_DISCARD_OOTB
    AC_MSG_CHECKING([for sctp dicard out-of-the-blue])
    AC_REQUIRE([_SCTP_OTHER_SCTP])dnl
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
	AC_DEFINE([SCTP_CONFIG_DISCARD_OOTB], [1], [
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
# SCTP_CONFIG_EXTENDED_IP_SUPPORT
    AC_MSG_CHECKING([for sctp extended ip support])
    AC_ARG_ENABLE([sctp-extended-ip-support],
	AS_HELP_STRING([--enable-sctp-extended-ip-support],
	    [enable extended IP support for SCTP. @<:@default=disabled@:>@]),
	[sctp_cv_extended_ip_support="$enableval"],
	[sctp_cv_extended_ip_support='no'])
    if test :"${sctp_cv_extended_ip_support:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_EXTENDED_IP_SUPPORT], [1], [
	    This provides extended IP support for SCTP for things like IP
	    Transparent Proxy and IP Masquerading.  This is experimental
	    stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_extended_ip_support])
    AC_C_BIGENDIAN(
	[sctp_cv_be_machine='yes'; sctp_cv_le_machine='no'],
	[sctp_cv_be_machine='no'; sctp_cv_le_machine='yes'],
	[sctp_cv_be_machine='yes'; sctp_cv_le_machine='yes'])
# SCTP_CONFIG_HMAC_SHA1
    AC_MSG_CHECKING([for sctp hmac sha-1])
    AC_ARG_ENABLE([sctp-hmac-sha1],
	AS_HELP_STRING([--disable-sctp-hmac-sha1],
	    [disable SHA-1 HMAC. @<:@default=enabled(BE),disabled(LE)@:>@]),
	[sctp_cv_hmac_sha1="$enableval"],
	[sctp_cv_hmac_sha1="$sctp_cv_be_machine"])
    if test :"$sctp_cv_hmac_sha1" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_HMAC_SHA1], [1], [
	    When defined, this provides the ability to sue the FIPS 180-1
	    (SHA-1) message authentication code in SCTP cookies.  When
	    defined and the appropriate sysctl and option is set, SCTP will
	    use SHA-1 HMAC when signing cookies in the INIT-ACK chunk.  If
	    undefined, the SHA-1 HMAC will be unavailable for use with SCTP.
	    If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_SHA1], [test :"$sctp_cv_hmac_sha1" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_hmac_sha1])
# SCTP_CONFIG_HMAC_MD5
    AC_MSG_CHECKING([for sctp hmac md5])
    AC_ARG_ENABLE([sctp-hmac-md5],
	AS_HELP_STRING([--disable-sctp-hmac-md5],
	    [disable MD5 HMAC. @<:@default=enabled(LE),disabled(BE)@:>@]),
	[sctp_cv_hmac_md5="$enableval"],
	[sctp_cv_hmac_md5="$sctp_cv_le_machine"])
    if test :"$sctp_cv_hmac_md5" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_HMAC_MD5], [1], [
	    When defined, this provides the ability to use the MD5 (RFC 1321)
	    message authentication code in SCTP cookies.  If you define this
	    macro, when the appropriate system control and stream option is
	    set, SCTP will use the MD5 HMAC when signing cookies in the
	    INIT-ACK chunk.  If this macro is undefined, the HD5 HMAC will be
	    unavailable for use with SCTP.  If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_MD5], [test :"$sctp_cv_hmac_md5" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_hmac_md5])
# SCTP_CONFIG_ADLER_32
    AC_MSG_CHECKING([for sctp Adler32 checksum])
    AC_ARG_ENABLE([sctp-adler32],
	AS_HELP_STRING([--enable-sctp-adler32],
	    [enable Adler32 checksum. @<:@default=disabled@:>@]),
	[sctp_cv_adler32="$enableval"],
	[sctp_cv_adler32='no'])
    if test :"$sctp_cv_adler32" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADLER_32], [1], [
	    This provides the ability to use the older RFC 2960 Adler32
	    checksum.  If SCTP_CONFIG_CRC_32 below is not selected, the Adler32
	    checksum is always provided.])dnl
    fi
    AM_CONDITIONAL([WITH_ADLER_32], [test :"$sctp_cv_adler32" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_adler32])
# SCTP_CONFIG_CRC_32C
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
	AC_DEFINE([SCTP_CONFIG_CRC_32C], [1], [
	    This provides the ability to use the newer CRC-32c checksum as
	    described in RFC 3309.  When this is selected and
	    SCTP_CONFIG_ADLER_32 is not selected above, then the only checksum
	    that will be used is the CRC-32c checksum.])dnl
    fi
    AM_CONDITIONAL([WITH_CRC_32C], [test :"$sctp_cv_crc32c" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_crc32c])
# SCTP_CONFIG_THROTTLE_PASSIVEOPENS
    AC_MSG_CHECKING([for sctp throttle passive opens])
    AC_ARG_ENABLE([sctp-throttle-passiveopens],
	AS_HELP_STRING([--enable-sctp-throttle-passiveopens],
	    [enable throttling of passive opens. @<:@default=disabled@:>@]),
	[sctp_cv_throttle_passiveopens="$enableval"],
	[sctp_cv_throttle_passiveopens='no'])
    if test :"$sctp_cv_throttle_passiveopens" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_THROTTLE_PASSIVEOPENS], [1], [
	    Special feature of Linux SCTP not mentioned in RFC 2960.  When
	    secure algorithms are used for signing cookies, the implementation
	    becomes vulnerable to INIT and COOKIE ECHO flooding.  If defined,
	    SCTP will only allow one INIT and one COOKE ECHO to be processed in
	    each interval corresponding to the sysctl sctp_throttle_itvl.
	    Setting sctp_throttle_itvl to 0 defeats this function.  If
	    undefined, each INIT and COOKIE ECHO will be processed.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_throttle_passiveopens])
# SCTP_CONFIG_ECN
    AC_MSG_CHECKING([for sctp ecn])
    AC_ARG_ENABLE([sctp-ecn],
	AS_HELP_STRING([--enable-sctp-ecn],
	    [enable Explicit Congestion Notification. @<:@default=enabled@:>@]),
	[sctp_cv_ecn="$enableval"],
	[sctp_cv_ecn='yes'])
    if test :"$sctp_cv_ecn" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ECN], [1], [
	    This enables support for Explicit Congestion Notification (ECN)
	    chunks in SCTP messages as defined in RFC 2960 and RFC 3168.  It
	    also adds syctl (/proc/net/ipv4/sctp_ecn) which allows ECN for SCTP
	    to be disabled at runtime.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_ecn])
# SCTP_CONFIG_LIFETIMES
    AC_MSG_CHECKING([for sctp lifetimes])
    AC_ARG_ENABLE([sctp-lifetimes],
	AS_HELP_STRING([--enable-sctp-lifetimes],
	    [enable SCTP message lifetimes. @<:@default=enabled@:>@]),
	[sctp_cv_lifetimes="$enableval"],
	[sctp_cv_lifetimes='yes'])
    if test :"$sctp_cv_lifetimes" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_LIFETIMES], [1], [
	    This enables support for message lifetimes as described in RFC 2960.
	    When enabled, message lifetimes can be set on messages.  See
	    sctp(7).  This feature is always enabled when Partial Reliability
	    Support is set.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_lifetimes])
# SCTP_CONFIG_ADD_IP
    AC_MSG_CHECKING([for sctp add ip])
    AC_ARG_ENABLE([sctp-add-ip],
	AS_HELP_STRING([--enable-sctp-add-ip],
	    [enable ADD-IP. @<:@default=enabled@:>@]),
	[sctp_cv_add_ip="$enableval"],
	[sctp_cv_add_ip='yes'])
    if test :"$sctp_cv_add_ip" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADD_IP], [1], [
	    This enables support for ADD-IP as described in
	    draft-ietf-tsvwg-addip-sctp-07.txt.  This allows the addition and
	    removal of IP addresses from existing connections.  This is
	    experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_add_ip])
# SCTP_CONFIG_ADAPTATION_LAYER_INFO
    AC_MSG_CHECKING([for sctp adaptation layer info])
    AC_ARG_ENABLE([sctp-adaptation-layer-info],
	AS_HELP_STRING([--enable-sctp-adaptation-layer-info],
	    [enable ALI. @<:@default=enabled@:>@]),
	[sctp_cv_adaptation_layer_info="$enableval"],
	[sctp_cv_adaptation_layer_info='yes'])
    if test :"$sctp_cv_adaptation_layer_info" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADAPTATION_LAYER_INFO], [1], [
	    This enables support for the Adaptation Layer Information parameter
	    described in draft-ietf-tsvwg-addip-sctp-07.txt for communicating
	    application layer information bits at initialization.  This is
	    experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_adaptation_layer_info])
# SCTP_CONFIG_PARTIAL_RELIABILITY
    AC_MSG_CHECKING([for sctp partial reliability])
    AC_ARG_ENABLE([sctp-partial-reliability],
	AS_HELP_STRING([--enable-sctp-partial-reliability],
	    [enable SCTP Partial Reliability (PR-SCTP). @<:@default=enabled@:>@]),
	[sctp_cv_partial_reliability="$enableval"],
	[sctp_cv_partial_reliability='yes'])
    if test :"$sctp_cv_partial_reliability" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_PARTIAL_RELIABILITY], [1], [
	    This enables support for PR-SCTP as described in
	    draft-stewart-tsvwg-prsctp-03.txt.  This allows for partial
	    reliability of message delivery on a "timed reliability" basis.
	    This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_partial_reliability])
# SCTP_CONFIG_ERROR_GENERATOR
    AC_MSG_CHECKING([for sctp error generator])
    AC_ARG_ENABLE([sctp-error-generator],
	AS_HELP_STRING([--disable-sctp-error-generator],
	    [disable SCTP error generator. @<:@default=enabled@:>@]),
	[sctp_cv_error_generator="$enableval"],
	[sctp_cv_error_generator='yes'])
    if test :"$sctp_cv_error_generator" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ERROR_GENERATOR], [1], [
	    This provides an internal error generator that can be accessed with
	    socket options for testing SCTP operation under packet loss.  You
	    will need this option to run some of the test programs distributed
	    with the SCTP module.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_error_generator])
# SCTP_CONFIG_TCP_COMPATIBLE
    AC_MSG_CHECKING([for sctp tcp compatibility])
    AC_ARG_ENABLE([sctp-tcp-compatible],
	AS_HELP_STRING([--disable-tcp-compatible],
	    [disable SCTP TCP compatibility. @<:@default=enabled@:>@]),
	[sctp_cv_tcp_compatible="$enableval"],
	[sctp_cv_tcp_compatible='yes'])
    if test :"$sctp_cv_tcp_compatible" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_TCP_COMPATIBLE], [1], [
	    This enables support for SOCK_STREAM type TCP compatible sockets
	    in addition to the normal SCTP SOCK_SEQPACKET sockets.  These work
	    good and are normally enabled.  Define this symbol.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_tcp_compatible])
# SCTP_CONFIG_UDP_COMPATIBLE
    AC_MSG_CHECKING([for sctp rudp compatibility])
    AC_ARG_ENABLE([sctp-udp-compatible],
	AS_HELP_STRING([--enable-udp-compatible],
	    [enable SCTP RUDP compatibility. @<:@default=disabled@:>@]),
	[sctp_cv_udp_compatible="$enableval"],
	[sctp_cv_udp_compatible='no'])
    if test :"$sctp_cv_udp_compatible" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_UDP_COMPATIBLE], [1], [
	    This enables support for SOCK_RDM type RUDP compatible sockets in
	    addition to the normal SCTP SOCK_SEQPACKET sockets.  These have
	    not been tested.  This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_udp_compatible])
])# _SCTP_CONFIG
# =============================================================================

# =============================================================================
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.62  2008-04-29 08:49:49  brian
# - updated headers for Affero release
#
# Revision 0.9.2.61  2007/08/14 09:42:14  brian
# - GPLv3 header update
#
# =============================================================================
# 
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
