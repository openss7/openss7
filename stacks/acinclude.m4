dnl =========================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
dnl
dnl @(#) $Id: acinclude.m4,v 0.9.2.1 2004/02/17 06:32:11 brian Exp $
dnl
dnl =========================================================================
dnl
dnl Copyright (C) 2001-2004  OpenSS7 Corp. <http://www.openss7.com>
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
dnl Last Modified $Date: 2004/02/17 06:32:11 $ by $Author: brian $
dnl 
dnl =========================================================================

m4_include([m4/kernel.m4])
m4_include([m4/rpm.m4])

# =========================================================================
# AC_SS7
# -------------------------------------------------------------------------
AC_DEFUN(AC_SS7,
[
    _SS7_OPTIONS
    _SS7_SETUP_PUBLIC
    _SS7_SETUP_DEBUG
    _SS7_CHECK_LIS_HEADERS
    _SS7_CHECK_XTI_HEADERS
    SS7_INCLUDES="-I- -imacros ./config.h -I./src/include -I${srcdir}/src/include${ss7_cv_lis_includes:+ -I}${ss7_cv_lis_includes} ${ss7_cv_xti_includes:+ -I}${ss7_cv_xti_includes}"
    # user CPPFLAGS and CFLAGS
    USER_CPPFLAGS="${CPPFLAGS}"
    USER_CFLAGS="${CFLAGS}"
    AC_SUBST([USER_CPPFLAGS])
    AC_SUBST([USER_CFLAGS])
    AC_SUBST([SS7_INCLUDES])
    AC_MSG_NOTICE([final user CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user INCLUDES  = $SS7_INCLUDES])
    _SS7_SETUP
    AC_MSG_NOTICE([final kern MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kern NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kern CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kern CFLAGS    = $KERNEL_CFLAGS])
    CPPFLAGS=
    CFLAGS=
    AC_RPM_SPEC
])# AC_SS7
# =========================================================================

# =========================================================================
# _SS7_OPTIONS
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_OPTIONS,
[
    AC_ARG_WITH([lis],
		AC_HELP_STRING([--with-lis=HEADERS],
			       [specify the LiS header file directory.
			        @<:@default=INCLUDEDIR/LiS@:>@]),
		[with_lis=$withval],
		[with_lis=''])
    AC_ARG_ENABLE([inet],
                  AC_HELP_STRING([--enable-inet],
                                 [enable inet package. @<:@default=no@:>@]),
                  [enable_inet=$enableval],
                  [enable_inet=''])
    AC_ARG_ENABLE([sctp2],
                  AC_HELP_STRING([--enable-sctp2],
                                 [enable sctp2 package. @<:@default=no@:>@]),
                  [enable_sctp2=$enableval],
                  [enable_sctp2=''])
    AC_ARG_ENABLE([public],
                  AC_HELP_STRING([--enable-public],
                                 [enable public release. @<:@default=yes@:>@]),
                  [enable_public=$enableval],
                  [enable_public=''])
    AC_ARG_ENABLE([k-debug],
                  AC_HELP_STRING([--enable-k-debug],
                                 [enable kernel module run-time debugging.
                                 @<:@default=no@:>@]),
                  [enable_k_debug=$enableval],
                  [enable_k_debug=''])
    AC_ARG_ENABLE([k-test],
                  AC_HELP_STRING([--enable-k-test],
                                 [enable kernel module run-time testing.
                                 @<:@default=no@:>@]),
                  [enable_k_test=$enableval],
                  [enable_k_test=''])
    AC_ARG_ENABLE([k-safe],
                  AC_HELP_STRING([--enable-k-safe],
                                 [enable kernel module run-time safety checks.
                                 @<:default=no@:>@]),
                  [enable_k_safe=$enableval],
                  [enable_k_safe=''])
])# _SS7_OPTIONS
# =========================================================================

# =========================================================================
# _SS7_SETUP_PUBLIC
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_SETUP_PUBLIC,
[
    AC_CACHE_CHECK([for public release], [ss7_cv_public], [dnl
        if test :"$enable_public" = :no ; then
            ss7_cv_public=no
        else
            ss7_cv_public=yes
        fi
    ])
    AM_CONDITIONAL(SS7_PUBLIC_RELEASE, test :"$ss7_cv_public" = :yes)
])# _SS7_SETUP_PUBLIC
# =========================================================================

# =========================================================================
# _SS7_SETUP_DEBUG
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_SETUP_DEBUG,
[
    AC_CACHE_CHECK([for debugging], [ss7_cv_debug], [dnl
        if test :"$enable_k_debug" = :yes ; then
            ss7_cv_debug=_DEBUG
        else
            if test :"$enable_k_test" = :yes ; then
                ss7_cv_debug=_TEST
            else
                if test :"$enable_k_safe" != :no ; then
                    ss7_cv_debug=_SAFE
                else
                    ss7_cv_debug=_NONE
                fi
            fi
        fi
    ])
    case "$ss7_cv_debug" in
        _DEBUG)
            AC_DEFINE_UNQUOTED([_DEBUG], [], [Define for kernel symbol
            debugging.  This has the effect of defeating inlines, making
            static declarations global, and activating all debugging macros.])
            ;;
        _TEST)
            AC_DEFINE_UNQUOTED([_TEST], [], [Define for kernel testing.  This
            has the same effect as _DEBUG for now.])
            ;;
        _SAFE)
            AC_DEFINE_UNQUOTED([_SAFE], [], [Define for kernel safety.  This
            has the effect of enabling safety debugging macros.  This is the
            default.])
            ;;
        *)
            AC_DEFINE_UNQUOTED([_NONE], [], [Define for maximum performance
            and minimum size.  This has the effect of disabling all safety
            debugging macros.])
            ;;
    esac
])# _SS7_SETUP_DEBUG
# =========================================================================

# =========================================================================
# _SS7_SETUP
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_SETUP,
[
    AC_LINUX_KERNEL
    _SS7_CHECK_LIS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel and LiS
])# _SS7_SETUP
# =========================================================================

# =========================================================================
# _SS7_CHECK_LIS
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_CHECK_LIS,
[
##    _SS7_CHECK_LIS_HEADERS
    _SS7_LIS_DEFINES
])# _SS7_CHECK_LIS
# =========================================================================

# =========================================================================
# _SS7_CHECK_LIS_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_CHECK_LIS_HEADERS,
[
    AC_CACHE_CHECK([for LiS header files], [ss7_cv_lis_includes], [dnl
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
		${ss7_cv_module_prefix}/${includedir}/LiS
		${ss7_cv_module_prefix}/${oldincludedir}/LiS
		${ss7_cv_module_prefix}/usr/include/LiS
		${ss7_cv_module_prefix}/usr/local/include/LiS
		${ss7_cv_module_prefix}/usr/src/LiS/include
		${includedir}/LiS
		${oldincludedir}/LiS
		/usr/src/LiS/include
                \""
	fi
	lis_search_path=`echo "$lis_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	ss7_cv_lis_includes=
	for ss7_dir in $lis_search_path
	do
	    if test -d $ss7_dir -a -r $ss7_dir/stropts.h
	    then
		ss7_cv_lis_includes="$ss7_dir"
		break
	    fi
	done
    ])
    if test :"${ss7_cv_lis_includes:-no}" = :no
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
])# _SS7_CHECK_LIS_HEADERS
# =========================================================================

# =========================================================================
# _SS7_CHECK_XTI_HEADERS
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_CHECK_XTI_HEADERS,
[
    AC_CACHE_CHECK([for XTI header files], [ss7_cv_xti_includes], [dnl
	if test :"${cross_compiling:-no}" = :no -a :"${with_k_release:-no}" = :no
	then
	    # compiling for the running kernel
	    eval "xti_search_path=\"
		`echo \"${with_lis}\" | sed -e 's|LiS|xti|'`
		${includedir}/xti
		${oldincludedir}/xti
		/usr/include/xti
		/usr/local/include/xti
		/usr/src/LiS/include/xti
                \""
	else
	    # building for another environment
	    eval "xti_search_path=\"
		`echo \"${with_lis}\" | sed -e 's|LiS|xti|'`
		${ss7_cv_module_prefix}/${includedir}/xti
		${ss7_cv_module_prefix}/${oldincludedir}/xti
		${ss7_cv_module_prefix}/usr/include/xti
		${ss7_cv_module_prefix}/usr/local/include/xti
		${ss7_cv_module_prefix}/usr/src/LiS/include/xti
		${includedir}/xti
		${oldincludedir}/xti
		/usr/src/LiS/include/xti
                \""
	fi
	xti_search_path=`echo "$xti_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
	ss7_cv_xti_includes=
	for ss7_dir in $xti_search_path
	do
	    if test -d $ss7_dir -a -r $ss7_dir/xti_inet.h
	    then
		ss7_cv_xti_includes="$ss7_dir"
		break
	    fi
	done
    ])
    if test :"${ss7_cv_xti_includes:-no}" = :no
    then
	AC_MSG_ERROR([
*** 
*** Could not find XTI include directories.  This package requires the
*** presence of XTI include directories to compile.  Specify the location of
*** XTI include directories with option --with-lis to configure and try again.
*** 
	])
    else
        : # this should be an XOPEN flag
# 	AC_DEFINE([_LIS_SOURCE], [1], [Define when compiling for LiS.])
    fi
])# _SS7_CHECK_XTI_HEADERS
# =========================================================================

# =========================================================================
# _SS7_LIS_DEFINES
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_LIS_DEFINES,
[
    AC_REQUIRE([AC_CANONICAL_HOST])
    case "$ss7_cv_march" in
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
])# _SS7_LIS_DEFINES
# =========================================================================

# =========================================================================
# _SS7_
# -------------------------------------------------------------------------
AC_DEFUN(_SS7_,
[
])# _SS7_
# =========================================================================

dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
