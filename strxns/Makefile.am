# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2005/02/19 12:19:57 $
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
# Last Modified $Date: 2005/02/19 12:19:57 $ by $Author: brian $
#
# =============================================================================

#
#   General rules
#
include am/openss7.am

lib_LTLIBRARIES			= 

if PUBLIC_RELEASE

if RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  ldltest \
				  ldlconfig

libexec_PROGRAMS		= \
				  ldltest

sbin_PROGRAMS			= \
				  ldlconfig \
				  @PACKAGE_TARNAME@_mknod

ldlconfig_SOURCES		= src/util/ldlconfig.c
ldlconfig_LDADD			= $(STREAMS_LDADD)
ldlconfig_CPPFLAGS		= $(XNS_INCLUDES) $(USER_CPPFLAGS)
ldlconfig_CFLAGS		= $(USER_CFLAGS)

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(XNS_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

ldltest_SOURCES			= src/test/ldltest.c
ldltest_LDADD			= $(STREAMS_LDADD)
ldltest_CPPFLAGS		= $(XNS_INCLUDES) $(USER_CPPFLAGS)
ldltest_CFLAGS			= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libXNSmods.a libXNSdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libXNSmods_a_SOURCES = \
				  src/modules/ip_strm_mod.c \
				  src/modules/ip_strm_mod.h \
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

mods_source_FILES		= $(nodist_EXTRA_libXNSmods_a_SOURCES)

nodist_libXNSmods_a_SOURCES	= $(mods_source_FILES)

libXNSmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNSmods_a_CPPFLAGS		= \
				  -I./src/include/os7 \
				  -I$(srcdir)/src/include/os7 \
				  -I./src/modules \
				  -I$(srcdir)/src/modules \
				  -imacros strconf.h \
				  $(XNS_INCLUDES) \
				  $(KERNEL_CPPFLAGS) \
				  $(KERNEL_MODFLAGS)
libXNSmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
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

drvs_source_FILES		= $(nodist_EXTRA_libXNSdrvs_a_SOURCES)

nodist_libXNSdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNSdrvs_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNSdrvs_a_CPPFLAGS		= \
				  -I./src/include/os7 \
				  -I$(srcdir)/src/include/os7 \
				  -I./src/drivers \
				  -I$(srcdir)/src/drivers \
				  -imacros strconf.h \
				  $(XNS_INCLUDES) \
				  $(KERNEL_CPPFLAGS) \
				  $(KERNEL_MODFLAGS)
libXNSdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+= 

endif

else

if RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  ldltest \
				  ldlconfig

libexec_PROGRAMS		= \
				  ldltest

sbin_PROGRAMS			= \
				  ldlconfig \
				  @PACKAGE_TARNAME@_mknod

ldlconfig_SOURCES		= src/util/ldlconfig.c
ldlconfig_LDADD			= $(STREAMS_LDADD)
ldlconfig_CPPFLAGS		= $(XNS_INCLUDES) $(USER_CPPFLAGS)
ldlconfig_CFLAGS		= $(USER_CFLAGS)

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(XNS_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

ldltest_SOURCES			= src/test/ldltest.c
ldltest_LDADD			= $(STREAMS_LDADD)
ldltest_CPPFLAGS		= $(XNS_INCLUDES) $(USER_CPPFLAGS)
ldltest_CFLAGS			= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libXNSmods.a libXNSdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libXNSmods_a_SOURCES = \
				  src/modules/ip_strm_mod.c \
				  src/modules/ip_strm_mod.h \
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

mods_source_FILES		= $(nodist_EXTRA_libXNSmods_a_SOURCES)

nodist_libXNSmods_a_SOURCES	= $(mods_source_FILES)

libXNSmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNSmods_a_CPPFLAGS		= \
				  -I./src/include/os7 \
				  -I$(srcdir)/src/include/os7 \
				  -I./src/modules \
				  -I$(srcdir)/src/modules \
				  -imacros strconf.h \
				  $(XNS_INCLUDES) \
				  $(KERNEL_CPPFLAGS) \
				  $(KERNEL_MODFLAGS)
libXNSmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
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

drvs_source_FILES		= $(nodist_EXTRA_libXNSdrvs_a_SOURCES)

nodist_libXNSdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNSdrvs_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNSdrvs_a_CPPFLAGS		= \
				  -I./src/include/os7 \
				  -I$(srcdir)/src/include/os7 \
				  -I./src/drivers \
				  -I$(srcdir)/src/drivers \
				  -imacros strconf.h \
				  $(XNS_INCLUDES) \
				  $(KERNEL_CPPFLAGS) \
				  $(KERNEL_MODFLAGS)
libXNSdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+= 

endif

endif

EXTRA_DIST			+= $(mods_source_FILES) $(drvs_source_FILES)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+=

export KERNEL_MODULES		= $(libXNSmods_a_OBJECTS) $(libXNSdrvs_a_OBJECTS) $(ADDON_OBJS)
export KERNEL_MODCONF		= $(STRCONF_CONFMOD)
export KERNEL_SUBDIR		= $(PACKAGE_TARNAME)
export KMODCONF_EXT		= $(PACKAGE_TARNAME)
export KERNEL_MODMAPS		= $(STREAMS_MODMAP)

include am/kernel.am

include am/strconf.am

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
