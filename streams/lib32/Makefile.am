# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/17 23:09:09 $
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
# Last Modified $Date: 2006/09/17 23:09:09 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.1  2006/09/17 23:09:09  brian
# - added lib32 compatibility directories
#
# =============================================================================

##
## General rules
##
include ../am/openss7.am

lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

MACHINE_FLAGS			= -m32

if CONFIG_STREAMS_STH
pkglibexec32_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_STH_MODULE
pkglibexec32_PROGRAMS		+= test-streams
endif

test_streams_SOURCES		= $(top_srcdir)/src/test/test-streams.c
test_streams_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_streams_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_CLONE
pkglibexec32_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CLONE_MODULE
pkglibexec32_PROGRAMS		+= test-clone
endif

test_clone_SOURCES		= $(top_srcdir)/src/test/test-clone.c
test_clone_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_clone_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_clone_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_CONNLD
pkglibexec32_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_CONNLD_MODULE
pkglibexec32_PROGRAMS		+= test-connld
endif

test_connld_SOURCES		= $(top_srcdir)/src/test/test-connld.c
test_connld_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_connld_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_connld_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_ECHO
pkglibexec32_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_ECHO_MODULE
pkglibexec32_PROGRAMS		+= test-echo
endif

test_echo_SOURCES		= $(top_srcdir)/src/test/test-echo.c
test_echo_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_echo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_echo_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_FIFO
pkglibexec32_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_FIFO_MODULE
pkglibexec32_PROGRAMS		+= test-fifo
endif

test_fifo_SOURCES		= $(top_srcdir)/src/test/test-fifo.c
test_fifo_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_fifo_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_LOG
pkglibexec32_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOG_MODULE
pkglibexec32_PROGRAMS		+= test-log
endif

test_log_SOURCES		= $(top_srcdir)/src/test/test-log.c
test_log_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_log_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_log_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_LOOP
pkglibexec32_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_LOOP_MODULE
pkglibexec32_PROGRAMS		+= test-loop
endif

test_loop_SOURCES		= $(top_srcdir)/src/test/test-loop.c
test_loop_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_loop_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_loop_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_NSDEV
pkglibexec32_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_NSDEV_MODULE
pkglibexec32_PROGRAMS		+= test-nsdev
endif

test_nsdev_SOURCES		= $(top_srcdir)/src/test/test-nsdev.c
test_nsdev_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_nsdev_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nsdev_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_MUX
pkglibexec32_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_MUX_MODULE
pkglibexec32_PROGRAMS		+= test-mux
endif

test_mux_SOURCES		= $(top_srcdir)/src/test/test-mux.c
test_mux_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_mux_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_mux_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_NULS
pkglibexec32_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_NULS_MODULE
pkglibexec32_PROGRAMS		+= test-nuls
endif

test_nuls_SOURCES		= $(top_srcdir)/src/test/test-nuls.c
test_nuls_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_nuls_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nuls_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_PIPE
pkglibexec32_PROGRAMS		+= test-pipe
endif
if CONFIG_STREAMS_PIPE_MODULE
pkglibexec32_PROGRAMS		+= test-pipe
endif

test_pipe_SOURCES		= $(top_srcdir)/src/test/test-pipe.c
test_pipe_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_PIPEMOD
pkglibexec32_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
pkglibexec32_PROGRAMS		+= test-pipemod
endif

test_pipemod_SOURCES		= $(top_srcdir)/src/test/test-pipemod.c
test_pipemod_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_pipemod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipemod_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_SAD
pkglibexec32_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SAD_MODULE
pkglibexec32_PROGRAMS		+= test-sad
endif

test_sad_SOURCES		= $(top_srcdir)/src/test/test-sad.c
test_sad_LDFLAGS		= libstreams.la $(MACHINE_FLAGS)
test_sad_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_SC
pkglibexec32_PROGRAMS		+= test-sc
endif
if CONFIG_STREAMS_SC_MODULE
pkglibexec32_PROGRAMS		+= test-sc
endif

test_sc_SOURCES			= $(top_srcdir)/src/test/test-sc.c
test_sc_LDFLAGS			= libstreams.la $(MACHINE_FLAGS)
test_sc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sc_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libstreams.la

libstreams_la_SOURCES		= $(top_srcdir)/lib/libc/testcancel.c \
				  $(top_srcdir)/lib/libc/putpmsg.c \
				  $(top_srcdir)/lib/libc/getpmsg.c \
				  $(top_srcdir)/lib/libc/isastream.c \
				  $(top_srcdir)/lib/libc/pipe.c \
				  $(top_srcdir)/lib/libc/fattach.c \
				  $(top_srcdir)/lib/libc/fdetach.c
libstreams_la_DEPENDENCIES	= $(top_srcdir)/lib/libc/streams.VERSION
libstreams_la_LDFLAGS		= -no-undefined -Wl,--version-script -Wl,$(top_srcdir)/lib/libc/streams.VERSION $(MACHINE_FLAGS)
libstreams_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libstreams_la_LDFLAGS		+= -version-info 0:1:0

lib32_LTLIBRARIES		+= libLiS.la

libLiS_la_SOURCES		= $(top_srcdir)/lib/libc/testcancel.c \
				  $(top_srcdir)/lib/libc/putpmsg.c \
				  $(top_srcdir)/lib/libc/getpmsg.c \
				  $(top_srcdir)/lib/libc/isastream.c \
				  $(top_srcdir)/lib/libc/pipe.c \
				  $(top_srcdir)/lib/libc/fattach.c \
				  $(top_srcdir)/lib/libc/fdetach.c
libLiS_la_DEPENDENCIES		= $(top_srcdir)/lib/libc/streams.VERSION
libLiS_la_LDFLAGS		= -no-undefined -Wl,--version-script -Wl,$(top_srcdir)/lib/libc/streams.VERSION $(MACHINE_FLAGS)
libLiS_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libLiS_la_LDFLAGS		+= -version-info 0:1:0

lib32_LTLIBRARIES		+= libpLiS.la

libpLiS_la_SOURCES		= $(top_srcdir)/lib/libc/testcancel.c \
				  $(top_srcdir)/lib/libc/putpmsg.c \
				  $(top_srcdir)/lib/libc/getpmsg.c \
				  $(top_srcdir)/lib/libc/isastream.c \
				  $(top_srcdir)/lib/libc/fattach.c \
				  $(top_srcdir)/lib/libc/fdetach.c
libpLiS_la_DEPENDENCIES		= $(top_srcdir)/lib/libc/streams.VERSION
libpLiS_la_LDFLAGS		= -no-undefined -Wl,--version-script -Wl,$(top_srcdir)/lib/libc/streams.VERSION $(MACHINE_FLAGS)
libpLiS_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libpLiS_la_LDFLAGS		+= -version-info 0:1:0

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
## Rules for building libraries
##
include ../am/libraries.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
