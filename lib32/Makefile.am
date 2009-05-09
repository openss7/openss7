# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
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
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>, or
# write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.
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
# Last Modified $Date$ by $Author$
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
LIB_INCLUDES			= -I$(top_builddir)/src/lib -I$(top_srcdir)/src/lib $(PKG_INCLUDES)

pkglibexec32_PROGRAMS		+= testlog

testlog_SOURCES			= $(top_srcdir)/src/test/testlog.c
testlog_LDFLAGS			=
testlog_LDADD			= libstreams.la
testlog_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"testlog"'
testlog_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_STH
pkglibexec32_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_STH_MODULE
pkglibexec32_PROGRAMS		+= test-streams
endif

test_streams_SOURCES		= $(top_srcdir)/src/test/test-streams.c
test_streams_LDFLAGS		=
test_streams_LDADD		= libstreams.la
test_streams_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-streams"'
test_streams_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_CLONE
pkglibexec32_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CLONE_MODULE
pkglibexec32_PROGRAMS		+= test-clone
endif

test_clone_SOURCES		= $(top_srcdir)/src/test/test-clone.c
test_clone_LDFLAGS		=
test_clone_LDADD		= libstreams.la
test_clone_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-clone"'
test_clone_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_CONNLD
pkglibexec32_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_CONNLD_MODULE
pkglibexec32_PROGRAMS		+= test-connld
endif

test_connld_SOURCES		= $(top_srcdir)/src/test/test-connld.c
test_connld_LDFLAGS		=
test_connld_LDADD		= libstreams.la
test_connld_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-connld"'
test_connld_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_ECHO
pkglibexec32_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_ECHO_MODULE
pkglibexec32_PROGRAMS		+= test-echo
endif

test_echo_SOURCES		= $(top_srcdir)/src/test/test-echo.c
test_echo_LDFLAGS		=
test_echo_LDADD			= libstreams.la
test_echo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-echo"'
test_echo_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_FIFO
pkglibexec32_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_FIFO_MODULE
pkglibexec32_PROGRAMS		+= test-fifo
endif

test_fifo_SOURCES		= $(top_srcdir)/src/test/test-fifo.c
test_fifo_LDFLAGS		=
test_fifo_LDADD			= libstreams.la
test_fifo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-fifo"'
test_fifo_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_LOG
pkglibexec32_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOG_MODULE
pkglibexec32_PROGRAMS		+= test-log
endif

test_log_SOURCES		= $(top_srcdir)/src/test/test-log.c
test_log_LDFLAGS		=
test_log_LDADD			= libstreams.la
test_log_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-log"'
test_log_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_LOOP
pkglibexec32_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_LOOP_MODULE
pkglibexec32_PROGRAMS		+= test-loop
endif

test_loop_SOURCES		= $(top_srcdir)/src/test/test-loop.c
test_loop_LDFLAGS		=
test_loop_LDADD			= libstreams.la
test_loop_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-loop"'
test_loop_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_NSDEV
pkglibexec32_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_NSDEV_MODULE
pkglibexec32_PROGRAMS		+= test-nsdev
endif

test_nsdev_SOURCES		= $(top_srcdir)/src/test/test-nsdev.c
test_nsdev_LDFLAGS		=
test_nsdev_LDADD		= libstreams.la
test_nsdev_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-nsdev"'
test_nsdev_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_MUX
pkglibexec32_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_MUX_MODULE
pkglibexec32_PROGRAMS		+= test-mux
endif

test_mux_SOURCES		= $(top_srcdir)/src/test/test-mux.c
test_mux_LDFLAGS		=
test_mux_LDADD			= libstreams.la
test_mux_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-mux"'
test_mux_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_NULS
pkglibexec32_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_NULS_MODULE
pkglibexec32_PROGRAMS		+= test-nuls
endif

test_nuls_SOURCES		= $(top_srcdir)/src/test/test-nuls.c
test_nuls_LDFLAGS		=
test_nuls_LDADD			= libstreams.la
test_nuls_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-nuls"'
test_nuls_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_PIPE
pkglibexec32_PROGRAMS		+= test-pipe
endif
if CONFIG_STREAMS_PIPE_MODULE
pkglibexec32_PROGRAMS		+= test-pipe
endif

test_pipe_SOURCES		= $(top_srcdir)/src/test/test-pipe.c
test_pipe_LDFLAGS		=
test_pipe_LDADD			= libstreams.la
test_pipe_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-pipe"'
test_pipe_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_PIPEMOD
pkglibexec32_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
pkglibexec32_PROGRAMS		+= test-pipemod
endif

test_pipemod_SOURCES		= $(top_srcdir)/src/test/test-pipemod.c
test_pipemod_LDFLAGS		=
test_pipemod_LDADD		= libstreams.la
test_pipemod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-pipemod"'
test_pipemod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_SAD
pkglibexec32_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SAD_MODULE
pkglibexec32_PROGRAMS		+= test-sad
endif

test_sad_SOURCES		= $(top_srcdir)/src/test/test-sad.c
test_sad_LDFLAGS		=
test_sad_LDADD			= libstreams.la
test_sad_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sad"'
test_sad_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

if CONFIG_STREAMS_SC
pkglibexec32_PROGRAMS		+= test-sc
endif
if CONFIG_STREAMS_SC_MODULE
pkglibexec32_PROGRAMS		+= test-sc
endif

test_sc_SOURCES			= $(top_srcdir)/src/test/test-sc.c
test_sc_LDFLAGS			=
test_sc_LDADD			= libstreams.la
test_sc_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sc"'
test_sc_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-tty

test_tty_SOURCES		= $(top_srcdir)/src/test/test-tty.c
test_tty_LDFLAGS		=
test_tty_LDADD			= libstreams.la
test_tty_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-tty"'
test_tty_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
##test_tty_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= ldltest

ldltest_SOURCES			= $(top_srcdir)/src/test/ldltest.c
ldltest_LDFLAGS			=
ldltest_LDADD			= libstreams.la
ldltest_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"ldltest"'
ldltest_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-np_ip

test_np_ip_SOURCES		= $(top_srcdir)/src/test/test-np_ip.c
test_np_ip_LDFLAGS		=
test_np_ip_LDADD		= libstreams.la
test_np_ip_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-np_ip"'
test_np_ip_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-ip

test_ip_SOURCES			= $(top_srcdir)/src/test/test-ip.c
test_ip_LDFLAGS			=
test_ip_LDADD			= libstreams.la
test_ip_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-ip"'
test_ip_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-tirdwr

test_tirdwr_SOURCES		= $(top_srcdir)/src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		=
test_tirdwr_LDADD		= libstreams.la
test_tirdwr_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-tirdwr"'
test_tirdwr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_tirdwr_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-timod

test_timod_SOURCES		= $(top_srcdir)/src/test/test-timod.c
test_timod_LDFLAGS		=
test_timod_LDADD		= libstreams.la
test_timod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-timod"'
test_timod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_timod_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-xnet

test_xnet_SOURCES		= $(top_srcdir)/src/test/test-xnet.c
test_xnet_LDFLAGS		=
test_xnet_LDADD			= libstreams.la libxnet.la
test_xnet_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnet"'
test_xnet_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnet_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-xnet_thr

test_xnet_thr_SOURCES		= $(top_srcdir)/src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -lpthread
test_xnet_thr_LDADD		= libstreams.la libxnet.la
test_xnet_thr_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnet_thr"' -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnet_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-xnsl

test_xnsl_SOURCES		= $(top_srcdir)/src/test/test-xnsl.c
test_xnsl_LDFLAGS		=
test_xnsl_LDADD			= libstreams.la libxnet.la libxnsl.la
test_xnsl_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnsl"'
test_xnsl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnsl_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-xnsl_thr

test_xnsl_thr_SOURCES		= $(top_srcdir)/src/test/test-xnsl.c
test_xnsl_thr_LDFLAGS		= -lpthread
test_xnsl_thr_LDADD		= libstreams.la libxnet.la libxnsl.la
test_xnsl_thr_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnsl_thr"' -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnsl_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sockpath

test_sockpath_SOURCES		= $(top_srcdir)/src/test/test-sockpath.c
test_sockpath_LDADD		= libstreams.la libsockpath.la
test_sockpath_LDFLAGS		=
test_sockpath_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sockpath"'
test_sockpath_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sockpath_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-socket

test_socket_SOURCES		= $(top_srcdir)/src/test/test-socket.c
test_socket_LDADD		= libstreams.la libsocket.la
test_socket_LDFLAGS		=
test_socket_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-socket"'
test_socket_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_socket_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-socklib

test_socklib_SOURCES		= $(top_srcdir)/src/test/test-socklib.c
test_socklib_LDADD		= libstreams.la libsocklib.la
test_socklib_LDFLAGS		=
test_socklib_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-socklib"'
test_socklib_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_socklib_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sockmod

test_sockmod_SOURCES		= $(top_srcdir)/src/test/test-sockmod.c
test_sockmod_LDADD		= libstreams.la libsocklib.la
test_sockmod_LDFLAGS		=
test_sockmod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sockmod"'
test_sockmod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sockmod_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-socksys

test_socksys_SOURCES		= $(top_srcdir)/src/test/test-socksys.c
test_socksys_LDADD		= libstreams.la libsocket.la
test_socksys_LDFLAGS		=
test_socksys_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-socksys"'
test_socksys_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_socksys_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-inet_raw

test_inet_raw_SOURCES		= $(top_srcdir)/src/test/test-inet_raw.c
test_inet_raw_LDADD		= libstreams.la
test_inet_raw_LDFLAGS		=
test_inet_raw_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_raw"'
test_inet_raw_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_raw_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-inet_udp

test_inet_udp_SOURCES		= $(top_srcdir)/src/test/test-inet_udp.c
test_inet_udp_LDADD		= libstreams.la
test_inet_udp_LDFLAGS		=
test_inet_udp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_udp"'
test_inet_udp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_udp_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-inet_tcp

test_inet_tcp_SOURCES		= $(top_srcdir)/src/test/test-inet_tcp.c
test_inet_tcp_LDADD		= libstreams.la
test_inet_tcp_LDFLAGS		=
test_inet_tcp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_tcp"'
test_inet_tcp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_tcp_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-inet_sctp

test_inet_sctp_SOURCES		= $(top_srcdir)/src/test/test-inet_sctp.c
test_inet_sctp_LDADD		= libstreams.la
test_inet_sctp_LDFLAGS		=
test_inet_sctp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_sctp"'
test_inet_sctp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_sctp_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sctp

test_sctp_SOURCES		= $(top_srcdir)/src/test/test-sctp.c
test_sctp_LDADD			= libstreams.la
test_sctp_LDFLAGS		=
test_sctp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp"'
test_sctp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sctp_n

test_sctp_n_SOURCES		= $(top_srcdir)/src/test/test-sctp_n.c
test_sctp_n_LDADD		= libstreams.la
test_sctp_n_LDFLAGS		=
test_sctp_n_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_n"'
test_sctp_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_n_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sctp_n2

test_sctp_n2_SOURCES		= $(top_srcdir)/src/test/test-sctp_n2.c
test_sctp_n2_LDADD		= libstreams.la
test_sctp_n2_LDFLAGS		=
test_sctp_n2_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_n2"'
test_sctp_n2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_n2_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sctp_nc

test_sctp_nc_SOURCES		= $(top_srcdir)/src/test/test-sctp_nc.c
test_sctp_nc_LDADD		= libstreams.la
test_sctp_nc_LDFLAGS		=
test_sctp_nc_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_nc"'
test_sctp_nc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_nc_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sctp_ns

test_sctp_ns_SOURCES		= $(top_srcdir)/src/test/test-sctp_ns.c
test_sctp_ns_LDADD		= libstreams.la
test_sctp_ns_LDFLAGS		=
test_sctp_ns_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_ns"'
test_sctp_ns_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_ns_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-sctp_t

test_sctp_t_SOURCES		= $(top_srcdir)/src/test/test-sctp_t.c
test_sctp_t_LDADD		= libstreams.la
test_sctp_t_LDFLAGS		=
test_sctp_t_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_t"'
test_sctp_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_t_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-etsi_n

test_etsi_n_SOURCES		= $(top_srcdir)/src/test/test-etsi_n.c
test_etsi_n_LDADD		= libstreams.la
test_etsi_n_LDFLAGS		=
test_etsi_n_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-etsi_n"'
test_etsi_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_etsi_n_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-etsi_t 

test_etsi_t_SOURCES		= $(top_srcdir)/src/test/test-etsi_t.c
test_etsi_t_LDADD		= libstreams.la
test_etsi_t_LDFLAGS		=
test_etsi_t_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-etsi_t"'
test_etsi_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_etsi_t_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

pkglibexec32_PROGRAMS		+= test-interop

test_interop_SOURCES		= $(top_srcdir)/src/test/test-interop.c
test_interop_LDADD		= libstreams.la
test_interop_LDFLAGS		=
test_interop_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-interop"'
test_interop_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_interop_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)

lib32_LTLIBRARIES		+= libstreams.la

libstreams_la_SOURCES		= $(top_srcdir)/src/lib/testcancel.c \
				  $(top_srcdir)/src/lib/putpmsg.c \
				  $(top_srcdir)/src/lib/putmsg.c \
				  $(top_srcdir)/src/lib/getpmsg.c \
				  $(top_srcdir)/src/lib/getmsg.c \
				  $(top_srcdir)/src/lib/isastream.c \
				  $(top_srcdir)/src/lib/pipe.c \
				  $(top_srcdir)/src/lib/fattach.c \
				  $(top_srcdir)/src/lib/fdetach.c \
				  $(top_srcdir)/src/lib/strlog.c \
				  $(top_srcdir)/src/lib/streams.h
libstreams_la_DEPENDENCIES	= $(top_srcdir)/src/lib/streams.VERSION
libstreams_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libstreams_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libLiS.la

libLiS_la_SOURCES		= $(top_srcdir)/src/lib/testcancel.c \
				  $(top_srcdir)/src/lib/putpmsg.c \
				  $(top_srcdir)/src/lib/putmsg.c \
				  $(top_srcdir)/src/lib/getpmsg.c \
				  $(top_srcdir)/src/lib/getmsg.c \
				  $(top_srcdir)/src/lib/isastream.c \
				  $(top_srcdir)/src/lib/pipe.c \
				  $(top_srcdir)/src/lib/fattach.c \
				  $(top_srcdir)/src/lib/fdetach.c \
				  $(top_srcdir)/src/lib/strlog.c \
				  $(top_srcdir)/src/lib/streams.h
libLiS_la_DEPENDENCIES		= $(top_srcdir)/src/lib/streams.VERSION
libLiS_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libpLiS.la

libpLiS_la_SOURCES		= $(top_srcdir)/src/lib/testcancel.c \
				  $(top_srcdir)/src/lib/putpmsg.c \
				  $(top_srcdir)/src/lib/putmsg.c \
				  $(top_srcdir)/src/lib/getpmsg.c \
				  $(top_srcdir)/src/lib/getmsg.c \
				  $(top_srcdir)/src/lib/isastream.c \
				  $(top_srcdir)/src/lib/fattach.c \
				  $(top_srcdir)/src/lib/fdetach.c \
				  $(top_srcdir)/src/lib/strlog.c \
				  $(top_srcdir)/src/lib/streams.h
libpLiS_la_DEPENDENCIES		= $(top_srcdir)/src/lib/streams.VERSION
libpLiS_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libpLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libcdiapi.la

libcdiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/cdiapi.c \
				  $(top_srcdir)/src/include/cdiapi.h \
				  $(top_srcdir)/gettext.h
libcdiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/cdiapi.VERSION
libcdiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/cdiapi.VERSION
libcdiapi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libcdiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libcdiapi_la_LIBADD		= libstreams.la

lib32_LTLIBRARIES		+= libdlpiapi.la

libdlpiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/dlpiapi.c \
				  $(top_srcdir)/src/include/dlpiapi.h \
				  $(top_srcdir)/gettext.h
libdlpiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/dlpiapi.VERSION
libdlpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpiapi.VERSION
libdlpiapi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libdlpiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libdlpiapi_la_LIBADD		= libstreams.la

lib32_LTLIBRARIES		+= libnpiapi.la

libnpiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/npiapi.c \
				  $(top_srcdir)/src/include/npiapi.h \
				  $(top_srcdir)/gettext.h
libnpiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/npiapi.VERSION
libnpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/npiapi.VERSION
libnpiapi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libnpiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libnpiapi_la_LIBADD		= libstreams.la

lib32_LTLIBRARIES		+= libdlpi.la

libdlpi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/dlpi.c \
				  $(top_srcdir)/src/include/libdlpi.h \
				  $(top_srcdir)/gettext.h
libdlpi_la_DEPENDENCIES		= $(top_srcdir)/src/lib/dlpi.VERSION
libdlpi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpi.VERSION
libdlpi_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libdlpi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libdlpi_la_LIBADD		=  libstreams.la

lib32_LTLIBRARIES		+= libxnet.la

libxnet_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/xnet.c \
				  $(top_srcdir)/gettext.h
libxnet_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnet.VERSION
libxnet_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnet.VERSION
libxnet_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnet_la_LIBADD		= libstreams.la

lib32_LTLIBRARIES		+= libxnsl.la

libxnsl_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/netselect.c \
				  $(top_srcdir)/src/lib/netselect.h \
				  $(top_srcdir)/src/lib/netdir.c \
				  $(top_srcdir)/src/lib/netdir_inet.c \
				  $(top_srcdir)/src/include/netconfig.h \
				  $(top_srcdir)/src/include/netdir.h \
				  $(top_srcdir)/src/include/sys/netconfig.h \
				  $(top_srcdir)/gettext.h
libxnsl_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnsl.VERSION
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  -ldl
libxnsl_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libxnsl_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnsl_la_LIBADD		= libstreams.la libxnet.la

lib32_LTLIBRARIES		+= inetaddr.la

inetaddr_la_SOURCES		= $(top_srcdir)/src/lib/netdir_inet.c \
				  $(top_srcdir)/src/include/netdir.h \
				  $(top_srcdir)/src/include/netconfig.h \
				  $(top_srcdir)/src/include/sys/netconfig.h
inetaddr_la_DEPENDENCIES	= libxnsl.la
inetaddr_la_LDFLAGS		= -module -avoid-version
inetaddr_la_CPPFLAGS		= -I$(top_builddir) -I$(top_srcdir) -I$(top_builddir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
inetaddr_la_LIBADD		= libstreams.la libxnet.la libxnsl.la

lib32_LTLIBRARIES		+= libsockpath.la

libsockpath_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/sockpath.c \
				  $(top_srcdir)/src/lib/sockpath.h \
				  $(top_srcdir)/src/include/sockdb.h \
				  $(top_srcdir)/src/include/sys/sockpath.h
libsockpath_la_DEPENDENCIES	= $(top_srcdir)/src/lib/sockpath.VERSION
libsockpath_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION
libsockpath_la_CPPFLAGS		= $(LIB_INCLUDES) $(USER_CPPFLAGS)
libsockpath_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsockpath_la_LIBADD		=

lib32_LTLIBRARIES		+= libsocket.la

libsocket_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/socket.c
libsocket_la_DEPENDENCIES	= $(top_srcdir)/src/lib/socket.VERSION
libsocket_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  -static -lc -shared
libsocket_la_CPPFLAGS		= $(LIB_INCLUDES) $(USER_CPPFLAGS)
libsocket_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsocket_la_LIBADD		= libstreams.la

lib32_LTLIBRARIES		+= libsocklib.la

libsocklib_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/socklib.c
libsocklib_la_DEPENDENCIES	= $(top_srcdir)/src/lib/socklib.VERSION
libsocklib_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  -static -lc -shared
libsocklib_la_CPPFLAGS		= $(LIB_INCLUDES) $(USER_CPPFLAGS)
libsocklib_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsocklib_la_LIBADD		= libstreams.la libxnet.la libxnsl.la libsockpath.la

#if WITH_SNMP_AGENT
#snmpdlmod32_LTLIBRARIES	+= strMIB.la
#endif
EXTRA_LTLIBRARIES		= strMIB.la

strMIB_la_SOURCES		= $(top_srcdir)/src/snmp/strMIB.c \
				  $(top_srcdir)/src/snmp/strMIB.h \
				  $(top_srcdir)/src/snmp/strExtMIB.c \
				  $(top_srcdir)/src/snmp/strExtMIB.h
strMIB_la_DEPENDENCIES		= libstreams.la
strMIB_la_LDFLAGS		= -module -avoid-version
				  -shared $(STREAMS_LDFLAGS) $(SNMP_LDFLAGS)
strMIB_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
strMIB_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
strMIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)

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
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
