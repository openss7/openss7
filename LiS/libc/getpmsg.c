/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stropts.h>

#include <pthread.h>

extern void __pthread_testcancel(void);

#pragma weak __pthread_testcancel
#pragma weak pthread_testcancel

#ifdef BLD32OVER64
typedef struct strbuf6 {
	int maxlen;
	int len;
	long long buf;
} strbuf6_t;

typedef struct getpmsg_args6 {
	int fd;
	long long ctl;
	long long dat;
	long long bandp;
	long long flagsp;

} getpmsg_args6_t;
#endif

void
pthread_testcancel(void)
{
	if (__pthread_testcancel)
		__pthread_testcancel();
	return;
}

static int
__getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
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
		ctl6.buf = (long long) ctlptr->buf;
		ptrc6 = &ctl6;
	}

	if (dataptr) {
		dat6.maxlen = dataptr->maxlen;
		dat6.len = dataptr->len;
		dat6.buf = (long long) dataptr->buf;
		ptrd6 = &dat6;
	}

	memset((void *) &args, 0, sizeof(getpmsg_args6_t));
	args.fd = fd;
	args.ctl = (long long) ptrc6;
	args.dat = (long long) ptrd6;
	args.bandp = (long long) bandp;
	args.flagsp = (long long) flagsp;

	rc = read(fd, &args, LIS_GETMSG_PUTMSG_ULEN);

	if (ctlptr) {
		ctlptr->len = ctl6.len;
	}
	if (dataptr) {
		dataptr->len = dat6.len;
	}

	return (rc);
#else
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
