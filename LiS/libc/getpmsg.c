/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 1.1.1.3.4.5 $) $Date: 2005/07/18 11:51:27 $"

static char const ident[] =
    "$RCSfile: getpmsg.c,v $ $Name:  $($Revision: 1.1.1.3.4.5 $) $Date: 2005/07/18 11:51:27 $";

/* This file can be processed with doxygen(1). */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>		/* bad: according to POSIX, stropts.h must expose ioctl()... */
#include <stropts.h>

#include <pthread.h>

extern void __pthread_testcancel(void);

#pragma weak __pthread_testcancel
#pragma weak pthread_testcancel

#ifdef BLD32OVER64
/**
 * @struct strbuf6
 * @date 2006-02-20
 * @author bidulock@openss7.org
 *
 * This is really, really, the wrong way to do this.  I cannot stress how wrong
 * this is.  This does not provide 32/64 bit compatibility at all.  It simply
 * attempts to turn a 32 bit call into a 64 bit call an passes it to the kernel.
 * A big problem is the pointer conversion (32->64), which is architecture
 * specific.  (Some need (void *)(long)(int32_t), some need (void
 * *)(ulong)(uint32_t), others might need to supply an offset.) For example of
 * how wrong this is: applications built on s390 using static libraries will not
 * run on s390x.  The proper way to do this is as it is done under Linux
 * Fast-STREAMS: leave the library alone and handle the conversion in the
 * kernel.  When performed by syscall, the syscall32 interface must perform the
 * conversion.  When performed by read/write call, the "magic length" must
 * indicate whether conversion is necessary.  When performed by ioctl, the 32/64
 * bit ioctl conversion functions must do the job.  I really have no desire to
 * "fix" LiS in this regard.  It is deprecated: don't use it.  Use Linux
 * Fast-STREAMS instead.
 */
#endif

#ifdef BLD32OVER64
typedef struct strbuf6 {
	int maxlen;
	int len;
	unsigned long long buf;
} strbuf6_t;

typedef struct getpmsg_args6 {
	int fd;
	unsigned long long ctl;
	unsigned long long dat;
	unsigned long long bandp;
	unsigned long long flagsp;

} getpmsg_args6_t;
#endif

void
pthread_testcancel(void)
{
	if (__pthread_testcancel)
		__pthread_testcancel();
	return;
}

int
__old_lis_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
#ifdef BLD32OVER64
	strbuf6_t ctl6;
	strbuf6_t *ptrc6 = 0;
	strbuf6_t dat6;
	strbuf6_t *ptrd6 = 0;
	getpmsg_args6_t args;
	int rc;

	if (ctlptr) {
		ctl6.maxlen = ctlptr->maxlen;
		ctl6.len = ctlptr->len;
		ctl6.buf = (unsigned long long) (unsigned long) ctlptr->buf;
		ptrc6 = &ctl6;
	}

	if (dataptr) {
		dat6.maxlen = dataptr->maxlen;
		dat6.len = dataptr->len;
		dat6.buf = (unsigned long long) (unsigned long) dataptr->buf;
		ptrd6 = &dat6;
	}

	memset((void *) &args, 0, sizeof(getpmsg_args6_t));
	args.fd = fd;
	args.ctl = (unsigned long long) (unsigned long) ptrc6;
	args.dat = (unsigned long long) (unsigned long) ptrd6;
	args.bandp = (unsigned long long) (unsigned long) bandp;
	args.flagsp = (unsigned long long) (unsigned long) flagsp;

	rc = read(fd, &args, LIS_GETMSG_PUTMSG_ULEN);

	if (ctlptr) {
		ctlptr->len = ctl6.len;
	}
	if (dataptr) {
		dataptr->len = dat6.len;
	}

	return (rc);
#else
	/**
	 * @date Wed Jun 22 20:56:59 MDT 2005
	 *
	 * This no longer works on FC4 2.6.11 kernel: validity checks are
	 * performed on the length before we get here.  We might as well patch
	 * this out for all kernels and use the ioctl method instead.  Emulating
	 * an system call in this fashion was foolish in the first place: the
	 * normal method for system call emulation under UNIX is with ioctl. -bb
	 */
	struct __lis_getpmsg_args {
		int fd;
		struct strbuf *ctlptr;
		struct strbuf *datptr;
		int *bandp;
		int *flagsp;
	};
	struct __lis_getpmsg_args args = {
		fd:fd,
		ctlptr:ctlptr,
		datptr:datptr,
		bandp:bandp,
		flagsp:flagsp,
	};

	return (read(fd, &args, LIS_GETMSG_PUTMSG_ULEN));
#endif
}

/** @brief get a message from a stream band.
  * @param fd a file descriptor representing the stream.
  * @param ctlptr a pointer to a strbuf structure returning the control part of the message.
  * @param datptr a pointer to a strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  */
int
__lis_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
#ifdef BLD32OVER64
	strbuf6_t ctl6;
	strbuf6_t *ptrc6 = 0;
	strbuf6_t dat6;
	strbuf6_t *ptrd6 = 0;
	getpmsg_args6_t args;
	int rc;

	if (ctlptr) {
		ctl6.maxlen = ctlptr->maxlen;
		ctl6.len = ctlptr->len;
		ctl6.buf = (unsigned long long) (unsigned long) ctlptr->buf;
		ptrc6 = &ctl6;
	}

	if (dataptr) {
		dat6.maxlen = dataptr->maxlen;
		dat6.len = dataptr->len;
		dat6.buf = (unsigned long long) (unsigned long) dataptr->buf;
		ptrd6 = &dat6;
	}

	memset((void *) &args, 0, sizeof(getpmsg_args6_t));
	args.fd = fd;
	args.ctl = (unsigned long long) (unsigned long) ptrc6;
	args.dat = (unsigned long long) (unsigned long) ptrd6;
	args.bandp = (unsigned long long) (unsigned long) bandp;
	args.flagsp = (unsigned long long) (unsigned long) flagsp;

	rc = read(fd, &args, LIS_GETMSG_PUTMSG_ULEN);

	if (ctlptr) {
		ctlptr->len = ctl6.len;
	}
	if (dataptr) {
		dataptr->len = dat6.len;
	}

	return (rc);
#else
	struct strpmsg args = { {-1, -1, NULL}, {-1, -1, NULL}, -1, -1 };
	int rc;

	if (ctlptr)
		args.ctlbuf = *ctlptr;
	if (datptr)
		args.databuf = *datptr;
	if (bandp)
		args.band = *bandp;
	if (flagsp)
		args.flags = *flagsp;
	if ((rc = ioctl(fd, I_GETPMSG, &args)) != -1) {
		if (ctlptr)
			ctlptr->len = args.ctlbuf.len;
		if (datptr)
			datptr->len = args.databuf.len;
		if (bandp)
			*bandp = args.band;
		if (flagsp)
			*flagsp = args.flags;
	}
	return (rc);
#endif
}

/** @brief get a message from a stream band.
  * @param fd a file descriptor representing the stream.
  * @param ctlptr a pointer to a strbuf structure returning the control part of the message.
  * @param datptr a pointer to a strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  *
  * getpmsg() must contain a thread cancellation point (SUS/XOPEN/POSIX).  In the
  * Linux Threads approach, this function will return EINTR if interrupted by a
  * signal.  When the function returns EINTR, the Linux Threads user should check
  * for cancellation with pthread_testcancel().  Because this function consists
  * of a single system call, asynchronous thread cancellation protection is not
  * required.
  */
int
__lis_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int ret;

	pthread_testcancel();
	ret = __lis_getpmsg(fd, ctlptr, datptr, bandp, flagsp);
	if (ret == -1)
		pthread_testcancel();
	return (ret);
}

/** @fn int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
  * @brief get a message from a stream band.
  * @param fd a file descriptor representing the stream.
  * @param ctlptr a pointer to a strbuf structure returning the control part of the message.
  * @param datptr a pointer to a strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  */
__asm__(".symver __lis_getpmsg_r,getpmsg@@LIS_1.0");

int
__old_lis_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int ret;

	pthread_testcancel();
	ret = __old_lis_getpmsg(fd, ctlptr, datptr, bandp, flagsp);
	if (ret == -1)
		pthread_testcancel();
	return (ret);
}

__asm__(".symver __old_lis_getpmsg_r,getpmsg@LIS_0.0");

// vim: ft=c com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
