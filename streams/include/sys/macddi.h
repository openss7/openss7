/*****************************************************************************

 @(#) $Id: macddi.h,v 0.9.2.1 2005/04/21 01:54:59 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/04/21 01:54:59 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_MACDDI_H__
#define __SYS_MACDDI_H__

#ident "@(#) $RCSfile: macddi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __MAC_EXTERN_INLINE
#define __MAC_EXTERN_INLINE extern __inline__
#endif				/* __AIX_EXTERN_INLINE */

#include <sys/strconf.h>

#ifndef _MAC_SOURCE
#warning "_MAC_SOURCE not defined but macddi.h included"
#endif

#if defined(CONFIG_STREAMS_COMPAT_MAC) || defined(CONFIG_STREAMS_COMPAT_MAC_MODULE)

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

#if !defined _AIX_SOURCE
extern int mi_open_comm(caddr_t *mi_list, size_t size, queue_t *q, dev_t *dev, int flag, int sflag, cred_t *credp);
extern int mi_close_comm(caddr_t *mi_list, queue_t *q);
extern caddr_t mi_next_ptr(caddr_t strptr);
extern caddr_t mi_prev_ptr(caddr_t strptr);
__MAC_EXTERN_INLINE void mi_bufcall(queue_t *q, int size, int priority)
{
	extern bcid_t __bufcall(queue_t *q, unsigned size, int priority, void (*function) (long), long arg);
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	if (__bufcall(q, size, priority, (void (*)) (long) qenable, (long) q) == 0)
		qenable(q);
}
#endif

extern mblk_t *mi_timer_alloc(queue_t *q, size_t size);
extern void mi_timer(mblk_t *mp, unsigned long msec);
extern void mi_timer_cancel(mblk_t *mp);
extern void mi_timer_free(mblk_t *mp);
extern int mi_timer_valid(mblk_t *mp);

extern void mi_detach(queue_t *q, caddr_t ptr);
extern void mi_close_detached(caddr_t *mi_list, caddr_t strptr);
extern void mi_open_detached(caddr_t *mi_list, size_t size, dev_t *devp);

#elif defined(_MAC_SOURCE)
#warning "_MAC_SOURCE defined but not CONFIG_STREAMS_COMPAT_MAC"
#endif

#endif				/* __SYS_MACDDI_H__ */
