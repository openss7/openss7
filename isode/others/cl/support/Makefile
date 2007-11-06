###############################################################################
#   Instructions to Make, for compilation of ISODE HULA support processes
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

.c.o:;		$(CC) $(CFLAGS) -c $*.c


LIBES   =	$(TOPDIR)librosy.a $(TOPDIR)librosap.a $(TOPDIR)librtsap.a \
		$(TOPDIR)libacsap.a $(TOPDIR)libpsap2.a $(TOPDIR)libpepy.a \
		$(TOPDIR)libpsap.a $(TOPDIR)libssap.a $(TOPDIR)libtsap.a \
		$(TOPDIR)libcompat.a $(TP4LIBES)
UDLIBES   =	../tusap/libtusap.a /usr/lib/libisode.a
#		/usr/lib/libcompat.a

#		/usr/lib/librosy.a /usr/lib/librtsap.a \
#		/usr/lib/libacsap.a /usr/lib/libpsap2.a /usr/lib/libpepy.a \
#		/usr/lib/libpsap.a /usr/lib/libssap.a /usr/lib/libtsap.a \

# librousap, acusap, pusap2   ......  from billy 
 
LLIBS   =	$(TOPDIR)llib-lrosy $(TOPDIR)llib-lrosap $(TOPDIR)llib-lrtsap \
		$(TOPDIR)llib-lacsap $(TOPDIR)llib-lpsap2 \
		$(TOPDIR)llib-lpsap $(TOPDIR)llib-lssap $(TOPDIR)llib-ltsap \
		$(TOPDIR)llib-lcompat $(TP4LLIBS)
CFILES	=	tsapud.c
HFILES	=	$(HDIR)tsap.h $(HDIR)tusap.h \
		$(HDIR)isoaddrs.h $(HDIR)isoservent.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

##################################################################
# tsapud    TSAP UNIT DATA DAEMON
##################################################################

#inst-tsapud:	$(ETCDIR)tsapud

#$(ETCDIR)tsapud:	xtsapud
#		-cp $@ zxtsapud
#		-rm -f $@
#		cp xtsapud $@
#		-@ls -gls $@
#		-@echo ""

tsapud:		xtsapud
		@echo $@ made.

xtsapud:	tsapud.o 
		$(LDCC) $(LDFLAGS) -o $@ tsapud.o $(UDLIBES) $(LSOCKET)

tsapud.o:	$(HFILES) $(HDIR)logger.h $(HDIR)tailor.h


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a x* z* _* core isodevrsn.c llib-lisode \
		    isoaliases isoentities isobjects isoservices

grind:;		iprint Makefile isoentities isobjects isoservices isotailor
		tgrind -lc $(CFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve



