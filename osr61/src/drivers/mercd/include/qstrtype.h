/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
#ifndef __QSTREAMTYPES_H__
#define __QSTREAMTYPES_H__
/******************************************************************************
*  
*
*     THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Intel Corporation.
*     The copyright notice above does not evidence any actual or
*     intended publication of such source code.
******************************************************************************/

/******************************************************************************
*       FILE: qstrmapi.h
*     Author: kishb
*
*    PURPOSE:
*
* HISTORY
* Date            Who   Description
* 11/95           BK    Changed definition of QBufInfo so its format is easier
*                       to read by all type of processors.
* 08/95           BK    Added CStream APIs
* 07/95           BK    Made changes to support QStreamRef
* 01/95           BK    Initial Version
*
* Notes:
******************************************************************************/

/***************************************************************************/
/***************************** IMPORTANT NOTE ******************************/
/***************************************************************************/
/*                                                                         */
/* This file belongs to the kernel group and must not be modified by any   */
/* other group.                                                            */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
/********************************* NOTE ************************************/
/***************************************************************************/
/***************************************************************************/
/*                                                                         */
/* This file is part of the system release. Please try not to add kernel   */
/* specific stuff to this file. However, if you must, it should be         */
/* included in if (QCPU < QHOST) section.                                  */
/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define QMAX_BLOCKS_STREAM       5

typedef struct {
   Uint24   id;
   Uint24   mode;
   void *   gHandle;
   Uint8    head;
   Uint8    tail;
} QStreamHandleEntry;

typedef QStreamHandleEntry *QStreamHandle;


typedef struct {
   QStreamHandle  bHandle;
   Uint8          offsetIdx;
   void           *pUsrHead;
} QBufRefEntry;

typedef QBufRefEntry *QBufRef;


/*
 * The following structure is used in the bstream layer to process stream
 * reference to handle conversions (must be 11 mmdl bytes).
 */   
typedef struct {
   Uint24         openMode;      /* Mode stream was originally opened in */
   QStreamHandle  bHandle;
   Uint16         streamId;      /* */
   Uint16         pad;           /* */
} QStreamReference;

typedef QStreamReference   QStreamRef;

#ifdef __cplusplus
}
#endif

#endif /* __QSTREAMTYPES_H__ */
