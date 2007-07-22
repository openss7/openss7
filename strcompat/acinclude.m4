# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.31 $) $Date: 2007/07/22 01:10:21 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2007/07/22 01:10:21 $ by $Author: brian $
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

# =============================================================================
# AC_COMPAT
# -----------------------------------------------------------------------------
AC_DEFUN([AC_COMPAT], [dnl
    _OPENSS7_PACKAGE([SCOMPAT], [OpenSS7 STREAMS Compatibility])
    _COMPAT_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strcompat-core.postinst
		     debian/strcompat-core.postrm
		     debian/strcompat-core.preinst
		     debian/strcompat-core.prerm
		     debian/strcompat-devel.preinst
		     debian/strcompat-dev.postinst
		     debian/strcompat-dev.preinst
		     debian/strcompat-dev.prerm
		     debian/strcompat-doc.postinst
		     debian/strcompat-doc.preinst
		     debian/strcompat-doc.prerm
		     debian/strcompat-init.postinst
		     debian/strcompat-init.postrm
		     debian/strcompat-init.preinst
		     debian/strcompat-init.prerm
		     debian/strcompat-lib.preinst
		     debian/strcompat-source.preinst
		     debian/strcompat-util.preinst
		     src/util/modutils/strcompat
		     src/include/sys/strcompat/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _COMPAT_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    if test :${linux_cv_k_ko_modules:-no} = :no ; then
	PKG_MODFLAGS='$(STREAMS_MODFLAGS)'
	if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	    PKG_MODFLAGS="${PKG_MODFLAGS}${PKG_MODFLAGS:+ }"'-include ${top_builddir}/${MODVERSIONS_H}'
	    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/include'
	fi
    fi
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
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
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _COMPAT_OUTPUT
    _AUTOTEST
])# AC_COMPAT
# =============================================================================

# =============================================================================
# _COMPAT_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_OPTIONS], [dnl
])# _COMPAT_OPTIONS
# =============================================================================

# =============================================================================
# _COMPAT_SETUP_COMPAT
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_SETUP_COMPAT], [dnl
    AC_ARG_ENABLE([compat-os7],
	AS_HELP_STRING([--enable-compat-os7],
	    [enable source compatibility with OpenSS7 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_os7="$enableval"],
	    [enable_compat_os7='module'])
    AC_ARG_ENABLE([compat-svr3],
	AS_HELP_STRING([--enable-compat-svr3],
	    [enable source compatibility with SVR 4.2 MP variants.
	    @<:@default=module@:>@]),
	    [enable_compat_svr3="$enableval"],
	    [enable_compat_svr3='module'])
    AC_ARG_ENABLE([compat-svr4],
	AS_HELP_STRING([--enable-compat-svr4],
	    [enable source compatibility with SVR 4.2 MP variants.
	    @<:@default=module@:>@]),
	    [enable_compat_svr4="$enableval"],
	    [enable_compat_svr4='module'])
    AC_ARG_ENABLE([compat-mps],
	AS_HELP_STRING([--enable-compat-mps],
	    [enable source compatibility with MPS variants.
	    @<:@default=moudle@:>@]),
	    [enable_compat_mps="$enableval"],
	    [enable_compat_mps='module'])
    AC_ARG_ENABLE([compat-sol8],
	AS_HELP_STRING([--enable-compat-sol8],
	    [enable source compatibility with Solaris 8 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_sol8="$enableval"],
	    [enable_compat_sol8='module'])
    AC_ARG_ENABLE([compat-uw7],
	AS_HELP_STRING([--enable-compat-uw7],
	    [enable source compatibility with UnixWare 7 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_uw7="$enableval"],
	    [enable_compat_uw7='module'])
    AC_ARG_ENABLE([compat-osf],
	AS_HELP_STRING([--enable-compat-osf],
	    [enable source compatibility with OSF/1.2 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_osf="$enableval"],
	    [enable_compat_osf='module'])
    AC_ARG_ENABLE([compat-aix],
	AS_HELP_STRING([--enable-compat-aix],
	    [enable source compatibility with AIX 4 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_aix="$enableval"],
	    [enable_compat_aix='module'])
    AC_ARG_ENABLE([compat-hpux],
	AS_HELP_STRING([--enable-compat-hpux],
	    [enable source compatibility with HPUX variants.
	    @<:@default=module@:>@]),
	    [enable_compat_hpux="$enableval"],
	    [enable_compat_hpux='module'])
    AC_ARG_ENABLE([compat-irix],
	AS_HELP_STRING([--enable-compat-irix],
	    [enable source compatibility with IRIX variants.
	    @<:@default=module@:>@]),
	    [enable_compat_irix="$enableval"],
	    [enable_compat_irix='module'])
    AC_ARG_ENABLE([compat-lis],
	AS_HELP_STRING([--enable-compat-lis],
	    [enable source compatibility with LiS variants.
	    @<:@default=module@:>@]),
	    [enable_compat_lis="$enableval"],
	    [enable_compat_lis='module'])
    AC_ARG_ENABLE([compat-lfs],
	AS_HELP_STRING([--enable-compat-lfs],
	    [enable source compatibility with LiS variants.
	    @<:@default=module@:>@]),
	    [enable_compat_lfs="$enableval"],
	    [enable_compat_lfs='module'])
    AC_ARG_ENABLE([compat-mac],
	AS_HELP_STRING([--enable-compat-mac],
	    [enable source compatibility with MacOT variants.
	    @<:@default=module@:>@]),
	    [enable_compat_mac="$enableval"],
	    [enable_compat_mac='module'])
    AC_CACHE_CHECK([for STREAMS OpenSS7 compatibility], [compat_compat_os7], [dnl
	compat_compat_os7="${enable_compat_os7:-module}"
	if test :$compat_compat_os7 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_os7='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS UNIX(R) SVR 3.2 compatibility], [compat_compat_svr3], [dnl
	compat_compat_svr3="${enable_compat_svr3:-module}"
	if test :$compat_compat_svr3 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_svr3='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS UNIX(R) SVR 4.2 compatibility], [compat_compat_svr4], [dnl
	compat_compat_svr4="${enable_compat_svr4:-module}"
	if test :$compat_compat_svr4 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_svr4='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS MPS(R) compatibility], [compat_compat_mps], [dnl
	compat_compat_mps="${enable_compat_mps:-module}"
	if test :$compat_compat_mps = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_mps='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS Solaris(R) 8 compatibility], [compat_compat_sol8], [dnl
	case ${enable_compat_sol8:-module} in
	    (yes) compat_compat_svr4=yes ;;
	    (module) if test :$compat_compat_svr4 != :yes ; then compat_compat_svr4=module ; fi ;;
	esac
	compat_compat_sol8="${enable_compat_sol8:-module}"
	if test :$compat_compat_sol8 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_sol8='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS UnixWare(R) 7 compatibility], [compat_compat_uw7], [dnl
	case ${enable_compat_uw7:-module} in
	    (yes) compat_compat_svr4=yes ;;
	    (module) if test :$compat_compat_svr4 != :yes ; then compat_compat_svr4=module ; fi ;;
	esac
	compat_compat_uw7="${enable_compat_uw7:-module}"
	if test :$compat_compat_uw7 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_uw7='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS OSF/1.2 compatibility], [compat_compat_osf], [dnl
	case ${enable_compat_osf:-module} in
	    (yes) compat_compat_svr4=yes ;;
	    (module) if test :$compat_compat_svr4 != :yes ; then compat_compat_svr4=module ; fi ;;
	esac
	case ${enable_compat_osf:-module} in
	    (yes) compat_compat_mps=yes ;;
	    (module) if test :$compat_compat_mps != :yes ; then compat_compat_mps=module ; fi ;;
	esac
	compat_compat_osf="${enable_compat_osf:-module}"
	if test :$compat_compat_osf = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_osf='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS AIX(R) 4 compatibility], [compat_compat_aix], [dnl
	case ${enable_compat_aix:-module} in
	    (yes) compat_compat_svr4=yes ;;
	    (module) if test :$compat_compat_svr4 != :yes ; then compat_compat_svr4=module ; fi ;;
	esac
	case ${enable_compat_aix:-module} in
	    (yes) compat_compat_mps=yes ;;
	    (module) if test :$compat_compat_mps != :yes ; then compat_compat_mps=module ; fi ;;
	esac
	compat_compat_aix="${enable_compat_aix:-module}"
	if test :$compat_compat_aix = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_aix='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS HPUX(R) compatibility], [compat_compat_hpux], [dnl
	case ${enable_compat_hpux:-module} in
	    (yes) compat_compat_svr4=yes ;;
	    (module) if test :$compat_compat_svr4 != :yes ; then compat_compat_svr4=module ; fi ;;
	esac
	case ${enable_compat_hpux:-module} in
	    (yes) compat_compat_mps=yes ;;
	    (module) if test :$compat_compat_mps != :yes ; then compat_compat_mps=module ; fi ;;
	esac
	compat_compat_hpux="${enable_compat_hpux:-module}"
	if test :$compat_compat_hpux = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_hpux='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS IRIX(R) compatibility], [compat_compat_irix], [dnl
	case ${enable_compat_irix:-module} in
	    (yes) compat_compat_svr4=yes ;;
	    (module) if test :$compat_compat_svr4 != :yes ; then compat_compat_svr4=module ; fi ;;
	esac
	compat_compat_irix="${enable_compat_irix:-module}"
	if test :$compat_compat_irix = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_irix='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS LiS compatibility], [compat_compat_lis], [dnl
	if test :$streams_cv_package = :LiS ; then
	    compat_compat_lis='not required'
	else
	    compat_compat_lis="${enable_compat_lis:-module}"
	    if test ":$compat_compat_lis" = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
		compat_compat_lis='yes'
	    fi
	fi])
    AC_CACHE_CHECK([for STREAMS LfS compatibility], [compat_compat_lfs], [dnl
	if test :$streams_cv_package = :LfS ; then
	    compat_compat_lfs='not required'
	else
	    compat_compat_lfs="${enable_compat_lfs:-module}"
	    if test ":$compat_compat_lfs" = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
		compat_compat_lfs='yes'
	    fi
	fi])
    AC_CACHE_CHECK([for STREAMS MacOT compatibility], [compat_compat_mac], [dnl
	case ${enable_compat_mac:-module} in
	    (yes) compat_compat_mps=yes ;;
	    (module) if test :$compat_compat_mps != :yes ; then compat_compat_mps=module ; fi ;;
	esac
	compat_compat_mac="${enable_compat_mac:-module}"
	if test :$compat_compat_mac = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    compat_compat_mac='yes'
	fi])
    case ${compat_compat_os7:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OS7], [], [When defined, Linux Fast STREAMS
	    will attempt to be as compatible as possible (without replicating any bugs) with the
	    OpenSS7 docs so that STREAMS drivers and modules written to OpenSS7 specs will compile
	    with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
	    OpenSS7 will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OS7_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the OpenSS7 docs so that STREAMS drivers and modules written to OpenSS7 specs will
	    compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written
	    for OpenSS7 will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_svr3:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR3], [], [When defined, Linux Fast STREAMS
	    will attempt to be as compatible as possible (without replicating any bugs) with the
	    UNIX(R) SVR 3.2 docs so that STREAMS drivers and modules written to UNIX(R) SVR 3.2
	    specs will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and
	    modules written for UNIX(R) SVR 3.2 will require porting in more respects.  This
	    symbol determines whether compatibility will be compiled and linkable with Linux
	    Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR3_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the UNIX(R) SVR 3.2 docs so that STREAMS drivers and modules written to UNIX(R) SVR
	    3.2 specs will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and
	    modules written for UNIX(R) SVR 3.2 will require porting in more respects.  This
	    symbol determines whether compatibility will be compiled as a loadable module to Linux
	    Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_svr4:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4], [], [When defined, Linux Fast STREAMS
	    will attempt to be as compatible as possible (without replicating any bugs) with the
	    UNIX(R) SVR 4.2 MP docs so that STREAMS drivers and modules written to UNIX(R) SVR 4.2
	    MP specs will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and
	    modules written for UNIX(R) SVR 4.2 MP will require porting in more respects.  This
	    symbol determines whether compatibility will be compiled and linkable with Linux
	    Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the UNIX(R) SVR 4.2 MP docs so that STREAMS drivers and modules written to UNIX(R) SVR
	    4.2 MP specs will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and
	    modules written for UNIX(R) SVR 4.2 MP will require porting in more respects.  This
	    symbol determines whether compatibility will be compiled as a loadable module to Linux
	    Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_mps:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MPS], [], [When defined, Linux Fast STREAMS
	    will attempt to be as compatible as possible (without replicating any bugs) with the
	    Mentat Portable STREAMS documentation so that STREAMS drivers and modules written for
	    Mentat Portable STREAMS will compile with Linux Fast STREAMS.  When undefined, STREAMS
	    drivers and modules written for Mentat Portable STREAMS will require porting in more
	    respects.  This symbol determines whether compatibility will be compiled and linkable
	    with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MPS_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the Mentat Portable STREAMS documentation so that STREAMS drivers and modules written
	    for Mentat Portable STREAMS will compile with Linux Fast STREAMS.  When undefined,
	    STREAMS drivers and modules written for Mentat Portable STREAMS will require porting in
	    more respects.  This symbol determines whether compatibility will be compiled as a
	    loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_sol8:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the Solaris(R) 8 release so that STREAMS drivers and modules written for Solaris(R) 8
	    will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
	    written for Solaris(R) 8 will require porting in more respects.  This symbol determines
	    whether compatibility will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the Solaris(R) 8 release so that STREAMS drivers and modules written for Solaris(R) 8
	    will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
	    written for Solaris(R) 8 will require porting in more respects.  This symbol determines
	    whether compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_uw7:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the UnixWare(R) 7 release so that STREAMS drivers and modules written for UnixWare(R) 7
	    will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
	    written for UnixWare(R) 7 will require porting in more respects.  This symbol determines
	    whether compatibility will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the UnixWare(R) 7 release so that STREAMS drivers and modules written for UnixWare(R) 7
	    will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
	    written for UnixWare(R) 7 will require porting in more respects.  This symbol determines
	    whether compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_osf:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the OSF(R)/1.2 release so that STREAMS drivers and modules written for OSF(R)/1.2 will
	    compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written
	    for OSF(R)/1.2 will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the OSF(R)/1.2 release so that STREAMS drivers and modules written for OSF(R)/1.2 will
	    compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written
	    for OSF(R)/1.2 will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_aix:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the AIX(R) 5L Version 5.1 release so that STREAMS drivers and modules written for AIX(R)
	    5L Version 5.1 will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers
	    and modules written for AIX(R) 5L Version 5.1 will require porting in more respects.
	    This symbol determines whether compatibility will be compiled and linkable with Linux
	    Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the AIX(R) 5L Version 5.1 release so that STREAMS drivers and modules written for AIX(R)
	    5L Version 5.1 will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers
	    and modules written for AIX(R) 5L Version 5.1 will require porting in more respects.
	    This symbol determines whether compatibility will be compiled as a loadable module to
	    Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_hpux:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the HPUX(R) release so that STREAMS drivers and modules written for HPUX(R) will compile
	    with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
	    HPUX(R) will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled and linkable with Linux Fast-STREAMS.  ])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the HPUX(R) release so that STREAMS drivers and modules written for HPUX(R) will compile
	    with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
	    HPUX(R) will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case ${compat_compat_irix:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_IRIX], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the IRIX(R) release so that STREAMS drivers and modules written for IRIX(R) will compile
	    with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
	    IRIX(R) will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled and linkable with Linux Fast-STREAMS.  ])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_IRIX_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the IRIX(R) release so that STREAMS drivers and modules written for IRIX(R) will compile
	    with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
	    IRIX(R) will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case "${compat_compat_lis:-module}" in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LIS], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the LiS release so that STREAMS drivers and modules written for LiS will compile with
	    Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for LiS will
	    require porting in more respects.  This symbol determines whether compatibility will be
	    compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LIS_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the LiS release so that STREAMS drivers and modules written for LiS will compile with
	    Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for LiS will
	    require porting in more respects.  This symbol determines whether compatibility will be
	    compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    case "${compat_compat_lfs:-module}" in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LFS], [], [When defined, Linux STREAMS
	    will attempt to be as compatible as possible (without replicating any bugs) with
	    the LfS release so that STREAMS drivers and modules written for LfS will compile with
	    Linux STREAMS.  When undefined, STREAMS drivers and modules written for LfS will
	    require porting in more respects.  This symbol determines whether compatibility will be
	    compiled and linkable with Linux STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LFS_MODULE], [], [When defined, Linux
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the LfS release so that STREAMS drivers and modules written for LfS will compile with
	    Linux STREAMS.  When undefined, STREAMS drivers and modules written for LfS will
	    require porting in more respects.  This symbol determines whether compatibility will be
	    compiled as a loadable module to Linux STREAMS.])
	    ;;
    esac
    case ${compat_compat_mac:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MAC], [], [When defined, Linux Fast STREAMS
	    will attempt to be as compatible as possible (without replicating any bugs) with the
	    MacOT(R) release so that STREAMS drivers and modules written for MacOT(R) will compile
	    with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
	    MacOT(R) will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MAC_MODULE], [], [When defined, Linux Fast
	    STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
	    the MacOT(R) release so that STREAMS drivers and modules written for MacOT(R) will
	    compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written
	    for MacOT(R) will require porting in more respects.  This symbol determines whether
	    compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OS7],		[test :${compat_compat_os7:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OS7_MODULE],	[test :${compat_compat_os7:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR3],	[test :${compat_compat_svr3:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR3_MODULE],	[test :${compat_compat_svr3:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR4],	[test :${compat_compat_svr4:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR4_MODULE],	[test :${compat_compat_svr4:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MPS],		[test :${compat_compat_mps:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MPS_MODULE],	[test :${compat_compat_mps:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SUN],		[test :${compat_compat_sol8:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SUN_MODULE],	[test :${compat_compat_sol8:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_UW7],		[test :${compat_compat_uw7:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_UW7_MODULE],	[test :${compat_compat_uw7:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OSF],		[test :${compat_compat_osf:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OSF_MODULE],	[test :${compat_compat_osf:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_AIX],		[test :${compat_compat_aix:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_AIX_MODULE],	[test :${compat_compat_aix:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_HPUX],	[test :${compat_compat_hpux:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [test :${compat_compat_hpux:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_IRIX],	[test :${compat_compat_irix:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_IRIX_MODULE], [test :${compat_compat_irix:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_LIS],		[test ":${compat_compat_lis:-module}"	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_LIS_MODULE],	[test ":${compat_compat_lis:-module}"	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_LFS],		[test ":${compat_compat_lfs:-module}"	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_LFS_MODULE],	[test ":${compat_compat_lfs:-module}"	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MAC],		[test :${compat_compat_mac:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MAC_MODULE],	[test :${compat_compat_mac:-module}	= :module])
])# _COMPAT_SETUP_COMPAT
# =============================================================================

# =============================================================================
# _COMPAT_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _COMPAT_CONFIG_KERNEL
    _GENKSYMS
    _COMPAT_CHECK_KERNEL
    _LINUX_STREAMS
    _COMPAT_SETUP_COMPAT
])# _COMPAT_SETUP
# =============================================================================

# =============================================================================
# _COMPAT_CHECK_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_CHECK_KERNEL], [dnl
    _COMPAT_CONFIG_KERNEL
    _COMPAT_CONFIG_FATTACH
    _COMPAT_CONFIG_LIS
    _COMPAT_CONFIG_LFS
])# _COMPAT_CHECK_KERNEL
# =============================================================================

# =============================================================================
# _COMPAT_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuraiton checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_CONFIG_KERNEL], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/slab.h linux/cdev.h \
			  linux/hardirq.h linux/cpumask.h linux/kref.h linux/security.h \
			  asm/uaccess.h linux/kthread.h linux/compat.h linux/ioctl32.h \
			  asm/ioctl32.h linux/syscalls.h linux/rwsem.h linux/smp_lock.h \
			  linux/devfs_fs_kernel.h linux/compile.h linux/utsrelease.h], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
])
    AC_SUBST([EXPOSED_SYMBOLS])dnl
    _LINUX_CHECK_FUNCS([try_module_get module_put to_kdev_t force_delete kern_umount iget_locked \
			process_group process_session cpu_raise_softirq check_region pcibios_init \
			pcibios_find_class pcibios_find_device pcibios_present \
			pcibios_read_config_byte pcibios_read_config_dword \
			pcibios_read_config_word pcibios_write_config_byte \
			pcibios_write_config_dword pcibios_write_config_word \
			pci_dac_dma_sync_single pci_dac_dma_sync_single_for_cpu \
			pci_dac_dma_sync_single_for_device pci_dac_set_dma_mask \
			pci_find_class pci_dma_sync_single pci_dma_sync_sg \
			pci_dac_page_to_dma pci_dac_dma_to_page \
			pci_dac_dma_to_offset vmalloc vfree \
			sleep_on interruptible_sleep_on sleep_on_timeout \
			read_trylock write_trylock atomic_add_return path_lookup \
			MOD_DEC_USE_COUNT MOD_INC_USE_COUNT cli sti], [:], [
			case "$lk_func" in
			    pcibios_*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				;;
			    pci_*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_osif_${lk_func}"
				;;
			    *sleep_on*|vmalloc|vfree)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				;;
			esac ], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
#ifdef HAVE_KINC_ASM_UACCESS_H
#include <asm/uaccess.h>
#endif
#ifdef HAVE_KINC_LINUX_COMPAT_H
#include <linux/compat.h>
#endif
])
    _LINUX_CHECK_MACROS([MOD_DEC_USE_COUNT MOD_INC_USE_COUNT \
			 read_trylock write_trylock], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
#ifdef HAVE_KINC_ASM_UACCESS_H
#include <asm/uaccess.h>
#endif
])
    _LINUX_CHECK_TYPES([paddr_t, irqreturn_t, irq_handler_t, bool, kmem_cache_t *], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/time.h>		/* for struct timespec */
])
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel irq_handler_t has 2 arguments], [linux_cv_type_irq_handler_t_2args], [dnl
	    if test :$linux_cv_type_irq_handler_t = :yes ; then
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/time.h>		/* for struct timespec */
]],
		    [[
irq_handler_t my_autoconf_function_pointer1 = NULL;
irqreturn_t (*my_autoconf_function_pointer2)(int, void *) = NULL;
my_autoconf_function_pointer1 = my_autoconf_function_pointer2;
]]) ],
		    [linux_cv_type_irq_handler_t_2args='yes'],
		    [linux_cv_type_irq_handler_t_2args='no'])
	    else
		linux_cv_type_irq_handler_t_2args='no'
	    fi
	])
	if test :$linux_cv_type_irq_handler_t_2args = :yes ; then
	    AC_DEFINE([HAVE_KTYPE_IRQ_HANDLER_2ARGS], [1], [Define if
		       'irq_handler_t' takes 2 arguments.])
	fi
    ])
    AH_TEMPLATE([kmem_cachep_t], [This kmem_cache_t is deprecated in recent
	2.6.20 kernels.  When it is deprecated, define this to struct
	kmem_cache *.])
    if test :"${linux_cv_type_kmem_cache_t_p:-no}" = :no ; then
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [struct kmem_cache *])
    else
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [kmem_cache_t *])
    fi
dnl 
dnl In later kernels, the super_block.u.geneic_sbp and the filesystem specific
dnl union u itself have been removed and a simple void pointer for filesystem
dnl specific information has been put in place instead.  We don't really care
dnl one way to the other, but this check discovers which way is used.
dnl 
    _LINUX_CHECK_MEMBERS([struct task_struct.namespace.sem,
			 struct task_struct.session,
			 struct task_struct.pgrp,
			 struct super_block.s_fs_info,
			 struct super_block.u.generic_sbp,
			 struct file_system_type.read_super,
			 struct file_system_type.get_sb,
			 struct super_operations.read_inode2,
			 struct kstatfs.f_type], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
])
	_LINUX_KERNEL_EXPORT_ONLY([path_lookup])
	_LINUX_KERNEL_EXPORT_ONLY([raise_softirq])
	_LINUX_KERNEL_EXPORT_ONLY([raise_softirq_irqoff])
	_LINUX_KERNEL_SYMBOL_EXPORT([put_filp])
	_LINUX_KERNEL_ENV([dnl
	    AC_CACHE_CHECK([for kernel inode_operation lookup with nameidata],
			   [linux_cv_have_iop_lookup_nameidata], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif]],
			[[struct inode_operations temp;
(*temp.lookup)((struct inode *)0, (struct dentry *)0, (struct nameidata *)0);]]) ],
		    [linux_cv_have_iop_lookup_nameidata='yes'],
		    [linux_cv_have_iop_lookup_nameidata='no'])
	    ])
	    if test :$linux_cv_have_iop_lookup_nameidata = :yes ; then
		AC_DEFINE([HAVE_INODE_OPERATIONS_LOOKUP_NAMEIDATA], [1],
		    [Set if inode_operation lookup function takes nameidata pointer.])
	    fi
	    AC_CACHE_CHECK([for kernel do_settimeofday with timespec],
			   [linux_cv_have_timespec_settimeofday], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#include <linux/time.h>		/* for struct timespec */]],
			[[struct timespec ts;
int retval;
retval = do_settimeofday(&ts);]]) ],
		[linux_cv_have_timespec_settimeofday='yes'],
		[linux_cv_have_timespec_settimeofday='no'])
	    ])
	    if test :$linux_cv_have_timespec_settimeofday = :yes ; then
		AC_DEFINE([HAVE_TIMESPEC_DO_SETTIMEOFDAY], [1],
		    [Define if do_settimeofday takes struct timespec and returns int.])
	    fi
	])
])# _COMPAT_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _COMPAT_CONFIG_FATTACH
# -----------------------------------------------------------------------------
# 
# symbols to implement fattach
# -----------------------------------------------------------------------------
# path_init             <--- exported
# path_walk             <--- exported
# path_release          <--- exported
# getname               <--- exported
# d_mountpoint          <--- inline, declared in <linux/dcache.h>
# mntput                <--- inline, declared in <linux/mount.h>
# clone_mnt             <=== static, can be ripped
# check_mnt             <=== static, can be ripped
# graft_tree            <=== static, can be ripped
# 
# symbols to implement fdetach
# -----------------------------------------------------------------------------
# path_init             <--- exported
# path_release          <--- exported
# check_mnt             <=== static, can be ripped
#
# symbols to override system fifos
# -----------------------------------------------------------------------------
# def_fifo_fops         <-- extern, declared in <linux/fs.h>
# 
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_CONFIG_FATTACH], [dnl
    lfs_pipe=yes
    lfs_fattach=yes
    _LINUX_KERNEL_SYMBOLS([namespace_sem, mount_sem, check_mnt])
    _LINUX_KERNEL_SYMBOL_EXPORT([clone_mnt], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([graft_tree], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([do_umount], [lfs_fattach=no; lfs_pipe=no])
    AC_CACHE_CHECK([for kernel symbol support for fattach/fdetach], [lfs_cv_fattach], [dnl
	lfs_cv_fattach="$lfs_fattach" ])
    if test :"${lfs_cv_fattach:-no}" != :no ; then
	AC_DEFINE([HAVE_KERNEL_FATTACH_SUPPORT], [1],
	[If the addresses for the necessary symbols above are defined, then
	define this to include fattach/fdetach support.])
    fi
    AC_CACHE_CHECK([for kernel symbol support for pipe], [lfs_cv_pipe], [dnl
	lfs_cv_pipe="$lfs_pipe" ])
    if test :${lfs_cv_pipe:-no} != :no ; then
	AC_DEFINE([HAVE_KERNEL_PIPE_SUPPORT], [1],
	[If the addresses for the necessary symbols above are defined, then
	define this to include pipe support.])
    fi
    _LINUX_KERNEL_SYMBOL_EXPORT([def_fifo_fops])
])# _COMPAT_CONFIG_FATTACH
# =============================================================================

# =============================================================================
_COMPAT_CONFIG_LIS
# -----------------------------------------------------------------------------
# symbols to rip for STREAMS support (without system call generation)
# -----------------------------------------------------------------------------
# sys_unlink            <-- extern, not declared
# sys_mknod             <-- extern, not declared
# sys_umount            <-- extern, not declared
# sys_mount             <-- extern, not declared
# pcibios_init          <-- extern, declared in <linux/pci.h>
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_CONFIG_LIS], [dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_unlink], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_unlink() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_mknod], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_mknod() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_umount], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_umount() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_mount], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_mount() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([pcibios_init])
])# _COMPAT_CONFIG_LIS
# =============================================================================

# =============================================================================
# _COMPAT_CONFIG_LFS
# -----------------------------------------------------------------------------
# symbols to rip for Linux Fast STREAMS
# -----------------------------------------------------------------------------
# file_move             <-- extern, declared in <linux/fs.h>
# open_softirq          <-- extern, declared in <linux/interrupt.h>
# sock_readv_writev     <-- extern, declared in <linux/net.h>
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_CONFIG_LFS], [dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([file_move])
    _LINUX_KERNEL_SYMBOL_EXPORT([open_softirq], [dnl
	AC_MSG_ERROR([
*** 
*** Compiling Linux Fast STREAMS requires the availability of the kernel
*** symbol open_softirq so that the necessary software IRQs can be
*** established.  This symbol is not normally exported from the kernel.
*** I have looked in "${linux_cv_k_sysmap:-(no sysmap)}" for the symbol
*** and cannot find it.  This problem needs to be corrected or workaround
*** found before you can compile Linux Fast STREAMS for the system for
*** which it is being configured.
*** ]) ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sock_readv_writev])
    _LINUX_KERNEL_SYMBOL_EXPORT([__wake_up_sync])
])# _COMPAT_CONFIG_LFS
# =============================================================================

# =============================================================================
# _COMPAT_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_OUTPUT], [dnl
    _COMPAT_CONFIG
    _COMPAT_STRCONF
])# _COMPAT_OUTPUT
# =============================================================================

# =============================================================================
# _COMPAT_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    strcomp_cv_config="${pkg_bld}/src/include/sys/strcompat/config.h"
    strcomp_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    strcomp_cv_ldadd= # "${pkg_bld}/libcompat.la"
    strcomp_cv_ldflags= # "${pkg_bld}/lib32/libcompat.la"
    strcomp_cv_ldadd32= # "-L${pkg_bld}/.libs/"
    strcomp_cv_ldflags32= # "${pkg_bld}/lib32/.libs/"
    strcomp_cv_manpath="${pkg_bld}/doc/man"
    strcomp_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    strcomp_cv_modmap="${pkg_bld}/Modules.map"
    strcomp_cv_symver="${pkg_bld}/Module.symvers"
    strcomp_cv_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
])# _COMPAT_CONFIG
# =============================================================================

# =============================================================================
# _COMPAT_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=242
    strconf_cv_midbase=10
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
])# _COMPAT_STRCONF
# =============================================================================

# =============================================================================
# _COMPAT_
# -----------------------------------------------------------------------------
AC_DEFUN([_COMPAT_], [dnl
])# _COMPAT_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
