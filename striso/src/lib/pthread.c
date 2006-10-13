/*****************************************************************************

 @(#) $RCSfile: pthread.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/13 00:09:14 $

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

 Last Modified $Date: 2006/10/13 00:09:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: pthread.c,v $
 Revision 0.9.2.2  2006/10/13 00:09:14  brian
 - fixed segfault in pthread_setcanceltype

 Revision 0.9.2.1  2006/10/10 10:12:44  brian
 - added losts of striso files

 Revision 0.9.2.1  2006/09/25 12:30:57  brian
 - added files for new strnsl package

 Revision 0.9.2.4  2006/09/24 21:57:23  brian
 - documentation and library updates

 Revision 0.9.2.3  2006/09/22 20:54:27  brian
 - tweaked source file for use with doxygen

 Revision 0.9.2.2  2006/09/18 13:52:56  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:03:15  brian
 - added libxnsl source files

 *****************************************************************************/

#ident "@(#) $RCSfile: pthread.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/13 00:09:14 $"

static char const ident[] =
    "$RCSfile: pthread.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/13 00:09:14 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup pthread OpenSS7 Library Thread Safety
  * @{ */

/** @file
  * Thread-safety implementation file.
  * This file provide implementation of functions used for thread-safety as
  * well as for non-recursive versions of the library.  This file is shared by
  * a number of libraries. */

/** @page threadsafety Discussion on Thread-Safety
  *
  * This is a lightweight pthreads replacement for the non-recursive
  * environment.  This module defines a number of POSIX thread functions that in
  * the implementations of the second set.  are used by library code.  They are
  * defined as weak aliases.  This file provides replacement dummy functions for
  * POSIX thread functions to be used when there is no need for thread-safety.
  * When the libpthread library is loaded against a library containing this
  * package, the POSIX thread definitions from libpthread will be used instead.
  * Both defined and undefined weak symbols are used so that it does not matter
  * whether the libpthread library is loaded before or after the library
  * containing this package.
  *
  * The magic of weak functions is a black art.  This one works currently, don't
  * mess with it.
  *
  * When linked to this object, a library can pretty much use whatever
  * cancellation mechanisms, read-write locks and thread specific data they like
  * using the normal POSIX thread function calls as defined in in <pthread.h>
  * and not worry about non-recursive versions.  */
/** @todo What really needs to be done for these functions is to also set a
  * pthread_atfork() routine to reinitialize all static areas used by the
  * library when fork(2) is called. */

#include <limits.h>
#include <errno.h>
#include <pthread.h>

#ifdef DEBUG
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#endif

/** @name Asynchronous Thread Cancellation Functions
  * @brief Functions for asynchronous thread cancellation.
  *
  * These functions are declared in <pthread.h>, but are also declared here as
  * verification against the system headers.  These symbols are declared as weak
  * undefined symbols in glibc and provide thread safety capabilities to glibc
  * from an add-on pthread library.  They are used in the OpenSS7 libraries to
  * detect whether a suitable pthread library is present (loaded).
  *
  * The first set of symbols: __pthread_cleanup_push(), __pthread_cleanup_pop(),
  * __pthread_cleanup_push_defer(), __pthread_cleanup_pop_restore() and
  * __pthread_testcancel(), are weak @e undefined symbols in the glibc library.
  * These symbols are provided when libpthread loads before glibc.
  *
  * The second set of symbols: _pthread_cleanup_push(), _pthread_cleanup_pop(),
  * _pthread_cleanup_push_defer(), _pthread_cleanup_pop_restore(),
  * pthread_testcancel() and pthread_setcanceltype(), are weak @e defined
  * symbols that are overriden when libpthread loads after glibc.  The first
  * four of these are non-portable and that is why they begin with a single
  * underscore.
  *
  * The proper approach in implementation is to implement the second set as weak
  * @e defined symbols that override glibc's symbols and are still overridden by
  * libpthread strong symbols, and to test the presence of the first set in the
  * implementations of the second set.
  *
  * @{ */
#pragma weak __pthread_cleanup_push
extern void __pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
				   void *arg);
#pragma weak __pthread_cleanup_pop
extern void __pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute);

#pragma weak __pthread_cleanup_push_defer
extern void __pthread_cleanup_push_defer(struct _pthread_cleanup_buffer *buffer,
					 void (*routine) (void *), void *arg);
#pragma weak __pthread_cleanup_pop_restore
extern void __pthread_cleanup_pop_restore(struct _pthread_cleanup_buffer *buffer, int execute);

#pragma weak __pthread_testcancel
extern void __pthread_testcancel(void);

#pragma weak __pthread_setcanceltype
extern int __pthread_setcanceltype(int type, int *oldtype);

#pragma weak _pthread_cleanup_push
extern void _pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
				  void *arg);
#pragma weak _pthread_cleanup_pop
extern void _pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute);

#pragma weak _pthread_cleanup_push_defer
extern void _pthread_cleanup_push_defer(struct _pthread_cleanup_buffer *buffer,
					void (*routine) (void *), void *arg);
#pragma weak _pthread_cleanup_pop_restore
extern void _pthread_cleanup_pop_restore(struct _pthread_cleanup_buffer *buffer, int execute);

#pragma weak pthread_testcancel
extern void pthread_testcancel(void);

#pragma weak pthread_setcanceltype
extern int pthread_setcanceltype(int type, int *oldtype);

/** @} */

/** @name Read/Write Thread Locking Functions
  * @brief Functions for reader/writer thread mutual exclusion.
  *
  * These functions are declared in <pthread.h>, but are also declared here as
  * verification against the system headers.  These symbols are declared as weak
  * undefined symbols in glibc and provide thread safety capabilities to glibc
  * from an add-on pthread library.  They are used in the OpenSS7 libraries to
  * detect whether a suitable pthread library is present (loaded).
  *
  * The first set of symbols: __pthread_rwlock_init(),
  * __pthread_rwlock_rdlock(), __pthread_rwlock_wrlock(),
  * __pthread_rwlock_unlock() and __pthread_rwlock_destroy(), are weak @e
  * undefined symbols in the glibc library.  These symbols are provided when
  * libpthread loads before glibc.
  *
  * The second set of symbols: pthread_rwlock_init(), pthread_rwlock_rdlock(),
  * pthread_rwlock_wrlock(), pthread_rwlock_unlock() and
  * pthread_rwlock_destroy(), are weak @e defined symbols that are overriden
  * when libpthread loads after glibc.  The first four of these are non-portable
  * and that is why they begin with a single underscore.
  *
  * The proper approach in implementation is to implement the second set as weak
  * @e defined symbols that override glibc's symbols and are still overridden by
  * libpthread strong symbols, and to test the presence of the first set in the
  * implementations of the second set.
  *
  * @{ */
#pragma weak __pthread_rwlock_init
extern int __pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr);

#pragma weak __pthread_rwlock_rdlock
extern int __pthread_rwlock_rdlock(pthread_rwlock_t * rwlock);

#pragma weak __pthread_rwlock_wrlock
extern int __pthread_rwlock_wrlock(pthread_rwlock_t * rwlock);

#pragma weak __pthread_rwlock_unlock
extern int __pthread_rwlock_unlock(pthread_rwlock_t * rwlock);

#pragma weak __pthread_rwlock_destroy
extern int __pthread_rwlock_destroy(pthread_rwlock_t * rwlock);

#pragma weak pthread_rwlock_init
extern int pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr);

#pragma weak pthread_rwlock_rdlock
extern int pthread_rwlock_rdlock(pthread_rwlock_t * rwlock);

#pragma weak pthread_rwlock_wrlock
extern int pthread_rwlock_wrlock(pthread_rwlock_t * rwlock);

#pragma weak pthread_rwlock_unlock
extern int pthread_rwlock_unlock(pthread_rwlock_t * rwlock);

#pragma weak pthread_rwlock_destroy
extern int pthread_rwlock_destroy(pthread_rwlock_t * rwlock);

/** @} */

/** @name Thread Specific Data
  * @brief Functions for thread specific data.
  *
  * These functions are declared in <pthread.h>, but are also declared here as
  * verification against the system headers.  These symbols are declared as weak
  * undefined symbols in glibc and provide thread safety capabilities to glibc
  * from an add-on pthread library.  They are used in the OpenSS7 libraries to
  * detect whether a suitable pthread library is present (loaded).
  *
  * The first set of symbols: __pthread_once(), __pthread_key_create(),
  * __pthread_setspecific(), __pthread_getspecific() and __pthread_key_delete(),
  * are weak @e undefined symbols in the glibc library.  These symbols are
  * provided when libpthread loads before glibc.
  *
  * The second set of symbols: pthread_once(), pthread_key_create(),
  * pthread_setspecific(), pthread_getspecific() and pthread_key_delete(), are
  * weak @e defined symbols that are overriden when libpthread loads after
  * glibc.  The first four of these are non-portable and that is why they begin
  * with a single underscore.
  *
  * The proper approach in implementation is to implement the second set as weak
  * @e defined symbols that override glibc's symbols and are still overridden by
  * libpthread strong symbols, and to test the presence of the first set in the
  * implementations of the second set.
  *
  * @{ */
#pragma weak __pthread_once
extern int __pthread_once(pthread_once_t * once_control, void (*init_routine) (void));

#pragma weak __pthread_key_create
extern int __pthread_key_create(pthread_key_t * key, void (*destr_function) (void *));

#pragma weak __pthread_setspecific
extern int __pthread_setspecific(pthread_key_t key, const void *pointer);

#pragma weak __pthread_getspecific
extern void *__pthread_getspecific(pthread_key_t key);

#pragma weak __pthread_key_delete
extern int __pthread_key_delete(pthread_key_t key);

#pragma weak pthread_once
extern int pthread_once(pthread_once_t * once_control, void (*init_routine) (void));

#pragma weak pthread_key_create
extern int pthread_key_create(pthread_key_t * key, void (*destr_function) (void *));

#pragma weak pthread_setspecific
extern int pthread_setspecific(pthread_key_t key, const void *pointer);

#pragma weak pthread_getspecific
extern void *pthread_getspecific(pthread_key_t key);

#pragma weak pthread_key_delete
extern int pthread_key_delete(pthread_key_t key);

/** @} */

/** Push a cleanup function.
  * @param buffer a pointer to a stack allocated cleanup buffer.
  * @param routine the cleanup routine.
  * @param arg an argument for the cleanup routine.
  *
  * This is a replacement for the internal glibc function
  * _pthread_cleanup_push().  The function is declared in <pthread.h> as a weak
  * undefined symbol in glibc to allow the pthreads library to override the
  * function.  The struct _pthread_cleanup_buffer structure is also declared in
  * <pthread.h>.
  */
void
_pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *), void *arg)
{
	/** If the weak undefined symbol, __pthread_cleanup_push() is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_cleanup_push)
		return __pthread_cleanup_push(buffer, routine, arg);
	/** Otherwise, The function must initialize the cleanup buffer pointed
	  * to by buffer that was pushed onto the stack by glibc.  */
	buffer->__routine = routine;
	buffer->__arg = arg;
	buffer->__canceltype = 0;
	buffer->__prev = NULL;
}

/** Pop, and possibly execute, a cleanup function.
  * @param buffer a pointer to a stack allocated cleanup buffer.
  * @param execute whether to execute the cleanup function.
  *
  * This is a replacement for the internal glibc function
  * _pthread_cleanup_pop().  The function is declared in <pthread.h> as a weak
  * undefined symbol in glibc to allow the pthreads library to override the
  * function.  The struct _pthread_cleanup_buffer structure is also declared in
  * <pthread.h>.
  *
  * The function must deinitialize the cleanup buffer pointerd to by buffer
  * that will be popped from the stack by glibc.  */
void
_pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute)
{
	if (__pthread_cleanup_pop)
		return __pthread_cleanup_pop(buffer, execute);
	if (execute)
		(*buffer->__routine) (buffer->__arg);
}

/** Cleanup thread on asynchronous exit.
  * @param buffer a pointer to a stack allocated cleanup buffer.
  * @param routine the cleanup routine.
  * @param arg an argument for the cleanup routine.
  *
  * This is a replacement for the internal glibc function
  * _pthread_cleanup_push_defer().  The function is declared in <pthread.h> as a
  * weak undefined symbol in glibc to allow the pthreads library to override the
  * function.  The struct _pthread_cleanup_buffer structure is also declared in
  * <pthread.h>.
  */
void
_pthread_cleanup_push_defer(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
			    void *arg)
{
	/** If the weak undefined symbol, __pthread_cleanup_push_defer() is
	  * loaded, a suitable threads library exists, and the library symbol is
	  * called. */
	if (__pthread_cleanup_push_defer)
		return __pthread_cleanup_push_defer(buffer, routine, arg);
	/** Otherwise, The function must initialize the cleanup buffer pointed
	  * to by buffer that was pushed onto the stack by glibc as well as
	  * setting the cancellation type for the current thread to deferred and
	  * storing the previous thread cancellation state into the supplied
	  * buffer. */
	buffer->__routine = routine;
	buffer->__arg = arg;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &buffer->__canceltype);
	buffer->__prev = NULL;
}

/** Cleanup thread on asynchronous exit.
  * @param buffer a pointer to a stack allocated cleanup buffer.
  * @param execute whether to execute the cleanup function.
  *
  * This is a replacement for the internal glibc function
  * _pthread_cleanup_pop().  The function is declared in <pthread.h> as a weak
  * undefined symbol in glibc to allow the pthreads library to override the
  * function.  The struct _pthread_cleanup_buffer structure is also declared in
  * <pthread.h>. */
void
_pthread_cleanup_pop_restore(struct _pthread_cleanup_buffer *buffer, int execute)
{
	/** If the weak undefined symbol, __pthread_cleanup_pop_restore() is
	  * loaded, a suitable threads library exists, and the library symbol is
	  * called. */
	if (__pthread_cleanup_pop_restore)
		return __pthread_cleanup_pop_restore(buffer, execute);
	/** Otherwise, function must deinitialize the cleanup buffer pointerd to
	  * by buffer that will be popped from the stack by glibc, as well a
	  * setting the thread cancellation type back to the previous value from
	  * buffer.  */
	if (execute)
		(*buffer->__routine) (buffer->__arg);
	pthread_setcanceltype(buffer->__canceltype, NULL);
}

/** Test for asyncrhonous thread cancellation (while deferred).
  * This is a non-recursive replacement for the pthread library function, pthread_testcancel(3). */
void
pthread_testcancel(void)
{
	/** If the weak undefined symbol, __pthread_testcancel() is loaded, a
	  * suitable threads library exists and the library symbol is called.
	  */
	if (__pthread_testcancel)
		return __pthread_testcancel();
	/** Otherwise, for non-threaded operation, this is a noop. */
	return;
}

/** Thread cancellation type storage.
  * @internal This is a statically allocated internal storage for holding the
  * thread cancellation type when no suitable threads library is loaded
  * (non-thread-safe operation).  */
static int __pthread_canceltype = 0;

/** Set or defer thread asynchronous cancellation.
  * @param type cancellation type to set.
  * @param oldtype place to return current type.
  *
  * This is a non-recursive replacement for the pthread library function, pthread_setcanceltype(3). */
int
pthread_setcanceltype(int type, int *oldtype)
{
	/** If the weak undefined symbol, __pthread_setcanceltype() is loaded, a suitable threads
	  * library exists, and the library symbol is called.  */
	if (__pthread_setcanceltype)
		return __pthread_setcanceltype(type, oldtype);
	/** Otherwise, for non-thread operation, this function simply saves new
	  * type and returns the oldtype from a static variable.  One of the
	  * ramifications of this is that the cancel type might not be correct
	  * after a fork(2). */
	if (oldtype != NULL)
		*oldtype = __pthread_canceltype;
	__pthread_canceltype = type;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
}

/** Initialize a reader/writer lock.
  * @param rwlock the lock to initalize.
  * @param attr a pointer to lock attributes.
  *
  * This is a non-recursive replacement for the pthread library function,
  * pthread_rwlock_init(3). */
int
pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr)
{
	/** If the weak undefined symbol, __pthread_rwlock_init(), is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_rwlock_init)
		return __pthread_rwlock_init(rwlock, attr);
	/** Otherwise, for non-threaded operation, this function simply
	  * initializes the first byte of the lock structure to zero to indicate
	  * that the lock is unlocked. */
	*(char *) rwlock = 0;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
}

/** Read-lock a reader/writer lock.
  * @param rwlock the lock to read lock.
  *
  * This is a non-recursive replacement for the pthread library function,
  * pthread_rwlock_rdlock(3). */
int
pthread_rwlock_rdlock(pthread_rwlock_t * rwlock)
{
	/** If the weak undefined symbol, __pthread_rwlock_rdlock(), is loaded,
	  * a suitablel threads library exists, and the library symbol is
	  * called. */
	if (__pthread_rwlock_rdlock)
		return __pthread_rwlock_rdlock(rwlock);
	/** Otherwise, for non-threaded operation, this function simply
	  * increments the first by of the lock structure (POSIX locks are
	  * recursive in that multiple reader locks can be taken by the same
	  * thread).  */
	*(char *) rwlock = *(char *) rwlock + 1;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
}

/** Write-lock a reader/writer lock.
  * @param rwlock the lock to write lock.
  *
  * This is a non-recursive replacement for the pthread library function, pthread_rwlock_wrlock(3).
  * */
int
pthread_rwlock_wrlock(pthread_rwlock_t * rwlock)
{
	/** If the weak undefined symbol, __pthread_rwlock_wrlock(), is loaded,
	  * a suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_rwlock_wrlock)
		return __pthread_rwlock_wrlock(rwlock);
	/** Otherwise, the first byte of the lock structus is imply decremented
	  * (although POSIX locks cannot recurse on write).  Note that if the
	  * byte is not zero, then it is an errror that can be detected here.
	  * If debugging, print an error message.  */
	if (*(char *) rwlock != 0) {
		errno = EDEADLK;
#ifdef DEBUG
		perror(__FUNCTION__);
#endif
		return (errno);
	}
	*(char *) rwlock = *(char *) rwlock - 1;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
	/** @retval EDEADLK Attempting to lock would cause a deadlock.  */
}

/** Unlock a reader/writer lock.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_rwlock_unlock(3). */
int
pthread_rwlock_unlock(pthread_rwlock_t * rwlock)
{
	/** If the weak undefined symbol, __pthread_rwlock_unlock(), is loaded,
	  * a suitable threads library exists, and the library symbol is called.
	  * */
	if (__pthread_rwlock_unlock)
		return __pthread_rwlock_unlock(rwlock);
	/** Otherwise, if the value of the first byte is greater than zero (read
	  * locked), decrement the value; minus one (-1) (write locked), set it
	  * to zero.  Note that if the byte is already set to zero, the lock is
	  * unlocked.  If debugging, print an error message. */
	if (*(char *) rwlock == -2 || *(char *)rwlock == 0) {
#ifdef DEBUG
		errno = EINVAL;
		perror(__FUNCTION__);
#endif
		return (EINVAL);
	}
	if (*(char *) rwlock > 0)
		*(char *) rwlock = *(char *) rwlock - 1;
	else
		*(char *) rwlock = 0;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
}

/** Destroy a reader/writer lock.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_rwlock_destroy(3). */
int
pthread_rwlock_destroy(pthread_rwlock_t * rwlock)
{
	/** If the weak undefined symbol, __pthread_rwlock_destroy(), is loaded,
	  * a suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_rwlock_destroy)
		return __pthread_rwlock_destroy(rwlock);
	/** Otherwise, just set it to some illegal value. */
	*(char *) rwlock = -2;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
}

/** Perform a procedure once for all threads.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_once(3). */
int
pthread_once(pthread_once_t * once_control, void (*init_routine) (void))
{
	/** If the weak undefined symbol, __pthread_once(), is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_once)
		return __pthread_once(once_control, init_routine);
	/** Otherwise, if the value of the control is zero, set it to one and
	  * call the init routine. */
	if (*once_control == 0) {
		*once_control = 1;
		init_routine();
	}
	return (0);
	/** @return Always returns zero (0). */
}

/** Key retention structure. */
struct __pthread_key_data {
	int exists;	/**< True when created, false when available. */
	void *value;	/**< Value if it exists. */
};

/** Array of thread-specific-data keys. */
static struct __pthread_key_data ___pthread_keys[PTHREAD_KEYS_MAX] = { { 0, }, };

/** Create a thread-specific data key.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_key_create(3). */
int
pthread_key_create(pthread_key_t * key, void (*destr_function) (void *))
{
	int i;

	/** If the weak undefined symbol, __pthread_key_create(), is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_key_create)
		return __pthread_key_create(key, destr_function);
	/** Otherwise, allocate a new key from the statically allocated keys
	  * structure and null its value. */
	for (i = 0; i < PTHREAD_KEYS_MAX; i++)
		if (!___pthread_keys[i].exists)
			break;
	if (i == PTHREAD_KEYS_MAX)
		return (ENOMEM);
	___pthread_keys[i].exists = 1;
	___pthread_keys[i].value = NULL;
	*key = i;
	return (0);
	/** @return Returns zero (0) on success, error number on failure.  On
	  * success, the key value is stored in the area pointed to by argument
	  * key. */
	/** @retval ENOMEM No keys left. */
}

/** Set a thread-specific data value.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_setspecific(3). */
int
pthread_setspecific(pthread_key_t key, const void *pointer)
{
	/** If the weak undefined symbol, __pthread_setspecific(), is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_setspecific)
		return __pthread_setspecific(key, pointer);
	/** Otherwise, set the value in the statically allocated array.  */
	if (key < 0 || key >= PTHREAD_KEYS_MAX || !___pthread_keys[key].exists)
		return (EINVAL);
	___pthread_keys[key].value = (void *) pointer;
	return (0);
	/** @return Returns zero (0) on success, error number on
	  * failure.  On success, the thread specific data pointer
	  * specified by argument key is set to the value of argument
	  * pointer. */
	/** @retval EINVAL Invalid key argument. */
}

/** Set a thread-specific data value.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_getspecific(3). */
void *
pthread_getspecific(pthread_key_t key)
{
	/** If the weak undefined symbol, __pthread_getspecific(), is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_getspecific)
		return __pthread_getspecific(key);
	/** Otherwise, get the value from the statically allocated array.  */
	if (key < 0 || key >= PTHREAD_KEYS_MAX || !___pthread_keys[key].exists) {
		errno = EINVAL;
		return (NULL);
	}
	return ___pthread_keys[key].value;
	/** @return On success, returns the thread specific data associated with
	  * argument key; on failure, returns NULL. */
	/** @retval EINVAL Invalid key argument. */
}

/** Delete a thread-specific data key.
  * This is a non-recursive replacement for the pthread library function,
  * pthread_key_delete(3). */
int
pthread_key_delete(pthread_key_t key)
{
	/** If the weak undefined symbol, __pthread_key_delete(), is loaded, a
	  * suitable threads library exists, and the library symbol is called.
	  */
	if (__pthread_key_delete)
		return __pthread_key_delete(key);
	/* Otherwise, mark the key available in the statically allocated array.
	 */
	if (key < 0 || key >= PTHREAD_KEYS_MAX || !___pthread_keys[key].exists)
		return (EINVAL);
	___pthread_keys[key].exists = 0;
	___pthread_keys[key].value = NULL;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
	/** @retval EINVAL Invalid key argument. */
}

/** @} */

// vim: ft=c com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/
