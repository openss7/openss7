# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: snmp.m4,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008/01/13 21:53:04 $
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
# Last Modified $Date: 2008/01/13 21:53:04 $ by $Author: brian $
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
# Our agents use the older UCD-SNMP 4.2.6 interface including header_complex.
# The NET-SNMP releases of UCD-SNMP are missing some essential headers (used
# inside the package but not installed).  Recent versions of NET-SNMP such as
# 5.2.1 provide compatibility headers, but mangles some of them.  Certinaly
# nobody uses these or never took the time to fix them.  So we do a bunch of
# checks here, and signal the agent code to include fixed headers shipped with
# the package.
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_CHECK_HEADERS], [dnl
    AC_CACHE_CHECK([for snmp UCD compatibility headers], [snmp_cv_headers], [dnl
	AC_MSG_RESULT([checking])
	snmp_cv_headers=yes
	if test :${snmp_cv_headers:-yes} = :yes ; then :;
	    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-config.h], [], [
		snmp_cv_headers=no
		AC_MSG_WARN([cannot find <ucd-snmp/ucd-snmp-config.h>
*** 
*** Cannot find the UCD-SNMP compatibility header:
***
***     <ucd-snmp/ucd-snmp-config.h>
***
*** Perhaps you forgot to load the NET-SNMP development package
*** (e.g.  net-snmp-devel or libsnmp9-dev).  SNMP agents cannot
*** be built.  If this is not what you want, load the
*** development package and rerun configure.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [
])
	fi
	if test :${snmp_cv_headers:-yes} = :yes ; then :;
	    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-includes.h], [], [
		snmp_cv_headers=no
		AC_MSG_FAILURE([cannot find <ucd-snmp/ucd-snmp-includes.h>
*** 
*** Cannot find the UCD-SNMP compatibility header:
***
***	<ucd-snmp/ucd-snmp-includes.h>
***
*** This is peculiar as the <ucd-snmp/ucd-snmp-config.h> header
*** was found.  Your UCD compatibility headers appear completely
*** mangled: cannot proceed.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
])
	fi
	if test :${snmp_cv_headers:-yes} = :yes ; then :;
	    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-agent-includes.h], [], [
		snmp_cv_headers=no
		AC_MSG_FAILURE([cannot find <ucd-snmp/ucd-snmp-agent-includes.h
*** 
*** Cannot find the UCD-SNMP compatibility header:
***
***	<ucd-snmp/ucd-snmp-agent-includes.h>
***
*** This is peculiar as the <ucd-snmp/ucd-snmp-config.h> header
*** and <ucd-snmp/ucd-snmp-includes.h> header were found.  Your
*** UCD compatibility headers appear completely mangled: cannot
*** proceed.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
])
	fi
	AC_MSG_CHECKING([for snmp UCD compatibility headers])
    ])
    AC_CHECK_HEADERS([ucd-snmp/callback.h ucd-snmp/snmp-tc.h ucd-snmp/default_store.h ucd-snmp/snmp_alarm.h],
	[], [], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/ds_agent.h ucd-snmp/util_funcs.h ucd-snmp/header-complex.h ucd-snmp/mib_modules.h net-snmp/agent/mib_modules.h],
	[], [], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
#include <ucd-snmp/default_store.h>
#include <ucd-snmp/snmp_alarm.h>
])
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
	AC_CHECK_LIB([rpmdb], [main], [], [], [-lrpm]) #xxx
	AC_CHECK_LIB([rpm], [main])
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
*** net-snmp or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [-lnetsnmpagent -lnetsnmpmibs])
	    AC_CHECK_LIB([netsnmpagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libnetsnmpagent.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [-lnetsnmpmibs])
	    AC_CHECK_LIB([netsnmpmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libnetsnmpmibs.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ])
	else
	    AC_CHECK_LIB([snmp], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libsnmp.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ])
	    AC_CHECK_LIB([ucdagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libucdagent.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [-lucdmibs], [
int allow_severity = 0;
int deny_severity = 0;
])
	    AC_CHECK_LIB([ucdmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling native SNMP agents requires the native library
*** libucdmibs.  Most likely you need to install the net-snmp or
*** ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [], [
int allow_severity = 0;
int deny_severity = 0;
])
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
# NET-SNMP has some of the worst shared library construction I have ever seen.
# Although we have ELF for so many years now, NET-SNMP cannot build it s
# shared libraries to depend upon the shared libraries against which it was
# compiled (as we do).  The result is having to look for all the dependent
# shared libraries and link them explicitly with the application.
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
	AC_CHECK_LIB32([rpmdb], [main], [], [], [-lrpm]) #xxx
	AC_CHECK_LIB32([rpm], [main])
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
*** net-snmp or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [-lnetsnmpagent -lnetsnmpmibs])
	    AC_CHECK_LIB32([netsnmpagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libnetsnmpagent.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [-lnetsnmpmibs])
	    AC_CHECK_LIB32([netsnmpmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libnetsnmpmibs.  Most likely you need to install the
*** net-snmp or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ])
	else
	    AC_CHECK_LIB32([snmp], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libsnmp.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ])
	    AC_CHECK_LIB32([ucdagent], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libucdagent.  Most likely you need to install the net-snmp
*** or ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [-lucdmibs], [
int allow_severity = 0;
int deny_severity = 0;
])
	    AC_CHECK_LIB32([ucdmibs], [main], [], [dnl
		AC_MSG_ERROR([
*** 
*** Compiling 32-bit SNMP agents requires the 32-bit library
*** libucdmibs.  Most likely you need to install the net-snmp or
*** ucd-snmp runtime package.  Otherwise, pass
*** --without-snmp-agent or --without-snmp to configure.
*** ]) ], [], [
int allow_severity = 0;
int deny_severity = 0;
])
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
    _SNMP_DLMOD_LIBDIRS
    AC_SUBST([snmpdlmoddir])dnl
    AC_SUBST([snmpdlmod32dir])dnl
    AC_SUBST([snmpdlmod64dir])dnl
    AC_SUBST([snmpmibdir])dnl
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
# _SNMP_DLMOD_LIBDIRS
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_DLMOD_LIBDIRS], [dnl
    snmpdlmoddir='${libdir}/snmp/dlmod'
    snmpdlmod32dir='${lib32dir}/snmp/dlmod'
    snmpdlmod64dir='${lib64dir}/snmp/dlmod'
    snmpmibdir='${datarootdir}/snmp/mibs'
])# _SNMP_DLMOD_LIBDIRS
# =============================================================================

# =============================================================================
# _SNMP_
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_], [dnl
])# _SNMP_
# =============================================================================

# =============================================================================
#
# $Log: snmp.m4,v $
# Revision 0.9.2.6  2008/01/13 21:53:04  brian
# - dlmod SNMP agent build and installation
#
# Revision 0.9.2.5  2007/10/18 05:33:30  brian
# - better checking of NET-SNMP
#
# Revision 0.9.2.4  2007/10/15 06:47:49  brian
# - update to SNMP agent build
#
# Revision 0.9.2.3  2007/10/15 01:04:50  brian
# - updated SNMP build
#
# Revision 0.9.2.2  2007/10/13 08:51:41  brian
# - updates for 2.6.22 FC6 kernel and added MIBs
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
