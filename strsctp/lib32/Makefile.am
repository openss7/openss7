# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-28 23:13:22 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2008-04-28 23:13:22 $ by $Author: brian $
#
# =============================================================================

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

pkglibexec32_PROGRAMS		+= test-sctp

test_sctp_SOURCES		= $(top_srcdir)/src/test/test-sctp.c
test_sctp_LDADD			= $(STREAMS_LDADD32)
test_sctp_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sctp_n

test_sctp_n_SOURCES		= $(top_srcdir)/src/test/test-sctp_n.c
test_sctp_n_LDADD		= $(STREAMS_LDADD32)
test_sctp_n_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sctp_n2

test_sctp_n2_SOURCES		= $(top_srcdir)/src/test/test-sctp_n2.c
test_sctp_n2_LDADD		= $(STREAMS_LDADD32)
test_sctp_n2_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_n2_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_n2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sctp_nc

test_sctp_nc_SOURCES		= $(top_srcdir)/src/test/test-sctp_nc.c
test_sctp_nc_LDADD		= $(STREAMS_LDADD32)
test_sctp_nc_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_nc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sctp_ns

test_sctp_ns_SOURCES		= $(top_srcdir)/src/test/test-sctp_ns.c
test_sctp_ns_LDADD		= $(STREAMS_LDADD32)
test_sctp_ns_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_ns_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-sctp_t

test_sctp_t_SOURCES		= $(top_srcdir)/src/test/test-sctp_t.c
test_sctp_t_LDADD		= $(STREAMS_LDADD32)
test_sctp_t_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_sctp_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-etsi_n

test_etsi_n_SOURCES		= $(top_srcdir)/src/test/test-etsi_n.c
test_etsi_n_LDADD		= $(STREAMS_LDADD32)
test_etsi_n_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_etsi_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_etsi_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-etsi_t 

test_etsi_t_SOURCES		= $(top_srcdir)/src/test/test-etsi_t.c
test_etsi_t_LDADD		= $(STREAMS_LDADD32)
test_etsi_t_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_etsi_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_etsi_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-interop

test_interop_SOURCES		= $(top_srcdir)/src/test/test-interop.c
test_interop_LDADD		= $(STREAMS_LDADD32)
test_interop_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_interop_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_interop_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.5  2008-04-28 23:13:22  brian
# - updated headers for release
#
# Revision 0.9.2.4  2007/08/14 06:22:18  brian
# - GPLv3 header update
#
# Revision 0.9.2.3  2007/03/13 06:17:23  brian
# - suppress errors when building 32-bit libs
#
# Revision 0.9.2.2  2006/09/29 11:50:44  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.1  2006/09/17 23:09:12  brian
# - added lib32 compatibility directories
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
