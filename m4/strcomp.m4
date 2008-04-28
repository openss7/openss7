# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: strcomp.m4,v $ $Name:  $($Revision: 0.9.2.36 $) $Date: 2008-04-28 09:41:04 $
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
# This file provides some common macros for finding a STREAMS compatiblityu
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS compatibility package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strcompat loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _STRCOMP
# -----------------------------------------------------------------------------
# Check for the existence of COMPAT header file, particularly sys/os7/compat.h.
# COMPAT header files are required for building the ABI interface for STREAMS.
# Without COMPAT header files, the ABI interface for STREAMS will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP], [dnl
    AC_REQUIRE([_LINUX_STREAMS])
    _STRCOMP_OPTIONS
    _STRCOMP_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_STRCOMP_KERNEL])
    _STRCOMP_USER
    _STRCOMP_OUTPUT
    AC_SUBST([STRCOMP_CPPFLAGS])dnl
    AC_SUBST([STRCOMP_MODFLAGS])dnl
    AC_SUBST([STRCOMP_LDADD])dnl
    AC_SUBST([STRCOMP_LDADD32])dnl
    AC_SUBST([STRCOMP_LDFLAGS])dnl
    AC_SUBST([STRCOMP_LDFLAGS32])dnl
    AC_SUBST([STRCOMP_MODMAP])dnl
    AC_SUBST([STRCOMP_SYMVER])dnl
    AC_SUBST([STRCOMP_MANPATH])dnl
    AC_SUBST([STRCOMP_VERSION])dnl
])# _STRCOMP
# =============================================================================

# =============================================================================
# _STRCOMP_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_OPTIONS], [dnl
    AC_ARG_WITH([compat],
		AS_HELP_STRING([--with-compat=HEADERS],
			       [specify the STREAMS compatibility header file directory.
				@<:@default=INCLUDEDIR/strcompat@:>@]),
		[with_compat="$withval"],
		[with_compat=''])
])# _STRCOMP_OPTIONS
# =============================================================================

# =============================================================================
# _STRCOMP_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_SETUP], [dnl
    _STRCOMP_CHECK_HEADERS
    for strcomp_include in $strcomp_cv_includes ; do
	STRCOMP_CPPFLAGS="${STRCOMP_CPPFLAGS}${STRCOMP_CPPFLAGS:+ }-I${strcomp_include}"
    done
    if test :"${strcomp_cv_config:-no}" != :no ; then
	STRCOMP_CPPFLAGS="${STRCOMP_CPPFLAGS}${STRCOMP_CPPFLAGS:+ }-include ${strcomp_cv_config}"
    fi
    if test :"${strcomp_cv_modversions:-no}" != :no ; then
	STRCOMP_MODFLAGS="${STRCOMP_MODFLAGS}${STRCOMP_MODFLAGS:+ }-include ${strcomp_cv_modversions}"
    fi
])# _STRCOMP_SETUP
# =============================================================================

# =============================================================================
# _STRCOMP_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS Compatibility header files.  The
    # package normally requires compatibility header files to compile.
    AC_CACHE_CHECK([for compat include directory], [strcomp_cv_includes], [dnl
	strcomp_what="sys/os7/compat.h"
	if test :"${with_compat:-no}" != :no -a :"${with_compat:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_compat)])
	    for strcomp_dir in $with_compat ; do
		if test -d "$strcomp_dir" ; then
		    AC_MSG_CHECKING([for compat include directory... $strcomp_dir])
		    if test -r "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_cinludes="$with_compat"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${strcomp_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-compat="$with_compat"
***
*** however, $strcomp_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for compat include directory])
	fi
	if test :"${strcomp_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    strcomp_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/strcompat/src/include}"
	    strcomp_inc="${os7_cv_master_builddir:+$os7_cv_master_builddir/strcompat/include}"
	    strcomp_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/strcompat/src/include}"
	    if test -d "$strcomp_dir" ; then
		AC_MSG_CHECKING([for compat include directory... $strcomp_dir $strcomp_bld])
		if test -r "$strcomp_dir/$strcomp_what" ; then
		    strcomp_cv_includes="$strcomp_inc $strcomp_bld $strcomp_dir"
		    strcomp_cv_ldadd= # "$os7_cv_master_builddir/strcompat/libcompat.la"
		    strcomp_cv_ldadd32= # "$os7_cv_master_builddir/strcompat/lib32/libcompat.la"
		    strcomp_cv_modversions="$os7_cv_master_builddir/strcompat/include/sys/strcompat/modversions.h"
		    strcomp_cv_modmap="$os7_cv_master_builddir/strcompat/Modules.map"
		    strcomp_cv_symver="$os7_cv_master_builddir/strcompat/Module.symvers"
		    strcomp_cv_manpath="$os7_cv_master_builddir/strcompat/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for compat include directory])
	fi
	if test :"${strcomp_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    strcomp_here=`pwd`
	    AC_MSG_RESULT([(searching from $strcomp_here)])
	    for strcomp_dir in \
		$srcdir/strcompat*/src/include \
		$srcdir/../strcompat*/src/include \
		../_build/$srcdir/../../strcompat*/src/include \
		../_build/$srcdir/../../../strcompat*/src/include
	    do
		if test -d "$strcomp_dir" ; then
		    strcomp_bld=`echo $strcomp_dir | sed -e "s|^$srcdir/|$strcomp_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    strcomp_inc=`echo $strcomp_bld/../../include |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    strcomp_dir=`(cd $strcomp_dir; pwd)`
		    AC_MSG_CHECKING([for compat include directory... $strcomp_dir $strcomp_bld])
		    if test -d "$strcomp_bld" -a -r "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_includes="$strcomp_inc $strcomp_bld $strcomp_dir"
			strcomp_cv_ldadd= # `echo "$strcomp_bld/../../libcompat.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			strcomp_cv_ldadd32= # `echo "$strcomp_bld/../../lib32/libcompat.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			strcomp_cv_modversions=`echo "$strcomp_inc/sys/strcompat/modversions.h" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			strcomp_cv_modmap=`echo "$strcomp_bld/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			strcomp_cv_symver=`echo "$strcomp_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			strcomp_cv_manpath=`echo "$strcomp_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for compat include directory])
	fi
	if test :"${strcomp_cv_includes:-no}" = :no ; then
	    # Compat header files are normally found in the strcompat package now.
	    # They used to be part of the compatibility add-on package and even older
	    # versions are part of the LfS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "strcomp_search_path=\"
			${DESTDIR}${includedir}/strcompat
			${DESTDIR}${rootdir}${oldincludedir}/strcompat
			${DESTDIR}${rootdir}/usr/include/strcompat
			${DESTDIR}${rootdir}/usr/local/include/strcompat
			${DESTDIR}${rootdir}/usr/src/strcompat/src/include
			${DESTDIR}${includedir}/LiS
			${DESTDIR}${rootdir}${oldincludedir}/LiS
			${DESTDIR}${rootdir}/usr/include/LiS
			${DESTDIR}${rootdir}/usr/local/include/LiS
			${DESTDIR}${rootdir}/usr/src/LiS/include
			${DESTDIR}${oldincludedir}/strcompat
			${DESTDIR}/usr/include/strcompat
			${DESTDIR}/usr/local/include/strcompat
			${DESTDIR}/usr/src/strcompat/src/include
			${DESTDIR}${oldincludedir}/LiS
			${DESTDIR}/usr/include/LiS
			${DESTDIR}/usr/local/include/LiS
			${DESTDIR}/usr/src/LiS/include\""
		    ;;
		(LfS)
		    eval "strcomp_search_path=\"
			${DESTDIR}${includedir}/strcompat
			${DESTDIR}${rootdir}${oldincludedir}/strcompat
			${DESTDIR}${rootdir}/usr/include/strcompat
			${DESTDIR}${rootdir}/usr/local/include/strcompat
			${DESTDIR}${rootdir}/usr/src/strcompat/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strcompat
			${DESTDIR}/usr/include/strcompat
			${DESTDIR}/usr/local/include/strcompat
			${DESTDIR}/usr/src/strcompat/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    strcomp_search_path=`echo "$strcomp_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    strcomp_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for strcomp_dir in $strcomp_search_path ; do
		if test -d "$strcomp_dir" ; then
		    AC_MSG_CHECKING([for compat include directory... $strcomp_dir])
		    if test -r "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_includes="$strcomp_dir"
			#strcomp_cv_ldadd=
			#strcomp_cv_ldadd32=
			#strcomp_cv_modmap=
			#strcomp_cv_symver=
			#strcomp_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for compat include directory])
	fi
    ])
    AC_CACHE_CHECK([for compat ldadd native], [strcomp_cv_ldadd], [dnl
	strcomp_what="libcompat.la"
	strcomp_cv_ldadd=
	for strcomp_dir in $strcomp_cv_includes ; do
	    if test -f "$strcomp_dir/../../$strcomp_what" ; then
		strcomp_cv_ldadd=`echo "$strcomp_dir/../../$strcomp_what" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$strcomp_cv_ldadd" ; then
	    eval "strcomp_search_path =\"
		${DESTDIR}${rootdir}${libdir}
		${DESTDIR}${libdir}\""
	    strcomp_search_path=`echo "$strcomp_search_path" | sed -e 's|\<NONE\>|'$ac_default_prefix'|g;s|//|/|g'`
	    AC_MSG_RESULT([searching])
	    for strcomp_dir in $strcomp_search_path ; do
		if test -d "$strcomp_dir" ; then
		    AC_MSG_CHECKING([for compat ldadd native... $strcomp_dir])
		    if test -r "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_ldadd="$strcomp_dir/$strcomp_what"
			strcomp_cv_ldflags=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for compat ldadd native])
	fi
    ])
    AC_CACHE_CHECK([for compat ldflags], [strcomp_cv_ldflags], [dnl
	strcomp_cv_ldflags=
	if test -z "$strcomp_cv_ldadd" ; then
	    strcomp_cv_ldflags= # '-lcompat'
	else
	    strcomp_cv_ldflags= # "-L$(dirname $strcomp_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for compat ldadd 32-bit], [strcomp_cv_ldadd32], [dnl
	strcomp_what="libcompat.la"
	strcomp_cv_ldadd32=
	for strcomp_dir in $strcomp_cv_includes ; do
	    if test -f "$strcomp_dir/../../lib32/$strcomp_what" ; then
		strcomp_cv_ldadd32=`echo "$strcomp_dir/../../lib32/$strcomp_what" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$strcomp_cv_ldadd32" ; then
	    eval "strcomp_search_path =\"
		${DESTDIR}${rootdir}${lib32dir}
		${DESTDIR}${lib32dir}\""
	    strcomp_search_path=`echo "$strcomp_search_path" | sed -e 's|\<NONE\>|'$ac_default_prefix'|g;s|//|/|g'`
	    AC_MSG_RESULT([searching])
	    for strcomp_dir in $strcomp_search_path ; do
		if test -d "$strcomp_dir" ; then
		    AC_MSG_CHECKING([for compat ldadd 32-bit... $strcomp_dir])
		    if test -r "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_ldadd32="$strcomp_dir/$strcomp_what"
			strcomp_cv_ldflags32=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for compat ldadd 32-bit])
	fi
    ])
    AC_CACHE_CHECK([for compat ldflags 32-bit], [strcomp_cv_ldflags32], [dnl
	strcomp_cv_ldflags32=
	if test -z "$strcomp_cv_ldadd32" ; then
	    strcomp_cv_ldflags32= # '-lcompat'
	else
	    strcomp_cv_ldflags32= # "-L$(dirname $strcomp_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for compat modmap], [strcomp_cv_modmap], [dnl
	strcomp_cv_modmap=
	for strcomp_dir in $strcomp_cv_includes ; do
	    if test -f $strcomp_dir/../../Modules.map ; then
		strcomp_cv_modmap=`echo "$strcomp_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for compat symver], [strcomp_cv_symver], [dnl
	strcomp_cv_symver=
	for strcomp_dir in $strcomp_cv_includes ; do
	    if test -f $strcomp_dir/../../Module.symvers ; then
		strcomp_cv_symver=`echo "$strcomp_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for compat manpath], [strcomp_cv_manpath], [dnl
	strcomp_cv_manpath=
	for strcomp_dir in $strcomp_cv_includes ; do
	    if test -d $strcomp_dir/../../doc/man; then
		strcomp_cv_manpath=`echo "$strcomp_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${strcomp_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS compat include directories.  If
*** you wish to use the STREAMS compat package you will need to specify
*** the location of the STREAMS compat (strcompat) include directories with
*** the --with-compat=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS compat package?  The
*** STREAMS strcompat package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strcompat-0.9.2.3.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for compat version], [strcomp_cv_version], [dnl
	strcomp_cv_version=
	if test -z "$strcomp_cv_version" ; then
	    strcomp_what="sys/strcompat/version.h"
	    strcomp_file=
	    if test -n "$strcomp_cv_includes" ; then
		for strcomp_dir in $strcomp_cv_includes ; do
		    # old place for version
		    if test -f "$strcomp_dir/$strcomp_what" ; then
			strcomp_file="$strcomp_dir/$strcomp_what"
			break
		    fi
		    # new place for version
		    if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
			if test -f "$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" ; then
			    strcomp_file="$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what"
			    break
			fi
		    fi
		done
	    fi
	    if test :${strcomp_file:-no} != :no ; then
		strcomp_cv_version=`grep '#define.*\<STRCOMPAT_VERSION\>' $strcomp_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	    fi
	fi
	if test -z "$strcomp_version" ; then
	    strcomp_epoch=
	    strcomp_version=
	    strcomp_package=
	    strcomp_release=
	    if test -n "$strcomp_cv_includes" ; then
		for strcomp_dir in $strcomp_cv_includes ; do
		    if test -z "$strcomp_epoch" -a -s "$strcomp_dir/../.rpmepoch" ; then
			strcomp_epoch=`cat $strcomp_dir/../.rpmepoch`
		    fi
		    if test -z "$strcomp_epoch" -a -s "$strcomp_dir/../../.rpmepoch" ; then
			strcomp_epoch=`cat $strcomp_dir/../../.rpmepoch`
		    fi
		    if test -z "$strcomp_version" -a -s "$strcomp_dir/../.version" ; then
			strcomp_version=`cat $strcomp_dir/../.version`
		    fi
		    if test -z "$strcomp_version" -a -s "$strcomp_dir/../../.version" ; then
			strcomp_version=`cat $strcomp_dir/../../.version`
		    fi
		    if test -z "$strcomp_version" -a -s "$strcomp_dir/../configure" ; then
			strcomp_version=`grep -m 1 '^PACKAGE_VERSION=' $strcomp_dir/../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$strcomp_version" -a -s "$strcomp_dir/../../configure" ; then
			strcomp_version=`grep -m 1 '^PACKAGE_VERSION=' $strcomp_dir/../../configure | sed -e "s,^.*',,;s,'.*[$],,"`
		    fi
		    if test -z "$strcomp_package" -a -s "$strcomp_dir/../.pkgrelease" ; then
			strcomp_package=`cat $strcomp_dir/../.pkgrelease`
		    fi
		    if test -z "$strcomp_package" -a -s "$strcomp_dir/../../.pkgrelease" ; then
			strcomp_package=`cat $strcomp_dir/../../.pkgrelease`
		    fi
		    if test -z "$strcomp_release" -a -s "$strcomp_dir/../.rpmrelease" ; then
			strcomp_release=`cat $strcomp_dir/../.rpmrelease`
		    fi
		    if test -z "$strcomp_release" -a -s "$strcomp_dir/../../.rpmrelease" ; then
			strcomp_release=`cat $strcomp_dir/../../.rpmrelease`
		    fi
		done
	    fi
	    if test -n "$strcomp_epoch" -a -n "$strcomp_version" -a -n "$strcomp_package" -a -n "$strcomp_release" ; then
		strcomp_cv_version="$strcomp_epoch:$strcomp_version.$strcomp_package-$strcomp_release"
	    fi
	fi
    ])
    strcomp_what="sys/strcompat/config.h"
    AC_CACHE_CHECK([for compat $strcomp_what], [strcomp_cv_config], [dnl
	strcomp_cv_config=
	if test -n "$strcomp_cv_includes" ; then
	    AC_MSG_RESULT([(searching $strcomp_cv_includes)])
	    for strcomp_dir in $strcomp_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for compat $strcomp_what... $strcomp_dir])
		if test -f "$strcomp_dir/$strcomp_what" ; then
		    strcomp_cv_config="$strcomp_dir/$strcomp_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for compat $strcomp_what... $strcomp_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" ; then
			strcomp_cv_config="$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for compat $strcomp_what])
	fi
    ])
    strcomp_what="sys/strcompat/modversions.h"
    AC_CACHE_CHECK([for compat $strcomp_what], [strcomp_cv_modversions], [dnl
	strcomp_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$strcomp_cv_includes" ; then
		AC_MSG_RESULT([(searching $strcomp_cv_includes)])
		for strcomp_dir in $strcomp_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for compat $strcomp_what... $strcomp_dir])
		    if test -f "$strcomp_dir/$strcomp_what" ; then
			strcomp_cv_modversions="$strcomp_dir/$strcomp_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for compat $strcomp_what... $strcomp_dir/$linux_cv_k_release/$target_cpu])
			if test -f "$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what" ; then
			    strcomp_cv_includes="$strcomp_dir/$linux_cv_k_release/$target_cpu $strcomp_cv_includes"
			    strcomp_cv_modversions="$strcomp_dir/$linux_cv_k_release/$target_cpu/$strcomp_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for compat $strcomp_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for compat added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_compat" ; then
	if test :"${strcomp_cv_includes:-no}" = :no ; then :
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_compat --with-compat\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-compat'"
	    AC_MSG_RESULT([--with-compat])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_compat --without-compat\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-compat'"
	    AC_MSG_RESULT([--without-compat])
	fi
    else
	AC_MSG_RESULT([--with-compat="$with_compat"])
    fi
])# _STRCOMP_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _STRCOMP_KERNEL
# -----------------------------------------------------------------------------
# Need to know about irqreturn_t for sys/os7/compat.h STREAMS compatibility file.
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_KERNEL], [dnl
    _LINUX_CHECK_TYPES([irqreturn_t], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>	/* for irqreturn_t */ 
    ])
])# _STRCOMP_KERNEL
# =============================================================================

# =============================================================================
# _STRCOMP_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_OUTPUT], [dnl
    _STRCOMP_DEFINES
])# _STRCOMP_OUTPUT
# =============================================================================

# =============================================================================
# _STRCOMP_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_DEFINES], [dnl
    if test :"${strcomp_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_STRCOMP_MODVERSIONS_H], [1], [Define when
	    the STREAMS compatibiltiy release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    STRCOMP_CPPFLAGS="${STRCOMP_CPPFLAGS:+ ${STRCOMP_CPPFLAGS}}"
    STRCOMP_LDADD="$strcomp_cv_ldadd"
    STRCOMP_LDADD32="$strcomp_cv_ldadd32"
    STRCOMP_LDFLAGS="$strcomp_cv_ldflags"
    STRCOMP_LDFLAGS32="$strcomp_cv_ldflags32"
    STRCOMP_MODMAP="$strcomp_cv_modmap"
    STRCOMP_SYMVER="$strcomp_cv_symver"
    STRCOMP_MANPATH="$strcomp_cv_manpath"
    STRCOMP_VERSION="$strcomp_cv_version"
    MODPOST_INPUTS="${MODPOST_INPUTS}${STRCOMP_SYMVER:+${MODPOST_INPUTS:+ }${STRCOMP_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strcompat for
	that matter.
    ])
])# _STRCOMP_DEFINES
# =============================================================================

# =============================================================================
# _STRCOMP_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_USER], [dnl
])# _STRCOMP_USER
# =============================================================================

# =============================================================================
# _STRCOMP_
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCOMP_], [dnl
])# _STRCOMP_
# =============================================================================

# =============================================================================
#
# $Log: strcomp.m4,v $
# Revision 0.9.2.36  2008-04-28 09:41:04  brian
# - updated headers for release
#
# Revision 0.9.2.35  2007/08/12 19:05:32  brian
# - rearrange and update headers
#
# Revision 0.9.2.34  2007/03/07 10:12:59  brian
# - more corrections
#
# Revision 0.9.2.33  2007/03/07 09:24:08  brian
# - further corrections
#
# Revision 0.9.2.32  2007/03/07 07:29:22  brian
# - search harder for versions
#
# Revision 0.9.2.31  2007/03/06 23:39:54  brian
# - more corrections
#
# Revision 0.9.2.30  2007/03/06 23:13:57  brian
# - master build correction
#
# Revision 0.9.2.29  2007/03/04 23:26:40  brian
# - corrected modversions directory
#
# Revision 0.9.2.28  2007/03/04 23:14:43  brian
# - better search for modversions
#
# Revision 0.9.2.27  2007/03/04 22:16:20  brian
# - preparing for eventual library
#
# Revision 0.9.2.26  2007/03/01 07:17:25  brian
# - updating common build process
#
# Revision 0.9.2.25  2007/03/01 06:38:15  brian
# - updates to common build process
#
# Revision 0.9.2.24  2007/03/01 00:10:18  brian
# - update to build process for 2.4 kernels
#
# Revision 0.9.2.23  2007/02/28 11:51:32  brian
# - make sure build directory exists
#
# Revision 0.9.2.22  2007/02/27 08:38:37  brian
# - release corrections for 2.4 kernel builds
#
# Revision 0.9.2.21  2006-12-28 08:32:32  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.20  2006/10/27 22:17:02  brian
# - changes for 2.6.18 kernel
#
# Revision 0.9.2.19  2006/09/29 10:57:46  brian
# - autoconf does not like multiline cache variables
#
# Revision 0.9.2.18  2006/09/29 03:46:16  brian
# - substitute LDFLAGS32
#
# Revision 0.9.2.17  2006/09/29 03:22:38  brian
# - handle flags better
#
# Revision 0.9.2.16  2006/09/27 05:08:41  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.15  2006/09/18 00:33:51  brian
# - added checks for 32bit compatibility libraries
#
# Revision 0.9.2.14  2006/03/14 09:20:47  brian
# - typo
#
# Revision 0.9.2.13  2006/03/14 09:04:11  brian
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
