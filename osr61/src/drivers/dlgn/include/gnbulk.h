/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gnbulk.h
 * Description                  : generic bulk data definitions
 *
 *
 **********************************************************************/

#ifndef __GNBULK_H__
#define __GNBULK_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * GN_BULKPARM - contains bulk data transfer parameters
 */
typedef struct gn_bulkparm {
   unsigned short bp_sndlo;
   unsigned short bp_sndhi;
   unsigned short bp_rcvlo;
   unsigned short bp_rcvhi;
   unsigned short bp_sndmax;
   unsigned short bp_rcvmax;
   unsigned short bp_2pmsize;
   unsigned short bp_pad0;
   unsigned short bp_lowater;
   unsigned short bp_hiwater;
   unsigned short bp_rfu[6];
} GN_BULKPARM;


#endif


