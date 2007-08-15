# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/08/15 04:53:56 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/08/15 04:53:56 $ by $Author: brian $
#
# =============================================================================

include am/openss7.am

##
## Version file
##
include am/version.am

##
## Help
##
include am/help.am

##
## Problem Reports
##
include am/pr.am

#
# Just to force depcomp and libtool
#
lib_LTLIBRARIES =
EXTRA_LTLIBRARIES = libdummy.la
libdummy_la_SOURCES = dummy.c

#
# This is a little hook to move the distribution subdirectories out of the way
# so that a package can be overloaded later.
#
master-dist-hook:
	@(cd $(distdir) ; \
		list='$(subdirs)' ; for s in $$list ; do \
			if test -d "$$s" ; then \
				case " $(REMOVE_SUBDIRS) " in \
				(*" $$s "*) \
					$(ECHO) "rm -fr $$s" ; \
					rm -fr $$s ;; \
				(*) \
					if test $$s = stacks ; then d=strss7 ; else d="$$s" ; fi ; \
					version=`cat $$s/.version` ; \
					$(ECHO) "mv -f $$s $$d-$${version:-$(VERSION)}" ; \
					mv -f "$$s" "$$d-$${version:-$(VERSION)}" ; \
					$(ECHO) "ln -sf $$d-$${version:-$(VERSION)} $$s" ; \
					ln -sf $$d-$${version:-$(VERSION)} $$s ;; \
				esac ; \
			fi ; \
		done ; \
	)


DIST_HOOK += master-dist-hook

export RPMXOPTS="--nodeps"
export DEBXOPTS="-d"

all-distcheck: ALL_EXPORT_OPTS=\
	master_srcdir="$(master_srcdir)" \
	master_builddir="$(master_builddir)"

SUBDIRS	= @subdirs@

if DEVELOPMENT
SUBDIRS += doc
endif

dist_noinst_SCRIPTS		+= scripts/cflagcheck $(top_srcdir)/scripts/modpost.sh
EXTRA_SCRIPTS			+= scripts/cflagcheck

##
## Rules for lsms
##
include am/lsm.am

##
## Rules for archives
##
include am/archive.am

##
## Rules for making rpms
##
AM_RPMFLAGS 			= --define "_kversion $(kversion)"
AM_SRPMFLAGS 			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Rules for making release files
##
MANUAL_FILE			= doc/manual/$(PACKAGE).txt

include am/release.am

##
## Rules for making doxygen documentation.
##
ALL_RECURSIVE_TARGETS		+= all-doxy

doxy: all-doxy

EXTRA_DIST			+= \
				  README-LiS \
				  debian/README.Debian \
				  debian/TODO.Debian

DISTCLEANFILES			+= \
				  Modules.map \
				  System.symvers \
				  Module.symvers \
				  modpost.cache


# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.33  2007/08/15 04:53:56  brian
# - GPLv3 updates
#
# Revision 0.9.2.32  2007/03/24 01:46:45  brian
# - add dummy.c to invoke libtool and C compiler checks
#
# Revision 0.9.2.31  2007/03/16 10:30:16  brian
# - final changes from release
#
# Revision 0.9.2.30  2007/03/04 09:59:38  brian
# - updates for release, do not propagate empty flags, build improvements
#
# Revision 0.9.2.29  2007/03/04 00:25:51  brian
# - move directories
#
# Revision 0.9.2.28  2007/03/04 00:23:11  brian
# - add version file creation
#
# Revision 0.9.2.27  2007/01/08 09:37:09  brian
# - got manual pages building and instaling separately
#
# Revision 0.9.2.26  2006/10/01 21:30:10  brian
# - distribute release and library version files
#
# Revision 0.9.2.25  2006/09/18 17:43:05  brian
# - added doxy rules to makefile and release notes
#
# Revision 0.9.2.24  2006/09/18 01:15:18  brian
# - add 32bit libs, release file changes, additional packages
#
# Revision 0.9.2.23  2006/08/26 09:16:11  brian
# - better release file generation
#
# Revision 0.9.2.22  2006/08/22 13:02:54  brian
# - doc directory addition
#
# Revision 0.9.2.21  2006/07/11 01:57:54  brian
# - nexusware build fixes
#
# Revision 0.9.2.20  2006/03/21 13:24:32  brian
# - included pr.am
#
# Revision 0.9.2.19  2006/03/13 23:59:52  brian
# - pass down master source and build directories
#
# Revision 0.9.2.18  2006/03/07 12:32:55  brian
# - must clean module files
#
# Revision 0.9.2.17  2006/03/06 11:39:25  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
