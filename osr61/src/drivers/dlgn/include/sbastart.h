/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbastart.h
 * Description                  :  sbd-device startup/shutdown
 *
 *
 **********************************************************************/

#ifndef __SBDSTART_H__
#define __SBDSTART_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "typedefs.h"
#include "genbootdef.h"
#endif


/*
 * SB_INITBD: Device-dependent board initialization structure. Used
 * with sping board PM at PM startup time.
 */
typedef struct sb_initbd {

#ifndef BIG_ENDIAN
   UCHAR    *sib_physaddr;    /* Board physical address */
   ULONG    sib_type;         /* Board type code */
   ULONG    sib_featmask;
   ULONG    sib_defmask;
   USHORT   sib_totmem;       /* Board shared memory length */
   USHORT   sib_ioport;       /* Board IO port address */
   USHORT   sib_timslot;      /* Board time slot */
   UCHAR    sib_irqnum;
   UCHAR    sib_irqterm;
   GNB_BOOTCMD	gnbcmd;
   UCHAR    sib_fill[2];
   ULONG    sib_rfu[1];

#else

   UCHAR    *sib_physaddr;    /* Board physical address */
   ULONG    sib_type;         /* Board type code */
   ULONG    sib_featmask;
   ULONG    sib_defmask;
   ULONG    sib_totmem;       /* Board shared memory length */
   ULONG    sib_ioport;       /* Board IO port address */
   ULONG    sib_timslot;      /* Board time slot */
   ULONG    sib_frontend;     /* Board Front end type */
   ULONG    sib_irqnum;
   ULONG    sib_irqterm;
   GNB_BOOTCMD	gnbcmd;
   UCHAR    sib_fill[4];
   ULONG    sib_rfu[1];

#endif /* VME_SPAN */
} SB_INITBD;


#endif

