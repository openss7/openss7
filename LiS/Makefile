Makefile_ident="@(#) LiS Makefile 2.51 09/10/04"
#
# Makefile for the kernel STREAMS device drivers.
#
# Note 1: Dependencies are done automagically by 'make dep', which also
# removes any old dependencies.
#
# This Makefile is intended to be run from the command line.  It can
# also be run from other makefiles.
#

#
# Use 'make V=1' to see the full commands, make -s to suppress
# all non-warning output
#

# To deal with prior circular dependencies in this process, and to allow
# any subdirectory make to be invoked directly, Configure now creates a
# .config_mk to correspond to each Makefile in the LiS source tree, so that
# 'include .config_mk' can be done at the top of any Makefile and provide the
# make with any configuration-dependent variables it may need.  I.e.,
# once Configure has been run, e.g.,
#
#	make -C pkg
#
# may be done directly and safely.   Although this adds the requirement that
# Configure must be done first to make subdirs directly, the top Makefile,
# for backwards compatibility, will invoke Configure via the 'configure'
# target if a top .config_mk doesn't exist.  The 'configure' target will in turn
# recursively invoke 'make'.  This is now the extent of recursive invocations
# of the top-level Makefile.
#
# Note that some targets do nothing if .config_mk doesn't exist; this is
# to prevent unset variables from doing harm ( see clean & install related
# targets).
#
# Otherwise, this Makefile has been reordered so that dependencies are
# forward, as conventionally required for Unix makefiles, i.e., so that
# a target named as a dependent of another is defined further down in
# this file.

ifeq ($(wildcard .config_mk),)
#
# not configured - force 'make configure', which recursively does 'make all'
#
SRCDIR=$(shell pwd)
DEFAULT_TARGETS=configure
include quiet.mk
else
#
# configured - do 'make all' if no specific target named
#
DEFAULT_TARGETS=all
include .config_mk
endif

#
#  Our default target
#
default: $(DEFAULT_TARGETS)
	$(nothing)

ALL_TARGETS = config

ifneq ($(LIS_TARG),user)
ALL_TARGETS += libc util
endif

#
# Build modules only when running in the Linux kernel
#
ifeq ($(LIS_TARG),linux)
ALL_TARGETS += modules
endif

#
# If building on 64-bit s390 then build libc for 32-bit applications
# running over 64-bit kernel.
#
ifeq ($(ARCH),s390x)
ALL_TARGETS += libc32over64
endif

#
# Does the user want us to install streams.o in the official
# modules directory?
#
ifeq ($(MOD_INSTALL),y)
ALL_TARGETS += modules_install
endif

#
# Does the user want us to make the kernel after we make STREAMS?
#
ifeq ($(CONFIG_MK_KERNEL),y)
ALL_TARGETS += kernel
endif

ifeq ($(LIS_TARG),user)
ALL_TARGETS += streams.o libc util
endif


all: $(ALL_TARGETS)


#
# You may have to be root to do these targets
#
ifeq ($(LIS_TARG),linux)

ifneq ($(wildcard .config_mk),)
install: modules_install $(filter-out modules_install,$(ALL_TARGETS))
	$(Q_ECHO) $(qtag_INSTALL)
	$(Q)install -d $(DESTDIR)$(pkgdatadir)
	$(Q)install -d $(DESTDIR)$(pkglibdir)
	$(Q)install -d $(DESTDIR)$(bindir)
	$(Q)install -d $(DESTDIR)$(sbindir)
ifneq ($(pkgsrcdir),)
#
#	pkgsrcdir is _entirely_ for backward compatibility
#	(going forward, I recommend defining it away... - JB) -
#
#	we recognize two cases: with or without a DESTDIR:
#	. without a DESTDIR, we get rid of old /usr/src/LiS
#	  symlink not to SRCDIR.  We create a new symlink from
#	  pkgsrcdir to SRCDIR.  We won't handle uninstalling old
#	  ones since (unless they're /usr/src/LiS) we won't know
#	  what they were.
#	. with a DESTDIR, we likely won't have SRCDIR around,
#	  so we make a symlink from pkgsrcdir to pkgdatadir instead
#
ifeq ($(DESTDIR),)	# we have no DESTDIR
#	get rid of old /usr/src/LiS if it's a symlink
ifneq ($(wildcard /usr/src/LiS),)
	-$(Q)[ -L /usr/src/LiS ] && rm -f /usr/src/LiS || :
endif
#	if no pkgsrcdir now, make it a symlink to SRCDIR
ifeq ($(wildcard $(pksrcdir)),)
#	it's important here to make sure pkgsrcdir has no trailing /
	$(Q)install -d $(dir $(pkgsrcdir:%/=%))
	$(Q)(cd $(dir $(pkgsrcdir:%/=%)) && \
		$(LN) $(SRCDIR) $(notdir $(pkgsrcdir:%/=%)))
endif
else			# we have a DESTDIR
ifneq ($(pkgdatadir),)
#			  we have a pkgdatadir under DESTDIR
ifneq ($(pkgsrcdir),$(pkgdatadir))
#	make pkgsrcdir a symlink to pkgdatadir
#	it's important here to make sure pkgsrcdir has no trailing /
	$(Q)install -d $(DESTDIR)$(dir $(pkgsrcdir:%/=%))
	$(Q)(cd $(DESTDIR)$(dir $(pkgsrcdir:%/=%)) && \
		$(LN) $(pkgdatadir) $(notdir $(pkgsrcdir:%/=%)))
endif	# pkgsrcdir != pkgdatadir
endif	# pkgdatadir
endif	# DESTDIR
endif	# pkgsrcdir
#
	$(Q)$(MAKE) -C include $@
	$(Q)$(MAKE) -C $(LIBOBJ) $@
ifeq ($(ARCH),s390x)
	$(Q)$(MAKE) -C $(LIBOBJ32OVER64) $@
endif
	$(Q)$(MAKE) -C $(UTILOBJ) $@
	$(Q)$(MAKE) -C $(DRVROBJ) $@
	$(Q)$(MAKE) -C $(HEADOBJ) $@
	$(Q)$(MAKE) -C pkg $@
	$(Q)$(MAKE) -C man $@
	$(Q)$(MAKE) -C scripts $@
ifeq ($(DESTDIR),)
	$(Q)$(sbindir)/strmakenodes
endif
ifneq ($(pkgdatadir),)
#
#	install the make config files we used here, but replace
#	$(SRCDIR) in CONFIG with whatever is appropriate (mostly pkgdatadir)
#	CONFIG is kernel-version dependent, so we put it in a kernel-version
#	named subdir of pkgdatadir
#
	$(Q)install -d $(DESTDIR)$(pkgdatadir)/linux-$(KVER)
	$(Q)cat $(CONFIG) | \
	    sed "s:CONFIG=$(SRCDIR):CONFIG=$(pkgdatadir)/$(KVER):g" | \
	    sed "s:SRCDIR=$(SRCDIR):SRCDIR=$(pkgsrcdir):g" | \
	    sed "s:GENCONF=$(SRCDIR)/include:GENCONF=$(pkgincludedir):g" | \
	    sed "s:$(SRCDIR):$(pkgdatadir):g" > \
		$(DESTDIR)$(pkgdatadir)/linux-$(KVER)/config.in
	$(Q)install config.mk $(DESTDIR)$(pkgdatadir)/config.mk
	$(Q)install version   $(DESTDIR)$(pkgdatadir)/version
	$(Q)install quiet.mk  $(DESTDIR)$(pkgdatadir)/quiet.mk
#
#	FIXME - further populate pkgdatadir
##
endif
else	# no .config_mk
install:
	$(nothing)
endif

ifneq ($(wildcard .config_mk),)
#
# determine the preferred name for /etc/modules.conf or its equivalent:
# - kernel version 2.5 or later (MODULE_INIT_TOOLS) = modprobe.conf
# - older kernels - if not using DESTDIR, check existing file(s); use
#   modules.conf if it exists, else conf.modules
#
ifeq ($(MODULE_INIT_TOOLS),y)
CONFMOD = modprobe.conf
else
ifeq ($(DESTDIR),)
confmod = $(word 1,$(patsubst /etc/%,%,$(wildcard /etc/modules.conf /etc/conf.modules /etc/modprobe.conf)))
CONFMOD = $(if $(confmod),$(confmod),modules.conf)
else
CONFMOD = modules.conf
endif
endif

# note - SRCDIR/CONFMOD is generated by strconf (see below)
#
# Prior versions of LiS updated /etc/[modules.conf,conf.modules]
# to include the contents of the generated conf.modules file.
# This was done every time LiS' modules were installed.  Now we
# insert an "include" statement only once, and replace our own
# file CONFMOD.streams, which the include references.  This
# minimizes the change to the system file itself.
#
# because old versions changed the system file, however, we still
# need to get rid of the stuff it added.  The old update_conf.modules
# script did that, so we use if if we need to.
#

modules_install: modules $(CONFMOD).streams $(CONFMOD).incl FORCE
	$(Q_ECHO) $(qtag_INSTALL)"[modules]"
ifeq ($(CONFIG_STREAMS),m)
	$(Q)$(MAKE) -C modules $@
	$(Q)install -d $(DESTDIR)$(sysconfdir)
	$(Q)install $(CONFMOD).streams $(DESTDIR)$(sysconfdir)/.
ifeq ($(DESTDIR),)
	$(Q)grep "include .*/$(CONFMOD).streams" \
		$(DESTDIR)$(sysconfdir)/$(CONFMOD) >/dev/null 2>&1 || \
		cat $(CONFMOD).incl >> $(DESTDIR)$(sysconfdir)/$(CONFMOD)
	$(Q)grep "^#BEGIN LiS" $(sysconfdir)/$(CONFMOD) 2>/dev/null 2>&1 \
		&& scripts/update_conf.modules || :
	$(Q)depmod -a
else	# no DESTDIR
	$(Q)install -d $(DESTDIR)$(pkgdatadir)
	$(Q)install $(CONFMOD).incl $(DESTDIR)$(pkgdatadir)/.
endif	# DESTDIR
endif	# CONFIG_STREAMS=m

modprobe.conf.streams: $(SC_CONFMOD) modprobe.conf.incl
	$(Q)head -2 $(SC_CONFMOD) 				>  $@
	$(Q)cat $(SC_CONFMOD) | generate-modprobe.conf --stdin	>> $@

modules.conf.streams: $(SC_CONFMOD) modules.conf.incl
	$(Q)cat $(SC_CONFMOD) > $@

$(CONFMOD).incl:
	$(Q)echo "# $(package)"					>  $@
	$(Q)echo "include $(sysconfdir)/$(CONFMOD).streams"	>> $@

else	# no .config_mk
modules_install:
	$(nothing)
endif

ifeq ($(DESTDIR),)

#	FIXME - I haven't updated this target... -JB
#
uninstall: FORCE
ifneq ($(wildcard .config_mk),)
	-$(Q)env LD_LIBRARY_PATH=$(libdir):$(LD_LIBRARY_PATH) \
		$(sbindir)/strmakenodes -r
	-$(Q)$(MAKE) -C $(LIBOBJ) $@
ifeq ($(ARCH),s390x)
	-$(Q)$(MAKE) -C $(LIBOBJ32OVER64) $@
endif
	-$(Q)$(MAKE) -C $(UTILOBJ) $@
	-$(Q)$(MAKE) -C $(DRVROBJ) $@
	-$(Q)$(MAKE) -C man $@
	-$(Q)(cd $(sbindir) && rm -f strms_down strms_status strms_up)
ifeq ($(CONFIG_STREAMS),m)
	-$(Q) scripts/update_conf.modules
	-$(Q) (cd $(MOD_INST_DIR); rm -f streams.o streams-*.o)
endif
endif	# .config_mk
	$(nothing)

else

uninstall:
	$(nothing)

endif

else

install:
	$(nothing)

uninstall:
	$(nothing)

endif


util: libc FORCE
	$(Q)$(MAKE) -C $(UTILOBJ)

libc: config FORCE
	$(Q)$(MAKE) -C $(LIBOBJ)

libc32over64: config FORCE
	$(Q)$(MAKE) -C $(LIBOBJ32OVER64)


#
# User wants us to make the kernel whenever we do a make of STREAMS
#
kernel:	streams.o FORCE
	$(Q_ECHO) $(qtag_KERNEL)
	$(Q)$(MAKE) $(KERN_TARGET) -C $(KSRC)

all_but_kernel:			# Placeholder target from kernel build
	$(nothing)


ifeq ($(CONFIG_STREAMS),m)
#
# Build STREAMS loadable module(s)
#
# The .modules files are used to allow the generating directories to
# identify module-destined objects without copying them into modules/,
# since doing that (as was done before) doesn't allow a cleaned modules/
# to build itself
#

modules: .modules FORCE
	$(Q)$(MAKE) -C $(DRVROBJ) $@
	$(Q)$(MAKE) -C pkg $@
	$(Q)$(MAKE) -C modules 

.modules: streams.o
	$(Q)echo $^ > $@

endif

ifeq ($(CONFIG_STREAMS),k)

# FIXME - I haven't done the following for a LONG time - is it still
# supported?
#
modules: streams.o FORCE
#	$(Q)$(MAKE) -C modules clean
#	$(Q)$(MAKE) -C $(DRVROBJ) $@
#	$(Q)$(MAKE) -C pkg $@

endif


#
# The following four rules takes care of building streams.o
#
streams.o: $(DRVROBJ)/drivers.o $(HEADOBJ)/streamshead.o pkg/drivers.o
	$(Q_ECHO) $(qtag_LDm)$(reltarget)
	$(Q)$(LD) -r -d -o $@ $^

$(HEADOBJ)/streamshead.o: config
	$(Q)$(MAKE) -C $(HEADOBJ)

$(DRVROBJ)/drivers.o: config
	$(Q)$(MAKE) -C $(DRVROBJ)

pkg/drivers.o: config
	$(Q)$(MAKE) -C pkg


#
# Make a user-space version of STREAMS for laboratory testing
#
user: config FORCE
	$(Q_ECHO) $(qtag_USER)
	$(Q)$(MAKE) -C $(DRVROBJ)
	$(Q)$(MAKE) -C $(HEADOBJ)
	$(Q)$(MAKE) -C $(LIBOBJ)
	$(Q)$(MAKE) -C $(UTILOBJ)

tags: config FORCE
	-$(Q)rm -f tags
	$(Q)(find . -name '*.[ch]' -print | grep -v SCCS | xargs ctags -a)

#########################################################################
##                                                                     ##
##   Configuration                                                     ##
##                                                                     ##
#########################################################################

#
# A rule to run the configuration script
#
# Note that the .config_mk rule recursively invokes this Makefile - this
# Makefile thus doesn't depend on CONFIG being config.in, but will use
# that default if the .config_mk rule is executed
#
# CONFIG_OPT is used here to allow Configure options to be passed on
# the make command line, e.g. 'make CONFIG_OPT="-N -g -p n".  This
# allows one-step non-interactive building of LiS if -N|--noprompt
# is passed in this manner.
#
# Note also that 'make ... configure' from the command line will always
# also do the additional make targets here.  To run Configure only, it
# must be done from the command line.
#
configure: .config_mk

.config_mk:
	$(Q)sh Configure $(CONFIG_OPTS) --make \
		--comment "invoked from main makefile" --output config.in
#
	$(Q)$(MAKE) clean
	$(Q)$(MAKE) config
	$(Q)$(MAKE) dep
	$(Q)$(MAKE)

#########################################################################


ifeq ($(LIS_TARG),user)

STRCONF_XOPTS	= -rmake_nodes -Muser_mknod

else

#
# Kernel version 2.3, 2.4 have major device numbers allocated up
# into the 180 region.  We want to get safely above those.  One of
# these days the kernel developers are going to have to fix the
# 8-bit major/minor problem.  DMG 1-7-00
#
# Now even Red Hat 6.2 and 7.0 are encroaching on our major device
# number 50 default range.  So beginning with LiS-2.12 I am moving
# the LiS majors up to 240 for all versions.  This is the range
# that is reserved for "experimental" drivers. DMG 3-22-2001
#
# The macro LIS_MAJOR_BASE is where the major device numbers start.
# It can be set via Configure and is conveyed via CONFIG.
#
# Beginning in LiS-2.13B9 and LiS-2.14 this is going to default to 230
# to provide for more room for STREAMS devices.  You can still override
# as described above.
#
# 04/04/29 - 2.6.x and later 2.4.x kernels reserve major numbers up to 230
#
ifndef LIS_MAJOR_BASE
LIS_MAJOR_BASE=231
endif

STRCONF_XOPTS	= -b$(LIS_MAJOR_BASE)

endif

# These are the files generated by the strconf utility
SC_CONFIG   = $(LIS_INCL)/sys/LiS/config.h
SC_MODCONF  = $(SRCDIR)/head/modconf.inc
SC_DRVRCONF = $(SRCDIR)/drvrconf.mk
SC_CONFMOD  = $(SRCDIR)/modules.conf
ifeq ($(LIS_TARG),user)
SC_MKNODES  = $(LIBOBJ)/makenodes.c
else
SC_MKNODES  = $(UTILOBJ)/makenodes.c
endif
SC_GEN = $(SC_CONFIG) $(SC_MODCONF) $(SC_MKNODES) $(SC_DRVRCONF) $(SC_CONFMOD)

#
# This is our configuration proper.
#
# Use the target below for dependencies on the whole configuration.
#
CONFIG_TARGETS = $(SC_GEN)

config: $(CONFIG_TARGETS)
	$(nothing)

$(SC_GEN): Config.master $(UTILOBJ)/strconf
	$(Q_ECHO) $(qtag_STRCONF)$(reltarget)
	$(Q)$(UTILOBJ)/strconf			\
		-h$(SC_CONFIG)			\
		-o$(SC_MODCONF)			\
		-m$(SC_MKNODES)			\
		-l$(SC_DRVRCONF)		\
		-L$(SC_CONFMOD)			\
		$(STRCONF_XOPTS)		\
		$<
	$(Q)$(MAKE) dep

$(UTILOBJ)/strconf:
	$(Q)$(MAKE) MAKINGSTRCONF=1 -C $(UTILOBJ) strconf

ifeq ($(LIS_TARG),user)

# The file Config.master is just the contents of the file Config.user
Config.master: Config.user
#	$(Q_ECHO) $(qtag_CONFIG)$(reltarget)
	$(Q)cat $^ >$@

else

PKG_CONFIG_FILES = $(shell find pkg -mindepth 2 -maxdepth 2 -follow -type f -name Config)

#
# The file Config.master consists of the concatenation of the Config
# file from the directory LiS/drivers/str and all of the individual
# Configs from binary packages in subdirectories of pkg.
# TODO: The pkg directory should have a "make dep" that creates a
# file pkg/Config.pkg.
#
Config.master: $(DRVRDIR)/Config $(PKG_CONFIG_FILES)
#	$(Q_ECHO) $(qtag_CONFIG)$(reltarget)
	$(Q)cat $^ >$@

endif

#########################################################################
##   End of: Configuration                                             ##
#########################################################################


very-clean: clean realclean 
	$(nothing)

realclean: clean FORCE
ifneq ($(wildcard .config_mk),)
	-$(Q)$(MAKE) -C $(LIBDIR)/linux $@
	-$(Q)$(MAKE) -C $(HEADDIR)/linux $@
	-$(Q)$(MAKE) -C $(DRVRDIR)/linux $@
	-$(Q)$(MAKE) -C $(UTILDIR)/linux $@
	-$(Q)$(MAKE) -C $(LIBDIR)/user $@
	-$(Q)$(MAKE) -C $(HEADDIR)/user $@
	-$(Q)$(MAKE) -C $(DRVRDIR)/user $@
	-$(Q)$(MAKE) -C $(UTILDIR)/user $@
	-$(Q)rm -f pkg/proto/Space.o
	-$(Q)rm -f $(SC_GEN)
	-$(Q)rm -f Config.master
ifneq ($(CONFIG),)
	-$(Q)rm -f $(CONFIG)
endif
ifneq ($(GENCONF),)
	-$(Q)rm -f $(GENCONF)
endif
	-$(Q)find . -name \.config_mk -exec rm -f {} \;
	-$(Q)rm -f kver kconfig
	-$(Q)find . \( -name "*.o" -o -name "*~" \) $(Q_PRINT) -exec rm -f {} \;
endif	# .config_mk
	$(nothing)

clean: FORCE
ifneq ($(wildcard .config_mk),)
	-$(Q)$(MAKE) -C $(LIBOBJ) $@
ifeq ($(ARCH),s390x)
	-$(Q)$(MAKE) -C $(LIBOBJ32OVER64) $@
endif
	-$(Q)$(MAKE) -C $(HEADOBJ) $@
	-$(Q)$(MAKE) -C $(DRVROBJ) $@
	-$(Q)$(MAKE) -C $(UTILOBJ) $@
	-$(Q)$(MAKE) -C $(SRCDIR)/pkg $@
	-$(Q)$(MAKE) -C $(SRCDIR)/modules $@
#	$(Q_ECHO) $(qtag_CLEAN)
	-$(Q)rm -f streams.o Config.master .modules
	-$(Q)rm -f $(CONFMOD).incl $(CONFMOD).streams $(CONFMOD).save
endif	# .config_mk
	$(nothing)

dep: FORCE
ifneq ($(wildcard .config_mk),)
	$(Q)$(MAKE) -C $(LIBOBJ)  $@
ifeq ($(ARCH),s390x)
	$(Q)$(MAKE) -C $(LIBOBJ32OVER64) $@
endif
	$(Q)$(MAKE) -C $(HEADOBJ) $@
	$(Q)$(MAKE) -C $(DRVROBJ) $@
	$(Q)$(MAKE) -C $(UTILOBJ) $@
endif	# .config_mk
	$(nothing)


FORCE:
