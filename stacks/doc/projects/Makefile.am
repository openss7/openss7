## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2007/08/13 15:21:59 $
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
# Last Modified $Date: 2007/08/13 15:21:59 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= lab.texi hlr.texi voip.texi cnam.texi

lab_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

hlr_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

voip_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

cnam_TEXINFOS		= \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi

if MAINTAINER_MODE
lab_TEXINFOS		+= \
			  texi/args.texi
hlr_TEXINFOS		+= \
			  texi/args.texi
voip_TEXINFOS		+= \
			  texi/args.texi
cnam_TEXINFOS		+= \
			  texi/args.texi
endif

EXTRA_info_TEXINFOS	= \
			  texi/args.texi \
			  $(info_TEXINFOS) \
			  $(lab_TEXINFOS) \
			  $(hlr_TEXINFOS) \
			  $(voip_TEXINFOS) \
			  $(cnam_TEXINFOS)


if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			cnam_iface.fig \
			cnam_lab.fig \
			cnam_m2pa.fig \
			cnam_m2ua.fig \
			cnam_m3ua.fig \
			cnam_netarch.fig \
			cnam_soap.fig \
			cnam_sua.fig \
			cnam_tua.fig \
			cnam_xflow1.fig \
			cnam_xflow2.fig \
			cnam_xflow3.fig \
			cnam_xflow4.fig \
			cnam_a.fig \
			cnam_b.fig \
			cnam_c.fig \
			cnam_d.fig \
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
			voip_ifj_gr303v5.fig \
			lab000.fig \
			lab001.fig \
			lab002.fig \
			lab003.fig \
			lab004.fig \
			lab005.fig \
			lab006.fig \
			lab007.fig \
			floorplan.fig \
			elev.fig \
			labnode1.fig \
			labnode2.fig \
			labnode3.fig \
			labnode4.fig \
			labnode5.fig \
			labnode6.fig

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
			hlr_cost.tbl \
			labnodes.tbl \
			labpref.tbl \
			labmini.tbl

PNG_PNGS		= \
			acb56.png \
			cpc388.png \
			cpc396.png \
			pca200e.png \
			t400p-ss7.png \
			v400p.png \
			a101c.png \
			bri.png \
			rack2u.png \
			x346.png \
			pe2850.png \
			dl380g4.png

PS_PSS			= \
			cnam_protconf.ps \
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

## DEVELOPMENT
endif

include ../../am/info.am

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.22  2007/08/13 15:21:59  brian
## - header updates, added references
##
## Revision 0.9.2.21  2007/08/03 13:34:52  brian
## - manual updates, put ss7 modules in public release
##
## Revision 0.9.2.20  2007/07/04 08:32:16  brian
## - doc updates and m2pa correction
##
## Revision 0.9.2.19  2007/06/26 18:15:55  brian
## - added new documentation
##
## Revision 0.9.2.18  2007/03/14 04:27:27  brian
## - typo
##
## Revision 0.9.2.17  2007/03/14 04:22:59  brian
## - do not depend on args.texi or discheck target fails
##
## Revision 0.9.2.16  2007/03/06 21:26:12  brian
## - final updates and corrections to build process for release
##
## Revision 0.9.2.15  2006/08/26 09:16:55  brian
## - better release file generation
##
## Revision 0.9.2.14  2006/08/22 12:44:10  brian
## - documentation updates
##
## Revision 0.9.2.13  2006/05/01 14:19:22  brian
## - updating LAB document
##
## Revision 0.9.2.12  2006/04/30 15:26:27  brian
## - updated LAB document
##
## Revision 0.9.2.11  2006/04/29 21:30:19  brian
## - added UNT lab project documentation
##
## Revision 0.9.2.10  2006/03/20 13:50:27  brian
## - get automake to run clean
##
## Revision 0.9.2.9  2006/03/20 13:36:26  brian
## - added checks for Development
##
## Revision 0.9.2.8  2006/03/06 11:38:14  brian
## - updated headers
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
