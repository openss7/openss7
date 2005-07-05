# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: xns.m4,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2005/07/05 04:42:32 $
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
# Last Modified $Date: 2005/07/05 04:42:32 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _XNS
# -----------------------------------------------------------------------------
# Check for the existence of XNS header files, particularly sys/npi.h.  NPI
# headers files are required for building the NPI interface for SCTP.  Without
# NPI header files, the NPI interface to SCTP will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_XNS], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _XNS_OPTIONS
    _XNS_SETUP
    AC_SUBST([XNS_CPPFLAGS])
])# _XNS
# =============================================================================

# =============================================================================
# _XNS_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_XNS_OPTIONS], [dnl
    AC_ARG_WITH([xns],
		AC_HELP_STRING([--with-xns=HEADERS],
			       [specify the XNS header file directory.
				@<:@default=$INCLUDEDIR/strxns@:>@]),
		[with_xns="$withval"],
		[with_xns=''])
])# _XNS_OPTIONS
# =============================================================================

# =============================================================================
# _XNS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XNS_SETUP], [dnl
    _XNS_CHECK_HEADERS
    _XNS_DEFINES
])# _XNS_SETUP
# =============================================================================

# =============================================================================
# _XNS_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_XNS_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS XNS header files.  The package
    # normally requires XNS header files to compile.
    AC_CACHE_CHECK([for xns include directory], [xns_cv_includes], [dnl
	if test ":${with_xns:-no}" != :no -a :"${with_xns:-no}" != :yes ;  then
	    # First thing to do is to take user specified director(ies)
	    xns_cv_includes="$with_xns"
	fi
	xns_what="sys/npi.h"
	if test ":${xns_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    xns_here=`pwd`
	    for xns_dir in \
		$srcdir/strxns*/src/include \
		$srcdir/../strxns*/src/include \
		../_build/$srcdir/../../strxns*/src/include \
		../_build/$srcdir/../../../strxns*/src/include
	    do
		if test -d $xns_dir -a -r $xns_dir/$xns_what
		then
		    xns_bld=`echo $xns_dir | sed -e "s|^$srcdir/|$xns_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    xns_dir=`(cd $xns_dir; pwd)`
		    xns_cv_includes="$xns_dir $xns_bld"
		    break
		fi
	    done
	fi
	if test ":${xns_cv_includes:-no}" = :no ; then
	    # XNS header files are normally found in the strxns package now.
	    # They used to be part of the strxnet add-on package and even older
	    # versions are part of the LiS or LfS release packages.
	    eval "xns_search_path=\"
		${DESTDIR}${includedir}/strxns
		${DESTDIR}${rootdir}${oldincludedir}/strxns
		${DESTDIR}${rootdir}/usr/include/strxns
		${DESTDIR}${rootdir}/usr/local/include/strxns
		${DESTDIR}${rootdir}/usr/src/strxns/src/include
		${DESTDIR}${includedir}/strxnet
		${DESTDIR}${rootdir}${oldincludedir}/strxnet
		${DESTDIR}${rootdir}/usr/include/strxnet
		${DESTDIR}${rootdir}/usr/local/include/strxnet
		${DESTDIR}${rootdir}/usr/src/strxnet/src/include
		${DESTDIR}${oldincludedir}/strxns
		${DESTDIR}/usr/include/strxns
		${DESTDIR}/usr/local/include/strxns
		${DESTDIR}/usr/src/strxns/src/include
		${DESTDIR}${oldincludedir}/strxnet
		${DESTDIR}/usr/include/strxnet
		${DESTDIR}/usr/local/include/strxnet
		${DESTDIR}/usr/src/strxnet/src/include\""
	    case "$streams_cv_package" in
		LiS)
		    # XNS header files used to be part of the LiS package.
		    eval "xns_search_path=\"$xns_search_path
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
		    # XNS header files used to be part of the LfS package.
		    eval "xns_search_path=\"$xns_search_path
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
	    xns_search_path=`echo "$xns_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    for xns_dir in $xns_search_path ; do
		if test -d "$xns_dir" -a -r "$xns_dir/$xns_what" ; then
		    xns_cv_includes="$xns_dir"
		    break
		fi
	    done
	fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${xns_cv_includes:-no}" = :no ; then :
	if test :"$with_xns" = :no ; then
	    AC_MSG_ERROR([
***
*** Could not find XNS include directories.  This package requires the
*** presence of XNS include directories to compile.  Specify the location of
*** XNS include directories with option --with-xns to configure and try again.
*** ])
	fi
	if test -z "$with_xns" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_xns --with-xns\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-xns'"
	fi
    else
	if test -z "$with_xns" ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_xns --without-xns\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-xns'"
	fi
    fi
])# _XNS_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _XNS_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_XNS_DEFINES], [dnl
    for xns_include in $xns_cv_includes ; do
	XNS_CPPFLAGS="${XNS_CPPFLAGS}${XNS_CPPFLAGS:+ }-I${xns_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strxns for
	that matter.
    ])
])# _XNS_DEFINES
# =============================================================================

# =============================================================================
# _XNS_
# -----------------------------------------------------------------------------
AC_DEFUN([_XNS_], [dnl
])# _XNS_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
