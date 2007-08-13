## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2007/08/13 15:22:09 $
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
# Last Modified $Date: 2007/08/13 15:22:09 $ by $Author: brian $
#
# =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= tci.texi

tci_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/fdl.texi

if MAINTAINER_MODE
tci_TEXINFOS		+= \
			  texi/args.texi
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(tci_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			tci_fi10.fig \
			tci_fi11.fig \
			tci_fi12.fig \
			tci_fi13.fig \
			tci_fi14.fig \
			tci_fi15.fig \
			tci_fi16.fig \
			tci_fi17.fig \
			tci_fi18.fig \
			tci_fig1.fig \
			tci_fig2.fig \
			tci_fig3.fig \
			tci_fig4.fig \
			tci_fig5.fig \
			tci_fig6.fig \
			tci_fig7.fig \
			tci_fig8.fig \
			tci_fig9.fig \
			tci_fig.fig

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.17  2007/08/13 15:22:09  brian
## - header updates, added references
##
## Revision 0.9.2.16  2007/03/14 04:28:03  brian
## - typo
##
## Revision 0.9.2.15  2007/03/14 04:23:14  brian
## - do not depend on args.texi or discheck target fails
##
## Revision 0.9.2.14  2007/03/06 21:26:17  brian
## - final updates and corrections to build process for release
##
## Revision 0.9.2.13  2006/08/26 09:17:20  brian
## - better release file generation
##
## Revision 0.9.2.12  2006/08/22 12:44:19  brian
## - documentation updates
##
## Revision 0.9.2.11  2006/03/20 13:50:30  brian
## - get automake to run clean
##
## Revision 0.9.2.10  2006/03/20 13:36:29  brian
## - added checks for Development
##
## Revision 0.9.2.9  2006/03/06 11:38:19  brian
## - updated headers
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
