# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-03 10:46:37 $
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
# Last Modified $Date: 2008-05-03 10:46:37 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

## this must be literal
info_TEXINFOS		+= striso.texi

striso_TEXINFOS		= \
			  configure.texi \
			  descrip.texi \
			  news.texi \
			  prerequisites.texi \
			  releases.texi \
			  rpm.texi \
			  todo.texi \
			  tree.texi \
			  bugs.texi \
			  texi/fdl.texi \
			  texi/agpl3.texi \
			  texi/gpl3.texi \
			  texi/quick.texi \
			  texi/install.texi \
			  texi/testsuite.texi \
			  texi/lgpl3.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/compat.texi \
			  $(top_srcdir)/scripts/texi/mail.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

if MAINTAINER_MODE
striso_TEXINFOS		+= \
			  texi/args.texi
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(striso_TEXINFOS)

## DEVELOPMENT
endif

include ../../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.1  2008-05-03 10:46:37  brian
# - added package files
#
# Revision 0.9.2.14  2008-04-29 00:01:58  brian
# - updated headers for release
#
# Revision 0.9.2.13  2008-04-10 23:24:15  brian
# - distribute agpl3.texi
#
# Revision 0.9.2.12  2007/08/15 05:34:16  brian
# - GPLv3 updates
#
# Revision 0.9.2.11  2007/08/14 07:04:57  brian
# - GNUv3 header update
#
# Revision 0.9.2.10  2007/03/14 04:27:35  brian
# - typo
#
# Revision 0.9.2.9  2007/03/14 04:23:05  brian
# - do not depend on args.texi or discheck target fails
#
# Revision 0.9.2.8  2007/03/06 21:23:55  brian
# - final updates and corrections to build process for release
#
# Revision 0.9.2.7  2006/09/18 01:18:40  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.6  2006/08/27 12:27:06  brian
# - finalizing auto release files
#
# Revision 0.9.2.5  2006/08/26 14:46:41  brian
# - update todo documentation
#
# Revision 0.9.2.4  2006/08/26 09:18:48  brian
# - better release file generation
#
# Revision 0.9.2.3  2006/08/22 12:57:27  brian
# - updated documentation
#
# Revision 0.9.2.2  2006/04/08 03:39:15  brian
# - working up striso package
#
# Revision 0.9.2.1  2006/04/07 22:01:29  brian
# - initial branch
#
# Revision 0.9  2006/04/07 21:56:41  brian
# - initial addition of striso files
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
