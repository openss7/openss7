#################################################################
#                                                               #
#  ISODECL - HULA project - connectionless ISODE                #
#                                             			#
#  module:  	Makefile for acusap                             #
#  author:   	Bill Haggerty                                   #
#  date:     	4/89                                            #
#                                                               #
#  This makes the libacusap.a library.				#
#  This library supports the A-UNIT-DATA service.               #
#  It also makes the acusaptest program to test the service.    #
#								#
#################################################################
#								#
#			     NOTICE		   		#
#								#
#    Use of this module is subject to the restrictions of the   #
#    ISODE license agreement.					#
#								#
#################################################################
# 								#
#   Note:         						#
# 								#
#	no space available for symbols when making PEPY         #
#       encode/decode routines.  DSE left out of Makefile.      #
# 								#
#################################################################

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


LIBES	=	libacusap.a

# LIBES	=	libacusap.a $(TOPDIR)libpepy.a $(TOPDIR)libpsap.a \
#		$(TOPDIR)libcompat.a

LIBES1  = 	$(TOPDIR)libpusap2.a \
		/usr/lib/libcompat.a \
		/usr/lib/libpsap.a \
		/usr/lib/libpepy.a \
		/usr/fromkurt/libsusap.a \
	        /usr/fromkurt/libtusap.a \
		libbilly.a

LIBES2  =	$(TOPDIR)libpusap2.a \
		/usr/fromkurt/libsusap.a \
		/usr/fromkurt/libtusap.a \
		$(TOPDIR)libpsap.a \
		/usr/lib/libisode.a


LLIBS   =	$(TOPDIR)llib-lpsap2 $(TOPDIR)llib-lpsap $(TOPDIR)llib-lcompat\
		$(TOPDIR)llib-lpusap2 $(TOPDIR)llib-lacsap

HFILES	=	$(HDIR)acusap.h $(HDIR)pusap2.h $(HDIR)psap.h \
		$(HDIR)ssap.h $(HDIR)isoaddrs.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		libacusap
inst-all:	inst-libacusap manuals
install:	inst-all clean
lint:		l-libacusap l-acusaptest
test:		acusaptest


################################################################
# libacusap
################################################################

XCFILES	=	acsaprovider.c acsapblock.c acsaperror.c acsaplose.c \
		acsaprespond.c acsapinitiat.c acsapreleas1.c acsapreleas2.c \
		acsapreleas3.c acsapabort1.c acsapabort2.c acsapfindpci.c \
		acsapaddr.c \
		acsapdse.c dq2dn.c isoalias.c \
		acsapstub.c aetdbm.c aetseq.c isoentity.c \
		oid2aei.c sprintaei.c \
		is2paddr.c is2saddr.c is2taddr.c \
		asn2paddr.c asn2taddr.c paddr2asn.c taddr2asn.c \
		acserver.c
XOFILES	=	acsaprovider.o acsapblock.o acsaperror.o acsaplose.o \
		acsaprespond.o acsapinitiat.o acsapreleas1.o acsapreleas2.o \
		acsapreleas3.o acsapabort1.o acsapabort2.o acsapfindpci.o \
		acsapaddr.o \
		acsapdse.o dq2dn.o isoalias.o \
		acsapstub.o aetdbm.o aetseq.o isoentity.o \
		oid2aei.o sprintaei.o \
		is2paddr.o is2saddr.o is2taddr.o \
		asn2paddr.o asn2taddr.o paddr2asn.o taddr2asn.o \
		acserver.o \
		ACS-print.o acsold.o vacsold.o 

CFILES	=	acsapunitdat.c acsaputils.c
PYFILES	=	acs.py 
OFILES	=	acsapunitdat.o acsaputils.o \
		ACS-print.o \
		$(OSTRINGS)

inst-libacsap:	$(LIBDIR)libacusap.a $(LINTDIR)llib-lacusap

$(LIBDIR)libacusap.a:	libacusap.a
		-rm -f $@
		cp libacusap.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lacusap:	llib-lacusap
		-cp $@ zllib-lacusap
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lacusap > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libacusap:	libacusap.a

libacusap.a:	acusapvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			$(ACS-O) acusapvrsn.o
		-@rm -f $(TOPDIR)libacusap.a $(TOPDIR)llib-lacusap
		-@ln libacusap.a $(TOPDIR)libacusap.a
		-@ln llib-lacusap $(TOPDIR)llib-lacusap
		-@ls -l $@
		-@echo "AcuSAP library built normally"

ACS-O	=	ACS-[0-9]*.o
ACS-C	=	ACS-[0-9]*.c

ACS-O:		ACS-C
		@$(MAKE) `/bin/ls $(ACS-C) | sed 's/\.c$$/.o/'`
		-@touch $@

ACS-C:		ACS-types.py $(TOPDIR)pepy/xpepy
		-@rm -f $(ACS-C) $(ACS-O)
		$(TOPDIR)pepy/xpepy -a PY_advise -A -b ACS -m $(PYFLAGS) \
			ACS-types.py
		-@touch $@

DSE-O	=	DSE-[0-9]*.o
DSE-C	=	DSE-[0-9]*.c

DSE-O:		DSE-C
		@$(MAKE) `/bin/ls $(DSE-C) | sed 's/\.c$$/.o/'`
		-@touch $@

DSE-C:		dse.py $(TOPDIR)pepy/xpepy
		-@rm -f $(DSE-C) $(DSE-O)
		$(TOPDIR)pepy/xpepy -a PY_advise -A -b DSE -m $(PYFLAGS) dse.py
		-@touch $@

#acusapvrsn.c:	$(OFILES) ACS-O DSE-O
acusapvrsn.c:	$(OFILES) ACS-O 
		@$(UTILDIR)version.sh acsap > $@

l-libacsap:	ACS-print.c acsold.c ACS-C DSE-C true
		$(LINT) $(LFLAGS) $(CFILES) ACS-print.c acsold.c \
			$(ACS-C) $(DSE-C) $(LLIBS)

acsapunitdat.o: ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)isoservent.h \
		$(HDIR)tailor.h
acsaprovider.o:	ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapblock.o:	$(HDIR)acpkt.h $(HFILES)
acsaperror.o:	$(HFILES)
acsaplose.o: 	ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsaprespond.o: ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapinitiat.o: ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)isoservent.h \
		$(HDIR)tailor.h
acsapreleas1.o: ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapreleas2.o: ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapreleas3.o: ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapabort1.o: 	ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapabort2.o: 	ACS-types.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
acsapabort2.o: 	$(HDIR)acpkt.h $(HFILES)
acsapaddr.o: 	$(HFILES) $(HDIR)tailor.h
acsapdse.o: 	$(HFILES) $(HDIR)internet.h $(HDIR)tailor.h
dq2dn.o:	$(HDIR)psap.h $(HDIR)manifest.h $(HDIR)general.h \
		$(HDIR)config.h $(HDIR)isoaddrs.h $(HDIR)tailor.h \
		$(HDIR)quipu/name.h $(HDIR)quipu/attr.h $(HDIR)quipu/oid.h
isoalias.o:	$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h \
		$(HDIR)tailor.h
acsapstub.o: 	$(HFILES) $(HDIR)tailor.h
aetdbm.o: 	$(HFILES) $(HDIR)tailor.h
aetseq.o:	$(HFILES)
isoentity.o:	$(HFILES) $(HDIR)tailor.h
oid2aei.o:	$(HFILES)
sprintaei.o:	$(HFILES)
is2paddr.o:	$(HFILES)
is2saddr.o:	$(HFILES)
is2taddr.o:	$(HFILES)
asn2paddr.o:	$(HFILES) $(HDIR)tailor.h
asn2taddr.o:	$(HFILES) $(HDIR)tailor.h
paddr2asn.o:	$(HFILES) $(HDIR)tailor.h
taddr2asn.o:	$(HFILES) $(HDIR)tailor.h
acserver.o:	$(HFILES) $(HDIR)tailor.h

ACS-types.h:	acs.py $(TOPDIR)pepy/xposy
		$(TOPDIR)pepy/xposy -f -h -m -o ACS-types.py $(POFLAGS) acs.py
ACS-types.py:	acs.py $(TOPDIR)pepy/xposy
		$(TOPDIR)pepy/xposy -f -h -m -o ACS-types.py $(POFLAGS) acs.py

ACS-print.c:	acs.py $(TOPDIR)pepy/xpepy
		$(TOPDIR)pepy/xpepy -a PY_advise -d -m -S PRINT -o $@ $(PYFLAGS) acs.py

acsold.o:	$(HDIR)acpkt.h $(HFILES)
acsold.o:	acsold.c
acsold.c:	acsold.py $(TOPDIR)pepy/xpepy
vacsold.o:	$(HDIR)acpkt.h $(HFILES)
acsold.o:	vacsold.c
vacsold.c:	acsold.py $(TOPDIR)pepy/xpepy
		$(TOPDIR)pepy/xpepy -a PY_advise -m -d -S PRINT -o $@ $(PYFLAGS) acsold.py


################################################################
# acusaptest
################################################################

acusaptest:	acusaptest.o $(LIBES) $(LIBES2)
		$(LDCC) $(LDFLAGS) -o $@ acusaptest.o \
		ADD-1.o  ADD-2.o  ADD-3.o  ADD-4.o  ADD-5.o \
		$(LIBES) $(LIBES2) \
	        $(LSOCKET)

acusaptest.o:	$(HFILES) $(HDIR)isoservent.h $(HDIR)tailor.h
		$(CC) $(CFLAGS) -g -c $*.c

# l-acsaptest:;	$(LINT) $(LFLAGS) acsaptest.c llib-lacsap $(LLIBS)

ADD-O	=	ADD-[0-9]*.o
ADD-C	=	ADD-[0-9]*.c

ADD-O:		ADD-C
		@$(MAKE) `/bin/ls $(ADD-C) | sed 's/\.c$$/.o/'`
		-@touch $@
ADD-C:		ADD-types.py $(TOPDIR)pepy/xpepy
		-@rm -f $(ADD-C) $(ADD-O)
		$(TOPDIR)pepy/xpepy -a PY_advise -A -b ADD -m $(PYFLAGS) \
			ADD-types.py
		-@touch $@

ADD-types.h:	add.py $(TOPDIR)pepy/xposy
		$(TOPDIR)pepy/xposy -f -h -m -o ACS-types.py $(POFLAGS) add.py
ADD-types.py:	add.py $(TOPDIR)pepy/xposy
		$(TOPDIR)pepy/xposy -f -h -m -o ACS-types.py $(POFLAGS) acs.py
ADD-asn.py:	add.ry $(TOPDIR)rosy/xrosy
		$(TOPDIR)rosy/xrosy -m $(RYFLAGS) -o $@ add.ry


################################################################
# manual pages
################################################################

MANUALS	=	libacsap.3n

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a *.ph ACS* DSE* acsold.c vacsold.c dseaddress.c \
			z* _* core acsaptest acusapvrsn.c \
			$(CLEANSTRINGS)

grind:;		iprint Makefile
		tgrind -lc $(CFILES) acusapvrsn.c acsaptest.c llib-lacsap
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;

