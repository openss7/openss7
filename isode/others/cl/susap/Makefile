###############################################################################
#   Instructions to Make, for compilation of ISODE SSAP UNITDATA for HULA
###############################################################################

LIBES	=	libsusap.a $(TOPDIR)libtusap.a /usr/lib/libcompat.a \
		/usr/lib/libisode.a /usr/lib/libpsap.a /usr/lib/libpsap2.a \
		/usr/lib/libacsap.a 

# LIBES	=	libsusap.a /usr/lib/libcompat.a $(TOPDIR)/libtusap.a

LLIBS	=	$(TOPDIR)llib-lhula $(TOPDIR)llib-lcompat
HFILES	=	$(HDIR)spkt.h $(HDIR)ssap.h $(HDIR)tsap.h $(HDIR)isoaddrs.h \
		$(HDIR)general.h $(HDIR)manifest.h  $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		libsusap susaptest
inst-all:	inst-libssap manuals
install:	inst-all clean
lint:		l-libssap l-susaptest


################################################################
# libsusap
################################################################

CFILES	=	ssapunitdata.c str2spkt.c text2spkt.c tsdu2spkt.c ssaputils.c \
		susaperror.c susaplose.c


OFILES	=	ssapunitdata.o str2spkt.o text2spkt.o tsdu2spkt.o ssaputils.o \
		susaperror.o susaplose.o $(OSTRINGS)



$(LIBDIR)libsusap.a:	libsusap.a
		-rm -f $@
		cp libsusap.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lsusap:	llib-lsusap
		-cp $@ zllib-lsusap
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lsusap > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libsusap:	libsusap.a

libsusap.a:	susapvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			susapvrsn.o
		-@rm -f $(TOPDIR)libsusap.a $(TOPDIR)llib-lsusap
		-@ln libsusap.a $(TOPDIR)libsusap.a
		-@ln llib-lssap $(TOPDIR)llib-lssap
		-@ls -l $@
		-@echo "SuSAP library built normally"

susapvrsn.c:	$(OFILES)
		@$(UTILDIR)version.sh susap > $@

l-libsusap:;	$(LINT) $(LFLAGS) $(CFILES) susapvrsn.c $(LLIBS)

ssapunitdata.o:	$(HFILES) $(HDIR)susap.h $(HDIR)uderrors.h
susaperror.o:	$(HFILES) $(HDIR)susap.h $(HDIR)uderrors.h
susaplose.o:	$(HFILES) $(HDIR)uderrors.h
ssaputils.o:	$(HFILES) $(HDIR)susap.h $(HDIR)uderrors.h
str2spkt.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)susap.h $(HDIR)uderrors.h
text2spkt.o:	$(HFILES) $(HDIR)susap.h $(HDIR)uderrors.h
tsdu2spkt.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)susap.h $(HDIR)uderrors.h


################################################################
# susaptest
################################################################

susaptest:	susaptest.o libsusap.a
		$(LDCC) $(LDFLAGS) -o $@ susaptest.o $(LIBES) $(LSOCKET)

susaptest.o:	$(HFILES)
		$(CC) $(CFLAGS) -g -c $*.c

l-susaptest:;	$(LINT) $(LFLAGS) susaptest.c llib-lssap $(LLIBS)


################################################################
# manual pages
################################################################

MANUALS	=	libssap.3n

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a z* _* core susaptest susapvrsn.c \
			$(CLEANSTRINGS)

grind:;		iprint Makefile
		tgrind -lc $(CFILES) susapvrsn.c susaptest.c llib-lssap
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

