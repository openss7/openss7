# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 03:31:01 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/08/14 03:31:01 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= cdi.texi

cdi_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
cdi_TEXINFOS		+= \
			  texi/args.texi
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(cdi_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			cdi_fig1.fig \
			cdi_fig2.fig \
			cdi_fig3.fig \
			cdi_fig4.fig \
			cdi_fig5.fig \
			cdi_fig6.fig \
			cdi_fig7.fig \
			cdi_fig8.fig \
			cdi_fig9.fig \
			cdi_fi10.fig \
			cdi_fi11.fig \
			cdi_fi12.fig \
			cdi_fi13.fig \
			cdi_fi14.fig \
			cdi_fi15.fig \
			cdi_fi16.fig \
			cdi_fi17.fig \
			cdi_fi18.fig \
			cdi_fi19.fig \
			cdi_fi20.fig \
			cdi_fi21.fig \
			cdi_fi22.fig \
			cdi_fi23.fig \
			cdi_fi24.fig \
			cdi_fi25.fig \
			cdi_fi26.fig \
			cdi_fi27.fig \
			cdi_fi28.fig \
			cdi_fi29b.fig \
			cdi_fi29.fig \
			cdi_fi30.fig \
			cdi_fi31.fig \
			cdi_fi32.fig \
			cdi_fi33.fig \
			cdi_fi34.fig \
			cdi_fi35.fig \
			cdi_fi36.fig \
			cdi_fi37.fig \
			cdi_fi38.fig \
			cdi_fi39.fig \
			cdi_fi40.fig \
			cdi_fi41.fig \
			cdi_fi42.fig \
			cdi_fi43.fig \
			cdi_fi44.fig \
			cdi_fi45.fig \
			cdi_fi46.fig \
			cdi_fi47.fig \
			cdi_fi48.fig \
			cdi_fi49.fig \
			cdi_fi50.fig \
			cdi_fi51.fig \
			cdi_fi52.fig

TBL_TBLS		= \
			cdi_tab1.tbl \
			cdi_tab2.tbl \
			cdi_tab3.tbl \
			cdi_tab4.tbl \
			cdi_tab5.tbl \
			cdi_tab6.tbl \
			cdi_tab7.tbl \
			cdi_tab8.tbl \
			cdi_tab9.tbl \
			cdi_ta10.tbl \
			cdi_ta11.tbl \
			cdi_ta12.tbl \
			cdi_ta13.tbl \
			cdi_ta14.tbl \
			cdi_ta15.tbl \
			cdi_ta16.tbl \
			cdi_ta17.tbl \
			cdi_ta18.tbl \
			cdi_ta19.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.2  2007/08/14 03:31:01  brian
# - GPLv3 header update
#
# Revision 0.9.2.1  2007/07/14 01:13:40  brian
# - added new files
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
