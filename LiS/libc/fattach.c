/*****************************************************************************

 @(#) $Id: fattach.c,v 1.1.1.3.12.8 2007/08/14 10:47:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date: 2007/08/14 10:47:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: fattach.c,v $
 Revision 1.1.1.3.12.8  2007/08/14 10:47:13  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: fattach.c,v $ $Name:  $($Revision: 1.1.1.3.12.8 $) $Date: 2007/08/14 10:47:13 $"

static char const ident[] =
    "$RCSfile: fattach.c,v $ $Name:  $($Revision: 1.1.1.3.12.8 $) $Date: 2007/08/14 10:47:13 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @addtogroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS System Call fattach() implementation file.  */

/** @brief attach a stream to a path in a filesystem.
  * @param fd the file descriptor of the stream to attach.
  * @param path the path in the filesystem to which to attach the stream.
  * @version LIS_1.0
  *
  * This is the non-recursive version of the implementation function.
  */
int
__lis_fattach(int fd, const char *path)
{
	return (ioctl(fd, I_LIS_FATTACH, path));
}

/** @brief attach a stream to a path in a filesystem.
  * @param fd the file descriptor of the stream to attach.
  * @param path the path in the filesystem to which to attach the stream.
  * @version LIS_1.0 fattach()
  *
  * This is the recursive version of the implementation function.
  *
  * fattach() cannot contain a thread cancellation point.  Because this
  * function contains a single system call, it is asyncrhonous thread
  * cancellation safe.
  */
int
__lis_fattach_r(int fd, const char *path)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	ret = __lis_fattach(fd, path);
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

/** @fn int fattach(int fd, const char *path)
  * @brief attach a stream to a path in a filesystem.
  * @param fd the file descriptor of the stream to attach.
  * @param path the path in the filesystem to which to attach the stream.
  * @version LIS_1.0 __lis_fattach_r()
  */
__asm__(".symver __lis_fattach_r,fattach@@LIS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
