# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: strcomp.m4,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/07/14 03:57:42 $
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
# Last Modified $Date: 2005/07/14 03:57:42 $ by $Author: brian $
#
# =============================================================================


# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS compatiblityu release and necessary
# include directories and other configuration for compiling kernel modules to
# run with the STREAMS compatibility package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strcompat loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _STRCOMP
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP], [dnl
    _STRCOMP_OPTIONS
    _STRCOMP_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_STRCOMP_KERNEL])
    _STRCOMP_OUTPUT
    AC_SUBST([STRCOMP_CPPFLAGS])
    AC_SUBST([STRCOMP_MODFLAGS])
    AC_SUBST([STRCOMP_LDADD])
    AC_SUBST([STRCOMP_MODMAP])
    AC_SUBST([STRCOMP_SYMVER])
    AC_SUBST([STRCOMP_MANPATH])
    AC_SUBST([STRCOMP_VERSION])
])# _STRCOMP
# =============================================================================

# =============================================================================
# _STRCOMP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_OPTIONS], [dnl
    AC_ARG_WITH([compat],
	AS_HELP_STRING([--with-compat=HEADERS],
	    [specify the STREAMS compatibility header file directory.  @<:@default=INCLUDEDIR/strcompat@:>@]),
	[with_compat="$withval"],
	[with_compat=''])
])# _STRCOMP_OPTIONS
# =============================================================================

# =============================================================================
# _STRCOMP_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_SETUP], [dnl
    _STRCOMP_CHECK_HEADERS
    for strcomp_include in $strcomp_cv_includes ; do
	STRCOMP_CPPFLAGS="${STRCOMP_CPPFLAGS}${STRCOMP_CPPFLAGS:+ }-I${strcomp_include}"
    done
    if test :"${strcomp_cv_config:-no}" != :no ; then
	STRCOMP_CPPFLAGS="${STRCOMP_CPPFLAGS}${STRCOMP_CPPFLAGS:+ }-include ${strcomp_cv_config}"
    fi
    if test :"${strcomp_cv_modversions:-no}" != :no ; then
	STRCOMP_MODFLAGS="${STRCOMP_MODFLAGS}${STRCOMP_MODFLAGS:+ }-include ${strcomp_cv_modversions}"
    fi
])# _STRCOMP_SETUP
# =============================================================================

# =============================================================================
# _STRCOMP_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS Compatibility header files.  The
    # package normally requires compatibility header files to compile.
    AC_CACHE_CHECK([for compat include directory], [strcomp_cv_includes], [dnl
	if test :"${with_compat:-no}" != :no -a :"${with_compat:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    strcomp_cv_includes="$with_compat"
	fi
	strcomp_what="sys/os7/compat.h"
	if test ":${strcomp_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    strcomp_here=`pwd`
	    for strcomp_dir in \
		$srcdir/strcompat*/include \
		$srcdir/../strcompat*/src/include \
		../_build/$srcdir/../../strcompat*/src/include \
		../_build/$srcdir/../../../strcompat*/src/include
	    do
		if test -d $strcomp_dir -a -r $strcomp_dir/$strcomp_what ; then
		    strcomp_bld=`echo $strcomp_dir | sed -e "s|^$srcdir/|$strcomp_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    strcomp_dir=`(cd $strcomp_dir; pwd)`
		    strcomp_cv_includes="$strcomp_dir $strcomp_bld"
		    strcomp_cv_ldadd=
		    strcomp_cv_modmap=`echo "$strcomp_bld/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    strcomp_cv_symver=`echo "$strcomp_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    strcomp_cv_manpath=`echo "$strcomp_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    break
		fi
	    done
	fi
	if test ":${strcomp_cv_includes:-no}" = :no ; then
	    # Compat header files are normally found in the strcompat package now.
	    # They used to be part of the compatibility add-on package and even older
	    # versions are part of the LfS release packages.
	    eval "strcomp_search_path=\"
		${DESTDIR}${includedir}/strcompat
		${DESTDIR}${rootdir}${oldincludedir}/strcompat
		${DESTDIR}${rootdir}/usr/include/strcompat
		${DESTDIR}${rootdir}/usr/local/include/strcompat
		${DESTDIR}${rootdir}/usr/src/strcompat/src/include
		${DESTDIR}${includedir}/streams
		${DESTDIR}${rootdir}${oldincludedir}/streams
		${DESTDIR}${rootdir}/usr/include/streams
		${DESTDIR}${rootdir}/usr/local/include/streams
		${DESTDIR}${rootdir}/usr/src/streams/include
		${DESTDIR}${oldincludedir}/strcompat
		${DESTDIR}/usr/include/strcompat
		${DESTDIR}/usr/local/include/strcompat
		${DESTDIR}/usr/src/strcompat/src/include
		${DESTDIR}${oldincludedir}/streams
		${DESTDIR}/usr/include/streams
		${DESTDIR}/usr/local/include/streams
		${DESTDIR}/usr/src/streams/include\""
	    case "$strcomp_cv_package" in
		LiS)
		    # Compatibility header files could sometime be part of the LiS package
		    eval "strcomp_search_path=\"$strcomp_search_path
			${DESTDIR}${includedir}/LiS
			${DESTDIR}${rootdir}${oldincludedir}/LiS
			${DESTDIR}${rootdir}/usr/include/LiS
			${DESTDIR}${rootdir}/usr/local/include/LiS
			${DESTDIR}${rootdir}/usr/src/LiS/include
			${DESTDIR}${oldincludedir}/LiS
			${DESTDIR}/usr/include/LiS
			${DESTDIR}/usr/local/include/LiS
			${DESTDIR}/usr/src/LiS/include\""
		    ;;
		LfS)
		    # Compatibility header files used to be part of the LfS package
		    eval "strcomp_search_path=\"$strcomp_search_path
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    strcomp_search_path=`echo "$strcomp_search_path" | sed -e 's|\<NONE\>||g:s|//|/|g'`
	    for strcomp_dir in $strcomp_search_path ; do
		if test -d "$strcomp_dir" -a -r "$strcomp_dir/$strcomp_what" ; then
		    strcomp_cv_includes="$strcomp_dir"
		    strcomp_cv_ldadd=
		    strcomp_cv_modmap=
		    strcomp_cv_symver=
		    strcomp_cv_manpath=
		    break
		fi
	    done
	fi
    ])
    if test :"${strcomp_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS compat include directories.  If you
*** wish to use the STREAMS compat package you will need to specify the location
*** fo the STREAMS compat (strcompat) include directories with the --with-compat
*** option to configure and try again.  Perhaps you just forgot to load the
*** STREAMS compat package?  The STREAMS strcompat package is available from the
*** download page at http://www.openss7.org/ and comes in a tarball named
*** something like "strcompat-0.9.2.1.tar.gz".
*** ])
    fi
    strcomp_what="sys/config.h"
    AC_CACHE_CHECK([for streams lfs $strcomp_what], [strcomp_cv_config], [dnl
	strcomp_cv_config=no
	if test -n "$strcomp_cv_includes" ; then
	    for strcomp_dir in $strcomp_cv_includes ; do
		# old place for config
		if test -f "$strcomp_dir/$strcomp_what" ; then
		    strcomp_cv_config="$strcomp_dir/$strcomp_what"
		    break
		fi
		# new place for config
		if test -n $linux_cv_k_release ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" ; then
			strcomp_cv_config="$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" 
			break
		    fi
		fi
	    done
	fi
    ])
    AC_CACHE_CHECK([for strcompat version], [strcomp_cv_version], [dnl
	strcomp_what="sys/strcompat/version.h"
	strcomp_file=
	if test -n "$strcomp_cv_includes" ; then
	    for strcomp_dir in $strcomp_cv_includes ; do
		# old place for version
		if test -f "$strcomp_dir/$strcomp_what" ; then
		    strcomp_file="$strcomp_dir/$strcomp_what"
		    break
		fi
		# new place for version
		if test -n $linux_cv_k_release ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" ; then
			strcomp_file="$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" 
			break
		    fi
		fi
	    done
	fi
	if test :${strcomp_file:-no} != :no ; then
	    strcomp_cv_version=`grep '#define.*\<STRCOMPAT_VERSION\>' $strcomp_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    strcomp_what="sys/strcompat/modversions.h"
    AC_CACHE_CHECK([for strcompat $strcomp_what], [strcomp_cv_modversions], [dnl
	strcomp_cv_modversions='no'
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$strcomp_cv_includes" ; then
		for strcomp_dir in $strcomp_cv_includes ; do
		    # old place for modversions
		    if test -f "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_modversions="$strcomp_dir/$strcomp_what"
			break
		    fi
		    # new place for modversions
		    if test -n $linux_cv_k_release ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
			if test -f "$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" ; then
			    strcomp_cv_includes="$strcomp_dir/$linux_cv_k_release/$target_cpu $strcomp_cv_includes"
			    strcomp_cv_modversions="$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${strcomp_cv_includes:-no}" = :no ; then :
	if test :"$with_compat" = :no ; then
	    AC_MSG_ERROR([
***
*** Could not find strcompat include directories.  This package requires the
*** presence of strcompat include directories to compile.  Specify the location of
*** strcompat include directories with option --with-compat to configure and try again.
*** ])
	fi
	if test -z "$with_compat" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_compat --with-compat\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-compat'"
	fi
    else
	if test -z "$with_compat" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_compat --without-compat\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-compat'"
	fi
    fi
])# _STRCOMP_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _STRCOMP_KERNEL
# -----------------------------------------------------------------------------
# Need to know about irqreturn_t for sys/os7/compat.h STREAMS compatibility file.
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_KERNEL], [dnl
    _LINUX_CHECK_TYPES([irqreturn_t], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>	/* for irqreturn_t */ 
    ])
])# _STRCOMP_KERNEL
# =============================================================================

# =============================================================================
# _STRCOMP_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_OUTPUT], [dnl
    _STRCOMP_DEFINES
])# _STRCOMP_OUTPUT
# =============================================================================

# =============================================================================
# _STRCOMP_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_DEFINES], [dnl
    if test :"${strcomp_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_STRCOMP_MODVERSIONS_H], [1], [Define when
	    the STREAMS compatibiltiy release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    STRCOMP_CPPFLAGS="${STRCOMP_CPPFLAGS:+ ${STRCOMP_CPPFLAGS}}"
    STRCOMP_LDADD="$strcomp_cv_ldadd"
    STRCOMP_MODMAP="$strcomp_cv_modmap"
    STRCOMP_SYMVER="$strcomp_cv_symver"
    STRCOMP_MANPATH="$strcomp_cv_manpath"
    STRCOMP_VERSION="$strcomp_cv_version"
    MODPOST_INPUTS="${MODPOST_INPUTS}${STRCOMP_SYMVER:+${MODPOST_INPUTS:+ }${STRCOMP_SYMVER}}"
])# _STRCOMP_DEFINES
# =============================================================================

# =============================================================================
# _STRCOMP_
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_], [dnl
])# _STRCOMP_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
