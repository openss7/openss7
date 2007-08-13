# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/13 23:51:31 $
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
# Last Modified $Date: 2007/08/13 23:51:31 $ by $Author: brian $
#
# =============================================================================

include ../am/openss7.am

AM_EXPOSED_SYMBOLS	=

if WITH_LIS
AM_EXPOSED_SYMBOLS	+= qbufcall qtimeout qwriter streams_put
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

SUBDIRS = . man manual

##
## The following symbols are exposed by the header files (to ctags) but are only
## conditionally exposed.  These symbols would generate an unexported symbol
## error unless we list them here and tell check_decls about them.  We use
## AM_EXPOSED_SYMBOLS here and leave the EXPOSED_SYMBOLS susbtitution variable
## available to configure.  We should actually use configure to assemble this
## list: that is, when we check for support for the symbol and find that it is
## not supported, we should add it to the EXPOSED_SYMBOLS list in configure.
## We list them here manually for the moment.
##
# Can't support qwait or qwait_sig on LiS without additional support from LiS.
# Use SV_WAIT in qopen/qclose and SV_SIGNAL or SV_BROADCAST in put or srv
# procedure instead.
#
# Can't support wantio in LiS at all.
##
AM_EXPOSED_SYMBOLS 	+= qwait qwait_sig wantio


if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_decls
TESTS_ENVIRONMENT	+= EXPOSED_SYMBOLS='$(EXPOSED_SYMBOLS) $(AM_EXPOSED_SYMBOLS)' \
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
# Revision 0.9.2.6  2007/08/13 23:51:31  brian
# - updated to GPLv3 headers
#
# Revision 0.9.2.5  2006/03/06 11:38:30  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
