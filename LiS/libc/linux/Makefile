LIS_HOME = ../..
include $(LIS_HOME)/config.mk

PRELDOBJS = getpmsg.o getmsg.o putpmsg.o putmsg.o poll.o \
	  fattach.o fdetach.o isastream.o # mknod.o

LIBOBJS = $(PRELDOBJS) pipe.o

CP_OPTS	= -f
XOPTS  += -D__KERNEL_SYSCALLS__

include ../mk.bdy


#
# Dependency stuff
#

-include .depend

dep:
	$(CC) -M -DDEP $(LIBOBJS:%.o=$(LIBDIR)/%.c) >.depend

