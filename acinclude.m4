# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/02/20 09:54:39 $
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
# Last Modified $Date: 2005/02/20 09:54:39 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/streams.m4])
m4_include([m4/xopen.m4])
m4_include([m4/xti.m4])
m4_include([m4/xns.m4])
m4_include([m4/sctp.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/strconf.m4])

# =============================================================================
# AC_OS7
# -----------------------------------------------------------------------------
AC_DEFUN([AC_OS7], [dnl
    _OS7_OPTIONS
    _OPENSS7_PACKAGE([OpenSS7], [OpenSS7 Master Package])
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _DEB_DPKG
    _LDCONFIG
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    _LINUX_KERNEL
    _LINUX_STREAMS
    _XNS
    _XTI
    _INET
    _SCTP
dnl AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
dnl AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
dnl AC_MSG_NOTICE([final kern CPPFLAGS  = $KERNEL_CPPFLAGS])
dnl AC_MSG_NOTICE([final kern CFLAGS    = $KERNEL_CFLAGS])
dnl AC_MSG_NOTICE([final kern MODFLAGS  = $KERNEL_MODFLAGS])
dnl AC_MSG_NOTICE([final kern NOVERSION = $KERNEL_NOVERSION])
#   vars=`(set) 2>&1 | egrep '^(ac|man|rpm|linux|xns|xti|inet|sctp|streams)_cv_.*=' | sed -e 's|=.*||;s|^|declare -x |;s|$|; |'`
#   echo $vars
#   eval "$vars"
])# AC_OS7
# =============================================================================

# =============================================================================
# _OS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_OPTIONS], [dnl
    AC_ARG_WITH([SCTP],
		AS_HELP_STRING([--without-SCTP],
			       [do not include SCTP in master pack @<:@included@:>@]),
		[with_SCTP="$withval"],
		[with_SCTP='yes'])
    if test :${with_SCTP:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([sctp])
    fi
    AC_ARG_WITH([IPERF],
		AS_HELP_STRING([--without-IPERF],
			       [do not include IPERF in master pack @<:@included@:>@]),
		[with_IPERF="$withval"],
		[with_IPERF='yes'])
    if test :${with_IPERF:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([iperf])
    fi
    AC_ARG_WITH([LIS],
		AS_HELP_STRING([--without-LIS],
			       [do not include LIS in master pack @<:@included@:>@]),
		[with_LIS="$withval"],
		[with_LIS='yes'])
    if test :${with_LIS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([LiS])
    fi
    AC_ARG_WITH([STREAMS],
		AS_HELP_STRING([--without-STREAMS],
			       [do not include STREAMS in master pack @<:@included@:>@]),
		[with_STREAMS="$withval"],
		[with_STREAMS='yes'])
    if test :${with_STREAMS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([streams])
    fi
    AC_ARG_WITH([STRXNS],
		AS_HELP_STRING([--without-STRXNS],
			       [do not include STRXNS in master pack @<:@included@:>@]),
		[with_STRXNS="$withval"],
		[with_STRXNS='yes'])
    if test :${with_STRXNS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strxns])
    fi
    AC_ARG_WITH([STRXNET],
		AS_HELP_STRING([--without-STRXNET],
			       [do not include STRXNET in master pack @<:@included@:>@]),
		[with_STRXNET="$withval"],
		[with_STRXNET='yes'])
    if test :${with_STRXNET:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strxnet])
    fi
    AC_ARG_WITH([STRINET],
		AS_HELP_STRING([--without-STRINET],
			       [do not include STRINET in master pack @<:@included@:>@]),
		[with_STRINET="$withval"],
		[with_STRINET='yes'])
    if test :${with_STRINET:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strinet])
    fi
    AC_ARG_WITH([STRSCTP],
		AS_HELP_STRING([--without-STRSCTP],
			       [do not include STRSCTP in master pack @<:@included@:>@]),
		[with_STRSCTP="$withval"],
		[with_STRSCTP='yes'])
    if test :${with_STRSCTP:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strsctp])
    fi
    AC_ARG_WITH([NETPERF],
		AS_HELP_STRING([--without-NETPERF],
			       [do not include NETPERF in master pack @<:@included@:>@]),
		[with_NETPERF="$withval"],
		[with_NETPERF='yes'])
    if test :${with_NETPERF:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([netperf])
    fi
    AC_ARG_WITH([STACKS],
		AS_HELP_STRING([--without-STACKS],
			       [do not include STACKS in master pack @<:@included@:>@]),
		[with_STACKS="$withval"],
		[with_STACKS='yes'])
    if test :${with_STACKS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([stacks])
    fi
])# _OS7_OPTIONS
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
