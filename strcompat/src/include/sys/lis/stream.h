/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.13 2007/08/12 15:51:06 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/12 15:51:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.13  2007/08/12 15:51:06  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.12  2006/12/08 05:08:11  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.11  2006/11/03 10:39:19  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.10  2006/07/24 09:00:49  brian
 - results of udp2 optimizations

 Revision 0.9.2.9  2006/06/22 13:11:23  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.8  2006/05/23 10:44:03  brian
 - mark normal inline functions for unlikely text section

 Revision 0.9.2.7  2005/12/28 09:51:47  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.6  2005/12/19 12:44:25  brian
 - locking down for release

 Revision 0.9.2.5  2005/12/19 03:26:00  brian
 - wend for simple streamscall

 Revision 0.9.2.4  2005/07/18 12:25:40  brian
 - standard indentation

 Revision 0.9.2.3  2005/07/15 23:08:55  brian
 - checking in for sync

 Revision 0.9.2.2  2005/07/14 22:03:51  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:42  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_LIS_STREAM_H__
#define __SYS_LIS_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.13 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/lis/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __depr
#if __GNUC__ >= 3
#define __depr __attribute__ ((__deprecated__))
#else
#define __depr
#endif
#endif

#undef _RP
#define _RP streamscall __depr

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __LIS_EXTERN_INLINE
#define __LIS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif

#ifndef __LIS_EXTERN
#define __LIS_EXTERN extern streamscall
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _LIS_SOURCE
#warning "_LIS_SOURCE not defined but LIS stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_LIS || defined CONFIG_STREAMS_COMPAT_LIS_MODULE

__LIS_EXTERN_INLINE _RP int
lis_adjmsg(mblk_t *mp, int length)
{
	return adjmsg(mp, length);
}

__LIS_EXTERN_INLINE _RP struct msgb *
lis_allocb(int size, unsigned int priority, char *file_name, int line_nr)
{
	return allocb(size, priority);
}

__LIS_EXTERN_INLINE _RP struct msgb *
lis_allocb_physreq(int size, unsigned int priority, void *physreq_ptr, char *file_name, int line_nr)
{
	return allocb(size, priority);
}

__LIS_EXTERN_INLINE _RP queue_t *
lis_allocq(const char *name)
{
	return allocq();
}

__LIS_EXTERN_INLINE _RP int
lis_appq(queue_t *q, mblk_t *mp1, mblk_t *mp2)
{
	return appq(q, mp1, mp2);
}

__LIS_EXTERN_INLINE _RP queue_t *
lis_backq(queue_t *q)
{
	return backq(q);
}

__LIS_EXTERN_INLINE _RP queue_t *
lis_backq_fcn(queue_t *q, char *f, int l)
{
	return backq(q);
}

__LIS_EXTERN_INLINE _RP int
lis_bcanput(queue_t *q, unsigned char band)
{
	return bcanput(q, band);
}

__LIS_EXTERN_INLINE _RP int
lis_bcanputnext(queue_t *q, unsigned char band)
{
	return bcanputnext(q, band);
}

__LIS_EXTERN_INLINE _RP int
lis_bcanputnext_anyband(queue_t *q)
{
	return bcanputnext(q, ANYBAND);
}

__LIS_EXTERN_INLINE _RP int
lis_bufcall(unsigned size, int priority, void streamscall (*function) (long), long arg)
{
	return bufcall(size, priority, function, arg);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_copyb(mblk_t *mp)
{
	return copyb(mp);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_copymsg(mblk_t *mp)
{
	return copymsg(mp);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_dupb(mblk_t *mp)
{
	return dupb(mp);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_dupmsg(mblk_t *mp)
{
	return dupmsg(mp);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_esballoc(unsigned char *base, int size, int priority,
	     frtn_t *freeinfo, char *file_name, int line_nr)
{
	return esballoc(base, size, priority, freeinfo);
}

__LIS_EXTERN_INLINE _RP int
lis_esbbcall(int priority, void streamscall (*function) (long), long arg)
{
	return esbbcall(priority, function, arg);
}

__LIS_EXTERN_INLINE _RP void
lis_flushband(queue_t *q, unsigned char band, int flag)
{
	return flushband(q, band, flag);
}

__LIS_EXTERN_INLINE _RP void
lis_flushq(queue_t *q, int flag)
{
	return flushq(q, flag);
}

__LIS_EXTERN_INLINE _RP void
lis_freeb(mblk_t *bp)
{
	return freeb(bp);
}

__LIS_EXTERN_INLINE _RP void
lis_freemsg(mblk_t *mp)
{
	return freemsg(mp);
}

__LIS_EXTERN_INLINE _RP void
lis_freezestr(queue_t *q)
{
	return (void) freezestr(q);
}

__LIS_EXTERN_INLINE _RP void
lis_freeq(queue_t *q)
{
	return freeq(q);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_getq(queue_t *q)
{
	return getq(q);
}

__LIS_EXTERN_INLINE _RP int
lis_insq(queue_t *q, mblk_t *emp, mblk_t *mp)
{
	return insq(q, emp, mp);
}

__LIS_EXTERN_INLINE _RP void
lis_linkb(mblk_t *mp1, mblk_t *mp2)
{
	return linkb(mp1, mp2);
}

__LIS_EXTERN_INLINE _RP int
lis_max(int a, int b)
{
	return max(a, b);
}

__LIS_EXTERN_INLINE _RP int
lis_min(int a, int b)
{
	return min(a, b);
}

__LIS_EXTERN_INLINE _RP int
lis_msgdsize(mblk_t *mp)
{
	return msgdsize(mp);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_msgpullup(mblk_t *mp, int len)
{
	return msgpullup(mp, len);
}

__LIS_EXTERN_INLINE _RP int
lis_msgsize(mblk_t *mp)
{
	return msgsize(mp);
}

__LIS_EXTERN_INLINE _RP int
lis_pullupmsg(mblk_t *mp, int length)
{
	return pullupmsg(mp, length);
}

__LIS_EXTERN_INLINE _RP int
lis_putbq(queue_t *q, mblk_t *mp)
{
	return putbq(q, mp);
}

__LIS_EXTERN_INLINE _RP int
lis_putctl(queue_t *q, int type, char *file_name, int line_nr)
{
	return putctl(q, type);
}

__LIS_EXTERN_INLINE _RP int
lis_putctl1(queue_t *q, int type, int param, char *file_name, int line_nr)
{
	return putctl1(q, type, param);
}

__LIS_EXTERN_INLINE _RP int
lis_putnextctl(queue_t *q, int type, char *file_name, int line_nr)
{
	return putnextctl(q, type);
}

__LIS_EXTERN_INLINE _RP int
lis_putnextctl1(queue_t *q, int type, int param, char *file_name, int line_nr)
{
	return putnextctl1(q, type, param);
}

__LIS_EXTERN_INLINE _RP int
lis_putq(queue_t *q, mblk_t *mp)
{
	return putq(q, mp);
}

__LIS_EXTERN_INLINE _RP int
lis_qcountstrm(queue_t *q)
{
	return qcountstrm(q);
}

__LIS_EXTERN_INLINE _RP void
lis_qdetach(queue_t *q, int do_close, int flag, cred_t *creds)
{
	return (void) qdetach(q, flag, creds);
}

__LIS_EXTERN_INLINE _RP void
lis_qenable(queue_t *q)
{
	return qenable(q);
}

__LIS_EXTERN_INLINE _RP void
lis_qprocsoff(queue_t *rdq)
{
	return qprocsoff(rdq);
}

__LIS_EXTERN_INLINE _RP void
lis_qprocson(queue_t *rdq)
{
	return qprocson(rdq);
}

__LIS_EXTERN_INLINE _RP int
lis_qsize(queue_t *q)
{
	return qsize(q);
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_rmvb(mblk_t *mp, mblk_t *bp)
{
	return rmvb(mp, bp);
}

__LIS_EXTERN_INLINE _RP void
lis_rmvq(queue_t *q, mblk_t *mp)
{
	return rmvq(q, mp);
}

__LIS_EXTERN_INLINE _RP queue_t *
lis_safe_OTHERQ(queue_t *q, char *f, int l)
{
	return OTHERQ(q);
}

__LIS_EXTERN_INLINE _RP queue_t *
lis_safe_RD(queue_t *q, char *f, int l)
{
	return RD(q);
}

__LIS_EXTERN_INLINE _RP int
lis_safe_SAMESTR(queue_t *q, char *f, int l)
{
	return SAMESTR(q);
}

__LIS_EXTERN_INLINE _RP queue_t *
lis_safe_WR(queue_t *q, char *f, int l)
{
	return WR(q);
}

__LIS_EXTERN_INLINE _RP int
lis_safe_canenable(queue_t *q, char *f, int l)
{
	return canenable(q);
}

__LIS_EXTERN_INLINE _RP void
lis_safe_enableok(queue_t *q, char *f, int l)
{
	return enableok(q);
}

__LIS_EXTERN_INLINE _RP void
lis_safe_noenable(queue_t *q, char *f, int l)
{
	return noenable(q);
}

__LIS_EXTERN_INLINE _RP void
lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l)
{
	return put(q, mp);
}

__LIS_EXTERN_INLINE _RP void
lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l)
{
	return putnext(q, mp);
}

__LIS_EXTERN_INLINE _RP void
lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l)
{
	return qreply(q, mp);
}

__LIS_EXTERN_INLINE _RP void
lis_setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
{
	return setq(q, rinit, winit);
}

__LIS_EXTERN_INLINE _RP int
lis_strqget(queue_t *q, qfields_t what, unsigned char band, long *val)
{
	return strqget(q, what, band, val);
}

__LIS_EXTERN_INLINE _RP int
lis_strqset(queue_t *q, qfields_t what, unsigned char band, long val)
{
	return strqset(q, what, band, val);
}

__LIS_EXTERN_INLINE _RP int
lis_testb(int size, unsigned int priority)
{
	return testb(size, priority);
}

__LIS_EXTERN_INLINE _RP toid_t
lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, char *file_name, int line_nr)
{
	return timeout(timo_fcn, arg, ticks);
}

__LIS_EXTERN_INLINE _RP void
lis_unbufcall(int bcid)
{
	unbufcall(bcid);
}

__LIS_EXTERN_INLINE _RP void
lis_unfreezestr(queue_t *q)
{
	unfreezestr(q, 5);	/* note 5 is splstr */
}

__LIS_EXTERN_INLINE _RP mblk_t *
lis_unlinkb(mblk_t *mp)
{
	return unlinkb(mp);
}

__LIS_EXTERN_INLINE _RP toid_t
lis_untimeout(toid_t id)
{
	return (toid_t) untimeout(id);
}

__LIS_EXTERN_INLINE _RP int
lis_xmsgsize(mblk_t *mp)
{
	return xmsgsize(mp);
}

#ifndef __LIS_NO_MACROS
//#define lis_backq(__p1) lis_backq_fcn(__p1, __FILE__, __LINE__)
#define lis_timeout(__p1, __p2, __p3) lis_timeout_fcn(__p1, __p2, __p3, __FILE__, __LINE__)
#define lis_OTHERQ(__q) lis_safe_OTHERQ(__q, __FILE__, __LINE__)
#define lis_RD(__q) lis_safe_RD(__q, __FILE__, __LINE__)
#define lis_SAMESTR(__q) lis_safe_SAMESTR(__q, __FILE__, __LINE__)
#define lis_WR(__q) lis_safe_WR(__q, __FILE__, __LINE__)
#define lis_canenable(__q) lis_safe_canenable(__q, __FILE__, __LINE__)
#define lis_enableok(__q) lis_safe_enableok(__q, __FILE__, __LINE__)
#define lis_noenable(__q) lis_safe_noenable(__q, __FILE__, __LINE__)
#define lis_putmsg(__q, __mp) lis_safe_putmsg(__q, __mp, __FILE__, __LINE__)
#define lis_putnext(__q, __mp) lis_safe_putnext(__q, __mp, __FILE__, __LINE__)
#define lis_qreply(__q, __mp) lis_safe_qreply(__q, __mp, __FILE__, __LINE__)
#define lis_OTHER(__q) lis_OTHERQ(__q)
#define OTHER(__q) lis_OTHERQ(__q)
#endif

#else
#ifdef _LIS_SOURCE
#warning "_LIS_SOURCE defined by not CONFIG_STREAMS_COMPAT_LIS"
#endif
#endif

#endif				/* __SYS_LIS_STREAM_H__ */
