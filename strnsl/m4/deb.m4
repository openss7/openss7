# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: deb.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.25 $) $Date: 2008-09-20 11:17:14 $
#
# -----------------------------------------------------------------------------
#
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
# Last Modified $Date: 2008-09-20 11:17:14 $ by $Author: brian $
#
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
	AS_HELP_STRING([--with-deb-epoch=EPOCH],
	    [specify the EPOCH for the package file.  @<:@default=auto@:>@]),
	[with_deb_epoch="$withval"],
	[if test -r .debepoch; then d= ; else d="$srcdir/" ; fi
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
	AS_HELP_STRING([--with-deb-release=RELEASE],
	    [specify the RELEASE for the package files.  @<:@default=auto@:>@]),
	[with_deb_release="$withval"],
	[if test -r .debrelease ; then d= ; else d="$srcdir/" ; fi
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
    _DEB_DPKG_SETUP_DIST
dnl _DEB_DPKG_SETUP_ARCH
dnl _DEB_DPKG_SETUP_INDEP
    _DEB_DPKG_SETUP_TOPDIR
    _DEB_DPKG_SETUP_OPTIONS
    _DEB_DPKG_SETUP_BUILD
])# _DEB_DPKG_SETUP
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_DIST
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_DIST], [dnl
    AC_CACHE_CHECK([for deb distribution subdirectory], [deb_cv_dist_subdir], [dnl
	case "$dist_cv_host_flavor" in
	    (centos|lineox|whitebox|fedora|mandrake|mandriva|redhat|suse)
		deb_cv_dist_subdir="$dist_cv_host_flavor/$dist_cv_host_release/$dist_cv_host_cpu" ;;
	    (debian|ubuntu|*)
		deb_cv_dist_subdir="$dist_cv_host_flavor/$dist_cv_host_codename/$dist_cv_host_cpu" ;;
	esac
	if test -n "$deb_cv_dist_subdir"; then
	    deb_cv_dist_subdir=`echo "$deb_cv_dist_subdir" | sed -e 'y,ABCDEFGHIJKLMNOPQRSTUVWXYZ,abcdefghijklmnopqrstuvwxyz,'`
	fi
    ])
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
	AS_HELP_STRING([--enable-arch],
	    [build and install arch packages.  @<:@default=yes@:>@]),
	[enable_arch="$enableval"],
	[enable_arch='yes'])
    AC_MSG_RESULT([${enable_arch:-yes}])
    AM_CONDITIONAL([DEB_BUILD_ARCH], [test :"${enable_arch:-yes}" = :yes])dnl
	
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
	AS_HELP_STRING([--enable-indep],
	    [build and install indep packages.  @<:@default=yes@:>@]),
	[enable_indep="$enableval"],
	[enable_indep='yes'])
    AC_MSG_RESULT([${enable_indep:-yes}])
    AM_CONDITIONAL([DEB_BUILD_INDEP], [test :"${enable_indep:-yes}" = :yes])dnl
])# _DEB_DPKG_SETUP_INDEP
# =============================================================================

# =============================================================================
# _DEB_DPKG_SETUP_TOPDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_SETUP_TOPDIR], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    if test :"$PACKAGE_DISTDIR" = :`pwd` ; then
	deb_tmp='$(PACKAGE_DISTDIR)/debian'
    else
	deb_tmp='$(PACKAGE_DISTDIR)/debs/'"${deb_cv_dist_subdir}"
    fi
    AC_ARG_WITH([deb-topdir],
	AS_HELP_STRING([--with-deb-topdir=DIR],
	    [specify the deb top directory.  @<:@default=PKG-DISTDIR/debian@:>@]),
	[with_deb_topdir="$withval"],
	[with_deb_topdir="$deb_tmp"])
    AC_CACHE_CHECK([for deb top build directory], [deb_cv_topdir], [dnl
	case :"$with_deb_topdir" in
	    (:no | :NO)
		deb_cv_topdir="$deb_tmp"
		;;
	    (:yes | :YES | :default | :DEFAULT)
		deb_cv_topdir="/usr/src/debian"
		;;
	    (*)
		deb_cv_topdir="$with_deb_topdir"
		;;
	esac
    ])
    PACKAGE_DEBTOPDIR="$deb_cv_topdir"
    AC_SUBST([PACKAGE_DEBTOPDIR])dnl
    debdir='$(PACKAGE_DEBTOPDIR)'
    AC_SUBST([debdir])dnl
    AC_CACHE_CHECK([for deb BUILD directory], [deb_cv_builddir], [dnl
	# debbuilddir needs to be absolute: always build in the top build
	# directory on the local machine
	deb_cv_builddir=`pwd`/debian
    ])
    debbuilddir="$deb_cv_builddir"
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
dnl These environment variables are precious
dnl
    AC_ARG_VAR([DEB_BUILD_ARCH], [Debian build architecture])
    AC_ARG_VAR([DEB_BUILD_GNU_CPU], [Debian build cpu])
    AC_ARG_VAR([DEB_BUILD_GNU_SYSTEM], [Debian build os])
    AC_ARG_VAR([DEB_BUILD_GNU_TYPE], [Debian build alias])
    AC_ARG_VAR([DEB_HOST_ARCH], [Debian host/target architecture])
    AC_ARG_VAR([DEB_HOST_GNU_CPU], [Debian host/target cpu])
    AC_ARG_VAR([DEB_HOST_GNU_SYSTEM], [Debian host/target os])
    AC_ARG_VAR([DEB_HOST_GNU_TYPE], [Debian host/target alias])
dnl
dnl These commands are needed to perform DPKG package builds.
dnl
    BUILD_DPKG=no
    AC_ARG_VAR([DPKG],
	       [dpkg command. @<:@default=dpkg@:>@])
    AC_PATH_PROG([DPKG], [dpkg], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test -z "$DPKG"; then
	case "$target_vendor" in
	    (debian|ubuntu)
	    AC_MSG_WARN([Could not find dpkg program in PATH.])
	    ;;
	esac
	DPKG="${am_missing3_run}dpkg"
    fi
    AC_ARG_VAR([DPKG_SOURCE],
	       [dpkg-source command. @<:@default=dpkg-source@:>@])
    AC_PATH_PROG([DPKG_SOURCE], [dpkg-source], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test -z "$DPKG_SOURCE"; then
	case "$target_vendor" in
	    (debian|ubuntu)
	    AC_MSG_WARN([Could not find dpkg-source program in PATH.])
	    ;;
	esac
	DPKG_SOURCE="${am_missing3_run}dpkg-source"
    fi
    AC_ARG_VAR([DPKG_BUILDPACKAGE],
	       [dpkg-buildpackage command. @<:@default=dpkg-buildpackage@:>@])
    AC_PATH_PROG([DPKG_BUILDPACKAGE], [dpkg-buildpackage], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test -z "$DPKG_BUILDPACKAGE"; then
	case "$target_vendor" in
	    (debian|ubuntu)
	    AC_MSG_WARN([Could not find dpkg-buildpackage program in PATH.])
	    ;;
	esac
	DPKG_BUILDPACKAGE="${am_missing3_run}dpkg-buildpackage"
    fi
    case :$ac_cv_path_DPKG_SOURCE:$ac_cv_path_DPKG_BUILDPACKAGE in
	(::*|::|*:) ;; (*) BUILD_DPKG=yes ;;
    esac
    AM_CONDITIONAL([BUILD_DPKG], [test :$BUILD_DPKG = :yes])dnl
dnl
dnl These commands are needed to create DPKG (e.g. APT) repositories.
dnl
    AC_ARG_ENABLE([repo-apt],
	AS_HELP_STRING([--disable-repo-apt],
	    [disable apt repo construction.  @<:@default=auto@:>@]),
	[enable_repo_apt="$enableval"],
	[enable_repo_apt=yes])
    AC_ARG_VAR([APT_FTPARCHIVE],
	       [apt-ftparchive command. @<:@default=apt-ftparchive@:>@])
    AC_PATH_PROG([APT_FTPARCHIVE], [apt-ftparchive], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test "-z $APT_FTPARCHIVE"; then
	if test ":$enable_dpkg" = :yes; then
	    case "$target_vendor" in
		(debian|ubuntu)
		AC_MSG_WARN([Could not find apt-ftparchive program in PATH.])
		;;
		(*)
		enable_repo_apt=no
		;;
	    esac
	else
	    enable_repo_apt=no
	fi
	APT_FTPARCHIVE="${am_missing3_run}apt-ftparchive"
    fi
    AC_ARG_VAR([DPKG_SCANSOURCES],
	       [dpkg-scansources command. @<:@default=dpkg-scansources@:>@])
    AC_PATH_PROG([DPKG_SCANSOURCES], [dpkg-scansources], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test "-z $DPKG_SCANSOURCES"; then
	if test ":$enable_dpkg" = :yes; then
	    case "$target_vendor" in
		(debian|ubuntu)
		AC_MSG_WARN([Could not find dpkg-scansources program in PATH.])
		;;
		(*)
		enable_repo_apt=no
		;;
	    esac
	else
	    enable_repo_apt=no
	fi
	DPKG_SCANSOURCES="${am_missing3_run}dpkg-scansources"
    fi
    AC_ARG_VAR([DPKG_SCANPACKAGES],
	       [dpkg-scanpackages command])
    AC_PATH_PROG([DPKG_SCANPACKAGES], [dpkg-scanpackages], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test -z "$DPKG_SCANPACKAGES"; then
	if test ":$enable_dpkg" = :yes; then
	    case "$target_vendor" in
		(debian|ubuntu)
		AC_MSG_WARN([Could not find dpkg-scanpackages program in PATH.])
		;;
		(*)
		enable_repo_apt=no
		;;
	    esac
	else
	    enable_repo_apt=no
	fi
	DPKG_SCANPACKAGES="${am_missing3_run}dpkg-scanpackages"
    fi
    AC_ARG_VAR([DPKG_DEB],
	       [dpkg-deb command])
    AC_PATH_PROG([DPKG_DEB], [dpkg-deb], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test -z "$DPKG_DEB"; then
	if test ":$enable_dpkg" = :yes; then
	    case "$target_vendor" in
		(debian|ubuntu)
		AC_MSG_WARN([Could not find dpkg-deb program in PATH.])
		;;
		(*)
		enable_repo_apt=no
		;;
	    esac
	else
	    enable_repo_apt=no
	fi
	DPK_DEB="${am_missing3_run}dpkg-deb"
    fi
    AM_CONDITIONAL([BUILD_REPO_APT], [test ":$enable_repo_apt" = :yes])
    aptdir='$(PACKAGE_DEBTOPDIR)'
    AC_SUBST([aptdir])dnl
])# _DEB_DPKG_SETUP_BUILD
# =============================================================================

# =============================================================================
# _DEB_DPKG_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG_OUTPUT], [dnl
dnl the conditional confuses automake forcing extra config.status runs.
dnl if test :"${DPKG_SOURCE:-no}" != :no -a :"${DPKG_BUILDPACKAGE:-no}" != :no ; then
	AC_CONFIG_FILES([debian/rules
			 debian/control
			 debian/mscript_header
			 debian/mscript_preinst
			 debian/mscript_postinst
			 debian/mscript_prerm
			 debian/mscript_postrm])
	mscript_header="debian/mscript_header"
	mscript_preinst="debian/mscript_preinst"
	mscript_postinst="debian/mscript_postinst"
	mscript_prerm="debian/mscript_prerm"
	mscript_postrm="debian/mscript_postrm"
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
# _DEB_DPKG
# -----------------------------------------------------------------------------
AC_DEFUN([_DEB_DPKG], [dnl
    AC_REQUIRE([_DISTRO])
    _DEB_DPKG_OPTIONS
    _DEB_DPKG_SETUP
    _DEB_DPKG_OUTPUT
])# _DEB_DPKG
# =============================================================================

# =============================================================================
#
# $Log: deb.m4,v $
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
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
