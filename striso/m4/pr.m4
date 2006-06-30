# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: pr.m4,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/06/29 23:38:09 $
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
# Last Modified $Date: 2006/06/29 23:38:09 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: pr.m4,v $
# Revision 0.9.2.3  2006/06/29 23:38:09  brian
# - more variations for Ubuntu, which in most ways is much like Debian.
#
# Revision 0.9.2.2  2006/03/23 05:41:18  brian
# - renamed auto-pr to send-pr
#
# Revision 0.9.2.1  2006/03/21 13:23:45  brian
# - added problem reports
#
# =============================================================================

# =============================================================================
# _AUTOPR
# -----------------------------------------------------------------------------
AC_DEFUN([_AUTOPR], [dnl
    AC_REQUIRE([_DISTRO])
    _AUTOPR_SETUP
    _AUTOPR_OUTPUT
])# _AUTOPR
# =============================================================================

# =============================================================================
# _AUTOPR_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_AUTOPR_SETUP], [dnl
    AC_CACHE_CHECK([for send-pr distribution], [ap_cv_distribution], [dnl
	case "$dist_cv_host_flavor" in
	    (centos)	ap_cv_distribtuion="COS$dist_cv_host_release"	;;
	    (lineox)	ap_cv_distribution="LEL$dist_cv_host_release"	;;
	    (whitebox)	ap_cv_distribution="WBEL$dist_cv_host_release"	;;
	    (fedora)	ap_cv_distribution="FC$dist_cv_host_release"	;;
	    (redhat)
		case $dist_cv_host_release in
		    (6.2|7.[[0-3]]|8.0|9)
				ap_cv_distribution="RH$dist_cv_host_release" ;;
		    (2|2.?)	ap_cv_distribution="RHAS2" ;;
		    (3|3.0)	ap_cv_distribution="RHEL3" ;;
		    (4|4.0)	ap_cv_distribution="RHEL4" ;;
		    (5|5.0)	ap_cv_distribution="RHEL5" ;;
		esac ;;
	    (mandrake)	ap_cv_distribution="MDK$dist_cv_host_release"	;;
	    (suse)	ap_cv_distribution="SuSE$dist_cv_host_release"	;;
	    (debian)	ap_cv_distribution="Debian$dist_cv_host_release" ;;
	    (ubuntu)	ap_cv_distribution="Ubuntu$dist_cv_host_release" ;;
	    (montavista) ap_cv_distribtuion="MontaVista"		;;
	    (bluecat)	ap_cv_distribution="BlueCat"			;;
	    (yellowdog)	ap_cv_distribution="YellowDog"			;;
	    (denx)	ap_cv_distribution="ELDK"			;;
	    (*)		ap_cv_distribution='other'			;;
	esac
	])
    AP_DISTRIBUTION="$ap_cv_distribution"
    AC_SUBST([AP_DISTRIBUTION])
    AC_MSG_CHECKING([for send-pr script])
    AP_SCRIPT="$ac_aux_dir/send-pr"
    AC_MSG_RESULT([${AP_SCRIPT}])
    AC_SUBST([AP_SCRIPT])
    AC_MSG_CHECKING([for send-pr command])
    AP_AUTOPR="$SHELL $AP_SCRIPT"
    AC_MSG_RESULT([${AP_AUTOPR}])
    AC_SUBST([AP_AUTOPR])
    AC_MSG_CHECKING([for send-pr config])
    AP_CONFIG="scripts/send-pr.config"
    AC_MSG_RESULT([${AP_CONFIG}])
    AC_SUBST([AP_CONFIG])
    AC_CONFIG_FILES([scripts/send-pr.config])
])# _AUTOPR_SETUP
# =============================================================================

# =============================================================================
# _AUTOPR_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_AUTOPR_OUTPUT], [dnl
])# _AUTOPR_OUTPUT
# =============================================================================

# =============================================================================
# _AUTOPR_
# -----------------------------------------------------------------------------
AC_DEFUN([_AUTOPR_], [dnl
])# _AUTOPR_
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
