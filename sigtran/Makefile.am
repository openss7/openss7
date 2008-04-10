# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2008-04-10 10:24:00 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2008-04-10 10:24:00 $ by $Author: brian $
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
				  test-sigtran \
				  test-m2pa \
				  test-m2ua_as

pkglibexec_PROGRAMS		= test-sigtran \
				  test-m2pa \
				  test-m2ua_as

sbin_PROGRAMS			= @PACKAGE_TARNAME@_mknod \
				  m2paconfig

bin_PROGRAMS			= 

dist_sbin_SCRIPTS		= 

#SIGTRAN_EXTRA_TESTS		=
#SIGTRAN_EXTRA_TOOLS		=


@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

m2paconfig_SOURCES		= src/util/m2payac.y \
				  src/util/m2palex.l
m2paconfig_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
m2paconfig_LDADD		= $(STREAMS_LDADD)
m2paconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
m2paconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
m2paconfig_CFLAGS		= $(USER_CFLAGS)

test_sigtran_SOURCES		= src/test/test-sigtran.c
test_sigtran_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_sigtran_LDADD		= $(STREAMS_LDADD)
test_sigtran_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sigtran_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sigtran_CFLAGS		= $(USER_CFLAGS)

test_m2pa_SOURCES		= src/test/test-m2pa.c
test_m2pa_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_m2pa_LDADD			= $(STREAMS_LDADD)
test_m2pa_LDFLAGS		= $(STREAMS_LDFLAGS)
test_m2pa_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_m2pa_CFLAGS		= $(USER_CFLAGS)

test_m2ua_as_SOURCES		= src/test/test-m2ua_as.c
test_m2ua_as_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_m2ua_as_LDADD		= $(STREAMS_LDADD)
test_m2ua_as_LDFLAGS		= $(STREAMS_LDFLAGS)
test_m2ua_as_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_m2ua_as_CFLAGS		= $(USER_CFLAGS)

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

noinst_LIBRARIES		= libSIGTRANlink.a libSIGTRANmods.a libSIGTRANdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

EXTRA_libSIGTRANlink_a_SOURCES	= 
libSIGTRANlink_a_SOURCES	= 
libSIGTRANlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSIGTRANlink_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libSIGTRANlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSIGTRANlink_a_OBJECTS):	$(libSIGTRANlink_a_SOURCES) $(libSIGTRANlink_a_DEPENDENCIES)

EXTRA_libSIGTRANmods_a_SOURCES	= \
				src/modules/m2tp.c
libSIGTRANmods_a_SOURCES	= \
				src/modules/m2pa_sl.c \
				src/modules/m2ua_as.c \
				src/modules/m3ua_as.c \
				src/modules/sdl_sctp.c \
				src/modules/sdt_sctp.c \
				src/modules/sdt_tpi.c \
				src/modules/sl_tpi.c
libSIGTRANmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSIGTRANmods_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libSIGTRANmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSIGTRANmods_a_OBJECTS):	$(libSIGTRANmods_a_DEPENDENCIES)

EXTRA_libSIGTRANdrvs_a_SOURCES	= \
				src/drivers/isua_as.c \
				src/drivers/m2ua.c \
				src/drivers/m2ua_as.c \
				src/drivers/m3ua_as.c \
				src/drivers/m3ua_asp.c \
				src/drivers/m3ua_asp.h \
				src/drivers/m3ua.c \
				src/drivers/m3ua_data.h \
				src/drivers/m3ua.h \
				src/drivers/m3ua_lm.c \
				src/drivers/m3ua_lm.h \
				src/drivers/m3ua_lower.c \
				src/drivers/m3ua_msg.c \
				src/drivers/m3ua_msg.h \
				src/drivers/m3ua_mtp.c \
				src/drivers/m3ua_mtp.h \
				src/drivers/m3ua_mtpp.c \
				src/drivers/m3ua_mtpu.c \
				src/drivers/m3ua_nsp.c \
				src/drivers/m3ua_nsp.h \
				src/drivers/m3ua_sg.c \
				src/drivers/m3ua_sgp.c \
				src/drivers/m3ua_spp.c \
				src/drivers/m3ua_ss7.c \
				src/drivers/m3ua_tsp.c \
				src/drivers/m3ua_tsp.h \
				src/drivers/sua_as.c \
				src/drivers/sua_asp.c \
				src/drivers/sua_asp.h \
				src/drivers/sua.c \
				src/drivers/sua_ctrl.h \
				src/drivers/sua_data.h \
				src/drivers/sua.h \
				src/drivers/sua_lm.c \
				src/drivers/sua_lm.h \
				src/drivers/sua_lower.c \
				src/drivers/sua_msg.c \
				src/drivers/sua_msg.h \
				src/drivers/sua_nsp.c \
				src/drivers/sua_nsp.h \
				src/drivers/sua_sccp.c \
				src/drivers/sua_sccp.h \
				src/drivers/sua_send.h \
				src/drivers/sua_sgp.c \
				src/drivers/sua_sm.c \
				src/drivers/sua_spp.c \
				src/drivers/sua_ss7.c \
				src/drivers/sua_tsp.c \
				src/drivers/sua_tsp.h \
				src/drivers/tali.c \
				src/drivers/tali_data.h \
				src/drivers/tali.h \
				src/drivers/tali_isup.c \
				src/drivers/tali_lm.c \
				src/drivers/tali_msgs.h \
				src/drivers/tali_mtp.c \
				src/drivers/tali_sccp.c \
				src/drivers/tua_as.c \
				src/drivers/tua_asp.c \
				src/drivers/tua_asp.h \
				src/drivers/tua.c \
				src/drivers/tua_data.h \
				src/drivers/tua.h \
				src/drivers/tua_msg.h \
				src/drivers/tua_sgp.c \
				src/drivers/tua_spp.c \
				src/drivers/tua_ss7.c \
				src/drivers/ua_msg.h
libSIGTRANdrvs_a_SOURCES	= \
				src/drivers/ua_as.c \
				src/drivers/m2ua_as.c \
				src/drivers/m3ua_as.c \
				src/drivers/sua_as.c \
				src/drivers/tua_as.c \
				src/drivers/isua_as.c
libSIGTRANdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSIGTRANdrvs_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libSIGTRANdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSIGTRANdrvs_a_OBJECTS):	$(libSIGTRANdrvs_a_DEPENDENCIES)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

EXTRA_DOCS			+= \
				doc/drafts/isua/draft-bidulock-sigtran-isua-03.txt \
				doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-ig-00.txt \
				doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-mib-00.txt \
				doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-07.txt \
				doc/drafts/m2pa/draft-george-sigtran-m2pa-interop-01.txt \
				doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-02.txt \
				doc/drafts/m3pa/draft-averson-sigtran-m3pa-00.txt \
				doc/drafts/m3pa/draft-bidulock-sigtran-m3pa-01.txt \
				doc/drafts/m3ua/draft-bidulock-sigtran-m3gw-00.txt \
				doc/drafts/m3ua/draft-bidulock-sigtran-m3peer-01.txt \
				doc/drafts/m3ua/draft-bidulock-sigtran-m3ua-test-00.txt \
				doc/drafts/sctp/draft-bidulock-sigtran-gapalg-00.txt \
				doc/drafts/sctp/draft-bidulock-sigtran-sctpcongestion-00.txt \
				doc/drafts/sctp/draft-bidulock-sigtran-sctptli-00.txt \
				doc/drafts/sctp/draft-bidulock-tsvwg-sctpnpi-00.txt \
				doc/drafts/sctp/draft-bidulock-tsvwg-sctptpi-00.txt \
				doc/drafts/sua/draft-bidulock-sigtran-suaco-00.txt \
				doc/drafts/sua/draft-bidulock-sigtran-sua-test-00.txt \
				doc/drafts/tua/draft-bidulock-sigtran-tua-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-as-00.txt \
				doc/drafts/ua/draft-bidulock-sigtran-aspcong-00.txt \
				doc/drafts/ua/draft-bidulock-sigtran-aspext-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-corid-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-datamodel-00.txt \
				doc/drafts/ua/draft-bidulock-sigtran-loadgrp-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-loadsel-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-multisg-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-mutilsg-00.txt \
				doc/drafts/ua/draft-bidulock-sigtran-regext-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-sessid-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-sginfo-05.txt

EXTRA_DIST			+= $(EXTRA_DOCS)

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
KERNEL_OBJECTS			= $(libSIGTRANlink_a_OBJECTS) $(libSIGTRANmods_a_OBJECTS) $(libSIGTRANdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libSIGTRANmods_a_OBJECTS) $(libSIGTRANdrvs_a_OBJECTS) $(ADDON_MODS)
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
# Revision 0.9.2.16  2008-04-10 10:24:00  brian
# - removed deprecated Nexusware support
#
# Revision 0.9.2.15  2007/10/18 06:52:41  brian
# - new build support
#
# Revision 0.9.2.14  2007/08/19 11:48:02  brian
# - move stdbool.h, bison changes
#
# Revision 0.9.2.13  2007/08/18 03:53:07  brian
# - working up configuration files
#
# Revision 0.9.2.12  2007/08/14 08:33:17  brian
# - GPLv3 header update
#
# Revision 0.9.2.11  2007/03/13 06:16:58  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.10  2007/03/08 06:08:28  brian
# - modutil_DATA should not be distributed (and it reorders uninstall)
#
# Revision 0.9.2.9  2007/03/04 00:23:32  brian
# - add version file creation
#
# Revision 0.9.2.8  2007/02/14 14:08:45  brian
# - broad changes updating support for SS7 MTP and M3UA
#
# Revision 0.9.2.7  2007/02/10 22:32:01  brian
# - working up sigtran drivers
#
# Revision 0.9.2.6  2007/01/23 10:00:39  brian
# - added test program and m2ua-as updates
#
# Revision 0.9.2.5  2007/01/08 16:23:01  brian
# - final updates for release
#
# Revision 0.9.2.4  2006/12/29 05:50:56  brian
# - changes for successful master build
#
# Revision 0.9.2.3  2006/12/23 13:06:45  brian
# - manual page and other package updates for release
#
# Revision 0.9.2.2  2006/10/17 12:08:17  brian
# - update release files
#
# Revision 0.9.2.1  2006/10/16 10:42:40  brian
# - added new package files
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
