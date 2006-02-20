# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.35 $) $Date: 2006/02/20 09:36:14 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
# Last Modified $Date: 2006/02/20 09:36:14 $ by $Author: brian $
#
# =============================================================================

include ../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS = \
	strlog.h \
	stropts.h \
	sys/cmn_err.h \
	sys/ddi.h \
	sys/debug.h \
	sys/dki.h \
	sys/kmem.h \
	sys/strconf.h \
	sys/strdebug.h \
	sys/stream.h \
	sys/strlog.h \
	sys/stropts.h \
	sys/stropts32.h \
	sys/strsubr.h \
	sys/streams/cmn_err.h \
	sys/streams/ddi.h \
	sys/streams/debug.h \
	sys/streams/dki.h \
	sys/streams/kmem.h \
	sys/streams/strconf.h \
	sys/streams/strdebug.h \
	sys/streams/stream.h \
	sys/streams/strlog.h \
	sys/streams/stropts.h \
	sys/streams/stropts32.h \
	sys/streams/strsubr.h

if CONFIG_STREAMS_LOG
nobase_pkginclude_HEADERS += log.h
nobase_pkginclude_HEADERS += sys/log.h
nobase_pkginclude_HEADERS += sys/streams/log.h
endif
if CONFIG_STREAMS_LOG_MODULE
nobase_pkginclude_HEADERS += log.h
nobase_pkginclude_HEADERS += sys/log.h
nobase_pkginclude_HEADERS += sys/streams/log.h
endif
if CONFIG_STREAMS_LOOP
nobase_pkginclude_HEADERS += loop.h
nobase_pkginclude_HEADERS += sys/loop.h
nobase_pkginclude_HEADERS += sys/streams/loop.h
endif
if CONFIG_STREAMS_LOOP_MODULE
nobase_pkginclude_HEADERS += loop.h
nobase_pkginclude_HEADERS += sys/loop.h
nobase_pkginclude_HEADERS += sys/streams/loop.h
endif
if CONFIG_STREAMS_SAD
nobase_pkginclude_HEADERS += sad.h
nobase_pkginclude_HEADERS += sys/sad.h
nobase_pkginclude_HEADERS += sys/streams/sad.h
endif
if CONFIG_STREAMS_SAD_MODULE
nobase_pkginclude_HEADERS += sad.h
nobase_pkginclude_HEADERS += sys/sad.h
nobase_pkginclude_HEADERS += sys/streams/sad.h
endif
if CONFIG_STREAMS_SC
nobase_pkginclude_HEADERS += sys/sc.h
nobase_pkginclude_HEADERS += sys/streams/sc.h
endif
if CONFIG_STREAMS_SC_MODULE
nobase_pkginclude_HEADERS += sys/sc.h
nobase_pkginclude_HEADERS += sys/streams/sc.h
endif
if CONFIG_STREAMS_TESTMOD
nobase_pkginclude_HEADERS += sys/testmod.h
nobase_pkginclude_HEADERS += sys/streams/testmod.h
endif
if CONFIG_STREAMS_TESTMOD_MODULE
nobase_pkginclude_HEADERS += sys/testmod.h
nobase_pkginclude_HEADERS += sys/streams/testmod.h
endif

##nobase_pkginclude_HEADERS += \
##	sys/spec_fs_i.h \

nobase_nodist_pkginclude_HEADERS = \
	sys/streams/config.h \
	sys/streams/version.h

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

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
