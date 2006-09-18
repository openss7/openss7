# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: doxy.m4,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/09/18 13:20:04 $
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
# Last Modified $Date: 2006/09/18 13:20:04 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _DOXY_OPTIONS
# -----------------------------------------------------------------------------
# We use doxygen to document user files and kerneldoc to doucment kernel
# files.  Therefore, USER_CPPFLAGS needs to already be defined at this point
# so that we can strip out the include paths.
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_OPTIONS], [dnl
    _DOXY_CONFIG_STRIP_FROM_INC_PATH
    _DOXY_CONFIG_INPUT
])# _DOXY_OPTIONS
# =============================================================================

# =============================================================================
# _DOXY_CONFIG_STRIP_FROM_INC_PATH
# -----------------------------------------------------------------------------
# We use doxygen to document user files and kerneldoc to doucment kernel
# files.  Therefore, USER_CPPFLAGS and PKG_INCLUDES needs to already be
# defined at this point so that we can strip out the include paths.
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_CONFIG_STRIP_FROM_INC_PATH], [dnl
    top_builddir=`(cd . ; pwd)`
    top_srcdir=`(cd $srcdir ; pwd)`
    for doxy_tmp in $USER_CPPFLAGS $PKG_INCLUDES
    do
	doxy_incs=
	case $doxy_tmp in
	    -I*)
	    doxy_tmp=`echo $doxy_tmp | sed -e 's|^-I||'`
	    doxy_tmp=`eval "echo \"$doxy_tmp\""`
	    AC_MSG_CHECKING([for doxy include $doxy_tmp])
	    case $doxy_tmp in
		/*) ;;
		*)
		    if test -d $doxy_tmp ; then
			doxy_tmp=`(cd $doxy_tmp ; pwd)`
		    else
			AC_MSG_RESULT([no])
			continue
		    fi
		    ;;
	    esac
	    doxy_incs="${doxy_incs:+$doxy_incs }${doxy_tmp}"
	    AC_MSG_RESULT([yes])
	    ;;
	esac
    done
    DOXY_CONFIG_STRIP_FROM_INC_PATH="$doxy_incs"
    AC_SUBST([DOXY_CONFIG_STRIP_FROM_INC_PATH])dnl
    AC_SUBST([doxy_incs])dnl
])# _DOXY_CONFIG_STRIP_FROM_INC_PATH
# =============================================================================

# =============================================================================
# _DOXY_CONFIG_INPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_CONFIG_INPUT], [dnl
    doxy_abs_builddir=`(cd . ; pwd)`
    doxy_abs_srcdir=`(cd $srcdir ; pwd)`
    doxy_tmp=`find $doxy_abs_builddir $doxy_abs_srcdir -name '*.h' -o -name '*.c' | xargs grep -l 'doxygen(1)' | sort | uniq`
    doxy_inputs=
    for doxy_file in $doxy_tmp ; do
	AC_MSG_CHECKING([for doxy input $doxy_file])
	doxy_inputs="${doxy_inputs:+$doxy_inputs }${doxy_file}"
	AC_MSG_RESULT([yes])
    done
    DOXY_CONFIG_INPUT="$doxy_inputs"
    AC_SUBST([DOXY_CONFIG_INPUT])dnl
    AC_SUBST([doxy_abs_builddir])dnl
    AC_SUBST([doxy_abs_srcdir])dnl
    AC_SUBST([doxy_inputs])dnl
])# _DOXY_CONFIG_INPUT
# =============================================================================

# =============================================================================
# _DOXY_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_SETUP], [dnl
    AC_ARG_VAR([DOXYGEN], [doxygen command])
    AC_PATH_TOOL([DOXYGEN], [doxygen], [], [$PATH:/usr/local/bin:/usr/bin])
    if test :"${DOXYGEN:-no}" = :no ; then
	AC_MSG_WARN([Could not find doxygen program in PATH.])
    fi
    AM_CONDITIONAL([HAVE_DOXYGEN], [test ":${DOXYGEN:-no}" != :no])dnl
])# _DOXY_SETUP
# =============================================================================

# =============================================================================
# _DOXY_OUTPUT_CONFIG_COMMANDS
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_OUTPUT_CONFIG_COMMANDS], [dnl
    ${DOXYGEN:-doxygen} scripts/Doxyfile
])# _DOXY_OUTPUT_CONFIG_COMMANDS
# =============================================================================

# =============================================================================
# _DOXY_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_OUTPUT], [dnl
    AC_CONFIG_FILES([scripts/Doxyfile])
    if test -d $srcdir/doc ; then
	doxydir='./doc'
    else
	doxydir='.'
    fi
    AC_SUBST([doxydir])
dnl    AC_CONFIG_COMMANDS([doxygen],
dnl	[_DOXY_OUTPUT_CONFIG_COMMANDS], [dnl
dnldoxydir="doc"
dnlDOXYGEN="$DOXYGEN"
dnl    ])
])# _DOXY_OUTPUT
# =============================================================================

# =============================================================================
# _DOXY
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY], [dnl
    _DOXY_OPTIONS
    _DOXY_SETUP
    _DOXY_OUTPUT
])# _DOXY
# =============================================================================

# =============================================================================
# _DOXY_
# -----------------------------------------------------------------------------
AC_DEFUN([_DOXY_], [dnl
])# _DOXY_
# =============================================================================


# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
