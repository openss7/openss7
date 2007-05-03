# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2007/05/03 22:24:39 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2007/05/03 22:24:39 $ by $Author: brian $
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
m4_include([m4/xopen.m4])
m4_include([m4/xns.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_XNET
# -----------------------------------------------------------------------------
AC_DEFUN([AC_XNET], [dnl
    _OPENSS7_PACKAGE([XNET], [OpenSS7 XTI/TLI Networking])
    _XNET_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strxnet-core.postinst
		     debian/strxnet-core.postrm
		     debian/strxnet-core.preinst
		     debian/strxnet-core.prerm
		     debian/strxnet-devel.preinst
		     debian/strxnet-dev.postinst
		     debian/strxnet-dev.preinst
		     debian/strxnet-dev.prerm
		     debian/strxnet-doc.postinst
		     debian/strxnet-doc.preinst
		     debian/strxnet-doc.prerm
		     debian/strxnet-init.postinst
		     debian/strxnet-init.postrm
		     debian/strxnet-init.preinst
		     debian/strxnet-init.prerm
		     debian/strxnet-lib.preinst
		     debian/strxnet-source.preinst
		     debian/strxnet-util.preinst
		     src/util/modutils/strxnet
		     src/include/sys/strxnet/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _XNET_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
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
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _XNET_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_XNET
# =============================================================================

# =============================================================================
# _XNET_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_OPTIONS], [dnl
    AC_ARG_ENABLE([xti-servtype],
	AS_HELP_STRING([--disable-xti-servtype],
	    [disable xnet library checks for service type.
	     @<:@default=enabled@:>@]),
	[enable_xti_servtype="$enableval"],
	[enable_xti_servtype='yes'])
    AC_CACHE_CHECK([for XTI service type checks], [xti_servtype], [dnl
	xti_servtype="${enable_xti_servtype:-yes}"])
    if test ${xti_servtype:-yes} != yes ; then
	AC_DEFINE_UNQUOTED([CONFIG_XTI_IS_TYPELESS], [1], [Define when the XTI
	    library is not to check service types.  This is necessary when
	    T_COTS semantics are expected to be applied to T_CLTS providers.
	    When defined, the XTI library lets the underlying TPI driver
	    determine whether T_COTS/T_COTS_ORD primitives are supported or
	    not.])
    fi
    AC_ARG_ENABLE([xti-states],
	AS_HELP_STRING([--disable-xti-states],
	    [disable xnet library checks for state.
	     @<:@default=enabled@:>@]),
	[enable_xti_states="$enableval"],
	[enable_xti_states='yes'])
    AC_CACHE_CHECK([for XTI state checks], [xti_states], [dnl
	xti_states="${enable_xti_states:-yes}"])
    if test ${xti_states:-yes} != yes ; then
	AC_DEFINE_UNQUOTED([CONFIG_XTI_IS_STATELESS], [1], [Define when the
	    XTI library is not to check states.  This is necessary when T_COTS
	    semantics are expected to be applied to T_CLTS providers and
	    t_connect() is to be called on an unbound T_CLTS Stream.  When
	    defined, the XTI library lets the underlying TPI driver determine
	    whether the primitive is issued out of state or not.])
    fi
])# _XNET_OPTIONS
# =============================================================================

# =============================================================================
# _XNET_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LINUX_STREAMS
    _STRCOMP
    with_tli='yes'
    with_xnet='yes'
    with_xnsl='no'
    with_sock='no'
    _XOPEN
    _XNS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _XNET_SETUP_MODULE
    _XNET_CONFIG_KERNEL
])# _XNET_SETUP
# =============================================================================

# =============================================================================
# _XNET_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_SETUP_MODULE], [dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([XNET_CONFIG_MODULE], [], [When defined, XNET is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([XNET_CONFIG], [], [When defined, XNET is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([XNET_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([XNET_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])
# =============================================================================

# =============================================================================
# _XNET_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuration checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_CONFIG_KERNEL], [dnl
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
    _LINUX_CHECK_TYPES([irqreturn_t, irq_handler_t, bool, kmem_cache_t *], [:], [:], [
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
])# _XNET_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _XNET_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_OUTPUT], [dnl
    _XNET_CONFIG
    _XNET_STRCONF dnl
])# _XNET_OUTPUT
# =============================================================================

# =============================================================================
# _XNET_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    xti_cv_config="${pkg_bld}/src/include/sys/strxnet/config.h"
    xti_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    xti_cv_ldadd="${pkg_bld}/libxnet.la"
    xti_cv_ldflags="-L${pkg_bld}/.libs/"
    xti_cv_ldadd32="${pkg_bld}/lib32/libxnet.la"
    xti_cv_ldflags32="-L${pkg_bld}/lib32/.libs/"
    xti_cv_manpath="${pkg_bld}/doc/man"
    xti_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    xti_cv_modmap="${pkg_bld}/Modules.map"
    xti_cv_symver="${pkg_bld}/Module.symvers"
    xti_cv_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
])# _XNET_CONFIG
# =============================================================================

# =============================================================================
# _XNET_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=220
    strconf_cv_midbase=60
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
    _STRCONF dnl
])# _XNET_STRCONF
# =============================================================================

# =============================================================================
# _XNET_
# -----------------------------------------------------------------------------
AC_DEFUN([_XNET_], [dnl
])# _XNET_
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
