dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL  vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: autotest.m4,v 0.9 2004/08/18 21:01:45 brian Exp $
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
dnl Last Modified $Date: 2004/08/18 21:01:45 $ by $Author: brian $
dnl 
dnl =========================================================================

# ===========================================================================
# _AUTOTEST
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST], [dnl
    _AUTOTEST_OPTIONS
    _AUTOTEST_SETUP
    _AUTOTEST_OUTPUT
])# _AUTOTEST
# ===========================================================================

# ===========================================================================
# _AUTOTEST_OPTIONS
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_OPTIONS], [dnl
    AC_ARG_ENABLE([autotest],
        AC_HELP_STRING([--enable-autotest],
            [enable pre- and post-install testing.  @<:@default=yes@:>@]),
        [enable_autotest="$enableval"],
        [enable_autotest=''])
    AC_MSG_CHECKING([for autotest on check and installcheck])
    AC_MSG_RESULT([${enable_autotest}])
    AM_CONDITIONAL([PERFORM_TESTING], [test :"${enable_autotest:-yes}" = :yes])dnl
])# _AUTOTEST_OPTIONS
# ===========================================================================

# ===========================================================================
# _AUTOTEST_SETUP
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_SETUP], [dnl
    _AUTOTEST_SETUP_AUTOM4TE
    _AUTOTEST_SETUP_AUTOTEST
])# _AUTOTEST_SETUP
# ===========================================================================

# ===========================================================================
# _AUTOTEST_SETUP_AUTOM4TE
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_SETUP_AUTOM4TE], [dnl
    AC_ARG_VAR([AUTOM4TE], [Autom4te command])
    AC_PATH_TOOL([AUTOM4TE], [autom4te], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${AUTOM4TE:-no}" = :no ; then
        AC_MSG_WARN([Could not find autom4te program in PATH.])
        AUTOM4TE='autom4te'
    fi
])# _AUTOTEST_SETUP_AUTOM4TE
# ===========================================================================

# ===========================================================================
# _AUTOTEST_SETUP_AUTOTEST
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_SETUP_AUTOTEST], [dnl
    AC_ARG_VAR([AUTOTEST], [Autotest macro build command])
    AC_PATH_TOOL([AUTOTEST], [autotest], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${AUTOTEST:-no}" = :no ; then
        AC_MSG_WARN([Could not find autotest program in PATH.])
        AUTOTEST="$AUTOM4TE --language=autotest"
    fi
])# _AUTOTEST_SETUP_AUTOTEST
# ===========================================================================

# ===========================================================================
# _AUTOTEST_OUTPUT
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_OUTPUT], [dnl
    AC_CONFIG_TESTDIR([tests])
    AC_CONFIG_FILES([tests/Makefile])
    AC_CONFIG_FILES([tests/atlocal])
dnl _AUTOTEST_OUTPUT_CONFIG
])# _AUTOTEST_OUTPUT
# ===========================================================================

# ===========================================================================
# _AUTOTEST_OUTPUT_CONFIG
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_OUTPUT_CONFIG], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_CONFIG_COMMANDS([tests/atlocal], [dnl
cat >tests/atlocal <<ATEOF
@%:@
@%:@ Local procedures for building test suites.
@%:@ Generated by $[0].
@%:@ Copyright (c) 1997-2004  OpenSS7 Corporation.  All Rights Reserved.
@%:@
at_build=`$ac_top_srcdir/scripts/config.guess`
at_host="$ac_cv_host"
at_target="$ac_cv_target"
DEPMOD="$DEPMOD"
DESTDIR="$DESTDIR"
LSMOD="$LSMOD"
MODPROBE="$MODPROBE"
@%:@ Exit if cross compiling: we cannot run tests.
if test x"$cross_compiling" = xyes ; then exit 0 ; fi
ATEOF
], [dnl
DEPMOD="$DEPMOD"
DESTDIR="$DESTDIR"
LSMOD="$LSMOD"
MODPROBE="$MODPROBE"
ac_build="$ac_cv_build"
ac_host="$ac_cv_host"
ac_target="$ac_cv_target"
])
])# _AUTOTEST_OUTPUT_CONFIG
# ===========================================================================

# ===========================================================================
# _AUTOTEST_
# ---------------------------------------------------------------------------
AC_DEFUN([_AUTOTEST_], [dnl
])# _AUTOTEST_
# ===========================================================================

dnl =========================================================================
dnl
dnl Copyright (C) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
dnl
dnl All Rights Reserved.
dnl
dnl =========================================================================
dnl ENDING    OF SEPARATE COPYRIGHT MATERIAL  vim: ft=config sw=4 et
dnl =========================================================================
