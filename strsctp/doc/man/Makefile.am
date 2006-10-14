# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2006/10/14 06:21:34 $
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
# Last Modified $Date: 2006/10/14 06:21:34 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.16  2006/10/14 06:21:34  brian
# - updated manual pages for release
#
# Revision 0.9.2.15  2006/10/10 10:46:13  brian
# - updates for release
#
# Revision 0.9.2.14  2006/10/03 13:52:54  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.13  2006/05/07 07:24:56  brian
# - updates for testing
#
# Revision 0.9.2.12  2006/03/06 11:38:54  brian
# - updated headers
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX = strsctp

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

man_MANS = \
	man5/strsctp.5 \
	man7/npi_sctp.7 \
	man8/strsctp_mknod.8 \
	man8/test-interop.8 \
	man8/test-sctp.8 \
	man8/test-sctp_n.8 \
	man8/test-sctp_n2.8 \
	man8/test-sctp_nc.8 \
	man8/test-sctp_ns.8 \
	man8/test-sctp_t.8 \
	man8/test-etsi_n.8 \
	man8/test-etsi_t.8

if WITH_OPENSS7_SCTP
## WITH_OPENSS7_SCTP
## WITH_OPENSS7_SCTP
else
## ! WITH_OPENSS7_SCTP
man_MANS += \
	man3/xti_sctp.3 \
	man4/sctp.4 \
	man4/sctp_n.4 \
	man4/sctp_t.4 \
	man4/tpiperf.4 \
	man7/tpi_sctp.7
## ! WITH_OPENSS7_SCTP
endif

dist_noinst_MANS = \
	man3/xti_sctp.3.man \
	man4/sctp.4.man \
	man4/sctp_n.4.man \
	man4/sctp_t.4.man \
	man4/tpiperf.4.man \
	man5/strsctp.5.man \
	man7/npi_sctp.7.man \
	man7/tpi_sctp.7.man \
	man8/strsctp_mknod.8.man \
	man8/test-interop.8.man \
	man8/test-sctp.8.man \
	man8/test-sctp_n.8.man \
	man8/test-sctp_n2.8.man \
	man8/test-sctp_nc.8.man \
	man8/test-sctp_ns.8.man \
	man8/test-sctp_t.8.man \
	man8/test-etsi_n.8.man \
	man8/test-etsi_t.8.man
## PKG_BUILD_INDEP
endif

MANS_UNUSED = \
	man5/strsctp.5

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
