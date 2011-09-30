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
# _TXZ_SLACK
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_SLACK], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_TARDIR])
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_REQUIRE([_DISTRO])
    AC_REQUIRE([_REPO])
    AC_MSG_NOTICE([+-------------------------+])
    AC_MSG_NOTICE([| Slackpkg Archive Checks |])
    AC_MSG_NOTICE([+-------------------------+])
    _TXZ_ARCH_OPTIONS
    _TXZ_ARCH_SETUP
    _TXZ_REPO_SETUP
    _TXZ_ARCH_OUTPUT
])# _TXZ_SLACK
# =============================================================================

# =============================================================================
# _TXZ_ARCH_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_OPTIONS], [dnl
    _TXZ_OPTIONS_TXZ_EPOCH
    _TXZ_OPTIONS_TXZ_RELEASE
])# _TXZ_ARCH_OPTIONS
# =============================================================================

# =============================================================================
# _TXZ_OPTIONS_TXZ_EPOCH
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_OPTIONS_TXZ_EPOCH], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_EPOCH])
    AC_MSG_CHECKING([for txz epoch])
    AC_ARG_WITH([txz-epoch],
	[AS_HELP_STRING([--with-txz-epoch=EPOCH],
	    [slackpkg package EPOCH @<:@default=auto@:>@])],
	[], [dnl
	    if test -r .txzepoch; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.txzepoch
	    then with_txz_epoch="`cat ${d}.txzepoch`"
	    else with_txz_epoch="${with_pkg_epoch:-0}"
	    fi])
    AC_MSG_RESULT([${with_txz_epoch:-0}])
    PACKAGE_PACEPOCH="${with_txz_epoch:-0}"
    AC_SUBST([PACKAGE_PACEPOCH])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_PACEPOCH], [$PACKAGE_PACEPOCH], [The Slackware
			Epoch.  This defaults to 0.])
])# _TXZ_OPTIONS_TXZ_EPOCH
# =============================================================================

# =============================================================================
# _TXZ_OPTIONS_TXZ_RELEASE
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_OPTIONS_TXZ_RELEASE], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_RELEASE])
    AC_MSG_CHECKING([for txz release])
    AC_ARG_WITH([txz-release],
	[AS_HELP_STRING([--with-txz-release=RELEASE],
	    [slackpkg package RELEASE @<:@default=auto@:>@])],
	[], [dnl
	    if test -r .txzrelease ; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.txzrelease
	    then with_txz_release="`cat ${d}.txzrelease`"
	    else with_txz_release="${with_pkg_release:-1}"
	    fi])
    AC_MSG_RESULT([${with_txz_release:-1}])
    PACKAGE_TXZRELEASE="${with_txz_release:-1}"
    AC_SUBST([PACKAGE_TXZRELEASE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_TXZRELEASE], [$PACKAGE_TXZRELEASE], [The
			Slackware Release.  This defaults to 1.])
])# _TXZ_OPTIONS_TXZ_RELEASE
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP], [dnl
dnl _TXZ_ARCH_SETUP_DIST
dnl _TXZ_ARCH_SETUP_ARCH
dnl _TXZ_ARCH_SETUP_INDEP
    _TXZ_ARCH_SETUP_TOPDIR
    _TXZ_ARCH_SETUP_OPTIONS
    _TXZ_ARCH_SETUP_BUILD
])# _TXZ_ARCH_SETUP
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP_DIST
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP_DIST], [dnl
])# _TXZ_ARCH_SETUP_DIST
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP_ARCH
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP_ARCH], [dnl
])# _TXZ_ARCH_SETUP_ARCH
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP_INDEP
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP_INDEP], [dnl
])# _TXZ_ARCH_SETUP_INDEP
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP_TOPDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP_TOPDIR], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_ARG_WITH([txz-distdir],
	[AS_HELP_STRING([--with-txz-distdir=DIR],
	    [slackpkg dist directory @<:@default=PKG-DISTDIR/slackware/PKG-SUBDIR@:>@])],
	[], [with_txz_distdir='$(DISTDIR)/slackware/$(reposubdir)'])
    AC_MSG_CHECKING([for slackpkg distribution directory])
    if test ":${txzdistdir+set}" != :set ; then
	case ":${with_txz_distdir:-no}" in
	    (:no|:yes)	txzdistdir='$(DISTDIR)/slackware/$(reposubdir)' ;;
	    (*)		txzdistdir="$with_txz_distdir" ;;
	esac
    fi
    AC_MSG_RESULT([$txzdistdir])
    AC_SUBST([txzdistdir])dnl
    AC_ARG_WITH([txz_topdir],
	[AS_HELP_STRING([--with-txz-topdir=DIR],
	    [slackpkg top directory @<:@default=TXZ-DISTDIR/BRANCH@:>@])],
	[], [with_txz_topdir='$(txzdistdir)$(repobranch)'])
    AC_MSG_CHECKING([for slackpkg top build directory])
    if test ":${txzdir+set}" != :set ; then
	case ":${with_txz_topdir:-no}" in
	    (:no|:yes)	txzdir='$(txzkdistdir)$(repobranch)' ;;
	    (*)		txzdir="${with_txz_topdir}" ;;
	esac
    fi
    AC_MSG_RESULT([$txzdir])
    AC_SUBST([txzdir])dnl
    spkgdir='$(txzdistdir)'
    AC_SUBST([spkgdir])dnl
    spkgfulldir='$(txzdir)'
    AC_SUBST([spkgfulldir])dnl
    spkgmaindir='$(txzdir)/main'
    AC_SUBST([spkgmaindir])dnl
    spkgdebgdir='$(txzdir)/debug'
    AC_SUBST([spkgdebgdir])dnl
    spkgdevldir='$(txzdir)/devel'
    AC_SUBST([spkgdevldir])dnl
    spkgsrcsdir='$(txzdir)/source'
    AC_SUBST([spkgsrcsdir])dnl
    AC_MSG_CHECKING([for slackpkg BUILD directory])
    if test ":${spkgtopdir+set}" != :set ; then
	# spkgtopdir needs to be absolute: always build in the top build
	# directory on the local machine
	spkgtopdir=`pwd`/slackware
    fi
    AC_MSG_RESULT([$spkgtopdir])
    AC_SUBST([spkgtopdir])dnl
    AC_MSG_CHECKING([for makepkg source directory])
    if test ":${spkgsourcedir+set}" != :set ; then
	spkgsourcedir="${spkgtopdir}/src"
    fi
    AC_MSG_RESULT([$spkgsourcedir])
    AC_SUBST([spkgsourcedir])dnl
    AC_MSG_CHECKING([for makepkg build directory])
    if test ":${spkgbuilddir+set}" != :set ; then
	spkgbuilddir="${spkgtopdir}/bld"
    fi
    AC_MSG_RESULT([$spkgbuilddir])
    AC_SUBST([spkgbuilddir])dnl
    AC_MSG_CHECKING([for makepkg install directory])
    if test ":${spkginstalldir+set}" != :set ; then
	spkginstalldir="${spkgtopdir}/pkg"
    fi
    AC_MSG_RESULT([$spkginstalldir])
    AC_SUBST([spkginstalldir])dnl
])# _TXZ_ARCH_SETUP_TOPDIR
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP_OPTIONS], [dnl
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
		AC_MSG_CHECKING([for slackpkg argument '$arg'])
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
	AC_MSG_CHECKING([for deb argument $arg])
	if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
	    PACKAGE_PACOPTIONS="${PACKAGE_PACOPTIONS}${PACKAGE_PACOPTIONS:+ }$arg"
	    AC_MSG_RESULT([yes])
	else
	    :
	    AC_MSG_RESULT([no])
	fi
    fi
    AC_SUBST([PACKAGE_PACOPTIONS])dnl
])# _TXZ_ARCH_SETUP_OPTIONS
# =============================================================================

# =============================================================================
# _TXZ_ARCH_SETUP_BUILD
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_SETUP_BUILD], [dnl
  AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([slackpkg],
	[AS_HELP_STRING([--disable-slackpkg],
	    [build slackpkg @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (slackware)	enable_slackpkg=yes ;;
	    (*)		enable_slackpkg=no  ;;
	esac])
    AC_ARG_VAR([SLACKPKG],
	       [slackpkg command. @<:@default=slackpkg@:>@])
    _BLD_PATH_PROG([SLACKPKG], [slackpkg], [${am_missing3_run}slackpkg], [$tmp_path], [dnl
	case "$target_vendor" in
	    (slackware)
		AC_MSG_WARN([Cannot find slackpkg program in PATH.]) ;;
	    (*) enable_slackpkg=no ;;
	esac])
    AC_ARG_VAR([MAKEPKG],
	       [makepkg command. @<:@default=makepkg@:>@])
    _BLD_PATH_PROG([MAKEPKG], [makepkg], [${am_missing3_run}makepkg], [$tmp_path], [dnl
	case "$target_vendor" in
	    (slackware)
		AC_MSG_WARN([Cannot find makepkg program in PATH.]) ;;
	    (*) enable_slackpkg=no ;;
	esac])
    AC_CACHE_CHECK([for pkg building of slackpkg], [txz_cv_pkgs], [dnl
	txz_cv_pkgs=${enable_slackpkg:-no}
    ])
    AM_CONDITIONAL([BUILD_TXZS], [test ":$txz_cv_pkgs" = :yes])dnl
])# _TXZ_ARCH_SETUP_BUILD
# =============================================================================

# =============================================================================
# _TXZ_REPO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_REPO_SETUP], [dnl
    _TXZ_REPO_SETUP_SLACKPKG
])# _TXZ_REPO_SETUP
# =============================================================================

# =============================================================================
# _TXZ_REPO_SETUP_SLACKPKG
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_REPO_SETUP_SLACKPKG], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-slackpkg],
	[AS_HELP_STRING([--disable-repo-slackpkg],
	    [slackpkg repo construction @<:@default=auto@:>@])],
	[], [enable_repo_slackpkg=yes])
    AC_CACHE_CHECK([for slackpkg repo construction], [txz_cv_repo_slackpkg], [dnl
	txz_cv_repo_slackpkg=${enable_repo_slackpkg:-no}
    ])
    AM_CONDITIONAL([BUILD_REPO_SLACKPKG], [test ":$txz_cv_repo_slackpkg" = :yes])
])# _TXZ_REPO_SETUP_SLACKPKG
# =============================================================================

# =============================================================================
# _TXZ_ARCH_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_ARCH_OUTPUT], [dnl
dnl the conditional confuses automake forcing extra config.status runs.
dnl if test ":${REPO_ADD:-no}" != :no -a ":${REPO_REMOVE:-no}" != :no; then
dnl	AC_CONFIG_FILES([arch/PKGBUILD
dnl			 arch/install_header])
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME]).install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-repo.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-base.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-libs.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-compat.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-devel.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-debuginfo.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-debugsource.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-kernel.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-kernel-devel.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-kernel-debuginfo.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-kernel-debugsource.install)
dnl	AC_CONFIG_FILES(arch/m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME])-kernel-source.install)
dnl	install_header="arch/install_header"
dnl	AC_SUBST_FILE([install_header])
dnl fi
])# _TXZ_ARCH_OUTPUT
# =============================================================================

# =============================================================================
# _TXZ_
# -----------------------------------------------------------------------------
AC_DEFUN([_TXZ_], [dnl
])# _TXZ_
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
