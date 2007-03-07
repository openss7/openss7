# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: chan.m4,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/03/07 10:12:58 $
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
# Last Modified $Date: 2007/03/07 10:12:58 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: chan.m4,v $
# Revision 0.9.2.16  2007/03/07 10:12:58  brian
# - more corrections
#
# Revision 0.9.2.15  2007/03/07 09:24:07  brian
# - further corrections
#
# Revision 0.9.2.14  2007/03/07 07:29:21  brian
# - search harder for versions
#
# Revision 0.9.2.13  2007/03/06 23:39:53  brian
# - more corrections
#
# Revision 0.9.2.12  2007/03/06 23:13:56  brian
# - master build correction
#
# Revision 0.9.2.11  2007/03/04 23:26:40  brian
# - corrected modversions directory
#
# Revision 0.9.2.10  2007/03/04 23:14:42  brian
# - better search for modversions
#
# Revision 0.9.2.9  2007/03/01 07:17:25  brian
# - updating common build process
#
# Revision 0.9.2.8  2007/03/01 06:38:15  brian
# - updates to common build process
#
# Revision 0.9.2.7  2007/03/01 01:45:15  brian
# - updating build process
#
# Revision 0.9.2.6  2007/03/01 00:10:18  brian
# - update to build process for 2.4 kernels
#
# Revision 0.9.2.5  2007/02/28 11:51:31  brian
# - make sure build directory exists
#
# Revision 0.9.2.4  2007/02/22 08:36:38  brian
# - balance parentheses
#
# Revision 0.9.2.3  2006-12-28 08:32:31  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.2  2006/10/13 08:38:44  brian
# - proper check file
#
# Revision 0.9.2.1  2006/10/13 08:29:50  brian
# - added file to locate strchan package
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS CHAN
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS Channel package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strchan loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _CHAN
# -----------------------------------------------------------------------------
# Check for the existence of Channel header files, particularly sys/tpi.h.
# Channel header files are required for building the TPI interface for CHAN.
# Without Channel header files, the TPI interface to Channel will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _CHAN_OPTIONS
    _CHAN_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_CHAN_KERNEL])
    _CHAN_USER
    _CHAN_OUTPUT
    AC_SUBST([CHAN_CPPFLAGS])dnl
    AC_SUBST([CHAN_MODFLAGS])dnl
    AC_SUBST([CHAN_LDADD])dnl
    AC_SUBST([CHAN_LDADD32])dnl
    AC_SUBST([CHAN_LDFLAGS])dnl
    AC_SUBST([CHAN_LDFLAGS32])dnl
    AC_SUBST([CHAN_MODMAP])dnl
    AC_SUBST([CHAN_SYMVER])dnl
    AC_SUBST([CHAN_MANPATH])dnl
    AC_SUBST([CHAN_VERSION])dnl
])# _CHAN
# =============================================================================

# =============================================================================
# _CHAN_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_OPTIONS], [dnl
    AC_ARG_WITH([chan],
		AS_HELP_STRING([--with-chan=HEADERS],
			       [specify the Channel header file directory.
				@<:@default=$INCLUDEDIR/chan@:>@]),
		[with_chan="$withval"],
		[with_chan=''])
])# _CHAN_OPTIONS
# =============================================================================

# =============================================================================
# _CHAN_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_SETUP], [dnl
    _CHAN_CHECK_HEADERS
    for chan_include in $chan_cv_includes ; do
	CHAN_CPPFLAGS="${CHAN_CPPFLAGS}${CHAN_CPPFLAGS:+ }-I${chan_include}"
    done
    if test :"${chan_cv_config:-no}" != :no ; then
	CHAN_CPPFLAGS="${CHAN_CPPFLAGS}${CHAN_CPPFLAGS:+ }-include ${chan_cv_config}"
    fi
    if test :"${chan_cv_modversions:-no}" != :no ; then
	CHAN_MODFLAGS="${CHAN_MODFLAGS}${CHAN_MODFLAGS:+ }-include ${chan_cv_modversions}"
    fi
])# _CHAN_SETUP
# =============================================================================

# =============================================================================
# _CHAN_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS Channel header files.  The
    # package normally requires Channel header files to compile.
    AC_CACHE_CHECK([for chan include directory], [chan_cv_includes], [dnl
	chan_what="sys/ch.h"
	if test :"${with_chan:-no}" != :no -a :"${with_chan:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_chan)])
	    for chan_dir in $with_chan ; do
		if test -d "$chan_dir" ; then
		    AC_MSG_CHECKING([for chan include directory... $chan_dir])
		    if test -r "$chan_dir/$chan_what" ; then
			chan_cv_includes="$with_chan"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${chan_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-chan="$with_chan"
***
*** however, $chan_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for xit include directory])
	fi
	if test :"${chan_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    chan_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/strchan/src/include}"
	    chan_inc="${os7_cv_master_builddir:+$os7_cv_master_builddir/strchan/include}"
	    chan_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strchan/src/include}"
	    if test -d "$chan_dir" ; then
		AC_MSG_CHECKING([for chan include directory... $chan_dir $chan_bld])
		if test -r "$chan_dir/$chan_what" ; then
		    chan_cv_includes="$chan_inc $chan_bld $chan_dir"
		    chan_cv_ldadd= # "$os7_cv_master_builddir/strchan/libchan.la"
		    chan_cv_ldadd32= # "$os7_cv_master_builddir/strchan/lib32/libchan.la"
		    chan_cv_modversions="$os7_cv_master_builddir/strchan/include/sys/strchan/modversions.h"
		    chan_cv_modmap="$os7_cv_master_builddir/strchan/Modules.map"
		    chan_cv_symver="$os7_cv_master_builddir/strchan/Module.symvers"
		    chan_cv_manpath="$os7_cv_master_builddir/strchan/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for chan include directory])
	fi
	if test :"${chan_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    chan_here=`pwd`
	    AC_MSG_RESULT([(searching from $chan_here)])
	    for chan_dir in \
		$srcdir/strchan*/src/include \
		$srcdir/../strchan*/src/include \
		../_build/$srcdir/../../strchan*/src/include \
		../_build/$srcdir/../../../strchan*/src/include
	    do
		if test -d "$chan_dir" ; then
		    chan_bld=`echo $chan_dir | sed -e "s|^$srcdir/|$chan_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    chan_inc=`echo $chan_bld/../../include |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    chan_dir=`(cd $chan_dir; pwd)`
		    AC_MSG_CHECKING([for chan include directory... $chan_dir $chan_bld])
		    if test -d "$chan_bld" -a -r "$chan_dir/$chan_what" ; then
			chan_cv_includes="$chan_inc $chan_bld $chan_dir"
			chan_cv_ldadd= # `echo "$chan_bld/../../libchan.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			chan_cv_ldadd32= # `echo "$chan_bld/../../lib32/libchan.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			chan_cv_modversions=`echo "$chan_inc/sys/strchan/modversions.h" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			chan_cv_modmap=`echo "$chan_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			chan_cv_symver=`echo "$chan_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			chan_cv_manpath=`echo "$chan_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for chan include directory])
	fi
	if test :"${chan_cv_includes:-no}" = :no ; then
	    # Channel header files are normally found in the strchan package now.
	    # They used to be part of the XNET add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "chan_search_path=\"
			${DESTDIR}${includedir}/strchan
			${DESTDIR}${rootdir}${oldincludedir}/strchan
			${DESTDIR}${rootdir}/usr/include/strchan
			${DESTDIR}${rootdir}/usr/local/include/strchan
			${DESTDIR}${rootdir}/usr/src/strchan/src/include
			${DESTDIR}${oldincludedir}/strchan
			${DESTDIR}/usr/include/strchan
			${DESTDIR}/usr/local/include/strchan
			${DESTDIR}/usr/src/strchan/src/include\""
		    ;;
		(LfS)
		    eval "chan_search_path=\"
			${DESTDIR}${includedir}/strchan
			${DESTDIR}${rootdir}${oldincludedir}/strchan
			${DESTDIR}${rootdir}/usr/include/strchan
			${DESTDIR}${rootdir}/usr/local/include/strchan
			${DESTDIR}${rootdir}/usr/src/strchan/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strchan
			${DESTDIR}/usr/include/strchan
			${DESTDIR}/usr/local/include/strchan
			${DESTDIR}/usr/src/strchan/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    chan_search_path=`echo "$chan_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    chan_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for chan_dir in $chan_search_path ; do
		if test -d "$chan_dir" ; then
		    AC_MSG_CHECKING([for chan include directory... $chan_dir])
		    if test -r "$chan_dir/$chan_what" ; then
			chan_cv_includes="$chan_dir"
			#chan_cv_ldadd=
			#chan_cv_ldadd32=
			#chan_cv_modmap=
			#chan_cv_symver=
			#chan_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for chan include directory])
	fi
    ])
    AC_CACHE_CHECK([for chan ldadd native], [chan_cv_ldadd], [dnl
	chan_cv_ldadd=
	for chan_dir in $chan_cv_includes ; do
	    if test -f "$chan_dir/../../libchan.la" ; then
		chan_cv_ldadd=`echo "$chan_dir/../../libchan.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for chan ldflags], [chan_cv_ldflags], [dnl
	chan_cv_ldflags=
	if test -z "$chan_cv_ldadd" ; then
	    chan_cv_ldflags="-lchan"
	else
	    chan_cv_ldflags="-L$(dirname $chan_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for chan ldadd 32-bit], [chan_cv_ldadd32], [dnl
	chan_cv_ldadd32=
	for chan_dir in $chan_cv_includes ; do
	    if test -f "$chan_dir/../../lib32/libchan.la" ; then
		chan_cv_ldadd32=`echo "$chan_dir/../../lib32/libchan.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for chan ldflags 32-bit], [chan_cv_ldflags32], [dnl
	chan_cv_ldflags32=
	if test -z "$chan_cv_ldadd32" ; then
	    chan_cv_ldflags32="-lchan"
	else
	    chan_cv_ldflags32="-L$(dirname $chan_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for chan modmap], [chan_cv_modmap], [dnl
	chan_cv_modmap=
	for chan_dir in $chan_cv_includes ; do
	    if test -f "$chan_dir/../../Modules.map" ; then
		chan_cv_modmap=`echo "$chan_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for chan symver], [chan_cv_symver], [dnl
	chan_cv_symver=
	for chan_dir in $chan_cv_includes ; do
	    if test -f "$chan_dir/../../Module.symvers" ; then
		chan_cv_symver=`echo "$chan_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for chan manpath], [chan_cv_manpath], [dnl
	chan_cv_manpath=
	for chan_dir in $chan_cv_includes ; do
	    if test -d "$chan_dir/../../doc/man" ; then
		chan_cv_manpath=`echo "$chan_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${chan_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS Channel include directories.  If
*** you wish to use the STREAMS Channel package you will need to specify
*** the location of the STREAMS Channel (strchan) include directories with
*** the --with-chan=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS Channel package?  The
*** STREAMS strchan package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strchan-0.9.2.1.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for chan version], [chan_cv_version], [dnl
	chan_cv_version=
	if test -z "$chan_cv_version" ; then
	    chan_what="sys/strchan/version.h"
	    chan_file=
	    if test -n "$chan_cv_includes" ; then
		for chan_dir in $chan_cv_includes ; do
		    # old place for version
		    if test -f "$chan_dir/$chan_what" ; then
			chan_file="$chan_dir/$chan_what"
			break
		    fi
		    # new place for version
		    if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
			if test -f "$chan_dir/$linux_cv_k_release/$target_cpu/$chan_what" ; then
			    chan_file="$chan_dir/$linux_cv_k_release/$target_cpu/$chan_what"
			    break
			fi
		    fi
		done
	    fi
	    if test :${chan_file:-no} != :no ; then
		chan_cv_version=`grep '#define.*\<STRCHAN_VERSION\>' $chan_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	    fi
	fi
	if test -z "$chan_cv_version" ; then
	    chan_epoch=
	    chan_version=
	    chan_package=
	    chan_release=
	    if test -n "$chan_cv_includes" ; then
		for chan_dir in $chan_cv_includes ; do
		    if test -z "$chan_epoch" -a -s "$chan_dir/../.rpmepoch" ; then
			chan_epoch=`cat $chan_dir/../.rpmepoch`
		    fi
		    if test -z "$chan_epoch" -a -s "$chan_dir/../../.rpmepoch" ; then
			chan_epoch=`cat $chan_dir/../../.rpmepoch`
		    fi
		    if test -z "$chan_version" -a -s "$chan_dir/../.version" ; then
			chan_version=`cat $chan_dir/../.version`
		    fi
		    if test -z "$chan_version" -a -s "$chan_dir/../../.version" ; then
			chan_version=`cat $chan_dir/../../.version`
		    fi
		    if test -z "$chan_version" -a -s "$chan_dir/../configure" ; then
			chan_version=`grep -m 1 '^PACKAGE_VERSION=' $chan_dir/../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$chan_version" -a -s "$chan_dir/../../configure" ; then
			chan_version=`grep -m 1 '^PACKAGE_VERSION=' $chan_dir/../../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$chan_package" -a -s "$chan_dir/../.pkgrelease" ; then
			chan_package=`cat $chan_dir/../.pkgrelease`
		    fi
		    if test -z "$chan_package" -a -s "$chan_dir/../../.pkgrelease" ; then
			chan_package=`cat $chan_dir/../../.pkgrelease`
		    fi
		    if test -z "$chan_release" -a -s "$chan_dir/../.rpmrelease" ; then
			chan_release=`cat $chan_dir/../.rpmrelease`
		    fi
		    if test -z "$chan_release" -a -s "$chan_dir/../../.rpmrelease" ; then
			chan_release=`cat $chan_dir/../../.rpmrelease`
		    fi
		done
	    fi
	    if test -n "$chan_epoch" -a -n "$chan_version" -a -n "$chan_package" -a -n "$chan_release" ; then
		chan_cv_version="$chan_epoch:$chan_version.$chan_package-$chan_release"
	    fi
	fi
    ])
    chan_what="sys/strchan/config.h"
    AC_CACHE_CHECK([for chan $chan_what], [chan_cv_config], [dnl
	chan_cv_config=
	if test -n "$chan_cv_includes" ; then
	    AC_MSG_RESULT([(searching $chan_cv_includes)])
	    for chan_dir in $chan_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for chan $chan_what... $chan_dir])
		if test -f "$chan_dir/$chan_what" ; then
		    chan_cv_config="$chan_dir/$chan_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for chan $chan_what... $chan_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$chan_dir/$linux_cv_k_release/$target_cpu/$chan_what" ; then
			chan_cv_config="$chan_dir/$linux_cv_k_release/$target_cpu/$chan_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for chan $chan_what])
	fi
    ])
    chan_what="sys/strchan/modversions.h"
    AC_CACHE_CHECK([for chan $chan_what], [chan_cv_modversions], [dnl
	chan_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$chan_cv_includes" ; then
		AC_MSG_RESULT([(searching $chan_cv_includes)])
		for chan_dir in $chan_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for chan $chan_what... $chan_dir])
		    if test -f "$chan_dir/$chan_what" ; then
			chan_cv_modversions="$chan_dir/$chan_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for chan $chan_what... $chan_dir/$linux_cv_k_release/$target_cpu])
			if test "$chan_dir/$linux_cv_k_release/$target_cpu/$chan_what" ; then
			    chan_cv_includes="$chan_dir/$linux_cv_k_release/$target_cpu $chan_cv_includes"
			    chan_cv_modversions="$chan_dir/$linux_cv_k_release/$target_cpu/$chan_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for chan $chan_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for chan added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_chan" ; then
	if test :"${chan_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_chan --with-chan\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-chan'"
	    AC_MSG_RESULT([--with-chan])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_chan --without-chan\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-chan'"
	    AC_MSG_RESULT([--without-chan])
	fi
    else
	AC_MSG_RESULT([--with-chan="$with_chan"])
    fi
])# _CHAN_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _CHAN_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_KERNEL], [dnl
])# _CHAN_KERNEL
# =============================================================================

# =============================================================================
# _CHAN_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_OUTPUT], [dnl
    _CHAN_DEFINES
])# _CHAN_OUTPUT
# =============================================================================

# =============================================================================
# _CHAN_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_DEFINES], [dnl
    if test :"${chan_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_CHAN_MODVERSIONS_H], [1], [Define when
	    the STREAMS Channel release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    CHAN_CPPFLAGS="${CHAN_CPPFLAGS:+ ${CHAN_CPPFLAGS}}"
    CHAN_LDADD="$chan_cv_ldadd"
    CHAN_LDADD32="$chan_cv_ldadd32"
    CHAN_LDFLAGS="$chan_cv_ldflags"
    CHAN_LDFLAGS32="$chan_cv_ldflags32"
    CHAN_MODMAP="$chan_cv_modmap"
    CHAN_SYMVER="$chan_cv_symver"
    CHAN_MANPATH="$chan_cv_manpath"
    CHAN_VERSION="$chan_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${CHAN_SYMVER:+${MODPOST_INPUTS:+ }${CHAN_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strchan for
	that matter.
    ])
])# _CHAN_DEFINES
# =============================================================================

# =============================================================================
# _CHAN_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_USER], [dnl
])# _CHAN_USER
# =============================================================================

# =============================================================================
# _CHAN_
# -----------------------------------------------------------------------------
AC_DEFUN([_CHAN_], [dnl
])# _CHAN_
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
