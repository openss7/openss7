dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.9 2004/03/10 08:46:30 brian Exp $
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
dnl Last Modified $Date: 2004/03/10 08:46:30 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/kernel.m4])
m4_include([m4/rpm.m4])

# =========================================================================
# AC_LFS
# -------------------------------------------------------------------------
AC_DEFUN([AC_LFS], [
    _LFS_OPTIONS
    _LFS_SETUP_COMPAT
    _LFS_SETUP_PUBLIC
    _LFS_SETUP_DEBUG
    LFS_INCLUDES="-D_LFS_SOURCE=1 -I- -imacros ./config.h -I./include -I${srcdir}/include"
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    AC_SUBST([USER_CPPFLAGS])
    AC_SUBST([USER_CFLAGS])
    AC_SUBST([LFS_INCLUDES])
    AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user INCLUDES  = $LFS_INCLUDES])
    _LFS_SETUP
    AC_MSG_NOTICE([final kern MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kern NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kern CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kern CFLAGS    = $KERNEL_CFLAGS])
    CPPFLAGS=
    CFLAGS=
    _LFS_GET_STRCONF
dnl 
dnl Acutally generating output files is last.  We don't want to generate a thing
dnl until we have performed all the checks and balances.
dnl 
    AC_RPM_SPEC
    _LFS_OUTPUT_CONFIG_MASTER
    _LFS_OUTPUT_CONFIG
])# AC_LFS
# =========================================================================

# =========================================================================
# _LFS_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_OPTIONS], [
    AC_ARG_ENABLE([public],
                  AC_HELP_STRING([--enable-public],
                                 [enable public release. @<:@default=yes@:>@]),
                  [enable_public=$enableval],
                  [enable_public=''])
    AC_ARG_ENABLE([k-debug],
                  AC_HELP_STRING([--enable-k-debug],
                                 [enable kernel module run-time debugging.
                                 @<:@default=no@:>@]),
                  [enable_k_debug=$enableval],
                  [enable_k_debug=''])
    AC_ARG_ENABLE([k-test],
                  AC_HELP_STRING([--enable-k-test],
                                 [enable kernel module run-time testing.
                                 @<:@default=no@:>@]),
                  [enable_k_test=$enableval],
                  [enable_k_test=''])
    AC_ARG_ENABLE([k-safe],
                  AC_HELP_STRING([--enable-k-safe],
                                 [enable kernel module run-time safety checks.
                                 @<:default=no@:>@]),
                  [enable_k_safe=$enableval],
                  [enable_k_safe=''])
    AC_ARG_ENABLE([compat-svr4],
                  AC_HELP_STRING([--enable-compat-svr4],
                                 [enable source compatibility with SVR 4.2 MP
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_svr4=$enableval],
                  [enable_compat_svr4=''])
    AC_ARG_ENABLE([compat-sol8],
                  AC_HELP_STRING([--enable-compat-sol8],
                                 [enable source compatibility with Solaris 8
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_sol8=$enableval],
                  [enable_compat_sol8=''])
    AC_ARG_ENABLE([compat-uw7],
                  AC_HELP_STRING([--enable-compat-uw7],
                                 [enable source compatibility with UnixWare 7
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_uw7=$enableval],
                  [enable_compat_uw7=''])
    AC_ARG_ENABLE([compat-osf],
                  AC_HELP_STRING([--enable-compat-osf],
                                 [enable source compatibility with OSF/1.2
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_osf=$enableval],
                  [enable_compat_osf=''])
    AC_ARG_ENABLE([compat-aix],
                  AC_HELP_STRING([--enable-compat-aix],
                                 [enable source compatibility with AIX 4
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_aix=$enableval],
                  [enable_compat_aix=''])
    AC_ARG_ENABLE([compat-hpux],
                  AC_HELP_STRING([--enable-compat-hpux],
                                 [enable source compatibility with HPUX
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_hpux=$enableval],
                  [enable_compat_hpux=''])
    AC_ARG_ENABLE([compat-lis],
                  AC_HELP_STRING([--enable-compat-lis],
                                 [enable source compatibility with LiS
                                 variants.  @<:@default=no@:>@]),
                  [enable_compat_lis=$enableval],
                  [enable_compat_lis=''])
])# _LFS_OPTIONS
# =========================================================================

# =========================================================================
# _LFS_SETUP_PUBLIC
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_PUBLIC], [
    AC_CACHE_CHECK([for public release], [lfs_cv_public], [dnl
        if test :"$enable_public" = :no ; then
            lfs_cv_public=no
        else
            lfs_cv_public=yes
        fi
    ])
    AM_CONDITIONAL(LFS_PUBLIC_RELEASE, test :"$lfs_cv_public" = :yes)
])# _LFS_SETUP_PUBLIC
# =========================================================================

# =========================================================================
# _LFS_SETUP_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_DEBUG], [
    AC_CACHE_CHECK([for debugging], [lfs_cv_debug], [dnl
        if test :"$enable_k_debug" = :yes ; then
            lfs_cv_debug=_DEBUG
        else
            if test :"$enable_k_test" = :yes ; then
                lfs_cv_debug=_TEST
            else
                if test :"$enable_k_safe" != :no ; then
                    lfs_cv_debug=_SAFE
                else
                    lfs_cv_debug=_NONE
                fi
            fi
        fi
    ])
    case "$lfs_cv_debug" in
        _DEBUG)
            AC_DEFINE_UNQUOTED([_DEBUG], [], [Define for kernel symbol
            debugging.  This has the effect of defeating inlines, making
            static declarations global, and activating all debugging macros.])
            ;;
        _TEST)
            AC_DEFINE_UNQUOTED([_TEST], [], [Define for kernel testing.  This
            has the same effect as _DEBUG for now.])
            ;;
        _SAFE)
            AC_DEFINE_UNQUOTED([_SAFE], [], [Define for kernel safety.  This
            has the effect of enabling safety debugging macros.  This is the
            default.])
            ;;
        *)
            AC_DEFINE_UNQUOTED([_NONE], [], [Define for maximum performance
            and minimum size.  This has the effect of disabling all safety
            debugging macros.])
            ;;
    esac
    case "$lfs_cv_debug" in
        _DEBUG | _TEST)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DEBUG], [], [Define to peform
            internal structure tracking within the STREAMS executive as well
            as to provide additional /proc filesystem files for examining
            internal structures.]
            ;;
    esac
])# _LFS_SETUP_DEBUG
# =========================================================================

# =========================================================================
# _LFS_SETUP_COMPAT
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_COMPAT], [
    AC_CACHE_CHECK([for UNIX(R) SVR 4.2 compatibility], [lfs_cv_svr4], [dnl
        if test :"$enable_compat_svr4" != :no ; then lfs_cv_svr4=yes ; else lfs_cv_svr4=no ; fi
        if test :"$enable_compat_sol8" != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_uw7"  != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_osf"  != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_aix"  != :no ; then lfs_cv_svr4=yes ; fi
        if test :"$enable_compat_hpux" != :no ; then lfs_cv_svr4=yes ; fi
    ])
    AC_CACHE_CHECK([for Solaris(R) 8 compatibility], [lfs_cv_sol8], [dnl
        if test :"$enable_compat_sol8" != :no ; then lfs_cv_sol8=yes ; else lfs_cv_sol8=no ; fi
    ])
    AC_CACHE_CHECK([for UnixWare(R) 7 compatibility], [lfs_cv_uw7], [dnl
        if test :"$enable_compat_uw7" != :no  ; then lfs_cv_uw7=yes  ; else lfs_cv_uw7=no  ; fi
    ])
    AC_CACHE_CHECK([for OSF/1.2 compatibility], [lfs_cv_osf], [dnl
        if test :"$enable_compat_osf" != :no  ; then lfs_cv_osf=yes  ; else lfs_cv_osf=no  ; fi
    ])
    AC_CACHE_CHECK([for AIX(R) 4 compatibility], [lfs_cv_aix], [dnl
        if test :"$enable_compat_aix" != :no  ; then lfs_cv_aix=yes  ; else lfs_cv_aix=no  ; fi
    ])
    AC_CACHE_CHECK([for HPUX(R) compatibility], [lfs_cv_hpux], [dnl
        if test :"$enable_compat_hpux" != :no ; then lfs_cv_hpux=yes ; else lfs_cv_hpux=no ; fi
    ])
    AC_CACHE_CHECK([for LiS compatibility], [lfs_cv_lis], [dnl
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
# _LFS_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP], [
    AC_LINUX_KERNEL
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LFS_CHECK_KERNEL
    # all our config substitutions
    LFS_SC_MAJBASE=230
    LFS_SC_CONFIG='include/sys/config.h'
    LFS_SC_MODCONF='modconf.h'
    LFS_SC_DRVCONF='drvrconf.mk'
    LFS_SC_CONFMOD='conf.modules'
    LFS_SC_MAKEDEV='devices.lst'
    LFS_SC_MKNODES='src/util/strmakenodes.c'
    LFS_SC_STSETUP='strsetup.conf'
    LFS_SC_STRLOAD='strload.conf'
    # all our config substitutions
    AC_SUBST(LFS_SC_CONFIG)
    AC_SUBST(LFS_SC_CONFMOD)
    AC_SUBST(LFS_SC_MAKEDEV)
    AC_SUBST(LFS_SC_DRVCONF)
    AC_SUBST(LFS_SC_MAJBASE)
    AC_SUBST(LFS_SC_MKNODES)
    AC_SUBST(LFS_SC_MODCONF)
    AC_SUBST(LFS_SC_STSETUP)
    AC_SUBST(LFS_SC_STRLOAD)
])# _LFS_SETUP
# =========================================================================

# =========================================================================
# _LFS_GET_STRCONF
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_GET_STRCONF], [
    AC_REQUIRE([_LFS_SETUP])
    lfs_configs=
    for lfs_config in `find $srcdir -type f -name "Config"`
    do
        if test -r "$lfs_config" ; then
            lfs_configs="$lfs_configs $lfs_config"
        fi
    done
    AC_SUBST(lfs_configs)
])# _LFS_GET_STRCONF
# =========================================================================

# =============================================================================
# _LFS_OUTPUT_CONFIG_MASTER
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_OUTPUT_CONFIG_MASTER], [
    if test -n "$lfs_configs" ; then
dnl     Note that we never generate Config.master in the makefiles, we generate
dnl     it once here.
        cat $lfs_configs > Config.master
    fi
])# _LFS_OUTPUT_CONFIG_MASTER
# =============================================================================

# =============================================================================
# _LFS_OUTPUT_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_OUTPUT_CONFIG], [
          if test :"${LFS_SC_CONFIG:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_CONFIG from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --hconfig=$LFS_SC_CONFIG $lfs_configs"
          fi
          if test :"${LFS_SC_MODCONF:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_MODCONF from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --modconf=$LFS_SC_MODCONF $lfs_configs"
          fi
          if test :"${LFS_SC_MKNODES:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_MKNODES from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --makenodes=$LFS_SC_MKNODES $lfs_configs"
          fi
          if test :"${LFS_SC_DRVCONF:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_DRVCONF from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --driverconf=$LFS_SC_DRVCONF $lfs_configs"
          fi
          if test :"${LFS_SC_CONFMOD:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_CONFMOD from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --confmodules=$LFS_SC_CONFMOD $lfs_configs"
          fi
          if test :"${LFS_SC_MAKEDEV:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_MAKEDEV from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --strmknods=$LFS_SC_MAKEDEV $lfs_configs"
          fi
          if test :"${LFS_SC_STSETUP:+set}" = :set; then
              AC_MSG_NOTICE([creating $LFS_SC_STSETUP from $lfs_configs])
              eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --strsetup=$LFS_SC_STSETUP $lfs_configs"
          fi
#         if test :"${LFS_SC_STRLOAD:+set}" = :set; then
#             AC_MSG_NOTICE([creating $LFS_SC_STRLOAD from $lfs_configs])
#             eval "$SHELL $ac_aux_dir/strconf-sh -b${LFS_SC_MAJBASE} --strload=$LFS_SC_STRLOAD $lfs_configs"
#         fi
])# _LFS_OUTPUT_CONFIG
# =============================================================================

# =========================================================================
# _LFS_CHECK_KERNEL
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_KERNEL], [
    _LFS_CONFIG_INET
    _LFS_CONFIG_FATTACH
    _LFS_CONFIG_LIS
    _LFS_CONFIG_LFS
])# _LFS_CHECK_KERNEL
# =========================================================================

# =========================================================================
# _LFS_CONFIG_INET
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_INET], [
    AC_CACHE_CHECK([for tcp_openreq_cachep support], [lfs_cv_tcp_openreq_cachep], [dnl
        tmp_cppflags="$CPPFLAGS"
        CPPFLAGS="$KERNEL_CPPFLAGS"
        AC_EGREP_CPP([\<yes_we_have_the_required_support\>], [
#include <linux/config.h>
#include <linux/version.h>
#ifdef CONFIG_MODVERSIONS
#include <linux/modversion.h>
#endif
#include <linux/types.h>
#include <linux/fs.h>
#ifdef CONFIG_IPV6_MODULE
    yes_we_have_the_required_support
#endif
        ], [lfs_cv_tcp_openreq_cachep=yes], [lfs_cv_tcp_openreq_cachep=no])
        CPPFLAGS="$tmp_cppflags"
    ])
    if test :"$lfs_cv_tcp_openreq_cachep" = :no ; then
        AC_MSG_CHECKING([usable tcp_openreq_cachep address])
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_tcp_openreq_cachep_addr=`($EGREP '\<tcp_openreq_cachep' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
            lfs_cv_tcp_openreq_cachep_addr="${lfs_cv_tcp_openreq_cachep_addr:+0x}$lfs_cv_tcp_openreq_cachep_addr"
        fi
        if test -n "$lfs_cv_tcp_openreq_cachep_addr" ; then
            lfs_cv_tcp_openreq_cachep=yes
            AC_DEFINE_UNQUOTED([HAVE_TCP_OPENREQ_CACHEP_ADDR], [$lfs_cv_tcp_openreq_cachep_addr],
            [If the symbol tcp_openreq_cachep is not exported by your kernel,
            then define this to the address of the tcp_openreq_cachep in the
            kernel system map so that the INET driver can find the cache point
            to properly allocate and deallocate requests.])
            AC_MSG_RESULT([$lfs_cv_openreq_cachep_addr])
        else
            :
            AC_MSG_RESULT([no])
        fi
    fi
    if test :"$lfs_cv_tcp_openreq_cachep" = :yes ; then
        AC_DEFINE([HAVE_TCP_OPENREQ_CACHEP], [1], [Define if your kernel
        exports symbol tcp_openreq_cachep.  This symbol is needed by the INET
        driver to properly accept connection indications.])
    fi
])# _LFS_CONFIG_INET
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
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_FATTACH], [
    AC_CACHE_CHECK([for usable mount_sem address], [lfs_cv_mount_sem_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_mount_sem_addr=`($EGREP '\<mount_sem' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_mount_sem_addr="${lfs_cv_mount_sem_addr:+0x}$lfs_cv_mount_sem_addr"
        lfs_cv_mount_sem_addr="${lfs_cv_mount_sem_addr:-no}"
    ])
    if test :${lfs_cv_mount_sem_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_MOUNT_SEM_ADDR], [$lfs_cv_mount_sem_addr],
        [The symbol mount_sem is not exported by most kernels.  Define this to
        the address of the mount_sem in the kernel system map so that
        fattach/fdetach can be properly supported.])
    else
        AC_DEFINE_UNQUOTED([HAVE_TASK_NAMESPACE_SEM], [1],
        [Some recent 2.4 RH kernel place the mount semaphore into the task
        structure rather than using the static global mount_sem semaphore.
        Define this if you have a modified kernel.])
    fi
    AC_CACHE_CHECK([for usable clone_mnt address], [lfs_cv_clone_mnt_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_clone_mnt_addr=`($EGREP '\<clone_mnt' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_clone_mnt_addr="${lfs_cv_clone_mnt_addr:+0x}$lfs_cv_clone_mnt_addr"
        lfs_cv_clone_mnt_addr="${lfs_cv_clone_mnt_addr:-no}"
    ])
    if test :${lfs_cv_clone_mnt_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_CLONE_MNT_ADDR], [$lfs_cv_clone_mnt_addr],
        [The symbol clone_mnt is not exported by most kernels.  Define this to
        the address of clone_mnt in the kernel system map so that
        fattach/fdetach can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable check_mnt address], [lfs_cv_check_mnt_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_check_mnt_addr=`($EGREP '\<check_mnt' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_check_mnt_addr="${lfs_cv_check_mnt_addr:+0x}$lfs_cv_check_mnt_addr"
        lfs_cv_check_mnt_addr="${lfs_cv_check_mnt_addr:-no}"
    ])
    if test :${lfs_cv_check_mnt_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_CHECK_MNT_ADDR], [$lfs_cv_check_mnt_addr],
        [The symbol check_mnt is not exported by most kernels.  Define this to
        the address of check_mnt in the kernel system map so that
        fattach/fdetach can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable graft_tree address], [lfs_cv_graft_tree_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_graft_tree_addr=`($EGREP '\<graft_tree' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_graft_tree_addr="${lfs_cv_graft_tree_addr:+0x}$lfs_cv_graft_tree_addr"
        lfs_cv_graft_tree_addr="${lfs_cv_graft_tree_addr:-no}"
    ])
    if test :${lfs_cv_graft_tree_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_GRAFT_TREE_ADDR], [$lfs_cv_graft_tree_addr],
        [The symbol graft_tree is not exported by most kernels.  Define this to
        the address of graft_tree in the kernel system map so that
        fattach/fdetach can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable do_umount address], [lfs_cv_do_umount_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_do_umount_addr=`($EGREP '\<do_umount' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_do_umount_addr="${lfs_cv_do_umount_addr:+0x}$lfs_cv_do_umount_addr"
        lfs_cv_do_umount_addr="${lfs_cv_do_umount_addr:-no}"
    ])
    if test :${lfs_cv_do_umount_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_DO_UMOUNT_ADDR], [$lfs_cv_do_umount_addr],
        [The symbol do_umount is not exported by most kernels.  Define this to
        the address of do_umount in the kernel system map so that
        fattach/fdetach can be properly supported.])
    fi
    AC_CACHE_CHECK([for ability to support fattach/fdetach], [lfs_cv_fattach], [dnl
        case "$lfs_cv_clone_mnt_addr:$lfs_cv_check_mnt_addr:$lfs_cv_graft_tree_addr:$lfs_cv_do_umount_addr" in
            no:*:*:* | *:no:*:* | *:*:no:* | *:*:*:no)
                lfs_cv_fattach=no
                ;;
            *)
                lfs_cv_fattach=yes
                ;;
        esac
    ])
    if test :${lfs_cv_fattach:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_KERNEL_FATTACH_SUPPORT], [1],
        [If the addresses for the necessary symbols above are defined, then
        define this to include fattach/fdetach support.])
    fi
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
AC_DEFUN([_LFS_CONFIG_LIS], [dnl
    AC_CACHE_CHECK([for usable sys_unlink address], [lfs_cv_sys_unlink_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_sys_unlink_addr=`($EGREP '\<sys_unlink' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_sys_unlink_addr="${lfs_cv_sys_unlink_addr:+0x}$lfs_cv_sys_unlink_addr"
        lfs_cv_sys_unlink_addr="${lfs_cv_sys_unlink_addr:-no}"
    ])
    if test :${lfs_cv_sys_unlink_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_SYS_UNLINK_ADDR], [$lfs_cv_sys_unlink_addr],
        [The symbol sys_unlink is not exported by most kernels.  Define this
        to the address of sys_unlink in the kernel system map so that LiS
        compatibility can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable sys_mknod address], [lfs_cv_sys_mknod_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_sys_mknod_addr=`($EGREP '\<sys_mknod' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_sys_mknod_addr="${lfs_cv_sys_mknod_addr:+0x}$lfs_cv_sys_mknod_addr"
        lfs_cv_sys_mknod_addr="${lfs_cv_sys_mknod_addr:-no}"
    ])
    if test :${lfs_cv_sys_mknod_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_SYS_MKNOD_ADDR], [$lfs_cv_sys_mknod_addr],
        [The symbol sys_mknod is not exported by most kernels.  Define this to
        the address of sys_mknod in the kernel system map so that LiS
        compatibility can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable sys_umount address], [lfs_cv_sys_umount_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_sys_umount_addr=`($EGREP '\<sys_umount' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_sys_umount_addr="${lfs_cv_sys_umount_addr:+0x}$lfs_cv_sys_umount_addr"
        lfs_cv_sys_umount_addr="${lfs_cv_sys_umount_addr:-no}"
    ])
    if test :${lfs_cv_sys_umount_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_SYS_UMOUNT_ADDR], [$lfs_cv_sys_umount_addr],
        [The symbol sys_umount is not exported by most kernels.  Define this
        to the address of sys_umount in the kernel system map so that LiS
        compatibility can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable sys_mount address], [lfs_cv_sys_mount_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_sys_mount_addr=`($EGREP '\<sys_mount' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_sys_mount_addr="${lfs_cv_sys_mount_addr:+0x}$lfs_cv_sys_mount_addr"
        lfs_cv_sys_mount_addr="${lfs_cv_sys_mount_addr:-no}"
    ])
    if test :${lfs_cv_sys_mount_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_SYS_MOUNT_ADDR], [$lfs_cv_sys_mount_addr],
        [The symbol sys_mount is not exported by most kernels.  Define this to
        the address of sys_mount in the kernel system map so that LiS
        compatibility can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable pcibios_init address], [lfs_cv_pcibios_init_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_pcibios_init_addr=`($EGREP '\<pcibios_init' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_pcibios_init_addr="${lfs_cv_pcibios_init_addr:+0x}$lfs_cv_pcibios_init_addr"
        lfs_cv_pcibios_init_addr="${lfs_cv_pcibios_init_addr:-no}"
    ])
    if test :${lfs_cv_pcibios_init_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_PCIBIOS_INIT_ADDR], [$lfs_cv_pcibios_init_addr],
        [The symbol pcibios_init is not exported by most kernels.  Define this
        to the address of pcibios_init in the kernel system map so that LiS
        compatibility can be properly supported.])
    fi
])# _LFS_CONFIG_LIS
# =========================================================================

# =========================================================================
_LFS_CONFIG_LFS
# -------------------------------------------------------------------------
# symbols to rip for Linux Fast STREAMS
# -------------------------------------------------------------------------
# file_move             <-- extern, declared in <linux/fs.h>
# open_softirq          <-- extern, declared in <linux/interrupt.h>
# sock_readv_writev     <-- extern, declared in <linux/net.h>
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_LFS], [dnl
    AC_CACHE_CHECK([for usable file_move address], [lfs_cv_file_move_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_file_move_addr=`($EGREP '\<file_move' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_file_move_addr="${lfs_cv_file_move_addr:+0x}$lfs_cv_file_move_addr"
        lfs_cv_file_move_addr="${lfs_cv_file_move_addr:-no}"
    ])
    if test :${lfs_cv_file_move_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_FILE_MOVE_ADDR], [$lfs_cv_file_move_addr],
        [The symbol file_move is not exported by most kernels.  Define this to
        the address of file_move in the kernel system map so that Linux
        Fast-STREAMS can be properly supported.])
    fi
    AC_CACHE_CHECK([for usable open_softirq address], [lfs_cv_open_softirq_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_open_softirq_addr=`($EGREP '\<open_softirq' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_open_softirq_addr="${lfs_cv_open_softirq_addr:+0x}$lfs_cv_open_softirq_addr"
        lfs_cv_open_softirq_addr="${lfs_cv_open_softirq_addr:-no}"
    ])
    if test :${lfs_cv_open_softirq_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_OPEN_SOFTIRQ_ADDR], [$lfs_cv_open_softirq_addr],
        [The symbol open_softirq is not exported by most kernels.  Define this to
        the address of open_softirq in the kernel system map so that Linux
        Fast-STREAMS can be properly supported.])
    else
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
    fi
    AC_CACHE_CHECK([for usable sock_readv_writev address], [lfs_cv_sock_readv_writev_addr], [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" ; then
            lfs_cv_sock_readv_writev_addr=`($EGREP '\<sock_readv_writev' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`
        fi
        lfs_cv_sock_readv_writev_addr="${lfs_cv_sock_readv_writev_addr:+0x}$lfs_cv_sock_readv_writev_addr"
        lfs_cv_sock_readv_writev_addr="${lfs_cv_sock_readv_writev_addr:-no}"
    ])
    if test :${lfs_cv_sock_readv_writev_addr:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_SOCK_READV_WRITEV_ADDR], [$lfs_cv_sock_readv_writev_addr],
        [The symbol sock_readv_writev is not exported by most kernels.  Define this to
        the address of sock_readv_writev in the kernel system map so that Linux
        Fast-STREAMS can be properly supported.])
    fi
])# _LFS_CONFIG_LFS
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
