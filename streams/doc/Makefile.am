# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-04-28 12:53:38 $
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
# Last Modified $Date: 2008-04-28 12:53:38 $ by $Author: brian $
#
# =============================================================================

include ../am/openss7.am

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

SUBDIRS = . man manual papers

if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_decls
TESTS_ENVIRONMENT	+= EXPOSED_SYMBOLS='$(EXPOSED_SYMBOLS) $(AM_EXPOSED_SYMBOLS)' \
			   WARN_EXCESS=yes \
			   PKG_MANPATH='$(PKG_MANPATH)' \
			   mandir='$(mandir)'
CLEANFILES		+= $(top_builddir)/check_decls.log
DISTCLEANFILES		+= *.err *.out
## PERFORM_CHECKS
endif

## PKG_BUILD_INDEP
endif

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.11  2008-04-28 12:53:38  brian
# - update file headers for release
#
# Revision 0.9.2.10  2007/08/13 22:45:56  brian
# - GPLv3 header updates
#
# Revision 0.9.2.9  2007/06/17 02:19:41  brian
# - updates for release
#
# Revision 0.9.2.8  2006/03/06 11:38:37  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
