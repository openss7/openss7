dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.10 2004/05/23 07:24:23 brian Exp $
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
dnl Last Modified $Date: 2004/05/23 07:24:23 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/kernel.m4])
m4_include([m4/streams.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/xopen.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/libraries.m4])
m4_include([m4/strconf.m4])

# =========================================================================
# AC_SS7
# -------------------------------------------------------------------------
AC_DEFUN([AC_SS7], [
    ac_default_prefix='/usr'
    _SS7_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _LDCONFIG
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    _LINUX_KERNEL
    _LINUX_STREAMS
    _XOPEN
    SS7_INCLUDES="-I- -imacros ./config.h ${STREAMS_CPPFLAGS}${STREAMS_CPPFLAGS:+ }-I./src/include -I${srcdir}/src/include"
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    INCLUDES  = $SS7_INCLUDES])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_SUBST([USER_CPPFLAGS])
    AC_SUBST([USER_CFLAGS])
    AC_SUBST([SS7_INCLUDES])
    CPPFLAGS=
    CFLAGS=
    _SS7_SETUP
    _SS7_OUTPUT
])# AC_SS7
# =========================================================================

# =========================================================================
# _SS7_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_OPTIONS], [
])# _SS7_OPTIONS
# =========================================================================

# =========================================================================
# _SS7_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_SETUP], [
])# _SS7_SETUP
# =========================================================================

# =========================================================================
# _SS7_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_OUTPUT], [
    _SS7_STRCONF
])# _SS7_OUTPUT
# =========================================================================

# =========================================================================
# _SS7_STRCONF
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_STRCONF], [
    strconf_cv_stem='lis.conf'
dnl strconf_cv_input='Config.master'
    strconf_cv_majbase=180
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
dnl strconf_cv_drvconf='drvconf.mk'
dnl strconf_cv_confmod='conf.modules'
dnl strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes='ss7makenodes.c'
dnl strconf_cv_stsetup='strsetup.conf'
dnl strconf_cv_strload='strload.conf'
    _STRCONF
])# _SS7_STRCONF
# =========================================================================

# =========================================================================
# _SS7_
# -------------------------------------------------------------------------
AC_DEFUN([_SS7_], [
])# _SS7_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
