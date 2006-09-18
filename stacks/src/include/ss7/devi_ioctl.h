/*****************************************************************************

 @(#) $Id: devi_ioctl.h,v 0.9.2.2 2005/05/14 08:30:44 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:44 $ by $Author: brian $

 *****************************************************************************/

#ifndef __DEVI_IOCTL_H__
#define __DEVI_IOCTL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define DEV_IOC_MAGIC 'v'

/*
 *  DEVICE CONFIGURATION
 */

#define DEV_SYNCS 4

struct sdl;

#ifdef __KERNEL__

typedef struct dev_device {
	spinlock_t iflock;		/* interface lock */
	char *ifname;			/* interface name */
	dev_ulong ifflags;		/* interface flags */
	dev_ulong iftype;		/* interface type */
	dev_ulong ifgtype;		/* group type */
	dev_ulong ifgcrc;		/* group CRC type */
	dev_ulong ifmode;		/* interface mode */
	dev_ulong ifrate;		/* interface rate */
	dev_ulong ifclock;		/* interface clock */
	dev_ulong ifcoding;		/* interface coding */
	dev_ulong ifframing;		/* interface framing */
	dev_ulong ifleads;		/* interface leads */
	dev_ulong ifindex;		/* interface index */
	dev_ulong ifrxlevel;		/* interface rxlevel */
	dev_ulong iftxlevel;		/* interface txlevel */
	dev_ulong ifalarms;		/* interface alarms (E1/T1) */
	dev_ulong ifsyncsrc[DEV_SYNCS];	/* interface sync src (E1/T1) */
	dev_ulong irq;			/* interrupt */
	dev_ulong iobase;		/* i/o base address */
	dev_ulong dma_rx;		/* dma receive */
	dev_ulong dma_tx;		/* dma transmit */
} dev_device_t;

#endif

#define DEV_IOCCIFRESET		_IO(  DEV_IOC_MAGIC,  0 )

#define DEV_IOCGIFFLAGS		_IOW( DEV_IOC_MAGIC,  1, dev_ulong )
#define DEV_IOCSIFFLAGS		_IOW( DEV_IOC_MAGIC,  2, dev_ulong )

#define DEV_IF_UP		0x00000001	/* device should set on enable */
#define DEV_IF_RX_RUNNING	0x00000002	/* when Rx running */
#define DEV_IF_TX_RUNNING	0x00000004	/* when Tx running */
#define DEV_IF_SU_COMPRESS	0x00000008	/* when Rx compressing */

#define DEV_IOCGIFTYPE		_IOR( DEV_IOC_MAGIC,  3, dev_ulong )
#define DEV_IOCSIFTYPE		_IOW( DEV_IOC_MAGIC,  4, dev_ulong )

enum {
	DEV_TYPE_NONE,
	DEV_TYPE_V35,
	DEV_TYPE_DS0,
	DEV_TYPE_DS0A,
	DEV_TYPE_E1,
	DEV_TYPE_T1,
	DEV_TYPE_J1,
	DEV_TYPE_ATM,
	DEV_TYPE_PACKET
};

#define DEV_IOCGGRPTYPE		 _IOR( DEV_IOC_MAGIC,  5, dev_ulong )
#define DEV_IOCSGRPTYPE		 _IOW( DEV_IOC_MAGIC,  6, dev_ulong )

enum {
	DEV_GTYPE_NONE,
	DEV_GTYPE_T1,
	DEV_GTYPE_E1,
	DEV_GTYPE_J1,
	DEV_GTYPE_ATM,
	DEV_GTYPE_ETH,
	DEV_GTYPE_IP,
	DEV_GTYPE_UDP,
	DEV_GTYPE_TCP,
	DEV_GTYPE_RTP,
	DEV_GTYPE_SCTP
};

#define DEV_IOCGIFMODE		 _IOR( DEV_IOC_MAGIC,  7, dev_ulong )
#define DEV_IOCSIFMODE		 _IOW( DEV_IOC_MAGIC,  8, dev_ulong )

enum {
	DEV_MODE_NONE,
	DEV_MODE_DSU,
	DEV_MODE_CSU,
	DEV_MODE_DTE,
	DEV_MODE_DCE,
	DEV_MODE_CLIENT,
	DEV_MODE_SERVER,
	DEV_MODE_PEER,
	DEV_MODE_ECHO,
	DEV_MODE_REM_LB,
	DEV_MODE_LOC_LB,
	DEV_MODE_LB_ECHO,
	DEV_MODE_TEST
};

#define DEV_IOCGIFRATE		 _IOR( DEV_IOC_MAGIC,  9, dev_ulong )
#define DEV_IOCSIFRATE		 _IOW( DEV_IOC_MAGIC, 10, dev_ulong )

#define DEV_IOCGIFCLOCK		 _IOR( DEV_IOC_MAGIC, 11, dev_ulong )
#define DEV_IOCSIFCLOCK		 _IOW( DEV_IOC_MAGIC, 12, dev_ulong )

enum {
	DEV_CLOCK_NONE,
	DEV_CLOCK_INT,
	DEV_CLOCK_EXT,
	DEV_CLOCK_LOOP,
	DEV_CLOCK_MASTER,
	DEV_CLOCK_SLAVE,
	DEV_CLOCK_DPLL,
	DEV_CLOCK_ABR,
	DEV_CLOCK_SHAPER,
	DEV_CLOCK_TICK
};

#define DEV_IOCGIFCODING	 _IOR( DEV_IOC_MAGIC, 13, dev_ulong )
#define DEV_IOCSIFCODING	 _IOW( DEV_IOC_MAGIC, 14, dev_ulong )

enum {
	DEV_CODING_NONE,
	DEV_CODING_NRZ,
	DEV_CODING_NRZI,
	DEV_CODING_AMI,
	DEV_CODING_B6ZS,
	DEV_CODING_B8ZS,
	DEV_CODING_ESF,			/* FIXME: this is actually a framing value */
	DEV_CODING_AAL1,
	DEV_CODING_AAL2,
	DEV_CODING_AAL5,
	DEV_CODING_HDB3
};

#define DEV_IOCGIFLEADS		 _IOR( DEV_IOC_MAGIC, 15, dev_ulong )
#define DEV_IOCSIFLEADS		 _IOW( DEV_IOC_MAGIC, 16, dev_ulong )
#define DEV_IOCCIFLEADS		 _IOW( DEV_IOC_MAGIC, 17, dev_ulong )

enum {
	DEV_LEAD_DTR = 0x01,		/* for V.35 DTR lead set or clr */
	DEV_LEAD_RTS = 0x02,		/* for V.35 RTS lead set or clr */
	DEV_LEAD_DCD = 0x04,		/* for V.35 DCD lead get */
	DEV_LEAD_CTS = 0x08,		/* for V.35 CTS lead get */
	DEV_LEAD_DSR = 0x10		/* for V.35 DSR lead get */
};

#define DEV_IOCGIFALARMS    DEV_IOCGIFLEADS
#define DEV_IOCSIFALARMS    DEV_IOCSIFLEADS
#define DEV_IOCCIFALARMS    DEV_IOCCIFLEADS

enum {
	DEV_ALARM_RED = 0x01,		/* for E1/T1 Red Alarm */
	DEV_ALARM_BLU = 0x02,		/* for E1/T1 Blue Alarm */
	DEV_ALARM_YEL = 0x04,		/* for E1/T1 Yellow Alarm */
	DEV_ALARM_REC = 0x08		/* for E1/T1 Alarm Recovery */
};

#define DEV_IOCCDISCTX		 _IO(  DEV_IOC_MAGIC, 18 )
#define DEV_IOCCCONNTX		 _IO(  DEV_IOC_MAGIC, 19 )

#define DEV_IOCGIFFRAMING	 _IOR( DEV_IOC_MAGIC, 20, dev_ulong )
#define DEV_IOCSIFFRAMING	 _IOW( DEV_IOC_MAGIC, 21, dev_ulong )

enum {
	DEV_FRAMING_NONE,
	DEV_FRAMING_CCS,
	DEV_FRAMING_CAS,
	DEV_FRAMING_SF,
	DEV_FRAMING_ESF
};

#define DEV_IOCGIFGCRC		 _IOR( DEV_IOC_MAGIC, 22, dev_ulong )
#define DEV_IOCSIFGCRC		 _IOW( DEV_IOC_MAGIC, 23, dev_ulong )

enum {
	DEV_GCRC_NONE,
	DEV_GCRC_CRC4,
	DEV_GCRC_CRC5,
	DEV_GCRC_CRC6
};

#define DEV_IOCGIFLEVEL		 _IOR( DEV_IOC_MAGIC, 24, dev_ulong )
#define DEV_IOCSIFLEVEL		 _IOW( DEV_IOC_MAGIC, 25, dev_ulong )

#define DEV_IOCGIFSYNCSRC	 _IOR( DEV_IOC_MAGIC, 26, dev_ulong[DEV_SYNCS] )
#define DEV_IOCSIFSYNCSRC	 _IOR( DEV_IOC_MAGIC, 27, dev_ulong[DEV_SYNCS] )

#define DEV_IOC_FIRST    0
#define DEV_IOC_LAST    27
#define DEV_IOC_PRIVATE 32

#endif				/* __DEVI_IOCTL_H__ */
