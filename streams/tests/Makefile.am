# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2005/09/09 20:27:26 $
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
# Last Modified $Date: 2005/09/09 20:27:26 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.26  2005/09/09 20:27:26  brian
# - added mux driver
#
# Revision 0.9.2.25  2005/09/08 05:52:45  brian
# - added nullmod module and loop driver
# - corrections during testing
# - many ioctl(2p) test cases work very well now
#
# Revision 0.9.2.24  2005/07/21 20:47:25  brian
# - sync with notebook
#
# Revision 0.9.2.20  2005/07/21 20:47:25  brian
# - sync with notebook
#
# Revision 0.9.2.19  2005/06/15 00:46:05  brian
# - corrected a couple targets
#
# Revision 0.9.2.18  2005/05/30 14:25:55  brian
# - packaged standalong test suites
#
# Revision 0.9.2.17  2005/05/21 07:39:52  brian
# - new testsuite organization
#
# Revision 0.9.2.16  2005/05/20 04:44:58  brian
# - updated autotest makefiles
#
# Revision 0.9.2.15  2005/05/16 11:25:08  brian
# - minor rearrangement
#
# Revision 0.9.2.14  2005/05/15 23:57:20  brian
# - update tests
#
# Revision 0.9.2.13  2005/05/14 23:59:23  brian
# - getting autotest up and running
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
			$(TESTSUITE).at \
			local.at \
			package.m4
if CONFIG_STREAMS_CLONE
TESTSUITE_DEPS		+= $(TESTSUITE)-clone.at
endif
if CONFIG_STREAMS_CLONE_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-clone.at
endif
if CONFIG_STREAMS_CONNLD
TESTSUITE_DEPS		+= $(TESTSUITE)-connld.at
endif
if CONFIG_STREAMS_CONNLD_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-connld.at
endif
if CONFIG_STREAMS_ECHO
TESTSUITE_DEPS		+= $(TESTSUITE)-echo.at
endif
if CONFIG_STREAMS_ECHO_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-echo.at
endif
if CONFIG_STREAMS_FIFO
TESTSUITE_DEPS		+= $(TESTSUITE)-fifo.at
endif
if CONFIG_STREAMS_FIFO_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-fifo.at
endif
if CONFIG_STREAMS_LOG
TESTSUITE_DEPS		+= $(TESTSUITE)-log.at
endif
if CONFIG_STREAMS_LOG_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-log.at
endif
if CONFIG_STREAMS_LOOP
TESTSUITE_DEPS		+= $(TESTSUITE)-loop.at
endif
if CONFIG_STREAMS_LOOP_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-loop.at
endif
if CONFIG_STREAMS_NSDEV
TESTSUITE_DEPS		+= $(TESTSUITE)-nsdev.at
endif
if CONFIG_STREAMS_NSDEV_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-nsdev.at
endif
if CONFIG_STREAMS_MUX
TESTSUITE_DEPS		+= $(TESTSUITE)-mux.at
endif
if CONFIG_STREAMS_MUX_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-mux.at
endif
if CONFIG_STREAMS_NULS
TESTSUITE_DEPS		+= $(TESTSUITE)-nuls.at
endif
if CONFIG_STREAMS_NULS_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-nuls.at
endif
if CONFIG_STREAMS_PIPEMOD
TESTSUITE_DEPS		+= $(TESTSUITE)-pipemod.at
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-pipemod.at
endif
if CONFIG_STREAMS_SAD
TESTSUITE_DEPS		+= $(TESTSUITE)-sad.at
endif
if CONFIG_STREAMS_SAD_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-sad.at
endif
if CONFIG_STREAMS_SC
TESTSUITE_DEPS		+= $(TESTSUITE)-sc.at
endif
if CONFIG_STREAMS_SC_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-sc.at
endif

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
