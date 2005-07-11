# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.6.8 $) $Date: 2005/07/11 12:24:21 $
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
# Last Modified $Date: 2005/07/11 12:24:21 $ by $Author: brian $
#
# =============================================================================

include ../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS = \
	stropts.h \
	sys/cmn_err.h \
	sys/cred.h \
	sys/ddi.h \
	sys/debug.h \
	sys/dki.h \
	sys/gnu.stropts.h \
	sys/kmem.h \
	sys/LiS/buffcall.h \
	sys/LiS/config.h \
	sys/LiS/errmsg.h \
	sys/LiS/events.h \
	sys/LiS/fifo.h \
	sys/LiS/head.h \
	sys/LiS/linuxio.h \
	sys/LiS/linux-mdep.h \
	sys/lislocks.h \
	sys/LiS/log.h \
	sys/LiS/loop.h \
	sys/lismem.h \
	sys/LiS/minimux.h \
	sys/LiS/mod.h \
	sys/LiS/modcnt.h \
	sys/LiS/module.h \
	sys/LiS/msg.h \
	sys/LiS/msgutl.h \
	sys/LiS/mtdrv.h \
	sys/LiS/nullstrm.h \
	sys/lispci.h \
	sys/LiS/poll.h \
	sys/LiS/queue.h \
	sys/LiS/share.h \
	sys/LiS/stats.h \
	sys/LiS/strlog.h \
	sys/LiS/strmdbg.h \
	sys/LiS/wait.h \
	sys/major.h \
	sys/mkdev.h \
	sys/osif.h \
	sys/poll.h \
	sys/sad.h \
	sys/strconfig.h \
	sys/stream.h \
	sys/strmsg.h \
	sys/stropts.h \
	sys/strport.h

nobase_nodist_pkginclude_HEADERS = \
	sys/LiS/genconf.h \
	sys/LiS/version.h

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

EXTRA_DIST += $(nobase_pkginclude_HEADERS)

# vim: ft=automake
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
