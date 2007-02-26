##########@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@###################################
# Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
#
# This software program is licensed subject to the GNU General
# Public License (GPL). Version 2, June 1991, available at
# http://www.fsf.org/copyleft/gpl.html
#
###################################@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@##########
#######################################################################
# File Name                    : Makefile
# Description                  : Driver Compilation
#######################################################################

# Compiler
CC=gcc

DEBUGOPTS=
LDCOMBINEFLAGS = -r
KERNELRELEASE = $(shell uname -r)
KERNELBUILDDIR = /lib/modules/$(shell uname -r)/build
ASM_INC = $(KERNELBUILDDIR)/include/asm/mach-default

INC_PATH = -I$(PWD)/include 

# Option to build for 2.4 or 2.6 kernels
KERNELMAJOR = $(shell uname -r | cut -b3)
ifeq ($(KERNELMAJOR),6)
	
	SYSTEM_DEFINES = -D__KERNEL__ -DMODULE -DLINUX
	LOCAL_DEFINES = -DPPS -DMERCD_LINUX -DMERCD_PCI -DMERCD_DATATYPE_INTEL  -DSRAM_DRVR_DMA_SUPPORT
	CFLAGS += $(SYSTEM_DEFINES) $(LOCAL_DEFINES) -I/$(KERNELBUILDDIR)/include -I$(ASM_INC) $(INC_PATH)

	MODULE = mercd
	obj-m := $(MODULE).o
	mercd-objs := source/mercd.o source/mercd_native.o source/mercd_native_util.o source/mercd_linux_pci_dhal.o source/mercd_linux_osal.o source/mercd_ossdl_linux.o source/mercd_osshl_linux_pci.o source/mercd_linux_entry.o source/msdbndmgr.o source/msdbndutl.o source/msdbrdmgr.o source/msdbrdutl.o source/msddbg.o source/msddpc.o source/msddrvrmgr.o source/msdfwmgr.o source/msdmsgmgr.o source/msdqueue.o source/msdrcv.o source/msdshrdata.o source/msdsnd.o source/msdstrmmgr.o source/msdstrmutl.o source/msdsupp.o source/msdtime.o source/mercd_linux_dma_pci.o source/mercd_ww_ossdl_linux.o source/msdwwdpc.o source/msdwwinitmngr.o source/msdwwinitmngrutl.o source/msdwwmsgutl.o source/msdwwrcv.o source/msdwwsnd.o source/msdwwstrmutl.o source/msdwwtime.o source/msdwwutl.o

default:
	$(MAKE) -C $(KERNELBUILDDIR) SUBDIRS=$(PWD) modules;

clean:
	/bin/rm -f source/*.o .depend .mercd.* source/.*.o.cmd 
	/bin/rm -rf  .tmp_versions/
	/bin/rm -f mercd.*

else

	PROTO_DEFINES = -DFUNCPROTO=15 -DNARROWPROTO
	CXXSTD_DEFINES = -Dlinux -D__i386__ -D_POSIX_SOURCE -D_BSD_SOURCE -D_GNU_SOURCE -DX_LOCALE  $(PROTO_DEFINES)
	SYSTEM_DEFINES = -D__KERNEL__ -DMODULE -Wstrict-prototypes -Wpointer-arith -O1 -DINLINE=inline -DSTATIC=static -fomit-frame-pointer -fno-common
	LOCAL_DEFINES  =  -DRADM_SUPPORT -DLINUX -ULiS -DPPS -DMERCD_LINUX -DMERCD_PCI -DMERCD_DATATYPE_INTEL  -DSRAM_DRVR_DMA_SUPPORT -DLIS_OBJNAME=mercd
	GCCFLAGS= -c $(CXXSTD_DEFINES) $(SYSTEM_DEFINES) $(LOCAL_DEFINES) -DLINUX24 -I/$(KERNELBUILDDIR)/include -I$(ASM_INC) $(INC_PATH)
	CCFLAGS = $(GCCFLAGS)

	EXE_NAME= mercd
	MERCD_OBJS = source/mercd.o source/mercd_native.o source/mercd_native_util.o source/mercd_linux_pci_dhal.o source/mercd_linux_osal.o source/mercd_ossdl_linux.o source/mercd_osshl_linux_pci.o source/mercd_linux_entry.o source/msdbndmgr.o source/msdbndutl.o source/msdbrdmgr.o source/msdbrdutl.o source/msddbg.o source/msddpc.o source/msddrvrmgr.o source/msdfwmgr.o source/msdmsgmgr.o source/msdqueue.o source/msdrcv.o source/msdshrdata.o source/msdsnd.o source/msdstrmmgr.o source/msdstrmutl.o source/msdsupp.o source/msdtime.o source/mercd_linux_dma_pci.o source/mercd_ww_ossdl_linux.o source/msdwwdpc.o source/msdwwinitmngr.o source/msdwwinitmngrutl.o source/msdwwmsgutl.o source/msdwwrcv.o source/msdwwsnd.o source/msdwwstrmutl.o source/msdwwtime.o source/msdwwutl.o

.c.o:	 
	$(CC) $(GCCFLAGS) -o $@ $*.c 

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
	$(CC) $(GCCFLAGS) $(INC_PATH) -MM  $(MERCD_OBJS:.o=.c) > .depend

all::${EXE_NAME}.o

${EXE_NAME}.o: ${MERCD_OBJS} 
	$(LD) $(LDCOMBINEFLAGS) ${MERCD_OBJS} -o $@
