###############################################################################
#   Instructions to Make, for compilation of ISODE ROSY processes
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/rosy/RCS/Makefile,v 9.0 1992/06/16 12:37:29 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:37:29  isode
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

.SUFFIXES:	.ry .py .c .o

.ry.py:;	./xrosy $(RYFLAGS) $<


PEPYLIBES=	$(TOPDIR)psap/sprintoid.o \
		$(TOPDIR)psap/oid_cmp.o $(TOPDIR)psap/oid_cpy.o \
		$(TOPDIR)psap/oid_free.o \
		$(TOPDIR)compat/asprintf.o $(TOPDIR)compat/serror.o \
		$(TOPDIR)compat/sprintb.o $(TOPDIR)compat/sstr2arg.o
LIBES   =	$(TOPDIR)libpsap.a $(TOPDIR)libcompat.a
LLIBS   =	$(TOPDIR)llib-lpsap $(TOPDIR)llib-lcompat
HFILES	=	$(HDIR)rosy.h $(HDIR)rosap.h $(HDIR)acsap.h $(HDIR)psap2.h \
		$(HDIR)psap.h $(HDIR)ssap.h $(HDIR)isoaddrs.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h
PEPSYFILES=	$(HDIR)pepsy.h $(TOPDIR)pepsy/xpepsy
RYHFILES=	rosy-defs.h $(HDIR)psap.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		rosy librosy
inst-all:	inst-rosy manuals # inst-librosy
install:	inst-all clean
lint:		l-rosy l-librosy


##################################################################
# rosy
##################################################################

inst-rosy:	$(BINDIR)rosy

$(BINDIR)rosy:	xrosy
		-cp $@ zxrosy
		-rm -f $@
		cp xrosy $@
		-@ls -gls $@
		-@echo ""

rosy:		xrosy

xrosy:		rosyvrsn.o $(PEPYLIBES)
		$(LDCC) $(LDFLAGS) -o $@ rosy.o yacc.o pepy_misc.o \
			rosyvrsn.o $(PEPYLIBES) -lm

rosy.o:		rosy.c $(RYHFILES)
		$(CC) $(CFLAGS) -c rosy.c

pepy_misc.o:	$(TOPDIR)pepy/pepy_misc.c
		$(CC) $(CFLAGS) -c $?

rosyvrsn.c:	rosy.o yacc.o pepy_misc.o
		@$(UTILDIR)version.sh rosy > $@

yacc.o:		yacc.c lex.c $(RYHFILES)
		$(CC) $(CFLAGS) -c yacc.c

yacc.c:		yacc.y
		-@echo "expect 20 shift/reduce and 9 reduce/reduce conflicts"
		yacc $(YACCFLAGS) yacc.y
		mv y.tab.c $@

yacc.y:		$(TOPDIR)pepy/yacc.y.gnrc
		$(UTILDIR)extract.sh ROSY < $? > $@

lex.c:		lex.l
		$(LEX) $(LEXFLAGS) lex.l
		mv lex.yy.c $@

lex.l:		$(TOPDIR)pepy/lex.l.gnrc
		$(UTILDIR)extract.sh ROSY < $? > $@

rosy-defs.h:	$(TOPDIR)pepsy/pepsy.h.gnrc
		$(UTILDIR)extract.sh ROSY < $? > $@

l-rosy:		yacc.c lex.c true
		$(LINT) $(LFLAGS) rosy.c yacc.c $(TOPDIR)pepy/pepy_misc.c \
			rosyvrsn.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


################################################################
# librosy
################################################################

CFILES	=	rystub.c rydiscard.c ryoper.c ryopinvoke.c rygenid.c \
		rydispatch.c rydsresult.c rydserror.c rydsurej.c \
		rywait.c ryopblock.c rydsblock.c ryfind.c rylose.c

# pepsy based rosy library routines
P-OFILES=	p-rystub.o p-rydiscard.o p-ryoper.o p-ryopinvoke.o \
		p-rygenid.o p-rydispatch.o p-rydsresult.o p-rydserror.o \
		p-rydsurej.o p-rywait.o p-ryopblock.o p-rydsblock.o \
		p-ryfind.o p-rylose.o

# (old) normal rosy library routines
N-OFILES=	rystub.o rydiscard.o ryoper.o ryopinvoke.o \
		rygenid.o rydispatch.o rydsresult.o rydserror.o \
		rydsurej.o rywait.o ryopblock.o rydsblock.o \
		ryfind.o rylose.o

OFILES=		$(P-OFILES) $(N-OFILES) $(OSTRINGS)

inst-librosy:	$(LIBDIR)librosy.a $(LINTDIR)llib-lrosy

$(LIBDIR)librosy.a:	librosy.a
		-rm -f $@
		cp librosy.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lrosy:	llib-lrosy
		-cp $@ zllib-lrosy
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lrosy | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

librosy:	librosy.a

librosy.a:	$(OFILES)
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES)
		-@rm -f $(TOPDIR)librosy.a $(TOPDIR)llib-lrosy
		-@$(LN) librosy.a $(TOPDIR)librosy.a
		-@$(LN) llib-lrosy $(TOPDIR)llib-lrosy
		-@ls -l $@
		-@echo "ROSY library built normally"

l-librosy:	$(CFILES) true
		$(LINT) $(LFLAGS) $(CFILES) $(TOPDIR)llib-lrosap $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

p-rydiscard.o:	$(HFILES) $(PEPSYFILES) rydiscard.c
		-@rm -f p-rydiscard.c
		ln rydiscard.c p-rydiscard.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rydiscard.c
		-@rm -f p-rydiscard.c

p-rygenid.o:	$(HFILES) $(PEPSYFILES) rygenid.c
		-@rm -f p-rygenid.c
		ln rygenid.c p-rygenid.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rygenid.c
		-@rm -f p-rygenid.c

p-rystub.o:	$(HFILES) $(PEPSYFILES) rystub.c
		-@rm -f p-rystub.c
		ln rystub.c p-rystub.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rystub.c
		-@rm -f p-rystub.c

p-rywait.o:	$(HFILES) $(PEPSYFILES) rywait.c
		-@rm -f p-rywait.c
		ln rywait.c p-rywait.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rywait.c
		-@rm -f p-rywait.c

p-ryoper.o:	 $(HFILES) $(PEPSYFILES) ryoper.c
		-@rm -f p-ryoper.c
		ln ryoper.c p-ryoper.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-ryoper.c
		-@rm -f p-ryoper.c

p-ryopinvoke.o: $(HFILES) $(PEPSYFILES) ryopinvoke.c
		-@rm -f p-ryopinvoke.c
		ln ryopinvoke.c p-ryopinvoke.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-ryopinvoke.c
		-@rm -f p-ryopinvoke.c

p-rydispatch.o:	$(HFILES) $(PEPSYFILES) rydispatch.c
		-@rm -f p-rydispatch.c
		ln rydispatch.c p-rydispatch.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rydispatch.c
		-@rm -f p-rydispatch.c

p-rydsresult.o:	$(HFILES) $(PEPSYFILES) rydsresult.c
		-@rm -f p-rydsresult.c
		ln rydsresult.c p-rydsresult.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rydsresult.c
		-@rm -f p-rydsresult.c

p-rydserror.o:	$(HFILES) $(PEPSYFILES) rydserror.c
		-@rm -f p-rydserror.c
		ln rydserror.c p-rydserror.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rydserror.c
		-@rm -f p-rydserror.c

p-rydsurej.o: $(HFILES) $(PEPSYFILES) rydsurej.c
		-@rm -f p-rydsurej.c
		ln rydsurej.c p-rydsurej.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rydsurej.c
		-@rm -f p-rydsurej.c

p-ryopblock.o:	$(HFILES) $(PEPSYFILES) ryopblock.c
		-@rm -f p-ryopblock.c
		ln ryopblock.c p-ryopblock.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-ryopblock.c
		-@rm -f p-ryopblock.c

p-rydsblock.o:	$(HFILES) $(PEPSYFILES) rydsblock.c
		-@rm -f p-rydsblock.c
		ln rydsblock.c p-rydsblock.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rydsblock.c
		-@rm -f p-rydsblock.c

p-ryfind.o:	$(HFILES) $(PEPSYFILES) ryfind.c
		-@rm -f p-ryfind.c
		ln ryfind.c p-ryfind.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-ryfind.c
		-@rm -f p-ryfind.c

p-rylose.o:	$(HFILES) $(PEPSYFILES) rylose.c
		-@rm -f p-rylose.c
		ln rylose.c p-rylose.c
		$(CC) $(LIBCFLAGS) -DPEPSY_VERSION=2 -c p-rylose.c
		-@rm -f p-rylose.c


rystub.o:	$(HFILES)
rydiscard.o:	$(HFILES)
ryoper.o:	$(HFILES)
ryopinvoke.o:	$(HFILES)
rygenid.o:	$(HFILES)
rydispatch.o:	$(HFILES)
rydsresult.o:	$(HFILES)
rydserror.o:	$(HFILES)
rydsurej.o:	$(HFILES)
rywait.o:	$(HFILES)
ryopblock.o:	$(HFILES)
rydsblock.o:	$(HFILES)
ryfind.o:	$(HFILES)
rylose.o:	$(HFILES)

true:;


################################################################
# manual pages
################################################################

MANUALS	=	rosy.1 # librosy.3n

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a x* z* _* core rosy-defs.h yacc.y yacc.c lex.l \
			lex.c p-ry*.c rosyvrsn.c

grind:;		iprint Makefile
		tgrind -lc rosy-defs.h rosy.c rosyvrsn.c
		tgrind -ly yacc.y lex.l
		tgrind -lc $(CFILES) llib-lrosy
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve
