# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: rpm.m4,v $ $Name:  $($Revision: 1.1.2.15 $) $Date: 2011-08-07 11:14:37 $
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
# _RPM_SPEC
# -----------------------------------------------------------------------------
# Common things that need to be done in setting up an RPM spec file from an
# RPM.spec.in file.  This also includes stuff for converting the LSM file.
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_TARDIR])
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_REQUIRE([_DISTRO])dnl
    AC_REQUIRE([_REPO])dnl
    AC_MSG_NOTICE([+------------------------+])
    AC_MSG_NOTICE([| RPM Repository Support |])
    AC_MSG_NOTICE([+------------------------+])
    _RPM_SPEC_OPTIONS
    _RPM_SPEC_SETUP
    _RPM_REPO_SETUP
    _RPM_SPEC_OUTPUT
])# _RPM_SPEC
# =============================================================================

# =============================================================================
# _RPM_SPEC_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_OPTIONS], [dnl
    _RPM_OPTIONS_RPM_EPOCH
    _RPM_OPTIONS_RPM_RELEASE
])# _RPM_SPEC_OPTIONS
# =============================================================================

# =============================================================================
# _RPM_OPTIONS_RPM_EPOCH
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_OPTIONS_RPM_EPOCH], [dnl
    AC_MSG_CHECKING([for rpm epoch])
    AC_ARG_WITH([rpm-epoch],
	[AS_HELP_STRING([--with-rpm-epoch=EPOCH],
	    [rpm EPOCH for package @<:@default=auto@:>@])], [], [dnl
	    if test -r .rpmepoch; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.rpmepoch
	    then with_rpm_epoch="`cat ${d}.rpmepoch`"
	    else with_rpm_epoch=0
	    fi])
    AC_MSG_RESULT([${with_rpm_epoch:-0}])
    PACKAGE_RPMEPOCH="${with_rpm_epoch:-0}"
    AC_SUBST([PACKAGE_RPMEPOCH])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_RPMEPOCH], [$PACKAGE_RPMEPOCH], [The RPM Epoch.
			This defaults to 0.])
])# _RPM_OPTIONS_RPM_EPOCH
# =============================================================================

# =============================================================================
# _RPM_OPTIONS_RPM_RELEASE
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_OPTIONS_RPM_RELEASE], [dnl
    AC_MSG_CHECKING([for rpm release])
    AC_ARG_WITH([rpm-release],
	[AS_HELP_STRING([--with-rpm-release=RELEASE],
	    [rpm RELEASE for package @<:@default=auto@:>@])], [], [dnl
	    if test -r .rpmrelease ; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.rpmrelease
	    then with_rpm_release="`cat ${d}.rpmrelease`"
	    else with_rpm_release=1
	    fi])
    AC_MSG_RESULT([${with_rpm_release:-1}])
    PACKAGE_RPMRELEASE="${with_rpm_release:-1}"
    AC_SUBST([PACKAGE_RPMRELEASE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_RPMRELEASE], ["$PACKAGE_RPMRELEASE"], [The RPM
			Release. This defaults to 1.])
])# _RPM_OPTIONS_RPM_RELEASE
# =============================================================================

# =============================================================================
# _RPM_SPEC_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP], [dnl
    _RPM_SPEC_SETUP_DIST
    _RPM_SPEC_SETUP_TOPDIR
    _RPM_SPEC_SETUP_OPTIONS
    _RPM_SPEC_SETUP_BUILD
])# _RPM_SPEC_SETUP
# =============================================================================

# =============================================================================
# _RPM_SPEC_SETUP_DIST
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_DIST], [dnl
    AC_ARG_WITH([rpm-extra],
	[AS_HELP_STRING([--with-rpm-extra=EXTRA],
	    [EXTRA release string for RPMS @<:@default=auto@:>@])], [], [dnl
	    if test -r .rpmextra ; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.rpmextra
	    then with_rpm_extra="`cat ${d}.rpmextra`"
	    else with_rpm_extra=""
	    fi])
    AC_CACHE_CHECK([for rpm distribution extra release string], [rpm_cv_dist_extra], [dnl
	case :${with_rpm_extra:-auto} in
	    (:no)
		rpm_cv_dist_extra=
		;;
	    (:auto)
		rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\..*||g'`
		case "$dist_cv_host_flavor" in
		    (oracle)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?|5|5.?|6|6.?)
					rpm_cv_dist_extra=".uek${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".uek${dist_cv_host_release}"   ;;
			esac
			;;
		    (puias)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?|5|5.?|6|6.?)
					rpm_cv_dist_extra=".puias${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".puias${dist_cv_host_release}"   ;;
			esac
			;;
		    (centos)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?|5|5.?|6|6.?)
					rpm_cv_dist_extra=".centos${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".centos${dist_cv_host_release}"  ;;
			esac
			;;
		    (lineox)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?|5|5.?|6|6.?)
					rpm_cv_dist_extra=".lel${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".lel${dist_cv_host_release}"	    ;;
			esac
			;;
		    (whitebox)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?|5|5.?|6|6.?)
					rpm_cv_dist_extra=".WB${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".WB${dist_cv_host_release}"	    ;;
			esac
			;;
		    (scientific)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?|5|5.?|6|6.?)
					rpm_cv_dist_extra=".sl${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".sl${dist_cv_host_release}"	    ;;
			esac
			;;
		    (fedora)
			case $dist_cv_host_release in
			    ([1-6])	rpm_cv_dist_extra=".FC${rpm_tmp}"		    ;;
			    ([7-9]|[1-9][0-9])
			    		rpm_cv_dist_extra=".fc${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".fc${dist_cv_host_release}"	    ;;
			esac
			;;
		    (redhat)
			case $dist_cv_host_release in
			    (7.0)	rpm_cv_dist_extra=".7.0"			    ;;
			    (7.1)	rpm_cv_dist_extra=".7.1"			    ;;
			    (7.2)	rpm_cv_dist_extra=".7.2"			    ;;
			    (7.3)	rpm_cv_dist_extra=".7.3"			    ;;
			    (8.0)	rpm_cv_dist_extra=".8.0"			    ;;
			    (9)		rpm_cv_dist_extra=".9"				    ;;
			    (2|2.?)	rpm_cv_dist_extra=".EL"				    ;;
			    (3|3.?)	rpm_cv_dist_extra=".E3"				    ;;
			    (4|4.?)	rpm_cv_dist_extra=".EL4"			    ;;
			    (5|5.?|6|6.?)
					rpm_cv_dist_extra=".el${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra2=".el${dist_cv_host_release}"	    ;;
			esac
			;;
		    (rhel)
			case $dist_cv_host_release in
			    (2|2.?)	rpm_cv_dist_extra=".EL"				    ;;
			    (3|3.?)	rpm_cv_dist_extra=".E3"				    ;;
			    (4|4.?)	rpm_cv_dist_extra=".EL4"			    ;;
			    (5|5.?|6|6.?)
					rpm_cv_dist_extra=".el${rpm_tmp}"		    ;;
			    (*)		rpm_cv_dist_extra=".el${dist_cv_host_release}"	    ;;
			esac
			;;
		    (mandrake)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra=".${rpm_tmp}mdk"
			;;
		    (mandriva)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra=".${rpm_tmp}mdv"
			;;
		    (manbo)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra=".${rpm_tmp}mnb"
			;;
		    (mageia)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra=".${rpm_tmp}mga"
			;;
		    (mes)
			case $dist_cv_host_release in
			    (5|5.?)	rpm_cv_dist_extra="mdvmes${dist_cv_host_release}"   ;;
			    (*)		rpm_cv_dist_extra="mdvmes${dist_cv_host_release}"   ;;
			esac
			;;
		    (suse)
			rpm_cv_dist_extra=".${dist_cv_host_release:-SuSE}"
			;;
		    (sle)
			rpm_cv_dist_extra=".${dist_cv_host_release:-SLE}"
			;;
		    (sles)
			rpm_cv_dist_extra=".${dist_cv_host_release:-SLES}"
			;;
		    (sled)
			rpm_cv_dist_extra=".${dist_cv_host_release:-SLED}"
			;;
		    (debian)
			rpm_cv_dist_extra=".deb${dist_cv_host_release}"
			;;
		    (ubuntu)
			rpm_cv_dist_extra=".ubuntu${dist_cv_host_release}"
			;;
		    (lts)
			rpm_cv_dist_extra=".lts${dist_cv_host_release}"
			;;
		esac
		;;
	    (*)
		rpm_cv_dist_extra="$with_rpm_extra"
		;;
	esac
    ])
    AC_CACHE_CHECK([for rpm distribution extra release string2], [rpm_cv_dist_extra2], [dnl
	case :${with_rpm_extra:-auto} in
	    (:no)
		rpm_cv_dist_extra2=
		;;
	    (:auto)
		rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\..*||g'`
		case "$dist_cv_host_flavor" in
		    (oracle)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".UEK${dist_cv_host_release}"  ;;
			esac
			;;
		    (puias)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".PUIAS${dist_cv_host_release}"  ;;
			esac
			;;
		    (centos)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".COS${dist_cv_host_release}"  ;;
			esac
			;;
		    (lineox)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".LEL${dist_cv_host_release}"  ;;
			esac
			;;
		    (whitebox)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".WB${dist_cv_host_release}"   ;;
			esac
			;;
		    (scientific)
			case $dist_cv_host_release in
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".SL${dist_cv_host_release}"  ;;
			esac
			;;
		    (fedora)
			case $dist_cv_host_release in
			    ([1-6])	rpm_cv_dist_extra2=".FC${dist_cv_host_release}"	    ;;
			    (*)		rpm_cv_dist_extra2=".fc${dist_cv_host_release}"	    ;;
			esac
			;;
		    (redhat)
			case $dist_cv_host_release in
			    (7.[[0-3]])	  rpm_cv_dist_extra2=".7.x"			    ;;
			    (8.0)	  rpm_cv_dist_extra2=".8"			    ;;
			    (9)		  rpm_cv_dist_extra2=".9"			    ;;
			    (2|2.?)	  rpm_cv_dist_extra2=".EL"			    ;;
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			esac
			;;
		    (rhel)
			case $dist_cv_host_release in
			    (2|2.?)	  rpm_cv_dist_extra2=".EL"			    ;;
			    (3|3.?|4|4.?) rpm_cv_dist_extra2=".EL${rpm_tmp}"		    ;;
			    (5|5.?|6|6.?) rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			    (*)		  rpm_cv_dist_extra2=".el${rpm_tmp}"		    ;;
			esac
			;;
		    (mandrake)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra2=".${rpm_tmp}mdk"
			;;
		    (mandriva)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra2=".${rpm_tmp}mdv"
			;;
		    (manbo)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra2=".${rpm_tmp}mnb"
			;;
		    (mageia)
			rpm_tmp=`echo "$dist_cv_host_release" | sed 's|\.||g'`
			rpm_cv_dist_extra2=".${rpm_tmp}mga"
			;;
		    (mes)
			case $dist_cv_host_release in
			    (5|5.?)	rpm_cv_dist_extra2="mdvmes${dist_cv_host_release}"  ;;
			    (*)		rpm_cv_dist_extra2="mdvmes${dist_cv_host_release}"  ;;
			esac
			;;
		    (suse)
			rpm_cv_dist_extra2=".${dist_cv_host_release:-SuSE}"
			;;
		    (sle)
			rpm_cv_dist_extra2=".${dist_cv_host_release:-SLE}"
			;;
		    (sles)
			rpm_cv_dist_extra2=".${dist_cv_host_release:-SLES}"
			;;
		    (sled)
			rpm_cv_dist_extra2=".${dist_cv_host_release:-SLED}"
			;;
		    (debian)
			rpm_cv_dist_extra2=".deb${dist_cv_host_release}"
			;;
		    (ubuntu)
			rpm_cv_dist_extra2=".ubuntu${dist_cv_host_release}"
			;;
		    (lts)
			rpm_cv_dist_extra2=".lts${dist_cv_host_release}"
			;;
		esac
		;;
	    (*)
		rpm_cv_dist_extra2="$with_rpm_extra"
		;;
	esac
    ])
    AC_CACHE_CHECK([for rpm distribution default topdir], [rpm_cv_dist_topdir], [dnl
	case "$dist_cv_host_distro" in
	    (oracle|puias|centos|lineox|whitebox|scientific|fedora|redhat|rhel)
		rpm_cv_dist_topdir='/usr/src/redhat'	;;
	    (suse)
		rpm_cv_dist_topdir='/usr/src/SuSE'	;;
	    (sle)
		rpm_cv_dist_topdir='/usr/src/packages'	;;
	    (mandrake|mandriva|manbo|mageia|mes)
		rpm_cv_dist_topdir='/usr/src/RPM'	;;
	    (debian|ubuntu|lts)
		rpm_cv_dist_topdir='/usr/src/rpm'	;;
	    (*)
		rpm_cv_dist_topdir="$ac_abs_top_buiddir" ;;
	esac
    ])
    PACKAGE_RPMDIST="${dist_cv_host_distrib:-Unknown Linux} ${dist_cv_host_release:-Unknown}${dist_cv_host_codename:+ ($dist_cv_host_codename)}"
    AC_SUBST([PACKAGE_RPMDIST])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_RPMDIST], ["$PACKAGE_RPMDIST"], [The RPM Distribution.  This
	defaults to automatic detection.])
    PACKAGE_RPMEXTRA="${rpm_cv_dist_extra}"
    PACKAGE_RPMEXTRA2="${rpm_cv_dist_extra2}"
    AC_SUBST([PACKAGE_RPMEXTRA])dnl
    AC_SUBST([PACKAGE_RPMEXTRA2])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_RPMEXTRA], ["$PACKAGE_RPMEXTRA"], [The RPM Extra Release string.
	This defaults to automatic detection.])
    AC_DEFINE_UNQUOTED([PACKAGE_RPMEXTRA2], ["$PACKAGE_RPMEXTRA2"], [The RPM Extra Release string.
	This defaults to automatic detection.])
])# _RPM_SPEC_SETUP_DIST
# =============================================================================

# =============================================================================
# _RPM_SPEC_SETUP_TOPDIR
# -----------------------------------------------------------------------------
# This is setting up the RPM topdir and the SOURCES BUILD RPMS SRPMS and SPECS
# directories.  We override rpm settings to acheive several objectives:  We do
# not want to move the tarballs (sources) from the tarball distribution
# directory, but use them in place.  Therefore we want to override the SOURCES
# and SPECS directories.  Because topdir is on an NFS mount when we build, but
# we want to speed builds as much as possible, we override the BUILD directory
# to be in our autoconf build directory which should be on a local filesystem.
# The same is true of the BUILDROOT directory (where files are temporarily
# installed.) The remaining two, RPMS and SRPMS are left in the distribution
# directory under topdir.  The am/rpm.am makefile fragment will override rpm
# macros with these values.  Note that the names stay nicely out of the way of
# autoconf directory names, but all nicely end in dir so we will define them
# in the same way.  (Yes, autoconf defines `builddir' even though it is always
# `.').
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_TOPDIR], [dnl
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_DISTDIR])
    AC_ARG_WITH([rpm-distdir],
	[AS_HELP_STRING([--with-rpm-distdir=DIR],
	    [rpm dist directory @<:@default=PKG-DISTDIR/rpms/PKG-SUBDIR@:>@])],
	[], [with_rpm_distdir='$(DISTDIR)/rpms/$(reposubdir)'])
    AC_MSG_CHECKING([for rpm distribution directory])
    if test ":${rpmdistdir+set}" != :set ; then
	case ":${with_rpm_distdir:-no}" in
	    (:no|:yes)	rpmdistdir='$(DISTDIR)/rpms/$(reposubdir)' ;;
	    (*)		rpmdistdir="$with_rpm_distdir" ;;
	esac
    fi
    AC_MSG_RESULT([$rpmdistdir])
    AC_SUBST([rpmdistdir])dnl
    AC_ARG_WITH([rpm-topdir],
	[AS_HELP_STRING([--with-rpm-topdir=DIR],
	    [rpm top directory @<:@default=RPM-DISTDIR/BRANCH@:>@])],
	[], [with_rpm_topdir='$(rpmdistdir)$(repobranch)'])
    AC_MSG_CHECKING([for rpm top build directory])
    if test ":${topdir+set}" != :set ; then
	case ":${with_rpm_topdir:-no}" in
	    (:no|:yes)	topdir='$(rpmdistdir)$(repobranch)' ;;
	    (*)		topdir="$with_rpm_topdir" ;;
	esac
    fi
    AC_MSG_RESULT([$topdir])
    AC_SUBST([topdir])dnl
    topfulldir='$(topdir)'
    AC_SUBST([topfulldir])dnl
    topmaindir='$(topdir)/main'
    AC_SUBST([topmaindir])dnl
    topdebgdir='$(topdir)/debug'
    AC_SUBST([topdebgdir])dnl
    topdevldir='$(topdir)/devel'
    AC_SUBST([topdevldir])dnl
    topsrcsdir='$(topdir)/source'
    AC_SUBST([topsrcsdir])dnl
    # set defaults for the rest
    AC_REQUIRE([_OPENSS7_OPTIONS_PKG_TARDIR])
    AC_MSG_CHECKING([for rpm SOURCES directory])
    if test ":${sourcedir+set}" != :set ; then
	sourcedir='$(tardir)'
    fi
    AC_MSG_RESULT([$sourcedir])
    AC_SUBST([sourcedir])dnl
    AC_MSG_CHECKING([for rpm BUILD directory])
    if test ":${rpmbuilddir+set}" != :set ; then
	# rpmbuilddir needs to be absolute: always build in the top build
	# directory on the local machine
	rpmbuilddir=`pwd`/BUILD
    fi
    AC_MSG_RESULT([$rpmbuilddir])
    AC_SUBST([rpmbuilddir])dnl
    AC_MSG_CHECKING([for rpm BUILDROOT directory])
    if test ":${rpmbuildrootdir+set}" != :set ; then
	# rmpbuildrootdir needs to be absolute: always install in the top
	# build directory on the local machine
	rpmbuildrootdir=`pwd`/BUILDROOT
    fi
    AC_MSG_RESULT([$repbuildrootdir])
    AC_SUBST([rpmbuildrootdir])dnl
    AC_MSG_CHECKING([for rpm RPMS directory])
    if test ":${rpmdir+set}" != :set ; then
	rpmdir='$(topdir)/RPMS'
    fi
    AC_MSG_RESULT([$rpmdir])
    AC_SUBST([rpmdir])dnl
    AC_MSG_CHECKING([for rpm SRPMS directory])
    if test ":${srcrpmdir+set}" != :set ; then
	srcrpmdir='$(DISTDIR)/rpms/SRPMS'
    fi
    AC_MSG_RESULT([$srcrpmdir])
    AC_SUBST([srcrpmdir])dnl
    AC_MSG_CHECKING([for rpm SPECS directory])
    if test ":${specdir+set}" != :set ; then
	specdir='$(DISTDIR)/rpms/SPECS'
    fi
    AC_MSG_RESULT([$specdir])
    AC_SUBST([specdir])dnl
])# _RPM_SPEC_SETUP_TOPDIR
# =============================================================================

# =============================================================================
# _RPM_SPEC_SETUP_OPTIONS
# -----------------------------------------------------------------------------
# Older rpms (particularly those used by SuSE) rpms are too stupid to handle
# --with and --without rpmpopt syntax, so convert to the equivalent --define
# syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
# properly, so we use defines.
AC_DEFUN([_RPM_SPEC_SETUP_OPTIONS], [dnl
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
		AC_MSG_CHECKING([for rpm argument '$arg'])
		if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
		    nam=`echo $arg | sed 's|[[= ]].*$||;s|--enable|--with|;s|--disable|--without|;s|-|_|g;s|^__|_|'`
		    arg="--define \"${nam} ${arg}\""
		    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }$arg"
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
	AC_MSG_CHECKING([for rpm argument '$arg'])
	if (echo $arg | egrep '^(--enable|--disable|--with|--without)' >/dev/null 2>&1) ; then
	    nam=`echo $arg | sed 's|[[= ]].*$||;s|--enable|--with|;s|--disable|--without|;s|-|_|g;s|^__|_|'`
	    arg="--define \"${nam} ${arg}\""
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }$arg"
	    AC_MSG_RESULT([yes])
	else
	    :
	    AC_MSG_RESULT([no])
	fi
    fi
    AC_SUBST([PACKAGE_RPMOPTIONS])dnl
])# _RPM_SPEC_SETUP_OPTIONS
# =============================================================================

# =============================================================================
# _RPM_SPEC_SETUP_BUILD
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_SETUP_BUILD], [dnl
dnl
dnl These environment variables are precious.
dnl
dnl AC_ARG_VAR([RPM_SOURCE_DIR], [RPM source directory.])
dnl AC_ARG_VAR([RPM_BUILD_DIR], [RPM build directory.])
dnl AC_ARG_VAR([RPM_OPT_FLAGS], [RPM optimization cflags.])
dnl AC_ARG_VAR([RPM_ARCH], [RPM architecture.])
dnl AC_ARG_VAR([RPM_OS], [RPM operating system.])
dnl AC_ARG_VAR([RPM_DOC_DIR], [RPM documentation directory.])
dnl AC_ARG_VAR([RPM_PACKAGE_NAME], [RPM package name.])
dnl AC_ARG_VAR([RPM_PACKAGE_VERSION], [RPM package version.])
dnl AC_ARG_VAR([RPM_PACKAGE_RELEASE], [RPM package release.])
dnl AC_ARG_VAR([RPM_BUILD_ROOT], [RPM installation root.])
dnl AC_ARG_VAR([PKG_CONFIG_PATH], [RPM configuration path.])
dnl
dnl These commands are needed to perform RPM package builds.
dnl
dnl Note that even if one could coax a debian system to build
dnl srpms and rpms, they would not be useful.  Usually rpm tools
dnl are just there for examining rpm packages.
dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([rpms],
	[AS_HELP_STRING([--disable-rpms],
	    [build rpms @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|lts|mint)	enable_rpms=no	;;
	    (*)				enable_rpms=yes	;;
	esac])
    AC_ARG_ENABLE([srpms],
	[AS_HELP_STRING([--disable-srpms],
	    [build srpms @<:@default=auto@:>@])],
	[], [dnl
	case "$target_vendor" in
	    (debian|ubuntu|lts|mint)	enable_srpms=no	 ;;
	    (*)				enable_srpms=yes ;;
	esac])
    AC_ARG_VAR([RPM],
	       [Rpm command. @<:@default=rpm@:>@])
    _BLD_PATH_PROG([RPM], [rpm], [${am_missing3_run}rpm], [$tmp_path], [dnl
	if test ":$enable_rpms" = :yes; then
	    case "$target_vendor" in
		(oracle|puias|centos|lineox|whitebox|scientific|fedora|mandrake|mandriva|manbo|mageia|mes|redhat|suse)
		    AC_MSG_WARN([Cannot find rpm program in PATH.]) ;;
		(*) enable_rpms=no ;;
	    esac
	fi])
    AC_ARG_VAR([RPMBUILD],
	       [Rpm build command. @<:@default=rpmbuild@:>@])
    _BLD_PATH_PROG([RPMBUILD], [rpmbuild], [${am_missing3_run}rpmbuild], [$tmp_path], [dnl
	if test ":$enable_rpms" = :yes; then
	    RPMBUILD="$RPM"
	else
	    enable_srpms=no;
	fi])
dnl
dnl I add a test for the existence of /var/lib/rpm because debian has rpm commands
dnl but no rpm database and therefore cannot build rpm packages.  But it can build
dnl src.rpms.
dnl
    if test ! -d /var/lib/rpm; then
	enable_rpms=no
    fi
    AC_CACHE_CHECK([for rpm build rpms], [rpm_cv_rpms], [dnl
	rpm_cv_rpms=${enable_rpms:-no}
    ])
    AC_CACHE_CHECK([for rpm build srpms], [rpm_cv_srpms], [dnl
	rpm_cv_srpms=${enable_srpms:-no}
    ])
    AM_CONDITIONAL([BUILD_RPMS], [test ":$enable_rpms" = :yes])dnl
    AM_CONDITIONAL([BUILD_SRPMS], [test ":$enable_srpms" = :yes])dnl
])# _RPM_SPEC_SETUP_BUILD
# =============================================================================

# =============================================================================
# _RPM_REPO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_REPO_SETUP], [dnl
    _RPM_REPO_SETUP_YUM
    _RPM_REPO_SETUP_YAST
    _RPM_REPO_SETUP_URPMI
    _RPM_REPO_SETUP_APT
])# _RPM_REPO_SETUP
# =============================================================================

# =============================================================================
# _RPM_REPO_SETUP_YUM
# -----------------------------------------------------------------------------
# These commands are needed to create RPM (e.g. YUM) repositories.
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_REPO_SETUP_YUM], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-yum],
	[AS_HELP_STRING([--disable-repo-yum],
	    [yum repo construction @<:@default=auto@:>@])],
	[], [enable_repo_yum=yes])
    AC_ARG_VAR([CREATEREPO],
	       [Create repomd repository command. @<:@default=createrepo@:>@])
    _BLD_PATH_PROG([CREATEREPO], [createrepo], [${am_missing3_run}createrepo], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(redhat|rhel|whitebox|lineox|fedora|oracle|puias|centos|scientific)
		    _BLD_INSTALL_WARN([CREATEREPO], [
*** 
*** Configure could not find a suitable  tool for creating REPOMD
*** repositories.  This program is part of the 'createrepo' package on
*** RPM based distributions supporting REPOMD repositories.
*** ], [
*** On YUM  based distributions, try 'yum install createrepo'
*** On ZYPP based distributions, try 'zypper install createrepo'], [
*** 
*** Alteratively, you can reconfigure with --disable-repo-yum to disable
*** generation of REPOMD repositories.  Proceeding under the assumption
*** that --disable-repo-yum was specified.
*** ])
		    ;;
		(*)
		    AC_MSG_WARN([Cannot find createrepo program in PATH.])
		    ;;
	    esac
	fi
	enable_repo_yum=no])
    AC_ARG_VAR([MODIFYREPO],
	       [Modify repomd repository command. @<:@default=modifyrepo@:>@])
    _BLD_PATH_PROG([MODIFYREPO], [modifyrepo], [${am_missing3_run}modifyrepo], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(redhat|rhel|whitebox|lineox|fedora|oracle|puias|centos|scientific)
		    _BLD_INSTALL_WARN([MODIFYREPO], [
*** 
*** Configure could not find a suitable tool for modifying REPOMD
*** repositories.  This program is part of the 'modifyrepo' package on
*** RPM based distributions supporting REPOMD repositories.
*** ], [
*** On YUM  based distributions, try 'yum install createrepo'
*** On ZYPP based distributions, try 'zypper install createrepo'], [
*** 
*** Alteratively, you can reconfigure with --disable-repo-yum to disable
*** generation of REPOMD repositories.  Proceeding under the assumption
*** that --disable-repo-yum was specified.
*** ])
		    ;;
		(*)
		    AC_MSG_WARN([Cannot find modifyrepo program in PATH.])
		    ;;
	    esac
	fi])
    AC_CACHE_CHECK([for rpm yum repo construction], [rpm_cv_repo_yum], [dnl
	rpm_cv_repo_yum=${enable_repo_yum:-no}
    ])
    AM_CONDITIONAL([BUILD_REPO_YUM], [test ":$rpm_cv_repo_yum" = :yes])dnl
    repodir='$(rpmdistdir)/repodata'
    AC_SUBST([repodir])dnl
    repofulldir='$(topfulldir)/repodata'
    AC_SUBST([repofulldir])dnl
    repomaindir='$(topmaindir)/repodata'
    AC_SUBST([repomaindir])dnl
    repodebgdir='$(topdebgdir)/repodata'
    AC_SUBST([repodebgdir])dnl
    repodevldir='$(topdevldir)/repodata'
    AC_SUBST([repodevldir])dnl
    reposrcsdir='$(topsrcsdir)/repodata'
    AC_SUBST([reposrcsdir])dnl
])# _RPM_REPO_SETUP_YUM
# =============================================================================

# =============================================================================
# _RPM_REPO_SETUP_YAST
# -----------------------------------------------------------------------------
# These commands are needed to create YAST repositories.
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_REPO_SETUP_YAST], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-yast],
	[AS_HELP_STRING([--disable-repo-yast],
	    [yast repo construction @<:@default=auto@:>@])],
	[], [enable_repo_yast=yes])
    AC_ARG_VAR([CREATE_PACKAGE_DESCR],
	       [Create YaST package descriptions command.  @<:@default=create_package_descr@:>@])
    _BLD_PATH_PROG([CREATE_PACKAGE_DESCR], [create_package_descr], [${am_missing3_run}create_package_descr], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(suse|sle|sles|sled|opensuse)
		    _BLD_INSTALL_WARN([CREATE_PACKAGE_DESCR], [
*** 
*** Configure could not find a suitable tool for creating YaST package
*** descriptions.  This program is part of the inst-source-utils package
*** on SuSE Linux based distributions.
*** ], [
*** On SuSE based distributions, try 'zypper install inst-source-utils'.
***
*** SLES 10/11: 'zypper install inst-source-utils'
*** OpenSuSE:   'zypper install inst-source-utils'], [
*** 
*** Alternatively, you can reconfigured with --disable-repo-yast to
*** disable generation of YaST repositories.  Proceeding under the
*** assumption that --disable-repo-yast was specified.
*** ])
		    ;;
		(*)
		    AC_MSG_WARN([Cannot find 'create_package_descr' program in PATH.])
		    ;;
	    esac
	fi
	enable_repo_yast=no])
    AC_CACHE_CHECK([for rpm yast repo construction], [rpm_cv_repo_yast], [dnl
	rpm_cv_repo_yast=${enable_repo_yast:-no}
    ])
    AM_CONDITIONAL([BUILD_REPO_YAST], [test ":$rpm_cv_repo_yast" = :yes])dnl
    yastdir="$topdir"
    AC_SUBST([yastdir])dnl
])# _RPM_REPO_SETUP_YAST
# =============================================================================

# =============================================================================
# _RPM_REPO_SETUP_URPMI
# -----------------------------------------------------------------------------
# These commands are needed to create URPMI repositories
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_REPO_SETUP_URPMI], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-urpmi],
	[AS_HELP_STRING([--disable-repo-urpmi],
	    [urpmi repo construction @<:@default=auto@:>@])],
	[], [enable_repo_urpmi=yes])
    AC_ARG_VAR([GENDISTRIB],
	       [Generate urpmi distribution command. @<:@default=gendistrib@:>@])
    _BLD_PATH_PROG([GENDISTRIB], [gendistrib], [${am_missing3_run}gendistrib], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    AC_MSG_WARN([Cannot find 'gendistrib' program in PATH.])
	fi])
    AC_ARG_VAR([GENHDLIST],
	       [Generate urpmi repository command. @<:@default=genhdlist@:>@])
    _BLD_PATH_PROGS([GENHDLIST], [genhdlist2 genhdlist], [${am_missing3_run}genhdlist2], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(mandrake|mandriva|manbo|mageia|mes)
		    _BLD_INSTALL_WARN([GENHDLIST], [
*** 
*** Configure could not find a suitable tool for generating the hdlists
*** for a URPMI repository.  This program is part of the rpm-tools
*** package on MandrivaLinux based distributions.
*** ], [
*** On Mandriva based distributions, try 'urpmi rpm-tools'.
***
*** Mandriva 2010.2: 'urpmi rpm-tools'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-urpmi to
*** disable generation of URPMI repositories.  Proceeding under the
*** assumption that --disable-repo-urpmi was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'genhdlist' program in PATH.])
		    ;;
	    esac
	fi
	enable_repo_urpmi=no])
    AC_CACHE_CHECK([for rpm urpmi repo construction], [rpm_cv_repo_urpmi], [dnl
	rpm_cv_repo_urpmi=${enable_repo_urpmi:-no}
    ])
    AM_CONDITIONAL([BUILD_REPO_URPMI], [test ":$rpm_cv_repo_urpmi" = :yes])dnl
    mediadir='$(rpmdistdir)/media/media_info'
    AC_SUBST([mediadir])dnl
    mediafulldir='$(rpmdistdir)/media$(repobranch)/media_info'
    AC_SUBST([mediafulldir])dnl
    mediamaindir='$(rpmdistdir)/media$(repobranch)/main/media_info'
    AC_SUBST([mediamaindir])dnl
    mediadebgdir='$(rpmdistdir)/media$(repobranch)/debug/media_info'
    AC_SUBST([mediadebgdir])dnl
    mediadevldir='$(rpmdistdir)/media$(repobranch)/devel/media_info'
    AC_SUBST([mediadevldir])dnl
    mediasrcsdir='$(rpmdistdir)/media$(repobranch)/source/media_info'
    AC_SUBST([mediasrcsdir])dnl
])# _RPM_REPO_SETUP_URPMI
# =============================================================================

# =============================================================================
# _RPM_REPO_SETUP_APT
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_REPO_SETUP_APT], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_ENABLE([repo-apt-rpm],
	[AS_HELP_STRING([--disable-repo-apt-rpm],
	    [apt-rpm repo construction @<:@defulat=auto@:>@])],
	[], [enable_repo_apt_rpm=yes])
dnl 
dnl genbasedir is not really necessary: it is a script and our makefiles perform the functions
dnl themselves.
dnl
    AC_ARG_VAR([GENBASEDIR],
	       [Generate apt-rpm repository command. @<:@default=genbasedir@:>@])
    _BLD_PATH_PROGS([GENBASEDIR], [genbasedir], [${am_missing3_run}genbasedir], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    AC_MSG_WARN([Cannot find 'genbasedir' program in PATH.])
	fi])
    AC_ARG_VAR([GENSRCLIST],
	       [Generate apt-rpm repository sources command. @<:@default=gensrclist@:>@])
    _BLD_PATH_PROGS([GENSRCLIST], [gensrclist], [${am_missing3_run}gensrclist], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(mandrake|mandriva|mes|mageia)
		    _BLD_INSTALL_WARN([GENSRCLIST], [
*** 
*** Configure could not find a suitable tool for creating APT for RPM
*** source repository metadata.  This program is part of the 'apt'
*** package on Mandriva based distributions.
*** ], [
*** On Mandriva based distributions, try 'urpmi apt'.
***
*** Mageia 1:	'urpmi apt'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-apt-rpm to
*** disable generation of APT for RPM repositories.  Proceeding under
*** the assumption that --disable-repo-apt-rpm was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'gensrclist' program in PATH.]) ;;
	    esac
	fi
	enable_repo_apt_rpm=no])
    AC_ARG_VAR([GENPKGLIST],
	       [Generate apt-rpm repository packages command.  @<:@default=genpkglist@:>@])
    _BLD_PATH_PROGS([GENPKGLIST], [genpkglist], [${am_missing3_run}genpkglist], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    case "${target_vendor:-none}" in
		(mandrake|mandriva|mes|mageia)
		    _BLD_INSTALL_WARN([GENPKGLIST], [
*** 
*** Configure could not find a suitable tool for creating APT for RPM
*** package repository metadata.  This program is part of the 'apt'
*** package on Mandriva based distributions.
*** ], [
*** On Mandriva based distributions, try 'urmpi apt'.
***
*** Mageia 1:	'urpmi apt'], [
*** 
*** Alternatively, you can reconfigure with --disable-repo-apt-rpm to
*** disable generation of APT for RPM repositories.  Proceeding under
*** the assumption that --disable-repo-apt-rpm was specified.
*** ])
		    ;;
		(*) AC_MSG_WARN([Cannot find 'genpkglist' program in PATH.]) ;;
	    esac
	fi
	enable_repo_apt_rpm=no])
dnl
dnl countpkglist is not really necessary: it is a binary, but it is easy to emulate with strings,
dnl grep and wc.
dnl
    AC_ARG_VAR([COUNTPKGLIST],
	       [Count packages in apt-rpm package lists command. @<:@default=countpkglist@:>@])
    _BLD_PATH_PROGS([COUNTPKGLIST], [countpkglist], [${am_missing3_run}countpkglist], [$tmp_path], [dnl
	if test ":$rpm_cv_rpms" = :yes -a ${USE_MAINTAINER_MODE:-no} = yes ; then
	    AC_MSG_WARN([Cannot find 'countpkglist' program in PATH.])
	fi])
    AC_CACHE_CHECK([for rpm apt repo construction], [rpm_cv_repo_apt], [dnl
	rpm_cv_repo_apt=${enable_repo_apt_rpm:-no}
    ])
    AM_CONDITIONAL([BUILD_REPO_APT_RPM], [test ":$rpm_cv_repo_apt" = :yes])dnl
    aptrdir='$(topdir)'
    AC_SUBST([aptrdir])dnl
    aptrbasedir='$(aptrdir)'/base
    AC_SUBST([aptrbasedir])dnl
    aptrfulldir='$(aptrdir)/RPMS.full'
    AC_SUBST([aptrfulldir])dnl
    aptrmaindir='$(aptrdir)/RPMS.main'
    AC_SUBST([aptrmaindir])dnl
    aptrdebgdir='$(aptrdir)/RPMS.debug'
    AC_SUBST([aptrdebgdir])dnl
    aptrdevldir='$(aptrdir)/RPMS.devel'
    AC_SUBST([aptrdevldir])dnl
    aptrsrcsdir='$(aptrdir)/RPMS.source'
    AC_SUBST([aptrsrcsdir])dnl
    aptrsrpmdir='$(aptrdir)/SRPMS.source'
    AC_SUBST([aptrsrpmdir])dnl
])# _RPM_REPO_SETUP_APT
# =============================================================================

# =============================================================================
# _RPM_SPEC_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_SPEC_OUTPUT], [dnl
    AC_CONFIG_FILES([scripts/speccommon
		     scripts/product.xml
		     scripts/product-extras.xml
		     scripts/product-updates.xml
		     scripts/products.xml
		     scripts/products-extras.xml
		     scripts/products-updates.xml
		     scripts/patterns.xml])
    speccommon="scripts/speccommon"
    AC_SUBST_FILE([speccommon])
    AC_CONFIG_FILES(m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME]).spec)
    AC_CONFIG_FILES(m4_ifdef([AC_PACKAGE_TARNAME],[AC_PACKAGE_TARNAME]).lsm)
    if test ":${enable_public:-yes}" != :yes ; then
	PACKAGE="${PACKAGE_TARNAME}"
	VERSION="bin-${PACKAGE_VERSION}.${PACKAGE_RELEASE}${PACKAGE_PATCHLEVEL}"
    else
	PACKAGE="${PACKAGE_TARNAME}"
	VERSION="${PACKAGE_VERSION}.${PACKAGE_RELEASE}${PACKAGE_PATCHLEVEL}"
    fi
])# _RPM_SPEC_OUTPUT
# =============================================================================

# =============================================================================
# _RPM_
# -----------------------------------------------------------------------------
AC_DEFUN([_RPM_], [dnl
])# _RPM_
# =============================================================================

# =============================================================================
#
# $Log: rpm.m4,v $
# Revision 1.1.2.15  2011-08-07 11:14:37  brian
# - mostly mandriva and ubuntu build updates
#
# Revision 1.1.2.14  2011-07-27 07:52:19  brian
# - work to support Mageia/Mandriva compressed kernel modules and URPMI repo
#
# Revision 1.1.2.13  2011-06-09 11:30:54  brian
# - support mageia and mes
#
# Revision 1.1.2.12  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.11  2011-03-17 07:01:28  brian
# - build and repo system improvements
#
# Revision 1.1.2.10  2011-03-06 08:57:20  brian
# - repository updates
#
# Revision 1.1.2.9  2011-02-28 19:51:30  brian
# - better repository build
#
# Revision 1.1.2.8  2011-02-17 18:34:10  brian
# - repository and rpm build updates
#
# Revision 1.1.2.7  2011-02-10 17:29:45  brian
# - repo updates
#
# Revision 1.1.2.6  2011-02-09 17:59:27  brian
# - repository and rpm updates for suse
#
# Revision 1.1.2.5  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.4  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.3  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.2  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.1  2009-06-21 11:06:05  brian
# - added files to new distro
#
# Revision 0.9.2.84  2008-10-31 11:58:28  brian
# - more distros
#
# Revision 0.9.2.83  2008-10-27 12:23:34  brian
# - suppress warning on each iteration and cache results
#
# Revision 0.9.2.82  2008-09-28 06:50:23  brian
# - change order of spec files
#
# Revision 0.9.2.81  2008-09-28 05:52:34  brian
# - add subsitution of common spec file
#
# Revision 0.9.2.80  2008/09/22 17:57:38  brian
# - substitute rpm subdirectory
#
# Revision 0.9.2.79  2008/09/21 14:09:33  brian
# - correction
#
# Revision 0.9.2.78  2008-09-21 07:10:07  brian
# - environment passed to rpm and dpkg cannot be precious
#
# Revision 0.9.2.77  2008/09/20 23:50:05  brian
# - change fallback from rpmbuild to rpm
#
# Revision 0.9.2.76  2008-09-20 23:06:08  brian
# - corrections
#
# Revision 0.9.2.75  2008-09-20 12:15:24  brian
# - typo
#
# Revision 0.9.2.74  2008-09-20 11:17:14  brian
# - build system updates
#
# Revision 0.9.2.73  2008-09-19 06:19:38  brian
# - typo
#
# Revision 0.9.2.72  2008-09-19 05:18:45  brian
# - separate repo directory by architecture
#
# Revision 0.9.2.71  2008/09/18 09:11:01  brian
# - add more rpm stuff for debian
#
# Revision 0.9.2.70  2008-09-17 05:53:03  brian
# - place source in tarballs directory for remote build
#
# Revision 0.9.2.69  2008-09-16 09:47:47  brian
# - updates to rpmspec files
#
# Revision 0.9.2.68  2008-09-12 06:12:09  brian
# - correction
#
# Revision 0.9.2.67  2008-09-12 05:23:20  brian
# - add repo subdirectories
#
# Revision 0.9.2.66  2008/07/23 19:14:41  brian
# - indicates support to CentOS 5.2
#
# Revision 0.9.2.65  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.64  2008/04/27 22:28:39  brian
# - fix conditional error
#
# Revision 0.9.2.63  2007/10/17 20:00:28  brian
# - slightly different path checks
#
# Revision 0.9.2.62  2007/08/12 19:05:31  brian
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
