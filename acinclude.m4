# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2006/09/25 12:24:25 $
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
# Last Modified $Date: 2006/09/25 12:24:25 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/devfs.m4])
dnl m4_include([m4/genksyms.m4])
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
m4_include([m4/inet.m4])
m4_include([m4/sctp.m4])
m4_include([m4/ss7.m4])

# =============================================================================
# AC_OS7
# -----------------------------------------------------------------------------
AC_DEFUN([AC_OS7], [dnl
    _OPENSS7_PACKAGE([OpenSS7], [OpenSS7 Master Package])
    _OS7_OPTIONS
    _AUTOPR
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
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${NSL_CPPFLAGS:+ }}${NSL_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
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
    PKG_MANPATH="${NSL_MANPATH:+${NSL_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
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
    if test ! -d "$srcdir/sctp" ; then
	with_SCTP='no'
    fi
    AC_ARG_WITH([IPERF],
		AS_HELP_STRING([--without-IPERF],
			       [do not include IPERF in master pack @<:@detected@:>@]),
		[with_IPERF="$withval"],
		[with_IPERF="$with_ALL"])
    if test ! -d "$srcdir/iperf" ; then
	with_IPERF='no'
    fi
    AC_ARG_WITH([LIS],
		AS_HELP_STRING([--with-LIS],
			       [include LIS in master pack @<:@detected@:>@]),
		[with_LIS="$withval"],
		[with_LIS="$with_ALL"])
    if test ! -d "$srcdir/LiS" ; then
	with_LIS='no'
    fi
    AC_ARG_WITH([STREAMS],
		AS_HELP_STRING([--without-STREAMS],
			       [do not include STREAMS in master pack @<:@included@:>@]),
		[with_STREAMS="$withval"],
		[with_STREAMS='yes'])
    if test ! -d "$srcdir/streams" ; then
	with_STREAMS='no'
    fi
    AC_ARG_WITH([STRCOMPAT],
		AS_HELP_STRING([--without-STRCOMPAT],
			       [do not include STRCOMPAT in master pack @<:@included@:>@]),
		[with_STRCOMPAT="$withval"],
		[with_STRCOMPAT='yes'])
    if test ! -d "$srcdir/strcompat" ; then
	with_STRCOMPAT='no'
    fi
    AC_ARG_WITH([STRUTIL],
		AS_HELP_STRING([--with-STRUTIL],
			       [include STRUTIL in master pack @<:@detected@:>@]),
		[with_STRUTIL="$withval"],
		[with_STRUTIL="$with_ALL"])
    if test ! -d "$srcdir/strutil" ; then
	with_STRUTIL='no'
    fi
    AC_ARG_WITH([STRBCM],
		AS_HELP_STRING([--with-STRBCM],
			       [include STRBCM in master pack @<:@detected@:>@]),
		[with_STRBCM="$withval"],
		[with_STRBCM="$with_ALL"])
    if test ! -d "$srcdir/strbcm" ; then
	with_STRBCM='no'
    fi
    AC_ARG_WITH([STRTTY],
		AS_HELP_STRING([--with-STRTTY],
			       [include STRTTY in master pack @<:@detected@:>@]),
		[with_STRTTY="$withval"],
		[with_STRTTY="$with_ALL"])
    if test ! -d "$srcdir/strtty" ; then
	with_STRTTY='no'
    fi
    AC_ARG_WITH([STRXNS],
		AS_HELP_STRING([--without-STRXNS],
			       [do not include STRXNS in master pack @<:@included@:>@]),
		[with_STRXNS="$withval"],
		[with_STRXNS='yes'])
    if test ! -d "$srcdir/strxns" ; then
	with_STRXNS='no'
    fi
    AC_ARG_WITH([STRXNET],
		AS_HELP_STRING([--without-STRXNET],
			       [do not include STRXNET in master pack @<:@included@:>@]),
		[with_STRXNET="$withval"],
		[with_STRXNET='yes'])
    if test ! -d "$srcdir/strxnet" ; then
	with_STRXNET='no'
    fi
    AC_ARG_WITH([STRNSL],
		AS_HELP_STRING([--without-STRNSL],
			       [do not include STRNSL in master pack @<:@included@:>@]),
		[with_STRNSL="$withval"],
		[with_STRNSL='yes'])
    if test ! -d "$srcdir/strnsl" ; then
	with_STRNSL='no'
    fi
    AC_ARG_WITH([STRSOCK],
		AS_HELP_STRING([--without-STRSOCK],
			       [do not include STRSOCK in master pack @<:@included@:>@]),
		[with_STRSOCK="$withval"],
		[with_STRSOCK="$with_ALL"])
    if test ! -d "$srcdir/strsock" ; then
	with_STRSOCK='no'
    fi
    AC_ARG_WITH([STRINET],
		AS_HELP_STRING([--without-STRINET],
			       [do not include STRINET in master pack @<:@included@:>@]),
		[with_STRINET="$withval"],
		[with_STRINET='yes'])
    if test ! -d "$srcdir/strinet" ; then
	with_STRINET='no'
    fi
    AC_ARG_WITH([STRSCTP],
		AS_HELP_STRING([--without-STRSCTP],
			       [do not include STRSCTP in master pack @<:@included@:>@]),
		[with_STRSCTP="$withval"],
		[with_STRSCTP='yes'])
    if test ! -d "$srcdir/strsctp" ; then
	with_STRSCTP='no'
    fi
    AC_ARG_WITH([STRISO],
		AS_HELP_STRING([--with-STRISO],
			       [include STRISO in master pack @<:@detected@:>@]),
		[with_STRISO="$withval"],
		[with_STRISO="$with_ALL"])
    if test ! -d "$srcdir/striso" ; then
	with_STRISO='no'
    fi
    AC_ARG_WITH([NETPERF],
		AS_HELP_STRING([--without-NETPERF],
			       [do not include NETPERF in master pack @<:@included@:>@]),
		[with_NETPERF="$withval"],
		[with_NETPERF='yes'])
    if test ! -d "$srcdir/netperf" ; then
	with_NETPERF='no'
    fi
    AC_ARG_WITH([STACKS],
		AS_HELP_STRING([--without-STACKS],
			       [do not include STACKS in master pack @<:@included@:>@]),
		[with_STACKS="$withval"],
		[with_STACKS='yes'])
    if test ! -d "$srcdir/stacks" ; then
	with_STACKS='no'
    fi
])# _OS7_OPTIONS
# =============================================================================

# =============================================================================
# _OS7_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_OS7_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
dnl _GENKSYMS
    _OS7_CONFIG_KERNEL
    with_lis='yes'
    with_lfs='yes'
    if test :"${with_LIS:-no}" = :no ; then
	with_lis='no'
    fi
    if test :"${with_STREAMS:-yes}" = :no ; then
	with_lfs='no'
    fi
dnl
dnl Make this darn specific
dnl
    if test :"${with_lis}" = :yes ; then
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_lis --with-lis\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-lis'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--with-lis"
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_lis --without-lis\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-lis'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-lis"
    fi
    if test :"${with_lfs}" = :yes ; then
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_with_lfs --with-lfs\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--with-lfs'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--with-lfs"
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_lfs --without-lfs\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-lfs'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-lfs"
    fi
    if test :"${with_LIS:-no}" != :no -o :"${with_STREAMS:-yes}" != :no ; then
	_LINUX_STREAMS
    fi
dnl
dnl Make this darn specific too
dnl
    if test :"${with_STRCOMPAT:-yes}" != :no ; then
	_STRCOMP
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_compat --without-compat\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-compat'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-compat"
    fi
    if test :"${with_STRUTIL:-yes}" != :no ; then
	:
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_util --without-util\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-util'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-util"
    fi
    if test :"${with_STRBCM:-yes}" != :no ; then
	:
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_bcm --without-bcm\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-bcm'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-bcm"
    fi
    if test :"${with_STRTTY:-yes}" != :no ; then
	:
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_tty --without-tty\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-tty'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-tty"
    fi
    if test :"${with_STRXNS:-yes}" != :no ; then
	_XNS
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_xns --without-xns\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-xns'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-xns"
    fi
    if test :"${with_STRXNET:-yes}" != :no ; then
	_XTI
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_xti --without-xti\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-xti'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-xti"
    fi
    if test :"${with_STRNSL:-yes}" != :no ; then
	_NSL
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_nsl --without-nsl\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-nsl'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-nsl"
    fi
    if test :"${with_STRSOCK:-yes}" != :no ; then
	:
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_sock --without-sock\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-sock'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-sock"
    fi
    if test :"${with_STRINET:-yes}" != :no ; then
	_INET
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_inet --without-inet\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-inet'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-inet"
    fi
    if test :"${with_STRSCTP:-yes}" != :no ; then
	_SCTP
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_sctp --without-sctp\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-sctp'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-sctp"
    fi
    if test :"${with_STRISO:-yes}" != :no ; then
	:
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_iso --without-iso\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-iso'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-iso"
    fi
    if test :"${with_STACKS:-yes}" != :no ; then
	_SS7
    else
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS}${PACKAGE_RPMOPTIONS:+ }--define \"_without_ss7 --without-ss7\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS}${PACKAGE_DEBOPTIONS:+ }'--without-ss7'"
	ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--without-ss7"
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
    if test :${with_STRUTIL:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strutil])
    fi
    if test :${with_STRBCM:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strbcm])
    fi
    if test :${with_STRTTY:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strtty])
    fi
    if test :${with_STRXNS:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strxns])
    fi
    if test :${with_STRXNET:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strxnet])
    fi
    if test :${with_STRNSL:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strnsl])
    fi
    if test :${with_STRSOCK:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strsock])
    fi
    if test :${with_STRINET:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strinet])
    fi
    if test :${with_STRSCTP:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([strsctp])
    fi
    if test :${with_STRISO:-yes} = :yes ; then
	AC_CONFIG_SUBDIRS([striso])
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
