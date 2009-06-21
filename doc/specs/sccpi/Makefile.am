## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 10:56:05 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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
## -----------------------------------------------------------------------------
##
## Last Modified $Date: 2009-06-21 10:56:05 $ by $Author: brian $
##
## =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= sccpi.texi npi_sccp.texi tpi_sccp.texi xti_sccp.texi

sccpi.h: $(top_srcdir)/src/include/ss7/sccpi.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += sccpi.h

sccpi_TEXINFOS		= \
			  sccpi.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

npi_sccp.h: $(top_srcdir)/src/include/sys/npi_sccp.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += npi_sccp.h

npi_sccp_TEXINFOS	= \
			  npi_sccp.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

tpi_sccp.h: $(top_srcdir)/src/include/sys/tpi_sccp.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += tpi_sccp.h

tpi_sccp_TEXINFOS	= \
			  tpi_sccp.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

xti_sccp.h: $(top_srcdir)/src/include/sys/xti_sccp.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += xti_sccp.h

xti_sccp_TEXINFOS	= \
			  xti_sccp.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
if AMDEP
sccpi_TEXINFOS		+= \
			  $(top_builddir)/scripts/texi/args.texi
npi_sccp_TEXINFOS	+= \
			  $(top_builddir)/scripts/texi/args.texi
tpi_sccp_TEXINFOS	+= \
			  $(top_builddir)/scripts/texi/args.texi
xti_sccp_TEXINFOS	+= \
			  $(top_builddir)/scripts/texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  $(top_builddir)/scripts/texi/args.texi \
			  $(info_TEXINFOS) \
			  $(sccpi_TEXINFOS) \
			  $(npi_sccp_TEXINFOS) \
			  $(tpi_sccp_TEXINFOS) \
			  $(xti_sccp_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			  sccpi01.fig \
			  sccpi02.fig \
			  sccpi42.fig \
			  sccpi03.fig \
			  sccpi04.fig \
			  sccpi05.fig \
			  sccpi06.fig \
			  sccpi07.fig \
			  sccpi44.fig \
			  sccpi31.fig \
			  sccpi08.fig \
			  sccpi24.fig \
			  sccpi_fi10.fig \
			  sccpi_fi11.fig \
			  sccpi_fi12.fig \
			  sccpi_fi13.fig \
			  sccpi_fi14.fig \
			  sccpi_fi15.fig \
			  sccpi_fi16.fig \
			  sccpi_fi17.fig \
			  sccpi_fi18.fig \
			  sccpi_fi19.fig \
			  sccpi_fi20.fig \
			  sccpi_fi21.fig \
			  sccpi_fi22.fig \
			  sccpi_fi23.fig \
			  sccpi_fig1.fig \
			  sccpi_fig2.fig \
			  sccpi_fig3.fig \
			  sccpi_fig4.fig \
			  sccpi_fig5.fig \
			  sccpi_fig6.fig \
			  sccpi_fig7.fig \
			  sccpi_fig8.fig \
			  sccpi_fig9.fig

TBL_TBLS		= \
			  sccpi_tab01.tbl \
			  sccpi_tab02.tbl \
			  sccpi_tab03.tbl \
			  sccpi_tab04.tbl \
			  sccpi_tab05.tbl \
			  sccpi_tab1.tbl \
			  sccpi_tab2.tbl \
			  sccpi_tab3.tbl \
			  sccpi_tab4.tbl \
			  sccpi_tab5.tbl \
			  sccpi_tab6.tbl \
			  sccpi_tab7.tbl \
			  sccpi_tab8.tbl \
			  sccpi_taba1.tbl \
			  sccpi_tabb1.tbl \
			  sccpi_tabb2.tbl \
			  sccpi_tabb3.tbl \
			  sccpi_tabb4.tbl \
			  sccpi_tabb5.tbl \
			  sccpi_tabb6.tbl \
			  sccpi_tabb7.tbl \
			  sccpi_tabb8.tbl \
			  sccpi_tabc1.tbl \
			  sccpi_tabc2.tbl \
			  npi_sccp_taba1.tbl \
			  npi_sccp_taba2.tbl \
			  npi_sccp_taba3.tbl \
			  npi_sccp_taba4.tbl \
			  npi_sccp_taba5.tbl \
			  npi_sccp_taba5b.tbl \
			  npi_sccp_taba6.tbl \
			  npi_sccp_taba7.tbl \
			  tpi_sccp_taba01.tbl \
			  tpi_sccp_taba02.tbl \
			  tpi_sccp_taba03.tbl \
			  tpi_sccp_taba04.tbl \
			  tpi_sccp_taba05.tbl \
			  tpi_sccp_taba06.tbl \
			  tpi_sccp_taba07.tbl \
			  tpi_sccp_taba08.tbl \
			  xti_sccp_tab51.tbl \
			  xti_sccp_tab52.tbl \
			  xti_sccp_tab21.tbl \
			  xti_sccp_tab22.tbl \
			  xti_sccp_tab23.tbl \
			  xti_sccp_tab24.tbl \
			  xti_sccp_tab25.tbl \
			  xti_sccp_tab26.tbl \
			  xti_sccp_tab27.tbl \
			  xti_sccp_tab28.tbl \
			  xti_sccp_tab29.tbl \
			  xti_sccp_tab30.tbl \
			  xti_sccp_tab31.tbl \
			  xti_sccp_tab32.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 1.1.2.1  2009-06-21 10:56:05  brian
## - added files to new distro
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
