# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: sigtran.m4,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/03/01 07:17:25 $
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
# $Log: sigtran.m4,v $
# Revision 0.9.2.8  2007/03/01 07:17:25  brian
# - updating common build process
#
# Revision 0.9.2.7  2007/03/01 06:38:15  brian
# - updates to common build process
#
# Revision 0.9.2.6  2007/03/01 01:45:15  brian
# - updating build process
#
# Revision 0.9.2.5  2007/03/01 00:10:18  brian
# - update to build process for 2.4 kernels
#
# Revision 0.9.2.4  2007/02/28 11:51:32  brian
# - make sure build directory exists
#
# Revision 0.9.2.3  2006-12-28 08:32:32  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.2  2006/10/16 11:56:11  brian
# - correct what to search for
#
# Revision 0.9.2.1  2006/10/16 08:28:02  brian
# - added new package discovery macros
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS SIGTRAN
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS SIGTRAN package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have sigtran loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _SIGTRAN
# -----------------------------------------------------------------------------
# Check for the existence of SIGTRAN header files, particularly sigtran/sys/m3ua.h.
# SIGTRAN header files are required for building the SIGTRAN interface.
# Without SIGTRAN header files, the SIGTRAN interface will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _SIGTRAN_OPTIONS
    _SIGTRAN_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_SIGTRAN_KERNEL])
    _SIGTRAN_USER
    _SIGTRAN_OUTPUT
    AC_SUBST([SIGTRAN_CPPFLAGS])dnl
    AC_SUBST([SIGTRAN_MODFLAGS])dnl
    AC_SUBST([SIGTRAN_LDADD])dnl
    AC_SUBST([SIGTRAN_LDADD32])dnl
    AC_SUBST([SIGTRAN_LDFLAGS])dnl
    AC_SUBST([SIGTRAN_LDFLAGS32])dnl
    AC_SUBST([SIGTRAN_MODMAP])dnl
    AC_SUBST([SIGTRAN_SYMVER])dnl
    AC_SUBST([SIGTRAN_MANPATH])dnl
    AC_SUBST([SIGTRAN_VERSION])dnl
])# _SIGTRAN
# =============================================================================

# =============================================================================
# _SIGTRAN_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_OPTIONS], [dnl
    AC_ARG_WITH([sigtran],
		AS_HELP_STRING([--with-sigtran=HEADERS],
			       [specify the SIGTRAN header file directory.
				@<:@default=$INCLUDEDIR/sigtran@:>@]),
		[with_sigtran="$withval"],
		[with_sigtran=''])
])# _SIGTRAN_OPTIONS
# =============================================================================

# =============================================================================
# _SIGTRAN_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_SETUP], [dnl
    _SIGTRAN_CHECK_HEADERS
    for sigtran_include in $sigtran_cv_includes ; do
	SIGTRAN_CPPFLAGS="${SIGTRAN_CPPFLAGS}${SIGTRAN_CPPFLAGS:+ }-I${sigtran_include}"
    done
    if test :"${sigtran_cv_config:-no}" != :no ; then
	SIGTRAN_CPPFLAGS="${SIGTRAN_CPPFLAGS}${SIGTRAN_CPPFLAGS:+ }-include ${sigtran_cv_config}"
    fi
    if test :"${sigtran_cv_modversions:-no}" != :no ; then
	SIGTRAN_MODFLAGS="${SIGTRAN_MODFLAGS}${SIGTRAN_MODFLAGS:+ }-include ${sigtran_cv_modversions}"
    fi
])# _SIGTRAN_SETUP
# =============================================================================

# =============================================================================
# _SIGTRAN_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS SIGTRAN header files.  The
    # package normally requires SIGTRAN header files to compile.
    AC_CACHE_CHECK([for sigtran include directory], [sigtran_cv_includes], [dnl
	sigtran_what="sys/m3ua.h"
	if test :"${with_sigtran:-no}" != :no -a :"${with_sigtran:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_sigtran)])
	    for sigtran_dir in $with_sigtran ; do
		if test -d "$sigtran_dir" ; then
		    AC_MSG_CHECKING([for sigtran include directory... $sigtran_dir])
		    if test -r "$sigtran_dir/$sigtran_what" ; then
			sigtran_cv_includes="$with_sigtran"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${sigtran_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-sigtran="$with_sigtran"
***
*** however, $sigtran_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for sigtran include directory])
	fi
	if test :"${sigtran_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    sigtran_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/sigtran/src/include}"
	    sigtran_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/sigtran/src/include}"
	    if test -d "$sigtran_dir" ; then
		AC_MSG_CHECKING([for sigtran include directory... $sigtran_dir $sigtran_bld])
		if test -d "$sigtran_bld" -a -r "$sigtran_dir/$sigtran_what" ; then
		    sigtran_cv_includes="$sigtran_bld $sigtran_dir"
		    sigtran_cv_ldadd= # "$os7_cv_master_builddir/sigtran/libsigtran.la"
		    sigtran_cv_ldadd32= # "$os7_cv_master_builddir/sigtran/lib32/libsigtran.la"
		    sigtran_cv_modmap="$os7_cv_master_builddir/sigtran/Modules.map"
		    sigtran_cv_symver="$os7_cv_master_builddir/sigtran/Module.symvers"
		    sigtran_cv_manpath="$os7_cv_master_builddir/sigtran/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for sigtran include directory])
	fi
	if test :"${sigtran_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    sigtran_here=`pwd`
	    AC_MSG_RESULT([(searching from $sigtran_here)])
	    for sigtran_dir in \
		$srcdir/sigtran*/src/include \
		$srcdir/../sigtran*/src/include \
		../_build/$srcdir/../../sigtran*/src/include \
		../_build/$srcdir/../../../sigtran*/src/include
	    do
		if test -d "$sigtran_dir" ; then
		    sigtran_bld=`echo $sigtran_dir | sed -e "s|^$srcdir/|$sigtran_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    sigtran_dir=`(cd $sigtran_dir; pwd)`
		    AC_MSG_CHECKING([for sigtran include directory... $sigtran_dir $sigtran_bld])
		    if test -d "$sigtran_bld" -a -r "$sigtran_dir/$sigtran_what" ; then
			sigtran_cv_includes="$sigtran_bld $sigtran_dir"
			sigtran_cv_ldadd= # `echo "$sigtran_bld/../../libsigtran.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sigtran_cv_ldadd32= # `echo "$sigtran_bld/../../lib32/libsigtran.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sigtran_cv_modmap=`echo "$sigtran_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sigtran_cv_symver=`echo "$sigtran_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			sigtran_cv_manpath=`echo "$sigtran_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sigtran include directory])
	fi
	if test :"${sigtran_cv_includes:-no}" = :no ; then
	    # SIGTRAN header files are normally found in the sigtran package now.
	    # They used to be part of the SIGTRAN add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "sigtran_search_path=\"
			${DESTDIR}${includedir}/sigtran
			${DESTDIR}${rootdir}${oldincludedir}/sigtran
			${DESTDIR}${rootdir}/usr/include/sigtran
			${DESTDIR}${rootdir}/usr/local/include/sigtran
			${DESTDIR}${rootdir}/usr/src/sigtran/src/include
			${DESTDIR}${includedir}/ss7
			${DESTDIR}${rootdir}${oldincludedir}/ss7
			${DESTDIR}${rootdir}/usr/include/ss7
			${DESTDIR}${rootdir}/usr/local/include/ss7
			${DESTDIR}${oldincludedir}/sigtran
			${DESTDIR}/usr/include/sigtran
			${DESTDIR}/usr/local/include/sigtran
			${DESTDIR}/usr/src/sigtran/src/include
			${DESTDIR}${oldincludedir}/ss7
			${DESTDIR}/usr/include/ss7
			${DESTDIR}/usr/local/include/ss7\""
		    ;;
		(LfS)
		    eval "sigtran_search_path=\"
			${DESTDIR}${includedir}/sigtran
			${DESTDIR}${rootdir}${oldincludedir}/sigtran
			${DESTDIR}${rootdir}/usr/include/sigtran
			${DESTDIR}${rootdir}/usr/local/include/sigtran
			${DESTDIR}${rootdir}/usr/src/sigtran/src/include
			${DESTDIR}${includedir}/ss7
			${DESTDIR}${rootdir}${oldincludedir}/ss7
			${DESTDIR}${rootdir}/usr/include/ss7
			${DESTDIR}${rootdir}/usr/local/include/ss7
			${DESTDIR}${rootdir}/usr/src/ss7/include
			${DESTDIR}${oldincludedir}/sigtran
			${DESTDIR}/usr/include/sigtran
			${DESTDIR}/usr/local/include/sigtran
			${DESTDIR}/usr/src/sigtran/src/include
			${DESTDIR}${oldincludedir}/ss7
			${DESTDIR}/usr/include/ss7
			${DESTDIR}/usr/local/include/ss7
			${DESTDIR}/usr/src/ss7/include\""
		    ;;
	    esac
	    sigtran_search_path=`echo "$sigtran_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    sigtran_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for sigtran_dir in $sigtran_search_path ; do
		if test -d "$sigtran_dir" ; then
		    AC_MSG_CHECKING([for sigtran include directory... $sigtran_dir])
		    if test -r "$sigtran_dir/$sigtran_what" ; then
			sigtran_cv_includes="$sigtran_dir"
			#sigtran_cv_modmap=
			#sigtran_cv_symver=
			#sigtran_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sigtran include directory])
	fi
    ])
    AC_CACHE_CHECK([for sigtran ldadd native], [sigtran_cv_ldadd], [dnl
	sigtran_cv_ldadd=
	for sigtran_dir in $sigtran_cv_includes ; do
	    if test -f "$sigtran_dir/../../libsigtran.la" ; then
		sigtran_cv_ldadd=`echo "$sigtran_dir/../../libsigtran.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sigtran ldflags], [sigtran_cv_ldflags], [dnl
	sigtran_cv_ldflags=
	if test -z "$sigtran_cv_ldadd" ; then
	    sigtran_cv_ldflags= # '-lsigtran'
	else
	    sigtran_cv_ldflags= # "-L$(dirname $sigtran_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for sigtran ldadd 32-bit], [sigtran_cv_ldadd32], [dnl
	sigtran_cv_ldadd32=
	for sigtran_dir in $sigtran_cv_includes ; do
	    if test -f "$sigtran_dir/../../libsigtran.la" ; then
		sigtran_cv_ldadd32=`echo "$sigtran_dir/../../lib32/libsigtran.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sigtran ldflags 32-bit], [sigtran_cv_ldflags32], [dnl
	sigtran_cv_ldflags32=
	if test -z "$sigtran_cv_ldadd32" ; then
	    sigtran_cv_ldflags32= # '-lsigtran'
	else
	    sigtran_cv_ldflags32= # "-L$(dirname $sigtran_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for sigtran modmap], [sigtran_cv_modmap], [dnl
	sigtran_cv_modmap=
	for sigtran_dir in $sigtran_cv_includes ; do
	    if test -f "$sigtran_dir/../../Modules.map" ; then
		sigtran_cv_modmap=`echo "$sigtran_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sigtran symver], [sigtran_cv_symver], [dnl
	sigtran_cv_symver=
	for sigtran_dir in $sigtran_cv_includes ; do
	    if test -f "$sigtran_dir/../../Module.symvers" ; then
		sigtran_cv_symver=`echo "$sigtran_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for sigtran manpath], [sigtran_cv_manpath], [dnl
	sigtran_cv_manpath=
	for sigtran_dir in $sigtran_cv_includes ; do
	    if test -d "$sigtran_dir/../../doc/man" ; then
		sigtran_cv_manpath=`echo "$sigtran_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${sigtran_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS SIGTRAN include directories.  If
*** you wish to use the STREAMS SIGTRAN package, you will need to specify
*** the location of the STREAMS SIGTRAN (sigtran) include directories with
*** the --with-sigtran=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS SIGTRAN package?  The
*** STREAMS sigtran package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "sigtran-0.9a.3.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for sigtran version], [sigtran_cv_version], [dnl
	sigtran_what="sys/sigtran/version.h"
	sigtran_file=
	if test -n "$sigtran_cv_includes" ; then
	    for sigtran_dir in $sigtran_cv_includes ; do
		# old place for version
		if test -f "$sigtran_dir/$sigtran_what" ; then
		    sigtran_file="$sigtran_dir/$sigtran_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$sigtran_dir/$linux_cv_k_release/$target_cpu/$sigtran_what" ; then
			sigtran_file="$sigtran_dir/$linux_cv_k_release/$target_cpu/$sigtran_what"
			break
		    fi
		fi
	    done
	fi
	if test :"${sigtran_file:-no}" != :no ; then
	    sigtran_cv_version=`grep '#define.*\<SIGTRAN_VERSION\>' $sigtran_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    sigtran_what="sys/sigtran/config.h"
    AC_CACHE_CHECK([for sigtran $sigtran_what], [sigtran_cv_config], [dnl
	sigtran_cv_config=
	if test -n "$sigtran_cv_includes" ; then
	    AC_MSG_RESULT([(searching $sigtran_cv_includes)])
	    for sigtran_dir in $sigtran_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for sigtran $sigtran_what... $sigtran_dir])
		if test -f "$sigtran_dir/$sigtran_what" ; then
		    sigtran_cv_config="$sigtran_dir/$sigtran_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for sigtran $sigtran_what... $sigtran_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$sigtran_dir/$linux_cv_k_release/$target_cpu/$sigtran_what" ; then
			sigtran_cv_config="$sigtran_dir/$linux_cv_k_release/$target_cpu/$sigtran_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for sigtran $sigtran_what])
	fi
    ])
    sigtran_what="sys/sigtran/modversions.h"
    AC_CACHE_CHECK([for sigtran $sigtran_what], [sigtran_cv_modversions], [dnl
	sigtran_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$sigtran_cv_includes" ; then
		AC_MSG_RESULT([(searching $sigtran_cv_includes)])
		for sigtran_dir in $sigtran_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for sigtran $sigtran_what... $sigtran_dir])
		    if test -f "$sigtran_dir/$sigtran_what" ; then
			sigtran_cv_modversions="$sigtran_dir/$sigtran_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for sigtran $sigtran_what... $sigtran_dir/$linux_cv_k_release/$target_cpu])
			if test "$sigtran_dir/$linux_cv_k_release/$target_cpu/$sigtran_what" ; then
			    sigtran_cv_includes="$sigtran_dir/$linux_cv_k_release/$target_cpu $sigtran_cv_includes"
			    sigtran_cv_modversions="$sigtran_dir/$linux_cv_k_release/$target_cpu/$sigtran_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for sigtran $sigtran_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for sigtran added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_sigtran" ; then
	if test :"${sigtran_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_sigtran --with-sigtran\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-sigtran'"
	    AC_MSG_RESULT([--with-sigtran])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_sigtran --without-sigtran\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-sigtran'"
	    AC_MSG_RESULT([--without-sigtran])
	fi
    else
	AC_MSG_RESULT([--with-sigtran="$with_sigtran"])
    fi
])# _SIGTRAN_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _SIGTRAN_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_KERNEL], [dnl
])# _SIGTRAN_KERNEL
# =============================================================================

# =============================================================================
# _SIGTRAN_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_OUTPUT], [dnl
    _SIGTRAN_DEFINES
])# _SIGTRAN_OUTPUT
# =============================================================================

# =============================================================================
# _SIGTRAN_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_DEFINES], [dnl
    if test :"${sigtran_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_SIGTRAN_MODVERSIONS_H], [1], [Define when
	    the STREAMS SIGTRAN release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    SIGTRAN_CPPFLAGS="${SIGTRAN_CPPFLAGS:+ ${SIGTRAN_CPPFLAGS}}"
    SIGTRAN_LDADD="$sigtran_cv_ldadd"
    SIGTRAN_LDADD32="$sigtran_cv_ldadd32"
    SIGTRAN_LDFLAGS="$sigtran_cv_ldflags"
    SIGTRAN_LDFLAGS32="$sigtran_cv_ldflags32"
    SIGTRAN_MODMAP="$sigtran_cv_modmap"
    SIGTRAN_SYMVER="$sigtran_cv_symver"
    SIGTRAN_MANPATH="$sigtran_cv_manpath"
    SIGTRAN_VERSION="$sigtran_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${SIGTRAN_SYMVER:+${MODPOST_INPUTS:+ }${SIGTRAN_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and sigtran for
	that matter.
    ])
])# _SIGTRAN_DEFINES
# =============================================================================

# =============================================================================
# _SIGTRAN_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_USER], [dnl
])# _SIGTRAN_USER
# =============================================================================

# =============================================================================
# _SIGTRAN_
# -----------------------------------------------------------------------------
AC_DEFUN([_SIGTRAN_], [dnl
])# _SIGTRAN_
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
