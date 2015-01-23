# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) File: m4/repo.m4
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_YUM], [test :"${repo_cv_yum_repodir:-no}" != :no])
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_ZYPP], [test :"${repo_cv_zypp_repodir:-no}" != :no])
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_URPMI], [test :"${repo_cv_urpmi_repodir:-no}" != :no])
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_APT], [test :"${repo_cv_apt_dir:-no}" != :no])
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_SLAPT], [test :"${repo_cv_slapt_repodir}" != :no])
    AM_CONDITIONAL([WITH_INSTALL_SOURCE_PACMAN], [test :"${repo_cv_pacman_repodir}" != :no])
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
    AC_ARG_WITH([install-source-pacman],
	[AS_HELP_STRING([--with-install-source-pacman=@<:@PACMANDIR@:>@],
	    [PACMAN sources directory @<:@default=search@:>@])],
	[], [with_install_source_pacman=search])
    AC_ARG_WITH([install-source-slapt],
	[AS_HELP_STRING([--with-install-source-slapt=@<:@SLAPTDIR@:>@],
	    [SLAPT sources directory @<:@default=search@:>@])],
	[], [with_install_source_slapt=search])
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
    _REPO_SETUP_PACMAN
    _REPO_SETUP_SLAPT
    AC_CONFIG_FILES([scripts/register])
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
    repo_cv_dist_subdir="${host_distro}/${host_edition}/${host_arch}"
    AC_MSG_RESULT(["$repo_cv_dist_subdir"])
])# _REPO_SETUP_URL
# =============================================================================

# =============================================================================
# _REPO_SETUP_RPM
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_RPM], [dnl
    _BLD_FIND_DIR([for rpm gpg directory], [repo_cv_rpm_gpgdir], [
	    ${sysconfdir}/pki/rpm-gpg
	    ${rootdir}/etc/pki/rpm-gpg], [], [no])
])# _REPO_SETUP_RPM
# =============================================================================

# =============================================================================
# _REPO_SETUP_YUM
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_YUM], [dnl
    _BLD_FIND_DIR([for yum repo directory], [repo_cv_yum_repodir], [
	    ${sysconfdir}/yum.repos.d
	    ${sysconfdir}/yum/repos.d
	    ${rootdir}/etc/yum.repos.d
	    ${rootdir}/etc/yum/repos.d], [], [no], [], [], [with_install_source_yum])
    _BLD_FIND_FILE([for yum kmod config file], [repo_cv_yum_kmodconf], [
	    ${sysconfdir}/yum/pluginconf.d/kmod.conf
	    ${sysconfdir}/yum/pluginconf.d/fedorakmod.conf
	    ${rootdir}/etc/yum/pluginconf.d/kmod.conf
	    ${rootdir}/etc/yum/pluginconf.d/fedorakmod.conf], [no])
])# _REPO_SETUP_YUM
# =============================================================================

# =============================================================================
# _REPO_SETUP_ZYPP
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_ZYPP], [dnl
    _BLD_FIND_DIR([for zypp cred directory], [repo_cv_zypp_creddir], [
	    ${sysconfdir}/zypp/credentials.d
	    ${rootdir}/etc/zypp/credentials.d], [], [no], [], [], [with_credentials_zypp])
    _BLD_FIND_DIR([for zypp serv directory], [repo_cv_zypp_servdir], [
	    ${sysconfdir}/zypp/services.d
	    ${rootdir}/etc/zypp/services.d], [], [no], [], [], [with_services_zypp])
    _BLD_FIND_DIR([for zypp repo directory], [repo_cv_zypp_repodir], [
	    ${sysconfdir}/zypp/repos.d
	    ${rootdir}/etc/zypp/repos.d], [], [no], [], [], [with_install_source_zypp])
    _BLD_FIND_FILE([for zypp config file], [repo_cv_zypp_config], [
	    ${sysconfdir}/zypp/zypp.conf
	    ${rootdir}/etc/zypp/zypp.conf], [no])
])# _REPO_SETUP_ZYPP
# =============================================================================

# =============================================================================
# _REPO_SETUP_URPMI
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_URPMI], [dnl
    _BLD_FIND_DIR([for urpmi media directory], [repo_cv_urpmi_repodir], [
	    ${sysconfdir}/urpmi/mediacfg.d
	    ${rootdir}/etc/urpmi/mediacfg.d], [], [no], [], [], [with_install_source_uprmi])
    _BLD_FIND_DIR([for urpmi config directory], [repo_cv_urpmi_confdir], [
	    ${sysconfdir}/urpmi
	    ${rootdir}/etc/urpmi], [], [no])
    _BLD_FIND_FILE([for urpmi config file], [repo_cv_uprmi_config], [
	    ${sysconfdir}/urpmi/urpmi.cfg
	    ${rootdir}/etc/urpmi/urpmi.cfg], [no])
    ])
])# _REPO_SETUP_URPMI
# =============================================================================

# =============================================================================
# _REPO_SETUP_APT
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_APT], [dnl
    _BLD_FIND_DIR([for apt directory], [repo_cv_apt_dir], [
	    ${sysconfdir}/apt
	    ${rootdir}/etc/apt], [], [no])
    _BLD_FIND_DIR([for apt sources directory], [repo_cv_apt_repodir], [
	    ${sysconfdir}/apt/sources.list.d
	    ${rootdir}/etc/apt/sources.list.d], [], [no], [], [], [with_install_source_apt])
dnl
dnl The Connectiva apt-rpm distribution does not come equipped with a source directory, just a
dnl sources list.
dnl
    _BLD_FIND_FILE([for apt sources list], [repo_cv_apt_srclist], [
	    ${sysconfdir}/apt/sources.list
	    ${rootdir}/etc/apt/sources.list], [no])
    _BLD_FIND_DIR([for apt vendors directory], [repo_cv_apt_venddir], [
	    ${sysconfdir}/apt/vendors.list.d
	    ${rootdir}/etc/apt/vendors.list.d], [], [no])
dnl 
dnl The Connectiva apt-rpm distribution diverged from Debian apt on the means for handling GPG keys:
dnl it uses /etc/apt/vendors.list to identify the keys.
dnl 
    _BLD_FIND_FILE([for apt vendors list], [repo_cv_apt_vndlist], [
	    ${sysconfdir}/apt/vendors.list
	    ${rootdir}/etc/apt/vendors.list], [no])
    _BLD_FIND_DIR([for apt gpg directory], [repo_cv_apt_gpgdir], [
	    ${sysconfdir}/apt/trusted.gpg.d
	    ${rootdir}/etc/apt/trusted.gpg.d], [], [no])
])# _REPO_SETUP_APT
# =============================================================================

# =============================================================================
# _REPO_SETUP_PACMAN
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_PACMAN], [dnl
    _BLD_FIND_DIR([for pacman mirrors directory], [repo_cv_pacman_repodir], [
	    ${sysconfdir}/pacman.d
	    ${rootdir}/etc/pacman.d], [], [no], [], [], [with_install_source_pacman])
    _BLD_FIND_DIR([for pacman keyrings directory], [repo_cv_pacman_keyrings], [
	    ${datarootdir}/pacman/keyrings
	    ${rootdir}/usr/share/pacman/keyrings], [], [no])
    _BLD_FIND_FILE([for pacman config file], [repo_cv_pacman_config], [
	    ${sysconfdir}/pacman.conf
	    ${rootdir}/etc/pacman.conf], [no])
])# _REPO_SETUP_PACMAN
# =============================================================================

# =============================================================================
# _REPO_SETUP_SLAPT
# -----------------------------------------------------------------------------
AC_DEFUN([_REPO_SETUP_SLAPT], [dnl
    _BLD_FIND_DIR([for slapt directory], [repo_cv_slapt_repodir], [
	    ${sysconfdir}/slapt-get
	    ${rootdir}/etc/slapt-get], [], [no], [], [], [with_install_source_slapt])
    _BLD_FIND_FILE([for slapt-get rc file], [repo_cv_slapt_getrc], [
	    ${sysconfdir}/slapt-get/slapt-getrc
	    ${rootdir}/etc/slapt-get/slapt-getrc], [no])
    _BLD_FIND_FILE([for slapt-src rc file], [repo_cv_slapt_srcrc], [dnl
	    ${sysconfdir}/slapt-get/slapt-srcrc
	    ${rootdir}/etc/slapt-get/slapt-srcrc], [no])
])# _REPO_SETUP_SLAPT
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
    yumrepodir='${rootdir}/etc/yum.repos.d'
    if test :"${repo_cv_yum_repodir:-no}" != :no ; then
	yumrepodir="$repo_cv_yum_repodir"
    fi
    AC_SUBST([yumrepodir])dnl
    yumkmodconf='${rootdir}/etc/yum/pluginconf.d/kmod.conf'
    if test :"${repo_cv_yum_kmodconf:-no}" != :no ; then
	yumkmodconf="$repo_cv_yum_kmodconf"
    fi
    AC_SUBST([yumkmodconf])dnl
    rpmgpgdir='${rootdir}/etc/pki/rpm-gpg'
    if test :"${repo_cv_rpm_gpgdir:-no}" != :no ; then
	rpmgpgdir="$repo_cv_rpm_gpgdir"
    fi
    AC_SUBST([rpmgpgdir])dnl
    zyppcreddir='${rootdir}/etc/zypp/credentials.d'
    if test :"${repo_cv_zypp_creddir:-no}" != :no ; then
	zyppcreddir="$repo_cv_zypp_creddir"
    fi
    AC_SUBST([zyppcreddir])dnl
    zyppservdir='${rootdir}/etc/zypp/services.d'
    if test :"${repo_cv_zypp_servdir:-no}" != :no ; then
	zyppservdir="$repo_cv_zypp_servdir"
    fi
    AC_SUBST([zyppservdir])dnl
    zypprepodir='${rootdir}/etc/zypp/repos.d'
    if test :"${repo_cv_zypp_repodir:-no}" != :no ; then
	zypprepodir="$repo_cv_zypp_repodir"
    fi
    AC_SUBST([zypprepodir])dnl
    zyppconfig='${rootdir}/etc/zypp/zypp.conf'
    if test :"${repo_cv_zypp_config:-no}" != :no ; then
	zyppconfig="$repo_cv_zypp_config"
    fi
    AC_SUBST([zyppconfig])dnl
    urpmirepodir='${rootdir}/etc/urpmi/mediacfg.d'
    urpmimediadir='${urpmirepodir}/${PACKAGE_NAME}-${target_edition}-${target_arch}'
    if test :"${repo_cv_urpmi_repodir:-no}" != :no ; then
	urpmirepodir="$repo_cv_urpmi_repodir"
	urpmimediadir="$repo_cv_urpmi_repodir/${PACKAGE_NAME}-${target_edition}-${target_arch}"
    fi
    AC_SUBST([urpmirepodir])dnl
    AC_SUBST([urpmimediadir])dnl
    urpmiconfig='${rootdir}/etc/urpmi/urpmi.cfg'
    if test :"${repo_cv_urpmi_config:-no}" != :no ; then
	urpmiconfig="$repo_cv_urpmi_config"
    fi
    AC_SUBST([urpmiconfig])
    urpmiconfdir='${rootdir}/etc/urpmi'
    if test :"${repo_cv_urpmi_confdir:-no}" != :no ; then
	urpmiconfdir="$repo_cv_urpmi_confdir"
    fi
    AC_SUBST([urpmiconfdir])dnl
    aptconfdir='${rootdir}/etc/apt'
    if test :"${repo_cv_apt_dir:-no}" != :no ; then
	aptconfdir="$repo_cv_apt_dir"
    fi
    AC_SUBST([aptconfdir])
    aptrepodir='${rootdir}/etc/apt/sources.list.d'
    if test :"${repo_cv_apt_repodir:-no}" != :no ; then
	aptrepodir="$repo_cv_apt_repodir"
    elif test :"${repo_cv_apt_srclist:-no}" != :no ; then
	aptrepodir="$repo_cv_apt_srclist.d"
    elif test :"${repo_cv_apt_dir:-no}" != :no ; then
	aptrepodir="$repo_cv_apt_dir/sources.list.d"
    fi
    AC_SUBST([aptrepodir])dnl
    aptsrclist='${rootdir}/etc/apt/sources.list'
    if test :"${repo_cv_apt_srclist:-no}" != :no ; then
	aptsrclist="$repo_cv_apt_srclist"
    fi
    AC_SUBST([aptsrclist])dnl
    aptvenddir='${rootdir}/etc/apt/vendors.list.d'
    if test :"${repo_cv_apt_venddir:-no}" != :no  ; then
	aptvenddir="$repo_cv_apt_venddir"
    elif test :"${repo_cv_apt_vndlist:-no}" != :no ; then
	aptvenddir="$repo_cv_apt_vndlist.d"
    elif test :"${repo_cv_apt_dir:-no}" != :no ; then
	aptvenddir="$repo_cv_apt_dir/vendors.list.d"
    fi
    AC_SUBST([aptvenddir])
    aptvndlist='${rootdir}/etc/apt/vendors.list'
    if test :"${repo_cv_apt_vndlist:-no}" != :no ; then
	aptvndlist="$repo_cv_apt_vndlist"
    fi
    AC_SUBST([aptvndlist])dnl
    aptgpgdir='${rootdir}/etc/apt/trusted.gpg.d'
    if test :"${repo_cv_apt_gpgdir:-no}" != :no ; then
	aptgpgdir="$repo_cv_apt_gpgdir"
    fi
    AC_SUBST([aptgpgdir])dnl
    slaptrepodir='${rootdir}/etc/slapt-get'
    if test :"${repo_cv_slapt_repodir:-no}" != :no ; then
	slaptrepodir="$repo_cv_slapt_repodir"
    fi
    AC_SUBST([slaptrepodir])dnl
    slaptgetrc='${rootdir}/etc/slapt-get/slapt-getrc'
    if test :"${repo_cv_slapt_getrc:-no}" != :no ; then
	slaptgetrc="$repo_cv_slapt_getrc"
    fi
    AC_SUBST([slaptgetrc])dnl
    slaptsrcrc='${rootdir}/etc/slapt-get/slapt-srcrc'
    if test :"${repo_cv_slapt_srcrc:-no}" != :no ; then
	slaptsrcrc="$repo_cv_slapt_srcrc"
    fi
    AC_SUBST([slaptsrcrc])dnl
    pacmanrepodir='${rootdir}/etc/slapt-get'
    if test :"${repo_cv_pacman_repodir:-no}" != :no ; then
	pacmanrepodir="$repo_cv_pacman_repodir"
    fi
    pacmankeyrings='${rootdir}/usr/share/pacman/keyrings'
    if test :"${repo_cv_pacman_keyrings:-no}" != :no ; then
	pacmankeyrings="$repo_cv_pacman_keyrings";
    fi
    AC_SUBST([pacmanrepodir])dnl
    pacmanconfig='${rootdir}/etc/pacman.conf'
    if test :"${repo_cv_pacman_config:-no}" != :no ; then
	pacmanconfig="$repo_cv_pacman_config"
    fi
    AC_SUBST([pacmanconfig])dnl
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
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
