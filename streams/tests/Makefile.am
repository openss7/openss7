# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/08/22 06:17:56 $
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
# Last Modified $Date: 2004/08/22 06:17:56 $ by $Author: brian $
#
# =============================================================================

#
# This automake file is used to specify the autotest definitions and targets
# necessary for performing pre-install and post-install checks.  Pre-install
# checks will work for libraries and application programs, however, it is
# necessary to insert kernel modules into the running kernel to perform most
# pre-install checks.
#

TESTSUITE		= testsuite

$(srcdir)/package.m4: $(top_srcdir)/configure
	@( \
		echo 'm4_define([AT_PACKAGE_NAME], [@PACKAGE_NAME@])'; \
		echo 'm4_define([AT_PACKAGE_TARNAME], [@PACKAGE_TARNAME@])'; \
		echo 'm4_define([AT_PACKAGE_VERSION], [@PACKAGE_VERSION@])'; \
		echo 'm4_define([AT_PACKAGE_STRING], [@PACKAGE_STRING@])'; \
		echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	) > $@

TESTSUITE_DEPS		= \
			$(srcdir)/testsuite.at \
			$(srcdir)/dummy1.m4 \
			$(srcdir)/dummy2.m4 \
			$(srcdir)/dummy3.m4 \
			$(srcdir)/dummy4.m4 \
			$(srcdir)/package.m4

$(TESTSUITE): $(TESTSUITE_DEPS)
	$(AUTOTEST) -I $(srcdir) $@.at -o $@.tmp
	mv -f $@.tmp $@

atconfig: $(top_builddir)/config.status
	( cd $(top_builddir) ; $(SHELL) ./config.status $(subdir)/$@ )

EXTRA_DIST		= testsuite.at testsuite package.m4 \
			  dummy1.m4 dummy2.m4 dummy3.m4 dummy4.m4
dist_noinst_SCRIPTS	= testsuite

if PERFORM_TESTING

installcheck-specfs:

installcheck-streams:

installcheck-xnet:

installcheck-timod:

installcheck-tirdwr:

installcheck-inet:

installcheck-test:
#	$(SHELL) $(TESTSUITE) -x -v -d

installcheck-hook: installcheck-specfs

installcheck-hook: installcheck-streams

installcheck-hook: installcheck-xnet

installcheck-hook: installcheck-timod

installcheck-hook: installcheck-tirdwr

installcheck-hook: installcheck-inet

installcheck-hook: installcheck-test

installcheck-hook: atconfig atlocal

else

installcheck-hook: atconfig atlocal

endif

installcheck-local:
	$(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" installcheck-hook

clean-local:
	@if test -r "$(TESTSUITE)" ; then \
		echo "$(SHELL) $(TESTSUITE) --clean" ; \
		$(SHELL) $(TESTSUITE) --clean ; \
	fi

distclean-local:
	rm -f atconfig atlocal

.PHONY: installcheck-hook \
	installcheck-specfs \
	installcheck-streams \
	installcheck-xnet \
	installcheck-timod \
	installcheck-tirdwr \
	installcheck-inet \
	installcheck-test

# vim: ft=automake
