/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.4 2005/12/28 09:51:48 brian Exp $

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

 Last Modified $Date: 2005/12/28 09:51:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.4  2005/12/28 09:51:48  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.3  2005/07/18 12:25:40  brian
 - standard indentation

 Revision 0.9.2.2  2005/07/15 23:09:21  brian
 - checking in for sync

 Revision 0.9.2.1  2005/07/12 13:54:43  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_OS7_STREAM_H__
#define __SYS_OS7_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/irix/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __OS7_EXTERN_INLINE
#define __OS7_EXTERN_INLINE extern __inline__
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
