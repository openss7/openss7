# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocin nosi
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2006/09/26 00:52:22 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2006/09/26 00:52:22 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/devfs.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])
m4_include([m4/strconf.m4])
m4_include([m4/streams.m4])
m4_include([m4/strcomp.m4])
m4_include([m4/xopen.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/inet.m4])
m4_include([m4/sctp.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_SS7
# -----------------------------------------------------------------------------
AC_DEFUN([AC_SS7], [dnl
    _OPENSS7_PACKAGE([SS7], [OpenSS7 SS7 Stack])
    _SS7_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strss7-core.postinst
		     debian/strss7-core.postrm
		     debian/strss7-core.preinst
		     debian/strss7-core.prerm
		     debian/strss7-devel.preinst
		     debian/strss7-dev.postinst
		     debian/strss7-dev.preinst
		     debian/strss7-dev.prerm
		     debian/strss7-doc.postinst
		     debian/strss7-doc.preinst
		     debian/strss7-doc.prerm
		     debian/strss7-init.postinst
		     debian/strss7-init.postrm
		     debian/strss7-init.preinst
		     debian/strss7-init.prerm
		     debian/strss7-lib.preinst
		     debian/strss7-source.preinst
		     debian/strss7-util.preinst
		     tests/testsuite.at
		     src/util/modutils/strss7
		     src/include/sys/strss7/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDADD"
    _SS7_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
dnl if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
dnl fi
    PKG_MODFLAGS='$(STREAMS_MODFLAGS) $(STRCOMP_MODFLAGS)'
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
    PKG_MANPATH="${STREAMS_MANPATH:+${STREAMS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${STRCOMP_MANPATH:+${STRCOMP_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XNS_MANPATH:+${XNS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XTI_MANPATH:+${XTI_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _SS7_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_SS7
# =============================================================================

# =============================================================================
# _SS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OPTIONS], [dnl
    _SS7_CHECK_INET
    _SS7_CHECK_SCTP
])# _SS7_OPTIONS
# =============================================================================

# =============================================================================
# _SS7_CHECK_INET
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_INET], [dnl
    AC_ARG_WITH([udp],
	AS_HELP_STRING([--with-udp],
	    [include udp version 2 driver in build.  @<:@default=yes@:>@]),
	[with_udp="$withval"],
	[with_udp='no'])
    AC_MSG_CHECKING([for UDP Version 2 driver])
    if test :"$with_udp" = :yes ; then
	ss7_cv_udp_v2='yes'
	AC_DEFINE([UDP_VERSION_2], [1], [
	    Define for UDP Version 2.  This define is needed by test programs and other programs
	    that need to determine if the UDP Version 2 driver is included in the build or not.])
    else
	ss7_cv_udp_v2='yes'
    fi
    AC_MSG_RESULT([$ss7_cv_udp_v2])
    AM_CONDITIONAL([WITH_UDP], [test :"$ss7_cv_udp_v2" = :yes])dnl
    AC_ARG_WITH([raw],
	AS_HELP_STRING([--with-raw],
	    [include raw version 2 driver in build.  @<:@default=yes@:>@]),
	[with_raw="$withval"],
	[with_raw='no'])
    AC_MSG_CHECKING([for RAW Version 2 driver])
    if test :"$with_raw" = :yes ; then
	ss7_cv_raw_v2='yes'
	AC_DEFINE([RAW_VERSION_2], [1], [
	    Define for RAW Version 2.  This define is needed by test programs and other programs
	    that need to determine if the RAW Version 2 driver is included in the build or not.])
    else
	ss7_cv_raw_v2='yes'
    fi
    AC_MSG_RESULT([$ss7_cv_raw_v2])
    AM_CONDITIONAL([WITH_RAW], [test :"$ss7_cv_raw_v2" = :yes])dnl
    AC_ARG_WITH([tcp],
	AS_HELP_STRING([--with-tcp],
	    [include tcp version 2 driver in build.  @<:@default=no@:>@]),
	[with_tcp="$withval"],
	[with_tcp='no'])
    AC_MSG_CHECKING([for TCP Version 2 driver])
    if test :"$with_tcp" = :yes ; then
	ss7_cv_tcp_v2='yes'
	AC_DEFINE([TCP_VERSION_2], [1], [
	    Define for TCP Version 2.  This define is needed by test programs and other programs
	    that need to determine if the TCP Version 2 driver is included in the build or not.])
    else
	ss7_cv_tcp_v2='yes'
    fi
    AC_MSG_RESULT([$ss7_cv_tcp_v2])
    AM_CONDITIONAL([WITH_TCP], [test :"$ss7_cv_tcp_v2" = :yes])dnl
])# _SS7_CHECK_INET
# =============================================================================

# =============================================================================
# _SS7_CHECK_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_SCTP], [dnl
    AC_MSG_CHECKING([for sctp version])
    if test :"$with_sctp2" = :yes ; then
	with_sctp='no'
	ss7_cv_sctp_version=2
	AC_DEFINE([SCTP_VERSION_2], [1], [
	    Define for SCTP Version 2.  This define is needed by test programs
	    and other programs that need to determine the difference between
	    the address format and options conventions for the two versions.])
    fi
    if test :"with_sctp" = :yes ; then
	with_sctp2='no'
	ss7_cv_sctp_version=1
	AC_DEFINE([SCTP_VERSION_1], [1], [
	    Define for SCTP Version 1.  This define is needed by test programs
	    and other programs that need to determine the difference between
	    the address format and options conventions for the two versions.])
    fi
    AM_CONDITIONAL([WITH_SCTP], [test :"$ss7_cv_sctp_version" = :1])dnl
    AM_CONDITIONAL([WITH_SCTP2], [test :"$ss7_cv_sctp_version" = :2])dnl
    AC_DEFINE_UNQUOTED([SCTP_VERSION], [$ss7_cv_sctp_version], [
	Define to 1 for SCTP Version 1.  Define to 2 for SCTP Version 2.  This
	define is needed by test programs that must determine the difference
	between the address format and options conventions for the two
	versions.])
    AC_MSG_RESULT([${ss7_cv_sctp_version:-no}])
])# _SS7_CHECK_SCTP
# =============================================================================

# =============================================================================
# _SS7_SETUP_DEBUG
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_DEBUG], [dnl
    case "$linux_cv_debug" in
    _DEBUG)
	AC_DEFINE_UNQUOTED([SS7_CONFIG_DEBUG], [], [Define to perform
			    internal structure tracking within SS7 as well as
			    to provide additional /proc filesystem files for
			    examining internal structures.])
	;;
    _TEST)
	AC_DEFINE_UNQUOTED([SS7_CONFIG_TEST], [], [Define to perform
			    performance testing with debugging.  This mode
			    does not dump massive amounts of information into
			    system logs, but peforms all assertion checks.])
	;;
    _SAFE)
	AC_DEFINE_UNQUOTED([SS7_CONFIG_SAFE], [], [Define to perform
			    fundamental assertion checks.  This is a safer
			    mode of operation.])
	;;
    _NONE | *)
	AC_DEFINE_UNQUOTED([SS7_CONFIG_NONE], [], [Define to perform no
			    assertion checks but report software errors.  This
			    is the smallest footprint, highest performance
			    mode of operation.])
	;;
    esac
])# _SS7_SETUP_DEBUG
# =============================================================================

# =============================================================================
# _SS7_OTHER_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OTHER_SCTP], [dnl
    linux_cv_other_sctp='no'
    linux_cv_lksctp_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp compiled in], [CONFIG_IP_SCTP])
    if test :"$linux_cv_CONFIG_IP_SCTP" = :"yes" ; then
	linux_cv_other_sctp='lksctp'
	linux_cv_lksctp_sctp='yes'
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
	linux_cv_other_sctp='lksctp'
	linux_cv_lksctp_sctp='yes'
	AC_DEFINE([HAVE_LKSCTP_SCTP], [1], [Some more recent 2.4.25 and
	    greater kernels have this poorman version of SCTP included in the
	    kernel.  Define this symbol if you have such a bastardized kernel.
	    When we have such a kernel we need to define lksctp's header
	    wrapper defines so that none of the lksctp header files are
	    included (we use our own instead).])
    fi
    linux_cv_openss7_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp compiled in], [CONFIG_SCTP])
    if test :"$linux_cv_CONFIG_SCTP" = :"yes" ; then
	linux_cv_other_sctp='openss7'
	linux_cv_openss7_sctp='yes'
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
	linux_cv_other_sctp='openss7'
	linux_cv_openss7_sctp='yes'
	AC_DEFINE([HAVE_OPENSS7_SCTP], [1], [Define if your kernel supports
	    the OpenSS7 Linux Kernel Sockets SCTP patches.  This enables
	    support in the SCTP driver for STREAMS on top of the OpenSS7 Linux
	    Kernel Sockets SCTP implementation.])
    fi
    AM_CONDITIONAL([WITH_LKSCTP_SCTP], [ test :"${linux_cv_lksctp_sctp:-no}"  = :yes])dnl
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], [test :"${linux_cv_openss7_sctp:-no}" = :yes])dnl
])# _SS7_OTHER_SCTP
# =============================================================================

# =============================================================================
# _SS7_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    _LINUX_STREAMS
    _STRCOMP
    with_ss7='yes'
    _XOPEN
    _XNS
    _XTI
    _INET
    _SCTP
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _SS7_OTHER_SCTP
    _SS7_SETUP_MODULE
    _SS7_CONFIG_KERNEL
    _SS7_SETUP_DEBUG
])# _SS7_SETUP
# =============================================================================

# =============================================================================
# _SS7_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_MODULE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([SS7_CONFIG_MODULE], [], [When defined, SS7 is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([SS7_CONFIG], [], [When defined, SS7 is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([SS7_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([SS7_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _SS7_SETUP_MODULE
# =============================================================================

# =============================================================================
# _SS7_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuration checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_CONFIG_KERNEL], [dnl
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
	    AC_DEFINE([HAVE_IP_ROUTE_OUTPUT_EXPLICIT], [1], [Define if you have the explicit
		version of ip_route_output.])
	fi
    ])
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/brlock.h \
			  linux/slab.h linux/security.h linux/snmp.h net/xfrm.h net/dst.h \
			  net/request_sock.h], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#if HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#if HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/sched.h>
    ])
    _LINUX_CHECK_TYPES([struct sockaddr_storage], [], [], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
    ])
    _LINUX_CHECK_FUNCS([rcu_read_lock dst_output dst_mtu ip_dst_output \
			ip_route_output_key __in_dev_get_rcu synchronize_net], [], [], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#if HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#if HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/inetdevice.h>
    ])
    _LINUX_CHECK_TYPES([irqreturn_t,
			pm_message_t,
			struct inet_protocol,
			struct net_protocol], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#if HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#include <linux/time.h>		/* for struct timespec */
#include <linux/pm.h>
#include <net/sock.h>
#include <net/protocol.h>
])
    _LINUX_CHECK_MACROS([rcu_read_lock], [], [], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#if HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#if HAVE_KINC_NET_DST_H
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
#include <linux/config.h>
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
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#if HAVE_KINC_NET_XFRM_H
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
#include <linux/config.h>
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
#include <linux/config.h>
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
	AC_CACHE_CHECK([for kernel skb_linearize with 1 argument], [linux_cv_have_skb_linearize_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/config.h>
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
    if test :"${with_sctp:-no}" = :yes -o :"${with_sctp2:-no}" = :yes -o :"${with_inet:-no}" = :yes ; then
	_LINUX_KERNEL_ENV([dnl
	    AC_CHECK_MEMBER([struct inet_protocol.protocol],
		[linux_cv_inet_protocol_style='old'],
		[:], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/protocol.h>
		])
	    AC_CHECK_MEMBER([struct inet_protocol.no_policy],
		[linux_cv_inet_protocol_style='new'],
		[:], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/protocol.h>
		])
	    AC_CHECK_MEMBER([struct dst_entry.path],
		[linux_cv_dst_entry_path='yes'],
		[linux_cv_dst_entry_path='no'], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/dst.h>
		])
	    ])
    fi
    if test :"${linux_cv_inet_protocol_style:+set}" = :set ; then
	case "$linux_cv_inet_protocol_style" in
	    old)
		AC_DEFINE([HAVE_OLD_STYLE_INET_PROTOCOL], [1], [Most
		2.4 kernels have the old style struct inet_protocol and the
		old prototype for inet_add_protocol() and inet_del_protocol()
		defined in <net/protocol.h>.  Some more recent RH kernels
		(such as EL3) use the 2.6 style of structure and prototypes.
		Define this macro if your kernel has the old style structure
		and prototypes.])
		;;
	    new)
		AC_DEFINE([HAVE_NEW_STYLE_INET_PROTOCOL], [1], [Most
		2.4 kernels have the old style struct inet_protocol and the
		old prototype for inet_add_protocol() and inet_del_protocol()
		defined in <net/protocol.h>.  Some more recent RH kernels
		(such as EL3) use the 2.6 style of structure and prototypes.
		Define this macro if your kernel has the new style structure
		and prototypes.])
		;;
	esac
    else
	with_udp='no'
	with_raw='no'
	with_tcp='no'
	with_sctp='no'
	with_sctp2='no'
    fi
    if test :"${linux_cv_dst_entry_path:-no}" = :yes ; then
	AC_DEFINE([HAVE_STRUCT_DST_ENTRY_PATH], [1], [Newer RHEL3
	kernels change the destination entry structure.  Define this macro to
	use the newer structure.])
    fi
    _LINUX_KERNEL_ENV([
	AC_CHECK_TYPES([struct sockaddr_storage], [
	    AC_DEFINE([HAVE_STRUCT_SOCKADDR_STORAGE], [1], [Most 2.4
		kernels do not define struct sockaddr_storage.  Define to 1 if
		your kernel supports struct sockaddr_storage.])], [:], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
	    ])
	])
    _LINUX_KERNEL_SYMBOL_EXPORT([icmp_statistics])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_nonlocal_bind])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_default_ttl])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_dynaddr])
    _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_min_pmtu])
    _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_mtu_expires])
dnl
dnl These are INET-only checks
dnl
    _LINUX_CHECK_MEMBERS([struct sk_buff.h.sh,
			  struct sock.protinfo.af_inet.ttl,
			  struct sock.protinfo.af_inet.uc_ttl,
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
    _LINUX_KERNEL_SYMBOLS([
			   afinet_get_info,
			   tcp_prot,
			   udp_prot,
			   raw_prot,
			   tcp_memory_allocated,
			   tcp_orphan_count,
			   tcp_sockets_allocated,
			   tcp_tw_count,
			   __tcp_push_pending_frames])
    _LINUX_CHECK_MEMBER([struct sock.__sk_common], [
	_LINUX_CHECK_MEMBER([struct inet_opt.tos],
	[AC_DEFINE([HAVE_TRN_SOCK_STRUCTURE], [1], [Define to 1 i you have the transitional Linux
						    2.6 style struct inet_opt.  Otherwise, leave
						    undefined for the newer Linux 2.6 or older Linux
						    2.4 struct sock.])],
	[AC_DEFINE([HAVE_NEW_SOCK_STRUCTURE], [1], [Define to 1 if you have the newer Linux 2.6
						    style struct inet_sock.  Otherwise, leave
						    undefined for either the transitional Linux 2.6
						    or older Linux 2.4 socket structure.])], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
#include <linux/ip.h>
	])],
	[AC_DEFINE([HAVE_OLD_SOCK_STRUCTURE], [1], [Define to 1 if you have the older Linux 2.4
						    style struct sock.  Otherwise leave undefined
						    for the new Linux 2.6 struct sock.])], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
])
    _LINUX_CHECK_MEMBERS([struct sock.sk_localroute,
			  struct sock.sk_debug], [], [], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
])
    _LINUX_KERNEL_SYMBOLS([sock_alloc,
			   tcp_current_mss,
			   tcp_set_skb_tso_segs,
			   tcp_set_skb_tso_factor], [], [])
    _LINUX_CHECK_FUNCS([inet_csk], [], [], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
])
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel function tcp_set_skb_tso_segs with sock argument], [linux_cv_tcp_set_skb_tso_segs_sock], [
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>]],
		[[void (*my_autoconf_function_pointer)(struct sock *, struct sk_buff *) = &tcp_set_skb_tso_segs;]]) ],
		[linux_cv_tcp_set_skb_tso_segs_sock='yes'],
		[linux_cv_tcp_set_skb_tso_segs_sock='no'])
	    ])
	])
    if test :$linux_cv_tcp_set_skb_tso_segs_sock = :yes ; then
	AC_DEFINE([HAVE_KFUNC_TCP_SET_SKB_TSO_SEGS_SOCK], [1], [Define to 1 if function
	tcp_set_skb_tso_segs takes a (struct sock *, struct sk_buff *) argument list.])
    fi
dnl
dnl These are SCTP-only checks
dnl
    _LINUX_CHECK_HEADERS([linux/percpu.h], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
])

    AC_REQUIRE([_SS7_OTHER_SCTP])dnl
dnl if test :"${linux_cv_openss7_sctp:-no}" = :yes ; then
dnl     with_sctp='no'
dnl     with_sctp2='no'
dnl fi
    if test :"${with_sctp2:-no}" = :yes ; then
	with_sctp='no'
    fi
    if test :"${with_sctp:-no}" = :yes -o :"${with_sctp2:-no}" = :yes ; then
	_LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_update_pmtu], [with_sctp='no'; with_sctp2='no'])
    fi
dnl
dnl These were INET-only checks
dnl
    _LINUX_KERNEL_EXPORTS([
	    add_wait_queue,
	    add_wait_queue_exclusive,
	    alloc_skb,
	    create_proc_entry,
	    del_timer,
	    dev_base_lock,
	    dev_base,
	    do_softirq,
	    free_pages,
	    __generic_copy_to_user,
	    __get_free_pages,
	    __get_user_4,
	    inet_accept,
	    inet_add_protocol,
	    inet_addr_type,
	    inet_del_protocol,
	    inetdev_lock,
	    inet_family_ops,
	    inet_getsockopt,
	    inet_recvmsg,
	    inet_register_protosw,
	    inet_release,
	    inet_sendmsg,
	    inet_setsockopt,
	    inet_shutdown,
	    inet_stream_connect,
	    inet_unregister_protosw,
	    ip_cmsg_recv,
	    ip_fragment,
	    ip_route_output_key,
	    __ip_select_ident,
	    ip_send_check,
	    irq_stat,
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
	    __lock_sock,
	    mod_timer,
	    net_statistics,
	    nf_hooks,
	    nf_hook_slow,
	    num_physpages,
	    __out_of_line_bug,
	    panic,
	    __pollwait,
	    printk,
	    proc_dointvec_jiffies,
	    proc_dointvec_minmax,
	    proc_dointvec,
	    proc_doulongvec_ms_jiffies_minmax,
	    proc_net,
	    ___pskb_trim,
	    put_cmsg,
	    register_sysctl_table,
	    __release_sock,
	    remove_proc_entry,
	    remove_wait_queue,
	    schedule_timeout,
	    secure_tcp_sequence_number,
	    send_sig,
	    sk_alloc,
	    skb_clone,
	    skb_copy_datagram_iovec,
	    skb_linearize,
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
	    sysctl_ip_default_ttl,
	    sysctl_jiffies,
	    sysctl_local_port_range,
	    unregister_sysctl_table,
	    __wake_up], [], [dnl
	    AC_MSG_WARN([
**** 
**** Linux kernel symbol ']LK_Export[' should be exported but it
**** isn't.  This could cause problems later.
**** ])])
    AC_SUBST([EXPOSED_SYMBOLS])
    EXPOSED_SYMBOLS="\
	mtp_n_uderror_ind \
	mtp_n_unitdata_ind \
	mtp_n_unitdata_req \
	sccp_n_uderror_ind \
	sccp_n_unitdata_ind \
	sccp_n_unitdata_req \
	sctp_data_ind \
	sctp_data_req \
	sctp_datack_ind \
	sctp_datack_req \
	sctp_exdata_ind \
	sctp_exdata_req"
])# _SS7_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _SS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OUTPUT], [dnl
    _SS7_CONFIG
    _SS7_STRCONF dnl
])# _SS7_OUTPUT
# =============================================================================

# =============================================================================
# _SS7_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_CONFIG], [dnl
# SCTP_CONFIG_SLOW_VERIFICATION
    AC_MSG_CHECKING([for sctp slow verification])
    AC_ARG_ENABLE([sctp-slow-verification],
	AS_HELP_STRING([--enable-sctp-slow-verification],
	    [enable slow verification of addresses and tags. @<:@default=no@:>@]),
	[ss7_cv_slow_verification="$enableval"],
	[ss7_cv_slow_verification='no'])
    if test :"${ss7_cv_slow_verification:-no}" = :yes ; then
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
    AC_MSG_RESULT([$ss7_cv_slow_verification])
# SCTP_CONFIG_THROTTLE_HEARTBEATS
    AC_MSG_CHECKING([for sctp throttle heartbeats])
    AC_ARG_ENABLE([sctp-throttle-heartbeats],
	AS_HELP_STRING([--enable-sctp-throttle-heartbeats],
	    [enable heartbeat throttling. @<:@default=no@:>@]),
	[ss7_cv_throttle_heartbeats="$enableval"],
	[ss7_cv_throttle_heartbeats='no'])
    if test :"${ss7_cv_throttle_heartbeats:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_THROTTLE_HEARTBEATS], [1], [
	    Special feature of OpenSS7 SCTP which is not mentioned in RFC
	    2960.  When defined, SCTP will throttle the rate at which it
	    responds to heartbeats to the system control sctp_heartbeat_itvl.
	    This makes SCTP more reslilient to implementations which flood
	    heartbeat messages.  For RFC 2960 compliant operation, leave this
	    undefined.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_throttle_heartbeats])
# SCTP_CONFIG_DISCARD_OOTB
    AC_MSG_CHECKING([for sctp dicard out-of-the-blue])
    AC_REQUIRE([_SS7_OTHER_SCTP])dnl
    AC_ARG_ENABLE([sctp-discard-ootb],
	AS_HELP_STRING([--enable-sctp-discard-ootb],
	    [enable discard out-of-the-blue packets. @<:@default=no@:>@]),
	[ss7_cv_discard_ootb="$enableval"],
	[if test :"${linux_cv_openss7_sctp:-no}" = :yes ; then
	    ss7_cv_discard_ootb='yes'
	 else
	    ss7_cv_discard_ootb='no'
	 fi])
    if test :"$ss7_cv_discard_ootb" = :yes ; then
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
    AC_MSG_RESULT([$ss7_cv_discard_ootb])
# SCTP_CONFIG_EXTENDED_IP_SUPPORT
    AC_MSG_CHECKING([for sctp extended ip support])
    AC_ARG_ENABLE([sctp-extended-ip-support],
	AS_HELP_STRING([--enable-sctp-extended-ip-support],
	    [enable extended IP support for SCTP. @<:@default=disabled@:>@]),
	[ss7_cv_extended_ip_support="$enableval"],
	[ss7_cv_extended_ip_support='no'])
    if test :"${ss7_cv_extended_ip_support:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_EXTENDED_IP_SUPPORT], [1], [
	    This provides extended IP support for SCTP for things like IP
	    Transparent Proxy and IP Masquerading.  This is experimental
	    stuff.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_extended_ip_support])
    AC_C_BIGENDIAN(
	[ss7_cv_be_machine='yes'; ss7_cv_le_machine='no'],
	[ss7_cv_be_machine='no'; ss7_cv_le_machine='yes'],
	[ss7_cv_be_machine='yes'; ss7_cv_le_machine='yes'])
# SCTP_CONFIG_HMAC_SHA1
    AC_MSG_CHECKING([for sctp hmac sha-1])
    AC_ARG_ENABLE([sctp-hmac-sha1],
	AS_HELP_STRING([--disable-sctp-hmac-sha1],
	    [disable SHA-1 HMAC. @<:@default=enabled(BE),disabled(LE)@:>@]),
	[ss7_cv_hmac_sha1="$enableval"],
	[ss7_cv_hmac_sha1="$ss7_cv_be_machine"])
    if test :"$ss7_cv_hmac_sha1" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_HMAC_SHA1], [1], [
	    When defined, this provides the ability to sue the FIPS 180-1
	    (SHA-1) message authentication code in SCTP cookies.  When
	    defined and the appropriate sysctl and option is set, SCTP will
	    use SHA-1 HMAC when signing cookies in the INIT-ACK chunk.  If
	    undefined, the SHA-1 HMAC will be unavailable for use with SCTP.
	    If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_SHA1], [test :"$ss7_cv_hmac_sha1" = :yes])dnl
    AC_MSG_RESULT([$ss7_cv_hmac_sha1])
# SCTP_CONFIG_HMAC_MD5
    AC_MSG_CHECKING([for sctp hmac md5])
    AC_ARG_ENABLE([sctp-hmac-md5],
	AS_HELP_STRING([--disable-sctp-hmac-md5],
	    [disable MD5 HMAC. @<:@default=enabled(LE),disabled(BE)@:>@]),
	[ss7_cv_hmac_md5="$enableval"],
	[ss7_cv_hmac_md5="$ss7_cv_le_machine"])
    if test :"$ss7_cv_hmac_md5" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_HMAC_MD5], [1], [
	    When defined, this provides the ability to use the MD5 (RFC 1321)
	    message authentication code in SCTP cookies.  If you define this
	    macro, when the appropriate system control and stream option is
	    set, SCTP will use the MD5 HMAC when signing cookies in the
	    INIT-ACK chunk.  If this macro is undefined, the HD5 HMAC will be
	    unavailable for use with SCTP.  If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_MD5], [test :"$ss7_cv_hmac_md5" = :yes])dnl
    AC_MSG_RESULT([$ss7_cv_hmac_md5])
# SCTP_CONFIG_ADLER_32
    AC_MSG_CHECKING([for sctp Adler32 checksum])
    AC_ARG_ENABLE([sctp-adler32],
	AS_HELP_STRING([--enable-sctp-adler32],
	    [enable Adler32 checksum. @<:@default=disabled@:>@]),
	[ss7_cv_adler32="$enableval"],
	[ss7_cv_adler32='no'])
    if test :"$ss7_cv_adler32" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADLER_32], [1], [
	    This provides the ability to use the older RFC 2960 Adler32
	    checksum.  If SCTP_CONFIG_CRC_32 below is not selected, the Adler32
	    checksum is always provided.])dnl
    fi
    AM_CONDITIONAL([WITH_ADLER_32], [test :"$ss7_cv_adler32" = :yes])dnl
    AC_MSG_RESULT([$ss7_cv_adler32])
# SCTP_CONFIG_CRC_32C
    AC_MSG_CHECKING([for sctp CRC-32C checksum])
    AC_ARG_ENABLE([sctp-crc32c],
	AS_HELP_STRING([--disable-sctp-crc32c],
	    [disable CRC-32C checksum. @<:@default=enabled@:>@]),
	[ss7_cv_crc32c="$enableval"],
	[ss7_cv_crc32c='yes'])
    if test :"${ss7_cv_adler32:-no}" != :yes ; then
	ss7_cv_crc32c='yes'
    fi
    if test :"$ss7_cv_crc32c" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_CRC_32C], [1], [
	    This provides the ability to use the newer CRC-32c checksum as
	    described in RFC 3309.  When this is selected and
	    SCTP_CONFIG_ADLER_32 is not selected above, then the only checksum
	    that will be used is the CRC-32c checksum.])dnl
    fi
    AM_CONDITIONAL([WITH_CRC_32C], [test :"$ss7_cv_crc32c" = :yes])dnl
    AC_MSG_RESULT([$ss7_cv_crc32c])
# SCTP_CONFIG_THROTTLE_PASSIVEOPENS
    AC_MSG_CHECKING([for sctp throttle passive opens])
    AC_ARG_ENABLE([sctp-throttle-passiveopens],
	AS_HELP_STRING([--enable-sctp-throttle-passiveopens],
	    [enable throttling of passive opens. @<:@default=disabled@:>@]),
	[ss7_cv_throttle_passiveopens="$enableval"],
	[ss7_cv_throttle_passiveopens='no'])
    if test :"$ss7_cv_throttle_passiveopens" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_THROTTLE_PASSIVEOPENS], [1], [
	    Special feature of Linux SCTP not mentioned in RFC 2960.  When
	    secure algorithms are used for signing cookies, the implementation
	    becomes vulnerable to INIT and COOKIE ECHO flooding.  If defined,
	    SCTP will only allow one INIT and one COOKE ECHO to be processed in
	    each interval corresponding to the sysctl sctp_throttle_itvl.
	    Setting sctp_throttle_itvl to 0 defeats this function.  If
	    undefined, each INIT and COOKIE ECHO will be processed.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_throttle_passiveopens])
# SCTP_CONFIG_ECN
    AC_MSG_CHECKING([for sctp ecn])
    AC_ARG_ENABLE([sctp-ecn],
	AS_HELP_STRING([--enable-sctp-ecn],
	    [enable Explicit Congestion Notification. @<:@default=enabled@:>@]),
	[ss7_cv_ecn="$enableval"],
	[ss7_cv_ecn='yes'])
    if test :"$ss7_cv_ecn" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ECN], [1], [
	    This enables support for Explicit Congestion Notification (ECN)
	    chunks in SCTP messages as defined in RFC 2960 and RFC 3168.  It
	    also adds syctl (/proc/net/ipv4/sctp_ecn) which allows ECN for SCTP
	    to be disabled at runtime.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_ecn])
# SCTP_CONFIG_LIFETIMES
    AC_MSG_CHECKING([for sctp lifetimes])
    AC_ARG_ENABLE([sctp-lifetimes],
	AS_HELP_STRING([--enable-sctp-lifetimes],
	    [enable SCTP message lifetimes. @<:@default=enabled@:>@]),
	[ss7_cv_lifetimes="$enableval"],
	[ss7_cv_lifetimes='yes'])
    if test :"$ss7_cv_lifetimes" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_LIFETIMES], [1], [
	    This enables support for message lifetimes as described in RFC 2960.
	    When enabled, message lifetimes can be set on messages.  See
	    sctp(7).  This feature is always enabled when Partial Reliability
	    Support is set.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_lifetimes])
# SCTP_CONFIG_ADD_IP
    AC_MSG_CHECKING([for sctp add ip])
    AC_ARG_ENABLE([sctp-add-ip],
	AS_HELP_STRING([--enable-sctp-add-ip],
	    [enable ADD-IP. @<:@default=enabled@:>@]),
	[ss7_cv_add_ip="$enableval"],
	[ss7_cv_add_ip='yes'])
    if test :"$ss7_cv_add_ip" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADD_IP], [1], [
	    This enables support for ADD-IP as described in
	    draft-ietf-tsvwg-addip-sctp-07.txt.  This allows the addition and
	    removal of IP addresses from existing connections.  This is
	    experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_add_ip])
# SCTP_CONFIG_ADAPTATION_LAYER_INFO
    AC_MSG_CHECKING([for sctp adaptation layer info])
    AC_ARG_ENABLE([sctp-adaptation-layer-info],
	AS_HELP_STRING([--enable-sctp-adaptation-layer-info],
	    [enable ALI. @<:@default=enabled@:>@]),
	[ss7_cv_adaptation_layer_info="$enableval"],
	[ss7_cv_adaptation_layer_info='yes'])
    if test :"$ss7_cv_adaptation_layer_info" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADAPTATION_LAYER_INFO], [1], [
	    This enables support for the Adaptation Layer Information parameter
	    described in draft-ietf-tsvwg-addip-sctp-07.txt for communicating
	    application layer information bits at initialization.  This is
	    experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_adaptation_layer_info])
# SCTP_CONFIG_PARTIAL_RELIABILITY
    AC_MSG_CHECKING([for sctp partial reliability])
    AC_ARG_ENABLE([sctp-partial-reliability],
	AS_HELP_STRING([--enable-sctp-partial-reliability],
	    [enable SCTP Partial Reliability (PR-SCTP). @<:@default=enabled@:>@]),
	[ss7_cv_partial_reliability="$enableval"],
	[ss7_cv_partial_reliability='yes'])
    if test :"$ss7_cv_partial_reliability" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_PARTIAL_RELIABILITY], [1], [
	    This enables support for PR-SCTP as described in
	    draft-stewart-tsvwg-prsctp-03.txt.  This allows for partial
	    reliability of message delivery on a "timed reliability" basis.
	    This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_partial_reliability])
# SCTP_CONFIG_ERROR_GENERATOR
    AC_MSG_CHECKING([for sctp error generator])
    AC_ARG_ENABLE([sctp-error-generator],
	AS_HELP_STRING([--disable-sctp-error-generator],
	    [disable SCTP error generator. @<:@default=enabled@:>@]),
	[ss7_cv_error_generator="$enableval"],
	[ss7_cv_error_generator='yes'])
    if test :"$ss7_cv_error_generator" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ERROR_GENERATOR], [1], [
	    This provides an internal error generator that can be accessed with
	    socket options for testing SCTP operation under packet loss.  You
	    will need this option to run some of the test programs distributed
	    with the SCTP module.])dnl
    fi
    AC_MSG_RESULT([$ss7_cv_error_generator])
])# _SS7_CONFIG
# =============================================================================

# =============================================================================
# _SS7_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=180
    strconf_cv_midbase=90
    if test ${streams_cv_package:-LfS} = LfS ; then
	if test ${linux_cv_minorbits:-8} -gt 8 ; then
dnl
dnl Tired of device conflicts on 2.6 kernels.
dnl
	    ((strconf_cv_majbase+=2000))
	fi
dnl
dnl Get these away from device numbers.
dnl
	((strconf_cv_midbase+=5000))
    fi
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
    strconf_cv_drvconf='drvconf.mk'
    strconf_cv_confmod='conf.modules'
    strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="${PACKAGE_TARNAME}_mknod.c"
    strconf_cv_strsetup='strsetup.conf'
    strconf_cv_strload='strload.conf'
    AC_REQUIRE([_LINUX_STREAMS])
    strconf_cv_package=${streams_cv_package:-LiS}
    strconf_cv_minorbits="${linux_cv_minorbits:-8}"
    _STRCONF dnl
])# _SS7_STRCONF
# =============================================================================

# =============================================================================
# _SS7_
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_], [dnl
])# _SS7_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocin nosi
# =============================================================================
