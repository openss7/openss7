# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/09/02 12:24:02 $
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
# Last Modified $Date: 2004/09/02 12:24:02 $ by $Author: brian $
#
# =============================================================================

if PUBLIC_RELEASE

if RPM_BUILD_USER

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp

libexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(INET_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= $(STREAMS_LDADD)
test_inet_raw_CPPFLAGS		= $(INET_INCLUDES) $(USER_CPPFLAGS)
test_inet_raw_CFLAGS		= $(USER_CFLAGS)

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= $(STREAMS_LDADD)
test_inet_tcp_CPPFLAGS		= $(INET_INCLUDES) $(USER_CPPFLAGS)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= $(STREAMS_LDADD)
test_inet_udp_CPPFLAGS		= $(INET_INCLUDES) $(USER_CPPFLAGS)
test_inet_udp_CFLAGS		= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libINETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libINETmods_a_SOURCES	= \
				  src/drivers/inet.c \
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/compat.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h \
				  ./config.h ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libINETmods_a_SOURCES)

nodist_libINETmods_a_SOURCES	= $(mods_source_FILES)

libINETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libINETmods_a_CPPFLAGS		= \
				  -I./src/include/os7 \
				  -I$(srcdir)/src/include/os7 \
				  -I./src/modules \
				  -I$(srcdir)/src/modules \
				  -I./src/drivers \
				  -I$(srcdir)/src/drivers \
				  -imacros strconf.h \
				  $(INET_INCLUDES) \
				  $(KERNEL_CPPFLAGS) \
				  $(KERNEL_MODFLAGS)
libINETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

KERNEL_MODULES			= $(libINETmods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

endif

else

if RPM_BUILD_USER

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp

libexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(INET_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= $(STREAMS_LDADD)
test_inet_raw_CPPFLAGS		= $(INET_INCLUDES) $(USER_CPPFLAGS)
test_inet_raw_CFLAGS		= $(USER_CFLAGS)

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= $(STREAMS_LDADD)
test_inet_tcp_CPPFLAGS		= $(INET_INCLUDES) $(USER_CPPFLAGS)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= $(STREAMS_LDADD)
test_inet_udp_CPPFLAGS		= $(INET_INCLUDES) $(USER_CPPFLAGS)
test_inet_udp_CFLAGS		= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libINETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libINETmods_a_SOURCES	= \
				  src/drivers/inet.c \
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/compat.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h \
				  ./config.h ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libINETmods_a_SOURCES)

nodist_libINETmods_a_SOURCES	= $(mods_source_FILES)

libINETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libINETmods_a_CPPFLAGS		= \
				  -I./src/include/os7 \
				  -I$(srcdir)/src/include/os7 \
				  -I./src/modules \
				  -I$(srcdir)/src/modules \
				  -I./src/drivers \
				  -I$(srcdir)/src/drivers \
				  -imacros strconf.h \
				  $(INET_INCLUDES) \
				  $(KERNEL_CPPFLAGS) \
				  $(KERNEL_MODFLAGS)
libINETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

KERNEL_MODULES			= $(libINETmods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

endif

endif

inet_source_FILES		= $(mods_source_FILES)

EXTRA_DIST			= $(inet_source_FILES)
dist_noinst_SCRIPTS		= configure.nexusware
EXTRA_SCRIPTS			=

KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KMODCONF_EXT			= $(PACKAGE_TARNAME)
KERNEL_SUBDIR			= $(PACKAGE_TARNAME)

include am/kernel.am

#include am/libraries.am

EXPORT_SRCFILES			=

EXPORT_VERFILES			=

PKGINCL = include/sys/$(PACKAGE_NAME)

GENKSYMS_CPPFLAGS = \
	-I./src/drivers -I$(srcdir)/src/drivers \
	$(INET_INCLUDES) $(KERNEL_CPPFLAGS)

include am/genksyms.am

STRMAKENODES			= @PACKAGE_TARNAME@_mknod

include am/strconf.am

if RPM_BUILD_KERNEL

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

install-devices: $(STRMAKENODES) $(INET_MKDEV)
	./$(STRMAKENODES)
	DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)

uninstall-devices: $(STRMAKENODES) $(INET_RMDEV)
	./$(STRMAKENODES) --remove
	DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)

dist_noinst_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)

endif

SUBDIRS = src doc

#
#   Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion $(PACKAGE_KVERSION)"
AM_SRPMFLAGS =

include am/rpm.am

#
#   General rules
#
include am/openss7.am

# vim: ft=automake
