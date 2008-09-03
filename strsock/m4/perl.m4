# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: perl.m4,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008/09/03 06:47:56 $
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
# Last Modified $Date: 2008/09/03 06:47:56 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# This macro file provides checks for perl libraries (primarily for SNMP).  What
# we are looking for is libperl.so or libperl.a as well as DynaLoader.a.  We
# also look for native and 32-bit compatibility libraries required by libperl.
# =============================================================================

# =============================================================================
# _PERL
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL], [dnl
    AC_REQUIRE([_DISTRO])
    _PERL_OPTIONS
    _PERL_SETUP
    _PERL_USER
    _PERL_OUTPUT
])# _PERL
# =============================================================================

# =============================================================================
# _PERL_OPTIONS
# -----------------------------------------------------------------------------
# Allow the user to specify the header file location.
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_OPTIONS], [dnl
    AC_ARG_WITH([perl],
		AS_HELP_STRING([--with-perl=HEADERS],
			       [specify the PERL header file directory.
				@<:@default=$INCLUDEDIR/perl@:>@]),
		[with_perl="$withval"],
		[with_perl=''])
])# _PERL_OPTIONS
# =============================================================================

# =============================================================================
# _PERL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_SETUP], [dnl
    _PERL_CHECK_HEADERS
    _PERL_CHECK_LIBS
    if test :${dist_cv_32bit_libs:-no} = :yes ; then
	_PERL_CHECK_LIBS32
    fi
    for perl_include in $perl_cv_includes ; do
	PERL_CPPFLAGS="${PERL_CPPFLAGS}${PERL_CPPFLAGS:+ }-I${perl_include}"
    done
    if test ":${perl_cv_config:=no}" != :no ; then
	PERL_CPPFLAGS="${PERL_CPPFLAGS}${PERL_CPPFLAGS:+ }-include ${perl_cv_config}"
    fi
    if test ":${perl_cv_modversions:-no}" != :no ; then
	PERL_MODFLAGS="${PERL_MODFLAGS}${PERL_MODFLAGS:+ }-include ${perl_cv_modversions}"
    fi
])# _PERL_SETUP
# =============================================================================

# =============================================================================
# _PERL_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_CHECK_HEADERS], [dnl
])# _PERL_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _PERL_CHECK_LIBS
# -----------------------------------------------------------------------------
# SuSE went and stuck the 64-bit perl libraries in the 32-bit library directory,
# so look there too.
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_CHECK_LIBS], [dnl
    # need to get LDFLAGS with the library path of libperl.so or libperl.a.
    AC_CACHE_CHECK([for perl native ldflags], [perl_cv_ldflags], [dnl
	perl_cv_ldflags=
	eval "perl_search_path=\"
	    ${DESTDIR}${rootdir}${libdir}/perl5/
	    ${DESTDIR}${libdir}/perl5/
	    ${DESTDIR}${rootdir}/usr/lib/perl5/
	    ${DESTDIR}/usr/lib/perl5/\""
	perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for perl_dir in $perl_search_path ; do
	    if test -d "$perl_dir" ; then
		perl_dirs=`find $perl_dir -name 'CORE' -type d | sort -r | uniq`
		for perl_dir in $perl_dirs ; do
		    AC_MSG_CHECKING([for perl native ldflags...  $perl_dir])
		    if test -r $perl_dir/libperl.so -o -r $perl_dir/libperl.a
		    then
			perl_cv_ldflags="${perl_cv_ldflags}${perl_cv_ldflags:+ }-L$perl_dir -Wl,-rpath -Wl,$perl_dir"
			AC_MSG_RESULT([yes])
		    else
			AC_MSG_RESULT([no])
		    fi
		done
	    fi
	done
	AC_MSG_CHECKING([for perl native ldflags])
    ])
    AC_CACHE_CHECK([for perl native libs], [perl_cv_libs], [dnl
	perl_save_LIBS="$LIBS" ; LIBS=
	perl_save_LDFLAGS="$LDFLAGS" ; LDFLAGS="$perl_cv_ldflags"
	AC_MSG_RESULT([checking])
	AC_CHECK_LIB([c], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libc.
*** ]) ])
	AC_CHECK_LIB([pthread], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libpthread.
*** ]) ])
	AC_CHECK_LIB([util], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libutil.
*** ]) ])
	AC_CHECK_LIB([crypt], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libcrypt.
*** ]) ])
	AC_CHECK_LIB([m], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libm.
*** ]) ])
	AC_CHECK_LIB([dl], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libdl.
*** ]) ])
	AC_CHECK_LIB([nsl], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libnsl.
*** ]) ])
	AC_CHECK_LIB([resolv], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libresolv.
*** ]) ])
	AC_CHECK_LIB([perl], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling native embedded perl applications requires the
*** native library libperl.
*** ]) ])
	perl_cv_libs="$LIBS"
	LIBS=$perl_save_LIBS
	LDFLAGS=$perl_save_LDFLAGS
	AC_MSG_CHECKING([for perl native libs])
    ])
    AC_CACHE_CHECK([for perl native ldadd], [perl_cv_ldadd], [dnl
	perl_cv_ldadd=
	eval "perl_search_path=\"
	    ${DESTDIR}${rootdir}${libdir}/perl5/
	    ${DESTDIR}${libdir}/perl5/
	    ${DESTDIR}${rootdir}/usr/lib/perl5/
	    ${DESTDIR}/usr/lib/perl5/\""
	perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for perl_dir in $perl_search_path ; do
	    if test -d "$perl_dir" ; then
		perl_dirs=`find $perl_dir -name 'DynaLoader' -type d | sort -r | uniq`
		for perl_dir in $perl_dirs ; do
		    AC_MSG_CHECKING([for perl native ldadd...  $perl_dir])
		    if test -r $perl_dir/DynaLoader.a
		    then
			perl_cv_ldadd="$perl_dir/DynaLoader.a"
			AC_MSG_RESULT([yes])
			break 2
		    fi
		    AC_MSG_RESULT([no])
		done
	    fi
	done
	perl_cv_ldadd="${perl_cv_ldadd}${perl_cv_ldadd:+ }${perl_cv_libs}"
	AC_MSG_CHECKING([for perl native ldadd])
    ])
])# _PERL_CHECK_LIBS
# =============================================================================

# =============================================================================
# _PERL_CHECK_LIBS32
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_CHECK_LIBS32], [dnl
    # need to get LDFLAGS with the library path of libperl.so or libperl.a.
    AC_CACHE_CHECK([for perl 32-bit ldflags], [perl_cv_ldflags32], [dnl
	perl_cv_ldflags32=-m32
	eval "perl_search_path=\"
	    ${DESTDIR}${rootdir}${lib32dir}/perl5/
	    ${DESTDIR}${lib32dir}/perl5/\""
	perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for perl_dir in $perl_search_path ; do
	    if test -d "$perl_dir" ; then
		perl_dirs=`find $perl_dir -name 'CORE' -type d | sort -r | uniq`
		for perl_dir in $perl_dirs ; do
		    AC_MSG_CHECKING([for perl 32-bit ldflags...  $perl_dir])
		    if test -r $perl_dir/libperl.so -o -r $perl_dir/libperl.a
		    then
			perl_cv_ldflags32="${perl_cv_ldflags32:--m32 }${perl_cv_ldflags32}${perl_cv_ldflags32:+ }-L$perl_dir -Wl,-rpath -Wl,$perl_dir"
			AC_MSG_RESULT([yes])
		    else
			AC_MSG_RESULT([no])
		    fi
		done
	    fi
	done
	AC_MSG_CHECKING([for perl 32-bit ldflags])
    ])
    AC_CACHE_CHECK([for perl 32-bit libs], [perl_cv_libs32], [dnl
	perl_save_LIBS="$LIBS" ; LIBS=
	perl_save_LDFLAGS="$LDFLAGS" ; LDFLAGS="$perl_cv_ldflags32"
	AC_MSG_RESULT([checking])
	AC_CHECK_LIB32([c], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libc.
*** ]) ])
	AC_CHECK_LIB32([pthread], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libpthread.
*** ]) ])
	AC_CHECK_LIB32([util], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libutil.
*** ]) ])
	AC_CHECK_LIB32([crypt], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libcrypt.
*** ]) ])
	AC_CHECK_LIB32([m], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libm.
*** ]) ])
	AC_CHECK_LIB32([dl], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libdl.
*** ]) ])
	AC_CHECK_LIB32([nsl], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libnsl.
*** ]) ])
	AC_CHECK_LIB32([resolv], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libresolv.
*** ]) ])
	AC_CHECK_LIB32([perl], [main], [], [dnl
	    AC_MSG_ERROR([
***
*** Compiling 32-bit embedded perl applications requires the
*** 32-bit library libperl.
*** ]) ])
	perl_cv_libs32="$LIBS"
	LIBS=$perl_save_LIBS
	LDFLAGS=$perl_save_LDFLAGS
	AC_MSG_CHECKING([for perl 32-bit libs])
    ])
    AC_CACHE_CHECK([for perl 32-bit ldadd], [perl_cv_ldadd32], [dnl
	perl_cv_ldadd32=
	eval "perl_search_path=\"
	    ${DESTDIR}${rootdir}${lib32dir}/perl5/
	    ${DESTDIR}${lib32dir}/perl5/\""
	perl_search_path=`echo "$perl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for perl_dir in $perl_search_path ; do
	    if test -d "$perl_dir" ; then
		perl_dirs=`find $perl_dir -name 'DynaLoader' -type d | sort -r | uniq`
		for perl_dir in $perl_dirs ; do
		    AC_MSG_CHECKING([for perl 32-bit ldadd...  $perl_dir])
		    if test -r $perl_dir/DynaLoader.a
		    then
			perl_cv_ldadd32="$perl_dir/DynaLoader.a"
			AC_MSG_RESULT([yes])
			break 2
		    fi
		    AC_MSG_RESULT([no])
		done
	    fi
	done
	perl_cv_ldadd32="${perl_cv_ldadd32}${perl_cv_ldadd32:+ }${perl_cv_libs32}"
	AC_MSG_CHECKING([for perl 32-bit ldadd])
    ])
])# _PERL_CHECK_LIBS32
# =============================================================================

# =============================================================================
# _PERL_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_USER], [dnl
])# _PERL_USER
# =============================================================================

# =============================================================================
# _PERL_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_OUTPUT], [dnl
    _PERL_DEFINES
    AC_SUBST([PERL_LDADD])dnl
    AC_SUBST([PERL_LDFLAGS])dnl
    AC_SUBST([PERL_LDADD32])dnl
    AC_SUBST([PERL_LDFLAGS32])dnl
])# _PERL_OUTPUT
# =============================================================================

# =============================================================================
# _PERL_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_DEFINES], [dnl
    PERL_LDADD="$perl_cv_ldadd"
    PERL_LDADD32="$perl_cv_ldadd32"
    PERL_LDFLAGS="$perl_cv_ldflags"
    PERL_LDFLAGS32="$perl_cv_ldflags32"
])# _PERL_DEFINES
# =============================================================================

# =============================================================================
# _PERL_
# -----------------------------------------------------------------------------
AC_DEFUN([_PERL_], [dnl
])# _PERL_
# =============================================================================

# =============================================================================
#
# $Log: perl.m4,v $
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
