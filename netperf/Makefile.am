# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2004/08/05 10:50:05 $
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
# Last Modified $Date: 2004/08/05 10:50:05 $ by $Author: brian $
#
# =============================================================================

sbin_SCRIPTS			= \
				  tcp_stream_script \
				  udp_stream_script \
				  tcp_rr_script \
				  udp_rr_script \
				  tcp_range_script \
				  snapshot_script \
				  arr_script \
				  packet_byte_script

sbin_PROGRAMS			= \
				  netperf \
				  netserver

noinst_LTLIBRARIES		= \
				  libnetperf.la

libnetperf_la_SOURCES		= \
				  netsh.c \
				  netlib.c \
				  nettest_bsd.c \
				  nettest_dlpi.c \
				  nettest_unix.c \
				  nettest_xti.c \
				  nettest_ipv6.c \
				  nettest_dns.c \
				  netlib.h \
				  netsh.h \
				  nettest_bsd.h \
				  nettest_dlpi.h \
				  nettest_unix.h \
				  nettest_xti.h \
				  nettest_ipv6.h \
				  nettest_dns.h \
				  hist.h \
				  ./config.h
libnetperf_la_DEPENDENCIES	= $(libnetperf_la_SOURCES)
$(libnetperf_la_OBJECTS):	./config.h

netperf_SOURCES			= netperf.c
netperf_LDFLAGS			= libnetperf.la $(NETPERF_LDADD)
netperf_DEPENDENCIES		= libnetperf.la ./config.h


netserver_SOURCES		= netserver.c
netserver_LDFLAGS		= libnetperf.la $(NETPERF_LDADD)
netserver_DEPENDENCIES		= libnetperf.la ./config.h

EXTRA_DIST			=


#
#   Rules for making rpms
#
AM_RPMFLAGS				=
AM_SRPMFLAGS				=

include am/rpm.am

#
#   General rules
#

include am/openss7.am


# vim: ft=automake
