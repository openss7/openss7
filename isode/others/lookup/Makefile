###############################################################################
#   Instructions to Make, for compilation of ISODE password lookup demo
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/lookup/RCS/Makefile,v 9.0 1992/06/16 12:42:23 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:42:23  isode
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
# Options
###############################################################################

BINDIR	=	/usr/new/


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

HFILES	=	ryinitiator.h ryresponder.h
CFILES	=	lookupd.c lookup.c ryinitiator.c ryresponder.c
RYFILES	=	lookup.ry


##################################################################
# Here it is...
##################################################################

all:		lookupd lookup
inst-all:	inst-lookupd inst-lookup manuals
install:	inst-all clean
lint:		l-lookupd l-lookup


##################################################################
# lookupd
##################################################################

inst-lookupd:	$(SBINDIR)ros.lookup

$(SBINDIR)ros.lookup:	lookupd
		-cp $@ zros.lookup
		-rm -f $@
		cp lookupd $@
		-@ls -gls $@
		-@echo ""

lookupd:	lookupd.o PasswordLookup-Rops.o ryresponder.o \
		PasswordLookup_tables.o
		$(LDCC) $(LDFLAGS) -o $@ lookupd.o \
			PasswordLookup-Rops.o ryresponder.o \
			PasswordLookup_tables.o $(LIBES) $(LSOCKET)

l-lookupd:	PasswordLookup-ops.c PasswordLookup_tables.c true
		$(LINT) $(LFLAGS) -DPERFORMER lookupd.c \
			PasswordLookup-ops.c ryresponder.c \
			PasswordLookup_tables.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

lookupd.o:	ryresponder.h PasswordLookup-ops.h \
			PasswordLookup-types.h

PasswordLookup-Rops.o:	PasswordLookup-ops.c PasswordLookup-ops.h
		$(CC) $(CFLAGS) -DPERFORMER -c PasswordLookup-ops.c
		mv PasswordLookup-ops.o $@

ryresponder.o:	ryresponder.h


##################################################################
# lookup
##################################################################

inst-lookup:	$(BINDIR)lookup

$(BINDIR)lookup:	lookup
		-cp $@ zlookup
		-rm -f $@
		cp lookup $@
		-@ls -gls $@
		-@echo ""

lookup:		lookup.o PasswordLookup-Iops.o ryinitiator.o \
			PasswordLookup_tables.o
		$(LDCC) $(LDFLAGS) -o $@ lookup.o PasswordLookup-Iops.o \
			ryinitiator.o PasswordLookup_tables.o $(LIBES) \
			$(LSOCKET)

l-lookup:	PasswordLookup-ops.c PasswordLookup_tables.c true
		$(LINT) $(LFLAGS) -DINVOKER lookup.c PasswordLookup-ops.c \
			ryinitiator.c PasswordLookup-stubs.c \
			PasswordLookup-tables.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

lookup.o:	ryinitiator.h PasswordLookup-ops.h PasswordLookup-types.h

PasswordLookup-Iops.o:	PasswordLookup-ops.c PasswordLookup-ops.h
		$(CC) $(CFLAGS) -DINVOKER -c PasswordLookup-ops.c
		mv PasswordLookup-ops.o $@

ryinitiator.o:	ryinitiator.h


################################################################
# liblookup
################################################################

PasswordLookup_tables.o: PasswordLookup_tables.c PasswordLookup-types.h

PasswordLookup_tables.c PasswordLookup-types.h:	PasswordLookup-asn.py \
			$(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m PasswordLookup-asn.py

PasswordLookup-asn.py:	lookup.ry $(TOPDIR)rosy/xrosy
		$(TOPDIR)rosy/xrosy -m -pepsy -o $@ lookup.ry
PasswordLookup-ops.c:	lookup.ry
PasswordLookup-ops.h:	lookup.ry
PasswordLookup-stubs.c:	lookup.ry


################################################################
# manual pages
################################################################

MANUALS	=	lookupd.8c lookup.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a PasswordLookup* lookupd lookup z* _* core

grind:;		iprint READ-ME Makefile
		tgrind -lc $(HFILES) $(CFILES)
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(RYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
