# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:23:20 $
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
# Last Modified $Date: 2004/06/22 06:23:20 $ by $Author: brian $
#
# =============================================================================

if PUBLIC_RELEASE

if RPM_BUILD_USER

libexec_PROGRAMS			= \
					  test-sctp \
					  test-sctp_n \
					  test-sctp_nc \
					  test-sctp_ns \
					  test-sctp_t

EXTRA_PROGRAMS				= \
					  test-sctp \
					  test-sctp_n \
					  test-sctp_nc \
					  test-sctp_ns \
					  test-sctp_t

sbin_PROGRAMS =				= \
					  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sctp_SOURCES			= src/test/test-sctp.c
test_sctp_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS			= $(USER_CFLAGS)

test_sctp_n_SOURCES			= src/test/test-sctp_n.c
test_sctp_n_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_n_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS			= $(USER_CFLAGS)

test_sctp_nc_SOURCES			= src/test/test-sctp_nc.c
test_sctp_nc_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_nc_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS			= $(USER_CFLAGS)

test_sctp_ns_SOURCES			= src/test/test-sctp_ns.c
test_sctp_ns_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_ns_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS			= $(USER_CFLAGS)

test_sctp_t_SOURCES			= src/test/test-sctp_t.c
test_sctp_t_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_t_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS			= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS				= libSCTPsctp.a
ADDON_OBJS				= streams-sctp.o

noinst_LIBRARIES			= $(ADDON_LIBS)
EXTRA_LIBRARIES				= libSCTPsctp.a

nodist_EXTRA_libSCTPsctp_a_SOURCES	= \
					  src/drivers/sctp.c \
					  src/drivers/sctp_cache.c \
					  src/drivers/sctp_cookie.c \
					  src/drivers/sctp_hash.c \
					  src/drivers/sctp_input.c \
					  src/drivers/sctp_msg.c \
					  src/drivers/sctp_n.c \
					  src/drivers/sctp_output.c \
					  src/drivers/sctp_route.c \
					  src/drivers/sctp_t.c \
					  src/drivers/sctp_cache.h \
					  src/drivers/sctp_cookie.h \
					  src/drivers/sctp_defs.h \
					  src/drivers/sctp.h \
					  src/drivers/sctp_hash.h \
					  src/drivers/sctp_input.h \
					  src/drivers/sctp_msg.h \
					  src/drivers/sctp_n.h \
					  src/drivers/sctp_output.h \
					  src/drivers/sctp_route.h \
					  src/drivers/sctp_t.h \
					  strconf.h
sctp_source_FILES			= $(nodist_EXTRA_libSCTPsctp_a_SOURCES)
nodist_libSCTPsctp_a_SOURCES		= $(sctp_source_FILES)

libSCTPsctp_a_DEPENDENCIES		= config.h strconf.h
libSCTPsctp_a_CPPFLAGS			= \
					  -I./src/include/os7 \
					  -I$(srcdir)/src/include/os7 \
					  -I./src/drivers \
					  -I$(srcdir)/src/drivers \
					  -imacros strconf.h \
					  $(SCTP_INCLUDES) \
					  $(KERNEL_CPPFLAGS) \
					  $(KERNEL_MODFLAGS)
libSCTPsctp_a_CFLAGS			= $(KERNEL_CFLAGS)
libSCTPsctp_a_OBJECTS			= $(libSCTPsctp_a_DEPENDENCIES)

streams-sctp: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^

KERNEL_MODULES				= $(ADDON_OBJS)
EXTRA_KERNEL_MODULES			=

endif

else

if RPM_BUILD_USER

libexec_PROGRAMS			= \
					  test-sctp \
					  test-sctp_n \
					  test-sctp_nc \
					  test-sctp_ns \
					  test-sctp_t

EXTRA_PROGRAMS				= \
					  test-sctp \
					  test-sctp_n \
					  test-sctp_nc \
					  test-sctp_ns \
					  test-sctp_t

sbin_PROGRAMS =				= \
					  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sctp_SOURCES			= src/test/test-sctp.c
test_sctp_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS			= $(USER_CFLAGS)

test_sctp_n_SOURCES			= src/test/test-sctp_n.c
test_sctp_n_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_n_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS			= $(USER_CFLAGS)

test_sctp_nc_SOURCES			= src/test/test-sctp_nc.c
test_sctp_nc_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_nc_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS			= $(USER_CFLAGS)

test_sctp_ns_SOURCES			= src/test/test-sctp_ns.c
test_sctp_ns_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_ns_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS			= $(USER_CFLAGS)

test_sctp_t_SOURCES			= src/test/test-sctp_t.c
test_sctp_t_LDFLAGS			= $(USER_LDFLAGS)
test_sctp_t_CPPFLAGS			= $(SCTP_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS			= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS				= libSCTPsctp.a
ADDON_OBJS				= streams-sctp.o

noinst_LIBRARIES			= $(ADDON_LIBS)
EXTRA_LIBRARIES				= libSCTPsctp.a

nodist_EXTRA_libSCTPsctp_a_SOURCES	= \
					  src/drivers/sctp.c \
					  src/drivers/sctp_cache.c \
					  src/drivers/sctp_cookie.c \
					  src/drivers/sctp_hash.c \
					  src/drivers/sctp_input.c \
					  src/drivers/sctp_msg.c \
					  src/drivers/sctp_n.c \
					  src/drivers/sctp_output.c \
					  src/drivers/sctp_route.c \
					  src/drivers/sctp_t.c \
					  src/drivers/sctp_cache.h \
					  src/drivers/sctp_cookie.h \
					  src/drivers/sctp_defs.h \
					  src/drivers/sctp.h \
					  src/drivers/sctp_hash.h \
					  src/drivers/sctp_input.h \
					  src/drivers/sctp_msg.h \
					  src/drivers/sctp_n.h \
					  src/drivers/sctp_output.h \
					  src/drivers/sctp_route.h \
					  src/drivers/sctp_t.h \
					  strconf.h
sctp_source_FILES			= $(nodist_EXTRA_libSCTPsctp_a_SOURCES)
nodist_libSCTPsctp_a_SOURCES		= $(sctp_source_FILES)

libSCTPsctp_a_DEPENDENCIES		= config.h strconf.h
libSCTPsctp_a_CPPFLAGS			= \
					  -I./src/include/os7 \
					  -I$(srcdir)/src/include/os7 \
					  -I./src/drivers \
					  -I$(srcdir)/src/drivers \
					  -imacros strconf.h \
					  $(SCTP_INCLUDES) \
					  $(KERNEL_CPPFLAGS) \
					  $(KERNEL_MODFLAGS)
libSCTPsctp_a_CFLAGS			= $(KERNEL_CFLAGS)
libSCTPsctp_a_OBJECTS			= $(libSCTPsctp_a_DEPENDENCIES)

streams-sctp: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^

KERNEL_MODULES				= $(ADDON_OBJS)
EXTRA_KERNEL_MODULES			=

endif

endif

EXTRA_DIST				= $(sctp_source_FILES)

dist_noinst_SCRIPTS			=
EXTRA_SCRIPTS				=

KERNEL_MODCONF				= conf.modules
KERNEL_SUBDIR				= sctp
KMODCONF_EXT				= sctp

include am/kernel.am

include am/libraries.am

EXPORT_SRCFILES				=
EXPORT_VERFILES				=

include am/genksyms.am

SCTPMAKENODES				= @PACKAGE_TARNAME@_mknod

include am/strconf.am

install-devices: $(SCTPMAKENODES)
	./$(SCTPMAKENODES)

uninstall-devices: $(SCTPMAKENODES)
	./$(SCTPMAKENODES) --remove

SUBDIRS = doc src

#
#   Rules for making rpms
#
AM_RPMFLAGS				= --define "_kversion $(kversion)"
AM_SRPMFLAGS				=

#
#   General rules
#

include am/openss7.am

# vim: ft=automake
