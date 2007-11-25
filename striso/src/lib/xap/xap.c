/*****************************************************************************

 @(#) $RCSfile: xap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/11/10 19:40:54 $

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

 Last Modified $Date: 2007/11/10 19:40:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xap.c,v $
 Revision 0.9.2.2  2007/11/10 19:40:54  brian
 - documentation updates

 Revision 0.9.2.1  2007/11/06 12:16:52  brian
 - added library and header files

 *****************************************************************************/

#ident "@(#) $RCSfile: xap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/11/10 19:40:54 $"

static char const ident[] = "$RCSfile: xap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/11/10 19:40:54 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup xap OpenSS7 XAP Library
  * @{ */

/** @file
  * OpenSS7 X/Open ASCE/Presentation (XAP) implemernation file.  */

/**
  * This manual contains documetnatoin of the OpenSS7 XAP Library functions that
  * are generated autormatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 XAP
  * Library and is not intended for users of the OpenSS7 XAP Library.  Users
  * should consult the documentation found in the user manual pages beginning
  * with xap(3).
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 XAP Library is designed to be thread-safe.  This is accomplished
  * in a number of ways.  Thread-safety depends on the use of glibc and the
  * pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data for errno and h_errno.
  * Because h_errno uses communications functions orthoginal to the XAP Library
  * services, we borrow h_errno and use it for ap_errno.  This does not cause a
  * problem because neither h_errno nor ap_errno need to maintain their value
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
  * Because libpthread uses Linux clone processes for threads, cancellation of a
  * thread is accomplished by sending a signal to the thread process.  This does
  * not directly result in cancellation, but will result in the failure of a
  * system call with the EINTR error code.  It is ncessary to test for
  * cancellation upon error return from system calls to perform the actual
  * cancellation of the thread.
  *
  * The XAP specification (OpenGroup XAP 3) lists the following functions as
  * being thread cancellation points:
  *
  * The OpenSS7 XAP Library adds the following functions that operate on data or
  * expedited data with options that are not present in the XAP 3
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
  * Functions that act as thread cancellation points must push routines onto the
  * function stack executed at exit of the thread to release the locks held by
  * the function.  These are performed with weak definitions of POSIX thread
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
	user->statef = (1<< state);
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
		user->state = AP_
	}
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
  * Upon successful completion the service provider may begin enqueuing incoming
  * associations or sending outbound association requests.  All necessary
  * environment variables (e.g. %AP_BIND_PADDR, %AP_ROLE_ALLOWED) should be set
  * prior to ap_bind().
  *
  * When this function is called, an attempt may be made to bind to the
  * specified address.  As a part of the bind procedure, an authorization check
  * may be performed to verify that all of the processes that share this XAP
  * instance are authorizsed to use the new address.  If all are authorised to
  * do so, the bind request will succeed and the XAP instance may be used to
  * send (receive) primitives from (addressed to) the new address.  Successfully
  * calling this function causes the state of the XAP instance to move from
  * %AP_UNBOUND to %AP_IDLE.
  *
  * Some implementations may perform no authorisation checking.  In this case,
  * the [%AP_ACCES] error response will not be generated.  Other implementations
  * may defer binding and authorisation until an %A_ASSOC_REQ or %A_ASSOC_RSP
  * primitive is issued.  In this case, if the authorisation check fails, the
  * %AP_ACCES error will be returned by ap_snd().  THe local address can then be
  * changed to an acceptable value and the primitive reissued, or the connection
  * can be closed.
  *
  * An instance can be bound to a presentation address only if all of the
  * processes that share it are authorised to use the requested address.
  * Consequently, when an attempt is made to bind an address, the effective UIDs
  * aof all of the processes that share this isntance of XAP may be checked
  * against the list of users allowed to use the requested address.  If all are
  * authorised to use the address, ap_bind() succeeds and the isntance is bound
  * to the specified presentation address.  On the other hand, if any of the
  * processes is not authorised to use the requested address, ap_bind() fails
  * and the instance remains unbound.
  *
  * @aperrno_p must be set to point to a location whcih will be used to carry an
  * error code back to the user.
  *
  * @par Return: Upon successful completion, a value of 0 is returned.
  * Otherwise, a value of -1 is returned and the location pointed to by
  * @aperrno_p is set to indicate the error.
  *
  * @par Errors:
  *	[%AP_ACCES]	Request to bind to specified address denied.
  *	[%AP_BADF]	The @fd parameter does not identify an XAP instance.
  *	[%AP_BADNSAP]	The format of the NSAP portion fo the Presentation
  *			Address is not supported.
  *	[%AP_NOENV]	There is no XAP environment associated with @fd.
  */
int
ap_bind(int fd, unsigned long *aperrno_p)
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

/** @brief
  * @param apperno
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_error().
  */
const char *
__xap_ap_error(unsigned long aperrno)
{
}

/** @fn const char *ap_error(unsigned long aperrno)
  * @param apperno
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_error().
  */
__asm__(".symver __xap_ap_error,ap_error@@XAP_1.0");

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
  * @param kind
  * @param val
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_get_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_get_env(int fd, unsigned long attr, void *val, unsigned long *aperrno_p)
{
}

/** @fn int ap_get_env(int fd, unsigned long attr, void *val, unsigned long *aperrno_p)
  * @param fd
  * @param kind
  * @param val
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_get_env().
  */
__asm__(".symver __xap_ap_get_env,ap_get_env@@XAP_1.0");

/** @brief
  * @param fd
  * @param env_file
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_init_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_init_env(int fd, const char *env_file, int flags, unsigned long *aperrno_p)
{
}

/** @fn int ap_init_env(int fd, const char *env_file, int flags, unsigned long *aperrno_p)
  * @param fd
  * @param env_file
  * @param flags
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_init_env().
  *
  * This function initializes an otherwise unintialized XAP instance identified by @fd.  Also, it
  * may be used to set the values fo several writable environment attributes with a single function
  * call rather than using ap_set_env() to set each attribute individually.
  *
  * If no environment exists when called, memory will be allocated for the environment attributes in
  * the calling process's data space and the attributes will be set to their default values.  If the
  * user wishes to override the defaults for certain writable attributes, values for those
  * attributes may be specified in an intialization file.
  *
  * If an environment already exists when called, attributes will be assigned values.  In this case,
  * attributes will not automatically be set to their default values.
  *
  * To set the environment attributes from values stored in a file, @env_file must point to a
  * null-terminated string that is the initialization file's pathname.  An environment
  * initialization file si generated by processing ap_env_file file using the ap_osic(1) command.
  * Setting env_file to NULL indicates that no values are to be taken from an environment
  * intialization file.
  *
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
__asm__(".symver __xap_ap_init_env,ap_init_env@@XAP_1.0");

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
  * Unfortunately, the AP_POLLxxx bits are poll(2s) bits instead of
  * I_SETSIG(7) bits and the call and return values must be bit-translated.
  * Also, we do not want to necessarily set AP_SETPOLL to I_SETSIG and
  * AP_GETPOLL to I_GETSIG, so those are translated too.
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
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
		*(long *)argument.v = flags;
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
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
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
  * @param provider
  * @param oflags
  * @param ap_user_alloc
  * @param ap_user_dalloc
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_open().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc,
	      ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
}

/** @fn int ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
  * @param provider
  * @param oflags
  * @param ap_user_alloc
  * @param ap_user_dalloc
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_open().
  */
__asm__(".symver __xap_ap_open,ap_open@@XAP_1.0");

/** @brief
  * @param fds
  * @param nfds
  * @param timeout
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_poll().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_poll(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *aperrno_p)
{
	struct pollfd *pfds = (struct pollfd *)fds;
	int err;

	if  ((err = poll(pfds, nfds, timeout)) == -1 && aperrno_p != NULL)
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
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
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
  * @param oflags
  * @param ap_user_alloc
  * @param ap_user_dealloc
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_restore().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_restore(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc,
		 ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
{
}

/** @fn int ap_restore(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p)
  * @param fd
  * @param savef
  * @param oflags
  * @param ap_user_alloc
  * @param ap_user_dealloc
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is strong alias of __xap_ap_restore().
  */
__asm__(".symver __xap_ap_restore,ap_restore@@XAP_1.0");

/** @brief
  * @param fd
  * @param savef
  * @param aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_save().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_save(int fd, FILE * savef, unsigned long *aperrno_p)
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
  * @param attr
  * @param val
  * @aperrno_p
  * @version XAP_1.0
  * @par Alias:
  * This symbol is an implementation of ap_set_env().
  *
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
  */
int
__xap_ap_set_env(int fd, unsigned long attr, ap_val_t val, unsigned long *aperrno_p)
{
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
  * @aperrno_p must be set to point to a location which will be used to carry an
  * error code to the user.
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
