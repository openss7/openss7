# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
# 
# @(#) $RCSfile: xopen.m4,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/05/24 03:37:04 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
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
# Last Modified $Date: 2004/05/24 03:37:04 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _XOPEN
# -----------------------------------------------------------------------------
# Common things that need to be done to support XOPEN/XNS networking.
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN], [
    AC_REQUIRE([_LINUX_KERNEL])
    _XOPEN_OPTIONS
    _XOPEN_SETUP
    _XOPEN_OUTPUT
])# _XOPEN
# =============================================================================

# =============================================================================
# _XOPEN_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_OPTIONS], [
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
AC_DEFUN([_XOPEN_CHECK_SCTP], [
    AC_ARG_WITH([sctp],
        AS_HELP_STRING([--with-sctp],
            [include xopen sctp driver in build.  @<:@default=yes@:>@]),
        [with_sctp="$withval"],
        [with_sctp='yes'])
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
AC_DEFUN([_XOPEN_CHECK_XNS], [
    AC_ARG_WITH([xns],
        AS_HELP_STRING([--with-xns],
            [include xopen xns headers in install.  @<:@default=yes@:>@]),
        [with_xns="$withval"],
        [with_xns='yes'])
])# _XOPEN_CHECK_XNS
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_TLI
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_TLI], [
    AC_ARG_WITH([tli],
        AS_HELP_STRING([--with-tli],
            [include xopen tli modules in build.  @<:@default=yes@:>@]),
        [with_tli="$withval"],
        [with_tli='yes'])
])# _XOPEN_CHECK_TLI
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_INET
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_INET], [
    AC_ARG_WITH([inet],
        AS_HELP_STRING([--with-inet],
            [include xopen inet driver in build.  @<:@default=yes@:>@]),
        [with_inet="$withval"],
        [with_inet='yes'])
])# _XOPEN_CHECK_INET
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_XNET
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_XNET], [
    AC_ARG_WITH([xnet],
        AS_HELP_STRING([--with-xnet],
            [include xopen xnet library in build.  @<:@default=yes@:>@]),
        [with_xnet="$withval"],
        [with_xnet='yes'])
])# _XOPEN_CHECK_XNET
# =============================================================================

# =============================================================================
# _XOPEN_CHECK_SOCK
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_CHECK_SOCK], [
    AC_ARG_WITH([sock],
        AS_HELP_STRING([--with-sock],
            [include xopen sock library in build.  @<:@default=yes@:>@]),
        [with_sock="$withval"],
        [with_sock='yes'])
])# _XOPEN_CHECK_SOCK
# =============================================================================

# =============================================================================
# _XOPEN_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP], [
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
AC_DEFUN([_XOPEN_OPENSS7_SCTP], [
    AC_CACHE_CHECK([for xopen sctp openss7 kernel], [xopen_cv_openss7_sctp], [
        _LINUX_KERNEL_ENV([
            AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
            ], [xopen_cv_openss7_sctp=yes], [xopen_cv_openss7_sctp=no])
        ])
    ])
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], test :"${xopen_cv_openss7_sctp:-no}" = :yes)
])# _XOPEN_OPENSS7_SCTP
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_SCTP], [
    AC_REQUIRE([_XOPEN_OPENSS7_SCTP])
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
AC_DEFUN([_XOPEN_SETUP_XNS], [
])# _XOPEN_SETUP_XNS
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_TLI
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_TLI], [
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
AC_DEFUN([_XOPEN_SETUP_INET], [
    AC_REQUIRE([_XOPEN_OPENSS7_SCTP])
    if test :"${xopen_cv_openss7_sctp:-no}" = :yes ; then
        AC_DEFINE([HAVE_OPENSS7_SCTP], [1],
        [Define if your kernel supports the OpenSS7 Linux Kernel Sockets SCTP
        patches.  This enables support in the INET driver for STREAMS on top
        of the OpenSS7 Linux Kernel Sockets SCTP implementation.])
    fi
    # these are critical
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([tcp_openreq_cachep], [with_inet='no'])
    fi
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([tcp_set_keepalive], [with_inet='no'])
    fi
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([ip_tos2prio], [with_inet='no'])
    fi
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([tcp_sync_mss], [with_inet='no'])
    fi
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([tcp_write_xmit], [with_inet='no'])
    fi
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([tcp_cwnd_application_limited], [with_inet='no'])
    fi
    # these are not critical
    if test :"$with_inet" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_rmem_default])
        _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_wmem_default])
        _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_tcp_fin_timeout])
        _LINUX_KERNEL_ENV([
            AC_CHECK_MEMBER([struct sock.protinfo.af_inet.ttl],
                [xopen_cv_af_inet_ttl_member_name='ttl'],
                [:],
                [
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
            AC_CHECK_MEMBER([struct sock.protinfo.af_inet.uc_ttl],
                [xopen_cv_af_inet_ttl_member_name='uc_ttl'],
                [:],
                [
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
        if test :"${xopen_cv_af_inet_ttl_member_name:+set}" = :set ; then
            AC_DEFINE_UNQUOTED([USING_AF_INET_TTL_MEMBER_NAME], [$xopen_cv_af_inet_ttl_member_name], [Most
            kernels call the time-to-live member of the af_inet structure ttl.  For some reason
            (probably because the old ttl member as 'int' and the new uc_ttl member is 'unsigned char')
            reported by Bala Viswanathan <balav@lsil.com> to the linux-streams mailing list, EL3 renames
            the member to uc_ttl on some kernels.  Define this to the member name used (ttl or uc_ttl)
            so that the inet driver can be properly supported.  If this is not defined, 'ttl' will be
            used as a default.])
        else
            with_inet='no'
        fi
    fi
])# _XOPEN_SETUP_INET
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_XNET
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_XNET], [
])# _XOPEN_SETUP_XNET
# =============================================================================

# =============================================================================
# _XOPEN_SETUP_SOCK
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_SETUP_SOCK], [
    with_sock='no'
])# _XOPEN_SETUP_SOCK
# =============================================================================

# =============================================================================
# _XOPEN_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_OUTPUT], [
    AM_CONDITIONAL([WITH_SCTP], test :"$with_sctp" = :yes )
    AM_CONDITIONAL([WITH_SCTP2], test :"$with_sctp2" = :yes )
    AM_CONDITIONAL([WITH_XNS], test :"$with_xns" = :yes )
    AM_CONDITIONAL([WITH_TLI], test :"$with_tli" = :yes )
    AM_CONDITIONAL([WITH_INET], test :"$with_inet" = :yes )
    AM_CONDITIONAL([WITH_XNET], test :"$with_xnet" = :yes )
    AM_CONDITIONAL([WITH_SOCK], test :"$with_sock" = :yes )
])# _XOPEN_OUTPUT
# =============================================================================

# =============================================================================
# _XOPEN_
# -----------------------------------------------------------------------------
AC_DEFUN([_XOPEN_], [
])# _XOPEN_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
