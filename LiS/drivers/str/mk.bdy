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

drivers.o: $(DRVRS) $(TDRVRS)
	$(LD) -r -o $@ $^

# Compiling a target-specific object ( $(TDRVRS) )
%.o: %.c
	$(CC) -DLIS_OBJNAME=$* -c -o $@ $<

# Compiling a target-independent object ( $(DRVRS) )
%.o: $(DRVRDIR)/%.c
	$(CC) -DLIS_OBJNAME=$* -c -o $@ $<

%.E: $(DRVRDIR)/%.c
	$(CC) -E $^ >$@
%.E: %.c
	$(CC) -E $^ >$@

common-clean:
	-rm -f $(DRVRS)
	-rm -f $(TDRVRS)
	-rm -f drivers.o

common-realclean: clean
	-rm -f .depend

#
# Dependency stuff
#

-include .depend

common-dep:
ifneq ($(strip $(DRVRS) $(TDRVRS)),)
	$(CC) -M -DDEP $(DRVRS:%.o=$(DRVRDIR)/%.c) $(TDRVRS:%.o=%.c) >.depend
else
	>.depend
endif

