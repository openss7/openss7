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

all:		isode1.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f isode1.imp isode1.ps isode1.dvi isode1.log _*

grind:		isode1.dvi true
		dvisp $(DFLAGS) -Xl isode1

imp:		isode1.imp

preview:	isode1.dvi true
		xdvi -l isode1.dvi

ps:		isode1.ps

isode1.dvi:	isode1.tex \
		    figure1.tex figure2.tex figure3.tex figure4.tex \
		    figure5.tex figure6.tex figure7.tex figure8.tex \
		    figure9.tex figure10.tex figure11.tex figure12.tex \
		    figure13.tex figure14.tex figure15.tex figure16.tex \
		    figure17.tex

true:;
