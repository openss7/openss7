/*****************************************************************************

 @(#) $Id: kmem.h,v 0.9.2.4 2004/08/22 06:17:51 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/22 06:17:51 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_KMEM_H__
#define __SYS_KMEM_H__ 1

#ident "@(#) $RCSfile: kmem.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/22 06:17:51 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#include <linux/string.h>
#include <linux/slab.h>

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#define KM_SLEEP    0
#define KM_NOSLEEP  1

typedef unsigned short cnodeid_t;

__EXTERN_INLINE void *kmem_alloc(size_t size, int flags)
{
	return size ? kmalloc(size, flags == KM_NOSLEEP ? GFP_ATOMIC : GFP_KERNEL) : NULL;
}

__EXTERN_INLINE void *kmem_zalloc(size_t size, int flags)
{
	void *mem;
	if ((mem = kmem_alloc(size, flags)))
		memset(mem, 0, size);
	return (mem);
}

extern void kmem_free(void *ptr, size_t size);

__EXTERN_INLINE void *kmem_alloc_node(size_t size, int flags, cnodeid_t node)
{
	return kmalloc(size, GFP_KERNEL);
}
__EXTERN_INLINE void *kmem_zalloc_node(size_t size, int flags, cnodeid_t node)
{
	void *mem;
	if ((mem = kmalloc(size, GFP_KERNEL)))
		memset(mem, 0, size);
	return (mem);
}

#endif				/* __SYS_KMEM_H__ */
