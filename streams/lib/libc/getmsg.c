/*****************************************************************************

 @(#) $RCSfile: getmsg.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/09/24 21:57:02 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 2.1 of the License.

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

 Last Modified $Date: 2006/09/24 21:57:02 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: getmsg.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/09/24 21:57:02 $"

static char const ident[] =
    "$RCSfile: getmsg.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/09/24 21:57:02 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

extern int __streams_getpmsg(int, struct strbuf *, struct strbuf *, int *, int *);
extern int __streams_getpmsg_r(int, struct strbuf *, struct strbuf *, int *, int *);
extern int __old_streams_getpmsg(int, struct strbuf *, struct strbuf *, int *, int *);
extern int __old_streams_getpmsg_r(int, struct strbuf *, struct strbuf *, int *, int *);

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS Library getmsg() implementation file.  */

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_1.0
  *
  * This function is a non-thread-safe implementation of getmsg().
  *
  * getmsg()
  * can be simply emulated with a call to
  * getpmsg().
  */
__hot int
__streams_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return __streams_getpmsg(fd, ctlptr, datptr, &band, flagsp);
}

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_1.0 getmsg()
  *
  * This function is a thread-safe implementation of getmsg().
  *
  * getmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even though
  * no asynchronous thread deferral has been performed.
  *
  * In the Linux Threads approach, this function will return EINTR if
  * interrupted by a signal.  When the function returns EINTR, the Linux
  * Threads user should check for cancellation with pthread_testcancel().
  * Because this function consists of a single system call, asynchronous
  * thread cancellation protection is not required.
  *
  * getmsg()
  * can be simply emulated with a call to
  * getpmsg().
  */
__hot int
__streams_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return __streams_getpmsg_r(fd, ctlptr, datptr, &band, flagsp);
}

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_0.0
  *
  * This function is an old-approach, non-thread-safe implementation of getmsg().
  *
  * getmsg()
  * can be simply emulated with a call to
  * getpmsg().
  */
__hot int
__old_streams_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return __old_streams_getpmsg(fd, ctlptr, datptr, &band, flagsp);
}

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_0.0 getmsg()
  *
  * This function is an old-approach, thread-safe implementation of getmsg().
  *
  * getmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even
  * though no asynchronous thread deferral has been performed.
  */
__hot int
__old_streams_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return __old_streams_getpmsg_r(fd, ctlptr, datptr, &band, flagsp);
}

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_1.0
  * @par Alias:
  * This symbol is a weak alias of __streams_getmsg().
  */
int __lis_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
    __attribute__ ((weak, alias("__streams_getmsg")));

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_1.0 getmsg()
  * @par Alias:
  * This symbol is a weak alias of __streams_getmsg_r().
  */
int __lis_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
    __attribute__ ((weak, alias("__streams_getmsg_r")));

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_0.0
  * @par Alias:
  * This symbol is a weak alias of __old_streams_getmsg().
  */
int __old_lis_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
    __attribute__ ((weak, alias("__old_streams_getmsg")));

/** @brief Get a message from a Stream.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_0.0 getmsg()
  * @par Alias:
  * This symbol is a weak alias of __old_streams_getmsg_r().
  */
int __old_lis_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
    __attribute__ ((weak, alias("__old_streams_getmsg_r")));

/** @fn int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_1.0 __streams_getmsg_r()
  * @version STREAMS_0.0 __old_streams_getmsg_r()
  * @version LIS_1.0 __lis_getmsg_r()
  * @version LIS_0.0 __old_lis_getmsg_r()
  */
__asm__(".symver __streams_getmsg_r,getmsg@@STREAMS_1.0");
__asm__(".symver __old_streams_getmsg_r,getmsg@STREAMS_0.0");
__asm__(".symver __lis_getmsg_r,getmsg@LIS_1.0");
__asm__(".symver __old_lis_getmsg_r,getmsg@LIS_0.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
