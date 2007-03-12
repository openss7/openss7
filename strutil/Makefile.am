# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2007/03/08 06:08:58 $
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
# Last Modified $Date: 2007/03/08 06:08:58 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.24  2007/03/08 06:08:58  brian
# - modutil_DATA should not be distributed (and it reorders uninstall)
#
# Revision 0.9.2.23  2007/03/04 00:24:13  brian
# - add version file creation
#
# Revision 0.9.2.22  2006/10/03 13:53:30  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.21  2006/10/01 21:30:25  brian
# - distribute release and library version files
#
# Revision 0.9.2.20  2006/09/29 11:51:06  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.19  2006/09/27 05:15:31  brian
# - distinguish LDADD from LDFLAGS
#
# Revision 0.9.2.18  2006/09/18 01:15:35  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.17  2006/08/26 09:19:48  brian
# - better release file generation
#
# Revision 0.9.2.16  2006/07/01 11:59:44  brian
# - added new KERNEL_BLDFLAGS to makefiles
#
# Revision 0.9.2.15  2006/03/21 13:24:43  brian
# - included pr.am
#
# Revision 0.9.2.14  2006/03/20 11:54:06  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.13  2006/03/07 12:34:23  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.12  2006/03/06 11:39:08  brian
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
lib32_LTLIBRARIES		=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod

pkglibexec_PROGRAMS		= \
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

sbin_PROGRAMS			= \
				  autopush \
				  fattach \
				  fdetach \
				  insf \
				  scls \
				  strace \
				  strerr \
				  strinfo \
				  strlogd \
				  strvf \
				  @PACKAGE_TARNAME@_mknod

bin_PROGRAMS			= \
				  strchg \
				  strconf \
				  strlog \
				  strreset

dist_sbin_SCRIPTS		= \
				  src/util/strclean \
				  src/util/strload \
				  src/util/strsetup


#UTIL_EXTRA_TESTS		=
#UTIL_EXTRA_TOOLS		=


@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

strchg_SOURCES			= src/util/strchg.c
strchg_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strchg_LDADD			= $(STREAMS_LDADD)
strchg_LDFLAGS			= $(STREAMS_LDFLAGS)
strchg_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strchg_CFLAGS			= $(USER_CFLAGS)

strconf_SOURCES			= src/util/strconf.c
strconf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strconf_LDADD			= $(STREAMS_LDADD)
strconf_LDFLAGS			= $(STREAMS_LDFLAGS)
strconf_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strconf_CFLAGS			= $(USER_CFLAGS)

strlog_SOURCES			= src/util/strlog.c
strlog_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strlog_LDADD			= $(STREAMS_LDADD)
strlog_LDFLAGS			= $(STREAMS_LDFLAGS) -lrt
strlog_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strlog_CFLAGS			= $(USER_CFLAGS)

strreset_SOURCES		= src/util/strreset.c
strreset_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strreset_LDADD			= $(STREAMS_LDADD)
strreset_LDFLAGS		= $(STREAMS_LDFLAGS)
strreset_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strreset_CFLAGS			= $(USER_CFLAGS)

autopush_SOURCES		= src/util/autopush.c
autopush_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
autopush_LDADD			= $(STREAMS_LDADD)
autopush_LDFLAGS		= $(STREAMS_LDFLAGS)
autopush_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
autopush_CFLAGS			= $(USER_CFLAGS)

fattach_SOURCES			= src/util/fattach.c
fattach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
fattach_LDADD			= $(STREAMS_LDADD)
fattach_LDFLAGS			= $(STREAMS_LDFLAGS)
fattach_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
fattach_CFLAGS			= $(USER_CFLAGS)

fdetach_SOURCES			= src/util/fdetach.c 
fdetach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
fdetach_LDADD			= $(STREAMS_LDADD)
fdetach_LDFLAGS			= $(STREAMS_LDFLAGS)
fdetach_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
fdetach_CFLAGS			= $(USER_CFLAGS)

insf_SOURCES			= src/util/insf.c
insf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
insf_LDADD			= $(STREAMS_LDADD)
insf_LDFLAGS			= $(STREAMS_LDFLAGS)
insf_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
insf_CFLAGS			= $(USER_CFLAGS)

scls_SOURCES			= src/util/scls.c
scls_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
scls_LDADD			= $(STREAMS_LDADD)
scls_LDFLAGS			= $(STREAMS_LDFLAGS)
scls_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
scls_CFLAGS			= $(USER_CFLAGS)

strace_SOURCES			= src/util/strace.c
strace_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strace_LDADD			= $(STREAMS_LDADD)
strace_LDFLAGS			= $(STREAMS_LDFLAGS)
strace_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strace_CFLAGS			= $(USER_CFLAGS)

#strclean_SOURCES		= src/util/strclean.c
#strclean_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
#strclean_LDADD			= $(STREAMS_LDADD)
#strclean_LDFLAGS		= $(STREAMS_LDFLAGS)
#strclean_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#strclean_CFLAGS		= $(USER_CFLAGS)

#@srcdir@/src/util/strclean.c:
#	touch $@

#@srcdir@/src/util/strclean: @srcdir@/src/util/strclean.c
#	touch $@

strerr_SOURCES			= src/util/strerr.c
strerr_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strerr_LDADD			= $(STREAMS_LDADD)
strerr_LDFLAGS			= $(STREAMS_LDFLAGS)
strerr_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strerr_CFLAGS			= $(USER_CFLAGS)

strinfo_SOURCES			= src/util/strinfo.c
strinfo_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strinfo_LDADD			= $(STREAMS_LDADD)
strinfo_LDFLAGS			= $(STREAMS_LDFLAGS)
strinfo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strinfo_CFLAGS			= $(USER_CFLAGS)

#strload_SOURCES		= src/util/strload.c
#strload_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
#strload_LDADD			= $(STREAMS_LDADD)
#strload_LDFLAGS		= $(STREAMS_LDFLAGS)
#strload_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#strload_CFLAGS			= $(USER_CFLAGS)

#@srcdir@/src/util/strload.c:
#	touch $@

#@srcdir@/src/util/strload: @srcdir@/src/util/strload.c
#	touch $@

strlogd_SOURCES			= src/util/strlogd.c
strlogd_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strlogd_LDADD			= $(STREAMS_LDADD)
strlogd_LDFLAGS			= $(STREAMS_LDFLAGS)
strlogd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strlogd_CFLAGS			= $(USER_CFLAGS)

#strsetup_SOURCES		= src/util/strsetup.c
#strsetup_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
#strsetup_LDADD			= $(STREAMS_LDADD)
#strsetup_LDFLAGS		= $(STREAMS_LDFLAGS)
#strsetup_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#strsetup_CFLAGS		= $(USER_CFLAGS)

#@srcdir@/src/util/strsetup.c:
#	touch $@

#@srcdir@/src/util/strsetup: @srcdir@/src/util/strsetup.c
#	touch $@

strvf_SOURCES			= src/util/strvf.c
strvf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strvf_LDADD			= $(STREAMS_LDADD)
strvf_LDFLAGS			= $(STREAMS_LDFLAGS)
strvf_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
strvf_CFLAGS			= $(USER_CFLAGS)

test_clone_SOURCES		= src/test/test-clone.c
test_clone_LDADD		= $(STREAMS_LDADD)
test_clone_LDFLAGS		= $(STREAMS_LDFLAGS)
test_clone_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_clone_CFLAGS		= $(LFS_CFLAGS)

test_connld_SOURCES		= src/test/test-connld.c
test_connld_LDADD		= $(STREAMS_LDADD)
test_connld_LDFLAGS		= $(STREAMS_LDFLAGS)
test_connld_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_connld_CFLAGS		= $(LFS_CFLAGS)

test_echo_SOURCES		= src/test/test-echo.c
test_echo_LDADD			= $(STREAMS_LDADD)
test_echo_LDFLAGS		= $(STREAMS_LDFLAGS)
test_echo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_echo_CFLAGS		= $(LFS_CFLAGS)

test_fifo_SOURCES		= src/test/test-fifo.c
test_fifo_LDADD			= $(STREAMS_LDADD)
test_fifo_LDFLAGS		= $(STREAMS_LDFLAGS)
test_fifo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(LFS_CFLAGS)

test_log_SOURCES		= src/test/test-log.c
test_log_LDADD			= $(STREAMS_LDADD)
test_log_LDFLAGS		= $(STREAMS_LDFLAGS)
test_log_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_log_CFLAGS			= $(LFS_CFLAGS)

test_nsdev_SOURCES		= src/test/test-nsdev.c
test_nsdev_LDADD		= $(STREAMS_LDADD)
test_nsdev_LDFLAGS		= $(STREAMS_LDFLAGS)
test_nsdev_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nsdev_CFLAGS		= $(LFS_CFLAGS)

test_nuls_SOURCES		= src/test/test-nuls.c
test_nuls_LDADD			= $(STREAMS_LDADD)
test_nuls_LDFLAGS		= $(STREAMS_LDFLAGS)
test_nuls_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nuls_CFLAGS		= $(LFS_CFLAGS)

test_pipe_SOURCES		= src/test/test-pipe.c
test_pipe_LDADD			= $(STREAMS_LDADD)
test_pipe_LDFLAGS		= $(STREAMS_LDFLAGS)
test_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(LFS_CFLAGS)

test_sad_SOURCES		= src/test/test-sad.c
test_sad_LDADD			= $(STREAMS_LDADD)
test_sad_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sad_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(LFS_CFLAGS)

test_pipemod_SOURCES		= src/test/test-pipemod.c
test_pipemod_LDADD		= $(STREAMS_LDADD)
test_pipemod_LDFLAGS		= $(STREAMS_LDFLAGS)
test_pipemod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipemod_CFLAGS		= $(LFS_CFLAGS)

test_sc_SOURCES			= src/test/test-sc.c
test_sc_LDADD			= $(STREAMS_LDADD)
test_sc_LDFLAGS			= $(STREAMS_LDFLAGS)
test_sc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sc_CFLAGS			= $(LFS_CFLAGS)

test_streams_SOURCES		= src/test/test-streams.c
test_streams_LDADD		= $(STREAMS_LDADD)
test_streams_LDFLAGS		= $(STREAMS_LDFLAGS)
test_streams_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(LFS_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libUTILlink.a libUTILmods.a libUTILdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

EXTRA_libUTILlink_a_SOURCES	= \
				  src/drivers/clone.c \
				  src/drivers/echo.c \
				  src/drivers/fifo.c \
				  src/drivers/log.c \
				  src/drivers/nsdev.c \
				  src/drivers/nuls.c \
				  src/drivers/pipe.c \
				  src/drivers/sad.c \
				  src/drivers/sfx.c \
				  src/drivers/spx.c \
				  src/modules/pipemod.c \
				  src/modules/connld.c \
				  src/modules/sc.c

libUTILlink_a_SOURCES		= \
				  src/drivers/clone.h \
				  src/drivers/fifo.h \
				  src/drivers/log.h \
				  src/drivers/pipe.h \
				  src/include/sys/log.h \
				  src/include/sys/sad.h \
				  src/include/sys/sc.h \
				  src/include/sys/strlog.h \
				  src/include/sys/strutil/log.h \
				  src/include/sys/strutil/sad.h \
				  src/include/sys/strutil/sc.h \
				  src/include/sys/strutil/strlog.h

libUTILlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_CLONE
libUTILlink_a_SOURCES		+= src/drivers/clone.c
libUTILlink_a_DEPENDENCIES	+= src/drivers/clone.h
endif
if CONFIG_STREAMS_ECHO
libUTILlink_a_SOURCES		+= src/drivers/echo.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_FIFO
libUTILlink_a_SOURCES		+= src/drivers/fifo.c
libUTILlink_a_DEPENDENCIES	+= src/drivers/fifo.h
endif
if CONFIG_STREAMS_LOG
libUTILlink_a_SOURCES		+= src/drivers/log.c
libUTILlink_a_DEPENDENCIES	+= src/drivers/log.h src/include/sys/log.h src/include/sys/strlog.h src/include/sys/strutil/log.h src/include/sys/strutil/strlog.h
endif
if CONFIG_STREAMS_NSDEV
libUTILlink_a_SOURCES		+= src/drivers/nsdev.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_NULS
libUTILlink_a_SOURCES		+= src/drivers/nuls.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_PIPE
libUTILlink_a_SOURCES		+= src/drivers/pipe.c
libUTILlink_a_DEPENDENCIES	+= src/drivers/pipe.h
endif
if CONFIG_STREAMS_SAD
libUTILlink_a_SOURCES		+= src/drivers/sad.c
libUTILlink_a_DEPENDENCIES	+= src/include/sys/sad.h src/include/sys/strutil/sad.h
endif
if CONFIG_STREAMS_SFX
libUTILlink_a_SOURCES		+= src/drivers/sfx.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_SPX
libUTILlink_a_SOURCES		+= src/drivers/spx.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_PIPEMOD
libUTILlink_a_SOURCES		+= src/modules/pipemod.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_CONNLD
libUTILlink_a_SOURCES		+= src/modules/connld.c
libUTILlink_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_SC
libUTILlink_a_SOURCES		+= src/modules/sc.c
libUTILlink_a_DEPENDENCIES	+= src/include/sys/sc.h src/include/sys/strutil/sc.h
endif

libUTILlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libUTILlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libUTILlink_a_OBJECTS):	$(libUTILlink_a_SOURCES) $(libUTILlink_a_DEPENDENCIES)

EXTRA_libUTILmods_a_SOURCES	= \
				  src/modules/pipemod.c \
				  src/modules/connld.c \
				  src/modules/sc.c

libUTILmods_a_SOURCES		= \
				  src/include/sys/sc.h \
				  src/include/sys/strutil/sc.h

libUTILmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_PIPEMOD_MODULE
libUTILmods_a_SOURCES		+= src/modules/pipemod.c
libUTILmods_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_CONNLD_MODULE
libUTILmods_a_SOURCES		+= src/modules/connld.c
libUTILmods_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_SC_MODULE
libUTILmods_a_SOURCES		+= src/modules/sc.c
libUTILmods_a_DEPENDENCIES	+= src/include/sys/sc.h src/include/sys/strutil/sc.h
endif

libUTILmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libUTILmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libUTILmods_a_OBJECTS):	$(libUTILmods_a_DEPENDENCIES)

EXTRA_libUTILdrvs_a_SOURCES	= \
				  src/drivers/clone.c \
				  src/drivers/echo.c \
				  src/drivers/fifo.c \
				  src/drivers/log.c \
				  src/drivers/nsdev.c \
				  src/drivers/nuls.c \
				  src/drivers/pipe.c \
				  src/drivers/sad.c \
				  src/drivers/sfx.c \
				  src/drivers/spx.c

libUTILdrvs_a_SOURCES		= \
				  src/drivers/clone.h \
				  src/drivers/fifo.h \
				  src/drivers/log.h \
				  src/drivers/pipe.h \
				  src/include/sys/log.h \
				  src/include/sys/sad.h \
				  src/include/sys/strlog.h \
				  src/include/sys/strutil/log.h \
				  src/include/sys/strutil/sad.h \
				  src/include/sys/strutil/strlog.h

libUTILdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_CLONE_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/clone.c
libUTILdrvs_a_DEPENDENCIES	+= src/drivers/clone.h
endif
if CONFIG_STREAMS_ECHO_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/echo.c
libUTILdrvs_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_FIFO_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/fifo.c
libUTILdrvs_a_DEPENDENCIES	+= src/drivers/fifo.h
endif
if CONFIG_STREAMS_LOG_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/log.c
libUTILdrvs_a_DEPENDENCIES	+= src/drivers/log.h src/include/sys/log.h src/include/sys/strlog.h src/include/sys/strutil/log.h src/include/sys/strutil/strlog.h
endif
if CONFIG_STREAMS_NSDEV_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/nsdev.c
libUTILdrvs_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_NULS_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/nuls.c
libUTILdrvs_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_PIPE_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/pipe.c
libUTILdrvs_a_DEPENDENCIES	+= src/drivers/pipe.h
endif
if CONFIG_STREAMS_SAD_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/sad.c
libUTILdrvs_a_DEPENDENCIES	+= src/include/sys/sad.h src/include/sys/strutil/sad.h
endif
if CONFIG_STREAMS_SFX_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/sfx.c
libUTILdrvs_a_DEPENDENCIES	+=
endif
if CONFIG_STREAMS_SPX_MODULE
libUTILdrvs_a_SOURCES		+= src/drivers/spx.c
libUTILdrvs_a_DEPENDENCIES	+=
endif

libUTILdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libUTILdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libUTILdrvs_a_OBJECTS):	$(libUTILdrvs_a_DEPENDENCIES)

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
KERNEL_OBJECTS			= $(libUTILlink_a_OBJECTS) $(libUTILmods_a_OBJECTS) $(libUTILdrvs_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libUTILmods_a_OBJECTS) $(libUTILdrvs_a_OBJECTS) $(ADDON_MODS)
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
if CONFIG_STREAMS_CLONE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif
if CONFIG_STREAMS_CLONE_MODULE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif

EXPORT_VERFILES			=
if CONFIG_STREAMS_CLONE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif
if CONFIG_STREAMS_CLONE_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif

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
				  src/util/init/strace.sh \
				  src/util/init/strerr.sh \
				  src/util/init/strlogd.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/strace \
				  src/util/init/strerr \
				  src/util/init/strlogd

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				  strsetup.conf \
				  strload.conf

dist_config_DATA		= \
				  src/util/sysconfig/strace \
				  src/util/sysconfig/strerr \
				  src/util/sysconfig/strlogd

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
