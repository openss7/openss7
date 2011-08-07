# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: repo.m4,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2011-08-07 11:14:37 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date: 2011-08-07 11:14:37 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _REPO
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO], [dnl
    AC_REQUIRE([_DISTRO])dnl
    AC_MSG_NOTICE([+--------------------+])
    AC_MSG_NOTICE([| Repository Sources |])
    AC_MSG_NOTICE([+--------------------+])
    _REPO_OPTIONS
    _REPO_SETUP
    _REPO_OUTPUT
])# _REPO
# =============================================================================

# =============================================================================
# _REPO_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_OPTIONS], [dnl
    AC_ARG_WITH([install-source-yum],
	[AS_HELP_STRING([--with-install-source-yum=@<:@REPODIR@:>@],
	    [YUM repo directory @<:@default=search@:>@])],
	[], [with_install_source_yum=search])
    AC_ARG_WITH([install-source-zypp],
	[AS_HELP_STRING([--with-install-source-zypp=@<:@REPODIR@:>@],
	    [ZYPP repo directory @<:@default=search@:>@])],
	[], [with_install_source_zypp=search])
    AC_ARG_WITH([install-source-urpmi],
	[AS_HELP_STRING([--with-install-source-urpmi=@<:@MEDIACFGDIR@:>@],
	    [URPMI repo directory @<:@default=search@:>@])],
	[], [with_install_source_urpmi=search])
    AC_ARG_WITH([install-source-apt],
	[AS_HELP_STRING([--with-install-source-apt=@<:@SOURCESDIR@:>@],
	    [APT sources directory @<:@default=search@:>@])],
	[], [with_install_source_apt=search])
    AC_ARG_WITH([repo-root],
	[AS_HELP_STRING([--with-repo-root=@<:@REPOROOT@:>@],
	    [repository root path @<:@default=repo@:>@])],
	[], [with_repo_root='repo'])
    AC_ARG_WITH([repo-base],
	[AS_HELP_STRING([--with-repo-base=@<:@REPOBASE@:>@],
	    [repository base path @<:@default=www.openss7.org@:>@])],
	[], [with_repo_base='www.openss7.org'])
    AC_ARG_WITH([repo-branch],
	[AS_HELP_STRING([--with-repo-branch=@<:@REPOBRANCH@:>@],
	    [repository branch path @<:@default=main@:>@])],
	[], [with_repo_branch=])
    AC_ARG_WITH([repo-service],
	[AS_HELP_STRING([--with-repo-service=@<:@REPOSERVICE@:>@])],
	    [repository service @<:@default=https@:>@],
	[], [with_repo_service='https'])
])# _REPO_OPTIONS
# =============================================================================

# =============================================================================
# _REPO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP], [dnl
    _REPO_SETUP_URL
    _REPO_SETUP_RPM
    _REPO_SETUP_YUM
    _REPO_SETUP_ZYPP
    _REPO_SETUP_URPMI
    _REPO_SETUP_APT
])# _REPO_SETUP
# =============================================================================

# =============================================================================
# _REPO_SETUP_URL
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_URL], [dnl
    AC_MSG_CHECKING([for repo root])
    repo_cv_reporoot="${with_repo_root:-repo}"
    AC_MSG_RESULT(["$repo_cv_reporoot"])
    AC_MSG_CHECKING([for repo base])
    repo_cv_repobase="${with_repo_base:-www.openss7.org}"
    AC_MSG_RESULT(["$repo_cv_repobase"])
    AC_MSG_CHECKING([for repo branch])
    repo_cv_repobranch="${with_repo_branch}"
    AC_MSG_RESULT(["$repo_cv_repobranch"])
    AC_MSG_CHECKING([for repo service])
    repo_cv_reposervice="${with_repo_service:-https}"
    AC_MSG_RESULT(["$repo_cv_reposervice"])
    AC_MSG_CHECKING([for repo subdirectory])
    repo_cv_dist_subdir="${host_distro}/${host_edition}/${host_cpu}"
    AC_MSG_RESULT(["$repo_cv_dist_subdir"])
])# _REPO_SETUP_URL
# =============================================================================

# =============================================================================
# _REPO_SETUP_RPM
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_RPM], [dnl
    AC_CACHE_CHECK([for rpm gpg directory], [repo_cv_rpm_gpgdir], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/pki/rpm-gpg
	    ${DESTDIR}${rootdir}/etc/pki/rpm-gpg
	    ${DESTDIR}${sysconfdir}/pki/rpm-gpg
	    ${DESTDIR}/etc/pki/rpm-gpg\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for rpm gpg directory... $repo_dir])
	    if test -d "$repo_dir" ; then
		repo_cv_rpm_gpgdir="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_rpm_gpgdir" || repo_cv_rpm_gpgdir=no
	AC_MSG_CHECKING([for rpm gpg directory])
    ])
])# _REPO_SETUP_RPM
# =============================================================================

# =============================================================================
# _REPO_SETUP_YUM
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_YUM], [dnl
    AC_CACHE_CHECK([for yum repo directory], [repo_cv_yum_repodir], [dnl
	case "${with_install_source_yum:-search}" in
	    (no) repo_cv_yum_repodir=no ;;
	    (yes|search) ;;
	    (*) if test -d "$with_install_source_yum" ; then
		    repo_cv_yum_repodir="$with_install_source_yum"
		fi ;;
	esac
	if test -z "$repo_cv_yum_repodir" ; then
	    eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/yum.repos.d
		${DESTDIR}${rootdir}/etc/yum.repos.d
		${DESTDIR}${sysconfdir}/yum.repos.d
		${DESTDIR}/etc/yum.repos.d\""
	    repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for repo_dir in $repo_search_path ; do
		AC_MSG_CHECKING([for yum repo directory... $repo_dir])
		if test -d "$repo_dir" ; then
		    repo_cv_yum_repodir="$repo_dir"
		    AC_MSG_RESULT([yes])
		    break
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	    test -n "$repo_cv_yum_repodir" || repo_cv_yum_repodir=no
	    AC_MSG_CHECKING([for yum repo directory])
	fi
    ])
    AC_CACHE_CHECK([for yum kmod config file], [repo_cv_yum_kmodconf], [dnl
	eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/yum/pluginconf.d/kmod.conf
		${DESTDIR}${rootdir}${sysconfdir}/yum/pluginconf.d/fedorakmod.conf
		${DESTDIR}${rootdir}/etc/yum/pluginconf.d/kmod.conf
		${DESTDIR}${rootdir}/etc/yum/pluginconf.d/fedorakmod.conf
		${DESTDIR}${sysconfdir}/yum/pluginconf.d/kmod.conf
		${DESTDIR}${sysconfdir}/yum/pluginconf.d/fedorakmod.conf
		${DESTDIR}/etc/yum/pluginconf.d/kmod.conf
		${DESTDIR}/etc/yum/pluginconf.d/fedorakmod.conf\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for yum kmod config file... $repo_dir])
	    if test -f "$repo_dir" ; then
		repo_cv_yum_kmodconf="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_yum_kmodconf" || repo_cv_yum_kmodconf=no
	AC_MSG_CHECKING([for yum kmod config file])
    ])
])# _REPO_SETUP_YUM
# =============================================================================

# =============================================================================
# _REPO_SETUP_ZYPP
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_ZYPP], [dnl
    AC_CACHE_CHECK([for zypp cred directory], [repo_cv_zypp_creddir], [dnl
	case "${with_credentials_zypp:-search}" in
	    (no) repo_cv_zypp_creddir=no ;;
	    (yes|search) ;;
	    (*) if test -d "$with_credentials_zypp" ; then
		repo_cv_zypp_creddir="$with_credentials_zypp"
		fi ;;
	esac
	if test -z "$repo_cv_zypp_creddir" ; then
	    eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/zypp/credentials.d
		${DESTDIR}${rootdir}/etc/zypp/credentials.d
		${DESTDIR}${sysconfdir}/zypp/credentials.d
		${DESTDIR}/etc/zypp/credentials.d\""
	    repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for repo_dir in $repo_search_path ; do
		AC_MSG_CHECKING([for zypp cred directory... $repo_dir])
		if test -d "$repo_dir" ; then
		    repo_cv_zypp_creddir="$repo_dir"
		    AC_MSG_RESULT([yes])
		    break
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	    test -n "$repo_cv_zypp_creddir" || repo_cv_zypp_creddir=no
	    AC_MSG_CHECKING([for zypp cred directory])
	fi
    ])
    AC_CACHE_CHECK([for zypp serv directory], [repo_cv_zypp_servdir], [dnl
	case "${with_services_zypp:-search}" in
	    (no) repo_cv_zypp_servdir=no ;;
	    (yes|search) ;;
	    (*) if test -d "$with_services_zypp" ; then
		repo_cv_zypp_servdir="$with_services_zypp"
		fi ;;
	esac
	if test -z "$repo_cv_zypp_servdir" ; then
	    eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/zypp/services.d
		${DESTDIR}${rootdir}/etc/zypp/services.d
		${DESTDIR}${sysconfdir}/zypp/services.d
		${DESTDIR}/etc/zypp/services.d\""
	    repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for repo_dir in $repo_search_path ; do
		AC_MSG_CHECKING([for zypp serv directory... $repo_dir])
		if test -d "$repo_dir" ; then
		    repo_cv_zypp_servdir="$repo_dir"
		    AC_MSG_RESULT([yes])
		    break
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	    test -n "$repo_cv_zypp_servdir" || repo_cv_zypp_servdir=no
	    AC_MSG_CHECKING([for zypp serv directory])
	fi
    ])
    AC_CACHE_CHECK([for zypp repo directory], [repo_cv_zypp_repodir], [dnl
	case "${with_install_source_zypp:-search}" in
	    (no) repo_cv_zypp_repodir=no ;;
	    (yes|search) ;;
	    (*) if test -d "$with_install_source_zypp" ; then
		    repo_cv_zypp_repodir="$with_install_source_zypp"
		fi ;;
	esac
	if test -z "$repo_cv_zypp_repodir" ; then
	    eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/zypp/repos.d
		${DESTDIR}${rootdir}/etc/zypp/repos.d
		${DESTDIR}${sysconfdir}/zypp/repos.d
		${DESTDIR}/etc/zypp/repos.d\""
	    repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for repo_dir in $repo_search_path ; do
		AC_MSG_CHECKING([for zypp repo directory... $repo_dir])
		if test -d "$repo_dir" ; then
		    repo_cv_zypp_repodir="$repo_dir"
		    AC_MSG_RESULT([yes])
		    break
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	    test -n "$repo_cv_zypp_repodir" || repo_cv_zypp_repodir=no
	    AC_MSG_CHECKING([for zypp repo directory])
	fi
    ])
    AC_CACHE_CHECK([for zypp config file], [repo_cv_zypp_config], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/zypp/zypp.conf
	    ${DESTDIR}${rootdir}/etc/zypp/zypp.conf
	    ${DESTDIR}${sysconfdir}/zypp/zypp.conf
	    ${DESTDIR}/etc/zypp/zypp.conf\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for zypp config file... $repo_dir])
	    if test -f "$repo_dir" ; then
		repo_cv_zypp_config="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_zypp_config" || repo_cv_zypp_config=no
	AC_MSG_CHECKING([for zypp config file])
    ])
])# _REPO_SETUP_ZYPP
# =============================================================================

# =============================================================================
# _REPO_SETUP_URPMI
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_URPMI], [dnl
    AC_CACHE_CHECK([for urpmi media directory], [repo_cv_urpmi_repodir], [dnl
	case "${with_install_source_urpmi:-search}" in
	    (no) repo_cv_urpmi_repodir=no ;;
	    (yes|search) ;;
	    (*) if test -d "$with_install_source_urpmi" ; then
		    repo_cv_urpmi_repodir="$with_install_source_urpmi"
		fi ;;
	esac
	if test -z "$repo_cv_urpmi_repodir" ; then
	    eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/urpmi/mediacfg.d
		${DESTDIR}${rootdir}/etc/urpmi/mediacfg.d
		${DESTDIR}${sysconfdir}/urpmi/mediacfg.d
		${DESTDIR}/etc/urpmi/mediacfg.d\""
	    repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for repo_dir in $repo_search_path ; do
		AC_MSG_CHECKING([for urpmi media directory... $repo_dir])
		if test -d "$repo_dir" ; then
		    repo_cv_urpmi_repodir="$repo_dir"
		    AC_MSG_RESULT([yes])
		    break
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	    test -n "$repo_cv_urpmi_repodir" || repo_cv_urpmi_repodir=no
	    AC_MSG_CHECKING([for urpmi media directory])
	fi
    ])
    AC_CACHE_CHECK([for urpmi config directory], [repo_cv_urpmi_confdir], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/urpmi
	    ${DESTDIR}${rootdir}$/etc/urpmi
	    ${DESTDIR}${sysconfdir}/urpmi
	    ${DESTDIR}/etc/urpmi\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for urpmi config directory... $repo_dir])
	    if test -d "$repo_dir" ; then
		repo_cv_urpmi_confdir="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_urpmi_confdir" || repo_cv_urpmi_confdir=no
	AC_MSG_CHECKING([for urpmi config directory])
    ])
    AC_CACHE_CHECK([for urpmi config file], [repo_cv_uprmi_config], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/urpmi/urpmi.cfg
	    ${DESTDIR}${rootdir}/etc/urmpi/urpmi.cfg
	    ${DESTDIR}${sysconfdir}/urmpi/urpmi.cfg
	    ${DESTDIR}/etc/urmpi/urpmi.cfg\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for urpmi config file... $repo_dir])
	    if test -f "$repo_dir" ; then
		repo_cv_urpmi_config="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_urpmi_config" || repo_cv_urpmi_config=no
	AC_MSG_CHECKING([for urpmi config file])
    ])
])# _REPO_SETUP_URPMI
# =============================================================================

# =============================================================================
# _REPO_SETUP_APT
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_APT], [dnl
    AC_CACHE_CHECK([for apt directory], [repo_cv_apt_dir], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/apt
	    ${DESTDIR}${rootdir}/etc/apt
	    ${DESTDIR}${sysconfdir}/apt
	    ${DESTDIR}/etc/apt\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for apt directory... $repo_dir])
	    if test -d "$repo_dir" ; then
		repo_cv_apt_dir="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_apt_dir" || repo_cv_apt_dir=no
	AC_MSG_CHECKING([for apt directory])
    ])
    AC_CACHE_CHECK([for apt sources directory], [repo_cv_apt_repodir], [dnl
	case "${with_install_source_apt:-search}" in
	    (no) repo_cv_apt_repodir=no ;;
	    (yes|search) ;;
	    (*) if test -d "$with_install_source_apt" ; then
		    repo_cv_apt_repodir="$with_install_source_apt"
		fi ;;
	esac
	if test -z "$repo_cv_apt_repodir" ; then
	    eval "repo_search_path=\"
		${DESTDIR}${rootdir}${sysconfdir}/apt/sources.list.d
		${DESTDIR}${rootdir}/etc/apt/sources.list.d
		${DESTDIR}${sysconfdir}/apt/sources.list.d
		${DESTDIR}/etc/apt/sources.list.d\""
	    repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for repo_dir in $repo_search_path ; do
		AC_MSG_CHECKING([for apt sources directory... $repo_dir])
		if test -d "$repo_dir" ; then
		    repo_cv_apt_repodir="$repo_dir"
		    AC_MSG_RESULT([yes])
		    break
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	    test -n "$repo_cv_apt_repodir" || repo_cv_apt_repodir=no
	    AC_MSG_CHECKING([for apt sources directory])
	fi
    ])
dnl
dnl The Connectiva apt-rpm distribution does not come equipped with a source directory, just a
dnl sources list.
dnl
    AC_CACHE_CHECK([for apt sources list], [repo_cv_apt_srclist], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/apt
	    ${DESTDIR}${rootdir}/etc/apt
	    ${DESTDIR}${sysconfdir}/apt
	    ${DESTDIR}/etc/apt\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for apt sources list... $repo_dir/sources.list])
	    if test -f "$repo_dir/sources.list" ; then
		repo_cv_apt_srclist="$repo_dir/sources.list"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_apt_srclist" || repo_cv_apt_srclist=no
	AC_MSG_CHECKING([for apt sources list])
    ])
dnl 
dnl The Connectiva apt-rpm distribution diverged from Debian apt on the means for handling GPG keys:
dnl it uses /etc/apt/vendors.list to identify the keys.
dnl 
    AC_CACHE_CHECK([for apt vendors list], [repo_cv_apt_vndlist], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/apt
	    ${DESTDIR}${rootdir}/etc/apt
	    ${DESTDIR}${sysconfdir}/apt
	    ${DESTDIR}/etc/apt\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for apt vendors list... $repo_dir/vendors.list])
	    if test -f "$repo_dir/vendors.list" ; then
		repo_cv_apt_vndlist="$repo_dir/vendors.list"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_apt_vndlist" || repo_cv_apt_vndlist=no
	AC_MSG_CHECKING([for apt vendors list])
    ])
    AC_CACHE_CHECK([for apt gpg directory], [repo_cv_apt_gpgdir], [dnl
	eval "repo_search_path=\"
	    ${DESTDIR}${rootdir}${sysconfdir}/apt/trusted.gpg.d
	    ${DESTDIR}${rootdir}/etc/apt/trusted.gpg.d
	    ${DESTDIR}${sysconfdir}/apt/trusted.gpg.d
	    ${DESTDIR}/etc/apt/trusted.gpg.d\""
	repo_search_path=`echo "$repo_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for repo_dir in $repo_search_path ; do
	    AC_MSG_CHECKING([for apt gpg directory... $repo_dir])
	    if test -d "$repo_dir" ; then
		repo_cv_apt_gpgdir="$repo_dir"
		AC_MSG_RESULT([yes])
		break
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$repo_cv_apt_gpgdir" || repo_cv_apt_gpgdir=no
	AC_MSG_CHECKING([for apt gpg directory])
    ])
])# _REPO_SETUP_APT
# =============================================================================

# =============================================================================
# _REPO_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_OUTPUT], [dnl
    reporoot='repo'
    if test :"${repo_cv_reporoot:-no}" != :no ; then
	reporoot="$repo_cv_reporoot"
    fi
    AC_SUBST([reporoot])dnl
    repobase='www.openss7.org'
    if test :"${repo_cv_repobase:-no}" != :no ; then
	repobase="$repo_cv_repobase"
    fi
    AC_SUBST([repobase])dnl
    repobranch=
    if test :"${repo_cv_branch:-no}" != :no ; then
	repobranch="/$repo_cv_repobranch"
    fi
    repobranch=`echo "$repobranch" | sed -e 's,^//*,/,'`
    AC_SUBST([repobranch])dnl
    reposerv='https'
    if test :"${repo_cv_reposervice:-no}" != :no ; then
	reposerv="$repo_cv_reposervice"
    fi
    AC_SUBST([reposerv])dnl
    reposubdir=
    if test :"${repo_cv_dist_subdir:-no}" != :no ; then
	reposubdir="$repo_cv_dist_subdir"
    fi
    AC_SUBST([reposubdir])dnl
    yumrepodir=
    if test :"${repo_cv_yum_repodir:-no}" != :no ; then
	yumrepodir="$repo_cv_yum_repodir"
    fi
    AC_SUBST([yumrepodir])dnl
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_YUM], [test :"${repo_cv_yum_repodir:-no}" != :no])
    yumkmodconf=
    if test "${repo_cv_yum_kmodconf:-no}" != :no ; then
	yumkmodconf="$repo_cv_yum_kmodconf"
    fi
    AC_SUBST([yumkmodconf])dnl
    rpmgpgdir=
    if test :"${repo_cv_rpm_gpgdir:-no}" != :no ; then
	rpmgpgdir="$repo_cv_rpm_gpgdir"
    fi
    AC_SUBST([rpmgpgdir])dnl
    zyppcreddir=
    if test :"${repo_cv_zypp_creddir:-no}" != :no ; then
	zyppcreddir="$repo_cv_zypp_creddir"
    fi
    AC_SUBST([zyppcreddir])dnl
    zyppservdir=
    if test :"${repo_cv_zypp_servdir:-no}" != :no ; then
	zyppservdir="$repo_cv_zypp_servdir"
    fi
    AC_SUBST([zyppservdir])dnl
    zypprepodir=
    if test :"${repo_cv_zypp_repodir:-no}" != :no ; then
	zypprepodir="$repo_cv_zypp_repodir"
    fi
    AC_SUBST([zypprepodir])dnl
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_ZYPP], [test :"${repo_cv_zypp_repodir:-no}" != :no])
    zyppconfig=
    if test :"${repo_cv_zypp_config:-no}" != :no ; then
	zyppconfig="$repo_cv_zypp_config"
    fi
    AC_SUBST([zyppconfig])dnl
    urpmirepodir=
    urpmimediadir=
    if test :"${repo_cv_urpmi_repodir:-no}" != :no ; then
	urpmirepodir="$repo_cv_urpmi_repodir"
	urpmimediadir="$repo_cv_urpmi_repodir/OpenSS7-${target_edition}-${target_cpu}"
    fi
    AC_SUBST([urpmirepodir])dnl
    AC_SUBST([urpmimediadir])dnl
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_URPMI], [test :"${repo_cv_urpmi_repodir:-no}" != :no])
    urpmiconfig=
    if test :"${repo_cv_urpmi_config:-no}" != :no ; then
	urpmiconfig="$repo_cv_urpmi_config"
    fi
    AC_SUBST([urpmiconfig])
    urpmiconfdir=
    if test :$"{repo_cv_urpmi_confdir:-no}" != :no ; then
	urpmiconfdir="$repo_cv_urpmi_confdir"
    fi
    AC_SUBST([urpmiconfdir])dnl
    aptconfdir=
    if test :"${repo_cv_apt_dir:-no}" != :no ; then
	aptconfdir="$repo_cv_apt_dir"
    fi
    AC_SUBST([aptconfdir])
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_APT], [test :"${repo_cv_apt_dir:-no}" != :no])
    aptrepodir=
    if test :"${repo_cv_apt_repodir:-no}" != :no ; then
	aptrepodir="$repo_cv_apt_repodir"
    fi
    AC_SUBST([aptrepodir])dnl
    aptsrclist=
    if test :"${repo_cv_apt_srclist:-no}" != :no ; then
	aptsrclist="$repo_cv_apt_srclist"
    fi
    AC_SUBST([aptsrclist])dnl
    aptvndlist=
    if test :"${repo_cv_apt_vndlist:-no}" != :no ; then
	aptvndlist="$repo_cv_apt_vndlist"
    fi
    AC_SUBST([aptvndlist])dnl
    aptgpgdir=
    if test :"${repo_cv_apt_gpgdir:-no}" != :no ; then
	aptgpgdir="$repo_cv_apt_gpgdir"
    fi
    AC_SUBST([aptgpgdir])dnl
])# _REPO_OUTPUT
# =============================================================================

# =============================================================================
# _REPO_
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_], [dnl
])# _REPO_
# =============================================================================

# =============================================================================
#
# $Log: repo.m4,v $
# Revision 1.1.2.5  2011-08-07 11:14:37  brian
# - mostly mandriva and ubuntu build updates
#
# Revision 1.1.2.4  2011-07-27 07:52:19  brian
# - work to support Mageia/Mandriva compressed kernel modules and URPMI repo
#
# Revision 1.1.2.3  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.2  2011-02-28 19:51:30  brian
# - better repository build
#
# Revision 1.1.2.1  2011-02-17 18:34:10  brian
# - repository and rpm build updates
#
# =============================================================================
# 
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
