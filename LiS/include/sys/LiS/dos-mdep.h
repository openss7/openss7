/*                               -*- Mode: C -*- 
 * dos-mdep.c --- Testing environment for LiS under MS-Dog.
 * Author          : Francisco J. Ballesteros
 * Created On      : Sat Jun  4 20:56:03 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Fri Sep 29 13:25:04 1995
 * Update Count    : 5
 * RCS Id          : $Id: dos-mdep.h,v 1.2 1996/01/20 17:01:57 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) incomplete,untested,compiled
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : provide Dog <-> LiS entry points.
 *                 : 
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Graham Wheeler
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 *
 *    You can reach me by email to
 *    gram@aztec.co.za
 */
/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/types.h>        /* common system types */
#include <stdlib.h>	      /* for NULL, malloc, free */
#include <stdio.h>	      /* for printf */
#include <mem.h>	      /* for memcpy */
#include <memory.h>	      /* for memcpy */
#include <assert.h>
#include <sys/signal.h>	      /* for signal numbers */


/*  -------------------------------------------------------------------  */

#define VOID			void
#define ALLOC(n)		malloc(n)
#define FREE(p,n)		free(p)
#define MEMCPY(dest, src, len)	memcpy(dest, src, len)
#define INLINE
#define LOG(f,l,s)		printf("%s (line %d of file %s)\n", s, l, f)
#define PANIC(msg)		( printf("PANIC: %s\n", msg), exit(0) )
#define LISASSERT(e)		assert(e)

#define splx(s)		((void)s)
#define splstr()	0

typedef unsigned long	ulong_t;
typedef unsigned short	ushort_t;
typedef unsigned char	uchar_t;
typedef short		o_uid_t;
typedef short		o_gid_t;
typedef short		uid_t;
typedef short		gid_t;
typedef unsigned	uint;
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned long   ulong;
typedef char*		caddr_t;

struct inode { char pad; };
struct wait_queue { char pad; };


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
