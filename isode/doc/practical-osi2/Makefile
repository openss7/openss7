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

all:		practical-osi2.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f practical-osi2.imp practical-osi2.ps practical-osi2.dvi \
			practical-osi2.log _*

grind:		practical-osi2.dvi true
		dvisp $(DFLAGS) -Xl practical-osi2

imp:		practical-osi2.imp

ps:		practical-osi2.ps

preview:	practical-osi2.dvi true
		xdvi -l practical-osi2.dvi

true:;

practical-osi2.dvi:	practical-osi2.tex \
		    figureP-1.tex \
		end-to-end.tex \
		    figureE-1.tex  figureE-2.tex  figureE-3.tex  \
		    figureE-4.tex  figureE-5.tex  figureE-6.tex  \
		    figureE-7.tex  figureE-8.tex  figureE-9.tex  \
		    figureE-10.tex figureE-11.tex figureE-12.tex \
 		    figureE-13.tex figureE-14.tex figureE-15.tex \
 		    figureE-16.tex figureE-17.tex figureE-18.tex \
 		    figureE-19.tex figureE-20.tex \
		application.tex \
		    figureA-1.tex  figureA-2.tex  figureA-3.tex  \
		    figureA-4.tex  figureA-5.tex  figureA-6.tex  \
		    figureA-7.tex  figureA-8.tex  \
		building.tex \
		    figureB-1.tex  figureB-2.tex  figureB-3.tex  \
		    figureB-4.tex  figureB-5.tex  figureB-6.tex  \
		    figureB-7.tex  figureB-8.tex  figureB-9.tex  \
		    figureB-10.tex figureB-11.tex figureB-12.tex \
		    figureB-13.tex figureB-14.tex figureB-15.tex \
		    figureB-16.tex figureB-17.tex figureB-18.tex \
		    figureB-19.tex figureB-20.tex figureB-21.tex \
		    figureB-22.tex figureB-23.tex \
		transition.tex \
		    figureT-1.tex  figureT-2.tex  figureT-3.tex  \
		    figureT-4.tex  figureT-5.tex  figureT-6.tex  \
						  figureT-9.tex  \
		    figureT-10.tex figureT-11.tex figureT-12.tex \
		    figureT-13.tex figureT-14.tex figureT-15.tex \
		    figureT-16.tex figureT-17.tex figureT-18.tex \
		    figureT-19.tex \
		isode.tex
