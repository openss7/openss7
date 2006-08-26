dnl =========================================================================
dnl
dnl @(#) $Id: xns.m4,v 0.9.2.3 2004/08/06 19:37:37 brian Exp $
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
# _XNS
# -------------------------------------------------------------------------
# Check for the existence of XNS header files, particularly sys/npi.h.  NPI
# headers files are required for building the NPI interface for SCTP.  Without
# NPI header files, the NPI interface to SCTP will not be built.
# -------------------------------------------------------------------------
AC_DEFUN([_XNS], [dnl
    AC_REQUIRE([_STREAMS])dnl
    _XNS_OPTIONS
    _XNS_SETUP dnl
])# _XNS
# =========================================================================

# =========================================================================
# 
# -------------------------------------------------------------------------
AC_DEFUN([_XNS_OPTIONS], [dnl
    AC_ARG_WITH([xns],
        AC_HELP_STRING([--with-xns=HEADERS],
            [specify the XNS header file directory.
            @<:@default=$INCLUDEDIR/strxns@:>@]),
        [with_xns="$withval"],
        [with_xns=''])
])# _XNS_OPTIONS
# =========================================================================

# =========================================================================
# _XNS_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_XNS_SETUP], [dnl
    # Test for the existence of Linux STREAMS XNS header files.  The package
    # normally requires XNS header files to compile.
    if test ":${with_xns:-no}" != :no -a :"${with_xns:-no}" != :yes ;  then
        xns_cv_includes="$with_xns"
    else
        eval "xns_search_path=\"
            $streams_cv_rootdir$includedir/strxns
            $streams_cv_rootdir$streams_cv_prefix$oldincludedir/strxns
            $streams_cv_rootdir$streams_cv_prefix/usr/include/strxns
            $streams_cv_rootdir$streams_cv_prefix/usr/local/include/strxns
            $streams_cv_rootdir$streams_cv_prefix/usr/src/strxns/src/include
            $streams_cv_rootdir$oldincludedir/strxns
            $streams_cv_rootdir/usr/include/strxns
            $streams_cv_rootdir/usr/local/include/strxns
            $streams_cv_rootdir/usr/src/strxns/src/include
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
        xns_search_path=`echo "$xns_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        xns_cv_includes=
        for xns_dir in $xns_search_path ; do
            AC_MSG_CHECKING([for xns include directory $xns_dir])
            if test -d "$xns_dir" -a -r "$xns_dir/sys/npi.h" ; then
                xns_cv_includes="$xns_dir"
                AC_MSG_RESULT([yes])
                break
            fi
            AC_MSG_RESULT([no])
        done
    fi
    AC_MSG_CHECKING([for xns include directory])
    AC_MSG_RESULT([${xns_cv_includes:-no}])
    if test :"${xns_cv_includes:-no}" = :no ; then
        :
#        AC_MSG_ERROR([
#***
#*** Could not find XNS include directories.  This package requires the
#*** presence of XNS include directories to compile.  Specify the location of
#*** XNS include directories with option --with-xns to configure and try again.
#***
#        ])
    else
        if test -z "$with_xns" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--with xns"
        fi
    fi
])# _XNS_SETUP
# =========================================================================

# =========================================================================
# _XNS_
# -------------------------------------------------------------------------
AC_DEFUN([_XNS_], [dnl
])# _XNS_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
