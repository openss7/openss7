# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2004/05/22 01:31:57 $
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
# Last Modified $Date: 2004/05/22 01:31:57 $ by $Author: brian $
#
# =============================================================================

lfs_headers = \
	log.h \
	sad.h \
	strlog.h \
	stropts.h \
	sys/aixddi.h \
	sys/cmn_err.h \
	sys/ddi.h \
	sys/debug.h \
	sys/dki.h \
	sys/hpuxddi.h \
	sys/kmem.h \
	sys/lisddi.h \
	sys/log.h \
	sys/osfddi.h \
	sys/sad.h \
	sys/sc.h \
	sys/spec_fs_i.h \
	sys/strconf.h \
	sys/strdebug.h \
	sys/stream.h \
	sys/strlog.h \
	sys/stropts.h \
	sys/strsubr.h \
	sys/sunddi.h \
	sys/svr4ddi.h \
	sys/uw7ddi.h

xns_headers = \
	sys/cdi.h \
	sys/dlpi.h \
	sys/npi.h

tli_headers = \
	sys/ticlts.h \
	sys/ticots.h \
	sys/ticotsord.h \
	sys/tihdr.h \
	sys/timod.h \
	sys/tirdwr.h \
	sys/tiuser.h \
	sys/tli.h \
	sys/tpi.h \
	ticlts.h \
	ticots.h \
	ticotsord.h \
	tihdr.h \
	timod.h \
	tirdwr.h \
	tiuser.h

inet_headers =

xnet_headers = \
	sys/xti_atm.h \
	sys/xti.h \
	sys/xti_inet.h \
	sys/xti_ip.h \
	sys/xti_local.h \
	sys/xti_mosi.h \
	sys/xti_osi.h \
	sys/xti_sctp.h \
	sys/xti_tcp.h \
	sys/xti_udp.h \
	sys/xti_xti.h \
	xti.h \
	xti_inet.h

sock_headers = \
	sys/sockmod.h

nobase_pkginclude_HEADERS = $(lfs_headers)

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


nobase_nodist_pkginclude_HEADERS = \
	sys/streams/modversions.h

if KERNEL_VERSIONS
nobase_nodist_pkginclude_HEADERS += \
	sys/streams/versions/aixcompat.ver \
	sys/streams/versions/hpuxcompat.ver \
	sys/streams/versions/liscompat.ver \
	sys/streams/versions/osfcompat.ver \
	sys/streams/versions/suncompat.ver \
	sys/streams/versions/svr4compat.ver \
	sys/streams/versions/uw7compat.ver \
	sys/streams/versions/strattach.ver \
	sys/streams/versions/strreg.ver \
	sys/streams/versions/strsad.ver \
	sys/streams/versions/strsched.ver \
	sys/streams/versions/strspecfs.ver \
	sys/streams/versions/strsysctl.ver \
	sys/streams/versions/strutil.ver \
	sys/streams/versions/strsyms.ver \
	sys/streams/versions/sth.ver
endif

# vim: ft=automake
