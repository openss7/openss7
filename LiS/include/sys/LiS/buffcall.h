/*****************************************************************************

 @(#) $Id: $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: $ by $Author: $

 *****************************************************************************/

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
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 */

#ifndef _BUFFCALL_H
#define _BUFFCALL_H 1

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

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
    void	 _RP (*usr_f)(long);/* user callback */
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
#if __LIS_INTERNAL__
extern volatile bclist_t	lis_bchash[N_BCHASH] ;	/* the bufcall table */
extern volatile bcinfo_t	*lis_bcfreel ;		/* the free list */
#endif

#if __LIS_INTERNAL__
extern volatile char lis_strbcflag;	/* the bufcall functions must be run */
#endif

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
lis_bufcall(unsigned size, int priority, void _RP (*function)(long), long arg)_RP;
#endif				/* __KERNEL__ */

/* esbbcall - like bufcall, but for lis_esballoc. The function
 *	will be called when there is a `good chance' that the
 *	lis_esballoc will succeed.
 */
#ifdef __KERNEL__
extern int lis_esbbcall(int priority, void _RP (*function)(long), long arg)_RP;
#endif				/* __KERNEL__ */

/* unbufcall - cancels a bufcall/esbbcall
 */
#ifdef __KERNEL__
extern void lis_unbufcall(int bcid)_RP;
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

#ifdef __KERNEL__

#if __LIS_INTERNAL__
extern void lis_init_bufcall(void);
extern void lis_terminate_bufcall(void);

extern void lis_dobufcall(int) ;
#endif

#endif /* __KERNEL__ */


/*  -------------------------------------------------------------------  */
#endif /*!_BUFFCALL_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
