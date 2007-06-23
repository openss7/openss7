# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: dist.m4,v $ $Name:  $($Revision: 0.9.2.30 $) $Date: 2007/06/23 10:13:11 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/06/23 10:13:11 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _DISTRO
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO], [dnl
dnl _DISTRO_CHECK_OS
dnl _DISTRO_OPTIONS
    _DISTRO_SETUP
    _DISTRO_OUTPUT
    _DISTRO_CHECK_VENDOR
    _DISTRO_ADJUST_64BIT_LIBDIR
])# _DISTRO
# =============================================================================

# =============================================================================
# _DISTRO_CHECK_OS
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_CHECK_OS], [dnl
    case "$build_os" in
	(linux|linux-gnu) ;;
	(*) AC_MSG_WARN([
*** 
*** The build system is $build_os  It is not wise to build for Linux on
*** a system other than Linux.  If you have problems later, please build
*** on a Linux system.
*** ]) ;;
    esac
    case "$host_os" in
	(linux|linux-gnu) ;;
	(*) AC_MSG_ERROR([
*** 
*** Sorry, this package only builds for Linux.  The host system is
*** $host_os  Please specify a linux host with the --host option to
*** configure.
*** ]) ;;
    esac
    case "$target_os" in
	(linux|linux-gnu) ;;
	(*) AC_MSG_ERROR([
*** 
*** Sorry, this package only builds for Linux.  The target system is
*** $target_os  Please specify a linux host with the --host or or
*** --target to configure.
*** ]) ;;
    esac
])# _DISTRO_CHECK_OS
# =============================================================================

# =============================================================================
# _DISTRO_OPTIONS
# -----------------------------------------------------------------------------
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
# =============================================================================

# =============================================================================
# _DISTRO_FUNCTIONS
# -----------------------------------------------------------------------------
# Note: check for capitalized versions now too: SuSE 10 uses SUSE LINUX (all caps
# for some stupid reaason).  Also, expect Mandrake to change to Mandriva any day
# soon.
# -----------------------------------------------------------------------------
AC_DEFUN_ONCE([_DISTRO_FUNCTIONS], [dnl
dist_get_flavor() {
dnl AC_MSG_WARN([checking for flavor in $[1]])
    case "$[1]" in
	(*CentOS*|*CENTOS*)				echo 'centos'	;;
	(*Lineox*|*LINEOX*)				echo 'lineox'	;;
	(*White?Box*|*WHITE?BOX*)			echo 'whitebox'	;;
	(*Fedora*|*FEDORA*)				echo 'fedora'	;;
	(*Mandrake*|*Mandriva*|*MANDRAKE*|*MANDRIVA*)	echo 'mandrake'	;;
	(*Red?Hat*|*RED?HAT*)				echo 'redhat'	;;
	(*SuSE*|*SUSE*|*Novell*|*NOVELL*)		echo 'suse'	;;
	(*Debian*|*DEBIAN*)				echo 'debian'	;;
	(*Ubuntu*|*UBUNTU*)				echo 'ubuntu'	;;
    esac
}
dist_get_vendor() {
dnl AC_MSG_WARN([checking for vendor in $[1]])
    case "$[1]" in
	(centos)	echo 'centos'	;;
	(lineox)	echo 'lineox'	;;
	(whitebox)	echo 'whitebox'	;;
	(fedora)	echo 'redhat'	;;
	(mandrake)	echo 'mandrake'	;;
	(redhat)	echo 'redhat'	;;
	(suse)		echo 'suse'	;;
	(debian)	echo 'debian'	;;
	(ubuntu)	echo 'ubuntu'	;;
	(unknown)	echo 'pc'	;;
	(*)		echo "$[1]"	;;
    esac
}
dist_get_release() {
dnl AC_MSG_WARN([checking for release in $[1]])
    echo "$[1]" | head -1 | sed -e 's|^[[^0-9.]]*||;s|[[^0-9.]].*$||'
}
dist_get_distrib() {
dnl AC_MSG_WARN([checking for distrib in $[1]])
    case "$[1]" in
	(centos)	echo 'CentOS Enterprise Linux' ;;
	(lineox)	echo 'Lineox Enterprise Linux' ;;
	(whitebox)	echo 'White Box Enterprise Linux' ;;
	(fedora)	echo 'Fedora' ;;
	(mandrake)	echo 'Mandrake Linux' ;;
	(redhat)	echo 'Red Hat Linux' ;;
	(suse)		echo 'SuSE Linux' ;;
	(debian)	echo 'Debian GNU/Linux' ;;
	(ubuntu)	echo 'Ubuntu' ;;
	(unknown)	echo 'Unknown Linux' ;;
	(*)		echo 'Linux' ;;
    esac
}
dist_get_codename() {
dnl AC_MSG_WARN([checking for codename in $[1]])
    echo "$[1]" | head -1 | sed -e 's|^.*(|(|;s|).*|)|;s|^[[^(]]*||;s|[[^)]]*[$]||;s|^(||;s|)[$]||'
}
dist_get_cpu() {
dnl AC_MSG_WARN([checking for cpu in $[1]])
    echo "$[1]" | grep '\<for\>' | head -1 | sed -e 's|^.*\<for\>[[[:space:]]]*||;s|[[[:space:]]].*[$]||'
}
])# _DISTRO_FUNCTIONS
# =============================================================================

# =============================================================================
# _DISTRO_SETUP
# -----------------------------------------------------------------------------
#
# NOTE:- for LSB compliant systems we should also be able to find
#   /etc/lsb-release and if we source that file we should see:
#
#    LSB_VERSION=               # the version supported
#    DISTRIB_ID="SuSE"          # the distribution id
#    DISTRIB_RELEASE="8.0"      # the distribution release
#    DISTRIB_DESCRIPTION="SuSE Linux 8.0 (i386)"   # the distribution line
#
# -----------------------------------------------------------------------------
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
	    /etc/centos-release
	    /etc/lineox-release
	    /etc/whitebox-release
	    /etc/fedora-release
	    /etc/mandrake-release
	    /etc/mandriva-release
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
	if test -z "$dist_cv_build_flavor" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` != 'debian_version' ; then
		dist_cv_build_flavor=$(dist_get_flavor "$(cat $dist_cv_build_rel_file)")
	    fi
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
	    dist_cv_build_flavor=$(dist_get_flavor "$(cat $dist_cv_build_issue_file | grep 'Linux\|Fedora\|Ubuntu' | head -1)")
	fi
	# do debian after lsb and issue for Ubuntu
	if test -z "$dist_cv_build_flavor" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` = 'debian_version' ; then
		dist_cv_build_flavor='debian'
	    fi
	fi
	if test -z "$dist_cv_build_flavor" ; then
	    dist_cv_build_flavor=$(dist_get_flavor "$(${CC-cc} $CFLAGS -v 2>&1 | grep 'gcc version')")
	fi
    ])
    AC_CACHE_CHECK([for dist build vendor], [dist_cv_build_vendor], [dnl
	dist_cv_build_vendor=$(dist_get_vendor "${dist_cv_build_flavor:-unknown}")
	if test -z "$dist_cv_build_vendor" ; then dist_cv_build_vendor=$build_vendor ; fi
    ])
    AC_CACHE_CHECK([for dist build release], [dist_cv_build_release], [dnl
	if test -z "$dist_cv_build_release" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` in
		(:debian_version)
		    : # do debian version after lsb and issue for Ubuntu
		    ;;
		(:*)
		    dist_cv_build_release=$(dist_get_release "$(cat $dist_cv_build_rel_file)")
		    ;;
	    esac
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
	    dist_cv_build_release=$(dist_get_release "$(cat $dist_cv_build_issue_file | grep 'Linux\|Fedora\|Ubuntu' | head -1)")
	fi
	if test -z "$dist_cv_build_release" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` in
		# do debian version after lsb and issue for Ubuntu
		(:debian_version)
		    dist_cv_build_release=$(dist_get_release "$(cat $dist_cv_build_rel_file)")
		    ;;
	    esac
	fi
	if test -z "$dist_cv_build_release" ; then
	    dist_cv_build_release=$(dist_get_release "$(${CC-cc} $CFLAGS -v 2>&1 | grep 'gcc version' | sed -e 's|.*(||;s|).*||;s| [[^ ]]*$||')")
	fi
    ])
    AC_CACHE_CHECK([for dist build distrib], [dist_cv_build_distrib], [dnl
	dist_cv_build_distrib=$(dist_get_distrib "$dist_cv_build_flavor")
    ])
    AC_CACHE_CHECK([for dist build codename], [dist_cv_build_codename], [dnl
	if test -z "$dist_cv_build_codename" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` in
		(:debian_version)
		    :
		    ;;
		(:SuSE-release)
		    # SuSE never really had a codename, but now they put OSS on OpenSuSE
		    dist_cv_build_codename=`head -1 $dist_cv_build_rel_file | sed -e 's|^.*\<OSS\>.*|OSS|'`
		    # Now they put openSUSE
		    dist_cv_build_codename=`echo "$dist_cv_build_codename" | sed -e 's|^.*\<openSUSE\>|openSUSE|'`
		    ;;
		(:*)
		    dist_cv_build_codename=$(dist_get_codename "$(cat $dist_cv_build_rel_file)")
		    ;;
	    esac
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
	if test -z "$dist_cv_build_codename" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` in
		# do debian after lsb for Ubuntu
		(:debian_version)
		    # under debian codenames are tied to release numbers
		    case "$dist_cv_build_release" in
			(3.0) dist_cv_build_codename='Woody' ;;
			(3.1) dist_cv_build_codename='Sarge' ;;
			(4.0) dist_cv_build_codename='Etch' ;;
			(*)   dist_cv_build_codename='unknown' ;;
		    esac
		    ;;
	    esac
	fi
	if test -z "$dist_cv_build_codename" -a ":${dist_cv_build_issue_file:-no}" != :no ; then
	    dist_cv_build_codename=$(dist_get_codename "$(cat $dist_cv_build_issue_file | grep 'Linux\|Fedora' | head -1)")
	fi
	# cannot get the codename from the compiler
    ])
    AC_CACHE_CHECK([for dist build cpu], [dist_cv_build_cpu], [dnl
	if test -z "$dist_cv_build_cpu" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_build_rel_file" | sed -e 's|.*/||'` != 'debian_version' ; then
		dist_cv_build_cpu=$(dist_get_cpu "$(cat $dist_cv_build_rel_file)")
	    else
		if test :${DEB_BUILD_ARCH+set} = :set ; then
		    dist_cv_build_cpu="$DEB_BUILD_ARCH"
		elif test -x /usr/bin/dpkg-architecture ; then
		    dist_cv_build_cpu=`LANG=C /usr/bin/dpkg-architecture -f -qDEB_BUILD_ARCH 2>/dev/null`
		fi
	    fi
	fi
	# cannot get the cpu from lsb
	if test -z "$dist_cv_build_cpu" -a ":${dist_cv_build_issue_file:-no}" != :no ; then
	    dist_cv_build_cpu=$(dist_get_cpu "$(cat $dist_cv_build_issue_file | grep 'Linux\|Fedora' | head -1)")
	fi
	# cannot get the cpu from the compiler
	if test -z "$dist_cv_build_cpu" ; then dist_cv_build_cpu=$build_cpu ; fi
    ])
    AC_CACHE_CHECK([for dist host lsb release file], [dist_cv_host_lsb_file], [dnl
	eval "dist_search_path=\"
	    ${DESTDIR}${sysconfdir}/lsb-release\""
	dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
	for dist_file in $dist_search_path
	do
	    if test -f "$dist_file"
	    then
		dist_cv_host_lsb_file="$dist_file"
		break
	    fi
	done
	if test -z "$dist_cv_host_lsb_file" ; then
	    dist_cv_host_lsb_file='no'
	fi
    ])
    AC_CACHE_CHECK([for dist host release file], [dist_cv_host_rel_file], [dnl
	eval "dist_search_path=\"
	    ${DESTDIR}${sysconfdir}/centos-release
	    ${DESTDIR}${sysconfdir}/lineox-release
	    ${DESTDIR}${sysconfdir}/whitebox-release
	    ${DESTDIR}${sysconfdir}/fedora-release
	    ${DESTDIR}${sysconfdir}/mandrake-release
	    ${DESTDIR}${sysconfdir}/mandriva-release
	    ${DESTDIR}${sysconfdir}/redhat-release
	    ${DESTDIR}${sysconfdir}/SuSE-release
	    ${DESTDIR}${sysconfdir}/debian_version\""
	dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
	for dist_file in $dist_search_path
	do
	    if test -f "$dist_file"
	    then
		dist_cv_host_rel_file="$dist_file"
		break
	    fi
	done
	if test -z "$dist_cv_host_rel_file" ; then
	    dist_cv_host_rel_file='no'
	fi
    ])
    AC_CACHE_CHECK([for dist host issue file], [dist_cv_host_issue_file], [dnl
	eval "dist_search_path=\"
	    ${DESTDIR}${sysconfdir}/issue
	    ${DESTDIR}${sysconfdir}/issue.net\""
	dist_search_path=$(echo "$dist_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g')
	for dist_file in $dist_search_path
	do
	    if test -f "$dist_file"
	    then
		dist_cv_host_issue_file="$dist_file"
		break
	    fi
	done
	if test -z "$dist_cv_host_issue_file" ; then
	    dist_cv_host_issue_file='no'
	fi
    ])
    AC_REQUIRE([_DISTRO_FUNCTIONS])
    AC_CACHE_CHECK([for dist host flavor], [dist_cv_host_flavor], [dnl
	if test -z "$dist_cv_host_flavor" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` != 'debian_version' ; then
		dist_cv_host_flavor=$(dist_get_flavor "$(cat $dist_cv_host_rel_file)")
	    fi
	fi
	if test -z "$dist_cv_host_flavor" -a ":${dist_cv_host_lsb_file:-no}" != :no ; then
	    . "$dist_cv_host_lsb_file"
	    dist_cv_host_flavor=$(dist_get_flavor "${DISTRIB_DESCRIPTION:-unknown}")
	    if test -z "$dist_cv_host_flavor" ; then
		dist_cv_host_flavor=$(echo "$DISTRIB_ID" | tr [[:upper:]] [[:lower:]] | sed -e 's|[[[:space:]]]*||g;s|linux||g')
	    fi
	    unset LSB_RELEASE
	    unset DISTRIB_ID
	    unset DISTRIB_VENDOR
	    unset DISTRIB_RELEASE
	    unset DISTRIB_CODENAME
	    unset DISTRIB_DESCRIPTION
	fi
	if test -z "$dist_cv_host_flavor" -a ":${dist_cv_host_issue_file:-no}" != :no ; then
	    dist_cv_host_flavor=$(dist_get_flavor "$(cat $dist_cv_host_issue_file | grep 'Linux\|Fedora\|Ubuntu' | head -1)")
	fi
	if test -z "$dist_cv_host_flavor" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    # do debian after lsb and issue for Ubuntu
	    if test `echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` = 'debian_version' ; then
		dist_cv_host_flavor='debian'
	    fi
	fi
	# cannot get host flavor using build system compiler
    ])
    AC_CACHE_CHECK([for dist host vendor], [dist_cv_host_vendor], [dnl
	dist_cv_host_vendor=$(dist_get_vendor "${dist_cv_host_flavor:-unknown}")
    ])
    AC_CACHE_CHECK([for dist host release], [dist_cv_host_release], [dnl
	if test -z "$dist_cv_host_release" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` in
		(:debian_version)
		    : # do debian version after lsb and issue for Ubuntu
		    ;;
		(:*)
		    dist_cv_host_release=$(dist_get_release "$(cat $dist_cv_host_rel_file)")
		    ;;
	    esac
	fi
	if test -z "$dist_cv_host_release" -a ":${dist_cv_host_lsb_file:-no}" != :no ; then
	    . "$dist_cv_host_lsb_file"
	    dist_cv_host_release=$(dist_get_release "$DISTRIB_RELEASE")
	    unset LSB_RELEASE
	    unset DISTRIB_ID
	    unset DISTRIB_VENDOR
	    unset DISTRIB_RELEASE
	    unset DISTRIB_CODENAME
	    unset DISTRIB_DESCRIPTION
	fi
	if test -z "$dist_cv_host_release" -a ":${dist_cv_host_issue_file:-no}" != :no ; then
	    dist_cv_host_release=$(dist_get_release "$(cat $dist_cv_host_issue_file | grep 'Linux\|Fedora\|Ubuntu' | head -1)")
	fi
	if test -z "$dist_cv_host_release" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` in
		# do debian version after lsb and issue for Ubuntu
		(:debian_version)
		    dist_cv_host_release=$(dist_get_release "$(cat $dist_cv_host_rel_file)")
		    ;;
	    esac
	fi
	# cannot get host release using build system compiler
	if test -z "$dist_cv_host_vendor" ; then dist_cv_host_vendor=$host_vendor ; fi
    ])
    AC_CACHE_CHECK([for dist host distrib], [dist_cv_host_distrib], [dnl
	dist_cv_host_distrib=$(dist_get_distrib "$dist_cv_host_flavor")
    ])
    AC_CACHE_CHECK([for dist host codename], [dist_cv_host_codename], [dnl
	if test -z "$dist_cv_host_codename" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` in
		(:debian_version)
		    :
		    ;;
		(:SuSE-release)
		    # SuSE never really had a codename, but now they put OSS on OpenSuSE
		    dist_cv_host_codename=`head -1 $dist_cv_host_rel_file | sed -e 's|^.*\<OSS\>.*|OSS|'`
		    # Now they put openSUSE
		    dist_cv_host_codename=`echo "$dist_cv_host_codename" | sed -e 's|^.*\<openSUSE\>|openSUSE|'`
		    ;;
		(:*)
		    dist_cv_host_codename=$(dist_get_codename "$(cat $dist_cv_host_rel_file)")
		    ;;
	    esac
	fi
	if test -z "$dist_cv_host_codename" -a ":${dist_cv_host_lsb_file:-no}" != :no ; then
	    . "$dist_cv_host_lsb_file"
	    dist_cv_host_codename=$(dist_get_codename "(${DISTRIB_CODENAME})")
	    unset LSB_RELEASE
	    unset DISTRIB_ID
	    unset DISTRIB_VENDOR
	    unset DISTRIB_RELEASE
	    unset DISTRIB_CODENAME
	    unset DISTRIB_DESCRIPTION
	fi
	if test -z "$dist_cv_host_codename" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` in
		# do debian after lsb for Ubuntu
		(:debian_version)
		    # under debian codenames are tied to release numbers
		    case "$dist_cv_host_release" in
			(3.0) dist_cv_host_codename='Woody' ;;
			(3.1) dist_cv_host_codename='Sarge' ;;
			(4.0) dist_cv_host_codename='Etch' ;;
			(*)   dist_cv_host_codename='unknown' ;;
		    esac
		    ;;
	    esac
	fi
	if test -z "$dist_cv_host_codename" -a ":${dist_cv_host_issue_file:-no}" != :no ; then
	    dist_cv_host_codename=$(dist_get_codename "$(cat $dist_cv_host_issue_file | grep 'Linux\|Fedora' | head -1)")
	fi
	# cannot get the codename from the compiler
    ])
    AC_CACHE_CHECK([for dist host cpu], [dist_cv_host_cpu], [dnl
	if test -z "$dist_cv_host_cpu" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_host_rel_file" | sed -e 's|.*/||'` != 'debian_version' ; then
		dist_cv_host_cpu=$(dist_get_cpu "$(cat $dist_cv_host_rel_file)")
	    else
		if test :${DEB_HOST_ARCH+set} = :set ; then
		    dist_cv_host_cpu="$DEB_HOST_ARCH"
		elif test -x /usr/bin/dpkg-architecture ; then
		    case "$dist_cv_host_flavor" in
			# debian needs the -gnu stripped, ubuntu does not
			(ubuntu)
			    dist_tmp="$host_os"
			    ;;
			(debian|*)
			    dist_tmp=`echo "$host_os" | sed -e 's|-gnu||'`
			    ;;
		    esac
		    dist_cv_host_cpu=`LANG=C /usr/bin/dpkg-architecture -a${host_cpu} -t${host_cpu}-${dist_tmp} -qDEB_HOST_ARCH 2>/dev/null`
		fi
	    fi
	fi
	# cannot get the cpu from lsb
	if test -z "$dist_cv_host_cpu" -a ":${dist_cv_host_issue_file:-no}" != :no ; then
	    dist_cv_host_cpu=$(dist_get_cpu "$(cat $dist_cv_host_issue_file | grep 'Linux\|Fedora' | head -1)")
	fi
	# cannot get the cpu from the compiler
	if test -z "$dist_cv_host_cpu" ; then dist_cv_host_cpu=$host_cpu ; fi
    ])
])# _DISTRO_SETUP
# =============================================================================

# =============================================================================
# _DISTRO_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_OUTPUT], [dnl
# now we adjust the cannonical names
    AC_MSG_CHECKING([build system type])
    build_vendor="$dist_cv_build_vendor"
    case "$build_vendor" in
	(centos|lineox|whitebox|redhat|suse|debian)  
	    case "$build_os" in (*linux*) build_os='linux'     ;; esac ;;
	(mandrake|ubuntu)  
	    case "$build_os" in (*linux*) build_os='linux-gnu' ;; esac ;;
    esac
    build="${build_cpu}-${build_vendor}-${build_os}"
    AC_MSG_RESULT([$build])
    AC_MSG_CHECKING([host system type])
    if test -z "$host_alias" ; then
	host_cpu="$build_cpu"
	host_vendor="$build_vendor"
	host_os="$build_os"
    else
	host_vendor="$dist_cv_host_vendor"
	case "$host_vendor" in
	    (centos|lineox|whitebox|redhat|suse|debian)  
		case "$host_os" in (*linux*) host_os='linux'     ;; esac ;;
	    (mandrake|ubuntu)  
		case "$host_os" in (*linux*) host_os='linux-gnu' ;; esac ;;
	esac
    fi
    host="${host_cpu}-${host_vendor}-${host_os}"
    AC_MSG_RESULT([$host])
    AC_MSG_CHECKING([target system type])
    if test -z "$target_alias" ; then
	target_cpu="$host_cpu"
	target_vendor="$host_vendor"
	target_os="$host_os"
    else
	target_vendor="$dist_cv_host_vendor"
	case "$target_vendor" in
	    (centos|lineox|whitebox|redhat|suse|debian)  
		case "$target_os" in (*linux*) target_os='linux'     ;; esac ;;
	    (mandrake|ubuntu)  
		case "$target_os" in (*linux*) target_os='linux-gnu' ;; esac ;;
	esac
    fi
    target="${target_cpu}-${target_vendor}-${target_os}"
    AC_MSG_RESULT([$target])
])# _DISTRO_OUTPUT
# =============================================================================

# =============================================================================
# _DISTRO_CHECK_VENDOR
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_CHECK_VENDOR], [dnl
    if test :"`echo $build_os | sed -e s'|-gnu$||'`" != :"`echo $host_os | sed -e s'|-gnu$||'`" -o \
	:"`echo $build_os | sed -e s'|-gnu$||'`" != :"`echo $target_os | sed -e s'|-gnu$||'`" ; then
	AC_MSG_WARN([
*** 
*** The build operating system ($build_os) is not the same as the host
*** or target operating system ($host_os or $target_os).  In general
*** this is not a good idea because the tool chain of one operating
*** system might not be compatible with the other.  If you encounter
*** problems later, build on the same operating system as the host and
*** target.
*** ])
    fi
    if test :"$build_vendor" != ":$host_vendor" -o ":$build_vendor" != ":$target_vendor" ; then
	if test :"${cross_compiling:-no}" = :no
	then
	    AC_MSG_WARN([
*** 
*** The build distribution ($build_vendor) is not the same as the host
*** or target distribution ($host_vendor or $target_vendor).  In general
*** this is not a good idea because the tool chain of one distribution
*** might not be compatible with the other.  If you encounter problems
*** later, build on the same distribution as the host and target.
*** ])
	fi
    fi
])# _DISTRO_CHECK_VENDOR
# =============================================================================
#
# =============================================================================
# _DISTRO_ADJUST_64BIT_LIBDIR
# -----------------------------------------------------------------------------
# adjust default lib directory for 64 bit
# Yes, this is a strange place to put this...
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_ADJUST_64BIT_LIBDIR], [dnl
    AC_ARG_ENABLE([32bit-libs],
	AS_HELP_STRING([--disable-32bit-libs],
	    [disable 32bit compatibility libraries (and test binaries) on
	     64-bit processors.  @<:@default=enabled@:>@]), [:], [:])
    have_32bit_libs=no
    lib32dir="$libdir"
    pkglib32dir="$pkglibdir"
    pkglibexec32dir="$pkglibexecdir"
    case $host_cpu in
	(*64)
	    lib64dir=`echo $libdir | sed -r -e 's|\<lib\>|lib64|g'`
	    lib32dir=`echo $libdir | sed -r -e 's|\<lib64\>|lib|g'`
	    libdir="$lib64dir"
	    pkglib32dir='${lib32dir}/${PACKAGE}'
	    pkglibexec32dir='${pkglibexecdir}/lib32'
	    have_32bit_libs=yes
	    ;;
    esac
    AC_CACHE_CHECK([for 32bit libs], [dist_cv_32bit_libs], [dnl
	dist_cv_32bit_libs=no
	if test :$have_32bit_libs = :yes ; then
	    if test :${enable_32bit_libs:-yes} = :yes ; then
		dist_cv_32bit_libs=yes
	    fi
	fi
    ])
    AM_CONDITIONAL([WITH_32BIT_LIBS],[test ":${dist_cv_32bit_libs:-no}" != :no])dnl
    AC_SUBST([lib32dir])dnl
    AC_SUBST([pkglib32dir])dnl
    AC_SUBST([pkglibexec32dir])dnl
])# _DISTRO_ADJUST_64BIT_LIBDIR
# =============================================================================

# =============================================================================
# _DISTRO_
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_], [dnl
])# _DISTRO_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
