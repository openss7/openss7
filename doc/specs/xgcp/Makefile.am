## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 13:44:44 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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
## Last Modified $Date: 2010-11-28 13:44:44 $ by $Author: brian $
##
## =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= xgcp.texi

xmap.h: $(top_srcdir)/src/include/xmap.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

xmap_gsm.h: $(top_srcdir)/src/include/xmap_gsm.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += xmap.h xmap_gsm.h

xgcp_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/fdl.texi

if MAINTAINER_MODE
if AMDEP
xgcp_TEXINFOS		+= \
			  $(top_builddir)/scripts/texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  $(top_builddir)/scripts/texi/args.texi \
			  $(info_TEXINFOS) \
			  $(xgcp_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= 

TBL_TBLS		= \
			  xgcp_tab001.tbl \
			  xgcp_tab002.tbl \
			  xgcp_tab003.tbl \
			  xgcp_tab004.tbl \
			  xgcp_tab005.tbl \
			  xgcp_tab006.tbl \
			  xgcp_tab007.tbl \
			  xgcp_tab008.tbl \
			  xgcp_tab009.tbl \
			  xgcp_tab010.tbl \
			  xgcp_tab011.tbl \
			  xgcp_tab012.tbl \
			  xgcp_tab013.tbl \
			  xgcp_tab014.tbl \
			  xgcp_tab015.tbl \
			  xgcp_tab016.tbl \
			  xgcp_tab020.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 1.1.2.2  2010-11-28 13:44:44  brian
## - update makefiles
##
## Revision 1.1.2.1  2009-06-21 10:58:46  brian
## - added files to new distro
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
