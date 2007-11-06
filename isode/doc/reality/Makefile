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

all:		reality.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f reality.imp reality.ps reality.dvi reality.log _*

grind:;		reality.dvi true
		dvisp $(DFLAGS) -Xl reality

imp:		reality.imp

preview:	reality.dvi true
		xdvi -l reality.dvi

ps:		reality.ps

true:;


reality.dvi:	reality.tex \
			figure1.tex figure2.tex
