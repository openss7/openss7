# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:19 $
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
# Last Modified $Date: 2008-07-06 14:58:19 $ by $Author: brian $
#
# =============================================================================

##
##  General rules.
##
include ../am/openss7.am

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

pkglibexec32_PROGRAMS		+= ldltest

ldltest_SOURCES			= $(top_srcdir)/src/test/ldltest.c
ldltest_LDADD			= $(STREAMS_LDADD32) $(STRCOMP_LDADD32)
ldltest_LDFLAGS			= $(STREAMS_LDFLAGS32) $(STRCOMP_LDFLAGS32) $(MACHINE_FLAGS)
ldltest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
ldltest_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-np_ip

test_np_ip_SOURCES		= $(top_srcdir)/src/test/test-np_ip.c
test_np_ip_LDADD		= $(STREAMS_LDADD32) $(STRCOMP_LDADD32)
test_np_ip_LDFLAGS		= $(STREAMS_LDFLAGS32) $(STRCOMP_LDFLAGS32) $(MACHINE_FLAGS)
test_np_ip_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_np_ip_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

#pkglibexec32_PROGRAMS		+= test-ip

#test_ip_SOURCES		= $(top_srcdir)/src/test/test-ip.c
#test_ip_LDADD			= $(STREAMS_LDADD32) $(STRCOMP_LDADD32)
#test_ip_LDFLAGS		= $(STREAMS_LDFLAGS32) $(STRCOMP_LDFLAGS32) $(MACHINE_FLAGS)
#test_ip_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#test_ip_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

libcdiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/cdiapi.c \
				  $(top_srcdir)/src/include/cdiapi.h \
				  $(top_srcdir)/gettext.h
libcdiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/cdiapi.VERSION
libcdiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/cdiapi.VERSION \
				  $(STREAMS_LDFLAGS32) $(STREAMS_LDADD32) $(MACHINE_FLAGS)
libcdiapi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/include -I$(top_builddir)/include
libcdiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libcdiapi_la_LIBADD		= 

lib32_LTLIBRARIES		+= libcdiapi.la

libdlpiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/dlpiapi.c \
				  $(top_srcdir)/src/include/dlpiapi.h \
				  $(top_srcdir)/gettext.h
libdlpiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/dlpiapi.VERSION
libdlpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpiapi.VERSION \
				  $(STREAMS_LDFLAGS32) $(STREAMS_LDADD32) $(MACHINE_FLAGS)
libdlpiapi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/include -I$(top_builddir)/include
libdlpiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libdlpiapi_la_LIBADD		= 

lib32_LTLIBRARIES		+= libdlpiapi.la

libnpiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/npiapi.c \
				  $(top_srcdir)/src/include/npiapi.h \
				  $(top_srcdir)/gettext.h
libnpiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/npiapi.VERSION
libnpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/npiapi.VERSION \
				  $(STREAMS_LDFLAGS32) $(STREAMS_LDADD32) $(MACHINE_FLAGS)
libnpiapi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/include -I$(top_builddir)/include
libnpiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libnpiapi_la_LIBADD		= 

lib32_LTLIBRARIES		+= libnpiapi.la

libdlpi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/libdlpi/dlpi.c \
				  $(top_srcdir)/src/include/libdlpi.h \
				  $(top_srcdir)/gettext.h
libdlpi_la_DEPENDENCIES		= $(top_srcdir)/src/lib/libdlpi/dlpi.VERSION
libdlpi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/libdlpi/dlpi.VERSION \
				  $(STREAMS_LDFLAGS32) $(STREAMS_LDADD32) $(MACHINE_FLAGS)
libdlpi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) \
				  -I$(top_srcdir)/src/lib/libdlpi -I$(top_srcdir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/include -I$(top_builddir)/include
libdlpi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libdlpi_la_LIBADD		= 

lib32_LTLIBRARIES		+= libdlpi.la

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
# Revision 0.9.2.5  2008-07-06 14:58:19  brian
# - improvements
#
# Revision 0.9.2.4  2007/08/14 03:31:03  brian
# - GPLv3 header update
#
# Revision 0.9.2.3  2007/03/13 06:17:32  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.2  2006/09/29 11:51:26  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.1  2006/09/17 23:09:16  brian
# - added lib32 compatibility directories
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
