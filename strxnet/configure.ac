dnl =========================================================================
dnl
dnl @(#) $Id: configure.ac,v 0.9.2.5 2004/05/19 20:07:57 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (C) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
dnl
dnl All Rights Reserved.
dnl
dnl Permission is granted to make and distribute verbatim copies of this
dnl README file provided the copyright notice and this permission notice are
dnl preserved on all copies.
dnl 
dnl Permission is granted to copy and distribute modified versions of this
dnl manual under the conditions for verbatim copying, provided that the
dnl entire resulting derived work is distributed under the terms of a
dnl permission notice identical to this one
dnl 
dnl Since the Linux kernel and libraries are constantly changing, this README
dnl file may be incorrect or out-of-date.  The author(s) assume no
dnl responsibility for errors or omissions, or for damages resulting from the
dnl use of the information contained herein.  The author(s) may not have
dnl taken the same level of care in the production of this manual, which is
dnl licensed free of charge, as they might when working professionally.
dnl 
dnl Formatted or processed versions of this manual, if unaccompanied by the
dnl source, must acknowledge the copyright and authors of this work.
dnl 
dnl =========================================================================
dnl 
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions
dnl apply to you.  If the Software is supplied by the Department of Defense
dnl ("DoD"), it is classified as "Commercial Computer Software" under
dnl paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
dnl Regulations ("DFARS") (or any successor regulations) and the Government is
dnl acquiring only the license rights granted herein (the license rights
dnl customarily provided to non-Government users).  If the Software is
dnl supplied to any unit or agency of the Government other than DoD, it is
dnl classified as "Restricted Computer Software" and the Government's rights
dnl in the Software are defined in paragraph 52.227-19 of the Federal
dnl Acquisition Regulations ("FAR") (or any success regulations) or, in the
dnl cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl 
dnl =========================================================================
dnl 
dnl Commercial licensing and support of this software is available from
dnl OpenSS7 Corporation at a fee.  See http://www.openss7.com/
dnl 
dnl =========================================================================
dnl
dnl Last Modified $Date: 2004/05/19 20:07:57 $ by $Author: brian $
dnl 
dnl =========================================================================

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([strxnet],[0.9.2],[bugs@openss7.org])
AC_CONFIG_SRCDIR([src/modules/timod.c])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_AUX_DIR([scripts])

auxdir="$ac_aux_dir"
AC_SUBST(auxdir)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE

AC_GNU_SOURCE

dnl
dnl Checks for programs.
dnl
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AM_PROG_INSTALL_STRIP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.14.1)

dnl
dnl Checks for libraries.
dnl
xnet_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([LiS], [fattach])
AC_CHECK_LIB([streams], [fattach])
AC_CHECK_LIB([xnet], [t_open])
AC_CHECK_LIB([socket], [socket])
LIBS="$xnet_save_LIBS"

dnl
dnl Checks for header files.
dnl
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h libintl.h locale.h netinet/in.h stddef.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h values.h])

dnl 
dnl Checks for typedefs, structures, and compiler characteristics.
dnl 
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl 
dnl Checks for library functions.
dnl 
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
if test :"${cross_compiling-no}" != :yes ; then
	AC_FUNC_MALLOC
fi
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([bzero gettimeofday memset strerror strtol])

AC_XNET

AC_CONFIG_FILES([
	Makefile
	intl/Makefile
	po/Makefile.in
	m4/Makefile
	src/include/Makefile
	src/Makefile
	doc/Makefile
	doc/man/Makefile
	doc/manual/Makefile
	src/drivers/lis.conf])

AC_OUTPUT
