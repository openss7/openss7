/*****************************************************************************

 @(#) $RCSfile: gen.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/19 11:15:08 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2005/07/19 11:15:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: gen.c,v $
 Revision 0.9.2.1  2005/07/19 11:15:08  brian
 - added syslogd and friends

 *****************************************************************************/

#ident "@(#) $RCSfile: gen.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/19 11:15:08 $"

static char const ident[] = "$RCSfile: gen.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/19 11:15:08 $";

/*
 *  Linux Fast-STREAMS libc replacement functions for logging
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <time.h>
#include <sys/time.h>
#include <stropts.h>
#include <pthread.h>
#include <sys/strlog.h>
#define SYSLOG_NAMES
#include <syslog.h>
#include <sys/sad.h>

/*
 * Placeholder functions for -libpthread.  These functions are defined as weak external functions,
 * meaning that if libpthread is loaded on top of this library, the definitions there will override
 * these placeholder defaults.  This is the same approach taken by thread-safe routines in libc.
 *
 * After defining these this way, we can just use them and they will operate properly when required
 * in a multi-threaded environment (with libpthread loaded) but will perform non-threaded
 * equivalents when it is not.
 */

extern void __pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
				   void *arg);
extern void __pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute);
extern void __pthread_cleanup_pop_restore(struct _pthread_cleanup_buffer *buffer, int execute);
extern void __pthread_testcancel(void);
extern void __pthread_setcanceltype(int type, int *oldtype);

extern int __pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_wrlockattr_t * attr);
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

/* *INDENT-OFF* */
static
    struct __slog_info_struct {
	int slog_fd;			/* the file descriptor for /dev/slog */
	int slog_options;		/* the options passed to openlog */
	int slog_flags;			/* the state of the logger */
	int slog_mask;			/* the log mask */
} __slog_info = {
	.slog_fd = 0,
	.slog_options = LOG_ODELAY,
	.slog_flags = 0,
	.slog_mask = LOG_UPTO(LOG_EMERG),
};
/* *INDENT-ON* */

static int __slog_fd = 0;

static pthread_rwlock_t __slog_info_lock = PTHREAD_RWLOCK_INITIALIZER;

static struct __slog_info_struct *
__slog_rdinfo(void)
{
	pthread_rwlock_rdlock(&__slog_info_lock);
	return (&__slog_info);
}
static struct __slog_info_struct *
__slog_wrinfo(void)
{
	int err;

	if (((err = pthread_rwlock_wrlock(&__slog_info_lock))) == 0)
		return (&__slog_info);
	errno = err;
	perror(__FUNCTION__);
	return (NULL);
}
static void
__slog_putinfo(void *arg)
{
	struct _slog_info_struct **slogp = (typeof(slogp)) arg;

	if (*slogp != NULL)
		pthread_rwlock_unlock(&__slog_info_lock);
	return;
}

/*
 * This function is a cancellation point.
 */
static int
__slog_openlog(struct __slog_info_struct *info, const char *ident, int option, int facility)
{
	info->slog_options = option;
	if (option & LOG_PID) {
		/* log the pid with each message */
	}
	if (option & LOG_CONS) {
		/* log on the console if errors in sending */
	}
	if (option & LOG_ODELAY) {
		/* delay open util first syslog() (default) */
	}
	if (option & LOG_NDELAY) {
		/* don't delay open */
	}
	if (option & LOG_NOWAIT) {
		/* don't wait for console forks: DEPRECATED */
	}
	if (option & LOG_PERROR) {
		/* log to stderr as well */
	}
	if (option & LOG_NDELAY) {
		int flags = (options & LOG_NOWAIT) ? (O_NONBLOCK | O_NELAY) : 0;

		if ((info->slog_fd = open("/dev/strlog", flags)) == -1) {
			info->slog_fd = 0;
			perror(__FUNCTION__);
			return (-1);
		}
	}
	return (0);
}

void
__slog_openlog_r(const char *ident, int option, int facility)
{
	struct __slog_info_struct *info = NULL;

	pthread_cleanup_push_defer_np(__slog_putinfo, &info);
	if ((info = __slog_wrinfo()) != NULL) {
		if (__slog_openlog(info, ident, option, facility) == -1)
			pthread_testcancel();
		__slog_putinfo(&info);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
}
void openlog(const char *, int, int) __attribute__ ((alias("__slog_openlog_r")));

/*
 * This function is a cancellation point.
 */
static int
__slog_closelog(struct __slog_info_struct *info)
{
	int err = 0;

	if (info->slog_fd != 0) {
		if ((err = close(info->slog_fd)) != 0) {
			perror(__FUNCTION__);
			info->slog_fd = 0;
		}
	}
	return (err);
}

void
__slog_closelog_r(void)
{
	struct __slog_info_struct *info = NULL;

	pthread_cleanup_push_defer_np(__slog_putinfo, &info);
	if ((info = __slog_wrinfo()) != NULL) {
		if (__slog_closelog(info) != 0)
			pthread_testcancel();
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(1);
}
void closelog(void) __attribute__ ((alias("__slog_closelog_r")));

/*
 * This function is not a thread cancellation point.
 */
int
__slog_setlogmask(struct __slog_info_struct *info, int mask)
{
	int oldmask = info->slog_mask;
	if (mask)
		info->slog_mask = mask;
	return (oldmask);
}
int
__slog_setlogmask(int mask)
{
	struct __slog_info_struct *info = NULL;
	int ret = -1;
	pthread_cleanup_push_defer_np(__slog_putinfo, &info);
	if ((info = __slog_wrinfo()) != NULL)
		ret = __slog_setlogmask(info, mask);
	pthread_cleanup_pop_restore_np(1);
	return (ret);
}
int setlogmask(int mask) __attribute__ ((alias("__slog_setlogmask_r")));

/*
 *  This function is a thread cancellation point.
 */
int
__slog_vsyslog(struct __slog_info_struct *info, int pri, const char *fmt, va_list args)
{
	int ret = -1;
	if (info->slog_fd == 0) {
		if ((ret = open("/dev/strlog", O_NONBLOCK | O_NDELAY)) == -1) {
			perror(__FUNCTION__);
			return (ret);
		}
		info->slog_fd = ret;
	}
	/* FIXME: format the message and send it to the log device */
	return (0);
}
void __slog_vsyslog_r(int pri, const char *fmt, va_list args)
{
}
void vsyslog(int pri, const char *fmt, va_list args) __attribute___((alias("__slog_vsyslog_r")));

/*
 *  This function is a thread cancellation point.
 */
void
__slog_syslog(int pri, const char *fmt, ...)
{
	va_list ap;

	va_start(fmt, ap);
	vsyslog(pri, fmt, ap);
	va_end(ap);
}
void syslog(int pri, const char *fmt, ...) __attribute__ ((alias("__slog_syslog")));
