# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/03 13:51:58 $
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
# Last Modified $Date: 2006/10/03 13:51:58 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.12  2006/10/03 13:51:58  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.11  2006/03/06 11:38:31  brian
# - updated headers
#
# =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS 	= \
				  sys/ddi.h \
				  sys/strconf.h \
				  sys/stream.h \
				  sys/os7/allocb.h \
				  sys/os7/bufpool.h \
				  sys/os7/bufq.h \
				  sys/os7/compat.h \
				  sys/os7/debug.h \
				  sys/os7/lock.h \
				  sys/os7/priv.h \
				  sys/os7/queue.h \
				  sys/os7/timer.h

if CONFIG_STREAMS_COMPAT_AIX
nobase_pkginclude_HEADERS	+= sys/aix/ddi.h sys/aix/strconf.h sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX
nobase_pkginclude_HEADERS	+= sys/hpux/ddi.h sys/hpux/strconf.h sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX
nobase_pkginclude_HEADERS	+= sys/irix/ddi.h sys/irix/strconf.h sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_LFS
nobase_pkginclude_HEADERS	+= sys/lfs/ddi.h sys/lfs/strconf.h sys/lfs/stream.h sys/strlog.h
endif
if CONFIG_STREAMS_COMPAT_LIS
nobase_pkginclude_HEADERS	+= sys/lis/ddi.h sys/lis/strconf.h sys/lis/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC
nobase_pkginclude_HEADERS	+= sys/mac/ddi.h sys/mac/strconf.h sys/mac/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS
nobase_pkginclude_HEADERS	+= sys/mps/ddi.h sys/mps/strconf.h sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_OS7
nobase_pkginclude_HEADERS	+= sys/os7/ddi.h sys/os7/strconf.h sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF
nobase_pkginclude_HEADERS	+= sys/osf/ddi.h sys/osf/strconf.h sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN
nobase_pkginclude_HEADERS	+= sys/sunddi.h sys/strsun.h sys/sun/ddi.h sys/sun/strconf.h sys/sun/strsun.h sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3
nobase_pkginclude_HEADERS	+= sys/svr3/ddi.h sys/svr3/strconf.h sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4
nobase_pkginclude_HEADERS	+= sys/svr4/ddi.h sys/svr4/strconf.h sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7
nobase_pkginclude_HEADERS	+= sys/uw7/ddi.h sys/uw7/strconf.h sys/uw7/stream.h
endif

if CONFIG_STREAMS_COMPAT_AIX_MODULE
nobase_pkginclude_HEADERS	+= sys/aix/ddi.h sys/aix/strconf.h sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
nobase_pkginclude_HEADERS	+= sys/hpux/ddi.h sys/hpux/strconf.h sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
nobase_pkginclude_HEADERS	+= sys/irix/ddi.h sys/irix/strconf.h sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
nobase_pkginclude_HEADERS	+= sys/lfs/ddi.h sys/lfs/strconf.h sys/lfs/stream.h sys/strlog.h
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
nobase_pkginclude_HEADERS	+= sys/lis/ddi.h sys/lis/strconf.h sys/lis/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
nobase_pkginclude_HEADERS	+= sys/mac/ddi.h sys/mac/strconf.h sys/mac/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
nobase_pkginclude_HEADERS	+= sys/mps/ddi.h sys/mps/strconf.h sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_OS7_MODULE
nobase_pkginclude_HEADERS	+= sys/os7/ddi.h sys/os7/strconf.h sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
nobase_pkginclude_HEADERS	+= sys/osf/ddi.h sys/osf/strconf.h sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
nobase_pkginclude_HEADERS	+= sys/sunddi.h sys/strsun.h sys/sun/ddi.h sys/sun/strconf.h sys/sun/strsun.h sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
nobase_pkginclude_HEADERS	+= sys/svr3/ddi.h sys/svr3/strconf.h sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
nobase_pkginclude_HEADERS	+= sys/svr4/ddi.h sys/svr4/strconf.h sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
nobase_pkginclude_HEADERS	+= sys/uw7/ddi.h sys/uw7/strconf.h sys/uw7/stream.h
endif

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

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
