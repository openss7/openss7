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

all:		practical-osi1.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f practical-osi1.imp practical-osi1.ps practical-osi1.dvi \
			practical-osi1.log _*

grind:		practical-osi1.dvi true
		dvisp $(DFLAGS) -Xl practical-osi1

imp:		practical-osi1.imp

ps:		practical-osi1.ps

preview:	practical-osi1.dvi true
		xdvi -l practical-osi1.dvi

true:;


practical-osi1.dvi:	practical-osi1.tex \
		a-overview.tex \
		a-session.tex \
		a-abstract.tex \
		a-presentation.tex \
		a-ase.tex \
		a-apps.tex \
		    figureA-1.tex  figureA-2.tex  figureA-3.tex  \
		    figureA-4.tex  figureA-5.tex  figureA-6.tex  \
		    figureA-7.tex  figureA-8.tex  figureA-9.tex  \
		    figureA-10.tex  figureA-11.tex  figureA-12.tex  \
		    figureA-13.tex  figureA-14.tex  figureA-15.tex  \
		    figureA-16.tex  figureA-17.tex  figureA-18.tex  \
		    figureA-19.tex  figureA-20.tex  figureA-21.tex  \
		    figureA-22.tex  figureA-23.tex  figureA-24.tex  \
		    figureA-25.tex  figureA-26.tex  figureA-27.tex  \
		    figureA-28.tex  figureA-29.tex  figureA-30.tex  \
		    figureA-31.tex  figureA-32.tex  figureA-33.tex  \
		    figureA-34.tex  figureA-35.tex  figureA-36.tex  \
		    figureA-37.tex  figureA-38.tex  figureA-39.tex  \
		    figureA-40.tex  figureA-41.tex  figureA-42.tex  \
		    figureA-43.tex  figureA-44.tex  figureA-45.tex  \
		    figureA-46.tex  figureA-47.tex  \
		    figureA-49.tex  figureA-50.tex  figureA-51.tex \
		    figureA-52.tex  figureA-53.tex  figureA-54.tex \
		    figureA-55.tex  figureA-56.tex  figureA-57.tex \
		    figureA-58.tex  figureA-59.tex  figureA-60.tex \
		    figureA-61.tex  figureA-62.tex  figureA-63.tex \
		    figureA-64.tex  figureA-65.tex  \
		end-to-end.tex \
		    figureE-1.tex  figureE-2.tex  figureE-3.tex  \
		    figureE-4.tex  figureE-5.tex  figureE-6.tex  \
		    figureE-7.tex  figureE-8.tex  figureE-9.tex  \
		    figureE-10.tex figureE-11.tex figureE-12.tex \
 		    figureE-13.tex figureE-14.tex figureE-15.tex \
 		    figureE-16.tex figureE-17.tex figureE-18.tex \
 		    figureE-19.tex figureE-20.tex \
		transition.tex \
		    figureT-1.tex  figureT-2.tex  figureT-3.tex  \
		    figureT-4.tex  figureT-5.tex  figureT-6.tex  \
						  figureT-9.tex  \
		    figureT-10.tex figureT-11.tex figureT-12.tex \
		    figureT-13.tex figureT-14.tex figureT-15.tex \
		    figureT-16.tex figureT-17.tex figureT-18.tex \
		    figureT-19.tex
