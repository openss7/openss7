/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gncfds.h
 * Description                  : generic constants
 *
 *
 **********************************************************************/

#ifndef __GNDEFS_H__
#define __GNDEFS_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


#define LD_NAMELEN               32    /* ASCIIZ logical device name size */
#define GB_NAMELEN               16    /* ASCIIZ board device name size */
#define GB_SUBLEN                4     /* ASCIIZ subdevice name size */
#define PM_NAMELEN               8     /* ASCIIZ PM name size */

#if defined(sparc)
#define GN_MAXHANDLES            1200  /* Max # handles on the driver */
#else
#define GN_MAXHANDLES            2096   /*prev 1024  handles on the driver */
#endif

#define GN_MAXCURCMDS            10    /* Max # simultaneous cmds on a device */
/*PCI can share the same interrupt and use different memory ranges for boards*/
#ifdef PCI_SPAN
#define GN_MAXIRQ                48    /* Max # PCI Boards on the system */
#else
#define GN_MAXIRQ                16    /* Max # IRQ levels on the system */
#endif

#define GN_MAXSLOTS              20    /* Max # back plane slot on the system */

#define GN_MAXEVENTS             32    /* Max # events on a device */

#define CMD_MAXREPLIES           8        /* Max # replies to a command */
#define CMD_NONBLOCKING          0x0001   /* Bitmask for nonblocking cmd. */
#define CMD_BLOCKING             0x0002   /* Bitmask for blocking cmd */
#define CMD_QUEUEBLOCKING        0x0004   /* Bitmask for queued blocking cmd */
#define CMD_PRIORITY             0x0008   /* Bitmask for priority cmd */

#define GN_DEVBUSY               0x01  /* Device busy flag for PM close */
#define GN_DEVQUEUED             0x02  /* Device queued flag for PM close */

#ifdef SOLARIS_24
#define GN_STOPFLAG		 0x1000
#endif /*SOLARIS_24*/

#endif


