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

all:		pilot.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f pilot.imp pilot.ps pilot.dvi \
			pilot.log _*

grind:		pilot.dvi true
		dvisp $(DFLAGS) -Xl pilot

imp:		pilot.imp

ps:		pilot.ps

preview:	pilot.dvi
		xdvi -l pilot.dvi
		
true:;


pilot.dvi:	pilot.tex \
		    figureW-2.tex
