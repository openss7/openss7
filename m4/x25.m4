# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: x25.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-03 12:50:22 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2008-05-03 12:50:22 $ by $Author: brian $
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS X.25
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS X.25 package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strx25 loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _X25
# -----------------------------------------------------------------------------
# Check for the existence of X.25 header files, particularly sys/tpi.h.
# X.25 header files are required for building the TPI interface for X.25.
# Without X.25 header files, the TPI interface to X.25 will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_X25], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _X25_OPTIONS
    _X25_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_X25_KERNEL])
    _X25_USER
    _X25_OUTPUT
    AC_SUBST([X25_CPPFLAGS])dnl
    AC_SUBST([X25_MODFLAGS])dnl
    AC_SUBST([X25_LDADD])dnl
    AC_SUBST([X25_LDADD32])dnl
    AC_SUBST([X25_LDFLAGS])dnl
    AC_SUBST([X25_LDFLAGS32])dnl
    AC_SUBST([X25_MODMAP])dnl
    AC_SUBST([X25_SYMVER])dnl
    AC_SUBST([X25_MANPATH])dnl
    AC_SUBST([X25_VERSION])dnl
])# _X25
# =============================================================================

# =============================================================================
# _X25_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_OPTIONS], [dnl
    AC_ARG_WITH([x25],
		AS_HELP_STRING([--with-x25=HEADERS],
			       [specify the X.25 header file directory.
				@<:@default=$INCLUDEDIR/x25@:>@]),
		[with_x25="$withval"],
		[with_x25=''])
])# _X25_OPTIONS
# =============================================================================

# =============================================================================
# _X25_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_SETUP], [dnl
    _X25_CHECK_HEADERS
    for x25_include in $x25_cv_includes ; do
	X25_CPPFLAGS="${X25_CPPFLAGS}${X25_CPPFLAGS:+ }-I${x25_include}"
    done
    if test :"${x25_cv_config:-no}" != :no ; then
	X25_CPPFLAGS="${X25_CPPFLAGS}${X25_CPPFLAGS:+ }-include ${x25_cv_config}"
    fi
    if test :"${x25_cv_modversions:-no}" != :no ; then
	X25_MODFLAGS="${X25_MODFLAGS}${X25_MODFLAGS:+ }-include ${x25_cv_modversions}"
    fi
])# _X25_SETUP
# =============================================================================

# =============================================================================
# _X25_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS X.25 header files.  The
    # package normally requires X.25 header files to compile.
    AC_CACHE_CHECK([for x25 include directory], [x25_cv_includes], [dnl
	x25_what="sys/xti_osi.h"
	if test :"${with_x25:-no}" != :no -a :"${with_x25:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_x25)])
	    for x25_dir in $with_x25 ; do
		if test -d "$x25_dir" ; then
		    AC_MSG_CHECKING([for x25 include directory... $x25_dir])
		    if test -r "$x25_dir/$x25_what" ; then
			x25_cv_includes="$with_x25"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${x25_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-x25="$with_x25"
***
*** however, $x25_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for xit include directory])
	fi
	if test :"${x25_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    x25_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/strx25/src/include}"
	    x25_inc="${os7_cv_master_builddir:+$os7_cv_master_builddir/strx25/include}"
	    x25_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strx25/src/include}"
	    if test -d "$x25_dir" ; then
		AC_MSG_CHECKING([for x25 include directory... $x25_dir $x25_bld])
		if test -r "$x25_dir/$x25_what" ; then
		    x25_cv_includes="$x25_inc $x25_bld $x25_dir"
		    x25_cv_ldadd= # "$os7_cv_master_builddir/strx25/libx25.la"
		    x25_cv_ldadd32= # "$os7_cv_master_builddir/strx25/lib32/libx25.la"
		    x25_cv_modversions="$os7_cv_master_builddir/strx25/include/sys/strx25/modversions.h"
		    x25_cv_modmap="$os7_cv_master_builddir/strx25/Modules.map"
		    x25_cv_symver="$os7_cv_master_builddir/strx25/Module.symvers"
		    x25_cv_manpath="$os7_cv_master_builddir/strx25/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for x25 include directory])
	fi
	if test :"${x25_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    x25_here=`pwd`
	    AC_MSG_RESULT([(searching from $x25_here)])
	    for x25_dir in \
		$srcdir/strx25*/src/include \
		$srcdir/../strx25*/src/include \
		../_build/$srcdir/../../strx25*/src/include \
		../_build/$srcdir/../../../strx25*/src/include
	    do
		if test -d "$x25_dir" ; then
		    x25_bld=`echo $x25_dir | sed -e "s|^$srcdir/|$x25_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    x25_inc=`echo $x25_bld/../../include |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    x25_dir=`(cd $x25_dir; pwd)`
		    AC_MSG_CHECKING([for x25 include directory... $x25_dir $x25_bld])
		    if test -d "$x25_bld" -a -r "$x25_dir/$x25_what" ; then
			x25_cv_includes="$x25_inc $x25_bld $x25_dir"
			x25_cv_ldadd= # `echo "$x25_bld/../../libx25.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			x25_cv_ldadd32= # `echo "$x25_bld/../../lib32/libx25.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			x25_cv_modversions=`echo "$x25_inc/sys/strx25/modversions.h" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			x25_cv_modmap=`echo "$x25_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			x25_cv_symver=`echo "$x25_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			x25_cv_manpath=`echo "$x25_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for x25 include directory])
	fi
	if test :"${x25_cv_includes:-no}" = :no ; then
	    # X.25 header files are normally found in the strx25 package now.
	    # They used to be part of the XNET add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "x25_search_path=\"
			${DESTDIR}${includedir}/strx25
			${DESTDIR}${rootdir}${oldincludedir}/strx25
			${DESTDIR}${rootdir}/usr/include/strx25
			${DESTDIR}${rootdir}/usr/local/include/strx25
			${DESTDIR}${rootdir}/usr/src/strx25/src/include
			${DESTDIR}${oldincludedir}/strx25
			${DESTDIR}/usr/include/strx25
			${DESTDIR}/usr/local/include/strx25
			${DESTDIR}/usr/src/strx25/src/include\""
		    ;;
		(LfS)
		    eval "x25_search_path=\"
			${DESTDIR}${includedir}/strx25
			${DESTDIR}${rootdir}${oldincludedir}/strx25
			${DESTDIR}${rootdir}/usr/include/strx25
			${DESTDIR}${rootdir}/usr/local/include/strx25
			${DESTDIR}${rootdir}/usr/src/strx25/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strx25
			${DESTDIR}/usr/include/strx25
			${DESTDIR}/usr/local/include/strx25
			${DESTDIR}/usr/src/strx25/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    x25_search_path=`echo "$x25_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    x25_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for x25_dir in $x25_search_path ; do
		if test -d "$x25_dir" ; then
		    AC_MSG_CHECKING([for x25 include directory... $x25_dir])
		    if test -r "$x25_dir/$x25_what" ; then
			x25_cv_includes="$x25_dir"
			#x25_cv_ldadd=
			#x25_cv_ldadd32=
			#x25_cv_modmap=
			#x25_cv_symver=
			#x25_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for x25 include directory])
	fi
    ])
    AC_CACHE_CHECK([for x25 ldadd native], [x25_cv_ldadd], [dnl
	x25_cv_ldadd=
	for x25_dir in $x25_cv_includes ; do
	    if test -f "$x25_dir/../../libx25.la" ; then
		x25_cv_ldadd=`echo "$x25_dir/../../libx25.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for x25 ldflags], [x25_cv_ldflags], [dnl
	x25_cv_ldflags=
	if test -z "$x25_cv_ldadd" ; then
	    x25_cv_ldflags="-lx25"
	else
	    x25_cv_ldflags="-L$(dirname $x25_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for x25 ldadd 32-bit], [x25_cv_ldadd32], [dnl
	x25_cv_ldadd32=
	for x25_dir in $x25_cv_includes ; do
	    if test -f "$x25_dir/../../lib32/libx25.la" ; then
		x25_cv_ldadd32=`echo "$x25_dir/../../lib32/libx25.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for x25 ldflags 32-bit], [x25_cv_ldflags32], [dnl
	x25_cv_ldflags32=
	if test -z "$x25_cv_ldadd32" ; then
	    x25_cv_ldflags32="-lx25"
	else
	    x25_cv_ldflags32="-L$(dirname $x25_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for x25 modmap], [x25_cv_modmap], [dnl
	x25_cv_modmap=
	for x25_dir in $x25_cv_includes ; do
	    if test -f "$x25_dir/../../Modules.map" ; then
		x25_cv_modmap=`echo "$x25_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for x25 symver], [x25_cv_symver], [dnl
	x25_cv_symver=
	for x25_dir in $x25_cv_includes ; do
	    if test -f "$x25_dir/../../Module.symvers" ; then
		x25_cv_symver=`echo "$x25_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for x25 manpath], [x25_cv_manpath], [dnl
	x25_cv_manpath=
	for x25_dir in $x25_cv_includes ; do
	    if test -d "$x25_dir/../../doc/man" ; then
		x25_cv_manpath=`echo "$x25_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${x25_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS X.25 include directories.  If
*** you wish to use the STREAMS X.25 package you will need to specify
*** the location of the STREAMS X.25 (strx25) include directories with
*** the --with-x25=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS X.25 package?  The
*** STREAMS strx25 package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strx25-0.9.2.1.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for x25 version], [x25_cv_version], [dnl
	x25_cv_version=
	if test -z "$x25_cv_version" ; then
	    x25_what="sys/strx25/version.h"
	    x25_file=
	    if test -n "$x25_cv_includes" ; then
		for x25_dir in $x25_cv_includes ; do
		    # old place for version
		    if test -f "$x25_dir/$x25_what" ; then
			x25_file="$x25_dir/$x25_what"
			break
		    fi
		    # new place for version
		    if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
			if test -f "$x25_dir/$linux_cv_k_release/$target_cpu/$x25_what" ; then
			    x25_file="$x25_dir/$linux_cv_k_release/$target_cpu/$x25_what"
			    break
			fi
		    fi
		done
	    fi
	    if test :${x25_file:-no} != :no ; then
		x25_cv_version=`grep '#define.*\<STRX25_VERSION\>' $x25_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	    fi
	fi
	if test -z "$x25_cv_version" ; then
	    x25_epoch=
	    x25_version=
	    x25_package=
	    x25_release=
	    if test -n "$x25_cv_includes" ; then
		for x25_dir in $x25_cv_includes ; do
		    if test -z "$x25_epoch" -a -s "$x25_dir/../.rpmepoch" ; then
			x25_epoch=`cat $x25_dir/../.rpmepoch`
		    fi
		    if test -z "$x25_epoch" -a -s "$x25_dir/../../.rpmepoch" ; then
			x25_epoch=`cat $x25_dir/../../.rpmepoch`
		    fi
		    if test -z "$x25_version" -a -s "$x25_dir/../.version" ; then
			x25_version=`cat $x25_dir/../.version`
		    fi
		    if test -z "$x25_version" -a -s "$x25_dir/../../.version" ; then
			x25_version=`cat $x25_dir/../../.version`
		    fi
		    if test -z "$x25_version" -a -s "$x25_dir/../configure" ; then
			x25_version=`grep -m 1 '^PACKAGE_VERSION=' $x25_dir/../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$x25_version" -a -s "$x25_dir/../../configure" ; then
			x25_version=`grep -m 1 '^PACKAGE_VERSION=' $x25_dir/../../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$x25_package" -a -s "$x25_dir/../.pkgrelease" ; then
			x25_package=`cat $x25_dir/../.pkgrelease`
		    fi
		    if test -z "$x25_package" -a -s "$x25_dir/../../.pkgrelease" ; then
			x25_package=`cat $x25_dir/../../.pkgrelease`
		    fi
		    if test -z "$x25_release" -a -s "$x25_dir/../.rpmrelease" ; then
			x25_release=`cat $x25_dir/../.rpmrelease`
		    fi
		    if test -z "$x25_release" -a -s "$x25_dir/../../.rpmrelease" ; then
			x25_release=`cat $x25_dir/../../.rpmrelease`
		    fi
		done
	    fi
	    if test -n "$x25_epoch" -a -n "$x25_version" -a -n "$x25_package" -a -n "$x25_release" ; then
		x25_cv_version="$x25_epoch:$x25_version.$x25_package-$x25_release"
	    fi
	fi
    ])
    x25_what="sys/strx25/config.h"
    AC_CACHE_CHECK([for x25 $x25_what], [x25_cv_config], [dnl
	x25_cv_config=
	if test -n "$x25_cv_includes" ; then
	    AC_MSG_RESULT([(searching $x25_cv_includes)])
	    for x25_dir in $x25_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for x25 $x25_what... $x25_dir])
		if test -f "$x25_dir/$x25_what" ; then
		    x25_cv_config="$x25_dir/$x25_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for x25 $x25_what... $x25_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$x25_dir/$linux_cv_k_release/$target_cpu/$x25_what" ; then
			x25_cv_config="$x25_dir/$linux_cv_k_release/$target_cpu/$x25_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for x25 $x25_what])
	fi
    ])
    x25_what="sys/strx25/modversions.h"
    AC_CACHE_CHECK([for x25 $x25_what], [x25_cv_modversions], [dnl
	x25_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$x25_cv_includes" ; then
		AC_MSG_RESULT([(searching $x25_cv_includes)])
		for x25_dir in $x25_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for x25 $x25_what... $x25_dir])
		    if test -f "$x25_dir/$x25_what" ; then
			x25_cv_modversions="$x25_dir/$x25_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for x25 $x25_what... $x25_dir/$linux_cv_k_release/$target_cpu])
			if test "$x25_dir/$linux_cv_k_release/$target_cpu/$x25_what" ; then
			    x25_cv_includes="$x25_dir/$linux_cv_k_release/$target_cpu $x25_cv_includes"
			    x25_cv_modversions="$x25_dir/$linux_cv_k_release/$target_cpu/$x25_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for x25 $x25_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for x25 added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_x25" ; then
	if test :"${x25_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_x25 --with-x25\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-x25'"
	    AC_MSG_RESULT([--with-x25])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_x25 --without-x25\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-x25'"
	    AC_MSG_RESULT([--without-x25])
	fi
    else
	AC_MSG_RESULT([--with-x25="$with_x25"])
    fi
])# _X25_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _X25_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_KERNEL], [dnl
])# _X25_KERNEL
# =============================================================================

# =============================================================================
# _X25_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_OUTPUT], [dnl
    _X25_DEFINES
])# _X25_OUTPUT
# =============================================================================

# =============================================================================
# _X25_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_DEFINES], [dnl
    if test :"${x25_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_X25_MODVERSIONS_H], [1], [Define when
	    the STREAMS X.25 release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    X25_CPPFLAGS="${X25_CPPFLAGS:+ ${X25_CPPFLAGS}}"
    X25_LDADD="$x25_cv_ldadd"
    X25_LDADD32="$x25_cv_ldadd32"
    X25_LDFLAGS="$x25_cv_ldflags"
    X25_LDFLAGS32="$x25_cv_ldflags32"
    X25_MODMAP="$x25_cv_modmap"
    X25_SYMVER="$x25_cv_symver"
    X25_MANPATH="$x25_cv_manpath"
    X25_VERSION="$x25_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${X25_SYMVER:+${MODPOST_INPUTS:+ }${X25_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strx25 for
	that matter.
    ])
])# _X25_DEFINES
# =============================================================================

# =============================================================================
# _X25_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_USER], [dnl
])# _X25_USER
# =============================================================================

# =============================================================================
# _X25_
# -----------------------------------------------------------------------------
AC_DEFUN([_X25_], [dnl
])# _X25_
# =============================================================================

# =============================================================================
#
# $Log: x25.m4,v $
# Revision 0.9.2.1  2008-05-03 12:50:22  brian
# - added x25.m4 to locate strx25 package
#
# =============================================================================
# 
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn

