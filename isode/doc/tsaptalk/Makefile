#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.pic .web .tex .dvi .imp .ps

.pic.tex:;	tpic $<

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if latex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		tsaptalk.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f tsaptalk.imp tsaptalk.ps tsaptalk.dvi tsaptalk.log _*
			
grind:		tsaptalk.dvi true
		dvisp $(DFLAGS) tsaptalk

imp:		tsaptalk.imp

preview:	tsaptalk.dvi true
		xdvi tsaptalk.dvi

ps:		tsaptalk.ps

true:;
