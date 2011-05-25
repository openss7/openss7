# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: jar.m4,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-02-07 04:48:32 $
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
# Last Modified $Date: 2011-02-07 04:48:32 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _JAR
# -----------------------------------------------------------------------------
AC_DEFUN([_JAR], [dnl
    AC_MSG_NOTICE([+----------------------------+])
    AC_MSG_NOTICE([| Java JAR Generation Checks |])
    AC_MSG_NOTICE([+----------------------------+])
    _JAR_OPTIONS
    _JAR_SETUP
    _JAR_OUTPUT
])# _JAR
# =============================================================================

# =============================================================================
# _JAR_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_JAR_OPTIONS], [dnl
])# _JAR_OPTIONS
# =============================================================================

# =============================================================================
# _JAR_SETUP
# -----------------------------------------------------------------------------
# Checks for the jar or fastjar command.  This command is used to generate
# Java archives.  In general it is possible to generate simple java archives
# using a script and the zip command.
# -----------------------------------------------------------------------------
AC_DEFUN([_JAR_SETUP], [dnl
    AC_REQUIRE([_OPENSS7_MISSING4])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    AC_ARG_VAR([JAR],
	[Java archive command. @<:@default=fastjar,jar@:>@])
    _BLD_PATH_PROGS([JAR], [fastjar jar], [${am_missing4_run}jar], [$tmp_path])
    AC_ARG_VAR([ZIP],
	[Zip archive command. @<:@default=zip@:>@])
    _BLD_PATH_PROGS([ZIP], [zip], [${am_missing4_run}zip], [$tmp_path])
])# _JAR_SETUP
# =============================================================================

# =============================================================================
# _JAR_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_JAR_OUTPUT], [dnl
])# _JAR_OUTPUT
# =============================================================================

# =============================================================================
# _JAR_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_JAR_XXX], [dnl
])# _JAR_XXX
# =============================================================================


# =============================================================================
#
# $Log: jar.m4,v $
# Revision 1.1.2.4  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.3  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.2  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
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

