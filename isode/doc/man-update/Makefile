#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.pic .c .h .s .ry .py .bnf .make .web .tex .dvi .imp .ps .pdf

.pic.tex:;	tpic $<

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.h.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.s.tex:;	sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.ry.tex:;	sh -c 'if tgrind -d ../../pepy/grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d ../../pepy/grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.bnf.tex:;      sh -c 'if tgrind -d ../../pepy/grindefs -f -lbnf $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.make.tex:;	sh -c 'if tgrind -f -lsrc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if pslatex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.tex.pdf:;	pdflatex $<

.dvi.imp:;	dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -b $* -z 10 $<; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		update.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f update*.imp update*.ps update*.dvi update*.log \
			update*.blg _*

grind:		update.dvi true
		dvisp $(DFLAGS) update

imp:		update.imp

ps:		update.ps

pdf:		update.pdf

split:		all true
		dvi2ps $(DFLAGS) -r -b update update.dvi

#############################################################
# update
#############################################################

update.dvi:	update.vrsn 

update.vrsn:	update.tex \
		   manual.tex \
		   q-de.tex \
		   q-dm.tex \
		   q-sacl.tex \
		   table-ports.tex  \
		version.sh
		@./version.sh update

#############################################################
# miscellany
#############################################################

preview:	update.dvi true
		xdvi update.dvi

true:;
