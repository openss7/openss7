/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/******************************************************************************
*  
*
*     THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Intel Corporation.
*     The copyright notice above does not evidence any actual or
*     intended publication of such source code.
******************************************************************************/

/****************************************************************************
 * FILE:         qtypes.h
 * AUTHOR:       puppalag
 * ORG:          Intel Corporation
 * ORIG-DATE:    5-Aug-95 at 17:48:31
 * LAST-MOD:     27-Feb-96 at 08:16:42 by Chris Chuba
 * PURPOSE:
 *
 * HISTORY:
 * Date           Who   Description
 * 07-Aug-95      GP    Initial Creation.
 ***************************************************************************/

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
#ifndef __QTYPES_H__
#define __QTYPES_H__

#include <qoscommon.h>

#ifndef QCPU
#error QCPU is not defined
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
/*
 *  Note:  Is it really necessary to exclude non VxWorks and Winos kernels
 *  from this Char typedef?
 */
#if defined QOS_VXWORKS || defined QOS_WINOS
#if ((QOS_VXWORKS || QOS_WINOS) || (QCPU >= QHOST))
typedef char   Char;
#endif
#endif

typedef short  QD_UINT16;
typedef long   QD_UINT32;

#if (QCPU < QHOST)
#ifdef __cplusplus
typedef bool boolean;
#else
typedef enum {false, true} boolean;
#endif /*__cplusplus*/
#endif /* (QCPU < QHOST) */

typedef  unsigned char        UInt8;
typedef  unsigned char        Uint8;
typedef  char                 Int8;

typedef  unsigned short int   UInt16;
typedef  unsigned short int   Uint16;
typedef  short int            Int16;

typedef  unsigned int         UInt24;
typedef  unsigned int         Uint24;
typedef  int                  Int24;


#if 0 && (QCPU == QM56301)
typedef  unsigned long        UInt32;
typedef  unsigned long        Uint32;
typedef  long                 Int32;
#else
typedef  unsigned int         UInt32;
typedef  unsigned int         Uint32;
typedef  int                  Int32;
#endif  /* (QCPU == QM56301) */

#include "qstrtype.h"

typedef unsigned int          QError;
typedef unsigned int          QMemRef;
typedef unsigned int          QParm;

/* prototype for task entrypoints created by qTaskCreate2 */
#ifdef LFS
typedef  void  (*QFuncPtr)(void);
#else
typedef  void  (*QFuncPtr)();
#endif

/* prototype for task entrypoints created by qTaskCreate2 */
typedef void (*QFuncPtr2)(void *pArgs);
    

#ifdef __cplusplus
}
#endif

#endif
