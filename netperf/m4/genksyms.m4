# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSFile$ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/03/08 00:22:36 $
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
# Last Modified $Date: 2005/03/08 00:22:36 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _GENKSYMS
# -----------------------------------------------------------------------------
AC_DEFUN([_GENKSYMS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    _KSYMS_OPTIONS
    _LINUX_KERNEL_ENV([_KSYMS_SETUP])
    _KSYMS_OUTPUT
])# _GENKSYMS
# =============================================================================

# =============================================================================
# _KSYMS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OPTIONS], [dnl
dnl AC_ARG_ENABLE([k-versions],
dnl     AS_HELP_STRING([--enable-k-versions],
dnl         [version all symbols @<:@default=automatic@:>@]),
dnl     [enable_k_versions="$enableval"],
dnl     [enable_k_versions=''])
])# _KSYMS_OPTIONS
# =============================================================================

# =============================================================================
# _KSYMS_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_SETUP], [dnl
    _LINUX_CHECK_KERNEL_CONFIG([for genksyms smp kernel], [CONFIG_SMP])
    if test :"${linux_cv_CONFIG_SMP:-no}" = :yes ; then
	GENKSYMS_SMP_PREFIX='-p smp_'
    fi
    _LINUX_CHECK_KERNEL_CONFIG([for genksyms SuSE production kernel], [CONFIG_REGPARM])
    if test :"${linux_cv_CONFIG_REGPARM:-no}" = :yes ; then
	GENKSYMS_SMP_PREFIX="${GENKSYMS_SMP_PREFIX}${GENKSYMS_SMP_PREFIX:--p }regparm_"
    fi
    AC_ARG_VAR([GENKSYMS], [Generate kernel symbols command])
    AC_PATH_TOOL([GENKSYMS], [genksyms], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${GENKSYMS:-no}" = :no ; then
	AC_MSG_WARN([Could not find genksyms program in PATH.])
	GENKSYMS=/sbin/genksyms
    fi
    AC_ARG_VAR([KGENKSYMS], [Generate kernel symbols command])
    AC_PATH_TOOL([KGENKSYMS], [genksyms], [], [${kbuilddir}/scripts/genksyms])
    if test :"${KGENKSYMS:-no}" = :no ; then
	AC_MSG_WARN([Could not find executable kernel genksyms program in $kbuilddir/scripts/genksyms.])
	KGENKSYMS='${kbuilddir}/scripts/genksyms/genksyms'
    fi
dnl AC_ARG_VAR([MODPOST], [Kernel module post processing command])
dnl AC_PATH_TOOL([MODPOST], [modpost], [], [${kbuilddir}/scripts:${kbuilddir}/scripts/mod])
dnl if test :"${MODPOST:-no}" = :no ; then
dnl	AC_MSG_WARN([Could not find executable kernel modpost program in $kbuilddir/scripts.])
dnl	MODPOST='${kbuilddir}/scripts/mod/modpost'
dnl fi
dnl
dnl This is a weird place to put these I know, but genksyms.am needs it
dnl
    AC_ARG_VAR([OBJDUMP], [Dump object files])
    AC_PATH_TOOL([OBJDUMP], [objdump], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${OBJDUMP:-no}" = :no ; then
	AC_MSG_WARN([Could not find executable objdump program in $PATH.])
	OBJDUMP=/usr/bin/objdump
    fi
    AC_ARG_VAR([NM], [List object file symbols])
    AC_PATH_TOOL([NM], [nm], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
    if test :"${NM:-no}" = :no ; then
	AC_MSG_WARN([Could not find executable nm program in $PATH.])
	NM=/usr/bin/nm
    fi
])# _KSYMS_SETUP
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT_MODSYMS_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT_MODSYMS_CONFIG], [dnl
    AC_CACHE_CHECK([for modsyms script], [MODSYMS_SCRIPT], [MODSYMS_SCRIPT="$ac_aux_dir/modsyms.sh"])
    AC_CACHE_CHECK([for modsyms command], [MODSYMS], [MODSYMS="${CONFIG_SHELL:-$SHELL} $MODSYMS_SCRIPT"])
    AC_CACHE_CHECK([for modsyms file], [MODSYMS_SYMVER], [MODSYMS_SYMVER="Modules.symver"])
    AC_CACHE_CHECK([for modsyms system map], [MODSYMS_SYSMAP], [dnl
	if test :${linux_cv_k_running:-no} = :no ; then
	    MODSYMS_SYSMAP="$ksysmap"
	else
	    MODSYMS_SYSMAP="$ksyms"
	fi
    ])
    AC_CACHE_CHECK([for modsyms module dir], [MODSYMS_MODDIR], [MODSYMS_MODDIR="$kmoduledir"])
    AC_SUBST([MODSYMS_SCRIPT])
    AC_SUBST([MODSYMS])
    AC_SUBST([MODSYMS_SYMVER])
    AC_SUBST([MODSYMS_SYSMAP])
    AC_SUBST([MODSYMS_MODDIR])
    AC_CONFIG_COMMANDS([modsyms], [dnl
	if test :${MODSYMS_SYMVER:+set} = :set ; then
	    AC_MSG_NOTICE([creating $MODSYMS_SYMVER from $MODSYMS_SYSMAP and $MODSYMS_MODDIR])
	    eval "$MODSYMS${MODSYMS_SYSMAP:+ -F $MODSYMS_SYSMAP}${MODSYMS_MODDIR:+ -d $MODSYMS_MODDIR} > $MODSYMS_SYMVER"
	fi
    ], [dnl
rootdir="$rootdir"
kversion="$kversion"
MODSYMS="$MODSYMS"
MODSYMS_SCRIPT="$MODSYMS_SCRIPT"
MODSYMS_SYMVER="$MODSYMS_SYMVER"
MODSYMS_SYSMAP="$MODSYMS_SYSMAP"
MODSYMS_MODDIR="$MODSYMS_MODDIR"
    ])
])# _KSYMS_OUTPUT_MODSYMS_CONFIG
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT_MODPOST_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT_MODPOST_CONFIG], [dnl
    AC_CACHE_CHECK([for modpost script], [ksyms_cv_modpost_script], [dnl
	ksyms_cv_modpost_script="$ac_aux_dir/modpost.sh"])
    MODPOST_SCRIPT="$ksyms_cv_modpost_script"
    AC_SUBST([MODPOST_SCRIPT])dnl
    AC_CACHE_CHECK([for modpost command], [ksyms_cv_modpost_command], [dnl
	ksyms_cv_modpost_command="${CONFIG_SHELL:-$SHELL} $MODPOST_SCRIPT"])
    MODPOST="$ksyms_cv_modpost_command"
    AC_SUBST([MODPOST])dnl
    AC_CACHE_CHECK([for modpost sys file], [ksyms_cv_modpost_sysver], [dnl
	ksyms_cv_modpost_sysver="System.symver"])
    MODPOST_SYSVER="$ksyms_cv_modpost_sysver"
    AC_SUBST([MODPOST_SYSVER])dnl
    AC_CACHE_CHECK([for modpost mod file], [ksyms_cv_modpost_modver], [dnl
	ksyms_cv_modpost_modver="Modules.symver"])
    MODPOST_MODVER="$ksyms_cv_modpost_modver"
    AC_SUBST([MODPOST_MODVER])dnl
    AC_CACHE_CHECK([for modpost system map], [ksyms_cv_modpost_sysmap], [dnl
	ksyms_cv_modpost_sysmap="$ksysmap"])
    MODPOST_SYSMAP="$ksyms_cv_modpost_sysmap"
    AC_SUBST([MODPOST_SYSMAP])dnl
    AC_CACHE_CHECK([for modpost module dir], [ksyms_cv_modpost_moddir], [dnl
	ksyms_cv_modpost_moddir="$kmoduledir"])
    MODPOST_MODDIR="$ksyms_cv_modpost_moddir"
    AC_SUBST([MODPOST_MODDIR])dnl
    AC_ARG_VAR([MODPOST_CACHE], [Cache file for modpost])
    test :"$MODPOST_CACHE" != : && unset ksyms_cv_modpost_cache
    AC_CACHE_CHECK([for modpost cache file], [ksyms_cv_modpost_cache], [dnl
	if test :$MODPOST_CACHE != : ; then
	    ksyms_cv_modpost_cache="$MODPOST_CACHE"
	else
	    if test -f ../modpost.cache -a -r ../modpost.cache
	    then
		ksyms_cv_modpost_cache="`(cd ..; pwd)`/modpost.cache"
	    else
		ksyms_cv_modpost_cache="modpost.cache"
	    fi
	fi
    ])
    MODPOST_CACHE="$ksyms_cv_modpost_cache"
    AC_SUBST([MODPOST_CACHE])dnl
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -m option], [CONFIG_MODVERSIONS])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -u option], [CONFIG_MODULE_UNLOAD])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -a option], [CONFIG_MODULE_SRCVERSION_ALL])
    AC_CACHE_CHECK([for modpost options], [ksyms_cv_modpost_options], [dnl
	ksyms_cv_modpost_options=
	if test :${linux_cv_CONFIG_MODVERSIONS:-no} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options}${ksyms_cv_modpost_options:+ }-m"
	fi
	if test :${linux_cv_CONFIG_MODULE_UNLOAD:-no} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options}${ksyms_cv_modpost_options:+ }-u"
	fi
	if test :${linux_cv_CONFIG_SRCVERSION_ALL:-no} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options}${ksyms_cv_modpost_options:+ }-a"
	fi
    ])
    MODPOST_OPTIONS="$ksyms_cv_modpost_options"
    AC_SUBST([MODPOST_OPTIONS])dnl
    MODPOST_INPUTS="$MODPOST_SYSVER"
    AC_SUBST([MODPOST_INPUTS])dnl
    AC_CONFIG_COMMANDS([modpost], [dnl
	AC_MSG_NOTICE([creating $MODPOST_SYSVER from $MODPOST_SYSMAP, $MODPOST_MODDIR and $MODPOST_INPUTS])
	eval "MODPOST_CACHE=$MODPOST_CACHE $MODPOST -vv${MODPOST_OPTIONS:+ $MODPOST_OPTIONS}${MODPOST_SYSMAP:+ -F $MODPOST_SYSMAP}${MODPOST_MODDIR:+ -d $MODPOST_MODDIR}${MODPOST_INPUTS:+ -i '$MODPOST_INPUTS'} -s $MODPOST_SYSVER"
    ], [dnl
rootdir="$rootdir"
kversion="$kversion"
MODPOST="$MODPOST"
MODPOST_SCRIPT="$MODPOST_SCRIPT"
MODPOST_SYSVER="$MODPOST_SYSVER"
MODPOST_MODVER="$MODPOST_MODVER"
MODPOST_SYSMAP="$MODPOST_SYSMAP"
MODPOST_MODDIR="$MODPOST_MODDIR"
MODPOST_CACHE="$MODPOST_CACHE"
MODPOST_OPTIONS="$MODPOST_OPTIONS"
MODPOST_INPUTS="$MODPOST_INPUTS"
    ])
])# _KSYMS_OUTPUT_MODPOST_CONFIG
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT], [dnl
    AC_SUBST([GENKSYMS_SMP_PREFIX])dnl
dnl _KSYMS_OUTPUT_MODSYMS_CONFIG
    if test :${linux_cv_k_ko_modules:-no} = :yes ; then
	_KSYMS_OUTPUT_MODPOST_CONFIG
    fi
])# _KSYMS_OUTPUT
# =============================================================================

# =============================================================================
# _KSYMS_
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_], [dnl
])# _KSYMS_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
