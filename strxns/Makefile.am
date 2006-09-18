# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2006/09/18 01:15:35 $
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
# Last Modified $Date: 2006/09/18 01:15:35 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.51  2006/09/18 01:15:35  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.50  2006/08/26 09:20:20  brian
# - better release file generation
#
# Revision 0.9.2.49  2006/07/11 12:32:06  brian
# - added ISO and other implementations to distribution
#
# Revision 0.9.2.48  2006/07/01 11:59:46  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.47  2006/05/08 03:36:49  brian
# - added init scripts
#
# Revision 0.9.2.46  2006/05/07 22:12:53  brian
# - updated for NPI-IP driver
#
# Revision 0.9.2.45  2006/05/06 10:22:16  brian
# - added test suite for NPI-IP driver
#
# Revision 0.9.2.44  2006/03/21 13:24:44  brian
# - included pr.am
#
# Revision 0.9.2.43  2006/03/20 11:54:08  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.42  2006/03/18 09:42:10  brian
# - added in ip driver and manual pages
#
# Revision 0.9.2.41  2006/03/07 12:34:26  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.40  2006/03/06 11:39:24  brian
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
				  ldltest \
				  ldlconfig \
				  test-np_ip \
				  @PACKAGE_TARNAME@_mknod
#				  test-ip

pkglibexec_PROGRAMS		= \
				  ldltest \
				  test-np_ip
#				  test-ip


sbin_PROGRAMS			= \
				  ldlconfig \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

ldlconfig_SOURCES		= src/util/ldlconfig.c
ldlconfig_LDADD			= $(STREAMS_LDADD) $(STRCOMP_LDADD)
ldlconfig_LDFLAGS		= -static
ldlconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
ldlconfig_CFLAGS		= $(USER_CFLAGS)

ldltest_SOURCES			= src/test/ldltest.c
ldltest_LDADD			= $(STREAMS_LDADD) $(STRCOMP_LDADD)
ldltest_LDFLAGS			= -static
ldltest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
ldltest_CFLAGS			= $(USER_CFLAGS)

#test_ip_SOURCES		= src/test/test-ip.c
#test_ip_LDADD			= $(STREAMS_LDADD) $(STRCOMP_LDADD)
#test_ip_LDFLAGS		= -static
#test_ip_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#test_ip_CFLAGS			= $(USER_CFLAGS)

test_np_ip_SOURCES		= src/test/test-np_ip.c
test_np_ip_LDADD		= $(STREAMS_LDADD) $(STRCOMP_LDADD)
test_np_ip_LDFLAGS		= -static
test_np_ip_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_np_ip_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libXNSmods.a libXNSdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libXNSmods_a_SOURCES = \
				  src/modules/debug.h \
				  src/modules/ip_strm_mod.c \
				  src/modules/ip_strm_mod.h

mods_source_FILES		= $(nodist_EXTRA_libXNSmods_a_SOURCES)

nodist_libXNSmods_a_SOURCES	= $(mods_source_FILES)

libXNSmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libXNSmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libXNSmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/debug.h \
				  src/drivers/ip_hooks.h \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
				  src/drivers/np_ip.c

dist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/cd.c \
				  src/drivers/dl.c \
				  src/drivers/dl.h \
				  src/drivers/dl_ip.c \
				  src/drivers/dlp.c \
				  src/drivers/ip.c \
				  src/drivers/nf.c \
				  src/drivers/np.c \
				  src/drivers/np.h

drvs_source_FILES		= $(nodist_EXTRA_libXNSdrvs_a_SOURCES)

nodist_libXNSdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNSdrvs_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/ip_hooks.h
libXNSdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libXNSdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)
libXNSdrvs_a_OBJECTS:		$(libXNSdrvs_a_DEPENDENCIES)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  ldltest \
				  ldlconfig \
				  test-np_ip
#				  test-ip

pkglibexec_PROGRAMS		= \
				  ldltest \
				  test-np_ip
#				  test-ip


sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod \
				  ldlconfig

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

ldlconfig_SOURCES		= src/util/ldlconfig.c
ldlconfig_LDADD			= $(STREAMS_LDADD) $(STRCOMP_LDADD)
ldlconfig_LDFLAGS		= -static
ldlconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
ldlconfig_CFLAGS		= $(USER_CFLAGS)

ldltest_SOURCES			= src/test/ldltest.c
ldltest_LDADD			= $(STREAMS_LDADD) $(STRCOMP_LDADD)
ldltest_LDFLAGS			= -static
ldltest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
ldltest_CFLAGS			= $(USER_CFLAGS)

#test_ip_SOURCES		= src/test/test-ip.c
#test_ip_LDADD			= $(STREAMS_LDADD) $(STRCOMP_LDADD)
#test_ip_LDFLAGS		= -static
#test_ip_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#test_ip_CFLAGS			= $(USER_CFLAGS)

test_np_ip_SOURCES		= src/test/test-np_ip.c
test_np_ip_LDADD		= $(STREAMS_LDADD) $(STRCOMP_LDADD)
test_np_ip_LDFLAGS		= -static
test_np_ip_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_np_ip_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libXNSmods.a libXNSdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libXNSmods_a_SOURCES = \
				  src/modules/debug.h \
				  src/modules/ip_strm_mod.c \
				  src/modules/ip_strm_mod.h

mods_source_FILES		= $(nodist_EXTRA_libXNSmods_a_SOURCES)

nodist_libXNSmods_a_SOURCES	= $(mods_source_FILES)

libXNSmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libXNSmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libXNSmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/debug.h \
				  src/drivers/ip_hooks.h \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
				  src/drivers/np_ip.c

dist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/cd.c \
				  src/drivers/dl.c \
				  src/drivers/dl.h \
				  src/drivers/dl_ip.c \
				  src/drivers/dlp.c \
				  src/drivers/ip.c \
				  src/drivers/nf.c \
				  src/drivers/np.c \
				  src/drivers/np.h

drvs_source_FILES		= $(nodist_EXTRA_libXNSdrvs_a_SOURCES)

nodist_libXNSdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNSdrvs_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/ip_hooks.h
libXNSdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libXNSdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)
libXNSdrvs_a_OBJECTS:		$(libXNSdrvs_a_DEPENDENCIES)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif

## ! PUBLIC_RELEASE
endif

## PKG_BUILD_ARCH
endif

EXTRA_DIST			+= $(mods_source_FILES) $(drvs_source_FILES)

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
KERNEL_OBJECTS			= $(libXNSmods_a_OBJECTS) $(libXNSdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libXNSmods_a_OBJECTS) $(libXNSdrvs_a_OBJECTS) $(ADDON_MODS)
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

GENKSYMS_CPPFLAGS		= \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

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

##
## Rules for doxygen documentation.
##
include am/doxy.am

EXTRA_DIST			+= \
				  TODO \
				  MANUAL \
				  README-cvs \
				  README-LiS \
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
