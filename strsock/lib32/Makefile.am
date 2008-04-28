# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-28 22:33:31 $
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
# Last Modified $Date: 2008-04-28 22:33:31 $ by $Author: brian $
#
# =============================================================================

include ../am/openss7.am

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=
pkglibexec32_PROGRAMS		=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

MACHINE_FLAGS			= -m32
LIB_INCLUDES			= -I$(top_builddir)/src/lib -I$(top_srcdir)/src/lib $(PKG_INCLUDES)

pkglibexec32_PROGRAMS		+= test-sockpath

test_sockpath_SOURCES		= $(top_srcdir)/src/test/test-sockpath.c
test_sockpath_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_sockpath_LDADD		= libsockpath.la $(STREAMS_LDADD32)
test_sockpath_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sockpath_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sockpath_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-socket

test_socket_SOURCES		= $(top_srcdir)/src/test/test-socket.c
test_socket_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_socket_LDADD		= libsocket.la $(STREAMS_LDADD32)
test_socket_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_socket_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_socket_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-socklib

test_socklib_SOURCES		= $(top_srcdir)/src/test/test-socklib.c
test_socklib_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_socklib_LDADD		= libsocklib.la $(STREAMS_LDADD32)
test_socklib_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_socklib_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_socklib_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sockmod

test_sockmod_SOURCES		= $(top_srcdir)/src/test/test-sockmod.c
test_sockmod_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_sockmod_LDADD		= libsocklib.la $(STREAMS_LDADD32)
test_sockmod_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sockmod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sockmod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-socksys

test_socksys_SOURCES		= $(top_srcdir)/src/test/test-socksys.c
test_socksys_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_socksys_LDADD		= libsocket.la $(STREAMS_LDADD32)
test_socksys_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_socksys_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_socksys_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libsockpath.la

libsockpath_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/sockpath.c \
				  $(top_srcdir)/src/lib/sockpath.h \
				  $(top_srcdir)/src/include/sockdb.h \
				  $(top_srcdir)/src/include/sys/sockpath.h
libsockpath_la_DEPENDENCIES	= $(top_srcdir)/src/lib/sockpath.VERSION
libsockpath_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION \
				  $(MACHINE_FLAGS)
libsockpath_la_CPPFLAGS		= $(LIB_INCLUDES) $(USER_CPPFLAGS)
libsockpath_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsockpath_la_LIBADD		=

lib32_LTLIBRARIES		+= libsocket.la

libsocket_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/socket.c
libsocket_la_DEPENDENCIES	= $(top_srcdir)/src/lib/socket.VERSION
libsocket_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  $(STREAMS_LDFLAGS32) -static -lc -shared $(MACHINE_FLAGS)
libsocket_la_CPPFLAGS		= $(LIB_INCLUDES) $(USER_CPPFLAGS)
libsocket_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsocket_la_LIBADD		= $(STREAMS_LDADD32)

lib32_LTLIBRARIES		+= libsocklib.la

libsocklib_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/socklib.c
libsocklib_la_DEPENDENCIES	= $(top_srcdir)/src/lib/socklib.VERSION
libsocklib_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  $(NSL_LDFLAGS32) -static -lc -shared $(MACHINE_FLAGS)
libsocklib_la_CPPFLAGS		= $(LIB_INCLUDES) $(USER_CPPFLAGS)
libsocklib_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsocklib_la_LIBADD		= $(NSL_LDADD32) libsockpath.la

## RPM_BUILD_USER
endif

## RPM_BUILD_ARCH
endif

##
## Rules for building libraries
##
include ../am/libraries.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.6  2008-04-28 22:33:31  brian
# - updated headers for release
#
# Revision 0.9.2.5  2007/08/14 05:17:11  brian
# - GPLv3 header update
#
# Revision 0.9.2.4  2007/03/13 06:17:24  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.3  2006/10/03 13:53:07  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.2  2006/09/29 11:50:47  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.1  2006/09/17 23:09:12  brian
# - added lib32 compatibility directories
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
