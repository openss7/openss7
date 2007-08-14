# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 04:56:43 $
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
# Last Modified $Date: 2007/08/14 04:56:43 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

xnsl_man_MANS 	= \
	man3/endnetconfig.3 \
	man3/endnetpath.3 \
	man3/freenetconfigent.3 \
	man3/getnetconfig.3 \
	man3/getnetconfigent.3 \
	man3/getnetpath.3 \
	man3/libxnsl.3 \
	man3/nc_perror.3 \
	man3/nc_sperror.3 \
	man3/_nderror.3 \
	man3/netbuf.3 \
	man3/netconfig.3 \
	man3/netdir.3 \
	man3/netdir_free.3 \
	man3/netdir_getbyaddr.3 \
	man3/netdir_getbyname.3 \
	man3/netdir_options.3 \
	man3/netdir_perror.3 \
	man3/netdir_sperror.3 \
	man3/netpath.3 \
	man3/setnetconfig.3 \
	man3/setnetpath.3 \
	man3/taddr2uaddr.3 \
	man3/uaddr2taddr.3 \
	man3/xnsl.3 \
	man3/inetaddr.3 \
	man5/netconfig.5 \
	man5/netdir.5 \
	man8/test-xnsl.8 \
	man8/test-xnsl_thr.8

man_MANS	= \
	man5/strnsl.5 \
	$(xnsl_man_MANS)

MANS_UNUSED = \
	man3/netbuf.3 \
	man3/netpath.3

xnsl_dist_noinst_MANS 	= \
	man3/endnetconfig.3.man \
	man3/endnetpath.3.man \
	man3/freenetconfigent.3.man \
	man3/getnetconfig.3.man \
	man3/getnetconfigent.3.man \
	man3/getnetpath.3.man \
	man3/libxnsl.3.man \
	man3/nc_perror.3.man \
	man3/nc_sperror.3.man \
	man3/_nderror.3.man \
	man3/netbuf.3.man \
	man3/netconfig.3.man \
	man3/netdir.3.man \
	man3/netdir_free.3.man \
	man3/netdir_getbyaddr.3.man \
	man3/netdir_getbyname.3.man \
	man3/netdir_options.3.man \
	man3/netdir_perror.3.man \
	man3/netdir_sperror.3.man \
	man3/netpath.3.man \
	man3/setnetconfig.3.man \
	man3/setnetpath.3.man \
	man3/taddr2uaddr.3.man \
	man3/uaddr2taddr.3.man \
	man3/xnsl.3.man \
	man3/inetaddr.3.man \
	man5/netconfig.5.man \
	man5/netdir.5.man \
	man5/strnsl.5.man \
	man8/test-xnsl.8.man \
	man8/test-xnsl_thr.8.man

dist_noinst_MANS	= $(xnsl_dist_noinst_MANS)

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.4  2007/08/14 04:56:43  brian
# - GPLv3 header update
#
# Revision 0.9.2.3  2006/12/31 13:47:42  brian
# - updated documentation for release
#
# Revision 0.9.2.2  2006/09/26 00:51:19  brian
# - corrected macro file name, reordered includes
#
# Revision 0.9.2.1  2006/09/25 12:30:48  brian
# - added files for new strnsl package
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
