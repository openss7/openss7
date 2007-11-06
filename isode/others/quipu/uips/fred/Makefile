###############################################################################
#   Instructions to Make, for compilation of ISODE QUIPU White Pages interface
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/Makefile,v 9.0 1992/06/16 12:44:30 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:44:30  isode
# Release 8.0
#
# 
###############################################################################

###############################################################################
#
#                                NOTICE
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
#LIBES	=	$(TOPDIR)libdirent.a  $(TOPDIR)libcompat.a
#LLIBS	=	$(TOPDIR)llib-ldirent $(TOPDIR)llib-lcompat


###############################################################################
# FILES
###############################################################################

HFILES  =       fred.h
CFILES  =       fred.c dispatch.c miscellany.c pipe.c whois.c


##############################################################
# Here it is...
##############################################################

all:            fred whitepages fredrc ufnrc fredsh dad
inst-all:       inst-fred inst-fredrc inst-ufnrc inst-fredsh inst-dad \
		inst-fredman manuals 
# inst-whitepages
install:        inst-all clean
lint:           l-fred l-dad


###################################################################
# fred
###################################################################

inst-fred:	$(BINDIR)fred

$(BINDIR)fred:	xfred
		-cp $@ zxfred
		-rm -f $@
		cp xfred $@
		-rm -f $(SBINDIR)in.whitepages
		if ln $@ $(SBINDIR)in.whitepages; \
		    then exit 0; \
		    else cp xfred $(SBINDIR)in.whitepages; \
		fi
		-@ls -gls $@ $(SBINDIR)in.whitepages
		-@echo ""

fred:		xfred
		@echo $@ made.

xfred:		fred.o dispatch.o miscellany.o pipe.o socket.o whois.o
		$(LDCC) $(LDFLAGS) -o $@ fred.o dispatch.o miscellany.o \
			pipe.o socket.o whois.o \
			$(LIBES) $(LSOCKET)

l-fred:;	$(LINT) $(LFLAGS) fred.c dispatch.c miscellany.c pipe.c \
			../dish/socket.c whois.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

fred.o:		fred.h
dispatch.o:	fred.h
miscellany.o:	fred.h
pipe.o:		fred.h

socket.o:	../dish/socket.c
		$(CC) $(CFLAGS) -c ../dish/socket.c

whois.o:	fred.h


###################################################################
# fredrc
###################################################################

inst-fredrc:	$(ETCDIR)fredrc

$(ETCDIR)fredrc: fredrc
		if [ -s $@ ]; \
		    then exit 0; \
		    else cp fredrc $@; chmod a+r $@; ls -gls $@; \
		fi
		if ln $(ETCDIR)fredrc $(ETCDIR)fredrc.old; \
		    then rm -f $@ ; cp fredrc $@; chmod a+r $@ ; ls -gls $@; \
		    else exit 0; \
		fi

fredrc:


###################################################################
# ufnrc
###################################################################

inst-ufnrc:	$(ETCDIR)ufnrc

$(ETCDIR)ufnrc: true
		if [ -s $@ ]; \
		    then exit 0; \
		    else cp ufnrc $@; chmod a+r $@ ; ls -gls $@; \
		fi
		if ln $(ETCDIR)ufnrc $(ETCDIR)ufnrc.old; \
		    then rm -f $@ ; cp ufnrc $@; chmod a+r $@ ; ls -gls $@; \
		    else exit 0; \
		fi

ufnrc:


###################################################################
# fredsh
###################################################################

inst-fredsh:	$(SBINDIR)fredsh

$(SBINDIR)fredsh:	xfredsh Makefile
		-cp $@ zxfredsh
		-rm -f $@
		sed -e 's%@(BINDIR)%$(BINDIR)%' < fredsh.sh > $@
		chmod a+rx $@
		-@ls -gls $@
		-@echo ""

fredsh:		xfredsh
		@echo $@ made.

xfredsh:	fredsh.sh
		cp fredsh.sh $@
		chmod a+rx $@


###################################################################
# fredman
###################################################################

inst-fredman:	$(ETCDIR)fred.0

$(ETCDIR)fred.0:	fred.1c Makefile
		-cp $@ zfred.0
		-rm -f $@
		(echo '.ds ED $(ETCDIR)' ; cat fred.1c) | \
			nroff -man -Tlpr | \
			tr "\010" "_" | \
			sed -e "/^/s/__//g" > $@
		-@ls -gls $@
		-@echo ""


###################################################################
# dad
###################################################################

inst-dad:	$(SBINDIR)dad

$(SBINDIR)dad:	xdad
		-cp $@ zxdad
		-rm -f $@
		cp xdad $@
		-@ls -gls $@
		-@echo ""

dad:		xdad
		@echo $@ made.

xdad:		dad.o socket.o
		$(LDCC) $(LDFLAGS) -o $@ dad.o socket.o $(LIBES) $(LSOCKET)

l-dad:;		$(LINT) $(LFLAGS) dad.c ../dish/socket.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


###################################################################
# whitepages
###################################################################

inst-whitepages:	$(BINDIR)whitepages

$(BINDIR)whitepages:	xwhitepages
		-cp $@ zxwhitepages
		-rm -f $@
		cp xwhitepages $@
		-@ls -gls $@
		-@echo ""

whitepages:	xwhitepages
		@echo $@ made.

xwhitepages:	whitepages.sh
		cp whitepages.sh $@
		chmod a+rx $@


################################################################
# manual pages
################################################################

MANUALS	=	fred.1c dad.8c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##############################################################
# clean
##############################################################

clean:;         rm -f *.ph *.o *.a a.out _* x* z* *.orig core

grind:;         iprint Makefile whitepages.sh fredsh
		tgrind -lc $(HFILES) $(CFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
