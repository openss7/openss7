# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: atm.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.3 $) $Date: 2008-12-06 12:13:00 $
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
# Last Modified $Date: 2008-12-06 12:13:00 $ by $Author: brian $
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a STREAMS ATM
# release and necessary include directories and other configuration for
# compiling kernel modules to run with the STREAMS ATM package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have stratm loaded on the build
# machine to compile modules.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _ATM
# -----------------------------------------------------------------------------
# Check for the existence of ATM header files, particularly sys/atm_dlpi.h.
# ATM header files are required for building the ATM interface.
# Without ATM header files, the ATM interface will not be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM], [dnl
    _ATM_CHECK
    if test :"${atm_cv_includes:-no}" = :no ; then
	AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS ATM include directories.  If
*** you wish to use the STREAMS ATM package you will need to specify
*** the location of the STREAMS ATM (stratm) include directories with
*** the --with-atm=@<:@DIRECTORY@:>@ option to ./configure and try again.
***
*** Perhaps you just forgot to load the STREAMS ATM package?  The
*** STREAMS stratm package is available from The OpenSS7 Project
*** download page at http://www.openss7.org/ and comes in a tarball
*** named something like "stratm-0.9.2.1.tar.gz".
*** ])
    fi
])# _ATM
# =============================================================================

# =============================================================================
# _ATM_CHECK
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_CHECK], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _ATM_OPTIONS
    _ATM_SETUP
dnl
dnl Skip kernel checks if not configuring for the kernel (i.e. no _LINUX_KERNEL)
dnl as we do for netperf.
dnl
    m4_ifdef([_LINUX_KERNEL], [_ATM_KERNEL])
    _ATM_USER
    _ATM_OUTPUT
    AC_SUBST([ATM_CPPFLAGS])dnl
    AC_SUBST([ATM_MODFLAGS])dnl
    AC_SUBST([ATM_LDADD])dnl
    AC_SUBST([ATM_LDADD32])dnl
    AC_SUBST([ATM_LDFLAGS])dnl
    AC_SUBST([ATM_LDFLAGS32])dnl
    AC_SUBST([ATM_MODMAP])dnl
    AC_SUBST([ATM_SYMVER])dnl
    AC_SUBST([ATM_MANPATH])dnl
    AC_SUBST([ATM_VERSION])dnl
])# _ATM_CHECK
# =============================================================================

# =============================================================================
# _ATM_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_OPTIONS], [dnl
    AC_ARG_WITH([atm],
	AS_HELP_STRING([--with-atm=HEADERS],
	    [specify the ATM header file directory.  @<:@default=INCLUDEDIR/stratm@:>@]),
	[with_atm="$withval" ; for s in ${!atm_cv_*} ; do eval "unset $s" ; done],
	[with_atm=''])
])# _ATM_OPTIONS
# =============================================================================

# =============================================================================
# _ATM_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_SETUP], [dnl
    _ATM_CHECK_HEADERS
    for atm_include in $atm_cv_includes ; do
	ATM_CPPFLAGS="${ATM_CPPFLAGS}${ATM_CPPFLAGS:+ }-I${atm_include}"
    done
    if test :"${atm_cv_config:-no}" != :no ; then
	ATM_CPPFLAGS="${ATM_CPPFLAGS}${ATM_CPPFLAGS:+ }-include ${atm_cv_config}"
    fi
    if test :"${atm_cv_modversions:-no}" != :no ; then
	ATM_MODFLAGS="${ATM_MODFLAGS}${ATM_MODFLAGS:+ }-include ${atm_cv_modversions}"
    fi
])# _ATM_SETUP
# =============================================================================

# =============================================================================
# _ATM_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS ATM header files.  The
    # package normally requires ATM header files to compile.
    AC_CACHE_CHECK([for atm include directory], [atm_cv_includes], [dnl
	atm_what="sys/atm_dlpi.h"
	if test :"${with_atm:-no}" != :no -a :"${with_atm:-no}" != :yes ; then
	    # First thing to do is to take user specified director(ies)
	    AC_MSG_RESULT([(searching $with_atm)])
	    for atm_dir in $with_atm ; do
		if test -d "$atm_dir" ; then
		    AC_MSG_CHECKING([for atm include directory... $atm_dir])
		    if test -r "$atm_dir/$atm_what" ; then
			atm_cv_includes="$with_atm"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    if test :"${atm_cv_includes:-no}" = :no ; then
		AC_MSG_WARN([
***
*** You have specified include directories using:
***
***	    --with-atm="$with_atm"
***
*** however, $atm_what does not exist in any of the specified
*** directories.  Configure will continue to search other known
*** directories.
*** ])
	    fi
	    AC_MSG_CHECKING([for atm include directory])
	fi
	if test :"${atm_cv_includes:-no}" = :no ; then
	    # The next place to look is under the master source and build
	    # directory, if any.
	    AC_MSG_RESULT([(searching $os7_cv_master_srcdir $os7_cv_master_builddir)])
	    atm_bld="${os7_cv_master_builddir:+$os7_cv_master_builddir/stratm/src/include}"
	    atm_inc="${os7_cv_master_builddir:+$os7_cv_master_builddir/stratm/include}"
	    atm_dir="${os7_cv_master_srcdir:+$os7_cv_master_srcdir/stratm/src/include}"
	    if test -d "$atm_dir" ; then
		AC_MSG_CHECKING([for atm include directory... $atm_dir $atm_bld])
		if test -r "$atm_dir/$atm_what" ; then
		    atm_cv_includes="$atm_inc $atm_bld $atm_dir"
		    atm_cv_ldadd= # "$os7_cv_master_builddir/stratm/libatm.la"
		    atm_cv_ldadd32= # "$os7_cv_master_builddir/stratm/lib32/libatm.la"
		    atm_cv_modversions="$os7_cv_master_builddir/stratm/include/sys/stratm/modversions.h"
		    atm_cv_modmap="$os7_cv_master_builddir/stratm/Modules.map"
		    atm_cv_symver="$os7_cv_master_builddir/stratm/Module.symvers"
		    atm_cv_manpath="$os7_cv_master_builddir/stratm/doc/man"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
	    fi
	    AC_MSG_CHECKING([for atm include directory])
	fi
	if test :"${atm_cv_includes:-no}" = :no ; then
	    # The next place to look now is for a peer package being built under
	    # the same top directory, and then the higher level directory.
	    atm_here=`pwd`
	    AC_MSG_RESULT([(searching from $atm_here)])
	    for atm_dir in \
		$srcdir/stratm*/src/include \
		$srcdir/../stratm*/src/include \
		../_build/$srcdir/../../stratm*/src/include \
		../_build/$srcdir/../../../stratm*/src/include
	    do
		if test -d "$atm_dir" ; then
		    atm_bld=`echo $atm_dir | sed -e "s|^$srcdir/|$atm_here/|;"'s|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    atm_inc=`echo $atm_bld/../../include |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		    atm_dir=`(cd $atm_dir; pwd)`
		    AC_MSG_CHECKING([for atm include directory... $atm_dir $atm_bld])
		    if test -d "$atm_bld" -a -r "$atm_dir/$atm_what" ; then
			atm_cv_includes="$atm_inc $atm_bld $atm_dir"
			atm_cv_ldadd= # `echo "$atm_bld/../../libatm.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			atm_cv_ldadd32= # `echo "$atm_bld/../../lib32/libatm.la" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			atm_cv_modversions=`echo "$atm_inc/sys/stratm/modversions.h" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			atm_cv_modmap=`echo "$atm_bld/../../Modules.map" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			atm_cv_symver=`echo "$atm_bld/../../Module.symvers" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			atm_cv_manpath=`echo "$atm_bld/../../doc/man" |sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for atm include directory])
	fi
	if test :"${atm_cv_includes:-no}" = :no ; then
	    # ATM header files are normally found in the stratm package.
	    case "$streams_cv_package" in
		(LiS)
		    eval "atm_search_path=\"
			${DESTDIR}${includedir}/stratm
			${DESTDIR}${rootdir}${oldincludedir}/stratm
			${DESTDIR}${rootdir}/usr/include/stratm
			${DESTDIR}${rootdir}/usr/local/include/stratm
			${DESTDIR}${rootdir}/usr/src/stratm/src/include
			${DESTDIR}${oldincludedir}/stratm
			${DESTDIR}/usr/include/stratm
			${DESTDIR}/usr/local/include/stratm
			${DESTDIR}/usr/src/stratm/src/include\""
		    ;;
		(LfS)
		    eval "atm_search_path=\"
			${DESTDIR}${includedir}/stratm
			${DESTDIR}${rootdir}${oldincludedir}/stratm
			${DESTDIR}${rootdir}/usr/include/stratm
			${DESTDIR}${rootdir}/usr/local/include/stratm
			${DESTDIR}${rootdir}/usr/src/stratm/src/include
			${DESTDIR}${includedir}/streams
			${DESTDIR}${rootdir}${oldincludedir}/streams
			${DESTDIR}${rootdir}/usr/include/streams
			${DESTDIR}${rootdir}/usr/local/include/streams
			${DESTDIR}${rootdir}/usr/src/streams/include
			${DESTDIR}${oldincludedir}/stratm
			${DESTDIR}/usr/include/stratm
			${DESTDIR}/usr/local/include/stratm
			${DESTDIR}/usr/src/stratm/src/include
			${DESTDIR}${oldincludedir}/streams
			${DESTDIR}/usr/include/streams
			${DESTDIR}/usr/local/include/streams
			${DESTDIR}/usr/src/streams/include\""
		    ;;
	    esac
	    atm_search_path=`echo "$atm_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    atm_cv_includes=
	    AC_MSG_RESULT([(searching)])
	    for atm_dir in $atm_search_path ; do
		if test -d "$atm_dir" ; then
		    AC_MSG_CHECKING([for atm include directory... $atm_dir])
		    if test -r "$atm_dir/$atm_what" ; then
			atm_cv_includes="$atm_dir"
			#atm_cv_ldadd=
			#atm_cv_ldadd32=
			#atm_cv_modmap=
			#atm_cv_symver=
			#atm_cv_manpath=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for atm include directory])
	fi
    ])
    AC_CACHE_CHECK([for atm ldadd native], [atm_cv_ldadd], [dnl
	atm_what="libatm.la"
	atm_cv_ldadd=
	for atm_dir in $atm_cv_includes ; do
	    if test -f "$atm_dir/../../$atm_what" ; then
		atm_cv_ldadd=`echo "$atm_dir/../../$atm_what" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$atm_cv_ldadd" ; then
	    eval "atm_search_path=\"
		${DESTDIR}${rootdir}${libdir}
		${DESTDIR}${libdir}\""
	    atm_search_path=`echo "$atm_search_path" | sed -e 's|\<NONE\>|'$ac_default_prefix'|g;s|//|/|g'`
	    AC_MSG_RESULT([(searching)])
	    for atm_dir in $atm_search_path ; do
		if test -d "$atm_dir" ; then
		    AC_MSG_CHECKING([for atm ldadd native... $atm_dir])
		    if test -r "$atm_dir/$atm_what" ; then
			atm_cv_ldadd="$atm_dir/$atm_what"
			atm_cv_ldflags=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for atm ldadd native])
	fi
    ])
    AC_CACHE_CHECK([for atm ldflags], [atm_cv_ldflags], [dnl
	atm_cv_ldflags=
	if test -z "$atm_cv_ldadd" ; then
	    atm_cv_ldflags= # '-L$(DESTDIR)$(rootdir)$(libdir) -latm'
	else
	    atm_cv_ldflags= # "-L$(dirname $atm_cv_ldadd)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for atm ldadd 32-bit], [atm_cv_ldadd32], [dnl
	atm_what="libatm.la"
	atm_cv_ldadd32=
	for atm_dir in $atm_cv_includes ; do
	    if test -f "$atm_dir/../../lib32/$atm_what" ; then
		atm_cv_ldadd32=`echo "$atm_dir/../../lib32/$atm_what" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
	if test -z "$atm_cv_ldadd32" ; then
	    eval "atm_search_path=\"
		${DESTDIR}${rootdir}${lib32dir}
		${DESTDIR}${lib32dir}\""
	    atm_search_path=`echo "$atm_search_path" | sed -e 's|\<NONE\>|'$ac_default_prefix'|g;s|//|/|g'`
	    AC_MSG_RESULT([(searching)])
	    for atm_dir in $atm_search_path ; do
		if test -d "$atm_dir" ; then
		    AC_MSG_CHECKING([for atm ldadd 32-bit... $atm_dir])
		    if test -r "$atm_dir/$atm_what" ; then
			atm_cv_ldadd32="$atm_dir/$atm_what"
			atm_cv_ldflags32=
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for atm ldadd 32-bit])
	fi
    ])
    AC_CACHE_CHECK([for atm ldflags 32-bit], [atm_cv_ldflags32], [dnl
	atm_cv_ldflags32=
	if test -z "$atm_cv_ldadd32" ; then
	    atm_cv_ldflags32= # '-L$(DESTDIR)$(rootdir)$(lib32dir) -latm'
	else
	    atm_cv_ldflags32= # "-L$(dirname $atm_cv_ldadd32)/.libs/"
	fi
    ])
    AC_CACHE_CHECK([for atm modmap], [atm_cv_modmap], [dnl
	atm_cv_modmap=
	for atm_dir in $atm_cv_includes ; do
	    if test -f "$atm_dir/../../Modules.map" ; then
		atm_cv_modmap=`echo "$atm_dir/../../Modules.map" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for atm symver], [atm_cv_symver], [dnl
	atm_cv_symver=
	for atm_dir in $atm_cv_includes ; do
	    if test -f "$atm_dir/../../Module.symvers" ; then
		atm_cv_symver=`echo "$atm_dir/../../Module.symvers" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for atm manpath], [atm_cv_manpath], [dnl
	atm_cv_manpath=
	for atm_dir in $atm_cv_includes ; do
	    if test -d "$atm_dir/../../doc/man" ; then
		atm_cv_manpath=`echo "$atm_dir/../../doc/man" | sed -e 's|/[[^/]][[^/]]*/\.\./|/|g;s|/[[^/]][[^/]]*/\.\./|/|g;s|/\./|/|g;s|//|/|g'`
		break
	    fi
	done
    ])
    AC_CACHE_CHECK([for atm version], [atm_cv_version], [dnl
	atm_cv_version=
	if test -z "$atm_cv_version" ; then
	    atm_what="sys/stratm/version.h"
	    atm_file=
	    if test -n "$atm_cv_includes" ; then
		for atm_dir in $atm_cv_includes ; do
		    # old place for version
		    if test -f "$atm_dir/$atm_what" ; then
			atm_file="$atm_dir/$atm_what"
			break
		    fi
		    # new place for version
		    if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
			if test -f "$atm_dir/$linux_cv_k_release/$target_cpu/$atm_what" ; then
			    atm_file="$atm_dir/$linux_cv_k_release/$target_cpu/$atm_what"
			    break
			fi
		    fi
		done
	    fi
	    if test :${atm_file:-no} != :no ; then
		atm_cv_version=`grep '#define.*\<STRATM_VERSION\>' $atm_file 2>/dev/null | sed -e 's|^[^"]*"||;s|".*$||'`
	    fi
	fi
	if test -z "$atm_cv_version" ; then
	    atm_epoch=
	    atm_version=
	    atm_package=
	    atm_release=
	    atm_patchlevel=
	    if test -n "$atm_cv_includes" ; then
		for atm_dir in $atm_cv_includes ; do
		    if test -z "$atm_epoch" -a -s "$atm_dir/../.rpmepoch" ; then
			atm_epoch=`cat $atm_dir/../.rpmepoch`
		    fi
		    if test -z "$atm_epoch" -a -s "$atm_dir/../../.rpmepoch" ; then
			atm_epoch=`cat $atm_dir/../../.rpmepoch`
		    fi
		    if test -z "$atm_version" -a -s "$atm_dir/../.version" ; then
			atm_version=`cat $atm_dir/../.version`
		    fi
		    if test -z "$atm_version" -a -s "$atm_dir/../../.version" ; then
			atm_version=`cat $atm_dir/../../.version`
		    fi
		    if test -z "$atm_version" ; then
			if test -z "$atm_version" -a -s "$atm_dir/../configure" ; then
			    atm_version=`grep '^PACKAGE_VERSION=' $atm_dir/../configure | head -1 | sed -e "s,^[[^']]*',,;s,'.*[$],,"`
			fi
			if test -z "$atm_version" -a -s "$atm_dir/../../configure" ; then
			    atm_version=`grep '^PACKAGE_VERSION=' $atm_dir/../../configure | head -1 | sed -e "s,^[[^']]*',,;s,'.*[$],,"`
			fi
			if test -z "$atm_package" -a -s "$atm_dir/../.pkgrelease" ; then
			    atm_package=`cat $atm_dir/../.pkgrelease`
			fi
			if test -z "$atm_package" -a -s "$atm_dir/../../.pkgrelease" ; then
			    atm_package=`cat $atm_dir/../../.pkgrelease`
			fi
			if test -z "$atm_patchlevel" -a -s "$atm_dir/../.pkgpatchlevel" ; then
			    atm_patchlevel=`cat $atm_dir/../.pkgpatchlevel`
			fi
			if test -z "$atm_patchlevel" -a -s "$atm_dir/../../.pkgpatchlevel" ; then
			    atm_patchlevel=`cat $atm_dir/../../.pkgpatchlevel`
			fi
			if test -n "$atm_version" -a -n "$atm_package" ; then
			    atm_version="$atm_version.$atm_package${atm_patchlevel:+.$atm_patchlevel}"
			else
			    atm_version=
			fi
		    fi
		    if test -z "$atm_release" -a -s "$atm_dir/../.rpmrelease" ; then
			atm_release=`cat $atm_dir/../.rpmrelease`
		    fi
		    if test -z "$atm_release" -a -s "$atm_dir/../../.rpmrelease" ; then
			atm_release=`cat $atm_dir/../../.rpmrelease`
		    fi
		done
	    fi
	    if test -n "$atm_epoch" -a -n "$atm_version" -a -n "$atm_release" ; then
		atm_cv_version="$atm_epoch:$atm_version-$atm_release"
	    fi
	fi
    ])
    atm_what="sys/stratm/config.h"
    AC_CACHE_CHECK([for atm $atm_what], [atm_cv_config], [dnl
	atm_cv_config=
	if test -n "$atm_cv_includes" ; then
	    AC_MSG_RESULT([(searching $atm_cv_includes)])
	    for atm_dir in $atm_cv_includes ; do
		# old place for config
		AC_MSG_CHECKING([for atm $atm_what... $atm_dir])
		if test -f "$atm_dir/$atm_what" ; then
		    atm_cv_config="$atm_dir/$atm_what"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
		# new place for config
		if test -n "$linux_cv_k_release" ; then
dnl		    if linux_cv_k_release is not defined (no _LINUX_KERNEL) then
dnl		    this will just not be set
		    AC_MSG_CHECKING([for atm $atm_what... $atm_dir/$linux_cv_k_release/$target_cpu])
		    if test -f "$atm_dir/$linux_cv_k_release/$target_cpu/$atm_what" ; then
			atm_cv_config="$atm_dir/$linux_cv_k_release/$target_cpu/$atm_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		fi
	    done
	    AC_MSG_CHECKING([for atm $atm_what])
	fi
    ])
    atm_what="sys/stratm/modversions.h"
    AC_CACHE_CHECK([for atm $atm_what], [atm_cv_modversions], [dnl
	atm_cv_modversions=
dnl	if linux_cv_k_ko_modules is not defined (no _LINUX_KERNEL) then we
dnl	assume normal objects
	if test :"${linux_cv_k_ko_modules:-no}" = :no ; then
	    if test -n "$atm_cv_includes" ; then
		AC_MSG_RESULT([(searching $atm_cv_includes)])
		for atm_dir in $atm_cv_includes ; do
		    # old place for modversions
		    AC_MSG_CHECKING([for atm $atm_what... $atm_dir])
		    if test -f "$atm_dir/$atm_what" ; then
			atm_cv_modversions="$atm_dir/$atm_what"
			AC_MSG_RESULT([yes])
			break
		    fi
		    AC_MSG_RESULT([no])
		    # new place for modversions
		    if test -n "$linux_cv_k_release" ; then
dnl			if linux_cv_k_release is not defined (no _LINUX_KERNEL)
dnl			then this will just not be set
			AC_MSG_CHECKING([for atm $atm_what... $atm_dir/$linux_cv_k_release/$target_cpu])
			if test -f "$atm_dir/$linux_cv_k_release/$target_cpu/$atm_what" ; then
			    atm_cv_includes="$atm_dir/$linux_cv_k_release/$target_cpu $atm_cv_includes"
			    atm_cv_modversions="$atm_dir/$linux_cv_k_release/$target_cpu/$atm_what"
			    AC_MSG_RESULT([yes])
			    break
			fi
			AC_MSG_RESULT([no])
		    fi
		done
		AC_MSG_CHECKING([for atm $atm_what])
	    fi
	fi
    ])
    AC_MSG_CHECKING([for atm added configure arguments])
dnl Older rpms (particularly those used by SuSE) are too stupid to handle --with
dnl and --without rpmopt syntax, so convert to the equivalent --define syntax.
dnl Also, I don't know that even rpm 4.2 handles --with xxx=yyy properly, so we
dnl use defines.
    if test -z "$with_atm" ; then
	if test :"${atm_cv_includes:-no}" = :no ; then
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_atm --with-atm\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-atm'"
	    AC_MSG_RESULT([--with-atm])
	else
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_atm --without-atm\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-atm'"
	    AC_MSG_RESULT([--without-atm])
	fi
    else
	AC_MSG_RESULT([--with-atm="$with_atm"])
    fi
])# _ATM_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _ATM_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_KERNEL], [dnl
])# _ATM_KERNEL
# =============================================================================

# =============================================================================
# _ATM_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_OUTPUT], [dnl
    _ATM_DEFINES
])# _ATM_OUTPUT
# =============================================================================

# =============================================================================
# _ATM_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_DEFINES], [dnl
    if test :"${atm_cv_modversions:-no}" != :no ; then
	AC_DEFINE_UNQUOTED([HAVE_SYS_ATM_MODVERSIONS_H], [1], [Define when
	    the STREAMS ATM release supports module versions such as
	    the OpenSS7 autoconf releases.])
    fi
    ATM_CPPFLAGS="${ATM_CPPFLAGS:+ }${ATM_CPPFLAGS}"
    ATM_LDADD="$atm_cv_ldadd"
    ATM_LDADD32="$atm_cv_ldadd32"
    ATM_LDFLAGS="$atm_cv_ldflags"
    ATM_LDFLAGS32="$atm_cv_ldflags32"
    ATM_MODMAP="$atm_cv_modmap"
    ATM_SYMVER="$atm_cv_symver"
    ATM_MANPATH="$atm_cv_manpath"
    ATM_VERSION="$atm_cv_version"
    MODPOST_INPUTS="${MODPOST_INPUTS}${ATM_SYMVER:+${MODPOST_INPUTS:+ }${ATM_SYMVER}}"
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
	Define for SuSv3.  This is necessary for LiS and LfS and stratm for
	that matter.
    ])
])# _ATM_DEFINES
# =============================================================================

# =============================================================================
# _ATM_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_USER], [dnl
])# _ATM_USER
# =============================================================================

# =============================================================================
# _ATM_
# -----------------------------------------------------------------------------
AC_DEFUN([_ATM_], [dnl
])# _ATM_
# =============================================================================

# =============================================================================
#
# $Log: atm.m4,v $
# Revision 0.9.2.3  2008-12-06 12:13:00  brian
# - more corrections
#
# Revision 0.9.2.2  2008-12-06 12:06:35  brian
# - correct
#
# Revision 0.9.2.1  2008-12-06 09:58:17  brian
# - added atm package check file
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
