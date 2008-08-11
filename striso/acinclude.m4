# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2008-08-11 22:27:22 $
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
# Last Modified $Date: 2008-08-11 22:27:22 $ by $Author: brian $
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
dnl m4_include([m4/xopen.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/nsl.m4])
m4_include([m4/sock.m4])
m4_include([m4/inet.m4])
m4_include([m4/sctp.m4])
m4_include([m4/chan.m4])
m4_include([m4/x25.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_ISO
# -----------------------------------------------------------------------------
AC_DEFUN([AC_ISO], [dnl
    _OPENSS7_PACKAGE([ISO], [OpenSS7 ISO Stack])
    _ISO_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/striso-core.postinst
		     debian/striso-core.postrm
		     debian/striso-core.preinst
		     debian/striso-core.prerm
		     debian/striso-devel.preinst
		     debian/striso-dev.postinst
		     debian/striso-dev.preinst
		     debian/striso-dev.prerm
		     debian/striso-doc.postinst
		     debian/striso-doc.preinst
		     debian/striso-doc.prerm
		     debian/striso-init.postinst
		     debian/striso-init.postrm
		     debian/striso-init.preinst
		     debian/striso-init.prerm
		     debian/striso-lib.preinst
		     debian/striso-source.preinst
		     debian/striso-util.preinst
		     src/util/modutils/striso
		     src/include/sys/striso/version.h
		     Module.mkvars])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _ISO_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${X25_CPPFLAGS:+ }}${X25_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${CHAN_CPPFLAGS:+ }}${CHAN_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SCTP_CPPFLAGS:+ }}${SCTP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${INET_CPPFLAGS:+ }}${INET_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${SOCK_CPPFLAGS:+ }}${SOCK_CPPFLAGS}"
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
    PKG_MANPATH="${SOCK_MANPATH:+${SOCK_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${INET_MANPATH:+${INET_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${SCTP_MANPATH:+${SCTP_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${CHAN_MANPATH:+${CHAN_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${X25_MANPATH:+${X25_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _ISO_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_ISO
# =============================================================================

# =============================================================================
# _ISO_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_OPTIONS], [dnl
    _ISO_CHECK_ISO
    _ISO_CHECK_SCTP
])# _ISO_OPTIONS
# =============================================================================

# =============================================================================
# _ISO_CHECK_ISO
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_CHECK_ISO], [dnl
])# _ISO_CHECK_ISO
# =============================================================================

# =============================================================================
# _ISO_CHECK_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_CHECK_SCTP], [dnl
])# _ISO_CHECK_SCTP
# =============================================================================

# =============================================================================
# _ISO_OTHER_SCTP
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_OTHER_SCTP], [dnl
])# _ISO_OTHER_SCTP
# =============================================================================

# =============================================================================
# _ISO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    _LINUX_STREAMS
    _STRCOMP
dnl with_iso='yes'
dnl _XOPEN
    _XNS
    _XTI
    _NSL
    _SOCK
    _INET
    _SCTP
    _CHAN
    _X25
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _ISO_OTHER_SCTP
    _ISO_SETUP_MODULE
    _ISO_CONFIG_KERNEL
])# _ISO_SETUP
# =============================================================================

# =============================================================================
# _ISO_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_SETUP_MODULE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([ISO_CONFIG_MODULE], [], [When defined, ISO is
			    being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([ISO_CONFIG], [], [When defined, ISO is being
			    compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([ISO_CONFIG_MODULE], [test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([ISO_CONFIG], [test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _ISO_SETUP_MODULE
# =============================================================================

# =============================================================================
# _ISO_CONFIG_KERNEL
# -----------------------------------------------------------------------------
# These are a bunch of kernel configuration checks primarily in support of 2.5
# and 2.6 kernels.
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_CONFIG_KERNEL], [dnl
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
			pm_message_t], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/types.h>
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
#include <linux/pm.h>
])
    AH_TEMPLATE([kmem_cachep_t], [This kmem_cache_t is deprecated in recent
	2.6.20 kernels.  When it is deprecated, define this to struct
	kmem_cache *.])
    if test :"${linux_cv_type_kmem_cache_t_p:-no}" = :no ; then
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [struct kmem_cache *])
    else
	AC_DEFINE_UNQUOTED([kmem_cachep_t], [kmem_cache_t *])
    fi
#    AC_SUBST([EXPOSED_SYMBOLS])
#    EXPOSED_SYMBOLS="\
#	mtp_n_uderror_ind \
#	mtp_n_unitdata_ind \
#	mtp_n_unitdata_req \
#	sccp_n_uderror_ind \
#	sccp_n_unitdata_ind \
#	sccp_n_unitdata_req \
#	sctp_data_ind \
#	sctp_data_req \
#	sctp_datack_ind \
#	sctp_datack_req \
#	sctp_exdata_ind \
#	sctp_exdata_req"
])# _ISO_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _ISO_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_OUTPUT], [dnl
    _ISO_CONFIG
    _ISO_STRCONF dnl
])# _ISO_OUTPUT
# =============================================================================

# =============================================================================
# _ISO_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_CONFIG], [dnl
    pkg_src=`(cd $srcdir ; /bin/pwd)`
    pkg_bld=`(cd . ; /bin/pwd)`
    iso_cv_config="${pkg_bld}/src/include/sys/striso/config.h"
    iso_cv_includes="${pkg_bld}/include ${pkg_bld}/src/include ${pkg_src}/src/include"
    iso_cv_ldadd= # "${pkg_bld}/libiso.la"
    iso_cv_ldflags= # "${pkg_bld}/lib32/libiso.la"
    iso_cv_ldadd32= # "-L${pkg_bld}/.libs/"
    iso_cv_ldflags32= # "${pkg_bld}/lib32/.libs/"
    iso_cv_manpath="${pkg_bld}/doc/man"
    iso_cv_modversions="${pkg_bld}/include/sys/${PACKAGE}/modversions.h"
    iso_cv_modmap="${pkg_bld}/Modules.map"
    iso_cv_symver="${pkg_bld}/Module.symvers"
    iso_cv_version="${PACKAGE_RPMEPOCH}:${VERSION}-${PACKAGE_RPMRELEASE}"
])# _ISO_CONFIG
# =============================================================================

# =============================================================================
# _ISO_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_STRCONF], [dnl
    strconf_cv_stem='lis.conf'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=206
    strconf_cv_midbase=120
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
])# _ISO_STRCONF
# =============================================================================

# =============================================================================
# _ISO_
# -----------------------------------------------------------------------------
AC_DEFUN([_ISO_], [dnl
])# _ISO_
# =============================================================================

# =============================================================================
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.24  2008-08-11 22:27:22  brian
# - added makefile variables for modules to acinclude
#
# Revision 0.9.2.23  2008-05-03 21:22:31  brian
# - updates for release
#
# Revision 0.9.2.22  2008-05-03 13:23:33  brian
# - added strx25 sub-package and package updates
#
# Revision 0.9.2.21  2008-04-29 00:01:52  brian
# - updated headers for release
#
# Revision 0.9.2.20  2007/08/14 07:04:37  brian
# - GNUv3 header update
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
