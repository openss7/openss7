include .config_mk

#
#  Makefile for LiS drivers.
#
#  This one builds drivers for user level.
#

-include $(LIS_HOME)/drvrconf.mk

OPT = -DUSER -D__KERNEL__
DRVRS = loop.o relay.o clone.o minimux.o printk.o nullstrm.o\
 timod.o tirdwr.o sad.o fifo.o pipemod.o putst.o
CC += -DLiS $(OPT) $(XOPTS)

# Do we want debugging?
ifeq ($(DBG_OPT),notused)
ifeq ($(DBG_OPT),y)
OPT += -g
endif
endif


include $(DRVRDIR)/mk.bdy


#
# Rule for relay3, a special compilation of relay.c
#
relay.o:	$(DRVRDIR)/relay.c
	$(Q_ECHO) $(tag_CCm)$(relpwdtarget)
	$(CC) $(MODOPT) -DLIS_OBJNAME=$* -DRELAY3 -c -o $@ $<


clean: common-clean

realclean: common-realclean

dep: common-dep
