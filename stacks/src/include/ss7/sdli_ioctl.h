/*****************************************************************************

 @(#) $Id: sdli_ioctl.h,v 0.9.2.8 2006/12/08 05:32:08 brian Exp $

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

 Last Modified $Date: 2006/12/08 05:32:08 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SDLI_IOCTL_H__
#define __SDLI_IOCTL_H__

#ident "@(#) $RCSfile: sdli_ioctl.h,v $ $Name:  $($Revision: 0.9.2.8 $) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define	SDL_IOC_MAGIC	'd'

/*
 *  PROTOCOL VARIANT AND OPTIONS
 */

#define	SDL_IOCGOPTIONS	_IOR(	SDL_IOC_MAGIC,	 0,	lmi_option_t	)
#define	SDL_IOCSOPTIONS	_IOW(	SDL_IOC_MAGIC,	 1,	lmi_option_t	)

/*
 *  CONFIGURATION
 */

#ifdef __KERNEL__
#ifdef _MSP_SOURCE
typedef struct sdl_timers {
	mblk_t *t9;			/* T9 timer */
} sdl_timers_t;
#else				/* _MSP_SOURCE */
typedef struct sdl_timers {
	toid_t t9;			/* T9 timer */
} sdl_timers_t;
#endif				/* _MSP_SOURCE */
#endif				/* __KERNEL__ */

#define SDL_SYNCS 4

typedef struct sdl_config {
	const char *ifname;		/* interface name */
	volatile sdl_ulong ifflags;	/* interface flags */
#	define	SDL_IF_UP		0x01	/* device enabled */
#	define	SDL_IF_RX_RUNNING	0x02	/* Rx running */
#	define	SDL_IF_TX_RUNNING	0x04	/* Tx running */
	sdl_ulong iftype;		/* interface type */
#	define	SDL_TYPE_NONE		0	/* unknown/unspecified */
#	define	SDL_TYPE_V35		1	/* V.35 interface */
#	define	SDL_TYPE_DS0		2	/* DS0 channel */
#	define	SDL_TYPE_DS0A		3	/* DS0A channel */
#	define	SDL_TYPE_E1		4	/* full E1 span */
#	define	SDL_TYPE_T1		5	/* full T1 span */
#	define	SDL_TYPE_J1		6	/* full J1 span */
#	define	SDL_TYPE_ATM		7	/* ATM SSCF */
#	define	SDL_TYPE_PACKET		8	/* other packet */
	sdl_ulong ifrate;		/* interface rate */
#	define	SDL_RATE_NONE		0
#	define	SDL_RATE_DS0A		56000
#	define	SDL_RATE_DS0		64000
#	define	SDL_RATE_T1		1544000
#	define	SDL_RATE_J1		1544000
#	define	SDL_RATE_E1		2048000
	sdl_ulong ifgtype;		/* group type */
#	define	SDL_GTYPE_NONE		0	/* */
#	define	SDL_GTYPE_T1		1	/* */
#	define	SDL_GTYPE_E1		2	/* */
#	define	SDL_GTYPE_J1		3	/* */
#	define	SDL_GTYPE_ATM		4	/* */
#	define	SDL_GTYPE_ETH		5	/* */
#	define	SDL_GTYPE_IP		6	/* */
#	define	SDL_GTYPE_UDP		7	/* */
#	define	SDL_GTYPE_TCP		8	/* */
#	define	SDL_GTYPE_RTP		9	/* */
#	define	SDL_GTYPE_SCTP		10	/* */
	sdl_ulong ifgrate;		/* interface group rate */
#	define	SDL_GRATE_NONE		0
#	define	SDL_GRATE_T1		1544000
#	define	SDL_GRATE_J1		1544000
#	define	SDL_GRATE_E1		2048000
	sdl_ulong ifmode;		/* interface mode */
#	define	SDL_MODE_NONE		0	/* */
#	define	SDL_MODE_DSU		1	/* */
#	define	SDL_MODE_CSU		2	/* */
#	define	SDL_MODE_DTE		3	/* */
#	define	SDL_MODE_DCE		4	/* */
#	define	SDL_MODE_CLIENT		5	/* */
#	define	SDL_MODE_SERVER		6	/* */
#	define	SDL_MODE_PEER		7	/* */
#	define	SDL_MODE_ECHO		8	/* */
#	define	SDL_MODE_REM_LB		9	/* */
#	define	SDL_MODE_LOC_LB		10	/* */
#	define	SDL_MODE_LB_ECHO	11	/* */
#	define	SDL_MODE_TEST		12	/* */
	sdl_ulong ifgmode;		/* interface group mode */
#	define	SDL_GMODE_NONE		0	/* no loopback */
#	define	SDL_GMODE_LOC_LB	1	/* loopback locally asserted */
#	define	SDL_GMODE_REM_LB	2	/* loopback remotely asserted */
	sdl_ulong ifgcrc;		/* group CRC type */
#	define	SDL_GCRC_NONE		0	/* */
#	define	SDL_GCRC_CRC4		1	/* */
#	define	SDL_GCRC_CRC5		2	/* */
#	define	SDL_GCRC_CRC6		3	/* */
#	define	SDL_GCRC_CRC6J		4	/* */
	sdl_ulong ifclock;		/* interface clock */
#	define	SDL_CLOCK_NONE		0	/* */
#	define	SDL_CLOCK_INT		1	/* */
#	define	SDL_CLOCK_EXT		2	/* */
#	define	SDL_CLOCK_LOOP		3	/* */
#	define	SDL_CLOCK_MASTER	4	/* */
#	define	SDL_CLOCK_SLAVE		5	/* */
#	define	SDL_CLOCK_DPLL		6	/* */
#	define	SDL_CLOCK_ABR		7	/* */
#	define	SDL_CLOCK_SHAPER	8	/* */
#	define	SDL_CLOCK_TICK		9	/* */
	sdl_ulong ifcoding;		/* interface coding */
#	define	SDL_CODING_NONE		0	/* */
#	define	SDL_CODING_NRZ		1	/* */
#	define	SDL_CODING_NRZI		2	/* */
#	define	SDL_CODING_AMI		3	/* */
#	define	SDL_CODING_B6ZS		4	/* */
#	define	SDL_CODING_B8ZS		5	/* */
#	define	SDL_CODING_AAL1		7	/* */
#	define	SDL_CODING_AAL2		8	/* */
#	define	SDL_CODING_AAL5		9	/* */
#	define	SDL_CODING_HDB3		10	/* */
	sdl_ulong ifframing;		/* interface framing */
#	define	SDL_FRAMING_NONE	0	/* */
#	define	SDL_FRAMING_CCS		1	/* */
#	define	SDL_FRAMING_CAS		2	/* */
#	define	SDL_FRAMING_SF		3	/* */
#	define	SDL_FRAMING_ESF		4	/* */
#	define	SDL_FRAMING_D4		SDL_FRAMING_SF
	sdl_ulong ifblksize;		/* interface block size */
	volatile sdl_ulong ifleads;	/* interface leads */
#	define	SDL_LEAD_DTR		0x01	/* for V.35 DTR lead set or clr */
#	define	SDL_LEAD_RTS		0x02	/* for V.35 RTS lead set or clr */
#	define	SDL_LEAD_DCD		0x04	/* for V.35 DCD lead get */
#	define	SDL_LEAD_CTS		0x08	/* for V.35 CTS lead get */
#	define	SDL_LEAD_DSR		0x10	/* for V.35 DSR lead get */
	volatile sdl_ulong ifbpv;	/* bipolar violations (E1/T1) */
	volatile sdl_ulong ifalarms;	/* interface alarms (E1/T1) */
#	define	SDL_ALARM_RED		0x01	/* for E1/T1 Red Alarm */
#	define	SDL_ALARM_BLU		0x02	/* for E1/T1 Blue Alarm */
#	define	SDL_ALARM_YEL		0x04	/* for E1/T1 Yellow Alarm */
#	define	SDL_ALARM_REC		0x08	/* for E1/T1 Alarm Recovery */
	volatile sdl_ulong ifrxlevel;	/* interface rxlevel */
	volatile sdl_ulong iftxlevel;	/* interface txlevel */
#	define	SDL_TXLEVEL_NONE	0
#	define	SDL_TXLEVEL_DSX_133FT	1
#	define	SDL_TXLEVEL_DSX_266FT	2
#	define	SDL_TXLEVEL_DSX_399FT	3
#	define	SDL_TXLEVEL_DSX_533FT	4
#	define	SDL_TXLEVEL_DSX_666FT	5
#	define	SDL_TXLEVEL_CSU_0DB	1
#	define	SDL_TXLEVEL_CSU_8DB	6
#	define	SDL_TXLEVEL_CSU_15DB	7
#	define	SDL_TXLEVEL_CSU_23DB	8
#	define	SDL_TXLEVEL_75OHM_NM	1
#	define	SDL_TXLEVEL_120OHM_NM	2
#	define	SDL_TXLEVEL_75OHM_PR	3
#	define	SDL_TXLEVEL_120OHM_PR	4
#	define	SDL_TXLEVEL_75OHM_HRL	5
#	define	SDL_TXLEVEL_120OHM_HRL	6
#	define	SDL_TXLEVEL_MON_0DB	9
#	define	SDL_TXLEVEL_MON_12DB	10
#	define	SDL_TXLEVEL_MON_20DB	11
#	define	SDL_TXLEVEL_MON_26DB	11
#	define	SDL_TXLEVEL_MON_30DB	11
#	define	SDL_TXLEVEL_MON_32DB	12
	volatile sdl_ulong ifsync;	/* current interface sync src (E1/T1) */
	sdl_ulong ifsyncsrc[SDL_SYNCS];	/* interface sync src (E1/T1) */
} sdl_config_t;

#define	SDL_IOCGCONFIG	_IOR(	SDL_IOC_MAGIC,	 2,	sdl_config_t	)
#define	SDL_IOCSCONFIG	_IOWR(	SDL_IOC_MAGIC,	 3,	sdl_config_t	)
#define	SDL_IOCTCONFIG	_IOWR(	SDL_IOC_MAGIC,	 4,	sdl_config_t	)
#define	SDL_IOCCCONFIG	_IOR(	SDL_IOC_MAGIC,	 5,	sdl_config_t	)

/*
 *  STATE
 */

typedef struct sdl_statem {
	volatile sdl_ulong tx_state;
	volatile sdl_ulong rx_state;
} sdl_statem_t;

#define	SDL_STATE_IDLE		0x00
#define	SDL_STATE_IN_SERVICE	0x01
#define	SDL_STATE_CONGESTED	0x02

#define	SDL_IOCGSTATEM	_IOR(	SDL_IOC_MAGIC,	6,	sdl_statem_t	)
#define	SDL_IOCCMRESET	_IOR(	SDL_IOC_MAGIC,	7,	sdl_statem_t	)

/*
 *  STATISTICS
 */

typedef struct sdl_stats {
	lmi_ulong header;
	sdl_ulong rx_octets;
	sdl_ulong tx_octets;
	sdl_ulong rx_overruns;
	sdl_ulong tx_underruns;
	sdl_ulong rx_buffer_overflows;
	sdl_ulong tx_buffer_overflows;
	sdl_ulong lead_cts_lost;
	sdl_ulong lead_dcd_lost;
	sdl_ulong carrier_lost;
} sdl_stats_t;

#define	SDL_IOCGSTATSP	_IOR(	SDL_IOC_MAGIC,	 8,	sdl_stats_t	)
#define	SDL_IOCSSTATSP	_IOWR(	SDL_IOC_MAGIC,	 9,	sdl_stats_t	)
#define	SDL_IOCGSTATS	_IOR(	SDL_IOC_MAGIC,	10,	sdl_stats_t	)
#define	SDL_IOCCSTATS	_IOW(	SDL_IOC_MAGIC,	11,	sdl_stats_t	)

/*
 *  EVENTS
 */
typedef struct sdl_notify {
	sdl_ulong events;
#	define	SDL_EVT_LOST_SYNC	0x0000001
#	define	SDL_EVT_SU_ERROR	0x0000002
#	define	SDL_EVT_TX_FAIL		0x0000004
#	define	SDL_EVT_RX_FAIL		0x0000008
} sdl_notify_t;

#define	SDL_IOCGNOTIFY	_IOR(	SDL_IOC_MAGIC,	12,	sdl_notify_t	)
#define	SDL_IOCSNOTIFY	_IOW(	SDL_IOC_MAGIC,	13,	sdl_notify_t	)
#define	SDL_IOCCNOTIFY	_IOW(	SDL_IOC_MAGIC,	14,	sdl_notify_t	)

#define	SDL_IOCCDISCTX	_IO(	SDL_IOC_MAGIC,	15	)
#define	SDL_IOCCCONNTX	_IO(	SDL_IOC_MAGIC,	16	)

#define	SDL_IOC_FIRST	 0
#define	SDL_IOC_LAST	16
#define	SDL_IOC_PRIVATE	32

#endif				/* __SDLI_IOCTL_H__ */
