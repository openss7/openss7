/*****************************************************************************

 @(#) $RCSfile: dlpiapi.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:21 $

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

 Last Modified $Date: 2008-07-06 14:58:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpiapi.c,v $
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

#ident "@(#) $RCSfile: dlpiapi.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:21 $"

static char const ident[] =
    "$RCSfile: dlpiapi.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:21 $";

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
	unsigned char bind_ack[DLPI_CTL_BUF_SIZE];
	unsigned char conn_con[DLPI_CTL_BUF_SIZE];
	unsigned char conn_ind[DLPI_CTL_BUF_SIZE];
	int data_cnt;
	int ctl_cnt;
	unsigned char data_buf[DLPI_DATA_BUF_SIZE];
	unsigned char ctl_buf[DLPI_CTL_BUF_SIZE];
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
__dlpi_tsd_alloc(void)
{
	int ret;
	void *buf;

	ret = pthread_key_create(&__dlpi_tsd_key, __dlpi_tsd_free);
	buf = malloc(sizeof(struct __dlpi_tsd));
	bzero(buf, sizeof(*buf));
	ret = pthread_setspecific(__dlpi_tsd_key, buf);
	return;
}

static struct __dlpi_tsd *
__dlpi_get_tsd(void)
{
	pthread_once(&__dlpi_tsd_once, __dlpi_tsd_alloc);
	return (struct __dlpi_tsd *) pthread_getspecific(__dlpi_tsd_key);
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
__dlpi_lock_rdlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__dlpi_lock_wrlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline void
__dlpi_lock_unlock(pthread_rwlock_t * rwlock)
{
	pthread_rwlock_unlock(rwlock);
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
	goto error;
      ebadf:
	errno = EBADF;
	goto error;
      list_lock_error:
	errno = err;
	goto error;
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

	(void) __dlpi_hlduser;
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
	goto error;
      ebadf:
	errno = EBADF;
	goto error;
      list_lock_error:
	errno = err;
	goto error;
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

	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely((du = __dlpi_fds[fd]) == NULL))
		goto ebadf;
	return (0);
      ebadf:
	errno = EBADF;
	goto error;
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
	return (__dlpi_get_tsd()->bind_ack);
}

__asm__(".symver __dlpi_bind_ack,_dlpi_bind_ack@@DLPIAPI_1.0");

unsigned char *
__dlpi_conn_con(void)
{
	return (__dlpi_get_tsd()->conn_con);
}

__asm__(".symver __dlpi_conn_con,_dlpi_conn_con@@DLPIAPI_1.0");

unsigned char *
__dlpi_conn_ind(void)
{
	return (__dlpi_get_tsd()->conn_ind);
}

__asm__(".symver __dlpi_conn_ind,_dlpi_conn_ind@@DLPIAPI_1.0");

int *
__dlpi_data_cnt(void)
{
	return (&(__dlpi_get_tsd()->data_cnt));
}

__asm__(".symver __dlpi_data_cnt,_dlpi_data_cnt@@DLPIAPI_1.0");

int *
__dlpi_ctl_cnt(void)
{
	return (&(__dlpi_get_tsd()->ctl_cnt));
}

__asm__(".symver __dlpi_ctl_cnt,_dlpi_ctl_cnt@@DLPIAPI_1.0");

unsigned char *
__dlpi_data_buf(void)
{
	return (__dlpi_get_tsd()->data_buf);
}

__asm__(".symver __dlpi_data_buf,_dlpi_data_buf@@DLPIAPI_1.0");

unsigned char *
__dlpi_ctl_buf(void)
{
	return (__dlpi_get_tsd()->ctl_buf);
}

__asm__(".symver __dlpi_ctl_buf,_dlpi_ctl_buf@@DLPIAPI_1.0");

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
	switch (errno) {
	case EINTR:
	case ERESTART:
		errno = save;
		return (DLPIAPI_EINTR);
	case EAGAIN:
		errno = save;
		return (DLPIAPI_EAGAIN);
	case ENXIO:
	case EINVAL:
	case EIO:
		errno = save;
		return (DLPIAPI_UNUSABLE);
	}
	return (DLPIAPI_SYSERR);
}

static int
_dlpi_put_both(int fd, char *ctrl_ptr, int ctrl_len, char *data_ptr, int data_len)
{
	struct strbuf ctrl, data, *ctlp, *datp;

	ctrl.buf = ctrl_ptr;
	ctrl.len = ctrl_len;
	ctrl.maxlen = -1;

	data.buf = data_ptr;
	data.len = data_len;
	data.maxlen = -1;

	ctlp = ctrl_ptr ? &ctrl : NULL;
	datp = data_ptr ? &data : NULL;

	return __dlpi_putmsg(fd, ctlp, datp, 0);
}

static int
_dlpi_put_proto(int fd, int length)
{
	return _dlpi_put_both(fd, (char *)dlpi_ctl_buf, length, NULL, 0);
}

static int
_dlpi_complete_req(int fd, int request, char *caller, int discard)
{
	union DL_primitives *p = (typeof(p)) dlpi_ctl_buf;
	int ret, flag;
	struct pollfd pfd;
	struct strbuf ctrl, data;

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

	ctrl.buf = (char *) dlpi_ctl_buf;
	ctrl.len = -1;
	ctrl.maxlen = DLPI_CTL_BUF_SIZE;

	data.buf = (char *) dlpi_data_buf;
	data.len = -1;
	data.maxlen = DLPI_DATA_BUF_SIZE;

	switch (poll(&pfd, 1, 100)) {
	default:
	case -1:
		return (DLPIAPI_SYSERR);
	case 0:
		return (DLPIAPI_EAGAIN);
	case 1:
		break;
	}

	ret = getmsg(fd, &ctrl, &data, &flag);

	if (ret < 0)
		return (DLPIAPI_SYSERR);
	if (ret != 0)
		goto tryagain;
	if (ctrl.len < sizeof(p->dl_primitive))
		goto tryagain;

	switch (p->dl_primitive) {
	case DL_ERROR_ACK:
		/* handle DL_ERROR_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (ctrl.len < sizeof(p->error_ack))
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
		if (ctrl.len < sizeof(p->ok_ack))
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
		if (ctrl.len < sizeof(p->info_ack))
			goto tryagain;
		if (p->info_ack.dl_addr_length != 0
		    && ctrl.len < p->info_ack.dl_addr_length + p->info_ack.dl_addr_offset)
			goto tryagain;
		if (p->info_ack.dl_qos_length != 0
		    && ctrl.len < p->info_ack.dl_qos_length + p->info_ack.dl_qos_offset)
			goto tryagain;
		if (p->info_ack.dl_qos_range_length != 0
		    && ctrl.len < p->info_ack.dl_qos_range_length + p->info_ack.dl_qos_range_offset)
			goto tryagain;
		if (p->info_ack.dl_brdcst_addr_length != 0
		    && ctrl.len <
		    p->info_ack.dl_brdcst_addr_length + p->info_ack.dl_brdcst_addr_offset)
			goto tryagain;
		return (ret);
	case DL_BIND_ACK:
		/* handle DL_BIND_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_BIND_REQ)
			goto tryagain;
		if (ctrl.len < sizeof(p->bind_ack))
			goto tryagain;
		if (p->bind_ack.dl_addr_length != 0
		    && ctrl.len < p->bind_ack.dl_addr_length + p->bind_ack.dl_addr_offset)
			goto tryagain;
		return (ret);
	case DL_PHYS_ADDR_ACK:
		/* handle DL_PHYS_ADDR_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_PHYS_ADDR_REQ)
			goto tryagain;
		if (ctrl.len < sizeof(p->phys_addr_ack))
			goto tryagain;
		if (p->phys_addr_ack.dl_addr_length != 0
		    && ctrl.len < p->phys_addr_ack.dl_addr_length + p->phys_addr_ack.dl_addr_offset)
			goto tryagain;
		return (ret);
	case DL_GET_STATISTICS_ACK:
		/* handle DL_GET_STATISTICS_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_GET_STATISTICS_REQ)
			goto tryagain;
		if (ctrl.len < sizeof(p->get_statistics_ack))
			goto tryagain;
		if (p->get_statistics_ack.dl_stat_length != 0
		    && ctrl.len <
		    p->get_statistics_ack.dl_stat_length + p->get_statistics_ack.dl_stat_offset)
			goto tryagain;
		return (ret);
	case DL_NOTIFY_ACK:
		/* handle DL_NOTIFY_ACK reply */
		if (flag != RS_HIPRI)
			goto tryagain;
		if (request != DL_NOTIFY_REQ)
			goto tryagain;
		if (ctrl.len < sizeof(p->notify_req))
			goto tryagain;
		return (ret);
	case DL_CONNECT_CON:
		/* handle DL_CONNECT_CON reply */
		if (flag != 0)
			goto tryagain;
		if (request != DL_CONNECT_REQ)
			goto tryagain;
		if (ctrl.len < sizeof(p->connect_con))
			goto tryagain;
		if (p->connect_con.dl_resp_addr_length != 0
		    && ctrl.len <
		    p->connect_con.dl_resp_addr_length + p->connect_con.dl_resp_addr_offset)
			goto tryagain;
		return (ret);
	case DL_RESET_CON:
		/* handle DL_RESET_CON reply */
		if (flag != 0)
			goto tryagain;
		if (request != DL_RESET_REQ)
			goto tryagain;
		if (ctrl.len < sizeof(p->reset_con))
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

__asm__(".symver __dlpi_send_attach_req,dlpi_send_attach_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_attach_req_r,dlpi_send_attach_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_bind_req,dlpi_send_bind_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_bind_req_r,dlpi_send_bind_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_connect_req,dlpi_send_connect_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_connect_req_r,dlpi_send_connect_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_connect_res,dlpi_send_connect_res@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_connect_res_r,dlpi_send_connect_res_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_detach_req,dlpi_send_detach_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_detach_req_r,dlpi_send_detach_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_disconnect_req,dlpi_send_disconnect_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_disconnect_req_r,dlpi_send_disconnect_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_info_req,dlpi_send_info_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_info_req_r,dlpi_send_info_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_reset_req,dlpi_send_reset_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_reset_req_r,dlpi_send_reset_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_reset_res,dlpi_send_reset_res@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_reset_res_r,dlpi_send_reset_res_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_stats_req,dlpi_send_stats_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_stats_req_r,dlpi_send_stats_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_test_req,dlpi_send_test_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_test_req_r,dlpi_send_test_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_test_res,dlpi_send_test_res@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_test_res_r,dlpi_send_test_res_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_unbind_req,dlpi_send_unbind_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_unbind_req_r,dlpi_send_unbind_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_xid_req,dlpi_send_xid_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_xid_req_r,dlpi_send_xid_req_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_xid_res,dlpi_send_xid_res@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_send_xid_res_r,dlpi_send_xid_res_r@@DLPIAPI_1.0");

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

/** @fn int dlpi_attach_ppa(int fd, unsigned long ppa)
  * @brief attach a DLPI Stream to a PPA.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  * @version: DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_attach_ppa().
  */
__asm__(".symver __dlpi_attach_ppa,dlpi_attach_ppa@@DLPIAPI_1.0");

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

/** @fn int dlpi_attach_ppa_r(int fd, unsigned long ppa)
  * @brief attach a DLPI Stream to a PPA.
  * @param fd the DLPI Stream.
  * @param ppa the PPA to attach.
  * @version: DLPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_attach_ppa_r().
  */
__asm__(".symver __dlpi_attach_ppa_r,dlpi_attach_ppa_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_detach_ppa,dlpi_detach_ppa@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_detach_ppa_r,dlpi_detach_ppa_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_bind_dlsap,dlpi_bind_dlsap@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_bind_dlsap_r,dlpi_bind_dlsap_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_unbind_dlsap,dlpi_unbind_dlsap@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_unbind_dlsap_r,dlpi_unbind_dlsap_r@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_connect_req,dlpi_connect_req@@DLPIAPI_1.0");

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

__asm__(".symver __dlpi_connect_req_r,dlpi_connect_req_r@@DLPIAPI_1.0");

__asm__(".symver __dlpi_open_GCOM_r,dlpi_open_GCOM@@DLPIAPI_1.0");
__asm__(".symver __dlpi_close_GCOM_r,dlpi_close_GCOM@@DLPIAPI_1.0");

/** @} */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
