#
# LiS configuration
#
LIS_HOME = ../..
include $(LIS_HOME)/config.mk

#
# Environment of USER means the intent is to run the code at user level 
# for testing.
#
OPT = -DUSER

XOBJS = usrio.o port.o


include $(HEADDIR)/mk.bdy

dep: common-dep
	$(CC) -M -DDEP usrio.c port.c >>.depend

