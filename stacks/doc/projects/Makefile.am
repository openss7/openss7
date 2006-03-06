# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/03/06 11:38:14 $
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
# Last Modified $Date: 2006/03/06 11:38:14 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.8  2006/03/06 11:38:14  brian
# - updated headers
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		= hlr.texi voip.texi

EXTRA_info_TEXINFOS	= \
			texi/args.texi \
			$(top_srcdir)/scripts/texi/fdl.texi

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			c.fig \
			d.fig \
			gb.fig \
			gc2.fig \
			gc.fig \
			gd.fig \
			ge.fig \
			gf.fig \
			gn.fig \
			gp.fig \
			gr.fig \
			gs.fig \
			hlrtest.fig \
			iface.fig \
			iu.fig \
			netconf.fig \
			voip_netconf.fig \
			osimap.fig \
			rack.fig \
			sgsn.fig \
			sgsntest.fig \
			sigbearer.fig \
			tiphon.fig \
			xflow1.fig \
			xflow2.fig \
			xflow3.fig \
			xflow4.fig \
			xflow5.fig \
			xflow6.fig \
			xflow.fig \
			voip_ifa.fig \
			voip_ifa_h323.fig \
			voip_ifa_sipt.fig \
			voip_ifc.fig \
			voip_ifc_h323.fig \
			voip_ifc_sipt.fig \
			voip_ifd.fig \
			voip_ifd_h323.fig \
			voip_ifd_sipt.fig \
			voip_ifeb.fig \
			voip_ifeb_ss7.fig \
			voip_ifeb_atm.fig \
			voip_ifj.fig \
			voip_ifj_ss7.fig \
			voip_ifj_isdn.fig \
			voip_ifj_gr303v5.fig

TBL_TBLS		= \
			hlr_schest.tbl \
			hlr_sched.tbl \
			hlr_gate0.tbl \
			hlr_gate1.tbl \
			hlr_gate2.tbl \
			hlr_gate3.tbl \
			hlr_gate4.tbl \
			hlr_gate5.tbl \
			hlr_gate6.tbl \
			hlr_traf.tbl \
			hlr_cost.tbl

PNG_PNGS		= \
			acb56.png \
			cpc388.png \
			cpc396.png \
			pca200e.png \
			t400p-ss7.png \
			v400p.png \
			a101c.png \
			bri.png

PS_PSS			= \
			mtp2.ps \
			mtp3.ps \
			platconf.ps \
			protconf.ps \
			sccp.ps \
			sdt.ps \
			sl.ps \
			stack.ps \
			tcap.ps

## PKG_BUILD_INDEP
endif

include ../../am/info.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
