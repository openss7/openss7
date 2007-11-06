##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.pbm .ry .py .c .pic .s .web .tex .dvi .imp .ps

.pbm.tex:;	pbmtotex $<

.ry.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.pic.tex:;	tpic $<

.s.tex:;	sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -l -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -o landscape -r -b $* -z 20 $<; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		internals2.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f internals2.imp internals2-*.ps internals2.dvi \
			internals2.log _*

grind:		internals2.dvi true
		dvisp $(DFLAGS) -Xl internals2

imp:		internals2.imp

ps:		internals2.ps

preview:	internals2.dvi true
		xdvi -l internals2.dvi

internals2.ps:	internals2.dvi

true:;

internals2.dvi:	internals2.tex \
		history.tex \
		    figureH-1.tex  figureH-2.tex  figureH-3.tex  \
		    figureH-4.tex  figureH-5.tex  figureH-6.tex  \
		    figureH-7.tex  figureH-8.tex  figureH-9.tex  \
		    figureH-10.tex figureH-11.tex figureH-12.tex \
		    figureH-13.tex figureH-14.tex \
		    isode.tex \
		lower.tex \
		    figureL-1.tex  figureL-2.tex  figureL-3.tex  \
		    figureL-4.tex  \
		upper.tex \
		    figureU-1.tex  \
		building.tex \
		    figureB-1.tex  figureB-2.tex  figureB-3.tex  \
		    figureB-4.tex  figureB-5.tex  figureB-6.tex  \
		    figureB-7.tex  figureB-8.tex  figureB-9.tex  \
		    figureB-10.tex figureB-11.tex figureB-12.tex \
		    figureB-13.tex figureB-14.tex figureB-15.tex \
		    figureB-16.tex figureB-17.tex figureB-18.tex \
		    figureB-19.tex figureB-20.tex figureB-23.tex \
		snmp.tex \
		    figureS-1.tex  figureS-2.tex  figureS-3.tex  \
		    figureS-4.tex  figureS-5.tex  figureS-6.tex  \
		    figureS-7.tex  figureS-8.tex  figureS-9.tex  \
		    figureS-10.tex figureS-11.tex figureS-12.tex \
		    figureS-13.tex figureS-14.tex figureS-15.tex \
		    figureS-16.tex figureS-17.tex figureS-18.tex \
		    figureS-19.tex figureS-20.tex figureS-21.tex \
		    figureS-22.tex figureS-23.tex figureS-24.tex \
		    figureS-25.tex figureS-26.tex figureS-27.tex \
		    figureS-28.tex figureS-29.tex figureS-30.tex \
		directory.tex \
		    figureD-1.tex  figureD-2.tex  figureD-3.tex  \
		    figureD-4.tex  figureD-5.tex  figureD-6.tex \
		    figureD-7.tex  figureD-8.tex  figureD-9.tex \
		    figureD-10.tex figureD-11.tex figureD-12.tex \
		    figureD-13.tex  \
		others.tex
