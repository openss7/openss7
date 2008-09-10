# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2008-09-10 03:49:55 $
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
# Last Modified $Date: 2008-09-10 03:49:55 $ by $Author: brian $
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
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/nsl.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_SOCK
# -----------------------------------------------------------------------------
AC_DEFUN([AC_SOCK], [dnl
    _OPENSS7_PACKAGE([SOCK], [OpenSS7 STREAMS Sockets])
    _SOCK_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strsock-core.postinst
		     debian/strsock-core.postrm
		     debian/strsock-core.preinst
		     debian/strsock-core.prerm
		     debian/strsock-devel.preinst
		     debian/strsock-dev.postinst
		     debian/strsock-dev.preinst
		     debian/strsock-dev.prerm
		     debian/strsock-doc.postinst
		     debian/strsock-doc.preinst
		     debian/strsock-doc.prerm
		     debian/strsock-init.postinst
		     debian/strsock-init.postrm
		     debian/strsock-init.preinst
		     debian/strsock-init.prerm
		     debian/strsock-lib.preinst
		     debian/strsock-source.preinst
		     debian/strsock-util.preinst
		     src/util/modutils/strsock
		     src/util/sysconfig/strsock
		     src/include/sys/strsock/version.h
		     Module.mkvars])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _SOCK_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${NSL_CPPFLAGS:+ }}${NSL_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SS7_CPPFLAGS:+ }}${SS7_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    if test :${linux_cv_k_ko_modules:-no} = :no ; then
	PKG_MODFLAGS='$(STREAMS_MODFLAGS) $(STRCOMP_MODFLAGS)'
dnl	if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	    PKG_MODFLAGS="${PKG_MODFLAGS}${PKG_MODFLAGS:+ }"'-include ${top_builddir}/${MODVERSIONS_H}'
dnl	    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/include'
dnl	fi
    fi
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
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
    PKG_MANPATH="${SS7_MANPATH:+${SS7_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XNS_MANPATH:+${XNS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XTI_MANPATH:+${XTI_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${NSL_MANPATH:+${NSL_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _SOCK_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_SOCK
# =============================================================================

# =============================================================================
# _SOCK_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_OPTIONS], [dnl
])# _SOCK_OPTIONS
# =============================================================================

# =============================================================================
# _SOCK_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LINUX_STREAMS
    _STRCOMP
    _XNS
    _XTI
    _NSL
    _SOCK_SETUP_MODULE
    _SOCK_CONFIG_KERNEL
])# _SOCK_SETUP
# =============================================================================

# =============================================================================
# _SOCK_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_SETUP_MODULE], [dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([SOCK_CONFIG_MODULE], [], [When defined, SOCK is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([SOCK_CONFIG], [], [When defined, SOCK is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([SOCK_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([SOCK_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])
# =============================================================================

# =============================================================================
# _SOCK_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuration checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_CONFIG_KERNEL], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/brlock.h \
			  linux/slab.h linux/security.h linux/snmp.h net/xfrm.h net/dst.h \
			  net/request_sock.h], [:], [:], [
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
#endif
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <linux/sched.h>
    ])
    _LINUX_CHECK_TYPES([irqreturn_t, irq_handler_t, bool, kmem_cache_t *,
			uintptr_t, intptr_t, uchar], [:], [:], [
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
#include <net/sock.h>
#include <net/protocol.h>
])
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
])# _SOCK_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _SOCK_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_OUTPUT], [dnl
    _SOCK_CONFIG
    _SOCK_STRCONF dnl
])# _SOCK_OUTPUT
# =============================================================================

# =============================================================================
# _SOCK_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    sock_cv_config="${pkg_bld}/src/include/sys/strsock/config.h"
    sock_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    sock_cv_ldadd="${pkg_bld}/libsocket.la"
    sock_cv_ldflags="-L${pkg_bld}/.libs/"
    sock_cv_ldadd32="${pkg_bld}/lib32/libsocket.la"
    sock_cv_ldflags32="-L${pkg_bld}/lib32/.libs/"
    sock_cv_manpath="${pkg_bld}/doc/man"
    sock_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    sock_cv_modmap="${pkg_bld}/Modules.map"
    sock_cv_symver="${pkg_bld}/Module.symvers"
    sock_cv_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
])# _SOCK_CONFIG
# =============================================================================

# =============================================================================
# _SOCK_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=221
    strconf_cv_midbase=80
    if test ${streams_cv_package:-LfS} = LfS ; then
	if test ${linux_cv_minorbits:-8} -gt 8 ; then
dnl
dnl Tired of device conflicts on 2.6 kernels.
dnl
	    ((strconf_cv_majbase+=2000))
	fi
dnl
dnl Get these away from device numbers.
dnl
	((strconf_cv_midbase+=5000))
    fi
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
])# _SOCK_STRCONF
# =============================================================================

# =============================================================================
# _SOCK_
# -----------------------------------------------------------------------------
AC_DEFUN([_SOCK_], [dnl
])# _SOCK_
# =============================================================================

# =============================================================================
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.20  2008-09-10 03:49:55  brian
# - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04
#
# Revision 0.9.2.19  2008-08-11 22:27:23  brian
# - added makefile variables for modules to acinclude
#
# Revision 0.9.2.18  2008-04-28 22:33:28  brian
# - updated headers for release
#
# Revision 0.9.2.17  2007/08/14 05:16:58  brian
# - GPLv3 header update
#
# Revision 0.9.2.16  2007/03/25 19:02:17  brian
# - changes to support 2.6.20-1.2307.fc5 kernel
#
# Revision 0.9.2.15  2007/03/05 23:02:11  brian
# - checking in release changes
#
# Revision 0.9.2.14  2007/03/04 23:41:41  brian
# - additional include path
#
# Revision 0.9.2.13  2007/03/04 23:30:25  brian
# - corrected modversions directory
#
# Revision 0.9.2.12  2007/03/04 23:14:31  brian
# - better search for modversions
#
# Revision 0.9.2.11  2007/03/02 10:04:37  brian
# - updates to common build process and versions for all exported symbols
#
# Revision 0.9.2.10  2007/01/08 16:24:26  brian
# - final updates for release
#
# Revision 0.9.2.9  2006/12/23 13:07:24  brian
# - manual page and other package updates for release
#
# Revision 0.9.2.8  2006/12/18 08:14:06  brian
# - resolve device numbering
#
# Revision 0.9.2.7  2006/10/12 10:26:37  brian
# - removed redundant debug flags
#
# Revision 0.9.2.6  2006/10/03 13:53:03  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.5  2006/09/26 00:51:25  brian
# - reordered includes
#
# Revision 0.9.2.4  2006/09/25 12:22:37  brian
# - working up library
#
# Revision 0.9.2.3  2006/09/18 13:20:14  brian
# - better directory detection
#
# Revision 0.9.2.2  2006/09/18 01:15:25  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.1  2006/08/23 10:03:52  brian
# - started STREAMS Sockets package
#
# =============================================================================
# 
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
