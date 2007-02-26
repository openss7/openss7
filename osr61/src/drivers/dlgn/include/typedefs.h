/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: typedefs.h
 * DESCRIPTION: Contains basic type definitions
 *        DATE: 10/06/92
 *          BY: EFN
 *
 **********************************************************************/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Type definitions for integral data
 */
typedef char                     CHAR;
typedef unsigned char            UCHAR;

typedef short int                SHORT;
typedef unsigned short int       USHORT;

typedef long int                 LONG;
typedef unsigned long int        ULONG;

typedef long int                 INT;
typedef unsigned long int        UINT;


/*
 * Type definitions for storage class specifiers
 */
#ifdef BIG_ENDIAN
#  ifdef PRIVATE
#    undef PRIVATE
#  endif
#  define PRIVATE
#else
#  ifndef PRIVATE
#  define PRIVATE
#  endif
#endif

#define GLOBAL


/*
 * Defines for standard buffer sizes
 */
#define BUFSZ_8            (8)
#define BUFSZ_16           (16)
#define BUFSZ_32           (32)
#define BUFSZ_64           (64)
#define BUFSZ_128          (128)
#define BUFSZ_256          (256)
#define BUFSZ_512          (512)
#define BUFSZ_1K           (1024)
#define BUFSZ_2K           (2048)
#define BUFSZ_4K           (4096)
#define BUFSZ_8K           (8192)
#define BUFSZ_16K          (16384)
#define BUFSZ_32K          (32768)
#define BUFSZ_64K          (65536)


#endif

