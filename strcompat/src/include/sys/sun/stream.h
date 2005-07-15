/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.2 2005/07/14 22:04:01 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2005/07/14 22:04:01 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.2  2005/07/14 22:04:01  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:44  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_SUN_STREAM_H__
#define __SYS_SUN_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/sun/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __SUN_EXTERN_INLINE
#define __SUN_EXTERN_INLINE extern __inline__
#endif

#ifndef _SUN_SOURCE
#warning "_SUN_SOURCE not defined but SUN stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_SUN || defined CONFIG_STREAMS_COMPAT_SUN_MODULE

__SUN_EXTERN_INLINE void freezestr_SUN(queue_t *q)
{
	freezestr(q);
}

#undef freezestr
#define freezestr freezestr_SUN

__SUN_EXTERN_INLINE void unfreezestr_SUN(queue_t *q)
{
#if LFS
	unfreezestr(q, -1UL);
#endif
#if LIS
	unfreezestr(q);
#endif
}

#undef unfreezestr
#define unfreezestr unfreezestr_SUN

#if LFS
extern void qwait(queue_t *rq);
extern int qwait_sig(queue_t *rq);
#endif

#if LFS
extern bufcall_id_t qbufcall(queue_t *q, size_t size, int priority, void (*function) (void *), void *arg);
extern timeout_id_t qtimeout(queue_t *q, void (*timo_fcn) (void *), void *arg, long ticks);
#endif

extern void qunbufcall(queue_t *q, bufcall_id_t bcid);
extern clock_t quntimeout(queue_t *q, timeout_id_t toid);

#if LFS
/* LiS already defines this */
__SUN_EXTERN_INLINE unsigned char queclass(mblk_t *mp)
{
	return (mp->b_datap->db_type < QPCTL ? QNORM : QPCTL);
}
#endif

#if LFS
extern void qwriter(queue_t *qp, mblk_t *mp, void (*func) (queue_t *qp, mblk_t *mp), int perimeter);
#endif

#define straln (caddr_t)((intptr_t)(a) & ~(sizeof(int)-1))

#elif defined _SUN_SOURCE
#warning "_SUN_SOURCE defined by not CONFIG_STREAMS_COMPAT_SUN"
#endif

#endif				/* __SYS_SUN_STREAM_H__ */
