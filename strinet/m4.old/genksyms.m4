# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
# 
# @(#) $RCSfile: genksyms.m4,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/05/23 07:24:25 $
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
# Last Modified $Date: 2004/05/23 07:24:25 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _GENKSYMS
# -----------------------------------------------------------------------------
AC_DEFUN([_GENKSYMS], [
    AC_REQUIRE([_LINUX_KERNEL])
    _KSYMS_OPTIONS
    _LINUX_KERNEL_ENV([
        _KSYMS_SETUP
    ])
    _KSYMS_OUTPUT
])# _GENKSYMS
# =============================================================================

# =============================================================================
# _KSYMS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OPTIONS], [
dnl AC_ARG_ENABLE([k-versions],
dnl     AS_HELP_STRING([--enable-k-versions],
dnl         [version all symbols @<:@default=automatic@:>@]),
dnl     [enable_k_versions=$enableval],
dnl     [enable_k_versions=''])
])# _KSYMS_OPTIONS
# =============================================================================

# =============================================================================
# _KSYMS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_SETUP], [
    AC_CACHE_CHECK([for SMP kernel], [ksyms_cv_smp], [
        AC_EGREP_CPP([\<yes_we_have_an_smp_kernel\>], [
#include <linux/version.h>
#include <linux/config.h>
#ifdef CONFIG_SMP
    yes_we_have_an_smp_kernel
#endif
        ], [ksyms_cv_smp=yes], [ksyms_cv_smp=no])
    ])
    if test :"${ksyms_cv_smp:-no}" = :yes ; then
        GENKSYMS_SMP_PREFIX='-p smp_'
    else
        GENKSYMS_SMP_PREFIX=''
    fi
    AC_ARG_VAR([GENKSYMS], [Generate kernel symbols command])
    AC_PATH_TOOL([GENKSYMS], [genksyms], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${GENKSYMS:-no}" = :no ; then
        AC_MSG_WARN([Could not find genksyms program in PATH.])
    fi
])# _KSYMS_SETUP
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT], [
    AC_SUBST([GENKSYMS_SMP_PREFIX])
])# _KSYMS_OUTPUT
# =============================================================================

# =============================================================================
# _KSYMS_
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_], [
])# _KSYMS_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
