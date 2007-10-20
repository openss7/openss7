###############################################################################
#   Instructions to Make, for compilation of ISODE/SunLink osilookup program
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/osilookup/RCS/Makefile,v 9.0 1992/06/16 12:43:08 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:43:08  isode
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

LIBES	=	
LLIBS   =	


###############################################################################
# Files
###############################################################################

HFILES	=	
CFILES	=	osilookup.c


##################################################################
# Here it is...
##################################################################

all:		osilookup
inst-all:	inst-osilookup
install:	inst-all clean
lint:		l-osilookup


##################################################################
# osilookup
##################################################################

inst-osilookup:	$(SBINDIR)osilookup

$(SBINDIR)osilookup:	xosilookup
		-cp $@ zxosilookup
		-rm -f $@
		cp xosilookup $@
		-@ls -gls $@
		-@echo ""

osilookup:	xosilookup

xosilookup:	osilookup.o
		$(LDCC) $(LDFLAGS) -o $@ osilookup.o $(LIBES) $(LSOCKET)

l-osilookup:;	$(LINT) $(LFLAGS) osilookup.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


##################################################################
# clean
##################################################################

clean:;		rm -f *.o x* z* _* core 

grind:;		iprint Makefile
		tgrind -lc $(HFILES) $(CFILES)

true:;
