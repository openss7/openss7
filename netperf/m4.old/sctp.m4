dnl =========================================================================
dnl
dnl @(#) $Id: sctp.m4,v 1.1.2.1 2004/08/06 12:44:08 brian Exp $
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
dnl Last Modified $Date: 2004/08/06 12:44:08 $ by $Author: brian $
dnl 
dnl =========================================================================

# =========================================================================
# _SCTP
# -------------------------------------------------------------------------
# Check for the existence of SCTP header files, particularly sys/xti_sctp.h.
# SCTP headers files are required for building the XTI interface for SCTP.
# Without SCTP header files, the SCTP interface to SCTP will not be built.
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP], [dnl
    AC_REQUIRE([_XTI])dnl
    _SCTP_OPTIONS
    _SCTP_SETUP dnl
    _SCTP_OPENSS7
])# _SCTP
# =========================================================================

# =========================================================================
# 
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPTIONS], [dnl
    AC_ARG_WITH([sctp],
        AC_HELP_STRING([--with-sctp=HEADERS],
            [specify the SCTP header file directory.
            @<:@default=$INCLUDEDIR/strsctp@:>@]),
        [with_sctp="$withval"],
        [with_sctp=''])
])# _SCTP_OPTIONS
# =========================================================================

# =========================================================================
# _SCTP_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_SETUP], [dnl
    # Test for the existence of Linux STREAMS SCTP header files.  The package
    # normally requires SCTP header files to compile.
    if test ":${with_sctp:-no}" != :no -a :"${with_sctp:-no}" != :yes ;  then
        sctp_cv_includes="$with_sctp"
    else
        eval "sctp_search_path=\"
            $streams_cv_rootdir$includedir/strsctp
            $streams_cv_rootdir$streams_cv_prefix$oldincludedir/strsctp
            $streams_cv_rootdir$streams_cv_prefix/usr/include/strsctp
            $streams_cv_rootdir$streams_cv_prefix/usr/local/include/strsctp
            $streams_cv_rootdir$streams_cv_prefix/usr/src/strsctp/src/include
            $streams_cv_rootdir$oldincludedir/strsctp
            $streams_cv_rootdir/usr/include/strsctp
            $streams_cv_rootdir/usr/local/include/strsctp
            $streams_cv_rootdir/usr/src/strsctp/src/include
            $streams_cv_rootdir$includedir/strinet
            $streams_cv_rootdir$streams_cv_prefix$oldincludedir/strinet
            $streams_cv_rootdir$streams_cv_prefix/usr/include/strinet
            $streams_cv_rootdir$streams_cv_prefix/usr/local/include/strinet
            $streams_cv_rootdir$streams_cv_prefix/usr/src/strinet/src/include
            $streams_cv_rootdir$oldincludedir/strinet
            $streams_cv_rootdir/usr/include/strinet
            $streams_cv_rootdir/usr/local/include/strinet
            $streams_cv_rootdir/usr/src/strinet/src/include
            $streams_cv_rootdir$includedir/strxnet
            $streams_cv_rootdir$streams_cv_prefix$oldincludedir/strxnet
            $streams_cv_rootdir$streams_cv_prefix/usr/include/strxnet
            $streams_cv_rootdir$streams_cv_prefix/usr/local/include/strxnet
            $streams_cv_rootdir$streams_cv_prefix/usr/src/strxnet/src/include
            $streams_cv_rootdir$oldincludedir/strxnet
            $streams_cv_rootdir/usr/include/strxnet
            $streams_cv_rootdir/usr/local/include/strxnet
            $streams_cv_rootdir/usr/src/strxnet/src/include
            $streams_cv_rootdir$includedir/streams
            $streams_cv_rootdir$streams_cv_prefix$oldincludedir/streams
            $streams_cv_rootdir$streams_cv_prefix/usr/include/streams
            $streams_cv_rootdir$streams_cv_prefix/usr/local/include/streams
            $streams_cv_rootdir$streams_cv_prefix/usr/src/streams/src/include
            $streams_cv_rootdir$oldincludedir/streams
            $streams_cv_rootdir/usr/include/streams
            $streams_cv_rootdir/usr/local/include/streams
            $streams_cv_rootdir/usr/src/streams/include
            $streams_cv_rootdir$includedir/LiS
            $streams_cv_rootdir$streams_cv_prefix$oldincludedir/LiS
            $streams_cv_rootdir$streams_cv_prefix/usr/include/LiS
            $streams_cv_rootdir$streams_cv_prefix/usr/local/include/LiS
            $streams_cv_rootdir$streams_cv_prefix/usr/src/LiS/include
            $streams_cv_rootdir$oldincludedir/LiS
            $streams_cv_rootdir/usr/include/LiS
            $streams_cv_rootdir/usr/local/include/LiS
            $streams_cv_rootdir/usr/src/LiS/include\""
        sctp_search_path=`echo "$sctp_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        sctp_cv_includes=
        for sctp_dir in $sctp_search_path ; do
            AC_MSG_CHECKING([for sctp include directory $sctp_dir])
            if test -d "$sctp_dir" -a -r "$sctp_dir/sys/xti_sctp.h" ; then
                sctp_cv_includes="$sctp_dir"
                AC_MSG_RESULT([yes])
                break
            fi
            AC_MSG_RESULT([no])
        done
    fi
    AC_MSG_CHECKING([for sctp include directory])
    AC_MSG_RESULT([${sctp_cv_includes:-no}])
    if test :"${sctp_cv_includes:-no}" = :no ; then
        :
#        AC_MSG_ERROR([
#***
#*** Could not find SCTP include directories.  This package requires the
#*** presence of SCTP include directories to compile.  Specify the location of
#*** SCTP include directories with option --with-sctp to configure and try again.
#***
#        ])
    fi
])# _SCTP_SETUP
# =========================================================================

# =========================================================================
# _SCTP_OPENSS7
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_OPENSS7], [dnl
    AC_CACHE_CHECK([for sctp openss7 kernel], [sctp_cv_openss7], [dnl
        AC_EGREP_CPP([\<yes_we_have_openss7_kernel_sctp_headers\>], [
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#ifdef SCTP_DISPOSITION_UNSENT
        yes_we_have_openss7_kernel_sctp_headers
#endif
        ], [sctp_cv_openss7=yes], [sctp_cv_openss7=no])
    ])
])# _SCTP_OPENSS7
# =========================================================================

# =========================================================================
# _SCTP_
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_], [dnl
])# _SCTP_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================

