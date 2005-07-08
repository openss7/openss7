# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/07/08 12:17:53 $
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
# Last Modified $Date: 2005/07/08 12:17:53 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS 	= \
				  os7/allocb.h \
				  os7/bufpool.h \
				  os7/bufq.h \
				  os7/compat.h \
				  os7/debug.h \
				  os7/lock.h \
				  os7/priv.h \
				  os7/queue.h \
				  os7/timer.h \
				  sys/strlog.h \
				  sys/strconf.h

if CONFIG_STREAMS_COMPAT_AIX
nobase_pkginclude_HEADERS	+= sys/aixddi.h
endif
if CONFIG_STREAMS_COMPAT_HPUX
nobase_pkginclude_HEADERS	+= sys/hpuxddi.h
endif
if CONFIG_STREAMS_COMPAT_IRIX
nobase_pkginclude_HEADERS	+= sys/irixddi.h
endif
if CONFIG_STREAMS_COMPAT_LFS
nobase_pkginclude_HEADERS	+= sys/lfsddi.h
endif
if CONFIG_STREAMS_COMPAT_LIS
nobase_pkginclude_HEADERS	+= sys/lisddi.h
endif
if CONFIG_STREAMS_COMPAT_MAC
nobase_pkginclude_HEADERS	+= sys/macddi.h
endif
if CONFIG_STREAMS_COMPAT_MPS
nobase_pkginclude_HEADERS	+= sys/mpsddi.h
endif
if CONFIG_STREAMS_COMPAT_OSF
nobase_pkginclude_HEADERS	+= sys/osfddi.h
endif
if CONFIG_STREAMS_COMPAT_SUN
nobase_pkginclude_HEADERS	+= sys/sunddi.h sys/strsun.h
endif
if CONFIG_STREAMS_COMPAT_SVR3
nobase_pkginclude_HEADERS	+= sys/svr3ddi.h
endif
if CONFIG_STREAMS_COMPAT_SVR4
nobase_pkginclude_HEADERS	+= sys/svr4ddi.h
endif
if CONFIG_STREAMS_COMPAT_UW7
nobase_pkginclude_HEADERS	+= sys/uw7ddi.h
endif

if CONFIG_STREAMS_COMPAT_AIX_MODULE
nobase_pkginclude_HEADERS	+= sys/aixddi.h
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
nobase_pkginclude_HEADERS	+= sys/hpuxddi.h
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
nobase_pkginclude_HEADERS	+= sys/irixddi.h
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
nobase_pkginclude_HEADERS	+= sys/lfsddi.h
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
nobase_pkginclude_HEADERS	+= sys/lisddi.h
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
nobase_pkginclude_HEADERS	+= sys/macddi.h
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
nobase_pkginclude_HEADERS	+= sys/mpsddi.h
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
nobase_pkginclude_HEADERS	+= sys/osfddi.h
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
nobase_pkginclude_HEADERS	+= sys/sunddi.h sys/strsun.h
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
nobase_pkginclude_HEADERS	+= sys/svr3ddi.h
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
nobase_pkginclude_HEADERS	+= sys/svr4ddi.h
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
nobase_pkginclude_HEADERS	+= sys/uw7ddi.h
endif

nobase_nodist_pkginclude_HEADERS = \
	sys/strcompat/version.h
#	sys/strcompat/modversions.h

## PKG_BUILD_ARCH
endif

# vim: ft=automake
