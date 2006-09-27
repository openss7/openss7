# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2006/09/27 05:15:28 $
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
# Last Modified $Date: 2006/09/27 05:15:28 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.51  2006/09/27 05:15:28  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.50  2006/09/18 01:15:33  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.49  2006/08/26 09:19:10  brian
# - better release file generation
#
# Revision 0.9.2.48  2006/08/07 22:17:00  brian
# - changes from SCTP Interop
#
# Revision 0.9.2.47  2006/07/29 07:44:03  brian
# - CVS checkin of changes before leaving for SCTP interop
#
# Revision 0.9.2.46  2006/07/01 11:59:44  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.45  2006/05/07 07:24:55  brian
# - updates for testing
#
# Revision 0.9.2.44  2006/03/21 13:24:42  brian
# - included pr.am
#
# Revision 0.9.2.43  2006/03/20 11:54:04  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.42  2006/03/07 12:34:22  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.41  2006/03/06 11:39:01  brian
# - updated headers
#
# =============================================================================

##
## General rules
##
include am/openss7.am

##
## Help
##
include am/help.am

##
## Problem Reports
##
include am/pr.am

lib_LTLIBRARIES			=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if PUBLIC_RELEASE
## PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t \
				  test-etsi_n \
				  test-etsi_t \
				  test-interop

pkglibexec_PROGRAMS		= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t \
				  test-etsi_n \
				  test-etsi_t \
				  test-interop

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sctp_SOURCES		= src/test/test-sctp.c
test_sctp_LDADD			= $(STREAMS_LDADD)
test_sctp_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS		= $(USER_CFLAGS)

test_sctp_n_SOURCES		= src/test/test-sctp_n.c
test_sctp_n_LDADD		= $(STREAMS_LDADD)
test_sctp_n_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS		= $(USER_CFLAGS)

test_sctp_n2_SOURCES		= src/test/test-sctp_n2.c
test_sctp_n2_LDADD		= $(STREAMS_LDADD)
test_sctp_n2_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_n2_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n2_CFLAGS		= $(USER_CFLAGS)

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c
test_sctp_nc_LDADD		= $(STREAMS_LDADD)
test_sctp_nc_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_nc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c
test_sctp_ns_LDADD		= $(STREAMS_LDADD)
test_sctp_ns_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_ns_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS)

test_sctp_t_SOURCES		= src/test/test-sctp_t.c
test_sctp_t_LDADD		= $(STREAMS_LDADD)
test_sctp_t_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS		= $(USER_CFLAGS)

test_etsi_n_SOURCES		= src/test/test-etsi_n.c
test_etsi_n_LDADD		= $(STREAMS_LDADD)
test_etsi_n_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_etsi_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_etsi_n_CFLAGS		= $(USER_CFLAGS)

test_etsi_t_SOURCES		= src/test/test-etsi_t.c
test_etsi_t_LDADD		= $(STREAMS_LDADD)
test_etsi_t_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_etsi_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_etsi_t_CFLAGS		= $(USER_CFLAGS)

test_interop_SOURCES		= src/test/test-interop.c
test_interop_LDADD		= $(STREAMS_LDADD)
test_interop_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_interop_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_interop_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= libSCTPsctp.a libSCTPtest.a
if WITH_KO_MODULES
ADDON_MODS			= streams_sctp.o
else
ADDON_MODS			= streams-sctp.o
endif
ADDON_OBJS			= $(libSCTPsctp_a_OBJECTS) $(libSCTPtest_a_OBJECTS)

noinst_LIBRARIES		= $(ADDON_LIBS)
EXTRA_LIBRARIES			+= libSCTPsctp.a libSCTPtest.a

nodist_EXTRA_libSCTPtest_a_SOURCES = \
				  src/modules/tpiperf.c

test_source_FILES		= $(nodist_EXTRA_libSCTPtest_a_SOURCES)

nodist_libSCTPtest_a_SOURCES	= \
			          $(test_source_FILES)

libSCTPtest_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSCTPtest_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libSCTPtest_a_CFLAGS		= $(KERNEL_CFLAGS)

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
				  src/drivers/sctp_t.h
sctp2_source_FILES		= \
				  src/drivers/sctp2.c \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_hooks.h
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
libSCTPsctp_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSCTPsctp_a_CFLAGS		= $(KERNEL_CFLAGS)

libSCTPsctp_a_OBJECTS: $(libSCTPsctp_a_DEPENDENCIES)
if WITH_KO_MODULES
streams_sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t \
				  test-etsi_n \
				  test-etsi_t \
				  test-interop

pkglibexec_PROGRAMS		= \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t \
				  test-etsi_n \
				  test-etsi_t \
				  test-interop

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sctp_SOURCES		= src/test/test-sctp.c
test_sctp_LDADD			= $(STREAMS_LDADD)
test_sctp_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS		= $(USER_CFLAGS)

test_sctp_n_SOURCES		= src/test/test-sctp_n.c
test_sctp_n_LDADD		= $(STREAMS_LDADD)
test_sctp_n_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS		= $(USER_CFLAGS)

test_sctp_n2_SOURCES		= src/test/test-sctp_n2.c
test_sctp_n2_LDADD		= $(STREAMS_LDADD)
test_sctp_n2_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_n2_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n2_CFLAGS		= $(USER_CFLAGS)

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c
test_sctp_nc_LDADD		= $(STREAMS_LDADD)
test_sctp_nc_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_nc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c
test_sctp_ns_LDADD		= $(STREAMS_LDADD)
test_sctp_ns_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_ns_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS)

test_sctp_t_SOURCES		= src/test/test-sctp_t.c
test_sctp_t_LDADD		= $(STREAMS_LDADD)
test_sctp_t_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS		= $(USER_CFLAGS)

test_etsi_n_SOURCES		= src/test/test-etsi_n.c
test_etsi_n_LDADD		= $(STREAMS_LDADD)
test_etsi_n_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_etsi_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_etsi_n_CFLAGS		= $(USER_CFLAGS)

test_etsi_t_SOURCES		= src/test/test-etsi_t.c
test_etsi_t_LDADD		= $(STREAMS_LDADD)
test_etsi_t_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_etsi_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_etsi_t_CFLAGS		= $(USER_CFLAGS)

test_interop_SOURCES		= src/test/test-interop.c
test_interop_LDADD		= $(STREAMS_LDADD)
test_interop_LDFLAGS		= -static $(STREAMS_LDFLAGS)
test_interop_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_interop_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= libSCTPsctp.a libSCTPtest.a
if WITH_KO_MODULES
ADDON_MODS			= streams_sctp.o 
else
ADDON_MODS			= streams-sctp.o
endif
ADDON_OBJS			= $(libSCTPsctp_a_OBJECTS) $(libSCTPtest_a_OBJECTS)

noinst_LIBRARIES		= $(ADDON_LIBS)
EXTRA_LIBRARIES			+= libSCTPsctp.a libSCTPtest.a

nodist_EXTRA_libSCTPtest_a_SOURCES = \
				  src/modules/tpiperf.c

test_source_FILES		= $(nodist_EXTRA_libSCTPtest_a_SOURCES)

nodist_libSCTPtest_a_SOURCES	= \
			          $(test_source_FILES)

libSCTPtest_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSCTPtest_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libSCTPtest_a_CFLAGS		= $(KERNEL_CFLAGS)

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
				  src/drivers/sctp_t.h
sctp2_source_FILES		= \
				  src/drivers/sctp2.c \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_hooks.h
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
libSCTPsctp_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSCTPsctp_a_CFLAGS		= $(KERNEL_CFLAGS)

libSCTPsctp_a_OBJECTS: $(libSCTPsctp_a_DEPENDENCIES)
if WITH_KO_MODULES
streams_sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## ! PUBLIC_RELEASE
endif

## PKG_BUILD_ARCH
endif

EXTRA_DIST			+= $(sctp_source_FILES) $(sctp2_source_FILES) $(test_source_FILES)

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
KERNEL_MODULES			= $(libSCTPtest_a_OBJECTS) $(ADDON_MODS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			= $(STREAMS_MODMAP) $(STRCOMP_MODMAP)

if WITH_KO_MODULES
kpre				= streams_
else
kpre				= streams-
endif

include am/kernel.am

if WITH_KO_MODULES
else
if PKG_BUILD_INDEP
if RPM_BUILD_USER
dist_modutil_DATA		= \
				src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
##include am/libraries.am
##
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

SUBDIRS = src

SUBDIRS += .

if WITH_32BIT_LIBS
SUBDIRS += lib32
endif

SUBDIRS += tests

if DEVELOPMENT
SUBDIRS += doc
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				src/util/init/strsctp.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				src/util/init/strsctp

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				src/util/sysconfig/strsctp.conf

dist_config_DATA		= \
				src/util/sysconfig/strsctp

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

##
## Rules for making release files
##
MANUAL_FILE			= doc/manual/$(PACKAGE).txt

include am/release.am

EXTRA_DIST			+= \
				  TODO \
				  MANUAL \
				  README-cvs \
				  README-nexusware \
				  debian/README.Debian \
				  debian/TODO.Debian

MAINTAINERCLEANFILES		+= \
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

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
