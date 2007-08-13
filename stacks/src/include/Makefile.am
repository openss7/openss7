## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/08/13 15:22:13 $
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
# Last Modified $Date: 2007/08/13 15:22:13 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

cdi_headers = \
	ss7/cdi.h \
	ss7/hdlc_ioctl.h \
	sys/cdi_daed.h \
	sys/cdi.h \
	sys/cdi_hdlc.h

dlpi_headers = \
	ss7/dl.h \
	ss7/dl_ioctl.h \
	ss7/dlpi_ioctl.h
	ss7/lapb_ioctl.h \
	ss7/lapf_ioctl.h \
	sys/dlpi.h

npi_headers = \
	sys/npi.h

tli_headers	= \
	sys/tihdr.h \
	sys/timod.h \
	sys/tirdwr.h \
	sys/tiuser.h \
	sys/tli.h \
	sys/tpi.h \
	tihdr.h \
	timod.h \
	tirdwr.h \
	tiuser.h

unix_headers	= \
	sys/ticlts.h \
	sys/ticots.h \
	sys/ticotsord.h \
	ticlts.h \
	ticots.h \
	ticotsord.h

xnet_headers	= \
	sys/xti.h \
	sys/xti_local.h \
	sys/xti_xti.h \
	xti.h

inet_headers	= \
	sys/npi_ip.h \
	sys/npi_tcp.h \
	sys/npi_udp.h \
	sys/tpi_inet.h \
	sys/tpi_ip.h \
	sys/tpi_tcp.h \
	sys/tpi_udp.h \
	sys/xti_inet.h \
	sys/xti_ip.h \
	sys/xti_tcp.h \
	sys/xti_udp.h \
	xti_inet.h

osi_headers	= \
	sys/dlpi_lapb.h \
	sys/dlpi_lapf.h \
	sys/xti_mosi.h \
	sys/xti_osi.h

atm_headers	= \
	sys/npi_sscop.h \
	sys/tpi_sscop.h \
	sys/xti_atm.h \
	sys/xti_sscop.h

#xnsl_headers	= \
#	netdir.h \
#	netconfig.h \
#	sys/netconfig.h

sock_headers	= \
	sys/sockmod.h

sctp_headers	= \
	ss7/sctp.h \
	ss7/sctpi.h \
	ss7/sctpi_ioctl.h \
	sys/npi_sctp.h \
	sys/tpi_sctp.h \
	sys/xti_sctp.h

isdn_headers = \
	ss7/capi.h \
	ss7/capi_ioctl.h \
	ss7/isdni.h \
	ss7/isdni_ioctl.h \
	ss7/lapd_ioctl.h \
	ss7/v52_ioctl.h \
	sys/dlpi_gr303.h \
	sys/dlpi_isdn.h \
	sys/dlpi_lapd.h \
	sys/dlpi_v52.h

sigtran_headers = \
	ss7/dua_ioctl.h \
	ss7/gr303_ioctl.h \
	ss7/gr303ua_ioctl.h \
	ss7/iua_ioctl.h \
	ss7/m2pa_ioctl.h \
	ss7/m2ua_ioctl.h \
	ss7/m3ua.h \
	ss7/m3ua_ioctl.h \
	ss7/ua_lm.h \
	ss7/ua_lm_ioctl.h \
	ss7/v5ua_ioctl.h \
	sys/dlpi_dua.h \
	sys/dlpi_gr303ua.h \
	sys/dlpi_iua.h \
	sys/dlpi_m2ua.h \
	sys/dlpi_v5ua.h

chan_headers = \
	ss7/chi.h \
	ss7/chi_ioctl.h \
	ss7/mgcp.h \
	ss7/mgi.h \
	ss7/mgi_ioctl.h \
	ss7/mxi.h \
	ss7/mxi_ioctl.h \
	ss7/ph.h \
	ss7/q920.h

voip_headers = \
	ss7/h225_ioctl.h \
	sys/dlpi_h225.h

gsm_headers = \
	ss7/gsma_ioctl.h \
	sys/dlpi_gsma.h


ss7_headers = \
	ss7/cci.h \
	ss7/devi.h \
	ss7/devi_ioctl.h \
	ss7/isupi.h \
	ss7/isupi_ioctl.h \
	ss7/lki.h \
	ss7/lki_ioctl.h \
	ss7/lmi.h \
	ss7/lmi_ioctl.h \
	ss7/mtp2_ioctl.h \
	ss7/mtp3b.h \
	ss7/mtp.h \
	ss7/mtpi.h \
	ss7/mtpi_ioctl.h \
	ss7/mtp_ioctl.h \
	ss7/sccp.h \
	ss7/sccpi.h \
	ss7/sccpi_ioctl.h \
	ss7/sccp_ioctl.h \
	ss7/sdli.h \
	ss7/sdli_ioctl.h \
	ss7/sdti.h \
	ss7/sdti_ioctl.h \
	ss7/sl.h \
	ss7/sli.h \
	ss7/sli_ioctl.h \
	ss7/slmi.h \
	ss7/slsi.h \
	ss7/slsi_ioctl.h \
	ss7/smi.h \
	ss7/smi_ioctl.h \
	ss7/tcap.h \
	ss7/tcapi.h \
	ss7/tcap_ioctl.h \
	ss7/tc.h \
	ss7/tr.h \
	sys/dlpi_mtp2.h \
	sys/npi_mtp.h \
	sys/npi_sccp.h \
	sys/npi_ss7.h \
	sys/tpi_mtp.h \
	sys/tpi_sccp.h \
	sys/tpi_sl.h \
	sys/tpi_ss7.h \
	sys/tpi_tcap.h \
	sys/xti_mtp.h \
	sys/xti_sccp.h \
	sys/xti_sl.h \
	sys/xti_ss7.h \
	sys/xti_tcap.h

nobase_pkginclude_HEADERS = \
	$(ss7_headers)

if WITH_XNS
## WITH_XNS
nobase_pkginclude_HEADERS += \
	$(cdi_headers) \
	$(dlpi_headers) \
	$(npi_headers)
## WITH_XNS
endif

if WITH_TLI
## WITH_TLI
nobase_pkginclude_HEADERS += \
	$(tli_headers) \
	$(unix_headers)
## WITH_TLI
endif

if WITH_XNET
## WITH_XNET
nobase_pkginclude_HEADERS += \
	$(xnet_headers) \
	$(inet_headers) \
	$(osi_headers) \
	$(atm_headers)
## WITH_XNET
endif

#if WITH_XNSL
### WITH_XNSL
#nobase_pkginclude_HEADERS += \
#	$(xnsl_headers)
### WITH_XNSL
#endif

if WITH_SOCK
## WITH_SOCK
nobase_pkginclude_HEADERS += \
	$(sock_headers)
## WITH_SOCK
endif

if WITH_SCTP
## WITH_SCTP
nobase_pkginclude_HEADERS += \
	$(sctp_headers)
## WITH_SCTP
endif

if WITH_ISDN
## WITH_ISDN
nobase_pkginclude_HEADERS += \
	$(isdn_headers)
## WITH_ISDN
endif

if WITH_SIGTRAN
## WITH_SIGTRAN
nobase_pkginclude_HEADERS += \
	$(sigtran_headers)
## WITH_SIGTRAN
endif

if WITH_CHAN
## WITH_CHAN
nobase_pkginclude_HEADERS += \
	$(chan_headers)
## WITH_CHAN
endif

if WITH_VOIP
## WITH_VOIP
nobase_pkginclude_HEADERS += \
	$(voip_headers)
## WITH_VOIP
endif

if WITH_GSM
## WITH_GSM
nobase_pkginclude_HEADERS += \
	$(gsm_headers)
## WITH_GSM
endif

dist_noinst_HEADERS = \
	$(cdi_headers) \
	$(dlpi_headers) \
	$(npi_headers) \
	$(tli_headers) \
	$(unix_headers) \
	$(xnet_headers) \
	$(inet_headers) \
	$(osi_headers) \
	$(atm_headers) \
	$(sock_headers) \
	$(sctp_headers) \
	$(isdn_headers) \
	$(sigtran_headers) \
	$(chan_headers) \
	$(voip_headers) \
	$(gsm_headers)

#	os7/allocb.h
#	os7/bufpool.h
#	os7/bufq.h
#	os7/debug.h
#	os7/lock.h
#	os7/priv.h
#	os7/queue.h
#	os7/timer.h

nobase_nodist_pkginclude_HEADERS = \
	sys/$(PACKAGE)/config.h \
	sys/$(PACKAGE)/version.h
#	sys/$(PACKAGE)/modversions.h

if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_headers
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
##
## $Log: Makefile.am,v $
## Revision 0.9.2.14  2007/08/13 15:22:13  brian
## - header updates, added references
##
## Revision 0.9.2.13  2006/12/18 10:51:00  brian
## - subpackaging changes for release
##
## Revision 0.9.2.12  2006/10/03 13:51:36  brian
## - changes to pass make check target
## - added some package config.h files
## - removed AUTOCONFIG_H from Makefile.am's
## - source code changes for compile
## - added missing manual pages
## - renamed conflicting manual pages
## - parameterized include Makefile.am
## - updated release notes
##
## Revision 0.9.2.11  2006/03/06 11:38:24  brian
## - updated headers
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
