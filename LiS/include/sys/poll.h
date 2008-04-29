/*****************************************************************************

 @(#) $RCSfile: poll.h,v $ $Name:  $($Revision: 1.1.1.1.12.8 $) $Date: 2008-04-29 08:33:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2008-04-29 08:33:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: poll.h,v $
 Revision 1.1.1.1.12.8  2008-04-29 08:33:15  brian
 - update headers for Affero release

 Revision 1.1.1.1.12.7  2007/08/14 10:47:08  brian
 - GPLv3 header update

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * <strpoll> --- poll
 * Author          : gram & nemo
 * Created On      : Mon Oct 17 11:37:12 1994
 * RCS Id          ; $Id: poll.h,v 1.1.1.1.12.8 2008-04-29 08:33:15 brian Exp $
 * Last Modified By: David Grothe
 * Last Modified On: 
 * Update Count    : 6
 * Status          : Debugged
 * Prefix(es)      : 
 * Requirements    :
 * Purpose         :
 *                 :
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

#ifndef _SYS_POLL_H
#define	_SYS_POLL_H

#ident "@(#) $RCSfile: poll.h,v $ $Name:  $($Revision: 1.1.1.1.12.8 $) $Date: 2008-04-29 08:33:15 $"

/* This file can be processed with doxygen(1). */

/*  -------------------------------------------------------------------  */

#if defined(__linux__)
/*
 * The strategy here is that if we are compiling for a 2.1 or 2.2 kernel
 * we want to use the kernel's poll.h.  Otherwise we will use the poll.h
 * that comes with LiS.  "Otherwise" also includes the use of LiS as a
 * portable STREAMS with no kernel poll.h to be included.
 */
# if !defined(_LIS_M_DEP_H) || !defined(__KERNEL__)
# ifndef USER			/* Nonsense, and error if no linux/version.h */
#  include <linux/version.h>
#  ifndef KERNEL_VERSION
#  define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#  endif
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#   define	KERNEL_2_0
#  else
#   define	KERNEL_2_1
#  endif
# endif				/* USER */
# endif				/* _LIS_M_DEP_H */
#endif				/* __linux__ */

/*
 * KERNEL_2_1 defined only if __linux__ is also defined.
 */
#if defined(KERNEL_2_1)		/* 2.1 kernel uses Linux's poll.h */
#define USED_LINUX_POLL_H	1
#include <linux/poll.h>		/* instead of the rest of this file */
#  if defined(_SPARC_LIS_) && !defined(POLLMSG)
#    define POLLMSG         0x0200	/* supply missing def for SPARC */
#  endif
#else				/* use LiS's poll.h */
				/* include the rest of this file */
#define USED_LIS_POLL_H	1	/**< Lis poll.h used indicator. */
#ifndef _POLL_H
#include <sys/LiS/poll.h>	/* streams module symbols & types */
#endif

/** @addtogroup strcalls
  * @{ */

/** @file
  * Poll header file.  */

/*  -------------------------------------------------------------------  */

/** Poll file descriptor/event structure.
  * Structure of file descriptor/event pairs supplied in the poll arrays.
  *
  * Magic Garden calls this "struct pollfd", AT&T STREAMS Programers Guide calls
  * it "struct poll".
  */
typedef struct poll {
	int fd;				/* File desc to poll. */
	short events;			/* Events of interest on fd. */
	short revents;			/* Events that occurred on fd. */
} pollfd_t;

/** Poll file descriptor/event structure. */
struct pollfd {
	int fd;				/* File desc to poll. */
	short events;			/* Events of interest on fd. */
	short revents;			/* Events that occurred on fd. */
};

/** @name Testable Select Events.
  * @{ */
#define	POLLIN		0x0001	/**< fd is readable. */
#define	POLLPRI		0x0002	/**< High priority info at fd. */
#define	POLLOUT		0x0004	/**< fd is writeable (won't block). */
#define	POLLRDNORM	0x0040	/**< Normal data is readable. */
#define	POLLWRNORM	POLLOUT
#define	POLLRDBAND	0x0080	/**< Out-of-band data is readable. */
#define	POLLWRBAND	0x0100	/**< Out-of-band data is writeable. */
#define	POLLMSG		0x0200	/**< M_SIG at head of queue. */

#define	POLLNORM	POLLRDNORM
/** @} */

/** @name Non-testable Poll Events
  * Non-testable poll events (may not be specified in events field,
  * but may be returned in revents field).
  * @{ */
#define	POLLERR		0x0008	/**< fd has error condition. */
#define	POLLHUP		0x0010	/**< fd has been hung up on. */
#define	POLLNVAL	0x0020	/**< Invalid pollfd entry. */
/** @} */

#endif				/* defined(KERNEL_2_1) */

/*
 * The prototype for the routine called from user programs for the
 * poll system call.
 */
#ifndef poll			/* no macro by that name */

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

extern int poll(struct pollfd *fds, unsigned long nfds, int timeout);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

/** @} */

#endif
/*  -------------------------------------------------------------------  */
#endif				/* _SYS_POLL_H */

// vim: ft=cpp com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
