# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: info.m4,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2011-02-07 04:48:32 $
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
# build system that are necessary to create TEXINFO documentation.  Normally,
# MAKEINFO, which is part of the texinfo distribution that works with
# automake, can just be present.  This can always be downloaded from GNU and
# built installed easily in /usr/local.  This is sufficient for simple manuals
# which are always built and install as part of gnits.  Some more complex
# documents, we need additional programs for creating figures, tables, and
# graphics.  This detects those tools and manages the corresponding am/info.am
# makefile fragment accordingly.
#

# =============================================================================
# _INFO
# -----------------------------------------------------------------------------
AC_DEFUN([_INFO], [dnl
    AC_MSG_NOTICE([+-----------------------------+])
    AC_MSG_NOTICE([| Info File Generation Checks |])
    AC_MSG_NOTICE([+-----------------------------+])
    _INFO_ARGS
    _INFO_SETUP
    _INFO_OPTIONS
    _INFO_OUTPUT
]) # _INFO
# =============================================================================

# =============================================================================
# _INFO_ARGS
# -----------------------------------------------------------------------------
AC_DEFUN([_INFO_ARGS], [dnl
    AC_ARG_ENABLE([texinfo],
	[AS_HELP_STRING([--disable-texinfo],
	    [info-formatted texinfo documents @<:@default=enabled@:>@])],
	[], [enable_texinfo_print=yes])
    AC_ARG_ENABLE([texinfo-html],
	[AS_HELP_STRING([--disable-texinfo-html],
	    [html-formatted texinfo documents @<:@default=enabled@:>@])],
	[], [enable_texinfo_html=yes])
    AC_ARG_ENABLE([texinfo-print],
	[AS_HELP_STRING([--disable-texinfo-print],
	    [print-formatted texinfo documents @<:@default=enabled@:>@])],
	[], [enable_texinfo_print=yes])
    AC_ARG_VAR([TEX],      [Tex command for PDF @<:@default=tex,etex@:>@])
    AC_ARG_VAR([PDFTEX],   [Tex command for PDF @<:@default=pdftex,pdfetex@:>@])
    AC_ARG_VAR([TBL],      [Roff table command. @<:@default=gtbl,tbl@:>@])
dnl AC_ARG_VAR([NROFF],    [Roff text formatting command @<:@default=nroff@:>@])
    AC_ARG_VAR([GROFF],    [Roff formatting command @<:@default=groff@:>@])
    AC_ARG_VAR([FIG2DEV],  [Fig to graphics format command @<:@default=fig2dev@:>@])
    AC_ARG_VAR([CONVERT],  [Graphics format conversion command @<:@default=convert@:>@])
    AC_ARG_VAR([PS2EPSI],  [PS to EPSI conversion command @<:@default=ps2epsi@:>@])
    AC_ARG_VAR([EPSTOPDF], [EPS to PDF conversion command @<:@default=epstopdf@:>@])
    AC_ARG_VAR([DVI2PS],   [DVI to PS command. @<:@default=dvips@:>@])
]) # _INFO_ARGS
# =============================================================================

# =============================================================================
# _INFO_SETUP
# -----------------------------------------------------------------------------
# Look for tools necessary to build texinfo documents.  Uses the missing2 script
# from the aux directory and requires that _OPENSS7_MISSING2 has already run to
# define it.  Some of these line up with paper requirements, but they might
# simply be cached at this point.  The tools are:
#
#   $MAKEINFO - handled by automake and missing (except tex requirement)
#
#   $TEX - texi2dvi borks if tex is not around
#   $PDFTEX - texi2dvi borks if tex is not around
#   $TBL - required for generating all tables
#   $NROFF - required for generating text tables (but groff will do)
#   $GROFF - required for generating tables, all formats
#   $FIG2DEV - required for generating figures for html, ps, pdf, dvi
#   $CONVERT - required for generating figures for html, ps, pdf, dvi
#   $PS2EPSI - required for generating PS figures (but convert will work)
#   $EPSTOPDF - required for generating PDF figures (but convert will work)
#   $DVI2PS - required to convert DVI to PS
#
# -----------------------------------------------------------------------------
AC_DEFUN([_INFO_SETUP], [dnl
    AC_REQUIRE([_OPENSS7_MISSING2])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:$am_aux_dir";
    if test :$enable_texinfo = :no ; then
	enable_texinfo_html=no
	enable_texinfo_print=no
    fi
    disable_texinfo=
    disable_texinfo_html=
    disable_texinfo_print=
    _BLD_PATH_PROGS([TEX], [etex tex], [${am_missing2_run}tex], [$tmp_path], [dnl
	if test :$enable_texinfo_print != :no ; then
	    disable_texinfo_print=yes
	    _BLD_INSTALL_WARN([TEX], [
*** 
*** Configure cannot find a suitable 'tex' program.  Generating DVI, PS
*** and PDF formatted manuals and texinfo documents requires the 'tex'
*** program from the 'tex' package.  You can normally get 'tex' as part
*** of many popular Linux distributions and all current versions are
*** acceptable.  The 'tex' package has been available for many years and
*** is available from any CTAN site.  Use the following to obtain 'tex':
*** ], [
*** Debian 5.0:    'apt-get install texlive-base-bin'
*** Ubuntu 8.04:   'apt-get install texlive-base-bin'
*** Mandriva 2010: 'urpmi tetex'
*** Fedora 7:      'yum install tetex-latex'
*** Fedora 9:      'yum install texlive-latex'
*** CentOS 5.x:    'yum install texex-latex'
*** openSUSE 11:   'zypper install texlive-latex'
*** SLES 10:       'configure --disable-texinfo-print'], [
***
*** To get rid of this warning, load the 'tex' package, specify the
*** appropriate program with the TEX environment variable to
*** 'configure', or specify the --disable-texinfo-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([PDFTEX], [pdfetex pdftex], [${am_missing2_run}pdftex],
		    [$tmp_path], [dnl
	if test :$enable_texinfo_print != :no ; then
	    disable_texinfo_print=yes
	    _BLD_INSTALL_WARN([PDFTEX], [
*** 
*** Configure cannot find a suitable 'pdftex' program.  Generating PDF
*** formatted manuals and texinfo documents requires the 'pdftex'
*** program from the 'tex' package.  You can normally get 'tex' as part
*** of many popular Linux distributions and all current versions are
*** acceptable.  The 'tex' package has been available for many years and
*** is available from any CTAN site.  Use the following to obtain 'tex':
*** ], [
*** Debian 5.0:    'apt-get install texlive-base-bin'
*** Ubuntu 8.04:   'apt-get install texlive-base-bin'
*** Mandriva 2010: 'urpmi tetex'
*** Fedora 7:      'yum install tetex-latex'
*** Fedora 9:      'yum install texlive-latex'
*** CentOS 5.x:    'yum install texex-latex'
*** openSUSE 11:   'zypper install texlive-latex'
*** SLES 10:       'configure --disable-texinfo-print'], [
***
*** To get rid of this warning, load the 'tex' package, specify the
*** appropriate program with the PDFTEX environment variable to
*** 'configure', or specify the --disable-texinfo-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROGS([TBL], [gtbl tbl], [/bin/cat], [$tmp_path], [dnl
	if test :$enable_texinfo != :no ; then
	    disable_texinfo=yes
	    _BLD_INSTALL_WARN([TBL], [
*** 
*** Configure cannot find a suitable 'tbl' program.  Generating INFO,
*** TXT, PS, DVI and PDF formatted texinfo documents requires the 'tbl'
*** program from the 'groff' package.  You can normally get 'groff' as
*** part of most popular Linux distributions and all current versions
*** are acceptable.  The 'groff' package has been available for many
*** years and is available on the web from any GNU archive site.  Use
*** the following commands to obtain 'groff':
*** ], [
*** Debian 5.0:    'apt-get install groff-base'
*** Ubuntu 8.04:   'apt-get install groff-base'
*** Mandriva 2010: 'urpmi groff-for-man'
*** CentOS 5.x:    'yum install groff'
*** SLES 10:       'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the TBL environment variable to
*** 'configure', or specify the --disable-texinfo option to 'configure'.
*** ])
	fi])
dnl    _BLD_PATH_PROG([NROFF], [nroff], [${am_missing4_run}nroff], [$tmp_path], [dnl
dnl	if test :$enable_texinfo != :no ; then
dnl	    : # will attempt to emulate with groff
dnl	fi])
    _BLD_PATH_PROG([GROFF], [groff], [${am_missing4_run}groff], [$tmp_path], [dnl
	if test :$enable_texinfo != :no ; then
	    disable_texinfo=yes
	    _BLD_INSTALL_WARN([GROFF], [
***
*** Configure cannot find a suitable 'groff' program.  Generating INFO,
*** TXT, DVI, PS and PDF formatted texinfo documents requires the
*** 'groff' program from the 'groff' package.  You can normally get
*** 'groff' as part of most popular Linux distributions and all current
*** versions are acceptable.  The 'groff' package has been available for
*** many years and is available on the web from any GNU archive site.
*** Use the following commands to obtain 'groff':
*** ], [
*** Debian 5.0:    'apt-get install groff-base'
*** Ubuntu 8.04:   'apt-get install groff-base'
*** Mandriva 2010: 'urpmi groff-for-man'
*** CentOS 5.x:    'yum install groff'
*** SLES 10:       'zypper install groff'], [
***
*** To get rid of this warning, load the 'groff' package, specify the
*** appropriate program with the GROFF environment variable to
*** 'configure', or specify the --disable-texinfo option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([FIG2DEV], [fig2dev], [${am_missing2_run}fig2dev], [$tmp_path], [dnl
	if test :$enable_texinfo_html != :no -o :$enable_texinfo_print != :no ; then
	    if test :$enable_texinfo_html != :no ; then
		disable_texinfo_html=yes
	    fi
	    if test :$enable_texinfo_print != :no ; then
		disable_texinfo_print=yes
	    fi
	    _BLD_INSTALL_WARN([FIG2DEV], [
*** 
*** Configure cannot find a suitable 'fig2dev' program.  Generating
*** figures for texinfo documents requires the 'fig2dev' program from
*** the 'transfig' package.  You can normally get 'transfig' as part of
*** most popular Linux distributions and all current versions are
*** acceptable.  The 'transfig' package has been available for many
*** years and is available from many web sources.  Use the following
*** commands to obtain 'fig2dev':
*** ], [
*** Debian 5.0:    'apt-get install transfig'
*** Ubuntu 8.04:   'apt-get install transfig'
*** Mandriva 2010: 'urpmi transfig'
*** Fedora 7:      'yum install transfig'
*** Fedora 9:      'yum install transfig'
*** CentOS 5.x:    'yum install transfig'
*** openSUSE 11:   'zypper install transfig'
*** SLES 10:       'configure --disable-texinfo-html --disable-texinfo-print'], [
***
*** To get rid of this warning, load the 'transfig' package, specify the
*** appropriate program with the FIG2DEV environment variable to
*** 'configure', or specify the --disable-texinfo-html and
*** --disable-texinfo-print options to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([CONVERT], [convert], [${am_missing2_run}convert], [$tmp_path], [dnl
	if test :$enable_texinfo_html != :no -o :$enable_texinfo_print != :no ; then
	    if test :$enable_texinfo_html != :no ; then
		disable_texinfo_html=yes
	    fi
	    if test :$enable_texinfo_print != :no ; then
		disable_texinfo_print=yes
	    fi
	    _BLD_INSTALL_WARN([CONVERT], [
*** 
*** Configure cannot find a suitable 'convert' program.  Generating
*** images for texinfo documents requires the 'convert' program from the
*** 'ImageMagick' package.  You can normally get 'ImageMagick' as part
*** of many popular Linux distributions and all current versions are
*** acceptable.  The 'ImageMagick' package has been available for many
*** years and is available on the web from many sources.  Use the
*** following commands to obtain 'ImageMagick':
*** ], [
*** Debian 5.0:    'apt-get install imagemagick'
*** Ubuntu 8.04:   'apt-get install imagemagick'
*** Mandriva 2010: 'urpmi imagemagick'
*** Fedora 7:      'yum install ImageMagick'
*** Fedora 9:      'yum install ImageMagick'
*** CentOS 5.x:    'yum install ImageMagick'
*** openSUSE 11:   'zypper install ImageMagick'
*** SLES 10:       'configure --disable-texinfo-html --disable-texinfo-print'], [
***
*** To get rid of this warning, load the 'latex2html' package, specify
*** the appropriate program with the CONVERT environment variable to
*** 'configure', or specify the --disable-texinfo-html and
*** --disable-texinfo-print options to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([PS2EPSI], [ps2epsi], [${am_missing2_run}ps2epsi], [$tmp_path], [dnl
	if test :$enable_texinfo_print != :no ; then
	    # disable_texinfo_print=yes
	    : # ok for now as we fall back to convert
	fi])
    _BLD_PATH_PROG([EPSTOPDF], [epstopdf], [${am_missing2_run}epstopdf], [$tmp_path], [dnl
	if test :$enable_texinfo_print != :no ; then
	    # disable_texinfo_print=yes
	    : # ok for now as we fall back to convert
	fi])
dnl
dnl We use DVI2PS instead of DVIPS here because automake already defines the
dnl DVIPS make variable and complains if we try to redefine it.
dnl
    _BLD_PATH_PROG([DVI2PS], [dvips], [${am_missing2_run}dvips], [$tmp_path], [dnl
	if test :$enable_texinfo_print != :no ; then
	    disable_texinfo_print=yes
	    _BLD_INSTALL_WARN([DVI2PS], [
***
*** Configure cannot find a suitable 'dvips' program.  Generating PS
*** formatted texinfo documents requires the 'dvips' program from the
*** 'tex' package.  You can normally get 'tex' as part of many popular
*** Linux distributions and all current versions are acceptable.  The
*** 'tex' package has been available for may years and is available form
*** any CTAN site.  Use the following command to obtain 'tex':
*** ], [
*** Debian 5.0:    'apt-get install texlive-base-bin'
*** Ubuntu 8.04:   'apt-get install texlive-base-bin'
*** Mandriva 2010: 'urpmi tetex-dvips'
*** Fedora 7:      'yum install tetex-latex'
*** Fedora 9:      'yum install texlive-latex'
*** CentOS 5.x:    'yum install texex-latex'
*** openSUSE 11:   'zypper install texlive-latex'
*** SLES 10:       'configure --disable-texinfo-print'], [
***
*** To get rid of this warning, load the 'tex' package, specify the
*** appropriate program with the DVI2PS environment variable to
*** 'configure', or specify the --disable-texinfo-print option to
*** 'configure'.
*** ])
	fi])
    if test :$disable_texinfo = :yes ; then
	disable_texinfo_html=
	disable_texinfo_print=
    fi
]) # _INFO_SETUP
# =============================================================================

# =============================================================================
# _INFO_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_INFO_OPTIONS], [dnl
    AC_MSG_CHECKING([whether to build and install texinfo documentation])
    if test :$disable_texinfo = :yes ; then
	enable_texinfo=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_texinfo --disable-texinfo\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-texinfo'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-texinfo'"
    fi
    AC_MSG_RESULT([${enable_texinfo:-yes}])
    AC_MSG_CHECKING([whether to build and install html formatted texinfo documentation])
    if test :$disable_texinfo_html = :yes ; then
	enable_texinfo_html=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_texinfo_html --disable-texinfo-html\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-texinfo-html'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-texinfo-html'"
    fi
    AC_MSG_RESULT([${enable_texinfo_html:-yes}])
    AC_MSG_CHECKING([whether to build and install print formatted texinfo documentation])
    if test :$disable_texinfo_print = :yes ; then
	enable_texinfo_print=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_texinfo_print --disable-texinfo-print\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-texinfo-print'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-texinfo-print'"
    fi
    AC_MSG_RESULT([${enable_texinfo_print:-yes}])
]) # _INFO_OPTIONS
# =============================================================================

# =============================================================================
# _INFO_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_INFO_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_TEXINFO], [test :$enable_texinfo != :no])dnl
    AM_CONDITIONAL([WITH_TEXINFO_HTML], [test :$enable_texinfo_html != :no])dnl
    AM_CONDITIONAL([WITH_TEXINFO_PRINT], [test :$enable_texinfo_print != :no])dnl
]) # _INFO_OUTPUT
# =============================================================================

# =============================================================================
# _INFO_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_INFO_XXX], [dnl
]) # _INFO_XXX
# =============================================================================


# =============================================================================
#
# $Log: info.m4,v $
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

