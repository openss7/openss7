# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2007/08/14 12:57:57 $
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
# Last Modified $Date: 2007/08/14 12:57:57 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

man_MANS = \
	man1/strlog.1 \
	man4/fifo.4 \
	man5/strutil.5 \
	man8/strconf.8 \
	man8/strlogd.8 \
	man8/strutil_mknod.8

if WITH_LIS
## WITH_LIS
man_MANS += \
	man1/strchg.1 \
	man1/strconf.1 \
	man1/strreset.1 \
	man3/pipe.3 \
	man3/s_pipe.3 \
	man4/clone.4 \
	man4/connld.4 \
	man4/conslog.4 \
	man4/echo.4 \
	man4/fifo.4s \
	man4/log.4 \
	man4/nsdev.4 \
	man4/nuls.4 \
	man4/pipe.4 \
	man4/pipemod.4 \
	man4/sad.4 \
	man4/sc.4 \
	man4/s_fifo.4 \
	man4/sfx.4 \
	man4/spx.4 \
	man4/strlog.4 \
	man5/autopush.5 \
	man5/strsetup.conf.5 \
	man8/autopush.8 \
	man8/fattach.8 \
	man8/fdetach.8 \
	man8/insf.8 \
	man8/scls.8 \
	man8/soon.8 \
	man8/strace.8 \
	man8/strclean.8 \
	man8/strerr.8 \
	man8/strinfo.8 \
	man8/strload.8 \
	man8/strsetup.8 \
	man8/strvf.8
## WITH_LIS
endif

MANS_UNUSED = \
	man3/pipe.3 \
	man3/s_pipe.3 \
	man4/s_fifo.4 \
	man8/soon.8 \
	man8/strconf.8

dist_noinst_MANS = \
	man1/strchg.1.man \
	man1/strconf.1.man \
	man1/strlog.1.man \
	man1/strreset.1.man \
	man3/pipe.3.man \
	man3/s_pipe.3.man \
	man4/clone.4.man \
	man4/connld.4.man \
	man4/conslog.4.man \
	man4/echo.4.man \
	man4/fifo.4.man \
	man4/fifo.4s.man \
	man4/log.4.man \
	man4/nsdev.4.man \
	man4/nuls.4.man \
	man4/pipe.4.man \
	man4/pipemod.4.man \
	man4/sad.4.man \
	man4/sc.4.man \
	man4/s_fifo.4.man \
	man4/sfx.4.man \
	man4/spx.4.man \
	man4/strlog.4.man \
	man5/autopush.5.man \
	man5/strsetup.conf.5.man \
	man5/strutil.5.man \
	man8/autopush.8.man \
	man8/fattach.8.man \
	man8/fdetach.8.man \
	man8/insf.8.man \
	man8/scls.8.man \
	man8/soon.8.man \
	man8/strace.8.man \
	man8/strclean.8.man \
	man8/strconf.8.man \
	man8/strerr.8.man \
	man8/strinfo.8.man \
	man8/strload.8.man \
	man8/strlogd.8.man \
	man8/strsetup.8.man \
	man8/strutil_mknod.8.man \
	man8/strvf.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.10  2007/08/14 12:57:57  brian
# - GNUv3 header updates
#
# Revision 0.9.2.9  2006/12/31 13:55:36  brian
# - updated documentation for release
#
# Revision 0.9.2.8  2006/10/02 12:08:26  brian
# - changes to pass check target
#
# Revision 0.9.2.7  2006/09/18 01:18:45  brian
# - updated documentation and include file makefiles
#
# Revision 0.9.2.6  2006/03/06 11:39:02  brian
# - updated headers
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
