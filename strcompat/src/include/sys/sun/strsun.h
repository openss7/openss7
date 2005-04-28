/*****************************************************************************

 @(#) $Id: strsun.h,v 0.9.2.1 2005/04/28 11:37:41 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/04/28 11:37:41 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_STRSUN_H
#define _SYS_STRSUN_H

#ident "@(#) $RCSfile: strsun.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/04/28 11:37:41 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#include <linux/types.h>

__EXTERN_INLINE unsigned char *DB_BASE(mblk_t *mp)
{
	return (unsigned char *) (mp->b_datap->db_base);
}
__EXTERN_INLINE unsigned char *DB_LIM(mblk_t *mp)
{
	return (unsigned char *) (mp->b_datap->db_lim);
}
__EXTERN_INLINE size_t DB_REF(mblk_t *mp)
{
	return (size_t) (mp->b_datap->db_ref);
}
__EXTERN_INLINE int DB_TYPE(mblk_t *mp)
{
	return (int) (mp->b_datap->db_type);
}

__EXTERN_INLINE long MBLKL(mblk_t *mp)
{
	return (long) (mp->b_wptr - mp->b_rptr);
}
__EXTERN_INLINE long MBLKSIZE(mblk_t *mp)
{
	return (long) (mp->b_datap->db_lim - mp->b_datap->db_base);
}
__EXTERN_INLINE long MBLKHEAD(mblk_t *mp)
{
	return (long) (mp->b_rptr - mp->b_datap->db_base);
}
__EXTERN_INLINE long MBLKTAIL(mblk_t *mp)
{
	return (long) (mp->b_datap->db_lim - mp->b_wptr);
}
__EXTERN_INLINE long MBLKIN(mblk_t *mp, ssize_t off, size_t len)
{
	return ((off >= 0) && (mp->b_rptr + off + len < mp->b_wptr));
}
__EXTERN_INLINE long OFFSET(void *p, void *base)
{
	return (long) ((caddr_t) p - (caddr_t) base);
}

__EXTERN_INLINE void merror(queue_t *q, mblk_t *mp, int error)
{
	mp->b_datap->db_type = M_ERROR;
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	mp->b_wptr += 2;
	mblk_t *mp;
	mp->b_rptr[0] = mp->b_rptr[1] = error;
	qreply(q, mp);
}
__EXTERN_INLINE void miocack(queue_t *q, mblk_t *mp, int rval, int error)
{
	union ioctypes *iocp;
	mp->b_datap->db_type = M_IOCACK;
	iocp = (typeof(iocp)) mp->b_rptr;
	iocp->iocblk.ioc_count = mp->b_cont ? msgsize(mp->b_cont) : 0;
	iocp->iocblk.ioc_rval = rval;
	iocp->iocblk.ioc_error = error;
	qreply(q, mp);
}
__EXTERN_INLINE void miocnak(queue_t *q, mblk_t *mp, int rval, int error)
{
	union ioctypes *iocp;
	mp->b_datap->db_type = M_IOCNAK;
	iocp = (typeof(iocp)) mp->b_rptr;
	iocp->iocblk.ioc_count = mp->b_cont ? msgsize(mp->b_cont) : 0;
	iocp->iocblk.ioc_rval = rval;
	iocp->iocblk.ioc_error = error;
	qreply(q, mp);
}
__EXTERN_INLINE mblk_t *mexchange(queue_t *q, mblk_t *mp, size_t s1, int i1, t_scalar_t t1)
{
	/* I don't really know what this does... */
	return (NULL);
}
__EXTERN_INLINE mblk_t *mexpanddb(mblk_t *mp, int i1, int i2)
{
	/* I don't really know what this does... */
	return (NULL);
}
__EXTERN_INLINE int miocpullup(mblk_t *mp, size_t len)
{
	return (mp->b_cont ? pullupmsg(mp->b_cont, len) : 0);
}
__EXTERN_INLINE size_t msgsize(mblk_t *mp)
{
	mblk_t *bp;
	size_t s;
	for (bp = mp, s = 0; bp; bp = bp->b_cont) {
		if (bp->b_wptr > bp->b_rptr) {
			s += bp->b_wptr - bp->b_rptr;
		}
	}
	return s;
}
__EXTERN_INLINE void mcopymsg(mblk_t *mp, unsigned char *buf)
{
	mblk_t *bp;
	unsigned char *ptr;
	for (bp = mp, ptr = buf; bp; bp = bp->b_cont) {
		if (bp->b_wptr > bp->b_rptr) {
			long len = bp->b_wptr - bp->b_rptr;
			bcopy(bp->b_rptr, ptr, len);
			ptr += len;
		}
	}
	return;
}

#endif				/* _SYS_STRSUN_H */
