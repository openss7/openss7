#
# common makefile for all environments
#
# This file is included from a target-specific makefile
# in a subdirectory below this directory.
# The makefile including this should have defined:
#   - The LiS configuration.
#     config.mk in the top of the source tree should
#     have been included.
#   - DRVRS variable.
#     Contains the names of the driver object files that are known
#     to all targets and should be linked into drivers.o.
#   - TDRVRS variable.
#     Contains the names of the driver object files that are only known
#     to this target and should be linked into drivers.o.
#   - CC variable.
#     Describes how to compile
#

#
# Targets and rules
#
default: all
	$(nothing)

all: $(Q_CC) drivers.o
	$(nothing)

drivers.o: $(DRVRS) $(TDRVRS)
	$(Q_ECHO) $(qtag_LD)$(relpwdtarget)
	$(Q)$(LD) -r -o $@ $^

# Compiling a target-specific object ( $(TDRVRS) )
%.o: %.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -DLIS_OBJNAME=$* -o $@ -c $<

# Compiling a target-independent object ( $(DRVRS) )
%.o: $(DRVRDIR)/%.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -DLIS_OBJNAME=$* -o $@ -c $<

%.E: $(DRVRDIR)/%.c
	$(Q)$(CC) -E $^ >$@

%.E: %.c
	$(Q)$(CC) -E $^ >$@


common-clean:
	-$(Q)rm -f $(DRVRS)
	-$(Q)rm -f $(TDRVRS)
	-$(Q)rm -f drivers.o .compiler

common-realclean: clean
	-$(Q)rm -f .depend

common-install: .compiler
	$(Q)install -d $(DESTDIR)$(pkgdatadir)/$(relpwd)
	$(Q)cat .compiler | \
	    sed "s:^:CC=\(:" | sed "s:$$:\):" | \
	    sed "s:$(SRCDIR)/include:$$\{pkgincludedir\}:g" | \
	    sed "s:$(SRCDIR):$$\{pkgdatadir\}:g" | \
	    sed "s:$(KINCL):$$\{KINCL\}:g" | \
	    sed "s:$(KSRC):$$\{KSRC\}:g" | \
	    cat > $(DESTDIR)$(pkgdatadir)/$(relpwd)/.compiler

common-uninstall:
	$(nothing)


# the following relates to the Q_CC variable, which may be set to .compiler if
# this target's output is desired
#
.compiler:
	$(Q_ECHO) $(qtag__WD_)$(relpwd)
	$(Q_ECHO) $(qtag__CC_)$(CC)
	@echo $(CC) > $@


#
# Dependency stuff
#

-include .depend

common-dep:
ifneq ($(strip $(DRVRS) $(TDRVRS)),)
	$(Q)$(CC) -M -DDEP $(DRVRS:%.o=$(DRVRDIR)/%.c) $(TDRVRS:%.o=%.c) >.depend
else
	$(Q)>.depend
endif

FORCE:
