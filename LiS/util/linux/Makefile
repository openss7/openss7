LIS_HOME = ../..
include $(LIS_HOME)/config.mk

OPT	= -DLINUX
BINDIR	= /usr/sbin
XUTILS  = ldltest ldlconfig test test2 fattach fdetach \
	  oc rwf mkfifo psfd pafd thrtst
XINST	= install_xutils
XUNINST	= uninstall_xutils
XCLEAN	= makenodes.c
ULOPTS += -lpthread

include $(UTILDIR)/mk.bdy

#thrtst:	thrtst.c
#	$(CC) -o $@ $^ $(ULOPTS) -lpthread
	
#strtst:	../strtst.c
#	$(CC) -o $@ $^ $(ULOPTS) -lpthread
	
install_xutils:	ldltest ldlconfig fattach fdetach
	cp $^ $(INST_SBIN)/.

uninstall_xutils:
	-(cd $(INST_SBIN); rm -f ldltest ldlconfig fattach fdetach)
