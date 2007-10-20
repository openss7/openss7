#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.pbm .c .grf .pct .pic .plt .s .web .tex .dvi .imp .ps

.pbm.tex:;	pbmtotex $<

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

all:		report-3.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f report-3.imp report-3.ps report-3.dvi report-3.log \
			report-3.blg _*

grind:		report-3.dvi true
		dvisp $(DFLAGS) report-3

imp:		report-3.imp

preview:	report-3.dvi true
		xdvi report-3.dvi

ps:		report-3.ps

true:;


#############################################################
# report-3
#############################################################

# customization files, et. al., not included below
report-3.dvi:	report-3.vrsn

report-3.vrsn:	report-3.tex text.tex version.sh \
		    figure1.tex figure2.tex figure3.tex figure4.tex figure5.tex
		@./version.sh report-3

biblio:;	

index:;		
