/*****************************************************************************

 @(#) $RCSfile: putmsg.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/09/18 13:52:52 $

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

 Last Modified $Date: 2006/09/18 13:52:52 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: putmsg.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/09/18 13:52:52 $"

static char const ident[] = "$RCSfile: putmsg.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/09/18 13:52:52 $";

#include <sys/types.h>
#include <stropts.h>

#if __GNUC__ < 3
#define inline inline
#define noinline extern
#else
#define inline __attribute__((always_inline))
#define noinline static __attribute__((noinline))
#endif
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

int __streams_putpmsg(int, const struct strbuf *, const struct strbuf *, int, int);
int __old_streams_putpmsg(int, const struct strbuf *, const struct strbuf *, int, int);

/**
 * @addtogroup strcalls
 * @fn int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
 * @brief put a message to a stream band.
 * @param fd a file descriptor representing the stream.
 * @param ctlptr a pointer to a strbuf structure describing the control part of the message.
 * @param datptr a pointer to a strbuf structure describing the data part of the message.
 * @param flags the priority of the message.
 *
 * This function is a thread cancellation point.
 */
int
__streams_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	return __streams_putpmsg(fd, ctlptr, datptr, -1, flags);
}

#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
__asm__(".symver __streams_putmsg,putmsg@@STREAMS_1.0");
#else
__asm__(".symver __streams_putmsg,putmsg@STREAMS_1.0");
#endif

int
__old_streams_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	return __old_streams_putpmsg(fd, ctlptr, datptr, -1, flags);
}

#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
__asm__(".symver __old_streams_putmsg,putmsg@STREAMS_0.0");
#else
__asm__(".symver __old_streams_putmsg,putmsg@@STREAMS_0.0");
#endif

int __lis_putmsg(int, const struct strbuf *, const struct strbuf *, int)
	__attribute__((weak, alias("__streams_putmsg")));

int __lis_putmsg_r(int, const struct strbuf *, const struct strbuf *, int)
	__attribute__((weak, alias("__streams_putmsg")));

__asm__(".symver __lis_putmsg_r,putmsg@LIS_1.0");

int __old_lis_putmsg(int, const struct strbuf *, const struct strbuf *, int)
	__attribute__((weak, alias("__old_streams_putmsg")));

int __old_lis_putmsg_r(int, const struct strbuf *, const struct strbuf *, int)
	__attribute__((weak, alias("__old_streams_putmsg")));

__asm__(".symver __old_lis_putmsg_r,putmsg@LIS_0.0");
