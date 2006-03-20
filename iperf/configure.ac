# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# =============================================================================

dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
AC_REVISION([$Id])
AC_PREREQ(2.59)
AC_INIT([Iperf],[2.0],[bugs@openss7.org],[iperf])

_OPENSS7_DIRCHANGE

AC_CONFIG_SRCDIR([iperf.lsm.in])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

_DISTRO

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE
dnl
dnl make --enable-maintainer-mode imply --enable-dependency-tracking to avoid typing
dnl
if test :${USE_MAINTAINER_MODE:-yes} = :yes ; then
	enable_dependency_tracking="${enable_dependency_tracking:-yes}"
fi

AC_GNU_SOURCE

AC_ARG_ENABLE(ipv6, AS_HELP_STRING([--disable-ipv6],
	    [disable ipv6 support (default is autodetect)]), 
	ac_cv_have_ipv6=$enable_ipv6,)

AC_ARG_ENABLE(multicast, AS_HELP_STRING([--disable-multicast],
	    [disable multicast support (default is autodetect)]),
	ac_cv_multicast=$enable_multicast,)

AC_ARG_ENABLE(threads, AS_HELP_STRING([--disable-threads],
	    [disable thread support (default is autodetect)]))

AC_ARG_ENABLE(debuginfo, AS_HELP_STRING([--enable-debuginfo],
	    [enable debugging info for sockets (default is no)]),
	enable_debuginfo=$enableval,
	enable_debuginfo=no)

AC_ARG_ENABLE(web100, AS_HELP_STRING([--disable-web100],
	    [disable web100 support (default is autodetect)]))


dnl
dnl Checks for programs.
dnl
AC_PROG_CXX
dnl CXXFLAGS=`echo " $CXXFLAGS " | sed -e "s/ -g / /"` # do not want it implicitly
AC_PROG_CC
dnl CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want it implicitly
dnl AC_ISC_POSIX
dnl AC_PROG_INSTALL
dnl AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_LANG(C)
dnl AC_CANONICAL_HOST

dnl
dnl Checks for libraries.
dnl
dnl check for -lpthread
 
if test "$enable_threads" != no; then
  ACX_PTHREAD()
    if test "$acx_pthread_ok" = yes; then
      AC_DEFINE([HAVE_POSIX_THREAD], 1, [Define for threads.])
      AC_DEFINE([_REENTRANT], 1, [Define for threads.])
    fi
fi

dnl check for -lnsl, -lsocket
AC_CHECK_FUNC(gethostbyname,,AC_CHECK_LIB(nsl, gethostbyname))
AC_CHECK_FUNC(socket,,AC_CHECK_LIB(socket, socket))

dnl
dnl Checks for header files.
dnl
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h libintl.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/socket.h sys/time.h syslog.h unistd.h])

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPES(ssize_t,,AC_DEFINE_UNQUOTED(ssize_t, int, [Define ssize_t type.]))
AC_HEADER_TIME
AC_STRUCT_TM

dnl these intXX_t and u_intXX_t need to be defined to be the right size.

AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(unsigned long long)

AH_TEMPLATE(int16_t)
AH_TEMPLATE(int32_t)
AH_TEMPLATE(int64_t)
AH_TEMPLATE(u_int16_t)
AH_TEMPLATE(u_int32_t)
AH_TEMPLATE(u_int64_t)

DAST_REPLACE_TYPE(int16_t, 2)
DAST_REPLACE_TYPE(int32_t, 4)
DAST_REPLACE_TYPE(int64_t, 8)

DAST_REPLACE_TYPE_UNSIGNED(u_int16_t, 2)
DAST_REPLACE_TYPE_UNSIGNED(u_int32_t, 4)
DAST_REPLACE_TYPE_UNSIGNED(u_int64_t, 8)

AC_CACHE_CHECK(3rd argument of accept, ac_cv_accept_arg, [
  dnl Try socklen_t (POSIX)
  DAST_ACCEPT_ARG(socklen_t)

  dnl Try int (original BSD)
  DAST_ACCEPT_ARG(int)

  dnl Try size_t (older standard; AIX)
  DAST_ACCEPT_ARG(size_t)

  dnl Try short (shouldn't be)
  DAST_ACCEPT_ARG(short)

  dnl Try long (shouldn't be)
  DAST_ACCEPT_ARG(long)
])

if test -z "$ac_cv_accept_arg" ; then
  ac_cv_accept_arg=int
fi

AC_DEFINE_UNQUOTED([Socklen_t], $ac_cv_accept_arg, [Define 3rd arg of accept])

dnl
dnl Checks for library functions.
dnl
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_FORK fi
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_MALLOC fi
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_MEMCMP fi
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit gettimeofday memset pthread_cancel select strchr strerror strtol usleep])
AC_REPLACE_FUNCS(snprintf inet_pton inet_ntop gettimeofday)

dnl             Gotten from some NetBSD configure.in
dnl		We assume that if sprintf() supports %lld or %qd,
dnl		then all of *printf() does. If not, disable long long
dnl		support because we don't know how to display it.

AH_TEMPLATE(HAVE_QUAD_SUPPORT)
AH_TEMPLATE(HAVE_PRINTF_QD)

AC_MSG_CHECKING(*printf() support for %lld)
can_printf_longlong=no
AC_RUN_IFELSE([AC_LANG_SOURCE([[
	#include <stdio.h>
	int main() {
		char buf[100];
		sprintf(buf, "%lld", 21726587590LL);
		return (strcmp(buf, "21726587590"));
	}
	]])],[
	AC_MSG_RESULT(yes)
	can_printf_longlong=yes
	],[
	AC_MSG_RESULT(no)
	],[ : ])

if test $can_printf_longlong != yes; then
	AC_MSG_CHECKING(*printf() support for %qd)
	AC_RUN_IFELSE([AC_LANG_SOURCE([[
		#include <stdio.h>
		int main() {
			char buf[100];
			sprintf(buf, "%qd", 21726587590LL);
			return (strcmp(buf, "21726587590"));
		}
		]])],[
		AC_MSG_RESULT(yes)
		can_printf_longlong=yes
		AC_DEFINE(HAVE_PRINTF_QD, 1)
		],[
		AC_MSG_RESULT(no)
		],[ : ])
fi

if test $can_printf_longlong = yes; then
	AC_DEFINE(HAVE_QUAD_SUPPORT, 1)
fi

dnl ===================================================================
dnl Check for compiler characteristics

DAST_CHECK_BOOL

AC_C_BIGENDIAN

dnl ===================================================================
dnl Check for system services

dnl check for multicast
if test "$ac_cv_multicast" != no; then
  AC_CHECK_TYPES(struct ip_mreq,,,[#include "include/headers_slim.h"])
  AC_CHECK_DECLS(IP_ADD_MEMBERSHIP,,,[#include "include/headers_slim.h"])
  AC_MSG_CHECKING(for multicast support)
  ac_cv_multicast=no
  if test "$ac_cv_have_decl_IP_ADD_MEMBERSHIP" = yes; then
    if test "$ac_cv_type_struct_ip_mreq" = yes; then
      ac_cv_multicast=yes
    fi
  fi
  AC_MSG_RESULT($ac_cv_multicast)
  if test "$ac_cv_multicast" = yes; then
    AC_DEFINE([HAVE_MULTICAST], 1, [Define to enable multicast support])
  fi
fi

dnl check for IPv6
if test "$ac_cv_have_ipv6" != no; then
  AC_CHECK_TYPES(struct sockaddr_storage,,,[#include "include/headers_slim.h"])
  AC_CHECK_TYPES(struct sockaddr_in6,,,[#include "include/headers_slim.h"])
  AC_CHECK_DECLS(AF_INET6,,,[#include "include/headers_slim.h"])
  AC_MSG_CHECKING(for IPv6 headers and structures)
  ac_cv_have_ipv6=no
  if test "$ac_cv_type_struct_sockaddr_storage" = yes; then
    if test "$ac_cv_type_struct_sockaddr_in6" = yes; then
      if test "$ac_cv_have_decl_AF_INET6" = yes; then
	AC_DEFINE([HAVE_IPV6], 1, [Define to enable IPv6 support])
	ac_cv_have_ipv6=yes
      fi
    fi
  fi
  AC_MSG_RESULT($ac_cv_have_ipv6)
fi

if test "$ac_cv_have_ipv6" = yes; then
  if test "$ac_cv_multicast" = yes; then
    AC_CHECK_TYPES(struct ipv6_mreq,,,[#include "include/headers_slim.h"])
    AC_CHECK_DECLS(IPV6_ADD_MEMBERSHIP,,,[#include "include/headers_slim.h"])
    AC_CHECK_DECLS(IPV6_MULTICAST_HOPS,,,[#include "include/headers_slim.h"])
    AC_MSG_CHECKING(for IPv6 multicast support)
    ac_cv_have_ipv6_multicast=no
    if test "$ac_cv_type_struct_ipv6_mreq" = yes; then
      if test "$ac_cv_have_decl_IPV6_ADD_MEMBERSHIP" = yes; then
	if test "$ac_cv_have_decl_IPV6_MULTICAST_HOPS" = yes; then
	  AC_DEFINE([HAVE_IPV6_MULTICAST], 1, [Define to enable IPv6 multicast support])
	  ac_cv_have_ipv6_multicast=yes
	fi
      fi
    fi
    AC_MSG_RESULT($ac_cv_have_ipv6_multicast)
  fi
fi

if test "$enable_debuginfo" = yes; then
AC_DEFINE([DBG_MJZ], 1, [Define if debugging info is desired])
fi

if test "$enable_web100" != no; then
if test -e "/proc/web100"; then
if test -d "/proc/web100"; then
if test -e "/proc/web100/header"; then
if test -f "/proc/web100/header"; then
if test -r "/proc/web100/header"; then
DAST_PATH_WEB100()
if test "$web100_success" = yes; then 
AC_DEFINE([HAVE_WEB100], 1, [Define if Web100 is desired and available])
fi fi fi fi fi fi
fi

dnl GNU make allows us to use the $(strip ...) builtin which eliminates a
dnl large amount of extra whitespace in compile lines.
AC_MSG_CHECKING(whether make is GNU make)
STRIP_BEGIN=
STRIP_END=
if $ac_make --version 2> /dev/null | grep '^GNU Make ' > /dev/null ; then
    STRIP_BEGIN='$(strip $(STRIP_DUMMY)'
    STRIP_END=')'
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi

dnl some Make 3.79 $(strip ) versions are broken and require an empty arg
STRIP_DUMMY=
AC_SUBST(STRIP_DUMMY)
AC_SUBST(STRIP_BEGIN)
AC_SUBST(STRIP_END)

_OPENSS7_PACKAGE([IPERF], [OpenSS7 IPERF Utility])
_MAN_CONVERSION
_PUBLIC_RELEASE
_RPM_SPEC
_DEB_DPKG
AM_CONDITIONAL(WITH_LFS, false)dnl
AM_CONDITIONAL(WITH_LIS, false)dnl

AC_CONFIG_FILES([Makefile
		compat/Makefile
		doc/Makefile
		doc/man/Makefile
		include/Makefile
		src/Makefile])
AC_OUTPUT

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocindent nosmartindent
