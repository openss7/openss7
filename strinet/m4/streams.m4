dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et nocindent
dnl =========================================================================
dnl
dnl @(#) $Id: streams.m4,v 0.9.2.30 2005/01/23 08:26:00 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (c) 2001-2005  OpenSS7 Corp. <http://www.openss7.com>
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
dnl Last Modified $Date: 2005/01/23 08:26:00 $ by $Author: brian $
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
# _LINUX_STREAMS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS], [dnl
    _LINUX_STREAMS_OPTIONS
    _LINUX_STREAMS_SETUP
    _LINUX_STREAMS_OUTPUT
    AC_SUBST([STREAMS_CPPFLAGS])
    AC_SUBST([STREAMS_LDADD])
])# _LINUX_STREAMS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_OPTIONS], [dnl
    AC_ARG_WITH([lis],
        AS_HELP_STRING([--with-lis=HEADERS],
            [specify the LiS header file directory.  @<:@default=INCLUDEDIR/LiS@:>@]),
        [with_lis="$withval"],
        [with_lis=''])
    AC_ARG_WITH([lfs],
        AS_HELP_STRING([--with-lfs=HEADERS],
            [specify the LfS header file directory.  @<:@default=INCLUDEDIR/LfS@:>@]),
        [with_lfs="$withval"],
        [with_lfs=''])
])# _LINUX_STREAMS_OPTIONS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_SETUP], [dnl
    if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no
    then
        _LINUX_STREAMS_LIS_CHECK_HEADERS
    fi
    if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no 
    then
        _LINUX_STREAMS_LFS_CHECK_HEADERS
    fi
    AC_CACHE_CHECK([for streams include directory], [streams_cv_includes], [dnl
        if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no 
        then
            if test :"${streams_cv_lis_includes:-no}" != :no 
            then
                streams_cv_includes="$streams_cv_lis_includes"
            fi
        fi
        if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no 
        then
            if test :"${streams_cv_lfs_includes:-no}" != :no 
            then
                streams_cv_includes="$streams_cv_lfs_includes"
            fi
        fi
    ])
    AC_CACHE_CHECK([for streams package], [streams_cv_package], [dnl
        if test :"${with_lis:-no}" != :no -o :"${with_lfs:-no}" = :no 
        then
            if test :"${streams_cv_lis_includes:-no}" != :no 
            then
                streams_cv_package="LiS"
            fi
        fi
        if test :"${with_lfs:-no}" != :no -o :"${with_lis:-no}" = :no 
        then
            if test :"${streams_cv_lfs_includes:-no}" != :no 
            then
                streams_cv_package="LfS"
            fi
        fi
    ])
    # need to add arguments for LiS or LfS so they will be passed to rpm
    AC_MSG_CHECKING([for streams added configure arguments])
    case "$streams_cv_package" in
        LiS)
            if test -z "$with_lis" 
            then :;
dnl             PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--define '_with_lis --with-lis'"
dnl             ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--with-lis"
            fi
            AC_MSG_RESULT([--with-lis])
            ;;
        LfS)
            if test -z "$with_lfs" 
            then :;
dnl             PACKAGE_OPTIONS="${PACKAGE_OPTIONS}${PACKAGE_OPTIONS:+ }--define '_with_lfs --with-lfs'"
dnl             ac_configure_args="${ac_configure_args}${ac_configure_args:+ }--with-lfs"
            fi
            AC_MSG_RESULT([--with-lfs])
            ;;
        *)
            if test :"${with_lis:-no}" != :no 
            then
                AC_MSG_ERROR([
*** 
*** Linux GCOM STREAMS was specified with the --with-lis flag, however,
*** configure could not find the LiS include directories.  This package requires
*** the presense of LiS include directories when the --with-lis flag is
*** specified.  Specify the correct location of LiS include directories with the
*** argument to option --with-lis to configure and try again.
*** ])
            fi
            if test :"${with_lfs:-no}" != :no 
            then
                AC_MSG_ERROR([
*** 
*** Linux Fast STREAMS was specified with the --with-lfs flag, however,
*** configure could not find the LfS include directories.  This package requires
*** the presense of LfS include directories when the --with-lfs flag is
*** specified.  Specify the correct location of LfS include directories with the
*** argument to option --with-lfs to configure and try again.
*** ])
            fi
            AC_MSG_ERROR([
*** 
*** Configure could not find the STREAMS include directories.  This package
*** requires the presence of STREAMS include directories.  Specify the correct
*** location of Linux GCOM STREAMS (LiS) include directories with the --with-lis
*** option to configure, or the correct location of Linux Fast STREAMS (LfS)
*** include directories with the --with-lfs option to configure, and try again.
*** ])
            ;;
    esac
    for streams_include in $streams_cv_includes 
    do
        STREAMS_CPPFLAGS="${STREAMS_CPPFLAGS}${STREAMS_CPPFLAGS:+ }-I${streams_include}"
    done
    AM_CONDITIONAL([WITH_LIS], [test :"${streams_cv_lis_includes:-no}" != :no])
    AM_CONDITIONAL([WITH_LFS], [test :"${streams_cv_lfs_includes:-no}" != :no])
])# _LINUX_STREAMS_SETUP
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LIS_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LIS_CHECK_HEADERS], [dnl
    # Test for the existence of Linux STREAMS header files.  The package normally requires either
    # Linux STREAMS or Linux Fast-STREAMS header files (or both) to compile.
    AC_CACHE_CHECK([for streams lis include directory], [streams_cv_lis_includes], [dnl
        if test :"${with_lis:-no}" != :no -a :"${with_lis:-no}" != :yes 
        then
            streams_cv_lis_includes="$with_lis"
        fi
        streams_what="sys/stream.h"
        if test ":${streams_cv_lis_includes:-no}" = :no 
        then
            # The next place to look now is for a peer package being built under
            # the same top directory, and then the higher level directory.
            streams_here=`pwd`
            streams_where="LiS/include"
            for streams_d in . .. 
            do
                streams_dir=`echo "$srcdir/$streams_d/$streams_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                streams_bld=`echo "$streams_here/$streams_d/$streams_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                if test -d $streams_dir -a -r $streams_dir/$streams_what 
                then
                    streams_cv_lis_includes="$streams_dir $streams_bld"
                    streams_cv_lis_ldadd=`echo "$streams_here/$streams_d/LiS/libLiS.la" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                    break
                fi
            done
        fi
        if test ":${streams_cv_lis_includes:-no}" = :no 
        then
            # note if linux kernel macros have not run this reduces
            streams_cv_lis_includes=
            eval "streams_search_path=\"
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
            streams_search_path=`echo "$streams_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            for streams_dir in $streams_search_path 
            do
                if test -d "$streams_dir" -a -r "$streams_dir/$streams_what"
                then
                    streams_cv_lis_includes="$streams_dir"
                    streams_cv_lis_ldadd="-lLiS"
                    break
                fi
            done
        fi
    ])
    if test :"${streams_cv_lis_includes:-no}" = :no 
    then
        AC_MSG_WARN([
*** 
*** Configure could not find the LiS STREAMS include directories.  If you
*** wish to use the LiS STREAMS package you will need to specify the location
*** of the Linux STREAMS (LiS) include directories with the --with-lis
*** option to configure and try again.  Perhaps you just forgot to load the
*** LiS STREAMS package?  The LiS STREAMS package is available from the
*** download page at http://www.openss7.org/ and comes in a tarball named
*** something like "LiS-2.16.18-22.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for streams lis sys/LiS/modversions.h], [streams_cv_lis_modversions], [dnl
        streams_cv_lis_modversions='no'
        if test -n "$streams_cv_lis_includes"
        then
            # old place for modversions
            if test -f "$streams_dir/sys/LiS/modversions.h" 
            then
                streams_cv_lis_modversions='yes'
                break
            fi
            # new place for modversions
            if test -n $linux_cv_k_release 
            then
dnl             if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
                if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/sys/LiS/modversions.h" 
                then
                    streams_cv_lis_includes="$streams_dir/$linux_cv_k_release/$target_cpu $streams_cv_lis_includes"
                    streams_cv_lis_modversions='yes'
                    break
                fi
            fi
        fi
    ])
    AC_CACHE_CHECK([for streams lis sys/LiS/module.h], [streams_cv_lis_module], [dnl
        streams_cv_lis_module='no'
        if test -n "$streams_cv_lis_includes"
        then
            for streams_dir in $streams_cv_lis_includes
            do
                if test -f "$streams_dir/sys/LiS/module.h" 
                then
                    streams_cv_lis_module='yes'
                    break
                fi
            done
        fi
    ])
])# _LINUX_STREAMS_LIS_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LFS_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LFS_CHECK_HEADERS], [dnl
    # Test for the existence of Linux Fast-STREAMS header files.  The package normally requires
    # either Linux STREAMS or Linux Fast-STREAMS header files (or both) to compile.
    AC_CACHE_CHECK([for streams lfs include directory], [streams_cv_lfs_includes], [dnl
        if test :"${with_lfs:-no}" != :no -a :"${with_lfs:-no}" != :yes
        then
            streams_cv_lfs_includes="$with_lfs"
        fi
        streams_what="sys/stream.h"
        if test ":${streams_cv_lfs_includes:-no}" = :no 
        then
            # The next place to look now is for a peer package being built under
            # the same top directory, and then the higher level directory.
            streams_here=`pwd`
            streams_where="streams/include"
            for streams_d in . .. 
            do
                streams_dir=`echo "$srcdir/$streams_d/$streams_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                streams_bld=`echo "$streams_here/$streams_d/$streams_where" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                if test -d $streams_dir -a -r $streams_dir/$streams_what 
                then
                    streams_cv_lfs_includes="$streams_dir $streams_bld"
                    streams_cv_lfs_ldadd=`echo "$streams_here/$streams_d/streams/libstreams.la" | sed -e 's|[[^ /\.]][[^ /\.]]*/\.\./||g;s|/\./|/|g;s|//|/|g;'`
                    break
                fi
            done
        fi
        if test ":${streams_cv_lfs_includes:-no}" = :no 
        then
            # note if linux kernel macros have not run this reduces
            streams_cv_lfs_includes=
            eval "streams_search_path=\"
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
            streams_search_path=`echo "$streams_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            for streams_dir in $streams_search_path 
            do
                if test -d "$streams_dir" -a -r "$streams_dir/$streams_what"
                then
                    streams_cv_lfs_includes="$streams_dir"
                    streams_cv_lfs_ldadd="-lstreams"
                    break
                fi
            done
        fi
    ])
    if test :"${streams_cv_lfs_includes:-no}" = :no
    then
        AC_MSG_WARN([
*** 
*** Configure could not find the LfS STREAMS include directories.  If you
*** wish to use the LfS STREAMS package you will need to specify the location
*** of the Linux Fast STREAMS (LfS) include directories with the --with-lfs
*** option to configure and try again.  Perhaps you just forgot to load the
*** LfS STREAMS package?  The LfS STREAMS package is available from the
*** download page at http://www.openss7.org/ and comes in a tarball named
*** something like "streams-0.7a-1.tar.gz".
*** ])
    fi
    AC_CACHE_CHECK([for streams lfs sys/streams/modversions.h], [streams_cv_lfs_modversions], [dnl
        streams_cv_lfs_modversions='no'
        if test -n "$streams_cv_lfs_includes"
        then
            for streams_dir in $streams_cv_lfs_includes
            do
                # old place for modversions
                if test -f "$streams_dir/sys/streams/modversions.h" 
                then
                    streams_cv_lfs_modversions='yes'
                    break
                fi
                # new place for modversions
                if test -n $linux_cv_k_release 
                then
dnl                 if linux_cv_k_release is not defined (no _LINUX_KERNEL) then this will just not be set
                    if test -f "$streams_dir/$linux_cv_k_release/$target_cpu/sys/streams/modversions.h" 
                    then
                        streams_cv_lfs_includes="$streams_dir/$linux_cv_k_release/$target_cpu $streams_cv_lfs_includes"
                        streams_cv_lfs_modversions='yes'
                        break
                    fi
                fi
            done
        fi
    ])
])# _LINUX_STREAMS_LFS_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_OUTPUT
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_OUTPUT], [dnl
    case "$streams_cv_package" in
        LiS)
            _LINUX_STREAMS_LIS_DEFINES
            : ;;
        LfS)
            _LINUX_STREAMS_LFS_DEFINES
            : ;;
    esac
])# _LINUX_STREAMS_OUTPUT
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LIS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LIS_DEFINES], [dnl
    if test :"${streams_cv_lis_modversions:-no}" != :no 
    then
        AC_DEFINE_UNQUOTED([HAVE_SYS_LIS_MODVERSIONS_H], [], [Define when the
            LiS release supports module versions such as the OpenSS7 autoconf
            release of LiS.])
    fi
    if test :"${streams_cv_lis_module:-no}" != :no 
    then
        AC_DEFINE_UNQUOTED([HAVE_SYS_LIS_MODULE_H], [], [Define when the LiS
            release provides its own module.h file such as 2.17 GCOM LiS
            releases.])
    fi
    case "$target_cpu" in
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
            STREAMS_CPPFLAGS="-D_S390X_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	s390*)			
            # Define when compiling for S390.  Strangely enough, _S390_LIS_ is
            # never checked without _S390X_LIS_.  Rendering it as an alias for
            # the above.
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
            STREAMS_CPPFLAGS="-D_SPARC_LIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
	    ;;
	*)			: ;;
    esac
    STREAMS_CPPFLAGS="-DLIS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    STREAMS_LDADD="$streams_cv_lis_ldadd"
])# _LINUX_STREAMS_LIS_DEFINES
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_LFS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_LFS_DEFINES], [dnl
    if test :"${streams_cv_lfs_modversions:-no}" != :no 
    then
        AC_DEFINE_UNQUOTED([HAVE_SYS_STREAMS_MODVERSIONS_H], [], [Define when
            the Linux Fast-STREAMS release supports module versions such as
            the OpenSS7 autoconf releases.])
    fi
    STREAMS_CPPFLAGS="-DLFS${STREAMS_CPPFLAGS:+ }${STREAMS_CPPFLAGS}"
    STREAMS_LDADD="$streams_cv_lfs_ldadd"
    AC_DEFINE_UNQUOTED([HAVE_BCID_T], [], [Linux Fast-STREAMS has this type.])
    AC_DEFINE_UNQUOTED([HAVE_BUFCALL_ID_T], [], [Linux Fast-STREAMS has this type.])
])# _LINUX_STREAMS_LFS_DEFINES
# =========================================================================

# =========================================================================
# _LINUX_STREAMS_
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_STREAMS_], [dnl
])# _LINUX_STREAMS_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et nocindent
dnl =========================================================================
