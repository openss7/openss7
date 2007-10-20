###############################################################################
#   Instructions to Make, for compilation of ISODE MOSY processes
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/mosy/RCS/Makefile,v 9.0 1992/06/16 12:42:35 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:42:35  isode
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

.SUFFIXES:	.c .o

.c.o:;		$(CC) $(CFLAGS) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LIBES   =	$(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

HFILES	=	mosy-defs.h 
YFILES	=	yacc.y lex.l


##################################################################
# Here it is...
##################################################################

all:		mosy
inst-all:	inst-mosy manuals
install:	inst-all clean
lint:		l-mosy


##################################################################
# mosy
##################################################################

inst-mosy:	$(BINDIR)mosy

$(BINDIR)mosy:	xmosy
		-cp $@ zxmosy
		-rm -f $@
		cp xmosy $@
		-@ls -gls $@
		-@echo ""

mosy:		xmosy

xmosy:		mosyvrsn.o
		$(LDCC) $(LDFLAGS) -o $@ mosy.o yacc.o pepy_misc.o mosyvrsn.o \
			$(LIBES) $(LSOCKET) -lm

mosy.o:		mosy.c $(HFILES)

pepy_misc.o:	$(TOPDIR)pepy/pepy_misc.c
		$(CC) $(CFLAGS) -c $?

mosyvrsn.c:	mosy.o yacc.o pepy_misc.o
		@$(UTILDIR)version.sh mosy > $@

yacc.o:		yacc.c lex.c $(HFILES)

yacc.c:		yacc.y
		-@echo "expect 23 shift/reduce and 11 reduce/reduce conflicts"
		yacc $(YACCFLAGS) yacc.y
		mv y.tab.c $@

yacc.y:		$(TOPDIR)pepy/yacc.y.gnrc
		$(UTILDIR)extract.sh MOSY < $? > $@

lex.c:		lex.l
		$(LEX) $(LEXFLAGS) lex.l
		mv lex.yy.c $@

lex.l:		$(TOPDIR)pepy/lex.l.gnrc
		$(UTILDIR)extract.sh MOSY < $? > $@

mosy-defs.h:	$(TOPDIR)pepsy/pepsy.h.gnrc
		$(UTILDIR)extract.sh MOSY < $? > $@

l-mosy:		yacc.c lex.c true
		$(LINT) -I$(TOPDIR)h $(LFLAGS) mosy.c yacc.c \
		    $(TOPDIR)pepy/pepy_misc.c mosyvrsn.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


################################################################
# manual pages
################################################################

MANUALS	=	mosy.1

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a x* z* _* core mosy-defs.h yacc.y yacc.c lex.l \
			lex.c mosyvrsn.c *.defs

grind:;		iprint Makefile
		tgrind -lc $(HFILES) mosy.c mosyvrsn.c
		tgrind -ly $(YFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
