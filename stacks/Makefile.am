# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/01/19 22:56:32 $
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
# Last Modified $Date: 2004/01/19 22:56:32 $ by $Author: brian $
#
# =============================================================================

libexec_PROGRAMS		= \
				  perftest \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-m2pa \
				  test-q781 \
				  test-q781-pipe \
				  test-q784 \
				  test-sctp \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t \
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
				  test-sctp \
				  test-sctp \
				  test-sctp_n \
				  test-sctp_nc \
				  test-sctp_ns \
				  test-sctp_t \
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

test_inet_raw_SOURCES		= src/test/inet/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/inet/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/inet/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@

test_tcpc_SOURCES		= src/test/inet/test-tcpc.c
test_tcpc_LDFLAGS		= -lLiS
test_tcpc_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_tcpc_CFLAGS		= @USER_CFLAGS@

test_tcps_SOURCES		= src/test/inet/test-tcps.c
test_tcps_LDFLAGS		= -lLiS
test_tcps_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_tcps_CFLAGS		= @USER_CFLAGS@

test_udpc_SOURCES		= src/test/inet/test-udpc.c
test_udpc_LDFLAGS		= -lLiS
test_udpc_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_udpc_CFLAGS		= @USER_CFLAGS@

test_udps_SOURCES		= src/test/inet/test-udps.c
test_udps_LDFLAGS		= -lLiS
test_udps_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_udps_CFLAGS		= @USER_CFLAGS@

test_q784_SOURCES		= src/test/isup/test-q784.c
test_q784_LDFLAGS		= -lLiS
test_q784_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_q784_CFLAGS		= @USER_CFLAGS@

test_m2pa_SOURCES		= src/test/m2pa/test-m2pa.c
test_m2pa_LDFLAGS		= -lLiS
test_m2pa_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_m2pa_CFLAGS		= @USER_CFLAGS@

perftest_SOURCES		= src/test/perftest.c
perftest_LDFLAGS		= -lLiS
perftest_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
perftest_CFLAGS			= @USER_CFLAGS@

test_sctp_SOURCES		= src/test/sctp/test-sctp.c
test_sctp_LDFLAGS		= -lLiS
test_sctp_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_sctp_CFLAGS		= @USER_CFLAGS@

test_sctp_n_SOURCES		= src/test/sctp/test-sctp_n.c
test_sctp_n_LDFLAGS		= -lLiS
test_sctp_n_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_sctp_n_CFLAGS		= @USER_CFLAGS@

test_sctp_nc_SOURCES		= src/test/sctp/test-sctp_nc.c
test_sctp_nc_LDFLAGS		= -lLiS
test_sctp_nc_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_sctp_nc_CFLAGS		= @USER_CFLAGS@

test_sctp_ns_SOURCES		= src/test/sctp/test-sctp_ns.c
test_sctp_ns_LDFLAGS		= -lLiS
test_sctp_ns_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_sctp_ns_CFLAGS		= @USER_CFLAGS@

test_sctp_t_SOURCES		= src/test/sctp/test-sctp_t.c
test_sctp_t_LDFLAGS		= -lLiS
test_sctp_t_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_sctp_t_CFLAGS		= @USER_CFLAGS@

test_q781_SOURCES		= src/test/sl/test-q781.c
test_q781_LDFLAGS		= -lLiS
test_q781_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_q781_CFLAGS		= @USER_CFLAGS@

test_q781_pipe_SOURCES		= src/test/sl/test-q781-pipe.c
test_q781_pipe_LDFLAGS		= -lLiS
test_q781_pipe_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_q781_pipe_CFLAGS		= @USER_CFLAGS@

test_x100p_SOURCES		= src/test/x400p-ss7/test-x100p.c
test_x100p_LDFLAGS		= -lLiS
test_x100p_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x100p_CFLAGS		= @USER_CFLAGS@

test_x400p_SOURCES		= src/test/x400p-ss7/test-x400p.c
test_x400p_LDFLAGS		= -lLiS
test_x400p_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_CFLAGS		= @USER_CFLAGS@

test_x400p_cap2_SOURCES		= src/test/x400p-ss7/test-x400p-cap2.c
test_x400p_cap2_LDFLAGS		= -lLiS
test_x400p_cap2_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_cap2_CFLAGS		= @USER_CFLAGS@

test_x400p_cap_SOURCES		= src/test/x400p-ss7/test-x400p-cap.c
test_x400p_cap_LDFLAGS		= -lLiS
test_x400p_cap_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_cap_CFLAGS		= @USER_CFLAGS@

test_x400p_hsl_SOURCES		= src/test/x400p-ss7/test-x400p-hsl.c
test_x400p_hsl_LDFLAGS		= -lLiS
test_x400p_hsl_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_hsl_CFLAGS		= @USER_CFLAGS@

test_x400p_perf_SOURCES		= src/test/x400p-ss7/test-x400p-perf.c
test_x400p_perf_LDFLAGS		= -lLiS
test_x400p_perf_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_perf_CFLAGS		= @USER_CFLAGS@

test_x400p_sdl_SOURCES		= src/test/x400p-ss7/test-x400p-sdl.c
test_x400p_sdl_LDFLAGS		= -lLiS
test_x400p_sdl_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_sdl_CFLAGS		= @USER_CFLAGS@

test_x400p_sdt2_SOURCES		= src/test/x400p-ss7/test-x400p-sdt2.c
test_x400p_sdt2_LDFLAGS		= -lLiS
test_x400p_sdt2_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_sdt2_CFLAGS		= @USER_CFLAGS@

test_x400p_sdt_SOURCES		= src/test/x400p-ss7/test-x400p-sdt.c
test_x400p_sdt_LDFLAGS		= -lLiS
test_x400p_sdt_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_sdt_CFLAGS		= @USER_CFLAGS@

test_x400p_sl_SOURCES		= src/test/x400p-ss7/test-x400p-sl.c
test_x400p_sl_LDFLAGS		= -lLiS
test_x400p_sl_CPPFLAGS		= @SS7_INCLUDES@ @USER_CPPFLAGS@
test_x400p_sl_CFLAGS		= @USER_CFLAGS@

ADDON_LIBS			= libSS7sctp.a
ADDON_OBJS			= streams-sctp.o

noinst_LIBRARIES		= libSS7mods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= libSS7sctp.a libSS7sscop.a libSS7m3ua.a libSS7sua.a libSS7tua.a libSS7tali.a

libSS7mods_a_SOURCES		= \
				  src/modules/cd/cd.c \
				  src/modules/cd/cd_daed.c \
				  src/modules/cd/cd_hdlc.c \
				  src/modules/dl/dl.c \
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
				  src/include/os7/allocb.h \
				  src/include/os7/bufpool.h \
				  src/include/os7/bufq.h \
				  src/include/os7/debug.h \
				  src/include/os7/lock.h \
				  src/include/os7/priv.h \
				  src/include/os7/queue.h \
				  src/include/os7/timer.h \
				  ./strconf.h

EXTRA_libSS7mods_a_SOURCES	= \
				  src/modules/acb56/sdl_acb56.c \
				  src/modules/acb56/sdt_acb56.c \
				  src/modules/acb56/sl_acb56.c \
				  src/modules/h225/h225.c \
				  src/modules/m2tp/m2tp.c \
				  src/modules/mtp/mtp_min.c \
				  src/modules/sdl/cdi.c \
				  src/modules/sdl/cdi_mod.c \
				  src/modules/sdt/sdtmux.c \
				  src/modules/sl/slm.c \
				  src/modules/ss7sctp/sdl_sctp.c \
				  src/modules/sscop/sscop.c \
				  src/modules/sscop/sscop2.c \
				  src/modules/tcap/tcap.c \
				  src/modules/tcap/tc.c \
				  src/modules/tcap/tr.c \
				  src/modules/x400p-ss7/ch_x400p.c \
				  src/modules/x400p-ss7/crc.c \
				  src/modules/x400p-ss7/mx_x400p.c \
				  src/modules/x400p-ss7/sdl_x400p.c \
				  src/modules/x400p-ss7/sdt_x400p.c \
				  src/modules/x400p-ss7/sl_x100p.c \
				  src/modules/x400p-ss7/sl_x400p.c \
				  src/modules/x400p-ss7/x100p-ss7.c \
				  src/modules/x400p-ss7/x400p_mx.c \
				  src/modules/x400p-ss7/x400p-ss7.c

libSS7mods_a_DEPENDENCIES	= ./config.h ./strconf.h
libSS7mods_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules -I@srcdir@/src/modules -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7mods_a_CFLAGS		= @KERNEL_CFLAGS@

libSS7sctp_a_SOURCES		= \
				  src/modules/sctp/sctp.c \
				  src/modules/sctp/sctp_cache.c \
				  src/modules/sctp/sctp_cookie.c \
				  src/modules/sctp/sctp_hash.c \
				  src/modules/sctp/sctp_input.c \
				  src/modules/sctp/sctp_msg.c \
				  src/modules/sctp/sctp_n.c \
				  src/modules/sctp/sctp_output.c \
				  src/modules/sctp/sctp_route.c \
				  src/modules/sctp/sctp_t.c \
				  src/modules/sctp/sctp_cache.h \
				  src/modules/sctp/sctp_cookie.h \
				  src/modules/sctp/sctp_defs.h \
				  src/modules/sctp/sctp.h \
				  src/modules/sctp/sctp_hash.h \
				  src/modules/sctp/sctp_input.h \
				  src/modules/sctp/sctp_msg.h \
				  src/modules/sctp/sctp_n.h \
				  src/modules/sctp/sctp_output.h \
				  src/modules/sctp/sctp_route.h \
				  src/modules/sctp/sctp_t.h \
				  ./strconf.h
libSS7sctp_a_DEPENDENCIES	= 
libSS7sctp_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules/sctp -I@srcdir@/src/modules/sctp -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7sctp_a_CFLAGS		= @KERNEL_CFLAGS@

streams-sctp.o: $(libSS7sctp_a_OBJECTS)
	$(LD) -r -o $@ $^

libSS7m3ua_a_SOURCES		= \
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
				  ./strconf.h

libSS7m3ua_a_DEPENDENCIES	= 
libSS7m3ua_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules/m3ua -I@srcdir@/src/modules/m3ua -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7m3ua_a_CFLAGS		= @KERNEL_CFLAGS@

streams-m3ua.o: $(libSS7m3ua_a_OBJECTS)
	$(LD) -r -o $@ $^

libSS7sscop_a_SOURCES		= \
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
				  ./strconf.h

libSS7sscop_a_DEPENDENCIES	= 
libSS7sscop_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules/sscop -I@srcdir@/src/modules/sscop -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7sscop_a_CFLAGS		= @KERNEL_CFLAGS@

streams-sscop.o: $(libSS7sscop_a_OBJECTS)
	$(LD) -r -o $@ $^

libSS7sua_a_SOURCES		= \
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
				  ./strconf.h

libSS7sua_a_DEPENDENCIES	= 
libSS7sua_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules/sua -I@srcdir@/src/modules/sua -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7sua_a_CFLAGS		= @KERNEL_CFLAGS@

streams-sua.o: $(libSS7sua_a_OBJECTS)
	$(LD) -r -o $@ $^

libSS7tua_a_SOURCES		= \
				  src/modules/tua/tua_asp.c \
				  src/modules/tua/tua.c \
				  src/modules/tua/tua_sgp.c \
				  src/modules/tua/tua_spp.c \
				  src/modules/tua/tua_ss7.c \
				  src/modules/tua/tua_asp.h \
				  src/modules/tua/tua_data.h \
				  src/modules/tua/tua.h \
				  src/modules/tua/tua_msg.h \
				  ./strconf.h

libSS7tua_a_DEPENDENCIES	= 
libSS7tua_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules/tua -I@srcdir@/src/modules/tua -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7tua_a_CFLAGS		= @KERNEL_CFLAGS@

streams-tua.o: $(libSS7tua_a_OBJECTS)
	$(LD) -r -o $@ $^

libSS7tali_a_SOURCES		= \
				  src/modules/tali/tali.c \
				  src/modules/tali/tali_isup.c \
				  src/modules/tali/tali_lm.c \
				  src/modules/tali/tali_mtp.c \
				  src/modules/tali/tali_sccp.c \
				  src/modules/tali/tali_data.h \
				  src/modules/tali/tali.h \
				  src/modules/tali/tali_msgs.h \
				  ./strconf.h

libSS7tali_a_DEPENDENCIES	= 
libSS7tali_a_CPPFLAGS		= -I./src/include/os7 -I@srcdir@/src/include/os7 -I./src/modules/tali -I@srcdir@/src/modules/tali -imacros ./strconf.h @SS7_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libSS7tali_a_CFLAGS		= @KERNEL_CFLAGS@

streams-tali.o: $(libSS7tali_a_OBJECTS)
	$(LD) -r -o $@ $^

KERNEL_MODULES			= $(libSS7mods_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= streams-m3ua.o streams-sscop.o streams-sua.o streams-tua.o streams-tali.o

all-am: $(KERNEL_MODULES)

#STRIP_KERNEL_MODULES		= $(STRIP) --strip-debug -X -x

install-exec-local: $(KERNEL_MODULES) conf.modules
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(kmoduledir)/ss7
	@list='$(KERNEL_MODULES)'; for p in $$list; do \
		if test -f $$p; then \
			f="`echo $$p | sed -e 's|^.*/||;s|libSS7mods_a|streams|;s|_|-|g'`" ; \
			echo "$(LIBTOOL) --mode=install $(INSTALL) -m 644 $$p $(DESTDIR)$(kmoduledir)/ss7/$$f" ; \
			$(LIBTOOL) --mode=install $(INSTALL) -m 644 $$p $(DESTDIR)$(kmoduledir)/ss7/$$f ; \
			if test -n "$(INSTALL_STRIP_FLAG)" -a -n "$(STRIP_KERNEL_MODULE)" ; then \
				echo "$(STRIP_KERNEL_MODULES) $(DESTDIR)$(kmoduledir)/ss7/$$f" ; \
				$(STRIP_KERNEL_MODULES) $(DESTDIR)$(kmoduledir)/ss7/$$f ; \
			fi ; \
		else :; fi ; \
	done
	$(INSTALL) -m 600 conf.modules $(DESTDIR)$(kmoduledir)/modules.ss7

install-data-local: $(man_MANS) $(srcdir)/doc/man/openss7.macros $(srcdir)/doc/man/openss7.refs
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(mandir)
	$(INSTALL_DATA) $(srcdir)/doc/man/openss7.macros $(DESTDIR)$(mandir)
	$(INSTALL_DATA) $(srcdir)/doc/man/openss7.refs   $(DESTDIR)$(mandir)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	@list='$(KERNEL_MODULES)'; for p in $$list; do \
		p="`echo $$p | sed -e 's|^.*/||;s|libSS7mods_a|streams|;s|_|-|g'`" ; \
		echo "$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(kmoduledir)/ss7/$$p" ; \
		$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(kmoduledir)/ss7/$$p ; \
	done
	rm -f $(DESTDIR)$(kmoduledir)/modules.ss7
	rm -f $(DESTDIR)$(mandir)/openss7.macros
	rm -f $(DESTDIR)$(mandir)/openss7.refs

man_MANS = \
	doc/man/man3/cframe.3 doc/man/man3/soon.3 doc/man/man3/xti_mtp.3 doc/man/man3/xti_sccp.3 \
	doc/man/man3/xti_ss7.3 doc/man/man3/xti_sscop.3 doc/man/man3/xti_tcap.3 \
	doc/man/man4/ISDN.4 doc/man/man4/MG.4 doc/man/man4/OpenSS7.4 doc/man/man4/SIGTRAN.4 doc/man/man4/SS7.4 \
	doc/man/man4/STREAMS.4 doc/man/man4/VOIP.4 doc/man/man4/acb56.4 doc/man/man4/bicc.4 \
	doc/man/man4/bicc_ioctl.4 doc/man/man4/bssap.4 doc/man/man4/bssap_ioctl.4 doc/man/man4/bua.4 \
	doc/man/man4/bua_ioctl.4 doc/man/man4/capi.4 doc/man/man4/capi_ioctl.4 doc/man/man4/ch.4 \
	doc/man/man4/ch_ioctl.4 doc/man/man4/cpc388.4 doc/man/man4/dass.4 doc/man/man4/dl.4 \
	doc/man/man4/dl_ioctl.4 doc/man/man4/dpnss.4 doc/man/man4/dua.4 doc/man/man4/dua_ioctl.4 \
	doc/man/man4/eth.4 doc/man/man4/fr.4 doc/man/man4/gr303.4 doc/man/man4/gr303_ioctl.4 \
	doc/man/man4/gr303ua.4 doc/man/man4/gr303ua_ioctl.4 doc/man/man4/gsma.4 doc/man/man4/h225.4 \
	doc/man/man4/h225_ioctl.4 doc/man/man4/h245.4 doc/man/man4/h248.4 doc/man/man4/h248_ioctl.4 \
	doc/man/man4/hdlc.4 doc/man/man4/hdlc_ioctl.4 doc/man/man4/idlc.4 \
	doc/man/man4/isdn.4 doc/man/man4/isdn_ioctl.4 doc/man/man4/isua.4 doc/man/man4/isua_ioctl.4 \
	doc/man/man4/isup.4 doc/man/man4/isup_ioctl.4 doc/man/man4/iua.4 doc/man/man4/iua_ioctl.4 \
	doc/man/man4/lapb.4 doc/man/man4/lapd.4 doc/man/man4/lapf.4 doc/man/man4/lmi_ioctl.4 \
	doc/man/man4/m2pa.4 doc/man/man4/m2tp.4 doc/man/man4/m2ua.4 doc/man/man4/m2ua_ioctl.4 \
	doc/man/man4/m3pa.4 doc/man/man4/m3tp.4 doc/man/man4/m3ua.4 doc/man/man4/m3ua_ioctl.4 \
	doc/man/man4/mg.4 doc/man/man4/mg_ioctl.4 doc/man/man4/mgc.4 doc/man/man4/mgcp.4 \
	doc/man/man4/mgcp_ioctl.4 doc/man/man4/mtp.4 doc/man/man4/mtp_ioctl.4 doc/man/man4/mtp_tpi.4 \
	doc/man/man4/mx.4 doc/man/man4/mx_ioctl.4 doc/man/man4/pca200e.4 doc/man/man4/q921.4 \
	doc/man/man4/q922.4 doc/man/man4/q931.4 doc/man/man4/rtp.4 doc/man/man4/sccp.4 \
	doc/man/man4/sccp_ioctl.4 doc/man/man4/sctp.4 doc/man/man4/sctp_ioctl.4 doc/man/man4/sdl.4 \
	doc/man/man4/sdl_eth.4 doc/man/man4/sdl_ioctl.4 doc/man/man4/sdl_ip.4 doc/man/man4/sdl_rtp.4 \
	doc/man/man4/sdl_sctp.4 doc/man/man4/sdl_tcp.4 doc/man/man4/sdl_udp.4 doc/man/man4/sdt.4 \
	doc/man/man4/sdt_ioctl.4 doc/man/man4/sipt.4 doc/man/man4/sipt_ioctl.4 doc/man/man4/sl.4 \
	doc/man/man4/sl_ioctl.4 doc/man/man4/openss7_soon.4 \
	doc/man/man4/sscop.4 doc/man/man4/sscop_ioctl.4 doc/man/man4/sscop_mce.4 doc/man/man4/sua.4 \
	doc/man/man4/sua_ioctl.4 doc/man/man4/tali.4 doc/man/man4/tali_ioctl.4 doc/man/man4/tcap.4 \
	doc/man/man4/tcap_ioctl.4 doc/man/man4/tua.4 doc/man/man4/tua_ioctl.4 doc/man/man4/ua_ioctl.4 \
	doc/man/man4/v5.4 doc/man/man4/v52.4 doc/man/man4/v5_ioctl.4 doc/man/man4/v5ua.4 \
	doc/man/man4/v5ua_ioctl.4 doc/man/man4/x100p-ch.4 doc/man/man4/x100p-ss7.4 doc/man/man4/x100p.4 \
	doc/man/man4/x400p-ch.4 doc/man/man4/x400p-ss7.4 doc/man/man4/x400p.4 \
	doc/man/man5/openss7_soon.5 \
	doc/man/man7/CC_ADDR_ACK.7 doc/man/man7/CC_ADDR_REQ.7 doc/man/man7/CC_ALERTING_IND.7 \
	doc/man/man7/CC_ALERTING_REQ.7 doc/man/man7/CC_BIND_ACK.7 doc/man/man7/CC_BIND_REQ.7 \
	doc/man/man7/CC_BLOCKING_CON.7 doc/man/man7/CC_BLOCKING_IND.7 doc/man/man7/CC_BLOCKING_REQ.7 \
	doc/man/man7/CC_BLOCKING_RES.7 doc/man/man7/CC_CALL_FAILURE_IND.7 doc/man/man7/CC_CALL_REATTEMPT_IND.7 \
	doc/man/man7/CC_CONNECT_IND.7 doc/man/man7/CC_CONNECT_REQ.7 doc/man/man7/CC_CONT_CHECK_IND.7 \
	doc/man/man7/CC_CONT_CHECK_REQ.7 doc/man/man7/CC_CONT_REPORT_IND.7 doc/man/man7/CC_CONT_REPORT_REQ.7 \
	doc/man/man7/CC_CONT_TEST_IND.7 doc/man/man7/CC_CONT_TEST_REQ.7 doc/man/man7/CC_DATALINK_FAILURE_IND.7 \
	doc/man/man7/CC_DISCONNECT_IND.7 doc/man/man7/CC_DISCONNECT_REQ.7 doc/man/man7/CC_ERROR_ACK.7 \
	doc/man/man7/CC_ERROR_IND.7 doc/man/man7/CC_FACILITY_IND.7 doc/man/man7/CC_FACILITY_REQ.7 \
	doc/man/man7/CC_FORWXFER_IND.7 doc/man/man7/CC_FORWXFER_REQ.7 doc/man/man7/CC_HOLD_CON.7 \
	doc/man/man7/CC_HOLD_IND.7 doc/man/man7/CC_HOLD_REQ.7 doc/man/man7/CC_HOLD_RES.7 \
	doc/man/man7/CC_IBI_IND.7 doc/man/man7/CC_IBI_REQ.7 doc/man/man7/CC_INFORMATION_IND.7 \
	doc/man/man7/CC_INFORMATION_REQ.7 doc/man/man7/CC_INFO_ACK.7 doc/man/man7/CC_INFO_REQ.7 \
	doc/man/man7/CC_INFO_TIMEOUT_IND.7 doc/man/man7/CC_MAINT_IND.7 doc/man/man7/CC_MORE_INFO_IND.7 \
	doc/man/man7/CC_MORE_INFO_REQ.7 doc/man/man7/CC_NOTIFY_IND.7 doc/man/man7/CC_NOTIFY_REQ.7 \
	doc/man/man7/CC_OK_ACK.7 doc/man/man7/CC_OPTMGMT_ACK.7 doc/man/man7/CC_OPTMGMT_REQ.7 \
	doc/man/man7/CC_PROCEEDING_IND.7 doc/man/man7/CC_PROCEEDING_REQ.7 doc/man/man7/CC_PROGRESS_IND.7 \
	doc/man/man7/CC_PROGRESS_REQ.7 doc/man/man7/CC_QUERY_CON.7 doc/man/man7/CC_QUERY_IND.7 \
	doc/man/man7/CC_QUERY_REQ.7 doc/man/man7/CC_QUERY_RES.7 doc/man/man7/CC_REGISTER_IND.7 \
	doc/man/man7/CC_REGISTER_REQ.7 doc/man/man7/CC_REJECT_IND.7 doc/man/man7/CC_REJECT_REQ.7 \
	doc/man/man7/CC_RELEASE_CON.7 doc/man/man7/CC_RELEASE_IND.7 doc/man/man7/CC_RELEASE_REQ.7 \
	doc/man/man7/CC_RELEASE_RES.7 doc/man/man7/CC_RESET_CON.7 doc/man/man7/CC_RESET_IND.7 \
	doc/man/man7/CC_RESET_REQ.7 doc/man/man7/CC_RESET_RES.7 doc/man/man7/CC_RESTART_CON.7 \
	doc/man/man7/CC_RESTART_REQ.7 doc/man/man7/CC_RESUME_CON.7 doc/man/man7/CC_RESUME_IND.7 \
	doc/man/man7/CC_RESUME_REJECT_IND.7 doc/man/man7/CC_RESUME_REJECT_REQ.7 doc/man/man7/CC_RESUME_REQ.7 \
	doc/man/man7/CC_RESUME_RES.7 doc/man/man7/CC_RETRIEVE_CON.7 doc/man/man7/CC_RETRIEVE_IND.7 \
	doc/man/man7/CC_RETRIEVE_REQ.7 doc/man/man7/CC_RETRIEVE_RES.7 doc/man/man7/CC_SETUP_COMPLETE_IND.7 \
	doc/man/man7/CC_SETUP_COMPLETE_REQ.7 doc/man/man7/CC_SETUP_CON.7 doc/man/man7/CC_SETUP_IND.7 \
	doc/man/man7/CC_SETUP_REQ.7 doc/man/man7/CC_SETUP_RES.7 doc/man/man7/CC_START_RESET_IND.7 \
	doc/man/man7/CC_STATUS_IND.7 doc/man/man7/CC_STOP_IND.7 doc/man/man7/CC_STOP_REQ.7 \
	doc/man/man7/CC_SUSPEND_CON.7 doc/man/man7/CC_SUSPEND_IND.7 doc/man/man7/CC_SUSPEND_REJECT_IND.7 \
	doc/man/man7/CC_SUSPEND_REJECT_REQ.7 doc/man/man7/CC_SUSPEND_REQ.7 doc/man/man7/CC_SUSPEND_RES.7 \
	doc/man/man7/CC_UNBIND_REQ.7 doc/man/man7/CC_UNBLOCKING_CON.7 doc/man/man7/CC_UNBLOCKING_IND.7 \
	doc/man/man7/CC_UNBLOCKING_REQ.7 doc/man/man7/CC_UNBLOCKING_RES.7 doc/man/man7/CC_addr_ack.7 \
	doc/man/man7/CC_addr_ack_t.7 doc/man/man7/CC_addr_req.7 doc/man/man7/CC_addr_req_t.7 \
	doc/man/man7/CC_alerting_ind.7 doc/man/man7/CC_alerting_ind_t.7 doc/man/man7/CC_alerting_req.7 \
	doc/man/man7/CC_alerting_req_t.7 doc/man/man7/CC_bind_ack.7 doc/man/man7/CC_bind_ack_t.7 \
	doc/man/man7/CC_bind_req.7 doc/man/man7/CC_bind_req_t.7 doc/man/man7/CC_blocking_con.7 \
	doc/man/man7/CC_blocking_con_t.7 doc/man/man7/CC_blocking_ind.7 doc/man/man7/CC_blocking_ind_t.7 \
	doc/man/man7/CC_blocking_req.7 doc/man/man7/CC_blocking_req_t.7 doc/man/man7/CC_blocking_res.7 \
	doc/man/man7/CC_blocking_res_t.7 doc/man/man7/CC_call_failure_ind.7 \
	doc/man/man7/CC_call_failure_ind_t.7 doc/man/man7/CC_call_reattempt_ind.7 \
	doc/man/man7/CC_call_reattempt_ind_t.7 doc/man/man7/CC_connect_ind.7 doc/man/man7/CC_connect_ind_t.7 \
	doc/man/man7/CC_connect_req.7 doc/man/man7/CC_connect_req_t.7 doc/man/man7/CC_cont_check_ind.7 \
	doc/man/man7/CC_cont_check_ind_t.7 doc/man/man7/CC_cont_check_req.7 doc/man/man7/CC_cont_check_req_t.7 \
	doc/man/man7/CC_cont_report_ind.7 doc/man/man7/CC_cont_report_ind_t.7 \
	doc/man/man7/CC_cont_report_req.7 doc/man/man7/CC_cont_report_req_t.7 doc/man/man7/CC_cont_test_ind.7 \
	doc/man/man7/CC_cont_test_ind_t.7 doc/man/man7/CC_cont_test_req.7 doc/man/man7/CC_cont_test_req_t.7 \
	doc/man/man7/CC_datalink_failure_ind.7 doc/man/man7/CC_datalink_failure_ind_t.7 \
	doc/man/man7/CC_disconnect_ind.7 doc/man/man7/CC_disconnect_ind_t.7 doc/man/man7/CC_disconnect_req.7 \
	doc/man/man7/CC_disconnect_req_t.7 doc/man/man7/CC_error_ack.7 doc/man/man7/CC_error_ack_t.7 \
	doc/man/man7/CC_error_ind.7 doc/man/man7/CC_error_ind_t.7 doc/man/man7/CC_facility_ind.7 \
	doc/man/man7/CC_facility_req.7 doc/man/man7/CC_forwxfer_ind.7 doc/man/man7/CC_forwxfer_ind_t.7 \
	doc/man/man7/CC_forwxfer_req.7 doc/man/man7/CC_forwxfer_req_t.7 doc/man/man7/CC_hold_con.7 \
	doc/man/man7/CC_hold_ind.7 doc/man/man7/CC_hold_req.7 doc/man/man7/CC_hold_res.7 \
	doc/man/man7/CC_ibi_ind.7 doc/man/man7/CC_ibi_ind_t.7 doc/man/man7/CC_ibi_req.7 \
	doc/man/man7/CC_ibi_req_t.7 doc/man/man7/CC_info_ack.7 doc/man/man7/CC_info_ack_t.7 \
	doc/man/man7/CC_info_req.7 doc/man/man7/CC_info_req_t.7 doc/man/man7/CC_info_timeout_ind.7 \
	doc/man/man7/CC_info_timeout_ind_t.7 doc/man/man7/CC_information_ind.7 \
	doc/man/man7/CC_information_ind_t.7 doc/man/man7/CC_information_req.7 \
	doc/man/man7/CC_information_req_t.7 doc/man/man7/CC_maint_ind.7 doc/man/man7/CC_maint_ind_t.7 \
	doc/man/man7/CC_more_info_ind.7 doc/man/man7/CC_more_info_ind_t.7 doc/man/man7/CC_more_info_req.7 \
	doc/man/man7/CC_more_info_req_t.7 doc/man/man7/CC_notify_ind.7 doc/man/man7/CC_notify_ind_t.7 \
	doc/man/man7/CC_notify_req.7 doc/man/man7/CC_notify_req_t.7 doc/man/man7/CC_ok_ack.7 \
	doc/man/man7/CC_ok_ack_t.7 doc/man/man7/CC_optmgmt_ack.7 doc/man/man7/CC_optmgmt_ack_t.7 \
	doc/man/man7/CC_optmgmt_req.7 doc/man/man7/CC_optmgmt_req_t.7 doc/man/man7/CC_proceeding_ind.7 \
	doc/man/man7/CC_proceeding_ind_t.7 doc/man/man7/CC_proceeding_req.7 doc/man/man7/CC_proceeding_req_t.7 \
	doc/man/man7/CC_progress_ind.7 doc/man/man7/CC_progress_ind_t.7 doc/man/man7/CC_progress_req.7 \
	doc/man/man7/CC_progress_req_t.7 doc/man/man7/CC_query_con.7 doc/man/man7/CC_query_con_t.7 \
	doc/man/man7/CC_query_ind.7 doc/man/man7/CC_query_ind_t.7 doc/man/man7/CC_query_req.7 \
	doc/man/man7/CC_query_req_t.7 doc/man/man7/CC_query_res.7 doc/man/man7/CC_query_res_t.7 \
	doc/man/man7/CC_register_ind.7 doc/man/man7/CC_register_req.7 doc/man/man7/CC_reject_ind.7 \
	doc/man/man7/CC_reject_ind_t.7 doc/man/man7/CC_reject_req.7 doc/man/man7/CC_reject_req_t.7 \
	doc/man/man7/CC_release_con.7 doc/man/man7/CC_release_con_t.7 doc/man/man7/CC_release_ind.7 \
	doc/man/man7/CC_release_ind_t.7 doc/man/man7/CC_release_req.7 doc/man/man7/CC_release_req_t.7 \
	doc/man/man7/CC_release_res.7 doc/man/man7/CC_release_res_t.7 doc/man/man7/CC_reset_con.7 \
	doc/man/man7/CC_reset_con_t.7 doc/man/man7/CC_reset_ind.7 doc/man/man7/CC_reset_ind_t.7 \
	doc/man/man7/CC_reset_req.7 doc/man/man7/CC_reset_req_t.7 doc/man/man7/CC_reset_res.7 \
	doc/man/man7/CC_reset_res_t.7 doc/man/man7/CC_restart_con.7 doc/man/man7/CC_restart_con_t.7 \
	doc/man/man7/CC_restart_req.7 doc/man/man7/CC_restart_req_t.7 doc/man/man7/CC_resume_con.7 \
	doc/man/man7/CC_resume_con_t.7 doc/man/man7/CC_resume_ind.7 doc/man/man7/CC_resume_ind_t.7 \
	doc/man/man7/CC_resume_reject_ind.7 doc/man/man7/CC_resume_reject_ind_t.7 \
	doc/man/man7/CC_resume_reject_req.7 doc/man/man7/CC_resume_reject_req_t.7 doc/man/man7/CC_resume_req.7 \
	doc/man/man7/CC_resume_req_t.7 doc/man/man7/CC_resume_res.7 doc/man/man7/CC_resume_res_t.7 \
	doc/man/man7/CC_retrieve_con.7 doc/man/man7/CC_retrieve_ind.7 doc/man/man7/CC_retrieve_req.7 \
	doc/man/man7/CC_retrieve_res.7 doc/man/man7/CC_setup_complete_ind.7 \
	doc/man/man7/CC_setup_complete_ind_t.7 doc/man/man7/CC_setup_complete_req.7 \
	doc/man/man7/CC_setup_complete_req_t.7 doc/man/man7/CC_setup_con.7 doc/man/man7/CC_setup_con_t.7 \
	doc/man/man7/CC_setup_ind.7 doc/man/man7/CC_setup_ind_t.7 doc/man/man7/CC_setup_req.7 \
	doc/man/man7/CC_setup_req_t.7 doc/man/man7/CC_setup_res.7 doc/man/man7/CC_setup_res_t.7 \
	doc/man/man7/CC_start_reset_ind.7 doc/man/man7/CC_start_reset_ind_t.7 doc/man/man7/CC_status_ind.7 \
	doc/man/man7/CC_status_ind_t.7 doc/man/man7/CC_stop_ind.7 doc/man/man7/CC_stop_ind_t.7 \
	doc/man/man7/CC_stop_req.7 doc/man/man7/CC_stop_req_t.7 doc/man/man7/CC_suspend_con.7 \
	doc/man/man7/CC_suspend_con_t.7 doc/man/man7/CC_suspend_ind.7 doc/man/man7/CC_suspend_ind_t.7 \
	doc/man/man7/CC_suspend_reject_ind.7 doc/man/man7/CC_suspend_reject_ind_t.7 \
	doc/man/man7/CC_suspend_reject_req.7 doc/man/man7/CC_suspend_reject_req_t.7 \
	doc/man/man7/CC_suspend_req.7 doc/man/man7/CC_suspend_req_t.7 doc/man/man7/CC_suspend_res.7 \
	doc/man/man7/CC_suspend_res_t.7 doc/man/man7/CC_unbind_req.7 doc/man/man7/CC_unbind_req_t.7 \
	doc/man/man7/CC_unblocking_con.7 doc/man/man7/CC_unblocking_con_t.7 doc/man/man7/CC_unblocking_ind.7 \
	doc/man/man7/CC_unblocking_ind_t.7 doc/man/man7/CC_unblocking_req.7 doc/man/man7/CC_unblocking_req_t.7 \
	doc/man/man7/CC_unblocking_res.7 doc/man/man7/CC_unblocking_res_t.7 doc/man/man7/CH_ATTACH_REQ.7 \
	doc/man/man7/CH_CONNECT_CON.7 doc/man/man7/CH_CONNECT_REQ.7 doc/man/man7/CH_DATA_IND.7 \
	doc/man/man7/CH_DATA_REQ.7 doc/man/man7/CH_DETACH_REQ.7 doc/man/man7/CH_DISABLE_CON.7 \
	doc/man/man7/CH_DISABLE_IND.7 doc/man/man7/CH_DISABLE_REQ.7 doc/man/man7/CH_DISCONNECT_CON.7 \
	doc/man/man7/CH_DISCONNECT_IND.7 doc/man/man7/CH_DISCONNECT_REQ.7 doc/man/man7/CH_ENABLE_CON.7 \
	doc/man/man7/CH_OK_ACK.7 doc/man/man7/CH_ENABLE_REQ.7 doc/man/man7/CH_ERROR_ACK.7 \
	doc/man/man7/CH_EVENT_IND.7 doc/man/man7/CH_INFO_ACK.7 doc/man/man7/CH_INFO_REQ.7 \
	doc/man/man7/CH_OPTMGMT_ACK.7 doc/man/man7/CH_OPTMGMT_REQ.7 doc/man/man7/CH_attach_req.7 \
	doc/man/man7/CH_attach_req_t.7 doc/man/man7/CH_connect_con.7 doc/man/man7/CH_connect_con_t.7 \
	doc/man/man7/CH_connect_req.7 doc/man/man7/CH_connect_req_t.7 doc/man/man7/CH_data_ind.7 \
	doc/man/man7/CH_data_ind_t.7 doc/man/man7/CH_data_req.7 doc/man/man7/CH_data_req_t.7 \
	doc/man/man7/CH_detach_req.7 doc/man/man7/CH_detach_req_t.7 doc/man/man7/CH_disable_con.7 \
	doc/man/man7/CH_disable_con_t.7 doc/man/man7/CH_disable_ind.7 doc/man/man7/CH_disable_ind_t.7 \
	doc/man/man7/CH_disable_req.7 doc/man/man7/CH_disable_req_t.7 doc/man/man7/CH_disconnect_con.7 \
	doc/man/man7/CH_disconnect_con_t.7 doc/man/man7/CH_disconnect_ind.7 doc/man/man7/CH_disconnect_ind_t.7 \
	doc/man/man7/CH_disconnect_req.7 doc/man/man7/CH_disconnect_req_t.7 doc/man/man7/CH_enable_con.7 \
	doc/man/man7/CH_enable_con_t.7 doc/man/man7/CH_enable_req.7 doc/man/man7/CH_enable_req_t.7 \
	doc/man/man7/CH_error_ack.7 doc/man/man7/CH_error_ack_t.7 doc/man/man7/CH_event_ind.7 \
	doc/man/man7/CH_event_ind_t.7 doc/man/man7/CH_info_ack.7 doc/man/man7/CH_info_ack_t.7 \
	doc/man/man7/CH_info_req.7 doc/man/man7/CH_info_req_t.7 doc/man/man7/CH_ok_ack.7 \
	doc/man/man7/CH_ok_ack_t.7 doc/man/man7/CH_optmgmt_ack.7 doc/man/man7/CH_optmgmt_ack_t.7 \
	doc/man/man7/CH_optmgmt_req.7 doc/man/man7/CH_optmgmt_req_t.7 doc/man/man7/MG_ABORT_REQ.7 \
	doc/man/man7/MG_ACTION_CON.7 doc/man/man7/MG_ACTION_IND.7 doc/man/man7/MG_ACTION_REQ.7 \
	doc/man/man7/MG_ATTACH_ACK.7 doc/man/man7/MG_ATTACH_REQ.7 doc/man/man7/MG_CONN_CON.7 \
	doc/man/man7/MG_CONN_REQ.7 doc/man/man7/MG_DATA_IND.7 doc/man/man7/MG_DATA_REQ.7 \
	doc/man/man7/MG_DETACH_REQ.7 doc/man/man7/MG_DISCON_CON.7 doc/man/man7/MG_DISCON_IND.7 \
	doc/man/man7/MG_DISCON_REQ.7 doc/man/man7/MG_ERROR_ACK.7 doc/man/man7/MG_INFO_ACK.7 \
	doc/man/man7/MG_INFO_REQ.7 doc/man/man7/MG_JOIN_CON.7 doc/man/man7/MG_JOIN_REQ.7 \
	doc/man/man7/MG_LEAVE_CON.7 doc/man/man7/MG_LEAVE_IND.7 doc/man/man7/MG_LEAVE_REQ.7 \
	doc/man/man7/MG_NOTIFY_IND.7 doc/man/man7/MG_NOTIFY_REQ.7 doc/man/man7/MG_OK_ACK.7 \
	doc/man/man7/MG_OPTMGMT_ACK.7 doc/man/man7/MG_OPTMGMT_REQ.7 doc/man/man7/MG_abort_req.7 \
	doc/man/man7/MG_abort_req_t.7 doc/man/man7/MG_action_con.7 doc/man/man7/MG_action_con_t.7 \
	doc/man/man7/MG_action_ind.7 doc/man/man7/MG_action_ind_t.7 doc/man/man7/MG_action_req.7 \
	doc/man/man7/MG_action_req_t.7 doc/man/man7/MG_attach_ack.7 doc/man/man7/MG_attach_ack_t.7 \
	doc/man/man7/MG_attach_req.7 doc/man/man7/MG_attach_req_t.7 doc/man/man7/MG_conn_con.7 \
	doc/man/man7/MG_conn_con_t.7 doc/man/man7/MG_conn_req.7 doc/man/man7/MG_conn_req_t.7 \
	doc/man/man7/MG_data_ind.7 doc/man/man7/MG_data_ind_t.7 doc/man/man7/MG_data_req.7 \
	doc/man/man7/MG_data_req_t.7 doc/man/man7/MG_detach_req.7 doc/man/man7/MG_detach_req_t.7 \
	doc/man/man7/MG_discon_con.7 doc/man/man7/MG_discon_con_t.7 doc/man/man7/MG_discon_ind.7 \
	doc/man/man7/MG_discon_ind_t.7 doc/man/man7/MG_discon_req.7 doc/man/man7/MG_discon_req_t.7 \
	doc/man/man7/MG_error_ack.7 doc/man/man7/MG_error_ack_t.7 doc/man/man7/MG_info_ack.7 \
	doc/man/man7/MG_info_ack_t.7 doc/man/man7/MG_info_req.7 doc/man/man7/MG_info_req_t.7 \
	doc/man/man7/MG_join_con.7 doc/man/man7/MG_join_con_t.7 doc/man/man7/MG_join_req.7 \
	doc/man/man7/MG_join_req_t.7 doc/man/man7/MG_leave_con.7 doc/man/man7/MG_leave_con_t.7 \
	doc/man/man7/MG_leave_ind.7 doc/man/man7/MG_leave_ind_t.7 doc/man/man7/MG_leave_req.7 \
	doc/man/man7/MG_leave_req_t.7 doc/man/man7/MG_notify_ind.7 doc/man/man7/MG_notify_ind_t.7 \
	doc/man/man7/MG_notify_req.7 doc/man/man7/MG_notify_req_t.7 doc/man/man7/MG_ok_ack.7 \
	doc/man/man7/MG_ok_ack_t.7 doc/man/man7/MG_optmgmt_ack.7 doc/man/man7/MG_optmgmt_ack_t.7 \
	doc/man/man7/MG_optmgmt_req.7 doc/man/man7/MG_optmgmt_req_t.7 doc/man/man7/MTP_ADDR_ACK.7 \
	doc/man/man7/MTP_ADDR_REQ.7 doc/man/man7/MTP_BIND_ACK.7 doc/man/man7/MTP_BIND_REQ.7 \
	doc/man/man7/MTP_CONN_REQ.7 doc/man/man7/MTP_DISCON_REQ.7 doc/man/man7/MTP_ERROR_ACK.7 \
	doc/man/man7/MTP_INFO_ACK.7 doc/man/man7/MTP_INFO_REQ.7 doc/man/man7/MTP_OK_ACK.7 \
	doc/man/man7/MTP_OPTMGMT_ACK.7 doc/man/man7/MTP_OPTMGMT_REQ.7 doc/man/man7/MTP_PAUSE_IND.7 \
	doc/man/man7/MTP_RESTART_BEGINS_IND.7 doc/man/man7/MTP_RESTART_COMPLETE_IND.7 \
	doc/man/man7/MTP_RESUME_IND.7 doc/man/man7/MTP_STATUS_IND.7 doc/man/man7/MTP_TRANSFER_IND.7 \
	doc/man/man7/MTP_TRANSFER_REQ.7 doc/man/man7/MTP_UNBIND_REQ.7 doc/man/man7/MX_ATTACH_REQ.7 \
	doc/man/man7/MX_CONNECT_CON.7 doc/man/man7/MX_CONNECT_REQ.7 doc/man/man7/MX_DATA_IND.7 \
	doc/man/man7/MX_DATA_REQ.7 doc/man/man7/MX_DETACH_REQ.7 doc/man/man7/MX_DISABLE_CON.7 \
	doc/man/man7/MX_DISABLE_IND.7 doc/man/man7/MX_DISABLE_REQ.7 doc/man/man7/MX_DISCONNECT_CON.7 \
	doc/man/man7/MX_DISCONNECT_IND.7 doc/man/man7/MX_DISCONNECT_REQ.7 doc/man/man7/MX_ENABLE_CON.7 \
	doc/man/man7/MX_OK_ACK.7 doc/man/man7/MX_ENABLE_REQ.7 doc/man/man7/MX_ERROR_ACK.7 \
	doc/man/man7/MX_INFO_ACK.7 doc/man/man7/MX_INFO_REQ.7 doc/man/man7/MX_OPTMGMT_ACK.7 \
	doc/man/man7/MX_OPTMGMT_REQ.7 doc/man/man7/MX_attach_req.7 doc/man/man7/MX_attach_req_t.7 \
	doc/man/man7/MX_connect_con.7 doc/man/man7/MX_connect_con_t.7 doc/man/man7/MX_connect_req.7 \
	doc/man/man7/MX_connect_req_t.7 doc/man/man7/MX_data_ind.7 doc/man/man7/MX_data_ind_t.7 \
	doc/man/man7/MX_data_req.7 doc/man/man7/MX_data_req_t.7 doc/man/man7/MX_detach_req.7 \
	doc/man/man7/MX_detach_req_t.7 doc/man/man7/MX_disable_con.7 doc/man/man7/MX_disable_con_t.7 \
	doc/man/man7/MX_disable_ind.7 doc/man/man7/MX_disable_ind_t.7 doc/man/man7/MX_disable_req.7 \
	doc/man/man7/MX_disable_req_t.7 doc/man/man7/MX_disconnect_con.7 doc/man/man7/MX_disconnect_con_t.7 \
	doc/man/man7/MX_disconnect_ind.7 doc/man/man7/MX_disconnect_ind_t.7 doc/man/man7/MX_disconnect_req.7 \
	doc/man/man7/MX_disconnect_req_t.7 doc/man/man7/MX_enable_con.7 doc/man/man7/MX_enable_con_t.7 \
	doc/man/man7/MX_enable_req.7 doc/man/man7/MX_enable_req_t.7 doc/man/man7/MX_error_ack.7 \
	doc/man/man7/MX_error_ack_t.7 doc/man/man7/MX_info_ack.7 doc/man/man7/MX_info_ack_t.7 \
	doc/man/man7/MX_info_req.7 doc/man/man7/MX_info_req_t.7 doc/man/man7/MX_ok_ack.7 \
	doc/man/man7/MX_ok_ack_t.7 doc/man/man7/MX_optmgmt_ack.7 doc/man/man7/MX_optmgmt_ack_t.7 \
	doc/man/man7/MX_optmgmt_req.7 doc/man/man7/MX_optmgmt_req_t.7 doc/man/man7/bicci.7 doc/man/man7/capi.7 \
	doc/man/man7/cci.7 doc/man/man7/cci_bicc.7 doc/man/man7/cci_bssap.7 doc/man/man7/cci_capi.7 \
	doc/man/man7/cci_dass.7 doc/man/man7/cci_gr303.7 doc/man/man7/cci_h225.7 doc/man/man7/cci_idlc.7 \
	doc/man/man7/cci_isdn.7 doc/man/man7/cci_isup.7 doc/man/man7/cci_sipt.7 doc/man/man7/npi_ip.7 \
	doc/man/man7/cdi_hdlc.7 doc/man/man7/chi.7 doc/man/man7/devi.7 doc/man/man7/dlpi_dass.7 \
	doc/man/man7/dlpi_dua.7 doc/man/man7/dlpi_eth.7 doc/man/man7/dlpi_gr303.7 doc/man/man7/dlpi_gr303ua.7 \
	doc/man/man7/dlpi_iua.7 doc/man/man7/dlpi_lapb.7 doc/man/man7/dlpi_lapd.7 doc/man/man7/dlpi_lapf.7 \
	doc/man/man7/dlpli_gr303ua.7 doc/man/man7/h225i.7 doc/man/man7/isdni.7 doc/man/man7/isup_socket.7 \
	doc/man/man7/isupi.7 doc/man/man7/lmi.7 doc/man/man7/mgi.7 doc/man/man7/mgi_h248.7 \
	doc/man/man7/mgi_mg.7 doc/man/man7/mgi_mgcp.7 doc/man/man7/mtp.7 doc/man/man7/mtp_npi.7 \
	doc/man/man7/mtp_socket.7 doc/man/man7/mtp_tpi.7 doc/man/man7/mtp_xti.7 doc/man/man7/mtpi.7 \
	doc/man/man7/mxi.7 doc/man/man7/npi_iso.7 doc/man/man7/npi_mtp.7 doc/man/man7/npi_q933.7 \
	doc/man/man7/npi_sccp.7 doc/man/man7/npi_sctp.7 doc/man/man7/npi_sscop.7 doc/man/man7/rudp.7 \
	doc/man/man7/saal_sscop.7 doc/man/man7/sccp_socket.7 doc/man/man7/sccpi.7 doc/man/man7/sdli.7 \
	doc/man/man7/sdli_2.7 doc/man/man7/sdti.7 doc/man/man7/sli.7 doc/man/man7/slsi.7 doc/man/man7/smi.7 \
	doc/man/man7/soon.7 doc/man/man7/ss7.7 doc/man/man7/sscop.7 doc/man/man7/sscopi.7 \
	doc/man/man7/tcap_socket.7 doc/man/man7/tcapi.7 doc/man/man7/tci.7 \
	doc/man/man7/tpi_mtp.7 doc/man/man7/tpi_sccp.7 doc/man/man7/tpi_sctp.7 doc/man/man7/tpi_ss7.7 \
	doc/man/man7/tpi_sscop.7 doc/man/man7/tpi_tcap.7 doc/man/man7/tri.7 \
	doc/man/man8/acb56.8 doc/man/man8/bicc.8 doc/man/man8/bua.8 doc/man/man8/cpc388.8 doc/man/man8/h225.8 \
	doc/man/man8/h323confd.8 doc/man/man8/isdn.8 doc/man/man8/isdnconfd.8 doc/man/man8/isua.8 \
	doc/man/man8/isup.8 doc/man/man8/iua.8 doc/man/man8/m2pa.8 doc/man/man8/m2tp.8 doc/man/man8/m2ua.8 \
	doc/man/man8/m3tp.8 doc/man/man8/m3ua.8 doc/man/man8/mgconfd.8 doc/man/man8/mtp.8 doc/man/man8/sccp.8 \
	doc/man/man8/sctp.8 doc/man/man8/sdl.8 doc/man/man8/sdl_udp.8 doc/man/man8/sdl_xxx.8 \
	doc/man/man8/sdt.8 doc/man/man8/sigtran.8 doc/man/man8/sl.8 doc/man/man8/sls.8 doc/man/man8/soip.8 \
	doc/man/man8/soon.8 doc/man/man8/ss7.8 doc/man/man8/ss7confd.8 doc/man/man8/ss7d.8 \
	doc/man/man8/ss7dev.8 doc/man/man8/strconf.8 doc/man/man8/sua.8 doc/man/man8/tcap.8 doc/man/man8/tua.8 \
	doc/man/man8/uaconfd.8 doc/man/man8/voipconfd.8

nobase_include_HEADERS = \
	src/include/os7/allocb.h src/include/os7/bufpool.h src/include/os7/bufq.h src/include/os7/debug.h \
	src/include/os7/lock.h src/include/os7/priv.h src/include/os7/queue.h src/include/os7/timer.h \
	src/include/ss7/capi.h src/include/ss7/capi_ioctl.h src/include/ss7/cci.h src/include/ss7/cdi.h \
	src/include/ss7/chi.h src/include/ss7/chi_ioctl.h src/include/ss7/devi.h src/include/ss7/devi_ioctl.h \
	src/include/ss7/dl.h src/include/ss7/dl_ioctl.h src/include/ss7/dlpi_ioctl.h \
	src/include/ss7/dua_ioctl.h src/include/ss7/gr303_ioctl.h src/include/ss7/gr303ua_ioctl.h \
	src/include/ss7/gsma_ioctl.h src/include/ss7/h225_ioctl.h src/include/ss7/hdlc_ioctl.h \
	src/include/ss7/isdni.h src/include/ss7/isdni_ioctl.h src/include/ss7/isupi.h \
	src/include/ss7/isupi_ioctl.h src/include/ss7/iua_ioctl.h src/include/ss7/lapb_ioctl.h \
	src/include/ss7/lapd_ioctl.h src/include/ss7/lapf_ioctl.h src/include/ss7/lki.h \
	src/include/ss7/lki_ioctl.h src/include/ss7/lmi.h src/include/ss7/lmi_ioctl.h \
	src/include/ss7/m2pa_ioctl.h src/include/ss7/m2ua_ioctl.h src/include/ss7/m3ua.h \
	src/include/ss7/m3ua_ioctl.h src/include/ss7/mgcp.h src/include/ss7/mgi.h src/include/ss7/mgi_ioctl.h \
	src/include/ss7/mtp2_ioctl.h src/include/ss7/mtp3b.h src/include/ss7/mtp.h src/include/ss7/mtpi.h \
	src/include/ss7/mtpi_ioctl.h src/include/ss7/mtp_ioctl.h src/include/ss7/mxi.h \
	src/include/ss7/mxi_ioctl.h src/include/ss7/ph.h src/include/ss7/q920.h src/include/ss7/sccp.h \
	src/include/ss7/sccpi.h src/include/ss7/sccpi_ioctl.h src/include/ss7/sccp_ioctl.h \
	src/include/ss7/sctp.h src/include/ss7/sctpi.h src/include/ss7/sctpi_ioctl.h src/include/ss7/sdli.h \
	src/include/ss7/sdli_ioctl.h src/include/ss7/sdti.h src/include/ss7/sdti_ioctl.h src/include/ss7/sl.h \
	src/include/ss7/sli.h src/include/ss7/sli_ioctl.h src/include/ss7/slmi.h src/include/ss7/slsi.h \
	src/include/ss7/slsi_ioctl.h src/include/ss7/smi.h src/include/ss7/smi_ioctl.h src/include/ss7/tcap.h \
	src/include/ss7/tcapi.h src/include/ss7/tcap_ioctl.h src/include/ss7/tc.h src/include/ss7/tr.h \
	src/include/ss7/ua_lm.h src/include/ss7/ua_lm_ioctl.h src/include/ss7/v52_ioctl.h \
	src/include/ss7/v5ua_ioctl.h \
	src/include/sys/cdi_daed.h src/include/sys/cdi.h src/include/sys/cdi_hdlc.h src/include/sys/dlpi_dua.h \
	src/include/sys/dlpi_gr303.h src/include/sys/dlpi_gr303ua.h src/include/sys/dlpi_gsma.h \
	src/include/sys/dlpi_h225.h src/include/sys/dlpi_isdn.h src/include/sys/dlpi_iua.h \
	src/include/sys/dlpi_lapb.h src/include/sys/dlpi_lapd.h src/include/sys/dlpi_lapf.h \
	src/include/sys/dlpi_m2ua.h src/include/sys/dlpi_mtp2.h src/include/sys/dlpi_v52.h \
	src/include/sys/dlpi_v5ua.h src/include/sys/npi.h src/include/sys/npi_ip.h src/include/sys/npi_mtp.h \
	src/include/sys/npi_sccp.h src/include/sys/npi_sctp.h src/include/sys/npi_ss7.h \
	src/include/sys/npi_sscop.h src/include/sys/npi_tcp.h src/include/sys/npi_udp.h \
	src/include/sys/sockmod.h src/include/sys/timod.h src/include/sys/tpi.h src/include/sys/tpi_inet.h \
	src/include/sys/tpi_ip.h src/include/sys/tpi_mtp.h src/include/sys/tpi_sccp.h \
	src/include/sys/tpi_sctp.h src/include/sys/tpi_sl.h src/include/sys/tpi_ss7.h \
	src/include/sys/tpi_sscop.h src/include/sys/tpi_tcap.h src/include/sys/tpi_tcp.h \
	src/include/sys/tpi_udp.h src/include/sys/xti.h src/include/sys/xti_inet.h src/include/sys/xti_ip.h \
	src/include/sys/xti_mosi.h src/include/sys/xti_mtp.h src/include/sys/xti_osi.h \
	src/include/sys/xti_sccp.h src/include/sys/xti_sctp.h src/include/sys/xti_sl.h \
	src/include/sys/xti_ss7.h src/include/sys/xti_sscop.h src/include/sys/xti_tcap.h \
	src/include/sys/xti_tcp.h src/include/sys/xti_udp.h

EXTRA_HEADERS = \
	src/modules/lm/lm.h src/modules/m3ua/m3ua_asp.h src/modules/m3ua/m3ua_data.h src/modules/m3ua/m3ua.h \
	src/modules/m3ua/m3ua_lm.h src/modules/m3ua/m3ua_msg.h src/modules/m3ua/m3ua_mtp.h \
	src/modules/m3ua/m3ua_nsp.h src/modules/m3ua/m3ua_tsp.h src/modules/sctp/sctp_cache.h \
	src/modules/sctp/sctp_cookie.h src/modules/sctp/sctp_defs.h src/modules/sctp/sctp.h \
	src/modules/sctp/sctp_hash.h src/modules/sctp/sctp_input.h src/modules/sctp/sctp_msg.h \
	src/modules/sctp/sctp_n.h src/modules/sctp/sctp_output.h src/modules/sctp/sctp_route.h \
	src/modules/sctp/sctp_t.h src/modules/sscop/sccop_user.h src/modules/sscop/sscop.h \
	src/modules/sscop/sscop_msgs.h src/modules/sscop/sscop_n_prov.h src/modules/sscop/sscop_prov.h \
	src/modules/sscop/sscop_send.h src/modules/sscop/sscop_time.h src/modules/sscop/sscop_t_prov.h \
	src/modules/sscop/sscop_user.h src/modules/sua/sua_asp.h src/modules/sua/sua_ctrl.h \
	src/modules/sua/sua_data.h src/modules/sua/sua.h src/modules/sua/sua_lm.h src/modules/sua/sua_msg.h \
	src/modules/sua/sua_nsp.h src/modules/sua/sua_sccp.h src/modules/sua/sua_send.h \
	src/modules/sua/sua_tsp.h src/modules/tali/tali_data.h src/modules/tali/tali.h \
	src/modules/tali/tali_msgs.h src/modules/tua/tua_asp.h src/modules/tua/tua_data.h \
	src/modules/tua/tua.h src/modules/tua/tua_msg.h src/modules/x400p-ss7/x400pfw.h src/modules/cd/cd.h

noinst_SCRIPTS = \
	$(auxdir)/strconf-sh

strconf_FILES = \
	src/modules/acb56/lis.conf \
	src/modules/bicc/lis.conf \
	src/modules/h225/lis.conf \
	src/modules/hdlc/lis.conf \
	src/modules/inet/lis.conf \
	src/modules/isdn/lis.conf \
	src/modules/isup/lis.conf \
	src/modules/lm/lis.conf \
	src/modules/m2pa/lis.conf \
	src/modules/m2tp/lis.conf \
	src/modules/m2ua/lis.conf \
	src/modules/m3ua/lis.conf \
	src/modules/mg/lis.conf \
	src/modules/mtp/lis.conf \
	src/modules/mx/lis.conf \
	src/modules/sccp/lis.conf \
	src/modules/sctp/lis.conf \
	src/modules/sdl/lis.conf \
	src/modules/sdt/lis.conf \
	src/modules/sl/lis.conf \
	src/modules/ss7ip/lis.conf \
	src/modules/ss7sctp/lis.conf \
	src/modules/sscop/lis.conf \
	src/modules/sua/lis.conf \
	src/modules/tali/lis.conf \
	src/modules/tcap/lis.conf \
	src/modules/tua/lis.conf \
	src/modules/x400p-ss7/lis.conf \
	src/modules/capi/lis.conf \
	src/modules/cd/lis.conf \
	src/modules/cdi/lis.conf \
	src/modules/cpc388/lis.conf \
	src/modules/cpc396/lis.conf \
	src/modules/devi/lis.conf \
	src/modules/dl/lis.conf \
	src/modules/dlpi/lis.conf \
	src/modules/ip/lis.conf \
	src/modules/lk/lis.conf \
	src/modules/matrix/lis.conf \
	src/modules/mgcp/lis.conf \
	src/modules/mtp3b/lis.conf \
	src/modules/npi/lis.conf \
	src/modules/raw_sctp/lis.conf \
	src/modules/switch/lis.conf \
	src/modules/tormenta/lis.conf \
	src/modules/xti_sctp/lis.conf

#
#   Need to override the following two automake rules to strip the first two
#   path components (src/include) from the header file path name for install.
#   This just saves us from doing a recursive make into the src/include directory.
#
install-nobase_includeHEADERS: $(nobase_include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(nobase_include_HEADERS)' ; for p in $$list ; do \
		if test -f "$$p" ; then d= ; else d="$(srcdir)/" ; fi ; \
		f="`echo $$p | sed -e 's|^[^/]*/[^/]*/||'`" ; \
		echo "$(nobase_includeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(includedir)/$$f" ; \
		$(nobase_includeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(includedir)/$$f ; \
	done

uninstall-nobase_includeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(nobase_include_HEADERS)' ; for p in $$list ; do \
		f="`echo $$p | sed -e 's|^[^/]*/[^/]*/||'`" ; \
		echo "rm -f $(DESTDIR)$(includedir)/$$f" ; \
		rm -f $(DESTDIR)$(includedir)/$$f ; \
	done

STRCONF_SCRIPT	    = $(auxdir)/strconf-sh
STRCONF		    = $(SHELL) $(STRCONF_SCRIPT) -b190

Config.master: $(strconf_FILES)
	cat /dev/null > Config.master
	@list='$(strconf_FILES)' ; cat /dev/null > Config.master ; for f in $$list ; do \
		if test -f "$$f" ; then d= ; else d="$(srcdir)/" ; fi ; \
		echo "cat $$d$$f >> Config.master" ; \
		cat $$d$$f >> Config.master ; \
	done

conf.modules: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --confmodules=conf.modules Config.master

strconf.h: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --hconfig=strconf.h Config.master

devices.lst: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --strmknods=devices.lst Config.master

$(libSS7mods_a_OBJECTS): strconf.h

all-am: conf.modules strconf.h devices.lst

EXTRA_DIST = openss7.spec openss7-logo-small.gif $(man_MANS) $(noinst_SCRIPTS) \
	     doc/man/openss7.macros doc/man/openss7.refs $(strconf_FILES) \
	     src/include/xti_inet.h $(EXTRA_HEADERS)

SUBDIRS = doc

#
#   Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion @kversion@"
AM_SRPMFLAGS =

include rpm.am

#
#   General rules
#

include openss7.am

# vim: ft=automake
