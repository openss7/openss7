/*****************************************************************************

 @(#) $Id: xti.h,v 0.9.2.6 2008-04-29 07:10:42 brian Exp $

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

 Last Modified $Date: 2008-04-29 07:10:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti.h,v $
 Revision 0.9.2.6  2008-04-29 07:10:42  brian
 - updating headers for release

 Revision 0.9.2.5  2007/08/14 12:17:08  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2006/09/25 12:10:08  brian
 - updated and doxygenified headers

 Revision 0.9.2.3  2005/05/14 08:30:43  brian
 - copyright header correction

 Revision 0.9.2.2  2004/09/02 09:31:14  brian
 - Synchronization with other packages.

 Revision 0.9.2.1  2004/05/12 08:01:38  brian
 - Added in xti library and STREAMS modules.

 Revision 1.1.4.2  2004/04/13 12:12:51  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _XTI_H
#define _XTI_H

#ident "@(#) $RCSfile: xti.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/**
  * @weakgroup xnet OpenSS7 XNET Library
  * @{ */

/** @file
  * XTI (User) Header File. */

#include <sys/xti.h>
#include <sys/xti_inet.h>
#include <sys/xti_atm.h>
#include <sys/xti_osi.h>
#include <sys/xti_mosi.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#ifndef __KERNEL__
/**
  * @name XTI Library Functions
  * These are the formal version of the XTI library functions.  They are strong
  * aliased to the thread-safe "__xnet_t_*_r()" versions below.  Where a "_r"
  * version does not exist the "__xnet_t_*()" version below is used in its
  * stead.
  * @{ */
/* *INDENT-OFF* */
/** XTI Library Function: t_accept - accept a connection request. */
extern int t_accept(int, int, const struct t_call *);
/** XTI Library Function: t_addleaf - add a leaf to point to multipoint
  * connection. */
extern int t_addleaf(int, int, struct netbuf *);
/** XTI Library Function: t_alloc - allocate a library structure. */
extern char *t_alloc(int, int, int);
/** XTI Library Function: t_bind - bind an address to a transport endpoint. */
extern int t_bind(int, const struct t_bind *, struct t_bind *);
/** XTI Library Function: t_close - close a transport endpoint. */
extern int t_close(int);
/** XTI Library Function: t_connect - establish a connection. */
extern int t_connect(int, const struct t_call *, struct t_call *);
/** XTI Library Function: t_error - produce error message. */
extern int t_error(const char *);
/** XTI Library Function: t_free - free a library structure. */
extern int t_free(void *, int);
/** XTI Library Function: t_getinfo - get protocol-specific service information.  */
extern int t_getinfo(int, struct t_info *);
/** XTI Library Function: t_getprotaddr - get protocol addresses. */
extern int t_getprotaddr(int, struct t_bind *, struct t_bind *);
/** XTI Library Function: t_getstate - get the current state. */
extern int t_getstate(int);
/** XTI Library Function: t_listen - listen for a connection indication. */
extern int t_listen(int, struct t_call *);
/** XTI Library Function: t_look - look at current event on a transport
  * endpoint.  */
extern int t_look(int);
/** XTI Library Function: t_open - establish a transport endpoint. */
extern int t_open(const char *, int, struct t_info *);
/** XTI Library Function: t_optmgmt - manage options for a transport endpoint.  */
extern int t_optmgmt(int, const struct t_optmgmt *, struct t_optmgmt *);
/** XTI Library Function: t_rcv - receive data or expedited data on a
  * connection.  */
extern int t_rcv(int, char *, unsigned int, int *);
/** XTI Library Function: t_rcvconnect - receive the confirmation from a
  * connection request. */
extern int t_rcvconnect(int, struct t_call *);
/** XTI Library Function: t_rcvdis - retrieve information from disconnect. */
extern int t_rcvdis(int, struct t_discon *);
/** XTI Library Function: t_rcvleafchange - acknowledge receipt of a leaf change
  * indication. */
extern int t_rcvleafchange(int, struct t_leaf_status *);
/** XTI Library Function: t_rcvopt - receive data with options. */
extern int t_rcvopt(int fd, struct t_unitdata *optdata, int *flags);
/** XTI Library Function: t_rcvrel - acknowledge receipt of an orderly release
  * indication. */
extern int t_rcvrel(int);
/** XTI Library Function: t_rcvreldata - receive an orderly release indication
  * or confirmation containing user data. */
extern int t_rcvreldata(int, struct t_discon *);
/** XTI Library Function: t_rcvudata - receive a data unit. */
extern int t_rcvudata(int, struct t_unitdata *, int *);
/** XTI Library Function: t_rcvuderr - receive a unit data error indication. */
extern int t_rcvuderr(int, struct t_uderr *);
/** XTI Library Function: t_rcvv - receive data or expedited data sent over a
  * connection and put the data into one or more noncontiguous buffers. */
extern int t_rcvv(int, struct t_iovec *, unsigned int, int *);
/** XTI Library Function: t_rcvvopt - receive data with options into one or more
  * noncontiguous buffers. */
extern int t_rcvvopt(int fd, const struct t_unitdata *optdata, const
		     struct t_iovec *iov, unsigned int iovcount, int flags);
/** XTI Library Function: t_rcvvudata - receive a data unit into one or more
  * noncontiguous buffers. */
extern int t_rcvvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int, int *);
/** XTI Library Function: t_removeleaf - remove a leaf from a point to
  * multipoint connection. */
extern int t_removeleaf(int, int, int);
/** XTI Library Function: t_snd - send data or expedited data over a connection.
  */
extern int t_snd(int, char *, unsigned int, int);
/** XTI Library Function: t_snddis - send user-initiated disconnect request. */
extern int t_snddis(int, const struct t_call *);
/** XTI Library Function: t_sndopt - send data with options. */
extern int t_sndopt(int fd, const struct t_unitdata *optdata, int flags);
/** XTI Library Function: t_sndrel - initiate an orderly release. */
extern int t_sndrel(int);
/** XTI Library Function: t_sndreldata - initiate or respond to an orderly
  * release with user data. */
extern int t_sndreldata(int, struct t_discon *);
/** XTI Library Function: t_sndudata - send a data unit. */
extern int t_sndudata(int, const struct t_unitdata *);
/** XTI Library Function: t_sndv - send data or expedited data, from one or more
  * noncontiguous buffers, on a connection. */
extern int t_sndv(int, const struct t_iovec *, unsigned int, int);
/** XTI Library Function: t_sndvopt - send data with options from one or more
  * non-contiguous buffers. */
extern int t_sndvopt(int fd, const struct t_unitdata *optdata, const
		     struct t_iovec *iov, unsigned int iovcount, int flags);
/** XTI Library Function: t_sndvudata - send a data unit from one or more
  * non-contiguous buffers. */
extern int t_sndvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int);
/** XTI Library Function: t_strerror - generate error message string. */
extern const char *t_strerror(int);
/** XTI Library Function: t_sync - synchronise transport library. */
extern int t_sync(int);
/** XTI Library Function: t_sysconf - get configurable XTI variables. */
extern int t_sysconf(int);
/** XTI Library Function: t_unbind - disable a transport endpoint. */
extern int t_unbind(int);
/* *INDENT-ON* */
/** @} */

/** @name Non-Thread-Safe XTI Library Functions
  * These are the non-thread-safe versions of the XTI Library functions.  They
  * can by called directly using these symbols if thread safety is not
  * required and they may by just a little bit faster than the "t_" versions
  * (which are the same as the "_r" thread-safe versions).
  * @{ */
extern int __xnet_t_accept(int, int, const struct t_call *);
extern int __xnet_t_addleaf(int, int, struct netbuf *);
extern char *__xnet_t_alloc(int, int, int);
extern int __xnet_t_bind(int, const struct t_bind *, struct t_bind *);
extern int __xnet_t_close(int);
extern int __xnet_t_connect(int, const struct t_call *, struct t_call *);
extern int __xnet_t_error(const char *);
extern int __xnet_t_free(void *, int);
extern int __xnet_t_getinfo(int, struct t_info *);
extern int __xnet_t_getprotaddr(int, struct t_bind *, struct t_bind *);
extern int __xnet_t_getstate(int);
extern int __xnet_t_listen(int, struct t_call *);
extern int __xnet_t_look(int);
extern int __xnet_t_open(const char *, int, struct t_info *);
extern int __xnet_t_optmgmt(int, const struct t_optmgmt *, struct t_optmgmt *);
extern int __xnet_t_rcv(int, char *, unsigned int, int *);
extern int __xnet_t_rcvconnect(int, struct t_call *);
extern int __xnet_t_rcvdis(int, struct t_discon *);
extern int __xnet_t_rcvleafchange(int, struct t_leaf_status *);
extern int __xnet_t_rcvopt(int fd, struct t_unitdata *optdata, int *flags);
extern int __xnet_t_rcvrel(int);
extern int __xnet_t_rcvreldata(int, struct t_discon *);
extern int __xnet_t_rcvudata(int, struct t_unitdata *, int *);
extern int __xnet_t_rcvuderr(int, struct t_uderr *);
extern int __xnet_t_rcvv(int, struct t_iovec *, unsigned int, int *);
extern int __xnet_t_rcvvopt(int fd, const struct t_unitdata *optdata, const
			    struct t_iovec *iov, unsigned int iovcount, int flags);
extern int __xnet_t_rcvvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int, int *);
extern int __xnet_t_removeleaf(int, int, int);
extern int __xnet_t_snd(int, char *, unsigned int, int);
extern int __xnet_t_snddis(int, const struct t_call *);
extern int __xnet_t_sndopt(int fd, const struct t_unitdata *optdata, int flags);
extern int __xnet_t_sndrel(int);
extern int __xnet_t_sndreldata(int, struct t_discon *);
extern int __xnet_t_sndudata(int, const struct t_unitdata *);
extern int __xnet_t_sndv(int, const struct t_iovec *, unsigned int, int);
extern int __xnet_t_sndvopt(int fd, const struct t_unitdata *optdata, const
			    struct t_iovec *iov, unsigned int iovcount, int flags);
extern int __xnet_t_sndvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int);
extern const char *__xnet_t_strerror(int);
extern int __xnet_t_sync(int);
extern int __xnet_t_sysconf(int);
extern int __xnet_t_unbind(int);

/** @} */

/** @name Thread-Safe XTI Library Functions
  * These are thread-safe versions of the XTI Library functions.  Functions
  * that do not appear on this list (with an "_r") do not require thread
  * protection (normally because they can contain a thread cancellation point
  * and they do not accept a file descriptor for which read lock protection in
  * required.)
  *
  * These are the thread-safe (reentrant), and asyncrhonous thread
  * cancellation conforming versions of the XTI Library functions without the
  * "_r" at the end.  Many of these functions contain asyncrhonous thread
  * cancellation deferral because they cannot contain thread cancellation
  * points and yet the implementation calls functions that contain thread
  * cancellation points.
  *
  * Functions that do not appear on this list (with an "_r") do not require
  * thread protection (normally because they can contain a thread cancellation
  * point and they do not accept a file descriptor for which read lock
  * protection in required.)
  * @{ */
extern int __xnet_t_accept_r(int fd, int resfd, const struct t_call *call);
extern int __xnet_t_addleaf_r(int fd, int leafid, struct netbuf *addr);
extern char *__xnet_t_alloc_r(int fd, int type, int fields);
extern int __xnet_t_bind_r(int fd, const struct t_bind *req, struct t_bind *ret);
extern int __xnet_t_close_r(int fd);
extern int __xnet_t_connect_r(int fd, const struct t_call *sndcall, struct t_call *rcvcall);
extern int __xnet_t_getinfo_r(int fd, struct t_info *info);
extern int __xnet_t_getstate_r(int fd);
extern int __xnet_t_listen_r(int fd, struct t_call *call);
extern int __xnet_t_look_r(int fd);
extern int __xnet_t_open_r(const char *path, int oflag, struct t_info *info);
extern int __xnet_t_optmgmt_r(int fd, const struct t_optmgmt *req, struct t_optmgmt *ret);
extern int __xnet_t_rcv_r(int fd, char *buf, unsigned int nbytes, int *flags);
extern int __xnet_t_rcvconnect_r(int fd, struct t_call *call);
extern int __xnet_t_rcvdis_r(int fd, struct t_discon *discon);
extern int __xnet_t_rcvleafchange_r(int fd, struct t_leaf_status *change);
extern int __xnet_t_rcvrel_r(int fd);
extern int __xnet_t_rcvreldata_r(int fd, struct t_discon *discon);
extern int __xnet_t_rcvopt_r(int fd, struct t_unitdata *optdata, int *flags);
extern int __xnet_t_rcvudata_r(int fd, struct t_unitdata *unitdata, int *flags);
extern int __xnet_t_rcvv_r(int fd, struct t_iovec *iov, unsigned int iovcount, int *flags);
extern int __xnet_t_rcvvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
				unsigned int iovcount, int *flags);
extern int __xnet_t_removeleaf_r(int fd, int leafid, int reason);
extern int __xnet_t_snd_r(int fd, char *buf, unsigned int nbytes, int flags);
extern int __xnet_t_snddis_r(int fd, const struct t_call *call);
extern int __xnet_t_sndrel_r(int fd);
extern int __xnet_t_sndreldata_r(int fd, struct t_discon *discon);
extern int __xnet_t_sndopt_r(int fd, const struct t_unitdata *optdata, int flags);
extern int __xnet_t_sndvopt_r(int fd, const struct t_unitdata *optdata, const struct t_iovec *iov,
			      unsigned int iovcount, int flags);
extern int __xnet_t_sndudata_r(int fd, const struct t_unitdata *unitdata);
extern int __xnet_t_sndv_r(int fd, const struct t_iovec *iov, unsigned int iovcount, int flags);
extern int __xnet_t_sndvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
				unsigned int iovcount);
extern int __xnet_t_unbind_r(int fd);

/** @} */

#endif				/* __KERNEL__ */

/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _XTI_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
