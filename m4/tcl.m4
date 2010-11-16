# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: tcl.m4,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-07-21 11:06:13 $
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
# Last Modified $Date: 2009-07-21 11:06:13 $ by $Author: brian $
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
    AC_CACHE_CHECK([for tcl include directory], [tcl_cv_includedir], [dnl
	AC_ARG_WITH([tcl],
	    [AS_HELP_STRING([--with-tcl=HEADERS],
		[the TCL header directory @<:@default=search@:>@])],
	    [], [with_tcl=search])
	case "${with_tcl:-search}" in
	    (no) tcl_cv_includedir=no ;;
	    (yes|search) ;;
	    (*) if test -f "$with_tcl/tcl.h" ; then tcl_cv_includedir="$with_tcl" ; fi ;;
	esac
	if test -z "$tcl_cv_includedir" ; then
	    eval "tcl_search_path=\"
		${DESTDIR}${rootdir}${includedir}
		${DESTDIR}${rootdir}${oldincludedir}
		${DESTDIR}${rootdir}/usr/include
		${DESTDIR}${rootdir}/usr/local/include
		${DESTDIR}${includedir}
		${DESTDIR}${oldincludedir}
		${DESTDIR}/usr/include
		${DESTDIR}/usr/local/include\""
	    tcl_search_path=`echo "$tcl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	    AC_MSG_RESULT([searching])
	    for tcl_dir in $tcl_search_path ; do
		test -d "$tcl_dir" || continue
		AC_MSG_CHECKING([for tcl include directory... $tcl_dir])
		tcl_files=`find "$tcl_dir" -mindepth 1 -maxdepth 2 -type f -name 'tcl.h' 2>/dev/null`
		for tcl_file in $tcl_files ; do
		    test -r "$tcl_file" || continue
		    tcl_dir="${tcl_file%/tcl.h}"
		    #tcl_dir="${tcl_dir#$DESTDIR}"
		    #tcl_dir="${tcl_dir#$rootdir}"
		    tcl_cv_includedir="$tcl_dir"
		    AC_MSG_RESULT([yes])
		    break 2
		done
		AC_MSG_RESULT([no])
	    done
	    test -n "$tcl_cv_includedir" || tcl_cv_includedir=no
	fi
	AC_MSG_CHECKING([for tcl include directory])
    ])
    if test :"${tcl_cv_includedir:-no}" = :no ; then
	if test :"${with_tcl:-search}" != :no ; then
	    AC_MSG_WARN([
***
*** Configure could not find the TCL extension include file tcl.h.  This
*** file is required to compile TCL extension libraries.  This file is
*** part of the 'tcl8.x' development package which is not always loaded
*** on all distributions.  Use the following commands to obtain the
*** 'tcl8.x' development package:
***
*** Debian 5.0:  'apt-get install tcl8.4-dev'
*** Ubuntu 8.04: 'apt-get install tcl8.4-dev'
*** CentOS 5.x:  'yum install tcl-devel'
*** openSUSE 11: 'zypper install tcl-devel'
*** SLES 10:     'zypper install tcl-devel'
*** RedHat 7.2:  'rpm install tcl-8.3.3-67'
***
*** Otherwise, specify the location of the TCL headers with the
*** --with-tcl=DIRECTORY argument, or --without-tcl, on the next run of
*** configure.  Continuing under the assumption that --without-tcl was
*** intended.
***])
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_tcl --without-tcl\""
	    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-tcl'"
	    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-tcl'"
	    with_tcl=no
	fi
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
# $Log: tcl.m4,v $
# Revision 1.1.2.2  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.1  2009-07-13 07:13:34  brian
# - changes for multiple distro build
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
