# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: libraries.m4,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-28 09:41:03 $
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
    AC_PATH_PROG([LDCONFIG], [ldconfig], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
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
    AC_SUBST([lib_abs_builddir])dnl
    AC_SUBST([lib_abs_srcdir])dnl
])# _LDCONFIG_SPEC_OUTPUT
# =========================================================================

# =============================================================================
#
# $Log: libraries.m4,v $
# Revision 0.9.2.15  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.14  2007/10/17 20:00:28  brian
# - slightly different path checks
#
# Revision 0.9.2.13  2007/08/12 19:05:31  brian
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
