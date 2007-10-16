# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.49 $) $Date: 2007/10/16 07:46:33 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/10/16 07:46:33 $ by $Author: brian $
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


if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

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
libnetperf_la_LDFLAGS		= -static
libnetperf_la_DEPENDENCIES	= $(libnetperf_la_SOURCES)
$(libnetperf_la_OBJECTS):	./config.h

netperf_SOURCES			= netperf.c
netperf_LDADD			= libnetperf.la $(NETPERF_LDADD)
netperf_LDFLAGS			= -static
netperf_DEPENDENCIES		= libnetperf.la ./config.h


netserver_SOURCES		= netserver.c
netserver_LDADD			= libnetperf.la $(NETPERF_LDADD)
netserver_LDFLAGS		= -static
netserver_DEPENDENCIES		= libnetperf.la ./config.h

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+= configure.nexusware

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += $(dist_sbin_SCRIPTS)

MANMACRO_PFX			= netperf

SUBDIRS =

if DEVELOPMENT
## DEVELOPMENT

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

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

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

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

include am/man.am

if PKG_BUILD_INDEP
SUBDIRS += manual
endif

netperf_hp.ps:
	cp -f $(srcdir)/netperf.ps $@

CLEANFILES += netperf_hp.ps

EXTRA_DIST			+= ACKNWLDGMNTS COPYRIGHT README.ovms Release_Notes netperf_hp.ps

EXTRA_TEX			+= \
				scripts/texinfo.cnf \
				scripts/texi/args.texi.in \
				scripts/texi/fdl.texi \
				scripts/texi/gpl.texi \
				scripts/texi/install.texi \
				scripts/texi/lesser.texi \
				scripts/texi/testsuite.texi

EXTRA_DIST			+= $(EXTRA_TEX)

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
AM_RPMFLAGS			=
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Rules for making release files
##
MANUAL_FILE			= manual/$(PACKAGE).txt

include am/release.am

EXTRA_DIST			+= \
				  LICENSE \
				  debian/README.Debian \
				  debian/TODO.Debian

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.49  2007/10/16 07:46:33  brian
# - makefile corrections and additions
#
# Revision 0.9.2.48  2007/08/14 07:26:43  brian
# - GPLv3 header update
#
# Revision 0.9.2.47  2007/03/04 00:23:22  brian
# - add version file creation
#
# Revision 0.9.2.46  2006/12/29 05:50:47  brian
# - changes for successful master build
#
# Revision 0.9.2.45  2006/10/01 21:30:14  brian
# - distribute release and library version files
#
# Revision 0.9.2.44  2006/09/18 01:15:27  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.43  2006/08/26 09:16:13  brian
# - better release file generation
#
# Revision 0.9.2.42  2006/03/21 13:24:35  brian
# - included pr.am
#
# Revision 0.9.2.41  2006/03/20 11:53:56  brian
# - added checks for DEVELOPMENT
#
# Revision 0.9.2.40  2006/03/07 12:34:18  brian
# - tools must be built static for distcheck
#
# Revision 0.9.2.39  2006/03/06 11:37:56  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
