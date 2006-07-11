# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/07/11 12:32:03 $
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
# Last Modified $Date: 2006/07/11 12:32:03 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/ISO.4

man_MANS = \
	man4/cmot.4 \
	man4/ISO.4 \
	man4/iso-ip.4 \
	man4/iso-lan.4 \
	man4/isos.4 \
	man4/isot.4 \
	man4/iso-udp.4 \
	man4/lpp.4 \
	man4/ns-tcp.4 \
	man4/tp0.4 \
	man4/tp1.4 \
	man4/tp2.4 \
	man4/tp3.4 \
	man4/tp4.4 \
	man4/tp.4 \
	man4/xol.4 \
	man4/xot.4 \
	man7/npi_osi.7

dist_noinst_MANS = \
	man4/cmot.4.man \
	man4/ISO.4.man \
	man4/iso-ip.4.man \
	man4/iso-lan.4.man \
	man4/isos.4.man \
	man4/isot.4.man \
	man4/iso-udp.4.man \
	man4/lpp.4.man \
	man4/ns-tcp.4.man \
	man4/tp0.4.man \
	man4/tp1.4.man \
	man4/tp2.4.man \
	man4/tp3.4.man \
	man4/tp4.4.man \
	man4/tp.4.man \
	man4/xol.4.man \
	man4/xot.4.man \
	man7/npi_osi.7.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
# $Log: Makefile.am,v $
# Revision 0.9.2.2  2006/07/11 12:32:03  brian
# - added ISO and other implementations to distribution
#
# Revision 0.9.2.1  2006/04/08 03:39:14  brian
# - working up striso package
#
# 
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor nocin nosi
