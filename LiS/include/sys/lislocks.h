/************************************************************************
*                        LiS Locks Interface                            *
*************************************************************************
*									*
* This is an interface to the kernel's lock handling routines.		*
* The idea is that STREAMS drivers that use these routines to manipulate*
* locks can be insulated in changes to the parameters in the kernel's	*
* locking routines.							*
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

#ifndef SYS_LISLOCKS_H
#define SYS_LISLOCKS_H	1

#ident "@(#) LiS lislocks.h 1.23 08/24/04"

#include <sys/LiS/genconf.h>

#define	FL	char *file, int line

/************************************************************************
*                          Spin Locks                                   *
*************************************************************************
*									*
* LiS defines its own spin lock type based upon the kernel's.		*
*									*
* We don't want the user's driver code to have to know anything about	*
* the size/shape of the kernel's spin lock structures, so we just 	*
* declare the spin lock space itself as a blind array of memory big	*
* enough to hold a kernel spin lock structure.				*
*									*
* LiS spin locks can be acquired in nested fashion by the same thread.	*
*									*
* LiS spin locks and RW locks share common structure elements.		*
*									*
************************************************************************/

#define LIS_SPIN_RW_COMMON_FIELDS					\
    lis_atomic_t nest ;							\
    void	*taskp ;						\
    unsigned	 contention_cnt ;	/* # times lock in contention */\
    char	*name ;							\
    char	*spinner_file ;		/* last to request the lock */	\
    int		 spinner_line ;						\
    int		 spinner_cntr ;		/* seq cntr */			\
    char	*owner_file ;		/* last to get the lock */	\
    int		 owner_line ;						\
    int		 owner_cntr ;		/* seq cntr */			\
    char	*unlocker_file ;	/* last to unlock the lock */	\
    int		 unlocker_line ;					\
    int		 unlocker_cntr ;	/* seq cntr */			\
    int		 allocated ;		/* allocated structure */

typedef volatile struct lis_spin_lock
{
    LIS_SPIN_RW_COMMON_FIELDS

    long	 spin_lock_mem[16] ;

} lis_spin_lock_t ;


/*
 * Call the lock functions by these define names so as to automatically
 * pass in the file and line number information.  In the defines, "lock" means
 * lis_spin_lock_t *lock, and "flags" means lis_flags_t *flags. So the usage is:
 *
 * lis_spin_lock_irqsave(&mylock, &myflags)
 *
 * LiS spin locks must be initialized via a call to lis_spin_lock_init.
 */
#define lis_spin_is_locked(lock) 	 	\
    				lis_spin_is_locked_fcn(lock,__FILE__,__LINE__) 
#define lis_spin_lock(lock) 	 	\
    				lis_spin_lock_fcn(lock,__FILE__,__LINE__) 
#define lis_spin_unlock(lock) 	 	\
    				lis_spin_unlock_fcn(lock,__FILE__,__LINE__) 
#define lis_spin_trylock(lock) 	 	\
    				lis_spin_trylock_fcn(lock,__FILE__,__LINE__) 
#define lis_spin_lock_irq(lock) 	\
    				lis_spin_lock_irq_fcn(lock,__FILE__,__LINE__) 
#define lis_spin_unlock_irq(lock) 	\
				lis_spin_unlock_irq_fcn(lock,__FILE__,__LINE__) 
#define lis_spin_lock_irqsave(lock,flags) 	 \
			lis_spin_lock_irqsave_fcn(lock,flags,__FILE__,__LINE__) 
#define lis_spin_unlock_irqrestore(lock,flags) 	 \
		    lis_spin_unlock_irqrestore_fcn(lock,flags,__FILE__,__LINE__)
#define	lis_spin_lock_init(lock,name)	\
			     lis_spin_lock_init_fcn(lock,name,__FILE__,__LINE__)
#define	lis_spin_lock_alloc(name)	\
			    lis_spin_lock_alloc_fcn(name,__FILE__,__LINE__)

#define lis_spin_lock_free(lock)	\
				lis_spin_lock_free_fcn(lock,__FILE__,__LINE__)
/*
 * These are the real routines.  Not to be called directly.
 */
#if defined(INT_PSW) && !defined(KERNEL_2_5)
typedef int		lis_flags_t ;
#else
typedef unsigned long	lis_flags_t ;
#endif

int	lis_spin_is_locked_fcn(lis_spin_lock_t *lock, FL) _RP;
void	lis_spin_lock_fcn(lis_spin_lock_t *lock, FL) _RP;
void	lis_spin_unlock_fcn(lis_spin_lock_t *lock, FL) _RP;
int	lis_spin_trylock_fcn(lis_spin_lock_t *lock, FL) _RP;
void	lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, FL) _RP;
void	lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, FL) _RP;
void	lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock,
				  lis_flags_t *flags, FL) _RP;
void	lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock,
				       lis_flags_t *flags, FL) _RP;
void	lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, FL) _RP;
lis_spin_lock_t *
	lis_spin_lock_alloc_fcn(const char *name, FL) _RP;
lis_spin_lock_t *
	lis_spin_lock_free_fcn(lis_spin_lock_t *lock, FL) _RP;


/************************************************************************
*                           Read/Write Spin Locks			*
*************************************************************************
*									*
* Multiple readers can hold the lock but only one writer and the writer	*
* also excludes all readers.						*
*									*
* These have different routines to get the lock for reading and for	*
* writing.  There are irq and non-irq forms of the routines.		*
*									*
************************************************************************/
typedef volatile struct lis_rw_lock
{
    LIS_SPIN_RW_COMMON_FIELDS

    long	 rw_lock_mem[16] ;

} lis_rw_lock_t ;

#define lis_rw_read_lock(lock) 	 	\
    				lis_rw_read_lock_fcn(lock,__FILE__,__LINE__) 
#define lis_rw_write_lock(lock) 	 	\
    				lis_rw_write_lock_fcn(lock,__FILE__,__LINE__) 

#define lis_rw_read_unlock(lock) 	 	\
    				lis_rw_read_unlock_fcn(lock,__FILE__,__LINE__) 
#define lis_rw_write_unlock(lock) 	 	\
    				lis_rw_write_unlock_fcn(lock,__FILE__,__LINE__) 

#define lis_rw_read_lock_irq(lock) 	\
			    lis_rw_read_lock_irq_fcn(lock,__FILE__,__LINE__)
#define lis_rw_write_lock_irq(lock) 	\
			    lis_rw_write_lock_irq_fcn(lock,__FILE__,__LINE__)

#define lis_rw_read_unlock_irq(lock) 	\
			    lis_rw_read_unlock_irq_fcn(lock,__FILE__,__LINE__) 
#define lis_rw_write_unlock_irq(lock) 	\
			    lis_rw_write_unlock_irq_fcn(lock,__FILE__,__LINE__) 

#define lis_rw_read_lock_irqsave(lock,flags) 	 \
		    lis_rw_read_lock_irqsave_fcn(lock,flags,__FILE__,__LINE__) 
#define lis_rw_write_lock_irqsave(lock,flags) 	 \
		    lis_rw_write_lock_irqsave_fcn(lock,flags,__FILE__,__LINE__) 

#define lis_rw_read_unlock_irqrestore(lock,flags) 	 \
		lis_rw_read_unlock_irqrestore_fcn(lock,flags,__FILE__,__LINE__)
#define lis_rw_write_unlock_irqrestore(lock,flags) 	 \
		lis_rw_write_unlock_irqrestore_fcn(lock,flags,__FILE__,__LINE__)

#define	lis_rw_lock_init(lock,name)	\
			     lis_rw_lock_init_fcn(lock,name,__FILE__,__LINE__)
#define	lis_rw_lock_alloc(lock,name)	\
			    lis_rw_lock_alloc_fcn(name,__FILE__,__LINE__)
#define lis_rw_lock_free(lock)	\
				lis_rw_lock_free_fcn(lock,__FILE__,__LINE__)
/*
 * These are the real routines.  Not to be called directly.
 */
void	lis_rw_read_lock_fcn(lis_rw_lock_t *lock, FL) _RP;
void	lis_rw_write_lock_fcn(lis_rw_lock_t *lock, FL) _RP;

void	lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, FL) _RP;
void	lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, FL) _RP;

void	lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, FL) _RP;
void	lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, FL) _RP;

void	lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, FL) _RP;
void	lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, FL) _RP;

void	lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock,
				     lis_flags_t *flags, FL) _RP;
void	lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock,
				      lis_flags_t *flags, FL) _RP;

void	lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock,
					  lis_flags_t *flags, FL) _RP;
void	lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock,
					   lis_flags_t *flags, FL)_RP;

void	lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, FL) _RP;
lis_rw_lock_t *
	lis_rw_lock_alloc_fcn(const char *name, FL) _RP;
lis_rw_lock_t *
	lis_rw_lock_free_fcn(lis_rw_lock_t *lock, FL) _RP;

/************************************************************************
*                           SPL Routines                                *
*************************************************************************
*									*
* For those of you using old-style Unix splstr/splx, there are some	*
* routines that do those functions.					*
*									*
************************************************************************/

#define	lis_splstr()		lis_splstr_fcn(__FILE__,__LINE__)
#define	lis_splx(x)		lis_splx_fcn(x,__FILE__,__LINE__)
#define	lis_spl0(x)		lis_spl0_fcn(__FILE__,__LINE__)

lis_flags_t	lis_splstr_fcn(FL) _RP;
void	lis_splx_fcn(lis_flags_t x, FL) _RP;
void	lis_spl0_fcn(FL) _RP;
void	lis_print_spl_track(void) _RP;
int	lis_own_spl(void) _RP;		/* do I own the global spl lock? */

/************************************************************************
*                           Semaphores                                  *
*************************************************************************
*									*
* An abstraction of a semaphore to keep user drivers from knowing what	*
* kernel semaphores look like.						*
*									*
************************************************************************/

typedef struct lis_semaphore
{
    void	*taskp ;		/* owners task pointer */
    unsigned	 contention_cnt ;	/* # times sem in contention */
    char	*downer_file ;		/* most recent "down" operation */
    int		 downer_line ;
    int		 downer_cntr ;		/* seq cntr */
    char	*owner_file ;		/* most recent successful "down" */
    int		 owner_line ;
    int		 owner_cntr ;		/* seq cntr */
    char	*upper_file ;		/* most recent "up" operation */
    int		 upper_line ;
    int		 upper_cntr ;		/* seq cntr */
    int		 allocated ;		/* allocated structure */
    struct timeval up_time ;		/* timestamp when lis_up called */
#if defined(CONFIG_PRIO_INHERIT)	/* a Timesys Linux thing */
    long	 sem_mem[50] ;
#else
    long	 sem_mem[20] ;
#endif

} lis_semaphore_t ;

/*
 * Drivers use these defines and not the real routines.
 */
#define	lis_up(s)	lis_up_fcn(s, __FILE__,__LINE__)
#define	lis_down(s)	lis_down_fcn(s, __FILE__,__LINE__)
#define	lis_down_nosig(s) lis_down_nosig_fcn(s, __FILE__,__LINE__)
extern void	lis_sem_init(lis_semaphore_t *,int)_RP;
extern lis_semaphore_t *lis_sem_destroy(lis_semaphore_t *)_RP;
extern lis_semaphore_t *lis_sem_alloc(int)_RP;

/*
 * Implementation routines, not to be used directly by drivers.
 */
extern void	lis_up_fcn(lis_semaphore_t *sem, FL) _RP;
extern int	lis_down_fcn(lis_semaphore_t *sem, FL) _RP;
extern void	lis_down_nosig_fcn(lis_semaphore_t *lsem, FL) _RP;

/*
 * Semaphore initialization is handled by lis_sem_init.  Semaphore destruction
 * is handled via this macro.  If the semaphore was allocated then 
 * sem_destroy will free it and return a NULL pointer.
 */
#define	SEM_DESTROY(sem_addr)	lis_sem_destroy(sem_addr)




#undef FL
#endif				/* SYS_LISLOCKS_H from top of file */
