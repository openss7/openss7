quiet_mk_ident="@(#) LiS quiet.mk 1.3 06/06/04"
#
# To put more focus on warnings, be less verbose by default
#
# Use 'make V=1' to see the full commands, make -s to suppress
# all non-warning output
#

ifdef V
  ifeq ("$(origin V)", "command line")
    LIS_BUILD_VERBOSE = $(V)
  endif
endif
ifndef LIS_BUILD_VERBOSE
  LIS_BUILD_VERBOSE = 0
endif

#
# The following was inspired by what the 2.6 kernels' 'kbuild' does,
# but is much simplified.
#
ifeq ($(LIS_BUILD_VERBOSE),1)
  quiet =
  Q =
  Q_ECHO = @:
  Q_PRINT = -print
  Q_CC =
else
  quiet = quiet_
  Q = @
  Q_ECHO = @echo
  Q_PRINT =
  Q_CC = .compiler
  # Do not print "Entering|Leaving directory ..."
  MAKEFLAGS += --no-print-directory
endif

# If the user is running make -s (silent mode), suppress echoing of
# commands

ifneq ($(findstring s,$(MAKEFLAGS)),)
  quiet = silent_
  Q = @
  Q_ECHO = @:
  Q_PRINT =
  Q_CC =
endif

# relative path function and variables useful for Q_ECHO'ing, and possibly
# other things as well
#
define _relpath
$(patsubst $(SRCDIR)/%,%,$(1))
endef

define _relpwd
$(patsubst $(SRCDIR)/%,%,$(shell pwd)/$(1)))
endef

relpwd		= $(patsubst $(SRCDIR)/%,%,$(shell pwd))
reltarget	= $(patsubst $(SRCDIR)/%,%,$(@))
relpwdtarget	= $(patsubst $(SRCDIR)/%,%,$(shell pwd)/$(@F))
reldep1		= $(patsubst $(SRCDIR)/%,%,$(<))
relpwddep1	= $(patsubst $(SRCDIR)/%,%,$(shell pwd)/$(<F))
reldeps		= $(foreach d,$^,$(patsubst $(SRCDIR)/%,%,$(d)))
relpwddeps	= $(foreach d,$^,$(patsubst $(SRCDIR)/%,%,$(shell pwd)/$(d)))

# empty target command - avoids "nothing to do for" messages
#
nothing	= @:

# output tags for use with Q_ECHO (defined here so we can consistently
# change them)
#
qtag_AR		= "  AR      "
qtag_CC		= "  CC      "
qtag__CC_	= " <CC>     "
qtag_CCm	= "  CC [m]  "
qtag_CLEAN	= "  CLEAN   "
qtag_CONFIG	= "  CONFIG  "
qtag_continue	= "  ...     "
qtag_DEP	= "  DEP     "
qtag_GENCONF	= "  GENCONF "
qtag_INSTALL	= "  INSTALL "
qtag_KBUILD	= "  KBUILD  "
qtag_KERNEL	= "  KERNEL  "
qtag_LD		= "  LD      "
qtag_LDm	= "  LD [m]  "
qtag_MAKE	= "  MAKE    "
qtag_MKDIR	= "  MKDIR   "
qtag_MODPOST	= "  MODPOST "
qtag_MODULES	= "  MODULES "
qtag__m_	= "     [m]  "
qtag_RANLIB	= "  RANLIB  "
qtag_RM		= "  RM      "
qtag_RPM	= "  RPM     "
qtag_STRCONF	= "  STRCONF "
qtag_SUBDIRS	= "  SUBDIRS "
qtag_SYMVERS	= "  SYMVERS "
qtag_USER	= "  USER    "
qtag__WD_	= " <WD>     "
