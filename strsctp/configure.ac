dnl =========================================================================
dnl
dnl @(#) $Id: configure.ac,v 0.9.2.3 2004/08/22 04:42:03 brian Exp $
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
dnl Last Modified $Date: 2004/08/22 04:42:03 $ by $Author: brian $
dnl 
dnl =========================================================================

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([strsctp],[0.9.2],[bugs@openss7.org])

# Need to adjust directories if default
ac_default_prefix='/usr'
if test :"$sysconfdir" = :'${prefix}/etc' ; then sysconfdir='/etc' ; fi
if test :"$localstatedir" = :'${prefix}/var' ; then localstatedir='/var' ; fi
if test :"$infodir" = :'${prefix}/info' ; then infodir='${prefix}/share/info' ; fi
if test :"$mandir" = :'${prefix}/man' ; then mandir='${prefix}/share/man' ; fi

AC_CONFIG_SRCDIR([src/drivers/sctp.c])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE

AC_GNU_SOURCE

#
# Checks for programs.
#
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AM_PROG_INSTALL_STRIP
AC_ARG_VAR([LD], [Linker loader command])
AC_PROG_LD
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

#
# Checks for libraries.
#
ss7_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([LiS], [putpmsg])
AC_CHECK_LIB([streams], [putpmsg])
AC_CHECK_LIB([xnet], [t_open])
AC_CHECK_LIB([socket], [socket])
LIBS="$ss7_save_LIBS"

#
# Checks for header files.
#
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

#
# Checks for typedefs, structures, and compiler characteristics.
#
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

# 
# Checks for library functions.
# 
# AM_GNU_GETTEXT
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_FORK fi
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday memset strerror strtol])

AC_SCTP

AC_CONFIG_FILES([Makefile
		 src/include/Makefile
		 src/Makefile
		 doc/Makefile
		 doc/man/Makefile
		 doc/manual/Makefile
		 doc/manual/texi/args.texi])

AC_OUTPUT
