# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/20 20:35:45 $
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
# Last Modified $Date: 2004/06/20 20:35:45 $ by $Author: brian $
#
# =============================================================================

sctp_headers	= \
	ss7/sctp.h \
	ss7/sctpi.h \
	ss7/sctpi_ioctl.h

xns_headers	= \
	sys/npi_sctp.h

tli_headers	= \
	sys/tpi_sctp.h

inet_headers	=

xnet_headers	= \
	sys/xti_sctp.h

sock_headers	= 

nobase_pkginclude_HEADERS = $(sctp_headers)

if WITH_XNS
nobase_pkginclude_HEADERS += $(xns_headers)
endif
if WITH_TLI
nobase_pkginclude_HEADERS += $(tli_headers)
endif
if WITH_INET
nobase_pkginclude_HEADERS += $(inet_headers)
endif
if WITH_XNET
nobase_pkginclude_HEADERS += $(xnet_headers)
endif
if WITH_SOCK
nobase_pkginclude_HEADERS += $(sock_headers)
endif

nobase_nodist_pkginclude_HEADERS = 
#	sys/strxnet/modversions.h

# vim: ft=automake
