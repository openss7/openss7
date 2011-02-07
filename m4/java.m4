# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: java.m4,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2011-02-07 04:48:32 $
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
# AC_PROG_JAVA
# -----------------------------------------------------------------------------
m4_define([AC_PROG_JAVA], [dnl
    AC_MSG_NOTICE([+-----------------------------+])
    AC_MSG_NOTICE([| Java Code Generation Checks |])
    AC_MSG_NOTICE([+-----------------------------+])
    _JAVA_OPTIONS
    _JAVA_SETUP
    _JAVA_USER
    _JAVA_OUTPUT
])# AC_PROG_JAVA
# =============================================================================

# =============================================================================
# _JAVA_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_OPTIONS], [dnl
    AC_ARG_ENABLE([java],
	[AS_HELP_STRING([--disable-java],
	    [JAVA modules @<:@default=enabled@:>@])],
	    [], [enable_java=yes])
])# _JAVA_OPTIONS
# =============================================================================

# =============================================================================
# _JAVA_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_SETUP], [dnl
    AC_ARG_VAR([CLASSPATH],
	       [Java CLASSPATH variable. @<:@default=auto@:>@])
    if test :${enable_java:-yes} = :yes ; then
	if test :${javaclasspath+set} != :set ; then
	    if test :${CLASSPATH+set} != :set ; then
		CLASSPATH='${builddir}:${srcdir}'
	    fi # java.m4 91
	    javaclasspath="$CLASSPATH"
	else
	    CLASSPATH='${javaclasspath}'
	fi # java.m4 95
	_JAVA_SETUP_GCJ
	_JAVA_SETUP_JAR
    fi # java.m4 98
    AC_SUBST([javaclasspath])dnl
])# _JAVA_SETUP
# =============================================================================

# =============================================================================
# _JAVA_SETUP_JAR
# -----------------------------------------------------------------------------
# Checks for the jar or fastjar command.  This command is used to generate
# Java archives.  In general it is possible to generate simple java archives
# using a script and the zip command.
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_SETUP_JAR], [dnl
    AC_REQUIRE([_OPENSS7_MISSING4])
    jar_tmp="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_VAR([JAR],
	[Java archive command. @<:@default=fastjar,jar@:>@])
dnl
dnl On older systems with both gcj and gcj3 use jar3 over jar.
dnl fastjar is a later incarnation.
dnl
    _BLD_PATH_PROGS([JAR], [fastjar jar3 jar], [${am_missing4_run}jar],
	[$jar_tmp], [dnl
	_BLD_INSTALL_WARN([JAR], [
***
*** Configure cannot find a suitable 'jar' program.  Generating Java
*** archives requires the 'jar' program.  You can normally get 'jar' as
*** part of most popular GNU/Linux distributions and all current
*** versions are acceptable.  The 'jar' program has been available for
*** many years.  'fastjar' is part of the GNU Compiler Collection.  You
*** can also get 'jar' as part of any JDK.  Use the following command to
*** obtain 'fastjar':
*** ], [
*** Debian 5.0:	 'apt-get install fastjar'
*** Ubuntu 8.04: 'apt-get install fastjar'
*** CentOS 5.x:	 'yum install libgcj'
*** SLES 10:	 'zypper install fastjar'
*** RH 7.3:	 'rpm -i libgcj3'], [
***
*** To get rid of this warning, load the 'fastjar' package or specify
*** the appropriate program with the JAR environment variable to
*** 'configure'.
*** ])])
    AC_ARG_VAR([ZIP],
	[Zip archive command. @<:@default=zip@:>@])
    _BLD_PATH_PROG([ZIP], [zip], [${am_missing4_run}zip],
	[$jar_tmp], [dnl
	_BLD_INSTALL_WARN([ZIP], [
***
*** Configure cannot find a suitable 'zip' program.  Try:
*** ], [
*** Debian 5.0:	 'apt-get install zip'
*** Ubuntu 8.04: 'apt-get install zip'
*** CentOS 5.x:	 'yum install zip'
*** SLES 10:	 'zypper install zip'
*** RH 7.3:	 'rpm -i zip'], [
***
*** To get rid of this warning, load the 'zip' program or specify the
*** appropriate program with the ZIP enviornment variable to
*** 'configure'.
*** ])])
dnl
dnl Note that SLES 11.1 fails to provide proper symbolic links from gjarsigner
dnl to gjarsigner-4.3 as well as from gkeytool to gkeytool-4.3.  This is the
dnl reason for explicitly placing gjarsigner-4.3 and gkeytool-4.3 in the lists.
dnl
    _BLD_PATH_PROGS([JARSIGNER], [gjarsigner jarsigner gjarsigner-4.3],
		    [${am_missing4_run}jarsigner], [$jar_tmp])
    _BLD_PATH_PROGS([KEYTOOL], [gkeytool keytool gkeytool-4.3],
		    [${am_missing4_run}keytool], [$jar_tmp])
])# _JAVA_SETUP_JAR
# =============================================================================

# =============================================================================
# _JAVA_SETUP_GCJ
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_SETUP_GCJ], [dnl
    _GCJ_CONFIG
])# _JAVA_SETUP_GCJ
# =============================================================================

# =============================================================================
# _JAVA_USER
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_USER], [dnl
])# _JAVA_USER
# =============================================================================

# =============================================================================
# _JAVA_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_JAVA], [test :${enable_java:-yes} = :yes])dnl
])# _JAVA_OUTPUT
# =============================================================================

# =============================================================================
# _JAVA_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_JAVA_XXX], [dnl
])# _JAVA_XXX
# =============================================================================

# =============================================================================
#
# $Log: java.m4,v $
# Revision 1.1.2.7  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.6  2010-11-28 13:55:51  brian
# - update build requirements, proper autoconf functions, build updates
#
# Revision 1.1.2.5  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.4  2009-07-13 07:13:27  brian
# - changes for multiple distro build
#
# Revision 1.1.2.3  2009-07-05 12:04:27  brian
# - updates for release builds
#
# Revision 1.1.2.2  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.1  2009-06-29 07:35:54  brian
# - added new macros for docs and java
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

