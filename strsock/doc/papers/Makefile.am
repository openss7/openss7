## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/06/17 02:13:20 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, write to the Free Software Foundation, Inc., 675 Mass
## Ave, Cambridge, MA 02139, USA.
##
## -----------------------------------------------------------------------------
##
## U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
## behalf of the U.S. Government ("Government"), the following provisions apply
## to you.  If the Software is supplied by the Department of Defense ("DoD"), it
## is classified as "Commercial Computer Software" under paragraph 252.227-7014
## of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
## successor regulations) and the Government is acquiring only the license rights
## granted herein (the license rights customarily provided to non-Government
## users).  If the Software is supplied to any unit or agency of the Government
## other than DoD, it is classified as "Restricted Computer Software" and the
## Government's rights in the Software are defined in paragraph 52.227-19 of the
## Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
## the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
## (or any successor regulations).
##
## -----------------------------------------------------------------------------
##
## Commercial licensing and support of this software is available from OpenSS7
## Corporation at a fee.  See http://www.openss7.com/
##
## -----------------------------------------------------------------------------
##
## Last Modified $Date: 2007/06/17 02:13:20 $ by $Author: brian $
##
## -----------------------------------------------------------------------------
##
## $Log: Makefile.am,v $
## Revision 0.9.2.1  2007/06/17 02:13:20  brian
## - add automake files for papers
##
## Revision 0.9.2.1  2007/06/13 20:18:50  brian
## - added papers to release
##
## =============================================================================

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

NAME = sockimp

MY_PHONY += all all-dat

all: $(NAME).bbl $(NAME).pdf

$(PLOTFIGS): %.fig: %.plot
	@make all-dat
	gnuplot $<

$(EPSFIGS): %.eps: %.fig
	fig2dev -L eps -e -g '#ffffff' -b 0 $< > $@

$(PLOTS:.plot=.eps): %.eps: %.fig
	fig2dev -L eps -e -g '#f7f7f7' -b 0 $< > $@

$(PDFFIGS): %.pdf: %.fig
	fig2dev -L pdf -e -p dummy -g '#ffffff' -b 0 $< > $@

$(PLOTS:.plot=.pdf): %.pdf: %.fig
	fig2dev -L pdf -e -p dummy -g '#f7f7f7' -b 0 $< > $@

$(GIFFIGS): %.gif: %.fig
	fig2dev -L gif -g '#c0d0f0' -t '#c0d0f0' -b 0 -S 2 $< > $@

$(PLOTS:.plot=.gif): %.gif: %.fig
	fig2dev -L gif -g 'white' -b 15 -S 2 $< > $@

#$(PNGFIGS): %.png: %.fig
#	fig2dev -L png -b 0 -S 4 $< > $@

$(PNGFIGS): %.png: %.gif
	convert -background '#c0d0f0' -transparent '#ffffff' -interlace Line $< $@

$(PLOTS:.plot=.png): %.png: %.gif
	convert -background 'white' -interlace Line $< $@

MY_PHONY += gif png eps pdf

gif: $(GIFFIGS) $(PLOTS:.plot=.gif)
png: $(PNGFIGS) $(PLOTS:.plot=.png)
eps: $(EPSFIGS) $(PLOTS:.plot=.eps)
pdf: $(PDFFIGS) $(PLOTS:.plot=.pdf)

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

$(NAME).aux: $(NAME).tex $(EPSFIGS) $(PLOTS:.plot=.eps)
	latex $(NAME).tex
	latex $(NAME).tex

$(NAME).bbl: $(NAME).bib $(NAME).tex $(EPSFIGS) $(PLOTS:.plot=.eps)
	if test -f $(NAME).aux ; then :; else make $(NAME).aux ; fi
	bibtex $(NAME)

$(NAME).dvi: $(NAME).tex $(NAME).bbl $(EPSFIGS) $(PLOTS:.plot=.eps)
	rm -f $(GIFFIGS) $(PLOTS:.plot=.gif)
	rm -f $(PNGFIGS) $(PLOTS:.plot=.png)
	latex $(NAME).tex
	latex $(NAME).tex

$(NAME).ps: $(NAME).tex $(NAME).bbl $(EPSFIGS) $(PLOTS:.plot=.eps)
	rm -f $(GIFFIGS) $(PLOTS:.plot=.gif)
	rm -f $(PNGFIGS) $(PLOTS:.plot=.png)
	pslatex $(NAME).tex
	pslatex $(NAME).tex
	dvips -t letter -f -o $@ $(NAME).dvi

$(NAME).pdf: $(NAME).tex $(NAME).bbl $(PDFFIGS) $(PLOTS:.plot=.pdf)
	rm -f $(GIFFIGS) $(PLOTS:.plot=.gif)
	rm -f $(PNGFIGS) $(PLOTS:.plot=.png)
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex

$(NAME).html: $(NAME).tex $(NAME).bbl $(PNGFIGS) $(PLOTS:.plot=.png)
	/usr/bin/latex2html -split 0 -noexternal_images -noascii_mode -font_size 12pt \
		-nonavigation -numbered_footnotes -nosubdir -rooted -noinfo \
		-norooted -antialias_text -antialias -white -image_type png \
		-scalable_fonts $(NAME).tex
	cat org.css >> $(NAME).css

clean-gif:
	rm -f $(GIFFIGS) $(PLOTS:.plot=.gif)

clean-png:
	rm -f $(PNGFIGS) $(PLOTS:.plot=.png)

clean-eps:
	rm -f $(EPSFIGS) $(PLOTS:.plot=.eps)

clean-pdf:
	rm -f $(PDFFIGS) $(PLOTS:.plot=.pdf)

clean-fig: clean-gif clean-png clean-eps clean-pdf
	rm -f $(PLOTFIGS)

clean: clean-fig clean-dat
	rm -f *.bak $(NAME).ps $(NAME).pdf $(NAME).dvi \
	$(NAME).aux $(NAME).bbl $(NAME).blg $(NAME).glo $(NAME).log $(NAME).pl \
	images.aux images.bbl images.glo images.log images.pl images.tex img*.png \
	index.html internals.pl labels.pl \
	$(NAME).css $(NAME).html WARNINGS

MY_PHONY += clean clean-fig clean-gif clean-png clean-eps clean-pdf clean-dat

EXTRA_DIST += $(NAME).pdf $(NAME).tex $(NAME).bib $(FIGFIGS) $(PLOT_DATS) $(PLOTS) org.css

## DEVELOPMENT
endif

