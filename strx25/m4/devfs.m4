# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: devfs.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.10 $) $Date: 2008-04-28 09:41:03 $
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
# Last Modified $Date: 2008-04-28 09:41:03 $ by $Author: brian $
#
# =============================================================================

# -----------------------------------------------------------------------------
# These are some things that need to be done different if we have a devfs.
# -----------------------------------------------------------------------------

# =============================================================================
# _LINUX_DEVFS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_DEVFS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])
    _LINUX_CHECK_KERNEL_CONFIG([], [CONFIG_DEVFS_FS])
    _LINUX_CHECK_KERNEL_CONFIG([], [CONFIG_DEVFS_MOUNT])
    AC_ARG_ENABLE([devfs],
	AS_HELP_STRING([--enable-devfs],
	    [build for the devfs. @<:@default=auto@:>@]))
    AC_CACHE_CHECK([for devfs kernel support], [devfs_cv_kernel_support], [dnl
	case ":$enable_devfs" in
	    (:yes)   devfs_cv_kernel_support=yes ;;
	    (:no)    devfs_cv_kernel_support=no ;;
	    (:)  if test :"${linux_cv_CONFIG_DEVFS_FS:-no}" = :yes -o :"${linux_cv_CONFIG_DEVFS_MOUNT:-no}" = :yes
		then devfs_cv_kernel_support=yes
		else devfs_cv_kernel_support=no
		fi ;;
	esac
    ])
    AC_CACHE_CHECK([for devfs modprobe entries], [devfs_cv_modprobe], [dnl
	devfs_cv_modprobe=no
	if test :"$linux_cv_k_ko_modules" = :yes
	then
	    eval "devfs_where=\"${DESTDIR}${sysconfdir}/modprobe.devfs\""
	else
	    eval "devfs_where=\"${DESTDIR}${sysconfdir}/modules.devfs\""
	fi
	if test -f "$devfs_where"
	then
	    devfs_cv_modprobe="yes (${devfs_where})"
	fi
    ])
    AC_CACHE_CHECK([for devfs daemon config], [devfs_cv_daemon_config], [dnl
	devfs_cv_daemon_config=no
	eval "devfs_where=\"${DESTDIR}${sysconfdir}/devfsd.conf\""
	if test -f "$devfs_where"
	then
	    devfs_cv_daemon_config="yes (${devfs_where})"
	fi
    ])
    AC_CACHE_CHECK([for devfs daemon], [devfs_cv_daemon], [dnl
	devfs_cv_daemon=no
	eval "devfs_where=\"${DESTDIR}${rootdir}/sbin/devfsd\""
	if test -x "$devfs_where"
	then
	    devfs_cv_daemon="yes (${devfs_where})"
	fi
    ])
    AC_CACHE_CHECK([for devfs build], [devfs_cv_build], [dnl
	devfs_cv_build=yes
	if test :"$devfs_cv_kernel_support" = :no
	then
	    devfs_cv_build=no
	fi
	if test :"$devfs_cv_modprobe" = :no
	then
	    devfs_cv_build=no
	fi
	if test :"$devfs_cv_daemon_config" = :no
	then
	    devfs_cv_build=no
	fi
	if test :"$devfs_cv_daemon" = :no
	then
	    devfs_cv_build=no
	fi
    ])
    if test :"${devfs_cv_build:-no}" = :no
    then
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_devfs --disable-devfs\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--disable-devfs'"
dnl	ac_configure_args="$ac_configure_args --disable-devfs"
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_devfs --enable-devfs\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--enable-devfs'"
dnl	ac_configure_args="$ac_configure_args --enable-devfs"
    fi
    AM_CONDITIONAL([WITH_DEVFS], [test :"${devfs_cv_build:-no}" != :no])dnl
])# _LINUX_DEVFS
# =============================================================================

# =============================================================================
#
# $Log: devfs.m4,v $
# Revision 0.9.2.10  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.9  2007/08/12 19:05:30  brian
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
