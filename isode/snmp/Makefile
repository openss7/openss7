###############################################################################
#   Instructions to Make, for compilation of 4BSD/ISODE SNMP software
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/snmp/RCS/Makefile,v 9.0 1992/06/16 12:38:11 isode Rel $
#
# Contributed by NYSERNet Inc.  This work was partially supported by the
# U.S. Defense Advanced Research Projects Agency and the Rome Air Development
# Center of the U.S. Air Force Systems Command under contract number
# F30602-88-C-0016.
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:38:11  isode
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


PEPYPATH=	-DPEPYPATH

# If using gcc, define the following...
GCCFLAGS=	-fwritable-strings
# GCCFLAGS=

.SUFFIXES:	.py .c .o

.c.o:;		$(CC) $(CFLAGS) $(GCCFLAGS) -c $*.c


LIBD   =	libisnmp.a $(TOPDIR)libisode.a
LIBES   =	libisnmp.a $(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode
MIB-C	=	$(AE-C) smux-g.c \
			system.c interfaces.c routes.c ip.c icmp.c tcp.c \
			udp.c clns.c 
AE-C	=	mib.c snmp-g.c view-g.c eval.c
MIB-O	=	$(AE-O) smux-g.o \
			system.o interfaces.o routes.o ip.o icmp.o tcp.o \
			udp.o clns.o
AE-O	=	mib.o snmp-g.o view-g.o eval.o
CFILES	=	snmpd.c \
			unixd.c users.c print.c unixFs.c \
			snmpi.c objects.c syntax.c values.c \
			smux.c smuxentry.c $(MIB-C)
PYFILES	=	snmp.py
HFILES	=	$(HDIR)psap.h $(HDIR)manifest.h $(HDIR)general.h \
		$(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		snmpd snmpc snmpt unixd snmpi runcom peers objects
inst-all:	inst-snmpd inst-snmpt inst-unixd inst-snmpi \
			inst-runcom inst-peers inst-objects inst-headers \
			inst-libisnmp manuals # inst-snmpc
install:	inst-all clean
lint:		l-snmpd l-snmpc l-snmpt l-unixd l-snmpi


##################################################################
# snmpd
##################################################################

inst-snmpd:	$(SBINDIR)snmpd

$(SBINDIR)snmpd: xsnmpd
		-cp $@ zsnmpd
		-rm -f $@
		cp xsnmpd $@
		-@ls -gls $@
		-@echo ""

snmpd:		xsnmpd

xsnmpd:		snmpd.o $(MIB-O) $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ snmpd.o $(MIB-O) $(LIBES) \
			$(LNLIST) $(LSOCKET)

l-snmpd:	SNMP_tables.c true
		$(LINT) $(LFLAGS) snmpd.c $(MIB-C) \
			SNMP_tables.c objects.c syntax.c values.c \
			smux.c smuxentry.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

snmpd.o:	SNMP-types.h mib.h objects.h smux.h $(HFILES) $(HDIR)tailor.h \
			$(HDIR)logger.h \
			$(HDIR)tsap.h $(HDIR)isoaddrs.h \
			$(HDIR)dgram.h $(HDIR)internet.h $(HDIR)tp4.h \
			snmp-g.h view-g.h smux-g.h
$(MIB-O):	SNMP-types.h mib.h objects.h $(HFILES) $(HDIR)logger.h
system.o:	$(HDIR)tailor.h $(HDIR)logger.h
interfaces.o:	interfaces.h $(HDIR)internet.h clns.h
addrtrans.o:	interfaces.h $(HDIR)internet.h clns.h
ip.o:		interfaces.h $(HDIR)internet.h clns.h routes.h
icmp.o:		$(HDIR)internet.h
tcp.o:		$(HDIR)internet.h
udp.o:		$(HDIR)internet.h
clns.o:		interfaces.h $(HDIR)internet.h clns.h routes.h
snmp-g.o:	snmp-g.h
view-g.o:	view-g.h $(HDIR)isoaddrs.h $(HDIR)internet.h
smux-g.o:	smux-g.h $(HDIR)internet.h


##################################################################
# snmpc
##################################################################

inst-snmpc:	$(SBINDIR)snmpc

$(SBINDIR)snmpc: xsnmpc
		-cp $@ zsnmpc
		-rm -f $@
		cp xsnmpc $@
		-@ls -gls $@
		-@echo ""

snmpc:		xsnmpc

xsnmpc:		snmpc.o $(AE-O) $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ snmpc.o $(AE-O) $(LIBES) \
			$(LNLIST) $(LSOCKET)

l-snmpc:	SNMP_tables.c true
		$(LINT) $(LFLAGS) -DSNMPC snmpd.c $(AE-C) \
			SNMP_tables.c objects.c syntax.c values.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

snmpc.o:	SNMP-types.h mib.h objects.h smux.h $(HFILES) $(HDIR)tailor.h \
			$(HDIR)logger.h \
			$(HDIR)tsap.h $(HDIR)isoaddrs.h \
			$(HDIR)dgram.h $(HDIR)internet.h $(HDIR)tp4.h \
			snmpd.c
		$(CC) $(CFLAGS) -DSNMPC -o snmpc.o -c snmpd.c


##################################################################
# snmpt
##################################################################

inst-snmpt:	$(SBINDIR)snmpt

$(SBINDIR)snmpt: xsnmpt
		-cp $@ zsnmpt
		-rm -f $@
		cp xsnmpt $@
		-@ls -gls $@
		-@echo ""

snmpt:		xsnmpt

xsnmpt:		snmpt.o $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ snmpt.o $(LIBES) $(LSOCKET)

l-snmpt:	SNMP_tables.c true
		$(LINT) $(LFLAGS) -DSNMPT snmpd.c SNMP_tables.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

snmpt.o:	SNMP-types.h mib.h objects.h smux.h $(HFILES) $(HDIR)tailor.h \
			$(HDIR)logger.h \
			$(HDIR)tsap.h $(HDIR)isoaddrs.h \
			$(HDIR)dgram.h $(HDIR)internet.h $(HDIR)tp4.h \
			snmpd.c
		$(CC) $(CFLAGS) -DSNMPT -o snmpt.o -c snmpd.c


##################################################################
# unixd
##################################################################

inst-unixd:	$(SBINDIR)smux.unixd

$(SBINDIR)smux.unixd: xunixd
		-cp $@ zunixd
		-rm -f $@
		cp xunixd $@
		-@ls -gls $@
		-@echo ""

it:		xunixd
		xunixd

unixd:		xunixd

xunixd:		unixd.o users.o print.o printcap.o unixFs.o $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ unixd.o users.o print.o printcap.o \
			unixFs.o $(LIBES) $(LSOCKET)

l-unixd:	SNMP_tables.c true
		$(LINT) $(LFLAGS) unixd.c users.c print.c printcap.c \
			unixFs.c SNMP_tables.c objects.c syntax.c values.c \
			smux.c smuxentry.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

unixd.o:	SNMP-types.h objects.h smux.h $(HFILES) $(HDIR)tailor.h \
			$(HDIR)logger.h
users.o:	SNMP-types.h objects.h smux.h $(HFILES) $(HDIR)logger.h
print.o:	SNMP-types.h objects.h smux.h $(HFILES) $(HDIR)logger.h \
			pathnames.h
printcap.o:	pathnames.h
unixFs.o:	SNMP-types.h objects.h smux.h $(HFILES) $(HDIR)logger.h


##################################################################
# snmpi
##################################################################

inst-snmpi:	$(BINDIR)snmpi

$(BINDIR)snmpi:	xsnmpi
		-cp $@ zxsnmpi
		-rm -f $@
		cp xsnmpi $@
		-@ls -gls $@
		-@echo ""

snmpi:		xsnmpi

xsnmpi:		snmpi.o snmpb.o $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ snmpi.o snmpb.o $(LIBES) $(LSOCKET)

l-snmpi:	SNMP_tables.c true
		$(LINT) $(LFLAGS) snmpi.c snmpb.c SNMP_tables.c \
			objects.c syntax.c values.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

snmpi.o:	SNMP-types.h objects.h $(HFILES) $(HDIR)tailor.h \
			$(HDIR)logger.h \
			$(HDIR)tsap.h $(HDIR)isoaddrs.h \
			$(HDIR)dgram.h $(HDIR)internet.h $(HDIR)tp4.h
snmpb.o:	SNMP-types.h objects.h $(HFILES) $(HDIR)tailor.h \
			$(HDIR)logger.h


################################################################
# headers
################################################################

inst-headers:	$(INCDIR)snmp/objects.h $(INCDIR)snmp/smux.h \
			$(PEPSYDIR)SNMP-types.h $(PEPSYDIR)SNMP.ph

$(INCDIR)snmp/objects.h: objects.h
		-cp $@ zobjects.h
		cp objects.h $@
		-@ls -gls $@
		-@echo ""

$(INCDIR)snmp/smux.h: smux.h
		-cp $@ zsmux.h
		cp smux.h $@
		-@ls -gls $@
		-@echo ""

$(PEPSYDIR)SNMP-types.h: SNMP-types.h
		-cp $@ zSNMP-types.h
		cp SNMP-types.h $@
		-@ls -gls $@
		-@echo ""

$(PEPSYDIR)SNMP.ph: SNMP.ph
		-cp $@ zSNMP.ph
		cp SNMP.ph $@
		-@ls -gls $@
		-@echo ""


################################################################
# libisnmp
################################################################

inst-libisnmp:	$(LIBDIR)libisnmp.a $(LINTDIR)llib-lisnmp

$(LIBDIR)libisnmp.a:	libisnmp.a
		-rm -f $@
		cp libisnmp.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lisnmp:	llib-lisnmp
		-cp $@ zllib-lisnmp
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)snmp/\1"%' \
			< llib-lisnmp | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libisnmp.a:	SNMP_tables.o objects.o syntax.o values.o smux.o smuxentry.o \
			loadobjects.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(SNMP-O) \
			SNMP_tables.o objects.o syntax.o values.o smux.o \
			smuxentry.o loadobjects.o
		-@ls -l $@
		-@echo "SNMP library built normally"

SNMP_tables.o:	SNMP_tables.c SNMP-types.h

SNMP_tables.c SNMP-types.h: snmp.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m snmp.py

objects.o:	objects.h $(HFILES) $(HDIR)tailor.h $(HDIR)logger.h
syntax.o:	objects.h $(HFILES) $(HDIR)tailor.h $(HDIR)logger.h \
		$(HDIR)internet.h clns.h
values.o:	objects.h SNMP-types.h $(HDIR)logger.h
smux.o:		smux.h SNMP-types.h $(HDIR)tailor.h $(HDIR)logger.h \
		$(HDIR)internet.h
smuxentry.o:	smux.h SNMP-types.h $(HDIR)tailor.h
loadobjects.o:	objects.h


##################################################################
# files
##################################################################

inst-runcom:	$(ETCDIR)snmpd.rc

$(ETCDIR)snmpd.rc:	snmpd.rc
		if [ -s $@ ]; \
		    then exit 0; \
		    else cp snmpd.rc $@; ls -gls $@; \
		fi
		if ln $@ $@.old; \
		    then rm -f $@; cp snmpd.rc $@; ls -gls $@; \
		    else exit 0; \
		fi

runcom:;


inst-peers:	$(ETCDIR)snmpd.peers

$(ETCDIR)snmpd.peers:	snmpd.peers
		-cp $@ zsnmpd.peers
		-rm -f $@
		cp snmpd.peers $@
		-chmod 600 $@
		-@ls -gls $@
		-@echo ""

peers:;


DEFS	=	objects.defs snmpd.defs unixd.defs # snmpc.defs
DEFS2	=	smi.defs \
		    mib.defs \
			ifx.defs appletalk.defs ospf.defs bgp.defs rmon.defs \
			dot1d.defs \
			dot3.defs dot5.defs dot4.defs ds1.defs ds3.defs \
		    clns.defs fddi.defs lanmgr.defs view.defs \
			sip.defs ppp.defs char.defs rs232.defs \
			para.defs frame-relay.defs etherhub.defs \
		    bsd.defs unix.defs smux.defs \
		    eval.defs

inst-objects:;	-mkdir $(INCDIR)snmp
		@for d in $(DEFS); do $(MAKE) TARGET=$$d inst-target; done
		@for d in $(DEFS2); do $(MAKE) TARGET2=$$d inst-target2; done

inst-target:	$(ETCDIR)$(TARGET)

$(ETCDIR)$(TARGET):	$(TARGET)
		-cp $@ z$(TARGET)
		-rm -f $@
		cp $(TARGET) $@
		-@if [ "$(TARGET)" = "objects.defs" ]; then \
		    ./xsnmpi -f $@ compile $@; fi
		-@ls -gls $@
		-@echo ""

inst-target2:	$(INCDIR)snmp/$(TARGET2)

$(INCDIR)snmp/$(TARGET2):	$(TARGET2)
		-cp $@ z$(TARGET2)
		-rm -f $@
		cp $(TARGET2) $@
		-@ls -gls $@
		-@echo ""

objects:	$(DEFS)

objects.defs:	$(DEFS2)
		cat $(DEFS2) > $@

snmpd.defs:	smi.defs mib.defs clns.defs view.defs bsd.defs smux.defs \
		    eval.defs
		cat smi.defs mib.defs clns.defs view.defs bsd.defs smux.defs \
		    eval.defs > $@

snmpc.defs:	smi.defs view.defs bsd.defs smux.defs eval.defs
		cat smi.defs view.defs bsd.defs smux.defs eval.defs > $@

unixd.defs:	smi.defs bsd.defs unix.defs
		cat smi.defs bsd.defs unix.defs > $@

loadobjects.c:  $(DEFS2)
		$(MAKE) objects.defs
		./xsnmpi -f objects.defs compile -s loadobjects


################################################################
# manual pages
################################################################

MANUALS	=	snmpd.8c snmpc.8c snmpt.8c unixd.8c snmpi.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a SNMP* x* z* _* core

grind:;		iprint Makefile
		tgrind -lc $(CFILES)
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES) $(DEFS2)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
