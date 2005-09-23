/*****************************************************************************

 @(#) $Id: strsubr.h,v 0.9.2.46 2005/09/23 05:49:44 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/09/23 05:49:44 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAMS_STRSUBR_H__
#define __SYS_STREAMS_STRSUBR_H__

#ident "@(#) $RCSfile: strsubr.h,v $ $Name:  $($Revision: 0.9.2.46 $) $Date: 2005/09/23 05:49:44 $"

#ifndef __SYS_STRSUBR_H__
#warning "Do no include sys/streams/strsubr.h directly, include sys/strsubr.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

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
			void (*func) (long);
			long arg;
			size_t size;
		} b;			/* bufcall event */
		struct {
			queue_t *queue;
			void (*func) (caddr_t);
			caddr_t arg;
			int pl;
			int cpu;
			struct timer_list timer;
		} t;			/* timeout event */
		struct {
			queue_t *queue;
			void (*func) (void *);
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
	int sq_nest;			/* lock nesting */
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
	atomic_t sq_refs;		/* structure references */
	struct syncq *sq_next;		/* list of all structures */
	struct syncq *sq_prev;		/* list of all structures */
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
#endif

/* stream head private structure */
struct stdata {
	queue_t *sd_rq;			/* rd queue for stream head */
	queue_t *sd_wq;			/* wr queue for stream head */
	dev_t sd_dev;			/* device number of driver */
	mblk_t *sd_iocblk;		/* message to return for ioctl */
	struct stdata *sd_other;	/* other stream head for pipes */
//      struct streamtab *sd_strtab;    /* driver streamtab */
	struct inode *sd_inode;		/* back pointer to inode */
//      struct dentry *sd_dentry;       /* back pointer to dentry */
	struct file *sd_file;		/* back pointer to (current) file */
	ulong sd_flag;			/* stream head state */
	ulong sd_rdopt;			/* read options */
	ulong sd_wropt;			/* write options */
	ulong sd_eropt;			/* error options */
	ulong sd_iocid;			/* sequence id for active ioctl */
//      ushort sd_iocwait;              /* number of procs awaiting ioctl */
	pid_t sd_session;		/* controlling session id */
	pid_t sd_pgrp;			/* foreground process group */
	ushort sd_wroff;		/* write offset */
	ssize_t sd_minpsz;		/* cached sd_wq->q_next->q_minpsz */
	ssize_t sd_maxpsz;		/* cached sd_wq->q_next->q_maxpsz */
	int sd_rerror;			/* read error */
	int sd_werror;			/* write error */
	int sd_opens;			/* number of successful opens */
	int sd_readers;			/* number of streampipe readers */
	int sd_writers;			/* number of streampipe writers */
//      int sd_rwaiters;                /* number of waiters on read */
//      int sd_wwaiters;                /* number of waiters on write */
	int sd_pushcnt;			/* number of modules pushed */
	int sd_nanchor;			/* number of modules anchored */
	unsigned long sd_sigflags;	/* signal flags */
	struct strevent *sd_siglist;	/* list of procs for SIGPOLL */
	struct fasync_struct *sd_fasync;	/* list of procs for SIGIO */
	// struct pollhead sd_polllist; /* list of poll wakeup functions */
	wait_queue_head_t sd_waitq;	/* waiters */
//      mblk_t *sd_mark;                /* pointer to marked message */
	ulong sd_closetime;		/* queue drain wait time on close */
	ulong sd_rtime;			/* time to forward held message */
	ulong sd_ioctime;		/* time to wait for ioctl() acknowledgement */
//	klock_t sd_klock;		/* lock for queues under this stream */
	rwlock_t sd_lock;		/* structure lock for this stream */
	rwlock_t sd_plumb;		/* plumbing and procedure lock for this stream */
	rwlock_t sd_freeze;		/* lock for freezing streams */
	struct task_struct *sd_freezer;	/* thread holding freeze lock */
	struct cdevsw *sd_cdevsw;	/* device entry */
	struct list_head sd_list;	/* list against device */
//      struct semaphore sd_mutex;      /* mutex for system calls */
	struct stdata *sd_clone;	/* clone streams */
	struct stdata *sd_links;	/* linked streams */
	struct stdata *sd_link_next;	/* next linked stream */
	struct linkblk *sd_linkblk;	/* link block for this stream */
	struct wantio *sd_directio;	/* direct io for this stream head */
	struct qinit *sd_muxinit;	/* oopy of read side qinit for intercept */
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

#define STRHIGH		5120	/* default hi water mark */
#define STRLOW		1024	/* default lo water mark */
#define STRMAXPSZ	(1<<12)	/* default max psz */
#define STRMINPSZ	0	/* default max psz */

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
	kmem_cache_t *si_cache;		/* memory cache */
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
	int lock;			/* thread lock */
	queue_t *qhead;			/* first queue in scheduled queues */
	queue_t **qtail;		/* last queue in scheduled queues */
	queue_t *currentq;		/* current queue being processed */
#if defined CONFIG_STREAMS_SYNCQS
	syncq_t *sqhead;		/* first syncq in scheduled syncqs */
	syncq_t **sqtail;		/* last sycnq in scheduled sycnqs */
#endif
	mblk_t *strmfuncs_head;		/* head of m_func pending exec */
	mblk_t **strmfuncs_tail;	/* tail of m_func pending exec */
	struct strevent *strbcalls_head;	/* head of bufcalls pending exec */
	struct strevent **strbcalls_tail;	/* tail of bufcalls pending exec */
	struct strevent *strtimout_head;	/* head of timeouts pending exec */
	struct strevent **strtimout_tail;	/* tail of timeouts pending exec */
	struct strevent *strevents_head;	/* head of strevent pending exec */
	struct strevent **strevents_tail;	/* tail of strevent pending exec */
	queue_t *scanqhead;		/* head of STREAMS scan queue */
	queue_t **scanqtail;		/* tail of STREAMS scan queue */
	mblk_t *freemsg_head;		/* head of flushed messages to free */
	mblk_t **freemsg_tail;		/* tail of flushed messages to free */
	mblk_t *freemblk_head;		/* head of free mdbblocks cached */
	mblk_t **freemblk_tail;		/* tail of free mdbblocks cached */
	struct strevent *freeevnt_head;	/* head of free stream events cached */
	struct strevent **freeevnt_tail;	/* tail of free stream events cached */
} __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

extern struct strthread strthreads[];

extern void STREAMS_FASTCALL(__raise_streams(void));

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

#define QRUNFLAG	(1 << qrunflag	)
#define STRMFUNCS	(1 << strmfuncs )
#define STRBCFLAG	(1 << strbcflag	)
#define STRBCWAIT	(1 << strbcwait	)
#define FLUSHWORK	(1 << flushwork	)
#define FREEBLKS	(1 << freeblks	)
#define STRTIMOUT	(1 << strtimout )
#define SCANQFLAG	(1 << scanqflag )
#define STREVENTS	(1 << strevents )
#define QSYNCFLAG	(1 << qsyncflag )
#define QWANTRUN	(1 << qwantrun  )

#define QRUNFLAGS	(QRUNFLAG|STRMFUNCS|STRBCFLAG|STRBCWAIT|FLUSHWORK|FREEBLKS|STRTIMOUT|SCANQFLAG|STREVENTS)

struct shinfo {
	struct stdata sh_stdata;
	atomic_t sh_refs;		/* references to this structure */
	struct list_head sh_list;
};

struct queinfo {
	queue_t rq;			/* read queue */
	queue_t wq;			/* write queue */
	struct stdata *qu_str;		/* stream head for this queue pair */
	wait_queue_head_t qu_qwait;	/* wait queue for qwait */
#if 0
	union {
		struct fmodsw *fmod;	/* streams module */
		struct cdevsw *cdev;	/* streams driver */
	} qu_u;
	int qu_flags;			/* queue pair flags */
#endif
#if 0
	klock_t qu_klock;		/* lock for this queue pair */
#endif
	atomic_t qu_refs;		/* references to this structure */
	struct list_head qu_list;
};

#if 0
#define qu_mod qu_u.fmod
#define qu_dev qu_u.cdev
#endif

#define qstream(__q) (((struct queinfo *)RD(__q))->qu_str)

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

struct mbinfo {
	mblk_t m_mblock;
	void (*m_func) (void);		/* allocating function SVR4 compatible */
	queue_t *m_queue;		/* last queue for this block */
	void *m_private;		/* private information for deferral */
	struct list_head m_list;
};
struct dbinfo {
	dblk_t d_dblock;
	struct list_head db_list;
};

/* I don't know why, we never allocate these, we place them in message blocks */
struct linkinfo {
	struct linkblk li_linkblk;
	struct list_head li_list;
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
	struct list_head s_list;
};

struct qbinfo {
	struct qband qbi_qband;
	atomic_t qbi_refs;		/* references to this structure */
	struct list_head qbi_list;
};

struct apinfo {
	struct strapush api_sap;
	struct list_head api_more;
	atomic_t api_refs;
	struct list_head api_list;
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
	struct devinfo *di_next;	/* Strinfo list linkage */
	struct devinfo *di_prev;	/* Strinfo list linkage */
};

struct mdlinfo {
	struct list_head mi_list;	/* list of modules for this switch table entry */
	struct list_head mi_hash;	/* list of modid hashes in slot */
	struct fmodsw *mi_mod;		/* switch table entry */
	struct module_info *mi_info;	/* quick pointer to module info */
	atomic_t mi_refs;		/* structure references */
	atomic_t mi_count;		/* open count */
	modID_t modid;			/* module id number */
	struct mdlinfo *mi_next;	/* Strinfo list linkage */
	struct mdlinfo *mi_prev;	/* Strinfo list linkage */
};

#define was128	(32*sizeof(ulong))
#define HDRSZ	(sizeof(struct mbinfo)-sizeof(struct dbinfo))
#define FASTBUF	(was128-HDRSZ)
/* 128 - (18 + 40) - 16 = 54 bytes fastbuf on 32-bit */
/* 256 - (30 + 76) - 32 = 128 bytes fastbuf on 64-bit */
/* having a bunch more for 64-bit is a good idea because elements of M_PROTO blocks will be larger
   as well. */

struct mdbblock {
	struct mbinfo msgblk;
	struct dbinfo datablk;
	unsigned char databuf[FASTBUF];
};

/* from strsched.c */
extern bcid_t __bufcall(queue_t *q, unsigned size, int priority, void (*function) (long), long arg);
extern toid_t __timeout(queue_t *q, timo_fcn_t *timo_fcn, caddr_t arg, long ticks, unsigned long pl,
			int cpu);
#if 0
extern void mdbblock_free(mblk_t *mp);
extern mblk_t *mdbblock_alloc(uint priority, void *func);
extern void freechain(mblk_t *mp, mblk_t **mpp);
extern struct qband *allocqb(void);
extern void freeqb(qband_t *qb);
#endif

extern int setsq(queue_t *q, struct fmodsw *fmod);
extern void qscan(queue_t *q);

/* from strlookup.c */
extern struct list_head cdevsw_list;	/* Drivers go here */
extern struct list_head fmodsw_list;	/* Modules go here */
extern struct list_head cminsw_list;	/* Minors go here */
extern int cdev_count;			/* Driver count */
extern int fmod_count;			/* Module count */
extern int cmin_count;			/* Node count */
extern struct devnode *STREAMS_FASTCALL(__cmaj_lookup(major_t major));
extern struct cdevsw *STREAMS_FASTCALL(__cdev_lookup(major_t major));
extern struct cdevsw *STREAMS_FASTCALL(__cdrv_lookup(modID_t modid));
extern struct devnode *STREAMS_FASTCALL(__cmin_lookup(struct cdevsw *cdev, minor_t minor));
extern struct fmodsw *STREAMS_FASTCALL(__fmod_lookup(modID_t modid));
extern struct cdevsw *STREAMS_FASTCALL(__cdev_search(const char *name));
extern struct fmodsw *STREAMS_FASTCALL(__fmod_search(const char *name));
extern struct devnode *STREAMS_FASTCALL(__cmin_search(struct cdevsw *cdev, const char *name));
extern void *STREAMS_FASTCALL(__smod_search(const char *name));
extern struct fmodsw *STREAMS_FASTCALL(fmod_str(const struct streamtab *str));
extern struct cdevsw *STREAMS_FASTCALL(cdev_str(const struct streamtab *str));
extern struct cdevsw *STREAMS_FASTCALL(sdev_get(major_t major));
extern void STREAMS_FASTCALL(sdev_put(struct cdevsw *cdev));
extern struct cdevsw *STREAMS_FASTCALL(cdrv_get(modID_t modid));
extern void STREAMS_FASTCALL(cdrv_put(struct cdevsw *cdev));
extern struct fmodsw *STREAMS_FASTCALL(fmod_get(modID_t modid));
extern void STREAMS_FASTCALL(fmod_put(struct fmodsw *fmod));
extern struct cdevsw *STREAMS_FASTCALL(cdev_find(const char *name));
extern struct cdevsw *STREAMS_FASTCALL(cdev_match(const char *name));
extern struct fmodsw *STREAMS_FASTCALL(fmod_find(const char *name));
extern struct devnode *STREAMS_FASTCALL(cmin_find(const struct cdevsw *cdev, const char *name));
extern struct devnode *STREAMS_FASTCALL(cmin_get(const struct cdevsw *cdev, minor_t minor));
extern struct devnode *STREAMS_FASTCALL(cmaj_get(const struct cdevsw *cdev, major_t major));
extern minor_t STREAMS_FASTCALL(cdev_minor(struct cdevsw *cdev, major_t major, minor_t minor));

/* from strreg.c */
extern int register_cmajor(struct cdevsw *cdev, major_t major, struct file_operations *fops);
extern int unregister_cmajor(struct cdevsw *cdev, major_t major);

/* other internals */
extern int sdev_add(struct cdevsw *cdev, modID_t modid);
extern void sdev_del(struct cdevsw *cdev);
extern rwlock_t cdevsw_lock;
extern void cmaj_add(struct devnode *cmaj, struct cdevsw *cdev, major_t major);
extern void cmaj_del(struct devnode *cmaj, struct cdevsw *cdev);
extern int cmin_add(struct devnode *cmin, struct cdevsw *cdev, minor_t minor);
extern void cmin_del(struct devnode *cmin, struct cdevsw *cdev);

extern long do_fattach(const struct file *file, const char *file_name);
extern long do_fdetach(const char *file_name);
extern long do_spipe(int *fds);

extern void fmod_add(struct fmodsw *fmod, modID_t modid);
extern void fmod_del(struct fmodsw *fmod);
extern rwlock_t fmodsw_lock;

extern rwlock_t nodesw_lock;

extern int spec_reparent(struct file *file, struct cdevsw *cdev, dev_t dev);
extern int spec_open(struct file *file, struct cdevsw *cdev, dev_t dev, int sflag);
extern struct vfsmount *STREAMS_FASTCALL(specfs_mount(void));
extern void STREAMS_FASTCALL(specfs_umount(void));

extern struct linkblk *alloclk(void);
extern void freelk(struct linkblk *l);

extern struct stdata *allocstr(void);
extern void freestr(struct stdata *sd);
extern struct stdata *STREAMS_FASTCALL(sd_get(struct stdata *sd));
extern void STREAMS_FASTCALL(sd_put(struct stdata **sdp));

extern int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag,
		    cred_t *crp);

extern struct devinfo *di_alloc(struct cdevsw *cdev);
extern void di_put(struct devinfo *di);

extern struct strevent *STREAMS_FASTCALL(sealloc(void));
extern int STREAMS_FASTCALL(sefree(struct strevent *se));

extern int sysctl_str_nstrpush;
extern int sysctl_str_strctlsz;

extern int register_clone(struct cdevsw *cdev);
extern int unregister_clone(struct cdevsw *cdev);

extern void runqueues(void);

/* If you use this structure, you might want to upcall to the stream head functions behind these.
 * These are them.  Note that the functions above or below are called after acquiring a reference to
 * the STREAM head but before performing basic access checks.  Note also, that ioctl will be an
 * unlocked_ioctl on systems that support it.  Take your own big kernel locks if you need to. */

extern unsigned int strpoll(struct file *file, struct poll_table_struct *poll);
extern ssize_t strread(struct file *file, char *buf, size_t len, loff_t *ppos);
extern ssize_t strwrite(struct file *file, const char *buf, size_t len, loff_t *ppos);
#if 0
extern ssize_t strreadv(struct file *file, const struct iovec *iov, unsigned long len, loff_t *ppos);
extern ssize_t strwritev(struct file *file, const struct iovec *iov, unsigned long count, loff_t *ppos);
#endif
extern ssize_t strsendpage(struct file *file, struct page *page, int offset, size_t size,
			   loff_t *ppos, int more);
extern int strgetpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int *bandp,
		      int *flagsp);
extern int strputpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int band,
		      int flags);
extern int strioctl(struct file *file, unsigned int cmd, unsigned long arg);

#if 0
extern loff_t strllseek(struct file *file, loff_t off, int whence);
extern int strmmap(struct file *filp, struct vm_area_struct *vma);
extern int stropen(struct inode *inode, struct file *file);
extern int strflush(struct file *file);
extern int strclose(struct inode *inode, struct file *file);
extern int strfasync(int fd, struct file *file, int on);
#endif

/* stream head read put and write service procedures, and open/close for use by replacement stream heads */
extern int strrput(queue_t *q, mblk_t *mp);
extern int strwput(queue_t *q, mblk_t *mp);
extern int strwsrv(queue_t *q);
extern int str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp);
extern int str_close(queue_t *q, int oflag, cred_t *crp);

extern struct file_operations strm_f_ops;

#endif				/* __SYS_STREAMS_STRSUBR_H__ */
