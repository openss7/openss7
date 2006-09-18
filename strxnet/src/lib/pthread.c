/*****************************************************************************

 @(#) $RCSfile: pthread.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/18 00:03:15 $

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

 Last Modified $Date: 2006/09/18 00:03:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: pthread.c,v $
 Revision 0.9.2.1  2006/09/18 00:03:15  brian
 - added libxnsl source files

 *****************************************************************************/

#ident "@(#) $RCSfile: pthread.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/18 00:03:15 $"

static char const ident[] = "$RCSfile: pthread.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/18 00:03:15 $";

/* This file can be processed with doxygen(1). */

#include <limits.h>
#include <pthread.h>

/*
 *  This is a lightweight pthreads replacement for the non-recursive environment.  This module
 *  defines a number of POSIX thread functions that are used by library code.  They are defined as
 *  weak aliases.  This file provides replacement dummy functions for POSIX thread functions to be
 *  used when there is no need for thread-safety.  When the libpthread library is loaded against a
 *  library containing this package, the POSIX thread definitions from libpthread will be used
 *  instead.  Both defined and undefined weak symbols are used so that it does not matter whether
 *  the libpthread library is loaded before or after the library containing this package.
 *
 *  The magic of weak functions is a black art.  This one works currently, don't mess with it.
 *
 *  When linked to this object, a library can pretty much use whatever cancellation mechanisms,
 *  read-write locks and thread specific data they like using the normal POSIX thread function calls
 *  as defined in in <pthread.h> and not worry about non-recursive versions.
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

