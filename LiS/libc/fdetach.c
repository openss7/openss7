/*****************************************************************************

 @(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 1.1.1.3.12.3 $) $Date: 2005/07/13 12:01:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/07/13 12:01:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 1.1.1.3.12.3 $) $Date: 2005/07/13 12:01:21 $"

static char const ident[] =
    "$RCSfile: fdetach.c,v $ $Name:  $($Revision: 1.1.1.3.12.3 $) $Date: 2005/07/13 12:01:21 $";

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

#define DUMMY_STREAM "/dev/fifo.0"	/* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

int
__lis_fdetach(const char *path)
{
	int ffd, error = 0;

	if ((ffd = open(DUMMY_STREAM, DUMMY_MODE)) < 0)
		return -1;
	if (ioctl(ffd, I_LIS_FDETACH, path) < 0)
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
 * @ingroup libLiS
 * @brief detach a path from a stream.
 * @param path the path in the filesystem from which to detach.
 *
 * fdetach() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
 * must protect from asyncrhonous cancellation between the open(), ioctl() and
 * close() operations.
 */
int
__lis_fdetach_r(const char *path)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	ret = __lis_fdetach(path);
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

__asm__(".symver __lis_fdetach_r,fdetach@@LIS_1.0");
