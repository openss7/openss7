dnl =========================================================================
dnl
dnl @(#) $Id: inet.m4,v 0.9.2.3 2004/08/06 19:37:37 brian Exp $
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
dnl Last Modified $Date: 2004/08/06 19:37:37 $ by $Author: brian $
dnl 
dnl =========================================================================

# =========================================================================
# _INET
# -------------------------------------------------------------------------
# Check for the existence of INET header files, particularly sys/xti_inet.h.
# INET headers files are required for building the XTI interface for SCTP.
# Without INET header files, the INET interface to SCTP will not be built.
# -------------------------------------------------------------------------
AC_DEFUN([_INET], [dnl
    AC_REQUIRE([_XTI])dnl
    _INET_OPTIONS
    _INET_SETUP dnl
])# _INET
# =========================================================================

# =========================================================================
# 
# -------------------------------------------------------------------------
AC_DEFUN([_INET_OPTIONS], [dnl
    AC_ARG_WITH([inet],
        AC_HELP_STRING([--with-inet=HEADERS],
            [specify the INET header file directory.
            @<:@default=$INCLUDEDIR/strinet@:>@]),
        [with_inet="$withval"],
        [with_inet=''])
])# _INET_OPTIONS
# =========================================================================

# =========================================================================
# _INET_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_INET_SETUP], [dnl
    # Test for the existence of Linux STREAMS INET header files.  The package
    # normally requires INET header files to compile.
    if test ":${with_inet:-no}" != :no -a :"${with_inet:-no}" != :yes ;  then
        inet_cv_includes="$with_inet"
    else
        eval "inet_search_path=\"
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
        inet_search_path=`echo "$inet_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        inet_cv_includes=
        for inet_dir in $inet_search_path ; do
            AC_MSG_CHECKING([for inet include directory $inet_dir])
            if test -d "$inet_dir" -a -r "$inet_dir/sys/xti_inet.h" ; then
                inet_cv_includes="$inet_dir"
                AC_MSG_RESULT([yes])
                break
            fi
            AC_MSG_RESULT([no])
        done
    fi
    AC_MSG_CHECKING([for inet include directory])
    AC_MSG_RESULT([${inet_cv_includes:-no}])
    if test :"${inet_cv_includes:-no}" = :no ; then
        :
#        AC_MSG_ERROR([
#***
#*** Could not find INET include directories.  This package requires the
#*** presence of INET include directories to compile.  Specify the location of
#*** INET include directories with option --with-inet to configure and try again.
#***
#        ])
    else
        if test -z "$with_inet" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--with inet"
        fi
    fi
])# _INET_SETUP
# =========================================================================

# =========================================================================
# _INET_
# -------------------------------------------------------------------------
AC_DEFUN([_INET_], [dnl
])# _INET_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================

