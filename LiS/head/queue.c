/*                               -*- Mode: C -*- 
 * queue.c --- streams statistics
 * Author          : Graham Wheeler, Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: queue.c,v 1.9 1996/01/24 19:01:26 dave Exp $
 * Purpose         : provide some queue for LiS
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
 *    You can reach us by email to 
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 *    dave@gcom.com
 */

#ident "@(#) LiS queue.c 2.60 10/08/04 11:38:25 "



/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/stream.h>
#include <sys/osif.h>
#include <stdarg.h>

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

#if !defined(__GNUC__) && !defined(HAS__FUNCTION__)
#define	__FUNCTION__	""
#endif


/*  -------------------------------------------------------------------  */
/*			   Global Variables				 */

/*
 * Queue contention tracking.
 *
 * These variables are used to keep track of lock contention amoung
 * queues.
 */
lis_atomic_t	lis_queue_contention_count ;
lis_spin_lock_t	lis_queue_contention_lock ;

#define	QUEUE_CONTENTION_SIZE	4	/* # locks to keep track of */
queue_t		*lis_queue_contention_table[QUEUE_CONTENTION_SIZE] ;

extern lis_q_sync_t      lis_queue_sync;         /* single threads q running */
extern lis_atomic_t	 lis_runq_req_cnt ;
extern lis_spin_lock_t	 lis_qhead_lock ;
extern int lis_safe_do_putmsg(queue_t *q, mblk_t *mp, ulong qflg, int retry,
		       char *f, int l);

#if defined(CONFIG_DEV)
extern void lis_cpfl(void *p, long a, const char *fcn, const char *f, int l);
#define CP(p,a)		lis_cpfl((p),(a),__FUNCTION__,__LIS_FILE__,__LINE__)
#else
#define lis_cpfl(a,b,c,d,e)
#define CP(p,a)		do {} while (0)
#endif

/*  -------------------------------------------------------------------  */
/*
 * Queue lock contention
 *
 * Base upon a debug bit we track the 4 most popular queues in terms
 * of contention for the semaphore based queue lock.
 *
 */
static void track_queue_contention(queue_t *q)
{
    queue_t		**p = lis_queue_contention_table ;
    queue_t		**e = NULL;
    queue_t		**r = NULL;
    lis_flags_t		  psw ;

    lis_spin_lock_irqsave(&lis_queue_contention_lock, &psw) ;

    for ( ; p != &lis_queue_contention_table[QUEUE_CONTENTION_SIZE]; p++)
    {
	if ((*p) == q)
	{
	    r = NULL ;			/* don't replace */
	    e = NULL ;			/* don't plug empty slot */
	    break ;
	}

	if ((*p) == NULL)
	{
	    if (e == NULL)
		e = p ;			/* first empty slot */
	    continue ;
	}

	if ((*p)->q_contention_cnt < q->q_contention_cnt)
	{
	    e = NULL ;			/* don't plug empty slot */
	    r = p ;
	}
    }

    if (r != NULL)		/* entry to be replaced */
	*r = q ;
    else
    if (e != NULL)		/* empty slot, lock not in table */
	*e = q ;		/* enter it into the table */

    lis_spin_unlock_irqrestore(&lis_queue_contention_lock, &psw) ;
}

static void purge_queue_contention(queue_t *q)
{
    queue_t		**p = lis_queue_contention_table ;
    lis_flags_t		  psw ;

    lis_spin_lock_irqsave(&lis_queue_contention_lock, &psw) ;

    for ( ; p != &lis_queue_contention_table[QUEUE_CONTENTION_SIZE]; p++)
    {
	if ((*p) == q)
	    *p = NULL ;
    }

    lis_spin_unlock_irqrestore(&lis_queue_contention_lock, &psw) ;
}

/*
 * Return a message with the decode of the contention table in it.
 */
mblk_t *lis_queue_contention_msg(void)
{
    mblk_t		 *mp ;
    queue_t		**p = lis_queue_contention_table ;
    queue_t		**p2 ;
    queue_t		 *t ;
    const char		 *name ;
    lis_flags_t		  psw ;
    unsigned		  tot_contention ;

    mp = allocb(QUEUE_CONTENTION_SIZE * 100 + 50, BPRI_MED) ;
    if (mp == NULL)
	return(NULL) ;


    lis_spin_lock_irqsave(&lis_queue_contention_lock, &psw) ;

    /*
     * Bubble sort the table in increasing lock contention order.
     */
    for ( ; p != &lis_queue_contention_table[QUEUE_CONTENTION_SIZE-1]; p++)
    {
	if ((*p) == NULL)
	    continue ;

	for (p2 = p + 1;
	     p2 != &lis_queue_contention_table[QUEUE_CONTENTION_SIZE];
	     p2++
	    )
	{
	    if ((*p2) == NULL)
		continue ;

	    if ( (*p2)->q_contention_cnt < (*p)->q_contention_cnt )
	    {
		t   = *p;
		*p  = *p2 ;
		*p2 = t ;
	    }
	}
    }


    tot_contention = K_ATOMIC_READ(&lis_queue_contention_count) ;
    sprintf(mp->b_wptr, "Total Queue Contention: %u\n", tot_contention) ;
    while (*mp->b_wptr) mp->b_wptr++;

    for (p = lis_queue_contention_table;
	 p != &lis_queue_contention_table[QUEUE_CONTENTION_SIZE];
	 p++
	)
    {
	if ((*p) == NULL)
	    continue ;

	if ((*p)->q_str)
	    name = ((stdata_t *) (*p)->q_str)->sd_name ;
	else
	if (!(*p)->q_qinfo || !(*p)->q_qinfo || !(*p)->q_qinfo->qi_minfo ||
			    !(name = (*p)->q_qinfo->qi_minfo->mi_idname))
	    name = "Anonymous" ;

	sprintf(mp->b_wptr, "%10lu(%ld%%): %s last-owner: %s %d\n",
		(*p)->q_contention_cnt,
		tot_contention ? ((*p)->q_contention_cnt * 100) / tot_contention
			       : 0,
	        name,
		(*p)->q_owner_file ? (*p)->q_owner_file : "Unknown",
		(*p)->q_owner_line) ;
	while (*mp->b_wptr) mp->b_wptr++;
    }

    lis_spin_unlock_irqrestore(&lis_queue_contention_lock, &psw) ;

    return(mp) ;
}



/*  -------------------------------------------------------------------  */
/*
 * Queue locking and unlocking
 *
 * The same task that first locks the queue can continue to lock it
 * in nested fashion.
 *
 * These semaphores are used more like spin locks than producer/consumer
 * coordination via semaphores.  We use a semaphore so that it is safe
 * to sleep or schedule while the queue is locked.
 *
 * This queue lock is only used from background task-level code.  There
 * is a separate spin lock that is used from ISR level to protect
 * fields from modification.  The lock being operated on here is used
 * to protect from re-entry of put and service procedures.
 */
int lis_lockq_fcn(queue_t *q, char *file, int line)
{
    int			 ret = 0 ;
    lis_q_sync_t	*qs ;

    if (q == NULL)
    {
	printk("lis_lockq: called w/NULL q ptr from %s #%d\n", file, line) ;
	return(-EINVAL) ;
    }

    if ((q->q_flag & QOPENING) || (qs = q->q_qsp) == NULL)
	return(0) ;		/* no locking, no contention */

    if (lis_is_current_task(qs->qs_taskp))
    {
	K_ATOMIC_INC(&qs->qs_nest) ;		/* nested q lock */
	return(0) ;
    }

    if (LIS_DEBUG_LOCK_CONTENTION && qs->qs_taskp != NULL)
    {
	K_ATOMIC_INC(&lis_queue_contention_count) ;
	q->q_contention_cnt++ ;
	track_queue_contention(q) ;
    }

    ret = lis_down_fcn(&qs->qs_sem, file, line) ;

    if (ret == 0)
    {
	K_ATOMIC_INC(&qs->qs_nest) ;
	qs->qs_taskp = lis_current_task_ptr ;
	if (LIS_DEBUG_LOCK_CONTENTION)
	{
	    q->q_owner_file = file ;
	    q->q_owner_line = line ;
	}
    }

    return(ret) ;
}

void lis_unlockq_fcn(queue_t *q, char *file, int line)
{
    lis_q_sync_t	*qs = q->q_qsp ;

    if (qs == NULL)
	return ;		/* no locking, no contention */

    if (lis_is_current_task(qs->qs_taskp))
	K_ATOMIC_DEC(&qs->qs_nest) ;

    if (K_ATOMIC_READ(&qs->qs_nest) == 0)
    {
	qs->qs_taskp = NULL ;
	lis_up_fcn(&qs->qs_sem, file, line) ;
    }
}

/*  -------------------------------------------------------------------  */
/*
 * lis_set_q_flags, lis_clr_q_flags
 *
 * This routine accepts a list of queues in varargs format and a bit
 * mask of q_flag bits to be set in all of them.  It gets the ISR lock
 * for all of the queues and then sets the requested bits.
 *
 * The final argument to the routine must be NULL to stop the argument
 * scanning.
 */
static void set_q_flags(ulong flags, int both_qs, va_list argp)
{
    queue_t	*q ;
    queue_t	*oq ;
    lis_flags_t  psw1, psw2 ;

    q = va_arg(argp, queue_t *) ;
    if (q == NULL) return ;


    oq = OTHER(q) ;
    LIS_QISRLOCK(q, &psw1) ;
    if (both_qs) LIS_QISRLOCK(oq, &psw2) ;

    set_q_flags(flags, both_qs, argp) ;		/* recurse and lock more */

    q->q_flag |= flags ;
    if (both_qs) oq->q_flag |= flags ;

    if (both_qs) LIS_QISRUNLOCK(oq, &psw2) ;
    LIS_QISRUNLOCK(q, &psw1) ;
}

void lis_set_q_flags(ulong flags, int both_qs, ...)
{
    va_list	 args ;

    va_start (args, both_qs);
    set_q_flags(flags, both_qs, args) ;
    va_end (args);

}

static void clr_q_flags(ulong flags, int both_qs, va_list argp)
{
    queue_t	*q ;
    queue_t	*oq ;
    lis_flags_t  psw1, psw2 ;

    q = va_arg(argp, queue_t *) ;
    if (q == NULL) return ;


    oq = OTHER(q) ;
    LIS_QISRLOCK(q, &psw1) ;
    if (both_qs) LIS_QISRLOCK(oq, &psw2) ;

    clr_q_flags(flags, both_qs, argp) ;		/* recurse and lock more */

    q->q_flag &= ~flags ;
    if (both_qs) oq->q_flag &= ~flags ;

    if (both_qs) LIS_QISRUNLOCK(oq, &psw2) ;
    LIS_QISRUNLOCK(q, &psw1) ;
}

void lis_clr_q_flags(ulong flags, int both_qs, ...)
{
    va_list	 args ;

    va_start (args, both_qs);
    clr_q_flags(flags, both_qs, args) ;
    va_end (args);

}

/*  -------------------------------------------------------------------  */
/* find_qband
 *
 * Given a queue and a class number, return a pointer to the
 * qband structure for that band.
 *
 * Return NULL if we had to allocate a band structure and we
 * could not get the memory.
 *
 * This routine assumes that LIS_QISRLOCK has been done before calling it.
 */
static INLINE struct qband *
find_qband(queue_t *q, int msg_class)
{
    struct qband	*qp ;
    uchar		 msgclass = (uchar) msg_class ;

    if (q == NULL || msg_class > LIS_MAX_BAND) return(NULL) ;

    qp = q->q_bandp;
    while (qp != NULL)
    {
	if ( qp->qb_first != NULL && qp->qb_first->b_band == msgclass )
	    break;			/* found the band structure */

	qp = qp->qb_next;
    }

    if (qp == NULL)			/* could not find the band struct */
    {					/* find an unused one */
	qp = q->q_bandp;
	while (qp != NULL)
	{
	    if ( qp->qb_first == NULL )
		break;			/* unused band structure */

	    qp = qp->qb_next;
	}
    }

    if (qp == NULL)			/* no such band */
    {					/* and no unused structures */
	qp = (qband_t*) LIS_QBAND_ALLOC(sizeof(struct qband),"qband ");
	if (qp == NULL) return(NULL) ;	/* could not allocate */

	qp->qb_next = q->q_bandp;
	q->q_bandp = qp;
	qp->qb_first = qp->qb_last = NULL;
    }

    if (qp->qb_first == NULL)
    {					/* new or resused qband structure */
	qp->qb_count = 0;
	qp->qb_flag  = 0 ;
	qp->qb_hiwat = q->q_hiwat;
	qp->qb_lowat = q->q_lowat;
    }

    return(qp) ;			/* rtn ptr to band structure */

} /* find_qband */

/*  -------------------------------------------------------------------  */
/* updatequeue - update queue info after a message is added
 *
 * pswp is the caller's value from holding the isr lock on the queue.  We may
 * need to release and reacquire that lock if we decide to call qenable.
 */
static INLINE void
updatequeue(queue_t *q, mblk_t *mp, int from_insq, lis_flags_t *pswp,
		int band_ptrs)
{
    uchar	 msg_class = mp->b_band;
    uchar	 msg_type  = mp->b_datap->db_type ;

    if (msg_type >= QPCTL || msg_class == 0)
    {
        q->q_count += lis_msgsize(mp);
        if (q->q_count > q->q_hiwat)
	    q->q_flag |= QFULL | QWASFULL ;
    }
    else
    {						/* qband considerations */
	struct qband *qp = find_qband(q, msg_class) ; /* get qband struct */

	if (qp != NULL)
	{
	    qp->qb_count += lis_msgsize(mp);	/* qband stats */
	    if (qp->qb_count > qp->qb_hiwat)
		qp->qb_flag |= QB_FULL | QB_WASFULL ;

	    if (band_ptrs)
	    {
		if (qp->qb_first == NULL)
		    qp->qb_first = mp;		/* this is 1st msg in band */
		else
		if (mp->b_next == qp->qb_first)	/* mp inserted b4 first msg */
		    qp->qb_first = mp;		/* mp is now 1st in band */

		if (qp->qb_last == NULL)		/* new qband */
		    qp->qb_last = mp;
		else
		if (mp->b_prev == qp->qb_last)	/* mp inserted after last msg */
		    qp->qb_last = mp;		/* mp is now last in band */
		/* Otherwise is could be somewhere in the middle */
	    }
	}
    }

    if (from_insq && (q->q_flag & QNOENB))	/* noenable in effect */
    {
	return ;				/* insq can't cause sched */
    }

    if (msg_type >= QPCTL ||
       (!(q->q_flag & QNOENB) && (q->q_flag & QWANTR) != 0))
    {
	LIS_QISRUNLOCK(q, pswp) ;		/* unlock b4 qenable */
        lis_qenable(q);
	LIS_QISRLOCK(q, pswp) ;			/* reacquire the lock */
    }

}/*updatequeue*/

/*  -------------------------------------------------------------------  */
/* find_q_spot
 *
 * Find "the correct spot" in the queue for the message.
 *
 * Find the element that we will link this message in front of. 
 *
 * find_q_spot_head finds the first message of the proper class.
 * find_q_spot_tail finds the message beyond the last message of
 * the given class.
 *
 * Return NULL if there is no message that we can link in front of.
 * This will be the return for an empty queue, or for a message that
 * needs to be placed on the end of the queue.
 *
 * This routine assumes that LIS_QISRLOCK has been done before calling it.
 */
static INLINE mblk_t *
find_q_spot_head(queue_t *q, mblk_t *mp)
{
    uchar	 msg_class ;
    uchar	 mp_type ;
    mblk_t	*p_cont ;
    mblk_t	*firstp ;

    if (q == NULL) return(NULL) ;
    if ((firstp = q->q_first) == NULL)
	return(NULL) ;

    msg_class = mp->b_band;
    if ((mp_type = mp->b_datap->db_type) >= QPCTL)
	mp->b_band = 0 ;			/* AT&T STRMS pgrmrs guide */

    if (mp_type >= QPCTL)			/* msg is high priority */
	return(firstp) ;			/* goes as the 1st msg */

    if (   firstp->b_datap->db_type < QPCTL	/* 1st is not high prior */
	&& msg_class >= firstp->b_band		/* greater class than 1st msg */
       )
	return(firstp) ;			/* goes as 1st msg */

    /* find the first non-high-priority message in the queue */
    for (p_cont = firstp;
	 (   p_cont != NULL
	  && p_cont->b_datap->db_type >= QPCTL   /* skip high prior msgs */
	 );
	 p_cont = p_cont->b_next
	) ;

    /* find the first message after that with class <= our msg */
    for (;
	 (   (p_cont != NULL)
	  && (p_cont->b_band > msg_class)	/* skip higher band msgs */
	 );
	 p_cont = p_cont->b_next
	) ;

    return(p_cont) ;			/* insert before here */

} /* find_q_spot_head */

static INLINE mblk_t *
find_q_spot_tail(queue_t *q, mblk_t *mp)
{
    uchar	 msg_class ;
    uchar	 mp_type ;
    mblk_t	*p_cont ;
    mblk_t	*lastp ;
    mblk_t	*firstp ;

    if (q == NULL) return(NULL) ;
    if ((firstp = q->q_first) == NULL)
	return(NULL) ;

    msg_class = mp->b_band;
    if ((mp_type = mp->b_datap->db_type) >= QPCTL)
	mp->b_band = 0 ;			/* AT&T STRMS pgrmrs guide */

        /* find a spot at the end of a message class */
    lastp = q->q_last ;
    if (lastp->b_datap->db_type >= QPCTL)	/* q full of hi-prior msgs */
	return(NULL) ;				/* it goes on the end */

    if (   mp_type < QPCTL			/* non-priority msg */
        && msg_class <= lastp->b_band		/* lower than last msg */
       )
	return(NULL) ;				/* it goes on the end */

    /* have to find a spot in the middle of the queue */
    /* first, skip over all of the high-priority msgs */

    for (p_cont = firstp;
	 (   p_cont != NULL
	  && p_cont->b_datap->db_type >= QPCTL   /* skip high prior msgs */
	 );
	 p_cont = p_cont->b_next
	) ;

    /* if the message is a high priority message then we are done */

    if (mp_type < QPCTL)			/* non-priority msg */
    {
    	for (;					/* find end of mp's band */
    	     (   (p_cont != NULL)
	      && (p_cont->b_band >= msg_class)	/* skip higher band msgs */
	     );
	     p_cont = p_cont->b_next
	    ) ;
    }

    return(p_cont) ;				/* insert before here */

} /* find_q_spot_tail */


/*  -------------------------------------------------------------------  */
/* ins_before
 *
 * This routine will insert the given message in front of the
 * message whose ptr is passed in.
 *
 * This routine assumes that LIS_QISRLOCK has been done before calling it.
 */
static INLINE void
ins_before(queue_t *q, mblk_t *before_msg, mblk_t *mp)
{
    LisUpCount(MSGSQD) ;			/* one more msg queued */
    if (q->q_first == NULL)			/* empty queue */
    {
    	mp->b_prev = mp->b_next = NULL;		/* quick & easy */
    	q->q_first = q->q_last = mp;
    	return ;				/* done */
    }

    if (before_msg == NULL)			/* add to end of queue */
    {
	q->q_last->b_next	= mp;
	mp->b_prev		= q->q_last;
	q->q_last		= mp;
	mp->b_next		= NULL;
	return ;
    }

    /* insert in front of 'before_msg' */

    mp->b_next			   = before_msg;
    mp->b_prev			   = before_msg->b_prev;

    if (before_msg->b_prev != NULL)
	before_msg->b_prev->b_next = mp;
    else
	q->q_first		   = mp;

    before_msg->b_prev		   = mp;

} /* ins_before */

/*  -------------------------------------------------------------------  */
/* chk_band - check a queue band for back-enable possibilities
 */
static INLINE void
chk_band(queue_t *q, struct qband *qp)
{
    if (qp->qb_count <= qp->qb_hiwat)
	qp->qb_flag &= ~QB_FULL;

    if (   (qp->qb_flag & QB_WASFULL)	/* q-band was full */
	&& qp->qb_count <= qp->qb_lowat	/* now below low water?	*/
       )
    {
	qp->qb_flag &= ~QB_WASFULL;	/* no longer full	*/
	if (qp->qb_flag & QB_WANTW)
	{				/* xfer flag to main queue */
	    qp->qb_flag &= ~QB_WANTW;
	    q->q_flag   |= QWANTW;
	}

	q->q_flag |= QBACK;
    }
}

/*  -------------------------------------------------------------------  */
/* adjust_q_count
 *
 * Performed for a message that was just removed from a queue, or is
 * just about to be.
 */
static INLINE void
adjust_q_count(queue_t *q, mblk_t *mp, int band_ptrs)
{
    q->q_flag &= ~QWANTR;		/* no implicit qenable now */

    if (mp->b_datap->db_type < QPCTL && mp->b_band > 0)
    {
	struct qband *qp = find_qband(q, mp->b_band) ; /* get qband struct */

	if (qp != NULL)
	{
	    qp->qb_count -= lis_msgsize(mp);	/* qband stats */

	    /* mp may be in the middle or end of a band if this was	*/
	    /* done via rmvq().						*/

	    if (band_ptrs && qp->qb_first == mp)/* rmving 1st msg of band */
	    {
		if (mp->b_next && mp->b_next->b_band == mp->b_band)
		    qp->qb_first = mp->b_next ;	/* nxt msg in same band */
		else
		{
		    qp->qb_first = NULL ;	/* band now empty */
		    qp->qb_last  = NULL;	/* band struct available */
		}
	    }

	    if (band_ptrs && qp->qb_last == mp)	/* rmving last msg of band */
	    {
		if (mp->b_prev && mp->b_prev->b_band == mp->b_band)
		    qp->qb_last = mp->b_prev ;	/* nxt msg in same band */
		else
		{
		    qp->qb_first = NULL ;	/* band now empty */
		    qp->qb_last  = NULL;	/* band struct available */
		}
	    }

	    chk_band(q, qp) ;			/* chk for backenable */
	}
    }
    else
    {
	q->q_count -= lis_msgsize(mp);
	if (q->q_count < q->q_hiwat)
	    q->q_flag &= ~QFULL;

	if (   (q->q_flag & QWASFULL)	/* queue was full		*/
	    && q->q_count <= q->q_lowat	/* below low water?		*/
	   )
	{
	    q->q_flag |= QBACK;		/* must back enable		*/
	    q->q_flag &= ~QWASFULL;	/* no longer full		*/
	}
    }
}
/*  -------------------------------------------------------------------  */
/* rmv_msg
 *
 * Remove a message from a queue.  The message is known to reside in
 * the queue.
 *
 * This routine assumes that LIS_QISRLOCK has been done before calling it.
 */
static INLINE mblk_t *
rmv_msg(queue_t *q, mblk_t *mp)
{
    if (mp == NULL)			/* no message */
    {					/* empty queue */
	q->q_flag |= QWANTR | QBACK;	/* note that someone wants to read */
	return(mp) ;
    }

    adjust_q_count(q, mp, 1) ;		/* adjust count and flags */

    /* perform q remove here */

    if (mp->b_prev != NULL)
    	mp->b_prev->b_next = mp->b_next;
    else
    	q->q_first = mp->b_next;

    if (mp->b_next != NULL)
    	mp->b_next->b_prev = mp->b_prev;
    else
    	q->q_last = mp->b_prev;

    mp->b_prev = mp->b_next = NULL;

    LisDownCount(MSGSQD) ;		/* one fewer msg queued */

    return(mp) ;			/* return our argument */

} /* rmv_msg */


/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */


/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/*
 * lis_check_q_magic
 *
 * lis_check_q_magic is only called by LiS when q == NULL or
 * q->q_magic != Q_MAGIC. If q != NULL then it is an automatic
 * conclusion that q->q_magic != Q_MAGIC.
 *
 * log errors when q == NULL or q->q_magic != Q_MAGIC
 * Return 0 always
 */
int	lis_check_q_magic(queue_t *q, char *file, int line)
{
    if (!q) 
	{
	printk("%s #%u: Queue pointer is NULL\n", file, line) ;
	}
    else
       	{
	printk("%s #%u: Queue magic number is 0x%lx, should be 0x%lx\n",
       		file, line, q->q_magic, Q_MAGIC) ;
       	}
    return(0) ;
  
} /* lis_check_q_magic */
  
/* backq - Return the queue which feeds this one.
 *
 * If q is a read queue then return the ptr to the downstream queue.
 * If q is a write queue then return the ptr to the upstream queue.
 */
queue_t * _RP
lis_backq_fcn(queue_t *q, char *f, int l)
{
    queue_t	*oq ;
    queue_t	*nq ;

    if (   !LIS_QMAGIC(q,f,l)
	|| !(oq = OTHER(q))
	|| !(nq = oq->q_next)
       )
        return NULL;

    /*
     *  handle twist in FIFOs/pipes also...
     */
    return (nq == q ? oq : OTHER(nq));

}/*lis_backq_fcn*/

queue_t * _RP
lis_backq(queue_t *q)
{
    return(lis_backq_fcn(q, __FILE__,__LINE__)) ;
}


/*  -------------------------------------------------------------------  */

/* lis_backenable - enable back queue if QWANTW is set, i.e.
 *	if a back queue is full.
 *
 *	If the queue is marked as closing then do not stop
 *	at the first queue with a service procedure, keep
 *	going as long as the queues are empty.  The idea is
 *	to eventually enable the service procedure for the
 *	queue head.
 *
 *	Caller has NOT acquired the queue isr lock.
 */
static void 
lis_backenable(queue_t *q)
{
    lis_flags_t	 psw ;
    lis_flags_t	 pswoq ;
    queue_t	*oq = q ;			/* original queue */

    LIS_RDQISRLOCK(oq, &pswoq) ;
    if (   (   (oq->q_flag & (QWANTW|QBACK))==(QWANTW|QBACK)
            || (oq->q_flag & QCLOSEWT)
           )
	&& oq->q_count <= oq->q_lowat
       )
    {
	/* Someone wants to write and there is space. Find a previous
	 * queue to back-enable and enable it.
	 *
	 * In the loop, "q" is a different queue than the one that was passed
	 * in.  If the stream is closing, the mark_closing routine will have
	 * marked all the queues from the queue head down to the driver as
	 * QCLOSING, which will keep us from scanning them.
	 */
	LIS_RDQISRUNLOCK(q, &pswoq) ;
	LIS_QISRLOCK(oq, &pswoq) ;
    	oq->q_flag &= ~(QWANTW|QBACK);
	LIS_QISRUNLOCK(oq, &pswoq) ;		/* unlock orig queue */
        while((q = lis_backq(q)) != NULL)
	{
	    LIS_RDQISRLOCK(q, &psw) ;		/* lock new queue */
	    if (   !LIS_CHECK_Q_MAGIC(q)
		|| (q->q_flag & (QPROCSOFF | QCLOSING))
	       )				/* busted or closing queue */
	    {
		LIS_RDQISRUNLOCK(q, &psw) ;
		break ;
	    }

	    if (   q->q_qinfo != NULL
		&& q->q_qinfo->qi_srvp != NULL
		&& lis_canenable(q))		/* noenable not in effect */
	    {
		LIS_RDQISRUNLOCK(q, &psw) ;	/* unlock new queue */
	     	lis_qenable(q);
	     	if (!F_ISSET(q->q_flag,QCLOSEWT) || q->q_count != 0)
		    break;			/* quit searching */
	    }
	    else				/* keep searching */
		LIS_RDQISRUNLOCK(q, &psw) ;	/* unlock new queue */
	}					/* iterate w/queue unlocked */
    }
    else					/* no back-enabling to do */
	LIS_RDQISRUNLOCK(oq, &pswoq) ;		/* unlock orig qeuue */

}/*lis_backenable*/

/*  -------------------------------------------------------------------  */
/* lis_getq - get message from head of queue
 */

mblk_t * _RP
lis_getq(queue_t *q)
{
    mblk_t *rtn;
    lis_flags_t     psw;

    if (q == NULL)
	return NULL;			/* sanity check	*/

    LIS_QISRLOCK(q, &psw) ;
    if (!LIS_CHECK_Q_MAGIC(q) || (q->q_flag & (QCLOSING |QPROCSOFF)))
    {
	LIS_QISRUNLOCK(q, &psw) ;
	return(NULL) ;
    }

    rtn = rmv_msg(q, q->q_first) ;
    LIS_QISRUNLOCK(q, &psw) ;		/* unlock b4 calling backenable */
    lis_backenable(q);
    return rtn;

}/*lis_getq*/

/*  -------------------------------------------------------------------  */
/* putq- put a message into a queue
 */
int _RP
lis_putq(queue_t *q, mblk_t *mp)
{
    lis_flags_t     psw;
    if (mp == NULL || q == NULL)
	return 0;

    LIS_QISRLOCK(q, &psw) ;
    if (   !LIS_CHECK_Q_MAGIC(q)
	|| (q->q_flag & (QCLOSING | QPROCSOFF))
       )
    {
	LIS_QISRUNLOCK(q, &psw) ;
    	return 0;
    }

    ins_before(q, find_q_spot_tail(q, mp), mp) ;

    updatequeue(q, mp, 0, &psw, 1);	/* Update queue counts and flags */
    LIS_QISRUNLOCK(q, &psw) ;
    return 1;				/* success */

}/*lis_putq*/

void _RP lis_putqf(queue_t *q, mblk_t *mp)
{
    if (!lis_putq(q,mp)) freemsg(mp) ;
}


/*  -------------------------------------------------------------------  */
/* putbq - return a message to a queue
 */
int _RP
lis_putbq(queue_t *q, mblk_t *mp)
{
    lis_flags_t     psw;

    if (mp == NULL || q == NULL)
    	return 0;

    LIS_QISRLOCK(q, &psw) ;
    if (   !LIS_CHECK_Q_MAGIC(q)
	|| (q->q_flag & (QCLOSING | QPROCSOFF))
       )
    {
	LIS_QISRUNLOCK(q, &psw) ;
    	return 0;
    }

    ins_before(q, find_q_spot_head(q, mp), mp) ;

    updatequeue(q, mp, 0, &psw, 1);	/* Update queue counts and flags */
    LIS_QISRUNLOCK(q, &psw) ;
    return 1;				/* success */

}/*lis_putbq*/


void _RP lis_putbqf(queue_t *q, mblk_t *mp)
{
    if (!lis_putbq(q, mp)) freemsg(mp) ;
}

/*  -------------------------------------------------------------------  */
/*insq - insert mp before emp. If emp is NULL, insert at end
 *	of queue. If the insertion is out-of-order, the insert fails.
 *	Returns 1 on success; 0 otherwise.
 */
int _RP
lis_insq(queue_t *q, mblk_t *emp, mblk_t *mp)
{
    lis_flags_t     psw;

    if (q == NULL || mp == NULL) return 0;

    LIS_RDQISRLOCK(q, &psw) ;
    if (   !LIS_CHECK_Q_MAGIC(q)
	|| (q->q_flag & (QCLOSING | QPROCSOFF))
       )
    {
	LIS_RDQISRUNLOCK(q, &psw) ;
    	return 0;
    }


    /* check to see if this insertion is proper */

    if (   mp->b_datap->db_type < QPCTL		/* non-high priority msg */
        && emp != NULL				/* have emp */
        && (   emp->b_datap->db_type >= QPCTL	/* emp is high priority */
            || mp->b_band < emp->b_band		/* too high a class */
            || (   emp->b_prev != NULL
                && mp->b_band > emp->b_prev->b_band
               )
           )
       )
    {
	LIS_RDQISRUNLOCK(q, &psw) ;
	LisUpFailCount(MSGSQD) ;		/* error queueing msg */
	return(0) ;				/* failed */
    }

    LIS_RDQISRUNLOCK(q, &psw) ;
    LIS_QISRLOCK(q, &psw) ;
    ins_before(q, emp, mp) ;

    updatequeue(q, mp, 1, &psw, 1);
    LIS_QISRUNLOCK(q, &psw) ;
    return 1;					/* success */

}/*lis_insq*/

/*  -------------------------------------------------------------------  */
/* rmvq - remove a message from a queue. If the message
 *		does not exist, panic.
 */
void _RP
lis_rmvq(queue_t *q, mblk_t *mp)
{
    mblk_t *bp;
    lis_flags_t     psw;
    if (mp == NULL) return;
    if (q == NULL)
    {
	printk("Null queue in rmvq");
	return ;
    }

    LIS_RDQISRLOCK(q, &psw) ;
    if (!LIS_CHECK_Q_MAGIC(q) || (q->q_flag & (QCLOSING | QPROCSOFF)))
    {
	LIS_RDQISRUNLOCK(q, &psw) ;
	return ;
    }

    /* check if mesage is in queue */
    bp = q->q_first;
    while (bp && bp != mp)
	bp = bp->b_next;
    if (bp == NULL)
    {
	LIS_RDQISRUNLOCK(q, &psw) ;
	printk("rmvq: message not in queue");
	return ;
    }

    LIS_RDQISRUNLOCK(q, &psw) ;
    /* Remove message from queue */
    LIS_QISRLOCK(q, &psw) ;
    rmv_msg(q, bp) ;
    LIS_QISRUNLOCK(q, &psw) ;			/* unlock b4 backenable */
    lis_backenable(q);

}/*lis_rmvq*/

/*  -------------------------------------------------------------------  */
/* lis_put_rput_q
 *
 * Like putq except that it inserts the message into the stream head
 * rput queue and does the message accounting in the actual stream
 * head queue.
 *
 * The stream head read queue always has QNOENB set, so the updatequeue
 * routine will not attempt a qenable.  The only time the queue gets
 * enabled is in here when the first message is put onto the queue.
 */
void lis_put_rput_q(stdata_t *hd, mblk_t *mp)
{
    lis_flags_t     psw;
    int need_qenable = 0 ;

    LIS_QISRLOCK(hd->sd_rq, &psw) ;

    mp->b_prev = NULL ;
    if (hd->sd_rput_hd == NULL)
    {					/* empty list */
	mp->b_next = NULL ;
	hd->sd_rput_tl = mp ;
	hd->sd_rput_hd = mp ;
	need_qenable = 1 ;
    }
    else
    {					/* elements in list */
	hd->sd_rput_tl->b_next = mp ;
	hd->sd_rput_tl = mp ;
    }

    updatequeue(hd->sd_rq, mp, 1, &psw, 0); /* Update queue counts and flags */

    LIS_QISRUNLOCK(hd->sd_rq, &psw) ;

    if (need_qenable)			/* after unlocking the queue */
	qenable(hd->sd_rq) ;		/* enable svc proc */
}

/*  -------------------------------------------------------------------  */
/* lis_get_rput_q
 *
 * Like getq except that it removes the message from the stream head
 * rput queue and does the message accounting in the actual stream
 * head queue.
 */
mblk_t *lis_get_rput_q(stdata_t *hd)
{
    lis_flags_t     psw;
    mblk_t	*mp = NULL ;

    LIS_QISRLOCK(hd->sd_rq, &psw) ;
    if ((mp = hd->sd_rput_hd) != NULL)
    {
	hd->sd_rput_hd = mp->b_next ;	/* link around message */
	mp->b_next = NULL ;
	mp->b_prev = NULL ;
	if (hd->sd_rput_hd == NULL)	/* queue empty?  */
	    hd->sd_rput_tl = NULL ;	/* clobber tail ptr */
	adjust_q_count(hd->sd_rq, mp, 0) ; /* adjust count and flags */
	LIS_QISRUNLOCK(hd->sd_rq, &psw) ;
	lis_backenable(hd->sd_rq);
    }
    else
	LIS_QISRUNLOCK(hd->sd_rq, &psw) ;

    return(mp) ;
}

/*  -------------------------------------------------------------------  */
/*
 * Put message into the deferred list.  Queue is ISR locked by caller
 */
void lis_defer_msg(queue_t *q, mblk_t *mp, int retry, lis_flags_t *psw)
{
    if (LIS_DEBUG_PUTNEXT)
    {
	printk("lis_defer_msg: %s to \"%s\" size %d retry=%d\n",
			    lis_msg_type_name(mp), lis_queue_name(q),
			    lis_msgsize(mp), retry) ;

	if (LIS_DEBUG_ADDRS)
	    printk("        q=%p, mp=%p, mp->b_rptr=%p, wptr=%p\n",
		    q, mp, mp->b_rptr, mp->b_wptr);
    }

    mp->b_prev = NULL ;
    if (retry)
    {					/* insert at head */
	mp->b_next = q->q_defer_head ;
	q->q_defer_head = mp ;
	if (q->q_defer_tail == NULL)
	    q->q_defer_tail = mp ;
	return ;
    }

    /* insert at tail */
    mp->b_next = NULL ;
    if (q->q_defer_head == NULL)
	q->q_defer_head = mp ;

    if (q->q_defer_tail)
	q->q_defer_tail->b_next = mp ;

    q->q_defer_tail = mp ;
    updatequeue(q, mp, 1, psw, 0);	/* Update queue counts and flags */
}

void lis_do_deferred_puts(queue_t *q)
{
    mblk_t	   *mp ;
    lis_flags_t     psw;

    LIS_QISRLOCK(q, &psw) ;
    if (q->q_defer_head == NULL || (q->q_flag & QDEFERRING))
    {
	LIS_QISRUNLOCK(q, &psw) ;
	return ;
    }

    q->q_flag |= QDEFERRING ;
    while ((mp = q->q_defer_head) != NULL)
    {
	q->q_defer_head = mp->b_next ;
	if (q->q_defer_head == NULL)
	    q->q_defer_tail = NULL ;
	mp->b_next = NULL ;
	mp->b_prev = NULL ;
	adjust_q_count(q, mp, 0) ;	/* adjust count and flags */
	LIS_QISRUNLOCK(q, &psw) ;

	if (LIS_DEBUG_PUTNEXT)
	{
	    printk("lis_do_deferred_puts: %s from \"%s\" size %d\n",
				lis_msg_type_name(mp), lis_queue_name(q),
				lis_msgsize(mp)) ;

	    if (LIS_DEBUG_ADDRS)
		printk("        q=%p, mp=%p, mp->b_rptr=%p, wptr=%p\n",
			q, mp, mp->b_rptr, mp->b_wptr);
	}

	if (!lis_safe_do_putmsg(q, mp, QOPENING, 1, __FILE__, __LINE__))
	{
	    LIS_QISRLOCK(q, &psw) ;
	    break ;
	}
	LIS_QISRLOCK(q, &psw) ;
    }
    q->q_flag &= ~QDEFERRING ;
    LIS_QISRUNLOCK(q, &psw) ;
}

/*  -------------------------------------------------------------------  */

/* lis_qenable - schedule a queue for service
 *
 * If the qeuue is not locked upon entry, lock it for our operations.
 *
 * Caller does NOT hold queue isr lock.
 *
 * Locking order:  Get the lis_qhead_lock before the queue isr lock.  See
 * 		   queuerun in stream.c for similar lock pair acquisition.
 */
void lis_retry_qenable(queue_t *q)
{
    lis_flags_t pswq;

    CP(q, q->q_flag) ;
    LIS_QISRLOCK(q, &pswq) ;
    if ((q->q_flag & QWANTENAB) && !(q->q_flag & Q_INH_ENABLE))
    {
	q->q_flag &= ~QWANTENAB ;
	LIS_QISRUNLOCK(q, &pswq) ;
	lis_qenable(q) ;
	return ;
    }

    LIS_QISRUNLOCK(q, &pswq) ;
}

void  _RP
lis_qenable(queue_t *q)
{
    lis_flags_t psw, pswq;
    stdata_t *hd ;

    if (!LIS_CHECK_Q_MAGIC(q)) return ;

    /* is there a service procedure? if not, just return */
    if (q->q_qinfo == NULL || q->q_qinfo->qi_srvp == NULL)
	return ;

    hd = (stdata_t *) q->q_str ;
    /* If any of the "inhibit" bits are set do not schedule the queue.
     * If any of the "defer" bits are set then set the QWANTENAB flag
     * so that the qenable can be retried when the indicated operation
     * is complete.
     */
    LIS_QISRLOCK(q, &pswq) ;
    if (q->q_flag & Q_INH_ENABLE)
    {
	if (q->q_flag & Q_DEFER_ENABLE)
	    q->q_flag |= QWANTENAB ;		/* qenable called */
	CP(q->q_str, q->q_flag) ;
	LIS_QISRUNLOCK(q, &pswq) ;
	return ;
    }
    q->q_flag |= QENAB;
    CP(q->q_str, q->q_flag) ;
    LIS_QISRUNLOCK(q, &pswq) ;

    /*
     * If this is a stream head queue then increase usage count on
     * the stream head.  We will decrement after calling the service
     * procedure.
     */
    if (   hd != NULL
	&& hd->magic == STDATA_MAGIC
	&& (hd->sd_wq == q || hd->sd_rq == q)
       )
	lis_head_get(hd) ;

    /* link into tail of list of scheduled queues (qtail) */
    lis_spin_lock_irqsave(&lis_qhead_lock, &psw) ;
#if 0			/* minimize time with qhead_lock held */
    lis_cpfl((void *) lis_qhead, (long) lis_qtail,
				__FUNCTION__, __FILE__, __LINE__) ;
    if (   (lis_qhead != NULL && lis_qtail == NULL)
	|| (lis_qhead == NULL && lis_qtail != NULL)
       )
	printk("LiS: qenable before: Qhead error: "
	       "lis_qhead=%lx lis_qtail=%lx\n",
	       lis_qhead, lis_qtail) ;
#endif
    q->q_link = NULL;
    if (lis_qhead == NULL)
	lis_qhead = q;
    else
    	lis_qtail->q_link = q;
    lis_qtail = q;		
#if 0			/* minimize time with qhead_lock held */
    if (   (lis_qhead != NULL && lis_qtail == NULL)
	|| (lis_qhead == NULL && lis_qtail != NULL)
       )
	printk("LiS: qenable after: Qhead error: "
	       "lis_qhead=%lx lis_qtail=%lx\n",
	       lis_qhead, lis_qtail) ;

    lis_cpfl((void *) lis_qhead, (long) lis_qtail,
				__FUNCTION__, __FILE__, __LINE__) ;
#endif
    K_ATOMIC_INC(&lis_runq_req_cnt) ;
    lis_spin_unlock_irqrestore(&lis_qhead_lock, &psw) ;

    LisUpCount(QSCHEDS) ;
    lis_setqsched(0);			/* schedule, but don't call now */

}/*lis_qenable*/

/*  -------------------------------------------------------------------  */

/* lis_setq - Set queue variables
 */
void
lis_setq(queue_t * q, struct qinit *rinit, struct qinit *winit)
{
    if (rinit == NULL || winit == NULL)
    {
	printk("lis_setq: NULL pointer(s) to qinit structures\n") ;
	return ;
    }

    if (q)
    {
	lis_flags_t psw;

	if (!LIS_CHECK_Q_MAGIC(q)) return ;

	LIS_QISRLOCK(q, &psw) ;
        q->q_qinfo  = rinit;
        q->q_lowat  = rinit->qi_minfo->mi_lowat;
        q->q_hiwat  = rinit->qi_minfo->mi_hiwat;
        q->q_minpsz = rinit->qi_minfo->mi_minpsz; 
        q->q_maxpsz = rinit->qi_minfo->mi_maxpsz; 
	LIS_QISRUNLOCK(q, &psw) ;

        q = LIS_WR(q);
	LIS_QISRLOCK(q, &psw) ;
        q->q_qinfo  = winit;
        q->q_lowat  = winit->qi_minfo->mi_lowat;
        q->q_hiwat  = winit->qi_minfo->mi_hiwat;
        q->q_minpsz = winit->qi_minfo->mi_minpsz; 
        q->q_maxpsz = winit->qi_minfo->mi_maxpsz; 
	LIS_QISRUNLOCK(q, &psw) ;
    }
}/*lis_setq*/

/*  -------------------------------------------------------------------  */
/* flush_worker - common routine for flushing queues
 *
 * The general approach to all flushes is to take all the messages
 * locally into this procedure, zero out the queue, walk the message
 * list and selectively free messages or put them back into the queue.
 *
 * The queue band structures get rebuilt as we put the messages back
 * into the queue (assuming that we are not flushing the whole queue).
 *
 * When all this is done, check all the bands, and the queue itself, for
 * flow control conditions that would lead to a backenable operation. 
 * We use the approach that if any band becomes flow control enabled then
 * we backenable the queue.  Hopefully the upstream module is playing
 * the "qband" game and knows to use bcanput or bcanput_anyband rather
 * than a simple canput.
 *
 * The "band" parameter is set to -1 to flush the whole queue or
 * is set to the specific band to flush if flushing a band.
 *
 * The "flush_all" parameter is non-zero if we are to flush all messages,
 * not just data messages.
 */
static void
flush_worker(queue_t *q, int band, int flush_all)
{
    mblk_t	 *mp;			/* owns entire list of msgs */
    mblk_t	 *mp_deferred ;
    mblk_t	 *p_next;
    struct qband *qp ;
    int		  q_flag;
    lis_flags_t   psw;
    int		  mtype ;

    if (!LIS_CHECK_Q_MAGIC(q)) return ;

    /*
     * First, take away the messages and clear out the queue pointers
     * for the queue itself and the bands.
     */
    LIS_QISRLOCK(q, &psw) ;
    q_flag = q->q_flag;
    mp = q->q_first;			/* mp is now the entire list of msgs */
    q->q_first = NULL;			/* q itself is now empty */
    q->q_last = NULL;
    q->q_count = 0;
    q->q_flag &= ~(QFULL|QWASFULL|QWANTR|QWANTW);

    for (qp = q->q_bandp; qp != NULL; qp = qp->qb_next)
    {
	qp->qb_first = NULL ;
	qp->qb_last  = NULL ;
	qp->qb_count = 0;
	qp->qb_flag &= ~QB_FULL ;
    }

    mp_deferred = q->q_defer_head ;
    q->q_defer_head = NULL ;
    q->q_defer_tail = NULL ;
    LIS_QISRUNLOCK(q, &psw) ;

    /*
     * Now walk the message list and selectively free or re-queue messages.
     */
flush_list:
    while (mp != NULL)
    {
    	p_next = mp->b_next; 
	mp->b_next = NULL ;			/* clobber link */
	mtype = mp->b_datap->db_type ;

	if (   (   band < 0			/* flush all bands */
	        || mp->b_band == band		/* msg in selected band */
	       )
	    && (   (   band == 0		/* flushing band 0 */
	 	    && mtype < QPCTL		/* is "ordinary" message */
	           )
		|| (   band != 0		/* flushing band or all */
		    && (   flush_all		/* flush all msgs in band */
			|| lis_datamsg(mtype)	/* is data msg */
		       )
		   )
	       )
	   )
	{
	    LisDownCount(MSGSQD) ;		/* one fewer msg queued */
	    if ( LIS_DEBUG_FLUSH )
		printk("lis_flush: "
		       "flushing %s (%d) band %d from queue \"%s\"\n",
			lis_msg_type_name(mp), mtype, 
			mp->b_band, lis_queue_name(q)) ;

	    if (mtype == M_PASSFP)
		lis_free_passfp(mp);
	    else
		lis_freemsg(mp);
	}
	else
	{
	    if ( LIS_DEBUG_FLUSH )
		printk("lis_flush: "
		       "retaining %s (%d) band %d in queue \"%s\"\n",
			lis_msg_type_name(mp), mtype,
			mp->b_band, lis_queue_name(q)) ;

	    lis_putqf(q, mp);
	}

	mp = p_next;
    }

    if (mp_deferred != NULL)			/* any deferred messages? */
    {
	mp = mp_deferred ;
	mp_deferred = NULL ;
	goto flush_list ;			/* do it again */
    }

    LIS_QISRLOCK(q, &psw) ;

    q->q_flag |= QWANTR ;			/* re-arm queue */

    /*
     * Finally, check each band, and the queue itself, for flow control
     * conditions that would allow a backenable.
     */
    for (qp = q->q_bandp; qp != NULL; qp = qp->qb_next)
    {
	chk_band(q, qp) ;		/* chk band for backenable */
    }

    if (   (q_flag & QWASFULL)		/* queue had been above hi water */
	&& q->q_count <= q->q_lowat	/* back below lo water now */
       )
    {
	q->q_flag |= (QBACK | QWANTW);	/* must back enable */
    }

    if (q->q_flag & (QCLOSING | QPROCSOFF))
    {
	LIS_QISRUNLOCK(q, &psw) ;
	return ;			/* don't enable anything */
    }

    LIS_QISRUNLOCK(q, &psw) ;		/* unlock b4 calling backenable */
    lis_backenable(q);			/* will check conditions */
    LIS_QISRLOCK(q, &psw) ;		/* relock */

    /*
     * If the queue was originally armed and now has messages in it
     * then go ahead and enable the queue itself.  Seems like an
     * unlikely case.
     */
    if ((q->q_flag & QWANTR) && q->q_first != NULL && lis_canenable(q))
    {
	LIS_QISRUNLOCK(q, &psw) ;
	lis_qenable(q) ;
    }
    else
	LIS_QISRUNLOCK(q, &psw) ;

} /* flush_worker */

/*  -------------------------------------------------------------------  */
/* lis_flushband - flush messages in a specified priority band.
 *	flag can be FLUSHDATA (flush only M_DATA, M_DELAY,
 *	M_PROTO, M_PCPROTO) or FLUSHALL.
 */
void _RP
lis_flushband(queue_t *q, unsigned char band, int flag)
{
    flush_worker(q, (int) band, flag & FLUSHALL) ;

}/*lis_flushband*/

/*  -------------------------------------------------------------------  */
/* lis_flushq - free messages from a queue, enabling upstream/downstream
 *	service routines if applicable. The flag is the same as for
 *	lis_flushband.
 */
void _RP
lis_flushq(queue_t *q, int flag)
{
    flush_worker(q, -1, flag & FLUSHALL) ;

}/*lis_flushq*/

/*  -------------------------------------------------------------------  */
/* putctl - allocate a message block, set the type,
 *	and put it on a queue. Returns 1 on success;
 *	0 if the allocation failed or type is one of
 *	M_DATA, M_PROTO or M_PCPROTO
 */
int _RP
lis_putctl(queue_t *q, int type, char *file_name, int line_nr)
{
    mblk_t *mp;

    if (   q == NULL
	|| type == M_DATA
	|| type == M_PROTO
	|| type == M_PCPROTO
	|| type == M_DELAY
	|| (mp = lis_allocb(0, BPRI_HI, file_name, line_nr)) == NULL
       )
    	   return 0;

    mp->b_datap->db_type = (uchar) type;
    lis_safe_putmsg(q, mp, file_name, line_nr) ;
    return 1;

}/*lis_putctl*/

int _RP lis_putnextctl(queue_t *q, int type, char *file_name, int line_nr)
{
    if (q == NULL || q->q_next == NULL)
	return(0) ;				/* no can do */

    return(lis_putctl(q->q_next, type, file_name, line_nr)) ;

} /* lis_putnextctl */

/*  -------------------------------------------------------------------  */
/* lis_putctl1 - as for lis_putctl, but with a one byte parameter
 */
int _RP
lis_putctl1(queue_t *q, int type, int param, char *file_name, int line_nr)
{
    mblk_t *mp;

    if (   q == NULL
	|| type == M_DATA
	|| type == M_PROTO
	|| type == M_PCPROTO
	|| (mp = lis_allocb(1, BPRI_HI, file_name, line_nr)) == NULL
       )
    	   return 0;

    mp->b_datap->db_type = (uchar) type;
    *mp->b_wptr++ = (uchar) param;
    lis_safe_putmsg(q, mp, file_name, line_nr) ;
    return 1;

}/*lis_putctl1*/

int _RP
lis_putnextctl1(queue_t *q, int type, int param, char *file_name, int line_nr)
{
    if (q == NULL || q->q_next == NULL)
	return(0) ;				/* no can do */

    return(lis_putctl1(q->q_next, type, param, file_name, line_nr)) ;

} /* lis_putnextctl1 */

/*  -------------------------------------------------------------------  */
/* lis_qsize - returns the number of messages on a queue
 */
int _RP
lis_qsize(queue_t *q)
{
    mblk_t *mp;
    int rtn = 0;
    lis_flags_t psw;
    if (!LIS_CHECK_Q_MAGIC(q)) return(0) ;

    LIS_RDQISRLOCK(q, &psw) ;
    for (mp = q->q_first; mp; mp = mp->b_next)
	rtn++;
    LIS_RDQISRUNLOCK(q, &psw) ;
    return rtn;
}/*lis_qsize*/

/*  -------------------------------------------------------------------  */
/* lis_strqget - get information about a (band of a) queue.
 *	Valid values for what are QHIWAT, QLOWAT, QMAXPSZ,
 *	QMINPSZ, QCOUNT, QFIRST, QLAST, QFLAG.
 *	Returns 0 on success.
 */
int  _RP
lis_strqget(queue_t *q, qfields_t what, unsigned char band, long *val)
{
    struct qband *qp ;
    lis_flags_t   psw;

    if ( val == NULL || !LIS_CHECK_Q_MAGIC(q)) return(EINVAL) ;

    LIS_RDQISRLOCK(q, &psw) ;
    if (band > 0)
    {
	qp = find_qband(q, band) ;			/* get qband struct */
	if (qp == NULL)
	{
	    LIS_RDQISRUNLOCK(q, &psw) ;
	    return(EINVAL);
	}

	switch (what)
	{
	case QHIWAT:
	    *val = qp->qb_hiwat ;
	    break ;
	case QLOWAT:
	    *val = qp->qb_lowat ;
	    break ;
	case QCOUNT:
	    *val = qp->qb_count ;
	    break ;
	case QFIRST:
	    *((mblk_t **)val) = qp->qb_first ;
	    break ;
	case QLAST:
	    *((mblk_t **)val) = qp->qb_last ;
	    break ;
	case QFLAG:
	    *val = qp->qb_flag ;
	    break ;
	default:
	case QMAXPSZ:
	case QMINPSZ:
	    LIS_RDQISRUNLOCK(q, &psw) ;
	    return(EINVAL) ;
	}

	LIS_RDQISRUNLOCK(q, &psw) ;
	return(0) ;				/* success */
    }

    switch (what)
    {
    case QHIWAT:
	*val = q->q_hiwat ;
	break ;
    case QLOWAT:
	*val = q->q_lowat ;
	break ;
    case QMAXPSZ:
	*val = q->q_maxpsz ;
	break ;
    case QMINPSZ:
	*val = q->q_minpsz ;
	break ;
    case QCOUNT:
	*val = q->q_count ;
	break ;
    case QFIRST:
	*((mblk_t **)val) = q->q_first ;
	break ;
    case QLAST:
	*((mblk_t **)val) = q->q_last ;
	break ;
    case QFLAG:
	*val = q->q_flag ;
	break ;
    default:
	LIS_RDQISRUNLOCK(q, &psw) ;
	return(EINVAL) ;
    }

    LIS_RDQISRUNLOCK(q, &psw) ;
    return(0);				/* return 0 for success */

}/*lis_strqget*/

/*  -------------------------------------------------------------------  */
/* lis_strqset - change information about a (band of a) queue.
 *	Valid values for what are QHIWAT, QLOWAT, QMAXPSZ,
 *	QMINPSZ.
 *	Returns 0 on success.
 */
int _RP
lis_strqset(queue_t *q, qfields_t what, unsigned char band, long val)
{
    struct qband *qp ;
    lis_flags_t   psw;

    if (!LIS_CHECK_Q_MAGIC(q)) return(EINVAL) ;

    LIS_QISRLOCK(q, &psw) ;
    if (band > 0)
    {
	qp = find_qband(q, band) ;			/* get qband struct */
	if (qp == NULL)
	{
	    LIS_QISRUNLOCK(q, &psw) ;
	    return(EINVAL);
	}

	switch (what)
	{
	case QHIWAT:
	    qp->qb_hiwat = val ;
	    break ;
	case QLOWAT:
	    qp->qb_lowat = val ;
	    break ;
	case QCOUNT:
	case QFIRST:
	case QLAST:
	case QFLAG:
	    LIS_QISRUNLOCK(q, &psw) ;
	    return(EPERM) ;
	default:
	case QMAXPSZ:
	case QMINPSZ:
	    LIS_QISRUNLOCK(q, &psw) ;
	    return(EINVAL) ;
	}

	LIS_QISRUNLOCK(q, &psw) ;
	return(0) ;				/* success */
    }

    switch (what)
    {
    case QHIWAT:
	q->q_hiwat = val ;
	break ;
    case QLOWAT:
	q->q_lowat = val ;
	break ;
    case QMAXPSZ:
	q->q_maxpsz = val ;
	break ;
    case QMINPSZ:
	q->q_minpsz = val ;
	break ;
    case QCOUNT:
    case QFIRST:
    case QLAST:
    case QFLAG:
	LIS_QISRUNLOCK(q, &psw) ;
	return(EPERM) ;
    default:
	LIS_QISRUNLOCK(q, &psw) ;
	return(EINVAL) ;
    }

    LIS_QISRUNLOCK(q, &psw) ;
    return(0);				/* return 0 for success */

}/*lis_strqset*/

/*  -------------------------------------------------------------------  */
/*
 * lis_set_q_sync
 *
 * Set up the queue sync structures for a queue.
 */
void lis_free_q_sync(queue_t *q)
{
    switch (q->q_qlock_option)
    {
    case LIS_QLOCK_NONE:
    case LIS_QLOCK_GLOBAL:
	break ;				/* nothing allocated */

    case LIS_QLOCK_QUEUE:
	  if (q->q_other->q_qsp != NULL)
	  {
	      lis_sem_destroy(&q->q_other->q_qsp->qs_sem) ;
	      LIS_QSYNC_FREE(q->q_other->q_qsp) ;
	  }
	  /* fall into next case */
    case LIS_QLOCK_QUEUE_PAIR:
	  if (q->q_qsp != NULL)
	  {
	      lis_sem_destroy(&q->q_qsp->qs_sem) ;
	      LIS_QSYNC_FREE(q->q_qsp) ;
	  }
	break ;
    }

    q->q_qsp = NULL ;
    q->q_other->q_qsp = NULL ;
    q->q_qlock_option = -1 ;		/* not specified */
    q->q_other->q_qlock_option = -1 ;	/* not specified */
}

int lis_set_q_sync(queue_t *q, int qlock_option)
{
    static lis_q_sync_t	qsz ;

    lis_free_q_sync(q) ;		/* clear out old option */
					/* sets q_qlock_option to -1 */
    switch (qlock_option)
    {
    case LIS_QLOCK_NONE:
	q->q_qsp = NULL ;
	q->q_other->q_qsp = NULL ;
	break ;

    case LIS_QLOCK_QUEUE:
    default:
	  qlock_option = LIS_QLOCK_QUEUE ;
	  q->q_qsp	     = LIS_QSYNC_ALLOC(sizeof(lis_q_sync_t), "Qsync") ;
	  q->q_other->q_qsp  = LIS_QSYNC_ALLOC(sizeof(lis_q_sync_t), "Qsync") ;
	  if (q->q_qsp == NULL || q->q_other->q_qsp == NULL)
	  {
	      if (q->q_qsp != NULL)
		  LIS_QSYNC_FREE(q->q_qsp) ;
	      if (q->q_other->q_qsp != NULL)
		  LIS_QSYNC_FREE(q->q_other->q_qsp) ;

	      q->q_qsp = NULL ;
	      q->q_other->q_qsp = NULL ;
	      return(-ENOMEM) ;
	  }
	  *q->q_qsp	     = qsz ;
	  *q->q_other->q_qsp = qsz ;
	  lis_sem_init(&q->q_qsp->qs_sem, 1) ;
	  lis_sem_init(&q->q_other->q_qsp->qs_sem, 1) ;
	break ;

    case LIS_QLOCK_QUEUE_PAIR:
	  q->q_qsp	     = LIS_QSYNC_ALLOC(sizeof(lis_q_sync_t), "Qsync") ;
	  if (q->q_qsp == NULL)
	      return(-ENOMEM) ;
	  q->q_other->q_qsp  = q->q_qsp ;
	  *q->q_qsp	     = qsz ;
	  lis_sem_init(&q->q_qsp->qs_sem, 1) ;
	break ;

    case LIS_QLOCK_GLOBAL:
	  q->q_qsp = &lis_queue_sync ;
	  q->q_other->q_qsp = &lis_queue_sync ;
	break ;
    }

    q->q_qlock_option = qlock_option ;
    q->q_other->q_qlock_option = qlock_option ;
    return(0) ;
}

/*  -------------------------------------------------------------------  */
/* Allocate a new NULL-initilized queue pair
 * return NULL on failure
 */
queue_t *
lis_allocq( const char *name )
{
  queue_t *q = (queue_t*)LIS_QUEUE_ALLOC(sizeof(queue_t)*2,"Queue");

  if (q == NULL) return(NULL) ;		/* memset does not null-check */

  LisUpCount(QUEUES) ;			/* one more queue */
  memset((void*)q,0,2*sizeof(queue_t));
  lis_mark_mem(q, name, MEM_QUEUE) ;	/* mark memory w/queue name */
  q->q_other = (q+1);                   /* set the q_other link */
  q->q_other->q_other = q;
  q->q_magic = q->q_other->q_magic = Q_MAGIC;
  q->q_flag = QREADR | QUSE ;		/* read side of queue */
  q->q_other->q_flag = QUSE ;		/* queue in use */

  if (lis_set_q_sync(q, LIS_QLOCK_QUEUE) < 0)	/* default */
  {
    lis_freeq(q) ;
    return(NULL) ;
  }

  lis_rw_lock_init(&q->q_isr_lock, name) ;
  lis_rw_lock_init(&q->q_other->q_isr_lock, name) ;

  return(q);

}/*lis_allocq*/

/*  -------------------------------------------------------------------  */
/* Deallocate a queue pair.  Presumably locking is not important
 * at this point.
 */
void
lis_freeq( queue_t *q )
{
    struct qband *qp ;
    struct qband *qpx ;
    queue_t	 *rq ;
    queue_t	 *wq ;
    int i ;

    lis_free_q_sync(q) ;

    rq = q = LIS_RD(q) ;
    wq = LIS_WR(q) ;
    for (i = 1; i <= 2; i++, q = wq)
    {
	if (!LIS_CHECK_Q_MAGIC(q)) continue ;

	purge_queue_contention(q) ;
	q->q_magic = Q_MAGIC ^ 1;
	for (qp = q->q_bandp; qp != NULL; )
	{
	    qpx = qp->qb_next ;
	    LIS_QBAND_FREE(qp);
	    qp = qpx ;
	}
    }

    LIS_QUEUE_FREE(rq);
    LisDownCount(QUEUES) ;			/* one fewer queue */

}/*lis_freeq*/

/*  -------------------------------------------------------------------  */
/* Insert mp2 after mp1
 * This is the same as inserting mp2 just in front of the next
 * message after mp1.
 */
void _RP
lis_appq(queue_t *q, mblk_t *mp1, mblk_t *mp2)
{
    if (mp1 == NULL || mp2 == NULL )
      return;

    if (!LIS_CHECK_Q_MAGIC(q))
    {
	freemsg(mp2) ;
	return ;
    }

    if (!lis_insq(q, mp1->b_next, mp2)) freemsg(mp2) ;

}/*lis_appq*/


/*  -------------------------------------------------------------------  */
/* lis_bcanput - search the stream starting from q until a service
 *	routine is found, if any, and test the found queue for
 *	flow control at a specified band. Schedule backenabling
 *	if flow controlled.
 *	For band==0 this is equivalent to canput. Returns 0 if
 *	flow controlled; 1 otherwise.
 */
int  _RP
lis_bcanput(queue_t *q, unsigned char band)
{
    lis_flags_t	  psw;
    struct qband *qp ;
    queue_t	 *oq = q;

    if (!LIS_CHECK_Q_MAGIC(q)) return(0) ;

    LIS_RDQISRLOCK(oq, &psw) ;

    /* search the stream for the next module with a service
     * procedure, and check its high water mark.  If no queue
     * has a service procedure then end with the queue at the
     * end of the stream.
     */
    while (   q->q_next != NULL
	   && (q->q_qinfo == NULL || q->q_qinfo->qi_srvp == NULL)
          )
	    q = q->q_next;

    if (q != oq)
    {
	LIS_RDQISRUNLOCK(oq, &psw) ;
	LIS_RDQISRLOCK(q, &psw);
    }

    /*
     * 'q' is now read-locked.  'oq' is unlocked if different from 'q'.
     * From here on, we just work with 'q'.
     */

    /* if the queue is flow controlled, set the QWANTW flag */
    if (band > 0)
    {
	qp = find_qband(q, band) ;			/* get qband struct */
	if (qp == NULL)
	{
	    LIS_RDQISRUNLOCK (q, &psw);
return_failure:
	    LisUpFailCount(CANPUTS) ;
	    return(0);
	}
	if (qp->qb_flag & QB_FULL)
	{
	    LIS_RDQISRUNLOCK (q, &psw);
	    LIS_QISRLOCK (q, &psw);
	    qp->qb_flag |= QB_WANTW ;
	    LIS_QISRUNLOCK (q, &psw);
	    goto return_failure ;
	}
    }
    else
    if (q->q_flag & QFULL)
    {
	LIS_RDQISRUNLOCK (q, &psw);
	LIS_QISRLOCK (q, &psw);
	q->q_flag |= QWANTW;
	LIS_QISRUNLOCK (q, &psw);
	goto return_failure ;
    }

    LisUpCount(CANPUTS) ;
    LIS_RDQISRUNLOCK (q, &psw);
    return 1;

}/*lis_bcanput*/

/*  -------------------------------------------------------------------  */
/* lis_bcanputnext - search the stream starting from the queue after q
 * 	until a service	routine is found.  Uses lis_bcanput().
 *
 *	This routine shows up in AT&T's MP spec for SVR4.
 */
int  _RP
lis_bcanputnext(queue_t *q, unsigned char band)
{
    if (!LIS_CHECK_Q_MAGIC(q) || 
	!LIS_CHECK_Q_MAGIC(q->q_next)) return(0);

    return(lis_bcanput(q->q_next, band)) ; /* check the next queue */

}/*lis_bcanputnext*/

/*  -------------------------------------------------------------------  */
/* lis_bcanputnext_anyband - search the stream starting from q->q_next
 *	until a service	routine is found, if any, and test the found queue
 *	for flow control on any non-zero band.  This is for the S_WRBAND
 *	option processing.
 *
 *	Note, in particular, that we do not return the band number of
 *	the band with available space in it.  This actually sounds
 *	pretty useless to me.  -- DMG
 */
int  _RP
lis_bcanputnext_anyband(queue_t *q)
{
    lis_flags_t   psw;
    struct qband *qp ;
    queue_t	 *oq ;			/* original (next) queue */

    if (   !LIS_CHECK_Q_MAGIC(q)
	|| (q = q->q_next) == NULL
	|| !LIS_CHECK_Q_MAGIC(q)
       )
	return 0;			/* sanity check */


    oq = q ;
    LIS_RDQISRLOCK(oq, &psw) ;

    /* search the stream for the next module with a service procedure */
    while (   q->q_next != NULL
	   && (q->q_qinfo == NULL || q->q_qinfo->qi_srvp == NULL)
	  )
	    q = q->q_next;

    /* Find any qband that has room in it */
    /* We even accept non-in-use qband structures, is that correct?  */
    for (qp = q->q_bandp; qp != NULL; qp = qp->qb_next)
    {
	if (!(qp->qb_flag & QB_FULL))
	    break ;
    }

    if (qp == NULL)			/* no bands with room */
    {
        LisUpFailCount(CANPUTS) ;
	LIS_RDQISRUNLOCK(oq, &psw) ;
	return(0);
    }

    LisUpCount(CANPUTS) ;
    LIS_RDQISRUNLOCK(oq, &psw) ;
    return 1;

}/*lis_bcanputnext_anyband*/

/*  -------------------------------------------------------------------  */
/*  lis_qcountstrm
 *
 * Return the q_qcount field from the next queue.
 */
int	_RP lis_qcountstrm(queue_t *q)
{
    lis_flags_t  psw;
    int		 nbytes = 0 ;
    queue_t	*oq = q ;

    if (!LIS_CHECK_Q_MAGIC(q)) return(0) ;

    LIS_RDQISRLOCK(oq, &psw) ;

    nbytes += q->q_count ;

    while (SAMESTR(q))
    {
	q = q->q_next ;
	if (!LIS_CHECK_Q_MAGIC(q)) break ;
	nbytes += q->q_count ;
    }

    LIS_RDQISRUNLOCK(oq, &psw) ;
    return(nbytes) ;

} /* lis_qcountstrm */

/*  -------------------------------------------------------------------  */
/*  lis_qprocson
 *
 * Allow q put/svc procedures to be called.
 */
void	_RP lis_qprocson(queue_t *rdq)
{
    lis_clr_q_flags((QPROCSOFF | QENAB), 1, rdq, NULL);
    lis_do_deferred_puts(rdq) ;		/* if any deferred */
    lis_do_deferred_puts(OTHER(rdq)) ;
    lis_retry_qenable(rdq) ;		/* see if qsched required */
    lis_retry_qenable(OTHER(rdq)) ;		/* see if qsched required */
}

/*  -------------------------------------------------------------------  */
/*  lis_qprocsoff
 *
 * Prevent q put/svc procedures from being called.
 */
void	_RP lis_qprocsoff(queue_t *rdq)
{
    lis_set_q_flags(QPROCSOFF, 1, rdq, NULL);
}

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
