# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: sock.m4,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2007/03/01 07:17:25 $
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
# Last Modified $Date: 2007/03/01 07:17:25 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: sock.m4,v $
# Revision 0.9.2.15  2007/03/01 07:17:25  brian
# - updating common build process
#
# Revision 0.9.2.14  2007/03/01 06:38:15  brian
# - updates to common build process
#
# Revision 0.9.2.13  2007/03/01 01:45:15  brian
# - updating build process
#
# Revision 0.9.2.12  2007/03/01 00:10:18  brian
# - update to build process for 2.4 kernels
#
# Revision 0.9.2.11  2007/02/28 11:51:32  brian
# - make sure build directory exists
#
# Revision 0.9.2.10  2007/02/22 08:36:38  brian
# - balance parentheses
#
# Revision 0.9.2.9  2006-12-28 08:32:32  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.8  2006/09/30 08:12:45  brian
# - corrected search start for includes
#
# Revision 0.9.2.7  2006/09/30 07:29:06  brian
# - corrected warning message
# - corrected variable name in xti.m4
# - added iso.m4 to locate striso package
#
# Revision 0.9.2.6  2006/09/29 10:57:46  brian
# - autoconf does not like multiline cache variables
#
# Revision 0.9.2.5  2006/09/29 03:56:53  brian
# - typos
#
# Revision 0.9.2.4  2006/09/29 03:46:16  brian
# - substitute LDFLAGS32
#
# Revision 0.9.2.3  2006/09/29 03:22:38  brian
# - handle flags better
#
# Revision 0.9.2.2  2006/09/27 05:08:41  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.1  2006/09/25 08:38:20  brian
# - added m4 files for locating NSL and SOCK
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS SOCK
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS SOCK package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strsock loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _SOCK
# -----------------------------------------------------------------------------
# Check for the existence of SOCK header files, particularly sys/tpi.h.
# SOCK header files are required for building the TPI interface for SOCK.
# Without SOCK header files, the TPI interface to SOCK will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _SOCK_OPTIONS
    _SOCK_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_SOCK_KERNEL])
    _SOCK_USER
    _SOCK_OUTPUT
    AC_SUBST([SOCK_CPPFLAGS])dnl
    AC_SUBST([SOCK_MODFLAGS])dnl
    AC_SUBST([SOCK_LDADD])dnl
    AC_SUBST([SOCK_LDADD32])dnl
    AC_SUBST([SOCK_LDFLAGS])dnl
    AC_SUBST([SOCK_LDFLAGS32])dnl
    AC_SUBST([SOCK_MODMAP])dnl
    AC_SUBST([SOCK_SYMVER])dnl
    AC_SUBST([SOCK_MANPATH])dnl
    AC_SUBST([SOCK_VERSION])dnl
])# _SOCK
# =============================================================================

# =============================================================================
# _SOCK_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_OPTIONS], [dnl
    AC_ARG_WITH([sock],
		AS_HELP_STRING([--with-sock=HEADERS],
			       [specify the SOCK header file directory.
				@<:@default=$INCLUDEDIR/sock@:>@]),
		[with_sock="$withval"],
		[with_sock=''])
])# _SOCK_OPTIONS
# =============================================================================

# =============================================================================
# _SOCK_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_SETUP], [dnl
    _SOCK_CHECK_HEADERS
    for sock_include in $sock_cv_includes ; do
	SOCK_CPPFLAGS="${SOCK_CPPFLAGS}${SOCK_CPPFLAGS:+ }-I${sock_include}"
    done
    if test :"${sock_cv_config:-no}" != :no ; then
	SOCK_CPPFLAGS="${SOCK_CPPFLAGS}${SOCK_CPPFLAGS:+ }-include ${sock_cv_config}"
    fi
    if test :"${sock_cv_modversions:-no}" != :no ; then
	SOCK_MODFLAGS="${SOCK_MODFLAGS}${SOCK_MODFLAGS:+ }-include ${sock_cv_modversions}"
    fi
])# _SOCK_SETUP
# =============================================================================

# =============================================================================
# _SOCK_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS SOCK header files.  The
    # package normally requires SOCK header files to compile.
    AC_CACHE_CHECK([for sock include directory], [sock_cv_includes], [dnl
	sock_what="sockmod.h"
	if test :"${with_sock:-no}" != :no -a :"${with_sock:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_sock)])
	    for sock_dir in $with_sock ; do
		if test -d "$sock_dir" ; then
		    AC_MSG_CHECKING([for sock include directory... $sock_dir])
		    if test -r "$sock_dir/$sock_what" ; then
			sock_cv_includes="$with_sock"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${sock_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-sock="$with_sock"
***
*** however, $sock_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for xit include directory])
	fi
	if test :"${sock_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    sock_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strsock/src/include}"
	    sock_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/strsock/src/include}"
	    if test -d "$sock_dir" ; then
		AC_MSG_CHECKING([for sock include directory... $sock_dir $sock_bld])
		if test -d "$sock_bld" -a -r "$sock_dir/$sock_what" ; then
		    sock_cv_includes="$sock_bld $sock_dir"
		    sock_cv_ldadd="$os7_cv_master_builddir/strsock/libsocket.la"
		    sock_cv_ldadd32="$os7_cv_master_builddir/strsock/lib32/libsocket.la"
		    sock_cv_modmap="$os7_cv_master_builddir/strsock/Modules.map"
		    sock_cv_symver="$os7_cv_master_builddir/strsock/Module.symvers"
		    sock_cv_manpath="$os7_cv_master_builddir/strsock/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for sock include directory])
	fi
	if test :"${sock_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    sock_here=`pwd`
	    AC_MSG_RESULT([(searching from $sock_here)])
	    for sock_dir in \
		$srcdir/strsock*/src/include \
		$srcdir/../strsock*/src/include \
		../_build/$srcdir/../../strsock*/src/include \
		../_build/$srcdir/../../../strsock*/src/include
	    do
		if test -d "$sock_dir" ; then
		    sock_bld=`echo $sock_dir | sed -e "s|^$srcdir/|$sock_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    sock_dir=`(cd $sock_dir; pwd)`
		    AC_MSG_CHECKING([for sock include directory... $sock_dir $sock_bld])
		    if test -d "$sock_bld" -a -r "$sock_dir/$sock_what" ; then
			sock_cv_includes="$sock_bld $sock_dir"
			sock_cv_ldadd=`echo "$sock_bld/../../libsocket.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sock_cv_ldadd32=`echo "$sock_bld/../../lib32/libsocket.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sock_cv_modmap=`echo "$sock_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sock_cv_symver=`echo "$sock_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sock_cv_manpath=`echo "$sock_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sock include directory])
	fi
	if test :"${sock_cv_includes:-no}" = :no ; then
	    # SOCK header files are normally found in the strsock package now.
	    # They used to be part of the SOCK add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "sock_search_path=\"
			${DESTDIR}${includedir}/strsock
			${DESTDIR}${rootdir}${oldincludedir}/strsock
			${DESTDIR}${rootdir}/usr/include/strsock
			${DESTDIR}${rootdir}/usr/local/include/strsock
			${DESTDIR}${rootdir}/usr/src/strsock/src/include
			${DESTDIR}${includedir}/LiS/sock
			${DESTDIR}${rootdir}${oldincludedir}/LiS/sock
			${DESTDIR}${rootdir}/usr/include/LiS/sock
			${DESTDIR}${rootdir}/usr/local/include/LiS/sock
			${DESTDIR}${rootdir}/usr/src/LiS/include/sock
			${DESTDIR}${includedir}/sock
			${DESTDIR}${rootdir}${oldincludedir}/sock
			${DESTDIR}${rootdir}/usr/include/sock
			${DESTDIR}${rootdir}/usr/local/include/sock
			${DESTDIR}${oldincludedir}/strsock
			${DESTDIR}/usr/include/strsock
			${DESTDIR}/usr/local/include/strsock
			${DESTDIR}/usr/src/strsock/src/include
			${DESTDIR}${oldincludedir}/LiS/sock
			${DESTDIR}/usr/include/LiS/sock
			${DESTDIR}/usr/local/include/LiS/sock
			${DESTDIR}/usr/src/LiS/include/sock
			${DESTDIR}${oldincludedir}/sock
			${DESTDIR}/usr/include/sock
			${DESTDIR}/usr/local/include/sock\""
		    ;;
		(LfS)
		    eval "sock_search_path=\"
			${DESTDIR}${includedir}/strsock
			${DESTDIR}${rootdir}${oldincludedir}/strsock
			${DESTDIR}${rootdir}/usr/include/strsock
			${DESTDIR}${rootdir}/usr/local/include/strsock
			${DESTDIR}${rootdir}/usr/src/strsock/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strsock
			${DESTDIR}/usr/include/strsock
			${DESTDIR}/usr/local/include/strsock
			${DESTDIR}/usr/src/strsock/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    sock_search_path=`echo "$sock_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    sock_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for sock_dir in $sock_search_path ; do
		if test -d "$sock_dir" ; then
		    AC_MSG_CHECKING([for sock include directory... $sock_dir])
		    if test -r "$sock_dir/$sock_what" ; then
			sock_cv_includes="$sock_dir"
			#sock_cv_ldadd=
			#sock_cv_ldadd32=
			#sock_cv_modmap=
			#sock_cv_symver=
			#sock_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sock include directory])
	fi
    ])
    AC_CACHE_CHECK([for sock ldadd native], [sock_cv_ldadd], [dnl
	sock_cv_ldadd=
	for sock_dir in $sock_cv_includes ; do
	    if test -f "$sock_dir/../../libsocket.la" ; then
		sock_cv_ldadd=`echo "$sock_dir/../../libsocket.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sock ldflags], [sock_cv_ldflags], [dnl
	sock_cv_ldflags=
	if test -z "$sock_cv_ldadd" ; then
	    sock_cv_ldflags="-lsocket"
	else
	    sock_cv_ldflags="-L$(dirname $sock_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for sock ldadd 32-bit], [sock_cv_ldadd32], [dnl
	sock_cv_ldadd32=
	for sock_dir in $sock_cv_includes ; do
	    if test -f "$sock_dir/../../lib32/libsocket.la" ; then
		sock_cv_ldadd32=`echo "$sock_dir/../../lib32/libsocket.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sock ldflags 32-bit], [sock_cv_ldflags32], [dnl
	sock_cv_ldflags32=
	if test -z "$sock_cv_ldadd32" ; then
	    sock_cv_ldflags32="-lsocket"
	else
	    sock_cv_ldflags32="-L$(dirname $sock_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for sock modmap], [sock_cv_modmap], [dnl
	sock_cv_modmap=
	for sock_dir in $sock_cv_includes ; do
	    if test -f "$sock_dir/../../Modules.map" ; then
		sock_cv_modmap=`echo "$sock_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sock symver], [sock_cv_symver], [dnl
	sock_cv_symver=
	for sock_dir in $sock_cv_includes ; do
	    if test -f "$sock_dir/../../Module.symvers" ; then
		sock_cv_symver=`echo "$sock_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sock manpath], [sock_cv_manpath], [dnl
	sock_cv_manpath=
	for sock_dir in $sock_cv_includes ; do
	    if test -d "$sock_dir/../../doc/man" ; then
		sock_cv_manpath=`echo "$sock_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${sock_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS SOCK include directories.  If
*** you wish to use the STREAMS SOCK package you will need to specify
*** the location of the STREAMS SOCK (strsock) include directories with
*** the --with-sock=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS SOCK package?  The
*** STREAMS strsock package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strsock-0.9.2.1.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for sock version], [sock_cv_version], [dnl
	sock_what="sys/strsock/version.h"
	sock_file=
	if test -n "$sock_cv_includes" ; then
	    for sock_dir in $sock_cv_includes ; do
		# old place for version
		if test -f "$sock_dir/$sock_what" ; then
		    sock_file="$sock_dir/$sock_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$sock_dir/$linux_cv_k_release/$target_cpu/$sock_what" ; then
			sock_file="$sock_dir/$linux_cv_k_release/$target_cpu/$sock_what"
			break
		    fi
		fi
	    done
	fi
	if test :${sock_file:-no} != :no ; then
	    sock_cv_version=`grep '#define.*\<STRSOCK_VERSION\>' $sock_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    sock_what="sys/strsock/config.h"
    AC_CACHE_CHECK([for sock $sock_what], [sock_cv_config], [dnl
	sock_cv_config=
	if test -n "$sock_cv_includes" ; then
	    AC_MSG_RESULT([(searching $sock_cv_includes)])
	    for sock_dir in $sock_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for sock $sock_what... $sock_dir])
		if test -f "$sock_dir/$sock_what" ; then
		    sock_cv_config="$sock_dir/$sock_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for sock $sock_what... $sock_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$sock_dir/$linux_cv_k_release/$target_cpu/$sock_what" ; then
			sock_cv_config="$sock_dir/$linux_cv_k_release/$target_cpu/$sock_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sock $sock_what])
	fi
    ])
    sock_what="sys/strsock/modversions.h"
    AC_CACHE_CHECK([for sock $sock_what], [sock_cv_modversions], [dnl
	sock_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$sock_cv_includes" ; then
		AC_MSG_RESULT([(searching $sock_cv_includes)])
		for sock_dir in $sock_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for sock $sock_what... $sock_dir])
		    if test -f "$sock_dir/$sock_what" ; then
			sock_cv_modversions="$sock_dir/$sock_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for sock $sock_what... $sock_dir/$linux_cv_k_release/$target_cpu])
			if test "$sock_dir/$linux_cv_k_release/$target_cpu/$sock_what" ; then
			    sock_cv_includes="$sock_dir/$linux_cv_k_release/$target_cpu $sock_cv_includes"
			    sock_cv_modversions="$sock_dir/$linux_cv_k_release/$target_cpu/$sock_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for sock $sock_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for sock added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_sock" ; then
	if test :"${sock_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_sock --with-sock\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-sock'"
	    AC_MSG_RESULT([--with-sock])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_sock --without-sock\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-sock'"
	    AC_MSG_RESULT([--without-sock])
	fi
    else
	AC_MSG_RESULT([--with-sock="$with_sock"])
    fi
])# _SOCK_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _SOCK_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_KERNEL], [dnl
])# _SOCK_KERNEL
# =============================================================================

# =============================================================================
# _SOCK_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_OUTPUT], [dnl
    _SOCK_DEFINES
])# _SOCK_OUTPUT
# =============================================================================

# =============================================================================
# _SOCK_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_DEFINES], [dnl
    if test :"${sock_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_SOCK_MODVERSIONS_H], [1], [Define when
	    the STREAMS SOCK release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    SOCK_CPPFLAGS="${SOCK_CPPFLAGS:+ ${SOCK_CPPFLAGS}}"
    SOCK_LDADD="$sock_cv_ldadd"
    SOCK_LDADD32="$sock_cv_ldadd32"
    SOCK_LDFLAGS="$sock_cv_ldflags"
    SOCK_LDFLAGS32="$sock_cv_ldflags32"
    SOCK_MODMAP="$sock_cv_modmap"
    SOCK_SYMVER="$sock_cv_symver"
    SOCK_MANPATH="$sock_cv_manpath"
    SOCK_VERSION="$sock_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${SOCK_SYMVER:+${MODPOST_INPUTS:+ }${SOCK_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strsock for
	that matter.
    ])
])# _SOCK_DEFINES
# =============================================================================

# =============================================================================
# _SOCK_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_USER], [dnl
])# _SOCK_USER
# =============================================================================

# =============================================================================
# _SOCK_
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_], [dnl
])# _SOCK_
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
