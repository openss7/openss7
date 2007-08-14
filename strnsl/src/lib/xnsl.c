/*****************************************************************************

 @(#) $RCSfile: xnsl.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 04:56:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 04:56:59 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xnsl.c,v $
 Revision 0.9.2.3  2007/08/14 04:56:59  brian
 - GPLv3 header update

 Revision 0.9.2.2  2006/10/02 11:31:54  brian
 - changes to get master builds working for RPM and DEB
 - added outside licenses to package documentation
 - added LICENSE automated release file
 - copy MANUAL to source directory
 - add and remove devices in -dev debian subpackages
 - get debian rules working better
 - release library version files
 - added notes to debian changelog
 - corrections for cooked manual pages in spec files
 - added release documentation to spec and rules files
 - copyright header updates
 - moved controlling tty checks in stream head
 - missing some defines for LiS build in various source files
 - added OSI headers to striso package
 - added includes and manual page paths to acincludes for various packages
 - added sunrpc, uidlpi, uinpi and uitpi licenses to documentation and release
   files
 - moved pragma weak statements ahead of declarations
 - changes for master build of RPMS and DEBS with LiS

 Revision 0.9.2.1  2006/09/25 12:30:57  brian
 - added files for new strnsl package

 Revision 0.9.2.3  2006/09/22 20:54:28  brian
 - tweaked source file for use with doxygen

 Revision 0.9.2.2  2006/09/18 13:52:56  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:03:15  brian
 - added libxnsl source files

 *****************************************************************************/

#ident "@(#) $RCSfile: xnsl.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 04:56:59 $"

static char const ident[] =
    "$RCSfile: xnsl.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 04:56:59 $";

/* This file cannot be processed with doxygen. */

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
#include <xti_inet.h>
#include <netconfig.h>
#include <netdir.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

/**
 * @defgroup libxnsl OpenSS7 Network Services Library (NSL)
 * @brief OpenSS7 NSL Library Calls
 *
 * This manual contains documentation of the OpenSS7 Network Services Library functions that are
 * generated automatically from the source code with doxygen.  This documentation is intended to be
 * used for maintainers of the OpenSS7 NSL library and is not intended for users of the OpenSS7 NSL
 * library.  Users should consult the documentation found in the user manual pages beginning with
 * xnsl(3).
 *
 * <h2>Thread Safety</h2>
 * The OpenSS7 NSL library is designed to be thread-safe.  This is accomplished in a number of ways.
 * Thread-safety depends on the use of glibc2 and the pthreads library.
 *
 * Glibc2 provides lightweight thread-specific data for errno and h_errno.  The OpenSS7 NSL library
 * takes a similar approach but uses weak aliased pthread thread-specific functions instead.
 *
 * Glibc2 also provides some weak undefined aliases for POSIX thread functions to perform its own
 * thread-safety.  When the pthread library (libpthread) is linked with glibc2, these functions call
 * libpthread functions instead of internal dummy routines.  The same approach is taken for the
 * OpenSS7 NSL library.  The library uses weak defined and undefined aliases that automatically
 * invoke libpthread functions when libpthread is (dynamically) linked and uses dummy functions when
 * it is not.  This maintains maximum efficiency when libpthread is not dynamically linked, but
 * provides full thread safety when it is.
 *
 * Libpthread behaves in some strange ways with regards to thread cancellation.  Because libpthread
 * uses Linux clone processes for threads, cancellation of a thread is accomplished by sending a
 * signal to the thread process.  This does not directly result in cancellation, but will result in
 * the failure of a system call with the EINTR error code.  It is necessary to test for cancellation
 * upon error return from system calls to perform the actual cancellation of the thread.
 *
 * The XNS specification (OpenGroup XNS 5.2) lists no functions in this group as containing thread
 * cancellation points.  Many of the netdb functions are, however, permitted to contain thread
 * cancellation points.  All NSL functions are permitted to contain thread cancellation points and
 * deferral is not required.
 *
 * Locks and asynchronous thread cancellation present challenges:
 *
 * Functions that act as thread cancellation points must push routines onto the function stack
 * executed at exit of the thread to release the locks held by the function.  These are performed
 * with weak definitions of POSIX thread library functions.
 *
 * Functions that do not act as thread cancellation points must defer thread cancellation before
 * taking locks and then release locks before thread canceallation is restored.
 *
 * The above are the techniques used by glibc2 for the same purpose and is the same technique that
 * is used by the OpenSS7 NSL library.
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
#define NDERR_BUFSZ 512
#endif

#ifndef NCERR_BUFSZ
#define NCERR_BUFSZ 512
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
 * @fn struct __nsl_tsd *__nsl_get_tsd(void)
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

struct netconf_handle {
	struct netconfig **nh_head;
	struct netconfig **nh_curr;
	pthread_rwlock_t nh_lock;
};

#define nc_line(__nc) (char *)(__nc)->nc_unused[7]
#define nc_next(__nc) (struct netconfig *)(__nc)->nc_unused[8]
#define nc_nextp(__nc) (struct netconfig **)&(__nc)->nc_unused[8]

/* This is to allow us to change the name of the network configuration
 * database file primarily for testing with test scripts. */

const char *__nsl_netconfig = NETCONFIG;

#undef NETCONFIG
#define NETCONFIG __nsl_netconfig

static pthread_rwlock_t __nsl_nc_lock = PTHREAD_RWLOCK_INITIALIZER;
static struct netconfig **__nsl_nc_list = NULL;

static struct netconfig **
__nsl_loadnetconfiglist(void)
{
	char buffer[1024];
	char *line;
	int linenum, entries = 0;
	FILE *file = NULL;
	struct netconfig *nc = NULL, *nclist = NULL, **nclistp = &nclist;
	struct __nsl_tsd *tsd = __nsl_get_tsd();

	if ((file = fopen(NETCONFIG, "r")) == NULL)
		goto openfail;

	/* read file one line at a time */
	for (linenum = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; linenum++) {
		char *str, *field, *tmp = NULL;
		int fieldnum;

		/* allocate one if we don't already have one */
		if (nc == NULL) {
			if ((nc = malloc(sizeof(*nc))) == NULL)
				goto nomem;
			memset(nc, 0, sizeof(*nc));
		}
		if ((nc_line(nc) = strdup(line)) == 0)
			goto nomem;

		for (str = nc_line(nc), fieldnum = 0; fieldnum <= 6
		     && (field = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL;
		     str = NULL, fieldnum++) {
			switch (fieldnum) {
			case 0:	/* network id field */
				if (field[0] == '#')
					break;
				nc->nc_netid = field;
				continue;
			case 1:	/* semantics field */
				if (strcmp(field, "tpi_clts") == 0) {
					nc->nc_semantics = NC_TPI_CLTS;
				} else if (strcmp(field, "tpi_cots") == 0) {
					nc->nc_semantics = NC_TPI_COTS;
				} else if (strcmp(field, "tpi_cots_ord") == 0) {
					nc->nc_semantics = NC_TPI_COTS_ORD;
				} else if (strcmp(field, "tpi_raw") == 0) {
					nc->nc_semantics = NC_TPI_RAW;
				} else {
					tsd->ncerror = NC_BADLINE;
					tsd->fieldnum = fieldnum;
					tsd->linenum = linenum;
					break;
				}
				continue;
			case 2:	/* flag field */
				nc->nc_flag = 0;
				if (strcmp(field, "-") == 0)
					continue;
				if (strspn(field, "vb") != strlen(field)) {
					tsd->ncerror = NC_BADLINE;
					tsd->fieldnum = fieldnum;
					tsd->linenum = linenum;
					break;
				}
				if (strchr(field, 'v'))
					nc->nc_flag |= NC_VISIBLE;
				if (strchr(field, 'b'))
					nc->nc_flag |= NC_BROADCAST;
				continue;
			case 3:	/* protofamily field */
				if (strcmp(field, "-") == 0)
					continue;
				nc->nc_protofmly = field;
				continue;
			case 4:	/* protocol field */
				if (strcmp(field, "-") == 0)
					continue;
				nc->nc_proto = field;
				continue;
			case 5:	/* device field */
				nc->nc_device = field;
				continue;
			case 6:	/* nametoaddrlibs field */
				if (strcmp(field, "-") == 0)
					continue;
				{
					int i;
					char *lib;
					char *list;
					char *tmp2 = NULL;

					/* count them */
					for (i = 1, lib = field; (lib = strpbrk(lib, ","));
					     i++, lib++) ;
					if ((nc->nc_lookups = calloc(i, sizeof(char *))) == NULL) {
						tsd->ncerror = NC_NOMEM;
						break;
					}
					nc->nc_nlookups = i;

					for (list = field, i = 0; i < 16 &&
					     (lib = strtok_r(list, ",", &tmp2)); list = NULL, i++)
						nc->nc_lookups[i] = lib;
				}
				continue;
			}
			break;
		}
		if (fieldnum == 0) {
			if (field == NULL || field[0] == '#') {
				free(nc_line(nc));
				nc_line(nc) = NULL;
				continue;	/* skip blank or comment lines */
			}
		}
		if (fieldnum != 6)
			goto error;
		/* good entry - add it to the list */
		*nclistp = nc;
		nclistp = nc_nextp(nc);
		nc = NULL;
		entries++;
	}
	{
		struct netconfig **ncp;
		int i;

		/* create master array */
		if ((ncp = calloc(entries + 1, sizeof(struct netconfig *))) == NULL) {
			tsd->ncerror = NC_NOMEM;
			goto error;
		}
		for (nc = nclist, i = 0; i < entries; i++) {
			ncp[i] = nc;
			nc = nc_next(nc);
		}
		ncp[entries] = NULL;

		__nsl_nc_list = ncp;
		return (ncp);
	}
      nomem:
	tsd->ncerror = NC_NOMEM;
	goto error;
      openfail:
	tsd->ncerror = NC_OPENFAIL;
	goto error;
      error:
	if (nc != NULL) {
		*nclistp = nc;
		nclistp = nc_nextp(nc);
		nc = NULL;
	}
	while ((nc = nclist)) {
		nclist = nc_next(nc);
		if (nc_line(nc) != NULL)
			free(nc_line(nc));
		if (nc->nc_lookups != NULL)
			free(nc->nc_lookups);
		free(nc);
	}
	if (file != NULL)
		fclose(file);
	return (NULL);
}

static struct netconfig **
__nsl_getnetconfiglist(void)
{
	struct netconfig **ncp;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if ((ncp = __nsl_nc_list) != NULL) {
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (ncp);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	pthread_rwlock_wrlock(&__nsl_nc_lock);
	if ((ncp = __nsl_nc_list) != NULL) {
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (ncp);
	}
	ncp = __nsl_loadnetconfiglist();
	pthread_rwlock_unlock(&__nsl_nc_lock);
	return (ncp);
}

/**
 * @fn void *setnetconfig(void)
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
	struct netconfig **ncp;
	struct netconf_handle *nh = NULL;

	if ((ncp = __nsl_getnetconfiglist()) == NULL)
		return (NULL);
	if ((nh = malloc(sizeof(*nh))) == NULL) {
		nc_error = NC_NOMEM;
		return (NULL);
	}
	pthread_rwlock_init(&nh->nh_lock, NULL);
	nh->nh_head = ncp;
	nh->nh_curr = ncp;
	nc_error = NC_NOERROR;
	return ((void *) nh);
}

void *setnetconfig(void)
    __attribute__ ((weak, alias("__nsl_setnetconfig")));

/**
 * @fn struct netconfig *getnetconfig(void *handle)
 * @param handle handle returned by setnetconfig().
 * @brief Retrieve next entry in the netconfig database.
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
	struct netconf_handle *nh = (struct netconf_handle *) handle;
	struct netconfig *nc;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if (__nsl_nc_list == NULL || nh == NULL) {
		nc_error = NC_NOSET;
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (NULL);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	pthread_rwlock_wrlock(&nh->nh_lock);
	if (!(nc = *(nh->nh_curr))) {
		nc_error = NC_NOMOREENTRIES;
		pthread_rwlock_unlock(&nh->nh_lock);
		return (NULL);
	}
	nh->nh_curr++;
	nc_error = NC_NOERROR;
	pthread_rwlock_unlock(&nh->nh_lock);
	return (nc);
}

struct netconfig *getnetconfig(void *handle)
    __attribute__ ((weak, alias("__nsl_getnetconfig")));

/**
 * @fn int endnetconfig(void *handle)
 * @param handle handle returned by setnetconfig().
 * @brief Release network configuration database.
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
	struct netconf_handle *nh = (struct netconf_handle *) handle;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if (__nsl_nc_list == NULL || nh == NULL) {
		nc_error = NC_NOSET;
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (-1);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	/* strange thing to do, no? */
	pthread_rwlock_wrlock(&nh->nh_lock);
	free(nh);
	nc_error = NC_NOERROR;
	return (0);
}

int endnetconfig(void *handle)
    __attribute__ ((weak, alias("__nsl_endnetconfig")));

/**
 * @fn struct netconfig *getnetconfigent(const char *netid)
 * @param netid the network id.
 * @brief Return a network configuration entry for a network id.
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
	struct netconfig **ncp, *nc_new = NULL;

	if ((ncp = __nsl_getnetconfiglist())) {
		nc_error = NC_NOTFOUND;
		for (; *ncp; ++ncp) {
			if (strcmp((*ncp)->nc_netid, netid) == 0) {
				if ((nc_new = malloc(sizeof(*nc_new))) == NULL) {
					nc_error = NC_NOMEM;
					break;
				}
				memset(nc_new, 0, sizeof(*nc_new));
				if ((nc_line(nc_new) = strdup(nc_line(*ncp))) == NULL) {
					free(nc_new);
					nc_error = NC_NOMEM;
					break;
				}
				if ((*ncp)->nc_nlookups > 0) {
					int i;

					nc_new->nc_nlookups = (*ncp)->nc_nlookups;
					nc_new->nc_lookups =
					    calloc((*ncp)->nc_nlookups, sizeof(char *));
					if (nc_new->nc_lookups == NULL) {
						free(nc_line(nc_new));
						free(nc_new);
						nc_error = NC_NOMEM;
						break;
					}
					memcpy(nc_new->nc_lookups, (*ncp)->nc_lookups,
					       (*ncp)->nc_nlookups * sizeof(char *));
					/* reindex everything into the new line */
					nc_new->nc_netid =
					    (*ncp)->nc_netid - nc_line(*ncp) + nc_line(nc_new);
					nc_new->nc_protofmly =
					    (*ncp)->nc_protofmly - nc_line(*ncp) + nc_line(nc_new);
					nc_new->nc_proto =
					    (*ncp)->nc_proto - nc_line(*ncp) + nc_line(nc_new);
					nc_new->nc_device =
					    (*ncp)->nc_device - nc_line(*ncp) + nc_line(nc_new);
					for (i = 0; i < (*ncp)->nc_nlookups; i++)
						nc_new->nc_lookups[i] =
						    (*ncp)->nc_lookups[i] - nc_line(*ncp) +
						    nc_line(nc_new);
				}
				nc_error = NC_NOERROR;
				break;
			}
		}
	}
	return (nc_new);
}

struct netconfig *getnetconfigent(const char *netid)
    __attribute__ ((weak, alias("__nsl_getnetconfigent")));

/**
 * @fn void freenetconfigent(struct netconfig *nc)
 * @param nc the database entry to free.
 * @brief Free a netconfig database entry.
 *
 * This function frees the netconfig structure pointed to by nc (previously
 * returned by getneconfigent()).
 *
 * Upon success, this function returns a pointer to the struct netconfig
 * structure corresponding to the netid; otherwise, it returns NULL.
 */
void
__nsl_freenetconfigent(struct netconfig *nc)
{
	if (nc_line(nc))
		free(nc_line(nc));
	if (nc->nc_lookups)
		free(nc->nc_lookups);
	free(nc);
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
 * @fn char *nc_sperror(void)
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
 * @fn void nc_perror(const char *msg)
 * @param msg message to prefix to error message.
 * @brief Print an error message to standard output.
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
 * @fn void *setnetpath(void)
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
	struct netconfig **ncp, **ncplist, **ncpath = NULL, **ncpath_new;
	struct netconf_handle *nh = NULL;
	char *path;
	int entries = 0;

	if ((ncplist = __nsl_getnetconfiglist()) == NULL)
		return (NULL);
	if ((path = getenv(NETPATH)) == NULL) {
		for (ncp = ncplist; *ncp; ncp++) {
			if ((*ncp)->nc_flag & NC_VISIBLE) {
				if (!
				    (ncpath_new = realloc(ncpath, (entries + 1) * sizeof(*ncpath))))
					goto nomem;
				ncpath = ncpath_new;
				ncpath[entries++] = *ncp;
			}
		}
	} else {
		char *str, *token, *tmp = NULL;

		if ((path = strdup(path)) == NULL)
			goto nomem;
		for (str = path; (token = strtok_r(str, ":", &tmp)); str = NULL) {
			for (ncp = ncplist; *ncp; ncp++) {
				if (strcmp((*ncp)->nc_netid, token) == 0) {
					if (!(ncpath_new =
					      realloc(ncpath, (entries + 1) * sizeof(*ncpath))))
						goto nomem;
					ncpath = ncpath_new;
					ncpath[entries++] = *ncp;
					break;
				}
			}
		}
	}
	if (entries == 0)
		goto notfound;
	if (!(ncpath_new = realloc(ncpath, (entries + 1) * sizeof(*ncpath))))
		goto nomem;
	ncpath = ncpath_new;
	ncpath[entries++] = NULL;
	if ((nh = malloc(sizeof(*nh))) == NULL)
		goto nomem;
	memset(nh, 0, sizeof(*nh));
	pthread_rwlock_init(&nh->nh_lock, NULL);
	nh->nh_head = ncpath;
	nh->nh_curr = ncpath;
	nc_error = NC_NOERROR;
	return ((void *) nh);

      notfound:
	nc_error = NC_NOTFOUND;
	goto error;
      nomem:
	nc_error = NC_NOMEM;
	goto error;
      error:
	if (path)
		free(path);
	if (ncpath)
		free(ncpath);
	if (nh)
		free(nh);
	return (NULL);

}

void *setnetpath(void)
    __attribute__ ((weak, alias("__nsl_setnetpath")));

/**
 * @fn struct netconfig *getnetpath(void *handle)
 * @param handle a pointer to the handle returned by setnetpath().
 * @brief Get the next entry associated with handle.
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
	return __nsl_getnetconfig(handle);
}

struct netconfig *getnetpath(void *handle)
    __attribute__ ((weak, alias("__nsl_getnetpath")));

/**
 * @fn int endnetpath(void *handle)
 * @param handle a pointer to the handle returned by setnetpath().
 * @brief Free netpath resources associated with handle.
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
	struct netconf_handle *nh = (struct netconf_handle *) handle;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if (__nsl_nc_list == NULL || nh == NULL) {
		nc_error = NC_NOSET;
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (-1);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	/* strange thing to do, no? */
	pthread_rwlock_wrlock(&nh->nh_lock);
	free(nh->nh_head);
	free(nh);
	nc_error = NC_NOERROR;
	return (0);
}

int endnetpath(void *handle)
    __attribute__ ((weak, alias("__nsl_endnetpath")));

/*
 *  Name-to-Address Translation Functions:
 *  ======================================
 */

static pthread_rwlock_t __nsl_xlate_lock = PTHREAD_RWLOCK_INITIALIZER;

struct __nsl_xlate {
	struct __nsl_xlate *next;	/* next in list */
	char *path;			/* full path to library */
	nd_gbn_t gbn;			/* _netdir_getbyname */
	nd_gba_t gba;			/* _netdir_getbyaddr */
	nd_opt_t opt;			/* _netdir_options */
	nd_t2u_t t2u;			/* _taddr2uaddr */
	nd_u2t_t u2t;			/* _uaddr2taddr */
	void *lib;			/* dlopen pointer */
} *__nsl_xlate_list = NULL;

#pragma weak __inet_netdir_getbyname
#pragma weak __inet_netdir_getbyaddr
#pragma weak __inet_netdir_options
#pragma weak __inet_taddr2uaddr
#pragma weak __inet_uaddr2taddr
#pragma weak __inet_netdir_mergeaddr

/*
 *  A little explanation is in order here.  SVR4 provides NSS defaults for inet
 *  if name to address translation libraries are not specified.  The functions
 *  below accomplish this.  However, they are here defined as weak undefined
 *  symbols and they only come into effect if the libn2a_inet library is linked.
 *  Also, as the libn2a_inet library also defines the netdir symbols, this
 *  library can also be specified as a name-to-address translation library in
 *  netconfig.  libn2a_inet is provided separately by the strinet package.
 */
extern struct nd_addrlist *__inet_netdir_getbyname(struct netconfig *nc,
						   struct nd_hostserv *service);
extern struct nd_hostservlist *__inet_netdir_getbyaddr(struct netconfig *nc, struct netbuf *addr);
extern int __inet_netdir_options(struct netconfig *nc, int option, int fd, char *pta);
extern char *__inet_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr);
extern struct netbuf *__inet_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr);
extern char *__inet_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr);

/**
 * @fn static struct __nsl_xlate *__nsl_load_xlate(const char *name)
 * @param name the name of the library to load.
 * @brief Load a name-to-address translation library.
 */
static struct __nsl_xlate *
__nsl_load_xlate(const char *name)
{
	struct __nsl_xlate *xl;

	pthread_rwlock_wrlock(&__nsl_xlate_lock);
	for (xl = __nsl_xlate_list; xl; xl = xl->next)
		if (strcmp(name, xl->path) == 0)
			goto found;
	if ((xl = malloc(sizeof(*xl))) == NULL)
		goto nomem;
	memset(xl, 0, sizeof(*xl));
	if ((xl->path = strdup(name)) == NULL)
		goto nomem;
	if ((xl->lib = dlopen(name, RTLD_LAZY)) == NULL)
		goto open;
	/* resolve all symbols from the n2a library */
	if ((xl->gbn = (nd_gbn_t) dlsym(xl->lib, "_netdir_getbyname")) == NULL)
		goto nosym;
	if ((xl->gba = (nd_gba_t) dlsym(xl->lib, "_netdir_getbyaddr")) == NULL)
		goto nosym;
	if ((xl->opt = (nd_opt_t) dlsym(xl->lib, "_netdir_options")) == NULL)
		goto nosym;
	if ((xl->t2u = (nd_t2u_t) dlsym(xl->lib, "_netdir_taddr2uaddr")) == NULL)
		goto nosym;
	if ((xl->u2t = (nd_u2t_t) dlsym(xl->lib, "_netdir_uaddr2taddr")) == NULL)
		goto nosym;
	xl->next = __nsl_xlate_list;
	__nsl_xlate_list = xl;
	goto found;
      nosym:
	_nderror = ND_NOSYM;
	goto error;
      open:
	_nderror = ND_OPEN;
	goto error;
      nomem:
	_nderror = ND_NOMEM;
	goto error;
      error:
	if (xl != NULL) {
		if (xl->lib != NULL)
			dlclose(xl->lib);
		if (xl->path != NULL)
			free(xl->path);
		free(xl);
		xl = NULL;
	}
      found:
	pthread_rwlock_unlock(&__nsl_xlate_lock);
	return (xl);
}

/**
 * @internal Deep free a netbuf structure.
 * @param b the netbuf structure to free.
 */
static void
__freenetbuf(struct netbuf *b)
{
	if (b) {
		if (b->buf)
			free(b->buf);
		free(b);
	}
}

/**
 * @internal Deep free an nd_addrlist structure.
 * @param n the nd_addrlist structure to free.
 */
static void
__freeaddrlist(struct nd_addrlist *n)
{
	if (n) {
		int i;
		struct netbuf *b;

		if ((b = n->n_addrs)) {
			for (i = 0; i < n->n_cnt; i++, b++) {
				if (b->buf)
					free(b->buf);
			}
			free(n->n_addrs);
		}
		free(n);
	}
}

/**
 * @internal Deep free an nd_hostserv structure.
 * @param n the nd_hostserv structure to free.
 */
static void
__freehostserv(struct nd_hostserv *h)
{
	if (h) {
		if (h->h_name)
			free(h->h_name);
		if (h->h_serv)
			free(h->h_serv);
		free(h);
	}
}

/**
 * @internal Deep free an nd_hostservlist structure.
 * @param n the nd_hostservlist structure to free.
 */
static void
__freehostservlist(struct nd_hostservlist *hl)
{
	if (hl) {
		int i;
		struct nd_hostserv *h;

		if ((h = hl->h_hostservs)) {
			for (i = 0; i < hl->h_cnt; i++, h++) {
				if (h->h_name)
					free(h->h_name);
				if (h->h_serv)
					free(h->h_serv);
			}
			free(hl->h_hostservs);
		}
		free(hl);
	}
}

/**
 * @fn static struct __nsl_xlate * __nsl_lookup_xlate(const char *name)
 * @param name the name of the library to look up.
 * @brief Lookup a name-to-address translation library.
 *
 * This function looks up the name-to-address translation library under read
 * lock.  Libraries, once loaded are never unloaded so this will work.
 */
static struct __nsl_xlate *
__nsl_lookup_xlate(const char *name)
{
	struct __nsl_xlate *xl;

	pthread_rwlock_rdlock(&__nsl_xlate_lock);
	for (xl = __nsl_xlate_list; xl; xl = xl->next)
		if (strcmp(name, xl->path) == 0)
			break;
	pthread_rwlock_unlock(&__nsl_xlate_lock);
	if (xl != NULL)
		return (xl);
	return (__nsl_load_xlate(name));
}

/**
 * @fn int netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service, struct nd_addrlist **addrs)
 * @param nc pointer to transport configuration data structure.
 * @param service pointer to nd_hostserv structure identifying machine and service name.
 * @param addrs returned pointer to allocated nd_addrlist representing the resulting transport addresses.
 * @brief Map machine and service name to transport addresses.
 *
 * This function converts the machine name and service name in the nd_hostserv
 * structure to a collection of addresses of the type understood by the
 * transport identified in the netconfig structure.
 */
int
__nsl_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service,
		       struct nd_addrlist **addrs)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_netdir_getbyname) {
		return ((*addrs = __inet_netdir_getbyname(nc, service)) ? 0 : -1);
	} else {
		struct __nsl_xlate *xl;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (*addrs = (*xl->gbn) (nc, service)))
				return (0);
			if (_nderror < 0)
				return (-1);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (-1);
}

int netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service, struct nd_addrlist **addrs)
    __attribute__ ((weak, alias("__nsl_netdir_getbyname")));

/**
 * @fn int netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service, struct netbuf *netaddr)
 * @param nc pointer to transport configuration data structure.
 * @param service returned pointer to a nd_hostservlist structure identifying the machine and service names.
 * @param netaddr pointer to a netbuf structure describing the transport address.
 * @brief Map transport address to a collection of machine and service names.
 *
 * This function maps addresses to service names.  The function returns a
 * service, a list of host and service pairs that yeild these addresses.  If
 * more than one tuple of host and service name is returned, the first type
 * contains the preferred host and service names.
 */
int
__nsl_netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service,
		       struct netbuf *netaddr)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_lookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_netdir_getbyaddr) {
		return ((*service = __inet_netdir_getbyaddr(nc, netaddr)) ? 0 : -1);
	} else {
		struct __nsl_xlate *xl;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (*service = (*xl->gba) (nc, netaddr)) != NULL)
				return (0);
			if (_nderror < 0)
				return (-1);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (-1);
}

int netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service, struct netbuf *netaddr)
    __attribute__ ((weak, alias("__nsl_netdir_getbyaddr")));

/**
 * @fn void netdir_free(void *ptr, int struct_type)
 * @param ptr pointer to the structure to free.
 * @param struct_type type of structure.
 * @brief frees a structure returned by other netdir functions.
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
		return __freenetbuf((struct netbuf *) ptr);
	case ND_ADDRLIST:
		return __freeaddrlist((struct nd_addrlist *) ptr);
	case ND_HOSTSERV:
		return __freehostserv((struct nd_hostserv *) ptr);
	case ND_HOSTSERVLIST:
		return __freehostservlist((struct nd_hostservlist *) ptr);
	default:
		_nderror = ND_UKNWN;
		break;
	}
	return;
}

void netdir_free(void *ptr, int struct_type)
    __attribute__ ((weak, alias("__nsl_netdir_free")));

/**
 * @fn int netdir_options(struct netconfig *nc, int option, int fd, char *pta)
 * @param nc pointer to transport configuration structure.
 * @param universal option option to manage.
 * @param fd transport endpoint file descriptor.
 * @param pta arguments to set.
 * @brief manage universal transport options.
 *
 * This function is used to do all transport specific setups and option
 * management.  fd is the associated file descriptor.  option, fd, and
 * pointer_to_args are passed to the netdir_options() function for the
 * transport specified in nc.
 */
int
__nsl_netdir_options(struct netconfig *nc, int option, int fd, char *pta)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_netdir_options) {
		return (__inet_netdir_options(nc, option, fd, pta));
	} else {
		struct __nsl_xlate *xl;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (*xl->opt) (nc, option, fd, pta) == 0)
				return (0);
			if (_nderror < 0)
				return (-1);
		}
		_nderror = ND_FAILCTRL;
	}
	return (-1);
}

int netdir_options(struct netconfig *nc, int option, int fd, char *pta)
    __attribute__ ((weak, alias("__nsl_netdir_options")));

/**
 * @fn char *taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
 * @param nc a pointer to the transport configuration data structure.
 * @param taddr the transport address to convert.
 * @brief Converts a transport address to a universal address.
 *
 * This function supports transaltion between universal addresses and TLI type
 * netbufs.  The taddr2uaddr() function takes a struct netbuf data structure
 * and returns a pointer to a string that contains the universal address.  It
 * returns NULL if the conversion is not possible.  This is not a fatal
 * condition as some transports do not support a universal address form.
 */
char *
__nsl_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_taddr2uaddr) {
		return (__inet_taddr2uaddr(nc, taddr));
	} else {
		struct __nsl_xlate *xl;
		char *uaddr;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (uaddr = (*xl->t2u) (nc, taddr)))
				return (uaddr);
			if (_nderror < 0)
				return (NULL);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (NULL);
}

char *taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
    __attribute__ ((weak, alias("__nsl_taddr2uaddr")));

/**
 * @fn struct netbuf *uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
 * @param nc a pointer to the transport configuration data structure.
 * @param uaddr the universal address to convert.
 * @brief Converts a universal address to a transport address.
 *
 * This function is the reverse of the taddr2uaddr() function. It returns the
 * struct netbuf data structure for the given universal address.
 *
 * It is wierd that noone documents what happens if there are no
 * name-to-address translation libraries and this function is called.
 */
struct netbuf *
__nsl_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_uaddr2taddr) {
		return (__inet_uaddr2taddr(nc, uaddr));
	} else {
		struct __nsl_xlate *xl = NULL;
		struct netbuf *taddr;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (taddr = (*xl->u2t) (nc, uaddr)))
				return (taddr);
			if (_nderror < 0)
				return (NULL);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (NULL);
}

struct netbuf *uaddr2taddr(struct netconfig *nc, struct netbuf *addr)
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
 * @fn char *netdir_sperror(void)
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
 * @fn void netdir_perror(char *msg)
 * @param msg prefix string.
 * @brief Print a netdir function error string to standard output.
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

/*
 *  Name-to-Address Translation Function defaults for inet:
 *  =======================================================
 */

/**
 * @param nc transport.
 * @param h host and service name.
 * @brief Inet _netdir_getbyname lookup function.
 *
 * Converts a host and service name into an address list.  This function uses
 * the newer getaddrinfo(3) AF_INET lookup call.  It is fairly simplistic.
 */
struct nd_addrlist *
__inet_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *h)
{
	struct addrinfo hints, *res = NULL, *ai;
	struct nd_addrlist *n = NULL;
	struct netbuf *b;
	int cnt;

	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;

	if (getaddrinfo(h->h_name, h->h_serv, &hints, &res) == -1) {
		_nderror = ND_SYSTEM;
		return (NULL);
	}
	/* count them */
	for (cnt = 0, ai = res; ai; cnt++, ai = ai->ai_next) ;
	if (cnt == 0) {
		_nderror = ND_NOHOST;
		return (NULL);
	}
	if ((n = malloc(sizeof(*n))) == NULL)
		goto nomem;
	if ((n->n_addrs = calloc(cnt, sizeof(struct netbuf))) == NULL)
		goto nomem;
	memset(n->n_addrs, 0, cnt * sizeof(struct netbuf));
	for (b = n->n_addrs, n->n_cnt = 0, ai = res; n->n_cnt < cnt;
	     b++, n->n_cnt++, ai = ai->ai_next) {
		if (ai->ai_addr && ai->ai_addrlen > 0) {
			if ((b->buf = malloc(ai->ai_addrlen)) == NULL)
				goto nomem;
			memcpy(b->buf, ai->ai_addr, ai->ai_addrlen);
			b->len = b->maxlen = ai->ai_addrlen;
		}
	}
	freeaddrinfo(res);
	return (n);
      nomem:
	_nderror = ND_NOMEM;
	goto error;
      error:
	netdir_free(n, ND_ADDRLIST);
	freeaddrinfo(res);
	return (NULL);
}

/**
 * @param nc transport.
 * @param addr address to lookup.
 * @brief Inet _netdir_getbyaddr lookup function.
 */
struct nd_hostservlist *
__inet_netdir_getbyaddr(struct netconfig *nc, struct netbuf *addr)
{
	char hbuf[128];
	char sbuf[128];
	char *host = NULL;
	char *serv = NULL;
	char **aliasp, **aliases = NULL;
	struct sockaddr_in *sin;
	struct servent *s;
	struct hostent *h;
	struct nd_hostservlist *hl = NULL;
	struct nd_hostserv *hs;
	int cnt;

	if (addr == NULL || addr->buf == NULL || addr->len == 0) {
		_nderror = ND_BADARG;
		return (NULL);
	}
	if (addr->len < sizeof(*sin)) {
		_nderror = ND_NO_ADDRESS;
		return (NULL);
	}
	sin = (struct sockaddr_in *) addr->buf;
	if (sin->sin_family != AF_INET && sin->sin_family != 0) {
		_nderror = ND_NO_ADDRESS;
		return (NULL);
	}
	if ((s = getservbyport(sin->sin_port, nc->nc_proto)) == NULL) {
		uint16_t in_port = ntohs(sin->sin_port);

		sprintf(sbuf, "%hd", in_port);
		serv = sbuf;
	} else {
		serv = s->s_name;
	}
	if ((h = gethostbyaddr((char *) &sin->sin_addr.s_addr, 4, AF_INET)) == NULL) {
		uint32_t in_addr = ntohl(sin->sin_addr.s_addr);

		sprintf(hbuf, "%d.%d.%d.%d", (in_addr >> 24) & 0xff, (in_addr >> 16) & 0xff,
			(in_addr >> 8) & 0xff, (in_addr >> 0) & 0xff);
		host = hbuf;
	} else {
		host = h->h_name;
		aliases = h->h_aliases;
	}
	/* count 'em all */
	for (cnt = 1, aliasp = h->h_aliases; *aliasp; cnt++, aliasp++) ;
	if ((hl = malloc(sizeof(*hl))) == NULL)
		goto nomem;
	memset(hl, 0, sizeof(*hl));
	if ((hs = hl->h_hostservs = calloc(cnt, sizeof(struct nd_hostserv))) == NULL)
		goto nomem;
	memset(hs, 0, cnt * sizeof(*hs));
	hl->h_cnt = 1;
	if ((hs->h_name = strdup(host)) == NULL)
		goto nomem;
	if ((hs->h_serv = strdup(serv)) == NULL)
		goto nomem;
	hs++;
	hl->h_cnt++;
	aliasp = h->h_aliases;
	for (; *aliasp; aliasp++, hs++, hl->h_cnt++) {
		if ((hs->h_name = strdup(*aliasp)) == NULL)
			goto nomem;
		if ((hs->h_serv = strdup(serv)) == NULL)
			goto nomem;
	}
	return (hl);
      nomem:
	_nderror = ND_NOMEM;
	goto error;
      error:
	netdir_free(hl, ND_HOSTSERVLIST);
	return (NULL);
}

static int __setoption(int fd, t_scalar_t level, t_scalar_t name, t_scalar_t value);
static int __setresvport(int fd, struct netbuf *addr);
static int __checkresvport(struct netbuf *addr);
static int __joinmulticast(int fd, struct netbuf *addr);
static int __leavemulticast(int fd, struct netbuf *addr);

int
__inet_netdir_options(struct netconfig *nc, int option, int fd, char *pta)
{
	switch (option) {
	case ND_SET_BROADCAST:
		return (__setoption(fd, T_INET_IP, T_IP_BROADCAST, T_YES));
	case ND_CLEAR_BROADCAST:
		return (__setoption(fd, T_INET_IP, T_IP_BROADCAST, T_NO));
	case ND_SET_REUSEADDR:
		return (__setoption(fd, T_INET_IP, T_IP_REUSEADDR, T_YES));
	case ND_CLEAR_REUSEADDR:
		return (__setoption(fd, T_INET_IP, T_IP_REUSEADDR, T_NO));
	case ND_SET_RESERVEDPORT:
		return (__setresvport(fd, (struct netbuf *) pta));
	case ND_CHECK_RESERVEDPORT:
		return (__checkresvport((struct netbuf *) pta));
	case ND_MERGEADDR:
	{
		struct nd_mergearg *m = (typeof(m)) (pta);

		if ((m->m_uaddr = __inet_netdir_mergeaddr(nc, m->c_uaddr, m->s_uaddr)))
			return (0);
		return (-1);
	}
	case ND_JOIN_MULTICAST:
		return (__joinmulticast(fd, (struct netbuf *) pta));
	case ND_LEAVE_MULTICAST:
		return (__leavemulticast(fd, (struct netbuf *) pta));
	default:
		_nderror = ND_NOCTRL;
		return (-1);
	}
}

static int
__setoption(int fd, t_scalar_t level, t_scalar_t name, t_scalar_t value)
{
	int state;
	struct t_optmgmt req, ret;
	struct {
		struct t_opthdr hdr;
		t_scalar_t value;
	} optbuf;

	_nderror = ND_SYSTEM;
	if ((state = t_getstate(fd)) != T_UNBND) {
		if (t_errno == TBADF)
			errno = EBADF;
		if (state != -1)
			errno = EISCONN;
		return (-1);
	}
	optbuf.hdr.len = sizeof(optbuf);
	optbuf.hdr.level = level;
	optbuf.hdr.name = name;
	optbuf.hdr.status = 0;
	optbuf.value = value;
	req.opt.buf = (char *) &optbuf;
	req.opt.len = sizeof(optbuf);
	req.opt.maxlen = sizeof(optbuf);
	req.flags = T_NEGOTIATE;
	ret.opt.buf = NULL;
	ret.opt.len = 0;
	ret.opt.maxlen = -1;
	ret.flags = T_FAILURE;
	if (t_optmgmt(fd, &req, &ret) == -1) {
		if (t_errno == TBADF)
			errno = EBADF;
		if (t_errno == TBADFLAG)
			errno = EINVAL;
		if (t_errno == TBADOPT)
			errno = EINVAL;
		if (t_errno == TBUFOVFLW)
			errno = EINVAL;
		if (t_errno == TNOTSUPPORT)
			errno = EOPNOTSUPP;
		if (t_errno == TOUTSTATE)
			errno = EPROTO;
		if (t_errno == TPROTO)
			errno = EPROTO;
		return (-1);
	}
	if (ret.flags != T_SUCCESS) {
		_nderror = ND_FAILCTRL;
		return (-1);
	}
	// _nderror = ND_OK;
	return (0);
}

static int
__setresvport(int fd, struct netbuf *addr)
{
	/* not implemented yet */
	_nderror = ND_NOCTRL;
	return (-1);
}
static int
__checkresvport(struct netbuf *addr)
{
	struct sockaddr_in *sin;
	unsigned short port;

	if (addr == NULL || addr->buf == NULL || addr->len == 0) {
		_nderror = ND_NO_ADDRESS;
		return (-1);
	}
	if (addr->len < sizeof(*sin)) {
		_nderror = ND_BADARG;
		return (-1);
	}
	sin = (typeof(sin)) (addr->buf);
	port = ntohs(sin->sin_port);
	// _nderror = ND_OK;
	if (port < IPPORT_RESERVED)
		return (0);
	return (1);
}
static int
__joinmulticast(int fd, struct netbuf *addr)
{
	/* not implemented yet */
	_nderror = ND_NOCTRL;
	return (-1);
}
static int
__leavemulticast(int fd, struct netbuf *addr)
{
	/* not implemented yet */
	_nderror = ND_NOCTRL;
	return (-1);
}

/**
 * @param nc the transport.
 * @param taddr the transport address.
 * @brief Convert an inet transport address to a universal address.
 *
 * Convert from a trasnsport address to a universal address.  The universal
 * address is formatted as %d.%d.%d.%d.%d.%d which represents the address and
 * port number from most significant byte to lease significant byte.  Each
 * number between the dots is a byte.
 */
char *
__inet_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
{
	struct sockaddr_in *sin;
	char buf[INET_ADDRSTRLEN + 16], *ret;
	unsigned short port;
	int len;

	if (taddr == NULL || taddr->buf == NULL || taddr->len == 0) {
		_nderror = ND_BADARG;
		return (NULL);
	}
	if (taddr->len < sizeof(*sin)) {
		_nderror = ND_NO_ADDRESS;
		return (NULL);
	}
	sin = (typeof(sin)) (taddr->buf);
	port = ntohs(sin->sin_port);

	if (inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf)) == NULL) {
		_nderror = ND_SYSTEM;
		return (NULL);
	}
	len = strlen(buf);
	(void) snprintf(buf + len, sizeof(buf) - len, ".%d.%d", port >> 8, port & 0xff);
	ret = strdup(buf);
	if (ret == NULL) {
		_nderror = ND_NOMEM;
		return (NULL);
	}
	return (ret);
}

/**
 * @param nc the transport.
 * @param uaddr the universal address.
 * @brief Convert a universal address to a transport address.
 *
 * Convert from universal address format to one suitable for use with inet.
 * The universal address is %d.%d.%d.%d.%d.%d which represent the address and
 * port number from most significant byte to least significant byte.  Each
 * number between the dots is a byte.
 */
struct netbuf *
__inet_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
{
	struct netbuf *nb = NULL;
	struct sockaddr_in *sin = NULL;
	uint32_t addr = 0;
	unsigned short port = 0;
	char *beg, *end;
	int field;

	if (uaddr == NULL || uaddr->buf == NULL || uaddr->len == 0) {
		_nderror = ND_BADARG;
		return (NULL);
	}
	if (uaddr->len < sizeof(*sin)) {
		_nderror = ND_NO_ADDRESS;
		return (NULL);
	}
	if ((nb = (typeof(nb)) malloc(sizeof(*nb))) == NULL) {
		_nderror = ND_NOMEM;
		return (NULL);
	}
	if ((sin = (typeof(sin)) malloc(sizeof(*sin))) == NULL) {
		free(nb);
		_nderror = ND_NOMEM;
		return (NULL);
	}
	nb->buf = (char *) sin;
	nb->maxlen = sizeof(*sin);
	nb->len = sizeof(*sin);

	for (field = 0, beg = end = uaddr->buf;; field++, beg = end + 1) {
		ulong value;

		if (field > 5)
			break;
		value = strtoul(beg, &end, 10);
		if (beg == end || (end[0] != '.' && end[0] != '\0')) {
			field--;
			break;
		}
		if (field <= 3)
			addr |= (value & 0xff) << (24 - 8 * field);
		if (field >= 4)
			port |= (value & 0xff) << (8 - 8 * (field - 4));
		if (end[0] == '\0')
			break;
	}
	if (field != 5) {
		free(nb);
		free(sin);
		_nderror = ND_NO_ADDRESS;
		return (NULL);
	}
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(addr);
	sin->sin_port = htons(port);
	return (nb);
}

/**
 * @param nc the transport.
 * @param caddr the client universal address.
 * @param saddr the server universal address.
 * @brief Merge a server and client address into a merged address.
 *
 * When a server address is underspecified (such as 0.0.0.0.1.12) this
 * function takes a client address (e.g. 192.168.0.5.1.12) and makes the
 * merged address into a specific address at which the client can contact the
 * server (possibly 192.168.0.1.1.12 in this case).
 */
char *
__inet_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr)
{
#if 0
	/* working on it */
	struct netbuf cbuf, sbuf, mbuf, *cadd = NULL, *sadd = NULL, *madd = NULL;
	struct sockaddr_in *csin, *ssin, *msin;
	char *maddr = NULL;

	sbuf.buf = saddr;
	sbuf.len = sbuf.maxlen = strlen(saddr);
	if (!(sadd = __inet_uaddr2taddr(nc, &sbuf)))
		goto error;
	ssin = (typeof(ssin)) sadd->buf;
	if (ssin->sin_addr.s_addr != INADDR_ANY) {
		if ((maddr = strdup(saddr)) == NULL)
			goto nomem;
		goto done;
	}

	cbuf.buf = caddr;
	cbuf.len = cbuf.maxlen = strlen(caddr);
	if (!(cadd = __inet_uaddr2taddr(nc, &cbuf)))
		goto error;
	csin = (typeof(csin)) cadd->buf;
	if (ssin->sin_addr.s_addr == INADDR_ANY) {
		if ((maddr = strdup(saddr)) == NULL)
			goto nomem;
		goto done;
	}

	mbuf.buf = saddr;
	mbuf.len = mbuf.maxlen = strlen(saddr);
	if (!(madd = __inet_uaddr2taddr(nc, &mbuf)))
		goto error;
	msin = (typeof(msin)) madd->buf;

	/* Need to find the local interface address that is closest to the client address and fill
	   it out in msin->sin_addr.s_addr. */

	if (!(maddr = __inet_taddr2uaddr(nc, madd)))
		goto error;
	goto done;

      nomem:
	_nderror = ND_NOMEM;
	goto error;
      error:
	if (maddr != NULL) {
		free(maddr);
		maddr == NULL;
	}
      done:
	netdir_free(cadd, ND_ADDR);
	netdir_free(sadd, ND_ADDR);
	netdir_free(madd, ND_ADDR);
	return (maddr);
#endif
	/* not implemented yet */
	_nderror = ND_NOCTRL;
	return (NULL);
}

/**
 * @section Identification
 * This development manual was written for the OpenSS7 NSL Library version \$Name:  $(\$Revision: 0.9.2.3 $).
 * @author Brian F. G. Bidulock
 * @version \$Name:  $(\$Revision: 0.9.2.3 $)
 * @date \$Date: 2007/08/14 04:56:59 $
 *
 * @}
 */

/*
 * vim: comments+=b\:TRANS
 */
