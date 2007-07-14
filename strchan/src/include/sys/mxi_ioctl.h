/*****************************************************************************

 @(#) $Id: mxi_ioctl.h,v 0.9.2.4 2007/07/14 01:35:35 brian Exp $

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

 Last Modified $Date: 2007/07/14 01:35:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mxi_ioctl.h,v $
 Revision 0.9.2.4  2007/07/14 01:35:35  brian
 - make license explicit, add documentation

 Revision 0.9.2.3  2006/12/06 11:26:11  brian
 - current development updates

 Revision 0.9.2.2  2006/11/27 11:41:58  brian
 - updated CH and MX headers to interface version 1.1

 Revision 0.9.2.1  2006/10/14 06:37:28  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ifndef __SYS_MXI_IOCTL_H__
#define __SYS_MXI_IOCTL_H__

#ident "@(#) $RCSfile: mxi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2006 OpenSS7 Corporation"

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

#if 0
typedef struct mx_ifconfig {
	mx_ulong ifaddr;		/* ppa (card,span,channel) */
	volatile mx_ulong ifflags;	/* interface flags */
#define MX_IF_UP	    0x01
#define MX_IF_RX_RUNNING    0x02
#define MX_IF_TX_RUNNING    0x04

	mx_ulong iftype;		/* interface type */
#define MX_TYPE_NONE	    0
#define MX_TYPE_V35	    1
#define MX_TYPE_DS0	    2
#define MX_TYPE_DS0A	    3
#define MX_TYPE_E1	    4
#define MX_TYPE_T1	    5
#define MX_TYPE_ATM	    6
#define MX_TYPE_PACKET	    7

	mx_ulong ifrate;		/* interface rate */
	mx_ulong ifgtype;		/* interface group (span) type */
#define MX_GTYPE_NONE	    0
#define MX_GTYPE_T1	    1
#define MX_GTYPE_E1	    2
#define MX_GTYPE_J1	    3
#define MX_GTYPE_ATM	    4
#define MX_GTYPE_ETH	    5
#define MX_GTYPE_IP	    6
#define MX_GTYPE_UDP	    7
#define MX_GTYPE_TCP	    8
#define MX_GTYPE_RTP	    9
#define MX_GTYPE_SCTP	    10

	mx_ulong ifgrate;		/* interface group (span) rate */
	mx_ulong ifmode;		/* interface mode */
#define MX_MODE_NONE	    0
#define MX_MODE_DSU	    1
#define MX_MODE_CSU	    2
#define MX_MODE_DTE	    3
#define MX_MODE_DCE	    4
#define MX_MODE_CLIENT	    5
#define MX_MODE_SERVER	    6
#define MX_MODE_PEER	    7
#define MX_MODE_REM_LB	    8
#define MX_MODE_LOC_LB	    9
#define MX_MODE_LB_ECHO	    10
#define MX_MODE_TEST	    11

	mx_ulong ifgmode;		/* interface group (span) mode */
#define MX_GMODE_NONE	    0
#define MX_GMODE_LOC_LB	    1
#define MX_GMODE_REM_LB	    2

	mx_ulong ifgcrc;		/* interface group crc */
#define MX_GCRC_NONE	    0
#define MX_GCRC_CRC4	    1
#define MX_GCRC_CRC5	    2
#define MX_GCRC_CRC6	    3

	mx_ulong ifclock;		/* interface clock */
#define MX_CLOCK_NONE	    0
#define MX_CLOCK_INT	    1
#define MX_CLOCK_EXT	    2
#define MX_CLOCK_LOOP	    3
#define MX_CLOCK_MASTER	    4
#define MX_CLOCK_SLAVE	    5
#define MX_CLOCK_DPLL	    6
#define MX_CLOCK_ABR	    7
#define MX_CLOCK_SHAPER	    8
#define MX_CLOCK_TICK	    9

	mx_ulong ifcoding;
#define MX_CODING_NONE	    0
#define MX_CODING_NRZ	    1
#define MX_CODING_NRZI	    2
#define MX_CODING_AMI	    3
#define MX_CODING_B6ZS	    4
#define MX_CODING_B8ZS	    5
#define MX_CODING_ESF	    6
#define MX_CODING_AAL1	    7
#define MX_CODING_AAL2	    8
#define MX_CODING_AAL5	    9
#define MX_CODING_HDB3	    10

	mx_ulong ifframing;
#define MX_FRAMING_NONE	    0
#define MX_FRAMING_CCS	    1
#define MX_FRAMING_CAS	    2
#define MX_FRAMING_SF	    3
#define MX_FRAMING_D4	    MX_FRAMING_SF
#define MX_FRAMING_ESF	    4

	mx_ulong ifblksize;
	volatile mx_ulong ifleads;
#define MX_LEAD_DTR	    0x01
#define MX_LEAD_RTS	    0x02
#define MX_LEAD_DCD	    0x04
#define MX_LEAD_CTS	    0x08
#define MX_LEAD_DSR	    0x10

	mx_ulong ifbpv;
	mx_ulong ifalarms;
#define MX_ALARM_RED	    0x01
#define MX_ALARM_BLU	    0x02
#define MX_ALARM_YEL	    0x04
#define MX_ALARM_REC	    0x08

	mx_ulong ifrxlevel;
	mx_ulong iftxlevel;
#define MX_LEVEL_NONE	    0
#define MX_LEVEL_75OHM	    1
#define MX_LEVEL_100OHM	    2
#define MX_LEVEL_120OHM	    3
#define MX_LEVEL_LBO_1	    4
#define MX_LEVEL_LBO_2	    5
#define MX_LEVEL_LBO_3	    6
#define MX_LEVEL_LBO_4	    7
#define MX_LEVEL_LBO_5	    8
#define MX_LEVEL_LBO_6	    9

	mx_ulong ifsync;
	mx_ulong ifsyncsrc[MX_SYNCS];
} mx_ifconfig_t;
#endif

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
