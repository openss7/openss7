# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/10/18 06:53:06 $
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
# Last Modified $Date: 2007/10/18 06:53:06 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

if DEVELOPMENT
## DEVELOPMENT

PAPER_PREFIX = socktest

PLOTS = 

EPSFIGS = \
	  bsdnet.eps \
	  sockmod.eps \
	  sock.eps \
	  streamsnet.eps \
	  sockets.eps

PDFFIGS = \
	  bsdnet.pdf \
	  sockmod.pdf \
	  sock.pdf \
	  streamsnet.pdf \
	  sockets.pdf

GIFFIGS = \
	  bsdnet.gif \
	  sockmod.gif \
	  sock.gif \
	  streamsnet.gif \
	  sockets.gif

PNGFIGS = \
	  bsdnet.png \
	  sockmod.png \
	  sock.png \
	  streamsnet.png \
	  sockets.png

FIGFIGS = \
	  bsdnet.fig \
	  sockmod.fig \
	  sock.fig \
	  streamsnet.fig \
	  sockets.fig

PLOTFIGS = $(PLOTS:.plot=.fig)
PLOTEPSS = $(PLOTS:.plot=.eps)
PLOTPDFS = $(PLOTS:.plot=.pdf)
PLOTGIFS = $(PLOTS:.plot=.gif)
PLOTPNGS = $(PLOTS:.plot=.png)

NAME = sockimp

MY_PHONY += all-papers all-dat

all-papers: $(NAME).bbl $(NAME).pdf

ALL_LOCAL += all-papers

$(PLOTFIGS): %.fig: %.plot
	@make all-dat
	$(GNUPLOT) $<

$(EPSFIGS): %.eps: %.fig
	$(FIG2DEV) -L eps -e -g '#ffffff' -b 0 $< $@

$(PLOTEPSS): %.eps: %.fig
	$(FIG2DEV) -L eps -e -g '#f7f7f7' -b 0 $< $@

$(PDFFIGS): %.pdf: %.fig
	$(FIG2DEV) -L pdf -e -p dummy -g '#ffffff' -b 0 $< $@

$(PLOTPDFS): %.pdf: %.fig
	$(FIG2DEV) -L pdf -e -p dummy -g '#f7f7f7' -b 0 $< $@

$(GIFFIGS): %.gif: %.fig
	$(FIG2DEV) -L gif -g '#c0d0f0' -t '#c0d0f0' -b 0 -S 2 $< $@

$(PLOTGIFS): %.gif: %.fig
	$(FIG2DEV) -L gif -g 'white' -b 15 -S 2 $< $@

#$(PNGFIGS): %.png: %.fig
#	$(FIG2DEV) -L png -b 0 -S 4 $< $@

$(PNGFIGS): %.png: %.gif
	$(CONVERT) -background '#c0d0f0' -transparent '#ffffff' -interlace Line $< $@

$(PLOTPNGS): %.png: %.gif
	$(CONVERT) -background 'white' -interlace Line $< $@

MY_PHONY += gif png eps pdf

gif: $(GIFFIGS) $(PLOTGIFS)
png: $(PNGFIGS) $(PLOTPNGS)
eps: $(EPSFIGS) $(PLOTEPSS)
pdf: $(PDFFIGS) $(PLOTPDFS)

$(PAPER_PREFIX)_centos_comp.fig $(PAPER_PREFIX)_centos_delay.fig $(PAPER_PREFIX)_centos_perf.fig $(PAPER_PREFIX)_centos_thrput.fig: $(PAPER_PREFIX)_centos.dat
$(PAPER_PREFIX)_fc6_comp.fig $(PAPER_PREFIX)_fc6_delay.fig $(PAPER_PREFIX)_fc6_perf.fig $(PAPER_PREFIX)_fc6_thrput.fig: $(PAPER_PREFIX)_fc6.dat
$(PAPER_PREFIX)_nb_comp.fig $(PAPER_PREFIX)_nb_delay.fig $(PAPER_PREFIX)_nb_perf.fig $(PAPER_PREFIX)_nb_thrput.fig: $(PAPER_PREFIX)_nb.dat
$(PAPER_PREFIX)_rh7_comp.fig $(PAPER_PREFIX)_rh7_delay.fig $(PAPER_PREFIX)_rh7_perf.fig $(PAPER_PREFIX)_rh7_thrput.fig: $(PAPER_PREFIX)_rh7.dat
$(PAPER_PREFIX)_smp_comp.fig $(PAPER_PREFIX)_smp_delay.fig $(PAPER_PREFIX)_smp_perf.fig $(PAPER_PREFIX)_smp_thrput.fig: $(PAPER_PREFIX)_smp.dat
$(PAPER_PREFIX)_cos5_comp.fig $(PAPER_PREFIX)_cos5_delay.fig $(PAPER_PREFIX)_cos5_perf.fig $(PAPER_PREFIX)_cos5_thrput.fig: $(PAPER_PREFIX)_cos5.dat
$(PAPER_PREFIX)_suse_comp.fig $(PAPER_PREFIX)_suse_delay.fig $(PAPER_PREFIX)_suse_perf.fig $(PAPER_PREFIX)_suse_thrput.fig: $(PAPER_PREFIX)_suse.dat
$(PAPER_PREFIX)_u704_comp.fig $(PAPER_PREFIX)_u704_delay.fig $(PAPER_PREFIX)_u704_perf.fig $(PAPER_PREFIX)_u704_thrput.fig: $(PAPER_PREFIX)_u704.dat
$(PAPER_PREFIX)_ubuntu_comp.fig $(PAPER_PREFIX)_ubuntu_delay.fig $(PAPER_PREFIX)_ubuntu_perf.fig $(PAPER_PREFIX)_ubuntu_thrput.fig: $(PAPER_PREFIX)_ubuntu.dat

PLOT_DATS =

PAPER_BIBS = \
	$(NAME).bib

PAPER_TEXS = \
     $(NAME).tex

PAPER_CSSS = \
     org.css

all-dat:
	@ files="$(PLOT_DATS) $(PAPER_BIBS) $(PAPER_CSSS) $(PAPER_TEXS)" ; for f in $$files ; do \
		if test ! -f $$f -o $(srcdir)/$$f -nt $$f ; then \
			if test -f $(srcdir)/$$f ; then \
				$(ECHO) "cp -pf $(srcdir)/$$f $$f" ; \
				cp -pf $(srcdir)/$$f $$f ; \
			else \
				$(ECHO) "touch $$f" ; \
				touch $$f ; \
			fi ; \
		fi ; \
	done

clean-dat:
	@ files="$(PLOT_DATS) $(PAPER_BIBS) $(PAPER_CSSS) $(PAPER_TEXS)" ; for f in $$files ; do \
		if test ! $$f -ef $(srcdir)/$$f ; then \
			$(ECHO) "rm -f $$f" ; \
			rm -f $$f ; \
		fi ; \
	done

$(NAME).aux: $(NAME).tex $(EPSFIGS) $(PLOTEPSS)
	$(LATEX) $(NAME).tex
	$(LATEX) $(NAME).tex

$(NAME).bbl: $(NAME).bib $(NAME).tex $(EPSFIGS) $(PLOTFIGS)
	if test -f $(NAME).aux ; then :; else make $(NAME).aux ; fi
	$(BIBTEX) $(NAME)

$(NAME).dvi: $(NAME).tex $(NAME).bbl $(EPSFIGS) $(PLOTEPSS)
	rm -f $(GIFFIGS) $(PLOTGIFS)
	rm -f $(PNGFIGS) $(PLOTPNGS)
	$(LATEX) $(NAME).tex
	$(LATEX) $(NAME).tex

$(NAME).ps: $(NAME).tex $(NAME).bbl $(EPSFIGS) $(PLOTEPSS)
	rm -f $(GIFFIGS) $(PLOTGIFS)
	rm -f $(PNGFIGS) $(PLOTPNGS)
	$(PSLATEX) $(NAME).tex
	$(PSLATEX) $(NAME).tex
	$(DVIPS) -t letter -f -o $@ $(NAME).dvi

$(NAME).pdf: $(NAME).tex $(NAME).bbl $(PDFFIGS) $(PLOTPDFS)
	rm -f $(GIFFIGS) $(PLOTGIFS)
	rm -f $(PNGFIGS) $(PLOTPNGS)
	$(PDFLATEX) $(NAME).tex
	$(PDFLATEX) $(NAME).tex

$(NAME).html: $(NAME).tex $(NAME).bbl $(PNGFIGS) $(PLOTPNGS)
	$(LATEX2HTML) -split 0 -noexternal_images -noascii_mode -font_size 12pt \
		-nonavigation -numbered_footnotes -nosubdir -rooted -noinfo \
		-norooted -antialias_text -antialias -white -image_type png \
		-scalable_fonts $(NAME).tex
	cat org.css >> $(NAME).css

clean-gif:
	rm -f $(GIFFIGS) $(PLOTGIFS)

clean-png:
	rm -f $(PNGFIGS) $(PLOTPNGS)

clean-eps:
	rm -f $(EPSFIGS) $(PLOTEPSS)

clean-pdf:
	rm -f $(PDFFIGS) $(PLOTPDFS)

clean-fig: clean-gif clean-png clean-eps clean-pdf
	rm -f $(PLOTFIGS)

clean-papers: clean-fig clean-dat
	rm -f *.bak $(NAME).ps $(NAME).pdf $(NAME).dvi \
	$(NAME).aux $(NAME).bbl $(NAME).blg $(NAME).glo $(NAME).log $(NAME).pl \
	images.aux images.bbl images.glo images.log images.pl images.tex img*.png \
	index.html internals.pl labels.pl \
	$(NAME).css $(NAME).html WARNINGS

CLEAN_LOCAL += clean-papers

MY_PHONY += clean-papers clean-fig clean-gif clean-png clean-eps clean-pdf clean-dat

EXTRA_DIST += $(NAME).pdf $(NAME).tex $(NAME).bib $(FIGFIGS) $(PLOT_DATS) $(PLOTS) org.css

.INTERMEDIATE: \
	$(PLOTPNGS) $(PNGFIGS) \
	$(PLOTEPSS) $(EPSFIGS) \
	$(PLOTPDFS) $(PDFFIGS) \
	$(PLOTGIFS) $(GIFFIGS) \
	$(PLOTFIGS)

## DEVELOPMENT
endif

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.5  2007/10/18 06:53:06  brian
# - new build support
#
# Revision 0.9.2.4  2007/08/14 05:17:09  brian
# - GPLv3 header update
#
# Revision 0.9.2.3  2007/06/18 09:20:51  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.2  2007/06/18 09:17:18  brian
# - updates to pass distcheck target
#
# Revision 0.9.2.1  2007/06/17 02:13:20  brian
# - add automake files for papers
#
# Revision 0.9.2.1  2007/06/13 20:18:50  brian
# - added papers to release
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
