# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.4.11 $) $Date: 2005/11/07 10:54:54 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# Unauthorized distribution or duplication is prohibited.
#
# This software and related documentation is protected by copyright and
# distributed under licenses restricting its use, copying, distribution and
# decompilation.  No part of this software or related documentation may be
# reproduced in any form by any means without the prior written
# authorization of the copyright holder, and licensors, if any.
#
# The recipient of this document, by its retention and use, warrants that
# the recipient will protect this information and keep it confidential, and
# will not disclose the information contained in this document without the
# written permission of its owner.
#
# The author reserves the right to revise this software and documentation
# for any reason, including but not limited to, conformity with standards
# promulgated by various agencies, utilization of advances in the state of
# the technical arts, or the reflection of changes in the design of any
# techniques, or procedures embodied, described, or referred to herein.
# The author is under no obligation to provide any feature listed herein.
#
# -----------------------------------------------------------------------------
#
# As an exception to the above, this software may be distributed under the
# GNU General Public License (GPL) Version 2 or later, so long as the
# software is distributed with, and only used for the testing of, OpenSS7
# modules, drivers, and libraries.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"),
# it is classified as "Commercial Computer Software" under paragraph
# 252.227-7014 of the DoD Supplement to the Federal Acquisition Regulations
# ("DFARS") (or any successor regulations) and the Government is acquiring
# only the license rights granted herein (the license rights customarily
# provided to non-Government users).  If the Software is supplied to any unit
# or agency of the Government other than DoD, it is classified as "Restricted
# Computer Software" and the Government's rights in the Software are defined
# in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR") (or
# any successor regulations) or, in the cases of NASA, in paragraph 18.52.227-86
# of the NASA Supplement to the FAR (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2005/11/07 10:54:54 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 1.1.4.11  2005/11/07 10:54:54  brian
# - added SAD testing
#
# Revision 1.1.4.10  2005/10/23 05:01:26  brian
# - test programs and modules for POSIX testing
#
# Revision 1.1.4.9  2005/06/15 00:46:04  brian
# - corrected a couple targets
#
# Revision 1.1.4.8  2005/05/30 14:25:54  brian
# - packaged standalong test suites
#
# Revision 1.1.4.7  2005/05/21 07:39:51  brian
# - new testsuite organization
#
# Revision 1.1.4.6  2005/05/21 07:14:25  brian
# - added full set of LiS tests
#
# Revision 1.1.4.5  2005/05/20 04:44:57  brian
# - updated autotest makefiles
#
# =============================================================================

include ../am/openss7.am

pkglibexecdir = $(libexecdir)/@PACKAGE@

#
# This automake file is used to specify the autotest definitions and targets
# necessary for performing pre-install and post-install checks.  Pre-install
# checks will work for libraries and application programs, however, it is
# necessary to insert kernel modules into the running kernel to perform most
# pre-install checks.
#

# my rule
TESTSUITE		= testsuite

# my rule
package.m4: $(top_srcdir)/configure
	@( \
		$(ECHO) 'm4_define([AT_PACKAGE_NAME], [@PACKAGE_NAME@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_TARNAME], [@PACKAGE@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_VERSION], [@VERSION@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_RELEASE], [@PACKAGE_RELEASE@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_STRING], [@PACKAGE_STRING@.@PACKAGE_RELEASE@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	) > $@

# my rule
TESTSUITE_DEPS		= \
			kernel.at \
			$(TESTSUITE)-streams.at \
			$(TESTSUITE)-fifo.at \
			$(TESTSUITE)-pipe.at \
			$(TESTSUITE)-sad.at \
			$(TESTSUITE)-strtst.at \
			$(TESTSUITE)-timetst.at \
			$(TESTSUITE)-test.at \
			$(TESTSUITE)-test2.at \
			$(TESTSUITE)-thrtst.at \
			$(TESTSUITE).at \
			local.at \
			package.m4

# my rule
$(TESTSUITE): $(TESTSUITE_DEPS)
	$(AUTOTEST) -I $(srcdir) $@.at -o $@

# my rule
atconfig: $(top_builddir)/config.status
	( cd $(top_builddir) ; $(SHELL) ./config.status $(subdir)/$@ )

# automake does this one for us
#
#atlocal: $(srcdir)/atlocal.in $(top_builddir)/config.status
#	( cd $(top_builddir) ; $(SHELL) ./config.status $(subdir)/$@ )

# my rule
all-testsuite: $(TESTSUITE) atconfig atlocal

if PERFORM_TESTING
## PERFORM_TESTING

INSTALLCHECK_TESTS =

# my rule
installcheck-test: $(TESTSUITE)
	$(SHELL) $< DESTDIR='$(DESTDIR)' krootdir='$(krootdir)' AUTOTEST_PATH='.'

INSTALLCHECK_TESTS += installcheck-test

# my rule
installcheck-testsuite: $(TESTSUITE) atconfig atlocal $(INSTALLCHECK_TESTS)

## PERFORM_TESTING
else
## ! PERFORM_TESTING

# my rule
installcheck-testsuite: $(TESTSUITE) atconfig atlocal

## ! PERFORM_TESTING
endif

# my rule
clean-testsuite:
	@for f in $(TESTSUITE) ; do \
		if test -r "$$f" ; then \
			$(ECHO) "$(SHELL) $$f --clean" ; \
			$(SHELL) $$f --clean ; \
			$(ECHO) "rm -f -- $$f" ; \
			rm -f -- $$f ; \
		fi ; \
	done

CLEANFILES		+= \
			package.m4

DISTCLEANFILES		+= \
			atconfig \
			atlocal

ALL_LOCAL += all-testsuite

INSTALLCHECK_LOCAL += installcheck-testsuite

CLEAN_LOCAL += clean-testsuite

if PKG_BUILD_INDEP
## Test suites are actually architecture independent!
pkglibexec_DATA		= atlocal
pkglibexec_SCRIPTS	= $(TESTSUITE)
endif

# my rule
EXTRA_DIST		+= $(TESTSUITE) $(TESTSUITE_DEPS)

.PHONY: all-testsuite installcheck-testsuite clean-testsuite $(INSTALLCHECK_TESTS)

# vim: ft=automake
