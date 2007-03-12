/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdtypes.h
 * Description                  : driver type definitions
 *
 *
 **********************************************************************/

#ifndef _MDTYPES_
#define _MDTYPES_

typedef int   merc_int_t;
typedef int   *pmerc_int_t;
typedef char  merc_char_t;
typedef char  *pmerc_char_t;
typedef void  merc_void_t;
typedef void  *pmerc_void_t;
typedef long  merc_long_t;
typedef long  *pmerc_long_t;
typedef short merc_short_t;
typedef short *pmerc_short_t;

typedef unsigned int   merc_uint_t;
typedef unsigned int   *pmerc_uint_t;
typedef unsigned char  merc_uchar_t;
typedef unsigned char  *pmerc_uchar_t;
typedef unsigned long  merc_ulong_t;
typedef unsigned long  *pmerc_ulong_t;
typedef unsigned short merc_ushort_t;
typedef unsigned short *pmerc_ushort_t;

/* Left old definitions for backward compatibility */
typedef unsigned long	ULONG;
typedef unsigned long	*PULONG;
typedef unsigned short  USHORT;
typedef unsigned short  *PUSHORT;
typedef unsigned char	UCHAR;
typedef unsigned char	*PUCHAR;
typedef char		CHAR;
typedef char		*PCHAR;
typedef short		SHORT;
typedef short		*PSHORT;
typedef long		LONG;
typedef long		*PLONG;
//typedef unsigned int	UInt32;
typedef unsigned int	UINT32;
typedef unsigned int	*PUINT32;
typedef int		INT32;
typedef int		*PINT32;
typedef void		*PVOID;

typedef unsigned long	MD_STATUS;
// Duplicate for Library
typedef USHORT		MD_HANDLE;

#ifndef _8_BIT_INSTANCE
typedef UINT32          MBD_HANDLE;
#endif

typedef USHORT		MSD_HANDLE;

typedef ULONG		KAW_HANDLE;

/* Duplicate for Library */
#ifdef LFS
typedef timo_fcn_t *PMD_FUNCTION;
typedef timo_fcn_t *PMSD_FUNCTION;
#else
typedef ULONG		(*PMD_FUNCTION)();
typedef ULONG		(*PMSD_FUNCTION)();
#endif

typedef struct msgb 	STRM_MSG;
typedef struct msgb 	*PSTRM_MSG;

#define	REGISTER	register

#define MD_PRIVATE		

#endif
