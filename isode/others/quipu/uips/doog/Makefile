###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/Makefile,v 9.0 1992/06/16 12:45:18 isode Rel $
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:45:18  isode
# Release 8.0
#
# 
###############################################################################

###############################################################################
# Generation Rules for program modules
###############################################################################

.c.o:;	$(CC) $(CFLAGS) $(INCQUERY) -c $*.c

###############################################################################
# Programs and Libraries
###############################################################################

QLIB	=	query/libQuery.a

ISOLIBS	=	$(TOPDIR)libdsap.a $(TOPDIR)libisode.a

LIBES	=	$(QLIB) $(ISOLIBS)

LLIBS	=	$(TOPDIR)llib-ldsap $(TOPDIR)llib-lisode

###############################################################################
# FILES
###############################################################################

CFILES	=	config.parse.c main.c interact.c init.c
OFILES	=	config.parse.o main.o interact.o init.o
BFILES	=	config.b

###############################################################################
# FLAGS and OPTIONS
###############################################################################

INCQUERY =	-I./query
YFLAGS	=	-d

###############################################################################
# Here it is...
###############################################################################

all:		doog
inst-all:	inst-doog inst-doogrc manuals
install:	inst-all clean
lint:		l-doog
depend:		$(CFILES)
		makedepend $(OPTIONS) $(INCQUERY) $(CFILES)

###############################################################################
# doog
###############################################################################

doog:           xdoog

xdoog:		$(OFILES) $(QLIB) 
	$(LDCC) $(LDFLAGS) -o $@ $(OFILES) \
		$(QLIB) \
		$(LIBDSAP) \
		$(LIBISODE) \
		$(LSOCKET)

$(QLIB):	true
		cd query; $(MAKE)


inst-doog:	$(BINDIR)doog

$(BINDIR)doog:	xdoog
		-cp $@ zxdoog
		rm -f $@
		cp xdoog $@
		-@ls -gls $@
		-@echo ""

l-doog:		l-query
		$(LINT) $(LFLAGS) $(OPTIONS) $(INCQUERY) $(CFILES) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

l-query:;	cd query ; $(MAKE) lint

config.parse.c: config.b
	bison $(YFLAGS) -o $@ $?

############################################################################
# doogrc
############################################################################

inst-doogrc:	$(ETCDIR)doogrc

$(ETCDIR)doogrc:;
		if [ -s $@ ]; \
			then exit 0; \
			else cp doogrc $@; ls -gls $@; \
		fi

###########################################################################
# manual pages
###########################################################################

MANUALS =	doog.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""

##############################################################
# clean
##############################################################

clean:;         rm -f *.ph *.o *.a a.out _* x* z* *.orig 
		rm -f core 
		cd query ; $(MAKE) clean

true:

grind:

# DO NOT DELETE THIS LINE -- make depend depends on it.

