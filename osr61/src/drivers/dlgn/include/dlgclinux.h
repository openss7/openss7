/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgclinux.h
 * Description                  : linux headers
 *
 *
 **********************************************************************/

#ifdef LFS
#define _LIS_SOURCE	1
#define _SVR4_SOURCE	1
#include <sys/os7/compat.h>
#else
#define VERSION(ver,rel,seq) (((ver)<<16) | ((rel)<<8) | (seq))
#define BREAKPOINT() asm("   int $3");

#ifndef MODULE
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif


#include <sys/LiS/config.h>
#if (CLONE__CMAJOR_0 > 220)
#include <sys/LiS/module.h>
#endif

#include <linux/version.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/pci.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#if LINUX_VERSION_CODE < 0x020400
#include <linux/malloc.h>
#else
#include <linux/slab.h>
#endif
#include <linux/vmalloc.h>

#include <linux/init.h>

#include <linux/delay.h>
#include <linux/ioctl.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#endif

#include <sys/stream.h>
#ifndef LFS
#include <sys/LiS/mod.h>
#endif
#include <sys/stropts.h>
#include <sys/dki.h>
#include <sys/dlpi.h>
#include <sys/ddi.h>
#include <sys/cmn_err.h>

#ifndef LFS
#if LINUX_VERSION_CODE < 0x020400
#include <sys/lispci.h>
#include <sys/osif.h>
#endif

#ifndef MODULE
#include <sys/osif.h>
#endif
#endif
