/* malloc.h - Quipu specific mallocing */

/* 
 * $Header: /xtel/isode/isode/h/quipu/RCS/malloc.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 * 
 * $Log: malloc.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef _QUIPUMALLOC_
#define _QUIPUMALLOC_

extern unsigned mem_heap;
extern unsigned attr_index;

/*
#define MALLOCSTACK
*/

/* HEAP Selection */
/* Two main heaps - general and database.
 * Database is split into main critical part, 
 * and several parts for attribute values.
 */
#define GENERAL_HEAP	mem_heap = 0;	/* general mallocing */
#define DATABASE_HEAP	mem_heap = 1;	/* critical database structures */
#define ATTRIBUTE_HEAP	{if (mem_heap == 1) mem_heap = 2 + attr_index;}
					/* non critical database structures */
#define RESTORE_HEAP	{if (mem_heap >= 2) mem_heap = 1;}

#define SET_HEAP(x)	{if (mem_heap == 1) mem_heap = 2 + set_heap (x);}
#endif
