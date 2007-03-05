# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/03/04 23:41:50 $
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
# Last Modified $Date: 2007/03/04 23:41:50 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.8  2007/03/04 23:41:50  brian
# - additional include path
#
# Revision 0.9.2.7  2007/03/04 23:30:29  brian
# - corrected modversions directory
#
# Revision 0.9.2.6  2007/03/04 23:14:40  brian
# - better search for modversions
#
# Revision 0.9.2.5  2007/03/02 10:04:53  brian
# - updates to common build process and versions for all exported symbols
#
# Revision 0.9.2.4  2006/12/29 05:52:03  brian
# - changes for successful master build
#
# Revision 0.9.2.3  2006/12/23 13:07:26  brian
# - manual page and other package updates for release
#
# Revision 0.9.2.2  2006/12/18 08:22:55  brian
# - resolve device numbering
#
# Revision 0.9.2.1  2006/10/16 10:54:53  brian
# - added new package files
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
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/nsl.m4])
m4_include([m4/sock.m4])
m4_include([m4/inet.m4])
m4_include([m4/sctp.m4])
m4_include([m4/chan.m4])
m4_include([m4/iso.m4])
m4_include([m4/isdn.m4])
m4_include([m4/ss7.m4])
m4_include([m4/sigtran.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_VOIP
# -----------------------------------------------------------------------------
AC_DEFUN([AC_VOIP], [dnl
    _OPENSS7_PACKAGE([VOIP], [OpenSS7 STREAMS VOIP])
    _VOIP_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strvoip-core.postinst
		     debian/strvoip-core.postrm
		     debian/strvoip-core.preinst
		     debian/strvoip-core.prerm
		     debian/strvoip-devel.preinst
		     debian/strvoip-dev.postinst
		     debian/strvoip-dev.preinst
		     debian/strvoip-dev.prerm
		     debian/strvoip-doc.postinst
		     debian/strvoip-doc.preinst
		     debian/strvoip-doc.prerm
		     debian/strvoip-init.postinst
		     debian/strvoip-init.postrm
		     debian/strvoip-init.preinst
		     debian/strvoip-init.prerm
		     debian/strvoip-lib.preinst
		     debian/strvoip-source.preinst
		     debian/strvoip-util.preinst
		     src/util/modutils/strvoip
		     src/util/sysconfig/strvoip
		     src/include/sys/strvoip/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _VOIP_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SIGTRAN_CPPFLAGS:+ }}${SIGTRAN_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${ISDN_CPPFLAGS:+ }}${ISDN_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${ISO_CPPFLAGS:+ }}${ISO_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${CHAN_CPPFLAGS:+ }}${CHAN_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SCTP_CPPFLAGS:+ }}${SCTP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${INET_CPPFLAGS:+ }}${INET_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SOCK_CPPFLAGS:+ }}${SOCK_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${NSL_CPPFLAGS:+ }}${NSL_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SS7_CPPFLAGS:+ }}${SS7_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    if test :${linux_cv_k_ko_modules:-no} = :no ; then
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
    PKG_MANPATH="${SS7_MANPATH:+${SS7_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XNS_MANPATH:+${XNS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XTI_MANPATH:+${XTI_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${NSL_MANPATH:+${NSL_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${SOCK_MANPATH:+${SOCK_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${INET_MANPATH:+${INET_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${SCTP_MANPATH:+${SCTP_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${CHAN_MANPATH:+${CHAN_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${ISO_MANPATH:+${ISO_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${ISDN_MANPATH:+${ISDN_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${SIGTRAN_MANPATH:+${SIGTRAN_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _VOIP_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_VOIP
# =============================================================================

# =============================================================================
# _VOIP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_OPTIONS], [dnl
    _VOIP_CHECK_VOIP
    _VOIP_CHECK_SCTP
])# _VOIP_OPTIONS
# =============================================================================

# =============================================================================
# _VOIP_CHECK_VOIP
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_CHECK_VOIP], [dnl
])# _VOIP_CHECK_VOIP
# =============================================================================

# =============================================================================
# _VOIP_CHECK_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_CHECK_SCTP], [dnl
])# _VOIP_CHECK_SCTP
# =============================================================================

# =============================================================================
# _VOIP_OTHER_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_OTHER_SCTP], [dnl
])# _VOIP_OTHER_SCTP
# =============================================================================

# =============================================================================
# _VOIP_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    _LINUX_STREAMS
    _STRCOMP
dnl with_voip='yes'
dnl _XOPEN
    _XNS
    _XTI
    _NSL
    _SOCK
    _INET
    _SCTP
    _CHAN
    _ISO
    _ISDN
    _SS7
    _SIGTRAN
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _VOIP_OTHER_SCTP
    _VOIP_SETUP_MODULE
    _VOIP_CONFIG_KERNEL
])# _VOIP_SETUP
# =============================================================================

# =============================================================================
# _VOIP_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_SETUP_MODULE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([VOIP_CONFIG_MODULE], [], [When defined, VOIP is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([VOIP_CONFIG], [], [When defined, VOIP is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([VOIP_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([VOIP_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _VOIP_SETUP_MODULE
# =============================================================================

# =============================================================================
# _VOIP_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# Later we should have some checks here for things like OSS, ALS, Zaptel, etc.
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_CONFIG_KERNEL], [dnl
])# _VOIP_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _VOIP_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_OUTPUT], [dnl
    _VOIP_CONFIG
    _VOIP_STRCONF dnl
])# _VOIP_OUTPUT
# =============================================================================

# =============================================================================
# _VOIP_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    voip_cv_config="${pkg_bld}/src/include/sys/strvoip/config.h"
    voip_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    voip_cv_ldadd= # "${pkg_bld}/libvoip.la"
    voip_cv_ldflags= # "-L${pkg_bld}/.libs/"
    voip_cv_ldadd32= # "${pkg_bld}/lib32/libvoip.la"
    voip_cv_ldflags32= # "-L${pkg_bld}/lib32/.libs/"
    voip_cv_manpath="${pkg_bld}/doc/man"
    voip_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    voip_cv_modmap="${pkg_bld}/Modules.map"
    voip_cv_symver="${pkg_bld}/Module.symvers"
    voip_cv_version="${PACAKGE_EPOCH}:${PACKAGE_VERSION}-${PACKAGE_RELEASE}"
])# _VOIP_CONFIG
# =============================================================================

# =============================================================================
# _VOIP_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=164
    strconf_cv_midbase=160
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
    _STRCONF dnl
])# _VOIP_STRCONF
# =============================================================================

# =============================================================================
# _VOIP_
# -----------------------------------------------------------------------------
AC_DEFUN([_VOIP_], [dnl
])# _VOIP_
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
