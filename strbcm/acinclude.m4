# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2006/09/18 13:20:08 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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
# Last Modified $Date: 2006/09/18 13:20:08 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.5  2006/09/18 13:20:08  brian
# - better directory detection
#
# Revision 0.9.2.4  2006/07/25 06:38:58  brian
# - expanded minor device numbers and optimization and locking corrections
#
# Revision 0.9.2.3  2006/05/08 03:12:27  brian
# - added module id base to strconf
#
# Revision 0.9.2.2  2006/03/21 13:24:13  brian
# - added problem report checks
#
# Revision 0.9.2.1  2006/03/08 11:42:09  brian
# - setting up package
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

# =============================================================================
# AC_BCM
# -----------------------------------------------------------------------------
AC_DEFUN([AC_BCM], [dnl
    _OPENSS7_PACKAGE([BCM], [OpenSS7 STREAMS Binary Compatibility Modules])
    _BCM_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strbcm-core.postinst
		     debian/strbcm-core.postrm
		     debian/strbcm-core.preinst
		     debian/strbcm-core.prerm
		     debian/strbcm-devel.preinst
		     debian/strbcm-dev.postinst
		     debian/strbcm-dev.preinst
		     debian/strbcm-dev.prerm
		     debian/strbcm-doc.postinst
		     debian/strbcm-doc.preinst
		     debian/strbcm-doc.prerm
		     debian/strbcm-init.postinst
		     debian/strbcm-init.postrm
		     debian/strbcm-init.preinst
		     debian/strbcm-init.prerm
		     debian/strbcm-lib.preinst
		     debian/strbcm-source.preinst
		     debian/strbcm-util.preinst
		     src/util/modutils/strbcm
		     src/util/sysconfig/strbcm
		     src/include/sys/strbcm/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _BCM_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
dnl if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
dnl fi
    PKG_MODFLAGS='$(STREAMS_MODFLAGS)'
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
    _BCM_OUTPUT
    _AUTOTEST
])# AC_BCM
# =============================================================================

# =============================================================================
# _BCM_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_OPTIONS], [dnl
])# _BCM_OPTIONS
# =============================================================================

# =============================================================================
# _BCM_SETUP_DEBUG
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_SETUP_DEBUG], [dnl
    case "$linux_cv_debug" in
	_DEBUG)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DEBUG], [], [Define to perform
		    internal structure tracking within the STREAMS executive
		    as well as to provide additional /proc filesystem files
		    for examining internal structures.])
	    ;;
	_TEST)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TEST], [], [Define to perform
		    performance testing with debugging.  This mode does not
		    dump massive amounts of information into system logs, but
		    peforms all assertion checks.])
	    ;;
	_SAFE)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAFE], [], [Define to perform
		    fundamental assertion checks.  This is a safer mode of
		    operation.])
	    ;;
	_NONE | *)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NONE], [], [Define to perform
		    no assertion checks but report software errors.  This is
		    the smallest footprint, highest performance mode of
		    operation.])
	    ;;
    esac
])# _BCM_SETUP_DEBUG
# =============================================================================

# =============================================================================
# _BCM_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _BCM_SETUP_DEBUG
    _LINUX_STREAMS
    _STRCOMP
])# _BCM_SETUP
# =============================================================================

# =============================================================================
# _BCM_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_OUTPUT], [dnl
    _BCM_STRCONF
])# _BCM_OUTPUT
# =============================================================================

# =============================================================================
# _BCM_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=242
    strconf_cv_midbase=20
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
    _STRCONF
])# _BCM_STRCONF
# =============================================================================

# =============================================================================
# _BCM_
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_], [dnl
])# _BCM_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
