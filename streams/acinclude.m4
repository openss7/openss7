# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.139 $) $Date: 2007/03/05 23:01:56 $
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
# Last Modified $Date: 2007/03/05 23:01:56 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.139  2007/03/05 23:01:56  brian
# - checking in release changes
#
# Revision 0.9.2.138  2007/03/04 23:41:36  brian
# - additional include path
#
# Revision 0.9.2.137  2007/03/04 23:30:04  brian
# - corrected modversions directory
#
# Revision 0.9.2.136  2007/03/04 23:14:25  brian
# - better search for modversions
#
# Revision 0.9.2.135  2007/03/03 08:39:55  brian
# - corrections for STREAMS_VERSION
#
# Revision 0.9.2.134  2007/03/02 09:23:19  brian
# - build updates and esballoc() feature
#
# Revision 0.9.2.133  2007/02/28 06:30:47  brian
# - updates and corrections, #ifdef instead of #if
#
# Revision 0.9.2.132  2007/01/09 10:56:43  brian
# - typo
#
# Revision 0.9.2.131  2007/01/02 16:32:02  brian
# - updates for release, disable streams-bcm by default
#
# Revision 0.9.2.130  2006/12/18 07:32:35  brian
# - lfs device names, autoload clone minors, device numbering, missing manpages
#
# Revision 0.9.2.129  2006/11/26 15:27:32  brian
# - testing and corrections to strlog capabilities
#
# Revision 0.9.2.128  2006/10/30 06:40:08  brian
# - changes to handle missing linux/compile.h on SuSE
#
# Revision 0.9.2.127  2006/10/29 13:11:40  brian
# - final changes for FC5 2.6.18 w/ inode diet
#
# Revision 0.9.2.126  2006/10/28 01:08:31  brian
# - better support for 2.6.18 inode diet
#
# Revision 0.9.2.125  2006/10/27 23:19:31  brian
# - changes for 2.6.18 kernel
#
# Revision 0.9.2.124  2006/10/12 10:22:43  brian
# - removed redundant debug flags
#
# Revision 0.9.2.123  2006/09/18 13:20:10  brian
# - better directory detection
#
# Revision 0.9.2.122  2006/09/18 01:15:23  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.121  2006/08/16 07:42:23  brian
# - added checks for non-exported kthread functions under SLES 9
#
# Revision 0.9.2.120  2006/07/25 06:39:04  brian
# - expanded minor device numbers and optimization and locking corrections
#
# Revision 0.9.2.119  2006/07/24 09:01:09  brian
# - results of udp2 optimizations
#
# Revision 0.9.2.118  2006/07/03 02:59:21  brian
# - updated documentation for release
#
# Revision 0.9.2.117  2006/05/08 03:12:28  brian
# - added module id base to strconf
#
# Revision 0.9.2.116  2006/03/21 13:24:15  brian
# - added problem report checks
#
# Revision 0.9.2.115  2006/03/05 03:51:19  brian
# - aligned to LiS
#
# Revision 0.9.2.114  2006/03/03 10:57:09  brian
# - 32-bit compatibility support, updates for release
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
# AC_LFS
# -----------------------------------------------------------------------------
AC_DEFUN([AC_LFS], [dnl
    _OPENSS7_PACKAGE([STREAMS], [Linux Fast-STREAMS])
    _LFS_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/streams-core.postinst
		     debian/streams-core.postrm
		     debian/streams-core.preinst
		     debian/streams-core.prerm
		     debian/streams-devel.preinst
		     debian/streams-dev.postinst
		     debian/streams-dev.preinst
		     debian/streams-dev.prerm
		     debian/streams-doc.postinst
		     debian/streams-doc.preinst
		     debian/streams-doc.prerm
		     debian/streams-init.postinst
		     debian/streams-init.postrm
		     debian/streams-init.preinst
		     debian/streams-init.prerm
		     debian/streams-lib.preinst
		     debian/streams-source.preinst
		     debian/streams-util.preinst
		     src/util/modutils/streams
		     include/sys/streams/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _LFS_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-DLFS=1'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    if test :${linux_cv_k_ko_modules:-no} = :no ; then
	if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	    PKG_MODFLAGS='-include ${top_builddir}/${MODVERSIONS_H}'
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
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _LFS_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_LFS
# =============================================================================

# =============================================================================
# _LFS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_OPTIONS], [dnl
])# _LFS_OPTIONS
# =============================================================================

# =============================================================================
# _LFS_SETUP_OPTIMIZE
# -----------------------------------------------------------------------------
# Optimizations and assertions are set up differently now.
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_OPTIMIZE], [dnl
    case "$linux_cv_optimize" in
	(size)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OPTIMIZE_SIZE], [1], [Define when
		optimizing for size.  That is -Os was passed to the compiler.])
	    ;;
	(speed)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OPTIMIZE_SPEED], [1], [Define when
		optimizing for speed.  That is -O3 was passed to the compiler.])
	    ;;
	(quick)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OPTIMIZE_NONE], [1], [Define when
		optimizing for profiling.  That is -O0 -g was passed to the
		compiler.])
	    ;;
	(normal | *)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OPTIMIZE_NORMAL], [1], [Define
		when performing normal optimization.  That is -O2 was passed to
		the compiler.])
	    ;;
    esac
])# _LFS_SETUP_OPTIMIZE
# =============================================================================

# =============================================================================
# _LFS_SETUP_DEBUG
# -----------------------------------------------------------------------------
# Optimizations and assertions are set up differently now.
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_DEBUG], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    case "$linux_cv_debug" in
    _DEBUG)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DEBUG], [], [Define to perform
			    internal structure tracking within the STREAMS
			    executive as well as to provide additional /proc
			    filesystem files for examining internal
			    structures.])
	;;
    _TEST)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TEST], [], [Define to perform
			    performance testing with debugging.  This mode does
			    not dump massive amounts of information into system
			    logs, but peforms all assertion checks.])
	;;
    _SAFE)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAFE], [], [Define to perform
			    fundamental assertion checks.  This is a safer mode
			    of operation.])
	;;
    _NONE | *)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NONE], [], [Define to perform no
			    assertion checks but report software errors.  This
			    is the smallest footprint, highest performance mode
			    of operation.])
	;;
    esac
])# _LFS_SETUP_DEBUG
# =============================================================================

# =============================================================================
# _LFS_SETUP_IRQ
# -----------------------------------------------------------------------------
# A note about this feature:  bottom half suppression is more expensive than
# interrupt suppression for the following reason: when bottom half suppression
# is released, pending bottom half processes are run causing cache pollution.
# When interrupts are released, no other process is run, avoiding memory cache
# pollution.  For performance, memory cache pollution is more important than
# the cycle cost if interrupt suppression.  Therefore, this feature is only
# (marginally) useful when doing interrupt based profiling one wishes sample
# data during the interrupt suppression period to be available.
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_IRQ], [dnl
    AC_ARG_ENABLE([streams-irq],
	AS_HELP_STRING([--disable-streams-irq],
	    [disable STREAMS irq suppression.
	     @<:@default=enabled@:>@]),
	    [enable_streams_irq="$enableval"],
	    [enable_streams_irq='yes'])
    AC_CACHE_CHECK([for STREAMS irq suppression], [lfs_streams_irq], [dnl
	lfs_streams_irq="${enable_streams_irq:-yes}"])
    case ${lfs_streams_irq:-yes} in
	(no)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NOIRQ], [1], [When defined]
	    AC_PACKAGE_TITLE [will not suppress interrupts for stream or queue
	    lock protection.  When defined a driver's put() procedure must not
	    be called from an ISR and must only be called from bottom half or
	    tasklets.  Bottom half locking is more expensive: don't enable
	    this except for interrupt based profiling.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_NOIRQ], [test :${lfs_streams_irq:-yes} = :no])
])# _LFS_SETUP_IRQ
# =============================================================================

# =============================================================================
# _LFS_SETUP_STATS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_STATS], [dnl
    AC_ARG_ENABLE([streams-stats],
	AS_HELP_STRING([--enable-streams-stats],
	    [enable STREAMS stats counting.
	     @<:@default=disabled@:>@]),
	    [enable_streams_stats="$enableval"],
	    [enable_streams_stats='no'])
    AC_CACHE_CHECK([for STREAMS stats counting], [lfs_streams_stats], [dnl
	lfs_streams_stats="${enable_streams_stats:-no}"])
    case ${lfs_streams_stats:-no} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DO_STATS], [1], [When defined]
	    AC_PACKAGE_TITLE [will perform STREAMS entry point counting in the
	    module_stat structure if a pointer is provided by the module or
	    driver.  This is a low cost item and is enabled by default.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_DO_STATS], [test :${lfs_streams_stats:-no} = :yes])
])# _LFS_SETUP_STATS
# =============================================================================

# =============================================================================
# _LFS_SETUP_SYNCQS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_SYNCQS], [dnl
    AC_ARG_ENABLE([streams-syncqs],
	AS_HELP_STRING([--enable-streams-syncqs],
	    [enable STREAMS synchronization queues.
	    @<:@default=disabled@:>@]),
	    [enable_streams_syncqs="$enableval"],
	    [enable_streams_syncqs='no'])
    _LINUX_CHECK_KERNEL_CONFIG([for STREAMS smp kernel], [CONFIG_SMP])dnl
    AC_CACHE_CHECK([for STREAMS synchronization], [lfs_streams_syncqs], [dnl
	if test :${linux_cv_CONFIG_SMP:-no} = :yes
	then
	    lfs_streams_syncqs="${enable_streams_syncqs:-no}"
	else
	    lfs_streams_syncqs='no'
	fi])
    case ${lfs_streams_syncqs:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SYNCQS], [1], [When defined]
	    AC_PACKAGE_TITLE [will include support for synchronization queues
	    and levels.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_SYNCQS], [test :${lfs_streams_syncqs:-yes} = :yes])
])# _LFS_SETUP_SYNCQS
# =============================================================================

# =============================================================================
# _LFS_SETUP_KTHREADS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_KTHREADS], [dnl
    AC_ARG_ENABLE([streams-kthreads],
	AS_HELP_STRING([--disable-streams-kthreads],
	    [disable STREAMS kernel threads.
	    @<:@default=enabled@:>@]),
	    [enable_streams_kthreads="$enableval"],
	    [enable_streams_kthreads='yes'])
    AC_CACHE_CHECK([for STREAMS kernel threads], [lfs_streams_kthreads], [dnl
	lfs_streams_kthreads="${enable_streams_kthreads:-no}"
	])
    case ${lfs_streams_kthreads:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_KTHREADS], [1], [When defined]
	    AC_PACKAGE_TITLE [will include use kernel threads for the STREAMS
	    scheduler; when undefined,] AC_PACKAGE_TITLE [will use softirqs
	    for the STREAMS scheduler.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_KTHREADS], [test :${lfs_streams_kthreads:-yes} = :yes])
])# _LFS_SETUP_KTHREADS
# =============================================================================

# =============================================================================
# _LFS_SETUP_UTILS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_UTILS], [dnl
    AC_ARG_ENABLE([streams-utils],
	AS_HELP_STRING([--disable-streams-utils],
	    [disable additional STREAMS utilities.
	    @<:@default=enabled@:>@]),
	    [enable_streams_utils="$enableval"],
	    [enable_streams_utils='yes'])
    AC_CACHE_CHECK([for STREAMS utilities], [lfs_streams_utils], [dnl
	lfs_streams_utils="${enable_streams_utils:-yes}"
	])
    case ${lfs_streams_utils:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTILS], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include additional STREAMS utilities.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_UTILS], [test :${lfs_streams_utils:-yes} = :yes])
])# _LFS_SETUP_UTILS
# =============================================================================

# =============================================================================
# _LFS_SETUP_COMPILE
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_COMPILE], [dnl
    AC_ARG_ENABLE([big-compile],
	AS_HELP_STRING([--disable-big-compile],
	    [disable compiling as one big computational unit,
	    @<:@default=enabled@:>@]),
	    [enable_big_compile="$enableval"],
	    [enable_big_comiple='yes'])
    AC_CACHE_CHECK([for STREAMS big compile], [lfs_big_compile], [dnl
	lfs_big_compile="${enable_big_compile:-yes}"
	])
    case ${lfs_big_compile:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SEPARATE_COMPILE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will compile streams objects separately.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_SEPARATE_COMPILE], [test :${lfs_big_compile:-yes} != :yes])
])# _LFS_SETUP_COMPILE
# =============================================================================

# =============================================================================
# _LFS_SETUP_MODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_MODULES], [dnl
    AC_ARG_ENABLE([module-sth],
	AS_HELP_STRING([--enable-module-sth],
	    [enable module sth for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_sth="$enableval"],
	    [if test :${lfs_big_compile:-yes} = :yes ; then enable_module_sth='yes' ; else enable_module_sth='module' ; fi])
    AC_ARG_ENABLE([module-bufmod],
	AS_HELP_STRING([--enable-module-bufmod],
	    [enable bufmod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_bufmod="$enableval"],
	    [enable_module_bufmod='module'])
    AC_ARG_ENABLE([module-nullmod],
	AS_HELP_STRING([--enable-module-nullmod],
	    [enable nullmod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_nullmod="$enableval"],
	    [enable_module_nullmod='module'])
    AC_ARG_ENABLE([module-pipemod],
	AS_HELP_STRING([--enable-module-pipemod],
	    [enable pipemod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_pipemod="$enableval"],
	    [enable_module_pipemod='module'])
    AC_ARG_ENABLE([module-connld],
	AS_HELP_STRING([--enable-module-connld],
	    [enable connld module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_connld="$enableval"],
	    [enable_module_connld='module'])
    AC_ARG_ENABLE([module-sc],
	AS_HELP_STRING([--enable-module-sc],
	    [enable sc module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_sc="$enableval"],
	    [enable_module_sc='module'])
    AC_ARG_ENABLE([module-testmod],
	AS_HELP_STRING([--enable-module-testmod],
	    [enable testmod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_testmod="$enableval"],
	    [enable_module_testmod='module'])
    AC_CACHE_CHECK([for STREAMS module sth], [lfs_module_sth], [dnl
	lfs_module_sth="${enable_module_sth:-module}"
	if test :$lfs_module_sth = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_sth='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS module bufmod], [lfs_module_bufmod], [dnl
	lfs_module_bufmod="${enable_module_bufmod:-module}"
	if test :$lfs_module_bufmod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_bufmod='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS module nullmod], [lfs_module_nullmod], [dnl
	lfs_module_nullmod="${enable_module_nullmod:-module}"
	if test :$lfs_module_nullmod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_nullmod='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS module pipemod], [lfs_module_pipemod], [dnl
	lfs_module_pipemod="${enable_module_pipemod:-module}"
	if test :$lfs_module_pipemod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_pipemod='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS module connld], [lfs_module_connld], [dnl
	lfs_module_connld="${enable_module_connld:-module}"
	if test :$lfs_module_connld = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_connld='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS module sc], [lfs_module_sc], [dnl
	lfs_module_sc="${enable_module_sc:-module}"
	if test :$lfs_module_sc = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_sc='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS module testmod], [lfs_module_testmod], [dnl
	lfs_module_testmod="${enable_module_testmod:-module}"
	if test :$lfs_module_testmod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_testmod='yes'
	fi])
    case ${lfs_module_sth:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_STH], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the sth module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sth module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_STH_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the sth module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sth module as a standalone loadable kernel
	    module.])
	    ;;
    esac
dnl --------------------------------------
    case ${lfs_module_bufmod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_BUFMOD], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the bufmod module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the bufmod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_BUFMOD_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the bufmod module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the bufmod module as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_module_nullmod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULLMOD], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the nullmod module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the nullmod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULLMOD_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the nullmod module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the nullmod module as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_module_pipemod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the pipemod module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the pipemod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the pipemod module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the pipemod module as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_module_connld:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the connld module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the connld module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the connld module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the connld module as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_module_sc:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the sc module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sc module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the sc module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sc module as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_module_testmod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TESTMOD], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the testmod module for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the testmod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TESTMOD_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the testmod module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the testmod module as a standalone loadable kernel
	    module.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_STH],		[test :${lfs_module_sth:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_STH_MODULE],		[test :${lfs_module_sth:-module}	= :module])
dnl ===========================
    AM_CONDITIONAL([CONFIG_STREAMS_BUFMOD],		[test :${lfs_module_bufmod:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_BUFMOD_MODULE],	[test :${lfs_module_bufmod:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NULLMOD],		[test :${lfs_module_nullmod:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NULLMOD_MODULE],	[test :${lfs_module_nullmod:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD],		[test :${lfs_module_pipemod:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD_MODULE],	[test :${lfs_module_pipemod:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD],		[test :${lfs_module_connld:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD_MODULE],	[test :${lfs_module_connld:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SC],			[test :${lfs_module_sc:-module}		= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SC_MODULE],		[test :${lfs_module_sc:-module}		= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_TESTMOD],		[test :${lfs_module_testmod:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_TESTMOD_MODULE],	[test :${lfs_module_testmod:-module}	= :module])
])# _LFS_SETUP_MODULES
# =============================================================================

# =============================================================================
# _LFS_SETUP_DRIVERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_DRIVERS], [dnl
    AC_ARG_ENABLE([driver-clone],
	AS_HELP_STRING([--enable-driver-clone],
	    [enable clone driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_clone="$enableval"],
	    [if test :${lfs_big_compile:-yes} = :yes ; then enable_driver_clone='yes' ; else enable_driver_clone='module' ; fi])
    AC_ARG_ENABLE([driver-echo],
	AS_HELP_STRING([--enable-driver-echo],
	    [enable echo driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_echo="$enableval"],
	    [enable_driver_echo='module'])
    AC_ARG_ENABLE([driver-fifo],
	AS_HELP_STRING([--enable-driver-fifo],
	    [enable fifo driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_fifo="$enableval"],
	    [enable_driver_fifo='module'])
    AC_ARG_ENABLE([driver-log],
	AS_HELP_STRING([--enable-driver-log],
	    [enable log driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_log="$enableval"],
	    [enable_driver_log='module'])
    AC_ARG_ENABLE([driver-loop],
	AS_HELP_STRING([--enable-driver-loop],
	    [enable loop driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_loop="$enableval"],
	    [enable_driver_loop='module'])
    AC_ARG_ENABLE([driver-nsdev],
	AS_HELP_STRING([--enable-driver-nsdev],
	    [enable nsdev driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_nsdev="$enableval"],
	    [enable_driver_nsdev='module'])
    AC_ARG_ENABLE([driver-mux],
	AS_HELP_STRING([--enable-driver-mux],
	    [enable mux driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_mux="$enableval"],
	    [enable_driver_mux='module'])
    AC_ARG_ENABLE([driver-nuls],
	AS_HELP_STRING([--enable-driver-nuls],
	    [enable nuls driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_nuls="$enableval"],
	    [enable_driver_nuls='module'])
    AC_ARG_ENABLE([driver-pipe],
	AS_HELP_STRING([--enable-driver-pipe],
	    [enable pipe driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_pipe="$enableval"],
	    [enable_driver_pipe='module'])
    AC_ARG_ENABLE([driver-sad],
	AS_HELP_STRING([--enable-driver-sad],
	    [enable sad driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_sad="$enableval"],
	    [enable_driver_sad='module'])
    AC_ARG_ENABLE([driver-sfx],
	AS_HELP_STRING([--enable-driver-sfx],
	    [enable sfx driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_sfx="$enableval"],
	    [enable_driver_sfx='module'])
    AC_ARG_ENABLE([driver-spx],
	AS_HELP_STRING([--enable-driver-spx],
	    [enable spx driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_spx="$enableval"],
	    [enable_driver_spx='module'])
    AC_CACHE_CHECK([for STREAMS driver clone], [lfs_driver_clone], [dnl
	lfs_driver_clone="${enable_driver_clone:-module}"
	if test :$lfs_driver_clone = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_clone='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver echo],  [lfs_driver_echo], [dnl
	lfs_driver_echo="${enable_driver_echo:-module}"
	if test :$lfs_driver_echo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_echo='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver fifo],  [lfs_driver_fifo], [dnl
	lfs_driver_fifo="${enable_driver_fifo:-module}"
	if test :$lfs_driver_fifo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_fifo='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver log],   [lfs_driver_log], [dnl
	lfs_driver_log="${enable_driver_log:-module}"
	if test :$lfs_driver_log = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_log='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver loop],  [lfs_driver_loop], [dnl
	lfs_driver_loop="${enable_driver_loop:-module}"
	if test :$lfs_driver_loop = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_loop='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver nsdev], [lfs_driver_nsdev], [dnl
	lfs_driver_nsdev="${enable_driver_nsdev:-module}"
	if test :$lfs_driver_nsdev = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_nsdev='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver mux],  [lfs_driver_mux], [dnl
	lfs_driver_mux="${enable_driver_mux:-module}"
	if test :$lfs_driver_mux = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_mux='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver nuls],  [lfs_driver_nuls], [dnl
	lfs_driver_nuls="${enable_driver_nuls:-module}"
	if test :$lfs_driver_nuls = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_nuls='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver pipe],  [lfs_driver_pipe], [dnl
	lfs_driver_pipe="${enable_driver_pipe:-module}"
	if test :$lfs_driver_pipe = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_pipe='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver sad],   [lfs_driver_sad], [dnl
	lfs_driver_sad="${enable_driver_sad:-module}"
	if test :$lfs_driver_sad = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_sad='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver sfx], [lfs_driver_sfx], [dnl
	lfs_driver_sfx="${enable_driver_sfx:-module}"
	if test :$lfs_driver_sfx = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_sfx='yes'
	fi])
    AC_CACHE_CHECK([for STREAMS driver spx], [lfs_driver_spx], [dnl
	lfs_driver_spx="${enable_driver_spx:-module}"
	if test :$lfs_driver_spx = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_spx='yes'
	fi])
dnl ------------------------------------
    case ${lfs_driver_clone:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the clone driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the clone driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the clone driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the clone driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_echo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the echo driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the echo driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the echo driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the echo driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_fifo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the fifo driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the fifo driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the fifo driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the fifo driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_log:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the log driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the log driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the log driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the log driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_loop:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the loop driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the loop driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the loop driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the loop driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_nsdev:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the nsdev driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the nsdev driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the nsdev driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the nsdev driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_mux:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MUX], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the mux driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the mux driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MUX_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the mux driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the mux driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_nuls:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the nuls driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the nuls driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the nuls driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the nuls driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_pipe:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the pipe driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the pipe driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the pipe driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the pipe driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_sad:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the sad driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sad driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the sad driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sad driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_sfx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SFX], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the sfx driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sfx driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SFX_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the sfx driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sfx driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
    case ${lfs_driver_spx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SPX], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the spx driver for linkage with STREAMS.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the spx driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SPX_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the spx driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the spx driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
dnl =================================
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE],		[test :${lfs_driver_clone:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE_MODULE],	[test :${lfs_driver_clone:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO],		[test :${lfs_driver_echo:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO_MODULE],	[test :${lfs_driver_echo:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO],		[test :${lfs_driver_fifo:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO_MODULE],	[test :${lfs_driver_fifo:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG],		[test :${lfs_driver_log:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG_MODULE],		[test :${lfs_driver_log:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP],		[test :${lfs_driver_loop:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP_MODULE],	[test :${lfs_driver_loop:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV],		[test :${lfs_driver_nsdev:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV_MODULE],	[test :${lfs_driver_nsdev:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_MUX],		[test :${lfs_driver_mux:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_MUX_MODULE],		[test :${lfs_driver_mux:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS],		[test :${lfs_driver_nuls:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS_MODULE],	[test :${lfs_driver_nuls:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE],		[test :${lfs_driver_pipe:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE_MODULE],	[test :${lfs_driver_pipe:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD],		[test :${lfs_driver_sad:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD_MODULE],		[test :${lfs_driver_sad:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SFX],		[test :${lfs_driver_sfx:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SFX_MODULE],		[test :${lfs_driver_sfx:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SPX],		[test :${lfs_driver_spx:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SPX_MODULE],		[test :${lfs_driver_spx:-module}	= :module])
])# _LFS_SETUP_DRIVERS
# =============================================================================

# =============================================================================
# _LFS_SETUP_FIFOS
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_FIFOS], [dnl
    AC_ARG_ENABLE([streams-fifos],
	AS_HELP_STRING([--enable-streams-fifos],
	    [enable override of system fifos with STREAMS-based fifos.
	    @<:@default=no@:>@]),
	[enable_streams_fifos="$enableval"],
	[enable_streams_fifos='no'])
    if test :"$enable_streams_fifos" = :yes ; then
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OVERRIDE_FIFOS], [1], [When defined,]
		AC_PACKAGE_TITLE [will override the Linux system defined
		FIFOs at startup.  This should be used with care for a while,
		until streams FIFOs are proven.])
    fi
])# _LFS_SETUP_FIFOS
# =============================================================================

# =============================================================================
# _LFS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LFS_CHECK_KERNEL
    _LFS_SETUP_OPTIMIZE
    _LFS_SETUP_COMPAT
    _LFS_SETUP_MODULE
    _LFS_SETUP_IRQ
    _LFS_SETUP_STATS
    _LFS_SETUP_SYNCQS
    _LFS_SETUP_KTHREADS
    _LFS_SETUP_UTILS
    _LFS_SETUP_COMPILE
    _LFS_SETUP_MODULES
    _LFS_SETUP_DRIVERS
    _LFS_SETUP_FIFOS
    _LFS_SETUP_DEBUG
])# _LFS_SETUP
# =============================================================================

# =============================================================================
# _LFS_SETUP_COMPAT
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_COMPAT], [dnl
    AC_CACHE_CHECK([for STREAMS binary compatibilty mode], [lfs_cv_bcm], [dnl
	AC_ARG_ENABLE([streams-bcm],
	    AS_HELP_STRING([--enable-streams-bcm],
		[disable STREAMS binary compatibility mode.
		@<:@default=disabled@:>@]),
		[enable_streams_bcm="$enableval"],
		[enable_streams_bcm='no'])
	lfs_cv_bcm="${enable_streams_bcm:-no}"])
    AH_TEMPLATE([CONFIG_STREAMS_LIS_BCM], [Defined when] AC_PACKAGE_TITLE [was
	compiled for LiS Binary Compatibility.])
    AH_VERBATIM([streamscall], m4_text_wrap([Use this macro like fastcall.  It
	is set to an attribute with the number of parameters passed in registers
	to STREAMS callouts (qi_putp, qi_srvp, qi_qopen, qi_qclose, qi_admin).
	In binary compatibility mode, this sets the number of parameters passed
	in registers to zero.  Otherwise, it defaults to the CONFIG_REPARM
	setting for the kernel. */], [   ], [/* ])[
#if defined __i386__ || defined __x86_64__ || defined __k8__
#undef streamscall
#else
#define streamscall
#endif])
    AH_VERBATIM([STREAMSCALL], m4_text_wrap([Use this macro like FASTCALL().  It
	is set to an attribute with the number of parameters passed in registers
	to STREAMS callouts (qi_putp, qi_srvp, qi_qopen, qi_qclose, qi_admin).
	In binary compatibility mode, this sets the number of parameters passed
	in registers to zero.  Otherwise, it defaults to the CONFIG_REPARM
	setting for the kernel. */], [   ], [/* ])[
#if defined __i386__ || defined __x86_64__ || defined __k8__
#undef STREAMSCALL
#else
#define STREAMSCALL(__x) __x
#endif])
    if test :"${lfs_cv_bcm:-no}" = :yes
    then
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LIS_BCM], [1])
	AC_DEFINE_UNQUOTED([streamscall], [__attribute__((__regparm__(0)))])
    else
	AC_DEFINE_UNQUOTED([streamscall], [__attribute__((__regparm__(3)))])
    fi
    AC_DEFINE_UNQUOTED([STREAMSCALL(__x)], [__x streamscall])
])# _LFS_SETUP_COMPAT
# =============================================================================

# =============================================================================
# _LFS_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_SETUP_MODULE], [dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MODULE], [1], [When defined, STREAMS
	    is being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS], [1], [When defined, STREAMS is
	    being compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([CONFIG_STREAMS_MODULE],	[test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([CONFIG_STREAMS],		[test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _LFS_SETUP_MODULE
# =============================================================================

# =============================================================================
# _LFS_CHECK_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_CHECK_KERNEL], [dnl
    _LFS_CONFIG_KERNEL
    _LFS_CONFIG_FATTACH
    _LFS_CONFIG_LIS
    _LFS_CONFIG_LFS
])# _LFS_CHECK_KERNEL
# =============================================================================

# =============================================================================
# _LFS_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuraiton checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_KERNEL], [dnl
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
dnl
dnl On the inode diet it is necessary to forget any i_cdev pointer before
dnl using i_pipe when an external inode is being reused for character device
dnl based FIFOS.  2.6.18 does not export tasklist_lock
dnl
    _LINUX_KERNEL_SYMBOLS([cd_forget, tasklist_lock])
dnl
dnl When the module_text_address() symbol is available, we can pull some
dnl sneaky tricks in the esballoc() and freeb() functions to do proper module
dnl reference counting for the free routine callback function.
dnl
    _LINUX_KERNEL_SYMBOLS([module_text_address])
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
])# _LFS_CHECK_KERNEL
# =============================================================================

# =============================================================================
# _LFS_CONFIG_FATTACH
# -----------------------------------------------------------------------------
# 
# symbols to implement fattach
# -----------------------------------------------------------------------------
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
# -----------------------------------------------------------------------------
# path_init             <--- exported
# path_release          <--- exported
# check_mnt             <=== static, can be ripped
#
# symbols to override system fifos
# -----------------------------------------------------------------------------
# def_fifo_fops         <-- extern, declared in <linux/fs.h>
# 
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_FATTACH], [dnl
    lfs_pipe=yes
    lfs_fattach=yes
    _LINUX_KERNEL_SYMBOLS([namespace_sem, mount_sem, check_mnt])
    _LINUX_KERNEL_SYMBOL_EXPORT([clone_mnt], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([graft_tree], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([do_umount], [lfs_fattach=no; lfs_pipe=no])
    AC_CACHE_CHECK([for kernel symbol support for fattach/fdetach], [lfs_cv_fattach], [dnl
	lfs_cv_fattach="$lfs_fattach" ])
    if test :"${lfs_cv_fattach:-no}" != :no ; then
	AC_DEFINE([HAVE_KERNEL_FATTACH_SUPPORT], [1],
	[If the addresses for the necessary symbols above are defined, then
	define this to include fattach/fdetach support.])
    fi
    AC_CACHE_CHECK([for kernel symbol support for pipe], [lfs_cv_pipe], [dnl
	lfs_cv_pipe="$lfs_pipe" ])
    if test :${lfs_cv_pipe:-no} != :no ; then
	AC_DEFINE([HAVE_KERNEL_PIPE_SUPPORT], [1],
	[If the addresses for the necessary symbols above are defined, then
	define this to include pipe support.])
    fi
    _LINUX_KERNEL_SYMBOL_EXPORT([def_fifo_fops])
])# _LFS_CONFIG_FATTACH
# =============================================================================

# =============================================================================
_LFS_CONFIG_LIS
# -----------------------------------------------------------------------------
# symbols to rip for STREAMS support (without system call generation)
# -----------------------------------------------------------------------------
# sys_unlink            <-- extern, not declared
# sys_mknod             <-- extern, not declared
# sys_umount            <-- extern, not declared
# sys_mount             <-- extern, not declared
# pcibios_init          <-- extern, declared in <linux/pci.h>
# -----------------------------------------------------------------------------
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
# =============================================================================

# =============================================================================
# _LFS_CONFIG_LFS
# -----------------------------------------------------------------------------
# symbols to rip for Linux Fast STREAMS
# -----------------------------------------------------------------------------
# file_move             <-- extern, declared in <linux/fs.h>
# open_softirq          <-- extern, declared in <linux/interrupt.h>
# sock_readv_writev     <-- extern, declared in <linux/net.h>
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG_LFS], [dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([file_move])
    _LINUX_KERNEL_SYMBOL_EXPORT([file_kill])
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
    _LINUX_KERNEL_SYMBOL_EXPORT([do_exit])
])# _LFS_CONFIG_LFS
# =============================================================================

# =============================================================================
# _LFS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_OUTPUT], [dnl
    _LFS_CONFIG
    _LFS_STRCONF
    AM_CONDITIONAL(WITH_LFS, true)dnl
    AM_CONDITIONAL(WITH_LIS, false)dnl
])# _LFS_OUTPUT
# =============================================================================

# =============================================================================
# _LFS_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    streams_cv_lfs_config="${pkg_bld}/include/sys/streams/config.h"
    streams_cv_lfs_includes="${pkg_bld}/include ${pkg_bld}/include ${pkg_src}/include"
    streams_cv_lfs_ldadd="${pkg_bld}/libstreams.la"
    streams_cv_lfs_ldflags="-L${pkg_bld}/.libs/"
    streams_cv_lfs_ldadd32="${pkg_bld}/lib32/libstreams.la"
    streams_cv_lfs_ldflags32="-L${pkg_bld}/lib32/.libs/"
    streams_cv_lfs_manpath="${pkg_bld}/doc/man"
    streams_cv_lfs_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    streams_cv_lfs_modmap="${pkg_bld}/Modules.map"
    streams_cv_lfs_symver="${pkg_bld}/Module.symvers"
    streams_cv_lfs_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
    streams_cv_config="$streams_cv_lfs_config"
    streams_cv_includes="$streams_cv_lfs_includes"
dnl streams_cv_ldadd="$streams_cv_lfs_ldadd"
dnl streams_cv_ldadd32="$streams_cv_lfs_ldadd32"
dnl streams_cv_ldflags="$streams_cv_lfs_ldflags"
dnl streams_cv_ldflags32="$streams_cv_lfs_ldflags32"
dnl streams_cv_manpath="$streams_cv_lfs_manpath"
    streams_cv_modversions="$streams_cv_lfs_modversions"
dnl streams_cv_modmap="$streams_cv_lfs_modmap"
dnl streams_cv_symver="$streams_cv_lfs_symver"
    streams_cv_version="${VERSION}"
    streams_cv_package="LfS"
])# _LFS_CONFIG
# =============================================================================

# =============================================================================
# _LFS_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    if test ${linux_cv_minorbits:-8} -gt 8 ; then
	strconf_cv_majbase=2001
    else
	strconf_cv_majbase=231
    fi
    strconf_cv_midbase=5001
    strconf_cv_config='include/sys/config.h'
    strconf_cv_modconf='modconf.h'
    strconf_cv_drvconf='drvconf.mk'
    strconf_cv_confmod='conf.modules'
    strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="src/util/${PACKAGE_TARNAME}_mknod.c"
    strconf_cv_strsetup='strsetup.conf'
    strconf_cv_strload='strload.conf'
    strconf_cv_package='LfS'
    strconf_cv_minorbits="${linux_cv_minorbits:-8}"
    _STRCONF
])# _LFS_STRCONF
# =============================================================================

# =============================================================================
# _LFS_
# -----------------------------------------------------------------------------
AC_DEFUN([_LFS_], [dnl
])# _LFS_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
