# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: agent.m4,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-07-21 11:06:12 $
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
# Last Modified $Date: 2009-07-21 11:06:12 $ by $Author: brian $
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
dnl
dnl Building SNMP agents requires the presence of perl libraries.  Note that
dnl only fedora, redhat and other broken packaging of net-snmp needs perl
dnl libraries.  The Debian 4.0 (Etch) packaging have proper load dependencies
dnl between libraries, so loading the SNMP agent libraries properly loads
dnl dependent libraries.
dnl
    AC_REQUIRE([_PERL_LIBRARIES])
dnl
dnl Building SNMP agents requires the presence of snmp libraries.
dnl
    AC_REQUIRE([_SNMP])
    _AGENT_EXTENSIONS
])# _AGENT
# =============================================================================

AC_DEFUN([_AGENT_MSG_WARN], [dnl
    AS_REQUIRE_SHELL_FN([agent_msg_warn], [dnl
    cat<<EOF

*** 
*** Compiling SNMP agents requires the availability of UCD-SNMP header
*** files and in particular <[$]1>.  This file
*** could not be found.  Perhaps you need to load the NET-SNMP
*** development package (net-snmp-dev or libsnmp9-dev).  Use the
*** following commands to obtain the NET-SNMP development package:
***
*** Debian 4.0:  'apt-get install libsnmp9-dev'
*** Ubuntu 8.04: 'apt-get install libsnmp-dev'
*** CentOS 5.x:  'yum install net-snmp-devel'
*** SLES 10:     'zypper install net-snmp-devel'
*** RedHat 7.2:  'rpm -i ucd-snmp-devel-4.2.5-8.72.1'
*** RedHat 7.3:  'rpm -i ucd-snmp-devel-4.2.5-8.73.1'
***
*** Repeat after loading the correct package or by specifying the
*** configure argument --without-snmp-agent: continuing under the
*** assumption that the option --without-snmp was intended.
***
EOF
    ])dnl
    if test :"${with_snmp_agent:-yes}" != :no ; then
	ac_msg=`agent_msg_warn $1`
	AC_MSG_WARN([$ac_msg])
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_snmp_agent --without-snmp-agent\""
	PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-snmp-agent'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-snmp-agent'"
	with_snmp_agent=no
    fi
])

# =============================================================================
# _AGENT_EXTENSIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_AGENT_EXTENSIONS], [dnl
    # It is not necessary to compile the SNMP agents.  This option
    # decides whether to do so or not.
    AC_ARG_WITH([snmp-agent],
	[AS_HELP_STRING([--with-snmp-agent=HEADERS, --without-snmp-agent],
	    [SNMP agent header directory @<:@default=$INCLUDEDIR@:>@])]
	[AS_HELP_STRING([--without-snmp-agent],
	    [suppress SNMP agents @<:@default=enabled@:>@])])
    AM_CONDITIONAL([WITH_SNMP_AGENT], [test :"${with_snmp_agent:-yes}" != :no])dnl
    AC_MSG_NOTICE([+-----------------------------------+])
    AC_MSG_NOTICE([| SNMP agent UCD header file checks |])
    AC_MSG_NOTICE([+-----------------------------------+])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-config.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/ucd-snmp-config.h])])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-includes.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/ucd-snmp-includes.h])], [
#include <ucd-snmp/ucd-snmp-config.h>
])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-agent-includes.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/ucd-snmp-agent-includes.h])], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/callback.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/callback.h])], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/snmp-tc.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/snmp-tc.h])], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
])
    AC_CHECK_HEADERS([ucd-snmp/default_store.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/default_store.h])], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
])
    AC_CHECK_HEADERS([ucd-snmp/snmp_alarm.h], [],
	[_AGENT_MSG_WARN([ucd-snmp/snmp_alarm.h])], [
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
])# _AGENT_EXTENSIONS
# =============================================================================

# =============================================================================
#
# $Log: agent.m4,v $
# Revision 1.1.2.2  2009-07-21 11:06:12  brian
# - changes from release build
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# Revision 0.9.2.4  2008-04-28 09:41:03  brian
# - updated headers for release
#
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
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
