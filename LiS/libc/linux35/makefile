#
# This is a stand-alone makefile that will make the library
# routines in 2.0.36 style.
#
SRC	= ..
CC	= cc -O2 -c
POLL	= poll.o
LIBBASE	= LiS
LIBDIR	= ..
LIBOBJS	= getpmsg.o putpmsg.o $(POLL)
XOPTS	= -I.

my_default:	all
	install $(LIBDIR)/lib$(LIBBASE).a .

include ../mk.bdy
