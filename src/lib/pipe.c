/*****************************************************************************

 @(#) File: src/lib/pipe.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 *****************************************************************************/

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS System Call pipe() implementation file. */

/** @brief open a streams based pipe.
  * @param fds a pointer to the two file descriptors, one for each end of the pipe.
  *
  * This is the non-thread-safe implementation of pipe().
  *
  * This implementation opens two pipe(4) devices to get two Stream heads and
  * then welds them together using the #I_PIPE input-output control to on
  * Stream head with the other Stream head's file descriptor as an argument.
  *
  * There exists a slightly different approach using the spx(4) device.  In
  * that case, two spx(4) devices are opened and then one Stream's file
  * descriptor is passed to the other using the #I_FDINSERT intput-output
  * control.  That approach is illustrated in Stevens "Advanced Programming in
  * the UNIX(R) Environment", but is is slower and more cumbersome than this
  * approach.  This approach is Linux Fast-STREAMS specific.  The Stevens
  * approach would probably work also on UnixWare, AIX and others.
  */
int
__streams_pipe(int fds[2])
{
	int fd1, fd2;
	int error;

	if (unlikely((fd1 = open("/dev/streams/clone/pipe", O_RDWR)) < 0)) {
		return (-1);
	}
	if (unlikely((fd2 = open("/dev/streams/clone/pipe", O_RDWR)) < 0)) {
		error = errno;
		close(fd1);
		errno = error;
		return (-1);
	}
	if (unlikely(ioctl(fd1, I_PIPE, fd2) < 0)) {
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

/** @brief open a streams based pipe.
  * @param fds a pointer to the two file descriptors, one for each end of the pipe.
  * @version STREAMS_1.0 pipe()
  *
  * This is the thread-safe implementation of pipe().
  *
  * pipe() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
  * must protect from asyncrhonous cancellation between the open(), ioctl() and
  * close() operations in __streams_pipe().  */
int
__streams_pipe_r(int fds[2])
{
	int oldtype = 0;
	int retval;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	retval = __streams_pipe(fds);
	pthread_setcanceltype(oldtype, NULL);
	return (retval);
}

/** @fn int pipe(int fds[2])
  * @param fds a pointer to the two file descriptors, one for each end of the pipe.
  * @version STREAMS_1.0 __streams_pipe_r().
  */
__asm__(".symver __streams_pipe_r,pipe@@STREAMS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
