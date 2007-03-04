# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/03/04 00:24:07 $
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
# Last Modified $Date: 2007/03/04 00:24:07 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.9  2007/03/04 00:24:07  brian
# - add version file creation
#
# Revision 0.9.2.8  2006/10/03 13:53:03  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.7  2006/10/01 21:30:23  brian
# - distribute release and library version files
#
# Revision 0.9.2.6  2006/09/29 11:50:46  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.5  2006/09/27 05:15:29  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.4  2006/09/18 17:43:14  brian
# - added doxy rules to makefile and release notes
#
# Revision 0.9.2.3  2006/09/18 01:15:25  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.2  2006/08/26 09:19:23  brian
# - better release file generation
#
# Revision 0.9.2.1  2006/08/23 10:03:51  brian
# - started STREAMS Sockets package
#
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

EXTRA_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod

pkglibexec_PROGRAMS		= \
				  test-sockpath \
				  test-socket \
				  test-socklib \
				  test-sockmod \
				  test-socksys

sbin_PROGRAMS			= \
				  initsock \
				  soconfig \
				  @PACKAGE_TARNAME@_mknod

bin_PROGRAMS			= 

dist_sbin_SCRIPTS		= 

#SOCK_EXTRA_TESTS		=
#SOCK_EXTRA_TOOLS		=


@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_sockpath_SOURCES		= src/test/test-sockpath.c
test_sockpath_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_sockpath_LDADD		= libsockpath.la $(STREAMS_LDADD)
test_sockpath_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sockpath_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sockpath_CFLAGS		= $(USER_CFLAGS)

test_socket_SOURCES		= src/test/test-socket.c
test_socket_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_socket_LDADD		= libsocket.la $(STREAMS_LDADD)
test_socket_LDFLAGS		= $(STREAMS_LDFLAGS)
test_socket_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_socket_CFLAGS		= $(USER_CFLAGS)

test_socklib_SOURCES		= src/test/test-socklib.c
test_socklib_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_socklib_LDADD		= libsocklib.la $(STREAMS_LDADD)
test_socklib_LDFLAGS		= $(STREAMS_LDFLAGS)
test_socklib_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_socklib_CFLAGS		= $(USER_CFLAGS)

test_sockmod_SOURCES		= src/test/test-sockmod.c
test_sockmod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_sockmod_LDADD		= libsocklib.la $(STREAMS_LDADD)
test_sockmod_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sockmod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sockmod_CFLAGS		= $(USER_CFLAGS)

test_socksys_SOURCES		= src/test/test-socksys.c
test_socksys_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_socksys_LDADD		= libsocket.la $(STREAMS_LDADD)
test_socksys_LDFLAGS		= $(STREAMS_LDFLAGS)
test_socksys_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_socksys_CFLAGS		= $(USER_CFLAGS)

initsock_SOURCES		= src/util/initsock.c
initsock_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
initsock_LDADD			= $(STREAMS_LDADD)
initsock_LDFLAGS		= $(STREAMS_LDFLAGS)
initsock_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
initsock_CFLAGS			= $(USER_CFLAGS)

soconfig_SOURCES		= src/util/soconfig.c
soconfig_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
soconfig_LDADD			= $(STREAMS_LDADD)
soconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
soconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
soconfig_CFLAGS			= $(USER_CFLAGS)

#xxxxxx_SOURCES			= src/xxxxxx.c
#xxxxxx_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
#xxxxxx_LDADD			= $(STREAMS_LDADD)
#xxxxxx_LDFLAGS			= $(STREAMS_LDFLAGS)
#xxxxxx_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#xxxxxx_CFLAGS			= $(USER_CFLAGS)

lib_LTLIBRARIES			+= libsockpath.la

libsockpath_la_SOURCES		= src/lib/pthread.c \
				  src/lib/sockpath.c \
				  src/lib/sockpath.h \
				  src/include/sockdb.h \
				  src/include/sys/sockpath.h
libsockpath_la_DEPENDENCIES	= src/lib/sockpath.VERSION
libsockpath_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION
libsockpath_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(top_srcdir)/src/include -I$(top_builddir)/src/include
libsockpath_la_CFLAGS		= $(USER_CFLAGS)
libsockpath_la_LIBADD		=

lib_LTLIBRARIES			+= libsocket.la

libsocket_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socket.c \
				  gettext.h
libsocket_la_DEPENDENCIES	= src/lib/socket.VERSION
libsocket_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  $(STREAMS_LDFLAGS) -static -lc -shared
libsocket_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(top_srcdir)/src/include -I$(top_builddir)/src/include
libsocket_la_CFLAGS		= $(USER_CFLAGS)
libsocket_la_LIBADD		= $(STREAMS_LDADD)

lib_LTLIBRARIES			+= libsocklib.la

libsocklib_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socklib.c \
				  gettext.h
libsocklib_la_DEPENDENCIES	= src/lib/socklib.VERSION
libsocklib_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  $(NSL_LDFLAGS) -static -lc -shared
libsocklib_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(top_srcdir)/src/include -I$(top_builddir)/src/include
libsocklib_la_CFLAGS		= $(USER_CFLAGS)
libsocklib_la_LIBADD		= $(NSL_LDADD) libsockpath.la

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libSOCKlink.a libSOCKmods.a libSOCKdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

EXTRA_libSOCKlink_a_SOURCES	= src/modules/sockmod.c \
				  src/drivers/socksys.c
libSOCKlink_a_SOURCES		= 
libSOCKlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSOCKlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libSOCKlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSOCKlink_a_OBJECTS):	$(libSOCKlink_a_SOURCES) $(libSOCKlink_a_DEPENDENCIES)

EXTRA_libSOCKmods_a_SOURCES	= src/modules/sockmod.c
libSOCKmods_a_SOURCES		= src/modules/sockmod.c
libSOCKmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSOCKmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libSOCKmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSOCKmods_a_OBJECTS):	$(libSOCKmods_a_DEPENDENCIES)

EXTRA_libSOCKdrvs_a_SOURCES	= src/drivers/socksys.c
libSOCKdrvs_a_SOURCES		= src/drivers/socksys.c
libSOCKdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSOCKdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libSOCKdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSOCKdrvs_a_OBJECTS):	$(libSOCKdrvs_a_DEPENDENCIES)

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
KERNEL_OBJECTS			= $(libSOCKlink_a_OBJECTS) $(libSOCKmods_a_OBJECTS) $(libSOCKdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libSOCKmods_a_OBJECTS) $(libSOCKdrvs_a_OBJECTS) $(ADDON_MODS)
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
LIBRARY_VERSION_FILES		= \
				  src/lib/socket.VERSION \
				  src/lib/sockpath.VERSION \
				  src/lib/socklib.VERSION

include am/libraries.am

EXPORT_SRCFILES			=

EXPORT_VERFILES			=

GENKSYMS_CPPFLAGS		= \
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

dist_initrd_SCRIPTS		= src/util/init/@PACKAGE_LCNAME@.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/@PACKAGE_LCNAME@

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= src/util/sysconfig/@PACKAGE_LCNAME@.conf \
				  src/util/sysconfig/sock2path

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

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
