dnl =========================================================================
dnl vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.3 2005/01/23 08:56:14 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corp. <http://www.openss7.com>
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
dnl Acquisition Regulations ("FAR") (or any successor regulations) or, in the
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
dnl Last Modified $Date: 2005/01/23 08:56:14 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/openss7.m4])
m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/streams.m4])
m4_include([m4/xopen.m4])
m4_include([m4/xti.m4])
m4_include([m4/xns.m4])
m4_include([m4/sctp.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/libraries.m4])
m4_include([m4/strconf.m4])

# =========================================================================
# AC_OS7
# -------------------------------------------------------------------------
AC_DEFUN([AC_OS7], [dnl
    _OPENSS7_PACKAGE([OPENSS7], [OpenSS7 Master Package])
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _RPM_SPEC
    _LDCONFIG
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    _LINUX_KERNEL
    _LINUX_STREAMS
    _XNS
    _XTI
    _INET
    _SCTP
dnl AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
dnl AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
dnl AC_MSG_NOTICE([final kern CPPFLAGS  = $KERNEL_CPPFLAGS])
dnl AC_MSG_NOTICE([final kern CFLAGS    = $KERNEL_CFLAGS])
dnl AC_MSG_NOTICE([final kern MODFLAGS  = $KERNEL_MODFLAGS])
dnl AC_MSG_NOTICE([final kern NOVERSION = $KERNEL_NOVERSION])
    vars=`(set) 2>&1 | egrep '^(ac|man|rpm|linux|xns|xti|inet|sctp|streams)_cv_.*=' | sed -e 's|=.*||;s|^|declare -x |;s|$|; |'`
    echo $vars
    eval "$vars"
])# AC_OS7
# =========================================================================
