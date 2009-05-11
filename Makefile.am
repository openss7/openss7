# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>, or
# write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.
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

AUTOMAKE_OPTIONS += nostdinc
AM_CPPFLAGS = -I. -I$(top_builddir) -I$(srcdir)

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

##
## These are for ptys and should be part of the strutil or strio package.
##
bin_PROGRAMS			=

sbin_PROGRAMS			=

#dist_bin_SCRIPTS		= scripts/modpost.sh scripts/modsyms.sh

dist_sbin_SCRIPTS		=

pkglibexec_PROGRAMS		=

#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= @PACKAGE_TARNAME@_mknod

#LDADD				= libstreams.la

lib_LTLIBRARIES			=

lib32_LTLIBRARIES		=

snmpdlmod_LTLIBRARIES		=

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

noinst_LIBRARIES		=

## =====================================================================

noinst_LIBRARIES		+= libOS7specfs.a

EXTRA_libOS7specfs_a_SOURCES	= src/kernel/strspecfs.c \
				  src/kernel/strlookup.c \
				  src/kernel/strattach.c \
				  src/kernel/strpipe.c
libOS7specfs_a_SOURCES		= src/kernel/specfs.c \
				  src/kernel/strspecfs.h \
				  src/kernel/strlookup.h \
				  src/kernel/strattach.h \
				  src/kernel/strpipe.h \
				  src/kernel/strargs.h
libOS7specfs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7specfs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7specfs_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

specfs.o: stamp-verobjs
	@list=`$(ECHO) " $(libOS7specfs_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

## =====================================================================

noinst_LIBRARIES		+= libOS7streams.a

EXTRA_libOS7streams_a_SOURCES	= src/kernel/strmain.c \
				  src/kernel/strreg.c \
				  src/kernel/strprocfs.c \
				  src/kernel/strsched.c \
				  src/kernel/strsysctl.c \
				  src/kernel/strsyms.c \
				  src/kernel/strutil.c \
				  src/kernel/strsad.c \
				  src/modules/sth.c \
				  src/modules/sth.h \
				  src/drivers/clone.c \
				  src/drivers/clone.h
libOS7streams_a_SOURCES		= src/kernel/streams.c \
				  src/kernel/strreg.h \
				  src/kernel/strprocfs.h \
				  src/kernel/strsched.h \
				  src/kernel/strsysctl.h \
				  src/kernel/strutil.h \
				  src/kernel/strsad.h \
				  src/kernel/strargs.h
libOS7streams_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7streams_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7streams_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

streams.o: stamp-verobjs
	@list=`$(ECHO) " $(libOS7streams_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

## =====================================================================

noinst_LIBRARIES		+= libOS7kernel.a

libOS7kernel_a_SOURCES		= src/include/sys/os7/allocb.h \
				  src/include/sys/os7/bufpool.h \
				  src/include/sys/os7/bufq.h \
				  src/include/sys/os7/compat.h \
				  src/include/sys/os7/debug.h \
				  src/include/sys/os7/lock.h \
				  src/include/sys/os7/priv.h \
				  src/include/sys/os7/queue.h \
				  src/include/sys/os7/timer.h \
				  src/kernel/os7compat.c \
				  src/include/sys/os7/ddi.h \
				  src/include/sys/os7/strconf.h \
				  src/include/sys/os7/stream.h \
				  src/kernel/svr3compat.c \
				  src/include/sys/svr3/ddi.h \
				  src/include/sys/svr3/strconf.h \
				  src/include/sys/svr3/stream.h \
				  src/kernel/svr4compat.c \
				  src/include/sys/svr4/ddi.h \
				  src/include/sys/svr4/strconf.h \
				  src/include/sys/svr4/stream.h \
				  src/kernel/mpscompat.c \
				  src/include/sys/mps/ddi.h \
				  src/include/sys/mps/strconf.h \
				  src/include/sys/mps/stream.h \
				  src/kernel/suncompat.c \
				  src/include/sys/sunddi.h \
				  src/include/sys/strsun.h \
				  src/include/sys/sun/ddi.h \
				  src/include/sys/sun/strconf.h \
				  src/include/sys/sun/strsun.h \
				  src/include/sys/sun/stream.h \
				  src/kernel/uw7compat.c \
				  src/include/sys/uw7/ddi.h \
				  src/include/sys/uw7/strconf.h \
				  src/include/sys/uw7/stream.h \
				  src/kernel/osfcompat.c \
				  src/include/sys/osf/ddi.h \
				  src/include/sys/osf/strconf.h \
				  src/include/sys/osf/stream.h \
				  src/kernel/aixcompat.c \
				  src/include/sys/aix/ddi.h \
				  src/include/sys/aix/strconf.h \
				  src/include/sys/aix/stream.h \
				  src/kernel/hpuxcompat.c \
				  src/include/sys/hpux/ddi.h \
				  src/include/sys/hpux/strconf.h \
				  src/include/sys/hpux/stream.h \
				  src/kernel/irixcompat.c \
				  src/include/sys/irix/ddi.h \
				  src/include/sys/irix/strconf.h \
				  src/include/sys/irix/stream.h \
				  src/kernel/maccompat.c \
				  src/include/sys/mac/ddi.h \
				  src/include/sys/mac/strconf.h \
				  src/include/sys/mac/stream.h
libOS7kernel_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_EXPSYMTAB)
libOS7kernel_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7kernel_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

## =====================================================================

noinst_LIBRARIES		+= libOS7modules.a

EXTRA_libOS7modules_a_SOURCES	= src/modules/mx_sdl.c \
				  src/modules/ch_x400p.c \
				  src/modules/mx_x400p.c \
				  src/modules/cd.c src/modules/cd.h \
				  src/modules/cd_daed.c \
				  src/modules/cd_hdlc.c \
				  src/modules/hdlc.c \
				  src/modules/m2tp.c \
				  src/modules/cdi_mod.c \
				  src/modules/sscop.c \
				  src/modules/sscop2.c \
				  src/modules/m2tp.c
libOS7modules_a_SOURCES		= src/modules/bufmod.c \
				  src/modules/nullmod.c \
				  src/modules/pipemod.c \
				  src/modules/connld.c \
				  src/modules/sc.c src/include/sys/sc.h \
				  src/modules/testmod.c src/include/sys/testmod.h \
				  src/modules/pckt.c \
				  src/modules/ptem.c \
				  src/modules/ttcompat.c \
				  src/modules/ldterm.c \
				  src/modules/debug.h \
				  src/modules/ip_strm_mod.c src/modules/ip_strm_mod.h \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  src/modules/sockmod.c \
				  src/modules/tpiperf.c \
				  src/modules/ch_pmod.c \
				  src/modules/mx_pmod.c \
				  src/modules/cdpmod.c \
				  src/modules/dlaix.c \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c \
				  src/modules/phys.c \
				  src/modules/aal1.c \
				  src/modules/aal2.c \
				  src/modules/aal5.c \
				  src/modules/sscop.c \
				  src/modules/sscopmce.c \
				  src/modules/mtp_npi.c \
				  src/modules/mtp_tpi.c \
				  src/modules/sccp_mod.c \
				  src/modules/npi_sccp.c \
				  src/modules/tpi_sccp.c \
				  src/modules/tr_mod.c \
				  src/modules/tc_mod.c \
				  src/modules/sdl.c \
				  src/modules/spm.c \
				  src/modules/sdl_pmod.c \
				  src/modules/sdl_ch.c \
				  src/modules/sdt.c \
				  src/modules/sdt_pmod.c \
				  src/modules/sl.c \
				  src/modules/sm_mod.c \
				  src/modules/slpmod.c \
				  src/modules/m2pa_sl.c \
				  src/modules/m2ua_as.c \
				  src/modules/m3ua_as.c \
				  src/modules/sdl_sctp.c \
				  src/modules/sdt_sctp.c \
				  src/modules/sdt_tpi.c \
				  src/modules/sl_tpi.c
libOS7modules_a_CPPFLAGS	= -I$(top_builddir)/src/modules -I$(top_srcdir)/src/modules \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_EXPSYMTAB)
libOS7modules_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7modules_a_OBJECTS):	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

## =====================================================================

noinst_LIBRARIES		+= libOS7drivers.a

EXTRA_libOS7drivers_a_SOURCES	= src/drivers/cd.c \
				  src/drivers/dl.c src/drivers/dl.h \
				  src/drivers/dl_ip.c \
				  src/drivers/dlp.c \
				  src/drivers/ip.c \
				  src/drivers/nf.c \
				  src/drivers/np.c src/drivers/np.h \
				  src/drivers/tc.c \
				  src/drivers/tr.c \
				  src/drivers/isdn.c \
				  src/drivers/lm.c src/drivers/lm.h \
				  src/drivers/m2ua.c \
				  src/drivers/mg.c \
				  src/drivers/sl_x400p.c \
				  src/drivers/sl_x100p.c \
				  src/drivers/sdt_x400p.c \
				  src/drivers/sdl_x400p.c \
				  src/drivers/x400p_mx.c \
				  src/drivers/cd.c src/drivers/cd.h \
				  src/drivers/dl.c \
				  src/drivers/dl_dua.c \
				  src/drivers/dl_gr303.c \
				  src/drivers/dl_gr303ua.c \
				  src/drivers/dl_gsma.c \
				  src/drivers/dl_h225.c \
				  src/drivers/dl_iua.c \
				  src/drivers/dl_lapb.c \
				  src/drivers/dl_lapd.c \
				  src/drivers/dl_lapf.c \
				  src/drivers/dl_v52.c \
				  src/drivers/dl_v5ua.c \
				  src/drivers/hdlc.c \
				  src/drivers/sdl_acb56.c \
				  src/drivers/sdt_acb56.c \
				  src/drivers/sl_acb56.c \
				  src/drivers/h225.c \
				  src/drivers/cdi.c \
				  src/drivers/sdtmux.c \
				  src/drivers/slm.c \
				  src/drivers/sscop.c \
				  src/drivers/isua_as.c \
				  src/drivers/m2ua.c \
				  src/drivers/m2ua_as.c \
				  src/drivers/m3ua_as.c \
				  src/drivers/m3ua_asp.c src/drivers/m3ua_asp.h \
				  src/drivers/m3ua.c src/drivers/m3ua_data.h src/drivers/m3ua.h \
				  src/drivers/m3ua_lm.c src/drivers/m3ua_lm.h \
				  src/drivers/m3ua_lower.c \
				  src/drivers/m3ua_msg.c src/drivers/m3ua_msg.h \
				  src/drivers/m3ua_mtp.c src/drivers/m3ua_mtp.h \
				  src/drivers/m3ua_mtpp.c \
				  src/drivers/m3ua_mtpu.c \
				  src/drivers/m3ua_nsp.c src/drivers/m3ua_nsp.h \
				  src/drivers/m3ua_sg.c \
				  src/drivers/m3ua_sgp.c \
				  src/drivers/m3ua_spp.c \
				  src/drivers/m3ua_ss7.c \
				  src/drivers/m3ua_tsp.c src/drivers/m3ua_tsp.h \
				  src/drivers/sua_as.c \
				  src/drivers/sua_asp.c src/drivers/sua_asp.h \
				  src/drivers/sua.c src/drivers/sua_ctrl.h src/drivers/sua_data.h src/drivers/sua.h \
				  src/drivers/sua_lm.c src/drivers/sua_lm.h \
				  src/drivers/sua_lower.c \
				  src/drivers/sua_msg.c src/drivers/sua_msg.h \
				  src/drivers/sua_nsp.c src/drivers/sua_nsp.h \
				  src/drivers/sua_sccp.c src/drivers/sua_sccp.h \
				  src/drivers/sua_send.h \
				  src/drivers/sua_sgp.c \
				  src/drivers/sua_sm.c \
				  src/drivers/sua_spp.c \
				  src/drivers/sua_ss7.c \
				  src/drivers/sua_tsp.c src/drivers/sua_tsp.h \
				  src/drivers/tali.c src/drivers/tali_data.h src/drivers/tali.h \
				  src/drivers/tali_isup.c \
				  src/drivers/tali_lm.c \
				  src/drivers/tali_msgs.h \
				  src/drivers/tali_mtp.c \
				  src/drivers/tali_sccp.c \
				  src/drivers/tua_as.c \
				  src/drivers/tua_asp.c src/drivers/tua_asp.h \
				  src/drivers/tua.c src/drivers/tua_data.h src/drivers/tua.h \
				  src/drivers/tua_msg.h \
				  src/drivers/tua_sgp.c \
				  src/drivers/tua_spp.c \
				  src/drivers/tua_ss7.c \
				  src/drivers/ua_msg.h
libOS7drivers_a_SOURCES		= src/drivers/fifo.c src/drivers/fifo.h \
				  src/drivers/sad.c src/include/sys/sad.h \
				  src/drivers/nsdev.c \
				  src/drivers/echo.c \
				  src/drivers/mux.c \
				  src/drivers/nuls.c \
				  src/drivers/pipe.c src/drivers/pipe.h \
				  src/drivers/log.c src/include/log.h src/include/sys/log.h src/include/strlog.h src/include/sys/strlog.h src/drivers/log.h \
				  src/drivers/loop.c src/include/sys/loop.h \
				  src/drivers/sfx.c \
				  src/drivers/spx.c \
				  src/drivers/pty.c \
				  src/drivers/debug.h \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
				  src/drivers/np_ip.c \
				  src/drivers/socksys.c \
				  src/drivers/inet.c \
				  src/drivers/udp.c \
				  src/drivers/rawip.c \
				  src/drivers/tcp.c \
				  src/drivers/x25-llc2.c \
				  src/drivers/x25-lapb.c \
				  src/drivers/x25-plp.c \
				  src/drivers/clns.c \
				  src/drivers/atm.c \
				  src/drivers/mtp.c \
				  src/drivers/mtp_min.c \
				  src/drivers/isup.c \
				  src/drivers/sccp.c \
				  src/drivers/tcap.c \
				  src/drivers/sdlm.c \
				  src/drivers/sl_min.c \
				  src/drivers/sl_mux.c \
				  src/drivers/x400p-ss7.c \
				  src/drivers/x100p-ss7.c \
				  src/drivers/sl_x400p.c \
				  src/drivers/sl_x100p.c \
				  src/drivers/v400pfw.h \
				  src/drivers/v401pfw.h \
				  src/drivers/x400pfw.h \
				  src/drivers/ua_as.c \
				  src/drivers/m2ua_as.c \
				  src/drivers/m3ua_as.c \
				  src/drivers/sua_as.c \
				  src/drivers/tua_as.c \
				  src/drivers/isua_as.c
libOS7drivers_a_CPPFLAGS	= -I$(top_builddir)/src/drivers -I$(top_srcdir)/src/drivers \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_EXPSYMTAB)
libOS7drivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7drivers_a_OBJECTS):	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

## =====================================================================

noinst_LIBRARIES		+= libOS7sctp.a

EXTRA_libOS7sctp_a_SOURCES	= src/drivers/sctp.c \
				  src/drivers/sctp_cache.c \
				  src/drivers/sctp_cookie.c \
				  src/drivers/sctp_hash.c \
				  src/drivers/sctp_input.c \
				  src/drivers/sctp_msg.c \
				  src/drivers/sctp_n.c \
				  src/drivers/sctp_output.c \
				  src/drivers/sctp_route.c \
				  src/drivers/sctp_t.c \
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_crc32c.h \
				  src/drivers/sctp_cache.h \
				  src/drivers/sctp_cookie.h \
				  src/drivers/sctp_defs.h \
				  src/drivers/sctp.h \
				  src/drivers/sctp_hash.h \
				  src/drivers/sctp_input.h \
				  src/drivers/sctp_msg.h \
				  src/drivers/sctp_n.h \
				  src/drivers/sctp_output.h \
				  src/drivers/sctp_route.h \
				  src/drivers/sctp_t.h
libOS7sctp_a_SOURCES		= src/drivers/sctp2.c \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_adler32.c \
				  src/drivers/sctp_adler32.h \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_crc32c.h
libOS7sctp_a_CPPFLAGS		= -I$(top_builddir)/src/drivers -I$(top_srcdir)/src/drivers \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7sctp_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7sctp_a_OBJECTS):	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

$(kpre)sctp.o: $(libOS7sctp_a_OBJECTS)
	$(LD) -r -o $@ $^

## RPM_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

EXTRA_DOCS			+= doc/drafts/isua/draft-bidulock-sigtran-isua-03.txt \
				   doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-ig-00.txt \
				   doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-mib-00.txt \
				   doc/drafts/m2pa/draft-bidulock-sigtran-m2pa-test-07.txt \
				   doc/drafts/m2pa/draft-george-sigtran-m2pa-interop-01.txt \
				   doc/drafts/m2ua/draft-bidulock-sigtran-m2ua-ss7test-02.txt \
				   doc/drafts/m3pa/draft-averson-sigtran-m3pa-00.txt \
				   doc/drafts/m3pa/draft-bidulock-sigtran-m3pa-01.txt \
				   doc/drafts/m3ua/draft-bidulock-sigtran-m3gw-00.txt \
				   doc/drafts/m3ua/draft-bidulock-sigtran-m3peer-01.txt \
				   doc/drafts/m3ua/draft-bidulock-sigtran-m3ua-test-00.txt \
				   doc/drafts/sctp/draft-bidulock-sigtran-gapalg-00.txt \
				   doc/drafts/sctp/draft-bidulock-sigtran-sctpcongestion-00.txt \
				   doc/drafts/sctp/draft-bidulock-sigtran-sctptli-00.txt \
				   doc/drafts/sctp/draft-bidulock-tsvwg-sctpnpi-00.txt \
				   doc/drafts/sctp/draft-bidulock-tsvwg-sctptpi-00.txt \
				   doc/drafts/sua/draft-bidulock-sigtran-suaco-00.txt \
				   doc/drafts/sua/draft-bidulock-sigtran-sua-test-00.txt \
				   doc/drafts/tua/draft-bidulock-sigtran-tua-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-as-00.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-aspcong-00.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-aspext-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-corid-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-datamodel-00.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-loadgrp-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-loadsel-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-multisg-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-mutilsg-00.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-regext-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-sessid-04.txt \
				   doc/drafts/ua/draft-bidulock-sigtran-sginfo-05.txt

EXTRA_DIST			+= $(EXTRA_DOCS)

EXTRA_TEX			+= scripts/texinfo.cnf \
				   scripts/texi/args.texi.in \
				   scripts/texi/fdl.texi \
				   scripts/texi/gpl.texi \
				   scripts/texi/install.texi \
				   scripts/texi/lesser.texi \
				   scripts/texi/testsuite.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= scripts/missing2 scripts/missing3
EXTRA_SCRIPTS			+= scripts/missing2 scripts/missing3

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libOS7specfs_a_OBJECTS) \
				  $(libOS7streams_a_OBJECTS) \
				  $(libOS7kernel_a_OBJECTS) \
				  $(libOS7modules_a_OBJECTS) \
				  $(libOS7drivers_a_OBJECTS) \
				  $(libOS7sctp_a_OBJECTS)
KERNEL_MODULES			= specfs.o \
				  streams.o \
				  $(libOS7kernel_a_OBJECTS) \
				  $(libOS7modules_a_OBJECTS) \
				  $(libOS7drivers_a_OBJECTS) \
				  $(kpre)sctp.o
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			=

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
nodist_modutil_DATA		= src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
LIBRARY_VERSION_FILES		= src/lib/streams.VERSION \
				  src/lib/cdiapi.VERSION \
				  src/lib/dlpiapi.VERSION \
				  src/lib/npiapi.VERSION \
				  src/lib/dlpi.VERSION \
				  src/lib/xnet.VERSION \
				  src/lib/xnsl.VERSION \
				  src/lib/socklib.VERSION \
				  src/lib/socket.VERSION \
				  src/lib/sockpath.VERSION \
				  src/lib/apli.VERSION \
				  src/lib/osi.VERSION \
				  src/lib/sx25.VERSION \
				  src/lib/xap_rose.VERSION \
				  src/lib/xap.VERSION \
				  src/lib/xds.VERSION \
				  src/lib/xftam.VERSION \
				  src/lib/xmap.VERSION \
				  src/lib/xmp.VERSION \
				  src/lib/xom.VERSION \
				  src/lib/xtcap.VERSION

include am/libraries.am

if RPM_BUILD_USER
## RPM_BUILD_USER

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

## =====================================================================

LIB_INCLUDES			= -I$(top_builddir)/src/lib -I$(top_srcdir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

lib_LTLIBRARIES			+= libstreams.la

libstreams_la_SOURCES		= src/lib/testcancel.c \
				  src/lib/putpmsg.c \
				  src/lib/putmsg.c \
				  src/lib/getpmsg.c \
				  src/lib/getmsg.c \
				  src/lib/isastream.c \
				  src/lib/pipe.c \
				  src/lib/fattach.c \
				  src/lib/fdetach.c \
				  src/lib/strlog.c \
				  src/lib/streams.h
libstreams_la_CPPFLAGS		= $(LIB_INCLUDES)
libstreams_la_CFLAGS		= $(USER_CFLAGS)
libstreams_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libcdiapi.la

libcdiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/cdiapi.c \
				  src/include/cdiapi.h \
				  gettext.h
libcdiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libcdiapi_la_CFLAGS		= $(USER_CFLAGS)
libcdiapi_la_LIBADD		= libstreams.la
libcdiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/cdiapi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libdlpiapi.la

libdlpiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/dlpiapi.c \
				  src/include/dlpiapi.h \
				  gettext.h
libdlpiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libdlpiapi_la_CFLAGS		= $(USER_CFLAGS)
libdlpiapi_la_LIBADD		= libstreams.la
libdlpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpiapi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libnpiapi.la

libnpiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/npiapi.c \
				  src/include/npiapi.h \
				  gettext.h
libnpiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libnpiapi_la_CFLAGS		= $(USER_CFLAGS)
libnpiapi_la_LIBADD		= libstreams.la
libnpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/npiapi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libdlpi.la

libdlpi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/dlpi.c \
				  src/include/libdlpi.h \
				  gettext.h
libdlpi_la_CPPFLAGS		= $(LIB_INCLUDES)
libdlpi_la_CFLAGS		= $(USER_CFLAGS)
libdlpi_la_LIBADD		= libstreams.la
libdlpi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libxnet.la

libxnet_la_SOURCES		= src/lib/pthread.c \
				  src/lib/xnet.c \
				  gettext.h
libxnet_la_CPPFLAGS		= $(LIB_INCLUDES)
libxnet_la_CFLAGS		= $(USER_CFLAGS)
libxnet_la_LIBADD		= libstreams.la
libxnet_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnet.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libxnsl.la

libxnsl_la_SOURCES		= src/lib/pthread.c \
				  src/lib/netselect.c \
				  src/lib/netselect.h \
				  src/lib/netdir.c \
				  src/lib/netdir_inet.c \
				  src/include/netconfig.h \
				  src/include/netdir.h \
				  src/include/sys/netconfig.h \
				  gettext.h
libxnsl_la_CPPFLAGS		= $(LIB_INCLUDES)
libxnsl_la_CFLAGS		= $(USER_CFLAGS)
libxnsl_la_LIBADD		= libstreams.la libxnet.la
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  -ldl

## =====================================================================

lib_LTLIBRARIES			+= inetaddr.la

inetaddr_la_SOURCES		= src/lib/netdir_inet.c \
				  src/include/netdir.h \
				  src/include/netconfig.h \
				  src/include/sys/netconfig.h
inetaddr_la_CPPFLAGS		= $(LIB_INCLUDES) -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS)
inetaddr_la_LIBADD		= libstreams.la libxnet.la libxnsl.la
inetaddr_la_LDFLAGS		= -module -avoid-version \
				  -shared

## =====================================================================

lib_LTLIBRARIES			+= libsockpath.la

libsockpath_la_SOURCES		= src/lib/pthread.c \
				  src/lib/sockpath.c \
				  src/lib/sockpath.h \
				  src/include/sockdb.h \
				  src/include/sys/sockpath.h
libsockpath_la_CPPFLAGS		= $(LIB_INCLUDES)
libsockpath_la_CFLAGS		= $(USER_CFLAGS)
libsockpath_la_LIBADD		= libstreams.la
libsockpath_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libsocket.la

libsocket_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socket.c \
				  gettext.h
libsocket_la_CPPFLAGS		= $(LIB_INCLUDES)
libsocket_la_CFLAGS		= $(USER_CFLAGS)
libsocket_la_LIBADD		= libstreams.la
libsocket_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  -static -lc -shared

## =====================================================================

lib_LTLIBRARIES			+= libsocklib.la

libsocklib_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socklib.c \
				  gettext.h
libsocklib_la_CPPFLAGS		= $(LIB_INCLUDES)
libsocklib_la_CFLAGS		= $(USER_CFLAGS)
libsocklib_la_LIBADD		= libstreams.la libxnet.la libxnsl.la libsockpath.la
libsocklib_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  -static -lc -shared

## =====================================================================

lib_LTLIBRARIES			+= libsx25.la

libsx25_la_SOURCES		= src/lib/padent.c \
				  src/lib/x25config.c \
				  src/lib/xhostent.c \
				  src/include/netx25/x25_control.h \
				  src/include/netx25/x25_proto.h \
				  src/include/netx25/xnetdb.h
libsx25_la_CPPFLAGS		= $(LIB_INCLUDES)
libsx25_la_CFLAGS		= $(USER_CFLAGS)
libsx25_la_LIBADD		= libstreams.la
libsx25_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sx25.VERSION

## =====================================================================

if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= strMIB.la
endif

strMIB_la_SOURCES		= src/snmp/strMIB.c \
				  src/snmp/strMIB.h \
				  src/snmp/strExtMIB.c \
				  src/snmp/strExtMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
strMIB_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
strMIB_la_CFLAGS		= $(USER_CFLAGS)
strMIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)
strMIB_la_LDFLAGS		= -module -avoid-version -shared $(SNMP_LDFLAGS)

## =====================================================================

if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= mxMIB.la
endif

mxMIB_la_SOURCES		= src/snmp/ds0Bundle.c \
				  src/snmp/ds0Bundle.h \
				  src/snmp/ds0.c \
				  src/snmp/ds0.h \
				  src/snmp/ds1EXT.c \
				  src/snmp/ds1EXT.h \
				  src/snmp/mxMIB.c \
				  src/snmp/mxMIB.h \
				  src/snmp/phyMIB.c \
				  src/snmp/phyMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
mxMIB_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
mxMIB_la_CFLAGS			= $(USER_CFLAGS)
mxMIB_la_LIBADD			= libstreams.la $(SNMP_LDADD)
mxMIB_la_LDFLAGS		= -module -avoid-version -shared $(SNMP_LDFLAGS)

## =====================================================================

if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= ss7MIB.la
endif

ss7MIB_la_SOURCES		= src/snmp/ss7MIB.c \
				  src/snmp/ss7MIB.h \
				  src/snmp/mtpMIB.c \
				  src/snmp/mtpMIB.h \
				  src/snmp/mtpOmMIB.c \
				  src/snmp/mtpOmMIB.h \
				  src/snmp/sccpMIB.c \
				  src/snmp/sccpMIB.h \
				  src/snmp/sccpOmMIB.c \
				  src/snmp/sccpOmMIB.h \
				  src/snmp/tcapMIB.c \
				  src/snmp/tcapMIB.h \
				  src/snmp/tcapOmMIB.c \
				  src/snmp/tcapOmMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
ss7MIB_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
ss7MIB_la_CFLAGS		= $(USER_CFLAGS)
ss7MIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)
ss7MIB_la_LDFLAGS		= -module -avoid-version -shared $(SNMP_LDFLAGS)

## =====================================================================

UTIL_INCLUDES			= -I$(top_builddir)/src/util -I$(top_srcidr)/src/util $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

strchg_SOURCES			= src/util/strchg.c $(CONFIG_HEADER)
strchg_CPPFLAGS			= $(UTIL_INCLUDES)
strchg_CFLAGS			= $(USER_CFLAGS)
strchg_LDADD			= libstreams.la

bin_PROGRAMS			+= strchg

## =====================================================================

strconf_SOURCES			= src/util/strconf.c $(CONFIG_HEADER)
strconf_CPPFLAGS		= $(UTIL_INCLUDES)
strconf_CFLAGS			= $(USER_CFLAGS)
strconf_LDADD			= libstreams.la

bin_PROGRAMS			+= strconf

## =====================================================================

strreset_SOURCES		= src/util/strreset.c $(CONFIG_HEADER)
strreset_CPPFLAGS		= $(UTIL_INCLUDES)
strreset_CFLAGS			= $(USER_CFLAGS)
strreset_LDADD			= libstreams.la

bin_PROGRAMS			+= strreset

## =====================================================================

asn_SOURCES			= src/util/asnyac.y src/util/asnlex.l asnyac.h $(CONFIG_HEADER)
asn_CPPFLAGS			= $(UTIL_INCLUDES)
asn_CFLAGS			= $(USER_CFLAGS)

EXTRA_PROGRAMS			+= asn
#bin_PROGRAMS			+= asn

## =====================================================================

gdmo_SOURCES			= src/util/gdmoyac.y src/util/gdmolex.l gdmoyac.h $(CONFIG_HEADER)
gdmo_CPPFLAGS			= $(UTIL_INCLUDES)
gdmo_CFLAGS			= $(USER_CFLAGS)

bin_PROGRAMS			+= gdmo

## =====================================================================

autopush_SOURCES		= src/util/autopush.c $(CONFIG_HEADER)
autopush_CPPFLAGS		= $(UTIL_INCLUDES)
autopush_CFLAGS			= $(USER_CFLAGS)
autopush_LDADD			= libstreams.la

sbin_PROGRAMS			+= autopush

## =====================================================================

fattach_SOURCES			= src/util/fattach.c $(CONFIG_HEADER)
fattach_CPPFLAGS		= $(UTIL_INCLUDES)
fattach_CFLAGS			= $(USER_CFLAGS)
fattach_LDADD			= libstreams.la

sbin_PROGRAMS			+= fattach

## =====================================================================

fdetach_SOURCES			= src/util/fdetach.c $(CONFIG_HEADER)
fdetach_CPPFLAGS		= $(UTIL_INCLUDES)
fdetach_CFLAGS			= $(USER_CFLAGS)
fdetach_LDADD			= libstreams.la

sbin_PROGRAMS			+= fdetach

## =====================================================================

insf_SOURCES			= src/util/insf.c $(CONFIG_HEADER)
insf_CPPFLAGS			= $(UTIL_INCLUDES)
insf_CFLAGS			= $(USER_CFLAGS)
insf_LDADD			= libstreams.la

sbin_PROGRAMS			+= insf

## =====================================================================

scls_SOURCES			= src/util/scls.c $(CONFIG_HEADER)
scls_CPPFLAGS			= $(UTIL_INCLUDES)
scls_CFLAGS			= $(USER_CFLAGS)
scls_LDADD			= libstreams.la

sbin_PROGRAMS			+= scls

## =====================================================================

strace_SOURCES			= src/util/strace.c $(CONFIG_HEADER)
strace_CPPFLAGS			= $(UTIL_INCLUDES)
strace_CFLAGS			= $(USER_CFLAGS)
strace_LDADD			= libstreams.la

sbin_PROGRAMS			+= strace

## =====================================================================

##strclean_SOURCES		= src/util/strclean.c $(CONFIG_HEADER)
##strclean_CPPFLAGS		= $(UTIL_INCLUDES)
##strclean_CFLAGS		= $(USER_CFLAGS)
##strclean_LDADD		= libstreams.la

#sbin_PROGRAMS			+= strclean

##@srcdir@/src/util/strclean.c:
##	touch $@

##@srcdir@/src/util/strclean: @srcdir@/src/util/strclean.c
##	touch $@

dist_sbin_SCRIPTS		+= src/util/strclean

## =====================================================================

strerr_SOURCES			= src/util/strerr.c $(CONFIG_HEADER)
strerr_CPPFLAGS			= $(UTIL_INCLUDES)
strerr_CFLAGS			= $(USER_CFLAGS)
strerr_LDADD			= libstreams.la

sbin_PROGRAMS			+= strerr

## =====================================================================

strinfo_SOURCES			= src/util/strinfo.c $(CONFIG_HEADER)
strinfo_CPPFLAGS		= $(UTIL_INCLUDES)
strinfo_CFLAGS			= $(USER_CFLAGS)
strinfo_LDADD			= libstreams.la

sbin_PROGRAMS			+= strinfo

## =====================================================================

##strload_SOURCES		= src/util/strload.c $(CONFIG_HEADER)
##strload_CPPFLAGS		= $(UTIL_INCLUDES)
##strload_CFLAGS		= $(USER_CFLAGS)
##strload_LDADD			= libstreams.la

#sbin_PROGRAMS			+= strload

##@srcdir@/src/util/strload.c:
##	touch $@

##@srcdir@/src/util/strload: @srcdir@/src/util/strload.c
##	touch $@

dist_sbin_SCRIPTS		+= src/util/strload

## =====================================================================

##strsetup_SOURCES		= src/util/strsetup.c $(CONFIG_HEADER)
##strsetup_CPPFLAGS		= $(UTIL_INCLUDES)
##strsetup_CFLAGS		= $(USER_CFLAGS)
##strsetup_LDADD		= libstreams.la

#sbin_PROGRAMS			+= strsetup

##@srcdir@/src/util/strsetup.c:
##	touch $@

##@srcdir@/src/util/strsetup: @srcdir@/src/util/strsetup.c
##	touch $@

dist_sbin_SCRIPTS		+= src/util/strsetup

## =====================================================================

strvf_SOURCES			= src/util/strvf.c $(CONFIG_HEADER)
strvf_CPPFLAGS			= $(UTIL_INCLUDES)
strvf_CFLAGS			= $(USER_CFLAGS)
strvf_LDADD			= libstreams.la

sbin_PROGRAMS			+= strvf

## =====================================================================

@PACKAGE_TARNAME@_mknod_SOURCES		= src/util/@PACKAGE_TARNAME@_mknod.c $(CONFIG_HEADER)
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(UTIL_INCLUDES)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

sbin_PROGRAMS			+= @PACKAGE_TARNAME@_mknod

## =====================================================================

streamsd_SOURCES		= src/snmp/streamsd.c \
				  src/snmp/streamsd.h \
				  src/snmp/strMIB.c \
				  src/snmp/strMIB.h \
				  src/snmp/strExtMIB.c \
				  src/snmp/strExtMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h \
				  $(CONFIG_HEADER)
streamsd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
streamsd_CFLAGS			= $(USER_CFLAGS)
streamsd_LDADD			= libstreams.la $(SNMP_LDADD)
streamsd_LDFLAGS		= $(SNMP_LDFLAGS)

if WITH_SNMP_AGENT
sbin_PROGRAMS			+= streamsd
endif
EXTRA_PROGRAMS			+= streamsd

## =====================================================================

ldlconfig_SOURCES		= src/util/ldlconfig.c $(CONFIG_HEADER)
ldlconfig_CPPFLAGS		= $(UTIL_INCLUDES)
ldlconfig_CFLAGS		= $(USER_CFLAGS)
ldlconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= ldlconfig

## =====================================================================

initsock_SOURCES		= src/util/initsock.c $(CONFIG_HEADER)
initsock_CPPFLAGS		= $(UTIL_INCLUDES)
initsock_CFLAGS			= $(USER_CFLAGS)
initsock_LDADD			= libstreams.la

sbin_PROGRAMS			+= initsock

## =====================================================================

soconfig_SOURCES		= src/util/soconfig.c $(CONFIG_HEADER)
soconfig_CPPFLAGS		= $(UTIL_INCLUDES)
soconfig_CFLAGS			= $(USER_CFLAGS)
soconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= soconfig

## =====================================================================

chand_SOURCES			= src/snmp/chand.c \
				  src/snmp/chand.h \
				  src/snmp/ds0Bundle.c \
				  src/snmp/ds0Bundle.h \
				  src/snmp/ds0.c \
				  src/snmp/ds0.h \
				  src/snmp/ds1EXT.c \
				  src/snmp/ds1EXT.h \
				  src/snmp/mxMIB.c \
				  src/snmp/mxMIB.h \
				  src/snmp/phyMIB.c \
				  src/snmp/phyMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h \
				  $(CONFIG_HEADER)
chand_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS)
chand_CFLAGS			= $(USER_CFLAGS)
chand_LDADD			= libstreams.la $(SNMP_LDADD)
chand_LDFLAGS			= $(SNMP_LDFLAGS)

if WITH_SNMP_AGENT
sbin_PROGRAMS			+= chand
endif
EXTRA_PROGRAMS			+= chand

## =====================================================================

ifrdlci_SOURCES			= src/util/ifrdlci.c $(CONFIG_HEADER)
ifrdlci_CPPFLAGS		= $(UTIL_INCLUDES)
ifrdlci_CFLAGS			= $(USER_CFLAGS)
ifrdlci_LDADD			= libstreams.la

sbin_PROGRAMS			+= ifrdlci

## =====================================================================

ifrstat_SOURCES			= src/util/ifrstat.c $(CONFIG_HEADER)
ifrstat_CPPFLAGS		= $(UTIL_INCLUDES)
ifrstat_CFLAGS			= $(USER_CFLAGS)
ifrstat_LDADD			= libstreams.la

sbin_PROGRAMS			+= ifrstat

## =====================================================================

ifrtune_SOURCES			= src/util/ifrtune.c $(CONFIG_HEADER)
ifrtune_CPPFLAGS		= $(UTIL_INCLUDES)
ifrtune_CFLAGS			= $(USER_CFLAGS)
ifrtune_LDADD			= libstreams.la

sbin_PROGRAMS			+= ifrtune

## =====================================================================

ixemap_SOURCES			= src/util/ixemap.c $(CONFIG_HEADER)
ixemap_CPPFLAGS			= $(UTIL_INCLUDES)
ixemap_CFLAGS			= $(USER_CFLAGS)
ixemap_LDADD			= libstreams.la

sbin_PROGRAMS			+= ixemap

## =====================================================================

ixetune_SOURCES			= src/util/ixetune.c $(CONFIG_HEADER)
ixetune_CPPFLAGS		= $(UTIL_INCLUDES)
ixetune_CFLAGS			= $(USER_CFLAGS)
ixetune_LDADD			= libstreams.la

sbin_PROGRAMS			+= ixetune

## =====================================================================

linkadd_SOURCES			= src/util/linkadd.c $(CONFIG_HEADER)
linkadd_CPPFLAGS		= $(UTIL_INCLUDES)
linkadd_CFLAGS			= $(USER_CFLAGS)
linkadd_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkadd

## =====================================================================

linkdel_SOURCES			= src/util/linkdel.c $(CONFIG_HEADER)
linkdel_CPPFLAGS		= $(UTIL_INCLUDES)
linkdel_CFLAGS			= $(USER_CFLAGS)
linkdel_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkdel

## =====================================================================

linklist_SOURCES		= src/util/linklist.c $(CONFIG_HEADER)
linklist_CPPFLAGS		= $(UTIL_INCLUDES)
linklist_CFLAGS			= $(USER_CFLAGS)
linklist_LDADD			= libstreams.la

sbin_PROGRAMS			+= linklist

## =====================================================================

linkreset_SOURCES		= src/util/linkreset.c $(CONFIG_HEADER)
linkreset_CPPFLAGS		= $(UTIL_INCLUDES)
linkreset_CFLAGS		= $(USER_CFLAGS)
linkreset_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkreset

## =====================================================================

linkstart_SOURCES		= src/util/linkstart.c $(CONFIG_HEADER)
linkstart_CPPFLAGS		= $(UTIL_INCLUDES)
linkstart_CFLAGS		= $(USER_CFLAGS)
linkstart_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkstart

## =====================================================================

linkstate_SOURCES		= src/util/linkstate.c $(CONFIG_HEADER)
linkstate_CPPFLAGS		= $(UTIL_INCLUDES)
linkstate_CFLAGS		= $(USER_CFLAGS)
linkstate_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkstate

## =====================================================================

linkstop_SOURCES		= src/util/linkstop.c $(CONFIG_HEADER)
linkstop_CPPFLAGS		= $(UTIL_INCLUDES)
linkstop_CFLAGS			= $(USER_CFLAGS)
linkstop_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkstop

## =====================================================================

lltune_SOURCES			= src/util/lltune.c $(CONFIG_HEADER)
lltune_CPPFLAGS			= $(UTIL_INCLUDES)
lltune_CFLAGS			= $(USER_CFLAGS)
lltune_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= lltune
#sbin_PROGRAMS			+= lltune

## =====================================================================

mlptune_SOURCES			= src/util/mlptune.c $(CONFIG_HEADER)
mlptune_CPPFLAGS		= $(UTIL_INCLUDES)
mlptune_CFLAGS			= $(USER_CFLAGS)
mlptune_LDADD			= libstreams.la

sbin_PROGRAMS			+= mlptune

## =====================================================================

nuimap_SOURCES			= src/util/nuimap.c $(CONFIG_HEADER)
nuimap_CPPFLAGS			= $(UTIL_INCLUDES)
nuimap_CFLAGS			= $(USER_CFLAGS)
nuimap_LDADD			= libstreams.la

sbin_PROGRAMS			+= nuimap

## =====================================================================

pvcmap_SOURCES			= src/util/pvcmap.c $(CONFIG_HEADER)
pvcmap_CPPFLAGS			= $(UTIL_INCLUDES)
pvcmap_CFLAGS			= $(USER_CFLAGS)
pvcmap_LDADD			= libstreams.la

sbin_PROGRAMS			+= pvcmap

## =====================================================================

vcstat_SOURCES			= src/util/vcstat.c $(CONFIG_HEADER)
vcstat_CPPFLAGS			= $(UTIL_INCLUDES)
vcstat_CFLAGS			= $(USER_CFLAGS)
vcstat_LDADD			= libstreams.la

sbin_PROGRAMS			+= vcstat

## =====================================================================

wantune_SOURCES			= src/util/wantune.c $(CONFIG_HEADER)
wantune_CPPFLAGS		= $(UTIL_INCLUDES)
wantune_CFLAGS			= $(USER_CFLAGS)
wantune_LDADD			= libstreams.la

sbin_PROGRAMS			+= wantune

## =====================================================================

x25diags_SOURCES		= src/util/x25diags.c $(CONFIG_HEADER)
x25diags_CPPFLAGS		= $(UTIL_INCLUDES)
x25diags_CFLAGS			= $(USER_CFLAGS)
x25diags_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25diags

## =====================================================================

x25file_SOURCES			= src/util/x25file.c $(CONFIG_HEADER)
x25file_CPPFLAGS		= $(UTIL_INCLUDES)
x25file_CFLAGS			= $(USER_CFLAGS)
x25file_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25file

## =====================================================================

x25info_SOURCES			= src/util/x25info.c $(CONFIG_HEADER)
x25info_CPPFLAGS		= $(UTIL_INCLUDES)
x25info_CFLAGS			= $(USER_CFLAGS)
x25info_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25info

## =====================================================================

x25netd_SOURCES			= src/util/x25netd.c $(CONFIG_HEADER)
x25netd_CPPFLAGS		= $(UTIL_INCLUDES)
x25netd_CFLAGS			= $(USER_CFLAGS)
x25netd_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25netd

## =====================================================================

x25route_SOURCES		= src/util/x25route.c $(CONFIG_HEADER)
x25route_CPPFLAGS		= $(UTIL_INCLUDES)
x25route_CFLAGS			= $(USER_CFLAGS)
x25route_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25route

## =====================================================================

x25stat_SOURCES			= src/util/x25stat.c $(CONFIG_HEADER)
x25stat_CPPFLAGS		= $(UTIL_INCLUDES)
x25stat_CFLAGS			= $(USER_CFLAGS)
x25stat_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25stat

## =====================================================================

x25trace_SOURCES		= src/util/x25trace.c $(CONFIG_HEADER)
x25trace_CPPFLAGS		= $(UTIL_INCLUDES)
x25trace_CFLAGS			= $(USER_CFLAGS)
x25trace_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25trace

## =====================================================================

x25tune_SOURCES			= src/util/x25tune.c $(CONFIG_HEADER)
x25tune_CPPFLAGS		= $(UTIL_INCLUDES)
x25tune_CFLAGS			= $(USER_CFLAGS)
x25tune_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= x25tune
#sbin_PROGRAMS			+= x25tune

## =====================================================================

x32map_SOURCES			= src/util/x32map.c $(CONFIG_HEADER)
x32map_CPPFLAGS			= $(UTIL_INCLUDES)
x32map_CFLAGS			= $(USER_CFLAGS)
x32map_LDADD			= libstreams.la

sbin_PROGRAMS			+= x32map

## =====================================================================

ss7statsd_SOURCES		= src/util/ss7statsd.c $(CONFIG_HEADER)
ss7statsd_CPPFLAGS		= $(UTIL_INCLUDES)
ss7statsd_CFLAGS		= $(USER_CFLAGS)
ss7statsd_LDADD			= libstreams.la

sbin_PROGRAMS			+= ss7statsd

## =====================================================================

slconfig_SOURCES		= src/util/slconf_yac.y \
				  src/util/slconf_lex.l \
				  src/util/slconf.c \
				  slconf_yac.h \
				  $(CONFIG_HEADER)
slconfig_CPPFLAGS		= $(UTIL_INCLUDES)
slconfig_CFLAGS			= $(USER_CFLAGS)
slconfig_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= slconfig
#sbin_PROGRAMS			+= slconfig

## =====================================================================

mtpconfig_SOURCES		= src/util/mtpconf_yac.y \
				  src/util/mtpconf_lex.l \
				  mtpconf_yac.h \
				  $(CONFIG_HEADER)
mtpconfig_CPPFLAGS		= $(UTIL_INCLUDES)
mtpconfig_CFLAGS		= $(USER_CFLAGS)
mtpconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= mtpconfig

## =====================================================================

x400config_SOURCES		= src/util/x400yac.y \
				  src/util/x400lex.l \
				  src/util/x400config.h \
				  x400yac.h \
				  $(CONFIG_HEADER)
x400config_CPPFLAGS		= $(UTIL_INCLUDES)
x400config_CFLAGS		= $(USER_CFLAGS)
x400config_LDADD		= libstreams.la

sbin_PROGRAMS			+= x400config

## =====================================================================

sdlconfig_SOURCES		= src/util/sdlconf.c \
				  src/util/sdlyac.y \
				  src/util/sdllex.l \
				  sdlyac.h \
				  $(CONFIG_HEADER)
sdlconfig_CPPFLAGS		= $(UTIL_INCLUDES)
sdlconfig_CFLAGS		= $(USER_CFLAGS)
sdlconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= sdlconfig

## =====================================================================

slconfd_SOURCES			= src/util/slconfd.c $(CONFIG_HEADER)
slconfd_CPPFLAGS		= $(UTIL_INCLUDES)
slconfd_CFLAGS			= $(USER_CFLAGS)
slconfd_LDADD			= libstreams.la

sbin_PROGRAMS			+= slconfd

## =====================================================================

slmon_SOURCES			= src/util/slmon.c $(CONFIG_HEADER)
slmon_CPPFLAGS			= $(UTIL_INCLUDES)
slmon_CFLAGS			= $(USER_CFLAGS)
slmon_LDADD			= libstreams.la

sbin_PROGRAMS			+= slmon

## =====================================================================

ss7capd_SOURCES			= src/util/ss7capd.c $(CONFIG_HEADER)
ss7capd_CPPFLAGS		= $(UTIL_INCLUDES)
ss7capd_CFLAGS			= $(USER_CFLAGS)
ss7capd_LDADD			= libstreams.la

sbin_PROGRAMS			+= ss7capd

## =====================================================================

m2paconfig_SOURCES		= src/util/m2payac.y \
				  src/util/m2palex.l \
				  m2payac.h \
				  $(CONFIG_HEADER)
m2paconfig_CPPFLAGS		= $(UTIL_INCLUDES)
m2paconfig_CFLAGS		= $(USER_CFLAGS)
m2paconfig_LDADD		= libstreams.la

sbin_PROGRAMS			+= m2paconfig

## ---------------------------------------------------------------------
## pkglibexec_PROGRAMS
## ---------------------------------------------------------------------
## =====================================================================

TEST_INCLUDES			= -I$(top_builddir)/src/test -I$(top_srcdir)/src/test $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

test_clone_SOURCES		= src/test/test-clone.c $(CONFIG_HEADER)
test_clone_CPPFLAGS		= $(TEST_INCLUDES)
test_clone_CFLAGS		= $(USER_CFLAGS)
test_clone_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-clone

## =====================================================================

test_connld_SOURCES		= src/test/test-connld.c $(CONFIG_HEADER)
test_connld_CPPFLAGS		= $(TEST_INCLUDES)
test_connld_CFLAGS		= $(USER_CFLAGS)
test_connld_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-connld

## =====================================================================

test_echo_SOURCES		= src/test/test-echo.c $(CONFIG_HEADER)
test_echo_CPPFLAGS		= $(TEST_INCLUDES)
test_echo_CFLAGS		= $(USER_CFLAGS)
test_echo_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-echo

## =====================================================================

test_fifo_SOURCES		= src/test/test-fifo.c $(CONFIG_HEADER)
test_fifo_CPPFLAGS		= $(TEST_INCLUDES)
test_fifo_CFLAGS		= $(USER_CFLAGS)
test_fifo_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-fifo

## =====================================================================

test_log_SOURCES		= src/test/test-log.c $(CONFIG_HEADER)
test_log_CPPFLAGS		= $(TEST_INCLUDES)
test_log_CFLAGS			= $(USER_CFLAGS)
test_log_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-log

## =====================================================================

test_loop_SOURCES		= src/test/test-loop.c $(CONFIG_HEADER)
test_loop_CPPFLAGS		= $(TEST_INCLUDES)
test_loop_CFLAGS		= $(USER_CFLAGS)
test_loop_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-loop

## =====================================================================

test_nsdev_SOURCES		= src/test/test-nsdev.c $(CONFIG_HEADER)
test_nsdev_CPPFLAGS		= $(TEST_INCLUDES)
test_nsdev_CFLAGS		= $(USER_CFLAGS)
test_nsdev_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-nsdev

## =====================================================================

test_mux_SOURCES		= src/test/test-mux.c $(CONFIG_HEADER)
test_mux_CPPFLAGS		= $(TEST_INCLUDES)
test_mux_CFLAGS			= $(USER_CFLAGS)
test_mux_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-mux

## =====================================================================

test_nuls_SOURCES		= src/test/test-nuls.c $(CONFIG_HEADER)
test_nuls_CPPFLAGS		= $(TEST_INCLUDES)
test_nuls_CFLAGS		= $(USER_CFLAGS)
test_nuls_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-nuls

## =====================================================================

test_pipe_SOURCES		= src/test/test-pipe.c $(CONFIG_HEADER)
test_pipe_CPPFLAGS		= $(TEST_INCLUDES)
test_pipe_CFLAGS		= $(USER_CFLAGS)
test_pipe_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-pipe

## =====================================================================

perftest_SOURCES		= src/test/perftest.c $(CONFIG_HEADER)
perftest_CPPFLAGS		= $(TEST_INCLUDES)
perftest_CFLAGS			= $(USER_CFLAGS)
perftest_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= perftest

## =====================================================================

perftestn_SOURCES		= src/test/perftest.c $(CONFIG_HEADER)
perftestn_CPPFLAGS		= $(TEST_INCLUDES) -DNATIVE_PIPES
perftestn_CFLAGS		= $(USER_CFLAGS)

pkglibexec_PROGRAMS		+= perftestn

## =====================================================================

test_pipemod_SOURCES		= src/test/test-pipemod.c $(CONFIG_HEADER)
test_pipemod_CPPFLAGS		= $(TEST_INCLUDES)
test_pipemod_CFLAGS		= $(USER_CFLAGS)
test_pipemod_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-pipemod

## =====================================================================

test_sad_SOURCES		= src/test/test-sad.c $(CONFIG_HEADER)
test_sad_CPPFLAGS		= $(TEST_INCLUDES)
test_sad_CFLAGS			= $(USER_CFLAGS)
test_sad_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sad

## =====================================================================

test_sc_SOURCES			= src/test/test-sc.c $(CONFIG_HEADER)
test_sc_CPPFLAGS		= $(TEST_INCLUDES)
test_sc_CFLAGS			= $(USER_CFLAGS)
test_sc_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sc

## =====================================================================

test_streams_SOURCES		= src/test/test-streams.c $(CONFIG_HEADER)
test_streams_CPPFLAGS		= $(TEST_INCLUDES)
test_streams_CFLAGS		= $(USER_CFLAGS)
test_streams_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-streams

## =====================================================================

testlog_SOURCES			= src/test/testlog.c $(CONFIG_HEADER)
testlog_CPPFLAGS		= $(TEST_INCLUDES)
testlog_CFLAGS			= $(USER_CFLAGS)
testlog_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= testlog

## =====================================================================

test_tty_SOURCES		= src/test/test-tty.c $(CONFIG_HEADER)
test_tty_CPPFLAGS		= $(TEST_INCLUDES)
test_tty_CFLAGS			= $(USER_CFLAGS)
test_tty_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-tty

## =====================================================================

ldltest_SOURCES			= src/test/ldltest.c $(CONFIG_HEADER)
ldltest_CPPFLAGS		= $(TEST_INCLUDES)
ldltest_CFLAGS			= $(USER_CFLAGS)
ldltest_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= ldltest

## =====================================================================

test_np_ip_SOURCES		= src/test/test-np_ip.c $(CONFIG_HEADER)
test_np_ip_CPPFLAGS		= $(TEST_INCLUDES)
test_np_ip_CFLAGS		= $(USER_CFLAGS)
test_np_ip_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-np_ip

## =====================================================================

test_ip_SOURCES			= src/test/test-ip.c $(CONFIG_HEADER)
test_ip_CPPFLAGS		= $(TEST_INCLUDES)
test_ip_CFLAGS			= $(USER_CFLAGS)
test_ip_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-ip

## =====================================================================

test_tirdwr_SOURCES		= src/test/test-tirdwr.c $(CONFIG_HEADER)
test_tirdwr_CPPFLAGS		= $(TEST_INCLUDES)
test_tirdwr_CFLAGS		= $(USER_CFLAGS)
test_tirdwr_LDADD		= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-tirdwr

## =====================================================================

test_timod_SOURCES		= src/test/test-timod.c $(CONFIG_HEADER)
test_timod_CPPFLAGS		= $(TEST_INCLUDES)
test_timod_CFLAGS		= $(USER_CFLAGS)
test_timod_LDADD		= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-timod

## =====================================================================

test_xnet_SOURCES		= src/test/test-xnet.c $(CONFIG_HEADER)
test_xnet_CPPFLAGS		= $(TEST_INCLUDES)
test_xnet_CFLAGS		= $(USER_CFLAGS)
test_xnet_LDADD			= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-xnet

## =====================================================================

test_xnet_thr_SOURCES		= src/test/test-xnet.c $(CONFIG_HEADER)
test_xnet_thr_CPPFLAGS		= $(TEST_INCLUDES) -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS)
test_xnet_thr_LDADD		= libstreams.la libxnet.la
test_xnet_thr_LDFLAGS		= -lpthread

pkglibexec_PROGRAMS		+= test-xnet_thr

## =====================================================================

test_xnsl_SOURCES		= src/test/test-xnsl.c $(CONFIG_HEADER)
test_xnsl_CPPFLAGS		= $(TEST_INCLUDES)
test_xnsl_CFLAGS		= $(USER_CFLAGS)
test_xnsl_LDADD			= libstreams.la libxnet.la libxnsl.la

pkglibexec_PROGRAMS		+= test-xnsl

## =====================================================================

test_xnsl_thr_SOURCES		= src/test/test-xnsl.c $(CONFIG_HEADER)
test_xnsl_thr_CPPFLAGS		= $(TEST_INCLUDES) -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS)
test_xnsl_thr_LDADD		= libstreams.la libxnet.la libxnsl.la
test_xnsl_thr_LDFLAGS		= -lpthread

pkglibexec_PROGRAMS		+= test-xnsl_thr

## =====================================================================

test_sockpath_SOURCES		= src/test/test-sockpath.c $(CONFIG_HEADER)
test_sockpath_CPPFLAGS		= $(TEST_INCLUDES)
test_sockpath_CFLAGS		= $(USER_CFLAGS)
test_sockpath_LDADD		= libstreams.la libsockpath.la

pkglibexec_PROGRAMS		+= test-sockpath

## =====================================================================

test_socket_SOURCES		= src/test/test-socket.c $(CONFIG_HEADER)
test_socket_CPPFLAGS		= $(TEST_INCLUDES)
test_socket_CFLAGS		= $(USER_CFLAGS)
test_socket_LDADD		= libstreams.la libsocket.la

pkglibexec_PROGRAMS		+= test-socket

## =====================================================================

test_socklib_SOURCES		= src/test/test-socklib.c $(CONFIG_HEADER)
test_socklib_CPPFLAGS		= $(TEST_INCLUDES)
test_socklib_CFLAGS		= $(USER_CFLAGS)
test_socklib_LDADD		= libstreams.la libsocklib.la

pkglibexec_PROGRAMS		+= test-socklib

## =====================================================================

test_sockmod_SOURCES		= src/test/test-sockmod.c $(CONFIG_HEADER)
test_sockmod_CPPFLAGS		= $(TEST_INCLUDES)
test_sockmod_CFLAGS		= $(USER_CFLAGS)
test_sockmod_LDADD		= libstreams.la libsocklib.la

pkglibexec_PROGRAMS		+= test-sockmod

## =====================================================================

test_socksys_SOURCES		= src/test/test-socksys.c $(CONFIG_HEADER)
test_socksys_CPPFLAGS		= $(TEST_INCLUDES)
test_socksys_CFLAGS		= $(USER_CFLAGS)
test_socksys_LDADD		= libstreams.la libsocket.la

pkglibexec_PROGRAMS		+= test-socksys

## =====================================================================

test_inet_raw_SOURCES		= src/test/test-inet_raw.c $(CONFIG_HEADER)
test_inet_raw_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_raw_CFLAGS		= $(USER_CFLAGS)
test_inet_raw_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_raw

## =====================================================================

test_inet_udp_SOURCES		= src/test/test-inet_udp.c $(CONFIG_HEADER)
test_inet_udp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_udp_CFLAGS		= $(USER_CFLAGS)
test_inet_udp_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_udp

## =====================================================================

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c $(CONFIG_HEADER)
test_inet_tcp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)
test_inet_tcp_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_tcp

## =====================================================================

test_inet_sctp_SOURCES		= src/test/test-inet_sctp.c $(CONFIG_HEADER)
test_inet_sctp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_sctp_CFLAGS		= $(USER_CFLAGS)
test_inet_sctp_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_sctp

## =====================================================================

test_sctp_SOURCES		= src/test/test-sctp.c $(CONFIG_HEADER)
test_sctp_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_CFLAGS		= $(USER_CFLAGS)
test_sctp_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp

## =====================================================================

test_sctp_n_SOURCES		= src/test/test-sctp_n.c $(CONFIG_HEADER)
test_sctp_n_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_n_CFLAGS		= $(USER_CFLAGS)
test_sctp_n_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_n

## =====================================================================

test_sctp_n2_SOURCES		= src/test/test-sctp_n2.c $(CONFIG_HEADER)
test_sctp_n2_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_n2_CFLAGS		= $(USER_CFLAGS)
test_sctp_n2_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_n2

## =====================================================================

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c $(CONFIG_HEADER)
test_sctp_nc_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS)
test_sctp_nc_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_nc

## =====================================================================

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c $(CONFIG_HEADER)
test_sctp_ns_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS)
test_sctp_ns_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_ns

## =====================================================================

test_sctp_t_SOURCES		= src/test/test-sctp_t.c $(CONFIG_HEADER)
test_sctp_t_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_t_CFLAGS		= $(USER_CFLAGS)
test_sctp_t_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_t

## =====================================================================

test_etsi_n_SOURCES		= src/test/test-etsi_n.c $(CONFIG_HEADER)
test_etsi_n_CPPFLAGS		= $(TEST_INCLUDES)
test_etsi_n_CFLAGS		= $(USER_CFLAGS)
test_etsi_n_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-etsi_n

## =====================================================================

test_etsi_t_SOURCES		= src/test/test-etsi_t.c $(CONFIG_HEADER)
test_etsi_t_CPPFLAGS		= $(TEST_INCLUDES)
test_etsi_t_CFLAGS		= $(USER_CFLAGS)
test_etsi_t_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-etsi_t

## =====================================================================

test_interop_SOURCES		= src/test/test-interop.c $(CONFIG_HEADER)
test_interop_CPPFLAGS		= $(TEST_INCLUDES)
test_interop_CFLAGS		= $(USER_CFLAGS)
test_interop_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-interop

## =====================================================================

test_chan_SOURCES		= src/test/test-chan.c $(CONFIG_HEADER)
test_chan_CPPFLAGS		= $(TEST_INCLUDES)
test_chan_CFLAGS		= $(USER_CFLAGS)
test_chan_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-chan

## =====================================================================

test_isdn_SOURCES		= src/test/test-isdn.c $(CONFIG_HEADER)
test_isdn_CPPFLAGS		= $(TEST_INCLUDES)
test_isdn_CFLAGS		= $(USER_CFLAGS)
test_isdn_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-isdn

## =====================================================================

test_atm_SOURCES		= src/test/test-atm.c $(CONFIG_HEADER)
test_atm_CPPFLAGS		= $(TEST_INCLUDES)
test_atm_CFLAGS			= $(USER_CFLAGS)
test_atm_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-atm

## =====================================================================

test_sl_SOURCES			= src/test/test-sl.c $(CONFIG_HEADER)
test_sl_CPPFLAGS		= $(TEST_INCLUDES)
test_sl_CFLAGS			= $(USER_CFLAGS)
test_sl_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sl

## =====================================================================

test_tcpc_SOURCES		= src/test/test-tcpc.c $(CONFIG_HEADER)
test_tcpc_CPPFLAGS		= $(TEST_INCLUDES)
test_tcpc_CFLAGS		= $(USER_CFLAGS)
test_tcpc_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-tcpc

## =====================================================================

test_tcps_SOURCES		= src/test/test-tcps.c $(CONFIG_HEADER)
test_tcps_CPPFLAGS		= $(TEST_INCLUDES)
test_tcps_CFLAGS		= $(USER_CFLAGS)
test_tcps_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-tcps

## =====================================================================

test_udpc_SOURCES		= src/test/test-udpc.c $(CONFIG_HEADER)
test_udpc_CPPFLAGS		= $(TEST_INCLUDES)
test_udpc_CFLAGS		= $(USER_CFLAGS)
test_udpc_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-udpc

## =====================================================================

test_udps_SOURCES		= src/test/test-udps.c $(CONFIG_HEADER)
test_udps_CPPFLAGS		= $(TEST_INCLUDES)
test_udps_CFLAGS		= $(USER_CFLAGS)
test_udps_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-udps

## =====================================================================

test_q784_SOURCES		= src/test/test-q784.c $(CONFIG_HEADER)
test_q784_CPPFLAGS		= $(TEST_INCLUDES)
test_q784_CFLAGS		= $(USER_CFLAGS)
test_q784_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= test-q784
#pkglibexec_PROGRAMS		+= test-q784

## =====================================================================

#perftest_SOURCES		= src/test/perftest.c $(CONFIG_HEADER)
#perftest_CPPFLAGS		= $(TEST_INCLUDES)
#perftest_CFLAGS		= $(USER_CFLAGS)
#perftest_LDADD			= libstreams.la

#pkglibexec_PROGRAMS		+= perftest

## =====================================================================

test_q781_SOURCES		= src/test/test-q781.c $(CONFIG_HEADER)
test_q781_CPPFLAGS		= $(TEST_INCLUDES)
test_q781_CFLAGS		= $(USER_CFLAGS)
test_q781_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-q781

## =====================================================================

test_q781_pipe_SOURCES		= src/test/test-q781-pipe.c $(CONFIG_HEADER)
test_q781_pipe_CPPFLAGS		= $(TEST_INCLUDES)
test_q781_pipe_CFLAGS		= $(USER_CFLAGS)
test_q781_pipe_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-q781-pipe

## =====================================================================

test_x100p_SOURCES		= src/test/test-x100p.c $(CONFIG_HEADER)
test_x100p_CPPFLAGS		= $(TEST_INCLUDES)
test_x100p_CFLAGS		= $(USER_CFLAGS)
test_x100p_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x100p

## =====================================================================

test_x400p_SOURCES		= src/test/test-x400p.c $(CONFIG_HEADER)
test_x400p_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_CFLAGS		= $(USER_CFLAGS)
test_x400p_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p

## =====================================================================

test_usage_SOURCES		= src/util/usage.c $(CONFIG_HEADER)
test_usage_CPPFLAGS		= $(UTIL_INCLUDES)
test_usage_CFLAGS		= $(USER_CFLAGS)
test_usage_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-usage

## =====================================================================

test_x400p_cap2_SOURCES		= src/test/test-x400p-cap2.c $(CONFIG_HEADER)
test_x400p_cap2_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS)
test_x400p_cap2_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-cap2

## =====================================================================

test_x400p_cap_SOURCES		= src/test/test-x400p-cap.c $(CONFIG_HEADER)
test_x400p_cap_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_cap_CFLAGS		= $(USER_CFLAGS)
test_x400p_cap_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-cap

## =====================================================================

test_x400p_hsl_SOURCES		= src/test/test-x400p-hsl.c $(CONFIG_HEADER)
test_x400p_hsl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS)
test_x400p_hsl_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-hsl

## =====================================================================

test_x400p_perf_SOURCES		= src/test/test-x400p-perf.c $(CONFIG_HEADER)
test_x400p_perf_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_perf_CFLAGS		= $(USER_CFLAGS)
test_x400p_perf_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-perf

## =====================================================================

test_x400p_sdl_SOURCES		= src/test/test-x400p-sdl.c $(CONFIG_HEADER)
test_x400p_sdl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS)
test_x400p_sdl_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sdl

## =====================================================================

test_x400p_sdt2_SOURCES		= src/test/test-x400p-sdt2.c $(CONFIG_HEADER)
test_x400p_sdt2_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS)
test_x400p_sdt2_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sdt2

## =====================================================================

test_x400p_sdt_SOURCES		= src/test/test-x400p-sdt.c $(CONFIG_HEADER)
test_x400p_sdt_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS)
test_x400p_sdt_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sdt

## =====================================================================

test_x400p_sl_SOURCES		= src/test/test-x400p-sl.c $(CONFIG_HEADER)
test_x400p_sl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sl_CFLAGS		= $(USER_CFLAGS)
test_x400p_sl_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sl

## =====================================================================

test_sigtran_SOURCES		= src/test/test-sigtran.c $(CONFIG_HEADER)
test_sigtran_CPPFLAGS		= $(TEST_INCLUDES)
test_sigtran_CFLAGS		= $(USER_CFLAGS)
test_sigtran_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sigtran

## =====================================================================

test_m2pa_SOURCES		= src/test/test-m2pa.c $(CONFIG_HEADER)
test_m2pa_CPPFLAGS		= $(TEST_INCLUDES)
test_m2pa_CFLAGS		= $(USER_CFLAGS)
test_m2pa_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-m2pa

## =====================================================================

test_m2ua_as_SOURCES		= src/test/test-m2ua_as.c $(CONFIG_HEADER)
test_m2ua_as_CPPFLAGS		= $(TEST_INCLUDES)
test_m2ua_as_CFLAGS		= $(USER_CFLAGS)
test_m2ua_as_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-m2ua_as

## =====================================================================

test_m3ua_raw_SOURCES		= src/test/test-m3ua-raw.c $(CONFIG_HEADER)
test_m3ua_raw_CPPFLAGS		= $(TEST_INCLUDES)
test_m3ua_raw_CFLAGS		= $(USER_CFLAGS)
test_m3ua_raw_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-m3ua-raw

## =====================================================================

test_voip_SOURCES		= src/test/test-voip.c $(CONFIG_HEADER)
test_voip_CPPFLAGS		= $(TEST_INCLUDES)
test_voip_CFLAGS		= $(USER_CFLAGS)
test_voip_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-voip

## =====================================================================

## PKG_BUILD_ARCH
endif
## RPM_BUILD_USER
endif

EXPORT_SRCFILES			=

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_SRCFILES			+= src/kernel/strattach.c \
				   src/kernel/strlookup.c \
				   src/kernel/strpipe.c \
				   src/kernel/strreg.c \
				   src/kernel/strsad.c \
				   src/kernel/strsched.c \
				   src/kernel/strspecfs.c \
				   src/kernel/strsysctl.c \
				   src/kernel/strutil.c \
				   src/kernel/strsyms.c

## CONFIG_STREAMS_SEPARATE_COMPILE
else
## !CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_SRCFILES			+= src/kernel/specfs.c \
				   src/kernel/streams.c

## !CONFIG_STREAMS_SEPARATE_COMPILE
endif

EXPORT_SRCFILES			+= src/drivers/clone.c \
				   src/modules/sth.c \
				   src/kernel/aixcompat.c \
				   src/kernel/hpuxcompat.c \
				   src/kernel/irixcompat.c \
				   src/kernel/maccompat.c \
				   src/kernel/mpscompat.c \
				   src/kernel/os7compat.c \
				   src/kernel/osfcompat.c \
				   src/kernel/suncompat.c \
				   src/kernel/svr3compat.c \
				   src/kernel/svr4compat.c \
				   src/kernel/uw7compat.c

EXPORT_VERFILES			=

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_VERFILES			+= $(PKGINCL)/versions/strattach.ver \
				   $(PKGINCL)/versions/strlookup.ver \
				   $(PKGINCL)/versions/strpipe.ver \
				   $(PKGINCL)/versions/strreg.ver \
				   $(PKGINCL)/versions/strsad.ver \
				   $(PKGINCL)/versions/strsched.ver \
				   $(PKGINCL)/versions/strspecfs.ver \
				   $(PKGINCL)/versions/strsysctl.ver \
				   $(PKGINCL)/versions/strutil.ver \
				   $(PKGINCL)/versions/strsyms.ver
## CONFIG_STREAMS_SEPARATE_COMPILE
else
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_VERFILES			+= $(PKGINCL)/versions/specfs.ver \
				   $(PKGINCL)/versions/streams.ver

## CONFIG_STREAMS_SEPARATE_COMPILE
endif

EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver \
				   $(PKGINCL)/versions/sth.ver \
				   $(PKGINCL)/versions/aixcompat.ver \
				   $(PKGINCL)/versions/hpuxcompat.ver \
				   $(PKGINCL)/versions/irixcompat.ver \
				   $(PKGINCL)/versions/maccompat.ver \
				   $(PKGINCL)/versions/mpscompat.ver \
				   $(PKGINCL)/versions/os7compat.ver \
				   $(PKGINCL)/versions/osfcompat.ver \
				   $(PKGINCL)/versions/suncompat.ver \
				   $(PKGINCL)/versions/svr3compat.ver \
				   $(PKGINCL)/versions/svr4compat.ver \
				   $(PKGINCL)/versions/uw7compat.ver

GENKSYMS_CPPFLAGS		= -I./src/kernel -I$(srcdir)/src/kernel \
				  -I./src/drivers -I$(srcdir)/src/drivers \
				  -I./src/modules -I$(srcdir)/src/modules \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

EXTRA_DIST			+= $(top_srcdir)/scripts/hot.lds

##
## Rules for generating versioned kernel symbols
##
include am/genksyms.am

##
## Rules for generating STREAMS configuration
##
include am/strconf.am

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

if PKG_BUILD_ARCH
if RPM_BUILD_USER
## these go with the utilities package
dist_sbin_SCRIPTS 		+= $(INET_MKDEV) $(INET_RMDEV)
endif
endif

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

install-inet-devices: $(INET_MKDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV) ; \
	fi

uninstall-inet-devices: $(INET_RMDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV) ; \
	fi

## RPM_BUILD_KERNEL
else
## ! RPM_BUILD_KERNEL

install-inet-devices:

uninstall-inet-devices:

## ! RPM_BUILD_KERNEL
endif

INSTALL_EXEC_HOOK += install-inet-devices

UNINSTALL_HOOK += uninstall-inet-devices

.PHONY: install-inet-devices uninstall-inet-devices

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src

if PKG_BUILD_ARCH
SUBDIRS += intl po
CLEANFILES += po/stamp-po
endif

SUBDIRS += .

if WITH_32BIT_LIBS
SUBDIRS += lib32
endif

SUBDIRS += tests

if DEVELOPMENT
if DOCUMENTATION
SUBDIRS += doc
endif
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/specfs.sh \
				  src/util/init/streams.sh \
				  src/util/init/@PACKAGE_LCNAME@.sh
if CONFIG_STREAMS_LOG
dist_initrd_SCRIPTS		+= src/util/init/strace.sh \
				   src/util/init/strerr.sh
endif
if CONFIG_STREAMS_LOG_MODULE
dist_initrd_SCRIPTS		+= src/util/init/strace.sh \
				   src/util/init/strerr.sh
endif

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/specfs \
				  src/util/init/streams \
				  src/util/init/@PACKAGE_LCNAME@
if CONFIG_STREAMS_LOG
dist_initrd_SCRIPTS		+= src/util/init/strace \
				   src/util/init/strerr
endif
if CONFIG_STREAMS_LOG_MODULE
dist_initrd_SCRIPTS		+= src/util/init/strace \
				   src/util/init/strerr
endif

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= src/util/sysconfig/streams.conf \
				  src/util/sysconfig/@PACKAGE_LCNAME@.conf \
				  src/util/sysconfig/sock2path
if CONFIG_STREAMS_UTILS
dist_sysconf_DATA		+= strsetup.conf \
				   strload.conf
endif

dist_config_DATA		= src/util/sysconfig/specfs \
				  src/util/sysconfig/streams \
				  src/util/sysconfig/@PACKAGE_LCNAME@
if CONFIG_STREAMS_LOG
dist_config_DATA		+= src/util/sysconfig/strace \
				   src/util/sysconfig/strerr
endif
if CONFIG_STREAMS_LOG_MODULE
dist_config_DATA		+= src/util/sysconfig/strace \
				   src/util/sysconfig/strerr
endif

## INSTALL_INITSCRIPTS
endif

dist_snmpmib_DATA		= 

if WITH_SNMP_AGENT
## WITH_SNMP_AGENT
dist_snmpmib_DATA		+= src/mibs/ALARM-MIB.mib \
				   src/mibs/ARC-MIB.mib \
				   src/mibs/ATM-APS-MIB.mib \
				   src/mibs/ATMF-CES.mib \
				   src/mibs/ATMF-CES-MIB.mib \
				   src/mibs/ATM-FORUM-ADDR-REG.mib \
				   src/mibs/ATM-FORUM-AUTO-CONFIG.mib \
				   src/mibs/ATM-FORUM-MIB.mib \
				   src/mibs/ATM-FORUM-SNMP-M4-MIB.mib \
				   src/mibs/ATM-FORUM-SRVC-REG.mib \
				   src/mibs/ATM-FORUM-TC-MIB.mib \
				   src/mibs/ATM-SOFT-PVC-MIB.mib \
				   src/mibs/ATM-SOFT-PVC-MIB.old.mib \
				   src/mibs/ATM-TRACE-MIB.mib \
				   src/mibs/CLNS-MIB.mib \
				   src/mibs/HC-PerfHist-TC-MIB.mib \
				   src/mibs/IMA-MIB.mib \
				   src/mibs/LAN-EMULATION-CLIENT-MIB.mib \
				   src/mibs/MPOA-MIB.mib \
				   src/mibs/OPENSS7-ACSE-MIB.mib \
				   src/mibs/OPENSS7-CDR2-MIB.mib \
				   src/mibs/OPENSS7-CDR-MIB.mib \
				   src/mibs/OPENSS7-CLNS-MIB.mib \
				   src/mibs/OPENSS7-CONS-MIB.mib \
				   src/mibs/OPENSS7-DL-MIB.mib \
				   src/mibs/OPENSS7-GMI-MIB.mib \
				   src/mibs/OPENSS7-ISO10733-MIB.mib \
				   src/mibs/OPENSS7-ISUP-MIB.mib \
				   src/mibs/OPENSS7-ISUP-OM-MIB.mib \
				   src/mibs/OPENSS7-LAPB-MIB.mib \
				   src/mibs/OPENSS7-LLC2-MIB.mib \
				   src/mibs/OPENSS7-M2PA-MIB.mib \
				   src/mibs/OPENSS7-M2UA-ASP-MIB.mib \
				   src/mibs/OPENSS7-M2UA-MIB.mib \
				   src/mibs/OPENSS7-M2UA-SL-MIB.mib \
				   src/mibs/OPENSS7-M3UA-ASP-MIB.mib \
				   src/mibs/OPENSS7-M3UA-SGP-MIB.mib \
				   src/mibs/OPENSS7-M3UA-TC.mib \
				   src/mibs/OPENSS7-MAP-MIB.mib \
				   src/mibs/OPENSS7-METRIC-MIB.mib \
				   src/mibs/OPENSS7-MIB.mib \
				   src/mibs/OPENSS7-MTP-MIB.mib \
				   src/mibs/OPENSS7-MTP-OM-MIB2.mib \
				   src/mibs/OPENSS7-MTP-OM-MIB.mib \
				   src/mibs/OPENSS7-MTP-TC.mib \
				   src/mibs/OPENSS7-MX-MIB.mib \
				   src/mibs/OPENSS7-PHY-MIB.mib \
				   src/mibs/OPENSS7-Q821-MIB.mib \
				   src/mibs/OPENSS7-SCCP2-MIB.mib \
				   src/mibs/OPENSS7-SCCP-MIB.mib \
				   src/mibs/OPENSS7-SCCP-OM-MIB.mib \
				   src/mibs/OPENSS7-SIGTRAN-MIB.mib \
				   src/mibs/OPENSS7-SIGTRAN-TC.mib \
				   src/mibs/OPENSS7-SL-MIB.mib \
				   src/mibs/OPENSS7-SL-MUX-MIB.mib \
				   src/mibs/OPENSS7-SMI.mib \
				   src/mibs/OPENSS7-SMI-MIB.mib \
				   src/mibs/OPENSS7-SNARE-MIB.mib \
				   src/mibs/OPENSS7-SS7-MIB.mib \
				   src/mibs/OPENSS7-SS7-OM-MIB.mib \
				   src/mibs/OPENSS7-SS7-TC.mib \
				   src/mibs/OPENSS7-STREAMS-EXT-MIB.mib \
				   src/mibs/OPENSS7-STREAMS-MIB.mib \
				   src/mibs/OPENSS7-TCAP2-MIB.mib \
				   src/mibs/OPENSS7-TCAP-MIB.mib \
				   src/mibs/OPENSS7-TCAP-OM-MIB.mib \
				   src/mibs/OPENSS7-TC.mib \
				   src/mibs/OPENSS7-TMN-SMI.mib \
				   src/mibs/OPENSS7-TMN-TC.mib \
				   src/mibs/OPENSS7-UMC-MIB.mib \
				   src/mibs/OPENSS7-X25-LLC-MIB.mib \
				   src/mibs/OPENSS7-X25-PLP-MIB.mib \
				   src/mibs/OPENSS7-X400P-MIB.mib \
				   src/mibs/OPENSS7-X400P-MX-MIB.mib \
				   src/mibs/PerfHist-TC-MIB.mib \
				   src/mibs/PNNI-MIB.mib \
				   src/mibs/PNNI-MOB-EXT-MIB.mib \
				   src/mibs/RFC1381-MIB.mib \
				   src/mibs/RFC1382-MIB.mib \
				   src/mibs/RFC1513-MIB.mib \
				   src/mibs/RFC2127-MIB.txt \
				   src/mibs/RFC2128-MIB.txt \
				   src/mibs/RFC2494-EXT-MIB.mib \
				   src/mibs/RFC2494-MIB.txt \
				   src/mibs/RFC2495-MIB.txt \
				   src/mibs/RFC2496-MIB.txt \
				   src/mibs/RFC2514-MIB.mib \
				   src/mibs/RFC2515-MIB.mib \
				   src/mibs/RFC3593-MIB.txt \
				   src/mibs/RFC3873-MIB.txt \
				   src/mibs/RFC3877-MIB.txt \
				   src/mibs/RFC3878-MIB.txt \
				   src/mibs/RFC3895-MIB.txt \
				   src/mibs/RFC3896-MIB.txt \
				   src/mibs/RFC4133-MIB.mib \
				   src/mibs/RFC4502-MIB.mib \
				   src/mibs/RFC4805-EXT-MIB.mib \
				   src/mibs/RFC4805-MIB.txt \
				   src/mibs/RS-232-MIB.mib

## WITH_SNMP_AGENT
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
## Rules for net config files
##
include am/netconfig.am
include am/sock2path.am

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

##
## Rules for SNMP modules.
##
SNMPORDER			= 01
include am/snmp.am

EXTRA_DIST			+= debian/README.Debian \
				   debian/TODO.Debian

MAINTAINERCLEANFILES		+= $(top_srcdir)/aclocal.m4 \
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

# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
