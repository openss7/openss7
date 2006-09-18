/*****************************************************************************

 @(#) $RCSfile: socket.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/09/18 13:52:53 $

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

 Last Modified $Date: 2006/09/18 13:52:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: socket.c,v $
 Revision 0.9.2.3  2006/09/18 13:52:53  brian
 - added doxygen markers to sources

 Revision 0.9.2.2  2006/09/18 01:43:53  brian
 - working up libsocket

 Revision 0.9.2.1  2006/09/01 08:55:47  brian
 - added headers and working up code

 *****************************************************************************/

#ident "@(#) $RCSfile: socket.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/09/18 13:52:53 $"

static char const ident[] =
    "$RCSfile: socket.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/09/18 13:52:53 $";

/* This file can be processed with doxygen(1). */

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
#include <sys/socket.h>
//#include <tihdr.h>
//#include <timod.h>

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
 * This manual contains documentation of the OpenSS7 Sockets Library functions that are generated
 * automatically from the source code with doxygen.  This documentation is intended to be used for
 * maintianers of the OpenSS7 Sockets Library and is not intended for users of the OpenSS7 Sockets
 * Library.  Users should consult the documentation found in the user manual pages beginning with
 * sockets(3).
 *
 * <h2>Thread Safety</h2>
 * The OpenSS7 Sockets Library is designed to be thread-safe.  This is accomplished in a number of
 * ways.  Thread-safety depends on the use of glibc2 and the pthreads library.
 *
 * Glibc2 provides lightweight thread-specific data for errno and h_errno.  Because h_errno uses
 * communications functions orthoginal to the Sockets Library services, we borrow h_errno and use it
 * for s_errno.  This does not cause a problem because neither h_errno nor s_errno need to maintain
 * their value across any other system call.
 *
 * Glibc2 also provides some weak undefined aliases for POSIX thread functions to perform its own
 * thread-safety.  When the pthread library (libpthread) is linked with glibc2, these functions call
 * libpthread functions instead of internal dummy routines.  The same approach is taken for the
 * OpenSS7 Sockets Library.  The library uses weak defined and undefined aliases that automatically
 * invoke libpthread functions when libpthread is (dynamically) linked and uses dummy functions when
 * it is not.  This maintains maximum efficiency when libpthread is not dynamically linked, but
 * provides full thrad safety when it is.
 *
 * Libpthread behaves in some strange ways with regards to thread cancellation.  Because libpthread
 * uses Linux clone processes for threads, cancellation of a thread is accomplished by sending a
 * signal to the thread process.  This does not directly result in the cancellation, but will result
 * in the failure of a system call with the EINTR error code.  It is necessary to test for
 * cancellation upon error return from system calles to perform the actual cancellation of the
 * thread.
 *
 * The Sockets specification (OpenGroup XNS 5.2) lists the following functions as being thread
 * cancellation points:
 *
 * Other Sockets functions are not permitted by XNS 5.2 to be thread cancellation points.  Any
 * function that cannot be a thread cancellation point needs to have its cancellation status
 * deferred if it internally invokes a function that permits thread cancellation.  Functions that do
 * not permit thread cancellation are:
 *
 * Locks an asynchronous thread cancellation present challenges:
 *
 * Functions that act as thread cancellation points must push routines onto the function stack
 * executed at exit of the thread to release the locks held by the function.  These are performed
 * with weak definitions of POSIX thread library functions.
 *
 * Functions that do not act as a thread cancellation point must defer thread cancellation before
 * taking locks and then release locks before thread cancellation is restored.
 *
 * The above are the techniques used by glibc2 for the same purpose and is the same technique that
 * is used by the OpenSS7 Sockets Library.
 *
 * XNS 5.2 says that thread cancellation points shall occur when a thread is executing the following
 * functions: accept(), connect(), recv(), recvfrom(), recvmsg(), send(), sendmsg(), sendto().
 *
 * XNS 5.2 says that thread cancellation points shall not occur in: bind(), getpeername(),
 * getsockname(), getsockopt(), listen(), setsockopt(), shutdown(), socket(), socketpair().
 *
 * @{
 */

struct _s_user {
	pthread_rwlock_t lock;		/* lock for this structure */
	struct si_udata info;		/* information structure */
};

static struct _s_user *_s_fds[OPEN_MAX] = { NULL, };

static int __sock_control_fd = -1;

/*
   LIBC declarations
 */
extern int __libc_accept(int fd, struct sockaddr *addr, socklen_t * len);
extern int __libc_bind(int fd, const struct sockaddr *addr, socklen_t len);
extern int __libc_connect(int fd, const struct sockaddr *addr, socklen_t len);
extern int __libc_getpeername(int fd, struct sockaddr *addr, socklen_t * len);
extern int __libc_getsockname(int fd, struct sockaddr *addr, socklen_t * len);
extern int __libc_getsockopt(int fd, int level, int name, void *value, socklen_t * len);
extern int __libc_listen(int fd, int backlog);
extern int __libc_setsockopt(int fd, int level, int name, const void *value, socklen_t len);
extern int __libc_shutdown(int fd, int how);
extern int __libc_socket(int domain, int type, int protocol);
extern int __libc_socketpair(int domain, int type, int protocol, int socket_vector[2]);
extern int __libc_close(int fd);

extern ssize_t __libc_recv(int fd, void *buf, size_t len, int flags);
extern ssize_t __libc_recvmsg(int fd, struct msghdr *msg, int flags);
extern ssize_t __libc_recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr,
			       socklen_t * alen);

extern ssize_t __libc_send(int fd, const void *buf, size_t len, int flags);
extern ssize_t __libc_sendmsg(int fd, const struct msghdr *msg, int flags);
extern ssize_t __libc_sendto(int fd, const void *buf, size_t len, int flags,
			     const struct sockaddr *addr, socklen_t alen);

/*
   Forward declarations 
 */
static long __sock_ioctl(struct socksysreq *args);

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
int __sock_close(int fd);

ssize_t __sock_recv(int fd, void *buf, size_t len, int flags);
ssize_t __sock_recvmsg(int fd, struct msghdr *msg, int flags);
ssize_t __sock_recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr,
			socklen_t * alen);

ssize_t __sock_send(int fd, const void *buf, size_t len, int flags);
ssize_t __sock_sendmsg(int fd, const struct msghdr *msg, int flags);
ssize_t __sock_sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
		      socklen_t alen);

static pthread_rwlock_t __sock_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

inline int
__sock_lock_rdlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
inline int
__sock_lock_wrlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
inline void
__sock_lock_unlock(void *rwlock)
{
	pthread_rwlock_unlock(rwlock);
}
inline int
__sock_list_rdlock(void)
{
	return __sock_lock_rdlock(&__sock_fd_lock);
}
inline int
__sock_list_wrlock(void)
{
	return __sock_lock_wrlock(&__sock_fd_lock);
}
static void
__sock_list_unlock(void *ignore)
{
	return __sock_lock_unlock(&__sock_fd_lock);
}
inline int
__sock_user_wrlock(struct _s_user *user)
{
	return __sock_lock_wrlock(&user->lock);
}
inline void
__sock_user_unlock(struct _s_user *user)
{
	return __sock_lock_unlock(&user->lock);
}
static void
__sock_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct _s_user *user = _s_fds[fd];

	__sock_user_unlock(user);
	__sock_list_unlock(NULL);
}

/**
 * @internal
 * @brief Get a locked socket user structure.
 * @param fd the socket descriptor for which to get the associated user structure.
 *
 * This is a range-checked array lookup of the library user structure associated
 * with the specific file descriptor.  Also, this function takes the necessary
 * locks for thread-safe operation.
 */
static __hot struct _s_user *
__sock_getuser(int fd)
{
	struct _s_user *user;
	int err;

	if (unlikely((err = __sock_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto badf;
	if (unlikely(!(user = _s_fds[fd])))
		goto badf;
	if (unlikely((err = __sock_user_wrlock(user))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	errno = err;
	__sock_list_unlock(NULL);
	goto error;
      badf:
	/* At this point we check if we have a stream and if we do, we need to perform an
	   SI_GETUDATA to see if we can syncrhonize the file descriptor with the socket
	   information.  If we can, this file descriptor was created by some other mechanism, such
	   as open, t_open, or dup. */
	if (isastream(fd)) {
		struct strioctl ioc;

		if (!(user = (struct _s_user *) malloc(sizeof(*user))))
			goto enomem;
		memset(user, 0, sizeof(*user));
		switch (ioctl(fd, I_FIND, "sockmod")) {
		case -1:
		default:
			goto badfind;
		case 0:
			/* XXX: maybe we shouldn't push it if it does not already exist on the
			   emodule stack (i.e. it is a stream by not a socket. */
			if (ioctl(fd, I_PUSH, "sockmod") == -1)
				goto badpush;
			break;
		case 1:
			break;
		}
		ioc.ic_cmd = SI_GETUDATA;
		ioc.ic_dp = (char *) &user->info;
		ioc.ic_len = sizeof(struct si_udata);
		ioc.ic_timout = SOCKMOD_TIMEOUT;
		if (ioctl(fd, I_STR, &ioc) == -1)
			goto badgetuser;
		_s_fds[fd] = user;
		return (user);
	      enomem:
		errno = ENOMEM;
		goto error;
	      badgetuser:
		ioctl(fd, I_POP, NULL);
	      badfind:
	      badpush:
		free(user);
	}
	errno = EBADF;
	goto error;
      list_lock_error:
	errno = err;
	goto error;
      error:
	return (NULL);
}

static long
__sock_ioctl(struct socksysreq *args)
{
	struct strioctl ioc;

	if (__sock_control_fd == -1) {
		if ((__sock_control_fd = open("/dev/socksys", O_RDWR)) == -1) {
			fprintf(stderr, "Cannot open control socket\n");
			return (-1);
		}
	}
	ioc.ic_cmd = SIOCSOCKSYS;
	ioc.ic_timout = SOCKMOD_TIMEOUT;
	ioc.ic_len = sizeof(*args);
	ioc.ic_dp = (char *) args;
	return ioctl(__sock_control_fd, I_STR, &ioc);
}

/**
 * @fn int accept(int fd, struct sockaddr *addr, socklen_t *len)
 * @brief Accept a socket connection.
 * @param fd the socket from which to accept the connection.
 * @param addr a pointer to a sockaddr structure to receive the connecting address.
 * @param len the length of the sockaddr.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 *
 * IMPLEMENTATION: When Streams are created by the library in user space, it is necessary to open a
 * new Stream, possibly bind it to the same address as the Stream specified by fd, and then pass
 * this newly created Stream to the ioctl call.  When Streams are created by socksys(4) or
 * sockmod(4), the module or driver will open the Stream, however, there is a problem with LiS in
 * this regard in that the top module's put procedure does not necessarily execute with caller user
 * context, there is no way of hooking STREAMS ioctl (e.g. strioctl) and a Stream cannot be opened
 * without the danger of putting the STREAMS scheduler kernel thread to sleep.
 */
int
__sock_accept(int fd, struct sockaddr *addr, socklen_t * len)
{
	int err, sd;
	struct _s_user *user;
	struct socksysreq args = {
		{SO_ACCEPT, fd, (long) addr, (long) len,}
	};
	if (!(user = (struct _s_user *) malloc(sizeof(*user))))
		goto enomem;
	memset(user, 0, sizeof(*user));
	if ((sd = __sock_ioctl(&args)) == -1)
		goto badsock;
	/* try to convert it into a socket */
	if (isastream(sd)) {
		struct strioctl ioc;

		if (ioctl(sd, I_PUSH, "sockmod") == -1)
			goto badioctl;
		ioc.ic_cmd = SI_GETUDATA;
		ioc.ic_dp = (char *) &user->info;
		ioc.ic_len = sizeof(struct si_udata);
		ioc.ic_timout = SOCKMOD_TIMEOUT;
		if (ioctl(sd, I_STR, &ioc) == -1)
			goto badgetuser;
		_s_fds[sd] = user;
	} else
		_s_fds[sd] = NULL;
	return (0);
      badgetuser:
      badioctl:
	err = errno;
	_s_fds[sd] = NULL;
	close(sd);
	free(user);
	errno = err;
	return (-1);
      badsock:
	err = errno;
	free(user);
	errno = err;
	return (-1);
      enomem:
	errno = ENOMEM;
	return (-1);
}

int
__sock_accept_r(int fd, struct sockaddr *addr, socklen_t * len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_accept(fd, addr, len)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_accept(fd, addr, len)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_accept_r,accept@@SOCKET_1.0");

/**
 * @fn int bind(int fd, const struct sockaddr *addr, socklen_t len)
 * @brief Bind a socket to a name.
 * @param fd the socket to name.
 * @param addr a pointer to the sockaddr structure that contains the name.
 * @param len the length address contained in the sockaddr structure.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_bind(int fd, const struct sockaddr *addr, socklen_t len)
{
	struct socksysreq args = {
		{SO_BIND, fd, (long) addr, len,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_bind_r,bind@@SOCKET_1.0");

/**
 * @fn int connect(int fd, const struct sockaddr *addr, socklen_t len)
 * @brief Connect a socket to a transport peer.
 * @param fd the socket from which to connect.
 * @param addr a pointer to the sockaddr structure containing the address to which to connect.
 * @param len the length address contained in the sockaddr structure.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 */
int
__sock_connect(int fd, const struct sockaddr *addr, socklen_t len)
{
	struct socksysreq args = {
		{SO_CONNECT, fd, (long)addr, len,}
	};
	return __sock_ioctl(&args);
}

int
__sock_connect_r(int fd, const struct sockaddr *addr, socklen_t len)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_connect(fd, addr, len)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_connect(fd, addr, len)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_connect_r,connect@@SOCKET_1.0");

/**
 * @fn int getpeername(int fd, struct sockaddr *addr, socklen_t * len)
 * @brief Get the name of the peer socket.
 * @param fd the local socket.
 * @param addr a pointer to the sockaddr structure into which to receive the name.
 * @param len on call, the length of the sockaddr structure buffer, on return the length of the name stored in the sockaddr structure.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_getpeername(int fd, struct sockaddr *addr, socklen_t * len)
{
	struct socksysreq args = {
		{SO_GETPEERNAME, fd, (long) addr, (long) len,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_getpeername_r,getpeername@@SOCKET_1.0");

/**
 * @fn int getsockname(int fd, struct sockaddr *addr, socklen_t * len)
 * @brief Get the name of the local socket.
 * @param fd the socket from which to get the name.
 * @param addr a pointer to the sockaddr structure into which to receive the name.
 * @param len on call, the length of the sockaddr structure buffer, on return the length of the name stored in the sockaddr structure.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_getsockname(int fd, struct sockaddr *addr, socklen_t * len)
{
	struct socksysreq args = {
		{SO_GETSOCKNAME, fd, (long) addr, (long) len,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_getsockname_r,getsockname@@SOCKET_1.0");

/**
 * @fn int getsockopt(int fd, int level, int name, void *value, socklen_t * len)
 * @brief Get a socket option.
 * @param fd the socket from which to get the option.
 * @param level the protocol level for the option.
 * @param name the name of the option.
 * @param value a pointer to a buffer into which to receive the option value.
 * @param len on call, the length of the buffer, on return, the length of the returned option value.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_getsockopt(int fd, int level, int name, void *value, socklen_t * len)
{
	struct socksysreq args = {
		{SO_GETSOCKOPT, fd, level, name, (long) value, (long) len,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_getsockopt_r,getsockopt@@SOCKET_1.0");

/**
 * @fn int listen(int fd, int backlog)
 * @brief Listen on a socket.
 * @param fd the socket upon which to listen.
 * @param backlog the maximum number of outstanding (yet to be accepted) connections.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_listen(int fd, int backlog)
{
	struct socksysreq args = {
		{SO_LISTEN, fd, backlog,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_listen_r,listen@@SOCKET_1.0");

/**
 * @fn int setsockopt(int fd, int level, int name, const void *value, socklen_t len)
 * @brief Set a socket option.
 * @param fd the socket upon which to set the option.
 * @param level the protocol level of the option.
 * @param name the name of the option.
 * @param value a pointer to a buffer containing the value of the option.
 * @param len the length of the value in the buffer.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_setsockopt(int fd, int level, int name, const void *value, socklen_t len)
{
	struct socksysreq args = {
		{SO_SETSOCKOPT, fd, level, name, (long) value, len,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_setsockopt_r,setsockopt@@SOCKET_1.0");

/**
 * @fn int shutdown(int fd, int how)
 * @brief Shut down a socket for receive or transmission.
 * @param fd the socket to shut down.
 * @param how how to shut down the socket (SHUT_RD, SHUT_WR or SHUT_RW)
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_shutdown(int fd, int how)
{
	struct socksysreq args = {
		{SO_SHUTDOWN, fd, how,}
	};
	return __sock_ioctl(&args);
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

__asm__(".symver __sock_shutdown_r,shutdown@@SOCKET_1.0");

/**
 * @fn int socket(int domain, int type, int protocol)
 * @brief Create a socket.
 * @param domain the protocol domain (address or protocol family).
 * @param type the socket type (SOCK_STREAM, SOCK_DGRAM, etc.)
 * @param protocol the protocol within the domain, or zero (0) for default.
 *
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_socket(int domain, int type, int protocol)
{
	int err, sd;
	struct _s_user *user;
	struct socksysreq args = {
		{SO_SOCKET, domain, type, protocol,}
	};
	if (!(user = (struct _s_user *) malloc(sizeof(*user))))
		goto enomem;
	memset(user, 0, sizeof(*user));
	if ((sd = __sock_ioctl(&args)) == -1)
		goto badsock;
	/* try to convert it into a socket */
	if (isastream(sd)) {
		struct strioctl ioc;

		if (ioctl(sd, I_PUSH, "sockmod") == -1)
			goto badioctl;
		ioc.ic_cmd = O_SI_GETUDATA;
		ioc.ic_dp = (char *) &user->info;
		ioc.ic_len = sizeof(struct o_si_udata);
		ioc.ic_timout = SOCKMOD_TIMEOUT;
		if (ioctl(sd, I_STR, &ioc) == -1)
			goto badgetuser;
		user->info.sockparams.sp_family = domain;
		user->info.sockparams.sp_type = type;
		user->info.sockparams.sp_protocol = protocol;
		_s_fds[sd] = user;
	} else
		_s_fds[sd] = NULL;
	return (0);
      badgetuser:
      badioctl:
	err = errno;
	_s_fds[sd] = NULL;
	close(sd);
	free(user);
	errno = err;
	return (-1);
      badsock:
	err = errno;
	free(user);
	errno = err;
	return (-1);
      enomem:
	errno = ENOMEM;
	return (-1);
}

/**
 * @fn int socket(int domain, int type, int protocol)
 * @brief recursive socket function.
 * @param domain protocol family.
 * @param type socket type.
 * @param protocol protocol within family.
 *
 * This is a little different than most of the _r wrappers: we take a write lock
 * on the _s_fds list so that we are able to add the new file descriptor into
 * the list.  This will block most other threads from performing functions on
 * the list, also, we must wait fro a quiet period until all other functions
 * that read lock the list are not being used.  If you are sure that the
 * socket() will only be performed by one thread and that no other thread will
 * act on the file descriptor until socket() returns, use the non-recursive
 * version.
 */
int
__sock_socket_r(int domain, int type, int protocol)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__sock_list_unlock, NULL);
	if ((err = __sock_list_wrlock()) == 0) {
		ret = __sock_socket(domain, type, protocol);
		__sock_list_unlock(NULL);
	} else {
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_socket_r,socket@@SOCKET_1.0");

/**
 * @fn int socketpair(int domain, int type, int protocol, int socket_vector[2])
 * @brief Create a pair of connected sockets.
 * @param domain protocol family.
 * @param type socket type.
 * @param protocol protocol within family.
 * @param socket_vector vector into which to receive two integer file descriptors for the connected sockets.
 * 
 * NOTICES: This function cannot contain a thread cancellation point (according to XNS 5.2).
 */
int
__sock_socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	int err, socks[2] = {-1,-1};
	struct _s_user *user1, *user2;
	struct socksysreq args = {
		{SO_SOCKPAIR, domain, type, protocol, (long) socket_vector,}
	};
	if (!(user1 = (struct _s_user *) malloc(sizeof(*user1))))
		goto enomem1;
	memset(user1, 0, sizeof(*user1));
	if (!(user2 = (struct _s_user *) malloc(sizeof(*user2))))
		goto enomem2;
	memset(user2, 0, sizeof(*user2));
	if (__sock_ioctl(&args) == -1)
		goto badpair;
	/* try to convert them to sockets */
	if (isastream(socks[0])) {
		struct strioctl ioc;

		if (ioctl(socks[0], I_PUSH, "sockmod") == -1)
			goto badioctl;
		ioc.ic_cmd = O_SI_GETUDATA;
		ioc.ic_dp = (char *) &user1->info;
		ioc.ic_len = sizeof(struct o_si_udata);
		ioc.ic_timout = SOCKMOD_TIMEOUT;
		if (ioctl(socks[0], I_STR, &ioc) == -1)
			goto badgetuser;
		user1->info.sockparams.sp_family = domain;
		user1->info.sockparams.sp_type = type;
		user1->info.sockparams.sp_protocol = protocol;
		_s_fds[socks[0]] = user1;
	} else
		_s_fds[socks[0]] = NULL;
	if (isastream(socks[1])) {
		struct strioctl ioc;

		if (ioctl(socks[1], I_PUSH, "sockmod") == -1)
			goto badioctl;
		ioc.ic_cmd = O_SI_GETUDATA;
		ioc.ic_dp = (char *) &user1->info;
		ioc.ic_len = sizeof(struct o_si_udata);
		ioc.ic_timout = SOCKMOD_TIMEOUT;
		if (ioctl(socks[1], I_STR, &ioc) == -1)
			goto badgetuser;
		user2->info.sockparams.sp_family = domain;
		user2->info.sockparams.sp_type = type;
		user2->info.sockparams.sp_protocol = protocol;
		_s_fds[socks[1]] = user2;
	} else
		_s_fds[socks[1]] = NULL;
	return (0);
      badgetuser:
      badioctl:
	err = errno;
	_s_fds[socks[1]] = NULL;
	_s_fds[socks[0]] = NULL;
	close(socks[1]);
	close(socks[0]);
	free(user2);
	free(user1);
	errno = err;
	return (-1);
      badpair:
	err = errno;
	free(user2);
	free(user1);
	errno = err;
	return (-1);
      enomem2:
	free(user1);
      enomem1:
	errno = ENOMEM;
	return (-1);

}

/**
 * @brief recursive socketpair function.
 * @param domain protocol family.
 * @param type socket type.
 * @param protocol protocol within family.
 * @param socket_vector 2 element array for returned file descriptors.
 *
 * This is a little different than most of the _r wrappers: we take a write lock
 * on the _s_fds list so that we are able to add the new file descriptor into
 * the list.  This will block most other threads from performing functions on
 * the list, also, we must wait fro a quiet period until all other functions
 * that read lock the list are not being used.  If you are sure that the
 * socketpair() will only be performed by one thread and that no other thread
 * will act on the file descriptor until socketpair() returns, use the
 * non-recursive version.
 */
int
__sock_socketpair_r(int domain, int type, int protocol, int socket_vector[2])
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__sock_list_unlock, NULL);
	if ((err = __sock_list_wrlock()) == 0) {
		ret = __sock_socketpair(domain, type, protocol, socket_vector);
		__sock_list_unlock(NULL);
	} else {
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_socketpair_r,socketpair@@SOCKET_1.0");

/**
 * @fn ssize_t recv(int fd, void *buf, size_t len, int flags)
 * @brief Receive data from a socket.
 * @param fd the socket from which to receive data.
 * @param buf a pointer to a buffer into which to receive the data.
 * @param len the length of the buffer.
 * @param flags receive flags (e.g. MSG_WAITALL).
 *
 * RETURN VALUE: On success, this function returns a zero or positive integer reflecting the number
 * of bytes read into the user supplied buffer.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 */
ssize_t
__sock_recv(int fd, void *buf, size_t len, int flags)
{
	/* MSG_PEEK, MSG_OOB, or MSG_WAITALL */
	/* We should really just do a getpmsg here, with band setting according to MSG_OOB, or we
	   can do an I_PEEK if MSG_PEEK is set. */
	/* XNS says if the MSG_WAITALL flag is not set, data will be returned only up to the end of 
	   the first message. */
	struct socksysreq args = {
		{SO_RECV, fd, (long) buf, len, flags,}
	};
	return __sock_ioctl(&args);
}

/**
 * @brief Recursive receive data from a socket.
 * @param fd the socket from which to receive data.
 * @param buf a pointer to a buffer into which to receive the data.
 * @param len the length of the buffer.
 * @param flags receive flags (e.g. MSG_WAITALL).
 */
ssize_t
__sock_recv_r(int fd, void *buf, size_t len, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_recv(fd, buf, len, flags)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_recv(fd, buf, len, flags)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_recv_r,recv@@SOCKET_1.0");

/**
 * @fn ssize_t recvmsg(int fd, struct msghdr *msg, int flags)
 * @brief Receive data from a socket.
 * @param fd the socket from which to receive data.
 * @param msg a pointer to a message header structure describing the data and ancilliary data buffers into which to receive information.
 * @param flags receive flags, such as MSG_WAITALL.
 *
 * RETURN VALUE: On success, this function returns a zero or positive integer reflecting the number
 * of bytes read into the user supplied buffer.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 */
ssize_t
__sock_recvmsg(int fd, struct msghdr *msg, int flags)
{
	/* MSG_PEEK, MSG_OOB, or MSG_WAITALL */
	/* We should really just do a getpmsg here, with band setting according to MSG_OOB, or we
	   can do an I_PEEK if MSG_PEEK is set. */
	/* XNS says if the MSG_WAITALL flag is not set, data will be returned only up to the end of 
	   the first message. */
	/* Upon return we can set MSG_EOR, MSG_OOB, MSG_TRUNC or MSG_CTRUNC. */
	struct socksysreq args = {
		{SO_RECVMSG, fd, (long) msg, flags,}
	};
	return __sock_ioctl(&args);
}

ssize_t
__sock_recvmsg_r(int fd, struct msghdr *msg, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_recvmsg(fd, msg, flags)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_recvmsg(fd, msg, flags)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_recvmsg_r,recvmsg@@SOCKET_1.0");

/**
 * @fn ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen)
 * @brief Receive data from a socket.
 * @param fd the socket from which to receive data.
 * @param buf a pointer to a user supplied buffer into which to receive data.
 * @param len the length of the user supplied buffer in bytes.
 * @param flags receive flags (e.g. @c MSG_WAITALL).
 * @param addr a pointer to a sockaddr structure containing the address from which to receive data.
 * @param alen the length of the address in the sockaddr structure.
 *
 * RETURN VALUE: On success, this function returns a zero or positive integer reflecting the number
 * of bytes read into the user supplied buffer.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 */
ssize_t
__sock_recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen)
{
	/* MSG_PEEK, MSG_OOB, or MSG_WAITALL */
	/* We should really just do a getpmsg here, with band setting according to MSG_OOB, or we
	   can do an I_PEEK if MSG_PEEK is set. */
	/* XNS says if the MSG_WAITALL flag is not set, data will be returned only up to the end of 
	   the first message. */
	struct socksysreq args = {
		{SO_RECVFROM, fd, (long) buf, len, flags, (long) addr, (long) alen}
	};
	return __sock_ioctl(&args);
}

ssize_t
__sock_recvfrom_r(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t * alen)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_recvfrom(fd, buf, len, flags, addr, alen)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_recvfrom(fd, buf, len, flags, addr, alen)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_recvfrom_r,recvfrom@@SOCKET_1.0");

/**
 * @fn ssize_t send(int fd, const void *buf, size_t len, int flags)
 * @brief Send data on a socket.
 * @param fd the socket from which to send data.
 * @param buf a pointer to a user supplied buffer containing the data to send.
 * @param len the amount of data in the buffer in bytes.
 * @param flags send flags, such as MSG_EOR.
 *
 * RETURN VALUE: On success, this function returns a zero or positive integer reflecting the number
 * of bytes sent from the user supplied buffer.
 *
 * This function contains a thread cancellation point (according to XNS 5.2).
 */
ssize_t
__sock_send(int fd, const void *buf, size_t len, int flags)
{
	/* flags can be MSG_EOR or MSG_OOB */
	/* we really just want to put a T_DATA_REQ, T_EXDATA_REQ or T_UNITDATA_REQ message here.
	   Note that the T_UNITDATA_REQ requires an address from the user structure. */
	struct socksysreq args = {
		{SO_SEND, fd, (long)buf, len, flags,}
	};
	return __sock_ioctl(&args);
}

ssize_t
__sock_send_r(int fd, const void *buf, size_t len, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_send(fd, buf, len, flags)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_send(fd, buf, len, flags)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_send_r,send@@SOCKET_1.0");

/**
 * @fn ssize_t sendmsg(int fd, const struct msghdr *msg, int flags)
 * @brief Send data on a socket.
 * @param fd the socket from which to send data.
 * @param msg a pointer to a msghdr structure describing the destination address, data and ancilliary data associated with the data.
 * @param flags send flags, such as MSG_EOR.
 *
 * RETURN VALUE: On success, this function returns a zero or positive integer reflecting the number
 * of bytes sent from the user supplied buffer.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 */
ssize_t
__sock_sendmsg(int fd, const struct msghdr *msg, int flags)
{
	/* flags can be MSG_EOR or MSG_OOB */
	/* we really just want to put a T_OPTDATA_REQ or T_UNITDATA_REQ message here.  Note that
	   the T_UNITDATA_REQ requires an address from the user structure. */
	struct socksysreq args = {
		{SO_SENDMSG, fd, (long)msg, flags,}
	};
	return __sock_ioctl(&args);
}

ssize_t
__sock_sendmsg_r(int fd, const struct msghdr *msg, int flags)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_sendmsg(fd, msg, flags)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_sendmsg(fd, msg, flags)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_sendmsg_r,sendmsg@@SOCKET_1.0");

/**
 * @fn ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t alen)
 * @brief Send data on a socket.
 * @param fd the socket from which to send data.
 * @param buf a pointer to a user supplied buffer containing the data to send.
 * @param len the length of the data to send in octets.
 * @param flags send flags, such as MSG_EOR.
 * @param addr a pointer to a sockaddr structure containing the destination address for the data.
 * @param alen the length of the address in the sockaddr structure.
 *
 * RETURN VALUE: On success, this function returns a zero or positive integer reflecting the number
 * of bytes sent from the user supplied buffer.
 *
 * NOTICES: This function contains a thread cancellation point (according to XNS 5.2).
 */
ssize_t
__sock_sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
	      socklen_t alen)
{
	/* flags can be MSG_EOR or MSG_OOB */
	/* we really just want to put a T_OPTDATA_REQ or T_UNITDATA_REQ message here.  */
	struct socksysreq args = {
		{SO_SENDTO, fd, (long)buf, len, flags, (long)addr, alen}
	};
	return __sock_ioctl(&args);
}

ssize_t
__sock_sendto_r(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr,
		socklen_t alen)
{
	ssize_t ret = -1;

	pthread_cleanup_push_defer_np(__sock_putuser, &fd);
	if (__sock_getuser(fd)) {
		if ((ret = __sock_sendto(fd, buf, len, flags, addr, alen)) == -1)
			pthread_testcancel();
		__sock_putuser(&fd);
	} else {
		if ((ret = __libc_sendto(fd, buf, len, flags, addr, alen)) == -1)
			pthread_testcancel();
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __sock_sendto_r,sendto@@SOCKET_1.0");

/**
 * @section Identification
 * This development manual was written for the OpenSS7 Sockets Library version \$Name:  $(\$Revision: 0.9.2.3 $).
 * @author Brian F. G. Bidulock
 * @version \$Name:  $(\$Revision: 0.9.2.3 $)
 * @date \$Date: 2006/09/18 13:52:53 $
 *
 * @}
 */

/*
 * vim: comments+=b\:TRANS
 */
