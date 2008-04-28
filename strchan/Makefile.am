# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2008-04-28 23:39:48 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2008-04-28 23:39:48 $ by $Author: brian $
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

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod \
				  x400pmib \
				  mxmib \
				  phymib \
				  test-chan

pkglibexec_PROGRAMS		= test-chan

sbin_PROGRAMS			= @PACKAGE_TARNAME@_mknod

if WITH_SNMP_AGENT
sbin_PROGRAMS			+= x400pmib mxmib phymib
endif

bin_PROGRAMS			= 

dist_sbin_SCRIPTS		= 

#CHAN_EXTRA_TESTS		=
#CHAN_EXTRA_TOOLS		=


@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

x400pmib_SOURCES		= src/snmp/x400pMIB.c \
				  src/snmp/x400pMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
x400pmib_LDFLAGS		= $(SNMP_LDFLAGS)
x400pmib_LDADD			= $(STREAMS_LDADD) $(SNMP_LDADD)
x400pmib_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
x400pmib_CFLAGS			= $(USER_CFLAGS)

mxmib_SOURCES			= src/snmp/mxMIB.c \
				  src/snmp/mxMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
mxmib_LDFLAGS			= $(SNMP_LDFLAGS)
mxmib_LDADD			= $(STREAMS_LDADD) $(SNMP_LDADD)
mxmib_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
mxmib_CFLAGS			= $(USER_CFLAGS)

phymib_SOURCES			= src/snmp/phyMIB.c \
				  src/snmp/phyMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
phymib_LDFLAGS			= $(SNMP_LDFLAGS)
phymib_LDADD			= $(STREAMS_LDADD) $(SNMP_LDADD)
phymib_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
phymib_CFLAGS			= $(USER_CFLAGS)

test_chan_SOURCES		= src/test/test-chan.c
test_chan_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_chan_LDADD			= $(STREAMS_LDADD)
test_chan_LDFLAGS		= $(STREAMS_LDFLAGS)
test_chan_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_chan_CFLAGS		= $(USER_CFLAGS)

#xxxxxx_SOURCES			= src/xxxxxx.c
#xxxxxx_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
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

noinst_LIBRARIES		= libCHANlink.a libCHANmods.a libCHANdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

EXTRA_libCHANlink_a_SOURCES	= 
libCHANlink_a_SOURCES		= \
				  src/modules/ch_pmod.c \
				  src/modules/mx_pmod.c \
				  config.h strconf.h
libCHANlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libCHANlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libCHANlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCHANlink_a_OBJECTS):	$(libCHANlink_a_SOURCES) $(libCHANlink_a_DEPENDENCIES)

EXTRA_libCHANmods_a_SOURCES	=
libCHANmods_a_SOURCES		= 
libCHANmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libCHANmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libCHANmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCHANmods_a_OBJECTS):	$(libCHANmods_a_DEPENDENCIES)

EXTRA_libCHANdrvs_a_SOURCES	=
libCHANdrvs_a_SOURCES		=
libCHANdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libCHANdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libCHANdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCHANdrvs_a_OBJECTS):	$(libCHANdrvs_a_DEPENDENCIES)

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
dist_noinst_SCRIPTS		+= scripts/missing2
EXTRA_SCRIPTS			+= scripts/missing2

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libCHANlink_a_OBJECTS) $(libCHANmods_a_OBJECTS) $(libCHANdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libCHANmods_a_OBJECTS) $(libCHANdrvs_a_OBJECTS) $(ADDON_MODS)
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

if DEVELOPMENT
if DOCUMENTATION
SUBDIRS += doc
endif
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
# Revision 0.9.2.12  2008-04-28 23:39:48  brian
# - updated headers for release
#
# Revision 0.9.2.11  2008-04-10 23:36:23  brian
# - get Debian 4.0 deb/dsc build working
#
# Revision 0.9.2.10  2008-04-10 10:24:01  brian
# - removed deprecated Nexusware support
#
# Revision 0.9.2.9  2008/01/01 13:59:34  brian
# - added new mibs
#
# Revision 0.9.2.8  2007/10/18 06:52:48  brian
# - new build support
#
# Revision 0.9.2.7  2007/08/14 06:47:09  brian
# - GPLv3 header update
#
# Revision 0.9.2.6  2007/07/14 01:35:31  brian
# - make license explicit, add documentation
#
# Revision 0.9.2.5  2007/03/13 06:17:04  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.4  2007/03/08 06:08:45  brian
# - modutil_DATA should not be distributed (and it reorders uninstall)
#
# Revision 0.9.2.3  2007/03/04 00:23:42  brian
# - add version file creation
#
# Revision 0.9.2.2  2006/12/29 05:51:13  brian
# - changes for successful master build
#
# Revision 0.9.2.1  2006/10/13 07:57:26  brian
# - added initial STREAMS Channel package
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
