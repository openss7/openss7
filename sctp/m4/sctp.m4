# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: sctp.m4,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2005/07/08 12:03:21 $
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
# Last Modified $Date: 2005/07/08 12:03:21 $ by $Author: brian $
#
# =============================================================================

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
    _SCTP_OPENSS7
    AC_SUBST([SCTP_CPPFLAGS])
dnl AC_SUSBT([SCTP_LDADD])
    AC_SUBST([SCTP_MANPATH])
    AC_SUBST([SCTP_VERSION])
])# _SCTP
# =============================================================================

# =============================================================================
# _SCTP_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPTIONS], [dnl
    AC_ARG_WITH([sctp],
		AC_HELP_STRING([--with-sctp=HEADERS],
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
    _SCTP_DEFINES
])# _SCTP_SETUP
# =============================================================================

# =============================================================================
# _SCTP_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS SCTP header files.  The package
    # normally requires either Linux STREAMS or Linux Fast-STREAMS SCTP header
    # files (or both) to compile.
    AC_CACHE_CHECK([for sctp include directory], [sctp_cv_includes], [dnl
	if test ":${with_sctp:-no}" != :no -a :"${with_sctp:-no}" != :yes ;  then
	    # First thing to do is to take user specified director(ies)
	    sctp_cv_includes="$with_sctp"
	fi
	sctp_what="sys/xti_sctp.h"
	if test ":${sctp_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    sctp_here=`pwd`
	    for sctp_dir in \
		$srcdir/strsctp*/src/include \
		$srcdir/../strsctp*/src/include \
		../_build/$srcdir/../../strsctp*/src/include \
		../_build/$srcdir/../../../strsctp*/src/include
	    do
		if test -d $sctp_dir -a -r $sctp_dir/$sctp_what ; then
		    sctp_bld=`echo $sctp_dir | sed -e "s|^$srcdir/|$sctp_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    sctp_dir=`(cd $sctp_dir; pwd)`
		    sctp_cv_includes="$sctp_dir $sctp_bld"
dnl		    sctp_cv_manpath=`echo "$sctp_bld/../../libsctp.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    sctp_cv_manpath=`echo "$sctp_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    break
		fi
	    done
	fi
	if test ":${sctp_cv_includes:-no}" = :no ; then
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
		${DESTDIR}${rootdir}/usr/src/streams/src/include
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
		${DESTDIR}${oldincludedir}/streams
		${DESTDIR}/usr/include/streams
		${DESTDIR}/usr/local/include/streams
		${DESTDIR}/usr/src/streams/include
		${DESTDIR}${oldincludedir}/LiS
		${DESTDIR}/usr/include/LiS
		${DESTDIR}/usr/local/include/LiS
		${DESTDIR}/usr/src/LiS/include\""
	    sctp_search_path=`echo "$sctp_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    sctp_cv_includes=
	    for sctp_dir in $sctp_search_path ; do
		if test -d "$sctp_dir" -a -r "$sctp_dir/$sctp_what" ; then
		    sctp_cv_includes="$sctp_dir"
		    sctp_cv_manpath=
		    break
		fi
	    done
	fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${sctp_cv_includes:-no}" = :no ; then :
	if test :"$with_sctp" = :no ; then
	    AC_MSG_ERROR([
***
*** Could not find SCTP include directories.  This package requires the
*** presence of SCTP include directories to compile.  Specify the location of
*** SCTP include directories with option --with-sctp to configure and try again.
*** ])
	fi
	if test -z "$with_sctp" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_sctp --with-sctp\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-sctp'"
	fi
    else
	if test -z "$with_sctp" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_sctp --without-sctp\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-sctp'"
	fi
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
		if test -n $linux_cv_k_release ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
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
])# _SCTP_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _SCTP_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_DEFINES], [dnl
    for sctp_include in $sctp_cv_includes ; do
	SCTP_CPPFLAGS="${SCTP_CPPFLAGS}${SCTP_CPPFLAGS:+ }-I${sctp_include}"
    done
dnl SCTP_LDADD="$sctp_cv_ldadd"
    SCTP_MANPATH="$sctp_cv_manpath"
    SCTP_VERSION="$sctp_cv_version"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strsctp for
	that matter.
    ])
])# _SCTP_DEFINES
# =============================================================================

# =============================================================================
# _SCTP_OPENSS7
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPENSS7], [dnl
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
])# _SCTP_OPENSS7
# =========================================================================

# =========================================================================
# _SCTP_
# -----------------------------------------------------------------------------
AC_DEFUN([_SCTP_], [dnl
])# _SCTP_
# =========================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
