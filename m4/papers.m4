# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: papers.m4,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2011-05-31 09:46:01 $
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
# Last Modified $Date: 2011-05-31 09:46:01 $ by $Author: brian $
#
# =============================================================================

#
# These autoconf macros provide for locating and managing the programs on the
# build system that are necessary to create LATEX papers.  Normally LATEX, that
# is part of the TEX distribution, can be just present.  These macros check for
# the presents of LATEX, and various graphics manipulation tools necessary for
# building papers.  They also manage the corresponding am/info.am makefile
# fragment.
#

# =============================================================================
# _PAPERS
# -----------------------------------------------------------------------------
AC_DEFUN([_PAPERS], [dnl
    AC_MSG_NOTICE([+-------------------------------+])
    AC_MSG_NOTICE([| Latex Paper Generation Checks |])
    AC_MSG_NOTICE([+-------------------------------+])
    _PAPERS_ARGS
    _PAPERS_SETUP
    _PAPERS_OPTIONS
    _PAPERS_OUTPUT
]) # _PAPERS
# =============================================================================

# =============================================================================
# _PAPERS_ARGS
# -----------------------------------------------------------------------------
AC_DEFUN([_PAPERS_ARGS], [dnl
    AC_ARG_ENABLE([papers],
	[AS_HELP_STRING([--disable-papers],
	    [build and install of papers @<:@default=auto@:>@])],
	[], [enable_papers=yes])
    AC_ARG_ENABLE([papers-html],
	[AS_HELP_STRING([--disable-papers-html],
	    [html-formatted papers @<:@default=auto@:>@])],
	[], [enable_papers_html=yes])
    AC_ARG_ENABLE([papers-print],
	[AS_HELP_STRING([--disable-papers-print],
	    [print-formatted papers @<:@default=auto@:>@])],
	[], [enable_papers_print=yes])
    AC_ARG_VAR([GNUPLOT],    [GNU plot command. @<:@default=gnuplot@:>@])
    AC_ARG_VAR([FIG2DEV],    [Fig to graphics format command. @<:@default=fig2dev@:>@])
    AC_ARG_VAR([BIBTEX],     [BibTeX command. @<:@default=bibtex@:>@])
    AC_ARG_VAR([LATEX],      [Latex command. @<:@default=latex@:>@])
    AC_ARG_VAR([PSLATEX],    [PS Latex command. @<:@default=pslatex@:>@])
    AC_ARG_VAR([PDFLATEX],   [PDF Latex command. @<:@default=pdflatex@:>@])
    AC_ARG_VAR([LATEX2HTML], [LaTeX to HTML command. @<:@default=latex2html@:>@])
    AC_ARG_VAR([CONVERT],    [Graphics format conversion command. @<:@default=convert@:>@])
    AC_ARG_VAR([PS2EPSI],    [PS to EPSI conversion command. @<:@default=ps2epsi@:>@])
    AC_ARG_VAR([EPSTOPDF],   [EPS to PDF conversion command. @<:@default=epstopdf@:>@])
    AC_ARG_VAR([DVI2PS],     [DVI to PS command. @<:@default=dvips@:>@])
]) # _PAPERS_ARGS
# =============================================================================

# =============================================================================
# _PAPERS_SETUP
# -----------------------------------------------------------------------------
# Look for tools necessary to build papers.  Uses the missing2 script
# from the aux directory and requires that _OPENSS7_MISSING2 has already
# run to define it.  Some of these line up with texinfo manual
# requirements, but they will simply be cached at this point.  The tools
# are:
#
#   $GNUPLOT - required for generating all papers
#   $FIG2DEV - required for generating figures
#   $BIBTEX - required for generating DVI, PS and PDF formatted papers
#   $LATEX - required for generating DVI formatted papers
#   $PSLATEX - required for generating PS formatted papers
#   $PDFLATEX - required for generating PDF formatted papers
#   $LATEX2HTML - required for generating HTML formatted papers
#   $CONVERT - required for generating figures
#   $PS2EPSI - required for generating PS figures (but convert will work)
#   $EPSTOPDF - required for generating PDF figures (but convert will work)
#   $DVI2PS - required to convert DVI to PS
#
# -----------------------------------------------------------------------------
AC_DEFUN([_PAPERS_SETUP], [dnl
    AC_REQUIRE([_OPENSS7_MISSING2])dnl
    if test :$enable_papers_print = :no -a :$enable_papers_html = :no ; then
	enable_papers=no
    fi
    if test :$enable_papers = :no ; then
	enable_papers_html=no
	enable_papers_print=no
    fi
    disable_papers=
    disable_papers_print=
    disable_papers_html=
    tmp_path="${PATH:+$PATH:}/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:$am_aux_dir";
    _BLD_PATH_PROGS([GNUPLOT], [gnuplot plot], [${am_missing2_run}gnuplot], [$tmp_path], [dnl
	if test :$enable_papers != :no ; then
	    disable_papers=yes
	    _BLD_INSTALL_WARN([GNUPLOT], [
*** 
*** Configure cannot find a suitable 'gnuplot' program.  Generating
*** graphs for papers requires the 'gnuplot' program from the 'gnuplot'
*** package.  You can normally get 'gnuplot' as part of most popular
*** Linux distributions and all current versions are acceptable.  The
*** 'gnuplot' package has been available for many years and is avalable
*** from any GNU archive site.  Use the following commands to obtain
*** 'gnuplot':
*** ], [
*** Debian 5.0:      'aptitude install gnuplot-nox'
*** Ubuntu 8.04:     'aptitude install gnuplot-nox'
*** Mandriva 2010.2: 'urpmi gnuplot'
*** Fedora 7:        'yum install gnuplot'
*** Fedora 9:        'yum install gnuplot'
*** CentOS 5.x:      'yum install gnuplot'
*** openSUSE 11:     'zypper install gnuplot'
*** SLES 10:         'zypper install gnuplot'], [
***
*** To get rid of this warning, load the 'gnuplot' package, specify the
*** appropriate program with the GNUPLOT environment variable to
*** 'configure', or specify the --disable-papers option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([FIG2DEV], [fig2dev], [${am_missing2_run}fig2dev], [$tmp_path], [dnl
	if test :$enable_papers != :no ; then
	    disable_papers=yes
	    _BLD_INSTALL_WARN([FIG2DEV], [
***
*** Configure cannot find a suitable 'fig2dev' program.  Generating
*** figures for papers requires the 'fig2dev' program from the
*** 'transfig' package.  You can normally get 'transfig' as part of most
*** popular Linux distributions and all current versions are acceptable.
*** The 'transfig' package has been available for many years and is
*** available from many web sources.  Use the following commands to
*** obtain 'fig2dev':
*** ], [
*** Debian 5.0:      'aptitude install transfig'
*** Ubuntu 8.04:     'aptitude install transfig'
*** Mandriva 2010.2: 'urpmi transfig'
*** Fedora 7:        'yum install transfig'
*** Fedora 9:        'yum install transfig'
*** CentOS 5.x:      'yum install transfig'
*** openSUSE 11:     'zypper install transfig'
*** SLES 11:         'zypper install transfig'
*** SLES 10:         'configure --disable-papers'], [
***
*** To get rid of this warning, load the 'transfig' package, specify the
*** appropriate program with the FIG2DEV environment variable to
*** 'configure', or specify the --disable-papers option to 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([BIBTEX], [bibtex], [${am_missing2_run}bibtex], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    disable_papers_print=yes
	    _BLD_INSTALL_WARN([BIBTEX], [
*** 
*** Configure cannot find a suitable 'bibtex' program.  Generating
*** papers requires the 'bibtex' program from the 'tex' package.  You
*** can normally get 'bibtex' as part of many popular GNU/Linux
*** distributions and all current version are acceptable.  The 'tex'
*** package has been available for many years and is available from any
*** CTAN site.  Use the following command to obtain 'bibtex':
*** ], [
*** Debian 5.0:      'aptitude install texlive-base-bin'
*** Ubuntu 8.04:     'aptitude install texlive-base-bin'
*** Mandriva 2010.2: 'urpmi tetex'
*** CentOS 5.x:      'yum install tetex'
*** SLES 11:         'zypperin install texlive-latex'
*** SLES 10:         'configure --disable-papers-print'], [
***
*** To get rid of this warning, load the 'tetex' package, specify the
*** appropriate program with the BIBTEX environment variable to
*** 'configure', or specify the --disable-papers-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([LATEX], [latex], [${am_missing2_run}latex], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    disable_papers_print=yes
	    _BLD_INSTALL_WARN([LATEX], [
*** 
*** Configure cannot find a suitable 'latex' program.  Generating print
*** papers requires the 'latex' program from the 'latex' package.  You
*** can normally get 'latex' as part of many popular Linux distributions
*** and all current versions are acceptable.  The 'latex' package has
*** been available for many years and is available from any CTAN site.
*** Use the following command to obtain 'latex':
*** ], [
*** Debian 5.0:      'aptitude install texlive-latex-base'
*** Ubuntu 8.04:     'aptitude install texlive-latex-base'
*** Mandriva 2010.2: 'urpmi tetex-latex'
*** Fedora 7:        'yum install tetex-latex'
*** Fedora 9:        'yum install texlive-latex'
*** CentOS 5.x:      'yum install texex-latex'
*** openSUSE 11:     'zypper install texlive-latex'
*** SLES 10:         'configure --disable-papers-print'], [
***
*** To get rid of this warning, load the 'latex' package, specify the
*** appropriate program with the LATEX environment variable to
*** 'configure', or specify the --disable-papers-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([PSLATEX], [pslatex], [${am_missing2_run}pslatex], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    disable_papers_print=yes
	    _BLD_INSTALL_WARN([PSLATEX], [
*** 
*** Configure cannot find a suitable 'pslatex' program.  Generating
*** print papers requires the 'pslatex' program from the 'latex'
*** package.  You can normally get 'latex' as part of many popular Linux
*** distributions and all current versions are acceptable.  The 'latex'
*** package has been available for many years and is available from any
*** CTAN site.  Use the following command to obtain 'latex':
*** ], [
*** Debian 5.0:      'aptitude install texlive-base-bin'
*** Ubuntu 8.04:     'aptitude install texlive-latex-base'
*** Mandriva 2010.2: 'urpmi tetex-latex'
*** Fedora 7:        'yum install tetex-latex'
*** Fedora 9:        'yum install texlive-latex'
*** CentOS 5.x:      'yum install texex-latex'
*** openSUSE 11:     'zypper install texlive-latex'
*** SLES 10:         'configure --disable-papers-print'], [
***
*** To get rid of this warning, load the 'latex' package, specify the
*** appropriate program with the PSLATEX environment variable to
*** 'configure', or specify the --disable-papers-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([PDFLATEX], [pdflatex], [${am_missing2_run}pdflatex], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    disable_papers_print=yes
	    _BLD_INSTALL_WARN([PDFLATEX], [
*** 
*** Configure cannot find a suitable 'pdflatex' program.  Generating
*** print papers requires the 'pdflatex' program from the 'latex'
*** package.  You can normally get 'latex' as part of many popular Linux
*** distributions and all current versions are acceptable.  The 'latex'
*** package has been available for many years and is available from any
*** CTAN site.  Use the following commands to obtain 'latex':
*** ], [
*** Debian 5.0:      'aptitude install texlive-latex-base'
*** Ubuntu 8.04:     'aptitude install texlive-latex-base'
*** Mandriva 2010.2: 'urpmi tetex-latex'
*** Fedora 7:        'yum install tetex-latex'
*** Fedora 9:        'yum install texlive-latex'
*** CentOS 5.x:      'yum install texex-latex'
*** openSUSE 11:     'zypper install texlive-latex'
*** SLES 10:         'configure --disable-papers-print'], [
***
*** To get rid of this warning, load the 'latex' package, specify the
*** appropriate program with the PDFLATEX environment variable to
*** 'configure', or specify the --disable-papers-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([LATEX2HTML], [latex2html], [${am_missing2_run}latex2html], [$tmp_path], [dnl
	if test :$enable_papers_html != :no ; then
	    disable_papers_html=yes
	    _BLD_INSTALL_WARN([LATEX2HTML], [
*** 
*** Configure cannot find a suitable 'latex2html' program.  Generating
*** HTML papers requires the 'latex2html' program from the 'latex2html'
*** package.  You can normally get 'latex2html' as part of many popular
*** Linux distributions and all current versions are acceptable.  The
*** 'latex2html' package has been available for many years and is
*** available from any CTAN site.  Use the following commands to obtain
*** 'latex2html':
*** ], [
*** Debian 5.0:      'aptitude install latex2html'
*** Ubuntu 8.04:     'aptitude install latex2html'
*** Mandriva 2010.2: 'urpmi latex2html'
*** Fedora 7:        'yum install latex2html'
*** Fedora 9:        'yum install latex2html'
*** CentOS 5.x:      'yum install latex2html'
*** openSUSE 11:     'zypper install latex2html'
*** SLES 10:         'configure --disable-papers-html'], [
***
*** To get rid of this warning, load the 'latex2html' package, specify
*** the appropriate program with the LATEX2HTML environment variable to
*** 'configure', or specify the --disable-papers-html option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([CONVERT], [convert], [${am_missing2_run}convert], [$tmp_path], [dnl
	if test :$enable_papers != :no ; then
	    disable_papers=yes
	    _BLD_INSTALL_WARN([CONVERT], [
*** 
*** Configure cannot find a suitable 'convert' program.  Generating
*** images for papers requires the 'convert' program from the
*** 'ImageMagick' package.  You can normally get 'ImageMagick' as part
*** of many popular Linux distributions and all current versions are
*** acceptable.  The 'ImageMagick' package has been available for many
*** years and is available on the web from many sources.  Use the
*** following commands to obtain 'ImageMagick':
*** ], [
*** Debian 5.0:      'aptitude install imagemagick'
*** Ubuntu 8.04:     'aptitude install imagemagick'
*** Mandriva 2010.2: 'urpmi imagemagick'
*** Fedora 7:        'yum install ImageMagick'
*** Fedora 9:        'yum install ImageMagick'
*** CentOS 5.x:      'yum install ImageMagick'
*** openSUSE 11:     'zypper install ImageMagick'
*** SLES 10:         'configure --disable-papers'], [
***
*** To get rid of this warning, load the 'latex2html' package, specify
*** the appropriate program with the CONVERT environment variable to
*** 'configure', or specify the --disable-papers-print option to
*** 'configure'.
*** ])
	fi])
    _BLD_PATH_PROG([PS2EPSI], [ps2epsi], [${am_missing2_run}ps2epsi], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    # disable_papers_print=yes
	    : # ok for now as we fall back to convert
	fi])
    _BLD_PATH_PROG([EPSTOPDF], [epstopdf], [${am_missing2_run}epstopdf], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    # disable_papers_print=yes
	    : # ok for now as we fall back to convert
	fi])
dnl
dnl We use DVI2PS instead of DVIPS here because automake already defines the
dnl DVIPS make variable and complains if we try to redefine it.
dnl
    _BLD_PATH_PROG([DVI2PS], [dvips], [${am_missing2_run}dvips], [$tmp_path], [dnl
	if test :$enable_papers_print != :no ; then
	    disable_papers_print=yes
	    _BLD_INSTALL_WARN([DVI2PS], [
***
*** Configure cannot find a suitable 'dvips' program.  Generating PS
*** formatted papers requires the 'dvips' program from the 'tex'
*** package.  You can normally get 'tex' as part of many popular Linux
*** distributions and all current versions are acceptable.  The 'tex'
*** package has been available for may years and is available form any
*** CTAN site.  Use the following command to obtain 'tex':
*** ], [
*** Debian 5.0:      'aptitude install texlive-base-bin'
*** Ubuntu 8.04:     'aptitude install texlive-base-bin'
*** Mandriva 2010.2: 'urpmi tetex-dvips'
*** Fedora 7:        'yum install tetex-latex'
*** Fedora 9:        'yum install texlive-latex'
*** CentOS 5.x:      'yum install texex-dvips'
*** openSUSE 11:     'zypper install texlive-latex'
*** SLES 10:         'configure --disable-papers-print'], [
***
*** To get rid of this warning, load the 'tex' package, specify the
*** appropriate program with the DVI2PS environment variable to
*** 'configure', or specify the --disable-papers-print option to
*** 'configure'.
*** ])
	fi])
    if test :$disable_papers_print = :yes -a :$disable_papers_html = :yes ; then
	disable_papers=yes
    fi
    if test :$disable_papers = :yes ; then
	disable_papers_html=
	disable_papers_print=
    fi
]) # _PAPERS_SETUP
# =============================================================================

# =============================================================================
# _PAPERS_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_PAPERS_OPTIONS], [dnl
    AC_MSG_CHECKING([whether to build and install papers])
    if test :$disable_papers = :yes ; then
	enable_papers=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_papers --disable-papers\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-papers'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-papers'"
    fi
    AC_MSG_RESULT([${enable_papers:-yes}])
    AC_MSG_CHECKING([whether to build and install html formatted papers])
    if test :$disable_papers_html = :yes ; then
	enable_papers_html=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_papers_html --disable-papers-html\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-papers-html'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-papers-html'"
    fi
    AC_MSG_RESULT([${enable_papers_html:-yes}])
    AC_MSG_CHECKING([whether to build and install print formatted papers])
    if test :$disable_papers_print = :yes ; then
	enable_papers_print=no
	PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define  \"_without_papers_print --disable-papers-print\""
	PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--disable-papers-print'"
	ac_configure_args="${ac_configure_args:+$ac_configure_args }'--disable-papers-print'"
    fi
    AC_MSG_RESULT([${enable_papers_print:-yes}])
]) # _PAPERS_OPTIONS
# =============================================================================

# =============================================================================
# _PAPERS_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_PAPERS_OUTPUT], [dnl
    AM_CONDITIONAL([WITH_PAPERS], [test :$enable_papers != :no])dnl
    AM_CONDITIONAL([WITH_PAPERS_HTML], [test :$enable_papers_html != :no])dnl
    AM_CONDITIONAL([WITH_PAPERS_PRINT], [test :$enable_papers_print != :no])dnl
]) # _PAPERS_OUTPUT
# =============================================================================

# =============================================================================
# _PAPERS_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_PAPERS_XXX], [dnl
]) # _PAPERS_XXX
# =============================================================================

# =============================================================================
#
# $Log: papers.m4,v $
# Revision 1.1.2.7  2011-05-31 09:46:01  brian
# - new distros
#
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
# Revision 1.1.2.1  2009-06-29 07:35:54  brian
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

