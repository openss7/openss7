# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
# 
# @(#) $RCSfile: kernel.m4,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/02/12 02:36:30 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
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
# Last Modified $Date: 2004/02/12 02:36:30 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# AC_LINUX_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN(AC_LINUX_KERNEL,
[
    _LINUX_KERNEL_OPTIONS
    AC_REQUIRE([AC_CANONICAL_TARGET])
    # move user flags out of the way temporarily
    linux_tmp_cppflags="${CPPFLAGS}"
    linux_tmp_cflags="${CFLAGS}"
    CPPFLAGS=
    CFLAGS=
    _LINUX_KERNEL_SETUP
    KERNEL_CPPFLAGS="${CPPFLAGS}"
    KERNEL_CFLAGS="${CFLAGS}"
    AC_SUBST(KERNEL_CPPFLAGS)
    AC_SUBST(KERNEL_CFLAGS)
    # move user flags back where they were
    CPPFLAGS="${linux_tmp_cppflags}"
    CFLAGS="${linux_tmp_cflags}"
])# AC_LINUX_KERNEL
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN(_LINUX_KERNEL_OPTIONS,
[
    AC_ARG_WITH([k-release],
                AC_HELP_STRING([--with-k-release=UTSRELEASE],
                               [specify the UTS release of the linux kernel for
                                which the build is targetted.  If this option is
                                not set, the build will be targetted at the
                                kernel running in the build environment.
                                @<:@default=`uname -r`@:>@]),
                [with_k_release=$withval],
                [with_k_release=''])
    AC_ARG_WITH([k-modules],
                AC_HELP_STRING([--with-k-modules=DIR],
                               [specify the directory to which kernel modules
                                will be installed.
                                @<:@default=/lib/modules/K-RELEASE/misc@:>@]),
                [with_k_modules=$withval],
                [with_k_modules=''])
    AC_ARG_WITH([k-build],
                AC_HELP_STRING([--with-k-build=DIR],
                               [specify the base kernel build directory in which
                                configured kernel source resides.
                                @<:@default=K-MODULES-DIR/build@:>@]),
                [with_k_build=$withval],
                [with_k_build=''])
    AC_ARG_WITH([k-includes],
                AC_HELP_STRING([--with-k-includes=DIR],
                               [specify the include directory of the kernel for
                                which the build is targetted.
                                @<:@default=K-BUILD-DIR/include@:>@]),
                [with_k_includes=$withval],
                [with_k_includes=''])
    AC_ARG_WITH([k-archdir],
                AC_HELP_STRING([--with-k-archdir=DIR],
                               [specify the kernel source architecture specific
                                directory.  @<:@default=K-BUILD-DIR/arch@:>@]),
                [with_k_archdir=$withval],
                [with_k_archdir=''])
    AC_ARG_WITH([k-machdir],
                AC_HELP_STRING([--with-k-machdir=DIR],
                               [specify the kernel source machine specific
                                directory.  @<:@default=K-ARCHDIR/ARCH@:>@]),
                [with_k_machdir=$withval],
                [with_k_machdir=''])
    AC_ARG_WITH([k-sysmap],
                AC_HELP_STRING([--with-k-sysmap=MAP],
                               [specify the kernel system map file.
                                @<:@default=K-BUILD-DIR/System.map@:>@]),
                [with_k_sysmap=$withval],
                [with_k_sysmap=''])
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
                                 @<:@default=yes@:>@]),
                  [enable_k_safe=$enableval],
                  [enable_k_safe=''])
    AC_ARG_ENABLE([k-install],
                  AC_HELP_STRING([--enable-k-install],
                                 [specify whether kernel modules will be
                                 installed.  @<:@default=yes@:>@]),
                  [enable_k_install=$enableval],
                  [enable_k_install=''])
])# _LINUX_KERNEL_OPTIONS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN(_LINUX_KERNEL_SETUP,
[
    _LINUX_CHECK_KERNEL_RELEASE
    _LINUX_CHECK_KERNEL_MODULES
    _LINUX_CHECK_KERNEL_BUILD
    _LINUX_CHECK_KERNEL_SYSMAP
    _LINUX_CHECK_KERNEL_HEADERS
    _LINUX_CHECK_KERNEL_MARCH
    _LINUX_CHECK_KERNEL_RHBOOT
    _LINUX_CHECK_KERNEL_ARCHDIR
    _LINUX_CHECK_KERNEL_MACHDIR
    _LINUX_SETUP_KERNEL_CFLAGS
    _LINUX_CHECK_KERNEL_VERSIONS
    PACKAGE_KNUMBER="${linux_cv_k_major}.${linux_cv_k_minor}.${linux_cv_k_patch}"
    AC_SUBST(PACKAGE_KNUMBER)
    AC_DEFINE_UNQUOTED([PACKAGE_KNUMBER], ["$PACKAGE_KNUMBER"], [The Linux
        kernel version without EXTRAVERSION.])
    PACKAGE_KVERSION="$linux_cv_k_release"
    AC_SUBST(PACKAGE_KVERSION)
    AC_DEFINE_UNQUOTED([PACKAGE_KVERSION], ["$PACKAGE_KVERSION"], [The Linux
        kernel version for which the package was configured.])
])# _LINUX_KERNEL_SETUP
# =============================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_RELEASE,
[
    AC_CACHE_CHECK([for kernel release], [linux_cv_k_release], [dnl
        if test :"${with_k_release:-no}" = :no
        then
            if test :"${cross_compilling:-no}" != :yes
            then
                linux_cv_k_release="`uname -r`"
            else
                linux_cv_k_release=NONE
            fi
        else
            linux_cv_k_release="$with_k_release"
        fi
    ])
    if test :"$linux_cv_k_release" = :NONE
    then
        AC_MSG_ERROR([
*** 
*** You have not specified --with-k-releease indicating that the build is for
*** the running kernel, however, you are cross-compiling.  When
*** cross-compiling, you must specify the kernel release that you are
*** targetting using the --with-k-release option.  This option should specify
*** the entire UTS_RELEASE of the kernel, including the EXTRAVERSION such as
*** that associated with RedHat kernel releases.
*** 
        ])
    fi
    # pull out versions from release number
    AC_CACHE_CHECK([for kernel major release number], [linux_cv_k_major], [dnl
        linux_cv_k_major="`echo $linux_cv_k_release | sed -e 's|[[^0-9]].*||'`"
    ])
    if test ${linux_cv_k_major:-0} -ne 2; then
        AC_MSG_ERROR([
*** 
*** Kernel major release number is "$linux_cv_k_major".  That cannot be correct, unless
*** this is really old software now and Linux has already made it to release
*** 3.  In which case, give up.  More likely, "$linux_cv_k_release" is not the
*** UTS_RELEASE of the target.  Check the UTS_RELEASE of the target and
*** specify with with the configure option --with-k-release or look at the
*** result of uname -r ("`uname -r`") to determine the problem.
*** 
        ])
    fi
    AC_CACHE_CHECK([for kernel minor release number], [linux_cv_k_minor], [dnl
        linux_cv_k_minor="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.||;s|[[^0-9]].*||'`"
    ])
    if test ${linux_cv_k_minor:-0} -ne 4; then
        AC_MSG_ERROR([
*** 
*** Kernel minor release number "$linux_cv_k_minor" is either too old or too new, or
*** the UTS_RELEASE name "$linux_cv_k_release" is mangled.  Try specifiying a
*** 2.4 kernel with the --with-k-release option to configure.  If you are
*** trying to compile for a 2.2, 2.3, 2.5 or 2.6 kernel, give up.  Only 2.4
*** kernels are supported at the current time.
*** 
        ])
    fi
    AC_CACHE_CHECK([for kernel patch release number], [linux_cv_k_patch], [dnl
        linux_cv_k_patch="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.[[0-9]]*\.||;s|[[^0-9]].*||'`"
    ])
    if test ${linux_cv_k_patch:-0} -le 10; then
        AC_MSG_ERROR([
*** 
*** Kernel patch release number "$linux_cv_k_patch" is too old.  Try
*** configuring for a kernel later than 2.4.10.
*** 
        ])
    fi
    AC_CACHE_CHECK([for kernel extra release number], [linux_cv_k_extra], [dnl
        linux_cv_k_extra="`echo $linux_cv_k_release | sed -e 's|[[^-]]*-||'`"
    ])
    kversion="${linux_cv_k_release}"
    AC_SUBST(kversion)
])# _LINUX_CHECK_KERNEL_RELEASE
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODULES
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_MODULES,
[
    AC_CACHE_CHECK([for kernel modules directory], [linux_cv_k_modules], [dnl
        if test :"$prefix" = :NONE
        then
            linux_cv_module_prefix="${ac_default_prefix}"
        else
            linux_cv_module_prefix="${prefix}"
        fi
        dnl strip trailing [/usr][/local]
        linux_cv_module_prefix=`echo $linux_cv_module_prefix | sed -e 's|/local$||;s|/usr$||'`
        if test :"${with_k_modules:-no}" = :no
        then
            linux_cv_k_modules="${linux_cv_module_prefix}/lib/modules/${linux_cv_k_release}"
        else
            linux_cv_k_modules="$with_k_modules"
        fi
    ])
    if test ! -d "$linux_cv_k_modules"
    then
        if test -z "$with_k_modules" -o :"$with_k_modules" = :no
        then
            if test :"${with_k_release:-no}" = :no
            then
                AC_MSG_WARN([
*** 
*** You have not specified --with-k-release, so the build is for the running
*** kernel version
***     "`uname -r`"
*** and you have not specified the modules directory with ---with-k-modules.
*** However, the default modules directory for the running kernel:
***     "$linux_cv_k_modules"
*** does not exist in the build environment.  This should not happen except in
*** special cross-compiling situations.  Check that the kernel on the build
*** system is configured and installed correctly before trying again.  If the
*** modules directory is located in a different directory than normal, use the
*** --with-k-modules option to configure to correctly specify the kernel
*** modules directory.  If you do not want to build for the running kernel,
*** you need to specify --with-k-release even if it does not differ from the
*** build machine kernel version.
*** 
                ])
            fi
        else
            if test :"${with_k_release:-no}" = :no
            then
                if test -d "/lib/modules/${linux_cv_k_release}"
                then
                    linux_tmp='does exist'
                else
                    linux_tmp='also does not exist'
                fi
                AC_MSG_WARN([
*** 
*** You have not specified --with-k-release, so the build is for the running
*** kernel version
***     "`uname -r`"
*** and you have specified the modules directory with --with-k-modules.
*** However, the specified kernel modules directory:
***     "$with_k_modules"
*** does not exist in the build environment.  Either specify the correct
*** modules directory using --with-k-modules, or remove the flag to permit the
*** directory to default to:
***     "/lib/modules/${linux_cv_k_release}"
*** which $linux_tmp in the build environment.
*** If you do not want to build for the running kernel, you need to specify
*** --with-k-release even if it does not differ from the build machine kernel
*** version.
*** 
                ])
            fi
        fi
    fi
    krootdir="${linux_cv_module_prefix}"
    kmoduledir="${linux_cv_k_modules}"
    AC_SUBST(krootdir)
    AC_SUBST(kmoduledir)
])# _LINUX_CHECK_KERNEL_MODULES
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_BUILD
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_BUILD,
[
    AC_CACHE_CHECK([for kernel build directory], [linux_cv_k_build], [dnl
        if test :"${cross_compiling:-no}" = :no -a :"${with_k_release:-no}" = :no
        then
            eval "k_build_search_path=\"
                ${with_k_build}
                /lib/modules/${linux_cv_k_release}/build
                ${linux_cv_k_modules:+$linux_cv_k_modules/build}
                /usr/src/linux-${linux_cv_k_release}\""
        else
            eval "k_build_search_path=\"
                ${with_k_build}
                ${linux_cv_k_modules:+$linux_cv_k_modules/build}
                ${linux_cv_module_prefix}/lib/modules/${linux_cv_k_release}/build
                ${linux_cv_module_prefix}/usr/src/linux-${linux_cv_k_release}
                ${linux_cv_module_prefix}/src/linux-${linux_cv_k_release}
                ${libdir}/modules/${linux_cv_k_release}/build
                ${exec_prefix}/lib/modules/${linux_cv_k_release}/build\""
        fi
        k_build_search_path=`echo "$k_build_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        linux_cv_k_build=
        for linux_dir in $k_build_search_path
        do
            if test -d $linux_dir -a -r $linux_dir/include/linux/version.h
            then
                linux_cv_k_build="$linux_dir"
                break
            fi
        done
    ])
    if test :"${linux_cv_k_build:-no}" = :no
    then
        if test :"${with_k_release:-no}" = :no
        then
            AC_MSG_ERROR([
*** 
*** This package does not support headless kernel builds.  This is because the
*** only linux header files that you can have installed that would permit a
*** build without kernel source files sould be a pre-2.4.10 kernel header
*** package use to build libc or glibc2 that could not possibly be compatible
*** with the kernel structure in other kernels.  Install the appropriate
*** kernel source package in the build environment and then configure again.
*** 
            ])
        else
            AC_MSG_ERROR([
*** 
*** Cannot find viable kernel source directory yet not configured for
*** sourceless build on running kernel.  The following directories doe not
*** exist in the build environment:
***     "${linux_cv_k_modules}/build"
***     "/usr/src/${linux_cv_k_release}"
*** Check the setting of the following options before repeating:
***     --with-k-release "${with_k_release:-no}"
***     --with-k-modules "${with_k_modules:-no}"
***     --with-k-build   "${with_k_build:-no}"
*** 
            ])
        fi
    fi
])# _LINUX_CHECK_KERNEL_BUILD
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SYSMAP
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_SYSMAP,
[
    AC_CACHE_CHECK([for kernel system map], [linux_cv_k_sysmap], [dnl
        if test :"${cross_compiling:-no}" = :no -a :"${with_k_release:-no}" = :no
        then
            eval "k_sysmap_search_path=\"
                ${with_k_sysmap}
                ${linux_cv_k_build}/System.map-${linux_cv_k_release}
                /boot/System.map-${linux_cv_k_release}
                ${linux_cv_k_build}/System.map
                /boot/System.map
                /proc/ksyms\""
        else
            eval "k_sysmap_search_path=\"
                ${with_k_sysmap}
                ${linux_cv_k_build}/System.map-${linux_cv_k_release}
                ${linux_cv_module_prefix}/boot/System.map-${linux_cv_k_release}
                ${exec_prex}/boot/System.map-${linux_cv_k_release}
                ${linux_cv_k_build}/System.map
                ${linux_cv_module_prefix}/boot/System.map
                ${exec_prex}/boot/System.map\""
        fi
        k_sysmap_search_path=`echo "$k_sysmap_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        linux_cv_k_sysmap=
        for linux_file in $k_sysmap_search_path
        do
            if test -r $linux_file ; then
                linux_cv_k_sysmap="$linux_file"
                break
            fi
        done
    ])
])# _LINUX_CHECK_KERNEL_SYSMAP
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_HEADERS,
[
    AC_CACHE_CHECK([for kernel headers], [linux_cv_k_includes], [dnl
        if test :"${with_k_includes:-no}" = :no
        then
            linux_cv_k_includes="${linux_cv_k_build}/include"
        else
            linux_cv_k_includes="${with_k_includes}"
        fi
    ])
    if test ! -d "$linux_cv_k_includes"
    then
        AC_MSG_ERROR([
*** 
*** Kernel headers directory:
***     "$linux_cv_k_includes"
*** does not exit.  Specify a correct kernel header directory using the
*** --with-k-includes option to configure before attempting again.
*** 
        ])
    fi
])# _LINUX_CHECK_KERNEL_HEADERS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MARCH
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_MARCH,
[
    AC_CACHE_CHECK([for kernel machine], [linux_cv_march], [dnl
        linux_cv_march="$target_cpu"
        case "$target_cpu" in
            i586)
                case "${target_alias:-${host_alias:-$build_alias}}" in
                    k6*) linux_cv_march=k6 ;;
                    *) linux_cv_march=i586 ;;
                esac
                ;;
            i686)
                case "${target_alias:-${host_alias:-$build_alias}}" in
                    athlon*) linux_cv_march=athlon ;;
                    *) linux_cv_march=i686 ;;
                esac
                ;;
            i?86) linux_cv_march=i386 ;;
        esac
    ])
    # fix up the target spec
    target_cpu="$linux_cv_march"
    target="${target_cpu:-unknown}-${target_vendor:-unknown}-${target_os:-none}"
])# _LINUX_CHECK_KERNEL_MARCH
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_RHBOOT
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_RHBOOT,
[
    AC_CACHE_CHECK([for kernel Red Hat boot], [linux_cv_rh_boot_kernel], [dnl
        linux_cv_rh_boot_kernel=no
        if test -r "${linux_cv_k_includes}/linux/rhconfig.h"
        then
            case "$linux_cv_k_release" in
                *BOOT)
                    linux_cv_rh_boot_kernel=BOOT
                    ;;
                *smp)
                    linux_cv_rh_boot_kernel=smp
                    ;;
                *bigmem)
                    linux_cv_rh_boot_kernel=bigmem
                    ;;
                *debug)
                    linux_cv_rh_boot_kernel=debug
                    ;;
                *)
                    linux_cv_rh_boot_kernel=UP
                    ;;
            esac
        fi
    ])
    if test :"${linux_cv_rh_boot_kernel:-no}" != :no
    then
        AC_DEFINE_UNQUOTED([__BOOT_KERNEL_H], [], [Define for Red Hat kernel.])
        case "${linux_cv_rh_boot_kernel:-no}" in
            BOOT)
                AC_DEFINE([__BOOT_KERNEL_BOOT], [1], [Define for Red Hat BOOT kernel.])
                ;;
            smp)
                AC_DEFINE([__BOOT_KERNEL_SMP], [1], [Define for Red Hat SMP kernel.])
                ;;
            bigmem)
                AC_DEFINE([__BOOT_KERNEL_BIGMEM], [1], [Define for Red Hat BIGMEM kernel.])
                ;;
            debug)
                AC_DEFINE([__BOOT_KERNEL_DEBUG], [1], [Define for Red Hat DEBUG kernel.])
                ;;
            UP)
                AC_DEFINE([__BOOT_KERNEL_UP], [1], [Define for Red Hat UP kernel.])
                ;;
        esac
        case "$linux_cv_march" in
            i586)
                AC_DEFINE([__MODULE_KERNEL_i586], [1], [Define for i586 Red Hat kernel.])
                ;;
            i686)
                AC_DEFINE([__MODULE_KERNEL_i686], [1], [Define for i686 Red Hat kernel.])
                ;;
            i?86)
                AC_DEFINE([__MODULE_KERNEL_i386], [1], [Define for i386 Red Hat kernel.])
                ;;
            athlon)
                AC_DEFINE([__MODULE_KERNEL_athlon], [1], [Define for athlon Red Hat kernel.])
                ;;
        esac
    fi
])# _LINUX_CHECK_KERNEL_RHBOOT
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_ARCHDIR
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_ARCHDIR,
[
    AC_CACHE_CHECK([for kernel arch directory], [linux_cv_k_archdir], [dnl
        if test :"${with_k_archdir:-no}" = :no
        then
            linux_cv_k_archdir="${linux_cv_k_build}/arch"
        else
            linux_cv_k_archdir="$with_k_archdir"
        fi
    ])
    if test ! -d "$linux_cv_k_archdir"
    then
        AC_MSG_WARN([
*** 
*** "$linux_cv_k_archdir" does not exist.
*** 
        ])
    fi
])# _LINUX_CHECK_KERNEL_ARCHDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MACHDIR
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_MACHDIR,
[
    AC_CACHE_CHECK([for kernel mach directory], [linux_cv_k_machdir], [dnl
        if test :"${with_k_machdir:-no}" = :no
        then
            case "$linux_cv_march" in
                alpha*)                 linux_cv_k_machdir="alpha"      ;;
                arm*)                   linux_cv_k_machdir="arm"        ;;
                cris*)                  linux_cv_k_machdir="cris"       ;;
                i?86* | k6* | athlon*)  linux_cv_k_machdir="i386"       ;;
                ia64)                   linux_cv_k_machdir="ia64"       ;;
                m68*)                   linux_cv_k_machdir="m68k"       ;;
                mips64*)                linux_cv_k_machdir="mips64"     ;;
                mips*)                  linux_cv_k_machdir="mips"       ;;
                hppa*)                  linux_cv_k_machdir="parisc"     ;;
                ppc* | powerpc*)        linux_cv_k_machdir="ppc"        ;;
                s390x*)                 linux_cv_k_machdir="s390x"      ;;
                s390*)                  linux_cv_k_machdir="s390"       ;;
                sh*)                    linux_cv_k_machdir="sh"         ;;
                sparc64*)               linux_cv_k_machdir="sparc64"    ;;
                sparc*)                 linux_cv_k_machdir="sparc"      ;;
                *)                      linux_cv_k_machdir="$linux_cv_march" ;;
            esac
            linux_cv_k_machdir="${linux_cv_k_build}/arch/${linux_cv_k_machdir}"
        else
            linux_cv_k_machdir="$with_k_machdir"
        fi
    ])
    if test ! -d "$linux_cv_k_machdir"
    then
        AC_MSG_WARN([
*** 
*** "$linux_cv_k_machdir" does not exist.
*** 
        ])
    fi
])# _LINUX_CHECK_KERNEL_MACHDIR
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_CFLAGS
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_SETUP_KERNEL_CFLAGS,
[
    #
    # FIXME: we need to check what is in CFLAGS before we go adding stuff to
    # CFLAGS, that way the user can override these settings for better tuning.
    #
    # CFLAGS="-Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common"
    CFLAGS="-Wall -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common"
    linux_k_defs='-D__KERNEL__ -DLINUX'
    case "$linux_cv_march" in
        alpha*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -mno-fp-regs -ffixed=8"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=`echo $linux_cv_march | sed -e 's|^alpha||'` -Wa,-mev6"
            ;;
        arm*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-fno-common -pipe -fomit-frame-pointer"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-march=$linux_cv_march"
            AC_MSG_WARN([*** you need to set -mapcs and -mtune yourself in CFLAGS=])
            ;;
        cris*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-mlinux -pipe"
            CFLAGS=`echo "$CFLAGS" | sed -e'|-fomit-frame-pointer||'`
            ;;
        i?86*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe"
            if (`eval $CC $CFLAGS -mpreferred-stack-boundary=2 -S -o /dev/null -xc /dev/null`) >/dev/null 2>&1;
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-mpreferred-stack-boundary=2"
            fi
            CFLAGS="${CFLAGS}${CFLAGS:+ }-march=$linux_cv_march"
            ;;
        k6*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe"
            if (`eval $CC $CFLAGS -mpreferred-stack-boundary=2 -S -o /dev/null -xc /dev/null`) >/dev/null 2>&1;
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-mpreferred-stack-boundary=2"
            fi
            if (`eval $CC $CFLAGS -march=$linux_cv_march -S -o /dev/null -xc /dev/null`) >/dev/null 2>&1;
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-march=$linux_cv_march"
            else
                CFLAGS="${CFLAGS}${CFLAGS:+ }-march=i586"
            fi
            ;;
        athlon*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe"
            if (`eval $CC $CFLAGS -mpreferred-stack-boundary=2 -S -o /dev/null -xc /dev/null`) >/dev/null 2>&1;
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-mpreferred-stack-boundary=2"
            fi
            if (`eval $CC $CFLAGS -march=$linux_cv_march -S -o /dev/null -xc /dev/null`) >/dev/null 2>&1;
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-march=$linux_cv_march"
            else
                CFLAGS="${CFLAGS}${CFLAGS:+ }-march=i686 -malign-functions=4"
            fi
            ;;
        ia64)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -ffixed-r13 -mfixed-range=f10-f15,f32-f127 -falign-functions=32"
            case `eval $CC --version` in
                3.*) CFLAGS="${CFLAGS}${CFLAGS:+ }-frename-registers --param max-inline-insns=400" ;;
            esac
            CFLAGS="${CFLAGS}${CFLAGS:+ }-mconstant-gp"
            ;;
        m68*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -fno-strength-reduce -ffixed-a2"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-m${linux_cv_march}"
            CFLAGS=`echo "$CFLAGS" | sed -e'|-fomit-frame-pointer||'`
            ;;
        mips64*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-mabi=64 -G 0 -mno-abicalls -fno-pic -Wa,--trap --pipe"
            case "$linux_cv_march" in
                *r4300*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4300 -mips3" ;;
                *r4?00*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips3" ;;
                *r5000*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r8000 -mips4" ;;
                *r8000*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r8000 -mips4" ;;
                *r10000*)      CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r8000 -mips4" ;;
                *)             CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r8000 -mips3 -mmad" ;;
                # guess nevada
            esac
            CFLAGS="${CFLAGS}${CFLAGS:+ }-Wa,-32"
            ;;
        mips*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-G 0 -mno-abicalls -fno-pic"
            case "$linux_cv_march" in
                *r3000*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r3000 -mips1" ;;
                *r6000*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r6000 -mips2" ;;
                *r4300*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4300 -mips2" ;;
                *r4600*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips2" ;;
                *vr41??*)      CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips2" ;;
                *r4?00*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips2" ;;
                mips64*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips2" ;;
                mips32*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips2" ;;
                *r5000*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r5000 -mips2" ;;
                *r5432*)       CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r5000 -mips2" ;;
                *sb1 | *sb1el) CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r5000 -mips2" ;;
                *)             CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=r4600 -mips2" # guess
                AC_MSG_WARN([*** guessing cpu at r4600 mips2])
                ;;
            esac
            CFLAGS="${CFLAGS}${CFLAGS:+ }-Wa,--trap -pipe"
            ;;
        hppa*)
            linux_k_defs="${linux_k_defs}${linux_k_defs:+ }-D__linux__"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -fno-strength-reduce -mno-space-regs -mfast-indirect-calls -mschedule=7200 -mdisable-fpregs"
            ;;
        ppc* | powerpc*)
            linux_k_defs="${linux_k_defs}${linux_k_defs:+ }-D__powerpc__"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-fsigned-char -msoft-float -pipe -ffixed-r2 -Wno-uninitialized -mmultiple -mstring"
            ;;
        s390x*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -fno-strength-reduce"
            ;;
        s390*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -fno-strength-reduce"
            ;;
        sh*)
            case "$linux_cv_march" in
                *el | shl)  CFLAGS="${CFLAGS}${CFLAGS:+ }-ml" ;;
                *eb | sh)   CFLAGS="${CFLAGS}${CFLAGS:+ }-mb" ;;
                *)          CFLAGS="${CFLAGS}${CFLAGS:+ }-mb"
                AC_MSG_WARN([*** going big endian])
                ;;
            esac
            case "$linux_cv_march" in
                sh3*)   CFLAGS="${CFLAGS}${CFLAGS:+ }-m3" ;;
                sh4*)   CFLAGS="${CFLAGS}${CFLAGS:+ }-m4 -mno-implicit-fp" ;;
                *)      CFLAGS="${CFLAGS}${CFLAGS:+ }-m4 -mno-implicit-fp"
                AC_MSG_WARN([*** going sh4])
                ;;
            esac
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe"
            ;;
        sparc64*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -mno-fpu"
            if (`eval $CC -m64 -mcmodel=medlow -S -o /dev/null -xc /dev/null`) >/dev/null 2>&1
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-m64 -mcpu=ultrasparc -mcmodel=medlow"
            else
                CFLAGS="${CFLAGS}${CFLAGS:+ }-mtune=ultrasparc -mmedlow"
            fi
            CFLAGS="${CFLAGS}${CFLAGS:+ }-ffixed-g4 -fcall-used-g5 -fcall-used-g7 -Wno-sign-compare"
            if (`eval $CC -c -x assembler /dev/null -Wa,--help` | grep undeclared-regs) >/dev/null 2>&1
            then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-Wa,--undeclared-regs"
            fi
            ;;
        sparc*)
            if (`eval $CC -m32 -S -o /dev/null -xc /dev/null`)>/dev/null 2>&1; then
                CFLAGS="${CFLAGS}${CFLAGS:+ }-m32"
            fi
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -mno-fpu -fcall-used-g5 -fcall-used-g7"
            ;;
        *)
            :
            ;;
    esac
    linux_tmp=`eval $CC -print-libgcc-file-name | sed -e 's|libgcc.a|include|'`
    CPPFLAGS="${linux_k_defs}${linux_tmp:+ -nostdinc} -I${linux_cv_k_includes}${linux_tmp:+ -I}${linux_tmp}${CPPFLAGS:+ }${CPPFLAGS}"
])# _LINUX_SETUP_KERNEL_CFLAGS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_VERSIONS
# -------------------------------------------------------------------------
AC_DEFUN(_LINUX_CHECK_KERNEL_VERSIONS,
[
    AC_CACHE_CHECK([for kernel symbol versioning], [linux_cv_k_versions], [dnl
        AC_EGREP_CPP([\<yes_we_have_kernel_versions\>], [
#include <linux/version.h>
#include <linux/config.h>
#ifdef CONFIG_MODVERSIONS
    yes_we_have_kernel_versions
#endif
        ], [linux_cv_k_versions=yes], [linux_cv_k_versions=no])
    ])
    if test :"${linux_cv_k_versions:-no}" = :yes
    then
        KERNEL_MODFLAGS="-DMODULE -DMODVERSIONS"
    else
        KERNEL_MODFLAGS="-DMODULE"
    fi
    AC_SUBST([KERNEL_MODFLAGS])
    KERNEL_NOVERSION="-D__NO_VERSION__"
    AC_SUBST([KERNEL_NOVERSION])
])# _LINUX_CHECK_KERNEL_VERSIONS
# =========================================================================

# =============================================================================
# _LINUX_KERNEL_
# -----------------------------------------------------------------------------
AC_DEFUN(_LINUX_KERNEL_,
[
])# _LINUX_KERNEL_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
