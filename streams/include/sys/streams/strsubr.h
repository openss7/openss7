/*****************************************************************************

 @(#) $Id: strsubr.h,v 0.9.2.19 2005/03/05 13:07:49 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/03/05 13:07:49 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STRSUBR_H__
#define __SYS_STRSUBR_H__

#ident "@(#) $RCSfile: strsubr.h,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2005/03/05 13:07:49 $"

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
		struct {
			queue_t *queue;
			void (*func) (void *, mblk_t *);
			void *arg;
			int perim;
			mblk_t *mp;
		} p;			/* strput request */
	} x;
	struct strevent *se_next;
	struct strevent *se_prev;	/* actually hash list */
	int se_id;			/* identifier for this event structure */
	int se_seq;			/* use sequence number */
};

/* synchronization queue structure */
typedef struct syncq {
	spinlock_t sq_lock;		/* spin lock for this structure */
	int sq_count;			/* no of threads inside (negative for exclusive) */
	struct task_struct *sq_owner;	/* exclusive owner */
//	wait_queue_head_t sq_waitq;	/* waiters */
	struct strevent *sq_head;	/* head of event queue */
	struct strevent **sq_tail;	/* tail of event queue */
	struct syncq *sq_outer;		/* synch queue outside this one (if any) */
	unsigned int sq_flag;		/* synch queue flags */
	unsigned int sq_level;		/* synch queue level */
	struct syncq *sq_link;		/* synch queue schedule list */
	atomic_t sq_refs;		/* structure references */
	struct syncq *sq_next;		/* list of all structures */
	struct syncq *sq_prev;		/* list of all structures */
} syncq_t;

/* stream head private structure */
struct stdata {
	queue_t *sd_rq;			/* rd queue for stream head */
	queue_t *sd_wq;			/* wr queue for stream head */
	dev_t sd_dev;			/* device number of driver */
	mblk_t *sd_iocblk;		/* message to return for ioctl */
	struct stdata *sd_other;	/* other stream head for pipes */
	struct streamtab *sd_strtab;	/* driver streamtab */
	struct inode *sd_inode;         /* back pointer to inode */
//      struct dentry *sd_dentry;       /* back pointer to dentry */
	struct file *sd_file;		/* back pointer to file */
	ulong sd_flag;			/* stream head state */
	ulong sd_rdopt;			/* read options */
	ulong sd_wropt;			/* write options */
	ulong sd_eropt;			/* error options */
	ulong sd_iocid;			/* sequence id for active ioctl */
//	ushort sd_iocwait;		/* number of procs awaiting ioctl */
//	struct task_struct *sd_sidp;	/* controlling session id */
//	struct task_struct *sd_pgidp;	/* controlling process group */
	ushort sd_wroff;		/* write offset */
	int sd_rerror;			/* read error */
	int sd_werror;			/* write error */
	int sd_opens;			/* number of successful opens */
	int sd_readers;			/* number of streampipe readers */
	int sd_writers;			/* number of streampipe writers */
//	int sd_rwaiters;		/* number of waiters on read */
//	int sd_wwaiters;		/* number of waiters on write */
	int sd_pushcnt;			/* number of modules pushed */
	int sd_nanchor;			/* number of modules anchored */
	unsigned long sd_sigflags;		/* signal flags */
	struct fasync_struct *sd_siglist;	/* list of procs for SIGPOLL */
	// struct pollhead sd_polllist; /* list of poll wakeup functions */
	wait_queue_head_t sd_waitq;	/* waiters */
//	mblk_t *sd_mark;		/* pointer to marked message */
	ulong sd_closetime;		/* queue drain wait time on close */
//	ulong sd_rtime;			/* time to forward held message */
	rwlock_t sd_qlock;		/* lock for queues under this stream */
	struct task_struct *sd_owner;	/* exclusive lock owner */
	int sd_nest;			/* lock nesting */
	struct cdevsw *sd_cdevsw;	/* device entry */
	struct list_head sd_list;	/* list against device */
//	struct semaphore sd_mutex;	/* mutex for system calls */
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
	STRMSIG_BIT,
	STRDELIM_BIT,
	STRTOSTOP_BIT,
	STRISFIFO_BIT,
	STRISPIPE_BIT,
	STRISSOCK_BIT,
	STFROZEN_BIT,
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
#define STRMSIG	    (1<<STRMSIG_BIT)	/* M_SIG at head of queue */
#define STRDELIM    (1<<STRDELIM_BIT)	/* generate delimited messages */
#define STRTOSTOP   (1<<STRTOSTOP_BIT)	/* stop timeout */
#define STRISFIFO   (1<<STRISFIFO_BIT)	/* stream is a fifo */
#define STRISPIPE   (1<<STRISPIPE_BIT)	/* stream is a STREAMS pipe */
#define STRISSOCK   (1<<STRISSOCK_BIT)	/* stream is a STREAMS socket */
#define STFROZEN    (1<<STFROZEN_BIT)	/* stream is frozen */

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
	DYN_MODINFO,			/* struct modinfo */
	DYN_SYNCQ,			/* struct syncq */
	DYN_SIZE,			/* size */
};

struct strthread {
	volatile unsigned long flags;	/* flags */
	queue_t *qhead;			/* first queue in scheduled queues */
	queue_t **qtail;		/* last queue in scheduled queues */
	queue_t *currentq;		/* current queue being processed */
	syncq_t *sqhead;		/* first syncq in scheduled syncqs */
	syncq_t **sqtail;		/* last sycnq in scheduled sycnqs */
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
/* aligned so processors keep out of each other's way */

enum {
	queueflag,			/* runqueues() is running */
	qrunflag,			/* at least 1 queue to run */
	strbcflag,			/* bufcall() functions can be run */
	strbcwait,			/* bufcall() functions waiting */
	flushwork,			/* flushq has messages to free */
	freeblks,			/* fastfreed blocks for the cache */
	strtimout,			/* timeout() functions can be run */
	strevents,			/* strevents() functions can be run */
	qsyncflag,			/* at least 1 syncq to run */
};

#define QUEUEFLAG	(1 << queueflag	)
#define QRUNFLAG	(1 << qrunflag	)
#define STRBCFLAG	(1 << strbcflag	)
#define STRBCWAIT	(1 << strbcwait	)
#define FLUSHWORK	(1 << flushwork	)
#define FREEBLKS	(1 << freeblks	)
#define STRTIMOUT	(1 << strtimout )
#define STREVENTS	(1 << strevents )
#define QSYNCFLAG	(1 << qsyncflag )

struct shinfo {
	struct stdata sh_stdata;
	atomic_t sh_refs;		/* references to this structure */
	struct list_head sh_list;
};

struct queinfo {
	queue_t rq;			/* read queue */
	queue_t wq;			/* write queue */
	struct stdata *qu_str;		/* stream head for this queue pair */
#if 0
	union {
		struct fmodsw *fmod;	/* streams module */
		struct cdevsw *cdev;	/* streams driver */
	} qu_u;
	int qu_flags;			/* queue pair flags */
#endif
	atomic_t qu_refs;		/* references to this structure */
	wait_queue_head_t qu_qwait;	/* wait queue for qwait */
	rwlock_t qu_lock;		/* procs lock */
	struct task_struct *qu_owner;	/* exclusive locker */
	uint qu_nest;			/* exclusive lock nest */
	struct list_head qu_list;
};

#if 0
#define qu_mod qu_u.fmod
#define qu_dev qu_u.cdev
#endif

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
#define SE_STRPUT	5
#define SE_WRITER	6
#define SE_PUTP		7

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

struct modinfo {
	struct list_head mi_list;	/* list of modules for this switch table entry */
	struct list_head mi_hash;	/* list of modid hashes in slot */
	struct fmodsw *mi_mod;		/* switch table entry */
	struct module_info *mi_info;	/* quick pointer to module info */
	atomic_t mi_refs;		/* structure references */
	atomic_t mi_count;		/* open count */
	modID_t modid;			/* module id number */
	struct modinfo *mi_next;	/* Strinfo list linkage */
	struct modinfo *mi_prev;	/* Strinfo list linkage */
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
extern void mdbblock_free(mblk_t *mp);
extern mblk_t *mdbblock_alloc(uint priority, void *func);
extern void freechain(mblk_t *mp, mblk_t **mpp);
extern struct qband *allocqb(void);
extern void freeqb(qband_t *qb);

/* from strlookup.c */
extern struct list_head cdevsw_list;	/* Drivers go here */
extern struct list_head fmodsw_list;	/* Modules go here */
extern struct list_head cminsw_list;	/* Minors go here */
extern int cdev_count;			/* Driver count */
extern int fmod_count;			/* Module count */
extern int cmin_count;			/* Node count */
extern struct devnode *__cmaj_lookup(major_t major);
extern struct cdevsw *__cdev_lookup(major_t major);
extern struct cdevsw *__cdrv_lookup(modID_t modid);
extern struct devnode *__cmin_lookup(struct cdevsw *cdev, minor_t minor);
extern struct fmodsw *__fmod_lookup(modID_t modid);
extern struct cdevsw *__cdev_search(const char *name);
extern struct fmodsw *__fmod_search(const char *name);
extern struct devnode *__cmin_search(struct cdevsw *cdev, const char *name);
extern void *__smod_search(const char *name);
extern struct fmodsw *fmod_str(const struct streamtab *str);
extern struct cdevsw *cdev_str(const struct streamtab *str);
extern struct cdevsw *cdev_get(major_t major);
extern void cdev_put(struct cdevsw *cdev);
extern struct cdevsw *cdrv_get(modID_t modid);
extern void cdrv_put(struct cdevsw *cdev);
extern struct fmodsw *fmod_get(modID_t modid);
extern void fmod_put(struct fmodsw *fmod);
extern struct cdevsw *cdev_find(const char *name);
extern struct cdevsw *cdev_match(const char *name);
extern struct fmodsw *fmod_find(const char *name);
extern struct devnode *cmin_find(const struct cdevsw *cdev, const char *name);
extern struct devnode *cmin_get(const struct cdevsw *cdev, minor_t minor);
extern struct devnode *cmaj_get(const struct cdevsw *cdev, major_t major);
extern minor_t cdev_minor(struct cdevsw *cdev, major_t major, minor_t minor);

/* from strreg.c */
extern int register_cmajor(struct cdevsw *cdev, major_t major, struct file_operations *fops);
extern int unregister_cmajor(struct cdevsw *cdev, major_t major);


#endif				/* __SYS_STRSUBR_H__ */
