# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: xti.m4,v $ $Name:  $($Revision: 0.9.2.59 $) $Date: 2007/08/12 19:05:32 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/08/12 19:05:32 $ by $Author: brian $
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a NET-SNMP release and
# necessary include directories, libraries, and other configuration for
# compiling and linking SNMP agents to run with the STREAMS package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have net-snmp loaded on the build
# machine to compile agents.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _SNMP
# -----------------------------------------------------------------------------
# Check for the existences of SNMP header files, particulary
# <ucd-snmp/ucd-snmp-config.h>.  SNMP header files are rquired for building
# SNMP agents.  Without SNMP header files, the SNMP agent will no be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP], [dnl
    AC_REQUIRE([_DISTRO])
    AC_REQUIRE([_PERL])
    _SNMP_OPTIONS
    _SNMP_SETUP
    _SNMP_USER
    _SNMP_OUTPUT
])# _SNMP
# =============================================================================

# =============================================================================
# _SNMP_OPTIONS
# -----------------------------------------------------------------------------
# allow the user to specify the header file location.
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_OPTIONS], [dnl
    AC_ARG_WITH([snmp],
		AS_HELP_STRING([--with-snmp=HEADERS],
			       [specify the SNMP header file directory.
				@<:@default=$INCLUDEDIR/ucd-snmp@:>@]),
		[with_snmp="$withval"],
		[with_snmp=''])
])# _SNMP_OPTIONS
# =============================================================================

# =============================================================================
# _SNMP_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_SETUP], [dnl
    _SNMP_CHECK_HEADERS
    _SNMP_CHECK_LIBS
    if test :${dist_cv_32bit_libs:-no} = :yes ; then
	_SNMP_CHECK_LIBS32
    fi
    for snmp_include in $snmp_cv_includes ; do
	SNMP_CPPFLAGS="${SNMP_CPPFLAGS}${SNMP_CPPFLAGS:+ }-I${snmp_include}"
    done
    if test ":${snmp_cv_config:=no}" != :no ; then
	SNMP_CPPFLAGS="${SNMP_CPPFLAGS}${SNMP_CPPFLAGS:+ }-include ${snmp_cv_config}"
    fi
    if test ":${snmp_cv_modversions:-no}" != :no ; then
	SNMP_MODFLAGS="${SNMP_MODFLAGS}${SNMP_MODFLAGS:+ }-include ${snmp_cv_modversions}"
    fi
])# _SNMP_SETUP
# =============================================================================

# =============================================================================
# _SNMP_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_CHECK_HEADERS], [dnl
])# _SNMP_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _SNMP_CHECK_LIBS
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_CHECK_LIBS], [dnl
    AC_CACHE_CHECK([for snmp native libs], [snmp_cv_libs], [dnl
	AC_MSG_RESULT([checking])
	snmp_save_LIBS="$LIBS"
	snmp_save_LDFLAGS="$LDFLAGS"
	LIBS="$perl_cv_ldadd"
	LDFLAGS="$perl_cv_ldflags"

	AC_CHECK_LIB([wrap], [main]) ###
	AC_CHECK_LIB([sensors], [main])
	AC_CHECK_LIB([m], [main])
	AC_CHECK_LIB([crypto], [main])
	AC_CHECK_LIB([z], [main]) ###
	AC_CHECK_LIB([popt], [main]) ###
	AC_CHECK_LIB([rpmio], [main])
	AC_CHECK_LIB([rpm], [main])
	AC_CHECK_LIB([rpmdb], [main]) #xxx
	AC_CHECK_LIB([dl], [main])

	snmp_cv_libs="$LIBS"
	snmp_cv_ldflags="$LDFLAGS"
	LIBS="$snmp_save_LIBS"
	LDFLAGS="$snmp_save_LDFLAGS"
	AC_MSG_CHECKING([for snmp native libs])
    ])
    AC_CACHE_CHECK([for snmp native ldadd], [snmp_cv_ldadd], [dnl
	AC_MSG_RESULT([checking])
	snmp_save_LIBS="$LIBS"
	snmp_save_LDFLAGS="$LDFLAGS"
	LIBS="$snmp_cv_libs"
	LDFLAGS="$snmp_cv_ldflags"

	AC_CHECK_LIB([netsnmp], [main])

	if test ":${ac_cv_lib_netsnmp_main:-no}" = :yes
	then
	    AC_CHECK_LIB([netsnmphelpers], [main], [], [dnl
		AC_MSG_ERROR([
***
*** Compiling native SNMP agents requires the native library
*** libnetsnmphelpers.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.
*** ]) ], [-lnetsnmpagent -lnetsnmpmibs])
	    AC_CHECK_LIB([netsnmpagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libnetsnmpagent.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.
*** ]) ], [-lnetsnmpmibs])
	    AC_CHECK_LIB([netsnmpmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libnetsnmpmibs.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.
*** ]) ])
	else
	    AC_CHECK_LIB([snmp], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libsnmp.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.
*** ]) ])
	    AC_CHECK_LIB([ucdagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libucdagent.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.
*** ]) ])
	    AC_CHECK_LIB([ucdmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libucdmibs.  Most likely you need to install the net-snmp or
*** ucd-snmp runtime package.
*** ]) ])
	fi

	snmp_cv_ldadd="$LIBS"
	snmp_cv_ldflags="$LDFLAGS"
	LIBS="$snmp_save_LIBS"
	LDFLAGS="$snmp_save_LDFLAGS"
	AC_MSG_CHECKING([for snmp native ldadd])
    ])
])# _SNMP_CHECK_LIBS
# =============================================================================

# =============================================================================
# _SNMP_CHECK_LIBS32
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_CHECK_LIBS32], [dnl
    AC_CACHE_CHECK([for snmp 32-bit libs], [snmp_cv_libs32], [dnl
	AC_MSG_RESULT([checking])
	snmp_save_LIBS="$LIBS"
	snmp_save_LDFLAGS="$LDFLAGS"
	LIBS="$perl_cv_ldadd32"
	LDFLAGS="$perl_cv_ldflags32"

	AC_CHECK_LIB32([wrap], [main]) ###
	AC_CHECK_LIB32([sensors], [main])
	AC_CHECK_LIB32([m], [main])
	AC_CHECK_LIB32([crypto], [main])
	AC_CHECK_LIB32([z], [main]) ###
	AC_CHECK_LIB32([popt], [main]) ###
	AC_CHECK_LIB32([rpmio], [main])
	AC_CHECK_LIB32([rpm], [main])
	AC_CHECK_LIB32([rpmdb], [main]) #xxx
	AC_CHECK_LIB32([dl], [main])

	snmp_cv_libs32="$LIBS"
	snmp_cv_ldflags32="$LDFLAGS"
	LIBS="$snmp_save_LIBS"
	LDFLAGS="$snmp_save_LDFLAGS"
	AC_MSG_CHECKING([for snmp 32-bit libs])
    ])
    AC_CACHE_CHECK([for snmp 32-bit ldadd], [snmp_cv_ldadd32], [dnl
	AC_MSG_RESULT([checking])
	snmp_save_LIBS="$LIBS"
	snmp_save_LDFLAGS="$LDFLAGS"
	LIBS="$snmp_cv_libs32"
	LDFLAGS="$snmp_cv_ldflags32"

	AC_CHECK_LIB32([netsnmp], [main])

	if test ":${ac_cv_lib32_netsnmp_main:-no}" = :yes
	then
	    AC_CHECK_LIB32([netsnmphelpers], [main], [], [dnl
		AC_MSG_ERROR([
***
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libnetsnmphelpers.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.
*** ]) ], [-lnetsnmpagent -lnetsnmpmibs])
	    AC_CHECK_LIB32([netsnmpagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libnetsnmpagent.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.
*** ]) ], [-lnetsnmpmibs])
	    AC_CHECK_LIB32([netsnmpmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libnetsnmpmibs.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.
*** ]) ])
	else
	    AC_CHECK_LIB32([snmp], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libsnmp.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.
*** ]) ])
	    AC_CHECK_LIB32([ucdagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libucdagent.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.
*** ]) ])
	    AC_CHECK_LIB32([ucdmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libucdmibs.  Most likely you need to install the net-snmp or
*** ucd-snmp runtime package.
*** ]) ])
	fi

	snmp_cv_ldadd32="$LIBS"
	snmp_cv_ldflags32="$LDFLAGS"
	LIBS="$snmp_save_LIBS"
	LDFLAGS="$snmp_save_LDFLAGS"
	AC_MSG_CHECKING([for snmp 32-bit ldadd])
    ])
])# _SNMP_CHECK_LIBS32
# =============================================================================

# =============================================================================
# _SNMP_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_USER], [dnl
])# _SNMP_USER
# =============================================================================

# =============================================================================
# _SNMP_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_OUTPUT], [dnl
    _SNMP_DEFINES
    AC_SUBST([SNMP_LDADD])dnl
    AC_SUBST([SNMP_LDADD32])dnl
    AC_SUBST([SNMP_LDFLAGS])dnl
    AC_SUBST([SNMP_LDFLAGS32])dnl
])# _SNMP_OUTPUT
# =============================================================================

# =============================================================================
# _SNMP_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_DEFINES], [dnl
    SNMP_LDADD="$snmp_cv_ldadd"
    SNMP_LDADD32="$snmp_cv_ldadd32"
    SNMP_LDFLAGS="$snmp_cv_ldflags"
    SNMP_LDFLAGS32="$snmp_cv_ldflags32"
])# _SNMP_DEFINES
# =============================================================================

# =============================================================================
# _SNMP_
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_], [dnl
])# _SNMP_
# =============================================================================

# =============================================================================
#
# $Log$
# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
