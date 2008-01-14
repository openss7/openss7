## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2008/01/14 18:04:52 $
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
# Last Modified $Date: 2008/01/14 18:04:52 $ by $Author: brian $
#
# =============================================================================

##
## General rules
##
include ../am/openss7.am

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

pkglibexec32_PROGRAMS		=

MACHINE_FLAGS			= -m32

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

test_timod_SOURCES		= $(top_srcdir)/src/test/tli/test-timod.c
test_timod_LDADD		= libxnet.la
test_timod_LDFLAGS		= $(MACHINE_FLAGS)
test_timod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-timod\"
test_timod_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_tirdwr_SOURCES		= $(top_srcdir)/src/test/tli/test-tirdwr.c
test_tirdwr_LDADD		= libxnet.la
test_tirdwr_LDFLAGS		= $(MACHINE_FLAGS)
test_tirdwr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tirdwr\"
test_tirdwr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if WITH_TLI
## WITH_TLI
pkglibexec32_PROGRAMS		+= \
				  test-timod \
				  test-tirdwr
## WITH_TLI
endif

libxnet_la_SOURCES		= $(top_srcdir)/src/lib/libxnet/pthread.c \
				  $(top_srcdir)/src/lib/libxnet/xnet.c \
				  $(top_srcdir)/gettext.h
libxnet_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/libxnet/xnet.VERSION \
				  $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
libxnet_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(srcdir)/src/include/xti -I./src/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)
libxnet_la_LIBADD		= $(STREAMS_LDADD32)

test_xnet_SOURCES		= $(top_srcdir)/src/test/xnet/test-xnet.c
test_xnet_LDADD			= libxnet.la
test_xnet_LDFLAGS		= $(MACHINE_FLAGS)
test_xnet_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-xnet\"
test_xnet_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_xnet_thr_SOURCES		= $(top_srcdir)/src/test/xnet/test-xnet.c
test_xnet_thr_LDADD		= libxnet.la
test_xnet_thr_LDFLAGS		= -lpthread $(MACHINE_FLAGS)
test_xnet_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-xnet_thr\"
test_xnet_thr_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if WITH_XNET
## WITH_XNET
lib32_LTLIBRARIES		+= \
				   libxnet.la
pkglibexec32_PROGRAMS		+= \
				   test-xnet \
				   test-xnet_thr
## WITH_XNET
endif

test_inet_raw_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_raw.c
test_inet_raw_LDADD		= $(STREAMS_LDADD32)
test_inet_raw_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_inet_raw_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_raw\"
test_inet_raw_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_inet_tcp_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_tcp.c
test_inet_tcp_LDADD		= $(STREAMS_LDADD32)
test_inet_tcp_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_inet_tcp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_tcap\"
test_inet_tcp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_inet_udp_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_udp.c
test_inet_udp_LDADD		= $(STREAMS_LDADD32)
test_inet_udp_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_inet_udp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_udp\"
test_inet_udp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_inet_sctp_SOURCES		= $(top_srcdir)/src/test/inet/test-inet_sctp.c
test_inet_sctp_LDADD		= $(STREAMS_LDADD32)
test_inet_sctp_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_inet_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_sctp\"
test_inet_sctp_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if WITH_INET
## WITH_INET
pkglibexec32_PROGRAMS		+= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp
if WITH_OPENSS7_SCTP
pkglibexec32_PROGRAMS		+= \
				  test-inet_sctp
endif

## WITH_INET
endif

test_sctp_n_SOURCES		= $(top_srcdir)/src/test/sctp/test-sctp_n.c
test_sctp_n_LDADD		= $(STREAMS_LDADD32)
test_sctp_n_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-sctp_n\"
test_sctp_n_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_sctp_n2_SOURCES		= $(top_srcdir)/src/test/sctp/test-sctp_n2.c
test_sctp_n2_LDADD		= $(STREAMS_LDADD32)
test_sctp_n2_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_n2_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-sctp_n2\"
test_sctp_n2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_sctp_t_SOURCES		= $(top_srcdir)/src/test/sctp/test-sctp_t.c
test_sctp_t_LDADD		= $(STREAMS_LDADD32)
test_sctp_t_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-sctp_t\"
test_sctp_t_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_tcpc_SOURCES		= $(top_srcdir)/src/test/inet/test-tcpc.c
test_tcpc_LDADD			= $(STREAMS_LDADD32)
test_tcpc_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_tcpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tcpc\"
test_tcpc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_tcps_SOURCES		= $(top_srcdir)/src/test/inet/test-tcps.c
test_tcps_LDADD			= $(STREAMS_LDADD32)
test_tcps_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_tcps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tcps\"
test_tcps_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_udpc_SOURCES		= $(top_srcdir)/src/test/inet/test-udpc.c
test_udpc_LDADD			= $(STREAMS_LDADD32)
test_udpc_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_udpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-udpc\"
test_udpc_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_udps_SOURCES		= $(top_srcdir)/src/test/inet/test-udps.c
test_udps_LDADD			= $(STREAMS_LDADD32)
test_udps_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_udps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-udps\"
test_udps_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if WITH_SCTP
## WITH_SCTP
pkglibexec32_PROGRAMS		+= \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_t \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps
## WITH_SCTP
endif

if WITH_SCTP2
## WITH_SCTP2
pkglibexec32_PROGRAMS		+= \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_t \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps
## WITH_SCTP2
endif

test_m2pa_SOURCES		= $(top_srcdir)/src/test/m2pa/test-m2pa.c
test_m2pa_LDADD			= $(STREAMS_LDADD32)
test_m2pa_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_m2pa_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-m2pa\"
test_m2pa_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if WITH_SIGTRAN
## WITH_SIGTRAN
pkglibexec32_PROGRAMS		+= \
				  test-m2pa
## WITH_SIGTRAN
endif

pkglibexec32_PROGRAMS		+= test-q781

test_q781_SOURCES		= $(top_srcdir)/src/test/sl/test-q781.c
test_q781_LDADD			= $(STREAMS_LDADD32)
test_q781_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_q781_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q781\"
test_q781_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x100p

test_x100p_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x100p.c
test_x100p_LDADD		= $(STREAMS_LDADD32)
test_x100p_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x100p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x100p\"
test_x100p_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p

test_x400p_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p.c
test_x400p_LDADD		= $(STREAMS_LDADD32)
test_x400p_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p\"
test_x400p_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

test_usage_SOURCES		= $(top_srcdir)/src/util/usage.c
test_usage_LDADD		= $(STREAMS_LDADD32)
test_usage_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_usage_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-usage\"
test_usage_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

if WITH_SNMP_AGENT
## WITH_SNMP_AGENT
pkglibexec32_PROGRAMS		+= test-usage
## WITH_SNMP_AGENT
endif

if PUBLIC_RELEASE
## PUBLIC_RELEASE

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

pkglibexec32_PROGRAMS		+= test-q781-pipe

test_q781_pipe_SOURCES		= $(top_srcdir)/src/test/sl/test-q781-pipe.c
test_q781_pipe_LDADD		= $(STREAMS_LDADD32)
test_q781_pipe_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_q781_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x781-pipe\"
test_q781_pipe_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-q784

test_q784_SOURCES		= $(top_srcdir)/src/test/isup/test-q784.c
test_q784_LDADD			= $(STREAMS_LDADD32)
test_q784_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_q784_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x784\"
test_q784_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-cap2

test_x400p_cap2_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-cap2.c
test_x400p_cap2_LDADD		= $(STREAMS_LDADD32)
test_x400p_cap2_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_cap2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-cap2\"
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-cap

test_x400p_cap_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-cap.c
test_x400p_cap_LDADD		= $(STREAMS_LDADD32)
test_x400p_cap_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_cap_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-cap\"
test_x400p_cap_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-hsl

test_x400p_hsl_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-hsl.c
test_x400p_hsl_LDADD		= $(STREAMS_LDADD32)
test_x400p_hsl_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_hsl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-hsl\"
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-perf

test_x400p_perf_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-perf.c
test_x400p_perf_LDADD		= $(STREAMS_LDADD32)
test_x400p_perf_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_perf_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-perf\"
test_x400p_perf_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sdl

test_x400p_sdl_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sdl.c
test_x400p_sdl_LDADD		= $(STREAMS_LDADD32)
test_x400p_sdl_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_sdl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdl\"
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sdt2

test_x400p_sdt2_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sdt2.c
test_x400p_sdt2_LDADD		= $(STREAMS_LDADD32)
test_x400p_sdt2_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_sdt2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdt2\"
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sdt

test_x400p_sdt_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sdt.c
test_x400p_sdt_LDADD		= $(STREAMS_LDADD32)
test_x400p_sdt_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_sdt_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdt\"
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

pkglibexec32_PROGRAMS		+= test-x400p-sl

test_x400p_sl_SOURCES		= $(top_srcdir)/src/test/x400p-ss7/test-x400p-sl.c
test_x400p_sl_LDADD		= $(STREAMS_LDADD32)
test_x400p_sl_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
test_x400p_sl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sl\"
test_x400p_sl_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## ! PUBLIC_RELEASE
endif

#perftest_SOURCES		= $(top_srcdir)/src/test/perftest.c
#perftest_LDADD			= $(STREAMS_LDADD32)
#perftest_LDFLAGS		= $(STREAMS_LDFLAGS32) $(MACHINE_FLAGS)
#perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"perftest\"
#perftest_CFLAGS		= $(USER_CFLAGS) $(MACHINE_FLAGS)

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

##
##  Rules for building libraries.
##
include ../am/libraries.am

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.10  2008/01/14 18:04:52  brian
## - update tests and MIBS
##
## Revision 0.9.2.9  2007/08/19 11:55:41  brian
## - move stdbool.h, obviate need for YFLAGS, general workup
##
## Revision 0.9.2.8  2007/08/13 15:22:12  brian
## - header updates, added references
##
## Revision 0.9.2.7  2007/03/13 06:17:01  brian
## - suppress errors when building 32-bit libs
##
## Revision 0.9.2.6  2007/03/12 10:19:25  brian
## - corrections
##
## Revision 0.9.2.5  2006/12/18 10:50:58  brian
## - subpackaging changes for release
##
## Revision 0.9.2.4  2006/09/29 11:50:11  brian
## - libtool library tweaks in Makefile.am
## - better rpm spec handling in *.spec.in
## - added AC_LIBTOOL_DLOPEN to configure.ac
## - updated some copyright headers
## - rationalized item in two packages
## - added manual pages, drivers and modules to new strtty package
##
## Revision 0.9.2.3  2006/09/26 00:52:24  brian
## - rationalized to embedded packages
##
## Revision 0.9.2.2  2006/09/25 20:15:45  brian
## - rationalized to strinet and strxnet
##
## Revision 0.9.2.1  2006/09/17 23:09:06  brian
## - added lib32 compatibility directories
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
