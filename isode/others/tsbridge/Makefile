###############################################################################
#   Instructions to Make, for compilation of ISODE Transport Service Bridge
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/tsbridge/RCS/Makefile,v 9.0 1992/06/16 12:48:17 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:48:17  isode
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


###############################################################################
# Generation Rules for program modules
###############################################################################

.c.o:;		$(CC) $(CFLAGS) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LIBES	=	$(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

HFILES	=	
CFILES	=	tsbridge.c


##################################################################
# Here it is...
##################################################################

all:		tsbridge
inst-all:	inst-tsbridge manuals
install:	inst-all clean
lint:		l-tsbridge


##################################################################
# tsbridge
##################################################################

inst-tsbridge:	$(SBINDIR)tsbridge

$(SBINDIR)tsbridge:	xtsbridge
		-cp $@ zxtsbridge
		-rm -f $@
		cp xtsbridge $@
		-@ls -gls $@
		-@echo ""

tsbridge:	xtsbridge
		@echo $@ made.

xtsbridge:	tsbridge.o
		$(LDCC) $(LDFLAGS) -o $@ tsbridge.o $(LIBES) $(LSOCKET)

l-tsbridge:;	$(LINT) $(LFLAGS) tsbridge.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


################################################################
# manual pages
################################################################

MANUALS	=	tsbridge.8c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##################################################################
# clean
##################################################################

clean:;		rm -f *.o x* z* _* core 

grind:;		iprint Makefile
		tgrind -lc $(HFILES) $(CFILES)

true:;
