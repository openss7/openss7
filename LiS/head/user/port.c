/************************************************************************
*                         Porting Routines                              *
*************************************************************************
*									*
* This file contains the routines with the "port_" prefix.  These	*
* routines are tailored for the USER environment.			*
*									*
* Author:	David Grothe	<dave@gcom.com>				*
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

#ident "@(#) LiS port.c 2.16 04/09/04 14:20:35 "

#include <sys/stream.h>
#include <errno.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#include <sys/LiS/usrio.h>

/************************************************************************
*                        Data Structures                                *
************************************************************************/


/************************************************************************
*                         Global Storage                                *
************************************************************************/
int		 spin_lock_nest_level ;
int		 spin_lock_error_count ;
lis_atomic_t	 lis_spin_lock_count ;
lis_atomic_t	 lis_spin_lock_contention_count ;
lis_spin_lock_t	*most_recent_spin_lock ;


/************************************************************************
*                           Prototypes                                  *
************************************************************************/

extern file_t		*user_file_of_fd(int fd) ;  /* in usrio.c */

#define _return(e) return ((e)<0?-(errno=-(e)):(e))

/************************************************************************
*                           spin_lock_error                             *
*************************************************************************
*									*
* Mainly a place to put a breakpoint.					*
*									*
************************************************************************/
void spin_lock_error(void)
{
    spin_lock_error_count++ ;
}

/************************************************************************
*                         check_for_holding_spinlock                    *
*************************************************************************
*									*
* Called from various places that might "sleep" to catch occurrances of	*
* potential sleeps while holding a spinlock.				*
*									*
************************************************************************/
void check_for_holding_spinlock(void)
{
    if (spin_lock_nest_level)
    {
	if (most_recent_spin_lock != NULL)
	    printf("Holding spin lock \"%s\"\n", most_recent_spin_lock->name) ;
	else
	    printf("spin_lock_nest_level != 0 but no recent spin lock\n") ;

	spin_lock_error() ;
    }
}

/************************************************************************
*                         decr_spin_lock_count                          *
************************************************************************/
void decr_spin_lock_count(lis_spin_lock_t *lock)
{
    if (spin_lock_nest_level == 0)
    {
	printf("Spin lock nest level would go negative.  Lock=\"%s\"\n",
		lock->name) ;

	spin_lock_error() ;
    }
    else
    if (--spin_lock_nest_level == 0)
	most_recent_spin_lock = NULL ;
}

/************************************************************************
*                           port_printf                                 *
*************************************************************************
*									*
* This is the printf fuction.  Return is always zero.			*
*									*
************************************************************************/
int	port_printf(char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);

    return(0) ;

} /* port_printf */

/************************************************************************
*                            port_panic                                 *
*************************************************************************
*									*
* This is the panic routine.  It is not supposed to return.		*
*									*
************************************************************************/
void	port_panic(char *msg)
{
    port_printf("\nPANIC: %s\n", msg) ;
    exit(1) ;

} /* port_panic */

/************************************************************************
*                           port_malloc                                 *
*************************************************************************
*									*
* Allocate nbytes of memory and return a pointer to it.			*
*									*
************************************************************************/
void	*port_malloc(int nbytes, int class)
{
    (void) class ;			/* memory class */

    return(malloc(nbytes)) ;

} /* port_malloc */

/************************************************************************
*                          port_zmalloc                                 *
*************************************************************************
*									*
* Allocate nbytes of memory and clear to zero.				*
*									*
************************************************************************/
void	*port_zmalloc(int nbytes, int class)
{
    void	*ptr = malloc(nbytes) ;

    (void) class ;			/* memory class */

    if (ptr != NULL)
	memset(ptr, 0, nbytes) ;

    return(ptr) ;

} /* port_zmalloc */

/************************************************************************
*                           port_free                                   *
*************************************************************************
*									*
* Free a block of memory allocated by port_malloc.			*
*									*
************************************************************************/
void	port_free(void *ptr)
{
    free(ptr) ;

} /* port_free */

/************************************************************************
*                          port_print_mem                               *
*************************************************************************
*									*
* Walk the memory link list and print out each element.			*
*									*
************************************************************************/
void	port_print_mem(void)
{
    lis_print_mem() ;

} /* port_print_mem */

/************************************************************************
*                             port_memcpy                               *
*************************************************************************
*									*
* Copy nbytes from src to dst.						*
*									*
************************************************************************/
void	port_memcpy(void *dst, void *src, int nbytes)
{
    memcpy(dst, src, nbytes) ;

} /* port_memcpy */

/************************************************************************
*                            port_verify_area                           *
*************************************************************************
*									*
* Ensure that the user area pointed to by usr_addr can be accessed	*
* for reading or writing for lgth bytes.				*
*									*
* Return negative for failure.  The absolute value of the return code	*
* is an "errno" value.  Return 0 for success.				*
*									*
************************************************************************/
int	port_verify_area(struct file *f, int rd_wr_fcn,
			 const void *usr_addr, int lgth)
{
    (void) f ;
    (void) usr_addr ;
    (void) lgth ;

    switch (rd_wr_fcn)
    {
    case VERIFY_READ:
	if (   usr_addr == NULL
	    || lgth > 0x10000
	   )
	    _return(-EFAULT) ;

	return(0) ;				/* OK */
    case VERIFY_WRITE:
	if (   usr_addr == NULL
	    || lgth > 0x10000
	   )
	    _return(-EFAULT) ;

	return(0) ;				/* OK */
    default:
	_return(-EINVAL) ;			/* bad fcn code */
    }

} /* port_verify_area */

/************************************************************************
*                          port_memcpy_fromfs                           *
*************************************************************************
*									*
* Copy len bytes from user-space memory at ubuf into kernel space	*
* memory at kbuf.							*
*									*
* When porting to a real O/S environment, this routine will have to do	*
* more than what is coded here.						*
*									*
* Caller must guarantee that user addresses are valid before calling	*
* this routine.								*
*									*
************************************************************************/
int	port_memcpy_fromfs(struct file *f, void *kbuf,
			    const void *ubuf, int len)
{
    (void) f ;
    check_for_holding_spinlock() ;
    memcpy(kbuf, ubuf, len) ;
    return(0) ;

} /* port_memcpy_fromfs */

/************************************************************************
*                           port_memcpy_tofs                            *
*************************************************************************
*									*
* Copy len bytes from kernel-space memory at kbuf into user space	*
* memory at ubuf.							*
*									*
* When porting to a real O/S environment, this routine will have to do	*
* more than what is coded here.						*
*									*
* Caller must guarantee that user addresses are valid before calling	*
* this routine.								*
*									*
************************************************************************/
int	port_memcpy_tofs(struct file *f, const void *kbuf, void *ubuf, int len)
{
    (void) f ;
    check_for_holding_spinlock() ;
    memcpy(ubuf, kbuf, len) ;
    return(0) ;

} /* port_memcpy_tofs */

/************************************************************************
*                          port_get_fs_byte                             *
*************************************************************************
*									*
* Get a byte from user space and return it.				*
*									*
************************************************************************/
int	port_get_fs_byte(struct file *f, const void *uaddr)
{
    char	c ;

    check_for_holding_spinlock() ;
    if (port_verify_area(f, VERIFY_READ, uaddr, 1) < 0) return(0) ;

    port_memcpy_fromfs(f, &c, uaddr, 1) ;
    return( (int) c ) ;

} /* port_get_fs_byte */

/************************************************************************
*                            port_splstr                                *
*************************************************************************
*									*
* Disable interrupts to protect streams queue processing.  Return	*
* old CPU state through the pointer save_state.				*
*									*
* For user level testing, this is a dummy routine.			*
*									*
************************************************************************/
void	port_splstr(lis_flags_t *save_state)
{
    (void) save_state ;

} /* port_splstr */

/************************************************************************
*                             port_splx                                 *
*************************************************************************
*									*
* Restore the CPU interrupt state according to the saved state		*
* in the integer pointed to by saved_state.				*
*									*
************************************************************************/
void	port_splx(lis_flags_t *saved_state)
{
    (void) saved_state ;

} /* port_splx */

void	lis_splx_fcn(lis_flags_t x, char *file, int line)
{
    port_splx(&x) ;
    (void) file ;
    (void) line ;
}

/************************************************************************
*                             port_spl0                                 *
*************************************************************************
*									*
* Unconditionally enable interrupts and return the previous CPU state	*
* in the integer pointed to by save_state.				*
*									*
************************************************************************/
void	port_spl0(lis_flags_t *save_state)
{
    (void) save_state ;

} /* port_spl0 */

/************************************************************************
*                          port_sem_P                                   *
*************************************************************************
*									*
* Perform a 'P' operation on the semaphore.				*
*									*
* This is a dummy for user-level testing.				*
*									*
************************************************************************/
int	port_sem_P(struct semaphore *sem_addr)
{
    lis_flags_t		flags ;

    check_for_holding_spinlock() ;
    SPLSTR(flags) ;
    if (sem_addr->sem_count > 0)
    {					/* semaphore is available */
	sem_addr->sem_count-- ;		/* acquire it */
	SPLX(flags) ;
	return(0) ;
    }

    do
    {
	SPLX(flags) ;			/* open interrupt window */
	SPLSTR(flags) ;

	port_announce_time(1) ;		/* make time appear to pass */
	lis_runqueues() ;		/* run streams scheduler */
    }
    while (sem_addr->sem_count <= 0) ;	/* exit when semaphore available */

    sem_addr->sem_count-- ;		/* acquire it */
    SPLX(flags) ;
    return(0) ;

} /* port_sem_P */

/************************************************************************
*                          port_sem_V                                   *
*************************************************************************
*									*
* Perform a 'V' operation on the semaphore.				*
*									*
* This is a dummy for user-level testing.				*
*									*
************************************************************************/
void	port_sem_V(struct semaphore *sem_addr)
{
    ++sem_addr->sem_count ;		/* give up semaphore */

} /* port_sem_V */

/************************************************************************
*                            port_sem_init                              *
*************************************************************************
*									*
* Initialize the semaphore.						*
*									*
* This is a dummy for user-level testing.				*
*									*
************************************************************************/
void	port_sem_init(struct semaphore *sem_addr, int counter)
{
    sem_addr->sem_count = counter ;

} /* port_sem_init */

/************************************************************************
*                            port_sem_destroy                           *
*************************************************************************
*									*
* De-initialize the semaphore.						*
*									*
* This is a dummy for user-level testing.				*
*									*
************************************************************************/
void	port_sem_destroy(struct semaphore *sem_addr)
{
    memset(sem_addr, 0, sizeof(*sem_addr)) ;

} /* port_sem_destroy */

/************************************************************************
*                         Spin Locks                                    *
*************************************************************************
*									*
* Dummies.								*
*									*
************************************************************************/
#define	FL	char *file, int line

int    lis_spin_is_locked_fcn(lis_spin_lock_t *lock, FL)
{
    return(0) ;
}

void    lis_spin_lock_fcn(lis_spin_lock_t *lock, FL)
{
    lock->spin_lock_mem[0]++ ;
    spin_lock_nest_level++ ;
    most_recent_spin_lock = lock ;
}

void    lis_spin_unlock_fcn(lis_spin_lock_t *lock, FL)
{
    if (--lock->spin_lock_mem[0] < 0)
	spin_lock_error() ;
    decr_spin_lock_count(lock) ;
}

int     lis_spin_trylock_fcn(lis_spin_lock_t *lock, FL)
{
    lock->spin_lock_mem[0]++ ;
    spin_lock_nest_level++ ;
    most_recent_spin_lock = lock ;
    return(1) ;
}

void    lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, FL)
{
    lock->spin_lock_mem[0]++ ;
    spin_lock_nest_level++ ;
    most_recent_spin_lock = lock ;
}

void    lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, FL)
{
    if (--lock->spin_lock_mem[0] < 0)
	spin_lock_error() ;
    decr_spin_lock_count(lock) ;
}

void    lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, lis_flags_t *flags, FL)
{
    lock->spin_lock_mem[0]++ ;
    spin_lock_nest_level++ ;
    most_recent_spin_lock = lock ;
}

void    lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock,
				       lis_flags_t *flags, FL)
{
    if (--lock->spin_lock_mem[0] < 0)
	spin_lock_error() ;
    decr_spin_lock_count(lock) ;
}

void    lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, FL)
{
    static lis_spin_lock_t	z ;

    *lock = z ;
    lock->name = (char *) name ;
}

void	lis_print_spl_track(void)
{
}

int	lis_own_spl(void)
{
    return(0) ;
}

#undef FL

/************************************************************************
*                         Read/Write Spin Locks                         *
*************************************************************************
*									*
* Simulation of read-write locks.					*
*									*
************************************************************************/

#define	FL	char *file, int line

void    lis_rw_read_lock_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_write_lock_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, FL)
{
}

void    lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock,
				     lis_flags_t *flags, FL)
{
}

void    lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock,
				      lis_flags_t *flags, FL)
{
}

void    lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock,
					  lis_flags_t *flags, FL)
{
}

void    lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock,
					   lis_flags_t *flags, FL)
{
}

static void lis_rw_lock_fill(lis_rw_lock_t *lock, const char *name, FL)
{
    if (name == NULL) name = "RW-Lock" ;

    lock->name 		= (char *) name ;
    lock->spinner_file  = "Initialized" ;
    lock->owner_file    = lock->spinner_file ;
    lock->unlocker_file = lock->spinner_file ;
}


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

    lock_size = sizeof(*lock) ;
    lock = (lis_rw_lock_t *) ALLOCF(lock_size, "RW-Lock ");
    if (lock == NULL) return(NULL) ;

    lis_rw_lock_init_fcn(lock, name, file, line) ;
    lock->allocated = 1 ;
    return(lock) ;
}

lis_rw_lock_t *
lis_rw_lock_free_fcn(lis_rw_lock_t *lock, FL)
{
    if (lock->allocated)
	FREE((void *)lock) ;
    return(NULL) ;
}

#undef FL

/************************************************************************
*                          port_get_uid                                 *
*************************************************************************
*									*
* Get user id.								*
*									*
************************************************************************/
int	 port_get_uid(struct file *f)
{
    (void) f ;
    return(0 /*204*/) ;

} /* port_get_uid */

/************************************************************************
*                           port_get_gid                                *
*************************************************************************
*									*
* Get group id.								*
*									*
************************************************************************/
int	 port_get_gid(struct file *f)
{
    (void) f ;
    return(0 /*145*/) ;

} /* port_get_gid */

/************************************************************************
*                          port_get_euid                                *
*************************************************************************
*									*
* Get effective user id.						*
*									*
************************************************************************/
int	 port_get_euid(struct file *f)
{
    (void) f ;
    return(0 /*205*/) ;

} /* port_get_euid */

/************************************************************************
*                           port_get_egid                               *
*************************************************************************
*									*
* Get effective group id.						*
*									*
************************************************************************/
int	 port_get_egid(struct file *f)
{
    (void) f ;
    return(0 /*146*/) ;

} /* port_get_egid */

/************************************************************************
*                             port_get_pgrp                             *
*************************************************************************
*									*
* Get process group id.							*
*									*
************************************************************************/
int	 port_get_pgrp(struct file *f)
{
    (void) f ;
    return(33) ;

} /* port_get_pgrp */

/************************************************************************
*                              port_get_pid                             *
*************************************************************************
*									*
* Get process id.							*
*									*
************************************************************************/
int	 port_get_pid(struct file *f)
{
    (void) f ;
    return(19) ;

} /* port_get_pid */

/************************************************************************
*                             port_suser                                *
*************************************************************************
*									*
* Return true if we are running as the super user, false otherwise.	*
*									*
************************************************************************/
int	port_suser(struct file *f)
{
    return(port_get_uid(f) == 0) ;

} /* port_suser */

/************************************************************************
*                           port_kill_proc                              *
*************************************************************************
*									*
* Send the signal 'sig' to the process 'pid'.  'priv' is true if the	*
* user is super-user.  Return is negative error number or zero for	*
* success.								*
*									*
************************************************************************/
int	port_kill_proc(int pid, int sig, int priv)
{
    (void) pid ;			/* compiler happiness */
    (void) sig ;			/* compiler happiness */
    (void) priv ;			/* compiler happiness */
    return(0) ;

} /* port_kill_proc */

/************************************************************************
*                            port_kill_pg                               *
*************************************************************************
*									*
* Send the signal 'sig' to the process group 'pgrp'.  'priv' is true	*
* if the user is super-user.  Return is negative error number or	*
* zero for success.							*
*									*
************************************************************************/
int	port_kill_pg(int pgrp, int sig, int priv)
{
    (void) pgrp ;			/* compiler happiness */
    (void) sig ;			/* compiler happiness */
    (void) priv ;			/* compiler happiness */
    return(0) ;

} /* port_kill_pg */

/************************************************************************
*                         port_fd_to_str				*
*************************************************************************
*									*
* Given in integer file desriptor, fd, return a pointer to the stream	*
* head structure associated with the file.				*
*									*
************************************************************************/
stdata_t	*port_fd_to_str(int fd)
{
    file_t	*fp = user_file_of_fd(fd) ;

    if (fp == NULL)
	return(NULL) ;		/* no file */

    return(fp->f_drvr_ptr) ;	/* ptr to stdata structure */

} /* port_fd_to_str */

/************************************************************************
*                            port_session                               *
*************************************************************************
*									*
* Return the session number of the current process.  I do not understand*
* what this is supposed to be and it may be superfluous.		*
*									*
************************************************************************/
int	port_session(struct file *f)
{
    return(port_get_pid(f)) ;

} /* port_session */

/************************************************************************
*                         port_setqsched                                *
*************************************************************************
*									*
* This routine is supposed to schedule the queues to be run.  		*
* We do nothing.  The LiS entry point routines all call lis_runqueues	*
* on their way out which causes lis_run_queues to be called to actually	*
* run the queues.							*
*									*
************************************************************************/
void	port_setqsched(int can_call)
{
    (void) can_call ;			/* parameter not used */
#if 0					/* do nothing */
    lis_run_queues() ;
#endif

} /* port_setqsched */
/************************************************************************
*                         port_select_wakeup                            *
*************************************************************************
*									*
* A dummy for select mechanism (which we don't implement).		*
*									*
************************************************************************/
void port_select_wakeup(stdata_t *hd)
{
} /* port_select_wakeup */

/************************************************************************
*                           Semaphores                                  *
*************************************************************************
*									*
* Fake ones for user mode testing.					*
*									*
************************************************************************/

#define FL	char *file, int line

void	lis_up_fcn(lis_semaphore_t *lsem, FL)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;

    lsem->upper_file = file ;
    lsem->upper_line = line ;
    lsem->owner_file = "Available" ;
    lsem->owner_line = 0 ;
    port_sem_V(sem) ;
}

int	lis_down_fcn(lis_semaphore_t *lsem, FL)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;
    int			 ret ;

    lsem->downer_file = file ;
    lsem->downer_line = line ;
    ret = port_sem_P(sem) ;
    if (ret == 0)
    {
	lsem->owner_file = file ;
	lsem->owner_line = line ;
    }
    else
    {
	lsem->owner_file = "Error" ;
	lsem->owner_line = ret ;
    }

    return(ret) ;
}

void	lis_down_nosig_fcn(lis_semaphore_t *lsem, FL)
{
    lis_down_fcn(lsem, file, line) ;
}

#undef FL

void	lis_sem_init(lis_semaphore_t *lsem, int count)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;
    static lis_semaphore_t	blank ;

    *lsem = blank ;
    port_sem_init(sem, count) ;
    lsem->owner_file = "Initialized" ;
}

lis_semaphore_t *lis_sem_destroy(lis_semaphore_t *lsem)
{
    struct semaphore	*sem = (struct semaphore *) lsem->sem_mem ;
    port_sem_destroy(sem) ;
    return(NULL) ;
}

/************************************************************************
*                        Signal Sets                                    *
*************************************************************************
*									*
* Dummies.  Only meaningful in kernel context.				*
*									*
************************************************************************/
void lis_clear_and_save_sigs(stdata_t *hd)
{
    (void) hd ;
}

void lis_restore_sigs(stdata_t *hd)
{
    (void) hd ;
}
