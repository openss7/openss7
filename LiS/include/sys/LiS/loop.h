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

 $Log$
 *****************************************************************************/

#ifndef __SYS_LIS_LOOP_H__
#define __SYS_LIS_LOOP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/* This file can be processed with doxygen(1). */

/*
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 */

/** @addtogroup loop
  * @{ */

/** @file
  * STREAMS Loop Driver loop(4) header file.  */

/** @name Loop Driver Magic Number
  * @{ */
#define	LOOP_IOCTL(n)	(('l' << 8) | (n))
/** @} */

/** @name Loop Driver Input-Output Control Commands
  * @{ */
#define LOOP_SET	LOOP_IOCTL(1)
#define LOOP_PUTNXT	LOOP_IOCTL(2)
#define	LOOP_MSGLVL	LOOP_IOCTL(3)
#define	LOOP_TIMR	LOOP_IOCTL(4)
#define	LOOP_MARK	LOOP_IOCTL(5)
#define	LOOP_GET_DEV	LOOP_IOCTL(6)
#define	LOOP_BUFCALL	LOOP_IOCTL(7)
#define	LOOP_CONCAT	LOOP_IOCTL(8)
#define	LOOP_COPY	LOOP_IOCTL(9)
#define	LOOP_DENY_OPEN	LOOP_IOCTL(10)	/**< Deny 2nd open of device. */
#define	LOOP_BURST	LOOP_IOCTL(11)	/**< Fwd next msg as a burst. */
#define	LOOP_FLUSH	LOOP_IOCTL(12)	/**< Flush and send data. */
/** @} */

/**
  * Loop parent structure.
  * The following structure is sent to loop via a TRANSPARENT ioctl.
  */
typedef struct {
	int cmnd;			/**< See defines below. */
	int i_arg;			/**< Integer argument for cmnd. */
	void *p_arg;			/**< Pointer argument for cmnd. */
} loop_xparent_t;

/** @name Loop Parent Commands
  * @{ */
#define	LOOP_XPARENT_COPYIN	101	/**< Undefined ioctl values == xparent. */
#define	LOOP_XPARENT_COPYOUT	102	/**< Undefined ioctl values == xparent. */
/** @} */

/** @} */

#endif				/* __SYS_LIS_LOOP_H__ */

// vim: ft=cpp com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
