/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.14 2006/11/03 10:39:15 brian Exp $

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

 Last Modified $Date: 2006/11/03 10:39:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.14  2006/11/03 10:39:15  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_AIX_DDI_H__
#define __SYS_AIX_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.14 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __AIX_EXTERN_INLINE
#define __AIX_EXTERN_INLINE __EXTERN_INLINE
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _AIX_SOURCE
#warning "_AIX_SOURCE not defined but AIX ddi.h included"
#endif

#if defined(CONFIG_STREAMS_COMPAT_AIX) || defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)

#else
#ifdef _AIX_SOURCE
#warning "_AIX_SOURCE defined but not CONFIG_STREAMS_COMPAT_AIX"
#endif
#endif

#endif				/* __SYS_AIX_DDI_H__ */
