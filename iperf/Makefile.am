SUBDIRS = compat doc include src

include am/openss7.am

AM_CPPFLAGS = @STRIP_BEGIN@ \
	      -I$(top_srcdir)/include \
	      @STRIP_END@

EXTRA_DIST = autogen.sh

#
#   Rules for making rpms
#
AM_RPMFLAGS			=
AM_SRPMFLAGS			=

include am/rpm.am

# vim: ft=automake
