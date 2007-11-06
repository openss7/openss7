# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: openss7.m4,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2007/10/18 06:12:53 $
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
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/10/18 06:12:53 $ by $Author: brian $
#
# =============================================================================

dnl
dnl Handle differences between autoconf 2.59 and 2.60, 2.61.  The reason for
dnl some backward compatibility to 2.59 is cause many still recent Linux
dnl distros ship with 2.59.  (Although Debian and Ubuntu are at 2.60.)
dnl
m4_ifndef([AC_USE_SYSTEM_EXTENSIONS],
	  [m4_define([AC_USE_SYSTEM_EXTENSIONS], [AC_GNU_SOURCE])])

m4_ifdef([AC_COPYRIGHT],
	 [m4_define([AC_NOTICE], [AC_COPYRIGHT([$1]) AH_TOP([/* 
]m4_bpatsubst([[$1]], [^], [ * ])[
 */])])],
	 [m4_define([AC_NOTICE], [AH_TOP([/* 
]m4_bpatsubst([[$1]], [^], [ * ])[
 */])])])

# =============================================================================
# _OPENSS7_PACKAGE([SHORT-TITLE], [LONG-TITLE])
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_PACKAGE], [dnl
    PACKAGE_TITLE='$2'
    AC_SUBST([PACKAGE_TITLE])dnl
    m4_ifndef([AC_PACKAGE_TITLE], [m4_define([AC_PACKAGE_TITLE], [$2])])
    PACKAGE_SHORTTITLE='$1'
    AC_SUBST([PACKAGE_SHORTTITLE])dnl
    m4_ifndef([AC_PACKAGE_SHORTTITLE], [m4_define([AC_PACKAGE_SHORTTITLE], [$1])])
    upper='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
    lower='abcdefghijklmnopqrstuvwxyz'
    PACKAGE_LCNAME=`echo "$PACKAGE_NAME" | sed -e y/$upper/$lower/`
    AC_SUBST([PACKAGE_LCNAME])dnl
    m4_ifndef([AC_PACKAGE_LCNAME], [m4_define([AC_PACKAGE_LCNAME], [m4_tolower(AC_PACKAGE_NAME)])])
    PACKAGE_UCNAME=`echo "$PACKAGE_NAME" | sed -e y/$lower/$upper/`
    AC_SUBST([PACKAGE_UCNAME])dnl
    m4_ifndef([AC_PACKAGE_UCNAME], [m4_define([AC_PACKAGE_UCNAME], [m4_toupper(AC_PACKAGE_NAME)])])
    unset upper lower
    PACKAGE_DATE=`date -I`
    AC_SUBST([PACKAGE_DATE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_DATE], ["$PACKAGE_DATE"], [The package release date.])
    PKGINCL="include/sys/${PACKAGE_NAME}"
    AC_SUBST([PKGINCL])dnl
    AC_NOTICE(
[
=============================================================================
BEGINNING OF SEPARATE COPYRIGHT MATERIAL
=============================================================================

Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 3 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

-----------------------------------------------------------------------------

U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
behalf of the U.S. Government ("Government"), the following provisions apply
to you.  If the Software is supplied by the Department of Defense ("DoD"), it
is classified as "Commercial Computer Software" under paragraph 252.227-7014
of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
successor regulations) and the Government is acquiring only the license rights
granted herein (the license rights customarily provided to non-Government
users).  If the Software is supplied to any unit or agency of the Government
other than DoD, it is classified as "Restricted Computer Software" and the
Government's rights in the Software are defined in paragraph 52.227-19 of the
Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
(or any successor regulations).

-----------------------------------------------------------------------------

Commercial licensing and support of this software is available from OpenSS7
Corporation at a fee.  See http://www.openss7.com/

=============================================================================

])dnl

    _OPENSS7_OPTIONS
    _OPENSS7_CACHE
    _OPENSS7_OPTIONS_CFLAGS
    _OPENSS7_MISSING
    AC_SUBST([cross_compiling])dnl
])# _OPENSS7_PACKAGE
# =============================================================================

# =============================================================================
# _OPENSS7_DIRCHANGE
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_DIRCHANGE], [dnl
    ac_default_prefix='/usr'
dnl
dnl Find the real root of the install tree
dnl
    if test :"$prefix" = :NONE ; then
	newprefix="$ac_default_prefix"
    else
	newprefix="$prefix"
    fi
    rootdir=`echo $newprefix | sed -e 's|/local$||;s|/usr$||'`
dnl
dnl Need to adjust directories if default.  This also handles subtitutional
dnl differences between autoconf 2.59 and autoconf 2.60 and 2.61.
dnl
    if test :"$sysconfdir" = :'${prefix}/etc' ; then sysconfdir='${rootdir}/etc' ; fi
    if test :"$localstatedir" = :'${prefix}/var' ; then localstatedir='${rootdir}/var' ; fi
    if test :"$sharedstatedir" = :'${prefix}/com' ; then localstatedir='${rootdir}/com' ; fi
    if test :"${newprefix#$rootdir}" = : ; then
	if test :"${datarootdir+set}" != :set ; then datarootdir='${prefix}/usr/share' ; fi
	if test :"$datarootdir" = :'${prefix}/share' ; then datarootdir='${prefix}/usr/share' ; fi
	if test :"$infodir" = :'${prefix}/info' ; then infodir='${datarootdir}/info' ; fi
	if test :"$mandir" = :'${prefix}/man' ; then mandir='${datarootdir}/man' ; fi
    fi
    if test :"${newprefix#$rootdir}" = :/usr ; then
	if test :"${datarootdir+set}" != :set ; then datarootdir='${prefix}/share' ; fi
	if test :"$infodir" = :'${prefix}/info' ; then infodir='${datarootdir}/info' ; fi
	if test :"$mandir" = :'${prefix}/man' ; then mandir='${datarootdir}/man' ; fi
    fi
    AC_SUBST([datarootdir])
    if test :"${datadir+set}" != :set ; then datadir='${datarootdir}' ; fi
    AC_SUBST([datadir])
    if test :"${localedir+set}" != :set ; then localedir='${datarootdir}/locale' ; fi
    AC_SUBST([localedir])
    if test :"${docdir+set}" != :set ; then docdir='${datarootdir}/doc/${PACKAGE_TARNAME}' ; fi
    AC_SUBST([docdir])
    if test :"${htmldir+set}" != :set ; then htmldir='${docdir}' ; fi
    AC_SUBST([htmldir])
    if test :"${dvidir+set}" != :set ; then dvidir='${docdir}' ; fi
    AC_SUBST([dvidir])
    if test :"${pdfdir+set}" != :set ; then pdfdir='${docdir}' ; fi
    AC_SUBST([pdfdir])
    if test :"${psdir+set}" != :set ; then psdir='${docdir}' ; fi
    AC_SUBST([psdir])
    AC_SUBST([rootdir])
dnl
dnl Need to check this before libtool gets done
dnl
    AC_MSG_CHECKING([for development environment])
    AC_ARG_ENABLE([devel],
	AS_HELP_STRING([--disable-devel],
	    [disable development environment.  @<:@default=enabled@:>@]),
	[dnl
	    if test :"${USE_MAINTAINER_MODE:-no}" != :no
	    then
		enable_devel='yes'
	    else
		enable_devel="$enableval"
	    fi
	], [enable_devel='yes'])
    AC_MSG_RESULT([$enable_devel])
    AM_CONDITIONAL([DEVELOPMENT], [test :"${enable_devel:-yes}" = :yes])dnl
dnl
dnl Don't build libtool static libraries if development environment not
dnl specified
dnl
    if test :"${enable_devel:-yes}" != :yes
    then
	enable_static='no'
    fi
])# _OPENSS7_DIRCHANGE
# =============================================================================

# =============================================================================
# _OPENSS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OUTPUT], [dnl
    eval "PACKAGE_BINDIR=\"${bindir}\""
    eval "PACKAGE_BINDIR=\"${PACKAGE_BINDIR}\""
    eval "PACKAGE_BINDIR=\"${PACKAGE_BINDIR}\""
    PACKAGE_BINDIR=`echo "$PACKAGE_BINDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_BINDIR], ["${PACKAGE_BINDIR}"], [Define to
	the location of the bindir.])
    eval "PACKAGE_SBINDIR=\"${sbindir}\""
    eval "PACKAGE_SBINDIR=\"${PACKAGE_SBINDIR}\""
    eval "PACKAGE_SBINDIR=\"${PACKAGE_SBINDIR}\""
    PACKAGE_SBINDIR=`echo "$PACKAGE_SBINDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_SBINDIR], ["${PACKAGE_SBINDIR}"], [Define to
	the location of the sbindir.])
    eval "PACKAGE_LIBEXECDIR=\"${libexecdir}\""
    eval "PACKAGE_LIBEXECDIR=\"${PACKAGE_LIBEXECDIR}\""
    eval "PACKAGE_LIBEXECDIR=\"${PACKAGE_LIBEXECDIR}\""
    PACKAGE_LIBEXECDIR=`echo "$PACKAGE_LIBEXECDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_LIBEXECDIR], ["${PACKAGE_LIBEXECDIR}"], [Define to
	the location of the libexecdir.])
    eval "PACKAGE_LIBEXEC32DIR=\"${libexec32dir}\""
    eval "PACKAGE_LIBEXEC32DIR=\"${PACKAGE_LIBEXEC32DIR}\""
    eval "PACKAGE_LIBEXEC32DIR=\"${PACKAGE_LIBEXEC32DIR}\""
    PACKAGE_LIBEXEC32DIR=`echo "$PACKAGE_LIBEXEC32DIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_LIBEXEC32DIR], ["${PACKAGE_LIBEXEC32DIR}"], [Define to
	the location of the libexec32dir.])
    eval "PACKAGE_DATAROOTDIR=\"${datarootdir}\""
    eval "PACKAGE_DATAROOTDIR=\"${PACKAGE_DATAROOTDIR}\""
    eval "PACKAGE_DATAROOTDIR=\"${PACKAGE_DATAROOTDIR}\""
    PACKAGE_DATAROOTDIR=`echo "$PACKAGE_DATAROOTDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_DATAROOTDIR], ["${PACKAGE_DATAROOTDIR}"], [Define to
	the location of the datarootdir.])
    eval "PACKAGE_DATADIR=\"${datadir}\""
    eval "PACKAGE_DATADIR=\"${PACKAGE_DATADIR}\""
    eval "PACKAGE_DATADIR=\"${PACKAGE_DATADIR}\""
    PACKAGE_DATADIR=`echo "$PACKAGE_DATADIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_DATADIR], ["${PACKAGE_DATADIR}"], [Define to
	the location of the datadir.])
    eval "PACKAGE_SYSCONFDIR=\"${sysconfdir}\""
    eval "PACKAGE_SYSCONFDIR=\"${PACKAGE_SYSCONFDIR}\""
    eval "PACKAGE_SYSCONFDIR=\"${PACKAGE_SYSCONFDIR}\""
    PACKAGE_SYSCONFDIR=`echo "$PACKAGE_SYSCONFDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_SYSCONFDIR], ["${PACKAGE_SYSCONFDIR}"], [Define to
	the location of the sysconfdir.])
    eval "PACKAGE_SHAREDSTATEDIR=\"${sharedstatedir}\""
    eval "PACKAGE_SHAREDSTATEDIR=\"${PACKAGE_SHAREDSTATEDIR}\""
    eval "PACKAGE_SHAREDSTATEDIR=\"${PACKAGE_SHAREDSTATEDIR}\""
    PACKAGE_SHAREDSTATEDIR=`echo "$PACKAGE_SHAREDSTATEDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_SHAREDSTATEDIR], ["${PACKAGE_SHAREDSTATEDIR}"], [Define to
	the location of the sharedstatedir.])
    eval "PACKAGE_LOCALSTATEDIR=\"${localstatedir}\""
    eval "PACKAGE_LOCALSTATEDIR=\"${PACKAGE_LOCALSTATEDIR}\""
    eval "PACKAGE_LOCALSTATEDIR=\"${PACKAGE_LOCALSTATEDIR}\""
    PACKAGE_LOCALSTATEDIR=`echo "$PACKAGE_LOCALSTATEDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_LOCALSTATEDIR], ["${PACKAGE_LOCALSTATEDIR}"], [Define to
	the location of the localstatedir.])
    eval "PACKAGE_INCLUDEDIR=\"${includedir}\""
    eval "PACKAGE_INCLUDEDIR=\"${PACKAGE_INCLUDEDIR}\""
    eval "PACKAGE_INCLUDEDIR=\"${PACKAGE_INCLUDEDIR}\""
    PACKAGE_INCLUDEDIR=`echo "$PACKAGE_INCLUDEDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_INCLUDEDIR], ["${PACKAGE_INCLUDEDIR}"], [Define to
	the location of the includedir.])
    eval "PACKAGE_OLDINCLUDEDIR=\"${oldincludedir}\""
    eval "PACKAGE_OLDINCLUDEDIR=\"${PACKAGE_OLDINCLUDEDIR}\""
    eval "PACKAGE_OLDINCLUDEDIR=\"${PACKAGE_OLDINCLUDEDIR}\""
    PACKAGE_OLDINCLUDEDIR=`echo "$PACKAGE_OLDINCLUDEDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_OLDINCLUDEDIR], ["${PACKAGE_OLDINCLUDEDIR}"], [Define to
	the location of the oldincludedir.])
    eval "PACKAGE_DOCDIR=\"${docdir}\""
    eval "PACKAGE_DOCDIR=\"${PACKAGE_DOCDIR}\""
    eval "PACKAGE_DOCDIR=\"${PACKAGE_DOCDIR}\""
    PACKAGE_DOCDIR=`echo "$PACKAGE_DOCDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_DOCDIR], ["${PACKAGE_DOCDIR}"], [Define to
			the location of the docdir.])
    eval "PACKAGE_INFODIR=\"${infodir}\""
    eval "PACKAGE_INFODIR=\"${PACKAGE_INFODIR}\""
    eval "PACKAGE_INFODIR=\"${PACKAGE_INFODIR}\""
    PACKAGE_INFODIR=`echo "$PACKAGE_INFODIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_INFODIR], ["${PACKAGE_INFODIR}"], [Define to
	the location of the infodir.])
    eval "PACKAGE_HTMLDIR=\"${htmldir}\""
    eval "PACKAGE_HTMLDIR=\"${PACKAGE_HTMLDIR}\""
    eval "PACKAGE_HTMLDIR=\"${PACKAGE_HTMLDIR}\""
    PACKAGE_HTMLDIR=`echo "$PACKAGE_HTMLDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_HTMLDIR], ["${PACKAGE_HTMLDIR}"], [Define to
	the location of the htmldir.])
    eval "PACKAGE_DVIDIR=\"${dvidir}\""
    eval "PACKAGE_DVIDIR=\"${PACKAGE_DVIDIR}\""
    eval "PACKAGE_DVIDIR=\"${PACKAGE_DVIDIR}\""
    PACKAGE_DVIDIR=`echo "$PACKAGE_DVIDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_DVIDIR], ["${PACKAGE_DVIDIR}"], [Define to
	the location of the dvidir.])
    eval "PACKAGE_PDFDIR=\"${pdfdir}\""
    eval "PACKAGE_PDFDIR=\"${PACKAGE_PDFDIR}\""
    eval "PACKAGE_PDFDIR=\"${PACKAGE_PDFDIR}\""
    PACKAGE_PDFDIR=`echo "$PACKAGE_PDFDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_PDFDIR], ["${PACKAGE_PDFDIR}"], [Define to
	the location of the pdfdir.])
    eval "PACKAGE_PSDIR=\"${psdir}\""
    eval "PACKAGE_PSDIR=\"${PACKAGE_PSDIR}\""
    eval "PACKAGE_PSDIR=\"${PACKAGE_PSDIR}\""
    PACKAGE_PSDIR=`echo "$PACKAGE_PSDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_PSDIR], ["${PACKAGE_PSDIR}"], [Define to
	the location of the psdir.])
    eval "PACKAGE_LIBDIR=\"${libdir}\""
    eval "PACKAGE_LIBDIR=\"${PACKAGE_LIBDIR}\""
    eval "PACKAGE_LIBDIR=\"${PACKAGE_LIBDIR}\""
    PACKAGE_LIBDIR=`echo "$PACKAGE_LIBDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_LIBDIR], ["${PACKAGE_LIBDIR}"], [Define to
	the location of the libdir.])
    eval "PACKAGE_LIB32DIR=\"${lib32dir}\""
    eval "PACKAGE_LIB32DIR=\"${PACKAGE_LIB32DIR}\""
    eval "PACKAGE_LIB32DIR=\"${PACKAGE_LIB32DIR}\""
    PACKAGE_LIB32DIR=`echo "$PACKAGE_LIB32DIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_LIB32DIR], ["${PACKAGE_LIB32DIR}"], [Define to
	the location of the lib32dir.])
    eval "PACKAGE_LOCALEDIR=\"${localedir}\""
    eval "PACKAGE_LOCALEDIR=\"${PACKAGE_LOCALEDIR}\""
    eval "PACKAGE_LOCALEDIR=\"${PACKAGE_LOCALEDIR}\""
    PACKAGE_LOCALEDIR=`echo "$PACKAGE_LOCALEDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_LOCALEDIR], ["${PACKAGE_LOCALEDIR}"], [Define to
	the location of the localedir.])
    eval "PACKAGE_MANDIR=\"${mandir}\""
    eval "PACKAGE_MANDIR=\"${PACKAGE_MANDIR}\""
    eval "PACKAGE_MANDIR=\"${PACKAGE_MANDIR}\""
    PACKAGE_MANDIR=`echo "$PACKAGE_MANDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_MANDIR], ["${PACKAGE_MANDIR}"], [Define to
	the location of the mandir.])
    eval "PACKAGE_INITRDDIR=\"${initrddir}\""
    eval "PACKAGE_INITRDDIR=\"${PACKAGE_INITRDDIR}\""
    eval "PACKAGE_INITRDDIR=\"${PACKAGE_INITRDDIR}\""
    PACKAGE_INITRDDIR=`echo "$PACKAGE_INITRDDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_INITRDDIR], ["${PACKAGE_INITRDDIR}"], [Define to
	the location of the initrddir.])
    eval "PACKAGE_CONFIGDIR=\"${configdir}\""
    eval "PACKAGE_CONFIGDIR=\"${PACKAGE_CONFIGDIR}\""
    eval "PACKAGE_CONFIGDIR=\"${PACKAGE_CONFIGDIR}\""
    PACKAGE_CONFIGDIR=`echo "$PACKAGE_CONFIGDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_CONFIGDIR], ["${PACKAGE_CONFIGDIR}"], [Define to
	the location of the configdir.])
    eval "PACKAGE_KMODULEDIR=\"${kmoduledir}\""
    eval "PACKAGE_KMODULEDIR=\"${PACKAGE_KMODULEDIR}\""
    eval "PACKAGE_KMODULEDIR=\"${PACKAGE_KMODULEDIR}\""
    PACKAGE_KMODULEDIR=`echo "$PACKAGE_KMODULEDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_KMODULEDIR], ["${PACKAGE_KMODULEDIR}"], [Define to
	the location of the kmoduledir.])
    eval "PACKAGE_MODUTILDIR=\"${modutildir}\""
    eval "PACKAGE_MODUTILDIR=\"${PACKAGE_MODUTILDIR}\""
    eval "PACKAGE_MODUTILDIR=\"${PACKAGE_MODUTILDIR}\""
    PACKAGE_MODUTILDIR=`echo "$PACKAGE_MODUTILDIR" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
    AC_DEFINE_UNQUOTED([PACKAGE_MODUTILDIR], ["${PACKAGE_MODUTILDIR}"], [Define to
	the location of the modutildir.])
])# _OPENSS7_OUTPUT
# =============================================================================

# =============================================================================
# _OPENSS7_CACHE
# -----------------------------------------------------------------------------
# A little trick with caches and site files.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_CACHE], [dnl
    # force a cache file to be created even if not specified
    if test "$cache_file" = '/dev/null' -o :"$cache_file" = :
    then
	cache_file='config.cache'
	if test ! -e "$cache_file"
	then
	    cat /dev/null > "$cache_file"
	fi
    fi
    AC_SUBST([cache_file])dnl
    # if site file not specified, use local site file
    if test :"$CONFIG_SITE" = :
    then
	CONFIG_SITE='config.site'
    fi
    AC_SUBST([CONFIG_SITE])dnl
    AC_CONFIG_COMMANDS([siteconfig], [dnl
	if test :"$CONFIG_SITE" != : -a :"$cache_file" != :
	then
	    for config_site in $CONFIG_SITE
	    do
		if test -w "$config_site" -o ! -e "$config_site"
		then
		    if touch "$config_site" >/dev/null 2>&1
		    then
			cat "$cache_file" | egrep '^(ac_cv_|am_cv_|ap_cv_|lt_cv_)' > "$config_site" 2>/dev/null
			#cat "$cache_file" | egrep '^(ac_cv_|am_cv_|ap_cv_|lt_cv_)' | egrep -v '^(ac_cv_env_|ac_cv_host|ac_cv_target)' > "$config_site" 2>/dev/null
			#cat "$cache_file" | egrep -v '\<(ac_cv_env_|ac_cv_host|ac_cv_target|linux_cv_|ksyms_cv_|rpm_cv_|deb_cv_|strconf_cv_|sctp_cv_|xns_cv_|lis_cv_|lfs_cv_|strcomp_cv_|streams_cv_|xti_cv_|xopen_cv_|inet_cv_|xnet_cv_|devfs_cv_|init_cv_|pkg_cv_)' > "$config_site" 2>/dev/null
		    fi
		fi
	    done
	fi], [cache_file="$cache_file" ; CONFIG_SITE="$CONFIG_SITE"])
])# _OPENSS7_CACHE
# =============================================================================

# =============================================================================
# _OPENSS7_DEBUG
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_DEBUG], [dnl
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
	CFLAGS=`echo " $CFLAGS" | sed -e 's, -Wall,,g'`
	CFLAGS=`echo " $CFLAGS" | sed -e 's, -Werror,,g'`
dnl	CFLAGS=`echo " $CFLAGS" | sed -e 's, -Wundef,,g'`  dnl this frags out flex 2.5.33
	CFLAGS=`echo " $CFLAGS" | sed -e 's% -Wp,-D_FORTIFY_SOURCE=[[0-9]]*%%g'`
dnl	CFLAGS="${CFLAGS}${CFLAGS:+ }-Wall -Wstrict-prototypes -Wno-trigraphs -Wundef -Wp,-D_FORTIFY_SOURCE=2 -Werror"
	CFLAGS="${CFLAGS}${CFLAGS:+ }-Wall -Wstrict-prototypes -Wno-trigraphs -Wp,-D_FORTIFY_SOURCE=2 -Werror"
    fi
])# _OPENSS7_DEBUG
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS], [dnl
    _OPENSS7_OPTIONS_CHECK
    _OPENSS7_OPTIONS_DOCS
    _OPENSS7_OPTIONS_GPG
    _OPENSS7_OPTIONS_PKG_EPOCH
    _OPENSS7_OPTIONS_PKG_RELEASE
    _OPENSS7_OPTIONS_PKG_DISTDIR
    _OPENSS7_OPTIONS_PKG_ARCH
    _OPENSS7_OPTIONS_PKG_INDEP
])# _OPENSS7_OPTIONS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_CHECK
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_CHECK], [dnl
    AC_MSG_CHECKING([for preinstall tests on check])
    AC_ARG_ENABLE([checks],
	AS_HELP_STRING([--enable-checks],
	    [enable preinstall checks. @<:@default=auto@:>@]),
	[enable_checks="$enableval"], [dnl
	     if test :"${USE_MAINTAINER_MODE:-no}" != :no
	     then
		 enable_checks='yes'
	     else
		 enable_checks='no'
	     fi])
    AC_MSG_RESULT([$enable_checks])
    AM_CONDITIONAL([PERFORM_CHECKS], [test :"${enable_checks:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_CHECK
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_DOCS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_DOCS], [dnl
    AC_MSG_CHECKING([for documentation included])
    AC_ARG_ENABLE([docs],
	AS_HELP_STRING([--disable-docs],
	    [disable documentation build and install.  @<:@default=enabled@:>@]),
	[dnl
	    if test :"${USE_MAINTAINER_MODE:-no}" != :no
	    then
		enable_docs='yes'
	    else
		enable_docs="$enableval"
	    fi
	], [enable_docs='yes'])
    AC_MSG_RESULT([$enable_docs])
    AM_CONDITIONAL([DOCUMENTATION], [test :"${enable_docs:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_DOCS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_GPG
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_GPG], [dnl
    AC_ARG_VAR([GPG], [GPG signature command])
    AC_PATH_PROG([GPG], [gpg pgp], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${GPG:-no}" = :no ; then
	AC_MSG_WARN([Could not find gpg program in PATH.])
	GPG=/usr/bin/gpg
    fi
dnl ---------------------------------------------------------
    AC_MSG_CHECKING([for gpg user])
    AC_ARG_VAR([GNUPGUSER], [GPG user name])
    AC_ARG_WITH([gpg-user],
	AS_HELP_STRING([--with-gpg-user=USERNAME],
	    [specify the USER for signing DEBs, RPMs and tarballs.
	    @<:@default=${GNUPGUSER}@:>@]),
	[with_gpg_user="$withval"],
	[with_gpg_user="$GNUPGUSER"
	 if test :"${with_gpg_user:-no}" = :no ; then
	     if test -r .gnupguser; then d= ; else d="$srcdir/" ; fi
	     if test -r ${d}.gnupguser
	     then with_gpg_user="`cat ${d}.gnupguser`"
	     else with_gpg_user=''
	     fi
dnl          if test :"${with_gpg_user:-no}" = :no ; then
dnl              with_gpg_user="`whoami`"
dnl          fi
	 fi])
    GNUPGUSER="${with_gpg_user:-`whoami`}"
    AC_MSG_RESULT([${GNUPGUSER:-no}])
dnl ---------------------------------------------------------
    AC_MSG_CHECKING([for gpg home])
    AC_ARG_VAR([GNUPGHOME], [GPG home directory])
    AC_ARG_WITH([gpg-home],
	AS_HELP_STRING([--with-gpg-home=HOMEDIR],
	    [specify the HOME for signing DEBs, RPMs and tarballs.
	    @<:@default=${GNUPGHOME:-~/.gnupg}@:>@]),
	[with_gpg_home="$withval"],
	[with_gpg_home="$GNUPGHOME"
	 if test :"${with_gpg_home:-no}" = :no ; then
	     if test -r .gnupghome; then d= ; else d="$srcdir/" ; fi
	     if test -r ${d}.gnupghome
	     then with_gpg_home="`cat ${d}.gnupghome`"
	     else with_gpg_home=''
	     fi
dnl          if test :"${with_gpg_home:-no}" = :no ; then
dnl              with_gpg_home='~/.gnupg'
dnl          fi
	 fi])
    GNUPGHOME="${with_gpg_home:-~/.gnupg}"
    AC_MSG_RESULT([${GNUPGHOME:-no}])
])# _OPENSS7_OPTIONS_GPG
# =============================================================================

# =========================================================================
# _OPENSS7_OPTIONS_PKG_EPOCH
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_EPOCH], [dnl
    AC_MSG_CHECKING([for pkg epoch])
    AC_ARG_WITH([pkg-epoch],
	AS_HELP_STRING([--with-pkg-epoch=EPOCH],
	    [specify the EPOCH for the package file.  @<:@default=auto@:>@]),
	[with_pkg_epoch="$withval"],
	[if test -r .pkgepoch; then d= ; else d="$srcdir/" ; fi
	 if test -r ${d}.pkgepoch
	 then with_pkg_epoch="`cat ${d}.pkgepoch`"
	 else with_pkg_epoch=1
	 fi])
    AC_MSG_RESULT([${with_pkg_epoch:-1}])
    PACKAGE_EPOCH="${with_pkg_epoch:-1}"
    AC_SUBST([PACKAGE_EPOCH])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_EPOCH], [$PACKAGE_EPOCH], [The Package Epoch.
	This defaults to 1.])
])# _OPENSS7_OPTIONS_PKG_EPOCH
# =========================================================================

# =========================================================================
# _OPENSS7_OPTIONS_PKG_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_RELEASE], [dnl
    AC_MSG_CHECKING([for pkg release])
    AC_ARG_WITH([pkg-release],
	AS_HELP_STRING([--with-pkg-release=RELEASE],
	    [specify the RELEASE for the package files.  @<:@default=auto@:>@]),
	[with_pkg_release="$withval"],
	[if test -r .pkgrelease ; then d= ; else d="$srcdir/" ; fi
	 if test -r ${d}.pkgrelease
	 then with_pkg_release="`cat ${d}.pkgrelease`"
	 else with_pkg_release=1
	 fi])
    AC_MSG_RESULT([${with_pkg_release:-1}])
    PACKAGE_RELEASE="${with_pkg_release:-1}"
    AC_SUBST([PACKAGE_RELEASE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_RELEASE], ["$PACKAGE_RELEASE"], [The Package
	Release. This defaults to Custom.])
])# _OPENSS7_OPTIONS_PKG_RELEASE
# =========================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_DISTDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_DISTDIR], [dnl
    pkg_tmp=`(cd . ; pwd)`
    AC_ARG_WITH([pkg-distdir],
	AS_HELP_STRING([--with-pkg-distdir=DIR],
	    [specify the package distribution directory. @<:@default=.@:>@]),
	[with_pkg_distdir="$withval"],
	[with_pkg_distdir="$pkg_tmp"])
    AC_CACHE_CHECK([for pkg distdir], [pkg_cv_distdir], [dnl
	case :"${with_pkg_distdir:-default}" in
	    (:no|:NO)
		pkg_cv_distdir="$pkg_tmp"
		;;
	    (:yes|:YES|:default|:DEFAULT)
		pkg_cv_distdir="/usr/src"
		;;
	    (*)
		pkg_cv_distdir="$with_pkg_distdir"
		;;
	esac
    ])
    PACKAGE_DISTDIR="$pkg_cv_distdir"
    AC_SUBST([PACKAGE_DISTDIR])dnl
])# _OPENSS7_OPTIONS_PKG_DISTDIR
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_ARCH
# -----------------------------------------------------------------------------
# Debian (and rpm for that matter) can build architecture dependent or
# architecture independent packages.  This option specifies whether architecture
# dependent packages are to be built and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_ARCH], [dnl
    AC_MSG_CHECKING([for deb build/install of arch packages])
    AC_ARG_ENABLE([arch],
	AS_HELP_STRING([--enable-arch],
	    [build and install arch packages.  @<:@default=yes@:>@]),
	[enable_arch="$enableval"],
	[enable_arch='yes'])
    AC_MSG_RESULT([${enable_arch:-yes}])
    AM_CONDITIONAL([PKG_BUILD_ARCH], [test :"${enable_arch:-yes}" = :yes])dnl
	
])# _OPENSS7_OPTIONS_PKG_ARCH
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_INDEP
# -----------------------------------------------------------------------------
# Debian (and rpm for that matter) can build architecture dependent or
# architecture independent packages.  This option specifies whether architecture
# independent packages are to be built and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_INDEP], [dnl
    AC_MSG_CHECKING([for deb build/install of indep packages])
    AC_ARG_ENABLE([indep],
	AS_HELP_STRING([--enable-indep],
	    [build and install indep packages.  @<:@default=yes@:>@]),
	[enable_indep="$enableval"],
	[enable_indep='yes'])
    AC_MSG_RESULT([${enable_indep:-yes}])
    AM_CONDITIONAL([PKG_BUILD_INDEP], [test :"${enable_indep:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_PKG_INDEP
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_CFLAGS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_CFLAGS], [dnl
    AC_MSG_CHECKING([for user CFLAGS])
    AC_MSG_RESULT([${CFLAGS}])
    AC_MSG_CHECKING([for user CFLAGS])
    AC_ARG_WITH([optimize],
	AC_HELP_STRING([--with-optimize=HOW],
	    [specify optimization, normal, size, speed or quick,
	     @<:@default=auto@:>@]),
	[with_optimize="$withval"],
	[with_optimize=''])
    case "${with_optimize:-auto}" in
	(size)
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    CFLAGS="-Os -g${CFLAGS:+ $CFLAGS}"
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    ;;
	(speed)
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    CFLAGS="-O3 -g${CFLAGS:+ $CFLAGS}"
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    CFLAGS="${CFLAGS:+$CFLAGS }-freorder-blocks"
	    ;;
	(normal)
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    CFLAGS="-O2 -g${CFLAGS:+ $CFLAGS}"
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    CFLAGS="${CFLAGS:+$CFLAGS }-freorder-blocks"
	    ;;
	(quick)
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?keep-inline-functions,,g'`
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -f(no-)?keep-static-consts,,g'`
	    CFLAGS="-O0 -g${CFLAGS:+ $CFLAGS}"
	    CFLAGS="${CFLAGS:+$CFLAGS }-finline"
	    CFLAGS="${CFLAGS:+$CFLAGS }-fno-keep-inline-functions"
	    CFLAGS="${CFLAGS:+$CFLAGS }-fno-keep-static-consts"
	(auto)
	    : # don't do anything
	    ;;
    esac
    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -W(no-)?trigraphs,,g'`
    CFLAGS="${CFLAGS:+$CFLAGS }-Wno-trigraphs"
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wno-system-headers"
dnl	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -W(no-)?undef,,g'`
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wundef"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wno-endif-labels"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wbad-function-cast"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wcast-qual"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wcast-align"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wwrite-strings"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wconversion"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wsign-compare"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Waggregate-return"
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -W(no-)?strict-prototypes,,g'`
	    CFLAGS="${CFLAGS:+$CFLAGS }-Wstrict-prototypes"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wmissing-prototypes"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wmissing-declarations"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wmissing-noreturn"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wmissing-format-attribute"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wpacked"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wpadded"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wredundant-decls"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wnested-externs"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wunreachable-code"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Winline"
dnl	    CFLAGS="${CFLAGS:+$CFLAGS }-Wdisabled-optimization"
	    CFLAGS=`echo " $CFLAGS" | sed -e 's, -Wall,,g'`
	    CFLAGS="${CFLAGS:+$CFLAGS }-Wall"
	    CFLAGS=`echo " $CFLAGS" | sed -e 's% -Wp,-D_FORTIFY_SOURCE=[[0-9]]*%%g'`
	    CFLAGS="${CFLAGS:+$CFLAGS }-Wp,-D_FORTIFY_SOURCE=2"
	    CFLAGS=`echo " $CFLAGS" | sed -r -e 's, -W(no-)?error,,g'`
	    CFLAGS="${CFLAGS:+$CFLAGS }-Werror"
    fi
    CFLAGS=`echo "$CFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    AC_MSG_RESULT([${CFLAGS}])
])# _OPENSS7_OPTIONS_CFLAGS
# =============================================================================

# =============================================================================
# _OPENSS7_MISSING
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_MISSING], [dnl
    test x"${MISSING2+set}" = xset || MISSING2="\${SHELL} $am_aux_dir/missing2"
    if eval "$MISSING2 --run true" ; then
	am_missing2_run="$MISSING2 --run "
    else
	am_missing2_run=
	AC_MSG_WARN(['missing2' script is too old or missing])
    fi
    AC_ARG_VAR([LATEX], [Latex command.])
    AC_PATH_PROG([LATEX], [latex], [${am_missing2_run}latex],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([PSLATEX], [PS Latex command.])
    AC_PATH_PROG([PSLATEX], [pslatex], [${am_missing2_run}pslatex],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([PDFLATEX], [PDF Latex command.])
    AC_PATH_PROG([PDFLATEX], [pdflatex], [${am_missing2_run}pdflatex],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([BIBTEX], [BibTeX command.])
    AC_PATH_PROG([BIBTEX], [bibtex], [${am_missing2_run}bibtex],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([LATEX2HTML], [LaTeX to HTML command.])
    AC_PATH_PROG([LATEX2HTML], [latex2html], [${am_missing2_run}latex2html],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
dnl
dnl
dnl frags out automake
dnl
dnl    AC_ARG_VAR([DVIPS], [DVI to PS command.])
dnl    AC_PATH_PROG([DVIPS], [dvips], [${am_missing2_run}dvips],
dnl		 [$PATH:/usr/local/bin:/usr/bin:/bin])

    AC_ARG_VAR([DVIPDF], [DVI to PDF command.])
    AC_PATH_PROG([DVIPDF], [dvipdf], [${am_missing2_run}dvipdf],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([GNUPLOT], [GNU plot command.])
    AC_PATH_PROG([GNUPLOT], [gnuplot], [${am_missing2_run}gnuplot],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([FIG2DEV], [Fig to graphics format command.])
    AC_PATH_PROG([FIG2DEV], [fig2dev], [${am_missing2_run}fig2dev],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([CONVERT], [Graphics format conversion command.])
    AC_PATH_PROG([CONVERT], [convert], [${am_missing2_run}convert],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([PS2EPSI], [PS to EPSI conversion command.])
    AC_PATH_PROG([PS2EPSI], [ps2epsi], [${am_missing2_run}ps2epsi],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
    AC_ARG_VAR([EPSTOPDF], [EPS to PDF conversion command.])
    AC_PATH_PROG([EPSTOPDF], [epstopdf], [${am_missing2_run}epstopdf],
		 [$PATH:/usr/local/bin:/usr/bin:/bin])
])# _OPENSS7_MISSING
# =============================================================================

# =============================================================================
# _OPENSS7
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7], [dnl
])# _OPENSS7
# =============================================================================

# =============================================================================
#
# $Log: openss7.m4,v $
# Revision 0.9.2.57  2007/10/18 06:12:53  brian
# - more -Wundef to remove
#
# Revision 0.9.2.56  2007/10/18 05:27:52  brian
# - remove DVIPS warning
#
# Revision 0.9.2.55  2007/10/18 05:03:03  brian
# - more laxity for flex
#
# Revision 0.9.2.54  2007/10/18 04:23:31  brian
# - -Wundef frags out flex, and missing quotes
#
# Revision 0.9.2.53  2007/10/17 20:00:28  brian
# - slightly different path checks
#
# Revision 0.9.2.52  2007/10/17 19:22:32  brian
# - better tool detection
#
# Revision 0.9.2.51  2007/10/17 18:00:13  brian
# - added latex2html
#
# Revision 0.9.2.50  2007/10/17 17:43:02  brian
# - build updates
#
# Revision 0.9.2.49  2007/10/17 13:22:19  brian
# - correction
#
# Revision 0.9.2.48  2007/10/17 08:18:30  brian
# - corrections
#
# Revision 0.9.2.47  2007/10/17 08:07:39  brian
# - added missing checks
#
# Revision 0.9.2.46  2007/10/17 07:36:09  brian
# - added documentation suppression
#
# Revision 0.9.2.45  2007/08/19 11:17:34  brian
# - spelling correction
#
# Revision 0.9.2.44  2007/08/19 11:16:48  brian
# - add defines for directories
#
# Revision 0.9.2.43  2007/08/12 19:05:31  brian
# - rearrange and update headers
#
# Revision 0.9.2.42  2007/03/08 04:28:49  brian
# - substituions changed for program checking macros
#
# Revision 0.9.2.41  2007/02/22 08:36:38  brian
# - balance parentheses
#
# Revision 0.9.2.40  2007-02-12 16:55:47  brian
# - more autoconf 2.61 support
#
# Revision 0.9.2.39  2007/02/12 10:39:48  brian
# - added support for autoconf 2.61
#
# Revision 0.9.2.38  2006/10/21 09:18:51  brian
# - better config.site generation
#
# Revision 0.9.2.37  2006/09/25 08:56:35  brian
# - corrections by inspection
#
# Revision 0.9.2.35  2006/07/25 06:38:52  brian
# - expanded minor device numbers and optimization and locking corrections
#
# Revision 0.9.2.34  2006/07/23 04:04:10  brian
# - more control for user optimizations
#
# Revision 0.9.2.33  2006/07/14 00:12:25  brian
# - substitute config cache and site filenames
#
# Revision 0.9.2.32  2006/03/25 12:55:08  brian
# - got enable_static backwards
#
# Revision 0.9.2.31  2006/03/20 12:12:18  brian
# - don't build libtool static libraries no devel
#
# Revision 0.9.2.30  2006/03/20 11:51:09  brian
# - added check for --disable-devel
#
# Revision 0.9.2.29  2006/03/11 09:49:51  brian
# - a bit better checking
#
# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
