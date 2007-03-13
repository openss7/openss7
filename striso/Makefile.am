# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2007/03/13 06:17:19 $
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
# Last Modified $Date: 2007/03/13 06:17:19 $ by $Author: brian $
#
# =============================================================================

##
## General rules
##
include am/openss7.am

##
## Version file
##
include am/version.am

##
## Help
##
include am/help.am

##
## Problem Reports
##
include am/pr.am

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if PUBLIC_RELEASE
## PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod

pkglibexec_PROGRAMS		= 

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libISOmods.a libISOdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libISOmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c

dist_EXTRA_libISOmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c

mods_source_FILES		= $(nodist_EXTRA_libISOmods_a_SOURCES)

nodist_libISOmods_a_SOURCES	= $(mods_source_FILES)

libISOmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libISOmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libISOmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libISOdrvs_a_SOURCES = \
				  src/drivers/clns.c \
				  src/drivers/x25-lapb.c \
				  src/drivers/x25-plp.c

dist_EXTRA_libISOdrvs_a_SOURCES = \
				  src/drivers/clns.c \
				  src/drivers/tp.c \
				  src/drivers/x25-lapb.c \
				  src/drivers/x25-plp.c

drvs_source_FILES		= $(nodist_EXTRA_libISOdrvs_a_SOURCES)

nodist_libISOdrvs_a_SOURCES	= $(drvs_source_FILES)

libISOdrvs_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libISOdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libISOdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)
libISOdrvs_a_OBJECTS:		$(libISOdrvs_a_DEPENDENCIES)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= 

pkglibexec_PROGRAMS		= 


sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		= 
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libISOmods.a libISOdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libISOmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c

dist_EXTRA_libISOmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c

mods_source_FILES		= $(nodist_EXTRA_libISOmods_a_SOURCES)

nodist_libISOmods_a_SOURCES	= $(mods_source_FILES)

libISOmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libISOmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libISOmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libISOdrvs_a_SOURCES = \
				  src/drivers/clns.c \
				  src/drivers/x25-lapb.c \
				  src/drivers/x25-plp.c

dist_EXTRA_libISOdrvs_a_SOURCES = \
				  src/drivers/clns.c \
				  src/drivers/tp.c \
				  src/drivers/x25-lapb.c \
				  src/drivers/x25-plp.c

drvs_source_FILES		= $(nodist_EXTRA_libISOdrvs_a_SOURCES)

nodist_libISOdrvs_a_SOURCES	= $(drvs_source_FILES)

libISOdrvs_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libISOdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libISOdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)
libISOdrvs_a_OBJECTS:		$(libISOdrvs_a_DEPENDENCIES)

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
KERNEL_OBJECTS			= $(libISOmods_a_OBJECTS) $(libISOdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libISOmods_a_OBJECTS) $(libISOdrvs_a_OBJECTS) $(ADDON_MODS)
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
nodist_modutil_DATA		= \
				src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
##LIBRARY_VERSION_FILES		=
##
##include am/libraries.am

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
## Rules for net config files
##
include am/netconfig.am
include am/sock2path.am

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
				  README-LiS \
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

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.13  2007/03/13 06:17:19  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.12  2007/03/08 06:08:52  brian
# - modutil_DATA should not be distributed (and it reorders uninstall)
#
# Revision 0.9.2.11  2007/03/04 00:23:57  brian
# - add version file creation
#
# Revision 0.9.2.10  2006/12/29 05:51:34  brian
# - changes for successful master build
#
# Revision 0.9.2.9  2006/10/16 06:09:37  brian
# - added netconfig and sock2path config files for packages
#
# Revision 0.9.2.8  2006/10/03 13:52:14  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.7  2006/10/01 21:30:21  brian
# - distribute release and library version files
#
# Revision 0.9.2.6  2006/09/18 01:15:32  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.5  2006/08/26 09:18:52  brian
# - better release file generation
#
# Revision 0.9.2.4  2006/07/11 12:32:02  brian
# - added ISO and other implementations to distribution
#
# Revision 0.9.2.3  2006/07/11 09:17:18  brian
# - changes to get striso subpackage to package
#
# Revision 0.9.2.2  2006/04/08 03:39:14  brian
# - working up striso package
#
# Revision 0.9.2.1  2006/04/07 22:01:25  brian
# - initial branch
#
# Revision 0.9  2006/04/07 21:56:37  brian
# - initial addition of striso files
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor nocin nosi
