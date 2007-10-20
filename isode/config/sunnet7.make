###############################################################################
#   Instructions to Make, for compilation of ISODE processes for
#	SunLink OSI and X.25 7.0 on SunOS 4
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/config/RCS/sunnet7.make,v 9.0 1992/06/16 12:08:13 isode Rel $
#
#
# $Log: sunnet7.make,v $
# Revision 9.0  1992/06/16  12:08:13  isode
# Release 8.0
#
#
##############################################################################

##############################################################################
#
#				 NOTICE
#
#    Acquisition, use, and distribution of this module and related
#    materials are subject to the restrictions of a license agreement.
#    Consult the Preface in the User's Manual for the full terms of
#    this agreement.
#
###############################################################################

#    See sunos4.make for shared library options

###############################################################################
# Options
###############################################################################

OPTIONS	=	-I. -I$(TOPDIR)h -I/usr/sunlink/osi/include $(PEPYPATH) $(KRBOPT)

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/isode/local/bin/
SBINDIR	=	/usr/isode/etc/
ETCDIR	=	/usr/isode/etc/
LOGDIR	=	/usr/isode/tmp/
INCDIRM	=	/usr/isode/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/isode/lib/
LINTDIR	=	/usr/isode/lib/lint/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-bsd42
MANDIR	=	/usr/isode/man/
MANOPTS	=	-bsd42


###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc
# -O loses...
CFLAGS  =	      $(OPTIONS)
LIBCFLAGS=	      $(CFLAGS)
LINT    =	lint
LFLAGS  =	-bhuz $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
LDFLAGS =	-s -L/usr/sunlink/osi/lib
ARFLAGS	=

LN	=	ln

LSOCKET	=	-losi $(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

# -x may be harmful on earlier releases of SunOS, your mileage may vary...

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
		-ld -x -r $@
		mv a.out $@
