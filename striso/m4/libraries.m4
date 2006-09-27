# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: libraries.m4,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/09/27 02:56:45 $
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
# Last Modified $Date: 2006/09/27 02:56:45 $ by $Author: brian $
#
# =============================================================================


# =========================================================================
# _LDCONFIG
# -------------------------------------------------------------------------
AC_DEFUN([_LDCONFIG], [dnl
    _LDCONFIG_SPEC_OPTIONS
    _LDCONFIG_SPEC_SETUP
    _LDCONFIG_SPEC_OUTPUT
])# _LDCONFIG
# =========================================================================

# =========================================================================
# _LDCONFIG_SPEC_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LDCONFIG_SPEC_OPTIONS], [dnl
])# _LDCONFIG_SPEC_OPTIONS
# =========================================================================

# =========================================================================
# _LDCONFIG_SPEC_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_LDCONFIG_SPEC_SETUP], [dnl
    AC_ARG_VAR([LDCONFIG], [Configure loader command])
    AC_PATH_TOOL([LDCONFIG], [ldconfig], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${LDCONFIG:-no}" = :no ; then
	AC_MSG_WARN([Could not find ldconfig program in PATH.])
	LDCONFIG=/sbin/ldconfig
    fi
])# _LDCONFIG_SPEC_SETUP
# =========================================================================

# =========================================================================
# _LDCONFIG_SPEC_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_LDCONFIG_SPEC_OUTPUT], [dnl
    lib_abs_builddir=`(cd . ; pwd)`
    lib_abs_srcdir=`(cd $srcdir ; pwd)`
    AC_SUBST([lib_asb_builddir])dnl
    AC_SUBST([lib_asb_srcdir])dnl
])# _LDCONFIG_SPEC_OUTPUT
# =========================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
