# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: tcl.m4,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-07-13 07:13:34 $
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
# Last Modified $Date: 2009-07-13 07:13:34 $ by $Author: brian $
#
# =============================================================================

m4_define([AC_PROG_TCL], [dnl
dnl
dnl First order of business is to find tclConfig.sh.  This file contains
dnl shell variables that provide information on where other things can
dnl be found.  Unfortunately it is included in various places.  Debian
dnl places this file in ${libdir}/tcl8.4 whereas RedHat/CentOS and SuSE
dnl places it in ${libdir}.  Ubuntu places it in ${libdir}/tcl8.4
dnl
    AC_CACHE_CHECK([for tcl configuration], [ac_cv_tcl_config_sh], [dnl
	eval "tcl_search_path=\"
	    ${DESTDIR}${rootdir}${libdir}
	    ${DESTDIR}${rootdir}/usr/lib
	    ${DESTDIR}${libdir}
	    ${DESTDIR}/usr/lib\""
	tcl_search_path=`echo "$tcl_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for tcl_dir in $tcl_search_path ; do
	    if test -d "$tcl_dir" ; then
		AC_MSG_CHECKING([for tcl configuration... $tcl_dir])
		tcl_files=`find "$tcl_dir" -mindepth 1 -maxdepth 2 -name 'tclConfig.sh' 2>/dev/null | sort -ru`
		for tcl_file in $tcl_files ; do
		    if test -f "$tcl_file" && grep '^TCL_PACKAGE_PATH' "$tcl_file" >/dev/null ; then
			ac_cv_tcl_config_sh="$tcl_file"
			AC_MSG_RESULT([yes])
			break 2
		    fi
		done
		AC_MSG_RESULT([no])
	    fi
	done
	test -n "$ac_cv_tcl_config_sh" || ac_cv_tcl_config_sh=no
	AC_MSG_CHECKING([for tcl configuration])
    ])
    if test :"${ac_cv_tcl_config_sh:-no}" = :no ; then
	AC_MSG_ERROR([
***
*** Configure could not find a suitable tclConfig.sh TCL configuration
*** file.  This file is required to configure TCL interface libraries
*** and TCL interpreted scripts.  This file is part of the 'tcl8.x'
*** development package which is not always loaded on all distributions.
*** Use the following commands to obtain the 'tcl8.x' development
*** package:
***
*** Debian 4.0:  'apt-get install tcl8.4-dev'
*** Ubuntu 8.04: 'apt-get install tcl8.4-dev'
*** CentOS 5.x:  'yum install tcl-devel'
*** openSUSE 11: 'zypper install tcl-devel'
*** SLES 10:     'zypper install tcl-devel'
*** RedHat 7.2:  'rpm install tcl-8.3.3-67'
*** ])
    else
	TCLCONFIG="$ac_cv_tcl_config_sh"
    fi
dnl
dnl We can now use tclConfig.sh to find out more information about where
dnl to install things.  TCL_PACKAGE_PATH contains the same as the
dnl $tcl_pkgPath variable under tclsh.  The first directory in this list
dnl is the directory into which to install binary packages.  The second
dnl directory (or first if there is only one directory) in this list is
dnl the directory into which to install source packages.
dnl
    AC_CACHE_CHECK([for tcl binary install directory], [ac_cv_tcllibdir], [dnl
	tcl_dirs=
	tcl_cmd=`grep '^TCL_PACKAGE_PATH' "$TCLCONFIG" | sed -n 's,TCL_PACKAGE_PATH=,tcl_dirs=,p;t'`
	eval "${tcl_cmd:-:}"
	for tcl_dir in $tcl_dirs ; do
	    ac_cv_tcllibdir="${rootdir}$tcl_dir"
	    break
	done
	test -n "$ac_cv_tcllibdir" || ac_cv_tcllibdir=no
    ])
    if test :"${ac_cv_tcllibdir:-no}" = :no ; then
	AC_MSG_WARN([Cannot find tcl binary install directory, assuming \[$]{libdir}.])
	tcllibdir="${rootdir}${libdir}"
    else
	tcllibdir="$ac_cv_tcllibdir"
    fi
    AC_SUBST([tcllibdir])dnl
    pkgtcllibdir='${tcllibdir}/${PACKAGE_NAME}${PACKAGE_VERSION}'
    AC_SUBST([pkgtcllibdir])dnl
    AC_CACHE_CHECK([for tcl source install directory], [ac_cv_tclsrcdir], [dnl
	tcl_dirs=
	tcl_cmd=`grep '^TCL_PACKAGE_PATH' "$TCLCONFIG" | sed -n 's,TCL_PACKAGE_PATH=,tcl_dirs=,p;t'`
	eval "${tcl_cmd:-:}"
	for tcl_dir in $tcl_dirs ; do
	    if test -n "$ac_cv_tclsrcdir" ; then
		ac_cv_tclsrcdir="${rootdir}$tcl_dir"
		break
	    else
		ac_cv_tclsrcdir="${rootdir}$tcl_dir"
		continue
	    fi
	done
	test -n "$ac_cv_tclsrcdir" || ac_cv_tclsrcdir=no
    ])
    if test :"${ac_cv_tclsrcdir:-no}" = :no ; then
	AC_MSG_WARN([Cannot find tcl source install directory, assuming \[$]{datadir}.])
	tclsrcdir="${rootdir}${datadir}"
    else
	tclsrcdir="$ac_cv_tclsrcdir"
    fi
    AC_SUBST([tclsrcdir])dnl
dnl
dnl Search for the TCL include directories.  This can be specified to
dnl configure by specifying the tclincludedir variable.  The TCL include
dnl directories are searched for in the target, host and then build
dnl directories.  We use tclConfig.sh to direct the search.  If we
dnl cannot find them, assume that the compiler knows how to find tcl.h.
dnl
    AC_CACHE_CHECK([for tcl include directory], [ac_cv_tcl_includedir], [dnl
	tcl_dirs=
	tcl_cmd=`grep '^TCL_INCLUDE_SPEC' "$TCLCONFIG" | sed -n 's,TCL_INCLUDE_SPEC=,tcl_dirs=,p;t'`
	eval "${tcl_cmd:-:}"
	ac_cv_tcl_includedir="${tcl_tmp:-no}"
	tcl_dirs=`echo " $tcl_dirs" | sed 's,[[[:space:]]]-I[[[:space:]]]*,,g'`
	AC_MSG_RESULT([searching])
	for tcl_dir in $tcl_dirs ; do
	    eval "tcl_search_path=\"
		${rootdir}${tcl_dir}
		${tcl_dir}\""
	    for tcl_tmp in $tcl_search_path ; do
		AC_MSG_CHECKING([for tcl include directory... -I$tcl_tmp])
		if test -d "${DESTDIR}$tcl_tmp" -a -f "${DESTDIR}$tcl_tmp/tcl.h" ; then
		    ac_cv_tcl_includedir="$tcl_tmp"
		    AC_MSG_RESULT([yes])
		    break 2
		else
		    AC_MSG_RESULT([no])
		fi
	    done
	done
	test -n "$ac_cv_tcl_includedir" || ac_cv_tcl_includedir=no
	AC_MSG_CHECKING([for tcl include directory])
    ])
    if test :"${ac_cv_tcl_includedir:-no}" = :no ; then
	AC_MSG_WARN([Cannot find include flags, assuming -I\${includedir}.])
	tclincludedir=
    else
	tclincludedir="$ac_cv_tcl_includedir"
    fi
    AC_SUBST([tclincludedir])dnl
    AC_CACHE_CHECK([for tcl cppflags], [ac_cv_tcl_cppflags], [dnl
	if test -n "$tclincludedir" ; then
	    ac_cv_tcl_cppflags="-I$tclincludedir"
	else
	    ac_cv_tcl_cppflags=
	fi
    ])
    TCL_CPPFLAGS="$ac_cv_tcl_cppflags"
    AC_SUBST([TCL_CPPFLAGS])dnl
    AC_CACHE_CHECK([for tcl ldflags], [ac_cv_tcl_ldflags], [dnl
	tcl_tmp=
	tcl_cmd=`grep '^TCL_LIB_SPEC' "$TCLCONFIG" | sed -n 's,TCL_LIB_SPEC=,tcl_tmp=,p;t'`
	eval "${tcl_cmd:-:}"
	ac_cv_tcl_ldflags="${tcl_tmp:-no}"
    ])
    if test :"${ac_cv_tcl_ldflags:-no}" = :no ; then
	AC_MSG_WARN([Cannot find library, assuming -ltcl.])
	TCL_LDFLAGS='-ltcl'
	TCL_DBGX=
    else
	TCL_LDFLAGS="$ac_cv_tcl_ldflags"
	TCL_DBGX=
    fi
    AC_SUBST([TCL_LDFLAGS])dnl
    AC_SUBST([TCL_DBGX])dnl
])


# =============================================================================
#
# $Log: tcl.m4,v $
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
