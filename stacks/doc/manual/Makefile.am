# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.31 $) $Date: 2006/09/18 01:18:35 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2006/09/18 01:18:35 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.31  2006/09/18 01:18:35  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.30  2006/08/27 12:26:38  brian
# - finalizing auto release files
#
# Revision 0.9.2.29  2006/08/26 14:46:32  brian
# - update todo documentation
#
# Revision 0.9.2.28  2006/08/26 09:16:47  brian
# - better release file generation
#
# Revision 0.9.2.27  2006/08/22 12:44:09  brian
# - documentation updates
#
# Revision 0.9.2.26  2006/03/20 13:50:26  brian
# - get automake to run clean
#
# Revision 0.9.2.25  2006/03/20 13:36:25  brian
# - added checks for Development
#
# Revision 0.9.2.24  2006/03/06 11:38:13  brian
# - updated headers
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= strss7.texi

if WITH_XNET
## WITH_XNET
info_TEXINFOS		+= libxnet.texi
## WITH_XNET
endif

EXTRA_info_TEXINFOS	= \
			  strss7.texi \
			  libxnet.texi \
			  configure.texi \
			  descrip.texi \
			  news.texi \
			  prerequisites.texi \
			  releases.texi \
			  rpm.texi \
			  todo.texi \
			  tree.texi \
			  bugs.texi \
			  texi/args.texi \
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

## DEVELOPMENT
endif

include ../../am/info.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
