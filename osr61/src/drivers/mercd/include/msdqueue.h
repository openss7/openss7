/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdqueue.h
 * Description                  : queue data structure definitions
 *
 *
 **********************************************************************/

#ifndef _MSDQUEUE_
#define _MSDQUEUE_

// Generic MSD Message struct used by the MD queue functions
// It maps to any message block that has a Next pointer
// as the first field.

typedef struct _MSD_GEN_MSG {
	struct _MSD_GEN_MSG  *Next;
} MSD_GEN_MSG, *PMSD_GEN_MSG;

typedef struct _MSD_GEN_TWIN_LINK_MSG {
	struct _MSD_GEN_TWIN_LINK_MSG  *Next;
	struct _MSD_GEN_TWIN_LINK_MSG  *SecondNext;
} MSD_GEN_TWIN_LINK_MSG, *PMSD_GEN_TWIN_LINK_MSG;


// MSD_QUEUE definition
// Description: queue control struct
#if defined LiS || defined LIS || defined LFS
typedef struct _MSD_QUEUE {
	pmerc_void_t	QueueHead;	/* pointer to head of queue*/
	pmerc_void_t	QueueTail;	/* pointer to head of queue*/
	merc_uint_t	HighWater;	/* high water mark*/
	merc_uint_t	LowWater;	/* low water mark*/
} MSD_QUEUE, *PMSD_QUEUE;
#else
#define WAITQUEUE_T wait_queue_head_t
#define ATOMIC_T atomic_t
#define TRUE 1
#define FALSE 0

typedef struct _MSD_QUEUE {
	pmerc_void_t       QueueHead;      /* pointer to head of queue*/
	pmerc_void_t       QueueTail;      /* pointer to head of queue*/
	merc_uint_t        HighWater;      /* high water mark*/
	merc_uint_t        LowWater;       /* low water mark*/
	mercd_mutex_T      readMutex;
	mercd_mutex_T      writeMutex;
	WAITQUEUE_T        readers;
	merc_uint_t        numberOfBlocks;
	ATOMIC_T           msgCapacity;
	ATOMIC_T           wrapAround;
} MSD_QUEUE, *PMSD_QUEUE;

typedef MSD_QUEUE      queue_t;
typedef MSD_QUEUE      *pmercd_strm_queue_sT;
#endif

#define MSD_QUEUE_EMPTY(Q)	((Q)->QueueHead==NULL)

#endif // _MSDQUEUE_
