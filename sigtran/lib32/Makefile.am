## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/13 06:16:59 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, write to the Free Software Foundation, Inc., 675 Mass
## Ave, Cambridge, MA 02139, USA.
##
## -----------------------------------------------------------------------------
##
## U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
## behalf of the U.S. Government ("Government"), the following provisions apply
## to you.  If the Software is supplied by the Department of Defense ("DoD"), it
## is classified as "Commercial Computer Software" under paragraph 252.227-7014
## of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
## successor regulations) and the Government is acquiring only the license rights
## granted herein (the license rights customarily provided to non-Government
## users).  If the Software is supplied to any unit or agency of the Government
## other than DoD, it is classified as "Restricted Computer Software" and the
## Government's rights in the Software are defined in paragraph 52.227-19 of the
## Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
## the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
## (or any successor regulations).
##
## -----------------------------------------------------------------------------
##
## Commercial licensing and support of this software is available from OpenSS7
## Corporation at a fee.  See http://www.openss7.com/
##
## -----------------------------------------------------------------------------
##
## Last Modified $Date: 2007/03/13 06:16:59 $ by $Author: brian $
##
## -----------------------------------------------------------------------------
##
## $Log: Makefile.am,v $
## Revision 0.9.2.4  2007/03/13 06:16:59  brian
## - suppress errors when building 32-bit libs
##
## Revision 0.9.2.3  2007/01/23 10:00:44  brian
## - added test program and m2ua-as updates
##
## Revision 0.9.2.2  2006/10/17 12:12:37  brian
## - working up new packages
##
## Revision 0.9.2.1  2006/10/16 10:42:46  brian
## - added new package files
##
## =============================================================================

##
##  General rules.
##
include ../am/openss7.am

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

pkglibexec32_PROGRAMS		+= test-sigtran

test_sigtran_SOURCES		= $(top_srcdir)/src/test/test-sigtran.c
test_sigtran_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_sigtran_LDADD		= $(STREAMS_LDADD32)
test_sigtran_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sigtran_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sigtran_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-m2pa

test_m2pa_SOURCES		= $(top_srcdir)/src/test/test-m2pa.c
test_m2pa_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_m2pa_LDADD			= $(STREAMS_LDADD32)
test_m2pa_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_m2pa_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_m2pa_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-m2ua_as

test_m2ua_as_SOURCES		= $(top_srcdir)/src/test/test-m2ua_as.c
test_m2ua_as_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG)
test_m2ua_as_LDADD		= $(STREAMS_LDADD32)
test_m2ua_as_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_m2ua_as_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_m2ua_as_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
