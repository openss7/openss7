# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/09/29 11:50:14 $
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
# Last Modified $Date: 2006/09/29 11:50:14 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.10  2006/09/29 11:50:14  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.9  2006/09/27 05:15:14  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.8  2006/09/18 01:15:29  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.7  2006/08/26 09:17:43  brian
# - better release file generation
#
# Revision 0.9.2.6  2006/07/01 11:59:41  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.5  2006/03/21 13:24:38  brian
# - included pr.am
#
# Revision 0.9.2.4  2006/03/20 11:54:00  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.3  2006/03/14 21:09:54  brian
# - two levels of distro specific rpm extra tags
#
# Revision 0.9.2.2  2006/03/08 23:20:40  brian
# - changes for successful build
#
# Revision 0.9.2.1  2006/03/08 08:20:05  brian
# - starting off strbcm package
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

EXTRA_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod

pkglibexec_PROGRAMS		= \
				  test-bcm

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

bin_PROGRAMS			= 

dist_sbin_SCRIPTS		= 

#BCM_EXTRA_TESTS		=
#BCM_EXTRA_TOOLS		=


@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_bcm_SOURCES		= src/test/test-bcm.c
test_bcm_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_bcm_LDADD			= $(STREAMS_LDADD)
test_bcm_LDFLAGS		= $(STREAMS_LDFLAGS)
test_bcm_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_bcm_CFLAGS			= $(USER_CFLAGS)

#xxxxxx_SOURCES			= src/xxxxxx.c
#xxxxxx_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
#xxxxxx_LDADD			= $(STREAMS_LDADD)
#xxxxxx_LDFLAGS			= $(STREAMS_LDFLAGS)
#xxxxxx_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#xxxxxx_CFLAGS			= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libBCMlink.a libBCMmods.a libBCMdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

EXTRA_libBCMlink_a_SOURCES	= 
libBCMlink_a_SOURCES		= 
libBCMlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libBCMlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libBCMlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libBCMlink_a_OBJECTS):	$(libBCMlink_a_SOURCES) $(libBCMlink_a_DEPENDENCIES)

EXTRA_libBCMmods_a_SOURCES	= 
libBCMmods_a_SOURCES		= \
				  src/modules/testmod.h \
				  src/modules/testmod.c
libBCMmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libBCMmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libBCMmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libBCMmods_a_OBJECTS):	$(libBCMmods_a_DEPENDENCIES)

EXTRA_libBCMdrvs_a_SOURCES	= 
libBCMdrvs_a_SOURCES		= \
				  src/drivers/testdrv.h \
				  src/drivers/testdrv.c
libBCMdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libBCMdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libBCMdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libBCMdrvs_a_OBJECTS):	$(libBCMdrvs_a_DEPENDENCIES)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

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
KERNEL_OBJECTS			= $(libBCMlink_a_OBJECTS) $(libBCMmods_a_OBJECTS) $(libBCMdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libBCMmods_a_OBJECTS) $(libBCMdrvs_a_OBJECTS) $(ADDON_MODS)
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
AUTOCONFIG_H			= src/$(PKGINCL)/config.h
##
##EXTRA_DIST			+= $(AUTOCONFIG_H).in
##
EXPORT_SRCFILES			=

EXPORT_VERFILES			=

GENKSYMS_CPPFLAGS		= \
				  -I./pkg -I$(srcdir)/pkg \
				  -I./src/drivers -I$(srcdir)/src/drivers \
				  -I./src/modules -I$(srcdir)/src/modules \
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

CLEANFILES += pkg/*_wrapper.c

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
