dnl =============================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =============================================================================
dnl 
dnl @(#) $RCSfile: sctp.m4,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/01/24 07:33:00 $
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl
dnl All Rights Reserved.
dnl
dnl This program is free software; you can redistribute it and/or modify it under
dnl the terms of the GNU General Public License as published by the Free Software
dnl Foundation; either version 2 of the License, or (at your option) any later
dnl version.
dnl
dnl This program is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
dnl FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
dnl details.
dnl
dnl You should have received a copy of the GNU General Public License along with
dnl this program; if not, write to the Free Software Foundation, Inc., 675 Mass
dnl Ave, Cambridge, MA 02139, USA.
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions apply
dnl to you.  If the Software is supplied by the Department of Defense ("DoD"), it
dnl is classified as "Commercial Computer Software" under paragraph 252.227-7014
dnl of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
dnl successor regulations) and the Government is acquiring only the license rights
dnl granted herein (the license rights customarily provided to non-Government
dnl users).  If the Software is supplied to any unit or agency of the Government
dnl other than DoD, it is classified as "Restricted Computer Software" and the
dnl Government's rights in the Software are defined in paragraph 52.227-19 of the
dnl Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
dnl the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Commercial licensing and support of this software is available from OpenSS7
dnl Corporation at a fee.  See http://www.openss7.com/
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Last Modified $Date: 2005/01/24 07:33:00 $ by $Author: brian $
dnl
dnl =============================================================================

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
    _SCTP_SETUP
    _SCTP_OPENSS7
    AC_SUBST([SCTP_CPPFLAGS])
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
    _SCTP_CHECK_HEADERS
    _SCTP_DEFINES
])# _SCTP_SETUP
# =========================================================================

# =========================================================================
# _SCTP_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS SCTP header files.  The package
    # normally requires SCTP header files to compile.
    AC_CACHE_CHECK([for sctp include directory], [sctp_cv_includes], [dnl
        if test ":${with_sctp:-no}" != :no -a :"${with_sctp:-no}" != :yes ;  then
            sctp_cv_includes="$with_sctp"
        fi
        sctp_what="sys/xti_sctp.h"
        if test ":${sctp_cv_includes:-no}" = :no ; then
            # The next place to look now is for a peer package being built under
            # the same top directory, and then the higher level directory.
            sctp_here=`pwd`
            sctp_where="strsctp/src/include"
            for sctp_d in . .. ; do
                sctp_dir="$srcdir/$sctp_d/$sctp_where"
                sctp_bld=`echo "$sctp_here/$sctp_d/$sctp_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                if test -d $sctp_dir -a -r $sctp_dir/$sctp_what ; then
                    sctp_dir=`(cd $sctp_dir; pwd)`
                    sctp_cv_includes="$sctp_dir $sctp_bld"
                    break
                fi
            done
        fi
        if test ":${sctp_cv_includes:-no}" = :no ; then
            eval "sctp_search_path=\"
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}$oldincludedir/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/include/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/local/include/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}${linux_cv_k_prefix}/usr/src/strsctp/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/strsctp
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/strsctp/src/include
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
            sctp_search_path=`echo "$sctp_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            sctp_cv_includes=
            for sctp_dir in $sctp_search_path ; do
                if test -d "$sctp_dir" -a -r "$sctp_dir/$sctp_what" ; then
                    sctp_cv_includes="$sctp_dir"
                    break
                fi
            done
        fi
    ])
    if test :"${sctp_cv_includes:-no}" = :no ; then :
        AC_MSG_WARN([
***
*** Could not find SCTP include directories.  This package requires the
*** presence of SCTP include directories to compile.  Specify the location of
*** SCTP include directories with option --with-sctp to configure and try again.
***
        ])
    else
        if test -z "$with_sctp" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--with sctp"
        fi
    fi
])# _SCTP_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _SCTP_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_SCTP_DEFINES], [dnl
    for sctp_include in $sctp_cv_includes ; do
        SCTP_CPPFLAGS="${SCTP_CPPFLAGS}${SCTP_CPPFLAGS:+ }-I${sctp_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
        Define for SuSv3.  This is necessary for LiS and LfS and strsctp for
        that matter.
    ])
])# _SCTP_DEFINES
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


dnl =============================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =============================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =============================================================================
