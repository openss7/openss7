## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 07:10:40 $
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
# Last Modified $Date: 2008-04-29 07:10:40 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= sli.texi

sli_TEXINFOS		= \
			  lmi.h \
			  sli.h \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
## MAINTAINER_MODE

sli_TEXINFOS		+= \
			  texi/args.texi

## MAINTAINER_MODE
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(sli_TEXINFOS)

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
			sli01.fig \
			sli02.fig \
			sli03.fig \
			sli04.fig \
			sli05.fig \
			sli06.fig \
			sli07.fig \
			sli08.fig \
			sli09.fig \
			sli10.fig \
			sli11.fig \
			sli12.fig \
			sli13.fig \
			sli14.fig \
			sli15.fig \
			sli16.fig \
			sli17.fig \
			sli18.fig \
			sli19.fig \
			sli20.fig \
			sli21.fig \
			sli22.fig \
			sli23.fig \
			sli24.fig \
			sli25.fig \
			sli26.fig \
			sli27.fig \
			sli28.fig \
			sli29.fig \
			sli30.fig \
			sli31.fig \
			sli32.fig \
			sli33.fig \
			sli34.fig

TBL_TBLS		= \
			lmit01.tbl \
			slit01.tbl \
			slit02.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am


## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.5  2008-04-29 07:10:40  brian
## - updating headers for release
##
## Revision 0.9.2.4  2007/08/14 12:17:05  brian
## - GPLv3 header updates
##
## Revision 0.9.2.3  2007/08/13 15:22:09  brian
## - header updates, added references
##
## Revision 0.9.2.2  2007/07/09 09:04:51  brian
## - working up SLI specification
##
## Revision 0.9.2.1  2007/07/04 08:24:58  brian
## - added new files
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
