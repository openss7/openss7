include .config_mk

#
#  Makefile for LiS drivers
#
#  This one builds drivers for the Linux kernel.
#
#  FIXME - some of these modules (e.g., relay3, inet) can now be moved
#  to a pkg/ subdir of their own. since modules can now be built from
#  there
#
-include $(SRCDIR)/drvrconf.mk

OPT = -DLINUX -D__KERNEL__
MODOPT = -DMODULE
ifeq ($(KBUILD_NEEDS_MODNAME),y)
  MODOPT += -DKBUILD_MODNAME=$(*F:%=streams-%)
endif
#ifeq ($(DBG_OPT),y)
#  OPT += -DDEBUG
#endif

CC += -DLiS $(CCREGPARM) $(OPT) $(XOPTS)


# Object names known to all targets
OBJS := fifo loop relay clone minimux printk timod tirdwr sad pipemod

# Object names only known to the Linux kernel target.
TOBJS := ip_strm_mod ldl connld mtdrv
#
# FIXME - move inet to the pkg/ directory in a subdir of its own; that
# should work now...
#
ifeq ($(CONFIG_INET),y)
ifeq ($(IPV6_MODULE),y)
TOBJS += inet
INET_INST=inet_install
INET_CLEAN=inet_clean
ifeq ($(DBG_OPT),y)
XOPTS += -D_SAFE
endif
endif				# IPV6_MODULE
endif				# CONFIG_INET

ifneq ($(KVER_MAJORMINOR),2.2)
TOBJS += ip_to_dlpi
endif
ifeq ($(ARCH),s390)
TOBJS := connld
endif
ifeq ($(ARCH),s390x)
TOBJS := connld
endif

# Similar to TOBJS, but non-streams modules
ZOBJS := 

# Remove unknown object names
LINKS := $(filter $(OBJS),$(MODCONF_LINKS))
LOADS := $(filter $(OBJS),$(MODCONF_LOADS))
TLINKS := $(filter $(TOBJS),$(MODCONF_LINKS))
TLOADS := $(filter $(TOBJS),$(MODCONF_LOADS))
ZLOADS := $(ZOBJS)

# Add back Linux-only modules not in the Config file
TLOADS += relay3

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

# FIXME: are DMODLS and MODULES the same?

MODULES	= $(MODLS) $(TMODLS) $(ZMODLS)

else   # not CONFIG_MODULES

ifneq ($(strip $(MODLS) $(TMODLS) $(ZMODLS)),)
first_rule:
	@echo "Configured loadables, but no kernel module support." >&2
	@/bin/false
endif  # ifdef MODLS

endif  # ifdef CONFIG_MODULES


# How to compile a loadable only known here ( $(TMODLS) )
streams-%.o: %.c
	$(Q_ECHO) $(qtag_CCm)$(relpwdtarget)
	$(Q)$(CC) $(MODOPT) -DLIS_OBJNAME=$* -o $@ -c $<

# How to compile a loadable known to all targets ( $(MODLS) )
streams-%.o: $(DRVRDIR)/%.c
	$(Q_ECHO) $(qtag_CCm)$(relpwdtarget)
	$(Q)$(CC) $(MODOPT) -DLIS_OBJNAME=$* -o $@ -c $<

%.o: %.c
	$(Q_ECHO) $(qtag_CCm)$(relpwdtarget)
	$(Q)$(CC) $(MODOPT) -DLIS_OBJNAME=$* -o $@ -c $<


# the following uses variables, but defines targets and rules, including
# a 'default' rule which should be first (with the exception of first_rule
# above).
#
# so, variables should be defined before this include, as should rules to
# override, but additional targets and rules after
#
include $(DRVRDIR)/mk.bdy


# A target that makes modules

modules: .modules
	$(nothing)

.modules: $(MODULES)
	$(Q)echo $(relpwddeps) > $@


clean: common-clean $(INET_CLEAN)
	$(Q_ECHO) $(qtag_CLEAN)$(relpwd)
	-$(Q)rm -f $(OBJS:%=streams-%.o)
	-$(Q)rm -f $(TOBJS:%=streams-%.o)
	-$(Q)rm -f $(ZOBJS:%=%.o)
	-$(Q)rm -f .modules

realclean: clean common-realclean
	-$(Q)rm -f .depend.modules

install: common-install $(INET_INST)
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)

uninstall:	common-uninstall
	$(nothing)

#
# Install and clean for inet
#
inet_install:
	$(Q_ECHO) $(qtag_INSTALL)"[inet]"
	-$(Q)(cd $(SRCDIR)/scripts; ./inet_mkdev)

inet_clean:
	$(Q_ECHO) $tag_CLEAN)"[inet]"
	-$(Q)(cd $(SRCDIR)/scripts; ./inet_rmdev)

#
# Common rules
#


#
# Rule for relay3, a special compilation of relay.c
#
streams-relay3.o: relay3.c
	$(Q_ECHO) $(qtag_CCm)$(relpwdtarget)
	$(Q)$(CC) $(MODOPT) -DLIS_OBJNAME=$* -DRELAY3 -o $@ -c $<
#	-$(Q)rm -f $<

relay3.c: $(DRVRDIR)/relay.c
#	-$(Q)rm -f $@
	$(Q)cp $? $@

#
# Dependency stuff
#

-include .depend.modules

dep: relay3.c common-dep
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
ifneq ($(strip $(MODLS) $(TMODLS)),)
	$(Q)$(CC) $(MODOPT) -M $(MODLS:streams-%.o=$(DRVRDIR)/%.c) \
 $(TMODLS:streams-%.o=%.c) >.depend.modules
else
	$(Q)>.depend.modules
endif

