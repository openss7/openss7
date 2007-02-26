/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

#ifndef __GENTYPEDEFS_H__
#define __GENTYPEDEFS_H__

/*#if (defined(__TYPEDEFS_H__) || defined(_RESOURCE_)) */
#if (defined(__TYPEDEFS_H__) )
#else
/*
 * Type definitions for integral data
 */
typedef char			CHAR;		/* 8 Bit Signed */
typedef unsigned char		UCHAR;		/* 8 Bit Unsigned */

typedef short int		SHORT;		/* 16 Bit Signed */
typedef unsigned short int	USHORT;		/* 16 Bit Unsigned */

typedef int			INT; 		/* Machine Dependent Signed */
typedef unsigned int		UINT; 		/* Machine Dependent Unsigned */

typedef long    		LONG;		/* 32 Bit Signed */
typedef unsigned long    	ULONG;		/* 32 Bit Unsigned */
#endif /*  __TYPEDEFS_H__ || _RESOURCE_ */

typedef double			DOUBLE;		/* 32 Bit Floating Point */

#if(!defined(VOID) || !defined(LINUX))
typedef void VOID;
#endif
/*
 * Type definitions for storage class specifiers
 */
#if defined(sun) || defined(LINUX)
#undef PRIVATE
#endif

#define PRIVATE			static 
#define	GLOBAL

#endif

