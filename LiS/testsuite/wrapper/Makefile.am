# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.4.2 $) $Date: 2005/04/02 03:23:20 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
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
# Last Modified $Date: 2005/04/02 03:23:20 $ by $Author: brian $
#
# =============================================================================
#

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

$(top_builddir)/$(STRCONF_CONFIG):
	$(MAKE) -C $(top_builddir) $(AM_MAKEFLAGS) $(STRCONF_CONFIG)

$(top_builddir)/libLiS.la:
	$(MAKE) -C $(top_builddir) $(AM_MAKEFLAGS) libLiS.la

check_PROGRAMS		= wrapper

wrapper_SOURCES		= wrapper.c
wrapper_DEPENDENCIES	= $(top_builddir)/$(STRCONF_CONFIG) \
			  $(top_builddir)/libLiS.la
wrapper_LDFLAGS		= $(top_builddir)/libLiS.la
wrapper_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
wrapper_CFLAGS		= $(USER_CFLAGS)

## PKG_BUILD_ARCH
endif

# vim: ft=automake
