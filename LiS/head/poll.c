/************************************************************************
*                      poll() System Call				*
*************************************************************************
*									*
* This file does the hard work for the poll() system call.  It handles	*
* STREAMS files using the AT&T poll mechanism and non-streams files	*
* by using their select functions.					*
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
#ident "@(#) LiS poll.c 2.12 09/07/04 11:11:22 "

#include <sys/stream.h>
#ifdef LINUX_POLL
#define	USE_LINUX_POLL_H	1
#include <linux/poll.h>
#else
#include <sys/poll.h>
#endif
#include <sys/lislocks.h>
#include <sys/osif.h>

char	*lis_poll_file =
#if defined(USED_LINUX_POLL_H)
		    "<linux/poll.h>"
#elif defined(USED_LIS_POLL_H)
		    "<LiS/include/sys/poll.h>"
#else
		    "<unknown/poll.h>"
#endif
;

/* typedef struct semaphore	semaphore_t ; */
typedef struct file		file_t ;

long		lis_poll_sleeping ;	/* # times slept > twice on same poll */

#ifdef POLL_TRACE

typedef struct
{
    int		time_out ;
    long	target_time ;
    long	jiffies ;
    long	time_interval ;
    long	ticks ;

} poll_trace_t ;

poll_trace_t		lis_poll_trace[5] ;

#endif

#ifdef PORTABLE_POLL
/*
 * Routines elsewhere in the Linux kernel
 */
extern void	select_free_wait(select_table *p) ;
extern int	select_check(int flag, select_table *wait, struct file *file) ;
#endif

/*
 * Routines elsewhere in STREAMS
 */
extern char	*lis_poll_events(short events) _RP ;
extern unsigned	 lis_poll_bits(stdata_t *hd) ;


/************************************************************************
*                            lis_poll_wakeup                            *
*************************************************************************
*									*
* Called via the polldat structure to wakeup sleeping poll routine.	*
*									*
************************************************************************/
#ifdef PORTABLE_POLL
void	lis_poll_wakeup(long arg)
{
    struct wait_queue	**poll_wait_addr_ptr = (struct wait_queue **) arg ;

    wake_up(poll_wait_addr_ptr) ;

} /* lis_poll_wakeup */
#endif

/************************************************************************
*                            poll_do_select                             *
*************************************************************************
*									*
* This routine checks the file descriptor using the select mechanism.	*
*									*
* We map the different combinations of poll conditions down to the three*
* select conditions and use the Linux select mechanism to test the	*
* file for these conditions.						*
*									*
************************************************************************/
#ifdef PORTABLE_POLL
int poll_do_select(select_table *wait, file_t *fp, polldat_t *pdat_ptr)
{
    short	events = pdat_ptr->pd_events ;	/* poll events */
    short	revents = 0 ;
    short	msk ;

    if (   (msk = (events & (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND))) != 0
	&& select_check(SEL_IN, wait, fp) != 0
       )
    {
	revents |= msk ;
    }

    if (   (msk = (events & (POLLOUT | POLLWRNORM | POLLWRBAND))) != 0
	&& select_check(SEL_OUT, wait, fp) != 0
       )
    {
	revents |= msk ;
    }

    if (   (msk = (events & (POLLMSG | POLLNVAL | POLLERR | POLLHUP))) != 0
	&& select_check(SEL_EX, wait, fp) != 0
       )
    {
	revents |= msk ;
    }

    if ( LIS_DEBUG_POLL )
    {
	struct inode	*i ;
	kdev_t		 rdev ;

	i = fp->f_inode ;
	if (i != NULL)
	    rdev = i->i_rdev ;
	else
	    rdev = 0 ;

	printk("poll_do_select:  dev=(%d,%d) events:%s revents:%s\n",
	      MAJOR(rdev), MINOR(rdev),
	      lis_poll_events((short)events), lis_poll_events((short)revents)) ;
    }

    return(revents) ;

} /* poll_do_select */
#endif

/************************************************************************
*                             lis_poll                                  *
*************************************************************************
*									*
* Poll file descriptors for operations.					*
*									*
************************************************************************/
#ifdef PORTABLE_POLL
int	lis_poll(struct pollfd *fds, int nfds, int time_out)
{
    volatile int	 err;
    unsigned long	 size = nfds * sizeof(pollfd_t);
    long		 target_time ;			/* in ms */
    int			 timer_id = 0 ;
    long		 time_interval = time_out ;	/* in ms */
    long		 ms_per_tick = 1000/HZ ;
    long		 ticks ;			/* in system ticks */
    struct pollfd	*pfds;
    struct pollfd	*pfd_ptr ;
    polldat_t		*pdat_head;
    polldat_t		*pdat_ptr ;
    file_t		*fp ;
    int			 i;
    int			 done ;
    int			 ready_fd_cnt = 0 ;
    int			 psw ;
    int			 sleep_cnt ;
    struct wait_queue	 poll_wait = {current, NULL} ;
    struct wait_queue   *poll_wait_address ;
    select_table	 poll_select_wait_table ;
    select_table	*poll_select_wait = &poll_select_wait_table ;
    struct select_table_entry	*poll_select_table ;
    int			 select_table_size ;
    int			 is_stream = 0 ;
    extern struct file_operations lis_streams_fops ;

    /*
     * Round time interval up to a multiple of system ticks.
     */
    ticks = (time_interval + ms_per_tick - 1) / ms_per_tick ;
    target_time = lis_target_time(ticks*ms_per_tick) ;

    if (   (err=lis_check_umem(NULL,VERIFY_WRITE,fds,size))<0
	|| (err=lis_check_umem(NULL,VERIFY_READ,fds,size))<0
       )
	return(err);

    if ((pfds = ALLOCF(size,"poll-fd ")) == NULL) 
	return(-ENOMEM);

    if ((pdat_head = ALLOCF(nfds * sizeof(polldat_t), "poll-data ")) == NULL)
    {
	FREE(pfds);
	return(-ENOMEM);
    }

    select_table_size = nfds * 3 * sizeof(struct select_table_entry) ;

    if(!(poll_select_table = (struct select_table_entry*) __get_free_page(GFP_KERNEL)))
    {
	FREE(pfds);
	FREE(pdat_head);
	return -ENOMEM;
    }

    memset(pdat_head, 0, nfds * sizeof(polldat_t)) ;	/* clear to zero */
    memset(poll_select_table, 0, 4096) ;	/* clear to zero */
    poll_select_wait_table.nr = 0 ;
    poll_select_wait_table.entry = poll_select_table ;

    /*
     * Obtain copy of user's poll descriptor list
     */
    if ((err = lis_copyin(NULL,pfds, fds, size)) < 0)
    {
	FREE(pfds);
	FREE(pdat_head);
	return(err);
    }

    /*
     * Initialize our wait address pointer
     */
    init_waitqueue(&poll_wait_address) ;

    /*
     * Run through the descriptor list and check each file descriptor
     * for validity.  Mark the ones that are bad.
     */
    for (i = 0, pdat_ptr = pdat_head, pfd_ptr = pfds;
	 i < nfds;
	 i++, pdat_ptr++, pfd_ptr++
	)
    {
	if (   pfd_ptr->fd>=NR_OPEN
	    || (fp=current->files->fd[pfd_ptr->fd]) == NULL
	    || fp->f_inode == NULL
	   )
	{				/* not an open file descriptor */
	    pfd_ptr->revents = POLLNVAL ;
	    continue ;
	}

	pfd_ptr->revents    = 0 ;	/* ensure POLLNVAL not set */
	pdat_ptr->pd_fn	    = lis_poll_wakeup ;
	pdat_ptr->pd_arg    = (long) &poll_wait_address ;
    }

    /*
     * Loop until we find a valid file descriptor which statisfies the
     * required conditions.  If there were no good file descriptors
     * then just fall through and return to the user.
     *
     * If no file descriptors are found then wait.  When something
     * happens on one of the streams then the lis_poll_wakeup function
     * will get called, which will wake us up.  We then go around and
     * poll again.
     *
     * When we exit the loop we will unlink the polldat entries and
     * return to the user.
     *
     * The file system poll function is called with the file info and
     * a pointer to a polldat structure.  The file system routine
     * interrogates the file for the requested conditions according
     * to the 'pd_events' field in the polldat structure.  It returns
     * a bit-mask of the events that satisfy the conditions, if any.
     * The file system also sets the pd_headp pointer to point to
     * its list head in its stream structure for the polldat list.
     * We take care of linking the structure into the list.
     *
     */
    add_wait_queue(&poll_wait_address, &poll_wait) ;
    for (done = 0, sleep_cnt = 0; !done; )
    {
	current->state = TASK_INTERRUPTIBLE ;
	for (i = 0, ready_fd_cnt = 0, pdat_ptr = pdat_head, pfd_ptr = pfds;
	     i < nfds;
	     i++, pdat_ptr++, pfd_ptr++
	    )
	{
	    if (pfd_ptr->fd < 0 || pfd_ptr->revents & POLLNVAL)
		continue ;			/* only look at good ones */

	    pdat_ptr->pd_events = pfd_ptr->events ;
	    fp = current->files->fd[pfd_ptr->fd] ;
/* FIXME - use a test not dependent on lis_streams_fops */
	    is_stream = fp->f_op == &lis_streams_fops ;
/**/
	    if (is_stream)
	    {
		if ( (pfd_ptr->revents =
			(short) lis_strpoll(fp->f_inode, fp, pdat_ptr)) != 0 )
		{
		    done = 1 ;			/* loop breaker */
		    ready_fd_cnt++ ;		/* another ready descriptor */
		    continue ;			/* skip list manipulations */
		}
	    }
	    else				/* non-stream file */
	    {					/* use select */
		if ( (pfd_ptr->revents =
		       poll_do_select(poll_select_wait, fp, pdat_ptr)) != 0
		   )
		{
		    done = 1 ;			/* loop breaker */
		    ready_fd_cnt++ ;		/* another ready descriptor */
		    poll_select_wait = NULL ;	/* no more waiting */
		    continue ;			/* skip list manipulations */
		}
	    }

	    if (ready_fd_cnt)			/* skip list manipulations */
		continue ;			/* if have ready fds */

	    if (   !is_stream
		|| pdat_ptr->pd_headp == NULL
	        || pdat_ptr->pd_headp->ph_list == pdat_ptr
	        || pdat_ptr->pd_next != NULL
	        || pdat_ptr->pd_prev != NULL
	       )
		   continue ;		/* no list or already in it */

	    lis_spin_lock_irqsave(&pdat_ptr->pd_headp->sd_lock, &psw) ;

	    if (pdat_ptr->pd_headp->ph_list == NULL)
	    {				/* first entry */
		pdat_ptr->pd_next =
		pdat_ptr->pd_prev = NULL ;
		pdat_ptr->pd_headp->ph_list = pdat_ptr ;
	    }
	    else			/* multiple entries */
	    {
		pdat_ptr->pd_next = pdat_ptr->pd_headp->ph_list ;
		pdat_ptr->pd_prev = NULL ;

		pdat_ptr->pd_headp->ph_list->pd_prev = pdat_ptr ;
		pdat_ptr->pd_headp->ph_list	     = pdat_ptr ;
	    }

	    lis_spin_unlock_irqrestore(&pdat_ptr->pd_headp->sd_lock, &psw) ;
	}

	if (done)			/* done */
	    break ;			/* exit while loop */

	/*
	 * If signalled then return to caller
	 */
	if (current->signal & ~current->blocked)
	{
	    err = -EINTR ;
	    break ;
	}

	/*
	 * We recompute the interval until the original time out
	 * since we may wake up and go around several times before
	 * we finally decide to return.
	 *
	 * A timeout value of -1 means wait forever.
	 */
	if (time_out != -1)
	{
	    if ( (time_interval = lis_time_till(target_time)) <= 0 )
		break ;			/* time's up */

	    ticks = (time_interval + ms_per_tick - 1) / ms_per_tick ;
	    timer_id = timeout((timo_fcn_t *) lis_poll_wakeup,
			       (caddr_t) &poll_wait_address,
			       ticks) ;
	}

#ifdef POLL_TRACE
	if (sleep_cnt < sizeof(lis_poll_trace)/sizeof(lis_poll_trace[0]))
	{
	    lis_poll_trace[sleep_cnt].time_out = time_out ;
	    lis_poll_trace[sleep_cnt].target_time = target_time ;
	    lis_poll_trace[sleep_cnt].jiffies = jiffies ;
	    lis_poll_trace[sleep_cnt].time_interval = time_interval ;
	    lis_poll_trace[sleep_cnt].ticks = ticks ;
	}
#endif

	if (++sleep_cnt > 2)
	    lis_poll_sleeping++ ;

	schedule() ;

	if (timer_id != 0)
	    untimeout(timer_id) ;	/* cancel timer */
    }

    current->state = TASK_RUNNING ;
    remove_wait_queue(&poll_wait_address, &poll_wait) ;

    /*
     * Go through and unlink all the polldat structures
     */
    for (i = 0, pdat_ptr = pdat_head;
	 i < nfds;
	 i++, pdat_ptr++
	)
    {
	if (pdat_ptr->pd_headp != NULL)	/* have list head specified */
	{
	    lis_spin_lock_irqsave(&pdat_ptr->pd_headp->sd_lock, &psw) ;

	    if (pdat_ptr->pd_headp->ph_list == pdat_ptr)	/* at head */
		pdat_ptr->pd_headp->ph_list = pdat_ptr->pd_next ;

	    if (pdat_ptr->pd_next)	/* next element exists */
		pdat_ptr->pd_next->pd_prev = pdat_ptr->pd_prev ;

	    if (pdat_ptr->pd_prev)	/* previous element exists */
		pdat_ptr->pd_prev->pd_next = pdat_ptr->pd_next ;

	    lis_spin_unlock_irqrestore(&pdat_ptr->pd_headp->sd_lock, &psw) ;
	}
    }

    FREE(pdat_head);			/* done with polldat structures */

    select_free_wait(&poll_select_wait_table) ;	/* undo wait q entries */

    free_page((unsigned long) poll_select_table); /* done with select table */

    if (err == 0)
	err = lis_copyout(NULL,pfds, fds, size);  /* update user's poll list */

    FREE(pfds) ;			/* done with fd list */

    if (err < 0)
	return(err) ;			/* error return from semaphore */
    else
	return(ready_fd_cnt) ;		/* return # of ready fds */

} /* lis_poll */
#endif

/************************************************************************
*                           lis_poll_2_1                                *
*************************************************************************
*									*
* This is the kernel version 2.1 poll routine.  It is pointed to by	*
* the fops structure for STREAMS.  It handles polling for one stream.	*
* It builds a list of processes waiting on the stream in the task list	*
* owned in the stdata structure.					*
*									*
* If we have to wait then lis_wake_up_poll in head.c does the wakeup.	*
*									*
************************************************************************/
#ifdef LINUX_POLL
unsigned lis_poll_2_1(struct file *fp, poll_table *wait)
{
    stdata_t	*head ;

    if (fp == NULL)
	return(POLLNVAL) ;

    head = FILE_STR(fp) ;
    if (head->magic != STDATA_MAGIC)
    {
	printk("lis_poll_2_1: fp=%p wait=%p head=%p magic=%lx should be %lx\n",
		fp, wait, head, head->magic, STDATA_MAGIC) ;
	return(POLLNVAL) ;
    }

    /*
     * Put us into the wait queue first.  If any files have returned
     * non-zero then Linux stops the wait queue insertion.  Linux poll
     * system call cleans up the wait queues after all the poll operations
     * are done.
     */
    poll_wait(fp, &head->sd_task_list, wait) ;
    return(lis_poll_bits(head)) ;

} /* lis_poll_2_1 */
#endif
