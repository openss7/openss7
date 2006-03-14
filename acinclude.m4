# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2006/03/13 23:59:52 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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
# Last Modified $Date: 2006/03/13 23:59:52 $ by $Author: brian $
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
m4_include([m4/xopen.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/inet.m4])
m4_include([m4/sctp.m4])

# =============================================================================
# AC_OS7
# -----------------------------------------------------------------------------
AC_DEFUN([AC_OS7], [dnl
    _OPENSS7_PACKAGE([OpenSS7], [OpenSS7 Master Package])
    _OS7_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
dnl AC_CONFIG_FILES([debian/openss7-core.postinst
dnl		     debian/openss7-core.postrm
dnl		     debian/openss7-core.preinst
dnl		     debian/openss7-core.prerm
dnl		     debian/openss7-devel.preinst
dnl		     debian/openss7-dev.postinst
dnl		     debian/openss7-dev.preinst
dnl		     debian/openss7-dev.prerm
dnl		     debian/openss7-doc.postinst
dnl		     debian/openss7-doc.preinst
dnl		     debian/openss7-doc.prerm
dnl		     debian/openss7-init.postinst
dnl		     debian/openss7-init.postrm
dnl		     debian/openss7-init.preinst
dnl		     debian/openss7-init.prerm
dnl		     debian/openss7-lib.preinst
dnl		     debian/openss7-source.preinst
dnl		     debian/openss7-util.preinst
dnl		     src/util/modutils/openss7
dnl		     src/include/sys/openss7/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _OS7_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/$(STRCONF_CONFIG)'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/src/include -I$(top_srcdir)/src/include'
    if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
    fi
dnl PKG_MODFLAGS='$(STREAMS_MODFLAGS)'
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
    PKG_MANPATH="${XNS_MANPATH:+${XNS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XTI_MANPATH:+${XTI_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _OS7_OUTPUT
dnl _AUTOTEST
])# AC_OS7
# =============================================================================

# =============================================================================
# _OS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_OPTIONS], [dnl
    AC_ARG_WITH([ALL],
		AS_HELP_STRING([--with-ALL],
			       [include all packages in master pack @<:@disabled@:>@]),
		[with_ALL="$withval"],
		[with_ALL=''])
    AC_ARG_WITH([SCTP],
		AS_HELP_STRING([--without-SCTP],
			       [do not include SCTP in master pack @<:@detected@:>@]),
		[with_SCTP="$withval"],
		[with_SCTP="$with_ALL"])
    AC_ARG_WITH([IPERF],
		AS_HELP_STRING([--without-IPERF],
			       [do not include IPERF in master pack @<:@detected@:>@]),
		[with_IPERF="$withval"],
		[with_IPERF="$with_ALL"])
    AC_ARG_WITH([LIS],
		AS_HELP_STRING([--with-LIS],
			       [include LIS in master pack @<:@detected@:>@]),
		[with_LIS="$withval"],
		[with_LIS="$with_ALL"])
    AC_ARG_WITH([STREAMS],
		AS_HELP_STRING([--without-STREAMS],
			       [do not include STREAMS in master pack @<:@included@:>@]),
		[with_STREAMS="$withval"],
		[with_STREAMS='yes'])
    AC_ARG_WITH([STRCOMPAT],
		AS_HELP_STRING([--without-STRCOMPAT],
			       [do not include STRCOMPAT in master pack @<:@included@:>@]),
		[with_STRCOMPAT="$withval"],
		[with_STRCOMPAT='yes'])
    AC_ARG_WITH([STRBCM],
		AS_HELP_STRING([--with-STRBCM],
			       [include STRBCM in master pack @<:@detected@:>@]),
		[with_STRBCM="$withval"],
		[with_STRBCM='yes'])
    AC_ARG_WITH([STRUTIL],
		AS_HELP_STRING([--with-STRUTIL],
			       [include STRUTIL in master pack @<:@detected@:>@]),
		[with_STRUTIL="$withval"],
		[with_STRUTIL='no'])
    AC_ARG_WITH([STRXNS],
		AS_HELP_STRING([--without-STRXNS],
			       [do not include STRXNS in master pack @<:@included@:>@]),
		[with_STRXNS="$withval"],
		[with_STRXNS='yes'])
    AC_ARG_WITH([STRXNET],
		AS_HELP_STRING([--without-STRXNET],
			       [do not include STRXNET in master pack @<:@included@:>@]),
		[with_STRXNET="$withval"],
		[with_STRXNET='yes'])
    AC_ARG_WITH([STRINET],
		AS_HELP_STRING([--without-STRINET],
			       [do not include STRINET in master pack @<:@included@:>@]),
		[with_STRINET="$withval"],
		[with_STRINET='yes'])
    AC_ARG_WITH([STRSCTP],
		AS_HELP_STRING([--without-STRSCTP],
			       [do not include STRSCTP in master pack @<:@included@:>@]),
		[with_STRSCTP="$withval"],
		[with_STRSCTP='yes'])
    AC_ARG_WITH([NETPERF],
		AS_HELP_STRING([--without-NETPERF],
			       [do not include NETPERF in master pack @<:@included@:>@]),
		[with_NETPERF="$withval"],
		[with_NETPERF='yes'])
    AC_ARG_WITH([STACKS],
		AS_HELP_STRING([--without-STACKS],
			       [do not include STACKS in master pack @<:@included@:>@]),
		[with_STACKS="$withval"],
		[with_STACKS='yes'])
])# _OS7_OPTIONS
# =============================================================================

# =============================================================================
# _OS7_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    _OS7_CONFIG_KERNEL
    _LINUX_STREAMS
    if test ":${with_STRCOMPAT:-no}" != ":no" ; then
    _STRCOMP
    fi
    _XOPEN
    if test ":${with_XNS:-no}" != ":no" ; then
    _XNS
    fi
    if test ":${with_XNET:-no}" != ":no" ; then
    _XTI
    fi
    if test ":${with_INET:-no}" != ":no" ; then
    _INET
    fi
    if test ":${with_SCTP:-no}" != ":no" ; then
    _SCTP
    fi
])# _OS7_SETUP
# =============================================================================

# =============================================================================
# _OS7_CONFIG_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_CONFIG_KERNEL], [dnl
])# _OS7_CONFIG_KERNEL
# =============================================================================

# =============================================================================
# _OS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_OUTPUT], [dnl
    if test :${with_SCTP:-auto} = :auto ; then
	if test :${linux_cv_k_ko_modules:-yes} = :yes ; then
	    with_SCTP='no'
	else
	    with_SCTP='yes'
	fi
    fi
    if test :${with_IPERF:-auto} = :auto ; then
	if test :${linux_cv_k_ko_modules:-yes} = :yes ; then
	    with_IPERF='no'
	else
	    with_IPERF='yes'
	fi
    fi
    if test :${with_LIS:-auto} = :auto ; then
	with_LIS='no'
    fi
    if test :${with_STRUTIL:-auto} = :auto ; then
	with_STRUTIL='no'
    fi
    if test :${with_STRBCM:-auto} = :auto ; then
	with_STRBCM='no'
    fi
    if test :${with_SCTP:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([sctp])
    fi
    if test :${with_IPERF:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([iperf])
    fi
    if test :${with_LIS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([LiS])
    fi
    if test :${with_STREAMS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([streams])
    fi
    if test :${with_STRCOMPAT:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strcompat])
    fi
    if test :${with_STRBCM:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strbcm])
    fi
    if test :${with_STRUTIL:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strutil])
    fi
    if test :${with_STRXNS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strxns])
    fi
    if test :${with_STRXNET:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strxnet])
    fi
    if test :${with_STRINET:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strinet])
    fi
    if test :${with_STRSCTP:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strsctp])
    fi
    if test :${with_NETPERF:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([netperf])
    fi
    if test :${with_STACKS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([stacks])
    fi
    AC_CACHE_CHECK([for master srcdir],[os7_cv_master_srcdir],[dnl
	os7_cv_master_srcdir=`(cd $srcdir; pwd)`
    ])
    AC_CACHE_CHECK([for master builddir],[os7_cv_master_builddir],[dnl
	os7_cv_master_builddir=`pwd`
    ])
    master_srcdir="$os7_cv_master_srcdir"
    master_builddir="$os7_cv_master_builddir"
    AC_SUBST([master_srcdir])
    AC_SUBST([master_builddir])
])# _OS7_OUTPUT
# =============================================================================

# =============================================================================
# _OS7_
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_], [dnl
])# _OS7_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
