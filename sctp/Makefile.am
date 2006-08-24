# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.48 $) $Date: 2006/07/01 11:59:38 $
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
# Last Modified $Date: 2006/07/01 11:59:38 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.48  2006/07/01 11:59:38  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.47  2006/03/29 11:27:41  brian
# - corrections for rerelease, see ChangeLog
#
# Revision 0.9.2.46  2006/03/21 13:24:36  brian
# - included pr.am
#
# Revision 0.9.2.45  2006/03/20 11:53:57  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.44  2006/03/06 11:38:11  brian
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

if RPM_BUILD_USER
## RPM_BUILD_USER

sctp_programs			= \
				  test-tcps \
				  test-tcpc \
				  test-udps \
				  test-udpc \
				  test-sctps \
				  test-sctpc \
				  test-tcp-ds \
				  test-tcp-dc \
				  test-sctp-ds \
				  test-sctp-dc \
				  test-tcp-ts \
				  test-tcp-tc \
				  test-sctp-ts \
				  test-sctp-tc \
				  test-sctp-ss \
				  test-sctp-sc

if WITH_KO_MODULES
pkglibexec_PROGRAMS		= 
else
pkglibexec_PROGRAMS		= $(sctp_programs)
endif

EXTRA_PROGRAMS			+= $(sctp_programs)

test_tcps_SOURCES		= test/test-tcps.c
test_tcps_DEPENDENCIES		= $(CONFIG_HEADER)
test_tcps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcps_CFLAGS		= $(USER_CFLAGS)


test_tcpc_SOURCES		= test/test-tcpc.c
test_tcpc_DEPENDENCIES		= $(CONFIG_HEADER)
test_tcpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcpc_CFLAGS		= $(USER_CFLAGS)

test_udps_SOURCES		= test/test-udps.c
test_udps_DEPENDENCIES		= $(CONFIG_HEADER)
test_udps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_udps_CFLAGS		= $(USER_CFLAGS)


test_udpc_SOURCES		= test/test-udpc.c
test_udpc_DEPENDENCIES		= $(CONFIG_HEADER)
test_udpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_udpc_CFLAGS		= $(USER_CFLAGS)

test_sctps_SOURCES		= test/test-sctps.c
test_sctps_DEPENDENCIES		= $(CONFIG_HEADER)
test_sctps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctps_CFLAGS		= $(USER_CFLAGS)


test_sctpc_SOURCES		= test/test-sctpc.c
test_sctpc_DEPENDENCIES		= $(CONFIG_HEADER)
test_sctpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctpc_CFLAGS		= $(USER_CFLAGS)

test_tcp_ds_SOURCES		= test/test-tcp-ds.c
test_tcp_ds_DEPENDENCIES	= $(CONFIG_HEADER)
test_tcp_ds_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcp_ds_CFLAGS		= $(USER_CFLAGS)


test_tcp_dc_SOURCES		= test/test-tcp-dc.c
test_tcp_dc_DEPENDENCIES	= $(CONFIG_HEADER)
test_tcp_dc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcp_dc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ds_SOURCES		= test/test-sctp-ds.c
test_sctp_ds_DEPENDENCIES	= $(CONFIG_HEADER)
test_sctp_ds_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ds_CFLAGS		= $(USER_CFLAGS)


test_sctp_dc_SOURCES		= test/test-sctp-dc.c
test_sctp_dc_DEPENDENCIES	= $(CONFIG_HEADER)
test_sctp_dc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_dc_CFLAGS		= $(USER_CFLAGS)

test_tcp_ts_SOURCES		= test/test-tcp-ts.c
test_tcp_ts_DEPENDENCIES	= $(CONFIG_HEADER)
test_tcp_ts_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcp_ts_CFLAGS		= $(USER_CFLAGS)


test_tcp_tc_SOURCES		= test/test-tcp-tc.c
test_tcp_tc_DEPENDENCIES	= $(CONFIG_HEADER)
test_tcp_tc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcp_tc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ts_SOURCES		= test/test-sctp-ts.c
test_sctp_ts_DEPENDENCIES	= $(CONFIG_HEADER)
test_sctp_ts_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ts_CFLAGS		= $(USER_CFLAGS)


test_sctp_tc_SOURCES		= test/test-sctp-tc.c
test_sctp_tc_DEPENDENCIES	= $(CONFIG_HEADER)
test_sctp_tc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_tc_CFLAGS		= $(USER_CFLAGS)

test_sctp_ss_SOURCES		= test/test-sctp-ss.c
test_sctp_ss_DEPENDENCIES	= $(CONFIG_HEADER)
test_sctp_ss_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ss_CFLAGS		= $(USER_CFLAGS)


test_sctp_sc_SOURCES		= test/test-sctp-sc.c
test_sctp_sc_DEPENDENCIES	= $(CONFIG_HEADER)
test_sctp_sc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_sc_CFLAGS		= $(USER_CFLAGS)

sbin_PROGRAMS			=

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

if WITH_KO_MODULES
ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 
else
ADDON_LIBS			= libSCTPmodl.a
ADDON_MODS			= sctp.o
ADDON_OBJS			= $(libSCTPmodl_a_OBJECTS)
endif

noinst_LIBRARIES		= $(ADDON_LIBS)
EXTRA_LIBRARIES			+= libSCTPmodl.a

nodist_EXTRA_libSCTPmodl_a_SOURCES = \
				  src/sctp_adler32.h \
				  src/sctp_adler32.c \
				  src/sctp_crc32c.h \
				  src/sctp_crc32c.c \
				  src/sctp_hmac_md5.h \
				  src/sctp_hmac_md5.c \
				  src/sctp_hmac_sha1.h \
				  src/sctp_hmac_sha1.c \
				  src/sctp_md5.h \
				  src/sctp_md5.c \
				  src/sctp_sha1.h \
				  src/sctp_sha1.c \
				  src/sctp_debug.h \
				  src/sctp_namespace.h \
				  src/sctp.c

if WITH_KO_MODULES
sctp_source_FILES		= 
else
sctp_source_FILES		= $(nodist_EXTRA_libSCTPmodl_a_SOURCES)
endif

nodist_libSCTPmodl_a_SOURCES	= $(sctp_source_FILES)

libSCTPmodl_a_DEPENDENCIES	= $(CONFIG_HEADER)
libSCTPmodl_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION)
libSCTPmodl_a_CFLAGS		= $(KERNEL_CFLAGS)

libSCTPmodl_a_OBJECTS: $(libSCTPmodl_a_DEPENDENCIES)

combined-sctp.o: stamp-verobjs
	@list=`$(ECHO) " $(libSCTPmodl_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| |g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list && cp -f -- $@ sctp.o" ; \
	$(LD) -r -o $@ $$list && cp -f -- $@ sctp.o

sctp.o: combined-sctp.o

CLEANFILES += combined-sctp.o

## RPM_BUILD_KERNEL
endif

## PKG_BUILD_ARCH
endif

EXTRA_DIST			+= $(sctp_source_FILES)

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
KERNEL_MODCONF			=
KERNEL_SUBDIR			= kernel/net/ipv4
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			=

kpre				=

include am/kernel.am

##
## Rules for building libraries
##
##include am/libraries.am
##
##AUTOCONFIG_H			= $(PKGINCL)/config.h
##
##EXTRA_DIST			+= $(AUTOCONFIG_H).in

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
##include am/strconf.am
##
if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS =

if DEVELOPMENT
SUBDIRS += include
endif

SUBDIRS += . tests

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
				util/init/sctp.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				util/init/sctp

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				util/sysconfig/sctp.conf

dist_config_DATA		= \
				util/sysconfig/sctp

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
AM_RPMFLAGS 			= --define "_kversion $(kversion)"
AM_SRPMFLAGS 			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Put the text manual where we can find it in the distribution
##
doc/manual/$(PACKAGE).txt:
	make -C doc/manual $(PACKAGE).txt

MANUAL: doc/manual/$(PACKAGE).txt
	cp -f $< $@

ALL_LOCAL			+= MANUAL

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
