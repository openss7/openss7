/*****************************************************************************

 @(#) $Id: hdlc_ioctl.h,v 0.9.2.4 2007/02/13 14:05:28 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/02/13 14:05:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: hdlc_ioctl.h,v $
 Revision 0.9.2.4  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __HDLC_IOCTL_H__
#define __HDLC_IOCTL_H__

#ident "@(#) $RCSfile: hdlc_ioctl.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define HDLC_IOC_MAGIC 'h'

typedef struct hdlc_timers {
} hdlc_timers_t;

typedef struct hdlc_option {
	struct lmi_option proto;
} hdlc_option_t;

#define HDLC_FLAGS_ONE		0
#define HDLC_FLAGS_SHARED	1
#define HDLC_FLAGS_TWO		2
#define HDLC_FLAGS_THREE	3

#define HDLC_IOCGOPTIONS	_IOR(	HDLC_IOC_MAGIC,	 0,	hdlc_option_t	)
#define HDLC_IOCSOPTIONS	_IOW(	HDLC_IOC_MAGIC,	 1,	hdlc_option_t	)

typedef struct hdlc_config {
	dl_ulong version;
	dl_ulong style;
	struct hdlc_timers timers;
	dl_ulong N;			/* number of octets if octet counting */
	dl_ulong m;			/* maximum SIF size */
	dl_ulong b;			/* transmit block size */
	dl_ulong f;			/* number of flags between frames */
} hdlc_config_t;

#define HDLC_IOCGCONFIG		_IOR(	HDLC_IOC_MAGIC,	 2,	hdlc_config_t	)
#define HDLC_IOCSCONFIG		_IOW(	HDLC_IOC_MAGIC,	 3,	hdlc_config_t	)
#define HDLC_IOCTCONFIG		_IOW(	HDLC_IOC_MAGIC,	 4,	hdlc_config_t	)
#define HDLC_IOCCCONFIG		_IOW(	HDLC_IOC_MAGIC,	 5,	hdlc_config_t	)

typedef struct hdlc_statem {
	struct hdlc_timers timers;
} hdlc_statem_t;

#define HDLC_IOCGSTATEM		_IOR(	HDLC_IOC_MAGIC,	 6,	hdlc_statem_t	)
#define HDLC_IOCCMRESET		_IOR(	HDLC_IOC_MAGIC,	 7,	hdlc_statem_t	)

typedef struct hdlc_stats {
	lmi_sta_t header;
	dl_ulong tx_bytes;
	dl_ulong tx_frames;
	dl_ulong tx_frames_repeated;
	dl_ulong tx_underruns;
	dl_ulong tx_aborts;
	dl_ulong tx_buffer_underruns;
	dl_ulong tx_frames_in_error;
	dl_ulong rx_bytes;
	dl_ulong rx_frames;
	dl_ulong rx_frames_compressed;
	dl_ulong rx_aborts;
	dl_ulong rx_buffer_overflows;
	dl_ulong rx_frames_in_error;
	dl_ulong rx_sync_transitions;
	dl_ulong rx_bits_octet_counted;
	dl_ulong rx_crc_errors;
	dl_ulong rx_frame_errors;
	dl_ulong rx_frame_overflows;
	dl_ulong rx_frame_too_long;
	dl_ulong rx_frame_too_short;
	dl_ulong rx_residue_errors;
	dl_ulong rx_length_error;	/* error in length indicator */
	dl_ulong carrier_cts_lost;
	dl_ulong carrier_dcd_lost;
	dl_ulong carrier_lost;
} hdlc_stats_t;

#define HDLC_IOCGSTATSP		_IOR(	HDLC_IOC_MAGIC,	 8,	hdlc_stats_t	)
#define HDLC_IOCSSTATSP		_IOW(	HDLC_IOC_MAGIC,	 9,	hdlc_stats_t	)
#define HDLC_IOCGSTATS		_IOR(	HDLC_IOC_MAGIC,	10,	hdlc_stats_t	)
#define HDLC_IOCCSTATS		_IOW(	HDLC_IOC_MAGIC,	11,	hdlc_stats_t	)

typedef struct hdlc_notify {
	dl_ulong events;
} hdlc_notify_t;

#define HDLC_IOCGNOTIFY		_IOR(	HDLC_IOC_MAGIC,	12,	hdlc_notify_t	)
#define HDLC_IOCSNOTIFY		_IOW(	HDLC_IOC_MAGIC,	13,	hdlc_notify_t	)
#define HDLC_IOCCNOTIFY		_IOW(	HDLC_IOC_MAGIC,	14,	hdlc_notify_t	)

typedef struct hdlc_mgmt {
	dl_ulong cmd;
} hdlc_mgmt_t;

#define HDLC_ABORT		1

#define HDLC_IOCCMGMT		_IOW(	HDLC_IOC_MAGIC,	15,	dl_ulong	)

#define HDLC_IOC_FIRST		 0
#define HDLC_IOC_LAST		15
#define HDLC_IOC_PRIVATE	32

#endif				/* __HDLC_IOCTL_H__ */
