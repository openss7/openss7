#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_REVISION([$Id: configure.ac,v 0.9.2.2 2004/12/18 10:97:02 brian Exp $])
AC_PREREQ(2.59)
AC_INIT([OpenSS7], [0.9.2], [bugs@openss7.org])

# Need to adjust directories if default
ac_default_prefix='/usr'
if test :"$sysconfdir" = :'${prefix}/etc' ; then sysconfdir='/etc' ; fi
if test :"$localstatedir" = :'${prefix}/var' ; then localstatedir='/var' ; fi
if test :"$infodir" = :'${prefix}/info' ; then infodir='${prefix}/share/info' ; fi
if test :"$mandir" = :'${prefix}/man' ; then mandir='${prefix}/share/man' ; fi

AC_CONFIG_SRCDIR([strxns/config.h.in])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE

AC_GNU_SOURCE

# Checks for programs.
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
AM_GNU_GETTEXT_VERSION(0.14.1)

#
# Checks for libraries.
#
pgk_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
AC_CHECK_LIB([pthread], [pthread_create])
LIBS="$pkg_save_LIBS"

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([argz.h arpa/inet.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdint.h stdio_ext.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h termios.h unistd.h values.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
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
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

#
# Checks for library functions.
#
AC_FUNC_ERROR_AT_LINE
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_FORK fi
AC_PROG_GCC_TRADITIONAL
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_MALLOC fi
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
if test :"${cross_compiling:-no}" != :yes ; then :; AC_FUNC_SETPGRP fi
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify alarm atexit bzero getcwd gethostbyname gethrtime gettimeofday inet_ntoa localtime_r mempcpy memset munmap nl_langinfo pstat_getdynamic select setlocale socket sqrt stpcpy strcasecmp strchr strcspn strdup strerror strrchr strstr strtol strtoul uname])

AC_OPENSS7

AC_CONFIG_SUBDIRS(LiS strxns strxnet strinet strsctp)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
