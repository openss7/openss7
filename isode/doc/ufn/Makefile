#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:      .pic .c .h .s .ry .py .make .web .tex .dvi .imp .ps .bnf

.pic.tex:;      tpic $<

.c.tex:;        sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.h.tex:;        sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.s.tex:;        sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.ry.tex:;       sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;       sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.bnf.tex:;      sh -c 'if tgrind -d grindefs -f -lbnf $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.make.tex:;     sh -c 'if tgrind -f -lsrc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;      sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;      sh -c 'if latex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;      dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;       sh -c 'if dvi2ps $(DFLAGS) -r $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:            ufn.dvi

inst-all:;

install:        inst-all clean

lint:;

clean:;		rm -f ufn.dvi ufn.blg ufn.log ufn.ps ufn.imp _*

grind:		ufn.dvi
		dvisp $(DFLAGS) ufn

imp:		ufn.imp

preview:	ufn.dvi
		xdvi ufn.dvi

ps:		ufn.ps 

true:;


#############################################################
# ufn
#############################################################

ufn.dvi:	ufn.tex algo.tex

biblio:;	bibtex ufn

