#
# Prototype makefile for package subdirectories
#
LIS_HOME = ../..
include $(LIS_HOME)/config.mk

Space.o: Space.c $(LIS_INCL)/sys/LiS/config.h
	$(CC) -c $(XOPTS) -I$(LIS_INCL) -I$(LIS_INCL)/sys/LiS -o $@ $<

modules:

clean:
	-rm -f Space.o
