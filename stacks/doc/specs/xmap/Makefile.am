## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2009-03-13 11:20:24 $
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
# Last Modified $Date: 2009-03-13 11:20:24 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= xmap.texi

xmap_TEXINFOS	= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/fdl.texi

if MAINTAINER_MODE
if AMDEP
xmap_TEXINFOS		+= \
			  texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(xmap_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= 

TBL_TBLS		= \
			  xmap_tab001.tbl \
			  xmap_tab002.tbl \
			  xmap_tab003.tbl \
			  xmap_tab004.tbl \
			  xmap_tab005.tbl \
			  xmap_tab006.tbl \
			  xmap_tab007.tbl \
			  xmap_tab008.tbl \
			  xmap_tab009.tbl \
			  xmap_tab010.tbl \
			  xmap_tab011.tbl \
			  xmap_tab012.tbl \
			  xmap_tab013.tbl \
			  xmap_tab014.tbl \
			  xmap_tab015.tbl \
			  xmap_tab016.tbl \
			  xmap_tab017.tbl \
			  xmap_tab018.tbl \
			  xmap_tab019.tbl \
			  xmap_tab020.tbl \
			  xmap_tab021.tbl \
			  xmap_tab022.tbl \
			  xmap_tab023.tbl \
			  xmap_tab024.tbl \
			  xmap_tab025.tbl \
			  xmap_tab026.tbl \
			  xmap_tab027.tbl \
			  xmap_tab028.tbl \
			  xmap_tab029.tbl \
			  xmap_tab030.tbl \
			  xmap_tab031.tbl \
			  xmap_tab032.tbl \
			  xmap_tab033.tbl \
			  xmap_tab034.tbl \
			  xmap_tab035.tbl \
			  xmap_tab036.tbl \
			  xmap_tab037.tbl \
			  xmap_tab038.tbl \
			  xmap_tab039.tbl \
			  xmap_tab040.tbl \
			  xmap_tab041.tbl \
			  xmap_tab042.tbl \
			  xmap_tab043.tbl \
			  xmap_tab044.tbl \
			  xmap_tab045.tbl \
			  xmap_tab046.tbl \
			  xmap_tab047.tbl \
			  xmap_tab048.tbl \
			  xmap_tab049.tbl \
			  xmap_tab050.tbl \
			  xmap_tab051.tbl \
			  xmap_tab052.tbl \
			  xmap_tab053.tbl \
			  xmap_tab054.tbl \
			  xmap_tab055.tbl \
			  xmap_tab056.tbl \
			  xmap_tab057.tbl \
			  xmap_tab058.tbl \
			  xmap_tab059.tbl \
			  xmap_tab060.tbl \
			  xmap_tab061.tbl \
			  xmap_tab062.tbl \
			  xmap_tab063.tbl \
			  xmap_tab064.tbl \
			  xmap_tab065.tbl \
			  xmap_tab066.tbl \
			  xmap_tab067.tbl \
			  xmap_tab068.tbl \
			  xmap_tab069.tbl \
			  xmap_tab070.tbl \
			  xmap_tab071.tbl \
			  xmap_tab072.tbl \
			  xmap_tab073.tbl \
			  xmap_tab074.tbl \
			  xmap_tab075.tbl \
			  xmap_tab076.tbl \
			  xmap_tab077.tbl \
			  xmap_tab078.tbl \
			  xmap_tab079.tbl \
			  xmap_tab080.tbl \
			  xmap_tab081.tbl \
			  xmap_tab082.tbl \
			  xmap_tab083.tbl \
			  xmap_tab084.tbl \
			  xmap_tab085.tbl \
			  xmap_tab086.tbl \
			  xmap_tab087.tbl \
			  xmap_tab088.tbl \
			  xmap_tab089.tbl \
			  xmap_tab090.tbl \
			  xmap_tab091.tbl \
			  xmap_tab092.tbl \
			  xmap_tab093.tbl \
			  xmap_tab094.tbl \
			  xmap_tab095.tbl \
			  xmap_tab096.tbl \
			  xmap_tab097.tbl \
			  xmap_tab098.tbl \
			  xmap_tab099.tbl \
			  xmap_tab100.tbl \
			  xmap_tab101.tbl \
			  xmap_tab102.tbl \
			  xmap_tab103.tbl \
			  xmap_tab104.tbl \
			  xmap_tab105.tbl \
			  xmap_tab106.tbl \
			  xmap_tab107.tbl \
			  xmap_tab108.tbl \
			  xmap_tab109.tbl \
			  xmap_tab110.tbl \
			  xmap_tab111.tbl \
			  xmap_tab112.tbl \
			  xmap_tab113.tbl \
			  xmap_tab114.tbl \
			  xmap_tab115.tbl \
			  xmap_tab116.tbl \
			  xmap_tab117.tbl \
			  xmap_tab118.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.3  2009-03-13 11:20:24  brian
## - doc and header updates
##
## Revision 0.9.2.2  2009-03-12 15:29:57  brian
## - syntax corrections
##
## Revision 0.9.2.1  2009-03-12 14:52:32  brian
## - added XMAP documentation
##
## Revision 0.9.2.21  2008-11-26 12:23:05  brian
## - correction for FDL update
##
## Revision 0.9.2.20  2008-11-26 12:01:32  brian
## - updated to FDL 1.3
##
## Revision 0.9.2.19  2008-08-31 06:57:13  brian
## - do not customize docs unless maintainer modes and dependency tracking
##
## Revision 0.9.2.18  2008-04-29 07:10:38  brian
## - updating headers for release
##
## Revision 0.9.2.17  2007/08/13 15:22:03  brian
## - header updates, added references
##
## Revision 0.9.2.16  2007/03/14 04:28:01  brian
## - typo
##
## Revision 0.9.2.15  2007/03/14 04:23:13  brian
## - do not depend on args.texi or discheck target fails
##
## Revision 0.9.2.14  2007/03/06 21:26:15  brian
## - final updates and corrections to build process for release
##
## Revision 0.9.2.13  2006/08/26 09:17:13  brian
## - better release file generation
##
## Revision 0.9.2.12  2006/08/22 12:44:16  brian
## - documentation updates
##
## Revision 0.9.2.11  2006/03/20 13:50:29  brian
## - get automake to run clean
##
## Revision 0.9.2.10  2006/03/20 13:36:28  brian
## - added checks for Development
##
## Revision 0.9.2.9  2006/03/06 11:38:17  brian
## - updated headers
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
