/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 1.1.1.3.12.2 $) $Date: 2005/04/12 22:45:29 $"

static char const ident[] =
    "$RCSfile: pipe.c,v $ $Name:  $($Revision: 1.1.1.3.12.2 $) $Date: 2005/04/12 22:45:29 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @addtogroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS System Call pipe() implementation file.  */

/** @brief open a streams based pipe.
  * @param fds a pointer to the two file descriptors, one for each end of the pipe.
  */
int
__lis_pipe(int fds[2])
{
	int fd, error = 0;

	if ((fd = open(DUMMY_STREAM, DUMMY_MODE)) < 0)
		return -1;
	if (ioctl(fd, I_LIS_PIPE, fds) < 0)
		error = errno;
	close(fd);
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

/** @brief open a streams based pipe.
  * @param fds a pointer to the two file descriptors, one for each end of the pipe.
  * @version LIS_1.0 pipe()
  *
  * pipe() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
  * must protect from asyncrhonous cancellation between the open(), ioctl() and
  * close() operations.
  */
int
__lis_pipe_r(int fds[2])
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, &oldtype);
	ret = __lis_pipe(fds);
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

/** @fn int pipe(int fds[2])
  * @brief open a streams based pipe.
  * @param fds a pointer to the two file descriptors, one for each end of the pipe.
  * @version LIS_1.0 __lis_pipe_r()
  */
__asm__(".symver __lis_pipe_r,pipe@@LIS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
