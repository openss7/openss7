
include ../am/openss7.am

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

SUBDIRS = . man manual specs projects

if PERFORM_CHECKS
## PERFORM_CHECKS
dist_check_SCRIPTS	+= $(top_srcdir)/scripts/check_decls
TESTS_ENVIRONMENT	+= PKG_MANPATH='$(PKG_MANPATH)' \
			   mandir='$(mandir)'
CLEANFILES		+= $(top_builddir)/check_decls.log
DISTCLEANFILES		+= *.err *.out
## PERFORM_CHECKS
endif

## PKG_BUILD_INDEP
endif
