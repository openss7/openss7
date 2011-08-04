# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: genksyms.m4,v $ $Name:  $($Revision: 1.1.2.13 $) $Date: 2011-07-27 07:52:18 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2011-07-27 07:52:18 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _GENKSYMS
# -----------------------------------------------------------------------------
AC_DEFUN([_GENKSYMS], [dnl
    AC_REQUIRE([_LINUX_KERNEL])dnl
    AC_MSG_NOTICE([+---------------------------------+])
    AC_MSG_NOTICE([| Kernel Symbol Generation Checks |])
    AC_MSG_NOTICE([+---------------------------------+])
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
dnl     [AS_HELP_STRING([--enable-k-versions],
dnl         [version all symbols @<:@default=automatic@:>@])])
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
    _LINUX_CHECK_KERNEL_CONFIG([for genksyms preempt kernel], [CONFIG_PREEMPT])
    if test :"${linux_cv_CONFIG_PREEMPT:-no}" = :yes ; then
	GENKSYMS_SMP_PREFIX="${GENKSYMS_SMP_PREFIX}${GENKSYMS_SMP_PREFIX:--p }preempt_"
    fi
    _LINUX_CHECK_KERNEL_CONFIG([for genksyms regparms kernel], [CONFIG_REGPARM])
    if test :"${linux_cv_CONFIG_REGPARM:-no}" = :yes ; then
	GENKSYMS_SMP_PREFIX="${GENKSYMS_SMP_PREFIX}${GENKSYMS_SMP_PREFIX:--p }regparm_"
    fi
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    AC_ARG_VAR([GENKSYMS],
	       [Generate kernel symbols command. @<:@default=genksyms@:>@])
    _BLD_PATH_PROG([GENKSYMS], [genksyms], [/sbin/genksyms], [$tmp_path], [dnl
	if test :"$linux_cv_k_ko_modules" != :yes ; then
	    AC_MSG_WARN([Cannot find genksyms program in PATH.])
	fi])
    AC_ARG_VAR([KGENKSYMS],
	       [Generate kernel symbols command. @<:@default=auto@:>@])
    eval "ksyms_dirs=\"$kbuilddir $ksrcdir\""
    ksyms_path=
    for ksyms_tmp in $ksyms_dirs ; do
	ksyms_path="${ksyms_path:+$ksyms_path:}${ksyms_tmp}/scripts/genksyms"
    done
    _BLD_PATH_PROG([KGENKSYMS], [genksyms], [\${kbuilddir}/scripts/genksyms/genksyms],
		 [$ksyms_path], [dnl
	if test :"$linux_cv_k_ko_modules" = :yes ; then
	    AC_MSG_WARN([Could not find executable kernel genksyms program in $ksyms_path.])
	fi])
dnl AC_ARG_VAR([MODPOST],
dnl	       [Kernel module post processing command. @<:@default=modpost@:>@])
dnl _BLD_PATH_PROG([MODPOST], [modpost], [\${kbuilddir}/scripts/mod/modpost],
dnl	         [${kbuilddir}/scripts:${kbuilddir}/scripts/mod], [dnl
dnl	AC_MSG_WARN([Could not find executable kernel modpost program in $kbuilddir/scripts.])])
dnl
dnl This is a weird place to put these I know, but genksyms.am needs it
dnl
    AC_ARG_VAR([OBJDUMP],
	       [Dump object files. @<:@default=objdump@:>@])
    AC_PATH_TOOL([OBJDUMP], [objdump], [], [$tmp_path])
    if test :"${OBJDUMP:-no}" = :no ; then
	AC_MSG_WARN([Could not find executable objdump program in $PATH.])
	OBJDUMP=/usr/bin/objdump
    fi
    AC_ARG_VAR([NM],
	       [List object file symbols. @<:@default=nm@:>@])
    AC_PATH_TOOL([NM], [nm], [], [$tmp_path])
    if test :"${NM:-no}" = :no ; then
	AC_MSG_WARN([Could not find executable nm program in $PATH.])
	NM=/usr/bin/nm
    fi
    if test -f $ac_aux_dir/hot.lds
    then
	PKG_LDS="$ac_aux_dir/hot.lds"
    else
	PKG_LDS=''
    fi
    AC_SUBST([PKG_LDS])
    if test -f $ac_aux_dir/abi.VERSION -a :${linux_cv_k_ko_modules:-no} = :yes
    then
	PKG_ABI="$ac_aux_dir/abi.VERSION"
    else
	PKG_ABI=''
    fi
    AC_SUBST([PKG_ABI])
])# _KSYMS_SETUP
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT_MODSYMS_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT_MODSYMS_CONFIG], [dnl
    AC_CACHE_CHECK([for modsyms script], [MODSYMS_SCRIPT], [MODSYMS_SCRIPT="$ac_aux_dir/modsyms.sh"])
    AC_CACHE_CHECK([for modsyms command], [MODSYMS], [MODSYMS="${CONFIG_SHELL:-$SHELL} $MODSYMS_SCRIPT"])
    AC_CACHE_CHECK([for modsyms file], [MODSYMS_SYMVER], [MODSYMS_SYMVER="Module.symvers"])
    AC_CACHE_CHECK([for modsyms system map], [MODSYMS_SYSMAP], [dnl
	if test :"${kmodver:-no}" != :no
	then
	    MODSYMS_SYSMAP="$kmodver"
	else
	    MODSYMS_SYSMAP="$ksysmap"
	fi
	if test :${linux_cv_k_running:-no} = :yes ; then
	    if test -n "$kallsyms" ; then
		MODSYMS_SYSMAP="$kallsyms"
	    else
		if test -n "$ksyms" ; then
		    MODSYMS_SYSMAP="$ksyms"
		fi
	    fi
	fi
    ])
    AC_CACHE_CHECK([for modsyms module dir], [MODSYMS_MODDIR], [MODSYMS_MODDIR="$kmoddir"])
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
	ksyms_dir="`(cd $ac_aux_dir; pwd)`"
	ksyms_cv_modpost_script="$ksyms_dir/modpost.awk"])
    MODPOST_SCRIPT="$ksyms_cv_modpost_script"
    AC_SUBST([MODPOST_SCRIPT])dnl
    AC_CACHE_CHECK([for modpost command], [ksyms_cv_modpost_command], [dnl
	ksyms_cv_modpost_command="${AWK:-gawk} -f $MODPOST_SCRIPT --"])
    MODPOST="$ksyms_cv_modpost_command"
    AC_SUBST([MODPOST])dnl
    AC_CACHE_CHECK([for modpost sys file], [ksyms_cv_modpost_sysver], [dnl
	ksyms_dir="`pwd`"
	ksyms_cv_modpost_sysver="$ksyms_dir/System.symvers"])
    MODPOST_SYSVER="$ksyms_cv_modpost_sysver"
    AC_SUBST([MODPOST_SYSVER])dnl
    AC_CACHE_CHECK([for modpost symsets file], [ksyms_cv_modpost_symsets], [dnl
	ksyms_dir="`pwd`"
	ksyms_cv_modpost_symsets="$ksyms_dir/symsets-${kversion}.tar.gz"])
    MODPOST_SYMSETS="$ksyms_cv_modpost_symsets"
    AC_SUBST([MODPOST_SYMSETS])dnl
    AC_CACHE_CHECK([for modpost mod file], [ksyms_cv_modpost_modver], [dnl
	ksyms_dir="`pwd`"
	ksyms_cv_modpost_modver="$ksyms_dir/Module.symvers"])
    MODPOST_MODVER="$ksyms_cv_modpost_modver"
    AC_SUBST([MODPOST_MODVER])dnl
    AC_CACHE_CHECK([for modpost kmod file], [ksyms_cv_modpost_kmodver], [dnl
	ksyms_cv_modpost_kmodver="$kmodver"])
    MODPOST_KMODVER="$ksyms_cv_modpost_kmodver"
    AC_SUBST([MODPOST_KMODVER])dnl
    AC_CACHE_CHECK([for modpost system map], [ksyms_cv_modpost_sysmap], [dnl
	ksyms_cv_modpost_sysmap="$ksysmap"])
    MODPOST_SYSMAP="$ksyms_cv_modpost_sysmap"
    AC_SUBST([MODPOST_SYSMAP])dnl
    AC_CACHE_CHECK([for modpost module dir], [ksyms_cv_modpost_moddir], [dnl
	ksyms_cv_modpost_moddir="$kmoddir"])
    MODPOST_MODDIR="$ksyms_cv_modpost_moddir"
    AC_SUBST([MODPOST_MODDIR])dnl
    AC_CACHE_CHECK([for modpost ksymsets file], [ksyms_cv_modpost_ksymsets], [dnl
	ksyms_cv_modpost_ksymsets="$ksymsets"])
    MODPOST_KSYMSETS="$ksyms_cv_modpost_ksymsets"
    AC_SUBST([MODPOST_KSYMSETS])dnl
    AC_CACHE_CHECK([for modpost whitelist file], [ksyms_cv_modpost_whitelist], [dnl
	ksyms_cv_modpost_whitelist="$kabiwhitelist"])
    MODPOST_WHITELIST="$ksyms_cv_modpost_whitelist"
    AC_SUBST([MODPOST_WHITELIST])dnl
    MODPOST_SYMSETS="symsets-${kversion}.tar.gz"
    AC_SUBST([MODPOST_SYMSETS])dnl
dnl AC_ARG_VAR([MODPOST_CACHE], [Cache file for modpost])
    test :"$MODPOST_CACHE" != : && unset ksyms_cv_modpost_cache
    AC_CACHE_CHECK([for modpost cache file], [ksyms_cv_modpost_cache], [dnl
	if test :"$MODPOST_CACHE" != : ; then
	    ksyms_cv_modpost_cache="$MODPOST_CACHE"
	else
	    if test -f ../modpost.cache -a -r ../modpost.cache
	    then
		ksyms_dir="`(cd ..; pwd)`"
	    else
		ksyms_dir="`pwd`"
	    fi
	    ksyms_cv_modpost_cache="$ksyms_dir/modpost.cache"
	fi
    ])
    MODPOST_CACHE="$ksyms_cv_modpost_cache"
    AC_SUBST([MODPOST_CACHE])dnl
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -m option], [CONFIG_MODVERSIONS])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -u option], [CONFIG_MODULE_UNLOAD])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -a option], [CONFIG_MODULE_SRCVERSION_ALL])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -U option], [CONFIG_MODULE_FORCE_UNLOAD])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -s option], [CONFIG_MODULE_SIG])
    _LINUX_CHECK_KERNEL_CONFIG([for modpost -S option], [CONFIG_MODULE_SIG_FORCE])
    AC_CACHE_CHECK([for modpost options], [ksyms_cv_modpost_options], [dnl
	ksyms_cv_modpost_options=
	if test :${linux_cv_CONFIG_MODVERSIONS:-no} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-m"
	fi
	if test :${linux_cv_CONFIG_MODULE_UNLOAD:-no} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-u"
	fi
	if test :${linux_cv_CONFIG_SRCVERSION_ALL:-no} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-a"
	fi
	if test -r $MODPOST_KMODVER ; then
	    if (grep EXPORT_SYMBOL $MODPOST_KMODVER>/dev/null) ; then
		ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-x"
	    fi
	fi
	if test :${linux_cv_k_weak_modules:-yes} = :no ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-r"
	fi
	if test :${linux_cv_k_abi_support:-yes} = :no ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-U"
	fi
	if test :${linux_cv_k_weak_symbols:-yes} = :yes ; then
	    ksyms_cv_modpost_options="${ksyms_cv_modpost_options:+$ksyms_cv_modpost_options }-w"
	fi
    ])
    MODPOST_OPTIONS="$ksyms_cv_modpost_options"
    AC_SUBST([MODPOST_OPTIONS])dnl
    MODPOST_INPUTS="$MODPOST_SYSVER $MODPOST_KMODVER"
    AC_SUBST([MODPOST_INPUTS])dnl
    AC_CONFIG_COMMANDS([modpost], [dnl
	AC_MSG_NOTICE([creating $MODPOST_SYSVER from $MODPOST_SYSMAP, $MODPOST_MODDIR and $MODPOST_KMODVER])
	eval "MODPOST_CACHE=$MODPOST_CACHE $MODPOST${MODPOST_OPTIONS:+ $MODPOST_OPTIONS}${MODPOST_SYSMAP:+ -F $MODPOST_SYSMAP}${MODPOST_MODDIR:+ -d $MODPOST_MODDIR}${MODPOST_KMODVER:+ -i $MODPOST_KMODVER}${MODPOST_KSYMSETS:+ -K $MODPOST_KSYMSETS}${MODPOST_WHITELIST:+ -L $MODPOST_WHITELIST} -s $MODPOST_SYSVER" 2>&1 | \
	while read line ; do
	    echo "$as_me:$LINENO: $line" >&5
	    echo "$as_me: $line" >&2
	done
    ], [dnl
rootdir="$rootdir"
kversion="$kversion"
MODPOST="$MODPOST"
MODPOST_SCRIPT="$MODPOST_SCRIPT"
MODPOST_SYSVER="$MODPOST_SYSVER"
MODPOST_SYMSETS="$MODPOST_SYMSETS"
MODPOST_MODVER="$MODPOST_MODVER"
MODPOST_KMODVER="$MODPOST_KMODVER"
MODPOST_SYSMAP="$MODPOST_SYSMAP"
MODPOST_MODDIR="$MODPOST_MODDIR"
MODPOST_CACHE="$MODPOST_CACHE"
MODPOST_OPTIONS="$MODPOST_OPTIONS"
MODPOST_INPUTS="$MODPOST_INPUTS"
MODPOST_KSYMSETS="$MODPOST_KSYMSETS"
MODPOST_WHITELIST="$MODPOST_WHITELIST"
    ])
])# _KSYMS_OUTPUT_MODPOST_CONFIG
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT_UPDATES_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT_UPDATES_CONFIG], [dnl
    AC_CACHE_CHECK([for updates script], [ksyms_cv_updates_script], [dnl
	ksyms_dir="`(cd $ac_aux_dir; pwd)`"
	ksyms_cv_updates_script="$ksyms_dir/openss7-modules"])
    UPDATES_SCRIPT="$ksyms_cv_updates_script"
    AC_SUBST([UPDATES_SCRIPT])dnl
    AC_CACHE_CHECK([for updates command], [ksyms_cv_updates_command], [dnl
	ksyms_cv_updates_command="${AWK:-gawk} -f $UPDATES_SCRIPT --"])
    UPDATES="$ksyms_cv_updates_command"
    AC_SUBST([UPDATES])dnl
    AC_CACHE_CHECK([for updates options], [ksyms_cv_updates_options], [dnl
	case "$target_vendor" in
	    (oracle|puias|centos|lineox|whitebox|scientific|redhat|fedora)
		ksyms_cv_updates_options='--style=redhat' ;;
	    (suse|opensuse|sles|sled)
		ksyms_cv_updates_options='--style=suse11' ;;
	    (ubuntu|lts)
		ksyms_cv_updates_options='--style=ubuntu' ;;
	    (debian|mandrake|mandriva|manbo|mageia|mes|*)
		ksyms_cv_updates_options='--style=debian' ;;
	esac])
    UPDATES_OPTIONS="${ksyms_cv_updates_options:+ $ksyms_cv_updates_options}"
    AC_SUBST([UPDATES_OPTIONS])dnl
    AC_CACHE_CHECK([for updates directory], [ksyms_cv_updates_directory], [dnl
	AC_ARG_WITH([k-updates],
	    [AS_HELP_STRING([--with-k-updates=DIR],
		[kernel update script directory @<:@default=/etc/kernel@:>@])])
	if test :"${with_k_updates:-no}" != :no
	then
	    ksyms_cv_updates_directory="$with_k_updates"
	else
	    eval "k_updatedir_search_path=\"
		${DESTDIR}${rootdir}/etc/kernel
		${DESTDIR}/etc/kernel\""
	    k_updatedir_search_path=`echo "$k_updatedir_search_path" | sed -e 's,\<NONE\>,,g;s,//,/,g'`
	    ksyms_cv_updates_directory=
	    for ksyms_dir in $k_updatedir_search_path
	    do
		AC_MSG_CHECKING([for updates directory... $ksyms_dir])
		if test -d "$ksyms_dir" -a \( \
		    -d "$ksyms_dir/preinst.d" -o \
		    -d "$ksyms_dir/postinst.d" -o \
		    -d "$ksyms_dir/prerm.d" -o \
		    -d "$ksyms_dir/postrm.d" -o \
		    -d "$ksyms_dir/header_postinst.d" -o \
		    -d "$ksyms_dir/src_postinst.d" \)
		then
		    ksyms_cv_updates_directory="$ksyms_dir"
		    AC_MSG_RESULT([yes])
		    break
		fi
		AC_MSG_RESULT([no])
	    done
	fi
	if test :"${ksyms_cv_updates_directory:-no}" = :no -o ! -d "$ksyms_cv_updates_directory"
	then
	    case "$target_vendor" in
		(debian|unbuntu|mint)
		    if test -z "${DESTDIR}${rootdir}" ; then
			AC_MSG_WARN([
***
*** Configure cannot find the kernel updates directory, /etc/kernel,
*** even though the distribution supports it.  Proceeding under the
*** assumption that that --with-k-updates=/etc/kernel was intended.
*** ])
		    fi
		    ksyms_cv_updates_directory=/etc/kernel
		    ;;
		(redhat|rhel|fedora|oracle|puias|centos|scientific|lineox|whitebox)
		    if test -z "${DESTDIR}${rootdir}" ; then
			AC_MSG_WARN([
***
*** Configure cannot find the kernel updates directory, /etc/kernel,
*** even though the distribution supports it.  This is likely because
*** the DKMS package is not installed.  Proceeding under the assumption
*** that that --with-k-updates=/etc/kernel was intended.
*** ])
		    fi
		    ksyms_cv_updates_directory=/etc/kernel
		    ;;
		(sles|sled|sle|suse|opensuse)
		    ;;
		(mandrake|mandriva|manbo|mageia|mes)
		    ;;
		(*) if test -x "${DESTDIR}${rootdir}/sbin/new-kernel-pkg" ; then
			if test -z "${DESTDIR}${rootdir}" ; then
			    AC_MSG_WARN([
***
*** Configure cannot find the kernel updates directory, /etc/kernel.
*** This is likely because the DKMS package is not installed or
*** supported and the distro does not provide /sbin/new-kernel-pkg.
*** ])
			fi
		    fi
		    ;;
	    esac
	fi
	AC_MSG_CHECKING([for updates directory])
    ])
    kupdatedir="$ksyms_cv_updates_directory"
    AC_SUBST([kupdatedir])dnl
    AM_CONDITIONAL([KERNEL_UPDATES], [test :"${ksyms_cv_updates_directory:-no}" != :no])dnl
])# _KSYMS_OUTPUT_UPDATES_CONFIG
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT_SYMSETS_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT_SYMSETS_CONFIG], [dnl
    AC_CACHE_CHECK([for symsets script], [ksyms_cv_symsets_script], [dnl
	ksyms_dir="`(cd $ac_aux_dir; pwd)`"
	ksyms_cv_symsets_script="$ksyms_dir/symsets.awk"])
    SYMSETS_SCRIPT="$ksyms_cv_symsets_script"
    AC_SUBST([SYMSETS_SCRIPT])dnl
    AC_CACHE_CHECK([for symsets command], [ksyms_cv_symsets_command], [dnl
	ksyms_cv_symsets_command="${AWK:-gawk} -f $SYMSETS_SCRIPT --"])
    SYMSETS="$ksyms_cv_symsets_command"
    AC_SUBST([SYMSETS])dnl
    AC_CACHE_CHECK([for symsets options], [ksyms_cv_symsets_options], [dnl
	case "$target_vendor" in
	    (oracle|puias|centos|lineox|whitebox|scientific|redhat)
		ksyms_cv_symsets_options='--redhat' ;;
	    (*) ksyms_cv_symsets_options='--suse'   ;;
	esac])
    SYMSETS_OPTIONS="${ksyms_cv_symsets_options:+ $ksyms_cv_symsets_options}"
    AC_SUBST([SYMSETS_OPTIONS])dnl
])# _KSYMS_OUTPUT_SYMSETS_CONFIG
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT_MODVER_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT_MODVER_CONFIG], [dnl
    AC_CONFIG_COMMANDS([modvers], [dnl
	AC_MSG_NOTICE([creating $MODVERSIONS_H])
	mkdir -p -- $PKGINCL
	touch $MODVERSIONS_H
    ], [dnl
PKGINCL="$PKGINCL"
MODVERSIONS_H="$MODVERSIONS_H"
    ])
])# _KSYMS_OUTPUT_MODVER_CONFIG
# =============================================================================

# =============================================================================
# _KSYMS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_KSYMS_OUTPUT], [dnl
    AC_SUBST([GENKSYMS_SMP_PREFIX])dnl
dnl _KSYMS_OUTPUT_MODSYMS_CONFIG
    if test :${linux_cv_k_ko_modules:-no} = :yes ; then
	_KSYMS_OUTPUT_MODPOST_CONFIG
	_KSYMS_OUTPUT_UPDATES_CONFIG
dnl	_KSYMS_OUTPUT_SYMSETS_CONFIG
    else
	MODVERSIONS_H="${PKGINCL}/modversions.h"
	_KSYMS_OUTPUT_MODVER_CONFIG
    fi
    AC_SUBST([MODVERSIONS_H])dnl
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
# $Log: genksyms.m4,v $
# Revision 1.1.2.13  2011-07-27 07:52:18  brian
# - work to support Mageia/Mandriva compressed kernel modules and URPMI repo
#
# Revision 1.1.2.12  2011-06-09 11:30:54  brian
# - support mageia and mes
#
# Revision 1.1.2.11  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.10  2011-05-10 13:45:34  brian
# - weak modules workup
#
# Revision 1.1.2.9  2011-04-11 06:13:43  brian
# - working up weak updates
#
# Revision 1.1.2.8  2011-03-26 04:28:45  brian
# - updates to build process
#
# Revision 1.1.2.7  2011-03-17 07:01:28  brian
# - build and repo system improvements
#
# Revision 1.1.2.6  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.5  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.4  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.3  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.2  2009-06-22 03:34:24  brian
# - updates for release
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# Revision 0.9.2.28  2008-09-21 07:40:45  brian
# - add defaults to environment variables
#
# Revision 0.9.2.27  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.26  2007/10/17 20:00:27  brian
# - slightly different path checks
#
# Revision 0.9.2.25  2007/08/12 19:05:30  brian
# - rearrange and update headers
#
# =============================================================================
# 
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
