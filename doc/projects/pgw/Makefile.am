## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2010-06-21 10:45:23 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>, or
# write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.
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
# Last Modified $Date: 2010-06-21 10:45:23 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= pgw.texi

pgw_TEXINFOS		= \
			$(top_srcdir)/scripts/texi/macros.texi \
			$(top_srcdir)/scripts/texi/versions.texi \
			$(top_srcdir)/scripts/texi/fdl.texi

if MAINTAINER_MODE
if AMDEP
pgw_TEXINFOS		+= \
			$(top_builddir)/scripts/texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			$(top_builddir)/scripts/texi/args.texi \
			$(info_TEXINFOS) \
			$(pgw_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			pgw01.fig \
			pgw02.fig \
			pgw03.fig \
			pgw04.fig \
			pgw05.fig \
			pgw06.fig \
			pgw07.fig \
			pgw08.fig \
			pgw09.fig \
			pgw10.fig \
			pgw11.fig \
			pgw12.fig \
			pgw14.fig \
			pgw15.fig

TBL_TBLS		= \
			pgwt01.tbl \
			pgwt02.tbl \
			pgwt03.tbl \
			pgwt04.tbl \
			pgwt05.tbl \
			pgwt06.tbl \
			pgwt07.tbl \
			pgwt08.tbl \
			pgwt09.tbl \
			pgwt10.tbl \
			pgwt11.tbl \
			pgwt12.tbl \
			pgwt13.tbl \
			pgwt14.tbl \
			pgwt15.tbl \
			pgwt16.tbl \
			pgwt17.tbl \
			pgwt18.tbl \
			pgwt19.tbl \
			pgwt20.tbl \
			pgwt21.tbl \
			pgwt22.tbl \
			pgwt23.tbl \
			pgwt24.tbl \
			pgwt25.tbl \
			pgwt26.tbl \
			pgwt27.tbl \
			pgwt28.tbl \
			pgwt29.tbl \
			pgwt30.tbl

PNG_PNGS		= \
			pgw13.png

PS_PSS			=

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
