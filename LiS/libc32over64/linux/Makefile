include .config_mk

PRELDOBJS = getpmsg.o getmsg.o putpmsg.o putmsg.o \
	  fattach.o fdetach.o isastream.o # mknod.o poll.o

LIBOBJS = $(PRELDOBJS) pipe.o

CP_OPTS	= -f
XOPTS  += -D__KERNEL_SYSCALLS__
XOPTS  += -DBLD32OVER64 -m31
LDOPTS = -melf_s390

include $(LIBDIR32OVER64)/mk.bdy


#
# Dependency stuff
#

-include .depend

dep:
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(CC) -M -DDEP $(LIBOBJS:%.o=$(LIBDIR)/%.c) >.depend
