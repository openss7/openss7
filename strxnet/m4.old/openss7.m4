# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
# 
# @(#) kernel.m4,v LiS-2_16_18-1(0.9.2.1) 2004/01/13 16:11:34
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
# Last Modified 2004/01/13 16:11:34 by brian
#
# =============================================================================

# =============================================================================
# _OPENSS7
# -----------------------------------------------------------------------------
# Checks out a number of things particular to OpenSS7 releases.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7], [
])# _OPENSS7
# =============================================================================

# =========================================================================
# _OPENSS7_XNET_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_OPTIONS], [
    AC_ARG_ENABLE([inet],
                  AC_HELP_STRING([--enable-inet],
                                 [enable inet package. @<:@default=no@:>@]),
                  [enable_inet=$enableval],
                  [enable_inet=''])
    AC_ARG_ENABLE([sctp2],
                  AC_HELP_STRING([--enable-sctp2],
                                 [enable sctp2 package. @<:@default=no@:>@]),
                  [enable_sctp2=$enableval],
                  [enable_sctp2=''])
])# _OPENSS7_XNET_OPTIONS
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP], [
    _OPENSS7_XNET_CHECK_SCTP
    _OPENSS7_XNET_CHECK_XNS
    _OPENSS7_XNET_CHECK_TLI
    _OPENSS7_XNET_CHECK_INET
    _OPENSS7_XNET_CHECK_XNET
    _OPENSS7_XNET_CHECK_SOCK
])# _OPENSS7_XNET_SETUP
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_CHECK_SCTP
# -------------------------------------------------------------------------
# Check if the kernel supports OpenSS7 SCTP.  This is from an OpenSS7 kernel
# patch on the kernel.  If the kernel supports OpenSS7 Linux Native Sockets
# SCTP, then support will be configured for the SCTP in the kernel.
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_CHECK_SCTP], [
    AC_CACHE_CHECK([for sctp kernel], [os7_cv_have_sctp], [
        os7_cv_have_sctp=no
    ])
    AC_ARG_WITH([sctp],
        AS_HELP_STRING([--with-sctp],
            [include sctp kernel.  @<:@default=DETECTED@:>@]),
        [with_sctp="$enableval"],
        [with_sctp=''])
    AC_MSG_CHECKING([for package sctp kernel])
    case :"$with_sctp" in
        :yes)   os7_cv_need_sctp=yes ;;
        :no)    os7_cv_need_sctp=no  ;;
        :*) if test :"$sctp_cv_have_sctp" = :yes
            then os7_cv_need_sctp=no
            else os7_cv_need_sctp=yes
            fi ;;
    esac
    AC_MSG_RESULT([$os7_cv_need_sctp])
    if test :"$os7_cv_need_sctp" = :yes ; then :;
        _OPENSS7_XNET_SETUP_SCTP
    fi
    AM_CONDITIONAL_([WITH_SCTP], test :"$os7_cv_need_sctp" = :yes )
])# _OPENSS7_XNET_CHECK_SCTP
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_CHECK_XNS
# -------------------------------------------------------------------------
# Check if there are usable (recent OpenSS7) OSI header files.  These would be
# from a previous OpenSS7 installation.  If they are not usable, they will be
# replaced with local installed copies.  Also, this decides whether to install
# manual pages as well.
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_CHECK_XNS], [
    AC_CACHE_CHECK([for xns headers], [os7_cv_have_xns], [
        os7_cv_have_xns=no
    ])
    AC_ARG_WITH([xns],
        AS_HELP_STRING([--with-xns],
            [include xns headers.  @<:@default=DETECTED@:>@]),
        [with_xns="$enableval"],
        [with_xns=''])
    AC_MSG_CHECKING([for package xns headers])
    case :"$with_xns" in
        :yes)   os7_cv_need_xns=yes ;;
        :no)    os7_cv_need_xns=no  ;;
        :*) if test :"$xns_cv_have_xns" = :yes
            then os7_cv_need_xns=no
            else os7_cv_need_xns=yes
            fi ;;
    esac
    AC_MSG_RESULT([$os7_cv_need_xns])
    if test :"$os7_cv_need_xns" = :yes ; then :;
        _OPENSS7_XNET_SETUP_XNS
    fi
    AM_CONDITIONAL_([WITH_XNS], test :"$os7_cv_need_xns" = :yes )
])# _OPENSS7_XNET_CHECK_XNS
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_CHECK_TLI
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_CHECK_TLI], [
    AC_CACHE_CHECK([for tli modules], [os7_cv_have_tli], [
        os7_cv_have_tli=no
    ])
    AC_ARG_WITH([tli],
        AS_HELP_STRING([--with-tli],
            [include tli modules.  @<:@default=DETECTED@:>@]),
        [with_tli="$enableval"],
        [with_tli=''])
    AC_MSG_CHECKING([for package tli modules])
    case :"$with_tli" in
        :yes)   os7_cv_need_tli=yes ;;
        :no)    os7_cv_need_tli=no  ;;
        :*) if test :"$tli_cv_have_tli" = :yes
            then os7_cv_need_tli=no
            else os7_cv_need_tli=yes
            fi ;;
    esac
    AC_MSG_RESULT([$os7_cv_need_tli])
    if test :"$os7_cv_need_tli" = :yes ; then :;
        _OPENSS7_XNET_SETUP_TLI
    fi
    AM_CONDITIONAL_([WITH_TLI], test :"$os7_cv_need_tli" = :yes )
])# _OPENSS7_XNET_CHECK_TLI
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_CHECK_INET
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_CHECK_INET], [
    AC_CACHE_CHECK([for inet driver], [os7_cv_have_inet], [
        os7_cv_have_inet=no
    ])
    AC_ARG_WITH([inet],
        AS_HELP_STRING([--with-inet],
            [include inet driver.  @<:@default=DETECTED@:>@]),
        [with_inet="$enableval"],
        [with_inet=''])
    AC_MSG_CHECKING([for package inet driver])
    case :"$with_inet" in
        :yes)   os7_cv_need_inet=yes ;;
        :no)    os7_cv_need_inet=no  ;;
        :*) if test :"$inet_cv_have_inet" = :yes
            then os7_cv_need_inet=no
            else os7_cv_need_inet=yes
            fi ;;
    esac
    AC_MSG_RESULT([$os7_cv_need_inet])
    if test :"$os7_cv_need_inet" = :yes ; then :;
        _OPENSS7_XNET_SETUP_INET
    fi
    AM_CONDITIONAL_([WITH_INET], test :"$os7_cv_need_inet" = :yes )
])# _OPENSS7_XNET_CHECK_INET
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_CHECK_XNET
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_CHECK_XNET], [
    AC_CACHE_CHECK([for xnet library], [os7_cv_have_xnet], [
        os7_cv_have_xnet=no
    ])
    AC_ARG_WITH([xnet],
        AS_HELP_STRING([--with-xnet],
            [include xnet library.  @<:@default=DETECTED@:>@]),
        [with_xnet="$enableval"],
        [with_xnet=''])
    AC_MSG_CHECKING([for package xnet library])
    case :"$with_xnet" in
        :yes)   os7_cv_need_xnet=yes ;;
        :no)    os7_cv_need_xnet=no  ;;
        :*) if test :"$os7_cv_have_xnet" = :yes
            then os7_cv_need_xnet=no
            else os7_cv_need_xnet=yes
            fi ;;
    esac
    AC_MSG_RESULT([$os7_cv_need_xnet])
    if test :"$os7_cv_need_xnet" = :yes ; then :;
        _OPENSS7_XNET_SETUP_XNET
    fi
    AM_CONDITIONAL_([WITH_XNET], test :"$os7_cv_need_xnet" = :yes )
])# _OPENSS7_XNET_CHECK_XNET
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_CHECK_SOCK
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_CHECK_SOCK], [
    AC_CACHE_CHECK([for socket library], [os7_cv_have_sock], [
        os7_cv_have_sock=no
    ])
    AC_ARG_WITH([sock],
        AS_HELP_STRING([--with-sock],
            [include socket library.  @<:@default=DETECTED@:>@]),
        [with_sock="$enableval"],
        [with_sock=''])
    AC_MSG_CHECKING([for package socket library])
    case :"$with_sock" in
        :yes)   os7_cv_need_sock=yes ;;
        :no)    os7_cv_need_sock=no  ;;
        :*) if test :"$sock_cv_have_sock" = :yes
            then os7_cv_need_sock=no
            else os7_cv_need_sock=yes
            fi ;;
    esac
    AC_MSG_RESULT([$os7_cv_need_sock])
    if test :"$os7_cv_need_sock" = :yes ; then :;
        _OPENSS7_XNET_SETUP_SOCK
    fi
    AM_CONDITIONAL_([WITH_SOCK], test :"$os7_cv_need_sock" = :yes )
])# _OPENSS7_XNET_CHECK_SOCK
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP_SCTP
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP_SCTP], [
])# _OPENSS7_XNET_SETUP_SCTP
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP_XNS
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP_XNS], [
])# _OPENSS7_XNET_SETUP_XNS
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP_TLI
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP_TLI], [
])# _OPENSS7_XNET_SETUP_TLI
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP_INET
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP_INET], [
])# _OPENSS7_XNET_SETUP_INET
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP_XNET
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP_XNET], [
])# _OPENSS7_XNET_SETUP_XNET
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_SETUP_SOCK
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_SETUP_SOCK], [
])# _OPENSS7_XNET_SETUP_SOCK
# =========================================================================

# =========================================================================
# _OPENSS7_XNET_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_XNET_OUTPUT], [
])# _OPENSS7_XNET_OUTPUT
# =========================================================================

# =============================================================================
# 
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
