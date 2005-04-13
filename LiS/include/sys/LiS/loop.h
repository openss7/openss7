/*****************************************************************************

 @(#) $Id: $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: $ by $Author: $

 *****************************************************************************/

#ifndef __SYS_LIS_LOOP_H__
#define __SYS_LIS_LOOP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 */

#define	LOOP_IOCTL(n)	(('l' << 8) | (n))
#define LOOP_SET	LOOP_IOCTL(1)
#define LOOP_PUTNXT	LOOP_IOCTL(2)
#define	LOOP_MSGLVL	LOOP_IOCTL(3)
#define	LOOP_TIMR	LOOP_IOCTL(4)
#define	LOOP_MARK	LOOP_IOCTL(5)
#define	LOOP_GET_DEV	LOOP_IOCTL(6)
#define	LOOP_BUFCALL	LOOP_IOCTL(7)
#define	LOOP_CONCAT	LOOP_IOCTL(8)
#define	LOOP_COPY	LOOP_IOCTL(9)
#define	LOOP_DENY_OPEN	LOOP_IOCTL(10)	/* deny 2nd open of device */
#define	LOOP_BURST	LOOP_IOCTL(11)	/* fwd next msg as a burst */
#define	LOOP_FLUSH	LOOP_IOCTL(12)	/* flush and send data */

/*
 * The following structure is sent to loop via a TRANSPARENT ioctl.
 */
typedef struct {
	int cmnd;			/* see defines below */
	int i_arg;			/* integer argument for cmnd */
	void *p_arg;			/* pointer argument for cmnd */

} loop_xparent_t;

#define	LOOP_XPARENT_COPYIN	101	/* undefined ioctl values == xparent */
#define	LOOP_XPARENT_COPYOUT	102	/* undefined ioctl values == xparent */

#endif				/* __SYS_LIS_LOOP_H__ */
