# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/02/20 09:54:41 $
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
# Last Modified $Date: 2005/02/20 09:54:41 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/kernel.m4])
m4_include([m4/streams.m4])
m4_include([m4/xopen.m4])
m4_include([m4/xti.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/strconf.m4])

# =============================================================================
# AC_INET
# -----------------------------------------------------------------------------
AC_DEFUN([AC_INET], [dnl
    _OPENSS7_PACKAGE([INET], [OpenSS7 INET Networking])
    _INET_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _DEB_DPKG
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    _LINUX_KERNEL
    _LINUX_STREAMS
    with_inet='yes'
    _XOPEN
    _XTI
    INET_INCLUDES="-I- -imacros ./config.h -I./src/include -I${srcdir}/src/include${XTI_CPPFLAGS:+ }${XTI_CPPFLAGS}${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    INCLUDES  = $INET_INCLUDES])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([INET_INCLUDES])dnl
    CPPFLAGS=
    CFLAGS=
    _INET_SETUP
    _INET_OUTPUT dnl
])# AC_INET
# =============================================================================

# =============================================================================
# _INET_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_OPTIONS], [dnl
])# _INET_OPTIONS
# =============================================================================

# =============================================================================
# _INET_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_SETUP], [dnl
    AC_CACHE_CHECK([for sctp openss7 kernel], [inet_cv_openss7_sctp], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
	    ], [inet_cv_openss7_sctp=yes], [inet_cv_openss7_sctp=no]) ]) ])
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], test :"${inet_cv_openss7_sctp:-no}" = :yes)dnl
])# _INET_SETUP
# =============================================================================

# =============================================================================
# _INET_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_OUTPUT], [dnl
    _INET_STRCONF dnl
])# _INET_OUTPUT
# =============================================================================

# =============================================================================
# _INET_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
dnl strconf_cv_input='Config.master'
    strconf_cv_majbase=245
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="${PACKAGE_TARNAME}_mknod.c"
    strconf_cv_strsetup='inetsetup.conf'
    strconf_cv_strload='inetload.conf'
    AC_REQUIRE([_LINUX_STREAMS])
    strconf_cv_package=${streams_cv_package:-LiS}
    _STRCONF dnl
])# _INET_STRCONF
# =============================================================================

# =============================================================================
# _INET_
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_], [dnl
])# _INET_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
