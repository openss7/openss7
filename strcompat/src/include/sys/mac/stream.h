/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.9 2007/02/21 01:09:14 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/02/21 01:09:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.9  2007/02/21 01:09:14  brian
 - updating mtp.c driver, better mi_open allocators

 Revision 0.9.2.8  2006/12/23 13:07:13  brian
 - manual page and other package updates for release

 Revision 0.9.2.7  2006/12/08 05:08:12  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.6  2006/11/03 10:39:20  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.5  2006/06/22 13:11:24  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.4  2005/12/28 09:51:48  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.3  2005/07/18 12:25:40  brian
 - standard indentation

 Revision 0.9.2.2  2005/07/15 23:09:07  brian
 - checking in for sync

 Revision 0.9.2.1  2005/07/12 13:54:43  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_MAC_STREAM_H__
#define __SYS_MAC_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/mac/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __MAC_EXTERN_INLINE
#define __MAC_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif

#ifndef _MAC_SOURCE
#warning "_MAC_SOURCE not defined but MAC stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_MAC || defined CONFIG_STREAMS_COMPAT_MAC_MODULE

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

/* These are MPS definitions exposed by MacOT, but implemented in mpscompat.c */

extern mblk_t *mi_timer_alloc_MAC(queue_t *q, size_t size);
extern void mi_timer_MAC(mblk_t *mp, clock_t msec);
extern int mi_timer_cancel(mblk_t *mp);
extern mblk_t *mi_timer_q_switch(mblk_t *mp, queue_t *q, mblk_t *new_mp);
extern int mi_timer_valid(mblk_t *mp);
extern void mi_timer_free(mblk_t *mp);
extern unsigned long mi_timer_remain(mblk_t *mp);

extern queue_t *mi_allocq(struct streamtab *st);

extern mblk_t *mi_reuse_proto(mblk_t *mp, size_t size, int keep_on_error);
extern mblk_t *mi_reallocb(mblk_t *mp, size_t size);

extern uint8_t *mi_offset_param(mblk_t *mp, size_t offset, size_t len);
extern uint8_t *mi_offset_paramc(mblk_t *mp, size_t offset, size_t len);

extern int mi_set_sth_hiwat(queue_t *q, size_t size);
extern int mi_set_sth_lowat(queue_t *q, size_t size);
extern int mi_set_sth_maxblk(queue_t *q, ssize_t size);
extern int mi_set_sth_wroff(queue_t *q, size_t size);

extern caddr_t mi_next_ptr(caddr_t ptr);
extern caddr_t mi_prev_ptr(caddr_t ptr);
extern caddr_t mi_open_detached(caddr_t *mi_list, size_t size, dev_t *devp);
extern void mi_attach(queue_t *q, caddr_t ptr);
extern int mi_open_comm(caddr_t *mi_list, size_t size, queue_t *q, dev_t *dev, int flag, int sflag,
			cred_t *credp);
extern void mi_detach(queue_t *q, caddr_t ptr);
extern void mi_close_detached(caddr_t *mi_list, caddr_t strptr);
extern int mi_close_comm(caddr_t *mi_list, queue_t *q);

extern void mi_bufcall(queue_t *q, int size, int priority);

extern void mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len);
extern void mi_copyout(queue_t *q, mblk_t *mp);
extern mblk_t *mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len, int free);
extern void mi_copy_done(queue_t *q, mblk_t *mp, int err);
extern int mi_copy_state(queue_t *q, mblk_t *mp, mblk_t **mpp);
extern void mi_copy_set_rval(mblk_t *mp, int rval);

#define mi_timer_alloc(_q,_size)	mi_timer_alloc_MAC(_q,_size)
#define mi_timer(_mp,_msec)		mi_timer_MAC(_mp,_msec)

#else
#ifdef _MAC_SOURCE
#warning "_MAC_SOURCE defined by not CONFIG_STREAMS_COMPAT_MAC"
#endif
#endif

#endif				/* __SYS_MAC_STREAM_H__ */
