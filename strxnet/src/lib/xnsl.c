/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

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

#include <dlfcn.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

//#include <sys/sockmod.h>
//#include <sys/socksys.h>
//#include <sys/socket.h>
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

#include <xti.h>
#include <netconfig.h>
#include <netdir.h>

/**
 * @defgroup libxnsl OpenSS7 Network Sevices Library (NSL)
 * @brief OpenSS7 NSL Library Calls
 *
 * This manual contains documentation of the OpenSS7 Network Services Library
 * functions that are generated automatically from the source code with doxygen.
 * This documentation is not intended for users of the OpenSS7 Network Services
 * Library.  Users should consult the documentation found in the user manual
 * pages beginning with xnsl(3).
 *
 * <h2>Thread Safety</h2>
 * The OpenSS7 NSL library is designed to be thread-safe.  This is accomplished
 * in a number of ways.  Thread-safety depends on the use of glibc2 and the
 * pthreads library.
 *
 * Glibc2 provides lightweight thread-specific data for errno and h_errno.
 * Because errno and h_errno do not use communications functions orthogonal to
 * the NSL library services, we cannot borrow h_errno or errno for _nderror or
 * nc_error.  Therefore, both _nderror and nc_error are contained in
 * thread-specific data.
 *
 * Glibc2 also provides some weak undefined aliases for POSIX thread functions
 * to perform its own thread-safety.  When the pthread library (libpthread) is
 * linked with glibc2, these functions call libpthread functions instead of
 * internal dummy routines.  The same approach is taken for the OpenSS7 NSL
 * library.  The library uses weak defined and undefined aliases that
 * automatically invoke libpthread functions when libpthread is (dynamically)
 * linked and uses dummy functions when it is not.  Thsi maintains maximum
 * efficiency when libpthread is not dynamically linked, but providers full
 * thread safety when it is.
 *
 * Libpthread behaves in some strange ways with regards to thread cancellation.
 * Because libpthread uses Linux clone processes for threads, cancellation of a
 * thread is accomplished by sending a signal to the thread process.  This does
 * not directly result in cancellation, but will result in the failure of a
 * system call with the EINTR error code.  It is necessary to test for
 * cancellation upon error retrun from system calls to perform the actual
 * cancellation of a thread.
 *
 * The XNS specification (OpenGroup XNS 5.2) lists no functions in this group as
 * containing thread cancellation points.  All NSL functions must not contain a
 * thread cancellation point.
 *
 * Locks and asynchronous thread cancellation present challenges:
 *
 * Functions that act as thread cancellation points must push routines onto
 * the function stack executed at exit of the thread to release the locks held
 * by the function.  These are performed with weak definitions of POSIX thread
 * library functions.
 *
 * Functions that do not act as thread cancellation points must defer thread
 * cancellation before taking locks and then release locks before thread
 * canceallation is restored.
 *
 * The above are the techniques used by glibc2 for the same purpose and is the
 * same technique that is used by the OpenSS7 NSL library.
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

extern int __pthread_once(pthread_once_t * once_control, void (*init_routine) (void));
extern int __pthread_key_create(pthread_key_t * key, void (*destr_function) (void *));
extern int __pthread_setspecific(pthread_key_t key, const void *pointer);
extern void *__pthread_getspecific(pthread_key_t key);
extern int __pthread_key_delete(pthread_key_t key);

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

#pragma weak __pthread_once
#pragma weak __pthread_key_create
#pragma weak __pthread_getspecific
#pragma weak __pthread_setspecific
#pragma weak __pthread_key_delete

#pragma weak pthread_once
#pragma weak pthread_key_create
#pragma weak pthread_getspecific
#pragma weak pthread_setspecific
#pragma weak pthread_key_delete

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

int
pthread_once(pthread_once_t * once_control, void (*init_routine) (void))
{
	if (__pthread_once)
		return __pthread_once(once_control, init_routine);
	/* non-thread safe replacement */
	if (*once_control == 0)
		init_routine();
	*once_control = 1;
	return (0);
}

static pthread_key_t ___pthread_key_num = (pthread_key_t) (0);
static void *___pthread_keys[PTHREAD_KEYS_MAX] = { NULL, };

int
pthread_key_create(pthread_key_t * key, void (*destr_function) (void *))
{
	if (__pthread_key_create)
		return __pthread_key_create(key, destr_function);
	/* non-thread safe replacement */
	*key = ___pthread_key_num++;
	return (0);
}

int
pthread_setspecific(pthread_key_t key, const void *pointer)
{
	if (__pthread_setspecific)
		return __pthread_setspecific(key, pointer);
	/* non-thread safe replacement */
	___pthread_keys[key] = (void *) pointer;
	return (0);
}

void *
pthread_getspecific(pthread_key_t key)
{
	if (__pthread_getspecific)
		return __pthread_getspecific(key);
	/* non-thread safe replacement */
	return ___pthread_keys[key];
}

int
pthread_key_delete(pthread_key_t key)
{
	if (__pthread_key_delete)
		return __pthread_key_delete(key);
	/* non-thread safe replacement */
	if (key < ___pthread_key_num)
		___pthread_key_num = key;
	return (0);
}

#ifndef NDERR_BUFSZ
#define NDERR_BUFSZ 256
#endif

#ifndef NCERR_BUFSZ
#define NCERR_BUFSZ 256
#endif

/*
 * Thread-specific data structure for the xnsl library.
 */
struct __nsl_tsd {
	int nderror;			/* error for network directory functions */
	int ncerror;			/* error for network configuration functions */
	char nderrbuf[NDERR_BUFSZ];	/* error buffer for network directory functions */
	char ncerrbuf[NCERR_BUFSZ];	/* errof buffer for network configuration functions */
	int fieldnum;			/* field number of error */
	int linenum;			/* line number of error */
};

/*
 *  Once condition for Thread-Specific Data key creation.
 */
static pthread_once_t __nsl_tsd_once = PTHREAD_ONCE_INIT;

/*
 *  XNSL library Thread-Specific Data key.
 */
static pthread_key_t __nsl_tsd_key = 0;

static void
__nsl_tsd_free(void *buf)
{
	pthread_setspecific(__nsl_tsd_key, NULL);
	free(buf);
}

static void
__nsl_tsd_alloc(void)
{
	int ret;
	void *buf;

	ret = pthread_key_create(&__nsl_tsd_key, __nsl_tsd_free);
	buf = malloc(sizeof(struct __nsl_tsd));
	bzero(buf, sizeof(*buf));
	ret = pthread_setspecific(__nsl_tsd_key, buf);
	return;
}

/**
 * @internal
 * @fn struct __nsl_tsd *__nsl_get_tsd(void);
 * @brief Get thread specific data for the xnsl library.
 *
 * This function obtains (and allocates if necessary), thread specific data for
 * the executing thread.
 */
static struct __nsl_tsd *
__nsl_get_tsd(void)
{
	pthread_once(&__nsl_tsd_once, __nsl_tsd_alloc);
	return (struct __nsl_tsd *) pthread_getspecific(__nsl_tsd_key);
};

int *
__ncerror(void)
{
	return &(__nsl_get_tsd()->ncerror);
}

int *
__nderror(void)
{
	return &(__nsl_get_tsd()->nderror);
}

char *
__ncerrbuf(void)
{
	return &(__nsl_get_tsd()->ncerrbuf[0]);
}

char *
__nderrbuf(void)
{
	return &(__nsl_get_tsd()->nderrbuf[0]);
}

/*
 *  Network Selection Facility functions:
 *  =====================================
 */

/**
 * @fn struct netconfig *getnetconfig(void *handle);
 * @brief Retrieve next entry in the netconfig database.
 * @param handle handle returned by setnetconfig().
 *
 * This function returns a pointer to the current entry in the netconfig
 * database, formatted as a struct netconfig.  Successive calls will return
 * successive netconfig entries in the netconfig database.  This function can be
 * used to search the entire netconfig file.  This function returns NULL at the
 * end of the file.  handle is a the handle obtained through setnetconfig().
 */
struct netconfig *
__nsl_getnetconfig(void *handle)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netconfig *getnetconfig(void *handle)
    __attribute__ ((weak, alias("__nsl_getnetconfig")));

/**
 * @fn void *setnetconfig(void);
 * @brief Provide a handle to the netconfig database.
 *
 * This function hase the effect of binding to or rewinding the netconfig
 * database.  This function must be called before the first call to
 * getnetconfig() and may be called at any other time.  This function need not
 * be called before a call to getnetconfigent().  This function returns a unique
 * handle to be used by getnetconfig().
 *
 * This function returns a pointer to the current entry in the netconfig
 * database, formatted as a struct netconfig.  This function returns NULL at the
 * end of the file, or upon failure.
 */
void *
__nsl_setnetconfig(void)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

void *setnetconfig(void)
    __attribute__ ((weak, alias("__nsl_setnetconfig")));

/**
 * @fn int endnetconfig(void *handle);
 * @brief Release network configuration database.
 * @param handle handle returned by setnetconfig().
 *
 * This function should be called when processing is complete to release
 * resources held for reuse.  handle is the handle obtained through
 * setnetconfig().  Programmers should be aware, however, that the last call to
 * endnetconfig() frees all memory alocated by getnetconfig() for the struct
 * netconfig data structure.  This function may not be called before
 * setnetconfig().
 *
 * This function returns a unique handle to be used by getnetconfig().  IN the
 * case of an error, this function returns NULL and nc_perror() or nc_sperror()
 * can be used to print the reason for failure.
 */
int
__nsl_endnetconfig(void *handle)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int endnetconfig(void *handle)
    __attribute__ ((weak, alias("__nsl_endnetconfig")));

/**
 * @fn struct netconfig *getnetconfigent(const char *netid);
 * @brief Return a network configuration entry for a network id.
 * @param netid the network id.
 *
 * This function returns a pointer to the struct netconfig structure
 * corresponding to the argument netid.  It returns NULL if netid is invalid
 * (that is, does not name an entry in the netconfig database).
 *
 * This function returns 0 on success and -1 on failure (for example, if
 * setnetconfig() was not called previously).
 */
struct netconfig *
__nsl_getnetconfigent(const char *netid)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netconfig *getnetconfigent(const char *netid)
    __attribute__ ((weak, alias("__nsl_getnetconfigent")));

/**
 * @fn void freenetconfigent(struct netconfig *netconfig);
 * @brief Free a netconfig database entry.
 * @param netconfig the database entry to free.
 *
 * This function frees the netconfig structure pointed to by netconfig
 * (previously returned by getneconfigent()).
 *
 * Upon success, this function returns a pointer to the struct netconfig
 * structure corresponding to the netid; otherwise, it returns NULL.
 */
void
__nsl_freenetconfigent(struct netconfig *netconfig)
{
	errno = EOPNOTSUPP;
	return;
}

void freenetconfigent(struct netconfig *netconfig)
    __attribute__ ((weak, alias("__nsl_freenetconfigent")));

/* *INDENT-OFF* */
static const char *__nsl_nc_errlist[] = {
/*
TRANS Corresponds to the NC_NOERROR constant.  No error is indicated in the
TRANS nc_error(3) variable.  The last netconfig(3) or netpath(3) operation was
TRANS successful.
 */
	[NC_NOERROR] = gettext_noop("no error"),
/*
TRANS Corresponds to the NC_NOMEM constant.  Out of memory.  Memory could not be
TRANS allocated to complete the last netconfig(3) or netpath(3) operation.
 */
	[NC_NOMEM] = gettext_noop("out of memory"),
/*
TRANS Corresponds to the NC_NOSET constant.  The getnetconfig(3), getnetpath(3),
TRANS endnetconfig(3), or endnetpath(3) function was called out of order: that
TRANS is, before setnetconfig(3) or setnetpath(3) was called.
 */
	[NC_NOSET] = gettext_noop("routine called out of order"),
/*
TRANS Corresponds to the NC_OPENFAIL constant.  The /etc/netconfig file could
TRANS not be opened for reading.  %1$s is the name of the /etc/netconfig file.
 */
	[NC_OPENFAIL] = gettext_noop("could not open %s"),
/*
TRANS Corresponds to the NC_BADLINE constant.  A syntax error exists in the
TRANS /etc/netconfig file at the location shown.  The field number is the
TRANS whitespace separated field within the line in the /etc/netconfig file at
TRANS which the syntax error was detected.  The line number is the line number
TRANS in the /etc/netconfig file at which the syntax error was detected.  %1$s
TRANS is the name of the /etc/netconfig file.  %2$d is the field number.  %3$d
TRANS is the line number.
 */
	[NC_BADLINE] = gettext_noop("syntax error in %s: field %d of line %d"),
/*
TRANS Corresponds to the NC_NOTFOUND constant.  The "netid" specified as an
TRANS argument to the getnetconfigent(3) subroutine was not found in any entry
TRANS in the /etc/netconfig file.  %1$s is the name of the /etc/netconfig file.
 */
	[NC_NOTFOUND] = gettext_noop("netid not found in %s"),
/*
TRANS Corresponds to the NC_NOMOREENTRIES constant.  All entries in the
TRANS /etc/netconfig file has been exhausted.  They have either already been
TRANS retrieved with getnetconfig(3), getnetconfigent(3), getnetpath(3), or the
TRANS /etc/netconfig file contains no entries.  %1$s is the name of the
TRANS /etc/netconfig file.
 */
	[NC_NOMOREENTRIES] = gettext_noop("no more entries in %s"),
/*
TRANS Corresponds to any other constant.  An internal error in the libxnsl(3)
TRANS library has occurred resulting in an undefined error number.  This
TRANS situation should not occur and represents a bug in the library.  %1$d is
TRANS the decimal value of the unknown error number.
 */
	[NC_ERROR_MAX] = gettext_noop("unknown error number %d"),
};
/* *INDENT-ON* */

/**
 * @fn char *nc_sperror(void);
 * @brief Return an error string.
 *
 * This function is similar to nc_perror() but instead of printing the message
 * to standard error, will return a pointer to a string that contains the error
 * message.
 *
 * This function can also be used with the netpath access routines.
 *
 * This function returns a pointer to a buffer that contains the error messages
 * tirng.  This buffer is overwritten on each call.  In multithreaded
 * applications, this buffer is implemented as thread-specific data.
 *
 */
char *
__nsl_nc_sperror(void)
{
	int err, idx;
	struct __nsl_tsd *tsd = __nsl_get_tsd();
	char *errbuf = tsd->ncerrbuf;

	if (errbuf != NULL) {
		switch ((idx = err = tsd->ncerror)) {
		case NC_NOERROR:
		case NC_NOMEM:
		case NC_NOSET:
			(void) strncpy(errbuf, __nsl_nc_errlist[idx], NCERR_BUFSZ);
			break;
		case NC_OPENFAIL:
		case NC_NOTFOUND:
		case NC_NOMOREENTRIES:
			(void) snprintf(errbuf, NCERR_BUFSZ, __nsl_nc_errlist[idx], NETCONFIG);
			break;
		case NC_BADLINE:
			(void) snprintf(errbuf, NCERR_BUFSZ, __nsl_nc_errlist[idx], NETCONFIG,
					tsd->fieldnum, tsd->linenum);
			break;
		default:
			idx = NC_ERROR_MAX;
			(void) snprintf(errbuf, NCERR_BUFSZ, __nsl_nc_errlist[idx], err);
			break;
		}
	}
	return (errbuf);
}

char *nc_sperror(void)
    __attribute__ ((weak, alias("__nsl_nc_sperror")));

/**
 * @fn void nc_perror(const char *msg);
 * @brief Print an error message to standard output.
 * @param msg message to prefix to error message.
 *
 * This function prints and error message to standard error indicating why any
 * of the above routines failed.  The message is prepended with the string
 * provided in the msg argument and a colon.  A NEWLINE is appended to the end
 * of the message.
 *
 * This function can also be used with the netpath access routines.
 */
void
__nsl_nc_perror(const char *msg)
{
	if (msg)
		fprintf(stderr, "%s: %s\n", msg, __nsl_nc_sperror());
	else
		fprintf(stderr, "%s\n", __nsl_nc_sperror());
	return;
}

void nc_perror(const char *msg)
    __attribute__ ((weak, alias("__nsl_nc_perror")));

/**
 * @fn struct netconfig *getnetpath(void *handle);
 * @brief Get the next entry associated with handle.
 * @param handle a pointer to the handle returned by setnetpath().
 *
 * This function returns a pointer to the netconfig database entry corresponding
 * to the first valid NETPATH component.  The netconfig entry is formattted as a
 * struct netconfig.  On each subsequenc all, this function returns a pointer to
 * the netconfig entry that corresponds to the next valud NETPATH component.
 * This function can thus be used to search the netconfig database for all
 * networks included in the NETPATH variable.  When NETPATH has been exhausted,
 * this function returns NULL.
 *
 * This function silently ignores invalid NETPATH components.  A NETPATH
 * component is invalid if there is no corresponding entry in the netconfig
 * database.
 *
 * If the NETPATH environment variable is unset, this function bethaves as if
 * NETPATH were set to the sequence of "default" or "visible" networks in the
 * netconfig database, in the order in which they are listed.
 *
 * When first called, this function returns a pointer to the netconfig database
 * entry corresponding to the first valid NETPATH component.  When NETPATH has
 * been exhausted, it returns NULL.
 */
struct netconfig *
__nsl_getnetpath(void *handle)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netconfig *getnetpath(void *handle)
    __attribute__ ((weak, alias("__nsl_getnetpath")));

/**
 * @fn void *setnetpath(void);
 * @brief Get a handle for network configuration database.
 *
 * A call to this function binds to or rewinds NETPATH.  This function must be
 * called before the first call to getnetpath() and may be called at any other
 * time.  It returns a handle that is used by getnetpath().
 *
 * This function returns a handle that is used by getnetpath().  In case of an
 * error, this function returns NULL.  nc_perror() or nc_sperror() can be used
 * to print out the reason for failure.  See getnetconfig().
 */
void *
__nsl_setnetpath(void)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

void *setnetpath(void)
    __attribute__ ((weak, alias("__nsl_setnetpath")));

/**
 * @fn int endnetpath(void *handle);
 * @brief Free netpath resources associated with handle.
 * @param handle a pointer to the handle returned by setnetpath().
 *
 * This function may be called to unbind from NETPATH when processing is
 * complete, releasing resources for reuse.  Programmers should be aware,
 * however, that this function frees all memory allocated by getnetpath() for
 * the struct netconfig data structure.
 *
 * This function returns 0 on success and -1 on failure (for example, if
 * setnetpath() was not called previously).
 *
 */
int
__nsl_endnetpath(void *handle)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int endnetpath(void *handle)
    __attribute__ ((weak, alias("__nsl_endnetpath")));

/*
 *  Name-to-Address Translation Functions:
 *  ======================================
 */

pthread_rwlock_t __nsl_xlate_lock = PTHREAD_RWLOCK_INITIALIZER;

struct __nsl_xlate {
	struct __nsl_xlate *next;
	char *path;
	struct nd_addrlist *(*getbyname) (struct netconfig *, struct nd_hostserv *);
	struct nd_hostservlist *(*getbyaddr) (struct netconfig *, struct netbuf *);
	int (*options) (struct netconfig *, int, int, char *);
	char *(*taddr2_uaddr) (struct netconfig *, struct netbuf *);
	struct netbuf *(*uaddr2_taddr) (struct netconfig *, struct netbuf *);
	void *lib;
} *__nsl_xlate_list = NULL;

extern struct __nsl_xlate *load_xlate(const char *name);

/**
 * @fn int netdir_getbyname(struct netconfig *config, struct nd_hostserv *service, struct nd_addrlist **addrs);
 * @brief Map machine and service name to transport addresses.
 * @param config pointer to transport configuration data structure.
 * @param service pointer to nd_hostserv structure identifying machine and service name.
 * @param addrs returned pointer to allocated nd_addrlist representing the resulting transport addresses.
 *
 * This function converts the machine anem and service name in the nd_hostserv
 * structure to a collection of addresses of the type understood by the
 * transport identified in the netconfig structure.
 */
int
__nsl_netdir_getbyname(struct netconfig *config, struct nd_hostserv *service,
		       struct nd_addrlist **addrs)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int netdir_getbyname(struct netconfig *config, struct nd_hostserv *service,
		     struct nd_addrlist **addrs)
    __attribute__ ((weak, alias("__nsl_netdir_getbyname")));

/**
 * @fn int netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service, struct netbuf *netaddr);
 * @brief Map transport address to a collection of machine and service names.
 * @param config pointer to transport configuration data structure.
 * @param service returned pointer to a nd_hostservlist structure identifying the machine and service names.
 * @param netaddr pointer to a netbuf structure describing the transport address.
 *
 * This function maps addresses to service names.  The function returns a
 * service, a list of host and service pairs that yeild these addresses.  If
 * more than one tuple of host and service name is returned, the first type
 * contains the preferred host and service names.
 */
int
__nsl_netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service,
		       struct netbuf *netaddr)
{
	if (config == NULL) {
		_nderror = ND_BADARG;
	} else if (config->nc_lookups == 0
		   && (strcmp(config->nc_protofmly, NC_INET) == 0
		       || strcmp(config->nc_protofmly, NC_INET6) == 0)) {
	} else {
		struct __nsl_xlate *xl = NULL;
		char *lib = NULL;
		int i;

		_nderror = ND_SYSTEM;
		for (i = 0; i < config->nc_nlookups; i++) {
			lib = config->nc_lookups[i];
			for (xl = __nsl_xlate_list; xl; xl = xl->next) {
				if (strcmp(lib, xl->path) == 0) {
					if ((*service = (*xl->getbyaddr) (config, netaddr)) != NULL)
						return (ND_OK);
					if (_nderror < 0)
						return (_nderror);
				}
			}

		}
		if (xl == NULL) {
			/* could not find translation library */
			if ((xl = load_xlate(lib)) != NULL) {
				pthread_rwlock_wrlock(&__nsl_xlate_lock);
				xl->next = __nsl_xlate_list;
				__nsl_xlate_list = xl;
				pthread_rwlock_unlock(&__nsl_xlate_lock);
				if ((*service = (*xl->getbyaddr) (config, netaddr)) != NULL)
					return (ND_OK);
				if (_nderror < 0)
					return (_nderror);
			}
		}
	}
	return (_nderror);
}

int netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service,
		     struct netbuf *netaddr)
    __attribute__ ((weak, alias("__nsl_netdir_getbyaddr")));

/**
 * @fn void netdir_free(void *ptr, int struct_type);
 * @brief frees a structure returned by other netdir functions.
 * @param ptr pointer to the structure to free.
 * @param struct_type type of structure.
 *
 * This function is used to free the structures allocated by the name to
 * address translation functions.  The ptr parameter points to the structure
 * that has to be freed.  The parameter struct_type identifies the structure
 * and is one of ND_ADDR, ND_ADDRLIST, ND_HOSTSERV, ND_HOSTSERVLIST.  Note
 * that universal addresses are freed with free().
 */
void
__nsl_netdir_free(void *ptr, int struct_type)
{
	if (ptr == NULL) {
		_nderror = ND_BADARG;
		return;
	}
	switch (struct_type) {
	case ND_ADDR:
	{
		struct netbuf *na;

		na = (struct netbuf *) ptr;
		if (na->buf)
			free(na->buf);
		free(na);
		break;
	}
	case ND_ADDRLIST:
	{
		struct nd_addrlist *nal;
		int i;

		nal = (struct nd_addrlist *) ptr;
		for (i = 0; i < nal->n_cnt; i++)
			if (nal->n_addrs[i].buf)
				free(nal->n_addrs[i].buf);
		if (nal->n_addrs)
			free(nal->n_addrs);
		free(nal);
		break;
	}
	case ND_HOSTSERV:
	{
		struct nd_hostserv *nhs;

		nhs = (struct nd_hostserv *) ptr;
		if (nhs->h_name)
			free(nhs->h_name);
		if (nhs->h_serv)
			free(nhs->h_serv);
		free(nhs);
		break;
	}
	case ND_HOSTSERVLIST:
	{
		struct nd_hostservlist *nhsl;
		int i;

		nhsl = (struct nd_hostservlist *) ptr;
		for (i = 0; i < nhsl->h_cnt; i++) {
			if (nhsl->h_hostservs[i].h_name)
				free(nhsl->h_hostservs[i].h_name);
			if (nhsl->h_hostservs[i].h_serv)
				free(nhsl->h_hostservs[i].h_serv);
		}
		if (nhsl->h_hostservs)
			free(nhsl->h_hostservs);
		free(nhsl);
		break;
	}
	default:
		_nderror = ND_UKNWN;
		break;
	}
	return;
}

void netdir_free(void *ptr, int struct_type)
    __attribute__ ((weak, alias("__nsl_netdir_free")));

/**
 * @fn int netdir_options(struct netconfig *config, int option, int fd, char *point_to_args);
 * @brief manage universal transport options.
 * @param config pointer to transport configuration structure.
 * @param universal option option to manage.
 * @param fd transport endpoint file descriptor.
 * @param point_to_args arguments to set.
 *
 * This function is used to do all transport specific setups and option
 * management.  fd is the associated file descriptor.  option, fd, and
 * pointer_to_args are passed ot the netdir_options() function for the
 * transport specified in config.
 */
int
__nsl_netdir_options(struct netconfig *config, int option, int fd, char *point_to_args)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int netdir_options(struct netconfig *config, int option, int fd, char *point_to_args)
    __attribute__ ((weak, alias("__nsl_netdir_options")));

/**
 * @fn char *taddr2uaddr(struct netconfig *config, struct netbuf *addr);
 * @brief Converts a transport address to a universal address.
 * @param config a pointer to the transport configuration data structure.
 * @param addr the transport address to convert.
 *
 * This function supports transaltion between universal addresses and TLI type
 * netbufs.  The taddr2uaddr() function takes a struct netbuf data structure
 * and returns a pointer to a string that contains the universal address.  It
 * returns NULL if the conversion is not possible.  This is not a fatal
 * condition as some transports do not support a universal address form.
 */
char *
__nsl_taddr2uaddr(struct netconfig *config, struct netbuf *addr)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

char *taddr2uaddr(struct netconfig *config, struct netbuf *addr)
    __attribute__ ((weak, alias("__nsl_taddr2uaddr")));

/**
 * @fn struct netbuf *uaddr2taddr(struct netconfig *config, struct netbuf *addr);
 * @brief Converts a universal address to a transport address.
 * @param config a pointer to the transport configuration data structure.
 * @param addr the universal address to convert.
 *
 * This function is the reverse of the taddr2uaddr() function. It returns the
 * struct netbuf data structure for the given universal address.
 */
struct netbuf *
__nsl_uaddr2taddr(struct netconfig *config, struct netbuf *addr)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netbuf *uaddr2taddr(struct netconfig *config, struct netbuf *addr)
    __attribute__ ((weak, alias("__nsl_uaddr2taddr")));

/* *INDENT-OFF* */
static const char *__nsl_nd_errlist[] = {
/*
TRANS Corresponds to the ND_TRY_AGAIN constant.  This is a fatal error in the
TRANS sense that name-to-address mapping translations will stop at this point.
 */
	[ND_TRY_AGAIN-ND_ERROR_OFS] = gettext_noop("n2a: non-authoritative host not found"),
/*
TRANS Corresponds to the ND_NO_RECOVERY constant.  This is a fatal error in the
TRANS sense that name-to-address mapping translations will stop at this point.
 */
	[ND_NO_RECOVERY-ND_ERROR_OFS] = gettext_noop("n2a: non-recoverable error"),
/*
TRANS Corresponds to the ND_NO_DATA and ND_NO_ADDRESS constant.  This is a fatal
TRANS error in the sense that name-to-address mapping translations will stop at
TRANS this point.
 */
	[ND_NO_DATA-ND_ERROR_OFS] = gettext_noop("n2a: no data record of requested type"),
/*
TRANS Corresponds to the ND_BADARG constant.  This is a fatal error in the sense
TRANS that name-to-address mapping translations will stop at this point.
 */
	[ND_BADARG-ND_ERROR_OFS] = gettext_noop("n2a: bad arguments passed to routine"),
/*
TRANS Corresponds to the ND_NOMEM constant.  This is a fatal error in the sense
TRANS that name-to-address mapping translations will stop at this point.
 */
	[ND_NOMEM-ND_ERROR_OFS] = gettext_noop("n2a: memory allocation failed"),
/*
TRANS Corresponds to the ND_OK constant.  This is not an error.
 */
	[ND_OK-ND_ERROR_OFS] = gettext_noop("n2a: successful completion"),
/*
TRANS Corresponds to the ND_NOHOST constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOHOST-ND_ERROR_OFS] = gettext_noop("n2a: hostname not found"),
/*
TRANS Corresponds to the ND_NOSERV constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOSERV-ND_ERROR_OFS] = gettext_noop("n2a: service name not found"),
/*
TRANS Corresponds to the ND_NOSYM constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.  %1$s is a dlerror(3) string.
 */
	[ND_NOSYM-ND_ERROR_OFS] = gettext_noop("n2a: symbol missing in shared object: %s"),
/*
TRANS Corresponds to the ND_OPEN constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.  %1$s is a dlerror(3) string.
 */
	[ND_OPEN-ND_ERROR_OFS] = gettext_noop("n2a: could not open shared object: %s"),
/*
TRANS Corresponds to the ND_ACCESS constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_ACCESS-ND_ERROR_OFS] = gettext_noop("n2a: access denied for shared object"),
/*
TRANS Corresponds to the ND_UKNWN constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_UKNWN-ND_ERROR_OFS] = gettext_noop("n2a: attempt to free unknown object"),
/*
TRANS Corresponds to the ND_NOCTRL constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOCTRL-ND_ERROR_OFS] = gettext_noop("n2a: unknown option passed"),
/*
TRANS Corresponds to the ND_FAILCTRL constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_FAILCTRL-ND_ERROR_OFS] = gettext_noop("n2a: control operation failed"),
/*
TRANS Corresponds to the ND_SYSTEM constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_SYSTEM-ND_ERROR_OFS] = gettext_noop("n2a: system error"),
/*
TRANS Corresponds to the ND_NOCONVERT constant.  This is a non-fatal error in
TRANS the sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOCONVERT-ND_ERROR_OFS] = gettext_noop("n2a: conversion not possible"),
/*
TRANS Corresponds to the any other constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point. %1$d is the error number.
 */
	[ND_ERROR_MAX-ND_ERROR_OFS] = gettext_noop("n2a: unknown error %d"),
};
/* *INDENT-ON* */

/**
 * @fn char *netdir_sperror(void);
 * @brief Return a netdir function error string.
 *
 * This function returns a pointer to a buffer that contains the error message
 * string.  The buffer is overwritten on each call.  In multhreaded
 * applications, this buffers is implemented as thread-specific data.
 */
char *
__nsl_netdir_sperror(void)
{
	int err, idx;
	struct __nsl_tsd *tsd = __nsl_get_tsd();
	char *errbuf = tsd->nderrbuf;

	if (errbuf != NULL) {
		err = tsd->nderror;
		idx = err - ND_ERROR_OFS;
		switch (err) {
		case ND_TRY_AGAIN:
		case ND_NO_RECOVERY:
		case ND_NO_DATA:
		case ND_BADARG:
		case ND_NOMEM:
		case ND_OK:
		case ND_NOHOST:
		case ND_NOSERV:
		case ND_ACCESS:
		case ND_UKNWN:
		case ND_NOCTRL:
		case ND_FAILCTRL:
		case ND_SYSTEM:
		case ND_NOCONVERT:
			(void) strncpy(errbuf, __nsl_nd_errlist[idx], NDERR_BUFSZ);
			break;
		case ND_NOSYM:
		case ND_OPEN:
			(void) snprintf(errbuf, NDERR_BUFSZ, __nsl_nd_errlist[idx], dlerror());
			break;
		default:
			idx = ND_ERROR_MAX - ND_ERROR_OFS;
			(void) snprintf(errbuf, NDERR_BUFSZ, __nsl_nd_errlist[idx], err);
			break;
		}
	}
	return (errbuf);
}

char *netdir_sperror(void)
    __attribute__ ((weak, alias("__nsl_netdir_sperror")));

/**
 * @fn void netdir_perror(char *msg);
 * @brief Print a netdir function error string to standard output.
 * @param msg prefix string.
 *
 * This function prints an error message to standard output that states the
 * cause of a name-to-address mapping failure.  The error message is preceded by
 * the string given as an argument.
 *
 */
void
__nsl_netdir_perror(char *msg)
{
	if (msg)
		fprintf(stderr, "%s: %s\n", msg, __nsl_netdir_sperror());
	else
		fprintf(stderr, "%s\n", __nsl_netdir_sperror());
	return;
}
void netdir_perror(char *s)
    __attribute__ ((weak, alias("__nsl_netdir_perror")));

/**
 * @section Identification
 * This development manual was written for the OpenSS7 NSL Library version \$Name:  $(\$Revision: 0.9.2.19 $).
 * @author Brian F. G. Bidulock
 * @version \$Name:  $(\$Revision: 0.9.2.19 $)
 * @date \$Date: 2006/07/24 09:01:54 $
 *
 * @}
 */

/*
 * vim: comments+=b\:TRANS
 */
