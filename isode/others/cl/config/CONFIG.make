###############################################################################
#   Instructions to Make, for compilation of ISODE processes under SYS5
#	release 2 with EXOS 8044
###############################################################################

###############################################################################
#
# $Header: /f/iso/config/RCS/sys52-exos.make,v 5.0 88/07/21 14:31:23 mrose Rel $
#
#
# $Log:	sys52-exos.make,v $
# Revision 5.0  88/07/21  14:31:23  mrose
# Release 4.0
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

# HULA change -I/usr/include/EXOS to the following
EXOSINC =	/usr/src/EXOS/include/EXOS
OPTIONS	=	-I$(TOPDIR)h -I$(EXOSINC) -I/usr/include -I/usr/include/HULA
DOPTIONS=	-DDEBUG -DTESTDEBUG -DHULA -Di386 

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/local/bin/
ETCDIR	=	/etc/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
LIBDIR	=	/usr/lib/
LINTDIR	=	/usr/lib/lint/

SYSTEM	=	-sys5
MANOPTS	=	-sys5


###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc
#CFLAGS  =	-O	-DHULA	-Di386	-Uu3b    $(OPTIONS)
CFLAGS  =	-O -g	$(DOPTIONS) -Uu3b $(OPTIONS)
LINT    =	lint
LFLAGS  =	-bhuz $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
#LDFLAGS =	-ns
LDFLAGS = 	
ARFLAGS	=

# EXOS 8044 TCP/IP software
LSOCKET	=	-lsocket


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(CFLAGS) -c $*.c
