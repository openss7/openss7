/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dvbm.h
 * Description                  : Bulk Data Module header
 *
 *
 **********************************************************************/

#ifndef __DVBM_H__
#define __DVBM_H__

#ifdef __STANDALONE__            /* List of required header files */
#include <sys/types.h>
#include <sys/stream.h>
#include "gnbulk.h"
#endif

#include "d40low.h"		 /* for DL_PRSTATUS */
/* following should be in d40low.h */
#ifndef DL_PRSTATUS
#define DL_PRSTATUS   0x1D
#endif

#ifdef VME_SPAN
/* Default BDM parameters */
#define DEF_BDM_SENDLOW          0x4000
#define DEF_BDM_SENDHIGH         0x8000
#define DEF_BDM_RECEIVELOW       0x2000
#define DEF_BDM_RECEIVEHIGH      0x4000
#define DEF_BDM_SENDTHRESHOLD    0x8000
#define DEF_BDM_RCVTHRESHOLD     0x8000
#define DEF_BDM2PM_XFERSIZE      0x4000
#else
/* Default BDM parameters */
#define DEF_BDM_SENDLOW          0x2000
#define DEF_BDM_SENDHIGH         0x3000
#define DEF_BDM_RECEIVELOW       0x0800
#define DEF_BDM_RECEIVEHIGH      0x1000
#define DEF_BDM_SENDTHRESHOLD    0x2000
#define DEF_BDM_RCVTHRESHOLD     0x2000
#define DEF_BDM2PM_XFERSIZE      0x0800
#endif /* VME_SPAN */


/* BDM states */
#define SEND_IDLE                0x0001
#define SENDING                  0x0002
#define SEND_LAST                0x0004
#define RECEIVE_IDLE             0x0008
#define RECEIVING                0x0010
#define RECEIVE_LAST             0x0020
#define BDM_INITED               0x0040
#define BDM_BUFMT                0x0080
#define BDM_WAIT_MOREDATA        0x0100
#define BDM_PRSTATUS		 0x0200

typedef struct bdm_info {
   queue_t        *bi_qptr;
   mblk_t         *bi_wq;
   mblk_t         *bi_rq;
   int            bi_wqcnt;
   int            bi_rqcnt;
   unsigned long  bi_state;
   GN_BULKPARM    bi_bp;
} BDM_INFO;


#endif

