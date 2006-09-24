/*****************************************************************************

 @(#) $Id: stropts.h,v 0.9.2.14 2006/09/24 21:57:00 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (C) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/09/24 21:57:00 $ by $Author: brian $

 *****************************************************************************/

#ifndef _STROPTS_H
#define _STROPTS_H
#define _LIS_STROPTS_H

#ident "@(#) $Name:  $($Revision: 0.9.2.14 $) Copyright (c) 1997-2005  Open SS7 Corporation"

/* This file can be processed with doxygen(1). */

#include <sys/stropts.h>

#include <sys/ioctl.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/** @addtogroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * @brief Defines the interface to STREAMS(9) character device special files.
  */

/** @name STREAMS System Calls
  * System calls that are defined for STREAMS.
  * @{ */

/** Perform the I/O control operation specified by request on fd.
  * One argument may follow; its presence and type depend on request.  Return
  * value depends on request.  Usually -1 indicates error.
  */
extern int ioctl(int fd, unsigned long int request, ...);

/** Test whether fd is associated with a STREAM-based file. */
extern int isastream(int fd);

/** Receive next message from a STREAMS file. */
extern int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp);

/** Receive next message from a STREAMS file, with flagsp controlling which message. */
extern int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp);

/** Send a message on a STREAMS file. */
extern int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags);

/** Send a message on a STREAMS file to the specified band. */
extern int putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr,
		   int band, int flags);

/** Attach a STREAMS-based file descriptor, fd, to a file, path, in the file system name space. */
extern int fattach(int fd, const char *path);

/** Detach a name, path, from a STREAMS-based file descriptor. */
extern int fdetach(const char *path);

/** Create a one-way communication channel (pipe).
  * If successful, two file descriptors are stored in fds; bytes written on
  * fds[1] can be read from fds[0].  Returns 0 if successful, -1 if not.
  */
extern int pipe(int fds[2]);

/** @} */

/** @name STREAMS System Calls - Non-Thread-Safe
  * Non-thread-safe versions of the system calls for STREAMS.
  * @{ */
extern int __streams_fattach(int fd, const char *path);
extern int __streams_fdetach(const char *path);
extern int __streams_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp);
extern int __streams_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp,
			     int *flagsp);
extern int __streams_isastream(int fd);
extern int __streams_pipe(int fds[2]);
extern int __streams_putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr,
			    int flags);
extern int __streams_putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr,
			     int band, int flags);
/** @} */

/** @name STREAMS System Calls - Thread-Safe
  * Thread-safe versions of the system calls for STREAMS.
  * @{ */
extern int __streams_fattach_r(int fd, const char *path);
extern int __streams_fdetach_r(const char *path);
extern int __streams_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp);
extern int __streams_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp,
			       int *flagsp);
extern int __streams_isastream_r(int fd);
extern int __streams_pipe_r(int fds[2]);
extern int __streams_putmsg_r(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr,
			      int flags);
extern int __streams_putpmsg_r(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr,
			       int band, int flags);
/** @} */

/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _STROPTS_H */

// vim: ft=cpp com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
