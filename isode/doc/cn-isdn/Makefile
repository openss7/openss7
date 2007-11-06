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

all:		cn-isdn.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f cn-isdn.imp cn-isdn.ps cn-isdn.dvi cn-isdn.log \
			cn-isdn.blg biography.imp biography.ps biography.dvi \
			biography.log biography.blg _*

grind:		cn-isdn.dvi biography.dvi
		dvisp $(DFLAGS) cn-isdn
		dvisp $(DFLAGS) biography

imp:		cn-isdn.imp biography.imp

preview:	cn-isdn.dvi biography.dvi
		xdvi cn-isdn.dvi
		xdvi biography.dvi

ps:		cn-isdn.ps biography.ps

true:;


#############################################################
# cn-isdn
#############################################################

# customization files, et. al., not included below
cn-isdn.dvi:	cn-isdn.vrsn

cn-isdn.vrsn:	cn-isdn.tex \
		section1.tex \
		section2.tex figure2-1.tex \
		section3.tex figure3-1.tex table3-1.tex table3-2.tex \
		section4.tex \
		section5.tex \
		section6.tex \
		appendix1.tex appendix2.tex \
		cn-isdn.bbl version.sh
		@: ./version.sh cn-isdn

biblio:;	bibtex cn-isdn

index:;		idx2ind -l cn-isdn
