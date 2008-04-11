# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/10/18 06:53:02 $
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
# Last Modified $Date: 2007/10/18 06:53:02 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

include ../../am/papers.am

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
	$(PAPER_PREFIX)_suse_comp.plot \
	$(PAPER_PREFIX)_suse_delay.plot \
	$(PAPER_PREFIX)_suse_perf.plot \
	$(PAPER_PREFIX)_suse_thrput.plot \
	$(PAPER_PREFIX)_u704_comp.plot \
	$(PAPER_PREFIX)_u704_delay.plot \
	$(PAPER_PREFIX)_u704_perf.plot \
	$(PAPER_PREFIX)_u704_thrput.plot

FIG_FIGS = \
	sockets.fig \
	xtios.fig \
	sctp.fig

NAME = sctpresults

$(PAPER_PREFIX)_centos_comp.fig $(PAPER_PREFIX)_centos_delay.fig $(PAPER_PREFIX)_centos_perf.fig $(PAPER_PREFIX)_centos_thrput.fig: $(PAPER_PREFIX)_centos.dat
$(PAPER_PREFIX)_fc6_comp.fig $(PAPER_PREFIX)_fc6_delay.fig $(PAPER_PREFIX)_fc6_perf.fig $(PAPER_PREFIX)_fc6_thrput.fig: $(PAPER_PREFIX)_fc6.dat
$(PAPER_PREFIX)_nb_comp.fig $(PAPER_PREFIX)_nb_delay.fig $(PAPER_PREFIX)_nb_perf.fig $(PAPER_PREFIX)_nb_thrput.fig: $(PAPER_PREFIX)_nb.dat
$(PAPER_PREFIX)_rh7_comp.fig $(PAPER_PREFIX)_rh7_delay.fig $(PAPER_PREFIX)_rh7_perf.fig $(PAPER_PREFIX)_rh7_thrput.fig: $(PAPER_PREFIX)_rh7.dat
$(PAPER_PREFIX)_smp_comp.fig $(PAPER_PREFIX)_smp_delay.fig $(PAPER_PREFIX)_smp_perf.fig $(PAPER_PREFIX)_smp_thrput.fig: $(PAPER_PREFIX)_smp.dat
$(PAPER_PREFIX)_suse_comp.fig $(PAPER_PREFIX)_suse_delay.fig $(PAPER_PREFIX)_suse_perf.fig $(PAPER_PREFIX)_suse_thrput.fig: $(PAPER_PREFIX)_suse.dat
$(PAPER_PREFIX)_u704_comp.fig $(PAPER_PREFIX)_u704_delay.fig $(PAPER_PREFIX)_u704_perf.fig $(PAPER_PREFIX)_u704_thrput.fig: $(PAPER_PREFIX)_u704.dat

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

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.6  2007/10/18 06:53:02  brian
# - new build support
#
# Revision 0.9.2.5  2007/08/14 06:22:14  brian
# - GPLv3 header update
#
# Revision 0.9.2.4  2007/06/18 09:20:40  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.3  2007/06/18 09:17:16  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.2  2007/06/17 01:57:09  brian
# - updates for release, remove any later language
#
# Revision 0.9.2.1  2007/06/13 20:18:52  brian
# - added papers to release
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
