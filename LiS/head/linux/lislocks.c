/************************************************************************
*                         LiS Locks                                     *
*************************************************************************
*									*
*    Copyright (C) 2000  David Grothe, Gcom, Inc <dave@gcom.com>	*
*									*
* This library is free software; you can redistribute it and/or		*
* modify it under the terms of the GNU Library General Public		*
* License as published by the Free Software Foundation; either		*
* version 2 of the License, or (at your option) any later version.	*
* 									*
* This library is distributed in the hope that it will be useful,	*
* but WITHOUT ANY WARRANTY; without even the implied warranty of	*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*
* Library General Public License for more details.			*
* 									*
* You should have received a copy of the GNU Library General Public	*
* License along with this library; if not, write to the			*
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,		*
* Cambridge, MA 02139, USA.						*
*									*
************************************************************************/

#ident "@(#) LiS lislocks.c 1.48 09/07/04"

#include <sys/stream.h>
#include <sys/LiS/strmdbg.h>
#include <sys/lislocks.h>
#include <sys/lismem.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#if defined(KERNEL_2_5)

#if defined(CONFIG_DEV)
#define	SAVE_FLAGS(x)		local_save_flags(x)
#else 
#define	SAVE_FLAGS(x)
#endif

#else	/* KERNEL_2_5 */

#if defined(CONFIG_DEV)
#define	SAVE_FLAGS(x)		save_flags(x)
#else 
#define	SAVE_FLAGS(x)
#endif

#endif	/* KERNEL_2_5 */

#if defined(USE_KMEM_CACHE)
kmem_cache_t *lis_locks_cachep;
#endif

#define FL	char *file, int line

#if defined(CONFIG_DEV)
#define SET_SPINNER				\
	lock->spinner_file = file ;		\
	lock->spinner_line = line ;		\
	lock->spinner_cntr = ++lis_seq_cntr ;

#define SET_OWNER 				\
	lock->owner_file = file ; 		\
	lock->owner_line = line ; 		\
	lock->owner_cntr = ++lis_seq_cntr ;

#define SET_SPIN_UNLOCK				\
	lock->unlocker_file = file ;		\
	lock->unlocker_line = line ;		\
	lock->unlocker_cntr = ++lis_seq_cntr ;

#define	SPIN_FILL	lis_spin_lock_fill(lock, name, file, line) 

#define	RW_LOCK_FILL	lis_rw_lock_fill(lock, name, file, line) 

#define	SET_UPSEM				\
	lsem->upper_file = file ;		\
	lsem->upper_line = line ;		\
	lsem->upper_cntr = ++lis_seq_cntr ;

#define	SET_DSEM				\
	lsem->downer_file = file ;		\
	lsem->downer_line = line ;		\
	lsem->downer_cntr = ++lis_seq_cntr ;

#define	SET_SEMOWNER				\
	lsem->owner_file = file ;		\
	lsem->owner_line = line ;		\
	lsem->owner_cntr = ++lis_seq_cntr ;

#else

#define SET_SPINNER	(void) prev ;
#define SET_SPIN_UNLOCK	(void) prev ;
#define	SET_UPSEM
#define	SET_DSEM
#define	SPIN_FILL	lis_spin_lock_fill(lock, name) 
#define	RW_LOCK_FILL	lis_rw_lock_fill(lock, name) 

#define SET_OWNER 				\
	if (LIS_DEBUG_LOCK_CONTENTION)		\
	{					\
	    lock->owner_file = file ; 		\
	    lock->owner_line = line ; 		\
	    lock->owner_cntr = ++lis_seq_cntr ;	\
	}

#define	SET_SEMOWNER				\
	if (LIS_DEBUG_LOCK_CONTENTION)		\
	{					\
	    lsem->owner_file = file ;		\
	    lsem->owner_line = line ;		\
	    lsem->owner_cntr = ++lis_seq_cntr ;	\
	}

#endif

/*
 * This is a separate mechanism conditioned on
 * a debug bit.
 */
#define SET_LOCK_CONTENTION			\
	{					\
	    lock->contention_cnt++ ;		\
	    if (LIS_DEBUG_LOCK_CONTENTION)	\
		track_lock_contention(lock) ;	\
	}
#define SET_SEM_CONTENTION			\
	{					\
	    lsem->contention_cnt++ ;		\
	    if (LIS_DEBUG_LOCK_CONTENTION &&	\
		    lsem->taskp && lsem->taskp != current) \
		track_sem_contention(lsem) ;	\
	}

/************************************************************************
*                         Lock Contention                               *
*************************************************************************
*									*
* Some global counters to keep track of spin lock contention.		*
*									*
************************************************************************/

lis_atomic_t	lis_spin_lock_count ;
lis_atomic_t	lis_spin_lock_contention_count ;
spinlock_t	lis_contention_lock = SPIN_LOCK_UNLOCKED;

#define	LOCK_CONTENTION_SIZE	4	/* # locks to keep track of */
lis_spin_lock_t	*lis_lock_contention_table[LOCK_CONTENTION_SIZE] ;

/*
 * Same thing but for semaphores
 */
lis_atomic_t	 lis_sem_contention_count ;
lis_semaphore_t	*lis_sem_contention_table[LOCK_CONTENTION_SIZE] ;


/************************************************************************
*                         SPL Implementation                            *
*************************************************************************
*									*
* We use a global LiS spin lock to implement the SPL functions.		*
*									*
************************************************************************/

lis_spin_lock_t		lis_spl_lock ;		/* for simulating spl fcns */
lis_flags_t		lis_psw0 ;		/* base level psw value */
#define	USE_SPINLOCK	0


#define	THELOCK		lock		/* use passed-in lock */
#define DCL_r  	        rwlock_t *r = (rwlock_t *) lock->rw_lock_mem 
#define DCL_l  	     spinlock_t	*l = (spinlock_t *) THELOCK->spin_lock_mem 

/************************************************************************
*                         SPL Tracking                                  *
*************************************************************************
*									*
* The following structure is used to track SPL usage.			*
*									*
************************************************************************/
typedef struct
{
    int			 type ;
    int			 cpu ;
    void		*addr ;		/* of lock or semaphore */
    void		*tskp ;		/* current task */
    lis_flags_t		 flags ;
    int			 state ;
    char		*file ;
    int			 line ;
    int			 cntr ;

} spl_track_t ;

/*
 * Types
 */
#define	TRACK_SPLSTR	1		/* splstr call */
#define	TRACK_SPLX	2		/* splx call */
#define	TRACK_LOCK	3		/* some spinlock call */
#define	TRACK_UNLOCK	4		/* some unlock call */
#define	TRACK_DOWN	5		/* semaphore down */
#define	TRACK_UP	6		/* semaphore up */

#define	SPL_TRACK_SIZE	4096		/* power of 2 */
spl_track_t		 lis_spl_track[SPL_TRACK_SIZE] ;
spl_track_t		*lis_spl_track_ptr = lis_spl_track ;
/*
 * Note: this is a kernel lock, not an LiS lock.
 */
spinlock_t		lis_spl_track_lock = SPIN_LOCK_UNLOCKED ;

#if defined(CONFIG_DEV)
#define	LOCK_ENTRY(lock,typ,fil,lin,flgs)				\
    if ( LIS_DEBUG_SPL_TRACE )						\
    {									\
	spl_track_t		*p ;					\
	lis_flags_t		 flags ;				\
									\
	spin_lock_irqsave(&lis_spl_track_lock, flags) ;			\
	p = lis_spl_track_ptr ;						\
	if (++lis_spl_track_ptr >= &lis_spl_track[SPL_TRACK_SIZE])	\
	    lis_spl_track_ptr = lis_spl_track ;				\
	spin_unlock_irqrestore(&lis_spl_track_lock, flags) ;		\
									\
	p->addr = (void *) (lock) ;					\
	p->tskp = (void *) (current) ;					\
	p->file = fil ;							\
	p->line = lin ;							\
	p->type = typ ;							\
	p->flags = flgs ;						\
	p->cntr = ++lis_seq_cntr ;					\
	p->cpu  = smp_processor_id();					\
	p->state = (lock)->nest ;					\
    }

#define LOCK_EXIT(lock,typ,fil,lin,flgs)				\
    if ( LIS_DEBUG_SPL_TRACE )						\
    {									\
	spl_track_t		*p ;					\
	lis_flags_t		 flags ;				\
									\
	spin_lock_irqsave(&lis_spl_track_lock, flags) ;			\
	p = lis_spl_track_ptr ;						\
	if (++lis_spl_track_ptr >= &lis_spl_track[SPL_TRACK_SIZE])	\
	    lis_spl_track_ptr = lis_spl_track ;				\
	spin_unlock_irqrestore(&lis_spl_track_lock, flags) ;		\
									\
	p->addr = (void *) (lock) ;					\
	p->tskp = (void *) (current) ;					\
	p->file = fil ;							\
	p->line = lin ;							\
	p->type = typ ;							\
	p->flags = flgs ;						\
	p->cntr = ++lis_seq_cntr ;					\
	p->cpu  = smp_processor_id();					\
	p->state = (lock)->nest ;					\
    }

#define	SEM_ENTRY(sem,typ,fil,lin,flgs)					\
    if ( LIS_DEBUG_SPL_TRACE )						\
    {									\
	spl_track_t		*p ;					\
	lis_flags_t		 flags ;				\
									\
	spin_lock_irqsave(&lis_spl_track_lock, flags) ;			\
	p = lis_spl_track_ptr ;						\
	if (++lis_spl_track_ptr >= &lis_spl_track[SPL_TRACK_SIZE])	\
	    lis_spl_track_ptr = lis_spl_track ;				\
	spin_unlock_irqrestore(&lis_spl_track_lock, flags) ;		\
									\
	p->addr = (void *) (sem) ;					\
	p->tskp = (void *) (current) ;					\
	p->file = fil ;							\
	p->line = lin ;							\
	p->type = typ ;							\
	p->flags = flgs ;						\
	p->cntr = ++lis_seq_cntr ;					\
	p->cpu  = smp_processor_id();					\
	p->state = atomic_read(&((struct semaphore *) &sem->sem_mem)->count); \
    }

#define SEM_EXIT(sem,typ,fil,lin,flgs)					\
    if ( LIS_DEBUG_SPL_TRACE )						\
    {									\
	spl_track_t		*p ;					\
	lis_flags_t		 flags ;				\
									\
	spin_lock_irqsave(&lis_spl_track_lock, flags) ;			\
	p = lis_spl_track_ptr ;						\
	if (++lis_spl_track_ptr >= &lis_spl_track[SPL_TRACK_SIZE])	\
	    lis_spl_track_ptr = lis_spl_track ;				\
	spin_unlock_irqrestore(&lis_spl_track_lock, flags) ;		\
									\
	p->addr = (void *) (sem) ;					\
	p->tskp = (void *) (current) ;					\
	p->file = fil ;							\
	p->line = lin ;							\
	p->type = typ ;							\
	p->flags = flgs ;						\
	p->cntr = ++lis_seq_cntr ;					\
	p->cpu  = smp_processor_id();					\
	p->state = atomic_read(&((struct semaphore *) &sem->sem_mem)->count); \
    }
#else   
#define	LOCK_ENTRY(lock,typ,fil,lin,flgs)
#define	LOCK_EXIT(lock,typ,fil,lin,flgs)
#define	SEM_ENTRY(sem,typ,fil,lin,flgs)
#define	SEM_EXIT(sem,typ,fil,lin,flgs)
#endif	/* CONFIG_DEV	*/

/************************************************************************
*                         Prototypes                                    *
************************************************************************/

extern int	     lis_seq_cntr ;


/************************************************************************
*                        Lock Contention Tracking                       *
*************************************************************************
*									*
* Keep track of the few most contended for locks.			*
*									*
************************************************************************/
static void track_lock_contention(lis_spin_lock_t *lock)
{
    lis_spin_lock_t	**p = lis_lock_contention_table ;
    lis_spin_lock_t	**e = NULL;
    lis_spin_lock_t	**r = NULL;
    unsigned long	  psw ;

    spin_lock_irqsave(&lis_contention_lock, psw) ;

    for ( ; p != &lis_lock_contention_table[LOCK_CONTENTION_SIZE]; p++)
    {
	if ((*p) == lock)
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

	if ((*p)->contention_cnt < lock->contention_cnt)
	{
	    e = NULL ;			/* don't plug empty slot */
	    r = p ;
	}
    }

    if (r != NULL)		/* entry to be replaced */
	*r = lock ;
    else
    if (e != NULL)		/* empty slot, lock not in table */
	*e = lock ;		/* enter it into the table */

    spin_unlock_irqrestore(&lis_contention_lock, psw) ;
}

static void purge_lock_contention(lis_spin_lock_t *lock)
{
    lis_spin_lock_t	**p = lis_lock_contention_table ;
    unsigned long	  psw ;

    spin_lock_irqsave(&lis_contention_lock, psw) ;

    for ( ; p != &lis_lock_contention_table[LOCK_CONTENTION_SIZE]; p++)
    {
	if ((*p) == lock)
	    *p = NULL ;
    }

    spin_unlock_irqrestore(&lis_contention_lock, psw) ;
}

/*
 * Return a message with the decode of the contention table in it.
 */
mblk_t *lis_lock_contention_msg(void)
{
    mblk_t		 *mp ;
    lis_spin_lock_t	**p = lis_lock_contention_table ;
    lis_spin_lock_t	**p2 ;
    lis_spin_lock_t	 *t ;
    unsigned long	  psw ;
    unsigned		  tot_contention ;

    mp = allocb(LOCK_CONTENTION_SIZE * 100 + 50, BPRI_MED) ;
    if (mp == NULL)
	return(NULL) ;


    spin_lock_irqsave(&lis_contention_lock, psw) ;

    /*
     * Bubble sort the table in increasing lock contention order.
     */
    for ( ; p != &lis_lock_contention_table[LOCK_CONTENTION_SIZE-1]; p++)
    {
	if ((*p) == NULL)
	    continue ;

	for (p2 = p + 1;
	     p2 != &lis_lock_contention_table[LOCK_CONTENTION_SIZE];
	     p2++
	    )
	{
	    if ((*p2) == NULL)
		continue ;

	    if ( (*p2)->contention_cnt < (*p)->contention_cnt )
	    {
		t   = *p;
		*p  = *p2 ;
		*p2 = t ;
	    }
	}
    }


    tot_contention = K_ATOMIC_READ(&lis_spin_lock_contention_count) ;
    sprintf(mp->b_wptr, "Total Lock Contention: %u\n", tot_contention) ;
    while (*mp->b_wptr) mp->b_wptr++;

    for (p = lis_lock_contention_table;
	 p != &lis_lock_contention_table[LOCK_CONTENTION_SIZE];
	 p++
	)
    {
	if ((*p) == NULL)
	    continue ;

	sprintf(mp->b_wptr, "%10u(%d%%): %s last-owner: %s %d\n",
		(*p)->contention_cnt,
		tot_contention ?  ((*p)->contention_cnt * 100) / tot_contention
			       : 0,
		(*p)->name ? (*p)->name : "Anonymous",
		(*p)->owner_file ? (*p)->owner_file : "Unknown",
		(*p)->owner_line) ;
	while (*mp->b_wptr) mp->b_wptr++;
    }

    spin_unlock_irqrestore(&lis_contention_lock, psw) ;

    return(mp) ;
}

/************************************************************************
*                    Semaphore Contention Tracking                      *
*************************************************************************
*									*
* Keep track of the few most contended for semaphores.			*
*									*
************************************************************************/
static void track_sem_contention(lis_semaphore_t *lsem)
{
    lis_semaphore_t	**p = lis_sem_contention_table ;
    lis_semaphore_t	**e = NULL;
    lis_semaphore_t	**r = NULL;
    unsigned long	  psw ;

    spin_lock_irqsave(&lis_contention_lock, psw) ;

    for ( ; p != &lis_sem_contention_table[LOCK_CONTENTION_SIZE]; p++)
    {
	if ((*p) == lsem)
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

	if ((*p)->contention_cnt < lsem->contention_cnt)
	{
	    e = NULL ;			/* don't plug empty slot */
	    r = p ;
	}
    }

    if (r != NULL)		/* entry to be replaced */
	*r = lsem ;
    else
    if (e != NULL)		/* empty slot, lock not in table */
	*e = lsem ;		/* enter it into the table */

    spin_unlock_irqrestore(&lis_contention_lock, psw) ;
}

static void purge_sem_contention(lis_semaphore_t *lsem)
{
    lis_semaphore_t	**p = lis_sem_contention_table ;
    unsigned long	  psw ;

    spin_lock_irqsave(&lis_contention_lock, psw) ;

    for ( ; p != &lis_sem_contention_table[LOCK_CONTENTION_SIZE]; p++)
    {
	if ((*p) == lsem)
	    *p = NULL ;
    }

    spin_unlock_irqrestore(&lis_contention_lock, psw) ;
}

/*
 * Return a message with the decode of the contention table in it.
 */
mblk_t *lis_sem_contention_msg(void)
{
    mblk_t		 *mp ;
    lis_semaphore_t	**p = lis_sem_contention_table ;
    lis_semaphore_t	**p2 ;
    lis_semaphore_t	 *t ;
    unsigned long	  psw ;
    unsigned		  tot_contention ;

    mp = allocb(LOCK_CONTENTION_SIZE * 100 + 50, BPRI_MED) ;
    if (mp == NULL)
	return(NULL) ;


    spin_lock_irqsave(&lis_contention_lock, psw) ;

    /*
     * Bubble sort the table in increasing lock contention order.
     */
    for ( ; p != &lis_sem_contention_table[LOCK_CONTENTION_SIZE-1]; p++)
    {
	if ((*p) == NULL)
	    continue ;

	for (p2 = p + 1;
	     p2 != &lis_sem_contention_table[LOCK_CONTENTION_SIZE];
	     p2++
	    )
	{
	    if ((*p2) == NULL)
		continue ;

	    if ( (*p2)->contention_cnt < (*p)->contention_cnt )
	    {
		t   = *p;
		*p  = *p2 ;
		*p2 = t ;
	    }
	}
    }


    tot_contention = K_ATOMIC_READ(&lis_sem_contention_count) ;
    sprintf(mp->b_wptr, "Total Semaphore Contention: %u\n", tot_contention) ;
    while (*mp->b_wptr) mp->b_wptr++;

    for (p = lis_sem_contention_table;
	 p != &lis_sem_contention_table[LOCK_CONTENTION_SIZE];
	 p++
	)
    {
	if ((*p) == NULL)
	    continue ;

	sprintf(mp->b_wptr, "%10u(%d%%): last-owner: %s %d\n",
		(*p)->contention_cnt,
		tot_contention ?  ((*p)->contention_cnt * 100) / tot_contention
			       : 0,
		(*p)->owner_file ? (*p)->owner_file : "Unknown",
		(*p)->owner_line) ;
	while (*mp->b_wptr) mp->b_wptr++;
    }

    spin_unlock_irqrestore(&lis_contention_lock, psw) ;

    return(mp) ;
}

/************************************************************************
*                    Semaphore Wakeup Time Tracking                     *
*************************************************************************
*									*
* This is code to build a histogram of wakeup times for semaphores.	*
*									*
************************************************************************/
typedef struct
{
    int		micro_secs ;
    unsigned	counter ;

} histogram_bucket_t ;

histogram_bucket_t	lis_sem_hist[] =
			{
			    {1, 0},
			    {2, 0},
			    {3, 0},
			    {4, 0},
			    {5, 0},
			    {6, 0},
			    {7, 0},
			    {8, 0},
			    {9, 0},
			    {10, 0},
			    {20, 0},
			    {30, 0},
			    {40, 0},
			    {50, 0},
			    {60, 0},
			    {70, 0},
			    {80, 0},
			    {90, 0},
			    {100, 0},
			    {200, 0},
			    {300, 0},
			    {400, 0},
			    {500, 0},
			    {600, 0},
			    {700, 0},
			    {800, 0},
			    {900, 0},
			    {1000, 0},
			    {2000, 0},
			    {3000, 0},
			    {4000, 0},
			    {5000, 0},
			    {6000, 0},
			    {7000, 0},
			    {8000, 0},
			    {9000, 0},
			    {10000, 0},
			    {20000, 0},
			    {30000, 0},
			    {40000, 0},
			    {50000, 0},
			    {60000, 0},
			    {70000, 0},
			    {80000, 0},
			    {90000, 0},
			    {100000, 0},
			    {200000, 0},
			    {300000, 0},
			    {400000, 0},
			    {500000, 0},
			    {600000, 0},
			    {700000, 0},
			    {800000, 0},
			    {900000, 0},
			    {1000000, 0},
			    {2000000, 0},
			    {3000000, 0},
			    {4000000, 0},
			    {5000000, 0},
			    {6000000, 0},
			    {7000000, 0},
			    {8000000, 0},
			    {9000000, 0},

			    {0, 0}
			} ;


static void track_wakup_time(lis_semaphore_t *lsem)
{
    histogram_bucket_t *h = lis_sem_hist ;
    int		   interval ;		/* in micro seconds */
    struct timeval x ;

    do_gettimeofday(&x) ;
    interval =  (x.tv_sec * 1000000 + x.tv_usec) -
		(lsem->up_time.tv_sec * 1000000 + lsem->up_time.tv_usec) ;

    for ( ; h->micro_secs != 0; h++)
    {
	if (interval <= h->micro_secs)
	    break ;
    }

    h->counter++ ;
}

mblk_t *lis_get_sem_hist_msg(void)
{
    mblk_t	*mp ;

    mp = allocb(sizeof(lis_sem_hist), BPRI_MED) ;
    if (mp == NULL) return(NULL) ;

    memcpy(mp->b_wptr, lis_sem_hist, sizeof(lis_sem_hist)) ;
    mp->b_wptr += sizeof(lis_sem_hist) ;

    return(mp) ;
}

/************************************************************************
*                         lis_print_spl_track                           *
*************************************************************************
*									*
* Print out the spl tracking table.					*
*									*
************************************************************************/
void	 _RP lis_print_spl_track(void)
{
    spl_track_t		*p ;
    char		*typep ;
    int			 n ;

    printk("lis_print_spl_track: %lx lis_spl_track_ptr=%lx\n",
		(long)lis_spl_track, (long)lis_spl_track_ptr) ;

    for (p = lis_spl_track_ptr, n = SPL_TRACK_SIZE; n--;)
    {
	if (p->type != 0)
	{
	    switch (p->type)
	    {
	    case TRACK_SPLSTR:	typep = "splstr" ; break ;
	    case TRACK_SPLX:	typep = "splx  " ; break ;
	    case TRACK_LOCK:	typep = "lock  " ; break ;
	    case TRACK_UNLOCK:	typep = "unlock" ; break ;
	    case TRACK_DOWN:	typep = "down  " ; break ;
	    case TRACK_UP:	typep = "up    " ; break ;
	    default:		typep = "Unkwn " ; break ;
	    }

	    printk("%u:%s State=%d "
#if defined(INT_PSW) && !defined(KERNEL_2_5)
		    "Flgs=%03x "
#else
		    "Flgs=%03lx "
#endif
		    "%s #%u\n",
		    p->cntr,
		    typep,
		    p->state,
		    p->flags & 0x200,
		    p->file, p->line) ;
	}

	if (++p >= &lis_spl_track[SPL_TRACK_SIZE])
	    p = lis_spl_track ;
    }

    printk("lis_print_spl_track: end of output\n") ;
    
} /* lis_print_spl_track */

/************************************************************************
*                             lis_splstr                                *
*************************************************************************
*									*
* Disable interrupts, return the previous state.			*
*									*
************************************************************************/
lis_flags_t	 _RP lis_splstr_fcn(char *file, int line)
{
    lis_flags_t	 prev ;

    lis_spin_lock_irqsave_fcn(&lis_spl_lock, &prev, file, line) ;
    return(prev) ;

} /* lis_splstr */

/************************************************************************
*                             lis_splx                                  *
*************************************************************************
*									*
* Restore state.							*
*									*
************************************************************************/
void	 _RP lis_splx_fcn(lis_flags_t x, char *file, int line)
{
    lis_spin_unlock_irqrestore_fcn(&lis_spl_lock, &x, file, line) ;

} /* lis_splx */

/************************************************************************
*                             lis_spl0                                  *
*************************************************************************
*									*
* Go all the way back to base level.					*
*									*
************************************************************************/
void	 _RP lis_spl0_fcn(char *file, int line)
{
    while (lis_own_spl())
	lis_spin_unlock_irqrestore_fcn(&lis_spl_lock, &lis_psw0, file, line) ;

} /* lis_spl0 */

/************************************************************************
*                            lis_spl_init                               *
*************************************************************************
*									*
* Called only from LiS init routine.					*
*									*
************************************************************************/
void	lis_spl_init(void)
{
    lis_spin_lock_init(&lis_spl_lock, "LiS_SPL_Lock") ;
    lis_psw0 = lis_splstr() ;		/* capture base-level psw */
    lis_splx(lis_psw0) ;
}

/************************************************************************
*                            lis_own_spl                                *
*************************************************************************
*									*
* Return true if this task owns the global lis_spin_lock.		*
*									*
************************************************************************/
int	 _RP lis_own_spl(void)
{
    return (lis_spin_is_locked(&lis_spl_lock) && lis_spl_lock.taskp == current);
}

/************************************************************************
*                         Spin Locks                                    *
*************************************************************************
*									*
* These are the routines that interface to the kernel's spin lock 	*
* mechanism.								*
*									*
* Note for 2.2 implementation:  If SMP is not set then the 2.2 kernel	*
* has "pretend" spin locks that do not even track the state of the 	*
* lock.  If SMP is set then the implementation is genuine.  HOWEVER,	*
* the LiS routines do not use the kernel spin lock code.  The reason	*
* is that when LiS is running it owns the "big kernel lock".  It can	*
* happen in SMP situations that one CPU spins on a lock that a second	*
* thread needs to unlock, but can't because the first CPU is holding	*
* the "big kernel lock" while spinning.  Since the "big kernel lock"	*
* single threads kernel execution anyway the spin locks are not 	*
* necessary to proper driver function.  For the "irq" related spin	*
* lock functions we do the "cli" part, just not the spin lock part.	*
*									*
************************************************************************/

int	 _RP lis_spin_is_locked_fcn(lis_spin_lock_t *lock, FL)
{
    DCL_l ;

    (void) file; (void) line ;
    (void) l ;				/* compiler happiness in 2.2 */
#if defined(_PPC_LIS_)
    return(0) ;				/* PPC does not define this, odd */
#else
    return(spin_is_locked(l)) ;
#endif
}

void     _RP lis_spin_lock_fcn(lis_spin_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    void 	*tp ;
    DCL_l ;

    (void) l ;				/* suppress compiler warning */
    K_ATOMIC_INC(&lis_spin_lock_count) ;
    SAVE_FLAGS(prev);
    SET_SPINNER
    if ((tp = (void *) current) != lock->taskp)
    {
	if (spin_is_locked(l))
	{
	    K_ATOMIC_INC(&lis_spin_lock_contention_count) ;
	    SET_LOCK_CONTENTION
	}
	spin_lock(l) ;
	lock->taskp = tp ;
	SET_OWNER
    }
    K_ATOMIC_INC(&lock->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void     _RP lis_spin_unlock_fcn(lis_spin_lock_t *lock, FL)
{
    lis_flags_t		 prev ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    if (K_ATOMIC_READ(&lock->nest) > 0)
    {
	K_ATOMIC_DEC(&lock->nest) ;
	if (K_ATOMIC_READ(&lock->nest) == 0)
	{
	    lock->taskp = NULL ;
	    SET_SPIN_UNLOCK
	    spin_unlock(l) ;
	}
    }
}

int      _RP lis_spin_trylock_fcn(lis_spin_lock_t *lock, FL)
{
    int		ret ;
    lis_flags_t	prev ;
    void 	*tp ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    (void) prev ;
    K_ATOMIC_INC(&lis_spin_lock_count) ;
    SET_SPINNER
    if ((tp = (void *) current) != lock->taskp)
    {
	if ((ret = spin_trylock(l)) != 0)
	{
	    K_ATOMIC_INC(&lock->nest) ;
	    lock->taskp = tp ;
	    SET_OWNER
	}
	else
	{
	    K_ATOMIC_INC(&lis_spin_lock_contention_count) ;
	    SET_LOCK_CONTENTION
	}
	return(ret) ;
    }
    return(1) ;				/* already held */
}

void     _RP lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    void 	*tp ;
    DCL_l ;

    (void) l ;				/* compiler happiness in 2.2 */
    K_ATOMIC_INC(&lis_spin_lock_count) ;
    SAVE_FLAGS(prev) ;

    SET_SPINNER
    if ((tp = (void *) current) != THELOCK->taskp)
    {
	if (spin_is_locked(l))
	{
	    K_ATOMIC_INC(&lis_spin_lock_contention_count) ;
	    SET_LOCK_CONTENTION
	}
	spin_lock_irq(l) ;
	THELOCK->taskp = tp ;
	SET_OWNER
    }
    K_ATOMIC_INC(&THELOCK->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void     _RP lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    if (K_ATOMIC_READ(&THELOCK->nest) > 0)
    {
	K_ATOMIC_DEC(&THELOCK->nest) ;
	if (K_ATOMIC_READ(&THELOCK->nest) == 0)
	{
	    THELOCK->taskp = NULL ;
	    SET_SPIN_UNLOCK
	    spin_unlock_irq(l) ;
	}
    }
}

void     _RP lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, lis_flags_t *flags, FL)
{
    lis_flags_t	 prev ;
    void 	*tp ;
    DCL_l ;

    (void) l ;				/* compiler happiness in 2.2 */
    K_ATOMIC_INC(&lis_spin_lock_count) ;
    SAVE_FLAGS(prev) ;

    SET_SPINNER
    if ((tp = (void *) current) != THELOCK->taskp)
    {
	if (spin_is_locked(l))
	{
	    K_ATOMIC_INC(&lis_spin_lock_contention_count) ;
	    SET_LOCK_CONTENTION
	}
	spin_lock_irqsave(l, (*flags)) ;
	THELOCK->taskp = tp ;
	SET_OWNER
    }
    K_ATOMIC_INC(&THELOCK->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void     _RP lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock,
				       lis_flags_t *flags, FL)
{
    lis_flags_t	 prev ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    if (K_ATOMIC_READ(&THELOCK->nest) > 0)
    {
	K_ATOMIC_DEC(&THELOCK->nest) ;
	if (K_ATOMIC_READ(&THELOCK->nest) == 0)
	{
	    THELOCK->taskp = NULL ;
	    SET_SPIN_UNLOCK
	    spin_unlock_irqrestore(l, (*flags)) ;
	}
    }
}

#if defined(CONFIG_DEV)
static void lis_spin_lock_fill(lis_spin_lock_t *lock, const char *name, FL)
#else
static void lis_spin_lock_fill(lis_spin_lock_t *lock, const char *name)
#endif
{
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    if (name == NULL) name = "Spin-Lock" ;

    lock->name 		= (char *) name ;
    lock->spinner_file  = "Initialized" ;
    lock->owner_file    = lock->spinner_file ;
    lock->unlocker_file = lock->spinner_file ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    spin_lock_init(l) ;			/* kernel's init function */
}

void     _RP lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, FL)
{
    memset((void *)lock, 0, sizeof(*lock)) ;
    SPIN_FILL;
}

lis_spin_lock_t * _RP
lis_spin_lock_alloc_fcn(const char *name, FL)
{
    lis_spin_lock_t	*lock ;
    int			 lock_size ;

    lock_size = sizeof(*lock) - sizeof(lock->spin_lock_mem) +
						    sizeof(spinlock_t) ;
    lock = (lis_spin_lock_t *) LIS_LOCK_ALLOC(lock_size, "Spin-Lock ");
    if (lock == NULL) return(NULL) ;

    memset((void *)lock, 0, lock_size) ;
    SPIN_FILL;
    lock->allocated = 1 ;
    return(lock) ;
}

lis_spin_lock_t * _RP
lis_spin_lock_free_fcn(lis_spin_lock_t *lock, FL)
{
    if (lock == NULL)
	return(NULL) ;

    purge_lock_contention(lock) ;
    if (lock->allocated)
	LIS_LOCK_FREE((void *)lock) ;
    return(NULL) ;
}



/************************************************************************
*                         Read/Write Spin Locks                         *
*************************************************************************
*									*
* These are the routines that interface to the kernel's RW lock 	*
* mechanism.								*
*									*
* This is a more direct translation than spin locks.  There is no need	*
* to make these locks nestable, that I can think of, so there is less	*
* messing around with counts.						*
*									*
* For 2.2, all these routines are noops.				*
*									*
************************************************************************/


void    _RP lis_rw_read_lock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* suppress compiler warning */
    SAVE_FLAGS(prev) ;
    SET_SPINNER
    read_lock(r) ;
    lock->taskp = (void *) current ;
    SET_OWNER
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    _RP lis_rw_write_lock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* suppress compiler warning */
    SAVE_FLAGS(prev) ;
    SET_SPINNER
    write_lock(r) ;
    lock->taskp = (void *) current ;
    SET_OWNER
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    _RP lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    lock->taskp = NULL ;
    SET_SPIN_UNLOCK
    read_unlock(r) ;
}

void    _RP lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    lock->taskp = NULL ;
    SET_SPIN_UNLOCK
    write_unlock(r) ;
}

void    _RP lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    SET_SPINNER
    read_lock_irq(r) ;
    THELOCK->taskp = (void *) current ;
    SET_OWNER
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    _RP lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    SET_SPINNER
    write_lock_irq(r) ;
    THELOCK->taskp = (void *) current ;
    SET_OWNER
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    _RP lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    SET_SPIN_UNLOCK
    read_unlock_irq(r) ;
}

void    _RP lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    SET_SPIN_UNLOCK
    write_unlock_irq(r) ;
}

void    _RP lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock,
				     lis_flags_t *flags, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    SET_SPINNER
    read_lock_irqsave(r, (*flags)) ;
    THELOCK->taskp = (void *) current ;
    SET_OWNER
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    _RP lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock,
				      lis_flags_t *flags, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    SET_SPINNER
    write_lock_irqsave(r, (*flags)) ;
    THELOCK->taskp = (void *) current ;
    SET_OWNER
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    _RP lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock,
					  lis_flags_t *flags, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    SET_SPIN_UNLOCK
    read_unlock_irqrestore(r, (*flags)) ;
}

void    _RP lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock,
					   lis_flags_t *flags, FL)
{
    lis_flags_t	 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    SET_SPIN_UNLOCK
    write_unlock_irqrestore(r, (*flags)) ;
}

#if defined(CONFIG_DEV)
static void lis_rw_lock_fill(lis_rw_lock_t *lock, const char *name, FL)
#else
static void lis_rw_lock_fill(lis_rw_lock_t *lock, const char *name)
#endif
{
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    if (name == NULL) name = "RW-Lock" ;

    lock->name 		= (char *) name ;
    lock->spinner_file  = "Initialized" ;
    lock->owner_file    = lock->spinner_file ;
    lock->unlocker_file = lock->spinner_file ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    rwlock_init(r) ;			/* kernel's init function */
}


void    _RP lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, FL)
{
    memset((void *)lock, 0, sizeof(*lock)) ;
    RW_LOCK_FILL;
}

lis_rw_lock_t * _RP
lis_rw_lock_alloc_fcn(const char *name, FL)
{
    lis_rw_lock_t	*lock ;
    int			 lock_size ;

    lock_size = sizeof(*lock) - sizeof(lock->rw_lock_mem) +
						    sizeof(rwlock_t) ;
    lock = (lis_rw_lock_t *) LIS_LOCK_ALLOC(lock_size, "RW-Lock ");
    if (lock == NULL) return(NULL) ;

    memset((void *)lock, 0, lock_size) ;
    RW_LOCK_FILL;
    lock->allocated = 1 ;
    return(lock) ;
}

lis_rw_lock_t * _RP
lis_rw_lock_free_fcn(lis_rw_lock_t *lock, FL)
{
    if (lock->allocated)
	LIS_LOCK_FREE((void *)lock) ;
    return(NULL) ;
}

/************************************************************************
*                        Semaphores                                     *
*************************************************************************
*									*
* Subroutines here to insulate from kernel version differences.		*
*									*
************************************************************************/

/*
 * Initialize a semaphore
 */
#if defined(KERNEL_2_1)			/* 2.1 kernel or later */
# define SET_SEM_COUNT(semp,cnt)	sema_init((semp), (cnt))
#else					/* 2.0 kernel */
# ifdef ATOMIC_INIT
# define SET_SEM_COUNT(semp,cnt)	atomic_set(&((semp)->count), (cnt))
# else
# define SET_SEM_COUNT(semp,cnt)	(semp)->count = (cnt)
# endif
#endif

void	_RP lis_up_fcn(lis_semaphore_t *lsem, FL)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;

    SEM_EXIT(lsem,TRACK_UP,file,line,0) ;
    SET_UPSEM                           /* most recent "up" */
    lsem->taskp      = NULL ;
    if (LIS_DEBUG_SEMTIME)
	do_gettimeofday(&lsem->up_time) ;
    up(sem) ;
}

int	_RP lis_down_fcn(lis_semaphore_t *lsem, FL)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;
    int			 ret ;
    int			 had_to_wait = 0 ;

    SET_DSEM                            /* most recent "down" */

    if (!lsem->allocated)
    {
	printk("lis_down(sem=%p %s %d) uninitialized semaphore\n",
		lsem, file, line) ;
	return(-EINVAL) ;
    }

    if ((ret = down_trylock(sem)))
    {
	had_to_wait = 1 ;
	K_ATOMIC_INC(&lis_sem_contention_count) ;
	SET_SEM_CONTENTION
	ret = down_interruptible(sem) ;
    }

    if (ret == 0)
    {
	if (had_to_wait && LIS_DEBUG_SEMTIME)
	    track_wakup_time(lsem) ;

	SEM_ENTRY(lsem,TRACK_DOWN,file,line,0) ;
	lsem->taskp = (void *) current ;
	SET_SEMOWNER                    /* current owner */
    }
    else
    {
	SEM_ENTRY(lsem,TRACK_DOWN,file,line,ret) ;
	lsem->owner_file = "Error" ;
	lsem->owner_line = ret ;
	lsem->owner_cntr = ++lis_seq_cntr ;
    }

    return(ret) ;
}

/*
 * Do a down on a semaphore disallowing signals.  Use carefully.
 *
 * Useful for semaphores that must be used during close processing.
 * Chances are the process has been signalled at that time.
 */
#if defined(SIGMASKLOCK)
#define LOCK_MASK    spin_lock_irq(&current->sigmask_lock)
#define UNLOCK_MASK    recalc_sigpending(current); \
		       spin_unlock_irq(&current->sigmask_lock)
#else
#define LOCK_MASK    spin_lock_irq(&current->sighand->siglock)
#define UNLOCK_MASK    recalc_sigpending(); \
		       spin_unlock_irq(&current->sighand->siglock)
#endif

void	_RP lis_down_nosig_fcn(lis_semaphore_t *lsem, FL)
{
    sigset_t	save_sigs ;

    LOCK_MASK ;
    save_sigs = current->blocked ;
    sigfillset(&current->blocked) ;		/* block all signals */
    UNLOCK_MASK ;

    while (lis_down_fcn(lsem, file, line) < 0) ;

    LOCK_MASK ;
    current->blocked = save_sigs ;
    UNLOCK_MASK ;
}

static void lis_sem_fill(lis_semaphore_t *lsem, int count)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;

    SET_SEM_COUNT(sem,count);
    lsem->owner_file = "Initialized" ;
    lsem->owner_cntr = ++lis_seq_cntr ;
}

void	_RP lis_sem_init(lis_semaphore_t *lsem, int count)
{
    static lis_semaphore_t lis_sem_template ;	/* blank semaphore */

    *lsem = lis_sem_template ;			/* blank semaphore */
    lis_sem_fill(lsem, count) ;
    lsem->allocated = 2 ;			/* initialized static */
}


lis_semaphore_t * _RP lis_sem_destroy(lis_semaphore_t *lsem)
{
    int		allocated ;

    if (lsem == NULL)
	return(NULL) ;

    purge_sem_contention(lsem) ;
    lsem->owner_file = "De-Initialized" ;
    lsem->owner_cntr = ++lis_seq_cntr ;
    allocated = lsem->allocated ;
    lsem->allocated = 0 ;
    if (allocated == 1)
	LIS_LOCK_FREE(lsem) ;
    else
    {
	static lis_semaphore_t lis_sem_template ;	/* blank semaphore */
	*lsem = lis_sem_template ;			/* blank semaphore */
    }

    return(NULL) ;
}

lis_semaphore_t * _RP lis_sem_alloc(int count)
{
    lis_semaphore_t *lsem ;
    int		     sem_size ;

    sem_size = sizeof(*lsem) - sizeof(lsem->sem_mem) + sizeof(struct semaphore);
    lsem     = (lis_semaphore_t *) LIS_LOCK_ALLOC(sem_size, "Semaphore ");

    if (lsem == NULL) return(NULL) ;

    memset(lsem, 0, sem_size) ;
    lis_sem_fill(lsem, count) ;
    lsem->allocated = 1 ;			/* initialized dynamic alloc */
    return(lsem) ;
}

/************************************************************************
*                       Lock Initializaiton                             *
*************************************************************************
*									*
* This is code for the case of allocating locks in kernel cache.	*
*									*
************************************************************************/
#if defined(USE_KMEM_CACHE)
void lis_init_locks(void)
{
    lis_semaphore_t *lsem ;
    lis_spin_lock_t *llock ;
    int		     sem_size ;
    int		     spin_size ;
    int		     size ;

    sem_size = sizeof(*lsem) - sizeof(lsem->sem_mem) + sizeof(struct semaphore);
    spin_size = sizeof(*llock) - sizeof(llock->spin_lock_mem) +
						    sizeof(spinlock_t);
    size = sem_size > spin_size ? sem_size : spin_size ;
    lis_locks_cachep =
          kmem_cache_create("lis_locks_cache", size, 0,
                            SLAB_HWCACHE_ALIGN, NULL, NULL);
}

void lis_terminate_locks(void)
{
      lis_cache_destroy(lis_locks_cachep, &lis_locks_cnt, "lis_locks_cache");
}
#endif
