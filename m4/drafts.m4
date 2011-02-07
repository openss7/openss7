# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: drafts.m4,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2011-02-07 04:48:32 $
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
# Last Modified $Date: 2011-02-07 04:48:32 $ by $Author: brian $
#
# =============================================================================

#
# These autoconf macros provide for locating and managing the programs on the
# build system that are necessary  to create ME ROFF Internet-Drafts.  Normally,
# GROFF, that is part of the GROFF distribution, can be just present.  These
# macros check for the additional tools necessary for building print-formatted
# internet drafts.  They also manage the corresponding am/drafts.am makefile
# fragment.
#

# =============================================================================
# _DRAFTS
# -----------------------------------------------------------------------------
AC_DEFUN([_DRAFTS], [dnl
    AC_MSG_NOTICE([+------------------------------+])
    AC_MSG_NOTICE([| IETF Draft Generation Checks |])
    AC_MSG_NOTICE([+------------------------------+])
    _DRAFTS_ARGS
    _DRAFTS_SETUP
    _DRAFTS_OPTIONS
    _DRAFTS_OUTPUT
]) # _DRAFTS
# =============================================================================

# =============================================================================
# _DRAFTS_ARGS
# -----------------------------------------------------------------------------
AC_DEFUN([_DRAFTS_ARGS], [dnl
    AC_ARG_ENABLE([drafts],
	[AS_HELP_STRING([--disable-drafts],
	    [text formatted internet drafts @<:@default=auto@:>@])],
	[], [enable_drafts=yes])
    AC_ARG_ENABLE([drafts-html],
	[AS_HELP_STRING([--enable-drafts-html],
	    [html formatted internet drafts @<:@default=auto@:>@])],
	[], [enable_drafts_html=no])
    AC_ARG_ENABLE([drafts-print],
	[AS_HELP_STRING([--disable-drafts-print],
	    [print formatted internet drafts @<:@default=auto@:>@])],
	[], [enable_drafts_print=yes])
    AC_ARG_VAR([SOELIM],  [Roff source elimination command. @<:@default=gsoelim,soelim@:>@])
    AC_ARG_VAR([REFER],   [Roff references command. @<:@default=grefer,refer@:>@])
    AC_ARG_VAR([PIC],     [Roff picture command. @<:@default=gpic,pic@:>@])
    AC_ARG_VAR([TBL],     [Roff table command. @<:@default=gtbl,tbl@:>@])
dnl AC_ARG_VAR([NROFF],   [Roff text formatting command.  @<:@default=nroff@:>@])
    AC_ARG_VAR([GROFF],   [Roff formatting command.  @<:@default=groff@:>@])
    AC_ARG_VAR([PS2PDF],  [PS to PDF conversion command @<:@default=ps2pdf12@:>@])
    AC_ARG_VAR([FIG2DEV], [Fig to graphics format command. @<:@default=fig2dev@:>@])
    AC_ARG_VAR([DVIPDF],  [DVI to PDF conversion command @<:@default=dvipdf@:>@])
]) # _DRAFTS_ARGS
# =============================================================================

# =============================================================================
# _DRAFTS_SETUP
# -----------------------------------------------------------------------------
# Look for tools necessary to build draft documentation.  Uses the missing2
# script from the aux directory and requires that _OPENSS7_MISSING2 has already
# run to define it.  Some of these line up with manual page requirements, but
# they will then simply be cached at this point.  The tools are:
#
#   $SOELIM - absolute requirement
#   $REFER - absolute requirement
#   $PIC - absolute requirement
#   $TBL - absolute requirement
#   $NROFF - required for txt but GROFF can be used instead
#   $GROFF - required for dvi, ps and pdf formats (html too but broken)
#   $PS2PDF - required for pdf format, but DVIPDF can be used instead
#   $DVIPDF - required for pdf format when PS2PDF unavailable.
#   $FIG2DEV - required for dvi, ps, pdf and html formats.
#
# -----------------------------------------------------------------------------
AC_DEFUN([_DRAFTS_SETUP], [dnl
    AC_REQUIRE([_OPENSS7_MISSING2])dnl
    if test :$enable_drafts = :no ; then
	enable_drafts_print=no
    fi
    disable_drafts=
    disable_drafts_html=
    disable_drafts_print=
    tmp_path="${PATH:+$PATH:}/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:$am_aux_dir";
    _BLD_PATH_PROGS([SOELIM], [gsoelim soelim], [/bin/cat], [$tmp_path], [dnl
	if test :$enable_drafts != :no ; then
	    disable_drafts=yes
	    _BLD_INSTALL_WARN([SOELIM], [
***
*** Configure cannot find a suitable 'soelim' program.  Generating TXT,
*** PS and DVI formatted drafts requires the 'soelim' program from the
*** 'groff' package.  You can normally get 'groff' as part of most
*** poplular Linux distributions and all current versions are
*** acceptable.  The 'groff' package has been available for many years
*** and is available on the web from any GNU archive site.  Use the
*** following commands to obtain 'groff':
*** ], [
*** Debian 5.0:  'apt-get install groff-base'
*** Ubuntu 8.04: 'apt-get install groff'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the SOELIM environment variable to
*** 'configure', or specify the --disable-drafts option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([REFER], [grefer refer], [/bin/cat], [$tmp_path], [dnl
	if test :$enable_drafts != :no ; then
	    disable_drafts=yes
	    _BLD_INSTALL_WARN([REFER], [
***
*** Configure cannot find a suitable 'refer' program.  Generating TXT,
*** PS and DVI formatted drafts requires the 'refer' program from the
*** 'groff' package.  You can normally get 'groff' as part of most
*** poplular Linux distributions and all current versions are
*** acceptable.  The 'groff' package has been available for many years
*** and is available on the web from any GNU archive site.  Use the
*** following commands to obtain 'groff':
*** ], [
*** Debian 5.0:  'apt-get install groff'
*** Ubuntu 8.04: 'apt-get install groff_ext'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the REFER environment variable to
*** 'configure', or specify the --disable-drafts option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([PIC], [gpic pic], [/bin/cat], [$tmp_path], [dnl
	if test :$enable_drafts != :no ; then
	    disable_drafts=yes
	    _BLD_INSTALL_WARN([PIC], [
***
*** Configure cannot find a suitable 'pic' program.  Generating TXT, PS
*** and DVI formatted drafts requires the 'pic' program from the 'groff'
*** package.  You can normally get 'groff' as part of most poplular
*** Linux distributions and all current versions are acceptable.  The
*** 'groff' package has been available for many years and is available
*** on the web from any GNU archive site.  Use the following commands to
*** obtain 'groff':
*** ], [
*** Debian 5.0:  'apt-get install groff-base'
*** Ubuntu 8.04: 'apt-get install groff'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the PIC environment variable to
*** 'configure', or specify the --disable-drafts option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([TBL], [gtbl tbl], [/bin/cat], [$tmp_path], [dnl
	if test :$enable_drafts != :no ; then
	    disable_drafts=yes
	    _BLD_INSTALL_WARN([TBL], [
***
*** Configure cannot find a suitable 'tbl' program.  Generating TXT, PS
*** and DVI formatted drafts requires the 'tbl' program from the 'groff'
*** package.  You can normally get 'groff' as part of most poplular
*** Linux distributions and all current versions are acceptable.  The
*** 'groff' package has been available for many years and is available
*** on the web from any GNU archive site.  Use the following commands to
*** obtain 'groff':
*** ], [
*** Debian 5.0:  'apt-get install groff-base'
*** Ubuntu 8.04: 'apt-get install groff'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the TBL environment variable to
*** 'configure', or specify the --disable-drafts option to 'configure'.
*** ])
	fi])
dnl    _BLD_PATH_PROG([NROFF], [nroff], [${am_missing5_run}nroff], [$tmp_path], [dnl
dnl	if test :$enable_drafts != :no ; then
dnl	    : # will attempt to emulate with groff
dnl	fi])
    _BLD_PATH_PROG([GROFF], [groff], [${am_missing5_run}groff],
		 [$tmp_path], [dnl
	if test :$enable_drafts != :no ; then
	    disable_drafts=yes
	    _BLD_INSTALL_WARN([GROFF], [
***
*** Configure cannot find a suitable 'groff' program.  Generating TXT,
*** PS and DVI formatted drafts requires the 'groff' program from the
*** 'groff' package.  You can normally get 'groff' as part of most
*** poplular Linux distributions and all current versions are
*** acceptable.  The 'groff' package has been available for many years
*** and is available on the web from any GNU archive site.  Use the
*** following commands to obtain 'groff':
*** ], [
*** Debian 5.0:  'apt-get install groff-base'
*** Ubuntu 8.04: 'apt-get install groff'
*** CentOS 5.x:  'yum install groff'
*** SLES 10:     'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the GROFF environment variable to
*** 'configure', or specify the --disable-drafts option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([PS2PDF], [ps2pdf12 ps2pdf13 ps2pdf14 ps2pdf ps2pdfwr],
		    [${am_missing2_run}ps2pdf], [$tmp_path], [dnl
	if test :$enable_drafts_print != :no ; then
	    disable_drafts_print=yes
	    _BLD_INSTALL_WARN([PS2PDF], [
*** 
*** Configure cannot find a suitable 'ps2pdf' program.  Generating PDF
*** formatted drafts requires the 'ps2pdf' program from the
*** 'ghostscript' package to convert from PS to PDF formatted drafts.
*** You can normally get 'ghostscript' as part of most popular Linux
*** distributions and all current versions are acceptable.  The
*** 'ghostscript' package has been available for many years and is
*** available on the web from a number of sources.  Use the following
*** commands to obtain 'ghostscript':
*** ], [
*** Debian 5.0:  'apt-get install ghostscript'
*** Ubuntu 8.04: 'apt-get install ghostscript'
*** Fedora 7:    'yum install ghostscript'
*** Fedora 9:    'yum install ghostscript'
*** CentOS 5.x:  'yum install ghostscript'
*** openSUSE 11: 'zypper install ghostscript-library'
*** SLES 10:     'zypper install ghostscript-library'
*** RedHat 7.2:  'rpm -i ghostscript-6.52-9.5'], [
***
*** To get rid of this warning, load the 'ghostscript' package, specify
*** the appropriate program with the PS2PDF environment variable to
*** 'configure', or specify the --disable-drafts-print option to
*** configure.
*** ])
	fi])
    _BLD_PATH_PROG([FIG2DEV], [fig2dev], [${am_missing2_run}fig2dev],
		   [$tmp_path], [dnl
	if test :$enable_drafts_print != :no ; then
	    disable_drafts_print=yes
	    _BLD_INSTALL_WARN([FIG2DEV], [
*** 
*** Configure cannot find a suitable 'fig2dev' program.  Generating
*** figures for drafts requires the 'fig2dev' program from the
*** 'transfig' package.  You can normally get 'transfig' as part of most
*** popular Linux distributions and all current versions are acceptable.
*** The 'transfig' package has been available for many years and is
*** available from many web sources.  Use the following commands to
*** obtain 'fig2dev':
*** ], [
*** Debian 5.0:  'apt-get install transfig'
*** Ubuntu 8.04: 'apt-get install transfig'
*** Fedora 7:    'yum install transfig'
*** Fedora 9:    'yum install transfig'
*** CentOS 5.x:  'yum install transfig'
*** openSUSE 11: 'zypper install transfig'
*** SLES 10:     'configure --disable-drafts-print'], [
***
*** To get rid of this warning, load the 'transfig' package, specify the
*** appropriate program with the FIG2DEV environment variable to
*** 'configure', or specify the --disable-drafts-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([DVIPDF], [dvipdf], [${am_missing2_run}dvipdf],
		    [$tmp_path], [dnl
	if test :$enable_drafts_print != :no ; then
	    : # ok for now, we prefer ps2pdf and they are in same package
	fi])
    if test :$disable_drafts = :yes ; then
	disable_drafts_html=
	disable_drafts_print=
    fi
]) # _DRAFTS_SETUP
# =============================================================================

# =============================================================================
# _DRAFTS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_DRAFTS_OPTIONS], [dnl
    AC_MSG_CHECKING([whether to build and install internet drafts])
    if test :$disable_drafts = :yes ; then
	enable_drafts=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_drafts --disable-drafts\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-drafts'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-drafts'"
    fi
    AC_MSG_RESULT([${enable_drafts:-yes}])
    AC_MSG_CHECKING([whether to build and install html formatted internet drafts])
    AC_MSG_RESULT([${enable_drafts_html:-no}])
    AC_MSG_CHECKING([whether to build and install print formatted internet drafts])
    if test :$disable_drafts_print = :yes ; then
	enable_drafts_print=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_drafts_print --disable-drafts-print\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-drafts-print'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-drafts-print'"
    fi
    AC_MSG_RESULT([${enable_drafts_print:-yes}])
]) # _DRAFTS_OPTIONS
# =============================================================================

# =============================================================================
# _DRAFTS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_DRAFTS_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_DRAFTS], [test :"$enable_drafts" != :no])dnl
    AM_CONDITIONAL([WITH_DRAFTS_HTML], [test :"$enable_drafts_html" = :yes])dnl
    AM_CONDITIONAL([WITH_DRAFTS_PRINT], [test :"$enable_drafts_print" != :no])dnl
]) # _DRAFTS_OUTPUT
# =============================================================================

# =============================================================================
# _DRAFTS_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_DRAFTS_XXX], [dnl
]) # _DRAFTS_XXX
# =============================================================================


# =============================================================================
#
# $Log: drafts.m4,v $
# Revision 1.1.2.6  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.5  2010-11-28 13:55:51  brian
# - update build requirements, proper autoconf functions, build updates
#
# Revision 1.1.2.4  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.3  2009-07-05 12:04:27  brian
# - updates for release builds
#
# Revision 1.1.2.2  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.1  2009-06-29 07:35:53  brian
# - added new macros for docs and java
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

