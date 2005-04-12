# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.28 $) $Date: 2005/04/12 00:05:07 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2005/04/12 00:05:07 $ by $Author: brian $
#
# =============================================================================

##
## General rules
##
include am/openss7.am

lib_LTLIBRARIES			=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if PUBLIC_RELEASE
## PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

libexec_PROGRAMS		= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t

EXTRA_PROGRAMS			+= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sctp_SOURCES		= src/test/test-sctp.c
test_sctp_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS		= $(USER_CFLAGS)

test_sctp_n_SOURCES		= src/test/test-sctp_n.c
test_sctp_n_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS		= $(USER_CFLAGS)

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c
test_sctp_nc_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_nc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c
test_sctp_ns_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_ns_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS)

test_sctp_t_SOURCES		= src/test/test-sctp_t.c
test_sctp_t_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= libSCTPsctp.a
ADDON_MODS			= streams-sctp.o
ADDON_OBJS			= $(libSCTPsctp_a_OBJECTS)

noinst_LIBRARIES		= $(ADDON_LIBS)
EXTRA_LIBRARIES			+= libSCTPsctp.a

sctp_source_FILES		= \
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
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_crc32c.h \
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
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/compat.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h
sctp2_source_FILES		= \
				  src/drivers/sctp2.c \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_hooks.h \
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/compat.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h
if WITH_HMAC_MD5
## WITH_HMAC_MD5
sctp2_source_FILES		+= \
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_hmac_md5.h
## WITH_HMAC_MD5
endif
if WITH_HMAC_SHA1
## WITH_HMAC_SHA1
sctp2_source_FILES		+= \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_hmac_sha1.h
## WITH_HMAC_SHA1
endif
if WITH_ADLER_32
## WITH_ADLER_32
sctp2_source_FILES		+= \
				  src/drivers/sctp_adler32.c \
				  src/drivers/sctp_adler32.h
## WITH_ADLER_32
endif
if WITH_CRC_32C
## WITH_CRC_32C
sctp2_source_FILES		+= \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_crc32c.h
## WITH_CRC_32C
endif
if WITH_SCTP
## WITH_SCTP
nodist_libSCTPsctp_a_SOURCES	= $(sctp_source_FILES)
## WITH_SCTP
endif
if WITH_SCTP2
## WITH_SCTP2
nodist_libSCTPsctp_a_SOURCES	= $(sctp2_source_FILES)
## WITH_SCTP2
endif
libSCTPsctp_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/sctp_hooks.h
libSCTPsctp_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSCTPsctp_a_CFLAGS		= $(KERNEL_CFLAGS)

libSCTPsctp_a_OBJECTS: $(libSCTPsctp_a_DEPENDENCIES)
streams-sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

libexec_PROGRAMS		= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t

EXTRA_PROGRAMS			+= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sctp_SOURCES		= src/test/test-sctp.c
test_sctp_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS		= $(USER_CFLAGS)

test_sctp_n_SOURCES		= src/test/test-sctp_n.c
test_sctp_n_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS		= $(USER_CFLAGS)

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c
test_sctp_nc_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_nc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c
test_sctp_ns_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_ns_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS)

test_sctp_t_SOURCES		= src/test/test-sctp_t.c
test_sctp_t_LDFLAGS		= $(STREAMS_LDADD)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= libSCTPsctp.a
ADDON_MODS			= streams-sctp.o
ADDON_OBJS			= $(libSCTPsctp_a_OBJECTS)

noinst_LIBRARIES		= $(ADDON_LIBS)
EXTRA_LIBRARIES			+= libSCTPsctp.a

sctp_source_FILES		= \
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
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_crc32c.h \
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
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/compat.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h
sctp2_source_FILES		= \
				  src/drivers/sctp2.c \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_hooks.h \
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/compat.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h
if WITH_HMAC_MD5
## WITH_HMAC_MD5
sctp2_source_FILES		+= \
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_hmac_md5.h
## WITH_HMAC_MD5
endif
if WITH_HMAC_SHA1
## WITH_HMAC_SHA1
sctp2_source_FILES		+= \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_hmac_sha1.h
## WITH_HMAC_SHA1
endif
if WITH_ADLER_32
## WITH_ADLER_32
sctp2_source_FILES		+= \
				  src/drivers/sctp_adler32.c \
				  src/drivers/sctp_adler32.h
## WITH_ADLER_32
endif
if WITH_CRC_32C
## WITH_CRC_32C
sctp2_source_FILES		+= \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_crc32c.h
## WITH_CRC_32C
endif
if WITH_SCTP
## WITH_SCTP
nodist_libSCTPsctp_a_SOURCES	= $(sctp_source_FILES)
## WITH_SCTP
endif
if WITH_SCTP2
## WITH_SCTP2
nodist_libSCTPsctp_a_SOURCES	= $(sctp2_source_FILES)
## WITH_SCTP2
endif
libSCTPsctp_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSCTPsctp_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSCTPsctp_a_CFLAGS		= $(KERNEL_CFLAGS)

libSCTPsctp_a_OBJECTS: $(libSCTPsctp_a_DEPENDENCIES)
streams-sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## ! PUBLIC_RELEASE
endif

## PKG_BUILD_ARCH
endif

EXTRA_DIST			+= $(sctp_source_FILES) $(sctp2_source_FILES)

EXTRA_TEX			+= \
				scripts/texinfo.cnf \
				scripts/texi/args.texi.in \
				scripts/texi/fdl.texi \
				scripts/texi/gpl.texi \
				scripts/texi/install.texi \
				scripts/texi/lesser.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+= configure.nexusware

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(ADDON_OBJS)
KERNEL_MODULES			= $(ADDON_MODS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			= $(STREAMS_MODMAP)

if WITH_KO_MODULES
kpre				= streams_
else
kpre				= streams-
endif

include am/kernel.am

if WITH_KO_MODULES
else
if PKG_BUILD_INDEP
dist_modutil_DATA		= \
				src/util/modutils/@PACKAGE_LCNAME@
endif
endif

##
## Rules for building libraries
##
##include am/libraries.am
##
PKGINCL				= include/sys/$(PACKAGE_NAME)

##AUTOCONFIG_H			= $(PKGINCL)/config.h
##
##EXTRA_DIST			+= $(AUTOCONFIG_H).in
##
EXPORT_SRCFILES			=

EXPORT_VERFILES			=

GENKSYMS_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

##
## Rules for generating versioned kernel symbols
##
include am/genksyms.am

##
## Rules for generating STREAMS configuration
##
include am/strconf.am

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src doc tests

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		=

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		=

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		=

dist_config_DATA		=

## INSTALL_INITSCRIPTS
endif

## PKG_BUILD_INDEP
endif

## RPM_BUILD_USER
endif

##
## Rules for init scripts
##
include am/init.am

##
## Rules for lsms
##
include am/lsm.am

##
## Rules for archives
##
include am/archive.am

##
## Rules for making rpms
##
AM_RPMFLAGS			= --define "_kversion $(kversion)"
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

EXTRA_DIST		+= \
			debian/README.Debian \
			debian/TODO.Debian

MAINTAINERCLEANFILES	+= \
			$(top_srcdir)/aclocal.m4 \
			$(top_srcdir)/config.h.in \
			$(top_srcdir)/configure \
			$(top_srcdir)/COPYING \
			$(top_srcdir)/INSTALL \
			$(top_srcdir)/scripts/compile \
			$(top_srcdir)/scripts/config.guess \
			$(top_srcdir)/scripts/config.sub \
			$(top_srcdir)/scripts/depcomp \
			$(top_srcdir)/scripts/install-sh \
			$(top_srcdir)/scripts/ltmain.sh \
			$(top_srcdir)/scripts/missing \
			$(top_srcdir)/scripts/texinfo.tex

# vim: ft=automake
