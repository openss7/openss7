# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.6.24 $) $Date: 2008-08-31 06:57:10 $
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
# Last Modified $Date: 2008-08-31 06:57:10 $ by $Author: brian $
#
# =============================================================================


include ../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= LiS.texi

LiS_TEXINFOS		= \
			  configure.texi \
			  descrip.texi \
			  install.texi \
			  news.texi \
			  prerequisites.texi \
			  releases.texi \
			  rpm.texi \
			  todo.texi \
			  tree.texi \
			  bugs.texi \
			  texi/agpl3.texi \
			  texi/bsd.texi \
			  texi/decmit.texi \
			  texi/fdl12.texi \
			  texi/fdl.texi \
			  texi/gpl3.texi \
			  texi/gpl.texi \
			  texi/hp.texi \
			  texi/install.texi \
			  texi/isode.texi \
			  texi/lesser.texi \
			  texi/lgpl3.texi \
			  texi/mit.texi \
			  texi/quick.texi \
			  texi/sunrpc.texi \
			  texi/testsuite.texi \
			  texi/uidlpi.texi \
			  texi/uinpi.texi \
			  texi/ui.texi \
			  texi/uitpi.texi \
			  texi/uslbsd.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/compat.texi \
			  $(top_srcdir)/scripts/texi/mail.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

if MAINTAINER_MODE
if AMDEP
LiS_TEXINFOS		+= \
			  texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(LiS_TEXINFOS)

## DEVELOPMENT
endif

include ../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 1.1.6.24  2008-08-31 06:57:10  brian
# - do not customize docs unless maintainer modes and dependency tracking
#
# Revision 1.1.6.23  2008-08-03 02:43:51  brian
# - passing distcheck on wilbur
#
# Revision 1.1.6.22  2008-04-29 08:33:23  brian
# - update headers for Affero release
#
# Revision 1.1.6.21  2008-04-28 07:41:04  brian
# - updates for release
#
# Revision 1.1.6.20  2007/08/15 04:58:09  brian
# - GPLv3 updates
#
# Revision 1.1.6.19  2007/08/14 10:47:17  brian
# - GPLv3 header update
#
# Revision 1.1.6.18  2007/03/14 04:22:53  brian
# - do not depend on args.texi or discheck target fails
#
# Revision 1.1.6.17  2007/03/06 21:23:20  brian
# - final updates and corrections to build process for release
#
# Revision 1.1.6.16  2006/09/18 01:18:30  brian
# - updated documentation and include file makefiles
#
# Revision 1.1.6.15  2006/08/29 11:51:20  brian
# - removed useless files
#
# Revision 1.1.6.14  2006/08/27 12:26:26  brian
# - finalizing auto release files
#
# Revision 1.1.6.13  2006/08/26 14:46:26  brian
# - update todo documentation
#
# Revision 1.1.6.12  2006/08/26 09:15:57  brian
# - better release file generation
#
# Revision 1.1.6.11  2006/08/22 12:21:06  brian
# - updates to documentation and for release
#
# Revision 1.1.6.10  2006/07/03 02:59:00  brian
# - updated documentation for release
#
# Revision 1.1.6.9  2006/03/20 13:50:23  brian
# - get automake to run clean
#
# Revision 1.1.6.8  2006/03/20 13:36:23  brian
# - added checks for Development
#
# Revision 1.1.6.7  2006/03/06 11:37:44  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
