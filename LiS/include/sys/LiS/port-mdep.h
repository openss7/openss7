/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * mdep.h --- machine (actually kernel) dependencies.
 * Author          : David Grothe
 * Created On      : Sat Dec 23 11:45:00 1995
 * Last Modified By: David Grothe
 * RCS Id          : $Id: port-mdep.h,v 1.10 1996/01/27 00:40:28 dave Exp $
 * Purpose         : Map certain environment provided functions into abstract
 *		   : names so that routines can be written for different
 *		   : operating system environments.
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995-1997  David Grothe
 *
 *    You can reach me by email to 
 *    dave@gcom.com
 *
 * This file contains defines and externals for the portable version
 * of STREAMS.  It matches up with port-mdep.c in the 'head' directory.
 *
 * There are a few typedefs (or structure definitions) left out
 * of this file.  These are for environmental data types such as
 * files and semaphores.  The idea is that you first include a more
 * specfic -mdep.h file, such as user-mdep.h or qnx-mdep.h.  That
 * file defines these necessary structures and then includes port-mdep.h.
 *
 * When compiling in the 'head' directory you can set compile-time
 * switches to cause this to happen automatically via the file strport.h.
 * These switches are:
 *
 *	-DUSER		Causes include of user-mdep.h and port-mdep.h
 *	-DQNX		Causes include of qnx-mdep.h and port-mdep.h
 *	-DLINUX		Causes include of linux-mdep.h only
 *	-DPORTABLE	Causes just port-mdep.h to be included.  This will
 *			lead to syntax errors due to the missing structure
 *			declarations.
 */

#ifndef _PORT_MDEP_H
#define _PORT_MDEP_H 1

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

/* operating system includes go here */

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif
#ifndef _STLIB_H
#include <stdlib.h>		/* for NULL, malloc, free */
#endif
#ifndef _STDIO_H
#include <stdio.h>		/* for printf */
#endif
#ifndef _FCNTL_H
#include <fcntl.h>		/* open flags, etc */
#endif
#ifndef _ASSERT_H
#include <assert.h>		/* for assert macro */
#endif

/*  -------------------------------------------------------------------  */

/* some missing symbols
 */

#define SECS_TO(t)	(1000*(t))	/* pass secs to system tmout time units */

/* some missing generic types 
 */
#ifdef dev_t
#undef dev_t
#endif
#define	dev_t	port_dev_t	/* our own definition */

int port_kill_proc(int pid, int sig, int priv);
int port_kill_pg(int pgrp, int sig, int priv);
int port_suser(struct file *fp);

#define	lis_suser	port_suser	/* are we super user */
#define	lis_kill_proc	port_kill_proc	/* signal a process */
#define	lis_kill_pg	port_kill_pg	/* signal a process group */

int port_printf(char *fmt, ...) __attribute__ ((format(printf, 1, 2)));	/* printf routine */

#define	printk		port_printf
#define PRINTK		port_printf

int port_register_chrdev(unsigned major, const char *name, struct file_operations *fops);
int port_unregister_chrdev(unsigned major, char *name);

#define	register_chrdev		port_register_chrdev
#define	unregister_chrdev	port_unregister_chrdev

/*  -------------------------------------------------------------------  */
/*                          Timer Structure				 */

struct timer_list {			/* borrowed from Linux kernel */
	struct timer_list *next;
	struct timer_list *prev;
	unsigned long tdelta;
	unsigned long data;
	void (*function) (unsigned long);
};

extern void port_add_timer(struct timer_list *timer);
extern int port_del_timer(struct timer_list *timer);
extern void port_announce_time(long milli_sec);
extern long port_time_till(long target_time);
extern long port_target_time(long milli_sec);
extern long port_milli_to_ticks(long milli_sec);

#define port_init_timer(timer)						\
			    {						\
				    (timer)->next = NULL;		\
				    (timer)->prev = NULL;		\
			    }

#define	add_timer		port_add_timer
#define	del_timer		port_del_timer
#define	init_timer		port_init_timer
#define	lis_time_till		port_time_till
#define	lis_target_time		port_target_time
#define	lis_milli_to_ticks	port_milli_to_ticks

/*  -------------------------------------------------------------------  */

#define lis_free_page(cp) port_free_page((unsigned long)(cp))
#define	lis_fd2str	  port_fd_to_str

struct stdata *port_fd_to_str(int fd);

/* should well-define this...
 */
#define LISASSERT(e)		assert(e)

/* disable/enable interrupts
 */
#define SPLSTR(x)	port_splstr(&(x))	/* save intr state */
#define SPLX(x)		port_splx  (&(x))	/* restore intr state */
#define	SPL0(x)		port_spl0  (&(x))	/* enable intrs */

#define	lis_print_spl_track	port_print_spl_track	/* if it exists */
void port_print_spl_track(void);

#if 0
#define	lis_hitime()	0	/* no such routine here */
#endif

void port_splstr(lis_flags_t * save_state);
void port_splx(lis_flags_t * saved_state);
void port_spl0(lis_flags_t * save_state);

/* lock inodes...
 */
#define	LOCK_INO(i)	lis_down(&i->i_sem)
#define	ULOCK_INO(i)	lis_up(&i->i_sem)

/*
 * Clone driver support
 */
#define lis_grab_inode          port_grab_inode
#define lis_put_inode           port_put_inode
#define	lis_new_inode	        port_new_inode
#define	lis_is_stream_inode	port_is_stream_inode
#define lis_old_inode           port_old_inode
#define lis_show_inode_aliases  port_show_inode_aliases
#define lis_set_up_inode        port_set_up_inode
#define	lis_new_file_name       port_new_file_name
#define	lis_new_stream_name     port_new_stream_name
#define	lis_cleanup_file_opening       port_cleanup_file_opening
#define	lis_cleanup_file_closing       port_cleanup_file_closing

extern struct inode *port_grab_inode(struct inode *);
extern void port_put_inode(struct inode *);
extern int port_is_stream_inode(struct inode *);
extern struct inode *port_new_inode(struct file *, dev_t);
extern struct inode *port_old_inode(struct file *, struct inode *);
extern void port_show_inode_aliases(struct inode *);
extern struct inode *port_set_up_inode(struct file *f, struct inode *inode);
extern int port_new_file_name(struct file *, const char *);
extern void port_cleanup_file_opening(struct file *, struct stdata *, int);
extern void port_cleanup_file_closing(struct file *, struct stdata *);
extern void port_new_stream_name(struct stdata *, struct file *);

/*
 *  FIFO/pipe support
 */
#define lis_get_fifo    port_get_fifo
#define lis_get_pipe    port_get_pipe
#define lis_pipe        port_pipe
#define lis_ioc_pipe    port_pipe

extern int port_get_fifo(struct file **);
extern int port_get_pipe(struct file **, struct file **);
extern int port_pipe(unsigned int *);

/*
 *  fattach()/fdetach() support
 */
#define lis_fattach        port_fattach
#define lis_ioc_fattach    port_fattach
#define lis_fdetach        port_fdetach
#define lis_ioc_fdetach    port_fdetach
#define lis_fdetach_stream port_fdetach_stream

extern int port_fattach(struct file *, const char *);
extern int port_fdetach(const char *);

/*
 *  I_SENDFD/I_RECVFD support
 */
#define lis_sendfd  port_sendfd
#define lis_recvfd  port_recvfd
#define lis_free_passfp port_free_passfp

/************************************************************************
*                          MAJOR/MINOR                                  *
*************************************************************************
*									*
* These are macros that will extract the major and minor device		*
* numbers from a port_dev_t variable.					*
*									*
* Note that in SVR4 these are 16-bits each.  We therefore assume that	*
* port_dev_t is a 32-bit long.						*
*									*
* Also note that for the Linux kernel version of STREAMS, dev_t is	*
* a 16 bit number and these macros are defined differently in		*
* linux-mdep.h.								*
*									*
************************************************************************/

#define	STR_MAJOR(port_dev_t_var)	MAJOR((port_dev_t_var))
#define	STR_MINOR(port_dev_t_var)	MINOR((port_dev_t_var))

#ifndef VOID
#define VOID	void
#endif

/*
 * for passing to mem allocators
 */
#ifndef	GFP_ATOMIC
#define	GFP_ATOMIC		0x01
#endif
#ifndef	GFP_DMA
#define	GFP_DMA			0x02
#endif

#define UID(x)			port_get_uid((x))
#define GID(x)			port_get_gid((x))
#define EUID(x)			port_get_euid((x))
#define EGID(x)			port_get_egid((x))
#define PGRP(x)  		port_get_pgrp((x))
#define PID(x)			port_get_pid((x))

#define OPENFILES()		port_openfiles()
#define SESSION(f)		port_session((f))

#define DBLK_ALLOC(n,f,l,g)	lis_malloc(n,GFP_ATOMIC | (g),1,f,l)
#define ALLOC(n)		lis_malloc(n,GFP_ATOMIC,0,__FILE__,__LINE__)
#define ZALLOC(n)		lis_zmalloc(n,GFP_ATOMIC,__FILE__,__LINE__)
#define ALLOCF(n,f)		lis_malloc(n,GFP_ATOMIC,0, f __FILE__,__LINE__)
#define ALLOCF_CACHE(n,f)	lis_malloc(n,GFP_ATOMIC,1, f __FILE__,__LINE__)
#define MALLOC(n)		lis_malloc(n,GFP_ATOMIC,0,__FILE__,__LINE__)
#define LISALLOC(n,f,l)		lis_malloc(n,GFP_ATOMIC,0,f,l)
#define FREE(p)			lis_free(p,__FILE__,__LINE__)
#define	KALLOC(n,cls,cache)	port_malloc(n,cls)
#define	KFREE(p)		port_free(p)
#define MEMCPY(dest, src, len)	port_memcpy(dest, src, len)
#define PANIC(msg)		port_panic(msg)

int port_get_uid(struct file *);
int port_get_gid(struct file *);
int port_get_euid(struct file *);
int port_get_egid(struct file *);
int port_get_pgrp(struct file *);
int port_get_pid(struct file *);

void *port_malloc(int size, int class);
void port_free(void *ptr);
void port_print_mem(void);
void port_memcpy(void *dest, void *src, int len);
void port_panic(char *msg);

int port_openfiles(void);
int port_session(struct file *);
struct inode *port_file_to_ino(int fd);

/*
 * These are really called indirectly from port.c
 */
int port_sem_P(struct semaphore *sem_addr);
void port_sem_V(struct semaphore *sem_addr);
void port_sem_init(struct semaphore *sem_addr, int counter);
void port_sem_destroy(struct semaphore *sem_addr);

/*  -------------------------------------------------------------------  */

/* This should be entry points from the environment into LiS.
 * The surrounding OS should be fixed to call them when appropriate.
 */

void port_init(void);			/* intialize STREAMS */

/* some kernel memory has been free'd 
 * tell STREAMS
 */
extern void lis_memfree(void);

/* Get avail kernel memory size
 */
#define lis_kmemavail()	((unsigned long)-1)	/* lots of mem avail :) */

/*  -------------------------------------------------------------------  */
/* This will copyin usr string pointed by ustr and return the result  in
 * *kstr. It will stop at  '\0' or max bytes copyed in.
 * caller should call free_page(*kstr) on success.
 * Will return 0 or errno
 * STATUS: complete, untested
 */
#define	lis_copyin_str		port_copyin_str
int port_copyin_str(struct file *fp, const char *ustr, char **kstr, int max);

/* Just another copy in / out
 */
#define lis_copyin(fp,kbuf,ubuf,len)	port_memcpy_fromfs(fp,kbuf,ubuf,len)
#define lis_copyout(fp,kbuf,ubuf,len)	port_memcpy_tofs(fp,kbuf,ubuf,len)

int port_memcpy_fromfs(struct file *fp, void *kbuf, const void *ubuf, int len);
int port_memcpy_tofs(struct file *fp, const void *kbuf, void *ubuf, int len);
int port_get_fs_byte(struct file *fp, const void *uaddr);

/* check a user memory area
 */
#define lis_check_umem(fp,f,p,l)	port_verify_area(fp,f,p,l)
int port_verify_area(struct file *fp, int rd_wr_fcn, const void *usr_addr, int lgth);

#ifndef VERIFY_READ
#define VERIFY_READ 0		/* argument for lis_check_umem */
#endif
#ifndef VERIFY_WRITE
#define VERIFY_WRITE 1		/* argument for lis_check_umem */
#endif

/*  -------------------------------------------------------------------  */

/*
 * Portable construct to request that the STREAMS queues be run.
 *
 * port_setqsched must be coded to schedule the STREAMS queues
 * to be run.
 */
#define	lis_setqsched		port_setqsched
#define	lis_runqueues()		port_setqsched(1)

extern void port_setqsched(int can_call);

/*  -------------------------------------------------------------------  */

/*
 * Portable routine to wake up processes that are waiting on the
 * stream head pending select() action.  The wakeup routine has
 * at its disposal the sd_select structure inside the stdata structure
 * which it can use to hold information that will allow it to wake
 * up waiting processes.
 */
#define	lis_select_wakeup	port_select_wakeup

extern void port_select_wakeup(struct stdata *hd);

/*  -------------------------------------------------------------------  */

/*
 * Dummies for module count manipulations.  Used only in Linux kernel.
 */
#if 0				/* replaced by MODGET/PUT */
#define lis_inc_mod_cnt()	/* nothing at all */
#define lis_dec_mod_cnt()	/* nothing at all */
#endif

/*  -------------------------------------------------------------------  */

/*
 *  timers
 */
#define TL_NEXT(tl)  (tl).next
#define TL_PREV(tl)  (tl).prev

#endif				/* !__LIS_M_DEP_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
