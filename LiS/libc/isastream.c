/*****************************************************************************

 @(#) $RCSfile: isastream.c,v $ $Name:  $($Revision: 1.1.1.2.12.2 $) $Date: 2005/04/12 22:45:29 $

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

 Last Modified $Date: 2005/04/12 22:45:29 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: isastream.c,v $ $Name:  $($Revision: 1.1.1.2.12.2 $) $Date: 2005/04/12 22:45:29 $"

static char const ident[] =
    "$RCSfile: isastream.c,v $ $Name:  $($Revision: 1.1.1.2.12.2 $) $Date: 2005/04/12 22:45:29 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @addtogroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS System Call isastream() implementation file.  */

/** @brief Test a stream.
  * @param fd a file descriptor to test.
  * @version LIS_1.0
  *
  * This is a non-thread-safe implementation of isastream().
  */
int
__lis_isastream(int fd)
{
#ifdef USER
	return (user_ioctl(fd, I_CANPUT, 0) != -1);
#else
	return (ioctl(fd, I_CANPUT, 0) != -1);
#endif
}

/** @brief Test a stream.
  * @param fd a file descriptor to test.
  * @version LIS_1.0 isastream()
  *
  * This is a thread-safe implementation of isastream().
  *
  * isastream() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Because isastream() consists of a single system call, asynchronous thread
  * cancellation protection is not required.
  */
int
__lis_isastream_r(int fd)
{
	int oldtype;
	int retval;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	retval = __lis_isastream(fd);
	pthread_setcanceltype(oldtype, NULL);
	return (retval);
}

/** @fn int isastream(int fd)
  * @brief Test a stream.
  * @param fd a file descriptor to test.
  * @version LIS_1.0 __lis_isastream_r()
  */
__asm__(".symver __lis_isastream_r,isastream@@LIS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
