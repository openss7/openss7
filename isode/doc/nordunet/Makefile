##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.b .xb .xwd .c .grf .pct .pic .plt .s .web .tex .dvi .imp .ps .ry .py

.b.tex:;	itbit $<

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

.ry.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -l -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -o landscape -r $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		transition.dvi whitepages.dvi isode.dvi future.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f transition.imp transition.ps transition.dvi \
			transition.log \
			whitepages.imp whitepages.ps whitepages.dvi \
			whitepages.log \
			isode.imp isode.ps isode.dvi isode.log \
			future.imp future.ps future.dvi future.log _*

grind:		transition.dvi whitepages.dvi isode.dvi future.dvi true
		dvisp $(DFLAGS) -Xl transition
		dvisp $(DFLAGS) -Xl whitepages
		dvisp $(DFLAGS) -Xl isode
		dvisp $(DFLAGS) -Xl future

imp:		transition.imp whitepages.imp isode.imp future.imp

ps:		transition.ps whitepages.ps isode.ps future.ps

preview:	transition.dvi whitepages.dvi isode.dvi future.dvi
		xdvi -l transition.dvi
		xdvi -l whitepages.dvi
		xdvi -l isode.dvi
		xdvi -l future.dvi
		
true:;


transition.dvi:	transition.tex \
		    figureT-1.tex  figureT-2.tex  figureT-3.tex  \
		    figureT-4.tex  figureT-5.tex  figureT-6.tex  \
						  figureT-9.tex  \
		    figureT-10.tex figureT-11.tex figureT-12.tex \
		    figureT-13.tex figureT-14.tex figureT-15.tex \
		    figureT-16.tex figureT-17.tex figureT-18.tex \
		    figureT-19.tex figureT-20.tex

whitepages.dvi:	whitepages.tex \
		    figureW-2.tex

isode.dvi:	isode.tex \
		    figureI-1.tex  figureI-2.tex  figureI-7.tex  \
		    figureI-8.tex  figureI-9.tex  figureI-10.tex \
		    figureI-11.tex figureI-12.tex figureT-9.tex  \
		    figureT-15.tex figureT-16.tex figureT-19.tex

future.dvi:	future.tex
