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

all:		report-1.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f report-1.imp report-1.ps report-1.dvi report-1.log \
			report-1.blg _*

grind:		report-1.dvi true
		dvisp $(DFLAGS) report-1

imp:		report-1.imp

preview:	report-1.dvi true
		xdvi report-1.dvi

ps:		report-1.ps

true:;


#############################################################
# report-1
#############################################################

# customization files, et. al., not included below
report-1.dvi:	report-1.vrsn

report-1.vrsn:	report-1.tex text.tex version.sh
		@./version.sh report-1

biblio:;	

index:;		
