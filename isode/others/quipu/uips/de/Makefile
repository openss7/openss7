###############################################################################
#   Instructions to Make, for compilation of PARADISE de interface
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/Makefile,v 9.0 1992/06/16 12:45:59 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:45:59  isode
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
# Programs and Libraries
###############################################################################

LIBES	=	$(TOPDIR)libdsap.a $(TOPDIR)libisode.a
LLIBS   =	$(TOPDIR)llib-ldsap $(TOPDIR)llib-lisode

.c.o:;          $(CC) $(CFLAGS) -DPEPYPATH -c $*.c

###############################################################################
# FILES
###############################################################################

CFILES	=	de.c bind.c init.c util.c filter.c people.c ou.c org.c \
                country.c cname.c video.c list.c atts.c pager.c mapatt.c  \
		dehelp.c rawinput.c mapphone.c

OFILES	=	de.o bind.o init.o util.o filter.o people.o ou.o org.o \
                country.o cname.o video.o list.o atts.o pager.o mapatt.o \
		dehelp.o rawinput.o mapphone.o

HFILES  =       bind.h config.h destrings.h namelist.h util.h \
                cnamelist.h demanifest.h filter.h types.h mapatts.h \
		mapphone.h query.h

DEFILES =       debrowse decricket deex2 deorginfo dewelcome decommands \
                dedept dehelp deperson dewildcards decountry deex1 deorg \
       		detailor determtypes

##############################################################
# Here it is...
##############################################################

all:            de
inst-all:       inst-de help
install:        inst-all clean
lint:           l-de


###################################################################
# de
###################################################################

inst-de:	$(BINDIR)de

$(BINDIR)de:	xde
		-cp $@ zxde
		-rm -f $@
		cp xde $@
		-@ls -gls $@
		-@echo ""
		@$(UTILDIR)inst-man.sh $(MANOPTS) de.1
		-@echo ""

de:		xde
		@echo $@ made.

xde:		$(OFILES) $(LIBES)
		$(LDCC) $(LDFLAGS) -o $@ $(OFILES) \
			$(LIBDSAP) $(LIBISODE) $(LSOCKET) -ltermcap

l-de:;		$(LINT) $(LFLAGS) $(CFILES) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

help:		$(ETCDIR)de/detailor
		-cp -r etc/* $(ETCDIR)de
		-@ls -gls $(ETCDIR)de

$(ETCDIR)de/detailor: detailor
		-mkdir $(ETCDIR)de
		if [ -s $@ ]; \
		    then exit 0; \
		    else cp detailor $@; ls -gls $@; \
		fi
		if ln $(ETCDIR)de/detailor $(ETCDIR)de/detailor.old; \
		    then rm -f $@ ; cp detailor $@; ls -gls $@; \
		    else exit 0; \
		fi

detailor:

##############################################################
# termhelp
##############################################################

termhelp:	/etc/termcap
		./dotermhelp
		touch $@

##############################################################
# clean
##############################################################

clean:;         rm -f *.ph *.o *.a a.out _* x* z* *.orig core 
		rm -f termhelp de/determtypes.orig

grind:;         

true:;


