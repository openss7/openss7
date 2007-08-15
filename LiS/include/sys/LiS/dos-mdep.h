/*****************************************************************************

 @(#) $Id: dos-mdep.h,v 1.1.1.2.4.3 2007/08/14 10:47:09 brian Exp $

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

 Last Modified $Date: 2007/08/14 10:47:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dos-mdep.h,v $
 Revision 1.1.1.2.4.3  2007/08/14 10:47:09  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef __SYS_LIS_DOS_MDEP_H__
#define __SYS_LIS_DOS_MDEP_H__

#ident "@(#) $RCSfile: dos-mdep.h,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2007/08/14 10:47:09 $"

/*                               -*- Mode: C -*- 
 * dos-mdep.c --- Testing environment for LiS under MS-Dog.
 * Author          : Francisco J. Ballesteros
 * Created On      : Sat Jun  4 20:56:03 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Fri Sep 29 13:25:04 1995
 * Update Count    : 5
 * RCS Id          : $Id: dos-mdep.h,v 1.1.1.2.4.3 2007/08/14 10:47:09 brian Exp $
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
 *    You can reach me by email to
 *    gram@aztec.co.za
 */
/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/types.h>		/* common system types */
#include <stdlib.h>		/* for NULL, malloc, free */
#include <stdio.h>		/* for printf */
#include <mem.h>		/* for memcpy */
#include <memory.h>		/* for memcpy */
#include <assert.h>
#include <sys/signal.h>		/* for signal numbers */

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

typedef unsigned long ulong_t;
typedef unsigned short ushort_t;
typedef unsigned char uchar_t;
typedef short o_uid_t;
typedef short o_gid_t;
typedef short uid_t;
typedef short gid_t;
typedef unsigned uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef char *caddr_t;

struct inode {
	char pad;
};
struct wait_queue {
	char pad;
};

#endif				/* __SYS_LIS_DOS_MDEP_H__ */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
