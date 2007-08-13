# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.53 $) $Date: 2007/08/13 22:45:59 $
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
# Last Modified $Date: 2007/08/13 22:45:59 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= STREAMS.texi STREAMS_FAQ.texi SPG2.texi # SPG.texi porting.texi

STREAMS_TEXINFOS	= \
			  configure.texi \
			  descrip.texi \
			  install.texi \
			  news.texi \
			  prerequisites.texi \
			  releases.texi \
			  rpm.texi \
			  todo.texi \
			  tree.texi \
			  drivers.texi \
			  modules.texi \
			  utilities.texi \
			  bugs.texi \
			  texi/fdl.texi \
			  texi/gpl.texi \
			  texi/quick.texi \
			  texi/install.texi \
			  texi/lesser.texi \
			  texi/testsuite.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/compat.texi \
			  $(top_srcdir)/scripts/texi/mail.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

STREAMS_FAQ_TEXINFOS	= \
			  texi/gpl.texi \
			  texi/fdl.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/funding.texi


SPG2_TEXINFOS		= \
			  texi/gpl.texi \
			  texi/fdl.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

SPG_TEXINFOS		= \
			  texi/gpl.texi \
			  texi/fdl.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

porting_TEXINFOS	= \
			  texi/gpl.texi \
			  texi/fdl.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

if MAINTAINER_MODE
STREAMS_TEXINFOS	+= \
			  texi/args.texi
STREAMS_FAQ_TEXINFOS	+= \
			  texi/args.texi
SPG2_TEXINFOS		+= \
			  texi/args.texi
SPG_TEXINFOS		+= \
			  texi/args.texi
porting_TEXINFOS	+= \
			  texi/args.texi
endif


EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(STREAMS_TEXINFOS) \
			  $(STREAMS_FAQ_TEXINFOS) \
			  $(SPG2_TEXINFOS) \
			  $(SPG_TEXINFOS) \
			  $(porting_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			  SPG_fig01.fig \
			  SPG_fig02.fig \
			  SPG_fig03.fig \
			  SPG_fig04.fig \
			  SPG_fig05.fig \
			  SPG_fig06.fig \
			  SPG_fig07.fig \
			  SPG_fig08.fig \
			  SPG_fig09.fig \
			  SPG_fig10.fig \
			  SPG_fig11.fig \
			  SPG_fig12.fig \
			  SPG_fig13.fig \
			  SPG_fig14.fig \
			  SPG_fig15.fig \
			  SPG_fig15b.fig \
			  SPG_fig16.fig \
			  SPG_fig17.fig \
			  SPG_fig18a.fig \
			  SPG_fig18.fig \
			  SPG_fig19.fig \
			  SPG_fig20.fig \
			  SPG_fig21.fig \
			  SPG_fig22.fig \
			  SPG_fig23.fig \
			  SPG_fig24.fig \
			  SPG_fig25.fig \
			  SPG_fig26.fig \
			  SPG_fig27.fig \
			  SPG_fig28.fig \
			  SPG_fig29.fig \
			  SPG_fig30.fig \
			  SPG_fig31.fig \
			  SPG_fig32.fig \
			  SPG_fig33.fig \
			  SPG_fig34.fig \
			  SPG_fig35.fig \
			  SPG_fig36.fig \
			  SPG_fig37.fig \
			  SPG_fig38.fig \
			  SPG_fig39.fig \
			  SPG_fig40.fig \
			  SPG_fig41.fig \
			  SPG_fig42.fig \
			  SPG_fig43.fig \
			  SPG_fig44.fig \
			  SPG_fig45.fig \
			  SPG_fig46.fig \
			  SPG_fig47.fig \
			  SPG_fig48.fig \
			  SPG_fig49.fig \
			  SPG_fig50.fig \
			  SPG_fig51.fig \
			  SPG_fig52.fig \
			  SPG_fig53.fig \
			  SPG_fig54.fig \
			  SPG_fig55.fig \
			  SPG_fig56.fig

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.53  2007/08/13 22:45:59  brian
# - GPLv3 header updates
#
# Revision 0.9.2.52  2007/03/14 04:27:32  brian
# - typo
#
# Revision 0.9.2.51  2007/03/14 04:23:03  brian
# - do not depend on args.texi or discheck target fails
#
# Revision 0.9.2.50  2007/03/06 21:23:46  brian
# - final updates and corrections to build process for release
#
# Revision 0.9.2.49  2007/01/02 11:10:57  brian
# - removed porting manual from build (see strcompat)
#
# Revision 0.9.2.48  2006/09/18 01:18:38  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.47  2006/08/29 11:51:26  brian
# - removed useless files
#
# Revision 0.9.2.46  2006/08/27 12:26:44  brian
# - finalizing auto release files
#
# Revision 0.9.2.45  2006/08/26 14:46:37  brian
# - update todo documentation
#
# Revision 0.9.2.44  2006/08/26 09:18:10  brian
# - better release file generation
#
# Revision 0.9.2.43  2006/08/22 12:36:45  brian
# - udpates to documentation, tweaks to Stream head
#
# Revision 0.9.2.42  2006/03/20 13:50:32  brian
# - get automake to run clean
#
# Revision 0.9.2.41  2006/03/20 13:36:31  brian
# - added checks for Development
#
# Revision 0.9.2.40  2006/03/06 11:38:36  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
