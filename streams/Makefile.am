# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.56 $) $Date: 2004/08/22 09:20:04 $
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
# Last Modified $Date: 2004/08/22 09:20:04 $ by $Author: brian $
#
# =============================================================================

if RPM_BUILD_USER

bin_PROGRAMS			= \
				strchg \
				strconf \
				strreset

sbin_PROGRAMS			= \
				autopush \
				fattach \
				fdetach \
				insf \
				scls \
				strace \
				strerr \
				strinfo \
				strvf \
				@PACKAGE_TARNAME@_mknod

dist_sbin_SCRIPTS		= \
				$(srcdir)/src/util/strclean \
				$(srcdir)/src/util/strload \
				$(srcdir)/src/util/strsetup

libexec_PROGRAMS		= \
				test-clone \
				test-connld \
				test-echo \
				test-fifo \
				test-log \
				test-nsdev \
				test-nuls \
				test-pipe \
				test-pipemod \
				test-sad \
				test-sc \
				test-streams


#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= @PACKAGE_TARNAME@_mknod

LDADD				= libstreams.la

lib_LTLIBRARIES			= libstreams.la

endif

if RPM_BUILD_KERNEL

noinst_LIBRARIES		= \
				libLfSspecfs.a \
				libLfSkernel.a \
				libLfSdrivers.a \
				libLfSmodules.a \
				libLfScompat.a

libLfSspecfs_a_SOURCES		= \
				src/kernel/strspecfs.c \
				src/kernel/strspecfs.h \
				src/kernel/strlookup.c \
				src/kernel/strlookup.h \
				src/kernel/strreg.c \
				src/kernel/strreg.h \
				src/kernel/strattach.c \
				src/kernel/strattach.h \
				src/kernel/strpipe.c \
				src/kernel/strpipe.h \
				src/kernel/strargs.h

libLfSspecfs_a_DEPENDENCIES	= $(STRCONF_CONFIG) $(STRCONF_MODCONF) $(MODVERSIONS_H)
libLfSspecfs_a_CPPFLAGS		= \
				-I./src/kernel -I$(srcdir)/src/kernel \
				-I./src/drivers -I$(srcdir)/src/drivers \
				-I./src/modules -I$(srcdir)/src/modules \
				$(LFS_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) -DEXPORT_SYMTAB
libLfSspecfs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfSspecfs_a_OBJECTS): $(libLfSspecfs_a_DEPENDENCIES)


libLfSkernel_a_SOURCES		= \
				src/kernel/strmain.c \
				src/kernel/strprocfs.c \
				src/kernel/strprocfs.h \
				src/kernel/strsched.c \
				src/kernel/strsched.h \
				src/kernel/strsysctl.c \
				src/kernel/strsysctl.h \
				src/kernel/strsyms.c \
				src/kernel/strutil.c \
				src/kernel/strutil.h \
				src/kernel/strsad.c \
				src/kernel/strsad.h \
				src/kernel/strargs.h

libLfSkernel_a_DEPENDENCIES	= $(STRCONF_CONFIG) $(STRCONF_MODCONF) $(MODVERSIONS_H)
libLfSkernel_a_CPPFLAGS		= \
				-I./src/kernel -I$(srcdir)/src/kernel \
				-I./src/drivers -I$(srcdir)/src/drivers \
				-I./src/modules -I$(srcdir)/src/modules \
				$(LFS_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) -DEXPORT_SYMTAB
libLfSkernel_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfSkernel_a_OBJECTS): $(libLfSkernel_a_DEPENDENCIES)

libLfSdrivers_a_SOURCES		= \
				src/drivers/echo.c \
				src/drivers/nuls.c \
				src/drivers/sad.c \
				include/sys/sad.h \
				src/drivers/clone.c \
				src/drivers/clone.h \
				src/drivers/fifo.c \
				src/drivers/fifo.h \
				src/drivers/nsdev.c \
				src/drivers/pipe.c \
				src/drivers/pipe.h \
				include/log.h \
				include/sys/log.h \
				include/strlog.h \
				include/sys/strlog.h \
				src/drivers/log.h \
				src/drivers/log.c
#				src/drivers/sfx.c
#				src/drivers/spx.c

libLfSdrivers_a_DEPENDENCIES	= $(STRCONF_CONFIG) $(STRCONF_MODCONF) $(MODVERSIONS_H)
libLfSdrivers_a_CPPFLAGS	= \
				-I./src/kernel -I$(srcdir)/src/kernel \
				-I./src/drivers -I$(srcdir)/src/drivers \
				-I./src/modules -I$(srcdir)/src/modules \
				$(LFS_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) -DEXPORT_SYMTAB
libLfSdrivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfSdrivers_a_OBJECTS): $(libLfSdrivers_a_DEPENDENCIES)

libLfSmodules_a_SOURCES		= \
				src/modules/sth.c \
				src/modules/sth.h \
				src/modules/connld.c \
				src/modules/pipemod.c \
				src/modules/sc.c \
				include/sys/sc.h

libLfSmodules_a_DEPENDENCIES	= $(STRCONF_CONFIG) $(STRCONF_MODCONF) $(MODVERSIONS_H)
libLfSmodules_a_CPPFLAGS	= \
				-I./src/kernel -I$(srcdir)/src/kernel \
				-I./src/drivers -I$(srcdir)/src/drivers \
				-I./src/modules -I$(srcdir)/src/modules \
				$(LFS_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) -DEXPORT_SYMTAB
libLfSmodules_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfSmodules_a_OBJECTS): $(libLfSmodules_a_DEPENDENCIES)

libLfScompat_a_SOURCES		= \
				src/compat/aix/aixcompat.c \
				src/compat/hpux/hpuxcompat.c \
				src/compat/lis/liscompat.c \
				src/compat/osf/osfcompat.c \
				src/compat/sun/suncompat.c \
				src/compat/svr4/svr4compat.c \
				src/compat/uw7/uw7compat.c

libLfScompat_a_DEPENDENCIES	= $(STRCONF_CONFIG) $(STRCONF_MODCONF) $(MODVERSIONS_H)
libLfScompat_a_CPPFLAGS		= \
				-I./src/compat -I$(srcdir)/src/compat \
				-I./src/kernel -I$(srcdir)/src/kernel \
				-I./src/drivers -I$(srcdir)/src/drivers \
				-I./src/modules -I$(srcdir)/src/modules \
				$(LFS_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS) -DEXPORT_SYMTAB
libLfScompat_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfScompat_a_OBJECTS): $(libLfScompat_a_DEPENDENCIES)

specfs.o: $(libLfSspecfs_a_OBJECTS)
	$(LD) -r -o $@ $^

streams.o: $(libLfSkernel_a_OBJECTS)
	$(LD) -r -o $@ $^

endif

EXTRA_DIST			=
dist_noinst_SCRIPTS		= configure.nexusware
EXTRA_SCRIPTS			=

KERNEL_MODULES			= specfs.o streams.o $(libLfSdrivers_a_OBJECTS) $(libLfSmodules_a_OBJECTS) $(libLfScompat_a_OBJECTS)
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= streams
KMODCONF_EXT			= streams

include am/kernel.am

include am/libraries.am

if RPM_BUILD_USER

libstreams_la_SOURCES		= \
				lib/libc/fattach.c \
				lib/libc/fdetach.c \
				lib/libc/getmsg.c \
				lib/libc/getpmsg.c \
				lib/libc/isastream.c \
				lib/libc/pipe.c \
				lib/libc/putmsg.c \
				lib/libc/putpmsg.c
libstreams_la_LDFLAGS		= -no-undefined -version-info 0:0:0
libstreams_la_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS)

EXTRA_PROGRAMS			= @PACKAGE_TARNAME@_mknod

#bin_PROGRAMS			= timetst strtst $(LFS_EXTRA_TESTS)
#sbin_PROGRAMS			= streams $(LFS_EXTRA_TOOLS)

strchg_SOURCES			= src/util/strchg.c include/sys/config.h
strchg_DEPENDENCIES		= include/sys/config.h
strchg_LDADD			= libstreams.la
strchg_CPPFLAGS			= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strchg_CFLAGS			= $(USER_CFLAGS)

strconf_SOURCES			= src/util/strconf.c include/sys/config.h
strconf_DEPENDENCIES		= include/sys/config.h
strconf_LDADD			= libstreams.la
strconf_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strconf_CFLAGS			= $(USER_CFLAGS)

strreset_SOURCES		= src/util/strreset.c include/sys/config.h
strreset_DEPENDENCIES		= include/sys/config.h
strreset_LDADD			= libstreams.la
strreset_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strreset_CFLAGS			= $(USER_CFLAGS)

autopush_SOURCES		= src/util/autopush.c include/sys/config.h
autopush_DEPENDENCIES		= include/sys/config.h
autopush_LDFLAGS		= -static libstreams.la
autopush_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
autopush_CFLAGS			= $(USER_CFLAGS)

fattach_SOURCES			= src/util/fattach.c include/sys/config.h
fattach_DEPENDENCIES		= include/sys/config.h
fattach_LDFLAGS			= -static libstreams.la
fattach_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
fattach_CFLAGS			= $(USER_CFLAGS)

fdetach_SOURCES			= src/util/fdetach.c include/sys/config.h
fdetach_DEPENDENCIES		= include/sys/config.h
fdetach_LDFLAGS			= -static libstreams.la
fdetach_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
fdetach_CFLAGS			= $(USER_CFLAGS)

insf_SOURCES			= src/util/insf.c include/sys/config.h
insf_DEPENDENCIES		= include/sys/config.h
insf_LDFLAGS			= -static libstreams.la
insf_CPPFLAGS			= $(LFS_INCLUDES) $(USER_CPPFLAGS)
insf_CFLAGS			= $(USER_CFLAGS)

scls_SOURCES			= src/util/scls.c include/sys/config.h
scls_DEPENDENCIES		= include/sys/config.h
scls_LDFLAGS			= -static libstreams.la
scls_CPPFLAGS			= $(LFS_INCLUDES) $(USER_CPPFLAGS)
scls_CFLAGS			= $(USER_CFLAGS)

strace_SOURCES			= src/util/strace.c include/sys/config.h
strace_DEPENDENCIES		= include/sys/config.h
strace_LDFLAGS			= -static libstreams.la
strace_CPPFLAGS			= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strace_CFLAGS			= $(USER_CFLAGS)

#strclean_SOURCES		= src/util/strclean.c include/sys/config.h
#strclean_DEPENDENCIES		= include/sys/config.h
#strclean_LDFLAGS		= -static libstreams.la
#strclean_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
#strclean_CFLAGS		= $(USER_CFLAGS)

#@srcdir@/src/util/strclean.c:
#	touch $@

#@srcdir@/src/util/strclean: @srcdir@/src/util/strclean.c
#	touch $@

strerr_SOURCES			= src/util/strerr.c include/sys/config.h
strerr_DEPENDENCIES		= include/sys/config.h
strerr_LDFLAGS			= -static libstreams.la
strerr_CPPFLAGS			= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strerr_CFLAGS			= $(USER_CFLAGS)

strinfo_SOURCES			= src/util/strinfo.c include/sys/config.h
strinfo_DEPENDENCIES		= include/sys/config.h
strinfo_LDFLAGS			= -static libstreams.la
strinfo_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strinfo_CFLAGS			= $(USER_CFLAGS)

#strload_SOURCES		= src/util/strload.c include/sys/config.h
#strload_DEPENDENCIES		= include/sys/config.h
#strload_LDFLAGS		= -static libstreams.la
#strload_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
#strload_CFLAGS			= $(USER_CFLAGS)

#@srcdir@/src/util/strload.c:
#	touch $@

#@srcdir@/src/util/strload: @srcdir@/src/util/strload.c
#	touch $@

#strsetup_SOURCES		= src/util/strsetup.c include/sys/config.h
#strsetup_DEPENDENCIES		= include/sys/config.h
#strsetup_LDFLAGS		= -static libstreams.la
#strsetup_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
#strsetup_CFLAGS		= $(USER_CFLAGS)

#@srcdir@/src/util/strsetup.c:
#	touch $@

#@srcdir@/src/util/strsetup: @srcdir@/src/util/strsetup.c
#	touch $@

strvf_SOURCES			= src/util/strvf.c include/sys/config.h
strvf_DEPENDENCIES		= include/sys/config.h
strvf_LDFLAGS			= -static libstreams.la
strvf_CPPFLAGS			= $(LFS_INCLUDES) $(USER_CPPFLAGS)
strvf_CFLAGS			= $(USER_CFLAGS)

@PACKAGE_TARNAME@_mknod_SOURCES		= src/util/@PACKAGE_TARNAME@_mknod.c include/sys/config.h
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= include/sys/config.h
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(LFS_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_clone_SOURCES		= src/test/test-clone.c
test_clone_LDFLAGS		= libstreams.la
test_clone_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_clone_CFLAGS		= $(LFS_CFLAGS)

test_connld_SOURCES		= src/test/test-connld.c
test_connld_LDFLAGS		= libstreams.la
test_connld_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_connld_CFLAGS		= $(LFS_CFLAGS)

test_echo_SOURCES		= src/test/test-echo.c
test_echo_LDFLAGS		= libstreams.la
test_echo_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_echo_CFLAGS		= $(LFS_CFLAGS)

test_fifo_SOURCES		= src/test/test-fifo.c
test_fifo_LDFLAGS		= libstreams.la
test_fifo_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(LFS_CFLAGS)

test_log_SOURCES		= src/test/test-log.c
test_log_LDFLAGS		= libstreams.la
test_log_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_log_CFLAGS			= $(LFS_CFLAGS)

test_nsdev_SOURCES		= src/test/test-nsdev.c
test_nsdev_LDFLAGS		= libstreams.la
test_nsdev_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_nsdev_CFLAGS		= $(LFS_CFLAGS)

test_nuls_SOURCES		= src/test/test-nuls.c
test_nuls_LDFLAGS		= libstreams.la
test_nuls_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_nuls_CFLAGS		= $(LFS_CFLAGS)

test_pipe_SOURCES		= src/test/test-pipe.c
test_pipe_LDFLAGS		= libstreams.la
test_pipe_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(LFS_CFLAGS)

test_pipemod_SOURCES		= src/test/test-pipemod.c
test_pipemod_LDFLAGS		= libstreams.la
test_pipemod_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_pipemod_CFLAGS		= $(LFS_CFLAGS)

test_sad_SOURCES		= src/test/test-sad.c
test_sad_LDFLAGS		= libstreams.la
test_sad_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(LFS_CFLAGS)

test_sc_SOURCES			= src/test/test-sc.c
test_sc_LDFLAGS			= libstreams.la
test_sc_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_sc_CFLAGS			= $(LFS_CFLAGS)

test_streams_SOURCES		= src/test/test-streams.c
test_streams_LDFLAGS		= libstreams.la
test_streams_CPPFLAGS		= $(LFS_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(LFS_CFLAGS)

endif

EXPORT_SRCFILES = \
	src/compat/aix/aixcompat.c \
	src/compat/hpux/hpuxcompat.c \
	src/compat/lis/liscompat.c \
	src/compat/osf/osfcompat.c \
	src/compat/sun/suncompat.c \
	src/compat/svr4/svr4compat.c \
	src/compat/uw7/uw7compat.c \
	src/drivers/clone.c \
	src/kernel/strattach.c \
	src/kernel/strlookup.c \
	src/kernel/strpipe.c \
	src/kernel/strreg.c \
	src/kernel/strsad.c \
	src/kernel/strsched.c \
	src/kernel/strspecfs.c \
	src/kernel/strsysctl.c \
	src/kernel/strutil.c \
	src/kernel/strsyms.c \
	src/modules/sth.c

EXPORT_VERFILES = \
	include/sys/@PACKAGE_NAME@/versions/aixcompat.ver \
	include/sys/@PACKAGE_NAME@/versions/hpuxcompat.ver \
	include/sys/@PACKAGE_NAME@/versions/liscompat.ver \
	include/sys/@PACKAGE_NAME@/versions/osfcompat.ver \
	include/sys/@PACKAGE_NAME@/versions/suncompat.ver \
	include/sys/@PACKAGE_NAME@/versions/svr4compat.ver \
	include/sys/@PACKAGE_NAME@/versions/uw7compat.ver \
	include/sys/@PACKAGE_NAME@/versions/clone.ver \
	include/sys/@PACKAGE_NAME@/versions/strattach.ver \
	include/sys/@PACKAGE_NAME@/versions/strlookup.ver \
	include/sys/@PACKAGE_NAME@/versions/strpipe.ver \
	include/sys/@PACKAGE_NAME@/versions/strreg.ver \
	include/sys/@PACKAGE_NAME@/versions/strsad.ver \
	include/sys/@PACKAGE_NAME@/versions/strsched.ver \
	include/sys/@PACKAGE_NAME@/versions/strspecfs.ver \
	include/sys/@PACKAGE_NAME@/versions/strsysctl.ver \
	include/sys/@PACKAGE_NAME@/versions/strutil.ver \
	include/sys/@PACKAGE_NAME@/versions/strsyms.ver \
	include/sys/@PACKAGE_NAME@/versions/sth.ver

PKGINCL = include/sys/$(PACKAGE_NAME)

GENKSYMS_CPPFLAGS = \
	-I./src/compat -I$(srcdir)/src/compat \
	-I./src/kernel -I$(srcdir)/src/kernel \
	-I./src/drivers -I$(srcdir)/src/drivers \
	-I./src/modules -I$(srcdir)/src/modules \
	$(LFS_INCLUDES) $(KERNEL_CPPFLAGS)

include am/genksyms.am

STRMAKENODES			= @PACKAGE_TARNAME@_mknod

include am/strconf.am

if RPM_BUILD_KERNEL

install-devices: $(STRMAKENODES)
	./$(STRMAKENODES)

uninstall-devices: $(STRMAKENODES)
	./$(STRMAKENODES) --remove

endif

if RPM_BUILD_USER

SUBDIRS = include doc tests

endif

#
# Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion $(PACKAGE_KVERSION)"
AM_SRPMFLAGS =

include am/rpm.am

#
# General rules
#
include am/openss7.am

# vim: ft=automake
