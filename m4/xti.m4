# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: xti.m4,v $ $Name:  $($Revision: 0.9.2.60 $) $Date: 2008-04-28 09:41:04 $
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
# Last Modified $Date: 2008-04-28 09:41:04 $ by $Author: brian $
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS XNET
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS XNET package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strxnet loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _XTI
# -----------------------------------------------------------------------------
# Check for the existence of XTI header files, particularly sys/tpi.h.
# XTI header files are required for building the TPI interface for XTI.
# Without XTI header files, the TPI interface to XTI will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _XTI_OPTIONS
    _XTI_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_XTI_KERNEL])
    _XTI_USER
    _XTI_OUTPUT
    AC_SUBST([XTI_CPPFLAGS])dnl
    AC_SUBST([XTI_MODFLAGS])dnl
    AC_SUBST([XTI_LDADD])dnl
    AC_SUBST([XTI_LDADD32])dnl
    AC_SUBST([XTI_LDFLAGS])dnl
    AC_SUBST([XTI_LDFLAGS32])dnl
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
    for xti_include in $xti_cv_includes ; do
	XTI_CPPFLAGS="${XTI_CPPFLAGS}${XTI_CPPFLAGS:+ }-I${xti_include}"
    done
    if test :"${xti_cv_config:-no}" != :no ; then
	XTI_CPPFLAGS="${XTI_CPPFLAGS}${XTI_CPPFLAGS:+ }-include ${xti_cv_config}"
    fi
    if test :"${xti_cv_modversions:-no}" != :no ; then
	XTI_MODFLAGS="${XTI_MODFLAGS}${XTI_MODFLAGS:+ }-include ${xti_cv_modversions}"
    fi
])# _XTI_SETUP
# =============================================================================

# =============================================================================
# _XTI_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS XTI header files.  The
    # package normally requires XTI header files to compile.
    AC_CACHE_CHECK([for xti include directory], [xti_cv_includes], [dnl
	xti_what="xti.h"
	if test :"${with_xti:-no}" != :no -a :"${with_xti:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_xti)])
	    for xti_dir in $with_xti ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti include directory... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_includes="$with_xti"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${xti_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-xti="$with_xti"
***
*** however, $xti_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for xit include directory])
	fi
	if test :"${xti_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    xti_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/strxnet/src/include}"
	    xti_inc="${os7_cv_master_builddir:+$os7_cv_master_builddir/strxnet/include}"
	    xti_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strxnet/src/include}"
	    if test -d "$xti_dir" ; then
		AC_MSG_CHECKING([for xti include directory... $xti_dir $xti_bld])
		if test -r "$xti_dir/$xti_what" ; then
		    xti_cv_includes="$xti_inc $xti_bld $xti_dir"
		    xti_cv_ldadd="$os7_cv_master_builddir/strxnet/libxnet.la"
		    xti_cv_ldadd32="$os7_cv_master_builddir/strxnet/lib32/libxnet.la"
		    xti_cv_modversions="$os7_cv_master_builddir/strxnet/include/sys/strxnet/modversions.h"
		    xti_cv_modmap="$os7_cv_master_builddir/strxnet/Modules.map"
		    xti_cv_symver="$os7_cv_master_builddir/strxnet/Module.symvers"
		    xti_cv_manpath="$os7_cv_master_builddir/strxnet/doc/man"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    fi
	    AC_MSG_CHECKING([for xti include directory])
	fi
	if test :"${xti_cv_includes:-no}" = :no ; then
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
		    xti_inc=`echo $xti_bld/../../include |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    xti_dir=`(cd $xti_dir; pwd)`
		    AC_MSG_CHECKING([for xti include directory... $xti_dir $xti_bld])
		    if test -d "$xti_bld" -a -r "$xti_dir/$xti_what" ; then
			xti_cv_includes="$xti_inc $xti_bld $xti_dir"
			xti_cv_ldadd=`echo "$xti_bld/../../libxnet.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			xti_cv_ldadd32=`echo "$xti_bld/../../lib32/libxnet.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			xti_cv_modversions=`echo "$xti_inc/sys/strxnet/modversions.h" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			xti_cv_modmap=`echo "$xti_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			xti_cv_symver=`echo "$xti_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			xti_cv_manpath=`echo "$xti_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti include directory])
	fi
	if test :"${xti_cv_includes:-no}" = :no ; then
	    # XTI header files are normally found in the strxnet package now.
	    # They used to be part of the XNET add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
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
		(LfS)
		    eval "xti_search_path=\"
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
	    xti_search_path=`echo "$xti_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    xti_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for xti_dir in $xti_search_path ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti include directory... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_includes="$xti_dir"
			#xti_cv_ldadd=
			#xti_cv_ldadd32=
			#xti_cv_modmap=
			#xti_cv_symver=
			#xti_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti include directory])
	fi
    ])
    AC_CACHE_CHECK([for xti xnet ldadd native], [xti_cv_ldadd], [dnl
	xti_what="libxnet.la"
	xti_cv_ldadd=
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../$xti_what" ; then
		xti_cv_ldadd=`echo "$xti_dir/../../$xti_what" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$xti_cv_ldadd" ; then
	    eval "xti_search_path=\"
		${DESTDIR}${rootdir}${libdir}
		${DESTDIR}${libdir}\""
	    xti_search_path=`echo "$xti_search_path" | sed -e 's|\<NONE\>|'$ac_default_prefix'|g;s|//|/|g'`
	    AC_MSG_RESULT([searching])
	    for xti_dir in $xti_search_path ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti xnet ldadd native... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_ldadd="$xti_dir/$xti_what"
			xti_cv_ldflags=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti xnet ldadd native])
	fi
    ])
    AC_CACHE_CHECK([for xti xnet ldflags], [xti_cv_ldflags], [dnl
	xti_cv_ldflags=
	if test -z "$xti_cv_ldadd" ; then
	    xti_cv_ldflags='-L$(DESTDIR)$(rootdir)$(libdir) -lxnet'
	else
	    xti_cv_ldflags="-L$(dirname $xti_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for xti xnet ldadd 32-bit], [xti_cv_ldadd32], [dnl
	xti_what="libxnet.la"
	xti_cv_ldadd32=
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../lib32/$xti_what" ; then
		xti_cv_ldadd32=`echo "$xti_dir/../../lib32/$xti_what" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$xti_cv_ldadd32" ; then
	    eval "xti_search_path=\"
		${DESTDIR}${rootdir}${lib32dir}
		${DESTDIR}${lib32dir}\""
	    xti_search_path=`echo "$xti_search_path" | sed -e 's|\<NONE\>|'$ac_default_prefix'|g;s|//|/|g'`
	    AC_MSG_RESULT([searching])
	    for xti_dir in $xti_search_path ; do
		if test -d "$xti_dir" ; then
		    AC_MSG_CHECKING([for xti xnet ldadd 32-bit... $xti_dir])
		    if test -r "$xti_dir/$xti_what" ; then
			xti_cv_ldadd32="$xti_dir/$xti_what"
			xti_cv_ldflags32=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti xnet ldadd 32-bit])
	fi
    ])
    AC_CACHE_CHECK([for xti xnet ldflags 32-bit], [xti_cv_ldflags32], [dnl
	xti_cv_ldflags32=
	if test -z "$xti_cv_ldadd32" ; then
	    xti_cv_ldflags32='-L$(DESTDIR)$(rootdir)$(lib32dir) -lxnet'
	else
	    xti_cv_ldflags32="-L$(dirname $xti_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for xti modmap], [xti_cv_modmap], [dnl
	xti_cv_modmap=
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../Modules.map" ; then
		xti_cv_modmap=`echo "$xti_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for xti symver], [xti_cv_symver], [dnl
	xti_cv_symver=
	for xti_dir in $xti_cv_includes ; do
	    if test -f "$xti_dir/../../Module.symvers" ; then
		xti_cv_symver=`echo "$xti_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for xti manpath], [xti_cv_manpath], [dnl
	xti_cv_manpath=
	for xti_dir in $xti_cv_includes ; do
	    if test -d "$xti_dir/../../doc/man" ; then
		xti_cv_manpath=`echo "$xti_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${xti_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS XNET include directories.  If
*** you wish to use the STREAMS XNET package you will need to specify
*** the location of the STREAMS XNET (strxnet) include directories with
*** the --with-xti=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS XNET package?  The
*** STREAMS strxnet package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strxnet-0.9.2.8.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for xti version], [xti_cv_version], [dnl
	xti_cv_version=
	if test -z "$xti_cv_version" ; then
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
		    if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
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
	fi
	if test -z "$xti_cv_version" ; then
	    xti_epoch=
	    xti_version=
	    xti_package=
	    xti_release=
	    if test -n "$xti_cv_includes" ; then
		for xti_dir in $xti_cv_includes ; do
		    if test -z "$xti_epoch" -a -s "$xti_dir/../.rpmepoch" ; then
			xti_epoch=`cat $xti_dir/../.rpmepoch`
		    fi
		    if test -z "$xti_epoch" -a -s "$xti_dir/../../.rpmepoch" ; then
			xti_epoch=`cat $xti_dir/../../.rpmepoch`
		    fi
		    if test -z "$xti_version" -a -s "$xti_dir/../.version" ; then
			xti_version=`cat $xti_dir/../.version`
		    fi
		    if test -z "$xti_version" -a -s "$xti_dir/../../.version" ; then
			xti_version=`cat $xti_dir/../../.version`
		    fi
		    if test -z "$xti_version" -a -s "$xti_dir/../configure" ; then
			xti_version=`grep -m 1 '^PACKAGE_VERSION=' $xti_dir/../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$xti_version" -a -s "$xti_dir/../../configure" ; then
			xti_version=`grep -m 1 '^PACKAGE_VERSION=' $xti_dir/../../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$xti_package" -a -s "$xti_dir/../.pkgrelease" ; then
			xti_package=`cat $xti_dir/../.pkgrelease`
		    fi
		    if test -z "$xti_package" -a -s "$xti_dir/../../.pkgrelease" ; then
			xti_package=`cat $xti_dir/../../.pkgrelease`
		    fi
		    if test -z "$xti_release" -a -s "$xti_dir/../.rpmrelease" ; then
			xti_release=`cat $xti_dir/../.rpmrelease`
		    fi
		    if test -z "$xti_release" -a -s "$xti_dir/../../.rpmrelease" ; then
			xti_release=`cat $xti_dir/../../.rpmrelease`
		    fi
		done
	    fi
	    if test -n "$xti_epoch" -a -n "$xti_version" -a -n "$xti_package" -a -n "$xti_release" ; then
		xti_cv_version="$xti_epoch:$xti_version.$xti_package-$xti_release"
	    fi
	fi
    ])
    xti_what="sys/strxnet/config.h"
    AC_CACHE_CHECK([for xti $xti_what], [xti_cv_config], [dnl
	xti_cv_config=
	if test -n "$xti_cv_includes" ; then
	    AC_MSG_RESULT([(searching $xti_cv_includes)])
	    for xti_dir in $xti_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for xti $xti_what... $xti_dir])
		if test -f "$xti_dir/$xti_what" ; then
		    xti_cv_config="$xti_dir/$xti_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for xti $xti_what... $xti_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$xti_dir/$linux_cv_k_release/$target_cpu/$xti_what" ; then
			xti_cv_config="$xti_dir/$linux_cv_k_release/$target_cpu/$xti_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for xti $xti_what])
	fi
    ])
    xti_what="sys/strxnet/modversions.h"
    AC_CACHE_CHECK([for xti $xti_what], [xti_cv_modversions], [dnl
	xti_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$xti_cv_includes" ; then
		AC_MSG_RESULT([(searching $xti_cv_includes)])
		for xti_dir in $xti_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for xti $xti_what... $xti_dir])
		    if test -f "$xti_dir/$xti_what" ; then
			xti_cv_modversions="$xti_dir/$xti_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for xti $xti_what... $xti_dir/$linux_cv_k_release/$target_cpu])
			if test "$xti_dir/$linux_cv_k_release/$target_cpu/$xti_what" ; then
			    xti_cv_includes="$xti_dir/$linux_cv_k_release/$target_cpu $xti_cv_includes"
			    xti_cv_modversions="$xti_dir/$linux_cv_k_release/$target_cpu/$xti_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for xti $xti_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for xti added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
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
])# _XTI_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _XTI_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_KERNEL], [dnl
])# _XTI_KERNEL
# =============================================================================

# =============================================================================
# _XTI_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_OUTPUT], [dnl
    _XTI_DEFINES
])# _XTI_OUTPUT
# =============================================================================

# =============================================================================
# _XTI_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_DEFINES], [dnl
    if test :"${xti_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_XNET_MODVERSIONS_H], [1], [Define when
	    the STREAMS XNET release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    XTI_CPPFLAGS="${XTI_CPPFLAGS:+ ${XTI_CPPFLAGS}}"
    XTI_LDADD="$xti_cv_ldadd"
    XTI_LDADD32="$xti_cv_ldadd32"
    XTI_LDFLAGS="$xti_cv_ldflags"
    XTI_LDFLAGS32="$xti_cv_ldflags32"
    XTI_MODMAP="$xti_cv_modmap"
    XTI_SYMVER="$xti_cv_symver"
    XTI_MANPATH="$xti_cv_manpath"
    XTI_VERSION="$xti_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${XTI_SYMVER:+${MODPOST_INPUTS:+ }${XTI_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strxnet for
	that matter.
    ])
])# _XTI_DEFINES
# =============================================================================

# =============================================================================
# _XTI_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_USER], [dnl
])# _XTI_USER
# =============================================================================

# =============================================================================
# _XTI_
# -----------------------------------------------------------------------------
AC_DEFUN([_XTI_], [dnl
])# _XTI_
# =============================================================================

# =============================================================================
#
# $Log: xti.m4,v $
# Revision 0.9.2.60  2008-04-28 09:41:04  brian
# - updated headers for release
#
# Revision 0.9.2.59  2007/08/12 19:05:32  brian
# - rearrange and update headers
#
# Revision 0.9.2.58  2007/03/07 10:12:59  brian
# - more corrections
#
# Revision 0.9.2.57  2007/03/07 09:24:08  brian
# - further corrections
#
# Revision 0.9.2.56  2007/03/07 07:29:22  brian
# - search harder for versions
#
# Revision 0.9.2.55  2007/03/06 23:39:54  brian
# - more corrections
#
# Revision 0.9.2.54  2007/03/06 23:13:58  brian
# - master build correction
#
# Revision 0.9.2.53  2007/03/04 23:26:40  brian
# - corrected modversions directory
#
# Revision 0.9.2.52  2007/03/04 23:14:43  brian
# - better search for modversions
#
# Revision 0.9.2.51  2007/03/03 08:01:20  brian
# - corrections
#
# Revision 0.9.2.50  2007/03/03 05:42:46  brian
# - better standalong library detection
#
# Revision 0.9.2.49  2007/03/01 07:17:25  brian
# - updating common build process
#
# Revision 0.9.2.48  2007/03/01 06:38:15  brian
# - updates to common build process
#
# Revision 0.9.2.47  2007/03/01 01:45:16  brian
# - updating build process
#
# Revision 0.9.2.46  2007/03/01 00:10:19  brian
# - update to build process for 2.4 kernels
#
# Revision 0.9.2.45  2007/02/28 11:51:32  brian
# - make sure build directory exists
#
# Revision 0.9.2.44  2007/02/22 08:36:39  brian
# - balance parentheses
#
# Revision 0.9.2.43  2006-12-28 08:32:32  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.42  2006/09/30 07:29:06  brian
# - corrected warning message
# - corrected variable name in xti.m4
# - added iso.m4 to locate striso package
#
# Revision 0.9.2.41  2006/09/29 10:57:46  brian
# - autoconf does not like multiline cache variables
#
# Revision 0.9.2.40  2006/09/29 03:46:17  brian
# - substitute LDFLAGS32
#
# Revision 0.9.2.39  2006/09/29 03:22:39  brian
# - handle flags better
#
# Revision 0.9.2.38  2006/09/27 05:08:42  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.37  2006/09/25 08:56:35  brian
# - corrections by inspection
#
# Revision 0.9.2.35  2006/09/18 00:33:54  brian
# - added libxnsl library and checks for 32bit compatibility libraries
#
# Revision 0.9.2.34  2006/03/14 09:20:47  brian
# - typo
#
# Revision 0.9.2.33  2006/03/14 09:04:11  brian
# - syntax consistency, advanced search
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
