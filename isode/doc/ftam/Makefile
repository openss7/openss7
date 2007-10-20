##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.b .xb .c .grf .pic .plt .s .web .py .tex .dvi .imp .ps

.b.tex:;	tbit $<

.xb.tex:;	xtbit $<

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.grf.tex:;	sh -c 'if graph < $< | plot -TeX > $@; \
			then exit 0; else rm $@; exit 1; fi'

.pic.tex:;	tpic $<

.plt.tex:;	sh -c 'if plot -TeX < $< > $@; \
			then exit 0; else rm $@; exit 1; fi'

.s.tex:;	sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -l -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -o landscape $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		ftam.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f ftam.imp ftam.ps ftam.dvi ftam.log _*

grind:		ftam.dvi true
		dvisp $(DFLAGS) -Xl ftam

imp:		ftam.imp

preview:	ftam.dvi true
		xdvi -l ftam.dvi

ps:		ftam.ps

true:;


ftam.dvi:	ftam.tex \
		    filestore.tex \
			figure1.tex figure2.tex figure3.tex figure4.tex \
			figure4a.tex figure4b.tex figure4c.tex \
		    service.tex \
			figure5.tex figure6.tex \
		    protocol.tex \
			figure7.tex \
		    responder.tex \
			figure8.tex figure9.tex \
		    initiator.tex \
		    status.tex
