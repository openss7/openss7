/*****************************************************************************

 @(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/12/14 16:30:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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

 Last Modified $Date: 2005/12/14 16:30:05 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/12/14 16:30:05 $"

static char const ident[] =
    "$RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/12/14 16:30:05 $";

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stropts.h>

#include <pthread.h>
#include <errno.h>

extern void __pthread_testcancel(void);

#pragma weak __pthread_testcancel
#pragma weak pthread_testcancel

void
pthread_testcancel(void)
{
	if (__pthread_testcancel)
		__pthread_testcancel();
	return;
}

/**
 * @fn int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
 * @ingroup libLiS
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
static int
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
#if defined HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
	if ((err = ioctl(fd, I_GETPMSG, &args)) >= 0)
#else
	if ((err = read(fd, &args, LFS_GETMSG_PUTMSG_ULEN)) >= 0)
#endif
	{
		if (ctlptr)
			ctlptr->len = args.ctlbuf.len;
		if (datptr)
			datptr->len = args.databuf.len;
		if (bandp)
			*bandp = args.band;
		if (flagsp)
			*flagsp = args.flags;
	} else {
		int __olderrno;
		/* If we get an EINVAL error back it is likely due to a bad ioctl, in which case
		   this is not a Stream, so we need to check if it is a Stream and fix up the error
		   code.  We get EINTR for a controlling terminal. */
		if ((__olderrno = errno) == EINVAL || __olderrno == EINTR || __olderrno == ENOTTY)
			errno = (ioctl(fd, I_ISASTREAM) == -1) ? ENOSTR : __olderrno;
	}
	return (err);
}

int
getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int ret;

	pthread_testcancel();
	ret = __getpmsg(fd, ctlptr, datptr, bandp, flagsp);
	if (ret == -1)
		pthread_testcancel();
	return (ret);
}
