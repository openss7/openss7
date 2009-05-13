# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.60 $) $Date: 2008-12-06 12:58:12 $
#
# -----------------------------------------------------------------------------
#
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
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2008-12-06 12:58:12 $ by $Author: brian $
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
m4_include([m4/xopen.m4])
m4_include([m4/doxy.m4])
m4_include([m4/lib32.m4])
m4_include([m4/perl.m4])
m4_include([m4/snmp.m4])
m4_include([m4/agent.m4])


# =============================================================================
# AC_OPENSS7
# -----------------------------------------------------------------------------
AC_DEFUN([AC_OPENSS7], [dnl
    _OPENSS7_PACKAGE([OpenSS7], [OpenSS7])
    PKGINCL="src/include/sys/${PACKAGE_LCNAME}"
    _OS7_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/openss7-core.postinst
		     debian/openss7-core.postrm
		     debian/openss7-core.preinst
		     debian/openss7-core.prerm
		     debian/openss7-devel.preinst
		     debian/openss7-dev.postinst
		     debian/openss7-dev.preinst
		     debian/openss7-dev.prerm
		     debian/openss7-doc.postinst
		     debian/openss7-doc.preinst
		     debian/openss7-doc.prerm
		     debian/openss7-init.postinst
		     debian/openss7-init.postrm
		     debian/openss7-init.preinst
		     debian/openss7-init.prerm
		     debian/openss7-lib.preinst
		     debian/openss7-source.preinst
		     debian/openss7-util.preinst
		     src/util/modutils/openss7
		     src/include/sys/openss7/version.h
		     Module.mkvars])
    AC_CONFIG_HEADERS([src/drivers/ip_hooks.h
		       src/drivers/sctp_hooks.h
		       src/drivers/udp_hooks.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CPPFLAGS="${USER_CPPFLAGS}${USER_CPPFLAGS:+ }-DNAME=\\\"\`echo [\$][@] | sed -e 's,^[[^-]]*-,,;s,\.o,,'\`\\\""
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _OS7_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-DLFS=1'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    if test :${linux_cv_k_ko_modules:-no} = :no ; then
	PKG_MODFLAGS='$(STREAMS_MODFLAGS) $(STRCOMP_MODFLAGS)'
	if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	    PKG_MODFLAGS="${PKG_MODFLAGS}${PKG_MODFLAGS:+ }"'-include ${top_builddir}/${MODVERSIONS_H}'
dnl	    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/include'
	fi
    fi
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
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
    _OS7_OUTPUT
    _AGENT
    _AUTOTEST
    _DOXY
])# AC_OPENSS7
# =============================================================================

# =============================================================================
# _OS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_OPTIONS], [dnl
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([streams-irq],
	AS_HELP_STRING([--disable-streams-irq],
	    [disable STREAMS irq suppression.
	     @<:@default=enabled@:>@]),
	    [enable_streams_irq="$enableval"],
	    [enable_streams_irq='yes'])
    AC_MSG_CHECKING([for STREAMS irq suppression])
    case ${enable_streams_irq:-yes} in
	(no)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NOIRQ], [1], [When defined]
		AC_PACKAGE_TITLE [will not suppress interrupts for stream or
		queue lock protection.  When defined a driver's put() procedure
		must not be called from an ISR and must only be called from
		bottom half or tasklets.  Bottom half locking is more expensive:
		don't enable this except for interrupt based profiling.])
	    ;;
    esac
    AC_MSG_RESULT([${enable_streams_irq:-yes}])
    AM_CONDITIONAL([CONFIG_STREAMS_NOIRQ], [test :${enable_streams_irq:-yes} = :no])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([streams-stats],
	AS_HELP_STRING([--disable-streams-stats],
	    [disable STREAMS stats counting.
	     @<:@default=enabled@:>@]),
	    [enable_streams_stats="$enableval"],
	    [enable_streams_stats='yes'])
    AC_MSG_CHECKING([for STREAMS stats counting])
    case ${enable_streams_stats:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DO_STATS], [1], [When defined]
		AC_PACKAGE_TITLE [will perform STREAMS entry point counting in
		the module_stat structure if a pointer is provided by the module
		or driver.  This is a low cost item and is enabled by default.])
	    ;;
    esac
    AC_MSG_RESULT([${enable_streams_stats:-yes}])
    AM_CONDITIONAL([CONFIG_STREAMS_DO_STATS], [test :${enable_streams_stats:-yes} = :yes])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([streams-syncqs],
	AS_HELP_STRING([--disable-streams-syncqs],
	    [disable STREAMS synchronization queues.
	    @<:@default=enabled@:>@]),
	    [enable_streams_syncqs="$enableval"],
	    [enable_streams_syncqs='yes'])
    AC_MSG_CHECKING([for STREAMS synchronization])
    case ${enable_streams_syncqs:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SYNCQS], [1], [When defined]
		AC_PACKAGE_TITLE [will include support for synchronization
		queues and levels.])
	    ;;
    esac
    AC_MSG_RESULT([${enable_streams_syncqs:-yes}])
    AM_CONDITIONAL([CONFIG_STREAMS_SYNCQS], [test :${enable_streams_syncqs:-yes} = :yes])
dnl--------------------------------------------------------------------------
    AC_ARG_WITH([streams-kthreads],
	AS_HELP_STRING([--with-streams-kthreads],
	    [set STREAMS kernel thread operation to 'nice', 'normal', 'rt' or
	     'no'. @<:@default=nice@:>@]),
	    [with_streams_kthreads="$withval"],
	    [with_streams_kthreads='nice'])
    AC_MSG_CHECKING([for STREAMS kernel threads])
    case ${with_streams_kthreads:-nice} in
	(no|rt|normal) ;;
	(*) with_streams_kthreads=nice ;;
    esac
    AH_TEMPLATE([CONFIG_STREAMS_KTHREADS], [When defined] AC_PACKAGE_TITLE
	[will include use kernel threads for the STREAMS scheduler; when
	undefined,] AC_PACKAGE_TITLE [will use softirqs for the STREAMS
	scheduler.])
    AH_TEMPLATE([CONFIG_STREAMS_RT_KTHREADS], [When defined] AC_PACKAGE_TITLE
	[will run kernel threads under real-time priority (SCHED_FIFO); when
	undefined,] AC_PACKAGE_TITLE [will run kernel threads (SCHED_NORMAL).])
    AH_TEMPLATE([CONFIG_STREAMS_NN_KTHREADS], [When defined] AC_PACKAGE_TITLE
	[will run kernel threads under normal scheduling priority when
	CONFIG_STREAMS_RT_KTHREADS is not defined instead of at nice level 19.])
    case ${with_streams_kthreads:-nice} in
	(no) ;;
	(rt)	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_KTHREADS], [1])
		    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_RT_KTHREADS], [1]) ;;
	(normal)    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_KTHREADS], [1])
		    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NN_KTHREADS], [1]) ;;
	(nice)	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_KTHREADS], [1]) ;;
    esac
    AC_MSG_RESULT([${with_streams_kthreads:-nice}])
    AM_CONDITIONAL([CONFIG_STREAMS_KTHREADS], [test :${with_streams_kthreads:-nice} != :no])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([streams-utils],
	AS_HELP_STRING([--disable-streams-utils],
	    [disable additional STREAMS utilities.
	    @<:@default=enabled@:>@]),
	    [enable_streams_utils="$enableval"],
	    [enable_streams_utils='yes'])
    AC_MSG_CHECKING([for STREAMS utilities])
    case ${enable_streams_utils:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_UTILS], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include additional STREAMS utilities.])
	    ;;
    esac
    AC_MSG_RESULT([${enable_streams_utils:-yes}])
    AM_CONDITIONAL([CONFIG_STREAMS_UTILS], [test :${enable_streams_utils:-yes} = :yes])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([big-compile],
	AS_HELP_STRING([--disable-big-compile],
	    [disable compiling as one big computational unit,
	    @<:@default=enabled@:>@]),
	    [enable_big_compile="$enableval"],
	    [enable_big_comiple='yes'])
    AC_MSG_CHECKING([for STREAMS big compile])
    case ${enable_big_compile:-yes} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SEPARATE_COMPILE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will compile streams objects separately.])
	    ;;
    esac
    AC_MSG_RESULT([${enable_big_compile:-yes}])
    AM_CONDITIONAL([CONFIG_STREAMS_SEPARATE_COMPILE], [test :${enable_big_compile:-yes} != :yes])
dnl--------------------------------------------------------------------------
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
dnl--------------------------------------------------------------------------
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
    AC_DEFINE_UNQUOTED([streamscall], [__attribute__((__regparm__(3)))])
    AC_DEFINE_UNQUOTED([STREAMSCALL(__x)], [__x streamscall])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-sth],
	AS_HELP_STRING([--enable-module-sth],
	    [enable module sth for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_sth="$enableval"],
	    [enable_module_sth='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_STH], [test :${enable_module_sth:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_STH_MODULE], [test :${enable_module_sth:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-bufmod],
	AS_HELP_STRING([--enable-module-bufmod],
	    [enable bufmod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_bufmod="$enableval"],
	    [enable_module_bufmod='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_BUFMOD], [test :${enable_module_bufmod:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_BUFMOD_MODULE], [test :${enable_module_bufmod:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-nullmod],
	AS_HELP_STRING([--enable-module-nullmod],
	    [enable nullmod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_nullmod="$enableval"],
	    [enable_module_nullmod='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_NULLMOD], [test :${enable_module_nullmod:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NULLMOD_MODULE], [test :${enable_module_nullmod:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-pipemod],
	AS_HELP_STRING([--enable-module-pipemod],
	    [enable pipemod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_pipemod="$enableval"],
	    [enable_module_pipemod='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD], [test :${enable_module_pipemod:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPEMOD_MODULE], [test :${enable_module_pipemod:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-connld],
	AS_HELP_STRING([--enable-module-connld],
	    [enable connld module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_connld="$enableval"],
	    [enable_module_connld='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD], [test :${enable_module_connld:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CONNLD_MODULE], [test :${enable_module_connld:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-sc],
	AS_HELP_STRING([--enable-module-sc],
	    [enable sc module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_sc="$enableval"],
	    [enable_module_sc='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_SC], [test :${enable_module_sc:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SC_MODULE], [test :${enable_module_sc:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-testmod],
	AS_HELP_STRING([--enable-module-testmod],
	    [enable testmod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_testmod="$enableval"],
	    [enable_module_testmod='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_TESTMOD], [test :${enable_module_testmod:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_TESTMOD_MODULE], [test :${enable_module_testmod:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-timod],
	AS_HELP_STRING([--enable-module-timod],
	    [enable timod module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_timod="$enableval"],
	    [enable_module_timod='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_TIMOD], [test :${enable_module_timod:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_TIMOD_MODULE], [test :${enable_module_timod:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([module-tirdwr],
	AS_HELP_STRING([--enable-module-tirdwr],
	    [enable tirdwr module for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_module_tirdwr="$enableval"],
	    [enable_module_tirdwr='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_TIRDWR], [test :${enable_module_tirdwr:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_TIRDWR_MODULE], [test :${enable_module_tirdwr:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-clone],
	AS_HELP_STRING([--enable-driver-clone],
	    [enable clone driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_clone="$enableval"],
	    [enable_driver_clone='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE], [test :${enable_driver_clone:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_CLONE_MODULE], [test :${enable_driver_clone:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-echo],
	AS_HELP_STRING([--enable-driver-echo],
	    [enable echo driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_echo="$enableval"],
	    [enable_driver_echo='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO], [test :${enable_driver_echo:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_ECHO_MODULE], [test :${enable_driver_echo:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-fifo],
	AS_HELP_STRING([--enable-driver-fifo],
	    [enable fifo driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_fifo="$enableval"],
	    [enable_driver_fifo='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO], [test :${enable_driver_fifo:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_FIFO_MODULE], [test :${enable_driver_fifo:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-log],
	AS_HELP_STRING([--enable-driver-log],
	    [enable log driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_log="$enableval"],
	    [enable_driver_log='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG], [test :${enable_driver_log:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOG_MODULE], [test :${enable_driver_log:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-loop],
	AS_HELP_STRING([--enable-driver-loop],
	    [enable loop driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_loop="$enableval"],
	    [enable_driver_loop='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP], [test :${enable_driver_loop:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_LOOP_MODULE], [test :${enable_driver_loop:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-nsdev],
	AS_HELP_STRING([--enable-driver-nsdev],
	    [enable nsdev driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_nsdev="$enableval"],
	    [enable_driver_nsdev='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV], [test :${enable_driver_nsdev:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NSDEV_MODULE], [test :${enable_driver_nsdev:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-mux],
	AS_HELP_STRING([--enable-driver-mux],
	    [enable mux driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_mux="$enableval"],
	    [enable_driver_mux='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_MUX], [test :${enable_driver_mux:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_MUX_MODULE], [test :${enable_driver_mux:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-nuls],
	AS_HELP_STRING([--enable-driver-nuls],
	    [enable nuls driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_nuls="$enableval"],
	    [enable_driver_nuls='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS], [test :${enable_driver_nuls:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_NULS_MODULE], [test :${enable_driver_nuls:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-pipe],
	AS_HELP_STRING([--enable-driver-pipe],
	    [enable pipe driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_pipe="$enableval"],
	    [enable_driver_pipe='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE], [test :${enable_driver_pipe:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_PIPE_MODULE], [test :${enable_driver_pipe:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-sad],
	AS_HELP_STRING([--enable-driver-sad],
	    [enable sad driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_sad="$enableval"],
	    [enable_driver_sad='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD], [test :${enable_driver_sad:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SAD_MODULE], [test :${enable_driver_sad:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-sfx],
	AS_HELP_STRING([--enable-driver-sfx],
	    [enable sfx driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_sfx="$enableval"],
	    [enable_driver_sfx='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_SFX], [test :${enable_driver_sfx:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SFX_MODULE], [test :${enable_driver_sfx:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([driver-spx],
	AS_HELP_STRING([--enable-driver-spx],
	    [enable spx driver for linkage with STREAMS.
	    @<:@default=module@:>@]),
	    [enable_driver_spx="$enableval"],
	    [enable_driver_spx='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_SPX], [test :${enable_driver_spx:-module} = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_SPX_MODULE], [test :${enable_driver_spx:-module} = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-os7],
	AS_HELP_STRING([--enable-compat-os7],
	    [enable source compatibility with OpenSS7 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_os7="$enableval"],
	    [enable_compat_os7='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OS7], [test ":${enable_compat_os7:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OS7_MODULE], [test ":${enable_compat_os7:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-svr3],
	AS_HELP_STRING([--enable-compat-svr3],
	    [enable source compatibility with SVR 4.2 MP variants.
	    @<:@default=module@:>@]),
	    [enable_compat_svr3="$enableval"],
	    [enable_compat_svr3='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR3], [test ":${enable_compat_svr3:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR3_MODULE], [test ":${enable_compat_svr3:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-svr4],
	AS_HELP_STRING([--enable-compat-svr4],
	    [enable source compatibility with SVR 4.2 MP variants.
	    @<:@default=module@:>@]),
	    [enable_compat_svr4="$enableval"],
	    [enable_compat_svr4='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR4], [test ":${enable_compat_svr4:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SVR4_MODULE], [test ":${enable_compat_svr4:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-mps],
	AS_HELP_STRING([--enable-compat-mps],
	    [enable source compatibility with MPS variants.
	    @<:@default=moudle@:>@]),
	    [enable_compat_mps="$enableval"],
	    [enable_compat_mps='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MPS], [test ":${enable_compat_mps:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MPS_MODULE], [test ":${enable_compat_mps:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-sol8],
	AS_HELP_STRING([--enable-compat-sol8],
	    [enable source compatibility with Solaris 8 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_sol8="$enableval"],
	    [enable_compat_sol8='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SUN], [test ":${enable_compat_sol8:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_SUN_MODULE], [test ":${enable_compat_sol8:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-uw7],
	AS_HELP_STRING([--enable-compat-uw7],
	    [enable source compatibility with UnixWare 7 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_uw7="$enableval"],
	    [enable_compat_uw7='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_UW7], [test ":${enable_compat_uw7:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_UW7_MODULE], [test ":${enable_compat_uw7:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-osf],
	AS_HELP_STRING([--enable-compat-osf],
	    [enable source compatibility with OSF/1.2 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_osf="$enableval"],
	    [enable_compat_osf='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OSF], [test ":${enable_compat_osf:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_OSF_MODULE], [test ":${enable_compat_osf:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-aix],
	AS_HELP_STRING([--enable-compat-aix],
	    [enable source compatibility with AIX 4 variants.
	    @<:@default=module@:>@]),
	    [enable_compat_aix="$enableval"],
	    [enable_compat_aix='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_AIX], [test ":${enable_compat_aix:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_AIX_MODULE], [test ":${enable_compat_aix:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-hpux],
	AS_HELP_STRING([--enable-compat-hpux],
	    [enable source compatibility with HPUX variants.
	    @<:@default=module@:>@]),
	    [enable_compat_hpux="$enableval"],
	    [enable_compat_hpux='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_HPUX], [test ":${enable_compat_hpux:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [test ":${enable_compat_hpux:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-irix],
	AS_HELP_STRING([--enable-compat-irix],
	    [enable source compatibility with IRIX variants.
	    @<:@default=module@:>@]),
	    [enable_compat_irix="$enableval"],
	    [enable_compat_irix='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_IRIX], [test ":${enable_compat_irix:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_IRIX_MODULE], [test ":${enable_compat_irix:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([compat-mac],
	AS_HELP_STRING([--enable-compat-mac],
	    [enable source compatibility with MacOT variants.
	    @<:@default=module@:>@]),
	    [enable_compat_mac="$enableval"],
	    [enable_compat_mac='module'])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MAC], [test ":${enable_compat_mac:-module}" = :yes])
    AM_CONDITIONAL([CONFIG_STREAMS_COMPAT_MAC_MODULE], [test ":${enable_compat_mac:-module}" = :module])
dnl--------------------------------------------------------------------------
    AC_ARG_WITH([ip],
	AS_HELP_STRING([--with-ip],
	    [include np-ip revision 2 driver in build.  @<:@default=yes@:>@]),
	[with_ip="$withval"],
	[with_ip='no'])
    AC_MSG_CHECKING([for NP-IP driver])
    if test :"$with_ip" = :yes ; then
	os7_cv_ip_v2='yes'
	AC_DEFINE([IP_VERSION_2], [1], [Define for NP-IP driver.  This define is
	    needed by test programs and other programs that need to determine if
	    the NP-IP driver is included in the build or not.])
    else
	os7_cv_ip_v2='yes'
    fi
    AC_MSG_RESULT([$os7_cv_ip_v2])
    AM_CONDITIONAL([WITH_IP], [test :"$os7_cv_ip_v2" = :yes])dnl
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([xti-servtype],
	AS_HELP_STRING([--disable-xti-servtype],
	    [disable xnet library checks for service type.
	     @<:@default=enabled@:>@]),
	[enable_xti_servtype="$enableval"],
	[enable_xti_servtype='yes'])
    AC_MSG_CHECKING([for XTI service type checks])
    if test ${enable_xti_servtype:-yes} != yes ; then
	AC_DEFINE_UNQUOTED([CONFIG_XTI_IS_TYPELESS], [1], [Define when the XTI
	    library is not to check service types.  This is necessary when
	    T_COTS semantics are expected to be applied to T_CLTS providers.
	    When defined, the XTI library lets the underlying TPI driver
	    determine whether T_COTS/T_COTS_ORD primitives are supported or
	    not.])
    fi
    AC_MSG_RESULT([${enable_xti_servtype:-yes}])
dnl--------------------------------------------------------------------------
    AC_ARG_ENABLE([xti-states],
	AS_HELP_STRING([--disable-xti-states],
	    [disable xnet library checks for state.
	     @<:@default=enabled@:>@]),
	[enable_xti_states="$enableval"],
	[enable_xti_states='yes'])
    AC_MSG_CHECKING([for XTI state checks])
    if test ${enable_xti_states:-yes} != yes ; then
	AC_DEFINE_UNQUOTED([CONFIG_XTI_IS_STATELESS], [1], [Define when the
	    XTI library is not to check states.  This is necessary when T_COTS
	    semantics are expected to be applied to T_CLTS providers and
	    t_connect() is to be called on an unbound T_CLTS Stream.  When
	    defined, the XTI library lets the underlying TPI driver determine
	    whether the primitive is issued out of state or not.])
    fi
    AC_MSG_RESULT([${enable_xti_states:-yes}])
dnl--------------------------------------------------------------------------
    AC_ARG_WITH([udp],
	AS_HELP_STRING([--with-udp],
	    [include udp release 2 driver in build.  @<:@default=yes@:>@]),
	[with_udp="$withval"],
	[with_udp='no'])
    AC_MSG_CHECKING([for UDP Release 2 driver])
    if test :"$with_udp" = :yes ; then
	os7_cv_udp_v2='yes'
	AC_DEFINE([UDP_VERSION_2], [1], [Define for UDP Release 2.  This define
	    is needed by test programs and other programs that need to determine
	    if the UDP Release 2 driver is included in the build or not.])
    else
	os7_cv_udp_v2='yes'
    fi
    AC_MSG_RESULT([$os7_cv_udp_v2])
    AM_CONDITIONAL([WITH_UDP], [test :"$os7_cv_udp_v2" = :yes])dnl
dnl--------------------------------------------------------------------------
    AC_ARG_WITH([raw],
	AS_HELP_STRING([--with-raw],
	    [include raw release 2 driver in build.  @<:@default=yes@:>@]),
	[with_raw="$withval"],
	[with_raw='no'])
    AC_MSG_CHECKING([for RAW Release 2 driver])
    if test :"$with_raw" = :yes ; then
	os7_cv_raw_v2='yes'
	AC_DEFINE([RAW_VERSION_2], [1], [Define for RAW Release 2.  This define
	    is needed by test programs and other programs that need to determine
	    if the RAW Release 2 driver is included in the build or not.])
    else
	os7_cv_raw_v2='yes'
    fi
    AC_MSG_RESULT([$os7_cv_raw_v2])
    AM_CONDITIONAL([WITH_RAW], [test :"$os7_cv_raw_v2" = :yes])dnl
dnl--------------------------------------------------------------------------
    AC_ARG_WITH([tcp],
	AS_HELP_STRING([--with-tcp],
	    [include tcp release 2 driver in build.  @<:@default=no@:>@]),
	[with_tcp="$withval"],
	[with_tcp='no'])
    AC_MSG_CHECKING([for TCP Release 2 driver])
    if test :"$with_tcp" = :yes ; then
	os7_cv_tcp_v2='yes'
	AC_DEFINE([TCP_VERSION_2], [1], [Define for TCP Release 2.  This define
	    is needed by test programs and other programs that need to determine
	    if the TCP Release 2 driver is included in the build or not.])
    else
	os7_cv_tcp_v2='yes'
    fi
    AC_MSG_RESULT([$os7_cv_tcp_v2])
    AM_CONDITIONAL([WITH_TCP], [test :"$os7_cv_tcp_v2" = :yes])dnl
dnl--------------------------------------------------------------------------
    AC_ARG_WITH([sctp],
	AS_HELP_STRING([--with-sctp],
	    [include sctp release 1 driver in build.  @<:@default=no@:>@]),
	[with_sctp="$withval"],
	[with_sctp='no'])
    AC_ARG_WITH([sctp2],
	AS_HELP_STRING([--with-sctp2],
	    [include sctp release 2 driver in build.  @<:@default=yes@:>@]),
	[with_sctp2="$withval"],
	[with_sctp2='yes'])
    AC_MSG_CHECKING([for sctp version])
    if test :"$with_sctp2" = :yes ; then
	with_sctp='no'
	os7_cv_sctp_version=2
	AC_DEFINE([SCTP_VERSION_2], [1], [Define for SCTP Release 2.  This
	    define is needed by test programs and other programs that need to
	    determine the difference between the address format and options
	    conventions for the two versions.])
    else
	with_sctp='yes'
	os7_cv_sctp_version=1
	AC_DEFINE([SCTP_VERSION_1], [1], [Define for SCTP Release 1.  This
	    define is needed by test programs and other programs that need to
	    determine the difference between the address format and options
	    conventions for the two versions.])
    fi
    AC_DEFINE_UNQUOTED([SCTP_VERSION], [$os7_cv_sctp_version], [Define to 1 for
	SCTP Release 1.  Define to 2 for SCTP Release 2.  This define is needed
	by test programs that must determine the difference between the address
	format and options conventions for the two versions.])
    AC_MSG_RESULT([$os7_cv_sctp_version])
    AM_CONDITIONAL([WITH_SCTP], [test :"$os7_cv_sctp_version" = :1])dnl
    AM_CONDITIONAL([WITH_SCTP2], [test :"$os7_cv_sctp_version" = :2])dnl
])# _OS7_OPTIONS
# =============================================================================

# =============================================================================
# _OS7_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_SETUP], [dnl
    _OS7_SETUP_MODULE
    _OS7_CONFIG_KERNEL
    _OS7_CONFIG_SCTP
    _OS7_FINALIZE
])# _OS7_SETUP
# =============================================================================

# =============================================================================
# _OS7_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_SETUP_MODULE], [dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MODULE], [1], [When defined, OpenSS7
	    is being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS], [1], [When defined, OpenSS7 is
	    being compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([CONFIG_STREAMS_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([CONFIG_STREAMS], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _OS7_SETUP_MODULE
# =============================================================================

# =============================================================================
# _OS7_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuration checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_CONFIG_KERNEL], [dnl
dnl----------------------------------------------------------------------------
    _LINUX_CHECK_HEADERS([ \
	linux/namespace.h \
	linux/kdev_t.h \
	linux/statfs.h \
	linux/namei.h \
	linux/locks.h \
	asm/softirq.h \
	linux/brlock.h \
	linux/slab.h \
	linux/cdev.h \
	linux/hardirq.h \
	linux/cpumask.h \
	linux/kref.h \
	linux/security.h \
	asm/uaccess.h \
	linux/kthread.h \
	linux/compat.h \
	linux/ioctl32.h \
	asm/ioctl32.h \
	linux/syscalls.h \
	linux/rwsem.h \
	linux/smp_lock.h \
	linux/devfs_fs_kernel.h \
	linux/compile.h \
	linux/utsrelease.h \
	linux/fdtable.h \
	linux/snmp.h \
	net/xfrm.h \
	net/dst.h \
	net/request_sock.h \
	linux/percpu.h \
	], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/sched.h>
    ]) # _LINUX_CHECK_HEADERS
dnl----------------------------------------------------------------------------
    AC_SUBST([EXPOSED_SYMBOLS])dnl
dnl----------------------------------------------------------------------------
    _LINUX_CHECK_FUNCS([ \
	access_ok \
	atomic_add_return \
	check_region \
	cli \
	compat_ptr \
	cpumask_scnprintf \
	cpu_raise_softirq \
	create_proc_info_entry \
	dst_mtu \
	dst_output \
	finish_wait \
	force_delete \
	free_dma \
	generic_delete_inode \
	iget_locked \
	__in_dev_get_rcu \
	inet_csk \
	interruptible_sleep_on \
	ip_dst_output \
	ip_route_output_key \
	kern_umount \
	MOD_DEC_USE_COUNT \
	MOD_INC_USE_COUNT \
	module_put \
	nf_reset \
	num_online_cpus \
	path_lookup \
	pcibios_find_class \
	pcibios_find_device \
	pcibios_init \
	pcibios_present \
	pcibios_read_config_byte \
	pcibios_read_config_dword \
	pcibios_read_config_word \
	pcibios_write_config_byte \
	pcibios_write_config_dword \
	pcibios_write_config_word \
	pci_dac_dma_supported \
	pci_dac_dma_sync_single \
	pci_dac_dma_sync_single_for_cpu \
	pci_dac_dma_sync_single_for_device \
	pci_dac_dma_to_offset \
	pci_dac_dma_to_page \
	pci_dac_page_to_dma \
	pci_dac_set_dma_mask \
	pci_dma_sync_sg \
	pci_dma_sync_single \
	pci_find_class \
	pci_module_init \
	prepare_to_wait \
	prepare_to_wait_exclusive \
	process_group \
	process_session \
	rcu_read_lock \
	read_trylock \
	register_ioctl32_conversion \
	request_dma \
	set_cpus_allowed \
	set_user_nice \
	simple_statfs \
	skb_transport_header \
	sleep_on \
	sleep_on_timeout \
	sti \
	__symbol_get \
	__symbol_put \
	synchronize_net \
	task_pgrp_nr \
	task_session_nr \
	to_kdev_t \
	try_module_get \
	unregister_ioctl32_conversion \
	vfree \
	vmalloc \
	write_trylock \
	yield \
    ], [:], [
			case "$lk_func" in
			    pcibios_*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				;;
			    pci_*)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_osif_${lk_func}"
				;;
			    *sleep_on*|vmalloc|vfree)
				EXPOSED_SYMBOLS="${EXPOSED_SYMBOLS:+$EXPOSED_SYMBOLS }lis_${lk_func}"
				;;
			esac ], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
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
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
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
#include <asm/dma.h>		/* for request_dma, free_dma */
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/inetdevice.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
    ]) # _LINUX_CHECK_FUNCS
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
    _LINUX_CHECK_MACROS([ \
	access_ok \
	do_each_pid_task \
	MOD_DEC_USE_COUNT \
	MOD_INC_USE_COUNT \
	num_online_cpus \
	rcu_read_lock \
	read_trylock \
	write_trylock \
    ], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
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
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
    ]) # _LINUX_CHECK_MACROS
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
    _LINUX_CHECK_TYPES([
	paddr_t,
	irqreturn_t,
	irq_handler_t,
	bool,
	kmem_cache_t *,
	uintptr_t,
	intptr_t,
	uchar,
	pm_message_t,
	struct sockaddr_storage,
	struct inet_protocol,
	struct net_protocol], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/types.h>
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
#include <linux/pm.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/udp.h>
#include <net/tcp.h>
    ]) # _LINUX_CHECK_TYPES
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
    _LINUX_CHECK_MEMBERS([
	struct ctl_table.de,
	struct ctl_table.parent,
	struct dst_entry.path,
	struct file_operations.flush,
	struct file_operations.unlocked_ioctl,
	struct files_struct.fdtab,
	struct files_struct.max_fdset,
	struct file_system_type.get_sb,
	struct file_system_type.read_super,
	struct fown_struct.pid_type,
	struct inet_protocol.copy,
	struct inet_protocol.next,
	struct inet_protocol.no_policy,
	struct inet_protocol.protocol,
	struct inode.i_blksize,
	struct inode.i_lock,
	struct inode.i_mutex,
	struct inode.i_private,
	struct kobject.kref,
	struct kstatfs.f_type,
	struct net.dev_base_head,
	struct net_device.hard_header,
	struct net_device.rebuild_header,
	struct net_protocol.next,
	struct net_protocol.no_policy,
	struct net_protocol.proto,
	struct packet_type.af_packet_priv,
	struct packet_type.data,
	struct packet_type.list,
	struct packet_type.next,
	struct sk_buff.h.sh,
	struct sk_buff.transport_header,
	struct sock.protinfo.af_inet.ttl,
	struct sock.protinfo.af_inet.uc_ttl,
	struct sock.sk_debug,
	struct sock.sk_localroute,
	struct sock.tp_pinfo.af_sctp,
	struct super_block.s_fs_info,
	struct super_block.u.generic_sbp,
	struct super_operations.drop_inode,
	struct super_operations.put_inode,
	struct super_operations.read_inode,
	struct super_operations.read_inode2,
	struct task_struct.namespace.sem,
	struct task_struct.pgrp,
	struct task_struct.session,
	struct task_struct.signal], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
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
#ifdef HAVE_KINC_LINUX_FDTABLE_H
#include <linux/fdtable.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#endif
#include <linux/sysctl.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/protocol.h>
#ifdef HAVE_NET_DST_H
#include <net/dst.h>
#endif
    ]) # _LINUX_CHECK_MEMBERS
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_EXPORTS([
	alloc_skb,
	__generic_copy_to_user,
	inetdev_lock,
	inet_family_ops,
	inet_getsockopt,
	inet_recvmsg,
	inet_setsockopt,
	irq_stat,
	__lock_sock,
	__out_of_line_bug,
	path_lookup,
	__pollwait,
	raise_softirq,
	raise_softirq_irqoff,
	__release_sock,
	skb_linearize,
	__symbol_get,
	__symbol_put,
	sysctl_ip_default_ttl])
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_EXPORTS([
	add_wait_queue,
	add_wait_queue_exclusive,
	create_proc_entry,
	del_timer,
	dev_base,
	dev_base_lock,
	do_softirq,
	free_pages,
	__get_free_pages,
	__get_user_4,
	inet_accept,
	inet_add_protocol,
	inet_addr_type,
	inet_del_protocol,
	inet_register_protosw,
	inet_release,
	inet_sendmsg,
	inet_shutdown,
	inet_stream_connect,
	inet_unregister_protosw,
	ip_cmsg_recv,
	ip_fragment,
	ip_route_output_key,
	__ip_select_ident,
	ip_send_check,
	jiffies,
	kfree,
	__kfree_skb,
	kill_pg,
	kill_proc,
	kmem_cache_alloc,
	kmem_cache_create,
	kmem_cache_destroy,
	kmem_cache_free,
	kmem_find_general_cachep,
	mod_timer,
	net_statistics,
	nf_hooks,
	nf_hook_slow,
	num_physpages,
	panic,
	printk,
	proc_dointvec,
	proc_dointvec_jiffies,
	proc_dointvec_minmax,
	proc_doulongvec_ms_jiffies_minmax,
	proc_net,
	___pskb_trim,
	put_cmsg,
	register_sysctl_table,
	remove_proc_entry,
	remove_wait_queue,
	schedule_timeout,
	secure_tcp_sequence_number,
	send_sig,
	sk_alloc,
	skb_clone,
	skb_copy_datagram_iovec,
	skb_over_panic,
	skb_realloc_headroom,
	skb_under_panic,
	sk_free,
	sk_run_filter,
	sock_no_mmap,
	sock_no_sendpage,
	sock_no_socketpair,
	sock_wake_async,
	sock_wfree,
	sprintf,
	sysctl_intvec,
	sysctl_jiffies,
	sysctl_local_port_range,
	unregister_sysctl_table,
	__wake_up], [], [dnl
	    AC_MSG_WARN([
**** 
**** Linux kernel symbol ']LK_Export[' should be exported but it
**** isn't.  This could cause problems later.
**** ])])
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_SYMBOL_EXPORT([cdev_put])
    _LINUX_KERNEL_SYMBOL_EXPORT([def_fifo_fops])
    _LINUX_KERNEL_SYMBOL_EXPORT([do_exit])
    _LINUX_KERNEL_SYMBOL_EXPORT([file_kill])
    _LINUX_KERNEL_SYMBOL_EXPORT([file_move])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([icmp_statistics])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_min_pmtu])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_mtu_expires])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([pcibios_init])
    _LINUX_KERNEL_SYMBOL_EXPORT([put_filp])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([secure_tcp_sequence_number])
    _LINUX_KERNEL_SYMBOL_EXPORT([sock_readv_writev])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_default_ttl])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_dynaddr])
dnl _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_ip_nonlocal_bind])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_rmem_max])
    _LINUX_KERNEL_SYMBOL_EXPORT([sysctl_wmem_max])
    _LINUX_KERNEL_SYMBOL_EXPORT([__wake_up_sync])
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_SYMBOLS([
	afinet_get_info,
	cd_forget,
	check_mnt,
	compat_ptr,
	dev_base_head,
	group_send_sig_info,
	icmp_err_convert,
	icmp_statistics,
	inet_bind,
	inet_getname,
	inet_ioctl,
	inet_multi_getname,
	inet_proto_lock,
	inet_protos,
	ioctl32_hash_table,
	ioctl32_sem,
	ip_cmsg_send,
	ip_frag_mem,
	ip_frag_nqueues,
	ip_getsockopt,
	ip_route_output_flow,
	__ip_route_output_key,
	ip_rt_min_pmtu,
	ip_rt_mtu_expires,
	ip_rt_update_pmtu,
	ipsec_sk_policy,
	ip_setsockopt,
	is_current_pgrp_orphaned,
	is_ignored,
	is_orphaned_pgrp,
	kill_pgrp,
	kill_proc_info,
	kill_sl,
	kthread_bind,
	kthread_create,
	kthread_should_stop,
	kthread_stop,
	module_text_address,
	mount_sem,
	namespace_sem,
	raw_prot,
	sched_setscheduler,
	send_group_sig_info,
	session_of_pgrp,
	__setscheduler,
	skbuff_head_cache,
	snmp_get_info,
	sock_alloc,
	socket_get_info,
	sysctl_check_table,
	sysctl_ip_dynaddr,
	sysctl_ip_nonlocal_bind,
	sysctl_local_port_range,
	tasklist_lock,
	task_rq_lock,
	task_rq_unlock,
	tcp_current_mss,
	tcp_memory_allocated,
	tcp_orphan_count,
	tcp_prot,
	__tcp_push_pending_frames,
	tcp_set_skb_tso_factor,
	tcp_set_skb_tso_segs,
	tcp_sockets_allocated,
	tcp_tw_count,
	udp_prot,
	__xfrm_policy_check,
	xfrm_policy_delete,
	__xfrm_sk_clone_policy]) # _LINUX_KERNEL_SYMBOLS
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
dnl----------------------------------------------------------------------------
    AH_TEMPLATE([kmem_cachep_t], [This kmem_cache_t is deprecated in recent
	2.6.20 kernels.  When it is deprecated, define this to struct
	kmem_cache *.])
    if test :"${linux_cv_type_kmem_cache_t_p:-no}" = :no ; then
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [struct kmem_cache *])
    else
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [kmem_cache_t *])
    fi
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel kmem_cache_create with 5 args],
		       [linux_cv_kmem_cache_create_5_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/types.h>
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
#include <linux/time.h>		/* for struct timespec */]],
		    [[struct kmem_cache *(*my_autoconf_function_pointer)
		      (const char *, size_t, size_t, unsigned long,
		       void (*)(struct kmem_cache *, void *)) =
		       &kmem_cache_create;]]) ],
		[linux_cv_kmem_cache_create_5_args='yes'],
		[linux_cv_kmem_cache_create_5_args='no'])
	    ])
	if test :$linux_cv_kmem_cache_create_5_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_KMEM_CACHE_CREATE_5_ARGS], [1], [Define if
		function kmem_cache_create takes 4 arguments.])
	fi
	AH_VERBATIM([kmem_create_cache],
[/* silly kernel developers */
#ifdef HAVE_KFUNC_KMEM_CACHE_CREATE_5_ARGS
#define kmem_create_cache(a1,a2,a3,a4,a5,a6) kmem_cache_create(a1,a2,a3,a4,a5)
#else
#define kmem_create_cache(a1,a2,a3,a4,a5,a6) kmem_cache_create(a1,a2,a3,a4,a5,a6)
#endif])dnl
    ])
    at_ioctl_getmsg="$linux_cv_member_struct_file_operations_unlocked_ioctl"
    AC_SUBST([at_ioctl_getmsg])dnl
    AM_CONDITIONAL(USING_IOCTL_GETPMSG_PUTPMSG, test :$at_ioctl_getmsg = :yes)dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_CACHE_CHECK([for kernel inode_operation lookup with nameidata],
			   [linux_cv_have_iop_lookup_nameidata], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
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
#include <linux/types.h>
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
#include <linux/types.h>
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
#include <linux/types.h>
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
#include <linux/types.h>
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
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel irq_handler_t has 2 arguments], [linux_cv_type_irq_handler_t_2args], [dnl
	    if test :$linux_cv_type_irq_handler_t = :yes ; then
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
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
]],
		    [[
irq_handler_t my_autoconf_function_pointer1 = NULL;
irqreturn_t (*my_autoconf_function_pointer2)(int, void *) = NULL;
my_autoconf_function_pointer1 = my_autoconf_function_pointer2;
]]) ],
		    [linux_cv_type_irq_handler_t_2args='yes'],
		    [linux_cv_type_irq_handler_t_2args='no'])
	    else
		linux_cv_type_irq_handler_t_2args='no'
	    fi
	])
	if test :$linux_cv_type_irq_handler_t_2args = :yes ; then
	    AC_DEFINE([HAVE_KTYPE_IRQ_HANDLER_2ARGS], [1], [Define if
		'irq_handler_t' takes 2 arguments.])
	fi
    ])
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel ip_route_output], [linux_cv_have_ip_route_output], [dnl
	    CFLAGS="$CFLAGS -Werror-implicit-function-declaration"
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>]],
		[[ip_route_output(NULL, 0, 0, 0, 0);]]) ],
		    [linux_cv_have_ip_route_output='yes'],
		    [linux_cv_have_ip_route_output='no'])
	    ])
	if test :$linux_cv_have_ip_route_output = :yes ; then
	    AC_DEFINE([HAVE_IP_ROUTE_OUTPUT_EXPLICIT], [1], [Define if you have
		the explicit version of ip_route_output.])
	fi
    ])
    if test :"${linux_cv_have_ip_route_output:-no}" = :yes ; then
	AC_DEFINE([HAVE_IP_ROUTE_OUTPUT], [1], [Most 2.4 kernels have the
	    function ip_route_output() defined.  Newer RH kernels (EL3) use the
	    2.6 functions and do not provide ip_route_output().  Define this
	    macro if your kernel provides ip_route_output().])
dnl     else
dnl 	_LINUX_KERNEL_SYMBOL_EXPORT([ip_route_output_flow], [dnl
dnl 	    AC_MSG_ERROR([
dnl **** 
dnl **** To use the package on newer kernels requires the availability of the
dnl **** function ip_route_output_flow() to permit calling ip_route_connect()
dnl **** which is an inline in net/route.h.  I cannot find this symbol on your
dnl **** system and the resulting kernel module will therefore not load.
dnl ****
dnl 		    ])
dnl 	])
dnl 	_LINUX_KERNEL_SYMBOL_EXPORT([__ip_route_output_key], [dnl
dnl 	    AC_MSG_ERROR([
dnl **** 
dnl **** To use the package on newer kernels requires the availability of the
dnl **** function __ip_route_output_key() to permit calling ip_route_connect()
dnl **** which is an inline in net/route.h.  I cannot find this symbol on your
dnl **** system and the resulting kernel module will therefore not load.
dnl ****
dnl 		    ])
dnl 	])
    fi
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	if test :$linux_cv_xfrm_policy_delete_symbol = :yes ; then
	    AC_CACHE_CHECK([for kernel xfrm_policy_delete_symbol returns int],
			   [linux_cv_xfrm_policy_delete_returns_int], [dnl
		AC_COMPILE_IFELSE([
		    AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif]],
		    [[int retval = xfrm_policy_delete(NULL, 0);]]) ],
		    [linux_cv_xfrm_policy_delete_returns_int='yes'],
		    [linux_cv_xfrm_policy_delete_returns_int='no'])
	    ])
	    if test :$linux_cv_xfrm_policy_delete_returns_int = :yes ; then
		AC_DEFINE([HAVE_XFRM_POLICY_DELETE_RETURNS_INT], [1], [Define if
		    function xfrm_policy_delete returns int.])
	    fi
	fi
	AC_CACHE_CHECK([for kernel __ip_select_ident with 2 arguments], [linux_cv_have___ip_select_ident_2_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>]],
		[[void (*my_autoconf_function_pointer)(struct iphdr *, struct dst_entry *) = &__ip_select_ident;]]) ],
		[linux_cv_have___ip_select_ident_2_args='yes'],
		[linux_cv_have___ip_select_ident_2_args='no'])
	])
	if test :$linux_cv_have___ip_select_ident_2_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS], [1], [Define if
		function __ip_select_ident takes 2 arguments.])
	fi
	AC_CACHE_CHECK([for kernel __ip_select_ident with 3 arguments], [linux_cv_have___ip_select_ident_3_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>]],
		[[void (*my_autoconf_function_pointer)(struct iphdr *, struct dst_entry *, int) = &__ip_select_ident;]]) ],
		[linux_cv_have___ip_select_ident_3_args='yes'],
		[linux_cv_have___ip_select_ident_3_args='no'])
	])
	if test :$linux_cv_have___ip_select_ident_3_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS], [1], [Define if
		function __ip_select_ident takes 3 arguments.])
	fi
dnl 	if test :"${linux_cv_have___ip_select_ident_2_args:-no}" = :no \
dnl 	     -a :"${linux_cv_have___ip_select_ident_3_args:-no}" = :no
dnl 	then
dnl 	    AC_MSG_ERROR([
dnl ***
dnl *** Configure cannot determine whether your __ip_select_ident function takes
dnl *** 2 arguments or whether it takes 3 arguments.
dnl *** ])
dnl 	fi
	AC_CACHE_CHECK([for kernel skb_linearize with 1 argument], [linux_cv_have_skb_linearize_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/skbuff.h>]],
		[[int (*my_autoconf_function_pointer)(struct sk_buff *) = &skb_linearize;]]) ],
		[linux_cv_have_skb_linearize_1_arg='yes'],
		[linux_cv_have_skb_linearize_1_arg='no'])
	])
	if test :$linux_cv_have_skb_linearize_1_arg = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_SKB_LINEARIZE_1_ARG], [1], [Define if function
		skb_linearize takes 1 argument.])
	fi
	AC_CACHE_CHECK([for kernel tcp_push_pending_frames with 1 argument], [linux_cv_have_tcp_push_pending_frames_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
#include <linux/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
]],
		[[void (*my_autoconf_function_pointer)(struct sock *sk) = &tcp_push_pending_frames;]]) ],
		[linux_cv_have_tcp_push_pending_frames_1_arg='yes'],
		[linux_cv_have_tcp_push_pending_frames_1_arg='no'])
	])
	if test :$linux_cv_have_tcp_push_pending_frames_1_arg = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_TCP_PUSH_PENDING_FRAMES_1_ARG], [1], [Define
		if function tcp_push_pending_frames takes 1 argument.])
	fi
	AC_CACHE_CHECK([for kernel __tcp_push_pending_frames with 3 arguments], [linux_cv_have___tcp_push_pending_frames_3_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
#include <linux/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
]],
		[[void (*my_autoconf_function_pointer)(struct sock *, unsigned int, int) = &__tcp_push_pending_frames;]]) ],
		[linux_cv_have___tcp_push_pending_frames_3_args='yes'],
		[linux_cv_have___tcp_push_pending_frames_3_args='no'])
	])
	if test :$linux_cv_have___tcp_push_pending_frames_3_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC___TCP_PUSH_PENDING_FRAMES_3_ARGS], [1],
		[Define if function __tcp_push_pending_frames takes 3
		 arguments.])
	fi
    ])
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	AC_CHECK_MEMBER([struct inet_protocol.protocol],
	    [linux_cv_inet_protocol_style='old'],
	    [:], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/protocol.h>
	    ])
	AC_CHECK_MEMBER([struct inet_protocol.no_policy],
	    [linux_cv_inet_protocol_style='new'],
	    [:], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/protocol.h>
	    ])
	AC_CHECK_MEMBER([struct dst_entry.path],
	    [linux_cv_dst_entry_path='yes'],
	    [linux_cv_dst_entry_path='no'], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/dst.h>
	    ])
	])
    if test :"${linux_cv_inet_protocol_style:+set}" = :set ; then
	case "$linux_cv_inet_protocol_style" in
	    old)
		AC_DEFINE([HAVE_OLD_STYLE_INET_PROTOCOL], [1], [Most 2.4 kernels
		    have the old style struct inet_protocol and the old
		    prototype for inet_add_protocol() and inet_del_protocol()
		    defined in <net/protocol.h>.  Some more recent RH kernels
		    (such as EL3) use the 2.6 style of structure and prototypes.
		    Define this macro if your kernel has the old style structure
		    and prototypes.])
		;;
	    new)
		AC_DEFINE([HAVE_NEW_STYLE_INET_PROTOCOL], [1], [Most 2.4 kernels
		    have the old style struct inet_protocol and the old
		    prototype for inet_add_protocol() and inet_del_protocol()
		    defined in <net/protocol.h>.  Some more recent RH kernels
		    (such as EL3) use the 2.6 style of structure and prototypes.
		    Define this macro if your kernel has the new style structure
		    and prototypes.])
		;;
	esac
    else
	with_ip='no'
	with_udp='no'
	with_raw='no'
	with_tcp='no'
	with_sctp='no'
	with_sctp2='no'
    fi
    if test :"${linux_cv_dst_entry_path:-no}" = :yes ; then
	AC_DEFINE([HAVE_STRUCT_DST_ENTRY_PATH], [1], [Newer RHEL3 kernels change
	    the destination entry structure.  Define this macro to use the newer
	    structure.])
    fi
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([
	AC_CHECK_TYPES([struct sockaddr_storage], [
	    AC_DEFINE([HAVE_STRUCT_SOCKADDR_STORAGE], [1], [Most 2.4 kernels do
		not define struct sockaddr_storage.  Define to 1 if your kernel
		supports struct sockaddr_storage.])
	], [:], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
	    ])
	])
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([
	AC_CACHE_CHECK([for ip_frag_mem with 1 arg],
		       [linux_cv_ip_frag_mem_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[int (*my_autoconf_function_pointer)(struct net *)
		    = &ip_frag_mem;]])
		],
		[linux_cv_ip_frag_mem_1_arg='yes'],
		[linux_cv_ip_frag_mem_1_arg='no'])
	])
	if test :$linux_cv_ip_frag_mem_1_arg = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_FRAG_MEM_1_ARG], [1], [Define if function
		ip_frag_mem() takes 1 argument.])
	fi
	AC_CACHE_CHECK([for ip_frag_mem with 0 args],
		       [linux_cv_ip_frag_mem_0_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[int (*my_autoconf_function_pointer)(void)
		    = &ip_frag_mem;]])
		],
		[linux_cv_ip_frag_mem_0_args='yes'],
		[linux_cv_ip_frag_mem_0_args='no'])
	])
	if test :$linux_cv_ip_frag_mem_0_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_FRAG_MEM_0_ARGS], [1], [Define if function
		ip_frag_mem() takes 0 arguments.])
	fi
	AC_CACHE_CHECK([for ip_frag_nqueues with 1 arg],
		       [linux_cv_ip_frag_nqueues_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[int (*my_autoconf_function_pointer)(struct net *)
		    = &ip_frag_nqueues;]])
		],
		[linux_cv_ip_frag_nqueues_1_arg='yes'],
		[linux_cv_ip_frag_nqueues_1_arg='no'])
	])
	if test :$linux_cv_ip_frag_nqueues_1_arg = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_FRAG_NQUEUES_1_ARG], [1], [Define if
		function ip_frag_nqueues() takes 1 argument.])
	fi
	AC_CACHE_CHECK([for ip_frag_nqueues with 0 args],
		       [linux_cv_ip_frag_nqueues_0_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[int (*my_autoconf_function_pointer)(void)
		    = &ip_frag_nqueues;]])
		],
		[linux_cv_ip_frag_nqueues_0_args='yes'],
		[linux_cv_ip_frag_nqueues_0_args='no'])
	])
	if test :$linux_cv_ip_frag_nqueues_0_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_FRAG_NQUEUES_0_ARGS], [1], [Define if
		function ip_frag_nqueues() takes 0 arguments.])
	fi
	AC_CACHE_CHECK([for first_net_device with 1 arg],
		       [linux_cv_first_net_device_1_arg], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[struct net_device *(*my_autoconf_function_pointer)(struct net *)
		    = &first_net_device;]])
		],
		[linux_cv_first_net_device_1_arg='yes'],
		[linux_cv_first_net_device_1_arg='no'])
	])
	if test :$linux_cv_first_net_device_1_arg = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_FIRST_NET_DEVICE_1_ARG], [1], [Define if
		function first_net_device() takes 1 argument.])
	fi
	AC_CACHE_CHECK([for inet_addr_type with 2 args],
		       [linux_cv_inet_addr_type_2_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[unsigned (*my_autoconf_function_pointer)(struct net *, __be32)
		    = &inet_addr_type;]])
		],
		[linux_cv_inet_addr_type_2_args='yes'],
		[linux_cv_inet_addr_type_2_args='no'])
	])
	if test :$linux_cv_inet_addr_type_2_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_INET_ADDR_TYPE_2_ARGS], [1], [Define if
		function inet_addr_type() takes 2 arguments.])
	fi
	AC_CACHE_CHECK([for ip_route_output_key with 3 args],
		       [linux_cv_ip_route_output_key_3_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>]],
		    [[int (*my_autoconf_function_pointer)(struct net *, struct rtable **, struct
		     flowi *) = &ip_route_output_key;]])
		],
		[linux_cv_ip_route_output_key_3_args='yes'],
		[linux_cv_ip_route_output_key_3_args='no'])
	])
	if test :$linux_cv_ip_route_output_key_3_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_ROUTE_OUTPUT_KEY_3_ARGS], [1], [Define if
		function inet_addr_type() takes 3 arguments.])
	fi
    ])
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel member struct packet_type.func takes 4 args],
	    [linux_cv_kmem_struct_packet_type_func_4_args], [
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/netdevice.h>]],
[[struct packet_type temp;
(*temp.func)((struct sk_buff *)0, (struct net_device *)0, (struct packet_type *)0, (struct net_device *)0);]]) ],
		[linux_cv_kmem_struct_packet_type_func_4_args='yes'],
		[linux_cv_kmem_struct_packet_type_func_4_args='no'])
	    ])
	])
    if test :$linux_cv_kmem_struct_packet_type_func_4_args = :yes ; then
	AC_DEFINE([HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS], [1], [Define to 1
	    if the func member of the packet_type structure passes four
	    arguments instead of three.])
    fi
dnl----------------------------------------------------------------------------
    _LINUX_CHECK_MEMBER([struct sock.__sk_common], [
	_LINUX_CHECK_MEMBER([struct inet_opt.tos],
	[AC_DEFINE([HAVE_TRN_SOCK_STRUCTURE], [1], [Define to 1 i you have the
	    transitional Linux 2.6 style struct inet_opt.  Otherwise, leave
	    undefined for the newer Linux 2.6 or older Linux 2.4 struct
	    sock.])],
	[AC_DEFINE([HAVE_NEW_SOCK_STRUCTURE], [1], [Define to 1 if you have the
	    newer Linux 2.6 style struct inet_sock.  Otherwise, leave undefined
	    for either the transitional Linux 2.6 or older Linux 2.4 socket
	    structure.])
    ], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
#include <linux/ip.h>
	])],
	[AC_DEFINE([HAVE_OLD_SOCK_STRUCTURE], [1], [Define to 1 if you have the
	    older Linux 2.4 style struct sock.  Otherwise leave undefined for
	    the new Linux 2.6 struct sock.])
    ], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <net/sock.h>
    ])
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel function tcp_set_skb_tso_segs with sock argument], [linux_cv_tcp_set_skb_tso_segs_sock], [
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>]],
		[[void (*my_autoconf_function_pointer)(struct sock *, struct sk_buff *) = &tcp_set_skb_tso_segs;]]) ],
		[linux_cv_tcp_set_skb_tso_segs_sock='yes'],
		[linux_cv_tcp_set_skb_tso_segs_sock='no'])
	    ])
	])
    if test :$linux_cv_tcp_set_skb_tso_segs_sock = :yes ; then
	AC_DEFINE([HAVE_KFUNC_TCP_SET_SKB_TSO_SEGS_SOCK], [1], [Define to 1 if
	    function tcp_set_skb_tso_segs takes a
	    (struct sock *, struct sk_buff *) argument list.])
    fi
dnl----------------------------------------------------------------------------
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel ip_route_connect with 9 arguments], [linux_cv_have_ip_route_connect_9_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>]],
		[[int (*my_autoconf_function_pointer)(struct rtable **, __be32, __be32, u32, int, u8, __be16, __be16, struct sock *) = &ip_route_connect;]]) ],
		[linux_cv_have_ip_route_connect_9_args='yes'],
		[linux_cv_have_ip_route_connect_9_args='no'])
	])
	if test :$linux_cv_have_ip_route_connect_9_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_ROUTE_CONNECT_9_ARGS], [1], [Define if
		function ip_route_connect takes 9 arguments which was normally
		the case up to 2.6.20.])
	fi
	AC_CACHE_CHECK([for kernel ip_route_connect with 10 arguments], [linux_cv_have_ip_route_connect_10_args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>]],
		[[int (*my_autoconf_function_pointer)(struct rtable **, __be32, __be32, u32, int, u8, __be16, __be16, struct sock *, int) = &ip_route_connect;]]) ],
		[linux_cv_have_ip_route_connect_10_args='yes'],
		[linux_cv_have_ip_route_connect_10_args='no'])
	])
	if test :$linux_cv_have_ip_route_connect_10_args = :yes ; then
	    AC_DEFINE([HAVE_KFUNC_IP_ROUTE_CONNECT_10_ARGS], [1], [Define if
		function ip_route_connect takes 10 arguments which is the case
		from 2.6.21.])
	fi
    ])
dnl----------------------------------------------------------------------------
    EXPOSED_SYMBOLS="\
	mtp_n_uderror_ind \
	mtp_n_unitdata_ind \
	mtp_n_unitdata_req \
	sccp_n_uderror_ind \
	sccp_n_unitdata_ind \
	sccp_n_unitdata_req \
	sctp_data_ind \
	sctp_data_req \
	sctp_datack_ind \
	sctp_datack_req \
	sctp_exdata_ind \
	sctp_exdata_req"
dnl----------------------------------------------------------------------------
    lfs_pipe=yes
    lfs_fattach=yes
    _LINUX_KERNEL_SYMBOL_EXPORT([clone_mnt], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([graft_tree], [lfs_fattach=no; lfs_pipe=no])
    _LINUX_KERNEL_SYMBOL_EXPORT([do_umount], [lfs_fattach=no; lfs_pipe=no])
    AC_CACHE_CHECK([for kernel symbol support for fattach/fdetach], [os7_cv_fattach], [dnl
	os7_cv_fattach="$lfs_fattach" ])
    if test :"${os7_cv_fattach:-no}" != :no ; then
	AC_DEFINE([HAVE_KERNEL_FATTACH_SUPPORT], [1], [If the addresses for the
	    necessary symbols above are defined, then define this to include
	    fattach/fdetach support.])
    fi
    AC_CACHE_CHECK([for kernel symbol support for pipe], [os7_cv_pipe], [dnl
	os7_cv_pipe="$lfs_pipe" ])
    if test :${os7_cv_pipe:-no} != :no ; then
	AC_DEFINE([HAVE_KERNEL_PIPE_SUPPORT], [1], [If the addresses for the
	    necessary symbols above are defined, then define this to include
	    pipe support.])
    fi
dnl----------------------------------------------------------------------------
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
dnl----------------------------------------------------------------------------
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
dnl----------------------------------------------------------------------------
])# _OS7_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _OS7_CONFIG_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_CONFIG_SCTP], [dnl
    linux_cv_other_sctp='no'
    linux_cv_lksctp_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp compiled in], [CONFIG_IP_SCTP])
    if test :"$linux_cv_CONFIG_IP_SCTP" = :"yes" ; then
	linux_cv_other_sctp='lksctp'
	linux_cv_lksctp_sctp='yes'
	AC_MSG_ERROR([
**** 
**** Configure has detected a kernel with the deprecated lksctp compiled in.
**** This is NOT a recommended situation.  Installing OpenSS7 STREAMS SCTP on
**** such a bastardized kernel will most likely result in an unstable
**** situation.  Try a different kernel, or try recompiling your kernel with
**** lksctp removed (or at least compiled as a module).
**** ])
    fi
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with lksctp as module], [CONFIG_IP_SCTP_MODULE])
    if test :"$linux_cv_CONFIG_IP_SCTP_MODULE" = :"yes" ; then
	linux_cv_other_sctp='lksctp'
	linux_cv_lksctp_sctp='yes'
	AC_DEFINE([HAVE_LKSCTP_SCTP], [1], [Some more recent 2.4.25 and
	    greater kernels have this poorman version of SCTP included in the
	    kernel.  Define this symbol if you have such a bastardized kernel.
	    When we have such a kernel we need to define lksctp's header
	    wrapper defines so that none of the lksctp header files are
	    included (we use our own instead).])
    fi
    linux_cv_openss7_sctp='no'
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp compiled in], [CONFIG_SCTP])
    if test :"$linux_cv_CONFIG_SCTP" = :"yes" ; then
	linux_cv_other_sctp='openss7'
	linux_cv_openss7_sctp='yes'
	AC_MSG_WARN([
**** 
**** Configure has detected a kernel with OpenSS7 SCTP compiled in.  This is
**** NOT a recommended situation.  Installing OpenSS7 STREAMS SCTP on such a
**** kernel can lead to difficulties.  Try a different kernel, or try
**** recompiling with OpenSS7 SCTP compiled as a module, and perhaps removed.
**** ])
    fi
    _LINUX_CHECK_KERNEL_CONFIG([for kernel with openss7 sctp module], [CONFIG_SCTP_MODULE])
    if test :"$linux_cv_CONFIG_SCTP_MODULE" = :"yes" ; then
	linux_cv_other_sctp='openss7'
	linux_cv_openss7_sctp='yes'
	AC_DEFINE([HAVE_OPENSS7_SCTP], [1], [Define if your kernel supports
	    the OpenSS7 Linux Kernel Sockets SCTP patches.  This enables
	    support in the SCTP driver for STREAMS on top of the OpenSS7 Linux
	    Kernel Sockets SCTP implementation.])
    fi
    AM_CONDITIONAL([WITH_LKSCTP_SCTP], [ test :"${linux_cv_lksctp_sctp:-no}"  = :yes])dnl
    AM_CONDITIONAL([WITH_OPENSS7_SCTP], [test :"${linux_cv_openss7_sctp:-no}" = :yes])dnl
dnl----------------------------------------------------------------------------
dnl if test :"${linux_cv_openss7_sctp:-no}" = :yes ; then
dnl     with_sctp='no'
dnl     with_sctp2='no'
dnl fi
    if test :"${with_sctp2:-no}" = :yes ; then
	with_sctp='no'
    fi
    if test :"${with_sctp:-no}" = :yes -o :"${with_sctp2:-no}" = :yes ; then
	_LINUX_KERNEL_SYMBOL_EXPORT([ip_rt_update_pmtu], [with_sctp='no'; with_sctp2='no'])
    fi
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_SLOW_VERIFICATION
    AC_MSG_CHECKING([for sctp slow verification])
    AC_ARG_ENABLE([sctp-slow-verification],
	AS_HELP_STRING([--enable-sctp-slow-verification],
	    [enable slow verification of addresses and tags. @<:@default=no@:>@]),
	[os7_cv_slow_verification="$enableval"],
	[os7_cv_slow_verification='no'])
    if test :"${os7_cv_slow_verification:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_SLOW_VERIFICATION], [1], [When a message comes
	    from an SCTP endpoint with the correct verification tag, it is not
	    necessary to check ports or addresses to identify the SCTP
	    association to which it belongs.  When undefined, port numbers and
	    addresses are not checked on local tags and the addresses are not
	    checked on peer tags.  When defined, the redundant port number and
	    destination address checks are performed.  Both situations provide
	    RFC 2960 compliant operation.  If in doubt, leave this
	    undefined.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_slow_verification])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_THROTTLE_HEARTBEATS
    AC_MSG_CHECKING([for sctp throttle heartbeats])
    AC_ARG_ENABLE([sctp-throttle-heartbeats],
	AS_HELP_STRING([--enable-sctp-throttle-heartbeats],
	    [enable heartbeat throttling. @<:@default=no@:>@]),
	[os7_cv_throttle_heartbeats="$enableval"],
	[os7_cv_throttle_heartbeats='no'])
    if test :"${os7_cv_throttle_heartbeats:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_THROTTLE_HEARTBEATS], [1], [Special feature of
	    OpenSS7 SCTP which is not mentioned in RFC 2960.  When defined, SCTP
	    will throttle the rate at which it responds to heartbeats to the
	    system control sctp_heartbeat_itvl.  This makes SCTP more reslilient
	    to implementations which flood heartbeat messages.  For RFC 2960
	    compliant operation, leave this undefined.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_throttle_heartbeats])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_DISCARD_OOTB
    AC_MSG_CHECKING([for sctp dicard out-of-the-blue])
    AC_ARG_ENABLE([sctp-discard-ootb],
	AS_HELP_STRING([--enable-sctp-discard-ootb],
	    [enable discard out-of-the-blue packets. @<:@default=no@:>@]),
	[os7_cv_discard_ootb="$enableval"],
	[if test :"${linux_cv_openss7_sctp:-no}" = :yes ; then
	    os7_cv_discard_ootb='yes'
	 else
	    os7_cv_discard_ootb='no'
	 fi])
    if test :"$os7_cv_discard_ootb" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_DISCARD_OOTB], [1], [RFC 2960 requires the
	    implementation to send ABORT to some OOTB packets (packets for which
	    no SCTP association exists).  Sending ABORT chunks to unverified
	    source addreses with the T bit set opens SCTP to blind masquerade
	    attacks.  Not sending them may lead to delays at the peer endpoint
	    aborting associations where our ABORT ahs been lost and the stream
	    is already closes or if we have restarted and the peer still has
	    open associations to us.  If defined, SCTP will discard all OOTB
	    packets.  This is necessary if another SCTP stack is being run on
	    the same machine.  For RFC 2960 compliant operation, leave
	    undefined.  If in doubt, leave this undefined.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_discard_ootb])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_EXTENDED_IP_SUPPORT
    AC_MSG_CHECKING([for sctp extended ip support])
    AC_ARG_ENABLE([sctp-extended-ip-support],
	AS_HELP_STRING([--enable-sctp-extended-ip-support],
	    [enable extended IP support for SCTP. @<:@default=disabled@:>@]),
	[os7_cv_extended_ip_support="$enableval"],
	[os7_cv_extended_ip_support='no'])
    if test :"${os7_cv_extended_ip_support:-no}" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_EXTENDED_IP_SUPPORT], [1], [This provides
	    extended IP support for SCTP for things like IP Transparent Proxy
	    and IP Masquerading.  This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_extended_ip_support])
    AC_C_BIGENDIAN(
	[os7_cv_be_machine='yes'; os7_cv_le_machine='no'],
	[os7_cv_be_machine='no'; os7_cv_le_machine='yes'],
	[os7_cv_be_machine='yes'; os7_cv_le_machine='yes'])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_HMAC_SHA1
    AC_MSG_CHECKING([for sctp hmac sha-1])
    AC_ARG_ENABLE([sctp-hmac-sha1],
	AS_HELP_STRING([--disable-sctp-hmac-sha1],
	    [disable SHA-1 HMAC. @<:@default=enabled(BE),disabled(LE)@:>@]),
	[os7_cv_hmac_sha1="$enableval"],
	[os7_cv_hmac_sha1="$os7_cv_be_machine"])
    if test :"$os7_cv_hmac_sha1" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_HMAC_SHA1], [1], [When defined, this provides the
	    ability to sue the FIPS 180-1 (SHA-1) message authentication code in
	    SCTP cookies.  When defined and the appropriate sysctl and option is
	    set, SCTP will use SHA-1 HMAC when signing cookies in the INIT-ACK
	    chunk.  If undefined, the SHA-1 HMAC will be unavailable for use
	    with SCTP.  If in doubt, leave defined.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_hmac_sha1])
    AM_CONDITIONAL([WITH_HMAC_SHA1], [test :"$os7_cv_hmac_sha1" = :yes])dnl
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_HMAC_MD5
    AC_MSG_CHECKING([for sctp hmac md5])
    AC_ARG_ENABLE([sctp-hmac-md5],
	AS_HELP_STRING([--disable-sctp-hmac-md5],
	    [disable MD5 HMAC. @<:@default=enabled(LE),disabled(BE)@:>@]),
	[os7_cv_hmac_md5="$enableval"],
	[os7_cv_hmac_md5="$os7_cv_le_machine"])
    if test :"$os7_cv_hmac_md5" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_HMAC_MD5], [1], [When defined, this provides the
	    ability to use the MD5 (RFC 1321) message authentication code in
	    SCTP cookies.  If you define this macro, when the appropriate system
	    control and stream option is set, SCTP will use the MD5 HMAC when
	    signing cookies in the INIT-ACK chunk.  If this macro is undefined,
	    the HD5 HMAC will be unavailable for use with SCTP.  If in doubt,
	    leave defined.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_hmac_md5])
    AM_CONDITIONAL([WITH_HMAC_MD5], [test :"$os7_cv_hmac_md5" = :yes])dnl
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_ADLER_32
    AC_MSG_CHECKING([for sctp Adler32 checksum])
    AC_ARG_ENABLE([sctp-adler32],
	AS_HELP_STRING([--enable-sctp-adler32],
	    [enable Adler32 checksum. @<:@default=disabled@:>@]),
	[os7_cv_adler32="$enableval"],
	[os7_cv_adler32='no'])
    if test :"$os7_cv_adler32" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADLER_32], [1], [This provides the ability to use
	    the older RFC 2960 Adler32 checksum.  If SCTP_CONFIG_CRC_32 below is
	    not selected, the Adler32 checksum is always provided.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_adler32])
    AM_CONDITIONAL([WITH_ADLER_32], [test :"$os7_cv_adler32" = :yes])dnl
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_CRC_32C
    AC_MSG_CHECKING([for sctp CRC-32C checksum])
    AC_ARG_ENABLE([sctp-crc32c],
	AS_HELP_STRING([--disable-sctp-crc32c],
	    [disable CRC-32C checksum. @<:@default=enabled@:>@]),
	[os7_cv_crc32c="$enableval"],
	[os7_cv_crc32c='yes'])
    if test :"${os7_cv_adler32:-no}" != :yes ; then
	os7_cv_crc32c='yes'
    fi
    if test :"$os7_cv_crc32c" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_CRC_32C], [1], [This provides the ability to use
	    the newer CRC-32c checksum as described in RFC 3309.  When this is
	    selected and SCTP_CONFIG_ADLER_32 is not selected above, then the
	    only checksum that will be used is the CRC-32c checksum.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_crc32c])
    AM_CONDITIONAL([WITH_CRC_32C], [test :"$os7_cv_crc32c" = :yes])dnl
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_THROTTLE_PASSIVEOPENS
    AC_MSG_CHECKING([for sctp throttle passive opens])
    AC_ARG_ENABLE([sctp-throttle-passiveopens],
	AS_HELP_STRING([--enable-sctp-throttle-passiveopens],
	    [enable throttling of passive opens. @<:@default=disabled@:>@]),
	[os7_cv_throttle_passiveopens="$enableval"],
	[os7_cv_throttle_passiveopens='no'])
    if test :"$os7_cv_throttle_passiveopens" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_THROTTLE_PASSIVEOPENS], [1], [Special feature of
	    Linux SCTP not mentioned in RFC 2960.  When secure algorithms are
	    used for signing cookies, the implementation becomes vulnerable to
	    INIT and COOKIE ECHO flooding.  If defined, SCTP will only allow one
	    INIT and one COOKE ECHO to be processed in each interval
	    corresponding to the sysctl sctp_throttle_itvl.  Setting
	    sctp_throttle_itvl to 0 defeats this function.  If undefined, each
	    INIT and COOKIE ECHO will be processed.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_throttle_passiveopens])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_ECN
    AC_MSG_CHECKING([for sctp ecn])
    AC_ARG_ENABLE([sctp-ecn],
	AS_HELP_STRING([--enable-sctp-ecn],
	    [enable Explicit Congestion Notification. @<:@default=enabled@:>@]),
	[os7_cv_ecn="$enableval"],
	[os7_cv_ecn='yes'])
    if test :"$os7_cv_ecn" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ECN], [1], [This enables support for Explicit
	    Congestion Notification (ECN) chunks in SCTP messages as defined in
	    RFC 2960 and RFC 3168.  It also adds syctl (/proc/net/ipv4/sctp_ecn)
	    which allows ECN for SCTP to be disabled at runtime.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_ecn])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_LIFETIMES
    AC_MSG_CHECKING([for sctp lifetimes])
    AC_ARG_ENABLE([sctp-lifetimes],
	AS_HELP_STRING([--enable-sctp-lifetimes],
	    [enable SCTP message lifetimes. @<:@default=enabled@:>@]),
	[os7_cv_lifetimes="$enableval"],
	[os7_cv_lifetimes='yes'])
    if test :"$os7_cv_lifetimes" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_LIFETIMES], [1], [This enables support for
	    message lifetimes as described in RFC 2960.  When enabled, message
	    lifetimes can be set on messages.  See sctp(7).  This feature is
	    always enabled when Partial Reliability Support is set.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_lifetimes])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_ADD_IP
    AC_MSG_CHECKING([for sctp add ip])
    AC_ARG_ENABLE([sctp-add-ip],
	AS_HELP_STRING([--enable-sctp-add-ip],
	    [enable ADD-IP. @<:@default=enabled@:>@]),
	[os7_cv_add_ip="$enableval"],
	[os7_cv_add_ip='yes'])
    if test :"$os7_cv_add_ip" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADD_IP], [1], [This enables support for ADD-IP as
	    described in draft-ietf-tsvwg-addip-sctp-07.txt.  This allows the
	    addition and removal of IP addresses from existing connections.
	    This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_add_ip])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_ADAPTATION_LAYER_INFO
    AC_MSG_CHECKING([for sctp adaptation layer info])
    AC_ARG_ENABLE([sctp-adaptation-layer-info],
	AS_HELP_STRING([--enable-sctp-adaptation-layer-info],
	    [enable ALI. @<:@default=enabled@:>@]),
	[os7_cv_adaptation_layer_info="$enableval"],
	[os7_cv_adaptation_layer_info='yes'])
    if test :"$os7_cv_adaptation_layer_info" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ADAPTATION_LAYER_INFO], [1], [This enables
	    support for the Adaptation Layer Information parameter described in
	    draft-ietf-tsvwg-addip-sctp-07.txt for communicating application
	    layer information bits at initialization.  This is experimental
	    stuff.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_adaptation_layer_info])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_PARTIAL_RELIABILITY
    AC_MSG_CHECKING([for sctp partial reliability])
    AC_ARG_ENABLE([sctp-partial-reliability],
	AS_HELP_STRING([--enable-sctp-partial-reliability],
	    [enable SCTP Partial Reliability (PR-SCTP). @<:@default=enabled@:>@]),
	[os7_cv_partial_reliability="$enableval"],
	[os7_cv_partial_reliability='yes'])
    if test :"$os7_cv_partial_reliability" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_PARTIAL_RELIABILITY], [1], [This enables support
	    for PR-SCTP as described in draft-stewart-tsvwg-prsctp-03.txt.  This
	    allows for partial reliability of message delivery on a "timed
	    reliability" basis.  This is experimental stuff.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_partial_reliability])
dnl--------------------------------------------------------------------------
# SCTP_CONFIG_ERROR_GENERATOR
    AC_MSG_CHECKING([for sctp error generator])
    AC_ARG_ENABLE([sctp-error-generator],
	AS_HELP_STRING([--disable-sctp-error-generator],
	    [disable SCTP error generator. @<:@default=enabled@:>@]),
	[os7_cv_error_generator="$enableval"],
	[os7_cv_error_generator='yes'])
    if test :"$os7_cv_error_generator" = :yes ; then
	AC_DEFINE([SCTP_CONFIG_ERROR_GENERATOR], [1], [This provides an internal
	    error generator that can be accessed with socket options for testing
	    SCTP operation under packet loss.  You will need this option to run
	    some of the test programs distributed with the SCTP module.])dnl
    fi
    AC_MSG_RESULT([$os7_cv_error_generator])
])# _OS7_CONFIG_SCTP
# =============================================================================

# =============================================================================
# _OS7_FINALIZE
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_FINALIZE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
dnl--------------------------------------------------------------------------
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
dnl--------------------------------------------------------------------------
    case "$linux_cv_debug" in
    _DEBUG)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_DEBUG], [], [Define to perform
	    internal structure tracking within the STREAMS executive as well as
	    to provide additional /proc filesystem files for examining internal
	    structures.])
	;;
    _TEST)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TEST], [], [Define to perform
	    performance testing with debugging.  This mode does not dump massive
	    amounts of information into system logs, but peforms all assertion
	    checks.])
	;;
    _SAFE)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAFE], [], [Define to perform
	    fundamental assertion checks.  This is a safer mode of operation.])
	;;
    _NONE | *)
	AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NONE], [], [Define to perform no
	    assertion checks but report software errors.  This is the smallest
	    footprint, highest performance mode of operation.])
	;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module sth])
	if test :${enable_module_sth:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_sth='yes'
	fi
    AC_MSG_RESULT([${enable_module_sth:-module}])
    case ${enable_module_sth:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_STH], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the sth module for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the sth module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_STH_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the sth module as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the sth module as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module bufmod])
	if test :${enable_module_bufmod:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_bufmod='yes'
	fi
    AC_MSG_RESULT([${enable_module_bufmod:-module}])
    case ${enable_module_bufmod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_BUFMOD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the bufmod module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the bufmod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_BUFMOD_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the bufmod module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the bufmod module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module nullmod])
	if test :${enable_module_nullmod:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_nullmod='yes'
	fi
    AC_MSG_RESULT([${enable_module_nullmod:-module}])
    case ${enable_module_nullmod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULLMOD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the nullmod module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the nullmod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULLMOD_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the nullmod module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the nullmod module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module pipemod])
	if test :${enable_module_pipemod:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_pipemod='yes'
	fi
    AC_MSG_RESULT([${enable_module_pipemod:-module}])
    case ${enable_module_pipemod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the pipemod module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the pipemod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPEMOD_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the pipemod module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the pipemod module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module connld])
	if test :${enable_module_connld:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_connld='yes'
	fi
    AC_MSG_RESULT([${enable_module_connld:-module}])
    case ${enable_module_connld:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the connld module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the connld module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CONNLD_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the connld module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the connld module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module sc])
	if test :${enable_module_sc:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_sc='yes'
	fi
    AC_MSG_RESULT([${enable_module_sc:-module}])
    case ${enable_module_sc:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the sc module for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the sc module for linkage with STREAMS.]) ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SC_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the sc module as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the sc module as a standalone loadable kernel
	    module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module testmod])
	if test :$enable_module_testmod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_testmod='yes'
	fi
    AC_MSG_RESULT([${enable_module_testmod:-module}])
    case ${enable_module_testmod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TESTMOD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the testmod module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the testmod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TESTMOD_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the testmod module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the testmod module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module timod])
	if test :$enable_module_timod = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_timod='yes'
	fi
    AC_MSG_RESULT([${enable_module_timod:-module}])
    case ${enable_module_timod:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TIMOD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the timod module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the timod module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TIMOD_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the timod module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the timod module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS module tirdwr])
	if test :$enable_module_tirdwr = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_module_tirdwr='yes'
	fi
    AC_MSG_RESULT([${enable_module_tirdwr:-module}])
    case ${enable_module_tirdwr:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TIRDWR], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the tirdwr module for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the tirdwr module for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_TIRDWR_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the tirdwr module as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the tirdwr module as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver clone])
	if test :${enable_driver_clone:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_clone='yes'
	fi
    AC_MSG_RESULT([${enable_driver_clone:-module}])
    case ${enable_driver_clone:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the clone driver for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the clone driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_CLONE_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the clone driver as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the clone driver as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver echo])
	if test :${enable_driver_echo:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_echo='yes'
	fi
    AC_MSG_RESULT([${enable_driver_echo:-module}])
    case ${enable_driver_echo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the echo driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the echo driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_ECHO_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the echo driver as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the echo driver as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver fifo])
	if test :${enable_driver_fifo:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_fifo='yes'
	fi
    AC_MSG_RESULT([${enable_driver_fifo:-module}])
    case ${enable_driver_fifo:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the fifo driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the fifo driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_FIFO_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the fifo driver as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the fifo driver as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver log])
	if test :${enable_driver_log:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_log='yes'
	fi
    AC_MSG_RESULT([${enable_driver_log:-module}])
    case ${enable_driver_log:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the log driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the log driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOG_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the log driver as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the log driver as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver loop])
	if test :${enable_driver_loop:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_loop='yes'
	fi
    AC_MSG_RESULT([${enable_driver_loop:-module}])
    case ${enable_driver_loop:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the loop driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the loop driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_LOOP_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the loop driver as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the loop driver as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver nsdev])
	if test :${enable_driver_nsdev:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_nsdev='yes'
	fi
    AC_MSG_RESULT([${enable_driver_nsdev:-module}])
    case ${enable_driver_nsdev:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the nsdev driver for linkage
		with STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not
		include the nsdev driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NSDEV_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the nsdev driver as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the nsdev driver as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver mux])
	if test :${enable_driver_mux:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_mux='yes'
	fi
    AC_MSG_RESULT([${enable_driver_mux:-module}])
    case ${enable_driver_mux:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MUX], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the mux driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the mux driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_MUX_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the mux driver as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the mux driver as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver nuls])
	if test :${enable_driver_nuls:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_nuls='yes'
	fi
    AC_MSG_RESULT([${enable_driver_nuls:-module}])
    case ${enable_driver_nuls:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the nuls driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the nuls driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_NULS_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the nuls driver as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the nuls driver as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver pipe])
	if test :${enable_driver_pipe:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_pipe='yes'
	fi
    AC_MSG_RESULT([${enable_driver_pipe:-module}])
    case ${enable_driver_pipe:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the pipe driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the pipe driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_PIPE_MODULE], [1], [When
		defined,] AC_PACKAGE_TITLE [will include the pipe driver as a
		standalone loadable kernel module.  When undefined,]
		AC_PACKAGE_TITLE [will not include the pipe driver as a
		standalone loadable kernel module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver sad])
	if test :${enable_driver_sad:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_sad='yes'
	fi
    AC_MSG_RESULT([${enable_driver_sad:-module}])
    case ${enable_driver_sad:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the sad driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the sad driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SAD_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the sad driver as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the sad driver as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver sfx])
	if test :${enable_driver_sfx:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_sfx='yes'
	fi
    AC_MSG_RESULT([${enable_driver_sfx:-module}])
    case ${enable_driver_sfx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SFX], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the sfx driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the sfx driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SFX_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the sfx driver as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the sfx driver as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS driver spx])
	if test :${enable_driver_spx:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_driver_spx='yes'
	fi
    AC_MSG_RESULT([${enable_driver_spx:-module}])
    case ${enable_driver_spx:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SPX], [1], [When defined,]
		AC_PACKAGE_TITLE [ will include the spx driver for linkage with
		STREAMS.  When undefined,] AC_PACKAGE_TITLE [will not include
		the spx driver for linkage with STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_SPX_MODULE], [1], [When defined,]
		AC_PACKAGE_TITLE [will include the spx driver as a standalone
		loadable kernel module.  When undefined,] AC_PACKAGE_TITLE [will
		not include the spx driver as a standalone loadable kernel
		module.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS OpenSS7 compatibility])
	if test :${enable_compat_os7:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_os7='yes'
	fi
    AC_MSG_RESULT([${enable_compat_os7:-module}])
    case ${enable_compat_os7:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OS7], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the OpenSS7 docs so that
		STREAMS drivers and modules written to OpenSS7 specs will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for OpenSS7 will require porting in
		more respects.  This symbol determines whether compatibility
		will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OS7_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the OpenSS7 docs so
		that STREAMS drivers and modules written to OpenSS7 specs will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for OpenSS7 will require porting in
		more respects.  This symbol determines whether compatibility
		will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS UNIX(R) SVR 3.2 compatibility])
	if test :${enable_compat_svr3:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_svr3='yes'
	fi
    AC_MSG_RESULT([${enable_compat_svr3:-module}])
    case ${enable_compat_svr3:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR3], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the UNIX(R) SVR 3.2 docs so
		that STREAMS drivers and modules written to UNIX(R) SVR 3.2
		specs will compile with Linux Fast STREAMS.  When undefined,
		STREAMS drivers and modules written for UNIX(R) SVR 3.2 will
		require porting in more respects.  This symbol determines
		whether compatibility will be compiled and linkable with Linux
		Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR3_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the UNIX(R) SVR 3.2
		docs so that STREAMS drivers and modules written to UNIX(R) SVR
		3.2 specs will compile with Linux Fast STREAMS.  When undefined,
		STREAMS drivers and modules written for UNIX(R) SVR 3.2 will
		require porting in more respects.  This symbol determines
		whether compatibility will be compiled as a loadable module to
		Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS Solaris(R) 8 compatibility])
	case ${enable_compat_sol8:-module} in
	    (yes) enable_compat_svr4=yes ;;
	    (module) if test :$enable_compat_svr4 != :yes ; then enable_compat_svr4=module ; fi ;;
	esac
	if test :${enable_compat_sol8:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_sol8='yes'
	fi
    AC_MSG_RESULT([${enable_compat_sol8:-module}])
    case ${enable_compat_sol8:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the Solaris(R) 8 release so
		that STREAMS drivers and modules written for Solaris(R) 8 will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for Solaris(R) 8 will require
		porting in more respects.  This symbol determines whether
		compatibility will be compiled and linkable with Linux
		Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SUN_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the Solaris(R) 8
		release so that STREAMS drivers and modules written for
		Solaris(R) 8 will compile with Linux Fast STREAMS.  When
		undefined, STREAMS drivers and modules written for Solaris(R) 8
		will require porting in more respects.  This symbol determines
		whether compatibility will be compiled as a loadable module to
		Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS UnixWare(R) 7 compatibility])
	case ${enable_compat_uw7:-module} in
	    (yes) enable_compat_svr4=yes ;;
	    (module) if test :$enable_compat_svr4 != :yes ; then enable_compat_svr4=module ; fi ;;
	esac
	if test :${enable_compat_uw7:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_uw7='yes'
	fi
    AC_MSG_RESULT([${enable_compat_uw7:-module}])
    case ${enable_compat_uw7:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the UnixWare(R) 7 release so
		that STREAMS drivers and modules written for UnixWare(R) 7 will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for UnixWare(R) 7 will require
		porting in more respects.  This symbol determines whether
		compatibility will be compiled and linkable with Linux
		Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_UW7_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the UnixWare(R) 7
		release so that STREAMS drivers and modules written for
		UnixWare(R) 7 will compile with Linux Fast STREAMS.  When
		undefined, STREAMS drivers and modules written for UnixWare(R) 7
		will require porting in more respects.  This symbol determines
		whether compatibility will be compiled as a loadable module to
		Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS OSF/1.2 compatibility])
	case ${enable_compat_osf:-module} in
	    (yes) enable_compat_svr4=yes ;;
	    (module) if test :$enable_compat_svr4 != :yes ; then enable_compat_svr4=module ; fi ;;
	esac
	case ${enable_compat_osf:-module} in
	    (yes) enable_compat_mps=yes ;;
	    (module) if test :$enable_compat_mps != :yes ; then enable_compat_mps=module ; fi ;;
	esac
	if test :${enable_compat_osf:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_osf='yes'
	fi
    AC_MSG_RESULT([${enable_compat_osf:-module}])
    case ${enable_compat_osf:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the OSF(R)/1.2 release so
		that STREAMS drivers and modules written for OSF(R)/1.2 will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for OSF(R)/1.2 will require porting
		in more respects.  This symbol determines whether compatibility
		will be compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_OSF_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the OSF(R)/1.2
		release so that STREAMS drivers and modules written for
		OSF(R)/1.2 will compile with Linux Fast STREAMS.  When
		undefined, STREAMS drivers and modules written for OSF(R)/1.2
		will require porting in more respects.  This symbol determines
		whether compatibility will be compiled as a loadable module to
		Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS AIX(R) 4 compatibility])
	case ${enable_compat_aix:-module} in
	    (yes) enable_compat_svr4=yes ;;
	    (module) if test :$enable_compat_svr4 != :yes ; then enable_compat_svr4=module ; fi ;;
	esac
	case ${enable_compat_aix:-module} in
	    (yes) enable_compat_mps=yes ;;
	    (module) if test :$enable_compat_mps != :yes ; then enable_compat_mps=module ; fi ;;
	esac
	if test :${enable_compat_aix:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_aix='yes'
	fi
    AC_MSG_RESULT([${enable_compat_aix:-module}])
    case ${enable_compat_aix:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the AIX(R) 5L Version 5.1
		release so that STREAMS drivers and modules written for AIX(R)
		5L Version 5.1 will compile with Linux Fast STREAMS.  When
		undefined, STREAMS drivers and modules written for AIX(R) 5L
		Version 5.1 will require porting in more respects.  This symbol
		determines whether compatibility will be compiled and linkable
		with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_AIX_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the AIX(R) 5L
		Version 5.1 release so that STREAMS drivers and modules written
		for AIX(R) 5L Version 5.1 will compile with Linux Fast STREAMS.
		When undefined, STREAMS drivers and modules written for AIX(R)
		5L Version 5.1 will require porting in more respects.  This
		symbol determines whether compatibility will be compiled as a
		loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS HPUX(R) compatibility])
	case ${enable_compat_hpux:-module} in
	    (yes) enable_compat_svr4=yes ;;
	    (module) if test :$enable_compat_svr4 != :yes ; then enable_compat_svr4=module ; fi ;;
	esac
	case ${enable_compat_hpux:-module} in
	    (yes) enable_compat_mps=yes ;;
	    (module) if test :$enable_compat_mps != :yes ; then enable_compat_mps=module ; fi ;;
	esac
	if test :${enable_compat_hpux:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_hpux='yes'
	fi
    AC_MSG_RESULT([${enable_compat_hpux:-module}])
    case ${enable_compat_hpux:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the HPUX(R) release so that
		STREAMS drivers and modules written for HPUX(R) will compile
		with Linux Fast STREAMS.  When undefined, STREAMS drivers and
		modules written for HPUX(R) will require porting in more
		respects.  This symbol determines whether compatibility will be
		compiled and linkable with Linux Fast-STREAMS.  ])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_HPUX_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the HPUX(R) release
		so that STREAMS drivers and modules written for HPUX(R) will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for HPUX(R) will require porting in
		more respects.  This symbol determines whether compatibility
		will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS IRIX(R) compatibility])
	case ${enable_compat_irix:-module} in
	    (yes) enable_compat_svr4=yes ;;
	    (module) if test :$enable_compat_svr4 != :yes ; then enable_compat_svr4=module ; fi ;;
	esac
	if test :${enable_compat_irix:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_irix='yes'
	fi
    AC_MSG_RESULT([${enable_compat_irix:-module}])
    case ${enable_compat_irix:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_IRIX], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the IRIX(R) release so that
		STREAMS drivers and modules written for IRIX(R) will compile
		with Linux Fast STREAMS.  When undefined, STREAMS drivers and
		modules written for IRIX(R) will require porting in more
		respects.  This symbol determines whether compatibility will be
		compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_IRIX_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the IRIX(R) release
		so that STREAMS drivers and modules written for IRIX(R) will
		compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for IRIX(R) will require porting in
		more respects.  This symbol determines whether compatibility
		will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS MacOT compatibility])
	case ${enable_compat_mac:-module} in
	    (yes) enable_compat_mps=yes ;;
	    (module) if test :$enable_compat_mps != :yes ; then enable_compat_mps=module ; fi ;;
	esac
	if test :$enable_compat_mac = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_mac='yes'
	fi
    AC_MSG_RESULT([${enable_compat_mac:-module}])
    case ${enable_compat_mac:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MAC], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the MacOT(R) release so that
		STREAMS drivers and modules written for MacOT(R) will compile
		with Linux Fast STREAMS.  When undefined, STREAMS drivers and
		modules written for MacOT(R) will require porting in more
		respects.  This symbol determines whether compatibility will be
		compiled and linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MAC_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the MacOT(R)
		release so that STREAMS drivers and modules written for MacOT(R)
		will compile with Linux Fast STREAMS.  When undefined, STREAMS
		drivers and modules written for MacOT(R) will require porting in
		more respects.  This symbol determines whether compatibility
		will be compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS UNIX(R) SVR 4.2 compatibility])
	if test :${enable_compat_svr4:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_svr4='yes'
	fi
    AC_MSG_RESULT([${enable_compat_svr4:-module}])
    case ${enable_compat_svr4:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the UNIX(R) SVR 4.2 MP docs
		so that STREAMS drivers and modules written to UNIX(R) SVR 4.2
		MP specs will compile with Linux Fast STREAMS.  When undefined,
		STREAMS drivers and modules written for UNIX(R) SVR 4.2 MP will
		require porting in more respects.  This symbol determines
		whether compatibility will be compiled and linkable with Linux
		Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_SVR4_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the UNIX(R) SVR 4.2
		MP docs so that STREAMS drivers and modules written to UNIX(R)
		SVR 4.2 MP specs will compile with Linux Fast STREAMS.  When
		undefined, STREAMS drivers and modules written for UNIX(R) SVR
		4.2 MP will require porting in more respects.  This symbol
		determines whether compatibility will be compiled as a loadable
		module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl----------------------------------------------------------------------------
    AC_MSG_CHECKING([for STREAMS MPS(R) compatibility])
	if test :${enable_compat_mps:-module} = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    enable_compat_mps='yes'
	fi
    AC_MSG_RESULT([${enable_compat_mps:-module}])
    case ${enable_compat_mps:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MPS], [], [When defined,
		Linux Fast STREAMS will attempt to be as compatible as possible
		(without replicating any bugs) with the Mentat Portable STREAMS
		documentation so that STREAMS drivers and modules written for
		Mentat Portable STREAMS will compile with Linux Fast STREAMS.
		When undefined, STREAMS drivers and modules written for Mentat
		Portable STREAMS will require porting in more respects.  This
		symbol determines whether compatibility will be compiled and
		linkable with Linux Fast-STREAMS.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_STREAMS_COMPAT_MPS_MODULE], [], [When
		defined, Linux Fast STREAMS will attempt to be as compatible as
		possible (without replicating any bugs) with the Mentat Portable
		STREAMS documentation so that STREAMS drivers and modules
		written for Mentat Portable STREAMS will compile with Linux Fast
		STREAMS.  When undefined, STREAMS drivers and modules written
		for Mentat Portable STREAMS will require porting in more
		respects.  This symbol determines whether compatibility will be
		compiled as a loadable module to Linux Fast-STREAMS.])
	    ;;
    esac
dnl--------------------------------------------------------------------------
])# _OS7_FINALIZE
# =============================================================================

# =============================================================================
# _OS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_OUTPUT], [dnl
    _OS7_CONFIG
    _OS7_STRCONF
])# _OS7_OUTPUT
# =============================================================================

# =============================================================================
# _OS7_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    os7_cv_config="${pkg_bld}/src/include/sys/${PACKAGE}/config.h"
    os7_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    os7_cv_ldadd="${pkg_bld}/libstreams.la ${pkg_bld}/libtty.la ${pkg_bld}/libdlpi.la ${pkg_bld}/libxnet.la ${pkg_bld}/libxnsl.la ${pkg_bld}/libsocket.la ${pkg_bld}/libsx25.la ${pkg_bld}/libosi.la"
    os7_cv_ldflags="-L${pkg_bld}/.libs/"
    os7_cv_ldadd32="${pkg_bld}/lib32/libstreams.la ${pkg_bld}/lib32/libtty.la ${pkg_bld}/lib32/libdlpi.la ${pkg_bld}/lib32/libxnet.la ${pkg_bld}/lib32/libxnsl.la ${pkg_bld}/lib32/libsocket.la ${pkg_bld}/lib32/libsx25.la ${pkg_bld}/lib32/libosi.la"
    os7_cv_ldflags32="-L${pkg_bld}/lib32/.libs/"
    os7_cv_manpath="${pkg_bld}/doc/man"
    os7_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    os7_cv_modmap="${pkg_bld}/Modules.map"
    os7_cv_symver="${pkg_bld}/Module.symvers"
    os7_cv_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
    os7_cv_version="${VERSION}"
    os7_cv_package="LfS"
])# _OS7_CONFIG
# =============================================================================

# =============================================================================
# _OS7_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_STRCONF], [dnl
    AC_REQUIRE([_LINUX_KERNEL])
    strconf_prefix='os7'
    os7_cv_stem='Config'
    AC_CACHE_CHECK([for strconf major device number base], [os7_cv_majbase], [dnl
	if test ${linux_cv_minorbits:-8} -gt 8 ; then
	    os7_cv_majbase=2001
	else
	    os7_cv_majbase=231
	fi
    ])
    AC_CACHE_CHECK([for strconf module id base], [os7_cv_midbase], [dnl
	os7_cv_midbase=5001
    ])
    os7_cv_sconfig='src/include/sys/config.h'
    os7_cv_mknodes="src/util/${PACKAGE_TARNAME}_mknod.c"
    os7_cv_modconf="src/include/sys/modconf.h"
    _STRCONF
])# _OS7_STRCONF
# =============================================================================

# =============================================================================
# _OS7_
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_], [dnl
])# _OS7_
# =============================================================================

# =============================================================================
#
# $Log$
# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
