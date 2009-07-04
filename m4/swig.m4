# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: swig.m4,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2009-07-04 03:51:33 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date: 2009-07-04 03:51:33 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _SWIG
# -----------------------------------------------------------------------------
AC_DEFUN([_SWIG], [dnl
    _SWIG_OPTIONS
    _SWIG_SETUP
    _SWIG_OUTPUT
])# _SWIG
# =============================================================================

# =============================================================================
# _SWIG_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_SWIG_OPTIONS], [dnl
])# _SWIG_OPTIONS
# =============================================================================

# =============================================================================
# _SWIG_SETUP
# -----------------------------------------------------------------------------
# Checks for the swig command.  This command is used to generate interface
# files for various languages like java, tcl, etc.  We distribute the
# secondary result of this command so that the building system does not need
# swig installed to work, only when building from CVS.  The missing script
# should simply touch the target files.
# -----------------------------------------------------------------------------
AC_DEFUN([_SWIG_SETUP], [dnl
    AC_ARG_VAR([SWIG],
	[Swig command. @<@default=swig@:>@])
    AC_PATH_PROG([SWIG], [swig], [],
	[$PATH:/usr/local/bin:/usr/bin:/bin])
    if test :"${SWIG:-no}" = :no ; then
	SWIG="${am_missing4_run}swig"
	AC_MSG_WARN([Could not find swig program in PATH.])
    fi
])# _SWIG_SETUP
# =============================================================================

# =============================================================================
# _SWIG_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SWIG_OUTPUT], [dnl
])# _SWIG_OUTPUT
# =============================================================================

# =============================================================================
# _SWIG_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_SWIG_XXX], [dnl
])# _SWIG_XXX
# =============================================================================

# =============================================================================
#
# $Log: swig.m4,v $
# Revision 1.1.2.3  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.2  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.1  2009-06-21 11:06:05  brian
# - added files to new distro
#
# =============================================================================
# 
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn

