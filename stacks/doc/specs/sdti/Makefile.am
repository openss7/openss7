## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/07/09 09:12:59 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, write to the Free Software Foundation, Inc., 675 Mass
## Ave, Cambridge, MA 02139, USA.
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
## Last Modified $Date: 2007/07/09 09:12:59 $ by $Author: brian $
##
## =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= sdti.texi

sdti_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
## MAINTAINER_MODE

sdti_TEXINFOS		+= \
			  texi/args.texi

## MAINTAINER_MODE
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(sdti_TEXINFOS)

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
			sdti01.fig \
			sdti02.fig \
			sdti03.fig \
			sdti04.fig \
			sdti05.fig \
			sdti06.fig \
			sdti07.fig \
			sdti08.fig \
			sdti09.fig \
			sdti10.fig \
			sdti11.fig \
			sdti12.fig \
			sdti13.fig \
			sdti14.fig \
			sdti15.fig \
			sdti16.fig

TBL_TBLS		= \
			lmit01.tbl \
			sdtit01.tbl \
			sdtit02.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am


## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.2  2007/07/09 09:12:59  brian
## - working up SDTI specification
##
## Revision 0.9.2.1  2007/07/04 08:24:57  brian
## - added new files
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
