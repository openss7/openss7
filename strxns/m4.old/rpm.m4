dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) rpm.m4,v 0.9.2.1 2004/01/13 16:11:35 brian Exp
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
# _RPM_SPEC
# -------------------------------------------------------------------------
# Common things that need to be done in setting up an RPM spec file from an
# RPM.spec.in file.
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC], [dnl
    _RPM_SPEC_OPTIONS
    _RPM_SPEC_SETUP
    _RPM_SPEC_OUTPUT
])# _RPM_SPEC
# =========================================================================

# =========================================================================
# _RPM_SPEC_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_OPTIONS], [dnl
])# _RPM_SPEC_OPTIONS
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP], [dnl
    _RPM_SPEC_SETUP_EPOCH
    _RPM_SPEC_SETUP_RELEASE
    _RPM_SPEC_SETUP_TOOLS
    _RPM_SPEC_SETUP_MODULES
    _RPM_SPEC_SETUP_OPTIONS
    _RPM_SPEC_SETUP_BUILD
    _RPM_SPEC_SETUP_SIGN
])# _RPM_SPEC_SETUP
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_EPOCH
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_EPOCH], [dnl
    AC_ARG_WITH([rpm-epoch],
        AS_HELP_STRING([--with-rpm-epoch=EPOCH],
            [specify the EPOCH for the RPM spec file.  @<:@default=1@:>@]),
        [with_rpm_epoch="$withval"],
        [if test -r .rpmepoch; then d= ; else d="$srcdir/" ; fi
         if test -r ${d}.rpmepoch
         then with_rpm_epoch="`cat ${d}.rpmepoch`"
         else with_rpm_epoch=1
         fi])
    AC_MSG_CHECKING([for rpm epoch])
    AC_MSG_RESULT([${with_rpm_epoch:-1}])
    PACKAGE_EPOCH="${with_rpm_epoch:-1}"
    AC_SUBST([PACKAGE_EPOCH])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_EPOCH], [$PACKAGE_EPOCH], [The RPM Epoch. This
        defaults to 1.])
])# _RPM_SPEC_SETUP_EPOCH
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_RELEASE], [dnl
    AC_ARG_WITH([rpm-release],
        AS_HELP_STRING([--with-rpm-release=RELEASE],
            [specify the RELEASE for the RPM spec file.
            @<:@default=Custom@:>@]),
        [with_rpm_release="$withval"],
        [if test -r .rpmrelease ; then d= ; else d="$srcdir/" ; fi
         if test -r ${d}.rpmrelease
         then with_rpm_release="`cat ${d}.rpmrelease`"
         else with rpm_release='Custom'
         fi])
    AC_MSG_CHECKING([for rpm release])
    AC_MSG_RESULT([${with_rpm_release:-Custom}])
    PACKAGE_RELEASE="${with_rpm_release:-Custom}"
    AC_SUBST([PACKAGE_RELEASE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_RELEASE], ["$PACKAGE_RELEASE"], [The RPM
        Release. This defaults to Custom.])
])# _RPM_SPEC_SETUP_RELEASE
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_TOOLS
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_TOOLS], [dnl
    AC_ARG_ENABLE([tools],
        AS_HELP_STRING([--enable-tools],
            [build and install user packages.  @<:@default=yes@:>@]),
        [enable_tools="$enableval"],
        [enable_tools='yes'])
    AC_MSG_CHECKING([for rpm build/install of user packages])
    AC_MSG_RESULT([${enable_tools:-yes}])
    AM_CONDITIONAL([RPM_BUILD_USER], [test :"${enable_tools:-yes}" = :yes])dnl
])# _RPM_SPEC_SETUP_TOOLS
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_MODULES
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_MODULES], [dnl
    AC_ARG_ENABLE([modules],
        AS_HELP_STRING([--enable-modules],
            [build and install kernel packages.  @<:@default=yes@:>@]),
        [enable_modules="$enableval"],
        [enable_modules='yes'])
    AC_MSG_CHECKING([for rpm build/install of kernel packages])
    AC_MSG_RESULT([${enable_modules:-yes}])
    AM_CONDITIONAL([RPM_BUILD_KERNEL], [test :"${enable_modules:-yes}" = :yes])dnl
])# _RPM_SPEC_SETUP_MODULES
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_OPTIONS], [dnl
    PACKAGE_OPTIONS=
    arg=
    for arg_part in $ac_configure_args ; do
        if (echo "$arg_part" | grep "^'" >/dev/null 2>&1) ; then
            if test -n "$arg" ; then
                eval "arg=$arg"
                AC_MSG_CHECKING([for rpm argument '$arg'])
                if (echo "$arg" | grep -v '[[= ]]' >/dev/null 2>&1) ; then
                    if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
                        arg="`echo $arg | sed -e's|--enable|--with|;s|--disable|--without|;s|--with-|--with |;s|--without-|--without |;s|-|_|g;s|^__|--|'`"
                        PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }$arg"
                        AC_MSG_RESULT([$arg])
                    else
                        :
                        AC_MSG_RESULT([no])
                    fi
                else
                    :
                    AC_MSG_RESULT([no])
                fi
            fi
            arg="$arg_part"
        else
            arg="${arg}${arg:+ }${arg_part}"
        fi
    done
    if test -n "$arg" ; then
        eval "arg=$arg"
        AC_MSG_CHECKING([for rpm argument '$arg'])
        if (echo "$arg" | grep -v '[[= ]]' >/dev/null 2>&1) ; then
            if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
                arg="`echo $arg | sed -e's|--enable|--with|;s|--disable|--without|;s|--with-|--with |;s|--without-|--without |;s|-|_|g;s|^__|--|'`"
                PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }$arg"
                AC_MSG_RESULT([$arg])
            else
                :
                AC_MSG_RESULT([no])
            fi
        else
            :
            AC_MSG_RESULT([no])
        fi
    fi
    AC_SUBST([PACKAGE_OPTIONS])dnl
])# _RPM_SPEC_SETUP_OPTIONS
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_BUILD
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_BUILD], [dnl
    AC_ARG_VAR([RPM], [Rpm command])
    AC_PATH_TOOL([RPM], [rpm], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${RPM:-no}" = :no ; then
        AC_MSG_WARN([Could not find rpm program in PATH.])
    fi
    AC_ARG_VAR([RPMBUILD], [Build rpms command])
    AC_PATH_TOOL([RPMBUILD], [rpmbuild], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${RPMBUILD:-${RPM:-no}}" = :no ; then
        AC_MSG_WARN([Could not find rpmbuild program in PATH.])
    fi
    if test :"${RPMBUILD:-no}" = :no ; then
        RPMBUILD="$RPM"
    fi
    AM_CONDITIONAL([BUILD_RPMS], [test :"${RPMBUILD:-no}" != :no])dnl
])# _RPM_SPEC_SETUP_BUILD
# =========================================================================

# =========================================================================
# _RPM_SPEC_SETUP_SIGN
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_SIGN], [dnl
    AC_ARG_VAR([GPG], [GPG signature command])
    AC_PATH_TOOL([GPG], [gpg pgp], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${GPG:-no}" = :no ; then
        AC_MSG_WARN([Could not find gpg program in PATH.])
    fi
dnl ---------------------------------------------------------
    AC_ARG_VAR([GNUPGUSER], [GPG user name])
    AC_ARG_WITH([gpg-user],
        AS_HELP_STRING([--with-gpg-user=USERNAME],
            [specify the USER for signing RPMs and tarballs.
            @<:@default=${GNUPGUSER}@:>@]),
        [with_gpg_user="$withval"],
        [with_gpg_user="$GNUPGUSER"
         if test :"${with_gpg_user:-no}" = :no ; then
             if test -r .gnupguser; then d= ; else d="$srcdir/" ; fi
             if test -r ${d}.gnupguser
             then with_gpg_user="`cat ${d}.gnupguser`"
             else with_gpg_user=''
             fi
dnl          if test :"${with_gpg_user:-no}" = :no ; then
dnl              with_gpg_user="`whoami`"
dnl          fi
         fi])
    AC_MSG_CHECKING([for gpg user])
    GNUPGUSER="${with_gpg_user:-`whoami`}"
    AC_MSG_RESULT([${GNUPGUSER:-no}])
dnl ---------------------------------------------------------
    AC_ARG_VAR([GNUPGHOME], [GPG home directory])
    AC_ARG_WITH([gpg-home],
        AS_HELP_STRING([--with-gpg-home=HOMEDIR],
            [specify the HOME for signing RPMs and tarballs.
            @<:@default=${GNUPGHOME:-~/.gnupg}@:>@]),
        [with_gpg_home="$withval"],
        [with_gpg_home="$GNUPGHOME"
         if test :"${with_gpg_home:-no}" = :no ; then
             if test -r .gnupghome; then d= ; else d="$srcdir/" ; fi
             if test -r ${d}.gnupghome
             then with_gpg_home="`cat ${d}.gnupghome`"
             else with_gpg_home=''
             fi
dnl          if test :"${with_gpg_home:-no}" = :no ; then
dnl              with_gpg_home='~/.gnupg'
dnl          fi
         fi])
    AC_MSG_CHECKING([for gpg home])
    GNUPGHOME="${with_gpg_home:-~/.gnupg}"
    AC_MSG_RESULT([${GNUPGHOME:-no}])
])# _RPM_SPEC_SETUP_SIGN
# =========================================================================

# =========================================================================
# _RPM_SPEC_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_OUTPUT], [dnl
    AC_CONFIG_FILES(m4_ifdef([AC_PACKAGE_NAME],[AC_PACKAGE_NAME]).spec)
])# _RPM_SPEC_OUTPUT
# =========================================================================

# =========================================================================
# _RPM_
# -------------------------------------------------------------------------
AC_DEFUN([_RPM_], [dnl
])# _RPM_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
