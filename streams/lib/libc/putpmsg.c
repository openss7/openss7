/*****************************************************************************

 @(#) $RCSfile: putpmsg.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/07/24 09:01:14 $

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

 Last Modified $Date: 2006/07/24 09:01:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: putpmsg.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/07/24 09:01:14 $"

static char const ident[] =
    "$RCSfile: putpmsg.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/07/24 09:01:14 $";

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <sys/types.h>
#include <unistd.h>
#include <stropts.h>

#include <pthread.h>
#include <errno.h>

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

extern void pthread_testcancel(void);

noinline __unlikely void
__putpmsg_error(int fd)
{
	int __olderrno;

	/* If we get an EINVAL error back it is likely due to a bad ioctl, in which case this is
	   not a Stream, so we need to check if it is a Stream and fix up the error code.  We get
	   EINTR for a controlling terminal. */
	if ((__olderrno = errno) == EINVAL || __olderrno == EINTR || __olderrno == ENOTTY)
		errno = (ioctl(fd, I_ISASTREAM) == -1) ? ENOSTR : __olderrno;
	pthread_testcancel();
}

/**
 * @fn int putpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int band, int flags)
 * @ingroup libLiS
 * @brief put a message to a stream band.
 * @param fd a file descriptor representing the stream.
 * @param ctlptr a pointer to a strbuf structure describing the control part of the message.
 * @param datptr a pointer to a strbuf structure describing the data part of the message.
 * @param band the band to which to put the message.
 * @param flags the priority of the message.
 *
 * putpmsg() must contain a thread cancellation point (SUS/XOPEN/POSIX).  In
 * the Linux Threads approach, this function will return EINTR if interrupted
 * by a signal.  When the function returns EINTR, the Linux Threads user
 * should check for cancellation with pthread_testcancel().  Because this
 * function consists of a single system call, asynchronous thread cancellation
 * protection is not required.
 */
static inline __hot int
__putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int band, int flags)
{
	int err;
	struct strpmsg args;

	args.ctlbuf = ctlptr ? *ctlptr : ((struct strbuf) {
					  -1, -1, NULL});
	args.databuf = datptr ? *datptr : ((struct strbuf) {
					   -1, -1, NULL});
	args.band = band;
	args.flags = flags;

	pthread_testcancel();
#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
	if (likely((err = ioctl(fd, I_PUTPMSG, &args)) >= 0))
#else
	if (likely((err = write(fd, &args, LFS_GETMSG_PUTMSG_ULEN)) >= 0))
#endif
		return (err);
	__putpmsg_error(fd);
	return (err);
}

__hot int
__streams_putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int band,
		  int flags)
{
	return __putpmsg(fd, ctlptr, datptr, band, flags);
}

__asm__(".symver __streams_putpmsg,putpmsg@@STREAMS_1.0");

/**
 *
 * @fn int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
 * @ingroup libLiS
 * @brief put a message to a stream band.
 * @param fd a file descriptor representing the stream.
 * @param ctlptr a pointer to a strbuf structure describing the control part of the message.
 * @param datptr a pointer to a strbuf structure describing the data part of the message.
 * @param flags the priority of the message.
 *
 * This function is a thread cancellation point.
 */
__hot int
__streams_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
{
	return __putpmsg(fd, ctlptr, datptr, -1, flags);
}

__asm__(".symver __streams_putmsg,putmsg@@STREAMS_1.0");
