# =============================================================================
#
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-07-13 07:13:35 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
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
# Last Modified $Date: 2009-07-13 07:13:35 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

tcl_pkg_INDEX		= pkgIndex.tcl

if RPM_BUILD_USER
if PKG_BUILD_ARCH
pkgtcllib_DATA		= pkgIndex.tcl
endif
endif

tcl_c_MODULES		= xti/xti.i
tcl_cxx_MODULES		= streams/streams.i tcap/tcap.i

tcl_pkg_INDEXES		= xti/pkgIndex.tcl \
			  streams/pkgIndex.tcl \
			  tcap/pkgIndex.tcl

xtitcldir		= $(pkgtcllibdir)/xti

xti_la_SOURCES		= xti/xti_wrap.c
xti_la_CPPFLAGS		= -I$(builddir)/xti -I$(srcdir)/xti $(PKG_INCLUDES) $(USER_CPPFLAGS) $(TCL_CPPFLAGS) -DMODULE
xti_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
xti_la_LIBADD		= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
xti_la_LDFLAGS		= -module -export-dynamic -release $(VERSION) -shared

if RPM_BUILD_USER
if PKG_BUILD_ARCH
xtitcl_DATA		= xti/pkgIndex.tcl
xtitcl_LTLIBRARIES	= xti.la
endif
endif

streamstcldir		= $(pkgtcllibdir)/streams

streams_la_SOURCES	= streams/streams_wrap.cxx
streams_la_CPPFLAGS	= -I$(builddir)/streams -I$(srcdir)/streams $(PKG_INCLUDES) $(USER_CPPFLAGS) $(TCL_CPPFLAGS) -DMODULE
streams_la_CXXFLAGS	= $(USER_CFLAGS) -Wno-unused $(USER_DFLAGS)
streams_la_LIBADD	= $(top_builddir)/libstreams.la
streams_la_LDFLAGS	= -module -export-dynamic -release $(VERSION) -shared

if RPM_BUILD_USER
if PKG_BUILD_ARCH
streamstcl_DATA		= streams/pkgIndex.tcl
streamstcl_LTLIBRARIES	= streams.la
endif
endif

tcaptcldir		= $(pkgtcllibdir)/tcap

tcap_la_SOURCES		= tcap/tcap_wrap.cxx tcap/tcap.hh
tcap_la_CPPFLAGS	= -I$(builddir)/tcap -I$(srcdir)/tcap $(PKG_INCLUDES) $(USER_CPPFLAGS) $(TCL_CPPFLAGS) -DMODULE
tcap_la_CXXFLAGS	= $(USER_CFLAGS) -Wno-unused $(USER_DFLAGS)
tcap_la_LIBADD		= $(top_builddir)/libstreams.la
tcap_la_LDFLAGS		= -module -export-dynamic -release $(VERSION) -shared

if RPM_BUILD_USER
if PKG_BUILD_ARCH
tcaptcl_DATA		= tcap/pkgIndex.tcl
tcaptcl_LTLIBRARIES	= tcap.la
endif
endif

include ../../am/swig.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 1.1.2.1  2009-07-13 07:13:35  brian
# - changes for multiple distro build
#
# =============================================================================
# vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
