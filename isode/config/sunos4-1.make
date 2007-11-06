###############################################################################
#   Instructions to Make, for compilation of ISODE processes for SunOS
#	release 4.1
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/config/RCS/sunos4-1.make,v 9.0 1992/06/16 12:08:13 isode Rel $
#
#
# $Log: sunos4-1.make,v $
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

OPTIONS	=	-I. -I$(TOPDIR)h $(PEPYPATH) $(KRBOPT)

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/local/bin/
SBINDIR	=	/usr/etc/
ETCDIR	=	/usr/etc/
LOGDIR	=	/var/tmp/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/local/lib/
LINTDIR	=	/usr/local/lib/lint/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-bsd42
MANDIR	=	/usr/local/man/
MANOPTS	=	-bsd42


###############################################################################
# Shared libraries
###############################################################################

#    You can build a shared version of the ISODE library on suns under
#    SunOS 4.0 or greater. For non sparc based architecture, any
#    release greater than 4.0 will do. However, due to a bug in the
#    compiler and the size of ISODE, you will not be able to build a
#    shared ISODE unless you have SunOS 4.1 or greater.

#    First, comment out the definitions of LIBISODE and LIBDSAP above

#    Second, uncomment these three lines:

#SHAREDLIB=	shared
#LIBISODE=	-L$(TOPDIR) -lisode
#LIBDSAP=	-L$(TOPDIR) -ldsap

#    If you are not installing the libraries in the standard place
#    (/usr/lib or /usr/local/lib) you should add a "-L$(LIBDIR)" to
#    the above two lines.

#    Third, add
#		-pic		Sun 3
#		-PIC		Sparc
#    to LIBCFLAGS below

#    Finally, remove
#		-ld -x -r $@
#		mv a.out $@
#    from the .c.o rule below.

#    Having compiled and installed ISODE, you may need to run
#    /usr/etc/ldconifg to get the system to pick up the new
#    shared libraries.

###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc
CFLAGS  =	-g    $(OPTIONS)
LIBCFLAGS=	      $(CFLAGS)
LINT    =	lint
LFLAGS  =	-bhuz $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
LDFLAGS =	-g
ARFLAGS	=

LN	=	ln

LSOCKET	=	$(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
		-ld -x -r $@
		mv a.out $@
