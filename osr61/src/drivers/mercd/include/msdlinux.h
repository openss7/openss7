/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdlinux.h
 * Description                  : linux include
 *
 *
 **********************************************************************/

#ifndef _MSDLINUX_
#define _MSDLINUX_

#define PCI_VENDOR_ID_DLGC      0x10B5
#define PCI_VENDOR_ID_DLGC_NEW  0x1011
#define PCI_VENDOR_ID_DLGC_21555  0x8086   /* 21555*/

#define PCI_DEVICE_ID_DLGC_906D 0x906D
#define PCI_DEVICE_ID_DLGC_9054 0x9054
#define PCI_DEVICE_ID_DLGC_0046 0x0046
#define PCI_DEVICE_ID_DLGC_530D 0x530D
#define PCI_DEVICE_ID_DLGC_21555 0xB555    /* 21555 */
#define PCI_DEVICE_ID_DLGC_4143 0x4143     /* Pacific */
#define PCI_DEVICE_ID_DLGC_5356 0x5356     /* PCIe */

#define PCI_SUBVENDOR_ID_DLGC   0x12C7
#define PCI_SUBDEVICE_ID_DM3    0x4001
#define PCI_SUBDEVICE_ID_DISI   0x4002
#define PCI_SUBDEVICE_ID_DTI16  0x5001
#define PCI_SUBDEVICE_ID_ROZETTA 0x5002
#define PCI_SUBDEVICE_ID_DMVB	0x5005
#define PCI_SUBDEVICE_ID_NEWB	0x5007

// These include files must preceed driver specific headers 

/*
 * UNIX includes
 */
#ifndef __VMODULE__
#define __NO_VERSION__
//#include <linux/module.h>
#endif /* __VMODULE__ */

#include "msdnative.h"

#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/errno.h>

#if defined LiS || defined LIS || defined LFS
#include <sys/dlpi.h>
#include <sys/npi.h>
#include <sys/tihdr.h>
#if defined LiS || defined LIS
#include <sys/mkdev.h>
#endif
#endif

#ifndef LFS
#include <linux/config.h>
#endif
#include <linux/pci.h>
#ifdef LINUX24
#include <linux/init.h>
#endif
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#if defined LiS || defined LIS
#include <sys/cred.h>
#include <sys/osif.h>
#include <sys/lislocks.h>
#include <sys/lismem.h>
#else
#include <linux/spinlock.h>
#endif

#ifdef LINUX24
#ifndef IRQ_NONE
typedef void irqreturn_t;
#define IRQ_NONE
#define IRQ_HANDLED
#define IRQ_RETVAL(x)
#endif
#endif

/* 
 * Some common typedefs definitions 
 */
typedef void                   *IBLOCK_COOKIE;
typedef unsigned long 		DEV_ACC_ATTR;
typedef unsigned long		DEV_ACC_HANDLE;
typedef unsigned long 		SOFT_INTR_ID;
typedef unsigned int 		mercd_dpc_rt;

typedef struct pci_dev 	DEV_INFO;
typedef struct pci_dev  mercd_dev_info_T;
typedef struct pci_dev *pmercd_dev_info_T;

#if defined LiS || defined LIS || defined LFS
#ifdef LFS
typedef spinlock_t	mercd_mutex_T;
#else
typedef lis_spin_lock_t mercd_mutex_T;
#endif
typedef queue_t        *pmercd_strm_queue_sT;
#else
typedef spinlock_t 	mercd_mutex_T;
typedef msgb_t 		mblk_t;
#endif
typedef int             md_status_t;
typedef int             mercd_kcondvar_t;
typedef int             mercd_acc_attr_T;
typedef int            *pdevice_information_T ;
typedef void           *mercd_iblock_cookie_T;
typedef clock_t         mercd_clock_t;

typedef unsigned long   mercd_dev_acc_handle_T;
typedef unsigned long  *mercd_softintr_T;
typedef unsigned long  *pmercd_function_T;
typedef unsigned short *pmercd_adapter_map_T;
typedef struct	tasklet_struct	mercd_taskqueue_sT;

typedef struct _MercdMajor_ {
#if defined LiS || defined LIS || defined LFS
#define MERCD_MAX_MAJOR_DEV 5
#else
#define MERCD_MAX_MAJOR_DEV 1
#endif /* LiS */
	int major[ MERCD_MAX_MAJOR_DEV ] ;
	int openCount;

} MercdMajorTable;

#define PCI_CONF_SUBSYSID	0x2e
#define PCI_CONF_SUBVENID	0x2c
#define PCI_CONF_COMM		0x4
#define PCI_COMM_MAE		0x0002
#define PCI_COMM_ME		0x0004

#endif // _MSDLINUX_
