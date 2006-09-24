/*****************************************************************************

 @(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/09/22 21:21:19 $

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

 Last Modified $Date: 2006/09/22 21:21:19 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/09/22 21:21:19 $"

static char const ident[] =
    "$RCSfile: getpmsg.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/09/22 21:21:19 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * STREAMS Library getpmsg() implementation file.  */

/** @brief Handle errors appropriately.
  * @param fd the file descriptor associated with the error.
  *
  * Checks the error in errno, and if the error is a set of errors that indicate
  * that fd might not be a STREAMS character device special file, test the file
  * descriptor with #I_ISASTREAM.  If fd is not a Stream, all errors that would
  * indicate such are converted to {ENOSTR}.  Otherwise, we test for
  * cancellation and return with the original error in errno.
  *
  * getpmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even though
  * no asynchronous thread deferral has been performed.
  */
noinline __unlikely void
__getpmsg_error(int fd)
{
	int __olderrno;

	/** If we get an EINVAL error back it is likely due to a bad ioctl, in
	  * which case this is not a Stream, so we need to check if it is a
	  * Stream and fix up the error code.  We get EINTR for a controlling
	  * terminal. */
	if ((__olderrno = errno) == EINVAL || __olderrno == EINTR || __olderrno == ENOTTY)
		errno = (ioctl(fd, I_ISASTREAM) == -1) ? ENOSTR : __olderrno;
}

/** @brief Old getpmsg() approach.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  *
  * This is the older kernel implementation that uses read()/write() to
  * emulate a system call.  The older kernels do not support ioctl_unlocked()
  * or ioctl_compat() and this was the best way to emulate system calls on
  * older kernels.
  *
  * Under the old approach, a specialized length argument,
  * %LFS_GETMSG_PUTMSG_ULEN was passed to the read(2) system call with a
  * buffer pointer pointing to the argumnets of the getpmsg() call combined
  * into a struct strpmsg structure.  When the Stream head read function
  * recognizes the invalid length argument, it processes the call a getpmsg()
  * instead of read(2).
  *
  * Around Linux kernel 2.6.14, the read()/write() length argument is checked
  * by system code for validity before being passed to the character device
  * driver.  This made this approach no longer workable.  The only advantage
  * of the approach over ioctl() was that ioctl() took the big kernel lock
  * whereas read()/write() did not.  On or about 2.6.14, ioctl_unlocked() was
  * added to the kernel to permit unlocked input-output controls, destroying
  * any advantage that the old approach may have had.
  *
  * One difficulty with the old approach was 32-bit compatibility for 64-bit
  * kernels.  The magic length used by LiS-2.18 was only 32-bits long, whereas
  * the size_t argument is 64-bit on 64-bit calls and 32-bit on 32-bit calls.
  * Linux Fast-STREAMS used a 64-bit magic length which is truncated to 32-bit
  * when a 32-bit call is made to a 64-bit kernel, allowing strread() in the
  * Stream head to distriguish between 32-bit calls and 64-bit calls.  ioctl()
  * is much easier, especially since ioctl_compat() was added to the kernel.
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

	if (likely((err = read(fd, &args, LFS_GETMSG_PUTMSG_ULEN)) >= 0)) {
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
	return (err);
}

/** @brief New getpmsg() approach.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  *
  * This is the newer kernel implemetnation that uses ioctl() to emulate a
  * system call.  The newer kernels support ioctl_unlocked() and this is quite
  * efficient.  This is the only way that works on newer kernels and the old
  * method will fail.
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

	if (likely((err = ioctl(fd, I_GETPMSG, &args)) >= 0)) {
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
	return (err);
}

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_1.0
  *
  * This function is a new-approach, non-thread-safe implementation of getpmsg().
  */
__hot int
__streams_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int err;

	if (unlikely((err = __getpmsg(fd, ctlptr, datptr, bandp, flagsp)) == -1))
		__getpmsg_error(fd);
	return (err);
}

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_1.0 getpmsg()
  *
  * This function is a new-approach, thread-safe implementations of getpmsg().
  *
  * getpmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even though
  * no asynchronous thread deferral has been performed.
  *
  * In the Linux Threads approach, this function will return EINTR if
  * interrupted by a signal.  When the function returns EINTR, the Linux
  * Threads user should check for cancellation with pthread_testcancel().
  * Because this function consists of a single system call, asynchronous
  * thread cancellation protection is not required.
  */
__hot int
__streams_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int oldtype = 0;
	int err;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	if (unlikely((err = __getpmsg(fd, ctlptr, datptr, bandp, flagsp)) == -1)) {
		pthread_testcancel();
		__getpmsg_error(fd);
		pthread_testcancel();
	}
	pthread_setcanceltype(oldtype, NULL);
	return (err);
}

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_0.0
  *
  * This function is an old-approach, non-thread-safe implementation of getpmsg().
  */
__hot int
__old_streams_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
{
	int err;

	if (unlikely((err = __old_getpmsg(fd, ctlptr, datptr, bandp, flagsp)) == -1))
		__getpmsg_error(fd);
	return (err);
}

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_0.0 getpmsg()
  *
  * This function is an old-approach, thread-safe implementation of getpmsg().
  *
  * getpmsg() @e must contain a thread cancellation point (SUS/XOPEN/POSIX).
  * Therefore, this function includes a call to pthread_testcancel() even
  * though no asynchronous thread deferral has been performed.
  */
__hot int
__old_streams_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp,
			int *flagsp)
{
	int oldtype = 0;
	int err;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	if (unlikely((err = __old_getpmsg(fd, ctlptr, datptr, bandp, flagsp)) == -1)) {
		pthread_testcancel();
		__getpmsg_error(fd);
		pthread_testcancel();
	}
	pthread_setcanceltype(oldtype, NULL);
	return (err);
}

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_1.0
  * @par Alias:
  * This symbol is a weak alias of __streams_getpmsg().
  */
int __lis_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
    __attribute__ ((weak, alias("__streams_getpmsg")));

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_1.0 getpmsg()
  * @par Alias:
  * This symbol is a weak alias of __streams_getpmsg_r().
  */
int __lis_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
    __attribute__ ((weak, alias("__streams_getpmsg_r")));

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_0.0
  * @par Alias:
  * This symbol is a weak alias of __old_streams_getpmsg().
  */
int __old_lis_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
    __attribute__ ((weak, alias("__old_streams_getpmsg")));

/** @brief Get a message from a Stream band.
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version LIS_0.0 getpmsg()
  * @par Alias:
  * This symbol is a weak alias of __old_streams_getpmsg_r().
  */
int __old_lis_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
    __attribute__ ((weak, alias("__old_streams_getpmsg_r")));

/** @fn int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
  * @param fd a file descriptor representing the Stream.
  * @param ctlptr a pointer to a struct strbuf structure returning the control part of the message.
  * @param datptr a pointer to a struct strbuf structure returning the data part of the message.
  * @param bandp a pointer to the band returned for the received message.
  * @param flagsp a pointer to the flags returned for the received message.
  * @version STREAMS_1.0 __streams_getpmsg_r()
  * @version STREAMS_0.0 __old_streams_getpmsg_r()
  * @version LIS_1.0 __lis_getpmsg_r()
  * @version LIS_0.0 __old_lis_getpmsg_r()
  */
__asm__(".symver __streams_getpmsg_r,getpmsg@@STREAMS_1.0");
__asm__(".symver __old_streams_getpmsg_r,getpmsg@STREAMS_0.0");
__asm__(".symver __lis_getpmsg_r,getpmsg@LIS_1.0");
__asm__(".symver __old_lis_getpmsg_r,getpmsg@LIS_0.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
