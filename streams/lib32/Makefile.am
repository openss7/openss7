# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2008-04-28 12:54:03 $
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
# Last Modified $Date: 2008-04-28 12:54:03 $ by $Author: brian $
#
# =============================================================================

##
## General rules
##
include ../am/openss7.am

AUTOMAKE_OPTIONS += nostdinc
AM_CPPFLAGS = -I. -I$(top_builddir) -I$(srcdir)

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

MACHINE_FLAGS			= -m32

pkglibexec32_PROGRAMS		+= testlog

testlog_SOURCES			= $(top_srcdir)/src/test/testlog.c
testlog_LDFLAGS			= $(MACHINE_FLAGS)
testlog_LDADD			= libstreams.la
testlog_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
testlog_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_STH
pkglibexec32_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_STH_MODULE
pkglibexec32_PROGRAMS		+= test-streams
endif

test_streams_SOURCES		= $(top_srcdir)/src/test/test-streams.c
test_streams_LDFLAGS		= $(MACHINE_FLAGS)
test_streams_LDADD		= libstreams.la
test_streams_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_streams_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_CLONE
pkglibexec32_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CLONE_MODULE
pkglibexec32_PROGRAMS		+= test-clone
endif

test_clone_SOURCES		= $(top_srcdir)/src/test/test-clone.c
test_clone_LDFLAGS		= $(MACHINE_FLAGS)
test_clone_LDADD		= libstreams.la
test_clone_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_clone_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_CONNLD
pkglibexec32_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_CONNLD_MODULE
pkglibexec32_PROGRAMS		+= test-connld
endif

test_connld_SOURCES		= $(top_srcdir)/src/test/test-connld.c
test_connld_LDFLAGS		= $(MACHINE_FLAGS)
test_connld_LDADD		= libstreams.la
test_connld_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_connld_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_ECHO
pkglibexec32_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_ECHO_MODULE
pkglibexec32_PROGRAMS		+= test-echo
endif

test_echo_SOURCES		= $(top_srcdir)/src/test/test-echo.c
test_echo_LDFLAGS		= $(MACHINE_FLAGS)
test_echo_LDADD			= libstreams.la
test_echo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_echo_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_FIFO
pkglibexec32_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_FIFO_MODULE
pkglibexec32_PROGRAMS		+= test-fifo
endif

test_fifo_SOURCES		= $(top_srcdir)/src/test/test-fifo.c
test_fifo_LDFLAGS		= $(MACHINE_FLAGS)
test_fifo_LDADD			= libstreams.la
test_fifo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_fifo_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_LOG
pkglibexec32_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOG_MODULE
pkglibexec32_PROGRAMS		+= test-log
endif

test_log_SOURCES		= $(top_srcdir)/src/test/test-log.c
test_log_LDFLAGS		= $(MACHINE_FLAGS)
test_log_LDADD			= libstreams.la
test_log_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_log_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_LOOP
pkglibexec32_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_LOOP_MODULE
pkglibexec32_PROGRAMS		+= test-loop
endif

test_loop_SOURCES		= $(top_srcdir)/src/test/test-loop.c
test_loop_LDFLAGS		= $(MACHINE_FLAGS)
test_loop_LDADD			= libstreams.la
test_loop_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_loop_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_NSDEV
pkglibexec32_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_NSDEV_MODULE
pkglibexec32_PROGRAMS		+= test-nsdev
endif

test_nsdev_SOURCES		= $(top_srcdir)/src/test/test-nsdev.c
test_nsdev_LDFLAGS		= $(MACHINE_FLAGS)
test_nsdev_LDADD		= libstreams.la
test_nsdev_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nsdev_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_MUX
pkglibexec32_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_MUX_MODULE
pkglibexec32_PROGRAMS		+= test-mux
endif

test_mux_SOURCES		= $(top_srcdir)/src/test/test-mux.c
test_mux_LDFLAGS		= $(MACHINE_FLAGS)
test_mux_LDADD			= libstreams.la
test_mux_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_mux_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_NULS
pkglibexec32_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_NULS_MODULE
pkglibexec32_PROGRAMS		+= test-nuls
endif

test_nuls_SOURCES		= $(top_srcdir)/src/test/test-nuls.c
test_nuls_LDFLAGS		= $(MACHINE_FLAGS)
test_nuls_LDADD			= libstreams.la
test_nuls_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_nuls_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_PIPE
pkglibexec32_PROGRAMS		+= test-pipe
endif
if CONFIG_STREAMS_PIPE_MODULE
pkglibexec32_PROGRAMS		+= test-pipe
endif

test_pipe_SOURCES		= $(top_srcdir)/src/test/test-pipe.c
test_pipe_LDFLAGS		= $(MACHINE_FLAGS)
test_pipe_LDADD			= libstreams.la
test_pipe_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipe_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_PIPEMOD
pkglibexec32_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
pkglibexec32_PROGRAMS		+= test-pipemod
endif

test_pipemod_SOURCES		= $(top_srcdir)/src/test/test-pipemod.c
test_pipemod_LDFLAGS		= $(MACHINE_FLAGS)
test_pipemod_LDADD		= libstreams.la
test_pipemod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_pipemod_CFLAGS		= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_SAD
pkglibexec32_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SAD_MODULE
pkglibexec32_PROGRAMS		+= test-sad
endif

test_sad_SOURCES		= $(top_srcdir)/src/test/test-sad.c
test_sad_LDFLAGS		= $(MACHINE_FLAGS)
test_sad_LDADD			= libstreams.la
test_sad_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sad_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_SC
pkglibexec32_PROGRAMS		+= test-sc
endif
if CONFIG_STREAMS_SC_MODULE
pkglibexec32_PROGRAMS		+= test-sc
endif

test_sc_SOURCES			= $(top_srcdir)/src/test/test-sc.c
test_sc_LDFLAGS			= $(MACHINE_FLAGS)
test_sc_LDADD			= libstreams.la
test_sc_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sc_CFLAGS			= $(LFS_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libstreams.la

libstreams_la_SOURCES		= $(top_srcdir)/lib/libc/testcancel.c \
				  $(top_srcdir)/lib/libc/putpmsg.c \
				  $(top_srcdir)/lib/libc/putmsg.c \
				  $(top_srcdir)/lib/libc/getpmsg.c \
				  $(top_srcdir)/lib/libc/getmsg.c \
				  $(top_srcdir)/lib/libc/isastream.c \
				  $(top_srcdir)/lib/libc/pipe.c \
				  $(top_srcdir)/lib/libc/fattach.c \
				  $(top_srcdir)/lib/libc/fdetach.c \
				  $(top_srcdir)/lib/libc/strlog.c \
				  $(top_srcdir)/lib/libc/streams.h
libstreams_la_DEPENDENCIES	= $(top_srcdir)/lib/libc/streams.VERSION
libstreams_la_LDFLAGS		= -Wl,--version-script -Wl,$(lib_abs_srcdir)/lib/libc/streams.VERSION \
				  $(MACHINE_FLAGS)
libstreams_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libstreams_la_LDFLAGS		+= -version-info 0:1:0 -export-dynamic

lib32_LTLIBRARIES		+= libLiS.la

libLiS_la_SOURCES		= $(top_srcdir)/lib/libc/testcancel.c \
				  $(top_srcdir)/lib/libc/putpmsg.c \
				  $(top_srcdir)/lib/libc/putmsg.c \
				  $(top_srcdir)/lib/libc/getpmsg.c \
				  $(top_srcdir)/lib/libc/getmsg.c \
				  $(top_srcdir)/lib/libc/isastream.c \
				  $(top_srcdir)/lib/libc/pipe.c \
				  $(top_srcdir)/lib/libc/fattach.c \
				  $(top_srcdir)/lib/libc/fdetach.c \
				  $(top_srcdir)/lib/libc/strlog.c \
				  $(top_srcdir)/lib/libc/streams.h
libLiS_la_DEPENDENCIES		= $(top_srcdir)/lib/libc/streams.VERSION
libLiS_la_LDFLAGS		= -Wl,--version-script -Wl,$(lib_abs_srcdir)/lib/libc/streams.VERSION \
				  $(MACHINE_FLAGS)
libLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libLiS_la_LDFLAGS		+= -version-info 0:1:0 -export-dynamic

lib32_LTLIBRARIES		+= libpLiS.la

libpLiS_la_SOURCES		= $(top_srcdir)/lib/libc/testcancel.c \
				  $(top_srcdir)/lib/libc/putpmsg.c \
				  $(top_srcdir)/lib/libc/putmsg.c \
				  $(top_srcdir)/lib/libc/getpmsg.c \
				  $(top_srcdir)/lib/libc/getmsg.c \
				  $(top_srcdir)/lib/libc/isastream.c \
				  $(top_srcdir)/lib/libc/fattach.c \
				  $(top_srcdir)/lib/libc/fdetach.c \
				  $(top_srcdir)/lib/libc/strlog.c \
				  $(top_srcdir)/lib/libc/streams.h
libpLiS_la_DEPENDENCIES		= $(top_srcdir)/lib/libc/streams.VERSION
libpLiS_la_LDFLAGS		= -Wl,--version-script -Wl,$(lib_abs_srcdir)/lib/libc/streams.VERSION \
				  $(MACHINE_FLAGS)
libpLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libpLiS_la_LDFLAGS		+= -version-info 0:1:0 -export-dynamic

#if WITH_SNMP_AGENT
#snmpdlmod32_LTLIBRARIES	+= strMIB.la
#endif
EXTRA_LTLIBRARIES		= strMIB.la

strMIB_la_SOURCES		= $(top_srcdir)/src/snmp/strMIB.c \
				  $(top_srcdir)/src/snmp/strMIB.h
strMIB_la_DEPENDENCIES		= libstreams.la
strMIB_la_LDFLAGS		= -module -avoid-version $(MACHINE_FLAGS) \
				  -shared $(STREAMS_LDFLAGS) $(SNMP_LDFLAGS)
strMIB_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
strMIB_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
strMIB_la_LIBADD		= $(STREAMS_LDADD) $(SNMP_LDADD)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
## Rules for building libraries
##
include ../am/libraries.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.10  2008-04-28 12:54:03  brian
# - update file headers for release
#
# Revision 0.9.2.9  2008/01/13 21:53:03  brian
# - dlmod SNMP agent build and installation
#
# Revision 0.9.2.8  2007/08/13 22:46:12  brian
# - GPLv3 header updates
#
# Revision 0.9.2.7  2007/03/28 13:44:16  brian
# - updates to syncrhonization, release notes and documentation
#
# Revision 0.9.2.6  2007/03/13 06:17:15  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.5  2006/11/26 15:27:36  brian
# - testing and corrections to strlog capabilities
#
# Revision 0.9.2.4  2006/09/29 11:50:25  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.3  2006/09/25 12:14:19  brian
# - added -static back for distcheck
#
# Revision 0.9.2.2  2006/09/24 21:36:51  brian
# - added lib files
#
# Revision 0.9.2.1  2006/09/17 23:09:09  brian
# - added lib32 compatibility directories
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
