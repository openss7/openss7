dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.1 2004/04/05 12:39:05 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (C) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
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
dnl Last Modified $Date: 2004/04/05 12:39:05 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/public.m4])
m4_include([m4/kernel.m4])
m4_include([m4/streams.m4])
m4_include([m4/man.m4])
m4_include([m4/rpm.m4])

# =========================================================================
# AC_XNET
# -------------------------------------------------------------------------
AC_DEFUN(AC_XNET,
[
    ac_default_prefix='/usr'
    _XNET_OPTIONS
    AC_MAN_CONVERSION
    AC_PUBLIC_RELEASE
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    _XNET_SETUP
    XNET_INCLUDES="-I- -imacros ./config.h -I./src/include -I${srcdir}/src/include${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user INCLUDES  = $XNET_INCLUDES])
    AC_MSG_NOTICE([final kernel MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel CFLAGS    = $KERNEL_CFLAGS])
    AC_SUBST([USER_CPPFLAGS])
    AC_SUBST([USER_CFLAGS])
    AC_SUBST([XNET_INCLUDES])
    CPPFLAGS=
    CFLAGS=
    AC_RPM_SPEC
])# AC_XNET
# =========================================================================

# =========================================================================
# _XNET_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN(_XNET_OPTIONS,
[
    AC_ARG_ENABLE([inet],
                  AC_HELP_STRING([--enable-inet],
                                 [enable inet package. @<:@default=no@:>@]),
                  [enable_inet=$enableval],
                  [enable_inet=''])
    AC_ARG_ENABLE([sctp2],
                  AC_HELP_STRING([--enable-sctp2],
                                 [enable sctp2 package. @<:@default=no@:>@]),
                  [enable_sctp2=$enableval],
                  [enable_sctp2=''])
])# _XNET_OPTIONS
# =========================================================================

# =========================================================================
# _XNET_SETUP
# -------------------------------------------------------------------------
AC_DEFUN(_XNET_SETUP,
[
    AC_LINUX_KERNEL
    AC_LINUX_STREAMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel and LiS
])# _XNET_SETUP
# =========================================================================

# =========================================================================
# _XNET_
# -------------------------------------------------------------------------
AC_DEFUN(_XNET_,
[
])# _XNET_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
