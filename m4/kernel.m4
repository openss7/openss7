# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: kernel.m4,v $ $Name:  $($Revision: 0.9.2.162 $) $Date: 2008-04-10 10:53:23 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2008-04-10 10:53:23 $ by $Author: brian $
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
	AC_SUBST([KERNEL_CPPFLAGS])dnl
	AC_SUBST([KERNEL_CFLAGS])dnl
	AC_SUBST([KERNEL_LDFLAGS])dnl
	KERNEL_CPPFLAGS="$CPPFLAGS"
	KERNEL_CFLAGS="$CFLAGS"
	KERNEL_LDFLAGS="$LDFLAGS"])dnl
])# _LINUX_KERNEL
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV_FUNCTIONS
# -----------------------------------------------------------------------------
AC_DEFUN_ONCE([_LINUX_KERNEL_ENV_FUNCTIONS], [dnl
linux_flag_nest=0
linux_flags_push() {
    eval "linux_${linux_flag_nest}_cppflags=\"\$CPPFLAGS\""
    eval "linux_${linux_flag_nest}_cflags=\"\$CFLAGS\""
    eval "linux_${linux_flag_nest}_ldflags=\"\$LDFLAGS\""
    ((linux_flag_nest++))
}
linux_flags_pop() {
    ((linux_flag_nest--))
    eval "CPPFLAGS=\"\$linux_${linux_flag_nest}_cppflags\""
    eval "CFLAGS=\"\$linux_${linux_flag_nest}_cflags\""
    eval "LDFLAGS=\"\$linux_${linux_flag_nest}_ldflags\""
}
linux_kernel_env_push() {
    linux_flags_push
dnl We need safe versions of these flags without warnings or strange optimizations
dnl but with module flags included
dnl But we need to skip -DMODVERSIONS and -include /blah/blah/modversion.h on rh systems.
    BLDFLAGS=`echo "$KERNEL_BLDFLAGS" | sed -e "s|'||g;s|.#s|#s|;s|-DKBUILD_BASENAME.*|-DKBUILD_BASENAME=KBUILD_STR(phony)|"`
    MODFLAGS=`echo " $KERNEL_MODFLAGS -DKBUILD_MODNAME=\"phony\" " | sed -e 's| -DMOVERSIONS||g;s| -include [[^ ]]*||g'`
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
    done
}
])# _LINUX_KERNEL_ENV_FUNCTIONS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_ENV_BARE([COMMANDS])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_ENV_BARE], [dnl
AC_REQUIRE([_LINUX_KERNEL_ENV_FUNCTIONS])dnl
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
# _LINUX_KERNEL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_OPTIONS], [dnl
dnl AC_ARG_ENABLE([k-install],
dnl     AS_HELP_STRING([--enable-k-install],
dnl         [specify whether kernel modules will be installed.
dnl         @<:@default=yes@:>@]),
dnl     [enable_k_install="$enableval"],
dnl     [enable_k_install='yes'])
    AC_ARG_ENABLE([k-package],
	AS_HELP_STRING([--enable-k-package],
	    [specify whether kernel source packages will be generated.
	     @<:@default=disabled@:>@]),
	[enable_k_package="$enableval"],
	[enable_k_package='no'])
    AM_CONDITIONAL([WITH_K_PACKAGE], [test :"${enable_k_package:-no}" != :no])
])# _LINUX_KERNEL_OPTIONS
# =============================================================================

# =============================================================================
# _LINUX_KERNEL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_SETUP], [dnl
    _LINUX_CHECK_KERNEL_RELEASE
    _LINUX_CHECK_KERNEL_LINKAGE
    _LINUX_CHECK_KERNEL_TOOLS
    _LINUX_CHECK_KERNEL_MODULES
    _LINUX_CHECK_KERNEL_MARCH
    _LINUX_CHECK_KERNEL_BOOT
    _LINUX_CHECK_KERNEL_BUILDDIR
    _LINUX_CHECK_KERNEL_SRCDIR
    _LINUX_CHECK_KERNEL_SYSMAP
    _LINUX_CHECK_KERNEL_KSYMS
    _LINUX_CHECK_KERNEL_KALLSYMS
    _LINUX_CHECK_KERNEL_MINORBITS
    _LINUX_CHECK_KERNEL_COMPILER
    _LINUX_CHECK_KERNEL_ARCHDIR
    _LINUX_CHECK_KERNEL_MACHDIR
    _LINUX_CHECK_KERNEL_DOT_CONFIG
    _LINUX_CHECK_KERNEL_FILES
    _LINUX_SETUP_KERNEL_CFLAGS
    _LINUX_SETUP_KERNEL_DEBUG
    _LINUX_CHECK_KERNEL_SMP
    _LINUX_CHECK_KERNEL_PREEMPT
    _LINUX_CHECK_KERNEL_REGPARM
    _LINUX_CHECK_KERNEL_VERSIONS
    _LINUX_CHECK_KERNEL_MODVERSIONS
    _LINUX_CHECK_KERNEL_MODVER
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
	AS_HELP_STRING([--with-k-release=UTSRELEASE],
	    [specify the UTS release of the linux kernel for which the build
	    is targetted.  If this option is not set, the build will be
	    targetted at the kernel running in the build environment.
	    @<:@default=`uname -r`@:>@]),
	[with_k_release="$withval"],
	[with_k_release=])
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
*** You have not specified --with-k-releease indicating that the build
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
    if test ${linux_cv_k_major:-0} -ne 2
    then
	AC_MSG_ERROR([
*** 
*** Kernel major release number is "$linux_cv_k_major".  That cannot be
*** correct, unless this is really old software now and Linux has
*** already made it to release 3.  In which case, give up.  More likely,
*** "$linux_cv_k_release" is not the UTS_RELEASE of the target.  Check
*** the UTS_RELEASE of the target and specify with with the configure
*** option --with-k-release or look at the result of uname -r ("`uname
*** -r`") to determine the problem.
*** ])
    fi
    AC_CACHE_CHECK([for kernel minor release number], [linux_cv_k_minor], [dnl
	linux_cv_k_minor="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.||;s|[[^0-9]].*||'`" ])
    if test ${linux_cv_k_minor:-0} -ne 4 -a ${linux_cv_k_minor:-0} -ne 6
    then
	AC_MSG_ERROR([
*** 
*** Kernel minor release number "$linux_cv_k_minor" is either too old or
*** too new, or the UTS_RELEASE name "$linux_cv_k_release" is mangled.
*** Try specifiying a 2.4 or 2.6 kernel with the --with-k-release option
*** to configure.  If you are trying to compile for a 2.2, 2.3, 2.5 or
*** 2.7 kernel, give up.  Only 2.4 and 2.6 kernels are supported at the
*** current time.
*** ])
    fi
    AC_CACHE_CHECK([for kernel patch release number], [linux_cv_k_patch], [dnl
	linux_cv_k_patch="`echo $linux_cv_k_release | sed -e 's|[[0-9]]*\.[[0-9]]*\.||;s|[[^0-9]].*||'`" ])
    if test ${linux_cv_k_minor:-0} -eq 4 -a ${linux_cv_k_patch:-0} -le 10
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
    kmajor="${linux_cv_k_major}"
    kminor="${linux_cv_k_minor}"
    kpatch="${linux_cv_k_patch}"
    knumber="${kmajor}.${kminor}.${kpatch}"
    AC_SUBST([kmajor])dnl
    AC_SUBST([kminor])dnl
    AC_SUBST([kpatch])dnl
    AC_SUBST([knumber])dnl
    if test "$linux_cv_k_minor" -eq 4
    then
	AC_DEFINE_UNQUOTED([LINUX_2_4], [1], [Define for the linux 2.4 kernel series.])
    fi
    if test "$linux_cv_k_minor" -eq 6
    then
	AC_DEFINE_UNQUOTED([LINUX_2_6], [1], [Define for the linux 2.6 kernel series.])
    fi
    if test "$linux_cv_k_major" -eq 2 -a \( "$linux_cv_k_minor" -gt 5 -o "$linux_cv_k_patch" -ge 48 \)
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
    AM_CONDITIONAL([WITH_LINUX_2_4], [test $linux_cv_k_minor -eq 4])
    AM_CONDITIONAL([WITH_LINUX_2_6], [test $linux_cv_k_minor -eq 6])
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
	    AS_HELP_STRING([--with-k-linkage=LINKAGE],
		[specify the kernel LINKAGE, either 'loadable' for loadable modules or 'linkable'
		for linkable objects.  @<:@default=loadable@:>@]), [dnl
	    with_k_linkage="$withval"
	    case :${with_k_linkage:-loadable} in
		(:linkable|:objects)   with_k_linkage='linkable' ;;
		(:loadable|:modules|:*) with_k_linkage='loadable';;
	    esac],
	    [with_k_linkage='loadable'])
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
    AC_ARG_VAR([DEPMOD], [Build kernel module dependencies command])
    AC_PATH_PROG([DEPMOD], [depmod], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test "${DEPMOD:-no}" = :no ; then
	AC_MSG_WARN([Could not find depmod program in PATH.])
	DEPMOD=/sbin/depmod
    fi
    AC_ARG_VAR([MODPROBE], [Probe kernel module dependencies command])
    AC_PATH_PROG([MODPROBE], [modprobe], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test "${MODPROBE:-no}" = :no ; then
	AC_MSG_WARN([Could not find depmod program in PATH.])
	MODPROBE=/sbin/modprobe
    fi
    AC_ARG_VAR([LSMOD], [List kernel modules command])
    AC_PATH_PROG([LSMOD], [lsmod], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test "${LSMOD:-no}" = :no ; then
	AC_MSG_WARN([Could not find lsmod program in PATH.])
	LSMOD=/sbin/lsmod
    fi
    AC_ARG_VAR([LSOF], [List open files command])
    AC_PATH_PROG([LSOF], [lsof], [],
		 [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test "${LSOF:-no}" = :no ; then
	AC_MSG_WARN([Could not find lsof program in PATH.])
	LSOF=/sbin/lsof
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
    AC_CACHE_CHECK([for kernel modules install directory], [linux_cv_k_modules_eval], [dnl
	AC_ARG_WITH([k-modules],
	    AS_HELP_STRING([--with-k-modules=DIR],
		[specify the directory to which kernel modules will be installed.
		@<:@default=/lib/modules/K-RELEASE/misc@:>@]),
	    [with_k_modules="$withval"],
	    [with_k_modules=])
	if test :"${with_k_modules:-no}" != :no
	then
	    linux_cv_k_modules="$with_k_modules"
	else
	    linux_cv_k_modules='${rootdir}/lib/modules/${kversion}'
	fi
	eval "linux_cv_k_modules_eval=\"$linux_cv_k_modules\"" ])
    AC_CACHE_CHECK([for kernel modules directory], [linux_cv_k_moddir], [dnl
	AC_MSG_RESULT([searching...])
	if test :"${with_k_modules:-no}" != :no
	then
	    linux_cv_k_moddir="$with_k_modules"
	else
	    eval "k_moddir_search_path=\"
		${DESTDIR}${rootdir}/lib/modules/${kversion}
		${DESTDIR}/lib/modules/${kversion}\""
	    k_moddir_search_path=`echo "$k_moddir_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    linux_cv_k_moddir=
	    for linux_dir in $k_moddir_search_path
	    do
		AC_MSG_CHECKING([for kernel modules directory... $linux_dir])
		if test -d "$linux_dir" -a -f "$linux_dir/modules.dep"
		then
		    linux_cv_k_moddir="$linux_dir"
		    AC_MSG_RESULT([yes])
		    break;
		fi
		AC_MSG_RESULT([no])
	    done
	fi
	if test :"${linux_cv_k_moddir:-no}" = :no -o ! -d "$linux_cv_k_moddir"
	then
	    AC_MSG_WARN([
***
*** Strange, the modules directory is $linux_cv_k_modules_eval
*** but the file modules.dep could not be found in
***	"$with_k_modules"
***	"$k_moddir_search_path"
***
*** Check the settings of the following options before repeating:
***	--with-k-release ${kversion:-no}
***	--with-k-modules ${kmoduledir:-no}
***
*** This could cause some problems later.
*** ])
	fi
	AC_MSG_CHECKING([for kernel modules directory])
    ])
    kmoddir="$linux_cv_k_moddir"
    AC_SUBST([kmoddir])dnl
    AC_CACHE_CHECK([for kernel module compression], [linux_cv_k_compress], [dnl
	if test -r $linux_cv_k_moddir/modules.dep
	then
	    if ( egrep -q '\.(k)?o\.gz:' $linux_cv_k_moddir/modules.dep )
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
*** Strange, the modules directory is $linux_cv_k_moddir
*** but the file $linux_cv_k_moddir/modules.dep
*** does not exist.  This could cause some problems later.
*** ])
	    ;;
    esac
    AC_SUBST([COMPRESS_KERNEL_MODULES])dnl
    AC_SUBST([kzip])dnl
    kmoduledir="${linux_cv_k_modules}"
    AC_SUBST([kmoduledir])dnl
])# _LINUX_CHECK_KERNEL_MODULES
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
	    (centos|whitebox|redhat)
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
	    (debian|ubuntu)
		case "${kversion}" in
		    # debian boot kernels
		    (*)		    linux_cv_k_boot=		;;
		esac
		linux_cv_k_base="$kversion"
		;;
	    (*)
		linux_cv_k_boot=no
		linux_cv_k_base="$kversion"
	esac
	kbase="$linux_cv_k_base"
dnl 	if test :"$linux_cv_k_boot" != :no
dnl 	then
dnl 	    kboot="${linux_cv_k_boot:-UP}"
dnl 	else
dnl 	    kboot=
dnl 	fi
dnl 	if test :"$linux_cv_k_boot" != :no
dnl 	then
dnl dnl	    not all combinations are valid
dnl 	    kboot="$linux_cv_k_boot"
dnl 	    eval "linux_tmp=\"$linux_cv_k_config_spec\""
dnl 	    linux_tmp=`echo "$linux_tmp" | sed -e 's|\<NONE\>||g;s|//|/|g'`
dnl 	    if test ! -f $linux_tmp
dnl 	    then
dnl 		if test -n "$kboot"
dnl 		then
dnl 		    AC_MSG_ERROR([
dnl *** 
dnl *** It appears as though you have a Redhat/Mandrake boot (production) kernel,
dnl *** however, the kernel configuration file
dnl ***	"$kconfig"
dnl *** does not exist on your system.  This can happen in a couple of situations:
dnl *** 
dnl ***   (1) you have specified a boot kernel version ($kversion),
dnl ***       however, the kernel architecture ($karch) is wrong for the kernel.
dnl ***       (This can happen sometimes in an automated build.)
dnl *** 
dnl ***   (2) you have specified a kernel version ($kversion)
dnl ***       that ends in a suffix ($kboot) normally used by boot kernels,
dnl ***       but it is not a boot kernel.
dnl *** 
dnl *** Configure cannot proceed reliably under these conditions.
dnl *** ])
dnl 		else
dnl 		    AC_MSG_WARN([
dnl *** 
dnl *** It appears as though you have a Redhat/Mandrake boot (production) kernel
dnl *** based system, however, kernel configuration file
dnl *** 
dnl ***	"$linux_tmp"
dnl *** 
dnl *** does not exist on your system.  This can happen if you are running a
dnl *** kernel.org kernel on an RedHat or Mandrake based system.  Configure is
dnl *** proceeding under the assumption that this is NOT a boot kernel.  This may
dnl *** cause problems later.
dnl *** ])
dnl 		fi
dnl 		linux_cv_k_boot=no
dnl 		kboot=
dnl 		linux_cv_k_base="$kversion"
dnl 		kbase="$linux_cv_k_base"
dnl 	    fi
dnl 	fi
dnl dnl
dnl dnl	We make another series of checks here because there seem to be some
dnl dnl	people that cannot install a kernel rpm properly.
dnl dnl
dnl 	if test :"$linux_cv_k_boot" != :no
dnl 	then
dnl 	    linux_tmp="${DESTDIR}/lib/modules/${kversion}/build"
dnl 	    if test "$kbuilddir" != "$linux_tmp"
dnl 	    then
dnl 		if test -d "$linux_tmp"
dnl 		then
dnl 		    AC_MSG_ERROR([*** configure error])
dnl 		else
dnl 		    AC_MSG_ERROR([
dnl *** 
dnl *** Configure has detected a problem with your system.  Building for a
dnl *** "$dist_cv_host_distrib" "${kboot:-UP}" boot kernel, but the directory:
dnl *** 
dnl ***	"$linux_tmp"
dnl *** 
dnl *** does not exist.  Please install kernel${kboot:+-$kboot}-${kbase}.${karch}.rpm
dnl *** and kernel-source-${kbase}.${kmarch}.rpm correctly.
dnl *** ])
dnl 		fi
dnl 	    fi
dnl 	    linux_tmp="${DESTDIR}/boot/System.map-${kversion}"
dnl 	    if test "$ksysmap" != "$linux_tmp"
dnl 	    then
dnl 		if test -f "$linux_tmp"
dnl 		then
dnl 		    AC_MSG_ERROR([*** configure error])
dnl 		else
dnl 		    AC_MSG_ERROR([
dnl *** 
dnl *** Configure has detected a problem with your system.  Building for a
dnl *** "$dist_cv_host_distrib" "${kboot:-UP}" boot kernel, but the file:
dnl *** 
dnl ***	"$linux_tmp"
dnl *** 
dnl *** does not exist.  Please install kernel${kboot:+-$kboot}-${kbase}.${karch}.rpm
dnl *** correctly.
dnl *** ])
dnl 		fi
dnl 	    else
dnl 		case "$target_vendor" in
dnl 		    (redhat|centos|whitebox)
dnl dnl
dnl dnl			Unfortunately the redhat system map files are unreliable
dnl dnl			because the are not unique for each architecture.
dnl dnl
dnl 			linux_opt=
dnl 			test -n "${DESTDIR}" && linux_opt="--root ${DESTDIR}"
dnl 			linux_arch=`rpm -q --whatprovides $linux_tmp --qf "%{ARCH}\n" 2>/dev/null`
dnl 			if test "$linux_arch" != "$karch"
dnl 			then
dnl 			    AC_MSG_ERROR([
dnl *** 
dnl *** The system map file for "$dist_cv_host_distrib" "${kboot:-UP}" is
dnl *** 
dnl ***	"$linux_tmp"
dnl *** 
dnl *** but the architecture of that file is "$linux_arch" instead of "$karch".
dnl *** Reconfigure with "--target=${linux_arch}-${target_vendor}-${target_os}".
dnl *** 
dnl *** ])
dnl 			fi
dnl 			;;
dnl 		    (mandrake)
dnl 			;;
dnl 		    (*)
dnl 			;;
dnl 		esac
dnl 	    fi
dnl 	fi
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

# =========================================================================
# _LINUX_CHECK_KERNEL_BUILDDIR
# -------------------------------------------------------------------------
# The linux kernel build directory is the build kernel tree root against which
# kernel modules will be compiled.  In previous 2.4 systems this and the
# kernel source tree are the same.  In 2.6 based systems this build directory
# and the source directory can be different.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_BUILDDIR], [dnl
    AC_CACHE_CHECK([for kernel build directory], [linux_cv_k_build], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([k-build],
	    AS_HELP_STRING([--with-k-build=DIR],
		[specify the base kernel build directory in which configured
		kernel source resides.  @<:@default=K-MODULES-DIR/build@:>@]),
	    [with_k_build="$withval"],
	    [with_k_build=])
	if test :"${with_k_build:-no}" != :no
	then
	    linux_cv_k_build="$with_k_build"
	else
	    eval "k_build_search_path=\"
		${kmoduledir:+${DESTDIR}${kmoduledir}/build}
		${DESTDIR}${rootdir}/lib/modules/${kversion}/build
		${DESTDIR}${rootdir}/usr/src/kernels/${kversion}-${kmarch}
		${DESTDIR}${rootdir}/usr/src/kernels/${kversion}
		${DESTDIR}${rootdir}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}
		${DESTDIR}${rootdir}/usr/src/linux-obj
		${DESTDIR}${rootdir}/usr/src/kernel-headers-${kversion}
		${DESTDIR}${rootdir}/usr/src/kernel-headers-${knumber}
		${DESTDIR}${rootdir}/usr/src/linux-headers-${kversion}
		${DESTDIR}${rootdir}/usr/src/linux-headers-${knumber}
		${DESTDIR}${rootdir}/usr/src/linux-${kversion}
		${DESTDIR}${rootdir}/usr/src/linux-${kbase}
		${DESTDIR}${rootdir}/usr/src/linux-${knumber}
		${DESTDIR}${rootdir}/usr/src/linux-${kmajor}.${kminor}
		${DESTDIR}${rootdir}/usr/src/linux
		${DESTDIR}/lib/modules/${kversion}/build
		${DESTDIR}/usr/src/kernels/${kversion}-${kmarch}
		${DESTDIR}/usr/src/kernels/${kversion}
		${DESTDIR}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}
		${DESTDIR}/usr/src/linux-obj
		${DESTDIR}/usr/src/kernel-headers-${kversion}
		${DESTDIR}/usr/src/kernel-headers-${knumber}
		${DESTDIR}/usr/src/linux-headers-${kversion}
		${DESTDIR}/usr/src/linux-headers-${knumber}
		${DESTDIR}/usr/src/linux-${kversion}
		${DESTDIR}/usr/src/linux-${kbase}
		${DESTDIR}/usr/src/linux-${kmajor}.${kminor}
		${DESTDIR}/usr/src/linux\""
	    k_build_search_path=`echo "$k_build_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    linux_cv_k_build=
	    for linux_dir in $k_build_search_path ; do
		AC_MSG_CHECKING([for kernel build directory... $linux_dir])
		if test -d "$linux_dir" -a \
			-d "$linux_dir/include" -a \
			-d "$linux_dir/include/linux" -a \
			-f "$linux_dir/include/linux/autoconf.h" -a \
			-f "$linux_dir/include/linux/version.h"
		then
		    linux_cv_k_build="$linux_dir"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	fi
	if test :"${linux_cv_k_build:-no}" = :no -o ! -d "$linux_cv_k_build" 
	then
	    if test :"${linux_cv_k_build:-no}" = :no 
	    then
		AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel-source or kernel-headers package for the
*** target kernel "${kversion}" and then configure again.
*** 
*** The following directories do no exist in the build environment:
***	"$with_k_build"
***	"$k_build_search_path"
*** 
*** Check the settings of the following options before repeating:
***     --with-k-release ${kversion:-no}
***     --with-k-modules ${kmoduledir:-no}
***     --with-k-build   ${kversion:-no}
*** ])
	    else
		if ! -d "$linux_cv_k_build" 
		then
		    AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel-source or kernel-headers package for the
*** target kernel "${kversion}" and then configure again.
*** 
*** The following directories do not exist in the build environment:
***     ${linux_cv_k_build}
*** 
*** Check the settings of the following options before repeating:
***     --with-k-release ${kversion:-no}
***     --with-k-modules ${kmoduledir:-no}
***     --with-k-build   ${with_k_build:-no}
*** ])
		fi
	    fi
	fi
	AC_MSG_CHECKING([for kernel build directory]) ])
    kbuilddir="$linux_cv_k_build"
    AC_SUBST([kbuilddir])dnl
])# _LINUX_CHECK_KERNEL_BUILDDIR
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
    AC_CACHE_CHECK([for kernel source directory], [linux_cv_k_source], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([k-source],
	    AS_HELP_STRING([--with-k-source=DIR],
		[specify the base kernel source directory in which source
		resides separate from the build directory.
		@<:@default=/usr/src/K_VERSION@:>@]),
	    [with_k_source="$withval"],
	    [with_k_source=])
	if test :"${with_k_souce:-no}" != :no
	then
	    linux_cv_k_source="$with_k_source"
	else
	    eval "k_source_search_path=\"
		${kbuilddir}
		${DESTDIR}${rootdir}/usr/src/kernel-source-${knumber}
		${DESTDIR}${rootdir}/usr/src/linux-source-${knumber}
		${DESTDIR}${rootdir}/usr/src/linux-${kversion}
		${DESTDIR}${rootdir}/usr/src/linux-${kbase}
		${DESTDIR}${rootdir}/usr/src/linux-${knumber}
		${DESTDIR}${rootdir}/usr/src/linux-${kmajor}.${kminor}
		${DESTDIR}${rootdir}/usr/src/linux
		${DESTDIR}${rootdir}/lib/modules/${kversion}/source
		${DESTDIR}${rootdir}/usr/src/kernels/${kversion}-${kmarch}
		${DESTDIR}${rootdir}/usr/src/kernels/${kversion}
		${DESTDIR}/usr/src/kernel-source-${knumber}
		${DESTDIR}/usr/src/linux-source-${knumber}
		${DESTDIR}/usr/src/linux-${kversion}
		${DESTDIR}/usr/src/linux-${kbase}
		${DESTDIR}/usr/src/linux-${knumber}
		${DESTDIR}/usr/src/linux-${kmajor}.${kminor}
		${DESTDIR}/usr/src/linux
		${DESTDIR}/lib/modules/${kversion}/source
		${DESTDIR}/usr/src/kernels/${kversion}-${kmarch}
		${DESTDIR}/usr/src/kernels/${kversion}\""
	    k_source_search_path=`echo "$k_source_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g;s|/\./|/|g'`
	    linux_cv_k_source=
	    for linux_dir in $k_source_search_path ; do
		AC_MSG_CHECKING([for kernel source directory... $linux_dir])
		if test -d "$linux_dir" -a \
			-f "$linux_dir/Makefile" -a \
			-d "$linux_dir/kernel" -a \
			-f "$linux_dir/kernel/sched.c"
		then
		    linux_cv_k_source="$linux_dir"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	    if test -z "$linux_cv_k_source"
	    then
		linux_cv_k_source="$linux_cv_k_build"
	    fi
	fi
	AC_MSG_CHECKING([for kernel source directory]) ])
    ksrcdir="$linux_cv_k_source"
    AC_SUBST([ksrcdir])dnl
])# _LINUX_CHECK_KERNEL_SRCDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MODVER
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MODVER], [dnl
    AC_CACHE_CHECK([for kernel module ver], [linux_cv_k_modver], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([k-modver],
	    AS_HELP_STRING([--with-k-modver=MAP],
		[specify the kernel module symbol versions file.
		@<:@default=K-BUILD-DIR/Module.symvers@:>@]),
	    [with_k_modver="$withval"],
	    [with_k_modver=])
	if test :"${with_k_modver:-no}" != :no
	then
	    linux_cv_k_modver="$with_k_modver"
	else
	    eval "k_modver_search_path=\"
		${DESTDIR}${rootdir}/usr/src/kernels/${kversion}-${kmarch}/Module.symvers
		${DESTDIR}${rootdir}/usr/src/kernels/${kversion}/Module.symvers
		${kbuilddir}/Module.symvers-${kversion}
		${kbuilddir}/Module.symvers
		${DESTDIR}${rootdir}/boot/Module.symvers-${kversion}
		${DESTDIR}${rootdir}/boot/Module.symvers
		${DESTDIR}/usr/src/kernels/${kversion}-${kmarch}/Module.symvers
		${DESTDIR}/usr/src/kernels/${kversion}/Module.symvers
		${DESTDIR}/boot/Module.symvers-${kversion}
		${DESTDIR}/boot/Module.symvers\""
	    k_modver_search_path=`echo "$k_modver_search_path" | sed -e 's|\<NONE\>||g;s|/\./|/|g;s|//|/|g'`
	    linux_cv_k_modver=
	    for linux_file in $k_modver_search_path ; do
		AC_MSG_CHECKING([for kernel module ver... $linux_file])
		if test -r $linux_file
		then
		    linux_cv_k_modver="$linux_file"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	fi
	if test :"${linux_cv_k_modver:-no}" = :no
	then
	    if test :"${linux_cv_k_ko_modules:-no}" != :no
	    then
		if test :"${linux_cv_k_versions}" != :no -a ":${linux_cv_k_modversions}" != :no
		then
		    AC_MSG_WARN([
*** 
*** Configure could not find the module versions file for kernel version
*** "$kversion".  The locations searched were:
***	    "$with_k_modver"
***	    "$k_modver_search_path"
*** 
*** This can cause problems later.  Please specify the absolute location
*** of your kernel's module versions file with option --with-k-modver
*** before repeating.
*** ])
		fi
	    fi
	else
	    if test :"$linux_cv_k_running" != :yes
	    then
		case "$linux_cv_k_modver" in
		    (*/usr/src/kernels/*)
			;;
		    (*/boot/*|*/usr/src/*|*/lib/modules/*)
			case "$target_vendor" in
			    (mandrake)
				;;
			    (redhat|centos|whitebox|debian|ubuntu|suse|*)
				AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
*** 
***	"$linux_cv_k_modver"
*** 
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
				;;
			esac
			;;
		esac
	    fi
	fi
	AC_MSG_CHECKING([for kernel module ver]) ])
    kmodver="$linux_cv_k_modver"
    AC_SUBST([kmodver])dnl
])# _LINUX_CHECK_KERNEL_MODVER
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_SYSMAP
# -------------------------------------------------------------------------
# The linux kernel system map is only used for configuration and is not used
# otherwise.  This has to be the system map for the kernel build or for the
# running kernel.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_SYSMAP], [dnl
    AC_CACHE_CHECK([for kernel system map], [linux_cv_k_sysmap], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([k-sysmap],
	    AS_HELP_STRING([--with-k-sysmap=MAP],
		[specify the kernel system map file.
		@<:@default=K-BUILD-DIR/System.map@:>@]),
	    [with_k_sysmap="$withval"],
	    [with_k_sysmap=])
	if test :"${with_k_sysmap:-no}" != :no
	then
	    linux_cv_k_sysmap="$with_k_sysmap"
	else
dnl
dnl	    There is a problem with using redhat /boot system maps.  Redhat builds multiple kernels
dnl	    of the same name for different architectures.  So, for example, there is a 2.4.21-15.EL
dnl	    kernel built for i386, another built for i586, and yet another built for i686.  These
dnl	    all have different system map files, but only one is installed on a system.
dnl
dnl	    There is also a problem here with debian, the deb has a different system map for each
dnl	    kernel architecture (i.e. System.map-2.4.18-1-686 and System.map-2.4.18-1-k7), but the
dnl	    version number is not included in the filename.  So, the version on one of these files
dnl	    could be 2.4.18-13.1, another could be 2.4.18-12 and they do not necessarily match the
dnl	    running kernel.  In fact, on debian, there is no way to tell which exact version of the
dnl	    kernel is running.  Ubuntu understandably has the same problem.
dnl
	    eval "k_sysmap_search_path=\"
		${kbuilddir}/System.map-${kversion}
		${kbuilddir}/System.map
		${DESTDIR}${rootdir}/boot/System.map-${kversion}
		${DESTDIR}${rootdir}/boot/System.map
		${DESTDIR}/boot/System.map-${kversion}
		${DESTDIR}/boot/System.map\""
	    k_sysmap_search_path=`echo "$k_sysmap_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	    linux_cv_k_sysmap=
	    for linux_file in $k_sysmap_search_path ; do
		AC_MSG_CHECKING([for kernel system map... $linux_file])
		if test -r $linux_file 
		then
		    linux_cv_k_sysmap="$linux_file"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	fi
	if test :${linux_cv_k_sysmap:-no} = :no
	then
	    AC_MSG_ERROR([
*** 
*** Configure could not find the system map file for kernel version
*** "$kversion".  The locations searched were:
***	    "$with_k_sysmap"
***	    "$k_sysmap_search_path"
*** 
*** Please specify the absolute location of your kernel's system map
*** file with option --with-k-sysmap before repeating.
*** ])
	else
	    if test :$linux_cv_k_running != :yes
	    then
		case "$linux_cv_k_sysmap" in
		    (*/usr/src/kernels/*)
			;;
		    (*/boot/*|*/usr/src/*|*/lib/modules/*)
			case "$target_vendor" in
			    (mandrake)
dnl
dnl				Mandrakelinux blends the debian architecture name in the kernel
dnl				image name approach with the Redhat kernel version number in the
dnl				kernel image name approach to yeild reliable system map files.
dnl
				;;
			    (redhat|centos|whitebox|debian|ubuntu|suse|*)
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
***	"$linux_cv_k_sysmap"
*** 
*** This may cause problems later if you have mismatches between the
*** target kernel and the kernel symbols contained in that file.
*** ])
				;;
			esac
			;;
		esac
	    fi
	fi
	AC_MSG_CHECKING([for kernel system map]) ])
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
# _LINUX_CHECK_KERNEL_MINORBITS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MINORBITS], [dnl
    AC_CACHE_CHECK([for kernel minor device number bits], [linux_cv_minorbits], [dnl
dnl
dnl Just grep linux/kdev_t.h for #define MINORBITS.
dnl
	eval "linux_dirs=\"${kbuilddir} ${ksrcdir}\""
	for linux_dir in $linux_dirs
	do
	    linux_file="$linux_dir/include/linux/kdev_t.h"
	    if test -f $linux_file
	    then
		linux_line="`egrep '^#[[:space:]]*define[[:space:]][[:space:]]*MINORBITS[[:space:]]' $linux_file | head -1`"
		linux_rslt="`echo \"$linux_line\" | sed -e 's|^#[[:space:]]*define[[:space:]][[:space:]]*MINORBITS[[:space:]][[:space:]]*\([1-9][0-9]*\)[[:space:]]*$|\1|'`"
		if test "${linux_rslt:-0}" -ge 8 -a "${linux_rslt:-0}" -le 20
		then
		    linux_cv_minorbits="$linux_rslt"
		fi
		break
	    fi
	done
    ])
    if test :"$linux_cv_minorbits" = :
    then
	AC_MSG_WARN([
*** 
*** Configure could not determine the number of minor bits in a minor
*** device number for your kernel.  Configure is assuming that the
*** kernel only has 8-bits per minor device number.  This could cause
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
	if test :"$linux_cv_k_running" = :yes
	then
dnl
dnl Ubuntu stuck some more paretheses into their /proc/version string.
dnl Instead of keying off of the last parentheses in the string, use the key
dnl string containing two parentheses: this is common to all.
dnl
	    linux_cv_k_compiler=`cat /proc/version | sed -e 's,^.*(gcc version,gcc version,;s,)).*[$],),' 2>/dev/null`
	else
dnl
dnl	    not all distros leave this hanging around
dnl
	    eval "linux_dirs=\"${kbuilddir} ${ksrcdir}\""
	    for linux_dir in $linux_dirs
	    do
		linux_file="$linux_dir/include/linux/compile.h"
		if test -f $linux_file
		then
		    linux_cv_k_compiler=`grep LINUX_COMPILER $linux_file | sed -e 's,^.*gcc version,gcc version,;s,"[[^"]]*[$],,' 2>/dev/null`
		    break
		else
		    eval "linux_dir=\"$kmoduledir/kernel/arch\""
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
		fi
	    done
	fi
	linux_cv_compiler=`$CC -v 2>&1 | grep 'gcc version'`
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
    AC_CACHE_CHECK([for kernel arch directory], [linux_cv_k_archdir], [dnl
	AC_ARG_WITH([k-archdir],
	    AS_HELP_STRING([--with-k-archdir=DIR],
		[specify the kernel source architecture specific directory.
		@<:@default=K-BUILD-DIR/arch@:>@]),
	    [with_k_archdir="$withval"],
	    [with_k_archdir=])
	if test :"${with_k_archdir:-no}" != :no
	then
	    linux_cv_k_archdir="$with_k_archdir"
	else
	    linux_cv_k_archdir="${ksrcdir}/arch"
	fi
	if test :"${linux_cv_k_archdir:-no}" != :no -a ! -d "$linux_cv_k_archdir"
	then
	    AC_MSG_ERROR([
***
*** Kernel architecture directory:
***     "$linux_cv_k_archdir"
*** does not exist.  Specify the correct kernel architecture directory
*** using the --with-k-archdir option to configure before attempting
*** again.
*** ])
	fi])dnl
])# _LINUX_CHECK_KERNEL_ARCHDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_MACHDIR
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_MACHDIR], [dnl
    AC_CACHE_CHECK([for kernel machine base], [linux_cv_k_mach], [dnl
	case "$karch" in
	    (alpha*)		linux_cv_k_mach="alpha"	 ;;
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
dnl
dnl This sets up a local include2 directory with a symbolic link from asm to the
dnl machine directory.  This is only for Linux 2.6 kbuild kernels so that we can
dnl strip flags from the kernel makefiles.
dnl
    test -d include2 || mkdir include2
    ln -snf "${ksrcdir}/include/asm-${linux_cv_k_mach}" include2/asm
dnl
    AC_CACHE_CHECK([for kernel mach directory], [linux_cv_k_machdir], [dnl
	AC_ARG_WITH([k-machdir],
	    AS_HELP_STRING([--with-k-machdir=DIR],
		[specify the kernel source machine specific directory.
		@<:@default=K-ARCHDIR/ARCH@:>@]),
	    [with_k_machdir="$withval"],
	    [with_k_machdir=])
	if test :"${with_k_machdir:-no}" != :no
	then
	    linux_cv_k_machdir="$with_k_machdir"
	else
	    linux_cv_k_machdir="${ksrcdir}/arch/${linux_cv_k_mach}"
	fi
	if test :"${linux_cv_k_machdir:-no}" != :no -a \
	    \( ! -d "$linux_cv_k_machdir" -o ! -f "$linux_cv_k_machdir/Makefile" \)
	then
	    AC_MSG_ERROR([
***
*** Kernel machine directory:
***     "$linux_cv_k_machdir"
*** does not exist, or there is no Makefile in the directory.  Specify
*** the correct kernel machine directory using the --with-k-machdir
*** option to configure before attempting again.
*** ])
	fi ])
    kmachdir="$linux_cv_k_machdir"
    AC_SUBST([kmachdir])dnl
])# _LINUX_CHECK_KERNEL_MACHDIR
# =========================================================================

# =========================================================================
# _LINUX_CHECK_KERNEL_DOT_CONFIG
# -------------------------------------------------------------------------
# Find the kernel .config file so that we can use it to generate CFLAGS.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_KERNEL_DOT_CONFIG], [dnl
    AC_CACHE_CHECK([for kernel config file], [linux_cv_k_config], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([k-config],
	    AS_HELP_STRING([--with-k-config=CONFIG],
		[specify the kernel configuration file.
		@<:@default=BOOT/config-K-RELEASE@:>@]),
	    [with_k_config="$withval"],
	    [with_k_config=])
	if test :"${with_k_config:-no}" != :no
	then
	    linux_cv_k_config="$with_k_config"
	else
	    if test :"$linux_cv_k_running" = :yes
	    then
		eval "k_config_search_path=\"
		    ${ksrcdir}/configs/kernel-${knumber}-${karch}${kboot:+-}${kboot}.config
		    ${kboot:+${kmachdir}/defconfig${kboot:+-}${kboot}}
		    ${DESTDIR}${rootdir}/usr/src/kernels/${kversion}-${kmarch}/.config
		    ${DESTDIR}${rootdir}/usr/src/kernels/${kversion}/.config
		    ${DESTDIR}${rootdir}/usr/src/kernel-headers-${kversion}/.config
		    ${DESTDIR}${rootdir}/usr/src/linux-headers-${kversion}/.config
		    ${DESTDIR}${rootdir}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}/.config
		    ${DESTDIR}${rootdir}/usr/src/linux-obj/.config
		    ${DESTDIR}${rootdir}/boot/config-${kversion}
		    ${DESTDIR}${rootdir}/boot/config
		    ${kbuilddir}/.config
		    ${kboot:-${kmachdir}/defconfig${kboot:+-}${kboot}}
		    ${DESTDIR}/usr/src/kernels/${kversion}-${kmarch}/.config
		    ${DESTDIR}/usr/src/kernels/${kversion}/.config
		    ${DESTDIR}/usr/src/kernel-headers-${kversion}/.config
		    ${DESTDIR}/usr/src/linux-headers-${kversion}/.config
		    ${DESTDIR}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}/.config
		    ${DESTDIR}/usr/src/linux-obj/.config
		    ${DESTDIR}/boot/config-${kversion}
		    ${DESTDIR}/boot/config
		    ${ksrcdir}/.config\""
	    else
		eval "k_config_search_path=\"
		    ${ksrcdir}/configs/kernel-${knumber}-${karch}${kboot:+-}${kboot}.config
		    ${kboot:+${kmachdir}/defconfig${kboot:+-}${kboot}}
		    ${DESTDIR}${rootdir}/usr/src/kernels/${kversion}-${kmarch}/.config
		    ${DESTDIR}${rootdir}/usr/src/kernels/${kversion}/.config
		    ${DESTDIR}${rootdir}/usr/src/kernel-headers-${kversion}/.config
		    ${DESTDIR}${rootdir}/usr/src/linux-headers-${kversion}/.config
		    ${DESTDIR}${rootdir}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}/.config
		    ${DESTDIR}${rootdir}/usr/src/linux-obj/.config
		    ${kbuilddir}/.config
		    ${kboot:-${kmachdir}/defconfig${kboot:+-}${kboot}}
		    ${DESTDIR}/usr/src/kernels/${kversion}-${kmarch}/.config
		    ${DESTDIR}/usr/src/kernels/${kversion}/.config
		    ${DESTDIR}/usr/src/kernel-headers-${kversion}/.config
		    ${DESTDIR}/usr/src/linux-headers-${kversion}/.config
		    ${DESTDIR}/usr/src/linux-${kbase}-obj/${kmarch}/${kboot}/.config
		    ${DESTDIR}/usr/src/linux-obj/.config
		    ${ksrcdir}/.config\""
	    fi
	    k_config_search_path=`echo "$k_config_search_path" | sed -e 's|\<NONE\>||g;s|/\./|/|g;s|//|/|g'`
dnl
dnl	If we are configuring for the running kernel and DESTDIR is null, we can check the
dnl	/boot directories.  Otherwise this is dangerous.  Redhat puts different configuration files
dnl	in the /boot directory depending upon which architecture of kernel (with the same name) is
dnl	being booted.  In these other cases, the kernel configuration needs to be obtained from the
dnl	kernel source package.  /usr/src/linux-2.4/configs/*.config for RedHat/Fedora/EL3 and
dnl	/usr/src/linux-2.4/arch/MACHDIR/defconfig-* for Mandrake.  Debian and Suse do not build
dnl	multiple kernels from the same source package and in their case the file will be in
dnl	/usr/src/kernel-headers-kversion/.config and /usr/src/linux-2.4/.config respectively.
dnl
	    linux_cv_k_config=
	    for linux_file in $k_config_search_path ; do
		AC_MSG_CHECKING([for kernel config file... $linux_file])
		if test -f $linux_file
		then
		    linux_cv_k_config="$linux_file"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	fi
	if test :${linux_cv_k_config:-no} = :no
	then
	    AC_MSG_ERROR([
*** 
*** Configure cannot find the kernel configuration file.  The
*** directories searched were:
***	"$with_k_config"
***	"$k_config_search_path"
*** 
*** Specify the absolute location of the kernel configuration files with
*** the --with-k-config option to configure before attempting again.
*** ])
	else
	    case "$linux_cv_k_config" in
		(*/configs/*|*/arch/*/defconf*|*/kernel-headers-*|*/linux-headers-*|*/usr/src/kernels/*)
		    ;;
		(*/boot/*|*/usr/src/*|*/lib/modules/*)
		    AC_MSG_WARN([
*** 
*** Configuration information is being read from an unreliable source:
*** 
***	"$linux_cv_k_config"
*** 
*** This may cause problems later if you have mismatches between the
*** target kernel and the configuration information contained in that
*** file.
*** ])
		    ;;
	    esac
	fi
	AC_MSG_CHECKING([for kernel config file]) ])
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
	linux_cv_k_version=unknown
	case "$target_vendor" in
	    (debian|ubuntu)
		linux_pkg=`dpkg -S $linux_cv_k_sysmap 2>/dev/null | cut -f1 -d:` || linux_pkg=
		if test -n "$linux_pkg" ; then
		    linux_ver=`dpkg -s "$linux_pkg" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || linux_ver=
		else
		    linux_ver=
		fi
		linux_cv_k_version="${linux_ver:-unknown}"
		;;
	    (mandrake|redhat|centos|whitebox|suse|*)
		linux_ver=`rpm -q --qf "%{VERSION}" --whatprovides $linux_cv_k_sysmap 2>/dev/null`
		linux_cv_k_version="${linux_ver:-unknown}"
		;;
	esac
    ])
    if test :"${linux_cv_k_version:-unknown}" != :unknown
    then
	krelease="$linux_cv_k_version"
    else
	krelease=
    fi
    AC_SUBST([krelease])
    case "$target_vendor" in
	(debian)
	    kernel_image='kernel-image'
	    kernel_source='kernel-source'
	    kernel_headers='kernel-headers'
	    ;;
	(ubuntu)
	    kernel_image='linux-image'
	    kernel_source='linux-source'
	    kernel_headers='linux-headers'
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
    AC_MSG_CHECKING([for kernel file sanity])
	eval "linux_cv_files=\"$linux_cv_k_sysmap $linux_cv_k_build $linux_cv_k_source $linux_cv_k_config\""
	case "$target_vendor" in
	    (mandrake)
dnl
dnl		Mandrakelinux is built correctly.
dnl
		;;
	    (debian|ubuntu)
dnl
dnl		Debian can have a mismatch in kernel version.
dnl
		linux_cv_vers=
		for linux_file in $linux_cv_files
		do
		    linux_pkg=`dpkg -S $linux_file 2>/dev/null | cut -f1 -d:` || linux_pkg=
		    if test -n "$linux_pkg" ; then
			linux_ver=`dpkg -s "$linux_pkg" 2>/dev/null | grep '^Version:' | cut -f2 '-d '` || linux_ver=
		    else
			linux_ver=
		    fi
		    linux_cv_vers="${linux_cv_vers:+$linux_cv_vers }'$linux_ver'"
		done
		;;
	    (redhat|centos|whitebox|suse)
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
	esac
    AC_MSG_RESULT([ok])
])# _LINUX_CHECK_KERNEL_FILES
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_CFLAGS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_CFLAGS], [dnl
    AC_CACHE_CHECK([for kernel CFLAGS], [linux_cv_k_cflags], [dnl
	if test :"${cross_compiling:-no}" = :no
	then
	    linux_tmp=""
	else
	    linux_tmp="CROSS_COMPILING=`dirname $CC` ARCH=${linux_cv_k_mach}"
	fi
	cp -f "$kconfig" .config
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
	linux_cv_k_cflags="`${srcdir}/scripts/cflagcheck ${linux_tmp:+$linux_tmp }KERNEL_CONFIG=${kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${ksrcdir} TOPDIR=${ksrcdir} KBUILD_SRC=${ksrcdir} -I${ksrcdir} cflag-check | tail -1`"
	linux_cv_k_cflags_orig="$linux_cv_k_cflags"
	rm -f .config
	linux_cflags=
	AC_ARG_WITH([k-optimize],
	    AS_HELP_STRING([--with-k-optimize=HOW],
		[specify optimization, normal, size, speed or quick.
		@<:@default=auto@:>@]),
	    [with_k_optimize="$withval"],
	    [with_k_optimize="$with_optimize"])

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
dnl		Try to pass through whatever optmization options are present.
dnl
		linux_cflags="$linux_cflags${linux_cflags:+ }"`echo " $linux_cv_k_cflags " | sed -e 's|^.* -O|-O|;s| .*$||'`
		linux_cflags="$linux_cflags${linux_cflags:+ }"`echo " $linux_cv_k_cflags " | sed -e 's|^.* -g|-g|;s| .*$||'`
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
	    linux_cflags="${linux_cflags}${linux_cflags:+ }-Werror"
	fi
	AC_ARG_ENABLE([k-inline],
	    AS_HELP_STRING([--enable-k-inline],
		[enable kernel inline functions.  @<:@default=no@:>@]),
	    [enable_k_inline="$enableval"],
	    [enable_k_inline='no'])
	if test :"${enable_k_inline:-no}" != :no 
	then
	    linux_cflags="$linux_cflags${linux_cflags:+ }-finline-functions"
	fi
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -Iinclude | -I${kbuilddir}/include |g"`
	eval "linux_dir=\"${kbuilddir}/include2\"" ; if test -d "$linux_dir" ; then
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -Iinclude2 | -I${kbuilddir}/include2 |g"`
	fi
dnl
dnl	Only ppc and um currently include architecture directories.  The rest include asm
dnl	directories.
dnl
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -Iinclude/asm| -I${ksrcdir}/include/asm|g"`
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -Iarch/| -I${ksrcdir}/arch/|g"`
	linux_cv_k_cflags=`echo "$linux_cv_k_cflags" | sed -e "s| -O[[0-9s]]* | $linux_cflags |"`
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
	cp -f "$kconfig" .config
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
	linux_cv_k_cppflags="`${srcdir}/scripts/cflagcheck KERNEL_CONFIG=${kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${ksrcdir} TOPDIR=${ksrcdir} KBUILD_SRC=${ksrcdir} -I${ksrcdir} cppflag-check | tail -1`"
	linux_cv_k_cppflags_orig="$linux_cv_k_cppflags"
	rm -f .config
	linux_cv_k_cppflags="-nostdinc -iwithprefix include -DLINUX $linux_cv_k_cppflags"
dnl
dnl	Need to adjust 2.6.3 kernel stupid include includes to add the absolute
dnl	location of the source directory.  include2 on the otherhand is properly
dnl	set up for the asm directory.
dnl
	linux_cv_k_cppflags="$linux_cv_k_cppflags $linux_cv_k_more_cppflags"
	linux_cv_k_cppflags=`echo "$linux_cv_k_cppflags" | sed -e "s| -Iinclude | -I${kbuilddir}/include |g"`
	eval "linux_dir=\"${kbuilddir}/include2\"" ; if test -d "$linux_dir" ; then
	linux_cv_k_cppflags=`echo "$linux_cv_k_cppflags" | sed -e "s| -Iinclude2 | -I${kbuilddir}/include2 |g"`
	fi
dnl
dnl	Only ppc and um currently include architecture directories.  The rest include asm
dnl	directories.
dnl
	linux_cv_k_cppflags=`echo "$linux_cv_k_cppflags" | sed -e "s| -Iinclude/asm| -I${ksrcdir}/include/asm|g"`
	linux_cv_k_cppflags=`echo "$linux_cv_k_cppflags" | sed -e "s| -Iarch/| -I${ksrcdir}/arch/|g"`
dnl
dnl	Recent 2.6.15+ kernels include autoconf.h from the build directory instead of the source
dnl	directory.  I suppose the idea is to allow you to configure in a separate directory as
dnl	well as build.  Given 100 years, kbuild might catch up to autoconf.  SLES 10 for some silly
dnl	reason expands the current working directory in the autoconf.h include, so watch out for
dnl	leading junk in the autoconf.h argument.
dnl
	linux_cv_k_cppflags=`echo "$linux_cv_k_cppflags" | sed -r -e "s| -include (\.*/.*/)?include/linux/autoconf.h| -include ${kbuilddir}/include/linux/autoconf.h|"`
dnl
dnl	Non-kbuild (2.4 kernel) always needs include directories to be in the
dnl	build directory.
dnl
	if test :"$linux_cv_k_ko_modules" != :yes ; then
	    eval "linux_src=\"$ksrcdir\""
	    eval "linux_bld=\"$kbuilddir\""
	    if test "$linux_src" != "$linux_bld" ; then
		linux_cv_k_cppflags=`echo "$linux_cv_k_cppflags" | sed -e "s| -I${ksrcdir}/include| -I${kbuilddir}/include|g"`
	    fi
	fi
    ])
    AC_CACHE_CHECK([for kernel MODFLAGS], [linux_cv_k_modflags], [dnl
	cp -f "$kconfig" .config
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
	linux_cv_k_modflags="`${srcdir}/scripts/cflagcheck KERNEL_CONFIG=${kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${ksrcdir} TOPDIR=${ksrcdir} KBUILD_SRC=${ksrcdir} -I${ksrcdir} modflag-check | tail -1`"
	linux_cv_k_modflags_orig="$linux_cv_k_modflags"
	rm -f .config
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
	    eval "linux_src=\"$ksrcdir\""
	    eval "linux_bld=\"$kbuilddir\""
	    if test "$linux_src" != "$linux_bld" ; then
		linux_cv_k_modflags=`echo "$linux_cv_k_modflags" | sed -e "s| ${ksrcdir}/include| ${kbuilddir}/include|g"`
	    fi
	fi
    ])
    AC_CACHE_CHECK([for kernel KBUILD_STR], [linux_cv_k_bldflags], [dnl
	cp -f "$kconfig" .config
dnl
dnl	On some later systems (noticed first on openSUSE 10.2, a mkmakefile script is making a dummy
dnl	makefile in the current directory, however, it is outputing 'GEN /current/directory/Makefile'
dnl	when it does it faking out the flags check.  Therefore, we now tail the output.  The
dnl	makefile is overwritten by config.status and the flags otherwise seem to be generated
dnl	correctly.
dnl
	linux_cv_k_bldflags="`${srcdir}/scripts/cflagcheck KERNEL_CONFIG=${kconfig} SPEC_CFLAGS='-g' KERNEL_TOPDIR=${ksrcdir} TOPDIR=${ksrcdir} KBUILD_SRC=${ksrcdir} -I${ksrcdir} bldflag-check | tail -1`"
	linux_cv_k_bldflags_orig="$linux_cv_k_bldflags"
	rm -f .config
dnl
dnl	As of 2.6.16+ the KBUILD_BASENAME is stringified on the command line
dnl	and is no longer stringified in kernel source files.  This Makefile
dnl	check checks to see if the KBUILD_STR symbol is defined by the
dnl	makefile, and, if so, stringifies the KBUILD_BASENAME's.  We used to
dnl	have -DKBUILD_BASENAME in the Makefile.am, but as it should always
dnl	contain a name unique for the object linked into a module, it can
dnl	always be derived from the compilation target, and so we do that here.
dnl
	case "$linux_cv_k_bldflags" in
	    (*KBUILD_STR*)
		linux_cv_k_bldflags="'-DKBUILD_STR(s)=\#s'"
		;;
	    (*)
		linux_cv_k_bldflags="'-DKBUILD_STR(s)=s'"
		;;
	esac
	# the escapes and quotes here are delicate: don't change them!
	linux_cv_k_bldflags="${linux_cv_k_bldflags} '-DKBUILD_BASENAME=KBUILD_STR('\`echo [\$][@] | sed -e 's,lib.*_a-,,;s,\.o,,;s,-,_,g'\`')'"
    ])
    CFLAGS="$linux_cv_k_cflags"
    CPPFLAGS="$linux_cv_k_cppflags"
    KERNEL_MODFLAGS="$linux_cv_k_modflags"
    AC_SUBST([KERNEL_MODFLAGS])dnl
    KERNEL_BLDFLAGS="$linux_cv_k_bldflags"
    AC_SUBST([KERNEL_BLDFLAGS])dnl
])# _LINUX_SETUP_KERNEL_CFLAGS
# =========================================================================

# =========================================================================
# _LINUX_SETUP_KERNEL_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_SETUP_KERNEL_DEBUG], [dnl
    AC_MSG_CHECKING([for kernel debugging])
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
    AC_ARG_ENABLE([k-none],
	AS_HELP_STRING([--enable-k-none],
	    [enable no kernel module run-time checks.  @<:@default=no@:>@]),
	[enable_k_none="$enableval"],
	[enable_k_none='no'])
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
    AS_IF([test AS_VAR_GET(linux_Member) = yes], [$2], [$3])dnl
    AS_VAR_POPDEF([linux_Member])dnl
])# _LINUX_CHECK_MEMBER_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MEMBERS_internal(AGGREGATE.MEMBER, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_MEMBERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MEMBERS_internal],
    [m4_foreach([LK_Member], [$1],
	[AH_TEMPLATE(AS_TR_CPP(HAVE_KMEMB_[]LK_Member),
		[Define to 1 if `]m4_bpatsubst(LK_Member, [^[^.]*\.])[' is member of `]m4_bpatsubst(LK_Member, [\..*])['.])
	 _LINUX_CHECK_MEMBER_internal(LK_Member,
	    [AC_DEFINE(AS_TR_CPP(HAVE_KMEMB_[]LK_Member), 1)
$2],
	    [$3],
	    [$4])])
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
AC_DEFUN([_LINUX_CHECK_MACRO_internal],
    [AS_VAR_PUSHDEF([linux_Macro], [linux_cv_macro_$1])dnl
    AC_CACHE_CHECK([for kernel macro $1], linux_Macro,
	[AC_EGREP_CPP([\<yes_we_have_$1_defined\>], [
[$4]

#ifdef $1
    yes_we_have_$1_defined
#endif
	],
	[AS_VAR_SET(linux_Macro, yes)],
	[AS_VAR_SET(linux_Macro, no)])])
    AS_IF([test :AS_VAR_GET(linux_Macro) = :yes], [$2], [$3])dnl
    AS_VAR_POPDEF([linux_Macro])dnl
])# _LINUX_CHECK_MACRO_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_MACROS_internal(MACROS, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNCS for macros
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_MACROS_internal],
[AC_FOREACH([LK_Macro], [$1],
  [AH_TEMPLATE(AS_TR_CPP(HAVE_KMACRO_[]LK_Macro),
	       [Define to 1 if kernel macro ]LK_Macro[() exists.])])dnl
for lk_macro in $1
do
    _LINUX_CHECK_MACRO_internal($lk_macro,
	[AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_KMACRO_$lk_macro]), 1)
$2],
	[$3],
	[$4])dnl
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
    AS_IF([test :AS_VAR_GET(linux_Function) = :yes], [$2], [$3])dnl
    AS_VAR_POPDEF([linux_Function])dnl
])# _LINUX_CHECK_FUNC_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_FUNCS_internal(FUNCTIONS, [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND], [INCLUDES])
# -----------------------------------------------------------------------------
# Kernel environment equivalent of AC_CHECK_FUNCS
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_FUNCS_internal],
[AC_FOREACH([LK_Function], [$1],
  [AH_TEMPLATE(AS_TR_CPP(HAVE_KFUNC_[]LK_Function),
	       [Define to 1 if kernel function ]LK_Function[() exists.])])dnl
for lk_func in $1
do
    _LINUX_CHECK_FUNC_internal($lk_func,
	[AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_KFUNC_$lk_func]), 1)
$2],
	[$3],
	[$4])dnl
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
AC_DEFUN([_LINUX_CHECK_TYPE_internal], [dnl
    AS_VAR_PUSHDEF([linux_Type], [linux_cv_type_$1])dnl
    AC_CACHE_CHECK([for kernel type $1], linux_Type,
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$4])],
[if (($1 *) 0)
    return 0;
if (sizeof ($1))
    return 0;])],
	[AS_VAR_SET(linux_Type, yes)],
	[AS_VAR_SET(linux_Type, no)])])
    AS_IF([test AS_VAR_GET(linux_Type) = yes], [$2], [$3])dnl
    AS_VAR_POPDEF([linux_Type])dnl
])# _LINUX_CHECK_TYPE_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_TYPES_internal(TYPES,
#       [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#       [INCLUDES = DEFAULT-INCLUDES])
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_TYPES_internal], [dnl
    m4_foreach([LK_Type], [$1],
	[AH_TEMPLATE(AS_TR_CPP(HAVE_KTYPE_[]LK_Type),
		[Define to 1 if the system has the type ']LK_Type['.])
	 _LINUX_CHECK_TYPE_internal(LK_Type,
	    [AC_DEFINE(AS_TR_CPP(HAVE_KTYPE_[]LK_Type), 1)
$2],
	    [$3],
	    [$4])])
])# _LINUX_CHECK_TYPES_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_TYPE
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_TYPE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_ENV([dnl
	_LINUX_CHECK_TYPE_internal([$1], [$2], [$3], [$4])])
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
    AS_IF([test AS_VAR_GET(linux_Header) = yes], [$2], [$3])dnl
    AS_VAR_POPDEF([linux_Header])dnl
])# _LINUX_CHECK_HEADER_internal
# =============================================================================

# =============================================================================
# _LINUX_CHECK_HEADERS_internal
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_CHECK_HEADERS_internal], [dnl
    AC_FOREACH([LK_Header], [$1],
	[AH_TEMPLATE(AS_TR_CPP(HAVE_KINC_[]LK_Header),
	    [Define to 1 if you have the <]LK_Header[> header file.])])
for lk_header in $1
do
    _LINUX_CHECK_HEADER_internal($lk_header,
	[AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_KINC_$lk_header), 1)
$2],
	[$3],
	[$4])dnl
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
    if test -n "$1" ; then linux_tmp="$1" ; else linux_tmp="for kernel config $2" ; fi
    AS_VAR_PUSHDEF([linux_config], [linux_cv_$2])dnl
    AC_CACHE_CHECK([$linux_tmp], [linux_cv_$2], [dnl
	AC_EGREP_CPP([\<yes_we_have_$2_defined\>], [
#include <linux/version.h>
#include <linux/autoconf.h>
#ifdef $2
    yes_we_have_$2_defined
#endif
	], [AS_VAR_SET([linux_config], ['yes'])], [AS_VAR_SET([linux_config], ['no'])]) ])
    linux_tmp=AS_VAR_GET([linux_config])
    if test :"${linux_tmp:-no}" = :yes ; then :;
$3
    else :;
$4
    fi
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
	AS_HELP_STRING([--enable-k-modversions],
	    [specify whether symbol versioning is to be used on symbols
	    exported from built modules.  @<:@default=yes@:>@]),
	[enable_k_modversions="$enableval"],
	[enable_k_modversions="$linux_cv_k_versions"])
    if test :"${enable_k_modversions:-no}" = :yes
    then
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
AC_DEFUN([_LINUX_KERNEL_SYMBOL_ADDR], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AS_VAR_PUSHDEF([linux_symbol_addr], [linux_cv_$1_addr])dnl
    AC_CACHE_CHECK([for kernel symbol $1 address], linux_symbol_addr, [dnl
	linux_tmp=
	if test -z "$linux_tmp" -a -n "$ksyms" -a -r "$ksyms"
	then
	    linux_tmp="`($EGREP '\<$1\>' $ksyms | sed -e 's| .*||;s|^0[xX]||') 2>/dev/null`"
	fi
	if test -z "$linux_tmp" -a -n "$kallsyms" -a -r "$kallsyms"
	then
	    linux_tmp="`($EGREP '\<$1\>' $kallsyms | head -1 | sed -e 's| .*||;s|^0[xX]||') 2>/dev/null`"
	fi
	if test -z "$linux_tmp" -a -n "$ksysmap" -a -r "$ksysmap" 
	then
	    linux_tmp="`($EGREP '\<$1\>' $ksysmap | sed -e 's| .*||;s|^0[xX]||') 2>/dev/null`"
	fi
	linux_tmp="${linux_tmp:+0x}$linux_tmp"
	AS_VAR_SET([linux_symbol_addr], ["${linux_tmp:-no}"]) ])
    linux_tmp=AS_VAR_GET([linux_symbol_addr])
    if test :"${linux_tmp:-no}" != :no 
    then :; AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$1[]_ADDR), AS_VAR_GET([linux_symbol_addr]),
	    [The symbol $1 is not exported by some kernels.  Define this to
	    the address of $1 in the kernel system map so that kernel modules
	    can be properly supported.])
$3
    else :;
$2
    fi
    AS_VAR_POPDEF([linux_symbol_addr])dnl
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
AC_DEFUN([_LINUX_KERNEL_EXPORT_ONLY], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AS_VAR_PUSHDEF([linux_symbol_export], [linux_cv_$1_export])dnl
    AC_CACHE_CHECK([for kernel symbol $1 export], linux_symbol_export, [dnl
	linux_tmp=
	if test -z "$linux_tmp" -a -n "$ksyms" -a -r "$ksyms"
	then
	    if ( $EGREP -q  '(\<$1_R(smp_)?........\>|\<$1\>)' $ksyms 2>/dev/null )
	    then
		linux_tmp="yes ($ksyms)"
	    fi
	fi
	if test -z "$linux_tmp" -a -n "$kallsyms" -a -r "$kallsyms"
	then
	    if ( $EGREP -q '\<__ksymtab_$1\>' $kallsyms 2>/dev/null )
	    then
		linux_tmp="yes ($kallsyms)"
	    fi
	fi
	if test -z "$linux_tmp" -a -n "$ksysmap" -a -r "$ksysmap" 
	then
	    if ( $EGREP -q '\<__ksymtab_$1\>' $ksysmap 2>/dev/null )
	    then
		linux_tmp="yes ($ksysmap)"
	    fi
	fi
	if test -z "$linux_tmp" -a ":$linux_cv_k_ko_modules" != :yes
	then
	    _LINUX_KERNEL_ENV([dnl
		AC_EGREP_CPP([\<yes_symbol_$1_is_exported\>], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#ifdef $1
	yes_symbol_$1_is_exported
#endif
		], [linux_tmp='yes (linux/modversions.h)']) ])
	fi
	AS_VAR_SET([linux_symbol_export], ["${linux_tmp:-no}"]) ])
    linux_tmp=AS_VAR_GET([linux_symbol_export])
    if test :"${linux_tmp:-no}" != :no 
    then :; AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$1[]_EXPORT), [1], [The symbol $1
	    is not exported by some kernels.  Define this if the symbol $1
	    is exported by your kernel so that kernel modules can be supported
	    properly.])
$3
    else :;
$2
    fi
    AS_VAR_POPDEF([linux_symbol_export])dnl
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
AC_DEFUN([_LINUX_KERNEL_SYMBOL], [dnl
    AC_REQUIRE([_LINUX_KERNEL])
    AS_VAR_PUSHDEF([linux_symbol], [linux_cv_$1[]_symbol])dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([$1], [dnl
	AS_VAR_SET([linux_symbol], ['no'])], [dnl
	AS_VAR_SET([linux_symbol], ['yes'])])
    if test :AS_VAR_GET([linux_symbol]) = :yes
    then :; AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$1[]_SYMBOL), [1], [The symbol $1
	    is not exported by some kernels.  Define this if the symbol $1 is
	    either exported by your kernel, or can be stripped from the symbol
	    map, so that kernel modules can be supported properly.])
$2
    else :;
$3
    fi
    AS_VAR_POPDEF([linux_symbol])dnl
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
# _LINUX_KERNEL_
# -----------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL_], [dnl
])# _LINUX_KERNEL_
# =============================================================================

# =============================================================================
#
# $Log: kernel.m4,v $
# Revision 0.9.2.162  2008-04-10 10:53:23  brian
# - remove deprecated Nexusware support
#
# Revision 0.9.2.161  2007/10/17 20:00:27  brian
# - slightly different path checks
#
# Revision 0.9.2.160  2007/08/12 19:05:31  brian
# - rearrange and update headers
#
# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
