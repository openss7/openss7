# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# =============================================================================

# =============================================================================
# _PAC_ARCH
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_TARDIR])
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_REQUIRE([_DISTRO])
    AC_REQUIRE([_REPO])
    AC_MSG_NOTICE([+-----------------------+])
    AC_MSG_NOTICE([| Pacman Archive Checks |])
    AC_MSG_NOTICE([+-----------------------+])
    _PAC_ARCH_OPTIONS
    _PAC_ARCH_SETUP
    _PAC_REPO_SETUP
    _PAC_ARCH_OUTPUT
    AM_CONDITIONAL([BUILD_PKGS], [test ":$pac_cv_pkgs" = :yes])dnl
    AM_CONDITIONAL([BUILD_SPKG], [test ":$pac_cv_spkg" = :yes])dnl
    AM_CONDITIONAL([BUILD_REPO_PACMAN], [test ":$pac_cv_repo_pacman" = :yes])
])# _PAC_ARCH
# =============================================================================

# =============================================================================
# _PAC_ARCH_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_OPTIONS], [dnl
    _PAC_OPTIONS_PAC_EPOCH
    _PAC_OPTIONS_PAC_RELEASE
])# _PAC_ARCH_OPTIONS
# =============================================================================

# =============================================================================
# _PAC_OPTIONS_PAC_EPOCH
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_OPTIONS_PAC_EPOCH], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_EPOCH])
    AC_MSG_CHECKING([for pacman epoch])
    AC_ARG_WITH([pac-epoch],
	[AS_HELP_STRING([--with-pac-epoch=EPOCH],
	    [pacman package EPOCH @<:@default=auto@:>@])],
	[], [dnl
	    if test -r .pacepoch; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.pacepoch
	    then with_pac_epoch="`cat ${d}.pacepoch`"
	    else with_pac_epoch="${with_pkg_epoch:-0}"
	    fi])
    AC_MSG_RESULT([${with_pac_epoch:-0}])
    PACKAGE_PACEPOCH="${with_pac_epoch:-0}"
    AC_SUBST([PACKAGE_PACEPOCH])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_PACEPOCH], [$PACKAGE_PACEPOCH], [The Arch Epoch.
			This defaults to 0.])
])# _PAC_OPTIONS_PAC_EPOCH
# =============================================================================

# =============================================================================
# _PAC_OPTIONS_PAC_RELEASE
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_OPTIONS_PAC_RELEASE], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_RELEASE])
    AC_MSG_CHECKING([for pacman release])
    AC_ARG_WITH([pac-release],
	[AS_HELP_STRING([--with-pac-release=RELEASE],
	    [pacman package RELEASE @<:@default=auto@:>@])],
	[], [dnl
	    if test -r .pacrelease ; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.pacrelease
	    then with_pac_release="`cat ${d}.pacrelease`"
	    else with_pac_release="${with_pkg_release:-1}"
	    fi])
    AC_MSG_RESULT([${with_pac_release:-1}])
    PACKAGE_PACRELEASE="${with_pac_release:-1}"
    AC_SUBST([PACKAGE_PACRELEASE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_PACRELEASE], [$PACKAGE_PACRELEASE], [The Arch
			Release.  This defaults to 1.])
])# _PAC_OPTIONS_PAC_RELEASE
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP], [dnl
dnl _PAC_ARCH_SETUP_DIST
dnl _PAC_ARCH_SETUP_ARCH
dnl _PAC_ARCH_SETUP_INDEP
    _PAC_ARCH_SETUP_TOPDIR
    _PAC_ARCH_SETUP_OPTIONS
    _PAC_ARCH_SETUP_BUILD
])# _PAC_ARCH_SETUP
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP_DIST
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP_DIST], [dnl
])# _PAC_ARCH_SETUP_DIST
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP_ARCH
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP_ARCH], [dnl
])# _PAC_ARCH_SETUP_ARCH
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP_INDEP
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP_INDEP], [dnl
])# _PAC_ARCH_SETUP_INDEP
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP_TOPDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP_TOPDIR], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_ARG_WITH([pac-distdir],
	[AS_HELP_STRING([--with-pac-distdir=DIR],
	    [pacman dist directory @<:@default=PKG-DISTDIR/arch/PKG-SUBDIR@:>@])],
	[], [with_pac_distdir='$(DISTDIR)/arch/$(reposubdir)'])
    AC_MSG_CHECKING([for pacman distribution directory])
    if test ":${pacdistdir+set}" != :set ; then
	case ":${with_pac_distdir:-no}" in
	    (:no|:yes)	pacdistdir='$(DISTDIR)/arch/$(reposubdir)' ;;
	    (*)		pacdistdir="$with_pac_distdir" ;;
	esac
    fi
    AC_MSG_RESULT([$pacdistdir])
    AC_SUBST([pacdistdir])dnl
    AC_ARG_WITH([pac_topdir],
	[AS_HELP_STRING([--with-pac-topdir=DIR],
	    [pacman top directory @<:@default=PAC-DISTDIR/BRANCH@:>@])],
	[], [with_pac_topdir='$(pacdistdir)$(repobranch)'])
    AC_MSG_CHECKING([for pacman top build directory])
    if test ":${pacdir+set}" != :set ; then
	case ":${with_pac_topdir:-no}" in
	    (:no|:yes)	pacdir='$(packdistdir)$(repobranch)' ;;
	    (*)		pacdir="${with_pac_topdir}" ;;
	esac
    fi
    AC_MSG_RESULT([$pacdir])
    AC_SUBST([pacdir])dnl
    pkgdir='$(pacdistdir)'
    AC_SUBST([pkgdir])dnl
    pkgfulldir='$(pacdir)'
    AC_SUBST([pkgfulldir])dnl
    pkgmaindir='$(pacdir)/main'
    AC_SUBST([pkgmaindir])dnl
    pkgdebgdir='$(pacdir)/debug'
    AC_SUBST([pkgdebgdir])dnl
    pkgdevldir='$(pacdir)/devel'
    AC_SUBST([pkgdevldir])dnl
    pkgsrcsdir='$(pacdir)/source'
    AC_SUBST([pkgsrcsdir])dnl
    AC_MSG_CHECKING([for pacman BUILD directory])
    if test ":${mpkgtopdir+set}" != :set ; then
	# mpkgtopdir needs to be absolute: always build in the top build
	# directory on the local machine
	mpkgtopdir=`pwd`/arch
    fi
    AC_MSG_RESULT([$mpkgtopdir])
    AC_SUBST([mpkgtopdir])dnl
    AC_MSG_CHECKING([for pacman source directory])
    if test ":${mpkgsourcedir+set}" != :set ; then
	mpkgsourcedir="${mpkgtopdir}/src"
    fi
    AC_MSG_RESULT([$mpkgsourcedir])
    AC_SUBST([mpkgsourcedir])dnl
    AC_MSG_CHECKING([for pacman build directory])
    if test ":${mpkgbuilddir+set}" != :set ; then
	mpkgbuilddir="${mpkgtopdir}/bld"
    fi
    AC_MSG_RESULT([$mpkgbuilddir])
    AC_SUBST([mpkgbuilddir])dnl
    AC_MSG_CHECKING([for pacman install directory])
    if test ":${mpkginstalldir+set}" != :set ; then
	mpkginstalldir="${mpkgtopdir}/pkg"
    fi
    AC_MSG_RESULT([$mpkginstalldir])
    AC_SUBST([mpkginstalldir])dnl
])# _PAC_ARCH_SETUP_TOPDIR
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP_OPTIONS], [dnl
    arg=
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
		eval "arg=$arg"
		AC_MSG_CHECKING([for makepkg argument '$arg'])
		if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
		    PACKAGE_PACOPTIONS="${PACKAGE_PACOPTIONS}${PACKAGE_PACOPTIONS:+ }$arg"
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
	eval "arg=$arg"
	AC_MSG_CHECKING([for makepkg argument $arg])
	if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
	    PACKAGE_PACOPTIONS="${PACKAGE_PACOPTIONS}${PACKAGE_PACOPTIONS:+ }$arg"
	    AC_MSG_RESULT([yes])
	else
	    :
	    AC_MSG_RESULT([no])
	fi
    fi
    AC_SUBST([PACKAGE_PACOPTIONS])dnl
])# _PAC_ARCH_SETUP_OPTIONS
# =============================================================================

# =============================================================================
# _PAC_ARCH_SETUP_BUILD
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_SETUP_BUILD], [dnl
  AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([pkgs],
	[AS_HELP_STRING([--disable-pkgs],
	    [build pacman pkgs @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (arch)	enable_pkgs=yes ;;
	    (*)		enable_pkgs=no  ;;
	esac])
    AC_ARG_ENABLE([srcs],
	[AS_HELP_STRING([--disable-spkg],
	    [build packman src pkgs @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (arch)	enable_spkg=yes ;;
	    (*)		enable_spkg=no  ;;
	esac])
    AC_ARG_VAR([PACMAN],
	       [pacman command. @<:@default=pacman@:>@])
    _BLD_PATH_PROG([PACMAN], [pacman], [${am_missing3_run}pacman], [$tmp_path], [dnl
	case "$target_vendor" in
	    (arch)
		AC_MSG_WARN([Cannot find pacman program in PATH.]) ;;
	    (*) enable_pkgs=no; enable_spkg=no ;;
	esac])
    AC_ARG_VAR([MAKEPKG],
	       [makepkg command. @<:@default=makepkg@:>@])
    _BLD_PATH_PROG([MAKEPKG], [makepkg], [${am_missing3_run}makepkg], [$tmp_path], [dnl
	case "$target_vendor" in
	    (arch)
		AC_MSG_WARN([Cannot find makepkg program in PATH.]) ;;
	    (*) enable_pkgs=no; enable_spkg=no ;;
	esac])
    AC_CACHE_CHECK([for pacman building of pkgs], [pac_cv_pkgs], [dnl
	pac_cv_pkgs=${enable_pkgs:-no}
    ])
    AC_CACHE_CHECK([for pacman building of src pkgs], [pac_cv_spkg], [dnl
	pac_cv_spkg=${enable_spkg:-no}
    ])
])# _PAC_ARCH_SETUP_BUILD
# =============================================================================

# =============================================================================
# _PAC_REPO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_REPO_SETUP], [dnl
    _PAC_REPO_SETUP_PACMAN
])# _PAC_REPO_SETUP
# =============================================================================

# =============================================================================
# _PAC_REPO_SETUP_PACMAN
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_REPO_SETUP_PACMAN], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-pacman],
	[AS_HELP_STRING([--disable-repo-pacman],
	    [arch repo construction @<:@default=auto@:>@])],
	[], [enable_repo_pacman=yes])
    AC_ARG_VAR([REPO_ADD],
	       [repo-add command. @<:@default=repo-add@:>@])
    _BLD_PATH_PROG([REPO_ADD], [repo-add], [${am_missing3_run}repo-add], [$tmp_path], [dnl
	if test ":$pac_cv_pkgs" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(arch)
		    _BLD_INSTALL_WARN([REPO_ADD], [
*** 
*** Configure could not find a suitable tool for creating Arch linux
*** repository databases.  This program is part of the 'pacman' package
*** on Arch based distributions.
*** ], [
*** on Arch base distributiones, try 'pacman --sync pacman'.
***
*** Arch Linux:	'pacman --sync pacman'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-pacman to
*** disable generation of Arch databases.  Proceeding under the
*** assumption that --disable-repo-pacman was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'repo-remove' program in PATH.]) ;;
	    esac
	else enable_repo_pacman=no; fi])
    AC_ARG_VAR([REPO_REMOVE],
	       [repo-remove command. @<:@default=repo-remove@:>@])
    _BLD_PATH_PROG([REPO_REMOVE], [repo-remove], [${am_missing3_run}repo-remove], [$tmp_path], [dnl
	if test ":$pac_cv_pkgs" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes; then
	    case "${target_vendor:-none}" in
		(arch)
		    _BLD_INSTALL_WARN([REPO_REMOVE], [
*** 
*** Configure could not find a suitable tool for creating Arch linux
*** repository databases.  This program is part of the 'pacman' package
*** on Arch based distributions.
*** ], [
*** on Arch base distributiones, try 'pacman --sync pacman'.
***
*** Arch Linux:	'pacman --sync pacman'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-pacman to
*** disable generation of Arch databases.  Proceeding under the
*** assumption that --disable-repo-pacman was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'repo-remove' program in PATH.]) ;;
	    esac
	else enable_repo_pacman=no; fi])
    AC_CACHE_CHECK([for pacman repo construction], [pac_cv_repo_pacman], [dnl
	pac_cv_repo_pacman=${enable_repo_pacman:-no}
    ])
])# _PAC_REPO_SETUP_PACMAN
# =============================================================================

# =============================================================================
# _PAC_ARCH_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_ARCH_OUTPUT], [dnl
dnl the conditional confuses automake forcing extra config.status runs.
dnl if test ":${REPO_ADD:-no}" != :no -a ":${REPO_REMOVE:-no}" != :no; then
	AC_CONFIG_FILES([arch/PKGBUILD
			 arch/install_header])
	AC_CONFIG_FILES([arch/]AC_PACKAGE_TARNAME[.install
			 arch/]AC_PACKAGE_TARNAME[-repo.install
			 arch/]AC_PACKAGE_TARNAME[-base.install
			 arch/]AC_PACKAGE_TARNAME[-libs.install
			 arch/]AC_PACKAGE_TARNAME[-compat.install
			 arch/]AC_PACKAGE_TARNAME[-devel.install
			 arch/]AC_PACKAGE_TARNAME[-debuginfo.install
			 arch/]AC_PACKAGE_TARNAME[-debugsource.install
			 arch/]AC_PACKAGE_TARNAME[-kernel.install
			 arch/]AC_PACKAGE_TARNAME[-kernel-devel.install
			 arch/]AC_PACKAGE_TARNAME[-kernel-debuginfo.install
			 arch/]AC_PACKAGE_TARNAME[-kernel-debugsource.install
			 arch/]AC_PACKAGE_TARNAME[-kernel-source.install])
	install_header="arch/install_header"
	AC_SUBST_FILE([install_header])
dnl fi
])# _PAC_ARCH_OUTPUT
# =============================================================================

# =============================================================================
# _PAC_
# -----------------------------------------------------------------------------
AC_DEFUN([_PAC_], [dnl
])# _PAC_
# =============================================================================

# =============================================================================
#
# $Log$
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
