/*****************************************************************************

 @(#) $RCSfile: fattach.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2006/09/18 01:43:44 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 2.1 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

 Last Modified $Date: 2006/09/18 01:43:44 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fattach.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2006/09/18 01:43:44 $"

static char const ident[] =
    "$RCSfile: fattach.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2006/09/18 01:43:44 $";

/* This file can be processed with doxygen(1). */

#include <sys/types.h>
#include <stropts.h>
#include <sys/ioctl.h>

#if __GNUC__ < 3
#define inline inline
#define noinline extern
#else
#define inline __attribute__((always_inline))
#define noinline static __attribute__((noinline))
#endif
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

/**
 * @fn int fattach(int fd, const char *path)
 * @brief attach a stream to a path in a filesystem.
 * @param fd the file descriptor of the stream to attach.
 * @param path the path in the filesystem to which to attach the stream.
 *
 * fattach() cannot contain a thread cancellation point.  Because this
 * function contains a single system call, it is asyncrhonous thread
 * cancellation safe.
 */
int __unlikely
__streams_fattach(int fd, const char *path)
{
	return (ioctl(fd, I_FATTACH, path));
}

__asm__(".symver __streams_fattach,fattach@@STREAMS_1.0");

int __lis_fattach(int, const char *)
	__attribute__((weak, alias("__streams_fattach")));

__asm__(".symver __lis_fattach,fattach@LIS_1.0");
