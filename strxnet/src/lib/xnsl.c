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
	___pthread_keys[key] = (void *)pointer;
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

/*
 * Thread-specific data structure for the xnsl library.
 */
struct __nsl_tsd {
	int nderror;			/* error for network directory functions */
	int ncerror;			/* error for network configuration functions */
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
 * This function obtains (and allocates if necessary), thread specific data for the executing
 * thread.
 */
static struct __nsl_tsd *
__nsl_get_tsd(void)
{
	pthread_once(&__nsl_tsd_once, __nsl_tsd_alloc);
	return (struct __nsl_tsd *) pthread_getspecific(__nsl_tsd_key);
};

int *
__nderror(void)
{
	return &(__nsl_get_tsd()->nderror);
}

int *
__ncerror(void)
{
	return &(__nsl_get_tsd()->ncerror);
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
 * This function returns a pointer to the current entry in the netconfig database, formatted as a
 * struct netconfig.  Successive calls will return successive netconfig entries in the netconfig
 * database.  This function can be used to search the entire netconfig file.  This function returns
 * NULL at the end of the file.  handle is a the handle obtained through setnetconfig().
 *
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
 * This function hase the effect of binding to or rewinding the netconfig database.  This function
 * must be called before the first call to getnetconfig() and may be called at any other time.  This
 * function need not be called before a call to getnetconfigent().  This function returns a unique
 * handle to be used by getnetconfig().
 *
 * This function returns a pointer to the current entry in the netconfig database, formatted as a
 * struct netconfig.  This function returns NULL at the end of the file, or upon failure.
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
 * This function should be called when processing is complete to release resources held for reuse.
 * handle is the handle obtained through setnetconfig().  Programmers should be aware, however, that
 * the last call to endnetconfig() frees all memory alocated by getnetconfig() for the struct
 * netconfig data structure.  This function may not be called before setnetconfig().
 *
 * This function returns a unique handle to be used by getnetconfig().  IN the case of an error,
 * this function returns NULL and nc_perror() or nc_sperror() can be used to print the reason for
 * failure.
 *
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
 * This function returns a pointer to the struct netconfig structure corresponding to the argument
 * netid.  It returns NULL if netid is invalid (that is, does not name an entry in the netconfig
 * database).
 *
 * This function returns 0 on success and -1 on failure (for example, if setnetconfig() was not
 * called previously).
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
 * This function frees the netconfig structure pointed to by netconfig (previously returned by
 * getneconfigent()).
 *
 * Upon success, this function returns a pointer to the struct netconfig structure corresponding to
 * the netid; otherwise, it returns NULL.
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
	gettext_noop("no error"),
/*
TRANS Corresponds to the NC_NOMEM constant.  Out of memory.  Memory could not be
TRANS allocated to complete the last netconfig(3) or netpath(3) operation.
 */
	gettext_noop("out of memory"),
/*
TRANS Corresponds to the NC_NOSET constant.  The getnetconfig(3), getnetpath(3),
TRANS endnetconfig(3), or endnetpath(3) function was called out of order: that
TRANS is, before setnetconfig(3) or setnetpath(3) was called.
 */
	gettext_noop("routine called out of order"),
/*
TRANS Corresponds to the NC_OPENFAIL constant.  The /etc/netconfig file could
TRANS not be opened for reading.
 */
	gettext_noop("could not open " NETCONFIG),
/*
TRANS Corresponds to the NC_BADLINE constant.  A syntax error exists in the
TRANS /etc/netconfig file at the location shown.  The field number is the
TRANS whitespace separated field within the line in the /etc/netconfig file at
TRANS which the syntax error was detected.  The line number is the line number
TRANS in the /etc/netconfig file at which the syntax error was detected.
 */
	gettext_noop("syntax error in " NETCONFIG ": field %d of line %d"),
/*
TRANS Corresponds to the NC_NOTFOUND constant.  The "netid" specified as an
TRANS argument to the getnetconfigent(3) subroutine was not found in any entry
TRANS in the /etc/netconfig file.
 */
	gettext_noop("netid not found in " NETCONFIG),
/*
TRANS Corresponds to the NC_NOMOREENTRIES constant.  All entries in the
TRANS /etc/netconfig file has been exhausted.  They have either already been
TRANS retrieved with getnetconfig(3), getnetconfigent(3), getnetpath(3), or the
TRANS /etc/netconfig file contains no entries.
 */
	gettext_noop("no more entries in " NETCONFIG),
/*
TRANS Corresponds to any other constant.  An internal error in the libxnsl(3)
TRANS library has occurred resulting in an undefined error number.  This
TRANS situation should not occur and represents a bug in the library.
 */
	gettext_noop("unknown error"),
};
/* *INDENT-ON* */

/**
 * @fn char *nc_sperror(void);
 * @brief Return an error string.
 *
 * This function is similar to nc_perror() but instead of printing the message to standard error,
 * will return a pointer to a string that contains the error message.
 *
 * This function can also be used with the netpath access routines.
 *
 * This function returns a pointer to a buffer that contains the error messages tirng.  This buffer
 * is overwritten on each call.  In multithreaded applications, this buffer is implemented as
 * thread-specific data.
 *
 */
char *
__nsl_nc_sperror(void)
{
	int err;
	switch ((err = nc_error)) {
	default:
		err = NC_NOMOREENTRIES+1;
	case NC_NOERROR:
	case NC_NOMEM:
	case NC_NOSET:
	case NC_OPENFAIL:
	case NC_BADLINE:
	case NC_NOTFOUND:
	case NC_NOMOREENTRIES:
		return (char *)(__nsl_nc_errlist[err]);
	}
	errno = EOPNOTSUPP;
	return (NULL);
}

char *nc_sperror(void)
    __attribute__ ((weak, alias("__nsl_nc_sperror")));

/**
 * @fn void nc_perror(const char *msg);
 * @brief Print an error message to standard output.
 * @param msg message to prefix to error message.
 *
 * This function prints and error message to standard error indicating why any of the above routines
 * failed.  The message is prepended with the string provided in the msg argument and a colon.  A
 * NEWLINE is appended to the end of the message.
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
 * This function returns a pointer to the netconfig database entry corresponding to the first valid
 * NETPATH component.  The netconfig entry is formattted as a struct netconfig.  On each subsequenc
 * all, this function returns a pointer to the netconfig entry that corresponds to the next valud
 * NETPATH component.  This function can thus be used to search the netconfig database for all
 * networks included in the NETPATH variable.  When NETPATH has been exhausted, this function
 * returns NULL.
 *
 * This function silently ignores invalid NETPATH components.  A NETPATH component is invalid if
 * there is no corresponding entry in the netconfig database.
 *
 * If the NETPATH environment variable is unset, this function bethaves as if NETPATH were set to
 * the sequence of "default" or "visible" networks in the netconfig database, in the order in which
 * they are listed.
 *
 * When first called, this function returns a pointer to the netconfig database entry corresponding
 * to the first valid NETPATH component.  When NETPATH has been exhausted, it returns NULL.
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
 * A call to this function binds to or rewinds NETPATH.  This function must be called before the
 * first call to getnetpath() and may be called at any other time.  It returns a handle that is used
 * by getnetpath().
 *
 * This function returns a handle that is used by getnetpath().  In case of an error, this function
 * returns NULL.  nc_perror() or nc_sperror() can be used to print out the reason for failure.  See
 * getnetconfig().
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
 * This function may be called to unbind from NETPATH when processing is complete, releasing
 * resources for reuse.  Programmers should be aware, however, that this function frees all memory
 * allocated by getnetpath() for the struct netconfig data structure.
 *
 * This function returns 0 on success and -1 on failure (for example, if setnetpath() was not called
 * previously).
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
	errno = EOPNOTSUPP;
	return (-1);
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
	errno = EOPNOTSUPP;
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
	gettext_noop("n2a: non-authoritative host not found"),
/*
TRANS Corresponds to the ND_NO_RECOVERY constant.  This is a fatal error in the
TRANS sense that name-to-address mapping translations will stop at this point.
 */
	gettext_noop("n2a: non-recoverable error"),
/*
TRANS Corresponds to the ND_NO_DATA and ND_NO_ADDRESS constant.  This is a fatal
TRANS error in the sense that name-to-address mapping translations will stop at
TRANS this point.
 */
	gettext_noop("n2a: no data record of requested type"),
/*
TRANS Corresponds to the ND_BADARG constant.  This is a fatal error in the sense
TRANS that name-to-address mapping translations will stop at this point.
 */
	gettext_noop("n2a: bad arguments passed to routine"),
/*
TRANS Corresponds to the ND_NOMEM constant.  This is a fatal error in the sense
TRANS that name-to-address mapping translations will stop at this point.
 */
	gettext_noop("n2a: memory allocation failed"),
/*
TRANS Corresponds to the ND_OK constant.  This is not an error.
 */
	gettext_noop("n2a: successful completion"),
/*
TRANS Corresponds to the ND_NOHOST constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: hostname not found"),
/*
TRANS Corresponds to the ND_NOSERV constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: service name not found"),
/*
TRANS Corresponds to the ND_NOSYM constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: symbol missing in shared object: %s"),
/*
TRANS Corresponds to the ND_OPEN constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: could not open shared object: %s"),
/*
TRANS Corresponds to the ND_ACCESS constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: access denied for shared object"),
/*
TRANS Corresponds to the ND_UKNWN constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: attempt to free unknown object"),
/*
TRANS Corresponds to the ND_NOCTRL constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: unknown option passed"),
/*
TRANS Corresponds to the ND_FAILCTRL constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: control operation failed"),
/*
TRANS Corresponds to the ND_SYSTEM constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: system error"),
/*
TRANS Corresponds to the ND_NOCONVERT constant.  This is a non-fatal error in
TRANS the sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: conversion not possible"),
/*
TRANS Corresponds to the any other constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	gettext_noop("n2a: unknown error"),
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
	errno = EOPNOTSUPP;
	return (NULL);
}
char *netdir_sperror(void)
    __attribute__ ((weak, alias("__nsl_netdir_sperror")));

/**
 * @fn void netdir_perror(char *s);
 * @brief Print a netdir function error string to standard output.
 * @param s prefix string.
 *
 * This function prints an error message to standard output that states the
 * cause of a name-to-address mapping failure.  The error message is preceded
 * by the string given as an argument.
 *
 */
void
__nsl_netdir_perror(char *s)
{
	errno = EOPNOTSUPP;
}
void netdir_perror(char *s)
    __attribute__ ((weak, alias("__nsl_netdir_perror")));

/*
 * vim: comments+=b\:TRANS
 */
