# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/03/04 23:14:28 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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
# Last Modified $Date: 2007/03/04 23:14:28 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.5  2007/03/04 23:14:28  brian
# - better search for modversions
#
# Revision 0.9.2.4  2007/03/02 10:04:31  brian
# - updates to common build process and versions for all exported symbols
#
# Revision 0.9.2.3  2006/12/18 08:21:17  brian
# - resolve device numbering
#
# Revision 0.9.2.2  2006/09/26 00:51:18  brian
# - corrected macro file name, reordered includes
#
# Revision 0.9.2.1  2006/09/25 12:30:44  brian
# - added files for new strnsl package
#
# Revision 0.9.2.3  2006/09/18 13:20:14  brian
# - better directory detection
#
# Revision 0.9.2.2  2006/09/18 01:15:25  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.1  2006/08/23 10:03:52  brian
# - started STREAMS Sockets package
#
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
dnl m4_include([m4/devfs.m4])
dnl m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])
dnl m4_include([m4/strconf.m4])
m4_include([m4/streams.m4])
m4_include([m4/strcomp.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_XNSL
# -----------------------------------------------------------------------------
AC_DEFUN([AC_XNSL], [dnl
    _OPENSS7_PACKAGE([XNSL], [OpenSS7 STREAMS Network Services Library])
    _XNSL_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strnsl-core.postinst
		     debian/strnsl-core.postrm
		     debian/strnsl-core.preinst
		     debian/strnsl-core.prerm
		     debian/strnsl-devel.preinst
		     debian/strnsl-dev.postinst
		     debian/strnsl-dev.preinst
		     debian/strnsl-dev.prerm
		     debian/strnsl-doc.postinst
		     debian/strnsl-doc.preinst
		     debian/strnsl-doc.prerm
		     debian/strnsl-init.postinst
		     debian/strnsl-init.postrm
		     debian/strnsl-init.preinst
		     debian/strnsl-init.prerm
		     debian/strnsl-lib.preinst
		     debian/strnsl-source.preinst
		     debian/strnsl-util.preinst
		     src/include/sys/strnsl/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _XNSL_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    if test :${linux_cv_ko_modules:-no} = :no ; then
	PKG_MODFLAGS='$(STREAMS_MODFLAGS) $(STRCOMP_MODFLAGS)'
dnl	if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	    PKG_MODFLAGS="${PKG_MODFLAGS}${PKG_MODFLAGS:+ }"'-include ${top_builddir}/${MODVERSIONS_H}'
dnl	    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/include'
dnl	fi
    fi
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
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
dnl _XNSL_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_XNSL
# =============================================================================

# =============================================================================
# _XNSL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_XNSL_OPTIONS], [dnl
])# _XNSL_OPTIONS
# =============================================================================

# =============================================================================
# _XNSL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XNSL_SETUP], [dnl
    _LINUX_KERNEL
dnl _LINUX_DEVFS
dnl _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LINUX_STREAMS
    _STRCOMP
    _XNS
    _XTI
])# _XNSL_SETUP
# =============================================================================

# =============================================================================
# _XNSL_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_XNSL_OUTPUT], [dnl
    _XNSL_CONFIG
dnl _XNSL_STRCONF
])# _XNSL_OUTPUT
# =============================================================================

# =============================================================================
# _XNSL_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_XNSL_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    nsl_cv_config="${pkg_bld}/src/include/sys/strnsl/config.h"
    nsl_cv_includes="${pkg_bld}/src/include ${pkg_src}/src/include"
    nsl_cv_ldadd="${pkg_bld}/libxnsl.la"
    nsl_cv_ldflags="${pkg_bld}/lib32/libxnsl.la"
    nsl_cv_ldadd32="-L${pkg_bld}/.libs/"
    nsl_cv_ldflags32="${pkg_bld}/lib32/.libs/"
    nsl_cv_manpath="${pkg_bld}/doc/man"
    nsl_cv_modversions="${pkg_bld}/include/$linux_cv_k_release/$target_cpu/sys/${PACKAGE}/modversions.h"
    nsl_cv_modmap= # "${pkg_bld}/Modules.map"
    nsl_cv_symver= # "${pkg_bld}/Module.symvers"
    nsl_cv_version="${PACAKGE_EPOCH}:${PACKAGE_VERSION}-${PACKAGE_RELEASE}"
])# _XNSL_CONFIG
# =============================================================================

# =============================================================================
# _XNSL_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_XNSL_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=220
    strconf_cv_midbase=70
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
])# _XNSL_STRCONF
# =============================================================================

# =============================================================================
# _XNSL_
# -----------------------------------------------------------------------------
AC_DEFUN([_XNSL_], [dnl
])# _XNSL_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
