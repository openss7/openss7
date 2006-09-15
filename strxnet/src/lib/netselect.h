/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __LOCAL_NETSELECT_H__
#define __LOCAL_NETSELECT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2006 OpenSS7 Corporation."

/*
 *  Local definitions for the Network Selection Facility (netselect) within the OpenSS7 Network
 *  Services Library (NSL).
 */

#define NCERR_BUFSZ 512
#define NDERR_BUFSZ 512

/*
 * Thread-specific data structure for the xnsl library, both netselect and netdir components.
 */
struct __nsl_tsd {
	int nderror;			/* error for network directory functions */
	int ncerror;			/* error for network configuration functions */
	char nderrbuf[NDERR_BUFSZ];	/* error buffer for network directory functions */
	char ncerrbuf[NCERR_BUFSZ];	/* errof buffer for network configuration functions */
	int fieldnum;			/* field number of error */
	int linenum;			/* line number of error */
};

struct netconf_handle {
	struct netconfig **nh_head;
	struct netconfig **nh_curr;
	pthread_rwlock_t nh_lock;
};

#define nc_line(__nc) (char *)(__nc)->nc_unused[7]
#define nc_next(__nc) (struct netconfig *)(__nc)->nc_unused[8]
#define nc_nextp(__nc) (struct netconfig **)&(__nc)->nc_unused[8]
#define nc_line_set(__nc,__line) (__nc)->nc_unused[7] = (unsigned long)(__line)

#endif				/* __LOCAL_NETSELECT_H__ */
