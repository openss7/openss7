include .config_mk

default:
	$(nothing)

install:
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
	$(Q)install -d $(DESTDIR)$(mandir)/man3
	$(Q)install -d $(DESTDIR)$(mandir)/man8
	$(Q)install -d $(DESTDIR)$(mandir)/man9
	$(Q)install -m 0644 man3/* $(DESTDIR)$(mandir)/man3/.
	$(Q)install -m 0644 man8/* $(DESTDIR)$(mandir)/man8/.
	$(Q)install -m 0644 man9/* $(DESTDIR)$(mandir)/man9/.

uninstall:
	$(nothing)
