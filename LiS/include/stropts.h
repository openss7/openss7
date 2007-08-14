/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log: stropts.h,v $
 Revision 1.1.1.1.12.2  2005/05/14 08:35:09  brian
 - copyright header correction

 Revision 1.1.1.1.12.1  2005/03/09 23:13:50  brian
 - First stab at autoconf'ed 2.18.0.  Results of merge.

 *****************************************************************************/

#ifndef _STROPTS_H
#define _STROPTS_H
#define _LIS_STROPTS_H

#ident "@(#) $Name:  $($Revision: 1.1.1.1.12.2 $) Copyright (c) 1997-2005  Open SS7 Corporation"

/* This file can be processed with doxygen(1). */

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/** @addtogroup strcalls
  * @{ */

/** @file
  * STREAMS Options and System Calls (User Header File).  */

#include <sys/stropts.h>

/** @name STREAMS System Calls
  * User level routines for getmsg/putmsg, etc.
  * @{ */
extern int fattach(int fd, const char *path);
extern int fdetach(const char *path);
extern int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int *flagsp);
extern int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int *bandp, int *flagsp);
extern int isastream(int fd);
extern int pipe(int fds[2]);
extern int putmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int flags);
extern int putpmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int band, int flags);

/** @} */

/** @name STREAMS System Calls
  * Non-recusive implementation.
  * @{ */
extern int __lis_fattach(int fd, const char *path);
extern int __lis_fdetach(const char *path);
extern int __lis_getmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int *flagsp);
extern int __lis_getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int *bandp,
			 int *flagsp);
extern int __lis_isastream(int fd);
extern int __lis_pipe(int fds[2]);
extern int __lis_putmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int flags);
extern int __lis_putpmsg(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int band,
			 int flags);
/** @} */

/** @name STREAMS System Calls
  * Recursive implementation.
  * @{ */
extern int __lis_fdetach_r(const char *path);
extern int __lis_getmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int *flagsp);
extern int __lis_getpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int *bandp,
			   int *flagsp);
extern int __lis_putmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int flags);
extern int __lis_putpmsg_r(int fd, struct strbuf *ctlptr, struct strbuf *dataptr, int band,
			   int flags);
/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

/** @} */

#endif				/* _STROPTS_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
