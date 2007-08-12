/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.9 2007/08/12 15:51:10 brian Exp $

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

 Last Modified $Date: 2007/08/12 15:51:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.9  2007/08/12 15:51:10  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.8  2006/12/08 05:08:16  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.7  2006/11/03 10:39:21  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_OS7_STREAM_H__
#define __SYS_OS7_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/irix/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __OS7_EXTERN_INLINE
#define __OS7_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif

#ifndef __OS7_EXTERN
#define __OS7_EXTERN extern streamscall
#endif

#ifndef _OS7_SOURCE
#warning "_OS7_SOURCE not defined but OS7 stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_OS7 || defined CONFIG_STREAMS_COMPAT_OS7_MODULE

#include <sys/os7/debug.h>	/* generic debugging macros */
#include <sys/os7/bufq.h>	/* generic buffer queues */
#include <sys/os7/priv.h>	/* generic data structures */
#include <sys/os7/lock.h>	/* generic queue locking functions */
#include <sys/os7/queue.h>	/* generic put and srv routines */
#include <sys/os7/allocb.h>	/* generic buffer allocation routines */
#include <sys/os7/timer.h>	/* generic timer handling */
#include <sys/os7/bufpool.h>	/* generic buffer pools */

#else
#ifdef _OS7_SOURCE
#warning "_OS7_SOURCE defined by not CONFIG_STREAMS_COMPAT_OS7"
#endif
#endif

#endif				/* __SYS_OS7_STREAM_H__ */
