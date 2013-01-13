# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: bld.m4,v $ $Name:  $($Revision: 1.1.2.11 $) $Date: 2011-09-20 09:51:32 $
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
# Last Modified $Date: 2011-09-20 09:51:32 $ by $Author: brian $
#
# =============================================================================


# =============================================================================
# _BLD
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD], [dnl
    AC_REQUIRE([_DISTRO])dnl
    _BLD_PROG_CHECK
    _BLD_PATH_CHECK
    _BLD_FILE_CHECK
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
	if test :"${USE_MAINTAINER_MODE:-no}" != :no ; then
	    CONFIG_BLDREQ="${srcdir}/cache/${build_distos}-config.cache"
	else
	    CONFIG_BLDREQ="${build_distos}-config.cache"
	fi
    fi
    AC_SUBST([CONFIG_BLDREQ])dnl
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
# _BLD_PROG_CHECK(COMMAND)
# -----------------------------------------------------------------------------
# Tests to see which distro package and distro package version provides the
# command specified in variable COMMAND.  Saves the package name in cache
# variable bld_cv_pkg_name_[COMMAND]  and the package version in cache variable
# bld_cv_pkg_ver_[COMMAND].  The program itself is saved in cache variable
# bld_cv_pkg_prog_[COMMAND].
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_PROG_CHECK],
    [AC_REQUIRE_SHELL_FN([bld_prog_check],
	[AS_FUNCTION_DESCRIBE([bld_prog_check],
	    [COMMAND],
	    [COMMAND - Variable containing the command to check.])], [dnl
    _bld_tmp_cn="[$]1"
    eval "_bld_tmp_result=\"\${$_bld_tmp_cn}\""
    case "$_bld_tmp_result" in
	([[\\/]]* | ?:[[\\/]]*)
	    eval "_bld_tmp_cmd=\"\${$_bld_tmp_cn}\""
	    ;;
	(*)
	    # the command is expected just to be in the configure path
	    eval "_bld_tmp_cmd=\"\`which \${$_bld_tmp_cn} 2>/dev/null\`\""
	    ;;
    esac
    # check four levels of indirection for /etc/alternatives
    test -L "$_bld_tmp_cmd" && _bld_tmp_cmd=`readlink "$_bld_tmp_cmd"`
    test -L "$_bld_tmp_cmd" && _bld_tmp_cmd=`readlink "$_bld_tmp_cmd"`
    test -L "$_bld_tmp_cmd" && _bld_tmp_cmd=`readlink "$_bld_tmp_cmd"`
    test -L "$_bld_tmp_cmd" && _bld_tmp_cmd=`readlink "$_bld_tmp_cmd"`
    _bld_tmp_result=
    if test -n "$_bld_tmp_cmd" ; then
	if test -z "$_bld_tmp_result" -a -x "`which rpm 2>/dev/null`" ; then
	    _bld_tmp_result=`rpm -q --qf '[%{NAME}]\n' --whatprovides $_bld_tmp_cmd 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.* is not .*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*no package provides.*||'`
	fi
	if test -z "$_bld_tmp_result" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \) ; then
dnl	    dlocate is much faster than dpkg and dpkg-query
	    if which dlocate >/dev/null 2>&1
	    then dlocate=dlocate; term='$'
	    else dlocate=dpkg;    term=
	    fi
	    _bld_tmp_result=`$dlocate -S "$_bld_tmp_cmd$term" 2>/dev/null | tail -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*not found.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|[[^:]]*$$||;s|:.*||;s|,||g'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which pacman 2>/dev/null`" ; then
	    _bld_tmp_result=`pacman -Q --owns $_bld_tmp_cmd 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*No package owns.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | awk '{print[$]5}'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which slackpkg 2>/dev/null`"; then
	    _bld_tmp_cmd2=`echo "$_bld_tmp_cmd" | sed -e 's,^/,,'`
	    _bld_tmp_result=`slackpkg file-search $_bld_tmp_cmd2 2>/dev/null | grep '^[[[] installed []]]' | head -1 | awk '{print[$]NF}'`
	    test -z "$_bld_tmp_result" || _bld_tmp_result=`echo "$_bld_tmp_result" | awk 'BEGIN{FS="-";OFS="-"}{NF=NF-3;print[$]0}'`
	fi
    fi
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_name_${_bld_tmp_cn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_name_${_bld_tmp_cn}"
    fi
    _bld_tmp_result=
    if test -n "$_bld_tmp_cmd" ; then
	if test -z "$_bld_tmp_result" -a -x "`which rpm 2>/dev/null`" ; then
	    _bld_tmp_result=`rpm -q --qf '[%{VERSION}]\n' --whatprovides $_bld_tmp_cmd 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.* is not .*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*no package provides.*||'`
	fi
	if test -z "$_bld_tmp_result" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \) ; then
dnl	    dlocate is much faster than dpkg and dpkg-query
	    if which dlocate >/dev/null 2>&1
	    then dlocate=dlocate; term='$'
	    else dlocate=dpkg;    term=
	    fi
	    eval "_bld_tmp_result=\"\$bld_cv_pkg_name_${_bld_tmp_cn}\""
	    if test -n "$_bld_tmp_result" ; then
		_bld_tmp_result=`$dlocate -s "$_bld_tmp_result" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || _bld_tmp_result=
	    fi
	fi
	if test -z "$_bld_tmp_result" -a -x "`which pacman 2>/dev/null`" ; then
	    _bld_tmp_result=`pacman -Q --owns $_bld_tmp_cmd 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*No package owns.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | awk '{print[$]6}'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which slackpkg 2>/dev/null`"; then
	    _bld_tmp_cmd2=`echo "$_bld_tmp_cmd" | sed -e 's,^/,,'`
	    _bld_tmp_result=`slackpkg file-search $_bld_tmp_cmd2 2>/dev/null | grep '^[[[] installed []]]' | head -1 | awk '{print[$]NF}'`
	    test -z "$_bld_tmp_result" || _bld_tmp_result=`echo "$_bld_tmp_result" | awk 'BEGIN{FS="-"}{print[$](NF-2)}'`
	fi
    fi
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_ver_${_bld_tmp_cn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_ver_${_bld_tmp_cn}"
    fi
    _bld_tmp_result=`echo "$_bld_tmp_cmd" | sed -e 's|.*/||;s| .*||'`
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_prog_${_bld_tmp_cn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_prog_${_bld_tmp_cn}"
    fi
    eval "_bld_tmp_result=\"\$bld_cv_pkg_name_${_bld_tmp_cn}\""
    if test -n "$_bld_tmp_result" ; then
	case "$build_distro" in
	    (oracle|puias|centos|lineox|whitebox|scientific|fedora|rhel)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"yum install \$_bld_tmp_result\""
		;;
	    (suse|sle|openSUSE)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"zypper install \$_bld_tmp_result\""
		;;
	    (redhat)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"up2date install \$_bld_tmp_result\""
		;;
	    (mandrake|mandriva|manbo|mageia|mes)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"urpmi \$_bld_tmp_result\""
		;;
	    (debian|ubuntu|lts|mint)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"aptitude install \$_bld_tmp_result\""
		;;
	    (arch)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"pacman --sync \$_bld_tmp_result\""
		;;
	    (salix)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"slapt-get --install \$_bld_tmp_result\""
		;;
	    (slackware)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"slackpkg install \$_bld_tmp_result\""
		;;
	    (*)
		eval "unset bld_cv_pkg_cmd_${_bld_tmp_cn}"
		;;
	esac
    else
	eval "unset bld_cv_pkg_cmd_${_bld_tmp_cn}"
    fi])
])# _BLD_PROG_CHECK
# =============================================================================

# =============================================================================
# _BLD_PATH_CHECK(VARIABLE, [PATH])
# -----------------------------------------------------------------------------
# Tests to see which distro package and distro package version provides the path
# specified in the VARIABLE,PATH arguments.  Save the path name in cache variable
# bld_cv_pkg_path_[PATHNAME], package name in cache variable bld_cv_pkg_name_-
# [PATHNAME] and the package version in cache variable bld_cv_pkg_ver_[PATHNAME],
# where, [PATHNAME] is the upper-case translated [PATH] name (or VARIABLE contents).
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_PATH_CHECK],
    [AC_REQUIRE_SHELL_FN([bld_path_check],
	[AS_FUNCTION_DESCRIBE([bld_path_check],
	    [VARIABLE, [[PATH]]],
[VARIABLE - Variable name containing the directory portion of the path.
PATH - Optional subdirectory and filename within directory])], [dnl
    if test -n "[$]2" ; then
	eval "_bld_tmp_path=\"\${[$]1}/[$]2\""
	eval "_bld_tmp_pn=\"[$]2\""
    else
	eval "_bld_tmp_path=\"\${[$]1}\""
	eval "_bld_tmp_pn=\"\${[$]1}\""
    fi
    _bld_tmp_pn=`echo "$_bld_tmp_pn" | sed -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
    _bld_tmp_pn=`echo "$_bld_tmp_pn" | sed -e 's,[[^A-Z0-9_]],_,g'`
    _bld_tmp_result=
    if test -n "$_bld_tmp_path"; then
	if test -z "$_bld_tmp_result" -a -x "`which rpm 2>/dev/null`" ; then
	    _bld_tmp_result=`rpm -q --qf '[%{NAME}]\n' --whatprovides $_bld_tmp_path 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.* is not .*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*no package provides.*||'`
	fi
	if test -z "$_bld_tmp_result" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \) ; then
	    if which dlocate >/dev/null 2>&1
	    then dlocate=dlocate; term='$'
	    else dlocate=dpkg;    term=
	    fi
	    _bld_tmp_result=`$dlocate -S "$_bld_tmp_path$term" 2>/dev/null | tail -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*not found.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|[[^:]]*$$||;s|:.*||;s|,||g'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which pacman 2>/dev/null`" ; then
	    _bld_tmp_result=`pacman -Q --owns $_bld_tmp_path 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*No package owns.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | awk '{print[$]5}'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which slackpkg 2>/dev/null`"; then
	    _bld_tmp_path2=`echo "$_bld_tmp_path" | sed -e 's,^/,,'`
	    _bld_tmp_result=`slackpkg file-search $_bld_tmp_path2 2>/dev/null | grep '^[[[] installed []]]' | head -1 | awk '{print[$]NF}'`
	    test -z "$_bld_tmp_result" || _bld_tmp_result=`echo "$_bld_tmp_result" | awk 'BEGIN{FS="-";OFS="-"}{NF=NF-3;print[$]0}'`
	fi
    fi
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_name_${_bld_tmp_pn}=\"$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_name_${_bld_tmp_pn}"
    fi
    _bld_tmp_result=
    if test -n "$_bld_tmp_path" ; then
	if test -z "$_bld_tmp_result" -a -x "`which rpm 2>/dev/null`" ; then
	    _bld_tmp_result=`rpm -q --qf '[%{VERSION}]\n' --whatprovides $_bld_tmp_path 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.* is not .*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*no package provides.*||'`
	fi
	if test -z "$_bld_tmp_result" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \) ; then
	    if which dlocate >/dev/null 2>&1
	    then dlocate=dlocate; term='$'
	    else dlocate=dpkg;    term=
	    fi
	    eval "_bld_tmp_result=\"\$bld_cv_pkg_name_${_bld_tmp_pn}\""
	    if test -n "$_bld_tmp_result" ; then
		_bld_tmp_result=`$dlocate -s "$_bld_tmp_result" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || _bld_tmp_result=
	    fi
	fi
	if test -z "$_bld_tmp_result" -a -x "`which pacman 2>/dev/null`" ; then
	    _bld_tmp_result=`pacman -Q --owns $_bld_tmp_path 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*No package owns.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | awk '{print[$]6}'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which slackpkg 2>/dev/null`"; then
	    _bld_tmp_path2=`echo "$_bld_tmp_path" | sed -e 's,^/,,'`
	    _bld_tmp_result=`slackpkg file-search $_bld_tmp_path2 2>/dev/null | grep '^[[[] installed []]]' | head -1 | awk '{print[$]NF}'`
	    test -z "$_bld_tmp_result" || _bld_tmp_result=`echo "$_bld_tmp_result" | awk 'BEGIN{FS="-"}{print[$](NF-2)}'`
	fi
    fi
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_ver_${_bld_tmp_pn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_ver_${_bld_tmp_pn}"
    fi
    _bld_tmp_result="$_bld_tmp_path"
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_path_${_bld_tmp_pn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_path_${_bld_tmp_pn}"
    fi
    eval "_bld_tmp_result=\"\$bld_cv_pkg_name_${_bld_tmp_pn}\""
    if test -n "$_bld_tmp_result" ; then
	case "$build_distro" in
	    (oracle|puias|centos|lineox|whitebox|scientific|fedora|rhel)
		eval "bld_cv_pkg_cmd_${_bld_tmp_pn}=\"yum install \$_bld_tmp_result\""
		;;
	    (suse|sle|openSUSE)
		eval "bld_cv_pkg_cmd_${_bld_tmp_pn}=\"zypper install \$_bld_tmp_result\""
		;;
	    (redhat)
		eval "bld_cv_pkg_cmd_${_bld_tmp_pn}=\"up2date install \$_bld_tmp_result\""
		;;
	    (mandrake|mandriva|manbo|mageia|mes)
		eval "bld_cv_pkg_cmd_${_bld_tmp_pn}=\"urpmi \$_bld_tmp_result\""
		;;
	    (debian|ubuntu|lts|mint)
		eval "bld_cv_pkg_cmd_${_bld_tmp_pn}=\"aptitude install \$_bld_tmp_result\""
		;;
	    (arch)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"pacman --sync \$_bld_tmp_result\""
		;;
	    (salix)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"slapt-get --install \$_bld_tmp_result\""
		;;
	    (slackware)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"slackpkg install \$_bld_tmp_result\""
		;;
	    (*)
		eval "unset bld_cv_pkg_cmd_${_bld_tmp_pn}"
		;;
	esac
    else
	eval "unset bld_cv_pkg_cmd_${_bld_tmp_pn}"
    fi])dnl
])# _BLD_PATH_CHECK
# =============================================================================

# =============================================================================
# _BLD_FILE_CHECK(VARIABLE)
# -----------------------------------------------------------------------------
# Tests to see which distro package and distro package version provides the file
# specified in variable VARIABLE.  Save the package name in cache variable
# bld_cv_pkg_name_[FILENAME] and the package version in cache variable
# bld_cv_pkg_ver_[FILENAME].  The file name itself is save in cache variable
# bld_cv_pkg_file_[FILENAME].  Where, FILENAME is the file name portion of the
# file name converted to uppercase and only containing A-Za-z0-9_.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_FILE_CHECK],
    [AC_REQUIRE_SHELL_FN([bld_file_check],
	[AS_FUNCTION_DESCRIBE([bld_file_check],
	    [FILENAME],
	    [FILENAME - Variable containing the file to check.])], [dnl
    eval "_bld_tmp_file=\"\${[$]1}\""
    _bld_tmp_fn=`basename $_bld_tmp_file`
    _bld_tmp_fn=`echo "$_bld_tmp_fn" | sed -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
    _bld_tmp_fn=`echo "$_bld_tmp_fn" | sed -e 's,[[^A-Z0-9_]],_,g'`
    # check four levels of indirection for /etc/alternatives
    test -L "$_bld_tmp_file" && _bld_tmp_file=`readlink "$_bld_tmp_file"`
    test -L "$_bld_tmp_file" && _bld_tmp_file=`readlink "$_bld_tmp_file"`
    test -L "$_bld_tmp_file" && _bld_tmp_file=`readlink "$_bld_tmp_file"`
    test -L "$_bld_tmp_file" && _bld_tmp_file=`readlink "$_bld_tmp_file"`
    _bld_tmp_result=
    if test -n "$_bld_tmp_file"; then
	if test -z "$_bld_tmp_result" -a -x "`which rpm 2>/dev/null`" ; then
	    _bld_tmp_result=`rpm -q --qf '[%{NAME}]\n' --whatprovides $_bld_tmp_file 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.* is not .*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*no package provides.*||'`
	fi
	if test -z "$_bld_tmp_result" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \) ; then
	    if which dlocate >/dev/null 2>&1
	    then dlocate=dlocate; term='$'
	    else dlocate=dpkg;    term=
	    fi
	    _bld_tmp_result=`$dlocate -S "$_bld_tmp_file$term" 2>/dev/null | tail -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*not found.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|[[^:]]*$$||;s|:.*||;s|,||g'`
	fi
	if test -z "$_bld_tmp_result" -a -x "`which pacman 2>/dev/null`" ; then
	    _bld_tmp_result=`pacman -Q --owns $_bld_tmp_file 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*No package owns.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | awk '{print[$]5}'`
	fi
    fi
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_name_${_bld_tmp_fn}=\"$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_name_${_bld_tmp_fn}"
    fi
    _bld_tmp_result=
    if test -n "$_bld_tmp_file" ; then
	if test -z "$_bld_tmp_result" -a -x "`which rpm 2>/dev/null`" ; then
	    _bld_tmp_result=`rpm -q --qf '[%{VERSION}]\n' --whatprovides $_bld_tmp_file 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.* is not .*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*no package provides.*||'`
	fi
	if test -z "$_bld_tmp_result" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \) ; then
	    if which dlocate >/dev/null 2>&1
	    then dlocate=dlocate; term='$'
	    else dlocate=dpkg;    term=
	    fi
	    eval "_bld_tmp_result=\"\$bld_cv_pkg_name_${_bld_tmp_fn}\""
	    if test -n "$_bld_tmp_result" ; then
		_bld_tmp_result=`$dlocate -s "$_bld_tmp_result" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || _bld_tmp_result=
	    fi
	fi
	if test -z "$_bld_tmp_result" -a -x "`which pacman 2>/dev/null`" ; then
	    _bld_tmp_result=`pacman -Q --owns $_bld_tmp_file 2>/dev/null | head -1`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | sed -e 's|.*No package owns.*||'`
	    _bld_tmp_result=`echo "$_bld_tmp_result" | awk '{print[$]6}'`
	fi
    fi
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_ver_${_bld_tmp_fn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_ver_${_bld_tmp_fn}"
    fi
    _bld_tmp_result="$_bld_tmp_file"
    if test -n "$_bld_tmp_result" ; then
	eval "bld_cv_pkg_path_${_bld_tmp_fn}=\"\$_bld_tmp_result\""
    else
	eval "unset bld_cv_pkg_path_${_bld_tmp_fn}"
    fi
    eval "_bld_tmp_result=\"\$bld_cv_pkg_name_${_bld_tmp_fn}\""
    if test -n "$_bld_tmp_result" ; then
	case "$build_distro" in
	    (oracle|puias|centos|lineox|whitebox|scientific|fedora|rhel)
		eval "bld_cv_pkg_cmd_${_bld_tmp_fn}=\"yum install \$_bld_tmp_result\""
		;;
	    (suse|sle|openSUSE)
		eval "bld_cv_pkg_cmd_${_bld_tmp_fn}=\"zypper install \$_bld_tmp_result\""
		;;
	    (redhat)
		eval "bld_cv_pkg_cmd_${_bld_tmp_fn}=\"up2date install \$_bld_tmp_result\""
		;;
	    (mandrake|mandriva|manbo|mageia|mes)
		eval "bld_cv_pkg_cmd_${_bld_tmp_fn}=\"urpmi \$_bld_tmp_result\""
		;;
	    (debian|ubuntu|lts|mint)
		eval "bld_cv_pkg_cmd_${_bld_tmp_fn}=\"aptitude install \$_bld_tmp_result\""
		;;
	    (arch)
		eval "bld_cv_pkg_cmd_${_bld_tmp_cn}=\"pacman --sync \$_bld_tmp_result\""
		;;
	    (*)
		eval "unset bld_cv_pkg_cmd_${_bld_tmp_fn}"
		;;
	esac
    else
	eval "unset bld_cv_pkg_cmd_${_bld_tmp_fn}"
    fi])dnl
])# _BLD_FILE_CHECK
# =============================================================================


# =============================================================================
# _BLD_VAR_PATH_PROG([VARIABLE], [PROGRAMS], [PATH], [VAR-MSG], [IF-NOT-FOUND])
# -----------------------------------------------------------------------------
# Performs AC_ARG_VAR([VARIABLE], [VAR-MSG]), and then performs a call to
# AC_PATH_PROG([VARIABLE], [PROGRAMS], [IF-NOT-FOUND], [PATH]) and examines the
# result.  It then calls bld_prog_check "VARIABLE" when successful to set
# what package name and package version provides the program (if any).
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_VAR_PATH_PROG], [dnl
    AC_REQUIRE([_BLD_PROG_CHECK])dnl
    AC_ARG_VAR([$1], [$4])dnl
    AC_PATH_PROGS([$1], [$2], [], [$3])
    if test :"${$1:-no}" = :no ; then
	m4_if([$5], [], [:], [$5])
    else
	bld_prog_check "$1"
    fi
])# _BLD_VAR_PATH_PROG
# =============================================================================

# =============================================================================
# _BLD_PATH_PROG (VARIABLE, PROG-TO-CHECK-FOR, [VALUE-IF-NOT-FOUND],
#		  [PATH = `$PATH'], [IF-NOT-FOUND], [IF-FOUND])
# -----------------------------------------------------------------------------
# Like `AC_PATH_PROG', but also provides an action to take if not found.  Also,
# when the program is found, the package containing the message is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_PATH_PROG], [dnl
    AC_REQUIRE([_BLD_PROG_CHECK])dnl
    AC_PATH_PROG([$1], [$2], [], [$4])
    if test :"${$1:-no}" = :no ; then
	$1="$3"
	m4_if([$5], [], [:], [$5])
    else
	bld_prog_check "$1"
	m4_if([$6], [], [:], [$6])
    fi
])# _BLD_PATH_PROG
# =============================================================================

# =============================================================================
# _BLD_PATH_PROGS (VARIABLE, PROGS-TO-CHECK-FOR, [VALUE-IF-NOT-FOUND],
#		   [PATH = `$PATH'], [IF-NOT-FOUND], [IF-FOUND])
# -----------------------------------------------------------------------------
# Like `AC_PATH_PROGS', but also provides an action to take if not found.  Also,
# when the program is found, the package containing the message is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_PATH_PROGS], [dnl
    AC_REQUIRE([_BLD_PROG_CHECK])dnl
    AC_PATH_PROGS([$1], [$2], [], [$4])
    if test :"${$1:-no}" = :no ; then
	$1="$3"
	m4_if([$5], [], [:], [$5])
    else
	bld_prog_check "$1"
	m4_if([$6], [], [:], [$6])
    fi
])# _BLD_PATH_PROGS
# =============================================================================

# =============================================================================
# _BLD_INSTALL_WARN (VARIABLE, MESSAGE-LEAD, GENERIC-INSTALL-MESSAGE,
#		     MESSAGE-TAIL)
# -----------------------------------------------------------------------------
# To be used in the IF-NOT-FOUND portion of _BLD_PATH_PROG, _BLD_PATH_PROGS,
# _BLD_CHECK_HEADER or _BLD_CHECK_HEADERS.  If there is a cached way to install
# the program or header specified by VARIABLE, warn with the MESSAGE-LEAD, the
# specific installation message, and the MESSAGE-TAIL; otherwise warn with the
# MESSAGE-LEAD, the GENERIC-INSTALL-MESSAGE and the MESSAGE-TAIL.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_INSTALL_WARN], [dnl
    _bld_tmp_var="bld_cv_pkg_cmd_$1"
    eval "_bld_tmp_val=\"\$$_bld_tmp_var\""
    if test -n "$_bld_tmp_val" ; then
	_bld_tmp_msg="
*** $build_distro: $_bld_tmp_val"
    else
	_bld_tmp_msg="$3"
    fi
    _bld_tmp_msg="$2$_bld_tmp_msg$4"
    AC_MSG_WARN([$_bld_tmp_msg])
])# _BLD_INSTALL_WARN
# =============================================================================

# =============================================================================
# _BLD_INSTALL_ERROR (VARIABLE, MESSAGE-LEAD, GENERIC-INSTALL-MESSAGE,
#		      MESSAGE-TAIL)
# -----------------------------------------------------------------------------
# To be used in the IF-NOT-FOUND portion of _BLD_PATH_PROG, _BLD_PATH_PROGS,
# _BLD_CHECK_HEADER or _BLD_CHECK_HEADERS.  If there is a cached way to install
# the program specified by VARIABLE, error with the MESSAGE-LEAD, the specific
# installation message, and the MESSAGE-TAIL; otherwise error with the
# MESSAGE-LEAD, the GENERIC-INSTALL-MESSAGE and the MESSAGE-TAIL.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_INSTALL_ERROR], [dnl
    _bld_tmp_var="bld_cv_pkg_cmd_$1"
    eval "_bld_tmp_val=\"\$$_bld_tmp_var\""
    if test -n "$_bld_tmp_val" ; then
	_bld_tmp_msg="
*** $build_distro: $_bld_tmp_val"
    else
	_bld_tmp_msg="$3"
    fi
    _bld_tmp_msg="$2$_bld_tmp_msg$4"
    AC_MSG_ERROR([$_bld_tmp_msg])
])# _BLD_INSTALL_ERROR
# =============================================================================

# =============================================================================
# _BLD_FIND_DIR (DESC, VARIABLE, SEARCH-PATH, [FILE-PATH], [VALUE-IF-NOT-FOUND],
#		 [IF-NOT-FOUND], [IF-FOUND], [OPTION-VARIABLE], [TEST])
# -----------------------------------------------------------------------------
# Finds a directory by searching the directories specified in SEARCH-PATH for
# one that exists.  When [FILE-PATH] is specified, the specified subdirectory
# and file must exist relative to the searched directory, otherwise it is not
# considered a valid result and the search continues.  [VALUE-IF-NOT-FOUND]
# specifies the value to use for the directory and assign to VARIABLE when the
# directory is not found; otherwise, the value 'no' will be assigned to
# VARIABLE.  When [IF-NOT-FOUND] is specified it will be executed when a
# suitable directory was not found, [IF-FOUND] will be executed when a suitable
# directory was found and the value of the found directory will be assigned to
# VARIABLE.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_FIND_DIR], [dnl
    AC_REQUIRE([_BLD_PATH_CHECK])dnl
    AC_CACHE_CHECK([$1], [$2]_eval, [dnl
	m4_if([$8], [], [],
[	case "${$8:-search}" in
	    (no) $2=no ;;
	    (yes|search) ;;
	    (*) m4_if([$4], [],
		[if test -d "${$8}" ; then $2="${$8}" ; fi ;;],
		[if test -f "${$8}/$4" ; then $2="${$8}" ; fi ;;])
	esac])
	if test -z "${$2}" ; then
	    eval "bld_search_path=\"$3\""
	    bld_search_path=`echo "$bld_search_path" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g" | awk '{if(!([$]0 in seen)){print[$]0;seen[[$ 0]]=1}}'`
	    AC_MSG_RESULT([searching])
	    for bld_dir in $bld_search_path ; do
		AC_MSG_CHECKING([for $1... $bld_dir])
		if test -d "$bld_dir" $9; then
		    m4_if([$4], [],
    [		if test -z "$rootdir" -o "${bld_dir#$rootdir}" = "$bld_dir"
		then
		    $2="$bld_dir"
		else
		    $2='${rootdir}'"${bld_dir#$rootdir}"
		fi
		    AC_MSG_RESULT([yes])
		    break],
    [		bld_files=`find $bld_dir -follow -type f -name $(basename '$4') 2>/dev/null | grep -F '$4' | sort -ru`
		    for bld_file in $bld_files ; do
			test -r "$bld_file" || continue
			bld_dir="${bld_file%/$4}"
			test -d "$bld_dir" $9 || continue
			if test -z "$rootdir" -o "${bld_dir#$rootdir}" = "${bld_dir}"
			then
			    $2="${bld_dir}"
			else
			    $2='${rootdir}'"${bld_dir#$rootdir}"
			fi
			AC_MSG_RESULT([yes])
			break 2
		    done])
		fi
		AC_MSG_RESULT([no])
	    done
	    if test -z "${$2}" ; then
		$2="$5"
		m4_if([$6], [], [:], [$6])
		eval "[$2]_eval=\"${$2}\""
	    else
		eval "[$2]_eval=\"${$2}\""
		bld_path_check "[$2]_eval" "$4"
		m4_if([$7], [], [:], [$7])
	    fi
	    AC_MSG_CHECKING([for $1])
	fi
    ])
])# _BLD_FIND_DIR
# =============================================================================

# =============================================================================
# _BLD_FIND_FILE (DESC, VARIABLE, SEARCH-PATH, [VALUE-IF-NOT-FOUND],
#		  [IF-NOT-FOUND], [IF-FOUND], [OPTION-VARIABLE])
# -----------------------------------------------------------------------------
# Finds a directory by searching the full path filenames specified in
# SEARCH-PATH for one that exists.  [VALUE-IF-NOT-FOUND] specifies the value to
# use for the file and assign to VARIABLE when the file is not found; otherwise,
# the value 'no' will be assigned to VARIABLE.  When [IF-NOT-FOUND] is specified
# it will be executed when the file was not found, [IF-FOUND] will be executed
# when the file was found and the path of the found file will be assigned to
# VARIABLE.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_FIND_FILE], [dnl
    AC_REQUIRE([_BLD_FILE_CHECK])dnl
    AC_CACHE_CHECK([$1], [$2]_eval, [dnl
	m4_if([$7], [], [],
[	case "${$7:-search}" in
	    (no) $2=no ;;
	    (yes|search) ;;
	    (*) if test -f "${$7}" ; then $2="${$7}"; fi ;;
	esac])
	if test -z "${$2}" ; then
	    eval "bld_search_path=\"$3\""
	    bld_search_path=`echo "$bld_search_path" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g" | awk '{if(!([$]0 in seen)){print[$]0;seen[[$ 0]]=1}}'`
	    AC_MSG_RESULT([searching])
	    for bld_file in $bld_search_path ; do
		AC_MSG_CHECKING([for $1... $bld_file])
		if test -f "$bld_file" ; then
		    if test -z "$rootdir" -o "${bld_file#$rootdir}" = "${bld_file}"
		    then
			$2="${bld_file}"
		    else
			$2='${rootdir}'"${bld_file#$rootdir}"
		    fi
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	    if test -z "${$2}" ; then
		$2="$4"
		m4_if([$5], [], [:], [$5])
		eval "[$2]_eval=\"${$2}\""
	    else
		eval "[$2]_eval=\"${$2}\""
		bld_file_check "[$2]_eval"
		m4_if([$6], [], [:], [$6])
	    fi
	    AC_MSG_CHECKING([for $1])
	fi
    ])
])# _BLD_FIND_FILE
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
# -----------------------------------------------------------------------------
# Like `AC_CHECK_HEADER', but when the header is found, the package providing
# the header is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_CHECK_HEADER], [dnl
])# _BLD_CHECK_HEADER
# =============================================================================

# =============================================================================
# _BLD_CHECK_HEADERS (HEADER..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#		      [INCLUDES])
# -----------------------------------------------------------------------------
# Like `AC_CHECK_HEADERS', but when each header is found, the package providing
# the header is cached.
# -----------------------------------------------------------------------------
AC_DEFUN([_BLD_CHECK_HEADERS], [dnl
])# _BLD_CHECK_HEADERS
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
# Revision 1.1.2.11  2011-09-20 09:51:32  brian
# - updates from git
#
# Revision 1.1.2.10  2011-08-07 11:14:37  brian
# - mostly mandriva and ubuntu build updates
#
# Revision 1.1.2.9  2011-07-27 07:52:18  brian
# - work to support Mageia/Mandriva compressed kernel modules and URPMI repo
#
# Revision 1.1.2.8  2011-06-09 11:30:54  brian
# - support mageia and mes
#
# Revision 1.1.2.7  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.6  2011-05-10 13:45:34  brian
# - weak modules workup
#
# Revision 1.1.2.5  2011-02-28 19:51:29  brian
# - better repository build
#
# Revision 1.1.2.4  2011-02-17 18:34:10  brian
# - repository and rpm build updates
#
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
