## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) File: src/include/Makefile.am  Version: @VERSION@  Date: @PACKAGE_DATE@
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
## Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU Affero General Public License as published by the Free
## Software Foundation; version 3 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
## details.
##
## You should have received a copy of the GNU Affero General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>, or
## write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
## 02139, USA.
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
## =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

nobase_pkginclude_HEADERS = \
	ap_lib.h \
	ap_poll.h \
	cdiapi.h \
	dlpiapi.h \
	libdlpi.h \
	log.h \
	loop.h \
	netconfig.h \
	netdir.h \
	npiapi.h \
	npi_x25.h \
	omp_dmi.h \
	osi.h \
	osi_lib.h \
	ro_lib.h \
	sad.h \
	sgtty.h \
	sockdb.h \
	sockio.h \
	sockmod.h \
	socksys.h \
	strlog.h \
	stropts.h \
	sx25.h \
	termio.h \
	termios.h \
	ticlts.h \
	ticots.h \
	ticotsord.h \
	tihdr.h \
	timod.h \
	tirdwr.h \
	tiuser.h \
	ttcompat.h \
	ttychars.h \
	xap_env.h \
	xap.h \
	xap_rose.h \
	xdsbdcp.h \
	xds.h \
	xdsmdup.h \
	xdssap.h \
	xmap_ansi.h \
	xmap_gsm_at.h \
	xmap_gsm_ch.h \
	xmap_gsm_err.h \
	xmap_gsm_gc.h \
	xmap_gsm.h \
	xmap_gsm_ho.h \
	xmap_gsm_in.h \
	xmap_gsm_ls.h \
	xmap_gsm_mm.h \
	xmap_gsm_ms.h \
	xmap_gsm_om.h \
	xmap_gsm_sb.h \
	xmap_gsm_sm.h \
	xmap_gsm_ss.h \
	xmap_gsm_st.h \
	xmap_gsm_xx.h \
	xmap.h \
	xmap_sms.h \
	xmp_cmis.h \
	xmp.h \
	xmp_snmp.h \
	xmsga.h \
	xms.h \
	xmsima.h \
	xnetdb.h \
	xom.h \
	xomi.h \
	xti_apli.h \
	xti.h \
	xti_inet.h \
	xti_mosi.h \
	xti_osi.h \
	xti_xx25.h \
	net/bpf.h \
	net/lif.h \
	net/nit_if.h \
	net/nit_buf.h \
	net/nit_pf.h \
	netx25/ll_control.h \
	netx25/nli.h \
	netx25/x25_control.h \
	netx25/x25_proto.h \
	netx25/xnetdb.h \
	snet/dl_control.h \
	snet/dl_proto.h \
	snet/ixe_control.h \
	snet/ixe_proto.h \
	snet/ll_control.h \
	snet/ll_proto.h \
	snet/mlp_control.h \
	snet/mlp_proto.h \
	snet/wan_control.h \
	snet/wan_proto.h \
	snet/x25_control.h \
	snet/x25_proto.h \
	snet/x32_control.h \
	snet/xxx_control.h \
	ss7/capi.h \
	ss7/capi_ioctl.h \
	ss7/cci.h \
	ss7/cdi.h \
	ss7/chi.h \
	ss7/chi_ioctl.h \
	ss7/devi.h \
	ss7/devi_ioctl.h \
	ss7/dl.h \
	ss7/dl_ioctl.h \
	ss7/dlpi_ioctl.h \
	ss7/dua_ioctl.h \
	ss7/gr303_ioctl.h \
	ss7/gr303ua_ioctl.h \
	ss7/gsma_ioctl.h \
	ss7/h225_ioctl.h \
	ss7/hdlc_ioctl.h \
	ss7/isdni.h \
	ss7/isdni_ioctl.h \
	ss7/isupi.h \
	ss7/isupi_ioctl.h \
	ss7/iua_ioctl.h \
	ss7/lapb_ioctl.h \
	ss7/lapd_ioctl.h \
	ss7/lapf_ioctl.h \
	ss7/lki.h \
	ss7/lki_ioctl.h \
	ss7/lmi.h \
	ss7/lmi_ioctl.h \
	ss7/m2pa_ioctl.h \
	ss7/m2ua_ioctl.h \
	ss7/m3ua.h \
	ss7/m3ua_ioctl.h \
	ss7/map.h \
	ss7/map_ioctl.h \
	ss7/mgcp.h \
	ss7/mgi.h \
	ss7/mgi_ioctl.h \
	ss7/mtp2_ioctl.h \
	ss7/mtp3b.h \
	ss7/mtp.h \
	ss7/mtpi.h \
	ss7/mtpi_ioctl.h \
	ss7/mtp_ioctl.h \
	ss7/mxi.h \
	ss7/mxi_ioctl.h \
	ss7/ph.h \
	ss7/q920.h \
	ss7/sccp.h \
	ss7/sccpi.h \
	ss7/sccpi_ioctl.h \
	ss7/sccp_ioctl.h \
	ss7/sctp.h \
	ss7/sctpi.h \
	ss7/sctpi_ioctl.h \
	ss7/sdli.h \
	ss7/sdli_ioctl.h \
	ss7/sdti.h \
	ss7/sdti_ioctl.h \
	ss7/sl.h \
	ss7/sli.h \
	ss7/sli_ioctl.h \
	ss7/sl_ioctl.h \
	ss7/slmi.h \
	ss7/sl_mux.h \
	ss7/slsi.h \
	ss7/slsi_ioctl.h \
	ss7/smi.h \
	ss7/smi_ioctl.h \
	ss7/tcap.h \
	ss7/tcapi.h \
	ss7/tcap_ioctl.h \
	ss7/tc.h \
	ss7/tr.h \
	ss7/ua_lm.h \
	ss7/ua_lm_ioctl.h \
	ss7/v52_ioctl.h \
	ss7/v5ua_ioctl.h \
	ss7/x400p_ioctl.h \
	ss7/zapata.h \
	sys/bufmod.h \
	sys/pfmod.h \
	sys/apli.h \
	sys/atm_dlpi.h \
	sys/capi.h \
	sys/capi_ioctl.h \
	sys/cci.h \
	sys/cdi_daed.h \
	sys/cdi.h \
	sys/cdi_hdlc.h \
	sys/cdli.h \
	sys/ch.h \
	sys/chi.h \
	sys/chi_ioctl.h \
	sys/clns.h \
	sys/cmn_err.h \
	sys/cons.h \
	sys/ddi.h \
	sys/debug.h \
	sys/dki.h \
	sys/dl.h \
	sys/dlpi_aix.h \
	sys/dlpi_dua.h \
	sys/dlpi_ext.h \
	sys/dlpi_gr303.h \
	sys/dlpi_gr303ua.h \
	sys/dlpi_gsma.h \
	sys/dlpi.h \
	sys/dlpi_h225.h \
	sys/dlpi_ioctl.h \
	sys/dlpi_isdn.h \
	sys/dlpi_iua.h \
	sys/dlpi_lapb.h \
	sys/dlpi_lapd.h \
	sys/dlpi_lapf.h \
	sys/dlpi_m2ua.h \
	sys/dlpi_mtp2.h \
	sys/dlpi_sgi.h \
	sys/dlpi_sun.h \
	sys/dlpi_v52.h \
	sys/dlpi_v5ua.h \
	sys/dl_x25.h \
	sys/dsx_ioctl.h \
	sys/dua_ioctl.h \
	sys/gr303_ioctl.h \
	sys/gr303ua_ioctl.h \
	sys/h225.h \
	sys/h225_ioctl.h \
	sys/isdni.h \
	sys/isdni_ioctl.h \
	sys/iua_ioctl.h \
	sys/kmem.h \
	sys/lapb_ioctl.h \
	sys/lapd_ioctl.h \
	sys/lapf_ioctl.h \
	sys/ldl.h \
	sys/llc.h \
	sys/log.h \
	sys/loop.h \
	sys/m2pa_ioctl.h \
	sys/m2ua_as.h \
	sys/m2ua_as_ioctl.h \
	sys/m2ua_ioctl.h \
	sys/m2ua_lm.h \
	sys/m3ua_as.h \
	sys/m3ua_as_ioctl.h \
	sys/m3ua.h \
	sys/m3ua_ioctl.h \
	sys/map.h \
	sys/mgcp.h \
	sys/mgi.h \
	sys/mgi_ioctl.h \
	sys/mxi.h \
	sys/mxi_ioctl2.h \
	sys/mxi_ioctl.h \
	sys/netconfig.h \
	sys/np.h \
	sys/npiext.h \
	sys/npi.h \
	sys/npi_ip.h \
	sys/npi_mtp.h \
	sys/npi_sccp.h \
	sys/npi_sctp.h \
	sys/npi_ss7.h \
	sys/npi_sscop.h \
	sys/npi_tcp.h \
	sys/npi_udp.h \
	sys/npi_x25.h \
	sys/osi.h \
	sys/ph.h \
	sys/plp_ioctl.h \
	sys/pty.h \
	sys/q920.h \
	sys/sad.h \
	sys/sc.h \
	sys/scodlpi.h \
	sys/sgtty.h \
	sys/smi_ioctl.h \
	sys/sockio.h \
	sys/socklib.h \
	sys/sockmod.h \
	sys/sockpath.h \
	sys/socksys.h \
	sys/spec_fs_i.h \
	sys/strconf.h \
	sys/strdebug.h \
	sys/stream.h \
	sys/strlog.h \
	sys/stropts32.h \
	sys/stropts.h \
	sys/strsubr.h \
	sys/strsun.h \
	sys/strtty.h \
	sys/sunddi.h \
	sys/termio.h \
	sys/termios.h \
	sys/testmod.h \
	sys/ticlts.h \
	sys/ticots.h \
	sys/ticotsord.h \
	sys/tihdr.h \
	sys/timod.h \
	sys/tirdwr.h \
	sys/tiuser.h \
	sys/tli.h \
	sys/tp.h \
	sys/tpi.h \
	sys/tpi_inet.h \
	sys/tpi_ip.h \
	sys/tpi_mtp.h \
	sys/tpi_sccp.h \
	sys/tpi_sctp.h \
	sys/tpi_sl.h \
	sys/tpi_ss7.h \
	sys/tpi_sscop.h \
	sys/tpi_tcap.h \
	sys/tpi_tcp.h \
	sys/tpi_udp.h \
	sys/ttychars.h \
	sys/ttydev.h \
	sys/ttyio.h \
	sys/ua_as_ioctl.h \
	sys/ua_as_lm.h \
	sys/ua_ioctl.h \
	sys/ua_lm.h \
	sys/ua_lm_ioctl.h \
	sys/ua_sg_ioctl.h \
	sys/v52_ioctl.h \
	sys/v5ua_ioctl.h \
	sys/xti_apli.h \
	sys/xti_atm.h \
	sys/xti.h \
	sys/xti_inet.h \
	sys/xti_ip.h \
	sys/xti_local.h \
	sys/xti_mosi.h \
	sys/xti_mtp.h \
	sys/xti_osi.h \
	sys/xti_sccp.h \
	sys/xti_sctp.h \
	sys/xti_sl.h \
	sys/xti_ss7.h \
	sys/xti_sscop.h \
	sys/xti_tcap.h \
	sys/xti_tcp.h \
	sys/xti_udp.h \
	sys/xti_xti.h \
	sys/xti_xx25.h \
	sys/zapata.h \
	sys/aix/ddi.h \
	sys/aix/strconf.h \
	sys/aix/stream.h \
	sys/hpux/ddi.h \
	sys/hpux/strconf.h \
	sys/hpux/stream.h \
	sys/irix/ddi.h \
	sys/irix/strconf.h \
	sys/irix/stream.h \
	sys/mac/ddi.h \
	sys/mac/strconf.h \
	sys/mac/stream.h \
	sys/mps/ddi.h \
	sys/mps/strconf.h \
	sys/mps/stream.h \
	sys/os7/allocb.h \
	sys/os7/bufpool.h \
	sys/os7/bufq.h \
	sys/os7/compat.h \
	sys/os7/ddi.h \
	sys/os7/debug.h \
	sys/os7/lock.h \
	sys/os7/priv.h \
	sys/os7/queue.h \
	sys/os7/strconf.h \
	sys/os7/stream.h \
	sys/os7/timer.h \
	sys/osf/ddi.h \
	sys/osf/strconf.h \
	sys/osf/stream.h \
	sys/sun/ddi.h \
	sys/sun/strconf.h \
	sys/sun/stream.h \
	sys/sun/strsun.h \
	sys/svr3/ddi.h \
	sys/svr3/strconf.h \
	sys/svr3/stream.h \
	sys/svr4/ddi.h \
	sys/svr4/strconf.h \
	sys/svr4/stream.h \
	sys/uw7/ddi.h \
	sys/uw7/strconf.h \
	sys/uw7/stream.h \
	sys/snet/dl_control.h \
	sys/snet/dl_proto.h \
	sys/snet/ixe_control.h \
	sys/snet/ixe_proto.h \
	sys/snet/ll_control.h \
	sys/snet/ll_proto.h \
	sys/snet/mlp_control.h \
	sys/snet/mlp_proto.h \
	sys/snet/wan_control.h \
	sys/snet/wan_proto.h \
	sys/snet/x25_control.h \
	sys/snet/x25_proto.h \
	sys/snet/x32_control.h \
	sys/snet/xxx_control.h \
	sys/openss7/cmn_err.h \
	sys/openss7/ddi.h \
	sys/openss7/debug.h \
	sys/openss7/dki.h \
	sys/openss7/kmem.h \
	sys/openss7/log.h \
	sys/openss7/loop.h \
	sys/openss7/sad.h \
	sys/openss7/sc.h \
	sys/openss7/strconf.h \
	sys/openss7/strdebug.h \
	sys/openss7/stream.h \
	sys/openss7/strlog.h \
	sys/openss7/stropts32.h \
	sys/openss7/stropts.h \
	sys/openss7/strsubr.h \
	sys/openss7/testmod.h

dist_noinst_HEADERS = \
	ucd-snmp-4.2.5/agent_index.h \
	ucd-snmp-4.2.5/agent_read_config.h \
	ucd-snmp-4.2.5/agent_registry.h \
	ucd-snmp-4.2.5/agent_trap.h \
	ucd-snmp-4.2.5/asn1.h \
	ucd-snmp-4.2.5/auto_nlist.h \
	ucd-snmp-4.2.5/callback.h \
	ucd-snmp-4.2.5/default_store.h \
	ucd-snmp-4.2.5/ds_agent.h \
	ucd-snmp-4.2.5/header_complex.h \
	ucd-snmp-4.2.5/int64.h \
	ucd-snmp-4.2.5/keytools.h \
	ucd-snmp-4.2.5/mib.h \
	ucd-snmp-4.2.5/mibincl.h \
	ucd-snmp-4.2.5/mib_module_config.h \
	ucd-snmp-4.2.5/parse.h \
	ucd-snmp-4.2.5/read_config.h \
	ucd-snmp-4.2.5/scapi.h \
	ucd-snmp-4.2.5/snmp_agent.h \
	ucd-snmp-4.2.5/snmp_alarm.h \
	ucd-snmp-4.2.5/snmp_api.h \
	ucd-snmp-4.2.5/snmp_client.h \
	ucd-snmp-4.2.5/snmp_debug.h \
	ucd-snmp-4.2.5/snmp.h \
	ucd-snmp-4.2.5/snmp_impl.h \
	ucd-snmp-4.2.5/snmp_logging.h \
	ucd-snmp-4.2.5/snmp_parse_args.h \
	ucd-snmp-4.2.5/snmp-tc.h \
	ucd-snmp-4.2.5/snmpusm.h \
	ucd-snmp-4.2.5/snmpv3.h \
	ucd-snmp-4.2.5/snmp_vars.h \
	ucd-snmp-4.2.5/struct.h \
	ucd-snmp-4.2.5/system.h \
	ucd-snmp-4.2.5/tools.h \
	ucd-snmp-4.2.5/transform_oids.h \
	ucd-snmp-4.2.5/util_funcs.h \
	ucd-snmp-4.2.5/var_struct.h \
	ucd-snmp-4.2.5/version.h

nobase_nodist_pkginclude_HEADERS = \
	sys/$(PACKAGE)/config.h \
	sys/$(PACKAGE)/version.h
#	sys/$(PACKAGE)/wrapper.h
#	sys/$(PACKAGE)/modversions.h

nobase_nodist_kinclude_HEADERS = \
	sys/$(PACKAGE)/config.h \
	sys/$(PACKAGE)/version.h
#	sys/$(PACKAGE)/wrapper.h
#	sys/$(PACKAGE)/modversions.h

if PERFORM_CHECKS
## PERFORM_CHECKS
#dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_headers
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

## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
