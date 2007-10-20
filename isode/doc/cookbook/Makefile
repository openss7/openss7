##########################################################################
# Instructions to Make, for generation of nice-looking slides using SLiTeX
##########################################################################

.SUFFIXES:	.c .pic .web .ry .py .tex .dvi .imp .ps

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.pic.tex:;	tpic $<

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.ry.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if slitex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -l -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -o landscape $< > $@; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		cookbook.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f cookbook.imp cookbook.ps cookbook.dvi cookbook.log  \
			overview.imp overview.ps overview.dvi overview.log \
			_*

grind:		cookbook.dvi overview.dvi true
		dvisp $(DFLAGS) -Xl cookbook
		dvisp $(DFLAGS)     overview


imp:		cookbook.imp overview.imp

ps:		cookbook.ps overview.ps

preview:	cookbook.dvi overview.dvi true
		xdvi -l cookbook.dvi
		xdvi    overview.dvi

cookbook:	cookbook.ps overview.imp

true:;


cookbook.dvi:	cookbook.tex \
		review.tex \
		    figure1.tex figure2.tex figure3.tex figure4.tex \
		    figure5.tex figure24.tex figure23.tex figure6.tex \
		    figure30.tex \
		model.tex \
		services.tex \
		    figure25.tex figure26.tex figure27.tex figure28.tex \
		    figure29.tex figure7.tex figure8.tex figure9.tex \
		    figure10.tex \
		static.tex \
		    figure9.tex figure11.tex figure12.tex figure13.tex \
		    figure14.tex figure9.tex \
		dynamic.tex \
		    figure10.tex figure21.tex figure32.tex figure22.tex \
		    figure31.tex figure33.tex figure34.tex \
		    figure15.tex figure16.tex figure17.tex \
		    figure18.tex figure19.tex figure20.tex figure10.tex \
		what.tex 

overview.dvi:	overview.tex \
		    cmip-bind.tex cmip-rosy.tex cmip-posy.tex cmip-pepy.tex
		sh -c 'if latex \\nonstopmode\\input overview; \
			  then exit 0; else rm $@; exit 1; fi'

overview.ps:	overview.dvi
		sh -c 'if dvi2ps $(DFLAGS) -r overview.dvi > $@; \
			  then exit 0; else rm $@; exit 1; fi'
