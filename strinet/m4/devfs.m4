# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/03/16 13:28:06 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Last Modified $Date: 2005/03/16 13:28:06 $ by $Author: brian $
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
    AC_CACHE_CHECK([for kernel devfs build], [linux_cv_devfs_build], [dnl
	case ":$enable_devfs" in
	    :yes)   linux_cv_devfs_build=yes ;;
	    :no)    linux_cv_devfs_build=no ;;
	    :)  if test :"${linux_cv_CONFIG_DEVFS_FS:-no}" = :yes -o :"${linux_cv_CONFIG_DEVFS_MOUNT:-no}" = :yes
		then linux_cv_devfs_build=yes
		else linux_cv_devfs_build=no
		fi ;;
	esac
    ])
    if test :"${linux_cv_devfs_build:-no}" = :no
    then
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_devfs --disable-devfs\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--disable-devfs'"
dnl	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--disable-devfs"
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_devfs --enable-devfs\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--enable-devfs'"
dnl	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--enable-devfs"
    fi
    AM_CONDITIONAL([WITH_DEVFS], [test :"${linux_cv_devfs_build:-no}" != :no])dnl
])# _LINUX_DEVFS
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
