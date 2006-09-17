# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# -----------------------------------------------------------------------------
#
# $Log$
# =============================================================================

##
##  General rules.
##
include ../am/openss7.am

lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

pkglibexec32_PROGRAMS		+= test-tty

test_tty_SOURCES		= $(top_srcdir)/src/test/test-tty.c
test_tty_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_tty_LDADD			= $(STREAMS_LDADD32)
test_tty_LDFLAGS		= -static $(MACHINE_FLAGS)
test_tty_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_tty_CFLAGS			= $(USER_CFLAGS) $(MACHINE_FLAGS)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
