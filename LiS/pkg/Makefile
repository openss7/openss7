include .config_mk

#
# The following collect subdirs, whether containing a Makefile or not.
# OBJECT_SUBDIRS only collects immediate subdirs (for backward compatibility).
# 
OBJECT_SUBDIRS = $(shell find . -mindepth 1 -maxdepth 1 -follow -type d | grep -v CVS)
SUBDIRS = $(shell find . -mindepth 1 -follow -type d | grep -v CVS)

# OBJECTS and MODULES are the output .o files produced in this directory
# _only_.  Their paths relative to SRCDIR are saved to .objects and
# .modules, resp., after the named files are built.  Other Makefiles can
# then find and use these files appropriately.
#
# subdirectories of this one should produce their own .objects and .modules
# files.  All this Makefile need do is make sure they have that opportunity,
# by invoking their 'default'/'all' and/or 'modules' targets when they
# are invoked here
#
# Old versions of this Makefile didn't support building modules, or building
# base module objects below the immediate subdirectories.  This Makefile
# allows both, but achieves backward compatibility by using a surrogate
# makefile, mk.sub, for those immediate subdirs that don't produce
# .objects or .modules files when made.
#
OBJECTS	=
MODULES	=

CC += -D__KERNEL__ -DLINUX $(CCREGPARM) $(XOPTS)

ifneq ($(MODULES),)
MODOPT = -DMODULE
ifeq ($(KBUILD_NEEDS_MODNAME),y)
  MODOPT += -DKBUILD_MODNAME=$(*F:%=streams-%)
endif
endif


default: all
	$(nothing)

all: $(Q_CC) subdirs
	$(Q)$(MAKE) -C . drivers.o

modules: 
	$(nothing)

drivers.o: */*.o
	$(Q_ECHO) $(qtag_LD)$(relpwdtarget)
#
#	the .objects files are written with SRCDIR-relative paths;
#	truncate this directory's portion of that path as we read them
#
#	this is backward compatible because of how the 'subdirs'
#	target now works.  old makefiles did 'ld ... */*.o' to build
#	the drivers.o target - 'subdirs' now does 'make all' in all
#	subdirs, but if any don't make their own .objects files, 'subdirs'
#	will try again with a surrogate makefile (mk.sub) that
#	collects all of the old-style subdir's objects into a .objects
#	file.
#
	$(Q_ECHO) $(qtag_continue) */*.o
	$(Q)$(LD) -r -o $@ */*.o 

.objects: $(OBJECTS)
	$(Q)echo $(relpwddeps) > $@

.modules: $(MODULES)
	$(Q)echo $(relpwddeps) > $@

ifneq ($(OBJECTS),)
$(OBJECTS): %.o: %.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ -c $<
endif

ifneq ($(MODULES),)
$(MODULES): streams-%.o: %.c
	$(Q_ECHO) $(qtag_CCm)$(relpwdtarget)
	$(Q)$(CC) $(MODOPT) -DLIS_OBJNAME=$* -o $@ -c $<
endif


# for backward compatibility:
# . make in all subdirs
# . check the immediate subdirs for .objects files
# . if no .objects:
#   . remake with mk.sub
#
subdirs:
	$(Q): set -e; \
	for i in $(SUBDIRS); do \
	    [ ! -e $${i}/.config_mk ] && \
		$(LN) $(SRCDIR)/.config_mk $${i}/.config_mk; \
	    [ -f $${i}/Makefile ] && $(MAKE) -C $$i all || : ;\
	done

install: .compiler
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
	$(Q)install -d $(DESTDIR)$(pkgdatadir)/$(relpwd)
#	FIXME: clean up the following for externally building modules
	$(Q)install -m 0644 Makefile $(DESTDIR)$(pkgdatadir)/$(relpwd)/.
	$(Q)install -m 0644 mk.sub $(DESTDIR)$(pkgdatadir)/$(relpwd)/.
#
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


real-clean: clean FORCE
	$(Q): set -e; \
	for i in $(SUBDIRS); do \
	    [ -f $${i}/Makefile ] && $(MAKE) -C $$i $@ || : ;\
	done

clean: FORCE
	$(Q): set -e; \
	for i in $(SUBDIRS); do \
	    [ -e $${i}/.config_mk -a ! -f $${i}/Makefile ] && \
	    $(MAKE) -f $(shell pwd)/mk.sub -C $$i $@ || : ; \
	    rm -f $$i/.objects $$i/.config_mk $$i/.modules ;\
	done
	$(Q_ECHO) $(qtag_CLEAN)$(relpwd)
	-$(Q)rm -f drivers.o .objects .modules .compiler

FORCE:
