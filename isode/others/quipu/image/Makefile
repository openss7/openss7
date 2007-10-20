###############################################################################
#   Instructions to Make, for compilation of ISODE image programs
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/image/RCS/Makefile,v 9.0 1992/06/16 12:43:24 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:43:24  isode
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


.c.o:;		$(CC) $(CFLAGS) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LLIBS   =	$(TOPDIR)llib-ldsap $(TOPDIR)llib-lisode # -lX11


###############################################################################
# Files
###############################################################################

HFILES	=	imagesbr.h
CFILES	=	xface.c xwho.c imagesbr.c


##################################################################
# Here it is...
##################################################################

all:		xface xwho
inst-all:	inst-xface inst-xwho manuals
install:	inst-all clean
lint:		l-xface l-xwho


##################################################################
# xface
##################################################################

inst-xface:	$(BINDIR)xface

$(BINDIR)xface:	xface
		-cp $@ zxface
		-rm -f $@
		cp xface $@
		-@ls -gls $@
		-@echo ""

xface:		xface.o imagesbr.o \
			$(TOPDIR)libisode.a $(TOPDIR)libdsap.a
		$(LDCC) $(LDFLAGS) -o $@ xface.o imagesbr.o \
			$(LIBDSAP) $(LIBISODE) $(LSOCKET) -lX11 $(LIBGDBM)

l-xface:;	$(LINT) $(LFLAGS) xface.c imagesbr.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

xface.o:	imagesbr.h
imagesbr.o:	imagesbr.h


##################################################################
# xwho
##################################################################

inst-xwho:	$(BINDIR)xwho

$(BINDIR)xwho:	xwho
		-cp $@ zxwho
		-rm -f $@
		cp xwho $@
		-@ls -gls $@
		-@echo ""

xwho:		xwho.o imagesbr.o \
			$(TOPDIR)libisode.a $(TOPDIR)libdsap.a
		$(LDCC) $(LDFLAGS) -o $@ xwho.o imagesbr.o \
			$(LIBDSAP) $(LIBISODE) $(LSOCKET) -lX11 $(LIBGDBM)

l-xwho:;	$(LINT) $(LFLAGS) xwho.c imagesbr.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

xwho.o:		imagesbr.h


################################################################
# manual pages
################################################################

MANUALS	=	xface.1c xwho.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a xface xwho z* _* core

grind:;		iprint READ-ME Makefile
		tgrind -lc $(HFILES) $(CFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
