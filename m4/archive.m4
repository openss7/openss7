# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: archive.m4,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2009-07-21 11:06:12 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date: 2009-07-21 11:06:12 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# _ARCHIVE
# -----------------------------------------------------------------------------
AC_DEFUN([_ARCHIVE], [dnl
    _ARCHIVE_ARGS
    _ARCHIVE_SETUP
    _ARCHIVE_OPTIONS
    _ARCHIVE_OUTPUT
])# _ARCHIVE
# =============================================================================

# =============================================================================
# _ARCHIVE_ARGS
# -----------------------------------------------------------------------------
AC_DEFUN([_ARCHIVE_ARGS], [dnl
    AC_ARG_WITH([lzma],
	[AS_HELP_STRING([--with-lzma@<:@=LZMA_CMD@:>@],
	    [create lzma archives @<:@default=no@:>@])], [dnl
	    case "$with_lzma" in
		(no|yes) ;;
		(*) LZMA_CMD="$with_lzma" ; with_lzma=yes ;;
	    esac], [with_lzma=no])
    AC_ARG_WITH([xz],
	[AS_HELP_STRING([--with-xz@<:@=XZ_CMD@:>@],
	    [create xz archives @<:@default=no@:>@])], [dnl
	    case "$with_xz" in
		(no|yes) ;;
		(*) XZ_CMD="$with_xz" ; with_xz=yes ;;
	    esac], [with_xz=no])
    AC_ARG_ENABLE([bestzip],
	[AS_HELP_STRING([--enable-bestzip],
	    [best compression of archives @<:@default=bzip2@:>@])], [dnl
	    case "$enable_bestzip" in
		(xz|lzma|bzip2) disable_bestzip=no ;;
		(no) enable_bestzip=bzip2 ; disable_bestzip=yes;;
		(yes|*) enable_bestzip= ; disable_bestzip=no ;;
	    esac], [enable_bestzip=bzip2 ; disable_bestzip=yes])
    AC_ARG_ENABLE([repo-tar],
	[AS_HELP_STRING([--disable-repo-tar],
	    [tar repo construction @<:@default=yes@:>@])],
	[], [enable_repo_tar=yes])
    AC_SUBST([BESTZIP], [bzip2])
    AC_ARG_VAR([GZIP],      [Gzip default compression options @<:@default=-f9v@:>@])
    AC_ARG_VAR([GZIP_CMD],  [Gzip compression command @<:@default=gzip@:>@])
    AC_ARG_VAR([BZIP2],     [Bzip2 default compression options @<:@default=-f9v@:>@])
    AC_ARG_VAR([BZIP2_CMD], [Bzip2 compression command @<:@default=bzip2@:>@])
    AC_ARG_VAR([LZMA],      [Lzma default compression options @<:@default=-f9v@:>@])
    AC_ARG_VAR([LZMA_CMD],  [Lzma compression command @<:@default=lzma@:>@])
    AC_ARG_VAR([XZ],        [Xz default compression options @<:@default=-f9v@:>@])
    AC_ARG_VAR([XZ_CMD],    [Xz compression command @<:@default=xz@:>@])
    AC_ARG_VAR([MD5SUM],    [MD5 sum command @<:@default=md5sum@:>@])
    AC_ARG_VAR([SHA1SUM],   [SHA1 sum command @<:@default=sha1sum@:>@])
    AC_ARG_VAR([SHA256SUM], [SHA256 sum command @<:@default=sha256sum@:>@])
])# _ARCHIVE_ARGS
# =============================================================================

# =============================================================================
# _ARCHIVE_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_ARCHIVE_SETUP], [dnl
    AC_REQUIRE([_OPENSS7_MISSING3])
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin:$am_aux_dir"
    test -n "$GZIP" || GZIP='-f9v'
    AC_PATH_PROG([GZIP_CMD], [gzip], [], [$tmp_path])
    if test :"${GZIP_CMD:-no}" = :no ; then
	GZIP_CMD=
	AC_MSG_ERROR([
***
*** Configure cannot find a suitable 'gzip' program.  Creating gzip
*** archives requires the 'gzip' program from the 'gzip' package on the
*** build host.  The 'gzip' package has been available for many years on
*** all distributions and is available from any GNU archive site.  Try:
***
*** Debian:  'apt-get install gzip'
*** SuSE:    'zypper install gzip'
*** CentOS:  'yum install gzip'
***
*** To get rid of this error, load the 'gzip' package, or specify the
*** location with the GZIP_CMD environment variable to 'configure'.
*** ])
    fi
    test -n "$BZIP2" || BZIP2='-f9v'
    AC_PATH_PROG([BZIP2_CMD], [bzip2], [], [$tmp_path])
    if test :"${BZIP2_CMD:-no}" = :no ; then
	BZIP_CMD=
	AC_MSG_ERROR([
***
*** Configure cannot find a suitable 'bzip2' program.  Creating bzip2
*** archives requires the 'bzip2' program from the 'bzip2' package on
*** the build host.  The 'bzip2' package has been available for many
*** years on all distributions.  Try:
***
*** Debian:  'apt-get install bzip2'
*** SuSE:    'zypper install bzip2'
*** CentOS:  'yum install bzip2'
***
*** To get rid of this error, load the 'bzip2' package, or specify the
*** location with the BZIP2_CMD environment variable to 'configure'.
*** ])
    fi
    test -n "$LZMA" || LZMA='-f9v'
    AC_PATH_PROG([LZMA_CMD], [lzma], [], [$tmp_path])
    if test :"${LZMA_CMD:-no}" = :no ; then
	LZMZ_CMD=
	if test :"${disable_bestzip:-yes}" != :yes -a :"$enable_bestzip" = :lzma
	then
	    AC_MSG_WARN([
***
*** Configure cannot find the 'lzma' program, which is sometimes present
*** on recent systems.  Try:
***
*** Debian:  'apt-get install lzma'
*** SuSE:    'zypper install lzma'
*** CentOS:  'yum install lzma'
***
*** To get rid of this warning, load the 'lzma' package, or specify the
*** full path to the utility in the LZMA_CMD environment variable, or
*** simply do not specify --enable-bestzip='lzma' to configure.
*** ])
	fi
    else
	if test :"${disable_bestzip:-yes}" != :yes ; then
	    case "${enable_bestzip:-lzma}" in
		(bzip2) ;;
		(*) BESTZIP=lzma ;;
	    esac
	fi
    fi
    test -n "$XZ" || XZ='-f9v'
    AC_PATH_PROG([XZ_CMD], [xz], [], [$tmp_path])
    if test :"${XZ_CMD:-no}" = :no ; then
	XZ_CMD=
	if test :"${disable_bestzip:-yes}" != :yes -a :"$enable_bestzip" = :xz
	then
	    AC_MSG_WARN([
***
*** Configure cannot find the 'xz' program, which is sometimes present
*** on recent systems.  Try:
***
***   #> wget http://tukaani.org/xz/xz-4.999.8beta.tar.gz
***   #> tar xzf xz-4.999.8beta.tar.gz
***   #> cd xz-4.999.8beta
***   #> ./configure
***   #> make
***   #> sudo make install
***
*** To get rid of this warning, load the 'xz' package, or specify the
*** full path to the utility in the XZ_CMD environment variable, or
*** simply do not specify --enable-bestzip='xz' to configure.
*** ])
	fi
    else
	if test :"${disable_bestzip:-yes}" != :yes ; then
	    case "${enable_bestzip:-xz}" in
		(bzip2|lzma) ;;
		(*) BESTZIP=xz ;;
	    esac
	fi
    fi
    enable_bestzip="$BESTZIP"
    disable_repo_tar=
    AC_PATH_PROG([MD5SUM], [md5sum], [], [$tmp_path])
    if test :"${MD5SUM:-no}" = :no ; then
	MD5SUM="${am_missing3_run}md5sum"
	if test :$enable_repo_tar = :yes ; then
	    disable_repo_tar=yes
	    AC_MSG_ERROR([
*** 
*** Configure could not find a suitable 'md5sum' program.  Installing
*** tarball repositories requires the 'md5sum' program from the
*** 'coreutils' package on the install host.  The 'coreutils' package is
*** always installed: this script would not run without it.  It is
*** likely that your distribution has failed to install the 'md5sum'
*** program.  You can get the 'coreutils' pcakage from any GNU archive
*** site.  For example:
***
***   #> wget http://ftp.gnu.org/gnu/coreutils/coreutils-7.4.tar.gz
***   #> tar xzf coreutils-7.4.tar.gz
***   #> cd coreutils-7.4
***   #> ./configure --enable-install-program=md5sum
***   #> make
***   #> sudo make install
***
*** To get rid of this warning, load the 'md5sum' program from the
*** 'coreutils' package, specify the appropriate program with the MD5SUM
*** environment variable to 'configure', or specify --disable-repo-tar
*** to 'configure'.
*** ])
	fi
    fi
    AC_PATH_PROG([SHA1SUM], [sha1sum], [], [$tmp_path])
    if test :"${SHA1SUM:-no}" = :no ; then
	SHA1SUM="${am_missing3_run}sha1sum"
	if test :$enable_repo_tar = :yes ; then
	    disable_repo_tar=yes
	    AC_MSG_WARN([
*** 
*** Configure could not find a suitable 'sha1sum' program.  Installing
*** tarball repositories requires the 'sha1sum' program from the
*** 'coreutils' package on the install host.  The 'coreutils' package is
*** always installed: this script would not run without it.  It is
*** likely that your distribution has failed to install the 'sha1sum'
*** program.  You can get the 'coreutils' package from any GNU archive
*** site.  For example:
***
***   #> wget http://ftp.gnu.org/gnu/coreutils/coreutils-7.4.tar.gz
***   #> tar xzf coreutils-7.4.tar.gz
***   #> cd coreutils-7.4
***   #> ./configure --enable-install-program=sha1sum
***   #> make
***   #> sudo make install
***
*** To get rid of this warning, load the 'sha1sum' program from the
*** 'coreutils' package, specify the appropriate program with the
*** SHA1SUM environment variable to 'configure', or specify
*** --disable-repo-tar to 'configure'.
*** ])
	fi
    fi
    AC_PATH_PROG([SHA256SUM], [sha256sum], [], [$tmp_path])
    if test :"${SHA256SUM:-no}" = :no ; then
	SHA256SUM="${am_missing3_run}sha256sum"
	if test :$enable_repo_tar = :yes ; then
	    disable_repo_tar=yes
	    AC_MSG_WARN([
*** 
*** Configure could not find a suitable 'sha256sum' program.  Installing
*** tarball repositories requires the 'sha256sum' program from the
*** 'coreutils' package on the install host.  The 'coreutils' package is
*** always installed: this script would not run without it.  It is
*** likely that your distribution has failed to install the 'sha256sum'
*** program.  You can get the 'coreutils' package from any GNU archive
*** site.  For example:
***
***   #> wget http://ftp.gnu.org/gnu/coreutils/coreutils-7.4.tar.gz
***   #> tar xzf coreutils-7.4.tar.gz
***   #> cd coreutils-7.4
***   #> ./configure --enable-install-program=sha256sum
***   #> make
***   #> sudo make install
***
*** To get rid of this warning, load the 'sha256sum' program from the
*** 'coreutils' package, specify the appropriate program with the
*** SHA1SUM environment variable to 'configure', or specify
*** --disable-repo-tar to 'configure'.
*** ])
	fi
    fi
])# _ARCHIVE_SETUP
# =============================================================================

# =============================================================================
# _ARCHIVE_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_ARCHIVE_OPTIONS], [dnl
    AC_MSG_CHECKING([for build best compression for tarballs])
    AC_MSG_RESULT([${enable_bestzip:-bzip2}])
    AC_MSG_CHECKING([whether to build repository tarballs])
    if test :"$disable_repo_tar" = :yes ; then
	enable_repo_tar=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_repo_tar --disable-repo-tar\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-repo-tar'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-repo-tar'"
    fi
    AC_MSG_RESULT([${enable_repo_tar:-yes}])
])# _ARCHIVE_OPTIONS
# =============================================================================

# =============================================================================
# _ARCHIVE_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_ARCHIVE_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_BZIP2], [test :"${enable_bestzip:-bzip2}" = :bzip2])dnl
    AM_CONDITIONAL([WITH_LZMA], [test :"${enable_bestzip:-bzip2}" = :lzma])dnl
    AM_CONDITIONAL([WITH_XZ], [test :"${enable_bestzip:-bzip2}" = :xz])dnl
    AM_CONDITIONAL([BUILD_REPO_TAR], [test :"${enable_repo_tar:-yes}" != :no])dnl
])# _ARCHIVE_OUTPUT
# =============================================================================

# =============================================================================
# _ARCHIVE_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_ARCHIVE_XXX], [dnl
])# _ARCHIVE_XXX
# =============================================================================


# =============================================================================
#
# $Log: archive.m4,v $
# Revision 1.1.2.3  2009-07-21 11:06:12  brian
# - changes from release build
#
# Revision 1.1.2.2  2009-07-04 03:51:32  brian
# - updates for release
#
# Revision 1.1.2.1  2009-06-29 07:35:53  brian
# - added new macros for docs and java
#
# =============================================================================
# 
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn

