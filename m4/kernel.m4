# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) File: m4/kernel.m4
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# =============================================================================

# =============================================================================
# _LINUX_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL], [dnl
    AC_REQUIRE([AC_CANONICAL_TARGET])dnl
    AC_MSG_NOTICE([+---------------+])
    AC_MSG_NOTICE([| Kernel Checks |])
    AC_MSG_NOTICE([+---------------+])
    _LINUX_KERNEL_OPTIONS
    _LINUX_KERNEL_ENV_BARE([dnl
	CPPFLAGS=
	CFLAGS=
	LDFLAGS=
	_LINUX_KERNEL_SETUP
	AC_SUBST([KERNEL_CPPFLAGS])dnl
	AC_SUBST([KERNEL_CFLAGS])dnl
	AC_SUBST([KERNEL_LDFLAGS])dnl
	KERNEL_CPPFLAGS="$CPPFLAGS"
	KERNEL_CFLAGS="$CFLAGS"
	KERNEL_LDFLAGS="$LDFLAGS"])
])# _LINUX_KERNEL
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV_FUNCTIONS
# -----------------------------------------------------------------------------
AC_DEFUN_ONCE([_LINUX_KERNEL_ENV_FUNCTIONS], [dnl
linux_flag_nest=0
])# _LINUX_KERNEL_ENV_FUNCTIONS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV_BARE([COMMANDS])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_ENV_BARE], [dnl
AC_REQUIRE([_LINUX_KERNEL_ENV_FUNCTIONS])dnl
AC_REQUIRE_SHELL_FN([linux_flags_push],
    [AS_FUNCTION_DESCRIBE([linux_flags_push], [], [Save the current compilation
     flags to be popped later.])], [dnl
    eval "linux_${linux_flag_nest}_cc=\"\$CC\""
    eval "linux_${linux_flag_nest}_cppflags=\"\$CPPFLAGS\""
    eval "linux_${linux_flag_nest}_cflags=\"\$CFLAGS\""
    eval "linux_${linux_flag_nest}_ldflags=\"\$LDFLAGS\""
    ((linux_flag_nest++))])dnl
AC_REQUIRE_SHELL_FN([linux_flags_pop],
    [AS_FUNCTION_DESCRIBE([linux_flags_pop], [], [Restore the previous
     compilation flags that were pushed and saved.])], [dnl
    ((linux_flag_nest--))
    eval "CC=\"\$linux_${linux_flag_nest}_cc\""
    eval "CPPFLAGS=\"\$linux_${linux_flag_nest}_cppflags\""
    eval "CFLAGS=\"\$linux_${linux_flag_nest}_cflags\""
    eval "LDFLAGS=\"\$linux_${linux_flag_nest}_ldflags\""])dnl
AC_REQUIRE_SHELL_FN([linux_kernel_env_push],
    [AS_FUNCTION_DESCRIBE([linux_kernel_env_push], [], [Save the current
     compilation flags and the modify them so that they can be used for kernel
     compilation tests.])], [dnl
    linux_flags_push
dnl We need safe versions of these flags without warnings or strange optimizations
dnl but with module flags included
dnl But we need to skip -DMODVERSIONS and -include /blah/blah/modversion.h on rh systems.
    CC="$KCC"
    BLDFLAGS=`echo "$KERNEL_BLDFLAGS" | sed -e "s|'||g;s|.#s|#s|;s|-DKBUILD_BASENAME.*|-DKBUILD_BASENAME=KBUILD_STR(phony)|"`
    MODFLAGS=`echo " $KERNEL_MODFLAGS -DKBUILD_MODNAME=\"phony\" -DDEBUG_HASH=0 -DDEBUG_HASH2=0" | sed -e 's| -DMODVERSIONS||g;s| -include [[^ ]]*||g'`
    CPPFLAGS=`echo " $BLDFLAGS $MODFLAGS $KERNEL_CPPFLAGS " | sed -e 's| -W[[^[:space:]]]*||g;s| -O[[0-9s]]*| -O2|g;s|^ *||;s| *$||'`
    CFLAGS=`echo " $KERNEL_CFLAGS " | sed -e 's| -W[[^[:space:]]]*||g;s| -O[[0-9s]]*| -O2|g;s|^ *||;s| *$||'`
    LDFLAGS=`echo " $KERNEL_LDFLAGS " | sed -e 's| -W[[^[:space:]]]*||g;s| -O[[0-9s]]*| -O2|g;s|^ *||;s| *$||'`
    CFLAGS="$CFLAGS -Werror"
dnl careful about -Wno warnings which we must keep, so put them back one at a time
    for i in $MODFLAGS $KERNEL_CPPFLAGS ; do
	case "$i" in
	    (-Wno-*)
		CPPFLAGS="$CPPFLAGS $i" ;;
	esac
    done
    for i in $KERNEL_CFLAGS ; do
	case "$i" in
	    (-Wno-*)
		CFLAGS="$CFLAGS $i" ;;
	esac
    done
    for i in $KERNEL_LDFLAGS ; do
	case "$i" in
	    (-Wno-*)
		LDFLAGS="$LDFLAGS $i" ;;
	esac
    done])dnl
linux_kernel_env_push
$1
linux_flags_pop
])# _LINUX_KERNEL_ENV_BARE
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV([COMMANDS])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_ENV], [dnl
dnl make sure we have kernel environment
    AC_REQUIRE([_LINUX_KERNEL])dnl
dnl protect against nesting
    _LINUX_KERNEL_ENV_BARE([$1])dnl
])# _LINUX_KERNEL_ENV
# =============================================================================

# =============================================================================
# _LINUX_KBUILD_ENV([COMMANDS])
# -----------------------------------------------------------------------------
# Sets up the file and environment necessary to invoke cflagcheck.  This
# includes copying the configuration to .config, establishing .kernelvariables,
# executing the required [COMMANDS] and then removing these files.
#
# Newer kernel makefiles (2.6.32) take their $(CC) from .kernelvariables and
# cannot be overridden with the CC environment variable.  This was messing up
# option checks on systems where there were multiple gcc's defined and the
# kernel gcc is not the default.  So, now, when $(CC) is not simply gcc it needs
# to be output to .kernelvariables  Note that some other things that we might
# consider is AS, LD, CPP, AR, NM, STRIP, OBJCOPY, OBJDUMP, AWK, GENKSYMS,
# DEPMOD, KALLSYMS, PERL and CHECK.
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KBUILD_ENV], [dnl
	cp -f "$_kconfig" .config
	echo "CC = \$(CROSS_COMPILE)$KCC" > .kernelvariables
	$1
	rm -f .kernelvariables
	rm -f .config
])# _LINUX_KBUILD_ENV
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_OPTIONS], [dnl
dnl AC_ARG_ENABLE([k-install],
dnl     [AS_HELP_STRING([--enable-k-install],
dnl         [kernel modules @<:@default=yes@:>@])],
dnl     [], [enable_k_install=yes])
    AC_ARG_ENABLE([k-package],
	[AS_HELP_STRING([--enable-k-package],
	    [kernel source package @<:@default=disabled@:>@])],
	[], [enable_k_package=no])
    AM_CONDITIONAL([WITH_K_PACKAGE], [test :"${enable_k_package:-no}" != :no])
    AC_CACHE_CHECK([for kernel weak modules], [linux_cv_k_weak_modules], [dnl
    AC_ARG_ENABLE([k-weak-modules],
	[AS_HELP_STRING([--disable-k-weak-modules],
	    [disable use of only exported kernel symbols @<:@default=enabled@:>@])],
	[], [enable_k_weak_modules=yes])
    if test :"${enable_k_weak_modules:-yes}" = :yes ; then
	linux_cv_k_weak_modules='yes'
    else
	linux_cv_k_weak_modules='no'
    fi])
    if test :"${linux_cv_k_weak_modules:-yes}" = :yes ; then :;
	AC_DEFINE([CONFIG_KERNEL_WEAK_MODULES], [1], [When linking a kernel
	module, symbols addresses can be ripped from the system map and resolved
	when packaging the kernel module.  This provides a dependency between
	the kernel module and a specific kernel that is hidden to the module
	loader and is unsuitable for kernel weak updating modules.  When
	defined, ripped kernel symbols will not be used.])
    fi
    AM_CONDITIONAL([WITH_K_WEAK_MODULES], [test x"${enable_k_weak_modules:-yes}" = xyes])
    AC_CACHE_CHECK([for kernel weak symbols], [linux_cv_k_weak_symbols], [dnl
    AC_ARG_ENABLE([k-weak-symbols],
	[AS_HELP_STRING([--disable-k-weak-symbols],
	    [disable the use of weak symbols in the kernel @<:@default=enabled@:>@])],
	[], [enable_k_weak_symbols=yes])
    if test :"${enable_k_weak_symbols:-yes}" = :yes ; then
	linux_cv_k_weak_symbols='yes'
    else
	linux_cv_k_weak_symbols='no'
    fi])
    AH_VERBATIM([CONFIG_KERNEL_WEAK_SYMBOLS], m4_text_wrap([Undefined weak
	symbols can be used where the availability of a specific version of a
	symbol is not guaranteed but can be compensated for at run-time.  A
	typical use is to link a symbol that is not supported by a kernel ABI,
	or when undefined symbols must be resolved at load time.  When defined,
	undefined weak symbols will be used wherever possible. */], [   ],
	[/* ])[
#undef CONFIG_KERNEL_WEAK_SYMBOLS
#ifdef CONFIG_KERNEL_WEAK_SYMBOLS
#define streams_weak __attribute__((weak))
#else
#define streams_weak
#endif])
    if test :"${linux_cv_k_weak_symbols:-yes}" = :yes ; then :;
	AC_DEFINE([CONFIG_KERNEL_WEAK_SYMBOLS], [1])
    fi
    AC_CACHE_CHECK([for kernel abi support], [linux_cv_k_abi_support], [dnl
    AC_ARG_ENABLE([k-abi-support],
	[AS_HELP_STRING([--disable-k-abi-support],
	    [disable the requirement for kabi supported symbols @<:@default=enabled@:>@])],
	[], [enable_k_abi_support=yes])
    if test :"${enable_k_abi_support:-yes}" = :yes ; then
	linux_cv_k_abi_support='yes'
	linux_cv_k_weak_modules='yes'
    else
	linux_cv_k_abi_support='no'
    fi])
    if test :"${linux_cv_k_abi_support:-yes}" = :yes ; then :;
	AC_DEFINE([CONFIG_KERNEL_ABI_SUPPORT], [1], [Not all exported kernel
	symbols are supported by the distributor kernel ABI.  Use of unsupported
	symbols provides a dependency between a series of kernels and a module
	that is hidden to the RPM packaging system, and is unsuitable for kernel
	weak updating modules.  When defined, unsupported kernel symbols will
	not be used.])
    fi
])# _LINUX_KERNEL_OPTIONS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SETUP], [dnl
    _LINUX_CHECK_KERNEL_RELEASE
    _LINUX_CHECK_KERNEL_CACHE
    _LINUX_CHECK_KERNEL_LINKAGE
    _LINUX_CHECK_KERNEL_TOOLS
    _LINUX_CHECK_KERNEL_MODULES
    _LINUX_CHECK_KERNEL_SUBDIR
    _LINUX_CHECK_KERNEL_MARCH
    _LINUX_CHECK_KERNEL_BOOT
    _LINUX_CHECK_KERNEL_BUILDDIR
    _LINUX_CHECK_KERNEL_HDRDIR
    _LINUX_CHECK_KERNEL_SRCDIR
    _LINUX_CHECK_KERNEL_VMLINUX
    _LINUX_CHECK_KERNEL_SYSMAP
    _LINUX_CHECK_KERNEL_KSYMS
    _LINUX_CHECK_KERNEL_KALLSYMS
    _LINUX_CHECK_KERNEL_MINORBITS
    _LINUX_CHECK_KERNEL_COMPILER
    _LINUX_CHECK_KERNEL_ARCHDIR
    _LINUX_CHECK_KERNEL_ARCH
    _LINUX_CHECK_KERNEL_ASMDIR
    _LINUX_CHECK_KERNEL_MACHDIR
    _LINUX_CHECK_KERNEL_DOT_CONFIG
    _LINUX_CHECK_KERNEL_FILES
    _LINUX_SETUP_KERNEL_CFLAGS
    _LINUX_SETUP_KERNEL_BUILDID
    _LINUX_SETUP_KERNEL_DEBUG
    _LINUX_CHECK_KERNEL_SMP
    _LINUX_CHECK_KERNEL_PREEMPT
    _LINUX_CHECK_KERNEL_REGPARM
    _LINUX_CHECK_KERNEL_VERSIONS
    _LINUX_CHECK_KERNEL_MODVERSIONS
    _LINUX_CHECK_KERNEL_MODVER
    _LINUX_CHECK_KERNEL_SYMVERS
    _LINUX_CHECK_KERNEL_MODABI
    _LINUX_CHECK_KERNEL_SYMSETS
    _LINUX_CHECK_KERNEL_KABI
    _LINUX_CHECK_KERNEL_UPDATE_MODULES
    _LINUX_CHECK_KERNEL_MODULE_PRELOAD
    PACKAGE_KNUMBER="${knumber}"
    AC_SUBST([PACKAGE_KNUMBER])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_KNUMBER], ["$PACKAGE_KNUMBER"], [The Linux
	kernel version without EXTRAVERSION.])
    PACKAGE_KVERSION="${kversion}"
    AC_SUBST([PACKAGE_KVERSION])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_KVERSION], ["$PACKAGE_KVERSION"], [The Linux
	kernel version for which the package was configured.])dnl
])# _LINUX_KERNEL_SETUP
# =============================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_RELEASE], [dnl
    AC_MSG_CHECKING([for kernel release])
    AC_ARG_WITH([k-release],
	[AS_HELP_STRING([--with-k-release=UTSRELEASE],
	    [UTS release of target linux kernel @<:@default=`uname -r`@:>@])])
    linux_cv_k_running='no'
    if test :"${with_k_release:-no}" != :no ; then
	linux_cv_k_release="$with_k_release"
	if test :"${cross_compiling:-no}" = :no
	then
	    if test "$linux_cv_k_release" = "`uname -r`"
	    then
		linux_cv_k_running='yes'
	    fi
	fi
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
*** You have not specified --with-k-release indicating that the build
*** is for the running kernel, however, you are cross-compiling.  When
*** cross-compiling, you must specify the kernel release that you are
*** targetting using the --with-k-release option.  This option should
*** specify the entire UTS_RELEASE of the kernel, including the
*** EXTRAVERSION such as that associated with RedHat or Mandrake kernel
*** releases.
*** ])
    fi
dnl pull out versions from release number
    AC_CACHE_CHECK([for kernel major release number], [linux_cv_k_major], [dnl
	linux_cv_k_major="`echo $linux_cv_k_release | sed -e 's|[[^0-9]].*||'`" ])
    case ${linux_cv_k_major:-0} in
    (2|3|4) ;;
    (*)
	AC_MSG_ERROR([
*** 
*** Kernel major release number is "$linux_cv_k_major".  That cannot be
*** correct, unless this is really old software now and Linux has
*** already made it to release 4.  In which case, give up.  More likely,
*** "$linux_cv_k_release" is not the UTS_RELEASE of the target.  Check
*** the UTS_RELEASE of the target and specify it with the configure
*** option --with-k-release or look at the result of uname -r ("`uname
*** -r`") to determine the problem.
*** ]) ;;
    esac
    AC_CACHE_CHECK([for kernel minor release number], [linux_cv_k_minor], [dnl
	linux_cv_k_minor="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.||;s|[[^0-9]].*||'`" ])
    case ${linux_cv_k_major:-0}.${linux_cv_k_minor:-0} in
    (2.4|2.6|3.*|4.*) ;;
    (*)
	AC_MSG_ERROR([
*** 
*** Kernel minor release number "$linux_cv_k_minor" is either too old or
*** too new, or the UTS_RELEASE name "$linux_cv_k_release" is mangled.
*** Try specifiying a 2.4, 2.6 or 3.x kernel with the --with-k-release
*** option to configure.  If you are trying to compile for a 2.2, 2.3, 2.5
*** or 5.x kernel, give up.  Only 2.4, 2.6 and 3.x kernels are supported
*** at the current time.
*** ])
    esac
    AC_CACHE_CHECK([for kernel patch release number], [linux_cv_k_patch], [dnl
	linux_cv_k_patch="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.[[0-9]]*\.||;s|[[^0-9]].*||'`" ])
    if test ${linux_cv_k_major:-0} -eq 2 -a ${linux_cv_k_minor:-0} -eq 4 -a ${linux_cv_k_patch:-0} -le 10
    then
	AC_MSG_ERROR([
*** 
*** Kernel patch release number "$linux_cv_k_patch" is too old.  Try
*** configuring for a kernel later than 2.4.10.
*** ])
    fi
    AC_CACHE_CHECK([for kernel extra release number], [linux_cv_k_extra], [dnl
	linux_cv_k_extra="`echo $linux_cv_k_release | sed -e 's|[[^-]]*-||;s|-.*||'`" ])
    AC_CACHE_CHECK([for kernel flavor], [linux_cv_k_flavor], [dnl
	linux_cv_k_flavor="`echo $linux_cv_k_release | sed -e 's|[[^-]]*-||;s|[[^-]]*||;s|^-||'`" ])
    kversion="${linux_cv_k_release}"
    AC_SUBST([kversion])dnl
    kmajor="${linux_cv_k_major}"
    kminor="${linux_cv_k_minor}"
    kpatch="${linux_cv_k_patch}"
    kextra="${linux_cv_k_extra}"
    kflavor="${linux_cv_k_flavor}"
    knumber="${kmajor}.${kminor}.${kpatch}"
    AC_SUBST([kmajor])dnl
    AC_SUBST([kminor])dnl
    AC_SUBST([kpatch])dnl
    AC_SUBST([knumber])dnl
    AC_SUBST([kextra])dnl
    AC_SUBST([kflavor])dnl
    kseries="${kmajor}.${kminor}.${kpatch}${kflavor:+-$kflavor}"
    if test "$linux_cv_k_major" -eq 2 -a "$linux_cv_k_minor" -eq 4
    then
	AC_DEFINE_UNQUOTED([LINUX_2_4], [1], [Define for the linux 2.4 kernel series.])
	kseries="${kmajor}.${kminor}.${kpatch}${kflavor:+-$kflavor}"
    fi
    if test "$linux_cv_k_major" -eq 2 -a "$linux_cv_k_minor" -eq 6
    then
	AC_DEFINE_UNQUOTED([LINUX_2_6], [1], [Define for the linux 2.6 kernel series.])
	kseries="${kmajor}.${kminor}.${kpatch}${kflavor:+-$kflavor}"
    fi
    if test "$linux_cv_k_major" -eq 3
    then
	AC_DEFINE_UNQUOTED([LINUX_3_X], [1], [Define for the linux 3.x kernel series.])
	kseries="${kmajor}.${kminor}${kflavor:+-$kflavor}"
    fi
    if test "$linux_cv_k_major" -eq 4
    then
	AC_DEFINE_UNQUOTED([LINUX_4_X], [1], [Define for the linux 4.x kernel series.])
	kseries="${kmajor}.${kminor}${kflavor:+-$kflavor}"
    fi
    AC_SUBST([kseries])
    if test "$linux_cv_k_major" -eq 4 -o "$linux_cv_k_major" -eq 3 -o \( "$linux_cv_k_major" -eq 2 -a \( "$linux_cv_k_minor" -gt 5 -o "$linux_cv_k_patch" -ge 48 \) \)
    then
	AC_DEFINE_UNQUOTED([WITH_KO_MODULES], [1], [Define for linux 2.5.48+ .ko kernel modules.])
	kext=".ko"
	linux_cv_k_ko_modules='yes'
	KERNEL_NOVERSION="-D__NO_VERSION__"
	KERNEL_EXPSYMTAB="-DEXPORT_SYMTAB"
    else
	kext=".o"
	linux_cv_k_ko_modules='no'
	KERNEL_NOVERSION="-D__NO_VERSION__"
	KERNEL_EXPSYMTAB="-DEXPORT_SYMTAB"
    fi
    AM_CONDITIONAL([WITH_LINUX_2_4], [test $linux_cv_k_major -eq 2 -a $linux_cv_k_minor -eq 4])
    AM_CONDITIONAL([WITH_LINUX_2_6], [test $linux_cv_k_major -eq 2 -a $linux_cv_k_minor -eq 6])
    AM_CONDITIONAL([WITH_LINUX_3_X], [test $linux_cv_k_major -eq 3])
    AM_CONDITIONAL([WITH_LINUX_4_X], [test $linux_cv_k_major -eq 4])
    AM_CONDITIONAL([WITH_KO_MODULES], [test :${linux_cv_k_ko_modules:-no} = :yes])
    AC_SUBST([kext])dnl
    AC_SUBST([KERNEL_NOVERSION])dnl
    AC_SUBST([KERNEL_EXPSYMTAB])dnl
])# _LINUX_CHECK_KERNEL_RELEASE
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_LINKAGE
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_LINKAGE], [dnl
    AC_CACHE_CHECK([for kernel module/object linkage], [linux_cv_k_linkage], [dnl
	AC_ARG_WITH([k-linkage],
	    [AS_HELP_STRING([--with-k-linkage=LINKAGE],
		[kernel LINKAGE: 'loadable' for loadable modules or 'linkable'
		for linkable objects.  @<:@default=loadable@:>@])], [dnl
	    with_k_linkage="$withval"
	    case :${with_k_linkage:-loadable} in
		(:linkable|:objects)   with_k_linkage='linkable' ;;
		(:loadable|:modules|:*) with_k_linkage='loadable';;
	    esac],
	    [with_k_linkage=loadable])
	if test :${with_k_linkage:-loadable} = :linkable ; then
	    linux_cv_k_linkage='linkable'
	else
	    linux_cv_k_linkage='loadable'
	fi
    ])
dnl # Pointless now that we rip module flags from the kernel Makefile
dnl if test :${linux_cv_k_linkage:-loadable} = :loadable
dnl then
dnl	KERNEL_MODFLAGS="$KERNEL_MODFLAGS${KERNEL_MODFLAGS:+ }-DMODULE"
dnl fi
])# _LINUX_CHECK_KERNEL_LINKAGE
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_TOOLS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_TOOLS], [dnl
    AC_CACHE_CHECK([for kernel compiler], [linux_cv_k_cc], [dnl
	if test ":${KCC+set}" != :set ; then
	    KCC="${CC:-gcc}"
	fi
	linux_cv_k_cc="$KCC"
    ])
    if test ":${KCC+set}" != :set ; then
	KCC="${linux_cv_k_cc:-gcc}"
    fi
    if test ":${KCC:-gcc}" != :gcc; then
	PACKAGE_RPMOPTIONS="KCC=\"$KCC\"${PACKAGE_RPMOPTIONS:+ $PACKAGE_RPMOPTIONS}"
	PACKAGE_DEBOPTIONS="KCC=\"$KCC\"${PACKAGE_DEBOPTIONS:+ $PACKAGE_DEBOPTIONS}"
	ac_configure_args="KCC=\"$KCC\"${ac_configure_args:+ $ac_configure_args}"
    fi
    AC_SUBST([KCC])dnl
    AC_CACHE_CHECK([for kernel pre-processor], [linux_cv_k_cpp], [dnl
	if test ":${KCPP+set}" != :set ; then
	    KCPP="${KCC:-gcc} -E"
	fi
	linux_cv_k_cpp="$KCPP"
    ])
    if test ":${KCPP+set}" != :set ; then
	KCPP="${linux_cv_k_cpp:-gcc -E}"
    fi
    if test ":${KCPP:-gcc -E}" != ":gcc -E"; then
	PACKAGE_RPMOPTIONS="KCPP=\"$KCPP\"${PACKAGE_RPMOPTIONS:+ $PACKAGE_RPMOPTIONS}"
	PACKAGE_DEBOPTIONS="KCPP=\"$KCPP\"${PACKAGE_DEBOPTIONS:+ $PACKAGE_DEBOPTIONS}"
	ac_configure_args="KCPP=\"$KCPP\"${ac_configure_args:+ $ac_configure_args}"
    fi
    AC_SUBST([KCPP])dnl
    tmp_PATH="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    AC_ARG_VAR([DEPMOD],
	       [Build kernel module dependencies command. @<:@default=depmod@:>@])
    _BLD_PATH_PROG([DEPMOD], [depmod], [/sbin/depmod], [$tmp_PATH], [dnl
	AC_MSG_WARN([Cannot find depmod program in PATH.])])
    AC_ARG_VAR([MODPROBE],
	       [Probe kernel module dependencies command. @<:@default=modprobe@:>@])
    _BLD_PATH_PROG([MODPROBE], [modprobe], [/sbin/modprobe], [$tmp_PATH], [dnl
	AC_MSG_WARN([Cannot find depmod program in PATH.])])
    AC_ARG_VAR([LSMOD],
	       [List kernel modules command. @<:@default=lsmod@:>@])
    _BLD_PATH_PROG([LSMOD], [lsmod], [/sbin/lsmod], [$tmp_PATH], [dnl
	AC_MSG_WARN([Cannot find lsmod program in PATH.])])
    AC_ARG_VAR([LSOF],
	       [List open files command. @<:@default=lsof@:>@])
    _BLD_PATH_PROG([LSOF], [lsof], [/sbin/lsof], [$tmp_PATH], [dnl
	AC_MSG_WARN([Cannot find lsof program in PATH.])])
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
	[AS_HELP_STRING([--with-k-modules=DIR],
	    [kernel module install directory @<:@default=/lib/modules/K-RELEASE/misc@:>@])])
    _BLD_FIND_DIR([for kernel modules install directory], [linux_cv_k_modules], [
	    ${rootdir}/usr/lib/modules/${kversion}
	    ${rootdir}/lib/modules/${kversion}
    ], [], [${rootdir}/lib/modules/${kversion}], [dnl
		AC_MSG_ERROR([
*** 
*** Configure could not find the kernel modules directory for the
*** specified kernel version.  The directories search were:
***	"$with_k_modules"
***	"$bld_search_path"
***
*** Check the settings of the following options before repeating:
***	--with-k-release=${kversion}
***	--with-k-modules=${with_k_modules}
*** ])],
	    [], [with_k_modules])
    _kmoduledir="$linux_cv_k_modules_eval"
    kmoduledir="$linux_cv_k_modules"
    AC_SUBST([kmoduledir])dnl
    _BLD_FIND_DIR([for kernel modules directory], [linux_cv_k_moddir], [
	    ${rootdir}/usr/lib/modules/${kversion}
	    ${rootdir}/lib/modules/${kversion}
    ], [modules.dep], [], [dnl
		AC_MSG_WARN([
***
*** Strange, the modules directory is $kmoduledir
*** but the file modules.dep could not be found in
***	"$with_k_modules"
***	"$bld_search_path"
***
*** Check the settings of the following options before repeating:
***	--with-k-release ${kversion:-no}
***	--with-k-modules ${kmoduledir:-no}
***
*** This could cause some problems later.
*** ])],
	    [], [with_k_modules])
    _kmoddir="$linux_cv_k_moddir_eval"
    kmoddir="$linux_cv_k_moddir"
    AC_SUBST([kmoddir])dnl
    AC_CACHE_CHECK([for kernel module compression], [linux_cv_k_compress], [dnl
	if test -r $_kmoddir/modules.dep
	then
	    if ( egrep -q '\.(k)?o\.gz:' $_kmoddir/modules.dep )
	    then
		linux_cv_k_compress='yes'
	    else
		linux_cv_k_compress='no'
	    fi
	else
	    linux_cv_k_compress='unknown'
	fi ])
    case $linux_cv_k_compress in
	(yes)		COMPRESS_KERNEL_MODULES='gzip -f9v' ; kzip=".gz" ;;
	(no)		COMPRESS_KERNEL_MODULES= ; kzip= ;;
	(unknown|*)	COMPRESS_KERNEL_MODULES= ; kzip=
		AC_MSG_WARN([
*** 
*** Strange, the modules directory is $_kmoddir
*** but the file $_kmoddir/modules.dep
*** does not exist.  This could cause some problems later.
*** ])
	    ;;
    esac
    AC_SUBST([COMPRESS_KERNEL_MODULES])dnl
    AC_SUBST([kzip])dnl
])# _LINUX_CHECK_KERNEL_MODULES
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SUBDIR
# -------------------------------------------------------------------------
# Check which kernel subdirectory to use.  For RedHat/Fedora and others
# based on /sbin/weak-modules, use extra/openss7.  For SuSE Code 10, use
# updates/openss7.  For SuSE Code 11, use extra/openss7.  For others, just
# use openss7.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SUBDIR], [dnl
    AC_CACHE_CHECK([for kernel modules install subdirectory], [linux_cv_k_subdir], [dnl
	AC_ARG_WITH([k-subdir],
	    [AS_HELP_STRING([--with-k-subdir=SUBDIR],
		[kernel module install subdirectory @<:@default=auto@:>@])])
	if test :"${with_k_subdir:-no}" != :no
	then
	    linux_cv_k_subdir="$with_k_subdir"
	else
	    linux_cv_k_subdir=
	    if test -z "$linux_cv_k_subdir" -a :"${linux_cv_k_ko_modules:-no}" = :no
	    then
		linux_cv_k_subdir="$PACKAGE_LCNAME"
	    fi
	    if test -z "$linux_cv_k_subdir" -a -x /usr/lib/module-init-tools/weak-modules2
	    then
		linux_cv_k_subdir="extra/$PACKAGE_LCNAME"
	    fi
	    if test -z "$linux_cv_k_subdir" -a -x /usr/lib/module-init-tools/weak-modules
	    then
		linux_cv_k_subdir="updates/$PACKAGE_LCNAME"
	    fi
	    if test -z "$linux_cv_k_subdir" -a -x /sbin/weak-modules
	    then
		linux_cv_k_subdir="extra/$PACKAGE_LCNAME"
	    fi
	    if test -z "$linux_cv_k_subdir"
	    then
		linux_cv_k_subdir="updates/$PACKAGE_LCNAME"
	    fi
	fi
    ])
    ksubdir="$linux_cv_k_subdir"
    AC_SUBST([ksubdir])
    KERNEL_SUBDIR="$linux_cv_k_subdir"
    AC_SUBST([KERNEL_SUBDIR])
])# _LINUX_CHECK_KERNEL_SUBDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MARCH
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MARCH], [dnl
    AC_CACHE_CHECK([for kernel machine], [linux_cv_march], [dnl
	linux_cv_march="$target_cpu"
	case "$target_cpu" in
	    (i586)
		case "${target_alias:-${host_alias:-$build_alias}}" in
		    (k6-*)	linux_cv_march=k6 ;;
		    (*)		linux_cv_march=i586 ;;
		esac
		;;
	    (i686)
		case "${target_alias:-${host_alias:-$build_alias}}" in
		    (athlon-*)	linux_cv_march=athlon ;;
		    (*)		linux_cv_march=i686 ;;
		esac
		;;
	    (i?86)		linux_cv_march=i386 ;;
	esac ])
    kmarch="$linux_cv_march"
    AC_SUBST([kmarch])dnl
    karch="$target_cpu"
    AC_SUBST([karch])dnl
])# _LINUX_CHECK_KERNEL_MARCH
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_BOOT
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_BOOT], [dnl
    AC_REQUIRE([_DISTRO])
    AC_CACHE_CHECK([for kernel $dist_cv_host_distrib boot], [linux_cv_k_boot], [dnl
	linux_cv_k_boot=no
	linux_cv_k_base="$kversion"
	case "$target_vendor" in
	    (oracle|springdale|puias|centos|whitebox|redhat|scientific)
		case "${kversion}" in
		    # redhat boot kernels
		    (*BOOT)	    linux_cv_k_boot=BOOT	 ;;
		    (*smp)	    linux_cv_k_boot=smp		 ;;
		    (*bigmem)	    linux_cv_k_boot=bigmem	 ;;
		    (*hugemem)	    linux_cv_k_boot=hugemem	 ;;
		    (*debug)	    linux_cv_k_boot=debug	 ;;
		    (*enterprise)   linux_cv_k_boot=enterprise	 ;;
		    (*secure)	    linux_cv_k_boot=secure	 ;;
		    (*)		    linux_cv_k_boot=		 ;;
		esac
		linux_cv_k_base=`echo "$kversion" | sed -r -e s/$linux_cv_k_boot$//`
		;;
	    (mandrake)
		case "${kversion}" in
		    # mandrake boot kernels
		    (*smp)	    linux_cv_k_boot=smp		 ;;
		    (*enterprise)   linux_cv_k_boot=enterprise	 ;;
		    (*secure)	    linux_cv_k_boot=secure	 ;;
		    (*i686-up-4GB)  linux_cv_k_boot=i686-up-4GB	 ;;
		    (*maximum)	    linux_cv_k_boot=maximum	 ;; # FIXME
		    (*p3-smp-64GB)  linux_cv_k_boot=p3-smp-64GB	 ;;
		    (*i586-up-1GB)  linux_cv_k_boot=i586-up-1GB	 ;;
		    (*i686-up-64GB) linux_cv_k_boot=i686-up-64GB ;;
		    (*)		    linux_cv_k_boot=		 ;;
		esac
		linux_cv_k_base=`echo "$kversion" | sed -r -e s/$linux_cv_k_boot$// -e s/-$//`
		;;
	    (mandriva|mageia)
		case "${kversion}" in
		    # mandriva boot kernels (not needed for kernel.h)
		    (*-rsbac-desktop586-*)  linux_cv_k_boot='rsbac-desktop586'	;;
		    (*-rsbac-desktop-*)	    linux_cv_k_boot='rsbac-desktop'	;;
		    (*-rsbac-server-*)	    linux_cv_k_boot='rsbac-server'	;;
		    (*-tmb-desktop586-*)    linux_cv_k_boot='tmb-desktop586'	;;
		    (*-tmb-desktop-*)	    linux_cv_k_boot='tmb-desktop'	;;
		    (*-tmb-server-*)	    linux_cv_k_boot='tmb-server'	;;
		    (*-tmb-laptop-*)	    linux_cv_k_boot='tmb-laptop'	;;
		    (*-desktop586-*)	    linux_cv_k_boot='desktop586'	;;
		    (*-desktop-*)	    linux_cv_k_boot='desktop'		;;
		    (*-server-*)	    linux_cv_k_boot='server'		;;
		    (*-netbook-*)	    linux_cv_k_boot='netbook'		;;
		    (*-kerrighed-*)	    linux_cv_k_boot='kerrighed'		;;
		    (*-vserver-*)	    linux_cv_k_boot='vserver'		;;
		    (*-openvz-*)	    linux_cv_k_boot='openvz'		;;
		    (*-linus-*)		    linux_cv_k_boot='linux'		;;
		    (*-xen-pvops-*)	    linux_cv_k_boot='xen-pvops'		;;
		    (*-xen-*)		    linux_cv_k_boot='xen'		;;
		    (*-rt-*)		    linux_cv_k_boot='rt'		;;
		    (*)			    linux_cv_k_boot=			;;
		esac
		;;
	    (suse)
		case "${kversion}" in
		    # suse boot kernels
		    (*-default)	    linux_cv_k_boot=default	 ;;
		    (*-smp)	    linux_cv_k_boot=smp		 ;;
		    (*-bigsmp)	    linux_cv_k_boot=bigsmp	 ;;
		    (*-debug)	    linux_cv_k_boot=debug	 ;;
		    (*)		    linux_cv_k_boot=		 ;;
		esac
		linux_cv_k_base=`echo "$kversion" | sed -r -e s/-$linux_cv_k_boot$//`
		;;
	    (debian|ubuntu|mint)
		case "${kversion}" in
		    # debian boot kernels
		    (*)		    linux_cv_k_boot=		;;
		esac
		linux_cv_k_base="$kversion"
		;;
	    (arch|slackware|salix|*)
		linux_cv_k_boot=no
		linux_cv_k_base="$kversion"
		;;
	esac
	kbase="$linux_cv_k_base"
    ])
    kboot=
    if test :"$linux_cv_k_boot" != :no
    then
	AC_DEFINE_UNQUOTED([__BOOT_KERNEL_H_], [], [Define for RedHat/Mandrake kernel.])
	case "$linux_cv_k_boot" in
	    (BOOT)	   AC_DEFINE([__BOOT_KERNEL_BOOT],	   [1], [Define for RedHat BOOT kernel.])		 ;;
	    (smp)	   AC_DEFINE([__BOOT_KERNEL_SMP],	   [1], [Define for RedHat/Mandrake SMP kernel.])	 ;;
	    (bigmem)	   AC_DEFINE([__BOOT_KERNEL_BIGMEM],	   [1], [Define for RedHat BIGMEM kernel.])		 ;;
	    (hugemem)	   AC_DEFINE([__BOOT_KERNEL_HUGEMEM],	   [1], [Define for RedHat HUGEMEM kernel.])		 ;;
	    (debug)	   AC_DEFINE([__BOOT_KERNEL_DEBUG],	   [1], [Define for RedHat DEBUG kernel.])		 ;;
	    (enterprise)   AC_DEFINE([__BOOT_KERNEL_ENTERPRISE],   [1], [Define for RedHat/Mandrake ENTERPRISE kernel.]) ;;
	    (secure)	   AC_DEFINE([__BOOT_KERNEL_SECURE],	   [1], [Define for RedHat/Mandrake SECURE kernel.])	 ;;
	    (i686-up-4GB)  AC_DEFINE([__BOOT_KERNEL_I686_UP_4GB],  [1], [Define for Mandrake I686_UP_4GB kernel.])	 ;;
	    (maximum)	   AC_DEFINE([__BOOT_KERNEL_MAXIMUM],      [1], [Define for Mandrake MAXIMUM kernel.])		 ;;
	    (p3-smp-64GB)  AC_DEFINE([__BOOT_KERNEL_P3_SMP_64GB],  [1], [Define for Mandrake P3_SMP_64GB kernel.])	 ;;
	    (i568-up-1GB)  AC_DEFINE([__BOOT_KERNEL_I586_UP_1GB],  [1], [Define for Mandrake I586_UP_1GB kernel.])	 ;;
	    (i686-up-64GB) AC_DEFINE([__BOOT_KERNEL_I686_UP_64GB], [1], [Define for Mandrake I686_UP_64GB kernel.])	 ;;
	    (UP)	   AC_DEFINE([__BOOT_KERNEL_UP],	   [1], [Define for RedHat/Mandrake UP kernel.])	 ;;
	esac
	case "$linux_cv_k_boot" in
	    (enterprise)   case "$target_vendor" in (mandrake) case "$kmarch" in (i586|k6) kmarch=i686 ;; esac ;; esac	 ;;
	    (i686-up-4GB)  case "$target_vendor" in (mandrake) case "$kmarch" in (i586|k6) kmarch=i686 ;; esac ;; esac	 ;;
	    (maximum)	   case "$target_vendor" in (mandrake) case "$kmarch" in (i586|k6) kmarch=i686 ;; esac ;; esac	 ;;
	    (p3-smp-64GB)  case "$target_vendor" in (mandrake) case "$kmarch" in (i586|k6) kmarch=i686 ;; esac ;; esac	 ;;
	esac
	case "$kmarch" in
	    (alpha*)	   AC_DEFINE([__MODULE_KERNEL_alpha],	  [1], [Define for alpha RedHat/Mandrake kernel.])	 ;;
	    (athlon)	   AC_DEFINE([__MODULE_KERNEL_athlon],	  [1], [Define for athlon RedHat/Mandrake kernel.])	 ;;
	    (i586)	   AC_DEFINE([__MODULE_KERNEL_i586],	  [1], [Define for i586 RedHat/Mandrake kernel.])	 ;;
	    (i686)	   AC_DEFINE([__MODULE_KERNEL_i686],	  [1], [Define for i686 RedHat/Mandrake kernel.])	 ;;
	    (i?86)	   AC_DEFINE([__MODULE_KERNEL_i386],	  [1], [Define for i386 RedHat/Mandrake kernel.])	 ;;
	    (ia32e)	   AC_DEFINE([__MODULE_KERNEL_ia32e],	  [1], [Define for ia32e RedHat/Mandrake kernel.])	 ;;
	    (ia64)	   AC_DEFINE([__MODULE_KERNEL_ia64],	  [1], [Define for ia64 RedHat/Mandrake kernel.])	 ;;
	    (powerpc64*)   AC_DEFINE([__MODULE_KERNEL_ppc64],	  [1], [Define for ppc64 RedHat/Mandrake kernel.])	 ;;
	    (powerpc*)	   AC_DEFINE([__MODULE_KERNEL_ppc],	  [1], [Define for ppc RedHat/Mandrake kernel.])	 ;;
	    (s390x)	   AC_DEFINE([__MODULE_KERNEL_s390x],	  [1], [Define for s390x RedHat/Mandrake kernel.])	 ;;
	    (s390)	   AC_DEFINE([__MODULE_KERNEL_s390],	  [1], [Define for s390 RedHat/Mandrake kernel.])	 ;;
	    (x86_64)	   AC_DEFINE([__MODULE_KERNEL_x86_64],	  [1], [Define for x86_64 RedHat/Mandrake kernel.])	 ;;
	    (sparc64*)	   AC_DEFINE([__MODULE_KERNEL_sparc64],	  [1], [Define for sparc64 RedHat/Mandrake kernel.])	 ;;
	    (sparc*)	   AC_DEFINE([__MODULE_KERNEL_sparc],	  [1], [Define for sparc RedHat/Mandrake kernel.])	 ;;
	esac
	kboot="$linux_cv_k_boot"
    fi
    kbase="$linux_cv_k_base"
    AC_SUBST([kbase])dnl
    AC_SUBST([kboot])dnl
])# _LINUX_CHECK_KERNEL_BOOT
# =========================================================================

# Some notes about kernel directories:
#
# For earlier kernels in the 2.4 series, there is typically only one
# directory, source, which is typically /usr/src/linux/`uname -r` but may
# also be referenced via /lib/modules/`uname -r`/build.  This single
# directory contains fully configurable source.
#
# There are now four possible directories:
# - build,  typically /lib/modules/`uname -r`/build for the running kernel.
# - common, typically /lib/modules/`uname -r`/source for the running kernel.
# - source, typcially /usr/src/linux-source-${knumber}
# - kbuild, typically /usr/src/linux-kbuild-${knumber}
#
# There are now four header directories:
#
# /lib/modules/`uname -r`/build
#   This contains build-specific files.  That is, files that are specific
#   to a given configuration of the kernel (e.g. xen) for a given
#   architecture.  This contains only configuration and output files,
#   including files autogenerated by the makefile.  This is the
#   KBUILD_OUTPUT directory for the makefile.
#
#   When it exists, this directory is assigned to kbuilddir.
#
# /lib/modules/`uname -r`/source
#   This contains architecture-specific files that are common to all
#   builds for the same architecture.  This contains only default
#   configuration file, header files for the architecture, and symbolic
#   links to common (non-architecture specific) file.  This contains
#   symbolic links into the kbuild directory.  This is the KBUILD_SRC
#   directory for the makefile.
#
#   When it exists, this directory is assigned to khdrdir.
#
# /lib/modules/linux-kbuild-${knumber}
#   This contains kernel number specific kbuild scripts.  This allows
#   kbuild changes to be made independent of kernel changes.  The khdrdir
#   directory links into this directory.  So, there is no need to find it.
#
# /lib/modules/linux-source-${knumber}
#   This contains full linux kernel source tree.  It is not configured and
#   contains no output files.
#
#   When it exists, this directory is assigned to ksrcdir.

# =========================================================================
# _LINUX_CHECK_KERNEL_BUILDDIR
# -------------------------------------------------------------------------
# The linux kernel build directory is the build kernel tree root against which
# kernel modules will be compiled.  In previous 2.4 systems this and the
# kernel source tree are the same.  In 2.6 based systems this build directory
# and the source directory can be different.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_BUILDDIR], [dnl
    AC_ARG_WITH([k-build],
	[AS_HELP_STRING([--with-k-build=DIR],
	    [kernel build directory @<:@default=K-MODULES-DIR/build@:>@])])
    _BLD_FIND_DIR([for kernel build directory], [linux_cv_k_build], [
	    ${kmoduledir:+${kmoduledir}/build}
	    ${rootdir}/usr/lib/modules/${kversion}/build
	    ${rootdir}/lib/modules/${kversion}/build
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}
	    ${rootdir}/usr/src/kernels/${kversion}
	    ${rootdir}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}
	    ${rootdir}/usr/src/linux-obj
	    ${rootdir}/usr/src/kernel-headers-${kversion}
	    ${rootdir}/usr/src/kernel-headers-${knumber}-${kextra}
	    ${rootdir}/usr/src/kernel-headers-${knumber}
	    ${rootdir}/usr/src/linux-headers-${kversion}
	    ${rootdir}/usr/src/linux-headers-${knumber}-${kextra}
	    ${rootdir}/usr/src/linux-headers-${knumber}
	    ${rootdir}/usr/src/linux-${kversion}
	    ${rootdir}/usr/src/linux-${kbase}
	    ${rootdir}/usr/src/linux-${knumber}-${kextra}
	    ${rootdir}/usr/src/linux-${knumber}
	    ${rootdir}/usr/src/linux-${kmajor}.${kminor}
	    ${rootdir}/usr/src/linux], [], [], [dnl
		AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel-source or kernel-headers package for the
*** target kernel "${kversion}" and then configure again.
*** 
*** The following directories do no exist in the build environment:
***	"$with_k_build"
***	"$bld_search_path"
*** 
*** Check the settings of the following options before repeating:
***     --with-k-release ${kversion:-no}
***     --with-k-modules ${kmoduledir:-no}
***     --with-k-build   ${kversion:-no}
*** ])],[dnl
	    if test -f "$bld_dir/include/generated/autoconf.h" ; then
		linux_cv_k_autoconf='<generated/autoconf.h>'
	    else
		linux_cv_k_autoconf='<linux/autoconf.h>'
	    fi],[with_k_build],
	    [-a \( -f "$bld_dir/include/linux/autoconf.h" -o -f "$bld_dir/include/generated/autoconf.h" \) -a \( -f "$bld_dir/include/linux/version.h" -o -f "$bld_dir/include/generated/uapi/linux/version.h" \)])
    _kbuilddir="$linux_cv_k_build_eval"
    kbuilddir="$linux_cv_k_build"
    AC_SUBST([kbuilddir])dnl
    kincludedir="$kbuilddir/include"
    AC_SUBST([kincludedir])
])# _LINUX_CHECK_KERNEL_BUILDDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_HDRDIR
# -------------------------------------------------------------------------
# the linux kernel header directory is a directory that contains common
# files that are not architecture dependent, but consist only of headers
# necessary to create external kernel modules.  Under debian, at least,
# these subdirectories and files used to be linked from the build
# directory; however, squeeze no longer links these from the build
# directory.  This is different from the source directory, which is where
# we expect to find a fully configured linux source tree.  When this
# directory does not exist, it should default to the source directory.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_HDRDIR], [dnl
    AC_ARG_WITH([k-hdrdir],
	[AS_HELP_STRING([--with-k-hdrdir=DIR],
	    [kernel header directory @<:@default=K-BUILD-DIR@:>@])])
dnl 
dnl Check the build directory first, because this was the place that the
dnl common headers where positioned before.  If they are not in the build
dnl directory, we need to go searching for them.
dnl 
    _BLD_FIND_DIR([for kernel header directory], [linux_cv_k_hdrdir], [
	    ${kbuildir}/include/asm-generic
	    ${rootdir}/usr/lib/modules/${kversion}/source
	    ${rootdir}/lib/modules/${kversion}/source
	    ${rootdir}/usr/src/linux-headers-${kversion}
	    ${rootdir}/usr/src/linux-headers-${knumber}
	    ${rootdir}/usr/src/linux-headers-${knumber}-${kextra}
	    ${rootdir}/usr/src/linux-headers-${knumber}-${kextra}-common
	    ${rootdir}/usr/src/linux-headers-${knumber}-${kextra}-common-${kflavor}],
	    [], [$kbuilddir], [], [], [with_k_hdrdir], [-a -d "$bld_dir/include/asm-generic"])
    _khdrdir="$linux_cv_k_hdrdir_eval"
    khdrdir="$linux_cv_k_hdrdir"
    AC_SUBST([khdrdir])
])# _LINUX_CHECK_KERNEL_HDRDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SRCDIR
# -------------------------------------------------------------------------
# the linux kernel source directory is the kernel source tree root against
# which kernel modules will be compiled.  Under 2.4 kernels, this is the same
# as the build directory above (because builds can only be performed in the
# source directory).  However, for later 2.6 kernels it is possible to build
# in a directory separate from the kernel source tree.  SuSE 9.2 uses this to
# separate the build information from the source information.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SRCDIR], [dnl
    AC_ARG_WITH([k-source],
	[AS_HELP_STRING([--with-k-source=DIR],
	    [kernel source directory @<:@default=/usr/src/K_VERSION@:>@])])
dnl 
dnl Look for a directory that contains kernel/sched.c.  If this dir does
dnl not include sched.c then it is not a full source directory.
dnl 
    _BLD_FIND_DIR([for kernel source directory], [linux_cv_k_source], [
	    ${kbuilddir}
	    ${rootdir}/usr/src/kernel-source-${knumber}
	    ${rootdir}/usr/src/linux-source-${knumber}
	    ${rootdir}/usr/src/linux-source-${knumber}-${kextra}
	    ${rootdir}/usr/src/linux-${kversion}
	    ${rootdir}/usr/src/linux-${kbase}
	    ${rootdir}/usr/src/linux-${knumber}
	    ${rootdir}/usr/src/linux-${knumber}-${kextra}
	    ${rootdir}/usr/src/linux-${kmajor}.${kminor}
	    ${rootdir}/usr/src/linux
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}
	    ${rootdir}/usr/src/kernels/${kversion}
	    ${rootdir}/usr/src/linux-headers-${kversion}
	    ${rootdir}/usr/src/linux-headers-${knumber}
	    ${rootdir}/usr/src/linux-headers-${knumber}-${kextra}],
	    [], [$khdrdir], [], [], [with_k_source],
	    [-a -f "$bld_dir/Makefile" -a -d "$bld_dir/kernel" -a -f "$bld_dir/kernel/sched.c"])
    _ksrcdir="$linux_cv_k_source_eval"
    ksrcdir="$linux_cv_k_source"
    AC_SUBST([ksrcdir])dnl
])# _LINUX_CHECK_KERNEL_SRCDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODVER
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODVER], [dnl
    AC_ARG_WITH([k-modvers],
	[AS_HELP_STRING([--with-k-modvers=MODVERS],
	    [kernel module symbol versions @<:@default=K-BUILD-DIR/Module.symvers@:>@])],
	[], [with_k_modvers=search])
    _BLD_FIND_FILE([kernel module ver], [linux_cv_k_modvers], [
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}/Module.symvers
	    ${rootdir}/usr/src/kernels/${kversion}/Module.symvers
	    ${kbuilddir}/Module.symvers-${kversion}
	    ${kbuilddir}/Module.symvers
	    ${rootdir}/boot/Module.symvers-${kversion}
	    ${rootdir}/boot/Module.symvers], [], [dnl
	if test ${with_k_modvers:-search} != no ; then
	    if test ${linux_cv_k_ko_modules:-no} != no ; then
		if test ${linux_cv_k_versions:-no} != no -a ${linux_cv_k_modversions:-no} != no ; then
		    _BLD_INSTALL_WARN([MODULE_SYMVERS], [
*** 
*** Configure could not find the module versions file for kernel version
*** "${kversion}".  The locations searched were:
***	    "$with_k_modvers$bld_search_path"
*** 
*** Perhaps you need to load the kernel development package (e.g.,
*** kernel-develop) for kernel version "${kversion}".
*** Kernel module versions cannot be checked without this file.  This
*** can cause problems later.  If this is not what you want, load the
*** kernel development package and rerun configure.  Try:
*** ], [
*** RHEL 5.x:   'yum install kernel-develop-"${kversion}"'], [
***
*** Repeat after loading the correct package, specifying the correct
*** file using configure argument --with-k-modvers=MODVER, or by
*** specifying the configure argument --without-k-modvers: continuing
*** under the assumption that --without-k-modvers was intended.
*** ])
		    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_k_modvers --without-k-modvers\""
		    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-k-modvers'"
		    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-k-modvers'"
		    with_k_modvers=no
		fi
	    fi
	fi], [dnl
	if test ${linux_cv_k_running:-no} != yes ; then
	    case "${linux_cv_k_modvers}" in
		(*/usr/src/kernels/*)
		    ;;
		(*/boot/*|*/usr/src/*|*/lib/modules/*)
		    case "$target_vendor" in
			(mandrake|mandriva|mageia)
			    ;;
			(redhat|oracle|springdale|puias|centos|whitebox|scientific|debian|ubuntu|suse|*)
			    AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
***
***	"$linux_cv_k_modvers"
***
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
				;;
		    esac
		    ;;
	    esac
	fi], [with_k_modvers])
    if test ${linux_cv_k_modvers:-no} = no ; then
	kmodver=
    else
	kmodver="$linux_cv_k_modvers"
    fi
    AM_CONDITIONAL([WITH_K_MODVERS], [test :"${with_k_modvers:-search}" != :no])dnl
    AC_SUBST([kmodver])dnl
])# _LINUX_CHECK_KERNEL_MODVER
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_VMLINUX
# -------------------------------------------------------------------------
# Some distros (like ArchLinux) are discarding the System.map file.
# However, ArchLinux keeps the vmlinux image kicking around and we can just
# do a nm -s on it when we need the system map file.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_VMLINUX], [dnl
    AC_ARG_WITH([k-vmlinux],
	[AS_HELP_STRING([--with-k-vmlinux=IMAGE],
	    [kernel vmlinux image @<:@default=K-BUILD-DIR/vmlinux@:>@])])
    _BLD_FIND_FILE([for kernel vmlinux image], [linux_cv_k_vmlinux], [
	    ${kbuilddir}/vmlinux-${kversion}.xz
	    ${kbuilddir}/vmlinux-${kversion}.lzma
	    ${kbuilddir}/vmlinux-${kversion}.bz2
	    ${kbuilddir}/vmlinux-${kversion}.gz
	    ${kbuilddir}/vmlinux-${kversion}
	    ${kbuilddir}/vmlinux.xz
	    ${kbuilddir}/vmlinux.lzma
	    ${kbuilddir}/vmlinux.bz2
	    ${kbuilddir}/vmlinux.gz
	    ${kbuilddir}/vmlinux
	    ${rootdir}/boot/vmlinux-${kversion}.xz
	    ${rootdir}/boot/vmlinux-${kversion}.lzma
	    ${rootdir}/boot/vmlinux-${kversion}.bz2
	    ${rootdir}/boot/vmlinux-${kversion}.gz
	    ${rootdir}/boot/vmlinux-${kversion}
	    ${rootdir}/boot/vmlinux.xz
	    ${rootdir}/boot/vmlinux.lzma
	    ${rootdir}/boot/vmlinux.bz2
	    ${rootdir}/boot/vmlinux.gz
	    ${rootdir}/boot/vmlinux])
    _kvmlinux="$linux_cv_k_vmlinux_eval"
    kvmlinux="$linux_cv_k_vmlinux"
    AC_SUBST([kvmlinux])dnl
])# _LINUX_CHECK_KERNEL_VMLINUX
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
	[AS_HELP_STRING([--with-k-sysmap=MAP],
	    [kernel system map @<:@default=K-BUILD-DIR/System.map@:>@])])
dnl
dnl There is a problem with using redhat /boot system maps.  Redhat builds multiple
dnl kernels of the same name for different architectures.  So, for example, there is a
dnl 2.4.21-15.EL kernel built for i386, another built for i586, and yet another built for
dnl i686.  These all have different system map files, but only one is installed on a
dnl system.
dnl
dnl There is also a problem here with debian, the deb has a different system map for each
dnl kernel architecture (i.e. System.map-2.4.18-1-686 and System.map-2.4.18-1-k7), but the
dnl version number is not included in the filename.  So, the version on one of these files
dnl could be 2.4.18-13.1, another could be 2.4.18-12 and they do not necessarily match the
dnl running kernel.  In fact, on debian, there is no way to tell which exact version of
dnl the kernel is running.  Ubuntu understandably has the same problem.
dnl
dnl SuSE also has this problem.  There is one more .n number in the kernel package version
dnl than there is in the version following the system map name (i.e. the kernel version
dnl and /boot/System.map-$(kversion) are different.  Multiple kernel packages can generate
dnl conflicting System map files; therefore, it is not possible to determine for which the
dnl running kernel belongs (because uname -r does not return the full kernel version
dnl either).  In any case, the System.map file is only unreliable as regards the address
dnl of symbols when the conflict is between same architecture kernels (e.g. for SuSE,
dnl Debain, Ubuntue), but not otherwise (i.e., for RedHat).  This is because the header
dnl files are the same and the kABI is the same, it is just the addresses that might
dnl change because it is a different compilation.  For the running kernel use
dnl /proc/kallsyms for detection and addresses instead.
dnl
    _BLD_FIND_FILE([for kernel system map], [linux_cv_k_sysmap], [
	    ${kbuilddir}/System.map-generic-${kversion}.xz
	    ${kbuilddir}/System.map-generic-${kversion}.lzma
	    ${kbuilddir}/System.map-generic-${kversion}.bz2
	    ${kbuilddir}/System.map-generic-${kversion}.gz
	    ${kbuilddir}/System.map-generic-${kversion}
	    ${kbuilddir}/System.map-generic.xz
	    ${kbuilddir}/System.map-generic.lzma
	    ${kbuilddir}/System.map-generic.bz2
	    ${kbuilddir}/System.map-generic.gz
	    ${kbuilddir}/System.map-generic
	    ${kbuilddir}/System.map-huge-${kversion}.xz
	    ${kbuilddir}/System.map-huge-${kversion}.lzma
	    ${kbuilddir}/System.map-huge-${kversion}.bz2
	    ${kbuilddir}/System.map-huge-${kversion}.gz
	    ${kbuilddir}/System.map-huge-${kversion}
	    ${kbuilddir}/System.map-huge.xz
	    ${kbuilddir}/System.map-huge.lzma
	    ${kbuilddir}/System.map-huge.bz2
	    ${kbuilddir}/System.map-huge.gz
	    ${kbuilddir}/System.map-huge
	    ${kbuilddir}/System.map-${kversion}.xz
	    ${kbuilddir}/System.map-${kversion}.lzma
	    ${kbuilddir}/System.map-${kversion}.bz2
	    ${kbuilddir}/System.map-${kversion}.gz
	    ${kbuilddir}/System.map-${kversion}
	    ${kbuilddir}/System.map.xz
	    ${kbuilddir}/System.map.lzma
	    ${kbuilddir}/System.map.bz2
	    ${kbuilddir}/System.map.gz
	    ${kbuilddir}/System.map
	    ${rootdir}/boot/System.map-generic-${kversion}.xz
	    ${rootdir}/boot/System.map-generic-${kversion}.lzma
	    ${rootdir}/boot/System.map-generic-${kversion}.bz2
	    ${rootdir}/boot/System.map-generic-${kversion}.gz
	    ${rootdir}/boot/System.map-generic-${kversion}
	    ${rootdir}/boot/System.map-generic.xz
	    ${rootdir}/boot/System.map-generic.lzma
	    ${rootdir}/boot/System.map-generic.bz2
	    ${rootdir}/boot/System.map-generic.gz
	    ${rootdir}/boot/System.map-generic
	    ${rootdir}/boot/System.map-huge-${kversion}.xz
	    ${rootdir}/boot/System.map-huge-${kversion}.lzma
	    ${rootdir}/boot/System.map-huge-${kversion}.bz2
	    ${rootdir}/boot/System.map-huge-${kversion}.gz
	    ${rootdir}/boot/System.map-huge-${kversion}
	    ${rootdir}/boot/System.map-huge.xz
	    ${rootdir}/boot/System.map-huge.lzma
	    ${rootdir}/boot/System.map-huge.bz2
	    ${rootdir}/boot/System.map-huge.gz
	    ${rootdir}/boot/System.map-huge
	    ${rootdir}/boot/System.map-${kversion}.xz
	    ${rootdir}/boot/System.map-${kversion}.lzma
	    ${rootdir}/boot/System.map-${kversion}.bz2
	    ${rootdir}/boot/System.map-${kversion}.gz
	    ${rootdir}/boot/System.map-${kversion}
	    ${rootdir}/boot/System.map.xz
	    ${rootdir}/boot/System.map.lzma
	    ${rootdir}/boot/System.map.bz2
	    ${rootdir}/boot/System.map.gz
	    ${rootdir}/boot/System.map], [], [dnl
	if test -z "$kvmlinux" ; then
	    AC_MSG_ERROR([
*** 
*** Configure could not find the system map file for kernel version
*** "$kversion".  The locations searched were:
***	    "$with_k_sysmap"
***	    "$bld_search_path"
*** 
*** Please specify the absolute location of your kernel system map
*** file with option --with-k-sysmap before repeating.
*** ])
	fi],[dnl
	if test -z "$kvmlinux" ; then
	    if test :$linux_cv_k_running != :yes
	    then
		case "$linux_cv_k_sysmap_eval" in
		    (*/usr/src/kernels/*)
			;;
		    (*/boot/*|*/usr/src/*|*/lib/modules/*)
			case "$target_vendor" in
			    (mandrake|mandriva|mageia)
dnl
dnl				Mandrakelinux blends the debian architecture name in the kernel
dnl				image name approach with the Redhat kernel version number in the
dnl				kernel image name approach to yeild reliable system map files.
dnl
				;;
			    (redhat|oracle|springdale|puias|centos|whitebox|scientific|debian|ubuntu|suse|*)
dnl
dnl				Unfortunately the redhat system map files are unreliable because the
dnl				are not unique for each architecture.  The system map file has to be
dnl				checked against the architecture for which we are building.
dnl
dnl				Unfortunately the debian system map files are unreliable because
dnl				they are not unique by kernel version.  The system map file has to
dnl				be checked against the kernel version for which we are building.
dnl
dnl				I don't really know how SuSE or others fair in this situation yet,
dnl				but I assume it is like the rest.
dnl
				AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
*** 
***	"$linux_cv_k_sysmap_eval"
*** 
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
				;;
			esac
			;;
		esac
	    fi
	fi],[with_k_sysmap])
    _ksysmap="$linux_cv_k_sysmap_eval"
    ksysmap="$linux_cv_k_sysmap"
    AC_SUBST([ksysmap])dnl
])# _LINUX_CHECK_KERNEL_SYSMAP
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_KSYMS
# -------------------------------------------------------------------------
# This is only used for grepping for exported symbols.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_KSYMS], [dnl
    AC_CACHE_CHECK([for kernel /proc/ksyms], [linux_cv_k_ksyms], [dnl
	linux_cv_k_ksyms=no
	if test :"${linux_cv_k_running:-no}" = :yes -a -r /proc/ksyms
	then
	    linux_cv_k_ksyms=yes
	fi ])
    ksyms=
    if test :"${linux_cv_k_ksyms:-no}" = :yes
    then
	ksyms="/proc/ksyms"
    fi
    AC_SUBST([ksyms])dnl
])# _LINUX_CHECK_KERNEL_KSYMS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_KALLSYMS
# -------------------------------------------------------------------------
# This is only used for grepping for exported symbols and for use by modpost
# to determine the __crc_ values for exported kernel symbols.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_KALLSYMS], [dnl
    AC_CACHE_CHECK([for kernel /proc/kallsyms], [linux_cv_k_kallsyms], [dnl
	linux_cv_k_kallsyms=no
	if test :"${linux_cv_k_running:-no}" = :yes -a -r /proc/kallsyms
	then
	    linux_cv_k_kallsyms=yes
	fi ])
    kallsyms=
    if test :"${linux_cv_k_kallsyms:-no}" = :yes
    then
	kallsyms="/proc/kallsyms"
    fi
    AC_SUBST([kallsyms])dnl
])# _LINUX_CHECK_KERNEL_KALLSYMS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SYMVERS
# -------------------------------------------------------------------------
# The linux kernel symvers file is used during non-rpm install to tell
# where to add our symbol versions.  This is so that weak-updates can be
# performed properly.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SYMVERS], [dnl
    AC_ARG_WITH([k-symvers],
	[AS_HELP_STRING([--with-k-symvers=SYMVERS],
	    [kernel symbol versions @<:@default=/boot/symvers-KVERSION.gz@:>@])],
	[], [with_k_symvers=search])
    _BLD_FIND_FILE([kernel symvers], [linux_cv_k_symvers], [
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}/symvers-${kversion}.gz
	    ${kbuildir}/symvers-${kversion}.gz
	    ${rootdir}/boot/symvers-${kversion}.gz], [], [dnl
	if test ${with_k_symvers:-search} != no ; then
	    if test ${linux_cv_k_ko_modules:-no} != no ; then
		if test ${linux_cv_k_versions:-no} != no -a ${linux_cv_k_modversions:-no} != no ; then
		    case "$target_vendor" in
			# debian based systems do not have this file
			(debian|ubuntu|mint|arch|slackware|salix)
			    ;;
			(oracle|springdale|puias|centos|lineox|whitebox|scientific|redhat|suse|*)
			    tmp_fn="symvers-${kversion}.gz"
			    tmp_fn=`echo "$tmp_fn" | sed -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
			    tmp_fn=`echo "$tmp_fn" | sed -e 's,[[^A-Z0-9_]],_,g'`
			    _BLD_INSTALL_WARN([$tmp_fn], [
*** 
*** Configure could not find the symbol versions file for kernel version
*** "${kversion}".  The locations searched were:
***	    "$with_k_symvers$bld_search_path"
*** 
*** Perhaps you need to load the kernel development package (e.g.,
*** kernel-develop) for kernel version "${kversion}".
*** Kernel symbol sets cannot be checked without this file.  This can
*** cause problems later.  If this is not what you want, load the kernel
*** development package and rerun configure.  Try:
*** ], [
*** RHEL 5.x:   'yum install kernel-develop-"${kversion}"'], [
***
*** Repeat after loading the correct package, specifying the correct
*** file using configure argument --with-k-symvers=SYMVERS, or by
*** specifying the configure argument --without-k-symvers: continuing
*** under the assumption that --without-k-symvers was intended.
*** ])
			    ;;
		    esac
		    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_k_symvers --without-k-symvers\""
		    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-k-symvers'"
		    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-k-symvers'"
		    with_k_symvers=no
		fi
	    fi
	fi], [dnl
	if test ${linux_cv_k_running:-no} != yes ; then
	    case "$linux_cv_k_symvers" in
		(*/usr/src/kernels/*)
		    ;;
		(*/boot/*|*/usr/src/*|*/lib/modules/*)
		    case "$target_vendor" in
			(mandrake|mandriva|mageia)
			    ;;
			(redhat|oracle|springdale|puias|centos|whitebox|scientific|debian|ubuntu|suse|*)
			    AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
***
***	"$linux_cv_k_symvers"
***
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
			    ;;
		    esac
		    ;;
	    esac
	fi], [with_k_symvers])
    if test ${linux_cv_k_symvers:-no} = no ; then
	ksymvers=
    else
	ksymvers="$linux_cv_k_symvers"
    fi
    AM_CONDITIONAL([WITH_K_SYMVERS], [test :"${with_k_symvers:-search}" != :no])dnl
    AC_SUBST([ksymvers])dnl
])# _LINUX_CHECK_KERNEL_SYMVERS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODABI
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODABI], [dnl
    AC_ARG_WITH([k-modabi],
	[AS_HELP_STRING([--with-k-modabi=MAP],
	    [kernel module symbols in kabi @<:@default=K-BUILD-DIR/Module.kabi@:>@])],
	[], [with_k_modabi=search])
    _BLD_FIND_FILE([kernel module abi], [linux_cv_k_modabi], [
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}/Module.kabi
	    ${rootdir}/usr/src/kernels/${kversion}/Module.kabi
	    ${kbuilddir}/Module.kabi-${kversion}
	    ${kbuilddir}/Module.kabi
	    ${rootdir}/boot/Module.kabi-${kversion}
	    ${rootdir}/boot/Module.kabi], [], [dnl
	if test ${with_k_modabi:-search} != no ; then
	    if test ${linux_cv_k_ko_modules:-no} != no ; then
		if test ${linux_cv_k_versions:-no} != no -a ${linux_cv_k_modversions:-no} != no ; then
		    case "$target_vendor:$target_edition" in
			(redhat:[[45]]|oracle:[[45]]|puias:[[45]]|centos:[[45]]|whitebox:[[45]]|scientific:[[45]])
			    _BLD_INSTALL_WARN([MODULE_KABI], [
*** 
*** Configure could not find the module kABI file for kernel version
*** "$kversion".  The locations searched were:
***	    "$with_k_modabi"
***	    "$bld_search_path"
***
*** Perhaps you need to load the kernel development package (e.g.,
*** kernel-develop) for kernel version "${kversion}".
*** Kernel symbols used cannot be checked against the abi without this
*** file.  This can cause problems later.  If this is not what you want,
*** load the kernel development package and rerun configure.  Try:
*** ], [
*** RHEL 5.x:   'yum install kernel-develop-${kversion}'], [
***
*** Repeat after loading the correct package, specifying the correct
*** file using configure argument --with-k-modabi=MODABI, or by
*** specifying the configure argument --without-k-modabi: continuing
*** under the assumption that --without-k-modabi was intended.
*** ])
			    ;;
		    esac
		    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_k_modabi --without-k-modabi\""
		    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-k-modabi'"
		    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-k-modabi'"
		    with_k_modabi=no
		fi
	    fi
	fi], [dnl
	if test ${linux_cv_k_running:-no} != yes ; then
	    case "$linux_cv_k_modabi" in
		(*/usr/src/kernels/*)
		    ;;
		(*/boot/*|*/usr/src/*|*/lib/modules/*)
		    case "$target_vendor" in
			(mandrake|mandriva|mageia)
			    ;;
			(redhat|oracle|springdale|puias|centos|whitebox|scientific|debian|ubuntu|suse|*)
			    AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
*** 
***	"$linux_cv_k_modabi"
*** 
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
			    ;;
		    esac
		    ;;
	    esac
	fi], [with_k_modabi])
    if test ${linux_cv_k_modabi:-no} = no ; then
	kmodabi=
    else
	kmodabi="$linux_cv_k_modabi"
    fi
    AM_CONDITIONAL([WITH_K_MODABI], [test :"${with_k_modabi:-search}" != :no])dnl
    AC_SUBST([kmodabi])dnl
])# _LINUX_CHECK_KERNEL_MODABI
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SYMSETS
# -------------------------------------------------------------------------
# The linux kernel symsets file is used during non-rpm install to tell
# where to add our symbol sets.  This is so that kbuilds can be performed
# properly.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SYMSETS], [dnl
    AC_ARG_WITH([k-symsets],
	[AS_HELP_STRING([--with-k-symsets=SYMVERS],
	    [kernel symbol versions @<:@default=/boot/symsets-KVERSION.tar.gz@:>@])],
	[], [with_k_symsets=search])
    _BLD_FIND_FILE([kernel symsets], [linux_cv_k_symsets], [
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}/symsets-${kversion}.tar.gz
	    ${rootdir}/usr/src/kernels/${kversion}/symsets-${kversion}.tar.gz
	    ${kbuildir}/symsets-${kversion}.tar.gz
	    ${rootdir}/boot/symsets-${kversion}.tar.gz], [], [dnl
	if test ${with_k_symsets:-search} != no ; then
	    if test ${linux_cv_k_ko_modules:-no} != no ; then
		if test ${linux_cv_k_versions:-no} != no -a ${linux_cv_k_modversions:-no} != no ; then
		    case "$target_vendor:$target_edition" in
			(oracle:[[345]]:puias:[[345]]|centos:[[345]]|lineox:[[345]]|whitebox:[[345]]|scientific:[[345]]|redhat:[[345]]|suse:9*|suse:1[[012]]*)
			    tmp_fn="symsets-${kversion}.tar.gz"
			    tmp_fn=`echo "$tmp_fn" | sed -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
			    tmp_fn=`echo "$tmp_fn" | sed -e 's,[[^A-Z0-9_]],_,g'`
			    _BLD_INSTALL_WARN([$tmp_fn], [
*** 
*** Configure could not find the symbol sets file for kernel version
*** "${kversion}".  The locations searched were:
***	    "$with_k_symsets$bld_search_path"
*** 
*** Perhaps you need to load the kernel development package (e.g.,
*** kernel-develop) for kernel version "${kversion}".
*** Kernel symbol sets cannot be checked without this file.  This can
*** cause problems later.  If this is not what you want, load the kernel
*** development package and rerun configure.  Try:
*** ], [
*** RHEL 5.x:   'yum install kernel-develop-"${kversion}"'], [
***
*** Repeat after loading the correct package, specifying the correct
*** file using configure argument --with-k-symsets=SYMSETS, or by
*** specifying the configure argument --without-k-symsets: continuing
*** under the assumption that --without-k-symsets was intended.
*** ])
			    ;;
			# debian and some rpm based systems do not have this file
			(debian:*|ubuntu:*|mint:*|mandrake:*|mandriva:*|mageia:*|*)
			    ;;
		    esac
		    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_k_symsets --without-k-symsets\""
		    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-k-symsets'"
		    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-k-symsets'"
		    with_k_symsets=no
		fi
	    fi
	fi], [dnl
	if test ${linux_cv_k_running:-no} != yes ; then
	    case "${linux_cv_k_symsets}" in
		(*/usr/src/kernels/*)
		    ;;
		(*/boot/*|*/usr/src/*|*/lib/modules/*)
		    case "$target_vendor" in
			(mandrake|mandriva|mageia)
			    ;;
			(redhat|oracle|springdale|puias|centos|whitebox|scientific|debian|ubuntu|suse|*)
			    AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
***
***	"$linux_cv_k_symsets"
***
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
				;;
		    esac
		    ;;
	    esac
	fi], [with_k_symsets])
    if test ${linux_cv_k_symsets:-no} = no ; then
	ksymsets=
    else
	ksymsets="$linux_cv_k_symsets"
    fi
    AM_CONDITIONAL([WITH_K_SYMSETS], [test :"${with_k_symsets:-search}" != :no])dnl
    AC_SUBST([ksymsets])dnl
])# _LINUX_CHECK_KERNEL_SYMSETS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_KABI
# -------------------------------------------------------------------------
# Starting with RHEL6.0, RHEL kernels publish a separate kABI whitelist that may
# be used to enforce non-loading of kernel module packages that use symbols not
# provided in the kABI whitelist.  The directory is under /lib/modules/kabi and
# contains files such as: kabi_whitelist_$(arch).  The files contain a .ini
# style header [rhel6_$(arch)_whitelist] and then a list of tab indented symbol
# names.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_KABI], [dnl
    AC_ARG_WITH([kabi-whitelist],
	[AS_HELP_STRING([--with-kabi-whitelist=WHITELIST],
			[kABI whitelist file @<:@default=/lib/modules/kabi/kabi_whitelist_ARCH@:>@])],
	[], [with_kabi_whitelist=search])
    _BLD_FIND_FILE([kernel kABI whitelist], [linux_cv_kabi_whitelist], [
	    ${rootdir}/usr/lib/modules/kabi/kabi_whitelist_${karch}
	    ${rootdir}/usr/lib/modules/kabi-current/kabi_whitelist_${karch}
	    ${rootdir}/lib/modules/kabi/kabi_whitelist_${karch}
	    ${rootdir}/lib/modules/kabi-current/kabi_whitelist_${karch}
    ], [], [dnl
	if test ${with_kabi_whitelist:-search} != no ; then
	    case "$target_vendor:$target_edition" in
		(redhat:[[67]]*|oracle:[[67]]*|springdale:[[67]]*|puias:[[67]]*|centos:[[67]]*|scientific:[[67]]*)
		    tmp_fn="kabi_whitelist_${karch}"
		    tmp_fn=`echo "$tmp_fn" | sed -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
		    tmp_fn=`echo "$tmp_fn" | sed -e 's,[[^A-Z0-9_]],_,g'`
		    _BLD_INSTALL_WARN([$tmp_fn], [
*** 
*** Configure cannot find the kABI whitelist:
***
***	/usr/lib/modules/kabi/kabi_whitelist_${karch}
***	/usr/lib/modules/kabi-current/kabi_whitelist_${karch}
***	/lib/modules/kabi/kabi_whitelist_${karch}
***	/lib/modules/kabi-current/kabi_whitelist_${karch}
***
*** Perhaps you need to load the kABI whitelist package (e.g.,
*** kabi-whitelist).  Kernel symbols cannot be checked against the
*** whitelist without this file.  This can cause problems later.
*** If this is not what you want, load the kABI whitelist package
*** and rerun configure.  Try:
*** ], [
*** RHEL [[67]].x:   'yum install kabi-whitelist'
*** CentOS [[67]].x: 'yum install kabi-whitelist'
*** SL [[67]].x:     'yum install kabi-whitelist'
*** OLS [[67]].x:    'yum install kabi-whitelist'
*** PUIAS [[67]].x:  (copy from RHEL or other clone)], [
***
*** Repeat after loading the correct package, specifying the correct
*** file using configure argument --with-kabi-whitelist=WHITELIST,
*** or by specifying the configure argument --without-kabi-whitelist:
*** continuing under the assumption that --without-kabi-whitelist was
*** intended.
*** ])
		    ;;
	    esac
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_kabi_whitelist --without-kabi-whitelist\""
	    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-kabi-whitelist'"
	    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-kabi-whitelist'"
	    with_kabi_whitelist=no
	fi], [], [with_kabi_whitelist])
    if test ${linux_cv_kabi_whitelist:-no} = no ; then
	kabiwhitelist=
    else
	kabiwhitelist="$linux_cv_kabi_whitelist"
    fi
    AM_CONDITIONAL([WITH_KABI_WHITELIST], [test :"${with_kabi_whitelist:-search}" != :no])dnl
    AC_SUBST([kabiwhitelist])dnl
])# _LINUX_CHECK_KERNEL_KABI
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MINORBITS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MINORBITS], [dnl
    AC_CACHE_CHECK([for kernel minor device number bits], [linux_cv_minorbits], [dnl
dnl
dnl Just grep linux/kdev_t.h for #define MINORBITS.
dnl
	AC_MSG_RESULT([searching...])
	linux_cv_minorbits=
	linux_dirs="${_khdrdir} ${_kbuilddir} ${_ksrcdir}"
	for linux_dir in $linux_dirs
	do
	    AC_MSG_CHECKING([for kernel minor device number bits...  $linux_dir])
	    linux_file="$linux_dir/include/linux/kdev_t.h"
	    if test -f $linux_file
	    then
		linux_line="`egrep '^#[[:space:]]*define[[:space:]][[:space:]]*MINORBITS[[:space:]]' $linux_file | head -1`"
		linux_rslt="`echo \"$linux_line\" | sed -e 's|^#[[:space:]]*define[[:space:]][[:space:]]*MINORBITS[[:space:]][[:space:]]*\([1-9][0-9]*\)[[:space:]]*$|\1|'`"
		if test "${linux_rslt:-0}" -ge 8 -a "${linux_rslt:-0}" -le 20
		then
		    linux_cv_minorbits="$linux_rslt"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([found])
	    else
		AC_MSG_RESULT([no])
	    fi
	done
	AC_MSG_CHECKING([for kernel minor device number bits])
    ])
    if test -z "$linux_cv_minorbits"
    then
	AC_MSG_WARN([
*** 
*** Configure could not determine the number of minor bits in a minor
*** device number for your kernel.  Configure is assuming that the
*** kernel only has 8-bits per minor device number.  This will cause
*** problems later.
*** ])
    fi
])# _LINUX_CHECK_KERNEL_MINORBITS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_COMPILER
# -------------------------------------------------------------------------
# Ok, here we have headers and can finally check the compiler against the
# compiler used to compile the kernel.  For a running kernel we can check
# against /proc/version.  For non-running kernel we need to grab the version
# out of the header files linux/compile.h
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_COMPILER], [dnl
    AC_CACHE_CHECK([for kernel compiler], [linux_cv_k_compiler_match], [dnl
	linux_cv_k_compiler=
	if test :"$linux_cv_k_running" = :yes
	then
dnl
dnl Ubuntu stuck some more paretheses into their /proc/version string.
dnl Instead of keying off of the last parentheses in the string, use the key
dnl string containing two parentheses: this is common to all.
dnl
dnl Debian added a blank between the last and extra parentheses.
dnl
	    linux_cv_k_compiler=`cat /proc/version | sed -e 's,^.*(gcc version,gcc version,;s,)).*[$],),;s,) ).*[$],) ,' 2>/dev/null`
	else
dnl
dnl	    not all distros leave this hanging around
dnl
	    linux_dirs="${_khdrdir} ${_kbuilddir} ${_ksrcdir}"
	    for linux_dir in $linux_dirs
	    do
		for linux_file in $linux_dir/include/{linux,generated}/compile.h
		do
		    if test -f $linux_file
		    then
			linux_cv_k_compiler=`grep LINUX_COMPILER $linux_file | sed -e 's,^.*gcc version,gcc version,;s,"[[^"]]*[$],,' 2>/dev/null`
			break 2
		    fi
		done
		linux_dir="$_kmoduledir/kernel/arch"
		if test -d $linux_dir
		then
		    linux_mods=`find $linux_dir -type f -name '*'$kext$kzip 2>/dev/null`
		    for linux_mod in $linux_mods
		    do
			if test -f $linux_mod
			then
			    if echo "$linux_mod" | grep '\.gz[$]' >/dev/null 2>&1
			    then
				linux_com=`gunzip -dc $linux_mod | strings | grep '^GCC: (GNU) ' | head -1 2>/dev/null`
			    else
				linux_com=`cat $linux_mod | strings | grep '^GCC: (GNU) '  | head -1 2>/dev/null`
			    fi
			    if echo "$linux_com" | grep '^GCC: (GNU) ' >/dev/null 2>&1
			    then
				linux_cv_k_compiler=`echo "$linux_com" | sed -e 's,^GCC: (GNU) ,gcc version ,'`
				break
			    fi
			fi
		    done
		    if test :"${linux_cv_k_compiler:+set}" = :set
		    then
			break
		    fi
		fi
	    done
	fi
	linux_cv_compiler=`$KCC -v 2>&1 | grep 'gcc version'`
	if test -z "$linux_cv_k_compiler" -o -z "$linux_cv_compiler"
	then
	    linux_cv_k_compiler_match=unknown
	    linux_cv_k_compiler_vmatch=unknown
	else
	    if test "$linux_cv_k_compiler" = "$linux_cv_compiler"
	    then
		linux_cv_k_compiler_match=yes
		linux_cv_k_compiler_vmatch=yes
	    else
		linux_cv_k_compiler_match=no
		linux_c1=`echo "$linux_cv_k_compiler" | sed -e 's,gcc version ,,;s,[[[:space:]]].*[$],,'`
		case "$linux_c1" in ([[34]].*.*) linux_c1=`echo $linux_c1 | sed -e 's,\.[[^\.]]*[$],,'` ;; esac
		linux_c2=`echo "$linux_cv_compiler" | sed -e 's,gcc version ,,;s,[[[:space:]]].*[$],,'`
		case "$linux_c2" in ([[34]].*.*) linux_c2=`echo $linux_c2 | sed -e 's,\.[[^\.]]*[$],,'` ;; esac
		if test "$linux_c1" = "$linux_c2"
		then
		    linux_cv_k_compiler_vmatch=yes
		else
		    linux_cv_k_compiler_vmatch=no
		fi
	    fi
	fi
    ])
    case :"$linux_cv_k_compiler_match" in
	(:yes)
	    ;;
	(:no)
	    case :"$linux_cv_k_compiler_vmatch" in
		(:no)
	    AC_MSG_ERROR([
*** 
*** The kernel compiler was:
***   "$linux_cv_k_compiler",
*** and the current compiler is:
***   "$linux_cv_compiler".
*** 
*** These compilers do not match, not even in version.
*** This will cause real problems later.  Cannot proceed.
*** Specify the correct compiler with the KCC environment
*** variable when retrying.
*** ])
		    ;;
		(:yes)
	    AC_MSG_WARN([
*** 
*** The kernel compiler was:
***   "$linux_cv_k_compiler",
*** and the current compiler is:
***   "$linux_cv_compiler".
*** 
*** These compilers do not match.
*** This will cause problems later.
*** ])
		    ;;
	    esac
	    ;;
	(:unknown)
	    AC_MSG_WARN([
*** 
*** The kernel compiler was:
***   "$linux_cv_k_compiler",
*** and the current compiler is:
***   "$linux_cv_compiler".
*** 
*** Configure cannot determine whether the compilers match or do not match.
*** This will cause problems later.
*** ])
	    ;;
    esac
])# _LINUX_CHECK_KERNEL_COMPILER
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_ARCHDIR
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_ARCHDIR], [dnl
    AC_ARG_WITH([k-archdir],
	[AS_HELP_STRING([--with-k-archdir=DIR],
	    [kernel architecture directory @<:@default=K-BUILD-DIR/arch@:>@])])
    _BLD_FIND_DIR([for kernel arch directory], [linux_cv_k_archdir], [
	    ${khdrdir}/arch
	    ${kbuilddir}/arch
	    ${ksrcdir}/arch], [], [${khdrdir}/arch], [dnl
		AC_MSG_ERROR([
*** 
*** Configure cannot find the kernel architecture directory.  The
*** directories searched were:
***	"$with_k_archdir"
***	"$bld_search_path"
***
*** Specify the absolute location of the kernel architecture directory
*** with the --with-k-archdir option to configure before attempting
*** again.
*** ])], [], [with_k_archdir])
])# _LINUX_CHECK_KERNEL_ARCHDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_ARCH
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_ARCH], [dnl
    AC_CACHE_CHECK([for kernel machine base], [linux_cv_k_mach], [dnl
	case "$karch" in
	    (alpha*)		linux_cv_k_mach="alpha"	  ;;
	    (arm*|sa110)	linux_cv_k_mach="arm"	  ;;
	    (cris*)		linux_cv_k_mach="cris"	  ;;
	    (i?86*|k6*|athlon*)	linux_cv_k_mach="i386"	  ;;
	    (ia64)		linux_cv_k_mach="ia64"	  ;;
	    (m68*)		linux_cv_k_mach="m68k"	  ;;
	    (mips64*)		linux_cv_k_mach="mips64"  ;;
	    (mips*)		linux_cv_k_mach="mips"	  ;;
	    (hppa*)		linux_cv_k_mach="parisc"  ;;
	    (ppc*|powerpc*)	linux_cv_k_mach="ppc"	  ;;
	    (s390x*)		linux_cv_k_mach="s390x"	  ;;
	    (s390*)		linux_cv_k_mach="s390"	  ;;
	    (sh*)		linux_cv_k_mach="sh"	  ;;
	    (sparc64*|sun4u)	linux_cv_k_mach="sparc64" ;;
	    (sparc*)		linux_cv_k_mach="sparc"	  ;;
	    (*)			linux_cv_k_mach="$karch"  ;;
	esac
    ])
    kmach="$linux_cv_k_mach"
    AC_SUBST([kmach])dnl
dnl
    AC_CACHE_CHECK([for kernel source arch], [linux_cv_k_srcarch], [dnl
	case "$linux_cv_k_mach" in
	    (i386|x86_64)	linux_cv_k_srcarch="x86"		;;
	    (sparc64)		linux_cv_k_srcarch="sparc"		;;
	    (sh64)		linux_cv_k_srcarch="sh"			;;
	    (*)			linux_cv_k_srcarch="$linux_cv_k_mach"	;;
	esac
    ])
    ksrcarch="$linux_cv_k_srcarch"
    AC_SUBST([ksrcarch])dnl
dnl
    AC_CACHE_CHECK([for kernel header arch], [linux_cv_k_hdrarch], [dnl
	case "$linux_cv_k_srcarch" in
	    (m68knommu)		linux_cv_k_hdrarch="m68k"		 ;;
	    (*)			linux_cv_k_hdrarch="$linux_cv_k_srcarch" ;;
	esac
    ])
    khdrarch="$linux_cv_k_hdrarch"
    AC_SUBST([khdrarch])
])# _LINUX_CHECK_KERNEL_ARCH
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_ASMDIR
# -------------------------------------------------------------------------
# The asm include directory is a long-standing linux kernel directory.  To
# support multiple architectures, include/asm is linked to as specific asm
# directory that used to be in include/asm-${arch} for 2.4 kernels.  The
# makefiles genered include/asm which was a symbolic link to
# include/asm-${arch}.
#
# For 2.6 kernels, additional asm includes where placed in
# arch/${hdr-arch}/include/asm.  The include part of this directory,
# arch/${hdr-arch}/include, was included directly as an -I option to gcc.
#
# directory was moved to the include/arch/
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_ASMDIR], [dnl
    AC_ARG_WITH([k-asm],
	[AS_HELP_STRING([--with-k-asm=DIR],
	    [kernel asm directory @<:@default=search@:>@])])
dnl 
dnl These have moved around a bit.  Normally these headers were in the
dnl single source directory, then the build directory, then they got moved
dnl to a common header directory (all except asm-offsets.h), then they
dnl moved back to the build directory under arch/${ksrcarch}/include/asm.
dnl
dnl Note that as of somewhere before 2.6.32 all of the asm includes have
dnl moved to the arch/${khdrarch}/include/asm directory with the exception
dnl of the asm-offsets.h file.
dnl
    _BLD_FIND_DIR([for kernel asm directory], [linux_cv_k_asmdir], [
	    ${khdrdir}/include/asm-${kmach}
	    ${ksrcdir}/include/asm-${kmach}
	    ${kbuilddir}/include/asm-${kmach}
	    ${ksrcdir}/include/asm-${ksrcarch}
	    ${khdrdir}/include/asm-${ksrcarch}
	    ${kbuilddir}/include/asm-${ksrcarch}
	    ${ksrcdir}/include/asm-${khdrarch}
	    ${khdrdir}/include/asm-${khdrarch}
	    ${kbuilddir}/include/asm-${khdrarch}], [asm-offsets.h],
	    [$ksrcdir/include/asm], [], [], [with_k_asm])
    _kasmdir="$linux_cv_k_asmdir_eval"
    kasmdir="$linux_cv_k_asmdir"
    AC_SUBST([kasmdir])dnl
dnl
dnl	Some notes about include2: Because the source directory containing
dnl	architecture-specific asm includes might not be configured properly
dnl	for the specific architecture (particularly when multiple
dnl	architectures can be built such as x86 and amd64)
dnl
dnl	This sets up a local include2 directory with a symbolic link from
dnl	asm to the machine specific asm include directory.  This is so that
dnl	neither the build nor source directory need be writeen.  This is
dnl	only for Linux 2.6 kbuild kernels so that we can strip flags from
dnl	the kernel makefiles.  Without it, recent Makefiles will attempt to
dnl	create one.
dnl
	test -d include2 || mkdir include2
	ln -snf "$_kasmdir" include2/asm
])# _LINUX_CHECK_KERNEL_ASMDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MACHDIR
# -------------------------------------------------------------------------
# This is the architecture-specific subdirectory under arc.  These
# subdirectories have existsted since early 2.4 kernels and have persisted
# throughout recent 2.6 kernels.  However, the precise names have changed
# somewhat over the years, so we now search for an existing directory.
# Note also that this directory ${kmachdir}/include must be one of the -I
# includes on the gcc command line for 2.6 kernels.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MACHDIR], [dnl
    AC_ARG_WITH([k-machdir],
	[AS_HELP_STRING([--with-k-machdir=DIR],
	    [kernel machine directory @<:@default=K-ARCHDIR/ARCH@:>@])])
dnl 
dnl Just having the directory exist will do.  What goes in this directory
dnl has changed alot.  It is one of the places that we can search for
dnl defconfig, but it is essential to recent 2.6 kernel build and yet
dnl recently does not contain defconfig.
dnl 
    _BLD_FIND_DIR([for kernel mach directory], [linux_cv_k_machdir], [
	    ${kbuilddir}/arch/${kmach}
	    ${ksrcdir}/arch/${kmach}
	    ${kbuilddir}/arch/${ksrcarch}
	    ${ksrcdir}/arch/${ksrcarch}
	    ${kbuilddir}/arch/${khdrarch}
	    ${ksrcdir}/arch/${khdrarch}], [], [$ksrcdir/arch/$target_cpu],
	    [], [], [with_k_machdir])
    kmachdir="$linux_cv_k_machdir"
    AC_SUBST([kmachdir])dnl
])# _LINUX_CHECK_KERNEL_MACHDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_DOT_CONFIG
# -------------------------------------------------------------------------
# Find the kernel .config file so that we can use it to generate CFLAGS.
#
# This information can often be obtained from defconfig.  defconfig often
# exists in the arch directory ${kmachdir} but in some installed kernel
# headers can exixt in ${kbuilddir}/include/config.  It also usually exists
# in ${kbuilddir}/.config or ${ksrcdir}/.config.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_DOT_CONFIG], [dnl
    AC_ARG_WITH([k-config],
	[AS_HELP_STRING([--with-k-config=CONFIG],
	    [kernel configuration @<:@default=BOOT/config-K-RELEASE@:>@])])
dnl
dnl If we are configuring for the running kernel, we can check the /boot directories.
dnl Otherwise this is dangerous.  Redhat puts different configuration files in the /boot
dnl directory depending upon which architecture of kernel (with the same name) is being
dnl booted.  In these other cases, the kernel configuration needs to be obtained from the
dnl kernel source package.  /usr/src/linux-2.4/configs/*.config for RedHat/Fedora/EL3 and
dnl /usr/src/linux-2.4/arch/MACHDIR/defconfig-* for Mandrake.  Debian and Suse do not
dnl build multiple kernels from the same source package and in their case the file will be
dnl in /usr/src/kernel-headers-kversion/.config and /usr/src/linux-2.4/.config
dnl respectively.
dnl
    if test :"$linux_cv_k_running" = :yes
    then
	eval "k_config_search_path=\"
	    ${ksrcdir}/configs/kernel-${knumber}-${karch}${kboot:+-}${kboot}.config
	    ${kmachdir:+${kboot:+${kmachdir}/defconfig${kboot:+-}${kboot}}}
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}/.config
	    ${rootdir}/usr/src/kernels/${kversion}/.config
	    ${rootdir}/usr/src/kernel-headers-${kversion}/.config
	    ${rootdir}/usr/src/linux-headers-${kversion}/.config
	    ${rootdir}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}/.config
	    ${rootdir}/usr/src/linux-obj/.config
	    ${rootdir}/boot/config-${kversion}
	    ${rootdir}/boot/config
	    ${kbuilddir}/.config
	    ${kmachdir:+${kboot:-${kmachdir}/defconfig${kboot:+-}${kboot}}}
	    ${ksrcdir}/.config\""
    else
	eval "k_config_search_path=\"
	    ${ksrcdir}/configs/kernel-${knumber}-${karch}${kboot:+-}${kboot}.config
	    ${kmachdir:+${kboot:+${kmachdir}/defconfig${kboot:+-}${kboot}}}
	    ${rootdir}/usr/src/kernels/${kversion}-${kmarch}/.config
	    ${rootdir}/usr/src/kernels/${kversion}/.config
	    ${rootdir}/usr/src/kernel-headers-${kversion}/.config
	    ${rootdir}/usr/src/linux-headers-${kversion}/.config
	    ${rootdir}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}/.config
	    ${rootdir}/usr/src/linux-obj/.config
	    ${kbuilddir}/.config
	    ${kmachdir:+${kboot:-${kmachdir}/defconfig${kboot:+-}${kboot}}}
	    ${ksrcdir}/.config\""
    fi
    _BLD_FIND_FILE([for kernel config file], [linux_cv_k_config],
	    [$k_config_search_path], [], [dnl
	    AC_MSG_ERROR([
*** 
*** Configure cannot find the kernel configuration file.  The
*** directories searched were:
***	"$with_k_config"
***	"$bld_search_path"
*** 
*** Specify the absolute location of the kernel configuration files with
*** the --with-k-config option to configure before attempting again.
*** ])], [dnl
	    case "$linux_cv_k_config_eval" in
		(*/configs/*|*/arch/*/defconf*|*/kernel-headers-*|*/linux-headers-*|*/usr/src/kernels/*)
		    ;;
		(*/boot/*|*/usr/src/*|*/lib/modules/*)
		    AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
*** 
***	"$linux_cv_k_config_eval"
*** 
*** This may cause problems later if you have mismatches between the
*** target kernel and the configuration information contained in that
*** file.
*** ])
		    ;;
	    esac], [with_k_config])
    _kconfig="$linux_cv_k_config_eval"
    kconfig="$linux_cv_k_config"
    AC_SUBST([kconfig])dnl
])# _LINUX_CHECK_KERNEL_DOT_CONFIG
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_FILES
# -------------------------------------------------------------------------
# Check that some distribution specific things about the file that we have found
# are consistent towards a successful build.  Things to check are the rpm/deb
# versions and architectures on the System.map file, the config file, the kernel
# modules directory, the kernel build directory, the kernel headers directory.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_FILES], [dnl
    AC_CACHE_CHECK([for kernel package release], [linux_cv_k_version], [dnl
	linux_ver=
	for linux_file in $_ksysmap $_kvmlinux ; do
	    if test -z "$linux_ver" -a -x "`which rpm 2>/dev/null`"
	    then
		linux_ver=`rpm -q --qf "%{VERSION}" --whatprovides $linux_file 2>/dev/null | head -1`
		linux_ver=`echo "$linux_ver" | sed -e 's|.* is not .*||'`
		linux_ver=`echo "$linux_ver" | sed -e 's|.*no package provides.*||'`
	    fi
	    if test -z "$linux_ver" -a \( -x "`which dlocate 2>/dev/null`" -o -x "`which dpkg 2>/dev/null`" \)
	    then
    dnl
    dnl	    dlocate is much much faster than dpkg and dpkg-query
    dnl
		if which dlocate >/dev/null 2>&1
		then dlocate=dlocate; term='$'
		else dlocate=dpkg;    term=
		fi
		linux_pkg=`$dlocate -S "$linux_file$term" 2>/dev/null | cut -f1 -d:` || linux_pkg=
		if test -n "$linux_pkg" ; then
		    linux_ver=`$dlocate -s "$linux_pkg" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || linux_ver=
		fi
	    fi
	    if test -z "$linux_ver" -a -x "`which pacman 2>/dev/null`"
	    then
		linux_ver=`pacman -Q --owns $linux_file 2>/dev/null | head -1`
		linux_ver=`echo "$linux_ver" | sed -e 's|.*No package owns.*||'`
		linux_ver=`echo "$linux_ver" | awk '{print[$]6}'`
	    fi
	    if test -n "$linux_ver" ; then
		break
	    fi
	done
	linux_cv_k_version="${linux_ver:-unknown}"
    ])
    if test :"${linux_cv_k_version:-unknown}" != :unknown
    then
	krelease="$linux_cv_k_version"
    else
	krelease=
    fi
    AC_SUBST([krelease])
    case "$target_vendor-$kmajor.$kminor" in
	(debian-2.4|ubuntu-2.4|mint-2.4)
	    kernel_image="kernel-image-${kversion}"
	    kernel_source="kernel-source-${kmajor}-${kminor}"
	    kernel_headers="kernel-headers-${kmajor}-${kminor}"
	    ;;
	(debian-2.6|ubuntu-2.6|mint-2.6)
	    kernel_image="linux-image-${kversion}"
	    kernel_source="linux-source-${kmajor}.${kminor}"
	    kernel_headers="linux-headers-${kmajor}.${kminor}"
	    ;;
	(debian-3.*|ubuntu-3.*|mint-3.*)
	    kernel_image="linux-image-${kversion}"
	    kernel_source="linux-source-${kmajor}.${kminor}"
	    kernel_headers="linux-headers-${kmajor}.${kminor}"
	    ;;
	(slackware-3.*|salix-3.*)
	    kernel_image="kernel-generic-${kversion}"
	    kernel_source="kernel-source-${kversion}"
	    kernel_headers="kernel-headers-${kversion}"
	    ;;
	(arch-2.6)
	    if test "$kflavor" != "ARCH"; then
		kernel_image="kernel26${kflavor:+-$kflavor}"
		kernel_headers="kernel26${kflavor:+-$kflavor}-headers"
	    else
		kernel_image="kernel26"
		kernel_headers="kernel26-headers"
	    fi
	    kernel_source=
	    ;;
	(arch-3.*)
	    if test "$kflavor" != "ARCH"; then
		kernel_image="linux${kflavor:+-$kflavor}"
		kernel_headers="linux${kflavor:+-$kflavor}-headers"
	    else
		kernel_image="linux"
		kernel_headers="linux-headers"
	    fi
	    kernel_source=
	    ;;
	(*)
	    kernel_image='kernel'
	    kernel_source='kernel-sourcecode'
	    kernel_headers='kernel-devel'
	    ;;
    esac
    AC_SUBST([kernel_image])
    AC_SUBST([kernel_source])
    AC_SUBST([kernel_headers])
    AC_CACHE_CHECK([for kernel file sanity], [linux_cv_kernel_sanity], [dnl
	linux_cv_files="$_ksysmap $_kvmlinux $_kbuilddir $_ksrcdir $_kconfig"
	case "$target_vendor" in
	    (mandrake|mandriva|mageia)
dnl
dnl		Mandrakelinux is built correctly.
dnl
		;;
	    (debian|ubuntu|mint)
dnl
dnl		Debian can have a mismatch in kernel version.
dnl
		linux_cv_vers=
		for linux_file in $linux_cv_files
		do
dnl
dnl		    dlocate is much much faster than dpkg and dpkg-query
dnl
		    if which dlocate >/dev/null 2>&1
		    then dlocate=dlocate; term='$'
		    else dlocate=dpkg;    term=
		    fi
		    linux_pkg=`$dlocate -S "$linux_file$term" 2>/dev/null | cut -f1 -d:` || linux_pkg=
		    if test -n "$linux_pkg" ; then
			linux_ver=`$dlocate -s "$linux_pkg" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || linux_ver=
		    else
			linux_ver=
		    fi
		    linux_cv_vers="${linux_cv_vers:+$linux_cv_vers }'$linux_ver'"
		done
		;;
	    (redhat|oracle|springdale|puias|centos|whitebox|scientific|suse)
dnl
dnl		Redhat and variants can have a mismatch in kernel architecture.
dnl
		linux_cv_archs=
		for linux_file in $linux_cv_files
		do
		    linux_arch=`rpm -q --qf "%{ARCH}" --whatprovides $linux_file 2>/dev/null`
		    linux_cv_archs="${linux_cv_archs:+$linux_cv_archs }'$linux_arch'"
		done
		;;
	    (arch|slackware|salix|*)
		;;
	esac
	linux_cv_kernel_sanity=ok
    ])
])# _LINUX_CHECK_KERNEL_FILES
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_CFLAGS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_CFLAGS], [dnl
    AC_CACHE_CHECK([for kernel CFLAGS], [linux_cv_k_cflags], [dnl
	if test :"${cross_compiling:-no}" = :no
	then
	    linux_setup_kernel_cflags_tmp=""
	else
	    linux_setup_kernel_cflags_tmp="CROSS_COMPILING=`dirname $KCC` ARCH=${linux_cv_k_mach}"
	fi
	_LINUX_KBUILD_ENV([dnl
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
dnl	Newer kernel makefiles (openSUSE 11.0 with 2.6.25 kernel) need srctree and objtree defined
dnl	and make way too many files in the build directory, so we also now change directories to the
dnl	kernel build directory.  Do not configure as root.  Changing to build directory messes up
dnl	compiler checks in kernel makefiles that attempt to write to the current directory, so it
dnl	was removed.
dnl
dnl	Added in KBUILD_EXTMOD because on newer makefiles this keeps the makefile from trying to
dnl	rebuild intermediate makefiles not necessary for an external module build.
dnl
	linux_builddir=`pwd`
	linux_cv_k_cflags=$(env - PATH="$PATH" ${srcdir}/scripts/cflagcheck CC="$KCC" ${linux_setup_kernel_cflags_tmp:+$linux_setup_kernel_cflags_tmp }srctree=${_ksrcdir} objtree=${_kbuilddir} KERNELRELEASE=${kversion} KERNEL_CONFIG=${_kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${_ksrcdir} TOPDIR=${_ksrcdir} KBUILD_SRC=${_ksrcdir} KBUILD_OUTPUT=${_kbuilddir} KBUILD_EXTMOD=${linux_builddir} -I${_ksrcdir} -I${_khdrdir} -I${_kbuilddir} cflag-check | tail -1)
	linux_cv_k_cflags_orig="$linux_cv_k_cflags"])
	linux_cflags=
	AC_ARG_WITH([k-optimize],
	    [AS_HELP_STRING([--with-k-optimize=HOW],
		[optimization: normal, size, speed or quick @<:@default=auto@:>@])],
	    [], [with_k_optimize="$with_optimize"])

dnl
dnl	Recent x86_64 makefiles add -fno-reorder-blocks which impedes __builtin_expect() which is
dnl	not good.  Strip it off here.
dnl
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -r -e "s| -f(no-)?reorder-blocks||g"`
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -r -e "s| -f(no-)?reorder-functions||g"`
dnl
dnl	Recent x86_64 makefiles add -ffunction-sections which is just insane.  Strip it off here.
dnl
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -r -e "s| -f(no-)?function-sections||g"`
dnl
dnl	Recent kernels add -fno-unit-at-a-time, probably because of a whole bunch of top-level asm
dnl	problems.  Because it impedes optimization independent of positioning of functions in a
dnl	file, we want to kick it out.
dnl
dnl	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -fno-unit-at-a-time||"`

dnl	Some older 2.6 kernels do not set -ffreestanding when required: kick it out and then add it
dnl	back in here.
dnl
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -ffreestanding||"`
	linux_cv_k_cflags="$linux_cv_k_cflags -ffreestanding"

	case "${with_k_optimize:-auto}" in
	    (size)
		linux_cflags="$linux_cflags${linux_cflags:+ }-Os -g"
		linux_cv_debug_default="_NONE"
		linux_cv_optimize='size'
		;;
	    (speed)
		linux_cflags="$linux_cflags${linux_cflags:+ }-O3 -g"
		linux_cv_debug_default="_NONE"
		linux_cv_optimize='speed'
dnl
dnl		Please don't inline everything at -O3.
dnl
dnl		linux_cflags="$linux_cflags${linux_cflags:+ }-fno-inline-functions"
dnl		linux_cflags="$linux_cflags${linux_cflags:+ }-fno-inline-functions-called-once"
		;;
	    (normal)
		linux_cflags="$linux_cflags${linux_cflags:+ }-O2 -g"
		linux_cv_debug_default="_SAFE"
		linux_cv_optimize='normal'
		;;
	    (quick)
		linux_cflags="$linux_cflags${linux_cflags:+ }-O0 -g"
		linux_cv_debug_default="_TEST"
		linux_cv_optimize='quick'
dnl
dnl		Linux kernel header files have some functions that are expected to "disappear" just
dnl		because they are static inline and never referenced.
dnl
		linux_cflags="$linux_cflags${linux_cflags:+ }-finline"
		linux_cflags="$linux_cflags${linux_cflags:+ }-fno-keep-inline-functions"
		linux_cflags="$linux_cflags${linux_cflags:+ }-fno-keep-static-consts"
		;;
	    (auto)
dnl
dnl		Try to pass through whatever optmization options are present.  Always include
dnl		debugging information.  Use install-strip to remove it.
dnl
		linux_cflags="$linux_cflags${linux_cflags:+ }"`echo " $linux_cv_k_cflags " | sed -e 's|^.* -O|-O|;s| .*$||'`
		linux_cflags="$linux_cflags${linux_cflags:+ }-g"
		linux_cv_debug_default="_SAFE"
		linux_cv_optimize='normal'
		;;
	esac
	if test :"${USE_MAINTAINER_MODE:-no}" != :no
	then
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wno-system-headers"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wundef"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wno-endif-labels"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wbad-function-cast"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wcast-qual"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wcast-align"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wwrite-strings"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wconversion"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wsign-compare"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Waggregate-return"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wstrict-prototypes"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wmissing-prototypes"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wmissing-declarations"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wmissing-noreturn"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wmissing-format-attribute"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wpacked"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wpadded"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wredundant-decls"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wnested-externs"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wunreachable-code"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Winline"
dnl	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wdisabled-optimization"
	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Wp,-D_FORTIFY_SOURCE=2"
	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Werror${WFLAGS:+ }${WFLAGS}"
	fi
	AC_ARG_ENABLE([k-inline],
	    [AS_HELP_STRING([--enable-k-inline],
		[kernel inline functions @<:@default=disabled@:>@])],
	    [], [enable_k_inline=no])
	if test :"${enable_k_inline:-no}" != :no 
	then
	    linux_cflags="$linux_cflags${linux_cflags:+ }-finline-functions"
	fi
	linux_cv_k_cflags=`echo " $linux_cv_k_cflags " | sed -e "s| -Iinclude | -I${_kbuilddir}/include |g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	linux_dir="${_kbuilddir}/include2" ; if test -d "$linux_dir" ; then
	linux_cv_k_cflags=`echo " $linux_cv_k_cflags " | sed -e "s| -Iinclude2 | -I${_kbuilddir}/include2 |g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	fi
dnl
dnl	Only ppc and um currently include architecture directories.  The rest include asm
dnl	directories.
dnl
	linux_cv_k_cflags=`echo " $linux_cv_k_cflags " | sed -e "s| -Iinclude/asm| -I${_ksrcdir}/include/asm|g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	linux_cv_k_cflags=`echo " $linux_cv_k_cflags " | sed -e "s| -Iarch/| -I${_ksrcdir}/arch/|g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
dnl
dnl	We now always include -g for debugging information on kernel modules due to SLES 10.  Use
dnl	install-strip instead of install to remove it when unnecessary.
dnl
	linux_cv_k_cflags=`echo " $linux_cv_k_cflags " | sed -e "s| -g | |g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	linux_cv_k_cflags=`echo " $linux_cv_k_cflags " | sed -e "s| -O[[0-9s]]* | $linux_cflags |;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
dnl
dnl	Unfortunately, Linux 2.6 makefiles add (machine dependant) -I includes
dnl	to CFLAGS instead of CPPFLAGS, which is just plain wrong, but that's
dnl	fine, we will strip them out and append them to CPPFLAGS instead.
dnl
	linux_cflags=
	linux_cppflags=
	set dummy $linux_cv_k_cflags
	shift 1
	while test [$]# -gt 0 ; do
	    case ":[$]1" in
		(:-I|:-D)   linux_cppflags="${linux_cppflags}${linux_cppflags:+ }[$]1 [$]2" ; shift 2 ;;
		(:-I*|:-D*) linux_cppflags="${linux_cppflags}${linux_cppflags:+ }[$]1"    ; shift 1 ;;
		(:*)          linux_cflags="${linux_cflags}${linux_cflags:+ }[$]1"        ; shift 1 ;;
	    esac
	done
	linux_cv_k_cflags="$linux_cflags"
	linux_cv_k_more_cppflags="$linux_cppflags"
    ])
    case "${linux_cv_optimize:-normal}" in
	(size)
	    AC_DEFINE([_OPTIMIZE_SIZE], [1], [Define for kernel image size
		optimizations.  This has the effect of -Os and no -g, and
		disabling all assertions.])
	    ;;
	(speed)
	    AC_DEFINE([_OPTIMIZE_SPEED], [1], [Define for kernel image
		speed optimizations.  This has the effect of -O3 -g, and
		disabling all assertions.])
	    ;;
	(normal)
	    AC_DEFINE([_OPTIMIZE_NORMAL], [1], [Define for kernel image
		normal optimization.  This has the effect of -O2 -g, and
		setting assertions to SAFE.])
	    ;;
	(quick)
	    AC_DEFINE([_OPTIMIZE_NONE], [1], [Define for kernel image
		testing (profiling).  This has the effect of -O0 -g, and
		setting assertions to TEST.])
	    ;;
    esac
    AC_CACHE_CHECK([for kernel CPPFLAGS], [linux_cv_k_cppflags], [dnl
	_LINUX_KBUILD_ENV([dnl
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
dnl	Newer kernel makefiles (openSUSE 11.0 with 2.6.25 kernel) need srctree and objtree defined
dnl	and make way too many files in the build directory, so we also now change directories to the
dnl	kernel build directory.  Do not configure as root.  Changing to build directory messes up
dnl	compiler checks in kernel makefiles that attempt to write to the current directory, so it
dnl	was removed.
dnl
dnl	Added in KBUILD_EXTMOD because on newer makefiles this keeps the makefile from trying to
dnl	rebuild intermediate makefiles not necessary for an external module build.
dnl
	linux_builddir=`pwd`
	linux_cv_k_cppflags=$(env - PATH="$PATH" ${srcdir}/scripts/cflagcheck CC="$KCC" srctree=${_ksrcdir} objtree=${_kbuilddir} KERNELRELEASE=${kversion} KERNEL_CONFIG=${_kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${_ksrcdir} TOPDIR=${_ksrcdir} KBUILD_SRC=${_ksrcdir} KBUILD_OUTPUT=${_kbuilddir} KBUILD_EXTMOD=${linux_builddir} -I${_ksrcdir} -I${_khdrdir} -I${_kbuilddir} cppflag-check | tail -1)
	linux_cv_k_cppflags_orig="$linux_cv_k_cppflags"])
	linux_cv_k_cppflags="-nostdinc -isystem `$KCC -print-file-name=include` -iwithprefix include -DLINUX $linux_cv_k_cppflags"
dnl
dnl	Need to adjust 2.6.3 kernel stupid include includes to add the absolute
dnl	location of the source directory.  include2 on the otherhand is properly
dnl	set up for the asm directory.
dnl
	linux_cv_k_cppflags="$linux_cv_k_cppflags $linux_cv_k_more_cppflags"
	linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -e "s| -Iinclude | -Iinclude -I${_kbuilddir}/include -I${_khdrdir}/include -I${_ksrcdir}/include |g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	linux_dir="${_kbuilddir}/include2" ; if test -d "$linux_dir" ; then
	linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -e "s| -Iinclude2 | -I${_kbuilddir}/include2 |g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	fi
dnl
dnl	Some recent 3.x kernels place version.h in include/generated/uapi instead of
dnl	include/linux as was true of previous kernels.
dnl
	linux_dir="${_kbuilddir}/include/generated/uapi" ; if test -d "$linux_dir" ; then
	linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -e "s| -Iinclude/generated/uapi | -I${_kbuilddir}/include/generated/uapi |g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	fi
dnl
dnl	Only ppc and um currently include architecture directories.  The rest include asm
dnl	directories.
dnl
	linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -e "s| -Iinclude/asm| -I${_ksrcdir}/include/asm|g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -e "s| -Iarch/| -I${_ksrcdir}/arch/|g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
dnl
dnl	Recent 2.6.15+ kernels include autoconf.h from the build directory instead of the source
dnl	directory.  I suppose the idea is to allow you to configure in a separate directory as
dnl	well as build.  Given 100 years, kbuild might catch up to autoconf.  SLES 10 for some silly
dnl	reason expands the current working directory in the autoconf.h include, so watch out for
dnl	leading junk in the autoconf.h argument.  Note that Mandriva puts autoconf.h in
dnl	<generated/autoconf.h>.
dnl
	linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -r -e "s, -include (\.*/.*/)?include/(linux|generated)/autoconf.h, -include ${_kbuilddir}/include/\2/autoconf.h,;s,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,"`
dnl
dnl	Non-kbuild (2.4 kernel) always needs include directories to be in the
dnl	build directory.
dnl
	if test :"$linux_cv_k_ko_modules" != :yes ; then
	    linux_src="$_ksrcdir"
	    linux_bld="$_kbuilddir"
	    if test "$linux_src" != "$linux_bld" ; then
		linux_cv_k_cppflags=`echo " $linux_cv_k_cppflags " | sed -e "s| -I${_ksrcdir}/include| -I${_kbuilddir}/include|g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	    fi
	fi
    ])
    if ! echo "$linux_cv_k_cppflags" | grep 'autoconf\.h' >/dev/null 2>&1 ; then
	if ! echo "$linux_cv_k_cppflags" | grep 'kconfig\.h' >/dev/null 2>&1 ; then
	    AC_DEFINE_UNQUOTED([NEED_LINUX_AUTOCONF_H], [$linux_cv_k_autoconf],
		[Defined when the header file ]$linux_cv_k_autoconf[ needs to be explicitly included.])
dnl	    stupid gcc 3.3.3 compiler from FC2 defines 'linux' as '1' and expands it everywhere
	    linux_cv_k_cppflags="-Ulinux $linux_cv_k_cppflags"
	fi
    fi
    AC_CACHE_CHECK([for kernel MODFLAGS], [linux_cv_k_modflags], [dnl
	_LINUX_KBUILD_ENV([dnl
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
dnl	Newer kernel makefiles (openSUSE 11.0 with 2.6.25 kernel) need srctree and objtree defined
dnl	and make way too many files in the build directory, so we also now change directories to the
dnl	kernel build directory.  Do not configure as root.  Changing to build directory messes up
dnl	compiler checks in kernel makefiles that attempt to write to the current directory, so it
dnl	was removed.
dnl
dnl	Added in KBUILD_EXTMOD because on newer makefiles this keeps the makefile from trying to
dnl	rebuild intermediate makefiles not necessary for an external module build.
dnl
	linux_builddir=`pwd`
	linux_cv_k_modflags=$(env - PATH="$PATH" ${srcdir}/scripts/cflagcheck CC="$KCC" srctree=${_ksrcdir} objtree=${_kbuilddir} KERNELRELEASE=${kversion} KERNEL_CONFIG=${_kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${_ksrcdir} TOPDIR=${_ksrcdir} KBUILD_SRC=${_ksrcdir} KBUILD_OUTPUT=${_kbuilddir} KBUILD_EXTMOD=${linux_builddir} -I${_ksrcdir} -I${_khdrdir} -I${_kbuilddir} modflag-check | tail -1)
	linux_cv_k_modflags_orig="$linux_cv_k_modflags"])
dnl
dnl	Unfortunately we need to rip the module flags from the kernel source
dnl	directory makefiles; however, the modversions.h file is in the build
dnl	directory, not the source directory.  For debian this means that we take
dnl	the flags from the kernel-source package but the modversions.h file is
dnl	in the kernel-headers package.  So, we need to change source directory
dnl	to build directory unless they are the same.  Debian and Ubuntu seem to
dnl	handle this properly now for 2.6 kernels.
dnl
	if test :"$linux_cv_k_ko_modules" != :yes ; then
	    if test "$_ksrcdir" != "$_kbuilddir" ; then
		linux_cv_k_modflags=`echo " $linux_cv_k_modflags " | sed -e "s| ${_ksrcdir}/include| ${_kbuilddir}/include|g;s|^[[[:space:]]]*||;s|[[[:space:]]]*$||"`
	    fi
	fi
    ])
    AC_CACHE_CHECK([for kernel KBUILD_STR], [linux_cv_k_bldflags], [dnl
	_LINUX_KBUILD_ENV([dnl
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
dnl	Newer kernel makefiles (openSUSE 11.0 with 2.6.25 kernel) need srctree and objtree defined
dnl	and make way too many files in the build directory, so we also now change directories to the
dnl	kernel build directory.  Do not configure as root.  Changing to build directory messes up
dnl	compiler checks in kernel makefiles that attempt to write to the current directory, so it
dnl	was removed.
dnl
dnl	Added in KBUILD_EXTMOD because on newer makefiles this keeps the makefile from trying to
dnl	rebuild intermediate makefiles not necessary for an external module build.
dnl
	linux_builddir=`pwd`
	linux_cv_k_bldflags=$(env - PATH="$PATH" ${srcdir}/scripts/cflagcheck CC="$KCC" srctree=${_ksrcdir} objtree=${_kbuilddir} KERNELRELEASE=${kversion} KERNEL_CONFIG=${_kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${_ksrcdir} TOPDIR=${_ksrcdir} KBUILD_SRC=${_ksrcdir} KBUILD_OUTPUT=${_kbuilddir} KBUILD_EXTMOD=${linux_builddir} -I${_ksrcdir} -I${_khdrdir} -I${_kbuilddir} bldflag-check | tail -1)
	linux_cv_k_bldflags_orig="$linux_cv_k_bldflags"])
dnl
dnl	As of 2.6.16+ the KBUILD_BASENAME is stringified on the command line and is no longer
dnl	stringified in kernel source files.  This Makefile check checks to see if the KBUILD_STR
dnl	symbol is defined by the makefile, and, if so, stringifies the KBUILD_BASENAME's.  We used
dnl	to have -DKBUILD_BASENAME in the Makefile.am, but as it should always contain a name unique
dnl	for the object linked into a module, it can always be derived from the compilation target,
dnl	and so we do that here.
dnl
	case "$linux_cv_k_bldflags" in
	    (*KBUILD_STR*)
		linux_cv_k_bldflags="'-DKBUILD_STR(s)=\#s'"
		;;
	    (*)
		linux_cv_k_bldflags="'-DKBUILD_STR(s)=s'"
		;;
	esac
dnl	
dnl	As of 4.9.13 kernel, KBUILD_STR() no longer appears in the makefile, but must still
dnl	stringify KBUILD_BASENAME anyway.
dnl	
	if test ${linux_cv_k_major:-0} -ge 4 -a ${linux_cv_k_minor} -ge 9 ; then
		linux_cv_k_bldflags="'-DKBUILD_STR(s)=\#s'"
	fi
	# the escapes and quotes here are delicate: don't change them!
	linux_cv_k_bldflags="${linux_cv_k_bldflags} '-DKBUILD_BASENAME=KBUILD_STR('\`echo [\$][@] | sed -e 's,lib.*_a-,,;s,\.o,,;s,-,_,g'\`')'"
    ])
    AC_CACHE_CHECK([for kernel KBUILD_MODNAME], [linux_cv_k_mnflags], [dnl
	_LINUX_KBUILD_ENV([dnl
dnl
dnl	At some point (I first noticed it in 2.6.32 builds for RHEL 6.0, but they also occur in
dnl	Debian 2.6.32 kernel makefiles--the net says it started in 2.6.30) debug flags require that
dnl	KBUILD_MODNAME be defined when objects are compiled (not simply when they are having modpost
dnl	performed on them).  The target modnameflag-check was added to cflagcheck to rip these flags
dnl	from the makefile.
dnl
	linux_builddir=`pwd`
	linux_cv_k_mnflags=$(env - PATH="$PATH" ${srcdir}/scripts/cflagcheck CC="$KCC" modname=phony srctree=${_ksrcdir} objtree=${_kbuilddir} KERNELRELEASE=${kversion} KERNEL_CONFIG=${_kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${_ksrcdir} TOPDIR=${_ksrcdir} KBUILD_SRC=${_ksrcdir} KBUILD_OUTPUT=${_kbuilddir} KBUILD_EXTMOD=${linux_builddir} -I${_ksrcdir} -I${_khdrdir} -I${_kbuilddir} mnflag-check | tail -1)
	linux_cv_k_mnflags_orig="$linux_cv_k_mnflags"])
	case "$linux_cv_k_mnflags" in
	    (*KBUILD_MODNAME*)
dnl
dnl	This is only a temporary fix.  This needs to be the module name as seen by lsmod.  In almost
dnl	all cases, modules and drivers are $(kpre)KBUILD_BASENAME, but there are some exceptions:
dnl	specfs, streams itself, compatibility modules.  However, those are unlikely to invoke
dnl	anything requiring a dynamic debug anyways, and those will simply use a different name.
dnl
		# the escapes and quotes here are delicate: don't change them!
		linux_cv_k_mnflags="'-DKBUILD_MODNAME=KBUILD_STR('\`echo [\$](kpre)[\$][@] | sed -e 's,lib.*_a-,,;s,\.o,,;s,-,_,g'\`')'"
		;;
	    (*)
		linux_cv_k_mnflags=''
		;;
	esac
    ])
    AC_CACHE_CHECK([for kernel DEBUG_HASH], [linux_cv_k_dhflags], [dnl
	_LINUX_KBUILD_ENV([dnl
dnl
dnl	At some point (I first noticed it in 2.6.32 builds for RHEL 6.0, but they also occur in
dnl	Debian 2.6.32 kernel makefiles--the net says it started in 2.6.30) debug flags required that
dnl	DEBUG_HASH and DEBUG_HASH2 be defined and are created when CONFIG_DYNAMIC_DEBUG is defined.
dnl	The script/basic/hash function is used to hash the directory and module name.  The target
dnl	debugflag-check was added to cflagcheck to rip these flags from the makefile.  From the
dnl	copyright on the hash.c program, RedHat is responsible for this commerical idiocy.
dnl
	linux_builddir=`pwd`
	linux_cv_k_dhflags=$(env - PATH="$PATH" ${srcdir}/scripts/cflagcheck CC="$KCC" modname=phony srctree=${_ksrcdir} objtree=${_kbuilddir} KERNELRELEASE=${kversion} KERNEL_CONFIG=${_kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${_ksrcdir} TOPDIR=${_ksrcdir} KBUILD_SRC=${_ksrcdir} KBUILD_OUTPUT=${_kbuilddir} KBUILD_EXTMOD=${linux_builddir} -I${_ksrcdir} -I${_khdrdir} -I${_kbuilddir} dhflag-check | tail -1)
	linux_cv_k_dhflags_orig="$linux_cv_k_dhflags"])
	case "$linux_cv_k_dhflags" in
	    (*DEBUG_HASH*)
dnl
dnl	There are two hashes (unsigned char but only significant to 6 bits) that are used to shift a
dnl	bit to match against a 64-bit mask.  Two hash functions are used to generate the a djb2 and
dnl	r5 hash from the build directory and KBUILD_MODNAME.  Because the build directory is not
dnl	maintained in the kernel, the hashes can be whatever we want.  There is a 1:4096 chance that
dnl	whatever is used will be invoked by some other dynamic debug request, which will cause a
dnl	little data cache polution if unlucky.  Later I can write a short awk script to generate the
dnl	hash. From the copyright on the hash.c program, RedHat is responsible for this commerical
dnl	idiocy.
dnl
		linux_cv_k_dhflags='-DDEBUG_HASH=0 -DDEBUG_HASH2=63'
		;;
	    (*)
		linux_cv_k_dhflags=''
		;;
	esac
    ])
    CFLAGS="$linux_cv_k_cflags"
    CPPFLAGS="$linux_cv_k_cppflags"
    KERNEL_MODFLAGS="$linux_cv_k_modflags"
    AC_SUBST([KERNEL_MODFLAGS])dnl
    KERNEL_BLDFLAGS="$linux_cv_k_bldflags${linux_cv_k_mnflags:+ $linux_cv_k_mnflags}${linux_cv_k_dhflags:+ $linux_cv_k_dhflags}"
    AC_SUBST([KERNEL_BLDFLAGS])dnl
])# _LINUX_SETUP_KERNEL_CFLAGS
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_BUILDID
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_BUILDID], [dnl
    AC_CACHE_CHECK([for kernel build id], [linux_cv_k_buildid], [dnl
	if ${LD-ld} --build-id 2>&1 | grep 'unrecognized' >/dev/null 2>&1
	then
	    linux_cv_k_buildid=
	else
	    linux_cv_k_buildid='--build-id '
	fi
    ])
    KERNEL_BUILDID="$linux_cv_k_buildid"
    AC_SUBST([KERNEL_BUILDID])
])# _LINUX_SETUP_KERNEL_BUILDID
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_DEBUG], [dnl
    AC_MSG_CHECKING([for kernel debugging])
    AC_ARG_ENABLE([k-safe],
	[AS_HELP_STRING([--enable-k-safe],
	    [kernel run-time safety checks @<:@default=disabled@:>@])],
	[], [enable_k_safe=no])
    if test :"${enable_k_safe:-no}" != :no ; then
	linux_cv_debug='_SAFE'
    fi
    AC_ARG_ENABLE([k-test],
	[AS_HELP_STRING([--enable-k-test],
	    [kernel run-time testing @<:@default=disabled@:>@])],
	[], [enable_k_test=no])
    if test :"${enable_k_test:-no}" != :no ; then
	linux_cv_debug='_TEST'
    fi
    AC_ARG_ENABLE([k-debug],
	[AS_HELP_STRING([--enable-k-debug],
	    [kernel run-time debugging @<:@default=disabled@:>@])],
	[], [enable_k_debug=no])
    if test :"${enable_k_debug:-no}" != :no ; then
	linux_cv_debug='_DEBUG'
    fi
    AC_ARG_ENABLE([k-none],
	[AS_HELP_STRING([--enable-k-none],
	    [kernel module run-time checks @<:@default=disabled@:>@])],
	[], [enable_k_none=no])
    if test :"${enable_k_none:-no}" != :no 
    then
	linux_cv_debug='_NONE'
    fi
    if test -z "$linux_cv_debug" ; then
	linux_cv_debug="$linux_cv_debug_default"
    fi
    case "$linux_cv_debug" in
	(_DEBUG)
	    AC_DEFINE([_DEBUG], [1], [Define for kernel symbol debugging.  This
		has the effect of defeating inlines, making static declarations
		global, and activating all debugging macros.])
	    ;;
	(_TEST)
	    AC_DEFINE([_TEST], [1], [Define for kernel testing.  This has the
		same effect as _DEBUG for now.])
	    ;;
	(_SAFE)
	    AC_DEFINE([_SAFE], [1], [Define for kernel safety.  This has the
		effect of enabling safety debugging macros.  This is the
		default.])
	    ;;
	(*)
	    AC_DEFINE([_NONE], [1], [Define for maximum performance and minimum
		size.  This has the effect of disabling all safety debugging
		macros.])
	    ;;
    esac
    AC_MSG_RESULT([${linux_cv_debug:-no}])dnl
])# _LINUX_SETUP_KERNEL_DEBUG
# =========================================================================

# =============================================================================
# _LINUX_CHECK_MEMBER_internal(AGGREGATE.MEMBER, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent to AC_CHECK_MEMBER
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MEMBER_internal],
    [AS_LITERAL_IF([$1], [],
		   [AC_FATAL([$0: requires literal arguments])])dnl
    m4_bmatch([$1], [\.], ,
	     [m4_fatal([$0: Did not see any dot in '$1'])])dnl
    AS_VAR_PUSHDEF([linux_Member], [linux_cv_member_$1])dnl
    dnl Extract the aggregate name, and the member name
    AC_CACHE_CHECK([for kernel member $1], linux_Member,
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$4])],
    [dnl AGGREGATE linux_aggr;
    static m4_bpatsubst([$1], [\..*]) linux_aggr;
    dnl linux_aggr.MEMBER;
    if (linux_aggr.m4_bpatsubst([$1], [^[^.]*\.]))
    return 0;])],
		    [AS_VAR_SET(linux_Member, yes)],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$4])],
    [dnl AGGREGATE linux_aggr;
    static m4_bpatsubst([$1], [\..*]) linux_aggr;
    dnl sizeof linux_aggr.MEMBER;
    if (sizeof linux_aggr.m4_bpatsubst([$1], [^[^.]*\.]))
    return 0;])],
		    [AS_VAR_SET(linux_Member, yes)],
		    [AS_VAR_SET(linux_Member, no)])])])
    AS_VAR_IF([linux_Member], [yes], [$2], [$3])
    AS_VAR_POPDEF([linux_Member])dnl
])# _LINUX_CHECK_MEMBER_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MEMBERS_internal(AGGREGATE.MEMBER, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_MEMBERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MEMBERS_internal], [dnl
    m4_foreach([LK_Member], [$1],
	[AH_TEMPLATE(AS_TR_CPP(HAVE_KMEMB_[]LK_Member),
		[Define to 1 if ']m4_bpatsubst(LK_Member, [^[^.]*\.])[' is member of ']m4_bpatsubst(LK_Member, [\..*])['.])
	 _LINUX_CHECK_MEMBER_internal(LK_Member,
	    [AC_DEFINE(AS_TR_CPP(HAVE_KMEMB_[]LK_Member), 1) $2], [$3], [$4])])
])# _LINUX_CHECK_MEMBERS_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MEMBER(AGGREGATE.MEMBER, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent to AC_CHECK_MEMBER
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MEMBER], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_MEMBER_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_MEMBER
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MEMBERS(AGGREGATE.MEMBER, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_MEMBERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MEMBERS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_MEMBERS_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_MEMBERS
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MACRO_internal(MACRO, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNC for macros
# -----------------------------------------------------------------------------
m4_define([_LINUX_CHECK_MACRO_internal_BODY],
[AS_LINENO_PUSH([$[]1])
AC_CACHE_CHECK([for kernel macro $[]2], [linux_cv_macro_$[]{2}],
    [AC_EGREP_CPP([\<yes_we_have_${2}_defined\>],
[${3}
#ifdef ${2}
    yes_we_have_${2}_defined
#endif],
	[AS_VAR_SET([linux_cv_macro_$[]{2}], yes)],
	[AS_VAR_SET([linux_cv_macro_$[]{2}], no)])])
AS_LINENO_POP])
AC_DEFUN([_LINUX_CHECK_MACRO_internal], [dnl
    AC_REQUIRE_SHELL_FN([linux_check_macro_function],
	[AS_FUNCTION_DESCRIBE([linux_check_macro_function],
	    [LINENO MACRO HEADERS],
	    [Test whether MACRO exists as a kernel macro.])],
	[$0_BODY])dnl
    AS_VAR_PUSHDEF([linux_Macro], [linux_cv_macro_$1])dnl
    linux_check_macro_function "$LINENO" "$1" "$4"
    AS_VAR_IF([linux_Macro],[yes],[$2],[$3])
    AS_VAR_POPDEF([linux_Macro])dnl
])# _LINUX_CHECK_MACRO_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MACROS_internal(MACROS, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNCS for macros
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MACROS_internal],
[m4_foreach_w([LK_Macro],[$1],[AH_TEMPLATE(AS_TR_CPP(HAVE_KMACRO_[]LK_Macro),
	       [Define to 1 if kernel macro ]LK_Macro[() exists.])])dnl
for lk_macro in $1
do
    _LINUX_CHECK_MACRO_internal($lk_macro,
	[AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_KMACRO_$lk_macro]), 1)
$2],
	[$3],
	[$4])
done
])# _LINUX_CHECK_MACROS_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MACRO(MACRO, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent to AC_CHECK_FUNC for macros
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MACRO], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_MACRO_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_MACRO
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MACROS(MACROS, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNCS for macros
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MACROS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_MACROS_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_MACROS
# =============================================================================

# =============================================================================
# _LINUX_CHECK_FUNC_internal(FUNCTION, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNC
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_FUNC_internal],
    [AS_VAR_PUSHDEF([linux_Function], [linux_cv_func_$1])dnl
    AC_CACHE_CHECK([for kernel function $1], linux_Function,
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$4])], [
void (*my_autoconf_function_pointer)(void) = (typeof(my_autoconf_function_pointer))&$1;
	 ])],
	[AS_VAR_SET(linux_Function, yes)],
	[AS_VAR_SET(linux_Function, no)])])
    AS_VAR_IF([linux_Function], [yes], [$2], [$3])
    AS_VAR_POPDEF([linux_Function])dnl
])# _LINUX_CHECK_FUNC_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_FUNCS_internal(FUNCTIONS, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNCS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_FUNCS_internal],
[m4_foreach_w([LK_Function],[$1],[AH_TEMPLATE(AS_TR_CPP(HAVE_KFUNC_[]LK_Function),
	       [Define to 1 if kernel function ]LK_Function[() exists.])])dnl
for lk_func in $1
do
    _LINUX_CHECK_FUNC_internal($lk_func,
	[AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_KFUNC_$lk_func]), 1)
$2],
	[$3],
	[$4])
done
])# _LINUX_CHECK_FUNCS_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_FUNC(FUNCTION, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent to AC_CHECK_FUNC
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_FUNC], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_FUNC_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_FUNC
# =============================================================================

# =============================================================================
# _LINUX_CHECK_FUNCS(FUNCTIONS, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNCS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_FUNCS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_FUNCS_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_FUNCS
# =============================================================================

# =============================================================================
# _LINUX_CHECK_TYPE_internal
# -----------------------------------------------------------------------------
m4_define([_LINUX_CHECK_TYPE_internal_BODY],
[AS_LINENO_PUSH([$[]1])
AC_CACHE_CHECK([for kernel type $[]2], [$[]3],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$[]4])],
[if ((${2} *) 0)
    return 0;
if (sizeof (${2}))
    return 0;])],
	[AS_VAR_SET([$[]3], yes)],
	[AS_VAR_SET([$[]3], no)])])
AS_LINENO_POP])
AC_DEFUN([_LINUX_CHECK_TYPE_internal], [dnl
    AC_REQUIRE_SHELL_FN([linux_check_type_function],
	[AS_FUNCTION_DESCRIBE([linux_check_type_function],
	    [LINENO TYPE VAR HEADERS],
	    [Test whether TYPE exists.])],
	[$0_BODY])dnl
    AS_VAR_PUSHDEF([linux_Type], [linux_cv_type_$1])dnl
    linux_check_type_function "$LINENO" "$1" "linux_Type" "$linux_type_HEADERS"
    AS_VAR_IF([linux_Type], [yes], [$2], [$3])
    AS_VAR_POPDEF([linux_Type])dnl
])# _LINUX_CHECK_TYPE_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_TYPES_internal(TYPES,
#       [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#       [INCLUDES = DEFAULT-INCLUDES])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_TYPES_internal], [dnl
    linux_type_HEADERS="$4"
    m4_foreach([LK_Type], [$1],
	[AH_TEMPLATE(AS_TR_CPP(HAVE_KTYPE_[]LK_Type),
		[Define to 1 if the system has the type ']LK_Type['.])
	 _LINUX_CHECK_TYPE_internal(LK_Type,
	    [AC_DEFINE(AS_TR_CPP(HAVE_KTYPE_[]LK_Type), 1) $2], [$3])])
])# _LINUX_CHECK_TYPES_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_TYPE
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_TYPE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    linux_type_HEADERS="$4"
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_TYPE_internal([$1], [$2], [$3])])
])# _LINUX_CHECK_TYPE
# =============================================================================

# =============================================================================
# _LINUX_CHECK_TYPES
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_TYPES], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_TYPES_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_TYPES
# =============================================================================

# =============================================================================
# _LINUX_CHECK_HEADER_internal
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_HEADER_internal], [dnl
    AS_VAR_PUSHDEF([linux_Header], [linux_cv_header_$1])dnl
    AC_CACHE_CHECK([for kernel header $1], linux_Header,
	[AC_COMPILE_IFELSE([AC_LANG_SOURCE([AC_INCLUDES_DEFAULT([$4])
@%:@include <$1>])],
	[AS_VAR_SET(linux_Header, yes)],
	[AS_VAR_SET(linux_Header, no)])])
    AS_VAR_IF([linux_Header], [yes], [$2], [$3])
    AS_VAR_POPDEF([linux_Header])dnl
])# _LINUX_CHECK_HEADER_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_HEADERS_internal
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_HEADERS_internal], [dnl
    m4_foreach_w([LK_Header],[$1],[AH_TEMPLATE(AS_TR_CPP(HAVE_KINC_[]LK_Header),
	    [Define to 1 if you have the <]LK_Header[> header file.])])
for lk_header in $1
do
    _LINUX_CHECK_HEADER_internal($lk_header,
	[AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_KINC_$lk_header), 1)
$2],
	[$3],
	[$4])
done
])# _LINUX_CHECK_HEADERS_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_HEADER
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_HEADER], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_HEADER_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_HEADER
# =============================================================================

# =============================================================================
# _LINUX_CHECK_HEADERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_HEADERS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_HEADERS_internal([$1], [$2], [$3], [$4])])
])# _LINUX_CHECK_HEADERS
# =============================================================================

# =============================================================================
# _LINUX_CHECK_KERNEL_CONFIG_internal([CHECKING-LABEL], CONFIG-NAME, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_CONFIG_internal], [dnl
    if test -n "$1" ; then linux_check_kernel_config_tmp="$1" ; else linux_check_kernel_config_tmp="for kernel config $2" ; fi
    AS_VAR_PUSHDEF([linux_config], [linux_cv_$2])dnl
    AC_CACHE_CHECK([$linux_check_kernel_config_tmp], [linux_cv_$2], [dnl
	AC_EGREP_CPP([\<yes_we_have_$2_defined\>], [
#include <linux/version.h>
#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#ifdef $2
    yes_we_have_$2_defined
#endif
	], [AS_VAR_SET([linux_config], ['yes'])], [AS_VAR_SET([linux_config], ['no'])]) ])
    AS_VAR_IF([linux_config], [yes], [$3], [$4])
    AS_VAR_POPDEF([linux_config])dnl
])# _LINUX_CHECK_KERNEL_CONFIG_internal
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_CONFIG([CHECKING-LABEL], CONFIG-NAME, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_CONFIG], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_KERNEL_CONFIG_internal([$1], [$2], [$3], [$4]) ])
])# _LINUX_CHECK_KERNEL_CONFIG
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_REGPARM
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_REGPARM], [dnl
    _LINUX_CHECK_KERNEL_CONFIG_internal([], [CONFIG_REGPARM])dnl
])# _LINUX_CHECK_KERNEL_REGPARM
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SMP
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SMP], [dnl
    _LINUX_CHECK_KERNEL_CONFIG_internal([], [CONFIG_SMP])dnl
])# _LINUX_CHECK_KERNEL_SMP
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_PREEMPT
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_PREEMPT], [dnl
    _LINUX_CHECK_KERNEL_CONFIG_internal([], [CONFIG_PREEMPT])dnl
])# _LINUX_CHECK_KERNEL_PREEMPT
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_VERSIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_VERSIONS], [dnl
    _LINUX_CHECK_KERNEL_CONFIG_internal([], [CONFIG_MODVERSIONS],
    [linux_cv_k_versions=yes], [linux_cv_k_versions=no])dnl
])# _LINUX_CHECK_KERNEL_VERSIONS
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODVERSIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODVERSIONS], [dnl
    AC_MSG_CHECKING([for kernel module symbol versioning])
    AC_ARG_ENABLE([k-modversions],
	[AS_HELP_STRING([--enable-k-modversions],
	    [export symbol versioning @<:@default=auto@:>@])],
	[], [enable_k_modversions="$linux_cv_k_versions"])
    if test :"${enable_k_modversions:-no}" = :yes ; then
	linux_cv_k_modversions='yes'
    else
	linux_cv_k_modversions='no'
    fi
    AC_MSG_RESULT([$linux_cv_k_modversions])
    AM_CONDITIONAL([KERNEL_VERSIONS], [test x"$linux_cv_k_modversions" = xyes])dnl
])# _LINUX_CHECK_KERNEL_MODVERSIONS
# =========================================================================

# =============================================================================
# _LINUX_CHECK_KERNEL_UPDATE_MODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_UPDATE_MODULES], [dnl
    modutildir='${sysconfdir}/modutils'
    AC_SUBST([modutildir])dnl
])# _LINUX_CHECK_KERNEL_UPDATE_MODULES
# =============================================================================

# =============================================================================
# _LINUX_CHECK_KERNEL_MODULE_PRELOAD
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODULE_PRELOAD], [dnl
    if test :"${linux_cv_k_ko_modules:-no}" = :no
    then
	preloads='${sysconfdir}/modules'
    else
	preloads='${sysconfdir}/modprobe.preload'
    fi
    AC_SUBST([preloads])dnl
])# _LINUX_CHECK_KERNEL_MODULE_PRELOAD
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOL_ADDR(SYMBOLNAME, [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# -----------------------------------------------------------------------------
# Note that we also add any ripped symbols to KERNEL_WRAPPER so that if we choose
# to re-export the ripped symbols they will be treated specially by the symbol
# export check procedures.
# -----------------------------------------------------------------------------
m4_define([_LINUX_KERNEL_SYMBOL_ADDR_BODY],
[AS_LINENO_PUSH([$[]1])
AC_CACHE_CHECK([for kernel symbol $[]2 address], [linux_cv_$[]{2}_addr], [dnl
    linux_kernel_symbol_addr_tmp=
    if test -z "$linux_kernel_symbol_addr_tmp" -a -n "$ksyms" -a -r "$ksyms"; then
	linux_kernel_symbol_addr_tmp="`($EGREP '\<'${2}'\>$' $ksyms | sed -e 's| .*||;s|^0[[xX]]||') 2>/dev/null`"
    fi
    if test -z "$linux_kernel_symbol_addr_tmp" -a -n "$kallsyms" -a -r "$kallsyms"; then
	linux_kernel_symbol_addr_tmp="`($EGREP '\<'${2}'\>$' $kallsyms | head -1 | sed -e 's| .*||;s|^0[[xX]]||') 2>/dev/null`"
    fi
    if test -z "$linux_kernel_symbol_addr_tmp" -a -n "$_ksysmap" -a -r "$_ksysmap"; then
	linux_kernel_symbol_addr_tmp="`($EGREP '\<'${2}'\>$' $_ksysmap | sed -e 's| .*||;s|^0[[xX]]||') 2>/dev/null`"
    fi
    if test -z "$linux_kernel_symbol_addr_tmp" -a -n "$_kvmlinux" -a -r "$_kvmlinux"; then
	case "$_kvmlinux" in
	    (*.gz)  linux_img="/var/tmp/.tmp.$$.`basename $_kvmlinux .gz`"
		    test -r $linux_img || gzip -dc $_kvmlinux >$linux_img ;;
	    (*.bz2) linux_img="/var/tmp/.tmp.$$.`basename $_kvmlinux .bz2`"
		    test -r $linux_img || bzip2 -dc $_kvmlinux >$linux_img ;;
	    (*.xz)  linux_img="/var/tmp/.tmp.$$.`basename $_kvmlinux .xz`"
		    test -r $linux_img || xz -dc $_kvmlinux >$linux_img ;;
	    (*)	    linux_img="$_kvmlinux" ;;
	esac
	linux_kernel_symbol_addr_tmp="`(nm -Bs $linux_img | $EGREP '\<'${2}'\>$' | sed -e 's| .*||;s|^0[[xX]]||') 2>/dev/null`"
    fi
    linux_kernel_symbol_addr_tmp="${linux_kernel_symbol_addr_tmp:+0x}$linux_kernel_symbol_addr_tmp"
    AS_VAR_SET([linux_cv_$[]{2}_addr], ["${linux_kernel_symbol_addr_tmp:-no}"]) ])
    AS_VAR_IF([linux_cv_$[]{2}_addr],[no],[],[dnl
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}[]_SYMBOL), [1])dnl
	AS_VAR_SET([linux_cv_$[]{2}_symbol], ['yes'])
	if test :${linux_cv_k_weak_modules:-yes} = :no; then
	    AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_USABLE), [1])dnl
	    AS_VAR_SET([linux_cv_$[]{2}_usable], [yes])
	else
	    AS_VAR_SET([linux_cv_$[]{2}_usable], [no])
	fi
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_ADDR), AS_VAR_GET([linux_cv_$[]{2}_addr]))dnl
	KERNEL_WRAPPER="${KERNEL_WRAPPER:+$KERNEL_WRAPPER }$[]2"
	AC_SUBST([KERNEL_WRAPPER])])
AS_LINENO_POP])
AC_DEFUN([_LINUX_KERNEL_SYMBOL_ADDR], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_REQUIRE_SHELL_FN([linux_kernel_symbol_addr],
	[AS_FUNCTION_DESCRIBE([linux_kernel_symbol_addr],
	    [LINENO SYMBOL],
	    [Test whether SYMBOL can be ripped from the system maps.])],
	[$0_BODY])dnl
    AS_VAR_PUSHDEF([linux_symbol_addr],   [linux_cv_$1_addr])dnl
    linux_kernel_symbol_addr "$LINENO" "$1"
    AS_VAR_IF([linux_symbol_addr],[no],[$2],[$3])
    AS_VAR_POPDEF([linux_symbol_addr])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_SYMBOL), [Symbol $1 is available.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_USABLE), [Symbol $1 is usable.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_ADDR),   [Symbol $1 has an address.])dnl
])# _LINUX_KERNEL_SYMBOL_ADDR
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_EXPORT_ONLY(SYMBOLNAME, [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# -----------------------------------------------------------------------------
# This checks for exported symbols before attempting to rip symbols from the
# system maps.  The checks are made to try to find appropriate symbols first.
# If we are configured for the running kernel and /proc/ksyms can be read, it
# contains exported symbols and is used.  Otherwise, if a system map was located
# it will be used.  Otherwise, header files will be checked.
# -----------------------------------------------------------------------------
m4_define([_LINUX_KERNEL_EXPORT_ONLY_BODY],
[AS_LINENO_PUSH([$[]1])
AC_CACHE_CHECK([for kernel symbol $[]2 export], [linux_cv_$[]{2}_export],
    [linux_kernel_export_only_tmp=
    if test -z "$linux_kernel_export_only_tmp" -a -n "$ksyms" -a -r "$ksyms"; then
	if ( $EGREP -q  '(\<'${2}'_R(smp_)?........\>|\<'${2}'\>)' $ksyms 2>/dev/null ); then
	    linux_kernel_export_only_tmp="yes ($ksyms)"; fi; fi
    if test -z "$linux_kernel_export_only_tmp" -a -n "$kallsyms" -a -r "$kallsyms"; then
	if ( $EGREP -q '\<__ksymtab_'${2}'\>' $kallsyms 2>/dev/null ); then
	    linux_kernel_export_only_tmp="yes ($kallsyms)"; fi; fi
    if test -z "$linux_kernel_export_only_tmp" -a -n "$_ksysmap" -a -r "$_ksysmap"; then
	if ( $EGREP -q '\<__ksymtab_'${2}'\>' $_ksysmap 2>/dev/null ); then
	    linux_kernel_export_only_tmp="yes ($_ksysmap)"; fi; fi
    if test -z "$linux_kernel_export_only_tmp" -a -n "$_kvmlinux" -a -r "$_kvmlinux"; then
	case "$_kvmlinux" in
	    (*.gz)  linux_img="/var/tmp/.tmp.$$.`basename $_kvmlinux .gz`"
		    test -r $linux_img || gzip -dc $_kvmlinux >$linux_img ;;
	    (*.bz2) linux_img="/var/tmp/.tmp.$$.`basename $_kvmlinux .bz2`"
		    test -r $linux_img || bzip2 -dc $_kvmlinux >$linux_img ;;
	    (*.xz)  linux_img="/var/tmp/.tmp.$$.`basename $_kvmlinux .xz`"
		    test -r $linux_img || xz -dc $_kvmlinux >$linux_img ;;
	    (*)	    linux_img="$_kvmlinux" ;;
	esac
	if ( nm -Bs $linux_img | $EGREP -q '\<__ksymtab_'${2}'\>' 2>/dev/null ); then
	    linux_kernel_export_only_tmp="yes ($_kvmlinux)"
	fi
    fi
    if test -z "$linux_kernel_export_only_tmp" -a -n "$kmodver" -a -r "$kmodver"; then
	if ( $EGREP -q '\<'${2}'\>' $kmodver 2>/dev/null ); then
	    linux_kernel_export_only_tmp="yes ($kmodver)"; fi; fi
    if test -z "$linux_kernel_export_only_tmp" -a -n "$ksymvers" -a -r "$ksymvers"; then
	if ( zcat $ksymvers | $EGREP -q '\<'${2}'\>' 2>/dev/null ); then
	    linux_kernel_export_only_tmp="yes ($ksymvers)"; fi; fi
    if test -z "$linux_kernel_export_only_tmp" -a ":$linux_cv_k_ko_modules" != :yes; then
	AC_EGREP_CPP([\<yes_symbol_${2}_is_exported\>],
[#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#ifdef $[]2
	yes_symbol_${2}_is_exported
#endif], [linux_kernel_export_only_tmp='yes (linux/modversions.h)'])
    fi
    AS_VAR_SET([linux_cv_$[]{2}_export],["${linux_kernel_export_only_tmp:-no}"])])
    AS_VAR_IF([linux_cv_$[]{2}_export],[no],[],[dnl
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_SYMBOL), [1])dnl
	AS_VAR_SET([linux_cv_$[]{2}_symbol], [yes])
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_USABLE), [1])dnl
	AS_VAR_SET([linux_cv_$[]{2}_usable], [yes])
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_EXPORT), [1])dnl
	KERNEL_EXPORTS="${KERNEL_EXPORTS:+$KERNEL_EXPORTS }$[]2"
	AC_SUBST([KERNEL_EXPORTS]) ])
AS_LINENO_POP])
AC_DEFUN([_LINUX_KERNEL_EXPORT_ONLY], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_REQUIRE_SHELL_FN([linux_kernel_symbol_export],
	[AS_FUNCTION_DESCRIBE([linux_kernel_symbol_export],
	[LINENO SYMBOL],
	[Test whether SYMBOL is exported from the kernel.])],
	[$0_BODY])dnl
    AS_VAR_PUSHDEF([linux_symbol_export], [linux_cv_$1_export])dnl
    _LINUX_KERNEL_ENV([linux_kernel_symbol_export "$LINENO" "$1"])
    AS_VAR_IF([linux_symbol_export],[no],[$2],[$3])
    AS_VAR_POPDEF([linux_symbol_export])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_SYMBOL), [Symbol $1 is available.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_USABLE), [Symbol $1 is usable.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_EXPORT), [Symbol $1 is exported.])dnl
])# _LINUX_KERNEL_EXPORT_ONLY
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOL_EXPORT(SYMBOLNAME, [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# -----------------------------------------------------------------------------
# If the symbol is not exported, an attempt will be made to rip it from the
# system maps.
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SYMBOL_EXPORT], [dnl
    _LINUX_KERNEL_EXPORT_ONLY([$1], [_LINUX_KERNEL_SYMBOL_ADDR([$1], [$2], [$3])], [$3])
])# _LINUX_KERNEL_SYMBOL_EXPORT
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_EXPORTS(SYMBOLS, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_EXPORTS], [dnl
    m4_foreach([LK_Export], [$1], [dnl
	_LINUX_KERNEL_EXPORT_ONLY(LK_Export, [$3], [$2])])
])# _LINUX_KERNEL_EXPORTS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOL(SYMBOL, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SYMBOL],
    [AC_REQUIRE([_LINUX_KERNEL])
    _LINUX_KERNEL_SYMBOL_EXPORT([$1], [$2], [$3])
])# _LINUX_KERNEL_SYMBOL
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOLS(SYMBOLS, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SYMBOLS], [dnl
    m4_foreach([LK_Symbol], [$1], [dnl
	_LINUX_KERNEL_SYMBOL(LK_Symbol, [$2], [$3])])
])# _LINUX_KERNEL_SYMBOLS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SUPPORT_ONLY(SYMBOLNAME, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
# This checks for kABI supported symbols.
m4_define([_LINUX_KERNEL_SUPPORT_ONLY_BODY],
[AS_LINENO_PUSH([$[]1])
AC_CACHE_CHECK([for kernel symbol $[]2 support], [linux_cv_$[]{2}_support], [dnl
    linux_kernel_support_only_tmp=
    if test -z "$linux_kernel_support_only_tmp" -a -n "$kmodabi" -a -r "$kmodabi"; then
	if ( $EGREP -q '\<'${2}'\>' $kmodabi 2>/dev/null ); then
	    linux_kernel_support_only_tmp="yes ($kmodabi)"; fi; fi
    if test -z "$linux_kernel_support_only_tmp" -a -n "$ksymsets" -a -r "$ksymsets"; then
	if ( tar -xzOf $ksymsets | $EGREP -q '\<'${2}'\>' 2>/dev/null ); then
	    linux_kernel_support_only_tmp="yes ($ksymsets)"; fi; fi
    if test -z "$linux_kernel_support_only_tmp" -a -z "$kmodabi" -a -z "$ksymsets"; then
	if test -z "$linux_kernel_support_only_tmp" -a -n "$kmodver" -a -r "$kmodver"; then
	    if ( $EGREP -q '\<'${2}'\>' $kmodver 2>/dev/null ); then
		linux_kernel_support_only_tmp="yes ($kmodver)"; fi; fi
	if test -z "$linux_kernel_support_only_tmp" -a -n "$ksymvers" -a -r "$ksymvers"; then
	    if ( zcat $ksymvers | $EGREP -q '\<'${2}'\>' 2>/dev/null ); then
		linux_kernel_support_only_tmp="yes ($ksymvers)"; fi; fi
    fi
    AS_VAR_SET([linux_cv_$[]{2}_support], ["${linux_kernel_support_only_tmp:-no}"]) ])
    AS_VAR_IF([linux_cv_$[]{2}_support],[no],[],[dnl
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_EXPORT), [1])dnl
	AS_VAR_SET([linux_cv_$[]{2}_export], ["$linux_kernel_support_only_tmp"])
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_SYMBOL), [1])dnl
	AS_VAR_SET([linux_cv_$[]{2}_symbol], ['yes'])
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_USABLE), [1])dnl
	AS_VAR_SET([linux_cv_$[]{2}_usable], ['yes'])
	AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$[]{2}_SUPPORT),[1])dnl
	KERNEL_SUPPORT="${KERNEL_SUPPORT:+$KERNEL_SUPPORT }$1"
	AC_SUBST([KERNEL_SUPPORT])])
AS_LINENO_POP])
AC_DEFUN([_LINUX_KERNEL_SUPPORT_ONLY], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_REQUIRE_SHELL_FN([linux_kernel_symbol_support],
	[AS_FUNCTION_DESCRIBE([linux_kernel_symbol_support],
	    [LINENO SYMBOL],
	    [Test whether SYMBOL is supported by the kABI.])],
	[$0_BODY])dnl
    AS_VAR_PUSHDEF([linux_symbol_support], [linux_cv_$1_support])dnl
    linux_kernel_symbol_support "$LINENO" "$1"
    AS_VAR_IF([linux_symbol_support],[no],[$2],[$3])
    AS_VAR_POPDEF([linux_symbol_support])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_EXPORT), [Symbol $1 is exported.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_SYMBOL), [Symbol $1 is available.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_USABLE), [Symbol $1 is usable.])dnl
    AH_TEMPLATE(AS_TR_CPP(HAVE_$1[]_SUPPORT),[Symbol $1 is supported.])dnl
])# _LINUX_KERNEL_SUPPORT_ONLY
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SYMBOL_SUPPORT(SYMBOLNAME, [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# -----------------------------------------------------------------------------
# If the symbol is not supported, an attempt will be made to determine whether
# it is exported or rip it from the system maps.
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SYMBOL_SUPPORT],
    [_LINUX_KERNEL_SUPPORT_ONLY([$1], [$2], [$3])
])# _LINUX_KERNEL_SYMBOL_SUPPORT
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ABI_SYMBOLS(SYMBOLS, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_ABI_SYMBOLS],
    [m4_foreach([LK_Export], [$1],
		[_LINUX_KERNEL_SYMBOL_SUPPORT(LK_Export, [$3], [$2])])
])# _LINUX_KERNEL_ABI_SYMBOLS
# =============================================================================

# =============================================================================
# _LINUX_CHECK_KERNEL_CACHE
# -----------------------------------------------------------------------------
# When performing a config.status --recheck, re-performing all of the kernel
# configuration checks is too time consuming to the development cycle.
# Therefore, when we are performing a recheck and we have established all of the
# information necessary to provide a distro- and kernel- specific cache file
# name, read that file if it exists.
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_CACHE], [dnl
    # if kernel file is not specified, use local kernel file
    if test -z "$CONFIG_KERNEL" ; then
	CONFIG_KERNEL="${target}-${kversion}-config.cache"
    fi
    AC_SUBST([CONFIG_KERNEL])dnl
    if test "$no_create" = yes ; then
	for config_kernel in $CONFIG_KERNEL ; do
	    case "$config_kernel" in
		(/*) ;;
		(*) config_kernel="`pwd`/$config_kernel" ;;
	    esac
	    if test -r "$config_kernel" ; then
		AC_MSG_NOTICE([reading cache file... $config_kernel])
		. "$config_kernel"
	    fi
	done
    fi
    AC_CONFIG_COMMANDS([kernelconfig], [dnl
	if test -n "$CONFIG_KERNEL" ; then
	    for config_kernel in $CONFIG_KERNEL ; do
		if test -w "$config_kernel" -o ! -e "$config_kernel" ; then
		    if touch "$config_kernel" >/dev/null 2>&1 ; then
			cat "$cache_file" | egrep "^(test \"\\\[$]{)?(linux_cv_|ksyms_cv_|os7_cv_|strconf_cv_)" > "$config_kernel" 2>/dev/null
		    fi
		fi
	    done
	fi], [dnl
cache_file="$cache_file"
CONFIG_KERNEL="$CONFIG_KERNEL"
    ])
])# _LINUX_CHECK_KERNEL_CACHE
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_], [dnl
])# _LINUX_KERNEL_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
