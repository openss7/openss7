dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 1.1.2.1 2004/08/05 10:50:05 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (C) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
dnl
dnl All Rights Reserved.
dnl
dnl Permission is granted to make and distribute verbatim copies of this
dnl README file provided the copyright notice and this permission notice are
dnl preserved on all copies.
dnl 
dnl Permission is granted to copy and distribute modified versions of this
dnl manual under the conditions for verbatim copying, provided that the
dnl entire resulting derived work is distributed under the terms of a
dnl permission notice identical to this one
dnl 
dnl Since the Linux kernel and libraries are constantly changing, this README
dnl file may be incorrect or out-of-date.  The author(s) assume no
dnl responsibility for errors or omissions, or for damages resulting from the
dnl use of the information contained herein.  The author(s) may not have
dnl taken the same level of care in the production of this manual, which is
dnl licensed free of charge, as they might when working professionally.
dnl 
dnl Formatted or processed versions of this manual, if unaccompanied by the
dnl source, must acknowledge the copyright and authors of this work.
dnl 
dnl =========================================================================
dnl 
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions
dnl apply to you.  If the Software is supplied by the Department of Defense
dnl ("DoD"), it is classified as "Commercial Computer Software" under
dnl paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
dnl Regulations ("DFARS") (or any successor regulations) and the Government is
dnl acquiring only the license rights granted herein (the license rights
dnl customarily provided to non-Government users).  If the Software is
dnl supplied to any unit or agency of the Government other than DoD, it is
dnl classified as "Restricted Computer Software" and the Government's rights
dnl in the Software are defined in paragraph 52.227-19 of the Federal
dnl Acquisition Regulations ("FAR") (or any success regulations) or, in the
dnl cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl 
dnl =========================================================================
dnl 
dnl Commercial licensing and support of this software is available from
dnl OpenSS7 Corporation at a fee.  See http://www.openss7.com/
dnl 
dnl =========================================================================
dnl
dnl Last Modified $Date: 2004/08/05 10:50:05 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/public.m4])
m4_include([m4/streams.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/man.m4])
m4_include([m4/rpm.m4])

# =========================================================================
# AC_NETPERF
# -------------------------------------------------------------------------
AC_DEFUN([AC_NETPERF], [dnl
    ac_default_prefix='/usr'
    _NETPERF_OPTIONS
    _PUBLIC_RELEASE
    _MAN_CONVERSION
    _RPM_SPEC
    _STREAMS
    _XNS
    _XTI
    INCLUDES="-I- -imacros ./config.h${xns_cv_includes:+ -I}${xns_cv_includes}${xti_cv_includes:+ -I}${xti_cv_includes}${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    AC_SUBST([INCLUDES])
    _NETPERF_OUTPUT
])# AC_NETPERF
# =========================================================================

# =========================================================================
# _NETPERF_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_NETPERF_OPTIONS], [dnl
    AC_ARG_ENABLE([netperf-dirty],
        AS_HELP_STRING([--enable-netperf-dirty],
            [enable code to dirty buffers before calls to send
             @<:@default=yes@:>@]),
        [netperf_cv_dirty="$enableval"],
        [netperf_cv_dirty='yes'])
    AC_ARG_ENABLE([netperf-histogram],
        AS_HELP_STRING([--enable-netperf-histogram],
            [enable code to to keep a histogram of r/r times or time spent in
             send() @<:@default=yes@:>@]),
        [netperf_cv_histogram="$enableval"],
        [netperf_cv_histogram='yes'])
    AC_ARG_ENABLE([netperf-old-histogram],
        AS_HELP_STRING([--enable-netperf-old-histogram],
            [enable old pre-2.2pl6 formatted histogram
             @<:@default=no@:>@]),
        [netperf_cv_old_histogram="$enableval"],
        [netperf_cv_old_histogram='no'])
    AC_ARG_ENABLE([netperf-intervals],
        AS_HELP_STRING([--enable-netperf-intervals],
            [enable code to allow pacing of sends in a UDP, TCP or SCTP test.
             @<:@default=yes@:>@]),
        [netperf_cv_intervals="$enableval"],
        [netperf_cv_intervals='yes'])
    AC_ARG_ENABLE([netperf-do-dlpi],
        AS_HELP_STRING([--enable-netperf-do-dlpi],
            [enable code to test the DLPI implementation
             @<:@default=yes@:>@]),
        [netperf_cv_do_dlpi="$enableval"],
        [netperf_cv_do_dlpi='yes'])
    AC_ARG_ENABLE([netperf-do-xti],
        AS_HELP_STRING([--enable-netperf-do-xti],
            [enable code to test the XTI implementation
             @<:@default=yes@:>@]),
        [netperf_cv_do_xti="$enableval"],
        [netperf_cv_do_xti='yes'])
    AC_ARG_ENABLE([netperf-do-unix],
        AS_HELP_STRING([--enable-netperf-do-unix],
            [enable code to test the Unix domain implementation
             @<:@default=yes@:>@]),
        [netperf_cv_do_unix="$enableval"],
        [netperf_cv_do_unix='yes'])
    AC_ARG_WITH([netperf-debug-log-file],
        AS_HELP_STRING([--with-netperf-debug-log-file=LOGFILE],
            [specify the LOGFILE to which debug output is written
             @<:@default='/tmp/netperf.debug'@:>@]),
        [netperf_cv_debug_log_file="$enableval"],
        [netperf_cv_debug_log_file='/tmp/netperf.debug'])
    AC_ARG_ENABLE([netperf-use-looper],
        AS_HELP_STRING([--enable-netperf-looper],
            [enable looper or soaker processes to measure CPU utilitization
             @<:@default=no@:>@]),
        [netperf_cv_use_looper="$enableval"],
        [netperf_cv_use_looper='no'])
    AC_ARG_ENABLE([netperf-use-pstat],
        AS_HELP_STRING([--enable-netperf-use-pstat],
            [enable CPU utilization measurements with pstat()
             @<:@default=no@:>@]),
        [netperf_cv_use_pstat="$enableval"],
        [netperf_cv_use_pstat="${ac_cv_func_pstat:-no}"])
    AC_ARG_ENABLE([netperf-use-kstat],
        AS_HELP_STRING([--enable-netperf-use-kstat],
            [enable CPU utilization measurements with kstat()
             @<:@default=no@:>@]),
        [netperf_cv_use_kstat="$enableval"],
        [netperf_cv_use_kstat="${ac_cv_func_kstat:-no}"])
    AC_ARG_ENABLE([netperf-use-proc-stat],
        AS_HELP_STRING([--enable-netperf-use-proc-stat],
            [enable CPU utilization measurements with /proc/stat()
             @<:@default=yes@:>@]),
        [netperf_cv_use_proc_stat="$enableval"],
        [netperf_cv_use_proc_stat='yes'])
    AC_ARG_ENABLE([netperf-do-ipv6],
        AS_HELP_STRING([--enable-netperf-ipv6],
            [enable tests using socket interface to IPV6
             @<:@default=no@:>@]),
        [netperf_cv_do_ipv6="$enableval"],
        [netperf_cv_do_ipv6='no'])
    AC_ARG_ENABLE([netperf-do-dns],
        AS_HELP_STRING([--enable-netperf-dns],
            [enable tests that measure the performance of a DNS server
             @<:@default=no@:>@]),
        [netperf_cv_do_dns="$enableval"],
        [netperf_cv_do_dns='no'])
    AC_ARG_ENABLE([netperf-use-sysctl],
        AS_HELP_STRING([--enable-netperf-use-sysctl],
            [enable CPU utilization measurements with sysctl()
             @<:@default=no@:>@]),
        [netperf_cv_use_sysctl="$enableval"],
        [netperf_cv_use_sysctl='no'])
    AC_ARG_ENABLE([netperf-use-perfstat],
        AS_HELP_STRING([--enable-netperf-use-perfstat],
            [enable CPU utilization measurements with perfstat()
             @<:@default=no@:>@]),
        [netperf_cv_use_perfstat="$enableval"],
        [netperf_cv_use_perfstat="${ac_cv_func_perfstat:-no}"])
])# _NETPERF_OPTIONS
# =========================================================================

# =========================================================================
# _NETPERF_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_NETPERF_OUTPUT], [dnl
    AC_MSG_CHECKING([for netperf dirty])
    if test :"${netperf_cv_dirty:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([DIRTY], [], [
          Define to enable code to dirty buffers before calls to send.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_dirty:-no}])
    AC_MSG_CHECKING([for netperf histogram])
    if test :"${netperf_cv_histogram:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([HISTOGRAM], [], [
            Define to enable code to keep a histogram of r/r times or time
            spent in send().
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_histogram:-no}])
    AC_MSG_CHECKING([for netperf old histogram])
    if test :"${netperf_cv_old_histogram:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([OLD_HISTOGRAM], [], [
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_old_histogram:-no}])
    AC_MSG_CHECKING([for netperf intervals])
    if test :"${netperf_cv_intervals:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([INTERVALS], [], [
            Define to enable code to allow pacing of sends in a UDP, TCP or
            SCTP test.  This may have unexpected results on non-HPUX systems.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_intervals:-no}])
    AC_MSG_CHECKING([for netperf do dlpi])
    if test :"${netperf_cv_do_dlpi:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([DO_DLPI], [], [
            Define to include code to test the DLPI implementation.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_xti:-no}])
    AC_MSG_CHECKING([for netperf do xti])
    if test :"${netperf_cv_do_xti:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([DO_XTI], [], [
            Define to include code to test the XTI implementation.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_xti:-no}])
    AC_MSG_CHECKING([for netperf do unix])
    if test :"${netperf_cv_do_unix:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([DO_UNIX], [], [
            Define to include code to test the UNIX domain implementation.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_unix:-no}])
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
        AC_DEFINE_UNQUOTED([USE_LOOPER], [], [
            Define to use looper or soaker processes to measure CPU
            utilitization.  There will be forked-off at the beginning.  If you
            are running this way, it is important to see how much impact these
            have on the measurement.  A loopback test on uniprocessor should
            be able to consume approximately 100% of the CPU, and the
            difference between throughput with USE_LOOPER CPU and without
            should be small for a real network.  If it is not, then some work
            probably need to be done o reducing the priority of the looper
            processes.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_looper:-no}])
    AC_MSG_CHECKING([for netperf use pstat])
    if test :"${netperf_cv_use_pstat:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([USE_PSTAT], [], [
            If used on HP-UX 10.0 and later, this will make CPU utilization
            measurements with some information returned byt he 10.X pstat()
            call. This is very accurate, and should have no impact on the
            measurement. Astute observers will notice that the LOC_CPU and
            REM_CPU rates with this method look remarkably close to the
            clockrate of the machine.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_pstat:-no}])
    AC_MSG_CHECKING([for netperf use kstat])
    if test :"${netperf_cv_use_kstat:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([USE_KSTAT], [], [
            If used on Solaris 2.mumble, this will make CPU utilization
            measurements using the kstat interface.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_kstat:-no}])
    AC_MSG_CHECKING([for netperf use /proc/stat])
    if test :"${netperf_cv_use_proc_stat:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([USE_PROC_STAT], [], [
            For use on linux systems with CPU utilization info in /proc/stat.
            Provides a fairly accurate CPU load measurement without affecting
            measurement.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_proc_stat:-no}])
    AC_MSG_CHECKING([for netperf use sysctl])
    if test :"${netperf_cv_use_sysctl:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([USE_SYSCTL], [], [
            Use sysctl() on FreeBSD (perhaps other BSDs) to calculate CPU
            utilization.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_sysctl:-no}])
    AC_MSG_CHECKING([for netperf use perfstat])
    if test :"${netperf_cv_use_perfstat:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([USE_PERFSTAT], [], [
            Use the perfstat call on AIX to calculate CPU utilization.
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_use_perfstat:-no}])
    AC_MSG_CHECKING([for netperf do ipv6])
    if test :"${netperf_cv_do_ipv6:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([DO_IPV6], [], [
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_ipv6:-no}])
    AC_MSG_CHECKING([for netperf do dns])
    if test :"${netperf_cv_do_dns:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([DO_DNS], [], [
        ])dnl
    fi
    AC_MSG_RESULT([${netperf_cv_do_dns:-no}])
])# _NETPERF_OUTPUT
# =========================================================================

# =========================================================================
# _NETPERF_
# -------------------------------------------------------------------------
AC_DEFUN([_NETPERF_], [dnl
])# _NETPERF_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================

