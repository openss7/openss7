# =============================================================================
# 
# @(#) $RCSfile: Makefile,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/14 08:33:14 $
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
# Last Modified $Date: 2005/05/14 08:33:14 $ by $Author: brian $
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
BAS = $(MES:.me=)

all:: $(BAS) $(GT) # $(EPS) $(GIF) $(HT) $(GI)

$(BAS) : % : %.txt %.ps %.pdf ;

pics:: $(PIC)
gifs:: $(GIF) # $(GI)
ps:: $(EPS)
html:: $(HTML) $(GIF)

$(TXT) : %.txt : %.me ../idrefs ../draft.me; groff -R -s -p -t -e -Tascii -me $< | ./../fixup > $@
$(PSS) : %.ps  : %.me ../idrefs ../draft.me; groff -R -s -p -t -e -Tps    -me $< > $@
$(PDF) : %.pdf : %.ps ../idrefs ../draft.me; ps2pdf $< $@

$(PIC) : %.pic : %.fig ; fig2dev -L pic -p allps $< $@
$(GIF) : %.gif : %.fig ; fig2dev -L gif $< $@
$(EPS) : %.eps : %.fig ; fig2dev -L ps  $< $@

$(GI)  : %.gif : %.ag ; applix -quiet -add -call gifexport $< $@
$(PN)  : %.png : %.ag ; applix -quiet -add -call pngexport $< $@
$(HT)  : %_001.html : %.aw ; ../aw2h $<

txt:: $(TXT)

clean::
	rm -f *.bak $(TXT) $(PSS) $(PDF) $(PIC) $(GIF) $(EPS) $(GI) $(HT) \
	$(AW:%.aw=%_*.html) \
	$(AW:%.aw=%_object*.ag)

realclean::
	rm -f *.bak $(TXT) $(PSS) $(PDF) $(PIC) $(GIF) $(EPS) $(GI) $(HT) \
	$(AW:%.aw=%_*.html) \
	$(AW:%.aw=%_object*.ag) .depend

.depend: $(MES)
	@grep -H '^\.so' $(MES) | sed -e's/\.so//' | sed 's/\.me/.ps/' > .depend
	@grep -H '^\.so' $(MES) | sed -e's/\.so//' | sed 's/\.me/.txt/' >> .depend

%::

-include .depend
