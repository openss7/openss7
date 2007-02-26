# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: isdn.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.3 $) $Date: 2006-12-28 08:32:31 $
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
# Last Modified $Date: 2006-12-28 08:32:31 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: isdn.m4,v $
# Revision 0.9.2.3  2006-12-28 08:32:31  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.2  2006/10/16 11:44:46  brian
# - change search file
#
# Revision 0.9.2.1  2006/10/16 08:28:02  brian
# - added new package discovery macros
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS ISDN
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS ISDN package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strisdn loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _ISDN
# -----------------------------------------------------------------------------
# Check for the existence of ISDN header files, particularly isdn/sys/isdni.h
# ISDN header files are required for building the ISDN interface.
# Without ISDN header files, the ISDN interface will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _ISDN_OPTIONS
    _ISDN_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_ISDN_KERNEL])
    _ISDN_USER
    _ISDN_OUTPUT
    AC_SUBST([ISDN_CPPFLAGS])dnl
    AC_SUBST([ISDN_MODFLAGS])dnl
    AC_SUBST([ISDN_LDADD])dnl
    AC_SUBST([ISDN_LDADD32])dnl
    AC_SUBST([ISDN_LDFLAGS])dnl
    AC_SUBST([ISDN_LDFLAGS32])dnl
    AC_SUBST([ISDN_MODMAP])dnl
    AC_SUBST([ISDN_SYMVER])dnl
    AC_SUBST([ISDN_MANPATH])dnl
    AC_SUBST([ISDN_VERSION])dnl
])# _ISDN
# =============================================================================

# =============================================================================
# _ISDN_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_OPTIONS], [dnl
    AC_ARG_WITH([isdn],
		AS_HELP_STRING([--with-isdn=HEADERS],
			       [specify the ISDN header file directory.
				@<:@default=$INCLUDEDIR/isdn@:>@]),
		[with_isdn="$withval"],
		[with_isdn=''])
])# _ISDN_OPTIONS
# =============================================================================

# =============================================================================
# _ISDN_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_SETUP], [dnl
    _ISDN_CHECK_HEADERS
    for isdn_include in $isdn_cv_includes ; do
	ISDN_CPPFLAGS="${ISDN_CPPFLAGS}${ISDN_CPPFLAGS:+ }-I${isdn_include}"
    done
    if test :"${isdn_cv_config:-no}" != :no ; then
	ISDN_CPPFLAGS="${ISDN_CPPFLAGS}${ISDN_CPPFLAGS:+ }-include ${isdn_cv_config}"
    fi
    if test :"${isdn_cv_modversions:-no}" != :no ; then
	ISDN_MODFLAGS="${ISDN_MODFLAGS}${ISDN_MODFLAGS:+ }-include ${isdn_cv_modversions}"
    fi
])# _ISDN_SETUP
# =============================================================================

# =============================================================================
# _ISDN_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS ISDN header files.  The
    # package normally requires ISDN header files to compile.
    AC_CACHE_CHECK([for isdn include directory], [isdn_cv_includes], [dnl
	isdn_what="sys/isdni.h"
	if test :"${with_isdn:-no}" != :no -a :"${with_isdn:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_isdn)])
	    for isdn_dir in $with_isdn ; do
		if test -d "$isdn_dir" ; then
		    AC_MSG_CHECKING([for isdn include directory... $isdn_dir])
		    if test -r "$isdn_dir/$isdn_what" ; then
			isdn_cv_includes="$with_isdn"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${isdn_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-isdn="$with_isdn"
***
*** however, $isdn_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for isdn include directory])
	fi
	if test :"${isdn_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    isdn_search_path="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strisdn/src/include} ${os7_cv_master_builddir:+$os7_cv_master_builddir/strisdn/src/include}"
	    for isdn_dir in $isdn_search_path ; do
		if test -d "$isdn_dir" ; then
		    AC_MSG_CHECKING([for isdn include directory... $isdn_dir])
		    if test -r "$isdn_dir/$isdn_what" ; then
			isdn_cv_includes="$isdn_search_path"
			isdn_cv_ldadd= # "$os7_cv_master_builddir/strisdn/libisdn.la"
			isdn_cv_ldadd32= # "$os7_cv_master_builddir/strisdn/lib32/libisdn.la"
			isdn_cv_modmap= # "$os7_cv_master_builddir/strisdn/Modules.map"
			isdn_cv_symver= # "$os7_cv_master_builddir/strisdn/Module.symvers"
			isdn_cv_manpath="$os7_cv_master_builddir/strisdn/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for isdn include directory])
	fi
	if test :"${isdn_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    isdn_here=`pwd`
	    AC_MSG_RESULT([(searching from $isdn_here)])
	    for isdn_dir in \
		$srcdir/strisdn*/src/include \
		$srcdir/../strisdn*/src/include \
		../_build/$srcdir/../../strisdn*/src/include \
		../_build/$srcdir/../../../strisdn*/src/include
	    do
		if test -d "$isdn_dir" ; then
		    isdn_bld=`echo $isdn_dir | sed -e "s|^$srcdir/|$isdn_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    isdn_dir=`(cd $isdn_dir; pwd)`
		    AC_MSG_CHECKING([for isdn include directory... $isdn_dir])
		    if test -r "$isdn_dir/$isdn_what" ; then
			isdn_cv_includes="$isdn_dir $isdn_bld"
			isdn_cv_ldadd= # `echo "$isdn_bld/../../libisdn.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			isdn_cv_ldadd32= # `echo "$isdn_bld/../../lib32/libisdn.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			isdn_cv_modmap= # `echo "$isdn_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			isdn_cv_symver= # `echo "$isdn_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			isdn_cv_manpath=`echo "$isdn_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for isdn include directory])
	fi
	if test :"${isdn_cv_includes:-no}" = :no ; then
	    # ISDN header files are normally found in the strisdn package now.
	    # They used to be part of the ISDN add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "isdn_search_path=\"
			${DESTDIR}${includedir}/strisdn
			${DESTDIR}${rootdir}${oldincludedir}/strisdn
			${DESTDIR}${rootdir}/usr/include/strisdn
			${DESTDIR}${rootdir}/usr/local/include/strisdn
			${DESTDIR}${rootdir}/usr/src/strisdn/src/include
			${DESTDIR}${includedir}/isdn
			${DESTDIR}${rootdir}${oldincludedir}/isdn
			${DESTDIR}${rootdir}/usr/include/isdn
			${DESTDIR}${rootdir}/usr/local/include/isdn
			${DESTDIR}${oldincludedir}/strisdn
			${DESTDIR}/usr/include/strisdn
			${DESTDIR}/usr/local/include/strisdn
			${DESTDIR}/usr/src/strisdn/src/include
			${DESTDIR}${oldincludedir}/isdn
			${DESTDIR}/usr/include/isdn
			${DESTDIR}/usr/local/include/isdn\""
		    ;;
		(LfS)
		    eval "isdn_search_path=\"
			${DESTDIR}${includedir}/strisdn
			${DESTDIR}${rootdir}${oldincludedir}/strisdn
			${DESTDIR}${rootdir}/usr/include/strisdn
			${DESTDIR}${rootdir}/usr/local/include/strisdn
			${DESTDIR}${rootdir}/usr/src/strisdn/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strisdn
			${DESTDIR}/usr/include/strisdn
			${DESTDIR}/usr/local/include/strisdn
			${DESTDIR}/usr/src/strisdn/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    isdn_search_path=`echo "$isdn_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    isdn_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for isdn_dir in $isdn_search_path ; do
		if test -d "$isdn_dir" ; then
		    AC_MSG_CHECKING([for isdn include directory... $isdn_dir])
		    if test -r "$isdn_dir/$isdn_what" ; then
			isdn_cv_includes="$isdn_dir"
			isdn_cv_modmap=
			isdn_cv_symver=
			isdn_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for isdn include directory])
	fi
    ])
    AC_CACHE_CHECK([for isdn ldadd native],[isdn_cv_ldadd],[dnl
	for isdn_dir in $isdn_cv_includes ; do
	    if test -f "$isdn_dir/../../libisdn.la" ; then
		isdn_cv_ldadd=`echo "$isdn_dir/../../libisdn.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for isdn ldflags],[isdn_cv_ldflags],[dnl
	if test -z "$isdn_cv_ldadd" ; then
	    isdn_cv_ldflags= # '-lisdn'
	else
	    isdn_cv_ldflags= # "-L$(dirname $isdn_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for isdn ldadd 32-bit],[isdn_cv_ldadd32],[dnl
	for isdn_dir in $isdn_cv_includes ; do
	    if test -f "$isdn_dir/../../libisdn.la" ; then
		isdn_cv_ldadd32=`echo "$isdn_dir/../../lib32/libisdn.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for isdn ldflags 32-bit],[isdn_cv_ldflags32],[dnl
	if test -z "$isdn_cv_ldadd32" ; then
	    isdn_cv_ldflags32= # '-lisdn'
	else
	    isdn_cv_ldflags32= # "-L$(dirname $isdn_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for isdn modmap],[isdn_cv_modmap],[dnl
	for isdn_dir in $isdn_cv_includes ; do
	    if test -f "$isdn_dir/../../Modules.map" ; then
		isdn_cv_modmap=`echo "$isdn_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for isdn symver],[isdn_cv_symver],[dnl
	for isdn_dir in $isdn_cv_includes ; do
	    if test -f "$isdn_dir/../../Module.symvers" ; then
		isdn_cv_symver=`echo "$isdn_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for isdn manpath],[isdn_cv_manpath],[dnl
	for isdn_dir in $isdn_cv_includes ; do
	    if test -d "$isdn_dir/../../doc/man" ; then
		isdn_cv_manpath=`echo "$isdn_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${isdn_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS ISDN include directories.  If
*** you wish to use the STREAMS ISDN package, you will need to specify
*** the location of the STREAMS ISDN (strisdn) include directories with
*** the --with-isdn=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS ISDN package?  The
*** STREAMS strisdn package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strisdn-0.9a.3.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for isdn version], [isdn_cv_version], [dnl
	isdn_what="sys/strisdn/version.h"
	isdn_file=
	if test -n "$isdn_cv_includes" ; then
	    for isdn_dir in $isdn_cv_includes ; do
		# old place for version
		if test -f "$isdn_dir/$isdn_what" ; then
		    isdn_file="$isdn_dir/$isdn_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$isdn_dir/$linux_cv_k_release/$target_cpu/$isdn_what" ; then
			isdn_file="$isdn_dir/$linux_cv_k_release/$target_cpu/$isdn_what"
			break
		    fi
		fi
	    done
	fi
	if test :"${isdn_file:-no}" != :no ; then
	    isdn_cv_version=`grep '#define.*\<STRISDN_VERSION\>' $isdn_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    isdn_what="sys/config.h"
    AC_CACHE_CHECK([for isdn $isdn_what], [isdn_cv_config], [dnl
	isdn_cv_config=
	if test -n "$isdn_cv_includes" ; then
	    for isdn_dir in $isdn_cv_includes ; do
		# old place for config
		if test -f "$isdn_dir/$isdn_what" ; then
		    isdn_cv_config="$isdn_dir/$isdn-what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$isdn_dir/$linux_cv_k_release/$target_cpu/$isdn_what" ; then
			isdn_cv_config="$isdn_dir/$linux_cv_k_release/$target_cpu/$isdn_what"
			break
		    fi
		fi
	    done
	fi
    ])
    isdn_what="sys/strisdn/modversions.h"
    AC_CACHE_CHECK([for isdn $isdn_what], [isdn_cv_modversions], [dnl
	isdn_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$isdn_cv_includes" ; then
		for isdn_dir in $isdn_cv_includes ; do
		    # old place for modversions
		    if test -f "$isdn_dir/$isdn_what" ; then
			isdn_cv_modversions="$isdn_dir/$isdn_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			if test "$isdn_dir/$linux_cv_k_release/$target_cpu/$isdn_what" ; then
			    isdn_cv_includes="$isdn_dir/$linux_cv_k_release/$target_cpu $isdn_cv_includes"
			    isdn_cv_modversions="$isdn_dir/$linux_cv_k_release/$target_cpu/$isdn_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_MSG_CHECKING([for isdn added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_isdn" ; then
	if test :"${isdn_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_isdn --with-isdn\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-isdn'"
	    AC_MSG_RESULT([--with-isdn])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_isdn --without-isdn\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-isdn'"
	    AC_MSG_RESULT([--without-isdn])
	fi
    else
	AC_MSG_RESULT([--with-isdn="$with_isdn"])
    fi
])# _ISDN_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _ISDN_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_KERNEL], [dnl
])# _ISDN_KERNEL
# =============================================================================

# =============================================================================
# _ISDN_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_OUTPUT], [dnl
    _ISDN_DEFINES
])# _ISDN_OUTPUT
# =============================================================================

# =============================================================================
# _ISDN_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_DEFINES], [dnl
    if test :"${isdn_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_ISDN_MODVERSIONS_H], [1], [Define when
	    the STREAMS ISDN release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    ISDN_CPPFLAGS="${ISDN_CPPFLAGS:+ ${ISDN_CPPFLAGS}}"
    ISDN_LDADD="$isdn_cv_ldadd"
    ISDN_LDADD32="$isdn_cv_ldadd32"
    ISDN_LDFLAGS="$isdn_cv_ldflags"
    ISDN_LDFLAGS32="$isdn_cv_ldflags32"
    ISDN_MODMAP="$isdn_cv_modmap"
    ISDN_SYMVER="$isdn_cv_symver"
    ISDN_MANPATH="$isdn_cv_manpath"
    ISDN_VERSION="$isdn_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${ISDN_SYMVER:+${MODPOST_INPUTS:+ }${ISDN_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strisdn for
	that matter.
    ])
])# _ISDN_DEFINES
# =============================================================================

# =============================================================================
# _ISDN_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_USER], [dnl
])# _ISDN_USER
# =============================================================================

# =============================================================================
# _ISDN_
# -----------------------------------------------------------------------------
AC_DEFUN([_ISDN_], [dnl
])# _ISDN_
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
