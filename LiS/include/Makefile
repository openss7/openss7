include .config_mk

default:
	$(nothing)

install:
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
	$(Q)find . -type d $(Q_PRINT) \
		-exec install -d $(DESTDIR)$(pkgincludedir)/{} \
			>/dev/null \;
	$(Q)find . -type f -name "*.h" $(Q_PRINT) \
		-exec install -m 0644 {} $(DESTDIR)$(pkgincludedir)/{} \
			>/dev/null \;
ifneq ($(pkgincludedir),$(pkgdatatdir)/include)
#	add an 'include' symlink to pkgincludedir
	$(Q)(cd $(DESTDIR)$(pkgdatadir) && $(LN) $(pkgincludedir) include)
endif
ifneq ($(pkglibdir),$(pkgdatatdir))
#	add an 'include' symlink to pkgincludedir
	$(Q)(cd $(DESTDIR)$(pkglibdir) && $(LN) $(pkgincludedir) include)
endif

uninstall:
	$(nothing)

clean:
	$(nothing)
