/*****************************************************************************

 @(#) $RCSfile: dlpiapi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008/07/08 16:57:33 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008/07/08 16:57:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpiapi.c,v $
 Revision 0.9.2.6  2008/07/08 16:57:33  brian
 - updated libraries and manual pages

 Revision 0.9.2.5  2008-07-06 14:58:21  brian
 - improvements

 Revision 0.9.2.4  2008-07-01 12:06:40  brian
 - updated manual pages, added new API library headers and impl files

 Revision 0.9.2.3  2008-04-25 11:39:33  brian
 - updates to AGPLv3

 Revision 0.9.2.2  2007/08/14 03:31:17  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/01/05 06:14:55  brian
 - added GCOM api files, doc updates

 *****************************************************************************/

#ident "@(#) $RCSfile: dlpiapi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008/07/08 16:57:33 $"

static char const ident[] =
    "$RCSfile: dlpiapi.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008/07/08 16:57:33 $";

/*
 * This is an OpenSS7 implemetnation of the GCOM dlpiapi library.  It builds
 * and installs as both a shared object library as well as a static library.
 * GCOM originally implemented this library as a static library only called
 * dlpiapi.a.
 */

/* This file can be processed with doxygen(1). */

/** @weakgroup dlpi OpenSS7 DLPI API Library
  * @{ */

/** @file
  * OpenSS7 Data Link Provider Interface (DLPI) API Library implemenation file. */

#define _XOPEN_SOURCE	600
#define _REENTRANT
#define _THREAD_SAFE

#define NEED_T_USCALAR_T	1

#include <stdlib.h>
#include "gettext.h"
#include <sys/types.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
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
#define __EXCEPTIONS	1
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
#include <syslog.h>
#include <stdarg.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

#define _SUN_SOURCE
#define _HPUX_SOURCE
#define _AIX_SOURCE
#include <sys/dlpi.h>
#include <sys/dlpi_sun.h>
#include <sys/dlpi_ext.h>
#undef _SUN_SOURCE
#undef _HPUX_SOURCE
#undef _AIX_SOURCE
#include <dlpiapi.h>

#if defined __i386__ || defined __x86_64__ || defined __k8__
#define fastcall __attribute__((__regparm__(3)))
#else
#define fastcall
#endif

#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

#if __GNUC__ < 3
#define inline inline fastcall __hot
#define noinline extern fastcall __unlikely
#else
#define inline inline __attribute__((always_inline)) fastcall __hot
#define noinline static __attribute__((noinline)) fastcall __unlikely
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#undef min
#define min(a, b) (a < b ? a : b)

/** @brief thread-specific data
  */
struct __dlpi_tsd {
	int _dlerrno;
	char strbuf[BUFSIZ];		/* string buffer */
	unsigned char tsd_bind_ack[DLPI_CTL_BUF_SIZE];
	unsigned char tsd_conn_con[DLPI_CTL_BUF_SIZE];
	unsigned char tsd_conn_ind[DLPI_CTL_BUF_SIZE];
	int tsd_data_cnt;
	int tsd_ctl_cnt;
	unsigned char tsd_data_buf[DLPI_DATA_BUF_SIZE];
	unsigned char tsd_ctl_buf[DLPI_CTL_BUF_SIZE];
};

/** @brief once condition for Thread-Specific Data key creation.
  */
static pthread_once_t __dlpi_tsd_once = PTHREAD_ONCE_INIT;

/** @brief DLPI API Library Thread-Specific Data Key
  */
static pthread_key_t __dlpi_tsd_key = 0;

static void
__dlpi_tsd_free(void *buf)
{
	pthread_setspecific(__dlpi_tsd_key, NULL);
	free(buf);
}

static void
__dlpi_tsd_key_create(void)
{
	pthread_key_create(&__dlpi_tsd_key, __dlpi_tsd_free);
}

static struct __dlpi_tsd *
__dlpi_tsd_alloc(void)
{
	struct __dlpi_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(*tsdp));
	memset(tsdp, 0, sizeof(*tsdp));
	pthread_setspecific(__dlpi_tsd_key, tsdp);
	return (tsdp);
}

static struct __dlpi_tsd *
__dlpi_get_tsd(void)
{
	struct __dlpi_tsd *tsdp;

	pthread_once(&__dlpi_tsd_once, __dlpi_tsd_key_create);
	if (unlikely((tsdp =  (typeof(tsdp)) pthread_getspecific(__dlpi_tsd_key)) == NULL))
		tsdp = __dlpi_tsd_alloc();
	return (tsdp);
}

/** @brief #dlerrno location function.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This function is an implementation of _dlerrno().
  */
__hot int *
__dlpi__dlerrno(void)
{
	return (&__dlpi_get_tsd()->_dlerrno);
}

/** @fn int *_dlerrno(void)
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi__dlerrno().
  *
  * This function provides the location of the integer that contains the DLPI
  * API error number returned by the last DLPI API function that failed.  This
  * is normally used to provide #dlerrno in a thread-safe way as follows:
  *
  * @code
  * #define dlerrno (*(_dlerrno()))
  * @endcode
  */
__asm__(".symver __dlpi__dlerrno,_dlerrno@@DLPIAPI_1.0");

#ifndef dlerrno
#define dlerrno (*(_dlerrno()))
#endif

struct __dlpi_user {
	pthread_rwlock_t du_lock;
	int du_fd;
};

static struct __dlpi_user *__dlpi_fds[OPEN_MAX] = { NULL, };

static pthread_rwlock_t __dlpi_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static inline int
__dlpi_lock_init(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_init(rwlock, NULL);
}
static inline int
__dlpi_lock_rdlock(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__dlpi_lock_wrlock(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline void
__dlpi_lock_unlock(pthread_rwlock_t *rwlock)
{
	pthread_rwlock_unlock(rwlock);
}
static inline int
__dlpi_lock_destroy(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_destroy(rwlock);
}
static inline int
__dlpi_list_rdlock(void)
{
	return __dlpi_lock_rdlock(&__dlpi_fd_lock);
}
static inline int
__dlpi_list_wrlock(void)
{
	return __dlpi_lock_wrlock(&__dlpi_fd_lock);
}
static void
__dlpi_list_unlock(void *ignore)
{
	return __dlpi_lock_unlock(&__dlpi_fd_lock);
}
static inline int
__dlpi_user_destroy(int fd)
{
	struct __dlpi_user *du;
	int err;

	if (unlikely((du = __dlpi_fds[fd]) == NULL)) {
		errno = ENXIO;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	if (unlikely((err = __dlpi_lock_destroy(&du->du_lock)) != 0)) {
		errno = err;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	__dlpi_fds[fd] = NULL;
	free(du);
	return (0);
}
static inline struct __dlpi_user *
__dlpi_user_init(int fd)
{
	struct __dlpi_user *du;
	int err;

	if (unlikely((du = (struct __dlpi_user *) malloc(sizeof(*du))) == NULL)) {
		dlerrno = DL_SYSERR;
		return (du);
	}
	memset(du, 0, sizeof(*du));
	if (unlikely((err = __dlpi_lock_init(&du->du_lock)) != 0)) {
		free(du);
		errno = err;
		dlerrno = DL_SYSERR;
		return (NULL);
	}
	/** At one the DLPI API library did not have a
	  * cdi_close() function, meaning that some older
	  * applications programs might just do a close(2s) on
	  * an existing Stream.
	  */
	if (unlikely(__dlpi_fds[fd] != NULL))
		__dlpi_user_destroy(fd);
	du->du_fd = fd;
	__dlpi_fds[fd] = du;
	return (du);
}
static inline int
__dlpi_user_rdlock(struct __dlpi_user *du)
{
	return __dlpi_lock_rdlock(&du->du_lock);
}
static inline int
__dlpi_user_wrlock(struct __dlpi_user *du)
{
	return __dlpi_lock_wrlock(&du->du_lock);
}
static inline void
__dlpi_user_unlock(struct __dlpi_user *du)
{
	return __dlpi_lock_unlock(&du->du_lock);
}

static int __dlpi_initialized = 0;

/* Forward declarations of internal functions. */
static __hot void __dlpi_putuser(void *arg);
static __hot int __dlpi_getuser(int fd);
static __hot int __dlpi_hlduser(int fd);
static __hot int __dlpi_chkuser(int fd);

/** @internal
  * @brief release a user whether held or got.
  * @param arg argument.
  */
static __hot void
__dlpi_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct __dlpi_user *du = __dlpi_fds[fd];

	__dlpi_user_unlock(du);
	__dlpi_list_unlock(NULL);
	return;
}

/** @internal
  * @brief Get a write locked data link user structure.
  * @param fd the file descriptor for which to get the associated data link.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe write operation.
  *
  * [EAGAIN] the number of read locks on the list is excessive.
  * [EDEADLK] the list of fd lock is already held by the calling thread.
  */
static __hot int
__dlpi_getuser(int fd)
{
	struct __dlpi_user *du;
	int err;

	if (unlikely(__dlpi_initialized == 0))
		goto uninit;
	if (unlikely((err = __dlpi_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely((du = __dlpi_fds[fd]) == NULL))
		goto ebadf;
	if (unlikely((err = __dlpi_user_wrlock(du))))
		goto user_lock_error;
	return (0);
      user_lock_error:
	__dlpi_list_unlock(NULL);
	errno = err;
	dlerrno = DL_SYSERR;
	goto error;
      ebadf:
	errno = EBADF;
	dlerrno = DL_SYSERR;
	goto error;
      list_lock_error:
	errno = err;
	dlerrno = DL_SYSERR;
	goto error;
      uninit:
	errno = ELIBACC;
	dlerrno = DL_SYSERR;
	return (DLPIAPI_NOT_INIT);
      error:
	return (DLPIAPI_SYSERR);
}

/** @internal
  * @brief Get a read locked data link user structure.
  * @param fd the file descriptor for which to get the associated data link.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe write operation.
  *
  * [EAGAIN] the number of read locks on the list is excessive.
  * [EDEADLK] the list of fd lock is already held by the calling thread.
  */
static __hot int
__dlpi_hlduser(int fd)
{
	struct __dlpi_user *du;
	int err;

	if (unlikely(__dlpi_initialized == 0))
		goto uninit;
	if (unlikely((err = __dlpi_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely((du = __dlpi_fds[fd]) == NULL))
		goto ebadf;
	if (unlikely((err = __dlpi_user_rdlock(du))))
		goto user_lock_error;
	return (0);
      user_lock_error:
	__dlpi_list_unlock(NULL);
	errno = err;
	dlerrno = DL_SYSERR;
	goto error;
      ebadf:
	errno = EBADF;
	dlerrno = DL_SYSERR;
	goto error;
      list_lock_error:
	errno = err;
	dlerrno = DL_SYSERR;
	goto error;
      uninit:
	errno = ELIBACC;
	dlerrno = DL_SYSERR;
	return (DLPIAPI_NOT_INIT);
      error:
	return (DLPIAPI_SYSERR);
}

/** @internal
  * @brief Check a data link user structure.
  * @param fd the file descriptor for which to check the associated data link.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  This function takes no
  * locks.  It is primarily called by the non-reentrant versions of the
  * library functions.
  */
static __hot int
__dlpi_chkuser(int fd)
{
	struct __dlpi_user *du;

	if (unlikely(__dlpi_initialized == 0))
		goto uninit;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely((du = __dlpi_fds[fd]) == NULL))
		goto ebadf;
	return (0);
      ebadf:
	errno = EBADF;
	dlerrno = DL_SYSERR;
	goto error;
      uninit:
	errno = ELIBACC;
	dlerrno = DL_SYSERR;
	return (DLPIAPI_NOT_INIT);
      error:
	return (DLPIAPI_SYSERR);
}

/* These are the supposed global variables that are actually implemented as
 * thread-specific data. */

extern unsigned char *_dlpi_bind_ack(void);
extern unsigned char *_dlpi_conn_con(void);
extern unsigned char *_dlpi_conn_ind(void);
extern unsigned char *_dlpi_ctl_buf(void);
extern unsigned char *_dlpi_data_buf(void);
extern int *_dlpi_ctl_cnt(void);
extern int *_dlpi_data_cnt(void);

unsigned char *
__dlpi_bind_ack(void)
{
	return (__dlpi_get_tsd()->tsd_bind_ack);
}

__asm__(".symver __dlpi_bind_ack,_dlpi_bind_ack@@DLPIAPI_1.0");

unsigned char *
__dlpi_conn_con(void)
{
	return (__dlpi_get_tsd()->tsd_conn_con);
}

__asm__(".symver __dlpi_conn_con,_dlpi_conn_con@@DLPIAPI_1.0");

unsigned char *
__dlpi_conn_ind(void)
{
	return (__dlpi_get_tsd()->tsd_conn_ind);
}

__asm__(".symver __dlpi_conn_ind,_dlpi_conn_ind@@DLPIAPI_1.0");

int *
__dlpi_data_cnt(void)
{
	return (&(__dlpi_get_tsd()->tsd_data_cnt));
}

__asm__(".symver __dlpi_data_cnt,_dlpi_data_cnt@@DLPIAPI_1.0");

int *
__dlpi_ctl_cnt(void)
{
	return (&(__dlpi_get_tsd()->tsd_ctl_cnt));
}

__asm__(".symver __dlpi_ctl_cnt,_dlpi_ctl_cnt@@DLPIAPI_1.0");

unsigned char *
__dlpi_data_buf(void)
{
	return (__dlpi_get_tsd()->tsd_data_buf);
}

__asm__(".symver __dlpi_data_buf,_dlpi_data_buf@@DLPIAPI_1.0");

unsigned char *
__dlpi_ctl_buf(void)
{
	return (__dlpi_get_tsd()->tsd_ctl_buf);
}

__asm__(".symver __dlpi_ctl_buf,_dlpi_ctl_buf@@DLPIAPI_1.0");

static int __dlpi_log_options = DLPI_LOG_DEFAULT;

/** @internal
  * @brief perform putmsg() with DLPI API errors.
  * @param fd the DLPI Stream.
  * @param ctlp control part.
  * @param datp data part.
  * @param flag putmsg flag.
  */
static int
__dlpi_putmsg(int fd, struct strbuf *ctlp, struct strbuf *datp, int flag)
{
	int ret, save;

	save = errno;
	if (likely((ret = putmsg(fd, ctlp, datp, flag)) >= 0))
		return (ret);
	dlerrno = DL_SYSERR;
	switch (errno) {
	case EINTR:
	case ERESTART:
		return (DLPIAPI_EINTR);
	case EAGAIN:
		return (DLPIAPI_EAGAIN);
	case ENXIO:
	case EINVAL:
	case EIO:
		return (DLPIAPI_UNUSABLE);
	}
	return (DLPIAPI_SYSERR);
}

/** @internal
  * @brief put control and data
  * @param fd the DLPI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_len control portion length.
  * @param data_ptr data portion data buffer.
  * @param data_len data portion length.
  *
  * This version of the implementation does not need to check the validity of its arguments.
  */
static int
_dlpi_put_both(int fd, char *ctrl_ptr, int ctrl_len, char *data_ptr, int data_len)
{
	struct strbuf ctrl, data, *ctlp = NULL, *datp = NULL;

	if (ctrl_ptr != NULL && ctrl_len >= 0) {
		ctrl.buf = ctrl_ptr;
		ctrl.len = ctrl_len;
		ctrl.maxlen = -1;
		ctlp = &ctrl;
	}
	if (data_ptr != NULL && data_len >= 0) {
		data.buf = data_ptr;
		data.len = data_len;
		data.maxlen = -1;
		datp = &data;
	}
	return __dlpi_putmsg(fd, ctlp, datp, 0);
}

/** @brief put control and data
  * @param fd the DLPI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_len control portion length.
  * @param data_ptr data portion data buffer.
  * @param data_len data portion length.
  *
  * This is really just a ridiculous wrapper for putmsg(2s).
  */
int
__dlpi_put_both(int fd, char *ctrl_ptr, int ctrl_len, char *data_ptr, int data_len)
{
	int err;

	if (unlikely(ctrl_ptr == NULL && ctrl_len > 0))
		goto einval;
	if (unlikely(data_ptr == NULL && data_len > 0))
		goto einval;
	if (likely((err = __dlpi_chkuser(fd)) >= 0)) {
		return _dlpi_put_both(fd, ctrl_ptr, ctrl_len, data_ptr, data_len);
	}
	return (err);
      einval:
	return (DLPIAPI_PARAM_ERROR);
}

/** @brief The reentrant version of __dlpi_put_both().
  * @param fd the DLPI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_len control portion length.
  * @param data_ptr data portion data buffer.
  * @param data_len data portion length.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_put_both().
  */
int
__dlpi_put_both_r(int fd, char *ctrl_ptr, int ctrl_len, char *data_ptr, int data_len)
{
	int err;

	if (unlikely(ctrl_ptr == NULL && ctrl_len > 0))
		goto einval;
	if (unlikely(data_ptr == NULL && data_len > 0))
		goto einval;
	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_put_both(fd, ctrl_ptr, ctrl_len, data_ptr, data_len);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
      einval:
	return (DLPIAPI_PARAM_ERROR);
}

/** @fn int dlpi_put_both(int fd, char *ctrl_ptr, int ctrl_len, char *data_ptr, int data_len)
  * @param fd the DLPI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_len control portion length.
  * @param data_ptr data portion data buffer.
  * @param data_len data portion length.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_put_both_r().
  */
__asm__(".symver __dlpi_put_both_r,dlpi_put_both@@DLPIAPI_1.0");

/** @internal
  * @brief put a control message
  * @param fd the DLPI Stream.
  * @param nbytes the number of bytes in the control message.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_dlpi_put_proto(int fd, int nbytes)
{
	return _dlpi_put_both(fd, (char *) dlpi_ctl_buf, nbytes, NULL, 0);
}

/** @brief put a control message
  * @param fd the DLPI Stream.
  * @param nbytes the number of bytes in the control message.
  *
  * This is really just a ridiculous wrapper for putmsg(2s).
  */
int
__dlpi_put_proto(int fd, int nbytes)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_put_proto(fd, nbytes);
	return (err);
}

/** @brief The reentrant version of __dlpi_put_proto().
  * @param fd the DLPI Stream.
  * @param nbytes the number of bytes in the control message.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_put_proto().
  */
int
__dlpi_put_proto_r(int fd, int nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_put_proto(fd, nbytes);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int dlpi_put_proto(int fd, int nbytes)
  * @param fd the DLPI Stream.
  * @param nbytes the number of bytes in the control message.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_put_proto_r().
  */
__asm__(".symver __dlpi_put_proto_r,dlpi_put_proto@@DLPIAPI_1.0");

/** @internal
  * @brief get a message
  * @param fd the DLPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_dlpi_get_a_msg(int fd, char *buf, int nbytes)
{
	struct strbuf ctrl, data;
	int ret, flag = 0;

	ctrl.buf = (char *) dlpi_ctl_buf;
	ctrl.len = -1;
	ctrl.maxlen = DLPI_CTL_BUF_SIZE;

	data.buf = (char *) buf;
	data.len = -1;
	data.maxlen = nbytes;

	ret = getmsg(fd, &ctrl, &data, &flag);

	if (ret < 0)
		return (DLPIAPI_SYSERR);

	dlpi_ctl_cnt = ctrl.len;
	dlpi_data_cnt = data.len;
	return (ret);
}

/** @brief get a message
  * @param fd the DLPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  */
int
__dlpi_get_a_msg(int fd, char *buf, int nbytes)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_get_a_msg(fd, buf, nbytes);
	return (err);
}

/** @brief The reentrant version of __dlpi_get_a_msg().
  * @param fd the DLPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_get_a_msg().
  */
int
__dlpi_get_a_msg_r(int fd, char *buf, int nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_get_a_msg(fd, buf, nbytes);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (0);
}

/** @fn int dlpi_get_a_msg(int fd, char *buf, int nbytes)
  * @param fd the DLPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_get_a_msg_r().
  */
__asm__(".symver __dlpi_get_a_msg_r,dlpi_get_a_msg@@DLPIAPI_1.0");

static int
_dlpi_complete_req(int fd, int request, char *caller, int discard)
{
	union DL_primitives *p = (typeof(p)) dlpi_ctl_buf;
	int ret, flag;
	struct pollfd pfd;

	(void) _dlpi_complete_req;
      tryagain:
	pfd.fd = fd;
	pfd.events = POLLPRI | POLLERR | POLLHUP | POLLMSG;
	pfd.revents = 0;

	flag = RS_HIPRI;

	switch (request) {
	case DL_ATTACH_REQ:	/* expect DL_OK_ACK */
	case DL_DETACH_REQ:	/* expect DL_OK_ACK */
	case DL_BIND_REQ:	/* expect DL_BIND_ACK */
	case DL_UNBIND_REQ:	/* expect DL_OK_ACK */
	case DL_CONNECT_RES:	/* expect DL_OK_ACK */
	case DL_RESET_RES:	/* expect DL_OK_ACK */
	case DL_INFO_REQ:	/* expect DL_INFO_ACK */
	case DL_DISCONNECT_REQ:	/* expect DL_OK_ACK */
	case DL_GET_STATISTICS_REQ:	/* expect DL_GET_STATISTICS_ACK */
	case DL_PHYS_ADDR_REQ:	/* expect DL_PHYS_ADDR_ACK */
	case DL_NOTIFY_REQ:	/* expect DL_NOTIFY_ACK */
		break;
	case DL_CONNECT_REQ:	/* expect DL_CONNECT_CON */
	case DL_RESET_REQ:	/* expect DL_RESET_CON */
		pfd.events |= POLLIN | POLLRDNORM;
		flag = 0;
		break;
	default:
		return (DLPIAPI_PARAM_ERROR);
	}

	switch (poll(&pfd, 1, 100)) {
	default:
	case -1:
		return (DLPIAPI_SYSERR);
	case 0:
		return (DLPIAPI_EAGAIN);
	case 1:
		break;
	}

	if (unlikely((ret = _dlpi_get_a_msg(fd, (char *) dlpi_data_buf, DLPI_DATA_BUF_SIZE)) < 0))
		return (ret);
	if (ret != 0)
		goto tryagain;

	if (dlpi_ctl_cnt < sizeof(p->dl_primitive))
		goto tryagain;

	switch (p->dl_primitive) {
	case DL_ERROR_ACK:
		/* handle DL_ERROR_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->error_ack))
			goto tryagain;
		if (p->error_ack.dl_error_primitive != request)
			goto tryagain;
		if (p->error_ack.dl_errno == DL_SYSERR) {
			errno = p->error_ack.dl_unix_errno;
			return (DLPIAPI_SYSERR);
		}
		return (-p->error_ack.dl_errno);
	case DL_OK_ACK:
		/* handle DL_OK_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->ok_ack))
			goto tryagain;
		if (p->ok_ack.dl_correct_primitive != request)
			goto tryagain;
		return (ret);
	case DL_INFO_ACK:
		/* handle DL_INFO_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_INFO_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->info_ack))
			goto tryagain;
		if (p->info_ack.dl_addr_length != 0
		    && dlpi_ctl_cnt < p->info_ack.dl_addr_length + p->info_ack.dl_addr_offset)
			goto tryagain;
		if (p->info_ack.dl_qos_length != 0
		    && dlpi_ctl_cnt < p->info_ack.dl_qos_length + p->info_ack.dl_qos_offset)
			goto tryagain;
		if (p->info_ack.dl_qos_range_length != 0
		    && dlpi_ctl_cnt <
		    p->info_ack.dl_qos_range_length + p->info_ack.dl_qos_range_offset)
			goto tryagain;
		if (p->info_ack.dl_brdcst_addr_length != 0
		    && dlpi_ctl_cnt <
		    p->info_ack.dl_brdcst_addr_length + p->info_ack.dl_brdcst_addr_offset)
			goto tryagain;
		return (ret);
	case DL_BIND_ACK:
		/* handle DL_BIND_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_BIND_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->bind_ack))
			goto tryagain;
		if (p->bind_ack.dl_addr_length != 0
		    && dlpi_ctl_cnt < p->bind_ack.dl_addr_length + p->bind_ack.dl_addr_offset)
			goto tryagain;
		return (ret);
	case DL_PHYS_ADDR_ACK:
		/* handle DL_PHYS_ADDR_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_PHYS_ADDR_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->phys_addr_ack))
			goto tryagain;
		if (p->phys_addr_ack.dl_addr_length != 0
		    && dlpi_ctl_cnt <
		    p->phys_addr_ack.dl_addr_length + p->phys_addr_ack.dl_addr_offset)
			goto tryagain;
		return (ret);
	case DL_GET_STATISTICS_ACK:
		/* handle DL_GET_STATISTICS_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_GET_STATISTICS_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->get_statistics_ack))
			goto tryagain;
		if (p->get_statistics_ack.dl_stat_length != 0
		    && dlpi_ctl_cnt <
		    p->get_statistics_ack.dl_stat_length + p->get_statistics_ack.dl_stat_offset)
			goto tryagain;
		return (ret);
	case DL_NOTIFY_ACK:
		/* handle DL_NOTIFY_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_NOTIFY_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->notify_req))
			goto tryagain;
		return (ret);
	case DL_CONNECT_CON:
		/* handle DL_CONNECT_CON reply */
		if (flag != 0)
			goto tryagain;
		if (request != DL_CONNECT_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->connect_con))
			goto tryagain;
		if (p->connect_con.dl_resp_addr_length != 0
		    && dlpi_ctl_cnt <
		    p->connect_con.dl_resp_addr_length + p->connect_con.dl_resp_addr_offset)
			goto tryagain;
		return (ret);
	case DL_RESET_CON:
		/* handle DL_RESET_CON reply */
		if (flag != 0)
			goto tryagain;
		if (request != DL_RESET_REQ)
			goto tryagain;
		if (dlpi_ctl_cnt < sizeof(p->reset_con))
			goto tryagain;
		return (ret);
	case DL_UNITDATA_IND:
		if (flag != 0)
			goto tryagain;
		if (discard)
			goto tryagain;
		goto tryagain;
	default:
		goto tryagain;
	}
	goto tryagain;
}

int
__dlpi_complete_req(int fd, int request, char *caller, int discard)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_complete_req(fd, request, caller, discard);
	return (err);
}

int
__dlpi_complete_req_r(int fd, int request, char *caller, int discard)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_complete_req(fd, request, caller, discard);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_complete_req_r,dlpi_complete_req@@DLPIAPI_1.0");

/** @internal
  * @brief send a DL_ATTACH_REQ on a DLPI Stream.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  */
static int
_dlpi_send_attach_req(int fd, unsigned long ppa)
{
	dl_attach_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_ATTACH_REQ;
	p->dl_ppa = ppa;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_attach_req(int fd, unsigned long ppa)
{
	int ret;

	if (likely((ret = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_attach_req(fd, ppa);
	return (ret);
}

int
__dlpi_send_attach_req_r(int fd, unsigned long ppa)
{
	int ret;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((ret = __dlpi_hlduser(fd)) >= 0)) {
		ret = _dlpi_send_attach_req(fd, ppa);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __dlpi_send_attach_req_r,dlpi_send_attach_req@@DLPIAPI_1.0");

static int
_dlpi_send_bind_req(int fd, unsigned long dlsap, int conind_nr, int service_mode, int conn_mgmt,
		    int auto_flags)
{
	dl_bind_req_t *p = (typeof(p)) dlpi_ctl_buf;

	switch (service_mode) {
	case (DL_CLDLS):
	case (DL_CODLS):
	case (DL_ACLDLS):
	case (DL_CLDLS | DL_CODLS):
	case (DL_ACLDLS | DL_CLDLS):
	case (DL_HP_RAWDLS):
		break;
	default:
		return (DLPIAPI_PARAM_ERROR);
	}

	if (!(service_mode & DL_CODLS)) {
		if (conind_nr != 0)
			return (DLPIAPI_PARAM_ERROR);
		if (conn_mgmt != 0)
			return (DLPIAPI_PARAM_ERROR);
	}

	switch (auto_flags) {
	case (DL_AUTO_XID):
	case (DL_AUTO_TEST):
	case (DL_AUTO_XID | DL_AUTO_TEST):
		break;
	default:
		return (DLPIAPI_PARAM_ERROR);
	}

	p->dl_primitive = DL_BIND_REQ;
	p->dl_sap = dlsap;
	p->dl_max_conind = conind_nr;
	p->dl_service_mode = service_mode;
	p->dl_conn_mgmt = conn_mgmt;
	p->dl_xidtest_flg = auto_flags;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_bind_req(int fd, unsigned long dlsap, int conind_nr, int service_mode, int conn_mgmt,
		     int auto_flags)
{
	int ret;

	if (likely((ret = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_bind_req(fd, dlsap, conind_nr, service_mode, conn_mgmt,
					   auto_flags);
	return (ret);
}

int
__dlpi_send_bind_req_r(int fd, unsigned long dlsap, int conind_nr, int service_mode, int conn_mgmt,
		       int auto_flags)
{
	int ret;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((ret = __dlpi_hlduser(fd)) >= 0)) {
		ret =
		    _dlpi_send_bind_req(fd, dlsap, conind_nr, service_mode, conn_mgmt, auto_flags);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __dlpi_send_bind_req_r,dlpi_send_bind_req@@DLPIAPI_1.0");

/** @brief connect a DLPI Stream to a remote station.
  * @param fd the DLPI Stream.
  * @param dsap the dsap of the remote station.
  *
  * Note that GCOM DLPI API cannot handle LLC2 or any other connection-oriented
  * data link protocol that has dlsap addresses longer than 32-bits.  How to
  * handle extended addressing such as that for Frame Relay is a big question.
  * For now we assume that the address is least significant bit first with
  * extended addressing.  However, if the dsap is zero (0) it means the default
  * DLSAP and we will not provide an address at all.
  */
static int
_dlpi_send_connect_req(int fd, unsigned long dsap)
{
	struct {
		dl_connect_req_t p;
		unsigned char buffer[0];
	} *buf = (typeof(buf)) dlpi_ctl_buf;
	int len = 0;

	while (dsap) {
		buf->buffer[len] = (dsap & 0x7f);
		buf->buffer[len] |= (dsap >>= 7) ? 0x80 : 0x00;
		len++;
	}

	buf->p.dl_primitive = DL_CONNECT_REQ;
	buf->p.dl_dest_addr_length = len;
	buf->p.dl_dest_addr_offset = len ? sizeof(buf->p) : 0;
	buf->p.dl_qos_length = 0;
	buf->p.dl_qos_offset = 0;
	buf->p.dl_growth = 0;

	return _dlpi_put_proto(fd, sizeof(buf->p) + len);
}

int
__dlpi_send_connect_req(int fd, unsigned long dsap)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) == 0))
		return _dlpi_send_connect_req(fd, dsap);
	return (err);
}

int
__dlpi_send_connect_req_r(int fd, unsigned long dsap)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_connect_req(fd, dsap);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_connect_req_r,dlpi_send_connect_req@@DLPIAPI_1.0");

static int
_dlpi_send_connect_res(int fd, unsigned long correlation, unsigned long token)
{
	dl_connect_res_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_CONNECT_RES;
	p->dl_correlation = correlation;
	p->dl_resp_token = token;
	p->dl_qos_length = 0;
	p->dl_qos_offset = 0;
	p->dl_growth = 0;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_connect_res(int fd, unsigned long correlation, unsigned long token)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_connect_res(fd, correlation, token);
	return (err);
}

int
__dlpi_send_connect_res_r(int fd, unsigned long correlation, unsigned long token)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_connect_res(fd, correlation, token);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);

}

__asm__(".symver __dlpi_send_connect_res_r,dlpi_send_connect_res@@DLPIAPI_1.0");

static int
_dlpi_send_detach_req(int fd)
{
	dl_detach_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_DETACH_REQ;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_detach_req(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_detach_req(fd);
	return (err);
}

int
__dlpi_send_detach_req_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_detach_req(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_detach_req_r,dlpi_send_detach_req@@DLPIAPI_1.0");

static int
_dlpi_send_disconnect_req(int fd, int reason)
{
	dl_disconnect_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_DISCONNECT_REQ;
	p->dl_reason = reason;
	p->dl_correlation = 0;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_disconnect_req(int fd, int reason)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_disconnect_req(fd, reason);
	return (err);
}

int
__dlpi_send_disconnect_req_r(int fd, int reason)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_disconnect_req(fd, reason);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_disconnect_req_r,dlpi_send_disconnect_req@@DLPIAPI_1.0");

static int
_dlpi_send_info_req(int fd)
{
	dl_info_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_INFO_REQ;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_info_req(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_info_req(fd);
	return (err);
}

int
__dlpi_send_info_req_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_info_req(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_info_req_r,dlpi_send_info_req@@DLPIAPI_1.0");

static int
_dlpi_send_reset_req(int fd)
{
	dl_reset_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_RESET_REQ;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_reset_req(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_reset_req(fd);
	return (err);
}

int
__dlpi_send_reset_req_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_reset_req(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_reset_req_r,dlpi_send_reset_req@@DLPIAPI_1.0");

static int
_dlpi_send_reset_res(int fd)
{
	dl_reset_res_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_RESET_RES;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_reset_res(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_reset_res(fd);
	return (err);
}

int
__dlpi_send_reset_res_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_reset_res(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_reset_res_r,dlpi_send_reset_res@@DLPIAPI_1.0");

static int
_dlpi_send_stats_req(int fd)
{
	dl_get_statistics_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_GET_STATISTICS_REQ;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_stats_req(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_stats_req(fd);
	return (err);
}

int
__dlpi_send_stats_req_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_stats_req(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_stats_req_r,dlpi_send_stats_req@@DLPIAPI_1.0");

static int
_dlpi_send_test_req(int fd, unsigned long pfb, char *data_ptr, int data_len,
		    unsigned char *addr_ptr, int addr_len)
{
	dl_test_req_t *p = (typeof(p)) dlpi_ctl_buf;
	unsigned char *b = (typeof(b)) (p + 1);

	if (addr_len < 0 || addr_len > 20)
		return (DLPIAPI_PARAM_ERROR);
	if (addr_ptr == NULL && addr_len != 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_len < 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_ptr == NULL && data_len != 0)
		return (DLPIAPI_PARAM_ERROR);

	p->dl_primitive = DL_TEST_REQ;
	p->dl_flag = pfb;
	p->dl_dest_addr_length = addr_len;
	p->dl_dest_addr_offset = addr_len ? sizeof(*p) : 0;
	memcpy(b, addr_ptr, addr_len);

	return _dlpi_put_both(fd, (char *) dlpi_ctl_buf, sizeof(*p) + addr_len, data_ptr, data_len);
}

int
__dlpi_send_test_req(int fd, unsigned long pfb, char *data_ptr, int data_len,
		     unsigned char *addr_ptr, int addr_len)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_test_req(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
	return (err);
}

int
__dlpi_send_test_req_r(int fd, unsigned long pfb, char *data_ptr, int data_len,
		       unsigned char *addr_ptr, int addr_len)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_test_req(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_test_req_r,dlpi_send_test_req@@DLPIAPI_1.0");

static int
_dlpi_send_test_res(int fd, unsigned long pfb, char *data_ptr, int data_len,
		    unsigned char *addr_ptr, int addr_len)
{
	dl_test_res_t *p = (typeof(p)) dlpi_ctl_buf;
	unsigned char *b = (typeof(b)) (p + 1);

	if (addr_len < 0 || addr_len > 20)
		return (DLPIAPI_PARAM_ERROR);
	if (addr_ptr == NULL && addr_len != 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_len < 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_ptr == NULL && data_len != 0)
		return (DLPIAPI_PARAM_ERROR);

	p->dl_primitive = DL_TEST_RES;
	p->dl_flag = pfb;
	p->dl_dest_addr_length = addr_len;
	p->dl_dest_addr_offset = addr_len ? sizeof(*p) : 0;
	memcpy(b, addr_ptr, addr_len);

	return _dlpi_put_both(fd, (char *) dlpi_ctl_buf, sizeof(*p) + addr_len, data_ptr, data_len);
}

int
__dlpi_send_test_res(int fd, unsigned long pfb, char *data_ptr, int data_len,
		     unsigned char *addr_ptr, int addr_len)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_test_res(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
	return (err);
}

int
__dlpi_send_test_res_r(int fd, unsigned long pfb, char *data_ptr, int data_len,
		       unsigned char *addr_ptr, int addr_len)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_test_res(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_test_res_r,dlpi_send_test_res@@DLPIAPI_1.0");

static int
_dlpi_send_unbind_req(int fd)
{
	dl_unbind_req_t *p = (typeof(p)) dlpi_ctl_buf;

	p->dl_primitive = DL_UNBIND_REQ;

	return _dlpi_put_proto(fd, sizeof(*p));
}

int
__dlpi_send_unbind_req(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_unbind_req(fd);
	return (err);
}

int
__dlpi_send_unbind_req_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_unbind_req(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_unbind_req_r,dlpi_send_unbind_req@@DLPIAPI_1.0");

static int
_dlpi_send_xid_req(int fd, unsigned long pfb, char *data_ptr, int data_len,
		   unsigned char *addr_ptr, int addr_len)
{
	dl_xid_req_t *p = (typeof(p)) dlpi_ctl_buf;
	unsigned char *b = (typeof(b)) (p + 1);

	if (addr_len < 0 || addr_len > 20)
		return (DLPIAPI_PARAM_ERROR);
	if (addr_ptr == NULL && addr_len != 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_len < 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_ptr == NULL && data_len != 0)
		return (DLPIAPI_PARAM_ERROR);

	p->dl_primitive = DL_XID_REQ;
	p->dl_flag = pfb;
	p->dl_dest_addr_length = addr_len;
	p->dl_dest_addr_offset = addr_len ? sizeof(*p) : 0;
	memcpy(b, addr_ptr, addr_len);

	return _dlpi_put_both(fd, (char *) dlpi_ctl_buf, sizeof(*p) + addr_len, data_ptr, data_len);
}

int
__dlpi_send_xid_req(int fd, unsigned long pfb, char *data_ptr, int data_len,
		    unsigned char *addr_ptr, int addr_len)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_xid_req(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
	return (err);
}

int
__dlpi_send_xid_req_r(int fd, unsigned long pfb, char *data_ptr, int data_len,
		      unsigned char *addr_ptr, int addr_len)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_xid_req(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_xid_req_r,dlpi_send_xid_req@@DLPIAPI_1.0");

static int
_dlpi_send_xid_res(int fd, unsigned long pfb, char *data_ptr, int data_len,
		   unsigned char *addr_ptr, int addr_len)
{
	dl_xid_res_t *p = (typeof(p)) dlpi_ctl_buf;
	unsigned char *b = (typeof(b)) (p + 1);

	if (addr_len < 0 || addr_len > 20)
		return (DLPIAPI_PARAM_ERROR);
	if (addr_ptr == NULL && addr_len != 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_len < 0)
		return (DLPIAPI_PARAM_ERROR);
	if (data_ptr == NULL && data_len != 0)
		return (DLPIAPI_PARAM_ERROR);

	p->dl_primitive = DL_XID_RES;
	p->dl_flag = pfb;
	p->dl_dest_addr_length = addr_len;
	p->dl_dest_addr_offset = addr_len ? sizeof(*p) : 0;
	memcpy(b, addr_ptr, addr_len);

	return _dlpi_put_both(fd, (char *) dlpi_ctl_buf, sizeof(*p) + addr_len, data_ptr, data_len);
}

int
__dlpi_send_xid_res(int fd, unsigned long pfb, char *data_ptr, int data_len,
		    unsigned char *addr_ptr, int addr_len)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_send_xid_res(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
	return (err);
}

int
__dlpi_send_xid_res_r(int fd, unsigned long pfb, char *data_ptr, int data_len,
		      unsigned char *addr_ptr, int addr_len)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_hlduser(fd)) >= 0)) {
		err = _dlpi_send_xid_res(fd, pfb, data_ptr, data_len, addr_ptr, addr_len);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_send_xid_res_r,dlpi_send_xid_res@@DLPIAPI_1.0");

/** @internal
  * @brief attach a DLPI Stream to a PPA.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  *
  * This version of the implementation does not require validation of the file
  * descriptor.
  */
static int
_dlpi_attach_ppa(int fd, unsigned long ppa)
{
	int err;

	if (unlikely((err = _dlpi_send_attach_req(fd, ppa)) < 0))
		return (err);
	return _dlpi_complete_req(fd, DL_ATTACH_REQ, "dlpi_attach_ppa", 1);

}

/** @brief attach a DLPI Stream to a PPA.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  * @version: DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_attach_ppa().
  */
int
__dlpi_attach_ppa(int fd, unsigned long ppa)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_attach_ppa(fd, ppa);
	return (err);
}

/** @brief attach a DLPI Stream to a PPA.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  * @version: DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_attach_ppa_r().
  */
int
__dlpi_attach_ppa_r(int fd, unsigned long ppa)
{
	int ret;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((ret = __dlpi_getuser(fd) >= 0))) {
		ret = _dlpi_attach_ppa(fd, ppa);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int dlpi_attach_ppa(int fd, unsigned long ppa)
  * @brief attach a DLPI Stream to a PPA.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  * @version: DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_attach_ppa_r().
  */
__asm__(".symver __dlpi_attach_ppa_r,dlpi_attach_ppa@@DLPIAPI_1.0");

static int
_dlpi_detach_ppa(int fd)
{
	int err;

	if (unlikely((err = _dlpi_send_detach_req(fd)) < 0))
		return (err);
	return _dlpi_complete_req(fd, DL_DETACH_REQ, "dlpi_detach_ppa", 1);
}

int
__dlpi_detach_ppa(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_detach_ppa(fd);
	return (err);
}

int
__dlpi_detach_ppa_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_getuser(fd)) >= 0)) {
		err = _dlpi_detach_ppa(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_detach_ppa_r,dlpi_detach_ppa@@DLPIAPI_1.0");

static int
_dlpi_bind_dlsap(int fd, unsigned long dlsap, int conind_nr)
{
	int err;

	if (unlikely((err = _dlpi_send_bind_req(fd, dlsap, conind_nr, DL_CLDLS, 0, 0)) < 0))
		return (err);
	return _dlpi_complete_req(fd, DL_BIND_REQ, "dlpi_bind_dlsap", 1);
}

int
__dlpi_bind_dlsap(int fd, unsigned long dlsap, int conind_nr)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_bind_dlsap(fd, dlsap, conind_nr);
	return (err);
}

int
__dlpi_bind_dlsap_r(int fd, unsigned long dlsap, int conind_nr)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_getuser(fd)) >= 0)) {
		err = _dlpi_bind_dlsap(fd, dlsap, conind_nr);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_bind_dlsap_r,dlpi_bind_dlsap@@DLPIAPI_1.0");

static int
_dlpi_unbind_dlsap(int fd)
{
	int err;

	if (unlikely((err = _dlpi_send_unbind_req(fd)) < 0))
		return (err);
	return _dlpi_complete_req(fd, DL_UNBIND_REQ, "dlpi_unbind_dlsap", 1);
}

int
__dlpi_unbind_dlsap(int fd)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_unbind_dlsap(fd);
	return (err);
}

int
__dlpi_unbind_dlsap_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_getuser(fd)) >= 0)) {
		err = _dlpi_unbind_dlsap(fd);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_unbind_dlsap_r,dlpi_unbind_dlsap@@DLPIAPI_1.0");

static int
_dlpi_connect_req(int fd, unsigned long dsap)
{
	int err;

	if (unlikely((err = _dlpi_send_connect_req(fd, dsap)) < 0))
		return (err);
	return _dlpi_complete_req(fd, DL_CONNECT_REQ, "dlpi_connect_req", 1);
}

int
__dlpi_connect_req(int fd, unsigned long dsap)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_connect_req(fd, dsap);
	return (err);
}

int
__dlpi_connect_req_r(int fd, unsigned long dsap)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_getuser(fd)) >= 0)) {
		err = _dlpi_connect_req(fd, dsap);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_connect_req_r,dlpi_connect_req@@DLPIAPI_1.0");

static int
_dlpi_disconnect_req(int fd, int reason)
{
	int err;

	if (unlikely((err = _dlpi_send_disconnect_req(fd, reason)) < 0))
		return (err);
	return _dlpi_complete_req(fd, DL_DISCONNECT_REQ, "dlpi_disconnect_req", 1);
}

int
__dlpi_disconnect_req(int fd, int reason)
{
	int err;

	if (likely((err = __dlpi_chkuser(fd)) >= 0))
		return _dlpi_disconnect_req(fd, reason);
	return (err);
}

int
__dlpi_disconnect_req_r(int fd, int reason)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &fd);
	if (likely((err = __dlpi_getuser(fd)) >= 0)) {
		err = _dlpi_disconnect_req(fd, reason);
		__dlpi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_disconnect_req_r,dlpi_disconnect_req@@DLPIAPI_1.0");

/** @internal
  * @brief open a DLPI Stream.
  */
static int
_dlpi_open_data(void)
{
	struct __dlpi_user *du;
	int fd, save;

	if (unlikely(__dlpi_initialized == 0)) {
		errno = ELIBACC;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_NOT_INIT);
	}
	if ((fd = open("/dev/streams/clone/dlmux", O_RDWR)) < 0) {
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	if (unlikely((du = __dlpi_user_init(fd)) == NULL)) {
		save = errno;
		close(fd);
		errno = save;
		return (DLPIAPI_SYSERR);
	}
	return (fd);
}

/** @brief open a DLPI Stream.
  */
int
__dlpi_open_data(void)
{
	return _dlpi_open_data();
}

/** @brief The reentrant version of __dlpi_open_data().
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_open_data().
  */
int
__dlpi_open_data_r(void)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if (likely((err = __dlpi_list_wrlock()) == 0)) {
		err = _dlpi_open_data();
		__dlpi_list_unlock(NULL);
	} else {
		errno = err;
		dlerrno = DL_SYSERR;
		err = DLPIAPI_SYSERR;
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int dlpi_open_data(void)
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_open_data_r().
  */
__asm__(".symver __dlpi_open_data_r,dlpi_open_data@@DLPIAPI_1.0");

static int
_dlpi_open_GCOM(char *hostname)
{
	return _dlpi_open_data();
}

int
__dlpi_open_GCOM(char *hostname)
{
	if (hostname != NULL) {
		errno = ENOTSUP;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	return _dlpi_open_GCOM(hostname);
}

int
__dlpi_open_GCOM_r(char *hostname)
{
	int err;

	if (hostname != NULL) {
		errno = ENOTSUP;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if (likely((err = __dlpi_list_wrlock()) == 0)) {
		err = _dlpi_open_GCOM(hostname);
		__dlpi_list_unlock(NULL);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

__asm__(".symver __dlpi_open_GCOM_r,dlpi_open_GCOM@@DLPIAPI_1.0");

static int
_dlpi_close_GCOM(int fd)
{
	int err;

	if (unlikely((err = close(fd)) < 0)) {
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	return __dlpi_user_destroy(fd);
}

int
__dlpi_close_GCOM(int fd)
{
	int err;

	if (unlikely((err = __dlpi_chkuser(fd)) < 0))
		return (err);
	return _dlpi_close_GCOM(fd);
}

int
__dlpi_close_GCOM_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if (likely((err = __dlpi_list_wrlock()) == 0)) {
		if (likely((err = __dlpi_chkuser(fd)) >= 0)) {
			err = _dlpi_close_GCOM(fd);
		}
		__dlpi_list_unlock(NULL);
	} else {
		errno = err;
		dlerrno = DL_SYSERR;
		err = DLPIAPI_SYSERR;
	}
	pthread_cleanup_pop_restore_np(0);
	return (err); 
}

__asm__(".symver __dlpi_close_GCOM_r,dlpi_close_GCOM@@DLPIAPI_1.0");


/** @internal
  * @brief initialize DLPI library with log file.
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  */
static int
_dlpi_init_FILE(uint log_options, FILE *log_file)
{
	if (unlikely(__dlpi_initialized != 0)) {
		errno = EALREADY;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	__dlpi_initialized = 1;
	__dlpi_log_options = log_options;
	return (0);		/* FIXME */
}

/** @brief initialized DLPI library with log file.
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  */
int
__dlpi_init_FILE(uint log_options, FILE *log_file)
{
	if (unlikely(log_file == NULL)) {
		errno = EINVAL;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	return _dlpi_init_FILE(log_options, log_file);
}

/** @brief The reentrant version of __dlpi_init_FILE().
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_init_FILE().
  */
int
__dlpi_init_FILE_r(uint log_options, FILE *log_file)
{
	int err;

	if (unlikely(log_file == NULL)) {
		errno = EINVAL;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if (likely((err = __dlpi_list_wrlock()) == 0)) {
		err = _dlpi_init_FILE(log_options, log_file);
		__dlpi_list_unlock(NULL);
	} else {
		errno = err;
		dlerrno = DL_SYSERR;
		err = -1;
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int dlpi_init_FILE(uint log_options, FILE *log_file)
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_init_FILE_r().
  */
__asm__(".symver __dlpi_init_FILE_r,dlpi_init_FILE@@DLPIAPI_1.0");

/** @internal
  * @brief initialize DLPI library.
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  */
static int
_dlpi_init(uint log_options, char *log_name)
{
	FILE *f;
	int err, save;

	if (unlikely(__dlpi_initialized != 0)) {
		errno = EALREADY;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	if (log_name == NULL) {
		log_name = "/var/log/dlpiapi.log";
		// log_name = "dlpilogfile";
	}
	if (unlikely((f = fopen(log_name, "+w")) == NULL)) {
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	if (unlikely((err = __dlpi_init_FILE(log_options, f)) < 0)) {
		save = errno;
		fclose(f);
		errno = save;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	return (0);
}

/** @brief initialize DLPI library.
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  */
int
__dlpi_init(uint log_options, char *log_name)
{
	return _dlpi_init(log_options, log_name);
}

/** @brief The reentrant version of __dlpi_init().
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_init().
  */
int
__dlpi_init_r(uint log_options, char *log_name)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if (likely((err = __dlpi_list_wrlock()) == 0)) {
		err = _dlpi_init(log_options, log_name);
		__dlpi_list_unlock(NULL);
	} else {
		errno = err;
		dlerrno = DL_SYSERR;
		err = -1;
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int dlpi_init(uint log_options, char *log_name)
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_init_r().
  */
__asm__(".symver __dlpi_init_r,dlpi_init@@DLPIAPI_1.0");

/** @internal
  * @brief set log file size.
  * @param nbytes maximum size of log file (<=0 infinite).
  */
static int
_dlpi_set_log_size(long nbytes)
{
	if (unlikely(__dlpi_initialized == 0)) {
		errno = ELIBACC;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	if (unlikely(nbytes > 0)) {
		errno = ENOTSUP;
		dlerrno = DL_SYSERR;
		return (DLPIAPI_SYSERR);
	}
	return (0);
}

/** @brief set log file size.
  * @param nbytes maximum size of log file (<=0 infinite).
  */
int
__dlpi_set_log_size(long nbytes)
{
	return _dlpi_set_log_size(nbytes);
}

/** @brief The reentrant version of __dlpi_set_log_size().
  * @param nbytes maximum size of log file (<=0 infinite).
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_set_log_size().
  */
int
__dlpi_set_log_size_r(long nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if (likely((err = __dlpi_list_wrlock()) == 0)) {
		err = _dlpi_set_log_size(nbytes);
		__dlpi_list_unlock(NULL);
	} else {
		errno = err;
		dlerrno = DL_SYSERR;
		err = -1;
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int dlpi_set_log_size(long nbytes)
  * @param nbytes maximum size of log file (<=0 infinite).
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_set_log_size_r().
  */
__asm__(".symver __dlpi_set_log_size_r,dlpi_set_log_size@@DLPIAPI_1.0");

/* *INDENT-OFF* */
const char *__dlpi_std_errstr[] = {
/*
TRANS  DL_BADSAP: Bad LSAP selector.
*/
	[DL_BADSAP]		= gettext_noop("Bad LSAP selector"),
/*
TRANS  DL_BADADDR: DLSAP addr in improper format or invalid.
*/
	[DL_BADADDR]		= gettext_noop("DLSAP addr in improper format or invalid"),
/*
TRANS  DL_ACCESS: Improper permissions for request.
*/
	[DL_ACCESS]		= gettext_noop("Improper permissions for request"),
/*
TRANS  DL_OUTSTATE: Primitive issued in improper state.
*/
	[DL_OUTSTATE]		= gettext_noop("Primitive issued in improper state"),
/*
TRANS  DL_SYSERR: UNIX system error occurred.
*/
	[DL_SYSERR]		= gettext_noop("UNIX system error occurred"),
/*
TRANS  DL_BADCORR: Seq number not from outstand DL_CONN_IND.
*/
	[DL_BADCORR]		= gettext_noop("Seq number not from outstand DL_CONN_IND"),
/*
TRANS  DL_BADDATA: User data exceeded provider limit.
*/
	[DL_BADDATA]		= gettext_noop("User data exceeded provider limit"),
/*
TRANS  DL_UNSUPPORTED: Requested service not supplied by provider.
*/
	[DL_UNSUPPORTED]	= gettext_noop("Requested service not supplied by provider"),
/*
TRANS  DL_BADPPA: Specified PPA was invalid.
*/
	[DL_BADPPA]		= gettext_noop("Specified PPA was invalid"),
/*
TRANS  DL_BADPRIM: Primitive received not known by provider.
*/
	[DL_BADPRIM]		= gettext_noop("Primitive received not known by provider"),
/*
TRANS  DL_BADQOSPARAM: QOS parameters contained invalid values.
*/
	[DL_BADQOSPARAM]	= gettext_noop("QOS parameters contained invalid values"),
/*
TRANS  DL_BADQOSTYPE: QOS structure type is unknown/unsupported.
*/
	[DL_BADQOSTYPE]		= gettext_noop("QOS structure type is unknown/unsupported"),
/*
TRANS  DL_BADTOKEN: Token used not an active stream.
*/
	[DL_BADTOKEN]		= gettext_noop("Token used not an active stream"),
/*
TRANS  DL_BOUND: Attempted second bind with dl_max_conind.
*/
	[DL_BOUND]		= gettext_noop("Attempted second bind with dl_max_conind"),
/*
TRANS  DL_INITFAILED: Physical Link initialization failed.
*/
	[DL_INITFAILED]		= gettext_noop("Physical Link initialization failed"),
/*
TRANS  DL_NOADDR: Provider couldn't allocate alternate address.
*/
	[DL_NOADDR]		= gettext_noop("Provider couldn't allocate alternate address"),
/*
TRANS  DL_NOTINIT: Physical Link not initialized.
*/
	[DL_NOTINIT]		= gettext_noop("Physical Link not initialized"),
/*
TRANS  DL_UNDELIVERABLE: Previous data unit could not be delivered.
*/
	[DL_UNDELIVERABLE]	= gettext_noop("Previous data unit could not be delivered"),
/*
TRANS  DL_NOTSUPPORTED: Primitive is known but not supported.
*/
	[DL_NOTSUPPORTED]	= gettext_noop("Primitive is known but not supported"),
/*
TRANS  DL_TOOMANY: Limit exceeded.
*/
	[DL_TOOMANY]		= gettext_noop("Limit exceeded"),
/*
TRANS  DL_NOTENAB: Promiscuous mode not enabled.
*/
	[DL_NOTENAB]		= gettext_noop("Promiscuous mode not enabled"),
/*
TRANS  DL_BUSY: Other streams for PPA in post-attached state.
*/
	[DL_BUSY]		= gettext_noop("Other streams for PPA in post-attached state"),
/*
TRANS  DL_NOAUTO: Automatic handling of XID and TEST response not supported.
*/
	[DL_NOAUTO]		= gettext_noop("Automatic handling of XID and TEST response not supported"),
/*
TRANS  DL_NOXIDAUTO: Automatic handling of XID not supported.
*/
	[DL_NOXIDAUTO]		= gettext_noop("Automatic handling of XID not supported"),
/*
TRANS  DL_NOTESTAUTO: Automatic handling of TEST not supported.
*/
	[DL_NOTESTAUTO]		= gettext_noop("Automatic handling of TEST not supported"),
/*
TRANS  DL_XIDAUTO: Automatic handling of XID response.
*/
	[DL_XIDAUTO]		= gettext_noop("Automatic handling of XID response"),
/*
TRANS  DL_TESTAUTO: Automatic handling of TEST response.
*/
	[DL_TESTAUTO]		= gettext_noop("Automatic handling of TEST response"),
/*
TRANS  DL_PENDING: Pending outstanding connect indications.
*/
	[DL_PENDING]		= gettext_noop("Pending outstanding connect indications"),
/*
TRANS  Any other value: Unknown error.
*/
	[DL_PENDING + 1]	= gettext_noop("Unknown error"),
/*
TRANS  Values from 0x80-0xff: Device-specific error.
*/
	[DL_PENDING + 2]	= gettext_noop("Device-specific error"),
};
/* *INDENT-ON* */

/** @internal
  * @brief perror() to log file.
  * @param msg the prefix message.
  *
  * Note that msg should be const char *.
  */
static void
_dlpi_perror(char *msg)
{
	int error = dlerrno;

	if (unlikely(__dlpi_initialized == 0)) {
		errno = ELIBACC;
		dlerrno = DL_SYSERR;
		return;
	}
	if (error == DL_SYSERR) {
		syslog(LOG_INFO, "%s: %m", msg);
	} else {
		if (error >= 0x80 && error <= 0xff)
			error = DL_PENDING + 2;
		else if (error < 0 || error > DL_PENDING)
			error = DL_PENDING + 1;
		syslog(LOG_INFO, "%s: %s", msg, gettext(__dlpi_std_errstr[error]));
	}
}

/** @brief perror() to log file.
  * @param msg the prefix message.
  *
  * Note that msg should be const char *.
  */
void
__dlpi_perror(char *msg)
{
	return _dlpi_perror(msg);
}

/** @brief the reentrant version of __dlpi_perror().
  * @param msg the prefix message.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_perror().
  */
void
__dlpi_perror_r(char *msg)
{
	return _dlpi_perror(msg);
}

/** @fn void dlpi_perror(char *msg)
  * @param msg the prefix message.
  * @verison DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_perror_r().
  */
__asm__(".symver __dlpi_perror_r,dlpi_perror@@DLPIAPI_1.0");

/** @internal
  * @brief print message to log file.
  * @param fmt format string.
  */
static void
_dlpi_vprintf(const char *fmt, va_list args)
{
	vsyslog(LOG_INFO, fmt, args);
}

/** @brief print message to log file.
  * @param fmt format string.
  */
void
__dlpi_printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_dlpi_vprintf(fmt, args);
	va_end(args);
}

/** @brief The reentrant version of __dlpi_printf().
  * @param fmt format string.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_printf().
  */
void
__dlpi_printf_r(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_dlpi_vprintf(fmt, args);
	va_end(args);
}

/** @fn void dlpi_printf(char *fmt, ...)
  * @param fmt format string.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_printf_r().
  */
__asm__(".symver __dlpi_printf_r,dlpi_printf@@DLPIAPI_1.0");

/** @internal
  * @brief print data with indented hexadecimal.
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  */
static int
_dlpi_print_msg(unsigned char *buf, unsigned int nbytes, int indent)
{
	char tmp[BUFSIZ];
	int i, rows, cols, remaining;
	char *p = (char *) buf, *q = tmp;

	if (indent > 5)
		indent = 5;
	for (remaining = nbytes, rows = (nbytes + 15) >> 4; rows > 0; rows--, remaining -= cols) {
		cols = (remaining >= 16) ? 16 : remaining;
		for (q = tmp, i = 0; i < indent; i++, q++)
			*q = '\t';
		for (i = 0; i < cols; i++, q += 3, p++)
			sprintf(q, "%02X ", *p);
		syslog(LOG_INFO, "%s", tmp);
	}
	return (0);		/* XXX */
}

/** @brief print data with indented hexadecimal.
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  */
int
__dlpi_print_msg(unsigned char *buf, unsigned int nbytes, int indent)
{
	return _dlpi_print_msg(buf, nbytes, indent);
}

/** @brief The reentrant version of __dlpi_print_msg().
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_print_msg().
  */
int
__dlpi_print_msg_r(unsigned char *buf, unsigned int nbytes, int indent)
{
	return _dlpi_print_msg(buf, nbytes, indent);
}

/** @fn int dlpi_print_msg(unsigned char *buf, unsigned int nbytes, int indent)
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_print_msg_r().
  */
__asm__(".symver __dlpi_print_msg_r,dlpi_print_msg@@DLPIAPI_1.0");

/** @internal
  * @brief decode primtiive and output to syslog.
  * @param msg message to prefix to output.
  */
static int
_dlpi_decode_ctl(char *msg)
{
	union DL_primitives *p = (typeof(p)) msg;

	if (dlpi_ctl_cnt >= sizeof(p->dl_primitive)) {
		switch (p->dl_primitive) {
		case DL_INFO_REQ:
			syslog(LOG_INFO, "%s: DL_INFO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->info_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_BIND_REQ:
			syslog(LOG_INFO, "%s: DL_BIND_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->bind_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_UNBIND_REQ:
			syslog(LOG_INFO, "%s: DL_UNBIND_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->unbind_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_INFO_ACK:
			syslog(LOG_INFO, "%s: DL_INFO_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->info_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_BIND_ACK:
			syslog(LOG_INFO, "%s: DL_BIND_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->bind_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_ERROR_ACK:
			syslog(LOG_INFO, "%s: DL_ERROR_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->error_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_OK_ACK:
			syslog(LOG_INFO, "%s: DL_OK_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->ok_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_UNITDATA_REQ:
			syslog(LOG_INFO, "%s: DL_UNITDATA_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->unitdata_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_UNITDATA_IND:
			syslog(LOG_INFO, "%s: DL_UNITDATA_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->unitdata_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_UDERROR_IND:
			syslog(LOG_INFO, "%s: DL_UDERROR_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->uderror_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_UDQOS_REQ:
			syslog(LOG_INFO, "%s: DL_UDQOS_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->udqos_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_ATTACH_REQ:
			syslog(LOG_INFO, "%s: DL_ATTACH_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->attach_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DETACH_REQ:
			syslog(LOG_INFO, "%s: DL_DETACH_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->detach_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CONNECT_REQ:
			syslog(LOG_INFO, "%s: DL_CONNECT_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->connect_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CONNECT_IND:
			syslog(LOG_INFO, "%s: DL_CONNECT_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->connect_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CONNECT_RES:
			syslog(LOG_INFO, "%s: DL_CONNECT_RES", msg);
			if (dlpi_ctl_cnt >= sizeof(p->connect_res)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CONNECT_CON:
			syslog(LOG_INFO, "%s: DL_CONNECT_CON", msg);
			if (dlpi_ctl_cnt >= sizeof(p->connect_con)) {
				/* XXX decode more */
			}
			return (0);
		case DL_TOKEN_REQ:
			syslog(LOG_INFO, "%s: DL_TOKEN_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->token_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_TOKEN_ACK:
			syslog(LOG_INFO, "%s: DL_TOKEN_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->token_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DISCONNECT_REQ:
			syslog(LOG_INFO, "%s: DL_DISCONNECT_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->disconnect_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DISCONNECT_IND:
			syslog(LOG_INFO, "%s: DL_DISCONNECT_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->disconnect_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_SUBS_UNBIND_REQ:
			syslog(LOG_INFO, "%s: DL_SUBS_UNBIND_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->subs_unbind_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_RESET_REQ:
			syslog(LOG_INFO, "%s: DL_RESET_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reset_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_RESET_IND:
			syslog(LOG_INFO, "%s: DL_RESET_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reset_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_RESET_RES:
			syslog(LOG_INFO, "%s: DL_RESET_RES", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reset_res)) {
				/* XXX decode more */
			}
			return (0);
		case DL_RESET_CON:
			syslog(LOG_INFO, "%s: DL_RESET_CON", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reset_con)) {
				/* XXX decode more */
			}
			return (0);
		case DL_SUBS_BIND_REQ:
			syslog(LOG_INFO, "%s: DL_SUBS_BIND_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->subs_bind_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_SUBS_BIND_ACK:
			syslog(LOG_INFO, "%s: DL_SUBS_BIND_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->subs_bind_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_ENABMULTI_REQ:
			syslog(LOG_INFO, "%s: DL_ENABMULTI_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->enabmulti_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DISABMULTI_REQ:
			syslog(LOG_INFO, "%s: DL_DISABMULTI_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->disabmulti_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_PROMISCON_REQ:
			syslog(LOG_INFO, "%s: DL_PROMISCON_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->promiscon_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_PROMISCOFF_REQ:
			syslog(LOG_INFO, "%s: DL_PROMISCOFF_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->promiscoff_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DATA_ACK_REQ:
			syslog(LOG_INFO, "%s: DL_DATA_ACK_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->data_ack_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DATA_ACK_IND:
			syslog(LOG_INFO, "%s: DL_DATA_ACK_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->data_ack_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_DATA_ACK_STATUS_IND:
			syslog(LOG_INFO, "%s: DL_DATA_ACK_STATUS_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->data_ack_status_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_REPLY_REQ:
			syslog(LOG_INFO, "%s: DL_REPLY_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reply_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_REPLY_IND:
			syslog(LOG_INFO, "%s: DL_REPLY_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reply_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_REPLY_STATUS_IND:
			syslog(LOG_INFO, "%s: DL_REPLY_STATUS_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reply_status_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_REPLY_UPDATE_REQ:
			syslog(LOG_INFO, "%s: DL_REPLY_UPDATE_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reply_update_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_REPLY_UPDATE_STATUS_IND:
			syslog(LOG_INFO, "%s: DL_REPLY_UPDATE_STATUS_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reply_update_status_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_XID_REQ:
			syslog(LOG_INFO, "%s: DL_XID_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->xid_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_XID_IND:
			syslog(LOG_INFO, "%s: DL_XID_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->xid_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_XID_RES:
			syslog(LOG_INFO, "%s: DL_XID_RES", msg);
			if (dlpi_ctl_cnt >= sizeof(p->xid_res)) {
				/* XXX decode more */
			}
			return (0);
		case DL_XID_CON:
			syslog(LOG_INFO, "%s: DL_XID_CON", msg);
			if (dlpi_ctl_cnt >= sizeof(p->xid_con)) {
				/* XXX decode more */
			}
			return (0);
		case DL_TEST_REQ:
			syslog(LOG_INFO, "%s: DL_TEST_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->test_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_TEST_IND:
			syslog(LOG_INFO, "%s: DL_TEST_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->test_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_TEST_RES:
			syslog(LOG_INFO, "%s: DL_TEST_RES", msg);
			if (dlpi_ctl_cnt >= sizeof(p->test_res)) {
				/* XXX decode more */
			}
			return (0);
		case DL_TEST_CON:
			syslog(LOG_INFO, "%s: DL_TEST_CON", msg);
			if (dlpi_ctl_cnt >= sizeof(p->test_con)) {
				/* XXX decode more */
			}
			return (0);
		case DL_PHYS_ADDR_REQ:
			syslog(LOG_INFO, "%s: DL_PHYS_ADDR_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->phys_addr_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_PHYS_ADDR_ACK:
			syslog(LOG_INFO, "%s: DL_PHYS_ADDR_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->phys_addr_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_SET_PHYS_ADDR_REQ:
			syslog(LOG_INFO, "%s: DL_SET_PHYS_ADDR_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_phys_addr_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_GET_STATISTICS_REQ:
			syslog(LOG_INFO, "%s: DL_GET_STATISTICS_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->get_statistics_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_GET_STATISTICS_ACK:
			syslog(LOG_INFO, "%s: DL_GET_STATISTICS_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->get_statistics_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_NOTIFY_REQ:
			syslog(LOG_INFO, "%s: DL_NOTIFY_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->notify_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_NOTIFY_ACK:
			syslog(LOG_INFO, "%s: DL_NOTIFY_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->notify_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_NOTIFY_IND:
			syslog(LOG_INFO, "%s: DL_NOTIFY_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->notify_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_AGGR_REQ:
			syslog(LOG_INFO, "%s: DL_AGGR_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->aggr_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_AGGR_IND:
			syslog(LOG_INFO, "%s: DL_AGGR_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->aggr_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_UNAGGR_REQ:
			syslog(LOG_INFO, "%s: DL_UNAGGR_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->unaggr_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CAPABILITY_REQ:
			syslog(LOG_INFO, "%s: DL_CAPAILITY_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->capability_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CAPABILITY_ACK:
			syslog(LOG_INFO, "%s: DL_CAPABILITY_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->capability_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CONTROL_REQ:
			syslog(LOG_INFO, "%s: DL_CONTROL_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->control_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_CONTROL_ACK:
			syslog(LOG_INFO, "%s: DL_CONTROL_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->control_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_PASSIVE_REQ:
			syslog(LOG_INFO, "%s: DL_PASSIVE_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->passive_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_INTR_MODE_REQ:
			syslog(LOG_INFO, "%s: DL_INTR_MODE_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->intr_mode_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_PPA_REQ:
			syslog(LOG_INFO, "%s: DL_HP_PPA_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->ppa_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_PPA_ACK:
			syslog(LOG_INFO, "%s: DL_HP_PPA_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->ppa_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_MULTICAST_LIST_REQ:
			syslog(LOG_INFO, "%s: DL_HP_MULTICAST_LIST_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->multicast_list_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_MULTICAST_LIST_ACK:
			syslog(LOG_INFO, "%s: DL_HP_MULTICAST_LIST_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->multicast_list_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_RAWDATA_REQ:
			syslog(LOG_INFO, "%s: DL_HP_RAWDATA_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->rawdata_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_RAWDATA_IND:
			syslog(LOG_INFO, "%s: DL_HP_RAWDATA_IND", msg);
			if (dlpi_ctl_cnt >= sizeof(p->rawdata_ind)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_HW_RESET_REQ:
			syslog(LOG_INFO, "%s: DL_HP_HW_RESET_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->reset_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_INFO_REQ:
			syslog(LOG_INFO, "%s: DL_HP_INFO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->hp_info_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_INFO_ACK:
			syslog(LOG_INFO, "%s: DL_HP_INFO_ACK", msg);
			if (dlpi_ctl_cnt >= sizeof(p->hp_info_ack)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_ACK_TO_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_ACK_TO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_ack_to_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_P_TO_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_P_TO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_p_to_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_REJ_TO_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_REJ_TO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_rej_to_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_BUSY_TO_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_BUSY_TO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_busy_to_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_SEND_ACK_TO_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_SEND_ACK_TO_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_send_ack_to_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_MAX_RETRIES_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_MAX_RETRIES_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_max_retries_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_ACK_THRESHOLD_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_ACK_THRESHOLD_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_ack_threshold_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_LOCAL_WIN_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_LOCAL_WIN_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_local_win_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_REMOTE_WIN_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_REMOTE_WIN_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_remote_win_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_CLEAR_STATS_REQ:
			syslog(LOG_INFO, "%s: DL_HP_CLEAR_STATS_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->clear_stats_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_SET_LOCAL_BUSY_REQ:
			syslog(LOG_INFO, "%s: DL_HP_SET_LOCAL_BUSY_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->set_local_busy_req)) {
				/* XXX decode more */
			}
			return (0);
		case DL_HP_CLEAR_LOCAL_BUSY_REQ:
			syslog(LOG_INFO, "%s: DL_HP_CLEAR_LOCAL_BUSY_REQ", msg);
			if (dlpi_ctl_cnt >= sizeof(p->clear_local_busy_req)) {
				/* XXX decode more */
			}
			return (0);
		}
	}
	syslog(LOG_INFO, "%s:", msg);
	return (DLPIAPI_SYSERR);
}

/** @brief decode primtiive and output to syslog.
  * @param msg message to prefix to output.
  */
int
__dlpi_decode_ctl(char *msg)
{
	return _dlpi_decode_ctl(msg);
}

/** @brief The reentrant version of __dlpi_decode_ctl().
  * @param msg message to prefix to output.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This is an implementation of dlpi_decode_ctl().
  */
int
__dlpi_decode_ctl_r(char *msg)
{
	return _dlpi_decode_ctl(msg);
}

/** @fn int dlpi_decode_ctl(char *msg)
  * @param msg message to prefix to output.
  * @version DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_decode_ctl_r().
  */
__asm__(".symver __dlpi_decode_ctl_r,dlpi_decode_ctl@@DLPIAPI_1.0");

static int
_dlpi_clear_zombies(void)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_clear_zombies(void)
{
	return _dlpi_clear_zombies();
}

int
__dlpi_clear_zombies_r(void)
{
	return _dlpi_clear_zombies();
}

__asm__(".symver __dlpi_clear_zombies_r,dlpi_clear_zombies@@DLPIAPI_1.0");

static int
_dlpi_configure_dlsaps(int fd, ulong local_dlsap, ulong remote_dlsap)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_configure_dlsaps(int fd, ulong local_dlsap, ulong remote_dlsap)
{
	return _dlpi_configure_dlsaps(fd, local_dlsap, remote_dlsap);
}
int
__dlpi_configure_dlsaps_r(int fd, ulong local_dlsap, ulong remote_dlsap)
{
	return _dlpi_configure_dlsaps(fd, local_dlsap, remote_dlsap);
}

__asm__(".symver __dlpi_configure_dlsaps_r,dlpi_configure_dlsaps@@DLPIAPI_1.0");

static int
_dlpi_connect(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_connect(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap)
{
	return _dlpi_connect(ppa, bind_dlsap, local_dlsap, remote_dlsap);
}
int
__dlpi_connect_r(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap)
{
	return _dlpi_connect(ppa, bind_dlsap, local_dlsap, remote_dlsap);
}

__asm__(".symver __dlpi_connect_r,dlpi_connect@@DLPIAPI_1.0");

static int
_dlpi_connect_wait(int data)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_connect_wait(int data)
{
	return _dlpi_connect_wait(data);
}
int
__dlpi_connect_wait_r(int data)
{
	return _dlpi_connect_wait(data);
}

__asm__(".symver __dlpi_connect_wait_r,dlpi_connect_wait@@DLPIAPI_1.0");

static char *
_dlpi_decode_disconnect_reason(long reason)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (NULL);
}

char *
__dlpi_decode_disconnect_reason(long reason)
{
	return _dlpi_decode_disconnect_reason(reason);
}

char *
__dlpi_decode_disconnect_reason_r(long reason)
{
	return _dlpi_decode_disconnect_reason(reason);
}

__asm__(".symver __dlpi_decode_disconnect_reason_r,dlpi_decode_disconnect_reason@@DLPIAPI_1.0");

static int
_dlpi_discon_req(int fd, int reason)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_discon_req(int fd, int reason)
{
	return _dlpi_discon_req(fd, reason);
}

int
__dlpi_discon_req_r(int fd, int reason)
{
	return _dlpi_discon_req(fd, reason);
}

__asm__(".symver __dlpi_discon_req_r,dlpi_discon_req@@DLPIAPI_1.0");

static int
_dlpi_get_info(char *ptr)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_get_info(char *ptr)
{
	return _dlpi_get_info(ptr);
}

int
__dlpi_get_info_r(char *ptr)
{
	return _dlpi_get_info(ptr);
}

__asm__(".symver __dlpi_get_info_r,dlpi_get_info@@DLPIAPI_1.0");

static int
_dlpi_get_style(void)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_get_style(void)
{
	return _dlpi_get_style();
}

int
__dlpi_get_style_r(void)
{
	return _dlpi_get_style();
}

__asm__(".symver __dlpi_get_style_r,dlpi_get_style@@DLPIAPI_1.0");

static int
_dlpi_listen(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap, uint fork_options)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_listen(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap, uint fork_options)
{
	return _dlpi_listen(ppa, bind_dlsap, local_dlsap, remote_dlsap, fork_options);
}

int
__dlpi_listen_r(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap,
		uint fork_options)
{
	return _dlpi_listen(ppa, bind_dlsap, local_dlsap, remote_dlsap, fork_options);
}

__asm__(".symver __dlpi_listen_r,dlpi_listen@@DLPIAPI_1.0");

static int
_dlpi_open_log(void)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_open_log(void)
{
	return _dlpi_open_log();
}

int
__dlpi_open_log_r(void)
{
	return _dlpi_open_log();
}

__asm__(".symver __dlpi_open_log_r,dlpi_open_log@@DLPIAPI_1.0");

static int
_dlpi_rcv(int fd, char *data_ptr, int bfr_len, int flags, long *out_code)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_rcv(int fd, char *data_ptr, int bfr_len, int flags, long *out_code)
{
	return _dlpi_rcv(fd, data_ptr, bfr_len, flags, out_code);
}

int
__dlpi_rcv_r(int fd, char *data_ptr, int bfr_len, int flags, long *out_code)
{
	return _dlpi_rcv(fd, data_ptr, bfr_len, flags, out_code);
}

__asm__(".symver __dlpi_rcv_r,dlpi_rcv@@DLPIAPI_1.0");

static int
_dlpi_rcv_msg(int fd, char *data_ptr, int data_cnt, int flags)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_rcv_msg(int fd, char *data_ptr, int data_cnt, int flags)
{
	return _dlpi_rcv_msg(fd, data_ptr, data_cnt, flags);
}

int
__dlpi_rcv_msg_r(int fd, char *data_ptr, int data_cnt, int flags)
{
	return _dlpi_rcv_msg(fd, data_ptr, data_cnt, flags);
}

__asm__(".symver __dlpi_rcv_msg_r,dlpi_rcv_msg@@DLPIAPI_1.0");

static int
_dlpi_read_data(int fd, char *buf, int cnt)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_read_data(int fd, char *buf, int cnt)
{
	return _dlpi_read_data(fd, buf, cnt);
}
int
__dlpi_read_data_r(int fd, char *buf, int cnt)
{
	return _dlpi_read_data(fd, buf, cnt);
}

__asm__(".symver __dlpi_read_data_r,dlpi_read_data@@DLPIAPI_1.0");

static int
_dlpi_reset_req(int fd)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_reset_req(int fd)
{
	return _dlpi_reset_req(fd);
}
int
__dlpi_reset_req_r(int fd)
{
	return _dlpi_reset_req(fd);
}

__asm__(".symver __dlpi_reset_req_r,dlpi_reset_req@@DLPIAPI_1.0");

static int
_dlpi_reset_res(int fd)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_reset_res(int fd)
{
	return _dlpi_reset_res(fd);
}
int
__dlpi_reset_res_r(int fd)
{
	return _dlpi_reset_res(fd);
}

__asm__(".symver __dlpi_reset_res_r,dlpi_reset_res@@DLPIAPI_1.0");

static int
_dlpi_send_uic(int fd, char *datap, int data_len, unsigned char *addr_ptr, int addr_len)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_send_uic(int fd, char *datap, int data_len, unsigned char *addr_ptr, int addr_len)
{
	return _dlpi_send_uic(fd, datap, data_len, addr_ptr, addr_len);
}
int
__dlpi_send_uic_r(int fd, char *datap, int data_len, unsigned char *addr_ptr, int addr_len)
{
	return _dlpi_send_uic(fd, datap, data_len, addr_ptr, addr_len);
}

__asm__(".symver __dlpi_send_uic_r,dlpi_send_uic@@DLPIAPI_1.0");

static int
_dlpi_set_signal_handling(int fd, dlpi_sig_func_t func, int sig_num, int primitive_mask)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_set_signal_handling(int fd, dlpi_sig_func_t func, int sig_num, int primitive_mask)
{
	return _dlpi_set_signal_handling(fd, func, sig_num, primitive_mask);
}
int
__dlpi_set_signal_handling_r(int fd, dlpi_sig_func_t func, int sig_num, int primitive_mask)
{
	return _dlpi_set_signal_handling(fd, func, sig_num, primitive_mask);
}

__asm__(".symver __dlpi_set_signal_handling_r,dlpi_set_signal_handling@@DLPIAPI_1.0");

static int
_dlpi_set_unnum_frame_handler(int fd, unnum_frame_t handler)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_set_unnum_frame_handler(int fd, unnum_frame_t handler)
{
	return _dlpi_set_unnum_frame_handler(fd, handler);
}
int
__dlpi_set_unnum_frame_handler_r(int fd, unnum_frame_t handler)
{
	return _dlpi_set_unnum_frame_handler(fd, handler);
}

__asm__(".symver __dlpi_set_unnum_frame_handler_r,dlpi_set_unnum_frame_handler@@DLPIAPI_1.0");

static int
_dlpi_test_req(int fd, ulong pfb, char *datap, int length)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_test_req(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_test_req(fd, pfb, datap, length);
}
int
__dlpi_test_req_r(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_test_req(fd, pfb, datap, length);
}

__asm__(".symver __dlpi_test_req_r,dlpi_test_req@@DLPIAPI_1.0");

static int
_dlpi_test_res(int fd, ulong pfb, char *datap, int length)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_test_res(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_test_res(fd, pfb, datap, length);
}
int
__dlpi_test_res_r(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_test_res(fd, pfb, datap, length);
}

__asm__(".symver __dlpi_test_res_r,dlpi_test_res@@DLPIAPI_1.0");

static int
_dlpi_uic_req(int fd, char *datap, int length)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_uic_req(int fd, char *datap, int length)
{
	return _dlpi_uic_req(fd, datap, length);
}
int
__dlpi_uic_req_r(int fd, char *datap, int length)
{
	return _dlpi_uic_req(fd, datap, length);
}

__asm__(".symver __dlpi_uic_req_r,dlpi_uic_req@@DLPIAPI_1.0");

static int
_dlpi_write_data(int fd, char *buf, int cnt)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_write_data(int fd, char *buf, int cnt)
{
	return _dlpi_write_data(fd, buf, cnt);
}
int
__dlpi_write_data_r(int fd, char *buf, int cnt)
{
	return _dlpi_write_data(fd, buf, cnt);
}

__asm__(".symver __dlpi_write_data_r,dlpi_write_data@@DLPIAPI_1.0");

static int
_dlpi_xid_req(int fd, ulong pfb, char *datap, int length)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_xid_req(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_xid_req(fd, pfb, datap, length);
}
int
__dlpi_xid_req_r(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_xid_req(fd, pfb, datap, length);
}

__asm__(".symver __dlpi_xid_req_r,dlpi_xid_req@@DLPIAPI_1.0");

static int
_dlpi_xid_res(int fd, ulong pfb, char *datap, int length)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_xid_res(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_xid_res(fd, pfb, datap, length);
}
int
__dlpi_xid_res_r(int fd, ulong pfb, char *datap, int length)
{
	return _dlpi_xid_res(fd, pfb, datap, length);
}

__asm__(".symver __dlpi_xid_res_r,dlpi_xid_res@@DLPIAPI_1.0");

static int
_dlpi_xray_req(int fd, int upa, int on_off, int hi_wat, int lo_wat)
{
	errno = ENOTSUP;
	dlerrno = DL_SYSERR;
	return (-1);
}

int
__dlpi_xray_req(int fd, int upa, int on_off, int hi_wat, int lo_wat)
{
	return _dlpi_xray_req(fd, upa, on_off, hi_wat, lo_wat);
}
int
__dlpi_xray_req_r(int fd, int upa, int on_off, int hi_wat, int lo_wat)
{
	return _dlpi_xray_req(fd, upa, on_off, hi_wat, lo_wat);
}

__asm__(".symver __dlpi_xray_req_r,dlpi_xray_req@@DLPIAPI_1.0");

/** @} */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
