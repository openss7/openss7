# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) File: m4/init.m4
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
# _INIT_SCRIPTS
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS], [dnl
    _INIT_SCRIPTS_ARGS
    if test :${enable_tools:-yes} = :yes ; then
	AC_MSG_NOTICE([+--------------------+])
	AC_MSG_NOTICE([| Init Script Checks |])
	AC_MSG_NOTICE([+--------------------+])
	_INIT_SCRIPTS_SETUP
	_INIT_SCRIPTS_OPTIONS
    fi
    _INIT_SCRIPTS_OUTPUT
])# _INIT_SCRIPTS
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_ARGS
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_ARGS], [dnl
    AC_ARG_ENABLE([initscripts],
	[AS_HELP_STRING([--disable-initscripts],
	    [installation of init scripts @<:@default=auto@:>@])],
	[], [enable_initscripts=yes])
    AC_ARG_ENABLE([sysvinit],
	[AS_HELP_STRING([--disable-sysvinit],
	    [installation of sysvinit scripts @<:@default=auto@:>@])],
	[], [enable_sysvinit=yes])
    AC_ARG_ENABLE([systemd],
	[AS_HELP_STRING([--disable-systemd],
	    [installation of systemd units @<:@default-auto@:>@])],
	[], [enable_systemd=yes])
])# _INIT_SCRIPTS_ARGS
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_SYSVINIT_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SYSVINIT_SETUP], [dnl
dnl
dnl first find the inittab: we can always figure out the initial
dnl init script from the inittab
dnl
    disable_sysvinit=
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
	    init_tmp=`echo "${rootdir}$init_tmp" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g" | awk '{if(!([$]0 in seen)){print[$]0;seen[[$ 0]]=1}}'`
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
    tmp_PATH="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    _BLD_VAR_PATH_PROG([CHKCONFIG], [chkconfig], [$tmp_PATH],
	    [Chkconfig command. @<:@default=chkconfig@:>@])
    _BLD_VAR_PATH_PROG([INSSERV], [insserv], [$tmp_PATH],
	    [Insserv command. @<:@default=inserv@:>@])
dnl
dnl initrddir is where we are going to put init scripts
dnl
    _BLD_FIND_DIR([for init SysV init.d directory], [init_cv_initrddir], [
	    ${sysconfdir}/rc.d/init.d
	    ${sysconfdir}/init.d
	    ${sysconfdir}/rc.d], [], [no], [dnl
	    disable_sysvinit=yes], [], [], [-a ! -L "$bld_dir"])
dnl
dnl configdir is where we are going to put init script default files
dnl
    _BLD_FIND_DIR([for init SysV config directory], [init_cv_configdir], [
	    ${sysconfdir}/sysconfig
	    ${sysconfdir}/conf.d
	    ${sysconfdir}/default], [], [no], [dnl
	    disable_sysvinit=yes])
])# _INIT_SCRIPTS_SYSVINIT_SETUP
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_SYSTEMD_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SYSTEMD_SETUP], [dnl
dnl 
dnl This is where we need to add specfs as a early-boot module load.
dnl
    disable_systemd=
    _BLD_FIND_DIR([for modprobe.d directory], [init_cv_modprobed], [
	    ${rootdir}/usr/lib/modprobe.d
	    ${rootdir}/lib/modprobe.d
	    ${rootdir}/etc/modprobe.d], [], [no])
    _BLD_FIND_DIR([for systemd modules-load.d directory], [init_cv_modulesloadd], [
	    ${rootdir}/usr/lib/modules-load.d
	    ${rootdir}/lib/modules-load.d
	    ${rootdir}/etc/modules-load.d], [], [no])
    _BLD_FIND_DIR([for systemd sysctl.d directory], [init_cv_sysctld], [
	    ${rootdir}/usr/lib/sysctl.d
	    ${rootdir}/lib/sysctl.d
	    ${rootdir}/etc/sysctl.d], [], [no])
    _BLD_FIND_DIR([for systemd directory], [init_cv_systemd_dir], [
	    ${rootdir}/usr/lib/systemd
	    ${rootdir}/lib/systemd
	    ${rootdir}/etc/systemd], [], [no], [dnl
	    disable_systemd=yes])
    tmp_PATH="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    _BLD_VAR_PATH_PROG([SYSTEMCTL], [systemctl], [$tmp_PATH],
	    [Systemctl command. @<:@default=systemctl@:>@],
	    [disable_systemd=yes])
])# _INIT_SCRIPTS_SYSTEMD_SETUP
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SETUP], [dnl
    _INIT_SCRIPTS_SYSVINIT_SETUP
    _INIT_SCRIPTS_SYSTEMD_SETUP
])# _INIT_SCRIPTS_SETUP
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_OPTIONS], [dnl
    AC_MSG_CHECKING([for init SysV installation])
    if test :${enable_sysvinit:-yes} = :yes -a :$disable_sysvinit = :yes ; then
	enable_sysvinit=disabled
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_sysvinit --disable-sysvinit\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-sysvinit'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-sysvinit'"
    fi
    AC_MSG_RESULT([${enable_sysvinit:-yes}])
    AC_MSG_CHECKING([for init systemd installation])
    if test :${enable_systemd:-yes} = :yes -a :$disable_systemd = :yes ; then
	enable_systemd=disabled
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_systemd --disable-systemd\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-systemd'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-systemd'"
    fi
    disable_initscripts=
    if test :${enable_sysvinit:-yes} != :yes -a :${enable_systemd:-yes} != :yes ; then
	disable_initscripts=yes
	AC_MSG_WARN([Neither SysV Init nor systemd scripts can be installed.])
    fi
    AC_MSG_RESULT([${enable_systemd:-yes}])
    AC_MSG_CHECKING([for init installation])
    if test :${enable_initscripts:-yes} = :yes -a :$disable_initscripts = :yes ; then
	enable_initscripts=disabled
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_initscripts --disable-initscripts\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-initscripts'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-initscripts'"
    fi
    AC_MSG_RESULT([${enable_initscripts:-yes}])
])# _INIT_SCRIPTS_OPTIONS
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_SYSVINIT_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SYSVINIT_OUTPUT], [dnl
    AM_CONDITIONAL([INSTALL_SYSVINIT],    [test :"${enable_sysvinit:-yes}" = :yes])dnl
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
])# _INIT_SCRIPTS_SYSVINIT_OUTPUT
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_SYSTEMD_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SYSTEMD_OUTPUT], [dnl
    AM_CONDITIONAL([INSTALL_SYSTEMD], [test :"${enable_systemd:-yes}" = :yes])dnl
dnl 
dnl modprobe.d is where we are going to put specfs module install hook files
dnl 
    if test :"${init_cv_modprobed:-no}" != :no ; then
	modprobeddir="${init_cv_modprobed}"
    else
	modprobeddir='${sysconfdir}/modprobe.d'
    fi
    AC_SUBST([modprobeddir])
dnl 
dnl modulesloadddir is where we are going to put modules load config files
dnl 
    if test :"${init_cv_modulesloadd:-no}" != :no ; then
	modulesloadddir="${init_cv_modulesloadd}"
    else
	modulesloadddir='${sysconfdir}/modules-load.d'
    fi
    AC_SUBST([modulesloadddir])
dnl 
dnl sysctlddir is where we are going to put sysctl config files
dnl 
    if test :"${init_cv_sysctld:-no}" != :no ; then
	sysctlddir="${init_cv_sysctld}"
    else
	sysctlddir='${sysconfdir}/sysctl.d'
    fi
    AC_SUBST([sysctlddir])
dnl 
dnl systemddir is where we are going to put systemd unit files
dnl 
    if test :"${init_cv_systemd_dir:-no}" != :no ; then
	systemddir="${init_cv_systemd_dir}"
    else
	systemddir='${sysconfdir}/systemd'
    fi
    systemdsysdir='${systemddir}/system'
    AC_SUBST([systemddir])
    AC_SUBST([systemdsysdir])
])# _INIT_SCRIPTS_SYSTEMD_OUTPUT
# =============================================================================

# =============================================================================
# _INIT_SCRIPTS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_OUTPUT], [dnl
    AM_CONDITIONAL([INSTALL_INITSCRIPTS], [test :"${enable_initscripts:-yes}" = :yes])dnl
    _INIT_SCRIPTS_SYSVINIT_OUTPUT
    _INIT_SCRIPTS_SYSTEMD_OUTPUT
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
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
