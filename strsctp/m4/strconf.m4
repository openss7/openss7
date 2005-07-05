# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
# 
# @(#) $RCSfile: strconf.m4,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2005/07/04 19:57:40 $
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
# Last Modified $Date: 2005/07/04 19:57:40 $ by $Author: brian $
#
# =============================================================================


# =============================================================================
# _STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCONF], [dnl
dnl AC_MSG_NOTICE([------------------------------])
dnl AC_MSG_NOTICE([starting STREAMS configuration])
dnl AC_MSG_NOTICE([------------------------------])
    _STRCONF_OPTIONS
    _STRCONF_SETUP
    _STRCONF_OUTPUT
dnl AC_MSG_NOTICE([------------------------------])
dnl AC_MSG_NOTICE([complete STREAMS configuration])
dnl AC_MSG_NOTICE([------------------------------])
])# _STRCONF
# =============================================================================

# =============================================================================
# _STRCONF_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCONF_OPTIONS], [dnl
])# _STRCONF_OPTIONS
# =============================================================================

# =============================================================================
# _STRCONF_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCONF_SETUP], [dnl
    AC_MSG_CHECKING([for strconf stem file name])
    STRCONF_STEM="${strconf_cv_stem:-Config}"
    AC_MSG_RESULT([$STRCONF_STEM])
    AC_MSG_CHECKING([for strconf config files in $srcdir])
    STRCONF_CONFIGS=
    for strconf_tmp in `find $srcdir -name "$STRCONF_STEM" -o -name "$STRCONF_STEM.in"` ; do
	if test -f "$strconf_tmp" ; then
	    STRCONF_CONFIGS="${STRCONF_CONFIGS}${STRCONF_CONFIGS:+ }$strconf_tmp"
	fi
    done
    AC_MSG_RESULT([$STRCONF_CONFIGS])
    AC_MSG_CHECKING([for strconf script])
    STRCONF_SCRIPT="$ac_aux_dir/strconf-sh"
    AC_MSG_RESULT([${STRCONF_SCRIPT}])
    AC_MSG_CHECKING([for strconf command])
    STRCONF="$SHELL $STRCONF_SCRIPT"
    AC_MSG_RESULT([${STRCONF}])
    AC_ARG_WITH([strconf-master],
	AS_HELP_STRING([--with-strconf-master=FILENAME],
	    [specify the file name to which the configuration master
	    file is written @<:@default=Config.master@:>@]),
	[with_strconf_master="$withval"],
	[with_strconf_master=''])
    AC_MSG_CHECKING([for strconf master file])
    if test :"${with_strconf_master:-no}" != :no ; then
	strconf_cv_input="$with_strconf_master"
    fi
    STRCONF_INPUT="${strconf_cv_input:-Config.master}"
    AC_MSG_RESULT([${STRCONF_INPUT}])
    AC_ARG_WITH([base-major],
	AS_HELP_STRING([--with-base-major=MAJOR],
	    [specify the base major device number from which to start
	    numbering major devices @<:@default=230@:>@]),
	[with_base_major="$withval"],
	[with_base_major=''])
    AC_MSG_CHECKING([for strconf base major device number])
    if test :"${with_base_major:-no}" != :no ; then
	strconf_cv_majbase="$with_base_major"
    fi
    STRCONF_MAJBASE="${strconf_cv_majbase:-230}"
    AC_MSG_RESULT([${STRCONF_MAJBASE}])
    AC_MSG_CHECKING([for strconf configuration header file name])
    STRCONF_CONFIG="${strconf_cv_config:-include/sys/$PACKAGE_TARNAME/config.h}"
    AC_MSG_RESULT([${STRCONF_CONFIG}])
    AC_MSG_CHECKING([for strconf modules include file name])
    STRCONF_MODCONF="${strconf_cv_modconf:-head/modconf.inc}"
    AC_MSG_RESULT([${STRCONF_MODCONF}])
    AC_MSG_CHECKING([for strconf modules makefile include file name])
    STRCONF_DRVCONF="${strconf_cv_drvconf:-drvconf.mk}"
    AC_MSG_RESULT([${STRCONF_DRVCONF}])
    AC_MSG_CHECKING([for strconf kernel modules configuration file name])
    STRCONF_CONFMOD="${strconf_cv_confmod:-conf.modules}"
    AC_MSG_RESULT([${STRCONF_CONFMOD}])
    AC_MSG_CHECKING([for strconf rpm devices list file name])
    STRCONF_MAKEDEV="${strconf_cv_makedev:-devices.lst}"
    AC_MSG_RESULT([${STRCONF_MAKEDEV}])
    AC_MSG_CHECKING([for strconf makenodes source file name])
    STRCONF_MKNODES="${strconf_cv_mknodes:-util/linux/strmakenodes.c}"
    STRMAKENODES=`basename "$STRCONF_MKNODES" .c`
    AC_MSG_RESULT([${STRCONF_MKNODES}])
    AC_MSG_CHECKING([for strconf strsetup configuration file name])
    STRCONF_STSETUP="${strconf_cv_stsetup:-strsetup.conf}"
    AC_MSG_RESULT([${STRCONF_STSETUP}])
    AC_MSG_CHECKING([for strconf strload configuration file name])
    STRCONF_STRLOAD="${strconf_cv_strload:-strload.conf}"
    AC_MSG_RESULT([${STRCONF_STRLOAD}])
    AC_MSG_CHECKING([for strconf STREAMS package])
    STRCONF_PACKAGE="${strconf_cv_package:-LiS}"
    AC_MSG_RESULT([${STRCONF_PACKAGE}])
    AC_MSG_CHECKING([for strconf minor bits])
    STRCONF_MINORSZ="${strconf_cv_minorbits:-8}"
    AC_MSG_RESULT([${STRCONF_MINORBITS}])
])# _STRCONF_SETUP
# =============================================================================

# =============================================================================
# _STRCONF_OUTPUT_CONFIG_COMMANDS
# -------------------------------------------------------------------------
# The point of deferring the search for configuration files is so that we can
# generate configuration files as output from autoconf before using the
# strconf script to generate the output files.
# -------------------------------------------------------------------------
AC_DEFUN([_STRCONF_OUTPUT_CONFIG_COMMANDS], [dnl
    AC_MSG_NOTICE([searching for $STRCONF_INPUT input files in  $ac_srcdir and $ac_builddir])
    strconf_configs=
    # config.status idea of absolute is not absolute, might be an autoconf bug
    ac_abs_srcdir=`( cd $ac_srcdir ; /bin/pwd )`
    ac_abs_builddir=`( cd $ac_builddir ; /bin/pwd )`
    strconf_list="`find $ac_abs_srcdir/ $ac_abs_builddir/ -type f -name \"$STRCONF_STEM\" | sort | uniq`"
    for strconf_tmp in $strconf_list ; do
	case $strconf_tmp in
	    ("$ac_abs_builddir"*/"$PACKAGE"/*) continue ;;
	    ("$ac_abs_builddir"*/"$PACKAGE-$VERSION"/*) continue ;;
	    ("$ac_abs_builddir"*/"$PACKAGE-bin-$VERSION"/*) continue ;;
	esac
	if test -r "$strconf_tmp" ; then
	    strconf_configs="${strconf_configs}${strconf_configs:+ }$strconf_tmp"
	fi
    done
    if test -n "${strconf_configs}" -a -n "${STRCONF_INPUT}"; then
	AC_MSG_NOTICE([creating $STRCONF_INPUT])
	cat /dev/null > $STRCONF_INPUT
	for file in $strconf_configs
	do
	    AC_MSG_NOTICE([appending $file to  $STRCONF_INPUT])
	    ( echo "#" ; echo "# included from $file `date`" ; echo "#" ; cat $file ) | sed -e '/^##/d' | cat -s >> $STRCONF_INPUT
	done
	if test :"${STRCONF_CONFIG:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_CONFIG from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --hconfig=$STRCONF_CONFIG $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_MODCONF:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_MODCONF from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --modconf=$STRCONF_MODCONF $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_MKNODES:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_MKNODES from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --makenodes=$STRCONF_MKNODES $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_DRVCONF:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_DRVCONF from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --driverconf=$STRCONF_DRVCONF $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_CONFMOD:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_CONFMOD from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --confmodules=$STRCONF_CONFMOD $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_MAKEDEV:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_MAKEDEV from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --strmknods=$STRCONF_MAKEDEV $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_STSETUP:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_STSETUP from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --strsetup=$STRCONF_STSETUP $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
	if test :"${STRCONF_STRLOAD:+set}" = :set; then
	    AC_MSG_NOTICE([creating $STRCONF_STRLOAD from $STRCONF_INPUT])
	    eval "$STRCONF --package=${STRCONF_PACKAGE} -B${STRCONF_MINORSZ} -b${STRCONF_MAJBASE} --strload=$STRCONF_STRLOAD $STRCONF_INPUT" 2>&1 | \
	    while read line ; do
		echo "$as_me:$LINENO: $line" >&5
		echo "$as_me: $line" >&2
	    done
	fi
    fi
])# _STRCONF_OUTPUT_CONFIG_COMMANDS
# =============================================================================

# =============================================================================
# _STRCONF_OUTPUT_CONFIG
# -------------------------------------------------------------------------
AC_DEFUN([_STRCONF_OUTPUT_CONFIG], [dnl
    AC_CONFIG_COMMANDS([strconf],
	[_STRCONF_OUTPUT_CONFIG_COMMANDS], [dnl
ac_aux_dir="$ac_aux_dir"
PACKAGE="$PACKAGE"
VERSION="$VERSION"
PACKAGE_TARNAME="$PACKAGE_TARNAME"
PACKAGE_VERSION="$PACKAGE_VERSION"
PACKAGE_RELEASE="$PACKAGE_RELEASE"
STRCONF="$STRCONF"
STRCONF_STEM="$STRCONF_STEM"
STRCONF_SCRIPT="$STRCONF_SCRIPT"
STRCONF_INPUT="$STRCONF_INPUT"
STRCONF_MAJBASE="$STRCONF_MAJBASE"
STRCONF_CONFIG="$STRCONF_CONFIG"
STRCONF_MODCONF="$STRCONF_MODCONF"
STRCONF_MKNODES="$STRCONF_MKNODES"
STRCONF_DRVCONF="$STRCONF_DRVCONF"
STRCONF_CONFMOD="$STRCONF_CONFMOD"
STRCONF_MAKEDEV="$STRCONF_MAKEDEV"
STRCONF_STSETUP="$STRCONF_STSETUP"
STRCONF_STRLOAD="$STRCONF_STRLOAD"
STRCONF_PACKAGE="$STRCONF_PACKAGE"
STRCONF_MINORSZ="$STRCONF_MINORSZ"
    ])
])# _STRCONF_OUTPUT_CONFIG
# =============================================================================

# =============================================================================
# _STRCONF_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_STRCONF_OUTPUT], [dnl
	AC_SUBST([STRCONF])dnl
	AC_SUBST([STRCONF_STEM])dnl
	AC_SUBST([STRCONF_SCRIPT])dnl
	AC_SUBST([STRCONF_INPUT])dnl
	AC_SUBST([STRCONF_CONFIGS])dnl
	AC_SUBST([STRCONF_MAJBASE])dnl
	AC_SUBST([STRCONF_CONFIG])dnl
	AC_SUBST([STRCONF_MODCONF])dnl
	AC_SUBST([STRCONF_MKNODES])dnl
	AC_SUBST([STRCONF_DRVCONF])dnl
	AC_SUBST([STRCONF_CONFMOD])dnl
	AC_SUBST([STRCONF_MAKEDEV])dnl
	AC_SUBST([STRCONF_STSETUP])dnl
	AC_SUBST([STRCONF_STRLOAD])dnl
	AC_SUBST([STRCONF_PACKAGE])dnl
	AC_SUBST([STRMAKENODES])dnl
	_STRCONF_OUTPUT_CONFIG
])# _STRCONF_OUTPUT
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 noet nocindent
# =============================================================================
