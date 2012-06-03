# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: init.m4,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2011-05-31 09:46:01 $
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
# Last Modified $Date: 2011-05-31 09:46:01 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS], [dnl
    AC_MSG_NOTICE([+-------------------------+])
    AC_MSG_NOTICE([| SYSV Init Script Checks |])
    AC_MSG_NOTICE([+-------------------------+])
    _INIT_SCRIPTS_OPTIONS
    _INIT_SCRIPTS_SETUP
    _INIT_SCRIPTS_OUTPUT
])# _INIT_SCRIPTS
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_OPTIONS], [dnl
    AC_ARG_ENABLE([initscripts],
	[AS_HELP_STRING([--disable-initscripts],
	    [installation of init scripts @<:@default=enabled@:>@])],
	[], [enable_initscripts=yes])
])# _INIT_SCRIPTS_OPTIONS
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SETUP], [dnl
dnl
dnl first find the inittab: we can always figure out the initial
dnl init script from the inittab
dnl
    _BLD_FIND_FILE([for init SysV inittab], [init_cv_inittab], [
	    ${sysconfdir}/inittab], [no])
    AC_ARG_WITH([initial_init_script],
	[AS_HELP_STRING([--with-initial-init-script=PATH],
	    [initial system init script @<:@default=search@:>@])], [], [dnl
dnl 
dnl check for the initial init script from inittab
dnl 
	with_initial_init_script=no
	if test :"$init_cv_inittab" != :no ; then
	    init_tmp="$(< $init_cv_inittab | grep -c1 '^si::sysinit:' | sed -e 's|^si::sysinit:||;s|[[[:space:]]].*||')"
	    init_tmp=`echo "${rootdir}$init_tmp" | sed -e 's|\<NONE\>||g;s|//|/|g' | awk '{if(!([$]0 in seen)){print[$]0;seen[[$ 0]]=1}}'`
	    if test -f "$init_tmp" ; then
		with_initial_init_script="$init_tmp"
	    fi
	fi])
dnl
dnl fallback is to go looking for it in the usual places
dnl
    _BLD_FIND_FILE([for init SysV script], [init_cv_script], [dnl
	    ${sysconfdir}/init.d/rcS
	    ${sysconfdir}/rc.d/rc.sysinit], [no], [], [], [with_initial_init_script])
    _BLD_FIND_DIR([for init SysV rcS.d directory], [init_cv_rcs_dir], [
	    ${sysconfdir}/rcS.d], [], [no])
    _BLD_FIND_DIR([for init SysV rc.d directory], [init_cv_rc_dir], [
	    ${sysconfdir}/rc.d], [], [no])
dnl
dnl This script needs to exist to add to /etc/modules successfully
dnl
    _BLD_FIND_FILE([for init SysV rc.modules script], [init_cv_rc_modules], [
	    ${sysconfdir}/rc.d/rc.modules], [no])
dnl
dnl This is where we are going to add preloaded modules
dnl
    _BLD_FIND_FILE([for init SysV modules file], [init_cv_modules], [
	    ${sysconfdir}/modules], [no])
dnl
dnl This is where we are going to have to generate symbolic links if chkconfig
dnl does not exist
dnl
    _BLD_FIND_DIR([for init SysV rcX.d directory], [init_cv_rcx_dir], [
	    ${sysconfdir}/rc.d/rc[[S0-6]].d
	    ${sysconfdir}/rc[[S0-6]].d], [], [no])
dnl
dnl I suppose we really don't care about these.  It is funny, though, that SuSE
dnl used to use insserv (SuSE wrote it!), but now, SuSE uses service.  Debian
dnl used to use update-rc.d, but now uses insserv!
dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    AC_ARG_VAR([CHKCONFIG],
	       [Chkconfig command. @<:@default=chkconfig@:>@])
    _BLD_PATH_PROG([CHKCONFIG], [chkconfig], [], [$tmp_path], [dnl
	AC_MSG_WARN([Cannot find chkconfig program in PATH.])])
    AC_ARG_VAR([INSSERV],
	       [Insserv command. @<:@default=inserv@:>@])
    _BLD_PATH_PROG([INSSERV], [insserv], [], [$tmp_path], [dnl
	AC_MSG_WARN([Cannot find insserv program in PATH.])])
dnl
dnl initrddir is where we are going to put init scripts
dnl
    _BLD_FIND_DIR([for init SysV init.d directory], [init_cv_initrddir], [
	    ${sysconfdir}/rc.d/init.d
	    ${sysconfdir}/init.d
	    ${sysconfdir}/rc.d], [], [no], [], [], [], [-a ! -L "$bld_dir"])
dnl
dnl configdir is where we are going to put init script default files
dnl
    _BLD_FIND_DIR([for init SysV config directory], [init_cv_configdir], [
	    ${sysconfdir}/sysconfig
	    ${sysconfdir}/conf.d
	    ${sysconfdir}/default], [], [no])
    AC_CACHE_CHECK([for init SysV installation], [init_cv_install], [dnl
	init_cv_install='yes'
	test :"${enable_initscripts:-yes}" = :no && init_cv_install='no'
    ])
])# _INIT_SCRIPTS_SETUP
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_OUTPUT], [dnl
    AM_CONDITIONAL([INSTALL_INITSCRIPTS], [test :"${init_cv_install:-yes}" = :yes])dnl
    AM_CONDITIONAL([WITH_RCSD_DIRECTORY], [test :${init_cv_rcs_dir:-no} != :no])dnl
dnl
dnl initrddir is where we are going to put init scripts
dnl
    if test :"${init_cv_initrddir:-no}" != :no ; then
	initrddir="${init_cv_initrddir}"
    else
	if test :${init_cv_rcs_dir:-no} = :no ; then
dnl         redhat style
	    initrddir='${sysconfdir}/rc.d/init.d'
	else
dnl         debian style
	    initrddir='${sysconfdir}/init.d'
	fi
    fi
    AC_SUBST([initrddir])
dnl
dnl configdir is where we are going to put init script default files
dnl
    if test :"${init_cv_configdir:-no}" != :no ; then
	configdir="${init_cv_configdir}"
    else
	if test :${init_cv_rcs_dir:-no} = :no ; then
dnl         redhat style
	    configdir='${sysconfdir}/sysconfig'
	else
dnl         debian style
	    configdir='${sysconfdir}/default'
	fi
    fi
    AC_SUBST([configdir])
])# _INIT_SCRIPTS_OUTPUT
# =============================================================================

# =============================================================================
# _INIT_
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_], [dnl
])# _INIT_
# =============================================================================

# =============================================================================
#
# $Log: init.m4,v $
# Revision 1.1.2.5  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.4  2011-05-10 13:45:35  brian
# - weak modules workup
#
# Revision 1.1.2.3  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.2  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# Revision 0.9.2.18  2008-09-21 07:40:46  brian
# - add defaults to environment variables
#
# Revision 0.9.2.17  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.16  2007/10/17 20:00:27  brian
# - slightly different path checks
#
# Revision 0.9.2.15  2007/08/12 19:05:30  brian
# - rearrange and update headers
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
