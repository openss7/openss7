/*****************************************************************************

 @(#) $Id: xti_local.h,v 0.9.2.3 2005/07/14 22:04:26 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2005/07/14 22:04:26 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_XTI_LOCAL_H
#define _SYS_XTI_LOCAL_H

#include <config.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/**
 * \defgroup libxti OpenSS7 XTI Library
 * \brief OpenSS7 XNS/XTI Library Calls
 *
 * This manpage contains documentation of OpenSS7 XTI Library functions that
 * are generated automatically from the source code with doxygen.  This
 * documentation is intended to be used for maintainers of the OpenSS7 XTI
 * Library and is not intended for users of the OpenSS7 XTI Library.  Users
 * should consult the documentation found in xti(3).
 *
 * \author Brian F. G. Bidulock
 * \version \$Name:  $(\$Revision: 0.9.2.3 $)
 * \date \$Date: 2005/07/14 22:04:26 $
 */

#define _SC_T_IOV_MAX		0
#define _SC_T_DEFAULT_ADDRLEN	1
#define _SC_T_DEFAULT_CONNLEN	2
#define _SC_T_DEFAULT_DISCLEN	3
#define _SC_T_DEFAULT_OPTLEN	4
#define _SC_T_DEFAULT_DATALEN	5

#define _T_DEFAULT_ADDRLEN	128
#define _T_DEFAULT_CONNLEN	256
#define _T_DEFAULT_DISCLEN	256
#define _T_DEFAULT_OPTLEN	256
#define _T_DEFAULT_DATALEN	1024
#define _T_TIMEOUT		-1
#define _T_IOV_MAX		16

#ifdef XNET_THREAD_SAFE
extern pthread_mutex_t _t_fds_mutex;
#endif

extern int *_t_errno(void);
extern int _t_ioctl(int fd, int cmd, void *arg);
extern int _t_strioctl(int fd, int cmd, void *arg, size_t arglen);
extern int _t_putmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int flags);
extern int _t_getmsg(int fd, struct strbuf *ctrl, struct strbuf *data, int *flags);
extern int _t_rcvconnect(int fd, struct t_call *call, struct t_info *info);
extern int _t_getinfo(int fd, struct t_info *info);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _SYS_XTI_LOCAL_H */
