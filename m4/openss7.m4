# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
# 
# @(#) $RCSfile: openss7.m4,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/01/10 10:24:53 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
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
# Last Modified $Date: 2005/01/10 10:24:53 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
# $Log: openss7.m4,v $
# Revision 0.9.2.4  2005/01/10 10:24:53  brian
# - Correct genksyms cache clash.
#
# Revision 0.9.2.3  2004/12/21 12:22:06  brian
# - Added automatic caching to all packages.
#
# Revision 0.9.2.2  2004/08/17 11:41:24  brian
# - Added package description macros.
#
# =============================================================================

# =============================================================================
# _OPENSS7_PACKAGE([SHORT-TITLE], [LONG-TITLE])
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_PACKAGE], [
    PACKAGE_TITLE='$2'
    AC_SUBST([PACKAGE_TITLE])dnl
    PACKAGE_SHORTTITLE='$1'
    AC_SUBST([PACKAGE_SHORTTITLE])dnl
    _OPENSS7_CACHE
])# _OPENSS7_PACKAGE
# =============================================================================

# =============================================================================
# _OPENSS7_CACHE
# -----------------------------------------------------------------------------
# A little trick with caches and site files.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_CACHE], [
    # force a cache file to be created even if not specified
    if test "$cache_file" = '/dev/null' -o :"$cache_file" = :
    then
        cache_file='config.cache'
        if test ! -e "$cache_file"
        then
            cat /dev/null > "$cache_file"
        fi
    fi
    # if site file not specified, use local site file
    if test :"$CONFIG_SITE" = :
    then
        CONFIG_SITE='config.site'
    fi
    AC_CONFIG_COMMANDS([siteconfig], [dnl
        if test :"$CONFIG_SITE" != : -a :"$cache_file" != :
        then
            for config_site in $CONFIG_SITE
            do
                if test -w "$config_site" -o ! -e "$config_site"
                then
                    cat "$cache_file" | egrep -v '^(ac_cv_env_|ac_cv_host|ac_cv_target|linux_cv_|ksyms_cv_|strconf_cv_|sctp_cv_|xns_cv_|lis_cv_|streams_cv_|xti_cv_|xopen_cv_|inet_cv_|xnet_cv_)' > "$config_site" 2>/dev/null
                fi
            done
        fi], [cache_file="$cache_file" ; CONFIG_SITE="$CONFIG_SITE"])
])# _OPENSS7_CACHE
# =============================================================================

# =============================================================================
# _OPENSS7
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7], [
])# _OPENSS7
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
# =============================================================================
