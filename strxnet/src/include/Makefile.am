# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/07/10 11:42:47 $
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
# Last Modified $Date: 2005/07/10 11:42:47 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

tli_headers	= \
	sys/tihdr.h \
	sys/timod.h \
	sys/tirdwr.h \
	sys/tiuser.h \
	sys/tli.h \
	sys/tpi.h \
	tihdr.h \
	timod.h \
	tirdwr.h \
	tiuser.h

unix_headers	= \
	sys/ticlts.h \
	sys/ticots.h \
	sys/ticotsord.h \
	ticlts.h \
	ticots.h \
	ticotsord.h

xnet_headers	= \
	sys/xti.h \
	sys/xti_local.h \
	sys/xti_xti.h \
	xti.h

inet_headers	= \
	sys/xti_inet.h \
	sys/xti_ip.h \
	sys/xti_udp.h \
	sys/xti_tcp.h \
	xti_inet.h

osi_headers	= \
	sys/xti_osi.h \
	sys/xti_mosi.h

atm_headers	= \
	sys/xti_atm.h

sctp_headers	= \
	sys/xti_sctp.h

sock_headers	= \
	sys/sockmod.h

nobase_pkginclude_HEADERS = 

if WITH_TLI
## WITH_TLI
nobase_pkginclude_HEADERS += $(tli_headers) $(unix_headers)
## WITH_TLI
endif
if WITH_XNET
## WITH_XNET
nobase_pkginclude_HEADERS += $(xnet_headers) $(inet_headers) $(sctp_headers) $(osi_headers) $(atm_headers)
## WITH_XNET
endif
if WITH_SOCK
## WITH_SOCK
nobase_pkginclude_HEADERS += $(sock_headers)
## WITH_SOCK
endif

dist_noinst_HEADERS = \
	$(tli_headers) \
	$(unix_headers) \
	$(xnet_headers) \
	$(inet_headers) \
	$(osi_headers) \
	$(atm_headers) \
	$(sctp_headers) \
	$(sock_headers)

nobase_nodist_pkginclude_HEADERS = \
	sys/strxnet/version.h
#	sys/strxnet/modversions.h

if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_headers
TESTS_ENVIRONMENT	+= PKG_HEADERS='$(nobase_pkginclude_HEADERS) $(nobase_nodist_pkginclude_HEADERS)' \
			   PKG_MANPATH='$(PKG_MANPATH)' \
			   mandir='$(mandir)'
CLEANFILES		+= $(top_builddir)/check_headers.log
DISTCLEANFILES		+= *.err *.out
## PERFORM_CHECKS
endif

## PKG_BUILD_ARCH
endif

# vim: ft=automake
