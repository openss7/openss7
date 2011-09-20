# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: deb.m4,v $ $Name:  $($Revision: 1.1.2.11 $) $Date: 2011-09-20 09:51:32 $
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
# Last Modified $Date: 2011-09-20 09:51:32 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _DEB_DPKG
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_TARDIR])
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_REQUIRE([_DISTRO])
    AC_REQUIRE([_REPO])
    AC_MSG_NOTICE([+-----------------------+])
    AC_MSG_NOTICE([| Debian Archive Checks |])
    AC_MSG_NOTICE([+-----------------------+])
    _DEB_DPKG_OPTIONS
    _DEB_DPKG_SETUP
    _DEB_REPO_SETUP
    _DEB_DPKG_OUTPUT
])# _DEB_DPKG
# =============================================================================

# =============================================================================
# _DEB_DPKG_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_OPTIONS], [dnl
    _DEB_OPTIONS_DEB_EPOCH
    _DEB_OPTIONS_DEB_RELEASE
])# _DEB_DPKG_OPTIONS
# =============================================================================

# =============================================================================
# _DEB_OPTIONS_DEB_EPOCH
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_OPTIONS_DEB_EPOCH], [dnl
    AC_MSG_CHECKING([for deb epoch])
    AC_ARG_WITH([deb-epoch],
	[AS_HELP_STRING([--with-deb-epoch=EPOCH],
	    [deb package EPOCH @<:@default=auto@:>@])],
	[], [dnl
	    if test -r .debepoch; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.debepoch
	    then with_deb_epoch="`cat ${d}.debepoch`"
	    else with_deb_epoch=0
	    fi])
    AC_MSG_RESULT([${with_deb_epoch:-0}])
    PACKAGE_DEBEPOCH="${with_deb_epoch:-0}"
    AC_SUBST([PACKAGE_DEBEPOCH])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_DEBEPOCH], [$PACKAGE_DEBEPOCH], [The Debian
			Epoch.  This defaults to 0.])
])# _DEB_OPTIONS_DEB_EPOCH
# =============================================================================

# =============================================================================
# _DEB_OPTIONS_DEB_RELEASE
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_OPTIONS_DEB_RELEASE], [dnl
    AC_MSG_CHECKING([for deb release])
    AC_ARG_WITH([deb-release],
	[AS_HELP_STRING([--with-deb-release=RELEASE],
	    [deb package RELEASE @<:@default=auto@:>@])],
	[], [dnl
	    if test -r .debrelease ; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.debrelease
	    then with_deb_release="`cat ${d}.debrelease`"
	    else with_deb_release=0
	    fi])
    AC_MSG_RESULT([${with_deb_release:-0}])
    PACKAGE_DEBRELEASE="${with_deb_release:-0}"
    AC_SUBST([PACKAGE_DEBRELEASE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_DEBRELEASE], ["$PACKAGE_DEBRELEASE"], [The
			Debian Release. This defaults to 0.])
])# _DEB_OPTIONS_DEB_RELEASE
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP], [dnl
dnl _DEB_DPKG_SETUP_DIST
dnl _DEB_DPKG_SETUP_ARCH
dnl _DEB_DPKG_SETUP_INDEP
    _DEB_DPKG_SETUP_TOPDIR
    _DEB_DPKG_SETUP_OPTIONS
    _DEB_DPKG_SETUP_BUILD
    _DEB_DPKG_SETUP_HELPER
])# _DEB_DPKG_SETUP
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_DIST
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_DIST], [dnl
])# _DEB_DPKG_SETUP_DIST
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_ARCH
# -----------------------------------------------------------------------------
# Debian can build architecture dependent or architecture independent packages.
# This option specifies whether architecture dependent packages are to be built
# and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_ARCH], [dnl
    AC_MSG_CHECKING([for deb build/install of arch packages])
    AC_ARG_ENABLE([arch],
	[AS_HELP_STRING([--disable-arch],
	    [arch components @<:@default=enabled@:>@])],
	[], [enable_arch=yes])
    AC_MSG_RESULT([${enable_arch:-yes}])
    AM_CONDITIONAL([DEB_BUILD_ARCH], [test ":${enable_arch:-yes}" = :yes])dnl
	
])# _DEB_DPKG_SETUP_ARCH
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_INDEP
# -----------------------------------------------------------------------------
# Debian can build architecture dependent or architecture independent packages.
# This option specifies whether architecture independent packages are to be
# built and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_INDEP], [dnl
    AC_MSG_CHECKING([for deb build/install of indep packages])
    AC_ARG_ENABLE([indep],
	[AS_HELP_STRING([--disable-indep],
	    [indep components @<:@default=enabled@:>@])],
	[], [enable_indep=yes])
    AC_MSG_RESULT([${enable_indep:-yes}])
    AM_CONDITIONAL([DEB_BUILD_INDEP], [test ":${enable_indep:-yes}" = :yes])dnl
])# _DEB_DPKG_SETUP_INDEP
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_TOPDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_TOPDIR], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_ARG_WITH([deb-distdir],
	[AS_HELP_STRING([--with-deb-distdir=DIR],
	    [deb dist directory @<:@default=PKG-DISTDIR/debs/PKG-SUBDIR@:>@])],
	[], [with_deb_distdir='$(DISTDIR)/debs/$(reposubdir)'])
    AC_MSG_CHECKING([for deb distribution directory])
    if test ":${debdistdir+set}" != :set ; then
	case ":${with_deb_distdir:-no}" in
	    (:no|:yes)	debdistdir='$(DISTDIR)/debs/$(reposubdir)' ;;
	    (*)		debdistdir="$with_deb_distdir" ;;
	esac
    fi
    AC_MSG_RESULT([$debdistdir])
    AC_SUBST([debdistdir])dnl
    AC_ARG_WITH([deb-topdir],
	[AS_HELP_STRING([--with-deb-topdir=DIR],
	    [deb top directory @<:@default=DEB-DISTDIR/BRANCH@:>@])],
	[], [with_deb_topdir='$(debdistdir)$(repobranch)'])
    AC_MSG_CHECKING([for deb top build directory])
    if test ":${debdir+set}" != :set ; then
	case ":${with_deb_topdir:-no}" in
	    (:no|:yes)	debdir='$(debdistdir)$(repobranch)' ;;
	    (*)		debdir="$with_deb_topdir" ;;
	esac
    fi
    AC_MSG_RESULT([$debdir])
    AC_SUBST([debdir])dnl
    aptdir='$(debdistdir)'
    AC_SUBST([aptdir])dnl
    aptfulldir='$(debdir)'
    AC_SUBST([aptfulldir])dnl
    aptmaindir='$(debdir)/main'
    AC_SUBST([aptmaindir])dnl
    aptdebgdir='$(debdir)/debug'
    AC_SUBST([aptdebgdir])dnl
    aptdevldir='$(debdir)/devel'
    AC_SUBST([aptdevldir])dnl
    aptsrcsdir='$(debdir)/source'
    AC_SUBST([aptsrcsdir])dnl
    AC_MSG_CHECKING([for deb BUILD directory])
    if test ":${debbuilddir+set}" != :set ; then
	# debbuilddir needs to be absolute: always build in the top build
	# directory on the local machine
	debbuilddir=`pwd`/debian
    fi
    AC_MSG_RESULT([debbuilddir])
    AC_SUBST([debbuilddir])dnl
])# _DEB_DPKG_SETUP_TOPDIR
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_OPTIONS], [dnl
    args="$ac_configure_args"
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-maintainer-mode(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-dependency-tracking(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-modules(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-tools(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-arch(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-indep(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--(en|dis)able-devel(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--with(out)?-lis(=[[^[:space:]]]*)?(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--with(out)?-lfs(=[[^[:space:]]]*)?(.)? , ,g;s,^ *,,;s, *$,,'`
    args=`echo " $args " | sed -r -e 's, (.)?--with(out)?-k-release(=[[^[:space:]]]*)?(.)? , ,g;s,^ *,,;s, *$,,'`
    for arg_part in $args ; do
	if (echo "$arg_part" | grep "^'" >/dev/null 2>&1) ; then
	    if test -n "$arg" ; then
		AC_MSG_CHECKING([for deb argument $arg])
		if (echo $arg | egrep '^'"'"'(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
		    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }$arg"
		    AC_MSG_RESULT([yes])
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
	AC_MSG_CHECKING([for deb argument $arg])
	if (echo $arg | egrep '^'"'"'(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }$arg"
	    AC_MSG_RESULT([yes])
	else
	    :
	    AC_MSG_RESULT([no])
	fi
    fi
    AC_SUBST([PACKAGE_DEBOPTIONS])dnl
])# _DEB_DPKG_SETUP_OPTIONS
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_BUILD
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_BUILD], [dnl
dnl
dnl These environment variables 
dnl
dnl AC_ARG_VAR([DEB_BUILD_ARCH], [Debian build architecture])
dnl AC_ARG_VAR([DEB_BUILD_GNU_CPU], [Debian build cpu])
dnl AC_ARG_VAR([DEB_BUILD_GNU_SYSTEM], [Debian build os])
dnl AC_ARG_VAR([DEB_BUILD_GNU_TYPE], [Debian build alias])
dnl AC_ARG_VAR([DEB_HOST_ARCH], [Debian host/target architecture])
dnl AC_ARG_VAR([DEB_HOST_GNU_CPU], [Debian host/target cpu])
dnl AC_ARG_VAR([DEB_HOST_GNU_SYSTEM], [Debian host/target os])
dnl AC_ARG_VAR([DEB_HOST_GNU_TYPE], [Debian host/target alias])
dnl
dnl These commands are needed to perform DPKG package builds.
dnl
dnl Note that even if one could coax a non-debian system to build
dnl debs and dscs, they would not be useful.  Usually dpkg tools
dnl are just there for examining deb packages.
dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([debs],
	[AS_HELP_STRING([--disable-debs],
	    [build debs @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)	enable_debs=yes ;;
	    (*)				enabel_debs=no	;;
	esac])
    AC_ARG_ENABLE([dscs],
	[AS_HELP_STRING([--disable-dscs],
	    [build dscs @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)	enable_dscs=yes	;;
	    (*)				enable_dscs=no	;;
	esac])
    AC_ARG_VAR([DPKG],
	       [dpkg command. @<:@default=dpkg@:>@])
    _BLD_PATH_PROG([DPKG], [dpkg], [${am_missing3_run}dpkg], [$tmp_path], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)
		AC_MSG_WARN([Cannot find dpkg program in PATH.]) ;;
	    (*) enable_debs=no; enable_dscs=no ;;
	esac])
    AC_ARG_VAR([DPKG_ARCHITECTURE],
	       [dpkg-architecture command. @<:@default=dpkg-architecture@:>@])
    _BLD_PATH_PROG([DPKG_ARCHITECTURE], [dpkg-architecture], [${am_missing3_run}dpkg-architecture], [$tmp_path], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)
		AC_MSG_WARN([Cannot find dpkg-architecture program in PATH.]) ;;
	    (*) enable_debs=no; enable_dscs=no ;;
	esac])
    AC_ARG_VAR([DPKG_BUILDFLAGS],
	       [dpkg-buildflags command. @<:@default=dpkg-buildflags@:>@])
    _BLD_PATH_PROG([DPKG_BUILDFLAGS], [dpkg-buildflags], [${am_missing3_run}dpkg-buildflags], [$tmp_path], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)
		AC_MSG_WARN([Cannot find dpkg-buildflags program in PATH.]) ;;
	    (*) enable_debs=no; enable_dscs=no ;;
	esac])
    AC_ARG_VAR([DPKG_GENCHANGES],
	       [dpkg-genchanges command. @<:@default=dpkg-genchanges@:>@])
    _BLD_PATH_PROG([DPKG_GENCHANGES], [dpkg-genchanges], [${am_missing3_run}dpkg-genchanges], [$tmp_path], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)
		AC_MSG_WARN([Cannot find dpkg-genchanges program in PATH.]) ;;
	    (*) enable_debs=no; enable_dscs=no ;;
	esac])
    AC_ARG_VAR([DPKG_SOURCE],
	       [dpkg-source command. @<:@default=dpkg-source@:>@])
    _BLD_PATH_PROG([DPKG_SOURCE], [dpkg-source], [${am_missing3_run}dpkg-source], [$tmp_path], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)
		AC_MSG_WARN([Cannot find dpkg-source program in PATH.]) ;;
	    (*) enable_dscs=no ;;
	esac])
    AC_ARG_VAR([DPKG_BUILDPACKAGE],
	       [dpkg-buildpackage command. @<:@default=dpkg-buildpackage@:>@])
    _BLD_PATH_PROG([DPKG_BUILDPACKAGE], [dpkg-buildpackage], [${am_missing3_run}dpkg-buildpackage], [$tmp_path], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|mint)
		AC_MSG_WARN([Cannot find dpkg-buildpackage program in PATH.]) ;;
	    (*) enable_debs=no ;;
	esac])
    AC_CACHE_CHECK([for deb building of debs], [deb_cv_debs], [dnl
	deb_cv_debs=${enable_debs:-no}
    ])
    AC_CACHE_CHECK([for deb building of dscs], [deb_cv_dscs], [dnl
	deb_cv_dscs=${enable_dscs:-no}
    ])
    AM_CONDITIONAL([BUILD_DPKG], [test ":$deb_cv_debs:$deb_cv_dscs" = :yes:yes])dnl
])# _DEB_DPKG_SETUP_BUILD
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_HELPER
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_HELPER], [dnl
    AC_ARG_WITH([dh-compat],
	[AS_HELP_STRING([--with-dh-compat=LEVEL],
	    [debhelper compatability level @<:@default=5@:>@])],
	[], [with_dh_compat=5])
    AC_CACHE_CHECK([for deb helper compatability], [deb_cv_dh_compat], [dnl
	if test ":${DH_COMPAT+set}" != :set ; then
	    case :"${with_dh_compat:-no}" in
		(:no|:yes) DH_COMPAT=5 ;;
		(*)        DH_COMPAT="$with_dh_compat" ;;
	    esac
	fi
	deb_cv_dh_compat="$DH_COMPAT"
    ])
    if test ":${DH_COMPAT+set}" != :set ; then
	DH_COMPAT="${deb_cv_dh_compat:-5}"
    fi
    if test ":${DH_COMPAT:-5}" != :5 ; then
	PACKAGE_RPMOPTIONS="DH_COMPAT=\"$DH_COMPAT\"${PACKAGE_RPMOPTIONS:+ $PACKAGE_RPMOPTIONS}"
	PACKAGE_DEBOPTIONS="DH_COMPAT=\"$DH_COMPAT\"${PACKAGE_DEBOPTIONS:+ $PACKAGE_DEBOPTIONS}"
	ac_configure_args="DH_COMPAT=\"$DH_COMPAT\"${ac_configure_args:+ $ac_configure_args}"
    fi
    AC_SUBST([DH_COMPAT])dnl
    AC_ARG_WITH([dh-verbose],
	[AS_HELP_STRING([--with-dh-verbose=LEVEL],
	    [debhelper verbose level @<:@default=0@:>@])],
	[], [with_dh_verbose=0])
    AC_CACHE_CHECK([for deb helper verbosity], [deb_cv_dh_verbose], [dnl
	if test ":${DH_VERBOSE+set}" != :set ; then
	    case :"${with_dh_verbose:-no}" in
		(:no|:yes) DH_VERBOSE=0 ;;
		(*)        DH_VERBOSE="$with_dh_verbose" ;;
	    esac
	fi
	deb_cv_dh_verbose="$DH_VERBOSE"
    ])
    if test ":${DH_VERBOSE+set}" != :set ; then
	DH_VERBOSE="${deb_cv_dh_verbose:-0}"
    fi
    if test ":${DH_VERBOSE:-0}" != :0 ; then
	PACKAGE_RPMOPTIONS="DH_VERBOSE=\"$DH_VERBOSE\"${PACKAGE_RPMOPTIONS:+ $PACKAGE_RPMOPTIONS}"
	PACKAGE_DEBOPTIONS="DH_VERBOSE=\"$DH_VERBOSE\"${PACKAGE_DEBOPTIONS:+ $PACKAGE_DEBOPTIONS}"
	ac_configure_args="DH_VERBOSE=\"$DH_VERBOSE\"${ac_configure_args:+ $ac_configure_args}"
    fi
    AC_SUBST([DH_VERBOSE])dnl

])# _DEB_DPKG_SETUP_HELPER
# =============================================================================

# =============================================================================
# _DEB_REPO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_REPO_SETUP], [dnl
    _DEB_REPO_SETUP_APT
])# _DEB_REPO_SETUP
# =============================================================================

# =============================================================================
# _DEB_REPO_SETUP_APT
# -----------------------------------------------------------------------------
# These commands are needed to create DPKG (e.g. APT) repositories.
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_REPO_SETUP_APT], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-apt],
	[AS_HELP_STRING([--disable-repo-apt],
	    [apt repo construction @<:@default=auto@:>@])],
	[], [enable_repo_apt=yes])
    AC_ARG_VAR([APT_FTPARCHIVE],
	       [apt-ftparchive command. @<:@default=apt-ftparchive@:>@])
    _BLD_PATH_PROG([APT_FTPARCHIVE], [apt-ftparchive], [${am_missing3_run}apt-ftparchive], [$tmp_path], [dnl
	if test ":$deb_cv_debs" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(debian|ubuntu|mint)
		    _BLD_INSTALL_WARN([APT_FTPARCHIVE], [
*** 
*** Configure could not find a suitable tool for creating APT
*** repository metadata.  This program is part of the 'apt-utils'
*** package on Debian based distributions.
*** ], [
*** On Debian based distributions, try 'aptitude install apt-utils'.
***
*** Debian 5.0:   'aptitude install apt-utils'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-apt to
*** disable generation of APT repositories.  Proceeding under the
*** assumption that --disable-repo-apt was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'apt-ftparchive' program in PATH.]) ;;
	    esac
	else enable_repo_apt=no; fi])
    AC_ARG_VAR([DPKG_SCANSOURCES],
	       [dpkg-scansources command. @<:@default=dpkg-scansources@:>@])
    _BLD_PATH_PROG([DPKG_SCANSOURCES], [dpkg-scansources], [${am_missing3_run}dpkg-scansources], [$tmp_path], [dnl
	if test ":$deb_cv_debs" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(debian|ubuntu|mint)
		    _BLD_INSTALL_WARN([DPKG_SCANSOURCES], [
*** 
*** Configure could not find a suitable tool for creating APT source
*** repository metadata.  This program is part of the 'dpkg-dev' package
*** on Debian based distributions.
*** ], [
*** On Debian based distributions, try 'aptitude install dpkg-dev'.
***
*** Debian 5.0:   'aptitude install dpkg-dev'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-apt to
*** disable generation of APT repositories.  Proceeding under the
*** assumption that --disable-repo-apt was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'dpkg-scansources' program in PATH.]) ;;
	    esac
	else enable_repo_apt=no; fi])
    AC_ARG_VAR([DPKG_SCANPACKAGES],
	       [dpkg-scanpackages command. @<:@default=dpkg-scanpackages@:>@])
    _BLD_PATH_PROG([DPKG_SCANPACKAGES], [dpkg-scanpackages], [${am_missing3_run}dpkg-scanpackages], [$tmp_path], [dnl
	if test ":$deb_cv_debs" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(debian|ubuntu|mint)
		    _BLD_INSTALL_WARN([DPKG_SCANPACKAGES], [
*** 
*** Configure could not find a suitable tool for creating APT binary
*** repository metadata.  This program is part of the 'dpkg-dev' package
*** on Debian based distributions.
*** ], [
*** On Debian based distributions, try 'aptitude install dpkg-dev'.
***
*** Debian 5.0:   'aptitude install dpkg-dev'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-apt to
*** disable generation of APT repositories.  Proceeding under the
*** assumption that --disable-repo-apt was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'dpkg-scanpackages' program in PATH.]) ;;
	    esac
	else enable_repo_apt=no; fi])
    AC_ARG_VAR([DPKG_DEB],
	       [dpkg-deb command. @<:@default=dpkg-deb@:>@])
    _BLD_PATH_PROG([DPKG_DEB], [dpkg-deb], [${am_missing3_run}dpkg-deb], [$tmp_path], [dnl
	if test ":$deb_cv_debs" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(debian|ubuntu|mint)
		    _BLD_INSTALL_ERROR([DPKG_DEB], [
*** 
*** Configure could not find a suitable tool for examining APT binary
*** packages.  This program is part of the 'dpkg' package on Debian
*** based distributions and is essential.  Cannot continue.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'dpkg-deb' program in PATH.]) ;;
	    esac
	else enable_repo_apt=no; fi])
    AC_CACHE_CHECK([for deb apt repo construction], [deb_cv_repo_apt], [dnl
	deb_cv_repo_apt=${enable_repo_apt:-no}
    ])
    AM_CONDITIONAL([BUILD_REPO_APT], [test ":$deb_cv_repo_apt" = :yes])
])# _DEB_REPO_SETUP_APT
# =============================================================================

# =============================================================================
# _DEB_DPKG_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_OUTPUT], [dnl
dnl the conditional confuses automake forcing extra config.status runs.
dnl if test ":${DPKG_SOURCE:-no}" != :no -a ":${DPKG_BUILDPACKAGE:-no}" != :no ; then
	AC_CONFIG_FILES([debian/rules
			 debian/control
			 debian/changelog
			 debian/mscript_debhelper
			 debian/mscript_header
			 debian/mscript_preinst
			 debian/mscript_postinst
			 debian/mscript_prerm
			 debian/mscript_postrm])
	mscript_debhelper="debian/mscript_debhelper"
	mscript_header="debian/mscript_header"
	mscript_preinst="debian/mscript_preinst"
	mscript_postinst="debian/mscript_postinst"
	mscript_prerm="debian/mscript_prerm"
	mscript_postrm="debian/mscript_postrm"
	AC_SUBST_FILE([mscript_debhelper])
	AC_SUBST_FILE([mscript_header])
	AC_SUBST_FILE([mscript_preinst])
	AC_SUBST_FILE([mscript_postinst])
	AC_SUBST_FILE([mscript_prerm])
	AC_SUBST_FILE([mscript_postrm])
dnl fi
])# _DEB_DPKG_OUTPUT
# =============================================================================

# =============================================================================
# _DEB_
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_], [dnl
])# _DEB_
# =============================================================================

# =============================================================================
#
# $Log: deb.m4,v $
# Revision 1.1.2.11  2011-09-20 09:51:32  brian
# - updates from git
#
# Revision 1.1.2.10  2011-09-02 08:46:29  brian
# - sync up lots of repo and build changes from git
#
# Revision 1.1.2.9  2011-07-27 07:52:18  brian
# - work to support Mageia/Mandriva compressed kernel modules and URPMI repo
#
# Revision 1.1.2.8  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.7  2011-05-10 13:45:34  brian
# - weak modules workup
#
# Revision 1.1.2.6  2011-02-28 19:51:29  brian
# - better repository build
#
# Revision 1.1.2.5  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.4  2009-07-21 11:06:12  brian
# - changes from release build
#
# Revision 1.1.2.3  2009-07-04 03:51:32  brian
# - updates for release
#
# Revision 1.1.2.2  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# Revision 0.9.2.30  2008-10-27 12:23:34  brian
# - suppress warning on each iteration and cache results
#
# Revision 0.9.2.29  2008-09-21 13:39:53  brian
# - add patch level to debian changelog
#
# Revision 0.9.2.28  2008/09/21 07:40:45  brian
# - add defaults to environment variables
#
# Revision 0.9.2.27  2008-09-21 07:10:07  brian
# - environment passed to rpm and dpkg cannot be precious
#
# Revision 0.9.2.26  2008-09-20 23:06:07  brian
# - corrections
#
# Revision 0.9.2.25  2008-09-20 11:17:14  brian
# - build system updates
#
# Revision 0.9.2.24  2008-09-19 06:19:38  brian
# - typo
#
# Revision 0.9.2.23  2008-09-19 05:18:45  brian
# - separate repo directory by architecture
#
# Revision 0.9.2.22  2008-09-13 03:48:33  brian
# - repo subdirectories
#
# Revision 0.9.2.21  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.20  2007/10/17 20:00:26  brian
# - slightly different path checks
#
# Revision 0.9.2.19  2007/08/12 19:05:30  brian
# - rearrange and update headers
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
