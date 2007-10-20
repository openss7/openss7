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

all:		iso-migrate.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f iso-migrate.imp iso-migrate.ps iso-migrate.dvi \
			iso-migrate.log _*

grind:		iso-migrate.dvi true
		dvisp $(DFLAGS) -Xl iso-migrate

imp:		iso-migrate.imp

preview:	iso-migrate.dvi true
		xdvi -l iso-migrate.dvi

ps:		iso-migrate.ps

true:;


iso-migrate.dvi:	iso-migrate.tex figure1.tex figure2.tex figure3.tex \
		figure4.tex figure5.tex figure6.tex figure7.tex
