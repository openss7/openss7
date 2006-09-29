# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/09/29 11:50:40 $
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
# Last Modified $Date: 2006/09/29 11:50:40 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.2  2006/09/29 11:50:40  brian
# - libtool library tweaks in Makefile.am
# - better rpm spec handling in *.spec.in
# - added AC_LIBTOOL_DLOPEN to configure.ac
# - updated some copyright headers
# - rationalized item in two packages
# - added manual pages, drivers and modules to new strtty package
#
# Revision 0.9.2.1  2006/09/25 12:30:53  brian
# - added files for new strnsl package
#
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

pkglibexec32_PROGRAMS		+= test-xnsl

test_xnsl_SOURCES		= $(top_srcdir)/src/test/test-xnsl.c
test_xnsl_DEPENDENCIES		= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_xnsl_LDADD			= libxnsl.la
test_xnsl_LDFLAGS		= $(XTI_LDFLAGS32) $(MACHINE_FLAGS)
test_xnsl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_xnsl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-xnsl_thr

test_xnsl_thr_SOURCES		= $(top_srcdir)/src/test/test-xnsl.c
test_xnsl_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(top_builddir)/$(STRCONF_CONFIG) $(AUTOCONFIG_H)
test_xnsl_thr_LDADD		= libxnsl.la
test_xnsl_thr_LDFLAGS		= $(XTI_LDFLAGS32) -lpthread $(MACHINE_FLAGS)
test_xnsl_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

lib32_LTLIBRARIES		+= libxnsl.la

libxnsl_la_SOURCES		= $(top_srcdir)/src/lib/pthread.c \
				  $(top_srcdir)/src/lib/netselect.c \
				  $(top_srcdir)/src/lib/netselect.h \
				  $(top_srcdir)/src/lib/netdir.c \
				  $(top_srcdir)/src/lib/netdir_inet.c \
				  $(top_srcdir)/src/include/netconfig.h \
				  $(top_srcdir)/src/include/netdir.h \
				  $(top_srcdir)/src/include/sys/netconfig.h \
				  $(top_srcdir)/gettext.h
libxnsl_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnsl.VERSION
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  $(XTI_LDFLAGS32) -ldl $(MACHINE_FLAGS)
libxnsl_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS)
libxnsl_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnsl_la_LIBADD		= $(XTI_LDADD32)

lib32_LTLIBRARIES		+= inetaddr.la

inetaddr_la_SOURCES		= $(top_srcdir)/src/lib/netdir_inet.c \
				  $(top_srcdir)/src/include/netdir.h \
				  $(top_srcdir)/src/include/netconfig.h \
				  $(top_srcdir)/src/include/sys/netconfig.h
inetaddr_la_DEPENDENCIES	= libxnsl.la
inetaddr_la_LDFLAGS		= -module -avoid-version $(MACHINE_FLAGS)
inetaddr_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
inetaddr_la_LIBADD		= libxnsl.la

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
