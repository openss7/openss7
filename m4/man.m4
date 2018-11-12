# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) File: m4/man.m4
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

# =========================================================================
# _MAN_CONVERSION
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION], [dnl
    _MAN_CONVERSION_ARGS
    if test :${enable_docs:-yes} = :yes ; then
	AC_MSG_NOTICE([+------------------------+])
	AC_MSG_NOTICE([| Manual Page Generation |])
	AC_MSG_NOTICE([+------------------------+])
	_MAN_CONVERSION_SETUP
	_MAN_CONVERSION_OPTIONS
    fi
    _MAN_CONVERSION_OUTPUT
])# _MAN_CONVERSION
# =========================================================================

# =========================================================================
# _MAN_CONVERSION_ARGS
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION_ARGS], [dnl
    AC_ARG_WITH([cooked-manpages],
	[AS_HELP_STRING([--with-cooked-manpages],
	    [convert manual pages to simpler form @<:@default=no@:>@])],
	[], [with_cooked_manpages=no])
    without_cooked_manpages=
    AC_ARG_ENABLE([compress-manpages],
	[AS_HELP_STRING([--disable-compress-manpages],
	    [compress manpages @<:@default=enabled@:>@])],
	[], [enable_compress_manpages=yes])
    disable_compress_manpages=
])# _MAN_CONVERSION_ARGS
# =========================================================================

# =========================================================================
# _MAN_CONVERSION_SETUP
# -------------------------------------------------------------------------
# The tools are:
#
#   $SOELIM - absolute requirement
#   $REFER - absolute requirement
#   $PIC - absolute requirement
#   $TBL - absolute requirement
#   $NROFF - required, but GROFF can be used instead
#   $GROFF - required
#   $MAKEWHATIS - installation tool
#
#   $GZIP_CMD $GZIP - compression option
#   $BZIP2_CMD $BZIP - compression option
#   $LZMA_CMD $LZMA - compresssion option
#
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION_SETUP], [dnl
    tmp_PATH="${PATH:+$PATH:}/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:$am_aux_dir";
    _BLD_VAR_PATH_PROG([SOELIM], [gsoelim soelim], [$tmp_PATH],
	[Roff source elimination command. @<:@default=gsoelim,soelim@:>@], [dnl
	if test :$with_cooked_manpages != :no ; then
	    without_cooked_manpages=yes
	    _BLD_INSTALL_WARN([SOELIM], [
***
*** Configure cannot find a suitable 'soelim' program.  Generating
*** cooked manual pages requires the 'soelim' program from the 'groff'
*** package.  You can normally get 'groff' as part of most poplular
*** Linux distributions and all current versions are acceptable.  The
*** 'groff' package has been available for many years and is available
*** on the web from any GNU archive site.  Use the following commands to
*** obtain 'groff':
*** ], [
*** Debian 5.0:  'aptitude install groff-base'
*** Ubuntu 8.04: 'aptitude install groff'
*** CentOS 5.x:  'yum install groff'
*** RHEL 5.x:    'yum install groff'
*** SLES 10:     'zypper install groff'
*** Mandriva:    'urpmi groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify an
*** appropriate program with the SOELIM environment variable to
*** 'configure', or specify the --without-cooked-manpages option to
*** 'configure'.
*** ])
	fi])
    _BLD_VAR_PATH_PROG([REFER], [grefer refer], [$tmp_PATH],
	[Roff references command. @<:@default=grefer,refer@:>@], [dnl
	if test :$with_cooked_manpages != :no ; then
	    without_cooked_manpages=yes
	    _BLD_INSTALL_WARN([REFER], [
***
*** Configure cannot find a suitable 'refer' program.  Generating cooked
*** manual pages requires the 'refer' program from the 'groff' package.
*** You can normally get 'groff' as part of most poplular Linux
*** distributions and all current versions are acceptable.  The 'groff'
*** package has been available for many years and is available on the
*** web from any GNU archive site.  Use the following commands to obtain
*** 'groff':
*** ], [
*** Debian 5.0:  'aptitude install groff'
*** Ubuntu 8.04: 'aptitude install groff_ext'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'
*** Mandriva:    'urpmi groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify an
*** appropriate program with the REFER environment variable to
*** 'configure', or specify the --without-cooked-manpages option to
*** 'configure'.
*** ])
	fi])
    _BLD_VAR_PATH_PROG([PIC], [gpic pic], [$tmp_PATH],
	[Roff picture command. @<:@default=gpic,pic@:>@], [dnl
	if test :$with_cooked_manpages != :no ; then
	    # without_cooked_manpages=yes
	    : # do not need pic to cook manpages
	fi])
    _BLD_VAR_PATH_PROG([TBL], [gtbl tbl], [$tmp_PATH],
	[Roff table command. @<:@default=gtbl,tbl@:>@], [dnl
	if test :$with_cooked_manpages != :no ; then
	    without_cooked_manpages=yes
	    _BLD_INSTALL_WARN([TBL], [
***
*** Configure cannot find a suitable 'tbl' program.  Generating cooked
*** manual pages requires the 'tbl' program from the 'groff' package.
*** You can normally get 'groff' as part of most poplular Linux
*** distributions and all current versions are acceptable.  The 'groff'
*** package has been available for many years and is available on the
*** web from any GNU archive site.  Use the following commands to obtain
*** 'groff':
*** ], [
*** Debian 5.0:  'aptitude install groff-base'
*** Ubuntu 8.04: 'aptitude install groff'
*** CentOS 5.x:  'yum install groff'
*** RHEL 5.x:    'yum install groff'
*** SLES 10:     'zypper install groff'
*** Mandriva:    'urpmi groff-for-man'], [
***
*** To get rid of this warning, load the 'groff' package, specify an
*** appropriate program with the TBL environment variable to
*** 'configure', or specify the --without-cooked-manpages option to
*** 'configure'.
*** ])
	fi])
    _BLD_VAR_PATH_PROG([MAKEWHATIS], [mandb makewhatis], [$tmp_PATH],
	[Makewhatis command. @<:@default=mandb,makewhatis@:>@], [dnl
	_BLD_INSTALL_WARN([MAKEWHATIS], [
***
*** Configure cannot find a suitable 'makewhatis' program.  Installing
*** manual pages requires the 'mandb' or 'makewhatis' program from the
*** 'man' package.  You can normally get 'man' as part of most populate
*** Linux distributions and all current versions are acceptable.  The
*** 'man' package has been available for many years and is available on
*** the web from the Linux Documentation Project.  Use the following
*** commands to obtain 'man':
*** ], [
*** Debian 5.0:  'aptitude install man-db'
*** Ubuntu 8.04: 'aptitude install man'
*** Fedora 9:    'yum install man'
*** CentOS 5.x:  'yum install man'
*** openSUSE 11: 'zypper install man'
*** SLES 10:     'zypper install man'
*** Mandriva:    'urpmi man'], [
***
*** To get rid of this warning, load the 'man' package, specify an
*** appropriate program with the MAKEWHATIS environment variable to
*** 'configure'.
*** ])], [dnl
	tmp_program=`echo "$MAKEWHATIS" | sed 's,.*[[^ ][^$-]],,;s,.*/,,;s, *$,,'`
	if test :"$tmp_program" = :mandb ; then
	    MANDBFLAGS='-u'
	elif test :"$tmp_program" = :makewhatis ; then
	    MANDBFLAGS='-v -u -w'
	else
	    MANDBFLAGS=
	fi])
    AC_ARG_VAR([GZIP], [Gzip default compression options @<:@default=-f9@:>@])
    test -n "$GZIP" || GZIP='-f9'
    _BLD_VAR_PATH_PROG([GZIP_CMD], [gzip], [$tmp_PATH],
	[Gzip compression command @<:@default=gzip@:>@], [dnl
	_BLD_INSTALL_WARN([GZIP_CMD], [
***
*** Configure cannot find a suitable 'gzip' program.  Compressing manual
*** pages may require the 'gzip' program from the 'gzip' package on the
*** build host.  The 'gzip' package has been available for many years on
*** all distributions and is available from any GNU archive site.  Try:
*** ], [
*** Debian:   'aptitude install gzip'
*** SuSE:     'zypper install gzip'
*** CentOS:   'yum install gzip'
*** RedHat:   'yum install gzip'
*** Mandriva: 'urpmi gzip'], [
***
*** To get rid of this warning, load the 'gzip' package, or specify the
*** location with the GZIP_CMD environment variable to 'configure'.
*** ])])
    AC_ARG_VAR([BZIP2], [Bzip2 default compression options @<:@default=-f9@:>@])
    test -n "$BZIP2" || BZIP2='-f9'
    _BLD_VAR_PATH_PROG([BZIP2_CMD], [bzip2], [$tmp_PATH],
	[Bzip2 compression command @<:@default=bzip2@:>@], [dnl
	_BLD_INSTALL_WARN([BZIP2_CMD], [
***
*** Configure cannot find a suitable 'bzip2' program.  Compressing
*** manual pages may require the 'bzip2' program from the 'bzip2'
*** package on the build host.  The 'bzip2' package has been available
*** for many years on all distributions and is available from various
*** web sources.  Try:
*** ], [
*** Debian:   'aptitude install bzip2'
*** SuSE:     'zypper install bzip2'
*** CentOS:   'yum install bzip2'
*** RedHat:   'yum install bzip2'
*** Mandriva: 'urpmi bzip2'], [
***
*** To get rid of this warning, load the 'bzip2' package, or specify the
*** location with the BZIP2_CMD environment variable to 'configure'.
*** ])])
    AC_ARG_VAR([LZMA], [Lzma default compression options @<:@default=-f9@:>@])
    test -n "$LZMA" || LZMA='-f9'
    _BLD_VAR_PATH_PROG([LZMA_CMD], [lzma], [$tmp_PATH],
	[Lzma compression command @<:@default=lzma@:>@], [dnl
	_BLD_INSTALL_WARN([LZMA_CMD], [
***
*** Configure cannot find a suitable 'lzma' program.  Compressing
*** archives may require the 'lzma' program from the 'lzam' package on
*** the build host.  The 'lzma' package has been available for a number
*** of years on recent distributions and is available from various web
*** sources for others.  Try:
*** ], [
*** Debian 5.0:  'aptitude install lzma'
*** SuSE:        'zypper install lmza'
*** CentOS:      'yum install lzma'
*** CentOS:      'yum install xz-lzma-compat'
*** Mandriva:    'urpmi xz'], [
***
*** To get rid of this warning, load the 'lzma' package or specify the
*** location with the LZMA_CMD environment variable to 'configure'.
*** ])])
    AC_ARG_VAR([XZ], [Xz default compression options @<:@default=-f9@:>@])
    test -n "$XZ" || XZ='-f9'
    _BLD_VAR_PATH_PROG([XZ_CMD], [xz], [$tmp_PATH],
	[Xz compression command @<:@default=xz@:>@], [dnl
	_BLD_INSTALL_WARN([XZ_CMD], [
*** 
*** Configure cannot find a suitable 'xz' program.  Compressing archives
*** may require the 'xz' program from the 'xz' package on the build
*** host.  The 'xz' package has been available for a number of years on
*** recent distributions and is available from various web sources for
*** others.  Try:
*** ], [
*** Debian 5.0: 'aptitude install xz-utils'
*** SuSE:       'zypper install xz'
*** CentOS:     'yum install xz'
*** Mandriva:   'urpmi xz'], [
***
*** To get rid of this warning, load the 'xz' package or specify the
*** location with the XZ_CMD environment variable to 'configure'.
*** ])])
])# _MAN_CONVERSION_SETUP
# =========================================================================

# =========================================================================
# _MAN_CONVERSION_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION_OPTIONS], [dnl
    AC_MSG_CHECKING([for manpage conversion])
    if test :$without_cooked_manpages = :yes ; then
	with_cooked_manpages=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_cooked_manpages --without-cooked-manpages\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-cooked-manpages'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-cooked-manpages'"
    fi
    AC_MSG_RESULT([${with_cooked_manpages:-yes}])
    AC_MSG_CHECKING([for manpage compression])
    if test :$disable_compress_manpages = :yes ; then
	enable_compress_manpages=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_compress_manpages --disable-compress-manpages\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-compress-manpages'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-compress-manpages'"
    fi
    AC_MSG_RESULT([${enable_compress_manpages:-yes}])
])# _MAN_CONVERSION_OPTIONS
# =========================================================================

# =========================================================================
# _MAN_CONVERSION_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION_OUTPUT], [dnl
    AM_CONDITIONAL([COOKED_MANPAGES], [test :"${with_cooked_manpages:-yes}" != :no])dnl
    AM_CONDITIONAL([COMPRESS_MANPAGES], [test :"${enable_compress_manpages:-yes}" = :yes])dnl
])# _MAN_CONVERSION_OUTPUT
# =========================================================================

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
