/*****************************************************************************

 @(#) $RCSfile: xap.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 00:02:02 $

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

 Last Modified $Date: 2008-04-29 00:02:02 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xap.c,v $
 Revision 0.9.2.5  2008-04-29 00:02:02  brian
 - updated headers for release

 Revision 0.9.2.4  2008-04-25 08:38:32  brian
 - working up libraries modules and drivers

 Revision 0.9.2.3  2007/12/15 20:20:33  brian
 - updates

 Revision 0.9.2.2  2007/11/10 19:40:54  brian
 - documentation updates

 Revision 0.9.2.1  2007/11/06 12:16:52  brian
 - added library and header files

 *****************************************************************************/

#ident "@(#) $RCSfile: xap.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 00:02:02 $"

static char const ident[] =
    "$RCSfile: xap.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 00:02:02 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup xap OpenSS7 XAP Library
  * @{ */

/** @file
  * OpenSS7 X/Open ASCE/Presentation (XAP) implemernation file.  */

/**
  * This manual contains documetnatoin of the OpenSS7 XAP Library functions
  * that are generated autormatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 XAP
  * Library and is not intended for users of the OpenSS7 XAP Library.  Users
  * should consult the documentation found in the user manual pages beginning
  * with xap(3).
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 XAP Library is designed to be thread-safe.  This is
  * accomplished in a number of ways.  Thread-safety depends on the use of
  * glibc and the pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data for errno and h_errno.
  * Because h_errno uses communications functions orthoginal to the XAP Library
  * services, we borrow h_errno and use it for aperrno.  This does not cause a
  * problem because neither h_errno nor aperrno need to maintain their value
  * across any other system call.
  *
  * Glibc2 also provdes some weak undefined aliases for POSIX thread functions
  * to perform its own thread-safety.  When the pthread library (libpthread) is
  * linked with glibc2, these function call libpthread functions instead of
  * internal dummy routines.  The same approach is taken for the OpenSS7 XAP
  * Library.  The library uses weak defined and undefined aliases that
  * automatically invoke libpthread functions when libpthread is (dynamically)
  * linked and uses dummy functions when it is not.  This maintains maximum
  * efficiency when libpthread is not dynamically linked, but provides full
  * thread safety when it is.
  *
  * Libpthread behaves in some strange ways with regard to thread cancellation.
  * Because libpthread uses Linux clone processes for threads, cancellation of
  * a thread is accomplished by sending a signal to the thread process.  This
  * does not directly result in cancellation, but will result in the failure of
  * a system call with the EINTR error code.  It is ncessary to test for
  * cancellation upon error return from system calls to perform the actual
  * cancellation of the thread.
  *
  * The XAP specification (OpenGroup XAP 3) lists the following functions as
  * being thread cancellation points:
  *
  * The OpenSS7 XAP Library adds the following functions that operate on data
  * or expedited data with options that are not present in the XAP 3
  * specifications, that are also thread cancellation points:
  *
  * Other XAP functions are not permitted by XNS 5.2 to be thread cancellation
  * points.  Any function that cannot be a thread cancellation point needs to
  * have its cancellation status deferred if it internally invokes a functoin
  * that permits thread cancellation.  Functions that do not permit thread
  * cancellation are:
  *
  * Locks and asyncrhonous thread cancellation present challenges:
  *
  * Functions that act as thread cancellation points must push routines onto
  * the function stack executed at exit of the thread to release the locks held
  * by the function.  These are performed with weak definitions of POSIX thread
  * library functions.
  *
  * Functions that do not act as thread cancellation points must defer thread
  * cancellation before taking locks and then release locks before thread
  * cancellation is restored.
  *
  * The above are the techniques used by glibc2 for the same purpose and is the
  * same technique that is used by the OpenSS7 XAP Library.
  */

struct __xap_tsd {
	int aperrno;
};

/*
 *  Once condition for Thread-Specific Data key creation.
 */
static pthread_once_t __xap_tsd_once = PTHREAD_ONCE_INIT;

/*
 *  XAP Library Thread-Specific Data key.
 */
static pthread_key_t __xap_tsd_key = 0;

static void
__xap_tsd_free(void *buf)
{
	pthread_setspecific(__xap_tsd_key, NULL);
	free(buf);
}

static void
__xap_tsd_alloc(void)
{
	int ret;
	void *buf;

	ret = pthread_key_create(&__xap_tsd_key, __xap_tsd_free);
	buf = malloc(sizeof(struct __xap_tsd));
	bzero(buf, sizeof(*buf));
	ret = pthread_setspecific(__xap_tsd_key, buf);
	return;
}

/** @internal
  * @brief Get thread specific data for the XAP Library.
  *
  * This function obtains (and allocates if necessary), thread specific data
  * for the executing thread.
  */
static struct __xap_tsd *
__xap_get_tsd(void)
{
	pthread_once(&__xap_tsd_once, __xnet_tsd_alloc);
	return (struct __xap_tsd *) pthread_getspecific(__xap_tsd_key);
}

/** @brief #aperrno location function.
  * @version XAP_1.0
  * @par Alias:
  * This function is an implementatoin of _aperrno().
  */
int *
__xap__aperrno(void)
{
	return &(__xap_get_tsd()->aperrno);
}

/** @fn int *_aperrno(void)
  * @version XAP_1.0
  * @par Alias:
  * This symbol is a strong alias of __xap__aperrno().
  *
  * This function provides the location of the integer that contains the XAP
  * error number returned by the last XAP functoin that failed.  This is
  * normally used to provide #aperrno in a thread-safe way as follows:
  *
  * @code
  * #define aperrno (*(_aperrno()))
  * @endcode
  */
__asm__(".symver __xap__aperrno,_aperrno@@XAP_1.0");

char *
__xap__aperrbuf(void)
{
	return &(__xap_get_tsd()->aperrbuf[0]);
}

struct _ap_user {
	pthread_rwlock_t lock;	    /**< lock for this structure */
	int refs;		    /**< number of references to this structure */
	int event;		    /**< pending ap_look() events */
	int flags;		    /**< user flags */
	int fflags;		    /**< file flags */
	int gflags;		    /**< getmsg flags */
	int state;		    /**< XAP state */
	int statef;		    /**< XTI state flag */
	int prim;		    /**< last received XAP primitive */
	int qlen;		    /**< length of listen queue */
	int ocnt;		    /**< outstanding connection inidiciations */
	u_int8_t moredat;	    /**< more data in T_DATA_IND/T_OPTDATA_IND */
	u_int8_t moresdu;	    /**< more tsdu */
	u_int8_t moreexp;	    /**< more data in T_EXDATA_IND/T_OPTDATA_IND */
	u_int8_t moreedu;	    /**< more etsdu */
	u_int8_t moremsg;	    /**< more data with dis/con/rel message */
	int ctlmax;		    /**< maximum size of ctlbuf */
	char *ctlbuf;		    /**< ctrl part buffer */
	int datmax;		    /**< maximum size of datbuf */
	char *datbuf;		    /**< data part buffer */
	uint token;		    /**< acceptor id */
	struct strbuf ctrl;	    /**< ctrl part of received message */
	struct strbuf data;	    /**< data part of received message */
	struct ap_info info;	    /**< information structure */
	struct ap_env env;	    /**< environment structure */
};

static struct _ap_user *_ap_fds[OPEN_MAX] = { NULL, };

static pthread_rwlock_t __xap_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static inline int
__xap_lock_rdlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__xap_lock_wrlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline int
__xap_lock_unlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_unlock(rwlock);
}
static inline int
__xap_list_rdlock(void)
{
	return __xap_lock_rdlock(&__xap_fd_lock);
}
static inline int
__xap_list_wrlock(void)
{
	return __xap_lock_wrlock(&__xap_fd_lock);
}
static inline int
__xap_list_unlock(void)
{
	return __xap_lock_unlock(&__xap_fd_lock);
}
static inline int
__xap_user_rdlock(struct _xap_user *user)
{
	return __xap_lock_rdlock(&user->lock);
}
static inline int
__xap_user_wrlock(struct _xap_user *user)
{
	return __xap_lock_wrlock(&user->lock);
}
static inline int
__xap_user_unlock(struct _xap_user *user)
{
	return __xap_lock_unlock(&user->lock);
}

/** @internal
  * @brief	Release a locked XAP Library user instance structure.
  * @param arg	A pointer to the integer file descriptor of the XAP Library
  *		user structure.
  *
  * This function releases a locked XAP Library user structure.  The @arg
  * argument is a void pointer so that this function can be used as a cancel
  * deferral function.
  */
static void
__xap_ap_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct _xap_user *user = _xap_fds[fd];

	__xap_user_unlock(user);
	__xap_list_unlock(NULL);
	return;
}

/** @internal
  * @brief	Get a locked XAP Library user instance structure.
  * @param arg	A pointer to the integer file descriptor of the XAP Library
  *		user structure.
  *
  * This is a range-checked array lookup of the XAP Library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe operation.
  */
static __hot struct _xap_user *void
__xap_ap_getuser(int fd)
{
	struct _xap_user *user;
	int err;

	if (unlikely((err = __xap_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto badf;
	if (unlikely(!(user = _xap_fds[fd])))
		goto badf;
	if (unlikely((err = __xap_user_wrlock(user))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	aperrno = AP_INTERNAL;
	errno = err;
	__xap_list_unlock(NULL);
      tbadf:
	aperrno = AP_BADF;
	goto error;
      list_lock_error:
	aperrno = AP_INTERNAL;
	errno = err;
	goto error;
      error:
	return (NULL);
}

/** @brief		    Open an XAP instance.
  * @param provider	    Pathname of device to open(2) call.
  * @param oflags	    Flags to open(2) call.
  * @param ap_user_alloc    User buffer allocation callback.
  * @param ap_user_dalloc   User buffer deallocation callback.
  * @param aperrno_p	    Location to return error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_open().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  */
int
__xap_ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc,
	      ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
	struct _xap_user *user;
	int err, fd;

	if (oflags & ~(AP_NDELAY | AP_BUFFERS_ONLY))
		goto badflags;
	if ((ap_user_alloc == NULL) != (ap_user_dealloc == NULL))
		goto badalloc;
	if (!(user = (struct _xap_user *) malloc(sizeof(*user))))
		goto enomem;
	memset(user, 0, sizeof(*user));
	if ((fd = open(provider, oflags)) == -1)
		goto badopen;
	if (ioctl(fd, I_PUSH, "xapmod") != 0)
		goto badioctl;
	user->user_alloc = ap_user_alloc;
	user->user_dealloc = ap_user_dealloc;
	/* need to pick up all the capabilities from the stream */
	pthread_rwlock_init(&user->lock, NULL);
	/* destroyed for existing structure */
	_xap_fds[fd] = user;
	return (fd);
      badioctl:
	aperrno = errno;
	close(fd);
	free(user);
	errno = aperrno;
	goto error;
      badopen:
	aperrno = errno;
	free(user);
	errno = aperrno;
	goto error;
      enomem:
	aperrno = AP_INTERNAL;
	errno = ENOMEM;
	goto error;
      badalloc:
	aperrno = AP_BADALLOC;
	goto error;
      badflags:
	aperrno = AP_BADFLAGS;
	goto error;
      error:
	if (aperrno_p != NULL)
		*aperrno_p = aperrno;
	else
		errno = EFAULT;
	return (-1);
}

/** @brief		    The reentrant version of __xap_ap_open().
  * @param provider	    Pathname of device to open(2) call.
  * @param oflags	    Flags to open(2) call.
  * @param ap_user_alloc    User buffer allocation callback.
  * @param ap_user_dealloc  User buffer deallocation callback.
  * @param aperrno_p	    Location to return error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_open().
  *
  * This is a little different that most of the _r wrappers: we take a write
  * lock on the _ap_fds list so that we are able to add the new file descriptor
  * to the list.  This will block most other threads from performing functions
  * on the list, also, we must wait for a quiet period until all other
  * functions that read lock the list are not being used.  If you are sure that
  * the open will only be performed by one thread and that no other thread will
  * act on the file descriptor until open returnes, use the non-recursive
  * version.
  */
int
__xap_ap_open_r(const char *provider, int oflags, ap_ualloc_t ap_user_alloc,
		ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__xap_list_unlock, NULL);
	if ((err = __xap_list_wrlock()) == 0) {
		ret = __xap_ap_open(provider, oflags, ap_user_alloc, ap_user_dealloc, aperrno_p);
		__xap_list_unlock(NULL);
	} else {
		if (aperrno_p != NULL) {
			*aperrno_p = AP_INTERNAL;
			aperrno = AP_INTERNAL;
			errno = err;
		} else {
			errno = EFAULT;
		}
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
  * @param provider pathname of device to open(2) call.
  * @param oflags flags to open(2) call.
  * @param ap_user_alloc user buffer allocation callback.
  * @param ap_user_dalloc user buffer deallocation callback.
  * @param aperrno_p location to return error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_open_r().
  */
__asm__(".symver __xap_ap_open_r,ap_open@@XAP_1.0");

/** @internal
  * @brief Initialize the environment of an XAP Library instance.
  * @param user
  * @param env_file
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_init_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This function initializes an otherwise unintialized XAP instance identified
  * by @user.  Also, it may be used to set the values fo several writable
  * environment attributes with a single function call rather than using
  * ap_set_env() to set each attribute individually.
  *
  * If no environment exists when called, memory will be allocated for the
  * environment attributes in the calling process's data space and the
  * attributes will be set to their default values.  If the user wishes to
  * override the defaults for certain writable attributes, values for those
  * attributes may be specified in an intialization file.
  *
  * If an environment already exists when called, attributes will be assigned
  * values.  In this case, attributes will not automatically be set to their
  * default values.
  *
  * To set the environment attributes from values stored in a file, @env_file
  * must point to a null-terminated string that is the initialization file's
  * pathname.  An environment initialization file is generated by processing
  * ap_env_file file using the ap_osic(1) command.  Setting env_file to NULL
  * indicates that no values are to be taken from an environment intialization
  * file.
  */
static int
_xap_ap_init_env(struct _xap_user *user, const char *env_file, int flags, unsigned long *aperrno_p)
{
	struct ap_env *env;

	if (!(env = user->env)) {
		env = (struct ap_env *) malloc(sizeof(*env));
		if (!env)
			goto enomem;
		memset(env, 0, sizeof(*env));
		/* 
		 * FIXME:
		 *  set default values.
		 */
		user->env = env;
	}
	/* 
	 * FIXME:
	 * read env_file
	 */
	return (0);
      enomem:
	*aperrno_p = AP_NOMEM;
	goto error;
      error:
	return (-1);
}

/** @brief
  * @param fd
  * @param env_file
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_init_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the non-reentrant implementation of ap_init_env().
  */
int
__xap_ap_init_env(int fd, const char *env_file, int flags, unsigned long *aperrno_p)
{
	struct _xap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _xap_fds[fd]))
		goto badf;
	return _xap_ap_init_env(user, env_file, flags, aperrno_p);
      badf:
	*aperrno_p = AP_BADF;
	goto error;
      error:
	return (-1);
}

/** @brief
  * @param fd
  * @param env_file
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_init_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the reentrant implementation of ap_init_env().
  */
int
__xap_ap_init_env_r(int fd, const char *env_file, int flags, unsigned long *aperrno_p)
{
	struct _xap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__xap_ap_putuser, &fd);
	if ((user = __xap_ap_getuser(fd))) {
		ret = _xap_ap_init_env(user, env_file, flags, aperrno_p);
		__xap_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	*aperrno_p = aperrno;
	return (ret);
}

/** @fn int ap_init_env(int fd, const char *env_file, int flags, unsigned long *aperrno_p)
  * @param fd
  * @param env_file
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_init_env_r().
  *
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
__asm__(".symver __xap_ap_init_env_r,ap_init_env@@XAP_1.0");

/** @internal
  * @brief		Get an XAP environment attribute.
  * @param user		XAP user instance.
  * @param attr		Attribute to get.
  * @param val		Storage for retrieved attribute.
  * @param aperrno_p	Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  *
  */
int
_xap_ap_get_env(struct _xap_user *user, unsigned long attr, void *val,
		unsigned long *aperrno_p)
{
	if (val == NULL)
		goto noval;
	switch (attr) {
/* ACSE Attributes */
	case AP_ACSE_AVAIL:
		/** The available ACSE protocol versions.  Indicates which
		  * version of the ACSE protocol are currently available.  Not
		  * used in "X.410-1984" mode, see AP_MODE_SEL.  Defined bit
		  * values are as follows:
		  *
		  * AP_ACSEVER1  ACSE protocol version 1
		  *
		  * When set to the NULL bit-string (zero (0)), it idnicates
		  * that the XAP provider cannot provide an ACSE layer at all.
		  * In this case, the default value for AP_ACSE_SEL is NULL.
		  *
		  * Type unsigned long with bit values AP_ACSEVER1.  This
		  * attribute is readable always and writeable never. */
		goto readonly;
	case AP_ACSE_SEL:
		/** Indicates which version of the ACSE protocol has been
		  * selected for use with the current association.  Not used
		  * in "X.410-1984" mode, see AP_MODE_SEL.  For defined bit
		  * values, see AP_ACSE_AVAIL.  The default value is
		  * AP_ACSEVER1 provided that AP_ACSE_AVAIL includes
		  * AP_ACSEVER1, otherwise NULL.
		  *
		  * When this value is set to NULL (zero (0)), it indicates
		  * that the ACSE layer is to be set to NULL.  This means that
		  * ACSE layer primitive such as A-ASSOCIATE.Request map
		  * directly onto Presentation Layer primtiives such as
		  * P-CONNECT.Request.
		  *
		  * Type unsigned long with bit values AP_ACSEVER1 and default
		  * AP_ACSEVER1.  This attribute is readable always and
		  * writable only in states AP_UNBOUND and AP_IDLE. */
		goto uscalar;
	case AP_AFU_AVAIL:
		/** The available ACSE optional functional units.  Indicates
		  * which optional ACSE functional units are currently
		  * available.  Not used in "X.410-1984" mode.  Defined bit
		  * values are:
		  *
		  * AP_ACSE_AUTH	Authentication FU.
		  * AP_ACSE_CNTXNEG	Application/ASO Context Negotiation FU.
		  * AP_ACSE_HLASSOC	Higher Level Association FU.
		  * AP_ACSE_NESTASSOC	Nested Association FU.
		  *
		  * Type unsigned long with bit values NULL.  This attribute
		  * is readable always and writable never. */
		goto readonly;
	case AP_AFU_SEL:
		/** The selected ACSE optional functional units.  Indicates
		  * which optional ACSE functional units have been requested
		  * for use over the current association.  Not use in
		  * X.410-1984 mode.  Defined bit values are listed under
		  * AP_AFU_AVAIL.  The default value is the NULL bit-string
		  * indicating that only the non-negotiable kernel functional
		  * unit is present.
		  *
		  * Type unsigned long with bit values NULL and default NULL.
		  * This attribute is readable always and writable only in
		  * states AP_UNBOUND, AP_IDLE and AP_WASSOCrsp_ASSOCind. */
		goto uscalar;
	case AP_CNTX_NAME:
	{
		ap_objid_t *c;
		int olen = 0;
		char *oid;

		if ((c = (ap_objid_t *)val.v) == NULL)
			goto noval;
		if (c->length > 0) {
			olen = c->length;
			olen += _T_ALIGN_SIZE - 1;
			olen &= ~(_T_ALIGN_SIZE - 1);
			if (c->length <= AP_MAXOBJBUF)
				oid = (char *)c->short_buf;
			else
				oid = (char *)c->long_buf;
			strncpy(req->opt.buf + req->opt.len, oid, olen);
			req->opt.len += olen;
			hreq->len += c->length;
		}
		break;
	}
	case AP_ROLE_ALLOWED:
		goto uscalar;
	case AP_ROLE_CURRENT:
		goto uscalar;
	case AP_CLG_APT:
	case AP_CLD_APT:
	case AP_RSP_APT:
	{
		ap_apt_t *a;
		int alen = 0;

		if ((a = (ap_apt_t *)val.v) == NULL)
			goto noval;
		if (a->size > 0) {
			alen = a->size;
			alen += _T_ALIGN_SIZE - 1;
			alen &= ~(_T_ALIGN_SIZE - 1);
			strncpy((hreq+1), a->udata, alen);
			req->opt.len += alen;
			hreq->len += a->size;
		}
		break;
	}
	case AP_CLG_AEQ:
	case AP_CLD_AEQ:
	case AP_RSP_AEQ:
	{
		ap_aeq_t *a;
		int alen = 0;

		if ((a = (ap_apt_t *)val.v) == NULL)
			goto noval;
		if (a->size > 0) {
			alen = a->size;
			alen += _T_ALIGN_SIZE - 1;
			alen &= ~(_T_ALIGN_SIZE - 1);
			strncpy((hreq+1), a->udata, alen);
			req->opt.len += alen;
			hreq->len += a->size;
		}
		break;
	}
	case AP_CLG_APID:
	case AP_CLD_APID:
	case AP_RSP_APID:
	{
		ap_aei_api_id_t *a;
		int alen = 0;

		if ((a = (ap_apt_t *)val.v) == NULL)
			goto noval;
		if (a->size > 0) {
			alen = a->size;
			alen += _T_ALIGN_SIZE - 1;
			alen &= ~(_T_ALIGN_SIZE - 1);
			strncpy((hreq+1), a->udata, alen);
			req->opt.len += alen;
			hreq->len += a->size;
		}
		break;
	}
	case AP_CLG_AEID:
	case AP_CLD_AEID:
	case AP_RSP_AEID:
	{
		ap_aei_api_id_t *a;
		int alen = 0;

		if ((a = (ap_apt_t *)val.v) == NULL)
			goto noval;
		if (a->size > 0) {
			alen = a->size;
			alen += _T_ALIGN_SIZE - 1;
			alen &= ~(_T_ALIGN_SIZE - 1);
			strncpy((hreq+1), a->udata, alen);
			req->opt.len += alen;
			hreq->len += a->size;
		}
		break;
	}
	case AP_STATE:
		goto uscalar;
/* Presentation Attributes */
	case AP_PRES_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_PRES_SEL:
		/* FIXME: copy it */
		break;
	case AP_PFU_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_PFU_SEL:
		/* FIXME: copy it */
		break;
	case AP_BIND_PADDR:
		/* FIXME: copy it */
		break;
	case AP_LCL_PADDR:
		/* FIXME: copy it */
		break;
	case AP_REM_PADDR:
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
	case AP_PCDL:
		/* FIXME: copy it */
		break;
	case AP_PCDRL:
		/* FIXME: copy it */
		break;
/* Session Attributes */
	case AP_SESS_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_SESS_SEL:
		/* FIXME: copy it */
		break;
	case AP_SESS_OPT_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_SFU_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_SFU_SEL:
		/* FIXME: copy it */
		break;
	case AP_QOS:
		/* FIXME: copy it */
		break;
	case AP_CLG_CONN_ID:
	case AP_CLD_CONN_ID:
	{
		ap_conn_id_t *c;
		int clen = 0;
		unsigned char *wptr, *data;

		if ((c = (ap_conn_id_t *)val.v) == NULL)
			goto noval;
		wptr = (unsigned char *)(hreq + 1);
		clen = c->user_ref.length;
		data = c->user_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		clen = c->comm_ref.length;
		data = c->comm_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		clen = c->addtl_ref.length;
		data = c->addtl_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		break;
	}
	case AP_OLD_CONN_ID:
	{
		ap_old_conn_id_t *c;
		int clen = 0;
		unsigned char *wptr, *data;

		if ((c = (ap_old_conn_id_t *)val.v) == NULL)
			goto noval;
		wptr = (unsigned char *)(hreq + 1);
		clen = c->clg_user_ref.length;
		data = c->clg_user_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		clen = c->cld_user_ref.length;
		data = c->cld_user_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		clen = c->comm_ref.length;
		data = c->comm_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		clen = c->addtl_ref.length;
		data = c->addtl_ref.data;
		*(unsigned char *)wptr = clen;
		req->opt.len++;
		hreq->len++;
		wptr++;
		strncpy(wptr, data, clen);
		req->opt.len += clen;
		hreq->len += clen;
		wptr += clen;
		break;
	}
	case AP_TOKENS_AVAIL:
		goto readonly;
	case AP_TOKENS_OWNED:
		goto readonly;
	case AP_MSTATE:
		/** If the XAP instance is awaiting additional data from the
		  * user (the last ap_snd(3) call had the AP_MORE bit set),
		  * the AP_SNDMORE bit in AP_MSTATE is set.  If there is more
		  * user data for the current service (the last call to
		  * ap_rcv(3) returned with the AP_MORE bit set) then
		  * AP_RCVMORE bit is set.  (Note that it is possible for both
		  * bits to be set.)
		  *
		  * The segmentation state.  Type unsigned long with bit
		  * values AP_SNDMORE and AP_RCVMORE.  This attribute is
		  * readable always and writable never. */
		goto readonly;

	case AP_INIT_SYNC_PT:
		/* FIXME: copy it */
		break;
	case AP_LIB_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_LIB_SEL:
		/* FIXME: copy it */
		break;
	case AP_MODE_AVAIL:
		/** The AP_MODE_AVAIL attribute indicates the available modes
		  * of operation for XAP and Provider.  The modes that may be
		  * supported are normal (AP_NORMAL_MODE) and X.410-1984
		  * (AP_X410_MODE).  The available modes.  This attribute
		  * represents the Presentation Mode selections supported by
		  * the local presentation-entity.  Type unsigned long with
		  * bit values AP_NORMAL_MODE, AP_X410_MODE, AP_ROSE_MODE and
		  * AP_TP_MODE.  This attribute is readable always and
		  * writable never.
		  *
		  * The AP_MODE_AVAIL attribute indicates the available modes
		  * of operation for XAP and provider.  The modes that may be
		  * supported are those of XAP plus TP mode (AP_TP_MODE).  See
		  * also the XAP definition of AP_MODE_AVAIL.
		  *
		  * An additional flag, AP_ROSE_MODE is defined for this
		  * environment attribute.  If set, the service provider
		  * specified in the call to ap_open(3) is capable of
		  * providing the ROSE service.  If this flag is set, the
		  * equivalent flag in AP_MODE_SEL may be set to use the
		  * XAP-ROSE features of the service provider. */
		goto uscalar;
	case AP_MODE_SEL:
		goto uscalar;
	case AP_FLAGS:
		/* FIXME: copy it */
		break;
	case AP_COPYENV:
		goto uscalar;
	case AP_DIAGNOSTIC:
	{
		ap_diag_t *d;
		t_diag_t *t;
		int slen = 0;

		d = (ap_diag_t *)val.v;
		if (d->error != NULL) {
			slen = strnlen(d, 256);
			slen += _T_ALIGN_SIZE - 1;
			slen &= ~(_T_ALIGN_SIZE - 1);
		}
		t = (t_diag_t *)(opt->buf + opt->len);
		opt->len += sizeof(*t);
		t->rsn = d->rsn;
		t->evt = d->evt;
		t->src = d->src;
		if (slen) {
			strncpy(t->error, d->error, slen);
			opt->len += slen;
		}
		break;
	}
	case AP_INIT_TOKENS:
		/* FIXME: copy it */
		break;
	case AP_OPT_AVAIL:
		/* FIXME: copy it */
		break;
	case AP_QLEN:
		goto uscalar;
	default:
		goto badattr;
uscalar:
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
		break;
	}
	return (0);
      badattr:
	*aperrno_p = AP_BADATTR;
	goto error;
      noval:
	*aperrno_p = AP_NOVAL;
	goto error;
      error:
	return (-1);
}

/** @brief		Get an XAP environment attribute.
  * @param fd		XAP user instance.
  * @param attr		Attribute to get.
  * @param val		Storage for retrieved attribute.
  * @param aperrno_p	Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  *
  * This is the non-recursive version of ap_get_env().
  */
int
__xap_ap_get_env(int fd, unsigned long attr, void *val,
		 unsigned long *aperrno_p)
{
	struct _xap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _xap_fds[fd]))
		goto badf;
	return _xap_ap_get_env(user, attr, val, aperrno_p);
      badf:
	*aperrno_p = AP_BADF;
	return (-1);
}

/** @brief		Get an XAP environment attribute.
  * @param fd		XAP user instance.
  * @param attr		Attribute to get.
  * @param val		Storage for retrieved attribute.
  * @param aperrno_p	Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  *
  * This is the recursive version of ap_get_env().
  */
int
__xap_ap_get_env_r(int fd, unsigned long attr, void *val,
		   unsigned long *aperrno_p)
{
	struct _xap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__xap_ap_putuser, &fd);
	if ((user = __xap_ap_getuser(fd))) {
		ret = _xap_ap_get_env(user, attr, val, aperrno_p);
		__xap_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_get_env(int fd, unsigned long attr, void *val, unsigned long *aperrno_p)
  * @brief		Get an XAP environment attribute.
  * @param fd		XAP user instance.
  * @param attr		Attribute to get.
  * @param val		Storage for retrieved attribute.
  * @param aperrno_p	Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_get_env().
  */
__asm__(".symver __xap_ap_get_env_r,ap_get_env@@XAP_1.0");

/** @internal
  * @brief		Set an XAP environment attribute.
  * @param user		XAP user instance.
  * @param attr		Attribute to set.
  * @param val		Storage for setting attribute.
  * @aperrno_p		Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the internal implemetnation of ap_set_env().  This function working
  * with the cooperative xapmod STREAMS module uses the XTI library to perform
  * as simple options.
  */
int
_xap_ap_set_env(struct _xap_user *user, unsigned long attr, ap_val_t val,
		unsigned long *aperrno_p)
{
	struct t_optmgmt *req, *ret;
	struct t_opthdr *hreq, *hret;
	int fd = user->fd;

	if (vall == NULL)
		goto noval;
	if ((req = (struct t_optmgmt *)t_alloc(fd, T_OPTMGMT, T_OPT)) == NULL)
		goto badalloc;
	req->flags = T_NEGOTIATE;
	req->opt.len = sizeof(*hreq);
	hreq = (struct t_opthdr *)req->opt.buf;
	hreq->len = sizeof(*hreq);
	hreq->level = ((attr>>16)&0xffff);
	hreq->name = ((attr>>0)&0xffff);
	hreq->status = T_SUCCESS;
	if ((ret = (struct t_optmgmt *)t_alloc(fd, T_OPTMGMT, T_OPT)) == NULL)
		goto badalloc2;
	ret->flags = T_NEGOTIATE;
	ret->opt.len = -1;
	hret = (struct t_opthdr *)ret->opt.buf;
	switch (attr) {
	case AP_ACSE_AVAIL:
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
		break;
	case AP_ACSE_SEL:
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
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
	case AP_CLG_APT:
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
		/** The AP_LIB_AVAIL attribute indicates which version of XAP
		  * are available to the user.  The available library version.
		  * Type unsigned long with bit values AP_LIBVER1.  This
		  * attribute is readable always and writable never. */
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
		break;
	case AP_LIB_SEL:
		/** The AP_LIB_SEL attribute is used to indicate which version
		  * of XAP is used.  This attribute must be set before any
		  * other attributes are set or any primitives are issued.  The
		  * selected library version.  Type unsigned long with bit
		  * values AP_LIBVER1 and default none.  This attribute is
		  * readable aslways and writable only in states AP_UNBOUND. */
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
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
	case AP_FLAGS:
		/* FIXME: set it */
		break;
	case AP_TOKENS_AVAIL:
		/* FIXME: set it */
		break;
	case AP_TOKENS_OWNED:
		/* FIXME: set it */
		break;
	case AP_AFU_AVAIL:
		/* FIXME: set it */
		break;
	case AP_AFU_SEL:
		/* FIXME: set it */
		break;
	case AP_COPYENV:
		/* FIXME: set it */
		break;
	case AP_DIAGNOSTIC:
		/* FIXME: set it */
		break;
	case AP_INIT_TOKENS:
		/* FIXME: set it */
		break;
	case AP_OLD_CONN_ID:
		/* FIXME: set it */
		break;
	case AP_OPT_AVAIL:
		/** The AP_OPT_AVAIL attribute is used to indicate what
		  * optional functionality is supported in the underlying
		  * protocol implementation.  AP_XXXX_WILD indicates whether
		  * address wildcarding is supported at the specified level.
		  * (XXXX can be NSAP, TSEL, SSEL or PSEL.)
		  *
		  * AP_NSAP_WILD  NSAP can be wildcarded.
		  * AP_TSEL_WILD  TSEL can be wildcarded.
		  * AP_SSEL_WILD  SSEL can be wildcarded.
		  * AP_PSEL_WILD  PSEL can be wildcarded.
		  *
		  * The available options.  Type unsigned long with bit values
		  * AP_NSAP_WILD, AP_TSEL_WILD, AP_SSEL_WILD and AP_PSEL_WILD.
		  * This attribute is readable always and writable never.  */
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
		break;
	case AP_QLEN:
		/* FIXME: set it */
		break;
	case AP_QOS:
		/* FIXME: set it */
		break;
	case AP_SESS_OPT_AVAIL:
		/* FIXME: set it */
		break;
	case AP_RO_FAC_AVAIL:
		/** The AP_RO_FAC_AVAIL attribute is used to indicate the
		  * availability of factilities in the XAP-ROSE provider. The
		  * attribute is bit significant, and the following bit values
		  * are defined.
		  *
		  * AP_RO_BIND
		  *	BIND/UNBIND primitives.
		  *
		  * When a bit valud is set in the attribute value the
		  * corresponding facility is available.  */
		goto readonly;
		req->opt.len += sizeof(t_uscalar_t);
		hreq->len += sizeof(t_uscalar_t);
		*(t_uscalar_t *)(hreq + 1) = val.l;
		break;
	case AP_RO_PCI_LIST:
		/* FIXME: set it */
		break;
	default:
		goto badattr;
	}


	t_free(req, T_OPTMGMT);
	t_free(ret, T_OPTMGMT);

	return (0);
      badattr:
	*aperrno_p = AP_BADATTR;
	goto error;
      noval:
	*aperrno_p = AP_NOVAL;
	goto error;
      errror:
	return (-1);
}

/** @brief		Set an XAP environment attribute.
  * @param fd		XAP user instance.
  * @param attr		Attribute to set.
  * @param val		Storage for setting attribute.
  * @aperrno_p		Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the non-recursive (non-thread-safe) version of ap_set_env().
  */
int
__xap_ap_set_env(int fd, unsigned long attr, ap_val_t val,
		 unsigned long *aperrno_p)
{
	struct _xap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _xap_fds[fd]))
		goto badf;
	return _xap_ap_set_env(user, attr, val, aperrno_p);
      badf:
	*aperrno_p = AP_BADF;
	return (-1);
}

/** @brief		Set an XAP environment attribute.
  * @param fd		XAP user instance.
  * @param attr		Attribute to set.
  * @param val		Storage for setting attribute.
  * @aperrno_p		Location for returned error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the recursive (thread-safe) version of ap_set_env().
  */
int
__xap_ap_set_env_r(int fd, unsigned long attr, ap_val_t val,
		   unsigned long *aperrno_p)
{
	struct _xap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__xap_ap_putuser, &fd);
	if ((user = __xap_ap_getuser(fd))) {
		ret = _xap_ap_set_env(user, attr, val, aperrno_p);
		__xap_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int ap_set_env(int fd, unsigned long attr, ap_val_t val, unsigned long *aperrno_p)
  * @param fd
  * @param attr
  * @param val
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_set_env().
  */
__asm__(".symver __xap_ap_set_env,ap_set_env@@XAP_1.0");

/** @internal
  * @brief Restore XAP Library instance environment from file.
  * @param user
  * @param savef
  * @param oflags
  * @param ap_user_alloc
  * @param ap_user_dealloc
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  */
int
_xap_ap_restore(struct _xap_user *user, FILE *savef, int oflags, ap_ualloc_t ap_user_alloc,
		ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
	/* FIXME: restore from stream (perhaps using scanf?) */
}

/** @brief		    Restore XAP environment from file.
  * @param fd		    File descriptor of XAP instance.
  * @param savef	    File pointer of file from which to restore.
  * @param oflags	    Restoration flags.
  * @param ap_user_alloc    User buffer allocation callback.
  * @param ap_user_dealloc  User buffer deallocation callback.
  * @param aperrno_p	    Location to store error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the non-recursive version of ap_restore().
  */
int
__xap_ap_restore(int fd, FILE *savef, int oflags, ap_ualloc_t ap_user_alloc,
		 ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
	struct _xap_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto badf;
	if (!(user = _xap_fds[fd]))
		goto badf;
	return _xap_ap_restore(user, savef, ofalgs, ap_user_alloc, ap_user_dealloc, aperrno_p);
      badf:
	*aperrno_p = AP_BADF;
	return (-1);
}

/** @brief		    Restore XAP environment from file.
  * @param fd		    File descriptor for XAP instance.
  * @param savef	    File pointer of file from which to restore.
  * @param oflags	    Restoration flags.
  * @param ap_user_alloc    User buffer allocation callback.
  * @param ap_user_dealloc  User buffer deallocation callback.
  * @param aperrno_p	    Location to store error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This is the recursive version of ap_restore().
  */
int
__xap_ap_restore_r(int fd, FILE *savef, int oflags, ap_ualloc_t ap_user_alloc,
		 ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
	struct _xap_user *user;
	int ret = -1;

	pthread_cleanup_push_defer_np(__xap_ap_putuser, &fd);
	if ((user = __xap_ap_getuser(fd))) {
		ret = _xap_ap_restore(user, savef, ofalgs, ap_user_alloc, ap_user_dealloc, aperrno_p);
		__xap_ap_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @brief		    Restore XAP environment from file.
  * @fn int ap_restore(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
  * @param fd		    File descriptor for XAP instance.
  * @param savef	    File pointer of file from which to restore.
  * @param oflags	    Restoration flags.
  * @param ap_user_alloc    User buffer allocation callback.
  * @param ap_user_dealloc  User buffer deallocation callback.
  * @param aperrno_p	    Location to store error code.
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_restore().
  */
__asm__(".symver __xap_ap_restore_r,ap_restore@@XAP_1.0");

/** @internal
  * @brief Set the state of the endpoint to a constant state.
  * @param user a pointer to the _ap_user structure for this enpoint.
  * @param state the constant state value to set.
  *
  * This inline function sets the state of the ACSE/Presentation enpoint in the
  * user structure to the constant state specified.  This function is an inline
  * accepting a constant so that the compiler can inline just the appropriate
  * case value.
  */
static inline void
__xap_u_setstate_const(struct _ap_user *user, const int state)
{
	user->statef = (1 << state);
	switch (state) {
	case TS_UNBND:
	case TS_WACK_BREQ:
		user->state = AP_UNBOUND;
		break;
	case TS_WACK_UREQ:
	case TS_IDLE:
	case TS_WACK_CREQ:
		user->state = AP_IDLE;
		break;
	case TS_WCON_CREQ:
	case TS_WACK_DREQ6:
	user->state = AP_}
}

/** @brief Bind an XAP instance to a Presentation Address.
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_bind().
  *
  * This function associates the Presentation Address stored in the
  * %AP_BIND_PADDR environment variable with the XAP instance specified by @fd.
  * Upon successful completion the service provider may begin enqueuing
  * incoming associations or sending outbound association requests.  All
  * necessary environment variables (e.g. %AP_BIND_PADDR, %AP_ROLE_ALLOWED)
  * should be set prior to ap_bind().
  *
  * When this function is called, an attempt may be made to bind to the
  * specified address.  As a part of the bind procedure, an authorization check
  * may be performed to verify that all of the processes that share this XAP
  * instance are authorizsed to use the new address.  If all are authorised to
  * do so, the bind request will succeed and the XAP instance may be used to
  * send (receive) primitives from (addressed to) the new address.
  * Successfully calling this function causes the state of the XAP instance to
  * move from %AP_UNBOUND to %AP_IDLE.
  *
  * Some implementations may perform no authorisation checking.  In this case,
  * the [%AP_ACCES] error response will not be generated.  Other
  * implementations may defer binding and authorisation until an %A_ASSOC_REQ
  * or %A_ASSOC_RSP primitive is issued.  In this case, if the authorisation
  * check fails, the %AP_ACCES error will be returned by ap_snd().  THe local
  * address can then be changed to an acceptable value and the primitive
  * reissued, or the connection can be closed.
  *
  * An instance can be bound to a presentation address only if all of the
  * processes that share it are authorised to use the requested address.
  * Consequently, when an attempt is made to bind an address, the effective
  * UIDs aof all of the processes that share this isntance of XAP may be
  * checked against the list of users allowed to use the requested address.  If
  * all are authorised to use the address, ap_bind() succeeds and the isntance
  * is bound to the specified presentation address.  On the other hand, if any
  * of the processes is not authorised to use the requested address, ap_bind()
  * fails and the instance remains unbound.
  *
  * @aperrno_p must be set to point to a location whcih will be used to carry
  * an error code back to the user.
  *
  * @par Return: Upon successful completion, a value of 0 is returned.
  * Otherwise, a value of -1 is returned and the location pointed to by
  * @aperrno_p is set to indicate the error.
  *
  * @par Errors:
  *	[%AP_ACCES]	Request to bind to specified address denied.
  *	[%AP_BADF]	The @fd parameter does not identify an XAP instance.
  *	[%AP_BADNSAP]	The format of the NSAP portion fo the Presentation
  *	                Address is not supported.
  *	[%AP_NOENV]	There is no XAP environment associated with @fd.
  */
int
__xap_ap_bind(int fd, unsigned long *aperrno_p)
{
}

/** @fn int ap_bind(int fd, unsigned long *aperrno_p)
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_bind().
  */
__asm__(".symver __xap_ap_bind,ap_bind@@XAP_1.0");

/** @brief Close an XAP instance.
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_close().
  *
  * This function frees the resources allocated to support the isntance of XAP
  * identified by @fd.  (That is, within the calling process.)
  *
  * If the last close fot he XAP instance occurs while an association is still
  * active, the association (and any primitive that is being sent or received in
  * multiple parts using the %AP_MORE bit) is aborted before the resources are
  * released.
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_close(int fd, unsigned long *aperrno_p)
{
	int err;

	if ((err = close(fd)) != 0 && aperrno_p != NULL)
		if ((*aperrno_p = errno) == EBADF)
			*aperrno_p = AP_BADF;
	return (err);
}

/** @fn int ap_close(int fd, unsigned long *aperrno_p)
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_close().
  */
__asm__(".symver __xap_ap_close,ap_close@@XAP_1.0");


/* *INDENT-OFF* */
const char *__xap_ap_errlist[] = {
/*
TRANS AP_NOERROR, AP_SUCCESS - (0) [Note: The text here should be a short string
TRANS of about less than or equal to 64 characters in length.]
TRANS
TRANS No error is indicated in the aperrno variable.  The last operation was a
TRANS success.  aperrno will not be set to this value (zero) by the library,
TRANS the user must set aperrno to zero before the XAP library call and when
TRANS the call is successful, the aperrno value will be unaffected.  There is
TRANS no requirement that this value be set after a successful call, and calls
TRANS are even permitted to change aperrno to some other value, when the call
TRANS is actually successful.
 */
	gettext_noop("no error");
/*
TRANS AP_ACCES - (1) [Note: The text here should be a short string of about less
TRANS than or equal to 64 characters in length.]
 */
	gettext_noop("Request to bind to specified address denied.");
/*
TRANS AP_AGAIN - (2) [Note: The text here should be a short string of about less
TRANS than or equal to 64 characters in length.]
 */
	gettext_noop("Request not completed.");
/*
TRANS AP_BADATTRVAL - (3) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Bad value for environment attribute.");
/*
TRANS AP_BADCD_ACT_ID - (4) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field value invalid: act_id.");
/*
TRANS AP_BADCD_DIAG - (5) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: diag.");
/*
TRANS AP_BADCD_EVT - (6) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: event.");
/*
TRANS AP_BADCD_OLD_ACT_ID - (7) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: old_act_id.");
/*
TRANS AP_BADCD_OLD_CONN_ID - (8) [Note: The text here should be a short string
TRANS of about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: old_conn_id.");
/*
TRANS AP_BADCD_RES - (9) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: res.");
/*
TRANS AP_BADCD_RES_SRC - (10) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: res_src.");
/*
TRANS AP_BADCD_RESYNC_TYPE - (11) [Note: The text here should be a short string
TRANS of about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: resync_type.");
/*
TRANS AP_BADCD_RSN - (12) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: rsn.");
/*
TRANS AP_BADCD_SRC - (13) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: src.");
/*
TRANS AP_BADCD_SYNC_P_SN - (14) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: sync_p_sn.");
/*
TRANS AP_BADCD_SYNC_TYPE - (15) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: sync_type.");
/*
TRANS AP_BADCD_TOKENS - (16) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("Cdata field valud invalid: tokens.");
/*
TRANS AP_BADENC - (17) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("xxx");
/*
TRANS AP_BADENV - (18) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("A mandatory attribute is not set.");
/*
TRANS AP_BADF - (19) [Note: The text here should be a short string of about less
TRANS than or equal to 64 characters in length.]
 */
	gettext_noop("Not a presentation service endpoint.");
/*
TRANS AP_BADFLAGS - (20) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("The specified combination of flags is invalid.");
/*
TRANS AP_BADFREE - (21) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Could not free structure members.");
/*
TRANS AP_BADKIND - (22) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Unknown structure type.");
/*
TRANS AP_BADLSTATE - (23) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Instance in bad state for that command.");
/*
TRANS AP_BADPARSE - (24) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Attribute parse failed.");
/*
TRANS AP_BADPRIM - (25) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Unrecognized primitive from user.");
/*
TRANS AP_BADREF - (26) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("xxx");
/*
TRANS AP_BADRESTR - (27) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Attributes not restored doe to more bit on.");
/*
TRANS AP_BADROLE - (28) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Request invalid due to value of AP_ROLE.");
/*
TRANS AP_BADSAVE - (29) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Attributes not saved dues to more bit on.");
/*
TRANS AP_BADSAVEF - (30) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Invalid FILE pointer.");
/*
TRANS AP_BADUBUF - (31) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Bad length for user data.");
/*
TRANS AP_HANGUP - (32) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Association close or aborted.");
/*
TRANS AP_INTERNAL - (33) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("An internal error occured.");
/*
TRANS AP_LOOK - (34) [Note: The text here should be a short string of about less
TRANS than or equal to 64 characters in length.]
 */
	gettext_noop("A pending event requires attention.");
/*
TRANS AP_NOATTR - (35) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("No such attribute.");
/*
TRANS AP_NOENV - (36) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("No environment for that fd.");
/*
TRANS AP_NOMEM - (37) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Could not allocate enough memory.");
/*
TRANS AP_NOREAD - (38) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Attribute is not readable.");
/*
TRANS AP_NOSET - (39) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("xxx");
/*
TRANS AP_NOWRITE - (40) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Attribute is not writable.");
/*
TRANS AP_PDUREJ - (41) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Invalid PDU rejected.");
/* The following are XAP only (not APLI). */
/*
TRANS AP_AGAIN_DATA_PENDING - (42) [Note: The text here should be a short string
TRANS of about less than or equal to 64 characters in length.]
 */
	gettext_noop("XAP was unable to complete the requested action. Try again. There is an event available for the user to receive.");
/*
TRANS AP_BADALLOC - (43) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("The ap_user_alloc/ap_user_dalloc argument combination was invalid.");
/*
TRANS AP_BADASLSYN - (44) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("The transfer syntaxes proposed for the ACSE syntax are not supported.");
/*
TRANS AP_BADDATA - (45) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("User data not allowed on this service.");
/*
TRANS AP_BADNSAP - (46) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("The format of the NSAP portion of the Presentation Address is not supported.");
/*
TRANS AP_DATA_OVERFLOW - (47) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("User data and presentation service pci exceeds 512 bytes on session V1 or the length of user data exceeds a locally defined limit as stated in the CSQ.");
/*
TRANS AP_NOBUF - (48) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("Could not allocate enough buffers.");
/*
TRANS AP_NODATA - (49) [Note: The text here should be a short string of about
TRANS less than or equal to 64 characters in length.]
 */
	gettext_noop("An attempt was made to send a primitive with no user data.");
/*
TRANS AP_NO_PRECEDENCE - (50) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("The resynchronization requested by the local user does not have precedence over the on requested by the remote user.");
/*
TRANS AP_NOT_SUPPORTED - (51) [Note: The text here should be a short string of
TRANS about less than or equal to 64 characters in length.]
 */
	gettext_noop("The action requested is not supported by this implementation of XAP.");
/*
TRANS AP_SUCCESS_DATA_PENDING - (52) [Note: The text here should be a short
TRANS string of about less than or equal to 64 characters in length.]
 */
	gettext_noop("The requested action was completed successfully.  There is an event available for the user to receive.");
};
/* *INDENT-ON* */

const char *
__xap_ap_strerror(int errnum)
{
	if (0 < errnum && errnum < AP_ERRMAX)
		return gettext(__xap_ap_errlist[errnum]);
	return gettext(__xap_ap_errlist[AP_ERRMAX]);
}

/** @brief
  * @param apperno
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_error().
  *
  * Note that XAP errors in the header file are overloaded with other errors in
  * the same integer.  This is done by making bits 8 though 15 equal to AP_ID.
  * Regular errno style system errors can be included here too:
  */
const char *
__xap_ap_error(unsigned long aperrno)
{
	switch ((aperrno >> 8) & 0xff) {
	case AP_ID:
	case 0:
	default:
	}
}

/** @fn const char *ap_error(unsigned long aperrno)
  * @param apperno
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_error().
  */
__asm__(".symver __xap_ap_error,ap_error@@XAP_1.0");

/** @brief
  * @param aperrno
  * @param buffer
  * @param nbytes
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_error_r().
  */
int
__xap_ap_error_r(unsigned long aperrno, char *buffer, size_t nbytes)
{
	int retval = 0;

	switch (aperrno >> 8) {
	case AP_ID:
	default:
		if ((retval = __xap_ap_strerror_r(aperrno, buffer nbyte)) == -1) {
			/* oops, double error */
		}
	case 0:
		if ((retval = strerror_r(aperrno, buffer, nbyte)) == -1) {
			/* oops, double error */
		}
	}
	return (retval);
}

/** @fn ap_error_r(unsigned long aperrno, char *buffer, size_t nbytes)
  * @param aperrno
  * @param buffer
  * @param nbytes
  * @version XAP_1.0
  * @par Alias:
  * This symbol is a strong alias of __xap_ap_error_r().
  */
__asm__(".symver __xap_ap_error_r,ap_error_r@@XAP_1.0");

/** @brief
  * @param fd
  * @param kind
  * @param val
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_free().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_free(int fd, unsigned long kind, void *val, unsigned long *aperrno_p)
{
}

/** @fn int ap_free(int fd, unsigned long kind, void *val, unsigned long *aperrno_p)
  * @param fd
  * @param kind
  * @param val
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_free().
  */
__asm__(".symver __xap_ap_free,ap_free@@XAP_1.0");

/** @brief
  * @param fd
  * @param request
  * @param argument
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_ioctl().
  *
  * The implementation is a direct translation to I_SETSIG and I_GETSIG.
  * Unfortunately, the AP_POLLxxx bits are poll(2s) bits instead of I_SETSIG(7)
  * bits and the call and return values must be bit-translated.  Also, we do
  * not want to necessarily set AP_SETPOLL to I_SETSIG and AP_GETPOLL to
  * I_GETSIG, so those are translated too.
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This function exists largely because the ISO standards body did not want to
  * make the XAP library depend upon POSIX system calls, or at least that the
  * underlying implementation was not necessarily STREAMS.
  */
int
__xap_ap_ioctl(int fd, int request, ap_val_t argument, unsigned long *aperrno_p)
{
	int events = 0;
	int flags = 0;
	int err;

	switch (request) {
	case AP_SETPOLL:
		/* translate POLL bis to SIG bits */
		if (argument.l & AP_POLLIN)
			events |= S_INPUT;
		if (argument.l & AP_POLLPRI)
			events |= S_HIPRI;
		if (argument.l & AP_POLLOUT)
			events |= S_OUTPUT;
		if (argument.l & AP_POLLRDNORM)
			events |= S_RDNORM;
		if (argument.l & AP_POLLRDBAND)
			events |= S_RDBAND;
		if (argument.l & AP_POLLWRNORM)
			events |= S_WRNORM;
		if (argument.l & AP_POLLWRBAND)
			events |= S_WRBAND;
		if (argument.l & AP_POLLERR)
			events |= S_ERROR;
		if (argument.l & AP_POLLHUP)
			events |= S_HANGUP;
		if (argument.l & AP_POLLMSG)
			events |= S_MSG;
		if (argument.l & AP_POLLURG)
			events |= S_BANDURG;
		if ((err = ioctl(fd, I_SETSIG, events)) == -1)
			break;
		return (0);
	case AP_GETPOLL:
		if ((err = ioctl(fd, I_GETSIG, &events)) == -1)
			break;
		if (events & S_INPUT)
			flags |= AP_POLLIN;
		if (events & S_HIPRI)
			flags |= AP_POLLPRI;
		if (events & S_OUTPUT)
			flags |= AP_POLLOUT;
		if (events & S_RDNORM)
			flags |= AP_POLLRDNORM;
		if (events & S_RDBAND)
			flags |= AP_POLLRDBAND;
		if (events & S_WRNORM)
			flags |= AP_POLLWRNORM;
		if (events & S_WRBAND)
			flags |= AP_POLLWRBAND;
		if (events & S_ERROR)
			flags |= AP_POLLERR;
		if (events & S_HANGUP)
			flags |= AP_POLLHUP;
		if (events & S_MSG)
			flags |= AP_POLLMSG;
		if (events & S_BANDURG)
			flags |= AP_POLLURG;
		*(long *) argument.v = flags;
		return (0);
	default:
		errno = EINVAL;
		break;
	}
	switch (errno) {
	case EBADF:
	case ENOSTR:
	case ENOTTY:
	case ENODEV:
		if (aperrno_p != NULL)
			*aperrno_p = AP_BADF;
		break;
	case EOPNOTSUPP:
		if (aperrno_p != NULL)
			*aperrno_p = AP_NOT_SUPPORTED;
		break;
	default:
		if (aperrno_p != NULL)
			*aperrno_p = errno;
		break;
	}
	return (-1);
}

/** @fn int ap_ioctl(int fd, int request, ap_val_t argument, unsigned long *aperrno_p)
  * @param fd
  * @param request
  * @param argument
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_ioctl().
  */
__asm__(".symver __xap_ap_ioctl,ap_ioctl@@XAP_1.0");

/** @brief
  * @param fd
  * @param sptype
  * @param cdata
  * @param ubuf
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_look().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  *
  * This function is essentially the same as ap_rcv() except that it does not
  * consume the primitive and also does not cause a state transition.
  */
int
__xap_ap_look(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf,
	      int *flags, unsigned long *aperrno_p)
{
}

/** @fn int ap_look(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf, int *flags, unsigned long *aperrno_p)
  * @param fd
  * @param sptype
  * @param cdata
  * @param ubuf
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_look().
  */
__asm__(".symver __xap_ap_look,ap_look@@XAP_1.0");

/** @brief
  * @param fds
  * @param nfds
  * @param timeout
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_poll().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  */
int
__xap_ap_poll(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *aperrno_p)
{
	struct pollfd *pfds = (struct pollfd *) fds;
	int err;

	if ((err = poll(pfds, nfds, timeout)) == -1 && aperrno_p != NULL)
		*aperrno_p = errno;
	return (err);
}

/** @fn int ap_poll(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *aperrno_p)
  * @param fds
  * @param nfds
  * @param timeout
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_poll().
  */
__asm__(".symver __xap_ap_poll,ap_poll@@XAP_1.0");

/** @brief
  * @param fd
  * @param sptype
  * @param cdata
  * @param ubuf
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_rcv().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  */
int
__xap_ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf,
	     int *flags, unsigned long *aperrno_p)
{
}

/** @fn int ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf, int *flags, unsigned long *aperrno_p)
  * @param fd
  * @param sptype
  * @param cdata
  * @param ubuf
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_rcv().
  */
__asm__(".symver __xap_ap_rcv,ap_rcv@@XAP_1.0");

/** @brief
  * @param fd
  * @param savef
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_save().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  */
int
__xap_ap_save(int fd, FILE *savef, unsigned long *aperrno_p)
{
}

/** @fn int ap_save(int fd, FILE * savef, unsigned long *aperrno_p)
  * @param fd
  * @param savef
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_save().
  */
__asm__(".symver __xap_ap_save,ap_save@@XAP_1.0");

/** @brief
  * @param fd
  * @param sptype
  * @param cdata
  * @param ubuf
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbo is an implementation of ap_snd().
  *
  * @aperrno_p must be set to point to a location which will be used to carry
  * an error code to the user.
  */
int
__xap_ap_snd(int fd, unsigned long sptype, ap_cdata_t * cdata, ap_osi_vbuf_t * ubuf, int flags,
	     unsigned long *aperrno_p)
{
}

/** @fn int ap_snd(int fd, unsigned long sptype, ap_cdata_t *cdata, ap_osi_vbuf_t *ubuf, int flags, unsigned long aperrno_p)
  * @param fd
  * @param sptype
  * @param cdata
  * @param ubuf
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is a strong alias of __xap_ap_snd().
  */
__asm__(".symver __xap_ap_snd,ap_snd@@XAP_1.0");

/** @brief
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_ro_init().
  */
int
__xap_ap_ro_init(int fd, unsigned long *aperrno_p)
{
}

/** @fn int ap_ro_init(int fd, unsigned long *aperrno_p)
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is a strong alias of __xap_ap_ro_init().
  */
__asm__(".symver __xap_ap_ro_init,ap_ro_init@@XAP_1.0");

/** @brief
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of __xap_ap_ro_release().
  */
int
__xap_ap_ro_release(int fd, unsigned long *aperrno_p)
{
}

/** @fn int ap_ro_release(int fd, unsigned long *aperrno_p)
  * @param fd
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is a strong alias of __xap_ap_ro_release().
  */
__asm__(".symver __xap_ap_ro_release,ap_ro_release@@XAP_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
