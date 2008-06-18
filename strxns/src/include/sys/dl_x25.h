/*****************************************************************************

 @(#) $Id: dl_x25.h,v 0.9.2.3 2008-06-18 16:45:40 brian Exp $

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

 Last Modified $Date: 2008-06-18 16:45:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl_x25.h,v $
 Revision 0.9.2.3  2008-06-18 16:45:40  brian
 - widespread updates

 Revision 0.9.2.2  2008-04-25 11:39:32  brian
 - updates to AGPLv3

 Revision 0.9.2.1  2007/12/15 20:16:50  brian
 - added new mib files

 *****************************************************************************/

#ifndef __SYS_DL_X25_H__
#define __SYS_DL_X25_H__

#ident "@(#) $RCSfile: dl_x25.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* These tend to depend a lot on whether it is Solstice X.25, SGI IRIS, AIXlink/X.25, HP-UX
 * X.25/9000 or ther Spider NLI implementations. */

#if 1
#define LI_STATS	 1
#define LI_PLAIN	 2
#define LI_GSTATS	 3
#define LI_SPPA		 4
#define LI_LLC2TUNE	 5
#else
#define LI_PLAIN	 1
#define LI_SNID		 2
#define LI_STATS	 4
#define LI_GSTATS	22
#define LI_LAPBTUNE	19
#define LI_LLC2TUNE	35
#endif

#define tx_ign		 0	/**< Number of ignored and not sent. */
#define rx_badlen	 1	/**< Number of received bad length frames. */
#define rx_unknown	 2	/**< Number of received unknown frames. */
#define t1_exp		 3	/**< Number of T1 timeouts. */
#define t4_exp		 4	/**< Number of T4 timeouts. */
#define t4_n2_exp	 5	/**< Number of T4 timeouts after N2 tries. */

#define RR_rx_cmd	 6	/**< Number of received RR commands. */
#define RR_rx_rsp	 7	/**< Number of received RR responses. */
#define RR_tx_cmd	 8	/**< Number of sent RR commands. */
#define RR_tx_rsp	 9	/**< Number of sent RR responses. */
#define RR_tx_cmd_p	10	/**< Number of sent RR responses P-bit set. */

#define RNR_rx_cmd	11	/**< Number of received RNR commands. */
#define RNR_rx_rsp	12	/**< Number of received RNR responses. */
#define RNR_tx_cmd	13	/**< Number of sent RNR commands. */
#define RNR_tx_rsp	14	/**< Number of sent RNR responses. */
#define RNR_tx_rsp_p	15	/**< Number of sent RNR responses P-bit set. */

#define REJ_rx_cmd	16	/**< Number of received REJ commands. */
#define REJ_rx_rsp	17	/**< Number of received REJ responses. */
#define REJ_tx_cmd	18	/**< Number of sent REJ commands. */
#define REJ_tx_rsp	19	/**< Number of sent REJ responses. */
#define REJ_tx_rsp_p	20	/**< Number of sent REJ responses P-bit set. */

#define SABME_rx_cmd	21	/**< Number of received SABME commands. */
#define SABME_tx_cmd	22	/**< Number of sent SABME commands. */

#define DISC_rx_cmd	23	/**< Number of received DISC commands. */
#define DISC_tx_cmd	24	/**< Number of sent DISC commands. */

#define UA_rx_rsp	25	/**< Number of received UA responses. */
#define UA_tx_rsp	26	/**< Number of sent UA responses. */

#define DM_rx_rsp	27	/**< Number of received DM responses. */
#define DM_tx_rsp	28	/**< Number of sent DM responses. */

#define I_rx_cmd	29	/**< Number of received I-frame commands. */
#define I_tx_cmd	30	/**< Number of sent I-frame commands. */

#define FRMR_rx_rsp	31	/**< Number of received FRMR responses. */
#define FRMR_tx_rsp	32	/**< Number of sent FRMR responses. */

#define tx_rtr		33	/**< Number of retransmitted frames. */
#define rx_bad		34	/**< Number of received erroneous frames. */
#define rx_dud		35	/**< Number of received and discarded frames. */
#define rx_ign		36	/**< Number of received and ignored frames. */

#define I_rx_rsp	37	/**< Number of received I-frame responses. */
#define I_tx_rsp	38	/**< Number of sent I-frame responses. */

#define UI_rx_cmd	39	/**< Number of received UI frames. */
#define UI_tx_cmd	40	/**< Number of sent UI frames. */

#define XID_rx_cmd	41	/**< Number of received XID commands. */
#define XID_rx_rsp	42	/**< Number of received XID responses. */
#define XID_tx_cmd	43	/**< Number of sent XID commands. */
#define XID_tx_rsp	44	/**< Number of sent XID responses. */

#define TEST_rx_cmd	45	/**< Number of received TEST commands. */
#define TEST_rx_rsp	46	/**< Number of received TEST responses. */
#define TEST_tx_cmd	47	/**< Number of sent TEST commands. */
#define TEST_tx_rsp	48	/**< Number of sent TEST responses. */

#define llc2statmax	49

#define SABM_rx_cmd	37	/**< Number of received SABM commands. */
#define SABM_tx_cmd	38	/**< Number of sent SABM commands. */
#define SARM_rx_cmd	39	/**< Number of received SARM commands. */
#define SARM_tx_cmd	40	/**< Number of sent SARM commands. */

#define lapbstatmax	49

/* LLC2 statistics array */
typedef struct llc2_stats {
	uint32_t llc2monarray[llc2statmax];
} llc2stats_t;

/* LAPB statistics array */
typedef struct lapb_stats {
	uint32_t lapbmonarray[lapbstatmax];
} lapbstats_t;

/* LLC2 L_GETSTATS structure */
struct llc2_strioc {
	uint8_t lli_type;	/**< The table type.  Always LI_STATS. */
	uint8_t lli_spare[3];	/**< Reserved field set to zero. */
	uint32_t lli_ppa;	/**< The Physical Point of Attachment (PPA). */
	llc2stats_t lli_stats;	/**< The statistics table. */
};

/* LAPB L_GETSTATS structure */
struct lapb_strioc {
	uint8_t lli_type;	/**< The table type.  Always LI_STATS. */
	uint8_t lli_spare[3];	/**< Reserved field set to zero. */
	uint32_t lli_ppa;	/**< The Physical Point of Attachment (PPA). */
	lapbstats_t lli_stats;	/**< The statistics table. */
};

struct ll_hdioc {
	uint8_t lli_type;	/**< The table type.  Always LI_PLAIN. */
	uint8_t lli_spare[3];	/**< Reserved for alignment. */
	uint32_t lli_ppa;	/**< The Physical Point of Attachment (PPA).  Set this value to
				     0xff to zero statistics for all links. */
};

#define frames_tx	0	/**< Frames transmitted. */
#define frames_rx	1	/**< Frames received. */
#define sabm_tx		2	/**< SABMs transmitted. */
#define sabm_rx		3	/**< SABMs received. */
#define sabme_tx	2	/**< SABMEs transmitted. */
#define sabme_rx	3	/**< SABMEs received. */
#define bytes_tx	4	/**< Data bytes transmitted. */
#define bytes_rx	5	/**< Data bytes received. */
#define gobstatmax	6	/**< The size of the global statistics table. */

/* LLC2 L_GETGSTATS structure */
struct llc2_gstioc {
	uint8_t lli_type;	/**< The table type. Always LI_GSTATS. */
	uint8_t lli_spare[3];	/**< Reserved for alignment. */
	uint32_t llc2gstats[globstatmax];	/**< Global stats table. */
};

/* LAPB L_GETGSTATS structure */
struct lapb_gstioc {
	uint8_t lli_type;	/**< The table type. Always LI_GSTATS. */
	uint8_t lli_spare[3];	/**< Reserved for alignment. */
	uint32_t lapbgstats[globstatmax];	/**< Global stats table. */
};

/* Link Class (lli_class) values: */
#define LC_LLC1			15	/**< LLC Type 1 */
#define LC_LLC2			16	/**< LLC Type 2 */
#define LC_LAPBDTE		17	/**< LAPB - DTE */
#define LC_LAPBXDTE		18	/**< LAPB - DTE extended */
#define LC_LAPBDCE		19	/**< LAPB - DCE */
#define LC_LAPBXDCE		20	/**< LAPB - DCE extended */
#define LC_LAPDTE		21	/**< LAP - DTE */
#define LC_LAPDCE		22	/**< LAP - DCE */
#define LC_HDLC			27	/**< HDLC */
#define LC_HDLCX		28	/**< HDLC extended */

#if 1
struct ll_snioc {
	uint8_t lli_type;	/**< The table type.  Always LI_SPPA. */
	uint8_t lli_class;	/**< DTE/DCE/extended.  This indicates the type of link. LC_LLC2
				     must be used for LLC2. LC_LAPBDTE or LC_LAPBDCE must be used
				     for LAPB.  The file /usr/include/netdlc/ll_proto.h contains a
				     complete list of values. */
	uint16_t lli_spl_pri;	/**< SLP priority when used in an MLP multiplexer.  This determines
				     the priority of SLP when MLP is used. */
	uint32_t lli_ppa;	/**< Physical Point of Attachment (PPA) and Subnetwork ID character. */
	uint32_t lli_index;	/**< Link index under the MLP multiplexing driver.  This must be set
				     with the muxid value returned by the I_LINK(7) ioctl when LAPB
				     is placed over a serial driver. */
};
#else
struct ll_snioc {
	uint8_t lli_type;	/**< The table type.  Always LI_SNID. */
	uint8_t lli_class;	/**< DTE/DCE/extended.  This indicates the type of link. LC_LLC2
				     must be used for LLC2.  LC_LAPBDTE or LC_LAPBDCE must be used
				     for LAPB.  The file /usr/include/netdlc/ll_proto.h contains a
				     complete list of values. */
	uint8_t lli_spare[2];	/**< Reserved for alignment. */
	uint32_t lli_snid;	/**< Subnetwork ID character. */
	uint32_t lli_index;	/**< Link index under the MLP multiplexing driver.  This must be set
				     with the muxid value returned by the I_LINK(7) ioctl when LAPB
				     is placed over a serial driver. */
	uint32_t lli_slp_snid;	/**< Subnetwork identifier for SLP. */
	uint32_t lli_slp_pri;	/**< SLP priorirty when used in an MLP multiplexer.  This determines
				     the priority of the SL when MLP is used. */
};
#endif

typedef struct llc2_tune {
	uint16_t N2;		/**< Maximum number of retries.  Values between 1 and 255, default 
				     10. */
	uint16_t T1;		/**< Acknowledgement time (units of 0.1 seconds).  Values between
				     10 and 3000 dS, default 90. */
	uint16_t Tpf;		/**< P/F cycle retry time (units of 0.1 seconds).  Values between
				     10 and 2000 dS, default 7. */
	uint16_t Trej;		/**< Reject retry time (units of 0.1 seconds) */
	uint16_t Tbusy;		/**< Remote busy check time (units of 0.1 seconds). Values
				     between 10 and 30000 dS, default 100. */
	uint16_t Tidle;		/**< Idle P/F cycle time (units of 0.1 seconds).  Values betwen 0
				     and 32000 dS, default 250. */
	uint16_t ack_delay;	/**< RR delay time (units of 0.1 seconds).  Values between 0 and
				     3000 dS, default 4. */
	uint16_t notack_max;	/**< Maximum number of unacknowledged received I-frames. Values
				     between 0 and 127, default 3. */
	uint16_t tx_window;	/**< Transmit window (if no XID received). */
	uint16_t tx_probe;	/**< P-bit position before end of transmit window. Values between
				     0 and 127, default 0. */
	uint16_t max_I_len;	/**< Maximum I-frame length. */
	uint16_t xid_window;	/**< XID window size (receive window). Values between 1 and 127,
				     default 7. */
	uint16_t xid_Ndup;	/**< Duplicate MAC XID count (zero for no test).  Values between 0 
				     and 255, default 2. */
	uint16_t xid_Tdup;	/**< Duplication MAX XID time (units of 0.1 seconds). Values
				     between 0 and 3000 dS, default 50. */
} llc2tune_t;

/* LAPB conformance (llconform) values. */
#define IGN_UA_ERROR		(1UL<<0)	/**< UA frames ignored in ERROR state. */
#define FRMR_FRMR_ERROR		(1UL<<1)	/**< FRMR is retransmitted in ERROR state. */
#define FRMR_INVRSP_ERROR	(1UL<<2)	/**< Invalid response frames get FRMR. */
#define SFRAME_PBIT		(1UL<<3)	/**< S-frame commands must have P-bit set. */
#define NO_DM_ADM		(1UL<<4)	/**< Do not send DM on entry to ADM state. */
#define IGN_DM_ERROR		(1UL<<5)	/**< DM frames ignored in ERROR state. */

typedef struct lapb_tune {
	uint16_t N2;		/**< Maximum number of retries. */
	uint16_t T1;		/**< Acknowledgement time (units of 0.1 seconds) */
	uint16_t Tpf;		/**< P/F cycle retry time (units of 0.1 seconds) */
	uint16_t Trej;		/**< Reject retry time (units of 0.1 seconds) */
	uint16_t Tbusy;		/**< Remote busy check time (units of 0.1 seconds) */
	uint16_t Tidle;		/**< Idle P/F cycle time (units of 0.1 seconds) */
	uint16_t ack_delay;	/**< RR dleay time (units of 0.1 seconds) */
	uint16_t notack_max;	/**< Maximum number of unacknowledged received I-frames. */
	uint16_t tx_window;	/**< Transmit window size (if no XID received). */
	uint16_t tx_probe;	/**< P-bit position before end of transmit window. */
	uint16_t max_I_len;	/**< Maximum I-frame length. */
	uint16_t llconform;	/**< LAPB conformance. */
	uint16_t sabm_i_x32;	/**< Action when SABM received in X.32 setup. */
} lapbtune_t;

/* LLC2 L_SETTUNE or L_GETTUNE structure. */
struct llc2_tnioc {
	uint8_t lli_type;	/**< The table type.  Always LI_LLC2TUNE. */
	uint8_t lli_spare[3];	/**< Reserved for alignment. */
	uint32_t lli_ppa;	/**< The Physical Point of Attachment (PPA). */
	llc2tune_t llc2_tune;	/**< Table of tuning values. */
};

/* LAPB L_SETTUNE or L_GETTUNE structure. */
struct lapb_tnioc {
	uint8_t lli_type;	/**< The table type.  Always LI_LAPBTUNE. */
	uint8_t lli_spare[3];	/**< Reserved for alignment. */
	uint32_t lli_ppa;	/**< The Physical Point of Attachment (PPA). */
	lapbtune_t lapb_tune;	/**< Table of tuning values. */
};

#ifdef _SUN_SOURCE

#define LLIOC		('L'<<8)

#define L_SETPPA	(LLIOC|0x01)	/**< Set PPA. */
#define L_GETPPA	(LLIOC|0x02)	/**< Get PPA. */
#define L_SETTUNE	(LLIOC|0x03)	/**< Set tuning parameters. */
#define L_GETTUNE	(LLIOC|0x04)	/**< Get tuning parameters. */
#define L_GETSTATS	(LLIOC|0x05)	/**< Get statistics. */
#define L_ZEROSTATS	(LLIOC|0x06)	/**< Zero statistics. */
#define L_TRACEON	(LLIOC|0x07)	/**< Set message tracing on. */
#define L_TRACEOFF	(LLIOC|0x08)	/**< Set message tracing off. */

#define L_GETGSTATS	(LLIOC|0x0a)	/**< Get global statistics. */
#define L_ZEROGSTATS	(LLIOC|0x0b)	/**< Zero global statistics. */
#define L_LINKDISABLE	(LLIOC|0x0c)	/**< Disable a link. */
#define L_LINKENABLE	(LLIOC|0x0d)	/**< Enable a link. */

#define WIOC		('W'<<8)

#if 0
#define W_ZEROSTATS	(WIOC|0x01)
#define W_GETSTATS	(WIOC|0x02)
#endif
#define W_SETTUNE	(WIOC|0x03)	/**< Set WAN tuning parameters. */
#define W_GETTUNE	(WIOC|0x04)	/**< Get WAN tuning parameters. */
#if 0
#define W_PUTWANMAP	(WIOC|0x05)
#define W_GETWANMAP	(WIOC|0x06)
#define W_DELWANMAP	(WIOC|0x07)
#define W_STATUS	(WIOC|0x08)
#define W_ENABLE	(WIOC|0x09)
#define W_DISABLE	(WIOC|0x0a)
#endif

#else				/* _SUN_SOURCE */

#define LLIOC		('L'<<8)

#define L_SETSNID	(LLIOC|0x01)	/**< Set subnetwork identifier. */
#define L_GETSNID	(LLIOC|0x02)	/**< Get subnetwork identifier. */
#define L_SETTUNE	(LLIOC|0x03)	/**< Set tuning parameters. */
#define L_GETTUNE	(LLIOC|0x04)	/**< Get tuning parameters. */
#define L_GETSTATS	(LLIOC|0x05)	/**< Get statistics. */
#define L_ZEROSTATS	(LLIOC|0x06)	/**< Zero statistics. */
#define L_TRACEON	(LLIOC|0x07)	/**< Set message tracing on. */
#define L_TRACEOFF	(LLIOC|0x08)	/**< Set message tracing off. */

#define L_GETGSTATS	(LLIOC|0x0a)	/**< Get global statistics. */
#define L_ZEROGSTATS	(LLIOC|0x0b)	/**< Zero global statistics. */
#define L_LINKDISABLE	(LLIOC|0x0c)	/**< Disable a link. */
#define L_LINKENABLE	(LLIOC|0x0d)	/**< Enable a link. */

#define L_PUTX32MAP	(LLIOC|0x0f)	/**< Put X.32 table entries. */
#define L_GETX32MAP	(LLIOC|0x10)	/**< Get X.32 table entries. */

#endif				/* _SUN_SOURCE */

#endif				/* __SYS_DL_X25_H__ */
