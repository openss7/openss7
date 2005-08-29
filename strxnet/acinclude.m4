# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.35 $) $Date: 2005/08/29 10:19:15 $
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
# Last Modified $Date: 2005/08/29 10:19:15 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
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

# =============================================================================
# AC_XNET
# -----------------------------------------------------------------------------
AC_DEFUN([AC_XNET], [dnl
    _OPENSS7_PACKAGE([XNET], [OpenSS7 XTI/TLI Networking])
    _XNET_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strxnet-core.postinst
		     debian/strxnet-core.postrm
		     debian/strxnet-core.preinst
		     debian/strxnet-core.prerm
		     debian/strxnet-devel.preinst
		     debian/strxnet-dev.postinst
		     debian/strxnet-dev.preinst
		     debian/strxnet-dev.prerm
		     debian/strxnet-doc.postinst
		     debian/strxnet-doc.preinst
		     debian/strxnet-doc.prerm
		     debian/strxnet-init.postinst
		     debian/strxnet-init.postrm
		     debian/strxnet-init.preinst
		     debian/strxnet-init.prerm
		     debian/strxnet-lib.preinst
		     debian/strxnet-source.preinst
		     debian/strxnet-util.preinst
		     src/util/modutils/strxnet
		     src/include/sys/strxnet/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _XNET_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/$(STRCONF_CONFIG)'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/src/include -I$(top_srcdir)/src/include'
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
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _XNET_OUTPUT
    _AUTOTEST
])# AC_XNET
# =============================================================================

# =============================================================================
# _XNET_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_OPTIONS], [dnl
])# _XNET_OPTIONS
# =============================================================================

# =============================================================================
# _XNET_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    _LINUX_STREAMS
    _STRCOMP
    with_tli='yes'
    with_xnet='yes'
    _XOPEN
])# _XNET_SETUP
# =============================================================================

# =============================================================================
# _XNET_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_OUTPUT], [dnl
    _XNET_STRCONF dnl
])# _XNET_OUTPUT
# =============================================================================

# =============================================================================
# _XNET_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=245
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
])# _XNET_STRCONF
# =============================================================================

# =============================================================================
# _XNET_
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_], [dnl
])# _XNET_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
