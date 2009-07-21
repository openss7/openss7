# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: man.m4,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2009-07-21 11:06:13 $
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
# Last Modified $Date: 2009-07-21 11:06:13 $ by $Author: brian $
#
# =============================================================================

# =========================================================================
# _MAN_CONVERSION
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION], [dnl
    _MAN_CONVERSION_ARGS
    _MAN_CONVERSION_SETUP
    _MAN_CONVERSION_OPTIONS
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
    tmp_path="${PATH:+$PATH:}/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:$am_aux_dir";
    AC_ARG_VAR([SOELIM], [Roff source elimination command. @<:@default=gsoelim,soelim@:>@])
    AC_PATH_PROGS([SOELIM], [gsoelim soelim],
		  [/bin/cat], [$tmp_path])
    if test :"$SOELIM" = :/bin/cat ; then
	if test :$with_cooked_manpages != :no ; then
	    without_cooked_manpages=yes
	    AC_MSG_WARN([
***
*** Configure cannot find a suitable 'soelim' program.  Generating
*** cooked manual pages requires the 'soelim' program from the 'groff'
*** package.  You can normally get 'groff' as part of most poplular
*** Linux distributions and all current versions are acceptable.  The
*** 'groff' package has been available for many years and is available
*** on the web from any GNU archive site.  Use the following commands to
*** obtain 'groff':
***
*** Debian 4.0:  'apt-get install groff-base'
*** Ubuntu 8.04: 'apt-get install groff'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'
***
*** To get rid of this warning, load the 'groff' package, specify an
*** appropriate program with the SOELIM environment variable to
*** 'configure', or specify the --without-cooked-manpages option to
*** 'configure'.
*** ])
	fi
    fi
    AC_ARG_VAR([REFER], [Roff references command. @<:@default=grefer,refer@:>@])
    AC_PATH_PROGS([REFER], [grefer refer],
		  [/bin/cat], [$tmp_path])
    if test :"$REFER" = :/bin/cat ; then
	if test :$with_cooked_manpages != :no ; then
	    without_cooked_manpages=yes
	    AC_MSG_WARN([
***
*** Configure cannot find a suitable 'refer' program.  Generating cooked
*** manual pages requires the 'refer' program from the 'groff' package.
*** You can normally get 'groff' as part of most poplular Linux
*** distributions and all current versions are acceptable.  The 'groff'
*** package has been available for many years and is available on the
*** web from any GNU archive site.  Use the following commands to obtain
*** 'groff':
***
*** Debian 4.0:  'apt-get install groff'
*** Ubuntu 8.04: 'apt-get install groff_ext'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'
***
*** To get rid of this warning, load the 'groff' package, specify an
*** appropriate program with the REFER environment variable to
*** 'configure', or specify the --without-cooked-manpages option to
*** 'configure'.
*** ])
	fi
    fi
    AC_ARG_VAR([PIC], [Roff picture command. @<:@default=gpic,pic@:>@])
    AC_PATH_PROGS([PIC], [gpic pic],
		  [/bin/cat], [$tmp_path])
    if test :"$PIC" = :/bin/cat ; then
	if test :$with_cooked_manpages != :no ; then
	    # without_cooked_manpages=yes
	    : # do not need pic to cook manpages
	fi
    fi
    AC_ARG_VAR([TBL], [Roff table command. @<:@default=gtbl,tbl@:>@])
    AC_PATH_PROGS([TBL], [gtbl tbl],
		  [/bin/cat], [$tmp_path])
    if test :"$TBL" = :/bin/cat ; then
	if test :$with_cooked_manpages != :no ; then
	    without_cooked_manpages=yes
	    AC_MSG_WARN([
***
*** Configure cannot find a suitable 'tbl' program.  Generating cooked
*** manual pages requires the 'tbl' program from the 'groff' package.
*** You can normally get 'groff' as part of most poplular Linux
*** distributions and all current versions are acceptable.  The 'groff'
*** package has been available for many years and is available on the
*** web from any GNU archive site.  Use the following commands to obtain
*** 'groff':
***
*** Debian 4.0:  'apt-get install groff-base'
*** Ubuntu 8.04: 'apt-get install groff'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'
***
*** To get rid of this warning, load the 'groff' package, specify an
*** appropriate program with the TBL environment variable to
*** 'configure', or specify the --without-cooked-manpages option to
*** 'configure'.
*** ])
	fi
    fi
    AC_ARG_VAR([MAKEWHATIS], [Makewhatis command. @<:@default=makewhatis,mandb@:>@])
    AC_PATH_PROG([MAKEWHATIS], [makewhatis mandb],
		 [], [$tmp_path])
    if test :"$MAKEWHATIS" = : ; then
	AC_MSG_WARN([
***
*** Configure cannot find a suitable 'makewhatis' program.  Installing
*** manual pages requires the 'makewhatis' program from the 'man'
*** package.  You can normally get 'man' as part of most populate Linux
*** distributions and all current versions are acceptable.  The 'man'
*** package has been available for many years and is available on the
*** web from the Linux Documentation Project.  Use the following
*** commands to obtain 'man':
***
*** Debian 4.0:  'apt-get install man'
*** Ubuntu 8.04: 'apt-get install man'
*** Fedora 9:    'yum install man'
*** CentOS 5.x:  'yum install man'
*** openSUSE 11: 'zypper install man'
*** SLES 10:     'zypper isntall man'
***
*** To get rid of this warning, load the 'man' package, specify an
*** appropriate program with the MAKEWHATIS environment variable to
*** 'configure'.
*** ])
    else
	tmp_program=`echo "$MAKEWHATIS" | sed 's,.*[^ ][^$-],,;s,.*/,,;s, *$,,'`
	if test :"$tmp_program" = :mandb ; then
	    MANDBFLAGS='-u'
	elif test :"$tmp_program" = :makewhatis ; then
	    MANDBFLAGS='-v -u -w'
	else
	    MANDBFLAGS=
	fi
    fi
    AC_ARG_VAR([GZIP], [Gzip default compression options @<:@default=-f9v@:>@])
    test -n "$GZIP" || GZIP='-f9v'
    AC_ARG_VAR([GZIP_CMD], [Gzip compression command @<:@default=gzip@:>@])
    AC_PATH_PROG([GZIP_CMD], [gzip], [], [$tmp_path])
    if test :"${GZIP_CMD:-no}" = :no ; then
	GZIP_CMD=
	AC_MSG_WARN([
***
*** Configure cannot find a suitable 'gzip' program.  Compressing manual
*** pages may require the 'gzip' program from the 'gzip' package on the
*** build host.  The 'gzip' package has been available for many years on
*** all distributions and is available from any GNU archive site.  Try:
***
*** Debian:  'apt-get install gzip'
*** SuSE:    'zypper install gzip'
*** CentOS:  'yum install gzip'
***
*** To get rid of this warning, load the 'gzip' package, or specify the
*** location with the GZIP_CMD environment variable to 'configure'.
*** ])
    fi
    AC_ARG_VAR([BZIP2], [Bzip2 default compression options @<:@default=-f9v@:>@])
    test -n "$BZIP2" || BZIP2='-f9v'
    AC_ARG_VAR([BZIP2_CMD], [Bzip2 compression command @<:@default=bzip2@:>@])
    AC_PATH_PROG([BZIP2_CMD], [bzip2], [], [$tmp_path])
    if test :"${BZIP2_CMD:-no}" = :no ; then
	BZIP2_CMD=
	AC_MSG_WARN([
***
*** Configure cannot find a suitable 'bzip2' program.  Compressing
*** manual pages may require the 'bzip2' program from the 'bzip2'
*** package on the build host.  The 'bzip2' package has been available
*** for many years on all distributions and is available from various
*** web sources.  Try:
***
*** Debian:  'apt-get install bzip2'
*** SuSE:    'zypper install bzip2'
*** CentOS:  'yum install bzip2'
***
*** To get rid of this warning, load the 'bzip2' package, or specify the
*** location with the BZIP2_CMD environment variable to 'configure'.
*** ])
    fi
    AC_ARG_VAR([LZMA], [Lzma default compression options @<:@default=-f9v@:>@])
    test -n "$LZMA" || LZMA='-f9v'
    AC_ARG_VAR([LZMA_CMD], [Lzma compression command @<:@default=lzma@:>@])
    AC_PATH_PROG([LZMA_CMD], [lzma], [], [$tmp_path])
    if test :"${LZMA_CMD:-no}" = :no ; then
	LZMA_CMD=
    fi
    AC_ARG_VAR([XZ], [Xz default compression options @<:@default=-c@:>@])
    test -n "$XZ" || XZ='-c'
    AC_ARG_VAR([XZ_CMD], [Xz compression command @<:@default=xz@:>@])
    AC_PATH_PROG([XZ_CMD], [xz], [], [$tmp_path])
    if test :"${XZ_CMD:-no}" = :no ; then
	XZ_CMD=
    fi
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
# $Log: man.m4,v $
# Revision 1.1.2.3  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.2  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.1  2009-06-21 11:06:05  brian
# - added files to new distro
#
# Revision 0.9.2.19  2008-09-21 07:40:46  brian
# - add defaults to environment variables
#
# Revision 0.9.2.18  2008-08-02 05:06:15  brian
# - make LZMA compression detected
#
# Revision 0.9.2.17  2008/07/29 02:14:10  brian
# - more lzma compression additions
#
# Revision 0.9.2.16  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.15  2007/10/17 20:00:28  brian
# - slightly different path checks
#
# Revision 0.9.2.14  2007/08/12 19:05:31  brian
# - rearrange and update headers
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
