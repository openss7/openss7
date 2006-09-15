/*****************************************************************************

 @(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/07/24 09:01:14 $

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

 Last Modified $Date: 2006/07/24 09:01:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/07/24 09:01:14 $"

static char const ident[] = "$RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/07/24 09:01:14 $";

#define _XOPEN_SOURCE 600
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

#if 0
#define DUMMY_STREAM "/dev/nuls"	/* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

static int
__lis_pipe(int *fds)
{
	int fd, error = 0;

	if ((fd = open(DUMMY_STREAM, DUMMY_MODE)) < 0)
		return -1;
	if (ioctl(fd, I_PIPE, fds) < 0)
		error = errno;
	close(fd);
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}
#endif

/* I prefer not to use a dummy stream anyway. */
int
__streams_pipe(int *fds)
{
	int fd1, fd2;
	int error;

	if ((fd1 = open("/dev/pipe", O_RDWR)) < 0) {
		return (-1);
	}
	if ((fd2 = open("/dev/pipe", O_RDWR)) < 0) {
		error = errno;
		close(fd1);
		errno = error;
		return (-1);
	}
	if (ioctl(fd1, I_PIPE, fd2) < 0) {
		error = errno;
		close(fd2);
		close(fd1);
		errno = error;
		return (-1);
	}
	fds[0] = fd1;
	fds[1] = fd2;
	return (0);
}

/**
 * @fn pipe(int *fds)
 * @ingroup libLiS
 * @brief open a streams based pipe.
 * @param fds a pointer to the two file descriptors, one for each end of the pipe.
 *
 * pipe() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
 * must protect from asyncrhonous cancellation between the open(), ioctl() and
 * close() operations.
 */
int
__streams_pipe_r(int *fds)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, &oldtype);
#if 0
	ret = __lis_pipe(fds);
#else
	ret = __streams_pipe(fds);
#endif
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

__asm__(".symver __streams_pipe_r,pipe@@STREAMS_1.0");
