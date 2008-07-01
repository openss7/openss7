/*****************************************************************************

 @(#) $RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 11:51:00 $

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

 Last Modified $Date: 2008-07-01 11:51:00 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi.c,v $
 Revision 0.9.2.1  2008-07-01 11:51:00  brian
 - added manual pages and library implementation

 *****************************************************************************/

#ident "@(#) $RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 11:51:00 $"

static char const ident[] = "$RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 11:51:00 $";

/* This file can be processed by doxygen(1). */

/** @weakgroup dlpi OpenSS7 DLPI Library
  * @{ */

/** @file
  * OpenSS7 DLPI Library implementation file. */

#define _XOPEN_SOURCE	600
#define _REENTRANT
#define _THREAD_SAFE

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

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS	1
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

#ifdef _SUN_SOURCE
#include <sys/dlpi.h>
#include <sys/sundlpi.h>
#else
#define _SUN_SOURCE
#include <sys/dlpi.h>
#include <sys/sundlpi.h>
#undef _SUN_SOURCE
#endif

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

#include <net/if_arp.h>

/** @brief thread-specific data
  */
struct __dlpi_tsd {
	int dlerrno;
	char strbuf[BUFSIZ]; /* string buffer */
};

/** @brief once condition for Thread-Specific Data key creation.
  */
static pthread_once_t __dlpi_tsd_once = PTHREAD_ONCE_INIT;

/** @brief DLPI Library Thread-Specific Data Key
  */
static pthread_key_t __dlpi_tsd_key = 0;

static void
__dlpi_tsd_free(void *buf)
{
	pthread_setspecific(__dlpi_tsd_key, NULL);
	free(buf);
}

static void
__dlpi_tsd_alloc(void)
{
	int ret;
	void *buf;

	ret = pthread_key_create(&__dlpi_tsd_key, __dlpi_tsd_free);
	buf = malloc(sizeof(struct __dlpi_tsd));
	bzero(buf, sizeof(*buf));
	ret = pthread_setspecific(__dlpi_tsd_key, buf);
	return;
}

static struct __dlpi_tsd *
__dlpi_get_tsd(void)
{
	pthread_once(&__dlpi_tsd_once, __dlpi_tsd_alloc);
	return (struct __dlpi_tsd *) pthread_getspecific(__dlpi_tsd_key);
}

/** @brief #dlerrno location function.
  * @version DLPI_1.0
  * @par Alias:
  * This function is an implementation of _dlerrno().
  */
__hot int *
__dlpi_dlerrno(void)
{
	return (&__dlpi_get_tsd()->dlerrno);
}

/** @fn int *_dlerrno(void)
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi__dlerrno().
  *
  * This function provides the location of the integer that contains the DLPI
  * error number returned by the last DLPI function that failed.  This is
  * normally used to provide #dlerrno in a thread-safe way as follows:
  *
  * @code
  * #define dlerrno (*(_dlerrno()))
  * @endcode
  */
__asm__(".symver __dlpi__dlerrno,_dlerrno@@DLPI_1.0");

#ifndef dlerrno
#define dlerrno (*(_dlerrno()))
#endif

struct __dlpi_user {
	pthread_rwlock_t lock;		/**< lock for this structure */
	struct strbuf ctrl;		/**< ctrl part buffer */
	struct strbuf data;		/**< data part buffer */
	int ppa;			/**< Physical Point of Attachment */
	int fd;				/**< file descriptor */
	int mac_type;			/**< MAC Type for this Stream */
	char linkname[32];		/**< link name opened */
};

static struct __dlpi_user *__dlpi_dhs[OPEN_MAX] = { NULL, };

static struct pthread_rwlock_t __dlpi_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static inline int
__dlpi_lock_rdlock(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}
static inline int
__dlpi_lock_wrlock(pthread_rwlock_t *rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}
static inline void
__dlpi_lock_unlock(pthread_rwlock_t *rwlock)
{
	pthread_rwlock_unlock(rwlock);
}
static inline int
__dlpi_list_rdlock(void)
{
	return __dlpi_lock_rdlock(&__dlpi_fd_lock);
}
static inline int
__dlpi_list_wrlock(void)
{
	return __dlpi_lock_wrlock(&__dlpi_fd_lock);
}
static inline void
__dlpi_list_unlock(void *ignore)
{
	return __dlpi_lock_unlock(&__dlpi_fd_lock);
}
static inline int
__dlpi_user_rdlock(struct __dlpi_user *user)
{
	return __dlpi_lock_rdlock(&user->lock);
}
static inline int
__dlpi_user_wrlock(struct __dlpi_user *user)
{
	return __dlpi_lock_wrlock(&user->lock);
}
static inline void
__dlpi_user_unlock(struct __dlpi_user *user)
{
	return __dlpi_lock_unlock(&user->lock);
}

/** @internal
  * @brief release a user whether held or got
  * @param arg argument
  */
static void
__dlpi_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct __dlpi_user *user = __dlpi_dhs[fd];
	__dlpi_user_unlock(user);
	__dlpi_list_unlock(NULL);
	return;
}

/** @internal
  * @brief Get a write locked data link user structure.
  * @param fd the file descriptor for which to get the associated endpoint.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe write operation.
  */
static __hot struct __dlpi_user *
__dlpi_getuser(int fd)
{
	struct __dlpi_user *user;
	int err;

	if (unlikely((err = __dlpi_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely(!(user = __dlpi_dhs[fd])))
		goto ebadf;
	if (unlikely((err = __dlpi_user_wrlock(user))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	dlerrno = DL_SYSERR;
	errno = err;
	__dlpi_list_unlock(NULL);
	goto error;
      ebadf:
	dlerrno = DL_SYSERR;
	errno = EBADF;
	goto error;
      list_lock_error:
	dlerrno = DL_SYSERR;
	errno = err;
	goto error;
      error:
	return (NULL);
}

/** @internal
  * @brief Get a read locked data link user structure.
  * @param fd the file descriptor for which to get the associated endpoint.
  *
  * This is a range-checked array lookup of the library user structure
  * associated with the specified file descriptor.  In addition, this function
  * takes the necessary locks for thread-safe read operation.
  */
static __hot struct __dlpi_user *
__dlpi_hlduser(int fd)
{
	struct __dlpi_user *user;
	int err;

	if (unlikely((err = __dlpi_list_rdlock())))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely(!(user = __dlpi_dhs[fd])))
		goto ebadf;
	if (unlikely((err = __dlpi_user_rdlock(user))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	dlerrno = DL_SYSERR;
	errno = err;
	__dlpi_list_unlock(NULL);
	goto error;
      ebadf:
	dlerrno = DL_SYSERR;
	errno = EBADF;
	goto error;
      list_lock_error:
	dlerrno = DL_SYSERR;
	errno = err;
	goto error;
      error:
	return (NULL);
}

static __hot struct __dlpi_user *
__dlpi_chkuser(int fd)
{
	struct __dlpi_user *user;

	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto ebadf;
	if (unlikely(!(user = __dlpi_dhs[fd])))
		goto ebadf;
	return (user);
      ebadf:
	dlerrno = DL_SYSERR;
	errno = EBADF;
	goto error;
      error:
	return (NULL);
}

/** @internal
  * @brief A version of getmsg with DLPI errors.
  * @param dh a file descriptor representing the data link.
  * @param ctrl a pointer to a strbuf structure returning the control part of
  * the message.
  * @param data a pointer to a strbuf structure returning the data part of the
  * message.
  * @param flagsp a pointer to an integer returning the flags associated with
  * the retrieved message.
  *
  * This is the same as getmsg(2s) with the exception that DLPI errors are
  * returned.
  *
  * @since 2008-06-30T10+0000
  *
  * @return When __dlpi_getmsg() success, it returns zero (0); when it fails,
  * it returns -1.
  *
  * @par Errors
  * __dlpi_getmsg() can return the following DLPI errors:
  *
  */
static int
__dlpi_getmsg(dlpi_handle_t dh, struct strbuf *ctrl, struct strbuf *data, int *flagsp)
{
	int ret;

	if ((ret = getmsg(dh, ctrl, data, flagsp)) >= 0)
		return (ret);
	dlerrno = DL_SYSERR;
	switch (errno) {
	case EISDIR:
	case EBADF:
		dlerrno = DLPI_ENOLINK;
		break;
	case EFAULT:
	case ENODEV:
	case ENOSTR:
	case EIO:
	case EINVAL:
	case EAGAIN:
	case EINTR:
	case ENOSR:
		break;
	case EBADMSG:
		dlerrno = DLPI_EBADMSG;
		break;
	}
	return (-1);
}

static int
__dlpi_putmsg(dlpi_handle_t dh, struct strbuf *ctrl, struct strbuf *data, int flags)
{
	return putmsg(dh, ctlr, data, flags);
}

static int
__dlpi_peek(dlpi_handle_t dh, int timeout)
{
	struct __dlpi_user *user;

	if (!(user = __dlpi_chkuser(dh)))
		goto error;
	if (user->event == 0) {
		struct pollfd pfd;

		pfd.fd = dh;
		pfd.events = (POLLIN|POLLPRI|POLLRDNORM|POLLRDBAND|POLLMSG|POLLERR|POLLHUP);
		pfd.revents = 0;

		switch (poll(&pfd, 1, timeout)) {
		case 1:
			if (pfd.revents & (POLLMSG|POLLERR|POLLHUP))
				break;
			if (pfd.revents & (POLLIN|POLLPRI|POLLRDNORM|POLLRDBAND))
				break;
		case 0:
			return (0); /* nothing to report */
		case -1:
			goto syserr;
		default:
			goto proto;
		}
	}
	if (user->event == 0) {
		int ret, flag = 0;
		union DL_primitives *p = (typeof(p)) user->ctlbuf;

		user->ctrl.maxlen = user->ctlmax;
		user->ctrl.len = 0;
		user->ctrl.buf = user->ctlbuf;
		user->data.maxlen = user->datmax;
		user->data.len = 0;
		user->data.buf = user->datbuf;
		if (user->state == DL_IDLE)
			user->data.maxlen = 0;
		if ((ret = __dlpi_getmsg(dh, &user->ctrl, &user->data, &flag)) < 0)
			goto error;
		if ((ret & MORECTL) || ((ret & MOREDATA) && user->data.maxlen > 0))
			goto cleanup;
		if (flag != 0 || flags == RS_HIPRI)
			goto sync;
		switch (user->state) {
		}
	}
}

/** A bit of a discussion on locking:  Solaris(R) DLPI Library documentation
  * says that the dlpi_handle_t is a handle that can be used by multiple
  * threads but that no two threads may use the same handle at the same time.
  *
  * While I agree that results might be unpredictable if various functions
  * were performed at the same time, but there is no reason why multiple
  * threads cannot read and write data at the same time.  Therefore, this
  * library relaxes the restriction concerning handle access by multiple
  * threads and the necessary locks are taken to make these function more
  * fully thread-safe.
  */

/** @brief convert MAC type to ARP type
  * @param mactype The MAC type to convert.
  *
  * Simply converts the provided DLPI MAC type to an ARP hardware type.  If
  * the conversion is not possible, zero is returned; otherwise the ARP
  * hardware type is returned.
  */
uint
__dlpi_arptype(uint mactype)
{
	switch (mactype) {
	case DL_CSMACD:	/* IEEE 802.3 CSMA/CD network */
		return (ARPHRD_IEEE802);	/* IEEE 802.2 Ethernet/TR/TB */
	case DL_TPB:		/* IEEE 802.4 Token Passing Bus */
		return (ARPHRD_IEEE802);	/* IEEE 802.2 Ethernet/TR/TB */
	case DL_TPR:		/* IEEE 802.5 Token Passing Ring */
		return (ARPHRD_IEEE802_TR);	/* Magic type ident for TR */
	case DL_METRO:		/* IEEE 802.6 Metro Net */
		return (ARPHRD_IEEE802);	/* IEEE 802.2 Ethernet/TR/TB */
	case DL_ETHER:		/* Ethernet Bus */
		return (ARPHRD_ETHER);	/* Ethernet 10Mbps */
	case DL_HDLC:		/* ISO HDLC protocol support */
#if 0
		return (ARPHRD_CISCO);	/* Cisco HDLC */
#endif
		return (ARPHRD_HDLC);	/* Cisco HDLC */
	case DL_CHAR:		/* Character Synchronous protocol support */
		return (ARPHRD_RAWHDLC);	/* Raw HDLC */
	case DL_CTCA:		/* IBM Channel-to-Channel Adapter */
		break;
	case DL_FDDI:		/* Fiber Distributed data interface */
		return (ARPHRD_FDDI);	/* Fiber Distributed data interface */
	case DL_FC:		/* Fibre Channel interface */
		return (ARPHRD_FCPP);	/* Point to point fibrechannel */
#if 0
		return (ARPHRD_FCAL);	/* Fibrechannel arbitrated loop */
		return (ARPHRD_FCPL);	/* Fibrechannel public loop */
		return (ARPHRD_FCFABRIC);	/* Fibrechannel fabric */
#endif
	case DL_ATM:		/* ATM */
		return (ARPHRD_ATM);	/* ATM */
	case DL_IPATM:		/* ATM Classical IP interface */
		return (ARPHRD_ATM);	/* ATM */
	case DL_X25:		/* X.25 LAPB interface */
		return (ARPHRD_LAPB);
	case DL_ISDN:		/* ISDN interface */
		break;
	case DL_HIPPI:		/* HIPPI interface */
		return (ARPHRD_HIPPI);	/* High Performance Parallel Interface */
	case DL_100VG:		/* 100 Based VG Ethernet */
		return (ARPHRD_ETHER);	/* Ethernet 10Mbps */
	case DL_100VGTPR:	/* 100 Based VG Token Ring */
		return (ARPHRD_IEEE802_TR);	/* Magic type ident for TR */
	case DL_ETH_CSMA:	/* ISO 8802/3 and Ethernet */
		return (ARPHRD_ETHER);	/* Ethernet 10Mbps */
	case DL_100BT:		/* 100 Base T */
		return (ARPHRD_ETHER);	/* Ethernet 10Mbps */
	case DL_IB:		/* Infiniband */
		return (ARPHRD_INFINIBAND);
	case DL_FRAME:		/* Frame Relay LAPF */
		return (ARPHRD_DLCI);	/* Frame Relay DLCI */
	case DL_MPFRAME:	/* Multi-protocol over Frame Relay */
		return (ARPHRD_FRAD);	/* Frame Relay access device */
	case DL_ASYNC:		/* Character Asynchronous Protocol */
		/* or maybe raw HDLC */
		return (ARPHRD_DDCMP);	/* Digital's DDCMP protocol */
	case DL_IPX25:		/* X.25 Classical IP interface */
		return (ARPHRD_X25);	/* CCITT X.25 */
		return (ARPHRD_AX25);	/* AX.25 Level 2 */
	case DL_LOOP:		/* software loopback */
		return (ARPHRD_LOOPBACK);	/* Loopback device */
	case DL_OTHER:		/* Any other medium not listed above */
		return (ARPHRD_VOID);	/* Void type, nothing is known */
	case DL_IPV4:		/* IPv4 Tunnel Link */
		return (ARPHRD_TUNNEL);	/* IPIP tunnel */
	case DL_IPV6:		/* IPv6 Tunnel Link */
		return (ARPHRD_TUNNEL6);	/* IPIP6 tunnel */
	case DL_VNI:		/* Virtual network interface */
		return (ARPHRD_SIT);	/* sit0 IPv6-in-IPv4 */
	case DL_WIFI:		/* IEEE 802.11 */
		return (ARPHRD_IEEE80211);	/* IEEE 802.11 */
	}
	return (0);
#if 0
	/* these are not represented */
	return (ARPHRD_NETROM);	/* NET/ROM pseudo */
	return (ARPHRD_EETHER);	/* Experimental Ethernet */
	return (ARPHRD_PRONET);	/* PROnet token ring */
	return (ARPHRD_CHAOS);	/* Chaosnet */
	return (ARPHRD_ARCNET);	/* ARCnet */
	return (ARPHRD_APPLETLK);	/* AppleTalk */
	return (ARPHRD_METRICOM);	/* Metricom STRIP */
	return (ARPHRD_IEEE1394);	/* IEEE 1394 (HPSB) RFC 2734 */
	return (ARPHRD_EUI64);	/* ??? */

	return (ARPHRD_SLIP);
	return (ARPHRD_CSLIP);
	return (ARPHRD_SLIP6);
	return (ARPHRD_CSLIP6);
	return (ARPHRD_RSRVD);	/* Notional KISS type */
	return (ARPHRD_ADAPT);
	return (ARPHRD_ROSE);
	return (ARPHRD_HWX25);	/* Boards with X.25 in firmware */
	return (ARPHRD_PPP);

	return (ARPHRD_SKIP);	/* SKIP vif */
	return (ARPHRD_LOCALTLK);	/* Localtalk device */
	return (ARPHRD_BIF);	/* AP1000 BIF */
	return (ARPHRD_IPDDP);	/* IP over DDP tunneller */
	return (ARPHRD_IPGRE);	/* GRE over IP */
	return (ARPHRD_PIMREG);	/* PIMSM register interface */
	return (ARPHRD_ASH);	/* Nexus 64Mbps Ash */
	return (ARPHRD_ECONET);	/* Acorn Econet */
	return (ARPHRD_IRDA);	/* Linux-IrDA */
#endif
}

/** @brief The reentrant version of __dlpi_arptype().
  * @param mactype The MAC type to convert.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_arptype().
  */
uint
__dlpi_arptype_r(uint mactype)
{
	return __dlpi_arptype(mactype);
}

/** @fn uint dlpi_arptype(uint mactype)
  * @param mactype The MAC type to convert.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_arptype_r().
  */
__asm__(".symver __dlpi_arptype_r,dlpi_arptype@@DLPI_1.0");

/** @brief bind a link to a SAP
  * @param dh the link to bind.
  * @param sap the SAP to bind to the link.
  * @param boundsap returned bound SAP when successful.
  */
int
__dlpi_bind(dlpi_handle_t dh, uint sap, uint *boundsap)
{
}

/** @brief The reentrant version of __dlpi_bind().
  * @param dh the link to bind.
  * @param sap the SAP to bind to the link.
  * @param boundsap returned bound SAP when successful.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_bind().
  */
int
__dlpi_bind_r(dlpi_handle_t dh, uint sap, uint *boundsap)
{
}

/** @fn int dlpi_bind(dlpi_handle_t dh, uint sap, uint *boundsap)
  * @param dh the link to bind.
  * @param sap the SAP to bind to the link.
  * @param boundsap returned bound SAP when successful.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_bind_r().
  */
__asm__(".symver __dlpi_bind_r,dlpi_bind@@DLPI_1.0");

/** @brief close a link.
  * @param dh the link to close.
  * @version DLPI_1.0
  *
  * This function is a thread cancellation point.  dlpi_close() is a thread
  * cancellation point, and so is close(2); therefore, we defer cancellation
  * until the call completes.
  */
void
__dlpi_close(dlpi_handle_t dh)
{
	struct __dlpi_user *user;

	if (!(user = __dlpi_chkuser(dh)))
		goto error;
	if (close(dh) == 0 || errno != EINTR) {
		if (--user->refs == 0) {
			__dlpi_dhs[dh] = NULL;
			pthread_wrlock_destroy(&user->lock);
			if (user->ctrlbuf)
				free(user->ctlbuf);
			if (user->datbuf)
				free(user->datbuf);
			free(user);
		}
	}
	if (errno == EINTR)
		dlerrno = DL_SYSERR;
      error:
	return;
}

/** @brief The reentrant version of __dlpi_close().
  * @param dh the link to close.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_close().
  * 
  * This is a little different that most of the _r wrappers: we take a write
  * lock on the __dlpi_dhs list so that we are able to delete the file
  * descriptor from the list.  This will block most other threads from
  * performing functions on the list, also, we must wait for a quiet period
  * until all other functions that read lock the list are not being used.  If
  * you are sure that the close will only be performed by one thread and that
  * no other thread will act on the file descriptor until close returns, use
  * the non-recursive version.
  */
void
__dlpi_close_r(dlpi_handle_t dh)
{
	int err, ret = -1;

	pthread_cleanup_push_defer_np(__dlpi_list_unlock, NULL);
	if ((err = __dlpi_list_wrlock()) == 0) {
		ret = __dlpi_close(dh);
		__dlpi_list_unlock(NULL);
	} else {
		dlerrno = DL_SYSERR;
		errno = err;
	}
	pthread_cleanup_pop_restore_np(0);
	return (ref);
}

/** @fn void dlpi_close(dlpi_handle_t dh)
  * @param dh the link to close.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_close_r().
  */
__asm__(".symver __dlpi_close_r,dlpi_close@@DLPI_1.0");

/** @brief disable a multicast address.
  * @param dh link for which to disable address.
  * @param aptr pointer to multicast address to disabled.
  * @param alen length of multicase address.
  */
int
__dlpi_disabmulti(dlpi_handle_t dh, const void *aptr, size_t alen)
{
}

/** @brief The reentrant version of __dlpi_disabmulti().
  * @param dh link for which to disable address.
  * @param aptr pointer to multicast address to disabled.
  * @param alen length of multicase address.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_disabmulti().
  */
int
__dlpi_disabmulti_r(dlpi_handle_t dh, const void *aptr, size_t alen)
{
}

/** @fn int dlpi_disabmulti(dlpi_handle_t dh, const void *aptr, size_t alen)
  * @param dh link for which to disable address.
  * @param aptr pointer to multicast address to disabled.
  * @param alen length of multicase address.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_diabmulti_r().
  */
__asm__(".symver __dlpi_disabmulti_r,dlpi_disabmulti@@DLPI_1.0");

/** @brief disable a notification.
  * @param dh link for which to disable notification.
  * @param id notification handle from dlpi_enabnotify().
  * @param argp returned argument associated with notification.
  */
int
__dlpi_disabnotify(dlpi_handle_t dh, dlpi_notifyid_t id, void **argp)
{
}

/** @brief The reentrant version of __dlpi_disabnotify().
  * @param dh link for which to disable notification.
  * @param id notification handle from dlpi_enabnotify().
  * @param argp returned argument associated with notification.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_disabnotify().
  */
int
__dlpi_disabnotify_r(dlpi_handle_t dh, dlpi_notifyid_t id, void **argp)
{
}

/** @fn int dlpi_disabnotify(dlpi_handle_t dh, dlpi_notifyid_t id, void **argp)
  * @param dh link for which to disable notification.
  * @param id notification handle from dlpi_enabnotify().
  * @param argp returned argument associated with notification.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_disabnotify_r().
  */
__asm__(".symver __dlpi_disabnotify_r,dlpi_disabnotify@@DLPI_1.0");

/** @brief enable a multicast address.
  * @param dh link for which to enable address.
  * @param aptr pointer to multicast address to enable.
  * @param alen length of multicase address.
  */
int
__dlpi_enabmulti(dlpi_handle_t dh, const void *aptr, size_t alen)
{
}

/** @brief The reentrant version of __dlpi_enabmulti().
  * @param dh link for which to enable address.
  * @param aptr pointer to multicast address to enable.
  * @param alen length of multicase address.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_enabmulti().
  */
int
__dlpi_enabmulti_r(dlpi_handle_t dh, const void *aptr, size_t alen)
{
}

/** @fn int dlpi_enabmulti(dlpi_handle_t dh, const void *aptr, size_t alen)
  * @param dh link for which to enable address.
  * @param aptr pointer to multicast address to enable.
  * @param alen length of multicase address.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_enabmulti_r().
  */
__asm__(".symver __dlpi_enabmulti_r,dlpi_enabmulti@@DLPI_1.0");

/** @brief enable notification for a link.
  * @param dh link for notification.
  * @param fncp notification callback function.
  * @param arg argument to pass to callback.
  * @param nid location to receive notification handle.
  */
int
__dlpi_enabnotify(dlpi_handle_t dh, uint notes, dlpi_notifyfunc_t * fncp, void *arg,
		  dlpi_notifyid_t * nid)
{
}

/** @brief The reentrant version of __dlpi_enabnotify().
  * @param dh link for notification.
  * @param fncp notification callback function.
  * @param arg argument to pass to callback.
  * @param nid location to receive notification handle.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_enabnotify().
  */
int
__dlpi_enabnotify_r(dlpi_handle_t dh, uint notes, dlpi_notifyfunc_t * fncp, void *arg,
		    dlpi_notifyid_t * nid)
{
}

/** @fn int dlpi_enabnotify(dlpi_handle_t dh, uint notes, dlpi_notifyfunc_t * fncp, void *arg, dlpi_notifyid_t * nid)
  * @param dh link for notification.
  * @param fncp notification callback function.
  * @param arg argument to pass to callback.
  * @param nid location to receive notification handle.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_enabnotify_r().
  */
__asm__(".symver __dlpi_enabnotify_r,dlpi_enabnotify@@DLPI_1.0");

/** @brief return file description for link.
  * @param dh link for which to return file descriptor.
  */
int
__dlpi_fd(dlpi_handle_t dh)
{
	struct __dlpi_user *user;

	if ((user = __dlpi_chkuser(dh)) == NULL)
		goto error;
	return (dh);
      error:
	dlerrno = DL_SYSERR;
	errno = EBADF;
	return (-1);
}

/** @brief The reentrant version of __dlpi_fd().
  * @param dh link for which to return file descriptor.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_fd().
  */
int
__dlpi_fd_r(dlpi_handle_t dh)
{
	int ret = -1;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &dh);
	if (__dlpi_hlduser(dh)) {
		ret = __dlpi_fd(dh);
		__dlpi_putuser(&dh);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int dlpi_fd(dlpi_handle_t dh)
  * @param dh link for which to return file descriptor.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_fd_r().
  */
__asm__(".symver __dlpi_fd_r,dlpi_fd@@DLPI_1.0");

/** @brief get physical address of link.
  * @param dh link for which to get physical address.
  * @param type type of physical address to retrieve.
  * @param aptr pointer to buffer to received address.
  * @param alen length of buffer on call and return.
  */
int
__dlpi_get_physaddr(dlpi_handle_t dh, uint type, void *aptr, size_t *alen)
{
	struct __dlpi_user *user;
	union DL_primitives prim;
	struct strbuf ctrl;

	if ((user = __dlpi_chkuser(dh)) == NULL)
		goto error;

	prim.phys_addr_req.dl_primitive = DL_PHYS_ADDR_REQ;
	prim.phys_addr_req.dl_addr_type = type;

	ctrl.buf = (uchar *) &prim;
	ctrl.len = sizeof(prim.phys_addr_req);
	ctrl.maxlen = -1;

	if (putmsg(dh, &ctrl, NULL, 0) < 0)
		goto syserr;

}

/** @brief The reentrant version of __dlpi_get_physaddr().
  * @param dh link for which to get physical address.
  * @param type type of physical address to retrieve.
  * @param aptr pointer to buffer to received address.
  * @param alen length of buffer on call and return.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_get_physaddr().
  */
int
__dlpi_get_physaddr_r(dlpi_handle_t dh, uint type, void *aptr, size_t *alen)
{
	int ret = DLPI_FAILURE;

	pthread_cleanup_push_defer_np(__dlpi_putuser, &dh);
	if (__dlpi_getuser(dh)) {
		ret = __dlpi_get_physaddr(dh, type, aptr, alen);
		__dlpi_putuser(dh);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

/** @fn int dlpi_get_physaddr(dlpi_handle_t dh, uint type, void *aptr, size_t *alen)
  * @param dh link for which to get physical address.
  * @param type type of physical address to retrieve.
  * @param aptr pointer to buffer to received address.
  * @param alen length of buffer on call and return.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_get_physaddr_r().
  */
__asm__(".symver __dlpi_get_physaddr_r,dlpi_get_physaddr@@DLPI_1.0");

/** @brief convert MAC type to IF type.
  * @param mactype MAC type to convert.
  */
uint
__dlpi_iftype(uint mactype)
{
	switch (mactype) {
	case DL_CSMACD:	/* IEEE 802.3 CSMA/CD network */
		return (7);	/* iso88023Csmacd(7) */
	case DL_TPB:		/* IEEE 802.4 Token Passing Bus */
		return (8);	/* iso88024TokenBus(8) */
	case DL_TPR:		/* IEEE 802.5 Token Passing Ring */
		return (9);	/* iso88025TokenRing(9) */
	case DL_METRO:		/* IEEE 802.6 Metro Net */
		return (10);	/* iso88026Man(9) */
	case DL_ETHER:		/* Ethernet Bus */
		return (6);	/* ethernetCsmacd(6) */
	case DL_HDLC:		/* ISO HDLC protocol support */
		return (118);	/* hdlc(118) */
	case DL_CHAR:		/* Character Synchronous protocol support */
		return (83);	/* bsc(83) */
	case DL_CTCA:		/* IBM Channel-to-Channel Adapter */
		return (72);	/* ibm370parChan(72) */
	case DL_FDDI:		/* Fiber Distributed data interface */
		return (15);	/* fddi(15) */
	case DL_FC:		/* Fibre Channel interface */
		return (56);	/* fibreChannel(56) */
	case DL_ATM:		/* ATM */
		return (37);	/* atm(37) */
	case DL_IPATM:		/* ATM Classical IP interface */
		return (114);	/* ipOverAtm(114) */
	case DL_X25:		/* X.25 LAPB interface */
		return (16);	/* lapb(16) */
	case DL_ISDN:		/* ISDN interface */
		return (77);	/* lapd(77) */
	case DL_HIPPI:		/* HIPPI interface */
		return (47);	/* hippi(47) */
	case DL_100VG:		/* 100 Based VG Ethernet */
		return (6);	/* ethernetCsmacd(6) */
	case DL_100VGTPR:	/* 100 Based VG Token Ring */
		return (9);	/* iso88025TokenRing(9) */
	case DL_ETH_CSMA:	/* ISO 8802/3 and Ethernet */
		return (6);	/* ethernetCsmacd(6) */
	case DL_100BT:		/* 100 Base T */
		return (6);	/* ethernetCsmacd(6) */
	case DL_IB:		/* Infiniband */
		return (199);	/* infiniband(199) */
	case DL_FRAME:		/* Frame Relay LAPF */
		return (119);	/* lapf(119) */
	case DL_MPFRAME:	/* Multi-protocol over Frame Relay */
		return (92);	/* frameRelayMPI(92) */
	case DL_ASYNC:		/* Character Asynchronous Protocol */
		return (84);	/* async(84) */
	case DL_IPX25:		/* X.25 Classical IP interface */
		return (5);	/* rfc877x25(5) */
	case DL_LOOP:		/* software loopback */
		return (24);	/* softwareLoopback(24) */
	case DL_OTHER:		/* Any other medium not listed above */
		return (1);	/* other(1) */
	case DL_IPV4:		/* IPv4 Tunnel Link */
		return (136);	/* tunnel(136) */
	case DL_IPV6:		/* IPv6 Tunnel Link */
		return (136);	/* tunnel(136) */
	case DL_VNI:		/* Virtual network interface */
		return (135);	/* l2vlan(135) */
	case DL_WIFI:		/* IEEE 802.11 */
		return (71);	/* ieee80211(71) */
	}
	return (0);
}

/** @brief The reentrant version of __dlpi_iftype().
  * @param mactype MAC type to convert.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_iftype().
  */
uint
__dlpi_iftype_r(uint mactype)
{
	return __dlpi_iftype(mactype);
}

/** @fn uint dlpi_iftype(uint mactype)
  * @param mactype MAC type to convert.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_iftype_r().
  */
__asm__(".symver __dlpi_iftype_r,dlpi_iftype@@DLPI_1.0");

/** @brief get link information.
  * @param dh link for which to get information.
  * @param iptr information structu pointer to return.
  * @param opt options.
  */
int
__dlpi_info(dlpi_handle_t dh, dlpi_info_t ** iptr, uint opt)
{
}

/** @brief The reentrant version of __dlpi_info().
  * @param dh link for which to get information.
  * @param iptr information structu pointer to return.
  * @param opt options.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_info().
  */
int
__dlpi_info_r(dlpi_handle_t dh, dlpi_info_t ** iptr, uint opt)
{
}

/** @fn int dlpi_info(dlpi_handle_t dh, dlpi_info_t **iptr, uint opt)
  * @param dh link for which to get information.
  * @param iptr information structu pointer to return.
  * @param opt options.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_info_r().
  */
__asm__(".symver __dlpi_info_r,dlpi_info@@DLPI_1.0");

/** @brief name a link.
  * @param dh link to name.
  */
const char *
__dlpi_linkname(dlpi_handle_t dh)
{
}

/** @brief The reentrant version of __dlpi_linkname().
  * @param dh link to name.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_linkname().
  */
const char *
__dlpi_linkname_r(dlpi_handle_t dh)
{
}

/** @fn const char * dlpi_linkname(dlpi_handle_t dh)
  * @param dh link to name.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_linkname_r().
  */
__asm__(".symver __dlpi_linkname_r,dlpi_linkname@@DLPI_1.0");

/* *INDENT-OFF* */
const char *__dlpi_maclist[] = {
/*
TRANS  This MAC type corresponds to an interface to an IEEE 802.3 CSMA/CD
TRANS  network.
*/
	gettext_noop("IEEE 802.3 CSMA/CD"),
/*
TRANS  This MAC type corresponds to an interface to an IEEE 802.4 Token
TRANS  Passing Bus network.
*/
	gettext_noop("IEEE 802.4 Token Passing Bus"),
/*
TRANS  This MAC type corresponds to an interface to an IEEE 802.5 Token
TRANS  Passing Ring (Token Ring).
*/
	gettext_noop("IEEE 802.5 Token Passing Ring"),
/*
TRANS  This MAC type corresponds to an interface to an IEEE 802.6 Metro Net
TRANS  network.
*/
	gettext_noop("IEEE 802.6 Metro Net"),
/*
TRANS  This MAC type corresponds to an interface to an Ethernet Bus network.
*/
	gettext_noop("Ethernet Bus"),
/*
TRANS  This MAC type corresponds to an interface to ISO HDLC data link.
*/
	gettext_noop("ISO HDLC Protocol Support"),
/*
TRANS  This MAC type corresponds to an interface to Character Syncrhonous data
TRANS  link.  This may be synchronous character protocols such as BSC or
TRANS  DDCMP.
*/
	gettext_noop("Character Synchronous Protocol Support"),
/*
TRANS This MAC type corresponds to an IBM Channel-to-Channel Adapter.
*/
	gettext_noop("IBM Channel-to-Channel Adapter"),
/*
TRANS This MAC type corresponds to a Fiber Distributed Data Interface (FDDI)
TRANS to an FDDI LAN.
*/
	gettext_noop("Fiber Distributed Data Interface"),
/*
TRANS
*/
	gettext_noop("Other Interface"),
/*
TRANS  This MAC type corresponds to a Frame Relay LAPF (Link Access Procedure
TRANS  Frame-Relay) WAN data link.
*/
	gettext_noop("Frame Relay LAPF"),
/*
TRANS  This MAC type corresponds to an interface using Multi-protocol over
TRANS  Frame Relay.
*/
	gettext_noop("Multi-protocol over Frame Relay"),
/*
TRANS
*/
	gettext_noop("Character Asyncrhonous Protocol"),
/*
TRANS
*/
	gettext_noop("X.25 Classical IP Interface"),
/*
TRANS
*/
	gettext_noop("Software Loopback"),
/*
TRANS
*/
	gettext_noop("Fibre Channel Interface"),
/*
TRANS
*/
	gettext_noop("ATM (Asynchronous Transfer Mode)"),
/*
TRANS
*/
	gettext_noop("ATM Classical IP Interface"),
/*
TRANS
*/
	gettext_noop("X.25 LAPB Interface"),
/*
TRANS
*/
	gettext_noop("ISDN Interface"),
/*
TRANS
*/
	gettext_noop("HIPPI Interface"),
/*
TRANS
*/
	gettext_noop("100 Based VG Ethernet"),
/*
TRANS
*/
	gettext_noop("100 Based VG Token Ring"),
/*
TRANS
*/
	gettext_noop("ISO 8802/3 and Ethernet"),
/*
TRANS
*/
	gettext_noop("100 Base T"),
/*
TRANS
*/
	gettext_noop("Infiniband"),
/*
TRANS
*/
	gettext_noop("IPv4 Tunnel Link"),
/*
TRANS
*/
	gettext_noop("IPv6 Tunnel Link"),
/*
TRANS
*/
	gettext_noop("Virtual Network Interface"),
/*
TRANS
*/
	gettext_noop("IEEE 802.11"),
};
/* *INDENT-ON* */

/** @brief name a MAC type.
  * @param mactype MAC type to name.
  */
const char *
__dlpi_mactype(uint mactype)
{
}

/** @brief The reentrant version of __dlpi_mactype().
  * @param mactype MAC type to name.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_mactype().
  */
const char *
__dlpi_mactype_r(uint mactype)
{
}

/** @fn const char *dlpi_mactype(uint mactype)
  * @param mactype MAC type to name.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_mactype_r().
  */
__asm__(".symver __dlpi_mactype_r,dlpi_mactype@@DLPI_1.0");

/** @brief open a link.
  * @param linkname path name for link.
  * @param dhp area to return link handle.
  * @param flags open flags.
  *
  * The approach here is to open the /dev/streams/clone/dlmux multiplexing
  * driver and then issue the DLIOCGETPPA using the linkname as an argument to
  * obtain the PPA to be used to attach the Stream, and then issueing a
  * DL_ATTACH_REQ using that PPA.  Once the Stream is in the DL_UNBOUND state,
  * issue a DLIOCRAW if DLPI_RAW is set in flags, DLIOCNATIVE if DLPI_NATIVE
  * is set in flags, and DL_PASSIVE_REQ if DLPI_PASSIVE is set in flags.
  */
int
__dlpi_open(const char *linkname, dlpi_handle_t * dhp, uint flags)
{
	int fd = 0, ppa, err, save, flags, type;
	struct __dlpi_user *user = NULL;
	struct strioctl ic;
	union DL_primitives prim;
	struct strbuf ctrl;
	struct pollfd pfd;

	if (linkname == NULL | strnlen(linkname, 17) > 16)
		goto elinknameinval;

	if ((user = malloc(sizeof(*user))) == NULL)
		goto syserr;
	bzero(user, sizeof(*user));

	strncpy(user->linkname, linkname, 16);

	if ((fd = open("/dev/streams/clone/dlmux", O_RDWR)) < 0)
		goto syserr;

	user->fd = fd;

	ic.ic_cmd = DLIOCGETPPA;
	ic.ic_timout = 0;
	ic.ic_len = strnlen(linkname, 16);
	ic.ic_dp = (char *) linkname;

	if ((ppa = ioctl(fd, I_STR, &ic)) == -1) {
		if (errno == ESRCH)
			goto nolink;
		goto syserr;
	}
	if (ppa == 0)
		goto badlink;

	user->ppa = ppa;

	prim.dl_primitive = DL_ATTACH_REQ;
	prim.attach_req.dl_ppa = ppa;

	ctrl.buf = (unsigned char *) &prim;
	ctrl.len = sizeof(prim.attach_req);
	ctrl.maxlen = -1;

	if (putmsg(fd, &ctrl, NULL, 0) < 0)
		goto syserr;

	pfd.fd = fd;
	pfd.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLERR | POLLHUP | POLLMSG;
	pfd.revents = 0;

	switch (poll(&pfd, 1, 100)) {
	default:
	case -1:		/* error */
		goto syserr;
	case 0:		/* timeout */
		goto etimedout;
	case 1:		/* one file descriptor */
		if (pfd.revents & (POLLIN | POLLRDNORM | POLLRDBAND))
			break;
		goto failure;
	}

	ctrl.buf = (unsigned char *) &prim;
	ctrl.len = -1;
	ctrl.maxlen = sizeof(prim) + 32;

	if (getmsg(fd, &ctrl, NULL, &flags) < 0)
		goto syserr;

	if (ctrl.len < sizeof(prim.dl_primitive))
		goto badmsg;

	switch (prim.dl_primitive) {
	case DL_OK_ACK:
		if (ctrl.len < sizeof(prim.ok_ack))
			goto badmsg;
		if (prim.ok_ack.dl_correct_primitive != DL_ATTACH_REQ)
			goto badmsg;
		break;
	case DL_ERROR_ACK:
		if (ctrl.len < sizeof(prim.error_ack))
			goto badmsg;
		if (prim.error_ack.dl_error_primitive != DL_ATTACH_REQ)
			goto badmsg;
		err = prim.error_ack.dl_errno;
		errno = prim.error_ack.dl_unix_errno;
		goto error;
	default:
		goto badmsg;
	}

	if (flags & DLPI_RAW) {
		ic.ic_cmd = DLIOCRAW;
		ic.ic_timout = 0;
		ic.ic_len = 0;
		ic.ic_dp = NULL;
		if (ioctl(fd, I_STR, &ic) < 0) {
			if (errno == EINVAL) {
				/* well.... */
			}
			goto syserr;
		}
	}
	if (flags & DLPI_NATIVE) {
		ic.ic_cmd = DLIOCNATIVE;
		ic.ic_timout = 0;
		ic.ic_len = 0;
		ic.ic_dp = NULL;
		if ((type = ioctl(fd, I_STR, &ic)) < 0) {
			if (errno == EINVAL) {
				/* well.... */
			}
			goto syserr;
		}
		user->mac_type = type;
	}
	if (flags & DLPI_PASSIVE) {
		prim.dl_primitive = DL_PASSIVE_REQ;
		ctrl.buf = (unsigned char *) &prim;
		ctrl.len = sizeof(prim.dl_passive_req);
		ctrl.maxlen = -1;
		if (putmsg(fd, &ctrl, NULL, 0) < 0)
			goto syserr;
		pfd.fd = fd;
		pfd.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLERR | POLLHUP | POLLMSG;
		pfd.revents = 0;

		switch (poll(&pfd, 1, 100)) {
		default:
		case -1:	/* error */
			goto syserr;
		case 0:	/* timeout */
			goto etimedout;
		case 1:	/* one file descriptor */
			if (pfd.revents & (POLLIN | POLLRDNORM | POLLRDBAND))
				break;
			goto failure;
		}
		ctrl.buf = (unsigned char *) &prim;
		ctrl.len = -1;
		ctrl.maxlen = sizeof(prim) + 32;

		if (getmsg(fd, &ctrl, NULL, &flags) < 0)
			goto syserr;

		if (ctrl.len < sizeof(prim.dl_primitive))
			goto badmsg;

		switch (prim.dl_primitive) {
		case DL_OK_ACK:
			if (ctrl.len < sizeof(prim.ok_ack))
				goto badmsg;
			if (prim.ok_ack.dl_correct_primitive != DL_PASSIVE_REQ)
				goto badmsg;
			break;
		case DL_ERROR_ACK:
			if (ctrl.len < sizeof(prim.error_ack))
				goto badmsg;
			if (prim.error_ack.dl_error_primitive != DL_PASSIVE_REQ)
				goto badmsg;
			err = prim.error_ack.dl_errno;
			errno = prim.error_ack.dl_unix_errno;
			goto error;
		default:
			goto badmsg;
		}
	}
	*dlp = (void *) user;
	return (DLPI_SUCCESS);

      elinknameinval:
	err = DLPI_ELINKNAMEINVAL;
      failure:
	err = DLPI_FAILURE;
	goto error;
      etimedout:
	err = DLPI_ETIMEDOUT;
	goto error;
      badlink:
	err = DLPI_EBADLINK;
	goto error;
      nolink:
	err = DLPI_NOLINK;
	goto error;
      badmsg:
	err = DLPI_BADMSG;
	goto error;
      syserr:
	err = DL_SYSERR;
	goto error;
      error:
	save = errno;
	if (fd > 0)
		close(fd);
	if (user != NULL)
		free(user);
	errno = save;
	return (err);
}

/** @brief The reentrant version of __dlpi_open().
  * @param linkname path name for link.
  * @param dhp area to return link handle.
  * @param flags open flags.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_open().
  */
int
__dlpi_open_r(const char *linkname, dlpi_handle_t * dhp, uint flags)
{
}

/** @fn int dlpi_open(const char *linkname, dlpi_handle_t * dhp, uint flags)
  * @param linkname path name for link.
  * @param dhp area to return link handle.
  * @param flags open flags.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_open_r().
  */
__asm__(".symver __dlpi_open_r,dlpi_open@@DLPI_1.0");

/** @brief turn promiscuous mode off for a link.
  * @param dh the link.
  * @param level the level.
  */
int
__dlpi_promiscoff(dlpi_handle_t dh, uint level)
{
}

/** @brief The reentrant version of __dlpi_promiscoff().
  * @param dh the link.
  * @param level the level.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_promiscoff().
  */
int
__dlpi_promiscoff_r(dlpi_handle_t dh, uint level)
{
}

/** @fn int dlpi_promiscoff(dlpi_handle_t dh, uint level)
  * @param dh the link.
  * @param level the level.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_promiscoff_r().
  */
__asm__(".symver __dlpi_promiscoff_r,dlpi_promiscoff@@DLPI_1.0");

/** @brief turn promiscuous mode on for a link.
  * @param dh the link.
  * @param level the level.
  */
int
__dlpi_promiscon(dlpi_handle_t dh, uint level)
{
}

/** @brief The reentrant version of __dlpi_promiscon().
  * @param dh the link.
  * @param level the level.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_promiscon().
  */
int
__dlpi_promiscon_r(dlpi_handle_t dh, uint level)
{
}

/** @fn int dlpi_promiscon(dlpi_handle_t dh, uint level)
  * @param dh the link.
  * @param level the level.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_promiscon_r().
  */
__asm__(".symver __dlpi_promiscon_r,dlpi_promiscon@@DLPI_1.0");

/** @brief receive a message or callback.
  * @param dh the link.
  * @param saptr the source address buffer pointer.
  * @param salen the length of the source address buffer (on call and return).
  * @param buf the buffer for user data.
  * @param buflen the length of the buffer for user data.
  * @param recvp a pointer to a dlpi_recvinfo_t structure.
  */
int
__dlpi_recv(dlpi_handle_t dh, void **saptr, size_t *salen, void *buf, size_t *buflen, int wait,
	    dlpi_recvinfo_t * recvp)
{
}

/** @brief The reentrant version of __dlpi_recv().
  * @param dh the link.
  * @param saptr the source address buffer pointer.
  * @param salen the length of the source address buffer (on call and return).
  * @param buf the buffer for user data.
  * @param buflen the length of the buffer for user data.
  * @param recvp a pointer to a dlpi_recvinfo_t structure.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_recv().
  */
int
__dlpi_recv_r(dlpi_handle_t dh, void **saptr, size_t *salen, void *buf, size_t *buflen, int wait,
	      dlpi_recvinfo_t * recvp)
{
}

/** @fn int dlpi_recv(dlpi_handle_t dh, void **saptr, size_t *salen, void *buf, size_t *buflen, int wait, dlpi_recvinfo_t * recvp)
  * @param dh the link.
  * @param saptr the source address buffer pointer.
  * @param salen the length of the source address buffer (on call and return).
  * @param buf the buffer for user data.
  * @param buflen the length of the buffer for user data.
  * @param recvp a pointer to a dlpi_recvinfo_t structure.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_recv_r().
  */
__asm__(".symver __dlpi_recv_r,dlpi_recv@@DLPI_1.0");

/** @brief send message.
  * @param daptr destination address pointer.
  * @param dalen destination address length.
  * @param buf message buffer.
  * @param buflen message buffer lenght.
  * @param sendp a pointer to a dlpi_sendinfo_t structure.
  */
int
__dlpi_send(dlpi_handle_t dh, const void *daptr, size_t dalen, const void *buf, size_t buflen,
	    const dlpi_sendinfo_t * sendp)
{
}

/** @brief The reentrant version of __dlpi_send().
  * @param daptr destination address pointer.
  * @param dalen destination address length.
  * @param buf message buffer.
  * @param buflen message buffer lenght.
  * @param sendp a pointer to a dlpi_sendinfo_t structure.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_send().
  */
int
__dlpi_send_r(dlpi_handle_t dh, const void *daptr, size_t dalen, const void *buf, size_t buflen,
	      const dlpi_sendinfo_t * sendp)
{
}

/** @fn int dlpi_send(dlpi_handle_t dh, const void *daptr, size_t dalen, const void *buf, size_t buflen, const dlpi_sendinfo_t *sendp)
  * @param daptr destination address pointer.
  * @param dalen destination address length.
  * @param buf message buffer.
  * @param buflen message buffer lenght.
  * @param sendp a pointer to a dlpi_sendinfo_t structure.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_send_r().
  */
__asm__(".symver __dlpi_send_r,dlpi_send@@DLPI_1.0");

/** @brief set a link physical address.
  * @param dh the link.
  * @param type the type of physical address.
  * @param aptr the address pointer.
  * @param alen the address length.
  */
int
__dlpi_set_physaddr(dlpi_handle_t dh, uint type, const void *aptr, size_t alen)
{
}

/** @brief The reentrant version of __dlpi_set_physaddr().
  * @param dh the link.
  * @param type the type of physical address.
  * @param aptr the address pointer.
  * @param alen the address length.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_set_physaddr().
  */
int
__dlpi_set_physaddr_r(dlpi_handle_t dh, uint type, const void *aptr, size_t alen)
{
}

/** @fn int dlpi_set_physaddr(dlpi_handle_t dh, uint type, const void *aptr, size_t alen)
  * @param dh the link.
  * @param type the type of physical address.
  * @param aptr the address pointer.
  * @param alen the address length.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_set_physaddr_r().
  */
__asm__(".symver __dlpi_set_physaddr_r,dlpi_set_physaddr@@DLPI_1.0");

/** @brief set a timeout.
  * @param dh the link.
  * @param seconds the seconds to wait.
  */
int
__dlpi_set_timeout(dlpi_handle_t dh, int seconds)
{
}

/** @brief The reentrant version of __dlpi_set_timeout().
  * @param dh the link.
  * @param seconds the seconds to wait.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_set_timeout().
  */
int
__dlpi_set_timeout_r(dlpi_handle_t dh, int seconds)
{
}

/** @fn int dlpi_set_timeout(dlpi_handle_t dh, int seconds)
  * @param dh the link.
  * @param seconds the seconds to wait.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_set_timeout_r().
  */
__asm__(".symver __dlpi_set_timeout_r,dlpi_set_timeout@@DLPI_1.0");

/* *INDENT-OFF* */
const char *__dlpi_lib_errstr[] = {
/*
TRANS  DLPI_SUCCESS: DLPI operation succeeded.  The DLPI Library call completed
TRANS  successfully.  Any other value returned from a DLPI Library function
TRANS  indicates failure.
 */
	[DLPI_SUCCESS		- DLPI_SUCCESS] = gettext_noop("DLPI operation succeeded"),
/*
TRANS  DLPI_EINVAL: Invalid argument.  One of the arguments provided to the DLPI
TRANS  Library call was invalid, contained invalid information, or was not
TRANS  formatted correctly.  This only applies to DLPI Library arguments passed
TRANS  to public interfaces belonging to the DLPI Library.  This error code is a
TRANS  generic error condition used only when there is not a more specific error
TRANS  condition that applies.
 */
	[DLPI_EINVAL		- DLPI_SUCCESS] = gettext_noop("Invalid argument"),
/*
TRANS  DLPI_ELINKNAMEINVAL: Invalid DLPI linkname.  The linkname (first)
TRANS  argument to the dlpi_open() library call was invalid (e.g. NULL),
TRANS  contained invalid information (e.g.  a null string), or was not formatted
TRANS  correctly (e.g.  contained control characters).  This error code is only
TRANS  returned in response to the dlpi_open() library call.  See dlpi_open(3).
 */
	[DLPI_ELINKNAMEINVAL	- DLPI_SUCCESS] = gettext_noop("Invalid DLPI linkname"),
/*
TRANS  DLPI_ENOLINK: DLPI link does not exist.  The linkname (first) argument to
TRANS  the dlpi_open() library call was formatted correctly; however, the link
TRANS  to which it refers dos not exist.  This error code is only returned in
TRANS  response to the dlpi_open() library call.  See dlpi_open(3).
 */
	[DLPI_ENOLINK		- DLPI_SUCCESS] = gettext_noop("DLPI link does not exist"),
/*
TRANS  DLPI_EBADLINK: Bad DLPI link.  The linkname (first) argument to the
TRANS  dlpi_open() library call was formatted correctly, and refered to a
TRANS  existing link; however, the existing link device is unusable (e.g. it
TRANS  only supports DLPI version 1 and DLPI version 2 is required for proper
TRANS  operation).  This error code is only returned in response to the
TRANS  dlpi_open() library call.  See dlpi_open(3).
 */
	[DLPI_EBADLINK		- DLPI_SUCCESS] = gettext_noop("Bad DLPI link"),
/*
TRANS  DLPI_EINHANDLE: Invalid DLPI handle.  When a link is opened successfully
TRANS  using the dlpi_open() call, it returns a handle of type dlpi_handle_t
TRANS  that is later use as the first argument to many DLPI Library call and is
TRANS  used to make reference to the open link.  When the link is closed, the
TRANS  handle that corresponded to the open link can no longer be used.  This
TRANS  error indicate that the passed in link handle is invalid and does not
TRANS  reference an open data link.  See dlpi_open(3) and dlpi_close(3).
 */
	[DLPI_EINHANDLE		- DLPI_SUCCESS] = gettext_noop("Invalid DLPI handle"),
/*
TRANS  DLPI_ETIMEDOUT: DLPI operation timed out.  To effect many DLPI Library
TRANS  calls, the library passed DLPI primitives to the DLS provider on the open
TRANS  Stream and expects a response from the DLS provider.  To avoid hanging
TRANS  the calling process, when the expected response is not fortcoming within
TRANS  a user-settable time interval, the library call returns with this error
TRANS  code.
 */
	[DLPI_ETIMEDOUT		- DLPI_SUCCESS] = gettext_noop("DLPI operation timed out"),
/*
TRANS  DLPI_EVERNOTSUP: Unsupported DLPI version.  Some DLPI Library functions
TRANS  rely upon the DLS provider associated with the data link providing DLPI
TRANS  Revision 2.0.0 compatible support.  This is particularly required by the
TRANS  dlpi_info(3) library call.  When a DLPI library call discovers that the
TRANS  level of support required by the library is insufficient, it returns with
TRANS  this error code.  See dlpi_info(3) and DL_INFO_ACK(7).
 */
	[DLPI_EVERNOTSUP	- DLPI_SUCCESS] = gettext_noop("Unsupported DLPI version"),
/*
TRANS  DLPI_EMODENOTSUP: Unsupported DLPI connection mode.  The DLPI Library
TRANS  only supports connectionless mode data link service.  Should a data link
TRANS  only provide connection-oriented or acknowledged connectionless data link
TRANS  services, then the dlpi_info(3) library call will return this error code.
TRANS  Note that the DL_UNSUPPORTED error code will be returned in the same
TRANS  situation for dlpi_bind(3).  See dlpi_info(3) and DL_INFO_ACK(7).
 */
	[DLPI_EMODENOTSUP	- DLPI_SUCCESS] = gettext_noop("Unsupported DLPI connection mode"),
/*
TRANS  DLPI_EUNAVAILSAP: Unavailable DLPI SAP.  The DLPI Library always attempts
TRANS  to bind to the requested LSAP selector.  If the DLS provider binds to an
TRANS  LSAP other than the requested one (e.g., the DLS provider selects an
TRANS  alternate LSAP in accordance with DL_BIND_REQ rules), then the DLPI
TRANS  Library call, dlpi_bind(), fails with this error code.  See dlpi_bind(3)
TRANS  and DL_BIND_REQ(7).
 */
	[DLPI_EUNAVAILSAP	- DLPI_SUCCESS] = gettext_noop("Unavailable DLPI SAP"),
/*
TRANS  DLPI_FAILURE: DLPI operation failed.  When a DLPI primitive operation or
TRANS  recovery action performed by a DLPI Library function fails, and there is
TRANS  no other DLPI Library error that is appropriate, or a double-error
TRANS  condition has occured during recovery, the DLPI Library call returns this
TRANS  error code.  See dlpi_open(3) and dlpi_bind(3).
 */
	[DLPI_FAILURE		- DLPI_SUCCESS] = gettext_noop("DLPI operation failed"),
/*
TRANS  DLPI_ENOTSYTLE2: DLPI style-2 node reports style-1.  When the DLPI
TRANS  Library opens a DLPI Stream, it performs transparent opening of Style-1
TRANS  and Style-2 DLPI Streams.  Using the linkname and following the search
TRANS  algorithm of the dlpi_open() call, the DLPI Library expects some named
TRANS  devices to be Style-2.  If however, such a device happens to report
TRANS  Style-1, this error is returned.  See dlpi_open(3) and DL_ATTACH_REQ(7).
 */
	[DLPI_ENOTSTYLE2	- DLPI_SUCCESS] = gettext_noop("DLPI style-2 node reports style-1"),
/*
TRANS  DLPI_EBADMSG: Bad DLPI message.  When the DLPI Library receives a DLPI
TRANS  primitive from the DLS provider, and the DLPI primitive is recognized and
TRANS  expected, but improperly formatted (according to the DLPI Revision 2.0.0
TRANS  standard or extension specifications), this error code is returned.  This
TRANS  error code is not returned by any function in the DLPI Library for a
TRANS  proper functioning DLS provider.
 */
	[DLPI_EBADMSG		- DLPI_SUCCESS] = gettext_noop("Bad DLPI message"),
/*
TRANS  DLPI_ERAWNOTSUP: DLPI raw mode not supported.  The DLS user may specify
TRANS  the DLPI_RAW flags on the call to dlpi_open() to request raw mode of
TRANS  operation for the data link.  However, not all DLS providers support raw
TRANS  mode.  In the case that the DLS provider does not support raw mode, this
TRANS  error code is returned.  See dlpi_open(3) and dlpi_ioctl(4).
 */
	[DLPI_ERAWNOTSUP	- DLPI_SUCCESS] = gettext_noop("DLPI raw mode not supported"),
/*
TRANS  DLPI_ENOTEINVAL: Invalid DLPI notification type.  This error code is
TRANS  returned by the dlpi_enabnotify() call when the 'notes' (second) argument
TRANS  to that call does not specify a valid notification.  See
TRANS  dlpi_enabnotify(3) and DL_NOTIFY_REQ(7).
 */
	[DLPI_ENOTEINVAL	- DLPI_SUCCESS] = gettext_noop("Invalid DLPI notification type"),
/*
TRANS  DLPI_ENOTENOTSUP: DLPI notification not supported by link.  When the DLS
TRANS  user calls the dlpi_enabnotify() library function and the DLS provider
TRANS  does not support the DL_NOTIFY_REQ primitive, this error code is returned
TRANS  by the library call.  See dlpi_enabnotify(3) and DL_NOTIFY_REQ(7).
 */
	[DLPI_ENOTENOTSUP	- DLPI_SUCCESS] = gettext_noop("DLPI notification not supported by link"),
/*
TRANS  DLPI_ENOTEINVAL: Invalid DLPI notification identifier.  When the DLS user
TRANS  successfuly invokes the dlpi_enabnotify() library call, the DLS user is
TRANS  provided with an identifier that may be used to identify the notificaiton
TRANS  in a subsequent call to dlpi_disabnotify() to disable the asme set of
TRANS  notifications.  Once dlpi_disabnotify() has been called for an identifier
TRANS  handle, the handle is no longer valid.  When an invalid handle is
TRANS  supplied to the dlpi_disabnotify() library call, the call returns this
TRANS  error code.  See dlpi_disabnotify(3).
 */
	[DLPI_ENOTEIDINVAL	- DLPI_SUCCESS] = gettext_noop("Invalid DLPI notification identifier"),
}
/* *INDENT-ON* */

/* *INDENT-OFF* */
const char *__dlpi_std_errstr[] = {
/*
TRANS  DL_BADSAP: Bad LSAP selector.  When the DLS user wishes to bind a Stream
TRANS  to a DLSAP, it does so by specifying the LSAP selector portion of the
TRANS  DLSAP in a DL_BIND_REQ primitive.  This LSAP is subject to rules
TRANS  concerning validity and format by the underlying DLS provider.  In the
TRANS  case that the LSAP specified by the DLS user in the DL_BIND_REQ primitive
TRANS  is invalid or contains an invalid LSAP value, the DLS provider will
TRANS  respond with a DL_ERROR_ACK that may include the DLPI error DL_BADSAP.
TRANS
TRANS  However, the DLPI Revision 2.0.0 specification does not describe this
TRANS  error code being returned in response to a DL_BIND_REQ in this situation,
TRANS  but a DL_BADADDR value instead; therefore, there is no DLPI Library
TRANS  subroutine that will return this function.  See dlpi_bind(3),
TRANS  DL_BIND_REQ(7) and DL_ERROR_ACK(7) for more information.
 */
	[DL_BADSAP		- DL_BADSAP] = gettext_noop("Bad LSAP selector"),
/*
TRANS  DL_BADADDR: DLSAP address in improper format or invalid.  This error code
TRANS  can be returned in a number of contexts, many of which are no supported
TRANS  by the DLPI Library and are not discussed further here.
TRANS
TRANS  When issued by the DLS provider in a DL_ERROR_ACK primitive issued in
TRANS  response to a DL_BIND_REQ primitive containing a bad LSAP selector, it
TRANS  indicates that only the LSAP portion of the DLSAP address was in a in
TRANS  improper or invalid format.  See dlpi_bind(3) and DL_BIND_REQ(7).
TRANS
TRANS  When issued by the DLS provider in response to a DL_ENABMULTI_REQ or
TRANS  DL_DISABMULTI_REQ primitive, it indicates that the multicast address was
TRANS  in an improper or invalid format.  See dlpi_enabmulti(3),
TRANS  dlpi_disabmuti(3), DLPI_ENABMULTI_REQ(7), DL_DISABMULTI_REQ(7).
TRANS
TRANS  When issued by the DLS provider in a DL_UDERROR_IND primitive issued in
TRANS  response to a DL_UNITDATA_REQ primitive containing a bad address, it
TRANS  indicates that the DLSAP addres was in an improper or invalid format.
TRANS  See dlpi_send(3), DL_UNITDATA_REQ(7) and DL_UDERROR_IND(7).
TRANS
TRANS  Other uses of this error code are not supoprted by the DLPI Library.
 */
	[DL_BADADDR		- DL_BADSAP] = gettext_noop("DLSAP address in improper format or invalid"),
/*
TRANS  DL_ACCESS: Improper permissions for request.  This error code can be
TRANS  returned in a number of contexts, many of which are not supported by the
TRANS  DLPI Library and not discussed here.
TRANS
TRANS  When issued in response to a DL_ATTACH_REQ(7) primitive containing a
TRANS  Physical Point of Attachment (PPA) that has restricted access, the
TRANS  DL_ACCESS error code indicates that the DLS user did not have sufficient
TRANS  privilege (when the Stream was opened) to use the requested PPA.  See
TRANS  dlpi_open(3) and DL_ATTACH_REQ(7).
TRANS
TRANS  When issued in response to a DL_BIND_REQ(7) primitive containing a Link
TRANS  Service Access Point (LSAP) selector that has restricted access, the
TRANS  DL_ACCESS error code indicates that the DLS user did not have sufficient
TRANS  privilege (when the Stream was opened) to use the requested LSAP
TRANS  selector.  See dlpi_bind(3) and DL_BIND_REQ(7).
TRANS
TRANS  Other uses of this error code are not supoprted by the DLPI Library.
 */
	[DL_ACCESS		- DL_BADSAP] = gettext_noop("Improper permissions for request"),
/*
TRANS  DL_OUTSTATE: Primitive issued in improper state.  As primitives are
TRANS  exchanged between the DLS user and the DLS provider, the DLS provider
TRANS  tracks the state of the interface toward the DLS user.  Not all
TRANS  primitives are valid in all DLS user-provider interface states.  A
TRANS  DL_ERROR_ACK primitive containing this error code is issued by the DLS
TRANS  providing in response to any primitive requiring negative acknowledgement
TRANS  that requires the DLS provider to be in specific DLPI interface states,
TRANS  and was not issued in one of those specific states.  For the DLPI Libary
TRANS  this consists largely of the bind and unbind subroutines.  See
TRANS  dlpi_bind(3), dlpi_unbind(3), DL_BIND_REQ(7) and DL_UNBIND_REQ(7).
 */
	[DL_OUTSTATE		- DL_BADSAP] = gettext_noop("Primitive issued in improper state"),
/*
TRANS  DL_SYSERR: UNIX system error occured.  During execution of a DLPI Library
TRANS  subroutine or processing by the DLS provider of a DLS user issued
TRANS  primitive, an error may occur that is described by a UNIX system error
TRANS  code per errno(3).  This error code in the DLPI error code indicates that
TRANS  the UNIX system error code is either contained in the dl_unix_errno field
TRANS  of the associate DL_ERROR_ACK primitive, or, when returned by the DLPI
TRANS  Library subroutines, contained in the errno(3) global variable.
 */
	[DL_SYSERR		- DL_BADSAP] = gettext_noop("UNIX system error occurred"),
/*
TRANS  DL_BADCORR: Sequence numnber not from outstanding DL_CONN_IND.  This
TRANS  error code is not currently returned by any function in the DLPI Library
TRANS  for a properly functioning DLS provider.
 */
	[DL_BADCORR		- DL_BADSAP] = gettext_noop("Sequence number not from outstanding DL_CONN_IND"),
/*
TRANS  DL_BADDATA: User data exceeded provider limit.
 */
	[DL_BADDATA		- DL_BADSAP] = gettext_noop("User data exceeded provider limit"),
/*
TRANS  DL_UNSUPPORTED: Requested service not supplied by provider.  When the
TRANS  DLS user wishes to bind a DLPI Stream, the DLS user issues a DL_BIND_REQ
TRANS  primitive that specifies the requested service mode in the
TRANS  dl_service_mode field of the primitive.  As the DLPI Library only
TRANS  supports connectionless mode service, DL_CLDS, the dlpi_bind(3)
TRANS  subroutine always binds with this service type.  When the DLS provider
TRANS  does not support the DL_CLDLS service mode, this error code is returned.
TRANS  See dlpi_bind(3) and DL_BIND_REQ(7).
 */
	[DL_UNSUPPORTED		- DL_BADSAP] = gettext_noop("Requested service not supplied by provider"),
/*
TRANS  DL_BADPPA: Specified PPA wsa invalid.  When the DLS user wishes to attach
TRANS  a Style-2 DLPI Stream to a Physical Point of Attachment (PPA), it issues
TRANS  a DL_ATTACH_REQ primitive to the DLS provider that contains the requested
TRANS  PPA in the dl_ppa field of the primitive.  The DLPI Library transparently
TRANS  attaches Style-2 providers as part of the dlpi_open(3) library call.
TRANS  When the determined PPA is not appropriate for the driver, this error
TRANS  code is returned.  See dlpi_open(3) and DL_ATTACH_REQ(7).
 */
	[DL_BADPPA		- DL_BADSAP] = gettext_noop("Specified PPA was invalid"),
/*
TRANS  DL_BADPRIM: Primitive received not known by provider.  When the DLS
TRANS  provider receivees a primitive from the DLS user that is either too short
TRANS  to be a primitive (is not long enough to contain a dl_primitive field) or
TRANS  contains a dl_primtiive value that indicates a primitive that is unknown
TRANS  to the DLS provider, the DLS provider returnes a DL_ERROR_ACK primitive
TRANS  with the DL_BADPRIM error code.  This error code may be returned for
TRANS  extension primitives that are not known to the DLS provider.  See
TRANS  dlpi_enabnotify(3) and dlpi_disabnotify(3).
 */
	[DL_BADPRIM		- DL_BADSAP] = gettext_noop("Primitive received not known by provider"),
/*
TRANS  DL_BADQOSPARAM: QOS parameters contained invalid values.  This error code
TRANS  is not currently returned by any function in the DLPI Library for a
TRANS  properly functioning DLS provider.  See DL_UDQOS_REQ(7).
 */
	[DL_BADQOSPARAM		- DL_BADSAP] = gettext_noop("QOS parameters contained invalid values"),
/*
TRANS  DL_BADQOSTYPE: QOS structure type is unknown or unsupported.  This error
TRANS  code is not currently returned by any function in the DLPI Library for a
TRANS  properly functioning DLS provider.  See DL_UDQOS_REQ(7).
 */
	[DL_BADQOSTYPE		- DL_BADSAP] = gettext_noop("QOS structure type is unknown or unsupported"),
/*
TRANS  DL_BADTOKEN: Token used not an active stream.  This error code is not
TRANS  currently returned by any function in the DLPI Library for a properly
TRANS  functioning DLS provider.  See DL_CONNECT_RES(7).
 */
	[DL_BADTOKEN		- DL_BADSAP] = gettext_noop("Token used not an active stream"),
/*
TRANS  DL_BOUND: Attempted second bind with dl_max_conind.  This error code is
TRANS  not currently returned by any function in the DLPI Library for a properly
TRANS  functioning DLS provider.  See DL_BIND_REQ(7).
 */
	[DL_BOUND		- DL_BADSAP] = gettext_noop("Attempted second bind with dl_max_conind"),
/*
TRANS  DL_INITFAILED: Physical link initialization failed.  DLS providers are
TRANS  permitted to intitialize the link when it is successfully attached or
TRANS  bound.  The DLPI standard requires that the link be initialized by the
TRANS  time that the DL_BIND_ACK primitive is issued, either using the automatic
TRANS  initialization of the link using the DL_ATTACH_REQ or DL_BIND_REQ
TRANS  primitives, or non-automatic initialization requireing some other
TRANS  mechanism (such as an input-output control).  If link intialization has
TRANS  been attempted and failed, this error code may be returned to the attach
TRANS  or bind.  See dlpi_open(3), dlpi_bind(3), DL_ATTACH_REQ(7) and
TRANS  DL_BIND_REQ(7).
 */
	[DL_INITFAILED		- DL_BADSAP] = gettext_noop("Physical Link initialization failed"),
/*
TRANS  DL_NOADDR: Provider could not allocated alternate address.  When the DLS
TRANS  user attempts to bind to an LSAP selector that results in a DLSAP address
TRANS  that is already bound to another DLPI Stream and the DLS provider does
TRANS  not support multiple DLPI Streams to be bound to the same DLSAP, the DLS
TRANS  provider may attempt to choose an alternate DLSAP address to which to
TRANS  bind.  If an alternate DLSAP address cannot be formulated, the DLS
TRANS  provider may return this error code.  See dlpi_bind(3) and
TRANS  DL_BIND_REQ(7).
 */
	[DL_NOADDR		- DL_BADSAP] = gettext_noop("Provider could not allocate alternate address"),
/*
TRANS  DL_NOTINIT: Physical link not initialized.  DLS providers are permitted
TRANS  to intitialize the link when it is successfully attached or bound.  The
TRANS  DLPI standard requires that the link be initialized by the time that the
TRANS  DL_BIND_ACK primitive is issued, either using the automatic
TRANS  initialization of the link using the DL_ATTACH_REQ or DL_BIND_REQ
TRANS  primitives, or non-automatic initialization requireing some other
TRANS  mechanism (such as an input-output control).  If the DLS provider
TRANS  requires non-automatic initialization of the link and the link has not
TRANS  been initialized at the time that DLS user request the link be attached
TRANS  or bound, this error code is returned.  See dlpi_open(3), dlpi_bind(3),
TRANS  DL_ATTACH_REQ(7) and DL_BIND_REQ(7).
 */
	[DL_NOTINIT		- DL_BADSAP] = gettext_noop("Physical link not initialized"),
/*
TRANS  DL_UNDELIVERABLE: Previous data unit could not be delivered.
 */
	[DL_UNDELIVERABLE	- DL_BADSAP] = gettext_noop("Previous data unit could not be delivered"),
/*
TRANS  DL_NOTSUPPORTED: Primitive is known but not supported.  When the DLS
TRANS  provider receives a primitive that is known (part of the DLPI Revision
TRANS  2.0.0 standard or a known extension) but is not supported by the DLS
TRANS  provider, this error code is returned.  This error code can be returned
TRANS  by the DLPI Library for features which are optionally supported by the
TRANS  DLS provider.  See dlpi_disabmulti(3), dlpi_disabnotify(3),
TRANS  dlpi_enabmulti(3), dlpi_enabnotify(3), dlpi_get_physaddr(3),
TRANS  dlpi_promiscoff(3), dlpi_promiscon(3) and dlpi_set_physaddr(3).
 */
	[DL_NOTSUPPORTED	- DL_BADSAP] = gettext_noop("Primitive is known but not supported"),
/*
TRANS  DL_TOOMANY: Limit exceeded.  When the DLS user binds or enables
TRANS  additional DLSAPs or multicast addresses on a DLPI Stream, the number of
TRANS  DLSAP or multicast addresses that the DLS user attempts to bind may
TRANS  exceed a limit on the number of DLSAPs or multicast addresses that the
TRANS  DLS provider imposes.  In this case, the DLS provider returns this error
TRANS  code in response to the primitive that attempts to exceed the limit.  For
TRANS  the DLPI Library, this error code may be returned by the
TRANS  dlpi_enabmulti(3) library call.  See dlpi_enabmulti(3) and
TRANS  DL_ENABMULTI_REQ(7).
 */
	[DL_TOOMANY		- DL_BADSAP] = gettext_noop("Limit exceeded"),
/*
TRANS  DL_NOENAB: Promiscuous mode or multicast address not enabled.  When the
TRANS  DLS user enables promiscuous mode on a device or enables a multicast
TRANS  address, the state of the interface does not change.  Therefore, if the
TRANS  DLS user requests that a promiscuous mode be turned off or that a
TRANS  multicast address be disabled, and the promiscuous mode was not enabled,
TRANS  or the multicast address was not enabled, this error code is returned by
TRANS  the DLS provider.  See dlpi_promiscoff(3), dlpi_disabmulti(3),
TRANS  DL_PROMISCOFF_REQ(7) and DL_DISABMULTI_REQ(7).
 */
	[DL_NOTENAB		- DL_BADSAP] = gettext_noop("Promiscuous mode or multicast address not enabled"),
/*
TRANS  DL_BUSY: Other streams for PPA in post-attached state.  When a DLS user
TRANS  wishes to set the physical address for a particular PPA on a Style-2 DLPI
TRANS  Stream, the DLS provider will not allow the physical address to be set
TRANS  provided that there is another DLPI Stream attached and bound to the PPA.
TRANS  In the case where the DLS provider must prohibit the setting of the
TRANS  physical address, this error code is returned.  See dlpi_set_physaddr(3)
TRANS  and DL_SET_PHYS_ADDR_REQ(7).
 */
	[DL_BUSY		- DL_BADSAP] = gettext_noop("Other streams for PPA in post-attached state"),
/*
TRANS  DL_NOAUTO: Automatic handling of XID and TEST response not supported.
TRANS  This error code is not currently returned by any function in the DLPI
TRANS  Library for a properly functioning DLS provider.  See DL_BIND_REQ(7).
 */
	[DL_NOAUTO		- DL_BADSAP] = gettext_noop("Automatic handling of XID and TEST response not supported. "),
/*
TRANS  DL_NOXIDAUTO: Automatic handling of XID not supported.  This error code
TRANS  is not currently returned by any function in the DLPI Library for a
TRANS  properly functioning DLS provider.  See DL_BIND_REQ(7).
 */
	[DL_NOXIDAUTO		- DL_BADSAP] = gettext_noop("Automatic handling of XID not supported"),
/*
TRANS  DL_NOTESTAUTO: Automatic handling of TEST not supported.  This error code
TRANS  is not currently returned by any function in the DLPI Library for a
TRANS  properly functioning DLS provider.  See DL_BIND_REQ(7).
 */
	[DL_NOTESTAUTO		- DL_BADSAP] = gettext_noop("Automatic handling of TEST not supported"),
/*
TRANS  DL_XIDAUTO: Automatic handling of XID response.  This error code is not
TRANS  currently returned by any function in the DLPI Library for a properly
TRANS  functioning DLS provider.  See DL_XID_REQ(7).
 */
	[DL_XIDAUTO		- DL_BADSAP] = gettext_noop("Automatic handling of XID response"),
/*
TRANS  DL_TESTAUTO: Automatic handling of TEST response.  This error code is not
TRANS  currently returned by any function in the DLPI Library for a properly
TRANS  functioning DLS provider.  See DL_TEST_REQ(7).
 */
	[DL_TESTAUTO		- DL_BADSAP] = gettext_noop("Automatic handling of TEST response"),
/*
TRANS  DL_PENDING: Pending outstanding connect indications.  This error code is
TRANS  not currently returned by any function in the DLPI Library for a properly
TRANS  functioning DLS provider.  See DL_CONNECT_RES(7).
 */
	[DL_PENDING		- DL_BADSAP] = gettext_noop("Pending outstanding connect indications"),
};
/* *INDENT-ON* */



/** @brief provide a DLPI error string.
  * @param error the error number.
  */
const char *
__dlpi_strerror(int error)
{
	if (error == DL_SYSERR)
		return strerror(errno);
	if (DLPI_SUCCESS <= error && error < DLPI_ERRMAX)
		return __dlpi_lib_errstr[error];
	if (0 <= error && error < 0x1c)
		return __dlpi_std_errstr[error];

}

/** @brief The reentrant version of __dlpi_strerror().
  * @param error the error number.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_strerror().
  */
const char *
__dlpi_strerror_r(int error)
{
	if (error == DL_SYSERR) {
		switch (strerror_r(errno, __dlpi_get_tsd()->strbuf, BUFSIZ)) {
		case EINVAL:
		case ERANGE:
		case 0:
		}
		return (__dlpi_get_tsd()->strbuf);
	}

}

/** @fn const char *dlpi_strerror(int error)
  * @param error the error number.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_strerror_r().
  */
__asm__(".symver __dlpi_strerror_r,dlpi_strerror@@DLPI_1.0");

/** @brief unbind a link.
  * @param dh the link to unbind.
  */
int
__dlpi_unbind(dlpi_handle_t dh)
{
}

/** @brief The reentrant version of __dlpi_unbind().
  * @param dh the link to unbind.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_unbind().
  */
int
__dlpi_unbind_r(dlpi_handle_t dh)
{
}

/** @fn int dlpi_unbind(dlpi_handle_t dh)
  * @param dh the link to unbind.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_unbind_r().
  */
__asm__(".symver __dlpi_unbind_r,dlpi_unbind@@DLPI_1.0");

/** @brief walk the available links.
  * @param fn callback function.
  * @param arg argument for callback function.
  * @param flags options flags.
  */
void
__dlpi_walk(dlpi_walkfunc_t * fn, void *arg, uint flags)
{
}

/** @brief The reentrant version of __dlpi_walk().
  * @param fn callback function.
  * @param arg argument for callback function.
  * @param flags options flags.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is an implementation of dlpi_walk().
  */
void
__dlpi_walk_r(dlpi_walkfunc_t * fn, void *arg, uint flags)
{
}

/** @fn void dlpi_walk(dlpi_walkfunc_t *fn, void *arg, uint flags)
  * @param fn callback function.
  * @param arg argument for callback function.
  * @param flags options flags.
  * @version DLPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __dlpi_walk_r().
  */
__asm__(".symver __dlpi_walk_r,dlpi_walk@@DLPI_1.0");

/* @} */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
