# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.30 $) $Date: 2007/03/14 04:23:11 $
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
# Last Modified $Date: 2007/03/14 04:23:11 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= strxnet.texi libxnet.texi

strxnet_TEXINFOS	= \
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
			  texi/gpl.texi \
			  texi/uitpi.texi \
			  texi/quick.texi \
			  texi/install.texi \
			  texi/testsuite.texi \
			  texi/lesser.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/compat.texi \
			  $(top_srcdir)/scripts/texi/mail.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

libxnet_TEXINFOS	= \
			  texi/fdl.texi \
			  texi/lesser.texi \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/funding.texi

if MAINTAINTER_MODE
strxnet_TEXINFOS	+= \
			  texi/args.texi
libxnet_TEXINFOS	+= \
			  texi/args.texi
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(strxnet_TEXINFOS) \
			  $(libxnet_TEXINFOS)

## DEVELOPMENT
endif

include ../../am/info.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.30  2007/03/14 04:23:11  brian
# - do not depend on args.texi or discheck target fails
#
# Revision 0.9.2.29  2007/03/06 21:46:36  brian
# - final updates and corrections to build process for release
#
# Revision 0.9.2.28  2006/10/02 11:32:23  brian
# - changes to get master builds working for RPM and DEB
# - added outside licenses to package documentation
# - added LICENSE automated release file
# - copy MANUAL to source directory
# - add and remove devices in -dev debian subpackages
# - get debian rules working better
# - release library version files
# - added notes to debian changelog
# - corrections for cooked manual pages in spec files
# - added release documentation to spec and rules files
# - copyright header updates
# - moved controlling tty checks in stream head
# - missing some defines for LiS build in various source files
# - added OSI headers to striso package
# - added includes and manual page paths to acincludes for various packages
# - added sunrpc, uidlpi, uinpi and uitpi licenses to documentation and release
#   files
# - moved pragma weak statements ahead of declarations
# - changes for master build of RPMS and DEBS with LiS
#
# Revision 0.9.2.27  2006/09/18 01:18:47  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.26  2006/08/27 12:27:17  brian
# - finalizing auto release files
#
# Revision 0.9.2.25  2006/08/26 14:46:49  brian
# - update todo documentation
#
# Revision 0.9.2.24  2006/08/26 09:20:03  brian
# - better release file generation
#
# Revision 0.9.2.23  2006/08/22 12:57:36  brian
# - updated documentation
#
# Revision 0.9.2.22  2006/03/20 13:50:36  brian
# - get automake to run clean
#
# Revision 0.9.2.21  2006/03/20 13:36:35  brian
# - added checks for Development
#
# Revision 0.9.2.20  2006/03/06 11:39:10  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
