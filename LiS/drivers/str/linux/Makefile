#
#  Makefile for LiS drivers.
#
#  This one builds drivers for the Linux kernel.
#
LIS_HOME = ../../..
include $(LIS_HOME)/config.mk

-include $(LIS_HOME)/drvrconf.mk

OPT = -DLINUX -D__KERNEL__
MODOPT = -DMODULE
#ifeq ($(DBG_OPT),y)
#  OPT += -DDEBUG
#endif

CC += -DLiS $(OPT) $(XOPTS)


# Object names known to all targets
OBJS := fifo loop relay clone minimux printk timod tirdwr sad pipemod

# Object names only known to the Linux kernel target.
TOBJS := ip_strm_mod ldl connld ip_to_dlpi

# Similar to TOBJS, but non-streams modules
ZOBJS := liskmod

# Remove unknown object names
LINKS := $(filter $(OBJS),$(MODCONF_LINKS))
LOADS := $(filter $(OBJS),$(MODCONF_LOADS))
TLINKS := $(filter $(TOBJS),$(MODCONF_LINKS))
TLOADS := $(filter $(TOBJS),$(MODCONF_LOADS))
ZLOADS := $(ZOBJS)

# Linked object file names
DRVRS := $(LINKS:%=%.o)
TDRVRS := $(TLINKS:%=%.o)

# Loadable object file names
MODLS := $(LOADS:%=streams-%.o)
TMODLS := $(TLOADS:%=streams-%.o)
ZMODLS := $(ZLOADS:%=%.o)

#
# Module support
#
ifdef CONFIG_MODULES

ifdef CONFIG_MODVERSIONS
OPT += -DMODVERSIONS
endif 

MODULES := $(MODLS:%=$(LIS_HOME)/modules/%) 

else   # not CONFIG_MODULES

ifneq ($(strip $(MODLS) $(TMODLS)),)
first_rule:
	echo "Configured loadables, but no kernel module support." >&2
	/bin/false
endif  # ifdef MODLS

endif  # ifdef CONFIG_MODULES


#
# Our default target
#
default: drivers.o strinet

ifneq ($(strip $(MODLS) $(TMODLS)),)

# A target that makes modules
modules: $(MODLS) $(TMODLS) $(ZMODLS)
	ln -s $(^:%=$(DRVROBJ)/%) $(LIS_HOME)/modules

else  # No modules to be made here

modules:

endif

#
# Rules
#
clean: common-clean clean_strinet
	-rm -f $(OBJS:%=streams-%.o)
	-rm -f $(TOBJS:%=streams-%.o)
	-rm -f $(ZOBJS:%=%.o)

realclean: clean common-realclean realclean_strinet
	-rm -f .depend.modules

install:	common-install install_strinet

uninstall:	common-uninstall uninstall_strinet

# How to compile a loadable only known here ( $(TMODLS) )
streams-%.o: %.c
	$(CC) $(MODOPT) -DLIS_OBJNAME=$* -c -o $@ $^

# How to compile a loadable known to all targets ( $(MODLS) )
streams-%.o: $(DRVRDIR)/%.c
	$(CC) $(MODOPT) -DLIS_OBJNAME=$* -c -o $@ $^

#
# This would be the rule to compile modules in this directory,
# but it gets overridden by the global rule -- or fails to
# override it.  So the MODOPT does not get included.
#
%.o: %.c
	$(CC) $(MODOPT) -DLIS_OBJNAME=$* -c -o $@ $^
#
# Common rules
#

# Here because the general rule is in-effective
liskmod.o:	liskmod.c
	$(CC) $(MODOPT) -DLIS_OBJNAME=$* -c -o $@ $^

#
# OPEN SS7 strinet package
#
strinet:
	cd ../strinet; ./make_strinet

clean_strinet:
	cd ../strinet; ./make_strinet clean

realclean_strinet:
	cd ../strinet; ./make_strinet realclean

install_strinet:
	cd ../strinet; ./make_strinet install

uninstall_strinet:
	cd ../strinet; ./make_strinet uninstall

include $(DRVRDIR)/mk.bdy

#
# Dependency stuff
#

-include .depend.modules

dep: common-dep
ifneq ($(strip $(MODLS) $(TMODLS)),)
	$(CC) $(MODOPT) -M $(MODLS:streams-%.o=$(DRVRDIR)/%.c) \
 $(TMODLS:streams-%.o=%.c) >.depend.modules
else
	>.depend.modules
endif

