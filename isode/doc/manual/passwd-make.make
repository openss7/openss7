#   Instructions to Make, for compilation of ROS-based password lookup service

BINDIR	=	/usr/new/
SBINDIR	=	/usr/etc/
ETCDIR	=	/usr/etc/

CC	=	/bin/cc
CFLAGS	=	-g    $(OPTIONS)
LINT	=	lint
LFLAGS	=	-bhuz $(OPTIONS)
LLIBS   =	-lisode
LD	=	ld
LDCC	=	/bin/cc
LDFLAGS	=	-g
LIBES	=	-lisode

HFILES	=	ryinitiator.h ryresponder.h
CFILES	=	lookupd.c lookup.c ryinitiator.c ryresponder.c
RYFILES	=	lookup.ry


# Here it is...

all:		lookupd lookup
inst-all:	inst-lookupd inst-lookup # manuals
install:	inst-all clean
lint:		l-lookupd l-lookup


# lookupd

inst-lookupd:	$(SBINDIR)ros.lookup

$(SBINDIR)ros.lookup:	lookupd
		-cp $@ zros.lookup
		-rm -f $@
		cp lookupd $@
		-@ls -gls $@
		-@echo ""

lookupd:	lookupd.o PasswordLookup-Rops.o \
			ryresponder.o PasswordLookup_tables.o
		$(LDCC) $(LDFLAGS) -o $@ lookupd.o \
			PasswordLookup-Rops.o \
			ryresponder.o 
			PasswordLookup_tables.o \
			$(LIBES)

l-lookupd:	PasswordLookup-ops.c PasswordLookup_tables.c true
		$(LINT) $(LFLAGS) -DPERFORMER lookupd.c \
			PasswordLookup-ops.c ryresponder.c \
			PasswordLookup_tables.c $(LLIBS)

lookupd.o:	ryresponder.h PasswordLookup-ops.h \
			PasswordLookup-types.h

PasswordLookup-Rops.o:	PasswordLookup-ops.c PasswordLookup-ops.h
		$(CC) $(CFLAGS) -DPERFORMER -c PasswordLookup-ops.c
		mv PasswordLookup-ops.o $@

ryresponder.o:	ryresponder.h

PasswordLookup_tables.o: PasswordLookup_tables.c PasswordLookup-types.h

PasswordLookup_tables.c PasswordLookup-types.h:	PasswordLookup-asn.py
		pepsy -A -f -h -m PasswordLookup-asn.py

PasswordLookup-asn.py:	lookup.ry
		rosy -m -pepsy -o $@ lookup.ry
PasswordLookup-ops.c:	lookup.ry
PasswordLookup-ops.h:	lookup.ry
PasswordLookup-stubs.c:	lookup.ry


# lookup

inst-lookup:	$(BINDIR)lookup

$(BINDIR)lookup:	lookup
		-cp $@ zlookup
		-rm -f $@
		cp lookup $@
		-@ls -gls $@
		-@echo ""

lookup:		lookup.o PasswordLookup-Iops.o \
			ryinitiator.o PasswordLookup_tables.o
		$(LDCC) $(LDFLAGS) -o $@ lookup.o \
			PasswordLookup-Iops.o \
			ryinitiator.o \
			PasswordLookup_tables.o \
			$(LIBES)

l-lookup:	PasswordLookup-ops.c PasswordLookup_tables.c true
		$(LINT) $(LFLAGS) -DINVOKER lookup.c \
			PasswordLookup-ops.c ryinitiator.c \
			PasswordLookup-stubs.c \
			PasswordLookup_tables.c $(LLIBS)

lookup.o:	ryinitiator.h PasswordLookup-ops.h PasswordLookup-types.h

PasswordLookup-Iops.o:	PasswordLookup-ops.c PasswordLookup-ops.h
		$(CC) $(CFLAGS) -DINVOKER -c PasswordLookup-ops.c
		mv PasswordLookup-ops.o $@

ryinitiator.o:	ryinitiator.h


# manual pages

MANUALS	=	lookupd.8c lookup.1c

manuals:;	install -c -m 0444 lookupd.8c \
			/usr/man/man8/lookupd.8c
		install -c -m 0444 lookup.1c /usr/man/mann/lookup.n


# clean

clean:;		rm -f *.o PasswordLookup* lookupd lookup z* _* core

grind:;		iprint READ-ME Makefile
		tgrind -lc $(HFILES) $(CFILES)
		tgrind -lpepy -d ../../pepy/grindefs $(RYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
