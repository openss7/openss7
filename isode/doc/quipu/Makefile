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

all:            quipu.dvi

inst-all:;

install:        inst-all clean

lint:;

clean:;		rm -f quipu.dvi quipu.blg quipu.log quipu.ps quipu.imp _*

grind:		quipu.dvi
		dvisp $(DFLAGS) quipu

imp:		quipu.imp

preview:	quipu.dvi
		xdvi quipu.dvi

ps:		quipu.ps 

true:;


#############################################################
# quipu
#############################################################

# customization files, et. al., not included below
quipu.dvi:    quipu.vrsn quipu.bbl asn-summary.py 

ASN = asn-start.py na.py acl.py  schema.py edb.py  \
	reliableros.py getedb.py asn-end.py

asn-summary.py: $(ASN)
		cat $(ASN) > $@


quipu.vrsn:   quipu.tex \
		bits.tex \
		introduction.tex \
		design.tex \
		  acl.tex schema.tex \
		nameservice.tex \
		distribution.tex \
		  edb.tex na.tex \
		  ns.tex \
		update.tex \
		  reliableros.tex getedb.tex \
		management.tex \
		implementation.tex \
		appendix-a.tex \
		  asn-summary.tex \
		version.sh
		@./version.sh quipu

index:;         idx2ind -l quipu
