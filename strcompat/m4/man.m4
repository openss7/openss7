dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: man.m4,v 0.9.2.9 2005/01/21 00:15:00 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl
dnl All Rights Reserved.
dnl
dnl Permission is granted to make and distribute verbatim copies of this
dnl README file provided the copyright notice and this permission notice are
dnl preserved on all copies.
dnl 
dnl Permission is granted to copy and distribute modified versions of this
dnl manual under the conditions for verbatim copying, provided that the
dnl entire resulting derived work is distributed under the terms of a
dnl permission notice identical to this one
dnl 
dnl Since the Linux kernel and libraries are constantly changing, this README
dnl file may be incorrect or out-of-date.  The author(s) assume no
dnl responsibility for errors or omissions, or for damages resulting from the
dnl use of the information contained herein.  The author(s) may not have
dnl taken the same level of care in the production of this manual, which is
dnl licensed free of charge, as they might when working professionally.
dnl 
dnl Formatted or processed versions of this manual, if unaccompanied by the
dnl source, must acknowledge the copyright and authors of this work.
dnl 
dnl =========================================================================
dnl 
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions
dnl apply to you.  If the Software is supplied by the Department of Defense
dnl ("DoD"), it is classified as "Commercial Computer Software" under
dnl paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
dnl Regulations ("DFARS") (or any successor regulations) and the Government is
dnl acquiring only the license rights granted herein (the license rights
dnl customarily provided to non-Government users).  If the Software is
dnl supplied to any unit or agency of the Government other than DoD, it is
dnl classified as "Restricted Computer Software" and the Government's rights
dnl in the Software are defined in paragraph 52.227-19 of the Federal
dnl Acquisition Regulations ("FAR") (or any success regulations) or, in the
dnl cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl 
dnl =========================================================================
dnl 
dnl Commercial licensing and support of this software is available from
dnl OpenSS7 Corporation at a fee.  See http://www.openss7.com/
dnl 
dnl =========================================================================
dnl
dnl Last Modified $Date: 2005/01/21 00:15:00 $ by $Author: brian $
dnl 
dnl =========================================================================

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

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
