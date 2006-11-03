/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.8 2006/11/03 10:39:22 brian Exp $

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

 Last Modified $Date: 2006/11/03 10:39:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.8  2006/11/03 10:39:22  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.7  2006/07/24 09:00:59  brian
 - results of udp2 optimizations

 Revision 0.9.2.6  2006/06/22 13:11:27  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.5  2006/05/23 10:44:06  brian
 - mark normal inline functions for unlikely text section

 Revision 0.9.2.4  2005/12/28 09:51:48  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.3  2005/07/18 12:25:40  brian
 - standard indentation

 Revision 0.9.2.2  2005/07/14 22:03:58  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:44  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_OSF_STREAM_H__
#define __SYS_OSF_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.8 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/osf/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __OSF_EXTERN_INLINE
#define __OSF_EXTERN_INLINE __EXTERN_INLINE
#endif

#ifndef _OSF_SOURCE
#warning "_OSF_SOURCE not defined but OSF stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_OSF || defined CONFIG_STREAMS_COMPAT_OSF_MODULE

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

extern int streams_close_comm(queue_t *, int, cred_t *);
extern int streams_open_comm(unsigned int, queue_t *, dev_t *, int, int, cred_t *);
extern int streams_open_ocomm(dev_t, unsigned int, queue_t *, dev_t *, int, int, cred_t *);

__OSF_EXTERN_INLINE void
puthere(queue_t *q, mblk_t *mp)
{
	put(q, mp);
}

#else
#ifdef _OSF_SOURCE
#warning "_OSF_SOURCE defined by not CONFIG_STREAMS_COMPAT_OSF"
#endif
#endif

#endif				/* __SYS_OSF_STREAM_H__ */
