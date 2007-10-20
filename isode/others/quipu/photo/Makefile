###############################################################################
#   Instructions to Make, for compilation of QUIPU photo programs
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/photo/RCS/Makefile,v 9.0 1992/06/16 12:43:35 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:43:35  isode
# Release 8.0
#
# 
###############################################################################

###############################################################################
#
#				 NOTICE
#
#    Acquisition, use, and distribution of this module and related
#    materials are subject to the restrictions of a license agreement.
#    Consult the Preface in the User's Manual for the full terms of
#    this agreement.
#
###############################################################################

# Select you display type(s)...
# should really go in config/CONFIG.make
DISPLAY = ttyphoto fax2ps
# DISPLAY = sunphoto Xphoto xfax ttyphoto hexphoto t4014 fax2ps jpeg

# UNIX path for the xv binary
XV_LOCATION = /usr/local/bin/

#######################################################################
#    Here we go...
#######################################################################

# libphoto.a now merged with libdsap.a

all:    $(TOPDIR)libdsap.a $(DISPLAY)

lint:	lint-jpeg
	$(LINT) $(LFLAGS) d_main.c tty.c $(TOPDIR)llib-ldsap \
		| grep -v "warning: possible pointer alignment problem"

lint-jpeg:
	$(LINT) $(LFLAGS) jpegphoto.c \
		| grep -v "warning: possible pointer alignment problem"
	$(LINT) $(LFLAGS) jpeg2asn.c \
		| grep -v "warning: possible pointer alignment problem"

pr2pe:  e_main.o encode.o code_word.o 
	$(LDCC) $(LDFLAGS) -o pr2pe e_main.o encode.o code_word.o \
		 -lpixrect $(LIBDSAP) $(LIBISODE) $(LSOCKET)

sunphoto: $(TOPDIR)libdsap.a d_main.o sunview.o
	$(LDCC) $(LDFLAGS) -o sunphoto  \
		d_main.o sunview.o $(LIBDSAP) -lsuntool -lsunwindow \
		-lpixrect $(LIBISODE) $(LSOCKET)

Xphoto: $(TOPDIR)libdsap.a d_main.o winx.o
	$(LDCC) $(LDFLAGS) -o Xphoto \
		d_main.o winx.o -lX11 $(LIBDSAP) $(LIBISODE) $(LSOCKET)

xfax: $(TOPDIR)libdsap.a d_main.o faxx.o
	$(LDCC) $(LDFLAGS) -o xfax \
		d_main.o faxx.o -lX11 $(LIBDSAP) $(LIBISODE) $(LSOCKET)

fax2ps: $(TOPDIR)libdsap.a d_main.o ps.o
	$(LDCC) $(LDFLAGS) -o fax2ps \
		d_main.o ps.o $(LIBDSAP) $(LIBISODE) $(LSOCKET)

t4014: $(TOPDIR)libdsap.a d_main.o t4014.o
	$(LDCC) $(LDFLAGS) -o t4014 \
		d_main.o t4014.o -l4014 -lm $(LIBDSAP) $(LIBISODE) $(LSOCKET)

ttyphoto: $(TOPDIR)libdsap.a d_main.o tty.o
	$(LDCC) $(LDFLAGS) -o ttyphoto \
		d_main.o  tty.o $(LIBDSAP) $(LIBISODE) $(LSOCKET)

hexphoto: hexphoto.o d_main.o
	$(LDCC) $(LDFLAGS) -o hexphoto hexphoto.o d_main.o $(LIBISODE) $(LSOCKET)

jpeg:	xjpeg jpeg2asn

xjpeg:	jpegphoto.o
	$(LDCC) $(LDFLAGS) -o xjpeg jpegphoto.o $(LIBISODE) $(LSOCKET)

jpeg2asn:	jpeg2asn.o
	$(LDCC) $(LDFLAGS) -o jpeg2asn jpeg2asn.o $(LIBISODE) $(LSOCKET)

###############################################################################
# some programs for Jef Poskanzer's pbm package
###############################################################################

PBMSRC  =	/usr/src/pbmplus/
PBMDIR	=	$(PBMSRC)pbm/
PBMINC	=	-I$(PBMSRC) -I$(PBMDIR)
LIBPBM	=	$(PBMDIR)libpbm.a

pbm:		faxtopbm pbmtofax

faxtopbm:	faxtopbm.o $(TOPDIR)libdsap.a $(LIBPBM)
		$(LDCC) $(LDFLAGS) -o $@ faxtopbm.o \
			$(LIBDSAP) $(LIBPBM) $(LIBISODE) $(LSOCKET)

faxtopbm.o:	faxtopbm.c
		$(CC) $(CFLAGS) $(PBMINC) -c faxtopbm.c

pbmtofax:	pbmtofax.o encode.o code_word.o $(TOPDIR)libdsap.a $(LIBPBM)
		$(LDCC) $(LDFLAGS) -o $@ pbmtofax.o encode.o code_word.o \
			$(LIBDSAP) $(LIBPBM) $(LIBISODE) $(LSOCKET)

pbmtofax.o:	pbmtofax.c
		$(CC) $(CFLAGS) $(PBMINC) -c pbmtofax.c


#####################################################################
#    Installation...
#####################################################################

install:  inst-all clean

inst-all:  $(DISPLAY) 
	-mkdir $(SBINDIR)g3fax
	-if [ -f ttyphoto ]; then \
		mv $(SBINDIR)g3fax/ttyphoto zttyphoto; \
		cp ttyphoto $(SBINDIR)g3fax/ttyphoto; fi
	-if [ -f hexphoto ]; then \
		mv $(SBINDIR)g3fax/hexphoto zhexphoto; \
		cp hexphoto $(SBINDIR)g3fax/hexphoto; fi
	-if [ -f sunphoto ]; then \
		mv $(SBINDIR)g3fax/sunphoto zsunphoto; \
		cp sunphoto $(SBINDIR)g3fax/sunphoto; fi
	-if [ -f Xphoto ]; then \
		mv $(SBINDIR)g3fax/Xphoto zXphoto; \
		cp Xphoto $(SBINDIR)g3fax/Xphoto; fi
	-if [ -f xfax ]; then \
		mv $(SBINDIR)g3fax/xfax zxfax; \
		cp xfax $(SBINDIR)g3fax/xfax; fi
	-if [ -f fax2ps ]; then \
		mv $(SBINDIR)g3fax/fax2ps zfax2ps; \
		cp fax2ps $(SBINDIR)g3fax/fax2ps; fi
	-if [ -f t4014 ]; then \
		mv $(SBINDIR)g3fax/t4014 zt4014; \
		cp t4014 $(SBINDIR)g3fax/t4014; fi
	-if [ -f faxtopbm ]; then \
		mv $(SBINDIR)g3fax/faxtopbm zfaxtopbm; \
		cp faxtopbm $(SBINDIR)g3fax/faxtopbm; fi
	-if [ -f pbmtofax ]; then \
		mv $(SBINDIR)g3fax/pbmtofax zpbmtofax; \
		cp pbmtofax $(SBINDIR)g3fax/pbmtofax; fi
	-if [ -f pr2pe ]; then \
		mv $(SBINDIR)g3fax/pr2pe zpr2pe; \
		cp pr2pe $(SBINDIR)g3fax/pr2pe; fi
	-if [ -f jpeg2asn ]; then \
		mv $(SBINDIR)g3fax/jpeg2asn zjpeg2asn; \
		cp jpeg2asn $(SBINDIR)g3fax/jpeg2asn; fi
	-if [ -f xjpeg ]; then \
		mv $(SBINDIR)g3fax/xjpeg zxjpeg; \
		cp xjpeg $(SBINDIR)g3fax/xjpeg; \
		mv $(SBINDIR)g3fax/jpeg.sh zxjpeg.sh; \
		sed -e 's%$$(XV)%$(XV_LOCATION)%' < jpeg.sh > $(SBINDIR)g3fax/jpeg.sh; \
		chmod a+x $(SBINDIR)g3fax/jpeg.sh; fi	
	-if [ -f $(SBINDIR)g3fax/libphoto.a ]; then \
		mv $(SBINDIR)g3fax/libphoto.a zlibphoto.a; fi

clean:
	rm -f _* *.o *.old *% foo* core z* *.a \
		*.orig \
		sunphoto hexphoto ttyphoto \
		pr2pe Xphoto xfax t4014 \
		faxtopbm pbmtofax \
		fax2ps jpeg2asn xjpeg



# depend
code_word.o: ../../../h/quipu/photo.h
d_main.o: ../../../h/config.h
d_main.o: ../../../h/general.h
d_main.o: ../../../h/manifest.h
d_main.o: ../../../h/psap.h
d_main.o: ../../../h/quipu/photo.h
d_main.o: ../../../h/sys.file.h
e_main.o: ../../../h/quipu/photo.h
jpeg2asn.o: ./jpeg.h
encode.o: ../../../h/config.h
encode.o: ../../../h/general.h
encode.o: ../../../h/quipu/photo.h
faxtopbm.o: ../../../h/config.h
faxtopbm.o: ../../../h/general.h
faxtopbm.o: ../../../h/manifest.h
faxtopbm.o: ../../../h/psap.h
faxtopbm.o: ../../../h/sys.file.h
faxx.o: ../../../h/quipu/photo.h
hexphoto.o: ../../../h/config.h
hexphoto.o: ../../../h/general.h
hexphoto.o: ../../../h/logger.h
hexphoto.o: ../../../h/manifest.h
hexphoto.o: ../../../h/psap.h
hexphoto.o: ../../../h/quipu/config.h
hexphoto.o: ../../../h/quipu/photo.h
hexphoto.o: ../../../h/quipu/util.h
jpegphoto.o: ../../../h/config.h
jpegphoto.o: ../../../h/logger.h
jpegphoto.o: ../../../h/manifest.h
jpegphoto.o: ../../../h/tailor.h
jpegphoto.o: ./jpeg.h
pbmtofax.o: ../../../h/config.h
pbmtofax.o: ../../../h/general.h
pbmtofax.o: ../../../h/quipu/photo.h
ps.o: ../../../h/quipu/photo.h
sunview.o: ../../../h/quipu/photo.h
t4014.o: ../../../h/config.h
t4014.o: ../../../h/manifest.h
t4014.o: ../../../h/quipu/photo.h
template.o: ../../../h/quipu/photo.h
tty.o: ../../../h/quipu/photo.h
winx.o: ../../../h/quipu/photo.h
