#
# This is the common part of the makefiles in the util directory.
#
# "make all" is the standard thing to do to make the utility programs.
# "make install" will install them in $(DESTDIR)$([s]bindir)
# "make uninstall" will remove them from $(DESTDIR)$([s]bindir)
# "make clean" to clean up the object code
#
# Targets install and uninstall do nothing for user-land utilities.
#
# Invoking makefile needs to include $(SRCDIR)/config.mk and define:
#
# OPT      	Target specific compiler options
# [s]bindir	where to install the utilities
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

all: $(Q_CC) $(PGMS) $(UTILS) $(XUTILS)
	$(nothing)

install: .compiler streams makenodes polltst timetst strtst $(XINST)
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
	$(Q)install -d $(DESTDIR)$(sbindir)
	$(Q)install -m 0750 streams $(DESTDIR)$(sbindir)/streams
	$(Q)install -m 0750 makenodes $(DESTDIR)$(sbindir)/strmakenodes
	$(Q)install -d $(DESTDIR)$(bindir)
	$(Q)install -m 0755 polltst $(DESTDIR)$(bindir)/polltst
	$(Q)install -m 0755 timetst $(DESTDIR)$(bindir)/timetst
	$(Q)install -m 0755 strtst $(DESTDIR)$(bindir)/strtst
	$(Q)install -d $(DESTDIR)$(pkgdatadir)/$(relpwd)
	$(Q)cat .compiler | \
	    sed "s:^:CC=\(:" | sed "s:$$:\):" | \
	    sed "s:$(SRCDIR)/include:$$\{pkgincludedir\}:g" | \
	    sed "s:$(SRCDIR):$$\{pkgdatadir\}:g" | \
	    sed "s:$(KINCL):$$\{KINCL\}:g" | \
	    sed "s:$(KSRC):$$\{KSRC\}:g" | \
	    cat > $(DESTDIR)$(pkgdatadir)/$(relpwd)/.compiler

uninstall:	dummy $(XUNINST)
	-$(Q)rm -f $(DESTDIR)$(sbindir)/streams
	-$(Q)rm -f $(DESTDIR)$(sbindir)/strmakenodes
	-$(Q)rm -f $(DESTDIR)$(bindir)/polltst
	-$(Q)rm -f $(DESTDIR)$(bindir)/timetst
	-$(Q)rm -f $(DESTDIR)$(bindir)/strtst

else

all: $(UTILS) $(XUTILS)

install:
	$(nothing)

uninstall:
	$(nothing)

endif


# the following relates to the Q_CC variable, which may be set to .compiler if
# this target's output is desired
#
.compiler:
	$(Q_ECHO) $(qtag__WD_)$(relpwd)
	$(Q_ECHO) $(qtag__CC_)$(CC)
	@echo $(CC) > $@

clean:
	-$(Q_ECHO) $(qtag_CLEAN)$(relpwd)
	-$(Q)rm -f *.o $(PGMS) $(UTILS) $(XUTILS) $(XCLEAN) .compiler

realclean:	clean
	-$(Q)rm -f .depend
	-$(Q)rm -f strconf $(XREALCLEAN)

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
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $^ $(ULOPTS)

#
# The streams utility needs some special handling.
#
streams: $(UTILDIR)/streams.c $(LIBDEPS)
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -U__KERNEL__ -DMEMPRINT -o $@ $^ $(ULOPTS)

endif

#
# The strconf utility needs some special handling.
#
strconf: $(UTILDIR)/strconf.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $^

#
# A rule to build other utility programs with source in $(UTILDIR)
#
%: $(UTILDIR)/%.c $(LIBDEPS)
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $^ $(ULOPTS)

#
# A rule to build a target specific utility program with source in $(UTILOBJ)
#
%: %.c $(LIBDEPS)
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $^ $(ULOPTS)

#
# A special service to people struggling with cpp.
# This is not used by default.
#
%.E: $(UTILDIR)/%.c
	$(Q)$(CC) -E $^ >$@


#
# Dependency stuff
#
-include .depend

dep:
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(Q)$(CC) -M $(UTILS:%=$(UTILDIR)/%.c) >.depend
	$(Q)$(CC) -M $(UTILDIR)/strconf.c >>.depend
	$(Q)$(CC) -M $(UTILDIR)/streams.c >>.depend
ifneq ($(strip $(XUTILS:%=%.c)),)
	-$(Q)$(CC) -M $(XUTILS:%=%.c) >>.depend
endif

dummy:			# no rule
