/*****************************************************************************

 @(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-28 12:54:03 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-04-28 12:54:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: fdetach.c,v $
 Revision 0.9.2.15  2008-04-28 12:54:03  brian
 - update file headers for release

 Revision 0.9.2.14  2007/08/13 22:46:10  brian
 - GPLv3 header updates

 Revision 0.9.2.13  2006/12/18 10:08:56  brian
 - updated headers for release

 *****************************************************************************/

#ident "@(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-28 12:54:03 $"

static char const ident[] =
    "$RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-28 12:54:03 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS Library fdetach() implementation file. */

#define DUMMY_STREAM "/dev/fifo.0"	/* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

/** @brief Detach a path from a Stream.
  * @param path the path in the filesystem from which to detach.
  * @version STREAMS_1.0
  */
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

/** @brief Detach a path from a Stream.
  * @param path the path in the filesystem from which to detach.
  * @version STREAMS_1.0 fdetach()
  *
  * fdetach() cannot contain a thread cancellation point (SUS/XOPEN/POSIX).  We
  * must protect from asyncrhonous cancellation between the open(), ioctl() and
  * close() operations.  */
int __unlikely
__streams_fdetach_r(const char *path)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	ret = __streams_fdetach(path);
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

/** @brief Detach a path from a Stream.
  * @param path the path in the filesystem from which to detach.
  * @version LIS_1.0
  * @par Alias:
  * This symbol is a weak alias of __streams_fdetach().
  */
int __lis_fdetach(const char *path)
    __attribute__ ((weak, alias("__streams_fdetach")));

/** @brief Detach a path from a Stream.
  * @param path the path in the filesystem from which to detach.
  * @version LIS_1.0 fdetach()
  * @par Alias:
  * This symbol is a weak alias of __streams_fdetach_r().
  */
int __lis_fdetach_r(const char *path)
    __attribute__ ((weak, alias("__streams_fdetach_r")));

/** @fn int fdetach(const char *path)
  * @brief Detach a path from a Stream.
  * @param path the path in the filesystem from which to detach.
  * @version STREAMS_1.0 __streams_fdetach_r()
  * @version LIS_1.0 __lis_fdetach_r()
  */
__asm__(".symver __streams_fdetach_r,fdetach@@STREAMS_1.0");
__asm__(".symver __lis_fdetach_r,fdetach@LIS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
