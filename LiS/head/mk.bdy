############################ -*- Mode: Makefile -*- ###########################
## Makefile --- makefile for LiS STREAM head
## Author          : Francisco J. Ballesteros Camara
## Created On      : Mon Oct 17 11:41:20 1994
## Last Modified By: Ole Husgaard <sparre@login.dknet.dk>
###############################################################################
#
# This makefile is included from the makefile in the subdirectory
# below this directory for the particular target environment.
#
# The subdirectories are:
#
#	./linux		Linux kernel version
#	./user		User level testing version
#	./qnx		QNX kernel version
#
# The makefile including this file should have defined:
#   - The LiS configuration.
#     config.mk in the top of the source tree should have been included.
#   - OPT variable.
#     Contains target-specific compiler options. This should also contain
#     -DLINUX, -DUSER or -DQNX, depending on the target.
#   - XOBJS variable.
#     Contains the names of target specific object files that should be
#     linked into streamshead.o.
#

# How to compile
CC += -DLiS -D__KERNEL__ -DVERSION_2 $(XOPTS) $(OPT)

#
# The object files that need to be made for all targets
#
OBJS = head.o dki.o msg.o mod.o buffcall.o mdep.o events.o \
	msgutl.o queue.o safe.o stats.o stream.o strmdbg.o wait.o \
	cmn_err.o slog.o sputbuf.o version.o

ifeq ($(LIS_TARG),linux)
OBJS += osif.o
endif

#
# All we want is a streams head object file
#
all: streamshead.o

clean: 
	-rm -f streamshead.o $(OBJS) $(XOBJS) $(ASMOBJS)

realclean: clean
	-rm -f .depend

#
# Some rules to make object files
#
%.o: $(HEADDIR)/%.c
	$(CC) -c -o $@ $<

%.o: %.c
	$(CC) -c -o $@ $<

%.o: $(HEADDIR)/%.S
	$(CC) -c -o $@ $<

%.o: %.S
	$(CC) -c -o $@ $<

streamshead.o: $(OBJS) $(XOBJS) $(ASMOBJS)
	$(LD) -r -o $@ $^

#
# $(HEADDIR)/modconf.inc is created by the configuration and included
# by $(HEADDIR)/mod.c. It may not exist when "make dep" is done, so it is
# not included when "make dep" is done.
# But then we have to add the dependency here.
#
mod.o: $(HEADDIR)/modconf.inc


#
# A special service to people struggling with cpp.
# This is not used by default.
#
%.E: $(HEADDIR)/%.c
	$(CC) -E $^ >$@

%.E: %.c
	$(CC) -E $^ >$@


#
# Dependencies creation
#
-include .depend

common-dep:
	$(CC) -M -DDEP$(ASMOBJS:%.o=$(HEADDIR)/%.S) $(OBJS:%.o=$(HEADDIR)/%.c)  >.depend


