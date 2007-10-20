###############################################################################
#   Instructions to Make, for compilation of QUIPU protocol stuff
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/dsap/x500as/RCS/Makefile,v 9.0 1992/06/16 12:14:33 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:14:33  isode
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


PEPYPATH= 	-DPEPYPATH

UPDIR   =       ../

LLIBS	=	$(TOPDIR)llib-lisode


#############################################################
# Here it is...
#############################################################

all:            libx500as 
inst-all:	libx500as
install:        inst-all clean
lint:           l-libx500as


#############################################################
# libx500as
#############################################################

PESPY-C	=	IF_tables.c Thorn_tables.c AF_tables.c \
		DO_tables.c DAS_tables.c \
		Quipu_tables.c SA_tables.c NRS_tables.c

PESPY-O	=	IF_tables.o Thorn_tables.o AF_tables.o \
		DO_tables.o DAS_tables.o \
		Quipu_tables.o SA_tables.o NRS_tables.o

HAND-C	=	asn_ext.c
HAND-O  =	asn_ext.o

CFILES	=	$(HAND-C) $(PESPY-C)
OFILES	=	$(HAND-O) $(PESPY-O)

PEPSY   =	$(TOPDIR)pepsy/xpepsy

libx500as:	libx500as.a

libx500as.a:	x500asvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			x500asvrsn.o
		-@rm -f $(UPDIR)libx500as.a $(UPDIR)llib-lx500as
		-@ln libx500as.a $(UPDIR)libx500as.a
		-@$(LN) llib-lx500as $(UPDIR)llib-lx500as
		-@ls -l $@
		-@echo "QUIPU X500 Abstract Syntax library built normally"

x500asvrsn.c:	PEPSY.O $(HAND-O)
		@$(UTILDIR)version.sh x500as > $@

l-libx500as:;	$(LINT) $(LFLAGS) $(CFILES) $(PEPY-C) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


#######################################################
# saber
#######################################################

saber_src:;	#load -C $(OPTIONS) $(CFILES) 

saber_obj:;	#load libx500as.a

#######################################################
# asn
#######################################################

PEPSY.O:	PEPSY.C $(PESPY-O)
		touch PEPSY.O

PEPSY.C:	DSE.ph $(PESPY-C)
		touch PEPSY.C

# Eek do.py needs DAS.ph, but das.py needs DO.ph
# Need to resolve this somehow 

IF-types.h IF_tables.c: if.py if-cdefs.h $(PEPSY)
		$(PEPSY) -i "if-cdefs.h" -A -m $(PYFLAGS) if.py
		-@rm -f $(TOPDIR)h/quipu/IF*.h 
		-@$(LN) IF-types.h $(TOPDIR)h/quipu/IF-types.h

AF-types.h AF_tables.c: af.py af-cdefs.h $(PEPSY)
		$(PEPSY) -i "af-cdefs.h" -A -m $(PYFLAGS) af.py
		-@rm -f $(TOPDIR)h/quipu/AF*.h 
		-@$(LN) AF-types.h $(TOPDIR)h/quipu/AF-types.h

DO-types.h DO_tables.c: do.py do-cdefs.h $(PEPSY)
	   	$(PEPSY) -i "do-cdefs.h" -A -m $(PYFLAGS) do.py
		-@rm -f $(TOPDIR)h/quipu/DO*.h 
		-@$(LN) DO-types.h $(TOPDIR)h/quipu/DO-types.h
	   	@echo "*** You can ignore WARNINGs about DAS.ph/2.5.1.2.ph ***"

DAS-types.h DAS_tables.c: das.py das-cdefs.h $(PEPSY)
	   	$(PEPSY) -i "das-cdefs.h" -A -m $(PYFLAGS) das.py
		-@rm -f $(TOPDIR)h/quipu/DAS*.h
		-@$(LN) DAS-types.h $(TOPDIR)h/quipu/DAS-types.h

Quipu-types.h Quipu_tables.c: qu.py qu-cdefs.h $(PEPSY)
	   	$(PEPSY) -i "qu-cdefs.h" -A -m $(PYFLAGS) qu.py
		-@rm -f $(TOPDIR)h/quipu/Quipu*.h 
		-@$(LN) Quipu-types.h $(TOPDIR)h/quipu/Quipu-types.h

SA-types.h SA_tables.c: sa.py if-cdefs.h $(PEPSY)
	   	$(PEPSY) -i "if-cdefs.h" -A -m $(PYFLAGS) sa.py
		-@rm -f $(TOPDIR)h/quipu/SA*.h 
		-@$(LN) SA-types.h $(TOPDIR)h/quipu/SA-types.h

Thorn-types.h Thorn_tables.c: th.py if-cdefs.h $(PEPSY)
	   	$(PEPSY) -i "if-cdefs.h" -A -m $(PYFLAGS) th.py
		-@rm -f $(TOPDIR)h/quipu/Thorn*.h 
		-@$(LN) Thorn-types.h $(TOPDIR)h/quipu/Thorn-types.h

NRS-types.h NRS_tables.c: nrs.py nrs-cdefs.h $(PEPSY)
	   	$(PEPSY) -i "nrs-cdefs.h" -A -m $(PYFLAGS) nrs.py
		-@rm -f $(TOPDIR)h/quipu/NRS*.h 
		-@$(LN) NRS-types.h $(TOPDIR)h/quipu/NRS-types.h

DSE.ph:		$(TOPDIR)acsap/DSE.ph
		-@rm -f DSE.ph
		-$(LN) $(TOPDIR)acsap/DSE.ph DSE.ph
		-@rm -f DSE*.h
		-$(LN) $(TOPDIR)acsap/DSE-types.h DSE-types.h

#######################################################
# install headers
#######################################################

HEADERS = IF.ph IF-types.h \
	  AF.ph AF-types.h \
	  DAS.ph DAS-types.h \
	  DO.ph DO-types.h \
	  SA.ph SA-types.h \
	  NRS.ph NRS-types.h \
	  Quipu.ph Quipu-types.h \
	  Thorn.ph Thorn-types.h \
	  af-cdefs.h do-cdefs.h if-cdefs.h qu-cdefs.h \
	  das-cdefs.h nrs-cdefs.h

inst-headers:;	-mkdir $(PEPSYDIRM)
		@for h in $(HEADERS) ; \
		     do $(MAKE) TARGET=$$h inst-target; done

inst-target:	$(PEPSYDIR)$(TARGET)

$(PEPSYDIR)$(TARGET):	$(TARGET)
		-cp $@ z$(TARGET)
		cp $(TARGET) $@
		-@ls -gls $@
		-@echo ""

#######################################################
# clean
#######################################################

clean:;         rm -f *.ph *.o *.a a.out _* x* z* *.orig core x500asvrsn.c
		rm -f AF* IF* DAS* DO* Quipu* Thorn* SA* NRS* DSE* PEPSY*
		rm -f $(TOPDIR)h/quipu/IF*.h
		rm -f $(TOPDIR)h/quipu/AF*.h
		rm -f $(TOPDIR)h/quipu/DO*.h
		rm -f $(TOPDIR)h/quipu/DAS*.h
		rm -f $(TOPDIR)h/quipu/SA*.h
		rm -f $(TOPDIR)h/quipu/Quipu*.h
		rm -f $(TOPDIR)h/quipu/Thorn*.h
		rm -f $(TOPDIR)h/quipu/NRS*.h

grind:;         iprint Makefile
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs 
		tgrind -lc $(CFILES) llib-lx500as

true:;


# DO NOT DELETE THIS LINE
# Dependencies follow
AF_tables.o: ./AF-types.h
AF_tables.o: ./IF-types.h
AF_tables.o: ./af-cdefs.h
AF_tables.o: ./if-cdefs.h
DAS_tables.o: ./AF-types.h
DAS_tables.o: ./DAS-types.h
DAS_tables.o: ./DO-types.h
DAS_tables.o: ./IF-types.h
DAS_tables.o: ./af-cdefs.h
DAS_tables.o: ./das-cdefs.h
DAS_tables.o: ./do-cdefs.h
DAS_tables.o: ./if-cdefs.h
DO_tables.o: ./AF-types.h
DO_tables.o: ./DAS-types.h
DO_tables.o: ./DO-types.h
DO_tables.o: ./IF-types.h
DO_tables.o: ./af-cdefs.h
DO_tables.o: ./das-cdefs.h
DO_tables.o: ./do-cdefs.h
DO_tables.o: ./if-cdefs.h
IF_tables.o: ../../h/quipu/malloc.h
IF_tables.o: ./IF-types.h
IF_tables.o: ./if-cdefs.h
NRS_tables.o: ./NRS-types.h
NRS_tables.o: ./nrs-cdefs.h
Quipu_tables.o: ./AF-types.h
Quipu_tables.o: ./IF-types.h
Quipu_tables.o: ./Quipu-types.h
Quipu_tables.o: ./af-cdefs.h
Quipu_tables.o: ./if-cdefs.h
Quipu_tables.o: ./qu-cdefs.h
SA_tables.o: ./IF-types.h
SA_tables.o: ./SA-types.h
SA_tables.o: ./if-cdefs.h
Thorn_tables.o: ./IF-types.h
Thorn_tables.o: ./Thorn-types.h
Thorn_tables.o: ./if-cdefs.h
asn_ext.o: ../../h/config.h
asn_ext.o: ../../h/general.h
asn_ext.o: ../../h/isoaddrs.h
asn_ext.o: ../../h/logger.h
asn_ext.o: ../../h/manifest.h
asn_ext.o: ../../h/psap.h
asn_ext.o: ../../h/quipu/abandon.h
asn_ext.o: ../../h/quipu/add.h
asn_ext.o: ../../h/quipu/attr.h
asn_ext.o: ../../h/quipu/attrvalue.h
asn_ext.o: ../../h/quipu/authen.h
asn_ext.o: ../../h/quipu/bind.h
asn_ext.o: ../../h/quipu/common.h
asn_ext.o: ../../h/quipu/commonarg.h
asn_ext.o: ../../h/quipu/compare.h
asn_ext.o: ../../h/quipu/config.h
asn_ext.o: ../../h/quipu/dap.h
asn_ext.o: ../../h/quipu/ds_error.h
asn_ext.o: ../../h/quipu/ds_search.h
asn_ext.o: ../../h/quipu/dsargument.h
asn_ext.o: ../../h/quipu/dsp.h
asn_ext.o: ../../h/quipu/entry.h
asn_ext.o: ../../h/quipu/list.h
asn_ext.o: ../../h/quipu/modify.h
asn_ext.o: ../../h/quipu/modifyrdn.h
asn_ext.o: ../../h/quipu/name.h
asn_ext.o: ../../h/quipu/oid.h
asn_ext.o: ../../h/quipu/read.h
asn_ext.o: ../../h/quipu/remove.h
asn_ext.o: ../../h/quipu/turbo.h
asn_ext.o: ../../h/quipu/util.h
asn_ext.o: ./AF-types.h
asn_ext.o: ./IF-types.h
asn_ext.o: ./Quipu-types.h
asn_ext.o: ./af-cdefs.h
asn_ext.o: ./if-cdefs.h
asn_ext.o: ./qu-cdefs.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
