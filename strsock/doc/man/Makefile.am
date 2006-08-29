# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/08/29 11:44:04 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
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
# Last Modified $Date: 2006/08/29 11:44:04 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.2  2006/08/29 11:44:04  brian
# - added manual pages, working up docs
#
# Revision 0.9.2.1  2006/08/23 10:03:58  brian
# - started STREAMS Sockets package
#
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

##
## What we need to do here is to get the list of manual pages from the pages
## that have been dumped into any of the directories.  The configure script
## needs to find them, move them to the appropriate subdirectory, and then add
## them to these lists at configuration time.  If they are gzipped or bzipped
## we need to unzip them.  Then any external manual pages will be added to the
## distribution.  Manual pages ending in '.man' are uncooked.  Manual pages
## ending in '.[1-9]' are cooked.
##
man_MANS = \
	man3/accept.3 \
	man3/bind.3 \
	man3/connect.3 \
	man3/getpeername.3 \
	man3/getsockname.3 \
	man3/getsockopt.3 \
	man3/libsocket.3 \
	man3/listen.3 \
	man3/recv.3 \
	man3/recvfrom.3 \
	man3/recvmsg.3 \
	man3/send.3 \
	man3/sendmsg.3 \
	man3/sendto.3 \
	man3/setsockopt.3 \
	man3/shutdown.3 \
	man3/socket.3 \
	man3/socketpair.3 \
	man3/sockets.3 \
	man4/sockmod.4 \
	man4/socksys.4 \
	man5/sock2path.5 \
	man8/soconfig.8

dist_noinst_MANS = \
	man3/accept.3.man \
	man3/bind.3.man \
	man3/connect.3.man \
	man3/getpeername.3.man \
	man3/getsockname.3.man \
	man3/getsockopt.3.man \
	man3/libsocket.3.man \
	man3/listen.3.man \
	man3/recv.3.man \
	man3/recvfrom.3.man \
	man3/recvmsg.3.man \
	man3/send.3.man \
	man3/sendmsg.3.man \
	man3/sendto.3.man \
	man3/setsockopt.3.man \
	man3/shutdown.3.man \
	man3/socket.3.man \
	man3/socketpair.3.man \
	man3/sockets.3.man \
	man4/sockmod.4.man \
	man4/socksys.4.man \
	man5/sock2path.5.man \
	man8/soconfig.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
