dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.5 2004/12/18 11:15:25 brian Exp $
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
dnl Last Modified $Date: 2004/12/18 11:15:25 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/openss7.m4])
m4_include([m4/kernel.m4])
m4_include([m4/streams.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/strconf.m4])

# =========================================================================
# AC_SCTP
# -------------------------------------------------------------------------
AC_DEFUN([AC_SCTP], [dnl
    _OPENSS7_PACKAGE([SCTP], [OpenSS7 STREAMS SCTP])
    ac_default_prefix='/usr'
    _SCTP_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    USER_LDFLAGS="${LDADD}"
    _LINUX_KERNEL
    _LINUX_STREAMS
    _XNS
    _XTI
    SCTP_INCLUDES="-I- -imacros ./config.h${XNS_CPPFLAGS:+ }${XNS_CPPFLAGS}${XTI_CPPFLAGS:+ }${XTI_CPPFLAGS}${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS} -I./src/include -I${srcdir}/src/include"
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    INCLUDES  = $SCTP_INCLUDES])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([SCTP_INCLUDES])dnl
    CPPFLAGS=
    CFLAGS=
    _SCTP_SETUP
    _SCTP_OUTPUT dnl
])# AC_SCTP
# =========================================================================

# =========================================================================
# _SCTP_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPTIONS], [dnl
    _SCTP_OPENSS7_SCTP
    _SCTP_CHECK_SCTP
])# _SCTP_OPTIONS
# =========================================================================

# =========================================================================
# _SCTP_OPENSS7_SCTP
# -------------------------------------------------------------------------
# Check if we have OpenSS7 Kernel SCTP implementation.  If we do, we should
# warn or something.
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPENSS7_SCTP], [dnl
    AC_CACHE_CHECK([for sctp openss7 kernel], [sctp_cv_openss7_sctp], [dnl
        _LINUX_KERNEL_ENV([dnl
            AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/sctp.h>
#ifdef SCTPCB_FLAG_CONF
    yes_we_have_openss7_kernel_sctp
#endif
            ], [sctp_cv_openss7_sctp=yes], [sctp_cv_openss7_sctp=no]) ]) ])
    AC_MSG_CHECKING([for sctp openss7 sctp kernel])
    if test :"${sctp_cv_openss7_sctp:-no}" = :yes ; then
        AC_DEFINE([HAVE_OPENSS7_SCTP], [1],
        [Define if your kernel supports the OpenSS7 Linux Kernel Sockets SCTP
        patches.  This enables support in the SCTP driver for STREAMS on top
        of the OpenSS7 Linux Kernel Sockets SCTP implementation.])
    fi
    AC_MSG_RESULT([${sctp_cv_openss7_sctp:-no}])
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], [test :"${sctp_cv_openss7_sctp:-no}" = :yes])dnl
])# _SCTP_OPENSS7_SCTP
# =========================================================================

# =========================================================================
# _SCTP_CHECK_SCTP
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_CHECK_SCTP], [dnl
    AC_ARG_WITH([sctp],
        AS_HELP_STRING([--with-sctp],
            [include sctp version 1 driver in build.  @<:@default=no@:>@]),
        [with_sctp="$withval"],
        [with_sctp='no'])
    AC_ARG_WITH([sctp2],
        AS_HELP_STRING([--with-sctp2],
            [include sctp version 2 driver in build.  @<:@default=yes@:>@]),
        [with_sctp2="$withval"],
        [with_sctp2='yes'])
    AC_MSG_CHECKING([for sctp version])
    if test :"$with_sctp2" = :yes ; then
        with_sctp='no'
        sctp_cv_sctp_version=2
        AC_DEFINE_UNQUOTED([SCTP_VERSION_2], [], [
            Define for SCTP Version 2.  This define is needed by test programs
            and other programs that need to determine the difference between
            the address format and options conventions for the two versions.])
    else
        with_sctp='yes'
        sctp_cv_sctp_version=1
        AC_DEFINE_UNQUOTED([SCTP_VERSION_1], [], [
            Define for SCTP Version 1.  This define is needed by test programs
            and other programs that need to determine the difference between
            the address format and options conventions for the two versions.])
    fi
    AM_CONDITIONAL([WITH_SCTP], [test :"$sctp_cv_sctp_version" = :1])dnl
    AM_CONDITIONAL([WITH_SCTP2], [test :"$sctp_cv_sctp_version" = :2])dnl
    AC_DEFINE_UNQUOTED([SCTP_VERSION], [$sctp_cv_sctp_version], [
        Define to 1 for SCTP Version 1.  Define to 2 for SCTP Version 2.  This
        define is needed by test programs that must determine the difference
        between the address format and options conventions for the two
        versions.])
    AC_MSG_RESULT([$sctp_cv_sctp_version])
])# _SCTP_CHECK_SCTP
# =========================================================================

# =========================================================================
# _SCTP_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP], [dnl
    _LINUX_KERNEL_ENV([dnl
        AC_CACHE_CHECK([for sctp ip_route_output], [sctp_cv_have_ip_route_output], [dnl
            CFLAGS="$CFLAGS -Werror-implicit-function-declaration"
            AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[
#include <linux/config.h>
#include <linux/version.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>]],
                [[ip_route_output(NULL, 0, 0, 0, 0);]])
            ],
            [sctp_cv_have_ip_route_output='yes'],
            [sctp_cv_have_ip_route_output='no']) ])
    ])
    if test :"${sctp_cv_have_ip_route_output:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([HAVE_IP_ROUTE_OUTPUT], [], [Most 2.4 kernels have
        the function ip_route_output() defined.  Newer RH kernels (EL3) use
        the 2.6 functions and do not provide ip_route_output().  Define this
        macro if your kernel provides ip_route_output().])
    else
        _LINUX_KERNEL_SYMBOL_EXPORT([ip_route_output_flow], [dnl
            AC_MSG_ERROR([
**** 
**** To use the package on newer kernels requires the availability of the
**** function ip_route_output_flow() to permit calling ip_route_connect()
**** which is an inline in net/route.h.  I cannot find this symbol on your
**** system and the resulting kernel module will therefore not load.
****
                    ])
        ])
        _LINUX_KERNEL_SYMBOL_EXPORT([__ip_route_output_key], [dnl
            AC_MSG_ERROR([
**** 
**** To use the package on newer kernels requires the availability of the
**** function __ip_route_output_key() to permit calling ip_route_connect()
**** which is an inline in net/route.h.  I cannot find this symbol on your
**** system and the resulting kernel module will therefore not load.
****
                    ])
        ])
    fi
    AC_REQUIRE([_SCTP_OPENSS7_SCTP])dnl
dnl if test :"${sctp_cv_openss7_sctp:-no}" = :yes ; then
dnl     with_sctp='no'
dnl     with_sctp2='no'
dnl fi
    if test :"${with_sctp2:-no}" = :yes ; then
        with_sctp='no'
    fi
    if test :"${with_sctp:-no}" = :yes -o :"${with_sctp2:-no}" = :yes ; then
        _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_update_pmtu], [with_sctp='no'; with_sctp2='no'])
    fi
    if test :"${with_sctp:-no}" = :yes -o :"${with_sctp2:-no}" = :yes ; then
        _LINUX_KERNEL_ENV([dnl
            AC_CHECK_MEMBER([struct inet_protocol.protocol],
                [sctp_cv_inet_protocol_style='old'],
                [:], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/protocol.h>
                ])
            AC_CHECK_MEMBER([struct inet_protocol.no_policy],
                [sctp_cv_inet_protocol_style='new'],
                [:], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/protocol.h>
                ])
            AC_CHECK_MEMBER([struct dst_entry.path],
                [sctp_cv_dst_entry_path='yes'],
                [sctp_cv_dst_netry_path='no'], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/dst.h>
                ])
            ])
    fi
    if test :"${sctp_cv_inet_protocol_style:+set}" = :set ; then
        case "$sctp_cv_inet_protocol_style" in
            old)
                AC_DEFINE_UNQUOTED([HAVE_OLD_STYLE_INET_PROTOCOL], [], [Most
                2.4 kernels have the old style struct inet_protocol and the
                old prototype for inet_add_protocol() and inet_del_protocol()
                defined in <net/protocol.h>.  Some more recent RH kernels
                (such as EL3) use the 2.6 style of structure and prototypes.
                Define this macro if your kernel has the old style structure
                and prototypes.])
                ;;
            new)
                AC_DEFINE_UNQUOTED([HAVE_NEW_STYLE_INET_PROTOCOL], [], [Most
                2.4 kernels have the old style struct inet_protocol and the
                old prototype for inet_add_protocol() and inet_del_protocol()
                defined in <net/protocol.h>.  Some more recent RH kernels
                (such as EL3) use the 2.6 style of structure and prototypes.
                Define this macro if your kernel has the new style structure
                and prototypes.])
                ;;
        esac
    else
        with_sctp='no'
        with_sctp2='no'
    fi
    if test :"${sctp_cv_dst_entry_path:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([HAVE_STRUCT_DST_ENTRY_PATH], [], [Newer RHEL3
        kernels change the destination entry structure.  Define this macro to
        use the newer structure.])
    fi
    _LINUX_KERNEL_ENV([
        AC_CHECK_TYPES([struct sockaddr_storage], [
            AC_DEFINE_UNQUOTED([HAVE_STRUCT_SOCKADDR_STORAGE], [], [Most 2.4
                kernels do not define struct sockaddr_storage.  Define to 1 if
                your kernel supports struct sockaddr_storage.])], [:], [
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
    _LINUX_KERNEL_SYMBOL_EXPORT([icmp_statistics])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_nonlocal_bind])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_dynaddr])
    _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_min_pmtu])
    _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_mtu_expires])
])# _SCTP_SETUP
# =========================================================================

# =========================================================================
# _SCTP_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OUTPUT], [dnl
    _SCTP_CONFIG
    _SCTP_STRCONF dnl
])# _SCTP_OUTPUT
# =========================================================================

# =========================================================================
# _SCTP_CONFIG
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_CONFIG], [dnl
# SCTP_CONFIG_SLOW_VERIFICATION
    AC_MSG_CHECKING([for sctp slow verification])
    AC_ARG_ENABLE([sctp-slow-verification],
        AS_HELP_STRING([--enable-sctp-slow-verification],
            [enable slow verification of addresses and tags. @<:@default=no@:>@]),
        [sctp_cv_slow_verification="$enableval"],
        [sctp_cv_slow_verification='no'])
    if test :"${sctp_cv_slow_verification:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_SLOW_VERIFICATION], [], [
            When a message comes from an SCTP endpoint with the correct
            verification tag, it is not necessary to check ports or addresses
            to identify the SCTP association to which it belongs.  When
            undefined, port numbers and addresses are not checked on local
            tags and the addresses are not checked on peer tags.  When
            defined, the redundant port number and destination address checks
            are performed.  Both situations provide RFC 2960 compliant
            operation.  If in doubt, leave this undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_slow_verification])
# SCTP_CONFIG_THROTTLE_HEARTBEATS
    AC_MSG_CHECKING([for sctp throttle heartbeats])
    AC_ARG_ENABLE([sctp-throttle-heartbeats],
        AS_HELP_STRING([--enable-sctp-throttle-heartbeats],
            [enable heartbeat throttling. @<:@default=no@:>@]),
        [sctp_cv_throttle_heartbeats="$enableval"],
        [sctp_cv_throttle_heartbeats='no'])
    if test :"${sctp_cv_throttle_heartbeats:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_THROTTLE_HEARTBEATS], [], [
            Special feature of OpenSS7 SCTP which is not mentioned in RFC
            2960.  When defined, SCTP will throttle the rate at which it
            responds to heartbeats to the system control sctp_heartbeat_itvl.
            This makes SCTP more reslilient to implementations which flood
            heartbeat messages.  For RFC 2960 compliant operation, leave this
            undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_throttle_heartbeats])
    AC_MSG_CHECKING([for sctp dicard out-of-the-blue])
    AC_REQUIRE([_SCTP_OPENSS7_SCTP])dnl
    AC_ARG_ENABLE([sctp-discard-ootb],
        AS_HELP_STRING([--enable-sctp-discard-ootb],
            [enable discard out-of-the-blue packets. @<:@default=no@:>@]),
        [sctp_cv_discard_ootb="$enableval"],
        [if test :"${sctp_cv_openss7_sctp:-no}" = :yes ; then
            sctp_cv_discard_ootb='yes'
         else
            sctp_cv_discard_ootb='no'
         fi])
    if test :"$sctp_cv_discard_ootb" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_DISCARD_OOTB], [], [
            RFC 2960 requires the implementation to send ABORT to some OOTB
            packets (packets for which no SCTP association exists).  Sending
            ABORT chunks to unverified source addreses with the T bit set
            opens SCTP to blind masquerade attacks.  Not sending them may lead
            to delays at the peer endpoint aborting associations where our
            ABORT ahs been lost and the stream is already closes or if we have
            restarted and the peer still has open associations to us.  If
            defined, SCTP will discard all OOTB packets.  This is necessary if
            another SCTP stack is being run on the same machine.  For RFC 2960
            compliant operation, leave undefined.  If in doubt, leave this
            undefined.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_discard_ootb])
# SCTP_CONFIG_EXTENDED_IP_SUPPORT
    AC_MSG_CHECKING([for sctp extended ip support])
    AC_ARG_ENABLE([sctp-extended-ip-support],
        AS_HELP_STRING([--enable-sctp-extended-ip-support],
            [enable extended IP support for SCTP. @<:@default=disabled@:>@]),
        [sctp_cv_extended_ip_support="$enableval"],
        [sctp_cv_extended_ip_support='no'])
    if test :"${sctp_cv_extended_ip_support:-no}" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_EXTENDED_IP_SUPPORT], [], [
            This provides extended IP support for SCTP for things like IP
            Transparent Proxy and IP Masquerading.  This is experimental
            stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_extended_ip_support])
    AC_C_BIGENDIAN(
        [sctp_cv_be_machine='yes'; sctp_cv_le_machine='no'],
        [sctp_cv_be_machine='no'; sctp_cv_le_machine='yes'],
        [sctp_cv_be_machine='yes'; sctp_cv_le_machine='yes'])
# SCTP_CONFIG_HMAC_SHA1
    AC_MSG_CHECKING([for sctp hmac sha-1])
    AC_ARG_ENABLE([sctp-hmac-sha1],
        AS_HELP_STRING([--disable-sctp-hmac-sha1],
            [disable SHA-1 HMAC. @<:@default=enabled(BE),disabled(LE)@:>@]),
        [sctp_cv_hmac_sha1="$enableval"],
        [sctp_cv_hmac_sha1="$sctp_cv_be_machine"])
    if test :"$sctp_cv_hmac_sha1" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_HMAC_SHA1], [], [
            When defined, this provides the ability to sue the FIPS 180-1
            (SHA-1) message authentication code in SCTP cookies.  When
            defined and the appropriate sysctl and option is set, SCTP will
            use SHA-1 HMAC when signing cookies in the INIT-ACK chunk.  If
            undefined, the SHA-1 HMAC will be unavailable for use with SCTP.
            If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_SHA1], [test :"$sctp_cv_hmac_sha1" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_hmac_sha1])
# SCTP_CONFIG_HMAC_MD5
    AC_MSG_CHECKING([for sctp hmac md5])
    AC_ARG_ENABLE([sctp-hmac-md5],
        AS_HELP_STRING([--disable-sctp-hmac-md5],
            [disable MD5 HMAC. @<:@default=enabled(LE),disabled(BE)@:>@]),
        [sctp_cv_hmac_md5="$enableval"],
        [sctp_cv_hmac_md5="$sctp_cv_le_machine"])
    if test :"$sctp_cv_hmac_md5" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_HMAC_MD5], [], [
            When defined, this provides the ability to use the MD5 (RFC 1321)
            message authentication code in SCTP cookies.  If you define this
            macro, when the appropriate system control and stream option is
            set, SCTP will use the MD5 HMAC when signing cookies in the
            INIT-ACK chunk.  If this macro is undefined, the HD5 HMAC will be
            unavailable for use with SCTP.  If in doubt, leave defined.])dnl
    fi
    AM_CONDITIONAL([WITH_HMAC_MD5], [test :"$sctp_cv_hmac_md5" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_hmac_md5])
# SCTP_CONFIG_ADLER_32
    AC_MSG_CHECKING([for sctp Adler32 checksum])
    AC_ARG_ENABLE([sctp-adler32],
        AS_HELP_STRING([--enable-sctp-adler32],
            [enable Adler32 checksum. @<:@default=disabled@:>@]),
        [sctp_cv_adler32="$enableval"],
        [sctp_cv_adler32='no'])
    if test :"$sctp_cv_adler32" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_ADLER_32], [], [
            This provides the ability to use the older RFC 2960 Adler32
            checksum.  If CONFIG_SCTP_CRC_32 below is not selected, the Adler32
            checksum is always provided.])dnl
    fi
    AM_CONDITIONAL([WITH_ADLER_32], [test :"$sctp_cv_adler32" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_adler32])
# SCTP_CONFIG_CRC_32C
    AC_MSG_CHECKING([for sctp CRC-32C checksum])
    AC_ARG_ENABLE([sctp-crc32c],
        AS_HELP_STRING([--disable-sctp-crc32c],
            [disable CRC-32C checksum. @<:@default=enabled@:>@]),
        [sctp_cv_crc32c="$enableval"],
        [sctp_cv_crc32c='yes'])
    if test :"${sctp_cv_adler32:-no}" != :yes ; then
        sctp_cv_crc32c='yes'
    fi
    if test :"$sctp_cv_crc32c" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_CRC_32C], [], [
            This provides the ability to use the newer CRC-32c checksum as
            described in RFC 3309.  When this is selected and
            CONFIG_SCTP_ADLER_32 is not selected above, then the only checksum
            that will be used is the CRC-32c checksum.])dnl
    fi
    AM_CONDITIONAL([WITH_CRC_32C], [test :"$sctp_cv_crc32c" = :yes])dnl
    AC_MSG_RESULT([$sctp_cv_crc32c])
# SCTP_CONFIG_THROTTLE_PASSIVEOPENS
    AC_MSG_CHECKING([for sctp throttle passive opens])
    AC_ARG_ENABLE([sctp-throttle-passiveopens],
        AS_HELP_STRING([--enable-sctp-throttle-passiveopens],
            [enable throttling of passive opens. @<:@default=disabled@:>@]),
        [sctp_cv_throttle_passiveopens="$enableval"],
        [sctp_cv_throttle_passiveopens='no'])
    if test :"$sctp_cv_throttle_passiveopens" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_THROTTLE_PASSIVEOPENS], [], [
            Special feature of Linux SCTP not mentioned in RFC 2960.  When
            secure algorithms are used for signing cookies, the implementation
            becomes vulnerable to INIT and COOKIE ECHO flooding.  If defined,
            SCTP will only allow one INIT and one COOKE ECHO to be processed in
            each interval corresponding to the sysctl sctp_throttle_itvl.
            Setting sctp_throttle_itvl to 0 defeats this function.  If
            undefined, each INIT and COOKIE ECHO will be processed.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_throttle_passiveopens])
# SCTP_CONFIG_ECN
    AC_MSG_CHECKING([for sctp ecn])
    AC_ARG_ENABLE([sctp-ecn],
        AS_HELP_STRING([--enable-sctp-ecn],
            [enable Explicit Congestion Notification. @<:@default=disabled@:>@]),
        [sctp_cv_ecn="$enableval"],
        [sctp_cv_ecn='no'])
    if test :"$sctp_cv_ecn" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_ECN], [], [
            This enables support for Explicit Congestion Notification (ECN)
            chunks in SCTP messages as defined in RFC 2960 and RFC 3168.  It
            also adds syctl (/proc/net/ipv4/sctp_ecn) which allows ECN for SCTP
            to be disabled at runtime.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_ecn])
# SCTP_CONFIG_LIFETIMES
    AC_MSG_CHECKING([for sctp lifetimes])
    AC_ARG_ENABLE([sctp-lifetimes],
        AS_HELP_STRING([--enable-sctp-lifetimes],
            [enable SCTP message lifetimes. @<:@default=disabled@:>@]),
        [sctp_cv_lifetimes="$enableval"],
        [sctp_cv_lifetimes='no'])
    if test :"$sctp_cv_lifetimes" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_LIFETIMES], [], [
            This enables support for message lifetimes as described in RFC 2960.
            When enabled, message lifetimes can be set on messages.  See
            sctp(7).  This feature is always enabled when Partial Reliability
            Support is set.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_lifetimes])
# SCTP_CONFIG_ADD_IP
    AC_MSG_CHECKING([for sctp add ip])
    AC_ARG_ENABLE([sctp-add-ip],
        AS_HELP_STRING([--enable-sctp-add-ip],
            [enable ADD-IP. @<:@default=disabled@:>@]),
        [sctp_cv_add_ip="$enableval"],
        [sctp_cv_add_ip='no'])
    if test :"$sctp_cv_add_ip" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_ADD_IP], [], [
            This enables support for ADD-IP as described in
            draft-ietf-tsvwg-addip-sctp-07.txt.  This allows the addition and
            removal of IP addresses from existing connections.  This is
            experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_add_ip])
# SCTP_CONFIG_ADAPTATION_LAYER_INFO
    AC_MSG_CHECKING([for sctp adaptation layer info])
    AC_ARG_ENABLE([sctp-adaptation-layer-info],
        AS_HELP_STRING([--enable-sctp-adaptation-layer-info],
            [enable ALI. @<:@default=disabled@:>@]),
        [sctp_cv_adaptation_layer_info="$enableval"],
        [sctp_cv_adaptation_layer_info='no'])
    if test :"$sctp_cv_adaptation_layer_info" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_ADAPTATION_LAYER_INFO], [], [
            This enables support for the Adaptation Layer Information parameter
            described in draft-ietf-tsvwg-addip-sctp-07.txt for communicating
            application layer information bits at initialization.  This is
            experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_adaptation_layer_info])
# SCTP_CONFIG_PARTIAL_RELIABILITY
    AC_MSG_CHECKING([for sctp partial reliability])
    AC_ARG_ENABLE([sctp-partial-reliability],
        AS_HELP_STRING([--enable-sctp-partial-reliability],
            [enable SCTP Partial Reliability (PR-SCTP). @<:@default=disabled@:>@]),
        [sctp_cv_partial_reliability="$enableval"],
        [sctp_cv_partial_reliability='no'])
    if test :"$sctp_cv_partial_reliability" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_PARTIAL_RELIABILITY], [], [
            This enables support for PR-SCTP as described in
            draft-stewart-tsvwg-prsctp-03.txt.  This allows for partial
            reliability of message delivery on a "timed reliability" basis.
            This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_partial_reliability])
# SCTP_CONFIG_ERROR_GENERATOR
    AC_MSG_CHECKING([for sctp error generator])
    AC_ARG_ENABLE([sctp-error-generator],
        AS_HELP_STRING([--disable-sctp-error-generator],
            [disable SCTP error generator. @<:@default=enabled@:>@]),
        [sctp_cv_error_generator="$enableval"],
        [sctp_cv_error_generator='yes'])
    if test :"$sctp_cv_error_generator" = :yes ; then
        AC_DEFINE_UNQUOTED([SCTP_CONFIG_ERROR_GENERATOR], [], [
            This provides an internal error generator that can be accessed with
            socket options for testing SCTP operation under packet loss.  You
            will need this option to run some of the test programs distributed
            with the SCTP module.])dnl
    fi
    AC_MSG_RESULT([$sctp_cv_error_generator])
])# _SCTP_CONFIG
# =========================================================================

# =========================================================================
# _SCTP_STRCONF
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
dnl strconf_cv_input='Config.master'
    strconf_cv_majbase=240
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="${PACKAGE_TARNAME}_mknod.c"
dnl strconf_cv_stsetup='strsetup.conf'
dnl strconf_cv_strload='strload.conf'
    AC_REQUIRE([_LINUX_STREAMS])
    strconf_cv_package=${streams_cv_package:-LiS}
    _STRCONF dnl
])# _SCTP_STRCONF
# =========================================================================

# =========================================================================
# _SCTP_
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_], [dnl
])# _SCTP_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
