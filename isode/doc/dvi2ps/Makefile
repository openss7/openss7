.c.o:;		$(CC) $(CFLAGS) -c $*.c


# The following file and directory specifications may need changing at
# your site:

#
# where are the bitmaps stored?
#
#FONTAREA=/usr/local/tex/lib/fonts/pxl300
FONTAREA=/usr/local/tex/lib/fonts/gf300

#
# where is the header file to be found?
#
HDRFILE=/usr/local/tex/lib/tex.ps

# By default we will use GF format files.  If you want to use PXL
# format instead, then include a -DUSEPXL below.  Actually, if this
# is the case, the routines in gf.c are not needed
#
OPTIONS	=	-DFONTAREA=\"${FONTAREA}\" \
		-DHDRFILE=\"${HDRFILE}\" \
		-DTPIC -DTWG -DMTR # -DUSEPXL


################################################################
# Here it is...
################################################################

all:		dvi2ps
inst-all:	inst-dvi2ps inst-tex manuals
install:	inst-all clean
lint:		l-dvi2ps


################################################################
# dvi2ps
################################################################

inst-dvi2ps:	$(BINDIR)dvi2ps

$(BINDIR)dvi2ps:	xdvi2ps
		-cp $@ zdvi2ps
		-rm -f $@
		cp xdvi2ps $@
		-@ls -gls $@
		-@echo ""

dvi2ps:		xdvi2ps

xdvi2ps:	dvi2ps.o findfile.o drawPS.o gf.o
		$(LDCC) $(LDFLAGS) -o $@ dvi2ps.o findfile.o drawPS.o gf.o \
			$(LSOCKET)

l-dvi2ps:;	$(LINT) $(LFLAGS) dvi2ps.c findfile.c drawPS.c \
			| grep -v "warning: possible pointer alignment problem"

dvi2ps.o:	dvi2ps.c dvi2ps.h findfile.h
drawPS.o:	drawPS.c dvi2ps.h


################################################################
# tex
################################################################

inst-tex:	$(HDRFILE)

$(HDRFILE):	tex.ps
		-cp $@ ztex.ps
		-rm -f $@
		cp tex.ps $@
		-@ls -gls $@
		-@echo ""


################################################################
# manual pages
################################################################

MANUALS	=	dvi2ps.1l

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o x* z* core

grind:		iprint Makefile
		tgrind -lc dvi2ps.c drawPS.c gf.c findfile.c
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

imp:;

ps:;

preview:;

true:;
