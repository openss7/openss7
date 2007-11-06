##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.b .xb .xwd .c .grf .pct .pic .plt .s .web .tex .dvi .imp .ps

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

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -l -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -o landscape $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		isode2.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f isode2.imp isode2.ps isode2.dvi isode2.log _*

grind:		isode2.dvi true
		dvisp $(DFLAGS) -Xl isode2

imp:		isode2.imp

preview:	isode2.dvi true
		xdvi -l isode2.dvi

ps:		isode2.ps

isode2.dvi:	isode2.tex \
		    figure1.tex figure2.tex figure3.tex figure4.tex \
		    figure5.tex figure6.tex figure7.tex figure8.tex \
		    figure9.tex figure10.tex figure11.tex figure12.tex \
		    figure13.tex figure14.tex figure16.tex figure17.tex \
		    figure18.tex

true:;
