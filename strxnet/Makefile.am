# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/16 19:19:27 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
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
# Last Modified $Date: 2004/05/16 19:19:27 $ by $Author: brian $
#
# =============================================================================

lib_LTLIBRARIES			= 

if PUBLIC_RELEASE

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

libexec_PROGRAMS		= 
if WITH_TLI
libexec_PROGRAMS		+= \
				  test-tirdwr \
				  test-timod
endif
if WITH_INET
libexec_PROGRAMS		+= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp
endif
if WITH_XNET
libexec_PROGRAMS		+= \
				  test-xnet \
				  test-xnet_thr
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= @XNET_INCLUDES@ @USER_CPPFLAGS@
@PACKAGE_TARNAME@_mknod_CFLAGS		= @USER_CFLAGS@

if WITH_TLI
test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lLiS
test_tirdwr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lLiS
test_timod_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@
endif

if WITH_INET
test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@
endif

if WITH_XNET
test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDFLAGS		= -lLiS -lxnet
test_xnet_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_xnet_CFLAGS		= @USER_CFLAGS@

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -lLiS -lpthread -lxnet
test_xnet_thr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@ -D_REENTRANT
test_xnet_thr_CFLAGS		= @USER_CFLAGS@
endif

ADDON_LIBS			= 
ADDON_OBJS			= 

if WITH_XNET
lib_LTLIBRARIES			+= libxnet.la

libxnet_la_SOURCES		= src/lib/xnet.c gettext.h
libxnet_la_LDFLAGS		= -lLiS -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @XNET_INCLUDES@ @USER_CPPFLAGS@ -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= @USER_CFLAGS@
libxnet_la_LIBADD		= 
endif

noinst_LIBRARIES		= libXNETmods.a libXNETdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libXNETmods_a_SOURCES = \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  ./config.h ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libXNETmods_a_SOURCES)

nodist_libXNETmods_a_SOURCES	= $(mods_source_FILES)

libXNETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETmods_a_CPPFLAGS		= -I./src/modules -I@srcdir@/src/modules -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETmods_a_CFLAGS		= @KERNEL_CFLAGS@

nodist_EXTRA_libXNETdrvs_a_SOURCES = \
				  src/drivers/inet.c \
				  src/drivers/bufq.h \
				  src/drivers/debug.h \
				  ./config.h ./strconf.h

drvs_source_FILES		= $(nodist_EXTRA_libXNETdrvs_a_SOURCES)

nodist_libXNETdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNETdrvs_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETdrvs_a_CPPFLAGS		= -I./src/drivers -I@srcdir@/src/drivers -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETdrvs_a_CFLAGS		= @KERNEL_CFLAGS@

KERNEL_MODULES			= $(libXNETmods_a_OBJECTS) $(libXNETdrvs_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

else

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

libexec_PROGRAMS		= 
if WITH_TLI
libexec_PROGRAMS		+= \
				  test-tirdwr \
				  test-timod
endif
if WITH_INET
libexec_PROGRAMS		+= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp
endif
if WITH_XNET
libexec_PROGRAMS		+= \
				  test-xnet \
				  test-xnet_thr
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= @XNET_INCLUDES@ @USER_CPPFLAGS@
@PACKAGE_TARNAME@_mknod_CFLAGS		= @USER_CFLAGS@

if WITH_TLI
test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lLiS
test_tirdwr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lLiS
test_timod_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@
endif

if WITH_INET
test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@
endif

if WITH_XNET
test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDFLAGS		= -lLiS -lxnet
test_xnet_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_xnet_CFLAGS		= @USER_CFLAGS@

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -lLiS -lpthread -lxnet
test_xnet_thr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@ -D_REENTRANT
test_xnet_thr_CFLAGS		= @USER_CFLAGS@
endif

ADDON_LIBS			= 
ADDON_OBJS			= 

if WITH_XNET
lib_LTLIBRARIES			+= libxnet.la

libxnet_la_SOURCES		= src/lib/xnet.c gettext.h
libxnet_la_LDFLAGS		= -lLiS -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @XNET_INCLUDES@ @USER_CPPFLAGS@ -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= @USER_CFLAGS@
libxnet_la_LIBADD		= 
endif

noinst_LIBRARIES		= libXNETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libXNETmods_a_SOURCES = \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  ./config.h ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libXNETmods_a_SOURCES)

nodist_libXNETmods_a_SOURCES	= $(mods_source_FILES)

libXNETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETmods_a_CPPFLAGS		= -I./src/modules -I@srcdir@/src/modules -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETmods_a_CFLAGS		= @KERNEL_CFLAGS@

nodist_EXTRA_libXNETdrvs_a_SOURCES = \
				  src/drivers/inet.c \
				  src/drivers/bufq.h \
				  src/drivers/debug.h \
				  ./config.h ./strconf.h

drvs_source_FILES		= $(nodist_EXTRA_libXNETdrvs_a_SOURCES)

nodist_libXNETdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNETdrvs_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETdrvs_a_CPPFLAGS		= -I./src/drivers -I@srcdir@/src/drivers -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETdrvs_a_CFLAGS		= @KERNEL_CFLAGS@

KERNEL_MODULES			= $(libXNETmods_a_OBJECTS) $(libXNETdrvs_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

endif

xnet_source_FILES		= $(mods_source_FILES) $(drvs_source_FILES)

EXTRA_DIST			= $(xnet_source_FILES)
dist_noinst_SCRIPTS		=
EXTRA_SCRIPTS			=

KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KMODCONF_EXT			= @PACKAGE_TARNAME@
KERNEL_SUBDIR			= @PACKAGE_TARNAME@

include am/kernel.am

include am/libraries.am

EXPORT_SRCFILES =

EXPORT_VERFILES =

PKGINCL = include/sys/strxnet

GENKSYMS_CPPFLAGS = \
	-I./src/modules -I$(srcdir)/src/modules \
	-I./src/drivers -I$(srcdir)/src/drivers \
	$(XNET_INCLUDES) $(KERNEL_CPPFLAGS)

include am/genksyms.am

STRMAKENODES			= ./@PACKAGE_TARNAME@_mknod

include am/strconf.am

if WITH_INET

INET_MKDEV			= $(auxdir)/inet_mkdev
INET_RMDEV			= $(auxdir)/inet_rmdev

install-devices: $(STRMAKENODES) $(INET_MKDEV)
	$(STRMAKENODES)
	DESTDIR=$(DESTDIR) $(INET_MKDEV)

uninstall-devices: $(STRMAKENODES) $(INET_RMDEV)
	$(STRMAKENODES) --remove
	DESTDIR=$(DESTDIR) $(INET_RMDEV)

dist_noinst_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += scripts/inet_mkdev scripts/inet_rmdev

else

install-devices: $(STRMAKENODES)
	$(STRMAKENODES)

uninstall-devices: $(STRMAKENODES)
	$(STRMAKENODES) --remove

endif

SUBDIRS = src intl m4 po doc

#
#   Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion $(PACKAGE_KVERSION)"
AM_SRPMFLAGS =

include am/rpm.am

#
#   General rules
#
include am/openss7.am

# vim: ft=automake
