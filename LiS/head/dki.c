/************************************************************************
*                      SVR4 Driver-Kernel Interface                     *
*************************************************************************
*									*
* These routines implement an SVR4 compatible DKI.			*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/
/*
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
 */

#ident "@(#) LiS dki.c 2.7 3/27/01 19:53:11 "

#include <sys/stream.h>
#include <sys/osif.h>

/************************************************************************
*                       SVR4 Compatible timeout                         *
*************************************************************************
*									*
* This implementation of SVR4 compatible timeout functions uses the	*
* Linux timeout mechanism as the internals.				*
*									*
* The technique is to allocate memory that contains a Linux timer	*
* structure and an SVR4-like handle.  We build up a list of these	*
* structures in a linked-list headed by 'lis_tlist_head'.  We never	*
* use handle zero, so that value becomes a marker for an available	*
* entry.  We recycle the list elements as they expire or get untimeout-	*
* ed.									*
*									*
************************************************************************/

typedef struct tlist
{
    struct tlist	*next ;		/* thread of these */
    struct timer_list	 tl ;		/* Linux timer structure */
    int			 handle ;	/* SVR4-style handle */

} tlist_t ;

lis_spin_lock_t	  lis_tlist_lock ;
volatile tlist_t *lis_tlist_head ;	/* this list is no particular order */
volatile int	  lis_tlist_handle ;	/* next handle to use */

/*
 * "timeout" is #defined to be this function in dki.h.
 */
toid_t	lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
		    char *file_name, int line_nr)
{
    tlist_t	*tp ;
    tlist_t	*found ;
    int		 psw ;
    int		 handle ;
    int		 unique ;

    unique = 0 ;
    found = NULL ;
    lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;
    while (!unique)
    {
	do
	{					/* next handle */
	    handle = (++lis_tlist_handle) & 0x7FFFFFFF ;
	}
	while (handle == 0) ;			/* prevent use of handle 0 */

	unique = 1 ;				/* hopefully unique handle */
	for (tp = (tlist_t *) lis_tlist_head; tp != NULL; tp = tp->next)
	{					/* find one not-in use */
	    if (tp->handle == handle) unique = 0 ;	/* handle not unique */
	    if (   found == NULL
		&& (   tp->handle == 0
		    || (TL_NEXT(tp->tl) == NULL && TL_PREV(tp->tl) == NULL)
		   )
	       )
	    {
		tp->handle  = 0xFFFFFFFF ;	/* protect from "find" */
		TL_NEXT(tp->tl) = TL_PREV(tp->tl) = &(tp->tl) ;
		found = tp ;
	    }

	    if (!unique && found != NULL)
		break ;				/* try w/another handle */
	}					/* traverse entire list */
    }

    if (found == NULL)				/* must allocate a new one */
    {
	tp = (tlist_t *) LISALLOC(sizeof(*tp),file_name, line_nr) ;
	if (tp == NULL)
	{
	    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
	    return(0) ;		/* no memory for timer */
	}

	tp->next = (tlist_t *) lis_tlist_head ;
	lis_tlist_head = tp ;
    }
    else
    {
	tp = found ;				/* use found timer */
	lis_mark_mem(tp, file_name, line_nr) ;
    }

    tp->handle = handle ;
    lis_tmout(&tp->tl, (tmout_fcn_t *) timo_fcn, (long) arg, ticks) ;
						/* use Linux */
    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;

    return(tp->handle) ;			/* return handle */

} /* lis_timeout_fcn */

toid_t	lis_untimeout(toid_t id)
{
    tlist_t	*tp ;
    int		 psw ;

    lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;
    for (tp = (tlist_t *) lis_tlist_head; tp != NULL; tp = tp->next)
    {						/* find one not-in use */
	if (tp->handle == id)			/* is this our timer?  */
	{
	    lis_mark_mem(tp, "unused", MEM_TIMER) ;
	    if (TL_NEXT(tp->tl) != NULL || TL_PREV(tp->tl) != NULL)
		lis_untmout(&tp->tl) ;		/* stop it */

	    tp->handle = 0 ;			/* make available */
	    break ;				/* done */
	}
    }

    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
    return(0);					/* no rtn value specified */

} /* lis_untimeout */

void lis_terminate_dki(void)
{
	int psw;

	/*
	 *  In case of buggy drivers, timeouts could still happen.
	 *  Better be on the safe side.
	 */
        lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;
	while (lis_tlist_head != NULL) {
		tlist_t *tp = (tlist_t *) lis_tlist_head;

		if (TL_NEXT(tp->tl) != NULL || TL_PREV(tp->tl) != NULL) {
			printk("Cancelling forgotten timeout.\n");
			lis_untmout(&tp->tl);
		}
		lis_tlist_head = tp->next;
		FREE(tp);
	}
        lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
}

/************************************************************************
*                           lis_create_node                             *
*************************************************************************
*									*
* Create a node in the "/dev" directory.				*
*									*
************************************************************************/
#if 0			/* see rant in dki.h */
int      lis_create_node(char *name,
			 int   major_num,
			 int   minor_num,
			 int   spec_type,
			 int   flag)
{
    dev_t	dev ;

    if (flag == CLONE_DEV)
	dev = makedevice(LIS_CLONE, major_num) ;
    else
	dev = makedevice(major_num, minor_num) ;

    return(lis_mknod(name, spec_type, dev)) ;
}
#endif

/************************************************************************
*                          lis_remove_node                              *
*************************************************************************
*									*
* Remove the node from "/dev".						*
*									*
************************************************************************/
#if 0			/* see rant in dki.h */
int      lis_remove_node(char *name)
{
    return(lis_unlink(name)) ;
}
#endif
