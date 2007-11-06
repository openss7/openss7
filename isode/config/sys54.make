###############################################################################
#   Instructions to Make, for compilation of ISODE processes for generic SVR4
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/config/RCS/sys54.make,v 9.0 1992/06/16 12:08:13 isode Rel $
#
#
# $Log: sys54.make,v $
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
#
# Donated to ISODE by X-Tel Services Ltd
#
# Based on ICL DRS/NX 6000 Version 4.0  Level 4  Increment 4
#
###############################################################################

###############################################################################
# Options
###############################################################################

OPTIONS	=	-I. -I$(TOPDIR)h $(PEPYPATH) $(KRBOPT)

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/local/bin/
SBINDIR	=	/usr/etc/
ETCDIR	=	/usr/etc/
LOGDIR	=	/usr/tmp/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/lib/
LINTDIR	=	/usr/lib/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-sys5r4
MANDIR	=	/usr/man/
MANOPTS	=	-sys5


###############################################################################
# Shared libraries
###############################################################################

#    You can build the ISODE and DSAP libraries dynamically.
#    This option is not the default as its portability is questionable.
#    We advise you build statically first, then when that works, you
#    may wish to try the shared library options.
#
#    If you have changed LIBDIR above, shared libraries may not work.

#    First, comment out the definitions of LIBISODE and LIBDSAP above

#    Second, uncomment these three lines:

#SHAREDLIB=      shared
#LIBISODE=	-L$(TOPDIR) -lisode -lm 
#LIBDSAP=	-L$(TOPDIR) -ldsap

#    Third, add
#		-K PIC
#    to LIBCFLAGS below

#    You will need to install the libraries before the binaries will run.

###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

# For generic Sys5 R4 use /usr/bin/cc and /usr/bin/ld explicitly, 
# otherwise you may pick up /usr/ucb/cc.  If you don't have SVR4_UCB
# defined that would be a complete flop!
#
# If SVR4_UCB is defined you'll need to use /usr/ucb/cc and /usr/ucb/ld
#
CC      =	/usr/bin/cc
LD	=	/usr/bin/ld
CFLAGS  =       $(OPTIONS) $(LOPTIONS) 
LIBCFLAGS=      $(CFLAGS)
LINT    =	/usr/bin/lint
LFLAGS  =	-s $(OPTIONS)
LDCC	=	$(CC)
LDFLAGS =
ARFLAGS	=

LN	=	ln

# You won't need -lsocket or -nsl if you have SVR4_UCB defined.
LSOCKET	=	-lsocket -lnsl $(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
