dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et nocindent
dnl =========================================================================
dnl
dnl @(#) $Id: streams.m4,v 0.9.2.1 2004/04/05 12:39:05 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
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
dnl Last Modified $Date: 2004/04/05 12:39:05 $ by $Author: brian $
dnl 
dnl =========================================================================

dnl -------------------------------------------------------------------------
dnl This file provides some common macros for finding an LiS release and
dnl necessary include directories and other configuration for compiling kernel
dnl modules to run with LiS.  Eventually, this file will determine whether
dnl GCOM's LiS is being used or whether OpenSS7's LfS (Linux Fast STREAMS) is
dnl being used.
dnl -------------------------------------------------------------------------
dnl Interesting enough, there is no need to have LiS or LfS loaded on the build
dnl machine to compile modules.  Only the proper header files are required.  It
dnl is, not a good idea to install both sets of kernel modules for the same
dnl kernel.  For an rpm build, whether kernel modules are being built for LiS or
dnl LfS is determined using the the with_lis or with_lfs flags to the build.  If
dnl both are specified, it defaults to LfS.
dnl -------------------------------------------------------------------------

# =========================================================================
# AC_LINUX_STREAMS
# -------------------------------------------------------------------------
AC_DEFUN([AC_LINUX_STREAMS],
[
    AC_REQUIRE([AC_LINUX_KERNEL])
    _LINUX_STREAMS_OPTIONS
    _LINUX_STREAMS_SETUP
    _LINUX_STREAMS_OUTPUT
])# AC_LINUX_STREAMS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_OPTIONS],
[
    AC_ARG_WITH([lis],
        AC_HELP_STRING([--with-lis=HEADERS], [specify the LiS header file
            directory.  @<:@default=INCLUDEDIR/LiS@:>@]),
            [with_lis=$withval],
            [with_lis=''])
    AC_ARG_WITH([lfs],
        AC_HELP_STRING([--with-lfs=HEADERS], [specify the LfS header file
            directory. @<:@default=INCLUDEDIR/LfS@:>@]),
            [with_lfs=$withval],
            [with_lfs=''])
])# _LINUX_STREAMS_OPTIONS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_SETUP],
[
    AC_CACHE_CHECK([for kernel STREAMS header files], [streams_cv_includes], [dnl
        if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no ; then
            _LINUX_STREAMS_LIS_CHECK_HEADERS
            if test :"${lis_cv_includes:-no}" != :no ; then
                streams_cv_includes="$lis_cv_includes"
                streams_cv_package="LiS"
            fi
        fi
        if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no ; then
            _LINUX_STREAMS_LFS_CHECK_HEADERS
            if test :"${lfs_cv_includes:-no}" != :no ; then
                streams_cv_includes="$lfs_cv_includes"
                streams_cv_package="LFS"
            fi
        fi
        if test :"${streams_cv_includes:-no}" = :no ; then
            if test :"${with_lis:-no}" != :no ; then
                AC_MSG_ERROR([
*** 
*** Linux GCOM STREAMS was specified with the --with-lis flag, however,
*** configure could not find the LiS include directories.  This package requires
*** the presense of LiS include directories when the --with-lis flag is
*** specified.  Specify the correct location of LiS include directories with the
*** argument to option --with-lis to configure and try again.
*** 
                ])
            fi
            if test :"${with_lfs:-no}" != :no ; then
                AC_MSG_ERROR([
*** 
*** Linux Fast STREAMS was specified with the --with-lfs flag, however,
*** configure could not find the LfS include directories.  This package requires
*** the presense of LfS include directories when the --with-lfs flag is
*** specified.  Specify the correct location of LfS include directories with the
*** argument to option --with-lfs to configure and try again.
*** 
                ])
            fi
            AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS include directories.  This package
*** requires the presence of STREAMS include directories.  Specify the correct
*** location of Linux GCOM STREAMS (LiS) include directories with the --with-lis
*** option to configure, or the correct location of Linux Fast STREAMS (LfS)
*** include directories with the --with-lfs option to configure, and try again.
*** 
            ])
        fi
    ])
    STREAMS_CPPFLAGS="${STREAMS_CPPFLAGS}${STREAMS_CPPFLAGS:+ }-I${streams_cv_includes}"
])# _LINUX_STREAMS_SETUP
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LIS_CHECK_HEADERS
# -------------------------------------------------------------------------
# Test for the existence of Linux STREAMS header files.  The package normally
# requires either Linux STREAMS or Linux Fast-STREAMS header files (or both)
# to compile.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LIS_CHECK_HEADERS],
[
    lis_cv_includes=
    if test :"${cross_compiling:-no}" = :no -a :"${with_k_release:-no}" = :no
    then
        # compiling for the running kernel
        eval "lis_search_path=\"
            ${with_lis}
            ${includedir}/LiS
            ${oldincludedir}/LiS
            /usr/include/LiS
            /usr/local/include/LiS
            /usr/src/LiS/include
            \""
    else
        # building for another environment
        eval "lis_search_path=\"
            ${with_lis}
            ${linux_cv_module_prefix}/${includedir}/LiS
            ${linux_cv_module_prefix}/${oldincludedir}/LiS
            ${linux_cv_module_prefix}/usr/include/LiS
            ${linux_cv_module_prefix}/usr/local/include/LiS
            ${linux_cv_module_prefix}/usr/src/LiS/include
            ${includedir}/LiS
            ${oldincludedir}/LiS
            /usr/src/LiS/include
            \""
    fi
    lis_search_path=`echo "$lis_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
    for lis_dir in $lis_search_path ; do
        if test -d $lis_dir -a -r $lis_dir/sys/stream.h ; then
            lis_cv_includes="$lis_dir"
            break
        fi
    done
])# _LINUX_STREAMS_LIS_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LFS_CHECK_HEADERS
# -------------------------------------------------------------------------
# Test for the existence of Linux Fast-STREAMS header files.  The package
# normally requires either Linux STREAMS or Linux Fast-STREAMS header files
# (or both) to compile.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LFS_CHECK_HEADERS],
[
    lfs_cv_includes=
    if test :"${cross_compiling:-no}" = :no -a :"${with_k_release:-no}" = :no ; then
        # compiling for the running kernel
        eval "lfs_search_path=\"
            ${with_lfs}
            ${includedir}
            ${oldincludedir}
            /usr/include
            /usr/local/include
            /usr/src/streams/src/include
            \""
    else
        # building for another environment
        eval "lfs_search_path=\"
            ${with_lfs}
            ${linux_cv_module_prefix}/${includedir}
            ${linux_cv_module_prefix}/${oldincludedir}
            ${linux_cv_module_prefix}/usr/include
            ${linux_cv_module_prefix}/usr/local/include
            ${linux_cv_module_prefix}/usr/src/streams/src/include
            ${includedir}
            ${oldincludedir}
            /usr/src/streams/src/include
            \""
    fi
    lfs_search_path=`echo "$lfs_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
    for lfs_dir in $lfs_search_path ; do
        if test -d $lfs_dir -a -r $lfs_dir/sys/stream.h ; then
            lfs_cv_includes="$lfs_dir"
            break
        fi
    done
])# _LINUX_STREAMS_LFS_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_OUTPUT],
[
    case "$streams_cv_package" in
        LiS)
            _LINUX_STREAMS_LIS_DEFINES
            : ;;
        LfS)
            _LINUX_STREAMS_LFS_DEFINES
            : ;;
    esac
    AC_SUBST([STREAMS_CPPFLAGS])
])# _LINUX_STREAMS_OUTPUT
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LIS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LIS_DEFINES],
[
    case "$linux_cv_march" in
	alpha*)			: ;;
	arm*)			: ;;
	cris*)			: ;;
	i?86* | k6* | athlon*)	: ;;
	ia64)			: ;;
	m68*)			: ;;
	mips64*)		: ;;
	mips*)			: ;;
	hppa*)			: ;;
	ppc* | powerpc*)	
            # Define when compiling for PPC.  This define is only used for linux
            # kernel target.  This is really the wrong way to go about doing
            # this: the function should be checked for by autoconf instead of
            # placing the architectural dependencies in the LiS source.  The
            # define is used in <LiS/include/sys/osif.h> and "head/osif.c" to
            # determine whether PCI BIOS is present; in (head/linux-mdep.c) to
            # determine whether cpu binding is possible; to determine whether
            # spin_is_locked() is available in "head/linux/lislocks.c"; in
            # "head/mod.c" to determine whether to define struct pt_regs; and in
            # <LiS/include/sys/lislocks.h> to determine the size of semaphore
            # memory.
            #
            STREAMS_CPPFLAGS="-D_PPC_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	s390x*)			
            # Define when compiling for S390X.  This define is only used for the
            # linux kernel target.  This is really the wrong way to go about
            # doing this: the function should be checked for by autoconf instead
            # of placing the architectural depdendencies in the LiS source.  The
            # define is used in "head/linux-mdep.c" to determine whether
            # lis_pci_cleanup exists; "head/linux/exports.c" to determine
            # whether a bunch of functions are available; "head/osif.c" to
            # determine whether a bunch of PCI DMA mapping functions are
            # available; "include/sys/osif.h" to determine whether a bunch of
            # PCI DMA mapping functions are available.
            #
            STREAMS_CPPFLAGS="-D_S390X_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	s390*)			
            # Define when compiling for S390.  Strangely enough, _S390_LIS_ is
            # never checked without _S390X_LIS_.  Rendering it as an alias for
            # the above.
            #
            STREAMS_CPPFLAGS="-D_S390_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	sh*)			: ;;
	sparc64*)		
            # Define when compiling for Sparc64.  This define is only used for
            # the linux kernel target.  This is really the wrong way to go about
            # doing this: the function should be checked for by autoconf instead
            # of placing the architectural dependencies in the LiS source.  The
            # define is used to determine when ioremap functions are not
            # available <LiS/include/osif.h>.  Strangely enough, none of the
            # other checks are performed as for _SPARC_LIS_ below.
	    #
            STREAMS_CPPFLAGS="-D_SPARC64_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	sparc*)			
            # Define when compiling for Sparc.  This define is used for the
            # linux kernel target.  This is really the wrong way to go about
            # doing this: the function should be checked for by autoconf instead
            # of placing architectural depedencies in the LiS source.  The
            # define is used to determine when ioremap functions are not
            # available <LiS/include/osif.h>, when PCI BIOS is not present
            # (head/osif.c), and when <linux/poll.h> is missing POLLMSG
            # <LiS/include/sys/poll.h>
            #
            STREAMS_CPPFLAGS="-D_SPARC_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	*)			: ;;
    esac
    STREAMS_CPPFLAGS="-D_LIS_SOURCE${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
])# _LINUX_STREAMS_LIS_DEFINES
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LFS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LFS_DEFINES],
[
    STREAMS_CPPFLAGS="-D_LFS_SOURCE${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
])# _LINUX_STREAMS_LFS_DEFINES
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_],
[
])# _LINUX_STREAMS_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et nocindent
dnl =========================================================================
