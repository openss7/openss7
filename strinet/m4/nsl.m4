# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: nsl.m4,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/09/27 05:08:41 $
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
# Last Modified $Date: 2006/09/27 05:08:41 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: nsl.m4,v $
# Revision 0.9.2.2  2006/09/27 05:08:41  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.1  2006/09/25 08:38:20  brian
# - added m4 files for locating NSL and SOCK
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

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS NSL
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS NSL package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strnsl loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _NSL
# -----------------------------------------------------------------------------
# Check for the existence of NSL header files, particularly sys/tpi.h.
# NSL header files are required for building the TPI interface for NSL.
# Without NSL header files, the TPI interface to NSL will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _NSL_OPTIONS
    _NSL_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_NSL_KERNEL])
    _NSL_USER
    _NSL_OUTPUT
    AC_SUBST([NSL_CPPFLAGS])dnl
    AC_SUBST([NSL_MODFLAGS])dnl
    AC_SUBST([NSL_LDADD])dnl
    AC_SUBST([NSL_LDADD32])dnl
    AC_SUBST([NSL_LDFLAGS])dnl
    AC_SUBST([NSL_MODMAP])dnl
    AC_SUBST([NSL_SYMVER])dnl
    AC_SUBST([NSL_MANPATH])dnl
    AC_SUBST([NSL_VERSION])dnl
])# _NSL
# =============================================================================

# =============================================================================
# _NSL_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_OPTIONS], [dnl
    AC_ARG_WITH([nsl],
		AS_HELP_STRING([--with-nsl=HEADERS],
			       [specify the NSL header file directory.
				@<:@default=$INCLUDEDIR/nsl@:>@]),
		[with_nsl="$withval"],
		[with_nsl=''])
])# _NSL_OPTIONS
# =============================================================================

# =============================================================================
# _NSL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_SETUP], [dnl
    _NSL_CHECK_HEADERS
    for nsl_include in $nsl_cv_includes ; do
	NSL_CPPFLAGS="${NSL_CPPFLAGS}${NSL_CPPFLAGS:+ }-I${nsl_include}"
    done
    if test :"${nsl_cv_config:-no}" != :no ; then
	NSL_CPPFLAGS="${NSL_CPPFLAGS}${NSL_CPPFLAGS:+ }-include ${nsl_cv_config}"
    fi
    if test :"${nsl_cv_modversions:-no}" != :no ; then
	NSL_MODFLAGS="${NSL_MODFLAGS}${NSL_MODFLAGS:+ }-include ${nsl_cv_modversions}"
    fi
])# _NSL_SETUP
# =============================================================================

# =============================================================================
# _NSL_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS NSL header files.  The
    # package normally requires NSL header files to compile.
    AC_CACHE_CHECK([for nsl include directory], [nsl_cv_includes], [dnl
	nsl_what="netconfig.h"
	if test :"${with_nsl:-no}" != :no -a :"${with_nsl:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_nsl)])
	    for nsl_dir in $with_nsl ; do
		if test -d "$nsl_dir" ; then
		    AC_MSG_CHECKING([for nsl include directory... $nsl_dir])
		    if test -r "$nsl_dir/$nsl_what" ; then
			nsl_cv_includes="$with_nsl"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${nsl_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-nsl="$with_nsl"
***
*** however, $nsl_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for xit include directory])
	fi
	if test :"${nsl_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $master_srcdir $master_builddir)])
	    nsl_search_path="
		${master_srcdir:+$master_srcdir/strnsl/src/include}
		${master_builddir:+$master_builddir/strnsl/src/include}"
	    for nsl_dir in $nsl_search_path ; do
		if test -d "$nsl_dir" ; then
		    AC_MSG_CHECKING([for nsl include directory... $nsl_dir])
		    if test -r "$nsl_dir/$nsl_what" ; then
			nsl_cv_includes="$nsl_search_path"
			nsl_cv_ldadd="$master_builddir/strnsl/libxnsl.la"
			nsl_cv_ldadd32="$master_builddir/strnsl/lib32/libxnsl.la"
			nsl_cv_ldflags=
			nsl_cv_modmap= # "$master_builddir/strnsl/Modules.map"
			nsl_cv_symver= # "$master_builddir/strnsl/Module.symvers"
			nsl_cv_manpath="$master_builddir/strnsl/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for nsl include directory])
	fi
	if test :"${nsl_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    nsl_here=`pwd`
	    AC_MSG_RESULT([(searching from $nsl_here)])
	    for nsl_dir in \
		$srcdir/strnsl*/src/include \
		$srcdir/../strnsl*/src/include \
		../_build/$srcdir/../../strnsl*/src/include \
		../_build/$srcdir/../../../strnsl*/src/include
	    do
		if test -d "$nsl_dir" ; then
		    nsl_bld=`echo $nsl_dir | sed -e "s|^$srcdir/|$nsl_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    nsl_dir=`(cd $nsl_dir; pwd)`
		    AC_MSG_CHECKING([for nsl include directory... $nsl_dir])
		    if test -r "$nsl_dir/$nsl_what" ; then
			nsl_cv_includes="$nsl_dir $nsl_bld"
			nsl_cv_ldadd=`echo "$nsl_bld/../../libxnsl.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			nsl_cv_ldadd32=`echo "$nsl_bld/../../lib32/libxnsl.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			nsl_cv_ldflags=
			nsl_cv_modmap= # `echo "$nsl_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			nsl_cv_symver= # `echo "$nsl_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			nsl_cv_manpath=`echo "$nsl_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for nsl include directory])
	fi
	if test :"${nsl_cv_includes:-no}" = :no ; then
	    # NSL header files are normally found in the strnsl package now.
	    # They used to be part of the NSL add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "nsl_search_path=\"
			${DESTDIR}${includedir}/strnsl
			${DESTDIR}${rootdir}${oldincludedir}/strnsl
			${DESTDIR}${rootdir}/usr/include/strnsl
			${DESTDIR}${rootdir}/usr/local/include/strnsl
			${DESTDIR}${rootdir}/usr/src/strnsl/src/include
			${DESTDIR}${includedir}/LiS/nsl
			${DESTDIR}${rootdir}${oldincludedir}/LiS/nsl
			${DESTDIR}${rootdir}/usr/include/LiS/nsl
			${DESTDIR}${rootdir}/usr/local/include/LiS/nsl
			${DESTDIR}${rootdir}/usr/src/LiS/include/nsl
			${DESTDIR}${includedir}/nsl
			${DESTDIR}${rootdir}${oldincludedir}/nsl
			${DESTDIR}${rootdir}/usr/include/nsl
			${DESTDIR}${rootdir}/usr/local/include/nsl
			${DESTDIR}${oldincludedir}/strnsl
			${DESTDIR}/usr/include/strnsl
			${DESTDIR}/usr/local/include/strnsl
			${DESTDIR}/usr/src/strnsl/src/include
			${DESTDIR}${oldincludedir}/LiS/nsl
			${DESTDIR}/usr/include/LiS/nsl
			${DESTDIR}/usr/local/include/LiS/nsl
			${DESTDIR}/usr/src/LiS/include/nsl
			${DESTDIR}${oldincludedir}/nsl
			${DESTDIR}/usr/include/nsl
			${DESTDIR}/usr/local/include/nsl\""
		    ;;
		LfS)
		    eval "nsl_search_path=\"
			${DESTDIR}${includedir}/strnsl
			${DESTDIR}${rootdir}${oldincludedir}/strnsl
			${DESTDIR}${rootdir}/usr/include/strnsl
			${DESTDIR}${rootdir}/usr/local/include/strnsl
			${DESTDIR}${rootdir}/usr/src/strnsl/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strnsl
			${DESTDIR}/usr/include/strnsl
			${DESTDIR}/usr/local/include/strnsl
			${DESTDIR}/usr/src/strnsl/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    nsl_search_path=`echo "$nsl_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    nsl_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for nsl_dir in $nsl_search_path ; do
		if test -d "$nsl_dir" ; then
		    AC_MSG_CHECKING([for nsl include directory... $nsl_dir])
		    if test -r "$nsl_dir/$nsl_what" ; then
			nsl_cv_includes="$nsl_dir"
			nsl_cv_ldadd=
			nsl_cv_ldadd32=
			nsl_cv_ldflags="-lxnsl"
			nsl_cv_modmap=
			nsl_cv_symver=
			nsl_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for nsl include directory])
	fi
    ])
    AC_CACHE_CHECK([for nsl ldadd native],[nsl_cv_ldadd],[dnl
	for nsl_dir in $nsl_cv_includes ; do
	    if test -f "$nsl_dir/../../libxnsl.la" ; then
		nsl_cv_ldadd=`echo "$nsl_dir/../../libxnsl.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$nsl_cv_ldadd" ; then
	    nsl_cv_ldadd=
	    nsl_cv_ldflags="-lxnsl"
	fi
    ])
    AC_CACHE_CHECK([for nsl ldadd 32-bit],[nsl_cv_ldadd32],[dnl
	for nsl_dir in $nsl_cv_includes ; do
	    if test -f "$nsl_dir/../../lib32/libxnsl.la" ; then
		nsl_cv_ldadd32=`echo "$nsl_dir/../../lib32/libxnsl.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$nsl_cv_ldadd32" ; then
	    nsl_cv_ldadd32=
	fi
    ])
    AC_CACHE_CHECK([for nsl ldflags],[nsl_cv_ldflags],[dnl
	if test -z "$nsl_cv_ldadd$nsl_cv_ldadd32" ; then
	    nsl_cv_ldflags="-lxnsl"
	else
	    nsl_cv_ldflags=
	fi
    ])
    AC_CACHE_CHECK([for nsl modmap],[nsl_cv_modmap],[dnl
	for nsl_dir in $nsl_cv_includes ; do
	    if test -f "$nsl_dir/../../Modules.map" ; then
		nsl_cv_modmap=`echo "$nsl_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for nsl symver],[nsl_cv_symver],[dnl
	for nsl_dir in $nsl_cv_includes ; do
	    if test -f "$nsl_dir/../../Module.symvers" ; then
		nsl_cv_symver=`echo "$nsl_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for nsl manpath],[nsl_cv_manpath],[dnl
	for nsl_dir in $nsl_cv_includes ; do
	    if test -d "$nsl_dir/../../doc/man" ; then
		nsl_cv_manpath=`echo "$nsl_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${nsl_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS NSL include directories.  If
*** you wish to use the STREAMS NSL package you will need to specify
*** the location of the STREAMS NSL (strnsl) include directories with
*** the --with-nsl=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS XNS package?  The
*** STREAMS strxns package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strnsl-0.9.2.1.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for nsl version], [nsl_cv_version], [dnl
	nsl_what="sys/strnsl/version.h"
	nsl_file=
	if test -n "$nsl_cv_includes" ; then
	    for nsl_dir in $nsl_cv_includes ; do
		# old place for version
		if test -f "$nsl_dir/$nsl_what" ; then
		    nsl_file="$nsl_dir/$nsl_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$nsl_dir/$linux_cv_k_release/$target_cpu/$nsl_what" ; then
			nsl_file="$nsl_dir/$linux_cv_k_release/$target_cpu/$nsl_what"
			break
		    fi
		fi
	    done
	fi
	if test :${nsl_file:-no} != :no ; then
	    nsl_cv_version=`grep '#define.*\<STRNSL_VERSION\>' $nsl_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    nsl_what="sys/config.h"
    AC_CACHE_CHECK([for nsl $nsl_what], [nsl_cv_config], [dnl
	nsl_cv_config=
	if test -n "$nsl_cv_includes" ; then
	    for nsl_dir in $nsl_cv_includes ; do
		# old place for config
		if test -f "$nsl_dir/$nsl_what" ; then
		    nsl_cv_config="$nsl_dir/$nsl_what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$nsl_dir/$linux_cv_k_release/$target_cpu/$nsl_what" ; then
			nsl_cv_config="$nsl_dir/$linux_cv_k_release/$target_cpu/$nsl_what"
			break
		    fi
		fi
	    done
	fi
    ])
    nsl_what="sys/strnsl/modversions.h"
    AC_CACHE_CHECK([for nsl $nsl_what], [nsl_cv_modversions], [dnl
	nsl_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$nsl_cv_includes" ; then
		for nsl_dir in $nsl_cv_includes ; do
		    # old place for modversions
		    if test -f "$nsl_dir/$nsl_what" ; then
			nsl_cv_modversions="$nsl_dir/$nsl_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			if test "$nsl_dir/$linux_cv_k_release/$target_cpu/$nsl_what" ; then
			    nsl_cv_includes="$nsl_dir/$linux_cv_k_release/$target_cpu $nsl_cv_includes"
			    nsl_cv_modversions="$nsl_dir/$linux_cv_k_release/$target_cpu/$nsl_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_MSG_CHECKING([for nsl added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_nsl" ; then
	if test :"${nsl_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_nsl --with-nsl\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-nsl'"
	    AC_MSG_RESULT([--with-nsl])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_nsl --without-nsl\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-nsl'"
	    AC_MSG_RESULT([--without-nsl])
	fi
    else
	AC_MSG_RESULT([--with-nsl="$with_nsl"])
    fi
])# _NSL_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _NSL_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_KERNEL], [dnl
])# _NSL_KERNEL
# =============================================================================

# =============================================================================
# _NSL_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_OUTPUT], [dnl
    _NSL_DEFINES
])# _NSL_OUTPUT
# =============================================================================

# =============================================================================
# _NSL_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_DEFINES], [dnl
    if test :"${nsl_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_NSL_MODVERSIONS_H], [1], [Define when
	    the STREAMS NSL release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    NSL_CPPFLAGS="${NSL_CPPFLAGS:+ ${NSL_CPPFLAGS}}"
    NSL_LDADD="$nsl_cv_ldadd"
    NSL_LDADD32="$nsl_cv_ldadd32"
    NSL_LDFLAGS="$nsl_cv_ldflags"
    NSL_MODMAP="$nsl_cv_modmap"
    NSL_SYMVER="$nsl_cv_symver"
    NSL_MANPATH="$nsl_cv_manpath"
    NSL_VERSION="$nsl_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${NSL_SYMVER:+${MODPOST_INPUTS:+ }${NSL_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strnsl for
	that matter.
    ])
])# _NSL_DEFINES
# =============================================================================

# =============================================================================
# _NSL_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_USER], [dnl
])# _NSL_USER
# =============================================================================

# =============================================================================
# _NSL_
# -----------------------------------------------------------------------------
AC_DEFUN([_NSL_], [dnl
])# _NSL_
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
