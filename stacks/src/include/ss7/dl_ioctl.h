/*****************************************************************************

 @(#) $Id: dl_ioctl.h,v 0.9.2.5 2007/08/14 12:17:09 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:17:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl_ioctl.h,v $
 Revision 0.9.2.5  2007/08/14 12:17:09  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __DL_IOCTL_H__
#define __DL_IOCTL_H__

#ident "@(#) $RCSfile: dl_ioctl.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define DL_IOC_MAGIC 'd'

typedef struct dl_iochdr {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	dl_ulong cmd;			/* command or argument */
} dl_iochdr_t;

typedef struct dl_proto {
	dl_ulong pvar;
	dl_ulong popt;
} dl_proto_t;

/*
 *  Options
 */
#define DL_IOCGOPTIONS	 _IOR(	DL_IOC_MAGIC,	 0,	dl_iochdr_t	)
#define DL_IOCSOPTIONS	 _IOW(	DL_IOC_MAGIC,	 1,	dl_iochdr_t	)

/*
 *  Configuration
 */
#define DL_GET	    0
#define DL_ADD	    1
#define DL_CHA	    2
#define DL_DEL	    3

#define DL_IOCGCONFIG	_IOWR(	DL_IOC_MAGIC,	 2,	dl_iochdr_t	)
#define DL_IOCSCONFIG	_IOWR(	DL_IOC_MAGIC,	 3,	dl_iochdr_t	)
#define DL_IOCTCONFIG	_IOWR(	DL_IOC_MAGIC,	 4,	dl_iochdr_t	)
#define DL_IOCCCONFIG	_IOWR(	DL_IOC_MAGIC,	 5,	dl_iochdr_t	)

/*
 *  State
 */
#define DL_IOCGSTATEM	_IOWR(	DL_IOC_MAGIC,	 6,	dl_iochdr_t	)
#define DL_IOCCMRESET	_IOWR(	DL_IOC_MAGIC,	 7,	dl_iochdr_t	)

/*
 *  Statistics
 */
#define DL_IOCGSTATSP	_IOWR(	DL_IOC_MAGIC,	 8,	dl_iochdr_t	)
#define DL_IOCSSTATSP	_IOWR(	DL_IOC_MAGIC,	 9,	dl_iochdr_t	)
#define DL_IOCGSTATS	_IOWR(	DL_IOC_MAGIC,	10,	dl_iochdr_t	)
#define DL_IOCCSTATS	_IOWR(	DL_IOC_MAGIC,	11,	dl_iochdr_t	)

/*
 *  Notifications
 */
#define DL_IOCGNOTIFY	 _IOR(	DL_IOC_MAGIC,	12,	dl_iochdr_t	)
#define DL_IOCSNOTIFY	 _IOW(	DL_IOC_MAGIC,	13,	dl_iochdr_t	)
#define DL_IOCCNOTIFY	 _IOW(	DL_IOC_MAGIC,	14,	dl_iochdr_t	)

/*
 *  Management
 */
#define DL_IOCCMGMT	_IOWR(	DL_IOC_MAGIC,	15,	dl_iochdr_t	)

/*
 *  Pass-through IOCTLs
 */
#define DL_IOCCPASS	_IOWR(	DL_IOC_MAGIC,	16,	dl_iochdr_t	)

#define DL_IOC_FIRST	 0
#define DL_IOC_LAST	16
#define DL_IOC_PRIVATE	32

#endif				/* __DL_IOCTL_H__ */
