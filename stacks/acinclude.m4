dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.9 2004/05/15 07:09:27 brian Exp $
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
dnl Last Modified $Date: 2004/05/15 07:09:27 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/kernel.m4])
m4_include([m4/streams.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/libraries.m4])
m4_include([m4/strconf.m4])

# =========================================================================
# AC_SS7
# -------------------------------------------------------------------------
AC_DEFUN([AC_SS7], [
    ac_default_prefix='/usr'
    _SS7_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _LDCONFIG
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    _LINUX_KERNEL
    _LINUX_STREAMS
    SS7_INCLUDES="-I- -imacros ./config.h ${STREAMS_CPPFLAGS}${STREAMS_CPPFLAGS:+ }-I./src/include -I${srcdir}/src/include"
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    INCLUDES  = $SS7_INCLUDES])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_SUBST([USER_CPPFLAGS])
    AC_SUBST([USER_CFLAGS])
    AC_SUBST([SS7_INCLUDES])
    CPPFLAGS=
    CFLAGS=
    _SS7_SETUP
    _SS7_OUTPUT
])# AC_SS7
# =========================================================================

# =========================================================================
# _SS7_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_OPTIONS], [
])# _SS7_OPTIONS
# =========================================================================

# =========================================================================
# _SS7_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP], [
    _SS7_CHECK_SCTP
    _SS7_CHECK_XNS
    _SS7_CHECK_TLI
    _SS7_CHECK_INET
    _SS7_CHECK_XNET
    _SS7_CHECK_SOCK
])# _SS7_SETUP
# =========================================================================

# =========================================================================
# _SS7_CHECK_SCTP
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_SCTP], [
    AC_ARG_WITH([sctp2],
        AS_HELP_STRING([--with-sctp2],
            [include sctp2 package.  @<:@default=DETECTED@:>@]),
        [with_sctp2=$enableval],
        [with_sctp2=''])
    AC_MSG_CHECKING([for package sctp version 2])
    if test :"${with_sctp2:-no}" != :no ; then
        AC_MSG_RESULT([yes])
        _SS7_SETUP_SCTP
    else
        AC_MSG_RESULT([no])
    fi
    AM_CONDITIONAL([WITH_SCTP2], test :"${with_sctp2:-no}" != :no )
])# _SS7_CHECK_SCTP
# =========================================================================

# =========================================================================
# _SS7_CHECK_XNS
# -------------------------------------------------------------------------
# Check if there are usable (recent OpenSS) OSI headers files.  These would be
# from a previous OpenSS7 STREAMS installation.  If they are not usable, they
# will be replaced with local installed copies.
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_XNS], [
    AC_CACHE_CHECK([for package have xns headers], [ss7_cv_have_xns], [
        ss7_cv_have_xns=no
    ])
    AC_ARG_WITH([xns],
        AS_HELP_STRING([--with-xns],
            [include xns headers. @<:@default=DETECTED@:>@]),
        [with_xns="$enableval"],
        [with_xns=''])
    AC_MSG_CHECKING([for package need xns headers])
    case :"$with_xns" in
        :yes)   ss7_cv_need_xns=yes ;;
        :no)    ss7_cv_need_xns=no  ;;
        :*) if test :"$ss7_cv_have_xns" = :yes
            then ss7_cv_need_xns=no
            else ss7_cv_need_xns=yes
            fi ;;
    esac
    AC_MSG_RESULT([$ss7_cv_need_xns])
    if test :"$ss7_cv_need_xns" = :yes ; then
        :
        _SS7_SETUP_XNS
    fi
    AM_CONDITIONAL([WITH_XNS], test :"$ss7_cv_need_xns" = :yes )
])# _SS7_CHECK_XNS
# =========================================================================

# =========================================================================
# _SS7_CHECK_TLI
# -------------------------------------------------------------------------
# Check if there are usable (recent OpenSS7) timod or tirdwr modules.  These
# would be from a previous OpenSS7 rpm LiS installation.  If they are not
# usable, they will be removed in am/kernel.am.
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_TLI], [
    AC_CACHE_CHECK([for package have tli modules], [ss7_cv_have_tli], [
        ss7_cv_have_tli=no
        if test -d $linux_cv_k_modules/misc -a -f $linux_cv_k_modules/misc/streams-timod.o ; then
            if ( grep -q 'FAST STREAMS' $linux_cv_k_modules/misc/streams-timod.o ) 2>/dev/null ; then
                ss7_cv_have_tli=yes
            fi
        fi
    ])
    AC_ARG_WITH([tli],
        AS_HELP_STRING([--with-tli],
            [include tli modules.  @<:@default=DETECTED@:>@]),
        [with_tli=$enableval],
        [with_tli=''])
    AC_MSG_CHECKING([for package need tli modules])
    case :"$with_tli" in
        :yes) ss7_cv_need_tli=yes ;;
        :no)  ss7_cv_need_tli=no ;;
        :*) if test :"$ss7_cv_have_tli" = :yes
            then ss7_cv_need_tli=no
            else ss7_cv_need_tli=yes
            fi ;;
    esac
    AC_MSG_RESULT([$ss7_cv_need_tli])
    if test :"$ss7_cv_need_tli" = :yes ; then
        :
        _SS7_SETUP_TLI
    fi
    AM_CONDITIONAL([WITH_TLI], test :"$ss7_cv_need_tli" = :yes )
])# _SS7_CHECK_TLI
# =========================================================================

# =========================================================================
# _SS7_CHECK_INET
# -------------------------------------------------------------------------
# Check if there is a usable (recent OpenSS7 version) inet driver available.
# This would be from a previous OpenSS7 rpm LiS installation.  If it is not
# usable, it will be removed in am/kernel.am.
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_INET], [
    AC_CACHE_CHECK([for package have inet driver], [ss7_cv_have_inet], [
        ss7_cv_have_inet=no
        if test -d $linux_cv_k_modules/misc -a -f $linux_cv_k_modules/misc/streams-inet.o ; then
            if ( grep -q 'FAST STREAMS' $linux_cv_k_modules/misc/streams-inet.o ) 2>/dev/null ; then
                ss7_cv_have_inet=yes
            fi
        fi
    ])
    AC_ARG_WITH([inet],
        AS_HELP_STRING([--with-inet],
            [include inet package.  @<:@default=DETECTED@:>@]),
        [with_inet=$enableval],
        [with_inet=''])
    AC_MSG_CHECKING([for package need inet driver])
    case :"$with_inet" in
        :yes) ss7_cv_need_inet=yes ;;
        :no)  ss7_cv_need_inet=no ;;
        :*) if test :"$ss7_cv_have_inet" = :yes
            then ss7_cv_need_inet=no
            else ss7_cv_need_inet=yes
            fi ;;
    esac
    AC_MSG_RESULT([$ss7_cv_need_inet])
    if test :"$ss7_cv_need_inet" = :yes ; then
        :
        _SS7_SETUP_INET
    fi
    AM_CONDITIONAL([WITH_INET], test :"$ss7_cv_need_inet" = :yes )
])# _SS7_CHECK_INET
# =========================================================================

# =========================================================================
# _SS7_CHECK_XNET
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_XNET], [
    AC_CACHE_CHECK([for package have xnet library], [ss7_cv_have_xnet], [
        ss7_cv_have_xnet="$ac_cv_lib_xnet_t_open"
    ])
    AC_ARG_WITH([xnet],
        AS_HELP_STRING([--with-xnet],
            [include xnet library.  @<:@default=DETECTED@:>@]),
        [with_xnet=$enableval],
        [with_xnet=''])
    AC_MSG_CHECKING([for package need xnet library])
    case :"$with_xnet" in
        :yes) ss7_cv_need_xnet=yes ;;
        :no)  ss7_cv_need_xnet=no ;;
        :*) if test :"$ss7_cv_have_xnet" = :yes
            then ss7_cv_need_xnet=no
            else ss7_cv_need_xnet=yes
            fi ;;
    esac
    AC_MSG_RESULT([$ss7_cv_need_xnet])
    if test :"$ss7_cv_need_xnet" = :yes ; then
        :
        _SS7_SETUP_XNET
    fi
    AM_CONDITIONAL([WITH_XNET], test :"$ss7_cv_need_xnet" = :yes )
])# _SS7_CHECK_XNET
# =========================================================================

# =========================================================================
# _SS7_CHECK_SOCK
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_CHECK_SOCK], [
    AC_CACHE_CHECK([for package have socket library], [ss7_cv_have_sock], [
        ss7_cv_have_sock=no
    ])
    AC_ARG_WITH([sock],
        AS_HELP_STRING([--with-sock],
            [include socket library.  @<:@default=DETECTED@:>@]),
        [with_sock=$enableval],
        [with_sock=''])
    AC_MSG_CHECKING([for package need socket library])
    case :"$with_sock" in
        :yes) ss7_cv_need_sock=yes ;;
        :no)  ss7_cv_need_sock=no ;;
        :*) if test :"$ss7_cv_have_sock" = :yes
            then ss7_cv_need_sock=no
            else ss7_cv_need_sock=yes
            fi ;;
    esac
    AC_MSG_RESULT([$ss7_cv_need_sock])
    if test :"$ss7_cv_need_sock" = :yes ; then
        :
        _SS7_SETUP_SOCK
    fi
    AM_CONDITIONAL([WITH_SOCK], test :"$ss7_cv_need_sock" = :yes )
])# _SS7_CHECK_SOCK
# =========================================================================

# =========================================================================
# _SS7_SETUP_SCTP
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_SCTP], [
    AC_CACHE_CHECK([for package OpenSS7 Kernel SCTP], [ss7_cv_openss7_sctp], [
        _LINUX_KERNEL_ENV([
            AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
            ], [ss7_cv_openss7_sctp=yes], [ss7_cv_openss7_sctp=no])
        ])
    ])
    if test :"${ss7_cv_openss7_sctp:-no}" = :yes ; then
        AC_DEFINE([HAVE_OPENSS7_SCTP], [1],
            [Define if your kernel supports the OpenSS7 Linux Kernel Sockets SCTP patches.  This
            enables support in the INET driver for STREAMS on top of the OpenSS7 Linux Kernel
            Sockets SCTP implementation.])
    fi
])# _SS7_SETUP_SCTP
# =========================================================================

# =========================================================================
# _SS7_SETUP_XNS
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_XNS], [
])# _SS7_SETUP_XNS
# =========================================================================

# =========================================================================
# _SS7_SETUP_TLI
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_TLI], [
])# _SS7_SETUP_TLI
# =========================================================================

# =========================================================================
# _SS7_SETUP_INET
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
AC_DEFUN([_SS7_SETUP_INET], [
    AC_CACHE_CHECK([for package OpenSS7 Kernel SCTP], [ss7_cv_openss7_sctp], [
        _LINUX_KERNEL_ENV([
            AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
            ], [ss7_cv_openss7_sctp=yes], [ss7_cv_openss7_sctp=no])
        ])
    ])
    if test :"${ss7_cv_openss7_sctp:-no}" = :yes ; then
        AC_DEFINE([HAVE_OPENSS7_SCTP], [1],
            [Define if your kernel supports the OpenSS7 Linux Kernel Sockets SCTP patches.  This
            enables support in the INET driver for STREAMS on top of the OpenSS7 Linux Kernel
            Sockets SCTP implementation.])
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
            [ss7_cv_af_inet_ttl_member_name='ttl'],
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
            [ss7_cv_af_inet_ttl_member_name='uc_ttl'],
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
    if test :"${ss7_cv_af_inet_ttl_member_name:+set}" = :set ; then
        AC_DEFINE_UNQUOTED([USING_AF_INET_TTL_MEMBER_NAME], [$ss7_cv_af_inet_ttl_member_name],
            [Most kernels call the time-to-live member of the af_inet structure ttl.  For some
            reason (probably because the old ttl member as 'int' and the new uc_ttl member is
            'unsigned char') reported by Bala Viswanathan <balav@lsil.com> to the linux-streams
            mailing list, EL3 renames the member to uc_ttl on some kernels.  Define this to the
            member name used (ttl or uc_ttl) so that the inet driver can be properly supported.  If
            this is not defined, 'ttl' will be used as a default.])
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
])# _SS7_SETUP_INET
# =========================================================================

# =========================================================================
# _SS7_SETUP_XNET
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_XNET], [
])# _SS7_SETUP_XNET
# =========================================================================

# =========================================================================
# _SS7_SETUP_SOCK
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP_SOCK], [
])# _SS7_SETUP_SOCK
# =========================================================================

# =========================================================================
# _SS7_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_OUTPUT], [
    _SS7_STRCONF
])# _SS7_OUTPUT
# =========================================================================

# =========================================================================
# _SS7_STRCONF
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_STRCONF], [
    strconf_cv_stem='lis.conf'
dnl strconf_cv_input='Config.master'
    strconf_cv_majbase=180
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes='ss7makenodes.c'
dnl strconf_cv_stsetup='strsetup.conf'
dnl strconf_cv_strload='strload.conf'
    _STRCONF
])# _SS7_STRCONF
# =========================================================================

# =========================================================================
# _SS7_
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_], [
])# _SS7_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
