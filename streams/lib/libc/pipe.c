/*****************************************************************************

 @(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/10/12 09:55:46 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2005/10/12 09:55:46 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/10/12 09:55:46 $"

static char const ident[] =
    "$RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/10/12 09:55:46 $";

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

extern int __pthread_setcanceltype(int type, int *oldtype);

#pragma weak __pthread_setcanceltype
#pragma weak pthread_setcanceltype

int
pthread_setcanceltype(int type, int *oldtype)
{
	if (__pthread_setcanceltype)
		return __pthread_setcanceltype(type, oldtype);
	if (oldtype)
		*oldtype = type;
	return (0);
}

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
static int
__lfs_pipe(int *fds)
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
 * @ingroup libLiS
 * @brief open a streams based pipe.
 * @param fds a pointer to the two file descriptors, one for each end of the pipe.
 *
 * pipe() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
 * must protect from asyncrhonous cancellation between the open(), ioctl() and
 * close() operations.
 */
int
pipe(int *fds)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, &oldtype);
#if 0
	ret = __lis_pipe(fds);
#else
	ret = __lfs_pipe(fds);
#endif
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}
