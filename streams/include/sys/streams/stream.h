/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.19 2004/06/06 09:47:42 brian Exp $

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

 Last Modified $Date: 2004/06/06 09:47:42 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAM_H__
#define __SYS_STREAM_H__ 1

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2004/06/06 09:47:42 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef HAVE_LINUX_FAST_STREAMS
#define HAVE_LINUX_FAST_STREAMS
#endif				/* HAVE_LINUX_FAST_STREAMS */

#include <linux/config.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>

#include <linux/types.h>	/* for various types */

/* FIXME: Need to put these in autoconf */

#ifndef uchar
typedef unsigned char uchar;		/* idiots! */
#define uchar uchar
#endif

#ifndef uintptr_t
typedef unsigned long uintptr_t;
#define uintptr_t uintptr_t
#endif

#ifndef intptr_t
typedef long intptr_t;
#define intptr_t intptr_t
#endif

#ifndef __streams_dev_t
typedef unsigned long __streams_dev_t;
#define __streams_dev_t __streams_dev_t
#endif

#include <asm/system.h>		/* for xchg */
#include <asm/bitops.h>		/* for set_bit */
#include <asm/fcntl.h>		/* for O_NONBLOCK, etc */
#include <linux/sched.h>	/* for sleep_on and interruptible_sleep_on */

#ifndef __GENKSYMS__
#include "sys/streams/modversions.h"
#endif

#include <sys/dki.h>		/* for cred_t */
/* caller should have already included this, but make sure */
#include <sys/stropts.h>	/* for some defines */

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#ifndef FMNAMESZ
#define FMNAMESZ    8		/* 16 on OSF, 31 on Mac, 8 on LiS, 8 on UnixWare */
#endif				/* FMNAMESZ */

#ifndef FREAD
#define FREAD (O_RDONLY+1)
#endif
#ifndef FWRITE
#define FWRITE (O_WRONLY+1)
#endif
#ifndef FNDELAY
#define FNDELAY (O_NDELAY|O_NONBLOCK)
#endif
#ifndef FEXCL
#define FEXCL O_EXCL
#endif

extern int nstrpush;

/* 
 *  strdata - qinit structure stream head read
 *  stwdata - qinit structure stream head write
 */

typedef struct free_rtn {
	void (*free_func) (caddr_t);
	caddr_t free_arg;
} frtn_t;

typedef struct datab {
	union {
		struct datab *freep;
		struct free_rtn *frtnp;
	} db_f;
#define		db_freep db_f.freep
#define		db_frtnp db_f.frtnp
	unsigned char *db_base;
	unsigned char *db_lim;
	unsigned char db_ref;		/* shadow reference count */
	unsigned char db_type;
	unsigned char db_iswhat;	/* Mac OT */
	unsigned char db_filler2;	/* Mac OT */
	uint db_size;			/* Mac OT */
	unsigned char *db_msgaddr;	/* Mac OT */
	long db_filler;			/* Mac OT */
	/* Linux Fast-STREAMS specific members */
	atomic_t db_users;		/* actual reference count */
} dblk_t;
/* 18 bytes on 32 bit, 30 on 64 bit */

#define DBLK_REFMIN	0x01	/* Solaris */

#define DB_WASDUPED	0x01	/* UnixWare */
#define DB_2PIECE	0x02	/* UnixWare */

/* 
 *  Message type compatibility:
 *      S - Solaris
 *      U - UnixWare
 *      A - AIX
 *      O - OSF/1.2
 *      H - HP-UX
 *      M - Mac OT (AIX)
 *      L - LiS
 *  Message direction:
 *      v - downwards only
 *      ^ - upwards only
 *      | - both directions
 *      - - not a message
 *      ? - unknown
 */
/* The OSF numbering mismatch of QNORM messages looks like a typo in the stream.h header files.  If 
   the OSF number were interpreted in octal instead of hex, the numbering would be identical to the 
   others! Note that the MAC OT (AIX) did not make this error. */
typedef enum msg_type {
	QNORM = 0x00,			/* - S U O A H M L */
	M_DATA = 0x00,			/* | S U O A H M L */
	M_PROTO = 0x01,			/* | S U O A H M L */
	M_BREAK = 0x08,			/* v S U O(0x10) A H M L(0x02) */
	M_PASSFP = 0x09,		/* | S U O(0x11) A H M L(0x06) */
	M_EVENT = 0x0a,			/* ? S */
	M_SIG = 0x0b,			/* ^ S U O(0x13) A H M L(0x09) */
	M_DELAY = 0x0c,			/* v S U O(0x14) A H M L(0x04) */
	M_CTL = 0x0d,			/* | S U O(0x15) A H M L(0x03) */
	M_IOCTL = 0x0e,			/* v S U O(0x16) A H M L(0x05) */
	M_SETOPTS = 0x10,		/* ^ S U O(0x20) A H M L(0x08) */
	M_RSE = 0x11,			/* | S U O(0x21) A H M L(0x07) */
	M_TRAIL = 0x12,			/* ? U */
	M_BACKWASH = 0x13,		/* v A */
	QPCTL = 0x80,			/* - S U O A H M L(0x0a) */
	M_IOCACK = 0x81,		/* ^ S U O A H M L(0x0f) */
	M_IOCNAK = 0x82,		/* ^ S U O A H M L(0x10) */
	M_PCPROTO = 0x83,		/* | S U O A H M L(0x12) */
	M_PCSIG = 0x84,			/* ^ S U O A H M L(0x14) */
	M_READ = 0x85,			/* v S U O(0x8b) A H(0x8b) M(0x8b) L(0x15) */
	M_FLUSH = 0x86,			/* | S U O A H M L(0x0d) */
	M_STOP = 0x87,			/* v S U O A H M L(0x16) */
	M_START = 0x88,			/* v S U O A H M L(0x17) */
	M_HANGUP = 0x89,		/* ^ S U O A H M L(0x0e) */
	M_ERROR = 0x8a,			/* ^ S U O A H M L(0x0c) */
	M_COPYIN = 0x8b,		/* ^ S U O(0x8d) A H(0x8d) M(0x8c) L(0x0a) */
	M_COPYOUT = 0x8c,		/* ^ S U O(0x8e) A H(0x8e) M(0x8d) L(0x0b) */
	M_IOCDATA = 0x8d,		/* v S U O(0x8f) A H(0x8f) M(0x8e) L(0x11) */
	M_PCRSE = 0x8e,			/* | S U O(0x90) A H(0x90) M(0x90) L(0x13) */
	M_STOPI = 0x8f,			/* v S U O(0x91) A H(0x91) M(0x91) L(0x19) */
	M_STARTI = 0x90,		/* v S U O(0x92) A H(0x92) M(0x92) L(0x18) */
	/* the rest of these are all over the board, only M_UNHANGUP is common, they have been
	   renumbered so that at least they don't overlap */
	M_PCCTL = 0x91,			/* | U */
	M_PCSETOPTS = 0x92,		/* ^ U */
	M_PCEVENT = 0x93,		/* ? S(0x91) */
	M_UNHANGUP = 0x94,		/* ^ S(0x92) O */
	M_NOTIFY = 0x95,		/* ^ O(0x93) H(0x93) */
	M_HPDATA = 0x96,		/* ^ H(0x8c) M(0x93) */
	M_LETSPLAY = 0x97,		/* ^ A */
	M_DONTPLAY = 0x98,		/* v A */
	M_BACKDONE = 0x99,		/* v A */
} msg_type_t;

/* 40 bytes on 32 bit, 76 on 64 bit */
typedef struct msgb {
	struct msgb *b_next;		/* next msgb on queue */
	struct msgb *b_prev;		/* prev msgb on queue */
	struct msgb *b_cont;		/* next msgb in message */
	unsigned char *b_rptr;		/* rd pointer into datab */
	unsigned char *b_wptr;		/* wr pointer into datab */
	struct datab *b_datap;		/* pointer to datab */
	unsigned char b_band;		/* band of this message */
	unsigned char b_pad1;		/* padding */
	unsigned short b_flag;		/* message flags */
	long b_pad2;			/* padding */
	/* private Linux Fast-STREAMS specific members */
	struct queue *b_queue;		/* queue for this message */
	struct qband *b_bandp;		/* band for this message */
	size_t b_size;			/* size of this message on queue */
} mblk_t;

#define MSGMARK		(1<<0)	/* last byte of message is marked */
#define MSGNOLOOP	(1<<1)	/* don't loop mesage at stream head */
#define MSGDELIM	(1<<2)	/* message is delimited */
#define MSGNOGET	(1<<3)	/* UnixWare/Solaris/Mac OT/ UXP/V */	/* getq does not return
									   message */
#define MSGATTEN	(1<<4)	/* UXP/V attention to on read side */
#define MSGMARKNEXT	(1<<4)	/* Solaris */
#define MSGLOG		(1<<4)	/* UnixWare */
#define MSGNOTMARKNEXT	(1<<5)	/* Solaris */
#define MSGCOMPRESS	(1<<8)	/* OSF: compress like messages as space allows */
#define MSGNOTIFY	(1<<9)	/* OSF: notify when message consumed */

#define NOERROR		(-1)	/* UnixWare */
#define TRANSPARENT	(-1UL)
#define MAXIOCBSZ	1024	/* Solaris */

#define STRCTLSZ	256	/* Mac OT */
#define STRMSGSZ	8192	/* Mac OT */

struct stroptions {
	uint so_flags;			/* options to set */
	short so_readopt;		/* read option */
	ushort so_wroff;		/* write offset */
	ssize_t so_minpsz;		/* minimum read packet size */
	ssize_t so_maxpsz;		/* maximum read packet size */
	size_t so_hiwat;		/* read queue hi water mark */
	size_t so_lowat;		/* read queue lo water mark */
	unsigned char so_band;		/* band for water marks */
	unsigned char so_filler[3];	/* padding */
	ushort so_erropt;		/* Solaris */
	ssize_t so_maxblk;		/* Solaris */
	ushort so_copyopt;		/* Solaris */
};

#define INFPSZ		( -1UL)	/* infinite packet size */

#define SO_ALL		( -1UL)	/* set all old options (Solaris uses true mask) */
#define SO_READOPT	(1<< 0)	/* set read option */
#define SO_WROFF	(1<< 1)	/* set write offset */
#define SO_MINPSZ	(1<< 2)	/* set min packet size */
#define SO_MAXPSZ	(1<< 3)	/* set max packet size */
#define SO_HIWAT	(1<< 4)	/* set high water mark */
#define SO_LOWAT	(1<< 5)	/* set low water mark */
#define SO_MREADON	(1<< 6)	/* set read notification ON */
#define SO_MREADOFF	(1<< 7)	/* set read notification OFF */
#define SO_NDELON	(1<< 8)	/* old TTY semantics for NDELAY reads/writes */
#define SO_NDELOFF	(1<< 9)	/* STREAMS semantics for NDELAY reads/writes */
/* Note: OSF reverses IS... and TO... */
#define SO_ISTTY	(1<<10)	/* the stream is acting as a terminal */
#define SO_ISNTTY	(1<<11)	/* the stream is not acting as a terminal */
#define SO_TOSTOP	(1<<12)	/* stop on background writes to this stream */
#define SO_TONSTOP	(1<<13)	/* do not stop on background writes to stream */
#define SO_BAND		(1<<14)	/* water marks affect band */
#define SO_DELIM	(1<<15)	/* messages are delimited */
#define SO_NODELIM	(1<<16)	/* turn off delimiters */
#define SO_STRHOLD	(1<<17)	/* UnixWare/Solaris: enable strwrite message coalescing */
#define SO_ERROPT	(1<<18)	/* Solaris */
#define SO_LOOP		(1<<18)	/* UnixWare */
#define SO_COPYOPT	(1<<19)	/* Solaris: user io copy options */
#define SO_MAXBLK	(1<<20)	/* Solaris: maximum block size */

#define DEF_IOV_MAX	16	/* Solaris */

/* Mac OT has short for qb_flag and qb_pad1, OSF has long and ulong everywhere */
typedef struct qband {
	struct qband *qb_next;		/* next (lower) priority band */
	size_t qb_count;		/* number of bytes queued */
	struct msgb *qb_first;		/* first queue message in this band */
	struct msgb *qb_last;		/* last queued message in this band */
	size_t qb_hiwat;		/* hi water mark for flow control */
	size_t qb_lowat;		/* lo water mark for flow control */
	uint qb_flag;			/* flags */
	long qb_pad1;			/* OSF: reserved */
	/* Linux fast-STREAMS specific members */
	ssize_t qb_msgs;		/* messages in band */
	struct qband *qb_prev;		/* prev (higher) priority band */
	unsigned char qb_band;		/* band */
	unsigned char __pad[sizeof(long) - 1];	/* padding */
} qband_t;

enum {
	QB_FULL_BIT,			/* band full flow control */
	QB_WANTW_BIT,			/* back enable required */
	QB_BACK_BIT,			/* UnixWare/Solaris/UXP/V */
};
#define QB_FULL	    (1 << QB_FULL_BIT	)
#define QB_WANTW    (1 << QB_WANTW_BIT	)
#define QB_BACK	    (1 << QB_BACK_BIT	)	/* UnixWare/Solaris */

#define NBAND	    256		/* UnixWare/Solaris */

/* 
 *  This queue structure corresponds to the Expanded Fundamental Type queue
 *  structure.
 */
typedef struct queue {
	struct qinit *q_qinfo;		/* info structure for the queue */
	struct msgb *q_first;		/* head of queued messages */
	struct msgb *q_last;		/* tail of queued messages */
	struct queue *q_next;		/* next queue in this stream */
	struct queue *q_link;		/* next queue for scheduling */
	void *q_ptr;			/* private data pointer */
	size_t q_count;			/* number of bytes in queue */
	uint q_flag;			/* queue state */
	ssize_t q_minpsz;		/* min packet size accepted */
	ssize_t q_maxpsz;		/* max packet size accepted */
	size_t q_hiwat;			/* hi water mark for flow control */
	size_t q_lowat;			/* lo water mark for flow control */
	struct qband *q_bandp;		/* band's flow-control information */
	unsigned char q_nband;		/* number of priority bands */
	unsigned char q_blocked;	/* number of bands flow controlled */
	unsigned char qpad1[2];		/* padding */
	struct queue *q_other;		/* LiS, OSF */
	/* Linux fast-STREAMS specific members */
	struct msgb *q_pctl;		/* tail of queued QPCTL messages */
	ssize_t q_msgs;			/* messages on queue */
	rwlock_t q_rwlock;		/* lock for this queue structure */
	unsigned long q_iflags;		/* interrupt flags for freeze */
	struct task_struct *q_owner;	/* owner of the exclusive lock */
	int q_nest;			/* lock nesting */
	int (*q_ftmsg) (mblk_t *);	/* message filter */
#if 0
	/* Solaris has these specific fields following q_bandp */
	kmutex_t q_lock;
	struct stdata *q_stream;
#endif
	struct syncq *q_syncq;
#if 0
	unsigned char q_nband;
	kcondvar_t q_wait;
	kcondvar_t q_sync;
	struct queue *q_nfsrv;
	struct queue *q_nbsrv;
	ushort q_draining;
	short q_struiot;
	uint q_syncqmsgs;
#endif
} queue_t;

#define QENAB_BIT	 0	/* H( 6) L M( 7) O( 7) S U X */
#define QWANTR_BIT	 1	/* H( 3) L M( 4) O( 4) S U X */
#define QWANTW_BIT	 2	/* H( 4) L M( 5) O( 5) S U X */
#define QFULL_BIT	 3	/* H( 2) L M O S U X */
#define QREADR_BIT	 4	/* H( 0) L M( 1) O( 1) S U X */
#define QUSE_BIT	 5	/* H L M( 6) O( 6) S U X */
#define QNOENB_BIT	 6	/* H( 1) L M( 2) O( 2) S U X */
#define QUP_BIT		 7	/* H(15) H(16) U */
#define QBACK_BIT	 8	/* H( 7) L( 7) M O S U */
#define QOLD_BIT	 9	/* H( 8) M O */
#define QHLIST_BIT	10	/* H( 9) L(130 M O S( 9) U */
#define QTOENAB_BIT	11	/* U */
#define QSYNCH_BIT	12	/* H O U X(7) */
#define QSAFE_BIT	13	/* H O */
#define QWELDED_BIT	14	/* H M(11) O */
#define QSVCBUSY_BIT	15	/* L(14) S(16) U(17) */
#define QWCLOSE_BIT	16	/* L(10) L(12) S(17) */

#if 0
/* paraphenalia */
#define QBACKTRACE_BIT	17	/* H(10) */
#define QRETRY_BIT	17	/* L( 8) */
#define QUNWELDING_BIT	17	/* M(12) */
#define QPAIR_BIT	17	/* S(11) */
#define QINTER_BIT	17	/* U( 9) */
#define QPOP_BIT	18	/* H(17) */
#define QSCAN_BIT	18	/* L( 9) */
#define QPROTECTED_BIT	18	/* M(13) */
#define QPERQ_BIT	18	/* S(12) */
#define QBOUND_BIT	18	/* U(13) */
#define QWASFULL_BIT	19	/* L(12) */
#define QEXCOPENCLOSE_BIT	19	/* M(14) */
#define QPERMOD_BIT	19	/* S(13) */
#define QDEFCNT_BIT	19	/* U(14) */
#define QWAITING_BIT	20	/* L(15) */
#define QMTSAFE_BIT	20	/* S(14) */
#define QMOVED_BIT	20	/* U(16) */
#define QMTOUTPERIM_BIT	21	/* S(15) */
#define QEND_BIT	22	/* S(18) */
#define QWANTWSYNC_BIT	23	/* S(19) */
#define QSYNCSTR_BIT	24	/* S(20) */
#define QISDRV_BIT	25	/* S(21) */
#define QHOT_BIT	26	/* S(22) */
#define QNEXTHOT_BIT	27	/* S(23) */
#define _QINSERTING_BIT	28	/* S(26) */
#define _QREMOVING_BIT	29	/* S(27) */
#endif

#define QENAB		(1<<QENAB_BIT		)	/* queue is enabled to run */
#define QWANTR		(1<<QWANTR_BIT		)	/* flow controlled forward */
#define QWANTW		(1<<QWANTW_BIT		)	/* back-enable necessary */
#define QFULL		(1<<QFULL_BIT		)	/* queue is flow controlled */
#define QREADR		(1<<QREADR_BIT		)	/* this is the read queue */
#define QUSE		(1<<QUSE_BIT		)	/* queue being allocated */
#define QNOENB		(1<<QNOENB_BIT		)	/* do not enable with putq */
#define QUP		(1<<QUP_BIT		)	/* uni-processor emulation */
#define QBACK		(1<<QBACK_BIT		)	/* the queue has been back enabled */
#define QOLD		(1<<QOLD_BIT		)	/* module supports old style open/close */
#define QHLIST		(1<<QHLIST_BIT		)	/* stream head is using queue */
#define QTOENAB		(1<<QTOENAB_BIT		)	/* to be enabled */
#define QSYNCH		(1<<QSYNCH_BIT		)	/* flag for queue sync */
#define QSAFE		(1<<QSAFE_BIT		)	/* safe callbacks needed */
#define QWELDED		(1<<QWELDED_BIT		)	/* flags for welded queues */
#define QSVCBUSY	(1<<QSVCBUSY_BIT	)	/* service procedure running */
#define QWCLOSE		(1<<QWCLOSE_BIT		)	/* q in close wait */

#if 0
/* different names for the same things */
#define QBLKING		(1<<QUP_BIT		)	/* queue can block (up emulation) */
#define QUPMODE		(1<<QUP_BIT		)	/* flag for UP emulation queues */
#define QPROCSOFF	(1<<QHLIST_BIT		)	/* putp, srvp disabled (LiS only) */
#define QPROCSON	(1<<QMLIST_BIT		)	/* procs are enabled */
#define QFREEZE		(1<<QSYNCH_BIT		)	/* queue is frozen */
#define QLOCK		(1<<QSYNCH_BIT		)	/* queue sleep w/ pre-emption locked */
#define QINSERVICE	(1<<QSVCBUSY_BIT	)	/* q being serviced */
#define QRUNNING	(1<<QSVCBUSY_BIT	)	/* q being serviced */
#define QCLOSEWT	(1<<QWCLOSE_BIT		)	/* q in close wait */
#define QCLOSING	(1<<QWCLOSE_BIT		)	/* strm hd wants to close this queue */
#endif

#if 0
/* paraphenalia */
#define QBACKTRACE	(1<<QBACKTRACE_BIT	)	/* ??? */
#define QINTER		(1<<QINTER_BIT		)	/* interruptible queue processes */
#define QPAIR		(1<<QPAIR_BIT		)	/* inner perimeter at queue pair */
#define QRETRY		(1<<QRETRY_BIT		)	/* retry timer's set */
#define QUNWELDING	(1<<QUNWELDING_BIT	)	/* unwelding */
#define QBOUND		(1<<QBOUND_BIT		)	/* ??? */
#define QPERQ		(1<<QPERQ_BIT		)	/* inner perimeter per queue */
#define QPOP		(1<<QPOP_BIT		)	/* ??? */
#define QPROTECTED	(1<<QPROTECTED_BIT	)	/* ??? */
#define QSCAN		(1<<QSCAN_BIT		)	/* * queue in the scan list */
#define QDEFCNT		(1<<QDEFCNT_BIT		)	/* ??? */
#define QEXCOPENCLOSE	(1<<QEXCOPENCLOSE_BIT	)	/* exclusive open-close */
#define QPERMOD		(1<<QPERMOD_BIT		)	/* inner perimeter per module */
#define QWASFULL	(1<<QWASFULL_BIT	)	/* QFULL was set, still above low water */
#define QMOVED		(1<<QMOVED_BIT		)	/* ??? */
#define QMTSAFE		(1<<QMTSAFE_BIT		)	/* module is MT safe */
#define QWAITING	(1<<QWAITING_BIT	)	/* qdetach is waiting for wakeup */
#define QMTOUTPERIM	(1<<QMTOUTPERIM_BIT	)	/* outer perimeter */
#define QEND		(1<<QEND_BIT		)	/* ??? - end of stream? */
#define QWANTWSYNC	(1<<QWANTWSYNC_BIT	)	/* write sync events waiting */
#define QSYNCSTR	(1<<QSYNCSTR_BIT	)	/* flag for synchornization at stream */
#define QISDRV		(1<<QISDRV_BIT		)	/* ??? - driver vs. module */
#define QHOT		(1<<QHOT_BIT		)	/* ??? */
#define QNEXTHOT	(1<<QNEXTHOT_BIT	)	/* ??? */
#define _QINSERTING	(1<<_QINSERTING_BIT	)	/* ??? */
#define _QREMOVING	(1<<_QREMOVING_BIT	)	/* ??? */
#endif

typedef ushort modID_t;

#undef  module_info
#define module_info smodule_info
struct module_info {
	ushort mi_idnum;		/* module id number */
	char *mi_idname;		/* module name */
	ssize_t mi_minpsz;		/* min packet size accepted *//* OSF/Mac OT: long */
	ssize_t mi_maxpsz;		/* max packet size accepted *//* OSF/Mac OT: long */
	size_t mi_hiwat;		/* hi water mark *//* OSF/Mac OT: ulong */
	size_t mi_lowat;		/* lo water mark *//* OSF/Mac OT: ulong */
};

typedef struct module_stat {
	long ms_pcnt;			/* calls to qi_putp() */
	long ms_scnt;			/* calls to qi_srvp() */
	long ms_ocnt;			/* calls to qi_qopen() */
	long ms_ccnt;			/* calls to qi_qclose() */
	long ms_acnt;			/* calls to qi_qadmin() */
	void *ms_xprt;			/* module private stats */
	short ms_xsize;			/* len of private stats */
	uint ms_flags;			/* bool stats -- for future use */
} module_stat_t;

typedef int (*qi_putp_t) (queue_t *, mblk_t *);
typedef int (*qi_srvp_t) (queue_t *);
typedef int (*qi_qopen_t) (queue_t *, dev_t *, int, int, cred_t *);
typedef int (*qi_qclose_t) (queue_t *, int, cred_t *);
typedef int (*qi_qadmin_t) (queue_t *);

struct qinit {
	qi_putp_t qi_putp;		/* put procedure */
	qi_srvp_t qi_srvp;		/* service procedure */
	qi_qopen_t qi_qopen;		/* each open (read q only) */
	qi_qclose_t qi_qclose;		/* last close (read q only) */
	qi_qadmin_t qi_qadmin;		/* admin procedure (not used) */
	struct module_info *qi_minfo;	/* module info */
	struct module_stat *qi_mstat;	/* module stats */
	/* additional Linux fast-STREAMS elements */
};

struct streamtab {
	struct qinit *st_rdinit;	/* rd queue init */
	struct qinit *st_wrinit;	/* wr queue init */
	struct qinit *st_muxrinit;	/* mux lower rd queue init */
	struct qinit *st_muxwinit;	/* mux lower wr queue init */
};

typedef struct streamtab streamtab_t;

#if 0				/* AIX basically does this: */
struct fmodsw {
	struct fmodsw *d_next;		/* Pointer to next module in the list */
	struct fmodsw *d_prev;		/* Pointer to prev module in the list */
	const char *d_name;		/* Name of the module */
	ushort d_flags;			/* Flags specified at configuration time */
	syncq_head_t *d_sqh;		/* Pointer to synch queue for module-level synchronization */
	struct streamtab *d_str;	/* Pointer to streamtab associated with the module */
	int d_sq_level;			/* Synch level specified at configuration time */
	atomic_t d_refcnt;		/* Number of open or pushed count */
	int d_major;			/* Major device number (for this entry) */
};
/* AIX fmodsw flags */
#define F_MODSW_OLD_OPEN    0x1	/* Supports old-style (V.3) open/close paramters */
#define F_MODSW_QSAFETY	    0x2	/* Module requires safe timeout/bufcall callbacks */
#define F_MODSW_MPSAFE	    0x4	/* Non-MP-Safe drivers need funneling */
#endif

#define MAX_STRMOD	256
#define MAX_STRDEV	256
#define MAX_APUSH	8

struct fmodsw {
	struct list_head f_list;	/* list of all structures */
	struct list_head f_hash;	/* list of module hashes in slot */
	const char *f_name;		/* module name */
	struct streamtab *f_str;	/* pointer to streamtab for module */
	uint f_flag;			/* module flags */
	uint f_modid;			/* module id */
	atomic_t f_count;		/* open count */
	int f_sqlvl;			/* q sychronization level */
	struct syncq *f_syncq;		/* synchronization queue */
	struct module *f_kmod;		/* kernel module */
};

struct cdevsw;

struct devnode {
	struct list_head n_list;	/* list of all nodes for this device */
	struct list_head n_hash;	/* list of major hashes in slot */
	const char *n_name;		/* node name */
	struct streamtab *n_str;	/* streamtab for node */
	uint n_flag;			/* node flags */
	uint n_modid;			/* node module id */
	atomic_t n_count;		/* open count */
	int n_sqlvl;			/* q sychronization level */
	struct syncq *n_syncq;		/* synchronization queue */
	struct module *n_kmod;		/* kernel module */
	/* above must match fmodsw */
	int n_major;			/* node major device number */
	struct dentry *n_dentry;	/* specfs directory entry */
	mode_t n_mode;			/* inode mode */
	/* above must match cdevsw */
	int n_minor;			/* node minor device number */
	struct cdevsw *n_dev;		/* character device */
};
#define N_MAJOR		0x01	/* major device node */

struct file_operations;

struct cdevsw {
	struct list_head d_list;	/* list of all structures */
	struct list_head d_hash;	/* list of module hashes in slot */
	const char *d_name;		/* driver name */
	struct streamtab *d_str;	/* pointer to streamtab for driver */
	uint d_flag;			/* driver flags */
	uint d_modid;			/* driver moidule id */
	atomic_t d_count;		/* open count */
	int d_sqlvl;			/* q sychronization level */
	struct syncq *d_syncq;		/* synchronization queue */
	struct module *d_kmod;		/* kernel module */
	/* above must match fmodsw */
	int d_major;			/* base major device number */
	struct dentry *d_dentry;	/* specfs directory entry */
	mode_t d_mode;			/* inode mode */
	/* above must match devnode */
	struct file_operations *d_fop;	/* file operations */
	struct list_head d_majors;	/* major device nodes for this device */
	struct list_head d_minors;	/* minor device nodes for this device */
	struct list_head d_apush;	/* autopush list */
	struct stdata *d_plinks;	/* permanent links for this device */
	struct list_head d_stlist;	/* stream head list for this device */
};

#define PERIM_INNER	1
#define PERIM_OUTER	2

#if 0
/* basic common registration definitions */

#define STR_IS_DEVICE   0x00000001	/* device */
#define STR_IS_MODULE   0x00000002	/* module */
#define STR_TYPE_MASK   (STR_IS_DEVICE|STR_IS_MODULE)
#define STR_SYSV4_OPEN  0x00000100	/* V.4 open signature/return */
#define STR_QSAFETY     0x00000200	/* Module needs safe callbacks */
#define STR_IS_SECURE   0x00010000	/* Module/device is secure */

typedef enum {
	SQLVL_DEFAULT = 0,		/* default level */
	SQLVL_GLOBAL = 1,		/* STREAMS scheduler level */
	SQLVL_ELSEWHERE = 2,		/* module group level */
	SQLVL_MODULE = 3,		/* module level (default) */
	SQLVL_QUEUEPAIR = 4,		/* queue pair level */
	SQLVL_QUEUE = 5,		/* queue level */
	SQLVL_NOP = 6,			/* no synchronization */
} sqlvl_t;

#define F_DEVSW_CLONE		(1<<0)
#define F_DEVSW_MPSAFE		(1<<1)
#define F_DEVSW_SYNCH_QUEUE	(1<<0)
#define F_DEVSW_SYNCH_QUEUE	(1<<1)
#define F_DEVSW_SYNCH_QUEUE	(1<<2)
#define F_DEVSW_SYNCH_QUEUE	(1<<3)
#define F_DEVSW_SYNCH_QUEUE	(1<<4)
#define F_DEVSW_SYNCH_QUEUE	(1<<5)
#define F_DEVSW_SYNCH_NONE	(1<<6)
#endif

// #define D_REOPEN (1<<0) /* clone can reopen */
#define D_CLONE		(1<<1)	/* clone */
#define D_SAFE		(1<<2)	/* requires safe (interrupts off) callbacks */
#define D_UP		(1<<3)	/* per-stream sync, uniprocessor emulation */
// #define D_FIFO (1<<2) /* fifo */
// #define D_PIPE (1<<3) /* pipe */
// #define D_SOCK (1<<4) /* socket */
// #define D_LIS (1<<5) /* LiS compatible */
// #define D_HEAD (1<<6) /* stream head */
// #define D_NSDEV (1<<7) /* named streams device */
/* Solaris perimeter flags */
#define D_MP		(1<<8)	/* module is MP-safe */
#define D_MTPERMOD	(1<<9)	/* inner module perimeter */
#define D_MTQPAIR	(1<<10)	/* inner qpair perimeter */
#define D_MTPERQ	(1<<11)	/* inner queue perimeter */
#define D_MTOUTPERIM	(1<<12)	/* outer module perimeter */
#define D_MTOCEXCL	(1<<13)	/* open and close exclusive at outer perimeter */
#define D_MTPUTSHARED	(1<<14)	/* put procedures shared at the innner perimeter */
#define D_64BIT		(1<<15)	/* properly handles 64-bit offsets */
#define D_NEW		(1<<16)	/* SVR4 open and close */

struct protosw {
	struct list_head p_list;
	const char *p_devname;		/* full path device name */
	int p_family;			/* protocol family */
	int p_type;			/* socket type */
	int p_protocol;			/* protocol within family */
	int p_capability;		/* required capabilities */
};

#if 0
struct linkblk {
	queue_t *l_qtop;
	queue_t *l_qbot;
	int l_index;
	long l_pad[5];
};
#endif
struct linkblk {
	struct queue *l_qtop;		/* upper write queue */
	struct queue *l_qbot;		/* lower write queue */
	int l_index;			/* multiplexor index */
	char __pad[4 * sizeof(int) + sizeof(size_t) + sizeof(mblk_t *)];
};

#if 0
typedef union {
	long l;
	ulong ul;
	caddr_t cp;
	mblk_t *mp;
} ioc_pad;
struct iocblk {
	int ioc_cmd;
	cred_t *ioc_cr;
	uint ioc_id;
	ioc_pad ioc_cnt;
	int ioc_error;
	int ioc_rval;
	long ioc_filler[4];
};
#define ioc_count       ioc_cnt.ul
#define ioc_uid         ioc_cr->cr_uid
#define ioc_gid         ioc_cr->cr_gid
#endif
struct iocblk {
	int ioc_cmd;			/* command to perform */
	cred_t *ioc_cr;			/* credentials */
	uint ioc_id;			/* id of this ioctl */
	caddr_t __pad1;
	size_t ioc_count;		/* size of the data field */
	int ioc_error;			/* ioctl error code (for errno) */
	int ioc_rval;			/* system call return value */
	uint ioc_flag;
	mblk_t *__pad2;
};
#define ioc_uid		ioc_cr->cr_uid
#define ioc_gid		ioc_cr->cr_gid

#if 0
struct copyreq {
	int cq_cmd;
	cred_t *cq_cr;
	uint cq_id;
	ioc_pad cq_ad;
	uint cq_size;
	int cq_flag;
	mblk_t *cq_private;
	long cq_filler[4];
};
#define cq_addr cq_ad.cp
#define cq_uid  cq_cr->cr_uid
#define cq_gid  cq_cr->cr_gid
#endif
struct copyreq {
	int cq_cmd;			/* command being performed */
	cred_t *cq_cr;			/* credentials */
	uint cq_id;			/* id of this ioctl */
	caddr_t cq_addr;		/* data address */
	size_t cq_size;			/* size of data */
	int __pad1;
	int __pad2;
	uint cq_flag;			/* request flags (must be zero) */
	mblk_t *cq_private;		/* private state information */
};

#define cq_uid		cq_cr->cr_uid
#define cq_gid		cq_cr->cr_gid

#define STRCANON	0x01	/* b_cont data block contains canonical format specifier */
#define RECOPY		0x02	/* perform I_STR copyin again, this time using canonical format
				   specifier */

#if 0
struct copyresp {
	int cp_cmd;
	cred_t *cp_cr;
	uint cp_id;
	ioc_pad cp_rv;
	uint cp_pad1;
	int cp_pad2;
	mblk_t *cp_private;
	long cp_filler[4];
#define cp_rval cp_rv.l
#define cp_uid  cp_cr->cr_uid
#define cp_gid  cp_cr->cr_gid
};
#else
struct copyresp {
	int cp_cmd;			/* command being performed */
	cred_t *cp_cr;			/* credentials */
	uint cp_id;			/* id of this ioctl */
	caddr_t cp_rval;		/* data address */
	size_t __pad1;			/* alignment with copyreq */
	int __pad2;
	int __pad3;
	uint cp_flag;			/* response flags */
	mblk_t *cp_private;		/* private state information */
};
#endif

#define cp_uid		cp_cr->cr_uid
#define cp_gid		cp_cr->cr_gid

union ioctypes {
	struct iocblk iocblk;
	struct copyreq copyreq;
	struct copyresp copyresp;
};

/* AIX wantio structures (not binary compatible, or source compatible either).  I cannot find
   structure descriptions for either the contents of the M_LETSPLAY message (whether the count is
   an int), nor the wantio structure (which has the read/write/select(?) points. */
struct strlp {
	int lp_count;			/* count of supporting modules */
	queue_t *lp_queue;		/* pointer to queue write pointer (for reply) */
};

struct wantio {
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
	ssize_t (*readv) (struct file *, const struct iovec *, unsigned long, loff_t *);
	ssize_t (*writev) (struct file *, const struct iovec *, unsigned long, loff_t *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	int (*getpmsg) (struct file *, struct strbuf *, struct strbuf *, int *, int *);
	int (*putpmsg) (struct file *, struct strbuf *, struct strbuf *, int, int);
};

typedef int bcid_t;
typedef int bufcall_id_t;

typedef void *weld_arg_t;
typedef void (*weld_fcn_t) (weld_arg_t);

#define ANYBAND (-1)

/* LiS has these wrong */
#define BPRI_LO		1
#define BPRI_MED	2
#define BPRI_HI		3
#define BPRI_FT		4	/* Solaris */
#define BPRI_WAITOK	255	/* OSF */

#define DRVOPEN	    0x0
#define MODOPEN	    0x1
#define CLONEOPEN   0x2
#define CONSOPEN    0x4		/* Solaris */

#define OPENFAIL    (-1)

#define STRHIGH	    5120	/* UnixWare/Solaris */
#define STRLOW	    1024	/* UnixWare/Solaris */

#define STRLOFRAC   80		/* SVR 3.2/4 compatability (unused) */
#define STRMEDFRAC  90		/* SVR 3.2/4 compatability (unused) */
#define STRTHRESH   0		/* SVR 3.2/4 compatability (unused) */

#define STRUIOT_NONE	    -1	/* Solaris */
#define STRUIOT_DONTCARE     0	/* Solaris */
#define STRUIOT_STANDARD     1	/* Solaris */
#define STRUIOT_IP	     2	/* Solaris */

#define FLUSHALL    1
#define FLUSHDATA   0

typedef enum qfields {
	QHIWAT,				/* hi water mark */
	QLOWAT,				/* lo water mark */
	QMAXPSZ,			/* max packet size */
	QMINPSZ,			/* min packet size */
	QCOUNT,				/* count */
	QFIRST,				/* first message in queue */
	QLAST,				/* last message in queue */
	QFLAG,				/* state */
} qfields_t;

extern bcid_t bufcall(unsigned size, int priority, void (*function) (long), long arg);

extern int adjmsg(mblk_t *mp, ssize_t length);
extern int appq(queue_t *q, mblk_t *mp1, mblk_t *mp2);
extern int bcanget(queue_t *q, int band);
extern int bcanput(queue_t *q, int band);
extern int bcanputnext(queue_t *q, int band);
extern int canget(queue_t *q);
extern int canput(queue_t *q);
extern int canputnext(queue_t *q);
extern int enableq(queue_t *q);
extern int insq(queue_t *q, mblk_t *emp, mblk_t *mp);
extern int pullupmsg(mblk_t *mp, ssize_t len);
extern int putbq(queue_t *q, mblk_t *mp);
extern int putq(queue_t *q, mblk_t *mp);
extern int qattach(struct stdata *sd, struct fmodsw *fmod, dev_t *devp, int oflag, int sflag,
		   cred_t *crp);
extern int qdetach(queue_t *rq, int oflag, cred_t *crp);
extern int qclose(queue_t *q, int oflag, cred_t *credp);
extern int qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *credp);
extern int qpop(struct stdata *sd, int oflag, cred_t *crp);
extern int qpush(struct stdata *sd, const char *name, dev_t *devp, int oflag, cred_t *crp);
extern int qwait_sig(queue_t *q);
extern int strqget(queue_t *q, qfields_t what, unsigned char band, long *val);
extern int strqset(queue_t *q, qfields_t what, unsigned char band, long val);
extern int weldq(queue_t *, queue_t *, queue_t *, queue_t *, weld_fcn_t, weld_arg_t, queue_t *);
extern int unweldq(queue_t *, queue_t *, queue_t *, queue_t *, weld_fcn_t, weld_arg_t, queue_t *);

extern mblk_t *allocb(size_t size, unsigned int priority);
extern mblk_t *copyb(mblk_t *mp);
extern mblk_t *copymsg(mblk_t *mp);
extern mblk_t *dupb(mblk_t *mp);
extern mblk_t *dupmsg(mblk_t *mp);
extern mblk_t *esballoc(unsigned char *base, size_t size, uint priority, frtn_t *freeinfo);
extern mblk_t *getq(queue_t *q);
extern mblk_t *msgpullup(mblk_t *mp, ssize_t len);
extern mblk_t *qallocb(queue_t *q, size_t size, unsigned int priority);

extern modID_t getmid(const char *name);
extern qi_qadmin_t getadmin(modID_t modid);
extern queue_t *allocq(void);
extern ssize_t qcountstrm(queue_t *q);

extern void flushband(queue_t *q, int band, int flag);
extern void flushq(queue_t *q, int flag);
extern void freeb(mblk_t *bp);
extern void freeq(queue_t *q);
extern void put(queue_t *q, mblk_t *mp);
extern void putnext(queue_t *q, mblk_t *mp);
extern void qbackenable(queue_t *q);
extern void qdelete(queue_t *rq);
extern void qenable(queue_t *q);
extern void qinsert(queue_t *brq, queue_t *irq);
extern void qprocsoff(queue_t *q);
extern void qprocson(queue_t *q);
extern void unbufcall(bcid_t bcid);
extern void qwait(queue_t *q);
extern void rmvq(queue_t *q, mblk_t *mp);
extern void setq(queue_t *q, struct qinit *rinit, struct qinit *winit);
extern void setqsched(void);

/* Note: Solaris has a different prototype for these: see sunddi.h */
extern unsigned long freezestr(queue_t *q);
extern void unfreezestr(queue_t *q, unsigned long pl);

__EXTERN_INLINE bcid_t esbbcall(int priority, void (*function) (long), long arg)
{
	return bufcall(0, priority, function, arg);
}

__EXTERN_INLINE int SAMESTR(queue_t *q)
{
	return ((q->q_next != NULL) && ((q->q_flag & QREADR) == (q->q_next->q_flag & QREADR)));
}

#ifndef SAMESTR
#define SAMESTR(__q) SAMESTR(__q)
#endif
__EXTERN_INLINE int canenable(queue_t *q)
{
	return (!test_bit(QNOENB_BIT, &q->q_flag));
}
__EXTERN_INLINE int datamsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);
	/* just so happens there is a gap in the QNORM messages right at M_PCPROTO */
	return (((1 << mod) &
		 ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)) | (1 << M_DELAY)))
		!= 0);
}
__EXTERN_INLINE int isdatablk(dblk_t * db)
{
	return datamsg(db->db_type);
}
__EXTERN_INLINE int isdatamsg(mblk_t *mp)
{
	return isdatablk(mp->b_datap);
}
__EXTERN_INLINE int pcmsg(unsigned char type)
{
	return ((type & QPCTL) != 0);
}
__EXTERN_INLINE int putctl(queue_t *q, int type)
{
	mblk_t *mp;
	if (!datamsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		put(q, mp);
		return (1);
	}
	return (0);
}
__EXTERN_INLINE int putctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;
	if (!datamsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param;
		put(q, mp);
		return (1);
	}
	return (0);
}
__EXTERN_INLINE int putctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;
	if (!datamsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param1;
		*mp->b_wptr++ = (unsigned char) param2;
		put(q, mp);
		return (1);
	}
	return (0);
}
__EXTERN_INLINE int putnextctl(queue_t *q, int type)
{
	mblk_t *mp;
	if (!datamsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		putnext(q, mp);
		return (1);
	}
	return (0);
}
__EXTERN_INLINE int putnextctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;
	if (!datamsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param;
		putnext(q, mp);
		return (1);
	}
	return (0);
}
__EXTERN_INLINE int putnextctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;
	if (!datamsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param1;
		*mp->b_wptr++ = (unsigned char) param2;
		putnext(q, mp);
		return (1);
	}
	return (0);
}
__EXTERN_INLINE int testb(size_t size, unsigned int priority)
{
	mblk_t *mp;
	(void) priority;
	if ((mp = allocb(size, priority)))
		freeb(mp);
	return (mp != NULL);
}

__EXTERN_INLINE void linkb(mblk_t *mp1, mblk_t *mp2)
{
	mblk_t **mpp;
	for (mpp = &mp1; *mpp; mpp = &(*mpp)->b_cont) ;
	*mpp = mp2;
}
__EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2)
{
	if (mp1) {
		linkb(mp1, mp2);
		return (mp1);
	}
	return (mp2);
}
__EXTERN_INLINE mblk_t *rmvb(mblk_t *mp, mblk_t *bp)
{
	mblk_t **mpp;
	if (bp) {
		for (mpp = &mp; *mpp && *mpp != bp; mpp = &(*mpp)->b_cont) ;
		if (!*mpp)
			goto error;
		*mpp = bp->b_cont;
		bp->b_cont = NULL;
	}
	return (mp);
      error:
	return ((mblk_t *) (-1));
}
__EXTERN_INLINE mblk_t *unlinkb(mblk_t *mp)
{
	return (mp ? xchg(&mp->b_cont, NULL) : NULL);
}

__EXTERN_INLINE queue_t *OTHERQ(queue_t *q)
{
	return ((q->q_flag & QREADR) ? q + 1 : q - 1);
}

#ifndef OTHERQ
#define OTHERQ(__q) OTHERQ(__q)
#endif
__EXTERN_INLINE queue_t *RD(queue_t *q)
{
	return (q->q_flag & QREADR) ? q : q - 1;
}

#ifndef RD
#define RD(__q) RD(__q)
#endif
__EXTERN_INLINE queue_t *WR(queue_t *q)
{
	return ((q->q_flag & QREADR) ? q + 1 : q);
}

#ifndef WR
#define WR(__q) WR(__q)
#endif
__EXTERN_INLINE queue_t *backq(queue_t *q)
{
	struct queue *bq;
	return ((bq = OTHERQ(q)->q_next) ? OTHERQ(bq) : NULL);
}

__EXTERN_INLINE size_t msgdsize(mblk_t *mp)
{
	size_t size = 0;
	for (; mp; mp = mp->b_cont)
		if (mp->b_datap->db_type == M_DATA)
			if (mp->b_wptr > mp->b_rptr)
				size += mp->b_wptr - mp->b_rptr;
	return (size);
}
__EXTERN_INLINE size_t msgsize(mblk_t *mp)
{
	size_t size = 0;
	for (; mp; mp = mp->b_cont)
		if (mp->b_wptr > mp->b_rptr)
			size += mp->b_wptr - mp->b_rptr;
	return (size);
}
__EXTERN_INLINE size_t xmsgsize(mblk_t *mp)
{
	register mblk_t *bp = mp;
	register int type = 0;
	register size_t size = 0, blen;	/* find first non-zero length block for type */
	for (; bp; bp = bp->b_cont)
		if ((blen = bp->b_wptr - bp->b_rptr) > 0) {
			type = bp->b_datap->db_type;
			size += blen;
			break;
		}		/* finish counting rest of message */
	for (; bp; bp = bp->b_cont)
		if ((blen = bp->b_wptr - bp->b_rptr) > 0) {
			if (bp->b_datap->db_type == type)
				size += blen;
			else
				break;
		}
	return (size);
}

__EXTERN_INLINE ssize_t qsize(queue_t *q)
{
	return q->q_msgs;
}

__EXTERN_INLINE void enableok(queue_t *q)
{
	clear_bit(QNOENB_BIT, &q->q_flag);
}
__EXTERN_INLINE void freemsg(mblk_t *mp)
{
	mblk_t *mp_this;
	while ((mp_this = mp)) {
		mp = mp->b_cont;
		freeb(mp_this);
	}
}
__EXTERN_INLINE void noenable(queue_t *q)
{
	set_bit(QNOENB_BIT, &q->q_flag);
}
__EXTERN_INLINE void qreply(queue_t *q, mblk_t *mp)
{
	return putnext(OTHERQ(q), mp);
}

#define straln(a)   (caddr_t)((intptr_t)(a) & ~(sizeof(int)-1))

#endif				/* __SYS_STREAM_H__ */
