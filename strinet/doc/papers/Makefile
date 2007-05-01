# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# -----------------------------------------------------------------------------
#
# $Log$
# =============================================================================

PLOTS = \
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
	netperf_suse_comp.plot \
	netperf_suse_delay.plot \
	netperf_suse_perf.plot \
	netperf_suse_thrput.plot \
	netperf_ubuntu_comp.plot \
	netperf_ubuntu_delay.plot \
	netperf_ubuntu_perf.plot \
	netperf_ubuntu_thrput.plot

EPSFIGS = $(PLOTS:.plot=.eps)

PDFFIGS = $(PLOTS:.plot=.pdf)

PLOTFIGS = $(PLOTS:.plot=.fig)

.PHONY: all
all: testresults.bbl testresults.ps testresults.pdf

$(PLOTFIGS): %.fig: %.plot
	gnuplot $<
	touch $@

$(EPSFIGS): %.eps: %.fig
	fig2dev -L eps $< > $@

$(PDFFIGS): %.pdf: %.fig
	fig2dev -L pdf -p dummy $< > $@

netperf_centos_comp.fig netperf_centos_delay.fig netperf_centos_perf.fig netperf_centos_thrput.fig: netperf_centos.dat
netperf_fc6_comp.fig netperf_fc6_delay.fig netperf_fc6_perf.fig netperf_fc6_thrput.fig: netperf_fc6.dat
netperf_nb_comp.fig netperf_nb_delay.fig netperf_nb_perf.fig netperf_nb_thrput.fig: netperf_nb.dat
netperf_rh7_comp.fig netperf_rh7_delay.fig netperf_rh7_perf.fig netperf_rh7_thrput.fig: netperf_rh7.dat
netperf_smp_comp.fig netperf_smp_delay.fig netperf_smp_perf.fig netperf_smp_thrput.fig: netperf_smp.dat
netperf_suse_comp.fig netperf_suse_delay.fig netperf_suse_perf.fig netperf_suse_thrput.fig: netperf_suse.dat
netperf_ubuntu_comp.fig netperf_ubuntu_delay.fig netperf_ubuntu_perf.fig netperf_ubuntu_thrput.fig: netperf_ubuntu.dat

testresults.aux: testresults.tex $(EPSFIGS)
	latex testresults.tex

testresults.bbl: testresults.bib testresults.tex $(EPSFIGS)
	if test -f testresults.aux ; then :; else make testresults.aux ; fi
	bibtex testresults

testresults.dvi: testresults.tex testresults.bbl $(EPSFIGS)
	latex testresults.tex
	latex testresults.tex

testresults.ps: testresults.tex testresults.bbl $(EPSFIGS)
	pslatex testresults.tex
	pslatex testresults.tex
	dvips -t letter -f -o $@ testresults.dvi

testresults.pdf: testresults.tex testresults.bbl $(PDFFIGS)
	pdflatex $<
	pdflatex $<

clean:
	rm -f $(EPSFIGS) $(PDFFIGS) $(PLOTFIGS) \
	testresults.ps testresults.pdf testresults.dvi \
	testresults.bbl testresults.aux testresults.blg testresults.glo \
	testresults.log
