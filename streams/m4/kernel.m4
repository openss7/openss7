# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
# 
# @(#) $RCSfile: kernel.m4,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/06/07 17:45:43 $
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
# Last Modified $Date: 2004/06/07 17:45:43 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _LINUX_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL], [dnl
    AC_REQUIRE([AC_CANONICAL_TARGET])dnl
    _LINUX_KERNEL_OPTIONS
    _LINUX_KERNEL_ENV_BARE([dnl
        CPPFLAGS=
        CFLAGS=
        LDFLAGS=
        _LINUX_KERNEL_SETUP
        KERNEL_CPPFLAGS="$CPPFLAGS"
        KERNEL_CFLAGS="$CFLAGS"
        KERNEL_LDFLAGS="$LDFLAGS"
        AC_SUBST([KERNEL_CPPFLAGS])dnl
        AC_SUBST([KERNEL_CFLAGS])dnl
        AC_SUBST([KERNEL_LDFLAGS])dnl
    ])dnl
])# _LINUX_KERNEL
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV_BARE([COMMANDS])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_ENV_BARE], [dnl
        # move user flags out of the way temporarily
        linux_tmp_cppflags="$CPPFLAGS"
        linux_tmp_cflags="$CFLAGS"
        linux_tmp_ldflags="$LDFLAGS"
        CPPFLAGS="$KERNEL_CPPFLAGS"
        CFLAGS="$KERNEL_CFLAGS"
        LDFLAGS="$KERNEL_LDFLAGS"
        $1
        # move user flags back where they were
        CPPFLAGS="$linux_tmp_cppflags"
        CFLAGS="$linux_tmp_cflags"
        LDFLAGS="$linux_tmp_ldflags"
])# _LINUX_KERNEL_ENV_BARE
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV([COMMANDS])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_ENV], [dnl
    # make sure we have kernel environment
    AC_REQUIRE([_LINUX_KERNEL])dnl
    # protect against nesting
    if test :$linux_env != :kernel
    then
        linux_env='kernel'
        _LINUX_KERNEL_ENV_BARE([$1])
        linux_env='user'
    fi
])# _LINUX_KERNEL_ENV
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_OPTIONS], [dnl
dnl AC_ARG_ENABLE([k-install],
dnl     AS_HELP_STRING([--enable-k-install],
dnl         [specify whether kernel modules will be installed.
dnl         @<:@default=yes@:>@]),
dnl     [enable_k_install="$enableval"],
dnl     [enable_k_install='yes'])
])# _LINUX_KERNEL_OPTIONS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SETUP], [dnl
    _LINUX_CHECK_KERNEL_RELEASE
    _LINUX_CHECK_KERNEL_LINKAGE
    _LINUX_CHECK_KERNEL_PREFIX
    _LINUX_CHECK_KERNEL_ROOTDIR
    _LINUX_CHECK_KERNEL_TOOLS
    _LINUX_CHECK_KERNEL_MODULES
    _LINUX_CHECK_KERNEL_BUILD
    _LINUX_CHECK_KERNEL_SYSMAP
    _LINUX_CHECK_KERNEL_KSYMS
    _LINUX_CHECK_KERNEL_HEADERS
    _LINUX_CHECK_KERNEL_MARCH
    _LINUX_CHECK_KERNEL_RHBOOT
    _LINUX_CHECK_KERNEL_ARCHDIR
    _LINUX_CHECK_KERNEL_MACHDIR
    _LINUX_SETUP_KERNEL_CFLAGS
    _LINUX_SETUP_KERNEL_DEBUG
    _LINUX_CHECK_KERNEL_VERSIONS
    _LINUX_CHECK_KERNEL_MODVERSIONS
    PACKAGE_KNUMBER="${linux_cv_k_major}.${linux_cv_k_minor}.${linux_cv_k_patch}"
    AC_SUBST([PACKAGE_KNUMBER])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_KNUMBER], ["$PACKAGE_KNUMBER"], [The Linux
        kernel version without EXTRAVERSION.])
    PACKAGE_KVERSION="$linux_cv_k_release"
    AC_SUBST([PACKAGE_KVERSION])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_KVERSION], ["$PACKAGE_KVERSION"], [The Linux
        kernel version for which the package was configured.])
])# _LINUX_KERNEL_SETUP
# =============================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_RELEASE], [dnl
    AC_ARG_WITH([k-release],
        AS_HELP_STRING([--with-k-release=UTSRELEASE],
            [specify the UTS release of the linux kernel for which the build
            is targetted.  If this option is not set, the build will be
            targetted at the kernel running in the build environment.
            @<:@default=`uname -r`@:>@]),
        [with_k_release="$withval"],
        [with_k_release=])
    AC_MSG_CHECKING([for kernel release])
    linux_cv_k_running='no'
    if test :"${with_k_release:-no}" != :no ; then
        linux_cv_k_release="$with_k_release"
    else
        if test :"${cross_compiling:-no}" = :no
        then
            linux_cv_k_release="`uname -r`"
            linux_cv_k_running='yes'
        else
            linux_cv_k_release=
        fi
    fi
    AC_MSG_RESULT([${linux_cv_k_release:-no}])
    if test :"${linux_cv_k_release:-no}" = :no
    then
        AC_MSG_ERROR([
*** 
*** You have not specified --with-k-releease indicating that the build is for
*** the running kernel, however, you are cross-compiling.  When
*** cross-compiling, you must specify the kernel release that you are
*** targetting using the --with-k-release option.  This option should specify
*** the entire UTS_RELEASE of the kernel, including the EXTRAVERSION such as
*** that associated with RedHat kernel releases.
*** ])
    fi
    # pull out versions from release number
    AC_CACHE_CHECK([for kernel major release number], [linux_cv_k_major], [dnl
        linux_cv_k_major="`echo $linux_cv_k_release | sed -e 's|[[^0-9]].*||'`" ])
    if test ${linux_cv_k_major:-0} -ne 2
    then
        AC_MSG_ERROR([
*** 
*** Kernel major release number is "$linux_cv_k_major".  That cannot be correct, unless
*** this is really old software now and Linux has already made it to release
*** 3.  In which case, give up.  More likely, "$linux_cv_k_release" is not the
*** UTS_RELEASE of the target.  Check the UTS_RELEASE of the target and
*** specify with with the configure option --with-k-release or look at the
*** result of uname -r ("`uname -r`") to determine the problem.
*** ])
    fi
    AC_CACHE_CHECK([for kernel minor release number], [linux_cv_k_minor], [dnl
        linux_cv_k_minor="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.||;s|[[^0-9]].*||'`" ])
    if test ${linux_cv_k_minor:-0} -ne 4
    then
        AC_MSG_ERROR([
*** 
*** Kernel minor release number "$linux_cv_k_minor" is either too old or too new, or
*** the UTS_RELEASE name "$linux_cv_k_release" is mangled.  Try specifiying a
*** 2.4 kernel with the --with-k-release option to configure.  If you are
*** trying to compile for a 2.2, 2.3, 2.5 or 2.6 kernel, give up.  Only 2.4
*** kernels are supported at the current time.
*** ])
    fi
    AC_CACHE_CHECK([for kernel patch release number], [linux_cv_k_patch], [dnl
        linux_cv_k_patch="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.[[0-9]]*\.||;s|[[^0-9]].*||'`" ])
    if test ${linux_cv_k_patch:-0} -le 10
    then
        AC_MSG_ERROR([
*** 
*** Kernel patch release number "$linux_cv_k_patch" is too old.  Try
*** configuring for a kernel later than 2.4.10.
*** ])
    fi
    AC_CACHE_CHECK([for kernel extra release number], [linux_cv_k_extra], [dnl
        linux_cv_k_extra="`echo $linux_cv_k_release | sed -e 's|[[^-]]*-||'`" ])
    kversion="${linux_cv_k_release}"
    AC_SUBST([kversion])dnl
])# _LINUX_CHECK_KERNEL_RELEASE
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_LINKAGE
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_LINKAGE], [dnl
    AC_ARG_ENABLE([modules],
        AS_HELP_STRING([--disable-modules],
            [disable kernel modules, prepare object for linking into the
            kernel.  @<:@default=no@:>@]),
        [enable_modules="$enableval"],
        [enable_modules='yes'])
    AC_MSG_CHECKING([for kernel linkage])
    if test :"${enable_modules:-yes}" != :yes 
    then
        linux_cv_modules='no'
        linux_tmp='linkable object'
    else
        linux_cv_modules='yes'
        linux_tmp='loadable modules'
        KERNEL_MODFLAGS="$KERNEL_MODFLAGS${KERNEL_MODFLAGS:+ }-DMODULE"
    fi
    AC_MSG_RESULT([$linux_tmp])
])# _LINUX_CHECK_KERNEL_LINKAGE
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_PREFIX
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_PREFIX], [dnl
    AC_ARG_WITH([k-prefix],
        AS_HELP_STRING([--with-k-prefix=DIR],
            [specify the kernel directory prefix for install.
            @<:@default=/@:>@]),
        [with_k_prefix="$withval"],
        [with_k_prefix=])
    AC_MSG_CHECKING([for kernel root prefix])
    if test :"${with_k_prefix:-no}" != :no
    then
        linux_cv_k_prefix="$with_k_prefix"
    else
        if test :"$prefix" = :NONE
        then
            linux_cv_k_prefix="$ac_default_prefix"
        else
            linux_cv_k_prefix="$prefix"
        fi
        dnl strip trailing [/usr][/local]
        linux_cv_k_prefix=`echo $linux_cv_k_prefix | sed -e 's|/local$||;s|/usr$||'`
    fi
    AC_MSG_RESULT([${linux_cv_k_prefix:-no}])
    krootdir="$linux_cv_k_prefix"
    AC_SUBST([krootdir])dnl
])# _LINUX_CHECK_KERNEL_PREFIX
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_ROOTDIR
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_ROOTDIR], [dnl
    AC_ARG_VAR([DESTDIR], [Cross build root directory])
    AC_ARG_WITH([k-rootdir],
        AS_HELP_STRING([--with-k-rootdir=DIR],
            [specify the root directory for configure.
            @<:@default=${DESTDIR:-/}@:>@]),
        [with_k_rootdir="$withval"],
        [with_k_rootdir="$DESTDIR"])
    AC_MSG_CHECKING([for kernel root directory])
    linux_cv_k_rootdir="$with_k_rootdir"
    AC_MSG_RESULT([${linux_cv_k_rootdir:-no}])
])# _LINUX_CHECK_KERNEL_ROOTDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_TOOLS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_TOOLS], [dnl
    AC_ARG_VAR([DEPMOD], [Build kernel module dependencies command])
    if test :"${cross_compiling:-no}" = :no
    then
        AC_PATH_TOOL([DEPMOD], [depmod], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    else
        AC_MSG_CHECKING([for depmod])
        AC_MSG_RESULT([${DEPMOD:-no}])
    fi
    AC_ARG_VAR([MODPROBE], [Probe kernel module dependencies command])
    if test :"${cross_compiling:-no}" = :no
    then
        AC_PATH_TOOL([MODPROBE], [modprobe], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    else
        AC_MSG_CHECKING([for modprobe])
        AC_MSG_RESULT([${MODPROBE:-no}])
    fi
    AC_ARG_VAR([LSMOD], [List kernel modules command])
    if test :"${cross_compiling:-no}" = :no
    then
        AC_PATH_TOOL([LSMOD], [lsmod], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    else
        AC_MSG_CHECKING([for lsmod])
        AC_MSG_RESULT([${LSMOD:-no}])
    fi
    AC_ARG_VAR([LSOF], [List open files command])
    if test :"${cross_compiling:-no}" = :no
    then
        AC_PATH_TOOL([LSOF], [lsof], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    else
        AC_MSG_CHECKING([for lsof])
        AC_MSG_RESULT([${LSOF:-no}])
    fi
])# _LINUX_CHECK_KERNEL_TOOLS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODULES
# -------------------------------------------------------------------------
# This is really just the install directory and is not the final include
# directory.  Therefore, this directory does not need to exist.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODULES], [dnl
    AC_ARG_WITH([k-modules],
        AS_HELP_STRING([--with-k-modules=DIR],
            [specify the directory to which kernel modules will be installed.
            @<:@default=/lib/modules/K-RELEASE/misc@:>@]),
        [with_k_modules="$withval"],
        [with_k_modules=])
    AC_MSG_CHECKING([for kernel modules directory])
    if test :"${with_k_modules:-no}" != :no
    then
        linux_cv_k_modules="$with_k_modules"
    else
        linux_cv_k_modules="$linux_cv_k_prefix/lib/modules/${linux_cv_k_release}"
    fi
    AC_MSG_RESULT([${linux_cv_k_modules:-no}])
    kmoduledir="${linux_cv_k_modules}"
    AC_SUBST([kmoduledir])dnl
])# _LINUX_CHECK_KERNEL_MODULES
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_BUILD
# -------------------------------------------------------------------------
# The linux kernel build directory is the build kernel tree root against which
# kernel modules will be compiled.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_BUILD], [dnl
    AC_ARG_WITH([k-build],
        AS_HELP_STRING([--with-k-build=DIR],
            [specify the base kernel build directory in which configured
            kernel source resides.  @<:@default=K-MODULES-DIR/build@:>@]),
        [with_k_build="$withval"],
        [with_k_build=])
dnl AC_MSG_CHECKING([for kernel build directory])
    if test :"${with_k_build:-no}" != :no
    then
        linux_cv_k_build="$with_k_build"
    else
        eval "k_build_search_path=\"
            ${linux_cv_k_modules:+$linux_cv_k_rootdir$linux_cv_k_prefix$linux_cv_k_modules/build}
            $linux_cv_k_rootdir$linux_cv_k_prefix/lib/modules/$linux_cv_k_release/build
            $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/linux-$linux_cv_k_release
            $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/linux-2.4
            $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/linux
            ${linux_cv_k_modules:+$linux_cv_k_rootdir$linux_cv_k_modules/build}
            $linux_cv_k_rootdir/lib/modules/$linux_cv_k_release/build
            $linux_cv_k_rootdir/usr/src/linux-$linux_cv_k_release
            $linux_cv_k_rootdir/usr/src/linux-2.4
            $linux_cv_k_rootdir/usr/src/linux\""
        k_build_search_path=`echo "$k_build_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        linux_cv_k_build=
        for linux_dir in $k_build_search_path ; do
            AC_MSG_CHECKING([for kernel build directory $linux_dir])
            if test -d "$linux_dir" -a -r "$linux_dir/include/linux/version.h"
            then
                linux_cv_k_build="$linux_dir"
                AC_MSG_RESULT([yes])
                break
            fi
            AC_MSG_RESULT([no])
        done
    fi
    AC_MSG_CHECKING([for kernel build directory])
    AC_MSG_RESULT([${linux_cv_k_build:-no}])
    if test :"${linux_cv_k_build:-no}" = :no -o ! -d "$linux_cv_k_build" 
    then
        if test :"${linux_cv_k_build:-no}" = :no 
        then
            AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel source package for the target kernel
*** "$linux_cv_k_release" and then configure again.
*** 
*** The following directories do no exst in the build environment:
***     $linux_cv_k_rootdir$linux_cv_k_prefix$linux_cv_k_modules/build
***     $linux_cv_k_rootdir$linux_cv_k_prefix/lib/modules/$linux_cv_k_release/build
***     $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/linux-$linux_cv_k_release
***     $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/linux-2.4
***     $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/linux
***     $linux_cv_k_rootdir$linux_cv_k_modules/build
***     $linux_cv_k_rootdir/lib/modules/$linux_cv_k_release/build
***     $linux_cv_k_rootdir/usr/src/linux-$linux_cv_k_release
***     $linux_cv_k_rootdir/usr/src/linux-2.4
***     $linux_cv_k_rootdir/usr/src/linux
*** 
*** Check the settings of the following options before repeating:
***     --with-k-release ${linux_cv_k_release:-no}
***     --with-k-modules ${linux_cv_k_modules:-no}
***     --with-k-build   ${linux_cv_k_release:-no}
*** ])
        else
            if ! -d "$linux_cv_k_build" 
            then
                AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel source package for the target kernel
*** "$linux_cv_k_release" and then configure again.
*** 
*** The following directories do no exst in the build environment:
***     ${linux_cv_k_build}
*** 
*** Check the settings of the following options before repeating:
***     --with-k-release ${linux_cv_k_release:-no}
***     --with-k-modules ${linux_cv_k_modules:-no}
***     --with-k-build   ${linux_cv_k_release:-no}
*** ])
            fi
        fi
    fi
])# _LINUX_CHECK_KERNEL_BUILD
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SYSMAP
# -------------------------------------------------------------------------
# The linux kernel system map is only used for configuration and is not used
# otherwise.  This has to be the system map for the kernel build or for the
# running kernel.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SYSMAP], [dnl
    AC_ARG_WITH([k-sysmap],
        AS_HELP_STRING([--with-k-sysmap=MAP],
            [specify the kernel system map file.
            @<:@default=K-BUILD-DIR/System.map@:>@]),
        [with_k_sysmap="$withval"],
        [with_k_sysmap=])
dnl AC_MSG_CHECKING([for kernel system map])
    if test :"${with_k_sysmap:-no}" != :no
    then
        linux_cv_k_sysmap="$with_k_sysmap"
    else
        eval "k_sysmap_search_path=\"
            $linux_cv_k_build/System.map-$linux_cv_k_release
            $linux_cv_k_build/System.map
            $linux_cv_k_rootdir$linux_cv_k_prefix/boot/System.map-$linux_cv_k_release
            $linux_cv_k_rootdir$linux_cv_k_prefix/boot/System.map
            $linux_cv_k_rootdir/boot/System.map-$linux_cv_k_release
            $linux_cv_k_rootdir/boot/System.map\""
        k_sysmap_search_path=`echo "$k_sysmap_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        linux_cv_k_sysmap=
        for linux_file in $k_sysmap_search_path ; do
            AC_MSG_CHECKING([for kernel system map $linux_file])
            if test -r $linux_file 
            then
                linux_cv_k_sysmap="$linux_file"
                AC_MSG_RESULT([yes])
                break
            fi
            AC_MSG_RESULT([no])
        done
    fi
    AC_MSG_CHECKING([for kernel system map])
    AC_MSG_RESULT([${linux_cv_k_sysmap:-no}])
])# _LINUX_CHECK_KERNEL_SYSMAP
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_KSYMS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_KSYMS], [dnl
    AC_MSG_CHECKING([for kernel /proc/ksyms])
    linux_cv_k_ksyms=
    if test :"${linux_cv_k_running:-no}" = :yes -a -r /proc/ksyms
    then
        linux_cv_k_ksyms='/proc/ksyms'
    fi
    if test :"${linux_cv_k_ksyms:-no}" = :no
    then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([yes])
    fi
])# _LINUX_CHECK_KERNEL_KSYMS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_HEADERS
# -------------------------------------------------------------------------
# The linux kernel header files are only used for configuration and compile
# and are not used otherwise.  This has to be the header files for the kernel
# build or for the running kernel.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_HEADERS], [dnl
    AC_ARG_WITH([k-includes],
        AS_HELP_STRING([--with-k-includes=DIR],
            [specify the include directory of the kernel for which the build
            is targetted.  @<:@default=K-BUILD-DIR/include@:>@]),
        [with_k_includes="$withval"],
        [with_k_includes=])
    AC_MSG_CHECKING([for kernel headers])
    if test :"${with_k_includes:-no}" != :no
    then
        linux_cv_k_includes="$with_k_includes"
    else
        linux_cv_k_includes="${linux_cv_k_build}/include"
    fi
    AC_MSG_RESULT([${linux_cv_k_includes:-no}])
    if test ! -d "$linux_cv_k_includes"
    then
        AC_MSG_ERROR([
*** 
*** Kernel headers directory:
***     "$linux_cv_k_includes"
*** does not exit.  Specify a correct kernel header directory using the
*** --with-k-includes option to configure before attempting again.
*** ])
    fi
])# _LINUX_CHECK_KERNEL_HEADERS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MARCH
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MARCH], [dnl
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
        esac ])
    # fix up the target spec
    target_cpu="$linux_cv_march"
    target="${target_cpu:-unknown}-${target_vendor:-unknown}-${target_os:-none}"
])# _LINUX_CHECK_KERNEL_MARCH
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_RHBOOT
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_RHBOOT], [dnl
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
        fi ])
    if test :"${linux_cv_rh_boot_kernel:-no}" != :no
    then
        AC_DEFINE_UNQUOTED([__BOOT_KERNEL_H_], [], [Define for Red Hat kernel.])
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
AC_DEFUN([_LINUX_CHECK_KERNEL_ARCHDIR], [dnl
    AC_ARG_WITH([k-archdir],
        AS_HELP_STRING([--with-k-archdir=DIR],
            [specify the kernel source architecture specific directory.
            @<:@default=K-BUILD-DIR/arch@:>@]),
        [with_k_archdir="$withval"],
        [with_k_archdir=])
    AC_MSG_CHECKING([for kernel arch directory])
    if test :"${with_k_archdir:-no}" != :no
    then
        linux_cv_k_archdir="$with_k_archdir"
    else
        linux_cv_k_archdir="${linux_cv_k_build}/arch"
    fi
    AC_MSG_RESULT([${linux_cv_k_archdir:-no}])
    if test :"${linux_cv_k_archdir:-no}" != :no -a ! -d "$linux_cv_k_archdir"
    then
        AC_MSG_WARN([*** "$linux_cv_k_archdir" does not exist. ***])
    fi
])# _LINUX_CHECK_KERNEL_ARCHDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MACHDIR
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MACHDIR], [dnl
    AC_ARG_WITH([k-machdir],
        AS_HELP_STRING([--with-k-machdir=DIR],
            [specify the kernel source machine specific directory.
            @<:@default=K-ARCHDIR/ARCH@:>@]),
        [with_k_machdir="$withval"],
        [with_k_machdir=])
    AC_MSG_CHECKING([for kernel mach directory])
    if test :"${with_k_machdir:-no}" != :no
    then
        linux_cv_k_machdir="$with_k_machdir"
    else
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
    fi
    AC_MSG_RESULT([${linux_cv_k_machdir:-no}])
    if test :"${linux_cv_k_machdir:-no}" != :no -a ! -d "$linux_cv_k_machdir"
    then
        AC_MSG_WARN([*** "$linux_cv_k_machdir" does not exist. ***])
    fi
])# _LINUX_CHECK_KERNEL_MACHDIR
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_CFLAGS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_CFLAGS], [dnl
    #
    # FIXME: we need to check what is in CFLAGS before we go adding stuff to
    # CFLAGS, that way the user can override these settings for better tuning.
    #
    # CFLAGS="-Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common"
    CFLAGS="-Wall -Wno-trigraphs"
    AC_ARG_ENABLE([k-inline],
        AS_HELP_STRING([--enable-k-inline],
            [enable kernel inline functions.  @<:@default=no@:>@]),
        [enable_k_inline="$enableval"],
        [enable_k_inline='no'])
    if test :"${enable_k_inline:-no}" != :no 
    then
        CFLAGS="$CFLAGS${CFLAGS:+ }-Winline"
    fi
    AC_ARG_WITH([k-optimize],
        AS_HELP_STRING([--with-k-optimize=HOW],
            [specify optimization, normal, size, speed or quick.
            @<:@default=normal@:>@]),
        [with_k_optimize="$withval"],
        [with_k_optimize='normal'])
    case "${with_k_optimize:-normal}" in
        size)   CFLAGS="$CFLAGS${CFLAGS:+ }-Os" ;;
        speed)  CFLAGS="$CFLAGS${CFLAGS:+ }-O3" ;;
        quick)  CFLAGS="$CFLAGS${CFLAGS:+ }-O0" ;;
        *)      CFLAGS="$CFLAGS${CFLAGS:+ }-O2" ;;
    esac
    CFLAGS="$CFLAGS${CFLAGS:+ }-fomit-frame-pointer -fno-strict-aliasing -fno-common"
    if test :"${enable_k_inline:-no}" != :no 
    then
        CFLAGS="$CFLAGS${CFLAGS:+ }-finline-functions"
    fi
    linux_k_defs='-D__KERNEL__ -DLINUX'
    case "$linux_cv_march" in
        alpha*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -mno-fp-regs -ffixed=8"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-mcpu=`echo $linux_cv_march | sed -e 's|^alpha||'` -Wa,-mev6"
            ;;
        arm*)
            CFLAGS="${CFLAGS}${CFLAGS:+ }-fno-common -pipe -fomit-frame-pointer"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-march=$linux_cv_march"
            AC_MSG_WARN([*** you need to set -mapcs and -mtune yourself in CFLAGS= ***])
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
                AC_MSG_WARN([*** guessing cpu at r4600 mips2 ***])
                ;;
            esac
            CFLAGS="${CFLAGS}${CFLAGS:+ }-Wa,--trap -pipe"
            ;;
        hppa*)
            linux_k_defs="${linux_k_defs}${linux_k_defs:+ }-D__linux__"
            CFLAGS="${CFLAGS}${CFLAGS:+ }-pipe -fno-strength-reduce -mno-space-regs -mfast-indirect-calls -mdisable-fpregs"
            case "$linux_cv_march" in
                *0.9*) CFLAGS="${CFLAGS}${CFLAGS:+ }-march=0.9" ;;
                *2.0*) CFLAGS="${CFLAGS}${CFLAGS:+ }-march=2.0 -mschedule=8000" ;;
                *64*)  : ;;
                *)     : ;;
            esac
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
                AC_MSG_WARN([*** going big endian ***])
                ;;
            esac
            case "$linux_cv_march" in
                sh3*)   CFLAGS="${CFLAGS}${CFLAGS:+ }-m3" ;;
                sh4*)   CFLAGS="${CFLAGS}${CFLAGS:+ }-m4 -mno-implicit-fp" ;;
                *)      CFLAGS="${CFLAGS}${CFLAGS:+ }-m4 -mno-implicit-fp"
                AC_MSG_WARN([*** going sh4 ***])
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
            if (`eval $CC -m32 -S -o /dev/null -xc /dev/null`)>/dev/null 2>&1
            then
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
# _LINUX_SETUP_KERNEL_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_DEBUG], [dnl
    AC_MSG_CHECKING([for kernel debugging])
    linux_cv_debug='_NONE'
    AC_ARG_ENABLE([k-safe],
        AS_HELP_STRING([--enable-k-safe],
            [enable kernel module run-time safety checks.
            @<:@default=yes@:>@]),
        [enable_k_safe="$enableval"],
        [enable_k_safe='no'])
    if test :"${enable_k_safe:-no}" != :no 
    then
        linux_cv_debug='_SAFE'
    fi
    AC_ARG_ENABLE([k-test],
        AS_HELP_STRING([--enable-k-test],
            [enable kernel module run-time testing.  @<:@default=no@:>@]),
        [enable_k_test="$enableval"],
        [enable_k_test='no'])
    if test :"${enable_k_test:-no}" != :no 
    then
        linux_cv_debug='_TEST'
    fi
    AC_ARG_ENABLE([k-debug],
        AS_HELP_STRING([--enable-k-debug],
            [enable kernel module run-time debugging.  @<:@default=no@:>@]),
        [enable_k_debug="$enableval"],
        [enable_k_debug='no'])
    if test :"${enable_k_debug:-no}" != :no 
    then
        linux_cv_debug='_DEBUG'
    fi
    case "$linux_cv_debug" in
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
    AC_MSG_RESULT([${linux_cv_debug:-no}])
])# _LINUX_SETUP_KERNEL_DEBUG
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_VERSIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_VERSIONS], [dnl
    AC_CACHE_CHECK([for kernel symbol versioning], [linux_cv_k_versions], [dnl
        AC_EGREP_CPP([\<yes_we_have_kernel_versions\>], [
#include <linux/version.h>
#include <linux/config.h>
#ifdef CONFIG_MODVERSIONS
    yes_we_have_kernel_versions
#endif
        ], [linux_cv_k_versions=yes], [linux_cv_k_versions=no]) ])
    if test :"${linux_cv_k_versions:-no}" = :yes
    then
        KERNEL_MODFLAGS="$KERNEL_MODFLAGS${KERNEL_MODFLAGS:+ }-DMODVERSIONS"
    fi
    AC_SUBST([KERNEL_MODFLAGS])dnl
    KERNEL_NOVERSION="-D__NO_VERSION__"
    AC_SUBST([KERNEL_NOVERSION])dnl
dnl AM_CONDITIONAL([KERNEL_VERSIONS], test x"$linux_cv_k_versions" = xyes)dnl
])# _LINUX_CHECK_KERNEL_VERSIONS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODVERSIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODVERSIONS], [dnl
    AC_ARG_ENABLE([k-modversions],
        AS_HELP_STRING([--enable-k-modversions],
            [specify whether symbol versioning is to be used on symbols
            exported from built modules.  @<:@default=yes@:>@]),
        [enable_k_modversions="$enableval"],
        [enable_k_modversions='yes'])
    AC_MSG_CHECKING([for kernel module symbol versioning])
    if test :"${enable_k_modversions:-no}" = :yes
    then
        linux_cv_k_modversions='yes'
    else
        linux_cv_k_modversions='no'
    fi
    AC_MSG_RESULT([$linux_cv_k_modversions])
    AM_CONDITIONAL([KERNEL_VERSIONS], test x"$linux_cv_k_modversions" = xyes)dnl
])# _LINUX_CHECK_KERNEL_MODVERSIONS
# =========================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOL_ADDR(SYMBOLNAME, [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SYMBOL_ADDR], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AS_VAR_PUSHDEF([linux_symbol_addr], [linux_cv_$1_addr])dnl
    AC_CACHE_CHECK([for kernel symbol $1 address], linux_symbol_addr, [dnl
        if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" 
        then
            AS_VAR_SET([linux_symbol_addr], [`($EGREP '\<$1' $linux_cv_k_sysmap | sed -e 's| .*||') 2>/dev/null`])
        fi
        linux_tmp=AS_VAR_GET([linux_symbol_addr]);
        AS_VAR_SET([linux_symbol_addr], ["${linux_tmp:+0x}$linux_tmp"])
        linux_tmp=AS_VAR_GET([linux_symbol_addr]);
        AS_VAR_SET([linux_symbol_addr], ["${linux_tmp:-no}"]) ])
    linux_tmp=AS_VAR_GET([linux_symbol_addr])
    if test :"${linux_tmp:-no}" != :no 
    then :; AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$1_ADDR), AS_VAR_GET([linux_symbol_addr]),
            [The symbol $1 is not exported by most kernels.  Define this to
            the address of $1 in the kernel system map so that kernel modules
            can be properly supported.])
        $3
    else :; $2
    fi
    AS_VAR_POPDEF([linux_symbol_addr])dnl
])# _LINUX_KERNEL_SYMBOL_ADDR
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOL_EXPORT(SYMBOLNAME, [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# -----------------------------------------------------------------------------
# This checks for exported symbols before attempting to rip symbols from the
# system maps.  The checks are made to try to find appropriate symbols first.
# If we are configured for the running kernel and /proc/ksyms can be read, it
# contains exported symbols and is used.  Otherwise, if a system map was located
# it will be used.  Otherwise, header files will be checked.  If the symbol is
# not exported, an attempt will be made to rip it from the system maps.
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SYMBOL_EXPORT], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AS_VAR_PUSHDEF([linux_symbol_export], [linux_cv_$1_export])dnl
    AC_CACHE_CHECK([for kernel symbol $1 export], linux_symbol_export, [dnl
        linux_tmp='no'
        if test -n "$linux_cv_k_ksyms" -a -r "$linux_cv_k_ksyms"
        then
            if ( $EGREP -q  '(\<$1_R........\>|\<$1\>)' $linux_cv_k_ksyms 2>/dev/null )
            then
                linux_tmp="yes ($linux_cv_k_ksyms)"
            fi
        else
            if test -n "$linux_cv_k_sysmap" -a -r "$linux_cv_k_sysmap" 
            then
                if ( $EGREP -q '\<__ksymtab_$1\>' $linux_cv_k_sysmap 2>/dev/null )
                then
                    linux_tmp="yes ($linux_cv_k_sysmap)"
                fi
            else
                _LINUX_KERNEL_ENV([dnl
                    CPPFLAGS="$KERNEL_MODFLAGS $CPPFLAGS"
                    AC_EGREP_CPP([\<yes_symbol_$1_is_exported\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/modversions.h>
#include <linux/module.h>
#ifdef $1
        yes_symbol_$1_is_exported
#endif
                    ], [linux_tmp='yes (linux/modversions.h)']) ])
            fi
        fi
        AS_VAR_SET([linux_symbol_export], ["$linux_tmp"]) ])
    linux_tmp=AS_VAR_GET([linux_symbol_export])
    if test :"${linux_tmp:-no}" != :no 
    then :; $3
    else :; _LINUX_KERNEL_SYMBOL_ADDR([$1], [$2], [$3])
    fi
    AS_VAR_POPDEF([linux_symbol_export])dnl
])# _LINUX_KERNEL_SYMBOL_EXPORT
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_], [dnl
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
