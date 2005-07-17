# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/07/16 22:03:17 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2005/07/16 22:03:17 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
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
m4_include([m4/streams.m4])
m4_include([m4/strcomp.m4])

# =============================================================================
# AC_UTIL
# -----------------------------------------------------------------------------
AC_DEFUN([AC_UTIL], [dnl
    _OPENSS7_PACKAGE([SUTIL], [OpenSS7 STREAMS Utilities])
    _UTIL_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strutil-core.postinst
		     debian/strutil-core.postrm
		     debian/strutil-core.preinst
		     debian/strutil-core.prerm
		     debian/strutil-devel.preinst
		     debian/strutil-dev.postinst
		     debian/strutil-dev.preinst
		     debian/strutil-dev.prerm
		     debian/strutil-doc.postinst
		     debian/strutil-doc.preinst
		     debian/strutil-doc.prerm
		     debian/strutil-init.postinst
		     debian/strutil-init.postrm
		     debian/strutil-init.preinst
		     debian/strutil-init.prerm
		     debian/strutil-lib.preinst
		     debian/strutil-source.preinst
		     debian/strutil-util.preinst
		     src/util/modutils/strutil
		     src/include/sys/strutil/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _UTIL_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/$(STRCONF_CONFIG)'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/src/include -I$(top_srcdir)/src/include'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
    fi
    PKG_MODFLAGS='$(STREAMS_MODFLAGS)'
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    LDFLAGS   = $USER_LDFLAGS])
    AC_MSG_NOTICE([final package INCLUDES  = $PKG_INCLUDES])
    AC_MSG_NOTICE([final package MODFLAGS  = $PKG_MODFLAGS])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final kernel  LDFLAGS   = $KERNEL_LDFLAGS])
    AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_MSG_NOTICE([final streams MODFLAGS  = $STREAMS_MODFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([PKG_INCLUDES])dnl
    AC_SUBST([PKG_MODFLAGS])dnl
    PKG_MANPATH='$(mandir)'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    PKG_MANPATH="${STREAMS_MANPATH:+${STREAMS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${STRCOMP_MANPATH:+${STRCOMP_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _UTIL_OUTPUT
    _AUTOTEST
])# AC_UTIL
# =============================================================================

# =============================================================================
# _UTIL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_OPTIONS], [dnl
])# _UTIL_OPTIONS
# =============================================================================

# =============================================================================
# _UTIL_SETUP_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP_OPTIONS], [dnl
    AC_ARG_ENABLE([util-clone],
	AS_HELP_STRING([--enable-util-clone],
	    [enable clone driver.
	    @<:@default=module@:>@]),
	    [enable_util_clone="$enableval"],
	    [enable_util_clone='module'])
    AC_ARG_ENABLE([util-echo],
	AS_HELP_STRING([--enable-util-echo],
	    [enable echo driver.
	    @<:@default=module@:>@]),
	    [enable_util_echo="$enableval"],
	    [enable_util_echo='module'])
    AC_ARG_ENABLE([util-fifo],
	AS_HELP_STRING([--enable-util-fifo],
	    [enable fifo driver.
	    @<:@default=module@:>@]),
	    [enable_util_fifo="$enableval"],
	    [enable_util_fifo='module'])
    AC_ARG_ENABLE([util-log],
	AS_HELP_STRING([--enable-util-log],
	    [enable log driver.
	    @<:@default=module@:>@]),
	    [enable_util_log="$enableval"],
	    [enable_util_log='module'])
    AC_ARG_ENABLE([util-nsdev],
	AS_HELP_STRING([--enable-util-nsdev],
	    [enable nsdev driver.
	    @<:@default=module@:>@]),
	    [enable_util_nsdev="$enableval"],
	    [enable_util_nsdev='module'])
    AC_ARG_ENABLE([util-nuls],
	AS_HELP_STRING([--enable-util-nuls],
	    [enable nuls driver.
	    @<:@default=module@:>@]),
	    [enable_util_nuls="$enableval"],
	    [enable_util_nuls='module'])
    AC_ARG_ENABLE([util-pipe],
	AS_HELP_STRING([--enable-util-pipe],
	    [enable pipe driver.
	    @<:@default=module@:>@]),
	    [enable_util_pipe="$enableval"],
	    [enable_util_pipe='module'])
    AC_ARG_ENABLE([util-sad],
	AS_HELP_STRING([--enable-util-sad],
	    [enable sad driver.
	    @<:@default=module@:>@]),
	    [enable_util_sad="$enableval"],
	    [enable_util_sad='module'])
    AC_ARG_ENABLE([util-sfx],
	AS_HELP_STRING([--enable-util-sfx],
	    [enable sfx driver.
	    @<:@default=module@:>@]),
	    [enable_util_sfx="$enableval"],
	    [enable_util_sfx='module'])
    AC_ARG_ENABLE([util-spx],
	AS_HELP_STRING([--enable-util-spx],
	    [enable spx driver.
	    @<:@default=module@:>@]),
	    [enable_util_spx="$enableval"],
	    [enable_util_spx='module'])
    AC_ARG_ENABLE([util-pipemod],
	AS_HELP_STRING([--enable-util-pipemod],
	    [enable pipemod module.
	    @<:@default=module@:>@]),
	    [enable_util_pipemod="$enableval"],
	    [enable_util_pipemod='module'])
    AC_ARG_ENABLE([util-sc],
	AS_HELP_STRING([--enable-util-sc],
	    [enable sc module.
	    @<:@default=module@:>@]),
	    [enable_util_sc="$enableval"],
	    [enable_util_sc='module'])
    AC_CACHE_CHECK([for util clone driver], [util_clone], [dnl
	util_clone="${enable_util_clone:-module}"
	if test :$util_clone = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_clone='yes'
	fi])
    AC_CACHE_CHECK([for util echo driver], [util_echo], [dnl
	util_echo="${enable_util_echo:-module}"
	if test :$util_echo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_echo='yes'
	fi])
    AC_CACHE_CHECK([for util fifo driver], [util_fifo], [dnl
	util_fifo="${enable_util_fifo:-module}"
	if test :$util_fifo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_fifo='yes'
	fi])
    AC_CACHE_CHECK([for util log driver], [util_log], [dnl
	util_log="${enable_util_log:-module}"
	if test :$util_log = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_log='yes'
	fi])
    AC_CACHE_CHECK([for util nsdev driver], [util_nsdev], [dnl
	util_nsdev="${enable_util_nsdev:-module}"
	if test :$util_nsdev = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_nsdev='yes'
	fi])
    AC_CACHE_CHECK([for util nuls driver], [util_nuls], [dnl
	util_nuls="${enable_util_nuls:-module}"
	if test :$util_nuls = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_nuls='yes'
	fi])
    AC_CACHE_CHECK([for util pipe driver], [util_pipe], [dnl
	util_pipe="${enable_util_pipe:-module}"
	if test :$util_pipe = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_pipe='yes'
	fi])
    AC_CACHE_CHECK([for util sad driver], [util_sad], [dnl
	util_sad="${enable_util_sad:-module}"
	if test :$util_sad = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_sad='yes'
	fi])
    AC_CACHE_CHECK([for util sfx driver], [util_sfx], [dnl
	util_sfx="${enable_util_sfx:-module}"
	if test :$util_sfx = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_sfx='yes'
	fi])
    AC_CACHE_CHECK([for util spx driver], [util_spx], [dnl
	util_spx="${enable_util_spx:-module}"
	if test :$util_spx = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_spx='yes'
	fi])
    AC_CACHE_CHECK([for util pipemod module], [util_pipemod], [dnl
	util_pipemod="${enable_util_pipemod:-module}"
	if test :$util_pipemod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_pipemod='yes'
	fi])
    AC_CACHE_CHECK([for util sc module], [util_sc], [dnl
	util_sc="${enable_util_sc:-module}"
	if test :$util_sc = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_sc='yes'
	fi])
    case ${util_clone:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_CLONE], [], [When defined,] AC_PACKAGE_NAME [
	    will include the clone driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the clone driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_CLONE_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the clone driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the clone driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_echo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_ECHO], [], [When defined,] AC_PACKAGE_NAME [
	    will include the echo driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the echo driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_ECHO_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the echo driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the echo driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_fifo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_FIFO], [], [When defined,] AC_PACKAGE_NAME [
	    will include the fifo driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the fifo driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_FIFO_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the fifo driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the fifo driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_log:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_LOG], [], [When defined,] AC_PACKAGE_NAME [
	    will include the log driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the log driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_LOG_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the log driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the log driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_nsdev:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_NSDEV], [], [When defined,] AC_PACKAGE_NAME [
	    will include the nsdev driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the nsdev driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_NSDEV_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the nsdev driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the nsdev driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_nuls:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_NULS], [], [When defined,] AC_PACKAGE_NAME [
	    will include the nuls driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the nuls driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_NULS_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the nuls driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the nuls driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_pipe:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_PIPE], [], [When defined,] AC_PACKAGE_NAME [
	    will include the pipe driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the pipe driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_PIPE_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the pipe driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the pipe driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_sad:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SAD], [], [When defined,] AC_PACKAGE_NAME [
	    will include the sad driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the sad driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SAD_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the sad driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the sad driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_sfx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SFX], [], [When defined,] AC_PACKAGE_NAME [
	    will include the sfx driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the sfx driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SFX_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the sfx driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the sfx driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_spx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SPX], [], [When defined,] AC_PACKAGE_NAME [
	    will include the spx driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the spx driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SPX_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the spx driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the spx driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_pipemod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_PIPEMOD], [], [When defined,] AC_PACKAGE_NAME [
	    will include the pipemod module for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the pipemod module for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_PIPEMOD_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the pipemod module as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the pipemod module as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_sc:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SC], [], [When defined,] AC_PACKAGE_NAME [
	    will include the sc module for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_NAME [will not include the sc module for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTIL_SC_MODULE], [], [When defined,]
	    AC_PACKAGE_NAME [will include the sc module as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_NAME [will not include the sc module as a loadable kernel
	    module.])
	    ;;
    esac
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_CLONE],		[test :${util_clone:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_CLONE_MODULE],	[test :${util_clone:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_ECHO],		[test :${util_echo:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_ECHO_MODULE],	[test :${util_echo:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_FIFO],		[test :${util_fifo:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_FIFO_MODULE],	[test :${util_fifo:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_LOG],		[test :${util_log:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_LOG_MODULE],	[test :${util_log:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_NSDEV],		[test :${util_nsdev:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_NSDEV_MODULE],	[test :${util_nsdev:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_NULS],		[test :${util_nuls:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_NULS_MODULE],	[test :${util_nuls:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_PIPE],		[test :${util_pipe:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_PIPE_MODULE],	[test :${util_pipe:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SAD],		[test :${util_sad:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SAD_MODULE],	[test :${util_sad:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SFX],		[test :${util_sfx:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SFX_MODULE],	[test :${util_sfx:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SPX],		[test :${util_spx:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SPX_MODULE],	[test :${util_spx:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_PIPEMOD],	[test :${util_pipemod:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_PIPEMOD_MODULE],[test :${util_pipemod:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SC],		[test :${util_sc:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_UTIL_SC_MODULE],	[test :${util_sc:-module}	= :module])
])# _UTIL_SETUP_OPTIONS
# =============================================================================

# =============================================================================
# _UTIL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _UTIL_CONFIG_KERNEL
    _GENKSYMS
    _LINUX_STREAMS
    _STRCOMP
    _UTIL_SETUP_OPTIONS
])# _UTIL_SETUP
# =============================================================================

# =============================================================================
# _UTIL_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuraiton checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_CONFIG_KERNEL], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/slab.h linux/hardirq.h linux/security.h], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
])
    AC_SUBST([EXPOSED_SYMBOLS])dnl
    _LINUX_CHECK_FUNCS([try_module_get module_put to_kdev_t force_delete kern_umount iget_locked \
			process_group cpu_raise_softirq check_region pcibios_init \
			pcibios_find_class pcibios_find_device pcibios_present \
			pcibios_read_config_byte pcibios_read_config_dword \
			pcibios_read_config_word pcibios_write_config_byte \
			pcibios_write_config_dword pcibios_write_config_word \
			pci_dac_dma_sync_single pci_dac_dma_sync_single_for_cpu \
			pci_dac_dma_sync_single_for_device pci_dac_set_dma_mask \
			pci_find_class pci_dma_sync_single pci_dma_sync_sg \
			sleep_on interruptible_sleep_on sleep_on_timeout \
			read_trylock write_trylock \
			MOD_DEC_USE_COUNT MOD_INC_USE_COUNT cli sti path_lookup], [:], [
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
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#if HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#if HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
])
    _LINUX_CHECK_MACROS([MOD_DEC_USE_COUNT MOD_INC_USE_COUNT \
			 read_trylock write_trylock], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#if HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#if HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
])
    _LINUX_CHECK_TYPES([irqreturn_t], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#if HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
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
			 struct task_struct.session,
			 struct task_struct.pgrp,
			 struct super_block.s_fs_info,
			 struct super_block.u.generic_sbp,
			 struct file_system_type.read_super,
			 struct file_system_type.get_sb,
			 struct super_operations.read_inode2,
			 struct kstatfs.f_type], [:], [:], [
#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#if HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#if HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
])
])# _UTIL_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _UTIL_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_OUTPUT], [dnl
    _UTIL_STRCONF
])# _UTIL_OUTPUT
# =============================================================================

# =============================================================================
# _UTIL_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=245
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
    strconf_cv_drvconf='drvconf.mk'
    strconf_cv_confmod='conf.modules'
    strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="${PACKAGE_TARNAME}_mknod.c"
    strconf_cv_strsetup='strsetup.conf'
    strconf_cv_strload='strload.conf'
    AC_REQUIRE([_LINUX_STREAMS])
    strconf_cv_package=${streams_cv_package:-LiS}
    strconf_cv_minorbits="${linux_cv_minorbits:-8}"
    _STRCONF dnl
])# _UTIL_STRCONF
# =============================================================================

# =============================================================================
# _UTIL_
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_], [dnl
])# _UTIL_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
