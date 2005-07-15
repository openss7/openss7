# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/07/14 22:04:14 $
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
# Last Modified $Date: 2005/07/14 22:04:14 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/INET.4

man_MANS = \
	man3/xti_ip.3 \
	man3/xti_udp.3 \
	man3/xti_tcp.3 \
	man3/t_kpalive.3 \
	man3/t_tcp_info.3 \
	man4/arp.4 \
	man4/egp.4 \
	man4/ggp.4 \
	man4/icmp.4 \
	man4/idp.4 \
	man4/inet.4 \
	man4/INET.4 \
	man4/ip.4 \
	man4/ipip.4 \
	man4/pup.4 \
	man4/rawip.4 \
	man4/rip.4 \
	man4/tcp.4 \
	man4/ticlts.4 \
	man4/ticots.4 \
	man4/ticots_ord.4 \
	man4/ticotsord.4 \
	man4/udp.4 \
	man7/tpi_ip.7 \
	man7/tpi_udp.7 \
	man7/tpi_tcp.7 \
	man8/strinet_mknod.8 \
	man8/test-inet_raw.8 \
	man8/test-inet_tcp.8 \
	man8/test-inet_udp.8

if WITH_OPENSS7_SCTP
## WITH_OPENSS7_SCTP
man_MANS += \
	man3/xti_sctp.3 \
	man3/t_sctp_dest_status.3 \
	man3/t_sctp_dest_status_t.3 \
	man3/t_sctp_hb.3 \
	man3/t_sctp_hb_t.3 \
	man3/t_sctp_rto.3 \
	man3/t_sctp_rto_t.3 \
	man3/t_sctp_status.3 \
	man3/t_sctp_status_t.3 \
	man4/sctp.4 \
	man7/tpi_sctp.7 \
	man8/test-inet_sctp.8
## WITH_OPENSS7_SCTP
endif

dist_noinst_MANS = \
	man3/xti_ip.3.man \
	man3/xti_udp.3.man \
	man3/xti_tcp.3.man \
	man3/xti_sctp.3.man \
	man3/t_kpalive.3.man \
	man3/t_sctp_dest_status.3.man \
	man3/t_sctp_dest_status_t.3.man \
	man3/t_sctp_hb.3.man \
	man3/t_sctp_hb_t.3.man \
	man3/t_sctp_rto.3.man \
	man3/t_sctp_rto_t.3.man \
	man3/t_sctp_status.3.man \
	man3/t_sctp_status_t.3.man \
	man3/t_tcp_info.3.man \
	man4/arp.4.man \
	man4/egp.4.man \
	man4/ggp.4.man \
	man4/icmp.4.man \
	man4/idp.4.man \
	man4/inet.4.man \
	man4/INET.4.man \
	man4/ip.4.man \
	man4/ipip.4.man \
	man4/pup.4.man \
	man4/rawip.4.man \
	man4/rip.4.man \
	man4/sctp.4.man \
	man4/tcp.4.man \
	man4/ticlts.4.man \
	man4/ticots.4.man \
	man4/ticots_ord.4.man \
	man4/ticotsord.4.man \
	man4/udp.4.man \
	man7/tpi_ip.7.man \
	man7/tpi_udp.7.man \
	man7/tpi_tcp.7.man \
	man7/tpi_sctp.7.man \
	man8/strinet_mknod.8.man \
	man8/test-inet_raw.8.man \
	man8/test-inet_sctp.8.man \
	man8/test-inet_tcp.8.man \
	man8/test-inet_udp.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# vim: ft=automake
