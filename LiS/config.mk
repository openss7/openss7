#
# This file includes and calculates some make variables that are useful
# when compiling LiS and utilities.
#
# The makefile that includes this one will have to define SRCDIR to the
# root of the LiS tree & DESTDIR to the supposed root filesystem.
#
# It must not contain any rules.
#
# NOTE: the old -include of config.in here was removed, being a circular
# reference.  It must primarily define SRCDIR, but the include itself
# referenced SRCDIR...
# In the present form, having invoked config.in is an implicit prereq.

# the following are conveniences for staged installs - convention is that
# DESTDIR must be overridden on the command line to be set
#
DESTDIR =
DEST_INCLUDEDIR    = $(DESTDIR)$(includedir)
DEST_LIBDIR        = $(DESTDIR)$(libdir)
DEST_BINDIR        = $(DESTDIR)$(bindir)
DEST_SBINDIR       = $(DESTDIR)$(sbindir)
DEST_SYSCONFDIR    = $(DESTDIR)$(sysconfdir)
DEST_DATADIR       = $(DESTDIR)$(datadir)
DEST_PKGINCLUDEDIR = $(DESTDIR)$(pkgincludedir)
DEST_PKGLIBDIR     = $(DESTDIR)$(pkglibdir)
DEST_PKGDATADIR    = $(DESTDIR)$(pkgdatadir)
DEST_PKGSRCDIR     = $(DESTDIR)$(pkgsrcdir)
DEST_MANDIR        = $(DESTDIR)$(mandir)
#
# FIXME - libc uses some 'LIB64' variables, but I have no idea how
# to set them - they should be set here if not to be defined from
# the command line
DEST_LIB64DIR      = /usr/lib64

#
# If we are cross compiling then the user needs to supply us with a
# file to include named "cross-compile".  It should contain the 
# make variable assignments that we need to set up a cross compile:
#
# Power PC example of contents of this file:
# 
# CROSS_COMPILE	:= powerpc-linux-
# ARCH := ppc
#
# If CC_NAME has not been defined then default it to the dumbest setting.
# Doing a make realclean and a make will run the Configure program and
# get a more intelligent value for CC_NAME.
#
ifndef CC_NAME
    CC_NAME=gcc
    CC_OPTIMIZE=-O2
endif
ifeq ($(CROSS_COMPILING),y)
-include $(SRCDIR)/cross-compile
CC	=$(CROSS_COMPILE)gcc
else
CROSS_COMPILE	:=
CC	=$(CC_NAME)
endif

AS	=$(CROSS_COMPILE)as
LD	=$(CROSS_COMPILE)ld
CPP	=$(CC_NAME) -E
AR	=$(CROSS_COMPILE)ar
NM	=$(CROSS_COMPILE)nm
STRIP	=$(CROSS_COMPILE)strip
OBJCOPY	=$(CROSS_COMPILE)objcopy
OBJDUMP	=$(CROSS_COMPILE)objdump
RANLIB	=$(CROSS_COMPILE)ranlib
LN	=ln -sf

#
# Calculate LIS_TARG
#
# This is the name of the target-specific subdirectory used when compiling.
#
ifeq ($(TARGET),l)
LIS_TARG = linux
else
ifeq ($(TARGET),u)
LIS_TARG = user
else
ifeq ($(TARGET),q)
LIS_TARG = qnx
else
LIS_TARG = NO_LIS_TARGET
endif
endif
endif



#
# See if modules are supported in the kernel
#
ifdef KMODULES
ifeq ($(KMODULES),y)
CONFIG_MODULES=y
ifdef KMODVERS
ifeq ($(KMODVERS),y)
CONFIG_MODVERSIONS=y
endif
endif
endif
endif

ifeq ($(CROSS_COMPILING),n)
ARCH := $(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/)
endif
 
export ARCH

#
# This is where we have LiS specific includes
#
LIS_INCL = $(SRCDIR)/include

#
# Calculate the names of some often used directories.
#
LIBDIR  = $(SRCDIR)/libc
LIBDIR32OVER64 = $(SRCDIR)/libc32over64
UTILDIR = $(SRCDIR)/util
HEADDIR = $(SRCDIR)/head
DRVRDIR = $(SRCDIR)/drivers/str

LIBOBJ  = $(LIBDIR)/$(LIS_TARG)
LIBOBJ32OVER64  = $(LIBDIR32OVER64)/$(LIS_TARG)
UTILOBJ = $(UTILDIR)/$(LIS_TARG)
HEADOBJ = $(HEADDIR)/$(LIS_TARG)
DRVROBJ = $(DRVRDIR)/$(LIS_TARG)

#
# Commonly used options
#
# These warning options should always be used within LiS
# Some options cause problems when compiling system header files.  You
# can enable these options if you like just to see if they cause you
# any problems.
#
PROBLEMS = -Wpointer-arith
PROBLEMS = 
XOPTS = -Wall -Wstrict-prototypes $(PROBLEMS)
XOPTS+= -fno-strict-aliasing -Wno-sign-compare -fno-common

#
# Compiler options for debugging and optimization.
#
ifeq ($(DBG_OPT),y)
XOPTS += -ggdb -O $(CC_OPT2)
else
XOPTS += $(CC_OPTIMIZE) $(CC_OPT2) -DINLINE=inline -DSTATIC=static -fomit-frame-pointer
endif

#
# For sparc-architecture we need some patches
#
ifeq ($(ARCH),sparc)
XOPTS += -D_SPARC_LIS_
endif

ifeq ($(ARCH),sparc64)
XOPTS += -D_SPARC64_LIS_
endif

#
# For PowerPC some more options are needed/useful
#
ifeq ($(ARCH),ppc)
XOPTS += -D__powerpc__ -fno-builtin -pipe -D_PPC_LIS_
endif
# For s390 & s390x architecture we need some bypasses
ifeq ($(ARCH),s390)
XOPTS += -D_S390_LIS_
endif
ifeq ($(ARCH),s390x)
XOPTS += -D_S390X_LIS_
endif

#
# This will make glibc define only the types defined in the good old SysV
# The only exception is caddr_t (defined in <sys/LiS/user-mdep.h>) and that
# is defined on all SVR4 systems anyway.
#
# When we do not have the types of _every_ standard, LiS will not compile
# if somebody tries to use non-SysV types. This helps to keep the LiS source
# clean, and makes it more portable.
#
ifeq ($(LIS_TARG),user)
XOPTS += -D_SVID_SOURCE -D_BSD_SOURCE
ifdef OSTYPE
ifeq ($(OSTYPE),SunOS5)
XOPTS += -DLIS_HAVE_MAJOR_T -DLIS_HAVE_O_UID_T
endif
endif
endif

#
# Use a define when running in a SMP environment.
#
ifdef CONFIG_SMP
XOPTS += -D__SMP__
endif
ifdef KSMP
ifeq ($(KSMP),y)
XOPTS += -D__SMP__
endif
endif
ifeq ($(ARCH),s390)
XOPTS += -D__BOOT_KERNEL_SMP
endif
ifeq ($(ARCH),s390x)
XOPTS += -D__BOOT_KERNEL_SMP
endif



#
# If LiS generated a modversions.h file, note that fact.  linux-mdep.h
# is sensitive to this.
#
ifdef LISMODVERS
XOPTS += -DLISMODVERS
endif
#
# Similarly, if we generated an autoconf.h file set a flag to that effect.
#
ifdef LISAUTOCONF
XOPTS += -DLISAUTOCONF
endif

#
# Use the name of the "get_empty_inode" routine figured out at
# installation time.  This is being changed to "new_inode".
# Communicated via include/sys/LiS/genconf.h
#
#XOPTS += -DGET_EMPTY_INODE=$(GET_EMPTY_INODE) 

#
# Always use LiS includes when compiling
#
XOPTS += -I$(LIS_INCL)

# use of the /usr/include/linux, /usr/src/linux/, ... symlinks is
# deprecated as of 2.4 kernels, which is the oldest LiS still supports
#
# the following (set by Configure) should all be to kernel-version-specific
# directories
#
ifeq ($(LIS_TARG),linux)
XOPTS += -I$(KINCL) $(KINCL_MACH_GENERIC) $(KINCL_MACH_DEFAULT)
ifeq ($(RH_71_KLUDGE),y)
XOPTS += -DRH_71_KLUDGE
endif
ifeq ($(SIGMASKLOCK),y)
XOPTS += -DSIGMASKLOCK
endif
ifeq ($(RCVOID),y)
XOPTS += -DRCVOID
endif
ifeq ($(NOKSRC),1)
XOPTS += -DNOKSRC
endif
endif

#
# If the user wants backward-compatible constants in stropts.h
# then include the option that will cause that to happen.
#
ifdef LIS_OLD_CONSTS
ifeq ($(LIS_OLD_CONSTS),y)
XOPTS += -DUSE_OLD_CONSTS
endif
endif

#
# If the user wants Solaris style cmn_err then set the flag for that
#
ifdef SOLARIS_STYLE_CMN_ERR
ifeq ($(SOLARIS_STYLE_CMN_ERR),y)
XOPTS += -DSOLARIS_STYLE_CMN_ERR
endif
endif

#
# Base name of user-level library
#
ifeq ($(LIS_TARG),user)
LIBBASE = LiSuser
else
LIBBASE = LiS
endif

#
# If the user wants LiS built for development purposes (lots of internal
# LiS debugging mechanisms - code path tracing, lock and semaphore use
# tracing, carry __FILE__ and __LINE__ on stack throughout all function
# calls.
#
ifeq ($(CONFIG_DEV),y)
XOPTS += -DCONFIG_DEV
endif

#
# Use Linux Kernel memory caches for performance
#
ifeq ($(USE_KMEM_CACHE),y)
XOPTS += -DUSE_KMEM_CACHE
endif

#
# Use Linux Kernel memory cache and native timer management routines
#
ifeq ($(USE_KMEM_TIMER),y)
XOPTS += -DUSE_KMEM_TIMER
endif

#
# Options for linking user-level programs
# Refer directly to the library for static linking.  Using -lLiSuser
# seems to cause "can't find -lc" from C compiler (mystery message).
#
ifeq ($(LIS_SHLIB),y)
ULOPTS = -Wl,-L$(LIBDIR) -l$(LIBBASE)
LIBDEPS =
else
ULOPTS = -static -Wl,-L$(LIBDIR) -l$(LIBBASE)
ULOPTS += $(LIBDIR)/lib$(LIBBASE).a
LIBDEPS = $(LIBDIR)/lib$(LIBBASE).a
endif

