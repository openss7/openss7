# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: inet.m4,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2005/07/04 19:57:39 $
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
# Last Modified $Date: 2005/07/04 19:57:39 $ by $Author: brian $
#
# =============================================================================


# =============================================================================
# _INET
# -----------------------------------------------------------------------------
# Check for the existence of INET header files, particularly sys/xti_inet.h.
# INET headers files are required for building the XTI interface for SCTP.
# Without INET header files, the INET interface to SCTP will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_INET], [dnl
    AC_REQUIRE([_XTI])dnl
    _INET_OPTIONS
    _INET_SETUP
    AC_SUBST([INET_CPPFLAGS])
])# _INET
# =============================================================================

# =============================================================================
# 
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_OPTIONS], [dnl
    AC_ARG_WITH([inet],
	AC_HELP_STRING([--with-inet=HEADERS],
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
    _INET_DEFINES
])# _INET_SETUP
# =============================================================================

# =============================================================================
# _INET_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS INET header files.  The package
    # normally requires INET header files to compile.
    AC_CACHE_CHECK([for inet include directory], [inet_cv_includes], [dnl
	if test ":${with_inet:-no}" != :no -a :"${with_inet:-no}" != :yes ;  then
	    inet_cv_includes="$with_inet"
	fi
	inet_what="sys/xti_inet.h"
	if test ":${inet_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    inet_here=`pwd`
	    for inet_dir in $srcdir/strinet*/src/include $srcdir/../strinet*/src/include
	    do
		if test -d $inet_dir -a -r $inet_dir/$inet_what
		then
		    inet_bld=`echo $inet_dir | sed -e "s|^$srcdir/|$inet_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    inet_dir=`(cd $inet_dir; pwd)`
		    inet_cv_includes="$inet_dir $inet_bld"
		    break
		fi
	    done
	fi
	if test ":${inet_cv_includes:-no}" = :no ; then
	    eval "inet_search_path=\"
		${DESTDIR}${includedir}/strinet
		${DESTDIR}${rootdir}${oldincludedir}/strinet
		${DESTDIR}${rootdir}/usr/include/strinet
		${DESTDIR}${rootdir}/usr/local/include/strinet
		${DESTDIR}${rootdir}/usr/src/strinet/src/include
		${DESTDIR}${oldincludedir}/strinet
		${DESTDIR}/usr/include/strinet
		${DESTDIR}/usr/local/include/strinet
		${DESTDIR}/usr/src/strinet/src/include
		${DESTDIR}${includedir}/strxnet
		${DESTDIR}${rootdir}${oldincludedir}/strxnet
		${DESTDIR}${rootdir}/usr/include/strxnet
		${DESTDIR}${rootdir}/usr/local/include/strxnet
		${DESTDIR}${rootdir}/usr/src/strxnet/src/include
		${DESTDIR}${oldincludedir}/strxnet
		${DESTDIR}/usr/include/strxnet
		${DESTDIR}/usr/local/include/strxnet
		${DESTDIR}/usr/src/strxnet/src/include
		${DESTDIR}${includedir}/streams
		${DESTDIR}${rootdir}${oldincludedir}/streams
		${DESTDIR}${rootdir}/usr/include/streams
		${DESTDIR}${rootdir}/usr/local/include/streams
		${DESTDIR}${rootdir}/usr/src/streams/src/include
		${DESTDIR}${oldincludedir}/streams
		${DESTDIR}/usr/include/streams
		${DESTDIR}/usr/local/include/streams
		${DESTDIR}/usr/src/streams/include
		${DESTDIR}${includedir}/LiS
		${DESTDIR}${rootdir}${oldincludedir}/LiS
		${DESTDIR}${rootdir}/usr/include/LiS
		${DESTDIR}${rootdir}/usr/local/include/LiS
		${DESTDIR}${rootdir}/usr/src/LiS/include
		${DESTDIR}${oldincludedir}/LiS
		${DESTDIR}/usr/include/LiS
		${DESTDIR}/usr/local/include/LiS
		${DESTDIR}/usr/src/LiS/include\""
	    inet_search_path=`echo "$inet_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    inet_cv_includes=
	    for inet_dir in $inet_search_path ; do
		if test -d "$inet_dir" -a -r "$inet_dir/$inet_what" ; then
		    inet_cv_includes="$inet_dir"
		    break
		fi
	    done
	fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${inet_cv_includes:-no}" = :no ; then :
	if test :"$with_inet" = :no ; then
	    AC_MSG_ERROR([
***
*** Could not find INET include directories.  This package requires the
*** presence of INET include directories to compile.  Specify the location of
*** INET include directories with option --with-inet to configure and try again.
*** ])
	fi
	if test -z "$with_inet" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_inet --with-inet\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-inet'"
	fi
    else
	if test -z "$with_inet" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_inet --without-inet\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-inet'"
	fi
    fi
])# _INET_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _INET_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_DEFINES], [dnl
    for inet_include in $inet_cv_includes ; do
	INET_CPPFLAGS="${INET_CPPFLAGS}${INET_CPPFLAGS:+ }-I${inet_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strinet for
	that matter.
    ])
])# _INET_DEFINES
# =============================================================================

# =============================================================================
# _INET_
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_], [dnl
])# _INET_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
