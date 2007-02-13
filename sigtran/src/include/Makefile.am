## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/12/29 05:51:01 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, write to the Free Software Foundation, Inc., 675 Mass
## Ave, Cambridge, MA 02139, USA.
##
## -----------------------------------------------------------------------------
##
## U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
## behalf of the U.S. Government ("Government"), the following provisions apply
## to you.  If the Software is supplied by the Department of Defense ("DoD"), it
## is classified as "Commercial Computer Software" under paragraph 252.227-7014
## of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
## successor regulations) and the Government is acquiring only the license rights
## granted herein (the license rights customarily provided to non-Government
## users).  If the Software is supplied to any unit or agency of the Government
## other than DoD, it is classified as "Restricted Computer Software" and the
## Government's rights in the Software are defined in paragraph 52.227-19 of the
## Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
## the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
## (or any successor regulations).
##
## -----------------------------------------------------------------------------
##
## Commercial licensing and support of this software is available from OpenSS7
## Corporation at a fee.  See http://www.openss7.com/
##
## -----------------------------------------------------------------------------
##
## Last Modified $Date: 2006/12/29 05:51:01 $ by $Author: brian $
##
## -----------------------------------------------------------------------------
##
## $Log: Makefile.am,v $
## Revision 0.9.2.3  2006/12/29 05:51:01  brian
## - changes for successful master build
##
## Revision 0.9.2.2  2006/10/17 12:12:38  brian
## - working up new packages
##
## Revision 0.9.2.1  2006/10/16 10:42:48  brian
## - added new package files
##
## =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS = \
	sys/dlpi_dua.h \
	sys/dlpi_gr303ua.h \
	sys/dlpi_iua.h \
	sys/dlpi_m2ua.h \
	sys/dlpi_v5ua.h \
	sys/dua_ioctl.h \
	sys/gr303ua_ioctl.h \
	sys/iua_ioctl.h \
	sys/m2pa_ioctl.h \
	sys/m2ua_as.h \
	sys/m2ua_as_ioctl.h \
	sys/m2ua_ioctl.h \
	sys/m2ua_lm.h \
	sys/m3ua.h \
	sys/m3ua_as.h \
	sys/m3ua_as_ioctl.h \
	sys/m3ua_ioctl.h \
	sys/sigtran \
	sys/ua_as_ioctl.h \
	sys/ua_as_lm.h \
	sys/ua_ioctl.h \
	sys/ua_lm.h \
	sys/ua_lm_ioctl.h \
	sys/ua_sg_ioctl.h \
	sys/v5ua_ioctl.h

nobase_nodist_pkginclude_HEADERS = \
	sys/$(PACKAGE)/config.h \
	sys/$(PACKAGE)/version.h
#	sys/$(PACKAGE)/modversions.h

if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_headers
TESTS_ENVIRONMENT	+= CTAGS='$(CTAGIDS)' \
			   PKG_HEADERS='$(nobase_pkginclude_HEADERS) $(nobase_nodist_pkginclude_HEADERS)' \
			   PKG_MANPATH='$(PKG_MANPATH)' \
			   mandir='$(mandir)'
CLEANFILES		+= $(top_builddir)/check_headers.log
DISTCLEANFILES		+= *.err *.out
## PERFORM_CHECKS
endif

## PKG_BUILD_ARCH
endif

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
