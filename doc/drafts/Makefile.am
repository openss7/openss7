## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) File: doc/drafts/Makefile.am  Version: @VERSION@  Date: @PACKAGE_DATE@
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
# =============================================================================

include ../../am/openss7.am

if DEVELOPMENT
## DEVELOPMENT

ME_MES 			= \
			datamodel.me \
			draft-averson-sigtran-m3pa-00.me \
			draft-bidulock-sigtran-as-00.me \
			draft-bidulock-sigtran-aspcong-00.me \
			draft-bidulock-sigtran-aspcong-01.me \
			draft-bidulock-sigtran-aspext-00.me \
			draft-bidulock-sigtran-aspext-01.me \
			draft-bidulock-sigtran-aspext-02.me \
			draft-bidulock-sigtran-aspext-03.me \
			draft-bidulock-sigtran-aspext-04.me \
			draft-bidulock-sigtran-aspext-05.me \
			draft-bidulock-sigtran-corid-00.me \
			draft-bidulock-sigtran-corid-01.me \
			draft-bidulock-sigtran-corid-02.me \
			draft-bidulock-sigtran-corid-03.me \
			draft-bidulock-sigtran-corid-04.me \
			draft-bidulock-sigtran-corid-05.me \
			draft-bidulock-sigtran-datamodel-00.me \
			draft-bidulock-sigtran-gapalg-00.me \
			draft-bidulock-sigtran-isua-00.me \
			draft-bidulock-sigtran-isua-01.me \
			draft-bidulock-sigtran-isua-02.me \
			draft-bidulock-sigtran-isua-03.me \
			draft-bidulock-sigtran-isua-04.me \
			draft-bidulock-sigtran-loadgrp-00.me \
			draft-bidulock-sigtran-loadgrp-01.me \
			draft-bidulock-sigtran-loadgrp-02.me \
			draft-bidulock-sigtran-loadgrp-03.me \
			draft-bidulock-sigtran-loadgrp-04.me \
			draft-bidulock-sigtran-loadgrp-05.me \
			draft-bidulock-sigtran-loadsel-00.me \
			draft-bidulock-sigtran-loadsel-01.me \
			draft-bidulock-sigtran-loadsel-02.me \
			draft-bidulock-sigtran-loadsel-03.me \
			draft-bidulock-sigtran-loadsel-04.me \
			draft-bidulock-sigtran-loadsel-05.me \
			draft-bidulock-sigtran-m2pa-ig-00.me \
			draft-bidulock-sigtran-m2pa-ig-01.me \
			draft-bidulock-sigtran-m2pa-ig-02.me \
			draft-bidulock-sigtran-m2pa-mib-00.me \
			draft-bidulock-sigtran-m2pa-test-00.me \
			draft-bidulock-sigtran-m2pa-test-01.me \
			draft-bidulock-sigtran-m2pa-test-02.me \
			draft-bidulock-sigtran-m2pa-test-03.me \
			draft-bidulock-sigtran-m2pa-test-04.me \
			draft-bidulock-sigtran-m2pa-test-05.me \
			draft-bidulock-sigtran-m2pa-test-06.me \
			draft-bidulock-sigtran-m2pa-test-07.me \
			draft-bidulock-sigtran-m2pa-test-08.me \
			draft-bidulock-sigtran-m2pa-test-09.me \
			draft-bidulock-sigtran-m2ua-ss7test-00.me \
			draft-bidulock-sigtran-m2ua-ss7test-01.me \
			draft-bidulock-sigtran-m2ua-ss7test-02.me \
			draft-bidulock-sigtran-m2ua-ss7test-03.me \
			draft-bidulock-sigtran-m3gw-00.me \
			draft-bidulock-sigtran-m3pa-01.me \
			draft-bidulock-sigtran-m3peer-01.me \
			draft-bidulock-sigtran-m3ua-test-00.me \
			draft-bidulock-sigtran-multisg-01.me \
			draft-bidulock-sigtran-multisg-02.me \
			draft-bidulock-sigtran-multisg-03.me \
			draft-bidulock-sigtran-multisg-04.me \
			draft-bidulock-sigtran-mutilsg-00.me \
			draft-bidulock-sigtran-regext-00.me \
			draft-bidulock-sigtran-regext-01.me \
			draft-bidulock-sigtran-regext-02.me \
			draft-bidulock-sigtran-regext-03.me \
			draft-bidulock-sigtran-regext-04.me \
			draft-bidulock-sigtran-sctpcongestion-00.me \
			draft-bidulock-sigtran-sctptli-00.me \
			draft-bidulock-sigtran-sessid-00.me \
			draft-bidulock-sigtran-sessid-01.me \
			draft-bidulock-sigtran-sessid-02.me \
			draft-bidulock-sigtran-sessid-03.me \
			draft-bidulock-sigtran-sessid-04.me \
			draft-bidulock-sigtran-sginfo-00.me \
			draft-bidulock-sigtran-sginfo-01.me \
			draft-bidulock-sigtran-sginfo-02.me \
			draft-bidulock-sigtran-sginfo-03.me \
			draft-bidulock-sigtran-sginfo-04.me \
			draft-bidulock-sigtran-sginfo-05.me \
			draft-bidulock-sigtran-sginfo-06.me \
			draft-bidulock-sigtran-sginfo-07.me \
			draft-bidulock-sigtran-suaco-00.me \
			draft-bidulock-sigtran-sua-test-00.me \
			draft-bidulock-sigtran-tua-00.me \
			draft-bidulock-sigtran-tua-01.me \
			draft-bidulock-sigtran-tua-02.me \
			draft-bidulock-sigtran-tua-03.me \
			draft-bidulock-sigtran-tua-04.me \
			draft-bidulock-sigtran-tua-05.me \
			draft-bidulock-sigtran-tua-06.me \
			draft-bidulock-tsvwg-isos-00.me \
			draft-bidulock-tsvwg-itos-00.me \
			draft-bidulock-tsvwg-sctpnpi-00.me \
			draft-bidulock-tsvwg-sctptpi-00.me \
			draft-bidulock-tsvwg-snare-00.me \
			draft-bidulock-tsvwg-xos-00.me

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= \
			aspext-01.fig1.fig \
			aspext-01.fig2.fig \
			aspext-01.fig3.fig \
			aspext-01.fig4.fig \
			aspext-02.fig1.fig \
			aspext-02.fig2.fig \
			aspext-02.fig3.fig \
			aspext-02.fig4.fig \
			aspext-03.fig1.fig \
			aspext-03.fig2.fig \
			aspext-03.fig3.fig \
			aspext-03.fig4.fig \
			corid-01.fig10.fig \
			corid-01.fig11.fig \
			corid-01.fig12.fig \
			corid-01.fig13.fig \
			corid-01.fig14.fig \
			corid-01.fig1.fig \
			corid-01.fig2.fig \
			corid-01.fig3.fig \
			corid-01.fig4.fig \
			corid-01.fig5.fig \
			corid-01.fig6.fig \
			corid-01.fig7.fig \
			corid-01.fig8.fig \
			corid-01.fig9.fig \
			corid-02.fig10.fig \
			corid-02.fig11.fig \
			corid-02.fig12.fig \
			corid-02.fig13.fig \
			corid-02.fig14.fig \
			corid-02.fig1.fig \
			corid-02.fig2.fig \
			corid-02.fig3.fig \
			corid-02.fig4.fig \
			corid-02.fig5.fig \
			corid-02.fig6.fig \
			corid-02.fig7.fig \
			corid-02.fig8.fig \
			corid-02.fig9.fig \
			corid-03.fig10.fig \
			corid-03.fig11.fig \
			corid-03.fig12.fig \
			corid-03.fig13.fig \
			corid-03.fig14.fig \
			corid-03.fig1.fig \
			corid-03.fig2.fig \
			corid-03.fig3.fig \
			corid-03.fig4.fig \
			corid-03.fig5.fig \
			corid-03.fig6.fig \
			corid-03.fig7.fig \
			corid-03.fig8.fig \
			corid-03.fig9.fig \
			datamodel.fig1.fig \
			datamodel.fig2.fig \
			datamodel.fig3.fig \
			datamodel.fig5.fig \
			datamodel.figx1.fig \
			datamodel.figx2.fig \
			datamodel.figx3.fig \
			datamodel.figx4.fig \
			datamodel.figx5.fig \
			datamodel.figx6.fig \
			datamodel.figx7.fig \
			isua-00.fig1.fig \
			isua-00.fig2.fig \
			isua-00.fig3.fig \
			isua-00.fig4.fig \
			isua-00.fig5.fig \
			isua-00.fig6.fig \
			isua-00.fig7.fig \
			isua-01.fig1.fig \
			isua-01.fig2.fig \
			isua-01.fig3.fig \
			isua-01.fig4.fig \
			isua-01.fig5.fig \
			isua-01.fig6.fig \
			isua-01.fig7.fig \
			isua-02.fig1.fig \
			isua-02.fig2.fig \
			isua-02.fig3.fig \
			isua-02.fig4.fig \
			isua-02.fig5.fig \
			isua-02.fig6.fig \
			isua-02.fig7.fig \
			itos-00.fig1.fig \
			itos-00.fig2.fig \
			loadgrp-01.fig10.fig \
			loadgrp-01.fig11.fig \
			loadgrp-01.fig12.fig \
			loadgrp-01.fig13.fig \
			loadgrp-01.fig1.fig \
			loadgrp-01.fig2.fig \
			loadgrp-01.fig3.fig \
			loadgrp-01.fig4.fig \
			loadgrp-01.fig5.fig \
			loadgrp-01.fig6.fig \
			loadgrp-01.fig7.fig \
			loadgrp-01.fig8.fig \
			loadgrp-01.fig9.fig \
			loadgrp-02.fig10.fig \
			loadgrp-02.fig11.fig \
			loadgrp-02.fig12.fig \
			loadgrp-02.fig13.fig \
			loadgrp-02.fig1.fig \
			loadgrp-02.fig2.fig \
			loadgrp-02.fig3.fig \
			loadgrp-02.fig4.fig \
			loadgrp-02.fig5.fig \
			loadgrp-02.fig6.fig \
			loadgrp-02.fig7.fig \
			loadgrp-02.fig8.fig \
			loadgrp-02.fig9.fig \
			loadgrp-03.fig10.fig \
			loadgrp-03.fig11.fig \
			loadgrp-03.fig12.fig \
			loadgrp-03.fig13.fig \
			loadgrp-03.fig1.fig \
			loadgrp-03.fig2.fig \
			loadgrp-03.fig3.fig \
			loadgrp-03.fig4.fig \
			loadgrp-03.fig5.fig \
			loadgrp-03.fig6.fig \
			loadgrp-03.fig7.fig \
			loadgrp-03.fig8.fig \
			loadgrp-03.fig9.fig \
			loadsel-01.fig10.fig \
			loadsel-01.fig11.fig \
			loadsel-01.fig12.fig \
			loadsel-01.fig13.fig \
			loadsel-01.fig1.fig \
			loadsel-01.fig2.fig \
			loadsel-01.fig3.fig \
			loadsel-01.fig4.fig \
			loadsel-01.fig5.fig \
			loadsel-01.fig6.fig \
			loadsel-01.fig7.fig \
			loadsel-01.fig8.fig \
			loadsel-01.fig9.fig \
			loadsel-02.fig10.fig \
			loadsel-02.fig11.fig \
			loadsel-02.fig12.fig \
			loadsel-02.fig13.fig \
			loadsel-02.fig1.fig \
			loadsel-02.fig2.fig \
			loadsel-02.fig3.fig \
			loadsel-02.fig4.fig \
			loadsel-02.fig5.fig \
			loadsel-02.fig6.fig \
			loadsel-02.fig7.fig \
			loadsel-02.fig8.fig \
			loadsel-02.fig9.fig \
			loadsel-03.fig10.fig \
			loadsel-03.fig11.fig \
			loadsel-03.fig12.fig \
			loadsel-03.fig13.fig \
			loadsel-03.fig1.fig \
			loadsel-03.fig2.fig \
			loadsel-03.fig3.fig \
			loadsel-03.fig4.fig \
			loadsel-03.fig5.fig \
			loadsel-03.fig6.fig \
			loadsel-03.fig7.fig \
			loadsel-03.fig8.fig \
			loadsel-03.fig9.fig \
			regext-01.fig1.fig \
			regext-01.fig2.fig \
			regext-01.fig3.fig \
			regext-01.fig4.fig \
			regext-01.fig5.fig \
			regext-02.fig1.fig \
			regext-02.fig2.fig \
			regext-02.fig3.fig \
			regext-02.fig4.fig \
			regext-02.fig5.fig \
			regext-03.fig1.fig \
			regext-03.fig2.fig \
			regext-03.fig3.fig \
			regext-03.fig4.fig \
			regext-03.fig5.fig \
			sginfo-02.fig1.fig \
			sginfo-02.fig2.fig \
			sginfo-02.fig3.fig \
			sginfo-02.fig4.fig \
			sginfo-03.fig1.fig \
			sginfo-03.fig2.fig \
			sginfo-03.fig3.fig \
			sginfo-03.fig4.fig \
			sginfo-04.fig1.fig \
			sginfo-04.fig2.fig \
			sginfo-04.fig3.fig \
			sginfo-04.fig4.fig \
			ss7test-00.fig1.fig \
			ss7test-00.fig2.fig \
			ss7test-00.fig3.fig \
			ss7test-00.fig4.fig \
			ss7test-00.fig5.fig \
			ss7test-00.fig6.fig \
			tua01.fig \
			tua02.fig \
			tua03.fig \
			tua04.fig \
			tua05.fig \
			tua06.fig \
			tua07.fig \
			tua-01.fig1.fig \
			tua-01.fig2.fig \
			tua-01.fig3.fig \
			tua-01.fig4.fig \
			tua-01.fig5.fig \
			tua-01.fig6.fig \
			tua-01.fig7.fig \
			tua-02.fig1.fig \
			tua-02.fig2.fig \
			tua-02.fig3.fig \
			tua-02.fig4.fig \
			tua-02.fig5.fig \
			tua-02.fig6.fig \
			tua-02.fig7.fig \
			tua-03.fig1.fig \
			tua-03.fig2.fig \
			tua-03.fig3.fig \
			tua-03.fig4.fig \
			tua-03.fig5.fig \
			tua-03.fig6.fig \
			tua-03.fig7.fig

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../am/drafts.am


# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
