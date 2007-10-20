##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.b .xb .xwd .c .grf .pct .pic .plt .s .web .tex .dvi .imp .ps .fig

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.fig.pic:;      sh -c 'if f2p < $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


.pic.tex:;	tpic $<

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		ifip88-quipu.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f ifip88-quipu.imp ifip88-quipu.ps ifip88-quipu.dvi \
			ifip88-quipu.log _*

grind:		ifip88-quipu.dvi true
		dvisp $(DFLAGS) ifip88-quipu

imp:		ifip88-quipu.imp

preview:	ifip88-quipu.dvi true
		xdvi ifip88-quipu.dvi

ps:		ifip88-quipu.ps

true:;
