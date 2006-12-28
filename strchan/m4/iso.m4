# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: iso.m4,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/28 08:32:31 $
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
# Last Modified $Date: 2006/12/28 08:32:31 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: iso.m4,v $
# Revision 0.9.2.4  2006/12/28 08:32:31  brian
# - use cache names for master src and build directories
#
# Revision 0.9.2.3  2006/09/30 08:12:45  brian
# - corrected search start for includes
#
# Revision 0.9.2.2  2006/09/30 07:30:57  brian
# - turn on lib in iso.m4
#
# Revision 0.9.2.1  2006/09/30 07:29:06  brian
# - corrected warning message
# - corrected variable name in xti.m4
# - added iso.m4 to locate striso package
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS ISO
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS ISO package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have striso loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _ISO
# -----------------------------------------------------------------------------
# Check for the existence of ISO header files, particularly sys/tpi.h.
# ISO header files are required for building the TPI interface for ISO.
# Without ISO header files, the TPI interface to ISO will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _ISO_OPTIONS
    _ISO_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_ISO_KERNEL])
    _ISO_USER
    _ISO_OUTPUT
    AC_SUBST([ISO_CPPFLAGS])dnl
    AC_SUBST([ISO_MODFLAGS])dnl
    AC_SUBST([ISO_LDADD])dnl
    AC_SUBST([ISO_LDADD32])dnl
    AC_SUBST([ISO_LDFLAGS])dnl
    AC_SUBST([ISO_LDFLAGS32])dnl
    AC_SUBST([ISO_MODMAP])dnl
    AC_SUBST([ISO_SYMVER])dnl
    AC_SUBST([ISO_MANPATH])dnl
    AC_SUBST([ISO_VERSION])dnl
])# _ISO
# =============================================================================

# =============================================================================
# _ISO_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_OPTIONS], [dnl
    AC_ARG_WITH([iso],
		AS_HELP_STRING([--with-iso=HEADERS],
			       [specify the ISO header file directory.
				@<:@default=$INCLUDEDIR/iso@:>@]),
		[with_iso="$withval"],
		[with_iso=''])
])# _ISO_OPTIONS
# =============================================================================

# =============================================================================
# _ISO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_SETUP], [dnl
    _ISO_CHECK_HEADERS
    for iso_include in $iso_cv_includes ; do
	ISO_CPPFLAGS="${ISO_CPPFLAGS}${ISO_CPPFLAGS:+ }-I${iso_include}"
    done
    if test :"${iso_cv_config:-no}" != :no ; then
	ISO_CPPFLAGS="${ISO_CPPFLAGS}${ISO_CPPFLAGS:+ }-include ${iso_cv_config}"
    fi
    if test :"${iso_cv_modversions:-no}" != :no ; then
	ISO_MODFLAGS="${ISO_MODFLAGS}${ISO_MODFLAGS:+ }-include ${iso_cv_modversions}"
    fi
])# _ISO_SETUP
# =============================================================================

# =============================================================================
# _ISO_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS ISO header files.  The
    # package normally requires ISO header files to compile.
    AC_CACHE_CHECK([for iso include directory], [iso_cv_includes], [dnl
	iso_what="sys/xti_osi.h"
	if test :"${with_iso:-no}" != :no -a :"${with_iso:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_iso)])
	    for iso_dir in $with_iso ; do
		if test -d "$iso_dir" ; then
		    AC_MSG_CHECKING([for iso include directory... $iso_dir])
		    if test -r "$iso_dir/$iso_what" ; then
			iso_cv_includes="$with_iso"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${iso_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
*** 
*** You have specified include directories using:
***
***	    --with-iso="$with_iso"
***
*** however, $iso_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for xit include directory])
	fi
	if test :"${iso_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    iso_search_path="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/striso/src/include} ${os7_cv_master_builddir:+$os7_cv_master_builddir/striso/src/include}"
	    for iso_dir in $iso_search_path ; do
		if test -d "$iso_dir" ; then
		    AC_MSG_CHECKING([for iso include directory... $iso_dir])
		    if test -r "$iso_dir/$iso_what" ; then
			iso_cv_includes="$iso_search_path"
			iso_cv_ldadd= # "$os7_cv_master_builddir/striso/libiso.la"
			iso_cv_ldadd32= # "$os7_cv_master_builddir/striso/lib32/libiso.la"
			iso_cv_modmap= # "$os7_cv_master_builddir/striso/Modules.map"
			iso_cv_symver= # "$os7_cv_master_builddir/striso/Module.symvers"
			iso_cv_manpath="$os7_cv_master_builddir/striso/doc/man"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for iso include directory])
	fi
	if test :"${iso_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    iso_here=`pwd`
	    AC_MSG_RESULT([(searching from $iso_here)])
	    for iso_dir in \
		$srcdir/striso*/src/include \
		$srcdir/../striso*/src/include \
		../_build/$srcdir/../../striso*/src/include \
		../_build/$srcdir/../../../striso*/src/include
	    do
		if test -d "$iso_dir" ; then
		    iso_bld=`echo $iso_dir | sed -e "s|^$srcdir/|$iso_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    iso_dir=`(cd $iso_dir; pwd)`
		    AC_MSG_CHECKING([for iso include directory... $iso_dir])
		    if test -r "$iso_dir/$iso_what" ; then
			iso_cv_includes="$iso_dir $iso_bld"
			iso_cv_ldadd= # `echo "$iso_bld/../../libiso.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			iso_cv_ldadd32= # `echo "$iso_bld/../../lib32/libiso.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			iso_cv_modmap= # `echo "$iso_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			iso_cv_symver= # `echo "$iso_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			iso_cv_manpath=`echo "$iso_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for iso include directory])
	fi
	if test :"${iso_cv_includes:-no}" = :no ; then
	    # ISO header files are normally found in the striso package now.
	    # They used to be part of the XNET add-on package and even older
	    # versions are part of the LiS release packages.
	    case "$streams_cv_package" in
		(LiS)
		    eval "iso_search_path=\"
			${DESTDIR}${includedir}/striso
			${DESTDIR}${rootdir}${oldincludedir}/striso
			${DESTDIR}${rootdir}/usr/include/striso
			${DESTDIR}${rootdir}/usr/local/include/striso
			${DESTDIR}${rootdir}/usr/src/striso/src/include
			${DESTDIR}${oldincludedir}/striso
			${DESTDIR}/usr/include/striso
			${DESTDIR}/usr/local/include/striso
			${DESTDIR}/usr/src/striso/src/include\""
		    ;;
		LfS)
		    eval "iso_search_path=\"
			${DESTDIR}${includedir}/striso
			${DESTDIR}${rootdir}${oldincludedir}/striso
			${DESTDIR}${rootdir}/usr/include/striso
			${DESTDIR}${rootdir}/usr/local/include/striso
			${DESTDIR}${rootdir}/usr/src/striso/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/striso
			${DESTDIR}/usr/include/striso
			${DESTDIR}/usr/local/include/striso
			${DESTDIR}/usr/src/striso/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    iso_search_path=`echo "$iso_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    iso_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for iso_dir in $iso_search_path ; do
		if test -d "$iso_dir" ; then
		    AC_MSG_CHECKING([for iso include directory... $iso_dir])
		    if test -r "$iso_dir/$iso_what" ; then
			iso_cv_includes="$iso_dir"
			iso_cv_modmap=
			iso_cv_symver=
			iso_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for iso include directory])
	fi
    ])
    AC_CACHE_CHECK([for iso ldadd native],[iso_cv_ldadd],[dnl
	for iso_dir in $iso_cv_includes ; do
	    if test -f "$iso_dir/../../libiso.la" ; then
		iso_cv_ldadd=`echo "$iso_dir/../../libiso.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for iso ldflags],[iso_cv_ldflags],[dnl
	if test -z "$iso_cv_ldadd" ; then
	    iso_cv_ldflags="-liso"
	else
	    iso_cv_ldflags="-L$(dirname $iso_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for iso ldadd 32-bit],[iso_cv_ldadd32],[dnl
	for iso_dir in $iso_cv_includes ; do
	    if test -f "$iso_dir/../../lib32/libiso.la" ; then
		iso_cv_ldadd32=`echo "$iso_dir/../../lib32/libiso.la" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for iso ldflags 32-bit],[iso_cv_ldflags32],[dnl
	if test -z "$iso_cv_ldadd32" ; then
	    iso_cv_ldflags32="-liso"
	else
	    iso_cv_ldflags32="-L$(dirname $iso_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for iso modmap],[iso_cv_modmap],[dnl
	for iso_dir in $iso_cv_includes ; do
	    if test -f "$iso_dir/../../Modules.map" ; then
		iso_cv_modmap=`echo "$iso_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for iso symver],[iso_cv_symver],[dnl
	for iso_dir in $iso_cv_includes ; do
	    if test -f "$iso_dir/../../Module.symvers" ; then
		iso_cv_symver=`echo "$iso_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for iso manpath],[iso_cv_manpath],[dnl
	for iso_dir in $iso_cv_includes ; do
	    if test -d "$iso_dir/../../doc/man" ; then
		iso_cv_manpath=`echo "$iso_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    if test :"${iso_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS ISO include directories.  If
*** you wish to use the STREAMS ISO package you will need to specify
*** the location of the STREAMS ISO (striso) include directories with
*** the --with-iso=@<:@DIRECTORY@<:@ DIRECTORY@:>@@:>@ option to
*** ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS ISO package?  The
*** STREAMS striso package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "striso-0.9.2.1.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for iso version], [iso_cv_version], [dnl
	iso_what="sys/striso/version.h"
	iso_file=
	if test -n "$iso_cv_includes" ; then
	    for iso_dir in $iso_cv_includes ; do
		# old place for version
		if test -f "$iso_dir/$iso_what" ; then
		    iso_file="$iso_dir/$iso_what"
		    break
		fi
		# new place for version
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$iso_dir/$linux_cv_k_release/$target_cpu/$iso_what" ; then
			iso_file="$iso_dir/$linux_cv_k_release/$target_cpu/$iso_what"
			break
		    fi
		fi
	    done
	fi
	if test :${iso_file:-no} != :no ; then
	    iso_cv_version=`grep '#define.*\<STRISO_VERSION\>' $iso_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	fi
    ])
    iso_what="sys/config.h"
    AC_CACHE_CHECK([for iso $iso_what], [iso_cv_config], [dnl
	iso_cv_config=
	if test -n "$iso_cv_includes" ; then
	    for iso_dir in $iso_cv_includes ; do
		# old place for config
		if test -f "$iso_dir/$iso_what" ; then
		    iso_cv_config="$iso_dir/$iso_what"
		    break
		fi
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    if test -f "$iso_dir/$linux_cv_k_release/$target_cpu/$iso_what" ; then
			iso_cv_config="$iso_dir/$linux_cv_k_release/$target_cpu/$iso_what"
			break
		    fi
		fi
	    done
	fi
    ])
    iso_what="sys/striso/modversions.h"
    AC_CACHE_CHECK([for iso $iso_what], [iso_cv_modversions], [dnl
	iso_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$iso_cv_includes" ; then
		for iso_dir in $iso_cv_includes ; do
		    # old place for modversions
		    if test -f "$iso_dir/$iso_what" ; then
			iso_cv_modversions="$iso_dir/$iso_what"
			break
		    fi
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			if test "$iso_dir/$linux_cv_k_release/$target_cpu/$iso_what" ; then
			    iso_cv_includes="$iso_dir/$linux_cv_k_release/$target_cpu $iso_cv_includes"
			    iso_cv_modversions="$iso_dir/$linux_cv_k_release/$target_cpu/$iso_what"
			    break
			fi
		    fi
		done
	    fi
	fi
    ])
    AC_MSG_CHECKING([for iso added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_iso" ; then
	if test :"${iso_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_iso --with-iso\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-iso'"
	    AC_MSG_RESULT([--with-iso])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_iso --without-iso\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-iso'"
	    AC_MSG_RESULT([--without-iso])
	fi
    else
	AC_MSG_RESULT([--with-iso="$with_iso"])
    fi
])# _ISO_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _ISO_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_KERNEL], [dnl
])# _ISO_KERNEL
# =============================================================================

# =============================================================================
# _ISO_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_OUTPUT], [dnl
    _ISO_DEFINES
])# _ISO_OUTPUT
# =============================================================================

# =============================================================================
# _ISO_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_DEFINES], [dnl
    if test :"${iso_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_ISO_MODVERSIONS_H], [1], [Define when
	    the STREAMS ISO release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    ISO_CPPFLAGS="${ISO_CPPFLAGS:+ ${ISO_CPPFLAGS}}"
    ISO_LDADD="$iso_cv_ldadd"
    ISO_LDADD32="$iso_cv_ldadd32"
    ISO_LDFLAGS="$iso_cv_ldflags"
    ISO_LDFLAGS32="$iso_cv_ldflags32"
    ISO_MODMAP="$iso_cv_modmap"
    ISO_SYMVER="$iso_cv_symver"
    ISO_MANPATH="$iso_cv_manpath"
    ISO_VERSION="$iso_cv_version"
    MODPOST_INPUT="${MODPOST_INPUTS}${ISO_SYMVER:+${MODPOST_INPUTS:+ }${ISO_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and striso for
	that matter.
    ])
])# _ISO_DEFINES
# =============================================================================

# =============================================================================
# _ISO_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_USER], [dnl
])# _ISO_USER
# =============================================================================

# =============================================================================
# _ISO_
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_], [dnl
])# _ISO_
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
