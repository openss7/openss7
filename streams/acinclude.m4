dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.22 2004/05/15 07:09:45 brian Exp $
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
dnl Last Modified $Date: 2004/05/15 07:09:45 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/libraries.m4])
m4_include([m4/strconf.m4])

# =========================================================================
# AC_LFS
# -------------------------------------------------------------------------
AC_DEFUN([AC_LFS], [
    ac_default_prefix='/usr'
    _LFS_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _LDCONFIG
    _LFS_SETUP_COMPAT
    _LFS_SETUP_FIFOS
    LFS_INCLUDES="-D_LFS_SOURCE=1 -I- -imacros ./config.h -I./include -I${srcdir}/include"
    AC_SUBST([LFS_INCLUDES])
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    AC_SUBST([USER_LDFLAGS])
    AC_SUBST([USER_CPPFLAGS])
    AC_SUBST([USER_CFLAGS])
    AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user LDFLAGS   = $USER_LDFLAGS])
    AC_MSG_NOTICE([final user INCLUDES  = $LFS_INCLUDES])
    _LFS_SETUP
    AC_MSG_NOTICE([final kern MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kern NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kern CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kern CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final kern LDFLAGS   = $KERNEL_LDFLAGS])
    CPPFLAGS=
    CFLAGS=
    _LFS_STRCONF
])# AC_LFS
# =========================================================================

# =========================================================================
# _LFS_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_OPTIONS], [
])# _LFS_OPTIONS
# =========================================================================

# =========================================================================
# _LFS_SETUP_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_DEBUG], [
    case "$linux_cv_debug" in
        _DEBUG | _TEST)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DEBUG], [], [Define to perform
                    internal structure tracking within the STREAMS executive
                    as well as to provide additional /proc filesystem files
                    for examining internal structures.])
            ;;
    esac
])# _LFS_SETUP_DEBUG
# =========================================================================

# =========================================================================
# _LFS_SETUP_COMPAT
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_COMPAT], [
    AC_ARG_ENABLE([compat-svr4],
        AS_HELP_STRING([--enable-compat-svr4],
            [enable source compatibility with SVR 4.2 MP variants.
            @<:@default=yes@:>@]),
            [enable_compat_svr4="$enableval"],
            [enable_compat_svr4='yes'])
    AC_CACHE_CHECK([for UNIX(R) SVR 4.2 compatibility], [lfs_cv_svr4], [
        if test :"$enable_compat_svr4" != :no ; then lfs_cv_svr4=yes ; else lfs_cv_svr4=no ; fi
        if test :"$enable_compat_sol8" != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_uw7"  != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_osf"  != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_aix"  != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_hpux" != :no ; then lfs_cv_svr4=yes ; fi
    ])
    AC_ARG_ENABLE([compat-sol8],
        AS_HELP_STRING([--enable-compat-sol8],
            [enable source compatibility with Solaris 8 variants.
            @<:@default=yes@:>@]),
        [enable_compat_sol8="$enableval"],
        [enable_compat_sol8='yes'])
    AC_CACHE_CHECK([for Solaris(R) 8 compatibility], [lfs_cv_sol8], [
        if test :"$enable_compat_sol8" != :no ; then lfs_cv_sol8=yes ; else lfs_cv_sol8=no ; fi
    ])
    AC_ARG_ENABLE([compat-uw7],
        AS_HELP_STRING([--enable-compat-uw7],
            [enable source compatibility with UnixWare 7 variants.
            @<:@default=yes@:>@]),
        [enable_compat_uw7="$enableval"],
        [enable_compat_uw7='yes'])
    AC_CACHE_CHECK([for UnixWare(R) 7 compatibility], [lfs_cv_uw7], [
        if test :"$enable_compat_uw7" != :no  ; then lfs_cv_uw7=yes  ; else lfs_cv_uw7=no  ; fi
    ])
    AC_ARG_ENABLE([compat-osf],
        AS_HELP_STRING([--enable-compat-osf],
            [enable source compatibility with OSF/1.2 variants.
            @<:@default=yes@:>@]),
        [enable_compat_osf="$enableval"],
        [enable_compat_osf='yes'])
    AC_CACHE_CHECK([for OSF/1.2 compatibility], [lfs_cv_osf], [
        if test :"$enable_compat_osf" != :no  ; then lfs_cv_osf=yes  ; else lfs_cv_osf=no  ; fi
    ])
    AC_ARG_ENABLE([compat-aix],
        AS_HELP_STRING([--enable-compat-aix],
            [enable source compatibility with AIX 4 variants.
            @<:@default=yes@:>@]),
        [enable_compat_aix="$enableval"],
        [enable_compat_aix='yes'])
    AC_CACHE_CHECK([for AIX(R) 4 compatibility], [lfs_cv_aix], [
        if test :"$enable_compat_aix" != :no  ; then lfs_cv_aix=yes  ; else lfs_cv_aix=no  ; fi
    ])
    AC_ARG_ENABLE([compat-hpux],
        AS_HELP_STRING([--enable-compat-hpux],
            [enable source compatibility with HPUX variants.
            @<:@default=yes@:>@]),
        [enable_compat_hpux="$enableval"],
        [enable_compat_hpux='yes'])
    AC_CACHE_CHECK([for HPUX(R) compatibility], [lfs_cv_hpux], [
        if test :"$enable_compat_hpux" != :no ; then lfs_cv_hpux=yes ; else lfs_cv_hpux=no ; fi
    ])
    AC_ARG_ENABLE([compat-lis],
        AS_HELP_STRING([--enable-compat-lis],
            [enable source compatibility with LiS variants.
            @<:@default=yes@:>@]),
        [enable_compat_lis="$enableval"],
        [enable_compat_lis='yes'])
    AC_CACHE_CHECK([for LiS compatibility], [lfs_cv_lis], [
        if test :"$enable_compat_lis" != :no  ; then lfs_cv_lis=yes  ; else lfs_cv_lis=no  ; fi
    ])
    if test :"$lfs_cv_svr4" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the UNIX(R)
                SVR 4.2 MP docs so that STREAMS drivers and modules written to
                UNIX(R) SVR 4.2 MP specs will compile with Linux Fast STREAMS.
                When undefined, STREAMS drivers and modules written for
                UNIX(R) SVR 4.2 MP will require porting in more respects.])
    fi
    if test :"$lfs_cv_sol8" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the Solaris(R)
                8 release so that STREAMS drivers and modules written for
                Solaris(R) 8 will compile with Linux Fast STREAMS.  When
                undefined, STREAMS drivers and modules written for Solaris(R)
                8 will require porting in more respects.])
    fi
    if test :"$lfs_cv_uw7" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the
                UnixWare(R) 7 release so that STREAMS drivers and modules
                written for UnixWare(R) 7 will compile with Linux Fast
                STREAMS.  When undefined, STREAMS drivers and modules written
                for UnixWare(R) 7 will require porting in more respects.])
    fi
    if test :"$lfs_cv_osf" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the OSF(R)/1.2
                release so that STREAMS drivers and modules written for
                OSF(R)/1.2 will compile with Linux Fast STREAMS.  When
                undefined, STREAMS drivers and modules written for OSF(R)/1.2
                will require porting in more respects.])
    fi
    if test :"$lfs_cv_aix" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the AIX(R) 5L
                Version 5.1 release so that STREAMS drivers and modules
                written for AIX(R) 5L Version 5.1 will compile with Linux Fast
                STREAMS.  When undefined, STREAMS drivers and modules written
                for AIX(R) 5L Version 5.1 will require porting in more
                respects.])
    fi
    if test :"$lfs_cv_hpux" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the HPUX(R)
                release so that STREAMS drivers and modules written for
                HPUX(R) will compile with Linux Fast STREAMS.  When undefined,
                STREAMS drivers and modules written for HPUX(R) will require
                porting in more respects.])
    fi
    if test :"$lfs_cv_lis" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LIS_MODULE], [], [When
                defined, Linux Fast STREAMS will attempt to be as compatible
                as possible (without replicating any bugs) with the LiS
                release so that STREAMS drivers and modules written for LiS
                will compile with Linux Fast STREAMS.  When undefined, STREAMS
                drivers and modules written for LiS will require porting in
                more respects.])
    fi
])# _LFS_SETUP_COMPAT
# =========================================================================

# =========================================================================
# _LFS_SETUP_FIFOS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_FIFOS], [
    AC_ARG_ENABLE([streams-fifos],
        AS_HELP_STRING([--enable-streams-fifos],
            [enable override of system fifos with STREAMS-based fifos.
            @<:@default=no@:>@]),
        [enable_streams_fifos="$enableval"],
        [enable_streams_fifos='no'])
    if test :"$enable_streams_fifos" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OVERRIDE_FIFOS], [], [When defined,
                Linux Fast STREAMS will override the Linux system defined
                FIFOs at startup.  This should be used with care for a while,
                until streams FIFOs are proven.])
    fi
])# _LFS_SETUP_FIFOS
# =========================================================================

# =========================================================================
# _LFS_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP], [
    _LINUX_KERNEL
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LFS_SETUP_MODULE
    _LFS_CHECK_KERNEL
    _LFS_SETUP_DEBUG
])# _LFS_SETUP
# =========================================================================

# =========================================================================
# _LFS_SETUP_MODULE
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_MODULE], [
    if test :"${linux_cv_modules:-yes}" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MODULE], [], [When defined, STREAMS
            is being compiled as a loadable kernel module.])
    else
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS], [], [When defined, STREAMS is
            being compiled as a kernel linkable object.])
    fi
])# _LFS_SETUP_MODULE
# =========================================================================

# =========================================================================
# _LFS_CHECK_KERNEL
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_KERNEL], [
    _LFS_CHECK_XNS
    _LFS_CHECK_TLI
    _LFS_CHECK_INET
    _LFS_CHECK_XNET
    _LFS_CHECK_SOCK
    _LFS_CONFIG_FATTACH
    _LFS_CONFIG_LIS
    _LFS_CONFIG_LFS
])# _LFS_CHECK_KERNEL
# =========================================================================

# =========================================================================
# _LFS_CHECK_XNS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_XNS], [
    AC_ARG_WITH([xns],
        AS_HELP_STRING([--with-xns],
            [include xns headers in install.  @<:@default=no@:>@]),
        [with_xns="$withval"],
        [with_xns='no'])
    AC_MSG_CHECKING([for package xns headers])
    AC_MSG_RESULT([$with_xns])
    if test :"$with_xns" = :yes ; then :;
        _LFS_CONFIG_XNS
    fi
    AM_CONDITIONAL([WITH_XNS], test :"$with_xns" = :yes )
])# _LFS_CHECK_XNS
# =========================================================================

# =========================================================================
# _LFS_CHECK_TLI
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_TLI], [
    AC_ARG_WITH([tli],
        AS_HELP_STRING([--with-tli],
            [include tli modules in build.  @<:@default=no@:>@]),
        [with_tli="$withval"],
        [with_tli='no'])
    AC_MSG_CHECKING([for package tli modules])
    AC_MSG_RESULT([$with_tli])
    if test :"$with_tli" = :yes ; then :;
        _LFS_CONFIG_TLI
    fi
    AM_CONDITIONAL([WITH_TLI], test :"$with_tli" = :yes )
])# _LFS_CHECK_TLI
# =========================================================================

# =========================================================================
# _LFS_CHECK_INET
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_INET], [
    AC_ARG_WITH([inet],
        AS_HELP_STRING([--with-inet],
            [include inet driver in build.  @<:@default=no@:>@]),
        [with_inet="$withval"],
        [with_inet='no'])
    AC_MSG_CHECKING([for package inet driver])
    AC_MSG_RESULT([$with_inet])
    if test :"$with_inet" = :yes ; then :;
        _LFS_CONFIG_INET
    fi
    AM_CONDITIONAL([WITH_INET], test :"$with_inet" = :yes )
])# _LFS_CHECK_INET
# =========================================================================

# =========================================================================
# _LFS_CHECK_XNET
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_XNET], [
    AC_ARG_WITH([xnet],
        AS_HELP_STRING([--with-xnet],
            [include xnet library in build.  @<:@default=no@:>@]),
        [with_xnet="$withval"],
        [with_xnet='no'])
    AC_MSG_CHECKING([for package xnet library])
    AC_MSG_RESULT([$with_xnet])
    if test :"$with_xnet" = :yes ; then :;
        _LFS_CONFIG_XNET
    fi
    AM_CONDITIONAL([WITH_XNET], test :"$with_xnet" = :yes )
])# _LFS_CHECK_XNET
# =========================================================================

# =========================================================================
# _LFS_CHECK_SOCK
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_SOCK], [
    AC_ARG_WITH([sock],
        AS_HELP_STRING([--with-sock],
            [include sock library in build.  @<:@default=no@:>@]),
        [with_sock="$withval"],
        [with_sock='no'])
    AC_MSG_CHECKING([for package socket library])
    AC_MSG_RESULT([$with_sock])
    if test :"$with_sock" = :yes ; then :;
        _LFS_CONFIG_SOCK
    fi
    AM_CONDITIONAL([WITH_SOCK], test :"$with_sock" = :yes )
])# _LFS_CHECK_SOCK
# =========================================================================

# =========================================================================
# _LFS_CONFIG_XNS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_XNS], [
])# _LFS_CONFIG_XNS
# =========================================================================

# =========================================================================
# _LFS_CONFIG_TLI
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_TLI], [
])# _LFS_CONFIG_TLI
# =========================================================================

# =========================================================================
# _LFS_CONFIG_INET
# -------------------------------------------------------------------------
# tcp_openreq_cachep            <-- extern, declared in <net/tcp.h>
# tcp_set_keepalive             <-- extern, declared in <net/tcp.h>
# ip_tos2prio                   <-- extern, declared in <net/route.h>
# sysctl_rmem_default           <-- extern, declared in net/core/sock.c
# sysctl_wmem_default           <-- extern, declared in net/core/sock.c
# sysctl_tcp_fin_timeout        <-- extern, declared in net/ipv4/tcp.c
# -----------------------------------------------------------------------------
# New ones: (All only exported with IPv6 as module.)
# tcp_sync_mss                  <-- extern, declared in <net/tcp.h>
# tcp_write_xmit                <-- extern, declared in <net/tcp.h>
# tcp_cwnd_application_limited  <-- extern, declared in <net/tcp.h>
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_INET], [
    AC_CACHE_CHECK([for OpenSS7 Kernel SCTP], [lfs_cv_openss7_sctp], [
        _LINUX_KERNEL_ENV([
            AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
            ], [lfs_cv_openss7_sctp=yes], [lfs_cv_openss7_sctp=no])
        ])
    ])
    if test :"${lfs_cv_openss7_sctp:-no}" = :yes ; then
        AC_DEFINE([HAVE_OPENSS7_SCTP], [1],
        [Define if your kernel supports the OpenSS7 Linux Kernel Sockets SCTP
        patches.  This enables support in the INET driver for STREAMS on top
        of the OpenSS7 Linux Kernel Sockets SCTP implementation.])
    fi
    _LINUX_KERNEL_SYMBOL_ADDR([tcp_openreq_cachep])
    _LINUX_KERNEL_SYMBOL_ADDR([tcp_set_keepalive])
    _LINUX_KERNEL_SYMBOL_ADDR([ip_tos2prio])
    _LINUX_KERNEL_SYMBOL_ADDR([sysctl_rmem_default])
    _LINUX_KERNEL_SYMBOL_ADDR([sysctl_wmem_default])
    _LINUX_KERNEL_SYMBOL_ADDR([sysctl_tcp_fin_timeout])
    _LINUX_KERNEL_SYMBOL_ADDR([tcp_sync_mss])
    _LINUX_KERNEL_SYMBOL_ADDR([tcp_write_xmit])
    _LINUX_KERNEL_SYMBOL_ADDR([tcp_cwnd_application_limited])
    _LINUX_KERNEL_ENV([
        AC_CHECK_MEMBER([struct sock.protinfo.af_inet.ttl],
            [lfs_cv_af_inet_ttl_member_name='ttl'],
            [:],
            [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
            ])
        AC_CHECK_MEMBER([struct sock.protinfo.af_inet.uc_ttl],
            [lfs_cv_af_inet_ttl_member_name='uc_ttl'],
            [:],
            [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
            ])
        ])
    if test :"${lfs_cv_af_inet_ttl_member_name:+set}" = :set ; then
        AC_DEFINE_UNQUOTED([USING_AF_INET_TTL_MEMBER_NAME], [$lfs_cv_af_inet_ttl_member_name], [Most
        kernels call the time-to-live member of the af_inet structure ttl.  For some reason
        (probably because the old ttl member as 'int' and the new uc_ttl member is 'unsigned char')
        reported by Bala Viswanathan <balav@lsil.com> to the linux-streams mailing lfst, EL3 renames
        the member to uc_ttl on some kernels.  Define this to the member name used (ttl or uc_ttl)
        so that the inet driver can be properly supported.  If this is not defined, 'ttl' will be
        used as a default.])
    else
        AC_MSG_WARN([
***
*** You have really mangled kernel header files with regards to the 'sock'
*** structure.  The 'sock' structure on Linux 2.4 should have an af_inet.ttl
*** member (used in stock kernels) or an af_inet.uc_ttl member (used on some RH
*** kernels).  Your kernel headers are so bad that <net/sock.h> has neither
*** member defined.  Winging it with 'ttl' as the member name.
***
        ])
    fi
])# _LFS_CONFIG_INET
# =========================================================================

# =========================================================================
# _LFS_CONFIG_XNET
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_XNET], [
])# _LFS_CONFIG_XNET
# =========================================================================

# =========================================================================
# _LFS_CONFIG_SOCK
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_SOCK], [
])# _LFS_CONFIG_SOCK
# =========================================================================

# =========================================================================
# _LFS_CONFIG_FATTACH
# -------------------------------------------------------------------------
# 
# symbols to implement fattach
# -------------------------------------------------------------------------
# path_init             <--- exported
# path_walk             <--- exported
# path_release          <--- exported
# getname               <--- exported
# d_mountpoint          <--- inline, declared in <linux/dcache.h>
# mntput                <--- inline, declared in <linux/mount.h>
# clone_mnt             <=== static, can be ripped
# check_mnt             <=== static, can be ripped
# graft_tree            <=== static, can be ripped
# 
# symbols to implement fdetach
# -------------------------------------------------------------------------
# path_init             <--- exported
# path_release          <--- exported
# check_mnt             <=== static, can be ripped
#
# symbols to override system fifos
# -------------------------------------------------------------------------
# def_fifo_fops         <-- extern, declared in <linux/fs.h>
# 
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_FATTACH], [
    _LINUX_KERNEL_SYMBOL_ADDR([mount_sem], [], [
        AC_DEFINE_UNQUOTED([HAVE_TASK_NAMESPACE_SEM], [1],
        [Some recent 2.4 RH kernel place the mount semaphore into the task
        structure rather than using the static global mount_sem semaphore.
        Define this if you have a modified kernel.])
    ])
    lfs_pipe=yes
    lfs_fattach=yes
    _LINUX_KERNEL_SYMBOL_ADDR([clone_mnt], [], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_ADDR([check_mnt], [], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_ADDR([graft_tree], [], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_ADDR([do_umount], [], [lfs_fattach=no; lfs_pipe=no])
    AC_CACHE_CHECK([for kernel symbol support for fattach/fdetach], [lfs_cv_fattach], [
        lfs_cv_fattach="$lfs_fattach"
    ])
    if test :"${lfs_cv_fattach:-no}" != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_KERNEL_FATTACH_SUPPORT], [1],
        [If the addresses for the necessary symbols above are defined, then
        define this to include fattach/fdetach support.])
    fi
    AC_CACHE_CHECK([for kernel symbol support for pipe], [lfs_cv_pipe], [
        lfs_cv_pipe="$lfs_pipe"
    ])
    if test :${lfs_cv_pipe:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_KERNEL_PIPE_SUPPORT], [1],
        [If the addresses for the necessary symbols above are defined, then
        define this to include pipe support.])
    fi
    _LINUX_KERNEL_SYMBOL_ADDR([def_fifo_fops])
])# _LFS_CONFIG_FATTACH
# =========================================================================

# =========================================================================
_LFS_CONFIG_LIS
# -------------------------------------------------------------------------
# symbols to rip for LiS support (without system call generation)
# -------------------------------------------------------------------------
# sys_unlink            <-- extern, not declared
# sys_mknod             <-- extern, not declared
# sys_umount            <-- extern, not declared
# sys_mount             <-- extern, not declared
# pcibios_init          <-- extern, declared in <linux/pci.h>
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_LIS], [
    _LINUX_KERNEL_SYMBOL_ADDR([sys_unlink], [], [
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_unlink() will always return ENOSYS])
        fi
    ])
    _LINUX_KERNEL_SYMBOL_ADDR([sys_mknod], [], [
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_mknod() will always return ENOSYS])
        fi
    ])
    _LINUX_KERNEL_SYMBOL_ADDR([sys_umount], [], [
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_umount() will always return ENOSYS])
        fi
    ])
    _LINUX_KERNEL_SYMBOL_ADDR([sys_mount], [], [
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_mount() will always return ENOSYS])
        fi
    ])
    _LINUX_KERNEL_SYMBOL_ADDR([pcibios_init])
])# _LFS_CONFIG_LIS
# =========================================================================

# =========================================================================
# _LFS_CONFIG_LFS
# -------------------------------------------------------------------------
# symbols to rip for Linux Fast STREAMS
# -------------------------------------------------------------------------
# file_move             <-- extern, declared in <linux/fs.h>
# open_softirq          <-- extern, declared in <linux/interrupt.h>
# sock_readv_writev     <-- extern, declared in <linux/net.h>
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_LFS], [
    _LINUX_KERNEL_SYMBOL_ADDR([file_move])
    _LINUX_KERNEL_SYMBOL_ADDR([open_softirq], [], [
        AC_MSG_ERROR([
*** 
*** Compiling Linux Fast STREAMS requires the availability of the kernel
*** symbol open_softirq so that the necessary software IRQs can be
*** established.  This symbol is not normally exported from the kernel.
*** I have looked in "${linux_cv_k_sysmap:-(no sysmap)}" for the symbol
*** and cannot find it.  This problem needs to be corrected or workaround
*** found before you can compile Linux Fast STREAMS for the system for
*** which it is being configured.
*** 
        ])
    ])
    _LINUX_KERNEL_SYMBOL_ADDR([sock_readv_writev])
])# _LFS_CONFIG_LFS
# =========================================================================

# =========================================================================
# _LFS_STRCONF
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_STRCONF], [
    strconf_cv_stem='Config'
dnl strconf_cv_input='Config.master'
dnl strconf_cv_majbase=230
    strconf_cv_config='include/sys/config.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes='src/util/strmakenodes.c'
dnl strconf_cv_stsetup='strsetup.conf'
dnl strconf_cv_strload='strload.conf'
    _STRCONF
])# _LFS_STRCONF
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
