# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2007/03/08 06:08:46 $
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
# Last Modified $Date: 2007/03/08 06:08:46 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.25  2007/03/08 06:08:46  brian
# - modutil_DATA should not be distributed (and it reorders uninstall)
#
# Revision 0.9.2.24  2007/03/04 00:23:45  brian
# - add version file creation
#
# Revision 0.9.2.23  2007/02/26 07:25:46  brian
# - synchronizing changes
#
# Revision 0.9.2.22  2006/10/03 13:51:56  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.21  2006/10/01 21:30:17  brian
# - distribute release and library version files
#
# Revision 0.9.2.20  2006/09/29 11:50:18  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.19  2006/09/27 05:15:15  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.18  2006/09/18 01:15:30  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.17  2006/08/26 09:18:01  brian
# - better release file generation
#
# Revision 0.9.2.16  2006/07/01 11:59:42  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.15  2006/03/21 13:24:39  brian
# - included pr.am
#
# Revision 0.9.2.14  2006/03/20 11:54:01  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.13  2006/03/07 12:34:20  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.12  2006/03/06 11:38:34  brian
# - updated headers
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

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

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

## xxx_SOURCES			=
## xxx_LDADD			= $(STREAMS_LDADD)
## xxx_LDFLAGS			= $(STREAMS_LDFLAGS)
## xxx_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
## xxx_CFLAGS			= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libCOMPATlink.a libCOMPATmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

EXTRA_libCOMPATlink_a_SOURCES 	= \
				  src/kernel/os7compat.c \
				  src/kernel/svr3compat.c \
				  src/kernel/svr4compat.c \
				  src/kernel/mpscompat.c \
				  src/kernel/suncompat.c \
				  src/kernel/uw7compat.c \
				  src/kernel/osfcompat.c \
				  src/kernel/aixcompat.c \
				  src/kernel/hpuxcompat.c \
				  src/kernel/irixcompat.c \
				  src/kernel/liscompat.c \
				  src/kernel/lfscompat.c \
				  src/kernel/maccompat.c

libCOMPATlink_a_SOURCES		= \
				  src/include/sys/os7/allocb.h \
				  src/include/sys/os7/bufpool.h \
				  src/include/sys/os7/bufq.h \
				  src/include/sys/os7/compat.h \
				  src/include/sys/os7/debug.h \
				  src/include/sys/os7/lock.h \
				  src/include/sys/os7/priv.h \
				  src/include/sys/os7/queue.h \
				  src/include/sys/os7/timer.h

libCOMPATlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_COMPAT_OS7
libCOMPATlink_a_SOURCES		+= src/kernel/os7compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/os7/ddi.h src/include/sys/os7/strconf.h src/include/sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3
libCOMPATlink_a_SOURCES		+= src/kernel/svr3compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/svr3/ddi.h src/include/sys/svr3/strconf.h src/include/sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4
libCOMPATlink_a_SOURCES		+= src/kernel/svr4compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/svr4/ddi.h src/include/sys/svr4/strconf.h src/include/sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS
libCOMPATlink_a_SOURCES		+= src/kernel/mpscompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/mps/ddi.h src/include/sys/mps/strconf.h src/include/sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN
libCOMPATlink_a_SOURCES		+= src/kernel/suncompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/sunddi.h src/include/sys/strsun.h src/include/sys/sun/ddi.h src/include/sys/sun/strconf.h src/include/sys/sun/strsun.h src/include/sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7
libCOMPATlink_a_SOURCES		+= src/kernel/uw7compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/uw7/ddi.h src/include/sys/uw7/strconf.h src/include/sys/uw7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF
libCOMPATlink_a_SOURCES		+= src/kernel/osfcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/osf/ddi.h src/include/sys/osf/strconf.h src/include/sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_AIX
libCOMPATlink_a_SOURCES		+= src/kernel/aixcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/aix/ddi.h src/include/sys/aix/strconf.h src/include/sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX
libCOMPATlink_a_SOURCES		+= src/kernel/hpuxcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/hpux/ddi.h src/include/sys/hpux/strconf.h src/include/sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX
libCOMPATlink_a_SOURCES		+= src/kernel/irixcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/irix/ddi.h src/include/sys/irix/strconf.h src/include/sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_LIS
libCOMPATlink_a_SOURCES		+= src/kernel/liscompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/lis/ddi.h src/include/sys/lis/strconf.h src/include/sys/lis/stream.h
endif
if CONFIG_STREAMS_COMPAT_LFS
libCOMPATlink_a_SOURCES		+= src/kernel/lfscompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/lfs/ddi.h src/include/sys/lfs/strconf.h src/include/sys/lfs/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC
libCOMPATlink_a_SOURCES		+= src/kernel/maccompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/mac/ddi.h src/include/sys/mac/strconf.h src/include/sys/mac/stream.h
endif

libCOMPATlink_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libCOMPATlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCOMPATlink_a_OBJECTS):	$(libCOMPATlink_a_SOURCES) $(libCOMPATlink_a_DEPENDENCIES)

EXTRA_libCOMPATmods_a_SOURCES	= \
				  src/kernel/os7compat.c \
				  src/kernel/svr3compat.c \
				  src/kernel/svr4compat.c \
				  src/kernel/mpscompat.c \
				  src/kernel/suncompat.c \
				  src/kernel/uw7compat.c \
				  src/kernel/osfcompat.c \
				  src/kernel/aixcompat.c \
				  src/kernel/hpuxcompat.c \
				  src/kernel/irixcompat.c \
				  src/kernel/liscompat.c \
				  src/kernel/lfscompat.c \
				  src/kernel/maccompat.c

libCOMPATmods_a_SOURCES		= \
				  src/include/sys/os7/allocb.h \
				  src/include/sys/os7/bufpool.h \
				  src/include/sys/os7/bufq.h \
				  src/include/sys/os7/compat.h \
				  src/include/sys/os7/debug.h \
				  src/include/sys/os7/lock.h \
				  src/include/sys/os7/priv.h \
				  src/include/sys/os7/queue.h \
				  src/include/sys/os7/timer.h

libCOMPATmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_COMPAT_OS7_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/os7compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/os7/ddi.h src/include/sys/os7/strconf.h src/include/sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/svr3compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/svr3/ddi.h src/include/sys/svr3/strconf.h src/include/sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/svr4compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/svr4/ddi.h src/include/sys/svr4/strconf.h src/include/sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/mpscompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/mps/ddi.h src/include/sys/mps/strconf.h src/include/sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/suncompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/sunddi.h src/include/sys/strsun.h src/include/sys/sun/ddi.h src/include/sys/sun/strconf.h src/include/sys/sun/strsun.h src/include/sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/uw7compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/uw7/ddi.h src/include/sys/uw7/strconf.h src/include/sys/uw7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/osfcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/osf/ddi.h src/include/sys/osf/strconf.h src/include/sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/aixcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/aix/ddi.h src/include/sys/aix/strconf.h src/include/sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/hpuxcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/hpux/ddi.h src/include/sys/hpux/strconf.h src/include/sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/irixcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/irix/ddi.h src/include/sys/irix/strconf.h src/include/sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/liscompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/lis/ddi.h src/include/sys/lis/strconf.h src/include/sys/lis/stream.h
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/lfscompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/lfs/ddi.h src/include/sys/lfs/strconf.h src/include/sys/lfs/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/maccompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/mac/ddi.h src/include/sys/mac/strconf.h src/include/sys/mac/stream.h
endif

libCOMPATmods_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libCOMPATmods_a_CFLAGS		= $(KERNEL_CFLAGS)

EXTRA_KERNEL_MODULES		+=

## RPM_BUILD_KERNEL
endif

## PKG_BUILD_ARCH
endif

EXTRA_DIST			+=

EXTRA_TEX			+= \
				scripts/texinfo.cnf \
				scripts/texi/args.texi.in \
				scripts/texi/fdl.texi \
				scripts/texi/gpl.texi \
				scripts/texi/install.texi \
				scripts/texi/lesser.texi \
				scripts/texi/testsuite.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+= configure.nexusware

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libCOMPATlink_a_OBJECTS) $(libCOMPATmods_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libCOMPATmods_a_OBJECTS) $(ADDON_MODS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			= $(STREAMS_MODMAP)

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

if CONFIG_STREAMS_COMPAT_AIX
EXPORT_SRCFILES			+= src/kernel/aixcompat.c
endif
if CONFIG_STREAMS_COMPAT_HPUX
EXPORT_SRCFILES			+= src/kernel/hpuxcompat.c
endif
if CONFIG_STREAMS_COMPAT_IRIX
EXPORT_SRCFILES			+= src/kernel/irixcompat.c
endif
if CONFIG_STREAMS_COMPAT_LIS
EXPORT_SRCFILES			+= src/kernel/liscompat.c
endif
if CONFIG_STREAMS_COMPAT_LFS
EXPORT_SRCFILES			+= src/kernel/lfscompat.c
endif
if CONFIG_STREAMS_COMPAT_MAC
EXPORT_SRCFILES			+= src/kernel/maccompat.c
endif
if CONFIG_STREAMS_COMPAT_MPS
EXPORT_SRCFILES			+= src/kernel/mpscompat.c
endif
if CONFIG_STREAMS_COMPAT_OS7
EXPORT_SRCFILES			+= src/kernel/os7compat.c
endif
if CONFIG_STREAMS_COMPAT_OSF
EXPORT_SRCFILES			+= src/kernel/osfcompat.c
endif
if CONFIG_STREAMS_COMPAT_SUN
EXPORT_SRCFILES			+= src/kernel/suncompat.c
endif
if CONFIG_STREAMS_COMPAT_SVR3
EXPORT_SRCFILES			+= src/kernel/svr3compat.c
endif
if CONFIG_STREAMS_COMPAT_SVR4
EXPORT_SRCFILES			+= src/kernel/svr4compat.c
endif
if CONFIG_STREAMS_COMPAT_UW7
EXPORT_SRCFILES			+= src/kernel/uw7compat.c
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
EXPORT_SRCFILES			+= src/kernel/aixcompat.c
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
EXPORT_SRCFILES			+= src/kernel/hpuxcompat.c
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
EXPORT_SRCFILES			+= src/kernel/irixcompat.c
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SRCFILES			+= src/kernel/liscompat.c
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
EXPORT_SRCFILES			+= src/kernel/lfscompat.c
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
EXPORT_SRCFILES			+= src/kernel/maccompat.c
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
EXPORT_SRCFILES			+= src/kernel/mpscompat.c
endif
if CONFIG_STREAMS_COMPAT_OS7_MODULE
EXPORT_SRCFILES			+= src/kernel/os7compat.c
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
EXPORT_SRCFILES			+= src/kernel/osfcompat.c
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
EXPORT_SRCFILES			+= src/kernel/suncompat.c
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
EXPORT_SRCFILES			+= src/kernel/svr3compat.c
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
EXPORT_SRCFILES			+= src/kernel/svr4compat.c
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
EXPORT_SRCFILES			+= src/kernel/uw7compat.c
endif

EXPORT_VERFILES			=

if CONFIG_STREAMS_COMPAT_AIX
EXPORT_VERFILES			+= $(PKGINCL)/versions/aixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_HPUX
EXPORT_VERFILES			+= $(PKGINCL)/versions/hpuxcompat.ver
endif
if CONFIG_STREAMS_COMPAT_IRIX
EXPORT_VERFILES			+= $(PKGINCL)/versions/irixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_LIS
EXPORT_VERFILES			+= $(PKGINCL)/versions/liscompat.ver
endif
if CONFIG_STREAMS_COMPAT_LFS
EXPORT_VERFILES			+= $(PKGINCL)/versions/lfscompat.ver
endif
if CONFIG_STREAMS_COMPAT_MAC
EXPORT_VERFILES			+= $(PKGINCL)/versions/maccompat.ver
endif
if CONFIG_STREAMS_COMPAT_MPS
EXPORT_VERFILES			+= $(PKGINCL)/versions/mpscompat.ver
endif
if CONFIG_STREAMS_COMPAT_OS7
EXPORT_VERFILES			+= $(PKGINCL)/versions/os7compat.ver
endif
if CONFIG_STREAMS_COMPAT_OSF
EXPORT_VERFILES			+= $(PKGINCL)/versions/osfcompat.ver
endif
if CONFIG_STREAMS_COMPAT_SUN
EXPORT_VERFILES			+= $(PKGINCL)/versions/suncompat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR3
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr3compat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR4
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr4compat.ver
endif
if CONFIG_STREAMS_COMPAT_UW7
EXPORT_VERFILES			+= $(PKGINCL)/versions/uw7compat.ver
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/aixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/hpuxcompat.ver
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/irixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/liscompat.ver
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/lfscompat.ver
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/maccompat.ver
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/mpscompat.ver
endif
if CONFIG_STREAMS_COMPAT_OS7_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/os7compat.ver
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/osfcompat.ver
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/suncompat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr3compat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr4compat.ver
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/uw7compat.ver
endif

GENKSYMS_CPPFLAGS		= \
				  -I./src/kernel -I$(srcdir)/src/kernel \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

EXTRA_DIST			+= \
				  $(top_srcdir)/scripts/hot.lds

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
				  src/util/init/@PACKAGE_LCNAME@.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/@PACKAGE_LCNAME@

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				  src/util/sysconfig/@PACKAGE_LCNAME@.conf

dist_config_DATA		= \
				  src/util/sysconfig/@PACKAGE_LCNAME@

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
