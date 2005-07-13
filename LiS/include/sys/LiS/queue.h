/*****************************************************************************

 @(#) $Id: queue.h,v 1.1.1.5.4.2 2005/04/12 22:45:25 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/04/12 22:45:25 $ by $Author: brian $

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * queue.h --- streams queue handling
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: queue.h,v 1.1.1.5.4.2 2005/04/12 22:45:25 brian Exp $
 * Purpose         : here you have utilites to handle str queues.
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */


#ifndef _QUEUE_H
#define _QUEUE_H 1

#ident "@(#) $RCSfile: queue.h,v $ $Name:  $($Revision: 1.1.1.5.4.2 $) $Date: 2005/04/12 22:45:25 $"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* config definitions */
#endif
#ifndef _MSG_H
#include <sys/LiS/msg.h>	/* streams messages*/
#endif
#ifndef _STRPORT_H
#include <sys/strport.h>	/* machine-dependent porting constructs */
#endif
#ifndef SYS_LISLOCKS_H
#include <sys/lislocks.h>
#endif


/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/* Max # of bands -- hard limit since we use char to store band ids
 */
#define LIS_MAX_BAND	255

/* STREAMS queue flags (q_flag) mask values
 */
#define QENAB           0x000001 /* queue enabled */
#define QWANTR          0x000002 /* usr wants to read */
#define QWANTW          0x000004 /* usr wants to write */
#define QFULL           0x000008 /* queue is full */
#define QREADR          0x000010 /* it's the reader */
#define QUSE            0x000020 /* not a free queue */
#define QNOENB          0x000040 /* don't enable with putq() */
#define QBACK           0x000080 /* a back-enabled queue */
#define QRETRY		0x000100 /* retry timer's set */
#define QSCAN		0x000200 /* queue in the scan list */
#define	QCLOSING	0x000400 /* strm hd wants to close this queue */
#define QWASFULL	0x000800 /* QFULL was set, still above low water */
#define QCLOSEWT	0x001000 /* strm hd waiting for queue to drain */
#define	QPROCSOFF	0x002000 /* don't call Q put/svc routines */
#define QRUNNING	0x004000 /* svc proc is actually running */
#define	QWAITING	0x008000 /* qdetach is waiting for wakeup */
#define	QREADING	0x010000 /* read/getpmsg running on queue */
#define	QWANTENAB	0x020000 /* Q wants to be enabled */
#define	QOPENING	0x040000 /* open in process on strm hd above queue */
#define	QDEFERRING	0x080000 /* processing deferred messages */
#define	QFROZEN		0x100000 /* Q is in a frozen stream */

/*
 * When any of these bits are set a qenable will defer.
 *
 * putnext will defer if QPROCSOFF, QOPENING or QFROZEN is set but will proceed
 * in the presence of the rest.
 */
#define	Q_INH_ENABLE	(QENAB | QPROCSOFF | QCLOSING | QRUNNING | QOPENING | \
			 QFROZEN)
#define	Q_DEFER_ENABLE	(QENAB | QPROCSOFF |            QRUNNING | QOPENING | \
			 QFROZEN)

/*  -------------------------------------------------------------------  */
/* STREAMS queue's qband flags
 */
#define QB_FULL         0x000001 /* band is full */
#define QB_WANTW        0x000002 
#define QB_BACK         0x000004 
#define	QB_WASFULL	0x000008

/* flushq() flags
 */
#define FLUSHDATA 0		/* do not flush cntl msgs */
#define FLUSHALL  1		/* flush every msg */


/* what to qtrqget/set
 */
#define QHIWAT		0
#define QLOWAT		1
#define QMAXPSZ		2
#define QMINPSZ		3
#define QCOUNT		4
#define QFIRST		5
#define QLAST		6
#define QFLAG		7

/*  -------------------------------------------------------------------  */
/*				    Types                                */

typedef int qfields_t;

/*
 * This structure is used to synchronize access to a queue via the
 * service or put procedure.
 *
 * There are four options:
 *
 * No synchronization - drivers are reentrant or have their own locking.
 *
 * Sync on queue - read and write queue halves treated separately
 *
 * Sync on queue pair - read and write queue halves treated together
 *
 * Sync on global - use a global sync structure
 *
 * The sync structure has a semaphore for waiting and a place to save
 * the task ptr of the current "owner" of the queue.  The current owner
 * can access the queue in nested fashion, so a nest count is also kept.
 */
typedef struct lis_q_sync
{
    void		*qs_taskp ;	/* owner's task ptr */
    lis_atomic_t	 qs_nest ;	/* nest level of owner */
    lis_semaphore_t	 qs_sem ;	/* sem to wait on */

} lis_q_sync_t ;

/*
 *  The STREAMS queue structure. Each module has a queue upstream and
 * another one downstream.
 */

typedef
struct queue
{
  EXPORT
        struct  qinit   *q_qinfo;       /* procs and limits for queue [I]*/
        struct  msgb    *q_first;       /* first data block [Z]*/
        struct  msgb    *q_last;        /* last data block [Z]*/
        struct  queue   *q_next;        /* next Q of stream [Z]*/
        struct  queue   *q_link;        /* to next Q for the scan list [Z]*/
        void            *q_ptr;         /* module private data for free */
        ulong           q_count;        /* number of bytes on Q [Z]*/
        volatile ulong  q_flag;         /* queue state [Z]*/
  SHARE
        long            q_minpsz;       /* min packet size accepted [I]*/
        long            q_maxpsz;       /* max packet size accepted [I]*/
        ulong           q_hiwat;        /* queue high water mark [I]*/
        ulong           q_lowat;        /* queue low water mark [I]*/
  PRIVATE
        struct qband    *q_bandp;       /* separate flow information */
        struct queue    *q_other;       /* for RD()/WR()/OTHER() */
        void            *q_str;         /* pointer to stream's stdata */
        struct  queue   *q_scnxt;       /* next q in the scan list */
	ulong		 q_magic;	/* magic number */
	ulong	 	 q_contention_cnt ;
	lis_q_sync_t	*q_qsp ;	/* ptr to sync struct to use */
	int		 q_qlock_option;/* option elected */
	char		*q_owner_file;
	int		 q_owner_line;
	lis_rw_lock_t	 q_isr_lock;	/* for ISR protection */
	lis_semaphore_t *q_wakeup_sem ;	/* helps sync closes */
	struct msgb	*q_defer_head ;	/* to defer puts */
	struct msgb	*q_defer_tail ;
} queue_t;

#define	Q_MAGIC		((ulong) (0x93390000L | sizeof(queue_t)) )

#if __LIS_INTERNAL__
int     lis_check_q_magic(queue_t *q, char *filename, int linenr) ;
#define LIS_QMAGIC(q,f,l) \
	(((q) && ((q)->q_magic == Q_MAGIC)) || lis_check_q_magic((q),(f),(l)))
#define	LIS_CHECK_Q_MAGIC(q)	LIS_QMAGIC((q),__FILE__,__LINE__)
#endif

/*
 * Queue locking options
 *
 * Set at Configure time.  Applies to all queues.
 */
#define	LIS_QLOCK_NONE		0	/* no locking by LiS */
#define LIS_QLOCK_QUEUE		1	/* lock each queue separately */
#define LIS_QLOCK_QUEUE_PAIR	2	/* lock the pair */
#define	LIS_QLOCK_GLOBAL	3	/* use global lock */

/*
 * Queue locking functions.  These get the semaphore.
 */
#if __LIS_INTERNAL__
extern int  lis_lockq_fcn  (queue_t *q, char *file, int line);
extern void lis_unlockq_fcn(queue_t *q, char *file, int line);
extern int lis_set_q_sync(queue_t *q, int qlock_option);
#endif

/*
 * Routines to set and clear flags in a list of queues
 */
#if __LIS_INTERNAL__
extern void lis_set_q_flags(ulong flags, int both_qs, ...);
extern void lis_clr_q_flags(ulong flags, int both_qs, ...);
#endif

/*
 * Queue isr locking macro.  To be used ONLY internally by LiS.
 */
#define	LIS_QISRLOCK(qp,flgsp)			\
		    lis_rw_write_lock_irqsave(&(qp)->q_isr_lock,(flgsp))
#define	LIS_QISRUNLOCK(qp,flgsp)		\
		    lis_rw_write_unlock_irqrestore(&(qp)->q_isr_lock,(flgsp))
#define	LIS_RDQISRLOCK(qp,flgsp)			\
		    lis_rw_read_lock_irqsave(&(qp)->q_isr_lock,(flgsp))
#define	LIS_RDQISRUNLOCK(qp,flgsp)		\
		    lis_rw_read_unlock_irqrestore(&(qp)->q_isr_lock,(flgsp))

/*
 *  The qinit structure. It's used to hold the queue routines and info.
 *
 *  The SVR4 DKI contains contradictory information about the types
 *  of the put and srv procedures.  In the D2DK section, it says that
 *  that they are void type.  In the D4DK section on qinit, it says
 *  that they are int type.
 *
 *  After using the "void" form for some years, we are converting to the
 *  "int" form.  It seems that all the other Unix streams.h files use "int".
 *  Use "return(0)" in the functions.  The return value is ignored by LiS.
 */

typedef
struct  qinit {
#if defined(USE_VOID_PUT_PROC)
        void    _RP (*qi_putp)(queue_t*, mblk_t*);  /* put procedure */
        void    _RP (*qi_srvp)(queue_t*);           /* service procedure */
#else
        int     _RP (*qi_putp)(queue_t*, mblk_t*);  /* put procedure */
        int     _RP (*qi_srvp)(queue_t*);           /* service procedure */
#endif
        int     _RP (*qi_qopen)(queue_t *, dev_t *, int, int, cred_t *);
        int     _RP (*qi_qclose)(queue_t *, int,cred_t *); /* close procedure */
        int     _RP (*qi_qadmin)(void);         /* debugging */
        struct lis_module_info *qi_minfo;   /* module information structure */
        struct module_stat *qi_mstat;   /* module statistics structure */
} qinit_t;


/*
 * This structure hold a queue's priority band information.
 */

typedef
struct qband {
  SHARE
        struct qband    *qb_next;       /* next band's info */
        ulong           qb_count;       /* number of bytes in band */
        struct msgb     *qb_first;      /* beginning of band's data */
        struct msgb     *qb_last;       /* end of band's data */
        ulong           qb_hiwat;       /* high water mark for band */
        ulong           qb_lowat;       /* low water mark for band */
        ulong           qb_flag;        /* see above */
} qband_t;

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* Queues are allocated always in queue pairs, the pointer to the queue pair
 * may be a pointer to any of the two queues (usually the pointer to
 * RD(q). Flags are initialized by allocq in such a way RD(q) and WR(q) will
 * always work regardless the given queue.
 * The pointer given by allocq is actually a pointer to RD(q).
 */

#ifdef __KERNEL__
#if (defined(LINUX) && defined(USE_KMEM_CACHE))
#if __LIS_INTERNAL__
extern void lis_init_queues(void);
extern void lis_terminate_queues(void);
#endif
#endif

#if __LIS_INTERNAL__
/* Allocate a new queue pair
 * return NULL on failure 
 *
 */
extern queue_t * lis_allocq( const char *name );

/* Deallocate a new queue pair
 * return NULL on failure
 *
 */
extern void lis_freeq( queue_t *q );
#endif

#endif /* __KERNEL__ */

/* backq - Return the queue which feeds this one.
 * get ptr to the queue behind q. That queue with q_next == q.
 * returns NULL if no such queue
 *
 */
#ifdef __KERNEL__
extern queue_t * lis_backq_fcn(queue_t *q, char *f,int l)_RP;
extern queue_t * lis_backq(queue_t *q)_RP;
#endif				/* __KERNEL__ */

/* lis_backenable - enable back queue if QWANTW is set, i.e.
 *	if a back queue is full.
 */
#ifdef __KERNEL__
extern void lis_backenable(queue_t *q);
#endif				/* __KERNEL__ */

/* lis_getq - get message from head of queue
 *
 */
#ifdef __KERNEL__
extern mblk_t * lis_getq(queue_t *q)_RP;
#endif				/* __KERNEL__ */

/* putq- put a message into a queue
 *
 */
#ifdef __KERNEL__
extern int lis_putq(queue_t *q, mblk_t *mp)_RP;
extern void lis_putqf(queue_t *q, mblk_t *mp)_RP;
#endif				/* __KERNEL__ */

/* putbq - return a message to a queue
 *
 */
#ifdef __KERNEL__
extern int lis_putbq(queue_t *q, mblk_t *mp)_RP;
extern void lis_putbqf(queue_t *q, mblk_t *mp)_RP;
#endif				/* __KERNEL__ */

/*insq - insert nmp before emp. If emp is NULL, insert at end
 *	of queue. If the insertion is out-of-order, the insert fails.
 *	Returns 1 on success; 0 otherwise.
 *
 */
#ifdef __KERNEL__
extern int lis_insq(queue_t *q, mblk_t *emp, mblk_t *mp)_RP;
#endif				/* __KERNEL__ */

/* rmvq - remove a message from a queue. If the message
 *		does not exist, panic.
 *
 */
#ifdef __KERNEL__
extern void lis_rmvq(queue_t *q, mblk_t *mp)_RP;
#endif				/* __KERNEL__ */

/* lis_qenable - schedule a queue for service
 *
 */
#ifdef __KERNEL__
extern void lis_qenable(queue_t *q)_RP;
#if __LIS_INTERNAL__
extern void lis_retry_qenable(queue_t *q);
#endif
#endif				/* __KERNEL__ */

/* lis_setq - Set queue variables
 *
 */
#ifdef __KERNEL__
#if __LIS_INTERNAL__
extern void lis_setq(queue_t * q, struct qinit *rinit, struct qinit *winit);
#endif
#endif				/* __KERNEL__ */

/* lis_flushband - flush messages in a specified priority band.
 *	flag can be FLUSHDATA (flush only M_DATA, M_DELAY,
 *	M_PROTO, M_PCPROTO) or FLUSHALL.
 *
 */
#ifdef __KERNEL__
extern void lis_flushband(queue_t *q, unsigned char band, int flag)_RP;
#endif				/* __KERNEL__ */

/* lis_flushq - free messages from a queue, enabling upstream/downstream
 *	service routines if applicable. The flag is the same as for
 *	lis_flushband.
 *
 */
#ifdef __KERNEL__
extern void lis_flushq(queue_t *q, int flag)_RP;
#endif				/* __KERNEL__ */

/* putctl - allocate a message block, set the type,
 *	and put it on a queue. Returns 1 on success;
 *	0 if the allocation failed or type is one of
 *	M_DATA, M_PROTO or M_PCPROTO
 *
 */
#ifdef __KERNEL__
extern int lis_putctl(queue_t *q, int type, char *file_name, int line_nr)_RP;
extern int lis_putnextctl(queue_t *q, int type, char *file_name, int line_nr)_RP;
#endif				/* __KERNEL__ */

/* lis_putctl1 - as for lis_putctl, but with a one byte parameter
 *
 */
#ifdef __KERNEL__
extern int
lis_putctl1(queue_t *q, int type, int param, char *file_name, int line_nr)_RP;

extern int
lis_putnextctl1(queue_t *q, int type, int param, char *file_name, int line_nr)_RP;
#endif				/* __KERNEL__ */


/* lis_qsize - returns the number of messages on a queue
 *
 */
#ifdef __KERNEL__
extern int lis_qsize(queue_t *q)_RP;

#define	qsize		lis_qsize
#endif				/* __KERNEL__ */


/* lis_strqget - get information about a (band of a) queue.
 *	Valid values for what are QHIWAT, QLOWAT, QMAXPSZ,
 *	QMINPSZ, QCOUNT, QFIRST, QLAST, QFLAG.
 *	Returns 0 on success.
 *
 */
#ifdef __KERNEL__
extern int 
lis_strqget(queue_t *q, qfields_t what, unsigned char band, long *val)_RP;
#endif				/* __KERNEL__ */

/* lis_strqset - change information about a (band of a) queue.
 *	Valid values for what are QHIWAT, QLOWAT, QMAXPSZ,
 *	QMINPSZ.
 *	Returns 0 on success.
 *
 */
#ifdef __KERNEL__
extern int
lis_strqset(queue_t *q, qfields_t what, unsigned char band, long val)_RP;
#endif				/* __KERNEL__ */

#ifdef __KERNEL__
/*
 * Note:  Any routines that manipulate q_flag field of a queue need
 *        to use the "safe" versions for interrupt protection.
 */
extern void lis_safe_noenable(queue_t *q, char *f, int l)_RP;
extern void lis_safe_enableok(queue_t *q, char *f, int l)_RP;
#define lis_noenable(q)	 lis_safe_noenable(q, __FILE__, __LINE__)
#define lis_enableok(q)	 lis_safe_enableok(q, __FILE__, __LINE__)

extern int lis_safe_canenable(queue_t *q, char *f, int l)_RP;
#define lis_canenable(q) lis_safe_canenable(q, __FILE__, __LINE__)

extern queue_t *lis_safe_OTHERQ(queue_t *q, char *f, int l)_RP;
extern queue_t *lis_safe_RD(queue_t *q, char *f, int l)_RP;
extern queue_t *lis_safe_WR(queue_t *q, char *f, int l)_RP;
extern int lis_safe_SAMESTR(queue_t *q, char *f, int l)_RP;
#define LIS_OTHERQ(q)	lis_safe_OTHERQ(q, __FILE__, __LINE__)
#define LIS_RD(q)	lis_safe_RD(q, __FILE__, __LINE__)
#define LIS_WR(q)	lis_safe_WR(q, __FILE__, __LINE__)
#define LIS_SAMESTR(q)	lis_safe_SAMESTR(q, __FILE__, __LINE__)

#if __LIS_INTERNAL__
extern void lis_defer_msg(queue_t *q, mblk_t *mp, int retry, lis_flags_t *psw);
extern void lis_do_deferred_puts(queue_t *q);
#endif
extern void lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l)_RP;
extern void lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l)_RP;
#define lis_putnext(q,mp) 	lis_safe_putnext(q, mp, __FILE__, __LINE__)
#define lis_qreply(q,mp) 	lis_safe_qreply(q, mp, __FILE__, __LINE__)
#endif				/* __KERNEL__ */

/* Count messages on a queue 
 */
#ifdef __KERNEL__
extern int lis_qsize(queue_t *)_RP;			
#endif				/* __KERNEL__ */

/* Insert message(3) after message(2) or at start 
 */
#ifdef __KERNEL__
extern int lis_appq(queue_t *, mblk_t *, mblk_t *) _RP;
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#ifdef __KERNEL__

/* The functions below are to achieve mutual exclusion on the queue
 * They use the q_wait  wait queue to sleep and the q_lock char to
 * flag the lock.
 * you should use only lis_{lock,unlock}_queue().
 * the others are for internal use.
 */

void	lis_qprocson(queue_t *rdq) _RP;		/* SVR4 routine */
void	lis_qprocsoff(queue_t *rdq) _RP;		/* SVR4 routine */


/*
 * lis_freezestr, lis_unfreezestr
 *
 * Incomplete implementations of SVR4 freezestr/unfreezestr.  The LiS
 * implementation inhibits calling put/srv but not putq, getq, open or close.
 */
void	lis_freezestr(queue_t *q) _RP;
void	lis_unfreezestr(queue_t *q) _RP;

#define	freezestr	lis_freezestr
#define unfreezestr	lis_unfreezestr

/*  -------------------------------------------------------------------  */
/* lis_bcanput - search the stream starting from q until a service
 *	routine is found, if any, and test the found queue for
 *	flow control at a specified band. Schedule backenabling
 *	if flow controlled.
 *	For band==0 this is equivalent to canput. Returns 0 if
 *	flow controlled; 1 otherwise.
 *
 * test for flow cntl in band
 * returns STR_OK if msg can be put(), STR_ERR (0) if not.
 *
 */
extern int lis_bcanput(queue_t *q, unsigned char band)_RP;

/*  -------------------------------------------------------------------  */
/* lis_bcanputnext - search the stream starting from the queue after q
 *	until a service routine is found.
 *
 *	This is equivalent to lis_bcanput(q->q_next, band).  It is
 *	a routine in AT&T's MP DKI.
 *
 * returns STR_OK if msg can be put(), STR_ERR (0) if not.
 *
 */
extern int lis_bcanputnext(queue_t *q, unsigned char band)_RP;

/*  -------------------------------------------------------------------  */
/* lis_bcanput_anyband - search the stream starting from the queue q
 *	until a service routine is found.  Return true if there is some
 *	non-zero qband in that queue that can be written into.
 *
 * returns STR_OK if msg can be put(), STR_ERR (0) if not.
 *
 */
extern int lis_bcanput_anyband(queue_t *q)_RP;

/*  -------------------------------------------------------------------  */
/* lis_qcountstrm - return the accumulated q_count fields of all the
 *                  queues hooked together in this stream.
 *		    This routine also sets the QCLOSING flag for each
 *		    queue that it encounters.  It is used only by the
 *		    close logic to drain queues.
 */
extern int lis_qcountstrm(queue_t *q) _RP;


#endif /* __KERNEL__ */
/*  -------------------------------------------------------------------  */
#endif /*!_QUEUE_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
