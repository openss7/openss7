LIS_HOME = ../..
SRC=..
include $(LIS_HOME)/config.mk

OPT	= -DQNX
BINDIR	= /usr/local/bin
XLIBS	= -L$(Groot)/rsys/lib/qnx -lstrmio.a

all:	streams

streams:	streams.o
	$(CC) -o streams streams.o $(XLIBS)

streams.o:	../streams.c
	$(CC) -I../../include $(OPT) -c streams.o  ../streams.c
