# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: sctp.m4,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/03/14 09:20:46 $
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
# Last Modified $Date: 2006/03/14 09:20:46 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: sctp.m4,v $
# Revision 0.9.2.24  2006/03/14 09:20:46  brian
# - typo
#
# Revision 0.9.2.23  2006/03/14 09:04:10  brian
# - syntax consistency, advanced search
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS SCTP
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS SCTP package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strsctp loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _SCTP
# -----------------------------------------------------------------------------
# Check for the existence of SCTP header files, particularly sys/xti_sctp.h.
# SCTP headers files are required for building the XTI interface for SCTP.
# Without SCTP header files, the SCTP interface to SCTP will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP], [dnl
    AC_REQUIRE([_XTI])dnl
    _SCTP_OPTIONS
    _SCTP_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_SCTP_KERNEL])
    _SCTP_USER
    _SCTP_OUTPUT
    AC_SUBST([SCTP_CPPFLAGS])dnl
    AC_SUBST([SCTP_MODFLAGS])dnl
    AC_SUBST([SCTP_LDADD])dnl
    AC_SUBST([SCTP_LDADD32])dnl
    AC_SUBST([SCTP_MODMAP])dnl
    AC_SUBST([SCTP_SYMVER])dnl
    AC_SUBST([SCTP_MANPATH])dnl
    AC_SUBST([SCTP_VERSION])dnl
])# _SCTP
# =============================================================================

# =============================================================================
# _SCTP_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPTIONS], [dnl
    AC_ARG_WITH([sctp],
		AS_HELP_STRING([--with-sctp=HEADERS],
			       [specify the SCTP header file directory.
				@<:@default=$INCLUDEDIR/strsctp@:>@]),
		[with_sctp="$withval"],
		[with_sctp=''])
])# _SCTP_OPTIONS
# =============================================================================

# =============================================================================
# _SCTP_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP], [dnl
    _SCTP_CHECK_HEADERS
    for sctp_include in $sctp_cv_includes ; do
	SCTP_CPPFLAGS="${SCTP_CPPFLAGS}${SCTP_CPPFLAGS:+ }-I${sctp_include}"
    done
    if test :"${sctp_cv_config:-no}" != :no ; then
	SCTP_CPPFLAGS="${SCTP_CPPFLAGS}${SCTP_CPPFLAGS:+ }-include ${sctp_cv_config}"
    fi
    if test :"${sctp_cv_modversions:-no}" != :no ; then
	SCTP_MODFLAGS="${SCTP_MODFLAGS}${SCTP_MODFLAGS:+ }-include ${sctp_cv_modversions}"
    fi
])# _SCTP_SETUP
# =============================================================================

# =============================================================================
# _SCTP_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS SCTP header files.  The
    # package normally requires SCTP header files to compile.
    AC_CACHE_CHECK([for sctp include directory], [sctp_cv_includes], [dnl
	sctp_what="sys/xti_sctp.h"
	if test :"${with_sctp:-no}" != :no -a :"${with_sctp:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_sctp)])
	    for sctp_dir in $with_sctp ; do
		if test -d "$sctp_dir" ; then
		    AC_MSG_CHECKING([for sctp include directory... $sctp_dir])
		    if test -r "$sctp_dir/$sctp_what" ; then
			sctp_cv_includes="$with_sctp"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${sctp_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-sctp="$with_sctp"
***
*** however, $sctp_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for sctp include directory])
	fi
	if test :"${sctp_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $master_srcdir $master_builddir)])
	    sctp_search_path="
		${master_srcdir:+$master_srcdir/strsctp/src/include}
		${master_builddir:+$master_builddir/strsctp/src/include}"
	    for sctp_dir in $sctp_search_path ; do
		if test -d "$sctp_dir" ; then
		    AC_MSG_CHECKING([for sctp include directory... $sctp_dir])
		    if test -r "$sctp_dir/$sctp_what" ; then
			sctp_cv_includes="$sctp_search_path"
			sctp_cv_ldadd= # "$master_builddir/strsctp/libsctp.la"
			sctp_cv_ldadd32= # "$master_builddir/strsctp/lib32/libsctp.la"
			sctp_cv_modmap= # "$master_builddir/strsctp/Modules.map"
			sctp_cv_symver= # "$master_builddir/strsctp/Module.symvers"
			sctp_cv_manpath="$master_builddir/strsctp/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sctp include directory])
	fi
	if test :"${sctp_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    sctp_here=`pwd`
	    AC_MSG_RESULT([(searching from $sctp_here)])
	    for sctp_dir in \
		$srcdir/strsctp*/src/include \
		$srcdir/../strsctp*/src/include \
		../_build/$srcdir/../../strsctp*/src/include \
		../_build/$srcdir/../../../strsctp*/src/include
	    do
		if test -d "$sctp_dir" ; then
		    sctp_bld=`echo $sctp_dir | sed -e "s|^$srcdir/|$sctp_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    sctp_dir=`(cd $sctp_dir; pwd)`
		    AC_MSG_CHECKING([for sctp include directory... $sctp_dir])
		    if test -r "$sctp_dir/$sctp_what" ; then
			sctp_cv_includes="$sctp_dir $sctp_bld"
			sctp_cv_ldadd= # `echo "$sctp_bld/../../libsctp.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sctp_cv_ldadd32= # `echo "$sctp_bld/../../lib32/libsctp.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sctp_cv_modmap= # `echo "$sctp_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sctp_cv_symver= # `echo "$sctp_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sctp_cv_manpath=`echo "$sctp_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sctp include directory])
	fi
	if test :"${sctp_cv_includes:-no}" = :no ; then
	    # INET header files are normally found in the strsctp package now.
	    # They used to be part of the SS7 add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "sctp_search_path=\"
			${DESTDIR}${includedir}/strsctp
			${DESTDIR}${rootdir}${oldincludedir}/strsctp
			${DESTDIR}${rootdir}/usr/include/strsctp
			${DESTDIR}${rootdir}/usr/local/include/strsctp
			${DESTDIR}${rootdir}/usr/src/strsctp/src/include
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
			${DESTDIR}${oldincludedir}/strsctp
			${DESTDIR}/usr/include/strsctp
			${DESTDIR}/usr/local/include/strsctp
			${DESTDIR}/usr/src/strsctp/src/include
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
		    eval "sctp_search_path=\"
			${DESTDIR}${includedir}/strsctp
			${DESTDIR}${rootdir}${oldincludedir}/strsctp
			${DESTDIR}${rootdir}/usr/include/strsctp
			${DESTDIR}${rootdir}/usr/local/include/strsctp
			${DESTDIR}${rootdir}/usr/src/strsctp/src/include
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
			${DESTDIR}${oldincludedir}/strsctp
			${DESTDIR}/usr/include/strsctp
			${DESTDIR}/usr/local/include/strsctp
			${DESTDIR}/usr/src/strsctp/src/include
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
	    sctp_search_path=`echo "$sctp_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    sctp_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for sctp_dir in $sctp_search_path ; do
		if test -d "$sctp_dir" ; then
		    AC_MSG_CHECKING([for sctp include directory... $sctp_dir])
		    if test -r "$sctp_dir/$sctp_what" ; then
			sctp_cv_includes="$sctp_dir"
			sctp_cv_ldadd= # '-lsctp'
			sctp_cv_ldadd32= # '-lsctp'
			sctp_cv_modmap=
			sctp_cv_symver=
			sctp_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sctp include directory])
	fi
    ])
    AC_CACHE_CHECK([for sctp ldadd native],[sctp_cv_ldadd],[dnl
	for sctp_dir in $sctp_cv_includes ; do
	    if test -f "$sctp_dir/../../libsctp.la" ; then
		sctp_cv_ldadd=`echo "$sctp_dir/../../libsctp.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$sctp_cv_ldadd" ; then
	    sctp_cv_ldadd= # '-lsctp'
	fi
    ])
    AC_CACHE_CHECK([for sctp ldadd 32-bit],[sctp_cv_ldadd32],[dnl
	for sctp_dir in $sctp_cv_includes ; do
	    if test -f "$sctp_dir/../../lib32/libsctp.la" ; then
		sctp_cv_ldadd32=`echo "$sctp_dir/../../lib32/libsctp.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$sctp_cv_ldadd32" ; then
	    sctp_cv_ldadd32= # '-lsctp'
	fi
    ])
    AC_CACHE_CHECK([for sctp modmap],[sctp_cv_modmap],[dnl
	for sctp_dir in $sctp_cv_includes ; do
	    if test -f "$sctp_dir/../../Modules.map" ; then
		sctp_cv_modmap=`echo "$sctp_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sctp symver],[sctp_cv_symver],[dnl
	for sctp_dir in $sctp_cv_includes ; do
	    if test -f "$sctp_dir/../../Module.symvers" ; then
		sctp_cv_symver=`echo "$sctp_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sctp manpath],[sctp_cv_manpath],[dnl
	for sctp_dir in $sctp_cv_includes ; do
	    if test -d "$sctp_dir/../../doc/man" ; then
		sctp_cv_manpath=`echo "$sctp_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${sctp_cv_includes:-no}" = :no ; then :
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS SCTP include directories.  If
*** you wish to use the STREAMS SCTP package, you will need to specify
*** the location of the STREAMS SCTP (strsctp) include directories with
*** the --with-sctp=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS SCTP package?  The
*** STREAMS strsctp package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strsctp-0.9.2.5.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for sctp version], [sctp_cv_version], [dnl
	sctp_what="sys/strsctp/version.h"
	sctp_file=
	if test -n "$sctp_cv_includes" ; then
	    for sctp_dir in $sctp_cv_includes ; do
		# old place for version
		if test -f "$sctp_dir/$sctp_what" ; then
		    sctp_file="$sctp_dir/$sctp_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$sctp_dir/$linux_cv_k_release/$target_cpu/$sctp_what" ; then
			sctp_file="$sctp_dir/$linux_cv_k_release/$target_cpu/$sctp_what"
			break
		    fi
		fi
	    done
	fi
	if test :${sctp_file:-no} != :no ; then
	    sctp_cv_version=`grep '#define.*\<STRSCTP_VERSION\>' $sctp_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    sctp_what="sys/config.h"
    AC_CACHE_CHECK([for sctp $sctp_what], [sctp_cv_config], [dnl
	sctp_cv_config=
	if test -n "$sctp_cv_includes" ; then
	    for sctp_dir in $sctp_cv_includes ; do
		# old place for config
		if test -f "$sctp_dir/$sctp_what" ; then
		    sctp_cv_config="$sctp_dir/$sctp-what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$sctp_dir/$linux_cv_k_release/$target_cpu/$sctp_what" ; then
			sctp_cv_config="$sctp_dir/$linux_cv_k_release/$target_cpu/$sctp_what"
			break
		    fi
		fi
	    done
	fi
    ])
    sctp_what="sys/strsctp/modversions.h"
    AC_CACHE_CHECK([for sctp $sctp_what], [sctp_cv_modversions], [dnl
	sctp_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$sctp_cv_includes" ; then
		for sctp_dir in $sctp_cv_includes ; do
		    # old place for modversions
		    if test -f "$sctp_dir/$sctp_what" ; then
			sctp_cv_modversions="$sctp_dir/$sctp_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			if test "$sctp_dir/$linux_cv_k_release/$target_cpu/$sctp_what" ; then
			    sctp_cv_includes="$sctp_dir/$linux_cv_k_release/$target_cpu $sctp_cv_includes"
			    sctp_cv_modversions="$sctp_dir/$linux_cv_k_release/$target_cpu/$sctp_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_MSG_CHECKING([for sctp added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_sctp" ; then
	if test :"${sctp_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_sctp --with-sctp\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-sctp'"
	    AC_MSG_RESULT([--with-sctp])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_sctp --without-sctp\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-sctp'"
	    AC_MSG_RESULT([--without-sctp])
	fi
    else
	AC_MSG_RESULT([--with-sctp="$with_sctp"])
    fi
])# _SCTP_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _SCTP_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_KERNEL], [dnl
])# _SCTP_KERNEL
# =============================================================================

# =============================================================================
# _SCTP_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OUTPUT], [dnl
    _SCTP_DEFINES
])# _SCTP_OUTPUT
# =============================================================================

# =============================================================================
# _SCTP_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_DEFINES], [dnl
    if test :"${sctp_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_SCTP_MODVERSIONS_H], [1], [Define when
	    the STREAMS SCTP release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    SCTP_CPPFLAGS="${SCTP_CPPFLAGS:+ ${SCTP_CPPFLAGS}}"
    SCTP_LDADD="$sctp_cv_ldadd"
    SCTP_LDADD32="$sctp_cv_ldadd32"
    SCTP_MODMAP="$sctp_cv_modmap"
    SCTP_SYMVER="$sctp_cv_symver"
    SCTP_MANPATH="$sctp_cv_manpath"
    SCTP_VERSION="$sctp_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${SCTP_SYMVER:+${MODPOST_INPUTS:+ }${SCTP_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strsctp for
	that matter.
    ])
])# _SCTP_DEFINES
# =============================================================================

# =============================================================================
# _SCTP_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_USER], [dnl
    AC_CACHE_CHECK([for sctp openss7 kernel], [sctp_cv_openss7], [dnl
	AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp_headers\>], [
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#ifdef SCTP_DISPOSITION_UNSENT
	yes_we_have_openss7_kernel_sctp_headers
#endif
	], [sctp_cv_openss7=yes], [sctp_cv_openss7=no])
    ])
])# _SCTP_USER
# =============================================================================

# =============================================================================
# _SCTP_
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_], [dnl
])# _SCTP_
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
