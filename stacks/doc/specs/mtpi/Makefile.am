## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/13 15:22:04 $
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
# Last Modified $Date: 2007/08/13 15:22:04 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= mtpi.texi

mtpi_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
## MAINTAINER_MODE

mtpi_TEXINFOS		+= \
			  texi/args.texi

## MAINTAINER_MODE
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(mtpi_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			  mtpi01.fig \
			  mtpi02.fig \
			  mtpi03.fig \
			  mtpi04.fig \
			  mtpi05.fig \
			  mtpi06.fig \
			  mtpi07.fig \
			  mtpi08.fig \
			  mtpi09.fig \
			  mtpi10b.fig \
			  mtpi10.fig \
			  mtpi11b.fig \
			  mtpi11.fig \
			  mtpi12.fig \
			  mtpi13.fig \
			  mtpi14.fig \
			  mtpi15.fig \
			  mtpi16.fig \
			  mtpi17.fig \
			  mtpi18.fig \
			  mtpi19.fig \
			  mtpi20.fig \
			  mtpi2_1.fig \
			  mtpi21.fig \
			  mtpi22.fig \
			  mtpi23.fig \
			  mtpi24.fig \
			  mtpi25.fig \
			  mtpi26.fig \
			  mtpi27.fig \
			  mtpi28.fig \
			  mtpi30.fig \
			  mtpi3_10.fig \
			  mtpi3_11.fig \
			  mtpi3_12.fig \
			  mtpi3_13.fig \
			  mtpi3_14.fig \
			  mtpi3_15.fig \
			  mtpi3_1.fig \
			  mtpi31.fig \
			  mtpi3_2.fig \
			  mtpi32.fig \
			  mtpi3_3.fig \
			  mtpi33.fig \
			  mtpi3_4.fig \
			  mtpi34.fig \
			  mtpi3_5.fig \
			  mtpi35.fig \
			  mtpi3_6.fig \
			  mtpi36.fig \
			  mtpi3_7.fig \
			  mtpi37.fig \
			  mtpi3_8.fig \
			  mtpi38.fig \
			  mtpi3_9.fig \
			  mtpi39.fig \
			  mtpi40.fig \
			  mtpi41.fig \
			  mtpi42.fig \
			  mtpi43.fig \
			  mtpi44.fig \
			  mtpi45.fig \
			  mtpi46.fig \
			  mtpi47.fig \
			  mtpi48.fig \
			  mtpi49.fig

TBL_TBLS		= \
			  mtpi_tab01.tbl \
			  mtpi_tab02.tbl \
			  mtpi_tab03.tbl \
			  mtpi_tab04.tbl \
			  mtpi_tab05.tbl \
			  mtpi_tab06.tbl \
			  mtpi_tab07.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am


## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.2  2007/08/13 15:22:04  brian
## - header updates, added references
##
## Revision 0.9.2.1  2007/06/27 08:42:19  brian
## - added MTPI spec document
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
