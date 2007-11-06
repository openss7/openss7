# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: agent.m4,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/10/18 05:33:30 $
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
# Last Modified $Date: 2007/10/18 05:33:30 $ by $Author: brian $
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
    if test :"${with_snmp_agent:-yes}" = :no ; then
	agent_cv_snmp_agent=no
    else
	agent_cv_snmp_agent=yes
    fi
    if test :"${agent_cv_snmp_agent:-yes}" = :yes ; then :;
	_AGENT_SETUP_PERL
    fi
    # Note that only fedora, redhat and other broken packaging of net-snmp
    # needs perl libraries.  The Debian 4.0 (Etch) packaging have proper load
    # dependencies between libraries, so loading the SNMP agent libraries
    # properly loads dependent libraries.
    AC_MSG_CHECKING([for agent perl libraries])
    AC_MSG_RESULT([$agent_cv_snmp_agent])
    if test :"${agent_cv_snmp_agent:-yes}" = :yes ; then :;
	_AGENT_SETUP_SNMP
    fi
    AC_MSG_CHECKING([for agent snmp libraries])
    AC_MSG_RESULT([$agent_cv_snmp_agent])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-config.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/ucd-snmp-config.h>.  This file could not be
*** found.  Perhaps you need to load the NET-SNMP development package
*** (net-snmp-dev or libsnmp9-dev).  Perhaps the file is simply mangled.
*** Repeat after loading the correct package or setting --without-snmp-agent.
*** ]) ])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-includes.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/ucd-snmp-includes.h>.  This file could not be
*** found.  Perhaps you need to load the NET-SNMP development package
*** (net-snmp-dev or libsnmp9-dev).  Perhaps the file is simply mangled.
*** Repeat after loading the correct package or setting --without-snmp-agent.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-agent-includes.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/ucd-snmp-agent-includes.h>.  This file could
*** not be found.  Perhaps you need to load the NET-SNMP development package
*** (net-snmp-dev or libsnmp9-dev).  Perhaps the file is simply mangled.
*** Repeat after loading the correct package or setting --without-snmp-agent.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/callback.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/callback.h>.  This file could not be found.
*** Perhaps you need to load the NET-SNMP development package (net-snmp-dev or
*** libsnmp9-dev).  Perhaps the file is simply mangled.  Repeat after loading
*** the correct package or setting --without-snmp-agent.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/snmp-tc.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/snmp-tc.h>.  This file could not be found.
*** Perhaps you need to load the NET-SNMP development package (net-snmp-dev or
*** libsnmp9-dev).  Perhaps the file is simply mangled.  Repeat after loading
*** the correct package or setting --without-snmp-agent.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
])
    AC_CHECK_HEADERS([ucd-snmp/default_store.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/default_store.h>.  This file could not be
*** found.  Perhaps you need to load the NET-SNMP development package
*** (net-snmp-dev or libsnmp9-dev).  Perhaps the file is simply mangled.
*** Repeat after loading the correct package or setting --without-snmp-agent.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
])
    AC_CHECK_HEADERS([ucd-snmp/snmp_alarm.h], [], [
	AC_MSG_WARN([
*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header files
*** and in particular <ucd-snmp/snmp_alarm.h>.  This file could not be found.
*** Perhaps you need to load the NET-SNMP development package (net-snmp-dev or
*** libsnmp9-dev).  Perhaps the file is simply mangled.  Repeat after loading
*** the correct package or setting --without-snmp-agent.
*** ]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
#include <ucd-snmp/default_store.h>
])
    AC_CHECK_HEADERS([ucd-snmp/ds_agent.h ucd-snmp/util_funcs.h ucd-snmp/header-complex.h ucd-snmp/mib_modules.h net-snmp/agent/mib_modules.h], [], [], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
#include <ucd-snmp/default_store.h>
#include <ucd-snmp/snmp_alarm.h>
])
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
    AM_CONDITIONAL([WITH_SNMP_AGENT], [test :"${agent_cv_snmp_agent:-yes}" = :yes])dnl
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
# Revision 0.9.2.3  2007/10/18 05:33:30  brian
# - better checking of NET-SNMP
#
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
