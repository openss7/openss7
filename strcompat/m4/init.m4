dnl =========================================================================
dnl
dnl @(#) $Id: init.m4,v 0.9.2.3 2005/02/17 08:05:49 brian Exp $
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
dnl Last Modified $Date: 2005/02/17 08:05:49 $ by $Author: brian $
dnl 
dnl =========================================================================

# ===========================================================================
# _INIT_SCRIPTS
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS], [dnl
    _INIT_SCRIPTS_OPTIONS
    _INIT_SCRIPTS_SETUP
    _INIT_SCRIPTS_OUTPUT
])# _INIT_SCRIPTS
# ===========================================================================

# ===========================================================================
# _INIT_SCRIPTS_OPTIONS
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_OPTIONS], [dnl
    AC_ARG_ENABLE([initscripts],
        AS_HELP_STRING([--disable-initscripts],
            [disable installation of init scripts. @<:@default=enabled@:>@]),
        [enable_initscripts="$enableval"],
        [enable_initscripts='yes'])
])# _INIT_SCRIPTS_OPTIONS
# ===========================================================================

# ===========================================================================
# _INIT_SCRIPTS_SETUP
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_SETUP], [dnl
dnl
dnl first find the inittab: we can always figure out the initial
dnl init script from the inittab
dnl
    AC_CACHE_CHECK([for init SysV inittab], [init_cv_inittab], [dnl
        init_cv_inittab='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/inittab\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -f $init_tmp ; then
                init_cv_inittab="$init_tmp"
                break
            fi
        done
    ])
dnl 
dnl next check for the initial init script from inittab
dnl 
    AC_CACHE_CHECK([for init SysV script], [init_cv_script], [dnl
        init_cv_script='no'
        if test ":$init_cv_inittab" != :no ; then
            init_tmp="$(< $init_cv_inittab | grep -c1 '^si::sysinit:' | sed -e 's|^si::sysinit:||;s|[[[:space:]]].*||')"
            init_tmp=`echo "${DESTDIR}${rootdir}$init_tmp" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            if test -f "$init_tmp" ; then
                init_cv_script="$init_tmp"
            fi
        fi
dnl
dnl     fallback is to go looking for it in the usual places
dnl
        if test ":${init_cv_script:-no}" = :no ; then
            eval "init_search_path=\"
                ${DESTDIR}${sysconfdir}/init.d/rcS
                ${DESTDIR}${sysconfdir}/rc.d/rc.sysinit\""
            init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
            for init_tmp in $init_search_path ; do
                if test -f $init_tmp ; then
                    init_cv_script="$init_tmp"
                    break
                fi
            done
        fi
    ])
    AC_CACHE_CHECK([for init SysV rcS.d directory], [init_cv_rcs_dir], [dnl
        init_cv_rcs_dir='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/rcS.d
            ${sysconfdir}/rcS.d
            /etc/rcS.d\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -d $init_tmp ; then
                init_cv_rcs_dir="$init_tmp"
                break
            fi
        done
    ])
    AC_CACHE_CHECK([for init SysV rc.d directory], [init_cv_rc_dir], [dnl
        init_cv_rc_dir='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/rc.d\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -d $init_tmp ; then
                init_cv_rc_dir="$init_tmp"
                break
            fi
        done
    ])
dnl
dnl This script needs to exist to add to /etc/modules successfully
dnl
    AC_CACHE_CHECK([for init SysV rc.modules script], [init_cv_rc_modules], [dnl
        init_cv_rc_modules='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/rc.d/rc.modules\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -f $init_tmp ; then
                init_cv_rc_modules="$init_tmp"
                break
            fi
        done
    ])
dnl
dnl This is where we are going to add preloaded modules
dnl
    AC_CACHE_CHECK([for init SysV modules file], [init_cv_modules], [dnl
        init_cv_modules='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/modules\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -f $init_tmp ; then
                init_cv_modules="$init_tmp"
                break
            fi
        done
    ])
dnl
dnl This is where we are going to have to generate symbolic links if chkconfig
dnl does not exist
dnl
    AC_CACHE_CHECK([for init SysV rcX.d directory], [init_cv_rcx_dir], [dnl
        init_cv_rcx_dir='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/rc.d/rc[[S0-6]].d
            ${DESTDIR}${sysconfdir}/rc[[S0-6]].d\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -d $init_tmp ; then
                init_cv_rcx_dir="$init_tmp"
                break
            fi
        done
    ])
    AC_ARG_VAR([CHKCONFIG], [Chkconfig command])
    AC_PATH_TOOL([CHKCONFIG], [chkconfig], [], [$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin])
dnl
dnl initrddir is where we are going to put init scripts
dnl
    AC_CACHE_CHECK([for init SysV init.d directory], [init_cv_initrddir], [dnl
        init_cv_initrddir='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/rc.d/init.d
            ${DESTDIR}${sysconfdir}/init.d\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -d $init_tmp -a ! -L $init_tmp ; then
                init_cv_initrddir="$init_tmp"
                break
            fi
        done
    ])
dnl
dnl configdir is where we are going to put init script default files
dnl
    AC_CACHE_CHECK([for init SysV config directory], [init_cv_configdir], [dnl
        init_cv_configdir='no'
        eval "init_search_path=\"
            ${DESTDIR}${sysconfdir}/sysconfig
            ${DESTDIR}${sysconfdir}/default\""
        init_search_path=`echo "$init_search_path" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        for init_tmp in $init_search_path ; do
            if test -d $init_tmp ; then
                init_cv_configdir="$init_tmp"
                break
            fi
        done
        if test :${init_cv_configdir:-no} = :no ; then
            if test :${init_cv_rcs_dir:-no} = :no ; then
dnl             redhat style
                init_cv_configdir="${DESTDIR}/${sysconfdir}/sysconfig"
            else
dnl             debian style
                init_cv_configdir="${DESTDIR}/${sysconfdir}/default"
            fi
        fi
    ])
    AC_CACHE_CHECK([for init SysV installation], [init_cv_install], [dnl
        init_cv_install='yes'
        test ":${enable_initscripts:-yes}" = :no && init_cv_install='no'
    ])
])# _INIT_SCRIPTS_SETUP
# ===========================================================================

# ===========================================================================
# _INIT_SCRIPTS_OUTPUT
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_INIT_SCRIPTS_OUTPUT], [dnl
    AM_CONDITIONAL([INSTALL_INITSCRIPTS], [test :"${init_cv_install:-yes}" = :yes])dnl
    AM_CONDITIONAL([WITH_RCSD_DIRECTORY], [test :${init_cv_rcs_dir:-no} != :no])dnl
dnl
dnl initrddir is where we are going to put init scripts
dnl
    if test :"${init_cv_initrddir:-no}" != :no ; then
        init_tmp=`echo "${DESTDIR}" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        initrddir="${init_cv_initrddir#$init_tmp}"
    else
        if test :${init_cv_rcs_dir:-no} = :no ; then
dnl         redhat style
            initrddir="${sysconfdir}/rc.d/init.d"
        else
dnl         debian style
            initrddir="${sysconfdir}/init.d"
        fi
    fi
    AC_SUBST([initrddir])
dnl
dnl configdir is where we are going to put init script default files
dnl
    if test :"$init_cv_configdir:-no}" != :no ; then
        init_tmp=`echo "${DESTDIR}" | sed -e 's|\<NONE\>||g;s|//|/|g'`
        configdir="${init_cv_configdir#$init_dir}"
    else
        if test :${init_cv_rcs_dir:-no} = :no ; then
dnl         redhat style
            configdir="${sysconfdir}/sysconfig"
        else
dnl         debian style
            configdir="${sysconfdir}/default"
        fi
    fi
    AC_SUBST([configdir])
])# _INIT_SCRIPTS_OUTPUT
# ===========================================================================

# ===========================================================================
# _INIT_
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
AC_DEFUN([_INIT_], [dnl
])# _INIT_
# ===========================================================================


dnl =========================================================================
dnl 
dnl Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl 
dnl =========================================================================
dnl ENDING OF SEPARATE COPYRIGHT MATERIAL vim: ft=config sw=4 et
dnl =========================================================================
