dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.5 2004/03/01 11:32:21 brian Exp $
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
dnl Last Modified $Date: 2004/03/01 11:32:21 $ by $Author: brian $
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
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the UNIX(R) SVR
                4.2 MP docs so that STREAMS drivers and modules written to
                UNIX(R) SVR 4.2 MP specs will compile with Linux Fast STREAMS.
                When undefined, STREAMS drivers and modules writter for
                UNIX(R) SVR 4.2 MP will require porting in more respects.])
    fi
    if test :"$lfs_cv_sol8" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the Solaris(R) 8
                release so that STREAMS drivers and modules written for
                Solaris(R) 8 will compile with Linux Fast STREAMS.  When
                undefined, STREAMS drivers and modules writter for Solaris(R)
                8 will require porting in more respects.])
    fi
    if test :"$lfs_cv_uw7" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the UnixWare(R) 7
                release so that STREAMS drivers and modules written for
                UnixWare(R) 7 will compile with Linux Fast STREAMS.  When
                undefined, STREAMS drivers and modules writter for UnixWare(R)
                7 will require porting in more respects.])
    fi
    if test :"$lfs_cv_osf" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the OSF(R)/1.2
                release so that STREAMS drivers and modules written for
                OSF(R)/1.2 will compile with Linux Fast STREAMS.  When
                undefined, STREAMS drivers and modules writter for OSF(R)/1.2
                will require porting in more respects.])
    fi
    if test :"$lfs_cv_aix" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the AIX(R) 5L
                Version 5.1 release so that STREAMS drivers and modules
                written for AIX(R) 5L Version 5.1 will compile with Linux Fast
                STREAMS.  When undefined, STREAMS drivers and modules writter
                for AIX(R) 5L Version 5.1 will require porting in more
                respects.])
    fi
    if test :"$lfs_cv_hpux" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the HPUX(R)
                release so that STREAMS drivers and modules written for
                HPUX(R) will compile with Linux Fast STREAMS.  When undefined,
                STREAMS drivers and modules writter for HPUX(R) will require
                porting in more respects.])
    fi
    if test :"$lfs_cv_lis" = :yes ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LIS], [], [When defined,
                Linux Fast STREAMS will attempt to be as compatible as
                possible (without replicating any bugs) with the LiS release
                so that STREAMS drivers and modules written for LiS will
                compile with Linux Fast STREAMS.  When undefined, STREAMS
                drivers and modules writter for LiS will require porting in
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
    # all our config substitutions
    AC_SUBST(LFS_SC_CONFIG)
    AC_SUBST(LFS_SC_CONFMOD)
    AC_SUBST(LFS_SC_MAKEDEV)
    AC_SUBST(LFS_SC_DRVCONF)
    AC_SUBST(LFS_SC_MAJBASE)
    AC_SUBST(LFS_SC_MKNODES)
    AC_SUBST(LFS_SC_MODCONF)
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
        if test -r "$lfs_config"
        then
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
    if test -n "$lfs_configs"
    then
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
])# _LFS_OUTPUT_CONFIG
# =============================================================================

# =========================================================================
# _LFS_CHECK_KERNEL
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_KERNEL], [
    # ---------------
    # symbols to check:
    # -----------------
    # register_protosw
    # unregister_protosw
    # do_fattach
    # do_fdetach
    # ---------------
    # symbols to rip:
    # ---------------
    # pcibios_init          <-- extern, can be ripped, declared in <linux/pci.h>
    # sys_unlink            <-- extern, can be ripped, not declared
    # sys_mknod             <-- extern, can be ripped, not declared
    # sys_umount            <-- extern, can be ripped, not declared
    # sys_mount             <-- extern, can be ripped, not declared
    # file_move             <-- extern, can be ripped, declared in <linux/fs.h>
    # open_softirq          <-- extern, can be ripped, declared in <linux/interrupt.h>
    # sock_readv_writev     <-- extern, can be ripped, declared in <linux/net.h>
    #
    #   register_protosw
    #   unregister_protosw
    #   sock_f_ops
    #   sock_p_ops
    #   socksys_create
    # 
    # ---------------
    # symbols to implement fattach
    # ---------------
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
    # ---------------
    # symbols to implement fdetach
    # ---------------
    # path_init     <--- exported
    # path_release  <--- exported
    # check_mnt     <=== static, can be ripped

])# _LFS_CHECK_KERNEL
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
