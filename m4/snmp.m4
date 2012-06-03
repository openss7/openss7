# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: snmp.m4,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2011-05-31 09:46:02 $
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
# Last Modified $Date: 2011-05-31 09:46:02 $ by $Author: brian $
#
# =============================================================================

# -----------------------------------------------------------------------------
# This file provides some common macros for finding a NET-SNMP release and
# necessary include directories, libraries, and other configuration for
# compiling and linking SNMP agents to run with the STREAMS package.
# -----------------------------------------------------------------------------
# Interesting enough, there is no need to have net-snmp loaded on the build
# machine to compile agents.  Only the proper header files are required.
# -----------------------------------------------------------------------------

# =============================================================================
# _SNMP
# -----------------------------------------------------------------------------
# Check for the existences of SNMP header files, particulary
# <ucd-snmp/ucd-snmp-config.h>.  SNMP header files are required for building
# SNMP agents.  Without SNMP header files, the SNMP agent will no be built.
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP], [dnl
    _SNMP_HEADERS
    _SNMP_LIBRARIES
])# _SNMP
# =============================================================================

AC_DEFUN([_SNMP_MSG_WARN],
    [AC_REQUIRE_SHELL_FN([snmp_msg_warn],
	[AS_FUNCTION_DESCRIBE([snmp_msg_warn],
	    [HEADER],
	    [Indicates that a header file is required for SNMP.])],
	[cat <<EOF

*** 
*** Configure cannot find the UCD-SNMP compatibility header:
***
***	<[$]1>
*** 
*** Perhaps you forgot to load the NET-SNMP development package (e.g.,
*** net-snmp-devel or libsnmp-dev).  SNMP programs cannot be built.  If
*** this is not what you want, load the development package and rerun
*** configure.  Use the following commands to obtain the NET-SNMP
*** development package:
***
*** Debian 5.0:  'aptitude install libsnmp-dev'
*** Ubuntu 8.04: 'aptitude install libsnmp-dev'
*** CentOS 5.x:  'yum install net-snmp-devel'
*** SLES 10/11:  'zypper install net-snmp-devel'
*** ArchLinux:   'pacman -S net-snmp'
*** RedHat 7.2:  'rpm -i ucd-snmp-devel-4.2.5-8.72.1'
*** RedHat 7.3:  'rpm -i ucd-snmp-devel-4.2.5-8.73.1'
***
*** Repeat after loading the correct package or by specifying the
*** configure argument --without-snmp: continuing under the assumption
*** that the option --without-snmp was intended.
***
EOF])dnl
    if test :"${with_snmp:-yes}" != :no ; then
	ac_msg=`snmp_msg_warn $1`
	AC_MSG_WARN([$ac_msg])
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_snmp --without-snmp\""
	PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-snmp'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-snmp'"
	with_snmp=no
    fi
])

# =============================================================================
# _SNMP_HEADERS
# -----------------------------------------------------------------------------
# Our agents use the older UCD-SNMP 4.2.6 interface including header_complex.
# The NET-SNMP releases of UCD-SNMP are missing some essential headers (used
# inside the package but not installed).  Recent versions of NET-SNMP such as
# 5.2.1 provide compatibility headers, but mangles some of them.  Certainly
# nobody uses these or never took the time to fix them.  So we do a bunch of
# checks here, and signal the agent code to include fixed headers shipped with
# the package.  Note that Mandriva stick the ucd-snmp directory under net-snmp,
# as in /usr/include/net-snmp/ucd-snmp so we need to add -I/usr/include/net-snmp
# to the CPPFLAGS before testing.
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_HEADERS], [dnl
    # allow the user to specify the header file location.
    AC_ARG_WITH([snmp],
	[AS_HELP_STRING([--with-snmp=HEADERS],
	    [SNMP header directory @<:@default=search@:>@])],
	[], [with_snmp=search])
    _BLD_FIND_DIR([snmp include directory], [snmp_cv_includedir], [
	    ${includedir}
	    ${rootdir}${oldincludedir}
	    ${rootdir}/usr/include
	    ${rootdir}/usr/local/include], [ucd-snmp/ucd-snmp-config.h], [no], [dnl
	if test ${with_snmp:-search} != no ; then
	    _BLD_INSTALL_WARN([UCD_SNMP_UCD_SNMP_CONFIG_H], [
*** 
*** Configure cannot find the UCD-SNMP compatability header:
*** 
***	<ucd-snmp/ucd-snmp-config.h>
***
*** Perhaps you need to load the NET-SNMP development package (e.g.,
*** net-snmp-devel or libsnmp-dev).  SNMP programs cannot be built
*** without this header.  If this is not what you want, load the
*** development package and rerun configure.  Try:
*** ], [
*** Debian 5.0:   'aptitude install libsnmp-dev'
*** Ubuntu 8.04:  'aptitude install libsnmp-dev'
*** CentOS 5.x:   'yum install net-snmp-devel'
*** SLES 10/11:   'zypper install net-snmp-devel'
*** ArchLinux:    'pacman -S net-snmp'
*** Slack 13.37:  './configure --without-snmp'
*** Salix 13.37:  './configure --without-snmp'
*** RedHat 7.2:   'rpm -i ucd-snmp-devel-4.2.5-8.72.1'
*** RedHat 7.3:   'rpm -i ucd-snmp-devel-4.2.5-8.73.1'], [
*** 
*** Repeat after loading the correct package or by specifying the
*** configure argument --without-snmp: continuing under the assumption
*** that the option --without-snmp was intended.
*** ])
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_snmp --without-snmp\""
	    PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-snmp'"
	    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-snmp'"
	    with_snmp=no
	fi], [], [with_snmp])
    if test ${snmp_cv_includedir:-no} = no ; then
	snmpincludedir=
    else
	snmpincludedir="$snmp_cv_includedir"
    fi
    AM_CONDITIONAL([WITH_SNMP], [test :"${with_snmp:-search}" != :no])
    AC_SUBST([snmpincludedir])dnl
    AC_CACHE_CHECK([for snmp cppflags], [snmp_cv_cppflags], [dnl
	if test -n "$snmpincludedir" ; then
	    snmp_cv_cppflags="-I${snmpincludedir}"
	else
	    snmp_cv_cppflags=
	fi
    ])
    SNMP_CPPFLAGS="$snmp_cv_cppflags"
    AC_SUBST([SNMP_CPPFLAGS])dnl
    snmp_save_CPPFLAGS="$CPPFLAGS"
    if test -n "$SNMP_CPPFLAGS" ; then
	eval "CPPFLAGS=\"${CPPFLAGS:+$CPPFLAGS }$SNMP_CPPFLAGS\""
    fi
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-config.h], [], [dnl
	_SNMP_MSG_WARN([ucd-snmp/ucd-snmp-config.h]) ])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-includes.h], [], [dnl
	_SNMP_MSG_WARN([ucd-snmp/ucd-snmp-includes.h]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
])
    AC_CHECK_HEADERS([ucd-snmp/ucd-snmp-agent-includes.h], [], [
	_SNMP_MSG_WARN([ucd-snmp/ucd-snmp-agent-includes.h]) ], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/callback.h ucd-snmp/snmp-tc.h ucd-snmp/default_store.h ucd-snmp/snmp_alarm.h],
	[], [], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
])
    AC_CHECK_HEADERS([ucd-snmp/ds_agent.h ucd-snmp/util_funcs.h ucd-snmp/header-complex.h ucd-snmp/mib_modules.h net-snmp/agent/mib_modules.h],
	[], [], [
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
#include <ucd-snmp/default_store.h>
#include <ucd-snmp/snmp_alarm.h>
])
    CPPFLAGS="$snmp_save_CPPFLAGS"
])# _SNMP_HEADERS
# =============================================================================

AC_DEFUN([_SNMP_LIB_WARN],
    [AC_REQUIRE_SHELL_FN([snmp_lib_warn],
	[AS_FUNCTION_DESCRIBE([snmp_lib_warn],
	    [LIBRARY],
	    [Indicates tha a library is required for SNMP.])],
	[cat <<EOF
***
*** It is unlikely that SNMP agents will compile without the supporting
*** [$]1 library.  Expect problems later.
***
EOF])dnl
    if test :"${with_snmp:-yes}" != :no
    then
	case $1 in
	    (libpopt|librpmio|librpm|librpmdb)
		if test ":$deb_cv_debs:$deb_cv_dscs" != :yes:yes
		then
		    ac_msg=`snmp_lib_warn $1`
		    AC_MSG_WARN([$ac_msg])
		fi
		;;
	    (*)
		ac_msg=`snmp_lib_warn $1`
		AC_MSG_WARN([$ac_msg])
		;;
	esac
    fi
])

AC_DEFUN([_SNMP_LIB_ERROR],
    [AC_REQUIRE_SHELL_FN([snmp_lib_error],
	[AS_FUNCTION_DESCRIBE([snmp_lib_error],
	    [LIBRARY],
	    [Indicates that SNMP agents require a missing library.])],
	[cat <<EOF
***
*** Compiling SNMP agents requires the library [$]1.  Most likely you
*** need to install the net-snmp or ucd-snmp runtime package or a
*** supporting library.  Otherwise, pass --without-snmp to configure.
*** Continuing assuming that option --without-snmp was intended.
***
EOF])dnl
    if test :"${with_snmp:-yes}" != :no ; then
	ac_msg=`snmp_lib_error $1`
	AC_MSG_WARN([$ac_msg])
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_snmp --without-snmp\""
	PACKAGE_DEPOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-snmp'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-snmp'"
	with_snmp=no
    fi
])

# =============================================================================
# _SNMP_LIBRARIES
# -----------------------------------------------------------------------------
AC_DEFUN([_SNMP_LIBRARIES], [dnl
    AC_REQUIRE([_PERL_LIBRARIES])
    AC_CACHE_CHECK([for snmp libs], [snmp_cv_libs], [dnl
	AC_MSG_RESULT([checking])
	snmp_save_LIBS="$LIBS"
	snmp_save_LDFLAGS="$LDFLAGS"
	LIBS="$perl_cv_ldadd"
	LDFLAGS="$perl_cv_ldflags"

	AC_CHECK_LIB_BROKEN([wrap], [main], [],
	    [_SNMP_LIB_WARN([libwrap])]) ###
	AC_CHECK_LIB([sensors], [main], [],
	    [_SNMP_LIB_WARN([libsensors])])
	AC_CHECK_LIB([m], [main], [],
	    [_SNMP_LIB_WARN([libm])])
	AC_CHECK_LIB([crypto], [main], [],
	    [_SNMP_LIB_WARN([libcrypto])])
	AC_CHECK_LIB([z], [main], [],
	    [_SNMP_LIB_WARN([libz])]) ###
	AC_CHECK_LIB([popt], [main], [],
	    [_SNMP_LIB_WARN([libpopt])]) ###
	AC_CHECK_LIB([rpmio], [main], [],
	    [_SNMP_LIB_WARN([librpmio])])
dnl
dnl Note that for old versions of RedHat (7.2), librpm depends on librpmdb which
dnl in turn depends upon librpm.  You can only load these libraries together or
dnl not at all.  That is why [-lrpmdb] is specified as the fifth argument to
dnl AC_CHECK_LIB_BROKEN.  For target systems that do not have this dependency,
dnl librpmdb is the next in the dependency order anyway.  This will only cause a
dnl problem when librpmdb is broken too.
dnl
	AC_CHECK_LIB([rpm], [main], [],
	    [_SNMP_LIB_WARN([librpm])], [-lrpmdb])
	AC_CHECK_LIB([rpmdb], [main], [],
	    [_SNMP_LIB_WARN([librpmdb])]) #xxx
	AC_CHECK_LIB([dl], [main], [],
	    [_SNMP_LIB_WARN([libdl])])

	snmp_cv_libs="$LIBS"
	snmp_cv_ldflags="$LDFLAGS"
	LIBS="$snmp_save_LIBS"
	LDFLAGS="$snmp_save_LDFLAGS"
	AC_MSG_CHECKING([for snmp libs])
    ])
    AC_CACHE_CHECK([for snmp ldadd], [snmp_cv_ldadd], [dnl
	AC_MSG_RESULT([checking])
	snmp_save_LIBS="$LIBS"
	snmp_save_LDFLAGS="$LDFLAGS"
	LIBS="$snmp_cv_libs"
	LDFLAGS="$snmp_cv_ldflags"

	AC_CHECK_LIB([netsnmp], [main])

	if test ":${ac_cv_lib_netsnmp_main:-no}" = :yes
	then
	    AC_CHECK_LIB([netsnmphelpers], [main], [],
		[_SNMP_LIB_ERROR([libnetsnmphelpers])], [-lnetsnmpagent -lnetsnmpmibs])
	    AC_CHECK_LIB([netsnmpagent], [main], [],
		[_SNMP_LIB_ERROR([libnetsnmpagent])], [-lnetsnmpmibs])
	    AC_CHECK_LIB([netsnmpmibs], [main], [],
		[_SNMP_LIB_ERROR([libnetsnmpmibs])])
	else
	    AC_CHECK_LIB([snmp], [main], [],
		[_SNMP_LIB_ERROR([libsnmp])])
	    AC_CHECK_LIB([ucdagent], [main], [],
		[_SNMP_LIB_ERROR([libucdagent])], [-lucdmibs], [
int allow_severity = 0;
int deny_severity = 0;
])
	    AC_CHECK_LIB([ucdmibs], [main], [],
		[_SNMP_LIB_ERROR([libucdmibs])], [], [
int allow_severity = 0;
int deny_severity = 0;
])
	fi

	snmp_cv_ldadd="$LIBS"
	snmp_cv_ldflags="$LDFLAGS"
	LIBS="$snmp_save_LIBS"
	LDFLAGS="$snmp_save_LDFLAGS"
	AC_MSG_CHECKING([for snmp ldadd])
    ])
    SNMP_LDADD="$snmp_cv_ldadd"
    AC_SUBST([SNMP_LDADD])dnl
    SNMP_LDFLAGS="$snmp_cv_ldflags"
    AC_SUBST([SNMP_LDFLAGS])dnl
])# _SNMP_LIBRARIES
# =============================================================================

# =============================================================================
#
# $Log: snmp.m4,v $
# Revision 1.1.2.7  2011-05-31 09:46:02  brian
# - new distros
#
# Revision 1.1.2.6  2011-05-10 13:45:35  brian
# - weak modules workup
#
# Revision 1.1.2.5  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.4  2010-11-28 13:55:51  brian
# - update build requirements, proper autoconf functions, build updates
#
# Revision 1.1.2.3  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.2  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.1  2009-06-21 11:06:05  brian
# - added files to new distro
#
# Revision 0.9.2.8  2008-09-03 07:13:53  brian
# - only warn about missing SNMP 32-bit libs
#
# Revision 0.9.2.7  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.6  2008/01/13 21:53:04  brian
# - dlmod SNMP agent build and installation
#
# Revision 0.9.2.5  2007/10/18 05:33:30  brian
# - better checking of NET-SNMP
#
# Revision 0.9.2.4  2007/10/15 06:47:49  brian
# - update to SNMP agent build
#
# Revision 0.9.2.3  2007/10/15 01:04:50  brian
# - updated SNMP build
#
# Revision 0.9.2.2  2007/10/13 08:51:41  brian
# - updates for 2.6.22 FC6 kernel and added MIBs
#
# Revision 0.9.2.1  2007/10/13 08:13:17  brian
# - added macros for SNMP agents
#
# =============================================================================
# 
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
