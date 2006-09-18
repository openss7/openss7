/*****************************************************************************

 @(#) $Id: socklib.h,v 0.9.2.2 2006/09/18 13:52:42 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/09/18 13:52:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: socklib.h,v $
 Revision 0.9.2.2  2006/09/18 13:52:42  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:10:35  brian
 - added libsocket source files and manuals

 *****************************************************************************/

#ifndef __SYS_SOCKLIB_H__
#define __SYS_SOCKLIB_H__

#ident "@(#) $RCSfile: socklib.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

int accept(int fd, struct sockaddr *addr, socklen_t * len);
int bind(int fd, const struct sockaddr *addr, socklen_t len);
int connect(int fd, const struct sockaddr *addr, socklen_t len);
int getpeername(int fd, struct sockaddr *addr, socklen_t * len);
int getsockname(int fd, struct sockaddr *addr, socklen_t * len);
int getsockopt(int fd, int level, int name, void *value, socklen_t * len);
int listen(int fd, int backlog);
int setsockopt(int fd, int level, int name, const void *value, socklen_t len);
int shutdown(int fd, int how);
int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);

ssize_t recv(int fd, void *buf, size_t len, int flags);
ssize_t recvmsg(int fd, struct msghdr *msg, int flags);
ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen);

ssize_t send(int fd, const void *buf, size_t len, int flags);
ssize_t sendmsg(int fd, const struct msghdr *msg, int flags);
ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t alen);

#endif				/* __SYS_SOCKLIB_H__ */
