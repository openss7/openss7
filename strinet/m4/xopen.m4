# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/02/19 11:49:59 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Last Modified $Date: 2005/02/19 11:49:59 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _XOPEN_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_OPTIONS], [dnl
    _XOPEN_OPENSS7_SCTP
    _XOPEN_CHECK_SCTP
    _XOPEN_CHECK_XNS
    _XOPEN_CHECK_TLI
    _XOPEN_CHECK_INET
    _XOPEN_CHECK_XNET
    _XOPEN_CHECK_SOCK
])# _XOPEN_OPTIONS
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_SCTP], [dnl
    AC_ARG_WITH([sctp],
	AS_HELP_STRING([--with-sctp],
	    [include xopen sctp driver in build.  @<:@default=yes@:>@]),
	[with_sctp="$withval"],
	[with_sctp='no'])
    AC_ARG_WITH([sctp2],
	AS_HELP_STRING([--with-sctp2],
	    [include xopen sctp version 2 driver in build.  @<:@default=no@:>@]),
	[with_sctp2="$withval"],
	[with_sctp2='no'])
])# _XOPEN_CHECK_SCTP
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_XNS
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_XNS], [dnl
    AC_ARG_WITH([xns],
	AS_HELP_STRING([--with-xns],
	    [include xopen xns headers in install.  @<:@default=yes@:>@]),
	[with_xns="$withval"],
	[with_xns='no'])
])# _XOPEN_CHECK_XNS
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_TLI
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_TLI], [dnl
    AC_ARG_WITH([tli],
	AS_HELP_STRING([--with-tli],
	    [include xopen tli modules in build.  @<:@default=yes@:>@]),
	[with_tli="$withval"],
	[with_tli='no'])
])# _XOPEN_CHECK_TLI
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_INET
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_INET], [dnl
    AC_ARG_WITH([inet],
	AS_HELP_STRING([--with-inet],
	    [include xopen inet driver in build.  @<:@default=yes@:>@]),
	[with_inet="$withval"],
	[with_inet='no'])
])# _XOPEN_CHECK_INET
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_XNET
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_XNET], [dnl
    AC_ARG_WITH([xnet],
	AS_HELP_STRING([--with-xnet],
	    [include xopen xnet library in build.  @<:@default=yes@:>@]),
	[with_xnet="$withval"],
	[with_xnet='no'])
])# _XOPEN_CHECK_XNET
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_SOCK
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_SOCK], [dnl
    AC_ARG_WITH([sock],
	AS_HELP_STRING([--with-sock],
	    [include xopen sock library in build.  @<:@default=yes@:>@]),
	[with_sock="$withval"],
	[with_sock='no'])
])# _XOPEN_CHECK_SOCK
# =============================================================================

# =============================================================================
# _XOPEN_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP], [dnl
    if test :"$with_sctp" = :yes ; then :;
	_XOPEN_SETUP_SCTP
    fi
    AC_MSG_CHECKING([for xopen sctp driver])
    AC_MSG_RESULT([$with_sctp])
    if test :"$with_xns" = :yes ; then :;
	_XOPEN_SETUP_XNS
    fi
    AC_MSG_CHECKING([for xopen xns headers])
    AC_MSG_RESULT([$with_xns])
    if test :"$with_tli" = :yes ; then :;
	_XOPEN_SETUP_TLI
    fi
    AC_MSG_CHECKING([for xopen tli modules])
    AC_MSG_RESULT([$with_tli])
    if test :"$with_inet" = :yes ; then :;
	_XOPEN_SETUP_INET
    fi
    AC_MSG_CHECKING([for xopen inet driver])
    AC_MSG_RESULT([$with_inet])
    if test :"$with_xnet" = :yes ; then :;
	_XOPEN_SETUP_XNET
    fi
    AC_MSG_CHECKING([for xopen xnet library])
    AC_MSG_RESULT([$with_xnet])
    if test :"$with_sock" = :yes ; then :;
	_XOPEN_SETUP_SOCK
    fi
    AC_MSG_CHECKING([for xopen socket library])
    AC_MSG_RESULT([$with_sock])
])# _XOPEN_SETUP
# =============================================================================

# =============================================================================
# _XOPEN_OPENSS7_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_OPENSS7_SCTP], [dnl
    AC_CACHE_CHECK([for xopen sctp openss7 kernel], [xopen_cv_openss7_sctp], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
	    ], [xopen_cv_openss7_sctp=yes], [xopen_cv_openss7_sctp=no]) ]) ])
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], [test :"${xopen_cv_openss7_sctp:-no}" = :yes])dnl
])# _XOPEN_OPENSS7_SCTP
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_SCTP], [dnl
    AC_REQUIRE([_XOPEN_OPENSS7_SCTP])dnl
    if test :"${xopen_cv_openss7_sctp:-no}" = :yes ; then
	with_sctp='no'
	with_sctp2='no'
    fi
    if test :"${with_sctp2:-no}" = :yes ; then
	with_sctp='no'
    fi
])# _XOPEN_SETUP_SCTP
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_XNS
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_XNS], [dnl
])# _XOPEN_SETUP_XNS
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_TLI
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_TLI], [dnl
])# _XOPEN_SETUP_TLI
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_INET
# -----------------------------------------------------------------------------
# tcp_openreq_cachep            <-- extern, declared in <net/tcp.h>
# tcp_set_keepalive             <-- extern, declared in <net/tcp.h>
# ip_tos2prio                   <-- extern, declared in <net/route.h>
# sysctl_rmem_default           <-- extern, declared in net/core/sock.c
# sysctl_wmem_default           <-- extern, declared in net/core/sock.c
# sysctl_tcp_fin_timeout        <-- extern, declared in net/ipv4/tcp.c
# -----------------------------------------------------------------------------
# New ones: (All only exported with IPv6 as module.)
# tcp_sync_mss                  <-- extern, declared in <net/tcp.h>
# tcp_write_xmit                <-- extern, declared in <net/tcp.h>
# tcp_cwnd_application_limited  <-- extern, declared in <net/tcp.h>
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_INET], [dnl
    AC_REQUIRE([_XOPEN_OPENSS7_SCTP])dnl
    if test :"${xopen_cv_openss7_sctp:-no}" = :yes ; then
	AC_DEFINE([HAVE_OPENSS7_SCTP], [1],
	[Define if your kernel supports the OpenSS7 Linux Kernel Sockets SCTP
	patches.  This enables support in the INET driver for STREAMS on top
	of the OpenSS7 Linux Kernel Sockets SCTP implementation.])
    fi
    if test :"$with_inet" = :yes ; then
	_LINUX_KERNEL_SYMBOLS([tcp_openreq_cachep,
			       tcp_set_keepalive,
			       ip_tos2prio,
			       tcp_sync_mss,
			       tcp_write_xmit,
			       tcp_cwnd_application_limited,
			       sysctl_rmem_default,
			       sysctl_wmem_default,
			       sysctl_tcp_fin_timeout])
	_LINUX_CHECK_MEMBERS([struct sock.protinfo.af_inet.ttl,
			      struct sock.protinfo.af_inet.uc_ttl], [], [], [
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
    fi
])# _XOPEN_SETUP_INET
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_XNET
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_XNET], [dnl
])# _XOPEN_SETUP_XNET
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_SOCK
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_SOCK], [dnl
    with_sock='no'
])# _XOPEN_SETUP_SOCK
# =============================================================================

# =============================================================================
# _XOPEN_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_SCTP], [test :"$with_sctp" = :yes])dnl
    AM_CONDITIONAL([WITH_SCTP2], [test :"$with_sctp2" = :yes])dnl
    AM_CONDITIONAL([WITH_XNS], [test :"$with_xns" = :yes])dnl
    AM_CONDITIONAL([WITH_TLI], [test :"$with_tli" = :yes])dnl
    AM_CONDITIONAL([WITH_INET], [test :"$with_inet" = :yes])dnl
    AM_CONDITIONAL([WITH_XNET], [test :"$with_xnet" = :yes])dnl
    AM_CONDITIONAL([WITH_SOCK], [test :"$with_sock" = :yes])dnl
])# _XOPEN_OUTPUT
# =============================================================================

# =============================================================================
# _XOPEN_
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_], [dnl
])# _XOPEN_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# =============================================================================
# _XOPEN
# -----------------------------------------------------------------------------
# Common things that need to be done to support XOPEN/XNS networking.
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN], [dnl
    _XOPEN_OPTIONS
    _XOPEN_SETUP
    _XOPEN_OUTPUT
])# _XOPEN
# =============================================================================
