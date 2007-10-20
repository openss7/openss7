#################################################################
#                                                               #
#  ISODECL - HULA project - connectionless ISODE                #
#                                             			#
#  module:  	Makefile for pusap2                             #
#  author:   	Bill Haggerty                                   #
#  date:     	4/89                                            #
#                                                               #
#  This makes the libpusap.a library.				#
#  This library supports the P-UNIT-DATA service.               #
#  It also makes the pusap2test program to test the service.    #
#								#
#################################################################
#								#
#			     NOTICE		   		#
#								#
#    Use of this module is subject to the restrictions of the   #
#    ISODE license agreement.					#
#								#
#################################################################


###############################################################################
#   HULA
#   Instructions to Make, for compilation of ISODE PUSAP2 processes
#   Note no space available for symbols when making PEPY encode/decode routines
###############################################################################

###############################################################################
#
# $Header$
#
#
# $Log$
# 
###############################################################################

.SUFFIXES:	.py .c .o

.py.c:;		$(TOPDIR)pepy/xpepy -a PY_advise -m $(PYFLAGS) $<


LIBES	=	libpusap2.a

LIBES1	=	/usr/fromkurt/libsusap.a /usr/fromkurt/libtusap.a\
		/usr/lib/libcompat.a \
		/usr/lib/libpepy.a \
		/usr/lib/libpsap.a

LLIBS   =	$(TOPDIR)llib-lpsap $(TOPDIR)llib-lssap $(TOPDIR)llib-lcompat

HFILES	=	$(HDIR)pusap2.h $(HDIR)psap2.h $(HDIR)psap.h $(HDIR)ssap.h \
		$(HDIR)isoaddrs.h $(HDIR)manifest.h $(HDIR)general.h \
		$(HDIR)config.h $(HDIR)susap.h


##################################################################
# Here it is...
##################################################################

all:		libpusap2
inst-all:	inst-libpusap2 manuals
install:	inst-all clean
lint:		l-libpusap2
test:		pusap2test

################################################################
# libpusap2
################################################################
#		psaprovider.c psap2error.c \
#		psapexec.c psaprespond.c psapinitiate.c psaptoken.c \
#		psapactivity.c psapmajor1.c psapmajor2.c psapminor1.c \
#		psapminor2.c psapresync1.c psapresync2.c psapabort.c \
#		psapreport.c psaprelease1.c psaprelease2.c psapselect.c \

#		psaprovider.o psap2error.o \
#		psapexec.o psaprespond.o psapinitiate.o psaptoken.o \
#		psapactivity.o psapmajor1.o psapmajor2.o psapminor1.o \
#		psapminor2.o psapresync1.o psapresync2.o psapabort.o \
#		psapreport.o psaprelease1.o psaprelease2.o psapselect.o \

CFILES	=	psapunitdata.c \
		psaputils.c

PYFILES	=	ps.py

OFILES	=	psapunitdata.o \
		psaputils.o \
 		PS-print.o \
		$(OSTRINGS)

inst-libpusap2:	$(LIBDIR)libpusap2.a $(LINTDIR)llib-lpusap2

$(LIBDIR)libpusap2.a:	libpusap2.a
		-rm -f $@
		cp libpusap2.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lpusap2:	llib-lpusap2
		-cp $@ zllib-lpusap2
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lpusap2 > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libpusap2:	libpusap2.a

libpusap2.a:	pusap2vrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			$(PS-O) pusap2vrsn.o
		-@rm -f $(TOPDIR)libpusap2.a $(TOPDIR)llib-lpusap2
		-@ln libpusap2.a $(TOPDIR)libpusap2.a
		-@ln llib-lpusap2 $(TOPDIR)llib-lpusap2
		-@ls -l $@
		-@echo "PUSAP2 library built normally"

PS-O	=	PS-[0-9]*.o
PS-C	=	PS-[0-9]*.c

PS-O:		PS-C
		@$(MAKE) `/bin/ls $(PS-C) | sed 's/\.c$$/.o/'`
		-@touch $@

PS-C:		PS-types.py $(TOPDIR)pepy/xpepy
		-@rm -f $(PS-C) $(PS-O)
		$(TOPDIR)pepy/xpepy -a PY_advise -A -b PS -m $(PYFLAGS) \
			PS-types.py
		-@touch $@

pusap2vrsn.c:	$(OFILES) PS-O
		@$(UTILDIR)version.sh pusap2 > $@

l-libpusap2:	PS-print.c PS-C true
		$(LINT) $(LFLAGS) $(CFILES) PS-print.c $(PS-C) $(LLIBS)

psapunitdata.o:	PS-types.h $(HDIR)ppkt.h $(HFILES) $(HDIR)tailor.h 
psaputils.o:	PS-types.h $(HDIR)ppkt.h $(HFILES) $(HDIR)tailor.h

PS-types.h:	ps.py $(TOPDIR)pepy/xposy
		$(TOPDIR)pepy/xposy -f -h -m -o PS-types.py $(POFLAGS) ps.py
PS-types.py:	ps.py $(TOPDIR)pepy/xposy
		$(TOPDIR)pepy/xposy -f -h -m -o PS-types.py $(POFLAGS) ps.py

PS-print.c:	ps.py $(TOPDIR)pepy/xpepy
		$(TOPDIR)pepy/xpepy -a PY_advise -d -m -S PRINT -o $@ $(PYFLAGS) ps.py


################################################################
# pusap2test
################################################################

pusap2test:	pusap2test.o libpusap2.a
		$(LDCC) $(LDFLAGS) -o $@ pusap2test.o\
		$(LIBES) $(LIBES1) $(LSOCKET)\

pusap2test.o:   $(HFILES)
		$(CC) $(CFLAGS) -g -c $*.c



################################################################
# manual pages
################################################################

MANUALS	=	libpsap2.3n

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a PS* z* _* core pusap2vrsn.c \
			$(CLEANSTRINGS)

grind:;		iprint Makefile
		tgrind -lc $(CFILES) pusap2vrsn.c llib-lpusap2
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
