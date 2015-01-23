# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) File: m4/tcl.m4
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
# =============================================================================

# =============================================================================
# _TCL_EXTENSIONS
# -----------------------------------------------------------------------------
# Search for the TCL include directories necessary to build TCL extensions.
# This can be specified to configure by specifying --with-tcl=DIRECTORY.  The
# TCL include directories are searched for in the host and then build
# directories.  If we cannot find them, assume that TCL interfaces cannot build.
# -----------------------------------------------------------------------------
AC_DEFUN([_TCL_EXTENSIONS], [dnl
    AC_MSG_NOTICE([+-----------------------+])
    AC_MSG_NOTICE([| TCL Extension Support |])
    AC_MSG_NOTICE([+-----------------------+])
    AC_ARG_WITH([tcl],
	[AS_HELP_STRING([--with-tcl=HEADERS],
	    [the TCL header directory @<:@default=search@:>@])],
	[], [with_tcl=search])
    _BLD_FIND_DIR([tcl include directory], [tcl_cv_includedir], [
	    ${includedir}
	    ${rootdir}${oldincludedir}
	    ${rootdir}/usr/include
	    ${rootdir}/usr/local/include], [tcl.h], [no], [dnl
	if test ${with_tcl:-search} != no ; then
	    _BLD_INSTALL_WARN([TCL_H], [
***
*** Configure could not find the TCL extension include file tcl.h.  This
*** file is required to compile TCL extension libraries.  This file is
*** part of the 'tcl8.x' development package which is not always loaded
*** on all distributions.  Use the following commands to obtain the
*** 'tcl8.x' development package:
*** ], [
*** Debian 5.0:      'aptitude install tcl8.4-dev'
*** Ubuntu 8.04:     'aptitude install tcl8.4-dev'
*** Mandriva 2010.2: 'urpmi libtcl-devel'
*** CentOS 5.x:      'yum install tcl-devel'
*** openSUSE 11:     'zypper install tcl-devel'
*** SLES 10:         'zypper install tcl-devel'
*** ArchLinux:       'pacman -S tcl'
*** RedHat 7.2:      'rpm install tcl-8.3.3-67'], [
***
*** Otherwise, specify the location of the TCL headers with the
*** --with-tcl=DIRECTORY argument, or --without-tcl, on the next run of
*** configure.  Continuing under the assumption that --without-tcl was
*** intended.
*** ])
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_tcl --without-tcl\""
	    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-tcl'"
	    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-tcl'"
	    with_tcl=no
	fi], [], [with_tcl])
    if test ${tcl_cv_includedir:-no} = no ; then
	tclincludedir=
    else
	tclincludedir="$tcl_cv_includedir"
    fi
    AM_CONDITIONAL([WITH_TCL], [test :"${with_tcl:-search}" != :no])
    AC_SUBST([tclincludedir])dnl
    AC_CACHE_CHECK([for tcl cppflags], [tcl_cv_cppflags], [dnl
	if test -n "$tclincludedir" ; then
	    tcl_cv_cppflags="-I$tclincludedir"
	else
	    tcl_cv_cppflags=
	fi
    ])
    TCL_CPPFLAGS="$tcl_cv_cppflags"
    AC_SUBST([TCL_CPPFLAGS])dnl
])# _TCL_EXTENSIONS
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
