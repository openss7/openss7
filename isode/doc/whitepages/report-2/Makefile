#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.xb .xwd .c .grf .pct .pic .plt .s .web .tex .dvi .imp .ps

.xb.tex:;	xtbit $<

.xwd.tex:;	xtwd $<

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.grf.tex:;	sh -c 'if graph < $< | plot -TeX > $@; \
			then exit 0; else rm $@; exit 1; fi'

.pct.pic:;	pct2pic $<

.pic.tex:;	tpic $<

.plt.tex:;	sh -c 'if plot -TeX < $< > $@; \
			then exit 0; else rm $@; exit 1; fi'

.s.tex:;	sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if latex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		report-2.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f report-2.imp report-2.ps report-2.dvi report-2.log \
			report-2.blg _*

grind:		report-2.dvi true
		dvisp $(DFLAGS) report-2

imp:		report-2.imp

preview:	report-2.dvi true
		xdvi report-2.dvi

ps:		report-2.ps

true:;


#############################################################
# report-2
#############################################################

# customization files, et. al., not included below
report-2.dvi:	report-2.vrsn

report-2.vrsn:	report-2.tex text.tex report-2.bbl version.sh \
			tableINT-1.tex tableINT-2.tex tableINT-3.tex \
			tableINT-4.tex tableINT-5.tex tableINT-6.tex \
			tableINT-7.tex tableINT-8.tex tableINT-9.tex
		@./version.sh report-2

biblio:;	bibtex report-2

index:;		
