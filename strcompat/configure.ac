# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: configure.ac,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2008-08-20 10:56:42 $
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
# Last Modified $Date: 2008-08-20 10:56:42 $ by $Author: brian $
#
# =============================================================================

dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
AC_PREREQ([2.62])
AC_INIT([strcompat],[0.9.2],[bugs@openss7.org])
AC_REVISION([$Name:  $($Revision: 0.9.2.20 $) $Date: 2008-08-20 10:56:42 $])

_OPENSS7_DIRCHANGE

AC_CONFIG_SRCDIR([strcompat.lsm.in])
AC_CONFIG_HEADERS([config.h src/include/sys/strcompat/config.h])

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
AC_PROG_LD
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LEX
AC_PROG_YACC
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])
dnl AM_GNU_GETTEXT_VERSION([0.17])
dnl AM_GNU_GETTEXT

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
AC_CHECK_HEADERS([arpa/inet.h fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
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
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm gettimeofday inet_ntoa memset strtol strtoul])

AC_COMPAT

AC_CONFIG_FILES([Makefile
		 scripts/texi/args.texi
		 src/include/Makefile
		 src/Makefile
		 lib32/Makefile
		 doc/Makefile
		 doc/man/Makefile
		 doc/manual/texi/args.texi
		 doc/manual/Makefile])

_OPENSS7_OUTPUT

AC_OUTPUT

# =============================================================================
#
# $Log: configure.ac,v $
# Revision 0.9.2.20  2008-08-20 10:56:42  brian
# - fixes and build updates from newnet trip
#
# Revision 0.9.2.19  2008-08-02 23:25:08  brian
# - restore dependency defaults from old autoconf/automake
#
# Revision 0.9.2.18  2008-07-01 13:02:16  brian
# - gettext updates and references updates
#
# Revision 0.9.2.17  2008-04-28 16:47:00  brian
# - updates for release
#
# Revision 0.9.2.16  2007/10/16 06:04:28  brian
# - fix cross-compile bug
#
# Revision 0.9.2.15  2007/08/19 11:21:20  brian
# - instead of hardcoding directories
#
# Revision 0.9.2.14  2007/08/13 23:51:28  brian
# - updated to GPLv3 headers
#
# Revision 0.9.2.13  2007/03/26 08:26:18  brian
# - need gnu gettext version 0.16.1 for autoconf 2.61
#
# Revision 0.9.2.12  2007/03/26 07:56:23  brian
# - install html images properly, needs autoconf 2.61 and automake 2.10
#
# Revision 0.9.2.11  2007/02/22 08:47:05  brian
# - updated required autoconf to 2.60
#
# Revision 0.9.2.10  2007/02/12 16:56:09  brian
# - more autoconf 2.61 support
#
# Revision 0.9.2.9  2007/02/12 10:40:34  brian
# - added support for autoconf 2.61
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
