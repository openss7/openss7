# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: bld.m4,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-02-08 23:39:02 $
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
# Last Modified $Date: 2011-02-08 23:39:02 $ by $Author: brian $
#
# =============================================================================


# =============================================================================
# _BLD
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD], [dnl
    AC_REQUIRE([_DISTRO])dnl
    _BLD_BUILD_REQ
])# _BLD
# =============================================================================

# =============================================================================
# _BLD_BUILD_REQ
# -----------------------------------------------------------------------------
# The purpose of this macro is to create a file of build requirements and read
# those build requirements when the file exists.  This permits us to ship the
# build requirements files that were created on systems on which the build was
# successful.  The best way to accomplish this is to set
#
#   dist_cv_file_AC-VARIABLE-NAME
#
# to the absolute path to the file of the program or tool, and
#
#   dist_cv_pkg_AC-VARIABLE-NAME
#
# to the package that provides the program or tool required for build.  Then
# once the cache file is built these can be filtered out of the cache file and
# placed in a file for distribution.  If the build dependencies file is
# available, it can be loaded.
#
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_BUILD_REQ], [dnl
    # if build requirements file is not specified, check for one in the source
    # directory or one in the build directory
    if test -z "$CONFIG_BLDREQ" ; then
	case "$dist_cv_build_flavor" in
	    (centos|lineox|whitebox|fedora|mandrake|mandriva|redhat|suse)
		bld_req="${dist_cv_build_flavor}-${dist_cv_build_release}-${dist_cv_build_cpu}" ;;
	    (debian|ubuntu|*)
		bld_req="${dist_cv_build_flavor}-${dist_cv_build_codename}-${dist_cv_build_cpu}" ;;
	esac
	if test -n "$bld_req"; then
	    bld_req=`echo "$bld_req" | sed -e 'y,ABCDEFGHIJKLMNOPQRSTUVWXYZ,abcdefghijklmnopqrstuvwxyz,'`
	fi
	if test :"${USE_MAINTAINER_MODE:-no}" != :no ; then
	    CONFIG_BLDREQ="${srcdir}/${bld_req}-config.cache"
	else
	    CONFIG_BLDREQ="${bld_req}-config.cache"
	fi
    fi
    AC_SUBST([CONFIG_BLDREQ])dnl
    if test "$no_create" = yes ; then
	for config_bldreq in $CONFIG_BLDREQ ; do
	    case "$config_bldreq" in
		(/*) ;;
		(*) config_bldreq="`pwd`/$config_bldreq" ;;
	    esac
	    if test -r "$config_bldreq" ; then
		AC_MSG_NOTICE([reading cache file... $config_bldreq])
		. "$config_bldreq"
	    fi
	done
    fi
    AC_CONFIG_COMMANDS([bldconfig], [dnl
	if test -n "$CONFIG_BLDREQ" ; then
	    for config_bldreq in $CONFIG_BLDREQ ; do
		if test -w "$config_bldreq" -o ! -e "$config_bldreq" ; then
		    if touch "$config_bldreq" >/dev/null 2>&1 ; then
			cat "$cache_file" | egrep "^(test \"\\\[$]{)?bld_cv_" > "$config_bldreq" 2>/dev/null
		    fi
		fi
	    done
	fi], [dnl
cache_file="$cache_file"
CONFIG_BLDREQ="$CONFIG_BLDREQ"
    ])
])# _BLD_BUILD_REQ
# =============================================================================

# =============================================================================
# _BLD_BUILD_CHECK(COMMAND)
# -----------------------------------------------------------------------------
# Tests to see which distro package and distro package version provides the
# command specified in variable COMMAND.  Saves the package name in cache
# variable bld_cv_pkg_name_[COMMAND]  and the package version in cache variable
# bld_cv_pkg_ver_[COMMAND].
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_BUILD_CHECK], [dnl
    AC_REQUIRE([_BLD])dnl
    eval "tmp_cmd=\"\$bld_cv_pkg_name_$1\""
    if test ":$tmp_cmd" = : ; then
	case "${$1}" in
	    ([[\\/]]* | ?:[[\\/]]*)
		tmp_cmd="${$1}"
		;;
	    (*)
		# the command is expected just to be in the configure path
		tmp_cmd=`which ${$1} 2>/dev/null`
		;;
	esac
	tmp_result=
	if test -n "$tmp_cmd" ; then
	    case "$dist_cv_build_flavor" in
dnl	These use rpm
		(centos|lineox|whitebox|fedora|suse|opensuse|redhat|mandrake|mandriva)
		    tmp_result=`rpm -q --qf '[%{NAME}]\n' --whatprovides $tmp_cmd 2>/dev/null | head -1`
		    tmp_result=`echo "$tmp_result" | sed -e 's|.* is not .*||'`
		    ;;
dnl	These use dpkg
		(debian|ubuntu|mint)
		    tmp_result=`dpkg -S $tmp_cmd 2>/dev/null | tail -1`
		    tmp_result=`echo "$tmp_result" | sed -e 's|.*not found.*||'`
		    tmp_result=`echo "$tmp_result" | sed -e 's|[[^:]]*$$||;s|:.*||;s|,||g'`
		    ;;
	    esac
	fi
	if test -n "$tmp_result" ; then
	    bld_cv_pkg_name_$1="$tmp_result"
	else
	    unset bld_cv_pkg_name_$1
	fi
	tmp_result=
	if test -n "$tmp_cmd" ; then
	    case "$dist_cv_build_flavor" in
dnl	These use rpm
		(centos|lineox|whitebox|fedora|suse|opensuse|redhat|mandrake|mandriva)
		    tmp_result=`rpm -q --qf '[%{VERSION}]\n' --whatprovides $tmp_cmd 2>/dev/null | head -1`
		    tmp_result=`echo "$tmp_result" | sed -e 's|.* is not .*||'`
		    ;;
dnl	These use dpkg
		(debian|ubuntu|mint)
		    if test -n "$bld_cv_pkg_name_$1" ; then
			tmp_result=`dpkg-query -W -f='${Version}' $bld_cv_pkg_name_$1 2>/dev/null`
		    fi
		    ;;
	    esac
	fi
	if test -n "$tmp_result" ; then
	    bld_cv_pkg_ver_$1="$tmp_result"
	else
	    unset bld_cv_pkg_ver_$1
	fi
	tmp_result=`echo "$tmp_cmd" | sed -e 's|.*/||;s| .*||'`
	if test -n "$tmp_result" ; then
	    bld_cv_pkg_prog_$1="$tmp_result"
	else
	    unset bld_cv_pkg_prog_$1
	fi
	tmp_result="$bld_cv_pkg_name_$1"
	if test -n "$tmp_result" ; then
	    case "$dist_cv_build_flavor" in
		(centos|lineox|whitebox|fedora)
		    bld_cv_pkg_cmd_$1="yum install $tmp_result"
		    ;;
		(suse|opensuse)
		    bld_cv_pkg_cmd_$1="zypper install $tmp_result"
		    ;;
		(redhat)
		    bld_cv_pkg_cmd_$1="up2date install $tmp_result"
		    ;;
		(mandrake|mandriva)
		    bld_cv_pkg_cmd_$1="rpmi $tmp_result"
		    ;;
		(debian|ubuntu|mint)
		    bld_cv_pkg_cmd_$1="apt-get install $tmp_result"
		    ;;
		(*)
		    unset bld_cv_pkg_cmd_$1
		    ;;
	    esac
	else
	    unset bld_cv_pkg_cmd_$1
	fi
    fi])
])# _BLD_BUILD_CHECK
# =============================================================================

# =============================================================================
# _BLD_VAR_PATH_PROG([VARIABLE], [PROGRAMS], [PATH], [VAR-MSG], [IF-NOT-FOUND])
# -----------------------------------------------------------------------------
# Performs AC_ARG_VAR([VARIABLE], [VAR-MSG]), and then performs a call to
# AC_PATH_PROG([VARIABLE], [PROGRAMS], [IF-NOT-FOUND], [PATH]) and examines the
# result.  It then calls _BLD_BUILD_CHECK([VARIABLE]) when successful to set
# what package name and package version provides the program (if any).
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_VAR_PATH_PROG], [dnl
    AC_REQUIRE([_BLD])dnl
    AC_ARG_VAR([$1], [$4])dnl
    AC_PATH_PROG([$1], [$2], [], [$3])
    if test :"${$1:-no}" = :no ; then
	:
	$5
    else
	_BLD_BUILD_CHECK([$1])
    fi
])# _BLD_VAR_PATH_PROG
# =============================================================================

# =============================================================================
# _BLD_PATH_PROG (VARIABLE, PROG-TO-CHECK-FOR, [VALUE-IF-NOT-FOUND],
#		  [PATH = `$PATH'], [IF-NOT-FOUND], [IF-FOUND])
# Like `AC_PATH_PROG', but also provides an action to take if not found.  Also,
# when the program is found, the package containing the message is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_PATH_PROG], [dnl
    AC_REQUIRE([_BLD])dnl
    AC_PATH_PROG([$1], [$2], [], [$4])
    if test :"${$1:-no}" = :no ; then
	$1="$3"
	m4_if([$5], [], [AC_MSG_WARN([Cannot find $2 in PATH.])], [$5])
    else
	_BLD_BUILD_CHECK([$1])
	m4_if([$6], [], [:], [$6])
    fi
])# _BLD_PATH_PROG
# =============================================================================

# =============================================================================
# _BLD_PATH_PROGS (VARIABLE, PROGS-TO-CHECK-FOR, [VALUE-IF-NOT-FOUND],
#		   [PATH = `$PATH'], [IF-NOT-FOUND], [IF-FOUND])
# Like `AC_PATH_PROGS', but also provides an action to take if not found.  Also,
# when the program is found, the package containing the message is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_PATH_PROGS], [dnl
    AC_REQUIRE([_BLD])dnl
    AC_PATH_PROGS([$1], [$2], [], [$4])
    if test :"${$1:-no}" = :no ; then
	$1="$3"
	m4_if([$5], [], [AC_MSG_WARN([Cannot find $2 in PATH.])], [$5])
    else
	_BLD_BUILD_CHECK([$1])
	m4_if([$6], [], [:], [$6])
    fi
])# _BLD_PATH_PROGS
# =============================================================================

# =============================================================================
# _BLD_INSTALL_WARN (VARIABLE, MESSAGE-LEAD, GENERIC-INSTALL-MESSAGE,
#		     MESSAGE-TAIL)
# To be used in the IF-NOT-FOUND portion of _BLD_PATH_PROG or _BLD_PATH_PROGS.
# If there is a cached way to install the program specified by VARIABLE, warn
# with the MESSAGE-LEAD, the specific installation message, and the
# MESSAGE-TAIL; otherwise warn with the MESSAGE-LEAD, the
# GENERIC-INSTALL-MESSAGE and the MESSAGE-TAIL.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_INSTALL_WARN], [dnl
    if test -n "$bld_cv_pkg_cmd_$1" ; then
	tmp_msg="
*** $dist_cv_build_flavor: $bld_cv_pkg_cmd_$1"
    else
	tmp_msg="$3"
    fi
    AC_MSG_WARN([$2$tmp_msg$4])
])# _BLD_INSTALL_WARN
# =============================================================================

# =============================================================================
# _BLD_INSTALL_ERROR (VARIABLE, MESSAGE-LEAD, GENERIC-INSTALL-MESSAGE,
#		      MESSAGE-TAIL)
# To be used in the IF-NOT-FOUND portion of _BLD_PATH_PROG or _BLD_PATH_PROGS.
# If there is a cached way to install the program specified by VARIABLE, error
# with the MESSAGE-LEAD, the specific installation message, and the
# MESSAGE-TAIL; otherwise error with the MESSAGE-LEAD, the
# GENERIC-INSTALL-MESSAGE and the MESSAGE-TAIL.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_INSTALL_ERROR], [dnl
    if test -n "$bld_cv_pkg_cmd_$1" ; then
	tmp_msg="
*** $dist_cv_build_flavor: $bld_cv_pkg_cmd_$1"
    else
	tmp_msg="$3"
    fi
    AC_MSG_ERROR([$2$tmp_msg$4])
])# _BLD_INSTALL_ERROR
# =============================================================================

# =============================================================================
# _BLD_HEADER_CHECK (HEADER)
# -----------------------------------------------------------------------------
# Tests to see which distro package and distro package version provides the
# header file specified in variable HEADER.  Saves the package name in cache
# variable bld_cv_pkg_name_[HEADER] and the package version in cache variable
# bld_cv_pkg_ver_[HEADER].
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_HEADER_CHECK], [dnl
])# _BLD_HEADER_CHECK
# =============================================================================

# =============================================================================
# _BLD_CHECK_HEADER (HEADER, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#		     [INCLUDES])
# Like `AC_CHECK_HEADER', but when the header is found, the package providing
# the header is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_CHECK_HEADER], [dnl
])# _BLD_CHECK_HEADER
# =============================================================================

# =============================================================================
# _BLD_CHECK_HEADERS (HEADER..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#		      [INCLUDES])
# Like `AC_CHECK_HEADERS', but when each header is found, the package providing
# the header is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_CHECK_HEADERS], [dnl
])# _BLD_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _BLD_HEADER_WARN (HEADER, MESSAGE-LEAD, GENERIC-INSTALL-MESSAGE, MESSAGE-TAIL)
# To be used in the IF-NOT-FOUND portion of _BLD_CHECK_HEADER or
# _BLD_CHECK_HEADERS.  If there is a cached way to install the header specified
# by HEADER, warn with the MESSAGE-LEAD, the specific installation message, and
# the MESSAGE-TAIL; otherwise, warn with the MESSAGE-LEAD, the
# GENERIC-INSTALL-MESSAGE and the MESSAGE-TAIL.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_HEADER_WARN], [dnl
    [AS_VAR_PUSHDEF([bld_Header], [bld_cv_pkg_header_$1])]dnl
    [AS_VAR_IF([bld_Header], [], [dnl
	    tmp_msg="$3"
	], [
	    tmp_msg=AS_VAR_GET([bld_Header])
	    tmp_msg="
*** $dist_cv_build_flavor: $tmp_msg"
	])]
    [AS_VAR_POPDEF([bld_Header])]dnl
    AC_MSG_WARN([$2$tmp_msg$4])
])# _BLD_HEADER_WARN
# =============================================================================

# =============================================================================
# _BLD_HEADER_ERROR (HEADER, MESSAGE-LEAD, GENERIC-INSTALL-MESSAGE, MESSAGE-TAIL)
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_HEADER_ERROR], [dnl
# To be used in the IF-NOT-FOUND portion of _BLD_CHECK_HEADER or
# _BLD_CHECK_HEADERS.  If there is a cached way to install the header specified
# by HEADER, error with the MESSAGE-LEAD, the specific installation message, and
# the MESSAGE-TAIL; otherwise, error with the MESSAGE-LEAD, the
# GENERIC-INSTALL-MESSAGE and the MESSAGE-TAIL.
    [AS_VAR_PUSHDEF([bld_Header], [bld_cv_pkg_header_$1])]dnl
    [AS_VAR_IF([bld_Header], [], [dnl
	    tmp_msg="$3"
	], [
	    tmp_msg=AS_VAR_GET([bld_Header])
	    tmp_msg="
*** $dist_cv_build_flavor: $tmp_msg"
	])]
    [AS_VAR_POPDEF([bld_Header])]dnl
    AC_MSG_ERROR([$2$tmp_msg$4])
])# _BLD_HEADER_ERROR
# =============================================================================

# =============================================================================
# _BLD_
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_], [dnl
])# _BLD_
# =============================================================================

# =============================================================================
#
# $Log: bld.m4,v $
# Revision 1.1.2.3  2011-02-08 23:39:02  brian
# - last minute release updates
#
# Revision 1.1.2.2  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.1  2010-11-28 13:25:01  brian
# - added new build files, removed redundant files
#
#
# =============================================================================
# 
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
