/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : qhosttypes.h
 * Description                  : host type definitions
 *
 *
 **********************************************************************/

#ifndef __QHOSTTYPES_H__
#define __QHOSTTYPES_H__

//////////////////////////////////////////////////////////////
#include "qoscommon.h"
#ifdef QCPU
#undef QCPU		// redefining below
#endif
#define QLITTLEENDIAN
#ifndef QBIGENDIAN
#define QCPU QHOST_LITTLE_ENDIAN
#else
#define QCPU QHOST_BIG_ENDIAN
#endif
/////////////////////////////////////////////////////////////

#include "qtypes.h"

/* 16BIT Support */
#ifdef BITS8
  #define _8_BIT_INSTANCE
  #define QINST_WIDTH 8
#else
   #define QINST_WIDTH 16
#endif

/* Library types */
#ifdef _8_BIT_INSTANCE
typedef UInt16  QBindHandle;
#else
typedef UInt32  QBindHandle;
#endif

typedef int     QHandle;



/* Standard Mercury Function Returns */
typedef enum {QSTATUS_DONE, QSTATUS_PENDING, QSTATUS_ERROR, QSTATUS_NOERROR} 
	QStatus;


/*
 *  Operating system and CPU specific information - This drives 
 *  information in qcpu.h
 */

#ifdef __sparc   /* Sun sparc machine */

   /* Smallest accessable data type */
   #define QCHAR_BIT    8
   #define QCHAR_BYTE   1        /* Number of bytes per character(word) */


#ifdef SPARCPCI
   #define PCI
#endif /* UltraSparc PCI */

   /* Bus type of machine */
   #define VME

   /* Type of file handle for operating system */
   typedef int OSHandle;

   #define MERCD_SOFTINT_TYPE DDI_SOFTINT_LOW
#endif /* Sun sparc */

#if defined(__i386) || defined(i386)

   /* Smallest accessable data type */
   #define QCHAR_BIT    8
   #define QCHAR_BYTE   1        /* Number of bytes per character(word) */

   //#define MERCD_DATATYPE_INTEL /* Intel Data Type */ // defined in  Makefile

   /* Bus type of machine */
   #define PCI

   /* Type of file handle for operating system */
   typedef UInt32 OSHandle;

#ifdef sun
   #define MERCD_SOFTINT_TYPE DDI_SOFTINT_HIGH
#endif 
   #endif /* Solaris Intel */

/* AFTERMEETING */
/* Complex Mercury Types */
#ifdef AAAAAA
typedef  struct {
   Uint24 buf[3];
} QStreamRef;
#endif
/* AFTERMEETING */

#endif 

