/*                               -*- Mode: C -*- 
 * buffcall.c --- buffcall management
 * Author          : Francisco J. Ballesteros & Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * Last Modified On: Fri Dec 27 09:48:15 CST 1996
 * RCS Id          : $Id: buffcall.c,v 1.3 1996/01/17 18:57:03 dave Exp $
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
 *    dave@gcom.com
 */

#ident "@(#) LiS bufcall.c 2.5 2/4/03 17:47:06 "


/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/strport.h>
#include <sys/strconfig.h>	/* config definitions */
#include <sys/LiS/share.h>	/* streams shared defs*/
#include <sys/LiS/buffcall.h>	/* interface */
#include <sys/LiS/wait.h>	/* interface */
#include <sys/LiS/stats.h>

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

#define ESB_SIZE	-1	/* special size for bufcall entries when a
				 * bcinfo has this size it's really an entry
				 * for esballoc */


/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

lis_spin_lock_t		 lis_bc_lock ;
volatile bclist_t	 lis_bchash[N_BCHASH] ;	/* the bufcall table */
volatile bcinfo_t       *lis_bcfreel ;	/* the free list */

volatile int		 lis_bc_cnt ;	/* total bufcalls running */
volatile char		 lis_strbcflag;	/* the bufcall functions must be run */
volatile static char	 lis_strbctimer;/* bufcall timer is running */
struct timer_list	 lis_bctl ;	/* timer structure for bufcall */

extern lis_atomic_t	 lis_runq_req_cnt ;
extern lis_spin_lock_t	 lis_qhead_lock ;

void	lis_dobufcall(int) ;

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

/*
 * Timeout function.  Set the global flag that indicates that it is
 * time to do bufcalls and set the queue schedule flag.  When LiS
 * runs the queues it will notice the bufcall flag and do the bufcalls.
 */
static void bc_timeout(unsigned long not_used)
{
    int	 psw;

    (void) not_used ;
    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    lis_strbcflag = 1 ;
    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
    lis_spin_lock_irqsave(&lis_qhead_lock, &psw) ;
    lis_atomic_inc(&lis_runq_req_cnt) ;
    lis_spin_unlock_irqrestore(&lis_qhead_lock, &psw) ;

    lis_setqsched(0);			/* schedule, but don't run right now */

} /* bc_timeout */


/* link a new bcinfo struct, init it w/ given parms and return its address
 */
static int bc_link( int size, void (*function)(long), long arg )
{
    bcinfo_t		*bcinfo;
    bclist_t		*list ;
    int			 psw;
    int			 id ;
    static int		 id_num ;

    /*
     * First try to get a saved-up free structure from the list
     * of free buffcall structures.  Failing that, allocate one.
     * Failing that, return -1.
     */
    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    bcinfo = (bcinfo_t *) lis_bcfreel;
    if (bcinfo != NULL)
	lis_bcfreel = bcinfo->next;

    if ((id = ++id_num) == 0)
	id = ++id_num ;			/* id must be non-zero */

    if (bcinfo == NULL)
	bcinfo = ALLOCF(sizeof(*bcinfo), " buffcall struct") ;

    if (bcinfo == NULL)
    {
	LisUpFailCount(BUFCALLS) ;		/* stats array */
	LisDownCount(BUFCALLS) ;		/* stats array */
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
	return(0) ;				/* table full */
    }

    /*
     * Fill in the structure field values.  Do this before linking
     * the structure into the buffcall waiting list.
     */
    bcinfo->size	= size;
    bcinfo->usr_f	= function;
    bcinfo->usr_arg	= arg;
    bcinfo->id		= id ;
    bcinfo->next	= NULL ;
    bcinfo->state	= BC_STATE_ACTIVE ;

    /*
     * Link the new structure onto the end of the buffcall list.
     * Ensure we are uninterrupted while doing this.
     */
    list = (bclist_t *) &lis_bchash[id & BCHASH_MSK] ;
    bcinfo->prev = list->last ;		/* current to old last element */
    list->last   = bcinfo ;		/* list to new entry */
    if (list->first == NULL)		/* maybe only entry */
	list->first = bcinfo ;
    list->n_elts++ ;			/* count elements in hash list */
    lis_bc_cnt++ ;			/* total bufcalls running */

    if (lis_strbctimer == 0)		/* timer not running */
    {
	lis_strbctimer = 1 ;		/* set flag and start timer */
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
	lis_tmout(&lis_bctl, bc_timeout, 0, BUFCALL_N_TICKS) ;
    }
    else
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;

    return(id);				/* return bufcall id */

}/*bc_link*/

/*  -------------------------------------------------------------------  */

/* unlink a bcinfo struct. 
 *
 * Interrupts must be disabled prior to calling this routine.
 */
static void bc_ulink(bcinfo_t *bcinfo)
{
    bclist_t	*list ;

    if (bcinfo == NULL) return ;		/* no element to delink */

    list = (bclist_t *) &lis_bchash[bcinfo->id & BCHASH_MSK] ;

    if (bcinfo->prev != NULL)
	bcinfo->prev->next = bcinfo->next ;	/* link around */
    
    if (bcinfo->next != NULL)
	bcinfo->next->prev = bcinfo->prev ;	/* link around */
	
    if (list->first == bcinfo)			/* at head of list */
	list->first = bcinfo->next ;		/* NULL if last in list */

    if (list->last == bcinfo)			/* at end of list */
	list->last = bcinfo->prev ;		/* NULL if first in list */

    list->n_elts-- ;
    lis_bc_cnt-- ;				/* total bufcalls running */
    bcinfo->usr_f = NULL;			/* for safety */
    bcinfo->id = 0 ;

    bcinfo->next = (bcinfo_t *) lis_bcfreel ;
    lis_bcfreel = bcinfo ;

}/*bc_ulink*/

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* lis_bufcall - schedule recovery from alloc failure
 *
 */
int 
lis_bufcall(unsigned size, int priority, void (*function)(long), long arg)
{
    LisUpCount(BUFCALLS) ;			/* stats array */
    (void)priority;
    if (!function)
    {
	LisUpFailCount(BUFCALLS) ;		/* stats array */
	LisDownCount(BUFCALLS) ;		/* stats array */
	return(0);
    }

    return(bc_link(size,function,arg));

}/*lis_bufcall*/


/*  -------------------------------------------------------------------  */
/* esbbcall - like bufcall, but for lis_esballoc. The function
 *	will be called when there is a `good chance' that the
 *	lis_esballoc will succeed.
 */
int 
lis_esbbcall(int priority, void (*function)(long), long arg)
{
    LisUpCount(BUFCALLS) ;			/* stats array */
    (void)priority;
    if (!function)
    {
	LisUpFailCount(BUFCALLS) ;		/* stats array */
	LisDownCount(BUFCALLS) ;		/* stats array */
	return(0);
    }

    return(bc_link(ESB_SIZE,function,arg));

}/*lis_esbbcall*/

/*  -------------------------------------------------------------------  */
/* unbufcall - cancels a bufcall/esbbcall
 *
 */
void
lis_unbufcall(int bcid)
{
    bcinfo_t	*bcinfo ;
    bclist_t	*list ;
    int		 psw;

    /*
     * Unfortunately, the table has to be searched with interrupts
     * disabled to prevent the list from moving while we are
     * searching it.  We have to keep interrupts off until we have
     * removed the entry from the list in case of a second call on
     * unbufcall for the same element.
     *
     * The good news is that we have "hashed" the table fairly
     * broadly so we really cut down on the iteration count here.
     */
    list = (bclist_t *) &lis_bchash[bcid & BCHASH_MSK] ;
    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;

    for (bcinfo = list->first; bcinfo != NULL; bcinfo = bcinfo->next)
    {
	if (bcinfo->id == bcid)
	    break ;
    }

    if (bcinfo == NULL)
    {
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
	return ;		/* not in list */
    }

    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;

    /*
     * If another thread is servicing the list it may be calling this
     * function just now.  Wait for it to complete.
     */
    while (bcinfo->state == BC_STATE_CALLOUT) ;

    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    bcinfo->state = BC_STATE_UNUSED ;
    bc_ulink(bcinfo);				/* remove from list */
    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;

    LisDownCount(BUFCALLS) ;			/* stats array */

}/*lis_unbufcall*/

/*  -------------------------------------------------------------------  */
/* process a bufcall: remove it from the list call the event handler free up
 * space associated with used item else put event on back of list 
 *
 * Leave the timer running flag set until we process all the bufcalls.
 * Any new ones will get noticed at the end of the routine and we will
 * restart the timer if need be.
 */
void
lis_dobufcall(int cpu_id)
{
    int			 cnt;
    int			 i ;
    int			 psw;
    bclist_t		*list ;
    struct bcinfo	*bc;
    void	       (*fcn)(long);
    long		 arg ;

    (void) cpu_id ;
    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    if (!lis_strbcflag)
    {						/* possibly re-entered */
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
	return ;
    }

    lis_strbcflag = 0 ;				/* no more calls till timer */
    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;

    for (i = 0; lis_bc_cnt != 0 && i < N_BCHASH; i++)
    {
	list = (bclist_t *) &lis_bchash[i] ;

	/*
	 * Drive this loop with the count of the number of elements,
	 * not a linked-list walk.  The list may change as we process
	 * it.  New entries go on the end.  We remove entries from
	 * the front.  In that way we will process the older entries.
	 */
	for (cnt = list->n_elts; cnt > 0; cnt--)
	{
	    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
	    bc = list->first ;
	    if (bc == NULL)		/* should not be the case */
	    {
		lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
		continue ;		/* but skip anyway */
	    }

	    fcn = bc->usr_f ;
	    arg = bc->usr_arg ;
	    bc->state = BC_STATE_CALLOUT ;
	    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;

	    if (fcn != NULL)		/* cheap insurance */
		(*fcn)(arg) ;		/* call user function */

	    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
	    bc->state = BC_STATE_UNUSED ;
	    bc_ulink(bc);		/* remove from list */
	    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
	    LisDownCount(BUFCALLS) ;	/* stats array */
	}
    }

    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    lis_strbctimer = 0 ;		/* timer not running */
    if (lis_bc_cnt != 0 && lis_strbctimer == 0)
    {					/* more bufcalls to do */
	lis_strbctimer = 1 ;
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
	lis_tmout(&lis_bctl, bc_timeout, 0, BUFCALL_N_TICKS) ;
    }
    else
	lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
}

/*  -------------------------------------------------------------------  */
/* init the bufcall table
 */
void
lis_init_bufcall(void)
{
    int			 i ;
    int			 psw;
    bclist_t		*list ;

    /*
     * With interrupts off so the list can't be disturbed, delink all
     * elements from the bufcall lists.
     */
    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    for (i = 0; i < N_BCHASH; i++)
    {
	list = (bclist_t *) &lis_bchash[i] ;
	while (list->first != NULL)
	{
	    list->first->state = BC_STATE_UNUSED ;
	    bc_ulink(list->first);		/* remove from list */
	    LisDownCount(BUFCALLS) ;		/* stats array */
	}

	list->n_elts = 0 ;
    }

    lis_bc_cnt = 0 ;				/* total bufcalls running */

    lis_strbcflag = 0 ;
    lis_strbctimer = 0 ;
    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
}

/*  -------------------------------------------------------------------  */
/* Deallocate any memory allocations
 */
void lis_terminate_bufcall(void)
{
    int		psw ;

    lis_init_bufcall() ;

    lis_spin_lock_irqsave(&lis_bc_lock, &psw) ;
    while (lis_bcfreel != NULL)
    {
	bcinfo_t *bc = (bcinfo_t *) lis_bcfreel;

	lis_bcfreel = bc->next;
	FREE(bc);
    }
    lis_spin_unlock_irqrestore(&lis_bc_lock, &psw) ;
}

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
