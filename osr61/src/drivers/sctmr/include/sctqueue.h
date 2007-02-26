/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : queue.h
 * Description                  : queue handling
 *
 *
 **********************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

/*
 * DEFINES
 */
#define	TIMER_MAXNUM	400 /* 300 *//* Maximum number of timeout at any time */


/*
 * Structures
 */
typedef struct timer_info	TIMER_INFO;

struct timer_info {
   queue_t	*ti_qptr;	/* Q pointer */
   mblk_t	*ti_msgp;	/* Message block */
   TIMER_INFO	*next;		/* Next in link */
   TIMER_INFO	*prev;		/* Prev in link */
   long		ti_timeout;	/* Timeout in ticks */
   long		ti_timeleft;	/* Time left in ticks */
   long		ti_ddd;		/* ddd on which to generate an event */
   long		ti_id;		/* User Specified Identifier */
};


/*
 * Module Global Variables
 */
extern long		timer_cnt;

extern TIMER_INFO	*quse;
extern TIMER_INFO	*qfree;


/*
 * Function Prototypes
 */
#ifdef LFS
extern int		st_InitQueue(long);
extern int		st_Add2UsedQ(queue_t *, mblk_t *, long, long, long);
extern int		st_Add2FreeQ(TIMER_INFO *);
extern TIMER_INFO *	st_FindEntry(queue_t *, long, long);
#else
extern int		st_InitQueue();
extern int		st_Add2UsedQ();
extern int		st_Add2FreeQ();
extern TIMER_INFO *	st_FindEntry();
#endif

#endif
