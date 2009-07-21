# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: perl.m4,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2009-07-21 11:06:13 $
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
# Last Modified $Date: 2009-07-21 11:06:13 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# This macro file provides checks for perl libraries (primarily for SNMP).  What
# we are looking for is libperl.so or libperl.a as well as DynaLoader.a.  We
# also look for native libraries required by libperl.
# =============================================================================

# =============================================================================
# _PERL
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL], [dnl
    _PERL_EXTENSIONS
    _PERL_LIBRARIES
])# _PERL
# =============================================================================

# =============================================================================
# _PERL_EXTENSIONS
# -----------------------------------------------------------------------------
# Search for the PERL include directories necessary to build PERL extensions.
# This can be specified to configure by specifying --with-perl=DIRECTORY.  The
# PERL include directories are searched for in the host and then build
# directories.  If we cannot find them, assume that PERL interfaces cannot
# build.
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_EXTENSIONS], [dnl
    AC_CACHE_CHECK([for perl headers], [perl_cv_includedir], [dnl
	AC_ARG_WITH([perl],
	    [AS_HELP_STRING([--with-perl=@<:@HEADERS@:>@],
		[PERL header directory @<:@default=search@:>@])],
	    [], [with_perl=search])
	case "${with_perl:-search}" in
	    (no) perl_cv_includedir=no ;;
	    (yes|search) ;;
	    (*) if test -f "$with_perl/EXTERN.h" ; then perl_cv_includedir ; fi ;;
	esac
	if test -z "$perl_cv_includedir" ; then
	    eval "perl_search_path=\"
		${DESTDIR}${rootdir}${libdir}/perl5
		${DESTDIR}${rootdir}${libdir}/perl
		${DESTDIR}${rootdir}/usr/lib/perl5
		${DESTDIR}${rootdir}/usr/lib/perl
		${DESTDIR}${rootdir}/usr/local/lib/perl5
		${DESTDIR}${rootdir}/usr/local/lib/perl
		${DESTDIR}${libdir}/perl5
		${DESTDIR}${libdir}/perl
		${DESTDIR}/usr/lib/perl5
		${DESTDIR}/usr/lib/perl
		${DESTDIR}/usr/local/lib/perl5
		${DESTDIR}/usr/local/lib/perl\""
	    perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for perl_dir in $perl_search_path ; do
		test -d "$perl_dir" || continue
		AC_MSG_CHECKING([for perl headers... $perl_dir])
		perl_files=`find "$perl_dir" -type f -name 'EXTERN.h' 2>/dev/null | sort -ru`
		for perl_file in $perl_files ; do
		    test -r "$perl_file" || continue
		    perl_dir="${perl_file%/EXTERN.h}"
		    #perl_dir="${perl_dir#$DESTDIR}"
		    #perl_dir="${perl_dir#$rootdir}"
		    perl_cv_includedir="$perl_dir"
		    AC_MSG_RESULT([yes])
		    break 2
		done
		AC_MSG_RESULT([no])
	    done
	    test -n "$perl_cv_includedir" || perl_cv_includedir=no
	fi
	AC_MSG_CHECKING([for perl headers])
    ])
    if test :"${perl_cv_includedir:-no}" = :no ; then
	if test :"${with_perl:-search}" != :no ; then
	    AC_MSG_WARN([
***
*** Configure could not find the PERL extension include file EXTERN.h.
*** This file is required to compile PERL extension libraries.  This
*** file is part of the 'perl' development package which is not always
*** loaded on all distributions.  Use the following commands to obtain
*** the 'perl' development package:
***
*** Debian 4.0:  'apt-get install perl'
*** Ubuntu 8.04: 'apt-get install perl'
*** CentOS 5.x:  'yum install perl'
*** openSUSE 11: 'zypper install perl'
*** SLES 10:     'zypper install perl'
*** RedHat 7.2:  'rpm -i perl-5.6.1-36.1.73'
***
*** Otherwise, specify the location of the PERL headers with the
*** --with-perl=DIRECTORY argument, or --without-perl, on the next run
*** of configure.  Continuing under the assumption that --without-perl
*** was intended.
***])
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"__without_perl --without-perl\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-perl'"
	    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-perl'"
	    with_perl=no
	fi
	perlincludedir=
    else
	perlincludedir="$perl_cv_includedir"
    fi
    AM_CONDITIONAL([WITH_PERL], [test :"${with_perl:-search}" != :no])
    AC_SUBST([perlincludedir])dnl
    AC_CACHE_CHECK([for perl cppflags], [perl_cv_cppflags], [dnl
	if test -n "$perlincludedir" ; then
	    perl_cv_cppflags="-I$perlincludedir"
	else
	    perl_cv_cppflags=
	fi
    ])
    PERL_CPPFLAGS="$perl_cv_cppflags"
    AC_SUBST([PERL_CPPFLAGS])
])# _PERL_EXTENSIONS
# =============================================================================

AC_DEFUN([_PERL_LIB_ERROR], [dnl
    AS_REQUIRE_SHELL_FN([perl_lib_error], [dnl
    cat <<EOF

***
*** Compiling embedded perl applications requires the library
*** [$]1.
***
EOF
    ])dnl
    ac_msg=`perl_lib_error $1`
    AC_MSG_FAILURE([$ac_msg])
])

# =============================================================================
# _PERL_LIBRARIES
# -----------------------------------------------------------------------------
# SuSE went and stuck the 64-bit perl libraries in the 32-bit library directory,
# so look there too.  Note that Debian puts libperl.so in libdir.  This
# procedure does not need to always find the library, it is ok to not add and
# LDFLAGS when the library is where ld can find it.
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_LIBRARIES], [dnl
    # need to get LDFLAGS with the library path of libperl.so or libperl.a.
    AC_CACHE_CHECK([for perl ldflags], [perl_cv_ldflags], [dnl
	perl_cv_ldflags=
	eval "perl_search_path=\"
	    ${DESTDIR}${rootdir}${libdir}
	    ${DESTDIR}${rootdir}/usr/lib
	    ${DESTDIR}${rootdir}/usr/local/lib
	    ${DESTDIR}${libdir}
	    ${DESTDIR}/usr/lib
	    ${DESTDIR}/usr/local/lib\""
	perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	case "$host_cpu" in
	    (*64) perl_targ=elf64 ;;
	    (*)   perl_targ=elf32 ;;
	esac
	AC_MSG_RESULT([searching])
	for perl_dir in $perl_search_path ; do
	    test -d "$perl_dir" || continue
	    perl_libs=`find $perl_dir -name 'libperl.so' -o -name 'libperl.a' 2>/dev/null | sort -ru`
	    for perl_lib in $perl_libs ; do
		test -r "$perl_lib" || continue
		AC_MSG_CHECKING([for perl ldflags... $perl_lib])
		if (${OBJDUMP:-objdump} -f "$perl_lib" | grep $perl_targ >/dev/null) 2>/dev/null
		then
		    perl_dir="${perl_lib%/libperl.*}"
		    perl_dir="${perl_dir#$DESTDIR}"
		    perl_dir="${perl_dir#$rootdir}"
		    perl_cv_ldflags="${perl_cv_ldflags:+$perl_cv_ldflags }-L$perl_dir -Wl,-rpath -Wl,$perl_dir"
		    AC_MSG_RESULT([yes])
		    break 2
		fi
		AC_MSG_RESULT([no])
	    done
	done
	AC_MSG_CHECKING([for perl ldflags])
    ])
    PERL_LDFLAGS="$perl_cv_ldflags"
    AC_SUBST([PERL_LDFLAGS])dnl
    AC_CACHE_CHECK([for perl libs], [perl_cv_libs], [dnl
	perl_save_LIBS="$LIBS" ; LIBS=
	perl_save_LDFLAGS="$LDFLAGS" ; LDFLAGS="$perl_cv_ldflags"
	AC_MSG_RESULT([checking])
	AC_CHECK_LIB([c], [main], [],
	    [_PERL_LIB_ERROR([libc])])
	AC_CHECK_LIB([pthread], [main], [],
	    [_PERL_LIB_ERROR([libpthread])])
	AC_CHECK_LIB([util], [main], [],
	    [_PERL_LIB_ERROR([libutil])])
	AC_CHECK_LIB([crypt], [main], [],
	    [_PERL_LIB_ERROR([libcrypt])])
	AC_CHECK_LIB([m], [main], [],
	    [_PERL_LIB_ERROR([libm])])
	AC_CHECK_LIB([dl], [main], [],
	    [_PERL_LIB_ERROR([libdl])])
	AC_CHECK_LIB([nsl], [main], [],
	    [_PERL_LIB_ERROR([libnsl])])
	AC_CHECK_LIB([resolv], [main], [],
	    [_PERL_LIB_ERROR([libresolv])])
	AC_CHECK_LIB([perl], [main], [],
	    [_PERL_LIB_ERROR([libperl])])
	perl_cv_libs="$LIBS"
	LIBS=$perl_save_LIBS
	LDFLAGS=$perl_save_LDFLAGS
	AC_MSG_CHECKING([for perl libs])
    ])
    AC_CACHE_CHECK([for perl ldadd], [perl_cv_ldadd], [dnl
	perl_cv_ldadd=
	eval "perl_search_path=\"
	    ${DESTDIR}${rootdir}${libdir}
	    ${DESTDIR}${rootdir}/usr/lib
	    ${DESTDIR}${rootdir}/usr/local/lib
	    ${DESTDIR}${libdir}
	    ${DESTDIR}/usr/lib
	    ${DESTDIR}/usr/local/lib\""
	perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	case "$host_cpu" in
	    (*64) perl_targ=elf64 ;;
	    (*)   perl_targ=elf32 ;;
	esac
	AC_MSG_RESULT([searching])
	for perl_dir in $perl_search_path ; do
	    test -d "$perl_dir" || continue
	    perl_libs=`find $perl_dir -name 'DynaLoader.so' -o -name 'DynaLoader.a' 2>/dev/null | sort -ru`
	    for perl_lib in $perl_libs ; do
		test -r "$perl_lib" || continue
		AC_MSG_CHECKING([for perl ldadd... $perl_lib])
		if (${OBJDUMP:-objdump} -f "$perl_lib" | grep $perl_targ >/dev/null) 2>/dev/null
		then
		    perl_cv_ldadd="$perl_lib"
		    AC_MSG_RESULT([yes])
		    break 2
		fi
		AC_MSG_RESULT([no])
	    done
	done
	perl_cv_ldadd="${perl_cv_ldadd}${perl_cv_ldadd:+ }${perl_cv_libs}"
	AC_MSG_CHECKING([for perl ldadd])
    ])
    PERL_LDADD="$perl_cv_ldadd"
    AC_SUBST([PERL_LDADD])dnl
])# _PERL_LIBRARIES
# =============================================================================

# =============================================================================
#
# $Log: perl.m4,v $
# Revision 1.1.2.3  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.2  2009-07-13 07:13:27  brian
# - changes for multiple distro build
#
# Revision 1.1.2.1  2009-06-21 11:06:05  brian
# - added files to new distro
#
# Revision 0.9.2.7  2008-09-14 02:59:07  brian
# - correction
#
# Revision 0.9.2.6  2008/09/13 21:51:07  brian
# - avoid searching same perl directories
#
# Revision 0.9.2.5  2008-09-03 06:58:35  brian
# - fix typo
#
# Revision 0.9.2.4  2008/09/03 06:47:56  brian
# - search available library paths
#
# Revision 0.9.2.3  2008-09-03 05:22:41  brian
# - handle SuSE stupid perl packaging
#
# Revision 0.9.2.2  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.1  2007/10/13 08:13:17  brian
# - added macros for SNMP agents
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
