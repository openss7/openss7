
info_TEXINFOS = strxnet.texi

EXTRA_DIST = gpl.texi lesser.texi fdl.texi

all-local: ps pdf

docdir = @datadir@/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

install-data-local: all-local
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	$(INSTALL_DATA) strxnet.pdf $(DESTDIR)$(docdir)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f $(DESTDIR)$(docdir)/strxnet.pdf

