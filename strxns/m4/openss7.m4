dnl =============================================================================
dnl BEGINNING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =============================================================================
dnl 
dnl @(#) $RCSfile: openss7.m4,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/01/15 22:39:18 $
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl
dnl All Rights Reserved.
dnl
dnl This program is free software; you can redistribute it and/or modify it under
dnl the terms of the GNU General Public License as published by the Free Software
dnl Foundation; either version 2 of the License, or (at your option) any later
dnl version.
dnl
dnl This program is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
dnl FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
dnl details.
dnl
dnl You should have received a copy of the GNU General Public License along with
dnl this program; if not, write to the Free Software Foundation, Inc., 675 Mass
dnl Ave, Cambridge, MA 02139, USA.
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions apply
dnl to you.  If the Software is supplied by the Department of Defense ("DoD"), it
dnl is classified as "Commercial Computer Software" under paragraph 252.227-7014
dnl of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
dnl successor regulations) and the Government is acquiring only the license rights
dnl granted herein (the license rights customarily provided to non-Government
dnl users).  If the Software is supplied to any unit or agency of the Government
dnl other than DoD, it is classified as "Restricted Computer Software" and the
dnl Government's rights in the Software are defined in paragraph 52.227-19 of the
dnl Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
dnl the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Commercial licensing and support of this software is available from OpenSS7
dnl Corporation at a fee.  See http://www.openss7.com/
dnl
dnl -----------------------------------------------------------------------------
dnl
dnl Last Modified $Date: 2005/01/15 22:39:18 $ by $Author: brian $
dnl
dnl -----------------------------------------------------------------------------
dnl $Log: openss7.m4,v $
dnl Revision 0.9.2.8  2005/01/15 22:39:18  brian
dnl - Export necessary variables to check scripts.
dnl
dnl Revision 0.9.2.7  2005/01/15 19:34:37  brian
dnl - Working up check and installcheck testing.
dnl
dnl Revision 0.9.2.6  2005/01/14 06:38:47  brian
dnl - Updated copyright headers and comments.
dnl
dnl Revision 0.9.2.5  2005/01/14 00:28:43  brian
dnl - Warn all and turn warnings into errors in maintainer mode.
dnl
dnl Revision 0.9.2.4  2005/01/10 10:24:53  brian
dnl - Correct genksyms cache clash.
dnl
dnl Revision 0.9.2.3  2004/12/21 12:22:06  brian
dnl - Added automatic caching to all packages.
dnl
dnl Revision 0.9.2.2  2004/08/17 11:41:24  brian
dnl - Added package description macros.
dnl
dnl =============================================================================

# =============================================================================
# _OPENSS7_PACKAGE([SHORT-TITLE], [LONG-TITLE])
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_PACKAGE], [dnl
    PACKAGE_TITLE='$2'
    AC_SUBST([PACKAGE_TITLE])dnl
    PACKAGE_SHORTTITLE='$1'
    AC_SUBST([PACKAGE_SHORTTITLE])dnl
    _OPENSS7_CACHE
    _OPENSS7_DEBUG
    AC_SUBST([cross_compiling])dnl
])# _OPENSS7_PACKAGE
# =============================================================================

# =============================================================================
# _OPENSS7_CACHE
# -----------------------------------------------------------------------------
# A little trick with caches and site files.
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_CACHE], [dnl
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
# _OPENSS7_DEBUG
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7_DEBUG], [dnl
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
        CFLAGS=`echo "$CFLAGS" | sed -e 's|-Wall||;s|  | |g'`
        CFLAGS=`echo "$CFLAGS" | sed -e 's|-Werror||;s|  | |g'`
        CFLAGS=`echo "$CFLAGS" | sed -e 's|-Wundef||;s|  | |g'`
        CFLAGS=`echo "$CFLAGS" | sed -e 's|^  *||;s|  *$||;s|    | |g;s|   | |g;s|  | |g'`
        CFLAGS="${CFLAGS}${CFLAGS:+ }-Wall -Wstrict-prototypes -Wno-trigraphs -Wundef -Werror"
    fi
])# _OPENSS7_DEBUG
# =============================================================================

# =============================================================================
# _OPENSS7
# -----------------------------------------------------------------------------
AC_DEFUN([_OPENSS7], [dnl
])# _OPENSS7
# =============================================================================

dnl =============================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =============================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =============================================================================
