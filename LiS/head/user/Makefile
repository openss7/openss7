include .config_mk

#
# Environment of USER means the intent is to run the code at user level 
# for testing.
#
OPT = -DUSER

XOBJS = usrio.o port.o


include $(HEADDIR)/mk.bdy

dep: common-dep
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(Q)$(CC) -M -DDEP usrio.c port.c >>.depend

