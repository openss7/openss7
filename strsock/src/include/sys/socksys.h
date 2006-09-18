/*****************************************************************************

 @(#) $Id: socksys.h,v 0.9.2.2 2006/09/18 01:43:52 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/09/18 01:43:52 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_SOCKSYS_H__
#define __SYS_SOCKSYS_H__

#ident "@(#) $RCSfile: socksys.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

struct socksysreq {
	long args[7];
};

struct socksysreq32 {
	int32_t args[7];
};

struct socknewproto {
	int family;			/* address family */
	int type;			/* socket type */
	int proto;			/* protocol */
	dev_t dev;			/* major/minor (must be clone) */
	int flags;			/* protosw flags */
};

struct socknewproto32 {
	int32_t family;			/* address family */
	int32_t type;			/* socket type */
	int32_t proto;			/* protocol */
	dev_t dev;			/* major/minor (must be clone) */
	int32_t flags;			/* protosw flags */
};

#define SO_ACCEPT	     1
#define SO_BIND		     2
#define SO_CONNECT	     3
#define SO_GETPEERNAME	     4
#define SO_GETSOCKNAME	     5
#define SO_GETSOCKOPT	     6
#define SO_LISTEN	     7
#define SO_RECV		     8
#define SO_RECVFROM	     9
#define SO_SEND		    10
#define SO_SENDTO	    11
#define SO_SETSOCKOPT	    12
#define SO_SHUTDOWN	    13
#define SO_SOCKET	    14
#define SO_SELECT	    15
#define SO_GETIPDOMAIN	    16
#define SO_SETIPDOMAIN	    17
#define SO_ADJTIME	    18
#define SO_SETREUID	    19
#define SO_SETREGID	    20
#define SO_GETTIME	    21
#define SO_SETTIME	    22
#define SO_GETITIMER	    23
#define SO_SETITIMER	    24
#define SO_RECVMSG	    25
#define SO_SENDMSG	    26
#define SO_SOCKPAIR	    27

#define SIOCSOCKSYS	    0x8966

#define SOCKSYS_TIMEOUT	    -1

#endif				/* __SYS_SOCKSYS_H__ */
