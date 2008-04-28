/*****************************************************************************

 @(#) $RCSfile: putmsg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2008-04-28 12:54:03 $

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

 $Log: putmsg.c,v $
 Revision 0.9.2.14  2008-04-28 12:54:03  brian
 - update file headers for release

 Revision 0.9.2.13  2007/08/13 22:46:11  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: putmsg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2008-04-28 12:54:03 $"

static char const ident[] =
    "$RCSfile: putmsg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2008-04-28 12:54:03 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

extern int __streams_putpmsg(int, const struct strbuf *, const struct strbuf *, int, int);
extern int __streams_putpmsg_r(int, const struct strbuf *, const struct strbuf *, int, int);
extern int __old_streams_putpmsg(int, const struct strbuf *, const struct strbuf *, int, int);
extern int __old_streams_putpmsg_r(int, const struct strbuf *, const struct strbuf *, int, int);

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS Library putmsg() implementation file.  */

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version STREAMS_1.0
  *
  * This function is a non-thread-safe implementation of putmsg().
  *
  * putmsg()
  * can be simply emulated with a call to
  * putpmsg().
  */
__hot int
__streams_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	int band = -1;

	return __streams_putpmsg(fd, ctlptr, datptr, band, flags);
}

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version STREAMS_1.0 putmsg()
  *
  * This function is a thread-safe implementation of putmsg().
  *
  * putmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even though
  * no asynchronous thread deferral has been performed.
  *
  * In the Linux Threads approach, this function will return EINTR if
  * interrupted by a signal.  When the function returns EINTR, the Linux
  * Threads user should check for cancellation with pthread_testcancel().
  * Because this function consists of a single system call, asynchronous
  * thread cancellation protection is not required.
  *
  * putmsg()
  * can be simply emulated with a call to
  * putpmsg().
  */
int
__streams_putmsg_r(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	int band = -1;

	return __streams_putpmsg_r(fd, ctlptr, datptr, band, flags);
}

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version STREAMS_0.0
  *
  * This function is an old-approach, non-thread-safe implementation of putmsg().
  *
  * putmsg()
  * can be simply emulated with a call to
  * putpmsg().
  */
int
__old_streams_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	int band = -1;

	return __old_streams_putpmsg(fd, ctlptr, datptr, band, flags);
}

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version STREAMS_0.0 putmsg()
  *
  * This function is an old-approach, thread-safe implementation of putmsg().
  *
  * putmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even
  * though no asynchronous thread deferral has been performed.
  */
int
__old_streams_putmsg_r(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	int band = -1;

	return __old_streams_putpmsg_r(fd, ctlptr, datptr, band, flags);
}

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version LIS_1.0
  * @par Alias:
  * This symbol is a weak alias of __streams_putmsg().
  */
int __lis_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
    __attribute__ ((weak, alias("__streams_putmsg")));

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version LIS_1.0 putmsg()
  * @par Alias:
  * This symbol is a weak alias of __streams_putmsg_r().
  */
int __lis_putmsg_r(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
    __attribute__ ((weak, alias("__streams_putmsg_r")));

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version LIS_0.0
  * @par Alias:
  * This symbol is a weak alias of __old_streams_putmsg().
  */
int __old_lis_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
    __attribute__ ((weak, alias("__old_streams_putmsg")));

/** @brief Put a message to a stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version LIS_0.0 putmsg()
  * @par Alias:
  * This symbol is a weak alias of __old_streams_putmsg_r().
  */
int __old_lis_putmsg_r(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
    __attribute__ ((weak, alias("__old_streams_putmsg_r")));

/** @fn int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure describing the control part of the message.
  * @param datptr a pointer to a struct strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version STREAMS_1.0 __streams_putmsg_r()
  * @version STREAMS_0.0 __old_streams_putmsg_r()
  * @version LIS_1.0 __lis_putmsg_r()
  * @version LIS_0.0 __old_lis_putmsg_r()
  */
__asm__(".symver __streams_putmsg_r,putmsg@@STREAMS_1.0");
__asm__(".symver __old_streams_putmsg_r,putmsg@STREAMS_0.0");
__asm__(".symver __lis_putmsg_r,putmsg@LIS_1.0");
__asm__(".symver __old_lis_putmsg_r,putmsg@LIS_0.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
