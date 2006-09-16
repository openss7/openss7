/*****************************************************************************

 @(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/07/24 09:01:14 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2.1 of the License, or (at your option)
 any later version.

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

 Last Modified $Date: 2006/07/24 09:01:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/07/24 09:01:14 $"

static char const ident[] = "$RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/07/24 09:01:14 $";

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE 1
#define _REENTRANT
#define _THREAD_SAFE

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stropts.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <pthread.h>

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

extern int pthread_setcanceltype(int type, int *oldtype);

#define DUMMY_STREAM "/dev/fifo.0"	/* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

int __unlikely
__streams_fdetach(const char *path)
{
	int ffd, error = 0;

	if ((ffd = open(DUMMY_STREAM, DUMMY_MODE)) < 0)
		return -1;
	if (ioctl(ffd, I_FDETACH, path) < 0)
		error = errno;
	close(ffd);
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

/**
 * @fn int fdetach(const char *path)
 * @ingroup streams
 * @brief detach a path from a stream.
 * @param path the path in the filesystem from which to detach.
 *
 * fdetach() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
 * must protect from asyncrhonous cancellation between the open(), ioctl() and
 * close() operations.
 */
int __unlikely
__streams_fdetach_r(const char *path)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	ret = __streams_fdetach(path);
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

__asm__(".symver __streams_fdetach_r,fdetach@@STREAMS_1.0");

int __lis_fdetach(const char *)
	__attribute__((weak, alias("__streams_fdetach")));

int __lis_fdetach_r(const char *)
	__attribute__((weak, alias("__streams_fdetach_r")));

__asm__(".symver __lis_fdetach_r,fdetach@LIS_1.0");


