LIS_HOME = ../..
include $(LIS_HOME)/config.mk

XOPTS += -DUSER
LIBOBJS = makenodes.o $(LIS_HOME)/streams.o


include ../mk.bdy


#
# Dependency stuff
#

-include .depend

ifneq ($(strip $(wildcard makenodes.c)),)

dep:
	$(CC) -M -DDEP makenodes.c >.depend

else

dep:
	>.depend

endif

