###############################################################################
# 
#  RFA - Remote File Access
# 
#  Access and Management for a partial file system tree that exists
#  at two sites either as master files or slave files
# 
#  Makefile
# 
#  Contributed by Oliver Wenzel, GMD Berlin, 1990
# 
#
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/rfa/RCS/Makefile,v 9.0 1992/06/16 12:47:25 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:47:25  isode
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

PEPYPATH=	-DPEPYPATH

.SUFFIXES:	.ry .py .c .o

.c.o:;		$(CC) $(CFLAGS) -c $*.c

###############################################################################
# Programs and Libraries
###############################################################################

LIBES   =	$(TOPDIR)libisode.a
LLIBS   =	$(TOPDIR)llib-lisode

###############################################################################
# Files
###############################################################################

HFILES	=	$(HDIR)rosy.h $(HDIR)rosap.h $(HDIR)acsap.h $(HDIR)psap2.h \
		$(HDIR)psap.h $(HDIR)ssap.h $(HDIR)isoaddrs.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h

##################################################################
# Here it is...
##################################################################

all:		rfad rfa rfatime llock
inst-all:	inst-rfad inst-rfa inst-rfatime inst-llock manuals inst-tailor
install:	inst-all clean
lint:		l-rfad l-rfa l-rfatime l-llock

##################################################################
# llock
##################################################################

LLOCKO =	llock.o rfainfo.o dirname.o tailor.o ls.o advise.o $(LIBES)
LLOCKC =	llock.c rfainfo.c dirname.c tailor.c ls.o advise.c

inst-llock:	$(BINDIR)llock

$(BINDIR)llock:	xllock
		-cp $@ zllock
		-rm -f $@
		-rm -f $(BINDIR)lunlock
		cp xllock $@
		ln $@ $(BINDIR)lunlock 
		-@ls -gls $@
		-@echo ""

llock:		xllock
		@echo $@ made.

xllock:		$(LLOCKO)
		$(LDCC) $(LDFLAGS) -o $@ $(LLOCKO) 

l-llock:;	$(LINT) $(LFLAGS) $(LLOCKC) \
			| grep -v "warning: possible pointer alignment problem"


##################################################################
# rfatime
##################################################################

inst-rfatime:	$(SBINDIR)rfatime

$(SBINDIR)rfatime:	xrfatime
		-cp $@ zrfatime
		-rm -f $@
		cp xrfatime $@
		chown root $@
		chmod u+s $@
		-@ls -gls $@
		-@echo ""

rfatime:	xrfatime
		@echo $@ made.

xrfatime:	rfatime.o 
		$(LDCC) $(LDFLAGS) -o $@ rfatime.o $(LIBISODE) $(LSOCKET)

l-rfatime:;	$(LINT) $(LFLAGS) rfatime.c \
			| grep -v "warning: possible pointer alignment problem"


##################################################################
# rfad
##################################################################

RFADC=		rfad.c RFA-ops.c ryresponder.c reqmaster.c filedata.c \
		fileinfo.c dirname.c synctime.c rfa2fi.c rfainfo.c error.c \
		ls.c advise.c tailor.c filemode.c RFA_tables.c
RFADO=		rfad.o RFA-Rops.o ryresponder.o reqmaster.o filedata.o \
		fileinfo.o dirname.o synctime.o rfa2fi.o rfainfo.o error.o \
		ls.o advise.o tailor.o filemode.o RFA_tables.o

inst-rfad:	$(SBINDIR)ros.rfa

$(SBINDIR)ros.rfa:	xrfad
		-cp $@ zxros.rfa
		-rm -f $@
		cp xrfad $@
		chown root $@
		chmod u+s $@
		-@ls -gls $@
		-@echo ""

rfad:		xrfad
		@echo $@ made.

xrfad:		$(RFADO) $(LIBES) $(HFILES)
		$(LDCC) $(LDFLAGS) -o $@ $(RFADO) $(LIBISODE) $(LSOCKET)

l-rfad:		RFA-ops.c true
		$(LINT) $(LFLAGS) -DPERFORMER $(RFADC) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

rfad.o:		ryresponder.h RFA-ops.h RFA-types.h $(HFILES) $(HDIR)logger.h

RFA-Rops.o:	RFA-ops.c RFA-ops.h $(HFILES)
		$(CC) $(CFLAGS) -DPERFORMER -c RFA-ops.c
		mv RFA-ops.o $@

ryresponder.o:	ryresponder.h RFA-ops.h $(HFILES) $(HDIR)tailor.h


##################################################################
# rfa
##################################################################

RFAO	=	ls.o dirname.o rfainfo.o sync.o rfa2fi.o getfile.o printerr.o\
		error.o tailor.o advise.o filemode.o RFA_tables.o rfa.o \
		RFA-Iops.o ryinitiator.o
RFAC	=	ls.c dirname.c rfainfo.c sync.c rfa2fi.c getfile.c printerr.c\
		error.c tailor.c advise.c filemode.c RFA_tables.c rfa.c \
		RFA-ops.c ryinitiator.c

inst-rfa:	$(BINDIR)rfa

$(BINDIR)rfa:	xrfa
		-cp $@ zxrfa
		-rm -f $@
		cp xrfa $@
		chown root $@
		chmod u+s $@
		-@ls -gls $@
		-@echo ""

rfa:		xrfa
		@echo $@ made.

xrfa:		$(RFAO) $(LIBES)
		$(LDCC) $(LDFLAGS) -o $@ $(RFAO) $(LIBISODE) $(LSOCKET)

l-rfa:		RFA-ops.c true
		$(LINT) $(LFLAGS) -DINVOKER $(RFAC) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

rfa.o:		RFA-ops.h RFA-types.h $(HFILES)

RFA-Iops.o:	RFA-ops.c RFA-ops.h $(HFILES)
		$(CC) $(CFLAGS) -DINVOKER -c RFA-ops.c
		mv RFA-ops.o $@

ryinitiator.o:	$(HFILES)


################################################################
# librfa
################################################################

RFA_tables.o: RFA_tables.c RFA-types.h

RFA_tables.c RFA-types.h : RFA-asn.py $(TOPDIR)pepsy/xpepsy
	$(TOPDIR)pepsy/xpepsy -A -f -h -m RFA-asn.py

RFA-asn.py:   rfa.ry $(TOPDIR)rosy/xrosy
	$(TOPDIR)rosy/xrosy -m -pepsy -defs -o $@ rfa.ry
RFA-ops.c:    rfa.ry
RFA-ops.h:    rfa.ry
RFA-stubs.c:  rfa.ry


################################################################
# manual pages
################################################################

MANUALS	=	rfad.8c rfa.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# tailoring file
################################################################

inst-tailor:	$(ETCDIR)rfatailor

$(ETCDIR)rfatailor:	rfatailor
		-mv -f $@ $(ETCDIR)rfatailor.old
		cp rfatailor $@

################################################################
# clean
################################################################

clean:;		rm -f *.o *.a RFA* x* z* _* core

true:;


