/*****************************************************************************

 @(#) $Id: chi_ioctl.h,v 0.9 2004/01/17 08:08:11 brian Exp $

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

 Last Modified $Date: 2004/01/17 08:08:11 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_CHI_IOCTL_H__
#define __SS7_CHI_IOCTL_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

#include <linux/ioctl.h>

#define CH_IOC_MAGIC	'c'

/*
 *  CONFIGURATION
 */
typedef struct ch_config {
	ch_ulong block_size;		/* data block size (bits) */
	ch_ulong encoding;		/* encoding */
	ch_ulong sample_size;		/* sample size (bits) */
	ch_ulong rate;			/* clock rate (Hz) */
	ch_ulong tx_channels;		/* number of tx channels */
	ch_ulong rx_channels;		/* number of rx channels */
	ch_ulong opt_flags;		/* options flags */
} ch_config_t;

#define CH_IOCGCONFIG	_IOR(	CH_IOC_MAGIC,	2,  ch_config_t	    )
#define CH_IOCSCONFIG	_IOWR(	CH_IOC_MAGIC,	3,  ch_config_t	    )
#define CH_IOCTCONFIG	_IOWR(	CH_IOC_MAGIC,	4,  ch_config_t	    )
#define CH_IOCCCONFIG	_IOR(	CH_IOC_MAGIC,	5,  ch_config_t	    )

/*
 *  STATE
 */

typedef struct ch_statem {
	ch_ulong state;
	ch_ulong flags;
} ch_statem_t;

#define	CH_IOCGSTATEM	_IOR(	CH_IOC_MAGIC,	6,  ch_statem_t	    )
#define	CH_IOCCMRESET	_IOR(	CH_IOC_MAGIC,	7,  ch_statem_t	    )

/*
 *  STATISTICS
 */

typedef struct ch_stats {
	ch_ulong header;
} ch_stats_t;

#define	CH_IOCGSTATSP	_IOR(	CH_IOC_MAGIC,	 8, ch_stats_t	    )
#define	CH_IOCSSTATSP	_IOWR(	CH_IOC_MAGIC,	 9, ch_stats_t	    )
#define	CH_IOCGSTATS	_IOR(	CH_IOC_MAGIC,	10, ch_stats_t	    )
#define	CH_IOCCSTATS	_IOW(	CH_IOC_MAGIC,	11, ch_stats_t	    )

/*
 *  EVENTS
 */

typedef struct ch_notify {
	ch_ulong events;
} ch_notify_t;

#define	CH_IOCGNOTIFY	_IOR(	CH_IOC_MAGIC,	12, ch_notify_t	    )
#define	CH_IOCSNOTIFY	_IOW(	CH_IOC_MAGIC,	13, ch_notify_t	    )
#define	CH_IOCCNOTIFY	_IOW(	CH_IOC_MAGIC,	14, ch_notify_t	    )

#define CH_IOC_FIRST	 0
#define CH_IOC_LAST	14
#define CH_IOC_PRIVATE	32

#endif				/* __SS7_CHI_IOCTL_H__ */
