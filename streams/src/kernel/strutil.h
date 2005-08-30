/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.27 $) $Date: 2005/08/29 20:28:52 $

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

 Last Modified $Date: 2005/08/29 20:28:52 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

/* global synchq */
extern struct syncq *global_syncq;

extern int __rmvq(queue_t *q, mblk_t *mp);
extern int __flushq(queue_t *q, int flag, mblk_t ***mppp);

/* queue band gets and puts */
extern qband_t *FASTCALL(bget(qband_t *qb));
extern void FASTCALL(bput(qband_t **bp));

/* queue gets and puts */
extern queue_t *FASTCALL(qget(queue_t *q));
extern void FASTCALL(qput(queue_t **qp));

#if 0
/* kernel locks */
extern void FASTCALL(klockinit(klock_t *kl));
extern void FASTCALL(kwlock(klock_t *kl));
extern int FASTCALL(kwtrylock(klock_t *kl));
extern void FASTCALL(kwlock_wait(klock_t *kl));
extern int FASTCALL(kwlock_wait_sig(klock_t *kl));
extern void FASTCALL(kwunlock(klock_t *kl));
extern void FASTCALL(krlock(klock_t *kl));
extern void FASTCALL(krunlock(klock_t *kl));
extern void FASTCALL(krlock_irqsave(klock_t *kl, unsigned long *flagsp));
extern void FASTCALL(krunlock_irqrestore(klock_t *kl, unsigned long *flagsp));

/* for stream heads */
#define slockinit(__sd)			klockinit(&(__sd)->sd_klock)
#define swlock(__sd)			kwlock(&(__sd)->sd_klock)
#define swlock(__sd)			kwlock(&(__sd)->sd_klock)
#define swtrylock(__sd)			kwtrylock(&(__sd)->sd_klock)
#define swlock_wait(__sd)		kwlock_wait(&(__sd)->sd_klock)
#define swlock_wait_sig(__sd)		kwlock_wait_sig(&(__sd)->sd_klock)
#define swunlock(__sd)			kwunlock(&(__sd)->sd_klock)
#define srlock(__sd)			krlock(&(__sd)->sd_klock)
#define srunlock(__sd)			krunlock(&(__sd)->sd_klock)

#if 0
/* for stream heads from queues */
#define hwlock(__q)			kwlock(&(qstream(__q))->sd_klock)
#define hwtrylock(__q)			kwtrylock(&(qstream(__q))->sd_klock)
#define hwlock_wait(__q)		kwlock_wait(&(qstream(__q))->sd_klock)
#define hwlock_wait_sig(__q)		kwlock_wait_sig(&(qstream(__q))->sd_klock)
#define hwunlock(__q)			kwunlock(&(qstream(__q))->sd_klock)
#define hrlock(__q)			krlock(&(qstream(__q))->sd_klock)
#define hrunlock(__q)			krunlock(&(qstream(__q))->sd_klock)
#endif

#if 0
/* for queue pairs */
#define qplockinit(__rq)		klockinit(&((struct queinfo*)(__rq))->qu_klock)
#define qpwlock(__rq)			kwlock(&((struct queinfo*)(__rq))->qu_klock)
#define qpwtrylock(__rq)		kwtrylock(&((struct queinfo*)(__rq))->qu_klock)
#define qpwlock_wait(__rq)		kwlock_wait(&((struct queinfo*)(__rq))->qu_klock)
#define qpwlock_wait_sig(__rq)		kwlock_wait_sig(&((struct queinfo*)(__rq))->qu_klock)
#define qpwunlock(__rq)			kwunlock(&((struct queinfo*)(__rq))->qu_klock)
#define qprlock(__rq)			krlock(&((struct queinfo*)(__rq))->qu_klock)
#define qprunlock(__rq)			krunlock(&((struct queinfo*)(__rq))->qu_klock)
#endif

/* for queues */
#define qlockinit(__q)			klockinit(&(__q)->q_klock)
#define qwlock(__q)			kwlock(&(__q)->q_klock)
#define qwtrylock(__q)			kwtrylock(&(__q)->q_klock)
#define qwlock_wait(__q)		kwlock_wait(&(__q)->q_klock)
#define qwlock_wait_sig(__q)		kwlock_wait_sig(&(__q)->q_klock)
#define qwunlock(__q)			kwunlock(&(__q)->q_klock)
#define qrlock(__q)			krlock(&(__q)->q_klock)
#define qrunlock(__q)			krunlock(&(__q)->q_klock)
#if 0
#define qrlock_irqsave(__q,__f)		krlock_irqsave(&(__q)->q_klock,(__f))
#define qrunlock_irqrestore(__q,__f)	krunlock_irqrestore(&(__q)->q_klock,(__f))
#endif

#else

/*
 *  Simple locks, no nesting (not required).  Write locks suppress bottom halves and suppresses the
 *  STREAMS softirq, read locks only suppress the STREAMS softirq.  This allows read locks to be
 *  taken in all contexts except in_irq(), and write locks to be taken in all contexts except
 *  in_interrupt() (but allowed when in STREAMS softirq).
 *
 *  This means that it is inherently unsafe to call any queue handling functions from in_irq().
 *  Don't do that.  There are a couple of exceptions: putq() can be called from in_irq() but it will
 *  defer putting the message to the queue until STREAMS next executes (at bottom half).  Any
 *  failure to put the message (failure to allocate a queue band) will cause it to be discarded.
 *  put() can be called from in_irq(), but it will defer calling the put() procedure until STREAMS
 *  next executes (at bottom half).
 *
 *  It is safe to call queue handling utilities (and a host of other queue specific utilities) from
 *  bottom half, atomic or process context (!in_streams() && !in_irq)).  None of the xxxnext
 *  functions can be called from this context.  To call xxxnext utilities, first enter STREAMS
 *  context by taking a STREAM head read lock (this is what the STREAM head itself does).
 *
 *  It is safe to call all queue handling functions from within a queue open, close, put or service
 *  procedure.  Callbacks (bufcall, esballoc, timeout) that were invoked from within a queue open,
 *  close, put or service procedure are also safe.  bufcall(), timeout() callbacks invoked from
 *  outside of one of these queue procedures or queue syncrhonized callbacks, will run within the
 *  STREAMS environment, but will lack the STREAM head read locks necessary to use the xxxnext
 *  functions.
 *
 *  The same is true when transferring across a multiplexing driver.  When an upper or lower queue
 *  procedure executes queue handling functions on the queue pair opposite the side of the
 *  procedure, the xxxnext utilities cannot be used.  The same situation is true across the twist in
 *  a FIFO or pipe, pseudo-terminal, or any other pair of welded queues.  In some of these
 *  circumstances it is possible to take a STREAM head lock with the more portable MPSTR_STPLOCK()
 *  functions compatible with UnixWare.  (Basically a swlock().)
 *
 *  Also, getq() and putbq() must only be used from with a queue service procedure.
 */
#ifdef CONFIG_STREAMS_DEBUG
/* loosen locks for debugging */
#define write_lock_str(__l)	do { local_str_disable(); write_lock(__l);  } while (0)
#define write_unlock_str(__l)	do { write_unlock(__l); local_str_enable(); } while (0)
#else
#define write_lock_str(__l)	do { local_bh_disable(); local_str_disable(); write_lock(__l); } while (0)
#define write_unlock_str(__l)	do { write_unlock(__l); local_str_enable(); local_bh_enable(); } while (0)
#endif
#define read_lock_str(__l)	do { local_str_disable();  read_lock(__l); } while (0)
#define read_unlock_str(__l)	do { read_unlock(__l); local_str_enable(); } while (0)

#define slockinit(__sd)	rwlock_init(&(__sd)->sd_lock)
#define swlock(__sd)	write_lock_str(&(__sd)->sd_lock)
#define swunlock(__sd)	write_unlock_str(&(__sd)->sd_lock)
#define srlock(__sd)	read_lock_str(&(__sd)->sd_lock)
#define srunlock(__sd)	read_unlock_str(&(__sd)->sd_lock)

#define qlockinit(__q)	rwlock_init(&(__q)->q_lock)
#define qwlock(__q)	write_lock_str(&(__q)->q_lock)
#define qwunlock(__q)	write_unlock_str(&(__q)->q_lock)
#define qrlock(__q)	read_lock_str(&(__q)->q_lock)
#define qrunlock(__q)	read_unlock_str(&(__q)->q_lock)

#ifdef CONFIG_STREAMS_DEBUG
/* loosen locks for debugging */
#define qisunlocked(__q) \
(int)({ \
	int result; \
 \
	local_str_disable(); \
	if ((result = write_trylock(&(__q)->q_lock))) \
		write_unlock(&(__q)->q_lock); \
	local_str_enable(); \
	result; \
})
#else
#define qisunlocked(__q) \
(int)({ \
	int result; \
 \
	local_bh_disable(); \
	local_str_disable(); \
	if ((result = write_trylock(&(__q)->q_lock))) \
		write_unlock(&(__q)->q_lock); \
	local_str_enable(); \
	local_bh_enable(); \
	result; \
})
#endif
	


#endif


#endif				/* __LOCAL_STRUTIL_H__ */
