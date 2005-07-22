/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.6 2005/07/21 20:47:17 brian Exp $

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

 Last Modified $Date: 2005/07/21 20:47:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.6  2005/07/21 20:47:17  brian
 - sync with notebook

 Revision 0.9.2.5  2005/07/18 12:25:39  brian
 - standard indentation

 Revision 0.9.2.4  2005/07/18 01:00:30  brian
 - added missing STRHIGH, STRLOW, STRMAXPSZ and STRMINPSZ definitions

 Revision 0.9.2.3  2005/07/15 23:08:43  brian
 - checking in for sync

 Revision 0.9.2.2  2005/07/14 22:03:48  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:42  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_LFS_STREAM_H__
#define __SYS_LFS_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/lfs/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __LFS_EXTERN_INLINE
#define __LFS_EXTERN_INLINE extern __inline__
#endif

#ifndef _LFS_SOURCE
#warning "_LFS_SOURCE not defined but LFS stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_LFS || defined CONFIG_STREAMS_COMPAT_LFS_MODULE

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

/* Strangely, LiS 2.18.0 defined lis_appq, but no longer appq */
__LFS_EXTERN_INLINE int
appq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	return lis_appq(q, emp, nmp);
}

#define ANYBAND (-1)

extern int bcanget(queue_t *q, int band);
extern int canget(queue_t *q);

__LFS_EXTERN_INLINE int
enableq(queue_t *q)
{
	lis_flags_t flags;

	LIS_RDQISRLOCK(q, &flags);
	if (q->q_qinfo && q->q_qinfo->qi_srvp && !F_ISSET(q->q_flag, QNOENB)) {
		LIS_RDQISRUNLOCK(q, &flags);
		lis_qenable(q);
		return (1);
	}
	LIS_RDQISRUNLOCK(q, &flags);
	return (0);
}

typedef int (*qi_putp_t) (queue_t *, mblk_t *);
typedef int (*qi_srvp_t) (queue_t *);
typedef int (*qi_qopen_t) (queue_t *, dev_t *, int, int, cred_t *);
typedef int (*qi_qclose_t) (queue_t *, int, cred_t *);
typedef int (*qi_qadmin_t) (void);

__LFS_EXTERN_INLINE qi_qadmin_t
getadmin(modID_t modid)
{
	struct streamtab *st;
	struct qinit *qi;

	if ((st = lis_modstr(modid)))
		if ((qi = st->st_rdinit))
			return (qi->qi_qadmin);
	return (NULL);
}
__LFS_EXTERN_INLINE modID_t
getmid(const char *name)
{
	return lis_findmod(name);
}
__LFS_EXTERN_INLINE mblk_t *
linkmsg(mblk_t *mp1, mblk_t *mp2)
{
	if (mp1) {
		linkb(mp1, mp2);
		return (mp1);
	}
	return (mp2);
}
__LFS_EXTERN_INLINE int
pcmsg(unsigned char type)
{
	return ((type & QPCTL) != 0);
}

#undef datamsg
__LFS_EXTERN_INLINE int
datamsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	/* just so happens there is a gap in the QNORM mesages right at M_PCPROTO */
	return (((1 << mod) &
		 ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)) | (1 << M_DELAY))) !=
		0);
}

#define datamsg(_type) datamsg(_type)
#undef ctlmsg
__LFS_EXTERN_INLINE int
ctlmsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	/* just so happens there is a gap in the QNORM mesages right at M_PCPROTO */
	return (((1 << mod) & ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)))) == 0);
}

#define ctlmsg(_type) ctlmsg(_type)
#undef isdatablk
__LFS_EXTERN_INLINE int
isdatablk(dblk_t * db)
{
	return datamsg(db->db_type);
}

#define isdatablk(_db) isdatablk(_db)
#undef isdatamsg
__LFS_EXTERN_INLINE int
isdatamsg(mblk_t *mp)
{
	return isdatablk(mp->b_datap);
}

#define isdatamsg(_mp) isdatamsg(_mp)
#undef putctl
__LFS_EXTERN_INLINE int
putctl(queue_t *q, int type)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		put(q, mp);
		return (1);
	}
	return (0);
}

#define putctl(_q,_type) putctl(_q,_type)
#undef putctl1
__LFS_EXTERN_INLINE int
putctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param;
		put(q, mp);
		return (1);
	}
	return (0);
}

#define putctl1(_q,_type,_param) putctl1(_q,_type,_param)
#undef putctl2
__LFS_EXTERN_INLINE int
putctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param1;
		*mp->b_wptr++ = (unsigned char) param2;
		put(q, mp);
		return (1);
	}
	return (0);
}

#define putctl2(_q,_type,_param1,_param2) putctl2(_q,_type,_param1,_param2)
#undef putnextctl
__LFS_EXTERN_INLINE int
putnextctl(queue_t *q, int type)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

#define putnextctl(_q,_type) putnextctl(_q,_type)
#undef putnextctl1
__LFS_EXTERN_INLINE int
putnextctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

#define putnextctl1(_q,_type,_param) putnextctl1(_q,_type,_param)
#undef putnextctl2
__LFS_EXTERN_INLINE int
putnextctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param1;
		*mp->b_wptr++ = (unsigned char) param2;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

#define putnextctl2(_q,_type,_param1,_param2) putnextctl2(_q,_type,_param1,_param2)

__LFS_EXTERN_INLINE void setq(queue_t *q, struct qinit *rinit, struct qinit *wrinit)
{
	return lis_setq(q, rinit, wrinit);
}


#elif defined _LFS_SOURCE
#warning "_LFS_SOURCE defined by not CONFIG_STREAMS_COMPAT_LFS"
#endif

#define STRHIGH		5120	/* default hi water mark */
#define STRLOW		1024	/* default lo water mark */
#define STRMAXPSZ	(1<<12)	/* default max psz */
#define STRMINPSZ	0	/* default max psz */

#endif				/* __SYS_LFS_STREAM_H__ */
