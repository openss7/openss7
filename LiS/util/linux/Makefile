include .config_mk

OPT	= -DLINUX
XUTILS  = ldltest ldlconfig test test2 fattach fdetach \
	  oc rwf mkfifo psfd pafd thrtst
XINST	= install_xutils
XUNINST	= uninstall_xutils
XCLEAN	= makenodes.c
ULOPTS += -lpthread

include $(UTILDIR)/mk.bdy

default:
	$(nothing)

#thrtst:	thrtst.c
#	$(CC) -o $@ $^ $(ULOPTS) -lpthread
	
#strtst:	../strtst.c
#	$(CC) -o $@ $^ $(ULOPTS) -lpthread
	
install_xutils:	ldltest ldlconfig fattach fdetach
	$(Q)install -d $(DESTDIR)$(sbindir)
	$(Q)install $^ $(DESTDIR)$(sbindir)

uninstall_xutils:
	-$(Q)(cd $(DESTDIR)$(sbindir) && \
		rm -f ldltest ldlconfig fattach fdetach)
