/*                               -*- Mode: C -*- 
 * buffcall.h --- buffcall management 
 * Author          : Francisco J. Ballesteros & Graham Wheeler
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: buffcall.h,v 1.4 1996/01/27 00:40:24 dave Exp $
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros,
 *                        Denis Froschauer
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 */



#ifndef _BUFFCALL_H
#define _BUFFCALL_H 1

#ident "@(#) LiS buffcall.h 2.3 2/4/03 17:47:36 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* config definitions */
#endif
#ifndef _SHARE_H
#include <sys/LiS/share.h>	/* shared defs */
#endif
/*  -------------------------------------------------------------------  */
/*				    Types                                */

#ifdef __KERNEL__
/* buffcall list 
 */
/* entry */
typedef struct bcinfo
{
    struct bcinfo *next;	/* next node  */
    struct bcinfo *prev;	/* prev node  */
    unsigned	   size;	/* size for this node */
    int		   id ;		/* identifier for user */
    volatile int   state ;	/* state of entry */
    void	 (*usr_f)(long);/* user callback */
    long	   usr_arg;	/* arg for user callback */
} bcinfo_t;

#define BC_STATE_UNUSED		0	/* unused entry */
#define BC_STATE_ACTIVE		1	/* active entry */
#define	BC_STATE_CALLOUT	2	/* calling user function */

/* list head */
typedef struct bclist
{
    struct bcinfo *first;
    struct bcinfo *last;
    int		   n_elts ;
} bclist_t ;

#endif /* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */
#ifdef __KERNEL__

/*
 * A power of 2 hash will do since the bufcall ids are allocated as
 * sequential numbers.
 */
#define	N_BCHASH	128		/* power of 2 */
#define	BCHASH_MSK	(N_BCHASH - 1)
extern volatile bclist_t	lis_bchash[N_BCHASH] ;	/* the bufcall table */
extern volatile bcinfo_t	*lis_bcfreel ;		/* the free list */

extern volatile char lis_strbcflag;	/* the bufcall functions must be run */

/*
 * Timer ticks -- frequency to run bufcall timer.
 */
#define	BUFCALL_MS_TICKS	100	/* gives 100ms per timeout */
#define BUFCALL_N_TICKS ((SECS_TO(1)*BUFCALL_MS_TICKS/1000) ? \
			(SECS_TO(1)*BUFCALL_MS_TICKS/1000) : 1)


#endif /* __KERNEL__ */
/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* lis_bufcall - schedule recovery from alloc failure
 */
#ifdef __KERNEL__
extern int 
lis_bufcall(unsigned size, int priority, void (*function)(long), long arg);
#endif				/* __KERNEL__ */

/* esbbcall - like bufcall, but for lis_esballoc. The function
 *	will be called when there is a `good chance' that the
 *	lis_esballoc will succeed.
 */
#ifdef __KERNEL__
extern int lis_esbbcall(int priority, void (*function)(long), long arg);
#endif				/* __KERNEL__ */

/* unbufcall - cancels a bufcall/esbbcall
 */
#ifdef __KERNEL__
extern void lis_unbufcall(int bcid);
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

#ifdef __KERNEL__

extern void lis_init_bufcall(void);
extern void lis_terminate_bufcall(void);

extern void lis_dobufcall(int) ;

#endif /* __KERNEL__ */


/*  -------------------------------------------------------------------  */
#endif /*!_BUFFCALL_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
