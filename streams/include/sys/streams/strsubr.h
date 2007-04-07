/*****************************************************************************

 @(#) $Id: strsubr.h,v 0.9.2.79 2007/04/04 01:15:17 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2007/04/04 01:15:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strsubr.h,v $
 Revision 0.9.2.79  2007/04/04 01:15:17  brian
 - performance improvements (speeds up put and srv procedures)

 Revision 0.9.2.78  2007/03/30 11:59:10  brian
 - heavy rework of MP syncrhonization

 Revision 0.9.2.77  2007/03/25 19:01:10  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.76  2007/03/25 00:52:43  brian
 - synchronization updates

 Revision 0.9.2.75  2007/03/10 13:52:48  brian
 - checking in latest corrections for release

 Revision 0.9.2.74  2006/12/18 07:32:38  brian
 - lfs device names, autoload clone minors, device numbering, missing manpages

 Revision 0.9.2.73  2006/07/24 09:01:13  brian
 - results of udp2 optimizations

 Revision 0.9.2.72  2006/07/13 08:07:50  brian
 - added sk_buff data buffer allocation mechanism for stream head

 Revision 0.9.2.71  2006/07/10 12:22:40  brian
 - more fix for rwlock held across schedule detected by FC5 2.6.17

 Revision 0.9.2.70  2006/07/10 08:51:04  brian
 - fix for rwlock_t held across schedule detected by FC5 2.6.17 kernel

 Revision 0.9.2.69  2006/05/29 08:52:58  brian
 - started zero copy architecture

 Revision 0.9.2.68  2006/05/22 02:09:05  brian
 - changes from performance testing

 Revision 0.9.2.67  2006/02/22 11:37:18  brian
 - split giant wait queue into 4 independent queues

 Revision 0.9.2.66  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_STRSUBR_H__
#define __SYS_STREAMS_STRSUBR_H__

#ident "@(#) $RCSfile: strsubr.h,v $ $Name:  $($Revision: 0.9.2.79 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_STRSUBR_H__
#warning "Do no include sys/streams/strsubr.h directly, include sys/strsubr.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __STREAMS_EXTERN
#define __STREAMS_EXTERN __EXTERN streams_fastcall
#endif
#ifndef __STREAMS_ENTRYP
#define __STREAMS_ENTRYP __EXTERN streamscall
#endif

#include <linux/compiler.h>	/* for likely */
#include <linux/slab.h>		/* for kmem_cache_t */

#include <sys/stream.h>
#include <sys/sad.h>

#if 0
struct linesw {
	int (*l_open) ();
	int (*l_close) ();
	int (*l_read) ();
	int (*l_write) ();
	int (*l_ioctl) ();
	int (*l_input) ();
	int (*l_output) ();
	int (*l_mdmint) ();
};

struct termsw {
	int (*t_input) ();
	int (*t_output) ();
	int (*t_ioctl) ();
};
#endif

/* 
 *  STREAMS users don't need to know about our internal mdbblock allocation
 *  structures, queue pairs or others...
 */
struct strevent {
	union {
		struct {
			struct task_struct *procp;
			long events;
			int fd;
		} e;			/* stream event */
		struct {
			queue_t *queue;
			void streamscall (*func) (long);
			long arg;
			size_t size;
		} b;			/* bufcall event */
		struct {
			queue_t *queue;
			void streamscall (*func) (caddr_t);
			caddr_t arg;
			int pl;
			int cpu;
			struct timer_list timer;
		} t;			/* timeout event */
		struct {
			queue_t *queue;
			void streamscall (*func) (void *);
			void *arg;
			queue_t *q1, *q2, *q3, *q4;
		} w;			/* weld request */
	} x;
	struct strevent *se_next;
	struct strevent *se_prev;	/* actually hash list */
	int se_id;			/* identifier for this event structure */
	int se_seq;			/* use sequence number */
};

#define se_procp    x.e.procp
#define se_events   x.e.events
#define se_fd	    x.e.fd

#define se_func	    x.b.func
#define se_arg	    x.b.arg
#define se_size	    x.b.size

#if defined CONFIG_STREAMS_SYNCQS
/* synchronization queue structure */
typedef struct syncq {
	spinlock_t sq_lock;		/* spin lock for this structure */
	int sq_count;			/* no of threads inside (negative for exclusive) */
	struct task_struct *sq_owner;	/* exclusive owner */
#if 0
	int sq_nest;			/* lock nesting */
#endif
	wait_queue_head_t sq_waitq;	/* qopen/qclose waiters */
	struct strevent *sq_ehead;	/* head of event queue */
	struct strevent **sq_etail;	/* tail of event queue */
	queue_t *sq_qhead;		/* head of service queue */
	queue_t **sq_qtail;		/* tail of service queue */
	mblk_t *sq_mhead;		/* head of put queue */
	mblk_t **sq_mtail;		/* tail of put queue */
	struct syncq *sq_outer;		/* synch queue outside this one (if any) */
	unsigned int sq_flag;		/* synch queue flags */
	unsigned int sq_level;		/* synch queue level */
	struct syncq *sq_link;		/* synch queue schedule list */
	char sq_info[FMNAMESZ + 1];	/* synch queue info */
	atomic_t sq_refs;		/* structure references */
	struct syncq *sq_next;		/* list of all elsewhere structures */
	struct syncq **sq_prev;		/* list of all elsewhere structures */
#if defined CONFIG_STREAMS_DEBUG
	struct list_head *sq_list;	/* Strinfo list linkage */
#endif
} syncq_t;

/* we _really_ need our own flags here... */
enum {
	SQ_OUTER_BIT,
	SQ_INNER_BIT,
	SQ_EXCLUS_BIT,
	SQ_SHARED_BIT,
	SQ_WAITERS_BIT,
	SQ_BACKLOG_BIT,
	SQ_SCHED_BIT,
};

#define SQ_OUTER	(1<<SQ_OUTER_BIT)	/* this is an outer barrier (for Solaris compat) */
#define SQ_INNER	(1<<SQ_INNER_BIT)	/* this is an inner barrier */
#define SQ_EXCLUS	(1<<SQ_EXCLUS_BIT)	/* qopen/qclose excl at outer barrier */
#define SQ_SHARED	(1<<SQ_SHARED_BIT)	/* put shared at the inner barrier */
#define SQ_WAITERS	(1<<SQ_WAITERS_BIT)	/* this syncq has waiters */
#define SQ_BACKLOG	(1<<SQ_BACKLOG_BIT)	/* this syncq has a backlog left by non-waiters */
#define SQ_SCHED	(1<<SQ_SCHED_BIT)	/* this syncq is scheduled for backlog clearing */

#endif				/* defined CONFIG_STREAMS_SYNCQS */

/* stream head private structure */
struct stdata {
	volatile unsigned long sd_flag;	/* stream head state */
	queue_t *sd_rq;			/* rd queue for stream head */
	queue_t *sd_wq;			/* wr queue for stream head */
	dev_t sd_dev;			/* device number of driver */
	mblk_t *sd_iocblk;		/* message to return for ioctl */
	struct stdata * volatile sd_other;	/* other stream head for pipes */
//      struct streamtab *sd_strtab;    /* driver streamtab */
	struct inode *sd_inode;		/* back pointer to inode */
//      struct dentry *sd_dentry;       /* back pointer to dentry */
	struct file *sd_file;		/* back pointer to (current) file */
	rwlock_t sd_lock;		/* structure lock for this stream */
	rwlock_t sd_plumb;		/* plumbing and procedure lock for this stream */
	rwlock_t sd_freeze;		/* lock for freezing streams */
	struct task_struct *sd_freezer;	/* thread holding freeze lock */
	int sd_rdopt;			/* read options */
	int sd_wropt;			/* write options */
	int sd_eropt;			/* error options */
	int sd_iocid;			/* sequence id for active ioctl */
//      ushort sd_iocwait;              /* number of procs awaiting ioctl */
	pid_t sd_session;		/* controlling session id */
	pid_t sd_pgrp;			/* foreground process group */
	ushort sd_wroff;		/* write offset */
	ushort sd_wrpad;		/* write padding */
	unsigned char sd_band;		/* highest blocked band */
	ssize_t sd_minpsz;		/* cached sd_wq->q_next->q_minpsz */
	ssize_t sd_maxpsz;		/* cached sd_wq->q_next->q_maxpsz */
	ssize_t sd_strmsgsz;		/* cached sysctl_str_strmsgsz */
	ssize_t sd_strctlsz;		/* cached sysctl_str_strctlsz */
	int sd_rerror;			/* read error */
	int sd_werror;			/* write error */
	int sd_opens;			/* number of successful opens */
	int sd_readers;			/* number of streampipe readers */
	int sd_writers;			/* number of streampipe writers */
//      int sd_rwaiters;                /* number of waiters on read */
//      int sd_wwaiters;                /* number of waiters on write */
	int sd_nstrpush;		/* cached sysctl_str_nstrpush */
	int sd_pushcnt;			/* number of modules pushed */
	int sd_nanchor;			/* number of modules anchored */
	unsigned long sd_sigflags;	/* signal flags */
	struct strevent *sd_siglist;	/* list of procs for SIGPOLL */
	struct fasync_struct *sd_fasync;	/* list of procs for SIGIO */
	// struct pollhead sd_polllist; /* list of poll wakeup functions */
	wait_queue_head_t sd_polllist;	/* waiters on poll */
//      wait_queue_head_t sd_waitq;     /* waiters */
	wait_queue_head_t sd_rwaitq;	/* waiters on read/getmsg/getpmsg/I_RECVFD */
	wait_queue_head_t sd_wwaitq;	/* waiters on write/putmsg/putpmsg/I_SENDFD */
	wait_queue_head_t sd_iwaitq;	/* waiters on ioctl */
	wait_queue_head_t sd_owaitq;	/* waiters on open bit */
//      mblk_t *sd_mark;                /* pointer to marked message */
	ulong sd_closetime;		/* queue drain wait time on close */
	ulong sd_ioctime;		/* time to wait for ioctl() acknowledgement */
	ulong sd_rtime;			/* time to forward held message */
	struct stdata *sd_scanq;	/* next on scan list */
//      klock_t sd_klock;               /* lock for queues under this stream */
	struct cdevsw *sd_cdevsw;	/* device entry */
	struct list_head sd_list;	/* list against device */
//      struct semaphore sd_mutex;      /* mutex for system calls */
	struct stdata *sd_clone;	/* clone streams */
	struct stdata *sd_links;	/* linked streams */
	struct stdata *sd_link_next;	/* next linked stream */
	struct linkblk *sd_linkblk;	/* link block for this stream */
	struct wantio *sd_directio;	/* direct io for this stream head */
};

typedef struct stdata stdata_t;

#if 0				/* AIX has these members */
struct stdata {
	queue_t wq;			/* stream write queue */
	queue_t rq;			/* stream read queue */
	dev_t dev;			/* associated evice number of stream */
	int read_mode;			/* read mode */
	int write_mode;			/* write mode */
	long close_wait_timeout;	/* close wait timeout */
	int read_error;			/* read error on stream */
	int write_error;		/* write error on stream */
	ushort flags;			/* stream head flags */
	int push_cnt;			/* module push count */
	int wroff;			/* write offset */
	int ioc_id;			/* id of outstanding M_IOCTL request */
	int ioc_mp;			/* outstanding M_IOCTL message */
	struct stdata *next;		/* next stream head on the link */
	void *pollq;			/* list of active polls */
	void *sigsq;			/* list of active SIGPOLLs */
	void *shttyp;			/* pointer to tty information */
};
#endif

enum {
	IOCWAIT_BIT,
	RSLEEP_BIT,
	WSLEEP_BIT,
	STRPRI_BIT,
	STRHUP_BIT,
	STWOPEN_BIT,
	STPLEX_BIT,
	STRISTTY_BIT,
	STRDERR_BIT,
	STWRERR_BIT,
	STRCLOSE_BIT,
	SNDMREAD_BIT,
	STRHOLD_BIT,
	STRNDEL_BIT,
	STRMSIG_BIT,
	STRDELIM_BIT,
	STRTOSTOP_BIT,
	STRISFIFO_BIT,
	STRISPIPE_BIT,
	STRISSOCK_BIT,
	STRMOUNT_BIT,
	STRCSUM_BIT,
	STRCRC32C_BIT,
	STRSKBUFF_BIT,
};

#define IOCWAIT	    (1<<IOCWAIT_BIT)	/* ioctl in progress */
#define RSLEEP	    (1<<RSLEEP_BIT)	/* process sleeping on read */
#define WSLEEP	    (1<<WSLEEP_BIT)	/* process sleeping on write */
#define STRPRI	    (1<<STRPRI_BIT)	/* priority message waiting */
#define STRHUP	    (1<<STRHUP_BIT)	/* stream is hung up */
#define STWOPEN	    (1<<STWOPEN_BIT)	/* strem head open in progress */
#define STPLEX	    (1<<STPLEX_BIT)	/* stream linked under mux */
#define STRISTTY    (1<<STRISTTY_BIT)	/* stream is a terminal */
#define STRDERR	    (1<<STRDERR_BIT)	/* M_ERROR for read received */
#define STWRERR	    (1<<STWRERR_BIT)	/* M_ERROR for write received */
#define STRCLOSE    (1<<STRCLOSE_BIT)	/* wait for strclose to complete */
#define SNDMREAD    (1<<SNDMREAD_BIT)	/* send M_READ msg when read issued */
#define STRHOLD	    (1<<STRHOLD_BIT)	/* coallese written messages */
#define STRNDEL	    (1<<STRNDEL_BIT)	/* non-STREAM tty semantic for O_NDELAY */
#define STRMSIG	    (1<<STRMSIG_BIT)	/* M_SIG at head of queue */
#define STRDELIM    (1<<STRDELIM_BIT)	/* generate delimited messages */
#define STRTOSTOP   (1<<STRTOSTOP_BIT)	/* stop timeout */
#define STRISFIFO   (1<<STRISFIFO_BIT)	/* stream is a fifo */
#define STRISPIPE   (1<<STRISPIPE_BIT)	/* stream is a STREAMS pipe */
#define STRISSOCK   (1<<STRISSOCK_BIT)	/* stream is a STREAMS socket */
#define STRMOUNT    (1<<STRMOUNT_BIT)	/* stream head is fattached */
#define STRCSUM	    (1<<STRCSUM_BIT)	/* checksum on copyin for write (UDP/TCP) */
#define STRCRC32C   (1<<STRCRC32C_BIT)	/* checksum on copyin for write (CRC32C) */
#define STRSKBUFF   (1<<STRSKBUFF_BIT)	/* allocated sk_buffs for data at stream head */

/* unfortunately AIX appears to mix read and write option flags with stream head flags */
#if 0				/* AIX compatible flags */
#define F_STH_READ_ERROR      0x0001	/* M_ERROR with read error received, fail all reads */
#define F_STH_WRITE_ERROR     0x0002	/* M_ERROR with read error received, fail all writes */
#define F_STH_HANGUP	      0x0004	/* M_HANGUP received, no more data */
#define F_STH_NDELON	      0x0008	/* Do TTY semantics for ONDELAY handling. */
#define F_STH_ISATTY	      0x0010	/* The stream acts as a terminal. */
#define F_STH_MREADON	      0x0020	/* Generate M_READ messages. */
#define F_STH_TOSTOP	      0x0040	/* Diallow background writes (for job control). */
#define F_STH_PIPE	      0x0080	/* Stream is one end of pipe or FIFO. */
#define F_STH_WPIPE	      0x0100	/* Stream is "write" side of pipe. */
#define F_STH_FIFO	      0x0200	/* Stream is a FIFO. */
#define F_STH_LINKED	      0x0400	/* Stream has one or more lower streams linked. */
#define F_STH_CTTY	      0x0800	/* Stream is a controlling tty. */
#define F_STH_UNDEFINED_1     0x1000	/* not defined */
#define F_STH_UNDEFINED_2     0x2000	/* not defined */
#define F_STH_CLOSED	      0x4000	/* Stream has been closed, and should be freed. */
#define F_STH_CLOSING	      0x8000	/* Stream is closing. */
#else
#define F_STH_READ_ERROR      STRDERR	/* M_ERROR with read error received, fail all reads */
#define F_STH_WRITE_ERROR     STWRERR	/* M_ERROR with read error received, fail all writes */
#define F_STH_HANGUP	      STRHUP	/* M_HANGUP received, no more data */
#define F_STH_NDELON	      0x0000	/* Do TTY semantics for ONDELAY handling. */
#define F_STH_ISATTY	      STRISTTY	/* The stream acts as a terminal. */
#define F_STH_MREADON	      SNDMREAD	/* Generate M_READ messages. */
#define F_STH_TOSTOP	      STRTOSTOP	/* Diallow background writes (for job control). */
#define F_STH_PIPE	      STRISPIPE	/* Stream is one end of pipe or FIFO. */
#define F_STH_WPIPE	      0x0000	/* Stream is "write" side of pipe. */
#define F_STH_FIFO	      STRISFIFO	/* Stream is a FIFO. */
#define F_STH_LINKED	      STPLEX	/* Stream has one or more lower streams linked. */
#define F_STH_CTTY	      0x0000	/* Stream is a controlling tty. */
#define F_STH_UNDEFINED_1     0x0000	/* not defined */
#define F_STH_UNDEFINED_2     0x0000	/* not defined */
#define F_STH_CLOSED	      0x0000	/* Stream has been closed, and should be freed. */
#define F_STH_CLOSING	      STRCLOSE	/* Stream is closing. */
#endif

struct strinfo {
	kmem_cachep_t si_cache;		/* memory cache */
	rwlock_t si_rwlock;		/* lock for these entries */
	atomic_t si_cnt;		/* count of entries in the list */
	int si_hwl;			/* high water level for entries */
	struct list_head si_head;	/* entries in the list */
};

enum {
	DYN_STREAM,			/* struct shinfo */
	DYN_QUEUE,			/* struct queinfo */
	DYN_MSGBLOCK,			/* struct mbinfo */
	DYN_MDBBLOCK,			/* struct dbinfo */
	DYN_LINKBLK,			/* struct linkinfo */
	DYN_STREVENT,			/* struct seinfo */
	DYN_QBAND,			/* struct bandinfo */
	DYN_STRAPUSH,			/* struct apinfo */
	DYN_DEVINFO,			/* struct devinfo */
	DYN_MODINFO,			/* struct mdlinfo */
#if defined CONFIG_STREAMS_SYNCQS
	DYN_SYNCQ,			/* struct syncq */
#endif
	DYN_SIZE,			/* size */
};

struct strthread {
	volatile unsigned long flags;	/* flags */
	struct task_struct *proc;	/* task */
	atomic_t lock;			/* thread lock */
#if !defined CONFIG_STREAMS_NORECYCLE
	mblk_t *freemblk_head;		/* head of free mdbblocks cached */
	mblk_t **freemblk_tail;		/* tail of free mdbblocks cached */
	int freemblks;			/* number of mblks on the free list */
#endif
	queue_t *qhead;			/* first queue in scheduled queues */
	queue_t **qtail;		/* last queue in scheduled queues */
#if defined CONFIG_STREAMS_SYNCQS
	struct syncq_cookie *syncq_cookie;	/* open/close syncrhonization queue cookie */
	syncq_t *sqhead;		/* first syncq in scheduled syncqs */
	syncq_t **sqtail;		/* last sycnq in scheduled sycnqs */
	mblk_t *strmfuncs_head;		/* head of m_func pending exec */
	mblk_t **strmfuncs_tail;	/* tail of m_func pending exec */
#endif
	struct strevent *freeevnt_head;	/* head of free stream events cached */
	struct strevent **freeevnt_tail;	/* tail of free stream events cached */
	struct strevent *strbcalls_head;	/* head of bufcalls pending exec */
	struct strevent **strbcalls_tail;	/* tail of bufcalls pending exec */
	struct strevent *strtimout_head;	/* head of timeouts pending exec */
	struct strevent **strtimout_tail;	/* tail of timeouts pending exec */
	struct strevent *strevents_head;	/* head of strevent pending exec */
	struct strevent **strevents_tail;	/* tail of strevent pending exec */
	struct stdata *scanqhead;		/* head of STREAMS scan queue */
	struct stdata **scanqtail;		/* tail of STREAMS scan queue */
	mblk_t *freemsg_head;		/* head of flushed messages to free */
	mblk_t **freemsg_tail;		/* tail of flushed messages to free */
} __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#ifndef BIG_COMPILE
extern struct strthread strthreads[];
#endif

__STREAMS_EXTERN void __raise_streams(void);

/* aligned so processors keep out of each other's way */

enum {
	qrunflag,			/* at least 1 queue to run */
	strbcflag,			/* bufcall() functions can be run */
	strbcwait,			/* bufcall() functions waiting */
	flushwork,			/* flushq has messages to free */
	freeblks,			/* fastfreed blocks for the cache */
	strtimout,			/* timeout() functions can be run */
	scanqflag,			/* scanqueue list is active */
	strevents,			/* strevents() functions can be run */
	strmfuncs,			/* m_func's can be run */
	qsyncflag,			/* at least 1 syncq to run */
	qwantrun,			/* runqueues() wanted to run */
};

#define QRUNFLAG	(1 << qrunflag	)	/* 0x00000001 */
#define STRMFUNCS	(1 << strmfuncs )	/* 0x00000002 */
#define STRBCFLAG	(1 << strbcflag	)	/* 0x00000004 */
#define STRBCWAIT	(1 << strbcwait	)	/* 0x00000008 */
#define FLUSHWORK	(1 << flushwork	)	/* 0x00000010 */
#define FREEBLKS	(1 << freeblks	)	/* 0x00000020 */
#define STRTIMOUT	(1 << strtimout )	/* 0x00000040 */
#define SCANQFLAG	(1 << scanqflag )	/* 0x00000080 */
#define STREVENTS	(1 << strevents )	/* 0x00000100 */
#define QSYNCFLAG	(1 << qsyncflag )	/* 0x00000200 */
#define QWANTRUN	(1 << qwantrun  )	/* 0x00000400 */

#define QRUNFLAGS	(QRUNFLAG|STRMFUNCS|STRBCFLAG|STRBCWAIT|FLUSHWORK|FREEBLKS|STRTIMOUT|SCANQFLAG|STREVENTS)

struct shinfo {
	struct stdata sh_stdata;
	atomic_t sh_refs;		/* references to this structure */
#if defined CONFIG_STREAMS_DEBUG
	struct list_head sh_list;
#endif
};

struct queinfo {
	queue_t rq;			/* read queue */
	queue_t wq;			/* write queue */
	struct stdata *qu_str;		/* stream head for this queue pair */
	wait_queue_head_t qu_qwait;	/* wait queue for qwait */
	atomic_t qu_refs;		/* references to this structure */
#if defined CONFIG_STREAMS_DEBUG
	struct list_head qu_list;
#endif
};

#define rqstream(__rq) (*(struct stdata * volatile *)&(((struct queinfo *)(__rq))->qu_str))
#define wqstream(__wq) (*(struct stdata * volatile *)&(((struct queinfo *)(_RD(__wq)))->qu_str))
#define qstream(__q) rqstream(RD(__q))

enum {
	QU_MODULE_BIT,			/* queue pair is for module */
	QU_DRIVER_BIT,			/* queue pair is for driver */
	QU_STRMHD_BIT,			/* queue pair is for stream head */
	QU_QSYNCH_BIT,			/* queue pair sync bit */
};

#define QU_MODULE   (1 << QU_MODULE_BIT)
#define QU_DRIVER   (1 << QU_DRIVER_BIT)
#define QU_STRMHD   (1 << QU_STRMHD_BIT)
#define QU_QSYNCH   (1 << QU_QSYNCH_BIT)

/* 12 extra bytes on 32-bit, 24 extra on 64-bit */
/* additional 8 bytes on 32-bit, 16 on 64-bit, debug mode */
struct mbinfo {
	mblk_t m_mblock;
	void (*m_func) (void);		/* allocating function SVR4 compatible */
	queue_t *m_queue;		/* last queue for this block */
	void *m_private;		/* private information for deferral */
#if defined CONFIG_STREAMS_DEBUG
	struct list_head m_list;
#endif
};

/* additional 8 bytes on 32-bit, 16 on 64-bit, debug mode */
struct dbinfo {
	dblk_t d_dblock;
#if defined CONFIG_STREAMS_DEBUG
	struct list_head db_list;
#endif
};

struct linkinfo {
	struct linkblk li_linkblk;
#if defined CONFIG_STREAMS_DEBUG
	struct list_head li_list;
#endif
};

#define SE_STREAM	0
#define SE_BUFCALL	1
#define SE_TIMEOUT	2
#define SE_WELDQ	3
#define SE_UNWELDQ	4

struct seinfo {
	struct strevent s_strevent;
	unsigned int s_type;
	queue_t *s_queue;		/* queue guess for this strevent */
#if defined CONFIG_STREAMS_DEBUG
	struct list_head s_list;
#endif
};

struct qbinfo {
	struct qband qbi_qband;
	atomic_t qbi_refs;		/* references to this structure */
#if defined CONFIG_STREAMS_DEBUG
	struct list_head qbi_list;
#endif
};

struct apinfo {
	struct strapush api_sap;
	struct list_head api_more;
	atomic_t api_refs;
#if defined CONFIG_STREAMS_DEBUG
	struct list_head api_list;
#endif
};

struct devinfo {
	struct list_head di_list;	/* list of devices for this switch table entry */
	struct list_head di_hash;	/* list of major hashes in slot */
	struct cdevsw *di_dev;		/* switch table entry */
	struct module_info *di_info;	/* quick pointer to module info */
	atomic_t di_refs;		/* structure references */
	atomic_t di_count;		/* open count */
	major_t major;			/* major device number */
	minor_t minor;			/* minor device number */
#if defined CONFIG_STREAMS_DEBUG
	struct devinfo *di_next;	/* Strinfo list linkage */
	struct devinfo *di_prev;	/* Strinfo list linkage */
#endif
};

struct mdlinfo {
	struct list_head mi_list;	/* list of modules for this switch table entry */
	struct list_head mi_hash;	/* list of modid hashes in slot */
	struct fmodsw *mi_mod;		/* switch table entry */
	struct module_info *mi_info;	/* quick pointer to module info */
	atomic_t mi_refs;		/* structure references */
	atomic_t mi_count;		/* open count */
	modID_t modid;			/* module id number */
#if defined CONFIG_STREAMS_DEBUG
	struct mdlinfo *mi_next;	/* Strinfo list linkage */
	struct mdlinfo *mi_prev;	/* Strinfo list linkage */
#endif
};

#define was128	(32*sizeof(ulong))
#define HDRSZ	(sizeof(struct mbinfo)+sizeof(struct dbinfo))
#define BUFSZ	(was128-HDRSZ)
#define FASTBUF ((BUFSZ >= 128) ? 128 : ((BUFSZ >= 96) ? 96 : ((BUFSZ >= 64) ? 64 : 32)))
/* 128 - 32 - (12 + 20) = 64 => 64 bytes fastbuf on 32-bit (48 => 32 bytes in debug mode) */
/* 256 - 64 - (24 + 32) = 136 => 128 bytes fastbuf on 64-bit (104 => 96 bytes in debug mode) */
/* having a bunch more for 64-bit is a good idea because elements of M_PROTO blocks could be larger
   as well. */

struct mdbblock {
	struct mbinfo msgblk;
	struct dbinfo datablk;
	unsigned char pad[BUFSZ - FASTBUF];
	unsigned char databuf[FASTBUF];	/* should be nicely 32, 64 or 128 byte aligned */
};

/* from strsched.c */
__STREAMS_EXTERN void streams_schedule(void);
__STREAMS_EXTERN bcid_t __bufcall(queue_t *q, unsigned size, int priority,
				  void streamscall (*function) (long), long arg);
__STREAMS_EXTERN toid_t __timeout(queue_t *q, timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
				  unsigned long pl, int cpu);

__STREAMS_EXTERN int setsq(queue_t *q, struct fmodsw *fmod);
__STREAMS_EXTERN void qscan(struct stdata *sd);

/* from strlookup.c */
extern struct list_head cdevsw_list;	/* Drivers go here */
extern struct list_head fmodsw_list;	/* Modules go here */

#if 0
__STREAMS_EXTERN struct list_head cminsw_list;	/* Minors go here */
#endif
extern int cdev_count;			/* Driver count */
extern int fmod_count;			/* Module count */
extern int cmin_count;			/* Node count */
__STREAMS_EXTERN struct devnode *__cmaj_lookup(major_t major);
__STREAMS_EXTERN struct cdevsw *__cdev_lookup(major_t major);
__STREAMS_EXTERN struct cdevsw *__cdrv_lookup(modID_t modid);
__STREAMS_EXTERN struct devnode *__cmin_lookup(struct cdevsw *cdev, minor_t minor);
__STREAMS_EXTERN struct fmodsw *__fmod_lookup(modID_t modid);
__STREAMS_EXTERN struct cdevsw *__cdev_search(const char *name);
__STREAMS_EXTERN struct fmodsw *__fmod_search(const char *name);
__STREAMS_EXTERN struct devnode *__cmin_search(struct cdevsw *cdev, const char *name);
__STREAMS_EXTERN void *__smod_search(const char *name);
__STREAMS_EXTERN struct fmodsw *fmod_str(const struct streamtab *str);
__STREAMS_EXTERN struct cdevsw *cdev_str(const struct streamtab *str);
__STREAMS_EXTERN struct cdevsw *sdev_get(major_t major);
__STREAMS_EXTERN void sdev_put(struct cdevsw *cdev);
__STREAMS_EXTERN struct cdevsw *cdrv_get(modID_t modid);
__STREAMS_EXTERN void cdrv_put(struct cdevsw *cdev);
__STREAMS_EXTERN struct fmodsw *fmod_get(modID_t modid);
__STREAMS_EXTERN void fmod_put(struct fmodsw *fmod);
__STREAMS_EXTERN struct cdevsw *cdev_find(const char *name);
__STREAMS_EXTERN struct cdevsw *cdev_match(const char *name);
__STREAMS_EXTERN struct fmodsw *fmod_find(const char *name);
__STREAMS_EXTERN struct devnode *cmin_find(struct cdevsw *cdev, const char *name);
__STREAMS_EXTERN struct devnode *cmin_get(const struct cdevsw *cdev, minor_t minor);
__STREAMS_EXTERN struct devnode *cmaj_get(const struct cdevsw *cdev, major_t major);
__STREAMS_EXTERN minor_t cdev_minor(struct cdevsw *cdev, major_t major, minor_t minor);

/* from strreg.c */
__STREAMS_EXTERN int register_cmajor(struct cdevsw *cdev, major_t major,
				     struct file_operations *fops);
__STREAMS_EXTERN int unregister_cmajor(struct cdevsw *cdev, major_t major);

/* other internals */
__STREAMS_EXTERN int sdev_ini(struct cdevsw *cdev, modID_t modid);
__STREAMS_EXTERN int sdev_add(struct cdevsw *cdev);
__STREAMS_EXTERN void sdev_del(struct cdevsw *cdev);
__STREAMS_EXTERN void sdev_rel(struct cdevsw *cdev);
extern rwlock_t cdevsw_lock;
__STREAMS_EXTERN void cmaj_add(struct devnode *cmaj, struct cdevsw *cdev, major_t major);
__STREAMS_EXTERN void cmaj_del(struct devnode *cmaj, struct cdevsw *cdev);
__STREAMS_EXTERN int cmin_ini(struct devnode *cmin, struct cdevsw *cdev, minor_t minor);
__STREAMS_EXTERN int cmin_add(struct devnode *cmin, struct cdevsw *cdev);
__STREAMS_EXTERN void cmin_del(struct devnode *cmin, struct cdevsw *cdev);
__STREAMS_EXTERN void cmin_rel(struct devnode *cmin);

#if 0
__STREAMS_EXTERN rwlock_t cminsw_lock;
#endif

__STREAMS_EXTERN long do_fattach(const struct file *file, const char *file_name);
__STREAMS_EXTERN long do_fdetach(const char *file_name);
__STREAMS_EXTERN long do_spipe(int *fds);

__STREAMS_EXTERN void fmod_add(struct fmodsw *fmod, modID_t modid);
__STREAMS_EXTERN void fmod_del(struct fmodsw *fmod);
extern rwlock_t fmodsw_lock;

__STREAMS_EXTERN int spec_reparent(struct file *file, struct cdevsw *cdev, dev_t dev);
__STREAMS_EXTERN int spec_open(struct file *file, struct cdevsw *cdev, dev_t dev, int sflag);
__STREAMS_EXTERN struct vfsmount *specfs_mount(void);
__STREAMS_EXTERN void specfs_umount(void);

#ifndef BIG_COMPILE
__STREAMS_EXTERN struct linkblk *alloclk(void);
__STREAMS_EXTERN void freelk(struct linkblk *l);
#endif

__STREAMS_EXTERN struct stdata *allocstr(void);
__STREAMS_EXTERN void freestr(struct stdata *sd);

#ifndef BIG_COMPILE
__STREAMS_EXTERN struct stdata *sd_get(struct stdata *sd);
__STREAMS_EXTERN void sd_put(struct stdata **sdp);

__STREAMS_EXTERN int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag,
			      int sflag, cred_t *crp);
#endif

#if 0
__STREAMS_EXTERN struct devinfo *di_alloc(struct cdevsw *cdev);
__STREAMS_EXTERN void di_put(struct devinfo *di);
#endif

__STREAMS_EXTERN struct strevent *sealloc(void);
__STREAMS_EXTERN int sefree(struct strevent *se);

#ifndef BIG_COMPILE
extern ulong sysctl_str_nstrpush;
extern ulong sysctl_str_strctlsz;
#endif

__STREAMS_EXTERN int register_clone(struct cdevsw *cdev);
__STREAMS_EXTERN int unregister_clone(struct cdevsw *cdev);

__STREAMS_EXTERN void runqueues(void);

/* If you use this structure, you might want to upcall to the stream head functions behind these.
 * These are them.  Note that the functions above or below are called after acquiring a reference to
 * the STREAM head but before performing basic access checks.  Note also, that ioctl will be an
 * unlocked_ioctl on systems that support it.  Take your own big kernel locks if you need to. */

__STREAMS_EXTERN unsigned int strpoll(struct file *file, struct poll_table_struct *poll);
__STREAMS_EXTERN ssize_t strread(struct file *file, char *buf, size_t len, loff_t *ppos);
__STREAMS_EXTERN ssize_t strwrite(struct file *file, const char *buf, size_t len, loff_t *ppos);
__STREAMS_EXTERN ssize_t strsendpage(struct file *file, struct page *page, int offset, size_t size,
				     loff_t *ppos, int more);
__STREAMS_EXTERN int strgetpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp,
				int *bandp, int *flagsp);
__STREAMS_EXTERN int strputpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp,
				int band, int flags);
__STREAMS_EXTERN int strioctl(struct file *file, unsigned int cmd, unsigned long arg);

/* stream head read put and write service procedures, and open/close for use by replacement stream
 * heads */
__STREAMS_ENTRYP int strrput(queue_t *q, mblk_t *mp);
__STREAMS_ENTRYP int strwput(queue_t *q, mblk_t *mp);
__STREAMS_ENTRYP int strwsrv(queue_t *q);
__STREAMS_ENTRYP int str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp);
__STREAMS_ENTRYP int str_close(queue_t *q, int oflag, cred_t *crp);

extern struct file_operations strm_f_ops;

/*
 *  There are two ways of handling atomicity in per-cpu kernel threads, suppressing interrupts or
 *  using atomic exchanges.  For SMP it is probably better to suppress interrupts to avoid locking
 *  the bus.  Probably for UP too, because it kills the cache.  These macros also allow performance
 *  testing of the difference.
 */
#if 0
#define XCHG(__a,__b) xchg((__a),(__b))
#endif
#if 0
#define XCHG(__a,__b) \
((typeof(__b))({ \
	unsigned long __flags; \
	typeof(__b) __result; \
	streams_local_save(__flags); \
	__result = *(__a); \
	prefetchw(__result); \
	*(__a) = (__b); \
	streams_local_restore(__flags); \
	__result; \
}))
#endif
#if 1
#define XCHG(__a,__b) \
((typeof(__b))({ \
	typeof(__b) __result; \
	__result = *(__a); \
	prefetchw(__result); \
	*(__a) = (__b) ; \
	__result; \
}))
#endif

#endif				/* __SYS_STREAMS_STRSUBR_H__ */
