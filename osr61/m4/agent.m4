# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: agent.m4,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/10/15 17:21:26 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/10/15 17:21:26 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# These macros are for testing whether to include SNMP agent software in the
# build and other agent-related checks.
# =============================================================================

# =============================================================================
# _AGENT
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT], [dnl
    AC_REQUIRE([_DISTRO])
    _AGENT_OPTIONS
    _AGENT_SETUP
    _AGENT_USER
    _AGENT_OUTPUT
])# _AGENT
# =============================================================================

# =============================================================================
# _AGENT_OPTIONS
# -----------------------------------------------------------------------------
# It is not necessary to compile the SNMP agents.  This option decides whether
# to do so or not.
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_OPTIONS], [dnl
    AC_ARG_WITH([snmp-agent],
	AS_HELP_STRING([--with-snmp-agent],
	    [include SNMP agents in build. @<:@default=yes@:>@]),
	[with_snmp_agent="$withval"],
	[with_snmp_agent=''])
])# _AGENT_OPTIONS
# =============================================================================

# =============================================================================
# _AGENT_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_SETUP], [dnl
    AC_MSG_CHECKING([for perl libraries])
    AC_MSG_RESULT([$with_snmp_agent])
    if test :"${with_snmp_agent:-yes}" = :yes ; then :;
	_AGENT_SETUP_PERL
    fi
    AC_MSG_CHECKING([for snmp libraries])
    AC_MSG_RESULT([$with_snmp_agent])
    if test :"${with_snmp_agent:-yes}" = :yes ; then :;
	_AGENT_SETUP_SNMP
    fi
])# _AGENT_SETUP
# =============================================================================

# =============================================================================
# _AGENT_SETUP_PERL
# -----------------------------------------------------------------------------
# Building SNMP agents requires the presence of perl libraries.
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_SETUP_PERL], [dnl
    _PERL
])# _AGENT_SETUP_PERL
# =============================================================================

# =============================================================================
# _AGENT_SETUP_SNMP
# -----------------------------------------------------------------------------
# Building SNMP agents requires the presence of snmp libraries.
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_SETUP_SNMP], [dnl
    _SNMP
])# _AGENT_SETUP_SNMP
# =============================================================================

# =============================================================================
# _AGENT_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_USER], [dnl
])# _AGENT_USER
# =============================================================================

# =============================================================================
# _AGENT_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_SNMP_AGENT], [test :"${with_snmp_agent:-yes}" = :yes])dnl
])# _AGENT_OUTPUT
# =============================================================================

# =============================================================================
# _AGENT_
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_], [dnl
])# _AGENT_
# =============================================================================

# =============================================================================
#
# $Log: agent.m4,v $
# Revision 0.9.2.2  2007/10/15 17:21:26  brian
# - SNMP updates
#
# Revision 0.9.2.1  2007/10/15 06:49:07  brian
# - added agent checks and mib header
#
# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
