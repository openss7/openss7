dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL  vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: xns.m4,v 0.9.2.11 2005/01/27 04:36:08 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
# _XNS
# -------------------------------------------------------------------------
# Check for the existence of XNS header files, particularly sys/npi.h.  NPI
# headers files are required for building the NPI interface for SCTP.  Without
# NPI header files, the NPI interface to SCTP will not be built.
# -------------------------------------------------------------------------
AC_DEFUN([_XNS], [dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _XNS_OPTIONS
    _XNS_SETUP
    AC_SUBST([XNS_CPPFLAGS])
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
    _XNS_CHECK_HEADERS
    _XNS_DEFINES
])# _XNS_SETUP
# =========================================================================

# =========================================================================
# _XNS_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_XNS_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS XNS header files.  The package
    # normally requires XNS header files to compile.
    AC_CACHE_CHECK([for xns include directory], [xns_cv_includes], [dnl
        if test ":${with_xns:-no}" != :no -a :"${with_xns:-no}" != :yes ;  then
            # First thing to do is to take user specified director(ies)
            xns_cv_includes="$with_xns"
        fi
        xns_what="sys/npi.h"
        if test ":${xns_cv_includes:-no}" = :no ; then
            # The next place to look now is for a peer package being built under
            # the same top directory, and then the higher level directory.
            xns_here=`pwd`
            xns_where="strxns/src/include"
            for xns_d in . .. ; do
                xns_dir="$srcdir/$xns_d/$xns_where"
                xns_bld=`echo "$xns_here/$xns_d/$xns_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                if test -d $xns_dir -a -r $xns_dir/$xns_what ; then
                    xns_dir=`(cd $xns_dir; pwd)`
                    xns_cv_includes="$xns_dir $xns_bld"
                    break
                fi
            done
        fi
        if test ":${xns_cv_includes:-no}" = :no ; then
            # XNS header files are normally found in the strxns package now.
            # They used to be part of the strxnet add-on package and even older
            # versions are part of the LiS or LfS release packages.
            eval "xns_search_path=\"
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$includedir/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$oldincludedir/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/include/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/local/include/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/src/strxns/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$includedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$oldincludedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/local/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/src/strxnet/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/strxns
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/strxns/src/include
                ${linux_cv_k_rootdir:-$DESTDIR}$includedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/strxnet
                ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/strxnet/src/include\""
            case "$streams_cv_package" in
                LiS)
                    # XNS header files used to be part of the LiS package.
                    eval "xns_search_path=\"$xns_search_path
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$includedir/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$oldincludedir/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/include/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/local/include/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/src/LiS/include
                        ${linux_cv_k_rootdir:-$DESTDIR}$includedir/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/LiS
                        ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/LiS/include\""
                    ;;
                LfS)
                    # XNS header files used to be part of the LfS package.
                    eval "xns_search_path=\"$xns_search_path
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$includedir/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix$oldincludedir/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/include/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/local/include/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}$linux_cv_k_prefix/usr/src/streams/include
                        ${linux_cv_k_rootdir:-$DESTDIR}$includedir/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}$oldincludedir/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}/usr/include/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}/usr/local/include/streams
                        ${linux_cv_k_rootdir:-$DESTDIR}/usr/src/streams/include\""
                    ;;
            esac
            xns_search_path=`echo "$xns_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            for xns_dir in $xns_search_path ; do
                if test -d "$xns_dir" -a -r "$xns_dir/$xns_what" ; then
                    xns_cv_includes="$xns_dir"
                    break
                fi
            done
        fi
    ])
dnl Older rpms (particularly those used by SuSE) rpms are too stupid to handle
dnl --with and --without rpmpopt syntax, so convert to the equivalent --define
dnl syntax Also, I don't know that even rpm 4.2 handles --with xxx=yyy
dnl properly, so we use defines.
    if test :"${xns_cv_includes:-no}" = :no ; then :
        if test :"$with_xns" = :no ; then
            AC_MSG_ERROR([
***
*** Could not find XNS include directories.  This package requires the
*** presence of XNS include directories to compile.  Specify the location of
*** XNS include directories with option --with-xns to configure and try again.
*** ])
        fi
        if test -z "$with_xns" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--define \"_with_xns --with-xns\""
        fi
    else
        if test -z "$with_xns" ; then
            PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--define \"_without_xns --without-xns\""
        fi
    fi
])# _XNS_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _XNS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_XNS_DEFINES], [dnl
    for xns_include in $xns_cv_includes ; do
        XNS_CPPFLAGS="${XNS_CPPFLAGS}${XNS_CPPFLAGS:+ }-I${xns_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
        Define for SuSv3.  This is necessary for LiS and LfS and strxns for
        that matter.
    ])
])# _XNS_DEFINES
# =========================================================================

# =========================================================================
# _XNS_
# -------------------------------------------------------------------------
AC_DEFUN([_XNS_], [dnl
])# _XNS_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
