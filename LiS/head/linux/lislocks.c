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

#ident "@(#) LiS lislocks.c 1.21 4/17/03"

#include <sys/LiS/linux-mdep.h>
#include <sys/LiS/strmdbg.h>
#include <sys/lislocks.h>
#include <sys/lismem.h>
#include <linux/sched.h>
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
#include <linux/spinlock.h>
#elif defined(KERNEL_2_1)		/* 2.1 and 2.2 kernels */
#include <asm/spinlock.h>
#else					/* earlier kernels */
					/* there will be syntax errors */
#endif

#if defined(KERNEL_2_5)
#define	SAVE_FLAGS(x)		local_save_flags(x)
#else
#define	SAVE_FLAGS(x)		save_flags(x)
#endif

#define FL	char *file, int line


/************************************************************************
*                         Lock Contention                               *
*************************************************************************
*									*
* Some global counters to keep track of spin lock contention.		*
*									*
************************************************************************/

lis_atomic_t	lis_spin_lock_count ;
lis_atomic_t	lis_spin_lock_contention_count ;


/************************************************************************
*                         SPL Implementation                            *
*************************************************************************
*									*
* We use a global LiS spin lock to implement the SPL functions.		*
*									*
************************************************************************/

lis_spin_lock_t		lis_spl_lock ;		/* for simulating spl fcns */
int			lis_psw0 ;		/* base level psw value */
#define	USE_SPINLOCK	0


#if defined(KERNEL_2_3)			/* 2.4 kernel or later */
#define	THELOCK		lock		/* use passed-in lock */
#define DCL_r  	        rwlock_t *r = (rwlock_t *) lock->rw_lock_mem 
#else					/* 2.2 kernel */
#define	THELOCK		(&lis_spl_lock)	/* use global lock */
#endif

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
    int			 flags ;
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

#define	LOCK_ENTRY(lock,typ,fil,lin,flgs)				\
    if ( LIS_DEBUG_SPL_TRACE )						\
    {									\
	spl_track_t		*p ;					\
	int			 flags ;				\
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
	int			 flags ;				\
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
	int			 flags ;				\
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
	int			 flags ;				\
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

/************************************************************************
*                         Prototypes                                    *
************************************************************************/

extern unsigned long lis_hitime(void);  /* usec res; 64s cycle */
extern int	     lis_seq_cntr ;


/************************************************************************
*                         lis_print_spl_track                           *
*************************************************************************
*									*
* Print out the spl tracking table.					*
*									*
************************************************************************/
void	lis_print_spl_track(void)
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

	    printk("%u:%s State=%d Flgs=%03x %s #%u\n",
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
int	lis_splstr_fcn(char *file, int line)
{
    int			 prev ;

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
void	lis_splx_fcn(int x, char *file, int line)
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
void	lis_spl0_fcn(char *file, int line)
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
int	lis_own_spl(void)
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

int	lis_spin_is_locked_fcn(lis_spin_lock_t *lock, FL)
{
    DCL_l ;

    (void) file; (void) line ;
    (void) l ;				/* compiler happiness in 2.2 */
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
# if defined(_PPC_LIS_)
    return(0) ;				/* PPC does not define this, odd */
# else
    return(spin_is_locked(l)) ;
# endif
#elif defined(KERNEL_2_1)		/* 2.1 and 2.2 kernels */
# if defined (__SMP__)
    return((*((volatile int *) (&l->lock))) == 1) ; /* lock state exists */
# else
    return(0) ;				/* lock state not tracked */
# endif
#else					/* earlier kernels */
    return(0) ;				/* LOL with 2.0 kernels */
#endif
}

void    lis_spin_lock_fcn(lis_spin_lock_t *lock, FL)
{
    int		 prev ;
    DCL_l ;

    (void) l ;				/* suppress compiler warning */
    lis_atomic_inc(&lis_spin_lock_count) ;
    SAVE_FLAGS(prev) ;
    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    if ((void *) current != lock->taskp)
    {
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
	if (spin_is_locked(l))
	    lis_atomic_inc(&lis_spin_lock_contention_count) ;
	spin_lock(l) ;
#endif
	lock->taskp = (void *) current ;
	lock->owner_file = file ;
	lock->owner_line = line ;
	lock->owner_cntr = ++lis_seq_cntr ;
    }
    lis_atomic_inc(&lock->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_spin_unlock_fcn(lis_spin_lock_t *lock, FL)
{
    int		 prev ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    if (lis_atomic_read(&lock->nest) > 0)
    {
	lis_atomic_dec(&lock->nest) ;
	if (lis_atomic_read(&lock->nest) == 0)
	{
	    lock->taskp = NULL ;
	    lock->unlocker_file = file ;
	    lock->unlocker_line = line ;
	    lock->unlocker_cntr = ++lis_seq_cntr ;
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
	    spin_unlock(l) ;
#endif
	}
    }
}

int     lis_spin_trylock_fcn(lis_spin_lock_t *lock, FL)
{
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
    int		ret ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    lis_atomic_inc(&lis_spin_lock_count) ;
    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    if ((void *) current != lock->taskp)
    {
	if ((ret = spin_trylock(l)) != 0)
	{
	    lis_atomic_inc(&lock->nest) ;
	    lock->taskp = (void *) current ;
	    lock->owner_file = file ;
	    lock->owner_line = line ;
	    lock->owner_cntr = ++lis_seq_cntr ;
	}
	else
	    lis_atomic_inc(&lis_spin_lock_contention_count) ;
	return(ret) ;
    }
    return(1) ;				/* already held */
#else					/* 2.2 kernel */
    lis_spin_lock_fcn(lock, file, line) ;
    return(1) ;				/* always succeeds */
#endif
}

void    lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, FL)
{
    int		 prev ;
    DCL_l ;

    (void) l ;				/* compiler happiness in 2.2 */
    lis_atomic_inc(&lis_spin_lock_count) ;
    SAVE_FLAGS(prev) ;

    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    if ((void *) current != THELOCK->taskp)
    {
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
	if (spin_is_locked(l))
	    lis_atomic_inc(&lis_spin_lock_contention_count) ;
	spin_lock_irq(l) ;
#else					/* 2.2 kernel */
	cli() ;				/* global cli */
#endif
	THELOCK->taskp = (void *) current ;
	lock->owner_file = file ;
	lock->owner_line = line ;
	lock->owner_cntr = ++lis_seq_cntr ;
    }
    lis_atomic_inc(&THELOCK->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, FL)
{
    int		 prev ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    if (lis_atomic_read(&THELOCK->nest) > 0)
    {
	lis_atomic_dec(&THELOCK->nest) ;
	if (lis_atomic_read(&THELOCK->nest) == 0)
	{
	    THELOCK->taskp = NULL ;
	    lock->unlocker_file = file ;
	    lock->unlocker_line = line ;
	    lock->unlocker_cntr = ++lis_seq_cntr ;
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
	    spin_unlock_irq(l) ;
#else					/* 2.2 kernel */
	    sti() ;			/* global sti */
#endif
	}
    }
}

void    lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, int *flags, FL)
{
    int		 prev ;
    DCL_l ;

    (void) l ;				/* compiler happiness in 2.2 */
    lis_atomic_inc(&lis_spin_lock_count) ;
    SAVE_FLAGS(prev) ;

    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    if ((void *) current != THELOCK->taskp)
    {
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
	if (spin_is_locked(l))
	    lis_atomic_inc(&lis_spin_lock_contention_count) ;
	spin_lock_irqsave(l, (*flags)) ;
#else					/* 2.2 kernel */
	cli() ;				/* global cli */
	if (lis_atomic_read(&THELOCK->nest) == 0)
	    *flags = prev ;
	else
	    *flags = 0xff ;		/* invalid */
#endif
	THELOCK->taskp = (void *) current ;
	lock->owner_file = file ;
	lock->owner_line = line ;
	lock->owner_cntr = ++lis_seq_cntr ;
    }
    lis_atomic_inc(&THELOCK->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock, int *flags, FL)
{
    int		 prev ;
    DCL_l ;

    (void) l ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    if (lis_atomic_read(&THELOCK->nest) > 0)
    {
	lis_atomic_dec(&THELOCK->nest) ;
	if (lis_atomic_read(&THELOCK->nest) == 0)
	{
	    THELOCK->taskp = NULL ;
	    lock->unlocker_file = file ;
	    lock->unlocker_line = line ;
	    lock->unlocker_cntr = ++lis_seq_cntr ;
#if defined(KERNEL_2_3)			/* 2.3 kernel or later */
	    spin_unlock_irqrestore(l, (*flags)) ;
#else					/* 2.2 kernel */
	    if (*flags != 0xff)
		restore_flags(*flags) ;
#endif
	}
    }
}

static void lis_spin_lock_fill(lis_spin_lock_t *lock, const char *name, FL)
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

void    lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, FL)
{
    memset((void *)lock, 0, sizeof(*lock)) ;
    lis_spin_lock_fill(lock, name, file, line) ;
}

lis_spin_lock_t *
lis_spin_lock_alloc_fcn(const char *name, FL)
{
    lis_spin_lock_t	*lock ;
    int			 lock_size ;

    lock_size = sizeof(*lock) - sizeof(lock->spin_lock_mem) +
						    sizeof(spinlock_t) ;
    lock = (lis_spin_lock_t *) lis_alloc_kernel_fcn(lock_size, file, line);
    if (lock == NULL) return(NULL) ;

    memset((void *)lock, 0, lock_size) ;
    lis_spin_lock_fill(lock, name, file, line) ;
    lock->allocated = 1 ;
    return(lock) ;
}

lis_spin_lock_t *
lis_spin_lock_free_fcn(lis_spin_lock_t *lock, FL)
{
    if (lock->allocated)
	lis_free_mem_fcn((void *)lock, file, line) ;
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

#if defined(KERNEL_2_3)

void    lis_rw_read_lock_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* suppress compiler warning */
    SAVE_FLAGS(prev) ;
    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    read_lock(r) ;
    lock->taskp = (void *) current ;
    lock->owner_file = file ;
    lock->owner_line = line ;
    lock->owner_cntr = ++lis_seq_cntr ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_rw_write_lock_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* suppress compiler warning */
    SAVE_FLAGS(prev) ;
    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    write_lock(r) ;
    lock->taskp = (void *) current ;
    lock->owner_file = file ;
    lock->owner_line = line ;
    lock->owner_cntr = ++lis_seq_cntr ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    lock->taskp = NULL ;
    lock->unlocker_file = file ;
    lock->unlocker_line = line ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    read_unlock(r) ;
}

void    lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    lock->taskp = NULL ;
    lock->unlocker_file = file ;
    lock->unlocker_line = line ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    write_unlock(r) ;
}

void    lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    read_lock_irq(r) ;
    THELOCK->taskp = (void *) current ;
    lock->owner_file = file ;
    lock->owner_line = line ;
    lock->owner_cntr = ++lis_seq_cntr ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    write_lock_irq(r) ;
    THELOCK->taskp = (void *) current ;
    lock->owner_file = file ;
    lock->owner_line = line ;
    lock->owner_cntr = ++lis_seq_cntr ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    lock->unlocker_file = file ;
    lock->unlocker_line = line ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    read_unlock_irq(r) ;
}

void    lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    lock->unlocker_file = file ;
    lock->unlocker_line = line ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    write_unlock_irq(r) ;
}

void    lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    read_lock_irqsave(r, (*flags)) ;
    THELOCK->taskp = (void *) current ;
    lock->owner_file = file ;
    lock->owner_line = line ;
    lock->owner_cntr = ++lis_seq_cntr ;
    lis_atomic_inc(&THELOCK->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* compiler happiness in 2.2 */
    SAVE_FLAGS(prev) ;

    lock->spinner_file = file ;
    lock->spinner_line = line ;
    lock->spinner_cntr = ++lis_seq_cntr ;
    write_lock_irqsave(r, (*flags)) ;
    THELOCK->taskp = (void *) current ;
    lock->owner_file = file ;
    lock->owner_line = line ;
    lock->owner_cntr = ++lis_seq_cntr ;
    lis_atomic_inc(&THELOCK->nest) ;
    LOCK_ENTRY(lock,TRACK_LOCK,file,line,prev)
}

void    lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    lock->unlocker_file = file ;
    lock->unlocker_line = line ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    read_unlock_irqrestore(r, (*flags)) ;
}

void    lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    int		 prev ;
    DCL_r ;

    (void) r ;				/* avoid warning in non-SMP case */
    SAVE_FLAGS(prev) ;

    LOCK_EXIT(lock,TRACK_UNLOCK,file,line,prev)
    THELOCK->taskp = NULL ;
    lock->unlocker_file = file ;
    lock->unlocker_line = line ;
    lock->unlocker_cntr = ++lis_seq_cntr ;
    write_unlock_irqrestore(r, (*flags)) ;
}

static void lis_rw_lock_fill(lis_rw_lock_t *lock, const char *name, FL)
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

#else			/* KERNEL_2_3 */

/*
 * For 2.2 kernel, just use the "noop" spin lock routines.
 */
void    lis_rw_read_lock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_lock_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_write_lock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_lock_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_unlock_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_unlock_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_lock_irq_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_lock_irq_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_unlock_irq_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
    lis_spin_unlock_irq_fcn((lis_spin_lock_t *)lock, file, line) ;
}

void    lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    lis_spin_lock_irqsave_fcn((lis_spin_lock_t *)lock, flags, file, line) ;
}

void    lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    lis_spin_lock_irqsave_fcn((lis_spin_lock_t *)lock, flags, file, line) ;
}

void    lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    lis_spin_unlock_irqrestore_fcn((lis_spin_lock_t *)lock, flags, file, line) ;
}

void    lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flags, FL)
{
    lis_spin_unlock_irqrestore_fcn((lis_spin_lock_t *)lock, flags, file, line) ;
}

static void lis_rw_lock_fill(lis_rw_lock_t *lock, const char *name, FL)
{
    lis_spin_lock_fill((lis_spin_lock_t *)lock, name, file, line) ;
}

#endif			/* KERNEL_2_3 */

void    lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, FL)
{
    memset((void *)lock, 0, sizeof(*lock)) ;
    lis_rw_lock_fill(lock, name, file, line) ;
}

lis_rw_lock_t *
lis_rw_lock_alloc_fcn(const char *name, FL)
{
    lis_rw_lock_t	*lock ;
    int			 lock_size ;

    lock_size = sizeof(*lock) - sizeof(lock->rw_lock_mem) +
						    sizeof(rwlock_t) ;
    lock = (lis_rw_lock_t *) lis_alloc_kernel_fcn(lock_size, file, line);
    if (lock == NULL) return(NULL) ;

    memset((void *)lock, 0, lock_size) ;
    lis_rw_lock_fill(lock, name, file, line) ;
    lock->allocated = 1 ;
    return(lock) ;
}

lis_rw_lock_t *
lis_rw_lock_free_fcn(lis_rw_lock_t *lock, const char *name, FL)
{
    if (lock->allocated)
	lis_free_mem_fcn((void *)lock, file, line) ;
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

void	lis_up_fcn(lis_semaphore_t *lsem, FL)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;

    SEM_EXIT(lsem,TRACK_UP,file,line,0) ;
    lsem->upper_file = file ;		/* most recent "up" */
    lsem->upper_line = line ;
    lsem->upper_cntr = ++lis_seq_cntr ;
    lsem->taskp      = NULL ;
    up(sem) ;
}

int	lis_down_fcn(lis_semaphore_t *lsem, FL)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;
    int			 ret ;

    lsem->downer_file = file ;		/* most recent "down" */
    lsem->downer_line = line ;
    lsem->downer_cntr = ++lis_seq_cntr ;
    ret = down_interruptible(sem) ;
    if (ret == 0)
    {
	SEM_ENTRY(lsem,TRACK_DOWN,file,line,0) ;
	lsem->taskp = (void *) current ;
	lsem->owner_file = file ;	/* current owner */
	lsem->owner_line = line ;
	lsem->owner_cntr = ++lis_seq_cntr ;
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

static void lis_sem_fill(lis_semaphore_t *lsem, int count)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;

    SET_SEM_COUNT(sem,count);
    lsem->owner_file = "Initialized" ;
    lsem->owner_cntr = ++lis_seq_cntr ;
}

void	lis_sem_init(lis_semaphore_t *lsem, int count)
{
    static lis_semaphore_t lis_sem_template ;	/* blank semaphore */

    *lsem = lis_sem_template ;			/* blank semaphore */
    lis_sem_fill(lsem, count) ;
}


lis_semaphore_t *lis_sem_destroy(lis_semaphore_t *lsem)
{
    if (lsem->allocated)
	lis_free_mem(lsem) ;
    else
    {
	static lis_semaphore_t lis_sem_template ;	/* blank semaphore */
	*lsem = lis_sem_template ;			/* blank semaphore */
	lsem->owner_file = "De-Initialized" ;
	lsem->owner_cntr = ++lis_seq_cntr ;
    }

    return(NULL) ;
}

lis_semaphore_t *lis_sem_alloc(int count)
{
    lis_semaphore_t *lsem ;
    int		     sem_size ;

    sem_size = sizeof(*lsem) - sizeof(lsem->sem_mem) + sizeof(struct semaphore);
    lsem     = (lis_semaphore_t *) lis_alloc_kernel(sem_size);

    if (lsem == NULL) return(NULL) ;

    memset(lsem, 0, sem_size) ;
    lis_sem_fill(lsem, count) ;
    lsem->allocated = 1 ;
    return(lsem) ;
}
