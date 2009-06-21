# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:03:05 $
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
# Last Modified $Date: 2009-06-21 11:03:05 $ by $Author: brian $
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

## =====================================================================

UTIL_INCLUDES			= -I$(top_builddir)/src/util -I$(top_srcidr)/src/util $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

if WITH_SNMP_AGENT
pkglibexec32_PROGRAMS		+= test-usage
endif
EXTRA_PROGRAMS			+= test-usage

test_usage_SOURCES		= $(top_srcdir)/src/util/usage.c
test_usage_CPPFLAGS		= $(UTIL_INCLUDES)
test_usage_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_usage_LDADD		= libstreams.la

## =====================================================================

TEST_INCLUDES			= -I$(top_builddir)/src/test -I$(top_srcdir)/src/test $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

pkglibexec32_PROGRAMS		+= testlog

testlog_SOURCES			= $(top_srcdir)/src/test/testlog.c
testlog_CPPFLAGS		= $(TEST_INCLUDES)
testlog_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
testlog_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-streams

test_streams_SOURCES		= $(top_srcdir)/src/test/test-streams.c
test_streams_CPPFLAGS		= $(TEST_INCLUDES)
test_streams_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_streams_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-clone

test_clone_SOURCES		= $(top_srcdir)/src/test/test-clone.c
test_clone_CPPFLAGS		= $(TEST_INCLUDES)
test_clone_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_clone_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-connld

test_connld_SOURCES		= $(top_srcdir)/src/test/test-connld.c
test_connld_CPPFLAGS		= $(TEST_INCLUDES)
test_connld_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_connld_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-echo

test_echo_SOURCES		= $(top_srcdir)/src/test/test-echo.c
test_echo_CPPFLAGS		= $(TEST_INCLUDES)
test_echo_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_echo_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-fifo

test_fifo_SOURCES		= $(top_srcdir)/src/test/test-fifo.c
test_fifo_CPPFLAGS		= $(TEST_INCLUDES)
test_fifo_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_fifo_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-log

test_log_SOURCES		= $(top_srcdir)/src/test/test-log.c
test_log_CPPFLAGS		= $(TEST_INCLUDES)
test_log_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_log_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-loop

test_loop_SOURCES		= $(top_srcdir)/src/test/test-loop.c
test_loop_CPPFLAGS		= $(TEST_INCLUDES)
test_loop_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_loop_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-nsdev

test_nsdev_SOURCES		= $(top_srcdir)/src/test/test-nsdev.c
test_nsdev_CPPFLAGS		= $(TEST_INCLUDES)
test_nsdev_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_nsdev_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-mux

test_mux_SOURCES		= $(top_srcdir)/src/test/test-mux.c
test_mux_CPPFLAGS		= $(TEST_INCLUDES)
test_mux_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_mux_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-nuls

test_nuls_SOURCES		= $(top_srcdir)/src/test/test-nuls.c
test_nuls_CPPFLAGS		= $(TEST_INCLUDES)
test_nuls_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_nuls_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-pipe

test_pipe_SOURCES		= $(top_srcdir)/src/test/test-pipe.c
test_pipe_CPPFLAGS		= $(TEST_INCLUDES)
test_pipe_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_pipe_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-pipemod

test_pipemod_SOURCES		= $(top_srcdir)/src/test/test-pipemod.c
test_pipemod_CPPFLAGS		= $(TEST_INCLUDES)
test_pipemod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_pipemod_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sad

test_sad_SOURCES		= $(top_srcdir)/src/test/test-sad.c
test_sad_CPPFLAGS		= $(TEST_INCLUDES)
test_sad_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sad_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sc

test_sc_SOURCES			= $(top_srcdir)/src/test/test-sc.c
test_sc_CPPFLAGS		= $(TEST_INCLUDES)
test_sc_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sc_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-tty

test_tty_SOURCES		= $(top_srcdir)/src/test/test-tty.c
test_tty_CPPFLAGS		= $(TEST_INCLUDES)
test_tty_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_tty_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= ldltest

ldltest_SOURCES			= $(top_srcdir)/src/test/ldltest.c
ldltest_CPPFLAGS		= $(TEST_INCLUDES)
ldltest_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
ldltest_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-np_ip

test_np_ip_SOURCES		= $(top_srcdir)/src/test/test-np_ip.c
test_np_ip_CPPFLAGS		= $(TEST_INCLUDES)
test_np_ip_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_np_ip_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-ip

test_ip_SOURCES			= $(top_srcdir)/src/test/test-ip.c
test_ip_CPPFLAGS		= $(TEST_INCLUDES)
test_ip_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_ip_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-tirdwr

test_tirdwr_SOURCES		= $(top_srcdir)/src/test/test-tirdwr.c
test_tirdwr_CPPFLAGS		= $(TEST_INCLUDES)
test_tirdwr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_tirdwr_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-timod

test_timod_SOURCES		= $(top_srcdir)/src/test/test-timod.c
test_timod_CPPFLAGS		= $(TEST_INCLUDES)
test_timod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_timod_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-xnet

test_xnet_SOURCES		= $(top_srcdir)/src/test/test-xnet.c
test_xnet_CPPFLAGS		= $(TEST_INCLUDES)
test_xnet_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnet_LDADD			= libstreams.la libxnet.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-xnet_thr

test_xnet_thr_SOURCES		= $(top_srcdir)/src/test/test-xnet.c
test_xnet_thr_CPPFLAGS		= $(TEST_INCLUDES) -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnet_thr_LDADD		= libstreams.la libxnet.la
test_xnet_thr_LDFLAGS		= -lpthread

## =====================================================================

pkglibexec32_PROGRAMS		+= test-xnsl

test_xnsl_SOURCES		= $(top_srcdir)/src/test/test-xnsl.c
test_xnsl_CPPFLAGS		= $(TEST_INCLUDES)
test_xnsl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnsl_LDADD			= libstreams.la libxnet.la libxnsl.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-xnsl_thr

test_xnsl_thr_SOURCES		= $(top_srcdir)/src/test/test-xnsl.c
test_xnsl_thr_CPPFLAGS		= $(TEST_INCLUDES) -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_xnsl_thr_LDADD		= libstreams.la libxnet.la libxnsl.la
test_xnsl_thr_LDFLAGS		= -lpthread

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sockpath

test_sockpath_SOURCES		= $(top_srcdir)/src/test/test-sockpath.c
test_sockpath_CPPFLAGS		= $(TEST_INCLUDES)
test_sockpath_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sockpath_LDADD		= libstreams.la libsockpath.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-socket

test_socket_SOURCES		= $(top_srcdir)/src/test/test-socket.c
test_socket_CPPFLAGS		= $(TEST_INCLUDES)
test_socket_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_socket_LDADD		= libstreams.la libsocket.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-socklib

test_socklib_SOURCES		= $(top_srcdir)/src/test/test-socklib.c
test_socklib_CPPFLAGS		= $(TEST_INCLUDES)
test_socklib_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_socklib_LDADD		= libstreams.la libsocklib.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sockmod

test_sockmod_SOURCES		= $(top_srcdir)/src/test/test-sockmod.c
test_sockmod_CPPFLAGS		= $(TEST_INCLUDES)
test_sockmod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sockmod_LDADD		= libstreams.la libsocklib.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-socksys

test_socksys_SOURCES		= $(top_srcdir)/src/test/test-socksys.c
test_socksys_CPPFLAGS		= $(TEST_INCLUDES)
test_socksys_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_socksys_LDADD		= libstreams.la libsocket.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-inet_raw

test_inet_raw_SOURCES		= $(top_srcdir)/src/test/test-inet_raw.c
test_inet_raw_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_raw_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_raw_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-inet_udp

test_inet_udp_SOURCES		= $(top_srcdir)/src/test/test-inet_udp.c
test_inet_udp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_udp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_udp_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-inet_tcp

test_inet_tcp_SOURCES		= $(top_srcdir)/src/test/test-inet_tcp.c
test_inet_tcp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_tcp_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-inet_sctp

test_inet_sctp_SOURCES		= $(top_srcdir)/src/test/test-inet_sctp.c
test_inet_sctp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_sctp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_inet_sctp_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sctp

test_sctp_SOURCES		= $(top_srcdir)/src/test/test-sctp.c
test_sctp_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sctp_n

test_sctp_n_SOURCES		= $(top_srcdir)/src/test/test-sctp_n.c
test_sctp_n_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_n_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sctp_n2

test_sctp_n2_SOURCES		= $(top_srcdir)/src/test/test-sctp_n2.c
test_sctp_n2_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_n2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_n2_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sctp_nc

test_sctp_nc_SOURCES		= $(top_srcdir)/src/test/test-sctp_nc.c
test_sctp_nc_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_nc_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sctp_ns

test_sctp_ns_SOURCES		= $(top_srcdir)/src/test/test-sctp_ns.c
test_sctp_ns_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_ns_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sctp_t

test_sctp_t_SOURCES		= $(top_srcdir)/src/test/test-sctp_t.c
test_sctp_t_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sctp_t_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-etsi_n

test_etsi_n_SOURCES		= $(top_srcdir)/src/test/test-etsi_n.c
test_etsi_n_CPPFLAGS		= $(TEST_INCLUDES)
test_etsi_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_etsi_n_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-etsi_t 

test_etsi_t_SOURCES		= $(top_srcdir)/src/test/test-etsi_t.c
test_etsi_t_CPPFLAGS		= $(TEST_INCLUDES)
test_etsi_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_etsi_t_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-interop

test_interop_SOURCES		= $(top_srcdir)/src/test/test-interop.c
test_interop_CPPFLAGS		= $(TEST_INCLUDES)
test_interop_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_interop_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-chan

test_chan_SOURCES		= $(top_srcdir)/src/test/test-chan.c
test_chan_CPPFLAGS		= $(TEST_INCLUDES)
test_chan_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_chan_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-isdn

test_isdn_SOURCES		= $(top_srcdir)/src/test/test-isdn.c
test_isdn_CPPFLAGS		= $(TEST_INCLUDES)
test_isdn_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_isdn_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-atm

test_atm_SOURCES		= $(top_srcdir)/src/test/test-atm.c
test_atm_CPPFLAGS		= $(TEST_INCLUDES)
test_atm_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_atm_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p

test_x400p_SOURCES		= $(top_srcdir)/src/test/test-x400p.c
test_x400p_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-q781-pipe

test_q781_pipe_SOURCES		= $(top_srcdir)/src/test/test-q781-pipe.c
test_q781_pipe_CPPFLAGS		= $(TEST_INCLUDES)
test_q781_pipe_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_q781_pipe_LDADD		= libstreams.la

## =====================================================================

EXTRA_PROGRAMS			+= test-q784
#pkglibexec32_PROGRAMS		+= test-q784

test_q784_SOURCES		= $(top_srcdir)/src/test/test-q784.c
test_q784_CPPFLAGS		= $(TEST_INCLUDES)
test_q784_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_q784_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-cap2

test_x400p_cap2_SOURCES		= $(top_srcdir)/src/test/test-x400p-cap2.c
test_x400p_cap2_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_cap2_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-cap

test_x400p_cap_SOURCES		= $(top_srcdir)/src/test/test-x400p-cap.c
test_x400p_cap_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_cap_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_cap_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-hsl

test_x400p_hsl_SOURCES		= $(top_srcdir)/src/test/test-x400p-hsl.c
test_x400p_hsl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_hsl_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-perf

test_x400p_perf_SOURCES		= $(top_srcdir)/src/test/test-x400p-perf.c
test_x400p_perf_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_perf_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_perf_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-sdl

test_x400p_sdl_SOURCES		= $(top_srcdir)/src/test/test-x400p-sdl.c
test_x400p_sdl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_sdl_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-sdt2

test_x400p_sdt2_SOURCES		= $(top_srcdir)/src/test/test-x400p-sdt2.c
test_x400p_sdt2_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_sdt2_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-sdt

test_x400p_sdt_SOURCES		= $(top_srcdir)/src/test/test-x400p-sdt.c
test_x400p_sdt_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_sdt_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-x400p-sl

test_x400p_sl_SOURCES		= $(top_srcdir)/src/test/test-x400p-sl.c
test_x400p_sl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_x400p_sl_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-sigtran

test_sigtran_SOURCES		= $(top_srcdir)/src/test/test-sigtran.c
test_sigtran_CPPFLAGS		= $(TEST_INCLUDES)
test_sigtran_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_sigtran_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-m2pa

test_m2pa_SOURCES		= $(top_srcdir)/src/test/test-m2pa.c
test_m2pa_CPPFLAGS		= $(TEST_INCLUDES)
test_m2pa_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_m2pa_LDADD			= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-m2ua_as

test_m2ua_as_SOURCES		= $(top_srcdir)/src/test/test-m2ua_as.c
test_m2ua_as_CPPFLAGS		= $(TEST_INCLUDES)
test_m2ua_as_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_m2ua_as_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-m3ua-raw

test_m3ua_raw_SOURCES		= $(top_srcdir)/src/test/test-m3ua-raw.c
test_m3ua_raw_CPPFLAGS		= $(TEST_INCLUDES)
test_m3ua_raw_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_m3ua_raw_LDADD		= libstreams.la

## =====================================================================

pkglibexec32_PROGRAMS		+= test-voip

test_voip_SOURCES		= $(top_srcdir)/src/test/test-voip.c
test_voip_CPPFLAGS		= $(TEST_INCLUDES)
test_voip_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
test_voip_LDADD			= libstreams.la

## =====================================================================

LIB_INCLUDES			= -I$(top_builddir)/src/lib -I$(top_srcdir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

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
libstreams_la_CPPFLAGS		= $(LIB_INCLUDES)
libstreams_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libstreams_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION

## =====================================================================

lib32_LTLIBRARIES		+= libcdiapi.la

libcdiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/cdiapi.c \
				  $(top_srcdir)/src/include/cdiapi.h \
				  $(top_srcdir)/gettext.h
libcdiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libcdiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libcdiapi_la_LIBADD		= libstreams.la
libcdiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/cdiapi.VERSION

## =====================================================================

lib32_LTLIBRARIES		+= libdlpiapi.la

libdlpiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/dlpiapi.c \
				  $(top_srcdir)/src/include/dlpiapi.h \
				  $(top_srcdir)/gettext.h
libdlpiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libdlpiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libdlpiapi_la_LIBADD		= libstreams.la
libdlpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpiapi.VERSION

## =====================================================================

lib32_LTLIBRARIES		+= libnpiapi.la

libnpiapi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/npiapi.c \
				  $(top_srcdir)/src/include/npiapi.h \
				  $(top_srcdir)/gettext.h
libnpiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libnpiapi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libnpiapi_la_LIBADD		= libstreams.la
libnpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/npiapi.VERSION

## =====================================================================

lib32_LTLIBRARIES		+= libdlpi.la

libdlpi_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/dlpi.c \
				  $(top_srcdir)/src/include/libdlpi.h \
				  $(top_srcdir)/gettext.h
libdlpi_la_CPPFLAGS		= $(LIB_INCLUDES)
libdlpi_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libdlpi_la_LIBADD		=  libstreams.la
libdlpi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpi.VERSION

## =====================================================================

lib32_LTLIBRARIES		+= libxnet.la

libxnet_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/xnet.c \
				  $(top_srcdir)/gettext.h
libxnet_la_CPPFLAGS		= $(LIB_INCLUDES)
libxnet_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnet_la_LIBADD		= libstreams.la
libxnet_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnet.VERSION

## =====================================================================

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
libxnsl_la_CPPFLAGS		= $(LIB_INCLUDES)
libxnsl_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnsl_la_LIBADD		= libstreams.la libxnet.la
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  -ldl

## =====================================================================

lib32_LTLIBRARIES		+= inetaddr.la

inetaddr_la_SOURCES		= $(top_srcdir)/src/lib/netdir_inet.c \
				  $(top_srcdir)/src/include/netdir.h \
				  $(top_srcdir)/src/include/netconfig.h \
				  $(top_srcdir)/src/include/sys/netconfig.h
inetaddr_la_CPPFLAGS		= $(LIB_INCLUDES) -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
inetaddr_la_LIBADD		= libstreams.la libxnet.la libxnsl.la
inetaddr_la_LDFLAGS		= -module -avoid-version

## =====================================================================

lib32_LTLIBRARIES		+= libsockpath.la

libsockpath_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/sockpath.c \
				  $(top_srcdir)/src/lib/sockpath.h \
				  $(top_srcdir)/src/include/sockdb.h \
				  $(top_srcdir)/src/include/sys/sockpath.h
libsockpath_la_CPPFLAGS		= $(LIB_INCLUDES)
libsockpath_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsockpath_la_LIBADD		=
libsockpath_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION

## =====================================================================

lib32_LTLIBRARIES		+= libsocket.la

libsocket_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/socket.c
libsocket_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  -static -lc -shared
libsocket_la_CPPFLAGS		= $(LIB_INCLUDES)
libsocket_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsocket_la_LIBADD		= libstreams.la

lib32_LTLIBRARIES		+= libsocklib.la

libsocklib_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/socklib.c
libsocklib_la_CPPFLAGS		= $(LIB_INCLUDES)
libsocklib_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsocklib_la_LIBADD		= libstreams.la libxnet.la libxnsl.la libsockpath.la
libsocklib_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  -static -lc -shared

## =====================================================================

lib32_LTLIBRARIES		+= libsx25.la

libsx25_la_SOURCES		= $(top_srcdir)/src/lib/padent.c \
				  $(top_srcdir)/src/lib/x25config.c \
				  $(top_srcdir)/src/lib/xhostent.c \
				  $(top_srcdir)/src/include/netx25/x25_control.h \
				  $(top_srcdir)/src/include/netx25/x25_proto.h \
				  $(top_srcdir)/src/include/netx25/xnetdb.h
libsx25_la_CPPFLAGS		= $(LIB_INCLUDES)
libsx25_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libsx25_la_LIBADD		= libstreams.la
libsx25_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sx25.VERSION

## =====================================================================

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
# Revision 1.1.2.1  2009-06-21 11:03:05  brian
# - added files to new distro
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
