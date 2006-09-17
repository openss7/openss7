# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# -----------------------------------------------------------------------------
#
# $Log$
# =============================================================================

##
##  General rules.
##
include ../am/openss7.am

lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

if WITH_TLI
## WITH_TLI

pkglibexec32_PROGRAMS		+= test-timod

test_timod_SOURCES		= $(top_srcdir)/src/test/tli/test-timod.c
test_timod_LDADD		= $(STREAMS_LDADD32) libxnet.la
test_timod_LDFLAGS		= $(MACHINE_FLAGS)
test_timod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_timod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-tirdwr

test_tirdwr_SOURCES		= $(top_srcdir)/src/test/tli/test-tirdwr.c
test_tirdwr_LDADD		= $(STREAMS_LDADD32) libxnet.la
test_tirdwr_LDFLAGS		= $(MACHINE_FLAGS)
test_tirdwr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tirdwr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## WITH_TLI
endif

if WITH_INET
## WITH_INET

pkglibexec32_PROGRAMS		+= test-inet_raw

test_inet_raw_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_raw.c
test_inet_raw_LDADD		= $(STREAMS_LDADD32)
test_inet_raw_LDFLAGS		= $(MACHINE_FLAGS)
test_inet_raw_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_raw_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-inet_tcp

test_inet_tcp_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_tcp.c
test_inet_tcp_LDADD		= $(STREAMS_LDADD32)
test_inet_tcp_LDFLAGS		= $(MACHINE_FLAGS)
test_inet_tcp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-inet_udp

test_inet_udp_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_udp.c
test_inet_udp_LDADD		= $(STREAMS_LDADD32)
test_inet_udp_LDFLAGS		= $(MACHINE_FLAGS)
test_inet_udp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_inet_udp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## WITH_INET
endif

if WITH_XNET
## WITH_XNET

lib32_LTLIBRARIES		+= libxnet.la

libxnet_la_SOURCES		= $(top_srcdir)/src/lib/libxnet/xnet.c \
				  $(top_srcdir)/gettext.h
libxnet_la_LDFLAGS		= -no-undefined -version-info 0:0:0 $(MACHINE_FLAGS)
libxnet_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(srcdir)/src/include/xti -I./src/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnet_la_LIBADD		= $(STREAMS_LDADD32)

pkglibexec32_PROGRAMS		+= test-xnet

test_xnet_SOURCES		= $(top_srcdir)/src/test/xnet/test-xnet.c
test_xnet_LDADD			= $(STREAMS_LDADD32) libxnet.la
test_xnet_LDFLAGS		= $(MACHINE_FLAGS)
test_xnet_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnet_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-xnet_thr

test_xnet_thr_SOURCES		= $(top_srcdir)/src/test/xnet/test-xnet.c
test_xnet_thr_LDADD		= $(STREAMS_LDADD32) -lpthread libxnet.la
test_xnet_thr_LDFLAGS		= $(MACHINE_FLAGS)
test_xnet_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnet_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## WITH_XNET
endif

pkglibexec32_PROGRAMS		+= test-m2pa

test_m2pa_SOURCES		= $(top_srcdir)/src/test/m2pa/test-m2pa.c
test_m2pa_LDADD			= $(STREAMS_LDADD32)
test_m2pa_LDFLAGS		= $(MACHINE_FLAGS)
test_m2pa_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_m2pa_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-q781

test_q781_SOURCES		= $(top_srcdir)/src/test/sl/test-q781.c
test_q781_LDADD			= $(STREAMS_LDADD32)
test_q781_LDFLAGS		= $(MACHINE_FLAGS)
test_q781_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_q781_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-tcpc

test_tcpc_SOURCES		= $(top_srcdir)/src/test/inet/test-tcpc.c
test_tcpc_LDADD			= $(STREAMS_LDADD32)
test_tcpc_LDFLAGS		= $(MACHINE_FLAGS)
test_tcpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcpc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-tcps

test_tcps_SOURCES		= $(top_srcdir)/src/test/inet/test-tcps.c
test_tcps_LDADD			= $(STREAMS_LDADD32)
test_tcps_LDFLAGS		= $(MACHINE_FLAGS)
test_tcps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tcps_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-udpc

test_udpc_SOURCES		= $(top_srcdir)/src/test/inet/test-udpc.c
test_udpc_LDADD			= $(STREAMS_LDADD32)
test_udpc_LDFLAGS		= $(MACHINE_FLAGS)
test_udpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_udpc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-udps

test_udps_SOURCES		= $(top_srcdir)/src/test/inet/test-udps.c
test_udps_LDADD			= $(STREAMS_LDADD32)
test_udps_LDFLAGS		= $(MACHINE_FLAGS)
test_udps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_udps_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x100p

test_x100p_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x100p.c
test_x100p_LDADD		= $(STREAMS_LDADD32)
test_x100p_LDFLAGS		= $(MACHINE_FLAGS)
test_x100p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x100p_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p

test_x400p_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p.c
test_x400p_LDADD		= $(STREAMS_LDADD32)
test_x400p_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if PUBLIC_RELEASE
## PUBLIC_RELEASE

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

pkglibexec32_PROGRAMS		+= test-q781-pipe

test_q781_pipe_SOURCES		= $(top_srcdir)/src/test/sl/test-q781-pipe.c
test_q781_pipe_LDADD		= $(STREAMS_LDADD32)
test_q781_pipe_LDFLAGS		= $(MACHINE_FLAGS)
test_q781_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_q781_pipe_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-q784

test_q784_SOURCES		= $(top_srcdir)/src/test/isup/test-q784.c
test_q784_LDADD			= $(STREAMS_LDADD32)
test_q784_LDFLAGS		= $(MACHINE_FLAGS)
test_q784_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_q784_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-cap2

test_x400p_cap2_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-cap2.c
test_x400p_cap2_LDADD		= $(STREAMS_LDADD32)
test_x400p_cap2_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_cap2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-cap

test_x400p_cap_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-cap.c
test_x400p_cap_LDADD		= $(STREAMS_LDADD32)
test_x400p_cap_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_cap_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_cap_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-hsl

test_x400p_hsl_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-hsl.c
test_x400p_hsl_LDADD		= $(STREAMS_LDADD32)
test_x400p_hsl_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_hsl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-perf

test_x400p_perf_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-perf.c
test_x400p_perf_LDADD		= $(STREAMS_LDADD32)
test_x400p_perf_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_perf_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_perf_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sdl

test_x400p_sdl_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sdl.c
test_x400p_sdl_LDADD		= $(STREAMS_LDADD32)
test_x400p_sdl_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_sdl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sdt2

test_x400p_sdt2_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sdt2.c
test_x400p_sdt2_LDADD		= $(STREAMS_LDADD32)
test_x400p_sdt2_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_sdt2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sdt

test_x400p_sdt_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sdt.c
test_x400p_sdt_LDADD		= $(STREAMS_LDADD32)
test_x400p_sdt_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_sdt_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sl

test_x400p_sl_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sl.c
test_x400p_sl_LDADD		= $(STREAMS_LDADD32)
test_x400p_sl_LDFLAGS		= $(MACHINE_FLAGS)
test_x400p_sl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## ! PUBLIC_RELEASE
endif

#perftest_SOURCES		= $(top_srcdir)/src/test/perftest.c
#perftest_LDADD			= $(STREAMS_LDADD32)
#perftest_LDFLAGS		= $(MACHINE_FLAGS)
#perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#perftest_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
