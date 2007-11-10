# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/11/10 19:40:52 $
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
# Last Modified $Date: 2007/11/10 19:40:52 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS = \
	ap_lib.h \
	ap_poll.h \
	omp_dmi.h \
	osi.h \
	osi_lib.h \
	ro_lib.h \
	xap_env.h \
	xap.h \
	xap_rose.h \
	xdsbdcp.h \
	xds.h \
	xdsmdup.h \
	xdssap.h \
	xmp_cmis.h \
	xmp.h \
	xmp_snmp.h \
	xmsga.h \
	xms.h \
	xmsima.h \
	xom.h \
	xomi.h \
	xti.h \
	xti_mosi.h \
	xti_osi.h \
	xti_xx25.h \
	sys/clns.h \
	sys/cons.h \
	sys/lapb_ioctl.h \
	sys/np.h \
	sys/osi.h \
	sys/tp.h \
	sys/xti_mosi.h \
	sys/xti_osi.h


nobase_nodist_pkginclude_HEADERS = \
	sys/$(PACKAGE)/config.h \
	sys/$(PACKAGE)/version.h
#	sys/$(PACKAGE)/modversions.h

if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_headers
TESTS_ENVIRONMENT	+= CTAGS='$(CTAGIDS)' \
			   PKG_HEADERS='$(nobase_pkginclude_HEADERS) $(nobase_nodist_pkginclude_HEADERS)' \
			   PKG_MANPATH='$(PKG_MANPATH)' \
			   mandir='$(mandir)'
CLEANFILES		+= $(top_builddir)/check_headers.log
DISTCLEANFILES		+= *.err *.out
## PERFORM_CHECKS
endif

## PKG_BUILD_ARCH
endif

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.5  2007/11/10 19:40:52  brian
# - documentation updates
#
# Revision 0.9.2.4  2007/08/14 07:05:06  brian
# - GNUv3 header update
#
# Revision 0.9.2.3  2006/10/03 13:52:25  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.2  2006/10/02 11:31:49  brian
# - changes to get master builds working for RPM and DEB
# - added outside licenses to package documentation
# - added LICENSE automated release file
# - copy MANUAL to source directory
# - add and remove devices in -dev debian subpackages
# - get debian rules working better
# - release library version files
# - added notes to debian changelog
# - corrections for cooked manual pages in spec files
# - added release documentation to spec and rules files
# - copyright header updates
# - moved controlling tty checks in stream head
# - missing some defines for LiS build in various source files
# - added OSI headers to striso package
# - added includes and manual page paths to acincludes for various packages
# - added sunrpc, uidlpi, uinpi and uitpi licenses to documentation and release
#   files
# - moved pragma weak statements ahead of declarations
# - changes for master build of RPMS and DEBS with LiS
#
# Revision 0.9.2.1  2006/04/07 22:01:33  brian
# - initial branch
#
# Revision 0.9  2006/04/07 21:56:44  brian
# - initial addition of striso files
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor nocin nosi
