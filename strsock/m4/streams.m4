# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: streams.m4,v $ $Name:  $($Revision: 0.9.2.77 $) $Date: 2007/02/12 10:39:49 $
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
# Last Modified $Date: 2007/02/12 10:39:49 $ by $Author: brian $
#
# =============================================================================


# -----------------------------------------------------------------------------
# This file provides some common macros for finding an LiS release and
# necessary include directories and other configuration for compiling kernel
# modules to run with LiS.  Eventually, this file will determine whether
# GCOM's LiS is being used or whether OpenSS7's LfS (Linux Fast STREAMS) is
# being used.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have LiS or LfS loaded on the build
# machine to compile modules.  Only the proper header files are required.  It
# is, not a good idea to install both sets of kernel modules for the same
# kernel.  For an rpm build, whether kernel modules are being built for LiS or
# LfS is determined using the the with_lis or with_lfs flags to the build.  If
# both are specified, it defaults to LfS.
# -----------------------------------------------------------------------------

# =============================================================================
# _LINUX_STREAMS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS], [dnl
    _LINUX_STREAMS_OPTIONS
    _LINUX_STREAMS_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_LINUX_STREAMS_KERNEL])
    _LINUX_STREAMS_OUTPUT
    AC_SUBST([STREAMS_CPPFLAGS])dnl
    AC_SUBST([STREAMS_MODFLAGS])dnl
    AC_SUBST([STREAMS_LDADD])dnl
    AC_SUBST([STREAMS_LDADD32])dnl
    AC_SUBST([STREAMS_LDFLAGS])dnl
    AC_SUBST([STREAMS_LDFLAGS32])dnl
    AC_SUBST([STREAMS_MODMAP])dnl
    AC_SUBST([STREAMS_SYMVER])dnl
    AC_SUBST([STREAMS_MANPATH])dnl
    AC_SUBST([STREAMS_VERSION])dnl
])# _LINUX_STREAMS
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_OPTIONS], [dnl
    AC_ARG_WITH([lis],
	AS_HELP_STRING([--with-lis=HEADERS],
	    [specify the LiS header file directory.  @<:@default=INCLUDEDIR/LiS@:>@]),
	[with_lis="$withval" ; for s in ${!streams_cv_*} ; do eval "unset $s" ; done],
	[with_lis=''])
    AC_ARG_WITH([lfs],
	AS_HELP_STRING([--with-lfs=HEADERS],
	    [specify the LfS header file directory.  @<:@default=INCLUDEDIR/LfS@:>@]),
	[with_lfs="$withval" ; for s in ${!streams_cv_*} ; do eval "unset $s" ; done],
	[with_lfs=''])
])# _LINUX_STREAMS_OPTIONS
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_SETUP], [dnl
    if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
	_LINUX_STREAMS_LIS_CHECK_HEADERS
    fi
    if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
	_LINUX_STREAMS_LFS_CHECK_HEADERS
    fi
    AC_CACHE_CHECK([for streams include directory], [streams_cv_includes], [dnl
	if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
	    if test :"${streams_cv_lis_includes:-no}" != :no ; then
		streams_cv_includes="$streams_cv_lis_includes"
	    fi
	fi
	if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
	    if test :"${streams_cv_lfs_includes:-no}" != :no ; then
		streams_cv_includes="$streams_cv_lfs_includes"
	    fi
	fi
    ])
    AC_CACHE_CHECK([for streams include config file], [streams_cv_config], [dnl
	if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
	    if test :"${streams_cv_lis_includes:-no}" != :no ; then
		streams_cv_config="$streams_cv_lis_config"
	    fi
	fi
	if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
	    if test :"${streams_cv_lfs_includes:-no}" != :no ; then
		streams_cv_config="$streams_cv_lfs_config"
	    fi
	fi
    ])
    AC_CACHE_CHECK([for streams include modversions file], [streams_cv_modversions], [dnl
	if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
	    if test :"${streams_cv_lis_includes:-no}" != :no ; then
		streams_cv_modversions="$streams_cv_lis_modversions"
	    fi
	fi
	if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
	    if test :"${streams_cv_lfs_includes:-no}" != :no ; then
		streams_cv_modversions="$streams_cv_lfs_modversions"
	    fi
	fi
    ])
    AC_CACHE_CHECK([for streams package], [streams_cv_package], [dnl
	if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
	    if test :"${streams_cv_lis_includes:-no}" != :no ; then
		streams_cv_package="LiS"
	    fi
	fi
	if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
	    if test :"${streams_cv_lfs_includes:-no}" != :no ; then
		streams_cv_package="LfS"
	    fi
	fi
    ])
    AC_CACHE_CHECK([for streams version], [streams_cv_version], [dnl
	if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
	    if test :"${streams_cv_lis_includes:-no}" != :no ; then
		streams_cv_version="$streams_cv_lis_version"
	    fi
	fi
	if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
	    if test :"${streams_cv_lfs_includes:-no}" != :no ; then
		streams_cv_version="$streams_cv_lfs_version"
	    fi
	fi
    ])
    # need to add arguments for LiS or LfS so they will be passed to rpm
    AC_MSG_CHECKING([for streams added configure arguments])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly,
dnl so we use defines.
    case "$streams_cv_package" in
	LiS)
	    if test -z "$with_lis" 
	    then :;
dnl             PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_lis --with-lis\""
dnl		PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-lis'"
dnl             ac_configure_args="$ac_configure_args --with-lis"
	    fi
	    AC_MSG_RESULT([--with-lis])
	    ;;
	LfS)
	    if test -z "$with_lfs" 
	    then :;
dnl             PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_lfs --with-lfs\""
dnl		PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-lfs'"
dnl             ac_configure_args="$ac_configure_args --with-lfs"
	    fi
	    AC_MSG_RESULT([--with-lfs])
	    ;;
	*)
	    if test :"${with_lis:-no}" != :no ; then
		AC_MSG_ERROR([
*** 
*** Linux GCOM STREAMS was specified with the --with-lis flag, however,
*** configure could not find the LiS include directories.  This package
*** requires the presense of LiS include directories when the --with-lis
*** flag is specified.  Specify the correct location of LiS include
*** directories with the argument to option --with-lis to configure and
*** try again.
*** ])
	    fi
	    if test :"${with_lfs:-no}" != :no ; then
		AC_MSG_ERROR([
*** 
*** Linux Fast STREAMS was specified with the --with-lfs flag, however,
*** configure could not find the LfS include directories.  This package
*** requires the presense of LfS include directories when the --with-lfs
*** flag is specified.  Specify the correct location of LfS include
*** directories with the argument to option --with-lfs to configure and
*** try again.
*** ])
	    fi
	    AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS include directories.  This
*** package requires the presence of STREAMS include directories.
*** Specify the correct location of Linux GCOM STREAMS (LiS) include
*** directories with the --with-lis option to configure, or the correct
*** location of Linux Fast STREAMS (LfS) include directories with the
*** --with-lfs option to configure, and try again.
***
*** Perhaps you just forgot to load the LfS STREAMS package?  The LfS
*** STREAMS package is available from The OpenSS7 Project download page
*** at http://www.openss7.org/ and comes in a tarball named something
*** like "streams-0.9.2.1-1.tar.gz".
*** ])
	    ;;
    esac
    for streams_include in $streams_cv_includes ; do
	STREAMS_CPPFLAGS="${STREAMS_CPPFLAGS}${STREAMS_CPPFLAGS:+ }-I${streams_include}"
    done
    if test :"${streams_cv_config:-no}" != :no ; then
	STREAMS_CPPFLAGS="${STREAMS_CPPFLAGS}${STREAMS_CPPFLAGS:+ }-include ${streams_cv_config}"
    fi
    if test :"${streams_cv_modversions:-no}" != :no ; then
	STREAMS_MODFLAGS="${STREAMS_MODFLAGS}${STREAMS_MODFLAGS:+ }-include ${streams_cv_modversions}"
    fi
    AM_CONDITIONAL([WITH_LIS], [test :"${streams_cv_lis_includes:-no}" != :no])
    AM_CONDITIONAL([WITH_LFS], [test :"${streams_cv_lfs_includes:-no}" != :no])
])# _LINUX_STREAMS_SETUP
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_LIS_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LIS_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS header files.  The package normally requires either
    # Linux STREAMS or Linux Fast-STREAMS header files (or both) to compile.
    AC_CACHE_CHECK([for streams lis include directory], [streams_cv_lis_includes], [dnl
	streams_what="sys/stream.h"
	if test :"${with_lis:-no}" != :no -a :"${with_lis:-no}" != :yes ; then
	    AC_MSG_RESULT([(searching $with_lis)])
	    for streams_dir in $with_lis ; do
		if test -d "$streams_dir" ; then
		    AC_MSG_CHECKING([for streams lis include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lis_includes="$with_lis"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${streams_cv_lis_includes:-no}" = :no ; then
		AC_MSG_WARN([
***
*** You have specified include directories using:
***
***	    --with-lis="$with_lis"
***
*** however, $streams_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	fi
	if test :"${streams_cv_lis_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    streams_search_path="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/LiS/include} ${os7_cv_master_builddir:+$os7_cv_master_builddir/LiS/include}"
	    for streams_dir in $streams_search_path ; do
		if test -d "$streams_dir" ; then
		    AC_MSG_CHECKING([for streams lis include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lis_includes="$streams_search_path"
			streams_cv_lis_ldadd="$os7_cv_master_builddir/LiS/libLiS.la"
			streams_cv_lis_ldadd32="$os7_cv_master_builddir/LiS/lib32/libLiS.la"
			streams_cv_lis_modmap="$os7_cv_master_builddir/LiS/Modules.map"
			streams_cv_lis_symver="$os7_cv_master_builddir/LiS/Module.symvers"
			streams_cv_lis_manpath="$os7_cv_master_builddir/LiS/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		fi
	    done
	    AC_MSG_CHECKING([for streams lis include directory])
	fi
	if test :"${streams_cv_lis_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    streams_here=`pwd`
	    AC_MSG_RESULT([(searching from $streams_here)])
	    for streams_dir in \
		$srcdir/LiS*/include $srcdir/lis*/include \
		$srcdir/../LiS*/include $srcdir/../lis*/include \
		../_build/$srcdir/../../LiS*/include ../_build/$srcdir/../../lis*/include \
		../_build/$srcdir/../../../LiS*/include ../_build/$srcdir/../../../lis*/include
	    do
		if test -d "$streams_dir" ; then
		    streams_bld=`echo $streams_dir | sed -e "s|^$srcdir/|$streams_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    streams_dir=`(cd $streams_dir; pwd)`
		    AC_MSG_CHECKING([for streams lis include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lis_includes="$streams_dir $streams_bld"
			streams_cv_lis_ldadd=`echo "$streams_bld/../libLiS.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lis_ldadd32=`echo "$streams_bld/../lib32/libLiS.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lis_modmap=`echo "$streams_bld/../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lis_symver=`echo "$streams_bld/../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lis_manpath=`echo "$streams_bld/../man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for streams lis include directory])
	fi
	if test :"${streams_cv_lis_includes:-no}" = :no ; then
	    # note if linux kernel macros have not run this reduces
	    streams_cv_lis_includes=
	    eval "streams_search_path=\"
		${DESTDIR}${includedir}/LiS
		${DESTDIR}${rootdir}${oldincludedir}/LiS
		${DESTDIR}${rootdir}/usr/include/LiS
		${DESTDIR}${rootdir}/usr/local/include/LiS
		${DESTDIR}${rootdir}/usr/src/LiS/include
		${DESTDIR}${oldincludedir}/LiS
		${DESTDIR}/usr/include/LiS
		${DESTDIR}/usr/local/include/LiS
		${DESTDIR}/usr/src/LiS/include\""
	    streams_search_path=`echo "$streams_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    AC_MSG_RESULT([(searching)])
	    for streams_dir in $streams_search_path ; do
		if test -d "$streams_dir" ; then
		    AC_MSG_CHECKING([for streams lis include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lis_includes="$streams_dir"
			streams_cv_lis_modmap=
			streams_cv_lis_symver=
			streams_cv_lis_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for streams lis include directory])
	fi
    ])
    AC_CACHE_CHECK([for streams lis ldadd],[streams_cv_lis_ldadd],[dnl
	for streams_dir in $streams_cv_lis_includes ; do
	    if test -f $streams_dir/../libLiS.la ; then
		streams_cv_lis_ldadd=`echo "$streams_dir/../libLiS.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for streams lis ldflags],[streams_cv_lis_ldflags],[dnl
	if test -z "$streams_cv_lis_ldadd" ; then
	    streams_cv_lis_ldflags="-lLIS"
	else
	    streams_cv_lis_ldflags="-L$(dirname $streams_cv_lis_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for streams lis ldadd 32-bit],[streams_cv_lis_ldadd32],[dnl
	for streams_dir in $streams_cv_lis_includes ; do
	    if test -f $streams_dir/../lib32/libLiS.la ; then
		streams_cv_lis_ldadd32=`echo "$streams_dir/../lib32/libLiS.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for streams lis ldflags 32-bit],[streams_cv_lis_ldflags32],[dnl
	if test -z "$streams_cv_lis_ldadd32" ; then
	    streams_cv_lis_ldflags32="-lLIS"
	else
	    streams_cv_lis_ldflags32="-L$(dirname $streams_cv_lis_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for streams lis modmap],[streams_cv_lis_modmap],[dnl
	for streams_dir in $streams_cv_lis_includes ; do
	    if test -f $streams_dir/../Modules.map ; then
		streams_cv_lis_modmap=`echo "$streams_dir/../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_MSG_CHECKING([for streams lis symver],[streams_cv_lis_symver],[dnl
	for streams_dir in $streams_cv_lis_includes ; do
	    if test -f $streams_dir/../Module.symvers ; then
		streams_cv_lis_symver=`echo "$streams_dir/../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_MSG_CHECKING([for streams lis manpath],[streams_cv_lis_manpath],[dnl
	for streams_dir in $streams_cv_lis_includes ; do
	    if test -d $streams_dir/../man ; then
		streams_cv_lis_manpath=`echo "$streams_dir/../man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${streams_cv_lis_includes:-no}" = :no ; then
	AC_MSG_WARN([
*** 
*** Configure could not find the LiS STREAMS include directories.  If
*** you wish to use the LiS STREAMS package you will need to specify the
*** location of the Linux STREAMS (LiS) include directories with the
*** --with-lis=@<:@DIRECTORY@:>@ option to configure and try again.
***
*** Perhaps you just forgot to load the LiS STREAMS package?  The LiS
*** STREAMS package is available from The OpenSS7 Project download page
*** at http://www.openss7.org/ and comes in a tarball named something
*** like "LiS-2.18.3.tar.gz".
*** ])
    fi
    streams_what="sys/LiS/config.h"
    AC_CACHE_CHECK([for streams lis $streams_what], [streams_cv_lis_config], [dnl
	streams_cv_lis_config=no
	if test -n "$streams_cv_lis_includes" ; then
	    for streams_dir in $streams_cv_lis_includes ; do
		# old place for config
		if test -f "$streams_dir/$streams_what" ; then
		    streams_cv_lis_config="$streams_dir/$streams_what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" ; then
			streams_cv_lis_config="$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" 
			break
		    fi
		fi
	    done
	fi
    ])
    AC_CACHE_CHECK([for streams lis version], [streams_cv_lis_version], [dnl
	streams_what="sys/LiS/version.h"
	streams_file=
	if test -n "$streams_cv_lis_includes" ; then
	    for streams_dir in $streams_cv_lis_includes ; do
		# old place for version
		if test -f "$streams_dir/$streams_what" ; then
		    streams_file="$streams_dir/$streams_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" ; then
			streams_file="$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" 
			break
		    fi
		fi
	    done
	fi
	if test :${streams_file:-no} != :no ; then
	    streams_cv_lis_version=`grep '#define.*\<LIS_VERSION\>' $streams_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    LIS_VERSION="${streams_cv_lis_version:-0:2.18.1}"
    AC_SUBST([LIS_VERSION])
    streams_what="sys/LiS/modversions.h"
    AC_CACHE_CHECK([for streams lis $streams_what], [streams_cv_lis_modversions], [dnl
	streams_cv_lis_modversions='no'
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$streams_cv_lis_includes" ; then
		for streams_dir in $streams_cv_lis_includes ; do
		    # old place for modversions
		    if test -f "$streams_dir/$streams_what" ; then
			streams_cv_lis_modversions="$streams_dir/$streams_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
			if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" ; then
			    streams_cv_lis_includes="$streams_dir/$linux_cv_k_release/$target_cpu $streams_cv_lis_includes"
			    streams_cv_lis_modversions="$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" 
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_CACHE_CHECK([for streams lis sys/LiS/module.h], [streams_cv_lis_module], [dnl
	streams_cv_lis_module='no'
	if test -n "$streams_cv_lis_includes" ; then
	    for streams_dir in $streams_cv_lis_includes ; do
		if test -f "$streams_dir/sys/LiS/module.h" ; then
		    streams_cv_lis_module='yes'
		    break
		fi
	    done
	fi
    ])
])# _LINUX_STREAMS_LIS_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_LFS_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LFS_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS header files.  The package normally requires
    # either Linux STREAMS or Linux Fast-STREAMS header files (or both) to compile.
    AC_CACHE_CHECK([for streams lfs include directory], [streams_cv_lfs_includes], [dnl
	streams_what="sys/stream.h"
	if test :"${with_lfs:-no}" != :no -a :"${with_lfs:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_lfs)])
	    for streams_dir in $with_lfs ; do
		if test -d "$streams_dir" ; then
		    AC_MSG_CHECKING([for streams lfs include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lfs_includes="$with_lfs"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${streams_cv_lfs_includes:-no}" = :no ; then
		AC_MSG_WARN([
***
*** You have specified include directories using:
***
***	    --with-lfs="$with_lfs"
***
*** however, $streams_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	fi
	if test :"${streams_cv_lfs_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    streams_search_path="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/streams/include} ${os7_cv_master_builddir:+$os7_cv_master_builddir/streams/include}"
	    for streams_dir in $streams_search_path ; do
		if test -d "$streams_dir" ; then
		    AC_MSG_CHECKING([for streams lfs include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lfs_includes="$streams_search_path"
			streams_cv_lfs_ldadd="$os7_cv_master_builddir/streams/libstreams.la"
			streams_cv_lfs_ldadd32="$os7_cv_master_builddir/streams/lib32/libstreams.la"
			streams_cv_lfs_modmap="$os7_cv_master_builddir/streams/Modules.map"
			streams_cv_lfs_symver="$os7_cv_master_builddir/streams/Module.symvers"
			streams_cv_lfs_manpath="$os7_cv_master_builddir/streams/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		fi
	    done
	    AC_MSG_CHECKING([for streams lfs include directory])
	fi
	if test :"${streams_cv_lfs_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    streams_here=`pwd`
	    AC_MSG_RESULT([(searching from $streams_here)])
	    for streams_dir in \
		$srcdir/streams*/include \
		$srcdir/../streams*/include \
		../_build/$srcdir/../../streams*/include \
		../_build/$srcdir/../../../streams*/include
	    do
		if test -d "$streams_dir" ; then
		    streams_bld=`echo $streams_dir | sed -e "s|^$srcdir/|$streams_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    streams_dir=`(cd $streams_dir; pwd)`
		    AC_MSG_CHECKING([for streams lfs include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lfs_includes="$streams_dir $streams_bld"
			streams_cv_lfs_ldadd=`echo "$streams_bld/../libstreams.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lfs_ldadd32=`echo "$streams_bld/../lib32/libstreams.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lfs_modmap=`echo "$streams_bld/../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lfs_symver=`echo "$streams_bld/../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			streams_cv_lfs_manpath=`echo "$streams_bld/../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for streams lfs include directory])
	fi
	if test :"${streams_cv_lfs_includes:-no}" = :no ; then
	    # note if linux kernel macros have not run this reduces
	    streams_cv_lfs_includes=
	    eval "streams_search_path=\"
		${DESTDIR}${includedir}/streams
		${DESTDIR}${rootdir}${oldincludedir}/streams
		${DESTDIR}${rootdir}/usr/include/streams
		${DESTDIR}${rootdir}/usr/local/include/streams
		${DESTDIR}${rootdir}/usr/src/streams/include
		${DESTDIR}${oldincludedir}/streams
		${DESTDIR}/usr/include/streams
		${DESTDIR}/usr/local/include/streams
		${DESTDIR}/usr/src/streams/include\""
	    streams_search_path=`echo "$streams_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    AC_MSG_RESULT([(searching)])
	    for streams_dir in $streams_search_path ; do
		if test -d "$streams_dir" ; then
		    AC_MSG_CHECKING([for stream lfs include directory... $streams_dir])
		    if test -r "$streams_dir/$streams_what" ; then
			streams_cv_lfs_includes="$streams_dir"
			streams_cv_lfs_modmap=
			streams_cv_lfs_symver=
			streams_cv_lfs_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for streams lfs include directory])
	fi
    ])
    AC_CACHE_CHECK([for streams lfs ldadd native],[streams_cv_lfs_ldadd],[dnl
	for streams_dir in $streams_cv_lfs_includes ; do
	    if test -f $streams_dir/../libstreams.la ; then
		streams_cv_lfs_ldadd=`echo "$streams_dir/../libstreams.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for streams lfs ldflags],[streams_cv_lfs_ldflags],[dnl
	if test -z "$streams_cv_lfs_ldadd" ; then
	    streams_cv_lfs_ldflags="-lstreams"
	else
	    streams_cv_lfs_ldflags="-L$(dirname $streams_cv_lfs_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for streams lfs ldadd 32-bit],[streams_cv_lfs_ldadd32],[dnl
	for streams_dir in $streams_cv_lfs_includes ; do
	    if test -f $streams_dir/../lib32/libstreams.la ; then
		streams_cv_lfs_ldadd32=`echo "$streams_dir/../lib32/libstreams.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for streams lfs ldflags 32-bit],[streams_cv_lfs_ldflags32],[dnl
	if test -z "$streams_cv_lfs_ldadd32" ; then
	    streams_cv_lfs_ldflags32="-lstreams"
	else
	    streams_cv_lfs_ldflags32="-L$(dirname $streams_cv_lfs_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for streams lfs modmap],[streams_cv_lfs_modmap],[dnl
	for streams_dir in $streams_cv_lfs_includes ; do
	    if test -f $streams_dir/../Modules.map ; then
		streams_cv_lfs_modmap=`echo "$streams_dir/../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_MSG_CHECKING([for streams lfs symver],[streams_cv_lfs_symver],[dnl
	for streams_dir in $streams_cv_lfs_includes ; do
	    if test -f $streams_dir/../Module.symvers ; then
		streams_cv_lfs_symver=`echo "$streams_dir/../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_MSG_CHECKING([for streams lfs manpath],[streams_cv_lfs_manpath],[dnl
	for streams_dir in $streams_cv_lfs_includes ; do
	    if test -d $streams_dir/../doc/man ; then
		streams_cv_lfs_manpath=`echo "$streams_dir/../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${streams_cv_lfs_includes:-no}" = :no ; then
	AC_MSG_WARN([
*** 
*** Configure could not find the LfS STREAMS include directories.  If
*** you wish to use the LfS STREAMS package you will need to specify the
*** location of the Linux Fast STREAMS (LfS) include directories with
*** the --with-lfs option to configure and try again.
***
*** Perhaps you just forgot to load the LfS STREAMS package?  The LfS
*** STREAMS package is available from The OpenSS7 Project download page
*** at http://www.openss7.org/ and comes in a tarball named something
*** like "streams-0.9.2.1-1.tar.gz".
*** ])
    fi
    streams_what="sys/config.h"
    AC_CACHE_CHECK([for streams lfs $streams_what], [streams_cv_lfs_config], [dnl
	streams_cv_lfs_config=no
	if test -n "$streams_cv_lfs_includes" ; then
	    for streams_dir in $streams_cv_lfs_includes ; do
		# old place for config
		if test -f "$streams_dir/$streams_what" ; then
		    streams_cv_lfs_config="$streams_dir/$streams_what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" ; then
			streams_cv_lfs_config="$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" 
			break
		    fi
		fi
	    done
	fi
    ])
    AC_CACHE_CHECK([for streams lfs version], [streams_cv_lfs_version], [dnl
	streams_what="sys/streams/version.h"
	streams_file=
	if test -n "$streams_cv_lfs_includes" ; then
	    for streams_dir in $streams_cv_lfs_includes ; do
		# old place for version
		if test -f "$streams_dir/$streams_what" ; then
		    streams_file="$streams_dir/$streams_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" ; then
			streams_file="$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" 
			break
		    fi
		fi
	    done
	fi
	if test :${streams_file:-no} != :no ; then
	    streams_cv_lfs_version=`grep '#define.*\<STREAMS_VERSION\>' $streams_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    LFS_VERSION="${streams_cv_lfs_version:-0:0.9.2.1}"
    AC_SUBST([LFS_VERSION])
    streams_what="sys/streams/modversions.h"
    AC_CACHE_CHECK([for streams lfs $streams_what], [streams_cv_lfs_modversions], [dnl
	streams_cv_lfs_modversions='no'
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$streams_cv_lfs_includes" ; then
		for streams_dir in $streams_cv_lfs_includes ; do
		    # old place for modversions
		    if test -f "$streams_dir/$streams_what" ; then
			streams_cv_lfs_modversions="$streams_dir/$streams_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
			if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what" ; then
			    streams_cv_lfs_includes="$streams_dir/$linux_cv_k_release/$target_cpu $streams_cv_lfs_includes"
			    streams_cv_lfs_modversions="$streams_dir/$linux_cv_k_release/$target_cpu/$streams_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
])# _LINUX_STREAMS_LFS_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_KERNEL
# -----------------------------------------------------------------------------
# Need to know about irqreturn_t for os7/compat.h STREAMS compatibility file.
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_KERNEL], [dnl
    _LINUX_CHECK_TYPES([irqreturn_t], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>	/* for irqreturn_t */ 
    ])
])# _LINUX_STREAMS_KERNEL
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_OUTPUT], [dnl
    case "$streams_cv_package" in
	LiS)
	    _LINUX_STREAMS_LIS_DEFINES
	    : ;;
	LfS)
	    _LINUX_STREAMS_LFS_DEFINES
	    : ;;
    esac
])# _LINUX_STREAMS_OUTPUT
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_LIS_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LIS_DEFINES], [dnl
    if test :"${streams_cv_lis_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_LIS_MODVERSIONS_H], [1], [Define when the
	    LiS release supports module versions such as the OpenSS7 autoconf
	    release of LiS.])
    fi
    if test :"${streams_cv_lis_module:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_LIS_MODULE_H], [1], [Define when the LiS
	    release provides its own module.h file such as 2.17 GCOM LiS
	    releases.])
    fi
    case "$target_cpu" in
	alpha*)			: ;;
	arm*)			: ;;
	cris*)			: ;;
	i?86* | k6* | athlon*)	: ;;
	ia64)			: ;;
	m68*)			: ;;
	mips64*)		: ;;
	mips*)			: ;;
	hppa*)
dnl
dnl	    Define when compiling for HPPA.  This define is only used for linux
dnl	    kernel target.  This is really the wrong way to go about doing this:
dnl	    the function should be checked for by autoconf instead of placing
dnl	    the architectural dependencies in the LiS source.  The define is
dnl	    used in "head/linux-mdep.c" to determine whether lis_pci_cleanup
dnl	    exists; "head/linux/exports.c" to determine whether a bunch of
dnl	    functions are available; "head/osif.c" to determine whether a bunch
dnl	    of PCI DMA mapping functions are available.
dnl
	    STREAMS_CPPFLAGS="-D_HPPA_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	ppc64* | powerpc64* )
dnl
dnl	    Define when compiling for PPC64.  This define is only used for linux
dnl	    kernel target.  This is really the wrong way to go about doing this:
dnl	    the function should be checked for by autoconf instead of placing
dnl	    the architectural dependencies in the LiS source.  The define is
dnl	    used in "head/linux-mdep.c" to determine whether lis_pci_cleanup
dnl	    exists; "head/linux/exports.c" to determine whether a bunch of
dnl	    functions are available; "head/osif.c" to determine whether a bunch
dnl	    of PCI DMA mapping functions are available; "include/sys/osif.h" to
dnl	    determine whether a bunch of PCI DMA mapping functions are
dnl	    available.
dnl
	    STREAMS_CPPFLAGS="-D_PPC64_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    STREAMS_CPPFLAGS="-D_PPC_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	ppc* | powerpc*)	
dnl
dnl	    Define when compiling for PPC.  This define is only used for linux
dnl	    kernel target.  This is really the wrong way to go about doing this:
dnl	    the function should be checked for by autoconf instead of placing
dnl	    the architectural dependencies in the LiS source.  The define is
dnl	    used in <LiS/include/sys/osif.h> and "head/osif.c" to determine
dnl	    whether PCI BIOS is present; in (head/linux-mdep.c) to determine
dnl	    whether cpu binding is possible; to determine whether
dnl	    spin_is_locked() is available in "head/linux/lislocks.c"; in
dnl	    "head/mod.c" to determine whether to define struct pt_regs; and in
dnl	    <LiS/include/sys/lislocks.h> to determine the size of semaphore
dnl	    memory.
dnl
	    STREAMS_CPPFLAGS="-D_PPC_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	s390x*)			
dnl
dnl	    Define when compiling for S390X.  This define is only used for the
dnl	    linux kernel target.  This is really the wrong way to go about doing
dnl	    this: the function should be checked for by autoconf instead of
dnl	    placing the architectural depdendencies in the LiS source.  The
dnl	    define is used in "head/linux-mdep.c" to determine whether
dnl	    lis_pci_cleanup exists; "head/linux/exports.c" to determine whether
dnl	    a bunch of functions are available; "head/osif.c" to determine
dnl	    whether a bunch of PCI DMA mapping functions are available;
dnl	    "include/sys/osif.h" to determine whether a bunch of PCI DMA mapping
dnl	    functions are available.
dnl
	    STREAMS_CPPFLAGS="-D_S390X_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    STREAMS_CPPFLAGS="-D_S390_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	s390*)			
dnl
dnl	    Define when compiling for S390.  Strangely enough, _S390_LIS_ is
dnl	    never checked without _S390X_LIS_.  Rendering it as an alias for the
dnl	    above.
dnl
	    STREAMS_CPPFLAGS="-D_S390_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	sh*)			: ;;
	sparc64*)		
dnl
dnl	    Define when compiling for Sparc64.  This define is only used for the
dnl	    linux kernel target.  This is really the wrong way to go about doing
dnl	    this: the function should be checked for by autoconf instead of
dnl	    placing the architectural dependencies in the LiS source.  The
dnl	    define is used to determine when ioremap functions are not available
dnl	    <LiS/include/osif.h>.  Strangely enough, none of the other checks
dnl	    are performed as for _SPARC_LIS_ below.
dnl
	    STREAMS_CPPFLAGS="-D_SPARC64_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    STREAMS_CPPFLAGS="-D_SPARC_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	sparc*)			
dnl
dnl	    Define when compiling for Sparc.  This define is used for the linux
dnl	    kernel target.  This is really the wrong way to go about doing this:
dnl	    the function should be checked for by autoconf instead of placing
dnl	    architectural depedencies in the LiS source.  The define is used to
dnl	    determine when ioremap functions are not available
dnl	    <LiS/include/osif.h>, when PCI BIOS is not present (head/osif.c),
dnl	    and when <linux/poll.h> is missing POLLMSG <LiS/include/sys/poll.h>
dnl
	    STREAMS_CPPFLAGS="-D_SPARC_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	x86_64*)
dnl
dnl	    Define when compiling for X86_64.  This define is only used for
dnl	    linux kernel target.  This is really the wrong way to go about doing
dnl	    this: the function should be checked for by autoconf instead of
dnl	    placing the architectural dependencies in the LiS source.  The
dnl	    define is used in "head/linux-mdep.c" to determine whether
dnl	    lis_pci_cleanup exists; "head/linux/exports.c" to determine whether
dnl	    a bunch of functions are available; "head/osif.c" to determine
dnl	    whether a bunch of PCI DMA mapping functions are available;
dnl	    "include/sys/osif.h" to determine whether a bunch of PCI DMA mapping
dnl	    functions are available.
dnl
	    STREAMS_CPPFLAGS="-D_X86_64_LIS_${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	*)			: ;;
    esac
    STREAMS_CPPFLAGS="-DLIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    STREAMS_LDADD="$streams_cv_lis_ldadd"
    STREAMS_LDADD32="$streams_cv_lis_ldadd32"
    STREAMS_LDFLAGS="$streams_cv_lis_ldflags"
    STREAMS_LDFLAGS32="$streams_cv_lis_ldflags32"
    STREAMS_MODMAP="$streams_cv_lis_modmap"
    STREAMS_SYMVER="$streams_cv_lis_symver"
    STREAMS_MANPATH="$streams_cv_lis_manpath"
    STREAMS_VERSION="$streams_cv_lis_version"
    MODPOST_INPUTS="${MODPOST_INPUTS}${STREAMS_SYMVER:+${MODPOST_INPUTS:+ }${STREAMS_SYMVER}}"
])# _LINUX_STREAMS_LIS_DEFINES
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_LFS_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LFS_DEFINES], [dnl
    if test :"${streams_cv_lfs_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_STREAMS_MODVERSIONS_H], [1], [Define when
	    the Linux Fast-STREAMS release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    STREAMS_CPPFLAGS="-DLFS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    STREAMS_LDADD="$streams_cv_lfs_ldadd"
    STREAMS_LDADD32="$streams_cv_lfs_ldadd32"
    STREAMS_LDFLAGS="$streams_cv_lfs_ldflags"
    STREAMS_LDFLAGS32="$streams_cv_lfs_ldflags32"
    STREAMS_MODMAP="$streams_cv_lfs_modmap"
    STREAMS_SYMVER="$streams_cv_lfs_symver"
    STREAMS_MANPATH="$streams_cv_lfs_manpath"
    STREAMS_VERSION="$streams_cv_lfs_version"
    MODPOST_INPUTS="${MODPOST_INPUTS}${STREAMS_SYMVER:+${MODPOST_INPUTS:+ }${STREAMS_SYMVER}}"
    AC_DEFINE_UNQUOTED([HAVE_BCID_T], [1], [Linux Fast-STREAMS has this type.])
    AC_DEFINE_UNQUOTED([HAVE_BUFCALL_ID_T], [1], [Linux Fast-STREAMS has this type.])
])# _LINUX_STREAMS_LFS_DEFINES
# =============================================================================

# =============================================================================
# _LINUX_STREAMS_
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_], [dnl
])# _LINUX_STREAMS_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
