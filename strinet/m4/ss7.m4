# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:*** fo+=tcqlorn
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: ss7.m4,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/09/29 10:57:46 $
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
# Last Modified $Date: 2006/09/29 10:57:46 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: ss7.m4,v $
# Revision 0.9.2.8  2006/09/29 10:57:46  brian
# - autoconf does not like multiline cache variables
#
# Revision 0.9.2.7  2006/09/29 03:22:38  brian
# - handle flags better
#
# Revision 0.9.2.6  2006/09/27 05:08:41  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.5  2006/09/25 08:56:35  brian
# - corrections by inspection
#
# Revision 0.9.2.3  2006/09/18 00:33:51  brian
# - added checks for 32bit compatibility libraries
#
# Revision 0.9.2.2  2006/03/14 09:20:47  brian
# - typo
#
# Revision 0.9.2.1  2006/03/14 09:04:11  brian
# - syntax consistency, advanced search
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS SS7
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS SS7 package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have strss7 loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _SS7
# -----------------------------------------------------------------------------
# Check for the existence of SS7 header files, particularly ss7/sl.h.
# SS7 header files are required for building the SS7 interface.
# Without SS7 header files, the SS7 interface will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _SS7_OPTIONS
    _SS7_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_SS7_KERNEL])
    _SS7_USER
    _SS7_OUTPUT
    AC_SUBST([SS7_CPPFLAGS])dnl
    AC_SUBST([SS7_MODFLAGS])dnl
    AC_SUBST([SS7_LDADD])dnl
    AC_SUBST([SS7_LDADD32])dnl
    AC_SUBST([SS7_LDFLAGS])dnl
    AC_SUBST([SS7_MODMAP])dnl
    AC_SUBST([SS7_SYMVER])dnl
    AC_SUBST([SS7_MANPATH])dnl
    AC_SUBST([SS7_VERSION])dnl
])# _SS7
# =============================================================================

# =============================================================================
# _SS7_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OPTIONS], [dnl
    AC_ARG_WITH([ss7],
		AS_HELP_STRING([--with-ss7=HEADERS],
			       [specify the SS7 header file directory.
				@<:@default=$INCLUDEDIR/ss7@:>@]),
		[with_ss7="$withval"],
		[with_ss7=''])
])# _SS7_OPTIONS
# =============================================================================

# =============================================================================
# _SS7_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP], [dnl
    _SS7_CHECK_HEADERS
    for ss7_include in $ss7_cv_includes ; do
	SS7_CPPFLAGS="${SS7_CPPFLAGS}${SS7_CPPFLAGS:+ }-I${ss7_include}"
    done
    if test :"${ss7_cv_config:-no}" != :no ; then
	SS7_CPPFLAGS="${SS7_CPPFLAGS}${SS7_CPPFLAGS:+ }-include ${ss7_cv_config}"
    fi
    if test :"${ss7_cv_modversions:-no}" != :no ; then
	SS7_MODFLAGS="${SS7_MODFLAGS}${SS7_MODFLAGS:+ }-include ${ss7_cv_modversions}"
    fi
])# _SS7_SETUP
# =============================================================================

# =============================================================================
# _SS7_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS SS7 header files.  The
    # package normally requires SS7 header files to compile.
    AC_CACHE_CHECK([for ss7 include directory], [ss7_cv_includes], [dnl
	ss7_what="ss7/sl.h"
	if test :"${with_ss7:-no}" != :no -a :"${with_ss7:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_ss7)])
	    for ss7_dir in $with_ss7 ; do
		if test -d "$ss7_dir" ; then
		    AC_MSG_CHECKING([for ss7 include directory... $ss7_dir])
		    if test -r "$ss7_dir/$ss7_what" ; then
			ss7_cv_includes="$with_ss7"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${ss7_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-ss7="$with_ss7"
***
*** however, $ss7_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for ss7 include directory])
	fi
	if test :"${ss7_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $master_srcdir $master_builddir)])
	    ss7_search_path="${master_srcdir:+$master_srcdir/stacks/src/include} ${master_builddir:+$master_builddir/stacks/src/include}"
	    for ss7_dir in $ss7_search_path ; do
		if test -d "$ss7_dir" ; then
		    AC_MSG_CHECKING([for ss7 include directory... $ss7_dir])
		    if test -r "$ss7_dir/$ss7_what" ; then
			ss7_cv_includes="$ss7_search_path"
			ss7_cv_ldadd= # "$master_builddir/stacks/libss7.la"
			ss7_cv_ldadd32= # "$master_builddir/stacks/lib32/libss7.la"
			ss7_cv_modmap= # "$master_builddir/stacks/Modules.map"
			ss7_cv_symver= # "$master_builddir/stacks/Module.symvers"
			ss7_cv_manpath="$master_builddir/stacks/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for ss7 include directory])
	fi
	if test :"${ss7_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    ss7_here=`pwd`
	    AC_MSG_RESULT([(searching from $ss7_here)])
	    for ss7_dir in \
		$srcdir/stacks/src/include \
		$srcdir/strss7*/src/include \
		$srcdir/../stacks/src/include \
		$srcdir/../strss7*/src/include \
		../_build/$srcdir/../../stacks/src/include \
		../_build/$srcdir/../../strss7*/src/include \
		../_build/$srcdir/../../../stacks/src/include \
		../_build/$srcdir/../../../strss7*/src/include
	    do
		if test -d "$ss7_dir" ; then
		    ss7_bld=`echo $ss7_dir | sed -e "s|^$srcdir/|$ss7_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    ss7_dir=`(cd $ss7_dir; pwd)`
		    AC_MSG_CHECKING([for ss7 include directory... $ss7_dir])
		    if test -r "$ss7_dir/$ss7_what" ; then
			ss7_cv_includes="$ss7_dir $ss7_bld"
			ss7_cv_ldadd= # `echo "$ss7_bld/../../libss7.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			ss7_cv_ldadd32= # `echo "$ss7_bld/../../lib32/libss7.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			ss7_cv_modmap= # `echo "$ss7_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			ss7_cv_symver= # `echo "$ss7_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			ss7_cv_manpath=`echo "$ss7_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for ss7 include directory])
	fi
	if test :"${ss7_cv_includes:-no}" = :no ; then
	    # SS7 header files are normally found in the strss7 package now.
	    # They used to be part of the SS7 add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "ss7_search_path=\"
			${DESTDIR}${includedir}/strss7
			${DESTDIR}${rootdir}${oldincludedir}/strss7
			${DESTDIR}${rootdir}/usr/include/strss7
			${DESTDIR}${rootdir}/usr/local/include/strss7
			${DESTDIR}${rootdir}/usr/src/strss7/src/include
			${DESTDIR}${includedir}/LiS/ss7
			${DESTDIR}${rootdir}${oldincludedir}/LiS/ss7
			${DESTDIR}${rootdir}/usr/include/LiS/ss7
			${DESTDIR}${rootdir}/usr/local/include/LiS/ss7
			${DESTDIR}${rootdir}/usr/src/LiS/include/ss7
			${DESTDIR}${includedir}/ss7
			${DESTDIR}${rootdir}${oldincludedir}/ss7
			${DESTDIR}${rootdir}/usr/include/ss7
			${DESTDIR}${rootdir}/usr/local/include/ss7
			${DESTDIR}${oldincludedir}/strss7
			${DESTDIR}/usr/include/strss7
			${DESTDIR}/usr/local/include/strss7
			${DESTDIR}/usr/src/strss7/src/include
			${DESTDIR}${oldincludedir}/LiS/ss7
			${DESTDIR}/usr/include/LiS/ss7
			${DESTDIR}/usr/local/include/LiS/ss7
			${DESTDIR}/usr/src/LiS/include/ss7
			${DESTDIR}${oldincludedir}/ss7
			${DESTDIR}/usr/include/ss7
			${DESTDIR}/usr/local/include/ss7\""
		    ;;
		(LfS)
		    # Linux Fast-STREAMS has always been separate.
		    eval "ss7_search_path=\"
			${DESTDIR}${includedir}/strss7
			${DESTDIR}${rootdir}${oldincludedir}/strss7
			${DESTDIR}${rootdir}/usr/include/strss7
			${DESTDIR}${rootdir}/usr/local/include/strss7
			${DESTDIR}${rootdir}/usr/src/strss7/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/strss7
			${DESTDIR}/usr/include/strss7
			${DESTDIR}/usr/local/include/strss7
			${DESTDIR}/usr/src/strss7/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    ss7_search_path=`echo "$ss7_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    ss7_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for ss7_dir in $ss7_search_path ; do
		if test -d "$ss7_dir" ; then
		    AC_MSG_CHECKING([for ss7 include directory... $ss7_dir])
		    if test -r "$ss7_dir/$ss7_what" ; then
			ss7_cv_includes="$ss7_dir"
			ss7_cv_modmap=
			ss7_cv_symver=
			ss7_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for ss7 include directory])
	fi
    ])
    AC_CACHE_CHECK([for ss7 ldadd native],[ss7_cv_ldadd],[dnl
	for ss7_dir in $ss7_cv_includes ; do
	    if test -f "$ss7_dir/../../libss7.la" ; then
		ss7_cv_ldadd=`echo "$ss7_dir/../../libss7.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for ss7 ldflags],[ss7_cv_ldflags],[dnl
	if test -z "$ss7_cv_ldadd" ; then
	    ss7_cv_ldflags= # '-lss7'
	else
	    ss7_cv_ldflags= # "-L$(dirname $ss7_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for ss7 ldadd 32-bit],[ss7_cv_ldadd32],[dnl
	for ss7_dir in $ss7_cv_includes ; do
	    if test -f "$ss7_dir/../../libss7.la" ; then
		ss7_cv_ldadd32=`echo "$ss7_dir/../../lib32/libss7.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for ss7 ldflags 32-bit],[ss7_cv_ldflags32],[dnl
	if test -z "$ss7_cv_ldadd32" ; then
	    ss7_cv_ldflags32= # '-lss7'
	else
	    ss7_cv_ldflags32= # "-L$(dirname $ss7_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for ss7 modmap],[ss7_cv_modmap],[dnl
	for ss7_dir in $ss7_cv_includes ; do
	    if test -f "$ss7_dir/../../Modules.map" ; then
		ss7_cv_modmap=`echo "$ss7_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for ss7 symver],[ss7_cv_symver],[dnl
	for ss7_dir in $ss7_cv_includes ; do
	    if test -f "$ss7_dir/../../Module.symvers" ; then
		ss7_cv_symver=`echo "$ss7_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for ss7 manpath],[ss7_cv_manpath],[dnl
	for ss7_dir in $ss7_cv_includes ; do
	    if test -d "$ss7_dir/../../doc/man" ; then
		ss7_cv_manpath=`echo "$ss7_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${ss7_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS SS7 include directories.  If
*** you wish to use the STREAMS SS7 package, you will need to specify
*** the location of the STREAMS SS7 (strss7) include directories with
*** the --with-ss7=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS SS7 package?  The
*** STREAMS strss7 package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "strss7-0.9a.3.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for ss7 version], [ss7_cv_version], [dnl
	ss7_what="sys/strss7/version.h"
	ss7_file=
	if test -n "$ss7_cv_includes" ; then
	    for ss7_dir in $ss7_cv_includes ; do
		# old place for version
		if test -f "$ss7_dir/$ss7_what" ; then
		    ss7_file="$ss7_dir/$ss7_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$ss7_dir/$linux_cv_k_release/$target_cpu/$ss7_what" ; then
			ss7_file="$ss7_dir/$linux_cv_k_release/$target_cpu/$ss7_what"
			break
		    fi
		fi
	    done
	fi
	if test :"${ss7_file:-no}" != :no ; then
	    ss7_cv_version=`grep '#define.*\<STRSS7_VERSION\>' $ss7_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    ss7_what="sys/config.h"
    AC_CACHE_CHECK([for ss7 $ss7_what], [ss7_cv_config], [dnl
	ss7_cv_config=
	if test -n "$ss7_cv_includes" ; then
	    for ss7_dir in $ss7_cv_includes ; do
		# old place for config
		if test -f "$ss7_dir/$ss7_what" ; then
		    ss7_cv_config="$ss7_dir/$ss7-what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$ss7_dir/$linux_cv_k_release/$target_cpu/$ss7_what" ; then
			ss7_cv_config="$ss7_dir/$linux_cv_k_release/$target_cpu/$ss7_what"
			break
		    fi
		fi
	    done
	fi
    ])
    ss7_what="sys/strss7/modversions.h"
    AC_CACHE_CHECK([for ss7 $ss7_what], [ss7_cv_modversions], [dnl
	ss7_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$ss7_cv_includes" ; then
		for ss7_dir in $ss7_cv_includes ; do
		    # old place for modversions
		    if test -f "$ss7_dir/$ss7_what" ; then
			ss7_cv_modversions="$ss7_dir/$ss7_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			if test "$ss7_dir/$linux_cv_k_release/$target_cpu/$ss7_what" ; then
			    ss7_cv_includes="$ss7_dir/$linux_cv_k_release/$target_cpu $ss7_cv_includes"
			    ss7_cv_modversions="$ss7_dir/$linux_cv_k_release/$target_cpu/$ss7_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_MSG_CHECKING([for ss7 added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_ss7" ; then
	if test :"${ss7_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_ss7 --with-ss7\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-ss7'"
	    AC_MSG_RESULT([--with-ss7])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_ss7 --without-ss7\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-ss7'"
	    AC_MSG_RESULT([--without-ss7])
	fi
    else
	AC_MSG_RESULT([--with-ss7="$with_ss7"])
    fi
])# _SS7_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _SS7_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_KERNEL], [dnl
])# _SS7_KERNEL
# =============================================================================

# =============================================================================
# _SS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_OUTPUT], [dnl
    _SS7_DEFINES
])# _SS7_OUTPUT
# =============================================================================

# =============================================================================
# _SS7_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_DEFINES], [dnl
    if test :"${ss7_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_SS7_MODVERSIONS_H], [1], [Define when
	    the STREAMS SS7 release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    SS7_CPPFLAGS="${SS7_CPPFLAGS:+ ${SS7_CPPFLAGS}}"
    SS7_LDADD="$ss7_cv_ldadd"
    SS7_LDADD32="$ss7_cv_ldadd32"
    SS7_LDFLAGS="$ss7_cv_ldflags"
    SS7_MODMAP="$ss7_cv_modmap"
    SS7_SYMVER="$ss7_cv_symver"
    SS7_MANPATH="$ss7_cv_manpath"
    SS7_VERSION="$ss7_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${SS7_SYMVER:+${MODPOST_INPUTS:+ }${SS7_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and strss7 for
	that matter.
    ])
])# _SS7_DEFINES
# =============================================================================

# =============================================================================
# _SS7_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_USER], [dnl
])# _SS7_USER
# =============================================================================

# =============================================================================
# _SS7_
# -----------------------------------------------------------------------------
AC_DEFUN([_SS7_], [dnl
])# _SS7_
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
