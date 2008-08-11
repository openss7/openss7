# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2008-08-11 22:27:20 $
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
# Last Modified $Date: 2008-08-11 22:27:20 $ by $Author: brian $
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
# AC_BCM
# -----------------------------------------------------------------------------
AC_DEFUN([AC_BCM], [dnl
    _OPENSS7_PACKAGE([BCM], [OpenSS7 STREAMS Binary Compatibility Modules])
    _BCM_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/strbcm-core.postinst
		     debian/strbcm-core.postrm
		     debian/strbcm-core.preinst
		     debian/strbcm-core.prerm
		     debian/strbcm-devel.preinst
		     debian/strbcm-dev.postinst
		     debian/strbcm-dev.preinst
		     debian/strbcm-dev.prerm
		     debian/strbcm-doc.postinst
		     debian/strbcm-doc.preinst
		     debian/strbcm-doc.prerm
		     debian/strbcm-init.postinst
		     debian/strbcm-init.postrm
		     debian/strbcm-init.preinst
		     debian/strbcm-init.prerm
		     debian/strbcm-lib.preinst
		     debian/strbcm-source.preinst
		     debian/strbcm-util.preinst
		     src/util/modutils/strbcm
		     src/util/sysconfig/strbcm
		     src/include/sys/strbcm/version.h
		     Module.mkvars])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _BCM_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
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
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _BCM_OUTPUT
    _AUTOTEST
])# AC_BCM
# =============================================================================

# =============================================================================
# _BCM_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_OPTIONS], [dnl
])# _BCM_OPTIONS
# =============================================================================

# =============================================================================
# _BCM_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LINUX_STREAMS
    _STRCOMP
    _BCM_SETUP_MODULE
    _BCM_CONFIG_KERNEL
])# _BCM_SETUP
# =============================================================================

# =============================================================================
# _BCM_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_SETUP_MODULE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([BCM_CONFIG_MODULE], [], [When defined, BCM is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([BCM_CONFIG], [], [When defined, BCM is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([BCM_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([BCM_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _BCM_SETUP_MODULE
# =============================================================================

# =============================================================================
# _BCM_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# Later we should have some checks here for things like OSS, ALS, Zaptel, etc.
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_CONFIG_KERNEL], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/slab.h linux/cdev.h \
			  linux/hardirq.h linux/cpumask.h linux/kref.h linux/security.h \
			  asm/uaccess.h linux/kthread.h linux/compat.h linux/ioctl32.h \
			  asm/ioctl32.h linux/syscalls.h linux/rwsem.h linux/smp_lock.h \
			  linux/devfs_fs_kernel.h linux/compile.h linux/utsrelease.h], [:], [:], [
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
])
    AH_TEMPLATE([kmem_cachep_t], [This kmem_cache_t is deprecated in recent
	2.6.20 kernels.  When it is deprecated, define this to struct
	kmem_cache *.])
    if test :"${linux_cv_type_kmem_cache_t_p:-no}" = :no ; then
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [struct kmem_cache *])
    else
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [kmem_cache_t *])
    fi
])# _BCM_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _BCM_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_OUTPUT], [dnl
    _BCM_CONFIG
    _BCM_STRCONF dnl
])# _BCM_OUTPUT
# =============================================================================

# =============================================================================
# _BCM_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    bcm_cv_config="${pkg_bld}/src/include/sys/strbcm/config.h"
    bcm_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    bcm_cv_ldadd= # "${pkg_bld}/libbcm.la"
    bcm_cv_ldflags= # "${pkg_bld}/lib32/libbcm.la"
    bcm_cv_ldadd32= # "-L${pkg_bld}/.libs/"
    bcm_cv_ldflags32= # "${pkg_bld}/lib32/.libs/"
    bcm_cv_manpath="${pkg_bld}/doc/man"
    bcm_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    bcm_cv_modmap="${pkg_bld}/Modules.map"
    bcm_cv_symver="${pkg_bld}/Module.symvers"
    bcm_cv_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
])# _BCM_CONFIG
# =============================================================================

# =============================================================================
# _BCM_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=252
    strconf_cv_midbase=30
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
])# _BCM_STRCONF
# =============================================================================

# =============================================================================
# _BCM_
# -----------------------------------------------------------------------------
AC_DEFUN([_BCM_], [dnl
])# _BCM_
# =============================================================================

# =============================================================================
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.16  2008-08-11 22:27:20  brian
# - added makefile variables for modules to acinclude
#
# Revision 0.9.2.15  2008-04-28 17:30:58  brian
# - updates for release
#
# Revision 0.9.2.14  2007/08/14 02:46:14  brian
# - GPLv3 headers
#
# Revision 0.9.2.13  2007/03/25 19:00:47  brian
# - changes to support 2.6.20-1.2307.fc5 kernel
#
# Revision 0.9.2.12  2007/03/05 23:01:50  brian
# - checking in release changes
#
# Revision 0.9.2.11  2007/03/04 23:41:33  brian
# - additional include path
#
# Revision 0.9.2.10  2007/03/04 23:29:59  brian
# - corrected modversions directory
#
# Revision 0.9.2.9  2007/03/04 23:14:22  brian
# - better search for modversions
#
# Revision 0.9.2.8  2007/03/02 10:03:52  brian
# - updates to common build process and versions for all exported symbols
#
# Revision 0.9.2.7  2006/12/18 08:28:53  brian
# - resolve device numbering
#
# Revision 0.9.2.6  2006/10/12 10:20:40  brian
# - removed redundant debug flags
#
# Revision 0.9.2.5  2006/09/18 13:20:08  brian
# - better directory detection
#
# Revision 0.9.2.4  2006/07/25 06:38:58  brian
# - expanded minor device numbers and optimization and locking corrections
#
# Revision 0.9.2.3  2006/05/08 03:12:27  brian
# - added module id base to strconf
#
# Revision 0.9.2.2  2006/03/21 13:24:13  brian
# - added problem report checks
#
# Revision 0.9.2.1  2006/03/08 11:42:09  brian
# - setting up package
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
