# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/05/14 08:29:36 $
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
# Last Modified $Date: 2005/05/14 08:29:36 $ by $Author: brian $
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

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

# my rule
TESTSUITE		= testsuite

# my rule
package.m4: $(top_srcdir)/configure
	@( \
		$(ECHO) 'm4_define([AT_PACKAGE_NAME], [@PACKAGE_NAME@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_TARNAME], [@PACKAGE_TARNAME@])'; \
		$(ECHO) 'm4_define([AT_PACKAGE_VERSION], [@PACKAGE_VERSION@])'; \
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
			testsuite.at \
			kernel.at \
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
	$(SHELL) $<

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

## PKG_BUILD_INDEP
endif

.PHONY: all-testsuite installcheck-testsuite clean-testsuite \
	installcheck-specfs installcheck-streams installcheck-xnet installcheck-timod
	installcheck-tirdwr installcheck-inet installcheck-test

# vim: ft=automake
