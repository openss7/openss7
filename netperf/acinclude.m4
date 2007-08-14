# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.30 $) $Date: 2007/08/14 07:26:44 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/08/14 07:26:44 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/public.m4])
m4_include([m4/streams.m4])
m4_include([m4/strcomp.m4])
m4_include([m4/xti.m4])
m4_include([m4/xns.m4])
m4_include([m4/inet.m4])
m4_include([m4/sctp.m4])
m4_include([m4/man.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])

# =============================================================================
# AC_NETPERF
# -----------------------------------------------------------------------------
AC_DEFUN([AC_NETPERF], [dnl
    _OPENSS7_PACKAGE([NETPERF], [OpenSS7 NETPERF Utility])
    _NETPERF_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _DEB_DPKG
    _LINUX_STREAMS
    if test :"${streams_cv_package:-no}" = :no ; then
	if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" != :no ; then
	    AC_MSG_ERROR([
****
**** You have specified --with-lis or --with-lfs, yet I cannot find your
**** STREAMS include directories.  STREAMS include directories are necessary
**** to compile the package when --with-lis or --with-lfs is specified.
**** Try specifying the include directories with --with-lis=DIRECTORY or
**** --with-lfs=DIRECTORY and try again.
****
	    ])
	fi
    else
	if test :"${with_compat:-auto}" != :no ; then
	    _STRCOMP
	    if test :"${with_xns:-auto}" != :no ; then
		_XNS
	    fi
	fi
	if test :"${xns_cv_includes:-no}" = :no ; then
	    if test  :"${with_xns:-no}" != :no ; then
		AC_MSG_ERROR([
****
**** You have specified --with-xns, yet I cannot find your XNS include
**** directories.  XNS include directories are necessary to compile the
**** package when --with-xns is specified.  Try specifying the include
**** directory with --with-xns=DIRECTORY and try again.
****
		])
	    fi
	fi
	if test :"${with_xns:-auto}" != :no ; then
	    _XTI
	fi
	if test :"${xti_cv_includes:-no}" = :no ; then
	    if test :"${with_xti:-no}" != :no ; then
		AC_MSG_ERROR([
**** 
**** You have specified --with-xti, yet I cannot find your XTI include
**** directories.  XTI include directories are necessary to compile the
**** package when --with-xti is specified.  Try specifying the include
**** directory with --with-xti=DIRECTORY and try again.
**** 
		])
	    fi
	else
	    if test :"${with_inet:-auto}" != :no ; then
		_INET
	    fi
	    if test :"${inet_cv_includes:-no}" = :no ; then
		if test :"${with_inet:-no}" != :no ; then
		    AC_MSG_ERROR([
**** 
**** You have specified --with-inet, yet I cannot find your INET include
**** directories.  INET include directories are necessary to compile the
**** package when --with-inet is specified.  Try specifying the include
**** directory with --with-inet=DIRECTORY and try again.
**** 
		    ])
		fi
	    fi
	fi
    fi
    if test :"${with_sctp:-auto}" != :no ; then
	_SCTP
    fi
    if test :"${sctp_cv_includes:-no}" = :no ; then
	if test :"${with_sctp:-no}" != :no ; then
	    AC_MSG_ERROR([
**** 
**** You have specified --with-sctp, yet I cannot find your SCTP include
**** directories.  SCTP include directories are necessary to compile the
**** package when --with-sctp is specified.  Try specifying the include
**** directory with --with-sctp=DIRECTORY and try again.
**** 
	    ])
	fi
    fi
    CPPFLAGS="-include ./config.h"
    CPPFLAGS="${CPPFLAGS}${XTI_CPPFLAGS:+ $XTI_CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS}${INET_CPPFLAGS:+ $INET_CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS}${SCTP_CPPFLAGS:+ $SCTP_CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS}${XNS_CPPFLAGS:+ $XNS_CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS}${STRCOMP_CPPFLAGS:+ $STRCOMP_CPPFLAGS}"
    CPPFLAGS="${CPPFLAGS}${STREAMS_CPPFLAGS:+ $STREAMS_CPPFLAGS}"
    NETPERF_LDADD="$STREAMS_LDADD $XTI_LDADD"
    AC_SUBST([NETPERF_LDADD])
    _NETPERF_OUTPUT
])# AC_NETPERF
# =============================================================================

# =============================================================================
# _NETPERF_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_NETPERF_OPTIONS], [dnl
    AC_ARG_ENABLE([netperf-dirty],
	AS_HELP_STRING([--disable-netperf-dirty],
	    [disable code to dirty buffers before calls to send
	     @<:@default=enabled@:>@]),
	[netperf_cv_dirty="$enableval"],
	[netperf_cv_dirty='yes'])
    AC_ARG_ENABLE([netperf-histogram],
	AS_HELP_STRING([--disable-netperf-histogram],
	    [disable code to to keep a histogram of r/r times or time spent in
	     send() @<:@default=enabled@:>@]),
	[netperf_cv_histogram="$enableval"],
	[netperf_cv_histogram='yes'])
    AC_ARG_ENABLE([netperf-old-histogram],
	AS_HELP_STRING([--enable-netperf-old-histogram],
	    [enable old pre-2.2pl6 formatted histogram
	     @<:@default=disabled@:>@]),
	[netperf_cv_old_histogram="$enableval"],
	[netperf_cv_old_histogram='no'])
    AC_ARG_ENABLE([netperf-intervals],
	AS_HELP_STRING([--disable-netperf-intervals],
	    [disable code to allow pacing of sends in a UDP, TCP or SCTP test.
	     @<:@default=enabled@:>@]),
	[netperf_cv_intervals="$enableval"],
	[netperf_cv_intervals='yes'])
    AC_ARG_ENABLE([netperf-do-dlpi],
	AS_HELP_STRING([--disable-netperf-do-dlpi],
	    [disable code to test the DLPI implementation
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_dlpi="$enableval"],
	[netperf_cv_do_dlpi='yes'])
    AC_ARG_ENABLE([netperf-do-sctp],
	AS_HELP_STRING([--disable-netperf-do-sctp],
	    [disable code to test the SCTP implementation
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_sctp="$enableval"],
	[netperf_cv_do_sctp='yes'])
    AC_ARG_ENABLE([netperf-do-select],
	AS_HELP_STRING([--disable-netperf-do-select],
	    [disable code to perform select() on receive
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_select="$enableval"],
	[netperf_cv_do_select='yes'])
    AC_ARG_ENABLE([netperf-do-lwp],
	AS_HELP_STRING([--enable-netperf-do-lwp],
	    [enable code for light weight processes
	     @<:@default=disabled@:>@]),
	[netperf_cv_do_lwp="$enableval"],
	[netperf_cv_do_lwp='no'])
    AC_ARG_ENABLE([netperf-do-nbrr],
	AS_HELP_STRING([--disable-netperf-do-nbrr],
	    [disable code for non-blocking request/response
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_nbrr="$enableval"],
	[netperf_cv_do_nbrr='yes'])
    AC_ARG_ENABLE([netperf-do-xti],
	AS_HELP_STRING([--disable-netperf-do-xti],
	    [disable code to test the XTI implementation
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_xti="$enableval"],
	[netperf_cv_do_xti='yes'])
    AC_ARG_ENABLE([netperf-do-xti-sctp],
	AS_HELP_STRING([--disable-netperf-do-xti-sctp],
	    [disable code to test the XTI SCTP implementation
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_xti_sctp="$enableval"],
	[netperf_cv_do_xti_sctp='yes'])
    AC_ARG_ENABLE([netperf-do-unix],
	AS_HELP_STRING([--disable-netperf-do-unix],
	    [disable code to test the Unix domain implementation
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_unix="$enableval"],
	[netperf_cv_do_unix='yes'])
    AC_ARG_ENABLE([netperf-do-1644],
	AS_HELP_STRING([--disable-netperf-do-1644],
	    [disable code to test the transactions
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_1644="$enableval"],
	[netperf_cv_do_1644='yes'])
    AC_ARG_ENABLE([netperf-do-first-burst],
	AS_HELP_STRING([--disable-netperf-do-first-burst],
	    [disable first burst code
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_first_burst="$enableval"],
	[netperf_cv_do_first_burst='yes'])
    AC_ARG_WITH([netperf-debug-log-file],
	AS_HELP_STRING([--with-netperf-debug-log-file=LOGFILE],
	    [specify the LOGFILE to which debug output is written
	     @<:@default='/tmp/netperf.debug'@:>@]),
	[netperf_cv_debug_log_file="$enableval"],
	[netperf_cv_debug_log_file='/tmp/netperf.debug'])
    AC_ARG_ENABLE([netperf-use-looper],
	AS_HELP_STRING([--enable-netperf-use-looper],
	    [enable looper or soaker processes to measure CPU utilitization
	     @<:@default=disabled@:>@]),
	[netperf_cv_use_looper="$enableval"],
	[netperf_cv_use_looper='no'])
    AC_ARG_ENABLE([netperf-use-pstat],
	AS_HELP_STRING([--enable-netperf-use-pstat],
	    [enable CPU utilization measurements with pstat()
	     @<:@default=disabled@:>@]),
	[netperf_cv_use_pstat="$enableval"],
	[netperf_cv_use_pstat="${ac_cv_func_pstat:-no}"])
    AC_ARG_ENABLE([netperf-use-kstat],
	AS_HELP_STRING([--enable-netperf-use-kstat],
	    [enable CPU utilization measurements with kstat()
	     @<:@default=disabled@:>@]),
	[netperf_cv_use_kstat="$enableval"],
	[netperf_cv_use_kstat="${ac_cv_func_kstat:-no}"])
    AC_ARG_ENABLE([netperf-use-proc-stat],
	AS_HELP_STRING([--disable-netperf-use-proc-stat],
	    [disable CPU utilization measurements with /proc/stat()
	     @<:@default=enabled@:>@]),
	[netperf_cv_use_proc_stat="$enableval"],
	[netperf_cv_use_proc_stat='yes'])
    AC_ARG_ENABLE([netperf-do-ipv6],
	AS_HELP_STRING([--enable-netperf-do-ipv6],
	    [enable tests using socket interface to IPV6
	     @<:@default=disabled@:>@]),
	[netperf_cv_do_ipv6="$enableval"],
	[netperf_cv_do_ipv6='no'])
    AC_ARG_ENABLE([netperf-do-dns],
	AS_HELP_STRING([--disable-netperf-do-dns],
	    [disable tests that measure the performance of a DNS server
	     @<:@default=enabled@:>@]),
	[netperf_cv_do_dns="$enableval"],
	[netperf_cv_do_dns='yes'])
    AC_ARG_ENABLE([netperf-use-sysctl],
	AS_HELP_STRING([--enable-netperf-use-sysctl],
	    [enable CPU utilization measurements with sysctl()
	     @<:@default=disabled@:>@]),
	[netperf_cv_use_sysctl="$enableval"],
	[netperf_cv_use_sysctl='no'])
    AC_ARG_ENABLE([netperf-use-perfstat],
	AS_HELP_STRING([--enable-netperf-use-perfstat],
	    [enable CPU utilization measurements with perfstat()
	     @<:@default=disabled@:>@]),
	[netperf_cv_use_perfstat="$enableval"],
	[netperf_cv_use_perfstat="${ac_cv_func_perfstat:-no}"])
    AC_ARG_ENABLE([netperf-dont-wait],
	AS_HELP_STRING([--enable-netperf-dont-wait],
	    [enable to not wait for children to exit
	     @<:@default=disabled@:>@]),
	[netperf_cv_dont_wait="$enableval"],
	[netperf_cv_dont_wait='no'])
])# _NETPERF_OPTIONS
# =============================================================================

# =============================================================================
# _NETPERF_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_NETPERF_OUTPUT], [dnl
    AC_MSG_CHECKING([for netperf dirty])
    if test :"${netperf_cv_dirty:-no}" = :yes ; then
	AC_DEFINE([DIRTY], [1], [
	  Define to enable code to dirty buffers before calls to send.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_dirty:-no}])
    AC_MSG_CHECKING([for netperf histogram])
    if test :"${netperf_cv_histogram:-no}" = :yes ; then
	AC_DEFINE([HISTOGRAM], [1], [
	    Define to enable code to keep a histogram of r/r times or time
	    spent in send().
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_histogram:-no}])
    AC_MSG_CHECKING([for netperf old histogram])
    if test :"${netperf_cv_old_histogram:-no}" = :yes ; then
	AC_DEFINE([OLD_HISTOGRAM], [1], [
	    Define to enable old pre-2.2pl6 formatted histogram.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_old_histogram:-no}])
    AC_MSG_CHECKING([for netperf intervals])
    if test :"${netperf_cv_intervals:-no}" = :yes ; then
	AC_DEFINE([INTERVALS], [1], [
	    Define to enable code to allow pacing of sends in a UDP, TCP or
	    SCTP test.  This may have unexpected results on non-HPUX systems.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_intervals:-no}])
    AC_MSG_CHECKING([for netperf do dlpi])
    if test :"${xns_cv_includes:-no}" = :no ; then
	netperf_cv_do_dlpi=no
    fi
    if test :"${netperf_cv_do_dlpi:-no}" = :yes ; then
	AC_DEFINE([DO_DLPI], [1], [
	    Define to include code to test the DLPI implementation.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_dlpi:-no}])
    AC_MSG_CHECKING([for netperf do sctp])
    if test :"${sctp_cv_openss7:-no}" = :yes ; then
	netperf_cv_do_sctp=yes
    else
	netperf_cv_do_sctp=no
    fi
    if test :"${netperf_cv_do_sctp:-no}" = :yes ; then
	AC_DEFINE([DO_SCTP], [1], [
	    Define to include code to test the SCTP implementation.
	])
	CPPFLAGS="${CPPFLAGS:+$CPPFLAGS }-I${srcdir}/../sctp/include"
    fi
    AC_MSG_RESULT([${netperf_cv_do_sctp:-no}])
    AC_MSG_CHECKING([for netperf do select])
    if test :"${netperf_cv_do_select:-no}" = :yes ; then
	AC_DEFINE([DO_SELECT], [1], [
	    Define to do select() on receive.
	])
    fi
    AC_MSG_RESULT([${netperf_cv_do_select:-no}])
    AC_MSG_CHECKING([for netperf do lwp])
    if test :"${netperf_cv_do_lwp:-no}" = :yes ; then
	AC_DEFINE([DO_LWP], [1], [
	])
    fi
    AC_MSG_RESULT([${netperf_cv_do_lwp:-no}])
    AC_MSG_CHECKING([for netperf do nbrr])
    if test :"${netperf_cv_do_nbrr:-no}" = :yes ; then
	AC_DEFINE([DO_NBRR], [1], [
	    Define to do non-blocking request/response.
	])
    fi
    AC_MSG_RESULT([${netperf_cv_do_nbrr:-no}])
    AC_MSG_CHECKING([for netperf do xti])
    if test -n "$inet_cv_includes" ; then
	netperf_cv_do_xti=yes
    else
	netperf_cv_do_xti=no
	netperf_cv_do_xti_sctp=no
    fi
    if test :"${netperf_cv_do_xti:-no}" = :yes ; then
	AC_DEFINE([DO_XTI], [1], [
	    Define to include code to test the XTI implementation.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_xti:-no}])
    AC_MSG_CHECKING([for netperf do xti sctp])
    if test -n "$inet_cv_includes" -a -n "$sctp_cv_includes" ; then
	netperf_cv_do_xti_sctp=yes
    else
	netperf_cv_do_xti_sctp=no
    fi
    if test :"${netperf_cv_do_xti_sctp:-no}" = :yes ; then
	AC_DEFINE([DO_XTI_SCTP], [1], [
	    Define to include code to test the XTI SCTP implementation.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_xti_sctp:-no}])
    AC_MSG_CHECKING([for netperf do unix])
    if test :"${netperf_cv_do_unix:-no}" = :yes ; then
	AC_DEFINE([DO_UNIX], [1], [
	    Define to include code to test the UNIX domain implementation.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_unix:-no}])
    AC_MSG_CHECKING([for netperf do 1644])
    if test :"${netperf_cv_do_1644:-no}" = :yes ; then
	AC_DEFINE([DO_1644], [1], [
	    Define to include code to test T/TCP vs TCP transactions.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_first_burst:-no}])
    AC_MSG_CHECKING([for netperf do first burst])
    if test :"${netperf_cv_do_first_burst:-no}" = :yes ; then
	AC_DEFINE([DO_FIRST_BURST], [1], [
	    Define to include code to create an initial burst.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_first_burst:-no}])
    AC_MSG_CHECKING([for netperf debug log file])
    if test -n "$netperf_cv_debug_log_file" ; then
	AC_DEFINE_UNQUOTED([DEBUG_LOG_FILE], ["$netperf_cv_debug_log_file"], [
	    Define to specify where netserver should put its debug output when
	    debug is enabled.
	])dnl
    fi
    AC_MSG_RESULT([$netperf_cv_debug_log_file])
    AC_MSG_CHECKING([for netperf use looper])
    if test :"${netperf_cv_use_looper:-no}" = :yes ; then
	AC_DEFINE([USE_LOOPER], [1], [
	    Define to use looper or soaker processes to measure CPU
	    utilitization.  These will be forked-off at the beginning.  If you
	    are running this way, it is important to see how much impact these
	    have on the measurement.  A loopback test on uniprocessor should
	    be able to consume approximately 100% of the CPU, and the
	    difference between throughput with USE_LOOPER CPU and without
	    should be small for a real network.  If it is not, then some work
	    probably needs to be done on reducing the priority of the looper
	    processes.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_looper:-no}])
    AC_MSG_CHECKING([for netperf use pstat])
    if test :"${netperf_cv_use_pstat:-no}" = :yes ; then
	AC_DEFINE([USE_PSTAT], [1], [
	    If used on HP-UX 10.0 and later, this will make CPU utilization
	    measurements with some information returned by the 10.X pstat()
	    call. This is very accurate, and should have no impact on the
	    measurement. Astute observers will notice that the LOC_CPU and
	    REM_CPU rates with this method look remarkably close to the
	    clockrate of the machine.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_pstat:-no}])
    AC_MSG_CHECKING([for netperf use kstat])
    if test :"${netperf_cv_use_kstat:-no}" = :yes ; then
	AC_DEFINE([USE_KSTAT], [1], [
	    If used on Solaris 2.mumble, this will make CPU utilization
	    measurements using the kstat interface.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_kstat:-no}])
    AC_MSG_CHECKING([for netperf use /proc/stat])
    if test :"${netperf_cv_use_proc_stat:-no}" = :yes ; then
	AC_DEFINE([USE_PROC_STAT], [1], [
	    For use on linux systems with CPU utilization info in /proc/stat.
	    Provides a fairly accurate CPU load measurement without affecting
	    measurement.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_proc_stat:-no}])
    AC_MSG_CHECKING([for netperf use sysctl])
    if test :"${netperf_cv_use_sysctl:-no}" = :yes ; then
	AC_DEFINE([USE_SYSCTL], [1], [
	    Use sysctl() on FreeBSD (perhaps other BSDs) to calculate CPU
	    utilization.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_sysctl:-no}])
    AC_MSG_CHECKING([for netperf use perfstat])
    if test :"${netperf_cv_use_perfstat:-no}" = :yes ; then
	AC_DEFINE([USE_PERFSTAT], [1], [
	    Use the perfstat call on AIX to calculate CPU utilization.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_perfstat:-no}])
    AC_MSG_CHECKING([for netperf do ipv6])
    if test :"${netperf_cv_do_ipv6:-no}" = :yes ; then
	AC_DEFINE([DO_IPV6], [1], [
	    Define to enable IPv6 testing.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_ipv6:-no}])
    AC_MSG_CHECKING([for netperf do dns])
    if test :"${netperf_cv_do_dns:-no}" = :yes ; then
	AC_DEFINE([DO_DNS], [1], [
	    Define to enable DNS testing.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_dns:-no}])
    AC_MSG_CHECKING([for netperf dont wait])
    if test :"${netperf_cv_dont_wait:-no}" = :yes ; then
	AC_DEFINE([DONT_WAIT], [1], [
	    Define to enable MSG_DONTWAIT on sends.
	])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_dont_wait:-no}])
])# _NETPERF_OUTPUT
# =============================================================================

# =============================================================================
# _NETPERF_
# -----------------------------------------------------------------------------
AC_DEFUN([_NETPERF_], [dnl
])# _NETPERF_
# =============================================================================

# =============================================================================
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.30  2007/08/14 07:26:44  brian
# - GPLv3 header update
#
# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
