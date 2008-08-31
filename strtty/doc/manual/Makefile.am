# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2008-08-31 06:57:19 $
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
# Last Modified $Date: 2008-08-31 06:57:19 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= strtty.texi

strtty_TEXINFOS		= \
			  configure.texi \
			  descrip.texi \
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
strtty_TEXINFOS		+= \
			  texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(strtty_TEXINFOS)

## DEVELOPMENT
endif

include ../../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.14  2008-08-31 06:57:19  brian
# - do not customize docs unless maintainer modes and dependency tracking
#
# Revision 0.9.2.13  2008-08-03 02:44:02  brian
# - passing distcheck on wilbur
#
# Revision 0.9.2.12  2008-04-28 17:46:01  brian
# - updates for release
#
# Revision 0.9.2.11  2008-04-10 23:24:17  brian
# - distribute agpl3.texi
#
# Revision 0.9.2.10  2007/08/15 05:34:47  brian
# - GPLv3 updates
#
# Revision 0.9.2.9  2007/08/14 03:04:25  brian
# - GPLv3 header update
#
# Revision 0.9.2.8  2007/03/14 04:27:41  brian
# - typo
#
# Revision 0.9.2.7  2007/03/14 04:23:08  brian
# - do not depend on args.texi or discheck target fails
#
# Revision 0.9.2.6  2007/03/06 21:24:17  brian
# - final updates and corrections to build process for release
#
# Revision 0.9.2.5  2006/09/18 01:18:44  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.4  2006/08/27 12:27:14  brian
# - finalizing auto release files
#
# Revision 0.9.2.3  2006/08/26 14:46:46  brian
# - update todo documentation
#
# Revision 0.9.2.2  2006/08/26 09:19:32  brian
# - better release file generation
#
# Revision 0.9.2.1  2006/08/23 09:53:14  brian
# - started STREAMS Terminals package
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
