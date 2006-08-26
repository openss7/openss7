# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.133 $) $Date: 2006/07/24 09:01:09 $
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
# Last Modified $Date: 2006/07/24 09:01:09 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.133  2006/07/24 09:01:09  brian
# - results of udp2 optimizations
#
# Revision 0.9.2.132  2006/07/01 11:41:52  brian
# - changed to new KBUILD_ flags
#
# Revision 0.9.2.131  2006/03/21 13:24:40  brian
# - included pr.am
#
# Revision 0.9.2.130  2006/03/20 11:54:02  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.129  2006/03/06 11:38:40  brian
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

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

##
## These are for ptys and should be part of the strutil or strio package.
##
bin_PROGRAMS			=
if CONFIG_STREAMS_UTILS
bin_PROGRAMS			+= \
				  strchg \
				  strconf \
				  strreset
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod
if CONFIG_STREAMS_SAD
sbin_PROGRAMS			+= autopush
endif
if CONFIG_STREAMS_SAD_MODULE
sbin_PROGRAMS			+= autopush
endif
if CONFIG_STREAMS_LOG
sbin_PROGRAMS			+= strace strerr
endif
if CONFIG_STREAMS_LOG_MODULE
sbin_PROGRAMS			+= strace strerr
endif
if CONFIG_STREAMS_SC
sbin_PROGRAMS			+= scls strinfo
endif
if CONFIG_STREAMS_SC_MODULE
sbin_PROGRAMS			+= scls strinfo
endif
if CONFIG_STREAMS_PIPE
sbin_PROGRAMS			+= fattach fdetach
endif
if CONFIG_STREAMS_PIPE_MODULE
sbin_PROGRAMS			+= fattach fdetach
endif
if CONFIG_STREAMS_UTILS
sbin_PROGRAMS			+= \
				  insf \
				  strvf
endif

dist_sbin_SCRIPTS		=
if CONFIG_STREAMS_LOG
dist_sbin_SCRIPTS		+= src/util/strclean
endif
if CONFIG_STREAMS_LOG_MODULE
dist_sbin_SCRIPTS		+= src/util/strclean
endif
if CONFIG_STREAMS_UTILS
dist_sbin_SCRIPTS		+= \
				  src/util/strload \
				  src/util/strsetup
endif

pkglibexec_PROGRAMS		=
if CONFIG_STREAMS_STH
pkglibexec_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_STH_MODULE
pkglibexec_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_CLONE
pkglibexec_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CLONE_MODULE
pkglibexec_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CONNLD
pkglibexec_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_CONNLD_MODULE
pkglibexec_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_ECHO
pkglibexec_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_ECHO_MODULE
pkglibexec_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_FIFO
pkglibexec_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_FIFO_MODULE
pkglibexec_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_LOG
pkglibexec_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOG_MODULE
pkglibexec_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOOP
pkglibexec_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_LOOP_MODULE
pkglibexec_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_NSDEV
pkglibexec_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_NSDEV_MODULE
pkglibexec_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_MUX
pkglibexec_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_MUX_MODULE
pkglibexec_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_NULS
pkglibexec_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_NULS_MODULE
pkglibexec_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_PIPE
pkglibexec_PROGRAMS		+= test-pipe perftest perftestn
endif
if CONFIG_STREAMS_PIPE_MODULE
pkglibexec_PROGRAMS		+= test-pipe perftest perftestn
endif
if CONFIG_STREAMS_PIPEMOD
pkglibexec_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
pkglibexec_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_SAD
pkglibexec_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SAD_MODULE
pkglibexec_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SC
pkglibexec_PROGRAMS		+= test-sc
endif
if CONFIG_STREAMS_SC_MODULE
pkglibexec_PROGRAMS		+= test-sc
endif


#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= @PACKAGE_TARNAME@_mknod

#LDADD				= libstreams.la

lib_LTLIBRARIES			= libstreams.la

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

noinst_LIBRARIES		= \
				libLfS-specfs.a \
				libLfS-streams.a \
				libLfS-drivers.a \
				libLfS-modules.a

libLfS_specfs_a_SOURCES		= \
				src/kernel/strspecfs.h \
				src/kernel/strlookup.h \
				src/kernel/strattach.h \
				src/kernel/strpipe.h \
				src/kernel/strargs.h

if CONFIG_STREAMS_SEPARATE_COMPILE
libLfS_specfs_a_SOURCES		+= \
				src/kernel/strspecfs.c \
				src/kernel/strlookup.c \
				src/kernel/strattach.c \
				src/kernel/strpipe.c
else
libLfS_specfs_a_SOURCES		+= \
				src/kernel/specfs.c
endif

libLfS_specfs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H) \
				  src/kernel/strspecfs.c \
				  src/kernel/strlookup.c \
				  src/kernel/strattach.c \
				  src/kernel/strpipe.c
libLfS_specfs_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libLfS_specfs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_specfs_a_OBJECTS): $(libLfS_specfs_a_DEPENDENCIES)


libLfS_streams_a_SOURCES	= \
				src/kernel/strreg.h \
				src/kernel/strprocfs.h \
				src/kernel/strsched.h \
				src/kernel/strsysctl.h \
				src/kernel/strutil.h \
				src/kernel/strsad.h \
				src/kernel/strargs.h

if CONFIG_STREAMS_SEPARATE_COMPILE
libLfS_streams_a_SOURCES	+= \
				src/kernel/strmain.c \
				src/kernel/strreg.c \
				src/kernel/strprocfs.c \
				src/kernel/strsched.c \
				src/kernel/strsysctl.c \
				src/kernel/strsyms.c \
				src/kernel/strutil.c \
				src/kernel/strsad.c
else
libLfS_streams_a_SOURCES	+= \
				src/kernel/streams.c
endif

##if CONFIG_STREAMS_STH
##libLfS_streams_a_SOURCES	+= src/modules/sth.c src/modules/sth.h 
##endif
if CONFIG_STREAMS_BUFMOD
libLfS_streams_a_SOURCES	+= src/modules/bufmod.c
endif
if CONFIG_STREAMS_NULLMOD
libLfS_streams_a_SOURCES	+= src/modules/nullmod.c
endif
if CONFIG_STREAMS_PIPEMOD
libLfS_streams_a_SOURCES	+= src/modules/pipemod.c
endif
if CONFIG_STREAMS_CONNLD
libLfS_streams_a_SOURCES	+= src/modules/connld.c
endif
if CONFIG_STREAMS_SC
libLfS_streams_a_SOURCES	+= src/modules/sc.c include/sys/sc.h
endif

##if CONFIG_STREAMS_CLONE
##libLfS_streams_a_SOURCES	+= src/drivers/clone.c src/drivers/clone.h
##endif
if CONFIG_STREAMS_FIFO
libLfS_streams_a_SOURCES	+= src/drivers/fifo.c src/drivers/fifo.h
endif
if CONFIG_STREAMS_SAD
libLfS_streams_a_SOURCES	+= src/drivers/sad.c include/sys/sad.h
endif
if CONFIG_STREAMS_NSDEV
libLfS_streams_a_SOURCES	+= src/drivers/nsdev.c
endif
if CONFIG_STREAMS_ECHO
libLfS_streams_a_SOURCES	+= src/drivers/echo.c
endif
if CONFIG_STREAMS_MUX
libLfS_streams_a_SOURCES	+= src/drivers/mux.c
endif
if CONFIG_STREAMS_NULS
libLfS_streams_a_SOURCES	+= src/drivers/nuls.c
endif
if CONFIG_STREAMS_PIPE
libLfS_streams_a_SOURCES	+= src/drivers/pipe.c src/drivers/pipe.h
endif
if CONFIG_STREAMS_LOG
libLfS_streams_a_SOURCES	+= include/log.h include/sys/log.h include/strlog.h include/sys/strlog.h \
				   src/drivers/log.h src/drivers/log.c
endif
if CONFIG_STREAMS_LOOP
libLfS_streams_a_SOURCES	+= src/drivers/loop.c include/sys/loop.h
endif
if CONFIG_STREAMS_SFX
libLfS_streams_a_SOURCES	+= src/drivers/sfx.c
endif
if CONFIG_STREAMS_SPX
libLfS_streams_a_SOURCES	+= src/drivers/spx.c
endif

libLfS_streams_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H) \
				  src/kernel/strmain.c \
				  src/kernel/strreg.c \
				  src/kernel/strprocfs.c \
				  src/kernel/strsched.c \
				  src/kernel/strsysctl.c \
				  src/kernel/strsyms.c \
				  src/kernel/strutil.c \
				  src/kernel/strsad.c \
				  src/modules/sth.c \
				  src/drivers/clone.c
libLfS_streams_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libLfS_streams_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_streams_a_OBJECTS): $(libLfS_streams_a_DEPENDENCIES)

libLfS_drivers_a_SOURCES	=

if CONFIG_STREAMS_CLONE_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/clone.c src/drivers/clone.h
endif
if CONFIG_STREAMS_FIFO_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/fifo.c src/drivers/fifo.h
endif
if CONFIG_STREAMS_SAD_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/sad.c include/sys/sad.h
endif
if CONFIG_STREAMS_NSDEV_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/nsdev.c
endif
if CONFIG_STREAMS_ECHO_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/echo.c
endif
if CONFIG_STREAMS_MUX_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/mux.c
endif
if CONFIG_STREAMS_NULS_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/nuls.c
endif
if CONFIG_STREAMS_PIPE_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/pipe.c src/drivers/pipe.h
endif
if CONFIG_STREAMS_LOG_MODULE
libLfS_drivers_a_SOURCES	+= include/log.h include/sys/log.h include/strlog.h include/sys/strlog.h \
				   src/drivers/log.h src/drivers/log.c
endif
if CONFIG_STREAMS_LOOP_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/loop.c include/sys/loop.h
endif
if CONFIG_STREAMS_SFX_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/sfx.c
endif
if CONFIG_STREAMS_SPX_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/spx.c
endif

libLfS_drivers_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLfS_drivers_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLfS_drivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_drivers_a_OBJECTS): $(libLfS_drivers_a_DEPENDENCIES)

libLfS_modules_a_SOURCES	=

if CONFIG_STREAMS_STH_MODULE
libLfS_modules_a_SOURCES	+= src/modules/sth.c src/modules/sth.h 
endif
if CONFIG_STREAMS_BUFMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/bufmod.c
endif
if CONFIG_STREAMS_NULLMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/nullmod.c
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/pipemod.c
endif
if CONFIG_STREAMS_CONNLD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/connld.c
endif
if CONFIG_STREAMS_SC_MODULE
libLfS_modules_a_SOURCES	+= src/modules/sc.c include/sys/sc.h
endif
if CONFIG_STREAMS_TESTMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/testmod.c include/sys/testmod.h
endif

libLfS_modules_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLfS_modules_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLfS_modules_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_modules_a_OBJECTS): $(libLfS_modules_a_DEPENDENCIES)

specfs.o: stamp-verobjs
	@list=`$(ECHO) " $(libLfS_specfs_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

streams.o: stamp-verobjs
	@list=`$(ECHO) " $(libLfS_streams_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

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
				scripts/texi/lesser.texi \
				scripts/texi/testsuite.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+= configure.nexusware

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libLfS_specfs_a_OBJECTS) $(libLfS_streams_a_OBJECTS) $(libLfS_drivers_a_OBJECTS) $(libLfS_modules_a_OBJECTS)
KERNEL_MODULES			= specfs.o streams.o $(libLfS_drivers_a_OBJECTS) $(libLfS_modules_a_OBJECTS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			=

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
				src/util/modutils/streams
endif
endif
endif

##
## Rules for building libraries
##
include am/libraries.am

if RPM_BUILD_USER
## RPM_BUILD_USER

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

libstreams_la_SOURCES		= \
				lib/libc/testcancel.c \
				lib/libc/putpmsg.c \
				lib/libc/getpmsg.c \
				lib/libc/isastream.c \
				lib/libc/pipe.c \
				lib/libc/fattach.c \
				lib/libc/fdetach.c
libstreams_la_LDFLAGS		= -no-undefined
libstreams_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS)

if USING_IOCTL_GETPMSG_PUTPMSG
libstreams_la_LDFLAGS		+= -version-info 0:1:0
else
libstreams_la_LDFLAGS		+= -version-info 0:0:0
endif

EXTRA_PROGRAMS			+= @PACKAGE_TARNAME@_mknod

#bin_PROGRAMS			= timetst strtst $(LFS_EXTRA_TESTS)
#sbin_PROGRAMS			= streams $(LFS_EXTRA_TOOLS)

strchg_SOURCES			= src/util/strchg.c
strchg_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strchg_LDFLAGS			= -static libstreams.la
strchg_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strchg_CFLAGS			= $(USER_CFLAGS)

strconf_SOURCES			= src/util/strconf.c
strconf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strconf_LDFLAGS			= -static libstreams.la
strconf_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strconf_CFLAGS			= $(USER_CFLAGS)

strreset_SOURCES		= src/util/strreset.c
strreset_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strreset_LDFLAGS		= -static libstreams.la
strreset_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strreset_CFLAGS			= $(USER_CFLAGS)

autopush_SOURCES		= src/util/autopush.c
autopush_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
autopush_LDFLAGS		= -static libstreams.la
autopush_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
autopush_CFLAGS			= $(USER_CFLAGS)

fattach_SOURCES			= src/util/fattach.c
fattach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
fattach_LDFLAGS			= -static libstreams.la
fattach_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
fattach_CFLAGS			= $(USER_CFLAGS)

fdetach_SOURCES			= src/util/fdetach.c 
fdetach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
fdetach_LDFLAGS			= -static libstreams.la
fdetach_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
fdetach_CFLAGS			= $(USER_CFLAGS)

insf_SOURCES			= src/util/insf.c
insf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
insf_LDFLAGS			= -static libstreams.la
insf_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
insf_CFLAGS			= $(USER_CFLAGS)

scls_SOURCES			= src/util/scls.c
scls_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
scls_LDFLAGS			= -static libstreams.la
scls_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
scls_CFLAGS			= $(USER_CFLAGS)

strace_SOURCES			= src/util/strace.c
strace_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strace_LDFLAGS			= -static libstreams.la
strace_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strace_CFLAGS			= $(USER_CFLAGS)

##strclean_SOURCES		= src/util/strclean.c
##strclean_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
##strclean_LDFLAGS		= -static libstreams.la
##strclean_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
##strclean_CFLAGS		= $(USER_CFLAGS)

##@srcdir@/src/util/strclean.c:
##	touch $@

##@srcdir@/src/util/strclean: @srcdir@/src/util/strclean.c
##	touch $@

strerr_SOURCES			= src/util/strerr.c
strerr_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strerr_LDFLAGS			= -static libstreams.la
strerr_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strerr_CFLAGS			= $(USER_CFLAGS)

strinfo_SOURCES			= src/util/strinfo.c
strinfo_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strinfo_LDFLAGS			= -static libstreams.la
strinfo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strinfo_CFLAGS			= $(USER_CFLAGS)

##strload_SOURCES		= src/util/strload.c
##strload_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
##strload_LDFLAGS		= -static libstreams.la
##strload_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
##strload_CFLAGS			= $(USER_CFLAGS)

##@srcdir@/src/util/strload.c:
##	touch $@

##@srcdir@/src/util/strload: @srcdir@/src/util/strload.c
##	touch $@

##strsetup_SOURCES		= src/util/strsetup.c
##strsetup_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
##strsetup_LDFLAGS		= -static libstreams.la
##strsetup_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
##strsetup_CFLAGS		= $(USER_CFLAGS)

##@srcdir@/src/util/strsetup.c:
##	touch $@

##@srcdir@/src/util/strsetup: @srcdir@/src/util/strsetup.c
##	touch $@

strvf_SOURCES			= src/util/strvf.c
strvf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
strvf_LDFLAGS			= -static libstreams.la
strvf_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strvf_CFLAGS			= $(USER_CFLAGS)

@PACKAGE_TARNAME@_mknod_SOURCES		= src/util/@PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_clone_SOURCES		= src/test/test-clone.c
test_clone_LDFLAGS		= -static libstreams.la
test_clone_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_clone_CFLAGS		= $(LFS_CFLAGS)

test_connld_SOURCES		= src/test/test-connld.c
test_connld_LDFLAGS		= -static libstreams.la
test_connld_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_connld_CFLAGS		= $(LFS_CFLAGS)

test_echo_SOURCES		= src/test/test-echo.c
test_echo_LDFLAGS		= -static libstreams.la
test_echo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_echo_CFLAGS		= $(LFS_CFLAGS)

test_fifo_SOURCES		= src/test/test-fifo.c
test_fifo_LDFLAGS		= -static libstreams.la
test_fifo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(LFS_CFLAGS)

test_log_SOURCES		= src/test/test-log.c
test_log_LDFLAGS		= -static libstreams.la
test_log_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_log_CFLAGS			= $(LFS_CFLAGS)

test_loop_SOURCES		= src/test/test-loop.c
test_loop_LDFLAGS		= -static libstreams.la
test_loop_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_loop_CFLAGS		= $(LFS_CFLAGS)

test_nsdev_SOURCES		= src/test/test-nsdev.c
test_nsdev_LDFLAGS		= -static libstreams.la
test_nsdev_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nsdev_CFLAGS		= $(LFS_CFLAGS)

test_mux_SOURCES		= src/test/test-mux.c
test_mux_LDFLAGS		= -static libstreams.la
test_mux_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_mux_CFLAGS			= $(LFS_CFLAGS)

test_nuls_SOURCES		= src/test/test-nuls.c
test_nuls_LDFLAGS		= -static libstreams.la
test_nuls_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nuls_CFLAGS		= $(LFS_CFLAGS)

test_pipe_SOURCES		= src/test/test-pipe.c
test_pipe_LDFLAGS		= -static libstreams.la
test_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(LFS_CFLAGS)

perftest_SOURCES		= src/test/perftest.c
perftest_LDFLAGS		= -static libstreams.la
perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
perftest_CFLAGS			= $(LFS_CFLAGS)

perftestn_SOURCES		= src/test/perftest.c
perftestn_LDFLAGS		= 
perftestn_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNATIVE_PIPES
perftestn_CFLAGS		= $(LFS_CFLAGS)

test_pipemod_SOURCES		= src/test/test-pipemod.c
test_pipemod_LDFLAGS		= -static libstreams.la
test_pipemod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipemod_CFLAGS		= $(LFS_CFLAGS)

test_sad_SOURCES		= src/test/test-sad.c
test_sad_LDFLAGS		= -static libstreams.la
test_sad_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(LFS_CFLAGS)

test_sc_SOURCES			= src/test/test-sc.c
test_sc_LDFLAGS			= -static libstreams.la
test_sc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sc_CFLAGS			= $(LFS_CFLAGS)

test_streams_SOURCES		= src/test/test-streams.c
test_streams_LDFLAGS		= -static libstreams.la
test_streams_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(LFS_CFLAGS)

## PKG_BUILD_ARCH
endif
## RPM_BUILD_USER
endif

AUTOCONFIG_H			= include/sys/config.h

##EXTRA_DIST			+= $(AUTOCONFIG_H).in

if CONFIG_STREAMS_SEPARATE_COMPILE
EXPORT_SRCFILES			= \
				  src/kernel/strattach.c \
				  src/kernel/strlookup.c \
				  src/kernel/strpipe.c \
				  src/kernel/strreg.c \
				  src/kernel/strsad.c \
				  src/kernel/strsched.c \
				  src/kernel/strspecfs.c \
				  src/kernel/strsysctl.c \
				  src/kernel/strutil.c \
				  src/kernel/strsyms.c

if CONFIG_STREAMS_CLONE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif
if CONFIG_STREAMS_STH
EXPORT_SRCFILES			+= src/modules/sth.c
endif
else
EXPORT_SRCFILES			= \
				  src/kernel/specfs.c \
				  src/kernel/streams.c
endif

if CONFIG_STREAMS_CLONE_MODULE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif
if CONFIG_STREAMS_STH_MODULE
EXPORT_SRCFILES			+= src/modules/sth.c
endif

if CONFIG_STREAMS_SEPARATE_COMPILE
EXPORT_VERFILES			= \
				  $(PKGINCL)/versions/strattach.ver \
				  $(PKGINCL)/versions/strlookup.ver \
				  $(PKGINCL)/versions/strpipe.ver \
				  $(PKGINCL)/versions/strreg.ver \
				  $(PKGINCL)/versions/strsad.ver \
				  $(PKGINCL)/versions/strsched.ver \
				  $(PKGINCL)/versions/strspecfs.ver \
				  $(PKGINCL)/versions/strsysctl.ver \
				  $(PKGINCL)/versions/strutil.ver \
				  $(PKGINCL)/versions/strsyms.ver
if CONFIG_STREAMS_CLONE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif
if CONFIG_STREAMS_STH
EXPORT_VERFILES			+= $(PKGINCL)/versions/sth.ver
endif
else
EXPORT_VERFILES			= \
				  $(PKGINCL)/versions/specfs.ver \
				  $(PKGINCL)/versions/streams.ver
endif

if CONFIG_STREAMS_CLONE_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif
if CONFIG_STREAMS_STH_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/sth.ver
endif

GENKSYMS_CPPFLAGS		= \
				  -I./src/kernel -I$(srcdir)/src/kernel \
				  -I./src/drivers -I$(srcdir)/src/drivers \
				  -I./src/modules -I$(srcdir)/src/modules \
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

SUBDIRS =

if DEVELOPMENT
SUBDIRS += include
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

dist_initrd_SCRIPTS		= \
				  src/util/init/specfs.sh \
				  src/util/init/@PACKAGE_LCNAME@.sh
if CONFIG_STREAMS_LOG
dist_initrd_SCRIPTS		+= src/util/init/strace.sh \
				   src/util/init/strerr.sh
endif
if CONFIG_STREAMS_LOG_MODULE
dist_initrd_SCRIPTS		+= src/util/init/strace.sh \
				   src/util/init/strerr.sh
endif

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/specfs \
				  src/util/init/@PACKAGE_LCNAME@
if CONFIG_STREAMS_LOG
dist_initrd_SCRIPTS		+= src/util/init/strace \
				   src/util/init/strerr
endif
if CONFIG_STREAMS_LOG_MODULE
dist_initrd_SCRIPTS		+= src/util/init/strace \
				   src/util/init/strerr
endif

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				  src/util/sysconfig/@PACKAGE_LCNAME@.conf
if CONFIG_STREAMS_UTILS
dist_sysconf_DATA		+= \
				  strsetup.conf \
				  strload.conf
endif

dist_config_DATA		= \
				  src/util/sysconfig/specfs \
				  src/util/sysconfig/@PACKAGE_LCNAME@
if CONFIG_STREAMS_LOG
dist_config_DATA		+= src/util/sysconfig/strace \
				   src/util/sysconfig/strerr
endif
if CONFIG_STREAMS_LOG_MODULE
dist_config_DATA		+= src/util/sysconfig/strace \
				   src/util/sysconfig/strerr
endif

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
MANUAL_FILE			= doc/manual/$(PACKAGE_UCNAME).txt
RELEASE_FILES			= \
				  README \
				  README-alpha \
				  README-cvs \
				  README-LiS \
				  README-nexusware

include am/release.am

EXTRA_DIST			+= \
				  TODO \
				  MANUAL \
				  README-cvs \
				  README-nexusware \
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

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
