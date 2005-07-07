# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: xti.m4,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/07/07 04:12:55 $
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
# Last Modified $Date: 2005/07/07 04:12:55 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _XTI
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _XTI_OPTIONS
    _XTI_SETUP
    AC_SUBST([XTI_CPPFLAGS])
    AC_SUBST([XTI_LDADD])
    AC_SUBST([XTI_MANPATH])
])# _XTI
# =============================================================================

# =============================================================================
# _XTI_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_OPTIONS], [dnl
    AC_ARG_WITH([xti],
		AC_HELP_STRING([--with-xti=HEADERS],
			       [specify the XTI header file directory.
			       @<:@default=$INCLUDEDIR/xti@:>@]),
		[with_xti="$withval"],
		[with_xti=''])
])# _XTI_OPTIONS
# =============================================================================

# =============================================================================
# _XTI_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_SETUP], [dnl
    _XTI_CHECK_HEADERS
    _XTI_DEFINES
])# _XTI_SETUP
# =============================================================================

# =============================================================================
# _XTI_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS XTI header files.  The package
    # normally requires either Linux STREAMS or Linux Fast-STREAMS XTI header
    # files (or both) to compile.
    AC_CACHE_CHECK([for xti include directory], [xti_cv_includes], [dnl
	if test :"${with_xti:-no}" != :no -a :"${with_xti:-no}" != :yes 
	then
	    xti_cv_includes="$with_xti"
	fi
	xti_what="xti.h"
	if test ":${xti_cv_includes:-no}" = :no 
	then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    xti_here=`pwd`
	    for xti_dir in \
		$srcdir/strxnet*/src/include \
		$srcdir/../strxnet*/src/include \
		../_build/$srcdir/../../strxnet*/src/include \
		../_build/$srcdir/../../../strxnet*/src/include
	    do
		if test -d $xti_dir -a -r $xti_dir/$xti_what 
		then
		    xti_bld=`echo $xti_dir | sed -e "s|^$srcdir/|$xti_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    xti_dir=`(cd $xti_dir; pwd)`
		    xti_cv_includes="$xti_dir $xti_bld"
		    xti_cv_ldadd=`echo "$xti_bld/../../libxnet.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    xti_cv_manpath=`echo "$xti_bld/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g;'`
		    break
		fi
	    done
	fi
	if test ":${xti_cv_includes:-no}" = :no 
	then
	    case "$streams_cv_package" in
		LiS)
		    # Some of our oldest RPM releases of LiS put the xti header files into their own
		    # subdirectory (/usr/include/xti/).  The old version places them in with the LiS
		    # header files.  The current version has them separate as part of the strxnet
		    # package.  This tests whether we need an additional -I/usr/include/xti in the
		    # streams includes line.  This check can be dropped when the older RPM releases
		    # of LiS fall out of favor.
		    eval "xti_search_path=\"
			${DESTDIR}${includedir}/strxnet
			${DESTDIR}${rootdir}${oldincludedir}/strxnet
			${DESTDIR}${rootdir}/usr/include/strxnet
			${DESTDIR}${rootdir}/usr/local/include/strxnet
			${DESTDIR}${rootdir}/usr/src/strxnet/src/include
			${DESTDIR}${includedir}/LiS/xti
			${DESTDIR}${rootdir}${oldincludedir}/LiS/xti
			${DESTDIR}${rootdir}/usr/include/LiS/xti
			${DESTDIR}${rootdir}/usr/local/include/LiS/xti
			${DESTDIR}${rootdir}/usr/src/LiS/include/xti
			${DESTDIR}${includedir}/xti
			${DESTDIR}${rootdir}${oldincludedir}/xti
			${DESTDIR}${rootdir}/usr/include/xti
			${DESTDIR}${rootdir}/usr/local/include/xti
			${DESTDIR}${oldincludedir}/strxnet
			${DESTDIR}/usr/include/strxnet
			${DESTDIR}/usr/local/include/strxnet
			${DESTDIR}/usr/src/strxnet/src/include
			${DESTDIR}${oldincludedir}/LiS/xti
			${DESTDIR}/usr/include/LiS/xti
			${DESTDIR}/usr/local/include/LiS/xti
			${DESTDIR}/usr/src/LiS/include/xti
			${DESTDIR}${oldincludedir}/xti
			${DESTDIR}/usr/include/xti
			${DESTDIR}/usr/local/include/xti\""
		    ;;
		LfS)
		    # LfS has always been separate.
		    eval "xti_search_path=\"
			${DESTDIR}${includedir}/strxnet
			${DESTDIR}${rootdir}${oldincludedir}/strxnet
			${DESTDIR}${rootdir}/usr/include/strxnet
			${DESTDIR}${rootdir}/usr/local/include/strxnet
			${DESTDIR}${rootdir}/usr/src/strxnet/src/include
			${DESTDIR}${oldincludedir}/strxnet
			${DESTDIR}/usr/include/strxnet
			${DESTDIR}/usr/local/include/strxnet
			${DESTDIR}/usr/src/strxnet/src/include\""
		    ;;
	    esac
	    xti_search_path=`echo "$xti_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    for xti_dir in $xti_search_path 
	    do
		if test -d "$xti_dir" -a -r "$xti_dir/$xti_what" 
		then
		    xti_cv_includes="$xti_dir"
		    xti_cv_ldadd="-lxnet"
		    xti_cv_manpath=
		    break
		fi
	    done
	fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${xti_cv_includes:-no}" = :no 
    then
	if test :"$with_xti" = :no ; then
	    AC_MSG_ERROR([
*** 
*** Could not find XTI include directories.  This package requires the
*** presence of XTI include directories to compile.  Specify the location of
*** XTI include directories with option --with-xti to configure and try again.
*** ])
	fi
	if test -z "$with_xti" 
	then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_xti --with-xti\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-xti'"
	fi
    else
	if test -z "$with_xti" 
	then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_xti --without-xti\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-xti'"
	fi
    fi
])# _XTI_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _XTI_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_DEFINES], [dnl
    for xti_include in $xti_cv_includes 
    do
	XTI_CPPFLAGS="${XTI_CPPFLAGS}${XTI_CPPFLAGS:+ }-I${xti_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strxnet for
	that matter.
    ])
    XTI_LDADD="$xti_cv_ldadd"
    XTI_MANPATH="$xti_cv_manpath"
])# _XTI_DEFINES
# =============================================================================

# =============================================================================
# _XTI_
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_], [dnl
])# _XTI_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
