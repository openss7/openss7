dnl =========================================================================
dnl
dnl @(#) $Id: xti.m4,v 0.9.2.4 2004/12/18 11:13:21 brian Exp $
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
dnl Last Modified $Date: 2004/12/18 11:13:21 $ by $Author: brian $
dnl 
dnl =========================================================================

# =========================================================================
# _XTI
# -------------------------------------------------------------------------
AC_DEFUN([_XTI], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_REQUIRE([_LINUX_STREAMS])dnl
    _XTI_OPTIONS
    _XTI_SETUP
    AC_SUBST([XTI_CPPFLAGS])
])# _XTI
# =========================================================================

# =========================================================================
# _XTI_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_XTI_OPTIONS], [dnl
    AC_ARG_WITH([xti],
                AC_HELP_STRING([--with-xti=HEADERS],
                               [specify the XTI header file directory.
                               @<:@default=$INCLUDEDIR/xti@:>@]),
                [with_xti="$withval"],
                [with_xti=''])
])# _XTI_OPTIONS
# =========================================================================

# =========================================================================
# _XTI_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_XTI_SETUP], [dnl
    _XTI_CHECK_HEADERS
    _XTI_DEFINES
])# _XTI_SETUP
# =========================================================================

# =========================================================================
# _XTI_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_XTI_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS XTI header files.  The package
    # normally requires either Linux STREAMS or Linux Fast-STREAMS XTI header
    # files (or both) to compile.
    AC_CACHE_CHECK([for xti include directory], [xti_cv_includes], [
        if test :"${with_xti:-no}" != :no -a :"${with_xti:-no}" != :yes ; then
            xti_cv_includes="$with_xti"
        else
            eval "xti_search_path=\"
                $linux_cv_k_rootdir$includedir/strxnet
                $linux_cv_k_rootdir$linux_cv_k_prefix$oldincludedir/strxnet
                $linux_cv_k_rootdir$linux_cv_k_prefix/usr/include/strxnet
                $linux_cv_k_rootdir$linux_cv_k_prefix/usr/local/include/strxnet
                $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/strxnet/src/include
                $linux_cv_k_rootdir$oldincludedir/strxnet
                $linux_cv_k_rootdir/usr/include/strxnet
                $linux_cv_k_rootdir/usr/local/include/strxnet
                $linux_cv_k_rootdir/usr/src/strxnet/src/include \""
            case "$streams_cv_package" in
                LiS)
                    # Some of our oldest RPM releases of LiS put the xti header files into their own
                    # subdirectory (/usr/include/xti/).  The old version places them in with the LiS
                    # header files.  The current version has them separate as part of the strxnet
                    # package.  This tests whether we need an additional -I/usr/include/xti in the
                    # streams includes line.  This check can be dropped when the older RPM releases
                    # of LiS fall out of favor.
                    eval "xti_search_path=\"$xti_search_path
                        $linux_cv_k_rootdir$linux_cv_k_prefix$oldincludedir/LiS/xti
                        $linux_cv_k_rootdir$linux_cv_k_prefix/usr/include/LiS/xti
                        $linux_cv_k_rootdir$linux_cv_k_prefix/usr/local/include/LiS/xti
                        $linux_cv_k_rootdir$linux_cv_k_prefix/usr/src/LiS/include/xti
                        $linux_cv_k_rootdir$oldincludedir/LiS/xti
                        $linux_cv_k_rootdir/usr/include/LiS/xti
                        $linux_cv_k_rootdir/usr/local/include/LiS/xti
                        $linux_cv_k_rootdir/usr/src/LiS/include/xti
                        $linux_cv_k_rootdir$linux_cv_k_prefix$oldincludedir/xti
                        $linux_cv_k_rootdir$linux_cv_k_prefix/usr/include/xti
                        $linux_cv_k_rootdir$linux_cv_k_prefix/usr/local/include/xti
                        $linux_cv_k_rootdir$oldincludedir/xti
                        $linux_cv_k_rootdir/usr/include/xti
                        $linux_cv_k_rootdir/usr/local/include/xti\""
                    ;;
                LfS)
                    # LfS has always been separate.
                    ;;
            esac
            xti_search_path=`echo "$xti_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            for xti_dir in $xti_search_path ; do
                if test -d "$xti_dir" -a -r "$xti_dir/xti.h" ; then
                    xti_cv_includes="$xti_dir"
                    break
                fi
            done
        fi
        if test :"${xti_cv_includes:-no}" = :no ; then
            AC_MSG_RESULT([ERROR])
            AC_MSG_ERROR([
*** 
*** Could not find XTI include directories.  This package requires the
*** presence of XTI include directories to compile.  Specify the location of
*** XTI include directories with option --with-xti to configure and try again.
*** ])
        fi
    ])
])# _XTI_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _XTI_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_XTI_DEFINES], [dnl
    for xti_include in $xti_cv_includes ; do
        XTI_CPPFLAGS="${XTI_CPPFLAGS}${XTI_CPPFLAGS:+ }-I${xti_include}"
    done
    AC_DEFINE_UNQUOTED([_XOPEN_SOURCE], [600], [dnl
        Define for SuSv3.  This is necessary for LiS and LfS and strxnet for
        that matter.
    ])
])# _XTI_DEFINES
# =========================================================================

# =========================================================================
# _XTI_
# -------------------------------------------------------------------------
AC_DEFUN([_XTI_], [dnl
])# _XTI_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
