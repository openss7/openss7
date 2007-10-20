###############################################################################
#   Instructions to Make, for compilation of ISODE processes for SunOS
#	release 4
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/config/RCS/xtel-noshd.make,v 9.0 1992/06/16 12:08:13 isode Rel $
#
#
# $Log: xtel-noshd.make,v $
# Revision 9.0  1992/06/16  12:08:13  isode
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
# Options
###############################################################################

OPTIONS	=	-I. -I$(TOPDIR)h $(PEPYPATH)

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/local/bin/
SBINDIR	=	/usr/local/etc/
ETCDIR	=	/usr/local/etc/
LOGDIR	=	/usr/tmp/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/local/lib/
LINTDIR	=	/usr/local/lib/lint/

SYSTEM	=	-bsd42
MANDIR	=	/usr/local/man/
MANOPTS	=	-bsd42


###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc
# -O loses...
CFLAGS	=	$(OPTIONS) $(LOPTIONS) -O
LIBCFLAGS  =	$(CFLAGS)
LINT    =	lint
LFLAGS  =	-bhuz $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
LDFLAGS =	#-s
ARFLAGS	=

LN	=	ln

SHAREDLIB= 	

# native to 4.2BSD UNIX
LSOCKET	=
LIBISODE= -L $(TOPDIR) -lisode
LIBDSAP=  -L $(TOPDIR) -ldsap 

#LIBDSAP=  -L /xtel/pp/pp-beta/Chans/lists -ldl \
#          -L /xtel/pp/pp-beta/Lib -lpp \
#	  -L $(TOPDIR) -ldsap 

###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c

