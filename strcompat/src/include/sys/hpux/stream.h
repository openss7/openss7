/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.10 2008-04-28 16:47:08 brian Exp $

 -----------------------------------------------------------------------------

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

 Last Modified $Date: 2008-04-28 16:47:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.10  2008-04-28 16:47:08  brian
 - updates for release

 Revision 0.9.2.9  2007/08/12 15:51:01  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.8  2006/12/08 05:08:07  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.7  2006/11/03 10:39:16  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.6  2006/06/22 13:11:21  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.5  2005/12/28 09:51:47  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.4  2005/07/18 12:25:39  brian
 - standard indentation

 Revision 0.9.2.3  2005/07/15 23:08:31  brian
 - checking in for sync

 Revision 0.9.2.2  2005/07/14 22:03:45  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:42  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_HPUX_STREAM_H__
#define __SYS_HPUX_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.10 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/hpux/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __HPUX_EXTERN_INLINE
#define __HPUX_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif

#ifndef __HPUX_EXTERN
#define __HPUX_EXTERN extern streamscall
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _HPUX_SOURCE
#warning "_HPUX_SOURCE not defined but HPUX stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_HPUX || defined CONFIG_STREAMS_COMPAT_HPUX_MODULE

#ifndef lock_t
typedef spinlock_t lock_t;

#define lock_t lock_t
#endif

__HPUX_EXTERN lock_t *streams_get_sleep_lock(caddr_t event);

#ifdef LFS
typedef void (*streams_put_t) (void *, mblk_t *);
__HPUX_EXTERN void streams_put(streams_put_t func, queue_t *q, mblk_t *mp, void *priv);
#endif

#else
#ifdef _HPUX_SOURCE
#warning "_HPUX_SOURCE defined by not CONFIG_STREAMS_COMPAT_HPUX"
#endif
#endif

#endif				/* __SYS_HPUX_STREAM_H__ */
