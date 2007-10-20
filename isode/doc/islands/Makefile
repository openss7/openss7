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

all:		islands.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f islands.imp islands.ps islands.dvi islands.log _*

grind:		islands.dvi true
		dvisp $(DFLAGS) -Xl islands

imp:		islands.imp

preview:	islands.dvi true
		xdvi -l islands.dvi

ps:		islands.ps

true:;


islands.dvi:	islands.tex join.tex mappic.tex migrate.tex osipic.tex \
		problem.tex
