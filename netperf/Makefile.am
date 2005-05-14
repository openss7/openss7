# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.29 $) $Date: 2005/05/14 08:30:02 $
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
# Last Modified $Date: 2005/05/14 08:30:02 $ by $Author: brian $
#
# =============================================================================

#
#   General rules
#
include am/openss7.am

dist_bin_SCRIPTS		= \
				  netperf_arr \
				  netperf_packet_byte \
				  netperf_sctp_range \
				  netperf_sctp_rr \
				  netperf_sctp_stream \
				  netperf_snapshot \
				  netperf_tcp_range \
				  netperf_tcp_rr \
				  netperf_tcp_stream \
				  netperf_udp_range \
				  netperf_udp_rr \
				  netperf_udp_stream

dist_sbin_SCRIPTS		= \
				  sctp_stream_script \
				  tcp_stream_script \
				  udp_stream_script \
				  sctp_rr_script \
				  tcp_rr_script \
				  udp_rr_script \
				  sctp_range_script \
				  tcp_range_script \
				  snapshot_script \
				  arr_script \
				  packet_byte_script

bin_PROGRAMS			= \
				  netperf

sbin_PROGRAMS			= \
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
netperf_LDADD			= libnetperf.la $(NETPERF_LDADD)
netperf_DEPENDENCIES		= libnetperf.la ./config.h


netserver_SOURCES		= netserver.c
netserver_LDADD			= libnetperf.la $(NETPERF_LDADD)
netserver_DEPENDENCIES		= libnetperf.la ./config.h

dist_noinst_SCRIPTS		+= configure.nexusware

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += $(dist_sbin_SCRIPTS)

man1_MANS			= \
				  man1/netperf.1 \
				  man1/netperf_arr.1 \
				  man1/netperf_packet_byte.1 \
				  man1/netperf_sctp_range.1 \
				  man1/netperf_sctp_rr.1 \
				  man1/netperf_sctp_stream.1 \
				  man1/netperf_snapshot.1 \
				  man1/netperf_tcp_range.1 \
				  man1/netperf_tcp_rr.1 \
				  man1/netperf_tcp_stream.1 \
				  man1/netperf_udp_range.1 \
				  man1/netperf_udp_rr.1 \
				  man1/netperf_udp_stream.1

man8_MANS			= \
				  man8/sctp_range_script.8 \
				  man8/sctp_rr_script.8 \
				  man8/sctp_stream_script.8 \
				  man8/tcp_range_script.8 \
				  man8/tcp_rr_script.8 \
				  man8/tcp_stream_script.8 \
				  man8/udp_rr_script.8 \
				  man8/udp_stream_script.8 \
				  man8/snapshot_script.8 \
				  man8/arr_script.8 \
				  man8/packet_byte_script.8 \
				  man8/netserver.8

dist_noinst_MANS		= \
				  man1/netperf.man \
				  man1/netperf_arr.man \
				  man1/netperf_packet_byte.man \
				  man1/netperf_sctp_range.man \
				  man1/netperf_sctp_rr.man \
				  man1/netperf_sctp_stream.man \
				  man1/netperf_snapshot.man \
				  man1/netperf_tcp_range.man \
				  man1/netperf_tcp_rr.man \
				  man1/netperf_tcp_stream.man \
				  man1/netperf_udp_range.man \
				  man1/netperf_udp_rr.man \
				  man1/netperf_udp_stream.man \
				  man8/sctp_range_script.man \
				  man8/sctp_rr_script.man \
				  man8/sctp_stream_script.man \
				  man8/tcp_range_script.man \
				  man8/tcp_rr_script.man \
				  man8/tcp_stream_script.man \
				  man8/udp_rr_script.man \
				  man8/udp_stream_script.man \
				  man8/snapshot_script.man \
				  man8/arr_script.man \
				  man8/packet_byte_script.man \
				  man8/netserver.man

MANMACRO_PFX			= netperf

info_TEXINFOS			= netperf.texi
EXTRA_info_TEXINFOS		= \
				  scripts/texi/gpl.texi \
				  scripts/texi/fdl.texi \
				  scripts/texi/lesser.texi \
				  scripts/texi/install.texi

netperf_hp.ps:
	cp -f $(srcdir)/netperf.ps $@

CLEANFILES += netperf_hp.ps

EXTRA_DIST			+= ACKNWLDGMNTS COPYRIGHT README.ovms Release_Notes netperf_hp.ps

include am/info.am

include am/man.am

#
# Rules for lsms
#
include am/lsm.am

#
# Rules for archives
#
include am/archive.am

#
#   Rules for making rpms
#
AM_RPMFLAGS			=
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

#
# Rules for making deb packages
#
include am/deb.am

EXTRA_DIST		+= \
			debian/README.Debian \
			debian/TODO.Debian

# vim: ft=automake
