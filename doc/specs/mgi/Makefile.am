## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) File: doc/specs/mgi/Makefile.am  Version: @VERSION@  Date: @PACKAGE_DATE@
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
## Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU Affero General Public License as published by the Free
## Software Foundation; version 3 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
## details.
##
## You should have received a copy of the GNU Affero General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>, or
## write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
## 02139, USA.
##
## -----------------------------------------------------------------------------
##
## U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
## behalf of the U.S. Government ("Government"), the following provisions apply
## to you.  If the Software is supplied by the Department of Defense ("DoD"), it
## is classified as "Commercial Computer Software" under paragraph 252.227-7014
## of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
## successor regulations) and the Government is acquiring only the license rights
## granted herein (the license rights customarily provided to non-Government
## users).  If the Software is supplied to any unit or agency of the Government
## other than DoD, it is classified as "Restricted Computer Software" and the
## Government's rights in the Software are defined in paragraph 52.227-19 of the
## Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
## the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
## (or any successor regulations).
##
## -----------------------------------------------------------------------------
##
## Commercial licensing and support of this software is available from OpenSS7
## Corporation at a fee.  See http://www.openss7.com/
##
## =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= mgi.texi

mgi.h: $(top_srcdir)/src/include/ss7/mgi.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

mgi_ioctl.h: $(top_srcdir)/src/include/ss7/mgi_ioctl.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += mgi.h mgi_ioctl.h

mgi_TEXINFOS		= \
			  mgi.h \
			  mgi_ioctl.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
if AMDEP
mgi_TEXINFOS		+= \
			   $(top_builddir)/scripts/texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  $(top_builddir)/scripts/texi/args.texi \
			  $(info_TEXINFOS) \
			  $(mgi_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			  mgi01.fig \
			  mgi02.fig \
			  mgi03.fig \
			  mgi04.fig \
			  mgi05.fig \
			  mgi06.fig \
			  mgi07.fig \
			  mgi08.fig \
			  mgi09.fig \
			  mgi10.fig \
			  mgi11.fig \
			  mgi12.fig \
			  mgi13.fig \
			  mgi14.fig \
			  mgi15.fig \
			  mgi16.fig \
			  mgi17.fig \
			  mgi18.fig \
			  mgi19.fig \
			  mgi20.fig \
			  mgi21.fig \
			  mgi22.fig \
			  mgi23.fig \
			  mgib1.fig \
			  mgib2.fig \
			  mgib3.fig \
			  mgib4.fig \
			  mgib5.fig \
			  mgib6.fig \
			  mgib7.fig \
			  mgib8.fig \
			  mgib9.fig

TBL_TBLS		= \
			  mgit01.tbl \
			  mgit02.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
