# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/02/19 11:49:58 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Last Modified $Date: 2005/02/19 11:49:58 $ by $Author: brian $
#
# =============================================================================

# =========================================================================
# _MAN_CONVERSION
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION], [dnl
    _MAN_CONVERSION_OPTIONS
    _MAN_CONVERSION_SETUP
    _MAN_CONVERSION_OUTPUT
])# _MAN_CONVERSION
# =========================================================================

# =========================================================================
# _MAN_CONVERSION_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION_OPTIONS], [dnl
])# _MAN_CONVERSION_OPTIONS
# =========================================================================

# =========================================================================
# _MAN_CONVERSION_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_MAN_CONVERSION_SETUP], [dnl
    AC_MSG_CHECKING([for manpage conversion])
    AC_ARG_WITH([cooked-manpages],
	AS_HELP_STRING([--with-cooked-manpages],
	    [convert manual pages to remove macro dependencies and grefer
	    references.  @<:@default=no@:>@]),
	[with_cooked_manpages="$withval"],
	[with_cooked_manpages='no'])
    if test :"${with_cooked_manpages:-yes}" != :no ; then
	AC_MSG_RESULT([yes])
	AC_ARG_VAR([SOELIM], [Roff source elminiation command])
	AC_ARG_VAR([REFER], [Roff references command])
	AC_ARG_VAR([TBL], [Roff table command])
	AC_ARG_VAR([PIC], [Roff picture command])
	AC_PATH_PROGS([SOELIM], [gsoelim soelim], [/bin/cat], [$PATH:/usr/local/bin:/usr/bin:/bin])
	AC_PATH_PROGS([REFER], [grefer refer], [/bin/cat], [$PATH:/usr/local/bin:/usr/bin:/bin])
	AC_PATH_PROGS([TBL], [gtbl tbl], [/bin/cat], [$PATH:/usr/local/bin:/usr/bin:/bin])
	AC_PATH_PROGS([PIC], [gpic pic], [/bin/cat], [$PATH:/usr/local/bin:/usr/bin:/bin])
    else
	AC_MSG_RESULT([no])
    fi
    AC_MSG_CHECKING([for manpage compression])
    AC_ARG_ENABLE([compress-manpages],
	AS_HELP_STRING([--disable-compress-manpages],
	    [compress manpages with gzip or bzip2 or leave them uncompressed.
	    @<:@default=yes@:>@]),
	[enable_compress_manpages="$enableval"],
	[enable_compress_manpages='yes'])
    if test :"${enable_compress_manpages:-yes}" != :yes ; then
	AC_MSG_RESULT([no])
    else
	AC_MSG_RESULT([yes])
	AC_ARG_VAR([GZIP], [Gzip default compression options @<:@default=--best@:>@])
	if test -z "$GZIP"; then
	    GZIP='-f9v'
	fi
	AC_ARG_VAR([GZIP_CMD], [Gzip compression command @<:@default=gzip@:>@])
	AC_PATH_PROGS([GZIP_CMD], [gzip], [/usr/bin/gzip], [$PATH:/usr/local/bin:/usr/bin:/bin])
	AC_ARG_VAR([BZIP2], [Bzip2 default compression options @<:@default=@:>@])
	if test -z "$BZIP2"; then
	    BZIP2='-f9v'
	fi
	AC_ARG_VAR([BZIP2_CMD], [Bzip2 compression command @<:@default=bzip2@:>@])
	AC_PATH_PROGS([BZIP2_CMD], [bzip2], [/usr/bin/bzip2], [$PATH:/usr/local/bin:/usr/bin:/bin])
    fi
    AC_ARG_VAR([MAKEWHATIS], [Makewhatis command])
    AC_PATH_PROGS([MAKEWHATIS], [makewhatis], [/usr/sbin/makewhatis], [$PATH:/usr/local/sbin:/usr/sbin:/sbin])
])# _MAN_CONVERSION_SETUP
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
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
