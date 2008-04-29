/*****************************************************************************

 @(#) $RCSfile: putmsg.c,v $ $Name:  $($Revision: 1.1.1.1.12.9 $) $Date: 2008-04-29 08:33:16 $

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

 Last Modified $Date: 2008-04-29 08:33:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: putmsg.c,v $
 Revision 1.1.1.1.12.9  2008-04-29 08:33:16  brian
 - update headers for Affero release

 Revision 1.1.1.1.12.8  2007/08/14 10:47:13  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: putmsg.c,v $ $Name:  $($Revision: 1.1.1.1.12.9 $) $Date: 2008-04-29 08:33:16 $"

static char const ident[] =
    "$RCSfile: putmsg.c,v $ $Name:  $($Revision: 1.1.1.1.12.9 $) $Date: 2008-04-29 08:33:16 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @addtogroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS System Call putmsg() implementation file.  */

int __lis_putpmsg(int, struct strbuf *, struct strbuf *, int, int);
int __lis_putpmsg_r(int, struct strbuf *, struct strbuf *, int, int);

/** @brief put a message to a stream band.
  * @param fd a file descriptor representing the stream.
  * @param ctlptr a pointer to a strbuf structure describing the control part of the message.
  * @param datptr a pointer to a strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  */
int
__lis_putmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int flags)
{
	return __lis_putpmsg(fd, ctlptr, datptr, -1, flags);
}

/** @brief put a message to a stream band.
  * @param fd a file descriptor representing the stream.
  * @param ctlptr a pointer to a strbuf structure describing the control part of the message.
  * @param datptr a pointer to a strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version LIS_1.0 putmsg()
  *
  * This function is a thread cancellation point.
  */
int
__lis_putmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int flags)
{
	return __lis_putpmsg_r(fd, ctlptr, datptr, -1, flags);
}

int __old_lis_putpmsg(int, struct strbuf *, struct strbuf *, int, int);
int __old_lis_putpmsg_r(int, struct strbuf *, struct strbuf *, int, int);

int
__old_lis_putmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int flags)
{
	return __old_lis_putpmsg(fd, ctlptr, datptr, -1, flags);
}

/** @version LIS_0.0 putmsg()
  */
int
__old_lis_putmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int flags)
{
	return __old_lis_putpmsg_r(fd, ctlptr, datptr, -1, flags);
}

/** @fn int putmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int flags)
  * @brief put a message to a stream band.
  * @param fd a file descriptor representing the stream.
  * @param ctlptr a pointer to a strbuf structure describing the control part of the message.
  * @param datptr a pointer to a strbuf structure describing the data part of the message.
  * @param flags the priority of the message.
  * @version LIS_1.0 __lis_putmsg_r()
  * @version LIS_0.0 __old_lis_putmsg_r()
  */
__asm__(".symver __lis_putmsg_r,putmsg@@LIS_1.0");
__asm__(".symver __old_lis_putmsg_r,putmsg@LIS_0.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
