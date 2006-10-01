# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/10/01 21:30:22 $
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
# Last Modified $Date: 2006/10/01 21:30:22 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.4  2006/10/01 21:30:22  brian
# - distribute release and library version files
#
# Revision 0.9.2.3  2006/09/29 11:50:38  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.2  2006/09/27 05:15:27  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.1  2006/09/25 12:30:44  brian
# - added files for new strnsl package
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
				  test-xnsl \
				  test-xnsl_thr

pkglibexec_PROGRAMS		= \
				  test-xnsl \
				  test-xnsl_thr

sbin_PROGRAMS			= 

test_xnsl_SOURCES		= src/test/test-xnsl.c
test_xnsl_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_xnsl_LDADD			= libxnsl.la
test_xnsl_LDFLAGS		= 
test_xnsl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnsl_CFLAGS		= $(USER_CFLAGS)

test_xnsl_thr_SOURCES		= src/test/test-xnsl.c
test_xnsl_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_xnsl_thr_LDADD		= libxnsl.la
test_xnsl_thr_LDFLAGS		= -lpthread
test_xnsl_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS)

lib_LTLIBRARIES			+= libxnsl.la

libxnsl_la_SOURCES		= src/lib/pthread.c \
				  src/lib/netselect.c \
				  src/lib/netselect.h \
				  src/lib/netdir.c \
				  src/lib/netdir_inet.c \
				  src/include/netconfig.h \
				  src/include/netdir.h \
				  src/include/sys/netconfig.h \
				  gettext.h
libxnsl_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnsl.VERSION
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  $(XTI_LDFLAGS) -ldl
libxnsl_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS)
libxnsl_la_CFLAGS		= $(USER_CFLAGS)
libxnsl_la_LIBADD		= $(XTI_LDADD)

lib_LTLIBRARIES			+= inetaddr.la

inetaddr_la_SOURCES		= src/lib/netdir_inet.c \
				  src/include/netdir.h \
				  src/include/netconfig.h \
				  src/include/sys/netconfig.h
inetaddr_la_DEPENDENCIES	= libxnsl.la
inetaddr_la_LDFLAGS		= -module -avoid-version \
				  -shared $(XTI_LDFLAGS)
inetaddr_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS)
inetaddr_la_LIBADD		= $(XTI_LDADD)

## RPM_BUILD_USER
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-xnsl \
				  test-xnsl_thr

pkglibexec_PROGRAMS		= \
				  test-xnsl \
				  test-xnsl_thr

sbin_PROGRAMS			= 

test_xnsl_SOURCES		= src/test/test-xnsl.c
test_xnsl_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_xnsl_LDADD			= libxnsl.la
test_xnsl_LDFLAGS		= $(XTI_LDFLAGS)
test_xnsl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnsl_CFLAGS		= $(USER_CFLAGS)

test_xnsl_thr_SOURCES		= src/test/test-xnsl.c
test_xnsl_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_xnsl_thr_LDADD		= libxnsl.la
test_xnsl_thr_LDFLAGS		= $(XTI_LDFLAGS) -lpthread
test_xnsl_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS)

lib_LTLIBRARIES			+= libxnsl.la

libxnsl_la_SOURCES		= src/lib/pthread.c \
				  src/lib/netselect.c \
				  src/lib/netselect.h \
				  src/lib/netdir.c \
				  src/lib/netdir_inet.c \
				  src/include/netconfig.h \
				  src/include/netdir.h \
				  src/include/sys/netconfig.h \
				  gettext.h
libxnsl_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnsl.VERSION
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  $(XTI_LDFLAGS) -ldl
libxnsl_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS)
libxnsl_la_CFLAGS		= $(USER_CFLAGS)
libxnsl_la_LIBADD		= $(XTI_LDADD)

lib_LTLIBRARIES			+= inetaddr.la

inetaddr_la_SOURCES		= src/lib/netdir_inet.c \
				  src/include/netdir.h \
				  src/include/netconfig.h \
				  src/include/sys/netconfig.h
inetaddr_la_DEPENDENCIES	= 
inetaddr_la_LDFLAGS		= -module -avoid-version \
				  -shared $(XTI_LDFLAGS)
inetaddr_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS)
inetaddr_la_LIBADD		= $(XTI_LDADD)

## RPM_BUILD_USER
endif

## ! PUBLIC_RELEASE
endif

## PKG_BUILD_ARCH
endif

nsl_source_FILES		= $(mods_source_FILES)

EXTRA_DIST			+= $(nsl_source_FILES)

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

##
## Rules for building libraries
##
LIBRARY_VERSION_FILES		= \
				  src/lib/xnsl.VERSION

include am/libraries.am
##
##AUTOCONFIG_H			= src/$(PKGINCL)/config.h
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

if PKG_BUILD_ARCH
SUBDIRS += intl m4 po
CLEANFILES += po/stamp-po
endif

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

dist_initrd_SCRIPTS		=

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		=

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= src/util/sysconfig/netconfig

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

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
