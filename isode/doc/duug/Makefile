#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.c .fig .pic .web .ry .bnf .py .tex .dvi .imp .ps

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.fig.pic:;	f2p < $< > $@			

.pic.tex:;	tpic $<

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.ry.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.bnf.tex:;      sh -c 'if tgrind -d grindefs -f -lbnf $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if latex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		duug.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f duug.imp duug.ps duug.dvi duug.log duug.blg

grind:		duug.dvi
		dvisp $(DFLAGS) duug

imp:		duug.imp

preview:	duug.dvi
		xdvi duug.dvi

ps:		duug.ps

true:;


#############################################################
# duug
#############################################################


duug.dvi:	duug.tex \
		figure1.tex figure2.tex figure3.tex
