# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:*** fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: bcm.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2006-03-20 12:19:02 $
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
# Last Modified $Date: 2006-03-20 12:19:02 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: bcm.m4,v $
# Revision 0.9.2.1  2006-03-20 12:19:02  brian
# - attempt at BCM fragments
#
# =============================================================================

dnl
dnl These autoconf macros are used to find and generate automake variables for
dnl the am/bcm.am automake makefile fragment.
dnl

# =============================================================================
# _BCM
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM], [dnl
    _BCM_OPTIONS
    _BCM_SETUP
    _BCM_OUTPUT
])# _BCM
# =============================================================================

# =============================================================================
# _BCM_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_OPTIONS], [dnl
])# _BCM_OPTIONS
# =============================================================================

# =============================================================================
# _BCM_SETUP
# -----------------------------------------------------------------------------
# Use strconf-sh to determine which module and driver objects to configure.
# Source code file names must be derived directly from the object file name by
# simply replacing the .o with a .c.  The .o or .c file must be contained in
# the same directory as the Config file that specifies its object name.
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_SETUP], [dnl
    AC_REQUIRE([_STRCONF])
])# _BCM_SETUP
# =============================================================================

# =============================================================================
# _BCM_OUTPUT
# -----------------------------------------------------------------------------
# There are several automake variables that we need to substitute for the
# am/bcm.am makefile fragment:
# - KERNEL_SOURCES  : all of the 
# - KERNEL_OBJECTS  : all of the kernel objects that need to be built
# - KERNEL_MODULES  : all of the kenrel modules that need to be built
# - KERNEL_PRELOADS : all of the kernel modules that need to be preloaded
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_OUTPUT], [dnl
    AC_SUBST([PKG_SRCFILES])dnl all the source files that need to be built
    AC_SUBST([PKG_OBJFILES])dnl all the object files that need to be built
])# _BCM_OUTPUT
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:*** fo+=tcqlorn
