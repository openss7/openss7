bin_PROGRAMS = iperf

LIBCOMPAT_LDADDS = @STRIP_BEGIN@ \
		   $(top_builddir)/compat/libcompat.a \
		   @STRIP_END@

AM_CPPFLAGS = @STRIP_BEGIN@ \
	      -I$(top_srcdir)/include \
	      @STRIP_END@

iperf_LDFLAGS = @CFLAGS@ @PTHREAD_CFLAGS@ @WEB100_CFLAGS@ @DEFS@

iperf_SOURCES = \
		Client.cpp \
		Extractor.c \
		Launch.cpp \
		List.cpp \
		Listener.cpp \
		Locale.c \
		PerfSocket.cpp \
		ReportCSV.c \
		ReportDefault.c \
		Reporter.c \
		Server.cpp \
		Settings.cpp \
		SocketAddr.c \
		gnu_getopt.c \
		gnu_getopt_long.c \
		main.cpp \
		service.c \
		sockets.c \
		stdio.c \
		tcp_window_size.c

iperf_DEPENDENCIES = \
		../include/Client.hpp \
		../include/Condition.h \
		../include/Extractor.h \
		../include/List.h \
		../include/Listener.hpp \
		../include/Locale.h \
		../include/Mutex.h \
		../include/PerfSocket.hpp \
		../include/Reporter.h \
		../include/Server.hpp \
		../include/Settings.hpp \
		../include/SocketAddr.h \
		../include/Thread.h \
		../include/Timestamp.hpp \
		../include/delay.hpp \
		../include/gettimeofday.h \
		../include/gnu_getopt.h \
		../include/headers.h \
		../include/inet_aton.h \
		../include/report_CSV.h \
		../include/report_default.h \
		../include/service.h \
		../include/snprintf.h \
		../include/util.h \
		../include/version.h

iperf_LDADD = $(LIBCOMPAT_LDADDS)
