# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-07-23 08:29:17 $
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
# Last Modified $Date: 2008-07-23 08:29:17 $ by $Author: brian $
#
# =============================================================================

##
##  General rules.
##
include ../am/openss7.am


lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

EXTRA_PROGRAMS			+= 

pkglibexec32_PROGRAMS		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

#test_tirdwr_SOURCES		= $(top_srcdir)/src/test/test-tirdwr.c
#test_tirdwr_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
#test_tirdwr_LDADD		= $(STREAMS_LDADD32)
#test_tirdwr_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
#test_tirdwr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#test_tirdwr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

#pkglibexec32_PROGRAMS		+= test-tirdwr

libsx25_la_SOURCES		= $(top_srcdir)/src/lib/sx25/padent.c \
				  $(top_srcdir)/src/lib/sx25/x25config.c \
				  $(top_srcdir)/src/lib/sx25/xhostent.c \
				  $(top_srcdir)/src/include/netx25/x25_control.h \
				  $(top_srcdir)/src/include/netx25/x25_proto.h \
				  $(top_srcdir)/src/include/netx25/xnetdb.h
libsx25_la_DEPENDENCIES		= $(top_srcdir)/src/lib/sx25/sx25.VERSION
libsx25_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sx25/sx25.VERSION \
				  $(STREAMS_LDFLAGS32) $(STREAMS_LDADD32) $(MACHINE_FLAGS)
libsx25_la_CPPFLAGS		= -I$(top_srcdir)/src/lib/sx25 -I$(top_builddir)/src/lib/sx25 \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/src/include -I$(top_builddir)/src/include
libsx25_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsx25_la_LIBADD		=

lib32_LTLIBRARIES		+= libsx25.la

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.2  2008-07-23 08:29:17  brian
# - updated references and support for 2.6.18-92.1.6.el5 kernel
#
# Revision 0.9.2.1  2008-05-03 21:22:39  brian
# - updates for release
#
# =============================================================================
