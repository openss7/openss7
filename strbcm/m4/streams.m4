dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: streams.m4,v 0.9 2004/04/03 22:37:07 brian Exp $
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
dnl Last Modified $Date: 2004/04/03 22:37:07 $ by $Author: brian $
dnl 
dnl =========================================================================

dnl -------------------------------------------------------------------------
dnl This file provides some common macros for finding an LiS release and
dnl necessary include directories and other configuration for compiling kernel
dnl modules to run with LiS.  Eventually, this file will determine whether
dnl GCOM's LiS is being used or whether OpenSS7's LfS (Linux Fast STREAMS) is
dnl being used.
dnl -------------------------------------------------------------------------

# =========================================================================
# AC_LINUX_STREAMS
# -------------------------------------------------------------------------
AC_DEFUN([AC_LINUX_STREAMS],
[
    AC_REQUIRE([AC_LINUX_KERNEL])
    _STREAMS_OPTIONS
    _STREAMS_SETUP
])# AC_LINUX_STREAMS
# =========================================================================

# =========================================================================
# _STREAMS_LIS_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN([_STREAMS_],
[
    AC_ARG_WITH([lis],
                AC_HELP_STRING([--with-lis=HEADERS],
                               [specify the LiS header file directory.
                                @<:@default=INCLUDEDIR/LiS@:>@]),
                [with_lis=$withval],
                [with_lis=''])
])# _STREAMS_
# =========================================================================

# =========================================================================
# _STREAMS_SETUP
# -------------------------------------------------------------------------
AC_DEFUN([_STREAMS_SETUP],
[
    _STREAMS_LIS_CHECK_HEADERS
    _STREAMS_LIS_DEFINES
])# _STREAMS_SETUP
# =========================================================================

# =========================================================================
# _STREAMS_LIS_CHECK_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN([_STREAMS_LIS_CHECK_HEADERS],
[
    AC_CACHE_CHECK([for LiS header files], [streams_cv_includes], [dnl
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
	streams_cv_includes=
	for lis_dir in $lis_search_path
	do
	    if test -d $lis_dir -a -r $lis_dir/stropts.h
	    then
		streams_cv_includes="$lis_dir"
		break
	    fi
	done
    ])
    if test :"${streams_cv_includes:-no}" = :no
    then
	AC_MSG_ERROR([
*** 
*** Could not find LiS include directories.  This package requires the
*** presence of LiS include directories to compile.  Specify the location of
*** LiS include directories with option --with-lis to configure and try again.
*** 
	])
    else
	AC_DEFINE([_LIS_SOURCE], [1], [Define when compiling for LiS.])
    fi
])# _STREAMS_LIS_CHECK_HEADERS
# =========================================================================

# =========================================================================
# _STREAMS_LIS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN([_STREAMS_LIS_DEFINES],
[
    AC_REQUIRE([AC_LINUX_KERNEL])
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
	    AC_DEFINE([_PPC_LIS_], [1], [Define when compiling for PPC.  This
		define is only used for linux kernel target.  This is really the
		wrong way to go about doing this: the function should be checked
		for by autoconf instead of placing the architectural dependencies
		in the LiS source.  The define is used in <LiS/include/sys/osif.h>
		and "head/osif.c" to determine whether PCI BIOS is present; in
		(head/linux-mdep.c) to determine whether cpu binding is possible;
		to determine whether spin_is_locked() is available in
		"head/linux/lislocks.c"; in "head/mod.c" to determine whether to
		define struct pt_regs; and in <LiS/include/sys/lislocks.h> to
		determine the size of semaphore memory.])
	    ;;
	s390x*)			
	    AC_DEFINE([_S390X_LIS_], [1], [Define when compiling for S390X.  This
		define is only used for the linux kernel target.  This is really
		the wrong way to go about doing this: the function should be
		checked for by autoconf instead of placing the architectural
		depdendencies in the LiS source.  The define is used in
		"head/linux-mdep.c" to determine whether lis_pci_cleanup exists;
		"head/linux/exports.c" to determine whether a bunch of functions
		are available; "head/osif.c" to determine whether a bunch of PCI
		DMA mapping functions are available; "include/sys/osif.h" to
		determine whether a bunch of PCI DMA mapping functions are
		available.])
	    ;;
	s390*)			
	    AC_DEFINE([_S390_LIS_], [1], [Define when compiling for S390.
		Strangely enough, _S390_LIS_ is never checked without _S390X_LIS_.
		Rendering it as an alias for the above.])
	    ;;
	sh*)			: ;;
	sparc64*)		
	    AC_DEFINE([_SPARC64_LIS_], [1], [Define when compiling for Sparc64.
		This define is only used for the linux kernel target.  This is
		really the wrong way to go about doing this: the function should
		be checked for by autoconf instead of placing the architectural
		dependencies in the LiS source.  The define is used to determine
		when ioremap functions are not available <LiS/include/osif.h>.
		Strangely enough, none of the other checks are performed as for
		_SPARC_LIS_ below.])
	    ;;
	sparc*)			
	    AC_DEFINE([_SPARC_LIS_], [1], [Define when compiling for Sparc.  This
		define is used for the linux kernel target.  This is really the
		wrong way to go about doing this: the function should be checked
		for by autoconf instead of placing architectural depedencies in
		the LiS source.  The define is used to determine when ioremap
		functions are not available <LiS/include/osif.h>, when PCI BIOS is
		not present (head/osif.c), and when <linux/poll.h> is missing
		POLLMSG <LiS/include/sys/poll.h>])
	    ;;
	*)			: ;;
    esac
])# _STREAMS_LIS_DEFINES
# =========================================================================

# =========================================================================
# _STREAMS_
# -------------------------------------------------------------------------
AC_DEFUN([_STREAMS_],
[
])# _STREAMS_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
