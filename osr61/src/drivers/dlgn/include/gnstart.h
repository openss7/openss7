/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: gnstart.h
 * DESCRIPTION: Contains generic structures and definitions for Generic
 *              Driver, PM, and board startup/shutdown.
 *        DATE: 10/06/92
 *          BY: EFN
 *
 **********************************************************************/

#ifndef __GNSTART_H__
#define __GNSTART_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndefs.h"
#endif


/*
 * GN_PMCNT: PM count structure
 */
typedef struct gn_pmcnt {
   unsigned int gc_pmcnt;
   unsigned int gc_rfu[3];
} GN_PMCNT;


/*
 * GN_PMDEVCNT: Board/logdev count structure
 */
typedef struct gn_pmdevcnt {
   unsigned char  gd_pmname[PM_NAMELEN]; /* Contains ASCIIZ PM name string */
   unsigned int  gd_npmbds;             /* # board devices on this PM */
   unsigned int  gd_npmlds;             /* # logdev devices on this PM */
   unsigned short gd_nprotos;            /* # protocols available on this PM */
   unsigned short gd_pad0;
   unsigned int  gd_rfu[3];
} GN_PMDEVCNT;


/*
 * GN_INITBD: Generic board initialization structure.
 */
typedef struct gn_initbd {
   unsigned char  gib_name[GB_NAMELEN];  /* ASCIIZ board name string */
   unsigned char  gib_sub[GB_SUBLEN];    /* ASCIIZ subdevice name string */
   unsigned short gib_nlogdevs;          /* No. of logdevs on the board */
   unsigned short gib_irq;               /* Boards IRQ level; use 0 if none */
   unsigned int  gib_rfu[2];
} GN_INITBD;


/*
 * GN_STARTPM: Protocol Module startup/shutdown information structure.
 */
typedef struct gn_startpm {
   unsigned char gp_pmname[PM_NAMELEN];   /* Contains ASCIIZ PM name string */
   unsigned int gp_numbds;               /* # of BOARD devices on this PM */
   unsigned int gp_numlds;               /* # of LOGDEV devices on this PM */
   unsigned int gp_ngsz;                 /* Size of dev-dep area */
   unsigned int gp_rfu[3];
} GN_STARTPM;


/*
 * GN_STARTBD: Board startup/shutdown data structure
 */
typedef struct gn_startbd {
   unsigned char gb_bdname[GB_NAMELEN];   /* ASCIIZ board name string */
   unsigned int gb_rfu[4];
} GN_STARTBD;


#endif

