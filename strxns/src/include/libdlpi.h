/*****************************************************************************

 @(#) $Id: libdlpi.h,v 0.9.2.2 2008-07-06 14:58:20 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-07-06 14:58:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: libdlpi.h,v $
 Revision 0.9.2.2  2008-07-06 14:58:20  brian
 - improvements

 Revision 0.9.2.1  2008-07-01 11:50:59  brian
 - added manual pages and library implementation

 *****************************************************************************/

#ifndef __LIBDLPI_H__
#define __LIBDLPI_H__

#ident "@(#) $RCSfile: libdlpi.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/**
  * @weakgroup dlpi OpenSS7 DLPI Library
  * @{ */

/** @file
  * DLPI (User) Header File. */

#include <sys/types.h>
#include <ctype.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifdef _SUN_SOURCE
#include <sys/dlpi.h>
#include <sys/dlpi_sun.h>
#else
#define _SUN_SOURCE
#include <sys/dlpi.h>
#include <sys/dlpi_sun.h>
#undef _SUN_SOURCE
#endif

#if 0
#define DLPI_EBADMSG		(-EBADMSG)
#define DLPI_EINHANDLE		(-EBADF)
#define DLPI_ETIMEDOUT		(-ETIMEDOUT)
#define DLPI_EINVAL		(-EINVAL)
#define DLPI_EUNAVAILSAP	(-EBUSY)
#define DLPI_FAILURE		(-EFAULT)
#define DLPI_EBADLINK		(-ESRCH)
#define DLPI_ENOLINK		(-ENXIO)
#define DLPI_ERAWNOTSUP		(-ENOTSUP)
#define DLPI_EMODENOTSUP	(-ENOTSUP)
#define DLPI_EVERNOTSUP		(-ENOTSUP)
#define DLPI_ENOTENOTSUP	(-ENOTSUP)
#define DLPI_ENOTEIDINVAL	(-EINVAL)
#endif

#define DLPI_NATIVE		0
#define DLPI_PASSIVE		1
#define DLPI_RAW		2

#define DLPI_ADDRTYPE_UNICAST	0
#define DLPI_ADDRTYPE_GROUP	1

#define DLPI_PHYSADDR_MAX	63
#define DLPI_LINKNAME_MAX	32
#define DLPI_ANY_SAP		(-1U)

#define DLPI_SUCCESS		0x1000
#define DLPI_FAILURE		0x1001
#define DLPI_EBADLINK		0x1002
#define DLPI_EBADMSG		0x1003
#define DLPI_EINHANDLE		0x1004
#define DLPI_EINVAL		0x1005
#define DLPI_ELINKNAMEINVAL	0x1006
#define DLPI_EMODENOTSUP	0x1007
#define DLPI_ENOLINK		0x1008
#define DLPI_ENOTEIDINVAL	0x1009
#define DLPI_ENOTEINVAL		0x100a
#define DLPI_ENOTENOTSUP	0x100b
#define DLPI_ENOTSTYLE2		0x100c
#define DLPI_ERAWNOTSUP		0x100d
#define DLPI_ETIMEDOUT		0x100e
#define DLPI_EUNAVAILSAP	0x100f
#define DLPI_EVERNOTSUP		0x1010

/* should put these in sys/libdlpi.h */

struct __dlpi_user;

typedef struct __dlpi_user *dlpi_handle_t;

struct __dlpi_notify_id {
	uint dnid_note;
	uint dnid_speed;
	uint dnid_size;
	uint8_t dnid_physaddrlen;
	uint8_t dnid_physaddr[DLPI_PHYSADDR_MAX];
};

struct __dlpi_notify;

typedef struct __dlpi_notify *dlpi_notifyid_t;

typedef struct {
	uint di_opts;
	uint di_max_sdu;
	uint di_min_sdu;
	uint di_state;
	uint8_t di_mactype;
	char di_linkname[DLPI_LINKNAME_MAX];
	uint8_t di_physaddrlen;
	uint8_t di_physaddr[DLPI_PHYSADDR_MAX];
	uint8_t di_bcastaddrlen;
	uint8_t di_bcastaddr[DLPI_PHYSADDR_MAX];
	uint di_sap;
	int di_timeout;
	dl_qos_cl_sel1_t di_qos_sel;
	dl_qos_cl_range1_t di_qos_range;
} dlpi_info_t;

typedef ushort dlpi_addrtype_t;

typedef struct {
	uint8_t dri_destaddrlen;
	uint8_t dri_destaddr[DLPI_PHYSADDR_MAX];
	dlpi_addrtype_t dri_destaddrtype;
	size_t dri_totmsglen;
} dlpi_recvinfo_t;

typedef struct {
	uint dsi_sap;
	dl_priority_t dsi_prio;
} dlpi_sendinfo_t;

typedef struct {
	uint dni_note;
	union {
		uint dni_speed;
		uint dni_size;
		struct {
			uint8_t dni_physaddrlen;
			uint8_t dni_physaddr[DLPI_PHYSADDR_MAX];
		};
	};
} dlpi_notifyinfo_t;

typedef void dlpi_notifyfunc_t(dlpi_handle_t, dlpi_notifyinfo_t *, void *);
typedef bool dlpi_walkfunc_t(const char *name, void *arg);

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#ifndef __KERNEL__
/**
  * @name DLPI Library Functions
  * These are the formal version of the DLPI library functions.  They are
  * strong aliased to the thread-safe "__dlpi_*_r()" versions below.  Where a
  * "_r" version does not exist, the "__dlpi_*()" version below is used in its
  * stead.
  * @{ */
/* *INDENT-OFF* */
/** DLPI Library Function: dlpi_arptype - convert mactype to arptype */
extern uint dlpi_arptype(uint mactype);
/** DLPI Library Function: dlpi_bind - bind a link to a SAP */
extern int dlpi_bind(dlpi_handle_t dh, uint sap, uint *boundsap);
/** DLPI Library Function: dlpi_close - close a link */
extern void dlpi_close(dlpi_handle_t dh);
/** DLPI Library Function: dlpi_disabmulti - disable a multicast address */
extern int dlpi_disabmulti(dlpi_handle_t dh, const void *aptr, size_t alen);
/** DLPI Library Function: dlpi_disabnotify - disable notification */
extern int dlpi_disabnotify(dlpi_handle_t dh, dlpi_notifyid_t id, void **argp);
/** DLPI Library Function: dlpi_enabmulti - enable a multicast address */
extern int dlpi_enabmulti(dlpi_handle_t dh, const void *aptr, size_t alen);
/** DLPI Library Function: dlpi_enabnotify - enable notification */
extern int dlpi_enabnotify(dlpi_handle_t dh, uint notes, dlpi_notifyfunc_t *fncp, void *arg, dlpi_notifyid_t *nid);
/** DLPI Library Function: dlpi_fd - get link file descriptor */
extern int dlpi_fd(dlpi_handle_t dh);
/** DLPI Library Function: dlpi_get_physaddr - get link physical address */
extern int dlpi_get_physaddr(dlpi_handle_t dh, uint type, void *aptr, size_t *alen);
/** DLPI Library Function: dlpi_iftype - convert mactype to iftype */
extern uint dlpi_iftype(uint mactype);
/** DLPI Library Function: dlpi_info - get link information */
extern int dlpi_info(dlpi_handle_t dh, dlpi_info_t **di, uint opt);
/** DLPI Library Function: dlpi_linkname - get link name string */
extern const char *dlpi_linkname(dlpi_handle_t dh);
/** DLPI Library Function: dlpi_mactyp - get link mactype string */
extern const char *dlpi_mactype(uint mactype);
/** DLPI Library Function: dlpi_open - open a link */
extern int dlpi_open(const char *linkname, dlpi_handle_t *dhp, uint flags);
/** DLPI Library Function: dlpi_promiscoff - turn promiscuous mode off */
extern int dlpi_promiscoff(dlpi_handle_t dh, uint level);
/** DLPI Library Function: dlpi_promiscon - turn promiscuous mode on */
extern int dlpi_promiscon(dlpi_handle_t dh, uint level);
/** DLPI Library Function: dlpi_recv - receive events from link */
extern int dlpi_recv(dlpi_handle_t dh, void **saptr, size_t *salen, void *buf, size_t *buflen, int wait, dlpi_recvinfo_t *recvp);
/** DLPI Library Function: dlpi_send - send events to link */
extern int dlpi_send(dlpi_handle_t dh, const void *daptr, size_t dalen, const void *buf, size_t buflen, const dlpi_sendinfo_t *sendp);
/** DLPI Library Function: dlpi_set_physaddr - set link physical address */
extern int dlpi_set_physaddr(dlpi_handle_t dh, uint type, const void *aptr, size_t alen);
/** DLPI Library Function: dlpi_set_timeout - set timeout */
extern int dlpi_set_timeout(dlpi_handle_t dh, int seconds);
/** DLPI Library Function: dlpi_strerror - get error string */
extern const char *dlpi_strerror(int error);
/** DLPI Library Function: dlpi_unbind - unbind link from SAP */
extern int dlpi_unbind(dlpi_handle_t dh);
/** DLPI Library Function: dlpi_walk - walk available links */
extern void dlpi_walk(dlpi_walkfunc_t *fn, void *arg, uint flags);
/* *INDENT-ON* */
/** @} */

/** @name Non-Thread-Safe DLPI Library Functions
  * These are the non-thread-safe versions of the DLPI Library functions.
  * They can be called directly using these symbols if thread safety is not
  * required and they may be just a little bit faster than the "dlpi_"
  * versions (which are the same as the "_r" thread-safe versions).
  * @{ */
extern uint __dlpi_arptype(uint mactype);
extern int __dlpi_bind(dlpi_handle_t dh, uint sap, uint *boundsap);
extern void __dlpi_close(dlpi_handle_t dh);
extern int __dlpi_disabmulti(dlpi_handle_t dh, const void *aptr, size_t alen);
extern int __dlpi_disabnotify(dlpi_handle_t dh, dlpi_notifyid_t id, void **argp);
extern int __dlpi_enabmulti(dlpi_handle_t dh, const void *aptr, size_t alen);
extern int __dlpi_enabnotify(dlpi_handle_t dh, uint notes, dlpi_notifyfunc_t *fncp, void *arg, dlpi_notifyid_t *nid);
extern int __dlpi_fd(dlpi_handle_t dh);
extern int __dlpi_get_physaddr(dlpi_handle_t dh, uint type, void *aptr, size_t *alen);
extern uint __dlpi_iftype(uint mactype);
extern int __dlpi_info(dlpi_handle_t dh, dlpi_info_t *di, uint opt);
extern const char *__dlpi_linkname(dlpi_handle_t dh);
extern const char *__dlpi_mactype(uint mactype);
extern int __dlpi_open(const char *linkname, dlpi_handle_t *dhp, uint flags);
extern int __dlpi_promiscoff(dlpi_handle_t dh, uint level);
extern int __dlpi_promiscon(dlpi_handle_t dh, uint level);
extern int __dlpi_recv(dlpi_handle_t dh, void **saptr, size_t *salen, void *buf, size_t *buflen, int wait, dlpi_recvinfo_t *recvp);
extern int __dlpi_send(dlpi_handle_t dh, const void *daptr, size_t dalen, const void *buf, size_t buflen, const dlpi_sendinfo_t *sendp);
extern int __dlpi_set_physaddr(dlpi_handle_t dh, uint type, const void *aptr, size_t alen);
extern int __dlpi_set_timeout(dlpi_handle_t dh, int seconds);
extern const char *__dlpi_strerror(int error);
extern int __dlpi_unbind(dlpi_handle_t dh);
extern void __dlpi_walk(dlpi_walkfunc_t *fn, void *arg, uint flags);

/** @} */

/** @name Thread-Safe DLPI Library Functions
  * These are thread-safe versions of the DLPI Library functions.  Functions
  * that do not appear on this list (with an "_r") do not require thread
  * protection (normally because they can contain a thread cancellation point
  * and they do not accept a file descriptor for which read lock protection is
  * required.)
  *
  * These are the thread-safe (reentrant), and asyncrhonous thread
  * cancellation conforming version of the DLPI Library functions without the
  * "_r" at the end.  Many of these functions contain asynchronous thread
  * cancellation deferral because they cannot contain thread cancellation
  * points and yet the implementation calls functions that contain thread
  * cancellation points.
  *
  * Functions that do not appear on this list (with an "_r") do not require
  * thread protection (normally because they can contain a thread cancellation
  * point and they do not accept a file descriptor for which read lock
  * protection is required.)
  * @{ */
extern uint __dlpi_arptype_r(uint mactype);
extern int __dlpi_bind_r(dlpi_handle_t dh, uint sap, uint *boundsap);
extern void __dlpi_close_r(dlpi_handle_t dh);
extern int __dlpi_disabmulti_r(dlpi_handle_t dh, const void *aptr, size_t alen);
extern int __dlpi_disabnotify_r(dlpi_handle_t dh, dlpi_notifyid_t id, void **argp);
extern int __dlpi_enabmulti_r(dlpi_handle_t dh, const void *aptr, size_t alen);
extern int __dlpi_enabnotify_r(dlpi_handle_t dh, uint notes, dlpi_notifyfunc_t *fncp, void *arg,
			       dlpi_notifyid_t *nid);
extern int __dlpi_fd_r(dlpi_handle_t dh);
extern int __dlpi_get_physaddr_r(dlpi_handle_t dh, uint type, void *aptr, size_t *alen);
extern uint __dlpi_iftype_r(uint mactype);
extern int __dlpi_info_r(dlpi_handle_t dh, dlpi_info_t *iptr, uint opt);
extern const char *__dlpi_linkname_r(dlpi_handle_t dh);
extern const char *__dlpi_mactype_r(uint mactype);
extern int __dlpi_open_r(const char *linkname, dlpi_handle_t *dhp, uint flags);
extern int __dlpi_promiscoff_r(dlpi_handle_t dh, uint level);
extern int __dlpi_promiscon_r(dlpi_handle_t dh, uint level);
extern int __dlpi_recv_r(dlpi_handle_t dh, void **saptr, size_t *salen, void *buf, size_t *buflen,
			 int wait, dlpi_recvinfo_t *recvp);
extern int __dlpi_send_r(dlpi_handle_t dh, const void *daptr, size_t dalen, const void *buf,
			 size_t buflen, const dlpi_sendinfo_t *sendp);
extern int __dlpi_set_physaddr_r(dlpi_handle_t dh, uint type, const void *aptr, size_t alen);
extern int __dlpi_set_timeout_r(dlpi_handle_t dh, int seconds);
extern const char *__dlpi_strerror_r(int error);
extern int __dlpi_unbind_r(dlpi_handle_t dh);
extern void __dlpi_walk_r(dlpi_walkfunc_t *fn, void *arg, uint flags);

/** @} */
#endif				/* __KERNEL__ */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __LIBDLPI_H__ */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
