#
# This file includes and calculates some make variables that are useful
# when compiling LiS and utilities.
# The makefile that includes this one will have to define LIS_HOME to the
# root of the LiS tree.
#
# It must not contain any rules.
#

#
# Get LiS configuration, if it exists.
#
-include $(LIS_HOME)/config.in

#
# These variables could be configurable some day
#
# We are moving libLiS.{so,a} to /lib from /usr/lib.  Some more recent
# versions of Linux process /etc/ld.so.preload *prior* to mounting
# local file systems such as /usr.
#
INST_LIB = /lib
INST_LIB = /usr/lib
INST_BIN = /usr/bin
INST_SBIN = /usr/sbin
MANDIR = /usr/man

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
-include $(LIS_HOME)/cross-compile
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
 
#
# This is where we have LiS specific includes
#
LIS_INCL = $(LIS_HOME)/include

#
# Calculate the names of some often used directories.
#
LIBDIR  = $(LIS_HOME)/libc
UTILDIR = $(LIS_HOME)/util
HEADDIR = $(LIS_HOME)/head
DRVRDIR = $(LIS_HOME)/drivers/str

LIBOBJ  = $(LIBDIR)/$(LIS_TARG)
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
# NOTE: gcc 2.96 [20000731 (Red Hat Linux 7.1 2.96-98)] generates bad
# code that gets the SP out of whack with -O3.  So use -O2 for safety.
#
ifeq ($(DBG_OPT),y)
XOPTS += -g -O -DINLINE="" -DSTATIC="" -DLIS_SRC=\"$(LIS_HOME)\"
else
XOPTS += $(CC_OPTIMIZE) -DINLINE=inline -DSTATIC=static -fomit-frame-pointer
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

#
# If running in the Linux kernel we need the kernel includes too.
# Only the Linux kernel target use this include path.
# The default link /usr/include/linux -> ../src/linux/include/linux
# could be to the wrong kernel!
#
ifeq ($(LIS_TARG),linux)
XOPTS += -I$(KSRC)/include $(MACHINE_INCL)
ifneq ($(KSRC),/usr/src/linux)
XOPTS += -I/usr/src/linux/include
endif
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

