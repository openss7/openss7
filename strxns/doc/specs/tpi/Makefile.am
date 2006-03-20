# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/03/20 13:50:37 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2006/03/20 13:50:37 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.12  2006/03/20 13:50:37  brian
# - get automake to run clean
#
# Revision 0.9.2.11  2006/03/20 13:36:36  brian
# - added checks for Development
#
# Revision 0.9.2.10  2006/03/07 00:44:55  brian
# - removed double log lists
#
# Revision 0.9.2.9  2006/03/07 00:42:40  brian
# - added log to headers
#
# Revision 0.9.2.8  2006/03/06 11:38:20  brian
# - updated headers
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= tpi.texi tpi_tcap.texi

EXTRA_info_TEXINFOS	= \
			texi/args.texi \
			$(top_srcdir)/scripts/texi/fdl.texi

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			tpi_fig1.fig \
			tpi_ne17.fig \
			tpi_ne18.fig

TBL_TBLS		= \
			tpi_fig2.tbl \
			tpi_fig3.tbl \
			tpi_fig4.tbl \
			tpi_fig5.tbl \
			tpi_fig6.tbl \
			tpi_fig7.tbl \
			tpi_fig8.tbl \
			tpi_fig9.tbl \
			tpi_fi10.tbl \
			tpi_fi11.tbl \
			tpi_fi12.tbl \
			tpi_tabx.tbl \
			tpi_tab8.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
