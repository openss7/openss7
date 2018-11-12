/*****************************************************************************

 @(#) File: src/lib/fattach.c

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

static char const ident[] __attribute__ ((unused)) = "src/lib/fattach.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS Library fattach() implementation file.  */

/** @brief Attach a stream to a path in a filesystem.
  * @param fd the file descriptor of the stream to attach.
  * @param path the path in the filesystem to which to attach the stream.
  *
  * This is a non-thread-safe implementation of fattach().
  */
int __unlikely
__streams_fattach(int fd, const char *path)
{
	return (ioctl(fd, I_FATTACH, path));
}

/** @brief Attach a stream to a path in a filesystem.
  * @param fd the file descriptor of the stream to attach.
  * @param path the path in the filesystem to which to attach the stream.
  * @version STREAMS_1.0 fattach()
  *
  * This is a thread-safe implementation of fattach().
  *
  * fattach() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, asyncrhonous thread cancellation must be deferred across the
  * call and restored without testing for thread cancellation.
  */
int __unlikely
__streams_fattach_r(int fd, const char *path)
{
	int oldtype = 0;
	int retval;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	retval = __streams_fattach(fd, path);
	pthread_setcanceltype(oldtype, NULL);
	return (retval);
}

/** @fn int fattach(int fd, const char *path)
  * @param fd the file descriptor of the stream to attach.
  * @param path the path in the filesystem to which to attach the stream.
  * @version STREAMS_1.0 __streams_fattach_r()
  */
__asm__(".symver __streams_fattach_r,fattach@@STREAMS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
