# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.20 $) $Date: 2005/03/07 06:10:04 $
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
# Last Modified $Date: 2005/03/07 06:10:04 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])
m4_include([m4/strconf.m4])
m4_include([m4/streams.m4])
m4_include([m4/xopen.m4])

# =============================================================================
# AC_SS7
# -----------------------------------------------------------------------------
AC_DEFUN([AC_SS7], [dnl
    _OPENSS7_PACKAGE([SS7], [OpenSS7 SS7 Stack])
    _SS7_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _SS7_SETUP
    SS7_INCLUDES="-imacros ./config.h"
    SS7_INCLUDES="${SS7_INCLUDES}${XNS_CPPFLAGS:+ }${XNS_CPPFLAGS}"
    SS7_INCLUDES="${SS7_INCLUDES}${XTI_CPPFLAGS:+ }${XTI_CPPFLAGS}"
    SS7_INCLUDES="${SS7_INCLUDES}${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    SS7_INCLUDES="${SS7_INCLUDES} -I./src/include -I${srcdir}/src/include"
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    LDFLAGS   = $USER_LDFLAGS])
    AC_MSG_NOTICE([final user    INCLUDES  = $SS7_INCLUDES])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final kernel  LDFLAGS   = $KERNEL_LDFLAGS])
    AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([SS7_INCLUDES])dnl
    CPPFLAGS=
    CFLAGS=
    _SS7_OUTPUT
    _AUTOTEST
])# AC_SS7
# =============================================================================

# =============================================================================
# _SS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OPTIONS], [dnl
])# _SS7_OPTIONS
# =============================================================================

# =============================================================================
# _SS7_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP], [dnl
    _LINUX_KERNEL
    _GENKSYMS
    _LINUX_STREAMS
    _XOPEN
])# _SS7_SETUP
# =============================================================================

# =============================================================================
# _SS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OUTPUT], [dnl
    _SS7_STRCONF dnl
])# _SS7_OUTPUT
# =============================================================================

# =============================================================================
# _SS7_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
dnl strconf_cv_input='Config.master'
    strconf_cv_majbase=180
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="${PACKAGE_TARNAME}_mknod.c"
dnl strconf_cv_stsetup='strsetup.conf'
dnl strconf_cv_strload='strload.conf'
    AC_REQUIRE([_LINUX_STREAMS])
    strconf_cv_package=${streams_cv_package:-LiS}
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
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
