/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*                               -*- Mode: C -*- 
 * head.c --- LiS stream head processing
 * Author          : Graham Wheeler, Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: John A. Boyd Jr.
 * RCS Id          : $Id: head.c,v 1.1.1.10 2003/11/23 19:58:44 brian Exp $
 * Purpose         : stream head processing stuff
 * ----------------______________________________________________
 *
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros,
 *                        Denis Froschauer
 *    Copyright (C) 1997-2000
 *			  David Grothe, Gcom, Inc <dave@gcom.com>
 *    Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 *
 *
 * Modified 2001-02-09 by Jeff Goldszer <Jeff_goldszer@cnt.com> 
 *                     Fixed bugs the following:
 *                     1) Process hangs on io stream calls (ex. close)
 *                        following a T_LOOK IOCTL failure.
 *                     2) Getmsg does not return length of 0 when a 
 *                        peer pipe is closed.
 *                     3) Process hangs in Poll System call because
 *                        peer pipe is closed and Process Poll thread 
 *                        is not notified.
 *
 * Modified 2001-08-09 by Jeff Goldszer <Jeff_goldszer@cnt.com>
 *   Prevent a streams oops when an application does a putmsg to a
 *   streams which has been hung up.
 *
 */

/*  -------------------------------------------------------------------  */

/*

Analysis of open/close locking
==============================

It is important to ensure that open and close are synchronized properly.  In
particular, races involving the opening and closing of the same stream must be
resolved.  The following is a list of considerations in this system.

* Every stdata structure has a unique {major,minor} device identifier.

* Multiple open streams to the same dev must flow through the same stdata.

* The driver`s open routine can alter the values of the {major,minor}.

* A simultaneous close and an open to the same dev must be resolved.

The resolution of a simultaneous close and open goes as follows:

- The two operations must be serialized.

- If the close goes first then the open is treated as an open of a new stream.

- If the open goes first then the close simply decrements the open count and
  otherwise leaves the stdata structure intact.

There are factors that complicate the simple serialization of these two
operations.  They are as follows:

1) A STREAMS driver open procedure can sleep, as can a driver`s close procedure.
   This means that either of these two operations can take in indefinite period
   of time.

2) (1) means that a global semaphore cannot be used to serialize the two
   operations.  The semaphore might be held indefinitely due to sleeps in
   driver open and close routines.

3) A clone open, or other driver open activity, can change the {major,minor}
   device that the stream represents.  This leads, in effect, to a re-open
   of the newly designated stream.

The synchronization of these operations involves the following general
mechanisms.

i) The lis_stdata_sem is a semaphore that protects the linked list of stdata
   structures from alteration.  This list is searched at open time to compare
   the {maj,min} device id being opened with the device ids of all existing
   open streams.  By this mechanism the open routine discovers that the
   requested open should use an existing stdata structure.  If no such struct
   is found in the list then a new one is allocated, placed in the list and
   tagged with the {maj,min} of the device being opened, all under protection
   of the lis_stdata_sem.  This semaphore cannot be held indefinitely.

ii) The hd->sd_opening semaphore is used to exclude the open and close routine
    (or multiple opens) for a particular stream.  This semaphore exists within
    the stdata structure for the stream and thus can be held indefinitely if an
    open or a close routine sleeps.

iii) The stdata structures contain a reference count, not to be confused with
     the open count which count the number of outstanding references to the
     structure.  Whenever any operation, such as open, close, read, write, is
     in process on an stdata structure the reference count is incremented for
     the duration of the operation.  When the reference count goes to zero then
     the structure is deallocated.

iv) The mechanism in (iii) is used to allow the stdata structure to contain
    state information concerning whether it is in the process of being opened
    or closed and that state information can persist after the stream has been
    closed.  The information is valid as long as some routine has an
    outstanding reference to the stdata structure.

v) Mechanisms (ii), (iii) and (iv) allow the open routine to discover that it
   has lost a race with the close routine and work from a newly allocated
   structure instead of using the now-closed structure that was originally
   found in the linked list.

The races that need to be resolved can be characterized at a high level by the
following matrix:


	     | Open   | Close  |
    ---------+--------+--------+
     Open    |   A    |   C    |
    ---------+--------+--------+
     Close   |   C    |   B    |
    ---------+--------+--------+

A) Open vs Open

   In this case one of the opens gets the stdata_sem first and gets to search
   the list of streams.  If it finds the dev then the open process consists of
   incrementing the open count, otherwise it allocates the stdata structure.

   The second open will find the stdata structure and proceed on the path of
   incrementing the open count.

   When reopening a stream the open routine of every module in the stream is
   called.  It is assumed in this case that the device id will not change.

B) Close vs Close

   Both the stdata_sem and the sd_opening semaphores are acquired by the close
   routine.  Only the last close executes the close_action procedure on the
   stream.

   In the cast of the "last close", the close routine sets the STRCLOSE flag in
   the stdata structure.  This flag prevents the stdata structure from being
   found in a search of the linked list for a new stream.  Thus, the open count
   of the stream can never increase again once it has been decremented to zero
   by the close routine.

   The close_action routine dismantles the stream.

C) Open vs Close

   The open routine cannot find a stream in the stdata list if it is in the
   process of being dismantled by the close routine.  This is because the
   STRCLOSE flag makes such a stream invisible, even if the stdata structure
   itself remains allocated.  Thus, the open routine is working with a newly
   allocated stdata or one that is not in the middle of a final close.

   However, there is one tricky problem with open.  Due to (i), above, it is
   not allowed to hold the stdata_sem indefinitely.  However, due to (ii),
   above, the sd_opening semaphore may be held indefinitely.  This means that
   the open routine must not be holding the stdata_sem when it goes to acquire
   the sd_opening semaphore.  Releasing the sdata_sem allows another open or a
   close to execute prior to acquiring the sd_opening semaphore.

   Thus, the open routine must be able to tell which of three cases occurred
   once it has the sd_opening semaphore:

   1. A final close occurred.
   2. Another open occurred.
   3. Neither occurred.

   Detecting #1 is simply a matter of checking the STRCLOSE flag once the
   sd_opening semaphore is acquired.  If this flag is set then the stdata is
   being closed.  This instance of the stdata structure is abandoned and the
   open procedure repeats from the beginning, searching for an existing stdata
   or allocating a new one.

   If case #2 occurred the open procedure that executed first will have
   incremented the open count for the stdata structure.  The second open will
   detect this and follow the short path through the code.

   Case #3 is when the open count still equals zero after acquiring the
   sd_opening semaphore.  This leads to the long form of the open involving
   building the stream, etc.

   The close routine really has an easier time of it.  After acquiring the
   sd_opening semaphore it simply decrements the open count and looks to see if
   it reached zero.  If so, it sets the STRCLOSE flag and releases the
   sd_opening semaphore.  (It acquires the stdata_sem for the duration of
   setting the STRCLOSE flag.)  The STRCLOSE flag will prevent the open routine
   from attempting to use this stdata structure.

   Obviously, if the open routine performed its entire function before the
   close routine is entered then the close routine will find the open count to
   be non-zero and will not dismantle the stream.

*/


/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */


/* LiS implementation modules used */

#include <sys/stream.h>
#ifdef LINUX_POLL
#define	USE_LINUX_POLL_H	1
#include <linux/poll.h>
#else
#include <sys/poll.h>
#endif
#include <sys/LiS/errmsg.h>
#if !defined(ERANGE) && defined(LINUX)
#undef _ERRNO_H
#define __need_Emath 1
#include <errnos.h>
#endif
#include <sys/lismem.h>			/* for lis_free_all_pages */
#include <sys/osif.h>
#include <sys/cmn_err.h>

#include <sys/LiS/modcnt.h>		/* for LIS_MODGET/LIS_MODPUT */

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/* LINUX versions of the following are all now in linux-mdep.h */
#ifndef LINUX
#define D_COUNT(d)		1
#define FILE_D_COUNT(f)		1
#define MNTSYNC()		do {} while (0)
#define FILE_MNTGET(f)		0
#define MNT_COUNT(m)    	0
#endif

/*
 * Defines for measuring time intervals
 *
 * Each routine that is measuring its execution time will have a local
 * variable called "time_cell".  The measuring is enabled by a debug
 * bit.
 *
 * time_cell is declared as an unsigned long initialized to zero.
 */
#define	CLOCKON()	do { \
    if (LIS_DEBUG_MEAS_TIME) time_cell -= lis_hitime(); } while (0)

#define	CLOCKADD()	do { \
    if (LIS_DEBUG_MEAS_TIME) time_cell += lis_hitime(); } while (0)

#define	CLOCKOFF(item)	do { \
    if (LIS_DEBUG_MEAS_TIME) \
	 LisSetCounter(item, time_cell += lis_hitime()); } while (0)

/*
 *  timing functions - these are available outside the kernel also,
 *  depending on how lis_gettimeofday is defined.
 */

/*
 *  lis_hitime() -
 *
 *  This timer is restricted to a cycle of approx. 64 seconds.
 */
unsigned long _RP lis_hitime(void)
{
    struct timeval	 tv ;

    lis_gettimeofday(&tv);
    return( (tv.tv_sec & 0x3f) * 1000000 + tv.tv_usec ) ;

} /* lis_hitime */

/*
 *  lis_usecs() -
 *
 *  this is similar to lis_hitime, but it isn't restricted to cycling
 *  at 64 seconds.  This has considerable semantic advantage; namely,
 *  it allows simpler comparison of times crossing a single cycle
 *  boundary because the full range of the underlying type is used.
 */
unsigned long _RP lis_usecs(void)
{
    struct timeval tv;

    lis_gettimeofday(&tv);
    return( (tv.tv_sec * 1000000L) + tv.tv_usec );
}

/*
 *  lis_msecs() -
 *
 *  millisecond-resolution clock similar otherwise to lis_usecs().
 */
unsigned long _RP lis_msecs(void)
{
    struct timeval tv;

    lis_gettimeofday(&tv);
    return( (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L) );
}

/*
 *  lis_dsecs() -
 *
 *  decisecond(1/10th)-resolution clock similar otherwise to lis_usecs().
 *
 *  this is included because some protocols (which might be implemented as
 *  STREAMS modules) specify timer events at this resolution...
 */
unsigned long _RP lis_dsecs(void)
{
    struct timeval tv;

    lis_gettimeofday(&tv);
    return( (tv.tv_sec * 10L) + (tv.tv_usec / 100000L) );
}

/*
 *  lis_secs() -
 *
 *  seconds-resolution clock similar otherwise to lis_usecs().
 */
unsigned long _RP lis_secs(void)
{
    struct timeval tv;

    lis_gettimeofday(&tv);
    return(tv.tv_sec);
}

/*
 * Macro to do a putnext from the stream head.
 *
 * Do not call this macro while holding any locks since lis_runqueues may
 * call the queue runner, which may want locks that you are holding.  Nested
 * locks will probably save you, but best just not to do it.
 */
#define	PUTNEXT(q,m)				\
	do {					\
	    lis_putnext( (q), (m) ) ;		\
	    lis_runqueues();			\
	} while (0)

/*
 * Macro to determine whether there are any processes waiting on
 * a poll for a given stream.  When using kernel version 2.1 style
 * polling the queue head is a Linux specific structure, not our
 * portable poll list head.
 */
#if defined(PORTABLE_POLL)
#define	POLL_WAITING(hdp)	( (hdp)->sd_polllist.ph_list != NULL )
#elif defined(LINUX_POLL)
#define	POLL_WAITING(hdp)	( waitqueue_active(&(hdp)->sd_task_list) )
#else
#error "Either PORTABLE_POLL or LINUX_POLL must be defined"
#endif

#define	POLL_NOT_WAITING(hdp)	( ! POLL_WAITING(hdp) )

/*  -------------------------------------------------------------------  */
/*
 * This counter is used in keeping the code path tables and also in the
 * lock tracing.  It allows one to determine the relative order of 
 * traced events for forensic analysis.
 */
int		 lis_seq_cntr ;		/* used to establish ordering */

/*  -------------------------------------------------------------------  */
/*
 * Code path tracing.
 *
 * This is a compile-time option that is used for debugging LiS.
 */
typedef struct lis_code_path
{
    char	*fcn ;
    char	*file ;
    int		 line ;
    int		 cpu ;
    void	*ptr ;			/* arbitrary entry */
    long	 arg ;			/* arbitrary entry */
    int		 cntr ;			/* sequence counter */

} lis_code_path_t ;

#if defined(CONFIG_DEV)
#define	USE_CODE_PATH	1
#else
#undef	USE_CODE_PATH
#endif

#if defined(USE_CODE_PATH)

#define CP_SIZE		2048
lis_code_path_t		lis_code_path_tbl[CP_SIZE] ;
lis_code_path_t		*lis_code_path_ptr = lis_code_path_tbl ;
lis_spin_lock_t		lis_code_path_lock ;
static char		*lis_cp_fmt =		/* for printk */
			    "%u:CPU%u in %s() %s #%d -- %p 0x%lx\n";

#if defined(LINUX)
#define CPCPU	smp_processor_id()
#else
#define CPCPU	0
#endif

#define CPFL(p,a,func,f,l)						\
    do {								\
	  lis_flags_t psw ;						\
	  lis_code_path_t *pp ;						\
	  lis_spin_lock_irqsave(&lis_code_path_lock,&psw) ;		\
	  pp = lis_code_path_ptr++ ;					\
	  if (lis_code_path_ptr == &lis_code_path_tbl[CP_SIZE])		\
	    lis_code_path_ptr = lis_code_path_tbl ;			\
	  lis_spin_unlock_irqrestore(&lis_code_path_lock,&psw) ;	\
	  pp->fcn  = (char *)(func) ;					\
	  pp->file = (char *)(f) ;					\
	  pp->line = (l) ;						\
	  pp->cpu  = CPCPU ;						\
	  pp->ptr  = (void *)(p) ;					\
	  pp->arg  = (long)(a) ;					\
	  pp->cntr = ++lis_seq_cntr ;					\
	  if (LIS_DEBUG_CP)						\
	      printk(lis_cp_fmt, pp->cntr, CPCPU,			\
		      		(func),(f),(l),(void *)(p),(a));	\
       } while (0)

#if !defined(__GNUC__) && !defined(HAS__FUNCTION__)
#define	__FUNCTION__	""
#endif

#define CP(p,a)		CPFL((p),(a),__FUNCTION__,__LIS_FILE__,__LINE__)

#else

#define CPFL(p,a,fc,f,l)	do {;} while (0)
#define CP(p,a)			do {;} while (0)

#endif

#if defined(CONFIG_DEV)
void	lis_cpfl(void *p, long a, const char *fcn, const char *f, int l)
{
    CPFL(p,a,fcn,f,l) ;
}
#else
#define	lis_cpfl(p,a,fcn,f,l)
#endif

/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

lis_spin_lock_t	 lis_stdata_lock ;	/* protects global tables */
lis_spin_lock_t	 lis_qhead_lock ;	/* protects qhead, qtail & scanq */
lis_spin_lock_t	 lis_incr_lock ;	/* protects certain variable incrs */
extern lis_spin_lock_t	 lis_queue_contention_lock ;

volatile struct queue	*lis_qhead; /* first scheduled queue	*/
volatile struct queue	*lis_qtail; /* last scheduled queue		*/
volatile struct queue	*lis_scanqhead;	/* head of STREAMS scan queue	*/
volatile struct queue	*lis_scanqtail;	/* tail of STREAMS scan queue	*/
lis_atomic_t	 lis_in_syscall ;	/* processing a system call */
lis_atomic_t	 lis_queues_running ;	/* queues are being run now */
lis_atomic_t	 lis_runq_req_cnt ;	/* # req's to run queue runners */
int		 lis_num_cpus ;
lis_atomic_t	 lis_runq_cnt ;		/* # of qrun threads running */
volatile unsigned long lis_runq_cnts[LIS_NR_CPUS] ;
volatile unsigned long lis_queuerun_cnts[LIS_NR_CPUS] ;
volatile unsigned long lis_setqsched_cnts[LIS_NR_CPUS] ;
volatile unsigned long lis_setqsched_isr_cnts[LIS_NR_CPUS] ;
lis_atomic_t	 lis_runq_active_flags[LIS_NR_CPUS] ;
int		 lis_runq_pids[LIS_NR_CPUS] ;
volatile unsigned long	 lis_runq_wakeups[LIS_NR_CPUS] ;

int		 lis_nstrpush;	/* maximum # of pushed modules */
int		 lis_strhold;	/* if not zero str hold feature's activated*/
unsigned long    lis_strthresh;	/* configurable STREAMS memory limit */
unsigned int     lis_iocseq;	/* ioctl id */
stdata_t	*lis_stdata_head ;	/* list of stdata structures */
lis_atomic_t	 lis_stdata_cnt ;	/* counts stdata structs in use */
lis_atomic_t	 lis_open_cnt ;	        /* count opens */
lis_atomic_t	 lis_close_cnt ;        /* count closes */
lis_atomic_t     lis_doclose_cnt;
lis_semaphore_t  lis_stdata_sem;        /* stdata list semaphore */
lis_semaphore_t  lis_close_sem;         /* single threads closes */
lis_q_sync_t	 lis_queue_sync;         /* single threads q running */
lis_atomic_t	 lis_putnext_flag ;	/* prevents svc procs during putnext */
int		 lis_negstat ;		/* stat went negative */

extern long	 lis_max_mem ;		/* maximum to allocate */
extern long	 lis_max_msg_mem ;	/* maximum to allocate */
extern lis_spin_lock_t	  lis_msg_lock ;/* msg.c */
extern lis_spin_lock_t	  lis_bc_lock ; /* buffcall.c */
extern lis_spin_lock_t	  lis_mem_lock ;/* strmdbg.c */
extern lis_spin_lock_t	  lis_tlist_lock ; /* dki.c */
extern lis_atomic_t       lis_spin_lock_count ;
extern lis_atomic_t       lis_spin_lock_contention_count ;

#if defined(LINUX)
mblk_t *lis_lock_contention_msg(void) ;	/* lislocks.c */
mblk_t *lis_sem_contention_msg(void) ;	/* lislocks.c */
mblk_t *lis_queue_contention_msg(void) ;/* queue.c */
#endif



/*  -------------------------------------------------------------------  */
/*			   Streams Queue Structures                      */
/*
 * The stream head itself acts as a streams driver (sort-of).  So
 * it needs to have qinit structures just like other drivers.
 *
 * TBD:  We need a better way to set low and high water marks.
 * TBD:  Also packet sizes.
 */

struct module_info	strmhd_rdminfo =
		{0			/* mi_idnum */
		,"str_rput"		/* mi_idname */
		,LIS_MINPSZ		/* mi_minpsz */
		,LIS_MAXPSZ		/* mi_maxpsz */
		,0xFFFF			/* mi_hiwat */
		,0xF000			/* mi_lowat */
		} ;

struct module_info	strmhd_wrminfo =
		{0			/* mi_idnum */
		,"str_wput"		/* mi_idname */
		,LIS_MINPSZ		/* mi_minpsz */
		,LIS_MAXPSZ		/* mi_maxpsz */
		,0xFFFF			/* mi_hiwat */
		,0xF000			/* mi_lowat */
		} ;

struct module_stat	strmhd_rdmstat = { }; /* all counters == 0 */

struct module_stat	strmhd_wrmstat = { }; /* all counters == 0 */

struct qinit	strmhd_rdinit =
		{lis_strrput		/* qi_putp */
		,lis_strrsrv		/* qi_srvp */
		,NULL			/* qi_qopen */
		,NULL			/* qi_qclose */
		,NULL			/* qi_qadmin */
		,&strmhd_rdminfo	/* qi_minfo */
		,&strmhd_rdmstat	/* qi_mstat */
		} ;

struct qinit	strmhd_wrinit =
		{NULL			/* qi_putp */
		,&lis_strwsrv		/* qi_srvp */
		,NULL			/* qi_qopen */
		,NULL			/* qi_qclose */
		,NULL			/* qi_qadmin */
		,&strmhd_wrminfo	/* qi_minfo */
		,&strmhd_wrmstat	/* qi_mstat */
		} ;

struct streamtab strmhd_info = {
    &strmhd_rdinit,         /* read queue init */
    &strmhd_wrinit,         /* write queue init */
    NULL,
    NULL
};

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

/*  -------------------------------------------------------------------  */

extern int  lis_await_qsched(stdata_t *hd, queue_t *q);
extern void lis_qdetach(queue_t *q, int do_close, int flag, cred_t *creds) ;
extern void _RP lis_wakeup_close(caddr_t arg) ;
static void check_for_wantenable(stdata_t *hd);
static int  lis_strdoioctl(struct file *f, stdata_t *hd,
			    strioctl_t *ioc, cred_t *creds,
			    int do_copyin) ;
int lis_doclose(struct inode *,struct file *,stdata_t *, cred_t *);
static void lis_free_stdata( struct stdata *hd ) ;
static void lis_deallocate_polllist( stdata_t *hd ) ;
static void deschedule(queue_t *q) ;
static void lis_check_m_sig(stdata_t *hd) ;
static stdata_t * lis_alloc_stdata(void) ;
static void lis_tear_down_stream(stdata_t *hd) ;
static int  get_sd_opening_sem(stdata_t *head) ;
static void release_sd_opening_sem(stdata_t *head) ;

#ifdef LINUX
extern int  lis_assign_inode_to_file(struct file *f, struct inode *i) ;
extern void lis_start_qsched(void) ;
#endif						/* routine in linux-mdep.c */

extern void lis_show_inode_aliases(struct inode *);
extern int  lis_sleep_on_close_wt(void *q_str) ;	/* wait.c */
extern void lis_wakeup_close_wt(void *q_str) ;

extern void lis_put_rput_q(stdata_t *hd, mblk_t *mp);	/* queue.c */
extern mblk_t *lis_get_rput_q(stdata_t *hd);

/*  -------------------------------------------------------------------  */
/*                          lis_head_get				 */
/*
 * Increment the reference count for the passed stream head structure.
 * If you pass a NULL pointer it allocates a stream head structure and
 * returns a pointer to it with ref count set to 1.  Return of NULL
 * value means allocation failure.
 *
 * There is something of a race between this routine and the put routine
 * concerning the count.  It should not matter since if one thread is
 * putting the structure with count going from 1-->0 then it is closing
 * the stream.  Our thread cannot have a reference to the stream in that
 * case so we can't be trying to "get" the same stream.
 *
 * This routine should be called any time some usage of the stream head is
 * pending, not just at open time.  That is, the stream head needs to be
 * "gotten" for read, write, getmsg, putmsg and messages flowing upstream into
 * the stream head.  "put" calls will balance these out.
 *
 * Thus, when the use count goes to zero there really are no more possible
 * usages of this stream head structure and it can be deallocated.
 */
#if defined(CONFIG_DEV)
stdata_t *lis_head_get_fcn(stdata_t *hd, const char *file, int line)
#else
stdata_t *lis_head_get_fcn(stdata_t *hd)
#endif
{
#if !defined(CONFIG_DEV)
    static char *file = "head.c" ;
    static int   line = 0 ;
#endif
    stdata_t *ihd = hd;

    if (hd == NULL)
	hd = lis_alloc_stdata() ;

    if (hd != NULL)
	K_ATOMIC_INC(&hd->sd_refcnt);

    if (LIS_DEBUG_OPEN || LIS_DEBUG_CLOSE || LIS_DEBUG_REFCNTS)
    {
	if (!hd)
	    printk("lis_head_get(NULL) => NULL {%s@%d}\n",
		   file, line) ;
	else if (!ihd)
	    printk("lis_head_get(NULL) >> h@0x%p/++%d/%d \"%s\" {%s@%d}\n",
		   hd, LIS_SD_REFCNT(hd), LIS_SD_OPENCNT(hd), hd->sd_name,
		   file, line) ;
	else
	    printk("lis_head_get(h@0x%p/++%d/%d) \"%s\" {%s@%d}\n",
		   hd, LIS_SD_REFCNT(hd), LIS_SD_OPENCNT(hd), hd->sd_name,
		   file, line) ;
    }

    return(hd) ;
}

/*  -------------------------------------------------------------------  */
/*                          lis_head_put				 */
/*
 * Decrements the reference count for the passed stream head structure.
 * If the count reaches zero then the structure is no longer needed and
 * it is deallocated.  This deallocation amounts to the second half of
 * the close process for the associated stream and involves actually
 * tearing down the stream.
 *
 * Return the original pointer of the structure is still valid.  Return
 * NULL if the count hits zero and the structure is deallocated.
 *
 * We lock the head prior to the testing of the ref count so that
 * another thread decrementing simultaneously will have to wait until
 * we have made our decisions before proceeding.  This allow the orderly
 * sequencing of the ref count, for example, down from 2-->1 by us and
 * for the other thread to notice that the count has reached 1 and
 * perform the deallocation.  If both threads do the atomic_dec at the
 * same time the count could go to zero without the structure being
 * deallocated.
 */
#if defined(CONFIG_DEV)
stdata_t *lis_head_put_fcn(stdata_t *hd, const char *file, int line)
#else
stdata_t *lis_head_put_fcn(stdata_t *hd)
#endif
{
#if !defined(CONFIG_DEV)
    static char *file = "head.c" ;
    static int   line = 0 ;
#endif
    if (hd)
    {
	int oldcnt ;
	int tear_down ;

	tear_down = ((oldcnt = LIS_SD_REFCNT(hd)) == 1) ;
	K_ATOMIC_DEC(&hd->sd_refcnt);

	if (LIS_DEBUG_OPEN || LIS_DEBUG_CLOSE || LIS_DEBUG_REFCNTS ||
	    oldcnt <= 0)
	    printk("lis_head_put(h@0x%p/%s%d/%d) \"%s\" {%s@%d}\n",
		   hd, (oldcnt>0?"--":""), LIS_SD_REFCNT(hd),
		   LIS_SD_OPENCNT(hd), hd->sd_name,
		   file, line) ;

	if (tear_down)
	{
	    lis_tear_down_stream(hd) ;
	    hd = NULL ;
	}
    } else
	if (LIS_DEBUG_OPEN || LIS_DEBUG_CLOSE)
	    printk("lis_head_put(NULL) {%s@%d}\n", file, line);

    return(hd) ;
}

/*  -------------------------------------------------------------------  */
/*			    lis_incr					 */
/*
 * Increment an integer under spin lock protection and return its 
 * incremented value.  Useful for allocating unique numbers for
 * ioctls, etc.
 */
int	lis_incr(int *intp)
{
    int		ret ;

    lis_spin_lock(&lis_incr_lock) ;
    ret = ++(*intp) ;
    lis_spin_unlock(&lis_incr_lock) ;
    return(ret) ;
}

/*  -------------------------------------------------------------------  */
/*                          lis_down_nintr				 */
/*
 * Do a "down" operation, but don't return until it either succeeds
 * or fails for some reason other than EINTR.
 */
int	lis_down_nintr(lis_semaphore_t *sem)
{
    int		rslt ;

    while ((rslt = lis_down(sem)) == -EINTR) ;
    return(rslt) ;
}

int	lis_down_nintr_head(stdata_t *hd, lis_semaphore_t *sem)
{
    int		rslt ;

    lis_clear_and_save_sigs(hd);		/* undo pending signals */
    while ((rslt = lis_down(sem)) == -EINTR)
	lis_clear_and_save_sigs(hd);		/* undo pending signals */
    lis_restore_sigs(hd);			/* restore old signals */
    return(rslt) ;
}


/*  -------------------------------------------------------------------  */
/*                            lis_clone_major				 */
int _RP lis_clone_major(void)
{
    return(LIS_CLONE) ;		/* rtn major of clone device */
}

/*  -------------------------------------------------------------------  */
/*                              str_mux_cycle				 */

#if 0
	This whole mux-cycle business, as I see it, is completely
	artificial.  In order to construct a cycle you would have
	to do something like the following:

	    ioctl(fd2, I_LINK, fd3) ;		2-->3
	    ioctl(fd1, I_LINK, fd2) ;		1-->2-->3
	    ioctl(fd3, I_LINK, fd1) ;		3-->1-->2-->3 (cycle)

	However, you can''t do this because the third ioctl will
	fail.  fd3 has its STPLEX flag set so the ioctl is disallowed.

	--DMG

/* returns non-zero if the given link leads to cyclic graph
 */
static int str_mux_cycle(stdata_t *from, stdata_t *to)
{
    (void)from;
    (void)to;
    return(0);			/* no graph detection */
}
#endif

/*  -------------------------------------------------------------------  */
/*
 * new_muxid
 *
 * Get a new muxid.  Ensure that it is not in use already.
 */
static int
new_muxid(void)
{
    static int		 muxid ;
    int			 my_id ;
    stdata_t		*hd ;
    int			 n ;


    for (;;)
    {
	while ((my_id = lis_incr(&muxid)) < 0) muxid = 0 ; /* keep positive */

	if (lis_stdata_head == NULL) return(my_id) ;

	lis_spin_lock(&lis_stdata_lock) ;
	for (hd = lis_stdata_head, n = K_ATOMIC_READ(&lis_stdata_cnt);
	     n > 0 || hd != lis_stdata_head;
	     hd = hd->sd_next, n--)
	{
	    if (hd->sd_mux.mx_index == my_id) break ;	/* in-use */
	}

	lis_spin_unlock(&lis_stdata_lock) ;
	if (n == 0)			/* whole list, no match */
	    break ;			/* can use this one */
    }

    return(my_id) ;

} /* new_muxid */

/* 
 * Link the muxed stream under hd.  Return the l_index value.
 * 
 * Use a global running muxid so that if the same driver has multiple
 * control streams it can have unique muxids for its lowers without
 * having to know which control stream goes with which lower.
 */
static int
str_link_mux( stdata_t *hd, stdata_t *muxed, int cmd)
{
    muxed->sd_mux.mx_cmd   = cmd ;	/* save cmd used to link */
#if 0
    muxed->sd_mux.mx_index = ++hd->sd_l_index ;
#else
    muxed->sd_mux.mx_index = new_muxid() ;
#endif
    muxed->sd_mux.mx_next  = hd->sd_mux.mx_hd ;

    hd->sd_mux.mx_hd = muxed ;

    return(muxed->sd_mux.mx_index) ;

}/*str_link_mux*/


/*  -------------------------------------------------------------------  */
/* 
 * This routine unlinks the given multiplexor from the given
 * stream head.  It returns >= 0 for success, < 0 for failure.
 *
 * The parameters 'i', 'f' and 'head' refer to the control stream
 * of the multiplexor.  This stream may be open and we are unlinking
 * a specific lower indexed by l_index, or it may be closing and we
 * are unlinking all lower streams which were i_linked via this
 * control stream.
 *
 * Any mux's below hd whose reference count is currently 1 will
 * be closed.  For any such mux's we recursively unlink any mux's
 * below those.
 *
 * We do not close hd.  We decrement its ref cnt if the command
 * is I_PUNLINK.
 */
static  int
lis_i_unlink(struct inode	*i,
	     struct file	*f,
	     stdata_t		*hd,
	     int		 l_index,
	     int		 cmd,
	     cred_t		*creds)
{
    stdata_t		 *hp ;
    stdata_t		 *prev;
    stdata_t		 *next;
    queue_t		 *qtop ;
    strioctl_t		  ioc;
    linkblk_t		  lnk;
    int			  err ;
    int			  rtn = 0 ;
    lis_flags_t		  psw1, psw2 ;
    unsigned long  	  time_cell = 0 ;

    CLOCKON() ;
    for (hp = hd->sd_mux.mx_hd, prev = NULL, next = NULL; 
	 hp != NULL; 
	 hp = next)
    {
	next = hp->sd_mux.mx_next ;		/* link to next stream */

	if (l_index >= 0 && hp->sd_mux.mx_index != l_index)
	{
	    prev = hp ;
	    continue ;				/* skip this one */
	}

	if (cmd == I_UNLINK && hp->sd_mux.mx_cmd == I_PLINK)
	{
	    prev = hp ;
	    continue ;				/* skip this one */
	}

	/*
	 * Get permission from driver before unlinking
	 */
	for (qtop = hd->sd_wq; qtop; qtop = qtop->q_next)
	{
	    if (!SAMESTR(qtop)) break ;
	}

	lnk.l_qtop   = qtop;
	lnk.l_qbot   = hp->sd_wq;
	lnk.l_index  = hp->sd_mux.mx_index ;

			    /* note I_UNLINK vs I_PLINK eliminated above */
	CP(hd,lnk.l_index) ;
	if (hp->sd_mux.mx_cmd == I_LINK)
	    ioc.ic_cmd = I_UNLINK ;		/* linked w/I_LINK */
	else
	    ioc.ic_cmd = I_PUNLINK;		/* linked w/I_PLINK */

	ioc.ic_timout= LIS_LNTIME;
	ioc.ic_len   = sizeof(linkblk_t);
	ioc.ic_dp    = (char*)&lnk;

	CP(hp,hp->sd_rq) ;
	err = lis_await_qsched(hp, hp->sd_rq) ;
	if (err < 0)
	    goto error_rtn ;

	CP(hp,hp->sd_rq) ;
	CLOCKADD() ;
	if (   (err = lis_strdoioctl(f,hd,&ioc,creds,0)) < 0
	    && cmd == I_UNLINK			/* ignore errs for I_PUNLINK */
	   )
	{
	    CLOCKON() ;
error_rtn:
	    CP(hd,err) ;
	    rtn = err ;				/* will return error */
	    if (l_index < 0)			/* OK, skip this one */
	    {					/* lose the memory?? */
		prev = hp ;
		continue ;
	    }

	    CLOCKOFF(IOCTLTIME) ;
	    return(rtn);			/* can't unlink */
	}

	CLOCKON() ;
	/* set frozen flag so as to defer puts and qenables */
	lis_freezestr(hp->sd_rq) ;
	if (hd->sd_mux.mx_hd == hp)		/* delinking elt at hd */
	    hd->sd_mux.mx_hd = hp->sd_mux.mx_next ;
	else					/* delinking elt down in list */
	    prev->sd_mux.mx_next = hp->sd_mux.mx_next ;

	hp->sd_mux.mx_next = NULL ;		/* clobber link */

	CP(hd,hp) ;
	if ( LIS_DEBUG_IOCTL || LIS_DEBUG_LINK )
	{
	  printk(
	      "strioctl: I_UNLINK: ctl stream %s muxid=%d\n",
		    hd->sd_name,
		    lnk.l_index) ;
	  printk(
	      "                    l_qtop=\"%s\" l_qbot=\"%s\"->\"%s\"\n",
		    lis_queue_name(lnk.l_qtop),
		    hp->sd_name,
		    lis_queue_name(hp->sd_wq->q_next)) ;
	}

						/* restore queues */
	err = lis_set_q_sync(hp->sd_rq, LIS_QLOCK_QUEUE) ; /* strm head queue */
	if (err < 0)
	    goto error_rtn ;			/* fairly ungraceful */

	lis_setq(hp->sd_rq, &strmhd_rdinit, &strmhd_wrinit);
	LIS_QISRLOCK(hd->sd_wq, &psw1) ;
	LIS_QISRLOCK(hd->sd_rq, &psw2) ;
	hp->sd_wq->q_str	= hp ;		/* point q back to strm hd */
	hp->sd_rq->q_str	= hp ;		/* assume mux used these ptrs */
	hp->sd_rq->q_flag      |= QNOENB ;	/* restore flag */
	LIS_QISRUNLOCK(hd->sd_rq, &psw2) ;
	LIS_QISRUNLOCK(hd->sd_wq, &psw1) ;

	if (   hp->sd_mux.mx_cmd == I_PLINK	/* was linked w/I_PLINK */
	    && LIS_SD_OPENCNT(hd) > 0		/* safety first */
	   )
        {
	    K_ATOMIC_DEC(&hd->sd_opencnt);	/* decr open count of head */
	    hd->sd_linkcnt-- ;			/* one less link for head */
	    if (hd->sd_inode)                   /* ASSERT: this is OK */
		lis_put_inode(hd->sd_inode);    /* one less inode ref */
	    lis_head_put(hd) ;			/* undo extra get */
        }

	hp->sd_linkcnt-- ;			/* one less I_LINK */
	CLR_SD_FLAG(hp,STPLEX);			/* no longer multiplexed */

	/* perform any deferred puts/qenables from plumbing.  We do
	 * this even if the queue is closing since close logic may
	 * want to exchange some messages with the driver.
	 */
	lis_unfreezestr(hp->sd_rq) ;

	if (LIS_SD_OPENCNT(hp) == 1)		/* about to close */
	{
	    /*
	     * Close the lower stream.  The close routine may recurse
	     * on lis_i_unlink() if the lower stream is itself the
	     * control stream of a multiplexor.
	     *
	     * We do not have access to the file and inode of the
	     * lower stream.  Because the sd_opencnt is sitting at 1,
	     * the i_link is the only user of the stream.  The file
	     * and inode have been closed.
	     *
	     * lis_doclose will "put" the stdata structure, hp.
	     */
	    CLOCKADD() ;
	    lis_doclose(NULL, NULL, hp, creds) ;	/* close lower mux */
	    CLOCKON() ;
	}
	else
	if (LIS_SD_OPENCNT(hp) > 1)			/* be safe */
	{
	    K_ATOMIC_DEC(&hp->sd_opencnt) ;	/* decr open cnt */
	    lis_head_put(hp) ;			/* reduce cnt on lwr stream */
	}
	else
	{
	    rtn = -EIO ;			/* if no other error */
	    printk("strioctl: I_UNLINK: "
		    "ctl stream %s muxid=%d lower stream %s "
		    "bad open count %d\n",
		    hd->sd_name, lnk.l_index, hp->sd_name,
		    LIS_SD_OPENCNT(hp) - 1) ;
	}

	lis_head_put(hd) ;			/* decr ctl strm use cnt */
	if (l_index >= 0) break ;		/* done */
    }

    CLOCKOFF(IOCTLTIME) ;
    return(rtn) ;

} /* lis_i_unlink */

/*  -------------------------------------------------------------------  */
/* lis_unlink_all
 *
 * This routine unlinks all the lowers from any control streams
 * that have the same major device number as the one for 'hd'.
 *
 * This is an AT&T feature in which the user uses I_PUNLINK to
 * construct a multiplexor and then exits, closing the control
 * stream.  Later, to dismantle the multiplexor, the user opens
 * just any old clone device to the top level of the multiplexor
 * and then issues an I_PUNLINK with the argument MUXID_ALL.
 * That is when this routine gets called.
 *
 * If it had been left up to me (DMG), I would have forced the user to
 * have a dedicated minor device for the control stream and 
 * re-open it for purposes of issuing the I_PUNLINK.  This clone
 * business opens a gaping security hole.
 *
 * Anyway, the technique is to traverse our linked list of all
 * stream heads looking for matches of major device number and
 * control stream-ness.  Perform an I_PUNLINK on all the qualified
 * stream heads, being careful about the list integrity since
 * streams heads being closed will get unlinked from the list.
 *
 * If we find a stream that matches, we effectively simulate the
 * user opening the stream (bump usage counts), doing the I_PUNLINK,
 * and then closing the stream.
 *
 * If there is an error unlinking the lowers then we really will
 * lose the memory that is allocated for the lower streams.  Chances
 * are that the module use counts will stay positive and LiS will not
 * be able to be unloaded.  Thus, it is not a good idea for a driver
 * to respond with an error to an I_PUNLINK ioctl.
 */
static  int
lis_unlink_all(struct inode	*i,
	     struct file	*f,
	     stdata_t		*hd,
	     cred_t		*creds)
{
    stdata_t		*hdp ;
    int			 n ;
    int			 maj ;
    int			 rtn = 0 ;
    int			 r ;

    maj = getmajor(hd->sd_dev) ;
again:
    if (lis_stdata_head == NULL)
	return(rtn) ;

    lis_spin_lock(&lis_stdata_lock) ;
    for (hdp = lis_stdata_head, n = 0;
	 n == 0 || hdp != lis_stdata_head;
	 hdp = hdp->sd_next, n++)
    {
	if (getmajor(hdp->sd_dev) != maj || hdp->sd_mux.mx_hd == NULL)
	    continue ;			/* not our major, or not a ctl stream */

	lis_spin_unlock(&lis_stdata_lock) ;	/* done w/stdata_head */
	K_ATOMIC_INC(&hdp->sd_opencnt);	/* simulate "open" */
	lis_head_get(hdp) ;
	if ((r = lis_i_unlink(i, f, hdp, MUXID_ALL, I_PUNLINK, creds)) < 0)
	    rtn = r ;			/* save error for return */

	hdp->sd_mux.mx_hd = NULL ;	/* just to be sure */
	lis_doclose(NULL, NULL, hdp, creds) ;	/* close the mux */
	goto again ;			/* start over at head of list */
    }

    lis_spin_unlock(&lis_stdata_lock) ;
    return(rtn) ;

} /* lis_unlink_all */


/*  -------------------------------------------------------------------  */
/* lis_i_link
 *
 * Called from strioctl to perform the I_LINK (I_PLINK) function.
 *
 * Note about I_PLINK:  This causes the refcnt of the top stream
 * to be incremented so that a close of that stream will not cause
 * the stream to be deallocated and the multiplexor dismantled.
 * However, if the stream does get closed then the stdata structure
 * will be laying around in memory with refcnt==1 and nothing holding
 * onto it.  It will be their until the next reboot.  Hopefully,
 * this multiplexor can take care of itself until then.
 *
 * "hd" is the control stream.  "muxed" is the lower stream.
 */
int	lis_i_link(struct inode	*i,
		   struct file	*f,
		   stdata_t	*hd,
		   int		 muxfd,
		   int		 cmd,
		   cred_t	*creds)
{
    stdata_t		*muxed;
    queue_t		*qtop ;
    linkblk_t		 lnk;
    strioctl_t		 ioc;
    lis_flags_t	 	 psw;
    int			 err ;
    unsigned long  	 time_cell = 0 ;

#define	RTN(v) do { CLOCKOFF(IOCTLTIME) ; return(v); } while (0)

    CLOCKON() ;
    if ( (muxed = lis_fd2str(muxfd)) == NULL )
	RTN(-EINVAL);

    lis_head_get(hd) ;
    for (qtop = hd->sd_wq; qtop; qtop = qtop->q_next)
    {
	if (!SAMESTR(qtop)) break ;
    }

    lnk.l_qtop = qtop ;
    lnk.l_qbot = muxed->sd_wq;
    lnk.l_index= str_link_mux(hd, muxed, cmd);

    CP(hd,muxed) ;
    if ( LIS_DEBUG_IOCTL || LIS_DEBUG_LINK )
    {
	printk("strioctl: I_LINK: ctl stream %s l_index=%d\n",
		hd->sd_name,
		lnk.l_index) ;
	printk("                  l_qtop=\"%s\" l_qbot=\"%s\"->\"%s\"\n",
		lis_queue_name(lnk.l_qtop),
		muxed->sd_name,
		lis_queue_name(muxed->sd_wq->q_next)
		) ;
    }

    LIS_QISRLOCK(muxed->sd_rq, &psw) ;
    muxed->sd_rq->q_flag &= ~QNOENB ;		/* this was set for strm hd q */
    LIS_QISRUNLOCK(muxed->sd_rq, &psw) ;

    CP(hd,muxed->sd_rq) ;
    err = lis_await_qsched(muxed, muxed->sd_rq) ;
    if (err < 0)
	goto error_rtn ;

    lis_setq(muxed->sd_rq,
	     hd->sd_strtab->st_muxrinit, hd->sd_strtab->st_muxwinit);

    ioc.ic_cmd	 = I_LINK;
    ioc.ic_timout= LIS_LNTIME;
    ioc.ic_len	 = sizeof(linkblk_t);
    ioc.ic_dp	 = (char*)&lnk;
    SET_SD_FLAG(muxed,STPLEX);
    lis_head_get(muxed) ;
    K_ATOMIC_INC(&muxed->sd_opencnt);		/* so close won't deallocate */
    muxed->sd_linkcnt++ ;			/* one more I_LINK */
    CP(hd,muxed->sd_rq) ;
    CLOCKADD() ;
    if ((err = lis_strdoioctl(f,hd,&ioc,creds,0)) < 0)
    {
	CLOCKON() ;
	K_ATOMIC_DEC(&muxed->sd_opencnt);	/* undo refcnt bumps */
	muxed->sd_linkcnt-- ;
error_rtn:
	CP(hd,err) ;
	CLR_SD_FLAG(muxed,STPLEX);		/* not multiplexed */
	lis_setq(muxed->sd_rq,			/* put back old queues */
	     hd->sd_strtab->st_rdinit, hd->sd_strtab->st_wrinit);
	hd->sd_mux.mx_hd = muxed->sd_mux.mx_next ;/* undo str_link_mux effect */
	muxed->sd_mux.mx_next = NULL ;		/* clobber link */

	LIS_QISRLOCK(muxed->sd_rq, &psw) ;	/* undo queue flags */
	muxed->sd_rq->q_flag |= QNOENB ;
	LIS_QISRUNLOCK(muxed->sd_rq, &psw) ;

	lis_head_put(muxed) ;
	lis_head_put(hd) ;
	RTN(err);
    }

    CLOCKON() ;

    /*
     * Set initial queue flags.
     *
     * We set the QWANTR flag of both sides of the queue so that
     * a putq into either queue will schedule the service procedure
     * to run.  The flag means that some service procedure "wants"
     * to "read" from the queue, that is, take messages out of it.
     *
     * Set the lower queue to have the same queue locking options
     * as the driver queue below it.
     */
    lis_freezestr(muxed->sd_rq) ;
    if ((err = lis_set_q_sync(muxed->sd_rq, qtop->q_qlock_option)) < 0)
    {
	lis_unfreezestr(muxed->sd_rq) ;
	goto error_rtn ;
    }

    lis_set_q_flags(QWANTR, 1, muxed->sd_wq, NULL);

    if (muxed->sd_mux.mx_cmd==I_PLINK)	/* linked w/I_PLINK */
    {
	lis_head_get(hd) ;
	K_ATOMIC_INC(&hd->sd_opencnt);/* so a close won't deallocate */
	hd->sd_linkcnt++ ;		/* count that as a link */
	if (hd->sd_inode)               /* ASSERT: this is OK */
	    lis_grab_inode(hd->sd_inode);  /* keep inode too */
    }

    /* perform any deferred puts/qenables from plumbing */
    lis_unfreezestr(muxed->sd_rq) ;

    CP(hd,lnk.l_index) ;
    CLOCKOFF(IOCTLTIME) ;
    return(lnk.l_index);

#undef RTN

} /* lis_i_link */


/*  -------------------------------------------------------------------  */
/* detach queues from stream and free the stream
 */
static void lis_dismantle(stdata_t *hd, cred_t *creds)
{
    queue_t	*q ;

    if (hd->sd_wq != NULL)			/* still have strm head queue */
    {
	q = hd->sd_rq ;
	if (!LIS_CHECK_Q_MAGIC(q)) return ;
	LisDownCounter(MSGQDSTRHD, lis_qsize(q)) ;
	lis_qdetach(q, 1, 0, creds);
	hd->sd_wq = NULL ;
	hd->sd_rq = NULL ;
    }

}


/*  -------------------------------------------------------------------  */

/* set read options
 */
static int
set_readopt(int *flags, int rmode)
{
    long oflags=*flags;

    switch(rmode & RMODEMASK){
    case RNORM:
    case RMSGD:
    case RMSGN:
	*flags &= ~RMODEMASK ;			/* clear mode bits */
	*flags |= (rmode & RMODEMASK) ;		/* set mode */
	break;
    default:
	*flags=oflags;				/* restore original */
	return(-EINVAL);
    }

    switch(rmode & RPROTMASK){
    case RPROTNORM:
    case RPROTDAT:
    case RPROTDIS:
	*flags &= ~RPROTMASK ;			/* clear proto bits */
	*flags |= (rmode & RPROTMASK) ;		/* set read proto mask */
	break;
    case 0:					/* not being set */
	break;
    default:
	*flags=oflags;				/* restore original */
	return(-EINVAL);
    }

    return(0);
}

/* These will let you know if you should block on rd/wr as dictated by the
 * stream head and file flags. They should be called when the process should
 * block due to no free space or no msg avail.
 * Return non-zero if you shouldn't block and zero if you should do block 
 */
#define should_notblock_rd(hd,fp) (F_ISSET(fp->f_flags,O_NONBLOCK))

#define should_notblock_wr(hd,fp)					\
		    ((fp->f_flags & (O_NONBLOCK|OLDNDELAY)) == O_NONBLOCK)

/*  -------------------------------------------------------------------  */

/* Will add q to the scan list.  This happens as a result of the
 * scanq timer expiring.
 *
 * One minor problem:  If the kernel is going to run the streams
 * queues before returning from the timer interrupt then we are OK.
 * If, however, it is up to us to run ourselves then we have two
 * choices:
 *	1) wait until some user process enters the kernel on streams business
 *	2) run the queues here on top of the clock interrupt
 * I don't like either choice.  It would be best of the kernel ran
 * the streams queues from strategic places such as just before returning
 * from interrupt processing and just before returning to user level.
 *
 * NOTE: STREAMS queues are run by waking up a process so that everything
 *       is orderly. -- DMG
 */
static void _RP
add_to_scanl( caddr_t arg )
{
  lis_flags_t	 psw, pswq;
  stdata_t	*hd = (stdata_t *) arg ;
  queue_t	*q ;

  /* We'll use a naive FIFO, this should be revisited
   */
  q = hd->sd_wq ;			/* stream head write queue */
  hd->sd_scantimer = 0 ;		/* note that timer is not running */
  LIS_QISRLOCK(q, &pswq) ;		/* prevent ISR operations */
  if (F_ISSET(q->q_flag,(QSCAN|QPROCSOFF))) {
      LIS_QISRUNLOCK(q, &pswq) ;
      return;
  }

  F_SET(q->q_flag,QSCAN);
  LIS_QISRUNLOCK(q, &pswq) ;

  lis_head_get(hd) ;			/* increase refcnt of hd */

  lis_spin_lock_irqsave(&lis_qhead_lock, &psw) ;
  if (lis_scanqtail) 
    lis_scanqtail->q_scnxt = q;
  else
    lis_scanqhead = q;
  q->q_scnxt=NULL;
  lis_scanqtail=q;
  K_ATOMIC_INC(&lis_runq_req_cnt) ;
  lis_spin_unlock_irqrestore(&lis_qhead_lock, &psw) ;

  return ;

}/*add_to_scanl*/

static void
start_scanl_timer(stdata_t *hd)
{
    if (hd->sd_scantimer == 0)
	hd->sd_scantimer = lis_timeout_fcn(add_to_scanl,
					   (caddr_t) hd,
					   SECS_TO(LIS_RTIME),
					   "scan-list", MEM_TIMER
					   );  

} /* start_scanl_timer */


/*  -------------------------------------------------------------------  */
/*
 * stdata list manipulations
 */
static void ins_stdata(stdata_t *head)
{
    lis_spin_lock(&lis_stdata_lock) ;
    K_ATOMIC_INC(&lis_stdata_cnt) ;	/* count stdata structs */
    if (lis_stdata_head == NULL)
    {
	head->sd_next   = head ;
	head->sd_prev   = head ;
	lis_stdata_head = head ;
    }
    else
    {
	head->sd_next = lis_stdata_head ;
	head->sd_prev = lis_stdata_head->sd_prev ;
	lis_stdata_head->sd_prev->sd_next = head ;
	lis_stdata_head->sd_prev = head ;
    }
    lis_spin_unlock(&lis_stdata_lock) ;
}

static void rem_stdata(stdata_t *head)
{
    if (head->sd_next == NULL || head->sd_prev == NULL)
	return ;				/* not in the list */

    lis_spin_lock(&lis_stdata_lock) ;
    if (lis_stdata_head != NULL)
    {
	if (lis_stdata_head == head)	     /* deleting head element */
	    lis_stdata_head = head->sd_next ;	/* move down one */

	if (lis_stdata_head == head)		/* only element */
	    lis_stdata_head = NULL ;		/* no more streams */
	else					/* link around "head" */
	{
	    head->sd_prev->sd_next = head->sd_next ;
	    head->sd_next->sd_prev = head->sd_prev ;
	}
    }
    K_ATOMIC_DEC(&lis_stdata_cnt) ;
    head->sd_next = NULL ;
    head->sd_prev = NULL ;
    lis_spin_unlock(&lis_stdata_lock) ;
}

/*  -------------------------------------------------------------------  */
/*
 * Allocate and initialize an stdata structure. Do not get any of
 * the locks, leave that up to the caller.
 */
static stdata_t *
lis_alloc_stdata(void)
{
    queue_t	*q = lis_allocq("stream-head");
    stdata_t	*head ;

    if (!q)
    {
	if (LIS_DEBUG_OPEN)
	    printk("lis_alloc_stdata() - "
		   "failed to allocate queues for stream head\n");
	return NULL;
    }

    if (lis_set_q_sync(q, LIS_QLOCK_NONE) < 0)
    {
	lis_freeq(q) ;
	return(NULL) ;
    }

    head = (stdata_t*) LIS_HEAD_ALLOC(sizeof(stdata_t), "stream-head ") ;
    if (head == NULL)
    {
	lis_freeq(q) ;
	return(NULL) ;
    }

    lis_bzero(head, sizeof(*head)) ;	/* clear to zero */
    head->magic=STDATA_MAGIC;

    lis_spin_lock_init(&head->sd_lock, "LiS_Head_Lock") ;

    lis_sem_init(&head->sd_close_wt, 0) ; /* to wait for drainage */
    lis_sem_init(&head->sd_wioc, 1) ;
    lis_sem_init(&head->sd_wiocing, 0) ;
    lis_sem_init(&head->sd_wread, 0) ;		/* explicit init for emphasis */
    lis_sem_init(&head->sd_read_sem, 1) ;
    lis_sem_init(&head->sd_wwrite, 0) ;		/* explicit init for emphasis */
    lis_sem_init(&head->sd_write_sem, 1) ;
    lis_sem_init(&head->sd_closing, 0) ;	/* explicit init for emphasis */
    lis_sem_init(&head->sd_opening, 1) ;

#if defined(LINUX_POLL)
    init_waitqueue_head(&head->sd_task_list) ;
#endif
    head->sd_wq  = WR(q);
    head->sd_rq  = RD(q);
    head->sd_rq->q_str = head->sd_wq->q_str = head;
    head->sd_rq->q_flag |= QNOENB ;		/* no implicit qenable */

    LIS_MODGET();
    LisUpCount(MODUSE);

    /*
     * Link stdata structure into master list
     */
    ins_stdata(head) ;

    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
	printk("lis_alloc_stdata() >> h@0x%p/%d/%d\n",
	       head, LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head));

    CP(head,0) ;
    return(head) ;

}/*lis_alloc_stdata*/

/*  -------------------------------------------------------------------  */
/* Releases the stream head in the i-node
 *
 * We have to wait until here to remove the stdata structure from
 * the list because sometimes a stream gets "closed" having been
 * I_PLINKed and hangs around in memory.  Later it has to be findable
 * so that it can be opened again and I_UNLINKed.
 */
static void
lis_free_stdata( struct stdata *hd )
{
    mblk_t	*mp ;

    if (LIS_DEBUG_CLOSE || LIS_DEBUG_REFCNTS)
	printk("lis_free_stdata(h@0x%p/%d/%d) \"%s\"\n",
	       hd,
	       (hd?LIS_SD_REFCNT(hd):0),(hd?LIS_SD_OPENCNT(hd):0),
	       (hd&&hd->sd_name?hd->sd_name:""));

    /* must assert here that every pointer in the stdata struct which
     * may hold dynamic memory is NULL.
     */
    if (hd == NULL || hd->magic != STDATA_MAGIC) return ;

    CP(hd,0) ;
    rem_stdata(hd) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues() ;
    K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */

    /*
     *  if sd_from is set, we're holding the inode it references;
     *  release it now
     */
    if (hd->sd_from)
    {
	if (LIS_DEBUG_CLOSE || LIS_DEBUG_REFCNTS)
	    printk("    >> h@0x%p \"%s\" releasing sd_from %c@0x%p/%d\n",
		   hd, hd->sd_name,
#if defined(LINUX)
		   'd', hd->sd_from, D_COUNT(hd->sd_from)
#else
		   'i', hd->sd_from, I_COUNT(hd->sd_from)
#endif
		);
#if defined(LINUX)
	lis_dput(hd->sd_from);
	if (hd->sd_mount)
	    MNTPUT((struct vfsmount *)hd->sd_mount);
	hd->sd_mount = NULL;
#else
	lis_put_inode(hd->sd_from);
#endif
	hd->sd_from = NULL;
    }

    if (hd->sd_close_timer)
	untimeout(hd->sd_close_timer) ;

    SEM_DESTROY(&hd->sd_close_wt) ;	/* de-initialize the semaphores */
    SEM_DESTROY(&hd->sd_wioc) ;
    SEM_DESTROY(&hd->sd_wiocing) ;
    SEM_DESTROY(&hd->sd_wread) ;
    SEM_DESTROY(&hd->sd_read_sem) ;
    SEM_DESTROY(&hd->sd_wwrite) ;
    SEM_DESTROY(&hd->sd_write_sem) ;
    SEM_DESTROY(&hd->sd_closing) ;
    SEM_DESTROY(&hd->sd_opening) ;

    if (hd->sd_wmsg != NULL)
    {
	freemsg(hd->sd_wmsg) ;
	hd->sd_wmsg = NULL ;
    }

    if (hd->sd_iocblk != NULL)
    {
	freemsg(hd->sd_iocblk) ;
	hd->sd_iocblk = NULL ;
    }

    if (hd->sd_siglist != NULL)
	lis_free_elist(&hd->sd_siglist);

    while ((mp = hd->sd_rput_hd) != NULL)	/* flush rput list */
    {
	hd->sd_rput_hd = mp->b_next ;		/* link around message */
	freemsg(mp) ;
	LisDownCount(MSGQDSTRHD) ;		/* one fewer msg queued */
    }

    lis_deallocate_polllist(hd) ;

    hd->magic ^= 1 ;			/* clobbers the magic number */
    LIS_HEAD_FREE(hd) ;			/* give it back */

    LIS_MODPUT();
    LisDownCount(MODUSE);

}/*lis_free_stdata*/


/*  -------------------------------------------------------------------  */

/* Will check that inode & device are a proper STREAMS device
 *
 * Note:  This whole thing used to be a macro but the linker did
 *        something strange to the code that caused it not to
 *        work properly.  Making it into a procedure is an attempt
 *        to do an end-run around this problem. - DMG
 *
 * Anything that goes wrong here is cause for a message.  None of these
 * conditions should be able to happen.
 */
#define CHECK_INO(i,n) do {						\
			    int		rslt ;				\
									\
			    if ((rslt = lis_check_ino(i,n)) < 0)	\
				return(rslt) ;				\
			} while (0)

int	lis_global_major ;
int lis_check_ino(struct inode *i, char *msg)
{
    int		maj ;

    if (!i)
    { 					
	printk("lis_check_ino: called w/NULL inode from %s\n", msg); 	
	return(-EBADF); 			
    }						

    if (!S_ISCHR(i->i_mode)) 			
    {
	printk("lis_check_ino: i_mode=0%o is not char special\n", i->i_mode);
	return(-ENODEV);			
    }

    maj = getmajor(GET_I_RDEV(i)) ;
    lis_global_major = maj ;
    if (!LIS_DEVOK(maj))	
    {
	printk("lis_check_ino: major=%u minor=%u is not STREAMS\n",
		maj, getminor(GET_I_RDEV(i))) ;
	return(-ENXIO);
    }

    return(0) ;

} /* lis_check_ino */

/*  -------------------------------------------------------------------  */

/* copyin the buffer shown in usr strbuf struct sb, into *m.
 * As it's now it just copies the whole usr bufs to single blks.
 */
static  int
copyin_msgpart( struct file *f, stdata_t *hd, mblk_t **m, char *sb, int wroff )
{
    strbuf_t kbuf;
    int err;

    *m = NULL ;					/* NULL msg ptr by default */
    if ((err=lis_check_umem(f,VERIFY_READ,sb,sizeof(strbuf_t)))<0)
	return(err);

    						/* fetch strbuf */
    if ((err = lis_copyin(f,&kbuf,sb,sizeof(strbuf_t))) < 0)
	return(err) ;

    if (kbuf.len < 0 || kbuf.buf == NULL)	/* don't include this msg part */
	return(0) ;				/* not an error */

    /* This prevents an oops when the stream is hung up */
    if (F_ISSET(hd->sd_flag,STRCLOSE))
    {
        if (LIS_DEBUG_PUTMSG)
          printk("copyin_msgpart: stream %s: stream is closed or closing\n",
                  hd->sd_name) ;
        return(-EIO);
    }

    /* bb - When hung up a stream head must return ENXIO, this is clear */
    if (F_ISSET(hd->sd_flag,STRHUP))
    {
        if (LIS_DEBUG_PUTMSG)
          printk("copyin_msgpart: stream %s: stream has received M_HANGUP\n",
                  hd->sd_name) ;
        return(-ENXIO);
    }

    /* The if and else if should never happen. But if the conditions
    ** should occur the user should know immediately.
    */
    if(!hd->sd_wq)
	printk("**ERROR --- Stream head has no write queue %lx\n",(long)hd);
    else if (!hd->sd_wq->q_next)
	printk( "**ERROR --- No driver associated stream head %lx q=%lx\n",
        				(long)hd, (long)hd->sd_wq);
    else if (   kbuf.len < (int)hd->sd_wq->q_next->q_minpsz
	     || (   hd->sd_wq->q_next->q_maxpsz != INFPSZ
		 && kbuf.len > (int)hd->sd_wq->q_next->q_maxpsz 
	        )
	    )
	return(-ERANGE);

    if ( (*m=(mblk_t*)allocb(kbuf.len+wroff,BPRI_LO))==NULL)
	return(-ENOMEM);

    (*m)->b_rptr = ((*m)->b_wptr += wroff);
    if (kbuf.len>0) {
	if ((err=lis_check_umem(f,VERIFY_READ,kbuf.buf,kbuf.len))<0)
	    return(err);
	if ((err = lis_copyin(f,(*m)->b_wptr,kbuf.buf,kbuf.len)) < 0)
	    return(err);
	(*m)->b_wptr+=kbuf.len;
    }
    return(0);
}/*copyin_msgpart*/

/*  -------------------------------------------------------------------  */
/* copyin ctl & dat intro M_PROTO+M_DATA msg
 */
static int
copyin_msg(struct file *f, stdata_t *hd,
		mblk_t **m, strbuf_t *uctl, strbuf_t *udat)
{
    int err;
    mblk_t *aux = NULL;

    if (m == NULL) return(-EINVAL) ;

    *m = NULL ;					/* initially */
    if ( uctl != NULL )
    {
	if ((err=copyin_msgpart(f,hd,m,(char*)uctl,0))<0)
	{
	    if (*m != NULL)
		lis_freeb(*m);
	    *m = NULL ;
	    return(err);
	}

	if (*m != NULL)
	    lis_btype(*m) = M_PROTO;
    }
    else
	*m = NULL ;				/* no ctl msg part */

    if ( udat != NULL )
    {
	if ((err=copyin_msgpart(f,hd,&aux,(char*)udat,hd->sd_wroff))<0)
	{
	    if (*m != NULL)
		lis_freeb(*m);

	    *m = NULL ;
	    return(err);
	}
    }

    if (*m == NULL && aux == NULL)		/* no message to send */
	return(0) ;

    if (*m != NULL && aux != NULL)		/* have 2 msgs to send */
	lis_linkb(*m,aux);			/* link them */
    else
    if (*m == NULL)				/* have no ctl msg */
	*m = aux ;				/* rtn just data msg */

    return(0);
	
}/*copyin_msg*/

/*  -------------------------------------------------------------------  */

/* copyout part of msg.
 * It will assume it's ctl and return MORECTL/MOREDATA if more data is left.
 * msg won't be touched if !doit 
 * this function doesn't free any block, just adjust b_[rw]ptr's
 * is_ctl is true for the ctl part / false for the data part.
 *
 * 'm' can be NULL which means do not copy anything, but update
 * the user's strbuf.
 */
static int
copyout_msgpart( struct file *f, mblk_t *m,
		    strbuf_t *sb, strbuf_t *usb, int is_ctl, int doit)
{
    int err;
    int rtn = 0 ;
    int chunk ;
    int mlen = 0 ;		/* # of bytes for ctl or dta part */
    int size_set = 0 ;
    int maxlen = sb->maxlen;	/* how much the user wants */
    int gotsome= 0 ;
    int mtype = -1 ;
    int rtnlen ;

    if (doit)			/* usb points to user space */
    {				/* ensure writeable */
	if (usb && (err=lis_check_umem(f,VERIFY_WRITE,usb,sizeof(strbuf_t)))<0)
	  return(err);
	if (maxlen >= 0 && (err=lis_check_umem(f,VERIFY_WRITE,sb->buf,maxlen))<0)
	  return(err);
    }
    else			/* usb is in kernel space */
    {				/* but sb->buf is in user space */
	if (maxlen >= 0 && (err=lis_check_umem(f,VERIFY_WRITE,sb->buf,maxlen))<0)
	  return(err);
    }

    for (rtnlen=0;
	 m != NULL;
	 m=m->b_cont)
    {
	chunk = 0 ;
	mtype = lis_btype(m) ;
	if (size_set == 0)				/* size not yet set */
	{
	    if (   (!is_ctl && mtype == M_DATA)		/* hit data msg */
		|| ( is_ctl && mtype != M_DATA))	/* hit ctl msg */
	    {
		mlen = lis_xmsgsize(m);			/* set length */
		size_set = 1 ;
	    }

	    if (size_set == 0) continue ;		/* still looking */
	}

	if (is_ctl && mtype == M_DATA)			/* hit data msg */
	    break ;					/* quit */

	if (!is_ctl && mtype != M_DATA)			/* non-data msg */
	    continue ;					/* keep looking */

	gotsome = 1 ;					/* blk of correct type*/
	if (maxlen < 0)					/* no place for it */
	    break ;					/* stop looking */

	chunk=lis_min(maxlen,lis_mblksize(m));
	if (chunk > 0 && (err=lis_copyout(f,m->b_rptr,sb->buf+rtnlen,chunk))<0)
	    return(err) ;

	if (doit)
	  lis_adjmsg(m,chunk);

	 rtnlen  += chunk ;
	 maxlen  -= chunk ;
	 if (maxlen <= 0)
	    break ;
    }					/*for blk*/

    err = 0 ;				/* return value */
    if (gotsome)			/* got some bytes from msg */
    {
	sb->len = rtnlen ;
	if (sb->len < mlen)		/* less than what was in msg */
	    rtn = (is_ctl?MORECTL:MOREDATA);
    }
    else
    if (mtype >= 0)			/* saw at least one msg block */
	sb->len = -1 ;			/* no blocks of that type */
    /* else leave sb->len unmodified from caller's value */

    if (doit)
    {
	if (usb != NULL)
	    err = lis_copyout(f,sb,usb,sizeof(strbuf_t));
    }
    else
	*usb = *sb ;			/* update kernels rtn structure */

    return(err < 0 ? err : rtn);

}/*copyout_msgpart*/

/*  -------------------------------------------------------------------  */
/* copyout msg to usr buffers given by ctl & dta.
 * m should be a proto+data msg.
 * this function doesn't free any block, just adjust b_[rw]ptr's
 * msg won't be touched if !doit 
 */
static int
copyout_msg( struct file *f, mblk_t *m, strbuf_t *kctl, strbuf_t *kdta,
	     strbuf_t *uctl, strbuf_t *udta, int doit)
{
    int errctl,errdta;
    if ((errctl=copyout_msgpart(f,m,kctl,uctl,1,doit))<0)
	return(errctl);
    else if ((errdta=copyout_msgpart(f,m,kdta,udta,0,doit))<0)
	return(errdta);
    return(errctl|errdta);
}/*copyout_msg*/



/*  -------------------------------------------------------------------  */
/*Returns non-zero if we should hold msg on write at the stream head.
 */
#define strholding(sd)	(lis_strhold && F_ISSET((sd)->sd_flag,STRHOLD))

/*  -------------------------------------------------------------------  */
/* lis_await_qsched
 *
 * Waits until the indicated queue is no longer being run.
 */
int lis_await_qsched(stdata_t *hd, queue_t *q)
{
    lis_flags_t	psw_rq, psw_wq ;
    int		rslt ;
    queue_t	*rq ;
    queue_t	*wq ;
    lis_semaphore_t	wakeup_sem ;

    wq = WR(q);
    rq = RD(q) ;

    if (!((rq->q_flag | wq->q_flag) & (QENAB | QSCAN | QRUNNING)))
	return(0) ;

    lis_sem_init(&wakeup_sem, 0);
    rslt = 0 ;
    LIS_QISRLOCK(wq, &psw_wq) ;
    LIS_QISRLOCK(rq, &psw_rq) ;
    wq->q_wakeup_sem = &wakeup_sem ;
    rq->q_wakeup_sem = &wakeup_sem ;
    F_SET(rq->q_flag, QWAITING) ;
    F_SET(wq->q_flag, QWAITING) ;
    while (   ((rq->q_flag | wq->q_flag) & (QENAB | QSCAN | QRUNNING))
	   && rslt == 0
	  )
    {
	CP(hd,rq) ;
	LIS_QISRUNLOCK(rq, &psw_rq) ;
	LIS_QISRUNLOCK(wq, &psw_wq) ;

	lis_clear_and_save_sigs(hd);		/* undo pending signals */
	K_ATOMIC_DEC(&lis_in_syscall) ;		/* "done" with a system call */
	lis_runqueues() ;
	if (hd != NULL)
	{
	    lis_down_nosig(&wakeup_sem) ;
	    rslt = 0 ;
	}
	else
	    rslt = lis_down(&wakeup_sem) ;
	lis_restore_sigs(hd);			/* restore old signals */

	K_ATOMIC_INC(&lis_in_syscall) ;		/* processing a system call */
	LIS_QISRLOCK(wq, &psw_wq) ;
	LIS_QISRLOCK(rq, &psw_rq) ;
    }
    CP(hd,rslt) ;
    F_CLR(rq->q_flag, QWAITING) ;
    F_CLR(wq->q_flag, QWAITING) ;
    wq->q_wakeup_sem = NULL ;
    rq->q_wakeup_sem = NULL ;
    LIS_QISRUNLOCK(rq, &psw_rq) ;
    LIS_QISRUNLOCK(wq, &psw_wq) ;
    lis_sem_destroy(&wakeup_sem) ;
    return(rslt) ;
}


/*  -------------------------------------------------------------------  */
/* qdetach - unlink queue from stream, close driver/module, and free queue
 */
void 
lis_qdetach(queue_t *q, int do_close, int flag, cred_t *creds)
{
    lis_flags_t	psw_rq, psw_wq ;
    lis_flags_t psw_prq, psw_pwq ;	/* for locking previous queue */
    lis_flags_t psw_nrq, psw_nwq ;	/* for locking next queue */
    int		prq_locked = 0 ;
    int		pwq_locked = 0 ;
    int		nrq_locked = 0 ;
    int		nwq_locked = 0 ;
    int		rslt ;
    int		wq_rslt ;
    int		rq_rslt ;
    int		closing_pipe = 0 ;
    const char	*name = NULL;
    queue_t     *next_q, *prev_q;
    queue_t	*unfreeze_q ;		/* queue to unfreeze */
    queue_t	*rq ;
    queue_t	*wq ;
    stdata_t	*hd ;
    stdata_t	*hd_peer = NULL ;
    unsigned long  time_cell = 0 ;

    CLOCKON() ;
    if (q == NULL)				/* supposed to be read queue */
    {
	CLOCKOFF(CLOSETIME) ;
	return;
    }

    wq = WR(q);
    rq = RD(q) ;
    if (rq == NULL || wq == NULL)
    {
	CLOCKOFF(CLOSETIME) ;
	return;
    }

    hd = (stdata_t *) q->q_str ;
    if (hd != NULL && hd->magic != STDATA_MAGIC)
    {
	printk("lis_qdetach: %s head=%p NULL or bad magic: 0x%lx\n",
		lis_queue_name(q), hd, hd ? hd->magic : 0L) ;
	hd = NULL ;
    }

    lis_freezestr(rq) ;
    /*
     * freezestr will freeze the entire stream.  It will cross the midpoint
     * of a pipe and freeze all the way to the remote stream head queue.
     * If there is a previous queue we will save its pointer to use for
     * unfreezing, otherwise the next queue.
     */
    if (   (unfreeze_q = rq->q_next) == NULL
	&& (unfreeze_q = wq->q_next) == rq
       )
	unfreeze_q = NULL ;

    /*
     * We want to know if we are closing the stream head queue of a 
     * pipe.  If so then we need to be on the lookout for the twist
     * in the stream and to disassociate the two ends.
     *
     * We may be simply popping a module off of one end of a pipe.  That
     * case is treated normally since it might be just an I_POP.
     */
    if (rq == hd->sd_rq && (hd_peer = hd->sd_peer) && hd_peer != hd)
    {
	closing_pipe = 1 ;
	if (LIS_DEBUG_CLOSE)
	    printk("lis_qdetach: stream %s closing pipe, peer %s\n",
		    hd->sd_name, hd_peer->sd_name) ;
    }

    /*
     * Lock ordering:  when getting both the queue lock and the qhead_lock, get
     * the queue lock first.  See lis_qenable which needs the qhead_lock but is
     * always called with the queue lock being held already.
     *
     * Ignore error returns from lis_lockq().
     * In case of error we want to be selective about unlocking.
     */
    wq_rslt = lis_lockq(wq) ;			/* lock the write queue */
    rq_rslt = lis_lockq(rq) ;			/* lock the read queue */

    CP(rq,0) ;
    CP(wq,0) ;
    						/* excludes queuerun action */
    LIS_QISRLOCK(wq, &psw_wq) ;
    LIS_QISRLOCK(rq, &psw_rq) ;
    wq->q_flag |= QPROCSOFF ;			/* no more put/svc */
    rq->q_flag |= QPROCSOFF ;			/* no more put/svc */
    LIS_QISRUNLOCK(rq, &psw_rq) ;
    LIS_QISRUNLOCK(wq, &psw_wq) ;

    /*
     * Since we have the QPROCSOFF bits set it is now safe to let queuerun have
     * the queue lock if it has just removed this queue from the queue list.
     * It will notice the QPROCSOFF bits and not do anything.  We have to
     * release these locks before calling deschedule() so as to prevent a
     * deadly embrace with lis_qhead_lock.
     *
     * We need to not be holding any locks when we call the close routine
     * below.  The close routine may sleep, and sleeping while holding a
     * spinlock is a no-no.
     */
    if (rq_rslt == 0)
	lis_unlockq(rq) ;
    if (wq_rslt == 0)
	lis_unlockq(wq) ;

    /*
     * SVR4 STREAMS manual says to ignore close routine in the wq structure.
     */
    if (!LIS_CHECK_Q_MAGIC(rq) || !LIS_CHECK_Q_MAGIC(wq))
    {
	lis_unfreezestr(rq) ;
	return ;				/* lose the memory */
    }


    if (LIS_DEBUG_CLOSE)
    {
	name = lis_queue_name(q) ;
	if (name == NULL)
	    name = "Unknown-Q" ;
    }

    if (do_close && rq->q_qinfo != NULL && rq->q_qinfo->qi_qclose != NULL)
    {
	if (LIS_DEBUG_CLOSE)
	    printk("lis_qdetach(q@0x%p,?%d,...) \"%s\" >> calling close(...)\n",
		   rq, do_close, name);

	CP(rq,0) ;
	CLOCKADD() ;		/* user routine might sleep */
	/* call w/queue locked and marked as closing */
	K_ATOMIC_DEC(&lis_in_syscall) ;	/* "done" with a system call */
	lis_runqueues() ;
	(*rq->q_qinfo->qi_qclose)(rq, (rq->q_next ? 0 : flag), creds);
	K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
	CLOCKON() ;
    }

    /*
     * Now that the qprocs have been turned off and the close routine called
     * we need to ensure that the queues are not scheduled -- that is,
     * not in any scheduling list.  If we remove them from the list then
     * they won't be called.
     */
    deschedule(rq) ;			/* uses lis_qhead_lock */

    /*
     * If either queue service procedure is running on another CPU at this very
     * instant then we need to wait until it finishes execution.  This is the
     * case when the QRUNNING, QENAB and QSCAN flags are reset.
     *
     * The q_wakeup_sem is used by queuerun to wake us up if the QWAITING
     * flag is set.
     *
     * Signal consideration:  Because this is qdetach it is somewhat likely
     * that the process that we are running on has been signalled.  We want the
     * task structure signals to reflect this after we are done.  But in the
     * meantime we need to clear the signals so that we can sleep on a
     * semaphore without being "awakened" by signals that were raised
     * previously.
     */
    rslt = lis_await_qsched(hd, q) ;
    if (rslt < 0)
	printk("lis_qdetach(q@0x%p,...) "
	       "    >> lis_down(&wakeup_sem) >> <ERROR %d>\n",
	       rq, rslt) ;

    if (hd != NULL)
    {
	CLR_SD_FLAG(hd, STRFLUSHWT) ;
    }

    /*
     * Now everybody that needs to know should know that the queue
     * is going away, and no drivers or service procedures should be
     * messing with it.  It is safe to do the rest with the queues
     * locked, but without interrupt protection.
     *
     * Ignore error in locking the queue.
     */

    lis_lockq(wq) ;				/* lock the write queue */
    lis_lockq(rq) ;				/* lock the read queue */

    lis_flushq(wq, FLUSHALL);
    lis_flushq(rq, FLUSHALL);

    next_q = wq->q_next ;
    prev_q = rq->q_next ;

    if (name && LIS_DEBUG_CLOSE)
	printk("lis_qdetach(q@0x%p,...) \"%s\" >> "
		"wq=%p rq=%p next=%p unfrzq=%p prev=%p\n",
		q, name, wq, rq, next_q, unfreeze_q, prev_q) ;
    /*
     *  if this queue is last in a FIFO/pipe stream, the next queue
     *  from a write queue will be a read queue.  We handle that
     *  case below.
     *
     *  Lock previous and next queues as appropriate.
     *
     *  A word about lis_set_q_flags:  We set the QOPENING bit to cause
     *  puts into these queues to defer.  If we are called from pop_mod
     *  then there will be a call to check_for_wantenable() to follow
     *  shortly which will reset these bits and perform the deferred
     *  puts.  If we are called from close we are dismantling the 
     *  stream; closaction has already taken place so we don't care
     *  if the bits remain on.
     */
    if (next_q == rq)		/* FIFO linked to self */
    {
	/* all there is to lock is our queue, which we don't need to
	 * lock since we have the last reference to it except for
	 * the list pointers, and the previous queue.
	 */
       	next_q = prev_q;
	if (prev_q)
	{
	    LIS_QISRLOCK(prev_q, &psw_prq) ;
	    LIS_QISRLOCK(WR(prev_q), &psw_pwq) ;
	    prq_locked = 1 ;
	}
    }
    else
    {
	/* We need to lock the previous and next queues for link changing. */
	if (prev_q)
	{
	    LIS_QISRLOCK(prev_q, &psw_prq) ;
	    LIS_QISRLOCK(WR(prev_q), &psw_pwq) ;
	    prq_locked = 1 ;
	    pwq_locked = 1 ;
	}
	if (next_q)
	{
	    LIS_QISRLOCK(next_q, &psw_nrq) ;
	    LIS_QISRLOCK(OTHER(next_q), &psw_nwq) ;
	    nrq_locked = 1 ;
	    nwq_locked = 1 ;
	}
    }

    if (next_q) {
	if (name && LIS_DEBUG_CLOSE)
	    printk("lis_qdetach(q@0x%p,...) \"%s\""
		   " >> linking around write queue\n",
		   wq, name);
	OTHER(next_q)->q_next = prev_q;
    }
    if (prev_q) {
	if (name && LIS_DEBUG_CLOSE)
	    printk("lis_qdetach(q@0x%p,...) \"%s\""
		   " >> linking around read queue\n", rq, name);
	WR(prev_q)->q_next = next_q;
    }

    /*
     * Nobody can see the queues anymore but us.  Unlock them so that
     * the kernel sees that we are not holding any locks.
     */
    lis_unlockq(rq) ;
    lis_unlockq(wq) ;

    if (nwq_locked)	LIS_QISRUNLOCK(OTHER(next_q), &psw_nwq) ;
    if (nrq_locked)	LIS_QISRUNLOCK(next_q, &psw_nrq) ;
    if (pwq_locked)	LIS_QISRUNLOCK(WR(prev_q), &psw_pwq) ;
    if (prq_locked)	LIS_QISRUNLOCK(prev_q, &psw_prq) ;

    if (name && LIS_DEBUG_CLOSE)
	printk("lis_qdetach(q@0x%p,...) \"%s\" >> deallocating queues\n",
	       rq, name);

    CP(rq,0) ;
    lis_freeq(rq);			/* frees both sides of the queue */
    /*
     * We have frozen the stream from our detached queue downward.  Having
     * unlinked our queue we now need to unfreeze the rest of the stream
     * below us.
     *
     * In the case of a pipe the unfreeze_q may very well be the read
     * queue on the other side of the pipe.  By now the linkage back to
     * our own 'hd' has been severed.  We unfreeze the other end and then
     * check our own head to see if it is still frozen.  If so then we need
     * a head_put to balance what freezestr did.
     */
    if (unfreeze_q)
    {
	CP(unfreeze_q,unfreeze_q->q_str) ;
	lis_unfreezestr(unfreeze_q) ;	/* will balance lis_freezestr */
	if (F_ISSET(hd->sd_flag, STRFROZEN))
	{
	    CLR_SD_FLAG(hd, STRFROZEN) ;
	    lis_head_put(hd) ;		/* balance lis_freezestr */
	}
    }
    else
    {
	CP(hd,0) ;
	lis_head_put(hd) ;		/* balance lis_freezestr */
    }

    if (LIS_DEBUG_CLOSE)
	printk("lis_qdetach: closing_pipe=%d hd=%p hd->sd_peer=%p\n",
		closing_pipe, hd, hd_peer) ;

    if (closing_pipe)
    {					/* balance head_get in lis_get_pipe */
	int	flag ;

	CP(hd, hd_peer) ;
	lis_spin_lock_irqsave(&hd_peer->sd_lock, &psw_rq) ;
	hd_peer->sd_peer = hd_peer ;
	flag = hd_peer->sd_flag ;
	if (flag & STRFROZEN)
	    hd_peer->sd_flag &= ~STRFROZEN ;
	lis_spin_unlock_irqrestore(&hd_peer->sd_lock, &psw_rq) ;
	lis_head_put(hd_peer) ;
	if (flag & STRFROZEN)
	    lis_head_put(hd_peer) ;	/* balance freezestr */

	hd->sd_peer = hd ;
	lis_head_put(hd) ;
    }

    CLOCKOFF(CLOSETIME) ;

}/*lis_qdetach*/


/*  -------------------------------------------------------------------  */
/*
 *  lis_qopen() - calls the open routine associated with the read queue
 *  of q's pair (i.e., q may be a write queue).
 */
static int
lis_qopen( queue_t *q, dev_t *devp, int flag, cred_t *credp )
{
    queue_t	*rq = RD(q);
    int		 sflag;
    dev_t	 dev = MKDEV( 0,0 );
    const char	*name = NULL;
    int		 err;

    if (!(rq->q_qinfo))  {	/* ensure open routine */
	if (LIS_DEBUG_OPEN)
	    printk( "qopen: \"%s\" does not have qinfo\n",
		    lis_queue_name(rq) );
	return (-ENOPKG);			/* pkg not installed */
    }

    if (!(rq->q_qinfo->qi_qopen))  return (0);  /* open not required */

    if (!devp)  devp = &dev;

    sflag = (WR(rq)->q_next ? MODOPEN : DRVOPEN);

    if (LIS_DEBUG_OPEN)  name = lis_queue_name(q);
    if (name && LIS_DEBUG_OPEN)
	printk("lis_qopen(q@0x%p,...) "
	       "calling %s \"%s\" open @0x%p (dev=%x,flag=%x,sflag=%x)\n",
	       q, (sflag==MODOPEN?"module":"driver"), name,
	       rq->q_qinfo->qi_qopen,
	       *devp, flag, sflag);
    
    if ((err = (*(rq->q_qinfo->qi_qopen)) ( rq, devp,
					    flag, sflag, credp )) != 0)  {
	if (LIS_DEBUG_OPEN)
	    printk("lis_qopen(q@0x%p,...) \"%s\" >> error %d\n",
		   q, lis_queue_name(q), err);
	return (err < 0 ? err : -err);
    } else
	return (0);
}
/*
 *   lis_qattach - set up a queue pair for driver/module
 */
static int
lis_qattach( stdata_t *head, struct streamtab *info, dev_t *devp,
	     int flags, cred_t *credp )
{
    queue_t    *newq, *newwq;
    queue_t    *q = (head ? (head->sd_rq ? head->sd_rq : NULL) : NULL);
    queue_t    *rq, *wq ;
    lis_flags_t	psw1, psw2, psw3, psw4;
    const char *name = (info ?
			(info->st_rdinit ? 
			 (info->st_rdinit->qi_minfo ?
			  info->st_rdinit->qi_minfo->mi_idname
			  : NULL) : NULL) : NULL);
    int         err;

    if (!q)  return -EIO;

    if (!name)  name = "unnamed-queue";

    if (!(newq = lis_allocq(name)))  {
	if (LIS_DEBUG_OPEN)
	    printk("qattach: failed to allocate queue for \"%s\"\n", name );
	return -ENOMEM;
    }

    /* Set up the queue interface stuff.
     * Do this before SPL to avoid nested SPLs.
     */
    if (info != NULL)
	lis_setq(newq, info->st_rdinit, info->st_wrinit);

    rq = q ;
    wq = WR(q) ;
    newwq = WR(newq) ;
    LIS_QISRLOCK(rq, &psw1) ;
    LIS_QISRLOCK(wq, &psw2) ;
    LIS_QISRLOCK(newq, &psw3) ;
    LIS_QISRLOCK(newwq, &psw4) ;

    /* insert the queue, newq points to the read side of the queue */
    newq->q_next = rq;
    newwq->q_next = (wq->q_next == rq ? newq : wq->q_next);
    if (wq->q_next)
    	OTHER(newwq->q_next)->q_next = newq;
    wq->q_next = newwq;
    newq->q_str = newwq->q_str = head;

    /*
     * Set initial queue flags.
     *
     * We set the QWANTR flag of both sides of the queue so that
     * a putq into either queue will schedule the service procedure
     * to run.  The flag means that some service procedure "wants"
     * to "read" from the queue, that is, take messages out of it.
     *
     * QOPENING is set for a new queue and must be turned off by the
     * open routine.
     */
    F_SET(newq->q_flag,QWANTR|QOPENING);
    F_SET(newwq->q_flag,QWANTR|QOPENING);	/* yes, even tho it's the wrq */

    LIS_QISRUNLOCK(newwq, &psw4) ;
    LIS_QISRUNLOCK(newq, &psw3) ;
    LIS_QISRUNLOCK(wq, &psw2) ;
    LIS_QISRUNLOCK(rq, &psw1) ;

    if ((err = lis_qopen( newq, devp, flags, credp )) < 0)
	lis_qdetach( newq, 0, 0, credp );

    return(err);

}/*lis_qattach*/

/*  -------------------------------------------------------------------  */
/* Push this module in this stream and call its open proc.
 */
static int
push_mod( stdata_t *head, streamtab_t *mod, int id,
	  dev_t *devp, int flags, cred_t *creds )
{
    queue_t		*lrq, *lwq ;
    int			 err;

    if (head->sd_maxpushcnt && head->sd_pushcnt == head->sd_maxpushcnt)
	return(-ENOSR);

    lwq = head->sd_wq->q_next ;
    lrq = OTHER(lwq) ;

    lis_freezestr(head->sd_rq) ;

    if (   (err = lis_await_qsched(head, head->sd_rq)) < 0
	|| (err = lis_await_qsched(head, lrq)) < 0
       )
	goto return_error ;

    if ((err = lis_qattach( head, mod, devp, flags, creds )) < 0)
	goto return_error ;

    if ((err = lis_set_q_sync(head->sd_wq->q_next,
			      lis_fmod_sw[id].f_qlock_option)) < 0)
	goto return_error ;

    head->sd_pushcnt++;
    lis_fmod_sw[id].f_count++ ;

return_error:

    lis_unfreezestr(head->sd_rq) ;

    return(err);

}/*push_mod*/

/*  -------------------------------------------------------------------  */
/* Pop the top module off this stream and call its close proc.
 */
static int
pop_mod( stdata_t *head, int flags, cred_t *creds )
{
    modID_t	id ;
    int		err ;
    const char	*name ;

    if (!head->sd_pushcnt)
	return(-EINVAL);

    head->sd_pushcnt--;

    if (   !head->sd_wq->q_next
	|| !head->sd_wq->q_next->q_qinfo
	|| !head->sd_wq->q_next->q_qinfo->qi_minfo
	|| !(name = head->sd_wq->q_next->q_qinfo->qi_minfo->mi_idname)
       )
	return(-EINVAL);

    if ((id = lis_findmod(name)) > 0)	/* mod name to mod id */
	lis_fmod_sw[id].f_count-- ;

    /*
     * We want to set the QFROZEN flag in three queues.  The stream head
     * queue, the queue for the module below and the queue for the module/
     * driver below that.
     *
     * Then we want to wait for qenables to complete on all three.
     *
     * Recovery from errors awaiting qsched completion involves putting
     * back the flags in all three queues.
     */
    if (SAMESTR(head->sd_wq))
    {
	queue_t		*wqn = NULL ;
	queue_t		*wqnn = NULL ;

	lis_freezestr(head->sd_rq) ;
	if (   (err = lis_await_qsched(head, head->sd_wq)) < 0
	    || (err = lis_await_qsched(head, wqn = head->sd_wq->q_next)) < 0
	    || (err = lis_await_qsched(head, wqnn = wqn->q_next)) < 0
	   )
	{
	    lis_unfreezestr(head->sd_rq) ;
	    return(err) ;
	}

	lis_qdetach( LIS_RD(wqn), 1, flags, creds );
	if (wqnn != NULL)
	    lis_unfreezestr(wqnn) ;
    }

    return(0);

}/*pop_mod*/

/*  -------------------------------------------------------------------  */

/* Set STREAMS options
 */
static void
set_options( stdata_t *hd, stroptions_t *so)
{
    /* just set head flags as requested
     */
    if (F_ISSET(so->so_flags,SO_MREADON))	SET_SD_FLAG(hd,SNDMREAD);
    if (F_ISSET(so->so_flags,SO_MREADOFF))	CLR_SD_FLAG(hd,SNDMREAD);
    if (F_ISSET(so->so_flags,SO_NDELON))	SET_SD_FLAG(hd,OLDNDELAY);
    if (F_ISSET(so->so_flags,SO_NDELOFF))	CLR_SD_FLAG(hd,OLDNDELAY);
    if (F_ISSET(so->so_flags,SO_ISTTY))		SET_SD_FLAG(hd,STRISTTY);
    if (F_ISSET(so->so_flags,SO_ISNTTY))	CLR_SD_FLAG(hd,STRISTTY);
    if (F_ISSET(so->so_flags,SO_TOSTOP))	SET_SD_FLAG(hd,STRTOSTOP);
    if (F_ISSET(so->so_flags,SO_TONSTOP))	CLR_SD_FLAG(hd,STRTOSTOP);
    if (F_ISSET(so->so_flags,SO_STRHOLD))	SET_SD_FLAG(hd,STRHOLD);
    if (F_ISSET(so->so_flags,SO_DELIM))		SET_SD_FLAG(hd,STRDELIM);
    if (F_ISSET(so->so_flags,SO_NODELIM))	CLR_SD_FLAG(hd,STRDELIM);


    if (F_ISSET(so->so_flags,SO_READOPT))
	set_readopt(&hd->sd_rdopt,so->so_flags);

    /* This chunk will set parameters
     */
    if (F_ISSET(so->so_flags,SO_WROFF))		
        hd->sd_wroff=so->so_wroff;
    if (F_ISSET(so->so_flags,SO_MINPSZ))	
        hd->sd_rq->q_minpsz= so->so_minpsz;
    if (F_ISSET(so->so_flags,SO_MAXPSZ))
	hd->sd_rq->q_maxpsz= so->so_maxpsz;
    if (F_ISSET(so->so_flags,SO_BAND))
    {
	if (F_ISSET(so->so_flags,SO_HIWAT))
	    lis_strqset(hd->sd_rq, (qfields_t) QHIWAT,
						    so->so_band, so->so_hiwat);
	if (F_ISSET(so->so_flags,SO_LOWAT))
	    lis_strqset(hd->sd_rq, (qfields_t) QLOWAT,
						    so->so_band, so->so_lowat);
    }
    else					/* set in q struct */
    {
	if (F_ISSET(so->so_flags,SO_HIWAT))
	    hd->sd_rq->q_hiwat= so->so_hiwat;
	if (F_ISSET(so->so_flags,SO_LOWAT))
	    hd->sd_rq->q_lowat= so->so_lowat;
    }
}/*set_options*/

/*  -------------------------------------------------------------------  */

/* Wake up polling processes
 */
static void
lis_wake_up_poll(stdata_t *hd, int ev)
{
#if defined(PORTABLE_POLL)
  polldat_t *pd;

  if ( LIS_DEBUG_POLL )
    printk("lis_wake_up_poll: stream %s: revents:%s\n",
	    hd->sd_name, lis_poll_events((short)ev)) ;

  for (pd=hd->sd_polllist.ph_list; pd != NULL; pd=pd->pd_next) 
  {
    pd->pd_events = (short) ev ;		/* events being caused */
    if (pd->pd_fn != NULL)			/* careful */
	(*pd->pd_fn)(pd->pd_arg);		/* this will wake up the
						 * polling process.
	  					 */
  }

  lis_select_wakeup(hd) ;			/* wake up selectors */

#elif defined(LINUX_POLL)

  if ( LIS_DEBUG_POLL )
    printk("lis_wake_up_poll: stream %s: revents:%s\n",
		hd->sd_name, lis_poll_events((short)ev)) ;

  wake_up_interruptible(&hd->sd_task_list);

#else
#error "Either PORTABLE_POLL or LINUX_POLL must be defined"
#endif

}/*lis_wake_up_poll*/

/*  -------------------------------------------------------------------  */

/* deallocate poll list
 * 
 * Called from stdata structure deallocation code.  Shortly after
 * this routine returns the stdata structure will be deallocated.
 * We wake up any processes that are sleeping on poll data structures
 * chained off of this stdata structure.  Immediately upon return
 * from the wakeup, we go through the list and nullify the pointer
 * to the list head in all the pdat structures.  This means that
 * the poll code, when it finally wakes up, may find this head
 * pointer to be NULL.  If the poll code has not delinked the
 * poll entry by then, it may be in trouble.
 */
static void
lis_deallocate_polllist( stdata_t *hd)
{
#if defined(PORTABLE_POLL)
  polldat_t *pd;

  if (hd->sd_polllist.ph_list != NULL)
      lis_wake_up_poll(hd, POLLHUP) ;
  else						/* no pollers */
      lis_select_wakeup(hd) ;			/* wake up selectors */

  for (pd=hd->sd_polllist.ph_list; pd != NULL; pd = pd->pd_next) 
  {
      pd->pd_headp = NULL ;
  }

#elif defined(LINUX_POLL)

  init_waitqueue_head(&hd->sd_task_list) ;

#else
#error "Either PORTABLE_POLL or LINUX_POLL must be defined"
#endif
}/*lis_deallocate_polllist*/

/*  -------------------------------------------------------------------  */
/* kill procs in elist w/ sig
 */
static void 
kill_procs( struct strevent *elist, int sig, short e)
{
    strevent_t *ev;
    int res;

    (void) sig ;
    for(ev = elist->se_next ; ev != elist; ev=ev->se_next )
	if ((ev->se_evs & e) != 0){
	    if (F_ISSET(ev->se_evs,S_BANDURG) && F_ISSET(ev->se_evs,S_RDBAND))
	    {
		if (LIS_DEBUG_SIG)
		    printk("kill_proc(pid=%d, sig=0x%x)\n", ev->se_pid, SIGURG);
		if ((res=lis_kill_proc(ev->se_pid,SIGURG,1))<0)
		    lis_error(LIS_ERROR,
			      "kill_procs","kill_proc: errno %d",res);

		if (ev->se_evs& e& ~S_RDBAND) /* other events -> sigpoll too */
		{
		    if (LIS_DEBUG_SIG)
			printk("kill_proc(pid=%d, sig=0x%x)\n",
				ev->se_pid, SIGPOLL);
		    if ((res=lis_kill_proc(ev->se_pid,SIGPOLL,1))<0)
			lis_error(LIS_ERROR,
				  "kill_procs","kill_proc: errno %d",res);
		}
	    }
	    else
	    {
		if (LIS_DEBUG_SIG)
		    printk("kill_proc(pid=%d, sig=0x%x)\n",
			    ev->se_pid, SIGPOLL);
		if ((res=lis_kill_proc(ev->se_pid,SIGPOLL,1))<0)
		    lis_error(LIS_ERROR,"kill_procs","kill_proc: errno %d",res);
	    }
	}
    
}/*kill_procs*/
/*  -------------------------------------------------------------------  */

/* Will send relevant signals to the usr on arrival of M_[PC]SIG containing
 * `sig' at the stream head.
 */
static void 
signalusr(int sig, stdata_t *hd)
{
    if (sig==SIGPOLL)
	kill_procs(hd->sd_siglist,SIGPOLL,SIGPOLL); /* ?? !!! */
    else if (F_ISSET(hd->sd_flag,STRISTTY))
	lis_kill_pg(hd->sd_pgrp,sig,1);
}/*signalusr*/
/*  -------------------------------------------------------------------  */

/* Send an M_READ msg downstream
 */
static int
snd_mread(queue_t *wq, unsigned int ulen)
{
    mblk_t *mread = allocb(sizeof(ulen),BPRI_LO);
    unsigned *uptr ;

    if (mread == NULL)
	return(-ENOMEM);

    lis_btype(mread)=M_READ;
    uptr = (unsigned *) mread->b_wptr ;
    *uptr = ulen ;
    mread->b_wptr += sizeof(*uptr);
    lis_putnext(wq, mread);

    return(0);

}/*snd_mread*/

/* Send an M_FLUSH msg downstream
 */
static int
lis_snd_mflush(queue_t *wq, int flag, int band)
{
    mblk_t *mf = allocb(2*sizeof(int),BPRI_LO); /*sizeof mflush msg (?)*/
    if (mf == NULL)
	return(-ENOMEM);
    else {
	lis_btype(mf)=M_FLUSH;
	lis_putbyte(&(mf->b_wptr),(char)flag);
	if (F_ISSET(flag,FLUSHBAND))
	    lis_putbyte(&(mf->b_wptr),(char)band);
	lis_putnext(wq,mf);		/* don't use PUTNEXT here */
	return(0);
    }
}/*lis_snd_mflush*/

/*  -------------------------------------------------------------------  */
/* Send an ioctl negative ack  msg downstream reusing mp (M_IOCTL) msg.
 */
static int
snd_iocnak(queue_t *wq, mblk_t *mp,int code)
{
    iocblk_t *iocb = (iocblk_t*)(mp->b_rptr);
    mblk_t   *dtab;
    lis_btype(mp)=M_IOCNAK;
    iocb->ioc_error=code;
    dtab=lis_unlinkb(mp);
    lis_freemsg(dtab);
    lis_putnext(wq,mp);			/* don't use PUTNEXT here */
    return(0);
}/*snd_iocnak*/


/*  -------------------------------------------------------------------  */

/* Is this ioc blk out of sequence?
 */
static int
bad_ioc_seq( mblk_t *mp, stdata_t *hd)
{
    iocblk_t *iocb = (iocblk_t*)mp->b_rptr;

    return(iocb->ioc_id != (uint) hd->sd_iocseq);

}/*bad_ioc_seq*/

/*  -------------------------------------------------------------------  */

/* Call every open procedure in this stream
 */
static int
open_mods( stdata_t *head, dev_t *devp, int flags, cred_t *creds )
{
    queue_t *wq = (head ? head->sd_wq : NULL);
    dev_t    odev = *devp;
    int 	 err;

    if (wq == NULL)  return(-ENOPKG);		/* pkg not installed */

    while (wq)  {
	if ((err = lis_qopen( RD(wq), devp, flags, creds )) < 0)  return (err);

	if (!SAMESTR(wq))  return (0);

	if (!DEV_SAME(odev, *devp))  return (0);

	wq = wq->q_next;
    }

    return(0);

}/*open_mods*/

/*  -------------------------------------------------------------------  */
/*
 * lis_head_flush - called to process an M_FLUSH received from below
 *
 * This may be running at interrupt time.
 *
 * Return 0 if the flush is complete and mp is disposed of.  Return 1 if
 * the flush is deferred and mp is still valid.
 */
static
int lis_head_flush(stdata_t *shead, queue_t *q, mblk_t *mp, int flush_rputq)
{
    int		msgs_before = lis_qsize(q) ;
    lis_flags_t	psw ;
    mblk_t     *xp, *tmp = NULL ;

    if ( LIS_DEBUG_FLUSH )
	printk("lis_head_flush: "
		"M_FLUSH from \"%s\" on stream %s flags=0x%x flush_rputq=%d\n",
		lis_queue_name(backq(q)),
		shead->sd_name, *mp->b_rptr & 0xFF, flush_rputq) ;

    /*
     * Are we closing?  If so do not perform the flush, just wake up
     * the closing code.
     */
    if (F_ISSET(shead->sd_flag, STRFLUSHWT))
    {						/* close-time flush */
	CP(q,mp) ;
	freemsg(mp) ;				/* discard message */
	CLR_SD_FLAG(shead, STRFLUSHWT) ;
	lis_wakeup_close_wt(shead) ;
	return(0) ;				/* "done" */
    }

    /*
     * Is strrput or strread running?  If so, defer the flush until
     * strrsrv runs.
     */
    LIS_RDQISRLOCK(q, &psw) ;
    if (q->q_flag & QREADING)		/* read/getpmsg running */
    {
	LIS_RDQISRUNLOCK(q, &psw) ;
	return(1) ;			/* deferred, mp still allocated */
    }
    LIS_RDQISRUNLOCK(q, &psw) ;

    if (*mp->b_rptr & FLUSHR)
    {					/* flush read queue */
	CP(mp,0) ;
	if (flush_rputq)
	{
	    /*
	     * Flush the rput temporary queue.  Its messages are 
	     * accounted for in the stream head q structure.
	     */
	    while ((xp = lis_get_rput_q(shead)) != NULL)
	    {
	        /*
		 * All messages are temporary saved in a local list.
		 * Correct action will be taken after flush is complete.
		 * Unqueueing now will preserves q_count value.
		 */
		xp->b_next = tmp ;
		tmp = xp ;
	    }
	}
	/*
	 * Flush the "official" stream head queue.
	 */
	if (F_ISSET(*mp->b_rptr,FLUSHBAND))
	    lis_flushband(LIS_RD(q),mp->b_rptr[1],FLUSHDATA);
	else
	    lis_flushq(LIS_RD(q),FLUSHDATA);

	LIS_RDQISRLOCK(shead->sd_rq, &psw) ;
	if (   shead->sd_rq->q_first == NULL
	    || !lis_hipri(shead->sd_rq->q_first->b_datap->db_type)
	   )
	    CLR_SD_FLAG(shead,STRPRI);	/* no PCPROTO at head */
	LIS_RDQISRUNLOCK(shead->sd_rq, &psw) ;

	*mp->b_rptr &= ~FLUSHR ;		/* turn off flush read bit */
	    /*
	     * Do not discard messages corresponding to
	     * an in-progress ioctl (IOCWAIT flag).
	     * Discarding them could let the ioctl sleeping for ever
	     * (unless timeout is activated)
	     * Re-queuing now (after flush) preserves q_count value.
	     */
	    LIS_QISRLOCK(q, &psw) ;
	    while ( (xp = tmp) != NULL )
	    {
		tmp = xp->b_next ;
		xp->b_next = NULL ;
		if ( F_ISSET(shead->sd_flag, IOCWAIT) )
		    lis_put_rput_q(shead, xp) ;
		else
		{
		    msgs_before++ ;
		    freemsg(xp) ;
		}
	    }
	    LIS_QISRUNLOCK(q, &psw) ;


	LisDownCounter(MSGQDSTRHD, msgs_before - lis_qsize(q)) ;
    }

    if (*mp->b_rptr & FLUSHW)
    {					/* flush write queue */
	CP(mp,0) ;
	if (F_ISSET(*mp->b_rptr,FLUSHBAND))
	    lis_flushband(LIS_WR(q),mp->b_rptr[1],FLUSHDATA);
	else
	    lis_flushq(LIS_WR(q),FLUSHDATA);

	if (!(mp->b_flag & MSGNOLOOP))
	{
	    mp->b_flag |= MSGNOLOOP;
	    lis_putnext(LIS_WR(q),mp);	/* send back downstream */
	    return(0) ;			/* w/head unlocked */
				 	/* do not use PUTNEXT */
	}

	lis_freemsg(mp);		/* free message */
	return(0) ;			/* done */
    }

    lis_freemsg(mp);
    return(0) ;				/* done */
}

/*  -------------------------------------------------------------------  */
/* lis_stream_error
 *
 * Handles setting error conditions on a stream head.  Used by M_ERROR
 * processing and file closing.
 *
 * Pass 0 for error code to reset error flag.  Pass -1 for error code
 * to ignore that particular flag.  Pass positive error code to set that
 * error code in the stream head structure.
 */
void	lis_stream_error(stdata_t *shead, int rderr, int wrerr)
{
    if (rderr == 0)
    {
	shead->sd_rerror = 0 ;
	CLR_SD_FLAG(shead,STRDERR);
    }
    else if (rderr > 0 && rderr != NOERROR)
    {
	shead->sd_rerror = rderr ;
	SET_SD_FLAG(shead,STRDERR);
    }

    if (wrerr == 0)
    {
	shead->sd_werror = 0 ;
	CLR_SD_FLAG(shead,STWRERR);
    }
    else if (wrerr > 0 && wrerr != NOERROR)
    {
	shead->sd_werror = wrerr ;
	SET_SD_FLAG(shead,STWRERR);
    }

    /*
     * Wakeups and signals are performed whether there is an error or not, just
     * by virtue of calling this routine.
     */
    if (F_ISSET(shead->sd_sigflags,S_ERROR))
	kill_procs(shead->sd_siglist,SIGPOLL,S_ERROR);

    if (   POLL_WAITING(shead)
	|| F_ISSET(shead->sd_flag,STRSELPND)
       )
	lis_wake_up_poll(shead,POLLERR);

    lis_wake_up_all_wwrite(shead);
    lis_wake_up_all_wread(shead);
    lis_wake_up_all_read_sem(shead);

    if (F_ISSET(shead->sd_flag,IOCWAIT))
	lis_wake_up_wiocing(shead);
}

/*  -------------------------------------------------------------------  */
/* lis_process_rput - called from read service to actually process msg
 *
 * This is running as a service procedure and, therefore, does not have
 * to worry about interrupts.
 *
 * The message was conveyed via a special list from strrput.  Now we 
 * put the message into the official read queue and wake up processes
 * that might be waiting for messages.
 *
 * LOCK ORDERING:  We are entered with both the head and the queue
 * unlocked.  We need to lock both by locking the head first, and then
 * the queue -- the same order as the stream head code in strread or
 * strgetpmsg.  We return with both locks available.
 */
void lis_process_rput(stdata_t *shead, queue_t *q, mblk_t *mp)
{
    int		typ ;
    int		err ;

    CP(shead,mp) ;
    typ = lis_btype(mp);
    CP(shead,mp) ;
    if ((err = lis_lockq(q)) < 0)			/* lock the queue */
    {
	freemsg(mp) ;
	lis_stream_error(shead, err, err) ;
	return;
    }

    CP(q,mp) ;
    if (   F_ISSET(shead->sd_flag, STRCLOSE)	/* stream is closing */
	|| (q->q_flag & (QCLOSING | QPROCSOFF))
       )
    {						/* no file above stream */
	CP(q,mp) ;
	switch (typ)
	{
	case M_PCPROTO:
	case M_DATA:
	case M_PROTO:
	case M_SIG:
	case M_PCSIG:
	case M_COPYIN: 
	case M_COPYOUT:
	case M_ERROR:
	case M_HANGUP:
	case M_FLUSH:
	case M_SETOPTS:
	case M_IOCTL:				/* ioctl from below */
	case M_READ:
	    if (LIS_DEBUG_STRRPUT)
	      printk(
	       "lis_process_rput: "
	       "discarding %s from \"%s\" on closed stream %s\n",
			lis_msg_type_name(mp), lis_queue_name(backq(q)),
			shead->sd_name) ;
	    lis_freemsg(mp) ;			/* no hope of retrieving msg */
	    if (typ == M_FLUSH && F_ISSET(shead->sd_flag, STRFLUSHWT))
	    {					/* close-time flush */
		CP(q,0) ;
		CLR_SD_FLAG(shead, STRFLUSHWT) ;
		lis_wakeup_close_wt(shead) ;
	    }
	    goto return_point ;
	case M_PASSFP:
	    lis_free_passfp(mp);
	    goto return_point;
	case M_IOCNAK: 
	case M_IOCACK: 
	    printk("lis_process_rput: closing: M_IOCNAK or M_IOCACK "
		    "should have been handled by lis_strrput\n") ;
	    goto return_point;
	}
    }

    if (LIS_DEBUG_STRRPUT)
	printk("lis_process_rput: "
		"received %s (%d) from \"%s\" on stream %s size %d\n",
		lis_msg_type_name(mp), typ, lis_queue_name(backq(q)),
		shead->sd_name, lis_msgsize(mp)) ;

    CP(q,mp) ;
    switch(typ)
    {
    case M_DATA:
    case M_PCPROTO:			/* flag set in strrput */
    case M_PROTO:
    case M_PASSFP:
	CP(mp,0) ;
	LisUpCount(MSGQDSTRHD) ;
        putqf(q,mp);			/* put in official queue */
	if (   shead->sd_sigflags == 0			/* no signals */
	    && POLL_NOT_WAITING(shead)			/* no polling */
	    && !F_ISSET(shead->sd_flag,STRSELPND)	/* no selecting */
	   )
	{
	    lis_wake_up_wread(shead);	/* wake up if someone sleeping */
	    break ;
	}

	if (typ == M_PCPROTO || mp == q->q_first)
	{				/* hipri msg or 1st msg in queue */
	    short evs=0;
	    short polls=0 ;

	    if (lis_hipri(typ))
	    {
		F_SET(polls,POLLPRI) ;
		F_SET(evs,S_HIPRI);
	    }
	    else
	    {
		F_SET(polls,POLLIN) ;
		F_SET(evs,S_INPUT);

		if (lis_bband(mp)==0)
		{
		    F_SET(polls,POLLRDNORM) ;
		    F_SET(evs,S_RDNORM);
		}

		if (lis_bband(mp)>0)
		{
		    F_SET(polls,POLLRDBAND) ;
		    F_SET(evs,S_RDBAND);
		}
	    }

	    if (evs &= shead->sd_sigflags)
		kill_procs(shead->sd_siglist,SIGPOLL,evs);

	    if (   polls
		&& (   POLL_WAITING(shead)
		    || F_ISSET(shead->sd_flag,STRSELPND)
		   )
	       )
		lis_wake_up_poll(shead,polls);
	}
	lis_wake_up_wread(shead);	/* wake up if someone sleeping */
	break;
    case M_ERROR:
	{
	    unsigned char	rderr, wrerr ;

	    CP(mp,0) ;
	    switch (mp->b_wptr - mp->b_rptr) 	/* how many bytes?  */
	    {
	    default:				/* not valid format */
		mp->b_rptr = mp->b_datap->db_base;
		mp->b_wptr = mp->b_rptr ;
		*mp->b_wptr++ = EINVAL ;	/* supply default errnum */
		/*
		 * Fall into the one-byte case
		 */
	    case 1:				/* set errnum for both sides */
		rderr = *mp->b_rptr++;
		wrerr = rderr ;
		lis_stream_error(shead, rderr, wrerr) ;
		break ;

	    case 2:				/* set two sides individually */
		rderr = *mp->b_rptr++;
		wrerr = *mp->b_rptr++;
		lis_stream_error(shead, rderr, wrerr) ;
		break ;
	    }

	    lis_freemsg(mp);
	    break;
	}
    case M_HANGUP:
	CP(mp,0) ;
	lis_freemsg(mp);
	SET_SD_FLAG(shead,STRHUP);
	if (F_ISSET(shead->sd_sigflags,S_HANGUP))
	    kill_procs(shead->sd_siglist,SIGPOLL,S_HANGUP);
	if (   POLL_WAITING(shead)
	    || F_ISSET(shead->sd_flag,STRSELPND)
	   )
	    lis_wake_up_poll(shead,POLLHUP);

	if (F_ISSET(shead->sd_flag,STRATTACH) && !(shead->sd_rfdcnt)) {
	    lis_fdetach_stream(shead);
	}

	lis_wake_up_all_wwrite(shead);
	lis_wake_up_all_wread(shead);
	lis_wake_up_all_read_sem(shead);
	break;

    case M_FLUSH:			/* should be done in strrput */
	/*
	 * This should not defer.  We have the q lock, so strrput or
	 * strread should not be actively reading.  If this defers then
	 * it is a bug.
	 */
	CP(q,mp) ;
	if (lis_head_flush(shead, q, mp, 0))	/* just the hd q */
	{					/* deferred */
	    printk("lis_process_rput: Bug: M_FLUSH: lis_head_flush fail\n") ;
	    freemsg(mp) ;
	}

	break;

    case M_SIG:
	CP(mp,0) ;
	putqf(q,mp);			/* strread will signalusr() */
	LisUpCount(MSGQDSTRHD) ;
	lis_check_m_sig(shead) ;	/* see if it is the only msg in the q */
	break;
    case M_PCSIG:
	CP(mp,0) ;
	signalusr(*mp->b_rptr,shead);
	if (   POLL_WAITING(shead)
	    || F_ISSET(shead->sd_flag,STRSELPND)
	   )
	    lis_wake_up_poll(shead,POLLMSG);
	lis_freemsg(mp);
	break;
    case M_IOCNAK: 
    case M_IOCACK: 
	printk("lis_process_rput: M_IOCNAK or M_IOCACK "
		"should have been handled by lis_strrput\n") ;
	break ;
    case M_COPYIN: 
	CP(mp,0) ;
#if 0					/* intercepted in strrput */
	if (!F_ISSET(shead->sd_flag,IOCWAIT))
	    lis_freemsg(mp);
	else if (F_ISSET(shead->sd_flag,STWRERR) ||
		 F_ISSET(shead->sd_flag,STRDERR) || bad_ioc_seq(mp,shead)){
	    copyresp_t *res=(copyresp_t*)mp->b_rptr;
	    mp->b_wptr=(char*)(res+1);
	    mp->b_datap->db_type = M_IOCDATA ;
	    res->cp_rval=(caddr_t)1;
	    lis_unlockq(q) ;
	    lis_putnext(LIS_WR(q),mp);		/* do not use PUTNEXT */
	    return ;				/* w/head unlocked */
	}
	else {			/* everything's ok: now we process it */
	    shead->sd_iocblk=mp;
	    lis_wake_up_wiocing(shead);
	}
#endif
	break;
    case M_COPYOUT:
	CP(mp,0) ;
#if 0					/* intercepted in strrput */
	if (!F_ISSET(shead->sd_flag,IOCWAIT))
	    lis_freemsg(mp);
	else if (F_ISSET(shead->sd_flag,STWRERR) ||
		 F_ISSET(shead->sd_flag,STRDERR) ||  bad_ioc_seq(mp,shead)){
	    copyresp_t *res=(copyresp_t*)mp->b_rptr;
	    mp->b_wptr=(char*)(res+1);
	    mp->b_datap->db_type = M_IOCDATA ;
	    res->cp_rval=(caddr_t)1;
	    lis_unlockq(q) ;
	    lis_putnext(LIS_WR(q),mp);		/* do not use PUTNEXT */
	    return ;				/* w/head unlocked */
	}
	else {			/* everything's ok: now we process it */
	    shead->sd_iocblk=mp;
	    lis_wake_up_wiocing(shead);
	}
#endif
	break;
    case M_SETOPTS:
	CP(mp,0) ;
	set_options(shead,(stroptions_t*)mp->b_rptr);
	lis_freemsg(mp);
	break ;
    case M_IOCTL:				/* upside-down ioctl */
	CP(mp,0) ;
	snd_iocnak(LIS_WR(q),mp,1);
	lis_unlockq(q) ;
	return ;				/* w/head unlocked */
    case M_READ:				/* always discard */
    default:
	CP(mp,0) ;
	lis_freemsg(mp);
	break ;
    }

return_point:
    CP(mp,0) ;
    lis_unlockq(q) ;
}


/*  -------------------------------------------------------------------  */
/* lis_strrsrv - stream head read service procedure.
 *
 * Process messages queued by the receive put procedure.  Running this
 * as a service procedure allows us not to have to worry so much about
 * interrupts.  The lis_strrput procedure *could* be entered from interrupt
 * level.  This service procedure is guaranteed to be running at task level.
 *
 * The only interrupt level worry that we have is in taking messages out
 * of the rput list.
 *
 * LOCK ORDERING:  We are entered from the streams scheduler with the queue
 * locked.  The routine lis_process_rput needs both the head lock and the queue
 * lock.  It needs to get the head lock first, which means that we need to
 * release the qeuue lock.  
 */
int	_RP lis_strrsrv(queue_t *q)
{
    mblk_t   	*mp ;
    stdata_t	*hd ;
    lis_flags_t	 psw ;
    int		 need_qenable ;
    int		 err ;

    if (   !LIS_CHECK_Q_MAGIC(q)
        || (hd = q->q_str) == NULL
	|| hd->magic != STDATA_MAGIC
	|| hd->sd_rq != q
       )
    {
	printk("lis_strrsrv: msg to improperly formed queue\n") ;
	return(0) ;
    }

    CP(q,0) ;
    lis_unlockq(q) ;			/* unlock the queue */
					/* nothing locked now */

    while ((mp = lis_get_rput_q(hd)) != NULL)
    {
	LisDownCount(MSGQDSTRHD) ;	/* one fewer msg queued */
	lis_process_rput(hd, q, mp) ;
    }

    if (!LIS_CHECK_Q_MAGIC(q))
	return(0) ;

    LIS_QISRLOCK(q, &psw) ;
    q->q_flag &= ~QENAB;		/* allow qenable */
    need_qenable = (hd->sd_rput_hd != NULL) ;	/* msg(s) appeared */
    LIS_QISRUNLOCK(q, &psw) ;
    CP(q,0) ;
    err = lis_lockq(q) ;		/* re-lock queue, but not head */
    if (!err && need_qenable)
	qenable(q) ;
    CP(q,err) ;
    return(err) ;
}


/*  -------------------------------------------------------------------  */
/* lis_strrput - stream head read put procedure
 *
 * Don't do anything but just queue the message for the service procedure.
 *
 * This routine could be called from interrupt context, though that is not good
 * programming practice.  Therefore, it needs to use SPL protection when
 * inserting the message into the list.  Because of the SPL we don't need to
 * lock the stream head.
 */
int _RP
lis_strrput(queue_t *q, mblk_t *mp)
{
    stdata_t	*hd ;
    lis_flags_t  psw ;

    if (   !LIS_CHECK_Q_MAGIC(q)
        || (hd = q->q_str) == NULL
	|| hd->magic != STDATA_MAGIC
	|| hd->sd_rq != q
       )
    {
	printk("lis_strrput: msg to improperly formed queue\n") ;
	freemsg(mp) ;
	return(0) ;
    }

    CP(q,mp) ;
    LIS_RDQISRLOCK(q, &psw) ;
    if (q->q_flag & QPROCSOFF)
    {
	LIS_RDQISRUNLOCK(q, &psw) ;
	CP(q,mp) ;
	if ( LIS_DEBUG_CLOSE )
	    printk("lis_strput: discarding message from closing queue\n");

	freemsg(mp) ;
	return(0) ;
    }

    LIS_RDQISRUNLOCK(q, &psw) ;
    mp->b_next = NULL ;

    /*
     * The flush wait logic in close has disabled qenables so that the
     * stream head read service procedure cannot run.  We need to catch
     * this case here so that close does not have to suffer a 50ms timeout.
     *
     * Flush logic must be performed immediately so as not to flush messages
     * that arrive AFTER the M_FLUSH.
     *
     * Handle ioctl ack/nak here so they do not get caught in flushes.
     */
    switch (mp->b_datap->db_type)
    {
    case M_PCPROTO:
	if (F_ISSET(hd->sd_flag,STRPRI))
	{
	    CP(q,mp) ;
	    lis_freemsg(mp);
	    return(0) ;
	}
	SET_SD_FLAG(hd,STRPRI);
	break ;

    case M_FLUSH:
	if (lis_head_flush(hd, q, mp, 1))	/* both queues */
	    break ;				/* deferred */
	return(0) ;				/* done */

    case M_IOCNAK: 
    case M_IOCACK: 
    case M_COPYIN:
    case M_COPYOUT:
    case M_IOCDATA:
	LIS_RDQISRLOCK(q, &psw) ;
	if (!F_ISSET(hd->sd_flag,IOCWAIT)|| 
	    hd->sd_iocblk != NULL ||
	    (q->q_flag & QCLOSING) ||
	    /* F_ISSET(hd->sd_flag,(STWRERR|STRDERR)) || */
	    bad_ioc_seq(mp,hd)
	   )
	{
	    LIS_RDQISRUNLOCK(q, &psw) ;
	    CP(q,mp) ;
	    lis_freemsg(mp);
	}
	else
	{				/* everything's ok: now we process it */
	    LIS_RDQISRUNLOCK(q, &psw) ;
	    CP(q,mp) ;
	    hd->sd_iocblk=mp;
	    lis_wake_up_wiocing(hd);
	}
	return(0) ;
    }

    if (LIS_DEBUG_STRRPUT)
	printk("lis_strrput: %s queueing %s\n",
			    hd->sd_name, lis_msg_type_name(mp)) ;

    lis_put_rput_q(hd, mp) ;			/* put into sd_rput queue */
    LisUpCount(MSGQDSTRHD) ;
    return(0) ;

}/*lis_strrput*/


/*  -------------------------------------------------------------------  */
/*  lis_strwsrv
 *
 * This is the stream head write service procedure.  It is used
 * for detecting that the stream below the stream head no longer
 * has messages queued for output so the stream can be closed.
 */

int	_RP lis_strwsrv(queue_t *q)
{
    stdata_t	*hd ;
    mblk_t	*mp ;
    int		 couldput ;

    if (!LIS_CHECK_Q_MAGIC(q)) return(0) ;

    hd = (stdata_t *) q->q_str ;
    if (hd == NULL)				/* should not happen */
    {
	printk("lis_strwsrv: queue %s: q_str is NULL\n", lis_queue_name(q));
	return(0) ;
    }
    
    if (hd->magic != STDATA_MAGIC || hd->sd_wq != q) return(0) ;

    CP(hd,0) ;
    while (   (couldput = lis_bcanputnext(q,0))	/* can send downstream */
	   && (mp = getq(q)) != NULL		/* have queued messages */
	  )
    {
	CP(q,mp) ;
	lis_putnext(q, mp) ;			/* send downstream */
						/* don't use PUTNEXT */
    }

    if (   F_ISSET(hd->sd_flag, STRCLOSE)	/* stream is closing */
	&& lis_qcountstrm(q) == 0 		/* queues are empty */
       )
   {
	if ( LIS_DEBUG_CLOSE )
	    printk("lis_strwsrv: stream %s empty, wake up close routine\n",
		    hd->sd_name) ;

	CP(hd,0) ;
	lis_wakeup_close((caddr_t) hd) ;	/* wake up close */
   }

   if (hd->sd_sigflags != 0)
   {
	if (F_ISSET(hd->sd_sigflags,S_OUTPUT)
	    && lis_bcanputnext(q,0)
	   )
	{
	    CP(hd,0) ;
	    kill_procs(hd->sd_siglist,SIGPOLL,S_OUTPUT);
	}

	/*
	 * AT&T says in STREAMS Programmer's Guide:
	 *
	 * "S_WRBAND
	 *
	 * "A priority band greater than 0 of a queue downstream exists
	 * and is writeable.  This notifies the user that there is room
	 * on the queue for sending (or writing) priority data downstream."
	 */
	if (F_ISSET(hd->sd_sigflags,S_WRBAND)
	    && lis_bcanputnext_anyband(q)
	   )
	{
	    CP(hd,0) ;
	    kill_procs(hd->sd_siglist,SIGPOLL,S_WRBAND);
	}
   }

   if (couldput && K_ATOMIC_READ(&hd->sd_wrcnt) != 0)
					        /* write waiting flow ctrl */
   {
	CP(hd,0) ;
	lis_wake_up_all_wwrite(hd);		/* wake up all write/putmsg */
   }

    if (  (   POLL_WAITING(hd)
	   || F_ISSET(hd->sd_flag,STRSELPND)
          )
	&& lis_bcanputnext(q,0)			/* can still send downstream */
       )
    {
	CP(hd,0) ;
	lis_wake_up_poll(hd,POLLOUT);
    }

    return(0) ;

} /* lis_strwsrv */


/*  -------------------------------------------------------------------  */
/* This is the tmout handling function.
 * it will be called on response to a tmout and is responsible to
 * take the right actions.
 * It should be called w/ an stream head as argument.
 */
static void
lis_do_tmout( unsigned long arg )
{
    stdata_t *hd = (stdata_t*)arg;

    if (hd->magic != STDATA_MAGIC){
	lis_error(LIS_ERROR,"lis_do_tmout","timeout with bad stream 0x%p",hd);
	return;
    }
    if (F_ISSET(hd->sd_flag,STIOCTMR)){     /* ioctl timer: awake sleeper*/
	CP(hd,hd->sd_flag) ;
	CLR_SD_FLAG(hd,STIOCTMR);
	if (F_ISSET(hd->sd_flag,IOCWAIT))	/* waiting for ioctl */
	    lis_wake_up_wiocing(hd);
    }
}/*lis_do_tmout*/

static void
lis_do_rd_tmout( unsigned long arg )
{
    stdata_t *hd = (stdata_t*)arg;

    if (hd->magic != STDATA_MAGIC){
	lis_error(LIS_ERROR,"lis_do_rd_tmout",
			    "timeout with bad stream 0x%p",hd);
	return;
    }

    lis_wake_up_wread(hd);

}/*lis_do_tmout*/


/*  -------------------------------------------------------------------  */
/* lis_wait_for_wiocing - wait until ioc response arrived or timeout occur
 */
static int
lis_wait_for_wiocing(stdata_t *hd, int tmout, int ignore_errors)
{
    struct timer_list tl;
    int		      rslt ;

    if (tmout == 0)
	tmout = LIS_DFLT_TIM ;

    if ( !ignore_errors && F_ISSET(hd->sd_flag,(STRDERR|STWRERR)) )
	return(-hd->sd_rerror) ;
    SET_SD_FLAG(hd,STIOCTMR);
    if (tmout != INFTIM)
	lis_tmout(&tl,lis_do_tmout,(long)hd, SECS_TO(tmout));
    CP(hd,tmout) ;
    rslt = lis_sleep_on_wiocing(hd);
    if (rslt < 0)
    {
	CP(hd,rslt) ;
	if (tmout != INFTIM)
	    lis_untmout(&tl);
	CLR_SD_FLAG(hd,STIOCTMR);
	return(rslt) ;
    }

    if (!F_ISSET(hd->sd_flag,STIOCTMR)) /* timer expired */
    {
	CP(hd,ETIME) ;
	return(-ETIME);
    }
    else
    {
	CP(hd,0) ;
	if (tmout != INFTIM)
	    lis_untmout(&tl);
	CLR_SD_FLAG(hd,STIOCTMR);
    }
    return(0);			/* got msg */
}/*lis_wait_for_wiocing*/

/*  -------------------------------------------------------------------  */

/* copyout bmlks for reads
 */
static int 
copyout_blks(struct file *f, char *ubuff, long count, mblk_t *mp)
{
    mblk_t *mb;
    long ocount=count;
    int len;
    int err = 0 ;

    for (mb=mp ; mb && count > 0; mp=mb)
    {
	mb=mp->b_cont;				/* next buffer in chain */
	mp->b_cont = NULL ;			/* unchain this buffer */
	if (err == 0)
	{
	    len=lis_min(count,lis_mblksize(mp));	/* size of current bfr */
	    err = lis_copyout(f,mp->b_rptr,ubuff,len); /* cpy out current bfr */
	    count -=len;
	    ubuff +=len;
	}
	lis_freeb(mp);				/* free current buffer */
    }

    if (err < 0)
	return(err) ;				/* something went wrong */

    if (count != 0)			/* hopefully, copied whole buffer */
	printk("LiS:copyout_blks:"
		  "count (%ld) doesn't match data (%ld)", ocount, ocount-count);
    return(0) ;
}

/*  -------------------------------------------------------------------  */
/* strdoioctl - ioctl handler used by strioctl
 *
 * There is a special circumstance in which this routine is called
 * with a NULL pointer for 'f'.  This is from the i_unlink code in
 * which the file is not known.  We do not worry about null pointer
 * reference here since our only use of 'f' occurs with 'do_copyin'
 * is passed as a "true", which it is not from i_unlink code.
 */
static int
lis_strdoioctl(struct file *f, stdata_t *hd,
		strioctl_t *ioc, cred_t *creds, int do_copyin)
{
    mblk_t *mioc = NULL ;
    mblk_t *mdta = NULL ;
    iocblk_t *iocb;
    int err = 0 ;
    int errv = 0 ;
    int msgtype ;
    int ignore_errors = 0 ;
    unsigned long  time_cell = 0 ;

    /* Use RTN after getting wioc semaphore */
#define RTN(v)	do { errv=(v); goto return_point; } while (0)

    CLOCKON() ;
    if (ioc->ic_len != TRANSPARENT && (ioc->ic_len < 0 || ioc->ic_timout < -1))
    {
	ioc->ic_len = 0 ;		/* no data */
	CLOCKOFF(IOCTLTIME) ;
	return(-EINVAL);
    }
    if ((mioc=allocb(sizeof(copyreq_t),BPRI_LO))==NULL)
    {
	ioc->ic_len = 0 ;		/* no data */
	CLOCKOFF(IOCTLTIME) ;
	return(-ENOMEM);
    }
    if (ioc->ic_len){
	if (do_copyin && 
	    (err=lis_check_umem(f,VERIFY_READ,ioc->ic_dp,ioc->ic_len))<0)
	{
	    ioc->ic_len = 0 ;		/* no data */
	    CLOCKOFF(IOCTLTIME) ;
	    return(err);
	}
	if ((mdta=allocb(
	            ioc->ic_len == TRANSPARENT ? sizeof(ioc->ic_dp)
					       : ioc->ic_len,BPRI_LO))==NULL)
	{
	    ioc->ic_len = 0 ;		/* no data */
	    lis_freemsg(mioc);
	    CLOCKOFF(IOCTLTIME) ;
	    return(-ENOMEM);
	}
	if (do_copyin)
	{
	    if ((err = lis_copyin(f,mdta->b_wptr,ioc->ic_dp,ioc->ic_len)) < 0)
	    {
		ioc->ic_len = 0 ;		/* no data */
		lis_freemsg(mioc);
		CLOCKOFF(IOCTLTIME) ;
		return(err) ;
	    }
	    mdta->b_wptr += ioc->ic_len;
	}
	else
	if (ioc->ic_len != TRANSPARENT)
	{
	    memcpy(mdta->b_wptr,ioc->ic_dp,ioc->ic_len);
	    mdta->b_wptr += ioc->ic_len;
	}
	else
	{
	    void	**ptr_ptr = (void **) mdta->b_wptr ;
	    *ptr_ptr = ioc->ic_dp ;		/* pass in ptr */
	    mdta->b_wptr += sizeof(*ptr_ptr) ;
	}
	lis_linkb(mioc,mdta);
    }
    iocb=(iocblk_t*)mioc->b_wptr;
    mioc->b_wptr += sizeof(iocblk_t);
    mioc->b_datap->db_type = M_IOCTL ;
    iocb->ioc_cmd=ioc->ic_cmd;
    iocb->ioc_cr = creds ;
    iocb->ioc_count=ioc->ic_len;
    iocb->ioc_error=0;
    iocb->ioc_rval=0;
    iocb->ioc_filler[0] = 0 ;
    iocb->ioc_filler[1] = 0 ;
    iocb->ioc_filler[2] = 0 ;
    iocb->ioc_filler[3] = 0 ;
    /*
     * We only send one ioctl at a time downstream on a per stream basis.
     * To enforce this we hold this lock until the ioctl transaction with
     * the driver is complete, or until some error condition occurs.
     */
    /*
     * This routine is called only from strioctl or from the stream
     * head teardown code (via i_unlink).  strioctl holds the ioctl
     * semaphore which single-threads all ioctls on a given stream.
     */
    lis_head_get(hd) ;
    /*
     * Use RTN to return below here.
     */
    iocb->ioc_id = hd->sd_iocseq =  lis_incr(&lis_iocseq);
    /*
     * Reset items that could be not clean after a previous failing ioctl
     * (ioctl returning on signal or error)
     * - semaphore > 0
     * - sd_iocblk != NULL
     */
    lis_sem_init(&hd->sd_wiocing, 0) ;
    hd->sd_iocblk = NULL ;

  again:				/* wioc lock is always held when here */
    if (err < 0)
    {
	CP(hd,err) ;
	ioc->ic_len = 0 ;		/* no data */
	RTN(err) ;
    }

    if ( LIS_DEBUG_IOCTL )
	printk("lis_strdoioctl: send %s to \"%s\" on stream %s, cmd(0x%x)\n",
			lis_msg_type_name(mioc),
			lis_queue_name(hd->sd_wq->q_next),
			hd->sd_name, iocb->ioc_cmd) ;
    SET_SD_FLAG(hd,IOCWAIT);		/* for strrput */
    CLOCKADD() ;			/* exclude driver time */
    CP(hd,0) ;
    lis_putnext(hd->sd_wq,mioc);	/* do not use PUTNEXT */
    CLOCKON() ;
    CP(hd,0) ;
    mioc=NULL;			
    CLOCKADD() ;
    CP(hd,0) ;
    switch (ioc->ic_cmd)
    {
    case I_LINK:
    case I_PLINK:
    case I_UNLINK:
    case I_PUNLINK:
	ignore_errors = 1 ;
	break ;
    }

    err = lis_wait_for_wiocing(hd,ioc->ic_timout, ignore_errors) ;
    CLR_SD_FLAG(hd,IOCWAIT);		/* now completed */
    if (err < 0 || (mioc = hd->sd_iocblk) == NULL)	/* rtnd ioctl msg */
    {
	CP(hd,err) ;
	ioc->ic_len = 0 ;		/* no data */
	if (hd->sd_werror)		/* pick up error from M_ERROR if pres */
	    err = hd->sd_werror ;
	else
	if (hd->sd_rerror)
	    err = hd->sd_rerror ;
	RTN(err) ;
    }
    /* Here we have! */
    CLOCKON() ;
    hd->sd_iocblk = NULL ;
    if ( LIS_DEBUG_IOCTL )
	printk("lis_strdoioctl: %s from \"%s\" on stream %s\n",
		    lis_msg_type_name(mioc),
		    lis_queue_name(hd->sd_wq->q_next),
		    hd->sd_name) ;

    msgtype = lis_btype(mioc) ;
    CP(hd,msgtype) ;
    switch(msgtype)
    {
    case M_IOCACK:
	iocb=(iocblk_t*)mioc->b_rptr;
	if (iocb->ioc_error){
	    ioc->ic_len = 0 ;		/* no data */
	    RTN(-(iocb->ioc_error));
	}

	{
	    int rval=iocb->ioc_rval;
	    int len = iocb->ioc_count ;
	    mblk_t *dat=lis_unlinkb(mioc);

	    if (do_copyin)
	    {
		if (len > 0)
		{				/* data to return */
		    if ((err=lis_check_umem(f,VERIFY_WRITE,ioc->ic_dp,len)) < 0)
			rval = err ;		/* oops... */
		    else
		    {
			copyout_blks(f,ioc->ic_dp, len, dat);
			dat = NULL;		/* see copyout_blks() */
			ioc->ic_len = len ;	/* length */
		    }
		}
		else				/* no data to return */
		if (dat)			/* however, have empty bfr */
		    ioc->ic_len = 0 ;		/* so set len to zero */
	    }

	    if (dat)
		lis_freemsg(dat);

	    RTN(rval);
	}
    case M_IOCNAK:
	iocb=(iocblk_t*)mioc->b_rptr;
	{
	    int res=-(iocb->ioc_error);
	    ioc->ic_len = 0 ;		/* no data */
	    RTN(res ? res : -EINVAL);
	}
	break ;
    case M_COPYIN:
	{
	    copyreq_t *cp=(copyreq_t*)mioc->b_rptr;
	    copyresp_t*cr=(copyresp_t*)mioc->b_rptr;
	    u_int      len=cp->cq_size;
	    mblk_t    *dat=NULL;
	    char      *ubuf=cp->cq_addr;

	    err = 0 ;

	    dat = mioc->b_cont ;
	    if (dat != NULL)
	    {
		mioc->b_cont = NULL ;
		freemsg(dat) ;
		dat = NULL ;
	    }
	    lis_btype(mioc)=M_IOCDATA;
	    if (   (err=lis_check_umem(f,VERIFY_READ,ubuf,len)) < 0
		|| (dat=allocb(len,BPRI_LO))==NULL
		|| (err = lis_copyin(f,dat->b_wptr,ubuf,len)) < 0
	       )
	    {
		if (err < 0)
		    cr->cp_rval=(caddr_t)(long)(-err);
		else
		    cr->cp_rval=(caddr_t)ENOMEM;
		if (dat != NULL)
		    lis_freemsg(dat);
	    }
	    else
	    {
		dat->b_wptr += len ;
		lis_linkb(mioc,dat);
		cr->cp_rval=0;
	    }

	    mioc->b_wptr = mioc->b_rptr + sizeof(*cr) ;
	    goto again;			/* keep holding wioc lock */
	}
    case M_COPYOUT:
	{
	    copyreq_t *cp=(copyreq_t*)mioc->b_rptr;
	    copyresp_t*cr=(copyresp_t*)mioc->b_rptr;
	    char      *ubuf=cp->cq_addr;
	    mblk_t    *dat;
	    u_int      len=cp->cq_size;

	    lis_btype(mioc)=M_IOCDATA;
	    if ((err=lis_check_umem(f,VERIFY_WRITE,ubuf,len)) >= 0){
		dat=lis_unlinkb(mioc);
		if ((err = copyout_blks(f,ubuf,len,dat)) < 0)
		    cr->cp_rval=(caddr_t)(long)(-err);
		else
		    cr->cp_rval=0;
	    } 
	    mioc->b_wptr = mioc->b_rptr + sizeof(*cr) ;
	    goto again;			/* keep holding wioc lock */
	}
    default:
	ioc->ic_len = 0 ;		/* no data */
	RTN(-EINVAL) ;
    }

return_point:

    if (mioc != NULL)
	lis_freemsg(mioc);
    lis_head_put(hd) ;
    CLOCKOFF(IOCTLTIME) ;
    return(errv) ;

#undef RTN

}/*lis_strdoioctl*/

/*  -------------------------------------------------------------------  */
/*
 * lis_lookup_stdata
 *
 * Look through all the stdata structures for one that has the
 * given device id.  Return a pointer to it.
 */
static stdata_t *
lis_lookup_stdata( dev_t *dev,
#if defined(LINUX)
		   struct dentry *from,
#else
		   struct inode  *from,
#endif
		   stdata_t *excluded )
{
    stdata_t *hd;
    int       n;
    dev_t     odev = *dev;

    lis_spin_lock(&lis_stdata_lock) ;
    if (lis_stdata_head == NULL) {
        lis_spin_unlock(&lis_stdata_lock);
	return(NULL);			/* no stdatas in use */
    }

    for (hd = lis_stdata_head, n = 0;
	 n == 0 || hd != lis_stdata_head;
	 hd = hd->sd_next, n++)
    {
	if (hd->magic != STDATA_MAGIC)
	{
	    printk("lis_lookup_stdata(...) h@0x%p <MAGIC 0x%lx!=0x%lx>\n",
		   hd, hd->magic, STDATA_MAGIC) ;
	    break ;				/* don't believe sd_next */
	}

	if (hd != excluded)
	{
	    /*
	     * As long as the stream head is allocated the following
	     * tests are valid -- we don't need the head lock.  Owning
	     * the stdata_lock prevents free_stdata from actually freeing
	     * the stream head, but if it is that far along we will notice
	     * the STRCLOSE flag and omit looking at it.
	     */
	    if (F_ISSET(hd->sd_flag, STRCLOSE))	/* stream is closing */
		continue ;			/* don't look at this entry */
	    /*
	     *  this allows us to reopen a cloned FIFO inode
	     */
	    if (from && hd->sd_from && hd->sd_inode
#if defined(LINUX)
		&& (hd->sd_from->d_inode == from->d_inode)
#endif
		) 
	    {
		if ((LIS_DEBUG_OPEN && LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
		    printk("lis_lookup_stdata(d0x%x,%c@0x%p/%d,h@0x%p/%d/%d)"
			   " <reopen>\n    >> "
			   "h@0x%p/%d/%d i@0x%p/%d dev 0x%x\n",
			   DEV_TO_INT(odev),
#if defined(LINUX)
			   'd', from, (from?D_COUNT(from):0),
#endif
			   excluded,
			   (excluded?LIS_SD_REFCNT(excluded):0),
			   (excluded?LIS_SD_OPENCNT(excluded):0),
			   hd,
			   LIS_SD_REFCNT(hd), LIS_SD_OPENCNT(hd),
			   hd->sd_inode, I_COUNT(hd->sd_inode),
			   DEV_TO_INT(hd->sd_dev));
		*dev = hd->sd_dev;
		lis_spin_unlock(&lis_stdata_lock) ;	/* done with lock */
		return(hd);
	    }
	    if (DEV_SAME(hd->sd_dev, odev))		/* same maj/minor */
	    {
		if ((LIS_DEBUG_OPEN && LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
		    printk("lis_lookup_stdata(d0x%x,%c@0x%p/%d,h@0x%p/%d/%d)"
			   " <same>\n    >> "
			   "h@0x%p/%d/%d i@0x%p/%d\n",
			   DEV_TO_INT(odev),
#if defined(LINUX)
			   'd', from, (from?D_COUNT(from):0),
#endif
			   excluded,
			   (excluded?LIS_SD_REFCNT(excluded):0),
			   (excluded?LIS_SD_OPENCNT(excluded):0),
			   hd,
			   LIS_SD_REFCNT(hd), LIS_SD_OPENCNT(hd),
			   hd->sd_inode, I_COUNT(hd->sd_inode));
		lis_spin_unlock(&lis_stdata_lock) ;	/* done with lock */
		return(hd);
	    }
	}
    }

    lis_spin_unlock(&lis_stdata_lock) ;

    if ((LIS_DEBUG_OPEN && LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
	printk("lis_lookup_stdata(d0x%x,%c@0x%p/%d,h@0x%p/%d/%d)"
	       " => NULL\n",
	       DEV_TO_INT(odev),
#if defined(LINUX)
	       'd', from, (from?D_COUNT(from):0),
#endif
	       excluded,
	       (excluded?LIS_SD_REFCNT(excluded):0),
	       (excluded?LIS_SD_OPENCNT(excluded):0));

    return(NULL);			/* not found */
}

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */
/*  -------------------------------------------------------------------  */

/*  -------------------------------------------------------------------  */
/* lis_open_fifo							 */
/*
 * Special code for opening a FIFO file.
 */
static
int lis_open_fifo(struct inode *i, struct file *f, stdata_t *head,
		  int this_open, dev_t *ndev, cred_t *creds)
{
    int		maj = getmajor(head->sd_dev) ;
    int		err = 0;

    CP(head,*ndev) ;
    if (LIS_DEV_IS_CLONE(maj))
	lis_setq( head->sd_rq,
		  clone_info.st_rdinit, clone_info.st_wrinit );
    else
	lis_setq( head->sd_rq,
		  fifo_info.st_rdinit, fifo_info.st_wrinit );

    lis_new_stream_name(head, f) ;

    head->sd_peer = head;
    SET_SD_FLAG( head, STFIFO );
    F_SET( head->sd_wropt, SNDZERO|SNDPIPE );

    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
	printk("lis_open_fifo(i@0x%p/%d,f@0x%p/%d,h@0x%p/%d/%d)#%d"
	       " \"%s\"\n",
	       i, I_COUNT(i),
	       f, F_COUNT(f),
	       head, LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head),
	       this_open, FILE_NAME(f));

    K_ATOMIC_DEC(&lis_in_syscall) ;	/* "done" with system call */
    lis_runqueues() ;
    err = lis_qopen( head->sd_rq, ndev, f->f_flags, creds ); 
    CP(head,*ndev) ;
    K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
    if (err == 0)
	lis_setq(head->sd_rq, fifo_info.st_rdinit, fifo_info.st_wrinit);

    return(err) ;
}

/*  -------------------------------------------------------------------  */
/*
 * check_for_wantenable
 *
 * Run down the list of queues originiating from the passed in head
 * and see if any of them have the QWANTENB bit set.
 *
 * This is performed at the end of open operations (or push) so we also
 * clear the QOPENING flag while we are at it.
 */
static void check_for_wantenable(stdata_t *hd)
{
    lis_flags_t  psw ;
    unsigned	 flags ;
    queue_t	*q ;
    queue_t	*oq ;

    lis_head_get(hd) ;
    for (q = hd->sd_wq; q != NULL; q = q->q_next)
    {
	LIS_QISRLOCK(q, &psw) ;
	flags = q->q_flag ;
	q->q_flag &= ~(QOPENING | QFROZEN) ;
	LIS_QISRUNLOCK(q, &psw) ;
	lis_do_deferred_puts(q) ;
	lis_retry_qenable(q) ;

	oq = OTHER(q) ;
	LIS_QISRLOCK(oq, &psw) ;
	flags = oq->q_flag ;
	oq->q_flag &= ~(QOPENING | QFROZEN) ;
	LIS_QISRUNLOCK(oq, &psw) ;
	lis_do_deferred_puts(oq) ;
	lis_retry_qenable(oq) ;
    }
    lis_head_put(hd) ;
}

/*  -------------------------------------------------------------------  */
/* lis_freezestr, lis_unfreezestr
 *
 * Use the QFROZEN bit to freeze the entire stream represented by the
 * passed-in queue.
 *
 * The QFROZEN mechanism is an incomplete implementation of the standard
 * freezestr() construct.  The difference is that putq/getq/open/close can
 * take place for an LiS frozen stream, but standard semantics says not
 * to call these routines.
 */
void _RP lis_freezestr(queue_t *q)
{
    stdata_t	*hd ;
    stdata_t	*hd_peer ;
    queue_t	*oq = NULL ;
    lis_flags_t  psw ;

    if (q == NULL || (hd = q->q_str) == NULL || F_ISSET(hd->sd_flag,STRFROZEN))
	return ;

    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
	printk("freezestr: stream %s queue %s\n",
		hd->sd_name, lis_queue_name(q)) ;

    lis_head_get(hd) ;			/* "put" in unfreezestr */
    SET_SD_FLAG(hd, STRFROZEN) ;

    if (   (hd_peer = hd->sd_peer) && hd_peer != hd
	&& !F_ISSET(hd_peer->sd_flag,STRFROZEN))
    {
	SET_SD_FLAG(hd_peer, STRFROZEN) ;
	lis_head_get(hd_peer) ;
    }

    for (q = hd->sd_wq; q != NULL; q = q->q_next)
    {
	LIS_QISRLOCK(q, &psw) ;
	q->q_flag |= QFROZEN ;
	LIS_QISRUNLOCK(q, &psw) ;

	oq = OTHER(q) ;
	LIS_QISRLOCK(oq, &psw) ;
	oq->q_flag |= QFROZEN ;
	LIS_QISRUNLOCK(oq, &psw) ;
    }
}

void _RP lis_unfreezestr(queue_t *q)
{
    stdata_t	*hd ;
    stdata_t	*hd_peer ;

    if (q == NULL || (hd = q->q_str) == NULL || !F_ISSET(hd->sd_flag,STRFROZEN))
	return ;

    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
	printk("unfreezestr: stream %s queue %s\n",
		hd->sd_name, lis_queue_name(q)) ;

    check_for_wantenable(hd) ;

    CLR_SD_FLAG(hd, STRFROZEN) ;

    if (   (hd_peer = hd->sd_peer) && hd_peer != hd
	&& F_ISSET(hd_peer->sd_flag, STRFROZEN))
    {
	CLR_SD_FLAG(hd_peer, STRFROZEN) ;
	lis_head_put(hd_peer) ;
    }

    /*
     * STREAM write or putmsg may be sleeping due to encountering a
     * frozen stream.  If so, wake them up now.
     */
    lis_wake_up_all_wwrite(hd);		/* wake up all write/putmsg */

    lis_head_put(hd) ;			/* balance the "get" */
}

/*  -------------------------------------------------------------------  */
/* lis_stropen - open a stream (see Magic Garden 7.9.3 and 7.9.4)
 *
 * The stream head represents a single stream.  It has the status of
 *  an inode in the operating system.  As such it can be referenced by
 *  multiple files, but there must be a single inode per stream.  This
 *  routine enforces that relationship.  Moreover, the inode corresponding
 *  to a stream will be independent of any (other) filesystem.  This
 *  makes it safe (once again) to have stdata structures and inodes keep
 *  references to each other.
 */

static int get_sd_opening_sem(stdata_t *head)
{
    int		ret ;

    CP(head,head->sd_dev) ;
    if ((ret = lis_down(&head->sd_opening)) == 0)
	SET_SD_FLAG(head, STROSEM_HELD) ;
    return(ret) ;
}

static void release_sd_opening_sem(stdata_t *head)
{
    CLR_SD_FLAG(head, STROSEM_HELD) ;
    lis_up(&head->sd_opening) ;
}

int 
lis_stropen( struct inode *i, struct file *f )
{
    struct stdata *head = NULL;
    cred_t	   creds;
    dev_t	   odev ;
    dev_t	   ndev ;
    int		   err = 0;
    int		   hd_locked = 0;
    int		   stdata_locked = 0 ;	/* have stdata_sem */
    int		   dev_is_clone ;	/* opening the clone driver */
    int		   maj, mnr;
    int		   existing_head = 0 ;
    int		   f_count = F_COUNT(f) ;	/* file open count */
    unsigned long  time_cell = 0;
    unsigned long  this_open ;
#if defined(LINUX)
    struct dentry   *from;
    struct dentry   *oldd;
    struct vfsmount *oldmnt;
    int oldd_cnt, oldmnt_cnt;
#else
    struct inode *from;
#endif

    CHECK_INO(i,"stropen");		/* may return */

    CLOCKON() ;

    K_ATOMIC_INC(&lis_open_cnt);
    this_open = K_ATOMIC_READ(&lis_open_cnt);

#if defined(LINUX)
    from       = f->f_dentry;  /* save this for lookup_stdata() */
    /*
     *  we save the following for passing to cleanup_file_opening(),
     *  and bump the counts in case of a failure.  I.e., if these are
     *  non-LiS, we'll put() them once during this process, but we
     *  can't let them be disposed of, since if the open fails, the OS
     *  will try to dispose of them also.  This extra initial get on
     *  them will keep them around, and we'll account for it (via the
     *  counts we save here) in cleanup_file_opening().
     */
    oldd       = lis_dget(f->f_dentry);
    oldd_cnt   = D_COUNT(oldd)-1;
    oldmnt     = FILE_MNTGET(f);
    oldmnt_cnt = MNT_COUNT(oldmnt)-1;
#else
    from       = FILE_INODE(f);
#endif

    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
    {
	printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld\n"
	       "    << i_rdev=(%d,%d) f_mode=0%o f_flags=0x%x"
	       " <[%d] %d LiS inode(s), %d open stream(s)>\n",
	       i, I_COUNT(i), f, f_count, this_open,
	       getmajor(GET_I_RDEV(i)),
	       getminor(GET_I_RDEV(i)), f->f_mode, f->f_flags,
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       K_ATOMIC_READ(&lis_stdata_cnt)) ;

	if (LIS_DEBUG_REFCNTS) {
#if defined(LINUX)
	    printk("    << d@0x%p/%d m@0x%p/%d\n",
		   oldd, oldd_cnt, oldmnt, oldmnt_cnt);
#endif
	    lis_show_inode_aliases(i);
	}
    }

    /*
     * For more information about race conditions see "Analysis of
     * open/close locking" near the top of this file.
     *
     * Get a stream head structure to associate with the file.
     * The principle of STREAMS is that if a program opens some
     * particular {maj,min} device then all such opens should funnel
     * through the same stream head structure.  The exceptions are if
     * the maj is the clone device or if the CLONEOPEN flag is set,
     * in which case there is always a new structure allocated; and
     * FIFOs, which use CLONEOPEN and allocate a new structure if
     * the underlying inode is not open.  (Note in the latter case
     * that in typical implementations, FIFOs have their own file
     * type and are not char special - i.e., they don't have (maj,min) -
     * but LiS can't do this since Linux kernel FIFOs use that type,
     * so LiS has no choice but to implement them as char specials.)
     *
     * Re-opening any other existing {maj,min} mainly just causes
     * an increment of the reference/open count, but it should be noted
     * that a driver can do whatever it wants to do when CLONEOPEN
     * is set.  It will be passed a new head, but it can use an old
     * one if it chooses.  This routine must handle the case where
     * a new head is rejected by a driver in favor of an existing one.
     *
     * SMP considerations:  Open may be running concurrently on several
     * CPUs and all of them may be after the same {maj,min}.  One
     * opener needs to perform a full open and the others need to
     * find the result of this open and use it.  This implies holding
     * a semaphore during the open so that the "losers" can sleep.
     *
     * We use a global semaphore to serialize opens until the identity
     * of the head structure can be established.  Then the opener gets
     * a semaphore in the head structure for the remainder of the
     * open.  It holds this semaphore even across the call to the
     * user's open routine.  This is because we must not let a second
     * opener in until the file is completely opened by the first opener.
     */
    K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
    odev = GET_I_RDEV(i);		/* get {maj,min} */
    maj  = getmajor(odev);
    mnr  = getminor(odev);

retry_from_start:			/* retry point for open/close races */

    dev_is_clone = LIS_DEV_IS_CLONE(maj) ;
    if ((err = lis_down(&lis_stdata_sem)) < 0) 
	goto error_rtn ;

    stdata_locked = 1 ;			/* need "up" when exit */
    if ((head = FILE_STR(f)) != NULL)
    {
	existing_head = 1 ;
	head = lis_head_get(head) ;	/* increase ref cnt */
	CP(head,odev) ;
    }
    else
    {					/* opening new stream */
	/*
	 * Always get a new stream head structure for a clone open.
	 */
	if (dev_is_clone ||
	    (head = lis_lookup_stdata(&odev, from, NULL)) == NULL)
	{
	    existing_head = 0 ;
	    head = lis_head_get(NULL) ;	/* allocates new structure */
	    CP(head,odev) ;
	    if (head == NULL)
	    {
		err = -ENOMEM ;
		goto error_rtn ;
	    }

	    /*
	     * In order for lis_lookup_stdata to locate this stream head
	     * in case of a simultaneous open to this same device by another
	     * thread, we have to plant the maj/min device id in the stream
	     * head structure now.
	     *
	     * Note however, that for a just-allocated stream head structure
	     * the open count is still zero.
	     */
	    head->sd_dev = odev ;
	}
	else
	{
	    existing_head = 1 ;
	    lis_head_get(head) ;	/* incrs ref count */
	    maj  = getmajor(odev);	/* odev may have changed */
	    mnr  = getminor(odev);
	    dev_is_clone = LIS_DEV_IS_CLONE(maj) ;
	    CP(head,odev) ;
	}

	if (!head)			/* can't proceed w/o a strm head */
	{
	    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
		printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld\n"
		       "    >> failed to allocate stream head <ENOSR>\n",
		       i, I_COUNT(i), f, f_count, this_open);

	    err = -ENOSR ;
	    goto error_rtn ;
	}
    }

    if (stdata_locked)
    {
	lis_up(&lis_stdata_sem);	/* let go global semaphore */
	stdata_locked = 0 ;
    }

    /*
     * Releasing stdata_sem could allow this stream to be closed by
     * another thread.
     *
     * We want to get the stream's opening semaphore under circumstances
     * in which there is not other open in progress on this stream.
     */
    if ((err = get_sd_opening_sem(head)) < 0)
	goto error_rtn ;

    CP(head,odev) ;
    hd_locked = 1 ;			/* now have head's opening sem */

    /*
     * If the sd_dev does not match the device that we are trying to open
     * then we are in a race with another open.  Most likely is that this
     * head was used for a clone open and the sd_dev changed.
     *
     * We need to start over.
     */
    if (existing_head && !DEV_SAME(head->sd_dev, odev))
    {					/* clone open changed sd_dev */
	CP(head,odev) ;
	release_sd_opening_sem(head) ;
	hd_locked = 0 ;
	lis_head_put(head) ;
	goto retry_from_start ;		/* start over again */
    }

    /*
     * If the STRCLOSE flag is set then we are in a race with
     * the close routine.  The close routine is going to be messing
     * with this head structure, big time.  So we have to retry
     * from the top and get us a new head structure.  Note that this
     * one can no longer be found in a search due to the STRCLOSE
     * flag being set.
     */
    CP(head,odev) ;
    if (F_ISSET(head->sd_flag,STRCLOSE))
    {
	SET_FILE_STR(f, NULL);		/* unhook from file */
	CP(head,odev) ;
	release_sd_opening_sem(head) ;
	hd_locked = 0 ;
	lis_head_put(head) ;		/* give back use count */
	goto retry_from_start ;
    }

    /*
     * We now have the head to use and are protected against further
     * opens on this {maj,min} until we are finished.
     */

    creds.cr_uid  = (uid_t) EUID(f);
    creds.cr_gid  = (gid_t) EGID(f);
    creds.cr_ruid = (uid_t) UID(f);
    creds.cr_rgid = (gid_t) GID(f);

    SET_FILE_STR(f, head);		/* point file to strm hd */

    /*
     * If this is a reopen of an existing stream then there is
     * not much to do.
     */
    if (LIS_SD_OPENCNT(head) >= 1)
    {
	CP(head,odev) ;
	if (head->magic != STDATA_MAGIC)	/* paranoia */
	{
	    printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld\n"
		   "    >> hd@0x%p MAGIC 0x%lx!=0x%lx <EINVAL>\n",
		   i, I_COUNT(i), f, f_count, this_open,
		   head, head->magic, STDATA_MAGIC) ;
	    err = -EINVAL ;
	    head = NULL ;		/* pretend no strm head */
	    goto error_rtn ;
	}

	/*
	 * Stream was created by previous open of device: sleep if still
	 * opening, then call every module open() and awake sleepers
	 *
	 *  FIXME - we're holding a head-specific lock here (sd_opening) -
	 *  the head might get replaced, and if so, the locking might
	 *  get fouled up (see drivers/str/connld.c).  -JB 12/6/02
	 */
	head->sd_file = f;     		      /* in case a mod needs to know */
	ndev = odev ;
	if ((err = open_mods( head, &ndev, f->f_flags, &creds )) < 0)
	{
	    CP(head,-err) ;
	    printk("lis_stropen(...)#%ld\n"
		   "    >> open_mods() error (%d)\n",
		   this_open, err) ;
	    goto error_rtn ;
	}

	head->sd_file = NULL;  /* done with this now */

	/*
	 *  'connld' (or the like) may have completely replaced
	 *  the stream - check for that
	 */
	if (i != FILE_INODE(f))
	{
	    CP(head,i) ;
	    i = FILE_INODE(f);
	    head = FILE_STR(f);
	    CP(head,i) ;
	}

	/*
	 * When re-opening an existing stream the driver is not allowed
	 * to change the dev.
	 */
	if (!DEV_SAME(ndev, odev))
	{
	    CP(head,EBUSY) ;
	    printk("lis_stropen(...)#%ld\n"
		   "    >> re-open changed dev from 0x%x to 0x%x error (%d)\n",
		   this_open, odev, ndev, err) ;
	    err = -EBUSY ;
	    goto error_rtn ;
	}

	goto successful_rtn ;
    }

    /*
     * This is a new stream head.  This is the long-form of the open
     * routine.  This is where we have to take clone devices into account.
     * See below for more.
     */
    CP(head,odev) ;
    if (LIS_DEV_CAN_REOPEN(maj))
    {
        /*
	 *  cloned minor devices of this major can be reopened -
	 *
	 *  we "grab" the current inode and keep it as sd_from, so
	 *  we can be sure it's valid if we need to check it
	 *  (we don't keep 'sd_from' if cloning from a clone major)
	 */
	CP(head,odev) ;
#if defined(LINUX)
        head->sd_from  = lis_dget(f->f_dentry);
	head->sd_mount = FILE_MNTGET(f);
#else
        head->sd_from = lis_grab_inode(FILE_INODE(f));
#endif
	SET_SD_FLAG(head, STREOPEN);
	if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
	    printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld\n"
		   "    >> <= %c@0x%p/++%d <can reopen>\n",
		   i, I_COUNT(i), f, f_count, this_open,
#if defined(LINUX)
		   'd', head->sd_from, D_COUNT(head->sd_from)
#else
		   'i', head->sd_from, I_COUNT(head->sd_from)
#endif
		);
    }

    /*
     * Now we want a new inode for the file.  We want the inode (and
     * accompanying dentry) to be allocated under the "root" of the
     * LiS file system.  lis_set_up_inode arranges for this.
     */
    i = lis_set_up_inode(f, i) ;		/* allocate an LiS inode */
    if (i == NULL)
    {
	err = -ENOMEM ;				/* couldn't allocate struct */
	goto error_rtn ;
    }
    SET_INODE_STR(i, head);			/* point new inode to strm hd */

    head->sd_strtab	= LIS_DEVST(maj).f_str;	/* use strmtab of opened strm */
    head->sd_rdopt	= RPROTNORM | RNORM ;
    head->sd_wropt	= 0 ;
    head->sd_maxpsz	= LIS_MAXPSZ;
    head->sd_minpsz	= LIS_MINPSZ;
    head->sd_closetime	= LIS_CLTIME;
    head->sd_dev	= odev ;
    head->sd_open_flags	= f->f_flags;
    ndev		= odev ;

    SET_SD_FLAG(head, STWOPEN) ;
    if ( LIS_DEV_IS_FIFO(maj) || (dev_is_clone && LIS_DEV_IS_FIFO(mnr)) )
    {
	CP(head,odev) ;
	err = lis_open_fifo(i, f, head, this_open, &ndev, &creds) ;
    }
    else
    {
	CP(head,odev) ;
	lis_setq( head->sd_rq, &strmhd_rdinit, &strmhd_wrinit );

	lis_new_stream_name(head, f) ;
	/*
	 * lis_qattach will call the user's open routine, which is allowed
	 * to sleep.  We are holding no locks and still have the
	 * semaphore for the stream head.  We hold this semaphore
	 * across this call.  Others simultaneous openers of this same
	 * stream will just have to wait.
	 */
	K_ATOMIC_DEC(&lis_in_syscall) ;	/* "done" with a system call */
	lis_runqueues() ;
	err = lis_qattach( head, head->sd_strtab, &ndev, f->f_flags, &creds );
	K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
    }

    if (err < 0)				/* failed qopen */
	goto error_rtn ;			/* go cleanup */

    /*
     * Queues are attached and the user's open routine has returned
     * success.  The only remaining complication is that it may have
     * been a clone open.  If not, we are pretty much done.
     *
     * If it is a clone open then we need to adopt the new {maj,min}.
     * It is possible that the newly assigned "ndev" will match the
     * {maj,min} of an already-open stream.  If that is the case then
     * we need to abandon the stream-head that we have here and
     * perform a directed open to the existing stream.  This is an
     * unusual case.
     *
     * The more usual case is that we just adopt the new {maj,min}
     * into the existing head structure.  In this case we want to
     * also allocate a new inode for the cloned device; the old one
     * really pointed to the clone driver instead of the target
     * driver.
     */
    if (!DEV_SAME(ndev, odev))			/* clone open */
    {						/* changed the device */
	stdata_t	*other_hd ;

	/*
	 * If the clone open redirects us to an existing stream then
	 * we need to close the stream just opened and open the
	 * new stream as if it were a directed (non-clone) file open.
	 *
	 * The lookup_stdata routine will find it for us.
	 */
	CP(head,odev) ;
	CP(head,ndev) ;
	if ((err = lis_down(&lis_stdata_sem)) < 0) 
	    goto error_rtn ;
	stdata_locked = 1 ;
#if defined(LINUX)
	from = f->f_dentry;
#else
	from = FILE_INODE(f);
#endif
	/*
	 * If the device id got changed by the clone driver then the
	 * driver's open routine has been called by the clone driver
	 * itself.
	 *
	 * If that device has not been opened then we will continue to
	 * open it using this stream head.  However, if the target driver
	 * picked a device that is already open that is considered an
	 * error.
	 *
	 * A simultaneous directed open to ndev could have allocated the
	 * head pointed to by other_hd.  This means that the clone picked
	 * an in-use dev, which is an error.
	 */
	other_hd = lis_lookup_stdata( &ndev, from, head );
	if (other_hd != NULL)			/* ndev is already in use */
	{
	    err = -EBUSY ;
	    printk("lis_stropen(...)#%ld\n"
		   "    >> clone-open picked in-use dev 0x%x error (%d)\n",
		   this_open, ndev, err) ;
	    goto error_rtn ;
	}

	/*
	 * This is the case in which the clone open is going to use the
	 * same stream head that was used to call the clone driver, but
	 * with the new dev value filled in.
	 *
	 * We still own the sd_opening semaphore on this stream.  We will
	 * hold it until we store the new dev into the head structure.
	 * Once that is done any simultaneous open directed to this dev
	 * will find this structure.
	 */
	CP(head,ndev) ;
	SET_FILE_STR(f, head);			/* point file to strm hd */
	maj  = getmajor(ndev);
	mnr  = getminor(ndev);
	head->sd_strtab = LIS_DEVST(maj).f_str;
	head->sd_dev = ndev ;
	head->sd_name[0] = 0 ;			/* regenerate name */
	lis_up(&lis_stdata_sem);		/* let go global semaphore */
	stdata_locked = 0 ;

	lis_new_stream_name(head, f);
	if (!lis_new_inode( f, ndev ))		/* yet another inode */
	{
	    err = -ENOMEM ;
	    goto error_rtn ;
	}

	head->sd_inode = i = FILE_INODE(f);	/* save the new inode */
	SET_INODE_STR(i, head);			/* make inode point to head */

	if ((LIS_DEBUG_OPEN || LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
	    printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld\n"
		   "    >> <clone->new> h@0x%p/%d/%d \"%s\""
		   " i@0x%p/%d\n",
		   i, I_COUNT(i), f, f_count, this_open,
		   head,
		   LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head),
		   head->sd_name,
		   FILE_INODE(f), I_COUNT(FILE_INODE(f)));
    }
    else
    {			/* ndev == odev non-clone */
	/*
	 *  We always want a new inode for a stream; if head->sd_inode
	 *  isn't set yet, we may have a non-STREAMS inode, that is,
	 *  one that is not owned by the LiS "root" file system.
	 *
	 *  The reason we always want a new inode for a stream is so
	 *  that non-STREAMS file systems won't muck about with them;
	 *  i.e., we want streams inodes to be independent of other
	 *  filesystems
	 *
	 *  Coincidentally (for now), this also provides a means to
	 *  identify a stream; the i_dev major in an inode for a
	 *  stream will be lis_major, and the i_dev minor will be the
	 *  major of the stream's driver.
	 */
	CP(head,odev) ;
	if (!head->sd_inode)
	{
	    CP(head,0) ;
	    head->sd_inode = i ;
	}
    }

    if ((err = lis_set_q_sync(head->sd_wq, LIS_QLOCK_QUEUE)) == 0)
	err = lis_set_q_sync(head->sd_wq->q_next,
			     LIS_DEVST(maj).f_qlock_option) ;
    if (err < 0)
	goto error_rtn ;

    { /* Do autopush */
	const char *mods[MAXAPUSH];
	int j, cnt;

	cnt = lis_apushm(GET_I_RDEV(i), mods);
	for (j = 0; j < cnt; ++j)
	{
	    int		 id = lis_loadmod(mods[j]) ;
	    streamtab_t *st = lis_modstr(id);

	    if (!st || id == LIS_NULL_MID) {
		/*
		 *  Module not present. Probably one of:
		 *  a) Module open for a previous autopush slept, giving
		 *     kerneld(8) or root time to unload our loadable module.
		 *  b) A configured loadable module could not be loaded.
		 */
		err = -ENOSR; /* Assume (a), retry may succeed */
		goto error_rtn;
	    }
	    
	    CP(head,st) ;
	    if (   (err = lis_down(&lis_fmod_sw[id].f_sem)) < 0
		|| (err = push_mod(head, st, id, &ndev, f->f_flags, &creds)) < 0
	       )
	        goto error_rtn; /* push failure */
	    lis_up(&lis_fmod_sw[id].f_sem) ;
	}
    }

successful_rtn:					/* returning success */

    CP(head,odev) ;
    err = 0;
    if (!existing_head)
    {						/* opening a new stream */
	head->sd_creds = creds ;		/* LiS creds */
    }

#if defined(LINUX)
    /*
     *  synchronize FIFO read/write openers after (almost) everything else 
     *
     *  this is really only needed for FIFOs being opened O_RDONLY
     *  or O_WRONLY; the only likely failure case is O_WRONLY|O_NONBLOCK
     *  when no reader is open, which will return -ENXIO.
     *
     *  note that this is also done for pipe ends but really isn't needed,
     *  since pipe ends are always opened O_RDWR; unfortunately, we
     *  can't tell at this point whether a FIFO being opened will become
     *  a pipe end or not (FIXME).
     */
    if (F_ISSET(head->sd_flag,STFIFO))
	if ((err = lis_fifo_open_sync(i,f)) < 0)
	    goto error_rtn;
    lis_cleanup_file_opening(f, head, 0,
			     oldd, oldd_cnt, oldmnt, oldmnt_cnt);
#endif

    /*
     * Increment our open count for the first open of the file.  For
     * subsequent opens leave the open count alone since we will only
     * get one call to close and that is when the file's count goes
     * to zero.  Similarly, we need to "put" the head structure because
     * we called "get" for this open, but there will be no matching close.
     */
    if (f_count == 1)
	K_ATOMIC_INC(&head->sd_opencnt) ;	/* count opens to stream */
    else
    if (f_count > 1)
	lis_head_put(head) ;

    if (hd_locked)
    {
	CP(head,odev) ;
	release_sd_opening_sem(head) ;
    }

    /*
     * Clear opening flag.  This allows the queue to be scheduled.
     */
    CLR_SD_FLAG(head, STWOPEN) ;
    if (LIS_DEBUG_OPEN || LIS_DEBUG_REFCNTS)
    {
	printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld\n"
	       "    >> h@0x%p/%d/%d \"%s\""
	       " i@0x%p/%d rq@0x%p wq@0x%p\n",
	       i, I_COUNT(i), f, f_count, this_open,
	       head,
	       LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head),
	       head->sd_name,
	       FILE_INODE(f), I_COUNT(FILE_INODE(f)),
	       head->sd_rq, head->sd_wq);
	if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) {
#if defined(LINUX)
	    printk("    >> d@0x%p/%d m@0x%p/%d\n",
		   f->f_dentry, D_COUNT(f->f_dentry),
		   f->f_vfsmnt, MNT_COUNT(f->f_vfsmnt));
#endif
	    lis_show_inode_aliases(i);
	}
	lis_print_stream(head) ;
	printk("lis_stropen(i@0x%p/%d,f@0x%p/%d)#%ld done OK...\n"
	       "    >> <[%d] %d LiS inode(s), %d open stream(s)>\n",
	       i, I_COUNT(i), f, f_count, this_open,
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       K_ATOMIC_READ(&lis_stdata_cnt)) ;
    }

    CP(head,-err) ;
    CLOCKOFF(OPENTIME) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */

    check_for_wantenable(head) ;	/* deferred queue enables */

    lis_runqueues();

    return(err);

error_rtn:				/* come here if fail */

    CP(head,odev) ;
    if (stdata_locked)
	lis_up(&lis_stdata_sem);

    if ((LIS_DEBUG_OPEN || LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
    {
	char	*name = "(unknown)";

	if (head)
	    name = head->sd_name ;

	printk("lis_stropen(%s)#%ld "
	       " >> ERROR (%d)\n"
	       "    >> <[%d] %d LiS inode(s), %d open stream(s)>\n",
	       name, this_open, err,
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       K_ATOMIC_READ(&lis_stdata_cnt)) ;
    }

    SET_FILE_STR(f, NULL);		/* unhook from file */

    if (head)			        /* a head was used */
    {
#if defined(LINUX)
	lis_cleanup_file_opening(f, head, err,
				 oldd, oldd_cnt, oldmnt, oldmnt_cnt);
#else
	lis_cleanup_file_opening(f, head, err) ;
#endif
	if (hd_locked)			/* have opening semaphore */
	{
	    CP(head,odev) ;
	    release_sd_opening_sem(head) ;
	}

	if (LIS_SD_OPENCNT(head) == 0)	/* last open */
	{
	    if (SAMESTR(head->sd_wq))	/* get q below strm head */
		lis_qdetach( LIS_RD(head->sd_wq->q_next), 1, 
			     head->sd_open_flags, &head->sd_creds );
	    lis_dismantle(head, &creds);/* deallocate strm head queue */
	}

	lis_head_put(head) ;
    }

    CP(head,err) ;
    CLOCKOFF(OPENTIME) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();

    return(err) ;

}/*lis_stropen*/

/*  -------------------------------------------------------------------  */
/* strwrite - write to a stream (see Magic Garden 7.9.5)
 */
ssize_t
lis_strwrite(struct file *fp, const char *ubuff, size_t ulen, loff_t *op)
{
    stdata_t *hd;
    mblk_t   *held;
    int		 chunk;
    int		 written=0; 
    int		 err,newmsg;
    unsigned long  time_cell = 0 ;
    struct inode  *i = FILE_INODE(fp);

#define RTN(e)  do { err=(e); goto return_point; } while (0)

    /* Just in case we're holding messages in the write side we use
       head->sd_wmsg msg to hold data to be sent. When it's time to send it we
       just lis_putnext() such message.  If we're not holding then we just
       lis_putnext() the message w/ the usr data.       */

    CHECK_INO(i,"lis_strwrite"); /* may return */

    if (ulen == LIS_GETMSG_PUTMSG_ULEN)
    {
	putpmsg_args_t	args ;

	if (   !(err = lis_check_umem(fp,VERIFY_READ,ubuff,sizeof(args)))
	    && !(err = lis_copyin(fp,&args,ubuff,sizeof(args)))
	   )
	    err = lis_strputpmsg(i, fp, args.ctl, args.dat,
				 args.band, args.flags);
	return(err) ;
    }

    hd = FILE_STR(fp) ;
    if (hd == NULL)
	return(-ENODEV);

    CLOCKON() ;
    lis_head_get(hd) ;
    K_ATOMIC_INC(&lis_in_syscall) ;		/* processing a system call */

    if (ulen < hd->sd_minpsz)
    {
	if (LIS_DEBUG_WRITE)
	  printk(
	    "strwrite: stream %s: write length %d less than min size of %ld\n",
			hd->sd_name,
			ulen, hd->sd_minpsz) ;

	RTN(-EINVAL);
    }

    if (!ulen && !F_ISSET(hd->sd_wropt,SNDZERO)) /* we don't send anything */
    {
	if (LIS_DEBUG_WRITE)
	  printk(
	    "strwrite: stream %s: write length zero and SNDZERO not set\n",
			hd->sd_name) ;

	RTN(0);
    }

#if defined(LINUX) && defined(KERNEL_2_1)
    if (F_ISSET(hd->sd_flag,STFIFO)) {
	if ((err = lis_fifo_write_sync( i, 0 )) < 0) {
	    RTN(err);
	}
    }
#endif

    if ((err=lis_check_umem(fp,VERIFY_READ,ubuff,ulen))<0)
    {
	if (LIS_DEBUG_WRITE)
	  printk(
	    "strwrite: stream %s: user address 0x%lx cannot be read from\n",
			hd->sd_name, (long)ubuff) ;

	RTN(err);
    }

    if (F_ISSET(hd->sd_flag,STPLEX))
    {
	if (LIS_DEBUG_WRITE)
	  printk("strwrite: stream %s: stream has been I_LINKed\n",
		    hd->sd_name) ;
	RTN(-EINVAL) ;
    }

    do {
	if (F_ISSET(hd->sd_flag,STWRERR))
	{
	    if (LIS_DEBUG_WRITE)
	      printk(
		"strwrite: stream %s: stream has received M_ERROR %d\n",
			hd->sd_name, hd->sd_werror) ;
	    RTN(-hd->sd_werror);
	}

	if (F_ISSET(hd->sd_flag,STRCLOSE))
	{
	    if (LIS_DEBUG_WRITE)
	      printk("strwrite: stream %s: stream is closed or closing\n",
		      hd->sd_name) ;
	    RTN(-ENODEV);
	}

	/* bb - was incorrect, stream must return ENXIO rather than ENODEV
	 * when M_HANGUP received, this is clear. */
	if (F_ISSET(hd->sd_flag,STRHUP))
	{
	    if (LIS_DEBUG_WRITE)
	      printk("strwrite: stream %s: stream has received M_HANGUP\n",
		      hd->sd_name) ;
	    RTN(-ENXIO);
	}

	/* write packets until all data has been written. */
	if ((err = lis_lockq(hd->sd_wq)) < 0)	/* exclude wake_up_wwrite */
	    RTN(err) ;

	if (   K_ATOMIC_READ(&hd->sd_wrcnt) != 0
	    || !lis_bcanputnext(hd->sd_wq,0)
	    || F_ISSET(hd->sd_flag, STRFROZEN))
	{
	    if (should_notblock_wr(hd,fp))
	    {
		if (LIS_DEBUG_WRITE)
		    printk(
		     "strwrite: stream %s: flw cntrl blocked, return EAGAIN\n",
			    hd->sd_name) ;
		lis_unlockq(hd->sd_wq) ;
		RTN(-EAGAIN);
	    }

	    if (LIS_DEBUG_WRITE)
		printk("strwrite: stream %s: flw cntrl blocked, sleep\n",
			    hd->sd_name) ;
	    CLOCKADD() ;
	    err = lis_wait_on_wwrite(hd);	/* unlocks & relocks sd_wq */
	    CLOCKON() ;
	    if (err < 0)
	    {
		/* queue is unlocked upon error return */
		RTN(err) ;
	    }
	    lis_unlockq(hd->sd_wq) ;		/* unlock for loop or rtn */
	    continue ;		/* go back and retry errs and canput */
	}

	lis_unlockq(hd->sd_wq) ;

	/* get mblk if there's not one
	 *
	 * This section of code must be single threaded if there are
	 * multple writes running simultaneously on different CPUs.
	 *
	 * The semaphore protects sd_wmsg.
	 */
	lis_down(&hd->sd_write_sem) ;
	held=hd->sd_wmsg;
	chunk=lis_min((int)hd->sd_maxpsz,ulen-written);
	newmsg = (held==NULL);
	if ( held == NULL && 
	     (held=hd->sd_wmsg=allocb(chunk+hd->sd_wroff,BPRI_LO))==NULL)
	{
	    lis_up(&hd->sd_write_sem) ;
	    RTN(written?written:-ENOMEM);
	}

	/* now copy from usr to mblk (type M_DATA by default) */
	if (newmsg)
	{
	    held->b_rptr =
	    held->b_wptr = held->b_datap->db_base + hd->sd_wroff;
	    if (F_ISSET(hd->sd_flag,STRDELIM))		/* delimited msg */
		held->b_flag |= MSGDELIM ;
	}

	if (held->b_datap->db_lim - held->b_wptr >= chunk)    /* enough room */
	{
	    if ((err = lis_copyin(fp,held->b_wptr,ubuff,chunk)) < 0)
	    {
		lis_up(&hd->sd_write_sem) ;
		RTN(err) ;
	    }

	    ubuff        += chunk;
	    written      += chunk;
	    held->b_wptr += chunk;

	    if (strholding(hd) && 
		held->b_datap->db_lim - held->b_wptr > written)
	    {					/* hold for more data */
		if (LIS_DEBUG_WRITE)
		    printk("strwrite: stream %s: "
			   "hold message %d bytes for scan timer\n",
			    hd->sd_name, chunk) ;

		if (newmsg) 
		    start_scanl_timer(hd); 

		lis_up(&hd->sd_write_sem) ;
		RTN(written);
	    }
	}        


        /*
         * Either there was not enough room in the existing message
         * to hold the next write, or we wrote to a new message
         * and the write-holding mechanism is not active.
         *
         * In either case, now is the time to send the message downstream.
         */
        if (hd->sd_scantimer != 0)		/* scantimer running */
        {
	    untimeout(hd->sd_scantimer) ;	/* cancel the timer */
	    hd->sd_scantimer = 0 ;		/* note timer is not running */
        }

	if (LIS_DEBUG_WRITE)
	    printk(
		"strwrite: stream %s: send %d bytes downstream to %s\n",
		hd->sd_name, chunk, lis_queue_name(hd->sd_wq->q_next)) ;

	LisUpCount(WRITECNT) ;
	CLOCKADD() ;				/* exclude driver time */
        PUTNEXT(hd->sd_wq,hd->sd_wmsg);		/* put & maybe run queues */
	CLOCKON() ;
        hd->sd_wmsg = NULL ;			/* don't reuse the message */
	lis_up(&hd->sd_write_sem) ;

    } while (written<ulen);

    RTN(written);

return_point:

    lis_head_put(hd) ;
    CLOCKOFF(WRITETIME) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();
    return(err) ;

#undef RTN

}/*lis_strwrite*/

/*  -------------------------------------------------------------------  */
/* lis_check_m_sig
 *
 * Check the stream head read queue to see if there is an M_SIG at the
 * head of the queue.  If there is, remove it and signal the user.
 *
 */
static void lis_check_m_sig(stdata_t *hd)
{
    mblk_t	*hdmp ;

    hdmp  = hd->sd_rq->q_first ;
    if (hdmp == NULL || lis_btype(hdmp) != M_SIG)
	return ;

    hdmp  = getq(hd->sd_rq) ;
    LisDownCount(MSGQDSTRHD) ;			/* one fewer msg queued */
    signalusr(*hdmp->b_rptr,hd);
    freemsg(hdmp) ;
    if (   POLL_WAITING(hd)
	|| F_ISSET(hd->sd_flag,STRSELPND)
       )
	lis_wake_up_poll(hd,POLLMSG);
}

/*  -------------------------------------------------------------------  */
/* lis_requeue - requeue a message in stream head queue
 *
 * We need to pay attention to intr disables.  If we are re-inserting
 * a high priority message and an interrupt has meanwhile inserted
 * a high priority message at the head of the queue, then we should
 * delete the new message and insert our old one.  As if the new
 * message arrived at the stream head with ours still in place.
 *
 * The caller holds the queue lock.
 */
static void lis_requeue(stdata_t *hd, mblk_t *mp)
{
    mblk_t	*hdmp ;
    queue_t	*hd_rq;
    lis_flags_t  psw ;

    hd_rq = hd->sd_rq;
    /*
     * If we are about to insert a PCPROTO into the queue we need
     * to check to see if the current head message is also a
     * PCPROTO, and delete it if it is.
     *
     * Setting the STRPRI bit keeps strrput from adding any more
     * PCPROTOs to the queue.
     */
    if (lis_btype(mp) == M_PCPROTO)
    {
	SET_SD_FLAG(hd,STRPRI);
	LIS_QISRLOCK(hd_rq, &psw) ;
	hd_rq->q_flag |= QREADING ;		/* for strrput */
	LIS_QISRUNLOCK(hd_rq, &psw) ;
	hdmp  = hd_rq->q_first ;
	if (hdmp != NULL && lis_btype(hdmp) == M_PCPROTO)
	{				/* delete msg at head of q */
	    hdmp = lis_getq(hd_rq) ;
	    lis_freemsg(hdmp) ;
	    LisDownCount(MSGQDSTRHD) ;	/* one fewer msg queued */
	}

	LIS_QISRLOCK(hd_rq, &psw) ;
	hd_rq->q_flag &= ~QREADING;
	LIS_QISRUNLOCK(hd_rq, &psw) ;
    }

    lis_putbqf(hd_rq,mp);
    LisUpCount(MSGQDSTRHD) ;		/* one more msg queued */

} /* lis_requeue */

/*  -------------------------------------------------------------------  */
/* lis_strread - read from a stream head
 */
ssize_t 
lis_strread(struct file *fp, char *ubuff, size_t ulen, loff_t *op)
{
    stdata_t *hd;
    queue_t  *hd_rq = NULL ;
    mblk_t   *mp;
    lis_flags_t  psw ;
    int		 count=0;
    int		 qlocked = 0 ;
    int		 qisrlocked = 0 ;
    unsigned int chunk;
    int		 nbytes ;
    int		 msg_lgth ;
    int		 msg_marked ;
    int		 msgs_read = 0 ;
    int		 mread_sent ;
    int		 msg_is_pc_proto = 0 ;
    int err;
    unsigned long  time_cell = 0 ;
    struct inode *i = FILE_INODE(fp);

#define	RTN(val)  do { err = val; goto return_point; } while (0)

    CHECK_INO(i,"lis_strrread");

    if (ulen == LIS_GETMSG_PUTMSG_ULEN)
    {
	getpmsg_args_t	args ;

	if (   !(err = lis_check_umem(fp,VERIFY_READ,ubuff,sizeof(args)))
	    && !(err = lis_copyin(fp,&args,ubuff,sizeof(args)))
	   )
	    err = lis_strgetpmsg(i, fp, args.ctl, args.dat,
				 args.bandp, args.flagsp, 1);
	return(err) ;
    }
    
    hd = FILE_STR(fp) ;
    if (hd == NULL)
	return(-ENODEV);

    lis_head_get(hd) ;
    /*
     * The very first thing is to sleep on the semaphore that single
     * threads read operations.  getpmsg uses the same semaphore.
     */
    if ((err = lis_sleep_on_read_sem(hd)) < 0)
	return(err) ;				/* signalled out */

    CLOCKON() ;
    K_ATOMIC_INC(&lis_in_syscall) ;		/* processing a system call */

    if ((err=lis_check_umem(fp,VERIFY_WRITE,ubuff,ulen))<0)
    {
	if (LIS_DEBUG_READ)
	  printk(
	    "strread: stream %s: user buffer at 0x%lx cannot be written into\n",
		hd->sd_name, (long)ubuff) ;

	RTN(err);
    }

    if (F_ISSET(hd->sd_flag,STPLEX))
    {
	if (LIS_DEBUG_READ)
	  printk("strread: stream %s: stream has been I_LINKed\n", hd->sd_name);

	RTN(-EINVAL) ;
    }

    hd_rq=hd->sd_rq;
    if (!LIS_CHECK_Q_MAGIC(hd_rq)) RTN(-EINVAL) ;

    do {
	if ((err = lis_lockq(hd_rq)) < 0)	/* exclude strrsrv */
	    RTN(err) ;

	qlocked = 1 ;		/* return flag so as to unlock */

	mread_sent = 0 ;
	LIS_QISRLOCK(hd_rq, &psw) ;
	qisrlocked = 1 ;
	hd_rq->q_flag |= QREADING ;		/* for strrput */
	while (hd_rq->q_first == NULL && msgs_read == 0)
	{					/* nothing to read */
	    if (F_ISSET(hd->sd_flag,STRDERR))
	    {
		if (LIS_DEBUG_READ)
		  printk(
		    "strread: stream %s: stream has received M_ERROR %d\n",
			hd->sd_name, hd->sd_rerror) ;
		RTN(-hd->sd_rerror);
	    }
	    
	    if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    {
		if (LIS_DEBUG_READ)
		  printk("strread: stream %s: stream has received M_HANGUP\n",
			 hd->sd_name) ;

		RTN(0);
	    }

	    if (!mread_sent && F_ISSET(hd->sd_flag,SNDMREAD))
	    {				/* only send once per user call */
		hd_rq->q_flag &= ~QREADING;
		LIS_QISRUNLOCK(hd_rq, &psw) ;
		qisrlocked = 0 ;
		lis_unlockq(hd_rq) ;
		qlocked = 0 ;
		if (LIS_DEBUG_READ)
		    printk(
			"strread: stream %s: send M_READ downstream to %s\n",
			hd->sd_name, lis_queue_name(hd->sd_wq->q_next)) ;

		err = snd_mread(hd->sd_wq,ulen);
		if (err < 0)		/* probably ENOMEM */
		    RTN(err) ;

		mread_sent = 1 ;
		if ((err = lis_lockq(hd_rq)) < 0)
		    RTN(err) ;

		qlocked = 1 ;
		LIS_QISRLOCK(hd_rq, &psw) ;
		qisrlocked = 1 ;
		hd_rq->q_flag |= QREADING ;		/* for strrput */
		continue ;		/* go around again */
	    }

	    /*
	     * read queue empty, about to block.
	     */
	    if (should_notblock_rd(hd,fp))
	    {
		if (LIS_DEBUG_READ)
		    printk(
			"strread: stream %s: O_NONBLOCK set, return EAGAIN\n",
			hd->sd_name) ;

		RTN(-EAGAIN);
	    }

	    if (LIS_DEBUG_READ)
		printk("strread: stream %s: O_NONBLOCK not set, sleep\n",
			hd->sd_name) ;
	    hd_rq->q_flag &= ~QREADING;
	    LIS_QISRUNLOCK(hd_rq, &psw) ;
	    qisrlocked = 0 ;
	    CLOCKADD() ;
	    err = lis_sleep_on_wread(hd);	/* unlocks & relocks q */
	    CLOCKON() ;
	    if (err < 0)
	    {
		qlocked = 0;			/* sd_rq not locked on error */
		RTN(err) ;
	    }

	    LIS_QISRLOCK(hd_rq, &psw) ;
	    qisrlocked = 1 ;
	    hd_rq->q_flag |= QREADING ;		/* for strrput */
	}

	hd_rq->q_flag &= ~QREADING;
	LIS_QISRUNLOCK(hd_rq, &psw) ;
	qisrlocked = 0 ;
	mp = lis_getq(hd_rq);			/* while q still isr-locked */
	lis_check_m_sig(hd) ;			/* watch for M_SIG in q */
	lis_unlockq(hd_rq) ;
	qlocked = 0 ;
	if (mp == NULL)				/* out of data to read */
	{
	    RTN(count);				/* rtn amount read */
	}

	LisDownCount(MSGQDSTRHD) ;
	msg_marked = mp->b_flag & MSGMARK;
	switch(lis_btype(mp))
	{
	case M_DATA:
	    msg_lgth = 0 ;
	    err = 0 ;
	    msgs_read++ ;
	    do
	    {
		/*
		 * If rptr > wptr the message block is ill-formed
		 * and nbytes will be negative.
		 */
		if (lis_btype(mp) != M_DATA)
		    nbytes = -1 ;		/* pretend invalid */
		else
		    nbytes = mp->b_wptr - mp->b_rptr;

		if (nbytes > 0)			/* safety check */
		{
		    chunk=lis_min(ulen-count,nbytes);
		    if ((err = lis_copyout(fp,mp->b_rptr,ubuff,chunk)) < 0)
			break ;			/* stop reading */

		    msg_lgth   += nbytes ;	/* keep track of msg lgth */
		    mp->b_rptr += chunk;	/* message read ptr */
		    ubuff      += chunk ;	/* user buffer ptr */
		    count      += chunk;	/* bytes copied to user */
		    nbytes     -= chunk ;	/* bytes left in msg */
		    LisUpCount(READCNT) ;
		}

		if (nbytes <= 0)		/* time for nxt blk */
		{				/* includes bad blocks */
		    mblk_t *dust=mp;		/* and zero-lgth msgs */
		    mp=lis_unlinkb(mp);
		    lis_freeb(dust);
		}

	    } while(count < ulen && mp);

	    if (mp != NULL)			/* not at msg boundary */
	    {
		if ( err == 0 && (hd->sd_rdopt & RMODEMASK) == RMSGD )
		    lis_freemsg(mp);		/* discard rest of msg */
		else				/* RMSGN or RNORM */
		    lis_requeue(hd, mp) ;	/* save rest of msg */
	    }
	    if (LIS_DEBUG_READ)
	      printk(
	      "strread: stream %s: RDOPT=0x%x user buffer %d bytes, "
	      "read %d error %d\n",
		   hd->sd_name, hd->sd_rdopt, ulen, count, err) ;
	    if (   ulen == count		/* filled user buffer */
		|| (hd->sd_rdopt & RMODEMASK) != RNORM
		|| msg_lgth == 0		/* 0-lgth msg */
		|| msg_marked			/* at marked msg */
	       )				/* return to user */
	    {
	        RTN(count);
	    }

	    if (err < 0)			/* something went wrong */
		RTN(err) ;

	    break;
	case M_PCPROTO:
		CLR_SD_FLAG(hd,STRPRI);
		msg_is_pc_proto = 1 ;
		/* fall into next case */
	case M_PROTO: 
	    msgs_read++ ;
	    if (LIS_DEBUG_READ)
		printk(
		    "strread: stream %s: RDOPT=0x%x",
		    hd->sd_name, hd->sd_rdopt) ;
	    if (F_ISSET(hd->sd_rdopt,RPROTNORM))
	    {					/* return with EBADMSG */
		if (F_ISSET(hd->sd_rdopt,RPROTDIS))/* both bits set */
		{
		    if (LIS_DEBUG_READ)
			printk(" discard M_PROTO msg, return EBADMSG\n") ;
		    lis_freemsg(mp);		/* discard the messasge */
		}
		else
		{
		    if (LIS_DEBUG_READ)
			printk(" re-queue M_PROTO msg, return EBADMSG\n") ;
		    lis_requeue(hd, mp) ;	/* put back in queue */
		}
		RTN(-EBADMSG);			/* return */
	    }

	    if (F_ISSET(hd->sd_rdopt,RPROTDAT))/* treat M_PROTO as M_DATA */
	    {
		if (LIS_DEBUG_READ)
		    printk(" change M_PROTO msg to M_DATA\n") ;
		lis_btype(mp) = M_DATA;		/* change to M_DATA */
		lis_requeue(hd, mp) ;		/* put back in queue */
		break ;				/* go around again */
	    }

	    if (F_ISSET(hd->sd_rdopt,RPROTDIS))/* discard M_PROTO */
	    {					/* deliver M_DATA */
		mblk_t	*proto = mp;		/* the M_PROTO */
		if (LIS_DEBUG_READ)
		    printk(" discard M_PROTO block, retain M_DATA\n") ;
		mp = lis_unlinkb(mp);		/* mp is M_DATA or NULL */
		lis_freeb(proto);		/* discard proto */
		if (mp != NULL)			/* still have a msg */
		    lis_requeue(hd, mp) ;	/* put back in queue */
	    }					/* go around again */
	    else
	    {
		if (LIS_DEBUG_READ)
		    printk(" invalid rdopt, discard M_PROTO msg\n") ;

		lis_freemsg(mp);		/* discard the messasge */
		RTN(-EBADMSG);			/* return */
	    }

	    break;

	case M_READ:		/* add to-be-discarded msgs here */
	    if (LIS_DEBUG_READ)
		printk(
		    "strread: stream %s: read %s: discard\n",
		     hd->sd_name, lis_msg_type_name(mp)) ;
	    lis_freemsg(mp);
	    break;

	case M_PASSFP:
	    lis_free_passfp(mp);
	    RTN(-EBADMSG);  		/* DON'T discard */
	    
	case M_SIG:			/* should have been removed already */
	    lis_requeue(hd, mp) ;
	    mp = NULL ;
	    lis_check_m_sig(hd) ;		/* watch for M_SIG in q */
	    RTN(-EAGAIN) ;

	default:
	    if (LIS_DEBUG_READ)
		printk(
		    "strread: stream %s: read %s: return EBADMSG\n",
		     hd->sd_name, lis_msg_type_name(mp)) ;
	    lis_freemsg(mp);
	    RTN(-EBADMSG);
	}/*sw db_type*/
    } while (1);

return_point:
    if (hd_rq != NULL && qisrlocked)
    {
	hd_rq->q_flag &= ~QREADING;
	LIS_QISRUNLOCK(hd_rq, &psw) ;
    }

    if (hd_rq != NULL && qlocked)		/* do we have the rq locked? */
    {
	lis_check_m_sig(hd) ;			/* watch for M_SIG in q */
	lis_unlockq(hd_rq) ;
    }

    lis_wake_up_read_sem(hd) ;
    lis_head_put(hd) ;
    CLOCKOFF(READTIME) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();				/* run the streams queues */
    return(err) ;				/* return to user */

#undef	RTN

}/*lis_strrread*/

/*  -------------------------------------------------------------------  */
/* strputmsg - stream head handler for putmsg
 */
int
lis_strputpmsg(struct inode *i, struct file *fp, 
	       void *ctlp, void *datp, int band, int flags)
{
    int		    err = 0 ;
    int		    couldput ;
    mblk_t	   *msg;
    stdata_t	   *hd;
    const char     *name = "" ;
    strbuf_t	   *ctl = (strbuf_t *) ctlp ;
    strbuf_t	   *dat = (strbuf_t *) datp ;
    queue_t	   *q ;
    unsigned long  time_cell = 0 ;

#define	RTN(val)  do { err = val; goto return_point; } while (0)

    CHECK_INO(i,"lis_strputmsg"); /* may return */

    hd = FILE_STR(fp) ;
    if (hd == NULL)
	return(-ENODEV);

    CLOCKON() ;
    lis_head_get(hd) ;
    K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
    if (LIS_DEBUG_PUTMSG) name = lis_strm_name(hd) ;

    if (F_ISSET(hd->sd_flag,STPLEX))
    {
	if (LIS_DEBUG_PUTMSG)
	  printk( "strputmsg: stream %s: stream has been I_LINKed\n",
		    hd->sd_name) ;

	RTN(-EINVAL) ;
    }

#if defined(LINUX) && defined(KERNEL_2_1)
    if (F_ISSET(hd->sd_flag,STFIFO)) {
	ULOCK_INO(i);
	if ((err = lis_fifo_write_sync( i, 0 )) < 0) {
	    RTN(err);
	}
	LOCK_INO(i);
    }
#endif

    if (band >= 0)				/* putpmsg */
    {
	if ( (flags & (MSG_HIPRI|MSG_BAND)) == (MSG_HIPRI|MSG_BAND) )
	    RTN(-EINVAL) ;			/* flags are exclusive */
	if ( flags & ~(MSG_HIPRI|MSG_BAND) )
	    RTN(-EINVAL) ;			/* other flags set */
	if (flags == 0)
	    RTN(-EINVAL) ;			/* flags must be non-zero */
	if (F_ISSET(flags,MSG_HIPRI))		/* usr wants hi priority msg */
	    band = 0 ;				/* must be band zero */
    }
    else					/* putmsg */
    {
	if (flags & ~RS_HIPRI)
	    RTN(-EINVAL) ;			/* other flags set */
    }

    if ( (err=copyin_msg(fp,hd,&msg,ctl,dat))<0)	/* allocates msg */
    {
	if (msg)
	    freemsg(msg) ;
	RTN(err);
    }

    if (msg == NULL)			/* no message to send */
    {
	if (band >= 0)			/* putpmsg */
	{
	    if (flags & ~MSG_BAND)	/* asking for hi-pri */
		RTN(-EINVAL) ;		/* no can do */
	}
	else				/* putmsg */
	{
	    if (flags & RS_HIPRI)	/* asking for hi-pri */
		RTN(-EINVAL) ;		/* no can do */
	}

	RTN(0) ;			/* no options, no error */
    }

    if (band >= 0)			/* putpmsg */
    {
	if (F_ISSET(flags,MSG_HIPRI))		/* usr wants hi priority msg */
	{
	    if (lis_btype(msg) == M_PROTO)	/* is there a ctl part */
		lis_btype(msg)=M_PCPROTO;	/* yes */
	    else
	    {
		lis_freemsg(msg);
		RTN(-EINVAL) ;		/* no, sorry */
	    }
	}
	else					/* regular priority msg */
	if (F_ISSET(flags,MSG_BAND))		/* non-zero band */
	{
	    if (band >=0 && band <=LIS_MAX_BAND)	/* range check */
		lis_bband(msg) = (unsigned char) band;
	    else
	    {
		lis_freemsg(msg);
		RTN(-EINVAL);
	    }
	}
    }
    else					/* putmsg */
    {
	if (F_ISSET(flags,RS_HIPRI))		/* usr wants hi priority msg */
	{
	    if (lis_btype(msg) == M_PROTO)	/* is there a ctl part */
		lis_btype(msg)=M_PCPROTO;	/* yes */
	    else
	    {
		lis_freemsg(msg);
		RTN(-EINVAL) ;			/* no, sorry */
	    }
	}
    }

    /*
     * wait for flow control 
     *
     * If we are sending a high priority message (M_PCPROTO) ignore flow
     * control and send the messasge downstream unconditionally.
     *
     * Always check for error or hangup.
     */
    do
    {
	if (F_ISSET(hd->sd_flag,STWRERR))
	{
	    if (LIS_DEBUG_PUTMSG)
	      printk("strputmsg: stream %s: stream has received M_ERROR %d\n",
			hd->sd_name, hd->sd_werror) ;

	    freemsg(msg) ;
	    RTN(-hd->sd_werror);
	}

	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	{
	    if (LIS_DEBUG_PUTMSG)
	      printk("strputmsg: stream %s: stream has received M_HANGUP\n",
			hd->sd_name) ;
	    freemsg(msg) ;
	    RTN(-ENODEV);
	}

	if (lis_btype(msg) == M_PCPROTO)	/* high priority msg */
	{
	    couldput = 1 ;			/* pretend flw ctl enabled */
	    break ;
	}

	if ((err = lis_lockq(hd->sd_wq)) < 0)	/* exclude wake_up_wwrite */
	{
	    freemsg(msg) ;
	    RTN(err) ;
	}

	couldput = (K_ATOMIC_READ(&hd->sd_wrcnt) == 0) &&
	    	   !F_ISSET(hd->sd_flag, STRFROZEN) &&
		   lis_bcanputnext(hd->sd_wq,
				   (unsigned char) (band < 0 ? 0 : band));
						    /* check the band */
	if (   !couldput			/* flow control blocked */
	    && should_notblock_wr(hd,fp)	/* NDELAY or NONBLOCK */
	   )
	{
	    if (LIS_DEBUG_PUTMSG)
	      printk("strputmsg: stream %s: flw cntrl blocked, return EAGAIN\n",
		      hd->sd_name) ;
	    freemsg(msg) ;

	    lis_unlockq(hd->sd_wq) ;
	    RTN(-EAGAIN);
	}

	if (!couldput)				/* flw cntrl blocked */
	{
	    if (LIS_DEBUG_PUTMSG)
		printk("strputmsg: stream %s: flw cntrl blocked, sleep\n",
			hd->sd_name) ;

	    CLOCKADD() ;
	    err = lis_wait_on_wwrite(hd);	/* unlocks & relocks sd_wq */
	    CLOCKON() ;
	    if (err < 0)
	    {
		/* queue is unlocked upon error return */
		lis_freemsg(msg) ;
		RTN(err) ;
	    }
	}

	lis_unlockq(hd->sd_wq) ;

    } while (!couldput) ;
	    
    if (LIS_DEBUG_PUTMSG)
    {
	printk(
	   "strputmsg: stream %s: send %d/%d ctl/data bytes downstream to %s\n",
	    hd->sd_name,
	    msg->b_datap->db_type == M_DATA ? 0 : lis_xmsgsize(msg),
	    lis_msgdsize(msg),
	    lis_queue_name(hd->sd_wq->q_next)) ;

	if (LIS_DEBUG_ADDRS)
	    printk("strputmsg: q = 0x%lx, msg = 0x%lx\n",
		    (long)hd->sd_wq, (long)msg);
    }

    q = hd->sd_wq ;
    LisUpCount(WRITECNT) ;
    CLOCKADD() ;				/* exclude driver time */
    lis_putnext(q,msg);				/* send downstream */
    						/* don't use PUTNEXT */
    CLOCKON() ;
    err = 0 ;

return_point:
    CLOCKOFF(PUTMSGTIME) ;
    lis_head_put(hd) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();
    return(err) ;

#undef RTN

}/*lis_strputpmsg*/

/*  -------------------------------------------------------------------  */
/* strgetmsg - stream head handler for getmsg
 *
 * If 'doit' is set then this is a getmsg or getpmsg system call.
 * In this case 'ctl', 'dat' and 'flagsp' are all user space addresses.
 * 'bandp' can be NULL if the call is from getmsg().  It will be
 * a non-null user-space address if the call is from getpmsg().
 *
 * If 'doit' is not set then this is an I_PEEK call.  In this case
 * 'ctl', 'dat' and 'flagsp' and 'bandp' point to kernel memory.
 *
 * Note that getmsg() and I_PEEK use one encoding of the flags
 * (the RS_...) and that getpmsg() uses another (the MSG_...).
 */
int
lis_strgetpmsg(struct inode *i, struct file *fp, 
	       void *ctlp, void *datp, int *bandp, int *flagsp,
	       int doit)
{
    strbuf_t	*ctl = (strbuf_t *) ctlp ;
    strbuf_t	*dat = (strbuf_t *) datp ;
    const char	*ctl_type = "" ;
    const char	*dat_type = "" ;
    int		 err= 0 ;
    int		 rtn = 0 ;
    int		 qlocked = 0 ;
    int		 qreading = 0 ;
    lis_flags_t  psw ;
    stdata_t	*hd;
    strbuf_t	 kctl,kdat;
    mblk_t	*mp;
    queue_t	*rdq = NULL;
    int		 band = 0 ;
    int		 flags = 0 ;
    int		 mtype ;
    unsigned long  time_cell = 0 ;

#define RTN(v)	do { err=(v); goto err_return_point; } while (0)

    CHECK_INO(i,"lis_strgetmsg"); /* may return */

    hd = FILE_STR(fp) ;
    if (hd == NULL)
	return(-ENODEV);

    lis_head_get(hd) ;
    /*
     * The very first thing is to sleep on the semaphore that single
     * threads read operations.  strread uses the same semaphore.
     */
    if ((err = lis_sleep_on_read_sem(hd)) < 0)
	return(err) ;				/* signalled out */

    CLOCKON() ;
    K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
    if (F_ISSET(hd->sd_flag,STRDERR))
	RTN(-hd->sd_rerror);
    else if (doit && ctl != NULL &&
            (err=lis_check_umem(fp,VERIFY_READ,ctl,sizeof(strbuf_t)))<0)
       RTN(err);
    else if (doit && dat != NULL &&
            (err=lis_check_umem(fp,VERIFY_READ,dat,sizeof(strbuf_t)))<0)
       RTN(err);
    else if (flagsp == NULL)
	RTN(-EFAULT);
    else if (doit && (err=lis_check_umem(fp,VERIFY_READ,flagsp,sizeof(int)))<0)
	RTN(err);
    else if (doit && bandp != NULL
	     && (err=lis_check_umem(fp,VERIFY_READ,bandp,sizeof(int)))<0)
	RTN(err);

    if (F_ISSET(hd->sd_flag,STPLEX))
    {
	if (LIS_DEBUG_GETMSG)
	  printk("strgetmsg: stream %s: stream has been I_LINKed\n",
		    hd->sd_name) ;

	RTN(-EINVAL) ;
    }

    rdq = hd->sd_rq ;
    if (!LIS_CHECK_Q_MAGIC(rdq)) RTN(-EINVAL) ;

    if (doit)
    {					/* fetch structs from usr space */
	if (ctl != NULL)
	{
	    if ((err = lis_copyin(fp,&kctl,ctl,sizeof(strbuf_t))) < 0)
		RTN(err) ;
	}
	else
	{
	    kctl.maxlen = -1 ;
	    kctl.len    = -1 ;
	    kctl.buf    = NULL ;
	}

	if (dat != NULL)
	{
	    if ((err = lis_copyin(fp,&kdat,dat,sizeof(strbuf_t))) < 0)
		RTN(err) ;
	}
	else
	{
	    kdat.maxlen = -1 ;
	    kdat.len    = -1 ;
	    kdat.buf    = NULL ;
	}

	if ((err = lis_copyin(fp,&flags,flagsp,sizeof(flags))) < 0)
	    RTN(err) ;
	if (doit && bandp != NULL)
	{
	    if ((err = lis_copyin(fp,&band,bandp,sizeof(band))) < 0)
		RTN(err) ;
	}
	else
	    band  = 0 ;

	if (kctl.maxlen >= 0 || kdat.maxlen >= 0)
	{
	    if ((err = lis_lockq(rdq)) < 0)
		RTN(err) ;

	    qlocked = 1 ;
	    while (   rdq->q_first == NULL
	           && (hd->sd_flag & (STRHUP | STRDERR | STRCLOSE)) == 0
	          )
	    {
		if (should_notblock_rd(hd,fp))
		    RTN(-EAGAIN);

		CLOCKADD() ;
		err = lis_sleep_on_wread(hd);	       /* unlocks & relocks q */
		CLOCKON() ;
		if (err < 0) 
		{
		    /* sd_rq is not locked on error */
		    qlocked = 0 ;
		    RTN(err) ;
		}
	    }
	}
    }
    else				/* fetch structs from kernel space */
    {
	kctl  = *ctl ;
	kdat  = *dat ;
	flags = *flagsp ;
	band  = *bandp ;
    }

    /*
     * Lock the rd queue.  Every exit from here on should go out
     * via the "return_point" label for cleanup purposes.  The queue
     * lock keeps the read service procedure from running and messing
     * with the queue.
     *
     * There is only one copy of getpmsg or read running per stream,
     * gated by the sd_read_sem.  So we don't have to worry about 
     * interference from other user calls.
     */
    if (!qlocked && (err = lis_lockq(rdq)) < 0)
	    RTN(err) ;

    qlocked = 1 ;
    LIS_QISRLOCK(rdq, &psw) ;
    rdq->q_flag |= QREADING;			/* for strrput */
    LIS_QISRUNLOCK(rdq, &psw) ;
    qreading = 1 ;

    if ((mp = rdq->q_first) == NULL)		/* mp is 1st msg */
    {						/* empty read queue */
	kctl.len = -1;				/* nothing present on queue */
	kdat.len = -1;
	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	{
	    kctl.len=0;			      /* so sez STREAMS pgrmr's guide */
	    kdat.len=0;
	    rtn = 0 ;
	}
	else
	if (F_ISSET(hd->sd_flag,STRDERR))
	    err = -hd->sd_rerror;
	/*
	 * Return the strbuf structures whether or not there are errors.
	 * copyout_msg will not modify our len values due to the NULL
	 * message parameter.
	 */
	rtn=copyout_msg(fp,NULL,&kctl,&kdat,
		    (strbuf_t*)ctl,(strbuf_t*)dat,doit) ;

	goto return_point ;			/* go return 'rtn' */
    }

    if (LIS_DEBUG_GETMSG && LIS_DEBUG_DMP_MBLK && LIS_DEBUG_ADDRS)
    {
	printk("lis_strgetpmsg: q=%lx, mp=%lx, mp->b_rptr=%lx\n",
				(long)rdq, (long)mp, (long)mp->b_rptr);
    }

    /*
     * At this point we have a message.  Ensure that it is
     * of the type that we want to process.
     */
    mtype = lis_btype(mp) ;
    if (LIS_DEBUG_GETMSG)
    {
	if (mtype != M_DATA)
	{
	    ctl_type = lis_msg_type_name(mp) ;
	    dat_type = lis_msg_type_name(mp->b_cont) ;
	}
	else
	{
	    ctl_type = "NULL-MSG" ;
	    dat_type = lis_msg_type_name(mp) ;
	}
    }

    switch (mtype)
    {
    case M_DATA:
    case M_PROTO:
    case M_PCPROTO:
    case M_SIG:
	break ;				/* OK, keep going */

    case M_PASSFP:
	mp = lis_getq(rdq);			/* remove from queue */
	LisDownCount(MSGQDSTRHD) ;
	lis_free_passfp(mp);
	mp = NULL ;
	lis_check_m_sig(hd) ;			/* watch for M_SIG in q */
	err = -EBADMSG;
	goto return_point;
    default:
	err = -EBADMSG;
	goto return_point;
    }

    if (doit)					/* it's get[p]msg() */
    {
	if (lis_hipri(mtype))			/* M_PCPROTO */
	{
	    CLR_SD_FLAG(hd,STRPRI);		/* about to remove the msg */
	    flags = MSG_HIPRI ;
	    band  = 0 ;
	}
	else					/* normal priority msg */
	{
	    switch (flags)
	    {
	    case MSG_HIPRI:			/* user wanted hi-pri only */
						/* same code as RS_HIPRI */
		err = -EBADMSG;			/* hi-pri not at head of q */
		goto return_point ;		/* go return 'rtn' */

	    case MSG_BAND:			/* wants from particular band */
		if (bandp == NULL)		/* getmsg, not getpmsg */
		{
		    err = -EINVAL;		/* invalid flags */
		    goto return_point ;		/* go return 'rtn' */
		}
		if (mp->b_band < (unsigned char) band) /* msg in wrong band */
		{
		    err = -EBADMSG;
		    goto return_point ;		/* go return 'rtn' */
		}
		break ;				/* OK, keep going */

	    case MSG_ANY:			/* wants msg from any band */
		if (bandp == NULL)		/* getmsg, not getpmsg */
		{
		    err = -EINVAL;		/* invalid flags */
		    goto return_point ;		/* go return 'rtn' */
		}
		flags = MSG_BAND ;		/* reduce to MSG_BAND case */
		break ;

	    default:				/* other flags or zero */
		if (   flags != 0 		/* invalid flags */
		    || (bandp != NULL && flags == 0)	/* getpmsg w/flags=0 */
		   )
		{
		    err = -EINVAL;			/* invalid flags */
		    goto return_point ;		/* go return 'rtn' */
		}
		break ;
	    }

	    band  = mp->b_band ;		/* band to return */
	}

	/*
	 * Now we commit to removing the message from the read queue
	 * Up until here error conditions could have left the message
	 * in the queue.
	 */
	mp = lis_getq(rdq);			/* remove from queue */
	if ( mp == NULL )
	{
	    printk("strgetpmsg: empty queue: addr of queue 0x%lx\n",
		    (long) rdq);
	    err = -EINVAL;
	    goto return_point;
	}

	LIS_QISRLOCK(rdq, &psw) ;
	rdq->q_flag &= ~QREADING;	/* no longer "reading" */
	LIS_QISRUNLOCK(rdq, &psw) ;
	qreading = 0 ;

	LisDownCount(MSGQDSTRHD) ;
	if (LIS_DEBUG_GETMSG && LIS_DEBUG_DMP_DBLK)
	{
	    lis_print_msg(mp, "lis_strgetpmsg", PRINT_DATA_RDWR) ;
	}

	if (mtype == M_SIG)			/* process M_SIG */
	{
	    lis_requeue(hd, mp) ;
	    mp = NULL ;
	    lis_check_m_sig(hd) ;		/* watch for M_SIG in q */
	    err = -EAGAIN ;
	    goto return_point ;
	}
    }
    else					/* it's I_PEEK */
    {						/* have to keep queue locked */
	mblk_t	*peekmp ;			/* msg to peek at */

	if (mtype == M_SIG)			/* not suitable msg */
	{					/* msg still in queue */
	    lis_check_m_sig(hd) ;		/* watch for M_SIG in q */
	    err = -EAGAIN ;
	    goto return_point ;
	}

	if (lis_hipri(mtype))			/* M_PCPROTO */
	    flags = MSG_HIPRI ;
	else					/* M_DATA or M_PROTO */
	if (flags == RS_HIPRI)			/* wants hi-priority only */
	{
	    err = -EBADMSG;
	    goto return_point ;			/* go return 'rtn' */
	}
	else
	    flags = MSG_BAND ;			/* regular priority */

	peekmp = lis_dupmsg(mp);		/* prevents deallocation */
	if (peekmp == NULL)
	{
	    err = -ENOMEM;
	    goto return_point;
	}

	mp = peekmp ;				/* hold onto the dup'd mp */

	LIS_QISRLOCK(rdq, &psw) ;
	rdq->q_flag &= ~QREADING;		/* no longer "reading" */
	LIS_QISRUNLOCK(rdq, &psw) ;
	qreading = 0 ;
    }

    /*
     * mp is either the removed message or the dup'd copy for PEEK.  So
     * it must be disposed of in either case before we exit.  That's what
     * this next section of code does.
     */

    /* 
     * copyout_msg will copy data to usr returning MORECTL,MOREDATA status
     */
    if ((rtn=copyout_msg(fp,mp,&kctl,&kdat,
			 (strbuf_t*)ctl,(strbuf_t*)dat,doit)) < 0)
	lis_freemsg(mp);
    else 
    if (doit)				/* getmsg/getpmsg */
    {
	LisUpCount(READCNT) ;
	if (rtn)			/* there's still ctl or data in msg */
	  lis_requeue(hd, mp) ;		/* re-queue */
	else
	  lis_freemsg(mp);		/* done with message now */
    }
    else				/* I_PEEK */
    {
	rtn = kctl.len >= 0 || kdat.len >= 0 ;	/* got something */
        lis_freemsg(mp);		/* done with dup'd message now */
    }

return_point:

    if (rdq != NULL && qlocked)
    {
	lis_check_m_sig(hd) ;		/* watch for M_SIG in q */
	lis_unlockq(rdq) ;
	qlocked = 0 ;			/* flag checked again below */
    }

    if (err == 0 && rtn >= 0)		/* return flags to user */
    {
	if (!doit || bandp == NULL)	/* I_PEEK or getmsg */
	{				/* different encoding of flags */
	    switch (flags)
	    {
	    case MSG_HIPRI:
		flags = RS_HIPRI ;
		break ;
	    case MSG_BAND:
	    case MSG_ANY:
	    case 0:
	    default:
		flags = 0 ;
		break ;
	    }
	}

	if (!doit)
	    *flagsp = flags ;		/* return flags to kernal space */
	else				/* return flags to user space */
	if ((err=lis_check_umem(fp,VERIFY_WRITE,flagsp,sizeof(int))) == 0)
	    err = lis_copyout(fp,&flags,flagsp,sizeof(flags));
    }

    if (err == 0 && rtn >= 0 && doit && bandp != NULL)		/* getpmsg() */
    {
	if ((err=lis_check_umem(fp,VERIFY_WRITE,bandp,sizeof(int))) == 0)
	    err = lis_copyout(fp,&band,bandp,sizeof(band));
    }
    else				/* getmsg or I_PEEK */
    if (!doit && bandp != NULL)		/* kernel pointer */
	*bandp = band ;

    if (LIS_DEBUG_GETMSG)
    {
      printk("strgetmsg:    maxlen     len    return=0x%x flags=0x%x band=%d",
					  err ? err : rtn,
							  flags,     band) ;
      printk(" stream %s\n", hd->sd_name) ;
      printk("  control:    %6d  %6d    %s\n",
		  kctl.maxlen, kctl.len, ctl_type) ;
      printk("     data:    %6d  %6d    %s\n",
		  kdat.maxlen, kdat.len, dat_type) ;
    }

err_return_point:				/* return "err" */

    if (rdq != NULL && qreading)
    {
	LIS_QISRLOCK(rdq, &psw) ;
	rdq->q_flag &= ~QREADING;
	LIS_QISRUNLOCK(rdq, &psw) ;
    }

    if (rdq != NULL && qlocked)
	lis_unlockq(rdq) ;

    lis_wake_up_read_sem(hd) ;
    lis_head_put(hd) ;
    CLOCKOFF(GETMSGTIME) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();
    return(err != 0 ? err : rtn);

#undef RTN

}/*lis_strgetpmsg*/


/*  -------------------------------------------------------------------  */
/* strioctl - handle IOCTLs at the stream head
 *
 * The sd_wioc semaphore is used to ensure that only one ioctl per
 * stream is outstanding at a time.  This helps to minimize the 
 * locking of the stream head structure.
 */
int
lis_strioctl( struct inode *i, struct file *f, unsigned int cmd,
	      unsigned long arg )
{
    stdata_t *hd;
    int err=0,flush_pri=0;
    cred_t creds;
    streamtab_t *st;
    int		 id;
    unsigned long  time_cell = 0 ;
    pid_t pid=(pid_t)PID(f);

#define RTN(v)	do { err=(v); goto return_point; } while (0)

    CHECK_INO(i,"lis_strioctl"); /* may return */

    if (LIS_DEBUG_IOCTL && LIS_DEBUG_ADDRS)
	printk("lis_strioctl(i@0x%p/%d,f@0x%p/%d,cmd=0x%x,arg=0x%lx)"
	       " << i_rdev=0x%x\n",
	       i, (i?I_COUNT(i):0),
	       f, (f?F_COUNT(f):0),
	       cmd, arg, (i?GET_I_RDEV(i):0)) ;

    hd = FILE_STR(f) ;
    if (hd == NULL)
	return(-ENODEV);

    CLOCKON() ;
    K_ATOMIC_INC(&lis_in_syscall) ;		/* processing a system call */
    lis_head_get(hd) ;

    creds.cr_uid  = (uid_t) EUID(f);
    creds.cr_gid  = (gid_t) EGID(f);
    creds.cr_ruid = (uid_t) UID(f);
    creds.cr_rgid = (gid_t) GID(f);

    if (F_ISSET(hd->sd_flag,STPLEX))
    {
	if (LIS_DEBUG_IOCTL)
	  printk("lis_strioctl(...) \"%s\" >> stream has been I_LINKed\n",
		    hd->sd_name) ;

	err = -EINVAL ;
	goto return_no_unlock ;
    }

    /*
     * Single-thread ioctls on this stream
     */
    if ((err = lis_lock_wioc(hd)) < 0)
	goto return_no_unlock ;


    switch( cmd )		/* the often an ioc is called the 
				 * closer it should be to the begin of the sw*/
    {
    case I_NREAD:		/* tested */
	{ 
	    int bytes=0;
	    int siz ;
	    if ((err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,sizeof(int)))<0)
		RTN(err);
	    if ((err = lis_lockq(hd->sd_rq)) < 0)
		RTN(err) ;
	    siz = lis_qsize(hd->sd_rq) ;	/* # msgs in queue */
	    if (hd->sd_rq->q_first)
		bytes=lis_msgdsize(hd->sd_rq->q_first);
	    lis_unlockq(hd->sd_rq) ;
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_NREAD,...) \"%s\" "
			">> bytes=%d size=%d err=%d\n",
			    hd->sd_name, bytes, siz, err);
	    err = lis_copyout(f,&bytes,(char*)arg,sizeof(int));
	    if (err == 0)
		RTN(siz) ;			/* rtn # msgs in queue */
	}
	break;
    case I_PEEK:
	{
	    strpeek_t pb;
	    strbuf_t *ctl=&(pb.ctlbuf), *dat=&(pb.databuf);
	    int band =0;
	    int res,flags;
	    if (   (err=lis_check_umem(f,VERIFY_READ,
				    (char*)arg,sizeof(strpeek_t)))<0
		|| (err=lis_check_umem(f,VERIFY_WRITE,
				    (char*)arg,sizeof(strpeek_t)))<0
		|| (err=lis_copyin(f,&pb,(char*)arg,sizeof(strpeek_t)))<0
	       )
		RTN(err);
            flags=(int)pb.flags;
	    res=lis_strgetpmsg(i,f,ctl,dat,&band,&flags,0); /*0-> just peek!*/
            pb.flags=(long)flags;
	    err = 0 ;
	    if (res >= 0)
		err = lis_copyout(f,&pb,(char*)arg,sizeof(strpeek_t));
	    if (err < 0)		/* copyout failed */
		res = err ;		/* need to return error */
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_PEEK,...)"
		       " \"%s\" >> flags=0x%x rslt=%d\n",
		       hd->sd_name, flags, res) ;
	    RTN(res);
	}
	break;
    case I_STR:	
	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    RTN(-ENXIO);
	else {
	    strioctl_t ioc;
	    int res;
	    if (   (err=lis_check_umem(f,VERIFY_READ,
				    (char*)arg,sizeof(strioctl_t)))<0
		|| (err=lis_check_umem(f,VERIFY_WRITE,
				    (char*)arg,sizeof(strioctl_t)))<0
		|| (err=lis_copyin(f,&ioc,(char*)arg,sizeof(strioctl_t)))<0
	       )
		RTN(err);
	    CLOCKADD() ;
	    res=lis_strdoioctl(f,hd,&ioc,&creds,1);
	    CLOCKON() ;
	    if (res == 0)
		res=lis_copyout(f,&ioc,(char*)arg,sizeof(strioctl_t));
	    if ( LIS_DEBUG_IOCTL )
	     printk("lis_strioctl(...,I_STR,...) "
		    " \"%s\" >> ic_cmd=0x%x ic_timout=%d ic_len=%d rslt=%d\n",
		    hd->sd_name,
		    ioc.ic_cmd, ioc.ic_timout, ioc.ic_len, res) ;
		
	    RTN(res);
	}
	break;
    case I_PUSH:		/* tested */
	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    RTN(-ENXIO);

	if ((err=lis_check_umem(f,VERIFY_READ,(char*)arg,FMNAMESZ+1))<0)
	    RTN(err);
	else {
	    char *mname;
	    if((err=lis_copyin_str(f,(char*)arg,&mname,FMNAMESZ+1))<0)
		RTN(err);

	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_PUSH,\"%s\") %s <<\n",
			mname, hd->sd_name) ;

	    id = lis_loadmod(mname) ;
	    if (id == LIS_NULL_MID || (st = lis_modstr(id)) == NULL)
	    {
		err=-EINVAL;
		if ( LIS_DEBUG_IOCTL )
		    printk("lis_strioctl(...,I_PUSH,\"%s\")"
			   " >> module not found\n", mname) ;

		FREE((caddr_t)mname);
	    }
	    else
	    {
		dev_t ddev = GET_I_RDEV(i);

		FREE((caddr_t)mname);
		err = push_mod( hd, st, id, &ddev, f->f_flags, &creds );
		if ( LIS_DEBUG_IOCTL )
		{
		    printk("lis_strioctl(...,I_PUSH,\"%s\") >> ", mname) ;
		    if (err)	printk("error %d\n", err) ;
		    else	printk("OK\n") ;
		    lis_print_stream(hd) ;
		}

		if (F_ISSET(hd->sd_flag,STRISTTY)){
		    /* 
		       we should make this the controling terminal...
		     */
		    hd->sd_pgrp = PGRP(f);
		}
	    }
	}
	break;
    case I_POP:		/* tested */
      {
	const char	*qname = "";

	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    RTN(-ENXIO);

	if ( LIS_DEBUG_IOCTL )
	    qname = lis_queue_name(hd->sd_wq->q_next) ;

	err=pop_mod(hd,f->f_flags, &creds); 

	if ( LIS_DEBUG_IOCTL )
	{
	    printk("lis_strioctl(...,I_POP,\"%s\") \"%s\" >> ",
		    qname, hd->sd_name) ;

	    if (err)	printk("error %d\n", err) ;
	    else	printk("OK\n") ;
	    lis_print_stream(hd) ;
	}
      }
      break;
    case I_LOOK:		/* tested */
      {
	int	namelen ;
	char   *name ;
	if ((err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,FMNAMESZ+1))<0)
	{
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_LOOK,...)"
		       " >> lis_check_umem failed with err=%d\n",
			err);
	    RTN(err);
	}
	if (!hd->sd_pushcnt)
	{
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_LOOK,...)"
		       " >> failed - no modules on stream %d\n",
		-EINVAL);
	    RTN(-EINVAL);
	}
	if ( LIS_DEBUG_IOCTL )
	  printk("lis_strioctl(...,I_LOOK,...)"
		 " >> top driver/module of stream %s is \"%s\"\n",
		 hd->sd_name, lis_queue_name(hd->sd_wq->q_next)) ;

	name = (char *) lis_queue_name(hd->sd_wq->q_next) ;
	namelen = strlen(name) ;
	if (namelen > FMNAMESZ)
	    namelen = FMNAMESZ ;

	err = lis_copyout(f,name, (char*)arg, namelen+1);
	break;
      }
    case I_FLUSHBAND:
	{
	    struct bandinfo bi;
	    if (   (err=lis_check_umem(f,VERIFY_READ,
				    (char*)arg,sizeof(bandinfo_t)))<0
		|| (err=lis_copyin(f,&bi,(char*)arg,sizeof(bandinfo_t))) < 0
	       )
		RTN(err);
	    arg = (bi.bi_flag & FLUSHRW) | FLUSHBAND;
	    flush_pri=bi.bi_pri;
	}
        /*fall thru...*/
    case I_FLUSH:
	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    RTN(-ENXIO);
	if ( LIS_DEBUG_IOCTL )
	    printk("lis_strioctl(...,I_FLUSH,0x%lx)\n", arg) ;

	switch(arg & FLUSHRW){
	case FLUSHR:
#ifdef FIFO_IMPL
	    if (LIS_DEV_IS_FIFO(getmajor(GET_I_RDEV(i)))){
	      /*flush rw/other-wr for fifo;*/
	    }
	    else
#endif
	    {
	      int res = lis_snd_mflush(hd->sd_wq,arg,flush_pri);
	      RTN(res);
	    }
	    break;
	case FLUSHW:
#ifdef FIFO_IMPL
	    if (LIS_DEV_IS_FIFO(getmajor(GET_I_RDEV(i)))){
		/*flush rw/other-wr for fifo;*/
	    }
	    else
#endif
	    {
	      int res=lis_snd_mflush(hd->sd_wq,arg,flush_pri);
	      RTN(res);
	    }
	    break ;
	case FLUSHRW:
#ifdef FIFO_IMPL
	    if (LIS_DEV_IS_FIFO(getmajor(GET_I_RDEV(i)))){
		/*flush rd/other-rd;*/
	    }
	    else
#endif
	    {
		int res=lis_snd_mflush(hd->sd_wq,arg,flush_pri);
		RTN(res);
	    }
	default:
	    RTN(-EINVAL);
	}
	break;
    case I_SETSIG:
	if (!arg){
	    if (lis_del_from_elist(&(hd->sd_siglist),pid,S_ALL))
		err = -EINVAL;
	    else 
		if (!hd->sd_siglist)
		    hd->sd_sigflags=0;
	}
	else 
	if (lis_add_to_elist(&(hd->sd_siglist),pid, (short) arg))
	    err = -EAGAIN;
	else
	    F_SET(hd->sd_sigflags,arg);

	if ( LIS_DEBUG_IOCTL || LIS_DEBUG_SIG )
	{
	    printk("lis_strioctl(...,I_SETSIG,%ld) \"%s\" >> ",
		   arg, hd->sd_name) ;
	    if (err)	printk("error %d\n", err) ;
	    else	printk("OK\n") ;
	}

	break;
    case I_GETSIG:
	{
	    int evs;

	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_GETSIG,...) \"%s\" >> ",
		       hd->sd_name) ;

	    err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,sizeof(int)) ;
	    if (   err == 0
		&& (evs=(int)lis_get_elist_ent(hd->sd_siglist,pid)) == 0
	       )
		err = -EINVAL;

	    if ( LIS_DEBUG_IOCTL )
	    {
		if (err)	printk("error %d\n", err) ;
		else		printk("signal %d\n", evs) ;
	    }

	    if (err == 0)
		err = lis_copyout(f,&evs,(char*)arg,sizeof(int));
	}
	break;
    case I_FIND:		/* tested */
	{
	    char *mname = NULL;
	    queue_t *q;
	    int nmods = hd->sd_pushcnt;

	    if ((err=lis_check_umem(f,VERIFY_READ,(char*)arg,FMNAMESZ+1))<0)
		RTN(err);
	    if ((err=lis_copyin_str(f,(char*)arg,&mname,FMNAMESZ+1))<0)
		RTN(err);
	    if (!*mname)  /* no name here! */
	    {
		FREE(mname);
		RTN(-EINVAL);
	    }

	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_FIND,\"%s\") \"%s\" %d module(s)\n",
		       mname, hd->sd_name, nmods);

	    for (q=(hd->sd_wq ? hd->sd_wq->q_next : NULL);
		 q && nmods; q=q->q_next, nmods--)
	    {
		if (!LIS_CHECK_Q_MAGIC(q))
		{
		    FREE(mname) ;
		    RTN(-EINVAL);
		}
		if (strncmp(lis_queue_name(q),mname,FMNAMESZ) == 0)
		{
		    FREE(mname);
		    RTN(1);
		}
		if (!SAMESTR(q)) break ;
	    }

	    FREE(mname) ;
	    RTN(0);		/* not present */
	}
    case I_SRDOPT:
      {
	int res = set_readopt(&hd->sd_rdopt,arg);
	if ( LIS_DEBUG_IOCTL )
	    printk("lis_strioctl(...,I_SRDOPT,%ld) \"%s\" >> rslt=%d\n",
		   arg, hd->sd_name, res) ;
	RTN(res);
      }
    case I_GRDOPT:
	{
	    int rmode;
	    if ((err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,sizeof(int)))<0)
		RTN(err);
	    rmode=hd->sd_rdopt;
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_GRDOPT,...) \"%s\" << mode=0x%x\n",
		       hd->sd_name, rmode);
	    err = lis_copyout(f,&rmode,(char*)arg,sizeof(int));
	}	
	break;
    case I_FDINSERT:
      {
	/* This code compliments of Eric Levenez <levenez@club-internet.fr> */
	/* see magic garden 7.9.7 */
	strfdinsert_t	 strfdinsert;
	stdata_t	*ihd;
	queue_t		*q;
	mblk_t		*msg;
	int		 couldput;
	strbuf_t	*ctl;
	strbuf_t	*dat;

	/* primary sanity checks */

	if (F_ISSET (hd->sd_flag, STRDERR) || F_ISSET (hd->sd_flag, STWRERR))
	  RTN(-EINVAL);

	if (!hd->sd_wq->q_next)
	  RTN(-ENOSR);

	if (   (err = lis_check_umem (f, VERIFY_READ, (char *) arg,
				   sizeof (strfdinsert_t))) < 0
	    || (err = lis_copyin (f, &strfdinsert, (char *) arg,
				   sizeof (strfdinsert_t))) < 0
	   )
	    RTN(err);

	/* read structure command */


	/* sanity checks */
	if (   (strfdinsert.offset < 0)
	    || (strfdinsert.offset > 4096)
	    || (strfdinsert.ctlbuf.len < strfdinsert.offset + sizeof (char *))
	    || (strfdinsert.offset % sizeof (char *))
	    || (strfdinsert.fildes < 0)
	    || (strfdinsert.flags & ~RS_HIPRI))
	    RTN(-EINVAL);

	if ((ihd = lis_fd2str(strfdinsert.fildes)) == NULL)
	  RTN(-ENOSTR);

	lis_head_get(ihd) ;
	if (   F_ISSET (ihd->sd_flag, STRDERR)
	    || F_ISSET (ihd->sd_flag, STWRERR))
	{
	    lis_head_put(ihd) ;
	    RTN(-EINVAL);
	}

	if (F_ISSET (ihd->sd_flag, STPLEX))
	  RTN(-ENXIO);

	if (   (   (strfdinsert.databuf.len > 0)
		&& (strfdinsert.databuf.len < hd->sd_minpsz)
	       )
	    || (   (hd->sd_maxpsz != INFPSZ)
		&& (strfdinsert.databuf.len > hd->sd_maxpsz)
	       )
	   )
	{
	    lis_head_put(ihd) ;
	    RTN(-ERANGE);
	}

	/* find driver queue */

	for (q = ihd->sd_wq; q != NULL; q = q->q_next)
	{
	    if (!LIS_CHECK_Q_MAGIC(q))
	    {
		lis_head_put(ihd) ;
		RTN(-EINVAL) ;
	    }
	    if (!SAMESTR(q)) break ;
	}

	lis_head_put(ihd) ;			/* done with ihd */

	/* create the message a-la putpmsg */

	ctl = (strbuf_t *) (arg +
			    (unsigned long) &((strfdinsert_t *) NULL)->ctlbuf);
	dat = (strbuf_t *) (arg +
			    (unsigned long) &((strfdinsert_t *) NULL)->databuf);

	if ((err = copyin_msg (f, hd, &msg, ctl, dat)) < 0)
	{
	  if (msg)
	      freemsg(msg) ;
	  RTN(err);
	}

	if (!msg)
	  RTN(-EINVAL);

	/* for priority message */

	if (F_ISSET (strfdinsert.flags, RS_HIPRI))
	{
	  if (lis_btype (msg) == M_PROTO)
	    lis_btype (msg) = M_PCPROTO;
	  else
	  {
	    lis_freemsg (msg);
	    RTN(-EINVAL);
	  }
	}

	/* insert the read queue */

	*((queue_t **) (msg->b_rptr + strfdinsert.offset)) = LIS_RD (q);

	/* wait for flow control (code from lis_strputpmsg) */

	do
	{
	  if (F_ISSET (hd->sd_flag, STWRERR))
	  {
	    freemsg (msg);
	    RTN(-EINVAL);
	  }
	  if (F_ISSET (hd->sd_flag, STRHUP|STRCLOSE))
	  {
	    freemsg (msg);
	    RTN(-ENODEV);
	  }

	if ((err = lis_lockq(hd->sd_wq)) < 0)	/* exclude wake_up_wwrite */
	    RTN(err) ;

	  couldput = (K_ATOMIC_READ(&hd->sd_wrcnt) == 0) &&
	    	     !F_ISSET(hd->sd_flag, STRFROZEN) &&
		     lis_bcanputnext (hd->sd_wq, (unsigned char) 0);
	  if (   !couldput
	      && (   should_notblock_wr (hd, f)
		  || F_ISSET (strfdinsert.flags, MSG_HIPRI)
		 )
	     )
	  {
	    lis_unlockq(hd->sd_wq) ;
	    freemsg (msg);
	    RTN(-EAGAIN);
	  }

	  if (!couldput)
	  {
	    err = lis_wait_on_wwrite (hd);	/* unlocks & relocks sd_wq */
	    if (err < 0)
	    {
	      /* queue is unlocked upon error return */
	      lis_freemsg (msg);
	      RTN(err);
	    }
	  }
	  lis_unlockq(hd->sd_wq) ;
	}
	while (!couldput);

	/* send the message */

	if (LIS_DEBUG_IOCTL)
	    printk("lis_strioctl(...,I_FDINSERT,...)"
		    " \"%s\" >> q@0x%p q2@0x%p \n",
		   hd->sd_name, hd->sd_wq, LIS_RD(q));

	q = hd->sd_wq ;
	CLOCKADD() ;				/* exclude driver time */
	lis_putnext (q, msg);			/* do not use PUTNEXT */
	CLOCKON() ;
	RTN(0);
      }
      break;

    case I_SWROPT:
	if ( LIS_DEBUG_IOCTL )
	    printk("lis_strioctl(...,I_SWROPT,0x%lx) \"%s\"\n",
		   arg, hd->sd_name) ;
	hd->sd_wropt = arg ;
	RTN(0) ;
    case I_GWROPT:
	if ((err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,sizeof(int)))<0)
	    RTN(err);

	if ( LIS_DEBUG_IOCTL )
	    printk("lis_strioctl(...,I_GWROPT,...) \"%s\" >> mode=0x%x\n",
		   hd->sd_name, hd->sd_wropt);

	err = lis_copyout(f,&hd->sd_wropt,(char*)arg,sizeof(int));
	RTN(err);
    case I_SENDFD:
	if (LIS_DEBUG_IOCTL)
	    printk( "lis_strioctl(...,I_SENDFD,%d) \"%s\"\n",
		    (int)arg, hd->sd_name) ;
	RTN(lis_sendfd( hd, (unsigned int)arg, NULL ));
	
#if defined(I_E_RECVFD)
    case I_E_RECVFD:
#endif
    case I_RECVFD:
	if ((err = lis_check_umem( f, VERIFY_WRITE, (char*)arg,
				   sizeof(strrecvfd_t) )) < 0) {
	    RTN(err);
	} else {
	    strrecvfd_t recv;
	    
	    if (LIS_DEBUG_IOCTL)
		printk("lis_strioctl(...,I_RECVFD,...) \"%s\"\n",
		       hd->sd_name) ;
	
	    /*
	     *  synchronize with the sender if needed; we will wait
	     *  as long as an IOCTL would wait by default (i.e., 15 secs).
	     *  note that this potential wait can be "tuned" and/or avoided
	     *  by preceding the ioctl(I_RECVFD) with a poll(POLLIN), since
	     *  M_PASSFP is treated like a data message by poll().  If no
	     *  message is available after poll() has waited the specified
	     *  time, the caller can respond accordingly (possibly by not
	     *  calling ioctl(I_RECVFD) at all).
	     */
	    if ((err = lis_lockq(hd->sd_rq)) < 0)
		RTN(err) ;

	    if (!hd->sd_rq->q_first) {  /* any messages? */
		struct timer_list tl;
		int rslt;
		
		if (LIS_DEBUG_IOCTL)
		    printk("lis_strioctl(...,I_RECVFD,...)"
			   " >> waiting for an M_PASSFP\n" );
		lis_tmout( &tl, lis_do_rd_tmout,
			   (long)hd, SECS_TO(LIS_DFLT_TIM) );
		rslt = lis_sleep_on_wread(hd) ;	/* unlocks & relocks q */
		if (rslt  < 0) {
		    /* we got a signal... sd_rq is not locked on error*/
		    /* lis_unlockq(hd->sd_rq) ; */
		    lis_untmout(&tl);
		    RTN(rslt);
		}
		if (LIS_DEBUG_IOCTL)
		    printk( "lis_strioctl(...,I_RECVFD,...)"
			    " >> wait terminated\n" );
		lis_untmout(&tl);
	    }
	    
	    /*
	     *  we should have a message - it should be an M_PASSFP
	     *  if not, lis_recvfd will return an error
	     */
	    err = lis_recvfd( hd, &recv, f ) ;
	    lis_unlockq(hd->sd_rq) ;
	    if (err < 0)
		RTN(err);
	    
	    RTN(lis_copyout( f, (char *)&recv, (char *)arg,
			     sizeof(strrecvfd_t) ));
	}

    case I_LIST:
	if (!arg)		/* want number of modules */
	{
	    int nmods = hd->sd_pushcnt;

	    /* Add 1 to nmods (for driver) if not pipe/FIFO */
	    if (!F_ISSET(hd->sd_flag,STFIFO)) nmods++;

	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_LIST,NULL) \"%s\" >> return %d\n",
		       hd->sd_name, nmods) ;
	    RTN(nmods);
	}
	else {
	    str_list_t list;
	    queue_t   *q;
	    int	       nmods ;
	    if (   (err=lis_check_umem(f,VERIFY_READ,(char*)arg,
						sizeof(str_list_t)))<0
	        || (err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,
						sizeof(str_list_t)))<0
		|| (err=lis_copyin(f,&list,(char*)arg,sizeof(str_list_t)))<0
	       )
		RTN(err);
	    if (list.sl_nmods<=0)
		RTN(-EINVAL);
	    if ((err=lis_check_umem(f,VERIFY_WRITE,
				 list.sl_modlist,
				 sizeof(struct str_mlist)*list.sl_nmods)) <0)
		RTN(err);

	    if (hd->sd_wq == NULL || (q=hd->sd_wq->q_next) == NULL)
	    {
		printk("lis_strioctl(...,I_LIST,...)"
		       " \"%s\" >> NULL queue ptrs\n", hd->sd_name) ;
		RTN(0) ;			/* really a bug */
	    }

	    for (nmods = 0; 
		 q && list.sl_nmods-- ; 
		 q=q->q_next, list.sl_modlist++
		)
	    {
		if (!LIS_CHECK_Q_MAGIC(q)) break ;
		if ((err = lis_copyout(f,lis_queue_name(q),
			    list.sl_modlist->l_name,
			    FMNAMESZ)) < 0)
		{
		    RTN(err) ;
		}
		nmods++ ;
		if (!SAMESTR(q)) break ;
	    }
	    if ((err = lis_copyout(f, &nmods,
			    &((str_list_t *)arg)->sl_nmods, sizeof(int))) < 0)
		RTN(err) ;

	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_LIST,<non-NULL>)"
		       " \"%s\" >> %d modules, return 0\n",
		       hd->sd_name, nmods) ;
	    RTN(0);
	}
	break;
    case I_ATMARK:
	{
	    mblk_t	*mp ;
	    int		 rtn = 0 ;

	    /* Switch on arg and return whether the first msg in the
	     * stream head read queue is marked if arg is ANYMARK.
	     * Return whether is it the last marked message if arg is LASTMARK.
	     */
	    if (   hd->sd_wq == NULL		/* no queue */
		|| hd->sd_rq == NULL	/* no rd queue */
		|| (mp = hd->sd_rq->q_first) == NULL	/* no msg */
	       )
	    {
		rtn = 0 ;			/* obviously not marked */
	    }
	    else
	    switch (arg)
	    {
	    case ANYMARK:			/* is 1st msg marked?  */
		rtn = (mp->b_flag & MSGMARK) ? 1 : 0 ;
		break ;

	    case LASTMARK:			/* is 1st msg the last marked */
		if ( !(mp->b_flag & MSGMARK) )	/* 1st msg not marked at all */
		{
		    rtn = 0 ;			/* is not the last one, then */
		    break ;
		}

		rtn = 1 ;			/* assume no other msg marked */
		for (mp = mp->b_next; mp != NULL; mp = mp->b_next)
		{
		    if (mp->b_flag & MSGMARK)	/* some other msg is marked */
		    {
			rtn = 0 ;		/* condition not met */
			break ;
		    }
		}

		break ;

	    default:
		rtn = -EINVAL;			/* invalid argument */
	    }

	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_ATMARK,%ld) \"%s\" >> return %d\n",
		       arg, hd->sd_name, rtn) ;
	    RTN(rtn) ;
	}
	break;
    case I_CKBAND:
	if ( LIS_DEBUG_IOCTL )
	    printk("lis_strioctl(...,I_CKBAND,%ld) \"%s\"\n",
		   arg, hd->sd_name) ;
	if ( arg > LIS_MAX_BAND )
	    RTN(-EINVAL);
	else {
	    long val = 0;
	    
	    err = lis_strqget(hd->sd_rq,QCOUNT,(char)arg, &val) ; 
	    if (err) 
		val = -err ;			/* failed */
	    else
		val = (val != 0) ;		/* Solaris semantics */

	    RTN(val);
	}
	break;
    case I_GETBAND:
	{
	    int res;
	    if ((err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,sizeof(int)))<0)
		RTN(err);
	    if (!hd->sd_rq->q_first)
		RTN(-ENODATA);
	    res=hd->sd_rq->q_first->b_band;
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_GETBAND,...) \"%s\" >> rslt=%d\n",
		       hd->sd_name, res) ;
	    err = lis_copyout(f,&res,(char*)arg,sizeof(int));
	    RTN(err);
	}
	break;
    case I_CANPUT:
        {
	    int	res ;

	    if ( arg > LIS_MAX_BAND )
		RTN(-EINVAL);
	    else 
	    {
		res = lis_bcanputnext(hd->sd_wq, (unsigned char) arg);
		if ( LIS_DEBUG_IOCTL )
		    printk("lis_strioctl(...,I_CANPUT,...)"
			   " \"%s\" >> rslt=%d\n", hd->sd_name, res) ;
		RTN(res) ;
	    }
        }
	break;
    case I_SETCLTIME:
	if (arg >LIS_MAX_CLTIME)
	    RTN(-EINVAL);
	hd->sd_closetime=arg;
	if ( LIS_DEBUG_IOCTL )
	    printk("lis_strioctl(...,I_SETCLTIME,%ld) \"%s\"\n",
		   arg, hd->sd_name) ;
	RTN(0);
	break;
    case I_GETCLTIME:
	{
	    int res;
	    if ((err=lis_check_umem(f,VERIFY_WRITE,(char*)arg,sizeof(int)))<0)
		RTN(err);
	    res=hd->sd_closetime;
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...,I_GETCLTIME,...) \"%s\" >> rslt=%d\n",
		       hd->sd_name, res) ;
	    err = lis_copyout(f,&res,(char*)arg,sizeof(int));
	    RTN(err);
	}
	break;
    case I_PLINK:		
	/*fall through...  */
    case I_LINK:		/* link arg(=fd) stream below this one */
				/* see magic garden 7.5.1              */
				/* also AT&T STREAMS Pgrmr's Guide Chapt 10 */
	if (F_ISSET(hd->sd_flag,STRDERR|STWRERR))
	    RTN(-EINVAL);
	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    RTN(-ENXIO);

	if (   hd->sd_strtab->st_muxrinit == NULL
	    || hd->sd_strtab->st_muxwinit == NULL
	   )					/* must have lower qinits */
	    RTN(-EINVAL);

	CLOCKADD() ;
	err = lis_i_link(i, f, hd, (int) arg, cmd, &creds) ;
	CLOCKON() ;
	RTN(err) ;
	
    case I_PUNLINK:
	/*fall through...  */
    case I_UNLINK:
	if (F_ISSET(hd->sd_flag,STRHUP|STRCLOSE))
	    RTN(-ENXIO);
	if ((long) arg < -1)		/* compare as signed quantity */
	    RTN(-EINVAL);

	CLOCKADD() ;
	if (cmd == I_PUNLINK && arg == MUXID_ALL)
	    err = lis_unlink_all(i, f, hd, &creds) ;
	else
	    err = lis_i_unlink(i, f, hd, (int)arg, cmd, &creds ) ;
	CLOCKON() ;

	RTN(err) ;

    /*
     *  the following three _ioc_ routines do their own user-to-
     *  kernel space stuff internally; we just need to get 'arg' to
     *  them in appropriate form.
     */
    case I_LIS_PIPE:
	if (LIS_DEBUG_IOCTL)
	    printk( "lis_strioctl(...,I_LIS_PIPE,...)\n" );
	RTN(lis_ioc_pipe( (unsigned int *) arg ));
    case I_LIS_FATTACH:
	if (LIS_DEBUG_IOCTL)
	    printk("lis_strioctl(...,I_LIS_FATTACH,\"%s\") << \"%s\"\n",
		   (char *)arg, hd->sd_name);
	RTN(lis_ioc_fattach( f, (char *) arg ));
    case I_LIS_FDETACH:
	if (LIS_DEBUG_IOCTL)
	    printk( "lis_strioctl(...,I_LIS_FDETACH,\"%s\")\n",
		    (char *)arg );
	RTN(lis_ioc_fdetach( (char *)arg ));
    /**/

    case I_LIS_QRUN_STATS:
      {
        lis_qrun_stats_t	*stats ;

	err = lis_check_umem(f,VERIFY_WRITE, (char*)arg, sizeof(*stats)) ; 
	if (err < 0)
	    RTN(err);

        stats = (lis_qrun_stats_t*)ALLOCF(sizeof(lis_qrun_stats_t),"stats ");
        if (stats == NULL)
	    RTN(-ENOMEM);

	stats->num_cpus = lis_num_cpus;
	stats->num_qrunners = K_ATOMIC_READ(&lis_runq_cnt) ;
	stats->queues_running = K_ATOMIC_READ(&lis_queues_running) ;
	stats->runq_req_cnt = K_ATOMIC_READ(&lis_runq_req_cnt) ;
	memcpy(stats->runq_cnts, (void *)lis_runq_cnts, sizeof(stats->runq_cnts));
	memcpy(stats->queuerun_cnts, (void *)lis_queuerun_cnts,
						sizeof(stats->queuerun_cnts)) ;
	memcpy(stats->active_flags, (void *)lis_runq_active_flags,
						sizeof(stats->active_flags)) ;
	memcpy(stats->runq_pids, (void *)lis_runq_pids, sizeof(stats->runq_pids));
	memcpy(stats->runq_wakeups, (void *)lis_runq_wakeups,
						sizeof(stats->runq_wakeups));
	memcpy(stats->setqsched_cnts, (void *)lis_setqsched_cnts,
					    sizeof(stats->setqsched_cnts));
	memcpy(stats->setqsched_isr_cnts, (void *)lis_setqsched_isr_cnts,
					sizeof(stats->setqsched_isr_cnts));
	err = lis_copyout(f,stats,(char*)arg,sizeof(*stats));
        FREE(stats);
	RTN(err);
      }

    case I_LIS_GETSTATS:		/* LiS only - return stats */
	err = lis_check_umem(f,VERIFY_WRITE,(char*)arg, sizeof(lis_strstats)) ; 
	if (err < 0)
	    RTN(err);

	lis_strstats[LOCKCNTS][TOTAL] = lis_spin_lock_count ;
	lis_strstats[LOCKCNTS][FAILURES] = lis_spin_lock_contention_count ;

	err = lis_copyout(f,lis_strstats,(char*)arg,sizeof(lis_strstats));
	RTN(err);

    case I_LIS_LOCKS:
#if defined(LINUX)
	{
	    mblk_t	*mp = lis_lock_contention_msg() ;
	    int		 cnt1 ;
	    int		 cnt2 = 0 ;

	    if (mp == NULL)
		RTN(-ENOMEM) ;

	    mp->b_cont = lis_queue_contention_msg() ;
	    if (mp->b_cont)
		mp->b_cont->b_cont = lis_sem_contention_msg() ;
	    else
		mp->b_cont = lis_sem_contention_msg() ;

	    if (   (err = lis_check_umem(f,VERIFY_WRITE,
				         (char*)arg, msgdsize(mp))) < 0
		|| (err = lis_copyout(f,mp->b_rptr,
				      (char*)arg,
				      (cnt1 = mp->b_wptr - mp->b_rptr))) < 0
		|| (   mp->b_cont
		    && (err = lis_copyout(f,mp->b_cont->b_rptr,
			     (char*)arg + cnt1,
			 (cnt2 = mp->b_cont->b_wptr - mp->b_cont->b_rptr))) < 0
		   )
		|| (   mp->b_cont->b_cont
		    && (err = lis_copyout(f,mp->b_cont->b_cont->b_rptr,
			     (char*)arg + cnt1 + cnt2,
			     mp->b_cont->b_cont->b_wptr -
			     mp->b_cont->b_cont->b_rptr)) < 0
		   )

	       )
		RTN(err) ;

	    freemsg(mp) ;
	    break ;
	}
#else
	RTN(-EINVAL) ;
#endif

    case I_LIS_SEMTIME:			/* semaphore time histogram */
#if defined(LINUX)
	{
	    extern mblk_t *lis_get_sem_hist_msg(void) ;
	    mblk_t	*mp = lis_get_sem_hist_msg() ;
	    int		 cnt1 ;

	    if (mp == NULL)
		RTN(-ENOMEM) ;

	    if (   (err = lis_check_umem(f,VERIFY_WRITE,
				         (char*)arg, msgdsize(mp))) < 0
		|| (err = lis_copyout(f,mp->b_rptr,
				      (char*)arg,
				      (cnt1 = mp->b_wptr - mp->b_rptr))) < 0
	       )
		RTN(err) ;

	    freemsg(mp) ;
	    break ;
	}
#else
	RTN(-EINVAL) ;
#endif

    case I_LIS_PRNTSTRM:		/* LiS only - print stream */
	lis_print_stream(hd) ;
	break;

    case I_LIS_PRNTQUEUES:		/* LiS only - print queues */
	lis_print_queues() ;
	break;

    case I_LIS_PRNTMEM:			/* LiS only - print memory */
	lis_print_mem() ;		/* maybe an suser() check */
	break;

    case I_LIS_PRNTSPL:
	lis_print_spl_track() ;
	break ;

    case I_LIS_SDBGMSK:			/* LiS only - set debug mask */
        CLOCKOFF(IOCTLTIME) ;		/* may reset timing debug bit */
	lis_debug_mask = arg ;		/* maybe an suser() check */
        CLOCKON() ;
	break;
    case I_LIS_SDBGMSK2:		/* LiS only - set 2nd debug mask */
	lis_debug_mask2 = arg ;		/* maybe an suser() check */
	break;

    case I_LIS_GET_MAXMSGMEM:		/* LiS only - get max msg mem */
	err = lis_check_umem(f,VERIFY_WRITE,(char*)arg, sizeof(long)) ; 
	if (err < 0)
	    RTN(err);

	err=lis_copyout(f,&lis_max_msg_mem,(char*)arg,sizeof(lis_max_msg_mem));
	break;

    case I_LIS_SET_MAXMSGMEM:		/* LiS only - set max msg mem */
	if (!lis_suser(f))
	    RTN(-EINVAL) ;

	lis_max_msg_mem = arg ;
	K_ATOMIC_SET(&lis_strstats[MSGMEMLIM][MAXIMUM], lis_max_msg_mem) ;
	break;

    case I_LIS_GET_MAXMEM:		/* LiS only - get max mem */
	err = lis_check_umem(f,VERIFY_WRITE,(char*)arg, sizeof(long)) ; 
	if (err < 0)
	    RTN(err);

	err = lis_copyout(f,&lis_max_mem,(char*)arg,sizeof(lis_max_mem));
	break;

    case I_LIS_SET_MAXMEM:		/* LiS only - set max mem */
	if (!lis_suser(f))
	    RTN(-EINVAL) ;

	lis_max_mem = arg ;
	K_ATOMIC_SET(&lis_strstats[MEMLIM][MAXIMUM], lis_max_mem) ;
	break;


    default:
	/* encapsulate ioctl(fd,cmd,arg)as if the user specified 
	 * instead     ioctl(fd,I_STR,{cmd,arg})
	 * and pass down to strdoioctl()
	 */
	{
	    strioctl_t ioc;
	    int res;
	    ioc.ic_cmd=cmd;
	    ioc.ic_timout=LIS_LNTIME;
	    ioc.ic_len=TRANSPARENT;
	    ioc.ic_dp=(char*)arg;
	    CLOCKADD() ;
	    res=lis_strdoioctl(f,hd,&ioc,&creds,0);	/* no copyin */
	    CLOCKON() ;
	    if ( LIS_DEBUG_IOCTL )
		printk("lis_strioctl(...) \"%s\""
		       "<< ic_cmd=0x%x ic_timout=%d ic_len=0 >> rslt=%d\n",
		       hd->sd_name,
		       ioc.ic_cmd, ioc.ic_timout, res) ;
	    RTN(res);
	}
    }/*sw cmd*/

    /*
     * Fall into return point and return the value of "err".
     */
return_point:
    CP(hd,err) ;
    lis_unlock_wioc(hd) ;
return_no_unlock:
    lis_head_put(hd) ;
    CLOCKOFF(IOCTLTIME) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();
    CP(hd,err) ;
    return(err) ;

#undef RTN

}/*lis_strioctl*/

/*  -------------------------------------------------------------------  */
/* lis_poll_bits - evaluate poll bits for a stream			 */
/*
 * Used by lis_strpoll and by lis_poll_2_1 to evaluate a stream and
 * return poll events appropriate to the stream.
 */
unsigned lis_poll_bits(stdata_t *hd)
{
    int			 revents   = 0 ;
    int			 mtype ;
    lis_flags_t		 psw ;
    mblk_t		*mp ;

#define	RTN(msk)	do { revents = msk; goto rtn_point; } while (0)

    if ( F_ISSET(hd->sd_flag,(STRDERR|STWRERR)) )
	RTN(POLLERR) ;

    if (F_ISSET(hd->sd_flag,(STRHUP|STRCLOSE)))
	RTN(POLLHUP) ;

    if (F_ISSET(hd->sd_flag,STPLEX))
    {
	if (LIS_DEBUG_POLL)
	  printk("strpoll: stream %s: stream has been I_LINKed\n", hd->sd_name);

	RTN(POLLNVAL) ;
    }

    LIS_RDQISRLOCK(hd->sd_rq, &psw) ;
    if ((mp = hd->sd_rq->q_first) != NULL)	/* mp is 1st msg */
    {						/* a msg can be read */
	mtype = lis_btype(mp) ;
	LIS_RDQISRUNLOCK(hd->sd_rq, &psw) ;
	if (lis_hipri(mtype))
		revents |= POLLPRI ;
	else
	{
	    revents |= POLLIN ;			/* other than hi pri msg */
	    switch (mtype)
	    {
	    case M_DATA:
	    case M_PROTO:
		if (mp->b_band > 0)
		    revents |= POLLRDBAND ;
		else
		    revents |= POLLRDNORM ;
		break ;

	    case M_SIG:			/* I don't think this can happen */
		if (*mp->b_rptr == SIGPOLL)
		    revents |= POLLMSG ;
		break ;
	    }
	}
    }
    else
	LIS_RDQISRUNLOCK(hd->sd_rq, &psw) ;

    if (lis_bcanputnext(hd->sd_wq, 0))
	revents |= (POLLWRNORM | POLLOUT) ;	/* can write band 0 data */

    if (lis_bcanputnext_anyband(hd->sd_wq->q_next))
	revents |= POLLWRBAND ;			/* can write band > 0 */

rtn_point:
    if ( LIS_DEBUG_POLL )
	printk("lis_poll_bits: stream %s: revents:%s\n",
		hd->sd_name,
	        lis_poll_events((short)revents)) ;

    return(revents) ;				/* return events */

#undef RTN

} /* lis_poll_bits */

/*  -------------------------------------------------------------------  */
/* strpoll - called from lis_syspoll() poll() syscall handler
 *
 * The STREAMS poll function is called with the file info and
 * a pointer to a polldat structure.  The STREAMS routine
 * interrogates the file for the requested conditions according
 * to the 'pd_events' field in the polldat structure.  It returns
 * a bit-mask of the events that satisfy the conditions, if any.
 * The STREAMS routine also sets the pd_headp pointer to point to
 * its list head in its stream structure for the polldat list.
 * The caller takes care of linking the structure into the list.
 */
#ifdef PORTABLE_POLL
int
lis_strpoll(struct inode *i, struct file *f, void *ptr)
{
    polldat_t		*pdat_ptr  = (polldat_t *) ptr ;
    int			 revents   = 0 ;
    int			 events    = pdat_ptr->pd_events | POLLERR | POLLHUP ;
    stdata_t		*hd;
    unsigned long  	 time_cell = 0 ;

    (void) f ;				/* reference 'f' for compiler */

    if (   !(hd = FILE_STR(f))
	|| hd->magic != STDATA_MAGIC
       )
	return(POLLNVAL);

    CLOCKON() ;

    revents = lis_poll_bits(hd) & events ;	/* evaluate the stream */

    pdat_ptr->pd_headp = &hd->sd_polllist ;	/* ptr to list hd struct */
						/* allows insertion */
    if ( LIS_DEBUG_POLL )
	printk("strpoll: stream %s: events:%s revents:%s\n",
		hd->sd_name,
	      lis_poll_events((short)events), lis_poll_events((short)revents)) ;

    CLOCKOFF(POLLTIME) ;

    return(revents) ;				/* return events */

#undef RTN

} /* lis_strpoll */
#endif

/*  -------------------------------------------------------------------  */
/*  lis_wakeup_close
 *
 * This routine is used as a timout routine and as a utility function.
 * The argument is really a pointer to a streamhead structure.
 */
void	_RP lis_wakeup_close(caddr_t arg)
{
    stdata_t	*hd = (stdata_t *) arg ;

    if (hd->magic == STDATA_MAGIC)
    {
	CP(hd,hd->sd_flag) ;
	untimeout(hd->sd_close_timer) ;		/* cancel timer, just in case */
	hd->sd_close_timer = 0 ;
	lis_up(&hd->sd_closing) ;			/* semaphore wakeup */
    }
    else
	printk("lis_wakeup_close: hd=%lx bad magic: %lx\n",
		(long)hd, hd->magic);

} /* lis_wakeup_close */

/*  -------------------------------------------------------------------  */
/* lis_wakeup_flush
 *
 * Called if the M_FLUSH sent downstream at close time does not come
 * back to the stream head in a timely manner.
 */
void	_RP lis_wakeup_flush(caddr_t arg)
{
    stdata_t	*hd = (stdata_t *) arg ;

    if (hd->magic == STDATA_MAGIC)
    {
	CP(hd,hd->sd_flag) ;
	untimeout(hd->sd_close_timer) ;		/* cancel timer, just in case */
	hd->sd_close_timer = 0 ;
	CLR_SD_FLAG(hd, STRFLUSHWT) ;
	lis_wakeup_close_wt(hd) ;
    }
    else
	printk("lis_wakeup_flush: hd=%lx bad magic: %lx\n",
		(long)hd, hd->magic);

} /* lis_wakeup_flush */

/*  -------------------------------------------------------------------  */
/*  -------------------------------------------------------------------  */
/* deschedule - remove the queue from the scan list and q-run list
 *
 * The queue is unlocked when this routine is called.
 */
static void deschedule(queue_t *q)
{
    lis_flags_t	 psw;
    lis_flags_t	 psw1, psw2 ;
    queue_t	*rq, *wq ;
    queue_t	*p_scan ;
    queue_t	*p_prev ;
    stdata_t	*hd ;

    rq = RD(q) ;
    wq = WR(q) ;
    CP(rq,0) ;
    CP(wq,0) ;
    lis_spin_lock_irqsave(&lis_qhead_lock, &psw) ;
    LIS_QISRLOCK(wq, &psw1) ;
    if (F_ISSET(wq->q_flag,QSCAN))
    {					/* remove from the scan list */
	LIS_QISRUNLOCK(wq, &psw1) ;
	CP(q,0) ;
	p_prev = NULL;
	p_scan = (queue_t *) lis_scanqhead;
	while (p_scan != NULL)
	{					/* only wr q in scan list */
	    if (!LIS_CHECK_Q_MAGIC(p_scan)) break ;

	    if (p_scan == wq)
	    {					/* p_scan points to our queue */
	      K_ATOMIC_DEC(&lis_runq_req_cnt) ; /* one less thing to do */
	      LIS_QISRLOCK(p_scan, &psw1) ;	/* prevent ISR operations */
	      CP(p_scan,0) ;
	      if (p_prev != NULL)		/* trailing ptr valid?  */
		  p_prev->q_scnxt = p_scan->q_scnxt ;	/* link around q */
	      else				/* prev still is q_head */
		  lis_scanqhead = p_scan->q_scnxt ;	/* link hd around q */
						/* NULL if last elt in list */
	      if (lis_scanqtail == p_scan)	/* last elt in list? */
		  lis_scanqtail = p_prev ;	/* back up tail ptr */
						/* NULL if only elt in list */
	      /* do not move prev ptr */
	      LIS_QISRUNLOCK(p_scan, &psw1) ;
	      lis_head_put((stdata_t *)p_scan->q_str) ; /* balance ins scanl */
	    }
	    else				/* not our element */
		p_prev = p_scan;		/* keep trailing ptr */

	    p_scan = p_scan->q_scnxt ;		/* go to next elt of list */
	}

	LIS_QISRLOCK(wq, &psw1) ;		/* prevent ISR operations */
	wq->q_scnxt = NULL ;
	wq->q_flag &= ~QSCAN ;
	LIS_QISRUNLOCK(wq, &psw1) ;
    }
    else
	LIS_QISRUNLOCK(wq, &psw1) ;

    LIS_QISRLOCK(wq, &psw1) ;
    LIS_QISRLOCK(rq, &psw2) ;
    if ( (wq->q_flag | rq->q_flag) & QENAB )
    {
	p_prev = NULL;
	p_scan = (queue_t *) lis_qhead;

	/* Unschedule the service procedures.
	 */
	CP(q,0) ;

	LIS_QISRUNLOCK(rq, &psw2) ;
	LIS_QISRUNLOCK(wq, &psw1) ;
	if (   (lis_qhead != NULL && lis_qtail == NULL)
	    || (lis_qhead == NULL && lis_qtail != NULL)
	   )
	    printk("LiS: deschedule before: Qhead error: "
		   "lis_qhead=0x%p lis_qtail=0x%p\n",
		   lis_qhead, lis_qtail) ;

	while (p_scan != NULL)
	{
	    if (!LIS_CHECK_Q_MAGIC(p_scan)) break ;

	    if (p_scan == rq || p_scan == wq)
	    {					/* p_scan points to our queue */
	      K_ATOMIC_DEC(&lis_runq_req_cnt) ; /* one less thing to do */
	      CP(p_scan,lis_qhead) ;
	      CP(p_prev,lis_qtail) ;
	      if (p_prev != NULL)		/* trailing ptr valid?  */
		  p_prev->q_link = p_scan->q_link ;	/* link around q */
	      else				/* prev still is q_head */
		  lis_qhead = p_scan->q_link ;	/* link hd around q */
						/* NULL if last elt in list */
	      if (lis_qtail == p_scan)		/* last elt in list? */
		  lis_qtail = p_prev ;		/* back up tail ptr */
						/* NULL if only elt in list */
	      /* do not move prev ptr */
	      LisDownCount(QSCHEDS) ;
	      hd = (stdata_t *) p_scan->q_str ;
	      if (   hd->magic == STDATA_MAGIC
		  && (hd->sd_wq == p_scan || hd->sd_rq == p_scan)
	         )
		  lis_head_put(hd) ;		/* balance qenable */
	    }
	    else				/* not our element */
		p_prev = p_scan;		/* keep trailing ptr */

	    p_scan = p_scan->q_link ;		/* go to next elt of list */
	}

	if (   (lis_qhead != NULL && lis_qtail == NULL)
	    || (lis_qhead == NULL && lis_qtail != NULL)
	   )
	    printk("LiS: deschedule after: Qhead error: "
		   "lis_qhead=0x%p lis_qtail=0x%p\n",
		   lis_qhead, lis_qtail) ;

	LIS_QISRLOCK(wq, &psw1) ;		/* prevent ISR operations */
	wq->q_link = NULL ;
	wq->q_flag &= ~QENAB ;
	LIS_QISRUNLOCK(wq, &psw1) ;
	LIS_QISRLOCK(rq, &psw2) ;
	rq->q_link = NULL ;
	rq->q_flag &= ~QENAB ;
	LIS_QISRUNLOCK(rq, &psw2) ;
    }
    else
    {
	LIS_QISRUNLOCK(rq, &psw2) ;
	LIS_QISRUNLOCK(wq, &psw1) ;
    }

    lis_spin_unlock_irqrestore(&lis_qhead_lock, &psw) ;
}

/* mark_closing - mark both read and write queues as closing, all
 * the way down to the driver's queues.
 */
static void mark_closing(queue_t *sd_wq)
{
    queue_t	*sd_rq ;
    queue_t	*q ;
    lis_flags_t	 psw_wq, psw_rq ;

    CP(sd_wq,0) ;
    do
    {
	sd_rq = RD(sd_wq) ;

	CP(sd_wq,sd_rq) ;
						/* prevent qenable */
	LIS_QISRLOCK(sd_wq, &psw_wq) ;		/* prevent ISR operations */
	LIS_QISRLOCK(sd_rq, &psw_rq) ;
	CP(sd_wq,0) ;
	sd_wq->q_flag |= (QNOENB | QCLOSING);	/* prevent qenables */
	sd_rq->q_flag |= (QNOENB | QCLOSING);	/* prevent qenables */
	LIS_QISRUNLOCK(sd_rq, &psw_rq) ;
	LIS_QISRUNLOCK(sd_wq, &psw_wq) ;

	deschedule(sd_wq) ;			/* remove from sched lists */

	q = sd_wq ;				/* save for loop iteration */
	sd_wq = sd_wq->q_next ;
    }
    while (sd_wq != NULL && SAMESTR(q)) ;
}

/*
 * close_action
 *
 * A helper routine to perform close activities on a stream whose
 * open count just went to zero.
 *
 * It is possible that the close mechanism could be implemented more
 * elegently by performing these functions from lis_head_put.  The
 * technique would be to keep the stream intact, but disconnected
 * from file I/O, until all the flushing traffic settles down and
 * then perform these functions when the refcnt finally goes to
 * zero.  But I am going to save that restructuring for another time.
 */
static void close_action(stdata_t *head)
{
    int		rslt ;
    stdata_t   *hd_peer ;
    lis_flags_t	psw ;

    CP(head,0) ;
    SET_SD_FLAG(head, STRCLOSE) ;

    /*
     *  if we're a pipe, set the peer's STRHUP flag (but don't hang up
     *  the peer - it may still have to receive passed FDs, etc...)
     */
    if ((hd_peer = head->sd_peer) &&
	hd_peer != head &&
	!F_ISSET(hd_peer->sd_flag, STRHUP|STRCLOSE))
    {
	int	flag ;

	if ( LIS_DEBUG_CLOSE )
	    printk("close_action: closing pipe %s <===> %s\n",
		    head->sd_name, hd_peer->sd_name) ;
	CP(head,0) ;
	lis_spin_lock_irqsave(&hd_peer->sd_lock, &psw) ;
	hd_peer->sd_flag |= STRHUP ;
	hd_peer->sd_peer = hd_peer;
	flag = hd_peer->sd_flag ;
	if (flag & STRFROZEN)
	    hd_peer->sd_flag &= ~STRFROZEN ;
	lis_spin_unlock_irqrestore(&hd_peer->sd_lock, &psw) ;
	lis_stream_error(hd_peer, 0, 0) ;
	if (flag & STRFROZEN)
	    lis_head_put(hd_peer) ;		/* balance freezestr */
    }

    if (   head->sd_closetime != 0		/* have close time set */
	&& lis_qcountstrm(head->sd_wq) != 0	/* messages queued */
       )
    {						/* set a timer */
	CP(head,0) ;
	if ( LIS_DEBUG_CLOSE )
	  printk("lis_tear_down_stream: "
		 "stream %s waiting %dms for queues to drain\n",
		    head->sd_name, head->sd_closetime);

	if (SAMESTR(head->sd_wq))
	{
	    LIS_QISRLOCK(head->sd_wq->q_next, &psw) ;
	    head->sd_wq->q_next->q_flag |= QCLOSEWT;
	    LIS_QISRUNLOCK(head->sd_wq->q_next, &psw) ;
	}

	head->sd_close_timer =
	    lis_timeout_fcn(lis_wakeup_close,
			    (caddr_t) head,
			    lis_milli_to_ticks(head->sd_closetime),
			   "stream-close", MEM_TIMER) ;
	K_ATOMIC_DEC(&lis_in_syscall) ;	/* "done" with a system call */
	lis_runqueues() ;
	lis_down(&head->sd_closing) ;		/* sleep for awhile */
	K_ATOMIC_INC(&lis_in_syscall) ;	/* processing a system call */
	if (head->sd_close_timer)
	{
	    untimeout(head->sd_close_timer) ;
	    head->sd_close_timer = 0 ;
	}
	CP(head,0) ;
    }

    lis_stream_error(head, EIO, EIO) ;		/* wake up syscalls */
    lis_free_elist(&head->sd_siglist);		/* do in signal masks */
    if (head->sd_mux.mx_hd != NULL)		/* is a ctl stream */
    {
	CP(head,0) ;
						/* unlink all lowers */
	lis_i_unlink(NULL, NULL, head, -1, I_PUNLINK, &head->sd_creds) ;
	CP(head,0) ;
    }

    /*
     * The possible I_UNLINK above is that last legitimate message that can
     * be sent on the stream.  From now on message traffic is suppressed.
     *
     * By locking the queue we assure ourselves that there are no put
     * procedures running on other CPUs.  Then we prevent further qenables
     * on the stream head queue pair.  This keeps the service procedures
     * from running.  Since we haven't called the driver's close routine
     * yet, there could still be some interrupt level activity on the q.
     *
     * lis_qdetach will sync up with any residual service procedures
     * running on other CPUs.
     */
    CP(head,head->sd_wq) ;
    mark_closing(head->sd_wq) ;

    /*
     * Flush the stream and wait for the M_FLUSH to come back to
     * the stream head.  Then close all the queues.  We run a short
     * timer in case the M_FLUSH gets lost downstream somewhere.
     *
     * In the case of a STREAMS pipe, SAMESTR will return 0 and we
     * will omit the flushing operation.  When the other end of the
     * pipe closes it will flush its own messages.
     */
    if (SAMESTR(head->sd_wq))			/* is there a "downstream" */
    {
	SET_SD_FLAG(head,STRCLOSEWT|STRFLUSHWT); /* allows wakeups */
	head->sd_close_timer = lis_timeout_fcn(lis_wakeup_flush,
					   (caddr_t) head,
					   lis_milli_to_ticks(50),
					   "stream-flush", MEM_TIMER) ;
	lis_snd_mflush(head->sd_wq, FLUSHRW, 0);
	CP(head,head->sd_flag) ;
	rslt = 0 ;
	while (   F_ISSET(head->sd_flag,STRFLUSHWT)
	       && (rslt = lis_sleep_on_close_wt(head)) == 0
	      )
	    CP(head,head->sd_close_timer) ;

	if (head->sd_close_timer)
	{
	    untimeout(head->sd_close_timer) ;
	    head->sd_close_timer = 0 ;
	}

	if (rslt < 0 && rslt != -EINTR)
	    printk("lis_tear_down_stream: "
		   "sleep_on_close_wt returned %d\n", rslt) ;
    }

    CP(head,head->sd_flag) ;
    while (!pop_mod( head, head->sd_open_flags, &head->sd_creds )) {};

    CP(head,head->sd_wq) ;
    if (SAMESTR(head->sd_wq))
	lis_qdetach( LIS_RD(head->sd_wq->q_next), 1, 
		     head->sd_open_flags, &head->sd_creds );

    CP(head,0) ;
    lis_dismantle(head, &head->sd_creds);		/* deallocate queues */
}

/*  -------------------------------------------------------------------  */
/* doclose - dec refcnt & dismantle the stream when sd_opencnt gets to 0.
 *
 * This routine is called from several different contexts.  One is from
 * lis_strclose which is on the straightforward close path in closing
 * a file.  Another is from the unlink code when a stream is unlinked
 * from below a multiplexor and this is the last use of the stream.  In the
 * latter case, both file and inode pointers will be NULL since the file
 * and inode are both long gone.  It is also called from fdetach-related
 * routines.
 */
int
lis_doclose( struct inode *i, struct file *f, stdata_t *head, cred_t *creds )
{
    int		   opencnt ;
    unsigned long  time_cell = 0 ;
    long           this_doclose ;
    int		   sem2_err = 0 ;

    CLOCKON() ;

    /*
     * For information about race conditions see "Analysis of
     * open/close locking" near the top of this file.
     *
     */
    if (head == NULL || head->magic != STDATA_MAGIC)
    {
	printk("lis_doclose(...,h@0x%p,...)"
	       " << MAGIC 0x%lx!=0x%lx <EINVAL>\n",
	       head, (head?head->magic:0), STDATA_MAGIC) ;
	return(-EINVAL) ;
    }

    CP(head,0) ;
    K_ATOMIC_INC(&lis_doclose_cnt);
    this_doclose = K_ATOMIC_READ(&lis_doclose_cnt);

    if ((LIS_DEBUG_CLOSE || LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
    {
	printk("lis_doclose(i@0x%p/%d,f@0x%p/%d,h@0x%p/%d/%d.%d,...)#%ld/%d\n"
	       "    << \"%s\" "
#if defined(LINUX) && defined(KERNEL_2_1)
	       "d@0x%p/%d "
#endif
	       "<[%d] %d LiS inode(s), %d open stream(s)>\n",
	       i, (i?I_COUNT(i):0),
	       f, (f?F_COUNT(f):0),
	       head,
	       LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head), head->sd_linkcnt,
	       this_doclose, K_ATOMIC_READ(&lis_close_cnt),
	       head->sd_name,
#if defined(LINUX) && defined(KERNEL_2_1)
	       (f?f->f_dentry:NULL),
	       (f&&f->f_dentry?D_COUNT(f->f_dentry):0),
#endif
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       K_ATOMIC_READ(&lis_stdata_cnt)) ;
	lis_print_stream(head) ;
    }

#if defined(LINUX)
    /*
     *  FIFOs/pipes: synchronize at close time before anything else
     */
    if (i && f && F_ISSET(head->sd_flag,STFIFO))
	lis_fifo_close_sync( i, f );
#endif

    /*
     *  if we're hungup and fattached but no M_PASSFPs are queued,
     *  fdetach now - BEFORE we acquire any locks, because fdetach
     *  will itself call here once for each active fattach
     */
    if ((head->sd_flag & (STRHUP|STRATTACH)) == (STRHUP|STRATTACH) &&
	head->sd_rfdcnt == 0)			/* no passfps queued */
    {
	CP(head,0) ;
	lis_fdetach_stream(head);
    }

    /*
     * We get the stdata semaphore and hold it until we decide
     * whether or not this is the last close.  This prevents
     * any other closes on this same stream, or any open to
     * this stream from running until we decide whether or not
     * to set the STRCLOSE bit in the head flags.  Setting this
     * bit will prevent this stream from being found in a
     * search of the stdata structures.  Once that bit is set
     * an open attempt on the same {maj,min} will allocate a
     * new stream head and not use the one that we are closing.
     *
     * Getting the sd_opening semaphore serializes a simultaneous
     * open and close on the same stream -- possible with directed
     * opens to the same {maj,min}.
     *
     * What this all means is that once that bit is set we can
     * go about closing the stream without being bothered from
     * above.  There are still driver races to be considered.
     */
    lis_down_nosig(&head->sd_opening) ;
    SET_SD_FLAG(head, STROSEM_HELD) ;

    lis_del_from_elist(&head->sd_siglist,PID(f),S_ALL);

    /*
     * opencnt might be zero if we are called from stropen to close
     * the clone driver part of a clone open.  The open count for the
     * stream head that was used for the clone driver still has its
     * open count set to zero.
     */
    if ((opencnt = LIS_SD_OPENCNT(head)) > 0)
    {
	K_ATOMIC_DEC(&head->sd_opencnt) ;		 /* decr open count */
	opencnt-- ;
    }

    /*
     * If the open count is now down to the plink count
     * then all that is left are the plinks.  This means that
     * this is really the last close of the stream and the inode
     * is about to disappear.  We clear the inode ptr in the
     * stdata structure.  There will be no more accesses of
     * this stream from above via the file system.
     *
     * The stream remains allocated but cannot be reached via an
     * open.  Open will allocate a new stdata structure upon a
     * re-open.
     */
    CP(head,opencnt) ;
    if (head->sd_linkcnt != 0 && opencnt <= head->sd_linkcnt)
    {
	lis_free_elist(&head->sd_siglist);	/* do in signal masks */
    }

    /*
     * If f and i are non-NULL then we were called from the system
     * close routine.  Otherwise it is an embedded call from something
     * such as i_unlink.
     */
    if (f && (F_COUNT(f) <= 0))			/* detach from file */
    {
	SET_FILE_STR(f, NULL);
	/* f->f_op = NULL; */
 	lis_cleanup_file_closing(f, head) ;
    }

    if (i && (I_COUNT(i) <= 1))			/* detach from inode */
    {
	SET_INODE_STR(i, NULL);			/* detach from inode */
						/* caller will "iput" */
    }

    if (opencnt == 0)				/* last open */
    {
	/*
	 * Since the STRCLOSE flag is important to stream lookups, acquire
	 * the stdata semaphore before changing this flag.
	 */
	lis_down_nosig(&lis_stdata_sem);
	head->sd_creds = *creds ;		/* closer's credentials */
	SET_SD_FLAG(head, STRCLOSE) ;		/* mark as closing */
	lis_up(&lis_stdata_sem);		/* allow opens again */

	/*
	 * Release the semaphore before calling close_action.  i_unlink
	 * will make a nested call into us.
	 */
	if (sem2_err == 0)			/* if <0 we're really busted */
	    release_sd_opening_sem(head) ;

	close_action(head) ;			/* perform close functions */
    }
    else					/* not the last close */
    {
	if (sem2_err == 0)			/* if <0 we're really busted */
	    release_sd_opening_sem(head) ;
    }

    CP(head,0) ;

    lis_head_put(head) ;			/* done w/structure */

    if (LIS_DEBUG_CLOSE || LIS_DEBUG_REFCNTS)
	printk("lis_doclose(...)#%ld/%d"
	       " >> <[%d] %d LiS inode(s), %d open stream(s)>\n",
	       this_doclose, K_ATOMIC_READ(&lis_close_cnt),
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       K_ATOMIC_READ(&lis_stdata_cnt)) ;

    CLOCKOFF(CLOSETIME) ;

    return(opencnt);
								     
} /*lis_doclose*/

int
lis_strclose(struct inode *i, struct file *f)
{
    struct stdata *head;
    cred_t	   creds;
    unsigned long  time_cell = 0 ;
    unsigned long  this_close ;

#define RTNX	return(0)
    
    CLOCKON() ;

    K_ATOMIC_INC(&lis_close_cnt);
    this_close = K_ATOMIC_READ(&lis_close_cnt);

    if (!i)
    {
	printk("lis_strclose: called w/ null inode");
	CLOCKOFF(CLOSETIME) ;
	RTNX;
    }

    if ((LIS_DEBUG_CLOSE || LIS_DEBUG_ADDRS) || LIS_DEBUG_REFCNTS)
	printk("lis_strclose(i@0x%p/%d,f@0x%p/%d)#%ld i_rdev=(%d,%d)\n"
	       "    << "
#if defined(LINUX) && defined(KERNEL_2_1)
	       "d@0x%p/%d "
#endif
	       "<[%d] %d LiS inode(s), %d open stream(s)>\n",
	       i, I_COUNT(i), f, F_COUNT(f), this_close,
	       getmajor(GET_I_RDEV(i)), getminor(GET_I_RDEV(i)),
#if defined(LINUX) && defined(KERNEL_2_1)
	       f->f_dentry, D_COUNT(f->f_dentry),
#endif
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       K_ATOMIC_READ(&lis_stdata_cnt)) ;

    if (!I_COUNT(i))
    {
	printk("lis_strclose: called with unused inode (inode 0x%p file 0x%p)",
		  i, f);
	CLOCKOFF(CLOSETIME) ;
	RTNX;
    }

    if (   !S_ISCHR(i->i_mode)
	|| !LIS_DEVOK(getmajor(GET_I_RDEV(i)))
       )
    {
	CLOCKOFF(CLOSETIME) ;
	RTNX;
    }

    if (!(head = FILE_STR(f)))
    {
	printk("lis_strclose: called with null stream (inode 0x%p file 0x%p)",
		  i, f);
	CLOCKOFF(CLOSETIME) ;
	RTNX;
    }    

    creds.cr_uid  = (uid_t) EUID(f);
    creds.cr_gid  = (gid_t) EGID(f);
    creds.cr_ruid = (uid_t) UID(f);
    creds.cr_rgid = (gid_t) GID(f);

    CP(head,0) ;
    K_ATOMIC_INC(&lis_in_syscall) ;		/* processing a system call */
    CLOCKADD() ;
    lis_doclose(i, f, head, &creds) ;		/* close processing */

    CP(head,0) ;
    K_ATOMIC_DEC(&lis_in_syscall) ;	/* done processing a system call */
    lis_runqueues();
    RTNX ;

#undef RTNX

} /* lis_strclose */

/*  -------------------------------------------------------------------  */
/*                             lis_tear_down_stream			 */
/*
 * Tear down the stream.  The close_action routine has done all the
 * close-time hard work.  This routine just frees the structure.
 *
 * Some later revision may defer lots of the close processing to here.
 * The technique would be to call the driver's close routine, flush the
 * stream and just let the message traffic settle down.  Then de-allocate.
 */
static void lis_tear_down_stream(stdata_t *head)
{

    lis_free_stdata(head);			/* free the stdata structure */
}

/*  -------------------------------------------------------------------  */

/* Initialize some glob vars...
 */
void lis_init_head( void )
{
#if (defined(LINUX) && defined(USE_KMEM_CACHE))
    lis_init_locks();
#endif
#if defined(USE_CODE_PATH)
    lis_spin_lock_init(&lis_code_path_lock, "LiS_code_path_lock") ;
#endif
    /* We'd better use defaults from strconfig instead of magic #s.
     */
    lis_qhead = lis_qtail =  lis_scanqhead= lis_scanqtail = NULL;
    K_ATOMIC_SET(&lis_queues_running, 0);
    K_ATOMIC_SET(&lis_runq_req_cnt, 0) ;
    lis_nstrpush = 0;
    lis_strhold = 0;
    lis_strthresh = 0;
    /*lis_iocseq=whichever get's in the air -- so we have some kind of random
     *number for this seq # <-should be fixed*/

    lis_sem_init( &lis_stdata_sem, 1 );
    lis_sem_init( &lis_close_sem, 1 );
    lis_sem_init( &lis_queue_sync.qs_sem, 1 );
    lis_spin_lock_init(&lis_stdata_lock, "LiS-Stdata") ;
    lis_spin_lock_init(&lis_qhead_lock, "Qhead-Lock") ;
    lis_spin_lock_init(&lis_incr_lock, "Incr-Lock") ;
    lis_spin_lock_init(&lis_msg_lock, "Msg-Lock") ;
    lis_spin_lock_init(&lis_bc_lock, "Bufcall-Lock") ;
    lis_spin_lock_init(&lis_mem_lock, "Mem-Lock") ;
    lis_spin_lock_init(&lis_tlist_lock, "Tlist-Lock") ;
    lis_spin_lock_init(&lis_queue_contention_lock, "Qcontend-Lock") ;
    lis_cmn_err_init() ;
    lis_initialize_dki();

    lis_init_bufcall() ;
#if (defined(LINUX) && defined(USE_KMEM_CACHE))
    lis_init_queues();
    lis_init_msg();
#endif
    lis_init_mod() ;
}/*lis_init_head*/

/*  -------------------------------------------------------------------  */
/*
 * Terminate code
 */
void lis_terminate_head(void)
{
    lis_terminate_mod() ;
    lis_terminate_msg() ;
#if (defined(LINUX) && defined(USE_KMEM_CACHE))
    lis_terminate_queues();
#endif
    lis_terminate_bufcall() ;
    lis_terminate_dki() ;
    SEM_DESTROY(&lis_stdata_sem);
    SEM_DESTROY(&lis_close_sem);
    SEM_DESTROY(&lis_queue_sync.qs_sem);

} /* lis_terminate_head */

/*
 * This must be the very last thing
 */
void lis_terminate_final(void)
{
    lis_terminate_mem() ;
#if !defined(USER)
    lis_free_all_pages() ;		/* from lis page allocator */
#endif
#if (defined(LINUX) && defined(USE_KMEM_CACHE))
    lis_terminate_locks();
#endif
}


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# c-basic-offset: 4 ***
# End: ***
  ----------------------------------------------------------------------*/
