/************************************************************************
*                  Portable Machine Dependent Code                      *
*************************************************************************
*									*
* This file contains routines for machine (or O/S) dependent		*
* functions for the portable version of the streams code.		*
*									*
* This file contains some routines that will port to any environment	*
* and includes a second .c file which will contains routines that are	*
* unique to a particular environment.					*
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

#ident "@(#) LiS port-mdep.c 2.20 09/02/04 14:11:17 "

#include <sys/stream.h>
#include <sys/LiS/errmsg.h>

#include <sys/LiS/usrio.h>

extern inode_t	*user_inode_of_file(int fd) ;	/* in usrio.c */
extern inode_t	*user_inode(dev_t dev) ;
extern file_t	*user_file_of_fd(int fd) ;

extern int  user_sendfd(struct stdata *, unsigned int, struct file *);
extern int  user_recvfd(struct stdata *, struct strrecvfd *,struct file *);
extern void user_free_passfp(mblk_t *);
extern void user_new_stream_name(struct stdata *hd, struct file *f);

#define _return(e) return ((e)<0?-(errno=-(e)):(e))

/************************************************************************
*                         Global Variables                              *
************************************************************************/

unsigned long		port_clock ;	/* milli-second clock */
lis_spin_lock_t		port_timer_lock ;

/*
 * A few things just to satisfy the linker.  Not needed in "port" version.
 */
lis_atomic_t	lis_inode_cnt ;
lis_atomic_t	lis_mnt_cnt ;

/************************************************************************
*                           port_init                                   *
*************************************************************************
*									*
* This routine is called from outside to initialize the STREAMS system.	*
*									*
************************************************************************/
void	port_init(void)
{
    extern void		lis_init_head(void) ;

    lis_spin_lock_init(&port_timer_lock, "Timer-Lock") ;
    lis_init_head() ;

} /* port_init */

/************************************************************************
*                         port_copyin_str                               *
*************************************************************************
*									*
* Portable routine to copy a string in from user space to kernel.	*
*									*
* ustr		pointer to string in user space.			*
* kstr		pointer to a pointer that will point to kernel memory	*
*		where the string is copied.  We allocate this memory	*
*		and the caller must free it using the FREE macro.	*
* max		Max length of the string.				*
*									*
* Return:	< 0 for error, caller does not free memory.		*
*		0   for success, caller must free memory.		*
*									*
************************************************************************/
int 
port_copyin_str(struct file *f, const char *ustr, char **kstr, int maxb)
{
	int    i = maxb;
	int    error;
	char   c;
	char  *tmp;
	char  *mem ;

	if (maxb <= 0)
	    return(-ENOMEM);

	error = -EFAULT;
	c = (char) port_get_fs_byte(f, ustr++);
	if (!c)
	    return(-EINVAL);

	if ((mem = ALLOCF(maxb,"copyin-buf ")) == NULL)
	    return(-ENOMEM);

	*kstr = tmp = mem ;
	while (--i)
	{
	    *(tmp++) = c;
	    c = (char) port_get_fs_byte(f, ustr++);
	    if (!c)
	    {
		*tmp = '\0';
		return(0);
	    }
	}

	FREE(mem);
	return(error);

} /* port_copyin_str */


/************************************************************************
*                         Timer Mechanism                               *
*************************************************************************
*									*
* This code is borrowed from the Linux kernel and reworked so that	*
* it maintains time deltas rather than absolute time.  See the comments	*
* in the individual routines below.					*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
************************************************************************/

static struct timer_list timer_head =
			 { &timer_head, &timer_head, 0, 0, NULL };

/*
 * Add a timer list entry to the list of entries, this effectively
 * starts the timer.  The list is kept in ascending order of number
 * of ticks until the timer expires.  As you go down the list the
 * 'tdelta' fields are cumulative.  Each one specifies the additional
 * amount of time after the previous entry would expire.
 */
void port_add_timer(struct timer_list * timer)
{
    lis_flags_t		 flags;
    struct timer_list	*p;

    if (timer->next != NULL || timer->prev != NULL)
    {
	printk("add_timer(%lx) called with non-zero list\n", timer) ;
	return;
    }

    p = &timer_head;
    lis_spin_lock_irqsave(&port_timer_lock, &flags) ;
    do
    {
	timer->tdelta -= p->tdelta ;	/* reduce ticks difference */
	p = p->next;

    } while (p != &timer_head && timer->tdelta > p->tdelta);

    if (p != &timer_head)		/* real entry */
	p->tdelta -= timer->tdelta;	/* less delta to get to the next one */

    timer->next = p;			/* put timer before 'p' */
    timer->prev = p->prev;
    p->prev = timer;
    timer->prev->next = timer;

    lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;

} /* port_add_timer */

/*
 * Find the timer-list entry and remove it from the linked list
 */
int port_del_timer(struct timer_list * timer)
{
    lis_flags_t		 flags;
    struct timer_list	*p;

    p = &timer_head;
    lis_spin_lock_irqsave(&port_timer_lock, &flags) ;
    while ((p = p->next) != &timer_head)
    {
	if (p == timer)
	{
	    if (p->next != &timer_head)		/* real timer entry follows */
	    	p->next->tdelta += timer->tdelta ;	/* add back our time */

	    timer->next->prev = timer->prev;	/* delink our entry */
	    timer->prev->next = timer->next;
	    timer->next = timer->prev = NULL;

	    /*
	     * Clear any residual ticks saved up by the
	     * announce time routine if the timer list is
	     * now empty.
	     */
	    if (timer_head.next == &timer_head)
		port_announce_time(0) ;

	    lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;
	    return 1;
	}
    }

    lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;

    if (timer->next || timer->prev)
	    printk("del_timer(%lx) called from with timer not found in table\n",
		    timer) ;

    return 0;

} /* port_del_timer */

/*
 * The porting environment must call this routine periodically to
 * input time into the timeout mechanism.  This routine can be called
 * from interrupt level or from background.  The entire routine runs
 * with interrupts disabled.
 *
 * Calls to user time out routines will be made from here with the
 * processor status restored to that of caller.  If you call this
 * routine from interrupt level then all the timer callout routines
 * will be called with interrupts disabled.  If you call it from
 * background then they will be run with interrupts enabled.
 */
void port_announce_time(long new_milli_secs)
{
    static unsigned long milli_secs ;	/* time interval saved up */
    static int		 entered ;
    lis_flags_t		 flags;
    struct timer_list	*timer;

    lis_spin_lock_irqsave(&port_timer_lock, &flags) ;

    milli_secs += new_milli_secs ;	/* add to total nr of milli-secs */
    port_clock += new_milli_secs ;	/* keep global elapsed time */

    if (timer_head.next == &timer_head)	/* list empty */
    {
	milli_secs = 0 ;		/* do not accumulate time */
	lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;
	return ;			/* nested call */
    }

    if (entered)			/* nested entry */
    {
	lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;
	return ;			/* nested call */
    }

    entered = 1 ;			/* prevent nesting */

    /*
     * Because the table is ordered by time deltas, it is always the
     * ones at the head of the list that need to be sprung.  Whenever
     * we spring one we decrement that time's tdelta from our number
     * of milliseconds that have passed so we can meaningfully compare
     * against the next entry.
     */
    while (   (timer = timer_head.next) != &timer_head	/* list non-empty */
	   && timer->tdelta <= milli_secs		/* 1st elt expired */
	  )
    {
	void		       (*fn)(unsigned long) = timer->function;
	unsigned long     	 data = timer->data;

	milli_secs 	     -= timer->tdelta ;	/* less time to nxt ntry */

	timer->next->prev     = timer->prev;	/* delink timer entry */
	timer->prev->next     = timer->next;
	timer->next	      =			/* clear links */
	timer->prev	      = NULL;

	lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;
	if (fn != NULL) fn(data);		/* be careful of NULL ptr */
	lis_spin_lock_irqsave(&port_timer_lock, &flags) ;
    }

    entered = 0 ;				/* allow calls again */

    lis_spin_unlock_irqrestore(&port_timer_lock, &flags) ;
	
} /* port_announce_time */

/*
 * This routine is called from dki.c to start a timer. 'ticks' is
 * in units of HZ (typically 1/100th of a second, or 10ms).
 */
void
lis_tmout(struct timer_list *tl, void (*fn)(ulong), long arg, long ticks)
{
    tl->next	= NULL ;
    tl->prev	= NULL ;
    tl->function= fn;
    tl->data	= arg;
    tl->tdelta	= ticks * (1000/HZ) ;		/* convert to milli-secs */
    port_add_timer(tl);
}

/*
 * This routine is called from dki.c to stop a timer.
 */
void
lis_untmout(struct timer_list *tl)
{
    port_del_timer(tl);
}

/*
 * Initialization and allocation of timers
 */

int lis_timer_size ;

void lis_init_timers(int size)
{
    lis_timer_size = size ;
}

void lis_terminate_timers(void)
{
}

void *lis_alloc_timer(char *file, int line)
{
    void        *t ;

    t = LISALLOC(lis_timer_size,file, line) ;
    return(t) ;
}

void *lis_free_timer(void *timerp)
{
    FREE(timerp) ;
    return(NULL) ;
}


/************************************************************************
*                            port_time_till                             *
*************************************************************************
*									*
* Given a target time in terms of elapsed milli-seconds, in other words,*
* the same units as the global port_clock, return the number of		*
* milli-seconds that it will take to reach to target time.		*
*									*
* The return is negative if the target time is in the past, zero	*
* if it is the same as the current target time, and positive if		*
* target time is in the future.						*
*									*
************************************************************************/
long	port_time_till(long target_time)
{
    return( target_time - port_clock ) ;

} /* port_time_till */

/************************************************************************
*                           port_target_time                            *
*************************************************************************
*									*
* Convert the milli_sec interval to an absolute target time expressed	*
* in the same manner as the global port_clock.				*
*									*
************************************************************************/
long	port_target_time(long milli_sec)
{
    return( port_clock + milli_sec ) ;

} /* port_target_time */

/************************************************************************
*                           port_milli_to_ticks                         *
*************************************************************************
*									*
* "Convert" milli-seconds to ticks -- our clock is in milli-seconds.	
*									*
************************************************************************/
long	port_milli_to_ticks(long milli_sec)
{
    return(milli_sec) ;
}

/************************************************************************/

/*
 *  port_grab_inode() - bump inode's ref count
 */
struct inode *port_grab_inode( struct inode *i )
{
    (i->i_use)++;
    return i;
}

/*
 * port_set_up_inode - perform basic inode setup for LiS
 */
struct inode *port_set_up_inode(struct file *f, struct inode *inode)
{
    return(inode) ;
}

/*
 *  port_put_inode() - decrement inode's ref count, and free if
 *  no longer in use
 */
void port_put_inode( struct inode *i )
{
    if (--(i->i_use) <= 0) {
	i->i_str = NULL;
	lis_mark_mem(i, "unused inode: ", major(i->i_dev));
    }
}

/*
 * port_is_stream_inode
 */
int port_is_stream_inode(struct inode *i)
{
    return(1) ;
}

/*
 * port_new_inode() - replace f's inode with a new one
 */
struct inode *port_new_inode(struct file *fp, dev_t dev)
{
    struct inode *old;
    dir_t	*dp ;
    struct inode *new ;
    char	 name_buf[MAXNAMLEN];
    int		 rslt ;

    old = fp->f_inode;
    
    if (old == NULL)
	return(NULL) ;

    if (old->i_dev == dev)
	return(old) ;				/* use old one */

    new = user_inode(dev) ;			/* ptr to inode */
    if (new != NULL)				/* got a new one */
    {						/* allocate stores dev */
	ULOCK_INO(new) ;
	new->i_str   = old->i_str ;		/* xfer stream ptr */
	new->i_mode  = old->i_mode ;		/* inherit mode */
	new->i_ftime = old->i_ftime;		/* inherit times */
	new->i_ctime = old->i_ctime;
	new->i_atime = old->i_atime;
	new->i_mtime = old->i_mtime;
    }

    /*
     * Free old inode and use the new one.  Ensure that the
     * new one has a directory name of some sort.
     */
    if (fp != NULL)
    {						/* needs a name */
	fp->f_inode = new ;			/* hook to new inode */
	dp = user_search_dir_inode(new) ;
	if (dp != NULL)
	    strncpy(name_buf, dp->d_name, sizeof(name_buf)) ;
	else
	    sprintf(name_buf, "%s%s",
		    lis_strm_name(fp->f_drvr_ptr),
		    lis_maj_min_name(fp->f_drvr_ptr)) ;

	if (fp->f_name != NULL)			/* free any old name */
	    FREE(fp->f_name) ;

	fp->f_name = ALLOCF(strlen(name_buf) + 1, "clone-name ") ;
	if (fp->f_name)
	    strcpy(fp->f_name, name_buf) ;	/* save new name */

	if (!dp)				/* no directory entry */
	{
	    rslt = user_add_dir_entry(name_buf, new->i_mode, dev) ;
	    if (rslt != 0)
	    {
		printk("port_new_inode: error %d adding '%s' to directory\n",
			rslt, name_buf) ;
	    }
	}

	if (!(dp = user_search_dir(name_buf)))
	{
	    printk("port_new_inode: could not find '%s' in directory\n",
		    name_buf) ;
	}
	else
	{
	    new->i_inum = dp->d_inum  ;		/* assign inode nr */
	    dp->d_refcnt++ ;
	}
    }

    port_put_inode(old);

    return(new) ;				/* use new one */

} /* port_new_inode */

/*
 *  port_old_inode() - replace f's inode with one already in use
 */
struct inode *port_old_inode( struct file *f, struct inode *i )
{
    struct inode *oi = f->f_inode;

    f->f_inode = port_grab_inode(i);
    port_put_inode(oi);

    return(i);				/* use new one */
}

/*
 * port_new_file_name - rename the file
 */
int port_new_file_name(struct file *f, const char *name)
{
    if (f->f_name != NULL)			/* free any old name */
	FREE(f->f_name) ;

    f->f_name = ALLOCF(strlen(name) + 1, "file-name ") ;
    if (f->f_name)
	strcpy(f->f_name, name) ;		/* save new name */
    return(0) ;
}

/*
 * port_cleanup_file... - any unusual counter decrements or memory
 * freeing that needs to be done.
 */
void port_cleanup_file_opening(struct file *f, stdata_t *head, int open_fail)
{
}

void port_cleanup_file_closing(struct file *f, stdata_t *head)
{
}

void port_new_stream_name(struct stdata *hd, struct file *f)
{
    user_new_stream_name(hd, f) ;
}

void
port_show_inode_aliases( struct inode *i )
{
}

/*
 *  port_get_fifo()
 */
int port_get_fifo( struct file **f )
{
    return(user_get_fifo(f));
}

/*
 *  port_get_pipe()
 */
int port_get_pipe( struct file **f0, struct file **f1 )
{
    return(user_get_pipe( f0, f1 ));
}

int port_pipe( unsigned int *fd )
{
    return(user_pipe(fd));
}

int port_fattach( struct file *f, const char *path )
{
    _return(-ENOSYS);
}

int port_fdetach( const char *path )
{
    _return(-ENOSYS);
}

void port_fdetach_stream( stdata_t *head )
{
}

/*
 *  port_sendfd()
 */
int port_sendfd( stdata_t *head, unsigned int fd, struct file *fp )
{
    return(user_sendfd( head, fd, fp ));
}

/*
 *  port_recvfd()
 */
int port_recvfd( stdata_t *head, strrecvfd_t *recv, struct file *fp )
{
    return(user_recvfd( head, recv, fp ));
}

/*
 *  port_free_passfp()
 */
void port_free_passfp( mblk_t *mp )
{
    user_free_passfp(mp);
}

/************************************************************************
*                           Credentials                                 *
*************************************************************************
*									*
* Dummies here.  A porting file for a real operating system would have	*
* code here similar to what is in linux-mdep.c.				*
*									*
************************************************************************/
void	lis_task_to_creds(lis_kcreds_t *cp)
{
    (void) cp ;
}

void	lis_creds_to_task(lis_kcreds_t *cp)
{
    (void) cp ;
}


