# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2013  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date$ by $Author$
#
# =============================================================================

# =============================================================================
# _SSL
# -----------------------------------------------------------------------------
AC_DEFUN([_SSL], [dnl
    AC_MSG_NOTICE([+------------------+])
    AC_MSG_NOTICE([| SSL Certificates |])
    AC_MSG_NOTICE([+------------------+])
    _SSL_OPTIONS
    _SSL_SETUP
    _SSL_OUTPUT
])# _SSL
# =============================================================================

# =============================================================================
# _SSL_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_SSL_OPTIONS], [dnl
    AC_ARG_WITH([ssl-cert],
	[AS_HELP_STRING([--with-ssl-directory=SSLDIR],
	    [OpenSSL certificate directory @<:@default=auto@:>@])])
])# _SSL_OPTIONS
# =============================================================================

# =============================================================================
# _SSL_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_SSL_SETUP], [dnl
    tmp_PATH="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
    _BLD_FIND_DIR([for ca-cert directory], [ssl_cv_ca_cert_dir], [
	    ${rootdir}/usr/share/ca-certificates], [], [no])
    _BLD_VAR_PATH_PROG([UCC], [update-ca-certificates], [$tmp_PATH],
	[update ca certs command @<:@default=update-ca-certificates@:>@])
    AC_MSG_CHECKING([for ca-certificates support])
    bld_tmp=no
    if test :"${UCC:-no}" != :no -a :"${ssl_cv_ca_cert_dir:-no}" != :no ; then
	bld_tmp=yes
    fi
    AC_MSG_RESULT([])
    _BLD_FIND_DIR([for ssl directory], [ssl_cv_ssl_directory], [
	    ${rootdir}/etc/pki/tls
	    ${rootdir}/etc/ssl
	    ${rootdir}/usr/lib/ssl
	    ${rootdir}/usr/share/ssl
	    ${rootdir}/usr/local/ssl], [], [], [dnl
	AC_MSG_ERROR([
*** 
*** Configure could not find the OpenSSL certificates directory.  The
*** locations searched were:
***	    "$bld_search_path"
***
*** Please specify the absolute location of the OpenSSL certificates
*** directory with option --with-ssl-directory before repeating.
*** ])
	    ], [], [with_ssl_directory], [-a -d $bld_dir/certs])
    _BLD_VAR_PATH_PROG([OPENSSL], [openssl], [$tmp_PATH],
	[openssl command @<:@default=openssl@:>@], [dnl
	_BLD_INSTALL_ERROR([OPENSSL], [
*** 
*** Configure cannot find a suitable 'openssl' program.  Installing CA
*** certificates requires the 'openssl' program from the 'openssl'
*** package on the target host.  The 'openssl' package has been available
*** for may years on all distributions and is available everywhere.  Try:
*** ], [
*** Debian:    'aptitude install openssl'
*** SuSE:      'zypper install openssl'
*** CentOS:    'yum install openssl'
*** Mandriva:  'urpmi openssl'
*** Archlinux: 'yaourt -S openssl'], [
***
*** To get rid of this error, load the 'openssl' package, or specify the
*** location with the OPENSSL environment variable to 'configure'.
*** ])])
])# _SSL_SETUP
# =============================================================================

# =============================================================================
# _SSL_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_SSL_OUTPUT], [dnl
    ssldir="${ssl_cv_ssl_directory}"
    sslcertdir="$ssldir/certs"
    AC_SUBST([ssldir])
    AC_SUBST([sslcertdir])
    cacertdir="${ssl_cv_ca_cert_dir}"
    lcacertdir=`echo "${ssl_cv_ca_cert_dir:-no}" | sed 's,/share/,/local/share/,'`
    AC_SUBST([cacertdir])
    AC_SUBST([lcacertdir])
    AM_CONDITIONAL([WITH_UPDATE_CA_CERTS], [test :"${UCC:-no}" != :no -a :"${ssl_cv_ca_cert_dir:-no}" != :no])dnl
])# _SSL_OUTPUT
# =============================================================================

# =============================================================================
# _SSL_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_SSL_XXX], [dnl
])# _SSL_XXX
# =============================================================================


# =============================================================================
#
# $Log$
# =============================================================================
# 
# Copyright (c) 2008-2013  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
