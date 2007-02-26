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
# Description                  : streams-gpioDriver.o
#######################################################################

# Compiler
CC=gcc -c

VERSIONS = -DPKGVER="\"Version 6.1 \"" -DOSTYPEVER="\"Linux\"" -DCOPYRIGHT="\"Copyright (C) 2005 Intel Corporation\""

KERNEL_CFLAGS = -Wall -Wundef -Wno-trigraphs -fno-strict-aliasing -fno-common -fno-stack-protector -O2 -Wp,-D_FORTIFY=2 -Werror -fomit-frame-pointer -fasynchronous-unwind-tables -pipe -msoft-float -mpreferred-stack-boundary=2 -march=i586 -mtune=generic -mregparm=3 -Wdeclaration-after-statement -Wno-pointer-sign -ffreestanding
KERNEL_CPPFLAGS = -nostdinc -iwithprefix include -DLINUX -D__KERNEL__ -I/lib/modules/2.6.17-11-generic/build/include -Iinclude2 -I/usr/src/linux-source-2.6.17/include -include /lib/modules/2.6.17-11-generic/build/include/linux/autoconf.h -I/usr/src/linux-source-2.6.17/include/asm-i386/mach-default '-DKBUILD_STR(s)=\#s' '-DKBUILD_BASENAME=KBUILD_STR(gpio)' -DMODULE -DEXPORT_SYMTAB

LOCAL_DEFS = -DISA_SPAN -DPCI_SPAN -DPCI_SUPPORT -DBRI_SUPPORT -D_STATS_ -D__DRVDEBUG_H__ -DNEWSTYLE_DRIVER -DLIS_OBJNAME_STR=\"gpio\" -DLIS_OBJNAME=gpio -D_ISDN -DSB_PROGRESS -DDCB -DNOSBA -DDEMO_BRDID_FIX -DPCI_ANT -Di386 -DUSL -DWITH_ANTARES -DSE_NOSLP
CXXSTD_DEFS = -Dlinux -D__i386__ -D_POSIX_SOURCE -D_BSD_SOURCE -D_GNU_SOURCE -DX_LOCALE -DFUNCPROTO=15 -DNARROWPROTO 
SYSTEM_DEFS =  -DUNIX -DLINUX -D_ISDN -DINLINE=inline -DSTATIC=static $(KERNEL_CFLAGS)

INC_PATH = -DLFS=1 -I/usr/include/strxns -I/usr/include/strcompat -I/usr/include/streams $(KERNEL_CPPFLAGS) -I../dlgn/include -I ../gncfd/include -I./include
KER_PATH = # -I/lib/modules/$(shell uname -r)/build/include
ASM_INC = # $(KER_PATH)/asm/mach-default

LDCOMBINEFLAGS = -r
GCCFLAGS= $(CXXSTD_DEFS) $(SYSTEM_DEFS) $(ASM_INC) $(KER_PATH) $(INC_PATH) $(LOCAL_DEFS) $(VERSIONS)

EXE_NAME= streams-gpioDriver

ALL_OBJS = source/gpiodrv.o

# 
# New default rules for building objects
#
.c.o:	 
	$(CC)  $(GCCFLAGS) -o $@ $*.c 

#
# Now the rules
#
default: 
	$(MAKE) all

#
# Rule for the default targets: default depend
#
depend:
	$(CC) $(GCCFLAGS) $(INC_PATH) -MM  $(ALL_OBJS:.o=.c) > .makefile.dep

#
# Rule for the default targets: default clean 
#
clean: 
	/bin/rm -rf source/*.o .makefile.dep
	/bin/rm -rf $(EXE_NAME)*


all::${EXE_NAME}.o

${EXE_NAME}.o: ${ALL_OBJS} 
	$(LD) $(LDCOMBINEFLAGS) ${ALL_OBJS} -o $@
      
#
# Only build these tag files if debugging
#
cscope:
	ksh -c "cscope -Iinclude source/*.c "



