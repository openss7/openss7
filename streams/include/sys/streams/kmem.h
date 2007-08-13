/*****************************************************************************

 @(#) $Id: kmem.h,v 0.9.2.21 2007/08/13 22:46:08 brian Exp $

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

 Last Modified $Date: 2007/08/13 22:46:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: kmem.h,v $
 Revision 0.9.2.21  2007/08/13 22:46:08  brian
 - GPLv3 header updates

 Revision 0.9.2.20  2006/10/27 23:19:32  brian
 - changes for 2.6.18 kernel

 Revision 0.9.2.19  2006/02/22 11:35:46  brian
 - added __x86_64__ and __k8__ to regparms

 Revision 0.9.2.18  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_KMEM_H__
#define __SYS_STREAMS_KMEM_H__ 1

#ident "@(#) $RCSfile: kmem.h,v $ $Name:  $($Revision: 0.9.2.21 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_KMEM_H__
#warning "Do no include sys/streams/kmem.h directly, include sys/kmem.h instead."
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

#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/types.h>	/* for various types */

#define KM_SLEEP	0
#define KM_NOSLEEP	(1<<0)
#define KM_PHYSCONTIG	(1<<1)	/* IRIX 6.5 */
#define KM_CACHEALIGN	(1<<2)	/* IRIX 6.5 */
#define KM_DMA		(1<<3)	/* Linux Fast-STREAMS specific */

/* typedef unsigned short cnodeid_t; */
typedef int cnodeid_t;

__STREAMS_EXTERN void *kmem_alloc(size_t size, int flags);
__STREAMS_EXTERN void *kmem_zalloc(size_t size, int flags);
__STREAMS_EXTERN void kmem_free(void *ptr, size_t size);

#if 0
#ifdef CONFIG_NUMA
#warning kmem_alloc_node and kmem_zalloc_node are not supported on NUMA architectures
#endif
#endif

__STREAMS_EXTERN void *kmem_alloc_node(size_t size, int flags, cnodeid_t node);
__STREAMS_EXTERN void *kmem_zalloc_node(size_t size, int flags, cnodeid_t node);

#endif				/* __SYS_STREAMS_KMEM_H__ */
