dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) rpm.m4,v 1.1.2.1 2004/01/13 16:11:35 brian Exp
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
dnl Last Modified 2004/01/13 16:11:35 by brian
dnl 
dnl =========================================================================

# =========================================================================
# AC_RPM_SPEC
# -------------------------------------------------------------------------
# Common things that need to be done in setting up an RPM spec file from an
# RPM.spec.in file.
# -------------------------------------------------------------------------
AC_DEFUN(AC_RPM_SPEC,
[
    _RPM_SPEC_OPTIONS
    _RPM_SPEC_SETUP
    _RPM_SPEC_OUTPUT
])# AC_RPM_SPEC
# =========================================================================

# =========================================================================
# _RPM_SPEC_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN(_RPM_SPEC_OPTIONS,
[
    AC_ARG_WITH([rpm-epoch],
        AC_HELP_STRING([--with-rpm-epoch=EPOCH],
            [specify the EPOCH for the RPM spec file.
            @<:@default=1@:>@]),
        [with_rpm_epoch=$withval],
        [with_rpm_epoch=1])
    AC_ARG_WITH([rpm-release],
        AC_HELP_STRING([--with-rpm-release=RELEASE],
            [specify the RELEASE for the RPM spec file.
            @<:@default=1@:>@]),
        [with_rpm_release=$withval],
        [with_rpm_release='1'])
])# _RPM_SPEC_OPTIONS
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP
# -------------------------------------------------------------------------
AC_DEFUN(_RPM_SPEC_SETUP,
[
    # two extra subsitutions for the RPM spec file
    PACKAGE_EPOCH="${with_rpm_epoch:-1}"
    AC_SUBST(PACKAGE_EPOCH)
    AC_DEFINE_UNQUOTED([PACKAGE_EPOCH], [$PACKAGE_EPOCH], [The RPM Epoch. This
        defaults to 1.])
    PACKAGE_RELEASE="${with_rpm_release:-Custom}"
    AC_SUBST(PACKAGE_RELEASE)
    AC_DEFINE_UNQUOTED([PACKAGE_RELEASE], ["$PACKAGE_RELEASE"], [The RPM
        Release. This defaults to Custom.])
    PACKAGE_OPTIONS=
    for arg in $ac_configure_args ; do
        if (echo "$arg" | grep -v '[[= ]]' >/dev/null 2>&1) ; then
            eval "arg=$arg"
            if (echo "$arg" | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
                arg="`echo $arg | sed -e's|--enable|--with|;s|--disable|--without|;s|--with-|--with |;s|--without-|--without |;s|-|_|g;s|^__|--|'`"
                PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }$arg"
            fi
        fi
    done
    AC_SUBST(PACKAGE_OPTIONS)
])# _RPM_SPEC_SETUP
# =========================================================================

# =========================================================================
# _RPM_SPEC_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN(_RPM_SPEC_OUTPUT,
[
    AC_CONFIG_FILES(m4_ifdef([AC_PACKAGE_NAME],[AC_PACKAGE_NAME]).spec)
])# _RPM_SPEC_OUTPUT
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
