# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2008-08-02 16:42:26 $
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
# Last Modified $Date: 2008-08-02 16:42:26 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if DEVELOPMENT
## DEVELOPMENT

PAPER_PREFIX = perftest

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
	$(PAPER_PREFIX)_cos52_comp.plot \
	$(PAPER_PREFIX)_cos52_delay.plot \
	$(PAPER_PREFIX)_cos52_perf.plot \
	$(PAPER_PREFIX)_cos52_thrput.plot \
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
	legacypipe.fig \
	streamspipe.fig \
	socketpipe.fig

$(PAPER_PREFIX)_centos_comp.fig $(PAPER_PREFIX)_centos_delay.fig $(PAPER_PREFIX)_centos_perf.fig $(PAPER_PREFIX)_centos_thrput.fig: $(PAPER_PREFIX)_centos.dat
$(PAPER_PREFIX)_fc6_comp.fig $(PAPER_PREFIX)_fc6_delay.fig $(PAPER_PREFIX)_fc6_perf.fig $(PAPER_PREFIX)_fc6_thrput.fig: $(PAPER_PREFIX)_fc6.dat
$(PAPER_PREFIX)_nb_comp.fig $(PAPER_PREFIX)_nb_delay.fig $(PAPER_PREFIX)_nb_perf.fig $(PAPER_PREFIX)_nb_thrput.fig: $(PAPER_PREFIX)_nb.dat
$(PAPER_PREFIX)_rh7_comp.fig $(PAPER_PREFIX)_rh7_delay.fig $(PAPER_PREFIX)_rh7_perf.fig $(PAPER_PREFIX)_rh7_thrput.fig: $(PAPER_PREFIX)_rh7.dat
$(PAPER_PREFIX)_smp_comp.fig $(PAPER_PREFIX)_smp_delay.fig $(PAPER_PREFIX)_smp_perf.fig $(PAPER_PREFIX)_smp_thrput.fig: $(PAPER_PREFIX)_smp.dat
$(PAPER_PREFIX)_cos5_comp.fig $(PAPER_PREFIX)_cos5_delay.fig $(PAPER_PREFIX)_cos5_perf.fig $(PAPER_PREFIX)_cos5_thrput.fig: $(PAPER_PREFIX)_cos5.dat
$(PAPER_PREFIX)_cos52_comp.fig $(PAPER_PREFIX)_cos52_delay.fig $(PAPER_PREFIX)_cos52_perf.fig $(PAPER_PREFIX)_cos52_thrput.fig: $(PAPER_PREFIX)_cos52.dat
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
	$(PAPER_PREFIX)_cos52.dat \
	$(PAPER_PREFIX)_suse.dat \
	$(PAPER_PREFIX)_u704.dat \
	$(PAPER_PREFIX)_ubuntu.dat

LATEXS = \
     piperesults.tex

## DEVELOPMENT
endif

include ../../am/papers.am

EXTRA_DIST += \
	perftest_centos_comp.plot \
	perftest_centos_delay.plot \
	perftest_centos_perf.plot \
	perftest_centos_thrput.plot \
	perftest_fc6_comp.plot \
	perftest_fc6_delay.plot \
	perftest_fc6_perf.plot \
	perftest_fc6_thrput.plot \
	perftest_nb_comp.plot \
	perftest_nb_delay.plot \
	perftest_nb_perf.plot \
	perftest_nb_thrput.plot \
	perftest_rh7_comp.plot \
	perftest_rh7_delay.plot \
	perftest_rh7_perf.plot \
	perftest_rh7_thrput.plot \
	perftest_smp_comp.plot \
	perftest_smp_delay.plot \
	perftest_smp_perf.plot \
	perftest_smp_thrput.plot \
	perftest_cos5_comp.plot \
	perftest_cos5_delay.plot \
	perftest_cos5_perf.plot \
	perftest_cos5_thrput.plot \
	perftest_cos52_comp.plot \
	perftest_cos52_delay.plot \
	perftest_cos52_perf.plot \
	perftest_cos52_thrput.plot \
	perftest_suse_comp.plot \
	perftest_suse_delay.plot \
	perftest_suse_perf.plot \
	perftest_suse_thrput.plot \
	perftest_u704_comp.plot \
	perftest_u704_delay.plot \
	perftest_u704_perf.plot \
	perftest_u704_thrput.plot \
	perftest_ubuntu_comp.plot \
	perftest_ubuntu_delay.plot \
	perftest_ubuntu_perf.plot \
	perftest_ubuntu_thrput.plot \
	perftest_centos.dat \
	perftest_fc6.dat \
	perftest_nb.dat \
	perftest_rh7.dat \
	perftest_smp.dat \
	perftest_cos5.dat \
	perftest_cos52.dat \
	perftest_suse.dat \
	perftest_u704.dat \
	perftest_ubuntu.dat \
	legacypipe.fig \
	streamspipe.fig \
	socketpipe.fig \
	piperesults.tex \
	piperesults.css \
	org.css

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.12  2008-08-02 16:42:26  brian
# - updates for release
#
# Revision 0.9.2.11  2008-08-02 07:25:41  brian
# - autoconf 2.62 does not like variables in EXTRA_DIST
#
# Revision 0.9.2.10  2008-07-26 18:59:20  brian
# - added new data for CentOS 5.2
#
# Revision 0.9.2.9  2008-04-11 16:58:36  brian
# - removed deprecated NEXUSWARE support
#
# Revision 0.9.2.8  2007/12/15 20:19:46  brian
# - updates
#
# Revision 0.9.2.7  2007/10/18 06:52:51  brian
# - new build support
#
# Revision 0.9.2.6  2007/10/17 17:43:00  brian
# - build updates
#
# Revision 0.9.2.5  2007/08/13 22:46:01  brian
# - GPLv3 header updates
#
# Revision 0.9.2.4  2007/06/18 09:20:38  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.3  2007/06/18 09:17:10  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.2  2007/06/17 01:56:49  brian
# - updates for release, remove any later language
#
# Revision 0.9.2.1  2007/06/13 20:28:04  brian
# - added papers to release
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
