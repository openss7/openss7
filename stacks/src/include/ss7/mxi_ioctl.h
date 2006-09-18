/*****************************************************************************

 @(#) $Id: mxi_ioctl.h,v 0.9.2.2 2005/05/14 08:30:45 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:45 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_MXI_IOCTL_H__
#define __SS7_MXI_IOCTL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define MX_IOC_MAGIC	'c'

/*
 *  CONFIGURATION
 */
typedef struct mx_config {
	ulong block_size;		/* data block size (bits) */
	ulong encoding;			/* encoding */
	ulong sample_size;		/* sample size (bits) */
	ulong rate;			/* clock rate (Hz) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
	ulong opt_flags;		/* options flags */
} mx_config_t;

#define MX_IOCGCONFIG	_IOR(	MX_IOC_MAGIC,	2,  mx_config_t	    )
#define MX_IOCSCONFIG	_IOWR(	MX_IOC_MAGIC,	3,  mx_config_t	    )
#define MX_IOCTCONFIG	_IOWR(	MX_IOC_MAGIC,	4,  mx_config_t	    )
#define MX_IOCCCONFIG	_IOR(	MX_IOC_MAGIC,	5,  mx_config_t	    )

/*
 *  STATE
 */

typedef struct mx_statem {
	ulong state;
	ulong flags;
} mx_statem_t;

#define	MX_IOCGSTATEM	_IOR(	MX_IOC_MAGIC,	6,  mx_statem_t	    )
#define	MX_IOCCMRESET	_IOR(	MX_IOC_MAGIC,	7,  mx_statem_t	    )

/*
 *  STATISTICS
 */

typedef struct mx_stats {
	ulong header;
} mx_stats_t;

#define	MX_IOCGSTATSP	_IOR(	MX_IOC_MAGIC,	 8, mx_stats_t	    )
#define	MX_IOCSSTATSP	_IOWR(	MX_IOC_MAGIC,	 9, mx_stats_t	    )
#define	MX_IOCGSTATS	_IOR(	MX_IOC_MAGIC,	10, mx_stats_t	    )
#define	MX_IOCCSTATS	_IOW(	MX_IOC_MAGIC,	11, mx_stats_t	    )

/*
 *  EVENTS
 */

typedef struct mx_notify {
	ulong events;
} mx_notify_t;

#define	MX_IOCGNOTIFY	_IOR(	MX_IOC_MAGIC,	12, mx_notify_t	    )
#define	MX_IOCSNOTIFY	_IOW(	MX_IOC_MAGIC,	13, mx_notify_t	    )
#define	MX_IOCCNOTIFY	_IOW(	MX_IOC_MAGIC,	14, mx_notify_t	    )

typedef struct mx_mgmt {
	ulong cmd;
} mx_mgmt_t;

#define MX_MGMT_RESET		1

#define	MX_IOCCMGMT	_IOW(	MX_IOC_MAGIC,	15, mx_mgmt_t	    )

#define MX_IOC_FIRST	 0
#define MX_IOC_LAST	15
#define MX_IOC_PRIVATE	32

#endif				/* __SS7_MXI_IOCTL_H__ */
