include .config_mk

OPT	= -DQNX
BINDIR	= /usr/local/bin
XLIBS	= -L$(Groot)/rsys/lib/qnx -lstrmio.a

all:	streams

streams:	streams.o
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $^ $(XLIBS)

streams.o:	../streams.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -I$(SRCDIR)/include $(OPT) -o $@ -c $^
