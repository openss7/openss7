# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/04/03 12:44:16 $
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
# Last Modified $Date: 2004/04/03 12:44:16 $ by $Author: brian $
#
# =============================================================================

if PUBLIC_RELEASE

libexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod

sbin_PROGRAMS			= \
				  inetmakenodes

inetmakenodes_SOURCES		= inetmakenodes.c
inetmakenodes_DEPENDENCIES	= strconf.h
inetmakenodes_LDDADD		=
inetmakenodes_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
inetmakenodes_CFLAGS		= @USER_CFLAGS@

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lLiS
test_tirdwr_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lLiS
test_timod_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libINETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libINETmods_a_SOURCES	= \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  src/drivers/inet.c \
				  src/drivers/debug.h \
				  src/drivers/bufq.h \
				  ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libINETmods_a_SOURCES)

nodist_libINETmods_a_SOURCES	= $(mods_source_FILES)

libINETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libINETmods_a_CPPFLAGS		= -I./src/include -I@srcdir@/src/include -I./src/modules -I@srcdir@/src/modules -I./src/drivers -I@srcdir@/src/drivers -imacros ./strconf.h @INET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libINETmods_a_CFLAGS		= @KERNEL_CFLAGS@

KERNEL_MODULES			= $(libINETmods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

else

libexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod

sbin_PROGRAMS			= \
				  inetmakenodes

inetmakenodes_SOURCES		= inetmakenodes.c
inetmakenodes_DEPENDENCIES	= strconf.h
inetmakenodes_LDDADD		=
inetmakenodes_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
inetmakenodes_CFLAGS		= @USER_CFLAGS@

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lLiS
test_tirdwr_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lLiS
test_timod_CPPFLAGS		= @INET_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libINETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libINETmods_a_SOURCES	= \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  src/drivers/inet.c \
				  ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libINETmods_a_SOURCES)

nodist_libINETmods_a_SOURCES	= $(mods_source_FILES)

libINETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libINETmods_a_CPPFLAGS		= -I./src/include -I@srcdir@/src/include -I./src/modules -I@srcdir@/src/modules -I./src/drivers -I@srcdir@/src/drivers -imacros ./strconf.h @INET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libINETmods_a_CFLAGS		= @KERNEL_CFLAGS@

KERNEL_MODULES			= $(libINETmods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

endif

KERNEL_MODCONF			= conf.modules
KERNEL_SUBDIR			= inet
KMODCONF_EXT			= inet

MANMACRO_PFX			= strinet
mansrcdir			= $(srcdir)/doc/man

include kernel.am

man_MANS = \
	doc/man/man3/xxx.3 \
	doc/man/man4/xxx.4 \
	doc/man/man5/xxx.5 \
	doc/man/man7/xxx.7 \
	doc/man/man8/xxx.8


nobase_include_HEADERS = \
	src/include/xti/xti.h \
	src/include/xti/xti_inet.h \
	src/include/xti/xti_ip.h \
	src/include/xti/xti_sctp.h \
	src/include/xti/xti_tcp.h \
	src/include/xti/xti_udp.h \
	src/include/sys/xti.h \
	src/include/sys/xti_inet.h \
	src/include/sys/xti_ip.h \
	src/include/sys/xti_sctp.h \
	src/include/sys/xti_tcp.h \
	src/include/sys/xti_udp.h \
	src/include/sys/tihdr.h \
	src/include/sys/timod.h \
	src/include/xti.h \
	src/include/xti_inet.h \
	src/include/tihdr.h \
	src/include/timod.h

noinst_SCRIPTS = \
	$(auxdir)/strconf-sh

strconf_FILES = \
	src/modules/lis.conf \
	src/drivers/lis.conf

#
#   Need to override the following two automake rules to strip the first two
#   path components (src/include) from the header file path name for install.
#   This just saves us from doing a recursive make into the src/include directory.
#
install-nobase_includeHEADERS: $(nobase_include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(nobase_include_HEADERS)' ; for p in $$list ; do \
		if test -f "$$p" ; then d= ; else d="$(srcdir)/" ; fi ; \
		f="`echo $$p | sed -e 's|^[^/]*/[^/]*/||'`" ; \
		echo "$(nobase_includeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(includedir)/$$f" ; \
		$(nobase_includeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(includedir)/$$f ; \
	done

uninstall-nobase_includeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(nobase_include_HEADERS)' ; for p in $$list ; do \
		f="`echo $$p | sed -e 's|^[^/]*/[^/]*/||'`" ; \
		echo "rm -f $(DESTDIR)$(includedir)/$$f" ; \
		rm -f $(DESTDIR)$(includedir)/$$f ; \
	done

STRCONF_SCRIPT	    = $(auxdir)/strconf-sh
STRCONF		    = $(SHELL) $(STRCONF_SCRIPT) -b190
STRMAKENODES	    = inetmakenodes

Config.master: $(strconf_FILES)
	cat /dev/null > Config.master
	@list='$(strconf_FILES)' ; cat /dev/null > Config.master ; for f in $$list ; do \
		if test -f "$$f" ; then d= ; else d="$(srcdir)/" ; fi ; \
		echo "cat $$d$$f >> Config.master" ; \
		cat $$d$$f >> Config.master ; \
	done

conf.modules: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --confmodules=conf.modules Config.master

strconf.h: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --hconfig=strconf.h Config.master

devices.lst: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --strmknods=devices.lst Config.master

inetmakenodes.c: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --makenodes=inetmakenodes.c Config.master

clean-local:
	rm -f Config.master conf.modules strconf.h devices.lst inetmakenodes.c

$(libINETmods_a_OBJECTS): strconf.h

install-devices: $(STRMAKENODES)
	$(STRMAKENODES)

uninstall-devices: $(STRMAKENODES)
	$(STRMAKENODES) --remove

all-am: conf.modules strconf.h devices.lst

inet_source_FILES		= $(mods_source_FILES)

EXTRA_DOCS = 

EXTRA_DIST = strinet.spec openss7-logo-small.gif $(man_MANS) $(noinst_SCRIPTS) \
	     doc/man/strinet.macros doc/man/strinet.refs $(strconf_FILES) \
	     src/include/xti_inet.h $(inet_source_FILES) $(EXTRA_DOCS) \
	     src/drivers/debug.h src/drivers/bufq.h

SUBDIRS = doc

#
#   Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion @kversion@"
AM_SRPMFLAGS =

include rpm.am

#
#   General rules
#

include openss7.am

# vim: ft=automake
