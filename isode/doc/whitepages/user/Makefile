#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.xb .xwd .c .grf .pct .pic .plt .s .web .tex .dvi .imp .ps

.xb.tex:;	xtbit $<

.xwd.tex:;	xtwd $<

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.grf.tex:;	sh -c 'if graph < $< | plot -TeX > $@; \
			then exit 0; else rm $@; exit 1; fi'

.pct.pic:;	pct2pic $<

.pic.tex:;	tpic $<

.plt.tex:;	sh -c 'if plot -TeX < $< > $@; \
			then exit 0; else rm $@; exit 1; fi'

.s.tex:;	sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if latex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -b $* -z 10 $<; \
			  then exit 0; else rm $@; exit 1; fi'

#############################################################
# Here it is...
#############################################################

all:		user.dvi quick.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f user.imp user*.ps user.dvi user.log \
			quick.imp quick*.ps quick.dvi quick.log _*

grind:		user.dvi quick.dvi true
		dvisp $(DFLAGS) user quick

imp:		user.imp quick.imp

preview:	user.dvi quick.dvi true
		xdvi user.dvi
		xdvi quick.dvi

ps:		user.ps quick.ps

true:;


#############################################################
# user
#############################################################

user.dvi:	user.vrsn

user.vrsn:	user.tex \
		introduction.tex \
		    dir-intro.tex \
		    wp-intro.tex \
		fred.tex \
		dish.tex \
		others.tex \
		attributes.tex \
		version.sh
		@./version.sh user

biblio:;

index:;


#############################################################
# quick
#############################################################

quick.dvi:	quick.tex
