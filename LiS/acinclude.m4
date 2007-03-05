# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 1.1.6.56 $) $Date: 2007/03/04 23:29:53 $
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
# Foundation; version 2 of the License.
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
# Last Modified $Date: 2007/03/04 23:29:53 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: acinclude.m4,v $
# Revision 1.1.6.56  2007/03/04 23:29:53  brian
# - corrected modversions directory
#
# Revision 1.1.6.55  2007/03/04 23:14:18  brian
# - better search for modversions
#
# Revision 1.1.6.54  2007/03/03 08:39:53  brian
# - corrections for STREAMS_VERSION
#
# Revision 1.1.6.53  2007/03/02 10:03:20  brian
# - updates to common build process and versions for all exported symbols
#
# Revision 1.1.6.52  2007/02/28 06:30:10  brian
# - updates and corrections, #ifdef instead of #if
#
# Revision 1.1.6.51  2006/10/30 06:39:58  brian
# - changes to handle missing linux/compile.h on SuSE
#
# Revision 1.1.6.50  2006/10/27 22:38:53  brian
# - changes for 2.6.18 build
#
# Revision 1.1.6.49  2006/09/18 13:20:05  brian
# - better directory detection
#
# Revision 1.1.6.48  2006/09/18 01:15:20  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 1.1.6.47  2006/07/25 06:38:55  brian
# - expanded minor device numbers and optimization and locking corrections
#
# Revision 1.1.6.46  2006/07/07 21:17:36  brian
# - corrections for compile of FC5 with 2.6.17 kernel
#
# Revision 1.1.6.45  2006/05/08 03:12:23  brian
# - added module id base to strconf
#
# Revision 1.1.6.44  2006/03/21 13:24:09  brian
# - added problem report checks
#
# Revision 1.1.6.43  2006/03/14 21:09:48  brian
# - two levels of distro specific rpm extra tags
#
# Revision 1.1.6.42  2006/03/09 05:51:33  brian
# - autoconf nits and autoupdate changes
#
# Revision 1.1.6.41  2006/03/05 04:02:56  brian
# - changes primarily for fc4 x86_64 gcc 4.0.4 2.6.15 SMP
# - updates for new release
#
# Revision 1.1.6.40  2006/02/20 11:41:09  brian
# - added checks for 64 bit architectures
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/devfs.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])
m4_include([m4/strconf.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_LIS
# -----------------------------------------------------------------------------
AC_DEFUN([AC_LIS], [dnl
    _OPENSS7_PACKAGE([LiS], [Linux STREAMS (LiS)])
    _LIS_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/lis-core.postinst
		     debian/lis-core.postrm
		     debian/lis-core.preinst
		     debian/lis-core.prerm
		     debian/lis-devel.preinst
		     debian/lis-dev.postinst
		     debian/lis-dev.preinst
		     debian/lis-dev.prerm
		     debian/lis-doc.postinst
		     debian/lis-doc.preinst
		     debian/lis-doc.prerm
		     debian/lis-init.postinst
		     debian/lis-init.postrm
		     debian/lis-init.preinst
		     debian/lis-init.prerm
		     debian/lis-lib.preinst
		     debian/lis-source.preinst
		     debian/lis-util.preinst
		     util/modutils/lis
		     include/sys/LiS/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _LIS_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-DLIS=1 -D__LIS_INTERNAL__'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    if test :${linux_cv_k_ko_modules:-no} = :no ; then
	if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	    PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
	fi
    fi
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/include -I${top_srcdir}/include'
dnl Just check config.log if you want to see these...
dnl AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
dnl AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
dnl AC_MSG_NOTICE([final user    LDFLAGS   = $USER_LDFLAGS])
dnl AC_MSG_NOTICE([final package INCLUDES  = $PKG_INCLUDES])
dnl AC_MSG_NOTICE([final package MODFLAGS  = $PKG_MODFLAGS])
dnl AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
dnl AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
dnl AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
dnl AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
dnl AC_MSG_NOTICE([final kernel  LDFLAGS   = $KERNEL_LDFLAGS])
dnl AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
dnl AC_MSG_NOTICE([final streams MODFLAGS  = $STREAMS_MODFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([PKG_INCLUDES])dnl
    AC_SUBST([PKG_MODFLAGS])dnl
    PKG_MANPATH='$(mandir)'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _LIS_OUTPUT
    _AUTOTEST
    _DOXY
])#AC_LIS
# =============================================================================

# =============================================================================
# _LIS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OPTIONS], [dnl
])# _LIS_OPTIONS
# =============================================================================

# =============================================================================
# _LIS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SETUP], [dnl
    AC_DEFINE([LIS_2_18], [1], [Always define this for an LIS_2_18 distribution.])
    AC_ARG_ENABLE([user-mode],
	AS_HELP_STRING([--enable-user-mode],
	    [enable user mode build.  @<:@default=no@:>@]),
	[enable_user_mode="$enableval"],
	[enable_user_mode='no'])
    AC_CACHE_CHECK([for lis target], [lis_cv_target], [dnl
	lis_cv_target=user
	case "${enable_user_mode:-no}:${host_os}" in
	    no:linux*)	lis_cv_target=linux ;;
	    no:qnx*)	lis_cv_target=qnx ;;
	    *)		lis_cv_target=user ;;
	esac ])
    case "$lis_cv_target" in
	user)
	    LIS_TARGET=u
	    _LIS_USER_SETUP
	;;
	linux)
	    LIS_TARGET=l
	    _LIS_LINUX_SETUP
	;;
	qnx)
	    LIS_TARGET=q
	    _LIS_QNX_SETUP
	;;
	*)
	    LIS_TARGET=p
	    AC_MSG_ERROR([*** "$lis_cv_target" is an unsupported operating system.])
	;;
    esac
    LIS_SUBDIRS=$lis_cv_target
    AM_CONDITIONAL(LIS_USER_TARGET, test :"$LIS_TARGET" = :u)dnl
    AM_CONDITIONAL(LIS_LINUX_TARGET, test :"$LIS_TARGET" = :l)dnl
    AM_CONDITIONAL(LIS_QNX_TARGET, test :"$LIS_TARGET" = :q)dnl
    AC_SUBST([LIS_SUBDIRS])dnl
    AC_SUBST([POLLTST])dnl
])# _LIS_SETUP
# =============================================================================

# =============================================================================
# _LIS_NO_LINUX
# -----------------------------------------------------------------------------
# Null out all the definitions that we don't need when not building for linux
AC_DEFUN([_LIS_NO_LINUX], [dnl
dnl LIS_KERN_TARGET=
    STRCONF_MAJBASE=231
    STRCONF_CONFIG='include/sys/LiS/config.h'
    STRCONF_MODCONF='head/modconf.inc'
    STRCONF_DRVCONF='drvrconf.mk'
    STRCONF_CONFMOD='conf.modules'
    STRCONF_MAKEDEV='devices.lst'
    STRCONF_MKNODES='libc/user/strmakenodes.c' dnl
])# AC_LIS_NO_LINKX
# =============================================================================

# =============================================================================
# _LIS_USER_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_USER_SETUP], [dnl
    AC_MSG_WARN([*** User Mode is not (well) supported at this time: good luck! ***])
    _LIS_NO_LINUX
    POLLTST=polltst
    AC_DEFINE([_SVID_SOURCE], [1], [Define when building as user mode.])
    AC_DEFINE([_BSD_SOURCE], [1], [Define when building as user mode.])
    AC_DEFINE([_XOPEN_SOURCE], [500], [Define when building as user mode.  I added
				   this one.  Actually, it should always be
				   defined.  We really need to do an AH_VERBATIM
				   here to check if it is already defined.])
#
#   Below is effectively what gcom make files do but this is the wrong way to
#   do this.  We do a platform independent autoconf check so that we can build
#   the user target for systems with old SysV definitions.
#
#   AC_REQUIRE([AC_CANONICAL_TARGET])dnl
#   case "$host_os" in
#       solaris* | sun5*)
#           # this is the wrong way to do this, go looking for them instead
#           AC_DEFINE([LIS_HAVE_MAJOR_T], [1], [Define when build as user mode for SunOS5.])
#           AC_DEFINE([LIS_HAVE_O_UID_T], [1], [Define when build as user mode for SunOS5.])
#           ;;
#   esac
#
    AC_CHECK_TYPE(o_uid_t, lis_cv_type_o_uid_t=yes, lis_cv_type_o_uid_t=no)
    if test "$lis_cv_type_o_uid_t" = yes; then
	AC_DEFINE([LIS_HAVE_O_UID_T], [1], [Define this macro when your
		   <sys/types.h> has o_uid_t and o_gid_t already defined and
		   you are building for a user target to keep LiS source files
		   from redefining o_uid_t and o_gid_t.])
    fi
    AC_CHECK_TYPE(major_t, lis_cv_type_major_t=yes, lis_cv_type_major_t=no)
    if test "$lis_cv_type_major_t" = yes; then
	AC_DEFINE([LIS_HAVE_MAJOR_T], [1], [Define this macro when your
		   <sys/types.h> has major_t already defined and you are
		   building for a user target to keen LiS source files from
		   redefining major_t.])
    fi
# 
#   For user setup, as the gcom makefiles do, we always use the Config.user
#   file as the sole configuration file.
# 
    AC_DEFINE([USER], [1], [Define if compiling for USER.]) dnl
])# _LIS_USER_SETUP
# =============================================================================

# =============================================================================
# _LIS_QNX_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_QNX_SETUP], [dnl
    AC_MSG_WARN([*** QNX not supported at this time: good luck! ***])
    _LIS_NO_LINUX
    CPPFLAGS="-DQNX${CPPFLAGS:+ }${CPPFLAGS}"
    AC_DEFINE([QNX], [1], [Define if compiling for QNX.]) dnl
])# _LIS_QNX_SETUP
# =============================================================================

# =============================================================================
# _LIS_SCO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SCO_SETUP], [dnl
    AC_MSG_WARN([*** SCO not supported at this time: good luck! ***])
    _LIS_NO_LINUX
    CPPFLAGS="-DSCO${CPPFLAGS:+ }${CPPFLAGS}"
    AC_DEFINE([SCO], [1], [Define if compiling for SCO.]) dnl
])# _LIS_SCO_SETUP
# =============================================================================

# =============================================================================
# _LIS_DOS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_DOS_SETUP], [dnl
    AC_MSG_WARN([*** DOS not supported at this time: good luck! ***])
    _LIS_NO_LINUX
    CPPFLAGS="-DDOS${CPPFLAGS:+ }${CPPFLAGS}"
    AC_DEFINE([DOS], [1], [Define if compiling for DOS.]) dnl
])# _LIS_DOS_SETUP
# =============================================================================

# =============================================================================
# _LIS_LINUX_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_LINUX_SETUP], [dnl
    AC_MSG_NOTICE([-----------------------------------])
    AC_MSG_NOTICE([starting linux kernel configuration])
    AC_MSG_NOTICE([-----------------------------------])
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    _LIS_SETUP_LIS
    AC_MSG_NOTICE([-----------------------------------])
    AC_MSG_NOTICE([complete linux kernel configuration])
    AC_MSG_NOTICE([-----------------------------------])
    STRCONF_MAJBASE=231
    STRCONF_CONFIG='include/sys/LiS/config.h'
    STRCONF_MODCONF='head/modconf.inc'
    STRCONF_DRVCONF='drvrconf.mk'
    STRCONF_CONFMOD='conf.modules'
    STRCONF_MAKEDEV='devices.lst'
    STRCONF_MKNODES='util/linux/strmakenodes.c'
    AC_DEFINE([LINUX], [1], [Define if compiling for LINUX.]) dnl
])# _LIS_LINUX_SETUP
# =============================================================================

# =============================================================================
# _LIS_SETUP_LIS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SETUP_LIS], [dnl
    _LIS_CONFIG_SYSCALLS
    _LIS_CHECK_KERNEL
    LIS_KSRC="${ksrcdir:-$kbuilddir}"
    LIS_KINCL="${ksrcdir:-$kbuilddir}/include"
    LIS_NOKSRC=0
    if test :"${linux_cv_k_running:-no}" = :no
    then
	LIS_USE_RUNNING_KERNEL=n
    else
	LIS_USE_RUNNING_KERNEL=y
    fi
    LIS_NKVER="${linux_cv_k_major}.${linux_cv_k_minor}.${linux_cv_k_patch}"
    LIS_KVER="$linux_cv_k_release"
    LIS_KVER_MAJOR="$linux_cv_k_major"
    LIS_KVER_MINOR="$linux_cv_k_minor"
    LIS_KVER_PATCH="$linux_cv_k_patch"
    LIS_KVER_PATCHNO="$linux_cv_k_extra"
    LIS_KVER_MAJORMINOR="${linux_cv_k_major}.${linux_cv_k_minor}"
dnl we don't actually grep the header file
    LIS_KVER_H="$LIS_KVER"
    LIS_NKVER_H="$LIS_NKVER"
    AH_TEMPLATE([__SMP__], [Define for SMP compile.])
    _LINUX_CHECK_KERNEL_CONFIG([], [CONFIG_SMP],
	[LIS_KSMP=y], [LIS_KSMP=n ; AC_DEFINE([__SMP__], [1])])
    LIS_KMODULES=y
    _LINUX_CHECK_KERNEL_CONFIG([], [CONFIG_MODVERSIONS],
	[LIS_KMODVERS=y], [LIS_KMODVERS=n])
    LIS_LISMODVERS=y
    AH_TEMPLATE([LISMODVERS], [Define if a modversions.h file needs to be included.])
dnl AC_DEFINE([LISMODVERS], [1])
    LIS_LISAUTOCONF=n
# 
#   this is just to generate the template
# 
    AH_TEMPLATE([LISAUTOCONF], [Define if an autoconf.h file was generated.
	LISAUTOCONF is used by "include/sys/LiS/linux-mdep.h" to know when to
	include "LiS/sys/autoconf.h" and to prevent linux from including its
	own.  In autoconf we do this by setting the appropriate include
	directory order in the makefile.  Our local "include/linux/autoconf.h"
	will be included in preference to the kernel one.  I don't know about
	LiS's practice of mucking around with include file wrapper macros.  A
	renaming of some macros would kil portability.  We never define this
	macro even when we build our own autoconf but we do mark it in gcom's
	old config.in file when we build it.])
    _LINUX_CHECK_KERNEL_CONFIG([], [CONFIG_IPV6],
	[LIS_IPV6=y], [LIS_IPV6=n])
    _LINUX_CHECK_KERNEL_CONFIG([], [CONFIG_IPV6_MODULE],
	[LIS_IPV6_MODULE=y], [LIS_IPV6_MODULE=n])
    LIS_CONFIG_MK_KERNEL=n
    LIS_STRMS_QUEUES=t
    AH_TEMPLATE([USE_KTHREAD], [Define (always) for kernel thread queues.])
    AC_DEFINE([USE_KTHREAD], [1])
    eval "LIS_MOD_INST_DIR=\"$DESTDIR$kmoduledir\""
    if test :"${enable_k_install:-yes}" = :yes
    then LIS_MOD_INSTALL=y
    else LIS_MOD_INSTALL=n
    fi
    _LIS_RH_71_KLUDGE
    _LIS_SIGMASKLOCK
    _LIS_RCVOID
    _LIS_OLD_GCOM_CONFIG_IN
    LIS_KINCL_MACH_GENERIC="-I${ksrcdir:-$kbuilddir}/include/asm-${linux_cv_k_mach}/mach-generic"
    LIS_KINCL_MACH_DEFAULT="-I${ksrcdir:-$kbuilddir}/include/asm-${linux_cv_k_mach}/mach-default"
    _LIS_GET_EMPTY_INODE
    _LIS_SET_CPUS_ALLOWED
    _LIS_INT_PSW
    _LIS_KMEM_OPTS
    LIS_CONFIG_INET=n
    LIS_PKGCOMPILE=n
    LIS_PKGMODULES=n
    LIS_SILENT_BUILD=y
dnl
dnl We don't really care about this.  We have our own modpost.  This is just for
dnl the GCOM config.in file.
dnl
    if test :"${linux_cv_k_ko_modules:-no}" = :yes
    then LIS_KBUILD=y
	LIS_MODUTILS=n
	LIS_MODULE_INIT_TOOLS=y
	if test "$linux_cv_k_major$linux_cv_k_minor" -eq 26 -a "$linux_cv_k_patch" -gt 3
	then LIS_KBUILD_NEEDS_SYMVERS=y
	     LIS_KBUILD_NEEDS_MODNAME=y
	else LIS_KBUILD_NEEDS_SYMVERS=n
	     LIS_KBUILD_NEEDS_MODNAME=n
	fi
    else LIS_KBUILD=n
	 LIS_MODUTILS=y
	 LIS_MODULE_INIT_TOOLS=n
	 LIS_KBUILD_NEEDS_SYMVERS=n
	 LIS_KBUILD_NEEDS_MODNAME=n
    fi
dnl
dnl We don't care about this one either.  We rip symbols from the System.map and
dnl the kernel modules themselves.
dnl
    LIS_KSYMVERS="${MODPOST_SYSVER}"
    _LIS_ARCH_DEFINES
])# _LIS_SETUP_LIS
# =============================================================================

# =============================================================================
# _LIS_CHECK_KERNEL
# -----------------------------------------------------------------------------
# This is kernel configuration above and beyond the LiS config files.  In many
# places, LiS checks KERNEL_2_ something.  Using the kernel version is
# unreliable because some distros patch forward (from 2.6 into 2.4) and the
# kernel version is insufficient.  Availability of symbols is more practical and
# makes the resulting configuration script applicable to a wider range of
# kernels and distributions.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CHECK_KERNEL], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/slab.h linux/cdev.h \
			  linux/hardirq.h linux/cpumask.h linux/kref.h linux/security.h \
			  asm/uaccess.h linux/kthread.h linux/compat.h linux/ioctl32.h \
			  asm/ioctl32.h linux/syscalls.h linux/rwsem.h linux/smp_lock.h \
			  linux/devfs_fs_kernel.h linux/compile.h linux/utsrelease.h], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
])
    AC_SUBST([EXPOSED_SYMBOLS])dnl
dnl
dnl  Well, not exporting kthread_ functions is just about the stupidest thing
dnl  I've seen from a distro yet.  SLES takes the prize for this stupidity.
dnl
    _LINUX_KERNEL_SYMBOLS([kthread_create, kthread_should_stop, kthread_stop, kthread_bind])
    _LINUX_CHECK_FUNCS([try_module_get module_put to_kdev_t force_delete kern_umount iget_locked \
			process_group cpu_raise_softirq check_region pcibios_init \
			pcibios_find_class pcibios_find_device pcibios_present \
			pcibios_read_config_byte pcibios_read_config_dword \
			pcibios_read_config_word pcibios_write_config_byte \
			pcibios_write_config_dword pcibios_write_config_word \
			pci_dac_dma_sync_single pci_dac_dma_sync_single_for_cpu \
			pci_dac_dma_sync_single_for_device pci_dac_set_dma_mask \
			pci_find_class pci_dma_sync_single pci_dma_sync_sg \
			pci_dac_page_to_dma pci_dac_dma_to_page pci_dac_dma_to_offset \
			sleep_on interruptible_sleep_on sleep_on_timeout \
			cpumask_scnprintf __symbol_get __symbol_put \
			read_trylock write_trylock atomic_add_return path_lookup \
			MOD_DEC_USE_COUNT MOD_INC_USE_COUNT cli sti \
			num_online_cpus generic_delete_inode access_ok set_user_nice \
			set_cpus_allowed yield \
			prepare_to_wait prepare_to_wait_exclusive finish_wait \
			compat_ptr register_ioctl32_conversion unregister_ioctl32_conversion \
			simple_statfs], [:], [
			case "$lk_func" in
			    pcibios_*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				;;
			    pci_*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_osif_${lk_func}"
				;;
			    *sleep_on*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				;;
			esac ], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
#ifdef HAVE_KINC_ASM_UACCESS_H
#include <asm/uaccess.h>
#endif
#ifdef HAVE_KINC_LINUX_COMPAT_H
#include <linux/compat.h>
#endif
])
    _LINUX_CHECK_MACROS([MOD_DEC_USE_COUNT MOD_INC_USE_COUNT \
			 read_trylock write_trylock num_online_cpus \
			 cpumask_scnprintf access_ok], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
#ifdef HAVE_KINC_ASM_UACCESS_H
#include <asm/uaccess.h>
#endif
])
    _LINUX_CHECK_TYPES([irqreturn_t], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/time.h>		/* for struct timespec */
])
dnl 
dnl In later kernels, the super_block.u.geneic_sbp and the filesystem specific
dnl union u itself have been removed and a simple void pointer for filesystem
dnl specific information has been put in place instead.  We don't really care
dnl one way to the other, but this check discovers which way is used.
dnl 
    _LINUX_CHECK_MEMBERS([struct task_struct.namespace.sem,
			  struct task_struct.signal,
			  struct file_operations.flush,
			  struct super_operations.drop_inode,
			  struct task_struct.session,
			  struct task_struct.pgrp,
			  struct super_block.s_fs_info,
			  struct super_block.u.generic_sbp,
			  struct file_system_type.read_super,
			  struct file_system_type.get_sb,
			  struct super_operations.read_inode2,
			  struct kstatfs.f_type,
			  struct kobject.kref,
			  struct inode.i_mutex,
			  struct file_operations.unlocked_ioctl,
			  struct inode.i_lock,
			  struct files_struct.max_fdset,
			  struct files_struct.fdtab,
			  struct inode.i_private,
			  struct inode.i_blksize], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
])
    at_ioctl_getmsg="$linux_cv_member_struct_file_operations_unlocked_ioctl"
    AC_SUBST([at_ioctl_getmsg])dnl
    AM_CONDITIONAL(USING_IOCTL_GETPMSG_PUTPMSG, test :$at_ioctl_getmsg = :yes)dnl
	_LINUX_KERNEL_SYMBOLS([ioctl32_hash_table, ioctl32_sem, compat_ptr])
	_LINUX_KERNEL_SYMBOLS([is_ignored, is_orphaned_pgrp, kill_sl, kill_proc_info, send_group_sig_info, session_of_pgrp])
	_LINUX_KERNEL_SYMBOL_EXPORT([cdev_put])
	_LINUX_KERNEL_EXPORT_ONLY([path_lookup])
	_LINUX_KERNEL_EXPORT_ONLY([raise_softirq])
	_LINUX_KERNEL_EXPORT_ONLY([raise_softirq_irqoff])
	_LINUX_KERNEL_EXPORT_ONLY([__symbol_get])
	_LINUX_KERNEL_EXPORT_ONLY([__symbol_put])
	_LINUX_KERNEL_SYMBOL_EXPORT([put_filp])
	_LINUX_KERNEL_ENV([dnl
	    AC_CACHE_CHECK([for kernel inode_operation lookup with nameidata],
			   [linux_cv_have_iop_lookup_nameidata], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif]],
			[[struct inode_operations temp;
(*temp.lookup)((struct inode *)0, (struct dentry *)0, (struct nameidata *)0);]]) ],
		    [linux_cv_have_iop_lookup_nameidata='yes'],
		    [linux_cv_have_iop_lookup_nameidata='no'])
	    ])
	    if test :$linux_cv_have_iop_lookup_nameidata = :yes ; then
		AC_DEFINE([HAVE_INODE_OPERATIONS_LOOKUP_NAMEIDATA], [1],
		    [Set if inode_operation lookup function takes nameidata pointer.])
	    fi
	    AC_CACHE_CHECK([for kernel file_operations flush with fl_owner_t],
			   [linux_cv_have_fop_flush_fl_owner_t], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif]],
[[struct file_operations temp;
(*temp.flush)((struct file *)0, (fl_owner_t)0);]]) ],
		    [linux_cv_have_fop_flush_fl_owner_t='yes'],
		    [linux_cv_have_fop_flush_fl_owner_t='no'])
	    ])
	    if test :$linux_cv_have_fop_flush_fl_owner_t = :yes ; then
		AC_DEFINE([HAVE_FILE_OPERATIONS_FLUSH_FL_OWNER_T], [1],
			  [Set if file_operations flush function takes fl_owner_t.])
	    fi
	    AC_CACHE_CHECK([for kernel super_operation statfs with dentry],
			   [linux_cv_have_sop_statfs_dentry], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif]],
[[struct super_operations temp;
(*temp.statfs)((struct dentry *)0, (struct kstatfs *)0);]]) ],
		    [linux_cv_have_sop_statfs_dentry='yes'],
		    [linux_cv_have_sop_statfs_dentry='no'])
	    ])
	    if test :$linux_cv_have_sop_statfs_dentry = :yes ; then
		AC_DEFINE([HAVE_SUPER_OPERATIONS_STATFS_DENTRY], [1],
			  [Set if super_operations statfs function takes dentry pointer.])
	    fi
	    AC_CACHE_CHECK([for kernel file_system_type get_sb with vfsmount],
			   [linux_cv_have_fst_get_sb_vfsmount], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif]],
[[struct file_system_type temp;
(*temp.get_sb)((struct file_system_type *)0, 0, (char *)0, (void *)0, (struct vfsmount *)0);]]) ],
		    [linux_cv_have_fst_get_sb_vfsmount='yes'],
		    [linux_cv_have_fst_get_sb_vfsmount='no'])
	    ])
	    if test :$linux_cv_have_fst_get_sb_vfsmount = :yes ; then
		AC_DEFINE([HAVE_FILE_SYSTEM_TYPE_GET_SB_VFSMOUNT], [1],
			  [Set if file_system_type get_sb function takes vfsmount pointer.])
	    fi
	    AC_CACHE_CHECK([for kernel do_settimeofday with timespec],
			   [linux_cv_have_timespec_settimeofday], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/time.h>		/* for struct timespec */]],
			[[struct timespec ts;
int retval;
retval = do_settimeofday(&ts);]]) ],
		[linux_cv_have_timespec_settimeofday='yes'],
		[linux_cv_have_timespec_settimeofday='no'])
	    ])
	    if test :$linux_cv_have_timespec_settimeofday = :yes ; then
		AC_DEFINE([HAVE_TIMESPEC_DO_SETTIMEOFDAY], [1],
		    [Define if do_settimeofday takes struct timespec and returns int.])
	    fi
	])
])# _LIS_CHECK_KERNEL
# =============================================================================

# =============================================================================
# _LIS_ARCH_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_ARCH_DEFINES], [dnl
    AC_REQUIRE([AC_CANONICAL_HOST])dnl
    AH_TEMPLATE([_X86_64_LIS_], [Define when compiling for X86_64.  This define
	is only used for linux kernel target.  This is really the wrong way to
	go about doing this: the function should be checked for by autoconf
	instead of placing the architectural dependencies in the LiS source.
	The define is used in "head/linux-mdep.c" to determine whether
	lis_pci_cleanup exists; "head/linux/exports.c" to determine whether a
	bunch of functions are available; "head/osif.c" to determine whether a
	bunch of PCI DMA mapping functions are available; "include/sys/osif.h"
	to determine whether a bunch of PCI DMA mapping functions are
	available.])
    AH_TEMPLATE([_PPC64_LIS_], [Define when compiling for PPC64.  This define is
	only used for linux kernel target.  This is really the wrong way to go
	about doing this: the function should be checked for by autoconf instead
	of placing the architectural dependencies in the LiS source.  The define
	is used in "head/linux-mdep.c" to determine whether lis_pci_cleanup
	exists; "head/linux/exports.c" to determine whether a bunch of functions
	are available; "head/osif.c" to determine whether a bunch of PCI DMA
	mapping functions are available; "include/sys/osif.h" to determine
	whether a bunch of PCI DMA mapping functions are available.])
    AH_TEMPLATE([_HPPA_LIS_], [Define when compiling for HPPA.  This define is
	only used for linux kernel target.  This is really the wrong way to go
	about doing this: the function should be checked for by autoconf instead
	of placing the architectural dependencies in the LiS source.  The define
	is used in "head/linux-mdep.c" to determine whether lis_pci_cleanup
	exists; "head/linux/exports.c" to determine whether a bunch of functions
	are available; "head/osif.c" to determine whether a bunch of PCI DMA
	mapping functions are available.])
    AH_TEMPLATE([_PPC_LIS_], [Define when compiling for PPC.  This define is
	only used for linux kernel target.  This is really the wrong way to go
	about doing this: the function should be checked for by autoconf instead
	of placing the architectural dependencies in the LiS source.  The define
	is used in <LiS/include/sys/osif.h> and "head/osif.c" to determine
	whether PCI BIOS is present; in (head/linux-mdep.c) to determine whether
	cpu binding is possible; to determine whether spin_is_locked() is
	available in "head/linux/lislocks.c"; in "head/mod.c" to determine
	whether to define struct pt_regs; and in <LiS/include/sys/lislocks.h> to
	determine the size of semaphore memory.])
    AH_TEMPLATE([_S390X_LIS_], [Define when compiling for S390X.  This define is
	only used for the linux kernel target.  This is really the wrong way to
	go about doing this: the function should be checked for by autoconf
	instead of placing the architectural depdendencies in the LiS source.
	The define is used in "head/linux-mdep.c" to determine whether
	lis_pci_cleanup exists; "head/linux/exports.c" to determine whether a
	bunch of functions are available; "head/osif.c" to determine whether a
	bunch of PCI DMA mapping functions are available; "include/sys/osif.h"
	to determine whether a bunch of PCI DMA mapping functions are
	available.])
    AH_TEMPLATE([_S390_LIS_], [Define when compiling for S390.  Strangely
	enough, _S390_LIS_ is never checked without _S390X_LIS_.  Rendering it
	as an alias for the above.])
    AH_TEMPLATE([_SPARC64_LIS_], [Define when compiling for Sparc64.  This
	define is only used for the linux kernel target.  This is really the
	wrong way to go about doing this: the function should be checked for by
	autoconf instead of placing the architectural dependencies in the LiS
	source.  The define is used to determine when ioremap functions are not
	available <LiS/include/osif.h>.  Strangely enough, none of the other
	checks are performed as for _SPARC_LIS_ below.])
    AH_TEMPLATE([_SPARC_LIS_], [Define when compiling for Sparc.  This define is
	used for the linux kernel target.  This is really the wrong way to go
	about doing this: the function should be checked for by autoconf instead
	of placing architectural depedencies in the LiS source.  The define is
	used to determine when ioremap functions are not available
	<LiS/include/osif.h>, when PCI BIOS is not present (head/osif.c), and
	when <linux/poll.h> is missing POLLMSG <LiS/include/sys/poll.h>])
    case "$linux_cv_march" in
	alpha*)			:					;;
	arm*)			:					;;
	cris*)			:					;;
	i?86* | k6* | athlon*)	:					;;
	ia64)			:					;;
	m68*)			:					;;
	mips64*)		:					;;
	mips*)			:					;;
	hppa*)			AC_DEFINE([_HPPA_LIS_],		[1])	;;
	ppc64* | powerpc64*)	AC_DEFINE([_PPC64_LIS_],	[1])
				AC_DEFINE([_PPC_LIS_],		[1])	;;
	ppc* | powerpc*)	AC_DEFINE([_PPC_LIS_],		[1])	;;
	s390x*)			AC_DEFINE([_S390X_LIS_],	[1])
				AC_DEFINE([_S390_LIS_],		[1])	;;
	s390*)			AC_DEFINE([_S390_LIS_],		[1])	;;
	sh*)			:					;;
	sparc64*)		AC_DEFINE([_SPARC64_LIS_],	[1])
				AC_DEFINE([_SPARC_LIS_],	[1])	;;
	sparc*)			AC_DEFINE([_SPARC_LIS_],	[1])	;;
	x86_64*)		AC_DEFINE([_X86_64_LIS_],	[1])	;;
	*)			:					;;
    esac
dnl AH_TEMPLATE([__powerpc__], [Define when compiling for PPC.  This define is
dnl	only used for the linux target.  This is an error, this value is defined
dnl	by the GNU compiler when it compiles.  It is not used by LiS source
dnl	code.])
dnl AH_TEMPLATE([__BOOT_KERNEL_SMP], [Define when compiling for S390 or S390X.
dnl	This is an error.  __BOOT_KERNEL_SMP should only be defined in kernel.h.
dnl	This is not used by the LiS source and will be handled by kernel source
dnl	checks.])
dnl case "$linux_cv_march" in
dnl	ppc* | powerpc*)	: AC_DEFINE([__powerpc__],   [1]) ;;
dnl	s390*)			: AC_DEFINE([__BOOT_KERNEL_SMP], [1]) ;;
dnl esac dnl
])# _LIS_ARCH_DEFINES
# =============================================================================

# =============================================================================
# _LIS_GET_SYSTEM_MAP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_SYSTEM_MAP], [dnl
])#_LIS_GET_SYSTEM_MAP
# =============================================================================

# =============================================================================
_LIS_CONFIG_SYSCALLS
# -----------------------------------------------------------------------------
# symbols to rip for LiS support (without system call generation)
# -----------------------------------------------------------------------------
# sys_unlink            <-- extern, not declared
# sys_mknod             <-- extern, not declared
# sys_umount            <-- extern, not declared
# sys_mount             <-- extern, not declared
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CONFIG_SYSCALLS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
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
	fi ]) dnl
])# _LIS_CONFIG_SYSCALLS
# =============================================================================

# =============================================================================
# _LIS_RH_71_KLUDGE
# -----------------------------------------------------------------------------
# this is not really a RH 7.1 kludge but a 2.4.2 kludge
AC_DEFUN([_LIS_RH_71_KLUDGE], [dnl
    AH_TEMPLATE([RH_71_KLUDGE], [Define if you have redhat kernel 2.4.2])
    AC_MSG_CHECKING([for broken RH 7.1 2.4.2 kernel])
    LIS_RH_71_KLUDGE=''
    if test "${linux_cv_k_major}:${linux_cv_k_minor}:${linux_cv_k_patch}" = "2:4:2"
    then
	# - should also check for redhat something (in kernel source directory)
	if test -e "${linux_cv_k_includes}/linux/rhconfig.h"
	then
	    LIS_RH_71_KLUDGE='y'
	    AC_DEFINE([RH_71_KLUDGE], [1])
	    AC_MSG_RESULT([yes])
	else
	    LIS_RH_71_KLUDGE='n'
	    AC_MSG_RESULT([no])
	fi
    else
	LIS_RH_71_KLUDGE='n'
	AC_MSG_RESULT([no])
    fi dnl
])# _LIS_RH_71_KLUDGE
# =============================================================================

# =============================================================================
# _LIS_SIGMASKLOCK
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SIGMASKLOCK], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AH_TEMPLATE([SIGMASKLOCK], [This is necessary due to RedHat 2.6 patches to
	kernel header files.  RedHat changes some of the use of sigmask_lock.
	Autoconf will check your header files.  The code will lock sigmask_lock
	when required.])
    _LINUX_CHECK_MEMBER([struct task_struct.sigmask_lock], [dnl
	LIS_SIGMASKLOCK='y'
	AC_DEFINE([SIGMASKLOCK], [1])], [dnl
	LIS_SIGMASKLOCK='n'], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
	])
])# _LIS_SIGMASKLOCK
# =============================================================================

# =============================================================================
# _LIS_RCVOID
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_RCVOID], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AH_TEMPLATE([RCVOID], [This is necessary due to RedHat 2.6 patches ot kernel
	header files.  RedHat changes recalc_sigpending to take void instead of
	a task_struct pointer and defines a new recalc_sigpending_tsk to take
	the other's place.  Autoconf will check your header files.  The code
	will try to use recalc_sigpending() if this is defined, or
	recalc_sigpending(current) otherwise.])
    _LINUX_KERNEL_SYMBOL([recalc_sigpending_tsk], [dnl
	LIS_RCVOID=y
	AC_DEFINE([RCVOID], [1])], [dnl
	LIS_RCVOID=n])
])# _LIS_RCVOID
# =============================================================================


# =============================================================================
# _LIS_OLD_GCOM_CONFIG_IN
# -----------------------------------------------------------------------------
# These substitutions are only really required to generate the old gcom
# configuration files, in case someone wants to perform the old gcom build.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OLD_GCOM_CONFIG_IN], [dnl
dnl
dnl old gcom configuration script uses `uname -m`, but of course that is the
dnl build host and not the target host.  This allows proper cross compiling.
dnl
    LIS_MACHINE=$host_cpu
dnl
dnl old gcom files use the base CC name just as the linux kernel makefiles
dnl do.  This does not work for proper cross-compiling.  This is the CC name
dnl as provided to the configuration script in the CC environment variable,
dnl after we make adjustments to it.  This could be either *-*-*-gcc or just
dnl plain cc, even in a cross-compiling situation.  For compatibility with
dnl the gcom makefiles we adjust CC_NAME to be just the CC root and strip
dnl off or regenerate the CROSS_COMPILE component.
dnl
    AC_REQUIRE([AC_PROG_CC])dnl
    if test :"${cross_compiling:-no}" = :yes
    then
	LIS_CC_NAME==`eval "echo $CC | sed -e 's|.*-||g'"`
	LIS_CROSS_COMPILING=y
	LIS_CROSS_COMPILE=`eval "echo $CC | sed -e 's|-[[^-]]*[$]||'"`-
	if test :"${LIS_CROSS_COMPILE:-no}" = :no
	then
	    AC_REQUIRE([AC_CANONICAL_TARGET])dnl
	    LIS_CROSS_COMPILE="$host"-
	fi
    else
	LIS_CC_NAME=$CC
	LIS_CROSS_COMPILING=n
	LIS_CROSS_COMPILE=
    fi
    AC_SUBST([LIS_CROSS_COMPILE])dnl
dnl
dnl this might not work for other than GCC, but I don't think that the gcom
dnl make files really support other than GCC builds anyways.  We do support
dnl (kinda) non-gcc builds
dnl
    LIS_CC_VERS=`eval "$CC -v 2>&1 | grep 'gcc version' | sed -r -e 's|gcc version[[[:space:]]]*([[^[:space:]]]*).*|\1|'"`
    LIS_CC_OPTIMIZE=`echo "$KERNEL_CFLAGS" | grep -- '-O' | sed -r -e 's|.*(-O[[^[:space:]]]*).*|\1|'`
    LIS_CC_OPT2=`echo "$KERNEL_CFLAGS" | grep -- '-mpreferred-stack-boundary=2' | sed -r -e 's|.*(-mpreferred-stack-boundary=2).*|\1|'`
])# _LIS_OLD_GCOM_CONFIG_IN
# =============================================================================

# =============================================================================
# _LIS_GET_EMPTY_INODE
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_EMPTY_INODE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_SYMBOL([new_inode], [LIS_GET_EMPTY_INODE='new_inode((_sb))'])
    _LINUX_KERNEL_SYMBOL([get_empty_inode], [LIS_GET_EMPTY_INODE='get_empty_inode()'])
    if test :"$LIS_GET_EMPTY_INODE" = :
    then
	AC_MSG_ERROR([
*** 
*** You have really mangled kernel header files: the headers should include
*** either new_inode() or get_new_inode().  Some kernel versions and distros
*** have one or the other missing.  In particular, RedHat removed
*** get_new_inode() from their patched kernels and made new_inode() non-inline
*** (used to be inline in the header file).  Your kernel headers are so bad
*** that <linux/fs.h> defines neither.  Please check your kernel headers and
*** source and try again.
*** ])
    fi dnl
    AH_TEMPLATE([GET_EMPTY_INODE], [This is necessary due to RedHat mangling of
	kernel header files.  RedHat removes get_empty_inode() from <linux/fs.h>
	and makes new_inode() non-inline and removes it from the header file
	(now a function call).  Autoconf will check your header files.  The code
	will try to use whatever is defined here to get an empty inode.  Neither
	a check of the header files nor a running kernel symbol table will
	guarantee that those symbols are exported and usable by LiS.  If you get
	depmod errors on instalation mentioning either of these functions, then
	this is probably the cause.])
    AC_DEFINE_UNQUOTED([GET_EMPTY_INODE(_sb)], [$LIS_GET_EMPTY_INODE])
])# _LIS_GET_EMPTY_INODE
# =============================================================================

# =============================================================================
# _LIS_SET_CPUS_ALLOWED
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SET_CPUS_ALLOWED], [dnl
    _LINUX_CHECK_FUNC([set_cpus_allowed],
	[LIS_SET_CPUS_ALLOWED=y
	 AC_DEFINE([SET_CPUS_ALLOWED], [1], [Set if you have function set_cpus_allowed to tell LiS
	 to use the available function.]) ],
	[LIS_SET_CPUS_ALLOWED=n], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
	])
])# _LIS_SET_CPUS_ALLOWED
# =============================================================================

# =============================================================================
# _LIS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OUTPUT], [dnl
dnl
dnl The following are common setup requirements that do not depend on the
dnl architecutre or build target.
dnl
    LIS_CONFIG="config.in"
    LIS_SRCDIR="$srcdir"
    LIS_GENCONF="$srcdir/include/sys/LiS/genconf.h"
    LIS_CONFIG_STREAMS=m
dnl LIS_TARGET="$LIS_TARGET"
    _LIS_CONFIG_DEV
    _LIS_REGPARMS
    _LIS_OLD_CONSTS
    _LIS_SOLARIS_STYLE_CMN_ERR
    _LIS_DBG_OPT
    _LIS_SHLIB
    _LIS_ATOMIC_STATS
dnl
dnl All of these come straight from autoconf.
dnl
    LIS_PACKAGE="$PACKAGE"
    if test :"$prefix" = :NONE
    then LIS_PREFIX="$ac_default_prefix"
    else LIS_PREFIX="$prefix"
    fi
    if test :"$exec_prefix" = :NONE
    then LIS_EXECPREFIX="$LIS_PREFIX"
    else LIS_EXECPREFIX="$exec_prefix"
    fi
    LIS_INCLUDEDIR=`eval "echo \"$DESTDIR$includedir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    LIS_LIBDIR=`eval "echo \"$DESTDIR$libdir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    LIS_BINDIR=`eval "echo \"$DESTDIR$bindir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    LIS_SBINDIR=`eval "echo \"$DESTDIR$sbindir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    LIS_SYSCONFDIR=`eval "echo \"$DESTDIR$sysconfdir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    LIS_DATADIR=`eval "echo \"$DESTDIR$datadir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    LIS_PKGINCLUDEDIR="$DESTDIR$LIS_INCLUDEDIR/$PACKAGE"
    LIS_PKGLIBDIR="$DESTDIR$LIS_LIBDIR/$LIS_PACKAGE"
    LIS_PKGDATADIR="$DESTDIR$LIS_DATADIR/$LIS_PACKAGE"
    LIS_PKGSRCDIR="$DESTDIR$rootdir/usr/src"
    LIS_MANDIR=`eval "echo \"$mandir\" | sed -e 's|\<NONE\>|$LIS_PREFIX|g'"`
    eval "LIS_MODSUBDIR=\"$DESTDIR$kmoduledir/$PACKAGE_LCNAME\""
dnl
dnl This is done by autoconf.
dnl
    LIS_MAKE_DIRS=
dnl
dnl Acutally generating output files is last.  We don't want to generate a
dnl thing until we have performed all the checks and balances.
dnl
    _LIS_CONFIG
    _LIS_STRCONF
    LIS_MAJOR_BASE="$STRCONF_MAJBASE"
    _LIS_OUTPUT_CONFIG_IN dnl
    AM_CONDITIONAL(WITH_LFS, false)dnl
    AM_CONDITIONAL(WITH_LIS, true)dnl
dnl
dnl Missing templates.
dnl
    AH_TEMPLATE([DO_ADDERROR], [Unused.])
    AH_TEMPLATE([DO_STREAMDEBUG], [Unused.])
    AH_TEMPLATE([FIFO_DEBUG], [Unused.])
    AH_TEMPLATE([FIFO_IMPL], [Unused.])
    AH_TEMPLATE([GCOM], [Unused.])
    AH_TEMPLATE([GCOM_OPEN], [Unused.])
    AH_TEMPLATE([IP2XINET_DEBUG], [Unused.])
    AH_TEMPLATE([NOKSRC], [Unused.])
    AH_TEMPLATE([PIPE_DEBUG], [Unused.])
    AH_TEMPLATE([TC_PRIO_BULK], [Unused.])
    AH_TEMPLATE([TC_PRIO_BESTEFFORT], [Unused.])
    AH_TEMPLATE([TC_PRIO_INTERACTIVE], [Unused.])
    AH_TEMPLATE([SOPRI_BACKGROUND], [Unused.])
    AH_TEMPLATE([SOPRI_NORMAL], [Unused.])
    AH_TEMPLATE([SOPRI_INTERACTIVE], [Unused.])
    AH_TEMPLATE([STREAMS_DEBUG], [Unused.])
    AH_TEMPLATE([USE_VOID_PUT_PROC], [Unused.])

])# _LIS_OUTPUT
# =============================================================================

# =============================================================================
# _LIS_INT_PSW
# -----------------------------------------------------------------------------
# Determines from the --enable-broken-cpu-flags option whether to use proper
# unsigned long for cpu flags or whether to store then as an int.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_INT_PSW],[dnl
    AC_ARG_ENABLE([broken-cpu-flags],
	AS_HELP_STRING([--enable-broken-cpu-flags],
	    [use broken integer cpu flags for locks, otherwise use unsigned
	    long cpu flags.  @<:@default=no@:>@]),
	[enable_broken_cpu_flags="$enableval"],
	[enable_broken_cpu_flags='no'])
    AC_MSG_CHECKING([for lis broken cpu flags])
    if test :"${enable_broken_cpu_flags:-no}" = :no
    then
	LIS_INT_PSW='n'
	INT_PSW=0
	AC_MSG_RESULT([no])
    else
	LIS_INT_PSW='y'
	INT_PSW=1
	AC_DEFINE([INT_PSW], [1], [Define if you want interrupt flags stored as
		  int instead of unsigned long.  The kernel always uses
		  unsigned long.  It is generally a bad idea to use int when
		  the kernel uses unsigned long.  If in doubt, leave this
		  undefined.])
	AC_MSG_RESULT([yes])
    fi
    AC_SUBST([INT_PSW])dnl
])# _LIS_INT_PSW
# =============================================================================

# =============================================================================
# _LIS_CONFIG_DEV
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CONFIG_DEV], [dnl
    AC_ARG_ENABLE([lis-development],
	AS_HELP_STRING([--enable-lis-development],
	    [configure to report source code path trace @<:@default=no@:>@]),
	    [enable_lis_development="$enableval"],
	    [case :"$linux_cv_debug" in
		(:_DEBUG|:_TEST|:_SAFE) enable_lis_development=yes ;;
		(:_NONE|:*)		enable_lis_development=no  ;;
	    esac])
    AH_TEMPLATE([CONFIG_DEV], [Does what LIS_CONFIG_SAFE and SAFE used to.  The
	name of this macro was a spectacularly poor choice given that we are
	compiling kernel modules.])
    AC_MSG_CHECKING([for lis development build])
    if test :"${enable_lis_development:-no}" = :no
    then
	LIS_CONFIG_DEV=n
    else
	LIS_CONFIG_DEV=y
	AC_DEFINE([CONFIG_DEV], [1])
    fi
    AC_MSG_RESULT([${enable_lis_development:-no}])
])# _LIS_CONFIG_DEV
# =============================================================================

# =============================================================================
# _LIS_REGPARMS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_REGPARMS], [dnl
    LIS_CCREGPARM=`echo "$KERNEL_CFLAGS" | grep -- '-mregparm=3' | sed -r -e 's|.*(-mregparm=3).*|\1|'`
    AC_ARG_WITH([lis-regparms],
	AS_HELP_STRING([--with-lis-regparms],
	    [configure to set the regparms for exported symbols
	     @<:@default=0@:>@]),
	[with_lis_regparms="$withval"],
	[with_lis_regparms='0'])
    AH_TEMPLATE([STREAMS_REGPARM], [If you have a regparms kernel, define to the
	the number of register parameters passed to functions that LiS exports.
	This is for binary compatibility.])
    AH_VERBATIM([_RP], m4_text_wrap([Use this macro like fastcall.  It is set to
	an attribute with the number of parameters passed to exported LiS
	functions when there is a regparms kernel.  This is for binary
	compatibility. */], [   ], [/* ])[
#if defined __i386__ || defined __x86_64__ || defined __k8__
#undef _RP
#else
#define _RP
#endif])
    AC_MSG_CHECKING([for lis regparms])
    LIS_STREAMS_REGPARM="${with_lis_regparms:-0}"
    AC_DEFINE_UNQUOTED([STREAMS_REGPARM], [$LIS_STREAMS_REGPARM])
    if test :$LIS_STREAMS_REGPARM = :no ; then
	AC_DEFINE_UNQUOTED([_RP], [])
    else
	AC_DEFINE_UNQUOTED([_RP], [__attribute__((__regparm__(STREAMS_REGPARM)))])
    fi
    AC_MSG_RESULT([$LIS_STREAMS_REGPARM])
])# _LIS_REGPARMS
# =============================================================================

# =============================================================================
# _LIS_OLD_CONSTS
# -----------------------------------------------------------------------------
# Determine from the --with-solaris-consts option whether to use
# solaris/unixware compatible consts or backwards compatible LiS consts (which
# have no bearing in reality).
# -----------------------------------------------------------------------------
# I could eventually expand this to set more precisely the compatibility of
# constants within the STREAMS environment.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OLD_CONSTS], [dnl
    AH_TEMPLATE([USE_OLD_CONSTS], [Define to use LiS backware compatible
	constants (which have no bearing in reality).  Leave this undefined to
	get Solaris/Unixware compatible constants.])
    AC_ARG_WITH([solaris-consts],
	AS_HELP_STRING([--with-solaris-consts],
	    [use Unixware/Solaris compatible constants if yes, or use LiS
	    backward compatible constants if no.  @<:@default=yes@:>@]),
	[with_solaris_consts="$withval"],
	[with_solaris_consts='yes'])
    AC_MSG_CHECKING([for lis Unixware/Solaris compatible constants])
    if test :"${with_solaris_consts:-yes}" = :yes
    then
	LIS_OLD_CONSTS=n
	AC_MSG_RESULT([no])
    else
	LIS_OLD_CONSTS=y
	AC_DEFINE([USE_OLD_CONSTS], [1])
	AC_MSG_RESULT([yes])
    fi dnl
])# _LIS_OLD_CONSTS
# =============================================================================

# =============================================================================
# _LIS_SOLARIS_STYLE_CMN_ERR
# -----------------------------------------------------------------------------
# This is another target independent one again.  The default is wrong for
# Linux.  Linux does not issue newlines at the beginning of printks.  Setting
# this to the gcom makefile default, messes up your log files.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SOLARIS_STYLE_CMN_ERR], [dnl
    AH_TEMPLATE([SOLARIS_STYLE_CMN_ERR], [Define for solaris style cmn_err().
	This should always be defined for Linux, otherwise, the newlines
	generated at the beginning of cmn_err() will mess up the appearance of
	your logs.])
    AC_ARG_WITH([solaris-cmn_err],
	AS_HELP_STRING([--with-solaris-cmn_err],
	    [use a Solaris-style cmn_err that puts a newline at the end of the
	    line rather than the beginning @<:@default=yes@:>@]),
	[with_solaris_cmn_err="$withval"],
	[with_solaris_cmn_err='yes'])
    AC_MSG_CHECKING([for lis Solaris style cmn_err])
    if test :"${with_solaris_cmn_err:-yes}" = :yes
    then
	LIS_SOLARIS_STYLE_CMN_ERR=y
	AC_DEFINE([SOLARIS_STYLE_CMN_ERR], [1])
	AC_MSG_RESULT([yes])
    else
	LIS_SOLARIS_STYLE_CMN_ERR=n
	if test :"$lis_cv_target" = :linux
	then
	    AC_MSG_WARN([ *** 
*** You have asked for an SVR4-style cmn_err() on a Linux build using the
*** option --without-solaris-cmn-err.  This is probably not what you want.
*** Consider removing the option, otherwise, your system logs will have a
*** messy appearance due to the extra newline at the beginning of the
*** cmn_err() listing.
*** ])
	fi
	AC_MSG_RESULT([no])
    fi dnl
])# _LIS_SOLARIS_STYLE_CMN_ERR
# =============================================================================

# =============================================================================
# _LIS_DBG_OPT
# -----------------------------------------------------------------------------
# Setup defines and automake variables depending on the setting of the option
# --enable-k-debug.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_DBG_OPT], [dnl
    AH_TEMPLATE([LIS_CONFIG_SAFE], [Setting this macro is pointless (now).  LiS
	uses it to set another useless macro called SAFE which is never checked
	anywhere in the LiS code.  Perhaps Matt will use this macro (again), so
	I left it in here.])
    AH_TEMPLATE([LIS_TESTING], [Setting this macro is pointless (now).  LiS uses
	it to set another useless macro called TEST which is never checked
	anywhere in the LiS code.  Perhaps Matt will use this macro (again), so
	I left it in here.])
    AH_TEMPLATE([LIS_DEBUG], [Setting this macro is pointless (now).  LiS uses
	it to set another useless macro called TEST which is never checked
	anywhere in the LiS code.  Perhaps Matt will use this macro (again), so
	I left it in here.])
    AH_TEMPLATE([LIS_SRC], [LiS source code wants this defined when it is
	configured for debug.])
    AH_TEMPLATE([DEBUG], [Define this macro for debugging source code.])
    AH_TEMPLATE([POISON_MEM], [When defined, slab allocation will poison memory
	and redzone.  This should be done when in debugging or test modes.])
    AH_TEMPLATE([INLINE], [define to inline directive])
    AH_TEMPLATE([STATIC], [define to static directive])
    if test :"${enable_k_safe:-no}" != :no
    then
	AC_DEFINE([LIS_CONFIG_SAFE], [1])
    fi
    if test :"${enable_k_test:-no}" != :no
    then
	AC_DEFINE([LIS_TESTING], [1])
    fi
    if test :"${enable_k_debug:-no}" != :no
    then
	LIS_DBG_OPT=y
	lis_inline=
	lis_static=
	AC_DEFINE([LIS_DEBUG], [1])
	#
	# FIXME: This should actually be the install directory for the source
	# code rather than the build directory that contains the source code.
	#
	AC_DEFINE_UNQUOTED([LIS_SRC], "`(cd $srcdir; pwd)`")
	AC_DEFINE([DEBUG], [1])
	AC_DEFINE([POISON_MEM], [1])
    else
	LIS_DBG_OPT=n
	AC_REQUIRE([AC_C_INLINE])dnl
	lis_inline=$ac_cv_c_inline
	lis_static="static"
    fi
    AC_DEFINE_UNQUOTED([INLINE], [$lis_inline])
    AC_DEFINE_UNQUOTED([STATIC], [$lis_static])
])# _LIS_DBG_OPT
# =============================================================================

# =============================================================================
# _LIS_KMEM_OPTS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KMEM_OPTS], [dnl
    AH_TEMPLATE([USE_KMEM_CACHE], [Define this macro to use Linux kernel memory
	caches for queue structures and other highly used structures for some
	performance improvement.  For the best improvement, use Linux
	Fast-STREAMS.])
    AH_TEMPLATE([USE_KMEM_TIMER], [Define this macro to use the kernel memory
	caches for timer structures rather than sequential list searches.  This
	can really break, especially on SMP and is not safe at all.  It is
	disabled by default.])
    AC_ARG_ENABLE([k-timers],
	AS_HELP_STRING([--enable-k-timers],
	    [enable kernel memory caches for timers for speed.
	    @<:@default=no@:>@]),
	[enable_k_timers="$enableval"],
	[enable_k_timers='no'])
    AC_ARG_ENABLE([k-cache],
	AS_HELP_STRING([--enable-k-cache],
	    [enable kernel memory caches for speed.  @<:@default=no@:>@]),
	[enable_k_cache="$enableval"],
	[enable_k_cache='no'])
    if test :"$enable_k_timers" = :yes ; then
	$enable_k_cache='yes'
    fi
    AC_MSG_CHECKING([for lis kernel caches])
    if test :"$enable_k_cache" = :yes ; then
	LIS_USE_KMEM_CACHE=y
	AC_DEFINE([USE_KMEM_CACHE], [1])
    else
	LIS_USE_KMEM_CACHE=n
    fi
    AC_MSG_RESULT([$enable_k_cache])
    AC_MSG_CHECKING([for lis timer caches])
    if test :"$enable_k_timers" = :yes ; then
	LIS_USE_KMEM_TIMER=y
	AC_DEFINE([USE_KMEM_TIMER], [1])
    else
	LIS_USE_KMEM_TIMER=n
    fi
    AC_MSG_RESULT([$enable_k_timers])
])# _LIS_KMEM_OPTS
# =============================================================================

# =============================================================================
# _LIS_SHLIB
# -----------------------------------------------------------------------------
# Because we are a libtooliszed build, shared libraries are controlled with
# the libtoool options flags.  For the purpose of the gcom makefiles, we just
# set shared libraries to yes.  If you want automatic shared library support
# use this autoconf build process instead.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SHLIB], [dnl
    LIS_SHLIB=y dnl
])# _LIS_SHLIB
# =============================================================================

# =============================================================================
# _LIS_ATOMIC_STATS
# -----------------------------------------------------------------------------
# Determine from --enable-atomic-stats option whether to use atomic_t for
# stats or to use the normal int state.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_ATOMIC_STATS], [dnl
    AC_ARG_ENABLE([atomic-stats],
	AS_HELP_STRING([--enable-atomic-stats],
	    [use atomic_t for module_stat structure elements.
	    @<:@default=no@:>@]),
    [enable_atomic_stats="$enableval"],
    [enable_atomic_states='no'])
    AC_MSG_CHECKING([for lis atomic stats])
    if test :"${enable_atomic_stats:-no}" = :yes
    then
	AC_DEFINE([LIS_ATOMIC_STATS], [1], [Define if you want atomic_t for
	statistics elements.  Otherwise the LiS uses int. It is generally a
	bad idea to use atomic_t as many drivers are unaware of this
	enhancement.])
	ATOMIC_STATS=1
	AC_MSG_RESULT([yes])
    else
	ATOMIC_STATS=0
	AC_MSG_RESULT([no])
    fi
    AC_SUBST([ATOMIC_STATS])dnl
])# _LIS_ATOMIC_STATS
# =============================================================================

# =============================================================================
# _LIS_OUTPUT_CONFIG_IN
# -----------------------------------------------------------------------------
# Output all of the substitutions required by the gcom config.in file.  This
# is so that we can build the config.in file from out config.in.in file.  This
# is useful for any add-on modules that need the LiS configuration file or if
# someone really wants to use the old gcom build process (yuck).
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OUTPUT_CONFIG_IN], [dnl
    AC_CONFIG_FILES([config.in])
dnl -----------------------------------		-----------------------
dnl substitution				config.in variable name
dnl -----------------------------------		-----------------------
    AC_SUBST([LIS_CONFIG])			dnl CONFIG
    AC_SUBST([LIS_SRCDIR])			dnl SRCDIR
    AC_SUBST([LIS_GENCONF])			dnl GENCONF
    AC_SUBST([LIS_CONFIG_STREAMS])		dnl CONFIG_STREAMS
    AC_SUBST([LIS_TARGET])			dnl TARGET
    AC_SUBST([LIS_CROSS_COMPILING])		dnl CROSS_COMPILING
    AC_SUBST([LIS_KSRC])			dnl KSRC
    AC_SUBST([LIS_KINCL])			dnl KINCL
    AC_SUBST([LIS_NOKSRC])			dnl NOKSRC
    AC_SUBST([LIS_USE_RUNNING_KERNEL])		dnl USE_RUNNING_KERNEL
    AC_SUBST([LIS_NKVER])			dnl NKVER
    AC_SUBST([LIS_KVER])			dnl KVER
    AC_SUBST([LIS_KVER_MAJOR])			dnl KVER_MAJOR
    AC_SUBST([LIS_KVER_MINOR])			dnl KVER_MINOR
    AC_SUBST([LIS_KVER_PATCH])			dnl KVER_PATCH
    AC_SUBST([LIS_KVER_PATCHNO])		dnl KVER_PATCHNO
    AC_SUBST([LIS_KVER_MAJORMINOR])		dnl KVER_MAJORMINOR
    AC_SUBST([LIS_KVER_H])			dnl KVER_H
    AC_SUBST([LIS_NKVER_H])			dnl NKVER_H
    AC_SUBST([LIS_KSMP])			dnl KSMP
    AC_SUBST([LIS_KMODULES])			dnl KMODULES
    AC_SUBST([LIS_KMODVERS])			dnl KMODVERS
    AC_SUBST([LIS_LISMODVERS])			dnl LISMODVERS
    AC_SUBST([LIS_LISAUTOCONF])			dnl LISAUTOCONF
    AC_SUBST([LIS_IPV6])			dnl IPV6
    AC_SUBST([LIS_IPV6_MODULE])			dnl IPV6_MODULE
    AC_SUBST([LIS_CONFIG_MK_KERNEL])		dnl CONFIG_MK_KERNEL
    AC_SUBST([LIS_STRMS_QUEUES])		dnl STRMS_QUEUES
    AC_SUBST([LIS_MOD_INST_DIR])		dnl MOD_INST_DIR
    AC_SUBST([LIS_MOD_INSTALL])			dnl MOD_INSTALL
    AC_SUBST([LIS_RH_71_KLUDGE])		dnl RH_71_KLUDGE
    AC_SUBST([LIS_SIGMASKLOCK])			dnl SIGMASKLOCK
    AC_SUBST([LIS_RCVOID])			dnl RCVOID
    AC_SUBST([LIS_MACHINE])			dnl MACHINE
    AC_SUBST([LIS_CC_NAME])			dnl CC_NAME
    AC_SUBST([LIS_CC_VERS])			dnl CC_VERS
    AC_SUBST([LIS_CC_OPTIMIZE])			dnl CC_OPTIMIZE
    AC_SUBST([LIS_CC_OPT2])			dnl CC_OPT2
    AC_SUBST([LIS_KINCL_MACH_GENERIC])		dnl KINCL_MACH_GENERIC
    AC_SUBST([LIS_KINCL_MACH_DEFAULT])		dnl KINCL_MACH_DEFAULT
    AC_SUBST([LIS_GET_EMPTY_INODE])		dnl GET_EMPTY_INODE
    AC_SUBST([LIS_SET_CPUS_ALLOWED])		dnl SET_CPUS_ALLOWED
    AC_SUBST([LIS_INT_PSW])			dnl INT_PSW
    AC_SUBST([LIS_CONFIG_DEV])			dnl CONFIG_DEV
    AC_SUBST([LIS_USE_KMEM_CACHE])		dnl USE_KMEM_CACHE
    AC_SUBST([LIS_USE_KMEM_TIMER])		dnl USE_KMEM_TIMER
    AC_SUBST([LIS_MAJOR_BASE])			dnl LIS_MAJOR_BASE
    AC_SUBST([LIS_CONFIG_INET])			dnl CONFIG_INET
    AC_SUBST([LIS_PKGCOMPILE])			dnl PKGCOMPILE
    AC_SUBST([LIS_PKGMODULES])			dnl PKGMODULES
    AC_SUBST([LIS_SILENT_BUILD])		dnl SILENT_BUILD
    AC_SUBST([LIS_KBUILD])			dnl KBUILD
    AC_SUBST([LIS_KBUILD_NEEDS_MODNAME])	dnl KBUILD_NEEDS_MODNAME
    AC_SUBST([LIS_KBUILD_NEEDS_SYMVERS])	dnl KBUILD_NEEDS_SYMVERS
    AC_SUBST([LIS_MODUTILS])			dnl MODUTILS
    AC_SUBST([LIS_MODULE_INIT_TOOLS])		dnl MODULE_INIT_TOOLS
    AC_SUBST([LIS_KSYMVERS])			dnl KSYMVERS
    AC_SUBST([LIS_CCREGPARM])			dnl CCREGPARM
    AC_SUBST([LIS_STREAMS_REGPARM])		dnl STREAMS_REGPARM
    AC_SUBST([LIS_OLD_CONSTS])			dnl LIS_OLD_CONSTS
    AC_SUBST([LIS_SOLARIS_STYLE_CMN_ERR])	dnl SOLARIS_STYLE_CMN_ERR
    AC_SUBST([LIS_DBG_OPT])			dnl DBG_OPT
    AC_SUBST([LIS_SHLIB])			dnl LIS_SHLIB
    AC_SUBST([LIS_PACKAGE])			dnl package
    AC_SUBST([LIS_PREFIX])			dnl prefix
    AC_SUBST([LIS_EXECPREFIX])			dnl execprefix
    AC_SUBST([LIS_INCLUDEDIR])			dnl includedir
    AC_SUBST([LIS_LIBDIR])			dnl libdir
    AC_SUBST([LIS_BINDIR])			dnl bindir
    AC_SUBST([LIS_SBINDIR])			dnl sbindir
    AC_SUBST([LIS_SYSCONFDIR])			dnl sysconfdir
    AC_SUBST([LIS_DATADIR])			dnl datadir
    AC_SUBST([LIS_PKGINCLUDEDIR])		dnl pkgincludedir
    AC_SUBST([LIS_PKGLIBDIR])			dnl pkglibdir
    AC_SUBST([LIS_PKGDATADIR])			dnl pkgdatadir
    AC_SUBST([LIS_PKGSRCDIR])			dnl pkgsrcdir
    AC_SUBST([LIS_MANDIR])			dnl mandir
    AC_SUBST([LIS_MODSUBDIR])			dnl modsubdir
    AC_SUBST([LIS_MAKE_DIRS])			dnl LIS_MAKE_DIRS
dnl -----------------------------------		-----------------------
])#_LIS_OUTPUT_CONFIG_IN
# =============================================================================

# =============================================================================
# _LIS_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    streams_cv_lis_config="${pkg_bld}/include/sys/LiS/config.h"
    streams_cv_lis_includes="${pkg_bld}/include ${pkg_src}/include"
    streams_cv_lis_ldadd="${pkg_bld}/libstreams.la"
    streams_cv_lis_ldflags="-L${pkg_bld}/.libs/"
    streams_cv_lis_ldadd32="${pkg_bld}/lib32/libstreams.la"
    streams_cv_lis_ldflags32="-L${pkg_bld}/lib32/.libs/"
    streams_cv_lis_manpath="${pkg_bld}/doc/man"
    streams_cv_lis_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    streams_cv_lis_modmap="${pkg_bld}/Modules.map"
    streams_cv_lis_symver="${pkg_bld}/Module.symvers"
    streams_cv_lis_version="${PACKAGE_EPOCH}:${PACKAGE_VERSION}-${PACKAGE_RELEASE}"
    streams_cv_config="$streams_cv_lis_config"
    streams_cv_includes="$streams_cv_lis_includes"
dnl streams_cv_ldadd="$streams_cv_lis_ldadd"
dnl streams_cv_ldadd32="$streams_cv_lis_ldadd32"
dnl streams_cv_ldflags="$streams_cv_lis_ldflags"
dnl streams_cv_ldflags32="$streams_cv_lis_ldflags32"
dnl streams_cv_manpath="$streams_cv_lis_manpath"
    streams_cv_modversions="$streams_cv_lis_modversions"
dnl streams_cv_modmap="$streams_cv_lis_modmap"
dnl streams_cv_symver="$streams_cv_lis_symver"
    streams_cv_version="$streams_cv_lis_version"
    streams_cv_package="LiS"
])#_LIS_CONFIG
# =============================================================================

# =============================================================================
# _LIS_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_STRCONF], [dnl
    case "$lis_cv_target" in
	user)
	    strconf_cv_stem='Config.user'
	    ;;
	linux | *)
	    strconf_cv_stem='Config'
	    ;;
    esac
    strconf_cv_input='Config.master'
    strconf_cv_majbase=231
    strconf_cv_midbase=1
    strconf_cv_config='include/sys/LiS/config.h'
    strconf_cv_modconf='head/modconf.inc'
    strconf_cv_drvconf='drvconf.mk'
    strconf_cv_confmod='conf.modules'
    strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes='util/linux/strmakenodes.c'
    strconf_cv_strsetup='strsetup.conf'
    strconf_cv_strload='strload.conf'
    strconf_cv_package='LiS'
    strconf_cv_minorbits="${linux_cv_minorbits:-8}"
    _STRCONF
])# _LIS_STRCONF
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
