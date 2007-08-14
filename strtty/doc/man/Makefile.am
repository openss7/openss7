# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/14 03:04:23 $
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
# Last Modified $Date: 2007/08/14 03:04:23 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

man_MANS = \
	man3/grantpt.3tty \
	man3/ptsname.3tty \
	man3/unlockpt.3tty \
	man4/ldterm.4 \
	man4/pckt.4 \
	man4/ptc.4 \
	man4/ptem.4 \
	man4/ptm.4 \
	man4/ptmx.4tty \
	man4/pts.4tty \
	man4/pty.4 \
	man4/ttcompat.4 \
	man5/strtty.5 \
	man8/strtty_mknod.8 \
	man8/test-tty.8

dist_noinst_MANS = \
	man3/grantpt.3tty.man \
	man3/ptsname.3tty.man \
	man3/unlockpt.3tty.man \
	man4/ldterm.4.man \
	man4/pckt.4.man \
	man4/ptc.4.man \
	man4/ptem.4.man \
	man4/ptm.4.man \
	man4/ptmx.4tty.man \
	man4/pts.4tty.man \
	man4/pty.4.man \
	man4/ttcompat.4.man \
	man5/strtty.5.man \
	man8/strtty_mknod.8.man \
	man8/test-tty.8.man

MANS_UNUSED = \
	man3/grantpt.3tty \
	man3/ptsname.3tty \
	man3/unlockpt.3tty \
	man4/ptc.4 \
	man4/ptm.4 \
	man4/ptmx.4tty \
	man4/pts.4tty \
	man5/strtty.5

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.5  2007/08/14 03:04:23  brian
# - GPLv3 header update
#
# Revision 0.9.2.4  2007/03/13 23:54:32  brian
# - changes to pass check maintenance target
#
# Revision 0.9.2.3  2006/10/03 13:53:22  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.2  2006/09/29 11:51:00  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.1  2006/08/23 09:53:13  brian
# - started STREAMS Terminals package
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
