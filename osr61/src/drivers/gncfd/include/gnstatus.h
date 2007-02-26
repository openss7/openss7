/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gnstatus.h
 * Description                  : generic driver status
 *
 *
 **********************************************************************/

#ifndef __GNSTATUS_H__
#define __GNSTATUS_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndefs.h"
#endif


/*
 * Generic status flags
 */
#define GN_STARTED               0x01     /* bit set=active; clr=inactive */


/*
 * Generic status structures
 *
 *   The status structure fields marked 'y' contain valid information
 *   under the following conditions. The GN_BDSTATUS structure is not
 *   present under conditions given by a '-'.
 *
 *                DRVR   DRVR    PM     PM     BD     BD
 *     FIELD      STOP   STRT   STOP   STRT   STOP   STRT
 *   gds_numpms     y      y      y      y      y      y
 *   gds_numbds     0      y      y      y      y      y
 *   gds_status     y      y      y      y      y      y
 *
 *   gps_pmname     y      y      y      y      y      y
 *   gps_numbds            y      y      y      y      y
 *   gps_numlds            y      y      y      y      y
 *   gps_bdsz       y      y      y      y      y      y
 *   gps_ldsz       y      y      y      y      y      y
 *   gps_status     y      y      y      y      y      y
 *
 *   gbs_bdname     -      -      -      y      y      y
 *   gbs_nlogdevs   -      -      -      y      y      y
 *   gbs_irq        -      -      -      y      y      y
 *   gbs_status     -      -      -      y      y      y
 *
 */


/*
 * Generic Driver Status structure
 */
typedef struct gn_drvstatus {
   unsigned long gds_numpms;              /* # PM's in the system */
   unsigned long gds_numbds;              /* # boards in the system */
   unsigned long gds_status;              /* Status of Generic Driver */
   unsigned long gds_rfu;
} GN_DRVSTATUS;


/*
 * Protocol Module status structure
 */
typedef struct gn_pmstatus {
   unsigned char gps_pmname[PM_NAMELEN];  /* ASCIIZ PM name string */
   unsigned long gps_numbds;              /* # boards on this PM */
   unsigned long gps_numlds;              /* # logdevs in this PM */
   unsigned long gps_bdsz;                /* Dev-dep board struct size */
   unsigned long gps_ldsz;                /* Dev-dep logdev struct size */
   unsigned long gps_status;              /* Status of this PM */
   unsigned long gps_rfu;
} GN_PMSTATUS;


/*
 * Board status structure
 */
typedef struct gn_bdstatus {
   unsigned char gbs_bdname[GB_NAMELEN];  /* ASCIIZ board name string */
   unsigned long gbs_nlogdevs;            /* # logdevs on this board */
   unsigned long gbs_irq;                 /* IRQ level for this board */
   unsigned long gbs_status;              /* Status of this board */
   unsigned long gbs_rfu;
} GN_BDSTATUS;


#endif

