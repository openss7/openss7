dnl =========================================================================
dnl
dnl @(#) $Id: dist.m4,v 0.9.2.5 2005/02/13 12:15:29 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corp. <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
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
dnl Last Modified $Date: 2005/02/13 12:15:29 $ by $Author: brian $
dnl 
dnl =========================================================================

# ===========================================================================
# _DISTRO
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO], [dnl
    _DISTRO_CHECK_OS
    _DISTRO_OPTIONS
    _DISTRO_SETUP
    _DISTRO_OUTPUT
    _DISTRO_CHECK_VENDOR
])# _DISTRO
# ===========================================================================

# ===========================================================================
# _DISTRO_CHECK_OS
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO_CHECK_OS], [dnl
    case "$build_os" in
        (linux|linux-gnu) ;;
        (*) AC_MSG_WARN([
**** 
**** The build system is $build_os  It is not wise to
**** build for Linux on a system other than Linux.  If you have
**** problems later, please build on a Linux system.
**** ]) ;;
    esac
    case "$host_os" in
        (linux|linux-gnu) ;;
        (*) AC_MSG_ERROR([
**** 
**** Sorry, this package only builds for Linux.  The host system is
**** $host_os  Please specify a linux host with the --host
**** option to configure.
**** ]) ;;
    esac
    case "$target_os" in
        (linux|linux-gnu) ;;
        (*) AC_MSG_ERROR([
**** 
**** Sorry, this package only builds for Linux.  The target system is
**** $target_os  Please specify a linux host with the --host or
**** or --target to configure.
**** ]) ;;
    esac
])# _DISTRO_CHECK_OS
# ===========================================================================

# ===========================================================================
# _DISTRO_OPTIONS
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO_OPTIONS], [dnl
    AC_ARG_WITH([dist-vendor],
        AS_HELP_STRING([--with-dist-vendor=VENDOR],
            [override distribution VENDOR. @<:@default=auto@:>@]),
        [with_dist_vendor="$withval"],
        [with_dist_vendor=''])
    AC_ARG_WITH([dist-flavor],
        AS_HELP_STRING([--with-dist-flavor=FLAVOR],
            [override distribution FLAVOR. @<:@default=auto@:>@]),
        [with_dist_flavor="$withval"],
        [with_dist_flavor=''])
    AC_ARG_WITH([dist-release],
        AS_HELP_STRING([--with-dist-release=RELEASE],
            [override distribution RELEASE. @<:@default=auto@:>@]),
        [with_dist_release="$withval"],
        [with_dist_release=''])
])# _DISTRO_OPTIONS
# ===========================================================================

# ===========================================================================
# _DISTRO_FUNCTIONS
# ---------------------------------------------------------------------------
AC_DEFUN_ONCE([_DISTRO_FUNCTIONS], [dnl
dist_get_flavor() {
dnl AC_MSG_WARN([checking for flavor in $[1]])
    case "$[1]" in
        (*White?Box*)   echo 'whitebox' ;;
        (*Fedora?Core*) echo 'fedora'   ;;
        (*Mandrake*)    echo 'mandrake' ;;
        (*Red?Hat*)     echo 'redhat'   ;;
        (*SuSE*)        echo 'suse'     ;;
        (*Debian*)      echo 'debian'   ;;
    esac
}
dist_get_vendor() {
dnl AC_MSG_WARN([checking for vendor in $[1]])
    case "$[1]" in
        (whitebox)  echo 'whitebox' ;;
        (fedora)    echo 'redhat'   ;;
        (mandrake)  echo 'mandrake' ;;
        (redhat)    echo 'redhat'   ;;
        (suse)      echo 'suse'     ;;
        (debian)    echo 'debian'   ;;
        (unknown)   echo 'pc'       ;;
        (*)         echo "$[1]"       ;;
    esac
}
dist_get_release() {
dnl AC_MSG_WARN([checking for release in $[1]])
    echo "$[1]" | sed -e 's|^[[^0-9.]]*||;s|[[^0-9.]].*$||'
}
dist_get_distrib() {
dnl AC_MSG_WARN([checking for distrib in $[1]])
    case "$[1]" in
        (whitebox)  echo 'White Box Enterprise Linux' ;;
        (fedora)    echo 'Fedora Core' ;;
        (mandrake)  echo 'Mandrake Linux' ;;
        (redhat)    echo 'Red Hat Linux' ;;
        (suse)      echo 'SuSE Linux' ;;
        (debian)    echo 'Debian GNU/Linux' ;;
        (unknown)   echo 'Unknown Linux' ;;
        (*)         echo 'Linux' ;;
    esac
}
dist_get_codename() {
dnl AC_MSG_WARN([checking for codename in $[1]])
    echo "$[1]" | sed -e 's|^.*(|(|;s|).*|)|;s|^[[^(]]*||;s|[[^)]]*[$]||;s|^(||;s|)[$]||'
}
dist_get_cpu() {
dnl AC_MSG_WARN([checking for cpu in $[1]])
    echo "$[1]" | grep '\<for\>' | sed -e 's|^.*\<for\>[[[:space:]]]*||;s|[[[:space:]]].*[$]||'
}
])# _DISTRO_FUNCTIONS
# ===========================================================================

# ===========================================================================
# _DISTRO_SETUP
# ---------------------------------------------------------------------------
#
# NOTE:- for LSB compliant systems we should also be able to find
#   /etc/lsb-release and if we source that file we should see:
#
#    LSB_VERSION=               # the version supported
#    DISTRIB_ID="SuSE"          # the distribution id
#    DISTRIB_RELEASE="8.0"      # the distribution release
#    DISTRIB_DESCRIPTION="SuSE Linux 8.0 (i386)"   # the distribution line
#
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO_SETUP], [dnl
    # first we look for a release info file
    AC_CACHE_CHECK([for dist build lsb release file], [dist_cv_build_lsb_file], [dnl
        eval "dist_search_path=\"
            /etc/lsb-release\""
        dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
        for dist_file in $dist_search_path
        do
            if test -f "$dist_file"
            then
                dist_cv_build_lsb_file="$dist_file"
                break
            fi
        done
        if test -z "$dist_cv_build_lsb_file" ; then
            dist_cv_build_lsb_file='no'
        fi
    ])
    AC_CACHE_CHECK([for dist build release file], [dist_cv_build_rel_file], [dnl
        eval "dist_search_path=\"
            /etc/whitebox-release
            /etc/fedora-release
            /etc/mandrake-release
            /etc/redhat-release
            /etc/SuSE-release
            /etc/debian_version\""
        dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
        for dist_file in $dist_search_path
        do
            if test -f "$dist_file"
            then
                dist_cv_build_rel_file="$dist_file"
                break
            fi
        done
        if test -z "$dist_cv_build_rel_file" ; then
            dist_cv_build_rel_file='no'
        fi
    ])
    AC_CACHE_CHECK([for dist build issue file], [dist_cv_build_issue_file], [dnl
        eval "dist_search_path=\"
            /etc/issue
            /etc/issue.net\""
        dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
        for dist_file in $dist_search_path
        do
            if test -f "$dist_file"
            then
                dist_cv_build_issue_file="$dist_file"
                break
            fi
        done
        if test -z "$dist_cv_build_issue_file" ; then
            dist_cv_build_issue_file='no'
        fi
    ])
    AC_REQUIRE([_DISTRO_FUNCTIONS])
    AC_CACHE_CHECK([for dist build flavor], [dist_cv_build_flavor], [dnl
        dist_tmp=
        if test ":${dist_cv_build_rel_file:-no}" != :no ; then
            dist_cv_build_flavor=$(dist_get_flavor "$(< $dist_cv_build_rel_file)")
        fi
        if test -z "$dist_cv_build_flavor" -a ":${dist_cv_build_lsb_file:-no}" != :no ; then
            . "$dist_cv_build_lsb_file"
            dist_cv_build_flavor=$(dist_get_flavor "${DISTRIB_DESCRIPTION:-unknown}")
            if test -z "$dist_cv_build_flavor" ; then
                dist_cv_build_flavor=$(echo "$DISTRIB_ID" | tr [[:upper:]] [[:lower:]] | sed -e 's|[[[:space:]]]*||g;s|linux||g')
            fi
            unset LSB_RELEASE
            unset DISTRIB_ID
            unset DISTRIB_VENDOR
            unset DISTRIB_RELEASE
            unset DISTRIB_CODENAME
            unset DISTRIB_DESCRIPTION
        fi
        if test -z "$dist_cv_build_flavor" -a ":${dist_cv_build_issue_file:-no}" != :no ; then
            dist_cv_build_flavor=$(dist_get_flavor "$(< $dist_cv_build_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        if test -z "$dist_cv_build_flavor" ; then
            dist_cv_build_flavor=$(dist_get_flavor "$($CC $CFLAGS -v 2>&1 | grep 'gcc version')")
        fi
    ])
    AC_CACHE_CHECK([for dist build vendor], [dist_cv_build_vendor], [dnl
        dist_cv_build_vendor=$(dist_get_vendor "${dist_cv_build_flavor:-unknown}")
    ])
    AC_CACHE_CHECK([for dist build release], [dist_cv_build_release], [dnl
        if test ":${dist_cv_build_rel_file:-no}" != :no ; then
            dist_cv_build_release=$(dist_get_release "$(< $dist_cv_build_rel_file)")
        fi
        if test -z "$dist_cv_build_release" -a ":${dist_cv_build_lsb_file:-no}" != :no ; then
            . "$dist_cv_build_lsb_file"
            dist_cv_build_release=$(dist_get_release "$DISTRIB_RELEASE")
            unset LSB_RELEASE
            unset DISTRIB_ID
            unset DISTRIB_VENDOR
            unset DISTRIB_RELEASE
            unset DISTRIB_CODENAME
            unset DISTRIB_DESCRIPTION
        fi
        if test -z "$dist_cv_build_release" -a ":${dist_cv_build_issue_file:-no}" != :no ; then
            dist_cv_build_release=$(dist_get_release "$(< $dist_cv_build_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        if test -z "$dist_cv_build_release" ; then
            dist_cv_build_release=$(dist_get_release "$($CC $CFLAGS -v 2>&1 | grep 'gcc version' | sed -e 's|.*(||;s|).*||;s| [[^ ]]*$||')")
        fi
    ])
    AC_CACHE_CHECK([for dist build distrib], [dist_cv_build_distrib], [dnl
        dist_cv_build_distrib=$(dist_get_distrib "$dist_cv_build_flavor")
    ])
    AC_CACHE_CHECK([for dist build codename], [dist_cv_build_codename], [dnl
        if test ":${dist_cv_build_rel_file:-no}" != :no ; then
            dist_cv_build_codename=$(dist_get_codename "$(< $dist_cv_build_rel_file)")
        fi
        if test -z "$dist_cv_build_codename" -a ":${dist_cv_build_lsb_file:-no}" != :no ; then
            . "$dist_cv_build_lsb_file"
            dist_cv_build_codename=$(dist_get_codename "(${DISTRIB_CODENAME})")
            unset LSB_RELEASE
            unset DISTRIB_ID
            unset DISTRIB_VENDOR
            unset DISTRIB_RELEASE
            unset DISTRIB_CODENAME
            unset DISTRIB_DESCRIPTION
        fi
        if test -z "$dist_cv_build_codename" -a ":${dist_cv_build_issue_file:-no}" != :no ; then
            dist_cv_build_codename=$(dist_get_codename "$(< $dist_cv_build_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        # cannot get the codename from the compiler
    ])
    AC_CACHE_CHECK([for dist build cpu], [dist_cv_build_cpu], [dnl
        if test ":${dist_cv_build_rel_file:-no}" != :no ; then
            dist_cv_build_cpu=$(dist_get_cpu "$(< $dist_cv_build_rel_file)")
        fi
        # cannot get the cpu from lsb
        if test -z "$dist_cv_build_cpu" -a ":${dist_cv_build_issue_file:-no}" != :no ; then
            dist_cv_build_cpu=$(dist_get_cpu "$(< $dist_cv_build_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        # cannot get the cpu from the compiler
    ])
    AC_CACHE_CHECK([for dist target lsb release file], [dist_cv_target_lsb_file], [dnl
        eval "dist_search_path=\"
            ${rootdir:-$DESTDIR}${sysconfdir}/lsb-release\""
        dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
        for dist_file in $dist_search_path
        do
            if test -f "$dist_file"
            then
                dist_cv_target_lsb_file="$dist_file"
                break
            fi
        done
        if test -z "$dist_cv_target_lsb_file" ; then
            dist_cv_target_lsb_file='no'
        fi
    ])
    AC_CACHE_CHECK([for dist target release file], [dist_cv_target_rel_file], [dnl
        eval "dist_search_path=\"
            ${rootdir:-$DESTDIR}${sysconfdir}/whitebox-release
            ${rootdir:-$DESTDIR}${sysconfdir}/fedora-release
            ${rootdir:-$DESTDIR}${sysconfdir}/mandrake-release
            ${rootdir:-$DESTDIR}${sysconfdir}/redhat-release
            ${rootdir:-$DESTDIR}${sysconfdir}/SuSE-release
            ${rootdir:-$DESTDIR}${sysconfdir}/debian_version\""
        dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
        for dist_file in $dist_search_path
        do
            if test -f "$dist_file"
            then
                dist_cv_target_rel_file="$dist_file"
                break
            fi
        done
        if test -z "$dist_cv_target_rel_file" ; then
            dist_cv_target_rel_file='no'
        fi
    ])
    AC_CACHE_CHECK([for dist target issue file], [dist_cv_target_issue_file], [dnl
        eval "dist_search_path=\"
            ${rootdir:-$DESTDIR}${sysconfdir}/issue
            ${rootdir:-$DESTDIR}${sysconfdir}/issue.net\""
        dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
        for dist_file in $dist_search_path
        do
            if test -f "$dist_file"
            then
                dist_cv_target_issue_file="$dist_file"
                break
            fi
        done
        if test -z "$dist_cv_target_issue_file" ; then
            dist_cv_target_issue_file='no'
        fi
    ])
    AC_REQUIRE([_DISTRO_FUNCTIONS])
    AC_CACHE_CHECK([for dist target flavor], [dist_cv_target_flavor], [dnl
        dist_tmp=
        if test ":${dist_cv_target_rel_file:-no}" != :no ; then
            dist_cv_target_flavor=$(dist_get_flavor "$(< $dist_cv_target_rel_file)")
        fi
        if test -z "$dist_cv_target_flavor" -a ":${dist_cv_target_lsb_file:-no}" != :no ; then
            . "$dist_cv_target_lsb_file"
            dist_cv_target_flavor=$(dist_get_flavor "${DISTRIB_DESCRIPTION:-unknown}")
            if test -z "$dist_cv_target_flavor" ; then
                dist_cv_target_flavor=$(echo "$DISTRIB_ID" | tr [[:upper:]] [[:lower:]] | sed -e 's|[[[:space:]]]*||g;s|linux||g')
            fi
            unset LSB_RELEASE
            unset DISTRIB_ID
            unset DISTRIB_VENDOR
            unset DISTRIB_RELEASE
            unset DISTRIB_CODENAME
            unset DISTRIB_DESCRIPTION
        fi
        if test -z "$dist_cv_target_flavor" -a ":${dist_cv_target_issue_file:-no}" != :no ; then
            dist_cv_target_flavor=$(dist_get_flavor "$(< $dist_cv_target_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        # cannot get target flavor using build system compiler
    ])
    AC_CACHE_CHECK([for dist target vendor], [dist_cv_target_vendor], [dnl
        dist_cv_target_vendor=$(dist_get_vendor "${dist_cv_target_flavor:-unknown}")
    ])
    AC_CACHE_CHECK([for dist target release], [dist_cv_target_release], [dnl
        if test ":${dist_cv_target_rel_file:-no}" != :no ; then
            dist_cv_target_release=$(dist_get_release "$(< $dist_cv_target_rel_file)")
        fi
        if test -z "$dist_cv_target_release" -a ":${dist_cv_target_lsb_file:-no}" != :no ; then
            . "$dist_cv_target_lsb_file"
            dist_cv_target_release=$(dist_get_release "$DISTRIB_RELEASE")
            unset LSB_RELEASE
            unset DISTRIB_ID
            unset DISTRIB_VENDOR
            unset DISTRIB_RELEASE
            unset DISTRIB_CODENAME
            unset DISTRIB_DESCRIPTION
        fi
        if test -z "$dist_cv_target_release" -a ":${dist_cv_target_issue_file:-no}" != :no ; then
            dist_cv_target_release=$(dist_get_release "$(< $dist_cv_target_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        # cannot get target release using build system compiler
    ])
    AC_CACHE_CHECK([for dist target distrib], [dist_cv_target_distrib], [dnl
        dist_cv_target_distrib=$(dist_get_distrib "$dist_cv_target_flavor")
    ])
    AC_CACHE_CHECK([for dist target codename], [dist_cv_target_codename], [dnl
        if test ":${dist_cv_target_rel_file:-no}" != :no ; then
            dist_cv_target_codename=$(dist_get_codename "$(< $dist_cv_target_rel_file)")
        fi
        if test -z "$dist_cv_target_codename" -a ":${dist_cv_target_lsb_file:-no}" != :no ; then
            . "$dist_cv_target_lsb_file"
            dist_cv_target_codename=$(dist_get_codename "(${DISTRIB_CODENAME})")
            unset LSB_RELEASE
            unset DISTRIB_ID
            unset DISTRIB_VENDOR
            unset DISTRIB_RELEASE
            unset DISTRIB_CODENAME
            unset DISTRIB_DESCRIPTION
        fi
        if test -z "$dist_cv_target_codename" -a ":${dist_cv_target_issue_file:-no}" != :no ; then
            dist_cv_target_codename=$(dist_get_codename "$(< $dist_cv_target_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        # cannot get the codename from the compiler
    ])
    AC_CACHE_CHECK([for dist target cpu], [dist_cv_target_cpu], [dnl
        if test ":${dist_cv_target_rel_file:-no}" != :no ; then
            dist_cv_target_cpu=$(dist_get_cpu "$(< $dist_cv_target_rel_file)")
        fi
        # cannot get the cpu from lsb
        if test -z "$dist_cv_target_cpu" -a ":${dist_cv_target_issue_file:-no}" != :no ; then
            dist_cv_target_cpu=$(dist_get_cpu "$(< $dist_cv_target_issue_file | grep 'Linux\|Fedora' | head -1)")
        fi
        # cannot get the cpu from the compiler
    ])
])# _DISTRO_SETUP
# ===========================================================================

# ===========================================================================
# _DISTRO_OUTPUT
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO_OUTPUT], [dnl
# now we adjust the cannonical names
    AC_MSG_CHECKING([build system type])
    case "$dist_cv_build_vendor" in
        (whitebox)
            build_vendor='whitebox'
            build_os='linux'
            build="${build_cpu}-${build_vendor}-${build_os}"
            ac_cv_build="$build"
            ;;
        (redhat)
            build_vendor='redhat'
            build_os='linux'
            build="${build_cpu}-${build_vendor}-${build_os}"
            ac_cv_build="$build"
            ;;
        (mandrake)
            build_vendor='mandrake'
            build_os='linux-gnu'
            build="${build_cpu}-${build_vendor}-${build_os}"
            ac_cv_build="$build"
            ;;
        (suse)
            build_vendor='suse'
            build_os='linux'
            build="${build_cpu}-${build_vendor}-${build_os}"
            ac_cv_build="$build"
            ;;
        (debian)
            build_vendor='pc'
            build_os='linux-gnu'
            build="${build_cpu}-${build_vendor}-${build_os}"
            ac_cv_build="$build"
            ;;
    esac
    AC_MSG_RESULT([$build])
    AC_MSG_CHECKING([host system type])
    case "$dist_cv_target_vendor" in
        (whitebox)
            host_vendor='whitebox'
            host_os='linux'
            host="${host_cpu}-${host_vendor}-${host_os}"
            ac_cv_host="$host"
            ;;
        (redhat)
            host_vendor='redhat'
            host_os='linux'
            host="${host_cpu}-${host_vendor}-${host_os}"
            ac_cv_host="$host"
            ;;
        (mandrake)
            host_vendor='mandrake'
            host_os='linux-gnu'
            host="${host_cpu}-${host_vendor}-${host_os}"
            ac_cv_host="$host"
            ;;
        (suse)
            host_vendor='suse'
            host_os='linux'
            host="${host_cpu}-${host_vendor}-${host_os}"
            ac_cv_host="$host"
            ;;
        (debian)
            host_vendor='pc'
            host_os='linux-gnu'
            host="${host_cpu}-${host_vendor}-${host_os}"
            ac_cv_host="$host"
            ;;
    esac
    AC_MSG_RESULT([$host])
    AC_MSG_CHECKING([target system type])
    case "$dist_cv_target_vendor" in
        (whitebox)
            target_vendor='whitebox'
            target_os='linux'
            target="${target_cpu}-${target_vendor}-${target_os}"
            ac_cv_target="$target"
            ;;
        (redhat)
            target_vendor='redhat'
            target_os='linux'
            target="${target_cpu}-${target_vendor}-${target_os}"
            ac_cv_target="$target"
            ;;
        (mandrake)
            target_vendor='mandrake'
            target_os='linux-gnu'
            target="${target_cpu}-${target_vendor}-${target_os}"
            ac_cv_target="$target"
            ;;
        (suse)
            target_vendor='suse'
            target_os='linux'
            target="${target_cpu}-${target_vendor}-${target_os}"
            ac_cv_target="$target"
            ;;
        (debian)
            target_vendor='pc'
            target_os='linux-gnu'
            target="${target_cpu}-${target_vendor}-${target_os}"
            ac_cv_target="$target"
            ;;
    esac
    AC_MSG_RESULT([$target])
])# _DISTRO_OUTPUT
# ===========================================================================

# ===========================================================================
# _DISTRO_CHECK_VENDOR
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO_CHECK_VENDOR], [dnl
    if test ":$build_os" != ":$host_os" -o ":$build_os" != ":$target_os" ; then
        AC_MSG_WARN([
**** 
**** The build operating system ($build_os) is not the same
**** as the host or target operating system ($host_os or
**** $target_os).  In general this is not a good idea because
**** the tool chain of one operating system might not be
**** compatible with the other.  If you encounter problems
**** later, build on the same operating system as the host and
**** target.
**** ])
    elif test ":$build_vendor" != ":$host_vendor" -o ":$build_vendor" != ":$target_vendor" ; then
        AC_MSG_WARN([
**** 
**** The build distribution ($build_vendor) is not the same
**** as the host or target distribution ($host_vendor or
**** $target_vendor).  In general this is not a good idea because
**** the tool chain of one distribution might not be
**** compatible with the other.  If you encounter problems
**** later, build on the same distribution as the host and
**** target.
**** ])
    fi
])# _DISTRO_CHECK_VENDOR
# ===========================================================================

# ===========================================================================
# _DISTRO_
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_DISTRO_], [dnl
])# _DISTRO_
# ===========================================================================


dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
