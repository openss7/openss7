# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# =============================================================================

include ../am/openss7.am

bin_PROGRAMS = iperf

LIBCOMPAT_LDADDS = @STRIP_BEGIN@ \
		   $(top_builddir)/compat/libcompat.a \
		   @STRIP_END@

AM_CPPFLAGS = @STRIP_BEGIN@ \
	      -I$(top_srcdir)/include \
	      @STRIP_END@

iperf_LDFLAGS = @CFLAGS@ @PTHREAD_CFLAGS@ @WEB100_CFLAGS@ @DEFS@

iperf_SOURCES = \
		Client.cpp \
		Extractor.c \
		Launch.cpp \
		List.cpp \
		Listener.cpp \
		Locale.c \
		PerfSocket.cpp \
		ReportCSV.c \
		ReportDefault.c \
		Reporter.c \
		Server.cpp \
		Settings.cpp \
		SocketAddr.c \
		gnu_getopt.c \
		gnu_getopt_long.c \
		main.cpp \
		service.c \
		sockets.c \
		stdio.c \
		tcp_window_size.c

iperf_DEPENDENCIES = \
		../include/Client.hpp \
		../include/Condition.h \
		../include/Extractor.h \
		../include/List.h \
		../include/Listener.hpp \
		../include/Locale.h \
		../include/Mutex.h \
		../include/PerfSocket.hpp \
		../include/Reporter.h \
		../include/Server.hpp \
		../include/Settings.hpp \
		../include/SocketAddr.h \
		../include/Thread.h \
		../include/Timestamp.hpp \
		../include/delay.hpp \
		../include/gettimeofday.h \
		../include/gnu_getopt.h \
		../include/headers.h \
		../include/inet_aton.h \
		../include/report_CSV.h \
		../include/report_default.h \
		../include/service.h \
		../include/snprintf.h \
		../include/util.h \
		../include/version.h

iperf_LDADD = $(LIBCOMPAT_LDADDS)

# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
