##########@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@###################################
# Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
#
# This software program is licensed subject to the GNU General
# Public License (GPL). Version 2, June 1991, available at
# http://www.fsf.org/copyleft/gpl.html
#
###################################@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@##########
#**************************************************************************
#   FILE:        Makefile
#
#   PURPOSE:     Makefile for linux driver kernel interface module.
#
#   HISTORY:	 2.4 and 2.6 kernel compliant
#
#**************************************************************************/
CC = gcc -c
KERNELRELEASE = $(shell uname -r)
KERNELBUILDDIR = /lib/modules/$(KERNELRELEASE)/build
ASM_INC = $(KERNELBUILDDIR)/include/asm/mach-default
PWD = $(shell pwd)

SOURCE = ctimod.c

# Option to build for 2.4 or 2.6 kernels
KERNELMAJOR = $(shell uname -r | cut -b3)
ifeq ($(KERNELMAJOR),6)
MODULE = /lib/modules/$(KERNELRELEASE)/misc/ctimod.ko
CFLAGS += -D__KERNEL__ -DMODULE -I$(KERNELBUILDDIR)/include -I$(ASM_INC)
obj-m := ctimod.o

default:
	@if [ -f /lib/modules/$(KERNELRELEASE)/misc ]; then \
	echo ""; \
	else \
	mkdir -p /lib/modules/$(KERNELRELEASE)/misc; \
	fi; \
	$(MAKE) -C $(KERNELBUILDDIR) SUBDIRS=$(PWD) V=1 modules; \
	cp -f ctimod.ko /lib/modules/$(KERNELRELEASE)/misc/; \
	rm -f ctimod.ko ctimod.mod.c ctimod.mod.o ctimod.o ; \
	rm -rf .tmp_versions .ctimod*

else
MODULE = /lib/modules/$(KERNELRELEASE)/misc/ctimod.o
CFLAGS = -D__KERNEL__ -DMODULE  -I/$(KERNELBUILDDIR)/include -I$(ASM_INC)
OBJECT = $(SOURCE:.c=.o)
obj-m := $(MODULE)

all: $(OBJECT)
.c.o:
	@if [ -f /lib/modules/$(KERNELRELEASE)/misc ]; then \
        $(CC) $(CFLAGS) -o /lib/modules/$(KERNELRELEASE)/misc/$@ .c; \
        else \
        mkdir -p /lib/modules/$(KERNELRELEASE)/misc; \
        $(CC) $(CFLAGS) -o /lib/modules/$(KERNELRELEASE)/misc/$@ $*.c; \
        fi
endif

clean:
	@rm -f $(MODULE)

uninstall:
	@rm -f $(MODULE)
