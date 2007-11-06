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

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -b $* -z 10 $<; \
			  then exit 0; else rm $@; exit 1; fi'

#############################################################
# Here it is...
#############################################################

all:		administrator.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f administrator.imp administrator*.ps administrator.dvi \
			administrator.log administrator.blg _*

grind:		administrator.dvi true
		dvisp $(DFLAGS) administrator

imp:		administrator.imp

preview:	administrator.dvi true
		xdvi administrator.dvi

ps:		administrator.ps

true:;


#############################################################
# administration
#############################################################

# customization files, et. al., not included below
administrator.dvi:	administrator.vrsn administrator.bbl administrator.ind

administrator.vrsn:	administrator.tex \
		introduction.tex \
		    figureINT-1.tex  figureINT-2.tex  figureINT-3.tex  \
		    figureINT-4.tex  figureINT-5.tex  figureINT-6.tex  \
		    figureINT-7.tex  figureINT-8.tex  figureINT-9.tex  \
		    figureINT-10.tex figureINT-11.tex figureINT-12.tex \
		    figureINT-13.tex figureINT-14.tex \
		    tableINT-1.tex   tableINT-2.tex   tableINT-3.tex   \
		    tableINT-4.tex   tableINT-5.tex   tableINT-6.tex   \
		    tableINT-7.tex   tableINT-8.tex   tableINT-9.tex   \
		    tableINT-1b.tex  \
		configuration.tex \
		    figureCNF-1.tex  figureCNF-2.tex  \
		    tableCNF-1.tex   \
		maintenance.tex \
		version.sh
		@./version.sh administrator

biblio:;	bibtex administrator

index:;		idx2ind -l administrator
