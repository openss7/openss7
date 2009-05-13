# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date$ by $Author$
#
# =============================================================================

dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
AC_PREREQ(2.63)
AC_INIT([OpenSS7],[1.2],[bugs@openss7.org],[openss7])
AC_REVISION([$Name$($Revision$) $Date$])

_OPENSS7_DIRCHANGE

AC_CONFIG_SRCDIR([openss7.lsm.in])
AC_CONFIG_HEADERS([config.h src/include/sys/openss7/config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

_DISTRO

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE
dnl
dnl make --enable-maintainer-mode imply --enable-dependency-tracking to avoid
dnl typing, but allow user to override with --disable-dependency-tracking.
dnl
if test :${USE_MAINTAINER_MODE:-yes} = :yes ; then
	enable_dependency_tracking="${enable_dependency_tracking:-yes}"
fi
dnl
dnl force default from older autoconf/automake when not specified by caller
dnl
enable_dependency_tracking="${enable_dependency_tracking:-no}"

AC_USE_SYSTEM_EXTENSIONS

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
LT_PATH_LD
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_LEX
AC_PROG_YACC

LT_INIT([dlopen])
AC_SUBST([LIBTOOL_DEPS])
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT

dnl
dnl Checks for libraries.
dnl
pkg_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([LiS], [fattach])
AC_CHECK_LIB([streams], [fattach])
AC_CHECK_LIB([xnet], [t_open])
AC_CHECK_LIB([socket], [socket])
LIBS="$pkg_save_LIBS"

dnl
dnl Checks for header files.
dnl
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([argz.h arpa/inet.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdint.h stdio_ext.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/mount.h sys/socket.h sys/time.h sys/timeb.h syslog.h termios.h unistd.h values.h wchar.h]) 



dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

dnl
dnl Checks for library functions.
dnl
AC_FUNC_ERROR_AT_LINE
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_FORK fi
AC_PROG_GCC_TRADITIONAL
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_MALLOC fi
AC_FUNC_MEMCMP
AC_FUNC_MMAP
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_REALLOC fi
AC_FUNC_SELECT_ARGTYPES
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_SETPGRP fi
AC_DEFINE_UNQUOTED([RETSIGTYPE],[void],
    [Define as the return type of signal handlers (`int' or `void').])

AC_FUNC_STRERROR_R
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify alarm atexit bzero clock_gettime getcwd gethostbyaddr gethostbyname gethostname gethrtime gettimeofday inet_ntoa localeconv localtime_r mempcpy memset munmap nl_langinfo pow pstat_getdynamic select setlocale socket sqrt stpcpy strcasecmp strchr strcspn strdup strerror strncasecmp strpbrk strrchr strspn strstr strtol strtoul uname])



AC_OPENSS7

AC_CONFIG_FILES([Makefile
		 doc/Makefile
		 doc/drafts/Makefile
		 doc/man/Makefile
		 doc/manual/Makefile
		 doc/papers/Makefile
		 doc/papers/chan/Makefile
		 doc/papers/inet/Makefile
		 doc/papers/sctp/Makefile
		 doc/papers/sigtran/Makefile
		 doc/papers/streams/Makefile
		 doc/projects/Makefile
		 doc/projects/carrier/Makefile
		 doc/projects/cnam/Makefile
		 doc/projects/hlr/Makefile
		 doc/projects/lab/Makefile
		 doc/projects/mg/Makefile
		 doc/projects/sg/Makefile
		 doc/projects/sgp/Makefile
		 doc/projects/ss/Makefile
		 doc/projects/ssg/Makefile
		 doc/projects/voip/Makefile
		 doc/specs/Makefile
		 doc/specs/cci/Makefile
		 doc/specs/cdi/Makefile
		 doc/specs/chi/Makefile
		 doc/specs/dlpi/Makefile
		 doc/specs/lli/Makefile
		 doc/specs/mapi/Makefile
		 doc/specs/mtpi/Makefile
		 doc/specs/mxi/Makefile
		 doc/specs/nli/Makefile
		 doc/specs/npi/Makefile
		 doc/specs/sccpi/Makefile
		 doc/specs/sdli/Makefile
		 doc/specs/sdti/Makefile
		 doc/specs/sli/Makefile
		 doc/specs/tci/Makefile
		 doc/specs/tpi/Makefile
		 doc/specs/tri/Makefile
		 doc/specs/wan/Makefile
		 doc/specs/xcc/Makefile
		 doc/specs/xmap/Makefile
		 intl/Makefile
		 lib32/Makefile
		 po/Makefile.in
		 scripts/texi/args.texi
		 src/Makefile
		 src/drivers/Config
		 src/modules/Config
		 src/include/Makefile])

_OPENSS7_OUTPUT

AC_OUTPUT

# =============================================================================
#
# $Log$
# =============================================================================
# 
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
