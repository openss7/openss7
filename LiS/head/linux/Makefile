include .config_mk

#
# Environment of LINUX means the intent is to link the code into the
# Linux kernel.
#
OPT = -DLINUX

#
# Does the user want to run the queues using bottom half or kernel
# thread.  Set an option to be used when compiling linux-mdep.c
#
ifeq ($(STRMS_QUEUES),b)
OPT += -DUSE_BH
else
OPT += -DUSE_KTHREAD
endif

# Are we compiling to a Linux kernel with module versions?
ifdef CONFIG_MODULES
  ifdef CONFIG_MODVERSIONS
    OPT += -DMODVERSIONS
  endif 
endif 

# Are we compiling LiS as a loadable kernel module?
ifeq ($(CONFIG_STREAMS),m)
  OPT += -DMODULE
  ifeq ($(KBUILD_NEEDS_MODNAME),y)
    OPT += -DKBUILD_MODNAME=streams
  endif
endif 

#ifeq ($(DBG_OPT),y)
#  OPT += -DDEBUG
#endif

XOBJS = poll.o

#
# For any kernel later than 2.0.x we can include our own PCI interface
# code.
#
ifeq ($(KVER_MAJOR),2)
ifeq ($(KVER_MINOR),0)
else
  ifneq ($(ARCH),s390)
  ifneq ($(ARCH),s390x)
  XOBJS += lispci.o
  endif
  endif
endif
endif

#
# include LiS's memory allocator interface.
#
XOBJS += lismem.o

#
# include LiS's locking code
#
XOBJS += lislocks.o

#
# include the LiS exported symbol list
#
XOBJS += exports.o

include $(HEADDIR)/mk.bdy

modules: streamshead.o

dep: common-dep
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(Q)$(CC) -M -DDEP $(HEADDIR)/poll.c >>.depend
