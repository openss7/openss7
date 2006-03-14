# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: xti.m4,v $ $Name:  $($Revision: 0.9.2.32 $) $Date: 2006/03/13 23:21:41 $
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
# Last Modified $Date: 2006/03/13 23:21:41 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _XTI
# -----------------------------------------------------------------------------
# Check for the existence of XTI header files, particularly sys/tpi.h.  TPI
# headers files are required for building the TPI interface for SCTP.  Without
# TPI header files, the TPI interface to SCTP will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _XTI_OPTIONS
    _XTI_SETUP
    AC_SUBST([XTI_CPPFLAGS])dnl
    AC_SUBST([XTI_LDADD])dnl
    AC_SUBST([XTI_MODMAP])dnl
    AC_SUBST([XTI_SYMVER])dnl
    AC_SUBST([XTI_MANPATH])dnl
    AC_SUBST([XTI_VERSION])dnl
])# _XTI
# =============================================================================

# =============================================================================
# _XTI_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_OPTIONS], [dnl
    AC_ARG_WITH([xti],
		AS_HELP_STRING([--with-xti=HEADERS],
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
	xti_what="xti.h"
	if test :"${with_xti:-no}" != :no -a :"${with_xti:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_xti)])
	    for xti_dir in $with_xti ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti include directory... $xti_dir])
		    if test -r "$xns_dir/$xns_what" ; then
			xti_cv_includes="$with_xti"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test ":${xti_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
***
*** You have specified include directories using:
***
***	    --with-xti="$with_xti"
***
*** however, $xti_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
***])
	    fi
	fi
	if test ":${xti_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build directory, if any.
	    AC_MSG_RESULT([(searching $master_srcdir $master_builddir)])
	    xti_search_path="
		${master_srcdir:+$master_srcdir/strxnet/src/include}
		${master_builddir:+$master_builddir/strxnet/src/include}"
	    for xti_dir in $xti_search_path ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti include directory... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_includes="$xti_search_path"
			xti_cv_ldadd="$master_builddir/strxnet/libxnet.la"
			xti_cv_modmap= # "$master_builddir/strxnet/Modules.map"
			xti_cv_symver= # "$master_builddir/strxnet/Module.symvers"
			xti_cv_manpath="$master_builddir/strxnet/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		fi
	    done
	    AC_MSG_CHECKING([for xti include directory])
	fi
	if test ":${xti_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    xti_here=`pwd`
	    AC_MSG_RESULT([(searching from $xti_here)])
	    for xti_dir in \
		$srcdir/strxnet*/src/include \
		$srcdir/../strxnet*/src/include \
		../_build/$srcdir/../../strxnet*/src/include \
		../_build/$srcdir/../../../strxnet*/src/include
	    do
		if test -d "$xti_dir" ; then
		    xti_bld=`echo $xti_dir | sed -e "s|^$srcdir/|$xti_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    xti_dir=`(cd $xti_dir; pwd)`
		    AC_MSG_CHECKING([for xti include directory... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_includes="$xti_dir $xti_bld"
			xti_cv_ldadd=`echo "$xti_bld/../../libxnet.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			xti_cv_modmap=
			xti_cv_symver=
			xti_cv_manpath=`echo "$xti_bld/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti include directory])
	fi
	if test ":${xti_cv_includes:-no}" = :no ; then
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
	    AC_MSG_RESULT([(searching)])
	    for xti_dir in $xti_search_path ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti include directory... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_includes="$xti_dir"
			xti_cv_ldadd="-lxnet"
			xti_cv_modmap=
			xti_cv_symver=
			xti_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti include directory])
	fi
    ])
    AC_CACHE_CHECK([for xti ldadd],[xti_cv_ldadd],[dnl
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../libxnet.la" ; then
		xti_cv_ldadd=`echo "$xti_dir/../../libxnet.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$xti_cv_ldadd" ; then
	    xti_cv_ldadd='-lxnet'
	fi
    ])
    AC_CACHE_CHECK([for xti modmap],[xti_cv_modmap],[dnl
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../Modules.map" ; then
		xti_cv_modmap=`echo "$xti_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for xti symver],[xti_cv_symver],[dnl
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../Module.symvers" ; then
		xti_cv_symver=`echo "$xti_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for xti manpath],[xti_cv_manpath],[dnl
	for xti_dir in $xti_cv_includes ; do
	    if test -d "$xti_dir/../../doc/man" ; then
		xti_cv_manpath=`echo "$xti_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${xti_cv_includes:-no}" = :no ; then
	if test :"$with_xti" = :no ; then
	    AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS XNET include directories.  If
*** you wish to use the STREAMS XNET package you will need to specify
*** the location of the STREAMS XNET (strxnet) include directories with
*** the --with-xti=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS XNS package?  The
*** STREAMS strxns package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strxnet-0.9.2.8.tar.gz".
*** ])
	fi
    fi
    AC_MSG_CHECKING([for xti added configure arguments])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly,
dnl so we use defines.
    if test -z "$with_xti" ; then
	if test :"${xti_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_xti --with-xti\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-xti'"
	    AC_MSG_RESULT([--with-xti])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_xti --without-xti\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-xti'"
	    AC_MSG_RESULT([--without-xti])
	fi
    else
	AC_MSG_RESULT([--with-xti="$with_xti"])
    fi
    AC_CACHE_CHECK([for xti version], [xti_cv_version], [dnl
	xti_what="sys/strxnet/version.h"
	xti_file=
	if test -n "$xti_cv_includes" ; then
	    for xti_dir in $xti_cv_includes ; do
		# old place for version
		if test -f "$xti_dir/$xti_what" ; then
		    xti_file="$xti_dir/$xti_what"
		    break
		fi
		# new place for version
		if test -n $linux_cv_k_release ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
		    if test -f "$xti_dir/$linux_cv_k_release/$target_cpu/$xti_what" ; then
			xti_file="$xti_dir/$linux_cv_k_release/$target_cpu/$xti_what"
			break
		    fi
		fi
	    done
	fi
	if test :${xti_file:-no} != :no ; then
	    xti_cv_version=`grep '#define.*\<STRXNET_VERSION\>' $xti_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
])# _XTI_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _XTI_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_DEFINES], [dnl
    for xti_include in $xti_cv_includes ; do
	XTI_CPPFLAGS="${XTI_CPPFLAGS}${XTI_CPPFLAGS:+ }-I${xti_include}"
    done
    XTI_LDADD="$xti_cv_ldadd"
    XTI_MODMAP="$xti_cv_modmap"
    XTI_SYMVER="$xti_cv_symver"
    XTI_MANPATH="$xti_cv_manpath"
    XTI_VERSION="$xti_cv_version"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strxnet for
	that matter.
    ])
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
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
