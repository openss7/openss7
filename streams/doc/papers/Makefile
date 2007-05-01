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
	perftest_suse_comp.plot \
	perftest_suse_delay.plot \
	perftest_suse_perf.plot \
	perftest_suse_thrput.plot \
	perftest_ubuntu_comp.plot \
	perftest_ubuntu_delay.plot \
	perftest_ubuntu_perf.plot \
	perftest_ubuntu_thrput.plot

EPSFIGS = $(PLOTS:.plot=.eps) \
	legacypipe.eps \
	streamspipe.eps \
	socketpipe.eps

PDFFIGS = $(PLOTS:.plot=.pdf) \
	legacypipe.pdf \
	streamspipe.pdf \
	socketpipe.pdf

PLOTFIGS = $(PLOTS:.plot=.fig)

.PHONY: all
all: piperesults.bbl piperesults.ps piperesults.pdf

$(PLOTFIGS): %.fig: %.plot
	gnuplot $<
	touch $@

$(EPSFIGS): %.eps: %.fig
	fig2dev -L eps $< > $@

$(PDFFIGS): %.pdf: %.fig
	fig2dev -L pdf -p dummy $< > $@

perftest_fc6_comp.fig perftest_fc6_delay.fig perftest_fc6_perf.fig perftest_fc6_thrput.fig: perftest_fc6.dat
perftest_nb_comp.fig perftest_nb_delay.fig perftest_nb_perf.fig perftest_nb_thrput.fig: perftest_nb.dat
perftest_smp_comp.fig perftest_smp_delay.fig perftest_smp_perf.fig perftest_smp_thrput.fig: perftest_smp.dat
perftest_rh7_comp.fig perftest_rh7_delay.fig perftest_rh7_perf.fig perftest_rh7_thrput.fig: perftest_rh7.dat
perftest_centos_comp.fig perftest_centos_delay.fig perftest_centos_perf.fig perftest_centos_thrput.fig: perftest_centos.dat
perftest_suse_comp.fig perftest_suse_delay.fig perftest_suse_perf.fig perftest_suse_thrput.fig: perftest_suse.dat
perftest_ubuntu_comp.fig perftest_ubuntu_delay.fig perftest_ubuntu_perf.fig perftest_ubuntu_thrput.fig: perftest_ubuntu.dat

piperesults.aux: piperesults.tex $(EPSFIGS)
	latex piperesults.tex

piperesults.bbl: piperesults.bib piperesults.tex $(EPSFIGS)
	if test -f piperesults.aux ; then :; else make piperesults.aux ; fi
	bibtex piperesults

piperesults.dvi: piperesults.tex piperesults.bbl $(EPSFIGS)
	latex piperesults.tex
	latex piperesults.tex

piperesults.ps: piperesults.tex piperesults.bbl $(EPSFIGS)
	pslatex piperesults.tex
	pslatex piperesults.tex
	dvips -t letter -f -o $@ piperesults.dvi

piperesults.pdf: piperesults.tex piperesults.bbl $(PDFFIGS)
	pdflatex $<
	pdflatex $<

clean:
	rm -f $(EPSFIGS) $(PDFFIGS) $(PLOTFIGS) \
	piperesults.ps piperesults.pdf piperesults.dvi \
	piperesults.bbl piperesults.aux piperesults.blg piperesults.glo piperesults.log
