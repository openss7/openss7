
AUTOM4TE = autom4te

TESTS = testsuite

TESTSUITE = testsuite

installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) $(TESTSUITE)

AUTOTEST = $(AUTOM4TE) --language=autotest

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOTEST) -I $(srcdir) $@.at -o $@.tmp
	mv -f $@.tmp $@

EXTRA_DIST = testsuite.at testsuite
