/*****************************************************************************

 @(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/09/18 01:43:44 $

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

 Last Modified $Date: 2006/09/18 01:43:44 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/09/18 01:43:44 $"

static char const ident[] = "$RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/09/18 01:43:44 $";

/* This file can be processed with doxygen(1). */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <errno.h>
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
__getpmsg_error(int fd)
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
 * @fn int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
 * @brief get a message from a stream band.
 * @param fd a file descriptor representing the stream.
 * @param ctlptr a pointer to a strbuf structure returning the control part of the message.
 * @param datptr a pointer to a strbuf structure returning the data part of the message.
 * @param bandp a pointer to the band returned for the received message.
 * @param flagsp a pointer to the flags returned for the received message.
 *
 * getpmsg() must contain a thread cancellation point (SUS/XOPEN/POSIX).  In
 * the Linux Threads approach, this function will return EINTR if interrupted
 * by a signal.  When the function returns EINTR, the Linux Threads user
 * should check for cancellation with pthread_testcancel().  Because this
 * function consists of a single system call, asynchronous thread cancellation
 * protection is not required.
 */
static inline __hot int
__old_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int err;
	struct strpmsg args;

	args.ctlbuf = ctlptr ? *ctlptr : ((struct strbuf) {
					  -1, -1, NULL});
	args.databuf = datptr ? *datptr : ((struct strbuf) {
					   -1, -1, NULL});
	args.band = bandp ? *bandp : 0;
	args.flags = flagsp ? *flagsp : 0;

	pthread_testcancel();
	if (likely((err = read(fd, &args, LFS_GETMSG_PUTMSG_ULEN)) >= 0))
	{
		if (ctlptr)
			ctlptr->len = args.ctlbuf.len;
		if (datptr)
			datptr->len = args.databuf.len;
		if (bandp)
			*bandp = args.band;
		if (flagsp)
			*flagsp = args.flags;
		return (err);
	}
	__getpmsg_error(fd);
	return (err);
}

/**
 * @fn int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
 * @brief get a message from a STREAM.
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
static inline __hot int
__getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int err;
	struct strpmsg args;

	args.ctlbuf = ctlptr ? *ctlptr : ((struct strbuf) {
					  -1, -1, NULL});
	args.databuf = datptr ? *datptr : ((struct strbuf) {
					   -1, -1, NULL});
	args.band = bandp ? *bandp : 0;
	args.flags = flagsp ? *flagsp : 0;

	pthread_testcancel();
	if (likely((err = ioctl(fd, I_GETPMSG, &args)) >= 0))
	{
		if (ctlptr)
			ctlptr->len = args.ctlbuf.len;
		if (datptr)
			datptr->len = args.databuf.len;
		if (bandp)
			*bandp = args.band;
		if (flagsp)
			*flagsp = args.flags;
		return (err);
	}
	__getpmsg_error(fd);
	return (err);
}

__hot int
__streams_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	return __getpmsg(fd, ctlptr, datptr, bandp, flagsp);
}

#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
__asm__(".symver __streams_getpmsg,getpmsg@@STREAMS_1.0");
#else
__asm__(".symver __streams_getpmsg,getpmsg@STREAMS_1.0");
#endif

__hot int
__old_streams_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	return __old_getpmsg(fd, ctlptr, datptr, bandp, flagsp);
}

#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
__asm__(".symver __old_streams_getpmsg,getpmsg@STREAMS_0.0");
#else
__asm__(".symver __old_streams_getpmsg,getpmsg@@STREAMS_0.0");
#endif

int __lis_getpmsg(int, struct strbuf *, struct strbuf *, int *, int *)
	__attribute__((weak, alias("__streams_getpmsg")));

int __lis_getpmsg_r(int, struct strbuf *, struct strbuf *, int *, int *)
	__attribute__((weak, alias("__streams_getpmsg")));

__asm__(".symver __lis_getpmsg_r,getpmsg@LIS_1.0");

int __old_lis_getpmsg(int, struct strbuf *, struct strbuf *, int *, int *)
	__attribute__((weak, alias("__old_streams_getpmsg")));

int __old_lis_getpmsg_r(int, struct strbuf *, struct strbuf *, int *, int *)
	__attribute__((weak, alias("__old_streams_getpmsg")));

__asm__(".symver __old_lis_getpmsg_r,getpmsg@LIS_0.0");

__hot int
__streams_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return __getpmsg(fd, ctlptr, datptr, &band, flagsp);
}
#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
__asm__(".symver __streams_getmsg,getmsg@@STREAMS_1.0");
#else
__asm__(".symver __streams_getmsg,getmsg@STREAMS_1.0");
#endif

__hot int
__old_streams_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return __old_getpmsg(fd, ctlptr, datptr, &band, flagsp);
}
#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
__asm__(".symver __old_streams_getmsg,getmsg@STREAMS_0.0");
#else
__asm__(".symver __old_streams_getmsg,getmsg@@STREAMS_0.0");
#endif

int __lis_getmsg(int, struct strbuf *, struct strbuf *, int *)
	__attribute__((weak, alias("__streams_getmsg")));

int __lis_getmsg_r(int, struct strbuf *, struct strbuf *, int *)
	__attribute__((weak, alias("__streams_getmsg")));

__asm__(".symver __lis_getmsg_r,getmsg@LIS_1.0");

int __old_lis_getmsg(int, struct strbuf *, struct strbuf *, int *)
	__attribute__((weak, alias("__old_streams_getmsg")));

int __old_lis_getmsg_r(int, struct strbuf *, struct strbuf *, int *)
	__attribute__((weak, alias("__old_streams_getmsg")));

__asm__(".symver __old_lis_getmsg_r,getmsg@LIS_0.0");
