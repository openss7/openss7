/*****************************************************************************

 @(#) $Id: ll_control.h,v 1.1.2.3 2011-02-07 04:54:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-02-07 04:54:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ll_control.h,v $
 Revision 1.1.2.3  2011-02-07 04:54:43  brian
 - code updates for new distro support

 Revision 1.1.2.2  2010-11-28 14:21:53  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:26:50  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SNET_LL_CONTROL_H__
#define __SYS_SNET_LL_CONTROL_H__

#include <sys/types.h>
#include <stdint.h>

#define LAPB_STID	201
#define LLC_STID	202

#define OFF		 0
#define START		 1
#define D_CONN		 2
#define ADM		 3
#define ARM		 4
#define POLLING		 5
#define PRESETUP	 6
#define REGISTERING	 7
#define SETUP		 8
#define RESET		 9
#define LL_ERROR	10
#define NORMAL		11
#define ADM_CONN_REQ	12

#define L_SETSNID	('L'<<8 |  1)
#define L_GETSNID	('L'<<8 |  2)
#define L_SETTUNE	('L'<<8 |  3)
#define L_GETTUNE	('L'<<8 |  4)
#define L_GETSTATS	('L'<<8 |  5)
#define L_ZEROSTATS	('L'<<8 |  6)
#define L_TRACEON	('L'<<8 |  7)
#define L_TRACEOFF	('L'<<8 |  8)
#define L_GETGSTATS	('L'<<8 | 10)
#define L_ZEROGSTATS	('L'<<8 | 11)
#define L_LINKDISABLE	('L'<<8 | 12)
#define L_LINKENABLE	('L'<<8 | 13)
#define L_PUTX32MAP	('L'<<8 | 15)
#define L_GETX32MAP	('L'<<8 | 16)

#define LI_PLAIN	0x01
#define LI_SNID		0x02
#define LI_STATS	0x04
#define LI_GSTATS	0x16
#define LI_LAPBTUNE	0x13
#define LI_LLC2TUNE	0x23

#ifdef _SUN_SOURCE
#define LI_SPPA         LI_SNID
#define lli_ppa         lli_snid
#endif				/* _SUN_SOURCE */

struct ll_hdioc {
	uint8_t lli_type;
	uint8_t lli_spare[3];
	uint32_t lli_snid;
};

struct ll_sntioc {
	union {
		uint8_t lli_type;
		uint8_t lli_status;
	};
	uint8_t lli_spare[3];
	uint8_t lli_snid;

};

struct ll_snioc {
	uint8_t lli_type;
	uint8_t lli_class;
	uint8_t lli_spare[2];
	uint32_t lli_snid;
	uint32_t lli_index;
	uint32_t lli_slp_snid;
	uint32_t lli_slp_pri;
};

typedef struct lapbtune {
	uint16_t N2;			/* max numb retransmissions */
	uint16_t T1;			/* ack timer (deciseconds) */
	uint16_t Tpf;			/* poll timer (deciseconds) */
	uint16_t Trej;			/* reject timer (deciseconds) */
	uint16_t Tbusy;			/* busy timer (deciseconds) */
	uint16_t Tidle;			/* idle timer (deciseconds) */
	uint16_t ack_delay;		/* ack delay timer (deciseconds) */
	uint16_t notack_max;		/* max unacked I-frames */
	uint16_t tx_window;		/* transmit window (no XID) */
	uint16_t tx_probe;		/* window probe position */
	uint16_t max_I_len;		/* max I-frame size */
#define IGN_UA_ERROR		(1<<0)
#define FRMR_FRMR_ERROR		(1<<1)
#define FRMR_INVRSP_ERROR	(1<<2)
#define SFRAME_PBIT		(1<<3)
#define NO_DM_ADM		(1<<4)
#define IGN_DM_ERROR		(1<<5)
	uint16_t llconform;
	uint16_t sabm_in_x32;
} lapbtune_t;

struct lapb_tnioc {
	uint8_t lli_type;		/* LI_LAPBTUNE */
	uint8_t lli_spare[3];
	uint32_t lli_snid;
	struct lapbtune lapb_tune;
};

typedef struct llc2tune {
	uint16_t N2;			/* max numb retransmissions */
	uint16_t T1;			/* ack timer (deciseconds) */
	uint16_t Tpf;			/* poll timer (deciseconds) */
	uint16_t Trej;			/* reject timer (deciseconds) */
	uint16_t Tbusy;			/* busy timer (deciseconds) */
	uint16_t Tidle;			/* idle timer (deciseconds) */
	uint16_t ack_delay;		/* ack delay timer (deciseconds) */
	uint16_t notack_max;		/* max unacked I-frames */
	uint16_t tx_window;		/* transmit window (no XID) */
	uint16_t tx_probe;		/* window probe position */
	uint16_t max_I_len;		/* max I-frame size */
	uint16_t xid_window;		/* XID window size (receive window) */
	uint16_t xid_Ndup;		/* dup MAC XID count */
	uint16_t xid_Tdup;		/* dup MAC XID time (deciseconds) */
} llc2tune_t;

struct llc2_tnioc {
	uint8_t lli_type;		/* LI_LLC2TUNE */
	uint8_t lli_spare[3];
	uint32_t lli_snid;
	struct llc2tune llc2_tune;
};

#define tx_ign			 0
#define rx_badlen		 1
#define rx_unknown		 2
#define t1_exp			 3
#define t4_exp			 4
#define t4_n2_exp		 5
#define RR_rx_cmd		 6
#define RR_rx_rsp		 7
#define RR_tx_cmd		 8
#define RR_tx_rsp		 9
#define RR_tx_cmd_p		10
#define RNR_rx_cmd		11
#define RNR_rx_rsp		12
#define RNR_tx_cmd		13
#define RNR_tx_rsp		14
#define RNR_tx_cmd_p		15
#define REJ_rx_cmd		16
#define REJ_rx_rsp		17
#define REJ_tx_cmd		18
#define REJ_tx_rsp		19
#define REJ_tx_cmd_p		20
#define SABME_rx_cmd		21
#define SABME_tx_cmd		22
#define DISC_rx_cmd		23
#define DISC_tx_cmd		24
#define UA_rx_rsp		25
#define UA_tx_rsp		26
#define DM_rx_rsp		27
#define DM_tx_rsp		28
#define I_rx_cmd		29
#define I_tx_cmd		30
#define FRMR_rx_rsp		31
#define FRMR_tx_rsp		32
#define tx_rtr			33
#define rx_bad			34
#define rx_dud			35
#define rx_ign			36
#define XID_rx_cmd		37
#define XID_rx_rsp		38
#define XID_tx_cmd		39
#define XID_tx_rsp		40
#define TEST_rx_cmd		41
#define TEST_rx_rsp		42
#define TEST_tx_cmd		43
#define TEST_tx_rsp		44

#define SABM_rx_cmd		45
#define SABM_tx_cmd		46
#define SARM_rx_cmd		47
#define SARM_tx_cmd		48
#define lapbstatmax		49

typedef struct lapb_stats {
	uint32_t lapbmonarray[lapbstatmax];
} lapbstats_t;

struct lapb_stioc {
	uint8_t lli_type;
	uint8_t state;
	uint16_t lli_spare;
	uint32_t lli_snid;
	lapbstats_t lli_stats;
};

#define I_rx_rsp		45
#define I_tx_rsp		46
#define UI_rx_cmd		47
#define UI_tx_cmd		48
#define llc2statmax		49

typedef struct llc2_stats {
	uint32_t llc2monarray[llc2statmax];
} llc2stats_t;

struct llc2_stioc {
	uint8_t lli_type;
	uint8_t lli_spare[3];
	uint32_t lli_snid;
	llc2stats_t lli_stats;
};

#define frames_tx		 0
#define frames_rx		 1
#define sabm_tx			 2
#define sabm_rx			 3
#define sabme_tx		 2
#define sabme_rx		 3
#define bytes_tx		 4
#define bytes_rx		 5
#define globstatmax		 6

struct lapb_gstioc {
	uint8_t lli_type;
	uint8_t lli_spare[3];
	uint32_t lapbgstats[globstatmax];
};

struct llc2_gstioc {
	uint8_t lli_type;
	uint8_t lli_spare[3];
	uint32_t llc2gstats[globstatmax];
};

typedef union lli_union {
	struct ll_hdioc ll_hd;
	struct ll_snioc ll_sn;
	struct lapb_tnioc lapb_tn;
	struct lapb_stioc lapb_st;
	struct lapb_gstioc lapb_gst;
	struct llc2_tnioc llc2_tn;
	struct llc2_stioc llc2_st;
	struct llc2_gstioc llc2_gst;
} lliun_t;

#endif				/* __SYS_SNET_LL_CONTROL_H__ */
