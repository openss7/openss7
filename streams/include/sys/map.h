/*****************************************************************************

 @(#) $Id: map.h,v 0.9.2.1 2005/04/28 01:26:08 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/04/28 01:26:08 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_MAP_H
#define _SYS_MAP_H

#ident "@(#) $RCSfile: map.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/04/28 01:26:08 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#include <linux/types.h>

struct map {
	ulong m_size;			/* size of this segment in units */
	ulong m_addr;			/* index of the base of this segment */
};

__EXTERN_INLINE struct map *mapstart(struct map *map)
{
	return (&map[2]);
}
__EXTERN_INLINE ulong mapfree(struct map *map)
{
	return map[0].m_size;
}
__EXTERN_INLINE ulong mapwant(struct map *map)
{
	return map[0].m_addr;
}
__EXTERN_INLINE char *mapname(struct map *map)
{
	return (char *) map[1].m_addr;
}
__EXTERN_INLINE ulong mapsize(struct map *map)
{
	return map[1].m_size;
}

void rminit(struct map *map, ulong mapsize);
void mapinit(struct map *map, long size, ulong index, char *name, int mapsize);

struct map *rmallocmap(size_t mapsize);
struct map *rmallocmap_wait(size_t mapsize);
void rmfreemap(struct map *map);

ulong malloc(struct map *map, size_t size);
ulong rmalloc(struct map *map, size_t size);
ulong rmalloc_wait(struct map *map, size_t size);
ulong rmalloc_locked(struct map *map, size_t size);

void mfree(struct map *map, size_t size, ulong index);	/* also long for size */
void rmfree(struct map *map, size_t size, ulong index);	/* also long for size */

int rmget(struct map *map, size_t size, ulong index);	/* also long for size */

__EXTERN_INLINE ulong rmwanted(struct map *map)
{
	return map[0].m_addr;
}

void rmsetwant(struct map *map);

#if defined _OSF_SOURCE || defined _SUN_SOURCE
#define rminit mapinit
#define mapinit mapinit
#endif
#if defined _UW7_SOURCE || defined _UXP_SOURCE || defined _SUX_SOURCE
#define rminit rminit
#define mapinit rminit
#endif

#endif				/* _SYS_MAP_H */
