/*****************************************************************************

 @(#) $RCSfile: xnet.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2006/10/02 11:32:26 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 2.1 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

 Last Modified $Date: 2006/10/02 11:32:26 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: xnet.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2006/10/02 11:32:26 $"

static char const ident[] =
    "$RCSfile: xnet.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2006/10/02 11:32:26 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup xnet OpenSS7 XNET Library
  * @{ */

/** @file
  * OpenSS7 X/Open Networking Library (XNET) implementation file.  */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

/** @name System Configuration Names
  * These are system configuration names for use with t_sysconf().  Note that
  * _SC_T_IOV_MAX is already set to 1 by glibc header files in <unistd.h>.
  * Well, used to, not any more, it is some really big number.
  * @{ */
//#define _SC_T_IOV_MAX         1       /**< IOV maximum. */
#define _SC_T_DEFAULT_ADDRLEN	2	/**< Default address length. */
#define _SC_T_DEFAULT_CONNLEN	3	/**< Default connect data length. */
#define _SC_T_DEFAULT_DISCLEN	4	/**< Default disconnect data length. */
#define _SC_T_DEFAULT_OPTLEN	5	/**< Default option length. */
#define _SC_T_DEFAULT_DATALEN	6	/**< Default data length. */
/** @} */

/** @name System Configuration Values Defaults
  * These are the default values for the corresponding system configuration names and are the default
  * values returned by t_sysconf().
  * @{ */
#define _T_DEFAULT_ADDRLEN	128	/**< Default address length. */
#define _T_DEFAULT_CONNLEN	256	/**< Default connect data length. */
#define _T_DEFAULT_DISCLEN	256	/**< Default disconnect data length. */
#define _T_DEFAULT_OPTLEN	256	/**< Default options length. */
#define _T_DEFAULT_DATALEN	16384	/**< Default user data length. */
#define _T_TIMEOUT		-1	/**< Default timeout value for library ioctl. */
#define _T_IOV_MAX		16	/**< IOV maximum. */
/** @} */

#define NEED_T_USCALAR_T 1

#include <stdlib.h>

#include "gettext.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>

#if 0
#pragma weak getpmsg
#pragma weak putpmsg
#pragma weak getmsg
#pragma weak putmsg
#pragma weak isastream
#endif

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

#include <xti.h>
#include <tihdr.h>
#include <timod.h>

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
  * This manual contains documentation of the OpenSS7 XNS/XTI Library functions
  * that are generated automatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 XNS/XTI
  * Library and is not intended for users of the OpenSS7 XNS/XTI Library.
  * Users should consult the documentation found in the user manual pages
  * beginning with xti(3).
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 XNS/XTI Library is design to be thread-safe.  This is
  * accomplished in a number of ways.  Thread-safety depends on the use of
  * glibc2 and the pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data for errno and h_errno.
  * Because h_errno uses communications function orthoginal to the XTI Library
  * services, we borrow h_errno and use it for t_errno.  This does not cause a
  * problem because neither h_errno nor t_errno need to maintain their value
  * accross any other system call.
  *
  * Glibc2 also provides some weak undefined aliases for POSIX thread functions
  * to peform its own thread-safety.  When the pthread library (libpthread) is
  * linked with glibc2, these functions call libpthread functions instead
  * of internal dummy routines.  The same approach is taken for the OpenSS7
  * XNS/XTI Library.  The library uses weak defined and undefined aliases that
  * automatically invoke libpthread functions when libpthread is (dynamically)
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
  * The XTI sepcification (OpenGroup XNS 5.2) lists the following functions as
  * being thread cancellation points: t_close(), t_connect(), t_listen(),
  * t_rcv(), t_rcvconnect(), t_rcvrel(), t_rcvreldata(), t_rcvudata(), t_rcvv,
  * t_rcvvudata(), t_snd(), t_sndrel(), t_sndreldata(), t_sndudata(), t_sndv(),
  * t_sndvudata().
  *
  * The OpenSS7 XNS/XTI Library adds the following functions that operate on
  * data or expedited data with options that are not present in the XNS 5.2
  * specifications, that are also thread cancellation points: t_rcvopt(),
  * t_rcvvopt(), t_sndopt(), t_sndvopt().
  *
  * Other XTI functions are not permitted by XNS 5.2 to be thread cancellation
  * points.  Any function that cannot be a thread cancellation point needs to
  * have its cancellation status deferred if it internally invokes a function
  * that permits thread cancellation.  Functions that do not permit thread
  * cancellation are: t_accept(), t_addleaf(), t_alloc(), t_bind(), t_error(),
  * t_free(), t_getinfo(), t_getprotaddr(), t_getstate(), t_look(), t_open(),
  * t_optmgmt(), t_rcvdis(), t_rcvleafchange(), t_rcvuderr(), t_removeleaf(),
  * t_snddis(), t_strerror(), t_sync(), t_sysconf(), t_unbind().
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
  * same technique that is used by the OpenSS7 XNS/XTI library.
  */

struct __xnet_tsd {
	int terrno;
};

/*
 *  Once condition for Thread-Specific Data key creation.
 */
static pthread_once_t __xnet_tsd_once = PTHREAD_ONCE_INIT;

/*
 *  XNET library Thread-Specific Data key.
 */
static pthread_key_t __xnet_tsd_key = 0;

static void
__xnet_tsd_free(void *buf)
{
	pthread_setspecific(__xnet_tsd_key, NULL);
	free(buf);
}

static void
__xnet_tsd_alloc(void)
{
	int ret;
	void *buf;

	ret = pthread_key_create(&__xnet_tsd_key, __xnet_tsd_free);
	buf = malloc(sizeof(struct __xnet_tsd));
	bzero(buf, sizeof(*buf));
	ret = pthread_setspecific(__xnet_tsd_key, buf);
	return;
}

static struct __xnet_tsd *
__xnet_get_tsd(void)
{
	pthread_once(&__xnet_tsd_once, __xnet_tsd_alloc);
	return (struct __xnet_tsd *) pthread_getspecific(__xnet_tsd_key);
};

/** @brief #t_errno location function.
  * @version XNET_1.0
  * @par Alias:
  * This function is an implementation of _t_errno().
  */
int *
__xnet__t_errno(void)
{
	return &(__xnet_get_tsd()->terrno);
}

/** @fn int *_t_errno(void)
  * @version XNET_1.0
  * @par Alias:
  * This symbol is a strong alias of __xnet__t_errno().
  *
  * This function provides the location of the integer that contains the XTI/TLI
  * error number returned by the last XTI function that failed.  This is
  * normally used to provide #t_errno in a thread-safe way as follows:
  *
  * @code
  * #define t_errno (*(_t_errno()))
  * @endcode
  */
__asm__(".symver __xnet__t_errno,_t_errno@@XNET_1.0");

struct _t_user {
	pthread_rwlock_t lock;		/**< lock for this structure */
	int refs;			/**< number of references to this structure */
	int event;			/**< pending t_look() events */
	int flags;			/**< user flags */
	int fflags;			/**< file flags */
	int gflags;			/**< getmsg flags */
	int state;			/**< XTI state */
	int statef;			/**< TPI state flag */
	int prim;			/**< last received TLI primitive */
	int qlen;			/**< length of the listen queue */
	int ocnt;			/**< outstanding connection indications */
	u_int8_t moredat;		/**< more data in T_DATA_IND/T_OPTDATA_IND */
	u_int8_t moresdu;		/**< more tsdu */
	u_int8_t moreexp;		/**< more data in T_EXDATA_IND/T_OPTDATA_IND */
	u_int8_t moreedu;		/**< more etsdu */
	u_int8_t moremsg;		/**< more data with dis/con/rel message */
	int ctlmax;			/**< maximum size of ctlbuf */
	char *ctlbuf;			/**< ctrl part buffer */
	int datmax;			/**< maximum size of datbuf */
	char *datbuf;			/**< data part buffer */
	uint token;			/**< acceptor id */
	struct strbuf ctrl;		/**< ctrl part of received message */
	struct strbuf data;		/**< data part of received message */
	struct t_info info;		/**< information structure */
};

/** @name Transport User Flags
  * Flags for the _t_user::flags member of the struct _t_user structure.
  * @{ */
#define TUF_FLOW_NORM		01	/**< Was flow controlled for normal messages. */
#define TUF_FLOW_EXP		02	/**< Was flow controlled for expedited messages. */
#define TUF_SYNC_REQUIRED	04	/**< t_sync() required. */
#define TUF_WACK_INFO		010	/**< Waiting for T_INFO_ACK. */
#define TUF_WACK_OPTMGMT	020	/**< Waiting for T_OPTMGMT_ACK. */
#define TUF_WACK_ADDR		040	/**< Waiting for T_ADDR_ACK. */
#define TUF_WACK_CAPABILITY	0100	/**< Waitinf for T_CAPABILITY_ACK. */
#define TUF_WACK_GETADDR	0200	/**< Waiting for T_GETADDR_ACK. */
#define TUF_WACK_OK		0400	/**< Waiting for T_OK_ACK. */
#define TUF_WACK_BIND		01000	/**< Waiting for T_BIND_ACK. */
#define TUF_MORE_DATA		02000	/**< More data left on receive queue. */
/** @} */

#ifndef T_ACK
#define T_ACK (-2)		/**< for now */
#endif

static struct _t_user *_t_fds[OPEN_MAX] = { NULL, };

/** @name Transport State Flags
  * Efficiency for avoiding case statements with two results for multiple
  * states.
  * @{ */
#define TSF_UNBND	(1 << TS_UNBND		)   /**< Unbound state. */
#define TSF_WACK_BREQ	(1 << TS_WACK_BREQ	)   /**< Waiting for a T_BIND_ACK. */
#define TSF_WACK_UREQ	(1 << TS_WACK_UREQ	)   /**< Waiting for a T_OK_ACK. */
#define TSF_IDLE	(1 << TS_IDLE		)   /**< Idle state. */
#define TSF_WACK_OPTREQ	(1 << TS_WACK_OPTREQ	)   /**< Waiting to receive a T_OPTMGMT_ACK. */
#define TSF_WACK_CREQ	(1 << TS_WACK_CREQ	)   /**< Waiting to receive a T_OK_ACK. */
#define TSF_WCON_CREQ	(1 << TS_WCON_CREQ	)   /**< Waiting to receive a T_CONN_CON. */
#define TSF_WRES_CIND	(1 << TS_WRES_CIND	)   /**< Waiting to send a T_CONN_RES. */
#define TSF_WACK_CRES	(1 << TS_WACK_CRES	)   /**< Waiting to send a T_OK_ACK. */
#define TSF_DATA_XFER	(1 << TS_DATA_XFER	)   /**< Data Transfer state. */
#define TSF_WIND_ORDREL	(1 << TS_WIND_ORDREL	)   /**< Waiting to receive a T_ORDREL_IND. */
#define TSF_WREQ_ORDREL	(1 << TS_WREQ_ORDREL	)   /**< Waiting to send a T_ORDREL_REQ. */
#define TSF_WACK_DREQ6	(1 << TS_WACK_DREQ6	)   /**< Waiting to receive a T_OK_ACK. */
#define TSF_WACK_DREQ7	(1 << TS_WACK_DREQ7	)   /**< Waiting to receive a T_OK_ACK. */
#define TSF_WACK_DREQ9	(1 << TS_WACK_DREQ9	)   /**< Waiting to receive a T_OK_ACK. */
#define TSF_WACK_DREQ10	(1 << TS_WACK_DREQ10	)   /**< Waiting to receive a T_OK_ACK. */
#define TSF_WACK_DREQ11	(1 << TS_WACK_DREQ11	)   /**< Waiting to receive a T_OK_ACK. */
/** @} */

/** @internal
  * @brief Set the state of the endpoint to a constant state.
  * @param user a pointer to the _t_user structure for this endpoint.
  * @param state the constant state value to set.
  * 
  * This inline function sets the state of the the transport endpoint in the
  * user structure to the constate state specified.  This function is an inline
  * accepting a constant so that the compiler can inline just the appropriate
  * case value.  */
static inline void
__xnet_u_setstate_const(struct _t_user *user, const int state)
{
	user->statef = (1 << state);
	switch (state) {
	case TS_UNBND:
	case TS_WACK_BREQ:
		user->state = T_UNBND;
		break;
	case TS_WACK_UREQ:
	case TS_IDLE:
	case TS_WACK_CREQ:
		user->state = T_IDLE;
		break;
	case TS_WCON_CREQ:
	case TS_WACK_DREQ6:
		user->state = T_OUTCON;
		break;
	case TS_WRES_CIND:
	case TS_WACK_CRES:
	case TS_WACK_DREQ7:
		user->state = T_INCON;
		break;
	case TS_DATA_XFER:
	case TS_WACK_DREQ9:
		user->state = T_DATAXFER;
		break;
	case TS_WIND_ORDREL:
	case TS_WACK_DREQ10:
		user->state = T_OUTREL;
		break;
	case TS_WREQ_ORDREL:
	case TS_WACK_DREQ11:
		user->state = T_INREL;
		break;
	default:
		user->state = T_UNINIT;
	}
}

/** @internal
  * @brief Set the state of the endpoint to a variable state.
  * @param user a pointer to the _t_user structure for this endpoint.
  * @param state the variable state value to set.
  *
  * Sets the state of the transport endpoint in the user structure to the
  * variable state specified.  This is in contrast to the inline version.  @see
  * __xnet_u_setstate_const.
  */
static void
__xnet_u_setstate(struct _t_user *user, int state)
{
	user->statef = (1 << state);
	switch (state) {
	case TS_UNBND:
	case TS_WACK_BREQ:
		user->state = T_UNBND;
		break;
	case TS_WACK_UREQ:
	case TS_IDLE:
	case TS_WACK_CREQ:
		user->state = T_IDLE;
		break;
	case TS_WCON_CREQ:
	case TS_WACK_DREQ6:
		user->state = T_OUTCON;
		break;
	case TS_WRES_CIND:
	case TS_WACK_CRES:
	case TS_WACK_DREQ7:
		user->state = T_INCON;
		break;
	case TS_DATA_XFER:
	case TS_WACK_DREQ9:
		user->state = T_DATAXFER;
		break;
	case TS_WIND_ORDREL:
	case TS_WACK_DREQ10:
		user->state = T_OUTREL;
		break;
	case TS_WREQ_ORDREL:
	case TS_WACK_DREQ11:
		user->state = T_INREL;
		break;
	default:
		user->state = T_UNINIT;
	}
}

/** @internal
  * @brief Set the current event.
  * @param user a pointer to the _t_user structure for this endpoint.
  * @param prim the TPI primitive associated with the event.
  * @param flags the flags associated with a @c T_OPTDATA_IND primitive.
  * 
  * Sets the current event in the transport endpoint user structure according
  * to the current primitive.  */
static inline int
__xnet_u_setevent(struct _t_user *user, int prim, int flags)
{
	switch ((user->prim = prim)) {
	case -1:
		return (user->event = T_DATA);
	case T_CONN_IND:
		return (user->event = T_LISTEN);
	case T_CONN_CON:
		return (user->event = T_CONNECT);
	case T_DISCON_IND:
		return (user->event = T_DISCONNECT);
	case T_DATA_IND:
		return (user->event = T_DATA);
	case T_EXDATA_IND:
		return (user->event = T_EXDATA);
	case T_UNITDATA_IND:
		return (user->event = T_DATA);
	case T_UDERROR_IND:
		return (user->event = T_UDERR);
	case T_ORDREL_IND:
		return (user->event = T_ORDREL);
	case T_OPTDATA_IND:
		return (user->event = (flags & T_ODF_EX) ? T_EXDATA : T_DATA);
	default:
		return (user->event = 0);
	}
}

/** @internal
  * @brief Reset (consume) the current event.
  * @param user a pointer to the _t_user structure for this endpoint.
  *
  * Resets (or consumes) the current event.  All event information within the
  * specified user structure is reset.  */
static void
__xnet_u_reset_event(struct _t_user *user)
{
	if (user->event != T_EXDATA || (!user->moresdu && !user->moredat)) {
		user->prim = 0;
		user->event = 0;
		user->ctrl.maxlen = user->ctlmax;
		user->ctrl.len = 0;
		user->ctrl.buf = user->ctlbuf;
		user->data.maxlen = user->datmax;
		user->data.len = 0;
		user->data.buf = user->datbuf;
		user->moredat = 0;
		user->moresdu = 0;
		user->moreexp = 0;
		user->moreedu = 0;
		user->moremsg = 0;
	} else {
		/** When we are clearing an expedited data event, we must revert to an outstanding
		   data event. */
		user->prim = 0;
		user->event = T_DATA;
		user->ctrl.maxlen = user->ctlmax;
		user->ctrl.len = 0;
		user->ctrl.buf = user->ctlbuf;
		user->data.maxlen = user->datmax;
		user->data.len = 0;
		user->data.buf = user->datbuf;
		user->moreexp = 0;
		user->moreedu = 0;
		user->moremsg = 0;
	}
}

static inline int
__xnet_u_max_addr(struct _t_user *user)
{
	return (user->info.addr == T_INFINITE
		? MAXINT : (user->info.addr >= 0 ? user->info.addr : 0));
}
static inline int
__xnet_u_max_options(struct _t_user *user)
{
	return (user->info.options == T_INFINITE
		? MAXINT : (user->info.options >= 0 ? user->info.options : 0));
}
static inline int
__xnet_u_max_tsdu(struct _t_user *user)
{
	return ((user->info.tsdu == T_INFINITE || user->info.tsdu == 0)
		? MAXINT : (user->info.tsdu >= 0 ? user->info.tsdu : 0));
}
static inline int
__xnet_u_max_etsdu(struct _t_user *user)
{
	return ((user->info.etsdu == T_INFINITE || user->info.etsdu == 0)
		? MAXINT : (user->info.etsdu >= 0 ? user->info.etsdu : 0));
}
static inline int
__xnet_u_max_connect(struct _t_user *user)
{
	return (user->info.connect == T_INFINITE
		? MAXINT : (user->info.connect >= 0 ? user->info.connect : 0));
}
static inline int
__xnet_u_max_discon(struct _t_user *user)
{
	return (user->info.discon == T_INFINITE
		? MAXINT : (user->info.discon >= 0 ? user->info.discon : 0));
}
static inline int
__xnet_u_max_tidu(struct _t_user *user)
{
	return (user->info.tidu == T_INFINITE
		? MAXINT : (user->info.tidu >= 0 ? user->info.tidu : 0));
}

/*
   Forward declarations 
 */
static int __xnet_t_getmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int *flagsp);
static int __xnet_t_putmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int flags);
static int __xnet_t_putpmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int band, int flags);
static int __xnet_t_ioctl(int fd, int cmd, void *arg);
static int __xnet_t_strioctl(int fd, int cmd, void *arg, size_t arglen);
int __xnet_t_peek(int fd);

#if 0
int __xnet_t_accept(int fd, int resfd, const struct t_call *call);
int __xnet_t_addleaf(int fd, int leafid, struct netbuf *addr);
char *__xnet_t_alloc(int fd, int type, int fields);
int __xnet_t_bind(int fd, const struct t_bind *req, struct t_bind *ret);
int __xnet_t_close(int fd);
int __xnet_t_connect(int fd, const struct t_call *sndcall, struct t_call *rcvcall);
int __xnet_t_free(void *ptr, int type);
int __xnet_t_getinfo(int fd, struct t_info *info);
int __xnet_t_getstate(int fd);
int __xnet_t_listen(int fd, struct t_call *call);
int __xnet_t_look(int fd);
int __xnet_t_open(const char *path, int oflag, struct t_info *info);
int __xnet_t_optmgmt(int fd, const struct t_optmgmt *req, struct t_optmgmt *ret);
int __xnet_t_rcv(int fd, char *buf, unsigned int nbytes, int *flags);
int __xnet_t_rcvconnect(int fd, struct t_call *call);
int __xnet_t_rcvdis(int fd, struct t_discon *discon);
int __xnet_t_rcvleafchange(int fd, struct t_leaf_status *change);
int __xnet_t_rcvrel(int fd);
int __xnet_t_rcvreldata(int fd, struct t_discon *discon);
int __xnet_t_rcvopt(int fd, struct t_unitdata *optdata, int *flags);
int __xnet_t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags);
int __xnet_t_rcvv(int fd, struct t_iovec *iov, unsigned int iovcount, int *flags);
int __xnet_t_rcvvudata(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
		       unsigned int iovcount, int *flags);
int __xnet_t_removeleaf(int fd, int leafid, int reason);
int __xnet_t_snd(int fd, char *buf, unsigned int nbytes, int flags);
int __xnet_t_snddis(int fd, const struct t_call *call);
int __xnet_t_sndrel(int fd);
int __xnet_t_sndreldata(int fd, struct t_discon *discon);
int __xnet_t_sndopt(int fd, const struct t_unitdata *optdata, int flags);
int __xnet_t_sndvopt(int fd, const struct t_unitdata *optdata, const struct t_iovec *iov,
		     unsigned int iovcount, int flags);
int __xnet_t_sndudata(int fd, const struct t_unitdata *unitdata);
int __xnet_t_sndv(int fd, const struct t_iovec *iov, unsigned int iovcount, int flags);
int __xnet_t_sndvudata(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
		       unsigned int iovcount);
int __xnet_t_sysconf(int name);
int __xnet_t_unbind(int fd);
const char *__xnet_t_strerror(int errnum);
#endif

#if 0
int __xnet_t_accept_r(int fd, int resfd, const struct t_call *call);
int __xnet_t_addleaf_r(int fd, int leafid, struct netbuf *addr);
char *__xnet_t_alloc_r(int fd, int type, int fields);
int __xnet_t_bind_r(int fd, const struct t_bind *req, struct t_bind *ret);
int __xnet_t_close_r(int fd);
int __xnet_t_connect_r(int fd, const struct t_call *sndcall, struct t_call *rcvcall);
int __xnet_t_getinfo_r(int fd, struct t_info *info);
int __xnet_t_getstate_r(int fd);
int __xnet_t_listen_r(int fd, struct t_call *call);
int __xnet_t_look_r(int fd);
int __xnet_t_open_r(const char *path, int oflag, struct t_info *info);
int __xnet_t_optmgmt_r(int fd, const struct t_optmgmt *req, struct t_optmgmt *ret);
int __xnet_t_rcv_r(int fd, char *buf, unsigned int nbytes, int *flags);
int __xnet_t_rcvconnect_r(int fd, struct t_call *call);
int __xnet_t_rcvdis_r(int fd, struct t_discon *discon);
int __xnet_t_rcvleafchange_r(int fd, struct t_leaf_status *change);
int __xnet_t_rcvrel_r(int fd);
int __xnet_t_rcvreldata_r(int fd, struct t_discon *discon);
int __xnet_t_rcvopt_r(int fd, struct t_unitdata *optdata, int *flags);
int __xnet_t_rcvudata_r(int fd, struct t_unitdata *unitdata, int *flags);
int __xnet_t_rcvv_r(int fd, struct t_iovec *iov, unsigned int iovcount, int *flags);
int __xnet_t_rcvvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
			 unsigned int iovcount, int *flags);
int __xnet_t_removeleaf_r(int fd, int leafid, int reason);
int __xnet_t_snd_r(int fd, char *buf, unsigned int nbytes, int flags);
int __xnet_t_snddis_r(int fd, const struct t_call *call);
int __xnet_t_sndrel_r(int fd);
int __xnet_t_sndreldata_r(int fd, struct t_discon *discon);
int __xnet_t_sndopt_r(int fd, const struct t_unitdata *optdata, int flags);
int __xnet_t_sndvopt_r(int fd, const struct t_unitdata *optdata, const struct t_iovec *iov,
		       unsigned int iovcount, int flags);
int __xnet_t_sndudata_r(int fd, const struct t_unitdata *unitdata);
int __xnet_t_sndv_r(int fd, const struct t_iovec *iov, unsigned int iovcount, int flags);
int __xnet_t_sndvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
			 unsigned int iovcount);
int __xnet_t_unbind_r(int fd);
#endif

/** @internal
  * @brief A version of getmsg with XTI errors.
  * @param fd a file descriptor representing the transport endpoint.
  * @param ctrl a pointer to a strbuf structure returning the control part of
  * the message.
  * @param data a pointer to a strbuf structure returning the data part of the
  * message.
  * @param flagsp a pointer to an integer returning the flags associated with
  * the retrieved message.
  * 
  * This is the same as getmsg(2) with the exception that XTI errors are
  * returned.
  *
  * @since Sun Dec 21 19:23:52 MST 2003
  *
  * @return When __xnet_t_getmsg() succeeds, it returns zero (0); when it
  * fails, it returns -1.
  *
  * @par Errors
  * __xnet_t_getmsg() can return the following XTI errors:
  *
  * @retval TBADF
  * #fd is not associated with a file open for reading or is a directory
  * rather than a file.
  * @retval TNODATA
  * @c O_NONBLOCK was set on open(2) or with fcntl(2) and no data is available
  * to be read.
  * @retval TSYSERR
  * A Linux system error occured and the error number  is in errno(3).
  */
static int
__xnet_t_getmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int *flagsp)
{
	int ret;

	if ((ret = getmsg(fd, ctrl, data, flagsp)) >= 0)
		return (ret);
	t_errno = TSYSERR;
	switch (errno) {
	case EISDIR:
	case EBADF:
		t_errno = TBADF;
		break;
	case EFAULT:
	case ENODEV:
	case ENOSTR:
	case EIO:
	case EINVAL:
		break;
	case EAGAIN:
		t_errno = TNODATA;
		return (0);
	case EINTR:
	case ENOSR:
	case EBADMSG:
		break;
	}
	return (-1);
}

/**
  * @internal
  * @brief A version of putmsg with XTI errors.
  * @param fd a file descriptor representing the transport endpoint.
  * @param ctrl a pointer to a strbuf structure describing the control part of
  * the message.
  * @param data a pointer to a strbuf structure describing the data part of the
  * message.
  * @param flags a flag integer describing the nature of the message.
  * 
  * This is the same as putmsg(2) with the exception that XTI errors are
  * returned.
  */
static __hot int
__xnet_t_putmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int flags)
{
	int ret;

	if ((ret = putmsg(fd, ctrl, data, flags)) >= 0)
		return (ret);
	t_errno = TSYSERR;
	switch (errno) {
	case EAGAIN:
		t_errno = TFLOW;
		break;
	case EISDIR:
	case EBADF:
		t_errno = TBADF;
		break;
	case EFAULT:
	case EINTR:
	case EINVAL:
	case EIO:
	case ENODEV:
	case ENOSR:
	case ENOSTR:
	case ENXIO:
	case ERANGE:
		break;
	}
	return (-1);
}

/**
  * @internal
  * @brief A version of putpmsg with XTI errors.
  * @param fd a file descriptor representing the transport endpoint.
  * @param ctrl a pointer to a strbuf structure describing the control part of
  * the message.
  * @param data a pointer to a strbuf structure describing the data part of the
  * message.
  * @param band a band number describing the band for the message.
  * @param flags a flag integer describing the nature of the message.
  *
  * This is the same as putpmsg(2) with the exception that XTI errors are
  * returned.
  */
static int
__xnet_t_putpmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	int ret;

	if ((ret = putpmsg(fd, ctrl, data, band, flags)) != -1)
		return (ret);
	t_errno = TSYSERR;
	switch (errno) {
	case EAGAIN:
		t_errno = TFLOW;
		break;
	case EISDIR:
	case EBADF:
		t_errno = TBADF;
		break;
	case EFAULT:
	case EINTR:
	case EINVAL:
	case EIO:
	case ENODEV:
	case ENOSR:
	case ENOSTR:
	case ENXIO:
	case ERANGE:
		break;
	}
	return (-1);
}

static __hot int
__xnet_t_getdata(int fd, struct strbuf *udata, int expect)
{
	struct _t_user *user = _t_fds[fd];
	int ret, flag = 0;
	union T_primitives *p = (typeof(p)) user->ctlbuf;

	if (likely(user->event == T_DATA) || likely(user->event == T_EXDATA)) {
		if (unlikely(!(user->event & expect)))
			goto done;
		if (unlikely(user->data.len < 1))
			goto getmoredata;
		udata->len = min(udata->maxlen, user->data.len);
		memcpy(udata->buf, user->data.buf, udata->len);
		user->data.buf += udata->len;
		user->data.len -= udata->len;
	} else if (user->event == 0) {
		__xnet_u_reset_event(user);
	      getmoredata:
		user->data.maxlen = min(udata->maxlen, user->datmax);
		user->data.len = 0;
		user->data.buf = udata->buf;
		if ((ret = __xnet_t_getmsg(fd, &user->ctrl, &user->data, &flag)) < 0)
			goto error;
		if (user->ctrl.len < 0)
			user->ctrl.len = 0;
		if (user->data.len < 0)
			user->data.len = 0;
		udata->len = user->data.len;
		if (ret & MORECTL)
			goto cleanup;
		if (flag != 0 || flag == RS_HIPRI)
			goto tsync;
		if (user->ctrl.len || user->data.len) {
			if (user->event == 0 || user->ctrl.len > 0)
				__xnet_u_setevent(user, user->ctrl.len ? p->type : -1, 0);
			/* 
			   There is a little extra handling for data indications */
			switch (user->prim) {
			case -1:	/* just data */
				user->moredat = ((ret & MOREDATA) != 0);
				break;
			case T_DATA_IND:
				user->moresdu = (p->data_ind.MORE_flag != 0);
				user->moredat = ((ret & MOREDATA) != 0);
				break;
			case T_EXDATA_IND:
				user->moreedu = (p->exdata_ind.MORE_flag != 0);
				user->moreexp = ((ret & MOREDATA) != 0);
				break;
			case T_OPTDATA_IND:
				if (p->optdata_ind.DATA_flag & T_ODF_EX) {
					user->moreedu =
					    ((p->optdata_ind.DATA_flag & T_ODF_MORE) != 0);
					user->moreexp = ((ret & MOREDATA) != 0);
				} else {
					user->moresdu =
					    ((p->optdata_ind.DATA_flag & T_ODF_MORE) != 0);
					user->moredat = ((ret & MOREDATA) != 0);
				}
				break;
			case T_UNITDATA_IND:
				user->moresdu = 0;
				user->moredat = ((ret & MOREDATA) != 0);
				user->moreedu = 0;
				user->moreexp = 0;
				break;
			case T_CONN_IND:
			case T_CONN_CON:
			case T_DISCON_IND:
			case T_ORDREL_IND:
			case T_UDERROR_IND:
				user->moremsg = ((ret & MOREDATA) != 0);
				break;
			}
			if ((user->event & expect))
				user->data.len = 0;	/* consumed */
			else {
				/* 
				   didn't get what we were expecting, so we need to move any data
				   from the user buffer back to the look buffer */
				if (user->data.len > 0)
					memcpy(user->datbuf, user->data.buf, user->data.len);
				user->data.buf = user->datbuf;
				udata->len = 0;
			}
		}
	} else
		goto tlook;
      done:
	return (user->event);
      cleanup:
	while (ret != -1 && (ret & (MORECTL | MOREDATA)))
		ret = __xnet_t_getmsg(fd, &user->ctrl, &user->data, &flag);
	goto tsync;
      tlook:
	t_errno = TLOOK;
	goto error;
      tsync:
	__xnet_u_reset_event(user);	/* discard current event */
	user->flags |= TUF_SYNC_REQUIRED;
	goto tproto;
      tproto:
	t_errno = TPROTO;
	goto error;
      error:
	return (-1);
}

/**
  * @internal
  * @brief Get the next event on the transport endpoint.
  * @param fd a file descriptor for the transport endpoint.
  *
  * Returns the current, or obtains the current, event for the transport
  * endpoing and completes elements in the user data structure representing the
  * transport endpoint.  The control and data parts of any retrieved messages
  * are cached in the user structure and information interpreted and
  * appropriate flags set.
  */
static int
__xnet_t_getevent(int fd)
{
	struct _t_user *user = _t_fds[fd];
	int ret, flag = 0;
	union T_primitives *p = (typeof(p)) user->ctlbuf;

	switch (user->event) {
	case 0:
		__xnet_u_reset_event(user);
		if ((ret = __xnet_t_getmsg(fd, &user->ctrl, &user->data, &flag)) < 0)
			goto error;
		if (ret & MORECTL)
			goto cleanup;
		if (flag != 0 || flag == RS_HIPRI)
			goto tsync;
		if (user->ctrl.len || user->data.len) {
			__xnet_u_setevent(user, user->ctrl.len ? p->type : -1, 0);
			/* 
			   There is a little extra handling for data indications */
			switch (user->prim) {
			case -1:	/* just data */
				user->moredat = ((ret & MOREDATA) != 0);
				break;
			case T_DATA_IND:
				user->moresdu = (p->data_ind.MORE_flag != 0);
				user->moredat = ((ret & MOREDATA) != 0);
				break;
			case T_EXDATA_IND:
				user->moreedu = (p->exdata_ind.MORE_flag != 0);
				user->moreexp = ((ret & MOREDATA) != 0);
				break;
			case T_OPTDATA_IND:
				if (p->optdata_ind.DATA_flag & T_ODF_EX) {
					user->moreedu =
					    ((p->optdata_ind.DATA_flag & T_ODF_MORE) != 0);
					user->moreexp = ((ret & MOREDATA) != 0);
				} else {
					user->moresdu =
					    ((p->optdata_ind.DATA_flag & T_ODF_MORE) != 0);
					user->moredat = ((ret & MOREDATA) != 0);
				}
				break;
			case T_UNITDATA_IND:
				user->moresdu = 0;
				user->moredat = ((ret & MOREDATA) != 0);
				user->moreedu = 0;
				user->moreexp = 0;
				break;
			case T_CONN_IND:
			case T_CONN_CON:
			case T_DISCON_IND:
			case T_ORDREL_IND:
			case T_UDERROR_IND:
				user->moremsg = ((ret & MOREDATA) != 0);
				break;
			}
		}
	}
	return (user->event);
      cleanup:
	while (ret != -1 && (ret & (MORECTL | MOREDATA)))
		ret = __xnet_t_getmsg(fd, &user->ctrl, &user->data, &flag);
	goto tsync;
      tsync:
	__xnet_u_reset_event(user);	/* discard current event */
	user->flags |= TUF_SYNC_REQUIRED;
	goto tproto;
      tproto:
	t_errno = TPROTO;
	goto error;
      error:
	return (-1);
}

static pthread_rwlock_t __xnet_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static inline int
__xnet_lock_rdlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__xnet_lock_wrlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline void
__xnet_lock_unlock(void *rwlock)
{
	pthread_rwlock_unlock(rwlock);
}

static inline int
__xnet_list_rdlock(void)
{
	return __xnet_lock_rdlock(&__xnet_fd_lock);
}
static int
__xnet_list_wrlock(void)
{
	return __xnet_lock_wrlock(&__xnet_fd_lock);
}
static void
__xnet_list_unlock(void *ignore)
{
	return __xnet_lock_unlock(&__xnet_fd_lock);
}

#if 0
static int
__xnet_user_rdlock(struct _t_user *user)
{
	return __xnet_lock_rdlock(&user->lock);
}
#endif
static inline int
__xnet_user_wrlock(struct _t_user *user)
{
	return __xnet_lock_wrlock(&user->lock);
}
static inline void
__xnet_user_unlock(struct _t_user *user)
{
	return __xnet_lock_unlock(&user->lock);
}

static void
__xnet_t_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct _t_user *user = _t_fds[fd];

	__xnet_user_unlock(user);
	__xnet_list_unlock(NULL);
	return;
}

/**
  * @internal
  * @brief Get a locked transport user endpoint structure.
  * @param fd the file descriptor for which to get the associated endpoint.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe operation.
  */
static __hot struct _t_user *
__xnet_t_getuser(int fd)
{
	struct _t_user *user;
	int err;

	if (unlikely((err = __xnet_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto tbadf;
	if (unlikely(!(user = _t_fds[fd])))
		goto tbadf;
	if (unlikely((err = __xnet_user_wrlock(user))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	t_errno = TSYSERR;
	errno = err;
	__xnet_list_unlock(NULL);
	goto error;
      tbadf:
	t_errno = TBADF;
	goto error;
      list_lock_error:
	t_errno = TSYSERR;
	errno = err;
	goto error;
      error:
	return (NULL);
}

/**
  * @internal
  * @brief Test information about a transport endpoint.
  * @param fd the file descriptor for which to test the associated endpoint.
  * @param expect the event anticipated on the transport endpoint.
  * @param servtype the service type anticipated on the transport endpoint.
  * @param states the flag mask of the states anticipated for the transport endpoint.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * checks for expected events, service types and states as follows:
  *
  * When expect is not -1, if there is a current event on the transport
  * endpoint and that event is not the same as the expected event, then the
  * call will fail (return NULL) and set t_errno to @c T_LOOK.
  *
  * servtype is a bit mask of the service types expected for the transport
  * endpoint.  If the service type of the transport endpoint is not one of the
  * service types in the mask, then the call will fail (return NULL) and set
  * t_errno to @c TNOTSUPPORT.  To accept any service type, set servtype to -1.
  *
  * states is a bit mask of the (TPI) states expected for the transport
  * endpoint.  If the state of the transport endpoing is not one of the states
  * in the mask, then the call will fail (return NULL) and set t_errno to
  * @c TOUTSTATE.  To accept any state, set states to -1.
  */
static __hot struct _t_user *
__xnet_t_tstuser(int fd, const int expect, const int servtype, const int states)
{
	struct _t_user *user;

	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto tbadf;
	if (unlikely(!(user = _t_fds[fd])))
		goto tbadf;
	if (unlikely(user->flags & TUF_SYNC_REQUIRED))
		goto tproto;
	if (unlikely(user->event && user->event != expect && expect != -1))
		goto tlook;
	if (unlikely(!((1 << user->info.servtype) & servtype)))
		goto tnotsupport;
	if (unlikely(!(user->statef & states)))
		goto toutstate;
	return (user);
      toutstate:
	t_errno = TOUTSTATE;
	goto error;
      tnotsupport:
	t_errno = TNOTSUPPORT;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      tproto:
	t_errno = TPROTO;
	goto error;
      tbadf:
	t_errno = TBADF;
	goto error;
      error:
	return (NULL);
}

/**
  * @internal
  * @brief A version of ioctl(2) with XTI errors.
  * @param fd A file descriptor upon which to issue an IO control.
  * @param cmd The IO control command.
  * @param arg Argument to the IO control command.
  *
  * Our timod @c TI_ ioctls return the error codes (if any) in the return value
  * rather than errno.  If we get a non-zero return value, it indicates that we
  * need to unpack the ti and unix error codes and place them in the
  * appropriate error numbers.
  */
static int
__xnet_t_ioctl(int fd, int cmd, void *arg)
{
	int ret;

	switch ((ret = ioctl(fd, cmd, arg))) {
	case 0:
		return (0);
	case -1:
		switch (errno) {
		case EBADF:
		case ENOTTY:
		case EINVAL:
		case EPERM:
			t_errno = TBADF;
			break;
		default:
			t_errno = TSYSERR;
			break;
		}
		return (-1);
	default:
		if ((t_errno = ret & 0x00ff) == TSYSERR)
			if (!(errno = (ret >> 8) & 0x00ff))
				errno = EINVAL;
		return (-1);
	}
}

/**
  * @internal
  * @brief A version of ioctl(2) with XTI errors.
  * @param fd A file descriptor upon which to issue an IO control.
  * @param cmd The IO control command.
  * @param arg Argument to the IO control command.
  * @param arglen The length of the argument.
  *
  * This is a simple matter of packing an otherwise tranparent IO control into
  * a strioctl buffer and issuing an I_STR IO control instead.  This calls
  * __xnet_t_ioctl(), so it understands how to properly unpack timod XTI and UNIX
  * error codes.
  */
static int
__xnet_t_strioctl(int fd, int cmd, void *arg, size_t arglen)
{
	struct strioctl ioc;

	ioc.ic_cmd = cmd;
	ioc.ic_timout = _T_TIMEOUT;
	ioc.ic_len = arglen;
	ioc.ic_dp = arg;
	return __xnet_t_ioctl(fd, I_STR, &ioc);
}

/** @brief Accept a connection indication.
  * @param fd the file descriptor upon which the connection indication was received.
  * @param resfd the file descriptor upon which to accept the transport connection.
  * @param call a pointer to a t_call structure describing the responding transport endpoint.  */
int
__xnet_t_accept(int fd, int resfd, const struct t_call *call)
{
	struct _t_user *user, *resuser;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD), TSF_WRES_CIND)))
		goto error;
	if (fd == resfd) {
		resuser = user;
		if (user->ocnt > 1)
			goto tindout;
		if (user->ocnt < 1)
			goto tproto;
	} else
	    if (!(resuser = __xnet_t_tstuser(resfd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
					     TSF_UNBND | TSF_IDLE)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
	if (__xnet_t_peek(resfd) > 0)
		goto tlook;
#ifdef DEBUG
	if (!call)
		goto einval;
	if (call->addr.len < 0 || (call->addr.len > 0 && !call->addr.buf))
		goto einval;
	if (call->opt.len < 0 || (call->opt.len > 0 && !call->opt.buf))
		goto einval;
	if (call->udata.len < 0 || (call->udata.len > 0 && !call->udata.buf))
		goto einval;
#endif
	if (call && call->addr.len > __xnet_u_max_addr(user))
		goto tbadaddr;
	if (call && call->opt.len > __xnet_u_max_options(user))
		goto tbadopt;
	if (call && call->udata.len > __xnet_u_max_connect(user))
		goto tbaddata;
	/* 
	   Check if we need to bind the responding stream to the responding address.  This also
	   rules out the case where the listening stream and the responding stream are the same
	   address. */
	if (resuser->statef & TSF_UNBND) {
		size_t add_len = (call && call->addr.len > 0) ? call->addr.len : 0;
		struct {
			struct T_bind_req prim;
			unsigned char addr[add_len];
		} req;

		/* 
		   There is no way to pass the responding address to the stream as there is in the
		   NPI, therefore, we must bind the stream to the proper responding address before
		   accepting the connection indication. This sets the local name associated with
		   the accepting stream. */
		req.prim.PRIM_type = T_BIND_REQ;
		req.prim.ADDR_length = add_len;
		req.prim.ADDR_offset = add_len ? sizeof(req.prim) : 0;
		req.prim.CONIND_number = 0;
		if (add_len)
			memcpy(req.addr, call->addr.buf, add_len);
		if (__xnet_t_strioctl(resfd, TI_BIND, &req, sizeof(req)) != 0)
			goto error;
		__xnet_u_setstate_const(resuser, TS_IDLE);
	}
	/* 
	   XNS 5.2 says that if the responding stream is already bound that it must be bound to the 
	   same address a specified in call->addr, so we don't check */
	{
		size_t opt_len = (call && call->opt.len > 0) ? call->opt.len : 0;
		size_t dat_len = (call && call->udata.len > 0) ? call->udata.len : 0;
		struct {
			struct T_conn_res prim;
			unsigned char opt[opt_len];
			unsigned char udata[dat_len];
		} req;

		req.prim.PRIM_type = T_CONN_RES;
		req.prim.ACCEPTOR_id = resuser->token;
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = opt_len ? sizeof(req.prim) : 0;
		req.prim.SEQ_number = call ? call->sequence : 0;
		if (opt_len)
			memcpy(req.opt, call->opt.buf, opt_len);
		if (dat_len > 0)
			memcpy(req.opt + opt_len, call->udata.buf, dat_len);
		if (__xnet_t_strioctl(fd, TI_SETMYNAME, &req, sizeof(req)) != 0)
			goto error;
		__xnet_u_setstate_const(resuser, TS_DATA_XFER);
		if (user->ocnt && !--user->ocnt)
			__xnet_u_setstate_const(user, TS_IDLE);
	}
	return (0);
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbadaddr:
	t_errno = TBADADDR;
	goto error;
      tbadopt:
	t_errno = TBADOPT;
	goto error;
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tproto:
	t_errno = TPROTO;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      tindout:
	t_errno = TINDOUT;
	goto error;
      error:
	if (t_errno != TLOOK && (__xnet_t_peek(fd) > 0 || __xnet_t_peek(resfd) > 0))
		goto tlook;
	return (-1);
}

/** @brief The reentrant version of __xnet_t_accept().
  * @param fd the file descriptor upon which the connection indication was received.
  * @param resfd the file descriptor upon which to accept the transport connection.
  * @param call a pointer to a t_call structure describing the responding transport endpoint.
  * @version XNET_1.0
  * @par Alias:
  * This symbol is an implementation of t_accept().
  *
  * This function is NOT a thread cancellation point.  t_accept(3) is NOT a a
  * thread cancellation point; therefore, we disable cancellation for the
  * duration of the call.  */
int
__xnet_t_accept_r(int fd, int resfd, const struct t_call *call)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_accept(fd, resfd, call);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int t_accept(int fd, int resfd, const struct t_call *call)
  * @param fd the file descriptor upon which the connection indication was received.
  * @param resfd the file descriptor upon which to accept the transport connection.
  * @param call a pointer to a t_call structure describing the responding transport endpoint.
  * @version XNET_1.0
  * @par Alias:
  * This symbol is a strong alias of __xnet_t_accept_r().
  */
__asm__(".symver __xnet_t_accept_r,t_accept@@XNET_1.0");

/** @brief Add a leaf to a point to multipoint connection.
  * @param fd A file descriptor for the transport user endpoint.
  * @param leafid The identifier for the leaf.
  * @param addr A netbuf(3) structure describing the address of the added leaf.
  * @version XNET_1.0
  *
  * This XTI Liubrary function is only used for ATM.  It is used to add a leaf
  * to a point to multipoint connection.  This function does not translate to
  * a TPI message exchange, but invokes a t_optmgmt call on an existing ATM
  * connection.  */
int
__xnet_t_addleaf(int fd, int leafid, struct netbuf *addr)
{
#if defined HAVE_XTI_ATM_H
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD), TSF_DATA_XFER)))
		goto error;
#ifdef DEBUG
	if (!addr)
		goto einval;
	if (addr->len < 0 || (addr->len > 0 && !addr->buf))
		goto einval;
#endif
	if (!addr || addr->len < sizeof(struct t_atm_addr))
		goto tbadaddr;
	{
		struct {
			struct t_opthdr hdr;
			struct t_atm_add_leaf leaf;
		} opts;
		struct t_optmgmt req, ret;

		req.opt.maxlen = 0;
		req.opt.len = sizeof(opts);
		req.opt.buf = (char *) &opts;
		req.flags = T_CURRENT;
		ret.opt.maxlen = sizeof(opts);
		ret.opt.len = 0;
		ret.opt.buf = (char *) &opts;
		ret.flags = 0;
		opts.hdr.len = sizeof(opts);
		opts.hdr.level = T_ATM_SIGNALLING;
		opts.hdr.name = T_ATM_ADD_LEAF;
		opts.hdr.status = 0;
		opts.leaf.leaf_ID = leafid;
		memcpy(&opt.leaf.leaf_address, addr->buf, addr->len);
		if (__xnet_t_optmgmt(fd, &req, &ret) == -1)
			goto error;
		if (ret.opt.flags == T_FAILURE)
			goto tproto;
		if (ret.opt.len < sizeof(opts) || opts.hdr.len < sizeof(opts))
			goto tproto;
		if (opts.hdr.status != T_SUCCESS)
			goto tproto;
		if (opts.hdr.level != T_ATM_SIGNALLING || opts.hdr.name != T_ATM_ADD_LEAF)
			goto tproto;
		/**
		   The t_addleaf function requires an operating-system specific
		   blocking mechanism to know when to check for the leaf status
		   indication.  The most obvious approach is to have the ATM
		   transport service provider send a signal to the stream head
		   when an indication has arrived.  What signal (SIGPOLL, SIGIO,
		   SIGUSR) is a question. Nevertheless, we can check our open
		   flags here and either decide to block on a poll or not to
		   check for the indication that the addition was successful.
		   We do not do this yet.  We just always return nodata.  It is
		   the caller's responsibility to check with t_rcvleafchange(). 
		 */
		goto tnodata;
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbadaddr:
	t_errno = TBADADDR;
	goto error;
      error:
	return (-1);
#else				/* defined HAVE_XTI_ATM_H */
	t_errno = TNOTSUPPORT;
	return (-1);
#endif				/* defined HAVE_XTI_ATM_H */
}

#pragma weak __xnet_t_addleaf_r
/** @brief The reentrant version of __xnet_t_addleaf().
  * @param fd A file descriptor for the transport user endpoint.
  * @param leafid The identifier for the leaf.
  * @param addr A netbuf(3) structure describing the address of the added leaf.
  * @version XNET_1.0
  * @par Alias:
  * This function is an implementation of t_addleaf().
  *
  * This function is NOT a thread cancellation point.
  * t_addleaf() is NOT a thread cancellation point, but ioctl(2) is; therefore,
  * we disable cancellation for the duration of the call.
  *
  * This XTI Liubrary function is only used for ATM.  It is used to add a leaf
  * to a point to multipoint connection.  This function does not translate to
  * a TPI message exchange, but invokes a t_optmgmt call on an existing ATM
  * connection.  */
int
__xnet_t_addleaf_r(int fd, int leafid, struct netbuf *addr)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_addleaf(fd, leafid, addr);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int t_addleaf(int fd, int leafid, struct netbuf *addr)
  * @param fd A file descriptor for the transport user endpoint.
  * @param leafid The identifier for the leaf.
  * @param addr A netbuf(3) structure describing the address of the added leaf.
  * @version XNET_1.0
  * @par Alias:
  * This symbol is a weak alias of __xnet_t_addleaf_r().
  */
__asm__(".symver __xnet_t_addleaf_r,t_addleaf@@XNET_1.0");

/**
  * @brief Allocate an XTI structure and initialize fields.
  * @param fd A file descriptor for the transport user endpoint.
  * @param type The type of structure to allocate.
  * @param fields The fields in the structure to initialize.
  * @version XNET_1.0
  * @par Alias:
  * This function is an implementation of t_alloc().
  *
  * This function is NOT a thread cancellation point.
  *
  * Allocate the requested library structure and initialize the requested
  * fields.  This consists of allocating the structure itself and allocating
  * buffers for any of the requested fields.
  *
  * It would be more efficient if we allocated the structure and the buffers
  * together in a single allocation.  We would probably get better memory
  * performance if all of the pieces were kept contiguous.  However, the user
  * has the possibility of separately freeing the buffers and the t_free()
  * function does exactly that.
  * 
  * t_alloc() is NOT a thread cancellation point; therefore, we defer
  * cancellation for the duration of the call.  Note that there are no inherent
  * cancellation points within __xnet_t_alloc().
  */
char *
__xnet_t_alloc(int fd, int type, int fields)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, -1, -1)))
		goto error;
	switch (type) {
	case T_BIND:
	{
		struct t_bind *bind;

		if (!(bind = (struct t_bind *) malloc(sizeof(*bind))))
			goto badalloc;
		memset(bind, 0, sizeof(*bind));
		if (fields & T_ADDR) {
			int len;

			switch ((len = user->info.addr)) {
			case T_INFINITE:
				len = _T_DEFAULT_ADDRLEN;
			default:
				if (!(bind->addr.buf = (char *) malloc(len))) {
					free(bind);
					goto badalloc;
				}
				bind->addr.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					free(bind);
					goto einval;
				}
				break;
			}
		}
		return ((char *) bind);
	}
	case T_OPTMGMT:
	{
		struct t_optmgmt *opts;

		if (!(opts = (struct t_optmgmt *) malloc(sizeof(*opts))))
			goto badalloc;
		memset(opts, 0, sizeof(*opts));
		if (fields & T_OPT) {
			int len;

			switch ((len = user->info.options)) {
			case T_INFINITE:
				len = _T_DEFAULT_OPTLEN;
			default:
				if (!(opts->opt.buf = (char *) malloc(len))) {
					free(opts);
					goto badalloc;
				}
				opts->opt.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					free(opts);
					goto einval;
				}
				break;
			}
		}
		return ((char *) opts);
	}
	case T_CALL:
	{
		struct t_call *call;

		if (user->info.servtype == T_CLTS)
			goto tnostructype;
		if (!(call = (struct t_call *) malloc(sizeof(*call))))
			goto badalloc;
		memset(call, 0, sizeof(*call));
		if (fields & T_ADDR) {
			int len;

			switch ((len = user->info.addr)) {
			case T_INFINITE:
				len = _T_DEFAULT_ADDRLEN;
			default:
				if (!(call->addr.buf = (char *) malloc(len))) {
					free(call);
					goto badalloc;
				}
				call->addr.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					free(call);
					goto einval;
				}
				break;
			}
		}
		if (fields & T_OPT) {
			int len;

			switch ((len = user->info.options)) {
			case T_INFINITE:
				len = _T_DEFAULT_OPTLEN;
			default:
				if (!(call->opt.buf = (char *) malloc(len))) {
					if (call->addr.buf)
						free(call->addr.buf);
					free(call);
					goto badalloc;
				}
				call->opt.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					if (call->addr.buf)
						free(call->addr.buf);
					free(call);
					goto einval;
				}
				break;
			}
		}
		if (fields & T_UDATA) {
			int len;

			switch ((len = user->info.connect)) {
			case T_INFINITE:
				len = _T_DEFAULT_CONNLEN;
			default:
				if (!(call->udata.buf = (char *) malloc(len))) {
					if (call->addr.buf)
						free(call->addr.buf);
					if (call->opt.buf)
						free(call->opt.buf);
					free(call);
					goto badalloc;
				}
				call->udata.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					if (call->addr.buf)
						free(call->addr.buf);
					if (call->opt.buf)
						free(call->opt.buf);
					free(call);
					goto einval;
				}
				break;
			}
		}
		return ((char *) call);
	}
	case T_DIS:
	{
		struct t_discon *discon;

		if (user->info.servtype == T_CLTS)
			goto tnostructype;
		if (!(discon = (struct t_discon *) malloc(sizeof(*discon))))
			goto badalloc;
		memset(discon, 0, sizeof(*discon));
		if (fields & T_UDATA) {
			int len;

			switch ((len = user->info.discon)) {
			case T_INFINITE:
				len = _T_DEFAULT_DISCLEN;;
			default:
				if (!(discon->udata.buf = (char *) malloc(len))) {
					free(discon);
					goto badalloc;
				}
				discon->udata.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					free(discon);
					goto einval;
				}
				break;
			}
		}
		return ((char *) discon);
	}
	case T_UNITDATA:
	{
		struct t_unitdata *udata;

		if (user->info.servtype != T_CLTS)
			goto tnostructype;
		if (!(udata = (struct t_unitdata *) malloc(sizeof(*udata))))
			goto badalloc;
		memset(udata, 0, sizeof(*udata));
		if (fields & T_ADDR) {
			int len;

			switch ((len = user->info.addr)) {
			case T_INFINITE:
				len = _T_DEFAULT_ADDRLEN;
			default:
				if (!(udata->addr.buf = (char *) malloc(len))) {
					free(udata);
					goto badalloc;
				}
				udata->addr.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					free(udata);
					goto einval;
				}
				break;
			}
		}
		if (fields & T_OPT) {
			int len;

			switch ((len = user->info.options)) {
			case T_INFINITE:
				len = _T_DEFAULT_OPTLEN;
			default:
				if (!(udata->opt.buf = (char *) malloc(len))) {
					if (udata->addr.buf)
						free(udata->addr.buf);
					free(udata);
					goto badalloc;
				}
				udata->opt.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					if (udata->addr.buf)
						free(udata->addr.buf);
					free(udata);
					goto einval;
				}
				break;
			}
		}
		if (fields & T_UDATA) {
			int len;

			switch ((len = user->info.tsdu)) {
			case T_INFINITE:
			case 0:
				len = _T_DEFAULT_DATALEN;
			default:
				if (!(udata->udata.buf = (char *) malloc(len))) {
					if (udata->addr.buf)
						free(udata->addr.buf);
					if (udata->opt.buf)
						free(udata->opt.buf);
					free(udata);
					goto badalloc;
				}
				udata->udata.maxlen = len;
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					if (udata->addr.buf)
						free(udata->addr.buf);
					if (udata->opt.buf)
						free(udata->opt.buf);
					free(udata);
					goto einval;
				}
				break;
			}
		}
		return ((char *) udata);
	}
	case T_UDERROR:
	{
		struct t_uderr *uderr;

		if (user->info.servtype != T_CLTS)
			goto tnostructype;
		if (!(uderr = (struct t_uderr *) malloc(sizeof(*uderr))))
			goto badalloc;
		memset(uderr, 0, sizeof(*uderr));
		if (fields & T_ADDR) {
			int len;

			switch ((len = user->info.addr)) {
			case T_INFINITE:
				len = _T_DEFAULT_ADDRLEN;
			default:
				if (!(uderr->addr.buf = (char *) malloc(len))) {
					free(uderr);
					goto badalloc;
				}
				uderr->addr.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					free(uderr);
					goto einval;
				}
				break;
			}
		}
		if (fields & T_OPT) {
			int len;

			switch ((len = user->info.options)) {
			case T_INFINITE:
				len = _T_DEFAULT_OPTLEN;
			default:
				if (!(uderr->opt.buf = (char *) malloc(len))) {
					if (uderr->addr.buf)
						free(uderr->addr.buf);
					free(uderr);
					goto badalloc;
				}
				uderr->opt.maxlen = len;
			case 0:
				break;
			case T_INVALID:
				if (fields != T_ALL) {
					if (uderr->addr.buf)
						free(uderr->addr.buf);
					free(uderr);
					goto einval;
				}
				break;
			}
		}
		return ((char *) uderr);
	}
	case T_INFO:
	{
		struct t_info *inf;

		if (!(inf = (struct t_info *) malloc(sizeof(*inf))))
			goto badalloc;
		memset(inf, 0, sizeof(*inf));
		return ((char *) inf);
	}
	default:
		goto tnostructype;
	}
      tnostructype:
	t_errno = TNOSTRUCTYPE;
	goto error;
      einval:
	t_errno = TSYSERR;
	errno = EINVAL;
	goto error;
      badalloc:
	t_errno = TSYSERR;
	goto error;
      error:
	return ((char *) NULL);
}

/** The reentrant version of __xnet_t_alloc(). */
char *
__xnet_t_alloc_r(int fd, int type, int fields)
{
	char *ret = NULL;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_alloc(fd, type, fields);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_alloc_r,t_alloc@@XNET_1.0");

/** @brief Bind an address to a transport endpoint.
  * @param fd A file descriptor indicating the transport endpoint to bind.
  * @param req A t_bind structure indicating the bind parameters.
  * @param ret A t_bind structure to return the bind result.
  * @version XNET_1.0
  * @par Alias:
  * This function is an implementation of t_bind().
  *
  * This function is NOT a thread cancellation point.
  * t_bind() is NOT a thread cancellation point; however, ioctl(2) might be;
  * therefore, we disable cancellation for the duration of the call.
  */
int
__xnet_t_bind(int fd, const struct t_bind *req, struct t_bind *ret)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, -1, TSF_UNBND)))
		goto error;
#ifdef DEBUG
	if (req && (req->addr.len < 0 || (req->addr.len > 0 && !req->addr.buf)))
		goto einval;
	if (ret && (req->addr.maxlen < 0 || (ret->addr.maxlen > 0 && !ret->addr.buf)))
		goto einval;
#endif
	if (req && req->addr.len > __xnet_u_max_addr(user))
		goto tbadaddr;
	{
		size_t add_len = (req && req->addr.len > 0) ? req->addr.len : 0;
		size_t add_max = min(__xnet_u_max_addr(user), _T_DEFAULT_ADDRLEN);
		size_t qlen = (req && req->qlen > 0) ? req->qlen : 0;
		union {
			struct {
				struct T_bind_req prim;
				unsigned char addr[add_len];
			} req;
			struct {
				struct T_bind_ack prim;
				unsigned char addr[add_max];
			} ack;
		} buf;

		buf.req.prim.PRIM_type = T_BIND_REQ;
		buf.req.prim.ADDR_length = add_len;
		buf.req.prim.ADDR_offset = add_len ? sizeof(buf.req.prim) : 0;
		buf.req.prim.CONIND_number = qlen;
		if (add_len)
			memcpy(buf.req.addr, req->addr.buf, add_len);
		if (__xnet_t_strioctl(fd, TI_BIND, &buf, sizeof(buf)) != 0)
			goto error;
		__xnet_u_setstate_const(user, TS_IDLE);
		user->qlen = buf.ack.prim.CONIND_number;
		if (ret) {
			if (ret->addr.maxlen > 0) {
				if (ret->addr.maxlen < buf.ack.prim.ADDR_length)
					goto tbufovflw;
				if ((ret->addr.len = buf.ack.prim.ADDR_length))
					memcpy(ret->addr.buf,
					       ((char *) &buf) + buf.ack.prim.ADDR_offset,
					       ret->addr.len);
			}
			ret->qlen = buf.ack.prim.CONIND_number;
		}
		return (0);
	}
      tbufovflw:
	t_errno = TBUFOVFLW;
	goto error;
      tbadaddr:
	t_errno = TBADADDR;
	goto error;
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto tsyserr;
#endif
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_bind(). */
int
__xnet_t_bind_r(int fd, const struct t_bind *req, struct t_bind *ret)
{
	int rtv = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		rtv = __xnet_t_bind(fd, req, ret);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (rtv);
}

__asm__(".symver __xnet_t_bind_r,t_bind@@XNET_1.0");

/** @brief Close a transport endpoint.
  * @param fd A file descriptor for the transport endpoint to close.
  * @version XNET_1.0
  *
  * This function is a thread cancellation point.  t_close() is a thread
  * cancellation point, and so it close(2); therefore, we defer cancellation
  * until the call completes.
  */
int
__xnet_t_close(int fd)
{
	struct _t_user *user;
	int ret;

	if (!(user = __xnet_t_tstuser(fd, -1, -1, -1)))
		goto error;
	if ((ret = close(fd)) == 0 || errno != EINTR) {
		if (--user->refs == 0) {
			_t_fds[fd] = NULL;
			pthread_rwlock_destroy(&user->lock);
			if (user->ctlbuf)
				free(user->ctlbuf);
			if (user->datbuf)
				free(user->datbuf);
			free(user);
		}
	}
	if (ret == 0)
		return (0);
	if (errno == EINTR)
		t_errno = TSYSERR;
      error:
	return (-1);
}

/**
  * @brief Recursive t_close function.
  * @param fd A file descriptor for the transport endpoint to close.
  * @version XNET_1.0
  * @par Alias:
  * This function is an implementation of t_close().
  *
  * This is again a little different that most of the _r wrappers: we take a
  * write lock on the _t_fds list so that we are able to delete the file
  * descriptor from the list.  This will block most other threads from
  * performing functions on the list, also, we must wait for a quiet period
  * until all other functions that read lock the list are not being used.  If
  * you are sure that the close will only be performed by one thread and that
  * no other thread will act on the file descriptor until close returns, use
  * the non-recursive version.
  */
int
__xnet_t_close_r(int fd)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__xnet_list_unlock, NULL);
	if ((err = __xnet_list_wrlock()) == 0) {
		ret = __xnet_t_close(fd);
		__xnet_list_unlock(NULL);
	} else {
		t_errno = TSYSERR;
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_close_r,t_close@@XNET_1.0");

/** @brief Establish a transport connection.
  * @param fd the transport endpoint to connect.
  * @param sndcall a pointer to a t_call structure specifying the peer addres, options and data.
  * @param rcvcall a pointer to a t_call structure returning the responding address, options and data.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_connect(int fd, const struct t_call *sndcall, struct t_call *rcvcall)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD), TSF_IDLE)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
#ifdef DEBUG
	if (!sndcall)
		goto einval;
	if (sndcall->addr.len < 0 || (sndcall->addr.len > 0 && !sndcall->addr.buf))
		goto einval;
	if (sndcall->opt.len < 0 || (sndcall->opt.len > 0 && !sndcall->opt.buf))
		goto einval;
	if (sndcall->udata.len < 0 || (sndcall->udata.len > 0 && !sndcall->udata.buf))
		goto einval;
	if (rcvcall
	    && (rcvcall->addr.maxlen < 0 || (rcvcall->addr.maxlen > 0 && !rcvcall->addr.buf)))
		goto einval;
	if (rcvcall && (rcvcall->opt.maxlen < 0 || (rcvcall->opt.maxlen > 0 && !rcvcall->opt.buf)))
		goto einval;
	if (rcvcall
	    && (rcvcall->udata.maxlen < 0 || (rcvcall->udata.maxlen > 0 && !rcvcall->udata.buf)))
		goto einval;
#endif
	if (sndcall && sndcall->addr.len > __xnet_u_max_addr(user))
		goto tbadaddr;
	if (sndcall && sndcall->opt.len > __xnet_u_max_options(user))
		goto tbadopt;
	if (sndcall && sndcall->udata.len > __xnet_u_max_connect(user))
		goto tbaddata;
	{
		size_t add_len = (sndcall && sndcall->addr.len > 0) ? sndcall->addr.len : 0;
		size_t opt_len = (sndcall && sndcall->opt.len > 0) ? sndcall->opt.len : 0;
		size_t dat_len = (sndcall && sndcall->udata.len > 0) ? sndcall->udata.len : 0;
		struct {
			struct T_conn_req prim;
			unsigned char addr[add_len];
			unsigned char opt[opt_len];
			unsigned char udata[dat_len];
		} req;

		req.prim.PRIM_type = T_CONN_REQ;
		req.prim.DEST_length = add_len;
		req.prim.DEST_offset = add_len ? sizeof(req.prim) : 0;
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = opt_len ? sizeof(req.prim) + add_len : 0;
		if (add_len)
			memcpy(req.addr, sndcall->addr.buf, add_len);
		if (opt_len)
			memcpy(req.addr + add_len, sndcall->opt.buf, opt_len);
		if (dat_len)
			memcpy(req.addr + add_len + opt_len, sndcall->udata.buf, dat_len);
		if (__xnet_t_strioctl(fd, TI_SETPEERNAME, &req, sizeof(req)))
			goto error;
		__xnet_u_setstate_const(user, TS_WCON_CREQ);
	}
	return __xnet_t_rcvconnect(fd, rcvcall);
      tlook:
	t_errno = TLOOK;
	goto error;
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbadaddr:
	t_errno = TBADADDR;
	goto error;
      tbadopt:
	t_errno = TBADOPT;
	goto error;
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      error:
	if (t_errno != TLOOK && __xnet_t_peek(fd) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_connect(). */
int
__xnet_t_connect_r(int fd, const struct t_call *sndcall, struct t_call *rcvcall)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_connect(fd, sndcall, rcvcall)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_connect_r,t_connect@@XNET_1.0");

/** @brief Print an error message.
  * @param errmsg the error message to print.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_error(const char *errmsg)
{
	fprintf(stderr, "%s: %s\n", errmsg, __xnet_t_strerror(t_errno));
	return (0);
}

/** The reentrant version of __xnet_t_error(). */
int
__xnet_t_error_r(const char *errmsg)
{
	int oldtype, ret;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	ret = __xnet_t_error(errmsg);
	pthread_setcanceltype(oldtype, NULL);
	return (ret);
}

__asm__(".symver __xnet_t_error_r,t_error@@XNET_1.0");

/** @brief Free an XTI library structure.
  * @param ptr a pointer to the structure to free.
  * @param type the type of the structure pointed to.
  *
  * This function is NOT a thread cancellation point.
  *
  * Frees the specified datastructure.  Any buffers remaining in the
  * datastructure (with non-NULL netbuf buf elements) will be freed using
  * free(2) as well.  This can be used to free a structure allocated with
  * malloc(2) and not necessarily allocated with t_alloc(3).
  */
int
__xnet_t_free(void *ptr, int type)
{
	if (!ptr)
		goto einval;
	switch (type) {
	case T_BIND:
	{
		struct t_bind *bind = (struct t_bind *) ptr;

		if (bind->addr.buf)
			free(bind->addr.buf);
		free(bind);
		return (0);
	}
	case T_OPTMGMT:
	{
		struct t_optmgmt *opts = (struct t_optmgmt *) ptr;

		if (opts->opt.buf)
			free(opts->opt.buf);
		free(opts);
		return (0);
	}
	case T_CALL:
	{
		struct t_call *call = (struct t_call *) ptr;

		if (call->addr.buf)
			free(call->addr.buf);
		if (call->opt.buf)
			free(call->opt.buf);
		if (call->udata.buf)
			free(call->udata.buf);
		free(call);
		return (0);
	}
	case T_DIS:
	{
		struct t_discon *discon = (struct t_discon *) ptr;

		if (discon->udata.buf)
			free(discon->udata.buf);
		free(discon);
		return (0);
	}
	case T_UNITDATA:
	{
		struct t_unitdata *unitdata = (struct t_unitdata *) ptr;

		if (unitdata->addr.buf)
			free(unitdata->addr.buf);
		if (unitdata->opt.buf)
			free(unitdata->opt.buf);
		if (unitdata->udata.buf)
			free(unitdata->udata.buf);
		return (0);
	}
	case T_UDERROR:
	{
		struct t_uderr *uderr = (struct t_uderr *) ptr;

		if (uderr->addr.buf)
			free(uderr->addr.buf);
		if (uderr->opt.buf)
			free(uderr->opt.buf);
		free(uderr);
		return (0);
	}
	case T_INFO:
	{
		struct t_info *info = (struct t_info *) ptr;

		free(info);
		return (0);
	}
	default:
		goto tnostructype;
	}
      tnostructype:
	t_errno = TNOSTRUCTYPE;
	goto error;
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
      error:
	return (-1);
}

__asm__(".symver __xnet_t_free,t_free@@XNET_1.0");

/** @brief Get protocol information.
  * @param fd the transport endpoint for which to get information.
  * @param info a pointer into a t_info structure to contain the returned information.
  *
  * This function is NOT a thread cancellation point.
  *
  * There is a bit of a question whether we should go all the way to the
  * transport provider with a @c TI_GETINFO in response to this call.  We could
  * return just the local datastructure, or we could go to the transport
  * service provider and do a little bit of a sync with the underlying
  * transport provider.  For now we are just retrieving the local copy.
  *
  * One excuse for doing this is that the state of the transport service
  * provider can be different than the state of the enxpoint as far as XTI is
  * concerned.  This is because there can be events stacked up in our receive
  * buffer and stacked up in our send buffer from and to the transport
  * provider.  Setting the XTI state to the transport provider state without
  * flushing buffers could cause a mis-sychronization.  That is, we could make
  * matters worse.
  *
  * So, we can't do anything with retreived state.  But, we could recollect the
  * protocol limits.  Protocol limits could change.  For example, the transport
  * interface data unit size could change depending on the characteristics of
  * the network path for a specific connection or as a result of negotiation
  * during connection establishment or mid-connection.  Therefore, we go to the
  * transport service provider to recollect these items.
  */
int
__xnet_t_getinfo(int fd, struct t_info *info)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, -1, -1)))
		goto error;
#ifdef DEBUG
	if (!info)
		goto einval;
#endif
	{
		union {
			struct T_info_req req;
			struct T_info_ack ack;
		} buf;

		buf.req.PRIM_type = T_INFO_REQ;
		if (__xnet_t_strioctl(fd, TI_GETINFO, &buf, sizeof(buf)) != 0)
			goto error;
		user->info.addr = buf.ack.ADDR_size;
		user->info.options = buf.ack.OPT_size;
		user->info.tsdu = buf.ack.TSDU_size;
		user->info.etsdu = buf.ack.ETSDU_size;
		user->info.connect = buf.ack.CDATA_size;
		user->info.discon = buf.ack.DDATA_size;
		user->info.servtype = buf.ack.SERV_type;
		user->info.flags = buf.ack.PROVIDER_flag;
		/* 
		   need a define around this one 'cause it doesn't exist in XNS 5.2 */
		user->info.tidu = buf.ack.TIDU_size;
		/* 
		   ignore CURRENT_state */
		if (info)
			*info = user->info;
		return (0);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_getinfo(). */
int
__xnet_t_getinfo_r(int fd, struct t_info *info)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_getinfo(fd, info);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_getinfo_r,t_getinfo@@XNET_1.0");

/** @brief Get protocol addresses.
  * @param fd specifies the local transport endpoint for which to receive address information.
  * @param loc is a pointer to a t_bind structure that returns the local transport endpoint address bound to fd.
  * @param rem is a pointer to a t_bind structure that returns the remote transport endpoint address connected to fd, if fd is in a connected state.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_getprotaddr(int fd, struct t_bind *loc, struct t_bind *rem)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, -1, -1)))
		goto error;
	{
		size_t add_max = min(__xnet_u_max_addr(user), _T_DEFAULT_ADDRLEN);
		union {
			struct {
				struct T_addr_req prim;
			} req;
			struct {
				struct T_addr_ack prim;
				unsigned char loc[add_max];
				unsigned char rem[add_max];
			} ack;
		} buf;

		buf.req.prim.PRIM_type = T_ADDR_REQ;
		if (__xnet_t_strioctl(fd, TI_GETADDRS, &buf, sizeof(buf)) != 0)
			goto error;
		if (loc && loc->addr.maxlen < buf.ack.prim.LOCADDR_length)
			goto tbufovflw;
		if (rem && rem->addr.maxlen < buf.ack.prim.REMADDR_length)
			goto tbufovflw;
		if (loc && (loc->addr.len = buf.ack.prim.LOCADDR_length))
			memcpy(loc->addr.buf, (char *) &buf + buf.ack.prim.LOCADDR_offset,
			       loc->addr.len);
		if (rem && (rem->addr.len = buf.ack.prim.REMADDR_length))
			memcpy(rem->addr.buf, (char *) &buf + buf.ack.prim.REMADDR_offset,
			       rem->addr.len);
		return (0);
	}
      tbufovflw:
	t_errno = TBUFOVFLW;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_getprotaddr(). */
int
__xnet_t_getprotaddr_r(int fd, struct t_bind *loc, struct t_bind *rem)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_getprotaddr(fd, loc, rem);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_getprotaddr_r,t_getprotaddr@@XNET_1.0");

/** @brief Get interface state.
  * @param fd the transport endpoint for which to return the transport endpoint state.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_getstate(int fd)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, -1, -1, -1)))
		goto error;
	if (user->statef & (TSF_UNBND | TSF_IDLE | TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER |
			    TSF_WREQ_ORDREL | TSF_WIND_ORDREL)) {
		return (user->state);
	}
	if (user->statef & (TSF_WACK_DREQ6 | TSF_WACK_DREQ7 | TSF_WACK_DREQ9 | TSF_WACK_DREQ10 |
			    TSF_WACK_DREQ11 | TSF_WACK_BREQ | TSF_WACK_UREQ | TSF_WACK_CREQ |
			    TSF_WACK_CRES))
		goto tstatechng;
	goto tproto;
      tstatechng:
	t_errno = TSTATECHNG;
	goto error;
      tproto:
	user->flags |= TUF_SYNC_REQUIRED;
	t_errno = TPROTO;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_getstate(). */
int
__xnet_t_getstate_r(int fd)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_getstate(fd);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_getstate_r,t_getstate@@XNET_1.0");

/** @brief Listen for a transport connection request.
  * @param fd the transport endpoint upon which to listen.
  * @param call a pointer to a t_call structure to contain returned information about a connection indication.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_listen(int fd, struct t_call *call)
{
	struct _t_user *user;

	if (!
	    (user =
	     __xnet_t_tstuser(fd, T_LISTEN, (1 << T_COTS) | (1 << T_COTS_ORD),
			      TSF_WRES_CIND | TSF_IDLE)))
		goto error;
	if ((user->statef & TSF_IDLE) && user->qlen <= 0)
		goto tbadqlen;
	if (user->ocnt >= user->qlen)
		goto tqfull;
	switch (__xnet_t_getevent(fd)) {
	case T_LISTEN:
	{
		union T_primitives *p = (typeof(p)) user->ctrl.buf;

		__xnet_u_setstate_const(user, TS_WRES_CIND);
		user->ocnt++;
		if (call) {
			call->sequence = p->conn_ind.SEQ_number;
			if (call->addr.maxlen && call->addr.maxlen < p->conn_ind.SRC_length)
				goto tbufovflw;
			if (call->opt.maxlen && call->opt.maxlen < p->conn_ind.OPT_length)
				goto tbufovflw;
			if (call->udata.maxlen && call->udata.maxlen < user->data.len)
				goto tbufovflw;
			if (call->addr.maxlen && (call->addr.len = p->conn_ind.SRC_length))
				memcpy(call->addr.buf, (char *) p + p->conn_ind.SRC_offset,
				       call->addr.len);
			if (call->opt.maxlen && (call->opt.len = p->conn_ind.OPT_length))
				memcpy(call->opt.buf, (char *) p + p->conn_ind.OPT_offset,
				       call->opt.len);
			if (call->udata.maxlen && (call->udata.len = user->data.len))
				memcpy(call->udata.buf, user->data.buf, call->udata.len);
		}
		__xnet_u_reset_event(user);	/* consume the event */
		return (0);
	}
	case 0:
		goto tnodata;
	case -1:
		goto error;
	default:
		goto tlook;
	}
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume the event */
	t_errno = TBUFOVFLW;
	goto error;
      tqfull:
	t_errno = TQFULL;
	goto error;
      tbadqlen:
	t_errno = TBADQLEN;
	goto error;
      error:
	if (t_errno != TLOOK && __xnet_t_peek(fd) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_listen(). */
int
__xnet_t_listen_r(int fd, struct t_call *call)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_listen(fd, call)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_listen_r,t_listen@@XNET_1.0");

/** @brief Look for a transport event.
  * @param fd the transport endpoint upon which to look for events.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_look(int fd)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, -1, -1, -1)))
		goto error;
	if (user->event == 0) {
		/* 
		   we are after normal messages only. */
		int ret, flag = 0;
		union T_primitives *p = (typeof(p)) user->ctlbuf;

		user->ctrl.maxlen = user->ctlmax;
		user->ctrl.len = 0;
		user->ctrl.buf = user->ctlbuf;
		user->data.maxlen = user->datmax;
		user->data.len = 0;
		user->data.buf = user->datbuf;
		switch (user->info.servtype) {
		case T_CLTS:
			if (user->state == T_IDLE)
				user->data.maxlen = 0;
			break;
		case T_COTS_ORD:
			if (user->state == T_OUTREL)
				user->data.maxlen = 0;
		case T_COTS:
			if (user->state == T_DATAXFER)
				user->data.maxlen = 0;
			break;
		default:
			goto tsync;
		}
		if ((ret = __xnet_t_getmsg(fd, &user->ctrl, &user->data, &flag)) < 0)
			goto error;
		if ((ret & MORECTL) || ((ret & MOREDATA) && user->data.maxlen > 0))	/* bad */
			goto cleanup;
		if (flag != 0 || flag == RS_HIPRI)
			goto tsync;
		switch (user->state) {
		case T_UNINIT:
			/* 
			   cannot have any events - should have never gotten here */
			goto tsync;
		case T_UNBND:
			/* 
			   cannot have any events - should have never gotten here */
			goto tsync;
		case T_IDLE:
			switch (user->info.servtype) {
			case T_COTS:
			case T_COTS_ORD:
				if (user->qlen > 0)
					goto tincon;
				/* 
				   cannot have any events - should have never gotten here */
				goto tsync;
			case T_CLTS:
				/* 
				   T_UNITDATA_IND, T_UDERROR_IND */
				if (user->ctrl.len == 0) {
					if ((user->gflags = (ret & MOREDATA))) {
						user->event = T_DATA;
						return (T_DATA);
					}
					return (0);
				}
				switch ((user->prim = p->type)) {
				case T_UNITDATA_IND:
					user->event = T_DATA;
					break;
				case T_UDERROR_IND:
					user->event = T_UDERR;
					break;
				default:
					goto cleanup;
				}
				user->gflags = (ret & MOREDATA);
				return (user->event);
			default:
				goto tsync;
			}
		case T_OUTCON:
			/* 
			   T_CONN_CON or T_DISCON_IND */
			switch ((user->prim = p->type)) {
			case T_CONN_CON:
				user->event = T_CONNECT;
				break;
			case T_DISCON_IND:
				user->event = T_DISCONNECT;
				break;
			default:
				goto cleanup;
			}
			user->gflags = 0;
			return (user->event);
		case T_INCON:
			/* 
			   T_CONN_IND or T_DISCON_IND */
		      tincon:
			switch ((user->prim = p->type)) {
			case T_CONN_IND:
				user->event = T_LISTEN;
				break;
			case T_DISCON_IND:
				user->event = T_DISCONNECT;
				break;
			default:
				goto cleanup;
			}
			user->gflags = 0;
			return (user->event);
		case T_DATAXFER:
			/* 
			   T_DATA_IND, T_EXDATA_IND, T_OPTDATA_IND, T_ORDREL_IND, T_DISCON_IND */
		case T_OUTREL:
			/* 
			   T_DATA_IND, T_EXDATA_IND, T_OPTDATA_IND, T_ORDREL_IND, T_DISCON_IND */
			if (user->ctrl.len == 0) {
				if ((user->gflags = (ret & MOREDATA))) {
					user->event = T_DATA;
					return (T_DATA);
				}
				return (0);
			}
			switch ((user->prim = p->type)) {
			case T_DATA_IND:
				user->event = T_DATA;
				break;
			case T_EXDATA_IND:
				user->event = T_EXDATA;
				break;
			case T_OPTDATA_IND:
				user->event =
				    (p->optdata_ind.DATA_flag & T_ODF_EX) ? T_EXDATA : T_DATA;
				break;
			case T_ORDREL_IND:
				if (ret & MOREDATA) {
					user->data.maxlen = user->datmax;
					user->data.len = 0;
					user->data.buf = user->datbuf;
					ret = __xnet_t_getmsg(fd, NULL, &user->data, &flag);
					if (ret < 0)
						goto error;
					if (ret != 0)
						goto cleanup;
					if (flag != 0)
						goto tsync;
				}
				user->event = (ret & MOREDATA) ? T_ORDRELDATA : T_ORDREL;
				break;
			case T_DISCON_IND:
				if (ret & MOREDATA) {
					user->data.maxlen = user->datmax;
					user->data.len = 0;
					user->data.buf = user->datbuf;
					ret = __xnet_t_getmsg(fd, NULL, &user->data, &flag);
					if (ret < 0)
						goto error;
					if (ret != 0)
						goto cleanup;
					if (flag != 0)
						goto tsync;
				}
				user->event = T_DISCONNECT;
				break;
			default:
				goto cleanup;
			}
			user->gflags = (ret & MOREDATA);
			return (user->event);
		case T_INREL:
			/* 
			   T_DISCON_IND */
			switch ((user->prim = p->type)) {
			case T_DISCON_IND:
				user->event = T_DISCONNECT;
				break;
			default:
				goto cleanup;
			}
			user->gflags = 0;
			return (user->event);
		default:
			goto tsync;
		}
	      cleanup:
		user->ctrl.maxlen = user->ctlmax;
		user->data.maxlen = user->datmax;
		while (ret & (MORECTL | MOREDATA))
			if ((ret = __xnet_t_getmsg(fd, &user->ctrl, &user->data, &flag)) < 0)
				goto error;
		goto tsync;
	}
	return (user->event);
      tsync:
	user->flags |= TUF_SYNC_REQUIRED;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_look(). */
int
__xnet_t_look_r(int fd)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_look(fd);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_look_r,t_look@@XNET_1.0");

/*
   look for an event, but do not block 
 */
__hot int
__xnet_t_peek(int fd)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, -1, -1, -1)))
		goto error;
	if (user->event == 0) {
		struct pollfd pfd;

		pfd.fd = fd;
		pfd.events =
		    (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP);
		pfd.revents = 0;
		switch (poll(&pfd, 1, 0)) {
		case 1:
			if (pfd.revents & (POLLMSG | POLLERR | POLLHUP))
				goto tlook;
			if (pfd.revents & (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND))
				return __xnet_t_look(fd);
		case 0:
			break;
		case -1:
			goto tsyserr;
		default:
			goto tproto;
		}
	}
	return (user->event);
      tlook:
	t_errno = TLOOK;
	goto error;
      tproto:
	t_errno = TPROTO;
	goto error;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
      error:
	return (-1);
}

/** @brief Open a transport endpoint.
  * @param path a character string specifying the pat to the device to open.
  * @param oflag open flags.
  * @param info a pointer to a t_info structure to contain returned information about the transport endpoint.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_open(const char *path, int oflag, struct t_info *info)
{
	struct _t_user *user;
	int err, fd;

	t_errno = TSYSERR;
	if (!(user = (struct _t_user *) malloc(sizeof(*user))))
		goto enomem;
	memset(user, 0, sizeof(*user));
	user->ctlmax =
	    sizeof(union T_primitives) + _T_DEFAULT_ADDRLEN + _T_DEFAULT_ADDRLEN +
	    _T_DEFAULT_OPTLEN;
	user->datmax = _T_DEFAULT_DATALEN;
	if (!(user->ctlbuf = (char *) malloc(user->ctlmax)))
		goto enobufs;
	if (!(user->datbuf = (char *) malloc(user->datmax)))
		goto enobufs;
	if ((fd = open(path, oflag)) == -1)
		goto badopen;
	if (ioctl(fd, I_PUSH, "timod") != 0)
		goto badioctl;
	/* 
	   need to pick up all the capabilities from the stream */
	{
		int i;
		union {
			struct T_capability_req req;
			struct T_capability_ack ack;
		} buf;

		buf.req.PRIM_type = T_CAPABILITY_REQ;
		buf.req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		if (__xnet_t_strioctl(fd, TI_CAPABILITY, &buf, sizeof(buf)) != 0)
			goto badioctl;
		/**
		   We could put a fallback in t_open() if the transport provider does not support
		   capabilities (it would be really ancient); hoever, the best place to put this
		   support would be in timod.  timod has access to the queue pair pointer to
		   generate an acceptor id and also has the ability to complete the info structure
		   from a @c T_INFO_REQ. 
		 */
		if ((buf.ack.CAP_bits1 & (TC1_INFO | TC1_ACCEPTOR_ID)) !=
		    (TC1_INFO | TC1_ACCEPTOR_ID))
			goto badbits;
		/**
		   Although we have done a brand new open, we may have opened the same transport
		   provider stream that was opened under another file descriptor.  The unique
		   identifier for the transport stream is the ACCEPTOR_id.  We should search the
		   list of file descriptors looking for a user structure with the indicated
		   ACCEPTOR id and simply point to that one, and increment its reference count.
		   That will ensure that we do not maintain split state machines. 
		 */
		user->flags |= TUF_SYNC_REQUIRED;
		for (i = 0; i < OPEN_MAX; i++)
			if (_t_fds[i] && _t_fds[i]->token == buf.ack.ACCEPTOR_id) {
				free(user);
				user = _t_fds[i];
				pthread_rwlock_destroy(&user->lock);
				break;
			}
		user->fflags = oflag;
		user->info.addr = buf.ack.INFO_ack.ADDR_size;
		user->info.options = buf.ack.INFO_ack.OPT_size;
		user->info.tsdu = buf.ack.INFO_ack.TSDU_size;
		user->info.etsdu = buf.ack.INFO_ack.ETSDU_size;
		user->info.connect = buf.ack.INFO_ack.CDATA_size;
		user->info.discon = buf.ack.INFO_ack.DDATA_size;
		user->info.servtype = buf.ack.INFO_ack.SERV_type;
		user->info.flags = buf.ack.INFO_ack.PROVIDER_flag;
		user->info.tidu = buf.ack.INFO_ack.TIDU_size;
		user->token = buf.ack.ACCEPTOR_id;
		user->refs++;	/* one for new structure, see memset above */
		if (user->flags & TUF_SYNC_REQUIRED)
			__xnet_u_setstate(user, buf.ack.INFO_ack.CURRENT_state);
		pthread_rwlock_init(&user->lock, NULL);	/* destroyed for existing structure */
		_t_fds[fd] = user;
		user->flags &= ~TUF_SYNC_REQUIRED;
		if (info)
			*info = user->info;
	}
	return (fd);
      badbits:
	t_errno = TPROTO;
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
      enobufs:
	errno = ENOBUFS;
	if (user->ctlbuf)
		free(user->ctlbuf);
	if (user->datbuf)
		free(user->datbuf);
	free(user);
	goto error;
      enomem:
	errno = ENOMEM;
	goto error;
      error:
	return (-1);
}

/**
  * @brief The reentrant version of __xnet_t_open().
  * @param path a character string specifying the pat to the device to open.
  * @param oflag open flags.
  * @param info a pointer to a t_info structure to contain returned information
  * about the transport endpoint.
  *
  * This is a little different than most of the _r wrappers: we take a write
  * lock on the _t_fds list so that we are able to add the new file descriptor
  * into the list.  This will block most other threads from performing
  * functions on the list, also, we must wait for a quiet period until all
  * other functions that read lock the list are not being used.  If you are
  * sure that the open will only be performed by one thread and that no other
  * thread will act on the file descriptor until open returns, use the
  * non-recursive version.
  */
int
__xnet_t_open_r(const char *path, int oflag, struct t_info *info)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__xnet_list_unlock, NULL);
	if ((err = __xnet_list_wrlock()) == 0) {
		ret = __xnet_t_open(path, oflag, info);
		__xnet_list_unlock(NULL);
	} else {
		t_errno = TSYSERR;
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_open_r,t_open@@XNET_1.0");

/** @brief Manage transport options.
  * @param fd the transport endpoint for which to manage options.
  * @param req a pointer to a t_optmgmt structure containing the requested options.
  * @param ret a pointer to a t_optmgmt structure to contain the returned options.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_optmgmt(int fd, const struct t_optmgmt *req, struct t_optmgmt *ret)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, -1, -1)))
		goto error;
#ifdef DEBUG
	if (!req)
		goto einval;
	if (req && (req->opt.len < 0 || (req->opt.len > 0 && !req->opt.buf)))
		goto einval;
	if (ret && (ret->opt.maxlen < 0 || (ret->opt.maxlen > 0 && !ret->opt.buf)))
		goto einval;
#endif
	if (req && req->opt.len > __xnet_u_max_options(user))
		goto tbadopt;
	{
		size_t opt_len = (req && req->opt.len > 0) ? req->opt.len : 0;
		size_t opt_max = min(__xnet_u_max_options(user), _T_DEFAULT_OPTLEN);
		union {
			struct {
				struct T_optmgmt_req prim;
				unsigned char opts[opt_len];
			} req;
			struct {
				struct T_optmgmt_ack prim;
				unsigned char opts[opt_max];
			} ack;
		} buf;

		buf.req.prim.PRIM_type = T_OPTMGMT_REQ;
		buf.req.prim.OPT_length = opt_len;
		buf.req.prim.OPT_offset = opt_len ? sizeof(buf.req.prim) : 0;
		buf.req.prim.MGMT_flags = req ? req->flags : 0;
		if (opt_len)
			memcpy(buf.req.opts, req->opt.buf, opt_len);
		if (__xnet_t_strioctl(fd, TI_OPTMGMT, &buf, sizeof(buf)) != 0)
			goto error;
		if (ret) {
			ret->flags = buf.ack.prim.MGMT_flags;
			if (ret->opt.maxlen < buf.ack.prim.OPT_length)
				goto tbufovflw;
			if ((ret->opt.len = buf.ack.prim.OPT_length))
				memcpy(ret->opt.buf, (char *) &buf + buf.ack.prim.OPT_offset,
				       ret->opt.len);
		}
		return (0);
	}
      tbadopt:
	t_errno = TBADOPT;
	goto error;
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbufovflw:
	t_errno = TBUFOVFLW;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_optmgmt(). */
int
__xnet_t_optmgmt_r(int fd, const struct t_optmgmt *req, struct t_optmgmt *ret)
{
	int rtv = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		rtv = __xnet_t_optmgmt(fd, req, ret);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (rtv);
}

__asm__(".symver __xnet_t_optmgmt_r,t_optmgmt@@XNET_1.0");

/** @brief Receive transport data.
  * @param fd the transport endpoint upon which to received data.
  * @param buf a pointer to a buffer to contain the received data.
  * @param nbytes the maximum number of bytes in the caller supplied buffer.
  * @param flags a pointer to an int to contain the returned data flags.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_rcv(int fd, char *buf, unsigned int nbytes, int *flags)
{
	struct _t_user *user;
	int copied = 0, event = 0, flag = 0, result;

	if (!
	    (user =
	     __xnet_t_tstuser(fd, -1, (1 << T_COTS) | (1 << T_COTS_ORD),
			      TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto error;
	if (nbytes == 0)
		return (0);
#ifdef DEBUG
	if (!flags || !buf)
		goto einval;
#endif
	{
		struct strbuf data;

		data.maxlen = nbytes;
		data.len = 0;
		data.buf = buf ? buf : NULL;
		if ((result =
		     __xnet_t_getdata(fd, &data, (T_DATA | T_EXDATA))) & (T_DATA | T_EXDATA))
			event = result;
		for (;;) {
			switch (result) {
			case T_DATA:
				if (event != T_DATA)
					goto tlook;
				flag = ((user->moresdu || user->moredat) ? T_MORE : 0);
				break;
			case T_EXDATA:
				if (event != T_EXDATA)
					goto tlook;
				flag =
				    ((user->moreedu || user->moreexp) ? T_MORE : 0) | T_EXPEDITED;
				break;
			case 0:
				goto tnodata;
			case -1:
				goto error;
			default:
				goto tlook;
			}
			if ((copied += data.len) >= nbytes)
				break;
			if (!(flag & T_MORE)) {
				__xnet_u_reset_event(user);
				break;
			}
			data.maxlen -= data.len;
			data.buf += data.len;
			data.len = 0;
			result = __xnet_t_getdata(fd, &data, event);
		}
	}
	if (flags)
		*flags = flag;
	return (copied);
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      error:
	if (copied) {
		if (flags)
			*flags = flag;
		return (copied);
	}
	return (-1);
}

/** The reentrant version of __xnet_t_rcv(). */
int
__xnet_t_rcv_r(int fd, char *buf, unsigned int nbytes, int *flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcv(fd, buf, nbytes, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcv_r,t_rcv@@XNET_1.0");

/** @brief Receive the configuration from a connection request.
  * @param fd a file descriptor for the transport endpoint.
  * @param call a pointer to a struct t_call structure.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_rcvconnect(int fd, struct t_call *call)
{
	struct _t_user *user;
	union T_primitives *p;

	if (!
	    (user =
	     __xnet_t_tstuser(fd, T_CONNECT, (1 << T_COTS) | (1 << T_COTS_ORD), TSF_WCON_CREQ)))
		goto error;
#ifdef DEBUG
	if (call && (call->addr.maxlen < 0 || (call->addr.maxlen > 0 && !call->addr.buf)))
		goto einval;
	if (call && (call->opt.maxlen < 0 || (call->opt.maxlen > 0 && !call->opt.buf)))
		goto einval;
	if (call && (call->udata.maxlen < 0 || (call->udata.maxlen > 0 && !call->udata.buf)))
		goto einval;
#endif
	p = (typeof(p)) user->ctlbuf;
	switch (__xnet_t_getevent(fd)) {
	case T_CONNECT:
	{
		__xnet_u_setstate_const(user, TS_DATA_XFER);
		if (call) {
			if (call->addr.maxlen && call->addr.maxlen < p->conn_con.RES_length)
				goto tbufovflw;
			if (call->opt.maxlen && call->opt.maxlen < p->conn_con.OPT_length)
				goto tbufovflw;
			if (call->udata.maxlen && call->udata.maxlen < user->data.len)
				goto tbufovflw;
			if (call->addr.maxlen && (call->addr.len = p->conn_con.RES_length))
				memcpy(call->addr.buf, (char *) p + p->conn_con.RES_offset,
				       call->addr.len);
			if (call->opt.maxlen && (call->opt.len = p->conn_con.OPT_length))
				memcpy(call->opt.buf, (char *) p + p->conn_con.OPT_offset,
				       call->opt.len);
			if (call->udata.maxlen && (call->udata.len = user->data.len))
				memcpy(call->udata.buf, user->data.buf, call->udata.len);
		}
		__xnet_u_reset_event(user);	/* consume the event */
		return (0);
	}
	case 0:
		goto tnodata;
	case -1:
		goto error;
	default:
		goto tlook;
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume the event */
	t_errno = TBUFOVFLW;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_rcvconnect(). */
int
__xnet_t_rcvconnect_r(int fd, struct t_call *call)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvconnect(fd, call)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvconnect_r,t_rcvconnect@@XNET_1.0");

/** @brief Retrieve information from disconnection.
  * @param fd a file descriptor for the transport endpoint.
  * @param discon a pointer to a struct t_discon structure returning the
  * disconnection reason and data.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_rcvdis(int fd, struct t_discon *discon)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, T_DISCONNECT, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WCON_CREQ | TSF_WIND_ORDREL |
				      TSF_WREQ_ORDREL | TSF_WRES_CIND)))
		goto error;
	if (user->state == T_INCON && user->ocnt < 1)
		goto toutstate;
#ifdef DEBUG
	if (discon
	    && (discon->udata.maxlen < 0 || (discon->udata.maxlen > 0 && !discon->udata.buf)))
		goto einval;
#endif
	switch (__xnet_t_getevent(fd)) {
	case T_DISCONNECT:
	{
		union T_primitives *p = (typeof(p)) user->ctrl.buf;

		if (user->ocnt && --user->ocnt)
			__xnet_u_setstate_const(user, TS_WRES_CIND);
		else
			__xnet_u_setstate_const(user, TS_IDLE);
		if (discon) {
			discon->reason = p->discon_ind.DISCON_reason;
			discon->sequence = p->discon_ind.SEQ_number;
			if (discon->udata.maxlen && discon->udata.maxlen < user->data.len)
				goto tbufovflw;
			if (discon->udata.maxlen && (discon->udata.len = user->data.len))
				memcpy(discon->udata.buf, user->data.buf, discon->udata.len);
		}
		__xnet_u_reset_event(user);	/* consume the event */
		return (0);
	}
	case 0:
		goto tnodis;
	case -1:
		goto error;
	default:
		goto tlook;
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodis:
	t_errno = TNODIS;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume the event */
	t_errno = TBUFOVFLW;
	goto error;
      toutstate:
	t_errno = TOUTSTATE;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_rcvdis(). */
int
__xnet_t_rcvdis_r(int fd, struct t_discon *discon)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_rcvdis(fd, discon);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvdis_r,t_rcvdis@@XNET_1.0");

/** @brief Receive an indication about a leaf in a point-to-multipoint connection.
  * @param fd a file descriptor for the transport endpoint.
  * @param change a pointer to a t_leaf_status structure.
  *
  * This function is NOT a thread cancellation point.
  *
  * This function is used to determine the statuc of a leaf on a
  * point-to-multipoint connection.  This function can only be used in the
  * @c T_DATAXFER state.  The parameter fd identifies the local connection
  * endpoint that serves as the root of the point-to-multipoint connection, and
  * parameter change points to a t_leaf_status structure.
  *
  * The field leafid identifier the leaf whose status has changed, and field
  * status specifies the change (either @c T_CONNECT or @c T_DISCONNECT).  When
  * status has a value of @c T_CONNECT, field reason is meaningless.  When status
  * has a value of @c T_DISCONNECT, field reason specifies the reason why the leaf
  * was removed from the point-to-multipoint connection or why a pending
  * addleaf failed, through a protocol-dependent reason code.
  */
int
__xnet_t_rcvleafchange(int fd, struct t_leaf_status *change)
{
#if defined HAVE_XTI_ATM_H
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD), TSF_DATA_XFER)))
		goto error;
#ifdef DEBUG
	if (!change)
		goto einval;
#endif
	{
		struct {
			struct t_opthdr hdr;
			struct t_atm_leaf_ind leaf;
		} opts;
		struct t_optmgmt req, ret;

		req.opt.maxlen = 0;
		req.opt.len = sizeof(opts);
		req.opt.buf = (char *) &opts;
		req.flags = T_NEGOTIATE;
		ret.opt.maxlen = sizeof(opts);
		req.opt.len = 0;
		req.opt.buf = (char *) &opts;
		req.flags = 0;
		opts.hdr.len = sizeof(opts);
		opts.hdr.level = T_ATM_SIGNALLING;
		opts.hdr.name = T_ATM_LEAF_IND;
		opts.hdr.status = 0;
		if (__xnet_t_optmgmt(fd, &req, &ret) == -1)
			goto error;
		if (ret.opt.flags == T_FAILURE)
			goto tproto;
		if (ret.opt.len < sizeof(opts) || opts.hdr.len < sizeof(opts))
			goto tproto;
		if (opts.hdr.status != T_SUCCESS)
			goto tproto;
		if (opts.hdr.level != T_ATM_SIGNALLING || opts.hdr.name != T_ATM_LEAF_IND)
			goto tproto;
		if (opts.leaf.status == T_LEAF_NOCHANGE)
			goto tnodata;
		if (change) {
			change->leafid = opts.leaf.leaf_ID;
			change->status = opts.leaf.status;
			change->reason = opts.leaf.reason;
		}
		return (0);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tproto:
	t_errno = TPROTO;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      error:
	return (-1);
#else				/* defined HAVE_XTI_ATM_H */
	t_errno = TNOTSUPPORT;
	return (-1);
#endif				/* defined HAVE_XTI_ATM_H */
}

#pragma weak __xnet_t_rcvleafchange_r
/** The reentrant version of __xnet_t_rcvleafchange(). */
int
__xnet_t_rcvleafchange_r(int fd, struct t_leaf_status *change)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_rcvleafchange(fd, change);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}


__asm__(".symver __xnet_t_rcvleafchange_r,t_rcvleafchange@@XNET_1.0");

/** @brief Acknolwedge receipt of an orderly release indication.
  * @param fd the transport endpoint upon which to acknowledge the release indication.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_rcvrel(int fd)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, T_ORDREL, (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto error;
	switch (__xnet_t_getevent(fd)) {
	case T_ORDREL:
	{
		user->event = 0;
		if (user->state == T_DATAXFER)
			__xnet_u_setstate(user, TS_WREQ_ORDREL);
		if (user->state == T_OUTREL)
			__xnet_u_setstate(user, TS_IDLE);
		return (0);
	}
	case 0:
		goto tnorel;
	default:
		goto tlook;
	case -1:
		goto error;
	}
      tlook:
	t_errno = TLOOK;
	goto error;
      tnorel:
	t_errno = TNOREL;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_rcvrel(). */
int
__xnet_t_rcvrel_r(int fd)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvrel(fd)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvrel_r,t_rcvrel@@XNET_1.0");

/** @brief Acknolwedge receipt of an orderly release indication.
  * @param fd the transport endpoint upon which to acknowledge the release indication.
  * @param discon
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_rcvreldata(int fd, struct t_discon *discon)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, T_ORDREL, (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto error;
#ifdef DEBUG
	if (discon
	    && (discon->udata.maxlen < 0 || (discon->udata.maxlen > 0 && !discon->udata.buf)))
		goto einval;
#endif
	switch (__xnet_t_getevent(fd)) {
	case T_ORDREL:
	{
		union T_primitives *p = (typeof(p)) user->ctrl.buf;

		if (user->state == T_DATAXFER)
			__xnet_u_setstate(user, TS_WREQ_ORDREL);
		if (user->state == T_OUTREL)
			__xnet_u_setstate(user, TS_IDLE);
		if (discon) {
			discon->reason = 0;
			discon->sequence = 0;
			if (discon->udata.maxlen && discon->udata.maxlen < user->data.len)
				goto tbufovflw;
			if (discon->udata.maxlen && (discon->udata.len = user->data.len))
				memcpy(discon->udata.buf, user->data.buf, discon->udata.len);
		}
		__xnet_u_reset_event(user);	/* consume event */
		return (0);
	}
	case 0:
		goto tnorel;
	case -1:
		goto error;
	default:
		goto tlook;
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnorel:
	t_errno = TNOREL;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume event */
	t_errno = TBUFOVFLW;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_rcvreldata(). */
int
__xnet_t_rcvreldata_r(int fd, struct t_discon *discon)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvreldata(fd, discon)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvreldata_r,t_rcvreldata@@XNET_1.0");

/** @brief Receive data or expedited data with options.
  * @param fd a file descriptor for the transport endpoing.
  * @param optdata a pointer to a struct t_unitdata structure returning the
  * address, options and data associated with the data unit.
  * @param flags a pointer to an integer flags word returning flags associated
  * with the receive.
  *
  * @bug This function has to be converted to not place data in the look buffer
  * but into the user-supplied data area.  Only if the wrong event occurs,
  * should the data be moved to the look data area. 
  *
  * @remark There is no such function as t_rcvopt() under XTI.  The purpose
  * here is to try to support SCTP (which has many packet-specific options)
  * as well as passing of credentials on connection-oriented loopback and UNIX
  * domain sockets.  Note, however, that t_rcvudata() has the same prototype
  * and could be overloaded to support T_COTS and T_COTS_ORD as well.
  */
int
__xnet_t_rcvopt(int fd, struct t_unitdata *optdata, int *flags)
{
	struct _t_user *user;
	int copied = 0;

	if (!(user = __xnet_t_tstuser(fd, -1, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto error;
#ifdef DEBUG
	if (!optdata)
		goto einval;
	if (!flags)
		goto einval;
#endif
	if (!optdata)
		return (0);
	switch (__xnet_t_getevent(fd)) {
	case T_DATA:
	case T_EXDATA:
	{
		union T_primitives *p = (typeof(p)) user->ctrl.buf;

		switch (user->prim) {
		case T_DATA_IND:
			if (optdata->addr.maxlen >= 0)
				optdata->addr.len = 0;
			if (optdata->opt.maxlen >= 0)
				optdata->opt.len = 0;
			if (optdata->udata.maxlen
			    && (optdata->udata.maxlen =
				min(optdata->udata.maxlen, user->data.len))) {
				memcpy(optdata->udata.buf, user->data.buf, optdata->udata.len);
				user->data.len -= optdata->udata.len;
				user->data.buf += optdata->udata.len;
			}
			if (flags)
				*flags |= (user->moresdu || user->moredat
					   || user->data.len > 0) ? T_MORE : 0;
			break;
		case T_EXDATA_IND:
			if (optdata->addr.maxlen >= 0)
				optdata->addr.len = 0;
			if (optdata->opt.maxlen >= 0)
				optdata->opt.len = 0;
			if (optdata->udata.maxlen
			    && (optdata->udata.len = min(optdata->udata.maxlen, user->data.len))) {
				memcpy(optdata->udata.buf, user->data.buf, optdata->udata.len);
				user->data.len -= optdata->udata.len;
				user->data.buf += optdata->udata.len;
			}
			if (flags)
				*flags |= (user->moreedu || user->moreexp
					   || user->data.len >
					   0) ? T_MORE | T_EXPEDITED : T_EXPEDITED;
			break;
		case T_OPTDATA_IND:
			if (optdata->addr.maxlen >= 0)
				optdata->addr.len = 0;
			if (optdata->opt.maxlen && optdata->opt.maxlen < p->optdata_ind.OPT_length)
				goto tbufovflw;
			if (optdata->opt.maxlen && (optdata->opt.len = p->optdata_ind.OPT_length))
				memcpy(optdata->opt.buf, (char *) p + p->optdata_ind.OPT_offset,
				       optdata->opt.len);
			if (optdata->udata.maxlen
			    && (optdata->udata.len = min(optdata->udata.maxlen, user->data.len))) {
				memcpy(optdata->udata.buf, user->data.buf, optdata->udata.len);
				user->data.len -= optdata->udata.len;
				user->data.buf += optdata->udata.len;
			}
			if (p->optdata_ind.DATA_flag & T_ODF_EX) {
				if (flags)
					*flags |= (user->moreedu || user->moreexp
						   || user->data.len >
						   0) ? T_MORE | T_EXPEDITED : T_EXPEDITED;
			} else {
				if (flags)
					*flags |= (user->moresdu || user->moredat
						   || user->data.len > 0) ? T_MORE : 0;
			}
			break;
		default:
			goto tlook;
		}
		__xnet_u_reset_event(user);	/* consume event */
		return (0);
	}
	case 0:
		goto tnodata;
	case -1:
		goto error;
	default:
		goto tlook;
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume event */
	t_errno = TBUFOVFLW;
	goto error;
      error:
	if (!copied)
		return (-1);
	return (copied);
}

/** The reentrant version of __xnet_t_rcvopt().
  * @param fd a file descriptor for the transport endpoing.
  * @param optdata a pointer to a struct t_unitdata structure returning the
  * address, options and data associated with the data unit.
  * @param flags a pointer to an integer flags word returning flags associated
  * with the receive.
  *
  * This function is a thread cancellation point.
  *
  * @note Although ioctl(2) is also a thread cancellation point, care must be
  * taken to release the locks held by the thread before asynchronous
  * cancellation occurs, or at least that a cleanup function is provided.
  */
int
__xnet_t_rcvopt_r(int fd, struct t_unitdata *optdata, int *flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvopt(fd, optdata, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvopt_r,t_rcvopt@@XNET_1.0");

/** @brief Receive a data unit.
  * @param fd a file descriptor for the transport endpoint.
  * @param unitdata a pointer to a t_unitdata structure describing the address, options and data of the data unit.
  * @param flags a pointer to integer flags returned.
  *
  * This function is a thread cancellation point.
  *
  * @bug This function has to be converted to not place data in the look buffer
  * but into the user-supplied data area.  Only if the wrong event occurs,
  * should the data be moved to the look data area. 
  */
__hot int
__xnet_t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags)
{
	struct _t_user *user;
	int copied = 0, flag = 0, result;

	if (unlikely(!(user = __xnet_t_tstuser(fd, T_DATA, (1 << T_CLTS), TSF_IDLE))))
		goto error;
#ifdef DEBUG
	if (!unitdata)
		goto einval;
	if (!flags)
		goto einval;
#endif
	if (unlikely(!unitdata))
		goto done;
	{
		struct strbuf data;

		data.maxlen = likely(unitdata->udata.maxlen > 0)
		    ? unitdata->udata.maxlen : 0;
		data.len = 0;
		data.buf = unitdata->udata.buf;
		if (likely((result = __xnet_t_getdata(fd, &data, T_DATA)) == T_DATA)) {
			union T_primitives *p = (typeof(p)) user->ctrl.buf;

			if (likely(user->prim == T_UNITDATA_IND)) {
				if (unlikely(unitdata->addr.maxlen)
				    && unlikely(unitdata->addr.maxlen < p->unitdata_ind.SRC_length))
					goto tbufovflw;
				if (unlikely(unitdata->opt.maxlen)
				    && unlikely(unitdata->opt.maxlen < p->unitdata_ind.OPT_length))
					goto tbufovflw;
				if (unlikely(unitdata->addr.maxlen)
				    && (unitdata->addr.len = p->unitdata_ind.SRC_length))
					memcpy(unitdata->addr.buf,
					       (char *) p + p->unitdata_ind.SRC_offset,
					       unitdata->addr.len);
				if (unlikely(unitdata->opt.maxlen)
				    && (unitdata->opt.len = p->unitdata_ind.OPT_length))
					memcpy(unitdata->opt.buf,
					       (char *) p + p->unitdata_ind.OPT_offset,
					       unitdata->opt.len);
			} else {
				unitdata->addr.len = 0;
				unitdata->opt.len = 0;
			}
		}
		for (;;) {
			if (likely(result == T_DATA))
				flag = (user->moresdu || user->moredat) ? T_MORE : 0;
			else if (result == 0)
				goto tnodata;
			else if (result == -1)
				goto error;
			else
				goto tlook;

			if (likely((copied += data.len) >= unitdata->udata.maxlen))
				break;
			if (likely(!(flag & T_MORE))) {
				__xnet_u_reset_event(user);
				break;
			}
			data.maxlen -= data.len;
			data.len = 0;
			data.buf += data.len;
			result = __xnet_t_getdata(fd, &data, T_DATA);
		}
	}
	if (flags)
		*flags = flag;
	unitdata->udata.len = copied;
      done:
	return (0);
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume event */
	t_errno = TBUFOVFLW;
	goto error;
      error:
	if (flags)
		*flags = flag;
	unitdata->udata.len = copied;
	return (-1);
}

/** The reentrant version of __xnet_t_rcvudata(). */
__hot int
__xnet_t_rcvudata_r(int fd, struct t_unitdata *unitdata, int *flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvudata(fd, unitdata, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvudata_r,t_rcvudata@@XNET_1.0");

/** @brief Receive a unit data error indication.
  * @param fd a file descriptor for the transport endpoint.
  * @param uderr a pointer to a struct t_uderr structure returning the address,
  * options and error code.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_rcvuderr(int fd, struct t_uderr *uderr)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, T_UDERR, (1 << T_CLTS), TSF_IDLE)))
		goto error;
#ifdef DEBUG
	if (uderr && (uderr->addr.maxlen < 0 || (uderr->addr.maxlen > 0 && !uderr->addr.buf)))
		goto einval;
	if (uderr && (uderr->opt.maxlen < 0 || (uderr->opt.maxlen > 0 && !uderr->opt.buf)))
		goto einval;
#endif
	switch (__xnet_t_getevent(fd)) {
	case T_UDERR:
	{
		union T_primitives *p = (typeof(p)) user->ctrl.buf;

		if (uderr) {
			uderr->error = p->uderror_ind.ERROR_type;
			if (uderr->addr.maxlen && uderr->addr.maxlen < p->uderror_ind.DEST_length)
				goto tbufovflw;
			if (uderr->opt.maxlen && uderr->opt.maxlen < p->uderror_ind.OPT_length)
				goto tbufovflw;
			if (uderr->addr.maxlen && (uderr->addr.len = p->uderror_ind.DEST_length))
				memcpy(uderr->addr.buf, (char *) p + p->uderror_ind.DEST_offset,
				       uderr->addr.len);
			if (uderr->opt.maxlen && (uderr->opt.len = p->uderror_ind.OPT_length))
				memcpy(uderr->opt.buf, (char *) p + p->uderror_ind.OPT_offset,
				       uderr->opt.len);
		}
		__xnet_u_reset_event(user);
		return (0);
	}
	case 0:
		goto tnouderr;
	case -1:
		goto error;
	default:
		goto tlook;
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbufovflw:
	__xnet_u_reset_event(user);
	t_errno = TBUFOVFLW;
	goto error;
      tnouderr:
	t_errno = TNOUDERR;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	return (-1);
}

/** The reentrant version of __xnet_t_rcvuderr(). */
int
__xnet_t_rcvuderr_r(int fd, struct t_uderr *uderr)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_rcvuderr(fd, uderr);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvuderr_r,t_rcvuderr@@XNET_1.0");

/** @brief Read data or expedited data and scatter.
  * @param fd a file descriptor for the transport endpoint.
  * @param iov a pointer to an array of struct t_iovec io vectors.
  * @param iovcount the count of the elements in the t_iovec array.
  * @param flags a pointer to an integer flags word to contain the returned
  * message flags.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_rcvv(int fd, struct t_iovec *iov, unsigned int iovcount, int *flags)
{
	struct _t_user *user;
	int copied = 0, nbytes, n = 0, event = 0, flag = 0, result;

	if (!(user = __xnet_t_tstuser(fd, -1, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto error;
#ifdef DEBUG
	if (!flags)
		goto einval;
	if (!iov)
		goto einval;
#endif
	if (!iov)
		return (0);
	for (nbytes = 0, n = 0; n < iovcount; nbytes += iov[n].iov_len, n++) ;
	if (iovcount == 0 || iovcount > _T_IOV_MAX)
		goto tbaddata;
	{
		struct strbuf data;

		data.maxlen = iov[0].iov_len;
		data.len = 0;
		data.buf = iov[0].iov_base;
		if ((result =
		     __xnet_t_getdata(fd, &data, (T_DATA | T_EXDATA))) & (T_DATA | T_EXDATA))
			event = result;
		for (n = 0;;) {
			switch (result) {
			case T_DATA:
				if (event != T_DATA)
					goto tlook;
				flag = ((user->moresdu || user->moredat) ? T_MORE : 0);
				break;
			case T_EXDATA:
				if (event != T_EXDATA)
					goto tlook;
				flag =
				    ((user->moreedu || user->moreexp) ? T_MORE : 0) | T_EXPEDITED;
				break;
			case 0:
				goto tnodata;
			case -1:
				goto error;
			default:
				goto tlook;
			}
			if ((copied += data.len) >= nbytes)
				break;
			if (!(flag & T_MORE)) {
				__xnet_u_reset_event(user);
				break;
			}
			if (data.maxlen <= data.len) {
				if (++n >= iovcount)
					break;
				data.maxlen = iov[n].iov_len;
				data.buf = iov[n].iov_base;
				data.len = 0;
			} else {
				data.maxlen -= data.len;
				data.buf += data.len;
				data.len = 0;
			}
			result = __xnet_t_getdata(fd, &data, event);
		}
	}
	if (flags)
		*flags = flag;
	return (copied);
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      error:
	if (!copied)
		return (-1);
	if (flags)
		*flags = flag;
	return (copied);
}

/** The reentrant version of __xnet_t_rcvv(). */
int
__xnet_t_rcvv_r(int fd, struct t_iovec *iov, unsigned int iovcount, int *flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvv(fd, iov, iovcount, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvv_r,t_rcvv@@XNET_1.0");

/** @brief Receive data unit with scatter.
  * @param fd a file descriptor for the transport endpoint.
  * @param unitdata a pointer to a struct t_unitdata structure to contain the
  * received address, options and data unit.
  * @param iov a pointer to an array of struct t_iovec io vectors.
  * @param iovcount the count of the elements in the t_iovec array.
  * @param flags a pointer to an integer flags word to contain the returned
  * message flags.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_rcvvudata(int fd, struct t_unitdata *unitdata, struct t_iovec *iov, unsigned int iovcount,
		   int *flags)
{
	struct _t_user *user;
	int copied = 0, nbytes, n = 0, flag = 0, result;

	if (!(user = __xnet_t_tstuser(fd, T_DATA, (1 << T_CLTS), TSF_IDLE)))
		goto error;
	for (nbytes = 0, n = 0; n < iovcount; nbytes += iov[n].iov_len, n++) ;
	if (iovcount == 0 || iovcount > _T_IOV_MAX)
		goto tbaddata;
#ifdef DEBUG
	if (!unitdata)
		goto einval;
	if (!flags)
		goto einval;
#endif
	{
		struct strbuf data;

		data.maxlen = iov[0].iov_len;
		data.len = 0;
		data.buf = iov[0].iov_base;
		if ((result = __xnet_t_getdata(fd, &data, T_DATA)) == T_DATA) {
			union T_primitives *p = (typeof(p)) user->ctrl.buf;

			switch (user->prim) {
			case T_UNITDATA_IND:
				if (unitdata) {
					if (unitdata->addr.maxlen
					    && unitdata->addr.maxlen < p->unitdata_ind.SRC_length)
						goto tbufovflw;
					if (unitdata->opt.maxlen
					    && unitdata->opt.maxlen < p->unitdata_ind.OPT_length)
						goto tbufovflw;
					if (unitdata->addr.maxlen
					    && (unitdata->addr.len = p->unitdata_ind.OPT_length))
						memcpy(unitdata->addr.buf,
						       (char *) p + p->unitdata_ind.SRC_offset,
						       unitdata->addr.len);
					if (unitdata->opt.maxlen
					    && (unitdata->opt.len = p->unitdata_ind.OPT_length))
						memcpy(unitdata->opt.buf,
						       (char *) p + p->unitdata_ind.OPT_offset,
						       unitdata->opt.len);
				}
				break;
			default:
				if (unitdata) {
					unitdata->addr.len = 0;
					unitdata->opt.len = 0;
				}
				break;
			}
		}
		for (;;) {
			switch (result) {
			case T_DATA:
				flag = (user->moresdu || user->moredat) ? T_MORE : 0;
				break;
			case 0:
				goto tnodata;
			case -1:
				goto error;
			default:
				goto tlook;
			}
			if ((copied += data.len) >= nbytes)
				break;
			if (!(flag & T_MORE)) {
				__xnet_u_reset_event(user);
				break;
			}
			if (data.maxlen <= data.len) {
				if (++n >= iovcount)
					break;
				data.maxlen = iov[n].iov_len;
				data.len = 0;
				data.buf = iov[n].iov_base;
			} else {
				data.maxlen -= data.len;
				data.len = 0;
				data.buf += data.len;
			}
			result = __xnet_t_getdata(fd, &data, T_DATA);
		}
	}
	if (flags)
		*flags = flag;
	if (unitdata)
		unitdata->udata.len = copied;
	return (0);
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tlook:
	t_errno = TLOOK;
	goto error;
      tnodata:
	t_errno = TNODATA;
	goto error;
      tbufovflw:
	__xnet_u_reset_event(user);	/* consume event */
	t_errno = TBUFOVFLW;
	goto error;
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      error:
	if (flags)
		*flags = flag;
	if (unitdata)
		unitdata->udata.len = copied;
	return (-1);
}

/** The reentrant version of __xnet_t_rcvvudata(). */
int
__xnet_t_rcvvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
		     unsigned int iovcount, int *flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_rcvvudata(fd, unitdata, iov, iovcount, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_rcvvudata_r,t_rcvvudata@@XNET_1.0");

/** @brief Drop a leaf from a point-to-multipoint connection.
  * @param fd a file descriptor for the transport user endpoint.
  * @param leafid the identifier of the leaf to be removed.
  * @param reason the reason for removal.
  *
  * t_removeleaf() is NOT a thread cancellation point, but it calls
  * __xnet_t_optmgmt() which contains ioctl() which may be a cancellation
  * point; so, thread cancellation is disabled across calling this function.
  *
  * This XTI Library function is only used with ATM.  It is used to remove a
  * leaf from a point to multipoint connection.  TPI needs ATM extensions to be
  * able to perform this function.
  *
  * Note that leafid and reason are protocol specific values.
  */
int
__xnet_t_removeleaf(int fd, int leafid, int reason)
{
#if defined HAVE_XTI_ATM_H
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD), TSF_DATA_XFER)))
		goto error;
	{
		struct {
			struct t_opthdr hdr;
			struct t_atm_drop_leaf leaf;
		} opts;
		struct t_optmgmt req, ret;

		req.opt.maxlen = 0;
		req.opt.len = sizeof(opts);
		req.opt.buf = (char *) &opts;
		req.flags = T_CURRENT;
		ret.opt.maxlen = sizeof(opts);
		ret.opt.len = 0;
		ret.opt.buf = (char *) &opts;
		ret.flags = 0;
		opts.hdr.len = sizeof(opts);
		opts.hdr.level = T_ATM_SIGNALLING;
		opts.hdr.name = T_ATM_DROP_LEAF;
		opts.hdr.status = 0;
		opts.leaf.leaf_ID = leafid;
		opts.leaf.reason = reason;
		if (__xnet_t_optmgmt(fd, &req, &ret) == -1)
			goto error;
		if (ret.opt.flags == T_FAILURE)
			goto tproto;
		if (ret.opt.len < sizeof(opts) || opts.hdr.len < sizeof(opts))
			goto tproto;
		if (opts.hdr.status != T_SUCCESS)
			goto tproto;
		if (opts.hdr.level != T_ATM_SIGNALLING || opts.hdr.name != T_ATM_DROP_LEAF)
			goto tproto;
		/** The t_addleaf function requires an operating-system specific blocking mechanism
		   to know when to check for the leaf status indication.  The most obvious approach 
		   is to have the ATM transport service provider send a signal to the stream head
		   when an indication has arrived.  What signal (SIGPOLL, SIGIO, SIGUSR) is a
		   question. Nevertheless, we can check our open flags here and either decide to
		   block on a poll or not to check for the indication that the addition was
		   successful.  We do not do this yet.  We just always return nodata.  It is the
		   caller's responsibility to check with t_rcvleafchange(). 
		 */
		goto tnodata;
	}
      tnodata:
	t_errno = TNODATA;
	goto error;
      error:
	return (-1);
#else				/* defined HAVE_XTI_ATM_H */
	t_errno = TNOTSUPPORT;
	return (-1);
#endif				/* defined HAVE_XTI_ATM_H */
}

#pragma weak __xnet_t_removeleaf_r
/** The reentrant version of __xnet_t_removeleaf(). */
int
__xnet_t_removeleaf_r(int fd, int leafid, int reason)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_removeleaf(fd, leafid, reason);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_removeleaf_r,t_removeleaf@@XNET_1.0");

/** @brief Send data or expedited data over a connection.
  * @param fd a file descriptor for the transport endpoint.
  * @param buf a pointer to a character buffer containing the information to
  * send.
  * @param nbytes the number of bytes in the character buffer.
  * @param flags flags to specify the nature of the transmission.
  *
  * This function is a thread cancellation point.
  *
  * void *buf or char *buf is the big question here...
  *
  * User of the @c T_PUSH flag: we ignore it because we don't accumulate any data.
  * Also, there is no way to communicate it using TPI.
  *
  * Also, note that if the sending is interrupted or asynchronous and flow
  * controlled yet returns a positive non-zero number less than nbytes (partial
  * write), then we will properly set errno and t_errno anyway and the can be
  * examined by the caller to determine which action cause the partial write.
  */
int
__xnet_t_snd(int fd, char *buf, unsigned int nbytes, int flags)
{
	struct _t_user *user;
	int written = 0;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
#ifdef DEBUG
	if (!buf)
		goto einval;
#endif
	if (nbytes == 0 && !(user->info.flags & T_SNDZERO))
		goto tbaddata;
	if (nbytes > ((flags & T_EXPEDITED) ? __xnet_u_max_etsdu(user) : __xnet_u_max_tsdu(user)))
		goto tbaddata;
	if (!buf)
		return (0);
	{
		int band = (flags & T_EXPEDITED) ? 1 : 0;
		struct T_exdata_req prim;	/* same as T_data_req */
		struct strbuf ctrl, data;

		ctrl.maxlen = 0;
		ctrl.len = sizeof(prim);
		ctrl.buf = (char *) &prim;
		data.maxlen = 0;
		data.len = nbytes;
		data.buf = buf;
		prim.PRIM_type = (flags & T_EXPEDITED) ? T_EXDATA_REQ : T_DATA_REQ;
		do {
			data.len = min(__xnet_u_max_tidu(user), nbytes - written);
			data.buf = buf + written;
			prim.MORE_flag = (written + data.len < nbytes) || (flags & T_MORE);
			if (__xnet_t_putpmsg(fd, &ctrl, &data, band, MSG_BAND) == -1)
				goto error;
			written += data.len;
		} while (written < nbytes);
		return (written);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (!written) {
		if (t_errno != TLOOK && __xnet_t_peek(fd))
			goto tlook;
		return (-1);
	}
	return (written);
}

/** The reentrant version of __xnet_t_snd(). */
int
__xnet_t_snd_r(int fd, char *buf, unsigned int nbytes, int flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_snd(fd, buf, nbytes, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_snd_r,t_snd@@XNET_1.0");

/** @brief Initiate disconnect.
  * @param fd a file descriptor for the transport endpoing.
  * @param call a pointer to a struct t_call structure describing the
  * disconnection reason and data.
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_snddis(int fd, const struct t_call *call)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER |
				      TSF_WIND_ORDREL | TSF_WREQ_ORDREL)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
	if (user->state == T_INREL && (!call || (user->ocnt > 1 && !call->sequence)))
		goto tbadseq;
#ifdef DEBUG
	if (call && (call->udata.len < 0 || (call->udata.len > 0 && !call->udata.buf)))
		goto einval;
#endif
	if (call && call->udata.len > __xnet_u_max_discon(user))
		goto tbaddata;
	{
		size_t dat_len = (call && call->udata.len > 0) ? call->udata.len : 0;
		struct {
			struct T_discon_req prim;
			unsigned char udata[dat_len];
		} req;

		req.prim.PRIM_type = T_DISCON_REQ;
		req.prim.SEQ_number = call ? call->sequence : 0;
		if (dat_len)
			memcpy(req.udata, call->udata.buf, dat_len);
		switch (user->state) {
		case T_DATAXFER:
		case T_OUTCON:
		case T_OUTREL:
		case T_INREL:
			if (__xnet_t_strioctl(fd, TI_SETPEERNAME, &req, sizeof(req)) != 0)
				goto error;
			__xnet_u_setstate_const(user, TS_IDLE);
			return (0);
		case T_INCON:
			if (__xnet_t_strioctl(fd, TI_SETMYNAME, &req, sizeof(req)) != 0)
				goto error;
			if (user->ocnt > 0 && --user->ocnt)
				__xnet_u_setstate_const(user, TS_WRES_CIND);
			else
				__xnet_u_setstate_const(user, TS_IDLE);
			return (0);
		default:
			goto toutstate;
		}
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tbadseq:
	t_errno = TBADSEQ;
	goto error;
      toutstate:
	t_errno = TOUTSTATE;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (t_errno != TLOOK && __xnet_t_peek(fd) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_snddis(). */
int
__xnet_t_snddis_r(int fd, const struct t_call *call)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_snddis(fd, call);
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_snddis_r,t_snddis@@XNET_1.0");

/** @brief Initiate an orderly release.
  * @param fd a file descriptor for the transport endpoint
  *
  * This function is a thread cancellation point.
  *
  * Although XNS 5.2 describes the discon->reason as being a protocol dependent
  * reason code, there is no way in TPI to send a reason code, so it is
  * ignored.
  */
int
__xnet_t_sndrel(int fd)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS_ORD), TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
	{
		struct T_ordrel_req prim;
		struct strbuf ctrl;

		prim.PRIM_type = T_ORDREL_REQ;
		ctrl.maxlen = sizeof(prim);
		ctrl.len = sizeof(prim);
		ctrl.buf = (char *) &prim;
		if (__xnet_t_putmsg(fd, &ctrl, NULL, 0) != 0)
			goto error;
		switch (user->state) {
		case T_DATAXFER:
			__xnet_u_setstate_const(user, TS_WIND_ORDREL);
			break;
		case T_INREL:
			__xnet_u_setstate_const(user, TS_IDLE);
			break;
		}
		return (0);
	}
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (t_errno != TLOOK && __xnet_t_peek(fd) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_sndrel(). */
int
__xnet_t_sndrel_r(int fd)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndrel(fd)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndrel_r,t_sndrel@@XNET_1.0");

/** @brief Initiate an orderly release with data.
  * @param fd a file descriptor for the transport endpoint.
  * @param discon a pointer to a struct t_discon structure containing the
  * release reason and data.
  *
  * This function is a thread cancellation point.
  *
  * Although XNS 5.2 describes the discon->reason as being a protocol dependent
  * reason code, there is no way in TPI to send a reason code, so it is
  * ignored.
  */
int
__xnet_t_sndreldata(int fd, struct t_discon *discon)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS_ORD), TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
	if (!(user->info.flags & T_ORDRELDATA))
		goto tnotsupport;
#ifdef DEBUG
	if (discon && (discon->udata.len < 0 || (discon->udata.len > 0 && !discon->udata.buf)))
		goto einval;
#endif
	if (discon && discon->udata.len > __xnet_u_max_discon(user))
		goto tbaddata;
	{
		struct T_ordrel_req prim;
		struct strbuf ctrl, data;

		prim.PRIM_type = T_ORDREL_REQ;
		ctrl.maxlen = sizeof(prim);
		ctrl.len = sizeof(prim);
		ctrl.buf = (char *) &prim;
		data.maxlen = discon ? discon->udata.maxlen : 0;
		data.len = discon ? discon->udata.len : 0;
		data.buf = discon ? discon->udata.buf : NULL;
		if (__xnet_t_putmsg(fd, &ctrl, &data, 0) != 0)
			goto error;
		switch (user->state) {
		case T_DATAXFER:
			__xnet_u_setstate_const(user, TS_WIND_ORDREL);
			break;
		case T_INREL:
			__xnet_u_setstate_const(user, TS_IDLE);
			break;
		}
		return (0);
	}
      tbaddata:
	t_errno = TBADDATA;
	goto error;
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tnotsupport:
	t_errno = TNOTSUPPORT;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (t_errno != TLOOK && __xnet_t_peek(fd) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_sndreldata(). */
int
__xnet_t_sndreldata_r(int fd, struct t_discon *discon)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndreldata(fd, discon)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndreldata_r,t_sndreldata@@XNET_1.0");

/** @brief Send data or expedited data with options.
  * @param fd a file descriptor for the transport endpoint.
  * @param optdata a pointer to a struct t_unitdata structure containing the
  * options for the send.
  * @param flags flags to control the send.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_sndopt(int fd, const struct t_unitdata *optdata, int flags)
{
	struct _t_user *user;
	int written = 0;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WIND_ORDREL)))
		goto error;
#ifdef DEBUG
	if (!optdata)
		goto einval;
#endif
	if (optdata && optdata->opt.len > __xnet_u_max_options(user))
		goto tbadopt;
	if (optdata && optdata->udata.len > __xnet_u_max_tsdu(user))
		goto tbaddata;
	if (optdata && optdata->udata.len == 0 && !(user->info.flags & T_SNDZERO))
		goto tbaddata;
	{
		int band = (flags & T_EXPEDITED) ? 1 : 0;
		size_t opt_len = (optdata && optdata->opt.len) > 0 ? optdata->opt.len : 0;
		size_t dat_len = (optdata && optdata->udata.len) > 0 ? optdata->udata.len : 0;
		struct {
			struct T_optdata_req prim;
			unsigned char opt[opt_len];
		} req;
		struct strbuf ctrl, data;

		ctrl.maxlen = 0;
		ctrl.len = sizeof(req);
		ctrl.buf = (char *) &req;
		data.maxlen = 0;
		data.len = dat_len;
		data.buf = optdata ? optdata->udata.buf : NULL;
		req.prim.PRIM_type = T_OPTDATA_REQ;
		req.prim.DATA_flag =
		    ((flags & T_EXPEDITED) ? T_ODF_EX : 0) | ((flags & T_MORE) ? T_ODF_MORE : 0);
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = opt_len ? sizeof(req.prim) : 0;
		if (opt_len)
			memcpy(req.opt, optdata->opt.buf, opt_len);
		while (written < dat_len) {
			data.len = min(__xnet_u_max_tidu(user), dat_len - written);
			data.buf = optdata->udata.buf + written;
			req.prim.DATA_flag = (((flags & T_EXPEDITED)) ? T_ODF_EX : 0)
			    | (((flags & T_MORE) || (written + data.len < dat_len)) ? T_ODF_MORE :
			       0);
			if (__xnet_t_putpmsg(fd, &ctrl, &data, band, MSG_BAND))
				goto error;
			written += data.len;
		}
		return (written);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbadopt:
	t_errno = TBADOPT;
	goto error;
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      error:
	if (!written)
		return (-1);
	return (written);
}

/** The reentrant version of __xnet_t_sndopt(). */
int
__xnet_t_sndopt_r(int fd, const struct t_unitdata *optdata, int flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndopt(fd, optdata, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndopt_r,t_sndopt@@XNET_1.0");

/** @brief Gather and send data or expedited data with options.
  * @param fd a file descriptor for the transport endpoint.
  * @param optdata a pointer to a struct t_unitdata structure containing the
  * options for the send.
  * @param iov a pointer to a struct t_iovec array containing the io vectors
  * for the send.
  * @param iovcount the count of the number of elements in the struct t_iovec
  * array.
  * @param flags flags to control the send.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_sndvopt(int fd, const struct t_unitdata *optdata, const struct t_iovec *iov,
		 unsigned int iovcount, int flags)
{
	struct _t_user *user;
	int written = 0, nbytes, i;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
#ifdef DEBUG
	if (!iov)
		goto einval;
#endif
	for (nbytes = 0, i = 0; i < iovcount; nbytes += iov[i].iov_len, i++) ;
	if (nbytes == 0 && !(user->info.flags & T_SNDZERO))
		goto tbaddata;
	if (nbytes > ((flags & T_EXPEDITED) ? __xnet_u_max_etsdu(user) : __xnet_u_max_tsdu(user)))
		goto tbaddata;
	{
		size_t opt_len = (optdata && optdata->opt.len > 0) ? optdata->opt.len : 0;
		int band = (flags & T_EXPEDITED) ? 1 : 0;
		int n = 0, partial = 0;
		struct {
			struct T_optdata_req prim;
			unsigned char opt[opt_len];
		} req;
		struct strbuf ctrl, data;

		ctrl.maxlen = 0;
		ctrl.len = sizeof(req);
		ctrl.buf = (char *) &req;
		data.maxlen = 0;
		data.len = nbytes;
		data.buf = optdata ? optdata->udata.buf : NULL;
		req.prim.PRIM_type = T_OPTDATA_REQ;
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = opt_len ? sizeof(req.prim) : 0;
		if (opt_len)
			memcpy(req.opt, optdata->opt.buf, opt_len);
		do {
			do {
				data.len = min(__xnet_u_max_tidu(user), iov[n].iov_len - partial);
				data.buf = iov[n].iov_base + partial;
				req.prim.DATA_flag = (((flags & T_EXPEDITED)) ? T_ODF_EX : 0) |
				    (((flags & T_MORE)
				      || (written + data.len < nbytes)) ? T_ODF_MORE : 0);
				if (__xnet_t_putpmsg(fd, &ctrl, &data, band, MSG_BAND) == -1)
					goto error;
				partial += data.len;
				written += data.len;
			} while (partial < iov[n].iov_len);
			n++;
			partial = 0;
		} while (written < nbytes && n < iovcount);
		return (written);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      error:
	if (!written)
		return (-1);
	return (written);
}

/** The reentrant version of __xnet_t_sndvopt(). */
int
__xnet_t_sndvopt_r(int fd, const struct t_unitdata *optdata, const struct t_iovec *iov,
		   unsigned int iovcount, int flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndvopt(fd, optdata, iov, iovcount, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndvopt_r,t_sndvopt@@XNET_1.0");

/** @brief Send a data unit.
  * @param fd a file descriptor for the transport endpoint.
  * @param unitdata a pointer to a struct t_unitdata structure containing the
  * address, options and data unit for the send.
  *
  * This function is a thread cancellation point.
  *
  * This XTI call is normally only used for connection-less mode services.
  * However, we permit the call to be used for connection-oriented services
  * where we want to provide option data with the send (i.e. invoke a
  * @c T_OPTDATA_REQ).  There is no other way in XTI of generating option data
  * with a transmission.
  */
__hot int
__xnet_t_sndudata(int fd, const struct t_unitdata *unitdata)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, T_DATA, (1 << T_CLTS), TSF_IDLE)))
		goto error;
#if 1
	if ((__xnet_t_peek(fd) & ~T_DATA) > 0)
		goto tlook;
#endif
#ifdef DEBUG
	if (unlikely(!unitdata))
		goto einval;
	if (unlikely(unitdata->addr.len < 0 || (unitdata->addr.len > 0 && !unitdata->addr.buf)))
		goto einval;
	if (unlikely(unitdata->opt.len < 0 || (unitdata->opt.len > 0 && !unitdata->opt.buf)))
		goto einval;
	if (unlikely(unitdata->udata.len < 0 || (unitdata->udata.len > 0 && !unitdata->udata.buf)))
		goto einval;
#endif
	if (likely(unitdata)) {
		if (unlikely(unitdata->addr.len > __xnet_u_max_addr(user)))
			goto tbadaddr;
		if (unlikely(unitdata->opt.len > __xnet_u_max_options(user)))
			goto tbadopt;
		if (unlikely(unitdata->udata.len > __xnet_u_max_tsdu(user)))
			goto tbaddata;
		if (unlikely(unitdata->udata.len > __xnet_u_max_tidu(user)))
			goto tbaddata;
		if (unlikely(unitdata->udata.len == 0 && !(user->info.flags & T_SNDZERO)))
			goto tbaddata;
	} else {
		if (unlikely(!(user->info.flags & T_SNDZERO)))
			goto tbaddata;
	}
	{
		size_t add_len = (likely(unitdata)
				  && likely(unitdata->addr.len > 0)) ? unitdata->addr.len : 0;
		size_t opt_len = (likely(unitdata)
				  && unlikely(unitdata->opt.len > 0)) ? unitdata->opt.len : 0;
		struct {
			struct T_unitdata_req prim;
			unsigned char addr[add_len];
			unsigned char opt[opt_len];
		} req;
		struct strbuf ctrl, data;

		ctrl.maxlen = sizeof(req);
		ctrl.len = sizeof(req);
		ctrl.buf = (char *) &req;
		if (likely(unitdata)) {
			data.maxlen = unitdata->udata.maxlen;
			data.len = unitdata->udata.len;
			data.buf = unitdata->udata.buf;
		} else {
			data.maxlen = 0;
			data.len = 0;
			data.buf = NULL;
		}
		req.prim.PRIM_type = T_UNITDATA_REQ;
		req.prim.DEST_length = add_len;
		req.prim.DEST_offset = likely(add_len) ? sizeof(req.prim) : 0;
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = unlikely(opt_len) ? sizeof(req.prim) + add_len : 0;
		if (likely(add_len))
			memcpy(req.addr, unitdata->addr.buf, add_len);
		if (unlikely(opt_len))
			memcpy(req.addr + add_len, unitdata->opt.buf, opt_len);
		if (unlikely(__xnet_t_putmsg(fd, &ctrl, &data, 0)))
			goto error;
		return (0);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tbadopt:
	t_errno = TBADOPT;
	goto error;
      tbadaddr:
	t_errno = TBADADDR;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (t_errno != TLOOK && (__xnet_t_peek(fd) & ~T_DATA) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_sndudata(). */
__hot int
__xnet_t_sndudata_r(int fd, const struct t_unitdata *unitdata)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndudata(fd, unitdata)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndudata_r,t_sndudata@@XNET_1.0");

/** @brief Send data or expedited data from gather buffers.
  * @param fd a file descriptor for the transport endpoint.
  * @param iov a pointer to a struct t_iovec structure containing the io
  * vectors for the send.
  * @param iovcount the count of the number of io vectors in the struct t_iovec
  * structure array.
  * @param flags flags to control the send.
  *
  * This function is a thread cancellation point.
  *
  * putpmsg() is a thread cancellation point, however, t_sndv is also a thread
  * cancellation point.  Therefore, all we need to do is defer cancellation.
  */
int
__xnet_t_sndv(int fd, const struct t_iovec *iov, unsigned int iovcount, int flags)
{
	struct _t_user *user;
	int written = 0, nbytes, i;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
	if (__xnet_t_peek(fd) > 0)
		goto tlook;
#ifdef DEBUG
	if (!iov)
		goto einval;
#endif
	if (iovcount == 0 || iovcount > _T_IOV_MAX)
		goto tbaddata;
	for (nbytes = 0, i = 0; i < iovcount; nbytes += iov[i].iov_len, i++) ;
	if (nbytes == 0 && !(user->info.flags & T_SNDZERO))
		goto tbaddata;
	if (nbytes > ((flags & T_EXPEDITED) ? __xnet_u_max_etsdu(user) : __xnet_u_max_tsdu(user)))
		goto tbaddata;
	{
		int band = (flags & T_EXPEDITED) ? 1 : 0;
		int n = 0, partial = 0;
		struct T_exdata_req prim;	/* same as T_data_req */
		struct strbuf ctrl, data;

		ctrl.maxlen = 0;
		ctrl.len = sizeof(prim);
		ctrl.buf = (char *) &prim;
		data.maxlen = 0;
		data.len = nbytes;
		data.buf = NULL;
		prim.PRIM_type = (flags & T_EXPEDITED) ? T_EXDATA_REQ : T_DATA_REQ;
		do {
			do {
				data.len = min(__xnet_u_max_tidu(user), iov[n].iov_len - partial);
				data.buf = iov[n].iov_base + partial;
				prim.MORE_flag = (written + data.len < nbytes) || (flags & T_MORE);
				if (__xnet_t_putpmsg(fd, &ctrl, &data, band, MSG_BAND) == -1)
					goto error;
				partial += data.len;
				written += data.len;
			} while (partial < iov[n].iov_len);
			n++;
			partial = 0;
		} while (written < nbytes && n < iovcount);
		return (written);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (!written) {
		if (t_errno != TLOOK && __xnet_t_peek(fd))
			goto tlook;
		return (-1);
	}
	return (written);
}

/** The reentrant version of __xnet_t_sndv(). */
int
__xnet_t_sndv_r(int fd, const struct t_iovec *iov, unsigned int iovcount, int flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndv(fd, iov, iovcount, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndv_r,t_sndv@@XNET_1.0");

#if 0
/** @brief Send data or expedited data from gather buffers.
  * @param fd a file descriptor for the transport endpoint.
  * @param options a pointer to a struct t_optmgmt structure containing the
  * options for the send.
  * @param iov a pointer to a struct t_iovec structure array containing the io
  * verctors for the send.
  * @param iovcount the count of the number of elements in the struct t_iovec
  * structure array.
  * @param flags flags to control the send.
  *
  * This function is a thread cancellation point.
  */
int
__xnet_t_sndvopt(int fd, struct t_optmgmt *options, const struct t_iovec *iov,
		 unsigned int iovcount, int flags)
{
	struct _t_user *user;
	int written = 0, nbytes, i;

	if (!(user = __xnet_t_tstuser(fd, 0, (1 << T_COTS) | (1 << T_COTS_ORD),
				      TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto error;
#ifdef DEBUG
	if (!iov)
		goto einval;
	if (options && (options->opt.len < 0 || (options->opt.len > 0 && !options->opt.buf)))
		goto einval;
#endif
	if (options && options->opt.len > __xnet_u_max_options(user))
		goto tbadopt;
	if (iovcount == 0 || iovcount > _T_IOV_MAX)
		goto tbaddata;
	for (nbytes = 0, i = 0; i < iovcount; nbytes += iov[i].iov_len, i++) ;
	if (nbytes == 0 && !(user->info.flags & T_SNDZERO))
		goto tbaddata;
	if (nbytes > ((flags & T_EXPEDITED) ? __xnet_u_max_etsdu(user) : __xnet_u_max_tsdu(user)))
		goto tbaddata;
	{
		size_t opt_len = (options && options->opt.len > 0) ? options->opt.len : 0;
		int band = (flags & T_EXPEDITED) ? 1 : 0;
		int n = 0, partial = 0;
		struct {
			struct T_optdata_req prim;
			unsigned char opt[opt_len];
		} req;
		struct strbuf ctrl, data;

		ctrl.maxlen = 0;
		ctrl.len = sizeof(req);
		ctrl.buf = (char *) &req;
		data.maxlen = 0;
		data.len = nbytes;
		data.buf = NULL;
		req.prim.PRIM_type = T_OPTDATA_REQ;
		req.prim.DATA_flag =
		    ((flags & T_EXPEDITED) ? T_ODF_EX : 0) | ((flags & T_MORE) ? T_ODF_MORE : 0);
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = opt_len ? sizeof(req.prim) : 0;
		if (opt_len)
			memcpy(req.opt, options->opt.buf, opt_len);
		do {
			do {
				data.len = min(__xnet_u_max_tidu(user), iov[n].iov_len - partial);
				data.buf = iov[n].iov_base + partial;
				req.prim.DATA_flag = (((flags & T_EXPEDITED)) ? T_ODF_EX : 0)
				    | (((flags & T_MORE) || (written + data.len < nbytes)) ?
				       T_ODF_MORE : 0);
				if (__xnet_t_putpmsg(fd, &ctrl, &data, band, MSG_BAND) == -1)
					goto error;
				partial += data.len;
				written += data.len;
			} while (partial < iov[n].iov_len);
			n++;
			partial = 0;
		} while (written < nbytes && n < iovcount);
		return (written);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
#endif
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tbadopt:
	t_errno = TBADOPT;
	goto error;
      error:
	if (!written)
		return (-1);
	return (written);
}

/** The reentrant version of __xnet_t_sndvopt(). */
int
__xnet_t_sndvopt_r(int fd, struct t_optmgmt *options, const struct t_iovec *iov,
		   unsigned int iovcount, int flags)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndvopt(fd, options, iov, iovcount, flags)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndvopt_r,t_sndvopt@@XNET_1.0");
#endif

/** @brief Send data unit from gather buffers.
  * @param fd a file descriptor for the transport endpoint.
  * @param unitdata a pointer to a t_unitdata structure describing the address and options for the send.
  * @param iov a pointer to a t_iovec io vector structure array.
  * @param iovcount the count of the vectors in the t_iovec structure array.
  *
  * This function is a thread cancellation point.  This function is
  * asynchronous thread cancellation safe.
  * 
  * This is a rather peculiar function.  We have little choice but to buffer
  * the entire gather array internally and then send the data in one big
  * @c T_UNITDATA_REQ.  TPI does not help us and the operating system does not
  * help us either.
  */
int
__xnet_t_sndvudata(int fd, struct t_unitdata *unitdata, struct t_iovec *iov, unsigned int iovcount)
{
	struct _t_user *user;
	int nbytes, i;

	if (!(user = __xnet_t_tstuser(fd, T_DATA, (1 << T_CLTS), TSF_IDLE)))
		goto error;
	if ((__xnet_t_peek(fd) & ~T_DATA) > 0)
		goto tlook;
#ifdef DEBUG
	if (!unitdata || !iov)
		goto einval;
#endif
	if (iovcount == 0 || iovcount > _T_IOV_MAX)
		goto tbaddata;
	for (nbytes = 0, i = 0; i < iovcount; nbytes += iov[i].iov_len, i++) ;
	if (unitdata && unitdata->addr.len > __xnet_u_max_addr(user))
		goto tbadaddr;
	if (unitdata && unitdata->opt.len > __xnet_u_max_options(user))
		goto tbadopt;
	if (nbytes > __xnet_u_max_tsdu(user))
		goto tbaddata;
	if (nbytes > __xnet_u_max_tidu(user))
		goto tbaddata;
	if (nbytes == 0 && !(user->info.flags & T_SNDZERO))
		goto tbaddata;
	{
		size_t add_len = (unitdata && unitdata->addr.len > 0) ? unitdata->addr.len : 0;
		size_t opt_len = (unitdata && unitdata->opt.len > 0) ? unitdata->opt.len : 0;
		int offset, n;
		struct {
			struct T_unitdata_req prim;
			unsigned char addr[add_len];
			unsigned char opt[opt_len];
		} req;
		struct strbuf ctrl, data;

		ctrl.maxlen = 0;
		ctrl.len = sizeof(req);
		ctrl.buf = (char *) &req;
		data.maxlen = 0;
		data.len = nbytes;
		data.buf = NULL;
		if (!(data.buf = (char *) malloc(nbytes)))
			goto enomem;
		for (n = 0, offset = 0; n < iovcount; offset += iov[n].iov_len, n++)
			memcpy(data.buf + offset, iov[n].iov_base, iov[n].iov_len);
		req.prim.PRIM_type = T_UNITDATA_REQ;
		req.prim.DEST_length = add_len;
		req.prim.DEST_offset = add_len ? sizeof(req.prim) : 0;
		req.prim.OPT_length = opt_len;
		req.prim.OPT_offset = opt_len ? sizeof(req.prim) + add_len : 0;
		if (add_len)
			memcpy(req.addr, unitdata->addr.buf, add_len);
		if (opt_len)
			memcpy(req.addr + add_len, unitdata->opt.buf, opt_len);
		if (__xnet_t_putmsg(fd, &ctrl, &data, 0))
			goto error;
		return (0);
	}
#ifdef DEBUG
      einval:
	errno = EINVAL;
	goto tsyserr;
#endif
      enomem:
	errno = ENOMEM;
	goto tsyserr;
      tsyserr:
	t_errno = TSYSERR;
	goto error;
      tbaddata:
	t_errno = TBADDATA;
	goto error;
      tbadopt:
	t_errno = TBADOPT;
	goto error;
      tbadaddr:
	t_errno = TBADADDR;
	goto error;
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (t_errno != TLOOK && (__xnet_t_peek(fd) & ~T_DATA) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_sndvudata(). */
int
__xnet_t_sndvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
		     unsigned int iovcount)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		if ((ret = __xnet_t_sndvudata(fd, unitdata, iov, iovcount)) == -1)
			pthread_testcancel();
		__xnet_t_putuser(&fd);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sndvudata_r,t_sndvudata@@XNET_1.0");

/* *INDENT-OFF* */
const char *__xnet_t_errlist[] = {
/*
TRANS No error is indicated in the t_errno variable.  The last operation was a
TRANS success.  t_errno will not be set to this value (zero) by the library,
TRANS the user must set t_errno to zero before the XTI library call an when
TRANS the call is successful, the t_errno value will be unaffected.  There is
TRANS no requirement that this value be set after a successful call, and calls
TRANS are even permitted to change t_errno to some other value, when the call
TRANS is actually successful. 
 */
	gettext_noop("No error"),
/*
TRANS The format of the supplied transport endpoint address is of an incorrect
TRANS format.  This is not used to provide protocol errors but merely indicate
TRANS an error in the syntactic formatting of the address.
 */
	gettext_noop("Bad address format"),
/*
TRANS The format of the supplied transport options is of an incorrect format.
TRANS This is not used to provide protocol errors but merely indicate an error
TRANS in the the syntactic formatting of the options.
 */
	gettext_noop("Bad options format"),
/*
TRANS The user does not have sufficient permissions or priveledge to execute
TRANS the XTI library call, or to use the specified address, or to use the
TRANS specified options.
 */
	gettext_noop("Bad permissions"),
/*
TRANS The file descriptor is not known to the XTI library.
 */
	gettext_noop("Bad file descriptor"),
/*
TRANS The user did not provide an address on the call and the XTI library or
TRANS underlying transport service provider is unable to choose and assign an
TRANS address on the user's behalf.
 */
	gettext_noop("Unable to allocate an address"),
/*
TRANS The arguments of the XTI library call are correct, however, the
TRANS transport endpoint or transport provider endpoint state machine is in a
TRANS state in which the XTI library call is not permitted.  The C-Locale text
TRANS is traditional and does not really reflect the situation very well.
 */
	gettext_noop("Would place interface out of state"),
/*
TRANS When a transport connection indication is received it is given a
TRANS connection indication sequence number which is used to later accept or
TRANS reject the connection.  In this case, the user supplied sequence number
TRANS is invalid (does not correspond to an outstanding connection
TRANS indication).
 */
	gettext_noop("Bad call sequence number"),
/*
TRANS A UNIX system error has occured and the errno variable indicates the
TRANS UNIX system error.
 */
	gettext_noop("System error"),
/*
TRANS The XTI library call is otherwise correct, however, an asynchrnous event
TRANS has occured requiring the user to call the t_look() XTI library call to
TRANS process the asychronous event before proceeding.
 */
	gettext_noop("Has to t_look() for event"),
/*
TRANS The amount of data specified is incorrect.  Where an IO vector is
TRANS supplied, the IO vector count is out of range (zero or greater than
TRANS T_MAX_IOV), otherwise, when sending data, if the amount of data is zero,
TRANS the transport service provider does not permit zero-length sends, if the
TRANS amount of data is non-zero, then the amount of data exceeds protocol
TRANS limits provided by the transport service provider in response to a a
TRANS t_getinfo() XTI library call.
 */
	gettext_noop("Bad amount of data"),
/*
TRANS The user provided a buffer to the XTI library call into which to receive
TRANS addresses or options, however, the size of the buffer was too small to
TRANS accept the addresses or options that were received.  In the C-locale
TRANS this is termed a buffer overflow.
 */
	gettext_noop("Buffer was too small"),
/*
TRANS The user called a send function that would have otherwise blocked due to
TRANS flow control, however, O_NONBLOCK was set on the file descriptor.  This
TRANS error indicates that the call was abandonned due to flow control
TRANS conditions.  If O_NONBLOCK had not been set, the call would have blocked
TRANS due to flow control.
 */
	gettext_noop("Would block due to flow control"),
/*
TRANS The user called an XTI library call for a transport endpoint for which
TRANS O_NONBLOCK is set, and the library call was expecting to receive a data
TRANS indictation and no such indication was present.  If O_NONBLOCK had not
TRANS been set, the call would have blocked awaiting the indication.
 */
	gettext_noop("No data indication"),
/*
TRANS The user called an XTI library call for a transport endpoint for which
TRANS O_NONBLOCK is set, and the library call was expecting to receive a
TRANS disconnect indication and no such indication was present.  If O_NONBLOCK
TRANS had not been set, the call would have blocked awaiting the indication.
 */
	gettext_noop("No disconnect indication"),
/*
TRANS The user called an XTI library call for a transport endpoint for which
TRANS O_NONBLOCK is set, and the library call was expecting to receive a
TRANS unit data indication and no such indication was present.  If O_NONBLOCK
TRANS had not been set, the call would have blocked awaiting the indication.
 */
	gettext_noop("No unitdata error indication"),
/*
TRANS The flags argument supplied ot the XTI library call were invalid or
TRANS contained illegal information considerting the transport service or
TRANS transport service provider type.
 */
	gettext_noop("Bad flags"),
/*
TRANS The user called an XTI library call for a transport endpoint for which
TRANS O_NONBLOCK is set, and the library call was expecting to receive a
TRANS orederly release indication and no such indication was present.  If
TRANS O_NONBLOCK had not been set, the call would have blocked awaiting the
TRANS indication.
 */
	gettext_noop("No orderly release indication"),
/*
TRANS Some XTI library calls can only be used on connection-oriented transport
TRANS service providers, others only on connection-oriented transport service
TRANS providers with orderly release, others only on connectionless transport
TRANS service providers.  In addition, some messages exchanged with the
TRANS underlying transport service provider might not be supported by that
TRANS provider.  This message indications that the user has called an XTI
TRANS function that is either not supported for the given transport endpoint,
TRANS or that the underlying transport provider does not support the TPI
TRANS primitives supplied by the XTI library.
 */
	gettext_noop("Not supported"),
/*
TRANS Most XTI library calls require the transport endpoint state machine to
TRANS be in a given state.  Due to parallel events, the transport endpoint
TRANS state machine can be changing state when an XTI library function is
TRANS called.  This error indicates that the transport endpoint state machine
TRANS was changing state at the time that the call as generated and that the
TRANS call failed for that reason.
 */
	gettext_noop("State is currently changing"),
/*
TRANS The t_alloc() and t_free() functions take a structure type as an
TRANS argument.  This error indicates that the structure type provided to
TRANS these calls is either invalid or not supported by the XTI library.
 */
	gettext_noop("Structure type not supported"),
/*
TRANS An attempt was made to open a device name (e.g. /dev/tcp) that did not
TRANS correspond to a transport service provider or did not exist in the
TRANS filesystem.
 */
	gettext_noop("Bad transport provider name"),
/*
TRANS An attempt was made to listen on a stream that was bound with an number
TRANS of oustanding connection indications negotiated as zero (that is, a
TRANS non-listening stream).
 */
	gettext_noop("Listener queue length limit is zero"),
/*
TRANS A supplied address used to bind a transport endpoint to a specific local
TRANS address has failed because that address is in by another transport
TRANS endpoint and the transport service provider or protoocol does not permit
TRANS multiple transport endpoints to be bound to the same address.  The
TRANS address is considered "busy".
 */
	gettext_noop("Address already in use"),
/*
TRANS An attempt was made to initiate an outgoing transport connection on a
TRANS transport endpoint that is listening, and the listening transport
TRANS endpoint has outstanding transport connection indications that have not
TRANS been satisfied by accepting or refusing the connection indication.
 */
	gettext_noop("Outstanding connect indications"),
/*
TRANS A connection indication can only be accepted on a stream that
TRANS corresponds to the same transport provider that generated the connection
TRANS indication.  An attempt was made to accept an outstanding connection
TRANS indication on an accepting stream that corresponds to a transport
TRANS service provider other than the transport service provider from which
TRANS the connection indication was received.
 */
	gettext_noop("Not same transport provider"),
/*
TRANS A connection indication can only be accepted on a stream that is in the
TRANS unbound or idle states.  An attempt was made to accept a connection
TRANS indication on a listening stream for which outstanding connection
TRANS indications exist (qlen > 0).
 */
	gettext_noop("Connection acceptor has qlen > 0"),
/*
TRANS A connection indication can only be accepted on a stream that is in the
TRANS unbound or idle states.  When in the idle state, the transport service
TRANS provider may require that the accepting stream already be bound to the
TRANS same transport address as that for which the connection indication was
TRANS provided.
 */
	gettext_noop("Connection acceptor bound to different address"),
/*
TRANS A listening stream can only accept the sepcific number of connection
TRANS indications that was negotiated between the user and the transport
TRANS service provider at the time that the stream was bound as a listening
TRANS stream.  An attempt to t_listen on a listening transport endpoint which
TRANS already has the maximum number of outstanding connection indications
TRANS will receive this error.
 */
	gettext_noop("Connection indicator queue is full"),
/*
TRANS A problem in synchronization between the XTI library and the underlying
TRANS transport service provider was detected, or, an error for which no other
TRANS explanation or error code is available was detected.  This is a
TRANS catch-all error code and thus has the vague C-locale "protocol error"
TRANS description.
 */
	gettext_noop("Protocol error"),
/*
TRANS The transport service provider might return special error codes that are
TRANS not known to the XTI library.  This message is used to describe all
TRANS error codes not known to the XTI library.
 */
	gettext_noop("Unknown")
};
/* *INDENT-ON* */

__asm__(".symver __xnet_t_errstr,t_errstr@@XNET_1.0");

__asm__(".symver __xnet_t_errlist,t_errlist@@XNET_1.0");

int __xnet_t_nerr = 31;

__asm__(".symver __xnet_t_nerr,t_nerr@@XNET_1.0");

/** @brief Produce an error message string.
  * @param errnum the error number for which to return a string.
  *
  * This function is NOT a thread cancellation point.
  */
const char *
__xnet_t_strerror(int errnum)
{
	if (0 <= errnum && errnum <= TPROTO)
		return gettext(__xnet_t_errlist[errnum]);
	return gettext(__xnet_t_errlist[TPROTO + 1]);
}

__asm__(".symver __xnet_t_strerror,t_strerror@@XNET_1.0");

/** @brief Synchronize XTI library with TLI provider.
  * @param fd the file descriptor to synchronize
  *
  * t_sync() should be called whenever a new file descriptor is generated from
  * an old one (e.g. using dup() or dup2()) or when TPROTO was returned as an
  * error in t_errno for a previous XTI library call.
  *
  * @return Unpon success, t_sync() returns a positive integer reflecting the
  * state of the transport interface.  Upon failure, t_sync() returns -1 and
  * sets t_errno to one of the following values:
  *
  * @arg TBADF	    fd is invalid.
  * @arg TPROTO	    a protocol error occured between the XTI library and the
  *		    underlying transport provider.
  * @arg TSTATECHNG  the interface is in a transient state.
  * @arg TSYSERR	    a Linux system error occurred and the Linux error number
  *		    is set in errno.
  */
int
__xnet_t_sync(int fd)
{
	struct _t_user *user;

	if (0 > fd || fd >= OPEN_MAX)
		goto tbadf;
	if ((user = _t_fds[fd])) {
		/**
		   When the user structure already exists, t_sync() is likely called in response to 
		   TPROTO.  
		 */
		if (!(user->flags & TUF_SYNC_REQUIRED))
			return __xnet_t_getstate(fd);
		_t_fds[fd] = NULL;
		pthread_rwlock_destroy(&user->lock);
		free(user);
		user = NULL;	/* allocate a new one again below... */
	}
	{
		/**
		   When the user structure does not exist, t_sync() is likely called after a dup()
		   or dup2().  We need to perform the same functions that we performed after t_open().
		 */
		int i;
		union {
			struct T_capability_req req;
			struct T_capability_ack ack;
		} buf;

		t_errno = TSYSERR;
		if (!(user = (struct _t_user *) malloc(sizeof(*user))))
			goto enomem;
		memset(user, 0, sizeof(*user));
		user->ctlmax =
		    sizeof(union T_primitives) + _T_DEFAULT_ADDRLEN + _T_DEFAULT_ADDRLEN +
		    _T_DEFAULT_OPTLEN;
		user->datmax = _T_DEFAULT_DATALEN;
		if (!(user->ctlbuf = (char *) malloc(user->ctlmax)))
			goto enobufs;
		if (!(user->datbuf = (char *) malloc(user->datmax)))
			goto enobufs;
		buf.req.PRIM_type = T_CAPABILITY_REQ;
		buf.req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		if (__xnet_t_strioctl(fd, TI_CAPABILITY, &buf, sizeof(buf)) != 0)
			goto error;
		/**
		   We could put a fallback in t_open() if the transport provider does not support
		   capabilities (it would be really ancient); hoever, the best place to put this
		   support would be in timod.  timod has access to the queue pair pointer to
		   generate an acceptor id and also has the ability to complete the info structure
		   from a @c T_INFO_REQ. 
		 */
		if ((buf.ack.CAP_bits1 & (TC1_INFO | TC1_ACCEPTOR_ID)) !=
		    (TC1_INFO | TC1_ACCEPTOR_ID))
			goto tproto;
		user->flags |= TUF_SYNC_REQUIRED;
		for (i = 0; i < OPEN_MAX; i++)
			if (_t_fds[i] && _t_fds[i]->token == buf.ack.ACCEPTOR_id) {
				free(user);
				user = _t_fds[i];
				pthread_rwlock_destroy(&user->lock);
				break;
			}
#if 0
		/* 
		   FIXME: get these with fcntl */
		user->fflags = oflag;
#endif
		user->info.addr = buf.ack.INFO_ack.ADDR_size;
		user->info.options = buf.ack.INFO_ack.OPT_size;
		user->info.tsdu = buf.ack.INFO_ack.TSDU_size;
		user->info.etsdu = buf.ack.INFO_ack.ETSDU_size;
		user->info.connect = buf.ack.INFO_ack.CDATA_size;
		user->info.discon = buf.ack.INFO_ack.DDATA_size;
		user->info.servtype = buf.ack.INFO_ack.SERV_type;
		user->info.flags = buf.ack.INFO_ack.PROVIDER_flag;
		user->info.tidu = buf.ack.INFO_ack.TIDU_size;
		user->token = buf.ack.ACCEPTOR_id;
		user->refs++;	/* one for new structure, see memset above */
		if (user->flags & TUF_SYNC_REQUIRED)
			__xnet_u_setstate(user, buf.ack.INFO_ack.CURRENT_state);
		pthread_rwlock_init(&user->lock, NULL);	/* destroyed for existing structure */
		/* 
		   FIXME: need to install pthread_atfork handlert o re-initialize read-write locks */
		_t_fds[fd] = user;
		user->flags &= ~TUF_SYNC_REQUIRED;
		return __xnet_t_getstate(fd);
	}
      enobufs:
	if (user->ctlbuf)
		free(user->ctlbuf);
	if (user->datbuf)
		free(user->datbuf);
	if (user)
		free(user);
	t_errno = TSYSERR;
	errno = ENOBUFS;
	goto error;
      enomem:
	t_errno = TSYSERR;
	errno = ENOMEM;
	goto error;
      tproto:
	t_errno = TPROTO;
	goto error;
      tbadf:
	t_errno = TBADF;
	goto error;
      error:
	return (-1);
}

/**
  * @brief The reentrant version of __xnet_t_sndvudata().
  * @param fd the file descriptor to synchronize
  *
  * The reentrant version of t_sync() is like the reentrant version of t_open()
  * and t_close() in that it write locks the file descriptor list in
  * preparation for adding or removing a user structure to or from the list.
  * Because of this, all other threads holding read locks on the file
  * descriptor list must release them before this function will proceed.  While
  * this function is proceeding, all other threads attempting to execute XTI
  * library functions on file descriptors will be blocked until this function
  * exits.
  */
int
__xnet_t_sync_r(int fd)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__xnet_list_unlock, NULL);
	if ((err = __xnet_list_wrlock()) == 0) {
		ret = __xnet_t_sync(fd);
		__xnet_list_unlock(NULL);
	} else {
		t_errno = TSYSERR;
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_sync_r,t_sync@@XNET_1.0");

/** @brief Get configurable XTI variables.
  * @param name
  *
  * This function is NOT a thread cancellation point.
  */
int
__xnet_t_sysconf(int name)
{
	switch (name) {
	case _SC_T_IOV_MAX:
		return _T_IOV_MAX;
	case _SC_T_DEFAULT_ADDRLEN:
		return _T_DEFAULT_ADDRLEN;
	case _SC_T_DEFAULT_CONNLEN:
		return _T_DEFAULT_CONNLEN;
	case _SC_T_DEFAULT_DISCLEN:
		return _T_DEFAULT_DISCLEN;
	case _SC_T_DEFAULT_OPTLEN:
		return _T_DEFAULT_OPTLEN;
	case _SC_T_DEFAULT_DATALEN:
		return _T_DEFAULT_DATALEN;
	}
	t_errno = TBADFLAG;
	return (-1);
}

__asm__(".symver _xnet_t_sysconf,t_sysconf@@XNET_1.0");

/** @brief Remove an address from a transport endpoint.
  * @param fd a file descriptor indicating the transport endpoint to unbind.
  *
  * This function is NOT a thread cancellation point.
  *
  * t_unbind() can return a TLOOK error for T_CLTS when a unitdata or unitdata
  * error has arrived at the transport endpoint since the last library call.
  * Therefore, we call t_look() for transport service providers of service type
  * T_CLTS.
  *
  * t_unbind() can also return a TLOOK error for T_COTS or T_COTS_ORD on a
  * listening stream (qlen > 0) when a connection indication and possibly
  * disconnect indications for outstanding unprocessed connection indications
  * have arrived prior to the call.  The service provide cannot detect this, so
  * we call t_look() for listening streams as well.
  *
  * For other stream types, an asynchronous event is not possible and we only
  * check for outstanding events.
  */
int
__xnet_t_unbind(int fd)
{
	struct _t_user *user;

	if (!(user = __xnet_t_tstuser(fd, 0, -1, TSF_IDLE)))
		goto error;
	if (user->info.servtype == T_CLTS || user->qlen > 0)
		if (__xnet_t_peek(fd) > 0)
			goto tlook;
	{
		union {
			struct T_unbind_req req;
			struct T_ok_ack ack;
			struct T_error_ack err;
		} buf;

		buf.req.PRIM_type = T_UNBIND_REQ;
		if (__xnet_t_strioctl(fd, TI_UNBIND, &buf, sizeof(buf)) != 0)
			goto error;
		__xnet_u_setstate_const(user, TS_UNBND);
		return (0);
	}
      tlook:
	t_errno = TLOOK;
	goto error;
      error:
	if (t_errno != TLOOK && __xnet_t_peek(fd) > 0)
		goto tlook;
	return (-1);
}

/** The reentrant version of __xnet_t_unbind(). */
int
__xnet_t_unbind_r(int fd)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__xnet_t_putuser, &fd);
	if (__xnet_t_getuser(fd)) {
		ret = __xnet_t_unbind(fd);
		__xnet_t_putuser(&fd);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __xnet_t_unbind_r,t_unbind@@XNET_1.0");

/**
  * @section Identification
  * This development manual was written for the OpenSS7 XNS/XTI Library version \$Name:  $(\$Revision: 0.9.2.25 $).
  * @author Brian F. G. Bidulock
  * @version \$Name:  $(\$Revision: 0.9.2.25 $)
  * @date \$Date: 2006/10/02 11:32:26 $
  */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
