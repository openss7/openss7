# =============================================================================
# 
# @(#) $Id: Makefile,v 0.9.2.1 2004/02/29 19:05:39 brian Exp $
# 
# -----------------------------------------------------------------------------
# 
# Copyright (C) 1997-2001  Brian Bidulock <bidulock@dallas.net>
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
# Last Modified $Date: 2004/02/29 19:05:39 $ by $Author: brian $
#
# =============================================================================

MANS1 = $(wildcard man1/*.1)
TXTS1 = $(MANS1:.1=.txt)
PSSS1 = $(MANS1:.1=.ps)
PDFS1 = $(MANS1:.1=.pdf)
HTML1 = $(MANS1:.1=.html)
GZIP1 = $(MANS1:.1=.1.gz)

MANS2 = $(wildcard man2/*.2)
TXTS2 = $(MANS2:.2=.txt)
PSSS2 = $(MANS2:.2=.ps)
PDFS2 = $(MANS2:.2=.pdf)
HTML2 = $(MANS2:.2=.html)
GZIP2 = $(MANS2:.2=.2.gz)

MANS3 = $(wildcard man3/*.3)
TXTS3 = $(MANS3:.3=.txt)
PSSS3 = $(MANS3:.3=.ps)
PDFS3 = $(MANS3:.3=.pdf)
HTML3 = $(MANS3:.3=.html)
GZIP3 = $(MANS3:.3=.3.gz)

MANS4 = $(wildcard man4/*.4)
TXTS4 = $(MANS4:.4=.txt)
PSSS4 = $(MANS4:.4=.ps)
PDFS4 = $(MANS4:.4=.pdf)
HTML4 = $(MANS4:.4=.html)
GZIP4 = $(MANS4:.4=.4.gz)

MANS5 = $(wildcard man5/*.5)
TXTS5 = $(MANS5:.5=.txt)
PSSS5 = $(MANS5:.5=.ps)
PDFS5 = $(MANS5:.5=.pdf)
HTML5 = $(MANS5:.5=.html)
GZIP5 = $(MANS5:.5=.5.gz)

MANS6 = $(wildcard man6/*.6)
TXTS6 = $(MANS6:.6=.txt)
PSSS6 = $(MANS6:.6=.ps)
PDFS6 = $(MANS6:.6=.pdf)
HTML6 = $(MANS6:.6=.html)
GZIP6 = $(MANS6:.6=.6.gz)

MANS7 = $(wildcard man7/*.7)
TXTS7 = $(MANS7:.7=.txt)
PSSS7 = $(MANS7:.7=.ps)
PDFS7 = $(MANS7:.7=.pdf)
HTML7 = $(MANS7:.7=.html)
GZIP7 = $(MANS7:.7=.7.gz)

MANS8 = $(wildcard man8/*.8)
TXTS8 = $(MANS8:.8=.txt)
PSSS8 = $(MANS8:.8=.ps)
PDFS8 = $(MANS8:.8=.pdf)
HTML8 = $(MANS8:.8=.html)
GZIP8 = $(MANS8:.8=.8.gz)

MANS9 = $(wildcard man9/*.9)
TXTS9 = $(MANS9:.9=.txt)
PSSS9 = $(MANS9:.9=.ps)
PDFS9 = $(MANS9:.9=.pdf)
HTML9 = $(MANS9:.9=.html)
GZIP9 = $(MANS9:.9=.9.gz)

MANS = $(MANS1) $(MANS2) $(MANS3) $(MANS4) $(MANS5) $(MANS6) $(MANS7) $(MANS8) $(MANS9)
TXTS = $(TXTS1) $(TXTS2) $(TXTS3) $(TXTS4) $(TXTS5) $(TXTS6) $(TXTS7) $(TXTS8) $(TXTS9)
PSSS = $(PSSS1) $(PSSS2) $(PSSS3) $(PSSS4) $(PSSS5) $(PSSS6) $(PSSS7) $(PSSS8) $(PSSS9)
PDFS = $(PDFS1) $(PDFS2) $(PDFS3) $(PDFS4) $(PDFS5) $(PDFS6) $(PDFS7) $(PDFS8) $(PDFS9)
HTML = $(HTML1) $(HTML2) $(HTML3) $(HTML4) $(HTML5) $(HTML6) $(HTML7) $(HTML8) $(HTML9)
GZIP = $(GZIP1) $(GZIP2) $(GZIP3) $(GZIP4) $(GZIP5) $(GZIP6) $(GZIP7) $(GZIP8) $(GZIP9)

all:: $(TXTS) $(PSSS) $(PDFS) $(HTML) $(GZIP)

ps:: $(PSSS)
pdf:: $(PDFS)
txt:: $(TXTS)
html:: $(HTML)
gzip:: $(GZIP)

$(TXTS1) : %.txt  : %.1 ; man ./$< > $@
$(PSSS1) : %.ps   : %.1 ; man -t ./$< > $@
$(HTML1) : %.html : %.1 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS2) : %.txt  : %.2 ; man ./$< > $@
$(PSSS2) : %.ps   : %.2 ; man -t ./$< > $@
$(HTML2) : %.html : %.2 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS3) : %.txt  : %.3 ; man ./$< > $@
$(PSSS3) : %.ps   : %.3 ; man -t ./$< > $@
$(HTML3) : %.html : %.3 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS4) : %.txt  : %.4 ; man ./$< > $@
$(PSSS4) : %.ps   : %.4 ; man -t ./$< > $@
$(HTML4) : %.html : %.4 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS5) : %.txt  : %.5 ; man ./$< > $@
$(PSSS5) : %.ps   : %.5 ; man -t ./$< > $@
$(HTML5) : %.html : %.5 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS6) : %.txt  : %.6 ; man ./$< > $@
$(PSSS6) : %.ps   : %.6 ; man -t ./$< > $@
$(HTML6) : %.html : %.6 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS7) : %.txt  : %.7 ; man ./$< > $@
$(PSSS7) : %.ps   : %.7 ; man -t ./$< > $@
$(HTML7) : %.html : %.7 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS8) : %.txt  : %.8 ; man ./$< > $@
$(PSSS8) : %.ps   : %.8 ; man -t ./$< > $@
$(HTML8) : %.html : %.8 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@
$(TXTS9) : %.txt  : %.9 ; man ./$< > $@
$(PSSS9) : %.ps   : %.9 ; man -t ./$< > $@
$(HTML9) : %.html : %.9 ; cat `man -w -c $<` | ( cd $(dir $<) ; man2html ) > $@

$(PDFS) : %.pdf : %.ps ; ps2pdf $< $@
$(GZIP) : %.gz : % ; gzip -c $< > $@

clean::
	rm -f $(TXTS1) $(PSSS1) $(PDFS1) $(HTML1) $(GZIP1)
	rm -f $(TXTS2) $(PSSS2) $(PDFS2) $(HTML2) $(GZIP2)
	rm -f $(TXTS3) $(PSSS3) $(PDFS3) $(HTML3) $(GZIP3)
	rm -f $(TXTS4) $(PSSS4) $(PDFS4) $(HTML4) $(GZIP4)
	rm -f $(TXTS5) $(PSSS5) $(PDFS5) $(HTML5) $(GZIP5)
	rm -f $(TXTS6) $(PSSS6) $(PDFS6) $(HTML6) $(GZIP6)
	rm -f $(TXTS7) $(PSSS7) $(PDFS7) $(HTML7) $(GZIP7)
	rm -f $(TXTS8) $(PSSS8) $(PDFS8) $(HTML8) $(GZIP8)
	rm -f $(TXTS9) $(PSSS9) $(PDFS9) $(HTML9) $(GZIP9)

realclean:: clean

%::
