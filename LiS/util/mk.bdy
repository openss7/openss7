#
# This is the common part of the makefiles in the util directory.
#
# "make all" is the standard thing to do to make the utility programs.
# "make install" will install them in $(INST_SBIN)
# "make uninstall" will remove them from $(INST_SBIN)
# "make clean" to clean up the object code
#
# Targets install and uninstall do nothing for user-land utilities.
#
# Invoking makefile needs to include $(LIS_HOME)/config.mk and define:
#
# OPT         Target specific compiler options
# INST_SBIN   where to install the utilities
#

ifeq ($(MAKINGSTRCONF),1)
CC	= cc $(OPT) $(XOPTS)
else
CC	+= $(OPT) $(XOPTS)
endif

PGMS	= streams makenodes strconf
UTILS	= timetst strtst
ifneq ($(LIS_TARG),user)
UTILS  += polltst
endif


ifneq ($(LIS_TARG),user)

all: $(PGMS) $(UTILS) $(XUTILS)

install: streams makenodes polltst timetst strtst $(XINST)
	install -d $(INST_SBIN)
	install streams $(INST_SBIN)/streams
	install makenodes $(INST_SBIN)/strmakenodes
	install -d $(INST_BIN)
	install polltst $(INST_BIN)/polltst
	install timetst $(INST_BIN)/timetst
	install strtst $(INST_BIN)/strtst

uninstall:	dummy $(XUNINST)
	-rm -f $(INST_SBIN)/streams
	-rm -f $(INST_SBIN)/strmakenodes
	-rm -f $(INST_BIN)/polltst
	-rm -f $(INST_BIN)/timetst
	-rm -f $(INST_BIN)/strtst

else

all: $(UTILS) $(XUTILS)

install:

uninstall:

endif


clean:
	-rm -f *.o $(PGMS) $(UTILS) $(XUTILS) $(XCLEAN)

realclean:	clean
	-rm -f .depend
	-rm -f strconf $(XREALCLEAN)

#
# The userland target links the routine into the application
# and does not use seperate utilities.
#
ifneq ($(LIS_TARG),user)

#
# A utility program to create STREAMS device special nodes.
# The source is created by strconf and is target specific.
#
makenodes: $(UTILOBJ)/makenodes.c
	$(CC) -o $@ $^ $(ULOPTS)

#
# The streams utility needs some special handling.
#
streams: $(UTILDIR)/streams.c $(LIBDEPS)
	$(CC) -U__KERNEL__ -DMEMPRINT -o $@ $^ $(ULOPTS)

endif

#
# The strconf utility needs some special handling.
#
strconf: $(UTILDIR)/strconf.c
	$(CC) -o $@ $^

#
# A rule to build other utility programs with source in $(UTILDIR)
#
%: $(UTILDIR)/%.c $(LIBDEPS)
	$(CC) -o $@ $^ $(ULOPTS)

#
# A rule to build a target specific utility program with source in $(UTILOBJ)
#
%: %.c $(LIBDEPS)
	$(CC) -o $@ $^ $(ULOPTS)

#
# A special service to people struggling with cpp.
# This is not used by default.
#
%.E: $(UTILDIR)/%.c
	$(CC) -E $^ >$@


#
# Dependency stuff
#
-include .depend

dep:
	$(CC) -M $(UTILS:%=$(UTILDIR)/%.c) >.depend
	$(CC) -M $(UTILDIR)/strconf.c >>.depend
	$(CC) -M $(UTILDIR)/streams.c >>.depend
ifneq ($(strip $(XUTILS:%=%.c)),)
	-$(CC) -M $(XUTILS:%=%.c) >>.depend
endif

dummy:			# no rule
