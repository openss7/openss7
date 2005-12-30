# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.6.30 $) $Date: 2005/12/30 12:21:11 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2005/12/30 12:21:11 $ by $Author: brian $
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

if LIS_USER_TARGET

if PKG_BUILD_ARCH

if RPM_BUILD_USER

lib_LTLIBRARIES			= libLiSuser.la

libLiSuser_la_SOURCES		=
libLiSuser_la_LIBADD		= drivers/str/libstr_user.la head/libhead_user.la
libLiSuser_la_DEPENDENCIES	= drivers/str/libstr_user.la head/libhead_user.la
libLiSuser_la_LDFLAGS		=
libLiSuser_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiSuser_la_CFLAGS		= $(USER_CFLAGS)

pkglibexec_PROGRAMS		= timetst \
				  strtst \
				  polltst \
				  perftest \
				  test-streams \
				  test-fifo \
				  test-pipe \
				  test-sad
bin_PROGRAMS			=
sbin_PROGRAMS			= streams

LIS_EXTRA_TESTS			= polltst
LIS_EXTRA_PROGS			=
LIS_EXTRA_TOOLS			= 

LDADD				= libLiSuser.la

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

else
## ! LIS_USER_TARGET

if PKG_BUILD_ARCH

if RPM_BUILD_USER

pkglibexec_PROGRAMS		= timetst \
				  strtst \
				  perftest \
				  thrtst \
				  lis_test \
				  lis_test2 \
				  test-streams \
				  test-fifo \
				  test-pipe \
				  test-sad

bin_PROGRAMS			= oc \
				  pafd \
				  psfd \
				  rwf

sbin_PROGRAMS			= streams \
				  fattach \
				  fdetach \
				  mkfifo \
				  strmakenodes

LIS_EXTRA_TESTS			= thrtst lis_test lis_test2
LIS_EXTRA_PROGS			= oc pafd psfd rwf
LIS_EXTRA_TOOLS			= fattach fdetach mkfifo strmakenodes

LDADD				= libLiS.la

lib_LTLIBRARIES			= libLiS.la libpLiS.la

## RPM_BUILD_USER
endif

if PKG_BUILD_ARCH
if RPM_BUILD_KERNEL

noinst_LIBRARIES		= libLiShead.a libLiSdrvr.a libLiSload.a

libLiShead_a_SOURCES		= head/head.c \
				  head/dki.c \
				  head/msg.c \
				  head/mod.c \
				  head/buffcall.c \
				  head/events.c \
				  head/linux-mdep.c \
				  head/msgutl.c \
				  head/queue.c \
				  head/safe.c \
				  head/stats.c \
				  head/stream.c \
				  head/strmdbg.c \
				  head/wait.c \
				  head/cmn_err.c \
				  head/version.c \
				  head/osif.c \
				  head/poll.c \
				  head/linux/exports.c \
				  head/linux/lislocks.c \
				  head/linux/lismem.c \
				  head/linux/lispci.c
libLiShead_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLiShead_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) -DKBUILD_BASENAME=streams $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME=`echo $* | sed -e 's/^.*-//'`
libLiShead_a_CFLAGS		= $(KERNEL_CFLAGS)
$(libLiShead_a_OBJECTS): $(libLiShead_a_DEPENDENCIES)

libLiSdrvr_a_SOURCES		= drivers/str/clone.c \
				  drivers/str/fifo.c \
				  drivers/str/loop.c \
				  drivers/str/minimux.c \
				  drivers/str/printk.c \
				  drivers/str/sad.c \
				  drivers/str/linux/connld.c \
				  drivers/str/relay.c
libLiSdrvr_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLiSdrvr_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) -DKBUILD_BASENAME=streams $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) \
				  -DLIS_OBJNAME=`echo $* | sed -e 's/^.*-//'`
libLiSdrvr_a_CFLAGS		= $(KERNEL_CFLAGS)
$(libLiSdrvr_a_OBJECTS): $(libLiSdrvr_a_DEPENDENCIES)

drivers/str/linux/relay3.c: drivers/str/relay.c
	mkdir -p `echo '$@' | sed -e 's|/[^/]*$$||'`
	cp -f $< $@

CLEANFILES			+= drivers/str/linux/relay3.c

libLiSload_a_SOURCES		= drivers/str/pipemod.c \
				  drivers/str/linux/liskmod.c \
				  drivers/str/linux/mtdrv.c \
				  drivers/str/linux/relay3.c \
				  drivers/str/bufmod.c \
				  drivers/str/nullmod.c \
				  drivers/str/testmod.c \
				  drivers/str/nuls.c \
				  drivers/str/echo.c \
				  drivers/str/mux.c

libLiSload_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLiSload_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) -DKBUILD_BASENAME=`echo $@ | sed -e 's|lib.*_a-|${kpre}|;s|\.o||;s|-|_|g'` $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  -DLIS_OBJNAME=`echo $* | sed -e 's/^.*-//'`
libLiSload_a_CFLAGS		= $(KERNEL_CFLAGS)
$(libLiSload_a_OBJECTS): $(libLiSload_a_DEPENDENCIES)

streams.o: stamp-verobjs
	@list=`$(ECHO) " $(libLiShead_a_OBJECTS) $(libLiSdrvr_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

## RPM_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

if RPM_BUILD_USER

## non-preload library has pipe()
libLiS_la_SOURCES		= libc/fattach.c \
				  libc/fdetach.c \
				  libc/getmsg.c \
				  libc/getpmsg.c \
				  libc/isastream.c \
				  libc/pipe.c \
				  libc/putmsg.c \
				  libc/putpmsg.c
libLiS_la_LDFLAGS		= -no-undefined -version-info 0:2:0
libLiS_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS)

## preload library does not have pipe()
libpLiS_la_SOURCES		= libc/fattach.c \
				  libc/fdetach.c \
				  libc/getmsg.c \
				  libc/getpmsg.c \
				  libc/isastream.c \
				  libc/putmsg.c \
				  libc/putpmsg.c
libpLiS_la_LDFLAGS		= -no-undefined -version-info 0:2:0
libpLiS_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif
## PKG_BUILD_ARCH
endif
## ! LIS_USER_TARGET
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
KERNEL_OBJECTS			= $(libLiShead_a_OBJECTS) $(libLiSdrvr_a_OBJECTS) $(libLiSload_a_OBJECTS)
KERNEL_MODULES			= streams.o $(libLiSload_a_OBJECTS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			=

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
				util/modutils/lis
endif
endif
endif

##
## Rules for building libraries
##
include am/libraries.am

if RPM_BUILD_USER

if PKG_BUILD_ARCH

EXTRA_PROGRAMS			+= \
				  polltst \
				  thrtst \
				  lis_test \
				  lis_test2 \
				  fattach \
				  fdetach \
				  mkfifo \
				  oc \
				  pafd \
				  psfd \
				  rwf \
				  strmakenodes

dist_sbin_SCRIPTS		= strms_down strms_status strms_up

streams_SOURCES			= util/streams.c
streams_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
streams_LDFLAGS			= -static libLiS.la
streams_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMEMPRINT
streams_CFLAGS			= $(USER_CFLAGS)

timetst_SOURCES			= util/timetst.c
timetst_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
timetst_LDFLAGS			= -static libLiS.la
timetst_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
timetst_CFLAGS			= $(USER_CFLAGS)

strtst_SOURCES			= util/strtst.c
strtst_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strtst_LDFLAGS			= -lpthread -static libLiS.la
strtst_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strtst_CFLAGS			= $(USER_CFLAGS)

test_streams_SOURCES		= util/test-streams.c
test_streams_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_streams_LDFLAGS		= -lpthread -static libLiS.la
test_streams_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(USER_CFLAGS)

test_fifo_SOURCES		= util/test-fifo.c
test_fifo_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_fifo_LDFLAGS		= -lpthread -static libLiS.la
test_fifo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(USER_CFLAGS)

test_pipe_SOURCES		= util/test-pipe.c
test_pipe_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_pipe_LDFLAGS		= -lpthread -static libLiS.la
test_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(USER_CFLAGS)

test_sad_SOURCES		= util/test-sad.c
test_sad_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_sad_LDFLAGS		= -lpthread -static libLiS.la
test_sad_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(USER_CFLAGS)

perftest_SOURCES		= util/perftest.c
perftest_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
perftest_LDFLAGS		= -static libLiS.la
perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
perftest_CFLAGS			= $(USER_CFLAGS)

polltst_SOURCES			= util/polltst.c
polltst_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
polltst_LDFLAGS			= -static libLiS.la
polltst_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
polltst_CFLAGS			= $(USER_CFLAGS)

fattach_SOURCES			= util/linux/fattach.c
fattach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
fattach_LDFLAGS			= -static libLiS.la
fattach_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
fattach_CFLAGS			= $(USER_CFLAGS)

fdetach_SOURCES			= util/linux/fdetach.c
fdetach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
fdetach_LDFLAGS			= -static libLiS.la
fdetach_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
fdetach_CFLAGS			= $(USER_CFLAGS)

mkfifo_SOURCES			= util/linux/mkfifo.c
mkfifo_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
mkfifo_LDFLAGS			= -static libLiS.la
mkfifo_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
mkfifo_CFLAGS			= $(USER_CFLAGS)

oc_SOURCES			= util/linux/oc.c
oc_DEPENDENCIES			= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
oc_LDFLAGS			= -static libLiS.la
oc_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
oc_CFLAGS			= $(USER_CFLAGS)

pafd_SOURCES			= util/linux/pafd.c
pafd_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
pafd_LDFLAGS			= -static libLiS.la
pafd_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
pafd_CFLAGS			= $(USER_CFLAGS)

psfd_SOURCES			= util/linux/psfd.c
psfd_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
psfd_LDFLAGS			= -static libLiS.la
psfd_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
psfd_CFLAGS			= $(USER_CFLAGS)

rwf_SOURCES			= util/linux/rwf.c
rwf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
rwf_LDFLAGS			= -static libLiS.la
rwf_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
rwf_CFLAGS			= $(USER_CFLAGS)

lis_test_SOURCES		= util/linux/test.c
lis_test_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
lis_test_LDFLAGS		= -static libLiS.la
lis_test_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
lis_test_CFLAGS			= $(USER_CFLAGS)

lis_test2_SOURCES		= util/linux/test2.c
lis_test2_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
lis_test2_LDFLAGS		= -static libLiS.la
lis_test2_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
lis_test2_CFLAGS		= $(USER_CFLAGS)

thrtst_SOURCES			= util/linux/thrtst.c
thrtst_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
thrtst_LDFLAGS			= -lpthread -static libLiS.la
thrtst_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
thrtst_CFLAGS			= $(USER_CFLAGS)

strmakenodes_SOURCES		= util/linux/strmakenodes.c
strmakenodes_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strmakenodes_LDADD		=
strmakenodes_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strmakenodes_CFLAGS		= $(USER_CFLAGS)

## PKG_BUILD_ARCH
endif
## RPM_BUILD_USER
endif

AUTOCONFIG_H			= $(PKGINCL)/config.h

##EXTRA_DIST			+= $(AUTOCONFIG_H).in

EXPORT_SRCFILES			= \
				head/linux/exports.c

EXPORT_VERFILES			= \
				$(PKGINCL)/versions/exports.ver

GENKSYMS_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) \
				  -DLIS_OBJNAME=`echo $* | sed -e 's/^.*-//'`

include am/genksyms.am

include am/strconf.am

EXTRA_DIST			+= Config.user

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS =

if PKG_BUILD_ARCH
SUBDIRS += include
endif

SUBDIRS += tests

if PKG_BUILD_ARCH
SUBDIRS += . testsuite
endif

if PKG_BUILD_INDEP
SUBDIRS += man manual
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				util/init/streams.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				util/init/streams

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				util/sysconfig/streams.conf

dist_config_DATA		= \
				util/sysconfig/streams

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
