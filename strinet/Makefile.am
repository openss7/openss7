# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/02/19 12:19:56 $
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
# Last Modified $Date: 2005/02/19 12:19:56 $ by $Author: brian $
#
# =============================================================================

#
#   General rules
#
include am/openss7.am

if PUBLIC_RELEASE

if RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
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
EXTRA_LIBRARIES			+= 

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

EXTRA_KERNEL_MODULES		+= 

endif

else

if RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
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
EXTRA_LIBRARIES			+= 

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

EXTRA_KERNEL_MODULES		+= 

endif

endif

inet_source_FILES		= $(mods_source_FILES)

EXTRA_DIST			+= $(inet_source_FILES)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+=

export KERNEL_MODULES		= $(libINETmods_a_OBJECTS) $(ADDON_OBJS)
export KERNEL_MODCONF		= $(STRCONF_CONFMOD)
export KERNEL_SUBDIR		= $(PACKAGE_TARNAME)
export KMODCONF_EXT		= $(PACKAGE_TARNAME)
export KERNEL_MODMAPS		= $(STREAMS_MODMAP)

include am/kernel.am

include am/strconf.am

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

dist_noinst_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)

if RPM_BUILD_KERNEL

install-inet-devices: $(INET_MKDEV)
	@if test -r $(DESTDIR)$(krootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV) ; \
	fi

uninstall-inet-devices: $(INET_RMDEV)
	@if test -r $(DESTDIR)$(krootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV) ; \
	fi

else

install-inet-devices:

uninstall-inet-devices:

endif

INSTALL_EXEC_HOOK += install-inet-devices

UNINSTALL_HOOK += uninstall-inet-devices

.PHONY: install-inet-devices uninstall-inet-devices

if RPM_BUILD_USER

SUBDIRS = src doc

endif

#
# Rules for lsms
#
include am/lsm.am

#
# Rules for archives
#
include am/archive.am

#
#   Rules for making rpms
#
AM_RPMFLAGS 			= --define "_kversion $(kversion)"
AM_SRPMFLAGS 			=

include am/rpm.am

#
# Rules for making deb packages
#
#include am/deb.am

# vim: ft=automake
