# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/02/19 11:49:58 $
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
# Last Modified $Date: 2005/02/19 11:49:58 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _DEB_DPKG_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_OPTIONS], [dnl
])# _DEB_DPKG_OPTIONS
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP], [dnl
    _DEB_DPKG_SETUP_ARCH
    _DEB_DPKG_SETUP_INDEP
    _DEB_DPKG_SETUP_TOPDIR
    _DEB_DPKG_SETUP_OPTIONS
    _DEB_DPKG_SETUP_BUILD
])# _DEB_DPKG_SETUP
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_ARCH
# -----------------------------------------------------------------------------
# Debian can build architecture dependent or architecture independent packages.
# This option specifies whether architecture dependent packages are to be built
# and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_ARCH], [dnl
    AC_MSG_CHECKING([for deb build/install of arch packages])
    AC_ARG_ENABLE([arch],
	AS_HELP_STRING([--enable-arch],
	    [build and install arch packages.  @<:@default=yes@:>@]),
	[enable_arch="$enableval"],
	[enable_arch='yes'])
    AC_MSG_RESULT([${enable_arch:-yes}])
    AM_CONDITIONAL([DEB_BUILD_ARCH], [test :"${enable_arch:-yes}" = :yes])dnl
	
])# _DEB_DPKG_SETUP_ARCH
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_INDEP
# -----------------------------------------------------------------------------
# Debian can build architecture dependent or architecture independent packages.
# This option specifies whether architecture independent packages are to be
# built and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_INDEP], [dnl
    AC_MSG_CHECKING([for deb build/install of indep packages])
    AC_ARG_ENABLE([indep],
	AS_HELP_STRING([--enable-indep],
	    [build and install indep packages.  @<:@default=yes@:>@]),
	[enable_indep="$enableval"],
	[enable_indep='yes'])
    AC_MSG_RESULT([${enable_indep:-yes}])
    AM_CONDITIONAL([DEB_BUILD_INDEP], [test :"${enable_indep:-yes}" = :yes])dnl
])# _DEB_DPKG_SETUP_INDEP
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_TOPDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_TOPDIR], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    deb_tmp='$(PACKAGE_DISTDIR)/debian'
    AC_ARG_WITH([deb-topdir],
	AS_HELP_STRING([--with-deb-topdir=DIR],
	    [specify the deb top directory.  @<:@default=PKG-DISTDIR/debian@:>@]),
	[with_deb_topdir="$withval"],
	[with_deb_topdir="$deb_tmp"])
    AC_CACHE_CHECK([for deb top build directory], [deb_cv_topdir], [dnl
	case :"$with_deb_topdir" in
	    :no | :NO)
		deb_cv_topdir="$deb_tmp"
		;;
	    :yes | :YES | :default | :DEFAULT)
		deb_cv_topdir="/usr/src/debian"
		;;
	    *)
		deb_cv_topdir="$with_deb_topdir"
		;;
	esac
    ])
    PACKAGE_DEBTOPDIR="$deb_cv_topdir"
    AC_SUBST([PACKAGE_DEBTOPDIR])dnl
])# _DEB_DPKG_SETUP_TOPDIR
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_OPTIONS], [dnl
    PACKAGE_DEBOPTIONS=
    arg=
    for arg_part in $ac_configure_args ; do
	if (echo "$arg_part" | grep "^'" >/dev/null 2>&1) ; then
	    if test -n "$arg" ; then
		AC_MSG_CHECKING([for deb argument $arg])
		if (echo $arg | egrep '^'"'"'(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
		    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }$arg"
		    AC_MSG_RESULT([yes])
		else
		    :
		    AC_MSG_RESULT([no])
		fi
	    fi
	    arg="$arg_part"
	else
	    arg="${arg}${arg:+ }${arg_part}"
	fi
    done
    if test -n "$arg" ; then
	AC_MSG_CHECKING([for rpm argument $arg])
	if (echo $arg | egrep '^'"'"'(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }$arg"
	    AC_MSG_RESULT([yes])
	else
	    :
	    AC_MSG_RESULT([no])
	fi
    fi
    AC_SUBST([PACKAGE_DEBOPTIONS])dnl
    AC_SUBST([ac_configure_args])dnl
])# _DEB_DPKG_SETUP_OPTIONS
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_BUILD
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_BUILD], [dnl
    AC_ARG_VAR([DPKG], [dpkg command])
    AC_PATH_TOOL([DPKG], [dpkg], [], [$PATH:/usr/local/bin:/usr/bin])
    if test :"${DPKG:-no}" = :no ; then
	AC_MSG_WARN([Could not find dpkg program in PATH.])
    fi
    AC_ARG_VAR([DPKG_SOURCE], [dpkg-source command])
    AC_PATH_TOOL([DPKG_SOURCE], [dpkg-source], [], [$PATH:/usr/local/bin:/usr/bin])
    if test :"${DPKG_SOURCE:-no}" = :no ; then
	AC_MSG_WARN([Could not find dpkg-source program in PATH.])
    fi
    AC_ARG_VAR([DPKG_BUILDPACKAGE], [dpkg-buildpackage command])
    AC_PATH_TOOL([DPKG_BUILDPACKAGE], [dpkg-buildpackage], [], [$PATH:/usr/local/bin:/usr/bin])
    if test :"${DPKG_BUILDPACKAGE:-no}" = :no ; then
	AC_MSG_WARN([Could not find dpkg-buildpackage program in PATH.])
    fi
    AM_CONDITIONAL([BUILD_DPKG], [test :"${DPKG_SOURCE:-no}" != :no -a :"${DPKG_BUILDPACKAGE:-no}" != :no])dnl
])# _DEB_DPKG_SETUP_BUILD
# =============================================================================

# =============================================================================
# _DEB_DPKG_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_OUTPUT], [dnl
    if test :"${DPKG_SOURCE:-no}" != :no -a :"${DPKG_BUILDPACKAGE:-no}" != :no ; then
	AC_CONFIG_FILES([debian/rules
			 debian/control])
    fi
])# _DEB_DPKG_OUTPUT
# =============================================================================

# =============================================================================
# _DEB_
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_], [dnl
])# _DEB_
# =============================================================================

# =============================================================================
# _DEB_DPKG
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG], [dnl
    AC_REQUIRE([_DISTRO])
    _DEB_DPKG_OPTIONS
    _DEB_DPKG_SETUP
    _DEB_DPKG_OUTPUT
])# _DEB_DPKG
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
