/*****************************************************************************

 @(#) $Id: stream.h,v 1.1.2.1 2009-06-21 11:26:50 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2009-06-21 11:26:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 1.1.2.1  2009-06-21 11:26:50  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SUN_STREAM_H__
#define __SYS_SUN_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/sun/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __SUN_EXTERN_INLINE
#define __SUN_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif

#ifndef __SUN_EXTERN
#define __SUN_EXTERN extern streamscall
#endif

#ifndef _SUN_SOURCE
#warning "_SUN_SOURCE not defined but SUN stream.h included."
#endif

#include <sys/openss7/config.h>

#if defined CONFIG_STREAMS_COMPAT_SUN || defined CONFIG_STREAMS_COMPAT_SUN_MODULE

__SUN_EXTERN_INLINE void
freezestr_SUN(queue_t *q)
{
	freezestr(q);
}

#undef freezestr
#define freezestr freezestr_SUN

__SUN_EXTERN_INLINE void
unfreezestr_SUN(queue_t *q)
{
	unfreezestr(q, -1UL);
}

#undef unfreezestr
#define unfreezestr unfreezestr_SUN

__SUN_EXTERN void qwait(queue_t *rq);
__SUN_EXTERN int qwait_sig(queue_t *rq);

__SUN_EXTERN bufcall_id_t qbufcall(queue_t *q, size_t size, int priority, void streamscall (*function) (void *),
			     void *arg);
__SUN_EXTERN timeout_id_t qtimeout(queue_t *q, void streamscall (*timo_fcn) (void *), void *arg, long ticks);

__SUN_EXTERN void qunbufcall(queue_t *q, bufcall_id_t bcid);
__SUN_EXTERN clock_t quntimeout(queue_t *q, timeout_id_t toid);

/* LiS already defines this */
__SUN_EXTERN_INLINE unsigned char
queclass(mblk_t *mp)
{
	return (mp->b_datap->db_type < QPCTL ? QNORM : QPCTL);
}

__SUN_EXTERN void qwriter(queue_t *qp, mblk_t *mp, void streamscall (*func) (queue_t *qp, mblk_t *mp), int perimeter);

#define straln (caddr_t)((intptr_t)(a) & ~(sizeof(int)-1))

static __inline__ mblk_t *
mkiocb(unsigned int command)
{
	mblk_t *mp;
	union ioctypes *iocp;
	cred_t *crp = current_creds;
	static atomic_t ioc_id = ATOMIC_INIT(0);

	if ((mp = allocb(sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		mp->b_wptr += sizeof(*iocp);
		mp->b_cont = NULL;
		iocp = (typeof(iocp)) mp->b_rptr;
		iocp->iocblk.ioc_cmd = command;
		atomic_inc(&ioc_id);
		iocp->iocblk.ioc_id = atomic_read(&ioc_id);
		iocp->iocblk.ioc_cr = crp;
		iocp->iocblk.ioc_count = 0;
		iocp->iocblk.ioc_rval = 0;
		iocp->iocblk.ioc_error = 0;
		iocp->iocblk.ioc_flag = IOC_NATIVE;
	}
	return (mp);
}

/* These are MPS definitions exposed by OpenSolaris, but implemented in mpscompat.c */
__SUN_EXTERN mblk_t *mi_timer_alloc_SUN(size_t size);
__SUN_EXTERN void mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec);
__SUN_EXTERN void mi_timer_ticks(mblk_t *mp, clock_t ticks);
__SUN_EXTERN void mi_timer_stop(mblk_t *mp);
__SUN_EXTERN void mi_timer_move(queue_t *q, mblk_t *mp);
__SUN_EXTERN int mi_timer_valid(mblk_t *mp);
__SUN_EXTERN int mi_timer_requeue(mblk_t *mp);
__SUN_EXTERN void mi_timer_free(mblk_t *mp);
__SUN_EXTERN unsigned long mi_timer_remain(mblk_t *mp);
__SUN_EXTERN int mi_timer_running(mblk_t *mp);
__SUN_EXTERN int mi_timer_cond(mblk_t *mp, clock_t msec);

#define mi_timer_alloc(_size)		mi_timer_alloc_SUN(_size)
#define mi_timer(_q,_mp,_msec)		mi_timer_SUN(_q,_mp,_msec)

#else
#ifdef _SUN_SOURCE
#warning "_SUN_SOURCE defined by not CONFIG_STREAMS_COMPAT_SUN"
#endif
#endif

#endif				/* __SYS_SUN_STREAM_H__ */
