dnl =============================================================================
dnl 
dnl @(#) $RCSfile: configure.ac,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/02/17 11:47:25 $
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl
dnl All Rights Reserved.
dnl
dnl This program is free software; you can redistribute it and/or modify it under
dnl the terms of the GNU General Public License as published by the Free Software
dnl Foundation; either version 2 of the License, or (at your option) any later
dnl version.
dnl
dnl This program is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
dnl FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
dnl details.
dnl
dnl You should have received a copy of the GNU General Public License along with
dnl this program; if not, write to the Free Software Foundation, Inc., 675 Mass
dnl Ave, Cambridge, MA 02139, USA.
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions apply
dnl to you.  If the Software is supplied by the Department of Defense ("DoD"), it
dnl is classified as "Commercial Computer Software" under paragraph 252.227-7014
dnl of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
dnl successor regulations) and the Government is acquiring only the license rights
dnl granted herein (the license rights customarily provided to non-Government
dnl users).  If the Software is supplied to any unit or agency of the Government
dnl other than DoD, it is classified as "Restricted Computer Software" and the
dnl Government's rights in the Software are defined in paragraph 52.227-19 of the
dnl Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
dnl the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Commercial licensing and support of this software is available from OpenSS7
dnl Corporation at a fee.  See http://www.openss7.com/
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Last Modified $Date: 2005/02/17 11:47:25 $ by $Author: brian $
dnl
dnl =============================================================================

dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
AC_REVISION([$Id: configure.ac,v 0.9.2.11 2005/02/17 11:47:25 brian Exp $])
AC_PREREQ(2.59)
AC_INIT([SCTP], [0.2.20], [bugs@openss7.org], [sctp])

_OPENSS7_DIRCHANGE

AC_CONFIG_SRCDIR([src/sctp.c])
AC_CONFIG_HEADER([config.h include/linux/hooks.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

_DISTRO

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION-1)
AM_MAINTAINER_MODE
dnl
dnl make --enable-maintainer-mode imply --enable-dependency-tracking to avoid typing
dnl
if test :${USE_MAINTAINER_MODE:-yes} = :yes ; then
	enable_dependency_tracking='yes'
fi

AC_GNU_SOURCE

dnl
dnl Checks for programs.
dnl
AM_PROG_AS
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROG_INSTALL
AM_PROG_INSTALL_STRIP
AC_ARG_VAR([LD], [Linker loader command])
AC_PROG_LD
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

dnl
dnl Checks for libraries.
dnl
sctp_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
AC_CHECK_LIB([pthread], [pthread_create])
LIBS="$sctp_save_LIBS"

dnl
dnl Checks for header files.
dnl
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl
dnl Checks for library functions.
dnl
AC_FUNC_ERROR_AT_LINE
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_FORK fi
AC_PROG_GCC_TRADITIONAL
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_MALLOC fi
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday inet_ntoa memset pow socket sqrt])

AC_SCTP

AC_CONFIG_FILES([Makefile
                 scripts/texi/args.texi
                 include/Makefile
                 doc/Makefile
                 doc/man/Makefile
                 doc/manual/Makefile
                 doc/manual/texi/args.texi
		 doc/specs/Makefile])

AC_OUTPUT

dnl vim: ft=config sw=4 et
