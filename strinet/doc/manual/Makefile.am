
info_TEXINFOS = strinet.texi

EXTRA_DIST = gpl.texi lesser.texi fdl.texi

all-local: ps pdf

docdir = @datadir@/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

install-data-local: all-local
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	$(INSTALL_DATA) strinet.pdf $(DESTDIR)$(docdir)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f $(DESTDIR)$(docdir)/strinet.pdf

