include .config_mk

XOPTS += -DUSER
LIBOBJS = makenodes.o $(SRCDIR)/streams.o


include $(LIBDIR)/mk.bdy


#
# Dependency stuff
#

-include .depend

ifneq ($(strip $(wildcard makenodes.c)),)

dep:
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(Q)$(CC) -M -DDEP makenodes.c >.depend

else

dep:
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(Q)>.depend

endif
