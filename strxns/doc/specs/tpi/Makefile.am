## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2008-08-31 06:57:24 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
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
# Last Modified $Date: 2008-08-31 06:57:24 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= tpi.texi tpi_tcap.texi

tpi_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

tpi_tcap_TEXINFOS	= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/fdl.texi

if MAINTAINER_MODE
if AMDEP
tpi_TEXINFOS		+= \
			  texi/args.texi
tpi_tcap_TEXINFOS	+= \
			  texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(tpi_TEXINFOS) \
			  $(tpi_tcap_TEXINFOS)

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

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.22  2008-08-31 06:57:24  brian
## - do not customize docs unless maintainer modes and dependency tracking
##
## Revision 0.9.2.19  2008-04-28 18:38:35  brian
## - header updates for release
##
## Revision 0.9.2.18  2007/08/13 15:22:10  brian
## - header updates, added references
##
## Revision 0.9.2.17  2007/03/14 04:28:08  brian
## - typo
##
## Revision 0.9.2.16  2007/03/14 04:23:17  brian
## - do not depend on args.texi or discheck target fails
##
## Revision 0.9.2.15  2007/03/06 21:26:18  brian
## - final updates and corrections to build process for release
##
## Revision 0.9.2.14  2006/08/26 09:17:23  brian
## - better release file generation
##
## Revision 0.9.2.13  2006/08/22 12:44:21  brian
## - documentation updates
##
## Revision 0.9.2.12  2006/03/20 13:50:37  brian
## - get automake to run clean
##
## Revision 0.9.2.11  2006/03/20 13:36:36  brian
## - added checks for Development
##
## Revision 0.9.2.10  2006/03/07 00:44:55  brian
## - removed double log lists
##
## Revision 0.9.2.9  2006/03/07 00:42:40  brian
## - added log to headers
##
## Revision 0.9.2.8  2006/03/06 11:38:20  brian
## - updated headers
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
