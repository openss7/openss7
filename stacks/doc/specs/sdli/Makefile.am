## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-08-31 06:57:14 $
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
# Last Modified $Date: 2008-08-31 06:57:14 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= sdli.texi

sdli_TEXINFOS		= \
			  lmi.h \
			  sdli.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
if AMDEP
sdli_TEXINFOS		+= \
			  texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(sdli_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			  lmi02.fig \
			  lmi03.fig \
			  lmi04.fig \
			  lmi05.fig \
			  lmi06.fig \
			  lmi07.fig \
			  lmi08.fig \
			  lmi09.fig \
			  lmi10.fig \
			  lmi11.fig \
			  lmi13.fig \
			  lmi15.fig \
			  lmi16.fig \
			  sdli01.fig \
			  sdli02.fig \
			  sdli03.fig \
			  sdli04.fig \
			  sdli05.fig \
			  sdli06.fig \
			  sdli07.fig \
			  sdli08.fig \
			  sdli09.fig \
			  sdli10.fig \
			  sdli11.fig \
			  sdli12.fig \
			  sdli13.fig \
			  sdli14.fig \
			  sdli15.fig \
			  sdli16.fig

TBL_TBLS		= \
			  lmit01.tbl \
			  sdlit01.tbl \
			  sdlit02.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am


## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.6  2008-08-31 06:57:14  brian
## - do not customize docs unless maintainer modes and dependency tracking
##
## Revision 0.9.2.5  2008-04-29 07:10:39  brian
## - updating headers for release
##
## Revision 0.9.2.4  2007/08/14 12:17:01  brian
## - GPLv3 header updates
##
## Revision 0.9.2.3  2007/08/13 15:22:07  brian
## - header updates, added references
##
## Revision 0.9.2.2  2007/07/09 09:23:04  brian
## - working up SDLI specification
##
## Revision 0.9.2.1  2007/07/04 08:24:57  brian
## - added new files
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
