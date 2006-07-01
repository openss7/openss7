# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.60 $) $Date: 2006/07/01 11:59:45 $
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
# Last Modified $Date: 2006/07/01 11:59:45 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.60  2006/07/01 11:59:45  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.59  2006/03/21 13:24:44  brian
# - included pr.am
#
# Revision 0.9.2.58  2006/03/20 14:01:57  brian
# - fixed error on SUBDIRS
#
# Revision 0.9.2.57  2006/03/20 11:54:07  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.56  2006/03/07 12:34:25  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.55  2006/03/06 11:39:16  brian
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
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

pkglibexec_PROGRAMS		= 
if WITH_TLI
## WITH_TLI
pkglibexec_PROGRAMS		+= \
				  test-tirdwr \
				  test-timod
## WITH_TLI
endif
if WITH_XNET
## WITH_XNET
pkglibexec_PROGRAMS		+= \
				  test-xnet \
				  test-xnet_thr
## WITH_XNET
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

if WITH_TLI
## WITH_TLI
test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDADD		= $(STREAMS_LDADD)
test_tirdwr_LDFLAGS		= -static
test_tirdwr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tirdwr_CFLAGS		= $(USER_CFLAGS)

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDADD		= $(STREAMS_LDADD)
test_timod_LDFLAGS		= -static
test_timod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_timod_CFLAGS		= $(USER_CFLAGS)
## WITH_TLI
endif

if WITH_XNET
## WITH_XNET
test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDADD			= libxnet.la $(STREAMS_LDADD)
test_xnet_LDFLAGS		= -static
test_xnet_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnet_CFLAGS		= $(USER_CFLAGS)

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDADD		= libxnet.la $(STREAMS_LDADD) -lpthread
test_xnet_thr_LDFLAGS		= -static
test_xnet_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS)

lib_LTLIBRARIES			+= libxnet.la

libxnet_la_SOURCES		= src/lib/xnet.c gettext.h
libxnet_la_LDFLAGS		= -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS)
libxnet_la_LIBADD		= 
## WITH_XNET
endif

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libXNETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libXNETmods_a_SOURCES = \
				  src/modules/tirdwr.c \
				  src/modules/timod.c

mods_source_FILES		= $(nodist_EXTRA_libXNETmods_a_SOURCES)

nodist_libXNETmods_a_SOURCES	= $(mods_source_FILES)

libXNETmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libXNETmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libXNETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

pkglibexec_PROGRAMS		= 
if WITH_TLI
## WITH_TLI
pkglibexec_PROGRAMS		+= \
				  test-tirdwr \
				  test-timod
## WITH_TLI
endif
if WITH_XNET
## WITH_XNET
pkglibexec_PROGRAMS		+= \
				  test-xnet \
				  test-xnet_thr
## WITH_XNET
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

if WITH_TLI
## WITH_TLI
test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDADD		= $(STREAMS_LDADD)
test_tirdwr_LDFLAGS		= -static
test_tirdwr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tirdwr_CFLAGS		= $(USER_CFLAGS)

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDADD		= $(STREAMS_LDADD)
test_timod_LDFLAGS		= -static
test_timod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_timod_CFLAGS		= $(USER_CFLAGS)
## WITH_TLI
endif

if WITH_XNET
## WITH_XNET
test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDADD			= libxnet.la $(STREAMS_LDADD)
test_xnet_LDFLAGS		= -static
test_xnet_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnet_CFLAGS		= $(USER_CFLAGS)

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -static
test_xnet_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS)

lib_LTLIBRARIES			+= libxnet.la

libxnet_la_SOURCES		= src/lib/xnet.c gettext.h
libxnet_la_LDFLAGS		= $(STREAMS_LDADD) -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS)
libxnet_la_LIBADD		= 
## WITH_XNET
endif

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libXNETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libXNETmods_a_SOURCES = \
				  src/modules/tirdwr.c \
				  src/modules/timod.c

mods_source_FILES		= $(nodist_EXTRA_libXNETmods_a_SOURCES)

nodist_libXNETmods_a_SOURCES	= $(mods_source_FILES)

libXNETmods_a_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
libXNETmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libXNETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif

## ! PUBLIC_RELEASE
endif

## PKG_BUILD_ARCH
endif

xnet_source_FILES		= $(mods_source_FILES)

EXTRA_DIST			+= $(xnet_source_FILES)

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
KERNEL_OBJECTS			= $(libXNETmods_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libXNETmods_a_OBJECTS) $(ADDON_MODS)
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
include am/libraries.am
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
include am/strconf.am

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src

if PKG_BUILD_ARCH
SUBDIRS += intl m4 po
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
AM_RPMFLAGS 			= --define "_kversion $(kversion)"
AM_SRPMFLAGS 			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

EXTRA_DIST			+= \
				  TODO \
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
