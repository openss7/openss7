# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/08/22 09:20:05 $
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
# Last Modified $Date: 2004/08/22 09:20:05 $ by $Author: brian $
#
# =============================================================================

nobase_pkginclude_HEADERS = \
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

nobase_nodist_pkginclude_HEADERS = \
	sys/@PACKAGE_NAME@/modversions.h

if KERNEL_VERSIONS
nobase_nodist_pkginclude_HEADERS += \
	sys/@PACKAGE_NAME@/versions/aixcompat.ver \
	sys/@PACKAGE_NAME@/versions/hpuxcompat.ver \
	sys/@PACKAGE_NAME@/versions/liscompat.ver \
	sys/@PACKAGE_NAME@/versions/osfcompat.ver \
	sys/@PACKAGE_NAME@/versions/suncompat.ver \
	sys/@PACKAGE_NAME@/versions/svr4compat.ver \
	sys/@PACKAGE_NAME@/versions/uw7compat.ver \
	sys/@PACKAGE_NAME@/versions/strattach.ver \
	sys/@PACKAGE_NAME@/versions/strreg.ver \
	sys/@PACKAGE_NAME@/versions/strsad.ver \
	sys/@PACKAGE_NAME@/versions/strsched.ver \
	sys/@PACKAGE_NAME@/versions/strspecfs.ver \
	sys/@PACKAGE_NAME@/versions/strsysctl.ver \
	sys/@PACKAGE_NAME@/versions/strutil.ver \
	sys/@PACKAGE_NAME@/versions/strsyms.ver \
	sys/@PACKAGE_NAME@/versions/sth.ver
endif

# vim: ft=automake
