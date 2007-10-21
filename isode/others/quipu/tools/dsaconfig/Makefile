###############################################################################
#   Instructions to Make, for compilation of ISODE QUIPU DSA configurator
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/tools/dsaconfig/RCS/Makefile,v 9.0 1992/06/16 12:43:56 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:43:56  isode
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

LIBD	=	$(TOPDIR)libisode.a  $(TOPDIR)quipu/quipuvrsn.o
LIBES	=	$(LIBISODE)  $(TOPDIR)quipu/quipuvrsn.o
LLIBS   =	$(TOPDIR)llib-lisode 


###############################################################################
# FILES
###############################################################################

CFILES  =       dsaconfig.c usconfig.c


##############################################################
# Here it is...
##############################################################

all:            dsaconfig usconfig
inst-all:       inst-pilot inst-dsaconfig inst-usconfig manuals
install:        inst-all clean
lint:           l-dsaconfig


###################################################################
# pilot
###################################################################

inst-pilot:	$(ETCDIR)dsaptailor true
		cp -r quipu $(ETCDIR)

$(ETCDIR)dsaptailor: dsaptailor $(TOPDIR)dsap/dsaptailor.root-dsas
		if [ -s $@ ]; \
		    then exit 0; \
		    else cat dsaptailor $(TOPDIR)dsap/dsaptailor.root-dsas > $@; ls -gls $@; \
		fi
		if ln $@ $@.old; \
		    then rm -f $@; cat dsaptailor $(TOPDIR)dsap/dsaptailor.root-dsas > $@; ls -gls $@; \
		    else exit 0; \
		fi


###################################################################
# dsaconfig
###################################################################

inst-dsaconfig:	$(SBINDIR)dsaconfig

$(SBINDIR)dsaconfig:	xdsaconfig
		-cp $@ zxdsaconfig
		-rm -f $@
		cp xdsaconfig $@
		-@ls -gls $@
		-@echo ""

dsaconfig:	xdsaconfig
		@echo $@ made.

xdsaconfig:	dsaconfig.o $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ dsaconfig.o $(LIBES) $(LSOCKET)

l-dsaconfig:;	$(LINT) $(LFLAGS) dsaconfig.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


###################################################################
# usconfig
###################################################################

inst-usconfig:	$(SBINDIR)usconfig

$(SBINDIR)usconfig:	xusconfig
		-cp $@ zxusconfig
		-rm -f $@
		cp xusconfig $@
		-@ls -gls $@
		-@echo ""

usconfig:	xusconfig
		@echo $@ made.

xusconfig:	usconfig.o $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ usconfig.o $(LIBES) $(LSOCKET)

l-usconfig:;	$(LINT) $(LFLAGS) usconfig.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


################################################################
# manual pages
################################################################

MANUALS	=	dsaconfig.8 usconfig.8

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##############################################################
# clean
##############################################################

clean:;         rm -f *.ph *.o *.a a.out _* x* z* *.orig core

grind:;         iprint Makefile
		tgrind -lc $(CFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
