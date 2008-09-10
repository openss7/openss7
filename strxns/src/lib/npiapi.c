/*****************************************************************************

 @(#) $RCSfile: npiapi.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2008-09-10 03:50:08 $

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

 Last Modified $Date: 2008-09-10 03:50:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npiapi.c,v $
 Revision 0.9.2.8  2008-09-10 03:50:08  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.7  2008-08-20 10:57:05  brian
 - fixes and build updates from newnet trip

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

#ident "@(#) $RCSfile: npiapi.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2008-09-10 03:50:08 $"

static char const ident[] =
    "$RCSfile: npiapi.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2008-09-10 03:50:08 $";

/*
 * This is an OpenSS7 implemetnation of the GCOM npiapi library.  It builds
 * and installs as both a shared object library as well as a static library.
 * GCOM originally implemented this library as a static library only called
 * npiapi.a.
 */

/* This file can be processed with doxygen(1). */

/** @weakgroup npi OpenSS7 NPI API Library
  * @{ */

/** @file
  * OpenSS7 Network Provider Interface (NPI) API Library implementation file. */

#define _XOPEN_SOURCE	600
#define _REENTRANT
#define _THREAD_SAFE

/** @name Buffer Lengths
  * These are the default buffer lengths for the corresponding buffers.
  * @{ */
/** @} */

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

#include <sys/npi.h>
#include <npiapi.h>

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

/**
  * This manual contains documentation of the OpenSS7 NPI Library functions
  * that are generated automatically from the source code with doxygen(1).
  * This documentation is intended to be used for maintainers of the OpenSS7
  * NPI Library and is not intended for users of the OpenSS7 NPI Library.
  * Users should consult the documentation found in the user manual pages
  * begining with npi(3), or in the texinfo manuals.
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 NPI Library is designed to be thread-safe.  This is
  * accomplished in a number of ways.  Thread-safety depends on the use of
  * glibc2 and the pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data for errno.  So does
  * libnpi.
  *
  * Glibc2 also provides some weak undefined aliases for POSIX thread
  * functions to perform its own thread-safety.  When the pthread library
  * (libpthread) is linked with glibc2, these function call libpthread
  * functions instead of internal dummy routines.  The same approach is taken
  * by the OpenSS7 NPI Library.  The library uses weak defined and undefined
  * aliases that automatically invoke libpthread functions when libpthread is
  * (dynamically) linked and uses dummy libpthread functions when it is not.
  * This maintains maximum efficiency when libpthread is not dynamically
  * linked, but provides full thread-safety when it is.
  *
  * Libpthread behaves in some strange ways with regards to thread
  * cancellation.  Because libpthread uses Linux clone processes for threads,
  * cancellation of a thread is accomplished by sending a signal to the thread
  * process.  This does not directly result in cancellation, but will result
  * in the failure of a system call with the EINTR error code.  It is
  * necessary to test for cancellation upon error return from system calls to
  * perform the acutal cancellation of the thread.
  *
  * Any command where the caller might block is also given a thread
  * cancellation point.  Other NPI functions are not permitted to be thread
  * cancellation points.  Locks and asynchronous thread cancellation present
  * challenges:
  *
  * - Functions that act as thread cancellation points must push routines onto
  *   the function stack executed at exit of the thread to release the locks
  *   held by the function.  These are performed with weak definitions of
  *   POSIX thread library functions.
  *
  * - Functions that do not act as thread cancellation points must defer
  *   thread cancellation before taking locks and then release locks before
  *   thread cancellation is restored.
  *
  * The above are the techniques used by glibc2 for the same purpose as in the
  * same technique that is used by the OpenSS7 NPI Library.
  */

/** @brief thread-specific data
  */
struct __npi_tsd {
	int _nerrno;
	char strbuf[BUFSIZ];		/* string buffer */
	unsigned char tsd_bind_ack[NPI_CTL_BUF_SIZE];
	unsigned char tsd_conn_con[NPI_CTL_BUF_SIZE];
	unsigned char tsd_conn_ind[NPI_CTL_BUF_SIZE];
	int tsd_data_cnt;
	int tsd_ctl_cnt;
	unsigned char tsd_data_buf[NPI_DATA_BUF_SIZE];
	unsigned char tsd_ctl_buf[NPI_CTL_BUF_SIZE];
	int tsd_conn_ind_data_size;
	int tsd_conn_ind_data_skip;
	int tsd_disc_ind_data_size;
	int tsd_disc_ind_data_skip;
	int tsd_conn_con_data_size;
	int tsd_conn_con_data_skip;
	int tsd_discon_req_band;
	int tsd_reset_req_band;
	int tsd_flow_req_band;
	int tsd_data_req_band;
	int tsd_exdata_req_band;
	int tsd_datack_req_band;
	int tsd_other_req_band;
};

/** @brief once condition for Thread-Specific Data key creation.
  */
static pthread_once_t __npi_tsd_once = PTHREAD_ONCE_INIT;

/** @brief NPI Library Thread-Specific Data Key
  */
static pthread_key_t __npi_tsd_key = 0;

static void
__npi_tsd_free(void *buf)
{
	pthread_setspecific(__npi_tsd_key, NULL);
	free(buf);
}

static void
__npi_tsd_key_create(void)
{
	pthread_key_create(&__npi_tsd_key, __npi_tsd_free);
}

static struct __npi_tsd *
__npi_tsd_alloc(void)
{
	struct __npi_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(*tsdp));
	memset(tsdp, 0, sizeof(*tsdp));
	pthread_setspecific(__npi_tsd_key, (void *) tsdp);
	return (tsdp);
}

static struct __npi_tsd *
__npi_get_tsd(void)
{
	struct __npi_tsd *tsdp;

	pthread_once(&__npi_tsd_once, __npi_tsd_key_create);
	if (unlikely((tsdp = (typeof(tsdp)) pthread_getspecific(__npi_tsd_key)) == NULL))
		tsdp = __npi_tsd_alloc();
	return (tsdp);
}

/** @brief #nerrno location function.
  * @version NPIAPI_1.0
  * @par Alias:
  * This function is an implementation of _nerrno().
  */
__hot int *
__npi__nerrno(void)
{
	return (&__npi_get_tsd()->_nerrno);
}

/** @fn int *_nerrno(void)
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi__nerrno().
  *
  * This function provides the location of the integer that contains the NPI
  * API error number returned by the last NPI API function that failed.  This
  * is normally used to provide #nerrno in a thread-safe way as follows:
  *
  * @code
  * #define nerrno (*(_nerrno()))
  * @endcode
  */
__asm__(".symver __npi__nerrno,_nerrno@@NPIAPI_1.0");

#ifndef nerrno
#define nerrno (*(_nerrno()))
#endif

struct __npi_user {
	pthread_rwlock_t nu_lock;		/**< lock for this structure */
	int nu_fd;
	N_info_ack_t nu_info;
};

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

static struct __npi_user *__npi_fds[OPEN_MAX] = { NULL, };

static pthread_rwlock_t __npi_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static inline int
__npi_lock_init(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_init(rwlock, NULL);
}
static inline int
__npi_lock_rdlock(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__npi_lock_wrlock(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline void
__npi_lock_unlock(void *rwlock)
{
	pthread_rwlock_unlock(rwlock);
}
static inline int
__npi_lock_destroy(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_destroy(rwlock);
}
static inline int
__npi_list_rdlock(void)
{
	return __npi_lock_rdlock(&__npi_fd_lock);
}
static inline int
__npi_list_wrlock(void)
{
	return __npi_lock_wrlock(&__npi_fd_lock);
}
static void
__npi_list_unlock(void *ignore)
{
	return __npi_lock_unlock(&__npi_fd_lock);
}
static inline int
__npi_user_destroy(int fd)
{
	struct __npi_user *nu;
	int err;

	if (unlikely((nu = __npi_fds[fd]) == NULL)) {
		errno = ENXIO;
		nerrno = NSYSERR;
		return (-1);
	}
	if (unlikely((err = __npi_lock_destroy(&nu->nu_lock)) != 0)) {
		errno = err;
		nerrno = NSYSERR;
		return (-1);
	}
	__npi_fds[fd] = NULL;
	free(nu);
	return (0);
}
static inline struct __npi_user *
__npi_user_init(int fd)
{
	struct __npi_user *nu;
	int err;

	if (unlikely((nu = (struct __npi_user *)malloc(sizeof(*nu))) == NULL)) {
		nerrno = NSYSERR;
		return (nu);
	}
	memset(nu, 0, sizeof(*nu));
	if (unlikely((err = __npi_lock_init(&nu->nu_lock)) != 0)) {
		free(nu);
		errno = err;
		nerrno = NSYSERR;
		return (NULL);
	}
	/** At one the NPI API library did not have a
	  * cdi_close() function, meaning that some older
	  * applications programs might just do a close(2s) on
	  * an existing Stream.
	  */
	if (unlikely(__npi_fds[fd] != NULL))
		__npi_user_destroy(fd);
	nu->nu_fd = fd;
	__npi_fds[fd] = nu;
	return (nu);
}
static inline int
__npi_user_rdlock(struct __npi_user *nu)
{
	return __npi_lock_rdlock(&nu->nu_lock);
}
static inline int
__npi_user_wrlock(struct __npi_user *nu)
{
	return __npi_lock_wrlock(&nu->nu_lock);
}
static inline void
__npi_user_unlock(struct __npi_user *nu)
{
	return __npi_lock_unlock(&nu->nu_lock);
}

static int __npi_initialized = 0;

/* Forware declarations of internal functions. */
static __hot void __npi_putuser(void *arg);
static __hot int __npi_getuser(int fd);
static __hot int __npi_hlduser(int fd);
static __hot int __npi_chkuser(int fd);

/** @internal
  * @brief release a user structure.
  * @param arg the user file descriptor.
  *
  * This is a release function for releasing a library user structure.  Its
  * prototype allows it to be used as a pthread pop function.
  */
static __hot void
__npi_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct __npi_user *nu = __npi_fds[fd];

	__npi_user_unlock(nu);
	__npi_list_unlock(NULL);
	return;
}

/** @internal
  * @brief Get a write locked network user structure.
  * @param fd the file descriptor for which to get the associated network.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe operation.
  *
  * [EAGAIN] the number of read locks on the list is excessive.
  * [EDEADLK] the list of fd lock is already held by the calling thread.
  */
static __hot int
__npi_getuser(int fd)
{
	struct __npi_user *nu;
	int err;

	if (unlikely(__npi_initialized == 0))
		goto uninit;
	if (unlikely((err = __npi_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto nbadf;
	if (unlikely(!(nu = __npi_fds[fd])))
		goto nbadf;
	if (unlikely((err = __npi_user_wrlock(nu))))
		goto user_lock_error;
	return (0);
      user_lock_error:
	__npi_list_unlock(NULL);
	errno = err;
	nerrno = NSYSERR;
	goto error;
      nbadf:
	errno = EBADF;
	nerrno = NSYSERR;
	goto error;
      list_lock_error:
	errno = err;
	nerrno = NSYSERR;
	goto error;
      uninit:
	errno = ELIBACC;
	nerrno = NSYSERR;
	return (-1);
      error:
	return (-1);

}

/** @internal
  * @brief Get a read locked network user structure.
  * @param fd the file descriptor for which to get the associated network.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe write operation.
  *
  * [EAGAIN] the number of read locks on the list is excessive.
  * [EDEADLK] the list of fd lock is already held by the calling thread.
  */
static __hot int
__npi_hlduser(int fd)
{
	struct __npi_user *nu;
	int err;

	if (unlikely(__npi_initialized == 0))
		goto uninit;
	if (unlikely((err = __npi_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely((nu = __npi_fds[fd]) == NULL))
		goto ebadf;
	if (unlikely((err = __npi_user_rdlock(nu))))
		goto user_lock_error;
	return (0);
      user_lock_error:
	__npi_list_unlock(NULL);
	errno = err;
	nerrno = NSYSERR;
	goto error;
      ebadf:
	errno = EBADF;
	nerrno = NSYSERR;
	goto error;
      list_lock_error:
	errno = err;
	nerrno = NSYSERR;
	goto error;
      uninit:
	errno = ELIBACC;
	nerrno = NSYSERR;
	return (-1);
      error:
	return (-1);
}

/** @internal
  * @brief Check a network user structure.
  * @param fd the file descriptor for which to check the associated network.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  This function takes no
  * locks.  It is primarily called by the non-reentrant versions of the
  * library functions.
  */
static __hot int
__npi_chkuser(int fd)
{
	struct __npi_user *nu;

	if (unlikely(__npi_initialized == 0))
		goto uninit;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely((nu = __npi_fds[fd]) == NULL))
		goto ebadf;
	return (0);
      ebadf:
	errno = EBADF;
	nerrno = NSYSERR;
	goto error;
      uninit:
	errno = ELIBACC;
	nerrno = NSYSERR;
	return (-1);
      error:
	return (-1);
}

/* These are supposed global variables that are actually implemented as
 * thread-specific data. */

extern unsigned char *_npi_bind_ack(void);
extern unsigned char *_npi_conn_con(void);
extern unsigned char *_npi_conn_ind(void);
extern int *_npi_data_cnt(void);
extern int *_npi_ctl_cnt(void);
extern unsigned char *_npi_data_buf(void);
extern unsigned char *_npi_ctl_buf(void);
extern int *_npi_conn_ind_data_size(void);
extern int *_npi_conn_ind_data_skip(void);
extern int *_npi_disc_ind_data_size(void);
extern int *_npi_disc_ind_data_skip(void);
extern int *_npi_conn_con_data_size(void);
extern int *_npi_conn_con_data_skip(void);
extern int *_npi_discon_req_band(void);
extern int *_npi_reset_req_band(void);
extern int *_npi_flow_req_band(void);
extern int *_npi_data_req_band(void);
extern int *_npi_exdata_req_band(void);
extern int *_npi_datack_req_band(void);
extern int *_npi_other_req_band(void);

unsigned char *
__npi_bind_ack(void)
{
	return (__npi_get_tsd()->tsd_bind_ack);
}

__asm__(".symver __npi_bind_ack,_npi_bind_ack@@NPIAPI_1.0");

unsigned char *
__npi_conn_con(void)
{
	return (__npi_get_tsd()->tsd_conn_con);
}

__asm__(".symver __npi_conn_con,_npi_conn_con@@NPIAPI_1.0");

unsigned char *
__npi_conn_ind(void)
{
	return (__npi_get_tsd()->tsd_conn_ind);
}

__asm__(".symver __npi_conn_ind,_npi_conn_ind@@NPIAPI_1.0");

int *
__npi_data_cnt(void)
{
	return (&__npi_get_tsd()->tsd_data_cnt);
}

__asm__(".symver __npi_data_cnt,_npi_data_cnt@@NPIAPI_1.0");

int *
__npi_ctl_cnt(void)
{
	return (&__npi_get_tsd()->tsd_ctl_cnt);
}

__asm__(".symver __npi_ctl_cnt,_npi_ctl_cnt@@NPIAPI_1.0");

unsigned char *
__npi_data_buf(void)
{
	return (__npi_get_tsd()->tsd_data_buf);
}

__asm__(".symver __npi_data_buf,_npi_data_buf@@NPIAPI_1.0");

unsigned char *
__npi_ctl_buf(void)
{
	return (__npi_get_tsd()->tsd_ctl_buf);
}

__asm__(".symver __npi_ctl_buf,_npi_ctl_buf@@NPIAPI_1.0");

int *
__npi_conn_ind_data_size(void)
{
	return (&__npi_get_tsd()->tsd_conn_ind_data_size);
}

__asm__(".symver __npi_conn_ind_data_size,_npi_conn_ind_data_size@@NPIAPI_1.0");

int *
__npi_conn_ind_data_skip(void)
{
	return (&__npi_get_tsd()->tsd_conn_ind_data_skip);
}

__asm__(".symver __npi_conn_ind_data_skip,_npi_conn_ind_data_skip@@NPIAPI_1.0");

int *
__npi_disc_ind_data_size(void)
{
	return (&__npi_get_tsd()->tsd_disc_ind_data_size);
}

__asm__(".symver __npi_disc_ind_data_size,_npi_disc_ind_data_size@@NPIAPI_1.0");

int *
__npi_disc_ind_data_skip(void)
{
	return (&__npi_get_tsd()->tsd_disc_ind_data_skip);
}

__asm__(".symver __npi_disc_ind_data_skip,_npi_disc_ind_data_skip@@NPIAPI_1.0");

int *
__npi_conn_con_data_size(void)
{
	return (&__npi_get_tsd()->tsd_conn_con_data_size);
}

__asm__(".symver __npi_conn_con_data_size,_npi_conn_con_data_size@@NPIAPI_1.0");

int *
__npi_conn_con_data_skip(void)
{
	return (&__npi_get_tsd()->tsd_conn_con_data_skip);
}

__asm__(".symver __npi_conn_con_data_skip,_npi_conn_con_data_skip@@NPIAPI_1.0");

int *
__npi_discon_req_band(void)
{
	return (&__npi_get_tsd()->tsd_discon_req_band);
}

__asm__(".symver __npi_discon_req_band,_npi_discon_req_band@@NPIAPI_1.0");

int *
__npi_reset_req_band(void)
{
	return (&__npi_get_tsd()->tsd_reset_req_band);
}

__asm__(".symver __npi_reset_req_band,_npi_reset_req_band@@NPIAPI_1.0");

int *
__npi_flow_req_band(void)
{
	return (&__npi_get_tsd()->tsd_flow_req_band);
}

__asm__(".symver __npi_flow_req_band,_npi_flow_req_band@@NPIAPI_1.0");

int *
__npi_data_req_band(void)
{
	return (&__npi_get_tsd()->tsd_data_req_band);
}

__asm__(".symver __npi_data_req_band,_npi_data_req_band@@NPIAPI_1.0");

int *
__npi_exdata_req_band(void)
{
	return (&__npi_get_tsd()->tsd_exdata_req_band);
}

__asm__(".symver __npi_exdata_req_band,_npi_exdata_req_band@@NPIAPI_1.0");

int *
__npi_datack_req_band(void)
{
	return (&__npi_get_tsd()->tsd_datack_req_band);
}

__asm__(".symver __npi_datack_req_band,_npi_datack_req_band@@NPIAPI_1.0");

int *
__npi_other_req_band(void)
{
	return (&__npi_get_tsd()->tsd_other_req_band);
}

__asm__(".symver __npi_other_req_band,_npi_other_req_band@@NPIAPI_1.0");


static int __npi_log_options = NPI_LOG_DEFAULT;

/** @internal
  * @brief perform putmsg() with NPI API errors.
  * @param fd the NPI Stream.
  * @param ctlp control part.
  * @param datp data part.
  * @param band putpmsg band.
  * @param flag putpmsg flag.
  */
static int
__npi_putpmsg(int fd, struct strbuf *ctlp, struct strbuf *datp, int band, int flag)
{
	int ret, save;

	save = errno;
	do {
		if (likely((ret = putpmsg(fd, ctlp, datp, band, flag)) >= 0)) {
			errno = save;
			return (ret);
		}
	} while (errno == EINTR || errno == ERESTART);
	nerrno = NSYSERR;
	return (-1);
}

/** @internal
  * @brief put control and data
  * @param fd the NPI Stream.
  * @param ctrl_ptr control portion buffer.
  * @param ctrl_len control portion length.
  * @param data_ptr data portion buffer.
  * @param data_len data portion length.
  * @param band band for message.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_npi_put_both(int fd, char *ctrl_ptr, int ctrl_len, char *data_ptr, int data_len, int band)
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
	return __npi_putpmsg(fd, ctlp, datp, band, MSG_BAND);
}

/** @internal
  * @brief put a control message
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the control message.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_npi_put_proto(int fd, int nbytes)
{
	return _npi_put_both(fd, (char *) npi_ctl_buf, nbytes, NULL, 0, 0);
}

/** @brief put a control message
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the control message.
  *
  * Place an M_PROTO message to the Stream.  The message is assumed contained
  * in npi_ctl_buf and with a length of nbytes.  The return value from the
  * subroutine is the return value from the putmsg(2s) call.  When an error
  * occurs an error message is printed to the log file.
  */
int
__npi_put_proto(int fd, int nbytes)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_put_proto(fd, nbytes);
}

/** @brief The reentrant version of __npi_put_proto().
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the control message.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_put_proto().
  */
int
__npi_put_proto_r(int fd, int nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_hlduser(fd)) >= 0)) {
		err = _npi_put_proto(fd, nbytes);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int npi_put_proto(int fd, int nbytes)
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the control message.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_put_proto_r().
  */
__asm__(".symver __npi_put_proto_r,npi_put_proto@@NPIAPI_1.0");

/** @internal
  * @brief put a data message
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the data message.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_npi_put_data_buf(int fd, int nbytes)
{
	return _npi_put_both(fd, NULL, 0, (char *)npi_data_buf, nbytes, 0);
}

/** @brief put a data message
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the data message.
  *
  * This is really just a ridiculous wrapper for putmsg(2s).
  */
int
__npi_put_data_buf(int fd, int nbytes)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_put_data_buf(fd, nbytes);
}

/** @brief The reentrant version of __npi_put_data_buf().
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the data message.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_put_data_buf().
  */
int
__npi_put_data_buf_r(int fd, int nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_hlduser(fd)) >= 0)) {
		err = _npi_put_data_buf(fd, nbytes);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int npi_put_data_buf(int fd, int nbytes)
  * @param fd the NPI Stream.
  * @param nbytes the number of bytes in the data message.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_put_data_buf_r().
  */
__asm__(".symver __npi_put_data_buf_r,npi_put_data_buf@@NPIAPI_1.0");

/** @internal
  * @brief put a control and data message to the Stream
  * @param fd the Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @param flags flags associated with the data.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_npi_put_data_proto(int fd, char *buf, int nbytes, long flags)
{
	N_data_req_t *p = (typeof(p)) npi_ctl_buf;

	p->PRIM_type = N_DATA_REQ;
	p->DATA_xfer_flags = flags;

	return _npi_put_both(fd, (char *)p, sizeof(*p), buf, nbytes, 0);
}

/** @brief put an M_DATA and M_PROTO message to the Stream.
  * @param fd the Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @param flags flags associated with the data.
  *
  * GCOM documents that this subroutine sends the data and also the primitive
  * contained in the global (thread-specific) npi_ctl_buf.  The return value
  * is the return from the putmsg(2s) call.
  */
int
__npi_put_data_proto(int fd, char *buf, int nbytes, long flags)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_put_data_proto(fd, buf, nbytes, flags);
}

/** @brief The reentrant version of __npi_put_data_proto().
  * @param fd the Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @param flags flags associated with the data.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_put_data_proto().
  */
int
__npi_put_data_proto_r(int fd, char *buf, int nbytes, long flags)
{
	int err;
	
	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_hlduser(fd)) >= 0)) {
		err = _npi_put_data_proto(fd, buf, nbytes, flags);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int npi_put_data_proto(int fd, char *buf, int nbytes, long flags)
  * @param fd the Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @param flags flags associated with the data.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_put_data_proto_r().
  */
__asm__(".symver __npi_put_data_proto_r,npi_put_data_proto@@NPIAPI_1.0");

/** @internal
  * @brief put control and data message to the Stream
  * @param fd the NPI Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_npi_put_exdata_proto(int fd, char *buf, int nbytes)
{
	N_exdata_req_t *p = (typeof(p)) npi_ctl_buf;

	p->PRIM_type = N_EXDATA_REQ;

	return _npi_put_both(fd, (char *)p, sizeof(*p), buf, nbytes, 1);
}

/** @brief put control and data message to the Stream
  * @param fd the NPI Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  */
int
__npi_put_exdata_proto(int fd, char *buf, int nbytes)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_put_exdata_proto(fd, buf, nbytes);
}

/** @brief The reentrant version of __npi_put_exdata_proto().
  * @param fd the NPI Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_put_exdata_proto().
  */
int
__npi_put_exdata_proto_r(int fd, char *buf, int nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_hlduser(fd)) >= 0)) {
		err = _npi_put_exdata_proto(fd, buf, nbytes);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int npi_put_exdata_proto(int fd, char *buf, int nbytes)
  * @param fd the NPI Stream.
  * @param buf the data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_put_exdata_proto_r().
  */
__asm__(".symver __npi_put_exdata_proto_r,npi_put_exdata_proto@@NPIAPI_1.0");

/** @internal
  * @brief get a message
  * @param fd the NPI Stream.
  * @param buf the buffer for data portion of message.
  * @param nbytes size of buffer.
  *
  * This version of the implementation does not need to check the validity of
  * its arguments.
  */
static int
_npi_get_a_msg(int fd, char *buf, int nbytes)
{
	struct strbuf ctrl, data;
	int ret, flag = 0;

	ctrl.buf = (char *) npi_ctl_buf;
	ctrl.len = -1;
	ctrl.maxlen = NPI_CTL_BUF_SIZE;

	data.buf = (char *)buf;
	data.len = -1;
	data.maxlen = nbytes;

	ret = getmsg(fd, &ctrl, &data, &flag);

	if (ret < 0) {
		nerrno = NSYSERR;
		return (-1);
	}
	npi_ctl_cnt = ctrl.len;
	npi_data_cnt = data.len;
	return (ret);
}

/** @brief get a message
  * @param fd the NPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  */
int
__npi_get_a_msg(int fd, char *buf, int nbytes)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_get_a_msg(fd, buf, nbytes);
}

/** @brief The reentrant version of __npi_get_a_msg().
  * @param fd the NPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_get_a_msg().
  */
int
__npi_get_a_msg_r(int fd, char *buf, int nbytes)
{
	int err;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_hlduser(fd)) >= 0)) {
		err = _npi_get_a_msg(fd, buf, nbytes);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (0);
}

/** @fn int npi_get_a_msg(int fd, char *buf, int nbytes)
  * @param fd the NPI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_get_a_msg_r().
  */
__asm__(".symver __npi_get_a_msg_r,npi_get_a_msg@@NPIAPI_1.0");

/** @internal
  * @brief get a control message
  * @param fd the NPI Stream.
  *
  * This version of the implementation does not have to check for validity of
  * its arguments.
  */
static int
_npi_get_a_proto(int fd)
{
	int ret;

	ret = _npi_get_a_msg(fd, (char *) npi_data_buf, NPI_DATA_BUF_SIZE);
	if (unlikely(ret < 0))
		return (ret);
	if (likely((ret = npi_ctl_cnt) > 0))
		return (ret);
	return (-1);
}

/** @brief get a control message
  * @param fd the NPI Stream.
  */
int
__npi_get_a_proto(int fd)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_get_a_proto(fd);
}

/** @brief The reentrant version of __npi_get_a_proto().
  * @param fd the NPI Stream.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_get_a_proto().
  */
int
__npi_get_a_proto_r(int fd)
{
	int err;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_hlduser(fd)) >= 0)) {
		err = _npi_get_a_proto(fd);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int npi_get_a_proto(int fd)
  * @param fd the NPI Stream.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_get_a_proto_r().
  */
__asm__(".symver __npi_get_a_proto_r,npi_get_a_proto@@NPIAPI_1.0");

/** @internal
  * @brief expect a control message
  * @param fd the NPI Stream.
  * @param primitive expected primitive type.
  *
  * This version of the implementation does not have to check its arguments for
  * validity.
  */
static int
_npi_want_a_proto(int fd, int primitive)
{
	union N_primitives *p = (typeof(p)) npi_ctl_buf;
	int ret, flag, band, request;
	struct pollfd pfd;

      tryagain:
	pfd.fd = fd;
	pfd.events = POLLERR | POLLHUP | POLLMSG;
	pfd.revents = 0;

	flag = MSG_HIPRI;
	band = 0;

	switch (primitive) {
	case N_CONN_IND:
		request = -1;
		goto normal;
	case N_CONN_CON:
		request = N_CONN_REQ;
		goto normal;
	case N_DISCON_IND:
		request = -1;
		goto normal;
	case N_DATA_IND:
		request = -1;
		goto normal;
	case N_UNITDATA_IND:
		request = -1;
		goto normal;
	case N_UDERROR_IND:
		request = -1;
		goto normal;
	case N_DATACK_IND:
		request = -1;
		goto normal;
	case N_RESET_IND:
		request = -1;
		goto normal;
	case N_RESET_CON:
		request = N_RESET_REQ;
		goto normal;
	      normal:
		pfd.events |= POLLIN | POLLRDNORM;
		flag = MSG_BAND;
		band = 0;
		break;
	case N_EXDATA_IND:
		request = -1;
		goto expedited;
	      expedited:
		pfd.events |= POLLRDBAND;
		flag = MSG_BAND;
		band = 1;
		break;
	case N_INFO_ACK:
		request = N_INFO_REQ;
		goto priority;
	case N_BIND_ACK:
		request = N_BIND_REQ;
		goto priority;
	case N_ERROR_ACK:
		request = -1;
		goto priority;
	case N_OK_ACK:
		request = -1;
		goto priority;
	      priority:
		pfd.events |= POLLPRI;
		flag = MSG_HIPRI;
		band = 0;
		break;
	default:
		errno = EINVAL;
		nerrno = NSYSERR;
		return (-1);
	}

	switch (poll(&pfd, 1, 15000)) {
	default:
	case -1:
		nerrno = NSYSERR;
		return (-1);
	case 0:
		errno = ETIMEDOUT;
		nerrno = NSYSERR;
		return (-1);
	case 1:
		break;
	}

	/* FIXME */
	ret = 0;
	if (ret != 0)
		goto tryagain;

	if (npi_ctl_cnt < sizeof(p->type))
		goto tryagain;

	if (p->type != N_ERROR_ACK
	    && p->type != N_DISCON_IND && p->type != primitive)
		goto tryagain;
	switch (p->type) {
	case N_CONN_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->conn_ind)))
			goto tryagain;
		return (ret);
	case N_CONN_CON:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->conn_con)))
			goto tryagain;
		return (ret);
	case N_DISCON_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->discon_ind)))
			goto tryagain;
		return (ret);
	case N_DATA_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->data_ind)))
			goto tryagain;
		return (ret);
	case N_EXDATA_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 1))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->exdata_ind)))
			goto tryagain;
		return (ret);
	case N_INFO_ACK:
		if (unlikely(flag != MSG_HIPRI))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->info_ack)))
			goto tryagain;
		return (ret);
	case N_BIND_ACK:
		if (unlikely(flag != MSG_HIPRI))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->bind_ack)))
			goto tryagain;
		return (ret);
	case N_ERROR_ACK:
		if (unlikely(flag != MSG_HIPRI))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->error_ack)))
			goto tryagain;
		return (-1);
	case N_OK_ACK:
		if (unlikely(flag != MSG_HIPRI))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->ok_ack)))
			goto tryagain;
		return (ret);
	case N_UNITDATA_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->unitdata_ind)))
			goto tryagain;
		return (ret);
	case N_UDERROR_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->uderror_ind)))
			goto tryagain;
		return (ret);
	case N_DATACK_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->datack_ind)))
			goto tryagain;
		return (ret);
	case N_RESET_IND:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->reset_ind)))
			goto tryagain;
		return (ret);
	case N_RESET_CON:
		if (unlikely(flag != MSG_BAND))
			goto tryagain;
		if (unlikely(band != 0))
			goto tryagain;
		if (unlikely(npi_ctl_cnt < sizeof(p->reset_con)))
			goto tryagain;
		return (ret);
	}

	/* FIXME */
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

/** @brief expect a control message
  * @param fd the NPI Stream.
  * @param primitive expected primitive type.
  */
int
__npi_want_a_proto(int fd, int primitive)
{
	int err;

	if (unlikely((err = __npi_chkuser(fd)) < 0))
		return (err);
	return _npi_want_a_proto(fd, primitive);
}

/** @brief The reentrant version of __npi_want_a_proto().
  * @param fd the NPI Stream.
  * @param primitive expected primitive type.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_want_a_proto().
  */
int
__npi_want_a_proto_r(int fd, int primitive)
{
	int err;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (likely((err = __npi_getuser(fd)) >= 0)) {
		err = _npi_want_a_proto(fd, primitive);
		__npi_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (err);
}

/** @fn int npi_want_a_proto(int fd, int primitive)
  * @param fd the NPI Stream.
  * @param primitive expected primitive type.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_want_a_proto_r().
  */
__asm__(".symver __npi_want_a_proto_r,npi_want_a_proto@@NPIAPI_1.0");

/** @brief generates an ASCII description of a facility reference code.
  * @param fref facility reference code.
  * @param fval facility value.
  * @param flgth length of the facility value.
  * @param marker user-specified.
  *
  * When @marker is user-specified (the marker parameter is a positive
  * integer), the facilities specified are user-specified and cannot be
  * decoded according to the common rules.  A generic message will be
  * generated instead.
  *
  * Return Value: a pointer to the static bufffer containing the description
  * of the facility and its value.  For use with threaded programs, the static
  * buffer is contained in thread-specific data.  The non-thread-safe version
  * uses a static buffer statically allocated within the library.
  */
char *
__npi_ascii_facil(int fref, char *fval, unsigned int flgth, int marker)
{
	char *buf = __npi_get_tsd()->strbuf;

	/* FIXME: write out message */
	return (buf);
}

/** @brief The reentrant version of __npi_ascii_facil().
  * @param fref facility reference code.
  * @param fval facility value.
  * @param flgth length of the facility value.
  * @param marker user-specified.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ascii_facil().
  */
char *
__npi_ascii_facil_r(int fref, char *fval, unsigned int flgth, int marker)
{
	char *buf = __npi_get_tsd()->strbuf;

	/* FIXME: write out message */
	return (buf);
}

/** @fn char *npi_ascii_facil(int fref, char *fval, unsigned int flgth, int marker)
  * @param fref facility reference code.
  * @param fval facility value.
  * @param flgth length of the facility value.
  * @param marker user-specified.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ascii_facil_r().
  */
__asm__(".symver __npi_ascii_facil_r,npi_ascii_facil@@NPIAPI_1.0");

/** @brief bind a Stream to an NSAP.
  * @param fd Stream.
  * @param bind_nsap NSAP address.
  * @param coninds maximum number of outstanding connection indications.
  * @param flags bind flags.
  *
  * This function generates an N_BIND_REQ primitive and awaits an N_BIND_ACK
  * or N_ERROR_ACK message confirming the primitive.  The bind_nsap
  * address may contain wildcard characters `*' and `?' along with ASCII
  * digits from 0 through 9.  The `*' matches any string of digits and the
  * `?' matches any single digit.  coninds is the maximum number of
  * outstanding connection indications passed in the N_BIND_REQ primitive.
  *
  * Return Value: When succesful, returns the positive size of the N_BIND_ACK
  * primitive.  When unsuccessful, returns a negative value (-1) and writes a
  * diagnostic message to the log file.
  */
int
__npi_bind_ascii_nsap(int fd, char *bind_nsap, int coninds, unsigned flags)
{
	struct {
		union {
			np_ulong PRIM_type;
			N_bind_req_t req;
			N_bind_ack_t ack;
			N_error_ack_t err;
		} u;
		unsigned char buf[20];
	} prim;
	struct strbuf ctrl;
	int flag = 0;

	prim.u.req.PRIM_type = N_BIND_REQ;
	if (bind_nsap == NULL) {
		prim.u.req.ADDR_length = 0;
		prim.u.req.ADDR_offset = 0;
	} else {
		prim.u.req.ADDR_length = strnlen(bind_nsap, 20);
		prim.u.req.ADDR_offset = sizeof(prim.u.req);
	}
	prim.u.req.CONIND_number = coninds;
	prim.u.req.BIND_flags = flags;
	prim.u.req.PROTOID_length = 0;
	prim.u.req.PROTOID_offset = 0;
	strncpy((char *) &prim.u.req + 1, bind_nsap, 20);
	ctrl.buf = (char *) &prim;
	ctrl.len = sizeof(prim.u.req) + prim.u.req.ADDR_length;
	ctrl.maxlen = sizeof(prim);
	if (putmsg(fd, &ctrl, NULL, flag) < 0) {
		/* FIXME */
		return (-1);
	}
	ctrl.buf = (char *) &prim;
	ctrl.len = 0;
	ctrl.maxlen = sizeof(prim);
	if (getmsg(fd, &ctrl, NULL, &flag) < 0) {
		/* FIXME */
		return (-1);
	}
	switch (prim.u.PRIM_type) {
	case N_BIND_ACK:
		return (prim.u.ack.ADDR_offset + prim.u.ack.ADDR_length);
	case N_ERROR_ACK:
	default:
		break;
	}
	return (-1);
}

/** @brief The reentrant version of __npi_bind_ascii_nsap().
  * @param fd Stream.
  * @param bind_nsap NSAP address.
  * @param coninds maximum number of outstanding connection indications.
  * @param flags bind flags.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_bind_ascii_nsap().
  */
int
__npi_bind_ascii_nsap_r(int fd, char *bind_nsap, int coninds, unsigned flags)
{
	return __npi_bind_ascii_nsap(fd, bind_nsap, coninds, flags);
}

/** @fn
  * @param fd Stream.
  * @param bind_nsap NSAP address.
  * @param coninds maximum number of outstanding connection indications.
  * @param flags bind flags.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_bind_ascii_nsap_r().
  */
__asm__(".symver __npi_bind_ascii_nsap_r,npi_bind_ascii_nsap@@NPIAPI_1.0");

/** @brief bind an NSAP to a Stream.
  * @fd the Stream.
  * @bind_nsap the NSAP to bind.
  * @coninds the maximum number of outstanding connection indications.
  * @flags bind flags.
  *
  * This is similar to npi_bind_nsap
  */
int
__npi_bind_nsap(int fd, char *bind_nsap, int len, int coninds, unsigned flags)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_bind_nsap().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_bind_nsap().
  */
int
__npi_bind_nsap_r(int fd, char *bind_nsap, int len, int coninds, unsigned flags)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_bind_nsap_r().
  */
__asm__(".symver __npi_bind_nsap_r,npi_bind_nsap@@NPIAPI_1.0");

/** @brief
  */
int
__npi_conn_res(int fd, N_conn_ind_t * c, long tknval)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_conn_res().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_conn_res().
  */
int
__npi_conn_res_r(int fd, N_conn_ind_t * c, long tknval)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_conn_res_r().
  */
__asm__(".symver __npi_conn_res_r,npi_conn_res@@NPIAPI_1.0");

/** @brief
  */
int
__npi_connect(char *remote_asp, unsigned bind_flags)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_connect().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_connect().
  */
int
__npi_connect_r(char *remote_asp, unsigned bind_flags)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_connect_r().
  */
__asm__(".symver __npi_connect_r,npi_connect@@NPIAPI_1.0");

/** @brief
  */
int
__npi_connect_req(int fd, char *peer_sap, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_connect_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_connect_req().
  */
int
__npi_connect_req_r(int fd, char *peer_sap, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_connect_req_r().
  */
__asm__(".symver __npi_connect_req_r,npi_connect_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_connect_wait(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_connect_wait().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_connect_wait().
  */
int
__npi_connect_wait_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_connect_wait_r().
  */
__asm__(".symver __npi_connect_wait_r,npi_connect_wait@@NPIAPI_1.0");

/** @brief
  */
int
__npi_datack_req(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_datack_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_datack_req().
  */
int
__npi_datack_req_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_datack_req_r().
  */
__asm__(".symver __npi_datack_req_r,npi_datack_req@@NPIAPI_1.0");

/** @brief
  */
void
__npi_decode_ctl(char *p)
{
}

/** @brief The reentrant version of __npi_decode_ctl().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_decode_ctl().
  */
void
__npi_decode_ctl_r(char *p)
{
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_decode_ctl_r().
  */
__asm__(".symver __npi_decode_ctl_r,npi_decode_ctl@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_decode_primitive(long primitive)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_decode_primitive().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_decode_primitive().
  */
char *
__npi_decode_primitive_r(long primitive)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_decode_primitive_r().
  */
__asm__(".symver __npi_decode_primitive_r,npi_decode_primitive@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_decode_reason(long reason)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_decode_reason().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_decode_reason().
  */
char *
__npi_decode_reason_r(long reason)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_decode_reason_r().
  */
__asm__(".symver __npi_decode_reason_r,npi_decode_reason@@NPIAPI_1.0");

/** @brief
  */
int
__npi_discon_req(int fd, int reason, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_discon_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_discon_req().
  */
int
__npi_discon_req_r(int fd, int reason, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_discon_req_r().
  */
__asm__(".symver __npi_discon_req_r,npi_discon_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_discon_req_seq(int fd, int reason, long seq, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_discon_req_seq().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_discon_req_seq().
  */
int
__npi_discon_req_seq_r(int fd, int reason, long seq, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_discon_req_seq_r().
  */
__asm__(".symver __npi_discon_req_seq_r,npi_discon_req_seq@@NPIAPI_1.0");

/** @brief bind a binary local and remote NSAP.
  * @param fd the Stream.
  * @param bind_nsap local NSAP to bind.
  * @param bind_len length of local NSAP to bind.
  * @param rem_nsap remote NSAP to bind.
  * @param rem_len length of remote NSAP to bind.
  * @param lpa link identifier.
  * @param coninds maximum number of connection indications.
  * @param flags bind flags.
  */
int
__npi_ext_bind_nsap(int fd, char *bind_nsap, int bind_len, char *rem_nsap, int rem_len, long lpa,
		    int coninds, unsigned int flags)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_bind_nsap().
  * @param fd the Stream.
  * @param bind_nsap local NSAP to bind.
  * @param bind_len length of local NSAP to bind.
  * @param rem_nsap remote NSAP to bind.
  * @param rem_len length of remote NSAP to bind.
  * @param lpa link identifier.
  * @param coninds maximum number of connection indications.
  * @param flags bind flags.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_bind_nsap().
  */
int
__npi_ext_bind_nsap_r(int fd, char *bind_nsap, int bind_len, char *rem_nsap, int
		      rem_len, long lpa, int coninds, unsigned int flags)
{
	return (-1);		/* FIXME */
}

/** @fn int npi_ext_bind_nsap(int fd, char *bind_nsap, int bind_len, char *rem_nsap, int rem_len, long lpa, int coninds, unsigned int flags)
  * @param fd the Stream.
  * @param bind_nsap local NSAP to bind.
  * @param bind_len length of local NSAP to bind.
  * @param rem_nsap remote NSAP to bind.
  * @param rem_len length of remote NSAP to bind.
  * @param lpa link identifier.
  * @param coninds maximum number of connection indications.
  * @param flags bind flags.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_bind_nsap_r().
  */
__asm__(".symver __npi_ext_bind_nsap_r,npi_ext_bind_nsap@@NPIAPI_1.0");

/** @brief bind an ASCII local and remote NSAP.
  * @param fd the Stream.
  * @param bind_nsap the local NSAP ASCII string to bind.
  * @param rem_nsap the remote NSAP ACCII string to bind.
  * @param lpa link identifier.
  * @param coninds maximum number of connection indications.
  * @param flags bind flags.
  */
int
__npi_ext_bind_nsap_ascii(int fd, char *bind_nsap, char *rem_nsap, long lpa, int
			  coninds, unsigned int flags)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_bind_nsap_ascii().
  * @param fd the Stream.
  * @param bind_nsap the local NSAP ASCII string to bind.
  * @param rem_nsap the remote NSAP ACCII string to bind.
  * @param lpa link identifier.
  * @param coninds maximum number of connection indications.
  * @param flags bind flags.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_bind_nsap_ascii().
  */
int
__npi_ext_bind_nsap_ascii_r(int fd, char *bind_nsap, char *rem_nsap, long lpa, int coninds,
			    unsigned int flags)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_bind_nsap_ascii_r().
  */
__asm__(".symver __npi_ext_bind_nsap_ascii_r,npi_ext_bind_nsap_ascii@@NPIAPI_1.0");

/** @brief
  */
int
__npi_ext_connect_req(int fd, char *peer_sap, char *buf, int nbytes, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_connect_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_connect_req().
  */
int
__npi_ext_connect_req_r(int fd, char *peer_sap, char *buf, int nbytes, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_connect_req_r().
  */
__asm__(".symver __npi_ext_connect_req_r,npi_ext_connect_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_ext_connect_wait(int listen_fid, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_connect_wait().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_connect_wait().
  */
int
__npi_ext_connect_wait_r(int listen_fid, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_connect_wait_r().
  */
__asm__(".symver __npi_ext_connect_wait_r,npi_ext_connect_wait@@NPIAPI_1.0");

/** @brief
  */
int
__npi_ext_conn_res(int fd, N_conn_ind_t * c, long tknval, char *fac_ptr, int
		   fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_conn_res().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_conn_res().
  */
int
__npi_ext_conn_res_r(int fd, N_conn_ind_t * c, long tknval, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_conn_res_r().
  */
__asm__(".symver __npi_ext_conn_res_r,npi_ext_conn_res@@NPIAPI_1.0");

/** @brief
  */
int
__npi_ext_listen(char *bind_nsap, unsigned int fork_optns, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_listen().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_listen().
  */
int
__npi_ext_listen_r(char *bind_nsap, unsigned int fork_optns, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_listen_r().
  */
__asm__(".symver __npi_ext_listen_r,npi_ext_listen@@NPIAPI_1.0");

/** @brief
  */
int
__npi_ext_nbio_complete_listen(int listen_fid, int options, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_ext_nbio_complete_listen().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_ext_nbio_complete_listen().
  */
int
__npi_ext_nbio_complete_listen_r(int listen_fid, int options, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_ext_nbio_complete_listen_r().
  */
__asm__(".symver __npi_ext_nbio_complete_listen_r,npi_ext_nbio_complete_listen@@NPIAPI_1.0");

/** @brief
  */
int
__npi_fac_walk(char *facp, unsigned int facl, facil_proc_t * fcn)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_fac_walk().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_fac_walk().
  */
int
__npi_fac_walk_r(char *facp, unsigned int facl, facil_proc_t * fcn)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_fac_walk_r().
  */
__asm__(".symver __npi_fac_walk_r,npi_fac_walk@@NPIAPI_1.0");

/** @brief
  */
int
__npi_flags_connect_wait(int listen_fid, char *fac_ptr, int fac_len, int bind_flags)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_flags_connect_wait().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_flags_connect_wait().
  */
int
__npi_flags_connect_wait_r(int listen_fid, char *fac_ptr, int fac_len, int bind_flags)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_flags_connect_wait_r().
  */
__asm__(".symver __npi_flags_connect_wait_r,npi_flags_connect_wait@@NPIAPI_1.0");

/** @brief
  */
int
__npi_flags_listen(char *bind_nsap, unsigned int fork_optns, char *fac_ptr, int fac_len,
		   int bind_flags)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_flags_listen().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_flags_listen().
  */
int
__npi_flags_listen_r(char *bind_nsap, unsigned int fork_optns, char *fac_ptr, int fac_len,
		     int bind_flags)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_flags_listen_r().
  */
__asm__(".symver __npi_flags_listen_r,npi_flags_listen@@NPIAPI_1.0");

/** @brief
  */
int
__npi_flow_req(int fd, unsigned long flow_incr)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_flow_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_flow_req().
  */
int
__npi_flow_req_r(int fd, unsigned long flow_incr)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_flow_req_r().
  */
__asm__(".symver __npi_flow_req_r,npi_flow_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_get_and_log_facils(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_get_and_log_facils().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_get_and_log_facils().
  */
int
__npi_get_and_log_facils_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_get_and_log_facils_r().
  */
__asm__(".symver __npi_get_and_log_facils_r,npi_get_and_log_facils@@NPIAPI_1.0");

/** @brief
  */
int
__npi_get_facils(int fd, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_get_facils().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_get_facils().
  */
int
__npi_get_facils_r(int fd, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_get_facils_r().
  */
__asm__(".symver __npi_get_facils_r,npi_get_facils@@NPIAPI_1.0");

/** @brief
  */
int
__npi_info_req(int strm)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_info_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_info_req().
  */
int
__npi_info_req_r(int strm)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_info_req_r().
  */
__asm__(".symver __npi_info_req_r,npi_info_req@@NPIAPI_1.0");

/** @brief initialize the NPI library.
  * @param log_options log options.
  * @param log_name name of the log file.
  *
  * Performs initialization functions for the NPI API library.  It should be
  * called prior to using any other library subroutine.  The log_options
  * parameter is used to specify which messages are written to the log file.
  * This parameter consists of a number of single-bit values that are bitwise
  * OR'ed together to from the parameter value.  Logging opions are:
  *
  * NPI_LOG_FILE	write messages (also) to the log file.
  * NPI_LOG_STDERR	write messages (also) to stderr.
  * NPI_LOG_RX_PROTOS	logs all received protocol messages in ASCII decoded
  *			form.
  * NPI_LOG_RX_PROTOS	logs all send protocol messages in ASCII decoded form. 
  * NPI_LOG_ERRORS	Log all UNIX error messages in a manner similar to
  *			perror(3).
  * NPI_LOG_RX_DATA	log all received M_DATA messages (npi_read_data); dump
  *			the data contents in hex into the log file.
  * NPI_LOG_TX_DATA	log all sent M_DATA messages (npi_write_data); dump
  *			the data contents in hex into the log file.
  * NPI_LOG_SIGNALS	log signal handling.
  * NPI_LOG_CONINDS	log NPI connection indications.
  * NPI_LOG_OPTIONS	log initialization options.
  * NPI_LOG_FACILS	log X.25 facilities pertaining to the connection at
  *			disconnect time; npi_get_and_log_facils() internally
  *			obtains the facilities and prints them to the log.
  * NPI_LOG_DEFAULT	default set of logging options consisting of
  *			NPI_LOG_FILE, NPI_LOG_STDERR and NPI_LOG_ERRORS.
  *
  * The log_name parameter points to a file name used as the loggin file when
  * NPI_LOG_FILE is specified in log_options.  This file name pointer may also
  * be NULL indicates the default log file.
  *
  * Note that the npi_init() function need only be called by one POSIX thread.
  * The log options are global to the process using the library.
  *
  * Returns a positive integer on success and a negative one on error.
  */
int
__npi_init(unsigned int log_options, char *log_name)
{
	__npi_log_options = log_options;

	/* FIXME: open a file using the syslog facility and determine whether
	   to also use it to log to stderr. */
	return (1);
}

/** @brief The reentrant version of __npi_init().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_init().
  */
int
__npi_init_r(unsigned int log_options, char *log_name)
{
	return __npi_init(log_options, log_name);
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_init_r().
  */
__asm__(".symver __npi_init_r,npi_init@@NPIAPI_1.0");

/** @brief intialize NPI API library.
  * @param log_options logging option flags.
  * @param log_FILE file stream for logging.
  *
  * This is the same as npi_init_file() with the exception that the file is
  * already open and passed as the log_FILE argument.
  */
int
__npi_init_FILE(unsigned int log_options, FILE * log_FILE)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_init_FILE().
  * @param log_options logging option flags.
  * @param log_FILE file stream for logging.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_init_FILE().
  */
int
__npi_init_FILE_r(unsigned int log_options, FILE * log_FILE)
{
	return __npi_init_FILE(log_options, log_FILE);
}

/** @fn int npi_init_FILE(unsigned int log_options, FILE *log_FILE)
  * @param log_options logging option flags.
  * @param log_FILE file stream for logging.
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_init_FILE_r().
  */
__asm__(".symver __npi_init_FILE_r,npi_init_FILE@@NPIAPI_1.0");

/** @brief
  */
int
__npi_listen(char *bind_nsap, unsigned int fork_optns)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_listen().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_listen().
  */
int
__npi_listen_r(char *bind_nsap, unsigned int fork_optns)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_listen_r().
  */
__asm__(".symver __npi_listen_r,npi_listen@@NPIAPI_1.0");

/** @brief
  */
int
__npi_max_sdu(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_max_sdu().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_max_sdu().
  */
int
__npi_max_sdu_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_max_sdu_r().
  */
__asm__(".symver __npi_max_sdu_r,npi_max_sdu@@NPIAPI_1.0");

/** @brief
  */
int
__npi_nbio_complete_listen(int listen_fd, int options)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_nbio_complete_listen().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_nbio_complete_listen().
  */
int
__npi_nbio_complete_listen_r(int listen_fd, int options)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_nbio_complete_listen_r().
  */
__asm__(".symver __npi_nbio_complete_listen_r,npi_nbio_complete_listen@@NPIAPI_1.0");

/** @brief
  */
int
__npi_open_data(void)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_open_data().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_open_data().
  */
int
__npi_open_data_r(void)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_open_data_r().
  */
__asm__(".symver __npi_open_data_r,npi_open_data@@NPIAPI_1.0");

/** @brief
  */
void
__npi_perror(char *msg)
{
}

/** @brief The reentrant version of __npi_perror().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_perror().
  */
void
__npi_perror_r(char *msg)
{
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_perror_r().
  */
__asm__(".symver __npi_perror_r,npi_perror@@NPIAPI_1.0");

/** @brief
  */
void
__npi_print_msg(unsigned char *p, unsigned n, int indent)
{
}

/** @brief The reentrant version of __npi_print_msg().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_print_msg().
  */
void
__npi_print_msg_r(unsigned char *p, unsigned n, int indent)
{
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_print_msg_r().
  */
__asm__(".symver __npi_print_msg_r,npi_print_msg@@NPIAPI_1.0");

/** @brief
  */
void
__npi_printf(char *fmt, ...)
{
}

/** @brief The reentrant version of __npi_printf().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_printf().
  */
void
__npi_printf_r(char *fmt, ...)
{
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_printf_r().
  */
__asm__(".symver __npi_printf_r,npi_printf@@NPIAPI_1.0");

/** @brief
  */
int
__npi_rcv(int fd, char *buf, int cnt, long flags_in, long *flags_out)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_rcv().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_rcv().
  */
int
__npi_rcv_r(int fd, char *buf, int cnt, long flags_in, long *flags_out)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_rcv_r().
  */
__asm__(".symver __npi_rcv_r,npi_rcv@@NPIAPI_1.0");

/** @brief
  */
int
__npi_read_data(int fd, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_read_data().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_read_data().
  */
int
__npi_read_data_r(int fd, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_read_data_r().
  */
__asm__(".symver __npi_read_data_r,npi_read_data@@NPIAPI_1.0");

/** @brief
  */
int
__npi_reset_req(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_reset_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_reset_req().
  */
int
__npi_reset_req_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_reset_req_r().
  */
__asm__(".symver __npi_reset_req_r,npi_reset_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_reset_res(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_reset_res().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_reset_res().
  */
int
__npi_reset_res_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_reset_res_r().
  */
__asm__(".symver __npi_reset_res_r,npi_reset_res@@NPIAPI_1.0");

/** @brief
  */
int
__npi_send_connect_req(int fd, char *peer_sap, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_send_connect_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_send_connect_req().
  */
int
__npi_send_connect_req_r(int fd, char *peer_sap, char *buf, int cnt)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_send_connect_req_r().
  */
__asm__(".symver __npi_send_connect_req_r,npi_send_connect_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_send_ext_connect_req(int fd, char *peer_sap, char *buf, int cnt, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_send_ext_connect_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_send_ext_connect_req().
  */
int
__npi_send_ext_connect_req_r(int fd, char *peer_sap, char *buf, int cnt, char *fac_ptr, int fac_len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_send_ext_connect_req_r().
  */
__asm__(".symver __npi_send_ext_connect_req_r,npi_send_ext_connect_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_send_info_req(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_send_info_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_send_info_req().
  */
int
__npi_send_info_req_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_send_info_req_r().
  */
__asm__(".symver __npi_send_info_req_r,npi_send_info_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_send_reset_req(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_send_reset_req().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_send_reset_req().
  */
int
__npi_send_reset_req_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_send_reset_req_r().
  */
__asm__(".symver __npi_send_reset_req_r,npi_send_reset_req@@NPIAPI_1.0");

/** @brief
  */
int
__npi_send_reset_res(int fd)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_send_reset_res().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_send_reset_res().
  */
int
__npi_send_reset_res_r(int fd)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_send_reset_res_r().
  */
__asm__(".symver __npi_send_reset_res_r,npi_send_reset_res@@NPIAPI_1.0");

/** @brief
  */
int
__npi_set_log_size(long nbytes)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_set_log_size().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_set_log_size().
  */
int
__npi_set_log_size_r(long nbytes)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_set_log_size_r().
  */
__asm__(".symver __npi_set_log_size_r,npi_set_log_size@@NPIAPI_1.0");

/** @brief
  */
int
__npi_set_marks(int fid, unsigned int rd_lo_make, unsigned int rd_hi_mark, unsigned int wr_lo_mark,
		unsigned int wr_hi_mark)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_set_marks().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_set_marks().
  */
int
__npi_set_marks_r(int fid, unsigned int rd_lo_make, unsigned int rd_hi_mark,
		  unsigned int wr_lo_mark, unsigned int wr_hi_mark)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_set_marks_r().
  */
__asm__(".symver __npi_set_marks_r,npi_set_marks@@NPIAPI_1.0");

/** @brief
  */
int
__npi_set_pid(int fid)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_set_pid().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_set_pid().
  */
int
__npi_set_pid_r(int fid)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_set_pid_r().
  */
__asm__(".symver __npi_set_pid_r,npi_set_pid@@NPIAPI_1.0");

/** @brief
  */
int
__npi_set_signal_handling(int fid, npi_sig_func_t func, int sig_num, int primitive_mask)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_set_signal_handling().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_set_signal_handling().
  */
int
__npi_set_signal_handling_r(int fid, npi_sig_func_t func, int sig_num, int primitive_mask)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_set_signal_handling_r().
  */
__asm__(".symver __npi_set_signal_handling_r,npi_set_signal_handling@@NPIAPI_1.0");

/** @brief write data to a Stream.
  * @param fd the Stream.
  * @param buf the data buffer.
  * @param nbytes the length of the data buffer.
  *
  * Writes the data to the Stream.  If the data message exceeds the size of a
  * packet, the NPI driver will automatically segment the message into
  * packets, using X.25's M-bit mechanism to indicate that they should be
  * reassembled at the other end.
  */
int
__npi_write_data(int fd, char *buf, int nbytes)
{
	struct __npi_user *nu;
	int len, written = 0;

	if (__npi_getuser(fd) != 0)
		goto nbadf;
	nu = __npi_fds[fd];
	len = nbytes;
	if ((np_long) nu->nu_info.NSDU_size > 0)
		if (len > (np_long) nu->nu_info.NSDU_size)
			len = nu->nu_info.NSDU_size;
	if (len < nbytes) {
		if ((np_long) nu->nu_info.NIDU_size > 0)
			if (len > nu->nu_info.NIDU_size)
				len = nu->nu_info.NIDU_size;
		if ((np_long) nu->nu_info.NODU_size > 0)
			if (len > nu->nu_info.NODU_size)
				len = nu->nu_info.NODU_size;
	}
	/* Ok, now we have a length that we can use. */
	{
		N_data_req_t prim;
		struct strbuf ctrl, data;

		while (nbytes > written) {
			prim.PRIM_type = N_DATA_REQ;
			if (nbytes > written + len)
				prim.DATA_xfer_flags = N_MORE_DATA_FLAG;
			else
				prim.DATA_xfer_flags = 0;

			ctrl.buf = (char *) &prim;
			ctrl.len = (int) sizeof(prim);
			ctrl.maxlen = -1;

			data.buf = buf + written;
			data.len = len;
			data.maxlen = -1;

			if (putmsg(fd, &ctrl, &data, 0) < 0)
				goto badput;
			written += len;
		}
	}
      badput:
	nerrno = NSYSERR;
	goto error;
      nbadf:
	nerrno = NSYSERR;
	errno = EBADF;
	goto error;
      error:
	if (written > 0)
		return (written);
	return (-1);
}

/** @brief The reentrant version of __npi_write_data().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_write_data().
  */
int
__npi_write_data_r(int fd, char *buf, int nbytes)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__npi_putuser, &fd);
	if (__npi_getuser(fd)) {
		if ((ret = __npi_write_data(fd, buf, nbytes)) == -1)
			pthread_testcancel();
		__npi_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_write_data_r().
  */
__asm__(".symver __npi_write_data_r,npi_write_data@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_x25_clear_cause(int cause)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_x25_clear_cause().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_x25_clear_cause().
  */
char *
__npi_x25_clear_cause_r(int cause)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_x25_clear_cause_r().
  */
__asm__(".symver __npi_x25_clear_cause_r,npi_x25_clear_cause@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_x25_diagnostic(int diagnostic)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_x25_diagnostic().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_x25_diagnostic().
  */
char *
__npi_x25_diagnostic_r(int diagnostic)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_x25_diagnostic_r().
  */
__asm__(".symver __npi_x25_diagnostic_r,npi_x25_diagnostic@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_x25_registration_cause(int cause)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_x25_registration_cause().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_x25_registration_cause().
  */
char *
__npi_x25_registration_cause_r(int cause)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_x25_registration_cause_r().
  */
__asm__(".symver __npi_x25_registration_cause_r,npi_x25_registration_cause@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_x25_reset_cause(int cause)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_x25_reset_cause().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_x25_reset_cause().
  */
char *
__npi_x25_reset_cause_r(int cause)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_x25_reset_cause_r().
  */
__asm__(".symver __npi_x25_reset_cause_r,npi_x25_reset_cause@@NPIAPI_1.0");

/** @brief
  */
char *
__npi_x25_restart_cause(int cause)
{
	return (NULL);		/* FIXME */
}

/** @brief The reentrant version of __npi_x25_restart_cause().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of npi_x25_restart_cause().
  */
char *
__npi_x25_restart_cause_r(int cause)
{
	return (NULL);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_x25_restart_cause_r().
  */
__asm__(".symver __npi_x25_restart_cause_r,npi_x25_restart_cause@@NPIAPI_1.0");

/** @brief
  */
int
__npi_put_npi_proto(int fid, int len)
{
	return (-1);		/* FIXME */
}

/** @brief The reentrant version of __npi_put_npi_proto().
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of put_npi_proto().
  */
int
__npi_put_npi_proto_r(int fid, int len)
{
	return (-1);		/* FIXME */
}

/** @fn
  * @version NPIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __npi_put_npi_proto_r().
  */
__asm__(".symver __npi_put_npi_proto_r,put_npi_proto@@NPIAPI_1.0");

static char *
_pu_decode_handle(unsigned long pu_handle)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (NULL);
}

char *
__pu_decode_handle(unsigned long pu_handle)
{
	return _pu_decode_handle(pu_handle);
}
char *
__pu_decode_handle_r(unsigned long pu_handle)
{
	return _pu_decode_handle(pu_handle);
}

__asm__(".symver __pu_decode_handle_r,pu_decode_handle@@NPIAPI_1.0");

static int
_pu_dlpi_upa(int board, int port, int station, int *upa_ptr)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_dlpi_upa(int board, int port, int station, int *upa_ptr)
{
	return _pu_dlpi_upa(board, port, station, upa_ptr);
}
int
__pu_dlpi_upa_r(int board, int port, int station, int *upa_ptr)
{
	return _pu_dlpi_upa(board, port, station, upa_ptr);
}

__asm__(".symver __pu_dlpi_upa_r,pu_dlpi_upa@@NPIAPI_1.0");

static int
_pu_get_pu_id(int board, int port, int station, int *pu_id_ptr, unsigned long *pu_hndl_ptr)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_get_pu_id(int board, int port, int station, int *pu_id_ptr, unsigned long *pu_hndl_ptr)
{
	return _pu_get_pu_id(board, port, station, pu_id_ptr, pu_hndl_ptr);
}
int
__pu_get_pu_id_r(int board, int port, int station, int *pu_id_ptr, unsigned long *pu_hndl_ptr)
{
	return _pu_get_pu_id(board, port, station, pu_id_ptr, pu_hndl_ptr);
}

__asm__(".symver __pu_get_pu_id_r,pu_get_pu_id@@NPIAPI_1.0");

static int
_pu_get_stats(unsigned long pu_handle, int pu_id, int *pu_up_down_ptr, int *link_state_ptr,
	      int *modem_state_ptr)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_get_stats(unsigned long pu_handle, int pu_id, int *pu_up_down_ptr, int *link_state_ptr,
	       int *modem_state_ptr)
{
	return _pu_get_stats(pu_handle, pu_id, pu_up_down_ptr, link_state_ptr, modem_state_ptr);
}
int
__pu_get_stats_r(unsigned long pu_handle, int pu_id, int *pu_up_down_ptr, int *link_state_ptr,
		 int *modem_state_ptr)
{
	return _pu_get_stats(pu_handle, pu_id, pu_up_down_ptr, link_state_ptr, modem_state_ptr);
}

__asm__(".symver __pu_get_stats_r,pu_get_stats@@NPIAPI_1.0");

static int
_pu_get_board_info(unsigned long pu_handle, int pu_id, int *board_ptr, int *port_ptr)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_get_board_info(unsigned long pu_handle, int pu_id, int *board_ptr, int *port_ptr)
{
	return _pu_get_board_info(pu_handle, pu_id, board_ptr, port_ptr);
}
int
__pu_get_board_info_r(unsigned long pu_handle, int pu_id, int *board_ptr, int *port_ptr)
{
	return _pu_get_board_info(pu_handle, pu_id, board_ptr, port_ptr);
}

__asm__(".symver __pu_get_board_info_r,pu_get_board_info@@NPIAPI_1.0");

static int
_pu_id_to_pu_handle(int pu_id, unsigned long *pu_handle_ptr)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_id_to_pu_handle(int pu_id, unsigned long *pu_handle_ptr)
{
	return _pu_id_to_pu_handle(pu_id, pu_handle_ptr);
}
int
__pu_id_to_pu_handle_r(int pu_id, unsigned long *pu_handle_ptr)
{
	return _pu_id_to_pu_handle(pu_id, pu_handle_ptr);
}

__asm__(".symver __pu_id_to_pu_handle_r,pu_id_to_pu_handle@@NPIAPI_1.0");

static int
_pu_id_to_pu_number(unsigned long pu_id)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_id_to_pu_number(unsigned long pu_id)
{
	return _pu_id_to_pu_number(pu_id);
}
int
__pu_id_to_pu_number_r(unsigned long pu_id)
{
	return _pu_id_to_pu_number(pu_id);
}

__asm__(".symver __pu_id_to_pu_number_r,pu_id_to_pu_number@@NPIAPI_1.0");

static int
_pu_map_npi_lpa_to_handle(int npi_lpa, int *pu_id_ptr, unsigned long handle_ptr)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_map_npi_lpa_to_handle(int npi_lpa, int *pu_id_ptr, unsigned long handle_ptr)
{
	return _pu_map_npi_lpa_to_handle(npi_lpa, pu_id_ptr, handle_ptr);
}
int
__pu_map_npi_lpa_to_handle_r(int npi_lpa, int *pu_id_ptr, unsigned long handle_ptr)
{
	return _pu_map_npi_lpa_to_handle(npi_lpa, pu_id_ptr, handle_ptr);
}

__asm__(".symver __pu_map_npi_lpa_to_handle_r,pu_map_npi_lpa_to_handle@@NPIAPI_1.0");

static int
_pu_strerror(int pu_errnum)
{
	errno = ENOTSUP;
	nerrno = NSYSERR;
	return (-1);
}

int
__pu_strerror(int pu_errnum)
{
	return _pu_strerror(pu_errnum);
}
int
__pu_strerror_r(int pu_errnum)
{
	return _pu_strerror(pu_errnum);
}

__asm__(".symver __pu_strerror_r,pu_strerror@@NPIAPI_1.0");

/** @} */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
