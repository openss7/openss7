# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/12/17 21:52:43 $
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
# Last Modified $Date: 2004/12/17 21:52:43 $ by $Author: brian $
#
# =============================================================================

AUTOMAKE_OPTIONS = gnits 1.8 dist-bzip2 std-options -Wall

ACLOCAL_AMFLAGS = -I m4

#
# Accumulate these 'till the end
#
INSTALL_EXEC_LOCAL	=
INSTALL_DATA_LOCAL	=
UNINSTALL_LOCAL		=
INSTALL_EXEC_HOOK	=
INSTALL_DATA_HOOK	=
UNINSTALL_HOOK		=
ALL_LOCAL		=
CLEAN_LOCAL		=
DISTCLEAN_LOCAL		=

#
# Have the makefile regenerate libtool if necessary
#
$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

#
# I use vim, not emacs.  The following triggers Automake 1.8 to include
# targets for making vim tags files along with emacs TAGS files.
#
CTAGSFLAGS = --language-force=C --c-types=+px -I __initdata,__exitdata,EXPORT_SYMBOL,EXPORT_SYMBOL_NOVERS,EXPORT_SYMBOL_GPL

#
# A rule to generate an ifnames.log file
#
ifnames:
	ifnames `find $(srcdir) -name "*.h" -o -name "*.c"` > ifnames.log

distclean-logs:
	rm -f *.log

DISTCLEAN_LOCAL += distclean-logs

install-exec-local: $(INSTALL_EXEC_LOCAL)

install-data-local: $(INSTALL_DATA_LOCAL)

uninstall-local: $(UNINSTALL_LOCAL)

install-exec-hook: $(INSTALL_EXEC_HOOK)

install-data-hook: $(INSTALL_DATA_HOOK)

uninstall-hook: $(UNINSTALL_HOOK)

all-local: $(ALL_LOCAL)

clean-local: $(CLEAN_LOCAL)

distclean-local: $(DISTCLEAN_LOCAL)

SUBDIRS = @subdirs@

#
# Collected phonys from other automake fragments.
#
.PHONY: ifnames distclean-logs

# vim: ft=automake
