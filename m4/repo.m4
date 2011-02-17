# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: repo.m4,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2011-02-17 18:34:10 $
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
# Last Modified $Date: 2011-02-17 18:34:10 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _REPO
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO], [dnl
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
	    [ZYPP repo directory @<:@default=search@;>@])],
	[], [with_install_source_zypp=search])
    AC_ARG_WITH([install-source-apt],
	[AS_HELP_STRING([--with-install-source-apt=@<:@SOURCESDIR@:>@],
	    [APT sources directory @<:@default=search@:>@])],
	[], [with_install_source_apt=search])
])# _REPO_OPTIONS
# =============================================================================

# =============================================================================
# _REPO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP], [dnl
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
    AC_CACHE_CHECK([for zypp config file], [rpm_cv_zypp_config], [dnl
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
])# _REPO_SETUP
# =============================================================================

# =============================================================================
# _REPO_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_OUTPUT], [dnl
    yumrepodir=
    if test :"${repo_cv_yum_repodir:-no}" != :no ; then
	yumrepodir="$repo_cv_yum_repodir"
    fi
    AC_SUBST([yumrepodir])dnl
    AM_CONDITIONAL([WITH_YUM_SOURCE], [test :"${repo_cv_yum_repodir:-no}" != :no])
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
    zypprepodir=
    if test :"${repo_cv_zypp_repodir:-no}" != :no ; then
	zypprepodir="$repo_cv_zypp_repodir"
    fi
    AC_SUBST([zypprepodir])dnl
    AM_CONDITIONAL([WITH_ZYPP_SOURCE], [test :"${repo_cv_zypp_repodir:-no}" != :no])
    zyppconfig=
    if test :"${repo_cv_zypp_config:-no}" != :no ; then
	zyppconfig="$repo_cv_zypp_config"
    fi
    AC_SUBST([zyppconfig])dnl
    aptrepodir=
    if test :"${repo_cv_apt_repodir:-no}" != :no ; then
	aptrepodir="$repo_cv_apt_repodir"
    fi
    AC_SUBST([aptrepodir])dnl
    AM_CONDITIONAL([WITH_APT_SOURCE], [test :"${repo_cv_apt_repodir:-no}" != :no])
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
