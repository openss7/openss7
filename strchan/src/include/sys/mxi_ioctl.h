/*****************************************************************************

 @(#) $Id: mxi_ioctl.h,v 0.9.2.2 2006/11/27 11:41:58 brian Exp $

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

 Last Modified $Date: 2006/11/27 11:41:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mxi_ioctl.h,v $
 Revision 0.9.2.2  2006/11/27 11:41:58  brian
 - updated CH and MX headers to interface version 1.1

 Revision 0.9.2.1  2006/10/14 06:37:28  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ifndef __SYS_MXI_IOCTL_H__
#define __SYS_MXI_IOCTL_H__

#ident "@(#) $RCSfile: mxi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define MX_IOC_MAGIC	'c'

/*
 *  CONFIGURATION
 */
typedef struct mx_config {
	mx_ulong type;			/* unused */
	mx_ulong encoding;		/* encoding */
	mx_ulong block_size;		/* data block size (bits) */
	mx_ulong samples;		/* samples per block */
	mx_ulong sample_size;		/* sample size (bits) */
	mx_ulong rate;			/* clock rate (samples/second) */
	mx_ulong tx_channels;		/* number of tx channels */
	mx_ulong rx_channels;		/* number of rx channels */
	mx_ulong opt_flags;		/* options flags */
} mx_config_t;

#define MX_IOCGCONFIG	_IOR(	MX_IOC_MAGIC,	2,  mx_config_t	    )
#define MX_IOCSCONFIG	_IOWR(	MX_IOC_MAGIC,	3,  mx_config_t	    )
#define MX_IOCTCONFIG	_IOWR(	MX_IOC_MAGIC,	4,  mx_config_t	    )
#define MX_IOCCCONFIG	_IOR(	MX_IOC_MAGIC,	5,  mx_config_t	    )

/*
 *  STATE
 */

typedef struct mx_statem {
	mx_ulong state;
	mx_ulong flags;
} mx_statem_t;

#define	MX_IOCGSTATEM	_IOR(	MX_IOC_MAGIC,	6,  mx_statem_t	    )
#define	MX_IOCCMRESET	_IOR(	MX_IOC_MAGIC,	7,  mx_statem_t	    )

/*
 *  STATISTICS
 */

typedef struct mx_stats {
	mx_ulong header;
} mx_stats_t;

#define	MX_IOCGSTATSP	_IOR(	MX_IOC_MAGIC,	 8, mx_stats_t	    )
#define	MX_IOCSSTATSP	_IOWR(	MX_IOC_MAGIC,	 9, mx_stats_t	    )
#define	MX_IOCGSTATS	_IOR(	MX_IOC_MAGIC,	10, mx_stats_t	    )
#define	MX_IOCCSTATS	_IOW(	MX_IOC_MAGIC,	11, mx_stats_t	    )

/*
 *  EVENTS
 */

typedef struct mx_notify {
	mx_ulong events;
} mx_notify_t;

#define	MX_IOCGNOTIFY	_IOR(	MX_IOC_MAGIC,	12, mx_notify_t	    )
#define	MX_IOCSNOTIFY	_IOW(	MX_IOC_MAGIC,	13, mx_notify_t	    )
#define	MX_IOCCNOTIFY	_IOW(	MX_IOC_MAGIC,	14, mx_notify_t	    )

typedef struct mx_mgmt {
	mx_ulong cmd;
} mx_mgmt_t;

#define MX_MGMT_RESET		1

#define	MX_IOCCMGMT	_IOW(	MX_IOC_MAGIC,	15, mx_mgmt_t	    )

#define MX_IOC_FIRST	 0
#define MX_IOC_LAST	15
#define MX_IOC_PRIVATE	32

#endif				/* __SYS_MXI_IOCTL_H__ */
