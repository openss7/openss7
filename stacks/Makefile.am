# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2005/01/15 20:17:30 $
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
# Last Modified $Date: 2005/01/15 20:17:30 $ by $Author: brian $
#
# =============================================================================

#
#   General rules
#
include am/openss7.am

if PUBLIC_RELEASE

if RPM_BUILD_USER

libexec_PROGRAMS		= \
				  test-m2pa \
				  test-q781 \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps \
				  test-x100p \
				  test-x400p

EXTRA_PROGRAMS			= \
				  perftest \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-m2pa \
				  test-q781 \
				  test-q781-pipe \
				  test-q784 \
				  test-tcpc \
				  test-tcps \
				  test-timod \
				  test-tirdwr \
				  test-udpc \
				  test-udps \
				  test-x100p \
				  test-x400p \
				  test-x400p-cap \
				  test-x400p-cap2 \
				  test-x400p-hsl \
				  test-x400p-perf \
				  test-x400p-sdl \
				  test-x400p-sdt \
				  test-x400p-sdt2 \
				  test-x400p-sl \
				  test-xnet \
				  test-xnet_thr

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_tcpc_SOURCES		= src/test/inet/test-tcpc.c
test_tcpc_LDFLAGS		= $(STREAMS_LDADD)
test_tcpc_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_tcpc_CFLAGS		= $(USER_CFLAGS)

test_tcps_SOURCES		= src/test/inet/test-tcps.c
test_tcps_LDFLAGS		= $(STREAMS_LDADD)
test_tcps_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_tcps_CFLAGS		= $(USER_CFLAGS)

test_udpc_SOURCES		= src/test/inet/test-udpc.c
test_udpc_LDFLAGS		= $(STREAMS_LDADD)
test_udpc_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_udpc_CFLAGS		= $(USER_CFLAGS)

test_udps_SOURCES		= src/test/inet/test-udps.c
test_udps_LDFLAGS		= $(STREAMS_LDADD)
test_udps_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_udps_CFLAGS		= $(USER_CFLAGS)

#test_q784_SOURCES		= src/test/isup/test-q784.c
#test_q784_LDFLAGS		= $(STREAMS_LDADD)
#test_q784_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_q784_CFLAGS		= $(USER_CFLAGS)

test_m2pa_SOURCES		= src/test/m2pa/test-m2pa.c
test_m2pa_LDFLAGS		= $(STREAMS_LDADD)
test_m2pa_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_m2pa_CFLAGS		= $(USER_CFLAGS)

#perftest_SOURCES		= src/test/perftest.c
#perftest_LDFLAGS		= $(STREAMS_LDADD)
#perftest_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#perftest_CFLAGS		= $(USER_CFLAGS)

test_q781_SOURCES		= src/test/sl/test-q781.c
test_q781_LDFLAGS		= $(STREAMS_LDADD)
test_q781_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_q781_CFLAGS		= $(USER_CFLAGS)

#test_q781_pipe_SOURCES		= src/test/sl/test-q781-pipe.c
#test_q781_pipe_LDFLAGS		= $(STREAMS_LDADD)
#test_q781_pipe_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_q781_pipe_CFLAGS		= $(USER_CFLAGS)

test_x100p_SOURCES		= src/test/x400p-ss7/test-x100p.c
test_x100p_LDFLAGS		= $(STREAMS_LDADD)
test_x100p_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x100p_CFLAGS		= $(USER_CFLAGS)

test_x400p_SOURCES		= src/test/x400p-ss7/test-x400p.c
test_x400p_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_CFLAGS		= $(USER_CFLAGS)

#test_x400p_cap2_SOURCES	= src/test/x400p-ss7/test-x400p-cap2.c
#test_x400p_cap2_LDFLAGS	= $(STREAMS_LDADD)
#test_x400p_cap2_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_cap2_CFLAGS		= $(USER_CFLAGS)

#test_x400p_cap_SOURCES		= src/test/x400p-ss7/test-x400p-cap.c
#test_x400p_cap_LDFLAGS		= $(STREAMS_LDADD)
#test_x400p_cap_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_cap_CFLAGS		= $(USER_CFLAGS)

#test_x400p_hsl_SOURCES		= src/test/x400p-ss7/test-x400p-hsl.c
#test_x400p_hsl_LDFLAGS		= $(STREAMS_LDADD)
#test_x400p_hsl_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_hsl_CFLAGS		= $(USER_CFLAGS)

#test_x400p_perf_SOURCES	= src/test/x400p-ss7/test-x400p-perf.c
#test_x400p_perf_LDFLAGS	= $(STREAMS_LDADD)
#test_x400p_perf_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_perf_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sdl_SOURCES		= src/test/x400p-ss7/test-x400p-sdl.c
#test_x400p_sdl_LDFLAGS		= $(STREAMS_LDADD)
#test_x400p_sdl_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_sdl_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sdt2_SOURCES	= src/test/x400p-ss7/test-x400p-sdt2.c
#test_x400p_sdt2_LDFLAGS	= $(STREAMS_LDADD)
#test_x400p_sdt2_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sdt_SOURCES		= src/test/x400p-ss7/test-x400p-sdt.c
#test_x400p_sdt_LDFLAGS		= $(STREAMS_LDADD)
#test_x400p_sdt_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_sdt_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sl_SOURCES		= src/test/x400p-ss7/test-x400p-sl.c
#test_x400p_sl_LDFLAGS		= $(STREAMS_LDADD)
#test_x400p_sl_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
#test_x400p_sl_CFLAGS		= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libSS7mods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libSS7mods_a_SOURCES	= \
				  src/modules/inet/inet.c \
				  src/modules/m2pa/m2pa_sl.c \
				  src/modules/sdl/sdl.c \
				  src/modules/sdl/sdlm.c \
				  src/modules/sdl/spm.c \
				  src/modules/sdt/sdt.c \
				  src/modules/sl/sl.c \
				  src/modules/sl/sl_mux.c \
				  src/modules/sl/sm_mod.c \
				  src/modules/ss7ip/sdt_tpi.c \
				  src/modules/ss7ip/sl_tpi.c \
				  src/modules/ss7sctp/sdt_sctp.c \
				  src/modules/ss7sctp/sdl_sctp.c \
				  src/modules/ti/timod.c \
				  src/include/timod.h \
				  src/modules/ti/tirdwr.c \
				  src/include/tirdwr.h \
				  src/modules/x400p-ss7/x400p-ss7.c \
				  src/modules/x400p-ss7/x100p-ss7.c \
				  src/modules/x400p-ss7/x400pfw.h \
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h \
				  src/include/os7/compat.h \
				  strconf.h

mods_source_FILES		= $(nodist_EXTRA_libSS7mods_a_SOURCES)

nodist_libSS7mods_a_SOURCES	= $(mods_source_FILES)


#nodist_EXTRA_libSS7mods_a_SOURCES	= \
#				  src/modules/isdn/isdn.c \
#				  src/modules/isup/isup.c \
#				  src/modules/lm/lm.c \
#				  src/modules/lm/lm.h \
#				  src/modules/m2ua/m2ua.c \
#				  src/modules/mg/mg.c \
#				  src/modules/mtp/mtp.c \
#				  src/modules/mtp/mtp_npi.c \
#				  src/modules/mtp/mtp_tpi.c \
#				  src/modules/mx/mx_sdl.c \
#				  src/modules/sccp/sccp.c \
#				  src/modules/x400p-ss7/sl_x400p.c \
#				  src/modules/x400p-ss7/sl_x100p.c \
#				  src/modules/x400p-ss7/sdt_x400p.c \
#				  src/modules/x400p-ss7/sdl_x400p.c \
#				  src/modules/x400p-ss7/ch_x400p.c \
#				  src/modules/x400p-ss7/mx_x400p.c \
#				  src/modules/x400p-ss7/x400p_mx.c \
#				  src/modules/cd/cd.c \
#				  src/modules/cd/cd.h \
#				  src/modules/cd/cd_daed.c \
#				  src/modules/cd/cd_hdlc.c \
#				  src/modules/dl/dl.c \
#				  src/modules/dl/dl_dua.c \
#				  src/modules/dl/dl_gr303.c \
#				  src/modules/dl/dl_gr303ua.c \
#				  src/modules/dl/dl_gsma.c \
#				  src/modules/dl/dl_h225.c \
#				  src/modules/dl/dl_iua.c \
#				  src/modules/dl/dl_lapb.c \
#				  src/modules/dl/dl_lapd.c \
#				  src/modules/dl/dl_lapf.c \
#				  src/modules/dl/dl_v52.c \
#				  src/modules/dl/dl_v5ua.c \
#				  src/modules/hdlc/hdlc.c \
#				  src/modules/acb56/sdl_acb56.c \
#				  src/modules/acb56/sdt_acb56.c \
#				  src/modules/acb56/sl_acb56.c \
#				  src/modules/h225/h225.c \
#				  src/modules/m2tp/m2tp.c \
#				  src/modules/mtp/mtp_min.c \
#				  src/modules/sdl/cdi.c \
#				  src/modules/sdl/cdi_mod.c \
#				  src/modules/sdt/sdtmux.c \
#				  src/modules/sl/slm.c \
#				  src/modules/sscop/sscop.c \
#				  src/modules/sscop/sscop2.c \
#				  src/modules/tcap/tcap.c \
#				  src/modules/tcap/tc.c \
#				  src/modules/tcap/tr.c \
#				  src/modules/x400p-ss7/crc.c

libSS7mods_a_DEPENDENCIES	= config.h strconf.h
libSS7mods_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules -I$(srcdir)/src/modules -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7mods_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7mods_a_OBJECTS:		$(libSS7mods_a_DEPENDENCIES)

KERNEL_MODULES			= $(libSS7mods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

endif

else

if RPM_BUILD_USER

libexec_PROGRAMS		= \
				  perftest \
				  test-m2pa \
				  test-q781 \
				  test-q781-pipe \
				  test-q784 \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps \
				  test-x100p \
				  test-x400p \
				  test-x400p-cap \
				  test-x400p-cap2 \
				  test-x400p-hsl \
				  test-x400p-perf \
				  test-x400p-sdl \
				  test-x400p-sdt \
				  test-x400p-sdt2 \
				  test-x400p-sl

EXTRA_PROGRAMS			= \
				  perftest \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-m2pa \
				  test-q781 \
				  test-q781-pipe \
				  test-q784 \
				  test-tcpc \
				  test-tcps \
				  test-timod \
				  test-tirdwr \
				  test-udpc \
				  test-udps \
				  test-x100p \
				  test-x400p \
				  test-x400p-cap \
				  test-x400p-cap2 \
				  test-x400p-hsl \
				  test-x400p-perf \
				  test-x400p-sdl \
				  test-x400p-sdt \
				  test-x400p-sdt2 \
				  test-x400p-sl \
				  test-xnet \
				  test-xnet_thr

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= strconf.h
@PACKAGE_TARNAME@_mknod_LDDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_tcpc_SOURCES		= src/test/inet/test-tcpc.c
test_tcpc_LDFLAGS		= $(STREAMS_LDADD)
test_tcpc_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_tcpc_CFLAGS		= $(USER_CFLAGS)

test_tcps_SOURCES		= src/test/inet/test-tcps.c
test_tcps_LDFLAGS		= $(STREAMS_LDADD)
test_tcps_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_tcps_CFLAGS		= $(USER_CFLAGS)

test_udpc_SOURCES		= src/test/inet/test-udpc.c
test_udpc_LDFLAGS		= $(STREAMS_LDADD)
test_udpc_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_udpc_CFLAGS		= $(USER_CFLAGS)

test_udps_SOURCES		= src/test/inet/test-udps.c
test_udps_LDFLAGS		= $(STREAMS_LDADD)
test_udps_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_udps_CFLAGS		= $(USER_CFLAGS)

test_q784_SOURCES		= src/test/isup/test-q784.c
test_q784_LDFLAGS		= $(STREAMS_LDADD)
test_q784_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_q784_CFLAGS		= $(USER_CFLAGS)

test_m2pa_SOURCES		= src/test/m2pa/test-m2pa.c
test_m2pa_LDFLAGS		= $(STREAMS_LDADD)
test_m2pa_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_m2pa_CFLAGS		= $(USER_CFLAGS)

perftest_SOURCES		= src/test/perftest.c
perftest_LDFLAGS		= $(STREAMS_LDADD)
perftest_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
perftest_CFLAGS			= $(USER_CFLAGS)

test_q781_SOURCES		= src/test/sl/test-q781.c
test_q781_LDFLAGS		= $(STREAMS_LDADD)
test_q781_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_q781_CFLAGS		= $(USER_CFLAGS)

test_q781_pipe_SOURCES		= src/test/sl/test-q781-pipe.c
test_q781_pipe_LDFLAGS		= $(STREAMS_LDADD)
test_q781_pipe_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_q781_pipe_CFLAGS		= $(USER_CFLAGS)

test_x100p_SOURCES		= src/test/x400p-ss7/test-x100p.c
test_x100p_LDFLAGS		= $(STREAMS_LDADD)
test_x100p_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x100p_CFLAGS		= $(USER_CFLAGS)

test_x400p_SOURCES		= src/test/x400p-ss7/test-x400p.c
test_x400p_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_CFLAGS		= $(USER_CFLAGS)

test_x400p_cap2_SOURCES		= src/test/x400p-ss7/test-x400p-cap2.c
test_x400p_cap2_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_cap2_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS)

test_x400p_cap_SOURCES		= src/test/x400p-ss7/test-x400p-cap.c
test_x400p_cap_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_cap_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_cap_CFLAGS		= $(USER_CFLAGS)

test_x400p_hsl_SOURCES		= src/test/x400p-ss7/test-x400p-hsl.c
test_x400p_hsl_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_hsl_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS)

test_x400p_perf_SOURCES		= src/test/x400p-ss7/test-x400p-perf.c
test_x400p_perf_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_perf_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_perf_CFLAGS		= $(USER_CFLAGS)

test_x400p_sdl_SOURCES		= src/test/x400p-ss7/test-x400p-sdl.c
test_x400p_sdl_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_sdl_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS)

test_x400p_sdt2_SOURCES		= src/test/x400p-ss7/test-x400p-sdt2.c
test_x400p_sdt2_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_sdt2_CPPFLAGS	= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS)

test_x400p_sdt_SOURCES		= src/test/x400p-ss7/test-x400p-sdt.c
test_x400p_sdt_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_sdt_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS)

test_x400p_sl_SOURCES		= src/test/x400p-ss7/test-x400p-sl.c
test_x400p_sl_LDFLAGS		= $(STREAMS_LDADD)
test_x400p_sl_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_x400p_sl_CFLAGS		= $(USER_CFLAGS)

endif

if RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libSS7mods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= libSS7sscop.a libSS7m3ua.a libSS7sua.a libSS7tua.a libSS7tali.a

nodist_EXTRA_libSS7mods_a_SOURCES	= \
				  src/modules/cd/cd.c \
				  src/modules/cd/cd_daed.c \
				  src/modules/cd/cd_hdlc.c \
				  src/modules/dl/dl_dua.c \
				  src/modules/dl/dl_gr303.c \
				  src/modules/dl/dl_gr303ua.c \
				  src/modules/dl/dl_gsma.c \
				  src/modules/dl/dl_h225.c \
				  src/modules/dl/dl_iua.c \
				  src/modules/dl/dl_lapb.c \
				  src/modules/dl/dl_lapd.c \
				  src/modules/dl/dl_lapf.c \
				  src/modules/dl/dl_v52.c \
				  src/modules/dl/dl_v5ua.c \
				  src/modules/hdlc/hdlc.c \
				  src/modules/inet/inet.c \
				  src/modules/isdn/isdn.c \
				  src/modules/isup/isup.c \
				  src/modules/lm/lm.c \
				  src/modules/m2pa/m2pa_sl.c \
				  src/modules/m2ua/m2ua.c \
				  src/modules/mg/mg.c \
				  src/modules/mtp/mtp.c \
				  src/modules/mtp/mtp_npi.c \
				  src/modules/mtp/mtp_tpi.c \
				  src/modules/mx/mx_sdl.c \
				  src/modules/sccp/sccp.c \
				  src/modules/sdl/sdl.c \
				  src/modules/sdl/sdlm.c \
				  src/modules/sdl/spm.c \
				  src/modules/sdt/sdt.c \
				  src/modules/sl/sl.c \
				  src/modules/sl/sl_mux.c \
				  src/modules/sl/sm_mod.c \
				  src/modules/ss7ip/sdt_tpi.c \
				  src/modules/ss7ip/sl_tpi.c \
				  src/modules/ss7sctp/sdt_sctp.c \
				  src/modules/ss7sctp/sdl_sctp.c \
				  src/modules/ti/timod.c \
				  src/include/timod.h \
				  src/modules/ti/tirdwr.c \
				  src/include/tirdwr.h \
				  src/modules/x400p-ss7/x400p-ss7.c \
				  src/modules/x400p-ss7/x100p-ss7.c \
				  src/modules/x400p-ss7/sl_x400p.c \
				  src/modules/x400p-ss7/sl_x100p.c \
				  src/modules/x400p-ss7/sdt_x400p.c \
				  src/modules/x400p-ss7/sdl_x400p.c \
				  src/modules/x400p-ss7/ch_x400p.c \
				  src/modules/x400p-ss7/mx_x400p.c \
				  src/modules/x400p-ss7/x400p_mx.c \
				  src/modules/x400p-ss7/x400pfw.h \
				  src/modules/lm/lm.h \
				  src/modules/cd/cd.h \
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h \
				  src/include/os7/compat.h \
				  strconf.h

if WITH_LIS
nodist_EXTRA_libSS7mods_a_SOURCES	+= \
				  src/modules/dl/dl.c
endif

mods_source_FILES		= $(nodist_EXTRA_libSS7mods_a_SOURCES)

nodist_libSS7mods_a_SOURCES	= $(mods_source_FILES)

#nodist_EXTRA_libSS7mods_a_SOURCES	= \
#				  src/modules/acb56/sdl_acb56.c \
#				  src/modules/acb56/sdt_acb56.c \
#				  src/modules/acb56/sl_acb56.c \
#				  src/modules/h225/h225.c \
#				  src/modules/m2tp/m2tp.c \
#				  src/modules/mtp/mtp_min.c \
#				  src/modules/sdl/cdi.c \
#				  src/modules/sdl/cdi_mod.c \
#				  src/modules/sdt/sdtmux.c \
#				  src/modules/sl/slm.c \
#				  src/modules/sscop/sscop.c \
#				  src/modules/sscop/sscop2.c \
#				  src/modules/tcap/tcap.c \
#				  src/modules/tcap/tc.c \
#				  src/modules/tcap/tr.c \
#				  src/modules/x400p-ss7/crc.c

libSS7mods_a_DEPENDENCIES	= config.h strconf.h
libSS7mods_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules -I$(srcdir)/src/modules -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7mods_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7mods_a_OBJECTS:		$(libSS7mods_a_DEPENDENCIES)

nodist_EXTRA_libSS7m3ua_a_SOURCES = \
				  src/modules/m3ua/m3ua_as.c \
				  src/modules/m3ua/m3ua_asp.c \
				  src/modules/m3ua/m3ua.c \
				  src/modules/m3ua/m3ua_lm.c \
				  src/modules/m3ua/m3ua_lower.c \
				  src/modules/m3ua/m3ua_msg.c \
				  src/modules/m3ua/m3ua_mtp.c \
				  src/modules/m3ua/m3ua_mtpp.c \
				  src/modules/m3ua/m3ua_mtpu.c \
				  src/modules/m3ua/m3ua_nsp.c \
				  src/modules/m3ua/m3ua_sg.c \
				  src/modules/m3ua/m3ua_sgp.c \
				  src/modules/m3ua/m3ua_spp.c \
				  src/modules/m3ua/m3ua_ss7.c \
				  src/modules/m3ua/m3ua_tsp.c \
				  src/modules/m3ua/mtp_sm.c \
				  src/modules/m3ua/m3ua_asp.h \
				  src/modules/m3ua/m3ua_data.h \
				  src/modules/m3ua/m3ua.h \
				  src/modules/m3ua/m3ua_lm.h \
				  src/modules/m3ua/m3ua_msg.h \
				  src/modules/m3ua/m3ua_mtp.h \
				  src/modules/m3ua/m3ua_nsp.h \
				  src/modules/m3ua/m3ua_tsp.h \
				  strconf.h
m3ua_source_FILES		= $(nodist_EXTRA_libSS7m3ua_a_SOURCES)
nodist_libSS7m3ua_a_SOURCES	= $(m3ua_source_FILES)

libSS7m3ua_a_DEPENDENCIES	= config.h strconf.h
libSS7m3ua_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules/m3ua -I$(srcdir)/src/modules/m3ua -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7m3ua_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7m3ua_a_OBJECTS:		$(libSS7m3ua_a_DEPENDENCIES)

streams-m3ua.o: $(libSS7m3ua_a_OBJECTS)
	$(LD) -r -o $@ $^

nodist_EXTRA_libSS7sscop_a_SOURCES = \
				  src/modules/sscop/sscop_ip.c \
				  src/modules/sscop/sscop_n2.c \
				  src/modules/sscop/sscop_n.c \
				  src/modules/sscop/sscop_n_user.c \
				  src/modules/sscop/sscop_recv.c \
				  src/modules/sscop/sscop_t.c \
				  src/modules/sscop/sscop_time.c \
				  src/modules/sscop/sscop_t_user.c \
				  src/modules/sscop/sscop_udp.c \
				  src/modules/sscop/sccop_user.h \
				  src/modules/sscop/sscop.h \
				  src/modules/sscop/sscop_msgs.h \
				  src/modules/sscop/sscop_n_prov.h \
				  src/modules/sscop/sscop_prov.h \
				  src/modules/sscop/sscop_send.h \
				  src/modules/sscop/sscop_time.h \
				  src/modules/sscop/sscop_t_prov.h \
				  src/modules/sscop/sscop_user.h \
				  strconf.h
sscop_source_FILES		= $(nodist_EXTRA_libSS7sscop_a_SOURCES)
nodist_libSS7sscop_a_SOURCES	= $(sscop_source_FILES)

libSS7sscop_a_DEPENDENCIES	= config.h strconf.h
libSS7sscop_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules/sscop -I$(srcdir)/src/modules/sscop -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7sscop_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7sscop_a_OBJECTS:		$(libSS7sscop_a_DEPENDENCIES)

streams-sscop.o: $(libSS7sscop_a_OBJECTS)
	$(LD) -r -o $@ $^

nodist_EXTRA_libSS7sua_a_SOURCES = \
				  src/modules/sua/sua_asp.c \
				  src/modules/sua/sua.c \
				  src/modules/sua/sua_lm.c \
				  src/modules/sua/sua_lower.c \
				  src/modules/sua/sua_msg.c \
				  src/modules/sua/sua_nsp.c \
				  src/modules/sua/sua_sccp.c \
				  src/modules/sua/sua_sgp.c \
				  src/modules/sua/sua_sm.c \
				  src/modules/sua/sua_spp.c \
				  src/modules/sua/sua_ss7.c \
				  src/modules/sua/sua_tsp.c \
				  src/modules/sua/sua_asp.h \
				  src/modules/sua/sua_ctrl.h \
				  src/modules/sua/sua_data.h \
				  src/modules/sua/sua.h \
				  src/modules/sua/sua_lm.h \
				  src/modules/sua/sua_msg.h \
				  src/modules/sua/sua_nsp.h \
				  src/modules/sua/sua_sccp.h \
				  src/modules/sua/sua_send.h \
				  src/modules/sua/sua_tsp.h \
				  strconf.h
sua_source_FILES		= $(nodist_EXTRA_libSS7sua_a_SOURCES)
nodist_libSS7sua_a_SOURCES	= $(sua_source_FILES)

libSS7sua_a_DEPENDENCIES	= config.h strconf.h
libSS7sua_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules/sua -I$(srcdir)/src/modules/sua -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7sua_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7sua_a_OBJECTS:		$(libSS7sua_a_DEPENDENCIES)

streams-sua.o: $(libSS7sua_a_OBJECTS)
	$(LD) -r -o $@ $^

nodist_EXTRA_libSS7tua_a_SOURCES = \
				  src/modules/tua/tua_asp.c \
				  src/modules/tua/tua.c \
				  src/modules/tua/tua_sgp.c \
				  src/modules/tua/tua_spp.c \
				  src/modules/tua/tua_ss7.c \
				  src/modules/tua/tua_asp.h \
				  src/modules/tua/tua_data.h \
				  src/modules/tua/tua.h \
				  src/modules/tua/tua_msg.h \
				  strconf.h
tua_source_FILES		= $(nodist_EXTRA_libSS7tua_a_SOURCES)
nodist_libSS7tua_a_SOURCES	= $(tua_source_FILES)

libSS7tua_a_DEPENDENCIES	= config.h strconf.h
libSS7tua_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules/tua -I$(srcdir)/src/modules/tua -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7tua_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7tua_a_OBJECTS:		$(libSS7tua_a_DEPENDENCIES)

streams-tua.o: $(libSS7tua_a_OBJECTS)
	$(LD) -r -o $@ $^

nodist_EXTRA_libSS7tali_a_SOURCES = \
				  src/modules/tali/tali.c \
				  src/modules/tali/tali_isup.c \
				  src/modules/tali/tali_lm.c \
				  src/modules/tali/tali_mtp.c \
				  src/modules/tali/tali_sccp.c \
				  src/modules/tali/tali_data.h \
				  src/modules/tali/tali.h \
				  src/modules/tali/tali_msgs.h \
				  strconf.h
tali_source_FILES		= $(nodist_EXTRA_libSS7tali_a_SOURCES)
nodist_libSS7tali_a_SOURCES	= $(tali_source_FILES)

libSS7tali_a_DEPENDENCIES	= config.h strconf.h
libSS7tali_a_CPPFLAGS		= -I./src/include/os7 -I$(srcdir)/src/include/os7 -I./src/modules/tali -I$(srcdir)/src/modules/tali -imacros strconf.h $(SS7_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_MODFLAGS)
libSS7tali_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7tali_a_OBJECTS:		$(libSS7tali_a_DEPENDENCIES)

streams-tali.o: $(libSS7tali_a_OBJECTS)
	$(LD) -r -o $@ $^

KERNEL_MODULES			= $(libSS7mods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= streams-m3ua.o streams-sscop.o streams-sua.o streams-tua.o streams-tali.o

endif

endif

if RPM_BUILD_USER

test_timod_SOURCES		= src/test/tli/test-timod.c
test_timod_LDFLAGS		= $(STREAMS_LDADD) -lxnet
test_timod_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_timod_CFLAGS		= $(USER_CFLAGS)

test_tirdwr_SOURCES		= src/test/tli/test-tirdwr.c
test_tirdwr_LDFLAGS		= $(STREAMS_LDADD) -lxnet
test_tirdwr_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_tirdwr_CFLAGS		= $(USER_CFLAGS)

if WITH_TLI
libexec_PROGRAMS		+= \
				  test-timod \
				  test-tirdwr
endif

test_inet_raw_SOURCES		= src/test/inet/test-inet_raw.c
test_inet_raw_LDFLAGS		= $(STREAMS_LDADD)
test_inet_raw_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_inet_raw_CFLAGS		= $(USER_CFLAGS)

test_inet_tcp_SOURCES		= src/test/inet/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= $(STREAMS_LDADD)
test_inet_tcp_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)

test_inet_udp_SOURCES		= src/test/inet/test-inet_udp.c
test_inet_udp_LDFLAGS		= $(STREAMS_LDADD)
test_inet_udp_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_inet_udp_CFLAGS		= $(USER_CFLAGS)

test_xnet_SOURCES		= src/test/xnet/test-xnet.c
test_xnet_LDFLAGS		= $(STREAMS_LDADD) -lxnet
test_xnet_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_xnet_CFLAGS		= $(USER_CFLAGS)

test_xnet_thr_SOURCES		= src/test/xnet/test-xnet.c
test_xnet_thr_LDFLAGS		= $(STREAMS_LDADD) -lpthread -lxnet
test_xnet_thr_CPPFLAGS		= $(SS7_INCLUDES) $(USER_CPPFLAGS)
test_xnet_thr_CFLAGS		= $(USER_CFLAGS)

if WITH_INET
libexec_PROGRAMS		+= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp
endif

endif

if RPM_BUILD_KERNEL

if WITH_TLI
nodist_libSS7mods_a_SOURCES	+= \
				  src/modules/ti/timod.c \
				  src/include/timod.h \
				  src/modules/ti/tirdwr.c \
				  src/include/tirdwr.h
endif

if WITH_INET
nodist_libSS7mods_a_SOURCES	+= \
				  src/modules/inet/inet.c
endif

endif

ss7_source_FILES		= $(mods_source_FILES) \
				  $(sscop_source_FILES) \
				  $(m3ua_source_FILES) \
				  $(sua_source_FILES) \
				  $(tua_source_FILES) \
				  $(tali_source_FILES)

EXTRA_DOCS = \
	doc/drafts/isua/draft-bidulock-sigtran-isua-02.pdf \
	doc/drafts/isua/draft-bidulock-sigtran-isua-02.ps \
	doc/drafts/isua/draft-bidulock-sigtran-isua-02.txt \
	doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-05.pdf \
	doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-05.ps \
	doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-05.txt \
	doc/drafts/m2pa/draft-george-sigtran-m2pa-interop-01.txt \
	doc/drafts/m2pa/draft-ietf-sigtran-m2pa-11.txt \
	doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-01.pdf \
	doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-01.ps \
	doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-01.txt \
	doc/drafts/tua/draft-bidulock-sigtran-tua-03.pdf \
	doc/drafts/tua/draft-bidulock-sigtran-tua-03.ps \
	doc/drafts/tua/draft-bidulock-sigtran-tua-03.txt \
	doc/drafts/ua/draft-bidulock-sigtran-aspext-03.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-aspext-03.ps \
	doc/drafts/ua/draft-bidulock-sigtran-aspext-03.txt \
	doc/drafts/ua/draft-bidulock-sigtran-corid-03.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-corid-03.ps \
	doc/drafts/ua/draft-bidulock-sigtran-corid-03.txt \
	doc/drafts/ua/draft-bidulock-sigtran-datamodel-00.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-loadgrp-03.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-loadgrp-03.ps \
	doc/drafts/ua/draft-bidulock-sigtran-loadgrp-03.txt \
	doc/drafts/ua/draft-bidulock-sigtran-loadsel-03.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-loadsel-03.ps \
	doc/drafts/ua/draft-bidulock-sigtran-loadsel-03.txt \
	doc/drafts/ua/draft-bidulock-sigtran-regext-03.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-regext-03.ps \
	doc/drafts/ua/draft-bidulock-sigtran-regext-03.txt \
	doc/drafts/ua/draft-bidulock-sigtran-sginfo-04.pdf \
	doc/drafts/ua/draft-bidulock-sigtran-sginfo-04.ps \
	doc/drafts/ua/draft-bidulock-sigtran-sginfo-04.txt

EXTRA_TEX = \
	scripts/texinfo.cnf \
	scripts/texi/args.texi.in \
	scripts/texi/fdl.texi \
	scripts/texi/gpl.texi \
	scripts/texi/install.texi \
	scripts/texi/lesser.texi


EXTRA_DIST			= $(ss7_source_FILES) $(EXTRA_DOCS) $(EXTRA_TEX)
dist_noinst_SCRIPTS		= configure.nexusware
EXTRA_SCRIPTS			=

KERNEL_MODCONF			= conf.modules
KERNEL_SUBDIR			= openss7
KMODCONF_EXT			= openss7

include am/kernel.am

include am/libraries.am

if RPM_BUILD_USER

if WITH_XNET
lib_LTLIBRARIES			= libxnet.la
libexec_PROGRAMS		+= \
				  test-xnet \
				  test-xnet_thr
libxnet_la_SOURCES		= src/lib/libxnet/xnet.c gettext.h
libxnet_la_LDFLAGS		= -no-undefined -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(srcdir)/src/lib/libxnet -I./src/lib/libxnet $(SS7_INCLUDES) $(USER_CPPFLAGS) -I$(srcdir)/src/include/xti -I./src/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS)
libxnet_la_LIBADD		= $(STREAMS_LDADD)
endif

endif

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

STRMAKENODES			= @PACKAGE_TARNAME@_mknod

include am/strconf.am

if WITH_INET

install-devices: $(STRMAKENODES) $(INET_MKDEV)
	./$(STRMAKENODES)
	DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)

uninstall-devices:
	./$(STRMAKENODES) --remove
	DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)

else

install-devices: $(STRMAKENODES)
	./$(STRMAKENODES)

uninstall-devices:
	./$(STRMAKENODES) --remove

endif

dist_noinst_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV) 

SUBDIRS = doc src intl m4 po

#
#   Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion $(kversion)"
AM_SRPMFLAGS =

include am/rpm.am

dist_check_SCRIPTS	= $(srcdir)/scripts/check_modules

TESTS			= $(srcdir)/scripts/check_modules
TESTS_ENVIRONMENT	= KERNEL_MODULES='$(KERNEL_MODULES)' KERNEL_SYSMAP='$(DESTDIR)$(ksysmap)' $(SHELL)

# vim: ft=automake
