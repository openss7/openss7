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

all:		map-migrate.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f map-migrate.imp map-migrate.ps map-migrate.dvi \
			map-migrate.log _*

grind:		map-migrate.dvi true
		dvisp $(DFLAGS) -Xl map-migrate

imp:		map-migrate.imp

preview:	map-migrate.dvi true
		xdvi -l map-migrate.dvi

ps:		map-migrate.ps

true:;
