/*****************************************************************************

 @(#) src/include/sys/hpux/stream.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __SYS_HPUX_STREAM_H__
#define __SYS_HPUX_STREAM_H__

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

#include <sys/openss7/config.h>

#if defined CONFIG_STREAMS_COMPAT_HPUX || defined CONFIG_STREAMS_COMPAT_HPUX_MODULE

#ifndef lock_t
typedef spinlock_t lock_t;

#define lock_t lock_t
#endif

__HPUX_EXTERN lock_t *streams_get_sleep_lock(caddr_t event);

typedef void (*streams_put_t) (void *, mblk_t *);
__HPUX_EXTERN void streams_put(streams_put_t func, queue_t *q, mblk_t *mp, void *priv);

#else
#ifdef _HPUX_SOURCE
#warning "_HPUX_SOURCE defined by not CONFIG_STREAMS_COMPAT_HPUX"
#endif
#endif

#endif				/* __SYS_HPUX_STREAM_H__ */
