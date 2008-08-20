/*****************************************************************************

 @(#) $RCSfile: apli.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-08-20 10:56:48 $

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

 Last Modified $Date: 2008-08-20 10:56:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: apli.c,v $
 Revision 0.9.2.5  2008-08-20 10:56:48  brian
 - fixes and build updates from newnet trip

 Revision 0.9.2.4  2008-04-29 00:02:01  brian
 - updated headers for release

 Revision 0.9.2.3  2008-04-25 08:38:32  brian
 - working up libraries modules and drivers

 Revision 0.9.2.2  2007/12/15 20:20:32  brian
 - updates

 Revision 0.9.2.1  2007/11/06 12:16:51  brian
 - added library and header files

 *****************************************************************************/

#ident "@(#) $RCSfile: apli.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-08-20 10:56:48 $"

static char const ident[] = "$RCSfile: apli.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-08-20 10:56:48 $";

/* This file can be processed with doxygen(1). */

/**
  * @defgroup libapli OpenSS7 APLI Library
  * @brief OpenSS7 APLI Library
  *
  * This manual contains documentation of the OpenSS7 ACSE/Presentation Library
  * Interface (APLI) Library functions that are generated automatically from
  * the source code with doxygen(1).  This documentation is intended to be used
  * for maintainers of the OpenSS7 APLI library and is not intended for users
  * of the library.  Users should consult the documentation found in the user
  * manual pages beginning with apli(3).
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 APLI library is designed to be thread-safe.  This is
  * accomplished in a number of ways.  Thread-safety depends on the use of
  * glibc2 and the pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data for errno and h_errno.
  * The OpenSS7 APLI library takes a similar approach but uses weak aliased
  * pthread thread-specific functions instead.
  *
  * Glibc2 also provides some weak undefined aliases for POSIX thread function
  * to perform its own thread-safety.  When the pthread library (libpthread) is
  * linked with glibc2, these functions call libpthread functions instead of
  * internal dummy routines.  The same approach is taken for the OpenSS7 APLI
  * library.  The library uses weak defined and undefined aliases that
  * automaticlaly invoke libpthread functions when libpthread is (dynamically)
  * linked and uses dummy functions when it is not.  This maintains maximum
  * efficiency when libpthread is not dynamically linked, but provides full
  * thread safety when it is.
  *
  * Libpthread behaves in some strange ways with regards to thread
  * cancellation.  Because libpthread uses Linux clone processes for threads,
  * cancellation of a thread is accomplished by sending a signal to the thread
  * process.  This does not directly result in cancellation, but will result in
  * the failure of a system call with the EINTR error code.  It is necessary to
  * test for cancellation upon error return from system calls to perform the
  * actual cancellation of the thread.
  *
  * The APLI specification and XAP specification (OpenGroup XAP) lists no
  * functions in this group as containing thread cancellation points.  Many of
  * the apli function are, however, permitted to contain thread cancellation
  * points.  All APLI functions are permitted to contain thread cancellation
  * points and deferral is not required.
  *
  * Locks and asyncrhonous thread cancellation presents challenges:
  *
  * Functions that act as thread cancellation points must push routines onto
  * the function stack executed at exit of the thread to release the locks
  * helod by the function.  These are performed with weak definitions of POSIX
  * thread library functions.
  *
  * Functions that do not act as thread cancellation points must defer thread
  * cancellation before taking locks and then release locks before thread
  * cancellation is restored.
  *
  * The above are the tehcniques used by glibc2 for the same purpose and is the
  * same technique that is used by the OpenSS7 APLI library.
  *
  * @{
  */
extern void __pthread_cleanup_push(struct _pthread_cleanup_buffer *buffer, void (*routine) (void *),
				   void *arg);
extern void __pthread_cleanup_pop(struct _pthread_cleanup_buffer *buffer, int execute);
extern void __pthread_cleanup_push_defer(struct _pthread_cleanup_buffer *buffer,
					 void (*routine) (void *), void *arg);
extern void __pthread_cleanup_pop_restore(struct __pthread_cleanup_buffer *buffer, int execute);
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

#pragma weak __pthread_conce
#pragma weak __pthread_key_create
#pragma weak __pthread_getspecific
#pragma weak __pthread_setspecific
#pragma weak __pthread_key_delete

#pragma weak pthread_conce
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

#ifndef APERR_BUFSZ
#define APERR_BUFSZ BUFSIZ
#endif

/*
 * Thread-specific data structures for the apli lirbrary.
 */
struct __apli_tsd {
	int aperror; /**< error for APLI library functions */
	char aperrbuf[APERR_BUFSZ]; /**< error buffer for APLI library functions */
	int fieldnum; /**< field number of error */
	int linenum; /**< line number of error */
};

/*
 * Once condition for Thread-Specific Data Key creation.
 */
static pthread_once_t __apli_tsd_once = PTHREAD_ONCE_INIT;

/*
 * APLI library Thread-Specific Data key.
 */
stat pthread_key_t __apli_tsd_key = 0;

static void
__apli_tsd_free(void *buf)
{
	pthread_setspecific(__apli_tsd_key, NULL);
	free(buf);
}

struct void
__apli_tsd_key_create(void)
{
	pthread_key_create(&__apli_tsd_key, __apli_tsd_free);
}


static struct __apli_tsd *
__apli_tsd_alloc(void)
{
	struct __apli_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(sizeof(*tsdp)));
	bzero(tsdp, sizeof(*tsdp));
	pthread_setspecific(__apli_tsd_key, (void *) tsdp);
	return (tsdp);
}

/**
  * @internal
  * @fn struct __apli_tsd *__apli_get_tsd(void)
  * @brief Get thread specific data for the APLI library.
  *
  * This function obtains (and allocates if necessary), thread specific data
  * for the executing thread.
  */
static struct __apli_tsd *
__apli_get_tsd(void)
{
	struct __apli_tsd *tsdp;

	pthread_once(&__apli_tsd_once, __apli_tsd_key_create);
	if (unlikely((tsdp = (typeof(tsdp)) pthread_getspecific(__apli_tsd_key)) == NULL))
		tsdp = __apli_tsd_alloc();
	return (tsdp);
};

/** @brief #ap_errno location function.
  * @version APLI_1.0
  * @par Alias:
  * This function is an implementation of _ap_errno().
  */
int *
__apli__ap_errno(void)
{
	return &(__apli_get_tsd()->aperror);
}

/** @fn int *_ap_errno(void)
  * @version APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli__ap_errno().
  *
  * This function provides the location of the integer that contains the APLI
  * library error number returned by the last APLI function that failed.  This
  * is normally used to provide #ap_errno in a thread-safe way as follows:
  *
  * @code
  * #define ap_errno (*(_ap_errno()))
  * @endcode
  */
__asm__(".symver __apli__ap_errno,_ap_errno@@APLI_1.0");

char *
__apli__ap_errbuf(void)
{
	return &(__apli_get_tsd()->aperrbuf[0]);
}

struct _ap_user {
	pthread_rwlock_t lock;		/**< lock for this structure */
	int refs;			/**< number of references to this structure */
	ap_env_t *env;			/**< environment */
};

static struct _ap_user *_ap_fds[OPEN_MAX] = { NULL, };

static pthread_rwlock_t __apli_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static inline int
__apli_lock_rdlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__apli_lock_wrlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline int
__apli_lock_unlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_unlock(rwlock);
}
static inline int
__apli_list_rdlock(void)
{
	return __apli_lock_rdlock(&__apli_fd_lock);
}
static inline int
__apli_list_wrlock(void)
{
	return __apli_lock_wrlock(&__apli_fd_lock);
}
static inline int
__apli_list_unlock(void)
{
	return __apli_lock_unlock(&__apli_fd_lock);
}
static inline int
__apli_user_rdlock(struct _ap_user *user)
{
	return __apli_lock_rdlock(&user->lock);
}
static inline int
__apli_user_wrlock(struct _ap_user *user)
{
	return __apli_lock_wrlock(&user->lock);
}
static inline void
__apli_user_unlock(struct _ap_user *user)
{
	return __apli_lock_unlock(&user->lock);
}

/** @internal
  * @brief Release a locked APLI user instance structure.
  * @param arg a pointer to the integer file descriptor of the library user
  *	structure.
  *
  * This function release a locked library user structure.  The @arg argument is
  * a void pointer so that this function can be used as a cancel deferral
  * function.
  */
static void
__apli_ap_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct _ap_user *user = _ap_fds[fd];

	__apli_user_unlock(user);
	__apli_list_unlock(NULL);
	return;
}

/** @internal
  * @brief Get a locked APLI user instance structure.
  * @param fd the file descriptor for which to get the associated endpoint.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this functino
  * takes the necessary locks for thread-safe operation.
  */
static __hot struct _ap_user *void
__apli_ap_getuser(int fd)
{
	struct _ap_user *user;
	int err;

	if (unlikely((err = __apli_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto tbadf;
	if (unlikely(!(user = _ap_fds[fd])))
		goto tbadf;
	if (unlikely((err = __apli_user_wrlock(user))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	ap_errno = AP_INTERNAL;
	errno = err;
	__xnet_list_unlock(NULL);
      tbadf:
	ap_errno = AP_BADF;
	goto error;
      list_lock_error:
	ap_errno = AP_INTERNAL;
	errno = err;
	goto error;
      error:
	return (NULL);
}

/** @brief open an APLI instance.
  * @param pathname pathname of device to open(2) call.
  * @param oflags flags to open(2) call.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_open().
  */
int
__apli_ap_open(const char *pathname, int oflags)
{
	struct _ap_user *user;
	int err, fd;

	if (!(user = (struct _ap_user *) malloc(sizeof(*user))))
		goto enomem;
	msmset(user, 0, sizeof(*user));
	if ((fd = open(pathname, oflags)) == -1)
		goto badopen;
	if (ioctl(fd, I_PUSH, "apmod") != 0)
		goto badioctl;
	/* need to pick up all the capabilities from the stream */
	pthread_rwlock_init(&user->lock, NULL);
	/* destroyed for existing structure */
	_ap_fds[fd] = user;
	return (fd);
      badioctl:
	err = errno;
	close(fd);
	free(user);
	errno = err;
	goto error;
      badopen:
	err = errno;
	free(user);
	errno = err;
	goto error;
      enomem:
	ap_errno = AP_INTERNAL;
	errno = ENOMEM;
	goto error;
      error:
	return (-1);
}

/** @brief The reentrant version of __apli_ap_open().
  * @param pathname pathname of device to open(2) call.
  * @param oflags flags to open(2) call.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_open().
  *
  * This is a little different than most of the _r wrappers: we take a write
  * lock on the _ap_fds list so that we are able to add the new file descriptor
  * to the list.  This will block most other threads from performing functions
  * on the list, also, we must wait for a quiet period until all other functions
  * that read lock the list are not being used.  If you are sure that the open
  * will only be performed by one thread and that no other thread will act on
  * the file descriptor until open returns, use the non-recursive version.
  */
int
__apli_ap_open_r(const char *pathname, int oflags)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__apli_list_unlock, NULL);
	if ((err = __apli_list_wrlock()) == 0) {
		ret = __apli_ap_open(pathname, oflags);
		__apli_list_unlock(NULL);
	} else {
		ap_errno = AP_INTERNAL;
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_open(const char *pathname, int oflags)
  * @param pathname pathname of device to open(2) call.
  * @param oflags flags to open(2) call.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_open().
  */
__asm__(".symver __apli_ap_open_r,ap_open@@APLI_1.0");

/** @internal
  * @brief initialize the environment of an APLI instance.
  * @param user	    APLI instance.
  * @param env_file pathname of environment file.
  * @param flags    unsed.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_close().
  *
  * This function performs the following actions:
  *
  * 1. If there is no structure already allocated, an environment structure is
  *    allocated and filled out with default values.  If one already exists,
  *    nothing is done in this step.
  *
  * 2. Values from env_file are filled out against the environment structure.
  */
static int
_apli_ap_init_env(struct _ap_user *user, const char *env_file, int flags)
{
	sturct ap_env *env;

	if (!(env = user->env)) {
		env = (struct ap_env *) malloc(sizeof(*env));
		if (!env)
			goto enomem;
		memset(env, 0, sizeof(*env));
		/* FIXME: set default values */
		user->env = env;
	}
	/* FIXME: read env_file */
	return (0);
      enomem:
	ap_errno = AP_NOMEM;
	errno = ENOMEM;
	goto error;
      error:
	return (-1);
}

/** The non-reentrant version of ap_init_env(). */
int
__apli_ap_init_env(int fd, const char *env_file, int flags)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_init_env(user, env_file, flags);
      badf:
	ap_errno = AP_BADF;
	errno = EBADF;
	return (-1);
}

/** The reentrant version of ap_init_env(). */
int
__apli_ap_init_env_r(int fd, const char *env_file, int flags)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_init_env(user, env_file, flags);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __apli_ap_init_env_r,ap_init_env@@APLI_1.0");

/** @fn int ap_init_env(int fd, const char *env_file, int flags)
  * @param fd file descriptor of APLI instance.
  * @param env_file pathname of environment file.
  * @param flags unsed.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_init_env().
  */
__asm__(".symver __apli_ap_init_env,ap_init_env@@APLI_1.0");

/** @internal
  * @brief restore APLI environment from file
  * @param user APLI instance.
  * @param savef stream from which to restore APLI environment
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  */
static int
_apli_ap_restore(struct _ap_user *user, FILE * savef)
{
	/* FIXME: restore from stream (perhaps using scanf? */
}

/** @brief restore APLI environment from file
  * @param fd file descriptor of APLI instance.
  * @param savef stream from which to restore APLI environment
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  *
  * Non-recursive version of ap_restore().
  */
int
__apli_ap_restore(int fd, FILE * savef)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_restore(user, savef);
      badf:
	ap_errno = AP_BADF;
	errno = EBADF;
	return (-1);
}

/** @brief restore APLI environment from file
  * @param fd file descriptor of APLI instance.
  * @param savef stream from which to restore APLI environment
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  *
  * Recursive version of ap_restore().
  */
int
__apli_ap_restore_r(int fd, FILE * savef)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_restore(user, savef);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_restore(int fd, FILE *savef)
  * @param fd file descriptor of APLI instance.
  * @param savef stream from which to restore APLI environment
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_restore().
  */
__asm__(".symver __apli_ap_restore_r,ap_restore@@APLI_1.0");

/** @internal
  * @brief get an APLI environment attribute.
  * @param user APLI user instance.
  * @param attr attribute to get.
  * @param val storage for retrieved attribute.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  */
static int
_apli_ap_get_env(struct _ap_user *user, unsigned long attr, void *val)
{
	if (val == NULL)
		goto noval;
	switch (attr) {
	case AP_ACSE_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_ACSE_SEL:
		/* FIXME: copy it */
		break;
	case AP_BIND_PADDR:
		/* FIXME: copy it */
		break;
	case AP_CLD_AEID:
		/* FIXME: copy it */
		break;
	case AP_CLD_AEQ:
		/* FIXME: copy it */
		break;
	case AP_CLD_APID:
		/* FIXME: copy it */
		break;
	case AP_CLD_APT:
		/* FIXME: copy it */
		break;
	case AP_CLD_CONN_ID:
		/* FIXME: copy it */
		break;
	case AP_CLG_AEID:
		/* FIXME: copy it */
		break;
	case AP_CLG_AEQ:
		/* FIXME: copy it */
		break;
	case AP_CLG_APID:
		/* FIXME: copy it */
		break;
	case AP_CLG_CONN_ID:
		/* FIXME: copy it */
		break;
	case AP_CNTX_NAME:
		/* FIXME: copy it */
		break;
	case AP_DCS:
		/* FIXME: copy it */
		break;
	case AP_DPCN:
		/* FIXME: copy it */
		break;
	case AP_DPCR:
		/* FIXME: copy it */
		break;
	case AP_INIT_SYNC_PT:
		/* FIXME: copy it */
		break;
	case AP_LCL_PADDR:
		/* FIXME: copy it */
		break;
	case AP_LIB_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_LIB_SEL:
		/* FIXME: copy it */
		break;
	case AP_MODE_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_MODE_SEL:
		/* FIXME: copy it */
		break;
	case AP_MSTATE:
		/* FIXME: copy it */
		break;
	case AP_PCDL:
		/* FIXME: copy it */
		break;
	case AP_PCDRL:
		/* FIXME: copy it */
		break;
	case AP_PFU_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_PFU_SEL:
		/* FIXME: copy it */
		break;
	case AP_PRES_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_PRES_SEL:
		/* FIXME: copy it */
		break;
	case AP_REM_PADDR:
		/* FIXME: copy it */
		break;
	case AP_ROLE_ALLOWED:
		/* FIXME: copy it */
		break;
	case AP_ROLE_CURRENT:
		/* FIXME: copy it */
		break;
	case AP_RSP_AEID:
		/* FIXME: copy it */
		break;
	case AP_RSP_AEQ:
		/* FIXME: copy it */
		break;
	case AP_RSP_APID:
		/* FIXME: copy it */
		break;
	case AP_RSP_APT:
		/* FIXME: copy it */
		break;
	case AP_SESS_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_SESS_SEL:
		/* FIXME: copy it */
		break;
	case AP_SFU_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_SFU_SEL:
		/* FIXME: copy it */
		break;
	case AP_STATE:
		/* FIXME: copy it */
		break;
	case AP_STREAM_FLAGS:
		/* FIXME: copy it */
		break;
	case AP_TOKENS_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_TOKENS_OWNED:
		/* FIXME: copy it */
		break;
	default:
		goto badattr;
	}
	return (0);
      badattr:
	ap_errno = AP_BADATTR;
	errno = EINVAL;
	goto error;
      error:
	return (-1);
}

/** @brief non-recursive version of ap_get_env().
  * @param fd file descriptor of APLI instance.
  * @param attr attributes to get.
  * @param val storage for retrieved atttributes.
  * @version APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  */
int
__apli_ap_get_env(int fd, unsigned long attr, void *val)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_get_env(user, attr, val);
      badf:
	ap_errno = AP_BADF;
	errno = BADF;
	return (-1);
}

/** @brief recursive version of ap_get_evn().
  * @param fd file descriptor of APLI instance.
  * @param attr attributes to get.
  * @param val storage for retrieved atttributes.
  * @version APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  */
int
__apli_ap_get_env_r(int fd, unsigned long attr, void *val)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_get_env(user, attr, val);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_get_env(int fd, unsigned long attr, void *val)
  * @param fd file descriptor of APLI instance.
  * @param attr attribute to get.
  * @param val storage for retrieved attribute.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_get_env_r().
  */
__asm__(".symver __apli_ap_get_env_r,ap_get_env@@APLI_1.0");

/** @itnernal
  * @brief set an APLI environment attribute.
  * @param user APLI instance.
  * @param attr attribute to set.
  * @param val storage for attribute setting.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  */
int
__apli_ap_set_env(struct _ap_user *user, unsigned long attr, ap_val_t val)
{
	if (val == NULL)
		goto noval;
	switch (attr) {
	case AP_ACSE_AVAIL:
		/* FIXME: set it */
		break;
	case AP_ACSE_SEL:
		/* FIXME: set it */
		break;
	case AP_BIND_PADDR:
		/* FIXME: set it */
		break;
	case AP_CLD_AEID:
		/* FIXME: set it */
		break;
	case AP_CLD_AEQ:
		/* FIXME: set it */
		break;
	case AP_CLD_APID:
		/* FIXME: set it */
		break;
	case AP_CLD_APT:
		/* FIXME: set it */
		break;
	case AP_CLD_CONN_ID:
		/* FIXME: set it */
		break;
	case AP_CLG_AEID:
		/* FIXME: set it */
		break;
	case AP_CLG_AEQ:
		/* FIXME: set it */
		break;
	case AP_CLG_APID:
		/* FIXME: set it */
		break;
	case AP_CLG_CONN_ID:
		/* FIXME: set it */
		break;
	case AP_CNTX_NAME:
		/* FIXME: set it */
		break;
	case AP_DCS:
		/* FIXME: set it */
		break;
	case AP_DPCN:
		/* FIXME: set it */
		break;
	case AP_DPCR:
		/* FIXME: set it */
		break;
	case AP_INIT_SYNC_PT:
		/* FIXME: set it */
		break;
	case AP_LCL_PADDR:
		/* FIXME: set it */
		break;
	case AP_LIB_AVAIL:
		/* FIXME: set it */
		break;
	case AP_LIB_SEL:
		/* FIXME: set it */
		break;
	case AP_MODE_AVAIL:
		/* FIXME: set it */
		break;
	case AP_MODE_SEL:
		/* FIXME: set it */
		break;
	case AP_MSTATE:
		/* FIXME: set it */
		break;
	case AP_PCDL:
		/* FIXME: set it */
		break;
	case AP_PCDRL:
		/* FIXME: set it */
		break;
	case AP_PFU_AVAIL:
		/* FIXME: set it */
		break;
	case AP_PFU_SEL:
		/* FIXME: set it */
		break;
	case AP_PRES_AVAIL:
		/* FIXME: set it */
		break;
	case AP_PRES_SEL:
		/* FIXME: set it */
		break;
	case AP_REM_PADDR:
		/* FIXME: set it */
		break;
	case AP_ROLE_ALLOWED:
		/* FIXME: set it */
		break;
	case AP_ROLE_CURRENT:
		/* FIXME: set it */
		break;
	case AP_RSP_AEID:
		/* FIXME: set it */
		break;
	case AP_RSP_AEQ:
		/* FIXME: set it */
		break;
	case AP_RSP_APID:
		/* FIXME: set it */
		break;
	case AP_RSP_APT:
		/* FIXME: set it */
		break;
	case AP_SESS_AVAIL:
		/* FIXME: set it */
		break;
	case AP_SESS_SEL:
		/* FIXME: set it */
		break;
	case AP_SFU_AVAIL:
		/* FIXME: set it */
		break;
	case AP_SFU_SEL:
		/* FIXME: set it */
		break;
	case AP_STATE:
		/* FIXME: set it */
		break;
	case AP_STREAM_FLAGS:
		/* FIXME: set it */
		break;
	case AP_TOKENS_AVAIL:
		/* FIXME: set it */
		break;
	case AP_TOKENS_OWNED:
		/* FIXME: set it */
		break;
	default:
		goto badval;
	}
	return (0);
      badattr:
	ap_errno = AP_BADATTR;
	errno = EINVAL;
	goto error;
      error:
	return (-1);
}

/** @brief set an APLI environment attribute.
  * @param fd file descriptor of APLI instance.
  * @param attr attribute to set.
  * @param val storage for attribute setting.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  */
int
__apli_ap_set_env(int fd, unsigned long attr, ap_val_t val)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_set_env(user, attr, val);
      badf:
	ap_errno = AP_BADF;
	errno = BADF;
	return (-1);
}

/** @brief set an APLI environment attribute.
  * @param fd file descriptor of APLI instance.
  * @param attr attribute to set.
  * @param val storage for attribute setting.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  */
int
__apli_ap_set_env_r(int fd, unsigned long attr, ap_val_t val)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_set_env(user, attr, val);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_set_env(int fd, unsigned long attr, ap_val_t val)
  * @param fd file descriptor of APLI instance.
  * @param attr attribute to set.
  * @param val storage for attribute setting.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_set_env().
  */
__asm__(".symver __apli_ap_set_env,ap_set_env@@APLI_1.0");

/** @internal
  * @brief save APLI environment to file
  * @param user APLI instance.
  * @param savef pathname of file to which to save
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_save().
  */
static int
_apli_ap_save(struct _ap_user *user, FILE * savef)
{
}

/** @brief save APLI environment to file
  * @param fd file descriptor of APLI instance.
  * @param savef pathname of file to which to save
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_save().
  */
int
__apli_ap_save(int fd, FILE * savef)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_save(user, savef);
      badf:
	ap_errno = AP_BADF;
	errno = BADF;
	return (-1);
}

/** @brief save APLI environment to file
  * @param fd file descriptor of APLI instance.
  * @param savef pathname of file to which to save
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_save().
  */
int
__apli_ap_save_r(int fd, FILE * savef)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_save(user, savef);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_save(int fd, FILE *savef)
  * @param fd file descriptor of APLI instance.
  * @param savef stream to which to write attributes
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_save().
  */
__asm__(".symver __apli_ap_save,ap_save@@APLI_1.0");

/** @internal
  * @brief send an APLI service primitive.
  * @param user APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_snd().
  */
static int
_apli_ap_snd(struct _ap_user user, unsigned long sptype, ap_cdata_t * cdata, struct osi_buf *ubuf,
	     int flags)
{
	/* FIXME: perform send function. */
	return (0);
}

/** @brief send an APLI service primitive.
  * @param fd file descriptor of APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_snd().
  */
int
__apli_ap_snd(int fd, unsigned long sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int flags)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_snd(user, sptype, cdata, ubuf, flags);
      badf:
	ap_errno = AP_BADF;
	errno = BADF;
	return (-1);
}

/** @brief send an APLI service primitive.
  * @param fd file descriptor of APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_snd().
  */
int
__apli_ap_snd_r(int fd, unsigned long sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int flags)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(&__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_snd(user, sptype, cdata, ubuf, flags);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_snd(int fd, unsigned long sptype, ap_cdata_t *cdata, struct osi_buf *ubuf, int flags)
  * @param fd file descriptor of APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_snd().
  */
__asm__(".symver __apli_ap_snd,ap_snd@@APLI_1.0");

/** @internal
  * @brief send an APLI service primtive.
  * @param user APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_rcv().
  */
static int
_apli_ap_rcv(struct _ap_user *user, unsigned long *sptype, ap_cdata_t * cdata, struct osi_buf *ubuf,
	     int *flags)
{
	/* FIXME: perform receive function. */
	return (0);
}

/** @brief send an APLI service primtive.
  * @param fd file descriptor of APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_rcv().
  */
int
__apli_ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int *flags)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_rcv(user, sptype, cdata, ubuf, flags);
      badf:
	ap_errno = AP_BADF;
	errno = BADF;
	return (-1);
}

/** @brief send an APLI service primtive.
  * @param fd file descriptor of APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_rcv().
  */
int
__apli_ap_rcv_r(int fd, unsigned long *sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int *flags)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_rcv(user, sptype, cdata, ubuf, flags);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_rcv(int fd, unsigned long *sptype, ap_cdata_t *cdata, struct osi_buf *ubuf, int *flags)
  * @param fd file descriptor of APLI instance.
  * @param sptype service primitive type.
  * @param cdata control data.
  * @param ubuf user data.
  * @param flags continuation flags.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_rcv_r().
  */
__asm__(".symver __apli_ap_rcv_r,ap_rcv@@APLI_1.0");

/** @brief free internal members of APLI structure
  * @param kind kind of structure to free.
  * @param val pointer to structure to free.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_free().
  */
int
__apli_ap_free(unsigned long kind, void *val)
{
	switch (kind) {
	case AP_CDATA_T:
		/* FIXME: free it */
		goto done;
	case AP_CDL_T:
		/* FIXME: free it */
		goto done;
	case AP_CDRL_T:
		/* FIXME: free it */
		goto done;
	case AP_CONN_ID_T:
		/* FIXME: free it */
		goto done;
	case AP_DCN_T:
		/* FIXME: free it */
		goto done;
	case AP_DCS_T:
		/* FIXME: free it */
		goto done;
	case AP_PADDR_T:
		/* FIXME: free it */
		goto done;
	case AP_ANY_T:
		/* FIXME: free it */
		goto done;
	case AP_OBJID_T:
		/* FIXME: free it */
		goto done;
	}
	switch (kind) {
	case AP_ACSE_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_ACSE_SEL:
		/* FIXME: free it */
		goto done;
	case AP_BIND_PADDR:
		/* FIXME: free it */
		goto done;
	case AP_CLD_AEID:
		/* FIXME: free it */
		goto done;
	case AP_CLD_AEQ:
		/* FIXME: free it */
		goto done;
	case AP_CLD_APID:
		/* FIXME: free it */
		goto done;
	case AP_CLD_APT:
		/* FIXME: free it */
		goto done;
	case AP_CLD_CONN_ID:
		/* FIXME: free it */
		goto done;
	case AP_CLG_AEID:
		/* FIXME: free it */
		goto done;
	case AP_CLG_AEQ:
		/* FIXME: free it */
		goto done;
	case AP_CLG_APID:
		/* FIXME: free it */
		goto done;
	case AP_CLG_CONN_ID:
		/* FIXME: free it */
		goto done;
	case AP_CNTX_NAME:
		/* FIXME: free it */
		goto done;
	case AP_DCS:
		/* FIXME: free it */
		goto done;
	case AP_DPCN:
		/* FIXME: free it */
		goto done;
	case AP_DPCR:
		/* FIXME: free it */
		goto done;
	case AP_INIT_SYNC_PT:
		/* FIXME: free it */
		goto done;
	case AP_LCL_PADDR:
		/* FIXME: free it */
		goto done;
	case AP_LIB_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_LIB_SEL:
		/* FIXME: free it */
		goto done;
	case AP_MODE_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_MODE_SEL:
		/* FIXME: free it */
		goto done;
	case AP_MSTATE:
		/* FIXME: free it */
		goto done;
	case AP_PCDL:
		/* FIXME: free it */
		goto done;
	case AP_PCDRL:
		/* FIXME: free it */
		goto done;
	case AP_PFU_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_PFU_SEL:
		/* FIXME: free it */
		goto done;
	case AP_PRES_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_PRES_SEL:
		/* FIXME: free it */
		goto done;
	case AP_REM_PADDR:
		/* FIXME: free it */
		goto done;
	case AP_ROLE_ALLOWED:
		/* FIXME: free it */
		goto done;
	case AP_ROLE_CURRENT:
		/* FIXME: free it */
		goto done;
	case AP_RSP_AEID:
		/* FIXME: free it */
		goto done;
	case AP_RSP_AEQ:
		/* FIXME: free it */
		goto done;
	case AP_RSP_APID:
		/* FIXME: free it */
		goto done;
	case AP_RSP_APT:
		/* FIXME: free it */
		goto done;
	case AP_SESS_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_SESS_SEL:
		/* FIXME: free it */
		goto done;
	case AP_SFU_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_SFU_SEL:
		/* FIXME: free it */
		goto done;
	case AP_STATE:
		/* FIXME: free it */
		goto done;
	case AP_STREAM_FLAGS:
		/* FIXME: free it */
		goto done;
	case AP_TOKENS_AVAIL:
		/* FIXME: free it */
		goto done;
	case AP_TOKENS_OWNED:
		/* FIXME: free it */
		goto done;
	}
	switch (kind) {
	case AP_A_ASSOC_REQ:
		/* FIXME: free it */
		goto done;
	case AP_A_ASSOC_IND:
		/* FIXME: free it */
		goto done;
	case AP_A_ASSOC_RSP:
		/* FIXME: free it */
		goto done;
	case AP_A_ASSOC_CNF:
		/* FIXME: free it */
		goto done;
	case AP_A_RELEASE_REQ:
		/* FIXME: free it */
		goto done;
	case AP_A_RELEASE_IND:
		/* FIXME: free it */
		goto done;
	case AP_A_RELEASE_RSP:
		/* FIXME: free it */
		goto done;
	case AP_A_RELEASE_CNF:
		/* FIXME: free it */
		goto done;
	case AP_A_ABORT_REQ:
		/* FIXME: free it */
		goto done;
	case AP_A_ABORT_IND:
		/* FIXME: free it */
		goto done;
	case AP_A_PABORT_REQ:
		/* FIXME: free it */
		goto done;
	case AP_A_PABORT_IND:
		/* FIXME: free it */
		goto done;
	case AP_P_DATA_REQ:
		/* FIXME: free it */
		goto done;
	case AP_P_DATA_IND:
		/* FIXME: free it */
		goto done;
	case AP_P_TOKENGIVE_REQ:
		/* FIXME: free it */
		goto done;
	case AP_P_TOKENGIVE_IND:
		/* FIXME: free it */
		goto done;
	case AP_P_TOKENPLEASE_REQ:
		/* FIXME: free it */
		goto done;
	case AP_P_TOKENPLEASE_IND:
		/* FIXME: free it */
		goto done;
	}
	goto badkind;
      done:
	return (0);
      badkind:
	ap_errno = AP_BADKIND;
	errno = EINVAL;
	goto error;
      error:
	return (-1);
}

/** @fn int ap_free(unsigned long kind, void *val)
  * @param kind kind of structure to free.
  * @param val pointer to structure to free.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_free().
  */
__asm__(".symver __apli_ap_free,ap_free@@APLI_1.0");

/** @brief poll an APLI instance
  * @param fds poll file descriptor structures.
  * @param nfds number of poll file descriptor structures in array.
  * @param timeout time out in milliseconds.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_poll().
  */
int
__apli_ap_poll(struct ap_pollfd *fds, int nfds, int timeout)
{
	struct pollfd *_fds = (struct pollfd *) fds;
	int err;

	if ((err = poll(fds, nfds, timeout)) == -1)
		aperrno = AP_INTERNAL;
	return (err);
}

/** @fn int ap_poll(struct ap_pollfd *fds, int nfds, int timeout)
  * @param fds poll file descriptor structures.
  * @param nfds number of poll file descriptor structures in array.
  * @param timeout time out in milliseconds.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_poll().
  */
__asm__(".symver __apli_ap_poll,ap_poll@@APLI_1.0");

/* *INDENT-OFF* */
const char *__apli_ap_errlist[] = {
/*
TRANS AP_NOERROR - No error is indicated in the ap_errno variable.  The last
TRANS operation was a success.  ap_errno will not be set to this value (zero) by
TRANS the library, the user must set ap_errno to zero before the APLI library
TRANS call and when the call is successful, the ap_errno value will be
TRANS unaffected.  There is no requirement that this value be set after a
TRANS successful call, and calls are event permitted to change ap_errno to some
TRANS other value, when the call is actually successful.
*/
	gettext_noop("no error");
/*
TRANS AP_ACCES - Roughly equivalent to UNIX system error EPERM.
*/
	gettext_noop("request to bind to specified address denied");
/*
TRANS AP_AGAIN - Roughly equivalent to UNIX system error EAGAIN.
*/
	gettext_noop("request not completed");
/*
TRANS AP_BADATTRVAL - 
*/
	gettext_noop("bad value for environment attribute");
/*
TRANS AP_BADCD_ACT_ID - Roughly equivalent to UNIX system error EINVAL. The
TRANS member, act_id, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: act_id");
/*
TRANS AP_BADCD_DIAG - Roughly equivalent to UNIX system error EINVAL. The
TRANS member, diag, of the cdata (ap_cdata_t) structure passed to the ap_snd(3)
TRANS function contained a value that was not valid in the current state or
TRANS context.
*/
	gettext_noop("cdata field value invalid: diag");
/*
TRANS AP_BADCD_EVT - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, event, of the cdata (ap_cdata_t) structure passed to the ap_snd(3)
TRANS function contained a value that was not valid in the current state or
TRANS context.
*/
	gettext_noop("cdata field value invalid: event");
/*
TRANS AP_BADCD_OLD_ACT_ID - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, old_act_id, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: old_act_id");
/*
TRANS AP_BADCD_OLD_CONN_ID - Roughly equivalent to UNIX system error EINVAL.
TRANS The member, old_conn_id, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: old_conn_id");
/*
TRANS AP_BADCD_PABORT_IND - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, pabort_ind, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: pabort_ind");
/*
TRANS AP_BADCD_RES - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, res, of the cdata (ap_cdata_t) structure passed to the ap_snd(3)
TRANS function contained a value that was not valid in the current state or
TRANS context.
*/
	gettext_noop("cdata field value invalid: res");
/*
TRANS AP_BADCD_RES_SRC - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, res_src, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: res_src");
/*
TRANS AP_BADCD_RESYNC_TYPE - Roughly equivalent to UNIX system error EINVAL.
TRANS The member, resync_type, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: resync_type");
/*
TRANS AP_BADCD_RSN - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, rsn, of the cdata (ap_cdata_t) structure passed to the ap_snd(3)
TRANS function contained a value that was not valid in the current state or
TRANS context.
*/
	gettext_noop("cdata field value invalid: rsn");
/*
TRANS AP_BADCD_SRC - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, src, of the cdata (ap_cdata_t) structure passed to the ap_snd(3)
TRANS function contained a value that was not valid in the current state or
TRANS context.
*/
	gettext_noop("cdata field value invalid: src");
/*
TRANS AP_BADCD_SYNC_P_SN - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, sync_p_sn, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: sync_p_sn");
/*
TRANS AP_BADCD_SYNC_TYPE - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, sync_type, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: sync_type");
/*
TRANS AP_BADCD_TOKENS - Roughly equivalent to UNIX system error EINVAL.  The
TRANS member, tokens, of the cdata (ap_cdata_t) structure passed to the
TRANS ap_snd(3) function contained a value that was not valid in the current
TRANS state or context.
*/
	gettext_noop("cdata field value invalid: tokens");
/*
TRANS AP_BADENC - When the user passes data to the ap_snd(3) function, it must
TRANS be encoded in the appropriate encoding for the abstract/transfer syntax
TRANS negotiated for the presentation layer.  The ap_snd(3) function has
TRANS determined that the choice of encoding for the envelope of the user data
TRANS was incorrect.
*/
	gettext_noop("bad encoding chosen in enveloping function");
/*
TRANS AP_BADENV - A mandatory ALPI environment attribute, settable with the
TRANS ap_set_env(3) and gettable with the ap_get_env(3) functions, was not set
TRANS at the time of the call to ap_snd(3), but the service primitive invoked by
TRANS the function requires that the attribute be set by the caller to a
TRANS specific value.  Therefore, the call to ap_snd(3) has failed because a
TRANS mandatory attribute is not set.
*/
	gettext_noop("a mandatory attribute is not set");
/*
TRANS AP_BADF - Most APLI library functions have a file descriptor, fd, passed
TRANS as the first argument of the function.  This file descriptor is both a
TRANS handle to the APLI instance that the user wishes to control as well as
TRANS being a character special file, pipe or named FIFO, representing a
TRANS presentation service provider.  The APLI library tracks which of the
TRANS process' file descriptors are associated with presentation service
TRANS providers, and thus represent APLI instances.  Those that do not, such as
TRANS standard input, standard output or standard error, cannot have their file
TRANS descriptors passed to APLI library functions without generating this
TRANS error.  This error indicates that the passed in file descriptor, fd, did
TRANS not correspond to a presentation service endpoint, and so does not
TRANS correspond to an APLI instances and therefore cannot be acted upon by the
TRANS APLI library function.
*/
	gettext_noop("not a presentation service endpoint");
/*
TRANS AP_BADFLAGS -  For some combinations of flags passed to APLI library
TRANS functions as arguments, the combinations do not follow a strict choice of
TRANS unrestricted logical OR combination.  Some flag combinations are not
TRANS permitted or are invalid or are unspecified or unsupported.  This error
TRANS indicates that a combination of flags passed by the user was invalid per
TRANS the specification of the flags.
*/
	gettext_noop("combination of flags is invalid");
/*
TRANS AP_BADFREE - When the ap_free(3) function is called, it attempts to free,
TRANS using the free(3) C library call, all of the allocated structures internal
TRANS to the passed in structure pointer.  The only internal structures the may
TRANS be freed with ap_free(3) are the structures that were allocated by the
TRANS APLI library and passed to the user in response to a command.  As such,
TRANS when these same internal structures are passed to the ap_free(3) function,
TRANS the APLI library may be able to verify whether the structures were indeed
TRANS allocated by the library.  If this can be detected and it is determined
TRANS that an internal structure was not allocated by the APLI library, the
TRANS ap_free(3) function will not free the structure with the free(3) C library
TRANS call, and will, instead, return this error indicating that it could not
TRANS free structure members.
*/
	gettext_noop("could not free structure members");
/*
TRANS AP_BADKIND - The ap_free(3) function is called with a 'kind' argument that
TRANS indicates to which type of structure the call applies.  The value of this
TRANS argument may correspond to a struct type (in all capitals) symbolic
TRANS constant, or a service primitive type symbolic constant, or an environment
TRANS attribute symbolic constant.  When the 'kind' argument value passed does
TRANS not correspond to any of these symbolic constants, then the structure type
TRANS is unknown and this error generated.
*/
	gettext_noop("unknown structure type");
/*
TRANS AP_BADLSTATE - 
*/
	gettext_noop("instance in bad state for that command");
/*
TRANS AP_BADPARSE - 
*/
	gettext_noop("attribute parse failed");
/*
TRANS AP_BADPRIM - When calling the ap_snd(3) function, an 'sptype' argument is
TRANS passed which specifies the 'Service Primitive TYPE' for the call.  The
TRANS service primitive type identifies the protocol service primitive to be
TRANS passed to the presentation service provider.  When this error is
TRANS generated, it means that the user has passed a service primitive type,
TRANS sptype, does not correspond to a symbolic constant know to the APLI
TRANS library.
*/
	gettext_noop("unrecognized primitive from user");
/*
TRANS AP_BADRESTR - The ap_restore(3) function can be used to set APLI
TRANS environment attributes associated with an APLI instance.  When data is
TRANS sent in multiple segments, the AP_MORE bit is used in the flags argument
TRANS to ap_snd(3) for all calls except the last.  Attributes are not permitted
TRANS to change after calls to ap_snd(3) in which the AP_MORE bit has been set,
TRANS until a call to ap_snd(3) has been made with the AP_MORE bit clear.  This
TRANS error indicates that ap_restore(3) was called while the AP_MORE bit was
TRANS set (i.e. a call has been made to ap_snd(3) with the AP_MORE bit set in
TRANS the flags argument and no subsequent call to ap_snd(3) without the AP_MORE
TRANS bit set has been sent).
*/
	gettext_noop("not restored due to more bit on (AP_MORE set)");
/*
TRANS AP_BADROLE - The APLI library user has attempted to call a function,
TRANS typically ap_snd(3) for an environment attribute or service primitive type
TRANS which is not applicable to the roles (intiiator or responder) for which
TRANS the APLI instance is allowed (as per environment attribute ROLE_ALLOWED).
*/
	gettext_noop("request invalid due to value of AP_ROLE");
/*
TRANS AP_BADSAVE - The ap_save(3) function can be used to store APLI environment
TRANS attribtues associated with an APLI instance in a file.  When data is sent
TRANS in multiple segments, the AP_MORE bit is used in the flags argument to
TRANS ap_snd(3) for all calls except the last.  Attributes are not permitted to
TRANS change after calls to ap_snd(3) in which the AP_MORE bit has been set,
TRANS untila call to ap_snd(3) has been made with the AP_MORE bit clear.  The
TRANS AP_MORE bit is set in the environment during this period.  Because an
TRANS attribute value including the AP_MORE bit should not be saved, the
TRANS ap_save(3) function will fail while the AP_MORE bit is set, with this
TRANS error code.
*/
	gettext_noop("not save due to more bit on (AP_MORE set)");
/*
TRANS AP_BADSAVEF - The FILE pointer argument, savef, passed to ap_save(3) and
TRANS ap_restore(3) is not a valid file pointer.  It may be NULL or does not
TRANS represent a valid file stream open for writing.
*/
	gettext_noop("invalid FILE pointer");
/*
TRANS AP_BADUBUF - 
*/
	gettext_noop("bad length for user data");
/*
TRANS AP_HANGUP - 
*/
	gettext_noop("association close or aborted");
/*
TRANS AP_INTERNAL - 
*/
	gettext_noop("internal error");
/*
TRANS AP_LOOK - 
*/
	gettext_noop("a pending event requires attention");
/*
TRANS AP_NOATTR - 
*/
	gettext_noop("no such attribute")
/*
TRANS AP_NOENV - 
*/
	gettext_noop("no environment for that fd");
/*
TRANS AP_NOMEM - 
*/
	gettext_noop("could not allocate enough memory");
/*
TRANS AP_NOREAD - 
*/
	gettext_noop("attribute is not readable");
/*
TRANS AP_NOSET - 
*/
	gettext_noop("attribute is not setable");
/*
TRANS AP_NOWRITE - 
*/
	gettext_noop("attribute is not writable");
/*
TRANS AP_PDUREJ - 
*/
	gettext_noop("invalid PDU rejected");
};
/* *INDENT-ON* */

const char *
__apli_ap_strerror(int errnum)
{
	if (0 <= errnum && errnum < AP_ERRMAX)
		return gettext(__apli_ap_errlist[errnum]);
	return gettext(__apli_ap_errlist[AP_ERRMAX]);
}

/** @brief report an APLI error.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_error().
  */
const char *
__apli_ap_error(void)
{
	/* copy error string to thread-specific data */
	snprintf(__apli_ap_errbuf(), APERR_BUFSZ, "%s", __apli_ap_strerror());
}

/** @fn const char *ap_error(void)
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_error().
  */
__asm__(".symver __apli_ap_error,ap_error@@APLI_1.0");

/** @internal
  * @brief close an APLI instance.
  * @param user the APLI instance.
  * @param fd file descriptor associated with the APLI instance.
  */
static int
_apli_ap_close(struct _ap_user *user, int fd)
{
	int err;
	if ((err = close(fd)) != 0) {
		if (errno == EBADF)
			aperrno = AP_BADF;
		else
			aperrno = AP_INTERNAL;
	}
	return (err);
}

/** @brief close an APLI instance.
  * @param fd file descriptor of APLI instance.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_close().
  */
int
__apli_ap_close(int fd)
{
	struct _ap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _ap_fds[fd]))
		goto badf;
	return _apli_ap_close(user, fd);
      badf:
	ap_errno = AP_BADF;
	errno = BADF;
	return (-1);
}

/** @brief close an APLI instance.
  * @param fd file descriptor of APLI instance.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is an implementation of ap_close().
  */
int
__apli_ap_close_r(int fd)
{
	struct _ap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__apli_ap_putuser, &fd);
	if ((user = __apli_ap_getuser(fd))) {
		ret = _apli_ap_close(user, fd);
		__apli_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);

}

/** @fn int ap_close(int fd)
  * @param fd file descriptor of APLI instance.
  * @verion APLI_1.0
  * @par Alias:
  * This symbol is a strong alias of __apli_ap_close().
  */
__asm__(".symver __apli_ap_close,ap_close@@APLI_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS fo+=tcqlornb
