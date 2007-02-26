##########@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@###################################
# Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
#
# This software program is licensed subject to the GNU General
# Public License (GPL). Version 2, June 1991, available at
# http://www.fsf.org/copyleft/gpl.html
#
###################################@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@##########
#**************************************************************************
# File Name                    : Makefile
# Description                  : Driver Compilation
#**************************************************************************/
 
MAJOR_VER = 4
MINOR_VER = 0
PATCH_VER = 0
FULL_VER = $(MAJOR_VER).$(MINOR_VER).$(PATCH_VER)
CC = gcc
LD = ld
LDCOMBINEFLAGS = -r

KERNBUILD=/lib/modules/$(shell uname -r)/build
KERNINCLUDEDIR=$(KERNBUILD)/include
KERNMACHINCDIR=$(KERNINCLUDEDIR)/asm/mach-default
LOCALINCLUDEPATH=$(PWD)/include
DM3INC=$(PWD)/../mercd/include


# Option to build for 2.4 or 2.6 kernels
KERNELMAJOR = $(shell uname -r | cut -b3)
ifeq ($(KERNELMAJOR),6)
	SYSTEM_DEFINES = -D__KERNEL__ -DMODULE -DLINUX
	CFLAGS  += $(SYSTEM_DEFINES) -I$(KERNINCLUDEDIR) -I$(KERNMACHINCDIR) -I$(LOCALINCLUDEPATH) -I$(DM3INC) -DTASK -DPMACD_VERSION=$(FULL_VER)
	MODULE = pmac
	obj-m   := pmac.o
	pmac-objs := source/pmacd_board.o source/pmacd_interrupt.o source/pmacd_mf.o source/pmacd_proc.o source/pmacd_control.o source/pmacd_linux_entry.o source/pmacd_parameters.o source/pmacd_stream.o source/pmacd_driver.o source/pmacd_linux_pci.o source/pmacd_pmb.o source/pmacd_version.o
	MAKECOMMAND = $(MAKE) -C $(KERNBUILD) V=1 SUBDIRS=$(PWD) modules
default:
	$(MAKECOMMAND);\
	rm -f pmac.mod.c pmac.mod.o pmac.o

clean:
	/bin/rm -f source/*.o pmac.* .depend .pmac* source/.*.o.cmd
	/bin/rm -f $(PMACDRVL_BIN_EXT)/pmac.*
	/bin/rm -rf .tmp_versions/

	
else
	CFLAGS  = -I$(KERNINCLUDEDIR) -I$(LOCALINCLUDEPATH) -I$(DM3INC)  -D__KERNEL__ \
		-DMODULE -DLINUX24 -Wall -Wstrict-prototypes \
		-O2 -fomit-frame-pointer -pipe -fno-strength-reduce -march=i386 \
		-falign-loops=1 -falign-jumps=2 -falign-functions=2 -DCPU=386 \
		-DPMACD_VERSION=$(FULL_VER)

	PMAC_OBJS = source/pmacd_board.o source/pmacd_interrupt.o source/pmacd_mf.o source/pmacd_proc.o source/pmacd_control.o source/pmacd_linux_entry.o source/pmacd_parameters.o source/pmacd_stream.o source/pmacd_driver.o source/pmacd_linux_pci.o source/pmacd_pmb.o source/pmacd_version.o
	EXE_NAME= pmac
	MAKECOMMAND = $(CC) -c $(CFLAGS) -o $@ $*.c
.c.o:
	$(MAKECOMMAND)

default:
	$(MAKE) all

clean:
	/bin/rm -rf source/*.o .depend
	/bin/rm -rf $(EXE_NAME).*

endif


#
# Rule for the default targets: default depend
#
depend:
	$(CC) $(CFLAGS) -MM  $(PMAC_OBJS:.o=.c) > .depend

all::${EXE_NAME}.o

${EXE_NAME}.o: ${PMAC_OBJS}
	$(LD) $(LDCOMBINEFLAGS) ${PMAC_OBJS} -o $@

