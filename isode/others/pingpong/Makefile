###############################################################################
#   Instructions to Make, for compilation of ISODE xSAP processes
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/pingpong/RCS/Makefile,v 9.0 1992/06/16 12:43:14 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:43:14  isode
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

.c.o:;	$(CC) $(CFLAGS) -c $*.c


LIBES	=	$(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode

all:		pingpong
inst-all:	
install:	
lint:		l-pingpong

pingpong:	xpingpong

xpingpong:	pingpong.o
		$(LDCC) -o $@ pingpong.o $(LIBES) $(LSOCKET)

l-pingpong:;	$(LINT) $(LFLAGS) pingpong.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

clean:;		rm -f x* z* _* *.o core
