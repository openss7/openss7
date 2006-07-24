# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/05/08 03:12:42 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2006/05/08 03:12:42 $ by $Author: brian $
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
m4_include([m4/streams.m4])
m4_include([m4/strcomp.m4])

# =============================================================================
# AC_UTIL
# -----------------------------------------------------------------------------
AC_DEFUN([AC_UTIL], [dnl
    _OPENSS7_PACKAGE([SUTIL], [OpenSS7 STREAMS Utilities])
    _UTIL_OPTIONS
    _AUTOPR
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
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/src/include -I$(top_srcdir)/src/include'
dnl if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
dnl fi
    PKG_MODFLAGS='$(STREAMS_MODFLAGS)'
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
# _UTIL_SETUP_DEBUG
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP_DEBUG], [dnl
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
])# _UTIL_SETUP_DEBUG
# =============================================================================

# =============================================================================
# _UTIL_SETUP_MODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP_MODULES], [dnl
    AC_ARG_ENABLE([module-pipemod],
	AS_HELP_STRING([--enable-module-pipemod],
	    [enable pipemod module for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_module_pipemod="$enableval"],
	    [enable_module_pipemod='module'])
    AC_ARG_ENABLE([module-connld],
	AS_HELP_STRING([--enable-module-connld],
	    [enable connld module for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_module_connld="$enableval"],
	    [enable_module_connld='module'])
    AC_ARG_ENABLE([module-sc],
	AS_HELP_STRING([--enable-module-sc],
	    [enable sc module for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_module_sc="$enableval"],
	    [enable_module_sc='module'])
    AC_CACHE_CHECK([for util module pipemod], [util_module_pipemod], [dnl
	util_module_pipemod="${enable_module_pipemod:-module}"
	if test :$util_module_pipemod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_module_pipemod='yes'
	fi])
    AC_CACHE_CHECK([for util module connld], [util_module_connld], [dnl
	util_module_connld="${enable_module_connld:-module}"
	if test :$util_module_connld = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_module_connld='yes'
	fi])
    AC_CACHE_CHECK([for util module sc], [util_module_sc], [dnl
	util_module_sc="${enable_module_sc:-module}"
	if test :$util_module_sc = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_module_sc='yes'
	fi])
dnl --------------------------------------
    case ${util_module_pipemod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the pipemod module for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the pipemod module for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the pipemod module as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the pipemod module as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_module_connld:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the connld module for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the connld module for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the connld module as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the connld module as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_module_sc:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the sc module for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sc module for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the sc module as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sc module as a loadable kernel
	    module.])
	    ;;
    esac
dnl ===========================
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD],		[test :${util_module_pipemod:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD_MODULE],	[test :${util_module_pipemod:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD],		[test :${util_module_connld:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD_MODULE],	[test :${util_module_connld:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SC],			[test :${util_module_sc:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SC_MODULE],		[test :${util_module_sc:-module}	= :module])
])# _UTIL_SETUP_MODULES
# =============================================================================

# =============================================================================
# _UTIL_SETUP_DRIVERS
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP_DRIVERS], [dnl
    AC_ARG_ENABLE([driver-clone],
	AS_HELP_STRING([--enable-driver-clone],
	    [enable clone driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_clone="$enableval"],
	    [enable_driver_clone='module'])
    AC_ARG_ENABLE([driver-echo],
	AS_HELP_STRING([--enable-driver-echo],
	    [enable echo driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_echo="$enableval"],
	    [enable_driver_echo='module'])
    AC_ARG_ENABLE([driver-fifo],
	AS_HELP_STRING([--enable-driver-fifo],
	    [enable fifo driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_fifo="$enableval"],
	    [enable_driver_fifo='module'])
    AC_ARG_ENABLE([driver-log],
	AS_HELP_STRING([--enable-driver-log],
	    [enable log driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_log="$enableval"],
	    [enable_driver_log='module'])
    AC_ARG_ENABLE([driver-loop],
	AS_HELP_STRING([--enable-driver-loop],
	    [enable loop driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_loop="$enableval"],
	    [enable_driver_loop='module'])
    AC_ARG_ENABLE([driver-nsdev],
	AS_HELP_STRING([--enable-driver-nsdev],
	    [enable nsdev driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_nsdev="$enableval"],
	    [enable_driver_nsdev='module'])
    AC_ARG_ENABLE([driver-nuls],
	AS_HELP_STRING([--enable-driver-nuls],
	    [enable nuls driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_nuls="$enableval"],
	    [enable_driver_nuls='module'])
    AC_ARG_ENABLE([driver-pipe],
	AS_HELP_STRING([--enable-driver-pipe],
	    [enable pipe driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_pipe="$enableval"],
	    [enable_driver_pipe='module'])
    AC_ARG_ENABLE([driver-sad],
	AS_HELP_STRING([--enable-driver-sad],
	    [enable sad driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_sad="$enableval"],
	    [enable_driver_sad='module'])
    AC_ARG_ENABLE([driver-sfx],
	AS_HELP_STRING([--enable-driver-sfx],
	    [enable sfx driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_sfx="$enableval"],
	    [enable_driver_sfx='module'])
    AC_ARG_ENABLE([driver-spx],
	AS_HELP_STRING([--enable-driver-spx],
	    [enable spx driver for linkage with the kernel.
	    @<:@default=module@:>@]),
	    [enable_driver_spx="$enableval"],
	    [enable_driver_spx='module'])
    AC_CACHE_CHECK([for util driver clone], [util_driver_clone], [dnl
	util_driver_clone="${enable_driver_clone:-module}"
	if test :$util_driver_clone = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_clone='yes'
	fi])
    AC_CACHE_CHECK([for util driver echo], [util_driver_echo], [dnl
	util_driver_echo="${enable_driver_echo:-module}"
	if test :$util_driver_echo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_echo='yes'
	fi])
    AC_CACHE_CHECK([for util driver fifo], [util_driver_fifo], [dnl
	util_driver_fifo="${enable_driver_fifo:-module}"
	if test :$util_driver_fifo = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_fifo='yes'
	fi])
    AC_CACHE_CHECK([for util driver log], [util_driver_log], [dnl
	util_driver_log="${enable_driver_log:-module}"
	if test :$util_driver_log = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_log='yes'
	fi])
    AC_CACHE_CHECK([for util driver loop],  [util_driver_loop], [dnl
	util_driver_loop="${enable_driver_loop:-module}"
	if test :$util_driver_loop = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_loop='yes'
	fi])
    AC_CACHE_CHECK([for util driver nsdev], [util_driver_nsdev], [dnl
	util_driver_nsdev="${enable_driver_nsdev:-module}"
	if test :$util_driver_nsdev = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_nsdev='yes'
	fi])
    AC_CACHE_CHECK([for util driver nuls], [util_driver_nuls], [dnl
	util_driver_nuls="${enable_driver_nuls:-module}"
	if test :$util_driver_nuls = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_nuls='yes'
	fi])
    AC_CACHE_CHECK([for util driver pipe], [util_driver_pipe], [dnl
	util_driver_pipe="${enable_driver_pipe:-module}"
	if test :$util_driver_pipe = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_pipe='yes'
	fi])
    AC_CACHE_CHECK([for util driver sad], [util_driver_sad], [dnl
	util_driver_sad="${enable_driver_sad:-module}"
	if test :$util_driver_sad = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_sad='yes'
	fi])
    AC_CACHE_CHECK([for util driver sfx], [util_driver_sfx], [dnl
	util_driver_sfx="${enable_driver_sfx:-module}"
	if test :$util_driver_sfx = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_sfx='yes'
	fi])
    AC_CACHE_CHECK([for util driver spx], [util_driver_spx], [dnl
	util_driver_spx="${enable_driver_spx:-module}"
	if test :$util_driver_spx = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    util_driver_spx='yes'
	fi])
dnl ------------------------------------
    case ${util_driver_clone:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the clone driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the clone driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the clone driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the clone driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_echo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the echo driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the echo driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the echo driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the echo driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_fifo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the fifo driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the fifo driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the fifo driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the fifo driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_log:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the log driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the log driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the log driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the log driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_loop:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the loop driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the loop driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the loop driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the loop driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_nsdev:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the nsdev driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the nsdev driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the nsdev driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the nsdev driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_nuls:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the nuls driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the nuls driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the nuls driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the nuls driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_pipe:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the pipe driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the pipe driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the pipe driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the pipe driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_sad:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the sad driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sad driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the sad driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sad driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_sfx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SFX], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the sfx driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sfx driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SFX_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the sfx driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sfx driver as a loadable kernel
	    module.])
	    ;;
    esac
    case ${util_driver_spx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SPX], [], [When defined,] AC_PACKAGE_TITLE [
	    will include the spx driver for linkage with the kernel.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the spx driver for linkage with the kernel.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SPX_MODULE], [], [When defined,]
	    AC_PACKAGE_TITLE [will include the spx driver as a loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the spx driver as a loadable kernel
	    module.])
	    ;;
    esac
dnl =================================
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE],		[test :${util_driver_clone:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE_MODULE],	[test :${util_driver_clone:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO],		[test :${util_driver_echo:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO_MODULE],	[test :${util_driver_echo:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO],		[test :${util_driver_fifo:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO_MODULE],	[test :${util_driver_fifo:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG],		[test :${util_driver_log:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG_MODULE],		[test :${util_driver_log:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP],		[test :${util_driver_loop:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP_MODULE],	[test :${util_driver_loop:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV],		[test :${util_driver_nsdev:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV_MODULE],	[test :${util_driver_nsdev:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS],		[test :${util_driver_nuls:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS_MODULE],	[test :${util_driver_nuls:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE],		[test :${util_driver_pipe:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE_MODULE],	[test :${util_driver_pipe:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD],		[test :${util_driver_sad:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD_MODULE],		[test :${util_driver_sad:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SFX],		[test :${util_driver_sfx:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SFX_MODULE],		[test :${util_driver_sfx:-module}	= :module])
    AM_CONDITIONAL([CONFIG_STREAMS_SPX],		[test :${util_driver_spx:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SPX_MODULE],		[test :${util_driver_spx:-module}	= :module])
])# _UTIL_SETUP_DRIVERS
# =============================================================================

# =============================================================================
# _UTIL_SETUP_FIFOS
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP_FIFOS], [dnl
    AC_ARG_ENABLE([streams-fifos],
	AS_HELP_STRING([--enable-streams-fifos],
	    [enable override of system fifos with STREAMS-based fifos.
	    @<:@default=no@:>@]),
	[enable_streams_fifos="$enableval"],
	[enable_streams_fifos='no'])
    if test :"$enable_streams_fifos" = :yes ; then
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_OVERRIDE_FIFOS], [], [When defined,]
		AC_PACKAGE_TITLE [will override the Linux system defined
		FIFOs at startup.  This should be used with care for a while,
		until streams FIFOs are proven.])
    fi
])# _UTIL_SETUP_FIFOS
# =============================================================================

# =============================================================================
# _UTIL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _UTIL_CHECK_KERNEL
    _UTIL_SETUP_DEBUG
    _LINUX_STREAMS
    _UTIL_SETUP_MODULES
    _UTIL_SETUP_DRIVERS
    _UTIL_SETUP_FIFOS
    _STRCOMP
])# _UTIL_SETUP
# =============================================================================

# =============================================================================
# _UTIL_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_SETUP_MODULE], [dnl
])# _UTIL_SETUP_MODULE
# =============================================================================

# =============================================================================
# _UTIL_CHECK_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_CHECK_KERNEL], [dnl
    _UTIL_CONFIG_KERNEL
])# _UTIL_CHECK_KERNEL
# =============================================================================

# =============================================================================
# _UTIL_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuraiton checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_UTIL_CONFIG_KERNEL], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/slab.h linux/cdev.h \
			  linux/hardirq.h linux/cpumask.h linux/kref.h linux/security.h \
			  asm/uaccess.h], [:], [:], [
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
			cpumask_scnprintf __symbol_get __symbol_put \
			read_trylock write_trylock atomic_add_return path_lookup \
			MOD_DEC_USE_COUNT MOD_INC_USE_COUNT cli sti \
			num_online_cpus generic_delete_inode], [:], [
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
#if HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#include <linux/ioport.h>	/* for check_region */
#include <linux/pci.h>		/* for pci checks */
#if HAVE_KINC_ASM_UACCESS_H
#include <asm/uaccess.h>
#endif
])
    _LINUX_CHECK_MACROS([MOD_DEC_USE_COUNT MOD_INC_USE_COUNT \
			 read_trylock write_trylock num_online_cpus \
			 cpumask_scnprintf access_ok], [:], [:], [
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
#if HAVE_KINC_ASM_UACCESS_H
#include <asm/uaccess.h>
#endif
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
			  struct kobject.kref], [:], [:], [
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
	_LINUX_KERNEL_SYMBOL_EXPORT([cdev_put])
	_LINUX_KERNEL_EXPORT_ONLY([path_lookup])
	_LINUX_KERNEL_EXPORT_ONLY([raise_softirq])
	_LINUX_KERNEL_EXPORT_ONLY([raise_softirq_irqoff])
	_LINUX_KERNEL_SYMBOL_EXPORT([put_filp])
	_LINUX_KERNEL_ENV([dnl
	    AC_CACHE_CHECK([for kernel inode_operation lookup with nameidata],
			   [linux_cv_have_iop_lookup_nameidata], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
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
#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#if HAVE_KINC_LINUX_NAMEI_H
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
	    AC_CACHE_CHECK([for kernel do_settimeofday with timespec],
			   [linux_cv_have_timespec_settimeofday], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
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
#include <linux/interrupt.h>	/* for irqreturn_t */ 
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
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
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=242
dnl
dnl Tired of device conflicts on 2.6 kernels.
dnl
    if test ${linux_cv_minorbits:-8} -gt 8 ; then
	((strconf_cv_majbase+=2000))
    fi
    strconf_cv_midbase=30
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
    _STRCONF
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
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
