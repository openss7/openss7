dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.2 2004/12/18 11:17:02 brian Exp $
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
dnl Last Modified $Date: 2004/12/18 11:17:02 $ by $Author: brian $
dnl 
dnl =========================================================================

#dnl m4_include([m4/openss7.m4])
#dnl m4_include([m4/kernel.m4])
#dnl m4_include([m4/genksyms.m4])
#dnl m4_include([m4/streams.m4])
#dnl m4_include([m4/xopen.m4])
#dnl m4_include([m4/xti.m4])
#dnl m4_include([m4/xns.m4])
#dnl m4_include([m4/sctp.m4])
#dnl m4_include([m4/man.m4])
#dnl m4_include([m4/public.m4])
#dnl m4_include([m4/rpm.m4])
#dnl m4_include([m4/libraries.m4])
#dnl m4_include([m4/strconf.m4])

# =========================================================================
# AC_OPENSS7
# -------------------------------------------------------------------------
AC_DEFUN([AC_OPENSS7], [dnl
#dnl     _OPENSS7_PACKAGE([OPENSS7], [OpenSS7 Master Package])
#dnl     _MAN_CONVERSION
#dnl     _PUBLIC_RELEASE
#dnl     _RPM_SPEC
#dnl     _LDCONFIG
#dnl     # user CPPFLAGS and CFLAGS
#dnl     USER_CPPFLAGS="${CPPFLAGS}"
#dnl     USER_CFLAGS="${CFLAGS}"
#dnl     _LINUX_KERNEL
#dnl     _LINUX_STREAMS
#dnl     _XNS
#dnl     _XTI
#dnl     _XOPEN
#dnl     CPPFLAGS="-I- -include ./config.h${sctp_cv_includes:+ -I}${sctp_cv_includes}${inet_cv_includes:+ -I}${inet_cv_includes}${xti_cv_includes:+ -I}${xti_cv_includes}${xns_cv_includes:+ -I}${xns_cv_includes}${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
#dnl     AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
#dnl     AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
#dnl     AC_MSG_NOTICE([final user    INCLUDES  = $SCTP_INCLUDES])
#dnl     AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
#dnl     AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
#dnl     AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
#dnl     AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
#dnl     AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
#dnl     AC_SUBST([USER_CPPFLAGS])dnl
#dnl     AC_SUBST([USER_CFLAGS])dnl
declare -x ac_srcdir="$srcdir"
declare -x ac_builddir='.'
declare -x ac_abs_srcdir=`(cd $ac_srcdir ; pwd)`
declare -x ac_abs_builddir=`(cd $ac_builddir ; pwd)`
declare -x ac_top_srcdir="$ac_srcdir"
declare -x ac_top_builddir="$ac_builddir"
declare -x ac_abs_top_srcdir=`(cd $ac_top_srcdir; pwd)`
declare -x ac_abs_top_builddir=`(cd $ac_top_builddir; pwd)`
declare -x streams_cv_package='LiS'
declare -x streams_cv_includes="$ac_abs_top_builddir/LiS/include $ac_abs_top_srcdir/LiS/include"
declare -x streams_cv_lis_includes="$ac_abs_top_builddir/LiS/include $ac_abs_top_srcdir/LiS/include"
declare -x streams_cv_lfs_includes="$ac_abs_top_builddir/streams/include $ac_abs_top_srcdir/streams/include"
declare -x xti_cv_includes="$ac_abs_top_builddir/strxnet/src/include $ac_abs_top_srcdir/strxnet/src/include"
declare -x xns_cv_includes="$ac_abs_top_builddir/strxns/src/include $ac_abs_top_srcdir/strxns/src/include"
])# AC_OPENSS7
# =========================================================================

