# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/08/15 05:10:26 $
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
# Last Modified $Date: 2007/08/15 05:10:26 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= sctp.texi

sctp_TEXINFOS		= \
			  configure.texi \
			  descrip.texi \
			  news.texi \
			  prerequisites.texi \
			  releases.texi \
			  rpm.texi \
			  todo.texi \
			  tree.texi \
			  sctp.7.texi \
			  bugs.texi \
			  texi/fdl.texi \
			  texi/gpl3.texi \
			  texi/quick.texi \
			  texi/install.texi \
			  texi/lgpl3.texi \
			  texi/testsuite.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/compat.texi \
			  $(top_srcdir)/scripts/texi/mail.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

if MAINTAINER_MODE
sctp_TEXINFOS		+= \
			  texi/args.texi
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(sctp_TEXINFOS)

## DEVELOPMENT
endif

include ../../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.20  2007/08/15 05:10:26  brian
# - GPLv3 updates
#
# Revision 0.9.2.19  2007/08/14 09:42:23  brian
# - GPLv3 header update
#
# Revision 0.9.2.18  2007/03/14 04:22:57  brian
# - do not depend on args.texi or discheck target fails
#
# Revision 0.9.2.17  2007/03/06 21:23:33  brian
# - final updates and corrections to build process for release
#
# Revision 0.9.2.16  2006/09/18 01:18:34  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.15  2006/08/27 12:26:34  brian
# - finalizing auto release files
#
# Revision 0.9.2.14  2006/08/26 14:46:31  brian
# - update todo documentation
#
# Revision 0.9.2.13  2006/08/26 09:16:35  brian
# - better release file generation
#
# Revision 0.9.2.12  2006/08/22 12:31:41  brian
# - changes to documentation, rationalised sctp to strsctp driver
#
# Revision 0.9.2.11  2006/03/29 11:27:45  brian
# - corrections for rerelease, see ChangeLog
#
# Revision 0.9.2.10  2006/03/20 13:50:25  brian
# - get automake to run clean
#
# Revision 0.9.2.9  2006/03/20 13:36:25  brian
# - added checks for Development
#
# Revision 0.9.2.8  2006/03/06 11:37:58  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
