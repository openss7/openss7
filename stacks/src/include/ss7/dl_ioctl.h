/*****************************************************************************

 @(#) $Id: dl_ioctl.h,v 0.9.2.1 2004/08/21 10:14:39 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/21 10:14:39 $ by $Author: brian $

 *****************************************************************************/

#ifndef __DL_IOCTL_H__
#define __DL_IOCTL_H__

#define DL_IOC_MAGIC 'd'

typedef struct dl_iochdr {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* command or argument */
} dl_iochdr_t;

typedef struct dl_proto {
	ulong pvar;
	ulong popt;
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
