# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 1.1.6.1 $) $Date: 2005/03/09 23:13:44 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2005/03/09 23:13:44 $ by $Author: brian $
#
# =============================================================================

m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])
m4_include([m4/strconf.m4])

# =============================================================================
# AC_LIS
# -----------------------------------------------------------------------------
AC_DEFUN([AC_LIS], [dnl
    _OPENSS7_PACKAGE([LiS], [Linux STREAMS (LiS)])
    _LIS_OPTIONS
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _LIS_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-DLIS=1'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/$(CONFIG_HEADER)'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/$(STRCONF_CONFIG)'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/include -I$(top_srcdir)/include'
    if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
    fi
    AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
    AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
    AC_MSG_NOTICE([final user    LDFLAGS   = $USER_LDFLAGS])
    AC_MSG_NOTICE([final package INCLUDES  = $PKG_INCLUDES])
    AC_MSG_NOTICE([final package MODFLAGS  = $PKG_MODFLAGS])
    AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
    AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
    AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
    AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
    AC_MSG_NOTICE([final kernel  LDFLAGS   = $KERNEL_LDFLAGS])
dnl AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([PKG_INCLUDES])dnl
    AC_SUBST([PKG_MODFLAGS])dnl
    CPPFLAGS=
    CFLAGS=
    _LIS_OUTPUT
    _AUTOTEST
])#AC_LIS
# =============================================================================

# =============================================================================
# _LIS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OPTIONS], [dnl
])# _LIS_OPTIONS
# =============================================================================

# =============================================================================
# _LIS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SETUP], [dnl
    AC_ARG_ENABLE([user-mode],
	AS_HELP_STRING([--enable-user-mode],
	    [enable user mode build.  @<:@default=no@:>@]),
	[enable_user_mode="$enableval"],
	[enable_user_mode='no'])
    AC_CACHE_CHECK([for lis target], [lis_cv_target], [dnl
	lis_cv_target=user
	case "${enable_user_mode:-no}:${host_os}" in
	    no:linux*)	lis_cv_target=linux ;;
	    no:qnx*)	lis_cv_target=qnx ;;
	    *)		lis_cv_target=user ;;
	esac ])
    case "$lis_cv_target" in
	user)
	    LIS_TARGET=u
	    _LIS_USER_SETUP
	;;
	linux)
	    LIS_TARGET=l
	    _LIS_LINUX_SETUP
	;;
	qnx)
	    LIS_TARGET=q
	    _LIS_QNX_SETUP
	;;
	*)
	    LIS_TARGET=p
	    AC_MSG_ERROR([*** "$lis_cv_target" is an unsupported operating system.])
	;;
    esac
    LIS_SUBDIRS=$lis_cv_target
    AM_CONDITIONAL(LIS_USER_TARGET, test :"$LIS_TARGET" = :u)dnl
    AM_CONDITIONAL(LIS_LINUX_TARGET, test :"$LIS_TARGET" = :l)dnl
    AM_CONDITIONAL(LIS_QNX_TARGET, test :"$LIS_TARGET" = :q)dnl
    AC_SUBST([LIS_SUBDIRS])dnl
    AC_SUBST([POLLTST])dnl
])# _LIS_SETUP
# =============================================================================

# =============================================================================
# _LIS_NO_LINUX
# -----------------------------------------------------------------------------
# Null out all the definitions that we don't need when not building for linux
AC_DEFUN([_LIS_NO_LINUX], [dnl
    LIS_KSRC=
    LIS_KVER=
    LIS_KVER_MAJOR=
    LIS_KVER_MINOR=
    LIS_KVER_PATCH=
    LIS_KVER_PATCHNO=
    LIS_USE_RUNNING_KERNEL=
    LIS_CONFIG_MK_KERNEL=
    LIS_KERN_TARGET=
    LIS_STRMS_QUEUES=
    LIS_MOD_INST_DIR=
    LIS_MOD_INSTALL=
    LIS_KSMP=
    LIS_KMODULES=
    LIS_CONFIG_STREAMS=
    LIS_CONFIG_DEV=
    LIS_USE_KMEM_CACHE=
    LIS_USE_LINUX_KMEM_TIMER=
    STRCONF_MAJBASE=230
    STRCONF_CONFIG='include/sys/LiS/config.h'
    STRCONF_MODCONF='head/modconf.inc'
    STRCONF_DRVCONF='drvrconf.mk'
    STRCONF_CONFMOD='conf.modules'
    STRCONF_MAKEDEV='devices.lst'
    STRCONF_MKNODES='libc/user/strmakenodes.c' dnl
])# AC_LIS_NO_LINKX
# =============================================================================

# =============================================================================
# _LIS_USER_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_USER_SETUP], [dnl
    AC_MSG_WARN([*** User Mode is not (well) supported at this time: good luck! ***])
    _LIS_NO_LINUX
    POLLTST=polltst
    AC_DEFINE([_SVID_SOURCE], [1], [Define when building as user mode.])
    AC_DEFINE([_BSD_SOURCE], [1], [Define when building as user mode.])
    AC_DEFINE([_XOPEN_SOURCE], [500], [Define when building as user mode.  I added
				   this one.  Actually, it should always be
				   defined.  We really need to do an AH_VERBATIM
				   here to check if it is already defined.])
#
#   Below is effectively what gcom make files do but this is the wrong way to
#   do this.  We do a platform independent autoconf check so that we can build
#   the user target for systems with old SysV definitions.
#
#   AC_REQUIRE([AC_CANONICAL_TARGET])dnl
#   case "$host_os" in
#       solaris* | sun5*)
#           # this is the wrong way to do this, go looking for them instead
#           AC_DEFINE([LIS_HAVE_MAJOR_T], [1], [Define when build as user mode for SunOS5.])
#           AC_DEFINE([LIS_HAVE_O_UID_T], [1], [Define when build as user mode for SunOS5.])
#           ;;
#   esac
#
    AC_CHECK_TYPE(o_uid_t, lis_cv_type_o_uid_t=yes, lis_cv_type_o_uid_t=no)
    if test "$lis_cv_type_o_uid_t" = yes; then
	AC_DEFINE([LIS_HAVE_O_UID_T], [1], [Define this macro when your
		   <sys/types.h> has o_uid_t and o_gid_t already defined and
		   you are building for a user target to keep LiS source files
		   from redefining o_uid_t and o_gid_t.])
    fi
    AC_CHECK_TYPE(major_t, lis_cv_type_major_t=yes, lis_cv_type_major_t=no)
    if test "$lis_cv_type_major_t" = yes; then
	AC_DEFINE([LIS_HAVE_MAJOR_T], [1], [Define this macro when your
		   <sys/types.h> has major_t already defined and you are
		   building for a user target to keen LiS source files from
		   redefining major_t.])
    fi
# 
#   For user setup, as the gcom makefiles do, we always use the Config.user
#   file as the sole configuration file.
# 
    AC_DEFINE([USER], [1], [Define if compiling for USER.]) dnl
])# _LIS_USER_SETUP
# =============================================================================

# =============================================================================
# _LIS_QNX_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_QNX_SETUP], [dnl
    AC_MSG_WARN([*** QNX not supported at this time: good luck! ***])
    _LIS_NO_LINUX
    CPPFLAGS="-DQNX${CPPFLAGS:+ }${CPPFLAGS}"
    AC_DEFINE([QNX], [1], [Define if compiling for QNX.]) dnl
])# _LIS_QNX_SETUP
# =============================================================================

# =============================================================================
# _LIS_SCO_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SCO_SETUP], [dnl
    AC_MSG_WARN([*** SCO not supported at this time: good luck! ***])
    _LIS_NO_LINUX
    CPPFLAGS="-DSCO${CPPFLAGS:+ }${CPPFLAGS}"
    AC_DEFINE([SCO], [1], [Define if compiling for SCO.]) dnl
])# _LIS_SCO_SETUP
# =============================================================================

# =============================================================================
# _LIS_DOS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_DOS_SETUP], [dnl
    AC_MSG_WARN([*** DOS not supported at this time: good luck! ***])
    _LIS_NO_LINUX
    CPPFLAGS="-DDOS${CPPFLAGS:+ }${CPPFLAGS}"
    AC_DEFINE([DOS], [1], [Define if compiling for DOS.]) dnl
])# _LIS_DOS_SETUP
# =============================================================================

# =============================================================================
# _LIS_LINUX_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_LINUX_SETUP], [dnl
    AC_MSG_NOTICE([-----------------------------------])
    AC_MSG_NOTICE([starting linux kernel configuration])
    AC_MSG_NOTICE([-----------------------------------])
    _LINUX_KERNEL
    _GENKSYMS
    _LIS_HOME
    _LIS_CONFIG_SYSCALLS
    _LIS_RH_71_KLUDGE
    _LIS_GET_EMPTY_INODE
    _LIS_SIGMASKLOCK
    _LIS_RCVOID
    AC_MSG_NOTICE([-----------------------------------])
    AC_MSG_NOTICE([complete linux kernel configuration])
    AC_MSG_NOTICE([-----------------------------------])
    STRCONF_MAJBASE=230
    STRCONF_CONFIG='include/sys/LiS/config.h'
    STRCONF_MODCONF='head/modconf.inc'
    STRCONF_DRVCONF='drvrconf.mk'
    STRCONF_CONFMOD='conf.modules'
    STRCONF_MAKEDEV='devices.lst'
    STRCONF_MKNODES='util/linux/strmakenodes.c'
    AC_DEFINE([LINUX], [1], [Define if compiling for LINUX.]) dnl
])# _LIS_LINUX_SETUP
# =============================================================================

# =============================================================================
# _LIS_SETUP_LIS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SETUP_LIS], [dnl
	if test -z "$with_k_release" ; then
	    LIS_USE_RUNNING_KERNEL=y
	else
	    LIS_USE_RUNNING_KERNEL=n
	fi
	LIS_KVER="$linux_cv_k_release"
	LIS_NKVER="${linux_cv_k_major}.${linux_cv_k_minor}.${linux_cv_k_patch}"
	LIS_KVER_MAJOR="$linux_cv_k_major"
	LIS_KVER_MINOR="$linux_cv_k_minor"
	LIS_KVER_PATCH="$linux_cv_k_patch"
	LIS_KVER_PATCHNO="$linux_cv_k_extra"
	LIS_KVER_MAJORMINOR="${linux_cv_k_major}.${linux_cv_k_minor}"
	LIS_KSRC="${linux_cv_k_build}"
	LIS_NOKSRC=0
	_LIS_ARCH_DEFINES
dnl     _LIS_KVER_H
dnl     _LIS_NKVER_H
dnl     _LIS_KSMP
dnl     _LIS_KMODULES
dnl     _LIS_KMODVERS
dnl     _LIS_LISMODVERS
dnl     _LIS_LISAUTOCONF
dnl     _LIS_IPV6
dnl     _LIS_IPV6_MODULE
	_LIS_CONFIG_MK_KERNEL
	_LIS_STRMS_QUEUES
	_LIS_MOD_INST_DIR
	_LIS_MOD_INSTALL dnl
])# _LIS_SETUP_LIS
# =============================================================================

# =============================================================================
# _LIS_ARCH_DEFINES
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_ARCH_DEFINES], [dnl
    AC_REQUIRE([AC_CANONICAL_HOST])dnl
    case "$linux_cv_march" in
	alpha*)			: ;;
	arm*)			: ;;
	cris*)			: ;;
	i?86* | k6* | athlon*)	: ;;
	ia64)			: ;;
	m68*)			: ;;
	mips64*)		: ;;
	mips*)			: ;;
	hppa*)
	    AC_DEFINE([_HPPA_LIS_], [1], [Define when compiling for HPPA.
		This define is only used for linux kernel target.  This is
		really the wrong way to go about doing this: the function
		should be checked for by autoconf instead of placing the
		architectural dependencies in the LiS source.  The define is
		used in "head/linux-mdep.c" to determine whether
		lis_pci_cleanup exists; "head/linux/exports.c" to determine
		whether a bunch of functions are available; "head/osif.c" to
		determine whether a bunch of PCI DMA mapping functions are
		available.])
	    ;;
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
    case "$linux_cv_march" in
	ppc* | powerpc*)	
#           AC_DEFINE([__powerpc__], [1], [Define when compiling for PPC.
#               This define is only used for the linux target.  This is an
#               error, this value is defined by the GNU compiler when it
#               compiles.  It is not used by LiS source code.])
	    :
	    ;;
	s390*)			
#           AC_DEFINE([__BOOT_KERNEL_SMP], [1], [Define when compiling for
#               S390 or S390X.  This is an error.  __BOOT_KERNEL_SMP should
#               only be defined in kernel.h.  This is not used by the LiS
#               source and will be handled by kernel source checks.])
	    :
	    ;;
    esac dnl
])# _LIS_ARCH_DEFINES
# =============================================================================

# =============================================================================
# _LIS_GET_SYSTEM_MAP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_SYSTEM_MAP], [dnl
])#_LIS_GET_SYSTEM_MAP
# =============================================================================

# =============================================================================
# _LIS_CHECK_KVER
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CHECK_KVER], [dnl
])#_LIS_CHECK_KVER
# =============================================================================

# =============================================================================
# _LIS_GET_VERS_H
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_VERS_H], [dnl
])#_LIS_GET_VERS_H
# =============================================================================

# =============================================================================
# _LIS_GET_KVER
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_KVER], [dnl
])#_LIS_GET_KVER
# =============================================================================

# =============================================================================
# _LIS_VERIFY_KVER
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_VERIFY_KVER], [dnl
])#_LIS_VERIFY_KVER
# =============================================================================

# =============================================================================
# _LIS_GET_SMP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_SMP], [dnl
])#_LIS_GET_SMP
# =============================================================================

# =============================================================================
# _LIS_GET_MODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_MODULES], [dnl
])#_LIS_GET_MODULES
# =============================================================================

# =============================================================================
# _LIS_GET_IPV6
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_IPV6], [dnl
])#_LIS_GET_IPV6
# =============================================================================

# =============================================================================
# _LIS_GENERATE_SYMBOL_NAMES
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GENERATE_SYMBOL_NAMES], [dnl
])#_LIS_GENERATE_SYMBOL_NAMES
# =============================================================================

# =============================================================================
# _LIS_C_COMPILER_VERSION
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_C_COMPILER_VERSION], [dnl
])#_LIS_C_COMPILER_VERSION
# =============================================================================

# =============================================================================
# _LIS_OUTPUT_CONFIGVAR
# -----------------------------------------------------------------------------
# _LIS_OUTPUT_CONFIGVAR(LISCONFVARNAME) output a variable to the LiS
# configuration file config.in
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OUTPUT_CONFIGVAR], [dnl
])#_LIS_OUTPUT_CONFIGVAR
# =============================================================================

# =============================================================================
# _LIS_HOME
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_HOME], [dnl
    LIS_HOME="$srcdir" dnl
])# _LIS_HOME
# =============================================================================

# =============================================================================
_LIS_CONFIG_SYSCALLS
# -----------------------------------------------------------------------------
# symbols to rip for LiS support (without system call generation)
# -----------------------------------------------------------------------------
# sys_unlink            <-- extern, not declared
# sys_mknod             <-- extern, not declared
# sys_umount            <-- extern, not declared
# sys_mount             <-- extern, not declared
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CONFIG_SYSCALLS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_unlink], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_unlink() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_mknod], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_mknod() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_umount], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_mount() will always return ENOSYS])
	fi ])
    _LINUX_KERNEL_SYMBOL_EXPORT([sys_mount], [dnl
	if test :"${linux_cv_k_marchdir}" = :parisc ; then
	    AC_MSG_WARN([lis_umount() will always return ENOSYS])
	fi ]) dnl
])# _LIS_CONFIG_SYSCALLS
# =============================================================================

# =============================================================================
# _LIS_KSRC
# -----------------------------------------------------------------------------
# Determines the kernel source for a linux build
AC_DEFUN([_LIS_KSRC], [dnl
])# _LIS_KSRC
# =============================================================================

# =============================================================================
# _LIS_KVER
# -----------------------------------------------------------------------------
# Determines the kernel version for a linux build
AC_DEFUN([_LIS_KVER], [dnl
    if test :"${with_k_release:-no}" = :no
    then
	if test :"${enable_user_mode:-no}" != :yes
	then
	    AC_MSG_WARN([*** could not determine kernel version ***])
	    LIS_KVER=''
	    LIS_NKVER=''
	else
	    LIS_KVER=''
	    LIS_NKVER=''
	fi
    else
	LIS_KVER="${with_k_release}"
	LIS_NKVER=`echo -n ${LIS_KVER} | sed -e 's|-.*||'`
    fi dnl
])# _LIS_KVER
# =============================================================================

# =============================================================================
# _LIS_KVER_MAJOR
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KVER_MAJOR], [dnl
    AC_MSG_CHECKING([for lis kernel major version number])
    changequote(, )
    LIS_KVER_MAJOR=`echo -n ${LIS_KVER} | sed -e 's|[.].*||'`
    changequote([, ])
    if test :"$LIS_KVER_MAJOR" != :2; then
	AC_MSG_ERROR([*** unsupported kernel major number $LIS_KVER_MAJOR])
    fi
    AC_MSG_RESULT([$LIS_KVER_MAJOR]) dnl
])# _LIS_KVER_MAJOR
# =============================================================================

# =============================================================================
# _LIS_KVER_MINOR
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KVER_MINOR], [dnl
    AC_MSG_CHECKING([for lis kernel minor version number])
    changequote(, )
    LIS_KVER_MINOR=`echo -n ${LIS_KVER} | sed -e 's|^[^.]*[.]||' -e 's|\..*||'`
    changequote([, ])
    if test "${LIS_KVER_MINOR-0}" -lt 3 -o "${LIS_KVER_MINOR-0}" -gt 4; then
	AC_MSG_ERROR([*** unsupported kernel minor number $LIS_KVER_MINOR])
    fi
    if test "${LIS_KVER_MINOR-0}" -ne 4; then
	AC_MSG_WARN([*** good luck compiling for kernel minor number $LIS_KVER_MINOR ***])
    fi
    AC_MSG_RESULT([$LIS_KVER_MINOR]) dnl
])# _LIS_KVER_MINOR
# =============================================================================

# =============================================================================
# _LIS_KVER_PATCH
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KVER_PATCH], [dnl
    AC_MSG_CHECKING([for lis kernel patch level])
    changequote(, )
    LIS_KVER_PATCH=`echo -n ${LIS_KVER} | sed -e 's|^[^.]*[.][^.]*[.]||'`
    changequote([, ])
    AC_MSG_RESULT([$LIS_KVER_PATCH]) dnl
])# _LIS_KVER_PATCH
# =============================================================================

# =============================================================================
# _LIS_KVER_PATCHNO
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KVER_PATCHNO], [dnl
    AC_MSG_CHECKING([for lis kernel patch level number])
    changequote(, )
    LIS_KVER_PATCHNO=`echo -n ${LIS_KVER} | sed -e 's|^[^.]*[.][^.]*[.]||' -e 's|[-].*||'`
    changequote([, ])
    AC_MSG_RESULT([$LIS_KVER_PATCHNO]) dnl
])# _LIS_KVER_PATCHNO
# =============================================================================

# =============================================================================
# _LIS_KVER_MAJORMINOR
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KVER_MAJORMINOR], [dnl
    LIS_KVER_MAJORMINOR="${LIS_KVER_MAJOR}.${LIS_KVER_MINOR}" dnl
dnl AC_MSG_WARN([*** major/minor kernel release is ${LIS_KVER_MAJORMINOR} ***])
])# _LIS_KVER_MAJORMINOR
# =============================================================================


# =============================================================================
# _LIS_KVER_H
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KVER_H], [dnl
    LIS_KVER_H='' dnl
])# _LIS_KVER_H
# =============================================================================

# =============================================================================
# _LIS_NKVER_H
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_NKVER_H], [dnl
    LIS_NKVER_H='' dnl
])# _LIS_NKVER_H
# =============================================================================

# =============================================================================
# _LIS_KSMP
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KSMP], [dnl
    AC_ARG_ENABLE(debug, [  --enable-smp            force compile for SMP])
    if test :"${lis_smp:-no}" = :no
    then
	LIB_KSMP=''
    else
	LIB_KSMP='y'
	AC_DEFINE([__SMP__], [1], [LiS source code wants this defined when it is
			       configure for SMP.])
    fi dnl
])# _LIS_KSMP
# =============================================================================

# =============================================================================
# _LIS_KMODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KMODULES], [dnl
    LIS_KMODULES='y' dnl
])# _LIS_KMODULES
# =============================================================================

# =============================================================================
# _LIS_KMODVERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KMODVERS], [dnl
    LIS_KMODVERS='' dnl
])# _LIS_KMODVERS
# =============================================================================

# =============================================================================
# _LIS_LISMODVERS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_LISMODVERS], [dnl
    LIS_LISMODVERS=''
    if test :"${lis_never_set_this_variable:+never}" != :never
	AC_DEFINE([LISMODVERS], [1], [Define if a modversions.h file was generated.])
    fi dnl
])# _LIS_LISMODVERS
# =============================================================================

# =============================================================================
# _LIS_LISAUTOCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_LISAUTOCONF], [dnl
    LIS_LISAUTOCONF=n
# 
#   this is just to generate the template
# 
    if test :"${lis_never_set_this_variable:+never}" != :never
    then
	AC_DEFINE([LISAUTOCONF], [1], [Define if an autoconf.h file was
		   generated.  LISAUTOCONF is used by
		   "include/sys/LiS/linux-mdep.h" to know when to include
		   "LiS/sys/autoconf.h" and to prevent linux from including its
		   own.  In autoconf we do this by setting the appropriate
		   include directory order in the makefile.  Our local
		   "include/linux/autoconf.h" will be included in preference to
		   the kernel one.  I don't know about LiS's practice of mucking
		   around with include file wrapper macros.  A renaming of some
		   macros would kil portability.  We never define this macro
		   even when we build our own autoconf but we do mark it in
		   gcom's old config.in file when we build it.])
    fi dnl
])# _LIS_LISAUTOCONF
# =============================================================================

# =============================================================================
# _LIS_IPV6
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_IPV6], [dnl
    LIS_IPV6='' dnl
])# _LIS_IPV6
# =============================================================================

# =============================================================================
# _LIS_IPV6_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_IPV6_MODULE], [dnl
    LIS_IPV6_MODULE='' dnl
])# _LIS_IPV6_MODULE
# =============================================================================

# =============================================================================
# _LIS_CONFIG_MK_KERNEL
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_CONFIG_MK_KERNEL], [dnl
    LIS_CONFIG_MK_KERNEL='n' dnl
])# _LIS_CONFIG_MK_KERNEL
# =============================================================================

# =============================================================================
# _LIS_STRMS_QUEUES
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_STRMS_QUEUES], [dnl
    LIS_STRMS_QUEUES='t'
    AC_DEFINE([USE_KTHREAD], [1], [Define (always) for kernel thread queues.]) dnl
])# _LIS_STRMS_QUEUES
# =============================================================================

# =============================================================================
# _LIS_MOD_INST_DIR
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_MOD_INST_DIR], [dnl
    LIS_MOD_INST_DIR="$linux_cv_k_modules" dnl
])# _LIS_MOD_INST_DIR
# =============================================================================

# =============================================================================
# _LIS_MOD_INSTALL
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_MOD_INSTALL], [dnl
    if test :"${enable_k_install:-yes}" = :yes
    then
	LIS_MOD_INSTALL='y'
    else
	LIS_MOD_INSTALL='n'
    fi dnl
])# _LIS_MOD_INSTALL
# =============================================================================

# =============================================================================
# _LIS_RH_71_KLUDGE
# -----------------------------------------------------------------------------
# this is not really a RH 7.1 kludge but a 2.4.2 kludge
AC_DEFUN([_LIS_RH_71_KLUDGE], [dnl
    AC_MSG_CHECKING([for broken RH 7.1 2.4.2 kernel])
    LIS_RH_71_KLUDGE=''
    if test "${LIS_KVER_MAJOR}:${LIS_KVER_MINOR}:${LIS_KVER_PATCHNO}" = "2:4:2"
    then
	# - should also check for redhat something (in kernel source directory)
	if test -e "${linux_cv_k_includes}/linux/rhconfig.h"
	then
	    LIS_RH_71_KLUDGE='y'
	    AC_DEFINE([RH_71_KLUDGE], [1], [Define if you have redhat kernel 2.4.2])
	    AC_MSG_RESULT([yes])
	else
	    AC_MSG_RESULT([no])
	fi
    else
	AC_MSG_RESULT([no])
    fi dnl
])# _LIS_RH_71_KLUDGE
# =============================================================================

# =============================================================================
# _LIS_SIGMASKLOCK
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SIGMASKLOCK], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_CACHE_CHECK([for sigmask_lock], [lis_cv_sigmask_lock], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<sigmask_lock(_R(smp_)?........)?\>], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
	    ], [lis_cv_sigmask_lock=yes], [lis_cv_sigmask_lock=no]) ]) ])
    if test :"$lis_cv_sigmask_lock" = :yes
    then
	LIS_SIGMASKLOCK='y'
	AC_DEFINE([SIGMASKLOCK], [1], [This is necessary due to RedHat
	    mangling of kernel header files.  RedHat changes some of the use
	    of sigmask_lock.  Autoconf will check your header files.  The code
	    will lock sigmask_lock when required.])
    else
	LIS_SIGMASKLOCK='n'
    fi dnl
])# _LIS_SIGMASKLOCK
# =============================================================================

# =============================================================================
# _LIS_RCVOID
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_RCVOID], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_CACHE_CHECK([for recalc_sigpending_tsk() function], [lis_cv_rctsk], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<recalc_sigpending_tsk(_R(smp_)?........)?\>[(][^()]*[)]], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
	    ], [lis_cv_rctsk=yes], [lis_cv_rctsk=no]) ]) ])
    AC_CACHE_CHECK([for recalc_sigpending(void)], [lis_cv_rcvoid], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<recalc_sigpending(_R(smp_)?........)?\>[(]void[)]], [
#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
	    ], [lis_cv_rcvoid=yes], [lis_cv_rcvoid=no]) ]) ])
    if test :"$lis_cv_rcvoid" = :yes -o :"$lis_cv_rctsk" = :yes
    then
	LIS_RCVOID='y'
	AC_DEFINE([RCVOID], [1], [This is necessary due to RedHat
	    mangling of kernel header files.  RedHat changes
	    recalc_sigpending to take void instead of a task_struct pointer
	    and defines a new recalc_sigpending_tsk to take the other's place.
	    Autoconf will check your header files.  The code will try to use
	    recalc_sigpending() if this is defined, or
	    recalc_sigpending(current) otherwise.])
    else
	LIS_RCVOID='n'
    fi dnl
])# _LIS_RCVOID
# =============================================================================


# =============================================================================
# _LIS_OLD_GCOM_CONFIG_IN
# -----------------------------------------------------------------------------
# These substitutions are only really required to generate the old gcom
# configuration files, in case someone wants to perform the old gcom build.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OLD_GCOM_CONFIG_IN], [dnl
    # 
    # old gcom configuration script uses `uname -m`, but of course that is the
    # build host and not the target host.  This allows proper cross compiling.
    # 
    LIS_MACHINE=$host_cpu
    AC_SUBST([LIS_MACHINE])dnl
    # 
    # old gcom files use the base CC name just as the linux kernel makefiles
    # do.  This does not work for proper cross-compiling.  This is the CC name
    # as provided to the configuration script in the CC environment variable,
    # after we make adjustments to it.  This could be either *-*-*-gcc or just
    # plain cc, even in a cross-compiling situation.  For compatibility with
    # the gcom makefiles we adjust CC_NAME to be just the CC root and strip
    # off or regenerate the CROSS_COMPILE component.
    # 
    AC_REQUIRE([AC_PROG_CC])dnl
    if test :"${cross_compiling:+set}" = :set
    then
	LIS_CC_NAME==`echo $CC | sed -e's|.*-||g'`
	LIS_CC_OPTIMIZE=-O2
	LIS_CROSS_COMPILING=y
	LIS_CROSS_COMPILE="`echo $CC | sed -e's|-[[^-]]*[$]||'`"-
	if test :"${LIS_CROSS_COMPILE:-no}" = :no
	then
	    AC_REQUIRE([AC_CANONICAL_TARGET])dnl
	    LIS_CROSS_COMPILE="$host"-
	fi
	# need to add cross-compile file for old gcom makefiles
	AC_CONFIG_FILES([cross-compile])
    else
	LIS_CC_NAME=$CC
	LIS_CC_OPTIMIZE=-O2
	LIS_CROSS_COMPILING=n
	LIS_CROSS_COMPILE=
    fi
    AC_SUBST([LIS_CROSS_COMPILE])dnl
    #
    # this might not work for other than GCC, but I don't think that the gcom
    # make files really support other than GCC builds anyways.  We do support
    # (kinda) non-gcc builds
    #
    LIS_CC_VERS=`eval $CC --version </dev/null`
    # 
    # gcom makefiles set this to -I$KSRC/arch/i386/mach-generic for some
    # reason, but not for others and it is unclear why this is being done
    # because the directory does not appear to exist.  All machine specific
    # includes are behind the asm symbolic link.  We will double check for a
    # kernel build that the proper asm links are there.
    #
    LIS_MACHINE_INCL= dnl
])# _LIS_OLD_GCOM_CONFIG_IN
# =============================================================================

# =============================================================================
# _LIS_GET_EMPTY_INODE
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_GET_EMPTY_INODE], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_CACHE_CHECK([for new_inode() function], [lis_cv_have_new_inode], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<new_inode(_R(smp_)?........)?\>([^()]*)], [
#include <linux/version.h>
#include <linux/config.h>
#include <linux/types.h>
#include <linux/fs.h>
	    ], [lis_cv_have_new_inode=yes], [lis_cv_have_new_inode=no]) ]) ])
    AC_CACHE_CHECK([for get_empty_inode() function], [lis_cv_have_get_empty_inode], [dnl
	_LINUX_KERNEL_ENV([dnl
	    AC_EGREP_CPP([\<get_empty_inode(_R(smp_)?........)?([^()]*)], [
#include <linux/version.h>
#include <linux/config.h>
#include <linux/types.h>
#include <linux/fs.h>
	    ], [lis_cv_have_get_empty_inode=yes], [lis_cv_have_get_empty_inode=no]) ]) ])
    if test :"$lis_cv_have_new_inode" = :yes
    then
	LIS_GET_EMPTY_INODE='new_inode(LIS_SB)'
    else
	if test :"$lis_cv_have_get_empty_inode" = :yes
	then
	    LIS_GET_EMPTY_INODE='get_empty_inode()'
	else
	    AC_MSG_ERROR([
*** 
*** You have really mangled kernel header files: the headers should include
*** either new_inode() or get_new_inode().  Some kernel versions and distros
*** have one or the other missing.  In particular, RedHat removed
*** get_new_inode() from their patched kernels and made new_inode() non-inline
*** (used to be inline in the header file).  Your kernel headers are so bad
*** that <linux/fs.h> defines neither.  Please check your kernel headers and
*** source and try again.
*** ])
	fi
    fi dnl
dnl
dnl This definition goes in include/sys/LiS/genconf.h
dnl
dnl AC_DEFINE_UNQUOTED([GET_EMPTY_INODE], [$LIS_GET_EMPTY_INODE], [This is
dnl                     necessary due to RedHat mangling of kernel header
dnl                     files. RedHat removes get_empty_inode() from
dnl                     <linux/fs.h> and makes new_inode() non-inline and
dnl                     removes it from the header file (now a function call).
dnl                     Autoconf will check your header files.  The code will
dnl                     try to use whatever is defined here to get an empty
dnl                     inode.  Neither a check of the header files nor a
dnl                     running kernel symbol table will guarantee that those
dnl                     symbols are exported and usable by LiS.  If you get
dnl                     depmod errors on instalation mentioning either of
dnl                     these functions, then this is probably the cause.])
])# _LIS_GET_EMPTY_INODE
# =============================================================================

# =============================================================================
# _LIS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OUTPUT], [dnl
# 
#   The following are common setup requirements that do not depend on the
#   architecutre or build target.
#
    _LIS_INT_PSW
    _LIS_OLD_CONSTS
    _LIS_SOLARIS_STYLE_CMN_ERR
    _LIS_DBG_OPT
    _LIS_KMEM_OPTS
    _LIS_SHLIB
    _LIS_ATOMIC_STATS
# 
#   Acutally generating output files is last.  We don't want to generate a
#   thing until we have performed all the checks and balances.
# 
    _LIS_STRCONF
    _LIS_OUTPUT_CONFIG_IN dnl
    AM_CONDITIONAL(WITH_LFS, false)dnl
    AM_CONDITIONAL(WITH_LIS, false)dnl
])# _LIS_OUTPUT
# =============================================================================

# =============================================================================
# _LIS_INT_PSW
# -----------------------------------------------------------------------------
# Determines from the --enable-broken-cpu-flags option whether to use proper
# unsigned long for cpu flags or whether to store then as an int.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_INT_PSW],[dnl
    AC_ARG_ENABLE([broken-cpu-flags],
	AS_HELP_STRING([--enable-broken-cpu-flags],
	    [use broken integer cpu flags for locks, otherwise use unsigned
	    long cpu flags.  @<:@default=no@:>@]),
	[enable_broken_cpu_flags="$enableval"],
	[enable_broken_cpu_flags='no'])
    AC_MSG_CHECKING([for lis broken cpu flags])
    if test :"${enable_broken_cpu_flags:-no}" = :no
    then
	LIS_INT_PSW='n'
	INT_PSW=0
	AC_MSG_RESULT([no])
    else
	LIS_INT_PSW='y'
	INT_PSW=1
#
#       This definition goes in include/sys/LiS/genconf.h
#
#       AC_DEFINE([INT_PSW], [1], [Define if you want interrupt flags stored as
#                  int instead of unsigned long.  The kernel always uses
#                  unsigned long.  It is generally a bad idea to use int when
#                  the kernel uses unsigned long.  If in doubt, leave this
#                  undefined.])
	AC_MSG_RESULT([yes])
    fi
    AC_SUBST([INT_PSW])dnl
])# _LIS_INT_PSW
# =============================================================================

# =============================================================================
# _LIS_OLD_CONSTS
# -----------------------------------------------------------------------------
# Determine from the --with-solaris-consts option whether to use
# solaris/unixware compatible consts or backwards compatible LiS consts (which
# have no bearing in reality).
# -----------------------------------------------------------------------------
# I could eventually expand this to set more precisely the compatibility of
# constants within the STREAMS environment.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OLD_CONSTS], [dnl
    AC_ARG_WITH([solaris-consts],
	AS_HELP_STRING([--with-solaris-consts],
	    [use Unixware/Solaris compatible constants if yes, or use LiS
	    backward compatible constants if no.  @<:@default=yes@:>@]),
	[with_solaris_consts="$withval"],
	[with_solaris_consts='yes'])
    AC_MSG_CHECKING([for lis Unixware/Solaris compatible constants])
    if test :"${with_solaris_consts:-yes}" = :yes
    then
	LIS_OLD_CONSTS=n
	AC_MSG_RESULT([no])
    else
	LIS_OLD_CONSTS=y
	AC_DEFINE([USE_OLD_CONSTS], [1], [Define to use LiS backware compatible
		   constants (which have no bearing in reality).  Leave this
		   undefined to get Solaris/Unixware compatible constants.])
	AC_MSG_RESULT([yes])
    fi dnl
])# _LIS_OLD_CONSTS
# =============================================================================

# =============================================================================
# _LIS_SOLARIS_STYLE_CMN_ERR
# -----------------------------------------------------------------------------
# This is another target independent one again.  The default is wrong for
# Linux.  Linux does not issue newlines at the beginning of printks.  Setting
# this to the gcom makefile default, messes up your log files.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SOLARIS_STYLE_CMN_ERR], [dnl
    AC_ARG_WITH([solaris-cmn_err],
	AS_HELP_STRING([--with-solaris-cmn_err],
	    [use a Solaris-style cmn_err that puts a newline at the end of the
	    line rather than the beginning @<:@default=yes@:>@]),
	[with_solaris_cmn_err="$withval"],
	[with_solaris_cmn_err='yes'])
    AC_MSG_CHECKING([for lis Solaris style cmn_err])
    if test :"${with_solaris_cmn_err:-yes}" = :yes
    then
	LIS_SOLARIS_STYLE_CMN_ERR=y
	AC_DEFINE([SOLARIS_STYLE_CMN_ERR], [1], [Define for solaris style
		   cmn_err().  This should always be defined for Linux,
		   otherwise, the newlines generated at the beginning of
		   cmn_err() will mess up the appearance of your logs.])
	AC_MSG_RESULT([yes])
    else
	LIS_SOLARIS_STYLE_CMN_ERR=n
	if test :"$lis_cv_target" = :linux
	then
	    AC_MSG_WARN([ *** 
*** You have asked for an SVR4-style cmn_err() on a Linux build using the
*** option --without-solaris-cmn-err.  This is probably not what you want.
*** Consider removing the option, otherwise, your system logs will have a
*** messy appearance due to the extra newline at the beginning of the
*** cmn_err() listing.
*** ])
	fi
	AC_MSG_RESULT([no])
    fi dnl
])# _LIS_SOLARIS_STYLE_CMN_ERR
# =============================================================================

# =============================================================================
# _LIS_DBG_OPT
# -----------------------------------------------------------------------------
# Setup defines and automake variables depending on the setting of the option
# --enable-k-debug.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_DBG_OPT], [dnl
    if test :"${enable_k_safe:-no}" != :no
    then
	AC_DEFINE([LIS_CONFIG_SAFE], [1], [Setting this macro is pointless
		  (now).  LiS uses it to set another useless macro called SAFE
		  which is never checked anywhere in the LiS code.  Perhaps
		  Matt will use this macro (again), so I left it in here.])

	AC_DEFINE([CONFIG_DEV], [1], [Does what the one above used to.  The
		  name of this macro was a spectacularly poor choice given
		  that we are compiling kernel modules.])
	LIS_CONFIG_DEV=y
    else
	LIS_CONFIG_DEV=n
    fi
    if test :"${enable_k_test:-no}" != :no
    then
	AC_DEFINE([LIS_TESTING], [1], [Setting this macro is pointless (now).
		   LiS uses it to set another useless macro called TEST which
		   is never checked anywhere in the LiS code.  Perhaps Matt
		   will use this macro (again), so I left it in here.])
    fi
    if test :"${enable_k_debug:-no}" != :no
    then
	LIS_DBG_OPT=y
	lis_inline=
	lis_static=
	AC_DEFINE([LIS_DEBUG], [1], [Setting this macro is pointless (now).
		   LiS uses it to set another useless macro called TEST which
		   is never checked anywhere in the LiS code.  Perhaps Matt
		   will use this macro (again), so I left it in here.])
	#
	# FIXME: This should actually be the install directory for the source
	# code rather than the build directory that contains the source code.
	#
	AC_DEFINE_UNQUOTED([LIS_SRC], "`(cd $srcdir; pwd)`", [LiS source code
			    wants this defined when it is configured for
			    debug.])
	AC_DEFINE([DEBUG], [1], [Define this macro for debugging source code.])
    else
	LIS_DBG_OPT=n
	AC_REQUIRE([AC_C_INLINE])dnl
	lis_inline=$ac_cv_c_inline
	lis_static="static"
    fi
    AC_DEFINE_UNQUOTED([INLINE], [$lis_inline], [define to inline directive])
    AC_DEFINE_UNQUOTED([STATIC], [$lis_static], [define to static directive]) dnl
])# _LIS_DBG_OPT
# =============================================================================

# =============================================================================
# _LIS_KMEM_OPTS
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_KMEM_OPTS], [dnl
    AC_ARG_ENABLE([k-timers],
	AS_HELP_STRING([--enable-k-timers],
	    [enable kernel memory caches for timers for speed.
	    @<:@default=no@:>@]),
	[enable_k_timers="$enableval"],
	[enable_k_timers='no'])
    AC_ARG_ENABLE([k-cache],
	AS_HELP_STRING([--enable-k-cache],
	    [enable kernel memory caches for speed.  @<:@default=no@:>@]),
	[enable_k_cache="$enableval"],
	[enable_k_cache='no'])
    if test :"$enable_k_timers" = :yes ; then
	$enable_k_cache='yes'
    fi
    AC_MSG_CHECKING([for lis kernel caches])
    if test :"$enable_k_cache" = :yes ; then
	AC_DEFINE_UNQUOTED([USE_LINUX_KMEM_CACHE], [1], [Define this macro
	    to use Linux kernel memory caches for queue structures and
	    other highly used structures for some performance improvement.
	    For the best improvement, use Linux Fast-STREAMS.])
	LIS_USE_KMEM_CACHE=y
    else
	LIS_USE_KMEM_CACHE=n
    fi
    AC_MSG_RESULT([$enable_k_cache])
    AC_MSG_CHECKING([for lis timer caches])
    if test :"$enable_k_timers" = :yes ; then
	AC_DEFINE_UNQUOTED([USE_LINUX_KMEM_TIMER], [1], [Define this macro
	    to use the kernel memory cacnes for timer structures rather
	    than sequential list searches.  This can really break,
	    especially on SMP and is not safe at all.  It is disabled by
	    default.])
	LIS_USE_LINUX_KMEM_TIMER=y
    else
	LIS_USE_LINUX_KMEM_TIMER=n
    fi
    AC_MSG_RESULT([$enable_k_timers])
])# _LIS_KMEM_OPTS
# =============================================================================

# =============================================================================
# _LIS_SHLIB
# -----------------------------------------------------------------------------
# Because we are a libtooliszed build, shared libraries are controlled with
# the libtoool options flags.  For the purpose of the gcom makefiles, we just
# set shared libraries to yes.  If you want automatic shared library support
# use this autoconf build process instead.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_SHLIB], [dnl
    LIS_SHLIB=y dnl
])# _LIS_SHLIB
# =============================================================================

# =============================================================================
# _LIS_ATOMIC_STATS
# -----------------------------------------------------------------------------
# Determine from --enable-atomic-stats option whether to use atomic_t for
# stats or to use the normal int state.
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_ATOMIC_STATS], [dnl
    AC_ARG_ENABLE([atomic-stats],
	AC_HELP_STRING([--enable-atomic-stats],
	    [use atomic_t for module_stat structure elements.
	    @<:@default=no@:>@]),
    [enable_atomic_stats="$enableval"],
    [enable_atomic_states='no'])
    AC_MSG_CHECKING([for lis atomic stats])
    if test :"${enable_atomic_stats:-no}" = :yes
    then
#
#       This definition goes in include/sys/LiS/genconf.h
#
#       AC_DEFINE([LIS_ATOMIC_STATS], [1], [Define if you want atomic_t for
#       statistics elements.  Otherwise the LiS uses int. It is generally a
#       bad idea to use atomic_t as many drivers are unaware of this
#       enhancement.])
	ATOMIC_STATS=1
	AC_MSG_RESULT([yes])
    else
	ATOMIC_STATS=0
	AC_MSG_RESULT([no])
    fi
    AC_SUBST([ATOMIC_STATS])dnl
])# _LIS_ATOMIC_STATS
# =============================================================================

# =============================================================================
# _LIS_OUTPUT_CONFIG_IN
# -----------------------------------------------------------------------------
# Output all of the substitutions required by the gcom config.in file.  This
# is so that we can build the config.in file from out config.in.in file.  This
# is useful for any add-on modules that need the LiS configuration file or if
# someone really wants to use the old gcom build process (yuck).
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_OUTPUT_CONFIG_IN], [dnl
dnl -----------------------------------     -----------------------
dnl substitution                            config.in variable name
dnl -----------------------------------     -----------------------
    AC_SUBST([LIS_HOME])                    dnl LIS_HOME
    AC_SUBST([LIS_CONFIG_STREAMS])          dnl CONFIG_STREAMS
    AC_SUBST([LIS_TARGET])                  dnl TARGET
    AC_SUBST([LIS_CROSS_COMPILING])         dnl CROSS_COMPILING
    AC_SUBST([LIS_KSRC])                    dnl KSRC
    AC_SUBST([LIS_NOKSRC])                  dnl NOKSRC
    AC_SUBST([LIS_USE_RUNNING_KERNEL])      dnl USE_RUNNING_KERNEL
    AC_SUBST([LIS_NKVER])                   dnl NKVER
    AC_SUBST([LIS_KVER])                    dnl KVER
    AC_SUBST([LIS_KVER_MAJOR])              dnl KVER_MAJOR
    AC_SUBST([LIS_KVER_MINOR])              dnl KVER_MINOR
    AC_SUBST([LIS_KVER_PATCH])              dnl KVER_PATCH
    AC_SUBST([LIS_KVER_PATCHNO])            dnl KVER_PATCHNO
    AC_SUBST([LIS_KVER_MAJORMINOR])         dnl KVER_MAJORMINOR
    AC_SUBST([LIS_KVER_H])                  dnl KVER_H
    AC_SUBST([LIS_NKVER_H])                 dnl NKVER_H
    AC_SUBST([LIS_KSMP])                    dnl KSMP
    AC_SUBST([LIS_KMODULES])                dnl KMODULES
    AC_SUBST([LIS_KMODVERS])                dnl KMODVERS
    AC_SUBST([LIS_LISMODVERS])              dnl LISMODVERS
    AC_SUBST([LIS_LISAUTOCONF])             dnl LISAUTOCONF
    AC_SUBST([LIS_IPV6])                    dnl IPV6
    AC_SUBST([LIS_IPV6_MODULE])             dnl IPV6_MODULE
    AC_SUBST([LIS_CONFIG_MK_KERNEL])        dnl CONFIG_MK_KERNEL
    AC_SUBST([LIS_STRMS_QUEUES])            dnl STRMS_QUEUES
    AC_SUBST([LIS_MOD_INST_DIR])            dnl MOD_INST_DIR
    AC_SUBST([LIS_MOD_INSTALL])             dnl MOD_INSTALL
    AC_SUBST([LIS_RH_71_KLUDGE])            dnl RH_71_KLUDGE
    AC_SUBST([LIS_SIGMASKLOCK])             dnl SIGMASKLOCK
    AC_SUBST([LIS_RCVOID])                  dnl RCVOID
    AC_SUBST([LIS_MACHINE])                 dnl MACHINE
    AC_SUBST([LIS_CC_NAME])                 dnl CC_NAME
    AC_SUBST([LIS_CC_VERS])                 dnl CC_VERS
    AC_SUBST([LIS_CC_OPTIMIZE])             dnl CC_OPTIMIZE
    AC_SUBST([LIS_MACHINE_INCL])            dnl MACHINE_INCL
    AC_SUBST([LIS_GET_EMPTY_INODE])         dnl GET_EMPTY_INODE
    AC_SUBST([LIS_INT_PSW])                 dnl INT_PSW
    AC_SUBST([LIS_OLD_CONSTS])              dnl LIS_OLD_CONSTS
    AC_SUBST([LIS_SOLARIS_STYLE_CMN_ERR])   dnl SOLARIS_STYLE_CMN_ERR
    AC_SUBST([LIS_DBG_OPT])                 dnl DBG_OPT
    AC_SUBST([LIS_SHLIB])                   dnl LIS_SHLIB
    AC_SUBST([LIS_CONFIG_DEV])              dnl CONFIG_DEV
    AC_SUBST([LIS_USE_KMEM_CACHE])          dnl USE_KMEM_CACHE
    AC_SUBST([LIS_USE_LINUX_KMEM_TIMER])    dnl USE_LINUX_KMEM_TIMER
dnl -----------------------------------     -----------------------
])#_LIS_OUTPUT_CONFIG_IN
# =============================================================================

# =============================================================================
# _LIS_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_LIS_STRCONF], [dnl
    case "$lis_cv_target" in
	user)
	    strconf_cv_stem='Config.user'
	    ;;
	linux | *)
	    strconf_cv_stem='Config'
	    ;;
    esac
    strconf_cv_input='Config.master'
    strconf_cv_majbase=230
    strconf_cv_config='include/sys/LiS/config.h'
    strconf_cv_modconf='head/modconf.inc'
    strconf_cv_drvconf='drvconf.mk'
    strconf_cv_confmod='conf.modules'
    strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes='util/linux/strmakenodes.c'
    strconf_cv_stsetup='strsetup.conf'
    strconf_cv_strload='strload.conf'
    strconf_cv_package='LiS'
    _STRCONF
])# _LIS_STRCONF
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
