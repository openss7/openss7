dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.42 2005/02/13 11:27:27 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (C) 2001-2005  OpenSS7 Corp. <http://www.openss7.com>
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
dnl Last Modified $Date: 2005/02/13 11:27:27 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/libraries.m4])
m4_include([m4/strconf.m4])
m4_include([m4/autotest.m4])

# =========================================================================
# AC_LFS
# -------------------------------------------------------------------------
AC_DEFUN([AC_LFS], [dnl
    _OPENSS7_PACKAGE([STREAMS], [Linux Fast-STREAMS])
    _LFS_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _LDCONFIG
    LFS_INCLUDES="-DLFS=1 -I- -imacros ./config.h -I./include -I${srcdir}/include"
    AC_SUBST([LFS_INCLUDES])dnl
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
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
    AM_CONDITIONAL(WITH_LFS, false)dnl
    AM_CONDITIONAL(WITH_LIS, false)dnl
    _AUTOTEST
])# AC_LFS
# =========================================================================

# =========================================================================
# _LFS_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_OPTIONS], [dnl
])# _LFS_OPTIONS
# =========================================================================

# =========================================================================
# _LFS_SETUP_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_DEBUG], [dnl
    case "$linux_cv_debug" in
        _DEBUG)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DEBUG], [], [Define to perform
                    internal structure tracking within the STREAMS executive
                    as well as to provide additional /proc filesystem files
                    for examining internal structures.])
            ;;
        _TEST)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TEST], [], [Define to perform
                    performance testing with debugging.  This mode does not
                    dump massive amounts of information into system logs, but
                    peforms all assertion checks.])
            ;;
        _SAFE)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAFE], [], [Define to perform
                    fundamental assertion checks.  This is a safer mode of
                    operation.])
            ;;
        _NONE | *)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NONE], [], [Define to perform
                    no assertion checks but report software errors.  This is
                    the smallest footprint, highest performance mode of
                    operation.])
            ;;
    esac
])# _LFS_SETUP_DEBUG
# =========================================================================

# =========================================================================
# _LFS_SETUP_MODULES
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_MODULES], [dnl
    AC_ARG_ENABLE([module-sth],
        AS_HELP_STRING([--enable-module-sth],
            [enable sth (stream head) module linked into streams object. @<:@default=module@:>@]),
            [enable_module_sth="$enableval"],
            [enable_module_sth='module'])
    AC_ARG_ENABLE([module-pipemod],
        AS_HELP_STRING([--enable-module-pipemod],
            [enable pipemod module linked into streams object. @<:@default=module@:>@]),
            [enable_module_pipemod="$enableval"],
            [enable_module_pipemod='module'])
    AC_ARG_ENABLE([module-connld],
        AS_HELP_STRING([--enable-module-connld],
            [enable connld module linked into streams object. @<:@default=module@:>@]),
            [enable_module_connld="$enableval"],
            [enable_module_connld='module'])
    AC_ARG_ENABLE([module-sc],
        AS_HELP_STRING([--enable-module-sc],
            [enable sc module linked into streams object. @<:@default=module@:>@]),
            [enable_module_sc="$enableval"],
            [enable_module_sc='module'])
    AC_CACHE_CHECK([for STREAMS module sth], [lfs_module_sth], [dnl
        lfs_module_sth="${enable_module_sth:-module}"
        if test :$lfs_module_sth = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_module_sth='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS module pipemod], [lfs_module_pipemod], [dnl
        lfs_module_pipemod="${enable_module_pipemod:-module}"
        if test :$lfs_module_pipemod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_module_pipemod='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS module connld],  [lfs_module_connld],  [dnl
        lfs_module_connld="${enable_module_connld:-module}"
        if test :$lfs_module_connld = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_module_connld='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS module sc],      [lfs_module_sc],      [dnl
        lfs_module_sc="${enable_module_sc:-module}"
        if test :$lfs_module_sc = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_module_sc='yes'
        fi])
    case ${lfs_module_sth:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_STH], [], [Define to link the sth module (stream
            head) with the streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_STH_MODULE], [], [Define to create the sth (stream
            head) module as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_module_pipemod:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD], [], [Define to link the pipemod module with
            the streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD_MODULE], [], [Define to create the pipemod
            module as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_module_connld:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD], [], [Define to link the connld module with
            the streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD_MODULE], [], [Define to create the connld
            module as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_module_sc:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC], [], [Define to link the sc module with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC_MODULE], [], [Define to create the sc module as a
            standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_STH],            [test :${lfs_module_sth:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_STH_MODULE],     [test :${lfs_module_sth:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD],        [test :${lfs_module_pipemod:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD_MODULE], [test :${lfs_module_pipemod:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD],         [test :${lfs_module_connld:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD_MODULE],  [test :${lfs_module_connld:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SC],             [test :${lfs_module_sc:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SC_MODULE],      [test :${lfs_module_sc:-module} = :module])
])# _LFS_SETUP_MODULES
# =========================================================================

# =========================================================================
# _LFS_SETUP_DRIVERS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_DRIVERS], [dnl
    AC_ARG_ENABLE([driver-clone],
        AS_HELP_STRING([--enable-driver-clone],
            [enable clone driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_clone="$enableval"],
            [enable_driver_clone='module'])
    AC_ARG_ENABLE([driver-fifo],
        AS_HELP_STRING([--enable-driver-fifo],
            [enable fifo driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_fifo="$enableval"],
            [enable_driver_fifo='module'])
    AC_ARG_ENABLE([driver-loop],
        AS_HELP_STRING([--enable-driver-loop],
            [enable loop driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_loop="$enableval"],
            [enable_driver_loop='module'])
    AC_ARG_ENABLE([driver-sad],
        AS_HELP_STRING([--enable-driver-sad],
            [enable sad driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_sad="$enableval"],
            [enable_driver_sad='module'])
    AC_ARG_ENABLE([driver-nsdev],
        AS_HELP_STRING([--enable-driver-nsdev],
            [enable nsdev driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_nsdev="$enableval"],
            [enable_driver_nsdev='module'])
    AC_ARG_ENABLE([driver-echo],
        AS_HELP_STRING([--enable-driver-echo],
            [enable echo driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_echo="$enableval"],
            [enable_driver_echo='module'])
    AC_ARG_ENABLE([driver-nuls],
        AS_HELP_STRING([--enable-driver-nuls],
            [enable nuls driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_nuls="$enableval"],
            [enable_driver_nuls='module'])
    AC_ARG_ENABLE([driver-pipe],
        AS_HELP_STRING([--enable-driver-pipe],
            [enable pipe driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_pipe="$enableval"],
            [enable_driver_pipe='module'])
    AC_ARG_ENABLE([driver-log],
        AS_HELP_STRING([--enable-driver-log],
            [enable log driver linked into streams object. @<:@default=module@:>@]),
            [enable_driver_log="$enableval"],
            [enable_driver_log='module'])
    AC_CACHE_CHECK([for STREAMS driver clone], [lfs_driver_clone], [dnl
        lfs_driver_clone="${enable_driver_clone:-module}"
        if test :$lfs_driver_clone = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_clone='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver fifo],  [lfs_driver_fifo],  [dnl
        lfs_driver_fifo="${enable_driver_fifo:-module}"
        if test :$lfs_driver_fifo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_fifo='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver loop],  [lfs_driver_loop],  [dnl
        lfs_driver_loop="${enable_driver_loop:-module}"
        if test :$lfs_driver_loop = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_loop='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver sad],   [lfs_driver_sad],   [dnl
        lfs_driver_sad="${enable_driver_sad:-module}"
        if test :$lfs_driver_sad = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_sad='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver nsdev], [lfs_driver_nsdev], [dnl
        lfs_driver_nsdev="${enable_driver_nsdev:-module}"
        if test :$lfs_driver_nsdev = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_nsdev='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver echo],  [lfs_driver_echo],  [dnl
        lfs_driver_echo="${enable_driver_echo:-module}"
        if test :$lfs_driver_echo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_echo='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver nuls],  [lfs_driver_nuls],  [dnl
        lfs_driver_nuls="${enable_driver_nuls:-module}"
        if test :$lfs_driver_nuls = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_nuls='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver pipe],  [lfs_driver_pipe],  [dnl
        lfs_driver_pipe="${enable_driver_pipe:-module}"
        if test :$lfs_driver_pipe = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_pipe='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS driver log],   [lfs_driver_log],   [dnl
        lfs_driver_log="${enable_driver_log:-module}"
        if test :$lfs_driver_log = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_driver_log='yes'
        fi])
    case ${lfs_driver_clone:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE], [], [Define to link the clone driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE_MODULE], [], [Define to create the clone driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_fifo:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO], [], [Define to link the fifo driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO_MODULE], [], [Define to create the fifo driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_loop:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP], [], [Define to link the loop driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP_MODULE], [], [Define to create the loop driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_sad:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD], [], [Define to link the sad driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD_MODULE], [], [Define to create the sad driver as
            a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_nsdev:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV], [], [Define to link the nsdev driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV_MODULE], [], [Define to create the nsdev driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_echo:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO], [], [Define to link the echo driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO_MODULE], [], [Define to create the echo driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_nuls:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS], [], [Define to link the nuls driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS_MODULE], [], [Define to create the nuls driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_pipe:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE], [], [Define to link the pipe driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE_MODULE], [], [Define to create the pipe driver
            as a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    case ${lfs_driver_log:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG], [], [Define to link the log driver with the
            streams.o object.  Leave undefined otherwise.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG_MODULE], [], [Define to create the log driver as
            a standalone loadable kernel module.  Leave undefined otherwise.])
            ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE],          [test :${lfs_driver_clone:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE_MODULE],   [test :${lfs_driver_clone:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO],           [test :${lfs_driver_fifo:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO_MODULE],    [test :${lfs_driver_fifo:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP],           [test :${lfs_driver_loop:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP_MODULE],    [test :${lfs_driver_loop:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD],            [test :${lfs_driver_sad:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD_MODULE],     [test :${lfs_driver_sad:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV],          [test :${lfs_driver_nsdev:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV_MODULE],   [test :${lfs_driver_nsdev:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO],           [test :${lfs_driver_echo:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO_MODULE],    [test :${lfs_driver_echo:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS],           [test :${lfs_driver_nuls:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS_MODULE],    [test :${lfs_driver_nuls:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE],           [test :${lfs_driver_pipe:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE_MODULE],    [test :${lfs_driver_pipe:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG],            [test :${lfs_driver_log:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG_MODULE],     [test :${lfs_driver_log:-module} = :module])
])# _LFS_SETUP_DRIVERS
# =========================================================================

# =========================================================================
# _LFS_SETUP_COMPAT
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_COMPAT], [dnl
    AC_ARG_ENABLE([compat-svr4],
        AS_HELP_STRING([--enable-compat-svr4],
            [enable source compatibility with SVR 4.2 MP variants.
            @<:@default=module@:>@]),
            [enable_compat_svr4="$enableval"],
            [enable_compat_svr4='module'])
    AC_ARG_ENABLE([compat-sol8],
        AS_HELP_STRING([--enable-compat-sol8],
            [enable source compatibility with Solaris 8 variants.
            @<:@default=module@:>@]),
        [enable_compat_sol8="$enableval"],
        [enable_compat_sol8='module'])
    AC_ARG_ENABLE([compat-uw7],
        AS_HELP_STRING([--enable-compat-uw7],
            [enable source compatibility with UnixWare 7 variants.
            @<:@default=module@:>@]),
        [enable_compat_uw7="$enableval"],
        [enable_compat_uw7='module'])
    AC_ARG_ENABLE([compat-osf],
        AS_HELP_STRING([--enable-compat-osf],
            [enable source compatibility with OSF/1.2 variants.
            @<:@default=module@:>@]),
        [enable_compat_osf="$enableval"],
        [enable_compat_osf='module'])
    AC_ARG_ENABLE([compat-aix],
        AS_HELP_STRING([--enable-compat-aix],
            [enable source compatibility with AIX 4 variants.
            @<:@default=module@:>@]),
        [enable_compat_aix="$enableval"],
        [enable_compat_aix='module'])
    AC_ARG_ENABLE([compat-hpux],
        AS_HELP_STRING([--enable-compat-hpux],
            [enable source compatibility with HPUX variants.
            @<:@default=module@:>@]),
        [enable_compat_hpux="$enableval"],
        [enable_compat_hpux='module'])
    AC_ARG_ENABLE([compat-lis],
        AS_HELP_STRING([--enable-compat-lis],
            [enable source compatibility with LiS variants.
            @<:@default=module@:>@]),
        [enable_compat_lis="$enableval"],
        [enable_compat_lis='module'])
    AC_CACHE_CHECK([for STREAMS UNIX(R) SVR 4.2 compatibility], [lfs_compat_svr4], [dnl
        lfs_compat_svr4="${enable_compat_svr4:-module}"
        if test :$lfs_compat_svr4 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_svr4='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS Solaris(R) 8 compatibility], [lfs_compat_sol8], [dnl
        case ${enable_compat_sol8:-module} in
            (yes) lfs_compat_svr4=yes ;;
            (module) if test :$lfs_compat_svr4 != :yes ; then lfs_compat_svr4=module ; fi ;;
        esac
        lfs_compat_sol8="${enable_compat_sol8:-module}"
        if test :$lfs_compat_sol8 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_sol8='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS UnixWare(R) 7 compatibility], [lfs_compat_uw7], [dnl
        case ${enable_compat_uw7:-module} in
            (yes) lfs_compat_svr4=yes ;;
            (module) if test :$lfs_compat_svr4 != :yes ; then lfs_compat_svr4=module ; fi ;;
        esac
        lfs_compat_uw7="${enable_compat_uw7:-module}"
        if test :$lfs_compat_uw7 = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_uw7='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS OSF/1.2 compatibility], [lfs_compat_osf], [dnl
        case ${enable_compat_osf:-module} in
            (yes) lfs_compat_svr4=yes ;;
            (module) if test :$lfs_compat_svr4 != :yes ; then lfs_compat_svr4=module ; fi ;;
        esac
        lfs_compat_osf="${enable_compat_osf:-module}"
        if test :$lfs_compat_osf = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_osf='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS AIX(R) 4 compatibility], [lfs_compat_aix], [dnl
        case ${enable_compat_aix:-module} in
            (yes) lfs_compat_svr4=yes ;;
            (module) if test :$lfs_compat_svr4 != :yes ; then lfs_compat_svr4=module ; fi ;;
        esac
        lfs_compat_aix="${enable_compat_aix:-module}"
        if test :$lfs_compat_aix = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_aix='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS HPUX(R) compatibility], [lfs_compat_hpux], [dnl
        case ${enable_compat_hpux:-module} in
            (yes) lfs_compat_svr4=yes ;;
            (module) if test :$lfs_compat_svr4 != :yes ; then lfs_compat_svr4=module ; fi ;;
        esac
        lfs_compat_hpux="${enable_compat_hpux:-module}"
        if test :$lfs_compat_hpux = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_hpux='yes'
        fi])
    AC_CACHE_CHECK([for STREAMS LiS compatibility], [lfs_compat_lis], [dnl
        lfs_compat_lis="${enable_compat_lis:-module}"
        if test :$lfs_compat_lis = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
            lfs_compat_lis='yes'
        fi])
    case ${lfs_compat_svr4:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4], [], [When defined, Linux Fast STREAMS
            will attempt to be as compatible as possible (without replicating any bugs) with the
            UNIX(R) SVR 4.2 MP docs so that STREAMS drivers and modules written to UNIX(R) SVR 4.2
            MP specs will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and
            modules written for UNIX(R) SVR 4.2 MP will require porting in more respects.  This
            symbol determines whether compatibility will be compiled and linkable with Linux
            Fast-STREAMS.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the UNIX(R) SVR 4.2 MP docs so that STREAMS drivers and modules written to UNIX(R) SVR
            4.2 MP specs will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and
            modules written for UNIX(R) SVR 4.2 MP will require porting in more respects.  This
            symbol determines whether compatibility will be compiled as a loadable module to Linux
            Fast-STREAMS.])
            ;;
    esac
    case ${lfs_compat_sol8:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the Solaris(R) 8 release so that STREAMS drivers and modules written for Solaris(R) 8
            will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
            written for Solaris(R) 8 will require porting in more respects.  This symbol determines
            whether compatibility will be compiled and linkable with Linux Fast-STREAMS.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the Solaris(R) 8 release so that STREAMS drivers and modules written for Solaris(R) 8
            will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
            written for Solaris(R) 8 will require porting in more respects.  This symbol determines
            whether compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
            ;;
    esac
    case ${lfs_compat_uw7:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the UnixWare(R) 7 release so that STREAMS drivers and modules written for UnixWare(R) 7
            will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
            written for UnixWare(R) 7 will require porting in more respects.  This symbol determines
            whether compatibility will be compiled and linkable with Linux Fast-STREAMS.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the UnixWare(R) 7 release so that STREAMS drivers and modules written for UnixWare(R) 7
            will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules
            written for UnixWare(R) 7 will require porting in more respects.  This symbol determines
            whether compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
            ;;
    esac
    case ${lfs_compat_osf:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the OSF(R)/1.2 release so that STREAMS drivers and modules written for OSF(R)/1.2 will
            compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written
            for OSF(R)/1.2 will require porting in more respects.  This symbol determines whether
            compatibility will be compiled and linkable with Linux Fast-STREAMS.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the OSF(R)/1.2 release so that STREAMS drivers and modules written for OSF(R)/1.2 will
            compile with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written
            for OSF(R)/1.2 will require porting in more respects.  This symbol determines whether
            compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
            ;;
    esac
    case ${lfs_compat_aix:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the AIX(R) 5L Version 5.1 release so that STREAMS drivers and modules written for AIX(R)
            5L Version 5.1 will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers
            and modules written for AIX(R) 5L Version 5.1 will require porting in more respects.
            This symbol determines whether compatibility will be compiled and linkable with Linux
            Fast-STREAMS.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the AIX(R) 5L Version 5.1 release so that STREAMS drivers and modules written for AIX(R)
            5L Version 5.1 will compile with Linux Fast STREAMS.  When undefined, STREAMS drivers
            and modules written for AIX(R) 5L Version 5.1 will require porting in more respects.
            This symbol determines whether compatibility will be compiled as a loadable module to
            Linux Fast-STREAMS.])
            ;;
    esac
    case ${lfs_compat_hpux:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the HPUX(R) release so that STREAMS drivers and modules written for HPUX(R) will compile
            with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
            HPUX(R) will require porting in more respects.  This symbol determines whether
            compatibility will be compiled and linkable with Linux Fast-STREAMS.  ])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the HPUX(R) release so that STREAMS drivers and modules written for HPUX(R) will compile
            with Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for
            HPUX(R) will require porting in more respects.  This symbol determines whether
            compatibility will be compiled as a loadable module to Linux Fast-STREAMS.])
            ;;
    esac
    case ${lfs_compat_lis:-module} in
        (yes)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LIS], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the LiS release so that STREAMS drivers and modules written for LiS will compile with
            Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for LiS will
            require porting in more respects.  This symbol determines whether compatibility will be
            compiled and linkable with Linux Fast-STREAMS.])
            ;;
        (module)
            AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_LIS_MODULE], [], [When defined, Linux Fast
            STREAMS will attempt to be as compatible as possible (without replicating any bugs) with
            the LiS release so that STREAMS drivers and modules written for LiS will compile with
            Linux Fast STREAMS.  When undefined, STREAMS drivers and modules written for LiS will
            require porting in more respects.  This symbol determines whether compatibility will be
            compiled as a loadable module to Linux Fast-STREAMS.])
            ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR4], [test :${lfs_compat_svr4:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR4_MODULE], [test :${lfs_compat_svr4:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SUN], [test :${lfs_compat_sol8:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SUN_MODULE], [test :${lfs_compat_sol8:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_UW7], [test :${lfs_compat_uw7:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_UW7_MODULE], [test :${lfs_compat_uw7:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OSF], [test :${lfs_compat_osf:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OSF_MODULE], [test :${lfs_compat_osf:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_AIX], [test :${lfs_compat_aix:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_AIX_MODULE], [test :${lfs_compat_aix:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_HPUX], [test :${lfs_compat_hpux:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [test :${lfs_compat_hpux:-module} = :module])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_LIS], [test :${lfs_compat_lis:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_LIS_MODULE], [test :${lfs_compat_lis:-module} = :module])
])# _LFS_SETUP_COMPAT
# =========================================================================

# =========================================================================
# _LFS_SETUP_FIFOS
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_FIFOS], [dnl
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
AC_DEFUN([_LFS_SETUP], [dnl
    _LINUX_KERNEL
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LFS_CHECK_KERNEL
    _LFS_SETUP_DEBUG
    _LFS_SETUP_MODULE
    _LFS_SETUP_MODULES
    _LFS_SETUP_DRIVERS
    _LFS_SETUP_COMPAT
    _LFS_SETUP_FIFOS
])# _LFS_SETUP
# =========================================================================

# =========================================================================
# _LFS_SETUP_MODULE
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_MODULE], [dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MODULE], [], [When defined, STREAMS
            is being compiled as a loadable kernel module.])
    else
        AC_DEFINE_UNQUOTED([CONFIG_STREAMS], [], [When defined, STREAMS is
            being compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([CONFIG_STREAMS_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([CONFIG_STREAMS], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _LFS_SETUP_MODULE
# =========================================================================

# =========================================================================
# _LFS_CHECK_KERNEL
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_KERNEL], [dnl
    _LFS_CONFIG_FATTACH
    _LFS_CONFIG_LIS
    _LFS_CONFIG_LFS
])# _LFS_CHECK_KERNEL
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
AC_DEFUN([_LFS_CONFIG_FATTACH], [dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([mount_sem], [dnl
        AC_DEFINE_UNQUOTED([HAVE_TASK_NAMESPACE_SEM], [1],
        [Some recent 2.4 RH kernel place the mount semaphore into the task
        structure rather than using the static global mount_sem semaphore.
        Define this if you have a modified kernel.]) ])
    lfs_pipe=yes
    lfs_fattach=yes
    _LINUX_KERNEL_SYMBOL_EXPORT([clone_mnt], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([check_mnt], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([graft_tree], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([do_umount], [lfs_fattach=no; lfs_pipe=no])
    AC_CACHE_CHECK([for kernel symbol support for fattach/fdetach], [lfs_cv_fattach], [dnl
        lfs_cv_fattach="$lfs_fattach" ])
    if test :"${lfs_cv_fattach:-no}" != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_KERNEL_FATTACH_SUPPORT], [1],
        [If the addresses for the necessary symbols above are defined, then
        define this to include fattach/fdetach support.])
    fi
    AC_CACHE_CHECK([for kernel symbol support for pipe], [lfs_cv_pipe], [dnl
        lfs_cv_pipe="$lfs_pipe" ])
    if test :${lfs_cv_pipe:-no} != :no ; then
        AC_DEFINE_UNQUOTED([HAVE_KERNEL_PIPE_SUPPORT], [1],
        [If the addresses for the necessary symbols above are defined, then
        define this to include pipe support.])
    fi
    _LINUX_KERNEL_SYMBOL_EXPORT([def_fifo_fops])
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
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_unlink], [dnl
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_unlink() will always return ENOSYS])
        fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_mknod], [dnl
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_mknod() will always return ENOSYS])
        fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_umount], [dnl
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_umount() will always return ENOSYS])
        fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_mount], [dnl
        if test :"${linux_cv_k_marchdir}" = :parisc ; then
            AC_MSG_WARN([lis_mount() will always return ENOSYS])
        fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([pcibios_init])
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
AC_DEFUN([_LFS_CONFIG_LFS], [dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([file_move])
    _LINUX_KERNEL_SYMBOL_EXPORT([open_softirq], [dnl
        AC_MSG_ERROR([
*** 
*** Compiling Linux Fast STREAMS requires the availability of the kernel
*** symbol open_softirq so that the necessary software IRQs can be
*** established.  This symbol is not normally exported from the kernel.
*** I have looked in "${linux_cv_k_sysmap:-(no sysmap)}" for the symbol
*** and cannot find it.  This problem needs to be corrected or workaround
*** found before you can compile Linux Fast STREAMS for the system for
*** which it is being configured.
*** ]) ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sock_readv_writev])
    _LINUX_KERNEL_SYMBOL_EXPORT([__wake_up_sync])
])# _LFS_CONFIG_LFS
# =========================================================================

# =========================================================================
# _LFS_STRCONF
# -------------------------------------------------------------------------
AC_DEFUN([_LFS_STRCONF], [dnl
    strconf_cv_stem='Config'
dnl strconf_cv_input='Config.master'
dnl strconf_cv_majbase=230
    strconf_cv_config='include/sys/config.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="src/util/${PACKAGE_TARNAME}_mknod.c"
dnl strconf_cv_stsetup='strsetup.conf'
dnl strconf_cv_strload='strload.conf'
    strconf_cv_package='LfS'
    _STRCONF
])# _LFS_STRCONF
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
