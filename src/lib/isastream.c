/*****************************************************************************

 @(#) $RCSfile: isastream.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:21:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2010-11-28 14:21:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: isastream.c,v $
 Revision 1.1.2.2  2010-11-28 14:21:58  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:37:46  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: isastream.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:21:58 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS Library isastream() implementation file. */

/** @brief Test a stream.
  * @param fd a file descriptor to test.
  *
  * This is a non-thread-safe implementation of isastream().
  */
__unlikely int
__streams_isastream(int fd)
{
	if (ioctl(fd, I_ISASTREAM) == -1) {
		if (errno == EBADF)
			return (-1);
		return (0);
	}
	return (1);
}

/** @brief Test a stream.
  * @param fd a file descriptor to test.
  * @version STREAMS_1.0 isastream()
  *
  * This is a thread-safe implementation of isastream().
  *
  * isastream() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, asyncrhonous thread cancellation must be deferred across the
  * call and restored without testing for thread cancellation.
  */
__unlikely int
__streams_isastream_r(int fd)
{
	int oldtype = 0;
	int retval;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	retval = __streams_isastream(fd);
	pthread_setcanceltype(oldtype, NULL);
	return (retval);
}

/** @fn int isastream(int fd)
  * @param fd a file descriptor to test.
  * @version STREAMS_1.0 __streams_isastream_r()
  */
__asm__(".symver __streams_isastream_r,isastream@@STREAMS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
