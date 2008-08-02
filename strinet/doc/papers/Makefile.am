# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2008-08-02 07:26:09 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
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
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2008-08-02 07:26:09 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if DEVELOPMENT
## DEVELOPMENT

PAPER_PREFIX = netperf

PLOT_PLOTS = \
	$(PAPER_PREFIX)_centos_comp.plot \
	$(PAPER_PREFIX)_centos_delay.plot \
	$(PAPER_PREFIX)_centos_perf.plot \
	$(PAPER_PREFIX)_centos_thrput.plot \
	$(PAPER_PREFIX)_fc6_comp.plot \
	$(PAPER_PREFIX)_fc6_delay.plot \
	$(PAPER_PREFIX)_fc6_perf.plot \
	$(PAPER_PREFIX)_fc6_thrput.plot \
	$(PAPER_PREFIX)_nb_comp.plot \
	$(PAPER_PREFIX)_nb_delay.plot \
	$(PAPER_PREFIX)_nb_perf.plot \
	$(PAPER_PREFIX)_nb_thrput.plot \
	$(PAPER_PREFIX)_rh7_comp.plot \
	$(PAPER_PREFIX)_rh7_delay.plot \
	$(PAPER_PREFIX)_rh7_perf.plot \
	$(PAPER_PREFIX)_rh7_thrput.plot \
	$(PAPER_PREFIX)_smp_comp.plot \
	$(PAPER_PREFIX)_smp_delay.plot \
	$(PAPER_PREFIX)_smp_perf.plot \
	$(PAPER_PREFIX)_smp_thrput.plot \
	$(PAPER_PREFIX)_cos5_comp.plot \
	$(PAPER_PREFIX)_cos5_delay.plot \
	$(PAPER_PREFIX)_cos5_perf.plot \
	$(PAPER_PREFIX)_cos5_thrput.plot \
	$(PAPER_PREFIX)_suse_comp.plot \
	$(PAPER_PREFIX)_suse_delay.plot \
	$(PAPER_PREFIX)_suse_perf.plot \
	$(PAPER_PREFIX)_suse_thrput.plot \
	$(PAPER_PREFIX)_u704_comp.plot \
	$(PAPER_PREFIX)_u704_delay.plot \
	$(PAPER_PREFIX)_u704_perf.plot \
	$(PAPER_PREFIX)_u704_thrput.plot \
	$(PAPER_PREFIX)_ubuntu_comp.plot \
	$(PAPER_PREFIX)_ubuntu_delay.plot \
	$(PAPER_PREFIX)_ubuntu_perf.plot \
	$(PAPER_PREFIX)_ubuntu_thrput.plot

FIG_FIGS = \
	sockets.fig \
	xtios.fig \
	udp.fig

NAME = testresults

$(PAPER_PREFIX)_centos_comp.fig $(PAPER_PREFIX)_centos_delay.fig $(PAPER_PREFIX)_centos_perf.fig $(PAPER_PREFIX)_centos_thrput.fig: $(PAPER_PREFIX)_centos.dat
$(PAPER_PREFIX)_fc6_comp.fig $(PAPER_PREFIX)_fc6_delay.fig $(PAPER_PREFIX)_fc6_perf.fig $(PAPER_PREFIX)_fc6_thrput.fig: $(PAPER_PREFIX)_fc6.dat
$(PAPER_PREFIX)_nb_comp.fig $(PAPER_PREFIX)_nb_delay.fig $(PAPER_PREFIX)_nb_perf.fig $(PAPER_PREFIX)_nb_thrput.fig: $(PAPER_PREFIX)_nb.dat
$(PAPER_PREFIX)_rh7_comp.fig $(PAPER_PREFIX)_rh7_delay.fig $(PAPER_PREFIX)_rh7_perf.fig $(PAPER_PREFIX)_rh7_thrput.fig: $(PAPER_PREFIX)_rh7.dat
$(PAPER_PREFIX)_smp_comp.fig $(PAPER_PREFIX)_smp_delay.fig $(PAPER_PREFIX)_smp_perf.fig $(PAPER_PREFIX)_smp_thrput.fig: $(PAPER_PREFIX)_smp.dat
$(PAPER_PREFIX)_cos5_comp.fig $(PAPER_PREFIX)_cos5_delay.fig $(PAPER_PREFIX)_cos5_perf.fig $(PAPER_PREFIX)_cos5_thrput.fig: $(PAPER_PREFIX)_cos5.dat
$(PAPER_PREFIX)_suse_comp.fig $(PAPER_PREFIX)_suse_delay.fig $(PAPER_PREFIX)_suse_perf.fig $(PAPER_PREFIX)_suse_thrput.fig: $(PAPER_PREFIX)_suse.dat
$(PAPER_PREFIX)_u704_comp.fig $(PAPER_PREFIX)_u704_delay.fig $(PAPER_PREFIX)_u704_perf.fig $(PAPER_PREFIX)_u704_thrput.fig: $(PAPER_PREFIX)_u704.dat
$(PAPER_PREFIX)_ubuntu_comp.fig $(PAPER_PREFIX)_ubuntu_delay.fig $(PAPER_PREFIX)_ubuntu_perf.fig $(PAPER_PREFIX)_ubuntu_thrput.fig: $(PAPER_PREFIX)_ubuntu.dat

PLOT_DATS = \
	$(PAPER_PREFIX)_centos.dat \
	$(PAPER_PREFIX)_fc6.dat \
	$(PAPER_PREFIX)_nb.dat \
	$(PAPER_PREFIX)_rh7.dat \
	$(PAPER_PREFIX)_smp.dat \
	$(PAPER_PREFIX)_cos5.dat \
	$(PAPER_PREFIX)_suse.dat \
	$(PAPER_PREFIX)_u704.dat \
	$(PAPER_PREFIX)_ubuntu.dat

LATEXS = \
     $(NAME).tex

CSSS = \
     org.css

## DEVELOPMENT
endif

include ../../am/papers.am

EXTRA_DIST += \
	netperf_centos_comp.plot \
	netperf_centos_delay.plot \
	netperf_centos_perf.plot \
	netperf_centos_thrput.plot \
	netperf_fc6_comp.plot \
	netperf_fc6_delay.plot \
	netperf_fc6_perf.plot \
	netperf_fc6_thrput.plot \
	netperf_nb_comp.plot \
	netperf_nb_delay.plot \
	netperf_nb_perf.plot \
	netperf_nb_thrput.plot \
	netperf_rh7_comp.plot \
	netperf_rh7_delay.plot \
	netperf_rh7_perf.plot \
	netperf_rh7_thrput.plot \
	netperf_smp_comp.plot \
	netperf_smp_delay.plot \
	netperf_smp_perf.plot \
	netperf_smp_thrput.plot \
	netperf_cos5_comp.plot \
	netperf_cos5_delay.plot \
	netperf_cos5_perf.plot \
	netperf_cos5_thrput.plot \
	netperf_suse_comp.plot \
	netperf_suse_delay.plot \
	netperf_suse_perf.plot \
	netperf_suse_thrput.plot \
	netperf_u704_comp.plot \
	netperf_u704_delay.plot \
	netperf_u704_perf.plot \
	netperf_u704_thrput.plot \
	netperf_ubuntu_comp.plot \
	netperf_ubuntu_delay.plot \
	netperf_ubuntu_perf.plot \
	netperf_ubuntu_thrput.plot \
	netperf_centos.dat \
	netperf_fc6.dat \
	netperf_nb.dat \
	netperf_rh7.dat \
	netperf_smp.dat \
	netperf_cos5.dat \
	netperf_suse.dat \
	netperf_u704.dat \
	netperf_ubuntu.dat \
	sockets.fig \
	xtios.fig \
	udp.fig \
	testresults.tex \
	org.css

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.9  2008-08-02 07:26:09  brian
# - autoconf 2.62 does not like variables in EXTRA_DIST
#
# Revision 0.9.2.8  2008-04-28 22:52:09  brian
# - updated headers for release
#
# Revision 0.9.2.7  2008-04-11 16:58:39  brian
# - removed deprecated NEXUSWARE support
#
# Revision 0.9.2.6  2007/10/18 06:52:54  brian
# - new build support
#
# Revision 0.9.2.5  2007/08/14 04:27:01  brian
# - GPLv3 header update
#
# Revision 0.9.2.4  2007/06/18 09:20:39  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.3  2007/06/18 09:17:13  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.2  2007/06/17 01:56:55  brian
# - updates for release, remove any later language
#
# Revision 0.9.2.1  2007/06/13 20:18:50  brian
# - added papers to release
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
