/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.4 2005/12/28 09:51:49 brian Exp $

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

 Last Modified $Date: 2005/12/28 09:51:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.4  2005/12/28 09:51:49  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.3  2005/07/18 12:25:41  brian
 - standard indentation

 Revision 0.9.2.2  2005/07/14 22:04:05  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:45  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_SVR4_STREAM_H__
#define __SYS_SVR4_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/svr4/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __SVR4_EXTERN_INLINE
#define __SVR4_EXTERN_INLINE extern __inline__
#endif

#ifndef _SVR4_SOURCE
#warning "_SVR4_SOURCE not defined but SVR4 stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_SVR4 || defined CONFIG_STREAMS_COMPAT_SVR4_MODULE

/* don't use these functions, they are way too dangerous */
extern long MPSTR_QLOCK(queue_t *q);

#ifndef MPSTR_QLOCK
#define MPSTR_QLOCK(__q) MPSTR_QLOCK(__q)
#endif
extern void MPSTR_QRELE(queue_t *q, long s);

#ifndef MPSTR_QRELE
#define MPSTR_QRELE(__q,__f) MPSTR_QRELE(__q,__f)
#endif
extern long MPSTR_STPLOCK(struct stdata *stp);

#ifndef MPSTR_STPLOCK
#define MPSTR_STPLOCK(__s) MPSTR_STPLOCK(__s)
#endif
extern void MPSTR_STPRELE(struct stdata *stp, long s);

#ifndef MPSTR_STPRELE
#define MPSTR_STPRELE(__s,__f) MPSTR_STPRELE(__s,__f)
#endif

#else
#ifdef _SVR4_SOURCE
#warning "_SVR4_SOURCE defined by not CONFIG_STREAMS_COMPAT_SVR4"
#endif
#endif

#endif				/* __SYS_SVR4_STREAM_H__ */
