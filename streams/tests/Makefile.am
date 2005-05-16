# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2005/05/16 11:25:08 $
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
# Last Modified $Date: 2005/05/16 11:25:08 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.19  2005/05/16 11:25:08  brian
# - minor rearrangement
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

#
# This automake file is used to specify the autotest definitions and targets
# necessary for performing pre-install and post-install checks.  Pre-install
# checks will work for libraries and application programs, however, it is
# necessary to insert kernel modules into the running kernel to perform most
# pre-install checks.
#

# my rule
EXTRA_DIST		+= $(TESTSUITE) $(TESTSUITE_DEPS)
# my rule
dist_noinst_SCRIPTS	+= $(TESTSUITE)

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

# my rule
TESTSUITE		= testsuite

# my rule
package.m4: $(top_srcdir)/configure
	@( \
		$(ECHO) 'm4_define([AT_PACKAGE_NAME], [@PACKAGE_NAME@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_TARNAME], [@PACKAGE@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_VERSION], [@VERSION@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_STRING], [@PACKAGE_STRING@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	) > $@
	@if test ':$(srcdir)' != ':.' ; then \
		$(ECHO) "cp -pf package.m4 $(srcdir)/package.m4" ; \
		cp -pf package.m4 $(srcdir)/package.m4 ; \
		$(ECHO) "rm -f package.m4" ; \
		rm -f package.m4 ; \
	fi

# my rule
TESTSUITE_DEPS		= \
			kernel.at \
			clone.at \
			connld.at \
			echo.at \
			fifo.at \
			log.at \
			nsdev.at \
			nuls.at \
			pipemod.at \
			sad.at \
			sc.at \
			streams.at \
			testsuite.at \
			package.m4

# my rule
$(TESTSUITE): $(TESTSUITE_DEPS)
	$(AUTOTEST) -I $(srcdir) $@.at -o $@.tmp
	@file=`$(ECHO) '$@' | sed -e 's|.*/||'` ; \
	cp -pf $@.tmp $(srcdir)/$$file
	rm -f $@.tmp

# my rule
atconfig: $(top_builddir)/config.status
	( cd $(top_builddir) ; $(SHELL) ./config.status $(subdir)/$@ )

# automake does this one for us
#
#atlocal: $(srcdir)/atlocal.in $(top_builddir)/config.status
#	( cd $(top_builddir) ; $(SHELL) ./config.status $(subdir)/$@ )

# my rule
all-testsuite: $(TESTSUITE)

if PERFORM_TESTING

# my rule
installcheck-specfs:

# my rule
installcheck-streams:

# my rule
installcheck-xnet:

# my rule
installcheck-timod:

# my rule
installcheck-tirdwr:

# my rule
installcheck-inet:

# my rule
installcheck-test: $(TESTSUITE)
	$(SHELL) $< DESTDIR='$(DESTDIR)' krootdir='$(krootdir)' AUTOTEST_PATH='.'

# my rule
installcheck-testsuite: $(TESTSUITE) atconfig atlocal \
	installcheck-specfs installcheck-streams installcheck-xnet installcheck-timod \
	installcheck-tirdwr installcheck-inet installcheck-test

else

# my rule
installcheck-testsuite: $(TESTSUITE) atconfig atlocal

endif

# my rule
clean-testsuite: $(TESTSUITE)
	@if test -r "$<" ; then \
		$(ECHO) "$(SHELL) $< --clean" ; \
		$(SHELL) $< --clean ; \
	fi

CLEANFILES		+= \
			$(TESTSUITE) \
			atconfig \
			atlocal \
			package.m4

MAINTAINERCLEANFILES	+= \
			$(srcdir)/$(TESTSUITE) \
			$(srcdir)/atconfig \
			$(srcdir)/atlocal \
			$(srcdir)/package.m4

ALL_LOCAL += all-testsuite

INSTALLCHECK_LOCAL += installcheck-testsuite

CLEAN_LOCAL += clean-testsuite

## PKG_BUILD_ARCH
endif

.PHONY: all-testsuite installcheck-testsuite clean-testsuite \
	installcheck-specfs installcheck-streams installcheck-xnet installcheck-timod
	installcheck-tirdwr installcheck-inet installcheck-test

# vim: ft=automake
