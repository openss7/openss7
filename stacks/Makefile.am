## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.99 $) $Date: 2007/09/06 11:16:16 $
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
# Last Modified $Date: 2007/09/06 11:16:16 $ by $Author: brian $
#
# =============================================================================

##
## General rules
##
include am/openss7.am

##
## Version file
##
include am/version.am

##
## Help
##
include am/help.am

##
## Problem Reports
##
include am/pr.am

lib_LTLIBRARIES			=
lib32_LTLIBRARIES		=

AM_YFLAGS			= -d -k

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if PUBLIC_RELEASE
## PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-inet_raw \
				  test-inet_udp \
				  test-inet_tcp \
				  test-inet_sctp \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_t \
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

pkglibexec_PROGRAMS		= \
				  test-q781 \
				  test-x100p \
				  test-x400p

if WITH_SCTP
pkglibexec_PROGRAMS		+= \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps
endif
if WITH_SCTP2
pkglibexec_PROGRAMS		+= \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod \
				  ss7statsd \
				  x400config \
				  mtpconfig \
				  sdlconfig \
				  slconfig \
				  slconfd \
				  slmon \
				  ss7capd \
				  test-sl

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"@PACKAGE_TARNAME@_mknod\"
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

ss7statsd_SOURCES		= src/util/ss7statsd.c
ss7statsd_LDADD			= $(STREAMS_LDADD)
ss7statsd_LDFLAGS		= $(STREAMS_LDFLAGS)
ss7statsd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"ss7statsd\"
ss7statsd_CFLAGS		= $(USER_CFLAGS)

slconfig_SOURCES		= src/util/slconf_yac.y \
				  src/util/slconf_lex.l \
				  src/util/slconf.c
slconfig_LDADD			= $(STREAMS_LDADD)
slconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
slconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"slconfig\"
slconfig_CFLAGS			= $(USER_CFLAGS)

mtpconfig_SOURCES		= src/util/mtpconf_yac.y \
				  src/util/mtpconf_lex.l
mtpconfig_LDADD			= $(STREAMS_LDADD)
mtpconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
mtpconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"mtpconfig\"
mtpconfig_CFLAGS		= $(USER_CFLAGS)

x400config_SOURCES		= src/util/x400yac.y \
				  src/util/x400lex.l \
				  src/util/x400config.h
x400config_LDADD		= $(STREAMS_LDADD)
x400config_LDFLAGS		= $(STREAMS_LDFLAGS)
x400config_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"x400config\"
x400config_CFLAGS		= $(USER_CFLAGS)

sdlconfig_SOURCES		= src/util/sdlconf.c \
				  src/util/sdlyac.y \
				  src/util/sdllex.l
sdlconfig_LDADD			= $(STREAMS_LDADD)
sdlconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
sdlconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"sdlconfig\"
sdlconfig_CFLAGS		= $(USER_CFLAGS)

slconfd_SOURCES			= src/util/slconfd.c
slconfd_LDADD			= $(STREAMS_LDADD)
slconfd_LDFLAGS			= $(STREAMS_LDFLAGS)
slconfd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"slconfd\"
slconfd_CFLAGS			= $(USER_CFLAGS)

slmon_SOURCES			= src/util/slmon.c
slmon_LDADD			= $(STREAMS_LDADD)
slmon_LDFLAGS			= $(STREAMS_LDFLAGS)
slmon_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"slmod\"
slmon_CFLAGS			= $(USER_CFLAGS)

ss7capd_SOURCES			= src/util/ss7capd.c
ss7capd_LDADD			= $(STREAMS_LDADD)
ss7capd_LDFLAGS			= $(STREAMS_LDFLAGS)
ss7capd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"ss7capd\"
ss7capd_CFLAGS			= $(USER_CFLAGS)

test_sl_SOURCES			= src/test/sl/test-sl.c
test_sl_LDADD			= $(STREAMS_LDADD)
test_sl_LDFLAGS			= $(STREAMS_LDFLAGS)
test_sl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test_sl\"
test_sl_CFLAGS			= $(USER_CFLAGS)

test_tcpc_SOURCES		= src/test/inet/test-tcpc.c
test_tcpc_LDADD			= $(STREAMS_LDADD)
test_tcpc_LDFLAGS		= $(STREAMS_LDFLAGS)
test_tcpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tcpc\"
test_tcpc_CFLAGS		= $(USER_CFLAGS)

test_tcps_SOURCES		= src/test/inet/test-tcps.c
test_tcps_LDADD			= $(STREAMS_LDADD)
test_tcps_LDFLAGS		= $(STREAMS_LDFLAGS)
test_tcps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tcps\"
test_tcps_CFLAGS		= $(USER_CFLAGS)

test_udpc_SOURCES		= src/test/inet/test-udpc.c
test_udpc_LDADD			= $(STREAMS_LDADD)
test_udpc_LDFLAGS		= $(STREAMS_LDFLAGS)
test_udpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-udpc\"
test_udpc_CFLAGS		= $(USER_CFLAGS)

test_udps_SOURCES		= src/test/inet/test-udps.c
test_udps_LDADD			= $(STREAMS_LDADD)
test_udps_LDFLAGS		= $(STREAMS_LDFLAGS)
test_udps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-udps\"
test_udps_CFLAGS		= $(USER_CFLAGS)

test_q784_SOURCES		= src/test/isup/test-q784.c
test_q784_LDADD    		= $(STREAMS_LDADD)
test_q784_LDFLAGS		= $(STREAMS_LDFLAGS)
test_q784_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q784\"
test_q784_CFLAGS		= $(USER_CFLAGS)

test_m2pa_SOURCES		= src/test/m2pa/test-m2pa.c
test_m2pa_LDADD			= $(STREAMS_LDADD)
test_m2pa_LDFLAGS		= $(STREAMS_LDFLAGS)
test_m2pa_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-m2pa\"
test_m2pa_CFLAGS		= $(USER_CFLAGS)

#perftest_SOURCES		= src/test/perftest.c
#perftest_LDADD			= $(STREAMS_LDADD)
#perftest_LDFLAGS		= $(STREAMS_LDFLAGS)
#perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"perftest\"
#perftest_CFLAGS		= $(USER_CFLAGS)

test_q781_SOURCES		= src/test/sl/test-q781.c
test_q781_LDADD			= $(STREAMS_LDADD)
test_q781_LDFLAGS		= $(STREAMS_LDFLAGS)
test_q781_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q781\"
test_q781_CFLAGS		= $(USER_CFLAGS)

#test_q781_pipe_SOURCES		= src/test/sl/test-q781-pipe.c
#test_q781_pipe_LDADD		= $(STREAMS_LDADD)
#test_q781_pipe_LDFLAGS		= $(STREAMS_LDFLAGS)
#test_q781_pipe_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q781-pipe\"
#test_q781_pipe_CFLAGS		= $(USER_CFLAGS)

test_x100p_SOURCES		= src/test/x400p-ss7/test-x100p.c
test_x100p_LDADD		= $(STREAMS_LDADD)
test_x100p_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x100p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x100p\"
test_x100p_CFLAGS		= $(USER_CFLAGS)

test_x400p_SOURCES		= src/test/x400p-ss7/test-x400p.c
test_x400p_LDADD		= $(STREAMS_LDADD)
test_x400p_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p\"
test_x400p_CFLAGS		= $(USER_CFLAGS)

#test_x400p_cap2_SOURCES	= src/test/x400p-ss7/test-x400p-cap2.c
#test_x400p_cap2_LDADD		= $(STREAMS_LDADD)
#test_x400p_cap2_LDFLAGS	= $(STREAMS_LDFLAGS)
#test_x400p_cap2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-cap2\"
#test_x400p_cap2_CFLAGS		= $(USER_CFLAGS)

#test_x400p_cap_SOURCES		= src/test/x400p-ss7/test-x400p-cap.c
#test_x400p_cap_LDADD		= $(STREAMS_LDADD)
#test_x400p_cap_LDFLAGS		= $(STREAMS_LDFLAGS)
#test_x400p_cap_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-cap\"
#test_x400p_cap_CFLAGS		= $(USER_CFLAGS)

#test_x400p_hsl_SOURCES		= src/test/x400p-ss7/test-x400p-hsl.c
#test_x400p_hsl_LDADD		= $(STREAMS_LDADD)
#test_x400p_hsl_LDFLAGS		= $(STREAMS_LDFLAGS)
#test_x400p_hsl_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-hsl\"
#test_x400p_hsl_CFLAGS		= $(USER_CFLAGS)

#test_x400p_perf_SOURCES	= src/test/x400p-ss7/test-x400p-perf.c
#test_x400p_perf_LDADD		= $(STREAMS_LDADD)
#test_x400p_perf_LDFLAGS	= $(STREAMS_LDFLAGS)
#test_x400p_perf_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-perf\"
#test_x400p_perf_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sdl_SOURCES		= src/test/x400p-ss7/test-x400p-sdl.c
#test_x400p_sdl_LDADD		= $(STREAMS_LDADD)
#test_x400p_sdl_LDFLAGS		= $(STREAMS_LDFLAGS)
#test_x400p_sdl_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdl\"
#test_x400p_sdl_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sdt2_SOURCES	= src/test/x400p-ss7/test-x400p-sdt2.c
#test_x400p_sdt2_LDADD		= $(STREAMS_LDADD)
#test_x400p_sdt2_LDFLAGS	= $(STREAMS_LDFLAGS)
#test_x400p_sdt2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdt2\"
#test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sdt_SOURCES		= src/test/x400p-ss7/test-x400p-sdt.c
#test_x400p_sdt_LDADD		= $(STREAMS_LDADD)
#test_x400p_sdt_LDFLAGS		= $(STREAMS_LDFLAGS)
#test_x400p_sdt_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdt\"
#test_x400p_sdt_CFLAGS		= $(USER_CFLAGS)

#test_x400p_sl_SOURCES		= src/test/x400p-ss7/test-x400p-sl.c
#test_x400p_sl_LDADD		= $(STREAMS_LDADD)
#test_x400p_sl_LDFLAGS		= $(STREAMS_LDFLAGS)
#test_x400p_sl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sl\"
#test_x400p_sl_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libSS7mods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= 

nodist_EXTRA_libSS7mods_a_SOURCES	= \
				  src/modules/mtp/mtp.c \
				  src/modules/mtp/mtp_min.c \
				  src/modules/mtp/mtp_mod.c \
				  src/modules/mtp/mtp_npi.c \
				  src/modules/mtp/mtp_tpi.c \
				  src/modules/isup/isup.c \
				  src/modules/sccp/sccp.c \
				  src/modules/sccp/sccp_mod.c \
				  src/modules/tcap/tcap.c \
				  src/modules/tcap/tr_mod.c \
				  src/modules/tcap/tc_mod.c \
				  src/modules/sdl/sdl.c \
				  src/modules/sdl/sdlm.c \
				  src/modules/sdl/spm.c \
				  src/modules/sdl/sdl_pmod.c \
				  src/modules/sdl/sdl_ch.c \
				  src/modules/sdt/sdt.c \
				  src/modules/sdt/sdt_pmod.c \
				  src/modules/sl/sl.c \
				  src/modules/sl/sl_min.c \
				  src/modules/sl/sl_mux.c \
				  src/modules/sl/sm_mod.c \
				  src/modules/sl/slpmod.c \
				  src/modules/x400p-ss7/x400p-ss7.c \
				  src/modules/x400p-ss7/x100p-ss7.c \
				  src/modules/x400p-ss7/sl_x400p.c \
				  src/modules/x400p-ss7/sl_x100p.c \
				  src/modules/x400p-ss7/v400pfw.h \
				  src/modules/x400p-ss7/v401pfw.h \
				  src/modules/x400p-ss7/x400pfw.h \
				  config.h strconf.h

mods_source_FILES		= $(nodist_EXTRA_libSS7mods_a_SOURCES)

nodist_libSS7mods_a_SOURCES	= $(mods_source_FILES)


#nodist_EXTRA_libSS7mods_a_SOURCES	= \
#				  src/modules/tcap/tc.c \
#				  src/modules/tcap/tr.c \
#				  src/modules/isdn/isdn.c \
#				  src/modules/lm/lm.c \
#				  src/modules/lm/lm.h \
#				  src/modules/m2ua/m2ua.c \
#				  src/modules/mg/mg.c \
#				  src/modules/mx/mx_sdl.c \
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
#				  src/modules/sdl/cdi.c \
#				  src/modules/sdl/cdi_mod.c \
#				  src/modules/sdt/sdtmux.c \
#				  src/modules/sl/slm.c \
#				  src/modules/sscop/sscop.c \
#				  src/modules/sscop/sscop2.c \
#				  src/modules/x400p-ss7/crc.c

libSS7mods_a_DEPENDENCIES	= config.h strconf.h
libSS7mods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) -I$(srcdir)/src/modules
libSS7mods_a_CFLAGS		= $(KERNEL_CFLAGS) -DKBUILD_MODNAME=\"$(kpre)`echo $@ | sed -e 's,lib.*_a-,,;s,\.o,,;s,-,_,g'`\"
libSS7mods_a_OBJECTS:		$(libSS7mods_a_DEPENDENCIES)

EXTRA_KERNEL_MODULES		+= 

## RPM_BUILD_KERNEL
endif

## PUBLIC_RELEASE
else
## ! PUBLIC_RELEASE

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  test-inet_raw \
				  test-inet_udp \
				  test-inet_tcp \
				  test-inet_sctp \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_t \
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

pkglibexec_PROGRAMS		= \
				  test-q781 \
				  test-q781-pipe \
				  test-q784 \
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

if WITH_SCTP
pkglibexec_PROGRAMS		+= \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps
endif
if WITH_SCTP2
pkglibexec_PROGRAMS		+= \
				  test-tcpc \
				  test-tcps \
				  test-udpc \
				  test-udps
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod \
				  ss7statsd \
				  x400config \
				  mtpconfig \
				  sdlconfig \
				  slconfig \
				  slconfd \
				  slmon \
				  ss7capd \
				  test-sl

@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"@PACKAGE_TARNAME@_mknod\"
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

ss7statsd_SOURCES		= src/util/ss7statsd.c
ss7statsd_LDADD			= $(STREAMS_LDADD)
ss7statsd_LDFLAGS		= $(STREAMS_LDFLAGS)
ss7statsd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"ss7statsd\"
ss7statsd_CFLAGS		= $(USER_CFLAGS)

slconfig_SOURCES		= src/util/slconf_yac.y \
				  src/util/slconf_lex.l \
				  src/util/slconf.c
slconfig_LDADD			= $(STREAMS_LDADD)
slconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
slconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"slconfig\"
slconfig_CFLAGS			= $(USER_CFLAGS)

mtpconfig_SOURCES		= src/util/mtpconf_yac.y \
				  src/util/mtpconf_lex.l
mtpconfig_LDADD			= $(STREAMS_LDADD)
mtpconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
mtpconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"mtpconfig\"
mtpconfig_CFLAGS		= $(USER_CFLAGS)

x400config_SOURCES		= src/util/x400yac.y \
				  src/util/x400lex.l \
				  src/util/x400config.h
x400config_LDADD		= $(STREAMS_LDADD)
x400config_LDFLAGS		= $(STREAMS_LDFLAGS)
x400config_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"x400config\"
x400config_CFLAGS		= $(USER_CFLAGS)

sdlconfig_SOURCES		= src/util/sdlconf.c \
				  src/util/sdlyac.y \
				  src/util/sdllex.l
sdlconfig_LDADD			= $(STREAMS_LDADD)
sdlconfig_LDFLAGS		= $(STREAMS_LDFLAGS)
sdlconfig_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"sdlconfig\"
sdlconfig_CFLAGS		= $(USER_CFLAGS)

slconfd_SOURCES			= src/util/slconfd.c
slconfd_LDADD			= $(STREAMS_LDADD)
slconfd_LDFLAGS			= $(STREAMS_LDFLAGS)
slconfd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"slconfd\"
slconfd_CFLAGS			= $(USER_CFLAGS)

slmon_SOURCES			= src/util/slmon.c
slmon_LDADD			= $(STREAMS_LDADD)
slmon_LDFLAGS			= $(STREAMS_LDFLAGS)
slmon_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"slmon\"
slmon_CFLAGS			= $(USER_CFLAGS)

ss7capd_SOURCES			= src/util/ss7capd.c
ss7capd_LDADD			= $(STREAMS_LDADD)
ss7capd_LDFLAGS			= $(STREAMS_LDFLAGS)
ss7capd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"ss7capd\"
ss7capd_CFLAGS			= $(USER_CFLAGS)

test_sl_SOURCES			= src/test/sl/test-sl.c
test_sl_LDADD			= $(STREAMS_LDADD)
test_sl_LDFLAGS			= $(STREAMS_LDFLAGS)
test_sl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test_sl\"
test_sl_CFLAGS			= $(USER_CFLAGS)

test_tcpc_SOURCES		= src/test/inet/test-tcpc.c
test_tcpc_LDADD			= $(STREAMS_LDADD)
test_tcpc_LDFLAGS		= $(STREAMS_LDFLAGS)
test_tcpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tcpc\"
test_tcpc_CFLAGS		= $(USER_CFLAGS)

test_tcps_SOURCES		= src/test/inet/test-tcps.c
test_tcps_LDADD			= $(STREAMS_LDADD)
test_tcps_LDFLAGS		= $(STREAMS_LDFLAGS)
test_tcps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tcps\"
test_tcps_CFLAGS		= $(USER_CFLAGS)

test_udpc_SOURCES		= src/test/inet/test-udpc.c
test_udpc_LDADD			= $(STREAMS_LDADD)
test_udpc_LDFLAGS		= $(STREAMS_LDFLAGS)
test_udpc_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-udpc\"
test_udpc_CFLAGS		= $(USER_CFLAGS)

test_udps_SOURCES		= src/test/inet/test-udps.c
test_udps_LDADD			= $(STREAMS_LDADD)
test_udps_LDFLAGS		= $(STREAMS_LDFLAGS)
test_udps_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-udps\"
test_udps_CFLAGS		= $(USER_CFLAGS)

test_q784_SOURCES		= src/test/isup/test-q784.c
test_q784_LDADD			= $(STREAMS_LDADD)
test_q784_LDFLAGS		= $(STREAMS_LDFLAGS)
test_q784_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q784\"
test_q784_CFLAGS		= $(USER_CFLAGS)

test_m2pa_SOURCES		= src/test/m2pa/test-m2pa.c
test_m2pa_LDADD			= $(STREAMS_LDADD)
test_m2pa_LDFLAGS		= $(STREAMS_LDFLAGS)
test_m2pa_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-m2pa\"
test_m2pa_CFLAGS		= $(USER_CFLAGS)

#perftest_SOURCES		= src/test/perftest.c
#perftest_LDADD			= $(STREAMS_LDADD)
#perftest_LDFLAGS		= $(STREAMS_LDFLAGS)
#perftest_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"perftest\"
#perftest_CFLAGS		= $(USER_CFLAGS)

test_q781_SOURCES		= src/test/sl/test-q781.c
test_q781_LDADD			= $(STREAMS_LDADD)
test_q781_LDFLAGS		= $(STREAMS_LDFLAGS)
test_q781_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q781\"
test_q781_CFLAGS		= $(USER_CFLAGS)

test_q781_pipe_SOURCES		= src/test/sl/test-q781-pipe.c
test_q781_pipe_LDADD		= $(STREAMS_LDADD)
test_q781_pipe_LDFLAGS		= $(STREAMS_LDFLAGS)
test_q781_pipe_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-q781-pipe\"
test_q781_pipe_CFLAGS		= $(USER_CFLAGS)

test_x100p_SOURCES		= src/test/x400p-ss7/test-x100p.c
test_x100p_LDADD		= $(STREAMS_LDADD)
test_x100p_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x100p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x100p\"
test_x100p_CFLAGS		= $(USER_CFLAGS)

test_x400p_SOURCES		= src/test/x400p-ss7/test-x400p.c
test_x400p_LDADD		= $(STREAMS_LDADD)
test_x400p_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p\"
test_x400p_CFLAGS		= $(USER_CFLAGS)

test_x400p_cap2_SOURCES		= src/test/x400p-ss7/test-x400p-cap2.c
test_x400p_cap2_LDADD		= $(STREAMS_LDADD)
test_x400p_cap2_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_cap2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-cap2\"
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS)

test_x400p_cap_SOURCES		= src/test/x400p-ss7/test-x400p-cap.c
test_x400p_cap_LDADD		= $(STREAMS_LDADD)
test_x400p_cap_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_cap_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-cap\"
test_x400p_cap_CFLAGS		= $(USER_CFLAGS)

test_x400p_hsl_SOURCES		= src/test/x400p-ss7/test-x400p-hsl.c
test_x400p_hsl_LDADD		= $(STREAMS_LDADD)
test_x400p_hsl_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_hsl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-hsl\"
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS)

test_x400p_perf_SOURCES		= src/test/x400p-ss7/test-x400p-perf.c
test_x400p_perf_LDADD		= $(STREAMS_LDADD)
test_x400p_perf_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_perf_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-perf\"
test_x400p_perf_CFLAGS		= $(USER_CFLAGS)

test_x400p_sdl_SOURCES		= src/test/x400p-ss7/test-x400p-sdl.c
test_x400p_sdl_LDADD		= $(STREAMS_LDADD)
test_x400p_sdl_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_sdl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdl\"
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS)

test_x400p_sdt2_SOURCES		= src/test/x400p-ss7/test-x400p-sdt2.c
test_x400p_sdt2_LDADD		= $(STREAMS_LDADD)
test_x400p_sdt2_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_sdt2_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdt2\"
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS)

test_x400p_sdt_SOURCES		= src/test/x400p-ss7/test-x400p-sdt.c
test_x400p_sdt_LDADD		= $(STREAMS_LDADD)
test_x400p_sdt_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_sdt_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sdt\"
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS)

test_x400p_sl_SOURCES		= src/test/x400p-ss7/test-x400p-sl.c
test_x400p_sl_LDADD		= $(STREAMS_LDADD)
test_x400p_sl_LDFLAGS		= $(STREAMS_LDFLAGS)
test_x400p_sl_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-x400p-sl\"
test_x400p_sl_CFLAGS		= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		= libSS7mods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			+= libSS7sscop.a libSS7m3ua.a libSS7sua.a libSS7tua.a libSS7tali.a

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
				  src/modules/isdn/isdn.c \
				  src/modules/isup/isup.c \
				  src/modules/lm/lm.c \
				  src/modules/m2pa/m2pa_sl.c \
				  src/modules/m2ua/m2ua.c \
				  src/modules/mg/mg.c \
				  src/modules/mtp/mtp.c \
				  src/modules/mtp/mtp_min.c \
				  src/modules/mtp/mtp_mod.c \
				  src/modules/mtp/mtp_npi.c \
				  src/modules/mtp/mtp_tpi.c \
				  src/modules/mx/mx_sdl.c \
				  src/modules/sccp/sccp.c \
				  src/modules/sccp/sccp_mod.c \
				  src/modules/sdl/sdl.c \
				  src/modules/sdl/sdlm.c \
				  src/modules/sdl/spm.c \
				  src/modules/sdl/sdl_pmod.c \
				  src/modules/sdl/sdl_ch.c \
				  src/modules/sdt/sdt.c \
				  src/modules/sdt/sdt_pmod.c \
				  src/modules/sl/sl.c \
				  src/modules/sl/sl_min.c \
				  src/modules/sl/sl_mux.c \
				  src/modules/sl/sm_mod.c \
				  src/modules/sl/slpmod.c \
				  src/modules/ss7ip/sdt_tpi.c \
				  src/modules/ss7ip/sl_tpi.c \
				  src/modules/ss7sctp/sdt_sctp.c \
				  src/modules/ss7sctp/sdl_sctp.c \
				  src/modules/tcap/tcap.c \
				  src/modules/tcap/tr_mod.c \
				  src/modules/tcap/tc_mod.c \
				  src/modules/x400p-ss7/x400p-ss7.c \
				  src/modules/x400p-ss7/x100p-ss7.c \
				  src/modules/x400p-ss7/sl_x400p.c \
				  src/modules/x400p-ss7/sl_x100p.c \
				  src/modules/x400p-ss7/sdt_x400p.c \
				  src/modules/x400p-ss7/sdl_x400p.c \
				  src/modules/x400p-ss7/ch_x400p.c \
				  src/modules/x400p-ss7/mx_x400p.c \
				  src/modules/x400p-ss7/x400p_mx.c \
				  src/modules/x400p-ss7/v400pfw.h \
				  src/modules/x400p-ss7/v401pfw.h \
				  src/modules/x400p-ss7/x400pfw.h \
				  src/modules/lm/lm.h \
				  src/modules/cd/cd.h \
				  config.h strconf.h

if WITH_LIS
## WITH_LIS
nodist_EXTRA_libSS7mods_a_SOURCES	+= \
				  src/modules/dl/dl.c
## WITH_LIS
endif

mods_source_FILES		= $(nodist_EXTRA_libSS7mods_a_SOURCES)

nodist_libSS7mods_a_SOURCES	= $(mods_source_FILES)

#nodist_EXTRA_libSS7mods_a_SOURCES	= \
#				  src/modules/tcap/tc.c \
#				  src/modules/tcap/tr.c \
#				  src/modules/acb56/sdl_acb56.c \
#				  src/modules/acb56/sdt_acb56.c \
#				  src/modules/acb56/sl_acb56.c \
#				  src/modules/h225/h225.c \
#				  src/modules/m2tp/m2tp.c \
#				  src/modules/sdl/cdi.c \
#				  src/modules/sdl/cdi_mod.c \
#				  src/modules/sdt/sdtmux.c \
#				  src/modules/sl/slm.c \
#				  src/modules/sscop/sscop.c \
#				  src/modules/sscop/sscop2.c \
#				  src/modules/x400p-ss7/crc.c

libSS7mods_a_DEPENDENCIES	= config.h strconf.h
libSS7mods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) -I$(srcdir)/src/modules
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
				  config.h strconf.h
m3ua_source_FILES		= $(nodist_EXTRA_libSS7m3ua_a_SOURCES)
nodist_libSS7m3ua_a_SOURCES	= $(m3ua_source_FILES)

libSS7m3ua_a_DEPENDENCIES	= config.h strconf.h
libSS7m3ua_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSS7m3ua_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7m3ua_a_OBJECTS:		$(libSS7m3ua_a_DEPENDENCIES)

if WITH_KO_MODULES
streams_m3ua.o: $(libSS7m3ua_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-m3ua.o: $(libSS7m3ua_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

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
				  config.h strconf.h
sscop_source_FILES		= $(nodist_EXTRA_libSS7sscop_a_SOURCES)
nodist_libSS7sscop_a_SOURCES	= $(sscop_source_FILES)

libSS7sscop_a_DEPENDENCIES	= config.h strconf.h
libSS7sscop_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSS7sscop_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7sscop_a_OBJECTS:		$(libSS7sscop_a_DEPENDENCIES)

if WITH_KO_MODULES
streams_sscop.o: $(libSS7sscop_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-sscop.o: $(libSS7sscop_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

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
				  config.h strconf.h
sua_source_FILES		= $(nodist_EXTRA_libSS7sua_a_SOURCES)
nodist_libSS7sua_a_SOURCES	= $(sua_source_FILES)

libSS7sua_a_DEPENDENCIES	= config.h strconf.h
libSS7sua_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSS7sua_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7sua_a_OBJECTS:		$(libSS7sua_a_DEPENDENCIES)

if WITH_KO_MODULES
streams_sua.o: $(libSS7sua_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-sua.o: $(libSS7sua_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

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
				  config.h strconf.h
tua_source_FILES		= $(nodist_EXTRA_libSS7tua_a_SOURCES)
nodist_libSS7tua_a_SOURCES	= $(tua_source_FILES)

libSS7tua_a_DEPENDENCIES	= config.h strconf.h
libSS7tua_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSS7tua_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7tua_a_OBJECTS:		$(libSS7tua_a_DEPENDENCIES)

if WITH_KO_MODULES
streams_tua.o: $(libSS7tua_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-tua.o: $(libSS7tua_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

nodist_EXTRA_libSS7tali_a_SOURCES = \
				  src/modules/tali/tali.c \
				  src/modules/tali/tali_isup.c \
				  src/modules/tali/tali_lm.c \
				  src/modules/tali/tali_mtp.c \
				  src/modules/tali/tali_sccp.c \
				  src/modules/tali/tali_data.h \
				  src/modules/tali/tali.h \
				  src/modules/tali/tali_msgs.h \
				  config.h strconf.h
tali_source_FILES		= $(nodist_EXTRA_libSS7tali_a_SOURCES)
nodist_libSS7tali_a_SOURCES	= $(tali_source_FILES)

libSS7tali_a_DEPENDENCIES	= config.h strconf.h
libSS7tali_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSS7tali_a_CFLAGS		= $(KERNEL_CFLAGS)
libSS7tali_a_OBJECTS:		$(libSS7tali_a_DEPENDENCIES)

if WITH_KO_MODULES
streams_tali.o: $(libSS7tali_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-tali.o: $(libSS7tali_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

if WITH_KO_MODULES
EXTRA_KERNEL_MODULES		+= streams_m3ua.o streams_sscop.o streams_sua.o streams_tua.o streams_tali.o
else
EXTRA_KERNEL_MODULES		+= streams-m3ua.o streams-sscop.o streams-sua.o streams-tua.o streams-tali.o
endif

## RPM_BUILD_KERNEL
endif

## ! PUBLIC_RELEASE
endif

if RPM_BUILD_USER
## RPM_BUILD_USER

test_timod_SOURCES		= src/test/tli/test-timod.c
test_timod_LDADD		= libxnet.la
test_timod_LDFLAGS		=
test_timod_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-timod\"
test_timod_CFLAGS		= $(USER_CFLAGS)

test_tirdwr_SOURCES		= src/test/tli/test-tirdwr.c
test_tirdwr_LDADD		= libxnet.la
test_tirdwr_LDFLAGS		=
test_tirdwr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-tirdwr\"
test_tirdwr_CFLAGS		= $(USER_CFLAGS)

if WITH_TLI
## WITH_TLI
pkglibexec_PROGRAMS		+= \
				  test-timod \
				  test-tirdwr
## WITH_TLI
endif

test_inet_raw_SOURCES		= src/test/inet/test-inet_raw.c
test_inet_raw_LDADD		= $(STREAMS_LDADD)
test_inet_raw_LDFLAGS		= $(STREAMS_LDFLAGS)
test_inet_raw_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_raw\"
test_inet_raw_CFLAGS		= $(USER_CFLAGS)

test_inet_tcp_SOURCES		= src/test/inet/test-inet_tcp.c
test_inet_tcp_LDADD		= $(STREAMS_LDADD)
test_inet_tcp_LDFLAGS		= $(STREAMS_LDFLAGS)
test_inet_tcp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_tcp\"
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)

test_inet_udp_SOURCES		= src/test/inet/test-inet_udp.c
test_inet_udp_LDADD		= $(STREAMS_LDADD)
test_inet_udp_LDFLAGS		= $(STREAMS_LDFLAGS)
test_inet_udp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_udp\"
test_inet_udp_CFLAGS		= $(USER_CFLAGS)

test_inet_sctp_SOURCES		= src/test/inet/test-inet_sctp.c
test_inet_sctp_LDADD		= $(STREAMS_LDADD)
test_inet_sctp_LDFLAGS		= $(STREAMS_LDFLAGS)
test_inet_sctp_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-inet_sctp\"
test_inet_sctp_CFLAGS		= $(USER_CFLAGS)

test_sctp_n_SOURCES		= src/test/sctp/test-sctp_n.c
test_sctp_n_LDADD		= $(STREAMS_LDADD)
test_sctp_n_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sctp_n_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-sctp_n\"
test_sctp_n_CFLAGS		= $(USER_CFLAGS)

test_sctp_n2_SOURCES		= src/test/sctp/test-sctp_n2.c
test_sctp_n2_LDADD		= $(STREAMS_LDADD)
test_sctp_n2_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sctp_n2_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-sctp_n2\"
test_sctp_n2_CFLAGS		= $(USER_CFLAGS)

test_sctp_t_SOURCES		= src/test/sctp/test-sctp_t.c
test_sctp_t_LDADD		= $(STREAMS_LDADD)
test_sctp_t_LDFLAGS		= $(STREAMS_LDFLAGS)
test_sctp_t_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-sctp_t\"
test_sctp_t_CFLAGS		= $(USER_CFLAGS)

test_xnet_SOURCES		= src/test/xnet/test-xnet.c
test_xnet_LDADD			= libxnet.la
test_xnet_LDFLAGS		=
test_xnet_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-xnet\"
test_xnet_CFLAGS		= $(USER_CFLAGS)

test_xnet_thr_SOURCES		= src/test/xnet/test-xnet.c
test_xnet_thr_LDADD		= libxnet.la
test_xnet_thr_LDFLAGS		=
test_xnet_thr_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME=\"test-xnet_thr\"
test_xnet_thr_CFLAGS		= $(USER_CFLAGS)

if WITH_INET
## WITH_INET
pkglibexec_PROGRAMS		+= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp
if WITH_OPENSS7_SCTP
pkglibexec_PROGRAMS		+= \
				  test-inet_sctp
endif

## WITH_INET
endif

if WITH_SCTP
## WITH_SCTP
pkglibexec_PROGRAMS		+= \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_t
## WITH_SCTP
endif

if WITH_SCTP2
## WITH_SCTP2
pkglibexec_PROGRAMS		+= \
				  test-sctp_n \
				  test-sctp_n2 \
				  test-sctp_t
## WITH_SCTP2
endif

if WITH_SIGTRAN
## WITH_SIGTRAN
pkglibexec_PROGRAMS		+= \
				  test-m2pa
## WITH_SIGTRAN
endif

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

if WITH_TLI
## WITH_TLI
nodist_libSS7mods_a_SOURCES	+= \
				  src/modules/ti/timod.c \
				  src/include/timod.h \
				  src/modules/ti/tirdwr.c \
				  src/include/tirdwr.h
## WITH_TLI
endif

if WITH_INET
## WITH_INET
nodist_libSS7mods_a_SOURCES	+= \
				  src/modules/inet/inet.c \
				  src/modules/inet/udp.c \
				  src/modules/inet/rawip.c \
				  src/modules/inet/tcp.c \
				  src/modules/inet/udp_hooks.h
## WITH_INET
endif

if WITH_SIGTRAN
## WITH_SIGTRAN
nodist_libSS7mods_a_SOURCES	+= \
				  src/modules/m2pa/m2pa_sl.c \
				  src/modules/ss7ip/sdt_tpi.c \
				  src/modules/ss7ip/sl_tpi.c \
				  src/modules/ss7sctp/sdt_sctp.c \
				  src/modules/ss7sctp/sdl_sctp.c
## WITH_SIGTRAN
endif

## RPM_BUILD_KERNEL
endif

## PKG_BUILD_ARCH
endif

ss7_source_FILES		= $(mods_source_FILES) \
				  $(sscop_source_FILES) \
				  $(m3ua_source_FILES) \
				  $(sua_source_FILES) \
				  $(tua_source_FILES) \
				  $(tali_source_FILES)

EXTRA_DIST			+= $(ss7_source_FILES)

EXTRA_DOCS 			+= \
				doc/drafts/isua/draft-bidulock-sigtran-isua-03.pdf \
				doc/drafts/isua/draft-bidulock-sigtran-isua-03.ps \
				doc/drafts/isua/draft-bidulock-sigtran-isua-03.txt \
				doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-06.pdf \
				doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-06.ps \
				doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-06.txt \
				doc/drafts/m2pa/draft-george-sigtran-m2pa-interop-01.txt \
				doc/drafts/m2pa/draft-ietf-sigtran-m2pa-11.txt \
				doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-02.pdf \
				doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-02.ps \
				doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-02.txt \
				doc/drafts/tua/draft-bidulock-sigtran-tua-04.pdf \
				doc/drafts/tua/draft-bidulock-sigtran-tua-04.ps \
				doc/drafts/tua/draft-bidulock-sigtran-tua-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-aspext-04.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-aspext-04.ps \
				doc/drafts/ua/draft-bidulock-sigtran-aspext-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-corid-04.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-corid-04.ps \
				doc/drafts/ua/draft-bidulock-sigtran-corid-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-datamodel-00.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-loadgrp-04.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-loadgrp-04.ps \
				doc/drafts/ua/draft-bidulock-sigtran-loadgrp-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-loadsel-04.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-loadsel-04.ps \
				doc/drafts/ua/draft-bidulock-sigtran-loadsel-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-regext-04.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-regext-04.ps \
				doc/drafts/ua/draft-bidulock-sigtran-regext-04.txt \
				doc/drafts/ua/draft-bidulock-sigtran-sginfo-05.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-sginfo-05.ps \
				doc/drafts/ua/draft-bidulock-sigtran-sginfo-05.txt\
				doc/drafts/ua/draft-bidulock-sigtran-aspcong-00.pdf \
				doc/drafts/ua/draft-bidulock-sigtran-aspcong-00.ps \
				doc/drafts/ua/draft-bidulock-sigtran-aspcong-00.txt

EXTRA_DIST			+= $(EXTRA_DOCS)

EXTRA_TEX			+= \
				scripts/texinfo.cnf \
				scripts/texi/args.texi.in \
				scripts/texi/fdl.texi \
				scripts/texi/gpl.texi \
				scripts/texi/install.texi \
				scripts/texi/lesser.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+= configure.nexusware

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libSS7mods_a_OBJECTS) $(ADDON_OBJS)
KERNEL_MODULES			= $(libSS7mods_a_OBJECTS) $(ADDON_MODS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			= $(STREAMS_MODMAP) $(STRCOMP_MODMAP)

if WITH_KO_MODULES
## WITH_KO_MODULES
kpre				= streams_
else
kpre				= streams-
## WITH_KO_MODULES
endif

include am/kernel.am

if WITH_KO_MODULES
else
if PKG_BUILD_INDEP
if RPM_BUILD_USER
nodist_modutil_DATA		= \
				src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
LIBRARY_VERSION_FILES		= \
				src/lib/libxnet/xnet.VERSION

include am/libraries.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

if WITH_XNET
## WITH_XNET
lib_LTLIBRARIES			+= libxnet.la
pkglibexec_PROGRAMS		+= \
				  test-xnet \
				  test-xnet_thr
libxnet_la_SOURCES		= src/lib/libxnet/pthread.c \
				  src/lib/libxnet/xnet.c \
				  gettext.h
libxnet_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/libxnet/xnet.VERSION \
				  $(STREAMS_LDFLAGS)
libxnet_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -I$(srcdir)/src/include/xti -I./src/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS)
libxnet_la_LIBADD		= $(STREAMS_LDADD)
## WITH_XNET
endif

## RPM_BUILD_USER
endif

## PKG_BUILD_ARCH
endif

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

dist_noinst_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV) 

if WITH_INET
## WITH_INET

install-inet-devices: $(INET_MKDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV) ; \
	fi

uninstall-inet-devices:
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV) ; \
	fi

## WITH_INET
else
## ! WITH_INET

install-inet-devices:

uninstall-inet-devices:

## ! WITH_INET
endif

INSTALL_EXEC_HOOK += install-inet-devices

UNINSTALL_HOOK += uninstall-inet-devices

.PHONY: install-inet-devices uninstall-inet-devices

EXPORT_SRCFILES			=
if PUBLIC_RELEASE
else
EXPORT_SRCFILES			+= src/modules/cd/cd.c
endif

EXPORT_VERFILES			=
if PUBLIC_RELEASE
else
EXPORT_VERFILES			+= $(PKGINCL)/versions/cd.ver
endif

GENKSYMS_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -I$(srcdir)/src/modules

##
## Rules for generating versioned kernel symbols
##
include am/genksyms.am

##
## Rules for generating STREAMS configuration
##
include am/strconf.am

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src

if WITH_XNET
if PKG_BUILD_ARCH
SUBDIRS += intl po
CLEANFILES += po/stamp-po
endif
endif

SUBDIRS += .

if WITH_32BIT_LIBS
SUBDIRS += lib32
endif

SUBDIRS += tests

if DEVELOPMENT
SUBDIRS += doc
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/@PACKAGE_LCNAME@.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/@PACKAGE_LCNAME@

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= src/util/sysconfig/@PACKAGE_LCNAME@.conf

dist_config_DATA		= src/util/sysconfig/@PACKAGE_LCNAME@

## INSTALL_INITSCRIPTS
endif

## PKG_BUILD_INDEP
endif

## RPM_BUILD_USER
endif

##
## Rules for init scripts
##
include am/init.am

##
## Rules for lsms
##
include am/lsm.am

##
## Rules for archives
##
include am/archive.am

##
## Rules for making rpms
##
AM_RPMFLAGS			= --define "_kversion $(kversion)"
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Rules for making release files
##
MANUAL_FILE			= doc/manual/$(PACKAGE).txt

include am/release.am

##
## Rules for doxygen documentation.
##
include am/doxy.am

EXTRA_DIST			+= \
				  debian/README.Debian \
				  debian/TODO.Debian

MAINTAINERCLEANFILES		+= \
				  $(top_srcdir)/aclocal.m4 \
				  $(top_srcdir)/config.h.in \
				  $(top_srcdir)/configure \
				  $(top_srcdir)/COPYING \
				  $(top_srcdir)/INSTALL \
				  $(top_srcdir)/scripts/compile \
				  $(top_srcdir)/scripts/config.guess \
				  $(top_srcdir)/scripts/config.sub \
				  $(top_srcdir)/scripts/depcomp \
				  $(top_srcdir)/scripts/install-sh \
				  $(top_srcdir)/scripts/ltmain.sh \
				  $(top_srcdir)/scripts/missing \
				  $(top_srcdir)/scripts/texinfo.tex

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.99  2007/09/06 11:16:16  brian
## - testing updates
##
## Revision 0.9.2.98  2007/08/19 11:55:37  brian
## - move stdbool.h, obviate need for YFLAGS, general workup
##
## Revision 0.9.2.97  2007/08/18 03:53:10  brian
## - working up configuration files
##
## Revision 0.9.2.96  2007/08/13 15:21:55  brian
## - header updates, added references
##
## Revision 0.9.2.95  2007/08/12 15:35:43  brian
## - new release files with GPLv3
##
## Revision 0.9.2.94  2007/08/03 13:34:47  brian
## - manual updates, put ss7 modules in public release
##
## Revision 0.9.2.93  2007/07/21 20:09:42  brian
## - added pass structure
##
## Revision 0.9.2.92  2007/07/14 01:33:47  brian
## - make license explicit, add documentation
##
## Revision 0.9.2.91  2007/06/20 07:53:22  brian
## - updates for Fedora 7 and 2.6.21
##
## Revision 0.9.2.90  2007/03/16 10:30:27  brian
## - final changes from release
##
## Revision 0.9.2.89  2007/03/13 06:17:00  brian
## - suppress errors when building 32-bit libs
##
## Revision 0.9.2.88  2007/03/08 06:08:29  brian
## - modutil_DATA should not be distributed (and it reorders uninstall)
##
## Revision 0.9.2.87  2007/03/04 00:23:35  brian
## - add version file creation
##
## Revision 0.9.2.86  2007/02/21 01:09:04  brian
## - updating mtp.c driver, better mi_open allocators
##
## Revision 0.9.2.85  2007/02/17 02:49:04  brian
## - first clean recompile of MTP modules on LFS
##
## Revision 0.9.2.84  2007/02/13 07:55:40  brian
## - working up MTP and UAs
##
## Revision 0.9.2.83  2007/01/21 20:22:35  brian
## - working up drivers
##
## Revision 0.9.2.82  2006/12/29 05:51:03  brian
## - changes for successful master build
##
## Revision 0.9.2.81  2006/12/27 16:35:57  brian
## - added slpmod module and fixups for make check target
##
## Revision 0.9.2.80  2006/12/18 10:50:55  brian
## - subpackaging changes for release
##
## Revision 0.9.2.79  2006/12/07 10:04:34  brian
## - added init scripts to master makefile
##
## Revision 0.9.2.78  2006/12/06 11:45:13  brian
## - updated X400P driver and test suites
##
## Revision 0.9.2.77  2006/10/03 13:51:33  brian
## - changes to pass make check target
## - added some package config.h files
## - removed AUTOCONFIG_H from Makefile.am's
## - source code changes for compile
## - added missing manual pages
## - renamed conflicting manual pages
## - parameterized include Makefile.am
## - updated release notes
##
## Revision 0.9.2.76  2006/10/01 21:30:15  brian
## - distribute release and library version files
##
## Revision 0.9.2.75  2006/09/29 11:50:09  brian
## - libtool library tweaks in Makefile.am
## - better rpm spec handling in *.spec.in
## - added AC_LIBTOOL_DLOPEN to configure.ac
## - updated some copyright headers
## - rationalized item in two packages
## - added manual pages, drivers and modules to new strtty package
##
## Revision 0.9.2.74  2006/09/27 05:15:13  brian
## - distinguish LDADD from LDFLAGS
##
## Revision 0.9.2.73  2006/09/25 20:15:43  brian
## - rationalized to strinet and strxnet
##
## Revision 0.9.2.72  2006/09/18 17:43:10  brian
## - added doxy rules to makefile and release notes
##
## Revision 0.9.2.71  2006/09/18 01:15:22  brian
## - add 32bit libs, release file changes, additional packages
##
## Revision 0.9.2.70  2006/08/26 09:17:30  brian
## - better release file generation
##
## Revision 0.9.2.69  2006/07/01 12:32:36  brian
## - add clobbered section back in
##
## Revision 0.9.2.68  2006/07/01 11:59:39  brian
## - added new KERNEL_BLDFLAGS to makefiles
##
## Revision 0.9.2.67  2006/03/21 13:24:37  brian
## - included pr.am
##
## Revision 0.9.2.66  2006/03/20 11:53:58  brian
## - added checks for DEVELOPMENT
##
## Revision 0.9.2.65  2006/03/07 13:16:56  brian
## - tools must be compiled static for distcheck
##
## Revision 0.9.2.64  2006/03/07 01:04:06  brian
## - removed conflicting perftest
##
## Revision 0.9.2.63  2006/03/06 11:38:27  brian
## - updated headers
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
