##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.pic .web .tex .dvi .imp .ps

.pic.tex:;	tpic $<

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -l -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -o landscape $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		tsap.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f tsap.imp tsap.ps tsap.dvi tsap.log _*

grind:		tsap.dvi true
		dvisp $(DFLAGS) -Xl tsap

imp:		tsap.imp

preview:	tsap.dvi true
		xdvi -l tsap.dvi

ps:		tsap.ps

true:;
