# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: inet.m4,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/03/13 23:21:41 $
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
    AC_SUBST([INET_CPPFLAGS])dnl
    AC_SUBST([INET_LDADD])dnl
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
    _INET_DEFINES
])# _INET_SETUP
# =============================================================================

# =============================================================================
# _INET_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS INET header files.  The package
    # normally requires either Linux STREAMS or Linux Fast-STREAMS INET header
    # files (or both) to compile.
    AC_CACHE_CHECK([for inet include directory], [inet_cv_includes], [dnl
	inet_what="sys/xti_inet.h"
	if test ":${with_inet:-no}" != :no -a :"${with_inet:-no}" != :yes ;  then
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
	    if test ":${inet_cv_includes:-no}" = :no ; then
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
	if test ":${inet_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build directory, if any.
	    AC_MSG_RESULT([(searching $master_srcdir $master_builddir)])
	    inet_search_path="
		${master_srcdir:+$master_srcdir/strinet/src/include}
		${master_builddir:+$master_builddir/strinet/src/include}"
	    for inet_dir in $inet_search_path ; do
		if test -d "$inet_dir" ; then
		    AC_MSG_CHECKING([for inet include directory... $inet_dir])
		    if test -r "$inet_dir/$inet_what" ; then
			inet_cv_includes="$inet_search_path"
			inet_cv_ldadd= # "$master_builddir/strinet/libinet.la"
			inet_cv_modmap= # "$master_builddir/strinet/Modules.map"
			inet_cv_symver= # "$master_builddir/strinet/Module.symvers"
			inet_cv_manpath="$master_builddir/strinet/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		fi
	    done
	    AC_MSG_CHECKING([for inet include directory])
	fi
	if test ":${inet_cv_includes:-no}" = :no ; then
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
		    AC_MSG_CHECKING([for inet include directory... $inet_dir])
		    if test -r "$inet_dir/$inet_what" ; then
			inet_cv_includes="$inet_dir $inet_bld"
			inet_cv_ldadd=
			inet_cv_modmap=
			inet_cv_symver=
			inet_cv_manpath=`echo "$inet_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for inet include directory])
	fi
	if test ":${inet_cv_includes:-no}" = :no ; then
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
		${DESTDIR}${rootdir}/usr/src/streams/src/include
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
		${DESTDIR}${oldincludedir}/streams
		${DESTDIR}/usr/include/streams
		${DESTDIR}/usr/local/include/streams
		${DESTDIR}/usr/src/streams/include
		${DESTDIR}${oldincludedir}/LiS
		${DESTDIR}/usr/include/LiS
		${DESTDIR}/usr/local/include/LiS
		${DESTDIR}/usr/src/LiS/include\""
	    inet_search_path=`echo "$inet_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    inet_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for inet_dir in $inet_search_path ; do
		if test -d "$inet_dir" ; then
		    AC_MSG_CHECKING([for inet include directory... $inet_dir])
		    if test -r "$inet_dir/$inet_what" ; then
			inet_cv_includes="$inet_dir"
			inet_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for inet include directory])
	fi
    ])
    AC_CACHE_CHECK([for inet ldadd],[inet_cv_ldadd],[dnl
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../libinet.la" ; then
		inet_cv_ldadd=`echo "$inet_dir/../../libinet.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$inet_cv_ldadd" ; then
	    inet_cv_ldadd='-linet'
	    inet_cv_ldadd=
	fi
    ])
    AC_CACHE_CHECK([for inet modmap],[inet_cv_modmap],[dnl
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../Modules.map" ; then
		inet_cv_modmap=`echo "$inet_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for inet symver],[inet_cv_symver],[dnl
	for inet_dir in $inet_cv_includes ; do
	    if test -f "$inet_dir/../../Module.symvers" ; then
		inet_cv_symver=`echo "$inet_dir/../../Modules.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for inet manpath],[inet_cv_manpath],[dnl
	for inet_dir in $inet_cv_includes ; do
	    if test -d "$inet_dir/../../doc/man" ; then
		inet_cv_manpath=`echo "$inet_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${inet_cv_includes:-no}" = :no ; then :
	if test :"$with_inet" = :no ; then
	    AC_MSG_ERROR([
***
*** Configure could not find the STREAMS INET include directories.  If
*** you wish to use the STREAMS INET package, you will need to specify
*** the location of the STREAMS INET (strinet) include directories with
*** the --with-inet=@<:@DIRECTORY@:>@ option to ./configure and try
*** again.
***
*** Perhaps you just forgot to load the STREAMS INET package?  The
*** STREAMS strinet package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strinet-0.9.2.3.tar.gz".
*** ])
	fi
    fi
    AC_MSG_CHECKING([for inet added configure arguments])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly,
dnl so we use defines.
    if test -z "$with_inet" ; then
	if test :"${inet_cv_includes:-no}" = :no ; then :
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
		if test -n $linux_cv_k_release ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
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
])# _INET_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _INET_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_INET_DEFINES], [dnl
    for inet_include in $inet_cv_includes ; do
	INET_CPPFLAGS="${INET_CPPFLAGS}${INET_CPPFLAGS:+ }-I${inet_include}"
    done
    INET_LDADD="$inet_cv_ldadd"
    INET_MODMAP="$inet_cv_modmap"
    INET_SYMVER="$inet_cv_symver"
    INET_MANPATH="$inet_cv_manpath"
    INET_VERSION="$inet_cv_version"
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
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
