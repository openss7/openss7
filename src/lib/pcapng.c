/*****************************************************************************

 @(#) File: src/lib/pcapng.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
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

 *****************************************************************************/

static char const ident[] = "src/lib/pcapng.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/* This file can be processed with doxygen(1). */

/** @weakgroup pcapng OpenSS7 PCAP-NG Library
  * @{ */

/** @file
  * OpenSS7 PCAP Next Generation Library (PCAP-NG) implementation file. */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#define _REENTRANT
#define _THREAD_SAFE

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

#include <sys/utsname.h>

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
  * This manual contains documentation of the OpenSS7 PCAP-NG Library functions
  * that are generated automatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 PCAP-NG
  * Library and is not intended for users of the OpenSS7 PCAP-NG Library.  Users
  * should consult the documentation found in the user manual pages beginning
  * with pcapng(3).
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 PCAP-NG Library is designed to be thread-safe.  This is
  * accomplished in a number of ways.  Thread-safety depends on the use of glibc2
  * and the pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data for errno and h_errno.
  * Because these used communications functions orthoginal fo the PCAP-NG Library
  * services, we define new thread-specific data for perrno (a PCAP error
  * number) and perrbuf (a PCAP message buffer).
  *
  * Glibc2 also provides some weak undefined aliases for POSIX thread functions
  * to perform its own thread-safety.  When the pthread library (libpthread) is
  * linked with glibc2, these function call libpthread functions instead of
  * internal dummy routines.  The same approach is taken for the OpenSS7 PCAP-NG
  * Library.  The library uses weak defined and undefined aliases that
  * automatically invoke libpthread functions when libpthread is (dynamically)
  * linked and uses dummy functions when it is not.  This maintains maximum
  * efficiency when libpthread is not dynamically linked, but provides full
  * thread safety when it is.
  *
  * Libpthread behaves in some strange ways with regards to thread cancellation.
  * because libpthread uses Linux clone processes for threads, cancellation of a
  * thread is accomplished by sending a signal to the thread process.  This does
  * not directly result in cancellation, but will result in the failure of a
  * system call with the EINTR error code.  It is necessary to test for
  * cancellation upon error return from system calls to perform the actual
  * cancellation of the thread.
  *
  * OpenGroup documents list just about any function that can return EINTR as a
  * thread cancellation point.  In fitting with that, any function that calls a
  * system call function that can return with EINTR is also a thread cancellation
  * point.  Any function that cannot be a thread cancellation point needs to have
  * its cancellation status deferred if it internally invokes a function that
  * permits thread cancellation.  Functions that do not permit thread
  * cancellation are typically functions that are never allowed to block.
  *
  * Locks and asynchronous thread cancellation presents challenges.
  *
  * Functions that act as a thread cancellation point must push routines onto the
  * function stack executed at exit of the thread to release the locks held by
  * the function.  These are performed with weak definitions of POSIX thread
  * library functions.
  *
  * Functions that do not act as thread cancellation points must defer thread
  * cancellation before taking locks and then release locks before the thread
  * cancellation is restored.
  *
  * The above are the techniques used by glibc2 for the same purpose and is the
  * same technique that is used by the OpenSS7 PCAP-NG library.
  */

struct __pcapng_tsd {
	int perrno;
	char perrbuf[PCAP_ERRBUF_SIZE];
};

/* Once condition for thread-specific data key creation. */

static pthread_once_t __pcapng_tsd_once = PTHREAD_ONCE_INIT;

/* PCAP-NG library thread-specific data key */

static pthread_key_t __pcapng_tsd_key = 0;

static void
__pcapng_tsd_free(void *buf)
{
	pthread_setspecific(__pcapng_tsd_key, NULL);
	free(buf);
}

static void
__pcapng_tsd_key_create(void)
{
	pthread_key_create(&__pcapng_tsd_key, __pcapng_tsd_free);
}

static struct __pcapng_tsd *
__pcapng_tsd_alloc(void)
{
	struct __pcap_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(*tsdp));
	memset(tsdp, 0, sizeof(*tsdp));
	pthread_setspecific(__pcapng_tsd_key, (void *) tsdp);
	return (tsdp);
}

static struct __pcapng_tsd *
__pcapng_get_tsd(void)
{
	struct __pcapng_tsd *tsdp;

	pthread_once(&__pcapng_tsd_once, __pcapng_tsd_key_create);
	if (unlikely((tsdp = (typeof(tsdp)) pthread_getspecific(__pcapng_tsd_key)) == NULL))
		tsdp = __pcapng_tsd_alloc();
	return (tsdp);
}

/** @brief #p_errno location function
  * @version PCAPNG_1.0
  * @par Alias:
  * This function is an implementation of _p_errno().
  */
int *
__pcapng__p_errno(void)
{
	return &(__pcapng_get_tsd()->perrno);
}

/** @fn int *_p_errno(void)
  * @version PCAPNG_1.0
  * @par Alias:
  * This symbol is a strong alias of __pcapng__p_errno().
  *
  * This function provides the location of the integer that contains the PCAP-NG
  * error number returned by the last PCAP-NG function that failed.  This is
  * normally used to provide #p_errno in a thread-safe was as follows:
  *
  * @code
  * #define p_errno (*(_p_errno()))
  * @endcode
  */
__asm__(".symver __pcapng__p_errno,_p_errno@@PCAPNG_1.0");

/** @brief #p_errbuf location function
  * @version PCAPNG_1.0
  * @par Alias:
  * This function is an implementation of _p_errbuf().
  */
char *
__pcapng__p_errbuf(void)
{
	return (__pcapng_get_tsd()->perrbuf);
}

/** @fn char *_p_errbuf(void)
  * @version PCAPNG_1.0
  * @par Alias:
  * This symbol is a strong alias of __pcapng__p_errbuf().
  *
  * This function provides the location of the character error buffer that
  * contains the PCAP-NG error string returned by the last PCAP-NG function that
  * failed.  This is normally used to provide #p_errbuf in a thread-safe was as
  * follows:
  *
  * @code
  * #define p_errbuf (_p_errbuf())
  * @endcode
  */
__asm__(".symver __pcapng__p_errbuf,_p_errbuf@@PCAPNG_1.0");

#ifndef PCAP_ERROR_NO_SUCH_INTERFACE
#define PCAP_ERROR_NO_SUCH_INTERFACE -12
#endif

static inline void
__pcapng_set_error(int perror)
{
	if (perror != 0)
		p_errno = perror;
	switch (perror) {
	case 0:
		break;
	case PCAP_ERROR:
		break;
	case PCAP_ERROR_BREAK:
		errno = EINTR;
		break;
	case PCAP_ERROR_NOT_ACTIVATED:
		errno = ENXIO;	// not used
		break;
	case PCAP_ERROR_ACTIVATED:
		errno = EALREADY;
		break;
	case PCAP_ERROR_NO_SUCH_DEVICE:
		errno = ENODEV;
		break;
	case PCAP_ERROR_RFMON_NOTSUP:
		errno = ENOTSUP;
		break;
	case PCAP_ERROR_NOT_RFMON:
		errno = ENOTTY;	// not used
		break;
	case PCAP_ERROR_PERM_DENIED:
		errno = EACCES;
		break;
	case PCAP_ERROR_IFACE_NOT_UP:
		errno = ENETDOWN;
		break;
	case PCAP_ERROR_CANTSET_TSTAMP_TYPE:
		errno = EOPNOTSUPP;
		break;
	case PCAP_ERROR_PROMISC_PERM_DENIED:
		errno = EPERM;
		break;
	case PCAP_ERROR_NO_SUCH_INTERFACE:
		errno = EINVAL;
		break;
	case PCAP_WARNING:
		break;
	case PCAP_WARNING_PROMISC_NOTSUPP:
		errno = ENOTSUP;
		break;
	case PCAP_WARNING_TSTAMP_TYPE_NOTSUPP:
		errno = EOPNOTSUPP;
		break;
	default:
		break;
	}
}

static inline int
__pcapng_lock_rdlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_rdlock(rwlock);
}

static inline int
__pcapng_lock_wrlock(pthread_rwlock_t * rwlock)
{
	return pthread_rwlock_wrlock(rwlock);
}

static inline int
__pcapng_lock_unlock(void *rwlock)
{
	pthread_rwlock_unlock(rwlock);
}

static inline int
__pcapng_user_wrlock(pcapng_t *p)
{
	return __pcapng_lock_wrlock(&p->lock);
}

static inline int
__pcapng_user_rdlock(pcapng_t *p)
{
	return __pcapng_lock_rdlock(&p->lock);
}

static inline void
__pcapng_user_unlock(pcapng_t *p)
{
	return __pcapng_lock_unlock(&p->lock);
}

/** @internal
  * @brief Put a locked PCAP-NG structure.
  * @param p the PCAP-NG structure to lock.
  *
  * This is a straightforward unlocking of the structure.  Locking is performed
  * at the PCAP-NG structure level to protect operations on the internal PCAP
  * structures within.  This is largely because we have no other way of exerting
  * thread-safety control over the external libpcap library.
  */
static void
__pcapng_puthandle(void *arg)
{
	pcapng_t *p = (pcapng_t *) arg;

	__pcapng_user_unlock(p);
	return;
}

/** @internal
  * @brief Get a write-locked PCAP-NG structure.
  * @param p the PCAP-NG structure to lock.
  *
  * This is a straightforward locking of the structure.  Locking is performed at
  * the PCAP-NG structure level to protect operations on the internal PCAP
  * structures within.  This is largely because we have no other way of exerting
  * thread-safety control over the external libpcap library.
  *
  * __pcapng_puthandle() must be called to release the lock.
  */
static inline int
__pcapng_gethandle(pcapng_t *p)
{
	int err;

	if (unlikely((err = __pcapng_user_wrlock(p)))) {
		__pcapng_set_error(PCAP_ERROR);
		strncpy(p_errbuf, strerror(err));
		errno = err;
		return (-1);
	}
	return (0);
}

/** @internal
  * @brief Get a read-lock PCAP-NG structure.
  * @param p the PCAP-NG structure to lock.
  *
  * This is a straightforward locking of the structure.  Locking is performed at
  * the PCAP-NG structure level to protect operations on the internal PCAP
  * structures within.  This is largely because we have no other way of exerting
  * thread-safety control over the external libpcap library.
  *
  * __pcapng_puthandle() must be called to release the lock.
  */
static inline int
__pcapng_seehandle(pcapng_t *p)
{
	int err;

	if (unlikely((err = __pcapng_user_rdlock(p)))) {
		__pcapng_set_error(PCAP_ERROR);
		strncpy(p_errbuf, strerror(err));
		errno = err;
		return (-1);
	}
	return (0);
}

/** @brief Check if pcap structure is activated.
  */
int
__pcap_activated(pcap_t *p)
{
	return (p->activated);
}

/** @internal
  * @brief Get a handle on an interface.
  * @param p the PCAP-NG handle.
  * @param iid the interface number to get.
  * @param iface a pointer to a PCAP structure pointer to return the result.
  *
  * This function should be called with the #p structure locked with at least a
  * read lock.
  */
static int
__pcapng_getif(pcapng_t *p, int iid, struct pcapng_pcap **iface)
{
	if (0 > iid || iid > p->if_count) {
		__pcapng_set_error(PCAP_ERROR_NO_SUCH_INTERFACE);
		snprintf(p_errbuf, PCAP_ERRBUF_SIZE, "no such interface %d", iid);
		return (-1);
	}
	*iface = p->interfaces[iid];
	return 0;
}

static pcapng_pcap_t *
__pcapng_init_from_pcap(pcapng_t *p, pcap_t *pcap, char *device, char *ebuf)
{
	struct pcapng_pcap *pp, **interfaces;
	size_t size;

	size = (p->if_count + 1) * sizeof(pp);
	if ((interfaces = realloc(p->interfaces, size)) == NULL)
		goto bad;
	p->interfaces = interfaces;
	if ((pp = malloc(sizeof(*pp))) == NULL)
		goto error;
	memset(pp, 0, sizeof(*pp));
	pp->pcap = pcap;
	interfaces[p->if_count] = pp;
	p->if_count++;
	/* XXX: Might need to copy whatever information is in the pcap structure with accessor
	   methods into the pp->idbi structure.  Or, we might do this just before going to write
	   out the interface data block. */
	pp->idbi.idbi_linktype = pcap_datalink(pcap);
	pp->idbi.idbi_snaplen = pcap_snaplen(pcap);
	if (device != NULL)
		pp->idbi.idbi_name = strndup(device, 256);
	else if (pcap->opt.source != NULL)
		pp->idbi.idbi_name = strndup(pcap->opt.source, 256);
	else if (pcap->md.device != NULL)
		pp->idbi.idbi_name = strndup(pcap->md.device, 256);
	pp->idbi.idbi_desc = NULL;
	pp->idbi.idbi_ipv4addr = NULL;
	pp->idbi.idbi_ipv6addr = NULL;
	pp->idbi.idbi_macaddr = NULL;
	pp->idbi.idbi_euiaddr = NULL;
	pp->idbi.idbi_speed = 0;
	pp->idbi.idbi_tsresol = 0;	// usually 6;
	pp->idbi.idbi_tzone = pcap->tzoff;	// ???
	pp->idbi.idbi_filter.var_length = 0;	// can be bpf or expression
	pp->idbi.idbi_filter.var_value = NULL;	// can be bpf or expression
	pp->idbi.idbi_os = NULL;	// should be same as shb for live capture
	pp->idbi.idbi_fcslen = 0;
	pp->idbi.idbi_tsoffset[0] = 0;
	pp->idbi.idbi_tsoffset[1] = 0;
	pp->idbi.idbi_physaddr.var_length = 0;
	pp->idbi.idbi_physaddr.var_value = NULL;
	pp->idbi.idbi_uuid = NULL;	// should be same as shb for live capture
	if (pcap->md.ifindex > 0)
		pp->idbi.idbi_ifindex = pcap->md.ifindex;
	return (pp);
      error:
	snprintf(ebuf, PCAP_ERRBUF_SIZE, "cannot create pcap: %s", pcap_strerror(errno));
	goto bad;
      bad:
	pcap_close(pcap);
	free(pcap);
	return (NULL);
}

/** @brief Test whether an interface is activated.
  * @param p PCAP-NG capture handle.
  * @param iid interface identifier (index) to check.
  *
  * pacpng_activated() tests whether the interface, iid, is activated for the
  * PCAP-NG capture handle specified by p.
  */
int
__pcapng_activated(pcapng_t *p, int iid)
{
	struct pcapng_pcap *pp;
	int err;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return __pcap_activated(pp->pcap);
}

int
__pcapng_activated_r(pcapng_t *p, int iid)
{
	int ret;

	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_seehandle(p)) == 0) {
		ret = __pcapng_activated(p, iid);
		__pcapng_puthandle(p);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_activated_r,pcapng_activated@@PCAPNG_1.0");

/** @brief Find the number of interfaces for a PCAP-NG handle.
  * @param p the PCAP-NG handle to query.
  *
  * pcapng_interfaces() return the number of interfaces that are defined for the
  * PCAP-NG handle.  When no interfaces are defined, pcapng_interfaces() returns
  * zero (0).
  *
  * This function always succeeds.  Results are indeterminate if the function is
  * passed an invalid pointer.
  */
int
__pcapng_interfaces(pcapng_t *p)
{
	return p->if_count;
}

__asm__(".symver __pcapng_interfaces,pcapng_interfaces@@PCAPNG_1.0");

/** @brief Activate an interface of a PCAP-NG handle.
  * @param p the PCAP-NG handle.
  * @param iid the interface id to activate.
  *
  * This function is not a thread cancellation point.
  *
  * pcapng_activate() is used to activate an interface of a next-generation
  * packet capture handle to look at packets on the network, with the options
  * that were set on the interface being in effect.
  *
  * pcapng_activate() returns any of the values returned by pcap_activate().
  * In addition, pcapng_activate() returns PCAP_ERROR_NO_SUCH_INTERFACE when
  * the interface id is out of range.
  */
int
__pcapng_activate(pcapng_t *p, int iid)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	if ((err = pcap_activate(pp->pcap)) != 0) {
		__pcapng_set_error(err);
		strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
		err = -1;
	}
	return (err);
}

/** Thread-safe version of __pcapng_activate(). */
int
__pcapng_activate_r(pcapng_t *p, int iid)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_activate(p, iid);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_activate_r,pcapng_activate@@PCAPNG_1.0");

int
__pcapng_activate_all(pcapng_t *p, int *iidp)
{
	int err = 0, warn = 0, iid;
	struct pcapng_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			return (err);
		if (pp == NULL)
			continue;
		if ((err = pcap_activate(pp->pcap)) != 0) {
			if (err == PCAP_ERROR_ACTIVATED)
				continue;
			__pcapng_set_error(err);
			strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
			if (err < 0)
				return (-1);
			warn = 1;
		}
	}
	return (warn);
}

int
__pcapng_activate_all_r(pcapng_t *p)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_activate_all(p);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_activate_all_r,pcapng_activate_all@@PCAPNG_1.0");

/** @brief Force a pcapng_dispatch() or pcapng_loop() call to return.
  * @param p the PCAP-NG handle.
  *
  * pcapng_breakloop() sets a flag that will force pcapng_dispatch() or
  * pcapng_loop() to return rather than looping; they will return the number
  * of packets that have been processed so far, or -2 if no packets have been
  * processed so far.
  */
void
__pcapng_breakloop(pcapng_t *p)
{
	p->break_loop = 1;
}

__asm__(".symver __pcapng_breakloop,pcapng_breakloop@@PCAPNG_1.0");

/** @brief Check whether monitor mode can be set for an interface.
  * @param p the PCAP-NG handle.
  * @param iid the interface id to check.
  *
  * This function is NOT a thread cancellation point.
  *
  * pcapng_can_set_rfmon() checks wheter monitor mode could be set on an
  * interface when the interface is activated.
  *
  * pcapng_can_set_rfmon() returns any of the values returned by
  * pcap_can_set_rfmon().  It can also return PCAP_ERROR_NO_SUCH_INTERFACE
  * when the interface id is out of range.
  */
int
__pcapng_can_set_rfmon(pcapng_t *p, int iid)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp))) {
		return (err);
	}
	if ((err = pcap_can_set_rfmon(pp->pcap)) < 0) {
		__pcapng_set_error(err);
		strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
		return (-1);
	}
	return (err);
}

/** Thread-safe version of __pcapng_can_set_rfmon(). */
int
__pcapng_can_set_rfmon_r(pcapng_t *p, int iid)
{
	int ret;

	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_can_set_rfmon(p, iid);
		__pcapng_puthandle(p);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_can_set_rfmon_r,pcapng_can_set_rfmon@@PCAPNG_1.0");

/** @brief Close a PCAP-NG capture or savefile.
  * @param p the PCAP-NG handle.
  *
  * This function IS a thread cancellation point.
  *
  * pcapng_close() closes the files associated with p and deallocates
  * resources.
  */
void
__pcapng_close(pcapng_t *p)
{
	int iid, num = __pcapng_interfaces(p);

	if (num > 0) {
		for (iid = num - 1, iid >= 0; iid--) {
			struct pcapng_pcap *pp;

			if (__pcapng_getif(p, iid, &pp) == 0) {
				if (pp != NULL) {
					p->interfaces[iid] = NULL;
					pcap_close(pp->pcap);
					free(pp->pcap);
					free(pp);
				}
				p->if_count--;
			}
		}
	}
	p->if_count = 0;
	if (p->interfaces != NULL) {
		free(p->interfaces);
		p->interfaces = NULL;
	}
	if (p->buffer != NULL) {
		free(p->buffer);
		p->buffer = NULL;
	}
}

/** Thread-safe version of __pcapng_close(). */
int
__pcapng_close_r(pcapng_t *p)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_close(p);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(&oldstate, NULL);
	if (ret != 0)
		pthread_testcancel();
	return (ret);
}

__asm__(".symver __pcapng_close_r,pcapng_close@@PCAPNG_1.0");

/** @brief Compile a filter expression.
  * @param p the PCAP-NG handle.
  * @param iid the interface id against which to compile.
  * @param fp pointer to struct bpf_program to contain compiled program.
  * @param str the filter expression string to compile.
  * @param optimize optimize the filter program when true.
  * @param netmask an IPv4 netmask to detect broadcast or PCAP_NETMASK_UNKNOWN.
  *
  * This function is NOT a thread cancellation point.
  *
  * pcapng_compile() is used to compile the string str into a filter program.
  * fp is a pointer to a struct bpf_program and is filled in by
  * pcap_compile().
  *
  * pcapng_compile() returns any of the values returned by pcap_compile().
  * It can also return PCAP_NO_SUCH_INTERFACE when iid is out of range.
  */
int
__pcapng_compile(pcapng_t *p, int iid, struct bpf_program *fp, const char
		 *str, int optimize, bpf_u_int32 netmask)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)) != 0) {
		return err;
	}
	return pcap_compile(pp->pcap, fp, str, optimize, netmask);
}

/** Thread-safe version of __pcapng_compile(). */
int
__pcapng_compile_r(pcapng_t *p, int iid, struct bpf_program *fp, const char
		   *str, int optimize, bpf_u_int32 netmask)
{
	int ret;

	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_compile(p, iid, fp, str, optimize, netmask);
		__pcapng_puthandle(p);
	}
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_compile_r,pcapng_compile@@PCAPNG_1.0");

/** @brief Create a live capture handle.
  * @param source the network device or savefile to open.
  * @param ebuf error buffer.
  *
  * This function IS a thread cancellation point.
  *
  * pcapng_create() is used to create a packet capture handle to look at
  * packets on the network.  source is a string that specifies the network
  * device to open;  on Linux an argument of "any" can be used to capture
  * packets on all interfaces (using DLT_LINUX_SLL); however, a NULL source
  * argument will not begin capture on any interface.  A source argument of
  * "all" can be used to capture on all interfaces (an interface is defined
  * for each and every interface that can be found with pcap_findalldevs()).
  *
  * Each interface of the returned handle must be activated with
  * pcapng_activate() or pcapng_activate_all() before packets can be captured
  * with it; options for the interface, such as promiscuous mode, can be set
  * on each interface before activating it.
  *
  * This function can return any of the values returned by pcap_create().
  */
pcapng_t *
__pcapng_create(const char *source, char *ebuf)
{
	pcapng_t *p;
	struct utsname *up;

	if ((p = malloc(sizeof(*p))) == NULL)
		return (p);
	memset(p, 0, sizeof(*p));
	p->shbi.shbi_major = 1;
	p->shbi.shbi_minor = 0;
	p->shbi.shbi_section_length = 0xffffffffffffffff;
	p->shbi.shbi_comment = NULL;
	if (uname(up) == 0) {
		p->shbi.shbi_hardware = strndup(up->machine, 256);
		if ((p->shbi.shbi_os = malloc(257)) != NULL)
			snprintf(p->shbi.shbi_os, 256, "%s %s %s",
				 up->sysname, up->release, up->version);
	}
	p->shbi.shbi_userappl = strndup("libpcapng 1.0", 256);
	p->shbi.shbi_uuid = NULL;
	if (source == NULL)
		source = "all";
	if (source != NULL) {
		pcapng_pcap_t *pp;
		pcap_t *pcap;
		size_t size;

		if (strcmp(source, "any") == 0) {
			pcap_if_t *ifp, *devs;

			if (pcap_findalldevs(&devs, ebuf) != 0)
				goto bad;
			for (ifp = devs; ifp != NULL; ifp = ifp->next) {
				if (strcmp(ifp->name, "all") == 0)
					continue;
				if ((pcap = pcap_create(ifp->name, ebuf)) == NULL)
					continue;
				if ((pp = __pcapng_init_from_pcap(p, pcap, ifp->name, ebuf)) == NULL) {
					pcap_freealldevs(devs);
					goto bad;
				}
			}
			if (devs != NULL)
				pcap_freealldevs(devs);
		} else {
			if ((pcap = pcap_create(source, ebuf)) == NULL)
				goto bad;
			if ((pp = __pcapng_init_from_pcap(p, pcap, source, ebuf)) == NULL)
				goto bad;
		}
	}
	return (p);
      bad:
	if (p != NULL) {
		__pcapng_close(p);
		free(p);
		p = NULL;
	}
	return (p);
}

/** Thread-safe version of __pcapng_create(). */
pcapng_t *
__pcapng_create_r(const char *source, char *ebuf)
{
	pcapng_t *p;
	int oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	p = __pcapng_create(source, ebuf);
	pthread_setcancelstate(oldstate, NULL);
	if (p == NULL)
		pthread_testcancel();
	return (p);
}

__asm__(".symver __pcapng_create,pcapng_create@@PCAPNG_1.0");

/** @brief Get the link-layer header type for an interface.
  * @param p the PCAP-NG handle.
  * @param iid the interface id.
  *
  * This function is NOT a cancellation point.
  *
  * pcapng_datalink() returns the link-layer header type for the interface on
  * p specified by iid.
  */
int
__pcapng_datalink(pcapng_t *p, int iid)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp))) {
		return err;
	}
	return pcap_datalink(pp->pcap);
}

/** Thread-safe version of __pcapng_datalink(). */
int
__pcapng_datalink_r(pcapng_t *p, int iid)
{
	int ret;

	// unfortunately the darn thing might invoke an IOCTL
	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_datalink(p, iid);
		__pcapng_puthandle(p);
	} else
		ret = 0;
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_datalink_r,pcapng_datalink@@PCAPNG_1.0");

/** @brief Get the extended link-layer header info for an interface.
  * @param p the PCAP-NG handle.
  * @param iid the interface id.
  *
  * This function is NOT a cancellation point.
  *
  * pcapng_datalink() returns the extended link-layer header info for the
  * interface on p specified by iid.
  */
int
__pcapng_datalink_ext(pcapng_t *p, int iid)
{
	int err;
	pcap_t *pcap;

	if ((err = __pcapng_get(p, iid, &pcap))) {
		return err;
	}
	return pcap_datalink_ext(pcap);
}

/** Thread-safe version of __pcapng_datalink_ext(). */
int
__pcapng_datalink_ext_r(pcapng_t *p, int iid)
{
	int ret;

	// unfortunately the darn thing might invoke an IOCTL
	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_datalink_ext(p, iid);
		__pcapng_puthandle(p);
	} else
		ret = 0;
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_datalink_ext_r,pcapng_datalink_ext@@PCAPNG_1.0");

/** @brief
  * @param p the PCAP-NG handle.
  * @param cnt number of packets to retreive.
  * @param callback callback to perform on packets.
  * @param user transparent user data.
  *
  * This function IS a cancellation point.
  */
int
__pcapng_dispatch(pcapng_t *p, int cnt, pcapng_handler callback, u_char *user)
{
}

/** Thread-safe version of __pcapng_dispatch(). */
int
__pcapng_dispatch_r(pcapng_t *p, int cnt, pcapng_handler callback, u_char *user)
{
	int ret;

	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		if ((ret = __pcapng_dispatch(p, cnt, callback, user)) != 0)
			pthread_testcancel();
		__pcapng_puthandle(p);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_dispatch_r,pcapng_dispatch@@PCAPNG_1.0");

/** @brief
  * @param user transparent user data.
  * @param h PCAP-NG packet header.
  * @param sp packet data.
  *
  * This function IS a cancellation point.
  */
int
__pcapng_dump(void *user, const struct pcapng_pkthdr *h, const u_char *sp)
{
	return __pcapng_write_epb(user, h, sp);
}

/** Thread-safe version of __pcapng_dump(). */
int
__pcapng_dump_r(void *user, const struct pcapng_pkthdr *h, const u_char *sp)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	ret = __pcapng_dump(user, h, sp);
	pthread_setcanceltype(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_dump_r,pcapng_dump@@PCAPNG_1.0");

/** @brief Close a savefile being written.
  * @param p the PCAP-NG dumper handle.
  *
  * pcapng_dump_close() closes the PCAP-NG save file being written.
  */
void
__pcapng_dump_close(pcapng_dumper_t *p)
{
	(void) fclose((FILE *) p);
}

__asm__(".symver __pcapng_dump_close,pcapng_dump_close@@PCAPNG_1.0");

FILE *
__pcapng_dump_file(pcapng_dumper_t *p)
{
	return ((FILE *) p);
}

__asm__(".symver __pcapng_dump_file,pcapng_dump_file@@PCAPNG_1.0");

int
__pcapng_dump_flush(pcapng_dumper_t *p)
{
	if (fflush((FILE *) p) == EOF)
		return (-1);
	return (0);
}

__asm__(".symver __pcapng_dump_flush,pcapng_dump_flush@@PCAPNG_1.0");

pcapng_dumper_t *
__pcapng_dump_fopen(pcapng_t *p, FILE *f)
{
	if (__pcapng_write_shb(f, &p->shbi) != 0)
		return (NULL);
	return ((pcapng_dumper_t *) f);
}

__asm__(".symver __pcapng_dump_fopen,pcapng_dump_fopen@@PCAPNG_1.0");

int
__pcapng_dump_ftell(pcapng_dumper_t *p)
{
	return (ftell((FILE *) p));
}

__asm__(".symver __pcapng_dump_ftell,pcapng_dump_ftell@@PCAPNG_1.0");

static int
__pcapng_fread(FILE *f, const char *buf, size_t size, char *ebuf)
{
	size_t read = 0;

	errno = 0;
	while ((read += fread(buf + read, 1, size - read, f)) < (size - read)) {
		if (ferror(f)) {
			int err;

			switch ((err = errno)) {
			case EINTR:
			case EAGAIN:
				continue;
			}
			snprintf(ebuf, PCAP_ERRBUF_SIZE, "error reading save file: %s",
				 pcap_strerror(err));
			errno = err;
		} else if (feof(f)) {
			snprintf(ebuf, PCAP_ERRBUF_SIZE, "unexpected end of save file");
			errno = EFAULT;
		} else {
			snprintf(ebuf, PCAP_ERRBUF_SIZE, "short read %zu instead of %zu",
				 read, size);
			errno = EFAULT;
		}
		return (-1);
	}
	return (0);
}

static int
__pcapng_fwrite(FILE *f, const char *buf, size_t size)
{
	size_t wrote = 0;
	int err, fd;

	if ((fd = fileno(f)) == -1)
		goto errno;
	if (lockf(fd, F_LOCK, 0) == -1)
		goto errno;
	errno = 0;
	while ((wrote += fwrite(buf + wrote, size - wrote, 1, f)) < (size - wrote)) {
		if (errno != 0) {
			switch ((err = errno)) {
			case EINTR:
			case EAGAIN:
				errno = 0;
				continue;
			}
		} else
			err = EFAULT;
		goto unlock;
	}
	err = 0;
      unlock:
	lockf(fd, F_ULOCK, 0);
	if (err != 0)
		goto error;
	return (0);
      errno:
	err = errno;
      error:
	p_errno = PCAP_ERROR;
	strncpy(p_errbuf, strerror(err), PCAP_ERRBUF_SIZE);
	errno = err;
	return (-1);
}

static int
__pcapng_size_opt(const char *buf, int len)
{
	if (buf != NULL) {
		len = (len + 3) & ~0x3;
		return (4 + len);
	}
	return (0);
}

static int
__pcapng_size_opt_64(uint64_t val)
{
	if (val != 0)
		return (4 + 8);
	return (0);
}

static int
__pcapng_size_opt_32(uint32_t val)
{
	if (val != 0)
		return (4 + 4);
	return (0);
}

static int
__pcapng_size_opt_16(uint16_t val)
{
	if (val != 0)
		return (4 + 4);
	return (0);
}

static int
__pcapng_size_opt_8(uint8_t val)
{
	if (val != 0)
		return (4 + 4);
	return (0);
}

static int
__pcapng_size_opt_str(const char *str)
{
	int size = 0;

	if (str != NULL) {
		int len;

		if ((len = strnlen(str, PCAPNG_MAXSTR)) > 0) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	return (size);
}

static int
__pcapng_size_opt_var(const struct var *var)
{
	if (var != NULL && var->var_value != NULL) {
		int len;

		if ((len = var->var_length) > 0) {
			len = (len + 3) & ~0x3;
			return (4 + len);
		}
	}
	return (0);
}

static int
__pcapng_size_opt_ts(const uint32_t *ts)
{
	if (ts != NULL && (ts[0] != 0 || ts[1] != 0))
		return (4 + 8);
	return (0);
}

static int
__pcapng_size_data(const struct var *var)
{
	if (var != NULL && var->var_value != NULL) {
		int len;

		if ((len = var->var_length) > 0) {
			len = (len + 3) & ~0x3;
			return (len);	/* no option header */
		}
	}
	return (0);
}

static int
__pcapng_size_datalen(int len)
{
	if (len > 0) {
		len = (len + 3) & ~0x3;
		return (len);	/* no option header */
	}
	return (0);
}

static void
__pcapng_write_opt(struct opt **optp, int code, const char *buf, int len)
{
	if (buf != NULL) {
		(*optp)->opt_code = code;
		(*optp)->opt_len = len;
		memcpy((*optp)->opt_value, buf, len);
		len = (4 + (len + 3)) >> 2;
		(*optp) += len;
	}
}

static void
__pcapng_write_opt_str(struct opt **optp, int code, const char *str)
{
	if (str != NULL) {
		int len;

		if ((len = strnlen(str, PCAPNG_MAXSTR)) > 0)
			__pcapng_write_opt(optp, code, str, len);
	}
}

static void
__pcapng_write_opt_var(struct opt **optp, int code, const struct var *var)
{
	if (var != NULL && var->var_value != NULL) {
		int len;

		if ((len = var->var_length) > 0)
			__pcapng_write_opt(optp, code, var->var_value, len);
	}
}

static void
__pcapng_write_opt_64(struct opt **optp, int code, uint64_t val)
{
	if (val != 0)
		__pcapng_write_opt(optp, code, (char *) &val, 8);
}

static void
__pcapng_write_opt_32(struct opt **optp, int code, uint32_t val)
{
	if (val != 0)
		__pcapng_write_opt(optp, code, (char *) &val, 4);
}

static void
__pcapng_write_opt_16(struct opt **optp, int code, uint16_t val)
{
	if (val != 0)
		__pcapng_write_opt(optp, code, (char *) &val, 2);
}

static void
__pcapng_write_opt_8(struct opt **optp, int code, uint8_t val)
{
	if (val != 0)
		__pcapng_write_opt(optp, code, (char *) &val, 1);
}

static void
__pcapng_write_opt_ts(struct opt **optp, int code, const uint32_t *ts)
{
	if (ts != NULL && (ts[0] != 0 || ts[1] != 0))
		__pcapng_write_opt(optp, code, (char *) ts, 8);
}

static void
__pcapng_write_data(uint32_t **datp, const struct var *var)
{
	if (var != NULL && var->var_value != NULL) {
		int len;

		if ((len = var->var_length) > 0) {
			memcpy((*datp), var->var_value, len);
			len = (len + 3) >> 2;
			(*datp) += len;
		}
	}
}

static void
__pcapng_write_datalen(uint32_t **datp, char *buf, int len)
{
	if (data != NULL && len > 0) {
		memcpy((*datp), buf, len);
		len = (len + 3) >> 2;
		(*datp) += len;
	}
}

static size_t
__pcapng_size_shb(struct shb_info *shbi)
{
	size_t size = 0;

	size += 8;		/* header */
	size += 4;		/* magic */
	size += 2;		/* major */
	size += 2;		/* minor */
	size += 8;		/* section length */
	size += __pcapng_size_opt_str(shbi->shbi_comment);
	size += __pcapng_size_opt_str(shbi->shbi_hardware);
	size += __pcapng_size_opt_str(shbi->shbi_os);
	size += __pcapng_size_opt_str(shbi->shbi_userappl);
	size += __pcapng_size_opt(shbi->shbi_uuid, 16);
	size += 4;		/* end of options */
	size += 4;		/* trailer */
	return size;
}

static int
__pcapng_write_shb(FILE *f, struct shb_info *shbi)
{
	size_t size;
	char *buf;
	struct shb *shb;
	struct opt *opt;

	size = __pcapng_size_shb(shbi);
	buf = alloca(size);
	shb = (struct shb *) buf;
	shb->shb_type = PCAPNG_SHB_TYPE;
	shb->shb_length = size;
	shb->shb_magic = PCAPNG_MAGIC;
	shb->shb_major = 1;
	shb->shb_minor = 0;
	shb->shb_section_length = 0xffffffffffffffff;
	opt = shb->shb_options;
	__pcapng_write_opt_str(&opt, OPT_COMMENT, shbi->shbi_comment);
	__pcapng_write_opt_str(&opt, SHB_HARDWARE, shbi->shbi_hardware);
	__pcapng_write_opt_str(&opt, SHB_OS, shbi->shbi_os);
	__pcapng_write_opt_str(&opt, SHB_USERAPPL, shbi->shbi_userappl);h
	__pcapng_write_opt(&opt, SHB_UUID, shbi->shbi_uuid, 16);
	opt->opt_code = 0;
	opt->opt_len = 0;
	opt += 1;
	*(uint32_t *) opt = size;
	return __pcapng_fwrite(f, buf, size);
}

static size_t
__pcapng_size_idb(struct idb_info *idbi)
{
	size_t size = 0;

	size += 8;		// header
	size += 2;		// linktype
	size += 2;		// reserved
	size += 4;		// snaplen
	size += __pcapng_size_opt_str(idbi->idbi_comment);
	size += __pcapng_size_opt_str(idbi->idbi_name);
	size += __pcapng_size_opt_str(idbi->idbi_desc);
	size += __pcapng_size_opt(idbi->idbi_ipv4addr, 8);
	size += __pcapng_size_opt(idbi->idbi_ipv6addr, 17);
	size += __pcapng_size_opt(idbi->idbi_macaddr, 6);
	size += __pcapng_size_opt(idbi->idbi_euiaddr, 8);
	size += __pcapng_size_opt_64(idbi->idbi_speed);
	size += __pcapng_size_opt_8(idbi->idbi_tsresol);
	size += __pcapng_size_opt_32(idbi->idbi_tzone);
	size += __pcapng_size_opt_var(&idbi->idbi_filter);
	size += __pcapng_size_opt_str(idbi->idbi_os);
	size += __pcapng_size_opt_8(idbi->idbi_fcslen);
	size += __pcapng_size_opt_ts(idbi->idbi_tsoffset);
	size += __pcapng_size_opt_var(&idbi->idbi_physaddr);
	size += __pcapng_size_opt(idbi->idbi_uuid, 16);
	size += __pcapng_size_opt_32(idbi->idbi_ifindex);
	size += 4;		// end of options
	size += 4;		// trailer
	return size;
}

static int
__pcapng_write_idb(FILE *f, struct idb_info *idbi)
{
	size_t size;
	char *buf;
	struct idb *idb;
	struct opt *opt;

	size = __pcapng_size_idb(idbi);
	buf = alloca(size);
	idb = (struct idb *) buf;
	idb->idb_type = PCAPNG_IDB_TYPE;
	idb->idb_length = size;
	idb->idb_linktype = idbi->idbi_linktype;
	idb->idb_reserved = 0;
	idb->idb_snaplen = idbi->idbi_snaplen;
	opt = idb->idb_options;
	__pcapng_write_opt_str(&opt, OPT_COMMENT, idbi->idbi_comment);
	__pcapng_write_opt_str(&opt, IDB_NAME, idbi->idbi_name);
	__pcapng_write_opt_str(&opt, IDB_DESCRIPTION, idbi->idbi_desc);
	__pcapng_write_opt(&opt, IDB_IPV4ADDR, idbi->idbi_ipv4addr, 8);
	__pcapng_write_opt(&opt, IDB_IPV6ADDR, idbi->idbi_ipv6addr, 17);
	__pcapng_write_opt(&opt, IDB_MACADDR, idbi->idbi_macaddr, 6);
	__pcapng_write_opt(&opt, IDB_EUIADDR, idbi->idbi_euiaddr, 8);
	__pcapng_write_opt_64(&opt, IDB_SPEED, idbi->idbi_speed);
	__pcapng_write_opt_8(&opt, IDB_TSRESOL, idbi->idbi_tsresol);
	__pcapng_write_opt_32(&opt, IDB_TZONE, idbi->idbi_tzone);
	__pcapng_write_opt_var(&opt, IDB_FILTER, &idbi->idbi_filter);
	__pcapng_write_opt_str(&opt, IDB_OS, idbi->idbi_os);
	__pcapng_write_opt_8(&opt, IDB_FCSLEN, idbi->idbi_fcslen);
	__pcapng_write_opt_ts(&opt, IDB_TSOFFSET, idbi->idbi_tsoffset);
	__pcapng_write_opt_var(&opt, IDB_PHYSADDR, &idbi->idbi_physaddr);
	__pcapng_write_opt(&opt, IDB_UUID, idbi->idbi_uuid, 16);
	__pcapng_write_opt_32(&opt, IDB_IFINDEX, idbi->idbi_ifindex);
	opt->opt_code = 0;
	opt->opt_len = 0;
	opt += 1;
	*(uint32_t *) opt = size;
	return __pcapng_fwrite(f, buf, size);
}

static size_t
__pcapng_size_epb(struct pcapng_pkthdr *pph)
{
	size_t size = 0;

	size += 8;		/* header */
	size += 4;		/* interface id */
	size += 8;		/* timestamp */
	size += 4;		/* capture length */
	size += 4;		/* packet length */
	size += __pcapng_size_datalen(pph->pph_datalen);
//      size += __pcapng_size_opt_str(pph->pph_comment);
	size += __pcapng_size_opt_32(pph->pph_flags);
//      size += __pcapng_size_opt_var(&pph->pph_hash);
	size += __pcapng_size_opt_64(pph->pph_dropcount);
	size += __pcapng_size_opt_64(pph->pph_compcount);
	size += 4;		/* end of options */
	size += 4;		/* trailer */
	return size;
}

static int
__pcapng_write_epb(FILE *f, struct pcapng_pkthdr *pph, const char *data)
{
	size_t size;
	char *buf;
	struct epb *epb;
	uint32_t *dat;
	struct opt *opt;

	size = __pcapng_size_epb(pph);
	buf = alloca(size);
	epb = (struct epb *) buf;
	epb->epb_type = PCAPNG_EPB_TYPE;
	epb->epb_length = size;
	epb->epb_interfaceid = pph->pph_interfaceid;
	epb->epb_ts[0] = pph->pph_ts[0];
	epb->epb_ts[1] = pph->pph_ts[1];
	epb->epb_captlen = pph->pph_captlen;
	epb->epb_packlen = pph->pph_packlen;
	dat = epb->epb_data;
	__pcapng_write_datalen(&dat, data, pph->pph_datalen);
	opt = (struct opt *) dat;
//      __pcapng_write_opt_str(&opt, OPT_COMMENT, pph->pph_comment);
	__pcapng_write_opt_32(&opt, EPB_FLAGS, pph->pph_flags);
//      __pcapng_write_opt_var(&opt, EPB_HASH, &pph->pph_hash);
	__pcapng_write_opt_64(&opt, EPB_DROPCOUNT, pph->pph_dropcount);
	__pcapng_write_opt_64(&opt, EPB_COMPCOUNT, pph->pph_compcount);
	opt->opt_code = 0;
	opt->opt_len = 0;
	opt += 1;
	*(uint32_t *) opt = size;
	return __pcapng_fwrite(f, buf, size);
}

static size_t
__pcapng_size_spb(struct pcapng_pkthdr *pph)
{
	size_t size = 0;

	size += 8;		/* header */
	size += 4;		/* packet length */
	size += __pcapng_size_datalen(pph->pph_datalen);
	size += 4;		/* trailer */
	return size;
}

static int
__pcapng_write_spb(FILE *f, struct pcapng_pkthdr *pph, char *data)
{
	size_t size;
	char *buf;
	struct spb *spb;
	uint32_t *dat;

	size = __pcapng_size_spb(pph);
	buf = alloca(size);
	spb = (struct spb *) buf;
	spb->spb_type = PCAPNG_SPB_TYPE;
	spb->spb_length = size;
	spb->spb_packlen = pph->pph_packlen;
	dat = spb->spb_data;
	__pcapng_write_datalen(&dat, data, pph->pph_datalen);
	*(uint32_t *) dat = size;
	return __pcapng_fwrite(f, buf, size);
}

static size_t
__pcapng_size_opb(struct pcapng_pkthdr *pph)
{
	size_t size = 0;

	size += 8;		/* header */
	size += 2;		/* interface id */
	size += 2;		/* drops */
	size += 8;		/* timestamp */
	size += 4;		/* capture length */
	size += 4;		/* packet length */
	size += __pcapng_size_datalen(pph->pph_datalen);
//      size += __pcapng_size_opt_str(pph->pph_comment);
	size += __pcapng_size_opt_32(pph->pph_flags);
//      size += __pcapng_size_opt_var(&pph->pph_hash);
	size += 4;		/* end of options */
	size += 4;		/* trailer */
	return size;
}

static int
__pcapng_write_opb(FILE *f, struct pcapng_pkthdr *pph, char *data)
{
	size_t size;
	char *buf;
	struct opb *opb;
	uint32_t *dat;
	struct opt *opt;
	int err;

	size = __pcapng_size_opb(pph);
	buf = alloca(size);
	opb = (struct opb *) buf;
	opb->opb_type = PCAPNG_OPB_TYPE;
	opb->opb_length = size;
	opb->opb_interfaceid = pph->pph_interfaceid;
	opb->opb_drops = pph->pph_dropcount;
	opb->opb_ts[0] = pph->pph_ts[0];
	opb->opb_ts[1] = pph->pph_ts[1];
	opb->opb_captlen = pph->pph_captlen;
	opb->opb_packlen = pph->pph_packlen;
	dat = opb->opb_data;
	__pcapng_write_datalen(&dat, data, pph->pph_datalen);
	opt = (struct opt *) dat;
//      __pcapng_write_opt_str(&opt, OPT_COMMENT, pph->pph_comment);
	__pcapng_write_opt_32(&opt, EPB_FLAGS, pph->pph_flags);
//      __pcapng_write_opt_var(&opt, EPB_HASH, &pph->pph_hash);
	opt->opt_code = 0;
	opt->opt_len = 0;
	opt += 1;
	*(uint32_t *) opt = size;
	return __pcapng_fwrite(f, buf, size);
}

static int
__pcapng_detect_pb_type(struct pcapng_pkthdr *pph, struct idb_info *idbi, int if_count)
{
	if (pph->pph_compcount)
		return (PCAPNG_EPB_TYPE);
	if (pph->pph_dropcount) {
		if (pph->pph_dropcount > 0x0ffff)
			return (PCAPNG_EPB_TYPE);
		return (PCAPNG_OPB_TYPE);
	}
	if (if_count > 1)
		return (PCAPNG_OPB_TYPE);
	if (pph->pph_interfaceid)
		return (PCAPNG_OPB_TYPE);
	if (pph->pph_ts[0])
		return (PCAPNG_OPB_TYPE);
	if (pph->pph_ts[1])
		return (PCAPNG_OPB_TYPE);
	if (pph->pph_captlen && pph->pph_captlen < pph->pph_packlen &&
	    pph->pph_captlen != idbi->idbi_snaplen)
		return (PCAPNG_OPB_TYPE);
	if (pph->pph_datalen && pph->pph_datalen < pph->pph_packlen &&
	    pph->pph_datalen != idbi->idbi_snaplen)
		return (PCAPNG_OPB_TYPE);
	if (pph->pph_flags)
		return (PCAPNG_OPB_TYPE);
	return (PCAPNG_SPB_TYPE);
}

static size_t
__pcapng_size_isb(struct isb_info *isbi)
{
	size_t size = 0;

	size += 8;		/* header */
	size += 4;		/* interface id */
	size += 8;		/* timestamp */
	size += __pcapng_size_opt_ts(&isbi->isbi_starttime);
	size += __pcapng_size_opt_ts(&isbi->isbi_endtime);
	size += __pcapng_size_opt_64(isbi->isbi_ifrecv);
	size += __pcapng_size_opt_64(isbi->isbi_ifdrop);
	size += __pcapng_size_opt_64(isbi->isbi_filteraccept);
	size += __pcapng_size_opt_64(isbi->isbi_osdrop);
	size += __pcapng_size_opt_64(isbi->isbi_usrdeliv);
	size += __pcapng_size_opt_64(isbi->isbi_ifcomp);
	size += 4;		/* end of options */
	size += 4;		/* trailer */
	return size;
}

static int
__pcapng_write_isb(FILE *f, struct isb_info *isbi)
{
	size_t size;
	char *buf;
	struct isb *isb;
	struct opt *opt;

	size = __pcapng_size_isb(isbi);
	buf = alloca(size);
	isb = (struct isb *) buf;
	isb->isb_type = PCAPNG_ISB_TYPE;
	isb->isb_length = size;
	isb->isb_interfaceid = isbi->isbi_interfaceid;
	isb->isb_ts[0] = isbi->isbi_ts[0];
	isb->isb_ts[1] = isbi->isbi_ts[1];
	opt = isb->isb_options;
	__pcapng_write_opt_str(&opt, OPT_COMMENT, isbi->isbi_comment);
	__pcapng_write_opt_ts(&opt, ISB_STARTTIME, isbi->isbi_starttime);
	__pcapng_write_opt_ts(&opt, ISB_ENDTIME, isbi->isbi_endtime);
	__pcapng_write_opt_64(&opt, ISB_IFRECV, isbi->isbi_ifrecv);
	__pcapng_write_opt_64(&opt, ISB_IFDROP, isbi->isbi_ifdrop);
	__pcapng_write_opt_64(&opt, ISB_FILTERACCEPT, isbi->isbi_filteraccept);
	__pcapng_write_opt_64(&opt, ISB_OSDROP, isbi->isbi_osdrop);
	__pcapng_write_opt_64(&opt, ISB_USRDELIV, isbi->isbi_usrdeliv);
	__pcapng_write_opt_64(&opt, ISB_IFCOMP, isbi->isbi_ifcomp);
	opt->opt_code = 0;
	opt->opt_len = 0;
	opt += 1;
	*(uint32_t *) opt = size;
	return __pcapng_fwrite(f, buf, size);
}

pcapng_dumper_t *
__pcapng_dump_open(pcapng_t *p, const char *fname)
{
	pcapng_dumper_t *d;

	if (strcmp(fname, "-") == 0 || strcmp(fname, "/dev/stdout") == 0) {
		d = __pcapng_dump_fopen(p, stdout);
	} else {
		FILE *f;

		f = fopen(fname, "w");
		if (f == NULL) {
			sprintf(p->errbuf, sizeof(p->errbuf), "%s: %s", fname,
				pcap_strerror(errno));
			return (NULL);
		}
		if ((d = __pcapng_dump_fopen(p, f)) == NULL)
			fclose(f);
	}
	return (d);
}

__asm__(".symver __pcapng_dump_open,pcapng_dump_open@@PCAPNG_1.0");

FILE *
__pcapng_file(pcapng_t *p)
{
	return (p->file);
}

__asm__(".symver __pcapng_file,pcapng_file@@PCAPNG_1.0");

int
__pcapng_fileno(pcapng_t *p)
{
	return (p->fd);
}

__asm__(".symver __pcapng_fileno,pcapng_fileno@@PCAPNG_1.0");

pcapng_t *
__pcapng_fopen_offline(FILE *f, char *ebuf)
{
	pcapng_t *p = NULL;
	uint32_t *words;
	uint64_t *lword;
	uint16_t major, minor;
	uint64_t seclen;
	char *buffer, *ptr, *end;
	int swapped;
	size_t blen, header = 24;

	if ((buffer = malloc(header)) == NULL)
		goto error;
	if (__pcapng_fread(f, buffer, header, ebuf) < 0)
		goto bad;
	words = (typeof(words)) buffer;
	lword = (typeof(lword)) buffer;
	if (words[0] == PCAPNG_SHB_TYPE) {
		switch (words[2]) {
		case PCAPNG_MAGIC:
			swapped = 0;
			break;
		case bswap_32(PCAPNG_MAGIC):
			swapped = 1;
			break;
		default:
			goto notpcapng;
		}
	}
	if (swapped == 0) {
		major = words[3] >> 16;
		minor = words[3] >> 0;
		seclen = lword[2];
		blen = words[1];
	} else {
		major = bswap_32(words[3]) >> 16;
		minor = bswap_32(words[3]) >> 0;
		seclen = bswap_64(lword[2]);
		blen = bswap_32(words[1]);
	}
	if (major != 1 || minor != 0)
		goto badversion;
	if (blen & 0x3)
		goto badblocksize;
	if (blen > 4096)
		goto bigblocksize;
	if ((ptr = realloc(buffer, blen)) == NULL)
		goto error;
	buffer = ptr;
	words = (typeof(words)) buffer;
	lword = (typeof(lword)) buffer;
	if (__pcapng_fread(f, buffer + header, blen - header, ebuf) < 0)
		goto bad;
	if ((p = pcapng_create(NULL, ebuf)) == NULL)
		goto bad;
	pthread_rwlock_init(&p->lock, NULL);
	p->shbi.shbi_magic = PCAPNG_MAGIC;
	p->shbi.shbi_major = major;
	p->shbi.shbi_minor = minor;
	p->shbi.shbi_section_length = seclen;
	p->swapped = swapped;
	p->savefile = 1;
	words += 6;
	ptr += 24;
	end = buffer + blen - 4;
	while (ptr < end) {
		uint16_t code, olen;

		if (swapped == 0) {
			code = words[0] >> 16;
			olen = words[0] >> 0;
		} else {
			code = bswap_32(words[0]) >> 16;
			olen = bswap_32(words[0]) >> 0;
		}
		words += 1;
		ptr = (typeof(ptr)) words;
		switch (code) {
		case OPT_ENDOFOPT:	/* end of options */
			goto end_of_options;
		case OPT_COMMENT:
			/* unpack the comment string */
			if (olen > 0 && p->shbi.shbi_comment == NULL)
				p->shbi.shbi_comment = strndup(ptr, min(olen, 256));
			break;
		case SHB_HARDWARE:
			/* unpack the hardware string */
			if (olen > 0 && p->shbi.shbi_hardware == NULL)
				p->shbi.shbi_hardware = strndup(ptr, min(olen, 256));
			break;
		case SHB_OS:
			/* unpack the os string */
			if (olen > 0 && p->shbi.shbi_os == NULL)
				p->shbi.shbi_os = strndup(ptr, min(olen, 256));
			break;
		case SHB_USERAPPL:
			/* unpack the userappl string */
			if (olen > 0 && p->shbi.shbi_userappl == NULL)
				p->shbi.shbi_userappl = strndup(ptr, min(olen, 256));
			break;
		case SHB_UUID:
			/* unpack the UUID */
			if (olen == 16 && p->shbi.shbi_uuid == NULL)
				if ((p->shbi.shbi_uuid = malloc(olen)) != NULL)
					memcpy(p->shbi.shbi_uuid, ptr, olen);
			break;
		default:
			/* skip unrecognized blocks */
			break;
		}
		olen = (olen + 3) & ~0x3;
		ptr += olen;
		words = (typeof(words)) ptr;
		continue;
	      end_of_options:
		ptr = end;
		words = (typeof(words)) ptr;
		break;
	}
	if (buffer != NULL)
		free(buffer);
	return (p);
      notpcapng:
	snprintf(ebuf, PCAP_ERRBUF_SIZE, "not a PCAP-NG dump file");
	goto bad;
      badversion:
	snprintf(ebuf, PCAP_ERRBUF_SIZE, "files version %hu.%hu not supported", major, minor);
	goto bad;
      badblocksize:
	snprintf(ebuf, PCAP_ERRBUF_SIZE, "invalid block length %u", blen);
	goto bad;
      bigblocksize:
	snprintf(ebuf, PCAP_ERRBUF_SIZE, "block length to large %u > 4096", blen);
	goto bad;
      error:
	snprintf(ebuf, PCAP_ERRBUF_SIZE, "error reading save file: %s", pcap_strerror(errno));
	goto bad;
      bad:
	if (buffer != NULL)
		free(buffer);
	if (p != NULL) {
		__pcapng_close(p);
		free(p);
		p = NULL;
	}
	return (p);
}

__asm__(".symver __pcapng_fopen_offline,pcapng_fopen_offline@@PCAPNG_1.0");

void
__pcapng_free_datalinks(int *dlt_list)
{
}

__asm__(".symver __pcapng_free_datalinks,pcapng_free_datalinks@@PCAPNG_1.0");

void
__pcapng_free_tstamp_types(int *tstamp_types)
{
}

__asm__(".symver __pcapng_free_tstamp_types,pcapng_free_tstamp_types@@PCAPNG_1.0");

int
__pcapng_get_selectable_fd(pcapng_t *p, int iid)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_get_selectable_fd(pp->pcap);
}

__asm__(".symver __pcapng_get_selectable_fd,pcapng_get_selectable_fd@@PCAPNG_1.0");

char *
__pcapng_geterr(pcapng_t *p)
{
	return (p_errbuf);
}

__asm__(".symver __pcapng_geterr,pcapng_geterr@@PCAPNG_1.0");

int
__pcapng_getnonblock(pcapng_t *p, int iid, char *ebuf)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_getnonblock(pp->pcap, ebuf);
}

__asm__(".symver __pcapng_getnonblock,pcapng_getnonblock@@PCAPNG_1.0");

int
__pcapng_inject(pcapng_t *p, int iid, const void *buf, size_t size)
{
}

__asm__(".symver __pcapng_inject,pcapng_inject@@PCAPNG_1.0");

int
__pcapng_is_swapped(pcapng_t *p)
{
}

__asm__(".symver __pcapng_is_swapped,pcapng_is_swapped@@PCAPNG_1.0");

static const char pcapng_version_string[] = "libpcapng version 1.0";

const char *
__pcapng_lib_version(void)
{
	return (pcapng_version_string);
}

__asm__(".symver __pcapng_lib_version,pcapng_lib_version@@PCAPNG_1.0");

int
__pcapng_list_datalinks(pcapng_t *p, int iid, int **dlt_buf)
{
}

__asm__(".symver __pcapng_list_datalinks,pcapng_list_datalinks@@PCAPNG_1.0");

int
__pcapng_list_tstamp_types(pcapng_t *p, int iid, int **tstamp_types)
{
}

__asm__(".symver __pcapng_list_tstamp_types,pcapng_list_tstamp_types@@PCAPNG_1.0");

void
__pcapng_loop_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
	struct pcapng_pkthdr *pph;
}

int
__pcapng_loop(pcapng_t *p, int iid, pcapng_handler callback, u_char *user)
{
}

__asm__(".symver __pcapng_loop,pcapng_loop@@PCAPNG_1.0");

#ifndef PCAPNG_MAJOR_VERSION
#define PCAPNG_MAJOR_VERSION 1
#endif

int
__pcapng_major_version(pcapng_t *p)
{
	return (PCAPNG_MAJOR_VERSION);
}

__asm__(".symver __pcapng_major_version,pcapng_major_version@@PCAPNG_1.0");

#ifndef PCAPNG_MINOR_VERSION
#define PCAPNG_MINOR_VERSION 0
#endif

int
__pcapng_minor_version(pcapng_t *p)
{
	return (PCAPNG_MINOR_VERSION);
}

__asm__(".symver __pcapng_minor_version,pcapng_minor_version@@PCAPNG_1.0");

const u_char *
__pcapng_next(pcapng_t *p, struct pcapng_pkthdr *h)
{
}

__asm__(".symver __pcapng_next,pcapng_next@@PCAPNG_1.0");

int
__pcapng_next_ex(pcapng_t *p, struct pcapng_pkthdr **pkt_hdr, const u_char **pkt_data)
{
}

__asm__(".symver __pcapng_next_ex,pcapng_next_ex@@PCAPNG_1.0");

int
__pcapng_offline_filter(struct bpf_program *fp, const struct pcapng_pkthdr *h, const u_char *pkt)
{
}

__asm__(".symver __pcapng_offline_filter,pcapng_offline_filter@@PCAPNG_1.0");

pcapng_t *
__pcapng_open_dead(int linktype, int snaplen)
{
	pcapng_t *p;
	pcapng_pcap_t *pp;
	pcap_t *pcap;
	size_t size;

	if ((p = __pcapng_create(NULL, ebuf)) == NULL)
		goto bad;
	if ((pcap = pcap_open_dead(linktype, snaplen)) == NULL)
		goto bad;
	if ((pp = __pcapng_init_from_pcap(p, pcap, NULL, ebuf)) == NULL)
		goto bad;
	return (p);
      bad:
	if (p != NULL) {
		__pcapng_close(p);
		free(p);
		p = NULL;
	}
	return (p);
}

__asm__(".symver __pcapng_open_dead,pcapng_open_dead@@PCAPNG_1.0");

/** @brief Open a device for capturing.
  * @param device the network device (interfaces) to open.
  * @param snaplen snapshot length to use on each interface.
  * @param promisc whether to set promiscuous mode on each interface.
  * @param timeout milliseconds timeout for earch interface.
  * @param ebuf error string buffer.
  *
  * pcapng_open_live() is used to obtain a PCAP-NG handle to look at packets
  * on the network.  device is a string that specifies the network devive to
  * open; on Linux an argument of "any" can be used to capture packets on all
  * interfaces (using DLT_LINUX_SLL); however, a NULL device argument will
  * not begin capture on any interface.  A source argument of "all" can be
  * used to capture on all interfaces (an interface is defined for each and
  * every interface that can be found with pcap_findalldevs()).
  *
  * Each interface of the returned handle must be activated with
  * pcapng_activate() or pcapng_activate_all() before packets acn be captured
  * with it; options for the interface, such as promiscuous mode, can be set
  * on each interface before activating it.
  *
  * The function returns NULL when an error is encountered and the error
  * string is placed into the buffer pointed to by the ebuf argument.
  */
pcapng_t *
__pcapng_open_live(const char *device, int snaplen, int promisc, int timeout, char *ebuf)
{
	pcapng_t *p;

	if ((p = __pcapng_create(NULL, ebuf)) == NULL)
		goto bad;
	if (device == NULL)
		device = "all";
	if (device != NULL) {
		pcapng_pcap_t *pp;
		pcap_t *pcap, size_t size;

		if (strcmp(device, "any") == 0) {
			pcap_if_t *ifp, *devs;

			if (pcap_findalldevs(&devs, ebuf) != 0)
				goto bad;
			for (ifp = devs; ifp != NULL; ifp = ifp->next) {
				if (strcmp(ifp->name, "all") == 0)
					continue;
				pcap = pcap_open_live(ifp->name, snaplen, promisc, timeout, ebuf);
				if (pcap == NULL)
					continue;
				if ((pp = __pcapng_init_from_pcap(p, pcap, ifp->name, ebuf)) == NULL) {
					pcap_freealldevs(devs);
					goto bad;
				}
			}
			if (devs != NULL)
				pcap_freealldevs(devs);
		} else {
			pcap = pcap_open_live(device, snaplen, promisc, timeout, ebuf);
			if (pcap == NULL)
				goto bad;
			if ((pp = __pcapng_init_from_pcap(p, pcap, device, ebuf)) == NULL)
				goto bad;
		}
	}
	return (p);
      bad:
	if (p != NULL) {
		__pcapng_close(p);
		free(p);
		p = NULL;
	}
	return (p);
}

/** Thread-safe version of __pcapng_open_live(). */
pcapng_t *
__pcapng_open_live(const char *device, int snaplen, int promisc, int timeout, char *ebuf)
{
	pcapng_t *p;
	int oldstate, state = PTHREAD_CANCEL_DISABLE;
	int oldtype, type = PTHREAD_CANCEL_DEFERRED;

	(void) pthread_setcancelstate(state, &oldstate);
	(void) pthread_setcanceltype(type, &oldtype);
	if ((p = __pcapng_open_live(device, snaplen, promisc, timeout, ebuf)) == NULL)
		pthread_testcancel();
	(void) pthread_setcanceltype(oldtype, &type);
	(void) pthread_setcancelstate(oldstate, &state);
	return (p);
}

__asm__(".symver __pcapng_open_live_r,pcapng_open_live@@PCAPNG_1.0");

/** @brief Open a saved capture file for reading.
  * @param fname the filename to open.
  * @param ebuf an error buffer.
  *
  * pcapng_open_offline() is called to open a PCAP-NG savefile for reading.
  * fname specifies the name of the file to open.  The file can have the
  * PCAP file format or PCAP-NG file format.  The names "-" and "/dev/stdin"
  * are synonyms for stdin.
  *
  * Returns a pcapng_t pointer on success and NULL on failure.  When NULL is
  * returned, ebuf is filled in with an appropriate error message.  ebuf
  * is assumed to be able to hold at least PCAP_ERRBUF_SIZE characters (not
  * including the terminating null character).
  */
pcapng_t *
__pcapng_open_offline(const char *fname, char *ebuf)
{
	pcapng_t *p;

	if (strcmp(fname, "-") == 0 || strcmp(fname, "/dev/stdin") == 0) {
		p = __pcapng_fopen_offline(stdin, ebuf);
	} else {
		FILE *f;

		f = fopen(fname, "r");
		if (f == NULL) {
			sprintf(ebuf, PCAP_ERRBUF_SIZE, "%s: %s", fname, pcap_strerror(errno));
			return (f);
		}
		if ((p = __pcapng_fopen_offline(f, ebuf)) == NULL)
			fclose(f);
	}
	return (p);
}

__asm__(".symver __pcapng_open_offline,pcapng_open_offline@@PCAPNG_1.0");

void
__pcapng_perror(pcapng_t *p, char *prefix)
{
	(void) fprintf(stderr, "%s: %s\n", prefix, p->errbuf);
}

__asm__(".symver __pcapng_perror,pcapng_perror@@PCAPNG_1.0");

int
__pcapng_sendpacket(pcapng_t *p, int iid, const u_char *buf, int size)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_sendpacket(pp->pcap, buf, size);
}

__asm__(".symver __pcapng_sendpacket,pcapng_sendpacket@@PCAPNG_1.0");

int
__pcapng_set_buffer_size(pcapng_t *p, int iid, int buffer_size)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_buffer_size(pp->pcap, buffer_size);
}

__asm__(".symver __pcapng_set_buffer_size,pcapng_set_buffer_size@@PCAPNG_1.0");

int
__pcapng_set_datalink(pcapng_t *p, int iid, int dlt)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_datalink(pp->pcap, dlt);
}

int
__pcapng_set_datalink_r(pcapng_t *p, int iid, int dlt)
{
	int ret;

	pthread_cleanup_push_defer_np(__pcapng_puthandle, p);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		if ((ret = __pcapng_set_datalink(p, iid, dlt)) != 0)
			pthread_testcancel();
		__pcapng_puthandle(p);
	} else
		pthread_testcancel();
	pthread_cleanup_pop_restore_np(0);
	return (ret);
}

__asm__(".symver __pcapng_set_datalink_r,pcapng_set_datalink@@PCAPNG_1.0");

int
__pcapng_set_promisc(pcapng_t *p, int iid, int promisc)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_promisc(pp->pcap, promisc);
}

__asm__(".symver __pcapng_set_promisc,pcapng_set_promisc@@PCAPNG_1.0");

int
__pcapng_set_promisc_all(pcapng_t *p, int promisc, int *iidp)
{
	int err = 0, warn = 0, iid;
	struct pcapng_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			return (err);
		if (pp == NULL)
			continue;
		if ((err = pcap_set_promisc(pp->pcap, promisc)) != 0) {
			__pcapng_set_error(err);
			strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
			if (err < 0)
				return (-1);
			warn = 1;
		}
	}
	return (warn);
}

__asm__(".symver __pcapng_set_promisc_all,pcapng_set_promisc_all@@PCAPNG_1.0");

int
__pcapng_set_rfmon(pcapng_t *p, int iid, int rfmon)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_rfmon(pp->pcap, rfmon);
}

__asm__(".symver __pcapng_set_rfmon,pcapng_set_rfmon@@PCAPNG_1.0");

int
__pcapng_set_promisc_all(pcapng_t *p, int promisc, int *iidp)
{
	int err = 0, warn = 0, iid;
	struct pcapng_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			return (err);
		if (pp == NULL)
			continue;
		if ((err = pcap_set_promisc(pp->pcap, promisc)) != 0) {
			__pcapng_set_error(err);
			strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
			if (err < 0)
				return (-1);
			warn = 1;
		}
	}
	return (warn);
}

__asm__(".symver __pcapng_set_promisc_all,pcapng_set_promisc_all@@PCAPNG_1.0");

int
__pcapng_set_snaplen(pcapng_t *p, int iid, int snaplen)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_snaplen(pp->pcap, snaplen);
}

__asm__(".symver __pcapng_set_snaplen,pcapng_set_snaplen@@PCAPNG_1.0");

int
__pcapng_set_snaplen_all(pcapng_t *p, int snaplen, int *iidp)
{
	int err = 0, warn = 0, iid;
	struct pcapng_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			return (err);
		if (pp == NULL)
			continue;
		if ((err = pcap_set_snaplen(pp->pcap, snaplen)) != 0) {
			__pcapng_set_error(err);
			strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
			if (err < 0)
				return (-1);
			warn = 1;
		}
	}
	return (warn);
}

__asm__(".symver __pcapng_set_snaplen_all,pcapng_set_snaplen_all@@PCAPNG_1.0");

int
__pcapng_set_timeout(pcapng_t *p, int iid, int to_ms)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_timeout(pp->pcap, to_msg);
}

__asm__(".symver __pcapng_set_timeout,pcapng_set_timeout@@PCAPNG_1.0");

int
__pcapng_set_timeout_all(pcapng_t *p, int timeout, int *iidp)
{
	int err = 0, warn = 0, iid;
	struct pcapng_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			return (err);
		if (pp == NULL)
			continue;
		if ((err = pcap_set_timeout(pp->pcap, timeout)) != 0) {
			__pcapng_set_error(err);
			strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
			if (err < 0)
				return (-1);
			warn = 1;
		}
	}
	return (warn);
}

__asm__(".symver __pcapng_set_timeout_all,pcapng_set_timeout_all@@PCAPNG_1.0");

int
__pcapng_set_tstamp_type(pcapng_t *p, int iid, int tstamp_type)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_set_tstamp_type(pp->pcap, tstamp_type);
}

__asm__(".symver __pcapng_set_tstamp_type,pcapng_set_tstamp_type@@PCAPNG_1.0");

int
__pcapng_setdirection(pcapng_t *p, int iid, pcap_direction_t d)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_setdirection(pp->pcap, d);
}

__asm__(".symver __pcapng_setdirection,pcapng_setdirection@@PCAPNG_1.0");

int
__pcapng_setfilter(pcapng_t *p, int iid, struct bpf_program *fp)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_setfilter(pp->pcap, fp);
}

__asm__(".symver __pcapng_setfilter,pcapng_setfilter@@PCAPNG_1.0");

int
__pcapng_setnonblock(pcapng_t *p, int iid, int nonblock)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	if ((err = pcap_setnonblock(pp->pcap, nonblock, &p_errbuf)) != 0)
		__pcapng_set_error(err);
	return (err);
}

int
__pcapng_setnonblock_r(pcapng_t *p, int iid, int nonblock)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_setnonblock(p, iid, nonblock);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_setnonblock_r,pcapng_setnonblock@@PCAPNG_1.0");

int
__pcapng_setnonblock_all(pcapng_t *p, int nonblock, int *iidp)
{
	int err = 0, warn = 0, iid;
	struct pcpang_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			return (err);
		if (pp == NULL)
			continue;
		if ((err = pcap_setnonblock(pp->ppcap, nonblock, &p_errbuf)) != 0) {
			__pacp_set_error(err);
			if (err < 0)
				return (-1);
			warn = 1;
		}
	}
	return (warn);
}

int
__pcapng_setnonblock_all_r(pcapng_t *p, int nonblock, int *iidp)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_setnonblock_all(p, nonblock, iidp);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_setnonblock_all_r,pcapng_setnonblock_all@@PCAPNG_1.0");


int
__pcapng_snapshot(pcapng_t *p, int iid)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	return pcap_snapshot(pp->pcap);
}

__asm__(".symver __pcapng_snapshot,pcapng_snapshot@@PCAPNG_1.0");

int
__pcapng_stats(pcapng_t *p, int iid, struct pcapng_stats *ps)
{
	int err;
	struct pcapng_pcap *pp;
	struct pcap_stats pcapstats;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	if ((err = pcap_stats(pp->pcap, &pcapstats))) {
		__pcapng_set_error(err);
		strncpy(p_errbuf, pcap_geterr(pp->pcap), PCAP_ERRBUF_SIZE);
		return (-1);
	}
	ps->ps_next = NULL;
	ps->ps_interfaceid = iid;
	ps->ps_comment = NULL;
	ps->ps_ts[0] = FIXME;
	ps->ps_ts[1] = FIXME;
	ps->ps_starttime[0] = FIXME;
	ps->ps_starttime[1] = FIXME;
	ps->ps_endtime[0] = FIXME;
	ps->ps_endtime[1] = FIXME;
	ps->ps_ifrecv = pcapstats.ps_recv;
	ps->ps_ifdrop = pcapstats.ps_ifdrop;
	ps->ps_filteraccept = FIXME;
	ps->ps_osdrop = pcapstats.ps_drop;
	ps->ps_usrdeliv = FIXME;
	ps->ps_ifcomp = FIXME;
	return (0);
}

__asm__(".symver __pcapng_stats,pcapng_stats@@PCAPNG_1.0");

int
__pcapng_list_stats(pcapng_t *p, struct pcapng_stats **psp)
{
	int iid;
	struct pcapng_stats **ptr;

	if ((ptr = psp) == NULL) {
		p_errno = PCAP_ERROR;
		errno = EINVAL;
		strncpy(p_errbuf, strerror(EINVAL), PCAP_ERRBUF_SIZE);
		return (-1);
	}
	*ptr = NULL;
	for (iid = 0; iid < p->if_count; iid++) {
		struct pcapng_stats *ps;

		if ((*ptr = malloc(sizeof(**ptr))) == NULL) {
			p_errno = PCAP_ERROR;
			strncpy(p_errbuf, strerror(errno), PCAP_ERRBUF_SIZE);
			goto bad;
		}
		if (__pcapng_stats(p, iid, *ptr) != 0)
			goto bad;
		ptr = &ps->ps_next;
	}
	return (0);
      bad:
	__pcapng_free_stats(*psp);
	*psp = NULL;
	return (-1);
}

int
__pcapng_list_stats_r(pcapng_t *p, struct pcapng_stats **psp, int *iidp)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_list_stats(p, psp, iidp);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_list_stats_r,pcapng_list_stats@@PCAPNG_1.0");

void
__pcapng_free_stats_data(struct pcapng_stats *ps)
{
	if (ps->ps_comment != NULL) {
		free(ps->ps_comment);
		ps->ps_comment = NULL;
	}
	return;
}

void
__pcapng_free_stats(struct pcapng_stats *ps)
{
	struct pcapng_stats *psnext = ps;

	while ((ps = psnext) != NULL) {
		psnext = ps->ps_next;
		ps->ps_next = NULL;
		__pcapng_free_stats_data(ps);
		free(ps);
	}
	return;
}

__asm__(".symver __pcapng_free_stats_data,pcapng_free_stats_data@@PCAPNG_1.0");

int
__pcapng_dump_stats(pcapng_dumper_t *d, struct pcapng_stats *ps)
{
	FILE *f = (FILE *) d;

	if (ps == NULL)
		return (0);
	for (; ps != NULL; ps = ps->ps_next) {
		if (__pcapng_write_isb(f, &ps->ps_isbi) != 0)
			return (-1);
	}
	return (0);
}

int
__pcapng_dump_stats_r(pcapng_dumper_t *d, struct pcapng_stats *ps)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	ret = __pcapng_dump_stats(d, ps);
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_dump_stats_r,pcapng_dump_stats@@PCAPNG_1.0");

int
__pcapng_dump_stats_all(pcapng_dumper_t *d, pcapng_t *p, int *iidp)
{
	int err = 0, iid;
	struct pcapng_stats ps;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_stats(p, *iidp, &ps)))
			break;
		if ((err = __pcapng_dump_stats(d, &ps)))
			break;
	}
	return (err);
}

int
__pcapng_dump_stats_all_r(pcapng_dumper_t *d, pcapng_t *p, int *iidp)
{
	int ret, oldtype;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_dump_stats_all(d, p, iidp);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_dump_stats_all_r,pcapng_dump_stats_all@@PCAPNG_1.0");

int
__pcapng_if(pcapng_t *p, int iid, struct pcapng_if *ifp)
{
	int err;
	struct pcapng_pcap *pp;

	if ((err = __pcapng_getif(p, iid, &pp)))
		return (err);
	memset(ifp, 0, sizeof(*ifp));
	ifp->if_next = NULL;
	ifp->if_linktype = pp->idbi.idbi_linktype;
	ifp->if_snaplen = pp->idbi.idbi_snaplen;
	if (pp->idbi.idbi_comment)
		ifp->if_comment = strncpy(pp->idbi.idbi_comment, 256);
	if (pp->idbi.idbi_name)
		ifp->if_name = strncpy(pp->idbi.idbi_name, 256);
	if (pp->idbi.idbi_desc)
		ifp->if_desc = strncpy(pp->idbi.idbi_desc, 256);
	if (pp->idbi.idbi_ipv4addr)
		if ((ifp->if_ipv4addr = malloc(8)) != NULL)
			memcpy(ifp->if_ipv4addr, pp->idbi.idbi_ipv4addr, 8);
	if (pp->idbi.idbi_ipv6addr)
		if ((ifp->if_ipv6addr = malloc(17)) != NULL)
			memcpy(ifp->if_ipv6addr, pp->idbi.idbi_ipv6addr, 17);
	if (pp->idbi.idbi_macaddr)
		if ((ifp->if_macaddr = malloc(6)) != NULL)
			memcpy(ifp->if_macaddr, pp->idbi.idbi_macaddr, 6);
	if (pp->idbi.idbi_euiaddr)
		if ((ifp->if_euiaddr = malloc(8)) != NULL)
			memcpy(ifp->if_euiaddr, pp->idbi.idbi_euiaddr, 8);
	ifp->if_speed = pp->idbi.idbi_speed;
	ifp->if_tsresol = pp->idbi.idbi_tsresol;
	ifp->if_tzone = pp->idbi.idbi_tzone;
	if ((ifp->if_filter.var_length = pp->idbi.idbi_filter.var_length) != 0)
		if (pp->idbi.idbi_filter.var_value != NULL)
			if ((ifp->if_filter.var_value = malloc(ifp->if_filter.var_length)) != NULL)
				memcpy(ifp->if_filter.var_value, pp->idbi.idbi_filter.var_value,
				       ifp->if_filter.var_length);
	if (pp->idbi.idbi_os)
		ifp->if_os = strncpy(pp->idbi.idbi_os, 256);
	ifp->if_fcslen = pp->idbi.idbi_fcslen;
	ifp->if_tsoffset[0] = pp->idbi.idbi_tsoffset[0];
	ifp->if_tsoffset[1] = pp->idbi.idbi_tsoffset[1];
	if ((ifp->if_physaddr.var_length = pp->idbi.idbi_physaddr.var_length) != 0)
		if (pp->idbi.idbi_physaddr.var_value != NULL)
			if ((ifp->if_physaddr.var_value =
			     malloc(ifp->if_physaddr.var_length)) != NULL)
				memcpy(ifp->if_physaddr.var_value, pp->idbi.idbi_physaddr.var_value,
				       ifp->if_physaddr.var_length);
	if (pp->idbi.idbi_uuid)
		if ((ifp->if_uuid = malloc(16)) != NULL)
			memcpy(ifp->if_uuid, pp->idbi.idbi_uuid, 16);
	ifp->if_ifindex = pp->idbi.idbi_ifindex;
	return (0);
}

__asm__(".symver __pcapng_if,pcapng_if@@PCAPNG_1.0");

int
__pcapng_list_if(pcapng_t *p, struct pcapng_if **ifpp)
{
	int iid;
	struct pcapng_if **ptr;

	if ((ptr = ifpp) == NULL) {
		p_errno = PCAP_ERROR;
		errno = EINVAL;
		strncpy(p_errbuf, strerror(EINVAL), PCAP_ERRBUF_SIZE);
		return (-1);
	}
	*ptr = NULL;
	for (iid = 0; iid < p->if_count; iid++) {
		struct pcapng_if *ifp;

		if ((*ptr = malloc(sizeof(**ptr))) == NULL) {
			p_errno = PCAP_ERROR;
			strncpy(p_errbuf, strerrno(errno), PCAP_ERRBUF_SIZE);
			goto bad;
		}
		if (__pcapng_if(p, iid, *ptr) != 0)
			goto bad;
		ptr = &ifp->if_next;
	}
	return (0);
      bad:
	__pcapng_free_if(*ifpp);
	*ifpp = NULL;
	return (-1);
}

__asm__(".symver __pcapng_list_if,pcapng_list_if@@PCAPNG_1.0");

void
__pcapng_free_if_data(struct pcapng_if *ifp)
{
	if (ifp->if_comment != NULL) {
		free(ifp->if_comment);
		ifp->if_comment = NULL;
	}
	if (ifp->if_name != NULL) {
		free(ifp->if_name);
		ifp->if_name = NULL;
	}
	if (ifp->if_desc != NULL) {
		free(ifp->if_desc);
		ifp->if_desc = NULL;
	}
	if (ifp->if_ipv4addr != NULL) {
		free(ifp->if_ipv4addr);
		ifp->if_ipv4addr = NULL;
	}
	if (ifp->if_ipv6addr != NULL) {
		free(ifp->if_ipv6addr);
		ifp->if_ipv6addr = NULL;
	}
	if (ifp->if_macaddr != NULL) {
		free(ifp->if_macaddr);
		ifp->if_macaddr = NULL;
	}
	if (ifp->if_euiaddr != NULL) {
		free(ifp->if_euiaddr);
		ifp->if_euiaddr = NULL;
	}
	if (ifp->if_filter.var_value != NULL) {
		free(ifp->if_filter.var_value);
		ifp->if_filter.var_value = NULL;
		ifp->if_filter.var_length = 0;
	}
	if (ifp->if_os != NULL) {
		free(ifp->if_os);
		ifp->if_os = NULL;
	}
	if (ifp->if_physaddr.var_value != NULL) {
		free(ifp->if_physaddr.var_value);
		ifp->if_physaddr.var_value = NULL;
		ifp->if_physaddr.var_length = 0;
	}
	if (ifp->if_uuid != NULL) {
		free(ifp->if_uuid);
		ifp->if_uuid = NULL;
	}
	return;
}

__asm__(".symver __pcapng_free_if_data,pcapng_free_if_data@@PCAPNG_1.0");

void
__pcapng_free_if(struct pcapng_if *ifp)
{
	struct pcapng_if *ifpnext = ifp;

	while ((ifp = ifpnext) != NULL) {
		ifpnext = ifp->if_next;
		ifp->if_next = NULL;
		__pcapng_free_if_data(ifp);
		free(ifp);
	}
	return;
}

__asm__(".symver __pcapng_free_if,pcapng_free_if@@PCAPNG_1.0");

/** @brief Write an interface descriptor chain to a PCAP-NG file.
  * @param d the PCAP-NG savefile.
  * @param ifp a pointer to the interface descriptor chain to write.
  *
  * pcapng_dump_if() write the interface descriptors specified by the pointer
  * to the interface descriptor chain, ifp, to the PCAP-NG savefile specified
  * by d.
  */
int
__pcapng_dump_if(pcapng_dumper_t *d, struct pcapng_if *ifp)
{
	FILE *f = (typeof(f)) d;

	if (ifp == NULL)
		return (0);
	for (; ifp != NULL; ifp = ifp->if_next) {
		if (__pcapng_write_idb(f, &ifp->idbi) != 0)
			return (-1);
	}
	return (0);
}

/** Thread-safe version of __pcapng_dump_if. */
int
__pcapng_dump_if_r(pcapng_dumper_t *d, struct pcapng_if *ifp)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	ret = __pcapng_dump_if(d, ifp);
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_dump_if_r,pcapng_dump_if@@PCAPNG_1.0");

int
__pcapng_dump_if_all(pcapng_dumper_t *d, pcapng_t *p, int *iidp)
{
	int err = 0, iid;
	struct pcapng_pcap *pp;

	if (iidp == NULL)
		iidp = &iid;
	for (*iidp = 0; *iidp < p->if_count; (*iidp)++) {
		if ((err = __pcapng_getif(p, *iidp, &pp)))
			break;
		if ((err = __pcapng_dump_if(d, &pp->idbi)))
			break;
	}
	return (err);
}

int
__pcapng_dump_if_all_r(pcapng_dumper_t *d, pcapng_t *p, int *iidp)
{
	int ret, oldstate;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
	if ((ret = __pcapng_gethandle(p)) == 0) {
		ret = __pcapng_dump_if_all(d, p, iidp);
		__pcapng_puthandle(p);
	}
	pthread_setcancelstate(oldstate, NULL);
	return (ret);
}

__asm__(".symver __pcapng_dump_if_all_r,pcapng_dump_if_all@@PCAPNG_1.0");

/**
  * @section Identification
  * This development manual was written for the OpenSS7 PCAP-NG Library version \$Name$(\$Revision$).
  * @author Brian F. G. Bidulock
  * @version \$Name$(\$Revision$)
  * @date \$Date$
  */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
