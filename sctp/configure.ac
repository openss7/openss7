dnl =========================================================================
dnl
dnl @(#) $Id: configure.ac,v 0.9.2.2 2004/12/20 13:49:37 brian Exp $
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
dnl Last Modified $Date: 2004/12/20 13:49:37 $ by $Author: brian $
dnl 
dnl =========================================================================

dnl
dnl Process this file with autoconf to produce a configure script.
dnl
AC_REVISION([$Id: configure.ac,v 0.9.2.2 2004/12/20 13:49:37 brian Exp $])
AC_PREREQ(2.59)
AC_INIT([OpenSS7 Linux Native SCTP], [0.2.20], [bugs@openss7.org], [sctp])

if test "$cache_file" = /dev/null -o -z "$cache_file" ; then cache_file='config.cache' ; fi
if test -w "$cache_file"; then :; else touch "$cache_file" ; fi
if test -z "$CONFIG_SITE" ; then CONFIG_SITE='config.site' ; fi

AC_CONFIG_COMMANDS([siteconfig], [dnl
    if test -n "$CONFIG_SITE" -a -n "$cache_file" ; then
	if test -w "$CONFIG_SITE" -o ! -e "$CONFIG_SITE" ; then
	    cat "$cache_file" | egrep -v '^(ac_cv_env_|ac_cv_host|ac_cv_target|linux_cv_|sctp_cv_)' > "$CONFIG_SITE"
	fi
    fi], [cache_file="$cache_file" ; CONFIG_SITE="$CONFIG_SITE"])

# Need to adjust directories if default
ac_default_prefix='/usr'
if test :"$sysconfdir"    = :'${prefix}/etc'  ; then sysconfdir='/etc'              ; fi
if test :"$localstatedir" = :'${prefix}/var'  ; then localstatedir='/var'           ; fi
if test :"$infodir"       = :'${prefix}/info' ; then infodir='${prefix}/share/info' ; fi
if test :"$mandir"        = :'${prefix}/man'  ; then mandir='${prefix}/share/man'   ; fi

AC_CONFIG_SRCDIR([src/sctp.c])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_GNU_SOURCE

#
# Checks for programs.
#
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
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

#
# Checks for libraries.
#
sctp_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
AC_CHECK_LIB([pthread], [pthread_create])
LIBS="$sctp_save_LIBS"

#
# Checks for header files.
#
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

#
# Checks for typedefs, structures, and compiler characteristics.
#
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

#
# Checks for library functions.
#
AC_FUNC_ERROR_AT_LINE
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_FORK fi
AC_PROG_GCC_TRADITIONAL
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_MALLOC fi
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

# vim: ft=config sw=4
