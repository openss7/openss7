#################################################################
#                                                               #
#  ISODECL - HULA project - connectionless ISODE                #
#                                             			#
#  module:  	Makefile for rousap                             #
#  author:   	Bill Haggerty                                   #
#  date:     	4/89                                            #
#                                                               #
#  This makes the librousap.a library.				#
#  This library supports RO over the A-UNIT-DATA service.       #
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
#
# $Header$
#
#
# $Log$
# 
###############################################################################
#   Instructions to Make, for compilation of ISODE RoSAP processes
#   To use the A-UNIT-DATA service
###############################################################################

.SUFFIXES:	.py .c .o

.py.c:;		$(TOPDIR)pepy/xpepy -a PY_advise -m $(PYFLAGS) $<

LIBES	=	librousap.a
LLIBS   =	$(TOPDIR)llib-lacusap \
		$(TOPDIR)llib-lpusap2 $(TOPDIR)llib-lpsap \
		$(TOPDIR)llib-lcompat
HFILES	=	$(HDIR)rosap.h $(HDIR)acusap.h $(HDIR)pusap2.h \
		$(HDIR)psap.h \
		$(HDIR)isoaddrs.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		librousap
inst-all:	inst-librosap manuals
install:	inst-all clean
lint:		l-librosap


################################################################
# librosap
################################################################

CFILES	=	rosaperror.c \
		rosapinvoke.c rosapresult.c rosapuerror.c rosapureject.c \
		rosapwait.c rosapservice.c rosapapdu.c \
		rosapasync.c rosapselect.c rosaplose.c \
		ro2acu.c
PYFILES	=	ros.py
OFILES	=	rosaperror.o \
		rosapinvoke.o rosapresult.o rosapuerror.o rosapureject.o \
		rosapwait.o rosapservice.o rosapapdu.o \
		rosapasync.o rosapselect.o rosaplose.o \
		ro2acu.o \
		ros.o vros.o $(OSTRINGS)

inst-librousap:	$(LIBDIR)librousap.a $(LINTDIR)llib-lrousap

$(LIBDIR)librousap.a:	librousap.a
		-rm -f $@
		cp librousap.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lrousap:	llib-lrousap
		-cp $@ zllib-lrousap
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lrousap > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

librousap:	librousap.a

librousap.a:	rousapvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			rousapvrsn.o
		-@rm -f $(TOPDIR)librousap.a $(TOPDIR)llib-lrousap
		-@ln librousap.a $(TOPDIR)librousap.a
		-@ln llib-lrousap $(TOPDIR)llib-lrousap
		-@ls -l $@
		-@echo "RoSAP library built normally"

rousapvrsn.c:	$(OFILES)
		@$(UTILDIR)version.sh rosap > $@

l-librousap:	ros.c true
		$(LINT) $(LFLAGS) $(CFILES) ros.c rousapvrsn.c $(LLIBS)


rosaperror.o:	$(HFILES)
rosapinvoke.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapresult.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapuerror.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapureject.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapwait.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapservice.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapapdu.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES) $(HDIR)tailor.h
rosapasync.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosapselect.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
rosaplose.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)

ro2acu.o:	$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES) $(HDIR)tailor.h

#ro2rts.o:	$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
#ro2ps.o:	$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
#ro2ssexec.o:	$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES) $(HDIR)isoservent.h \
#		$(HDIR)tailor.h
#ro2ssrespond.o:$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
#ro2ssinitiat.o:$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES) $(HDIR)isoservent.h \
#		$(HDIR)tailor.h
#ro2ssreleas1.o:$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES)
#ro2ssreleas2.o:$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES)
#ro2ss.o:	$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES) $(HDIR)tailor.h
#ro2ssthorn.o:	$(HDIR)ropkt.h $(HDIR)acpkt.h $(HFILES)

ros.o:		$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
ros.o:		ros.c
ros.c:		ros.py $(TOPDIR)pepy/xpepy
vros.o:		$(HDIR)ropkt.h $(HDIR)acupkt.h $(HFILES)
vros.o:		vros.c
vros.c:		ros.py $(TOPDIR)pepy/xpepy
		$(TOPDIR)pepy/xpepy -a PY_advise -m -d -S PRINT -o $@ $(PYFLAGS) ros.py


################################################################
# manual pages
################################################################

MANUALS	=	librosap.3n

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.ph *.o *.a ros.c vros.c z* _* core rosapvrsn.c \
			$(CLEANSTRINGS)

grind:;		iprint Makefile
		tgrind -lc $(CFILES) rosapvrsn.c rosaptest.c llib-lrosap
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
