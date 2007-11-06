###############################################################################
#   Instructions to Make, for compilation of ISODE idist utility
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/idist/RCS/Makefile,v 9.0 1992/06/16 12:42:00 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:42:00  isode
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

PEPYPATH=	-DPEPYPATH

.c.o:;		$(CC) $(CFLAGS) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LIBES	=	$(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

HFILES	=	ryresponder.h defs.h
CFILES	=	docmd.c expand.c idistd.c idistd2.c install.c lookup.c \
			main.c ops.c ryinitiator.c ryresponder.c support.c
RYFILES	=	idist.ry


##################################################################
# Here it is...
##################################################################

all:		idistd idist
inst-all:	inst-idistd inst-idist manuals
install:	inst-all clean
lint:		l-idistd l-idist


##################################################################
# idistd
##################################################################

inst-idistd:	$(SBINDIR)ros.idist

$(SBINDIR)ros.idist:	idistd
		-cp $@ zros.idist
		-rm -f $@
		cp idistd $@
		-@ls -gls $@
		-@echo ""

IDISTDO=	idistd.o idistd2.o support.o Idist-Rops.o ryresponder.o
IDISTDC=	idistd.c idistd2.c support.c Idist-ops.c  ryresponder.c

idistd:		$(IDISTDO) Idist_tables.o
		$(CC) $(LDFLAGS) -o $@ $(IDISTDO) Idist_tables.o \
			$(LIBES) $(LSOCKET)

idistd.o:	ryresponder.h Idist-ops.h Idist-types.h defs.h

Idist-Rops.o:	Idist-ops.c Idist-ops.h
		$(CC) $(CFLAGS) -DPERFORMER -c Idist-ops.c
		mv Idist-ops.o $@

ryresponder.o:	ryresponder.h ryresponder.c Idist-ops.h
		$(CC) $(CFLAGS) -c ryresponder.c

idistd2.o:	Idist-types.h defs.h idistd2.c

l-idistd:	$(IDISTDC) Idist_tables.c true
		$(LINT) $(LFLAGS) -DPERFORMER $(IDISTDC) \
			Idist_tables.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


##################################################################
# idist
##################################################################

inst-idist:	$(BINDIR)idist

$(BINDIR)idist:	idist
		-cp $@ zidist
		-rm -f $@
		cp idist $@
		-@ls -gls $@
		-@echo ""

IDISTO=		main.o docmd.o expand.o gram.o install.o lookup.o support.o \
		ops.o Idist-Iops.o ryinitiator.o
IDISTC=		main.c docmd.c expand.c gram.c install.c lookup.c support.c \
		ops.c ryinitiator.c

idist:		$(IDISTO) Idist_tables.o
		$(CC) $(LDFLAGS) -o $@ $(IDISTO) Idist_tables.o $(LIBES) $(LSOCKET)

ops.o:		Idist-ops.h Idist-types.h defs.h

Idist-Iops.o:	Idist-ops.c Idist-ops.h
		$(CC) $(CFLAGS) -DINVOKER -c Idist-ops.c
		mv Idist-ops.o $@

main.o:		defs.h 

support.o:	defs.h Idist-ops.h Idist-types.h

docmd.o:	defs.h

expand.o:	defs.h

gram.o:		defs.h

lookup.o:	defs.h

install.o:	defs.h

l-idist:	$(IDISTC) Idist-ops.c Idist_tables.c true
		$(LINT) $(LFLAGS) -DINVOKER $(IDISTC) \
			Idist_tables.c Idist-stubs.c \
			$(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


################################################################
# libidist
################################################################

Idist_tables.o:	Idist_tables.c Idist-types.h

Idist_tables.c Idist-types.h: Idist-asn.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m Idist-asn.py

Idist-asn.py:	idist.ry $(TOPDIR)rosy/xrosy
		$(TOPDIR)rosy/xrosy -m -pepsy -defs -o $@ idist.ry
Idist-ops.c:	idist.ry
Idist-ops.h:	idist.ry
Idist-stubs.c:	idist.ry


################################################################
# manual pages
################################################################

MANUALS	=	idistd.8c idist.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a Idist* IDIST* gram.c idistd idist z* _* core

grind:;		iprint READ-ME Makefile
		tgrind -lc $(HFILES) $(CFILES)
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(RYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
