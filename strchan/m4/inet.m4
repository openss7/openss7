# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: inet.m4,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2007/02/28 11:51:31 $
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
# Last Modified $Date: 2007/02/28 11:51:31 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: inet.m4,v $
# Revision 0.9.2.34  2007/02/28 11:51:31  brian
# - make sure build directory exists
#
# Revision 0.9.2.33  2007/02/22 08:36:38  brian
# - balance parentheses
#
# Revision 0.9.2.32  2006-12-28 08:32:31  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.31  2006/09/29 10:57:45  brian
# - autoconf does not like multiline cache variables
#
# Revision 0.9.2.30  2006/09/29 03:46:16  brian
# - substitute LDFLAGS32
#
# Revision 0.9.2.29  2006/09/29 03:22:38  brian
# - handle flags better
#
# Revision 0.9.2.28  2006/09/27 05:08:41  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.27  2006/09/18 00:33:51  brian
# - added checks for 32bit compatibility libraries
#
# Revision 0.9.2.26  2006/03/14 09:20:46  brian
# - typo
#
# Revision 0.9.2.25  2006/03/14 09:04:10  brian
# - syntax consistency, advanced search
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS INET
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS INET package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strinet loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _INET
# -----------------------------------------------------------------------------
# Check for the existence of INET header files, particularly sys/xti_inet.h.
# INET header files are required for building the XTI interface for INET.
# Without INET header files, the XTI interface to INET will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_INET], [dnl
    AC_REQUIRE([_XTI])dnl
    _INET_OPTIONS
    _INET_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_INET_KERNEL])
    _INET_USER
    _INET_OUTPUT
    AC_SUBST([INET_CPPFLAGS])dnl
    AC_SUBST([INET_MODFLAGS])dnl
    AC_SUBST([INET_LDADD])dnl
    AC_SUBST([INET_LDADD32])dnl
    AC_SUBST([INET_LDFLAGS])dnl
    AC_SUBST([INET_LDFLAGS32])dnl
    AC_SUBST([INET_MODMAP])dnl
    AC_SUBST([INET_SYMVER])dnl
    AC_SUBST([INET_MANPATH])dnl
    AC_SUBST([INET_VERSION])dnl
])# _INET
# =============================================================================

# =============================================================================
# _INET_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_OPTIONS], [dnl
    AC_ARG_WITH([inet],
		AS_HELP_STRING([--with-inet=HEADERS],
			       [specify the INET header file directory.
				@<:@default=$INCLUDEDIR/strinet@:>@]),
		[with_inet="$withval"],
		[with_inet=''])
])# _INET_OPTIONS
# =============================================================================

# =============================================================================
# _INET_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_SETUP], [dnl
    _INET_CHECK_HEADERS
    for inet_include in $inet_cv_includes ; do
	INET_CPPFLAGS="${INET_CPPFLAGS}${INET_CPPFLAGS:+ }-I${inet_include}"
    done
    if test :"${inet_cv_config:-no}" != :no ; then
	INET_CPPFLAGS="${INET_CPPFLAGS}${INET_CPPFLAGS:+ }-include ${inet_cv_config}"
    fi
    if test :"${inet_cv_modversions:-no}" != :no ; then
	INET_MODFLAGS="${INET_MODFLAGS}${INET_MODFLAGS:+ }-include ${inet_cv_modversions}"
    fi
])# _INET_SETUP
# =============================================================================

# =============================================================================
# _INET_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS INET header files.  The
    # package normally requires INET header files to compile.
    AC_CACHE_CHECK([for inet include directory], [inet_cv_includes], [dnl
	inet_what="sys/xti_inet.h"
	if test :"${with_inet:-no}" != :no -a :"${with_inet:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_inet)])
	    for inet_dir in $with_inet ; do
		if test -d "$inet_dir" ; then
		    AC_MSG_CHECKING([for inet include directory... $inet_dir])
		    if test -r "$inet_dir/$inet_what" ; then
			inet_cv_includes="$with_inet"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${inet_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-inet="$with_inet"
***
*** however, $inet_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for inet include directory])
	fi
	if test :"${inet_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    inet_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strinet/src/includes}"
	    inet_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/strinet/src/includes}"
	    if test -d "$inet_dir" ; then
		AC_MSG_CHECKING([for inet include directory... $inet_dir $inet_bld])
		if test -d "$inet_bld" -a -r "$inet_dir/$inet_what" ; then
		    inet_cv_includes="$inet_dir $inet_bld"
		    #inet_cv_ldadd= # "$os7_cv_master_builddir/strinet/libinet.la"
		    #inet_cv_ldadd32= # "$os7_cv_master_builddir/strinet/lib32/libinet.la"
		    #inet_cv_modmap="$os7_cv_master_builddir/strinet/Modules.map"
		    #inet_cv_symver="$os7_cv_master_builddir/strinet/Module.symvers"
		    #inet_cv_manpath="$os7_cv_master_builddir/strinet/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for inet include directory])
	fi
	if test :"${inet_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    inet_here=`pwd`
	    AC_MSG_RESULT([(searching from $inet_here)])
	    for inet_dir in \
		$srcdir/strinet*/src/include \
		$srcdir/../strinet*/src/include \
		../_build/$srcdir/../../strinet*/src/include \
		../_build/$srcdir/../../../strinet*/src/include
	    do
		if test -d "$inet_dir" ; then
		    inet_bld=`echo $inet_dir | sed -e "s|^$srcdir/|$inet_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    inet_dir=`(cd $inet_dir; pwd)`
		    AC_MSG_CHECKING([for inet include directory... $inet_dir $inet_bld])
		    if test -d "$inet_bld" -a -r "$inet_dir/$inet_what" ; then
			inet_cv_includes="$inet_dir $inet_bld"
			#inet_cv_ldadd= # `echo "$inet_bld/../../libinet.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			#inet_cv_ldadd32= # `echo "$inet_bld/../../lib32/libinet.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			#inet_cv_modmap=`echo "$inet_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			#inet_cv_symver=`echo "$inet_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			#inet_cv_manpath=`echo "$inet_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for inet include directory])
	fi
	if test :"${inet_cv_includes:-no}" = :no ; then
	    # INET header files are normally found in the strinet package now.
	    # They used to be part of the INET add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "inet_search_path=\"
			${DESTDIR}${includedir}/strinet
			${DESTDIR}${rootdir}${oldincludedir}/strinet
			${DESTDIR}${rootdir}/usr/include/strinet
			${DESTDIR}${rootdir}/usr/local/include/strinet
			${DESTDIR}${rootdir}/usr/src/strinet/src/include
			${DESTDIR}${includedir}/strxnet
			${DESTDIR}${rootdir}${oldincludedir}/strxnet
			${DESTDIR}${rootdir}/usr/include/strxnet
			${DESTDIR}${rootdir}/usr/local/include/strxnet
			${DESTDIR}${rootdir}/usr/src/strxnet/src/include
			${DESTDIR}${includedir}/LiS
			${DESTDIR}${rootdir}${oldincludedir}/LiS
			${DESTDIR}${rootdir}/usr/include/LiS
			${DESTDIR}${rootdir}/usr/local/include/LiS
			${DESTDIR}${rootdir}/usr/src/LiS/include
			${DESTDIR}${oldincludedir}/strinet
			${DESTDIR}/usr/include/strinet
			${DESTDIR}/usr/local/include/strinet
			${DESTDIR}/usr/src/strinet/src/include
			${DESTDIR}${oldincludedir}/strxnet
			${DESTDIR}/usr/include/strxnet
			${DESTDIR}/usr/local/include/strxnet
			${DESTDIR}/usr/src/strxnet/src/include
			${DESTDIR}${oldincludedir}/LiS
			${DESTDIR}/usr/include/LiS
			${DESTDIR}/usr/local/include/LiS
			${DESTDIR}/usr/src/LiS/include\""
		    ;;
		(LfS)
		    eval "inet_search_path=\"
			${DESTDIR}${includedir}/strinet
			${DESTDIR}${rootdir}${oldincludedir}/strinet
			${DESTDIR}${rootdir}/usr/include/strinet
			${DESTDIR}${rootdir}/usr/local/include/strinet
			${DESTDIR}${rootdir}/usr/src/strinet/src/include
			${DESTDIR}${includedir}/strxnet
			${DESTDIR}${rootdir}${oldincludedir}/strxnet
			${DESTDIR}${rootdir}/usr/include/strxnet
			${DESTDIR}${rootdir}/usr/local/include/strxnet
			${DESTDIR}${rootdir}/usr/src/strxnet/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strinet
			${DESTDIR}/usr/include/strinet
			${DESTDIR}/usr/local/include/strinet
			${DESTDIR}/usr/src/strinet/src/include
			${DESTDIR}${oldincludedir}/strxnet
			${DESTDIR}/usr/include/strxnet
			${DESTDIR}/usr/local/include/strxnet
			${DESTDIR}/usr/src/strxnet/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    inet_search_path=`echo "$inet_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    inet_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for inet_dir in $inet_search_path ; do
		if test -d "$inet_dir" ; then
		    AC_MSG_CHECKING([for inet include directory... $inet_dir])
		    if test -r "$inet_dir/$inet_what" ; then
			inet_cv_includes="$inet_dir"
			#inet_cv_ldadd=
			#inet_cv_ldadd32=
			#inet_cv_modmap=
			#inet_cv_symver=
			#inet_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for inet include directory])
	fi
    ])
    AC_CACHE_CHECK([for inet ldadd native], [inet_cv_ldadd], [dnl
	inet_cv_ldadd=
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../libinet.la" ; then
		inet_cv_ldadd=`echo "$inet_dir/../../libinet.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for inet ldflags], [inet_cv_ldflags], [dnl
	inet_cv_ldflags=
	if test -z "$inet_cv_ldadd" ; then
	    inet_cv_ldflags= # '-linet'
	else
	    inet_cv_ldflags= # "-L$(dirname $inet_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for inet ldadd 32-bit], [inet_cv_ldadd32], [dnl
	inet_cv_ldadd32=
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../lib32/libinet.la" ; then
		inet_cv_ldadd32=`echo "$inet_dir/../../lib32/libinet.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for inet ldflags 32-bit], [inet_cv_ldflags32], [dnl
	inet_cv_ldflags32=
	if test -z "$inet_cv_ldadd32" ; then
	    inet_cv_ldflags32= # '-linet'
	else
	    inet_cv_ldflags32= # "-L$(dirname $inet_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for inet modmap], [inet_cv_modmap], [dnl
	inet_cv_modmap=
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../Modules.map" ; then
		inet_cv_modmap=`echo "$inet_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for inet symver], [inet_cv_symver], [dnl
	inet_cv_symver=
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../Module.symvers" ; then
		inet_cv_symver=`echo "$inet_dir/../../Modules.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for inet manpath], [inet_cv_manpath], [dnl
	inet_cv_manpath=
	for inet_dir in $inet_cv_includes ; do
	    if test -d "$inet_dir/../../doc/man" ; then
		inet_cv_manpath=`echo "$inet_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${inet_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS INET include directories.  If
*** you wish to use the STREAMS INET package, you will need to specify
*** the location of the STREAMS INET (strinet) include directories with
*** the --with-inet=@<:@DIRECTORY@<:@ DIRECOTYR@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS INET package?  The
*** STREAMS strinet package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strinet-0.9.2.3.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for inet version], [inet_cv_version], [dnl
	inet_what="sys/strinet/version.h"
	inet_file=
	if test -n "$inet_cv_includes" ; then
	    for inet_dir in $inet_cv_includes ; do
		# old place for version
		if test -f "$inet_dir/$inet_what" ; then
		    inet_file="$inet_dir/$inet_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$inet_dir/$linux_cv_k_release/$target_cpu/$inet_what" ; then
			inet_file="$inet_dir/$linux_cv_k_release/$target_cpu/$inet_what"
			break
		    fi
		fi
	    done
	fi
	if test :${inet_file:-no} != :no ; then
	    inet_cv_version=`grep '#define.*\<STRINET_VERSION\>' $inet_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    inet_what="sys/config.h"
    AC_CACHE_CHECK([for inet $inet_what], [inet_cv_config], [dnl
	inet_cv_config=
	if test -n "$inet_cv_includes" ; then
	    for inet_dir in $inet_cv_includes ; do
		# old place for config
		if test -f "$inet_dir/$inet_what" ; then
		    inet_cv_config="$inet_dir/$inet-what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$inet_dir/$linux_cv_k_release/$target_cpu/$inet_what" ; then
			inet_cv_config="$inet_dir/$linux_cv_k_release/$target_cpu/$inet_what"
			break
		    fi
		fi
	    done
	fi
    ])
    inet_what="sys/strinet/modversions.h"
    AC_CACHE_CHECK([for inet $inet_what], [inet_cv_modversions], [dnl
	inet_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$inet_cv_includes" ; then
		for inet_dir in $inet_cv_includes ; do
		    # old place for modversions
		    if test -f "$inet_dir/$inet_what" ; then
			inet_cv_modversions="$inet_dir/$inet_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			if test "$inet_dir/$linux_cv_k_release/$target_cpu/$inet_what" ; then
			    inet_cv_includes="$inet_dir/$linux_cv_k_release/$target_cpu $inet_cv_includes"
			    inet_cv_modversions="$inet_dir/$linux_cv_k_release/$target_cpu/$inet_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_MSG_CHECKING([for inet added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_inet" ; then
	if test :"${inet_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_inet --with-inet\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-inet'"
	    AC_MSG_RESULT([--with-inet])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_inet --without-inet\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-inet'"
	    AC_MSG_RESULT([--without-inet])
	fi
    else
	AC_MSG_RESULT([--with-inet="$with_inet"])
    fi
])# _INET_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _INET_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_KERNEL], [dnl
])# _INET_KERNEL
# =============================================================================

# =============================================================================
# _INET_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_OUTPUT], [dnl
    _INET_DEFINES
])# _INET_OUTPUT
# =============================================================================

# =============================================================================
# _INET_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_DEFINES], [dnl
    if test :"${inet_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_INET_MODVERSIONS_H], [1], [Define when
	    the STREAMS INET release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    INET_CPPFLAGS="${INET_CPPFLAGS:+ ${INET_CPPFLAGS}}"
    INET_LDADD="$inet_cv_ldadd"
    INET_LDADD32="$inet_cv_ldadd32"
    INET_LDFLAGS="$inet_cv_ldflags"
    INET_LDFLAGS32="$inet_cv_ldflags32"
    INET_MODMAP="$inet_cv_modmap"
    INET_SYMVER="$inet_cv_symver"
    INET_MANPATH="$inet_cv_manpath"
    INET_VERSION="$inet_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${INET_SYMVER:+${MODPOST_INPUTS:+ }${INET_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strinet for
	that matter.
    ])
])# _INET_DEFINES
# =============================================================================

# =============================================================================
# _INET_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_USER], [dnl
])# _INET_USER
# =============================================================================

# =============================================================================
# _INET_
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_], [dnl
])# _INET_
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
