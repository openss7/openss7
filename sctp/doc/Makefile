# =============================================================================
# 
# @(#) $RCSfile: Makefile,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2002/06/01 20:13:36 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2002/06/01 20:13:36 $ by $Author: brian $
#
# =============================================================================

AW = $(wildcard *.aw)
AG = $(wildcard *.ag)
GI = $(AG:.ag=.gif)
HT = $(AW:.aw=_001.html)

FIG = $(wildcard *.fig)
MES = $(wildcard *.me)
TXT = $(MES:.me=.txt)
PSS = $(MES:.me=.ps)
PDF = $(MES:.me=.pdf)
PIC = $(FIG:.fig=.pic)
GIF = $(FIG:.fig=.gif)
EPS = $(FIG:.fig=.eps)

all:: $(PIC) $(TXT) $(PSS) $(PDF) $(HT) $(EPS) $(GIF) $(GI)

gifs:: $(GI) $(GIF)
ps:: $(PSS) $(EPS)
html:: $(HTML) $(GIF)
txt:: $(TXT)

$(TXT) : %.txt : %.me idrefs ; groff -R -s -p -t -e -Tascii -me $< | ./fixup > $@
$(PSS) : %.ps  : %.me idrefs ; groff -R -s -p -t -e -Tps    -me $< > $@
$(PDF) : %.pdf : %.ps ; ps2pdf $< $@

$(PIC) : %.pic : %.fig ; fig2dev -L pic -p allps $< $@
$(GIF) : %.gif : %.fig ; fig2dev -L gif $< $@
$(EPS) : %.eps : %.fig ; fig2dev -L ps  $< $@

$(GI)  : %.gif : %.ag ; applix -quiet -add -call gifexport $< $@
$(PN)  : %.png : %.ag ; applix -quiet -add -call pngexport $< $@
$(HT)  : %_001.html : %.aw ; ../aw2h $<


clean::
	rm -f *.bak $(TXT) $(PSS) $(PDF) $(PIC) $(GIF) $(EPS) $(GI) $(HT) \
	$(AW:%.aw=%_*.html) \
	$(AW:%.aw=%_object*.ag)

%::
	
figs = $(wildcard datamodel.*.fig)
pics = $(figs:%.fig=%.pic)

#datamodel.ps: $(pics)
