dnl =========================================================================
dnl
dnl @(#) $Id: inet.m4,v 0.9.2.8 2005/01/27 04:36:08 brian Exp $
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
dnl Last Modified $Date: 2005/01/27 04:36:08 $ by $Author: brian $
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
    _INET_SETUP
    AC_SUBST([INET_CPPFLAGS])
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
    _INET_CHECK_HEADERS
    _INET_DEFINES
])# _INET_SETUP
# =========================================================================

# =========================================================================
# _INET_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_INET_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS INET header files.  The package
    # normally requires INET header files to compile.
    AC_CACHE_CHECK([for inet include directory], [inet_cv_includes], [dnl
        if test ":${with_inet:-no}" != :no -a :"${with_inet:-no}" != :yes ;  then
            inet_cv_includes="$with_inet"
        fi
        inet_what="sys/xti_inet.h"
        if test ":${inet_cv_includes:-no}" = :no ; then
            # The next place to look now is for a peer package being built under
            # the same top directory, and then the higher level directory.
            inet_here=`pwd`
            inet_where="strinet/src/include"
            for inet_d in . .. ; do
                inet_dir="$srcdir/$inet_d/$inet_where"
                inet_bld=`echo "$inet_here/$inet_d/$inet_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                if test -d $inet_dir -a -r $inet_dir/$inet_what ; then
                    inet_dir=`(cd $inet_dir; pwd)`
                    inet_cv_includes="$inet_dir $inet_bld"
                    break
                fi
            done
        fi
        if test ":${inet_cv_includes:-no}" = :no ; then
            eval "inet_search_path=\"
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}$oldincludedir/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/include/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/local/include/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/src/strinet/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/strinet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/strinet/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}$oldincludedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/local/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/src/strxnet/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/strxnet/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/streams
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}$oldincludedir/streams
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/include/streams
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/local/include/streams
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/src/streams/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/streams
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/streams
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/streams
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/streams/include
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}$oldincludedir/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/include/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/local/include/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/src/LiS/include
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/LiS
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/LiS/include\""
            inet_search_path=`echo "$inet_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            inet_cv_includes=
            for inet_dir in $inet_search_path ; do
                if test -d "$inet_dir" -a -r "$inet_dir/$inet_what" ; then
                    inet_cv_includes="$inet_dir"
                    break
                fi
            done
        fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${inet_cv_includes:-no}" = :no ; then :
        if test :"$with_inet" = :no ; then
            AC_MSG_ERROR([
***
*** Could not find INET include directories.  This package requires the
*** presence of INET include directories to compile.  Specify the location of
*** INET include directories with option --with-inet to configure and try again.
*** ])
        fi
        if test -z "$with_inet" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--define \"_with_inet --with-inet\""
        fi
    else
        if test -z "$with_inet" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--define \"_without_inet --without-inet\""
        fi
    fi
])# _INET_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _INET_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_INET_DEFINES], [dnl
    for inet_include in $inet_cv_includes ; do
        INET_CPPFLAGS="${INET_CPPFLAGS}${INET_CPPFLAGS:+ }-I${inet_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
        Define for SuSv3.  This is necessary for LiS and LfS and strinet for
        that matter.
    ])
])# _INET_DEFINES
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

