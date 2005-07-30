/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/07/29 22:20:10 $

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

 Last Modified $Date: 2005/07/29 22:20:10 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

/* global synchq */
extern struct syncq *global_syncq;

extern int __rmvq(queue_t *q, mblk_t *mp);
extern int __flushq(queue_t *q, int flag, mblk_t ***mppp);

/* queue band gets and puts */
extern qband_t * bget(qband_t *qb);
extern void bput(qband_t **bp);

/* queue gets and puts */
extern queue_t * qget(queue_t *q);
extern void qput(queue_t **qp);

/* kernel locks */
extern void klockinit(klock_t *kl);
extern void kwlock(klock_t *kl);
extern int kwtrylock(klock_t *kl);
extern void kwlock_wait(klock_t *kl);
extern int kwlock_wait_sig(klock_t *kl);
extern void kwunlock(klock_t *kl);
extern void krlock(klock_t *kl);
extern void krunlock(klock_t *kl);
extern void krlock_irqsave(klock_t *kl, unsigned long *flagsp);
extern void krunlock_irqrestore(klock_t *kl, unsigned long *flagsp);

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

/* for stream heads from queues */
#define hwlock(__q)			kwlock(&(qstream(__q))->sd_klock)
#define hwtrylock(__q)			kwtrylock(&(qstream(__q))->sd_klock)
#define hwlock_wait(__q)		kwlock_wait(&(qstream(__q))->sd_klock)
#define hwlock_wait_sig(__q)		kwlock_wait_sig(&(qstream(__q))->sd_klock)
#define hwunlock(__q)			kwunlock(&(qstream(__q))->sd_klock)
#define hrlock(__q)			krlock(&(qstream(__q))->sd_klock)
#define hrunlock(__q)			krunlock(&(qstream(__q))->sd_klock)

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
#if 0
#define qrlock(__q)			krlock(&(__q)->q_klock)
#define qrunlock(__q)			krunlock(&(__q)->q_klock)
#endif
#define qrlock_irqsave(__q,__f)		krlock_irqsave(&(__q)->q_klock,(__f))
#define qrunlock_irqrestore(__q,__f)	krunlock_irqrestore(&(__q)->q_klock,(__f))


#endif				/* __LOCAL_STRUTIL_H__ */
