# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2006/12/29 05:51:44 $
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
# Last Modified $Date: 2006/12/29 05:51:44 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/iso-ip.4 \
	man4/iso-udp.4 \
	man4/isos.4 \
	man4/xol.4 \
	man5/striso.5

man_MANS = \
	man4/clns.4 \
	man4/cmot.4 \
	man4/iso-ip.4 \
	man4/isos.4 \
	man4/isot.4 \
	man4/iso-lan.4 \
	man4/iso-udp.4 \
	man4/itos.4 \
	man4/itot.4 \
	man4/lapb.4 \
	man4/lpp.4 \
	man4/ns-tcp.4 \
	man4/plp.4 \
	man4/tcpns.4 \
	man4/tp0.4 \
	man4/tp1.4 \
	man4/tp2.4 \
	man4/tp3.4 \
	man4/tp4.4 \
	man4/tp.4 \
	man4/x25-lapb.4 \
	man4/x25-plp.4 \
	man4/x25_lapb.4 \
	man4/x25_plp.4 \
	man4/xol.4 \
	man4/xot.4 \
	man5/striso.5 \
	man7/npi_osi.7 \
	man8/striso_mknod.8

dist_noinst_MANS = \
	man4/clns.4.man \
	man4/cmot.4.man \
	man4/iso-ip.4.man \
	man4/isos.4.man \
	man4/isot.4.man \
	man4/iso-lan.4.man \
	man4/iso-udp.4.man \
	man4/itos.4.man \
	man4/itot.4.man \
	man4/lapb.4.man \
	man4/lpp.4.man \
	man4/ns-tcp.4.man \
	man4/plp.4.man \
	man4/tcpns.4.man \
	man4/tp0.4.man \
	man4/tp1.4.man \
	man4/tp2.4.man \
	man4/tp3.4.man \
	man4/tp4.4.man \
	man4/tp.4.man \
	man4/x25-lapb.4.man \
	man4/x25-plp.4.man \
	man4/x25_lapb.4.man \
	man4/x25_plp.4.man \
	man4/xol.4.man \
	man4/xot.4.man \
	man5/striso.5.man \
	man7/npi_osi.7.man \
	man8/striso_mknod.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
# $Log: Makefile.am,v $
# Revision 0.9.2.5  2006/12/29 05:51:44  brian
# - changes for successful master build
#
# Revision 0.9.2.4  2006/10/12 20:25:14  brian
# - manual page fixups for release
#
# Revision 0.9.2.3  2006/10/03 13:52:16  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.2  2006/07/11 12:32:03  brian
# - added ISO and other implementations to distribution
#
# Revision 0.9.2.1  2006/04/08 03:39:14  brian
# - working up striso package
#
# 
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor nocin nosi
