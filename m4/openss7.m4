# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) File: m4/openss7.m4
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

# AC_LANG_FLAG(FLAG, [ACTION-IF-WORKS], [ACTION-IF-NOT-WORK])
# -----------------------------------------------------------
# Test compilation with FLAG.
AC_DEFUN([AC_LANG_FLAG],
[_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])

# AC_LANG_FLAG(C)(FLAG, [IF-WORKS], [IF-DOES-NOT-WORK])
# --------------------------------------------------------
# Test whether a flag works on its own.  Consider the flag to not work if it
# generates warnings when plain compiles do not.
m4_define([AC_LANG_FLAG(C)],
[ac_test_CFLAGS=${CFLAGS+set}
ac_save_CFLAGS=$CFLAGS
AS_VAR_PUSHDEF([CacheVar], [AS_TR_SH(ac_cv_prog_c[]$1)])dnl
AC_CACHE_CHECK(whether $GCJ accepts $1, CacheVar,
    [ac_save_c_werror_flag=$ac_c_werror_flag
     ac_c_werror_flag=yes
     AS_VAR_SET([CacheVar], [no])
     CFLAGS="$1"
     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	AS_VAR_SET([CacheVar], [yes]),
	[CFLAGS=""
	_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	    [],
	    [ac_c_werror_flag=$ac_save_c_werror_flag
	     CFLAGS="$1"
	     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
		AS_VAR_SET([CacheVar], [yes]))])])
     ac_c_werror_flag=$ac_save_c_werror_flag])
if test :AS_VAR_GET([CacheVar]) = :yes
then :;
$2
else :;
$3
fi[]dnl
AS_VAR_POPDEF([CacheVar])dnl
])

# AC_LANG_FLAG(C++)(FLAG, [IF-WORKS], [IF-DOES-NOT-WORK])
# --------------------------------------------------------
# Test whether a flag works on its own.  Consider the flag to not work if it
# generates warnings when plain compiles do not.
m4_define([AC_LANG_FLAG(C++)],
[ac_test_CFLAGS=${CFLAGS+set}
ac_save_CFLAGS=$CFLAGS
AS_VAR_PUSHDEF([CacheVar], [AS_TR_SH(ac_cv_prog_cxx[]$1)])dnl
AC_CACHE_CHECK(whether $GCJ accepts $1, CacheVar,
    [ac_save_cxx_werror_flag=$ac_cxx_werror_flag
     ac_cxx_werror_flag=yes
     AS_VAR_SET([CacheVar], [no])
     CFLAGS="$1"
     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	AS_VAR_SET([CacheVar], [yes]),
	[CFLAGS=""
	_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	    [],
	    [ac_cxx_werror_flag=$ac_save_cxx_werror_flag
	     CFLAGS="$1"
	     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
		AS_VAR_SET([CacheVar], [yes]))])])
     ac_cxx_werror_flag=$ac_save_cxx_werror_flag])
if test :AS_VAR_GET([CacheVar]) = :yes
then :;
$2
else :;
$3
fi[]dnl
AS_VAR_POPDEF([CacheVar])dnl
])

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
    PACKAGE_PDATE=`date '+%B %e, %Y'`
    AC_SUBST([PACKAGE_PDATE])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_PDATE], ["$PACKAGE_PDATE"], [The package publishing date.])
    PKGINCL="include/sys/${PACKAGE_NAME}"
    AC_SUBST([PKGINCL])dnl
    PACKAGE_CVSTAG=`echo "${PACKAGE_LCNAME}-${VERSION}" | sed -e 's,-,_,g;s,\.,-,g'`
    AC_SUBST([PACKAGE_CVSTAG])dnl
    AC_NOTICE(
[
=============================================================================
BEGINNING OF SEPARATE COPYRIGHT MATERIAL
=============================================================================

Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
(or any successor regulations).'

-----------------------------------------------------------------------------

Commercial licensing and support of this software is available from OpenSS7
Corporation at a fee.  See http://www.openss7.com/

=============================================================================

])dnl

    _OPENSS7_OPTIONS
    _OPENSS7_CACHE
    AC_ARG_WITH([optimize],
	[AS_HELP_STRING([--with-optimize=HOW],
	    [optimization: normal, size, speed or quick @<:@default=auto@:>@])])
    _OPENSS7_OPTIONS_CFLAGS
    _OPENSS7_OPTIONS_LDFLAGS
    _OPENSS7_OPTIONS_CXXFLAGS
    _OPENSS7_OPTIONS_GCJFLAGS
    _OPENSS7_MISSING2
    _OPENSS7_MISSING3
    _OPENSS7_MISSING4
    _OPENSS7_UPDATE
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
    if test :"$sharedstatedir" = :'${prefix}/com' ; then sharedstatedir='${rootdir}/com' ; fi
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
    AC_SUBST([datarootdir])dnl
    if test :"${datadir+set}" != :set ; then datadir='${datarootdir}' ; fi
    AC_SUBST([datadir])dnl
    if test :"${localedir+set}" != :set ; then localedir='${datarootdir}/locale' ; fi
    AC_SUBST([localedir])dnl
    if test :"${docdir+set}" != :set ; then docdir='${datarootdir}/doc/${PACKAGE_TARNAME}' ; fi
    if test :"$docdir" = :'${datarootdir}/doc/${PACKAGE_TARNAME}' ; then
	docdir='${datarootdir}/doc/${PACKAGE}-${VERSION}'
    fi
    AC_SUBST([docdir])dnl
    if test :"${htmldir+set}" != :set ; then htmldir='${docdir}' ; fi
    AC_SUBST([htmldir])dnl
    if test :"${dvidir+set}" != :set ; then dvidir='${docdir}' ; fi
    AC_SUBST([dvidir])dnl
    if test :"${pdfdir+set}" != :set ; then pdfdir='${docdir}' ; fi
    AC_SUBST([pdfdir])dnl
    if test :"${psdir+set}" != :set ; then psdir='${docdir}' ; fi
    AC_SUBST([psdir])dnl
    AC_SUBST([rootdir])dnl
dnl
dnl Some more directories...
dnl
    if test :"${snmpdlmoddir+set}" != :set ; then snmpdlmoddir='${libdir}/snmp/dlmod' ; fi
    AC_SUBST([snmpdlmoddir])dnl
    if test :"${snmpmibdir+set}" != :set ; then snmpmibdir='${datarootdir}/snmp/mibs' ; fi
    AC_SUBST([snmpmibdir])dnl
    if test :"${javadir+set}" != :set ; then javadir='${datarootdir}/java' ; fi
    AC_SUBST([javadir])dnl
    if test :"${javadocdir+set}" != :set ; then javadocdir='${datarootdir}/javadoc' ; fi
    AC_SUBST([javadocdir])dnl
    if test :"${jnidir+set}" != :set ; then jnidir='${libdir}/java' ; fi
    AC_SUBST([jnidir])dnl
    if test :"${perldir+set}" != :set ; then perldir="${libdir}/perl5" ; fi
    AC_SUBST([perldir])dnl
    if test :"${tcllibdir+set}" != :set ; then tcllibdir='${libdir}' ; fi
    AC_SUBST([tcllibdir])dnl
    if test :"${tclsrcdir+set}" != :set ; then tclsrcdir='${datarootdir}' ; fi
    AC_SUBST([tclsrcdir])dnl
    if test :"${syslibdir+set}" != :set ; then syslibdir='${rootdir}/lib' ; fi
    AC_SUBST([syslibdir])dnl
    if test :"${sysbindir+set}" != :set ; then sysbindir='${rootdir}/bin' ; fi
    AC_SUBST([sysbindir])dnl
    if test :"${syssbindir+set}" != :set ; then syssbindir='${rootdir}/sbin' ; fi
    AC_SUBST([syssbindir])dnl
dnl
dnl Need to check this before libtool gets done
dnl
    AC_MSG_CHECKING([for development environment])
    AC_ARG_ENABLE([devel],
	[AS_HELP_STRING([--disable-devel],
	    [development environment @<:@default=enabled@:>@])])
    AC_MSG_RESULT([${enable_devel:-yes}])
    AM_CONDITIONAL([DEVELOPMENT], [test :"${enable_devel:-yes}" = :yes])dnl
dnl
dnl Don't build libtool static libraries if development environment not
dnl specified
dnl
    if test :"${enable_devel:-yes}" != :yes ; then
	enable_static=no
    fi
])# _OPENSS7_DIRCHANGE
# =============================================================================

# =============================================================================
# _OPENSS7_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OUTPUT], [dnl
    # expand these first
    test "x$prefix" = xNONE && prefix=$ac_default_prefix
    test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
    eval "PACKAGE_BINDIR=\"${bindir}\""
    eval "PACKAGE_BINDIR=\"${PACKAGE_BINDIR}\""
    eval "PACKAGE_BINDIR=\"${PACKAGE_BINDIR}\""
    PACKAGE_BINDIR=`echo "$PACKAGE_BINDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_BINDIR], ["${PACKAGE_BINDIR}"], [Define to
	the location of the bindir.])
    eval "PACKAGE_SBINDIR=\"${sbindir}\""
    eval "PACKAGE_SBINDIR=\"${PACKAGE_SBINDIR}\""
    eval "PACKAGE_SBINDIR=\"${PACKAGE_SBINDIR}\""
    PACKAGE_SBINDIR=`echo "$PACKAGE_SBINDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_SBINDIR], ["${PACKAGE_SBINDIR}"], [Define to
	the location of the sbindir.])
    eval "PACKAGE_LIBEXECDIR=\"${libexecdir}\""
    eval "PACKAGE_LIBEXECDIR=\"${PACKAGE_LIBEXECDIR}\""
    eval "PACKAGE_LIBEXECDIR=\"${PACKAGE_LIBEXECDIR}\""
    PACKAGE_LIBEXECDIR=`echo "$PACKAGE_LIBEXECDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_LIBEXECDIR], ["${PACKAGE_LIBEXECDIR}"], [Define to
	the location of the libexecdir.])
    eval "PACKAGE_DATAROOTDIR=\"${datarootdir}\""
    eval "PACKAGE_DATAROOTDIR=\"${PACKAGE_DATAROOTDIR}\""
    eval "PACKAGE_DATAROOTDIR=\"${PACKAGE_DATAROOTDIR}\""
    PACKAGE_DATAROOTDIR=`echo "$PACKAGE_DATAROOTDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_DATAROOTDIR], ["${PACKAGE_DATAROOTDIR}"], [Define to
	the location of the datarootdir.])
    eval "PACKAGE_DATADIR=\"${datadir}\""
    eval "PACKAGE_DATADIR=\"${PACKAGE_DATADIR}\""
    eval "PACKAGE_DATADIR=\"${PACKAGE_DATADIR}\""
    PACKAGE_DATADIR=`echo "$PACKAGE_DATADIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_DATADIR], ["${PACKAGE_DATADIR}"], [Define to
	the location of the datadir.])
    eval "PACKAGE_SYSCONFDIR=\"${sysconfdir}\""
    eval "PACKAGE_SYSCONFDIR=\"${PACKAGE_SYSCONFDIR}\""
    eval "PACKAGE_SYSCONFDIR=\"${PACKAGE_SYSCONFDIR}\""
    PACKAGE_SYSCONFDIR=`echo "$PACKAGE_SYSCONFDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_SYSCONFDIR], ["${PACKAGE_SYSCONFDIR}"], [Define to
	the location of the sysconfdir.])
    eval "PACKAGE_SHAREDSTATEDIR=\"${sharedstatedir}\""
    eval "PACKAGE_SHAREDSTATEDIR=\"${PACKAGE_SHAREDSTATEDIR}\""
    eval "PACKAGE_SHAREDSTATEDIR=\"${PACKAGE_SHAREDSTATEDIR}\""
    PACKAGE_SHAREDSTATEDIR=`echo "$PACKAGE_SHAREDSTATEDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_SHAREDSTATEDIR], ["${PACKAGE_SHAREDSTATEDIR}"], [Define to
	the location of the sharedstatedir.])
    eval "PACKAGE_LOCALSTATEDIR=\"${localstatedir}\""
    eval "PACKAGE_LOCALSTATEDIR=\"${PACKAGE_LOCALSTATEDIR}\""
    eval "PACKAGE_LOCALSTATEDIR=\"${PACKAGE_LOCALSTATEDIR}\""
    PACKAGE_LOCALSTATEDIR=`echo "$PACKAGE_LOCALSTATEDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_LOCALSTATEDIR], ["${PACKAGE_LOCALSTATEDIR}"], [Define to
	the location of the localstatedir.])
    eval "PACKAGE_INCLUDEDIR=\"${includedir}\""
    eval "PACKAGE_INCLUDEDIR=\"${PACKAGE_INCLUDEDIR}\""
    eval "PACKAGE_INCLUDEDIR=\"${PACKAGE_INCLUDEDIR}\""
    PACKAGE_INCLUDEDIR=`echo "$PACKAGE_INCLUDEDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_INCLUDEDIR], ["${PACKAGE_INCLUDEDIR}"], [Define to
	the location of the includedir.])
    eval "PACKAGE_OLDINCLUDEDIR=\"${oldincludedir}\""
    eval "PACKAGE_OLDINCLUDEDIR=\"${PACKAGE_OLDINCLUDEDIR}\""
    eval "PACKAGE_OLDINCLUDEDIR=\"${PACKAGE_OLDINCLUDEDIR}\""
    PACKAGE_OLDINCLUDEDIR=`echo "$PACKAGE_OLDINCLUDEDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_OLDINCLUDEDIR], ["${PACKAGE_OLDINCLUDEDIR}"], [Define to
	the location of the oldincludedir.])
    eval "PACKAGE_DOCDIR=\"${docdir}\""
    eval "PACKAGE_DOCDIR=\"${PACKAGE_DOCDIR}\""
    eval "PACKAGE_DOCDIR=\"${PACKAGE_DOCDIR}\""
    PACKAGE_DOCDIR=`echo "$PACKAGE_DOCDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_DOCDIR], ["${PACKAGE_DOCDIR}"], [Define to
			the location of the docdir.])
    eval "PACKAGE_INFODIR=\"${infodir}\""
    eval "PACKAGE_INFODIR=\"${PACKAGE_INFODIR}\""
    eval "PACKAGE_INFODIR=\"${PACKAGE_INFODIR}\""
    PACKAGE_INFODIR=`echo "$PACKAGE_INFODIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_INFODIR], ["${PACKAGE_INFODIR}"], [Define to
	the location of the infodir.])
    eval "PACKAGE_HTMLDIR=\"${htmldir}\""
    eval "PACKAGE_HTMLDIR=\"${PACKAGE_HTMLDIR}\""
    eval "PACKAGE_HTMLDIR=\"${PACKAGE_HTMLDIR}\""
    PACKAGE_HTMLDIR=`echo "$PACKAGE_HTMLDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_HTMLDIR], ["${PACKAGE_HTMLDIR}"], [Define to
	the location of the htmldir.])
    eval "PACKAGE_DVIDIR=\"${dvidir}\""
    eval "PACKAGE_DVIDIR=\"${PACKAGE_DVIDIR}\""
    eval "PACKAGE_DVIDIR=\"${PACKAGE_DVIDIR}\""
    PACKAGE_DVIDIR=`echo "$PACKAGE_DVIDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_DVIDIR], ["${PACKAGE_DVIDIR}"], [Define to
	the location of the dvidir.])
    eval "PACKAGE_PDFDIR=\"${pdfdir}\""
    eval "PACKAGE_PDFDIR=\"${PACKAGE_PDFDIR}\""
    eval "PACKAGE_PDFDIR=\"${PACKAGE_PDFDIR}\""
    PACKAGE_PDFDIR=`echo "$PACKAGE_PDFDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_PDFDIR], ["${PACKAGE_PDFDIR}"], [Define to
	the location of the pdfdir.])
    eval "PACKAGE_PSDIR=\"${psdir}\""
    eval "PACKAGE_PSDIR=\"${PACKAGE_PSDIR}\""
    eval "PACKAGE_PSDIR=\"${PACKAGE_PSDIR}\""
    PACKAGE_PSDIR=`echo "$PACKAGE_PSDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_PSDIR], ["${PACKAGE_PSDIR}"], [Define to
	the location of the psdir.])
    eval "PACKAGE_LIBDIR=\"${libdir}\""
    eval "PACKAGE_LIBDIR=\"${PACKAGE_LIBDIR}\""
    eval "PACKAGE_LIBDIR=\"${PACKAGE_LIBDIR}\""
    PACKAGE_LIBDIR=`echo "$PACKAGE_LIBDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_LIBDIR], ["${PACKAGE_LIBDIR}"], [Define to
	the location of the libdir.])
    eval "PACKAGE_LIB32DIR=\"${lib32dir}\""
    eval "PACKAGE_LIB32DIR=\"${PACKAGE_LIB32DIR}\""
    eval "PACKAGE_LIB32DIR=\"${PACKAGE_LIB32DIR}\""
    PACKAGE_LIB32DIR=`echo "$PACKAGE_LIB32DIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_LIB32DIR], ["${PACKAGE_LIB32DIR}"], [Define to
	the location of the lib32dir.])
    eval "PACKAGE_LOCALEDIR=\"${localedir}\""
    eval "PACKAGE_LOCALEDIR=\"${PACKAGE_LOCALEDIR}\""
    eval "PACKAGE_LOCALEDIR=\"${PACKAGE_LOCALEDIR}\""
    PACKAGE_LOCALEDIR=`echo "$PACKAGE_LOCALEDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_LOCALEDIR], ["${PACKAGE_LOCALEDIR}"], [Define to
	the location of the localedir.])
    eval "PACKAGE_MANDIR=\"${mandir}\""
    eval "PACKAGE_MANDIR=\"${PACKAGE_MANDIR}\""
    eval "PACKAGE_MANDIR=\"${PACKAGE_MANDIR}\""
    PACKAGE_MANDIR=`echo "$PACKAGE_MANDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_MANDIR], ["${PACKAGE_MANDIR}"], [Define to
	the location of the mandir.])
    eval "PACKAGE_INITRDDIR=\"${initrddir}\""
    eval "PACKAGE_INITRDDIR=\"${PACKAGE_INITRDDIR}\""
    eval "PACKAGE_INITRDDIR=\"${PACKAGE_INITRDDIR}\""
    PACKAGE_INITRDDIR=`echo "$PACKAGE_INITRDDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_INITRDDIR], ["${PACKAGE_INITRDDIR}"], [Define to
	the location of the initrddir.])
    eval "PACKAGE_CONFIGDIR=\"${configdir}\""
    eval "PACKAGE_CONFIGDIR=\"${PACKAGE_CONFIGDIR}\""
    eval "PACKAGE_CONFIGDIR=\"${PACKAGE_CONFIGDIR}\""
    PACKAGE_CONFIGDIR=`echo "$PACKAGE_CONFIGDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_CONFIGDIR], ["${PACKAGE_CONFIGDIR}"], [Define to
	the location of the configdir.])
    eval "PACKAGE_KMODULEDIR=\"${kmoduledir}\""
    eval "PACKAGE_KMODULEDIR=\"${PACKAGE_KMODULEDIR}\""
    eval "PACKAGE_KMODULEDIR=\"${PACKAGE_KMODULEDIR}\""
    PACKAGE_KMODULEDIR=`echo "$PACKAGE_KMODULEDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
    AC_DEFINE_UNQUOTED([PACKAGE_KMODULEDIR], ["${PACKAGE_KMODULEDIR}"], [Define to
	the location of the kmoduledir.])
    eval "PACKAGE_MODUTILDIR=\"${modutildir}\""
    eval "PACKAGE_MODUTILDIR=\"${PACKAGE_MODUTILDIR}\""
    eval "PACKAGE_MODUTILDIR=\"${PACKAGE_MODUTILDIR}\""
    PACKAGE_MODUTILDIR=`echo "$PACKAGE_MODUTILDIR" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g"`
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
			cat "$cache_file" | egrep "^(test \"\\\[$]{)?(ac_cv_|acl_cv_|am_cv_|ap_cv_|gl_cv_|gt_cv_|lt_cv_|nls_cv_)" > "$config_site" 2>/dev/null
			#cat "$cache_file" | egrep "^(test \"\[$]{)?(ac_cv_|am_cv_|ap_cv_|lt_cv_)" > "$config_site" 2>/dev/null
			#cat "$cache_file" | egrep '^(ac_cv_|am_cv_|ap_cv_|lt_cv_)' | egrep -v '^(ac_cv_env_|ac_cv_host|ac_cv_target)' > "$config_site" 2>/dev/null
			#cat "$cache_file" | egrep -v '\<(ac_cv_env_|ac_cv_host|ac_cv_target|linux_cv_|ksyms_cv_|rpm_cv_|deb_cv_|strconf_cv_|sctp_cv_|xns_cv_|lis_cv_|lfs_cv_|strcomp_cv_|streams_cv_|xti_cv_|xopen_cv_|inet_cv_|xnet_cv_|devfs_cv_|init_cv_|pkg_cv_)' > "$config_site" 2>/dev/null
		    fi
		fi
	    done
	fi], [cache_file="$cache_file" ; CONFIG_SITE="$CONFIG_SITE"])
])# _OPENSS7_CACHE
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS], [dnl
    AC_MSG_NOTICE([+--------------------------+])
    AC_MSG_NOTICE([| OpenSS7 Packaging Checks |])
    AC_MSG_NOTICE([+--------------------------+])
    _OPENSS7_OPTIONS_CHECK
    _OPENSS7_OPTIONS_DOCS
    _OPENSS7_OPTIONS_GPG
    _OPENSS7_OPTIONS_PKG_EPOCH
    _OPENSS7_OPTIONS_PKG_MAJOR_MINOR
    _OPENSS7_OPTIONS_PKG_RELEASE
    _OPENSS7_OPTIONS_PKG_PATCHLEVEL
    _OPENSS7_OPTIONS_PKG_DISTDIR
    _OPENSS7_OPTIONS_PKG_REPODIR
    _OPENSS7_OPTIONS_PKG_TARDIR
    _OPENSS7_OPTIONS_PKG_ARCH
    _OPENSS7_OPTIONS_PKG_INDEP
    _OPENSS7_OPTIONS_PKG_TOOLS
    _OPENSS7_OPTIONS_PKG_MODULES
])# _OPENSS7_OPTIONS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_CHECK
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_CHECK], [dnl
    AC_MSG_CHECKING([for preinstall tests on check])
    AC_ARG_ENABLE([checks],
	[AS_HELP_STRING([--disable-checks],
	    [preinstall checks @<:@default=enabled@:>@])])
    AC_MSG_RESULT([${enable_checks:-yes}])
    AM_CONDITIONAL([PERFORM_CHECKS], [test :"${enable_checks:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_CHECK
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_DOCS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_DOCS], [dnl
    AC_MSG_CHECKING([for documentation included])
    AC_ARG_ENABLE([docs],
	[AS_HELP_STRING([--disable-docs],
	    [documentation @<:@default=enabled@:>@])])
    AC_MSG_RESULT([${enable_docs:-yes}])
    AM_CONDITIONAL([DOCUMENTATION], [test :"${enable_docs:-yes}" = :yes])dnl
    AC_MSG_CHECKING([for documentation distributed])
    AC_ARG_ENABLE([distribute-docs],
	[AS_HELP_STRING([--enable-distribute-docs],
	    [distribute pre-built documentation @<:@default=disabled@:>@])])
    AC_MSG_RESULT([${enable_distribute_docs:-no}])
    AM_CONDITIONAL([DISTRIBUTE_DOCS], [test :"${enable_distribute_docs:-no}" = :yes])dnl
])# _OPENSS7_OPTIONS_DOCS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_GPG
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_GPG], [dnl
    tmp_PATH="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    _BLD_VAR_PATH_PROG([GPG], [gpg pgp], [$tmp_PATH], [GPG signature command. @<:@default=gpg@:>@], [dnl
	GPG=/usr/bin/gpg
	AC_MSG_WARN([Cannot find 'gpg' program in PATH.])])
dnl ---------------------------------------------------------
    AC_MSG_CHECKING([for gpg user])
    AC_ARG_VAR([GNUPGUSER], [GPG user name. @<:@default=auto@:>@])
    AC_ARG_WITH([gpg-user],
	[AS_HELP_STRING([--with-gpg-user=USERNAME],
	    [USER for signing DEBs, RPMs and tarballs @<:@default=${GNUPGUSER}@:>@])], [], [dnl
	    with_gpg_user="$GNUPGUSER"
	    if test :"${with_gpg_user:-no}" = :no ; then
		if test -r .gnupguser; then d= ; else d="$srcdir/" ; fi
		if test -r ${d}.gnupguser
		then with_gpg_user="`cat ${d}.gnupguser`"
		else with_gpg_user=''
		fi
dnl		if test :"${with_gpg_user:-no}" = :no ; then
dnl		    with_gpg_user="`whoami`"
dnl		fi
	    fi])
    GNUPGUSER="${with_gpg_user:-`whoami`}"
    AC_MSG_RESULT([${GNUPGUSER:-no}])
dnl ---------------------------------------------------------
    AC_MSG_CHECKING([for gpg home])
    AC_ARG_VAR([GNUPGHOME], [GPG home directory. @<:@default=auto@:>@])
    AC_ARG_WITH([gpg-home],
	[AS_HELP_STRING([--with-gpg-home=HOMEDIR],
	    [HOME for signing DEBs, RPMs and tarballs @<:@default=${GNUPGHOME:-~/.gnupg}@:>@])], [], [dnl
	    with_gpg_home="$GNUPGHOME"
	    if test :"${with_gpg_home:-no}" = :no ; then
		if test -r .gnupghome; then d= ; else d="$srcdir/" ; fi
		if test -r ${d}.gnupghome
		then with_gpg_home="`cat ${d}.gnupghome`"
		else with_gpg_home=''
		fi
dnl		if test :"${with_gpg_home:-no}" = :no ; then
dnl		    with_gpg_home='~/.gnupg'
dnl		fi
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
	[AS_HELP_STRING([--with-pkg-epoch=EPOCH],
	    [EPOCH of the package @<:@default=auto@:>@])], [], [dnl
	    if test -r .pkgepoch; then d= ; else d="$srcdir/" ; fi
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
# _OPENSS7_OPTIONS_PKG_MAJOR_MINOR
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_MAJOR_MINOR], [dnl
    AC_MSG_CHECKING([for pkg major])
    PACKAGE_MAJOR=`echo "$PACKAGE_VERSION" | sed -r 's,[[^[:digit:]]].*,,'`
    PACKAGE_MAJOR="${PACKAGE_MAJOR:-1}"
    AC_SUBST([PACKAGE_MAJOR])dnl
    AC_MSG_RESULT([$PACKAGE_MAJOR])
    AC_MSG_CHECKING([for pkg minor])
    PACKAGE_MINOR=`echo "$PACKAGE_VERSION" | sed -r 's,^[[[:digit:]]]+[[^[:digit:]]]+,,'`
    PACKAGE_MINOR="${PACKAGE_MINOR:-1}"
    AC_MSG_RESULT([$PACKAGE_MINOR])
    AC_SUBST([PACKAGE_MINOR])dnl
    AC_MSG_CHECKING([for pkg revision])
    PACKAGE_REVISION=`echo"$PACKAGE_RELEASE" | sed -r 's,[[^[:digit:]]].*,,'`
    PACKAGE_REVISION="${PACKAGE_REVISION:-1}"
    AC_MSG_RESULT([$PACKAGE_REVISION])
    AC_SUBST([PACKAGE_REVISION])
    AC_MSG_CHECKING([for pkg version number])
    PACKAGE_VERNUMB="$PACKAGE_MAJOR:$PACKAGE_MINOR:$PACKAGE_REVISION"
    AC_MSG_RESULT([$PACKAGE_VERNUMB])
    AC_SUBST([PACKAGE_VERNUMB])
])# _OPENSS7_OPTIONS_PKG_MAJOR_MINOR
# =========================================================================

# =========================================================================
# _OPENSS7_OPTIONS_PKG_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_RELEASE], [dnl
    AC_MSG_CHECKING([for pkg release])
    AC_ARG_WITH([pkg-release],
	[AS_HELP_STRING([--with-pkg-release=RELEASE],
	    [RELEASE of the package @<:@default=auto@:>@])], [], [dnl
	    if test -r .pkgrelease ; then d= ; else d="$srcdir/" ; fi
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
# _OPENSS7_OPTIONS_PKG_PATCHLEVEL
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_PATCHLEVEL], [dnl
    AC_MSG_CHECKING([for pkg patch level])
    AC_ARG_WITH([pkg-patchlevel],
	[AS_HELP_STRING([--with-pkg-patchlevel=@<:@PATCHLEVEL@:>@],
	    [PATCHLEVEL of the package @<:@default=date@:>@])], [dnl
	    if test :$with_pkg_patchlevel = :no
	    then with_pkg_patchlevel=
	    fi
	    if test :$with_pkg_patchlevel = :
	    then with_pkg_patchlevel=`date -uI | sed -e 's,-,,g'`
	    fi], [dnl
	    if test -r .pkgpatchlevel ; then d= ; else d="$srcdir/" ; fi
	    if test -r ${d}.pkgpatchlevel
	    then with_pkg_patchlevel="`cat ${d}.pkgpatchlevel`"
	    else with_pkg_patchlevel=
	    fi])
    AC_MSG_RESULT([$with_pkg_patchlevel])
    PACKAGE_PATCHLEVEL=${with_pkg_patchlevel:+.$with_pkg_patchlevel}
    AC_SUBST([PACKAGE_PATCHLEVEL])
    AC_DEFINE_UNQUOTED([PACKAGE_PATCHLEVEL], ["$PACKAGE_PATCHLEVEL"], [The
	Package Patch Level.  This defaults to null.])
    if test -n "$PACKAGE_PATCHLEVEL"; then
	# drop the release number for patches
	if echo "$PACKAGE_RELEASE" | egrep '^[0-9][0-9]*$' >/dev/null 2>&1; then
	    PACKAGE_RELEASE=$((PACKAGE_RELEASE-1))
	else
	    PACKAGE_RELEASE=`echo $PACKAGE_RELEASE | sed -e 'y,ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,0ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxy,'`
	fi
    fi
])# _OPENSS7_OPTIONS_PKG_PATCHLEVEL
# =========================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_DISTDIR
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_DISTDIR], [dnl
    AC_ARG_WITH([pkg-distdir],
	[AS_HELP_STRING([--with-pkg-distdir=DIR],
	    [package distribution directory @<:@default=.@:>@])],
	[], [with_pkg_distdir=`pwd`/repo])
    AC_MSG_CHECKING([for pkg distdir])
    if test ":${DISTDIR+set}" != :set ; then
	case :"${with_pkg_distdir:-no}" in
	    (:no|:yes) DISTDIR=`pwd`/repo ;;
	    (*)        DISTDIR="$with_pkg_distdir" ;;
	esac
    fi
    AC_MSG_RESULT([$DISTDIR])
    AC_SUBST([DISTDIR])dnl
])# _OPENSS7_OPTIONS_PKG_DISTDIR
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_REPODIR
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_REPODIR], [dnl
    AC_ARG_WITH([pkg-repodir],
	[AS_HELP_STRING([--with-pkg-repodir=DIR],
	    [package repository directory @<:@default=DISTDIR@:>@])],
	[], [with_pkg_repodir='${DISTDIR}'])
    AC_MSG_CHECKING([for pkg repodir])
    if test ":${REPODIR+set}" != :set ; then
	case :"${with_pkg_repodir:-no}" in
	    (:no|:yes) REPODIR='${DISTDIR}' ;;
	    (*)        REPODIR="$with_pkg_repodir" ;;
	esac
    fi
    AC_MSG_RESULT([$REPODIR])
    AC_SUBST([REPODIR])dnl
])# _OPENSS7_OPTIONS_PKG_REPODIR
# =========================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_TARDIR
# -----------------------------------------------------------------------------
# When building in place the default is simply the current directory, whereas
# when releasing to another directory, the default is based on the repo
# subdirectory name (tarballs).
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_TARDIR], [dnl
    AC_ARG_WITH([pkg-tardir],
	[AS_HELP_STRING([--with-pkg-tardir=DIR],
	    [tarball directory @<:@default=PKG-DISTDIR/tarballs@:>@])],
	[], [with_pkg_tardir='${DISTDIR}/tarballs'])
    AC_MSG_CHECKING([for pkg tardir])
    if test ":${tardir+set}" != :set ; then
	case :"${with_pkg_tardir:-no}" in
	    (:no|:yes)	tardir='${DISTDIR}/tarballs'	;;
	    (*)		tardir="$with_pkg_tardir"	;;
	esac
    fi
    AC_MSG_RESULT([$tardir])
    AC_SUBST([tardir])dnl
])# _OPENSS7_OPTIONS_PKG_TARDIR
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_ARCH
# -----------------------------------------------------------------------------
# Debian (and rpm for that matter) can build architecture dependent or
# architecture independent packages.  This option specifies whether architecture
# dependent packages are to be built and installed.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_ARCH], [dnl
    AC_MSG_CHECKING([for pkg build/install of arch packages])
    AC_ARG_ENABLE([arch],
	[AS_HELP_STRING([--disable-arch],
	    [arch packages @<:@default=enabled@:>@])])
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
    AC_MSG_CHECKING([for pkg build/install of indep packages])
    AC_ARG_ENABLE([indep],
	[AS_HELP_STRING([--disable-indep],
	    [indep packages @<:@default=enabled@:>@])])
    AC_MSG_RESULT([${enable_indep:-yes}])
    AM_CONDITIONAL([PKG_BUILD_INDEP], [test :"${enable_indep:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_PKG_INDEP
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_TOOLS
# -----------------------------------------------------------------------------
# The RPM spec file is set up for either building kernel dependent packages
# or kernel independent packages.  This option specifies whether kernel
# independent (user space) packages are to be built.  This option can also
# be used for general kernel independent builds.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_TOOLS], [dnl
    AC_MSG_CHECKING([for pkg build/install of user packages])
    AC_ARG_ENABLE([tools],
	[AS_HELP_STRING([--disable-tools],
	    [user packages @<:@default=enabled@:>@])],
	[enable_java=no], [enable_tools=yes])
    AC_MSG_RESULT([${enable_tools:-yes}])
    AM_CONDITIONAL([PKG_BUILD_USER], [test ":${enable_tools:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_PKG_TOOLS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_PKG_MODULES
# -----------------------------------------------------------------------------
# The RPM spec file is set up for either building kernel dependent packages
# or kernel independent packages.  This option specifies whether kernel
# dependent (kernel module) packages are to be built.  This option can also
# be used for general kernel dependent builds.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_PKG_MODULES], [dnl
    AC_MSG_CHECKING([for pkg build/install of kernel packages])
    AC_ARG_ENABLE([modules],
	[AS_HELP_STRING([--disable-modules],
	    [kernel packages @<:@default=enabled@:>@])],
	[], [enable_modules=yes])
    AC_MSG_RESULT([${enable_modules:-yes}])
    AM_CONDITIONAL([PKG_BUILD_KERNEL], [test ":${enable_modules:-yes}" = :yes])dnl
])# _OPENSS7_OPTIONS_PKG_MODULES
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_CFLAGS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_CFLAGS], [dnl
    AC_MSG_CHECKING([for user CFLAGS])
    AC_MSG_RESULT([${USER_CFLAGS} ${USER_DFLAGS} ${CFLAGS}])
    AC_MSG_CHECKING([for user CFLAGS])
    USER_CFLAGS="$CFLAGS${USER_CFLAGS:+ $USER_CFLAGS}"
    USER_DFLAGS=
    CFLAGS=
    case "${with_optimize:-auto}" in
	(size)
	    CFLAGS="-g -Os${CFLAGS:+ $CFLAGS}"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(speed)
	    CFLAGS="-g -O3${CFLAGS:+ $CFLAGS}"
	    CFLAGS="${CFLAGS:+$CFLAGS }-freorder-blocks"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(normal)
	    CFLAGS="-g -O2${CFLAGS:+ $CFLAGS}"
	    CFLAGS="${CFLAGS:+$CFLAGS }-freorder-blocks"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(quick)
	    CFLAGS="-g -O0${CFLAGS:+ $CFLAGS}"
	    CFLAGS="${CFLAGS:+$CFLAGS }-finline"
	    CFLAGS="${CFLAGS:+$CFLAGS }-fno-keep-inline-functions"
	    CFLAGS="${CFLAGS:+$CFLAGS }-fno-keep-static-consts"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?keep-inline-functions,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?keep-static-consts,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(auto|*)
	    os7_tmp=`echo " $USER_CFLAGS" | sed -r -n 's,.* (-g[[^[:space:]]]*).*,\1,p'`
	    CFLAGS="$CFLAGS${os7_tmp:+ $os7_tmp}"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed 's, -g[[^[:space:]]]*,,g'`
	    os7_tmp=`echo " $USER_CFLAGS" | sed -r -n 's,.* (-O[[0-9s]]*).*,\1,p'`
	    CFLAGS="$CFLAGS${os7_tmp:+ $os7_tmp}"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed 's, -O[[0-9s]]*,,g'`
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
    esac
    CFLAGS="${CFLAGS:+$CFLAGS }-pipe"
    USER_CFLAGS=`echo " $USER_CFLAGS" | sed 's, -pipe,,g'`
dnl USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -W(no-)?trigraphs,,g'`
dnl USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wno-trigraphs"
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's, -Wall,,g'`
	    USER_CFLAGS="-Wall${USER_CFLAGS:+ $USER_CFLAGS}"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -e 's% -Wp,-D_FORTIFY_SOURCE=[[0-9]]*%%g'`
	    USER_CFLAGS="-Wp,-D_FORTIFY_SOURCE=2${USER_CFLAGS:+ $USER_CFLAGS}"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wno-system-headers"
dnl	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -W(no-)?undef,,g'`
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wundef"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wno-endif-labels"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wbad-function-cast"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wcast-qual"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wcast-align"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wwrite-strings"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wconversion"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wsign-compare"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Waggregate-return"
dnl	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -W(no-)?strict-prototypes,,g'`
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wstrict-prototypes"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wmissing-prototypes"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wmissing-declarations"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wmissing-noreturn"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wmissing-format-attribute"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wpacked"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wpadded"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wredundant-decls"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wnested-externs"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wunreachable-code"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Winline"
dnl	    USER_CFLAGS="${USER_CFLAGS:+$USER_CFLAGS }-Wdisabled-optimization"
	    USER_CFLAGS=`echo " $USER_CFLAGS" | sed -r -e 's, -W(no-)?error,,g'`
	    USER_DFLAGS="${USER_DFLAGS:+$USER_DFLAGS }-Werror"
    fi
    USER_CFLAGS="${CFLAGS:$CFLAGS }$USER_CFLAGS"; CFLAGS=
    USER_CFLAGS=`echo "$USER_CFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    USER_DFLAGS=`echo "$USER_DFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    AC_MSG_RESULT([${USER_CFLAGS} ${USER_DFLAGS} ${CFLAGS}])
])# _OPENSS7_OPTIONS_CFLAGS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_LDFLAGS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_LDFLAGS], [dnl
    AC_MSG_CHECKING([for user LDFLAGS])
    AC_MSG_RESULT([${USER_LDFLAGS} ${LDFLAGS}])
    AC_CACHE_CHECK([for user build id], [os7_cv_buildid], [dnl
	if ${LD-ld} --build-id 2>&1 | grep 'unrecognized' >/dev/null 2>&1
	then
	    os7_cv_buildid=
	else
	    os7_cv_buildid='-Wl,--build-id'
	fi
    ])
    AC_MSG_CHECKING([for user LDFLAGS])
    USER_LDFLAGS="$LDFLAGS $USER_LDFLAGS $os7_cv_buildid"; LDFLAGS=
    USER_LDFLAGS=`echo "$USER_LDFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    AC_MSG_RESULT([${USER_LDFLAGS}])
])# _OPENSS7_OPTIONS_LDFLAGS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_CXXFLAGS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_CXXFLAGS], [dnl
    AC_MSG_CHECKING([for user CXXFLAGS])
    AC_MSG_RESULT([${USER_CXXFLAGS} ${USER_DXXFLAGS} ${CXXFLAGS}])
    AC_MSG_CHECKING([for user CXXFLAGS])
    USER_CXXFLAGS="$CXXFLAGS${USER_CXXFLAGS:+ $USER_CXXFLAGS}"
    USER_DXXFLAGS=
    CXXFLAGS=
    case "${with_optimize:-auto}" in
	(size)
	    CXXFLAGS="-g -Os${CXXFLAGS:+ $CXXFLAGS}"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(speed)
	    CXXFLAGS="-g -O3${CXXFLAGS:+ $CXXFLAGS}"
	    CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }-freorder-blocks"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(normal)
	    CXXFLAGS="-g -O2${CXXFLAGS:+ $CXXFLAGS}"
	    CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }-freorder-blocks"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(quick)
	    CXXFLAGS="-g -O0${CXXFLAGS:+ $CXXFLAGS}"
	    CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }-finline"
	    CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }-fno-keep-inline-functions"
	    CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }-fno-keep-static-consts"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?keep-inline-functions,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?keep-static-consts,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(auto|*)
	    os7_tmp=`echo " $USER_CXXFLAGS" | sed -r -n 's,.* (-g[[^[:space:]]]*).*,\1,p'`
	    CXXFLAGS="$CXXFLAGS${os7_tmp:+ $os7_tmp}"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed 's, -g[[^[:space:]]]*,,g'`
	    os7_tmp=`echo " $USER_CXXFLAGS" | sed -r -n 's,.* (-O[[0-9s]]*).*,\1,p'`
	    CXXFLAGS="$CXXFLAGS${os7_tmp:+ $os7_tmp}"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed 's, -O[[0-9s]]*,,g'`
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
    esac
    CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }-pipe"
    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed 's, -pipe,,g'`
dnl USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -W(no-)?trigraphs,,g'`
dnl USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wno-trigraphs"
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's, -Wall,,g'`
	    USER_CXXFLAGS="-Wall${USER_CXXFLAGS:+ $USER_CXXFLAGS}"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -e 's% -Wp,-D_FORTIFY_SOURCE=[[0-9]]*%%g'`
	    USER_CXXFLAGS="-Wp,-D_FORTIFY_SOURCE=2${USER_CXXFLAGS:+ $USER_CXXFLAGS}"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wno-system-headers"
dnl	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -W(no-)?undef,,g'`
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wundef"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wno-endif-labels"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wbad-function-cast"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wcast-qual"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wcast-align"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wwrite-strings"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wconversion"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wsign-compare"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Waggregate-return"
dnl	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -W(no-)?strict-prototypes,,g'`
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wstrict-prototypes"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wmissing-prototypes"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wmissing-declarations"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wmissing-noreturn"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wmissing-format-attribute"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wpacked"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wpadded"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wredundant-decls"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wnested-externs"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wunreachable-code"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Winline"
dnl	    USER_CXXFLAGS="${USER_CXXFLAGS:+$USER_CXXFLAGS }-Wdisabled-optimization"
	    USER_CXXFLAGS=`echo " $USER_CXXFLAGS" | sed -r -e 's, -W(no-)?error,,g'`
	    USER_DXXFLAGS="${USER_DXXFLAGS:+$USER_DXXFLAGS }-Werror"
    fi
    USER_CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }$USER_CXXFLAGS"; CXXFLAGS=
    CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    USER_CXXFLAGS=`echo "$USER_CXXFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    USER_DXXFLAGS=`echo "$USER_DXXFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    AC_MSG_RESULT([${USER_CXXFLAGS} ${USER_DXXFLAGS} ${CXXFLAGS}])
])# _OPENSS7_OPTIONS_CXXFLAGS
# =============================================================================

# =============================================================================
# _OPENSS7_OPTIONS_GCJFLAGS
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_OPTIONS_GCJFLAGS], [dnl
    AC_MSG_CHECKING([for user GCJFLAGS])
    AC_MSG_RESULT([${USER_GCJFLAGS} ${USER_GCDFLAGS} ${GCJFLAGS}])
    AC_MSG_CHECKING([for user GCJFLAGS])
    USER_GCJFLAGS="$GCJFLAGS${USER_GCJFLAGS:+ $USER_GCJFLAGS}"
    USER_GCDFLAGS=
    GCJFLAGS=
    case "${with_optimize:-auto}" in
	(size)
	    GCJFLAGS="-g -Os${GCJFLAGS:+ $GCJFLAGS}"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(speed)
	    GCJFLAGS="-g -O3${GCJFLAGS:+ $GCJFLAGS}"
	    GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-freorder-blocks"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(normal)
	    GCJFLAGS="-g -O2${GCJFLAGS:+ $GCJFLAGS}"
	    GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-freorder-blocks"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?inline-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?reorder-blocks,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?reorder-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?function-sections,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?unit-at-a-time,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(quick)
	    GCJFLAGS="-g -O0${GCJFLAGS:+ $GCJFLAGS}"
	    GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-finline"
	    GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-fno-keep-inline-functions"
	    GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-fno-keep-static-consts"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -g[[^[:space:]]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -O[[0-9s]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?keep-inline-functions,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?keep-static-consts,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
	(auto|*)
	    os7_tmp=`echo " $USER_GCJFLAGS" | sed -r -n 's,.* (-g[[^[:space:]]]*).*,\1,p'`
	    GCJFLAGS="$GCJFLAGS${os7_tmp:+ $os7_tmp}"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed 's, -g[[^[:space:]]]*,,g'`
	    os7_tmp=`echo " $USER_GCJFLAGS" | sed -r -n 's,.* (-O[[0-9s]]*).*,\1,p'`
	    GCJFLAGS="$GCJFLAGS${os7_tmp:+ $os7_tmp}"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed 's, -O[[0-9s]]*,,g'`
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -f(no-)?strict-aliasing,,g'`
	    ;;
    esac
    GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-pipe"
    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed 's, -pipe,,g'`
dnl USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -W(no-)?trigraphs,,g'`
dnl USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wno-trigraphs"
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's, -Wall,,g'`
	    USER_GCJFLAGS="-Wall${USER_GCJFLAGS:+ $USER_GCJFLAGS}"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -e 's% -Wp,-D_FORTIFY_SOURCE=[[0-9]]*%%g'`
	    USER_GCJFLAGS="-Wp,-D_FORTIFY_SOURCE=2${USER_GCJFLAGS:+ $USER_GCJFLAGS}"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wno-system-headers"
dnl	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -W(no-)?undef,,g'`
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wundef"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wno-endif-labels"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wbad-function-cast"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wcast-qual"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wcast-align"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wwrite-strings"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wconversion"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wsign-compare"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Waggregate-return"
dnl	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -W(no-)?strict-prototypes,,g'`
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wstrict-prototypes"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wmissing-prototypes"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wmissing-declarations"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wmissing-noreturn"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wmissing-format-attribute"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wpacked"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wpadded"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wredundant-decls"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wnested-externs"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wunreachable-code"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Winline"
dnl	    USER_GCJFLAGS="${USER_GCJFLAGS:+$USER_GCJFLAGS }-Wdisabled-optimization"
	    USER_GCJFLAGS=`echo " $USER_GCJFLAGS" | sed -r -e 's, -W(no-)?error,,g'`
	    USER_GCDFLAGS="${USER_GCDFLAGS:+$USER_GCDFLAGS }-Werror"
    fi
    USER_GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }$USER_GCJFLAGS"; GCJFLAGS=
    GCJFLAGS=`echo "$GCJFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    USER_GCJFLAGS=`echo "$USER_GCJFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    USER_GCDFLAGS=`echo "$USER_GCDFLAGS" | sed -e 's,^[[[:space:]]]*,,;s,[[[:space:]]]*$,,;s,[[[:space:]]][[[:space:]]]*, ,g'`
    AC_MSG_RESULT([${USER_GCJFLAGS} ${USER_GCDFLAGS} ${GCJFLAGS}])
])# _OPENSS7_OPTIONS_GCJFLAGS
# =============================================================================

# =============================================================================
# _OPENSS7_MISSING2
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_MISSING2], [dnl
    test x"${MISSING2+set}" = xset || MISSING2="\${SHELL} $am_aux_dir/missing2"
    if eval "$MISSING2 --run true" ; then
	am_missing2_run="$MISSING2 "
    else
	am_missing2_run=
	AC_MSG_WARN(['missing2' script is too old or missing])
    fi
])# _OPENSS7_MISSING2
# =============================================================================

# =============================================================================
# _OPENSS7_MISSING3
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_MISSING3], [dnl
    test x"${MISSING3+set}" = xset || MISSING3="\${SHELL} $am_aux_dir/missing3"
    if eval "$MISSING3 --run true" ; then
	am_missing3_run="$MISSING3 "
    else
	am_missing3_run=
	AC_MSG_WARN(['missing3' script is too old or missing])
    fi
])# _OPENSS7_MISSING3
# =============================================================================

# =============================================================================
# _OPENSS7_MISSING4
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_MISSING4], [dnl
    test x"${MISSING4+set}" = xset || MISSING4="\${SHELL} $am_aux_dir/missing4"
    if eval "$MISSING4 --run true" ; then
	am_missing4_run="$MISSING4 --run "
    else
	am_missing4_run=
	AC_MSG_WARN(['missing4' script is too old or missing])
    fi
])# _OPENSS7_MISSING4
# =============================================================================

# =============================================================================
# _OPENSS7_UPDATE
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_UPDATE], [dnl
    AC_CACHE_CHECK([for pkg update stamp], [pkg_cv_update_stamp], [dnl
	pkg_cv_update_stamp="`pwd`/update-stamp"
    ])
    USTAMP="$pkg_cv_update_stamp"
    AC_SUBST([USTAMP])dnl
])# _OPENSS7_UPDATE
# =============================================================================

# =============================================================================
# _OPENSS7
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7], [dnl
])# _OPENSS7
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
