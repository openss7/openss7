dnl =========================================================================
dnl
dnl @(#) $Id: public.m4,v 0.9.2.2 2004/06/27 06:35:30 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
dnl Last Modified $Date: 2004/06/27 06:35:30 $ by $Author: brian $
dnl 
dnl =========================================================================

dnl -------------------------------------------------------------------------
dnl This file provides some basic public/private release options.  A non-public
dnl release contains components that have not yet been publicly released by The
dnl OpenSS7 Project.  Non-public components are available on the CVS archive
dnl site.  See http://www.openss7.org/ for more information.
dnl -------------------------------------------------------------------------

# =========================================================================
# _PUBLIC_RELEASE
# -------------------------------------------------------------------------
AC_DEFUN([_PUBLIC_RELEASE], [dnl
    _PUBLIC_RELEASE_OPTIONS
    _PUBLIC_RELEASE_SETUP
    _PUBLIC_RELEASE_OUTPUT
])# _PUBLIC_RELEASE
# =========================================================================

# =========================================================================
# _PUBLIC_RELEASE_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_PUBLIC_RELEASE_OPTIONS], [dnl
])# _PUBLIC_RELEASE_OPTIONS
# =========================================================================

# =========================================================================
# _PUBLIC_RELEASE_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_PUBLIC_RELEASE_SETUP], [dnl
])# _PUBLIC_RELEASE_SETUP
# =========================================================================

# =========================================================================
# _PUBLIC_RELEASE_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_PUBLIC_RELEASE_OUTPUT], [dnl
    AC_ARG_ENABLE([public],
        AS_HELP_STRING([--disable-public],
            [disable public release.  @<:@default=no@:>@]),
        [enable_public="$enableval"],
        [enable_public='yes'])
    AC_MSG_CHECKING([for public release])
    if test :"${enable_public:-yes}" != :yes ; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([yes])
    fi
    AM_CONDITIONAL([PUBLIC_RELEASE], test :"${enable_public:-yes}" = :yes)dnl
])# _PUBLIC_RELEASE_OUTPUT
# =========================================================================

# =========================================================================
# _PUBLIC_
# -------------------------------------------------------------------------
AC_DEFUN([_PUBLIC_], [dnl
])# _PUBLIC_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
