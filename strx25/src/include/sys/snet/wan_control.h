/*****************************************************************************

 @(#) $Id: wan_control.h,v 0.9.2.2 2008-07-01 12:31:08 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-07-01 12:31:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: wan_control.h,v $
 Revision 0.9.2.2  2008-07-01 12:31:08  brian
 - updated man pages, drafts, specs, header files

 Revision 0.9.2.1  2008/06/18 16:43:14  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_SNET_WAN_CONTROL_H__
#define __SYS_SNET_WAN_CONTROL_H__

#ident "@(#) $RCSfile: wan_control.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* values for w_type field */
#define WAN_STATS	1	/* wan_stioc structure */
#define WAN_TUNE	2	/* wan_tnioc structure */
#define WAN_MAP		3	/* wan_mpioc union */
#define WAN_PLAIN	4	/* wan_hdioc structure */
#define WAN_SETSIG	5	/* wan_setsigf structure */

/* WAN_STATS structures */

typedef struct hstats {
	uint32_t hc_txgood;		/* good frames transmitted */
	uint32_t hc_txurun;		/* transmitter underruns */
	uint32_t hc_rxgood;		/* good frames received */
	uint32_t hc_rxorun;		/* receiver overruns */
	uint32_t hc_rxcrc;		/* CRC or framing errors */
	uint32_t hc_rxnobuf;		/* no receive buffer */
	uint32_t hc_rxnflow;		/* frame received no flow control */
	uint32_t hc_rxoflow;		/* buffer overflows */
	uint32_t hc_rxabort;		/* received aborts */
	uint32_t hc_intframes;		/* tranmission failures */
} hdlcstats_t;

/* values for w_state field */
#define HDLC_IDLE	0
#define HDLC_ESTB	1
#define HDLC_DISABLED	2
#define HDLC_CONN	3
#define HDLC_DISC	4

struct wan_stioc {
	uint8_t w_type;			/* always WAN_STATS */
	uint8_t w_state;		/* HDLC state */
	uint8_t w_spare[2];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
	hdlcstats_t hdlc_stats;		/* statistics */
};

/* WAN_TUNE structures */

/* values for WAN_cptype field */
#define WAN_NONE	0
#define WAN_X21P	1
#define WAN_V25bis	2

/* for a description of timers and defaults, see X.21 Annex C DTE Timers */
struct WAN_x21 {
	uint16_t WAN_cptype;		/* Always WAN_X21P. */
	uint16_t T1;			/* X.21 T1: call-request to proceed-to-select timer */
	uint16_t T2;			/* X.21 T2: end-of-selection to ready-for-data timer */
	uint16_t T3A;			/* X.21 T3A: addtn'l call prog or DCE provided info timer */
	uint16_t T4B;			/* X.21 T4B: call-accept to ready-for-data timer */
	uint16_t T5;			/* X.21 T5: DTE clear-request to DCE ready timer */
	uint16_t T6;			/* X.21 T6: DTE clear-confirmation to DCE ready timer */
};

/* default values for the WAN_x21 structure */
const struct WAN_x21 WAN_x21_defaults = {
	.WAN_cptype = WAN_X21P,		/* Always WAN_X21P */
	.T1 = 30,			/* 3.0 seconds */
	.T2 = 200,			/* 20.0 seconds */
	.T3A = 60,			/* 6.0 seconds */
	.T4B = 60,			/* 6.0 seconds */
	.T5 = 20,			/* 2.0 seconds */
	.T6 = 20,			/* 2.0 seconds */
};

/* for a description of timers and defaults, see V.25 bis Clause 5.2 */
struct WAN_v25 {
	uint16_t WAN_cptype;		/* Always WAN_V25bis. */
	uint16_t callreq;		/* V.25 bis T1: call init. to call estab. timer */
};

/* default values for the WAN_v25 structure */
const struct WAN_v25 WAN_v25_defaults = {
	.WAN_cptype = WAN_V25bis,	/* Always WAN_X21P */
	.callreq = 6000,		/* 600.0 seconds, 5 minutes */
};

/* values for WAN_interface field */
#define WAN_X21		0
#define WAN_V28		1
#define WAN_V35		2

/* values for WAN_phy_if field */
#define WAN_DTE		0
#define WAN_DCE		1

struct WAN_hddef {
	uint16_t WAN_maxframe;		/* WAN maximum frame size (octets). */
	uint32_t WAN_baud;		/* WAN baud rate. */
	uint16_t WAN_interface;		/* WAN physical interface. */
	union {
		uint16_t WAN_cptype;	/* significant when WAN_NONE */
		struct WAN_x21 WAN_x21def;	/* significant when WAN_X21P */
		struct WAN_v25 WAN_v25def;	/* significant when WAN_V25bis */
	} WAN_cpdef;			/* call procedure definitions */
};

/* values for WAN_options field */
#define TRANSLATE	0x0001

typedef struct wantune {
	uint16_t WAN_options;
	struct WAN_hddef WAN_hd;
} wantune_t;

struct wan_tnioc {
	uint8_t w_type;			/* always WAN_TUNE */
	uint8_t w_spare[3];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
	wantune_t wan_tune;		/* WAN tunables */
};

/* WAN_MAP structures */

typedef struct wanmap {
	uint8_t remsize;		/* remote address size in octets */
	uint8_t remaddr[20];		/* remote address containing remsize octets */
	uint8_t infsize;		/* interface address size in octets */
	uint8_t infaddr[30];		/* interface address containing infsize octets */
} wanmap_t;

typedef struct wanget {
	uint16_t first_ent;		/* index of first entry in entries member */
	uint16_t num_ent;		/* number of entries in entries member */
	wanmap_t entries[1];
	/* followed by (num_ent - 1) * sizeof(wanmap_t) entry buffer */
} wanget_t;

/* used with W_GETWANMAP */
struct wanmapgf {
	uint8_t w_type;			/* always WAN_MAP */
	uint8_t w_spare[3];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
	wanget_t wan_ents;		/* block of mapping entries */
};

/* used with W_PUTWANMAP */
struct wanmappf {
	uint8_t w_type;			/* always WAN_MAP */
	uint8_t w_spare[3];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
	wanmap_t wan_ent;		/* single mapping entry */
};

/* used with W_DELWANMAP */
struct wanmapdf {
	uint8_t w_type;			/* always WAN_MAP */
	uint8_t w_spare[3];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
};

union wan_mpioc {
	uint8_t w_type;			/* always WAN_MAP */
	struct wanmapgf wan_getmap;	/* W_GETWANMAP structure */
	struct wanmappf wan_putmap;	/* W_PUTWANMAP structure */
	struct wanmapdf wan_delmap;	/* W_DELWANMAP structure */
};

/* WAN_PLAIN structures */

struct wan_hdioc {
	uint8_t w_type;			/* always WAN_PLAIN */
	uint8_t w_spare[3];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
};

/* WAN_SETSIG structures */

/* definitions for w_ctrlsignal field */
#define W_RTS_HIGH	(1<<0)	/* set or indicate RTS high (X.21 C) */
#define W_DTR_HIGH	(1<<1)	/* set or indicate DTR high */
#define W_DCD_HIGH	(1<<2)	/* indicate DCD high */
#define W_DSR_HIGH	(1<<3)	/* indicate DSR high */
#define W_CTS_HIGH	(1<<4)	/* indicate CTS high (X.21 I) */
#define W_RI_HIGH	(1<<5)	/* indicate RI high */
#define W_RTS_LOW	(1<<6)	/* set RTS low */
#define W_DTR_LOW	(1<<7)	/* set DTR low */

typedef struct wan_setsig {
	uint8_t w_ctrlsignal;
	uint8_t w_reserved[3];
} wan_setsig_t;

struct wan_setsigf {
	uint8_t w_type;			/* always WAN_SETSIG */
	uint8_t w_spare[3];		/* spare bytes for alignment */
	uint32_t w_snid;		/* subnetwork identifier */
	wan_setsig_t wan_setsig;	/* signals and leads set */
};

#define W_ZEROSTATS	(('L'<<8)|000)	/* zero statistics */
#define W_GETSTATS	(('L'<<8)|001)	/* get statistics */
#define W_SETTUNE	(('L'<<8)|002)	/* set tunables */
#define W_GETTUNE	(('L'<<8)|003)	/* get tunables */
#define W_PUTWANMAP	(('L'<<8)|004)	/* put address mapping */
#define W_GETWANMAP	(('L'<<8)|005)	/* get address mappings */
#define W_DELWANMAP	(('L'<<8)|006)	/* del address mappings */
#define W_STATUS	(('L'<<8)|007)	/* get interface status */
#define W_ENABLE	(('L'<<8)|010)	/* enable interface */
#define W_DISABLE	(('L'<<8)|011)	/* disable interface */
#define W_SETSIG	(('L'<<8)|012)	/* set signals and leads */
#define W_GETSIG	(('L'<<8)|013)	/* get signals and leads */
#define W_POLLSIG	(('L'<<8)|014)	/* poll signals and leads */

#endif				/* __SYS_SNET_WAN_CONTROL_H__ */
