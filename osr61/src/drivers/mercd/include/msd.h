/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msd.h
 * Description                  : driver main include
 *
 *
 **********************************************************************/

#ifndef _MSD_H
#define _MSD_H

// INCLUDE THIS FIRST TO HAVE APPROPRIATE
// _8_BIT_INSTANCE & LITTLEENDIAN DEFINES DEFINED
#include "qhosttypes.h" /* 8BIT/16BIT defs */


#define OS_SOLARIS      1
#define OS_UNIXWARE     2

#define DM3_PLATFORM_VME        1
#define DM3_PLATFORM_PCI        2

#define MAX_INTERFACE_FUNCTION  81   //Was 64
#define MAX_OSAL_FUNC  		42
#define MAX_DHAL_FUNC  		42

#if defined LiS || defined LIS || defined LFS
#ifdef LFS
#define	_LIS_SOURCE	1
#endif
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/ddi.h>
#include <sys/cmn_err.h>
#ifdef LFS
#include <sys/strconf.h>
#include <sys/kmem.h>
#endif
#endif

#ifdef MERCD_SOLARIS
// These include files must preceed driver specific headers 

#include <sys/types.h>
#include <sys/kmem.h>
#include <sys/map.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/strlog.h>
#include <sys/log.h>
#include <sys/sunddi.h>
#include <sys/modctl.h>
#include <sys/devops.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/stat.h>
#include <sys/debug.h>
#include <sys/pci.h>

#endif  /* MERCD_SOLARIS */

#define MERCD_PCI_PLX_CONF_ADDR  0x10
#define MERCD_PCI_PLX_CONF_SRAM  0x18
#define MERCD_PCI_CONF_COMM_VALUE 0x147
#define PCI_PLX_ENUM_ASSERT 0xC0


#ifdef MERCD_UNIXWARE
// These include files must preceed driver specific headers 

#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/systm.h>
#include <sys/strlog.h>
#include <sys/log.h>
#include <sys/conf.h>
#include <sys/kmem.h>
#include <sys/stat.h>

#include <assert.h>
#include <sys/file.h>
#include <sys/fcntl.h>
#include <sys/cred.h>
#include <sys/termio.h>
#include <sys/termios.h>
#include <sys/termiox.h>
#include <sys/strtty.h>
#include <sys/eucioctl.h>
#include <sys/signal.h>
#include <sys/debug.h>

#include <sys/resmgr.h>
#include <sys/confmgr.h>
#include <sys/cm_i386at.h>
#include <sys/moddefs.h>


/* 
 * Some common typedefs definitions 
 */
typedef unsigned long 		DEV_ACC_ATTR;
typedef unsigned long		DEV_ACC_HANDLE;
typedef void * 			IBLOCK_COOKIE;
typedef unsigned long 		SOFT_INTR_ID;
typedef unsigned int		DEV_INFO;

#define PCI_CONF_SUBSYSID	0x2e
#define PCI_CONF_SUBVENID	0x2c
#define PCI_CONF_HSCSR		0x4a
#define PCI_CONF_COMM		0x4
#define PCI_COMM_MAE		0x0002
#define PCI_COMM_ME			0x0004

#endif /* MERCD_UNIXWARE */

#ifdef MERCD_LINUX
#include "msdtypes.h"
#include "msdlinux.h"
#if !defined LiS && !defined LIS && !defined LFS
#include "ctimod.h"
#include "msdnative.h"
#endif
#include "msdlinuxlock.h"
#include "msdlinuxwwmacros.h"
#endif /* MERCD_LINUX */


#include "msdtypes.h"
#include "msdmacro.h"
#include "msdparam.h"
#include "msdqueue.h"
#include "msddrvparam.h"
#include "msdmsgdef.h"
#include "msdmsgs.h"

#ifdef MERCD_SOLARIS
#include "msdsollock.h"
#include "msdsoltypes.h"
#endif 

#include "msdsramif.h"
#include "msdsram.h"
#include "qdrverr.h"
#include "msdvbrd.h"
#include "msddbg.h"
#include "msdwwtypes.h"
#include "msdwwdefs.h"
#include "msdstruct.h"


#include "msdmercif.h"
#include "msdshrdata.h"

#include "mercd_osal.h"
#include "mercd_dhal.h"
#include "msdmmgr.h"
#include "msddlver.h"

#ifdef MERCD_UNIXWARE
#include "msduxw7lock.h"
#endif

#define PCI_CONF_HSCSR		0x4a
#define PCI_HSCSR_CLR_INTR	0x2
#define CPCI_HSCSR_CLR_INTR     0x80
//THIRD_ROCK Support
#define PCI_CONF_21554_HSCSR    0xEE


#define  MERCD_PCI_BRIDGE_MAP      0
#define  MERCD_PCI_SRAM_MAP     1

#endif /* _MSD_H */
