# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/11/08 06:55:51 $
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
# Last Modified $Date: 2004/11/08 06:55:51 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX = strsctp

man_MANS = \
	man7/npi_sctp.7 \
	man7/strsctp_soon.7 \
	man8/sctp.8

if WITH_OPENSS7_SCTP
else
man_MANS += \
	man3/xti_sctp.3 \
	man4/sctp.4 \
	man4/sctp_ioctl.4 \
	man4/strsctp_soon.4 \
	man7/tpi_sctp.7
endif

dist_noinst_MANS = \
	man3/xti_sctp.3.man \
	man4/sctp.4.man \
	man4/sctp_ioctl.4.man \
	man4/strsctp_soon.4.man \
	man7/npi_sctp.7.man \
	man7/tpi_sctp.7.man \
	man7/strsctp_soon.7.man \
	man8/sctp.8.man

EXTRA_DIST = 

include ../../am/man.am

# vim: ft=automake
