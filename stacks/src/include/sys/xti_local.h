/*****************************************************************************

 @(#) $Id: xti_local.h,v 0.9.2.6 2008-04-29 07:10:46 brian Exp $

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

 Last Modified $Date: 2008-04-29 07:10:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_local.h,v $
 Revision 0.9.2.6  2008-04-29 07:10:46  brian
 - updating headers for release

 Revision 0.9.2.5  2007/08/14 12:17:13  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 *****************************************************************************/

#ifndef _SYS_XTI_LOCAL_H
#define _SYS_XTI_LOCAL_H

#ident "@(#) $RCSfile: xti_local.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI Local header file.
  *
  * This is private header file that contains definitions internal to the
  * OpenSS7 XTI/TLI Library implementation.  */

#include <config.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/*
 * XTI Local Header File.
 */

/**
  * @name System Configuration Names
  * Definitions for t_sysconf().
  * @{ */
#if 0
#define _SC_T_IOV_MAX		0	/**< IOV maximum. */
#endif
#define _SC_T_DEFAULT_ADDRLEN	1	/**< Default address length. */
#define _SC_T_DEFAULT_CONNLEN	2	/**< Default connect data length. */
#define _SC_T_DEFAULT_DISCLEN	3	/**< Default disconnect data length. */
#define _SC_T_DEFAULT_OPTLEN	4	/**< Default options length. */
#define _SC_T_DEFAULT_DATALEN	5	/**< Default data length. */
/** @} */

/**
  * @name System Configuration Default Values
  * Definitions for t_sysconf().
  * @{ */
#define _T_DEFAULT_ADDRLEN	128	/**< Default address length. */
#define _T_DEFAULT_CONNLEN	256	/**< Default connect data length. */
#define _T_DEFAULT_DISCLEN	256	/**< Default disconnect data length. */
#define _T_DEFAULT_OPTLEN	256	/**< Default optoins length. */
#define _T_DEFAULT_DATALEN	1024	/**< Default data length. */
#define _T_TIMEOUT		-1	/**< Default timeout. */
#define _T_IOV_MAX		16	/**< IOV maximum. */
/** @} */

#ifdef XNET_THREAD_SAFE
extern pthread_mutex_t _t_fds_mutex;
#endif

/**
  * @name Internal Functions
  * @{ */
extern int *_t_errno(void);
extern int _t_ioctl(int fd, int cmd, void *arg);
extern int _t_strioctl(int fd, int cmd, void *arg, size_t arglen);
extern int _t_putmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int flags);
extern int _t_getmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int *flags);
extern int _t_rcvconnect(int fd, struct t_call *call, struct t_info *info);
extern int _t_getinfo(int fd, struct t_info *info);

/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _SYS_XTI_LOCAL_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
