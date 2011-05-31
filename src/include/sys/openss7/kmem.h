/*****************************************************************************

 @(#) $Id: kmem.h,v 1.1.2.7 2011-05-31 09:46:07 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-05-31 09:46:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: kmem.h,v $
 Revision 1.1.2.7  2011-05-31 09:46:07  brian
 - new distros

 Revision 1.1.2.6  2011-04-07 15:24:03  brian
 - weak reference corrections

 Revision 1.1.2.5  2011-04-06 21:33:05  brian
 - corrections

 Revision 1.1.2.4  2011-04-05 16:35:13  brian
 - weak module design

 Revision 1.1.2.3  2010-11-28 14:21:52  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.2  2009-07-21 11:06:15  brian
 - changes from release build

 Revision 1.1.2.1  2009-06-21 11:26:48  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_OPENSS7_KMEM_H__
#define __SYS_OPENSS7_KMEM_H__ 1

#ifndef __SYS_KMEM_H__
#warning "Do not include sys/openss7/kmem.h directly, include sys/kmem.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef streams_fastcall
# if defined __i386__ || defined __x86_64__ || defined __k8__
#  define streams_fastcall __attribute__((__regparm__(3)))
# else
#  define streams_fastcall
# endif
#endif

#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __STREAMS_EXTERN
#define __STREAMS_EXTERN __EXTERN streams_fastcall
#endif

#ifdef NEED_LINUX_AUTOCONF_H
#include <linux/autoconf.h>
#endif
#include <linux/module.h>
#include <linux/types.h>	/* for various types */

#define KM_SLEEP	0
#define KM_NOSLEEP	(1<<0)
#define KM_PHYSCONTIG	(1<<1)	/* IRIX 6.5 */
#define KM_CACHEALIGN	(1<<2)	/* IRIX 6.5 */
#define KM_DMA		(1<<3)	/* Linux Fast-STREAMS specific */

/* typedef unsigned short cnodeid_t; */
typedef int cnodeid_t;

#ifndef HAVE_KMEM_ALLOC_EXPORT
__STREAMS_EXTERN void *kmem_alloc(size_t size, int flags);
#else
__STREAMS_EXTERN void *kmem_alloc_(size_t size, int flags);

#undef kmem_alloc
#define kmem_alloc(size,flags) kmem_alloc_(size,flags)
#endif

#ifndef HAVE_KMEM_ZALLOC_EXPORT
__STREAMS_EXTERN void *kmem_zalloc(size_t size, int flags);
#else
__STREAMS_EXTERN void *kmem_zalloc_(size_t size, int flags);

#undef kmem_zalloc
#define kmem_zalloc(size,flags) kmem_zalloc_(size,flags)
#endif

#ifndef HAVE_KMEM_FREE_EXPORT
__STREAMS_EXTERN void kmem_free(void *ptr, size_t size);
#else
__STREAMS_EXTERN void kmem_free_(void *ptr, size_t size);

#undef kmem_free
#define kmem_free(ptr,size) kmem_free_(ptr,size)
#endif

#if 0
#ifdef CONFIG_NUMA
#warning kmem_alloc_node and kmem_zalloc_node are not supported on NUMA architectures
#endif
#endif

__STREAMS_EXTERN void *kmem_alloc_node(size_t size, int flags, cnodeid_t node);
__STREAMS_EXTERN void *kmem_zalloc_node(size_t size, int flags, cnodeid_t node);

#endif				/* __SYS_OPENSS7_KMEM_H__ */
