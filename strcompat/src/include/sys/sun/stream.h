/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.12 2006/12/08 05:08:19 brian Exp $

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

 Last Modified $Date: 2006/12/08 05:08:19 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.12  2006/12/08 05:08:19  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.11  2006/11/03 10:39:23  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.10  2006/10/06 12:13:16  brian
 - updated manual pages to pass make check and for release
 - updated release files for release
 - added missing MacOT OSF/1 and MPS compatibility functions

 Revision 0.9.2.9  2006/07/24 09:01:00  brian
 - results of udp2 optimizations

 Revision 0.9.2.8  2006/06/22 13:11:28  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.7  2006/05/23 10:44:07  brian
 - mark normal inline functions for unlikely text section

 Revision 0.9.2.6  2005/12/28 09:51:49  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.5  2005/12/19 12:44:36  brian
 - locking down for release

 Revision 0.9.2.4  2005/07/18 12:25:41  brian
 - standard indentation

 Revision 0.9.2.3  2005/07/15 23:09:30  brian
 - checking in for sync

 Revision 0.9.2.2  2005/07/14 22:04:01  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:44  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_SUN_STREAM_H__
#define __SYS_SUN_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.12 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

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

#ifndef _SUN_SOURCE
#warning "_SUN_SOURCE not defined but SUN stream.h included."
#endif

#include <sys/strcompat/config.h>

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
#ifdef LFS
	unfreezestr(q, -1UL);
#endif
#ifdef LIS
	unfreezestr(q);
#endif
}

#undef unfreezestr
#define unfreezestr unfreezestr_SUN

#ifdef LFS
extern void qwait(queue_t *rq);
extern int qwait_sig(queue_t *rq);
#endif

#ifdef LFS
extern bufcall_id_t qbufcall(queue_t *q, size_t size, int priority, void streamscall (*function) (void *),
			     void *arg);
extern timeout_id_t qtimeout(queue_t *q, void streamscall (*timo_fcn) (void *), void *arg, long ticks);
#endif

extern void qunbufcall(queue_t *q, bufcall_id_t bcid);
extern clock_t quntimeout(queue_t *q, timeout_id_t toid);

#ifdef LFS
/* LiS already defines this */
__SUN_EXTERN_INLINE unsigned char
queclass(mblk_t *mp)
{
	return (mp->b_datap->db_type < QPCTL ? QNORM : QPCTL);
}
#endif

#ifdef LFS
__SUN_EXTERN_INLINE void qwriter(queue_t *qp, mblk_t *mp, void streamscall (*func) (queue_t *qp, mblk_t *mp), int perimeter);
#endif

#define straln (caddr_t)((intptr_t)(a) & ~(sizeof(int)-1))

__SUN_EXTERN_INLINE mblk_t *
mkiocb(unsigned int command)
{
	mblk_t *mp;
	union ioctypes *iocp;

#ifdef CONFIG_STREAMS_LIS_BCM
	/* FIXME: this goes away when we leave! */
	cred_t creds = {.cr_uid = current->euid,.cr_gid = current->egid,.cr_ruid =
		    current->uid,.cr_rgid = current->gid,
	}, *crp = &creds;
#else
	cred_t *crp = current_creds;
#endif
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
#ifdef LFS
		iocp->iocblk.ioc_flag = IOC_NATIVE;
#endif
	}
	return (mp);
}

/* These are MPS definitions exposed by OpenSolaris, but implemented in mpscompat.c */
extern mblk_t *mi_timer_alloc_SUN(size_t size);
extern void mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec);
extern void mi_timer_stop(mblk_t *mp);
extern void mi_timer_move(queue_t *q, mblk_t *mp);
extern int mi_timer_valid(mblk_t *mp);
extern void mi_timer_free(mblk_t *mp);

#define mi_timer_alloc(_size)		mi_timer_alloc_SUN(_size)
#define mi_timer(_q,_mp,_msec)		mi_timer_SUN(_q,_mp,_msec)

#else
#ifdef _SUN_SOURCE
#warning "_SUN_SOURCE defined by not CONFIG_STREAMS_COMPAT_SUN"
#endif
#endif

#endif				/* __SYS_SUN_STREAM_H__ */
