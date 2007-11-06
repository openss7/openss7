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

all:		monterey.dvi update.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f monterey.imp monterey.ps monterey.dvi monterey.log \
			update.imp update.ps update.dvi update.log _*

grind:		monterey.dvi true
		dvisp $(DFLAGS) -Xl monterey
		dvisp $(DFLAGS) -Xl update

imp:		monterey.imp update.imp

preview:	monterey.dvi update.dvi true
		xdvi -l monterey.dvi
		xdvi -l update.dvi

ps:		monterey.ps update.ps

true:;


monterey.dvi:	monterey.tex \
		    figure1.tex figure2.tex figure3a.tex figure3b.tex \
		    figure3c.tex figure3d.tex figure4.tex figure5.tex \
		    figure6.tex figure7.tex

update.dvi:	update.tex
