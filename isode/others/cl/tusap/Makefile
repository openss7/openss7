###############################################################################
#   Instructions to Make, for compilation of ISODE TSAP processes
#
#   kad	   modified for HULA connectionless tsap
#
###############################################################################

###############################################################################
#
# $Header$
#
#
# $Log$
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

LIBES	=	libtusap.a  /usr/lib/libcompat.a /usr/lib/libisode.a 

#/usr/lib/libisode.a \
#		/usr/lib/libpsap.a /usr/lib/libpsap2.a /usr/lib/libssap.a \
#		/usr/lib/libacsap.a

LLIBS	=	$(TOPDIR)llib-lcompat 
HFILES	=	$(HDIR)tsap.h $(HDIR)isoaddrs.h $(HDIR)tusap.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		libtusap udptest
test:		libtusap tsaputest		
inst-all:	inst-libtusap manuals
install:	inst-all clean
lint:		l-libtusap l-udptest


################################################################
# libtusap
################################################################

CFILES	=	tsapunitdata.c tsaputils.c uderror.c ts2udp.c \
		cltp.c tusaplose.c tusaperror.c \
		$(CTSAP)

# 		tsaplisten.c 
#		tsaprovider.c tsaperror.c tsapstate.c \
#		tsaprespond.c tsapinitiate.c tsaplose.c \
#		tsaplisten.c uderror.c $(CTSAP)
#		ts2udp.c tp0ts.c ts2tcp.c ts2x25.c ts2cons.c \
#		ts2bridge.c ts2sunlink.c $(CTSAP)

OFILES	=	tsapunitdata.o tsaputils.o uderror.o ts2udp.o \
		cltp.o tusaplose.o tusaperror.o \
		$(OTSAP) $(OSTRINGS)

#		tsaplisten.o \
#		tsaprovider.o tsaperror.o tsapstate.o \
#		tsaprespond.o tsapinitiate.o tsaplose.o \
#		tsaplisten.o uderror.o\
#		ts2udp.o tp0ts.o ts2tcp.o ts2x25.o ts2cons.o \
#		ts2bridge.o ts2sunlink.o $(OTSAP) \
#		$(OSTRINGS)


inst-libtusap:	$(LIBDIR)libtusap.a $(LINTDIR)llib-ltusap

$(LIBDIR)libtusap.a:	libtusap.a
		-rm -f $@
		cp libtusap.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-ltusap:	llib-ltusap
		-cp $@ zllib-ltusap
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-ltusap > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libtusap:	libtusap.a

libtusap.a:	tsapvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			tsapvrsn.o
		-@rm -f $(TOPDIR)libtusap.a $(TOPDIR)llib-ltusap
		-@ln libtusap.a $(TOPDIR)libtusap.a
		-@ln llib-ltusap $(TOPDIR)llib-ltusap
		-@ls -l $@
		-@echo "HULA TuSAP library built normally"

tsapvrsn.c:	$(OFILES)
		@$(UTILDIR)version.sh tsap > $@

l-libtusap:;	$(LINT) $(LFLAGS) $(CFILES) tsapvrsn.c $(LLIBS)

tsapunitdata.o:	$(HDIR)tpkt.h $(HDIR)tsap.h $(HDIR)tusap.h $(HFILES)
cltp.o:		$(HDIR)tpkt.h $(HDIR)tsap.h $(HDIR)tusap.h $(HFILES)
tsaputils.o:	$(HFILES)
uderror.o:	$(HDIR)uderrors.h $(HFILES)
tusaplose.o:	$(HDIR)tpkt.h $(HFILES)
tusaperror.o:	$(HFILES)
ts2udp.o:	$(HDIR)tpkt.h $(HDIR)tsap.h $(HFILES)


################################################################
# tsaptest
################################################################
test:		tsaputest.o libtusap.a
		$(LDCC) $(LDFLAGS) -o $@ tsaputest.o $(LIBES) $(LSOCKET)  

tsaputest.o:	$(HDIR)tusap.h $(HFILES)
		$(CC) $(CFLAGS) -g -c $*.c

udptest:	udptest.o libtusap.a
		$(LDCC) $(LDFLAGS) -o $@ udptest.o $(LIBES) $(LSOCKET)  

udptest.o:	$(HFILES)
		$(CC) $(CFLAGS) -g -c $*.c

l-udptest:;	$(LINT) $(LFLAGS) udptest.c llib-ltusap $(LLIBS)


################################################################
# manual pages
################################################################

MANUALS	=	libtsap.3n

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a z* _* core tsaptest tsapvrsn.c \
			$(CLEANSTRINGS)

grind:;		iprint Makefile
		tgrind -lc $(CFILES) tsapvrsn.c tsaptest.c llib-ltusap
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve
