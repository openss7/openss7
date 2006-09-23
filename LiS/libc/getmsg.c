/*****************************************************************************

 @(#) $RCSfile: getmsg.c,v $ $Name:  $($Revision: 1.1.1.1.12.2 $) $Date: 2005/04/12 22:45:29 $

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

#ident "@(#) $RCSfile: getmsg.c,v $ $Name:  $($Revision: 1.1.1.1.12.2 $) $Date: 2005/04/12 22:45:29 $"

static char const ident[] =
    "$RCSfile: getmsg.c,v $ $Name:  $($Revision: 1.1.1.1.12.2 $) $Date: 2005/04/12 22:45:29 $";

/* This file can be processed with doxygen(1). */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <sys/types.h>
#include <stropts.h>
#include <unistd.h>

int __lis_getpmsg(int, struct strbuf *, struct strbuf *, int *, int *);
int __lis_getpmsg_r(int, struct strbuf *, struct strbuf *, int *, int *);

/** @brief get a message from a STREAM.
  * @param fd a file descriptor for the stream.
  * @param ctlptr a pointer to a struct strbuf structure that returns the
  * control part of the retrieved message.
  * @param datptr a pointer to a struct strbuf structuer that returns the data
  * part of the retrieved message.
  * @param flagsp a pointer to an integer flags word that returns the priority
  * of the retrieved message.
  */
int
__lis_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	return __lis_getpmsg(fd, ctlptr, datptr, NULL, flagsp);
}

/** @brief get a message from a STREAM.
  * @param fd a file descriptor for the stream.
  * @param ctlptr a pointer to a struct strbuf structure that returns the
  * control part of the retrieved message.
  * @param datptr a pointer to a struct strbuf structuer that returns the data
  * part of the retrieved message.
  * @param flagsp a pointer to an integer flags word that returns the priority
  * of the retrieved message.
  *
  * getmsg() must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Because getmsg consists of a single call to getpmsg() which has the same
  * characteristics, no protection against asynchronous thread cancellation is
  * required.
  */
int
__lis_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	return __lis_getpmsg_r(fd, ctlptr, datptr, NULL, flagsp);
}

/** @fn int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
  * @brief get a message from a STREAM.
  * @param fd a file descriptor for the stream.
  * @param ctlptr a pointer to a struct strbuf structure that returns the
  * control part of the retrieved message.
  * @param datptr a pointer to a struct strbuf structuer that returns the data
  * part of the retrieved message.
  * @param flagsp a pointer to an integer flags word that returns the priority
  * of the retrieved message.
  */
__asm__(".symver __lis_getmsg_r,getmsg@@LIS_1.0");

int __old_lis_getpmsg(int, struct strbuf *, struct strbuf *, int *, int *);
int __old_lis_getpmsg_r(int, struct strbuf *, struct strbuf *, int *, int *);

int
__old_lis_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	return __old_lis_getpmsg(fd, ctlptr, datptr, NULL, flagsp);
}

int
__old_lis_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	return __old_lis_getpmsg_r(fd, ctlptr, datptr, NULL, flagsp);
}

__asm__(".symver __old_lis_getmsg_r,getmsg@LIS_0.0");

// vim: ft=c com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
