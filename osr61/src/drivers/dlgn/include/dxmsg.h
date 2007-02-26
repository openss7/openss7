/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: dxmsg.h
 * DESCRIPTION: Contains SRAM PM message definitions
 *
 **********************************************************************/

#ifndef __DXMSG_H__
#define __DXMSG_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gnmsg.h"
#endif


/*
 * Message idents used in msg_ident
 */
#define SR_STOP                (MC_BASEMSGNUM | 0x0001)
#define SR_DEVSEND             (MC_BASEMSGNUM | 0x0002)
#define SR_GETDEVINFO          (MC_BASEMSGNUM | 0x0003)
#define SR_CURDEVINFO          SR_GETDEVINFO
#define SR_GETLSTAT            (MC_BASEMSGNUM | 0x0004)
#define SR_CURLSTAT            SR_GETLSTAT
#define SR_OPENCNT             (MC_BASEMSGNUM | 0x0005)
#define SR_OPENCNTCMPLT        SR_OPENCNT
#define SR_BDRESET             (MC_BASEMSGNUM | 0x0006)
#define SR_BDRESETCMPLT        SR_BDRESET
#define SR_SETSTOP             (MC_BASEMSGNUM | 0x0007)
#define SR_DTIPARMINFO         (MC_BASEMSGNUM | 0x0008)
#define SR_DTIPARMINFOCMPLT    SR_DTIPARMINFO
#define SR_STOPSYNC            (MC_BASEMSGNUM | 0x000C)


#ifdef _ISDN
#define SR_SETINTFCID          (MC_BASEMSGNUM | 0x0009)
#define SR_SETINTFCIDCMPLT     SR_SETINTFCID
#define SR_GETINTFCID          (MC_BASEMSGNUM | 0x000A)
#define SR_GETINTFCIDCMPLT     SR_GETINTFCID
#define SR_SETNFASBD           (MC_BASEMSGNUM | 0x000B)
#define SR_SETNFASBDCMPLT      SR_SETNFASBD
#define SR_RDSRAM	       (MC_BASEMSGNUM | 0x000D)
#define SR_RDSRAMCMPLT         SR_RDSRAM
#endif /* _ISDN */


#ifndef DCB
#define DCB 1
#endif

#ifdef DCB
#define SR_GETATIBITS		(MC_BASEMSGNUM | 0x000C)
#define SR_GETATIBITSCMPLT	SR_GETATIBITS
#endif

#endif

