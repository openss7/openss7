#############################################################################
#
# Makefile for libQuery.a
#
#############################################################################

LLIBS	=	$(TOPDIR)llib-ldsap $(TOPDIR)llib-lisode

CFILES  =       attrt_list.c \
                bind.c \
                ditmodel.c \
                dn_list.c \
                ds_tasks.c \
                ds_util.c \
                error.c \
                init.c \
                modify.c \
                read.c \
                request.c \
                sequence.c \
                ufname.c \
                ufsearch.c \
                util.c

OFILES  =	attrt_list.o \
                bind.o \
                ditmodel.o \
                dn_list.o \
                ds_tasks.o \
                ds_util.o \
                error.o \
                init.o \
                modify.o \
                read.o \
		request.o \
                sequence.o \
                ufname.o \
                ufsearch.o \
                util.o


INCLUDE =	-I$(TOPDIR)h

#ARFLAGS	=	q

all:	libQuery.a

depend:;	makedepend $(INCLUDE) $(CFILES)

libQuery.a:	$(OFILES)
	-rm -f $@
	@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) 

clean:;	      rm -f *.o *.a

lint:
		$(LINT) $(LFLAGS) $(OPTIONS) $(INCQUERY) $(CFILES) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

grind:

# DO NOT DELETE THIS LINE -- make depend depends on it.

