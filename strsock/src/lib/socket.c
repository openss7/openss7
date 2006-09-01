/*****************************************************************************

 @(#) $RCSfile: socket.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/01 08:55:47 $

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

 Last Modified $Date: 2006/09/01 08:55:47 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: socket.c,v $
 Revision 0.9.2.1  2006/09/01 08:55:47  brian
 - added headers and working up code

 *****************************************************************************/

#ident "@(#) $RCSfile: socket.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/01 08:55:47 $"

static char const ident[] = "$RCSfile: socket.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/01 08:55:47 $";

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

#define NEED_T_USCALAR_T 1

#include <stdlib.h>

#include "gettext.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS 1
#endif

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <pthread.h>
#include <linux/limits.h>
#include <values.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

#include <sys/sockmod.h>
#include <sys/socksys.h>
#include <tihdr.h>
#include <timod.h>

#if defined __i386__ || defined __x86_64__ || defined __k8__
#define fastcall __attribute__((__regparm__(3)))
#else
#define fastcall
#endif

#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

#if __GNUC__ < 3
#define inline static inline fastcall __hot
#define noinline extern fastcall __unlikely
#else
#define inline static inline __attribute__((always_inline)) fastcall __hot
#define noinline static __attribute__((noinline)) fastcall __unlikely
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#undef min
#define min(a, b) (a < b ? a : b)

/**
 * @defgroup libsocket OpenSS7 Sockets Library
 * @brief OpenSS7 Sockets Library Calls
 *
 * This manual contains documentation of the OpenSS7 Sockets Library functions
 * that are generated automatically from the source code with doxygen.  This
 * documentation is intended to be used for maintianers of the OpenSS7 Sockets
 * Library and is not intended for users of the OpenSS7 Sockets Library.
 * Users should consult the documentation found in the user manual pages
 * beginning with sockets(3).
 *
 * <h2>Thread Safety</h2>
 * The OpenSS7 Sockets Library is designed to be thread-safe.  This is
 * accomplished in a number of ways.  Thread-safety depends on the use of
 * glibc2 and the pthreads library.
 *
 * Glibc2 provides lightweight thread-specific data for errno and h_errno.
 * Because h_errno uses communications functions orthoginal to the Sockets
 * Library services, we borrow h_errno and use it for s_errno.  This does not
 * cause a problem because neither h_errno nor s_errno need to maintain their
 * value across any other system call.
 *
 * Glibc2 also provides some weak undefined aliases for POSIX thread functions
 * to perform its own thread-safety.  When the pthread library (libpthread) is
 * linked with glibc2, these functions call libpthread functions instead of
 * internal dummy routines.  The same approach is taken for the OpenSS7
 * Sockets Library.  The library uses weak defined and undefined aliases that
 * automatically invoke libpthread functions when libpthread is (dynamically)
 * linked and uses dummy functions when it is not.  This maintains maximum
 * efficiency when libpthread is not dynamically linked, but provides full
 * thrad safety when it is.
 *
 * Libpthread behaves in some strange ways with regards to thread
 * cancellation.  Because libpthread uses Linux clone processes for threads,
 * cancellation of a thread is accomplished by sending a signal to the thread
 * process.  This does not directly result in the cancellation, but will
 * result in the failure of a system call with the EINTR error code.  It is
 * necessary to test for cancellation upon error return from system calles to
 * perform the actual cancellation of the thread.
 *
 * The Sockets specification (OpenGroup XNS 5.2) lists the following functions
 * as being thread cancellation points:
 *
 * Other Sockets functions are not permitted by XNS 5.2 to be thread
 * cancellation points.  Any function that cannot be a thread cancellation
 * point needs to have its cancellation status deferred if it internally
 * invokes a function that permits thread cancellation.  Functions that do not
 * permit thread cancellation are:
 *
 * Locks an asynchronous thread cancellation present challenges:
 *
 * Functions that act as thread cancellation points must push routines onto
 * the function stack executed at exit of the thread to release the locks held
 * by the function.  These are performed with weak definitions of POSIX thread
 * library functions.
 *
 * Functions that do not act as a thread cancellation point must defer thread
 * cancellation before taking locks and then release locks before thread
 * cancellation is restored.
 *
 * The above are the techniques used by glibc2 for the same purpose and is the
 * same technique that is used by the OpenSS7 Sockets Library.
 *
 * @{
 */

extern void __pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
				   void *arg);
extern void __pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute);
extern void __pthread_cleanup_push_defer(struct _pthread_cleanup_buffer *buffer,
					 void (*routine) (void *), void *arg);
extern void __pthread_cleanup_pop_restore(struct _pthread_cleanup_buffer *buffer, int execute);
extern void __pthread_testcancel(void);
extern int __pthread_setcanceltype(int type, int *oldtype);

extern int __pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr);
extern int __pthread_rwlock_rdlock(pthread_rwlock_t * rwlock);
extern int __pthread_rwlock_wrlock(pthread_rwlock_t * rwlock);
extern int __pthread_rwlock_unlock(pthread_rwlock_t * rwlock);
extern int __pthread_rwlock_destroy(pthread_rwlock_t * rwlock);

#pragma weak __pthread_cleanup_push
#pragma weak __pthread_cleanup_pop
#pragma weak __pthread_cleanup_push_defer
#pragma weak __pthread_cleanup_pop_restore
#pragma weak __pthread_testcancel
#pragma weak __pthread_setcanceltype

#pragma weak __pthread_rwlock_init
#pragma weak __pthread_rwlock_rdlock
#pragma weak __pthread_rwlock_wrlock
#pragma weak __pthread_rwlock_unlock
#pragma weak __pthread_rwlock_destroy

#pragma weak _pthread_cleanup_push
#pragma weak _pthread_cleanup_pop
#pragma weak _pthread_cleanup_push_defer
#pragma weak _pthread_cleanup_pop_restore
#pragma weak pthread_testcancel
#pragma weak pthread_setcanceltype

#pragma weak pthread_rwlock_init
#pragma weak pthread_rwlock_rdlock
#pragma weak pthread_rwlock_wrlock
#pragma weak pthread_rwlock_unlock
#pragma weak pthread_rwlock_destroy

void
_pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *), void *arg)
{
	if (__pthread_cleanup_push)
		return __pthread_cleanup_push(buffer, routine, arg);
	buffer->__routine = routine;
	buffer->__arg = arg;
	buffer->__canceltype = 0;
	buffer->__prev = NULL;
}

void
_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute)
{
	if (__pthread_cleanup_pop)
		return __pthread_cleanup_pop(buffer, execute);
	if (execute)
		(*buffer->__routine) (buffer->__arg);
}

void
_pthread_cleanup_push_defer(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
			    void *arg)
{
	if (__pthread_cleanup_push_defer)
		return __pthread_cleanup_push_defer(buffer, routine, arg);
	buffer->__routine = routine;
	buffer->__arg = arg;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &buffer->__canceltype);
	buffer->__prev = NULL;
}

void
_pthread_cleanup_pop_restore(struct _pthread_cleanup_buffer *buffer, int execute)
{
	if (__pthread_cleanup_pop_restore)
		return __pthread_cleanup_pop_restore(buffer, execute);
	if (execute)
		(*buffer->__routine) (buffer->__arg);
	pthread_setcanceltype(buffer->__canceltype, NULL);
}

void
pthread_testcancel(void)
{
	if (__pthread_testcancel)
		return __pthread_testcancel();
	return;
}

int
pthread_setcanceltype(int type, int *oldtype)
{
	if (__pthread_setcanceltype)
		return __pthread_setcanceltype(type, oldtype);
	if (oldtype)
		*oldtype = type;
	return (0);
}

int
pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr)
{
	if (__pthread_rwlock_init)
		return __pthread_rwlock_init(rwlock, attr);
	*(char *) rwlock = 0;
	return (0);
}

int
pthread_rwlock_rdlock(pthread_rwlock_t * rwlock)
{
	if (__pthread_rwlock_rdlock)
		return __pthread_rwlock_rdlock(rwlock);
	*(char *) rwlock = *(char *) rwlock + 1;
	return (0);
}

int
pthread_rwlock_wrlock(pthread_rwlock_t * rwlock)
{
	if (__pthread_rwlock_wrlock)
		return __pthread_rwlock_wrlock(rwlock);
	*(char *) rwlock = *(char *) rwlock - 1;
	return (0);
}

int
pthread_rwlock_unlock(pthread_rwlock_t * rwlock)
{
	if (__pthread_rwlock_unlock)
		return __pthread_rwlock_unlock(rwlock);
	if (*(char *) rwlock > 0)
		*(char *) rwlock = *(char *) rwlock - 1;
	else
		*(char *) rwlock = 0;
	return (0);
}

int
pthread_rwlock_destroy(pthread_rwlock_t * rwlock)
{
	if (__pthread_rwlock_destroy)
		return __pthread_rwlock_destroy(rwlock);
	*(char *) rwlock = 0xff;
	return (0);
}

int __sock_s_errno;

extern int *__h_errno_location(void);

#pragma weak __h_errno_location

#pragma weak __s_errno_location

int *
__s_errno_location(void)
{
	if (__h_errno_location != 0)
		return __h_errno_location();
	return &__sock_s_errno;
}

int *
_s_errno(void)
{
	return _s_errno_location();
}

struct _t_user {
	pthread_rwlock_t lock;		/* lock for this structure */
	int refs;			/* number of references to this structure */
	int event;			/* pending t_look() events */
	int flags;			/* user flags */
	int fflags;			/* file flags */
	int gflags;			/* getmsg flags */
	int state;			/* Socket state */
	int statef;			/* TPI state flag */
	int prim;			/* last received TPI primitive */
	int qlen;			/* length of the listen queue */
	int ocnt;			/* outstanding connection indications */
	u_int8_t moredat;		/* more data in T_DATA_IND/T_OPTDATA_IND */
	u_int8_t moresdu;		/* more tsdu */
	u_int8_t moreexp;		/* more data in T_EXDATA_IND/T_OPTDATA_IND */
	u_int8_t moreedu;		/* more etsdu */
	u_int8_t moremsg;		/* more data with dis/con/rel message */
	int ctlmax;			/* maximum size of ctlbuf */
	char *ctlbuf;			/* ctrl part buffer */
	int datmax;			/* maximum size of datbuf */
	char *datbuf;			/* data part buffer */
	uint token;			/* acceptor id */
	struct strbuf ctrl;		/* ctrl part of received message */
	struct strbuf data;		/* data part of received message */
	struct si_udata info;		/* information structure */
};

static struct _s_user *_s_fds[OPEN_MAX] = { NULL, };

/*
   Forward declarations 
 */
static int __sock_getmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int *flagsp);
static int __sock_putmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int flags);
static int __sock_putpmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int band, int flags);
static int __sock_ioctl(int fd, int cmd, void *arg);
static int __sock_strioctl(int fd, int cmd, void *arg, size_t arglen);

int __sock_accept(int fd, struct sockaddr *addr, socklen_t * len);
int __sock_bind(int fd, const struct sockaddr *addr, socklen_t len);
int __sock_connect(int fd, const struct sockaddr *addr, socklen_t len);
int __sock_getpeername(int fd, struct sockaddr *addr, socklen_t * len);
int __sock_getsockname(int fd, struct sockaddr *addr, socklen_t * len);
int __sock_getsockopt(int fd, int level, int name, void *value, socklen_t * len);
int __sock_listen(int fd, int backlog);
int __sock_setsockopt(int fd, int level, int name, const void *value, socklen_t len);
int __sock_shutdown(int fd, int how);
int __sock_socket(int domain, int type, int protocol);
int __sock_socketpair(int domain, int type, int protocol, int socket_vector[2]);

ssize_t __sock_recv(int fd, void *buf, size_t len, int flags);
ssize_t __sock_recvmsg(int fd, struct msghdr *msg, int flags);
ssize_t __sock_recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr,
			socklen_t * alen);

ssize_t __sock_send(int fd, const void *buf, size_t len, int flags);
ssize_t __sock_sendmsg(int fd, const struct msghdr *msg, int flags);
ssize_t __sock_sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
		      socklen_t alen);


static long
__sock_ioctl(int fd, const void *args, socklen_t arglen)
{
	struct strioctl ioc;

	ioc.ic_cmd = SIOCSOCKSYS;
	ioc.ic_timout = SOCKMOD_TIMEOUT;
	ioc.ic_len = arglen;
	ioc.ic_dp = args;
	return ioctl(fd, I_STR, &ioc);
}

int
__sock_accept(int fd, struct sockaddr *addr, socklen_t * len)
{
}

int
__sock_accept_r(int fd, struct sockaddr *addr, socklen_t * len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_accept(fd, addr, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_accept(fd, addr, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int accept(int fd, struct sockaddr *addr, socklen_t * len)
    __attribute__ ((alias("__sock_accept_r")));

int
__sock_bind(int fd, const struct sockaddr *addr, socklen_t len)
{
	struct {
		int cmd;
		int fd;
		struct sockaddr *addr;
		socklen_t len;
	} args = {
	SO_BIND, fd, addr, len};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_bind_r(int fd, const struct sockaddr *addr, socklen_t len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_bind(fd, addr, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_bind(fd, addr, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int bind(int fd, const struct sockaddr *addr, socklen_t len)
    __attribute__ ((alias("__sock_bind_r")));

int
__sock_connect(int fd, const struct sockaddr *addr, socklen_t len)
{
	struct {
		int cmd;
		int fd;
		struct sockaddr *addr;
		socklen_t len;
	} args = {
	SO_CONNECT, fd, addr, len};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_connect_r(int fd, const struct sockaddr *addr, socklen_t len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_connect(fd, addr, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_connect(fd, addr, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int connect(int fd, const struct sockaddr *addr, socklen_t len)
    __attribute__ ((alias("__sock_connect_r")));

int
__sock_getpeername(int fd, struct sockaddr *addr, socklen_t * len)
{
	struct {
		int cmd;
		int fd;
		struct sockaddr *addr;
		socklen_t *len;
	} args = {
	SO_GETPEER, fd, addr, len};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_getpeername_r(int fd, struct sockaddr *addr, socklen_t * len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_getpeername(fd, addr, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_getpeername(fd, addr, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int getpeername(int fd, struct sockaddr *addr, socklen_t * len)
    __attribute__ ((alias("__sock_getpeername_r")));

int
__sock_getsockname(int fd, struct sockaddr *addr, socklen_t * len)
{
	struct {
		int cmd;
		int fd;
		struct sockaddr *addr;
		socklen_t *len;
	} args = {
	SO_GETSOCK, fd, addr, len};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_getsockname_r(int fd, struct sockaddr *addr, socklen_t * len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_getsockname(fd, addr, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_getsockname(fd, addr, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int getsockname(int fd, struct sockaddr *addr, socklen_t * len)
    __attribute__ ((alias("__sock_getsockname_r")));

int
__sock_getsockopt(int fd, int level, int name, void *value, socklen_t * len)
{
	struct {
		int cmd;
		int fd;
		int level;
		int name;
		void *value;
		socklen_t *len;
	} args = {
	SO_GETSOCKOPT, fd, level, name, value, len};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_getsockopt_r(int fd, int level, int name, void *value, socklen_t * len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_getsockopt(fd, level, name, value, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_getsockopt(fd, level, name, value, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int getsockopt(int fd, int level, int name, void *value, socklen_t * len)
    __attribute__ ((alias("__sock_getsockopt_r")));

int
__sock_listen(int fd, int backlog)
{
	struct {
		int cmd;
		int fd;
		int backlog;
	} args = {
	SO_LISTEN, fd, backlog};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_listen_r(int fd, int backlog)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_listen(fd, backlog);
		__sock_putuser(&fd);
	} else
		ret = __libc_listen(fd, backlog);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int listen(int fd, int backlog)
    __attribute__ ((alias("__sock_listen_r")));

int
__sock_setsockopt(int fd, int level, int name, const void *value, socklen_t len)
{
	struct {
		int cmd;
		int fd;
		int level;
		int name;
		void *value;
		socklen_t len;
	} args = {
	SO_SETSOCKOPT, fd, level, name, value, len};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_setsockopt_r(int fd, int level, int name, const void *value, socklen_t len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_setsockopt(fd, level, name, value, len);
		__sock_putuser(&fd);
	} else
		ret = __libc_setsockopt(fd, level, name, value, len);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int setsockopt(int fd, int level, int name, const void *value, socklen_t len)
    __attribute__ ((alias("__sock_setsockopt_r")));

int
__sock_shutdown(int fd, int how)
{
	struct {
		int cmd;
		int fd;
		int how;
	} args = {
	SO_SHUTDOWN, fd, how};
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_shutdown_r(int fd, int how)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_shutdown(fd, how);
		__sock_putuser(&fd);
	} else
		ret = __libc_shutdown(fd, how);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int shutdown(int fd, int how)
    __attribute__ ((alias("__sock_shutdown_r")));

int
__sock_socket(int domain, int type, int protocol)
{
	struct {
		int cmd;
		int domain;
		int type;
		int protocol;
	} args = {
	SO_SOCKET, domain, type, protocol};
	/* More to do here... */
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_socket_r(int domain, int type, int protocol)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_socket(domain, type, protocol);
		__sock_putuser(&fd);
	} else
		ret = __libc_socket(domain, type, protocol);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int socket(int domain, int type, int protocol)
    __attribute__ ((alias("__sock_socket_r")));

int
__sock_socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	struct {
		int cmd;
		int domain;
		int type;
		int protocol;
		int *vector;
	} args = {
	SO_SOCKPAIR, domain, type, protocol, socket_vector};
	/* More to do here... */
	return __sock_ioctl(fd, &args, sizeof(args));
}

int
__sock_socketpair_r(int domain, int type, int protocol, int socket_vector[2])
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_socketpair(domain, type, protocol, socket_vector);
		__sock_putuser(&fd);
	} else
		ret = __libc_socketpair(domain, type, protocol, socket_vector);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

int socketpair(int domain, int type, int protocol, int socket_vector[2])
    __attribute__ ((alias("__sock_socketpair_r")));

ssize_t
__sock_recv(int fd, void *buf, size_t len, int flags)
{
	/* MSG_PEEK, MSG_OOB, or MSG_WAITALL */
	/* We should really just do a getpmsg here, with band setting
	 * according to MSG_OOB, or we can do an I_PEEK if MSG_PEEK is set. */
	/* XNS says if the MSG_WAITALL flag is not set, data will be returned
	 * only up to the end of the first message. */
}

ssize_t
__sock_recv_r(int fd, void *buf, size_t len, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_recv(fd, buf, len, flags);
		__sock_putuser(&fd);
	} else
		ret = __libc_recv(fd, buf, len, flags);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

ssize_t recv(int fd, void *buf, size_t len, int flags)
    __attribute__ ((alias("__sock_recv_r")));

ssize_t
__sock_recvmsg(int fd, struct msghdr *msg, int flags)
{
	/* MSG_PEEK, MSG_OOB, or MSG_WAITALL */
	/* We should really just do a getpmsg here, with band setting
	 * according to MSG_OOB, or we can do an I_PEEK if MSG_PEEK is set. */
	/* XNS says if the MSG_WAITALL flag is not set, data will be returned
	 * only up to the end of the first message. */
	/* Upon return we can set MSG_EOR, MSG_OOB, MSG_TRUNC or MSG_CTRUNC. */
}

ssize_t
__sock_recvmsg_r(int fd, struct msghdr *msg, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_recvmsg(fd, msg, flags);
		__sock_putuser(&fd);
	} else
		ret = __libc_recvmsg(fd, msg, flags);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

ssize_t recvmsg(int fd, struct msghdr *msg, int flags)
    __attribute__ ((alias("__sock_recvmsg_r")));

ssize_t
__sock_recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen)
{
	/* MSG_PEEK, MSG_OOB, or MSG_WAITALL */
	/* We should really just do a getpmsg here, with band setting
	 * according to MSG_OOB, or we can do an I_PEEK if MSG_PEEK is set. */
	/* XNS says if the MSG_WAITALL flag is not set, data will be returned
	 * only up to the end of the first message. */
}

ssize_t
__sock_recvfrom_r(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_recvfrom(fd, buf, len, flags, addr, alen);
		__sock_putuser(&fd);
	} else
		ret = __libc_recvfrom(fd, buf, len, flags, addr, alen);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen)
    __attribute__ ((alias("__sock_recvfrom_r")));

ssize_t
__sock_send(int fd, const void *buf, size_t len, int flags)
{
	/* flags can be MSG_EOR or MSG_OOB */
	/* we really just want to put a T_DATA_REQ, T_EXDATA_REQ or
	 * T_UNITDATA_REQ message here.  Note that the T_UNITDATA_REQ
	 * requires an address from the user structure. */
}

ssize_t
__sock_send_r(int fd, const void *buf, size_t len, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_recvfrom(fd, bug, len, flags, addr, alen);
		__sock_putuser(&fd);
	} else
		ret = __libc_recvfrom(fd, bug, len, flags, addr, alen);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

ssize_t send(int fd, const void *buf, size_t len, int flags)
    __attribute__ ((alias("__sock_send_r")));

ssize_t
__sock_sendmsg(int fd, const struct msghdr *msg, int flags)
{
	/* flags can be MSG_EOR or MSG_OOB */
	/* we really just want to put a T_OPTDATA_REQ or T_UNITDATA_REQ
	 * message here.  Note that the T_UNITDATA_REQ requires an address
	 * from the user structure. */
}

ssize_t
__sock_sendmsg_r(int fd, const struct msghdr *msg, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_sendmsg(fd, msg, flags);
		__sock_putuser(&fd);
	} else
		ret = __libc_sendmsg(fd, msg, flags);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

ssize_t sendmsg(int fd, const struct msghdr *msg, int flags)
    __attribute__ ((alias("__sock_sendmsg_r")));

ssize_t
__sock_sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
	      socklen_t alen)
{
	/* flags can be MSG_EOR or MSG_OOB */
	/* we really just want to put a T_OPTDATA_REQ or T_UNITDATA_REQ
	 * message here.  */
}

ssize_t
__sock_sendto_r(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
		socklen_t alen)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		ret = __sock_sendto(fd, buf, len, flags, addr, alen);
		__sock_putuser(&fd);
	} else
		ret = __libc_sendto(fd, buf, len, flags, addr, alen);
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
	       socklen_t alen)
    __attribute__ ((alias("__sock_sendto_r")));
