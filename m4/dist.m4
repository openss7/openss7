# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: dist.m4,v $ $Name:  $($Revision: 1.1.2.9 $) $Date: 2011-05-31 09:46:01 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2009-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2009  OpenSS7 Corporation <http://www.openss7.com/>
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
# Last Modified $Date: 2011-05-31 09:46:01 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _DISTRO
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO], [dnl
    AC_MSG_NOTICE([+---------------------+])
    AC_MSG_NOTICE([| Distribution Checks |])
    AC_MSG_NOTICE([+---------------------+])
dnl _DISTRO_CHECK_OS
dnl _DISTRO_OPTIONS
    _DISTRO_SETUP
    _DISTRO_OUTPUT
    _DISTRO_CACHE
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
	[AS_HELP_STRING([--with-dist-vendor=VENDOR],
	    [distribution VENDOR @<:@default=auto@:>@])])
    AC_ARG_WITH([dist-flavor],
	[AS_HELP_STRING([--with-dist-flavor=FLAVOR],
	    [distribution FLAVOR @<:@default=auto@:>@])])
    AC_ARG_WITH([dist-release],
	[AS_HELP_STRING([--with-dist-release=RELEASE],
	    [distribution RELEASE @<:@default=auto@:>@])])
])# _DISTRO_OPTIONS
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
	dist_search_path=$(echo "$dist_search_path" | sed 's|\<NONE\>||g;s|//|/|g')
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
	    /etc/manbo-release
	    /etc/mandriva-release
	    /etc/mandrake-release
	    /etc/redhat-release
	    /etc/SuSE-release
	    /etc/system-release
	    /etc/debian_version\""
	dist_search_path=$(echo "$dist_search_path" | sed 's|\<NONE\>||g;s|//|/|g')
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
	dist_search_path=$(echo "$dist_search_path" | sed 's|\<NONE\>||g;s|//|/|g')
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
    AC_REQUIRE_SHELL_FN([dist_get_flavor],
	[AS_FUNCTION_DESCRIBE([dist_get_flavor], [STRING], [Checks the string for linux distribution
	 flavor.  Note: check for capitalized versions now too: SuSE 10 uses SUSE LINUX (all caps
	 for some stupid reason).  Also expect Mandrake to change to Mandriva any day soon.])], [dnl
dnl AC_MSG_WARN([checking for flavor in $[1]])
    case "$[1]" in
	(*CentOS*|*CENTOS*)				echo 'centos'	  ;;
	(*Lineox*|*LINEOX*)				echo 'lineox'	  ;;
	(*White?Box*|*WHITE?BOX*)			echo 'whitebox'	  ;;
	(*Scientific*|*SCIENTIFIC*)			echo 'scientific' ;;
	(*Fedora*|*FEDORA*)				echo 'fedora'	  ;;
	(*Mandrake*|*MANDRAKE*)				echo 'mandrake'	  ;;
	(*Mandriva*|*MANDRIVA*)				echo 'mandriva'	  ;;
	(*Manbo*|*MANBO*)				echo 'manbo'	  ;;
	(*Red?Hat?Enterprise*)				echo 'rhel'	  ;;
	(*Red?Hat*|*RED?HAT*)				echo 'redhat'	  ;;
	(*SUSE?Linux?Enterprise?Server*)		echo 'sles'	  ;;
	(*SUSE?Linux?Enterprise?Desktop*)		echo 'sled'	  ;;
	(*SUSE?Linux?Enterprise*)			echo 'sle'	  ;;
	(*SuSE*|*SUSE*|*Novell*|*NOVELL*)		echo 'suse'	  ;;
	(*Debian*|*DEBIAN*)				echo 'debian'	  ;;
	(*Ubuntu*|*UBUNTU*)				echo 'ubuntu'	  ;;
    esac])
    AC_CACHE_CHECK([for dist build flavor], [dist_cv_build_flavor], [dnl
	if test -z "$dist_cv_build_flavor" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_build_rel_file" | sed 's|.*/||'` != 'debian_version' ; then
		dist_cv_build_flavor=$(dist_get_flavor "$(cat $dist_cv_build_rel_file)")
	    fi
	fi
	if test -z "$dist_cv_build_flavor" -a ":${dist_cv_build_lsb_file:-no}" != :no ; then
	    . "$dist_cv_build_lsb_file"
	    dist_cv_build_flavor=$(dist_get_flavor "${DISTRIB_DESCRIPTION:-unknown}")
	    if test -z "$dist_cv_build_flavor" ; then
		dist_cv_build_flavor=$(echo "$DISTRIB_ID" | tr [[:upper:]] [[:lower:]] | sed 's|[[[:space:]]]*||g;s|linux||g')
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
	    if test `echo "$dist_cv_build_rel_file" | sed 's|.*/||'` = 'debian_version' ; then
		dist_cv_build_flavor='debian'
	    fi
	fi
	if test -z "$dist_cv_build_flavor" ; then
	    dist_cv_build_flavor=$(dist_get_flavor "$(${CC-cc} $CFLAGS -v 2>&1 | grep 'gcc version')")
	fi
    ])
    AC_CACHE_CHECK([for build distro], [dist_cv_build_distro], [dnl
	dist_cv_build_distro=`echo "${dist_cv_build_flavor:-unknown}" | sed -r 's,^sle[[ds]]$$,sle,;s,^opensuse$$,suse,'`
    ])
    AC_REQUIRE_SHELL_FN([dist_get_vendor],
	[AS_FUNCTION_DESCRIBE([dist_get_vendor], [STRING], [Checks the string for linux distribution
	 vendor.])], [dnl
dnl AC_MSG_WARN([checking for vendor in $[1]])
    case "$[1]" in
	(centos)			echo 'centos'	  ;;
	(lineox)			echo 'lineox'	  ;;
	(whitebox)			echo 'whitebox'	  ;;
	(scientific)			echo 'scientific' ;;
	(mandrake)			echo 'mandrake'	  ;;
	(mandriva)			echo 'mandriva'	  ;;
	(manbo)				echo 'manbo'	  ;;
	(fedora|redhat|rhel)		echo 'redhat'	  ;;
	(suse|sle|sles|sled|opensuse)	echo 'suse'	  ;;
	(debian)			echo 'debian'	  ;;
	(ubuntu)			echo 'ubuntu'	  ;;
	(unknown)			echo 'pc'	  ;;
	(*)				echo "$[1]"	  ;;
    esac])
    AC_CACHE_CHECK([for dist build vendor], [dist_cv_build_vendor], [dnl
	dist_cv_build_vendor=$(dist_get_vendor "${dist_cv_build_flavor:-unknown}")
	if test -z "$dist_cv_build_vendor" ; then dist_cv_build_vendor=$build_vendor ; fi
    ])
    AC_REQUIRE_SHELL_FN([dist_get_release],
	[AS_FUNCTION_DESCRIBE([dist_get_release], [STRING], [Checks the string for linux
	 distribution release.])], [dnl
dnl AC_MSG_WARN([checking for release in $[1]])
    echo "$[1]" | head -1 | sed 's|^[[^0-9.]]*||;s|[[^0-9.]].*$||'])
    AC_CACHE_CHECK([for dist build release], [dist_cv_build_release], [dnl
	if test -z "$dist_cv_build_release" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_build_rel_file" | sed 's|.*/||'` in
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
	    case :`echo "$dist_cv_build_rel_file" | sed 's|.*/||'` in
		# do debian version after lsb and issue for Ubuntu
		(:debian_version)
		    dist_cv_build_release=$(dist_get_release "$(cat $dist_cv_build_rel_file)")
		    ;;
	    esac
	fi
	if test -z "$dist_cv_build_release" ; then
	    dist_cv_build_release=$(dist_get_release "$(${CC-cc} $CFLAGS -v 2>&1 | grep 'gcc version' | sed 's|.*(||;s|).*||;s| [[^ ]]*$||')")
	fi
    ])
    AC_REQUIRE_SHELL_FN([dist_get_distrib],
	[AS_FUNCTION_DESCRIBE([dist_get_distrib], [STRING], [Checks the string for linux
	 distribution description.])], [dnl
dnl AC_MSG_WARN([checking for distrib in $[1]])
    case "$[1]" in
	(centos)	echo 'CentOS Enterprise Linux' ;;
	(lineox)	echo 'Lineox Enterprise Linux' ;;
	(whitebox)	echo 'White Box Enterprise Linux' ;;
	(scientific)	echo 'Scientific Linux' ;;
	(fedora)	echo 'Fedora' ;;
	(mandrake)	echo 'Mandrake Linux' ;;
	(mandriva)	echo 'Mandriva Linux' ;;
	(manbo)		echo 'Manbo Linux' ;;
	(redhat)	echo 'Red Hat Linux' ;;
	(rhel)		echo 'Red Hat Enterprise Linux' ;;
	(suse)		echo 'SuSE Linux' ;;
	(sle)		echo 'SUSE Linux Enterprise' ;;
	(sles)		echo 'SUSE Linux Enterprise Server' ;;
	(sled)		echo 'SUSE Linux Enterprise Desktop' ;;
	(debian)	echo 'Debian GNU/Linux' ;;
	(ubuntu)	echo 'Ubuntu' ;;
	(unknown)	echo 'Unknown Linux' ;;
	(*)		echo 'Linux' ;;
    esac])
    AC_CACHE_CHECK([for dist build distrib], [dist_cv_build_distrib], [dnl
	dist_cv_build_distrib=$(dist_get_distrib "$dist_cv_build_flavor")
    ])
    AC_REQUIRE_SHELL_FN([dist_get_codename],
	[AS_FUNCTION_DESCRIBE([dist_get_codename], [STRING], [Checks the string for linux
	 distribution codename.])], [dnl
dnl AC_MSG_WARN([checking for codename in $[1]])
    echo "$[1]" | head -1 | sed 's|^.*(|(|;s|).*|)|;s|^[[^(]]*||;s|[[^)]]*[$]||;s|^(||;s|)[$]||'])
    AC_CACHE_CHECK([for dist build codename], [dist_cv_build_codename], [dnl
	if test -z "$dist_cv_build_codename" -a -x /usr/bin/lsb_release ; then
	    dist_cv_build_codename="`/usr/bin/lsb_release -c -s | sed 's,n/a,,'`"
	fi
	if test -z "$dist_cv_build_codename" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_build_rel_file" | sed 's|.*/||'` in
		(:debian_version)
		    :
		    ;;
		(:SuSE-release)
		    # SuSE never really had a codename, but now they put OSS on or openSUSE OpenSuSE
		    dist_cv_build_codename=`head -1 $dist_cv_build_rel_file | sed 's,^.*\<OSS\>.*,OSS,;t;s,^.*\<openSUSE\>.*,openSUSE,;t;s,.*,,'`
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
	    case :`echo "$dist_cv_build_rel_file" | sed 's|.*/||'` in
		# do debian after lsb for Ubuntu
		(:debian_version)
		    # under debian codenames are tied to release numbers
		    case "$dist_cv_build_release" in
			(3.0) dist_cv_build_codename='Woody' ;;
			(3.1) dist_cv_build_codename='Sarge' ;;
			(4.?) dist_cv_build_codename='Etch' ;;
			(5.?) dist_cv_build_codename='Lenny' ;;
			(6.?) dist_cv_build_codename='Squeeze' ;;
			(7.?) dist_cv_build_codename='Wheezy' ;;
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
    AC_CACHE_CHECK([for dist build edition], [dist_cv_build_edition], [dnl
	case "$dist_cv_build_distro" in
	    (suse)
		dist_tmp=`echo "${dist_cv_build_release}" | sed -r 's,^(9|[[1-9]][[0-9]])\..*[$],\1,'` ;;
	    (centos|lineox|whitebox|scientific|rhel|sle)
		dist_tmp=`echo "${dist_cv_build_release}" | sed -r 's,\..*[$],,'` ;;
	    (fedora|mandrake|mandriva|manbo|redhat)
		dist_tmp="${dist_cv_build_release}" ;;
	    (debian|ubuntu|*)
		dist_tmp="${dist_cv_build_codename}" ;;
	esac
	dist_cv_build_edition=`echo "$dist_tmp" | sed 'y,ABCDEFGHIJKLMNOPQRSTUVWXYZ,abcdefghijklmnopqrstuvwxyz,'`
    ])
    AC_REQUIRE_SHELL_FN([dist_get_cpu],
	[AS_FUNCTION_DESCRIBE([dist_get_cpu], [STRING], [Checks the string for machine cpu.])], [dnl
dnl AC_MSG_WARN([checking for cpu in $[1]])
    echo "$[1]" | grep '\<for\>' | head -1 | sed 's|^.*\<for\>[[[:space:]]]*||;s|[[[:space:]]].*[$]||'])
    AC_CACHE_CHECK([for dist build cpu], [dist_cv_build_cpu], [dnl
	if test -z "$dist_cv_build_cpu" -a ":${dist_cv_build_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_build_rel_file" | sed 's|.*/||'` != 'debian_version' ; then
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
	dist_search_path=$(echo "$dist_search_path" | sed 's|\<NONE\>||g;s|//|/|g')
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
	    ${DESTDIR}${sysconfdir}/manbo-release
	    ${DESTDIR}${sysconfdir}/mandriva-release
	    ${DESTDIR}${sysconfdir}/mandrake-release
	    ${DESTDIR}${sysconfdir}/redhat-release
	    ${DESTDIR}${sysconfdir}/SuSE-release
	    ${DESTDIR}${sysconfdir}/system-release
	    ${DESTDIR}${sysconfdir}/debian_version\""
	dist_search_path=$(echo "$dist_search_path" | sed 's|\<NONE\>||g;s|//|/|g')
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
	dist_search_path=$(echo "$dist_search_path" | sed 's|\<NONE\>||g;s|//|/|g')
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
    AC_CACHE_CHECK([for dist host flavor], [dist_cv_host_flavor], [dnl
	if test -z "$dist_cv_host_flavor" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_host_rel_file" | sed 's|.*/||'` != 'debian_version' ; then
		dist_cv_host_flavor=$(dist_get_flavor "$(cat $dist_cv_host_rel_file)")
	    fi
	fi
	if test -z "$dist_cv_host_flavor" -a ":${dist_cv_host_lsb_file:-no}" != :no ; then
	    . "$dist_cv_host_lsb_file"
	    dist_cv_host_flavor=$(dist_get_flavor "${DISTRIB_DESCRIPTION:-unknown}")
	    if test -z "$dist_cv_host_flavor" ; then
		dist_cv_host_flavor=$(echo "$DISTRIB_ID" | tr [[:upper:]] [[:lower:]] | sed 's|[[[:space:]]]*||g;s|linux||g')
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
	    if test `echo "$dist_cv_host_rel_file" | sed 's|.*/||'` = 'debian_version' ; then
		dist_cv_host_flavor='debian'
	    fi
	fi
	# cannot get host flavor using build system compiler
    ])
    AC_CACHE_CHECK([for host distro], [dist_cv_host_distro], [dnl
	dist_cv_host_distro=`echo "${dist_cv_host_flavor:-unknown}" | sed -r 's,^sle[[ds]]$$,sle,;s,^opensuse$$,suse,'`
    ])
    AC_CACHE_CHECK([for dist host vendor], [dist_cv_host_vendor], [dnl
	dist_cv_host_vendor=$(dist_get_vendor "${dist_cv_host_flavor:-unknown}")
    ])
    AC_CACHE_CHECK([for dist host release], [dist_cv_host_release], [dnl
	if test -z "$dist_cv_host_release" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    case :`echo "$dist_cv_host_rel_file" | sed 's|.*/||'` in
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
	    case :`echo "$dist_cv_host_rel_file" | sed 's|.*/||'` in
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
	    case :`echo "$dist_cv_host_rel_file" | sed 's|.*/||'` in
		(:debian_version)
		    :
		    ;;
		(:SuSE-release)
		    # SuSE never really had a codename, but now they put OSS on or openSUSE OpenSuSE
		    dist_cv_host_codename=`head -1 $dist_cv_host_rel_file | sed 's,^.*\<OSS\>.*,OSS,;t;s,^.*\<openSUSE\>.*,openSUSE,;t;s,.*,,'`
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
	    case :`echo "$dist_cv_host_rel_file" | sed 's|.*/||'` in
		# do debian after lsb for Ubuntu
		(:debian_version)
		    # under debian codenames are tied to release numbers
		    case "$dist_cv_host_release" in
			(3.0) dist_cv_host_codename='Woody' ;;
			(3.1) dist_cv_host_codename='Sarge' ;;
			(4.?) dist_cv_host_codename='Etch' ;;
			(5.?) dist_cv_host_codename='Lenny' ;;
			(6.?) dist_cv_host_codename='Squeeze' ;;
			(7.?) dist_cv_host_codename='Wheezy' ;;
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
    AC_CACHE_CHECK([for dist host edition], [dist_cv_host_edition], [dnl
	case "$dist_cv_host_distro" in
	    (suse)
		dist_tmp=`echo "${dist_cv_host_release}" | sed -r 's,^(9|[[1-9]][[0-9]])\..*[$],\1,'` ;;
	    (centos|lineox|whitebox|scientific|rhel|sle)
		dist_tmp=`echo "${dist_cv_host_release}" | sed -r 's,\..*[$],,'` ;;
	    (fedora|mandrake|mandriva|manbo|redhat)
		dist_tmp="${dist_cv_host_release}" ;;
	    (debian|ubuntu|*)
		dist_tmp="${dist_cv_host_codename}" ;;
	esac
	dist_cv_host_edition=`echo "$dist_tmp" | sed 'y,ABCDEFGHIJKLMNOPQRSTUVWXYZ,abcdefghijklmnopqrstuvwxyz,'`
    ])
    AC_CACHE_CHECK([for dist host cpu], [dist_cv_host_cpu], [dnl
	if test -z "$dist_cv_host_cpu" -a ":${dist_cv_host_rel_file:-no}" != :no ; then
	    if test `echo "$dist_cv_host_rel_file" | sed 's|.*/||'` != 'debian_version' ; then
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
			    dist_tmp=`echo "$host_os" | sed 's|-gnu||'`
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
	# if performing an RPM build, RPM tells us
	if test -z "$dist_cv_host_cpu" -a :"${RPM_ARCH+set}" = :set ; then
	    dist_cv_host_cpu="$RPM_ARCH"
	fi
	# if performing an DPKG build, DPKG tells us
	if test -z "$dist_cv_host_cpu" -a :"${DEB_HOST_ARCH+set}" = :set ; then
	    dist_cv_host_cpu="$DEB_HOST_ARCH"
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
	(centos|lineox|whitebox|scientific|redhat|suse|debian)  
	    case "$build_os" in (*linux*) build_os='linux'     ;; esac ;;
	(mandrake|mandriva|manbo|ubuntu)  
	    case "$build_os" in (*linux*) build_os='linux-gnu' ;; esac ;;
    esac
    build_distro="${dist_cv_build_distro:-unknown}"
    build_edition="${dist_cv_build_edition:-unknown}"
    build_distos="${build_distro}-${build_edition}-${build_cpu}"
    AC_SUBST([build_distro])dnl
    AC_SUBST([build_edition])dnl
    AC_SUBST([build_distos])dnl
    build="${build_cpu}-${build_vendor}-${build_os}"
    AC_MSG_RESULT([$build ($build_distos)])
    AC_MSG_CHECKING([host system type])
    if test -z "$host_alias" ; then
	host_cpu="$build_cpu"
	host_vendor="$build_vendor"
	host_os="$build_os"
	host_distro="$build_distro"
	host_edition="$build_edition"
    else
	host_vendor="$dist_cv_host_vendor"
	case "$host_vendor" in
	    (centos|lineox|whitebox|scientific|redhat|suse|debian)  
		case "$host_os" in (*linux*) host_os='linux'     ;; esac ;;
	    (mandrake|mandriva|manbo|ubuntu)  
		case "$host_os" in (*linux*) host_os='linux-gnu' ;; esac ;;
	esac
	host_distro="${dist_cv_host_distro:-unknown}"
	host_edition="${dist_cv_host_edition:-unknown}"
    fi
    AC_SUBST([host_distro])dnl
    AC_SUBST([host_edition])dnl
    AC_SUBST([host_distos])dnl
    host_distos="${host_distro}-${host_edition}-${host_cpu}"
    host="${host_cpu}-${host_vendor}-${host_os}"
    AC_MSG_RESULT([$host ($host_distos)])
    AC_MSG_CHECKING([target system type])
    if test -z "$target_alias" ; then
	target_cpu="$host_cpu"
	target_vendor="$host_vendor"
	target_os="$host_os"
    else
	target_vendor="$dist_cv_host_vendor"
	case "$target_vendor" in
	    (centos|lineox|whitebox|scientific|redhat|suse|debian)  
		case "$target_os" in (*linux*) target_os='linux'     ;; esac ;;
	    (mandrake|mandriva|manbo|ubuntu)  
		case "$target_os" in (*linux*) target_os='linux-gnu' ;; esac ;;
	esac
    fi
    target_distro="$host_distro"
    target_edition="$host_edition"
    target_distos="${target_distro}-${target_edition}-${target_cpu}"
    AC_SUBST([target_distro])dnl
    AC_SUBST([target_edition])dnl
    AC_SUBST([target_distos])dnl
    target="${target_cpu}-${target_vendor}-${target_os}"
    AC_MSG_RESULT([$target ($target_distos)])
])# _DISTRO_OUTPUT
# =============================================================================

# =============================================================================
# _DISTRO_CHECK_VENDOR
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_CHECK_VENDOR], [dnl
    if test :"`echo $build_os | sed s'|-gnu$||'`" != :"`echo $host_os | sed s'|-gnu$||'`" -o \
	:"`echo $build_os | sed s'|-gnu$||'`" != :"`echo $target_os | sed s'|-gnu$||'`" ; then
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
    if test ":$build_vendor" != ":$host_vendor" -o ":$build_vendor" != ":$target_vendor" ; then
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
	[AS_HELP_STRING([--disable-32bit-libs],
	    [32bit compatibility test binaries @<:@default=enabled@:>@])])
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
# _DISTRO_CACHE
# -----------------------------------------------------------------------------
# When performing a config.status --recheck, re-performing all of the distro
# configuration checks is too time consuming to the development cycle.
# Therefore, when we are performing a recheck and we have established all of the
# information necessary to provide a distro- specific cache file name, read that
# file name if it exists.
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_CACHE], [dnl
    # if distro file is not specified, use local distro file
    if test -z "$CONFIG_DIST" ; then
	CONFIG_DIST="${target}-config.cache"
    fi
    AC_SUBST([CONFIG_DIST])dnl
    if test "$no_create" = yes ; then
	for config_dist in $CONFIG_DIST ; do
	    case "$config_dist" in
		(/*) ;;
		(*) config_dist="`pwd`/$config_dist" ;;
	    esac
	    if test -r "$config_dist" ; then
		AC_MSG_NOTICE([reading cache file... $config_dist])
		. "$config_dist"
	    fi
	done
    fi
    AC_CONFIG_COMMANDS([distconfig], [dnl
	if test -n "$CONFIG_DIST" ; then
	    for config_dist in $CONFIG_DIST ; do
		if test -w "$config_dist" -o ! -e "$config_dist" ; then
		    if touch "$config_dist" >/dev/null 2>&1 ; then
			cat "$cache_file" | egrep "^(test \"\\\[$]{)?(ac_cv_|acl_cv_|am_cv_|ap_cv_|bld_cv_|deb_cv_|devfs_cv_|dist_cv_|gl_cv_|gt_cv_|init_cv_|lt_cv_|nls_cv_|perl_cv_|pkg_cv_|rpm_cv_|snmp_cv_|tcl_cv_)" > "$config_dist" 2>/dev/null
		    fi
		fi
	    done
	fi], [dnl
cache_file="$cache_file"
CONFIG_DIST="$CONFIG_DIST"
    ])
])# _DISTRO_CACHE
# =============================================================================

# =============================================================================
# _DISTRO_
# -----------------------------------------------------------------------------
AC_DEFUN([_DISTRO_], [dnl
])# _DISTRO_
# =============================================================================

# =============================================================================
#
# $Log: dist.m4,v $
# Revision 1.1.2.9  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.8  2011-03-26 04:28:45  brian
# - updates to build process
#
# Revision 1.1.2.7  2011-03-06 08:57:20  brian
# - repository updates
#
# Revision 1.1.2.6  2011-02-28 19:51:29  brian
# - better repository build
#
# Revision 1.1.2.5  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.4  2011-01-12 03:49:24  brian
# - support for RHEL 6 kernel
#
# Revision 1.1.2.3  2010-11-28 13:55:51  brian
# - update build requirements, proper autoconf functions, build updates
#
# Revision 1.1.2.2  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# Revision 0.9.2.34  2008-09-19 05:18:45  brian
# - separate repo directory by architecture
#
# Revision 0.9.2.33  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.32  2007/08/12 19:05:30  brian
# - rearrange and update headers
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
