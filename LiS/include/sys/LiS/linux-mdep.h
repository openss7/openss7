/*                               -*- Mode: C -*- 
 * mdep.h --- machine (actually kernel) dependencies.
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: linux-mdep.h,v 1.15 1996/01/27 00:40:26 dave Exp $
 * Purpose         : provide kernel independence as much as possible
 *                 : This could be also considered to be en embryo for
 *                 : dki stuff,i.e. linux-dki
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Francisco J. Ballesteros, Denis Froschauer
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
 *    You can reach su by email to any of
 *    nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 *    dave@gcom.com
 */

#ifndef _LIS_M_DEP_H
#define _LIS_M_DEP_H 1

#ident "@(#) LiS linux-mdep.h 2.50 12/18/02 20:36:11 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

/*
 * types.h will include <linux/config.h>.  If we have generated our own
 * autoconf.h file then we need to include it prior to anything else.
 * We set the include marker that prevents the <linux/config.h> from
 * including its own autoconf.h.
 */
#ifdef LISAUTOCONF
#include <sys/autoconf.h>           /* /usr/src/LiS/include/sys */
#define _LINUX_CONFIG_H 1	    /* prevent <linux/config.h> */
#endif
#ifndef _SYS_TYPES_H
#include <linux/types.h>
#define _SYS_TYPES_H	1	/* pretend included */
#endif

/* kernel includes go here */
#ifdef __KERNEL__
/*
 * The IRDA driver's use of queue_t interferes with ours.  Later versions
 * of the kernel source do not have this problem, but this is safe for all
 * versions.
 */
#define queue_t	irda_queue_t
#ifdef MODVERSIONS
# ifdef LISMODVERS
# include <sys/modversions.h>           /* /usr/src/LiS/include/sys */
# else
# include <linux/modversions.h>
# endif
#endif
#include <linux/version.h>
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#define	KERNEL_2_0
#else
# define	KERNEL_2_1	/* 2.1.x and 2.2.x kernel */
# if LINUX_VERSION_CODE > KERNEL_VERSION(2,3,0)
# define	KERNEL_2_3	/* 2.3.x and 2.4.x kernel */
#  if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,7)
#  define	KERNEL_2_4_7	/* 2.4.7+ redefines dentry structure */
#  endif
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
#  define	KERNEL_2_5	/* 2.5.x and 2.6.x kernel */
#  endif

# endif
#endif
#ifndef _LINUX_TYPES_H
#include <linux/types.h>        /* common system types */
#endif
#include <linux/kdev_t.h>	/* 1.3.xx needs this */
#include <linux/sched.h>	/* sleep,wake,... */
#include <linux/wait.h>
#include <linux/kernel.h>	/* suser,...*/
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/fs.h>		/* inodes,... */
#include <linux/fcntl.h>	/* inodes,... */
#include <linux/string.h>	/* memcpy,... */
#include <linux/timer.h>	/* timers */
#include <linux/mm.h>		/* memory manager, pages,... */
#include <linux/slab.h>		/* memory manager, pages,... */
#include <linux/stat.h>		/* S_ISCHR */
#include <asm/segment.h>	/* memcpy_{to,from}_fs */
#include <asm/system.h>		/* sti,cli */
#include <linux/errno.h>	      /* for errno */
#include <linux/signal.h>	      /* for signal numbers */
#if defined(KERNEL_2_1)		/* 2.1 kernel or later */
#include <sys/poll.h>		/* ends up being linux/poll.h */
#include <linux/file.h>
#include <asm/uaccess.h>
#endif
#include <sys/LiS/genconf.h>	/* generated configs from installation */
#include <sys/LiS/config.h>
/* #include <sys/lislocks.h>	needs lis_atomic_t, below */

/*
 * Kernel loadable module support
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,0)
#ifdef CONFIG_KERNELD
#include <linux/kerneld.h>
#define LIS_LOADABLE_SUPPORT 1
#endif
#else
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#define LIS_LOADABLE_SUPPORT 1
#endif
#endif
#undef queue_t			/* allow visibility for LiS */

#endif /* __KERNEL__ */

/*  -------------------------------------------------------------------  */

/*  -------------------------------------------------------------------  */

/*
 * 2.1/2.2 kernels define this, earlier ones don't
 * OSH: In 2.2.1 and later this is an inline function, not a macro
 *      and we only need this in the kernel.
 */
#if 0

#ifndef signal_pending
#define	signal_pending(tsk)	(tsk->signal & ~tsk->blocked)
#endif

#else

#ifdef __KERNEL__
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,1)
#ifndef signal_pending
#define	signal_pending(tsk)	(tsk->signal & ~tsk->blocked)
#endif
#ifndef sigismember
#define sigismember(sig_msk_addr,signo) ( *(sig_msk_addr) & (1 << (signo)) )
#endif
#ifndef sigaddset
#define sigaddset(sig_msk_addr,signo) ( *(sig_msk_addr) |= (1 << (signo)) )
#endif
#endif
#endif

#endif

/*  -------------------------------------------------------------------  */

#if defined(__KERNEL__) && defined(KERNEL_2_0)
# ifndef MODULE_AUTHOR
# define MODULE_AUTHOR(str)	 static const char module_author[] = str
# endif
# ifndef MODULE_DESCRIPTION
# define MODULE_DESCRIPTION(str) static const char module_description[] = str
# endif
#endif

/* some missing symbols
 */

#ifdef __KERNEL__
			/* seconds to system tmout time units */
#define SECS_TO(t)	lis_milli_to_ticks(1000*(t))

extern long lis_time_till(long target_time);
extern long lis_target_time(long milli_sec);
extern long lis_milli_to_ticks(long milli_sec) ;
#endif				/* __KERNEL__ */

/* some missing generic types 
 */
#undef uid 
#undef gid
typedef int     o_uid_t;
typedef int     o_gid_t;
typedef unsigned   char uchar;
typedef struct cred {
	uid_t	cr_uid;			/* effective user id */
	gid_t	cr_gid;			/* effective group id */
	uid_t	cr_ruid;		/* real user id */
	gid_t	cr_rgid;		/* real group id */
} cred_t;


#ifdef __KERNEL__
#if defined(KERNEL_2_5)
#define lis_suser(fp)	capable(CAP_SYS_ADMIN)
#else
#define lis_suser(fp)	suser()
#endif
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */


#ifdef __KERNEL__
#define lis_free_page(cp) free_page((unsigned long)(cp))

#define PRINTK		printk

/*
 *  The ASSERT macro.
 */
extern void lis_assert_fail(const char *expr, const char *objname,
			    const char *file, unsigned int line);

#ifdef ASSERT
#undef ASSERT			/* we want our own version */
#endif

#ifdef LIS_OBJNAME
#define ___ASSERT_XSTR(s) ___ASSERT_STR(s)
#define ___ASSERT_STR(s) #s
#define ASSERT(expr)							   \
	((void)((expr) ? 0 : lis_assert_fail(#expr,			   \
					      ___ASSERT_XSTR(LIS_OBJNAME), \
					     __FILE__, __LINE__)))
#else
#define ASSERT(expr)							   \
	((void)((expr) ? 0 : lis_assert_fail(#expr, "streams",		   \
					     __FILE__, __LINE__)))
#endif


/* disable/enable interrupts
 */
#define SPLSTR(x)	x = lis_splstr()
#define SPLX(x)		lis_splx(x)


/*
 * Atomic functions
 *
 * Usage is: lis_atomic_t	av ;
 *           lis_atomic_set(&av, 1) ;
 *
 * Use the lis_ versions of these for portability across kernel versions.
 * You can use the direct kernel versions for speed at the risk of needing a
 * recompile of your driver code with each new kernel version.
 *
 * The "long" type is intended to be an opaque type to the user.  The routines
 * cast the pointer to lis_atomic_t to a pointer to atomic_t (kernel struct)
 * for operational purposes.  By keeping the kernel's atomic_t type invisible
 * from STREAMS drivers we help insulate them from kernel changes.
 */
typedef	volatile long		lis_atomic_t ;

void	lis_atomic_set(lis_atomic_t *atomic_addr, int valu) ;
int	lis_atomic_read(lis_atomic_t *atomic_addr) ;
void	lis_atomic_add(lis_atomic_t *atomic_addr, int amt) ;
void	lis_atomic_sub(lis_atomic_t *atomic_addr, int amt) ;
void	lis_atomic_inc(lis_atomic_t *atomic_addr) ;
void	lis_atomic_dec(lis_atomic_t *atomic_addr) ;
int	lis_atomic_dec_and_test(lis_atomic_t *atomic_addr) ;

/*
 * Now include lislocks.h
 */
#include <sys/lislocks.h>

/*
 *  lis_gettimeofday -  used by lis_hitime and similar functions
 */
void lis_gettimeofday(struct timeval *tv);


/* lock inodes...
 *
 * Must use kernel semaphore routine directly since the inode semaphore is a
 * kernel semaphore and not an LiS semaphore.
 */
int lis_kernel_down(struct semaphore *sem);
void lis_kernel_up(struct semaphore *sem);

#if 0			/* don't need to hold inode semaphore for I/O oprns */
#define	LOCK_INO(i)	lis_kernel_down(&((i)->i_sem))
#define	ULOCK_INO(i)	lis_kernel_up(&((i)->i_sem))
#else
#define	LOCK_INO(i)	do {;} while (0)
#define	ULOCK_INO(i)	do {;} while (0)
#endif

/*
 *  inode/stdata access
 */
struct inode  *lis_file_inode(struct file *f);
char          *lis_file_name(struct file *f);
struct stdata *lis_file_str(struct file *f);
void           lis_set_file_str(struct file *f, struct stdata *s);
struct stdata *lis_inode_str(struct inode *i);
void           lis_set_inode_str(struct inode *i, struct stdata *s);
struct inode  *lis_set_up_inode(struct file *f, struct inode *inode) ;
#define FILE_INODE(f)   lis_file_inode(f)
#define FILE_NAME(f)    lis_file_name(f)
#define FILE_STR(f)     lis_file_str(f)
#define SET_FILE_STR(f,s) lis_set_file_str(f,s)
#define INODE_STR(i)    lis_inode_str(i)
#define SET_INODE_STR(i,s)  lis_set_inode_str(i,s)
#if   defined(KERNEL_2_3)			/* linux > 2.3.0 */
#define	I_COUNT(i)	( (i) ? atomic_read(&((i)->i_count)) : -1 )
#define F_COUNT(f)	( (f) ? atomic_read(&((f)->f_count)) : -1 )
#else
#define	I_COUNT(i)	( (i) ? ((i)->i_count) : -1 )
#define	F_COUNT(f)	( (f) ? ((f)->f_count) : -1 )
#endif

struct dentry *lis_d_alloc_root(struct inode *i, int m);
void           lis_dput(struct dentry *d);
struct dentry *lis_dget(struct dentry *d);

/*
 * mode (m) parameter values for lis_d_alloc_root
 */

#define LIS_D_ALLOC_ROOT_NORMAL   0
#define LIS_D_ALLOC_ROOT_MOUNT    1

/*
 *  clone support
 */
extern struct inode *lis_grab_inode(struct inode *ino);
extern void          lis_put_inode(struct inode *ino);
extern struct inode *lis_new_inode(struct file *,dev_t);
extern struct inode *lis_old_inode(struct file *,struct inode *);
extern int           lis_new_file_name(struct file *, const char *);
extern void          lis_new_stream_name(struct stdata *, struct file *);
#if defined(KERNEL_2_1)
extern void          lis_cleanup_file_opening(struct file *,
					      struct stdata *, int,
					      struct dentry *,  int,
					      struct vfsmount *, int);
#else
extern void          lis_cleanup_file_opening(struct file *,
					      struct stdata *, int);
#endif
extern void          lis_cleanup_file_closing(struct file *, struct stdata *);

extern int lis_major;

/*
 * Device node support
 */
int     lis_mknod(char *name, int mode, dev_t dev) ;
int     lis_unlink(char *name) ;


/*
 *  FIFO/pipe support
 */
extern int lis_get_fifo(struct file **);
extern int lis_get_pipe(struct file **, struct file **);
extern int lis_pipe( unsigned int * );

extern int  lis_fifo_open_sync(struct inode *, struct file *);
extern void lis_fifo_close_sync(struct inode *, struct file *);
extern int  lis_fifo_write_sync(struct inode *, int);

/*
 *  syscall interface
 */
extern asmlinkage int lis_sys_pipe(unsigned int *);
/*
 *  ioctl interface
 */
extern int lis_ioc_pipe(unsigned int *);

/*
 *  fattach()/fdetach() support
 */
extern int lis_fattach(struct file *, const char *);
extern int lis_fdetach(const char *);

/*
 *  syscall interfaces
 */
extern asmlinkage int lis_sys_fattach(int, const char *);
extern asmlinkage int lis_sys_fdetach(const char *);
/*
 *  ioctl interfaces
 */
extern int lis_ioc_fattach(struct file *, char *);
extern int lis_ioc_fdetach(char *);

/*
 * Kernel loadable module support
 */
#ifdef CONFIG_KERNELD
#define LIS_LOADABLE_SUPPORT 1
#endif

#ifdef LIS_LOADABLE_SUPPORT

/* lis_loadable_load - load a loadable module. */
int lis_loadable_load(const char *name);

#endif /* ifdef LIS_LOADABLE_SUPPORT */

/*
 * Process kill
 */
extern int	lis_kill_proc(int pid, int sig, int priv) ;
extern int	lis_kill_pg (int pgrp, int sig, int priv) ;

/* Use Linux system macros for MAJOR and MINOR */
#if defined(KERNEL_2_5)

#define	STR_MAJOR		MAJOR		/* for dev_t */
#define	STR_MINOR		MINOR		/* for dev_t */
#define	STR_KMAJOR		major		/* for kdev struct */
#define	STR_KMINOR		minor		/* for kdev struct */

#else			/* not KERNEL_2_5 */

#define	STR_MAJOR		MAJOR		/* for dev_t */
#define	STR_MINOR		MINOR		/* for dev_t */
#define	STR_KMAJOR		MAJOR		/* for kdev struct */
#define	STR_KMINOR		MINOR		/* for kdev struct */

#endif			/* KERNEL_2_5 */


/*
 * Kernel threads
 *
 * This function can be used to start a kernel thread.  Any driver can use
 * this function.
 *
 * 'fcn' is the function that serves as the entry point for the thread.
 *
 * 'arg' is the argument passed to that function.
 *
 * 'name' is the name to give to the function.  Keep it under 16 bytes.
 *
 * lis_thread_start returns the pid of the new process, or < 0 if an error
 * occurred.
 *
 * Before the 'fcn' is entered, the newly created thread will have shed all
 * user space files and mapped memory.  All signals are still enabled.  Note
 * that when the kernel goes down for reboot all processes are first sent a
 * SIGTERM.  Once those have been processed, all processes are then sent a
 * SIGKILL.  It is the implementor's choice which of these it pays attention to
 * in order to exit prior to a reboot.  The LiS queue runner ignores SIGTERM so
 * that it can be in place to help close streams files.  It then exits on
 * SIGKILL.  Other processes may behave differently.
 *
 * The 'fcn' is entered with the "big kernel lock" NOT held, just as it would
 * be for calling the "kernel_thread" function directly.  On 2.2 kernels, the
 * 'fcn' should get this lock so that it can utilize kernel services safely.
 *
 * The user's function returns a value when it exits and that value is returned
 * to the kernel.  It is not clear that anything actually pays any attention to
 * this returned value.  It particular, it is not visible to the thread that
 * started the new thread.
 */
int     lis_thread_start(int (*fcn)(void *), void *arg, const char *name) ;

#else				/* __KERNEL__ */

/*
 * For user programs, provide a substitute for the lis_atomic_t that
 * is the same size, and hopefully numerical layout, as the kernel's
 * type.  This allows uer programs to view data structures that have
 * lis_atomic_t data in them.
 */
typedef	volatile long		lis_atomic_t ;

#define	lis_atomic_set(atomic_addr,valu) (*(atomic_addr) = (valu))
#define	lis_atomic_read(atomic_addr) 	 (*(atomic_addr))
#define	lis_atomic_add(atomic_addr,amt)  (*(atomic_addr) += (amt))
#define	lis_atomic_sub(atomic_addr,amt)  (*(atomic_addr) -= (amt))
#define	lis_atomic_inc(atomic_addr) 	 ((*(atomic_addr))++)
#define	lis_atomic_dec(atomic_addr) 	 ((*(atomic_addr))--)
#define	lis_atomic_dec_and_test(atomic_addr) ((*(atomic_addr))--)



#endif				/* __KERNEL__ */

/************************************************************************
*                            major/minor                                *
*************************************************************************
*									*
* Macros to extract the major and minor device numbers from a dev_t	*
* variable.								*
*									*
************************************************************************/

/*
 * Major and minor macros come from linux ./include/linux/kdev_t.h
 *
 * If sysmacros.h has been included it defines major and minor in
 * the old way.  We want the new way so we undefine them and redefine
 * them to use the kdev_t style.
 */
#if defined(KERNEL_2_5)
/*
 * Use major and minor from kdev_t.h.
 *
 * dev_t is an integer.  kdev_t is a structure.  Someday the kernel will
 * have 20 bit minor device numbers in kdev structures.  For now the
 * dev_t is still 8:8.  LiS uses dev_t almost everywhere.  The Linux
 * inode structure i_rdev is a kdev_t.
 *
 * This topic will probably have to be revisited later.
 *
 * The mk_dev function returns a kdev_t value.
 */

#define makedevice(majornum,minornum)	mk_kdev(majornum,minornum)
#define DEV_SAME(d1,d2)	((d1) == (d2))
#define DEV_TO_INT(dev) ((int)(dev))
#define KDEV_TO_INT(kdev)	kdev_val(kdev)
#define INT_TO_KDEV(dev)	val_to_kdev((unsigned int)(dev))

#else			/* not KERNEL_2_5 */

#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif
#ifdef makedevice
#undef makedevice
#endif

#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H		/* pretend sysmacros.h included */
#endif

#define	major(dev_t_var)	MAJOR(dev_t_var)
#define	minor(dev_t_var)	MINOR(dev_t_var)
#define makedevice(majornum,minornum)	MKDEV(majornum,minornum)
#define DEV_SAME(d1,d2)	((d1) == (d2))
#define DEV_TO_INT(dev) ((int)(dev))
#define KDEV_TO_INT(kdev)	((int)(kdev))
#define INT_TO_KDEV(dev)	((dev_t)(dev))

#endif			/* KERNEL_2_5 */

typedef unsigned long	major_t ;	/* mimics SVR4 */
typedef unsigned long	minor_t ;	/* mimics SVR4 */

#define	LIS_FIFO  FIFO__CMAJOR_0
#define LIS_CLONE CLONE__CMAJOR_0

#ifdef __KERNEL__

#ifndef VOID
#define VOID	void
#endif

#define UID(fp)	  current->uid
#define GID(fp)	  current->gid
#define EUID(fp)  current->euid
#define EGID(fp)  current->egid
#define PGRP(fp)  current->pgrp
#define PID(fp)	  current->pid

#define OPENFILES()     current->files->count
#define SESSION()       current->session

#define DBLK_ALLOC(n,f,l,g)	lis_malloc(n,GFP_ATOMIC | (g),1,f,l)
#define ALLOC(n)		lis_malloc(n,GFP_ATOMIC,0,__FILE__,__LINE__)
#define ZALLOC(n)		lis_zmalloc(n,GFP_ATOMIC,__FILE__,__LINE__)
#define ALLOCF(n,f)		lis_malloc(n,GFP_ATOMIC,0, f __FILE__,__LINE__)
#define ALLOCF_CACHE(n,f)	lis_malloc(n,GFP_ATOMIC,1, f __FILE__,__LINE__)
#define MALLOC(n)		lis_malloc(n,GFP_ATOMIC,0,__FILE__,__LINE__)
#define LISALLOC(n,f,l)		lis_malloc(n,GFP_ATOMIC,0,f,l)
#define FREE(p)			lis_free(p,__FILE__,__LINE__)
#define MEMCPY(dest, src, len)	memcpy(dest, src, len)
#define PANIC(msg)		panic(msg)
/*
 * These are used only internally
 */
#define	KALLOC(n,cls,cache)	lis__kmalloc(n,cls,cache)	/* lismem.c */
#define	KFREE(p)		lis__kfree(p)			/* lismem.c */

extern struct stdata	*lis_fd2str(int fd) ;	/* file descr -> stream */

extern void *lis__kmalloc(int nbytes, int class, int use_cache) ;
extern void *lis__kfree(void *ptr) ;

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

/* This should be entry points from the kernel into LiS
 * kernel should be fixed to call them when appropriate.
 */

/* some kernel memory has been free'd 
 * tell STREAMS
 */
#ifdef __KERNEL__
extern void
lis_memfree( void );

/* Get avail kernel memory size
 */
#define lis_kmemavail()	((unsigned long)-1) /* lots of mem avail :) */

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/* This will copyin usr string pointed by ustr and return the result  in
 * *kstr. It will stop at  '\0' or max bytes copyed in.
 * caller should call free_page(*kstr) on success.
 * Will return 0 or errno
 */
#ifdef __KERNEL__
extern int 
lis_copyin_str(struct file *fp, const char *ustr, char **kstr, int max);
int     lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len);
int     lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len);
int	lis_check_umem(struct file *fp, int rd_wr_fcn,
	                   const void *usr_addr, int lgth) ;


#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

/*
 * The routine 'lis_runqueues' just requests that the queues be run
 * at a later time.  A daemon process runs the queus with the help
 * of a special driver.  This driver has the routine lis_setqsched
 * in it.  See drivers/str/runq.c.
 */
#ifdef __KERNEL__

extern void	lis_setqsched(int can_call) ;
extern lis_atomic_t	lis_in_syscall ;
extern lis_atomic_t	lis_runq_req_cnt ;
#define	lis_runqueues()		do {					      \
    				     if (lis_atomic_read(&lis_runq_req_cnt))  \
					lis_setqsched(1);		      \
				   } while(0)

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*
 * Macros for locking and unlocking a queue structure.
 */
#define lis_lockqf(qp,f,l) do { 					\
				 lis_spin_lock_fcn(&(qp)->q_lock,f,l);	\
			      } while(0)
#define lis_lockq(qp)	lis_lockqf(qp,__FILE__,__LINE__)

#define lis_unlockqf(qp,f,l) do { 					     \
				     lis_spin_unlock_fcn(&(qp)->q_lock,f,l); \
			        } while(0)
#define lis_unlockq(qp)	lis_unlockqf(qp,__FILE__,__LINE__)


/*  -------------------------------------------------------------------  */

/*
 * The routine 'lis_select' handles select calls from the Linux kernel.
 * The structure 'lis_select_t' is embedded in the stdata structure
 * and contains the wait queue head.
 */
#ifdef __KERNEL__

#ifdef KERNEL_2_0

typedef struct lis_select_struct
{
    struct wait_queue	*sel_wait ;

} lis_select_t ;

extern int	lis_select(struct inode *inode, struct file *file,
			   int sel_type, select_table *wait) ;

extern void	lis_select_wakeup(struct stdata *hd) ;

#elif defined(KERNEL_2_1)

extern unsigned	lis_poll_2_1(struct file *fp, poll_table * wait);

#else
#error "Either KERNEL_2_0 or KERNEL_2_1 needs to be defined"
#endif

/*
 * bzero and bcopy
 */
#define	bzero(addr,nbytes)	memset(addr, 0, nbytes)
#define	bcopy(src,dst,n)	memcpy(dst,src,n)

/*  -------------------------------------------------------------------  */

/*
 * These routines protect us from being unloaded while we have files open
 */

extern void    lis_inc_mod_cnt_fcn(const char *file, int line) ;
extern void    lis_dec_mod_cnt_fcn(const char *file, int line) ;

#define lis_inc_mod_cnt()	lis_inc_mod_cnt_fcn(__LIS_FILE__,__LINE__)
#define lis_dec_mod_cnt()	lis_dec_mod_cnt_fcn(__LIS_FILE__,__LINE__)

/*  -------------------------------------------------------------------  */

/*
 *  timers
 */
#if defined(KERNEL_2_5)
#define TL_NEXT(tl)  (struct timer_list *)(tl).entry.next
#define TL_PREV(tl)  (struct timer_list *)(tl).entry.prev
#elif defined(KERNEL_2_3)
#define TL_NEXT(tl)  (struct timer_list *)(tl).list.next
#define TL_PREV(tl)  (struct timer_list *)(tl).list.prev
#else
#define TL_NEXT(tl)  (tl).next
#define TL_PREV(tl)  (tl).prev
#endif

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*
 * These are externs for functions defined in the liskmod.c module.
 *
 * If the kernel is of an advanced enough version then these are
 * unnecessary since they will be inlines in a .h file or at least
 * there will be a standard extern for them in a kernel .h.
 *
 * When these externs are enabled the the liskmod module must be
 * loaded prior to streams.o in order for these symbols to be
 * resolved.
 */

#ifdef __KERNEL__

# if (   LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)	\
      && LINUX_VERSION_CODE <  KERNEL_VERSION(2,2,18))	\
  ||							\
     (   LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)	\
      && LINUX_VERSION_CODE <  KERNEL_VERSION(2,4,1))

extern void	put_unused_fd(unsigned int fd) ;

# endif

/*
 * The following version testing is only approximately correct.
 * I know that 2.2.5 does not have "igrab" and that 2.2.14
 * does.
 */
# if (   LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)	\
      && LINUX_VERSION_CODE <  KERNEL_VERSION(2,2,14))	\

extern struct inode *igrab(struct inode *inode) ;

# endif

/*
 * For convenience, define FATTACH_VIA_MOUNT if appropriate
 */
#if defined(KERNEL_2_4_7)
#define FATTACH_VIA_MOUNT 1
#endif

#endif				/* __KERNEL__ */



#endif /*!__LIS_M_DEP_H*/


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
