# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.61 $) $Date: 2006/08/26 09:18:41 $
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
# Last Modified $Date: 2006/08/26 09:18:41 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.61  2006/08/26 09:18:41  brian
# - better release file generation
#
# Revision 0.9.2.60  2006/07/11 12:32:00  brian
# - added ISO and other implementations to distribution
#
# Revision 0.9.2.59  2006/07/01 11:59:43  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.58  2006/04/25 06:46:58  brian
# - added 2nd gen TCP driver
#
# Revision 0.9.2.57  2006/04/23 18:13:21  brian
# - changes for compile
#
# Revision 0.9.2.56  2006/03/24 06:16:37  brian
# - added next generation rawip driver
#
# Revision 0.9.2.55  2006/03/24 00:08:29  brian
# - added second generation udp driver
#
# Revision 0.9.2.54  2006/03/21 13:24:41  brian
# - included pr.am
#
# Revision 0.9.2.53  2006/03/20 11:54:04  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.52  2006/03/07 12:34:21  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.51  2006/03/06 11:38:53  brian
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

EXTRA_PROGRAMS			+= test-inet_raw \
				  test-inet_udp \
				  test-inet_tcp \
				  test-inet_sctp \
				  @PACKAGE_TARNAME@_mknod

pkglibexec_PROGRAMS		= test-inet_raw \
				  test-inet_udp \
				  test-inet_tcp

if WITH_OPENSS7_SCTP
pkglibexec_PROGRAMS		+= test-inet_sctp
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDADD		= $(STREAMS_LDADD)
test_inet_raw_LDFLAGS		= -static
test_inet_raw_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_raw_CFLAGS		= $(USER_CFLAGS)

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDADD		= $(STREAMS_LDADD)
test_inet_udp_LDFLAGS		= -static
test_inet_udp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_udp_CFLAGS		= $(USER_CFLAGS)

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDADD		= $(STREAMS_LDADD)
test_inet_tcp_LDFLAGS		= -static
test_inet_tcp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)

test_inet_sctp_SOURCES		= src/test/test-inet_sctp.c
test_inet_sctp_LDADD		= $(STREAMS_LDADD)
test_inet_sctp_LDFLAGS		= -static
test_inet_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_sctp_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libINETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libINETmods_a_SOURCES = \
				  src/drivers/inet.c \
				  src/drivers/udp.c \
				  src/drivers/rawip.c \
				  src/drivers/tcp.c \
				  src/drivers/udp_hooks.h

dist_EXTRA_libINETmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c

mods_source_FILES		= $(nodist_EXTRA_libINETmods_a_SOURCES)

nodist_libINETmods_a_SOURCES	= $(mods_source_FILES)

libINETmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/udp_hooks.h
libINETmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libINETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-inet_raw \
				  test-inet_udp \
				  test-inet_tcp \
				  test-inet_sctp

pkglibexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_udp \
				  test-inet_tcp

if WITH_OPENSS7_SCTP
pkglibexec_PROGRAMS		+= \
				  test-inet_sctp
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDADD		= $(STREAMS_LDADD)
test_inet_raw_LDFLAGS		= -static
test_inet_raw_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_raw_CFLAGS		= $(USER_CFLAGS)

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDADD		= $(STREAMS_LDADD)
test_inet_udp_LDFLAGS		= -static
test_inet_udp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_udp_CFLAGS		= $(USER_CFLAGS)

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDADD		= $(STREAMS_LDADD)
test_inet_tcp_LDFLAGS		= -static
test_inet_tcp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)

test_inet_sctp_SOURCES		= src/test/test-inet_sctp.c
test_inet_sctp_LDADD		= $(STREAMS_LDADD)
test_inet_sctp_LDFLAGS		= -static
test_inet_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_sctp_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libINETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libINETmods_a_SOURCES	= \
				  src/drivers/inet.c \
				  src/drivers/udp.c \
				  src/drivers/rawip.c \
				  src/drivers/tcp.c \
				  src/drivers/udp_hooks.h

dist_EXTRA_libINETmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c

mods_source_FILES		= $(nodist_EXTRA_libINETmods_a_SOURCES)

nodist_libINETmods_a_SOURCES	= $(mods_source_FILES)

libINETmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H) src/drivers/udp_hooks.h
libINETmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libINETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## ! PUBLIC_RELEASE
endif

## PKG_BUILD_ARCH
endif

EXTRA_DIST			+= $(mods_source_FILES)

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
KERNEL_OBJECTS			= $(libINETmods_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libINETmods_a_OBJECTS) $(ADDON_MODS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			= $(STREAMS_MODMAP) $(STRCOMP_MODMAP)

if WITH_KO_MODULES
## WITH_KO_MODULES
kpre				= streams_
else
kpre				= streams-
## WITH_KO_MODULES
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

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

if PKG_BUILD_ARCH
if RPM_BUILD_USER
## these go with the utilities package
dist_sbin_SCRIPTS 		= $(INET_MKDEV) $(INET_RMDEV)
endif
endif

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

install-inet-devices: $(INET_MKDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV) ; \
	fi

uninstall-inet-devices: $(INET_RMDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV) ; \
	fi

## RPM_BUILD_KERNEL
else
## ! RPM_BUILD_KERNEL

install-inet-devices:

uninstall-inet-devices:

## ! RPM_BUILD_KERNEL
endif

INSTALL_EXEC_HOOK += install-inet-devices

UNINSTALL_HOOK += uninstall-inet-devices

.PHONY: install-inet-devices uninstall-inet-devices

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src

if WITH_32BIT_LIBS
SUBDIRS += . lib32
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

dist_initrd_SCRIPTS		= src/util/init/@PACKAGE_LCNAME@.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/@PACKAGE_LCNAME@

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= src/util/sysconfig/@PACKAGE_LCNAME@.conf

dist_config_DATA		= src/util/sysconfig/@PACKAGE_LCNAME@

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
