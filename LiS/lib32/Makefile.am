# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/09/29 11:50:00 $
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
# Last Modified $Date: 2006/09/29 11:50:00 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.4  2006/09/29 11:50:00  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.3  2006/09/25 12:17:26  brian
# - updates for distcheck
#
# Revision 0.9.2.2  2006/09/24 21:36:50  brian
# - added lib files
#
# Revision 0.9.2.1  2006/09/17 23:09:04  brian
# - added lib32 compatibility directories
#
# =============================================================================

##
## General rules
##
include ../am/openss7.am

pkglibexec32_PROGRAMS		=

lib32_LTLIBRARIES		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

pkglibexec32_PROGRAMS		+= timetst

timetst_SOURCES			= $(top_srcdir)/util/timetst.c
timetst_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
timetst_LDFLAGS			= $(MACHINE_FLAGS)
timetst_LDADD			= libLiS.la
timetst_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
timetst_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= strtst

strtst_SOURCES			= $(top_srcdir)/util/strtst.c
strtst_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
strtst_LDFLAGS			= -lpthread $(MACHINE_FLAGS)
strtst_LDADD			= libLiS.la
strtst_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strtst_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= polltst

polltst_SOURCES			= $(top_srcdir)/util/polltst.c
polltst_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
polltst_LDFLAGS			= $(MACHINE_FLAGS)
polltst_LDADD			= libLiS.la
polltst_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
polltst_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= perftest

perftest_SOURCES		= $(top_srcdir)/util/perftest.c
perftest_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
perftest_LDFLAGS		= $(MACHINE_FLAGS)
perftest_LDADD			= libLiS.la
perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
perftest_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-streams

test_streams_SOURCES		= $(top_srcdir)/util/test-streams.c
test_streams_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_streams_LDFLAGS		= -lpthread $(MACHINE_FLAGS)
test_streams_LDADD		= libLiS.la
test_streams_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-fifo

test_fifo_SOURCES		= $(top_srcdir)/util/test-fifo.c
test_fifo_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_fifo_LDFLAGS		= -lpthread $(MACHINE_FLAGS)
test_fifo_LDADD			= libLiS.la
test_fifo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-pipe

test_pipe_SOURCES		= $(top_srcdir)/util/test-pipe.c
test_pipe_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_pipe_LDFLAGS		= -lpthread $(MACHINE_FLAGS)
test_pipe_LDADD			= libLiS.la
test_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sad

test_sad_SOURCES		= $(top_srcdir)/util/test-sad.c
test_sad_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_sad_LDFLAGS		= -lpthread $(MACHINE_FLAGS)
test_sad_LDADD			= libLiS.la
test_sad_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libLiS.la

## non-preload library has pipe()
libLiS_la_SOURCES		= $(top_srcdir)/libc/fattach.c \
				  $(top_srcdir)/libc/fdetach.c \
				  $(top_srcdir)/libc/getmsg.c \
				  $(top_srcdir)/libc/getpmsg.c \
				  $(top_srcdir)/libc/isastream.c \
				  $(top_srcdir)/libc/pipe.c \
				  $(top_srcdir)/libc/putmsg.c \
				  $(top_srcdir)/libc/putpmsg.c \
				  $(top_srcdir)/libc/testcancel.c \
				  $(top_srcdir)/libc/streams.h
libLiS_la_DEPENDENCIES		= $(top_srcdir)/libc/LiS.VERSION
libLiS_la_LDFLAGS		= -version-info 0:2:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/libc/LiS.VERSION \
				  $(MACHINE_FLAGS)
libLiS_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libpLiS.la

## preload library does not have pipe()
libpLiS_la_SOURCES		= $(top_srcdir)/libc/fattach.c \
				  $(top_srcdir)/libc/fdetach.c \
				  $(top_srcdir)/libc/getmsg.c \
				  $(top_srcdir)/libc/getpmsg.c \
				  $(top_srcdir)/libc/isastream.c \
				  $(top_srcdir)/libc/putmsg.c \
				  $(top_srcdir)/libc/putpmsg.c \
				  $(top_srcdir)/libc/testcancel.c \
				  $(top_srcdir)/libc/streams.h
libpLiS_la_DEPENDENCIES		= $(top_srcdir)/libc/LiS.VERSION
libpLiS_la_LDFLAGS		= -version-info 0:2:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/libc/LiS.VERSION \
				  $(MACHINE_FLAGS)
libpLiS_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
## Rules for building libraries
##
include ../am/libraries.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
