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
CC += -DLiS -D__KERNEL__ -DVERSION_2 $(CCREGPARM) $(XOPTS) $(OPT)

#
# The object files that need to be made for all targets
#
OBJS = head.o dki.o msg.o mod.o buffcall.o mdep.o events.o \
	msgutl.o queue.o safe.o stats.o stream.o strmdbg.o wait.o \
	cmn_err.o version.o

ifeq ($(LIS_TARG),linux)
OBJS += osif.o
endif

# strmdbg always needs -DLIS_SRC; it's usually only passed if DBG_OPT=y
#
common-dep strmdbg.o strmdbg.c:  XOPTS += -DLIS_SRC=\"$(SRCDIR)\"

#
# All we want is a streams head object file
#
all: $(Q_CC) streamshead.o
	$(nothing)

streamshead.o: $(OBJS) $(XOBJS) $(ASMOBJS)
	$(Q_ECHO) $(qtag_LD)$(relpwdtarget)
	$(Q)$(LD) -r -o $@ $^

#
# Some rules to make object files
#
%.o: $(HEADDIR)/%.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -c -o $@ $<

%.o: %.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -c -o $@ $<

%.o: $(HEADDIR)/%.S
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -c -o $@ $<

%.o: %.S
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -c -o $@ $<


install: .compiler
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
	$(Q)install -d $(DESTDIR)$(pkgdatadir)/$(relpwd)
	$(Q)cat .compiler | \
	    sed "s:^:CC=\(:" | sed "s:$$:\):" | \
	    sed "s:$(SRCDIR)/include:$$\{pkgincludedir\}:g" | \
	    sed "s:$(SRCDIR):$$\{pkgdatadir\}:g" | \
	    sed "s:$(KINCL):$$\{KINCL\}:g" | \
	    sed "s:$(KSRC):$$\{KSRC\}:g" | \
	    cat > $(DESTDIR)$(pkgdatadir)/$(relpwd)/.compiler

uninstall:
	$(nothing)


# the following relates to the Q_CC variable, which may be set to .compiler if
# this target's output is desired
#
.compiler:
	$(Q_ECHO) $(qtag__WD_)$(relpwd)
	$(Q_ECHO) $(qtag__CC_)$(CC)
	@echo $(CC) > $@


realclean: clean
	-$(Q)rm -f .depend

clean: 
	$(Q_ECHO) $(qtag_CLEAN)$(relpwd)
	-$(Q)rm -f streamshead.o $(OBJS) $(XOBJS) $(ASMOBJS) .compiler

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
	$(Q)$(CC) -E $^ >$@

%.E: %.c
	$(Q)$(CC) -E $^ >$@


#
# Dependencies creation
#
-include .depend

common-dep:
	$(Q)$(CC) -M -DDEP$(ASMOBJS:%.o=$(HEADDIR)/%.S) $(OBJS:%.o=$(HEADDIR)/%.c)  >.depend


